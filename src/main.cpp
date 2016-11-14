/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "stm32f1xx.h"

#include <math.h>
#include "Radio.h"
#include "Voltmeter.h"
#include "ModelRegistry.h"
#include "Telemetry.h"
#include "Controller.h"
#include "PermanentStore.h"
#include "Audio.h"
#include "Trim.h"
#include "Led.h"
#include "WindowedAverage.h"

#ifdef ENABLE_ALARM
#include "Alarm.h"
#endif

// This file has 3 parts. First the definition of all components + assignments
// of pins/ports/adc etc. After that in begin() all the setup is done. Finally
// loop() is the main event loop.

Audio audio(TIM2, TIM_CHANNEL_4, GPIOA, GPIO_PIN_3);

// Next we configure the volt meters we need. There are two (one per battery)
ADC_HandleTypeDef vmeterAdcHandle;
GPIO_TypeDef *vmeterPorts[2] = {GPIOA, GPIOA};
uint32_t vmeterPins[2] = {GPIO_PIN_1, GPIO_PIN_2};
uint32_t vmeterChannels[2] = {ADC_CHANNEL_1, ADC_CHANNEL_2};
IRQn_Type irq = ADC1_IRQn;

long r1s[2] = {0,1};
long r2s[2] = {0,1};

Voltmeter vmeter(ADC1, &vmeterAdcHandle, vmeterPorts, vmeterPins,
    vmeterChannels, irq, r1s, r2s, 2);
// ---------- Voltmeter

Telemetry telemetry;

#ifdef ENABLE_ALARM
// Low battery alarms next
Alarm alarm((Audio*)&audio);
#endif

ADC_HandleTypeDef controllerAdcHandle;
uint32_t controllerChannels[4] = {ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6};
GPIO_TypeDef *potPorts[4] = {GPIOA, GPIOA, GPIOA, GPIOA};
uint32_t potPins[4] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6};
GPIO_TypeDef *switchPorts[4] = {GPIOC, GPIOC, GPIOC, GPIOC};
uint32_t switchPins[4] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};

Controller controller(ADC1, &controllerAdcHandle, controllerChannels, irq,
    potPorts, potPins, switchPorts, switchPins);

// Initializing the trim buttons. There are 8 in total.
GPIO_TypeDef *trimHighPorts[4] = {GPIOD,GPIOD, GPIOD, GPIOD};
uint32_t trimHighPins[4] = {GPIO_PIN_0,GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3};

GPIO_TypeDef *trimLowPorts[4] = {GPIOD,GPIOD, GPIOD, GPIOD};
uint32_t trimLowPins[4] = {GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_7};

Trim trim(
    trimLowPorts, trimLowPins,
    trimHighPorts, trimHighPins,
    (Audio*)&audio, 4);
// --------------- Trim


// Initialize the LED
GPIO_TypeDef *rgbPorts[3] = {GPIOB, GPIOB, GPIOB};
uint32_t rbgPins[3] = {GPIO_PIN_6,GPIO_PIN_7,GPIO_PIN_8};
uint32_t timerChannels[3] = {TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3};
Led led(TIM4, timerChannels, rgbPorts, rbgPins);
// --------------- LED

// The radio pin is used to pulse out the rc signal.
Radio radio((Audio*)&audio, TIM3, TIM_CHANNEL_4, GPIOB, GPIO_PIN_1);

ModelRegistry registry;

PermanentStore store;

unsigned long lastM = 0;
unsigned int iterations = 0;

uint8_t volts[4] = {};
uint8_t signals[1] = {};

void setModel(Model*, int16_t*);

#define DELAY 10
#define ITERATIONS(ms) ((ms) / DELAY)

#define THRESHOLD (MAX_LEVEL * 7 / 10)

static __IO uint32_t currentTime;

#define HISTORY (3)
static WindowedAverage window(HISTORY);

void setup() {

  led.begin();
  led.setColor(0,255,0);

  // initializing the controller will start reading the sticks
  controller.begin();

  // next let's initialize the radio. this will start sending pulses
  radio.begin();

  trim.begin();

  registry.begin();
  setModel(registry.current(), controller.getInputs());

  vmeter.begin();

  telemetry.begin();

#ifdef ENABLE_ALARM
  alarm.begin();
#endif

  led.setMode(Led::FLOAT);
}

void loop() {
  if (iterations % ITERATIONS(1000) == 0) {
    volts[0] = vmeter.getVoltage(0);
    volts[1] = vmeter.getVoltage(1);
    vmeter.update();

    telemetry.update();

    volts[2] = telemetry.getA1();
    volts[3] = telemetry.getA2();
    signals[0] = telemetry.getRssi();
  }

#ifdef ENABLE_ALARM
  if (iterations % ITERATIONS(300) == 0) {
    alarm.update(currentTime, volts,
        telemetry.aquired() ? 4 : 2, signals,
        telemetry.aquired() ? 1 : 0);

    if (alarm.hasSignalAlarm() || alarm.hasVoltAlarm()) {
      led.setMode(Led::BLINK);
      led.setColor(255,0,0);
    } else {
      led.setMode(Led::FLOAT);
    }
  }
#endif

  controller.update(currentTime);

  radio.update(controller.getInputs());

  trim.update(currentTime);

  led.update(currentTime);
}

void setModel(Model *m, int16_t *inputs) {
  store.load(m);

  radio.setModel(m);

  trim.setTrim(m->trim);

#ifdef ENABLE_ALARM
  alarm.setMinVolts(m->minVolts);
  alarm.setMinSignals(m->minSignals);
#endif
}

static void SystemClockConfig(void)
{

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  // enable clocks
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_TIM2_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_TIM4_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_ADC1_CLK_ENABLE();

  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __TIM2_CLK_ENABLE();
  __TIM3_CLK_ENABLE();
  __TIM4_CLK_ENABLE();
  __ADC1_CLK_ENABLE();
  __DMA1_CLK_ENABLE();

  //__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  //RCC_OscInitStruct.PLL.PLLM = 8;
  //RCC_OscInitStruct.PLL.PLLN = 288;
  //RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  //RCC_OscInitStruct.PLL.PLLQ = 6;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
  SystemCoreClockUpdate();
}

int main(void) {
  uint32_t start = 0;

  // init hal first
  HAL_Init();
  SystemClockConfig();

  // init components
  setup();

  for(;;) {

    currentTime = HAL_GetTick();

    // try to maintain stable rate of updates
    if (DELAY > window.get()) {
      HAL_Delay(DELAY-window.get());
    } else {
      led.setColor(255,0,0);
    }

    start = currentTime;

    // run the main loop
    loop();

    ++iterations;

    if (currentTime > start) {
      window.add(currentTime - start);
    }
  }
}


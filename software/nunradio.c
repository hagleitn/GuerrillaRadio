#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include <math.h>
#include "Radio.h"
#include "Voltmeter.h"
#include "ModelRegistry.h"
#include "Telemetry.h"
#include "Controller.h"
#include "PermanentStore.h"
#include "Trim.h"
#include "Led.h"
#include "WindowedAverage.h"

#ifdef ENABLE_ALARM
#include "Alarm.h"
#endif

Voltmeter vmeter1(A1, 0l, 1l);
Voltmeter vmeter2(A2, 0l, 1l);

Telemetry telemetry;

#ifdef ENABLE_ALARM
Alarm alarm(13);
#endif

uint8_t potPins[4] = {A3,A4,A5,A6};
uint8_t switchPins[4] = {2,3,4,5};

Controller controller(potPins, switchPins);

uint8_t trimHighPins[4] = {6,7,8,9};
uint8_t trimLowPins[4] = {10,11,12,14};

Trim trim(trimLowPins,trimHighPins, 13);

uint8_t rbgPins[3] = {A7,A8,A9};
Led led(rbgPins);

#ifdef ENABLE_DUAL_RATES
Radio radio(6, 13);
#else
Radio radio(6);
#endif

ModelRegistry registry;

PermanentStore store;

unsigned long lastM = 0;
unsigned int iterations = 0;

uint8_t volts[4] = {};
uint8_t signals[1] = {};

void setModel(Model*, int16_t*);
void handleButtons(int16_t *);

#define DELAY 10
#define ITERATIONS(ms) ((ms) / DELAY)

#define THRESHOLD (MAX_LEVEL * 7 / 10)

static __IO uint32_t counter;
static __IO uint32_t currentTime;

#define HISTORY (3)
uint32_t loopTimes[HISTORY] = {};
static __IO WindowedAverage window(&loopTimes, HISTORY);

void delay(uint32_t ms){
  counter = ms;
  while(counter != 0);
}

void SysTick_Handler(void) {
  if (counter != 0) {
    --counter;
  }
  ++currentTime;
}

void setup() {

  // wait for things to stabilize
  delay(500);

#ifdef DEBUG
  Serial.begin(9600);
#endif

  led.begin();
  led.setColor(0,255,0);

  radio.begin();

  controller.begin();

  trim.begin();

  registry.begin();
  setModel(registry.current(), controller.getInputs());

  vmeter1.begin();
  vmeter2.begin();

  telemetry.begin();

#ifdef ENABLE_ALARM
  alarm.begin();
#endif
}

void loop() {
  if (iterations % ITERATIONS(1000) == 0) {
    vmeter1.update();
    vmeter2.update();
    volts[0] = vmeter1.getVoltage();
    volts[1] = vmeter2.getVoltage();

    telemetry.update();

    volts[2] = telemetry.getA1();
    volts[3] = telemetry.getA2();
    signals[0] = telemetry.getRssi();

#ifdef DEBUG
    Serial.println("telemetry: ");
    Serial.println(telemetry.aquired());
    Serial.println(volts[0]);
    Serial.println(volts[1]);
    Serial.println(volts[2]);
    Serial.println(volts[3]);
    Serial.println(signals[0]);
#endif
  }

#ifdef ENABLE_ALARM
  if (iterations % ITERATIONS(300) == 0) {
    alarm.update(currentTime, volts,
                 telemetry.aquired() ? 4 : 2, signals,
                 telemetry.aquired() ? 1 : 0);

    if (alarm.hasSignalAlarm() || alarm.hasVoltAlarm()) {
      led.setColor(255,0,0);
    } else {
      led.setColor(0,255,0);
    }
  }
#endif

  controller.update(currentTime);

  radio.update(controller.getInputs());

  trim.update(currentTime);
}

void setModel(Model *m, int16_t *inputs) {
  store.load(m);

  radio.setModel(m);

  trim.setTrim(m->trim, m->numInputs);

#ifdef ENABLE_ALARM
  alarm.setMinVolts(m->minVolts);
  alarm.setMinSignals(m->minSignals);
#endif
}

int main(void) {
  uint32_t start = 0;

  // initialize
  setup();

  for(;;) {

    // try to mainain stable rate of updates
    if (delay > window.get()) {
      delay(DELAY-window.get());
    }

    start = currentTime;

    // run the main loop
    loop();

    ++iterations;

    if (currentTime > start) {
      window.avg(currentTime - start);
    }
  }
}

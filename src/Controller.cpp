/*
 * Controller.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: ghagleitner
 */

#include "Controller.h"
#include "Model.h"

Controller::Controller(
    ADC_TypeDef *adc,
    ADC_HandleTypeDef *adcHandle,
    uint32_t *adcChannels,
    IRQn_Type irq,
    GPIO_TypeDef **potPorts,
    uint32_t *potPins,
    GPIO_TypeDef **switchPorts,
    uint32_t *switchPins) :
    adc(adc),
    adcHandle(adcHandle),
    adcChannels(adcChannels),
    irq(irq),
    potPorts(potPorts),
    potPins(potPins),
    switchPorts(switchPorts),
    switchPins(switchPins),
    mode2(true)
{}

Controller::~Controller() {
}

void Controller::begin() {
  for (int i = 0; i < 4; ++i) {

      // digital ports
      GPIO_InitTypeDef gpioInit;
      gpioInit.Mode  = GPIO_MODE_INPUT;
      gpioInit.Pull  = GPIO_PULLDOWN;
      gpioInit.Speed = GPIO_SPEED_MEDIUM;
      gpioInit.Pin   = switchPins[i];
      HAL_GPIO_Init(switchPorts[i], &gpioInit);

      // analog ports
      gpioInit.Pin = potPins[i];
      gpioInit.Mode = GPIO_MODE_ANALOG;
      gpioInit.Pull = GPIO_NOPULL;
      gpioInit.Speed = GPIO_SPEED_MEDIUM;
      HAL_GPIO_Init(potPorts[i], &gpioInit);
  }

  // start the interrupt
  HAL_NVIC_SetPriority(irq, 0, 0);
  HAL_NVIC_EnableIRQ(irq);

  adcHandle->Instance = adc;

  adcHandle->Init.ScanConvMode = ENABLE;
  adcHandle->Init.ContinuousConvMode = DISABLE;
  adcHandle->Init.DiscontinuousConvMode = ENABLE;
  adcHandle->Init.NbrOfDiscConversion = 2;
  adcHandle->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  adcHandle->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  adcHandle->Init.NbrOfConversion = 0;

  HAL_ADC_Init(adcHandle);

  ADC_ChannelConfTypeDef adcChannel;

  for (int i = 0; i < 4; ++i) {
    adcChannel.Channel = adcChannels[i];
    adcChannel.Rank = i+1;
    adcChannel.SamplingTime = ADC_CONVERSIONCLOCKCYCLES_SAMPLETIME_13CYCLES5;

    if (HAL_ADC_ConfigChannel(adcHandle, &adcChannel) != HAL_OK)
    {
      asm("bkpt 255");
    }
  }

  HAL_ADC_Start_IT(adcHandle);
}

void Controller::update(unsigned long currentTime) {
  setInputs();
}

int16_t *Controller::getInputs() {
  return inputs;
}

void Controller::setMode(bool mode2) {
  this->mode2 = mode2;
}

bool Controller::getMode() {
  return mode2;
}

int16_t Controller::normalize(int x, int zero, int min, int max) {
  if (min == max) return min;

  int16_t val = (((x - zero) * ((MAX_LEVEL - MIN_LEVEL)/8)) / (max - min)) * 8;
  if (val > MAX_LEVEL) val = MAX_LEVEL;
  if (val < MIN_LEVEL) val = MIN_LEVEL;
  return val;
}

void Controller::setInputs() {

  for (int i = 0; i < 4; ++i) {
    values[i] = 0;//analogRead(potPins[i]);
    inputs[i] = normalize(values[i], zeros[i], mins[i], maxs[i]);
    if (values[i] > maxsReceived[i]) maxsReceived[i] = values[i];
    if (values[i] < minsReceived[i]) minsReceived[i] = values[i];
  }

  for (int i = 0; i < 4; ++i) {
    inputs[i+4] = HAL_GPIO_ReadPin(switchPorts[i],switchPins[i]) == GPIO_PIN_SET
        ? MIN_LEVEL : MAX_LEVEL;
  }
}

#ifdef ENABLE_CALIBRATION
void Controller::calibrateZero() {
  for (int i =0; i < 4; ++i) {
    zeros[i] = values[i];
  }
}

void Controller::calibrateMax() {
  for (int i = 0; i < 4; ++i) {
    mins[i] = minsReceived[i];
    maxs[i] = maxsReceived[i];
  }
}
#endif

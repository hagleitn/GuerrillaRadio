/*
 * Controller.h
 *
 *  Created on: Nov 14, 2016
 *      Author: ghagleitner
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "stdint.h"
#include "limits.h"
#include "stm32f1xx.h"

class Controller {

 private:
  bool mode2 = true;

  int16_t inputs[8] = {};
  int16_t values[8] = {};
  int16_t minsReceived[4] = {INT16_MAX,INT16_MAX,INT16_MAX,INT16_MAX};
  int16_t maxsReceived[4] = {INT16_MIN,INT16_MIN,INT16_MIN,INT16_MIN};
  int16_t mins[4] = {0,0,0,0};
  int16_t maxs[4] = {1023,1023,1023,1023};
  int16_t zeros[4] = {512,512,512,512};

  ADC_TypeDef *adc;
  ADC_HandleTypeDef *adcHandle;
  uint32_t *adcChannels;
  IRQn_Type irq;

  GPIO_TypeDef **potPorts;
  uint32_t *potPins;

  GPIO_TypeDef **switchPorts;
  uint32_t *switchPins;

 private:
  int16_t normalize(int x, int zero, int min, int max);
  void setInputs();

 public:

  Controller(
      ADC_TypeDef *adc, ADC_HandleTypeDef *adcHandle, uint32_t *adcChannels, IRQn_Type irq,
      GPIO_TypeDef **potPorts, uint32_t *potPins,
      GPIO_TypeDef **switchPorts, uint32_t *switchPins);
  ~Controller();
  void begin();
  void update(unsigned long currentTime);
  int16_t *getInputs();
  void setMode(bool mode2);
  bool getMode();
#ifdef ENABLE_CALIBRATION
  void calibrateZero();
  void calibrateMax();
#endif
};

#endif /* CONTROLLER_H_ */

/*
 * Voltmeter.h
 *
 *  Created on: Nov 13, 2016
 *      Author: ghagleitner
 */

#ifndef VOLTMETER_H_
#define VOLTMETER_H_

#include <stdint.h>
#include "stm32f1xx.h"
#include "Battery.h"

class Voltmeter {

 private:
  uint8_t *vins;
  GPIO_TypeDef **ports;
  ADC_TypeDef *adc;
  ADC_HandleTypeDef *adcHandle;
  uint32_t *pins;
  uint32_t *adcChannels;
  IRQn_Type irq;
  long *r1s;
  long *r2s;
  int n;

 public:

  int current;

  Voltmeter(ADC_TypeDef *adc, ADC_HandleTypeDef *adcHandle, GPIO_TypeDef **ports,
      uint32_t *pins, uint32_t *adcChannels, IRQn_Type irq, long *r1s, long *r2s, int n);
  ~Voltmeter();
  void begin();
  void update();
  inline uint8_t getVoltage(int i);
  inline long getR1(int i);
  inline long getR2(int i);
  inline void setVin(int i, uint8_t vin);
  inline ADC_HandleTypeDef *getAdcHandle();
};

inline ADC_HandleTypeDef *Voltmeter::getAdcHandle() {
  return adcHandle;
}

inline long Voltmeter::getR1(int i) {
  return r1s[i];
}

inline long Voltmeter::getR2(int i) {
  return r2s[i];
}

inline void Voltmeter::setVin(int i, uint8_t vin) {
  vins[i] = vin;
}

inline uint8_t Voltmeter::getVoltage(int i) {
  return this->vins[i];
}

#endif /* VOLTMETER_H_ */

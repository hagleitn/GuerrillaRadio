/*
 * PulseGenerator.h
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#ifndef PULSEGENERATOR_H_
#define PULSEGENERATOR_H_

#include "stm32f1xx.h"
#include "CircularBuffer.h"

class PulseGenerator {
private:
  TIM_TypeDef *timer;
  uint32_t timerChannel;
  GPIO_TypeDef *port;
  uint32_t pin;
  uint32_t prescaler;
  CircularBuffer buf;

public:

  void begin();
  void update(uint32_t*,int);

  PulseGenerator(
      TIM_TypeDef *timer,
      uint32_t channel,
      GPIO_TypeDef *port,
      uint32_t pin,
      uint32_t prescaler,
      size_t size,
      int n)
    : timer(timer),
      timerChannel(channel),
      port(port),
      pin(pin),
      prescaler(prescaler),
      buf(n, size)
  {};
  ~PulseGenerator() {};
};

extern "C" {
  void swap();
}

#endif /* PULSEGENERATOR_H_ */

/*
 * Audio.h
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdint.h>
#include "stm32f1xx.h"

class Audio {
private:
  TIM_TypeDef *timer;
  TIM_HandleTypeDef htim;
  TIM_OC_InitTypeDef hOc;
  uint32_t channel;

  GPIO_TypeDef *port;
  uint32_t pin;

  unsigned long startTime;
  uint16_t duration;

public:

  void begin();

  void play(uint16_t *samples);
  void tone(uint16_t freq, uint16_t duration);
  void update(unsigned long currentTime);

  Audio(TIM_TypeDef *timer, uint32_t channel, GPIO_TypeDef *port, uint32_t pin);
  virtual ~Audio();
};

#endif /* AUDIO_H_ */

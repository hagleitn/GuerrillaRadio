/*
 * PpmEncoder.h
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#include <stdint.h>
#include "PulseGenerator.h"

#ifndef PPMENCODER_H_
#define PPMENCODER_H_

#define HEADER (3000)
#define SIG_LOW (300)
#define MIN_DURATION (1000)
#define MID_DURATION (1500)

#define SCALE(A) (A)

#define FRAME_LENGTH (18)
#define PRESCALER (72)

class PpmEncoder {
private:
  PulseGenerator gen;
  uint32_t frame[FRAME_LENGTH];

public:

  void begin();
  void update(int16_t *channels, int n);

  PpmEncoder(TIM_TypeDef *timer, uint32_t timerChannel, GPIO_TypeDef *port, uint32_t pin);
  virtual ~PpmEncoder();
};

#endif /* PPMENCODER_H_ */

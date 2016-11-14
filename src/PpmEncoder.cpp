/*
 * PpmEncoder.cpp
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#include <PpmEncoder.h>

PpmEncoder::PpmEncoder(TIM_TypeDef *timer, uint32_t timerChannel, GPIO_TypeDef *port, uint32_t pin) :
  gen(timer, timerChannel, port, pin, PRESCALER, FRAME_LENGTH, 2) {
}

PpmEncoder::~PpmEncoder() {
}

void PpmEncoder::begin() {
  // init the pulse gen
  update(0, 8);

  gen.begin();

  // init the backup buffer
  update(0, 8);
}

void PpmEncoder::update(int16_t *channels, int n) {
  // PPM signal low is always 300us. One frame is sync pulse (sig low + sigh high 3ms),
  // followed by one pulse per channel (sig low + pwm time - 300)

  // sync pulse
  frame[0] = SCALE(SIG_LOW);
  frame[1] = SCALE(HEADER);

  for (int i = 0; i < n; ++i) {
    // channel pulse
    frame[2*i+2] = SCALE(SIG_LOW);
    frame[2*i+3] = SCALE((channels ? channels[i] : MID_DURATION) - SIG_LOW + MIN_DURATION);
  }

  gen.update(frame, FRAME_LENGTH);
}


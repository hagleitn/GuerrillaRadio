/*
 * Radio.h
 *
 *  Created on: Nov 13, 2016
 *      Author: ghagleitner
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "Audio.h"
#include "PpmEncoder.h"
#include "Model.h"

#define MIN_SERVO_PULSE 800
#define MAX_SERVO_PULSE 1800
#define RANGE (MAX_SERVO_PULSE - MIN_SERVO_PULSE)

class Radio {

 private:
  Audio *audio;
  bool lowRates;
  PpmEncoder encoder;
  int16_t *channels;
  int16_t *inputs;
  int16_t *trim;
  Model *model;

  int16_t norm(int16_t val);
  int16_t expo(int8_t a, int32_t x);
  void mix(int8_t *transform, int16_t *inputs, int16_t *channels, uint8_t n, uint8_t m);
  void init(int16_t *x, uint8_t n, int16_t c);
  void add(int16_t *y, int16_t *x1, int16_t *x2, uint8_t n);
  void scale(int16_t *y, int16_t *x, uint8_t scale, uint8_t n);
  void mult(int16_t *y, int16_t *x1, int8_t *x2, uint8_t n);

 public:

  Radio(Audio *audio,
      TIM_TypeDef *timer,
      uint32_t timerChannel,
      GPIO_TypeDef *port,
      uint32_t pin);
  ~Radio();
  void begin();
  void setModel(Model *m);
  void update(int16_t *inputs);
  int16_t *getInputs();
  int16_t *getChannels();
  int16_t *getTrim();
  void setTrim(int16_t *inputs, uint8_t n);

  void toggleRates();
  bool isLowRates();
};

#endif /* RADIO_H_ */

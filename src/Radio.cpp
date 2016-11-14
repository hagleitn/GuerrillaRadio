/*
 * Radio.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: ghagleitner
 */

#include <Radio.h>

#define LOW_FREQ (220)
#define HIGH_FREQ (880)
#define BUZZ_DURATION (500)

Radio::~Radio() {
}

Radio::Radio(
    Audio *audio,
    TIM_TypeDef *timer,
    uint32_t timerChannel,
    GPIO_TypeDef *port,
    uint32_t pin
) :
  audio(audio),
  lowRates(false),
  encoder(timer, timerChannel, port, pin),
  channels(0),
  inputs(0),
  trim(0),
  model(0)
{}

void Radio::begin() {
  //pinMode(radioPin, OUTPUT);
  //encoderBegin(radioPin);
}

void Radio::setModel(Model *m) {
  this->model = m;

  this->channels = m->channels;
  this->inputs = m->inputs;
  this->trim = m->trim;

  init(this->inputs, this->model->numInputs, 0);
  init(this->channels, this->model->numChannels, 0);

  this->lowRates = false;
}

void Radio::update(int16_t *inputs) {

  uint8_t n = this->model->numInputs;
  uint8_t m = this->model->numChannels;

  // appy expo to input
  for (uint8_t i = 0; i < n; ++i) {
    this->inputs[i] = expo(this->model->expo[i], inputs[i]);
  }

  // apply trim
  add(this->inputs, this->inputs, this->trim, n);

  // process the mix
  mix(this->model->transform, this->inputs, this->channels, n, m);

  for (uint8_t i = 0; i < n; ++i) {
    this->inputs[i] = norm(this->inputs[i]);
  }

  // limit throw with (dual) rates
  mult(this->channels, this->channels, this->lowRates ? this->model->drLow : this->model->drHigh, m);

  encoder.update(this->channels, m);
}

int16_t *Radio::getChannels() {
  return this->channels;
}

int16_t *Radio::getInputs() {
  return this->inputs;
}

int16_t *Radio::getTrim() {
  return this->trim;
}

void Radio::setTrim(int16_t *inputs, uint8_t n) {
  for (uint8_t i = 0; i < n; ++i) {
    this->trim[i] += expo(this->model->expo[i], inputs[i]);
  }
}

void Radio::toggleRates() {
  this->lowRates = !this->lowRates;
  audio->tone(lowRates ? LOW_FREQ : HIGH_FREQ, BUZZ_DURATION);
}

bool Radio::isLowRates() {
  return this->lowRates;
}

/**
 * mix is matrix multiplication:  channel = transform * inputs
 * transform nxm matrix
 * inputs n int16_t values coming from the controler
 * channels m outputs
 */
void Radio::mix(int8_t *transform, int16_t *inputs, int16_t *channels, uint8_t n, uint8_t m) {
  init(channels, m, 0);
  for (uint8_t j = 0; j < m; ++j)  {
    for (uint8_t i = 0; i < n; ++i) {
      channels[j] = norm(channels[j] + MULT(inputs[i], transform[i+j*n]));
    }
  }
}

int16_t Radio::norm(int16_t v) {
  if (v > MAX_LEVEL) return MAX_LEVEL;
  if (v < MIN_LEVEL) return MIN_LEVEL;
  return v;
}

// y = a*x^3+(1-a)x, a .. expo (0 >= a >= 1), x .. input (-1 <= x <= 1)
int16_t Radio::expo(int8_t a, int32_t x) {
  return (((BYTE_TO_PERCENT(a) * x  * x) / 100) * x) / (((int32_t)MAX_LEVEL) * MAX_LEVEL)
    + (100 - BYTE_TO_PERCENT(a)) * x / 100;
}

void Radio::init(int16_t *x, uint8_t n, int16_t c) {
  for (uint8_t i = 0; i < n; ++i) {
    x[i] = c;
  }
}

void Radio::add(int16_t *y, int16_t *x1, int16_t *x2, uint8_t n) {
  for (uint8_t i = 0; i < n; ++i) {
    y[i] = norm(x1[i] + x2[i]);
  }
}

void Radio::scale(int16_t *y, int16_t *x, uint8_t scale, uint8_t n) {
  for (uint8_t i = 0; i < n; ++i) {
    y[i] = scale * x[i];
  }
}

void Radio::mult(int16_t *y, int16_t *x1, int8_t *x2, uint8_t n) {
  for (uint8_t i = 0; i < n; ++i) {
    y[i] = MULT(x1[i], x2[i]);
  }
}


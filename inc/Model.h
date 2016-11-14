/*
 * Model.h
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#ifndef MODEL_H_
#define MODEL_H_

#include<stdint.h>

#define DIVISOR_PERCENTS (32)
#define PERCENT_TO_BYTE(P) ((int8_t)((((int)P) * DIVISOR_PERCENTS) / 100))
#define BYTE_TO_PERCENT(P) ((int8_t)((((int)P) * 100) / DIVISOR_PERCENTS))

#define MAX_LEVEL 512
#define MIN_LEVEL -512

#define MULT(A, B) ((A) * (B) / DIVISOR_PERCENTS)

typedef struct model_ {
  char const *name;
  int8_t *expo;
  int8_t *drLow;
  int8_t *drHigh;
#ifdef ENABLE_ALARM
  uint8_t *minVolts;
  uint8_t *minSignals;
#endif
  int8_t *transform;
  int16_t *channels;
  int16_t *inputs;
  int16_t *trim;
  uint8_t numChannels;
  uint8_t numInputs;
  uint8_t id;
} Model;

#endif /* MODEL_H_ */

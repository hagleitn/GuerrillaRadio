/*
 * WindowedAverage.h
 *
 *  Created on: Nov 13, 2016
 *      Author: ghagleitner
 */

#ifndef WINDOWEDAVERAGE_H_
#define WINDOWEDAVERAGE_H_

#include <stdint.h>
#include "CircularBuffer.h"

class WindowedAverage {
 private:
  CircularBuffer buf;
  int32_t avg;
  int n;

 public:
  int32_t add(int32_t);
  int32_t get();
  WindowedAverage(int n);
  ~WindowedAverage();
};

#endif /* WINDOWEDAVERAGE_H_ */

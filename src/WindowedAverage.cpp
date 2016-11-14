/*
 * WindowedAverage.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: ghagleitner
 */

#include <WindowedAverage.h>

WindowedAverage::WindowedAverage(int n) :
  buf(n * sizeof(int32_t), sizeof(int32_t)),
  avg(0), n(n)
{}

WindowedAverage::~WindowedAverage() {
}

int32_t WindowedAverage::add(int32_t x) {
  int els = buf.getNumElements();
  if (els) {
    int32_t first = *(int32_t *)buf.getFirst();
    avg = (avg*els - first + x) / ((els + 1 > n) ? n : els + 1);
  } else {
    avg = x;
  }
  buf.add((uint8_t*)&x);

  return avg;
}

 int32_t WindowedAverage::get() {
  return avg;
}

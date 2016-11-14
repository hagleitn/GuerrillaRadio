/*
 * CircularBuffer.cpp
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#include "CircularBuffer.h"
#include <stdlib.h>

void CircularBuffer::add(uint8_t *el) {
  if (first == next) {
    first += size;
    if (first >= buf + n) {
      first = buf;
    }
  }

  if (!first) {
    first = next;
  }

  if (el) {
    for(int i = 0; i < (int)size; ++i) {
      *next++ = *el++;
    }
  }

  if (next + size > buf + n) {
    next = buf;
  }
}

uint8_t *CircularBuffer::getFirst() {
  return first;
}

uint8_t *CircularBuffer:: getLast() {
  return next - size < buf ? buf + (n/size) * size : next - size;
}

uint8_t *CircularBuffer::getNth(int i) {
  int max = n/size;
  i = i + (first - buf) / size;
  return buf + (i % max) * size;
}

int CircularBuffer::getNumElements() {
  if (!first) {
    return 0;
  }
  return (first == next) ? n/size : (next - first) / size;
}

CircularBuffer::CircularBuffer(int n, size_t size) :
  buf((uint8_t*)malloc(n*size)), next(buf), first(0), n(n), size(size) {
}

CircularBuffer::~CircularBuffer() {
  free(buf);
}

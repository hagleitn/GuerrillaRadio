#include "CyclicBuffer.h"

void CyclicBuffer::add(uint8_t *el) {
  if (first == next) {
    first += size;
    if (first >= buf + n) {
      first = buf;
    }
  }

  if (!first) {
    first = next;
  }

  for(int i = 0; i < size; ++i) {
    *next++ = *el++;
  }

  if (next + size > buf + n) {
    next = buf;
  }
}

uint8_t *CyclicBuffer::first() {
  return *first;
}

uint8_t *CycicBuffer:: last() {
  next - size < buf ? buf + (n/size) * size : next - size;
}

uint8_t *CyclicBuffer::nth(int i) {
  int max = n/size;
  i = i + (first - buf) / size;
  return buf + (i % max) * size;
}

int CyclicBuffer::elements() {
  if (!first) {
    return 0;
  }
  return (first == next) n/size : (next - first) / size;
}

CyclicBuffer::CyclicBuffer(int32_t *buf, int n, size_t size) :
  buf(buf), n(n), next(buf), first(0)
{}

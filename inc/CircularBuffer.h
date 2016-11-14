#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <stdint.h>
#include <stddef.h>

class CircularBuffer {
 private:

  uint8_t *buf;
  uint8_t *next;
  uint8_t *first;

  int n;
  size_t size;

 public:

  void add(uint8_t *);
  uint8_t *getFirst();
  uint8_t *getLast();
  uint8_t *getNth(int n);
  int getNumElements();

  CircularBuffer(int n, size_t size);
  ~CircularBuffer();
};

#endif /* CIRCULARBUFFER_H_ */

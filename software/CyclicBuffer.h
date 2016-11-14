#ifndef CyclicBuffer_h
#define CyclicBuffer_h

class CyclicBuffer {
 private:

  uint8_t *buf;
  uint8_t *next;
  uint8_t *first;

  int n;
  size_t size;

 public:

  void add(uint8_t *);
  uint8_t *first();
  uint8_t *last();
  uint8_t *nth(int n);
  int elements();

  CyclicBuffer(int32_t *buf, int n, size_t size);
}

#endif

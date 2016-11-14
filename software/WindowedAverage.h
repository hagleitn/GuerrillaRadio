#ifndef WindowedAverage_h
#define WindowedAverage_h

class WindowedAverage {
 private:
  CyclicBuffer buf;
  int32_t avg;
  int n;

 public:
  inline int32_t add(uint32_t);
  inline int32_t get();
  inline WindowedAverage(int32_t *buf, int n);
}

inline WindowedAverage(int32_t *buf, int n) :
  buf((uint8_t *)buf, n * sizeof(int32_t), sizeof(int32_t)),
  avg(0), n(n)
{}

inline int32_t add(int32_t x) {
  int els = buf.elements();
  if (els) {
    int32_t first = *(int32_t *)buf.first();
    avg = (avg*els - first + x) / ((els + 1 > n) ? n : els + 1);
  } else {
    avg = x;
  }
  buf.add((uint8_t*)&x);

  return avg;
}

inline int32_t get() {
  return avg;
}
#endif

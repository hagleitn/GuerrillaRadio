#ifndef TRIM_H_
#define TRIM_H_

#include "stm32f1xx.h"
#include "Audio.h"

#define TRIM_MIN (-128)
#define TRIM_MAX (128)
#define TRIM_INCREMENT (4)
#define MIN_UPDATE_INTERVAL (512)

#define INC_FREQ (880)
#define DEC_FREQ (440)
#define BEEP_DURATION (256)

class Trim {

 private:

  GPIO_TypeDef **trimLowPorts;
  uint32_t *trimLowPins;

  GPIO_TypeDef **trimHighPorts;
  uint32_t *trimHighPins;

  Audio *audio;

  int numInputs;

  unsigned long lastAdjust;

  int16_t *trim;

 public:

  Trim(GPIO_TypeDef **, uint32_t *, GPIO_TypeDef **, uint32_t *, Audio *, int);
  ~Trim();

  void setTrim(int16_t *trim);

  void begin();
  void update(unsigned long currentTime);
};

#endif /* TRIM_H_ */

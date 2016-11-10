#ifndef Trim_h
#define Trim_h

#include "Arduino.h"

#define TRIM_MIN (-128)
#define TRIM_MAX (128)
#define TRIM_INCREMENT (4)
#define MIN_UPDATE_INTERVAL (512)

#define INC_FREQ (880)
#define DEC_FREQ (440)
#define BEEP_DURATION (256);

class Trim {

 private:

  int numInputs;
  uint8_t alertPin;
  int *trim;
  uint8_t *trimHighPins;
  uint8_t *trimLowPins;

  unsigned long lastAdjust;

 public:

  inline Trim();

  inline void setTrim(int *trim);

  void begin();
  void update(unsigned long currentTime);
};

inline Trim::Trim(uint8_t *trimLowPins, uint8_t *trimHighPins,
                  int numInputs, uint8_t alertPin) :
            trimLowPins(trimLowPins), trimHighPins(trimHighPins),
            numInputs(numInputs), alertPin(alertPin)
{}

inline Trim::setTrim(int *trim) {
  this->trim = trim;
}

#endif

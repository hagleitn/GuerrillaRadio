#ifndef Led_h
#define Led_h

#include "Arduino.h"

class Led {

 private:

  uint8_t *rgbPins;
  uint8_t color[3] = {};

  unsigned long lastAdjust;

 public:

  inline Led(uint8_t *rgbPins);

  inline void setColor(uint8_t red, uint8_t green, uint8_t blue);

  void begin();
  void update(unsigned long currentTime);
};

inline Led::Led(uint8_t *rgbPins) :
            rgbPins(rgbPins)
{}

inline Led::begin() {
  for (int i = 0; i < 3; ++i) {
    pinMode(rgbPins[i], OUTPUT);
  }
}

inline Led::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  color[0] = red;
  color[1] = green;
  color[2] = blue;

  for (int i = 0; i < 3; ++i) {
    analogWrite(rgbPins[i], color[i]);
  }
}

#endif

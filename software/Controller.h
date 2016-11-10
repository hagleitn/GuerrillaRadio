#ifndef Controller_h
#define Controller_h

#include "Arduino.h"
#include "limits.h"

class Controller {

 private:
  bool mode2 = true;

  int minPitch;
  int maxPitch;
  int minRoll;
  int maxRoll;
  int minX;
  int maxX;
  int minY;
  int maxY;
  int zeroPitch;
  int zeroRoll;
  int zeroX;
  int zeroY;

  int16_t inputs[8] = {};
  int16_t minsReceived[4] = {INT16_MAX,INT16_MAX,INT16_MAX,INT16_MAX};
  int16_t maxsReceived[4] = {INT16_MIN,INT16_MIN,INT16_MIN,INT16_MIN};
  int16_t mins[4] = {0,0,0,0};
  int16_t maxs[4] = {1023,1023,1023,1023};
  int16_t zeros[4] = {512,512,512,512};
  int8_t *potPins;
  int8_t *switchPins;

 private:
  int16_t normalize(int x, int zero, int min, int max);
  void setInputs();

 public:

  inline Controller(int16_t*, int16_t*);
  inline void begin();
  inline void update(unsigned long currentTime);
  inline int16_t *getInputs();
  inline void setMode(bool mode2);
  inline bool getMode();
#ifdef ENABLE_CALIBRATION
  void calibrateZero();
  void calibrateMax();
#endif
};

inline Controller::Controller(int8_t *potPins, int8_t *switchPins) :
                  potPins(potPins), switchPins(switchPins),
                  mode2(true), minPitch(0),
                  maxPitch(1023), minRoll(0), maxRoll(1023),
                  minX(0), maxX(1023), minY(0), maxY(1023),
                  zeroPitch(90), zeroRoll(0), zeroX(0), zeroY(0)
{}

inline void Controller::begin() {
  for (int i = 0; i < 4; ++i) {
    pinMode(switchPins[i], INPUT);
  }
}

inline void Controller::update(unsigned long currentTime) {
  setInputs();
}

inline int16_t *Controller::getInputs() {
  return inputs;
}

inline void Controller::setMode(bool mode2) {
  this->mode2 = mode2;
}

inline bool Controller::getMode() {
  return mode2;
}

#endif

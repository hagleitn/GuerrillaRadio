#include "Controller.h"
#include "Model.h"

int16_t Controller::normalize(int x, int zero, int min, int max) {
  if (min == max) return min;

  int16_t val = (((x - zero) * ((MAX_LEVEL - MIN_LEVEL)/8)) / (max - min)) * 8;
  if (val > MAX_LEVEL) val = MAX_LEVEL;
  if (val < MIN_LEVEL) val = MIN_LEVEL;
  return val;
}

void Controller::setInputs() {

  for (int i = 0; i < 8; ++i) {
    int16_t val = analogRead(potPins[i]);
    inputs[i] = normalize(val, zeroPots[i], minPots[i], maxPots[i]);
    if (val > maxsReceived[i]) maxsReceived[i] = val;
    if (val < minsReceived[i]) minsReceived[i] = val;
  }

  for (int i = 0; i < 8; ++i) {
    inputs[i+4] = digitalRead(switchPins[i]) == HIGH ? MIN_LEVEL : MAX_LEVEL;
  }
}

#ifdef ENABLE_CALIBRATION
void Controller::calibrateZero() {
  for (int i =0; i < 4; ++i) {
    zeroPots[i] = inputs[i];
  }
}

void Controller::calibrateMax() {
  for (int i = 0; i < 4; ++i) {
    mins[i] = minsReceived[i];
    maxs[i] = maxsReceived[i];
  }
}
#endif

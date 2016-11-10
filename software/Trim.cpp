#include "Trim.h"

void Trim::update(unsigned long currentTime) {
  if (currentTime - lastAdjust < MIN_UPDATE_INTERVAL) {
    return;
  }

  for (int i = 0; i < numInputs; ++i) {
    if (digitalRead(trimLowPins[i]) == HIGH) {
      lastAdjust = currentTime;
      trim[i] -= TRIM_INCREMENT;
      if (trim[i] < TRIM_MIN) {
        trim[i] = TRIM_MIN;
      }
      tone(alertPin, DEC_FREQ, BEEP_DURATION);
    }

    if (digitalRead(trimHighPins[i]) == HIGH) {
      lastAdjust = currentTime;
      trim[i] += TRIM_INCREMENT;
      if (trim[i] > TRIM_MAX) {
        trim[i] = TRIM_MAX;
      }
      tone(alertPin, INC_FREQ, BEEP_DURATION);
    }
  }
}

Trim::begin() {
  for (int i = 0; i < numInputs; ++i) {
    pinMode(trimLowPins[i], INPUT);
    pinMode(trimHighPins[i], INPUT);
  }
}

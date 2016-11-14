#include "Trim.h"

void Trim::update(unsigned long currentTime) {
  if (currentTime - lastAdjust < MIN_UPDATE_INTERVAL) {
    return;
  }

  for (int i = 0; i < numInputs; ++i) {
    if (HAL_GPIO_ReadPin(trimLowPorts[i],trimLowPins[i]) == GPIO_PIN_SET) {
      lastAdjust = currentTime;
      trim[i] -= TRIM_INCREMENT;
      if (trim[i] < TRIM_MIN) {
        trim[i] = TRIM_MIN;
      }
      audio->tone(DEC_FREQ, BEEP_DURATION);
    }

    if (HAL_GPIO_ReadPin(trimLowPorts[i],trimLowPins[i]) == GPIO_PIN_SET) {
      lastAdjust = currentTime;
      trim[i] += TRIM_INCREMENT;
      if (trim[i] > TRIM_MAX) {
        trim[i] = TRIM_MAX;
      }
      audio->tone(INC_FREQ, BEEP_DURATION);
    }
  }
}

void Trim::begin() {
  for (int i = 0; i < numInputs; ++i) {
    // Set up GPIOB1 (timer 3, channel 4) in alternate function (AF) mode
      GPIO_InitTypeDef GPIO_InitStruct;
      GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
      GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
      GPIO_InitStruct.Pin   = trimLowPins[i];
      HAL_GPIO_Init(trimLowPorts[i], &GPIO_InitStruct);
  }

  for (int i = 0; i < numInputs; ++i) {
    // Set up GPIOB1 (timer 3, channel 4) in alternate function (AF) mode
      GPIO_InitTypeDef GPIO_InitStruct;
      GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
      GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
      GPIO_InitStruct.Pin   = trimHighPins[i];
      HAL_GPIO_Init(trimHighPorts[i], &GPIO_InitStruct);
  }
}

Trim::Trim(GPIO_TypeDef **trimLowPorts, uint32_t *trimLowPins,
    GPIO_TypeDef **trimHighPorts, uint32_t *trimHighPins,
    Audio *audio, int numInputs) :
                trimLowPorts(trimLowPorts), trimLowPins(trimLowPins),
                trimHighPorts(trimHighPorts), trimHighPins(trimHighPins),
                audio(audio), numInputs(numInputs), lastAdjust(0), trim(0)
{}

Trim::~Trim() {
}

void Trim::setTrim(int16_t *trim) {
  this->trim = trim;
}


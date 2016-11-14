/*
 * Led.cpp
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#include <Led.h>

Led::~Led() {
}

Led::Led(TIM_TypeDef *timer, uint32_t *timerChannels, GPIO_TypeDef **rgbPorts, uint32_t *rgbPins) :
            timer(timer), timerChannels(timerChannels), htim(),
            rgbPorts(rgbPorts), rgbPins(rgbPins), mode(SOLID), toggle(1),
            lastToggle(0), currentColor(0)
{}

void Led::begin() {
  for (int i = 0; i < 3; ++i) {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStruct.Pin   = rgbPins[i];
    HAL_GPIO_Init(rgbPorts[i], &GPIO_InitStruct);
  }

  // Set up the counter itself.
  htim.Instance = timer;

  // No prescaler (PSC), so the count (CNT) will count up 72 million times per second
  htim.Init.Prescaler   = 72;
  htim.Init.CounterMode = TIM_COUNTERMODE_UP;

  // When the counter hits the Period value, it will reset back to 0.
  // This is the ARR register. In this case, the clock is 72mhz so:
  //   64,000,000 / 842 / 2 = 38,005hz
  // The divide by two is because it takes two toggles to create one wave
  htim.Init.Period = 256;

  TIM_OC_InitTypeDef hOc;

  hOc.OCMode = TIM_OCMODE_PWM1;
  hOc.Pulse = 0;
  hOc.OCIdleState = TIM_OCIDLESTATE_SET;
  hOc.OCPolarity = TIM_OCPOLARITY_HIGH;
  hOc.OCFastMode = TIM_OCFAST_ENABLE;

  for (int i = 0; i < 3; ++i) {
    HAL_TIM_PWM_ConfigChannel(&htim, &hOc, timerChannels[i]);
  }

  HAL_TIM_Base_Init(&htim);
  HAL_TIM_PWM_Init(&htim);

  start();

}

void Led::start() {
  for (int i = 0; i < 3; ++i) {
    HAL_TIM_PWM_Start(&htim, timerChannels[i]);
  }
}

void Led::stop() {
  for (int i = 0; i < 3; ++i) {
    HAL_TIM_PWM_Stop(&htim, timerChannels[i]);
  }
}

void Led::updateChannels() {
  for (int i = 0; i < 3; ++i) {
    __HAL_TIM_SET_COMPARE(&htim, timerChannels[i], color[i]);
  }
}

void Led::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  color[0] = red;
  color[1] = green;
  color[2] = blue;

  updateChannels();
}

void Led::setMode(MODE m) {
  if (mode == BLINK && !toggle) {
    start();
  }

  setColor(0,0,0);

  mode = m;

  toggle = true;
  currentColor = 0;
}

void Led::update(unsigned long currentTime) {
  switch(mode) {
  case BLINK:
    if (currentTime - lastToggle > BLINK_DURATION) {
      lastToggle = currentTime;
      toggle = !toggle;
      if (toggle) {
        start();
      } else {
        stop();
      }
    }
    break;
  case FLOAT:
    color[currentColor] += toggle ? 1 : -1;
    if (color[currentColor] == 0 || color[currentColor] == 255) {
      currentColor = currentTime % 3;
      toggle = color[currentColor] == 0;
    }
    updateChannels();
    break;
  case SOLID:
    break;
  }
}

/*
 * Audio.cpp
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#include <Audio.h>

Audio::Audio(TIM_TypeDef *timer,
    uint32_t channel, GPIO_TypeDef *port, uint32_t pin) :
    timer(timer), channel(channel), port(port),
    pin(pin), startTime(0), duration(0)
{}

void Audio::begin() {

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  GPIO_InitStruct.Pin   = pin;
  HAL_GPIO_Init(port, &GPIO_InitStruct);

  // Set up the counter itself.
  htim.Instance = timer;

  // No prescaler (PSC), so the count (CNT) will count up 72 million times per second
  htim.Init.Prescaler   = 71;
  htim.Init.CounterMode = TIM_COUNTERMODE_UP;

  hOc.OCMode = TIM_OCMODE_PWM1;
  hOc.Pulse = 0;
  hOc.OCIdleState = TIM_OCIDLESTATE_SET;
  hOc.OCPolarity = TIM_OCPOLARITY_HIGH;
  hOc.OCFastMode = TIM_OCFAST_ENABLE;
}

void Audio::play(uint16_t* samples) {
}

void Audio::tone(uint16_t freq, uint16_t duration) {
  if (this->duration) {
    // don't allow sounds to be interrupted
    return;
  }

  this->duration = duration;

  // When the counter hits the Period value, it will reset back to 0.
  // This is the ARR register.
  //   1,000,000 / period / 2 = freq hz
  // The divide by two is because it takes two toggles to create one wave
  htim.Init.Period = 500000/freq;

  HAL_TIM_PWM_ConfigChannel(&htim, &hOc, channel);

  HAL_TIM_Base_Init(&htim);
  HAL_TIM_PWM_Init(&htim);

  HAL_TIM_PWM_Start(&htim, channel);
}

void Audio::update(unsigned long currentTime) {
  if (duration && !startTime) {
    startTime = currentTime;
  }

  if (startTime && currentTime - startTime > (unsigned long)duration) {
    HAL_TIM_PWM_Stop(&htim, channel);
    HAL_TIM_PWM_DeInit(&htim);
    HAL_TIM_Base_DeInit(&htim);
    duration = 0;
    startTime = 0;
  }
}

Audio::~Audio() {
}


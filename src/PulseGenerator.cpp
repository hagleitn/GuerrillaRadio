/*
 * PulseGenerator.cpp
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#include <PulseGenerator.h>

static CircularBuffer *_buf;

void PulseGenerator::begin() {

  _buf = &buf;

  DMA_HandleTypeDef DMA_Handle;

  DMA_Handle.Instance = DMA1_Channel6;
  DMA_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
  DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
  DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;
  DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  DMA_Handle.Init.Mode = DMA_CIRCULAR;
  DMA_Handle.Init.Priority = DMA_PRIORITY_HIGH;
  HAL_DMA_Init(&DMA_Handle);

  // Set up GPIOB1 (timer 3, channel 4) in alternate function (AF) mode
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pin   = pin;
  HAL_GPIO_Init(port, &GPIO_InitStruct);

  // Set up the counter itself.
  TIM_Base_InitTypeDef TIM_BaseStruct;
  TIM_HandleTypeDef TIM_HandleStruct;
  TIM_HandleStruct.Instance = timer;

  // No prescaler (PSC), so the count (CNT) will count up 72 million times per second
  TIM_BaseStruct.Prescaler   = prescaler;
  TIM_BaseStruct.CounterMode = TIM_COUNTERMODE_UP;

  // When the counter hits the Period value, it will reset back to 0.
  // This is the ARR register. In this case, the clock is 72mhz so:
  //   64,000,000 / 842 / 2 = 38,005hz
  // The divide by two is because it takes two toggles to create one wave
  TIM_BaseStruct.Period = 0;

  TIM_HandleStruct.Init    = TIM_BaseStruct;
  //TIM_HandleStruct.Channel = HAL_TIM_ACTIVE_CHANNEL_4;

  // Initialize the timer hardware in output compare mode
  HAL_TIM_OC_Init(&TIM_HandleStruct);

  // Set the parameters for output compare
  TIM_OC_InitTypeDef TIM_OCStruct;

  // Toggle the associated pin when CNT >= CCR
  TIM_OCStruct.OCMode = TIM_OCMODE_TOGGLE;

  // This is the counter value when the the channel will be toggled
  // For this simple case, the value here does not matter.
  TIM_OCStruct.Pulse = 0;

  // Configure the channel.
  HAL_TIM_OC_ConfigChannel(&TIM_HandleStruct, &TIM_OCStruct, timerChannel);

  // swap out the first values
  swap();

  // Start the timer comparing
  HAL_TIM_OC_Start(&TIM_HandleStruct, timerChannel);
}

void PulseGenerator::update(uint32_t *times, int n) {
  uint32_t *p = (uint32_t *)buf.getFirst();
  while(n--) {
    *p++ = *times++;
  }
}

void swap() {
  _buf->add(0);
}

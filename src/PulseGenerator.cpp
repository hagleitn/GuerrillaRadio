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

	TIM_BaseStruct.Prescaler   = prescaler;
	TIM_BaseStruct.CounterMode = TIM_COUNTERMODE_UP;

	// When the counter hits the Period value, it will reset back to 0.
	// This is the ARR register. In this case, the clock is 72mhz so:
	TIM_BaseStruct.Period = 0;

	TIM_HandleStruct.Init    = TIM_BaseStruct;

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

	static DMA_HandleTypeDef  hdma_tim;

	/* Set the parameters to be configured */
	hdma_tim.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_tim.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_tim.Init.MemInc = DMA_MINC_ENABLE;
	hdma_tim.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD ;
	hdma_tim.Init.MemDataAlignment = DMA_MDATAALIGN_WORD ;
	hdma_tim.Init.Mode = DMA_CIRCULAR;
	hdma_tim.Init.Priority = DMA_PRIORITY_HIGH;

	/* Set hdma_tim instance */
	hdma_tim.Instance = DMA1_Channel1;

	__HAL_LINKDMA(&TIM_HandleStruct, hdma[TIM_DMA_ID_UPDATE], hdma_tim);

	/* Initialize TIMx DMA handle */
	HAL_DMA_Init(TIM_HandleStruct.hdma[TIM_DMA_ID_UPDATE]);

	/*##-2- Configure the NVIC for DMA #########################################*/
	/* NVIC configuration for DMA transfer complete interrupt */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);


	// swap out the first values
	swap();

	// Start the timer comparing
	HAL_TIM_OC_Start(&TIM_HandleStruct, timerChannel);

	HAL_TIM_DMABurst_WriteStart(&TIM_HandleStruct, TIM_DMABASE_ARR,
			TIM_DMA_UPDATE, (uint32_t*)buf.getFirst(), TIM_DMABURSTLENGTH_1TRANSFER);
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

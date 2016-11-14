/*
 * Voltmeter.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: ghagleitner
 */

#include "Voltmeter.h"
#include "stdlib.h"

Voltmeter::Voltmeter(
    ADC_TypeDef *adc,
    ADC_HandleTypeDef *adcHandle,
    GPIO_TypeDef **ports,
    uint32_t *pins,
    uint32_t *adcChannels,
    IRQn_Type irq,
    long *r1s,
    long *r2s,
    int n) :
    adc(adc),
    vins((uint8_t*)malloc(n*sizeof(uint8_t))),
    ports(ports),
    adcHandle(adcHandle),
    adcChannels(adcChannels),
    irq(irq),
    pins(pins),
    r1s(r1s),
    r2s(r2s),
    n(n),
    current(0)
{
}

static Voltmeter *vmeter;

extern "C"
{

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* adcHandle)
{
  vmeter->setVin(vmeter->current, COMPUTE_VOLTS(HAL_ADC_GetValue(adcHandle),
      vmeter->getR1(vmeter->current), vmeter->getR2(vmeter->current)));
}

void ADC_IRQHandler()
{
  HAL_ADC_IRQHandler(vmeter->getAdcHandle());
}

}

void Voltmeter::begin() {
  GPIO_InitTypeDef gpioInit;

  for (int i = 0; i < n; ++i) {
    gpioInit.Pin = pins[i];
    gpioInit.Mode = GPIO_MODE_ANALOG;
    gpioInit.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ports[i], &gpioInit);
  }

  HAL_NVIC_SetPriority(irq, 0, 0);
  HAL_NVIC_EnableIRQ(irq);

  ADC_ChannelConfTypeDef adcChannel;

  adcHandle->Instance = adc;

  //adcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
  //adcHandle.Init.Resolution = ADC_RESOLUTION_12B;
  adcHandle->Init.ScanConvMode = ENABLE;
  adcHandle->Init.ContinuousConvMode = DISABLE;
  adcHandle->Init.DiscontinuousConvMode = ENABLE;
  adcHandle->Init.NbrOfDiscConversion = 2;
  //adcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  adcHandle->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  adcHandle->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  adcHandle->Init.NbrOfConversion = 0;
  //adcHandle.Init.DMAContinuousRequests = ENABLE;
  //adcHandle.Init.EOCSelection = DISABLE;

  HAL_ADC_Init(adcHandle);

  for (int i = 0; i < n; ++i) {
    adcChannel.Channel = adcChannels[i];
    adcChannel.Rank = i+1;
    adcChannel.SamplingTime = ADC_CONVERSIONCLOCKCYCLES_SAMPLETIME_13CYCLES5;

    if (HAL_ADC_ConfigChannel(adcHandle, &adcChannel) != HAL_OK)
    {
      asm("bkpt 255");
    }
  }

  HAL_ADC_Start_IT(adcHandle);
}

void Voltmeter::update() {
  long value = 0;
  if (HAL_ADC_PollForConversion(adcHandle, 1000000) == HAL_OK)
  {
    value = HAL_ADC_GetValue(adcHandle);
  }
}

Voltmeter::~Voltmeter() {
  free(vins);
}


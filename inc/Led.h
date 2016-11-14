/*
 * Led.h
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#ifndef LED_H_
#define LED_H_

#include "stm32f1xx.h"

#define BLINK_DURATION (500ul)

class Led {

public:
  enum MODE { FLOAT, SOLID, BLINK };

private:

  TIM_TypeDef *timer;
  uint32_t *timerChannels;
  TIM_HandleTypeDef htim;
  GPIO_TypeDef **rgbPorts;
  uint32_t *rgbPins;
  uint8_t color[3];

  MODE mode;
  bool toggle;
  unsigned long lastToggle;

  int currentColor;

  void start();
  void stop();
  void updateChannels();

public:

  Led(TIM_TypeDef *timer, uint32_t *timerChannels, GPIO_TypeDef **rgbPorts, uint32_t *rgbPins);
  ~Led();

  void setColor(uint8_t red, uint8_t green, uint8_t blue);
  void setMode(MODE m);
  void begin();
  void update(unsigned long currentTime);
};

#endif /* LED_H_ */

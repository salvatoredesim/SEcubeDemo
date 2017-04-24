/*
 * LedHw.h
 *
 *  Created on: 01 dic 2016
 *      Author: raidenfox
 */

#ifndef LEDHW_H_
#define LEDHW_H_

#include "gpio.h"

#define LED_GPIO_GATE1 GPIOD
#define LED_GPIO_GATE2 GPIOE

#define LED1 GPIO_PIN_14
#define LED2 GPIO_PIN_15
#define LED3 GPIO_PIN_0
#define LED4 GPIO_PIN_1
#define LED5 GPIO_PIN_7
#define LED6 GPIO_PIN_8
#define LED7 GPIO_PIN_9
#define LED8 GPIO_PIN_10

#define ON GPIO_PIN_RESET
#define OFF GPIO_PIN_SET

GPIO_InitTypeDef GPIO_Leds_InitStruct;

void Init_Leds();
void write_conf();
void write_conf_fail();

#endif /* LEDHW_H_ */

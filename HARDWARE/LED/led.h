#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
//#include "timer.h"
//#include "lcd.h"
#include "stdio.h"
#include "delay.h"
#include "sys.h"
//#include "pwm.h"
#include "key.h"

#define LED0 PAout(8);      //PA8引脚的输出

void LED_Init(void);
void LED_GPIO_High(void);
void LED_GPIO_low(void);

#endif
#ifndef __DELAY_H
#define __DELAY_H
#include "sys.h"

//初始化SysTick定时器
void delay_init(void);

//毫秒级延时，参数nms为延时毫秒数
void delay_ms(u32 nms);

//微秒级延时，参数nus为延时微秒数
void delay_us(u32 nms);

//简单毫秒级延时（不依赖OS），参数nms为延时毫秒数
void delay_xms(u32 nms);


#endif
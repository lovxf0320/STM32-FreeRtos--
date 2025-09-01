#ifndef __BEGSTP_H
#define __BEGSTP_H
#include "stm32f10x.h"


/*
 * 模块: TB6612 电机方向控制（仅方向，PWM 在其他模块）
 * 管脚:
 *   - PB12/PB13 -> 电机1 方向 IN1/IN2
 *   - PB14/PB15 -> 电机2 方向 IN1/IN2
 *   - PB3       -> TB6612 STBY（高=使能，低=待机/停止）
 * 使用:
 *   1) 先调用 begstp_Init() 完成 GPIO 初始化并默认退出待机。
 *   2) 速度由 PWM 模块控制；本模块只改变方向/启停。
 *   3) stop() 会拉低 STBY（PB3），需再次拉高后方可继续转动。
 */
#define C_DIR_1 PBout(12)
#define C_DIR_2 PBout(13)
#define D_DIR_1 PBout(14)
#define D_DIR_2 PBout(15)


/*
 * 两侧电机同时控制
 */
void backforward(void);
void forward(void);
void stop(void);
void right(void);
void left(void);
void begstp_Init(void);


/*
 * 单独控制某一路电机方向
 */
void motor1_forward(void);
void motor1_backforward(void);
void motor2_forward(void);
void motor2_backforward(void);


#endif
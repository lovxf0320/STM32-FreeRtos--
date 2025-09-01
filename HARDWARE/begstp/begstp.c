#include "begstp.h"

/*
 * 模块总览: TB6612 的方向与启停控制
 * - 方向控制: 
 *   电机1 -> PB12/PB13 分别对应 IN1/IN2；
 *   电机2 -> PB14/PB15 分别对应 IN1/IN2；
 *   IN1=0, IN2=1 => 正转；IN1=1, IN2=0 => 反转；
 * - 使能/待机:
 *   STBY 引脚接 PB3；PB3=1 为使能，PB3=0 为待机（电机停止）。
 * - 使用建议:
 *   1) 先调用 begstp_Init() 完成 GPIO 初始化并置 PB3=1。
 *   2) 通过 forward()/backforward() 控制两侧同向；
 *      通过 motor1_* / motor2_* 精细控制单侧方向；
 *   3) stop() 会拉低 PB3 使进入待机，需重新拉高 PB3 才能继续运行。
 */

/*
 *电路板电机驱动部分映射如下
 *电机    编码器引脚     编码器定时器    编码器数组数据   引脚:EN   DIR_1  DIR_2    定时器通道
 *  A       B6,B7          TIM4       Encoder_Date[2]   PA8      PB0    PB1     TIM1->CH1
 *  B       A6,A7          TIM3       Encoder_Date[1]   PA9      PB10   PB11    TIM1->CH2
 */

/*
四个管脚设置不同的电平(1为高电平0为低电平)，作为TB6612的逻辑输入，用以控制电机转动状态。 
打个比方，四个管脚IO分别设置成0 1 0 1，电机反转，那么反过来设置成1 0 1 0时，电机正转
*/
	
/*查阅真值表，AIN1  0   0   1
             AIN2  0   1   0
                 停止 正转 反转*/


void begstp_Init(void){
    GPIO_InitTypeDef GPIO_InitTypeStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_3;
    GPIO_InitTypeStructure.GPIO_Speed=GPIO_Speed_50HZ;
    GPIO_Init(GPIOB,&GPIO_InitTypeStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_3);     //PB3高电平 - 使能TB6612
    //由于STBY 引脚接 PB3；PB3=1 为使能，PB3=0 为待机（电机停止）。
}


void backforward(void){
    //小车两个电机反转，因为越过终点线时，单纯停止电机工作，小车会因为惯性停不下来，所以短时间的反转，能抵消惯性，从而使小车迅速停下来
    GPIO_SetBits(GPIOB,GPIO_Pin_12);    //PB12高电平    1
    GPIO_ResetBits(GPIOB,GPIO_Pin_13);  //PB13低电平    0
    GPIO_SetBits(GPIOB,GPIO_Pin_14);    //PB14高电平    1
    GPIO_ResetBits(GPIOB,GPIO_Pin_15);  //PB15高电平    0
}


void forward(void){
    //小车两个电机正转
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);  //PB12低电平    0
    GPIO_SetBits(GPIOB,GPIO_Pin_13);    //PB13高电平    1
    
    GPIO_ResetBits(GPIOB,GPIO_Pin_14);  //PB14高电平    0
    GPIO_SetBits(GPIOB,GPIO_Pin_15);    //PB15高电平    1
}


void stop(void){//STBY置零，电机停止    PB3
    GPIO_ResetBits(GPIOB,GPIO_Pin_3);
}

//******************设置方向*******************************

void motor1_forward(void){
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);  //PB12低电平    0
    GPIO_SetBits(GPIOB,GPIO_Pin_13);    //PB13高电平    1
}


void motor2_forward(void){
    GPIO_ResetBits(GPIOB,GPIO_Pin_14);  //PB14低电平    0
    GPIO_SetBits(GPIOB,GPIO_Pin_15);    //PB15高电平    1
}


void motor1_backforward(void){
    GPIO_SetBits(GPIOB,GPIO_Pin_12);    //PB12高电平    1
    GPIO_ResetBits(GPIOB,GPIO_Pin_13);  //PB13低电平    0
}


void motor2_backforward(void){
    GPIO_SetBits(GPIOB,GPIO_Pin_14);    //PB14高电平    1
    GPIO_ResetBits(GPIOB,GPIO_Pin_15);  //PB15高电平    0
}
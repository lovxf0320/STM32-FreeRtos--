#include "led.h"

//初始化PB5和PE5位输出口，并使能这两个口的时钟
//LED IO初始化
void LED_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    //使能APB2外设时钟
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitTypeDef);                     //初始化GPIO
}

void LED_GPIO_High(void){
    GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

void LED_GPIO_low(void){
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);       //清空数据端口位
}
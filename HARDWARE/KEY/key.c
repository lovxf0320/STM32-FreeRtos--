#include "stm32f10x.h"
#include "key.h"
#include "delay.h"

unsigned int k;     //全局变量，用于记录KEY1按键的状态或者按下次数（1或2）

void KEY_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);   //时钟使能
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;         //PA0(WK_UP)
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;     //下拉输入模式
    GPIO_Init(GPIOA,&GPIO_InitStructure);           //初始化

    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;  //PA0(K1、K2)
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;         //上拉输入模式
    GPIO_Init(GPIOC,&GPIO_InitStructure);               //初始化

}

// 按键扫描函数，支持两种模式：
// mode=1：支持连按（按住不松开时可重复触发）
// mode=0：不支持连按（需松开按键后才能再次触发）
// 返回值：0（无按键按下），或KEY1_PRES、KEY2_PRES、WKUP_PRES（对应按键被按下）
u8 KEY_Scan(u8 mode){
    static u8 key_up=1;     //key_up=1代表没有按键触发

    if(mode){
        key_up=1;
    }

    if(key_up&&(KEY1==0||KEY2==0||WK_UP==1)){
        key_up=0;
        delay_xms(10);      //延时消抖
        if(KEY1==0){
            return KEY1_PRES;
        }else if(KEY2==0){
            return KEY2_PRES;
        }if(WKUP_PRES==1){
            return WKUP_PRES;
        }

    }else if(KEY1==1||KEY2==1||WK_UP==0){
        key_up=1;
    }

    return 0;
    
}


void key_sta_charg(void){
    if(KEY1==0){
        delay_xms(5);
        if(KEY==0){
            k++;
			delay_xms(5);
			if(k>2)
			k=1;
        }
    }
}
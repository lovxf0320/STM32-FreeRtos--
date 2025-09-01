/**
  ******************************************************************************
  * @file    main.c
  * @author  用户
  * @version V1.0.0
  * @date    2024
  * @brief   基于STM32F103ZE和FreeRTOS的主程序文件
  * @note    该程序实现了多任务系统，包含传感器读取、电机控制、通信等功能
  ******************************************************************************
  * @attention
  *
  * 本程序基于STM32F103ZE微控制器和FreeRTOS实时操作系统开发
  * 主要功能包括：
  * - 多任务调度管理
  * - 传感器数据采集（MPU6050陀螺仪、编码器等）
  * - 电机控制（PWM控制、步进电机等）
  * - 通信功能（USART、ESP8266 WiFi模块等）
  * - 人机交互（LCD显示、按键输入、LED指示等）
  *
  ******************************************************************************
  */

/* 标准库头文件包含 */
#include "stm32f10x.h"    // STM32F10x标准外设库头文件
#include "stdio.h"         // 标准输入输出库

/* 内存管理相关头文件 */
#include "malloc.h"        // 动态内存分配
#include "string.h"        // 字符串处理函数

/* FreeRTOS实时操作系统头文件 */
#include "FreeRTOS.h"      // FreeRTOS核心头文件
#include "task.h"          // 任务管理相关
#include "semphr.h"        // 信号量相关

/* 用户自定义任务头文件 */
#include "my_task.h"       // 用户任务定义

/* 系统功能模块头文件 */
#include "sys.h"           // 系统配置和初始化
#include "delay.h"         // 延时函数
#include "led.h"           // LED控制
#include "key.h"           // 按键处理
#include "lcd.h"           // LCD显示控制
#include "usart.h"         // 串口通信

/* ESP8266 WiFi模块相关头文件 */
#include "esp8266.h"       // ESP8266 WiFi模块驱动
#include "usart2.h"        // USART2串口配置

/* MPU6050陀螺仪相关头文件 */
#include "mpu6050.h"       // MPU6050传感器驱动
#include "inv_mpu.h"       // InvenSense MPU库
#include "inv_mpu_dmp_motion_driver.h"  // DMP运动驱动库

/* 硬件控制相关头文件 */
#include "encoder.h"       // 编码器读取
#include "pwm.h"           // PWM信号生成
#include "timer.h"         // 定时器配置

/* 电机控制相关头文件 */
#include "begstp.h"        // 步进电机控制
#include "motor.h"         // 直流电机控制

/* 遥控器相关头文件 */
#include "remote_ctrl.h"   // 遥控器信号处理



int main(void){
  //float pitch, roll, yaw;
  //char pitch_str[20];
  //char roll_str[20];
  //char yaw_str[20];	  //欧拉角
  SystemInit();
  my_mem_init(SRAMIN);  //初始化内存池
  time7_init();         //初始化定时器4
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  USART_Config();
  usart2_init(1152000); //初始化USART2
  delay_init();         //初始化延时
  LED_Init();           //初始化LED
  LCD_Init();           //初始化LCD
  LCD_Clear(WHITE);

  esp8266_start_trans();

  Encoder_Count_Init(); //初始化编码器

  MPU_Init();          //初始化MPU6050

  while(mpu_dmp_init()){
    delay_ms(20);
    printf("初始化失败");
  }

  USART2_RX_STA=0;

  LCD_Clear(BLACK);
  begstp_Init();
  forward();
  TIM1_PWM_Init();
  //time6_init();     //初始化定时器2

  FreeRTOS_Task_Start();

  return 0;
}
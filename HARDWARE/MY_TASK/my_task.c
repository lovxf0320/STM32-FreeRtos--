#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include "key.h"

#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "my_task.h"

#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usart.h"

//********************ESP8266相关******************************
#include "usart2.h"
#include "esp8266.h"
//*********************ESP8266相关******************************

//********************MPU6050陀螺仪相关******************************
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "mpu6050.h"

//********************MPU6050陀螺仪相关******************************

#include "encoder.h"
#include "pwm.h"
#include "time.h"

#include "begstp.h"
#include "motor.h"

#include "remote_ctrl.h"

//查询任务状态
char task_buff[500];            
char task_time_buff[500];       

//定义任务句柄
TaskHandle_t StartTask_Handler;     //开始任务
TaskHandle_t LEDTask_Handler;       //LED任务
TaskHandle_t UITask_Handler;        //UI任务
TaskHandle_t SPEEDTask_Handler;     //测速任务
TaskHandle_t MOTOTask_Handler;      //电机任务
TaskHandle_t USART2Task_Handler;    //串口二任务
TaskHandle_t LCDTask_Handler;       //LCD任务
//二值信号量句柄
SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄

void FreeRTOS_Task_Start(void){
    //创建开始任务
    xTaskCreate(
        (TaskFunction_t)start_task,         //回调函数
        (const char*)"start_task",          //任务名称
        (uint16_t)START_TASK_SIZE,          //任务堆栈的大小
        (void*)NULL,                        //任务传入的参数
        (UBaseType_t)START_TASK_PRIO,       //任务优先级
        (TaskHandle_t*)&StartTask_Handler,  //任务句柄
    );

    vTaskStartScheduler();
}

void start_task(void *pvParameters){
    taskENTER_CRITICAL();

    BinarySemaphore=xSemaphoreCreateBinary();   //创建二值信号量，
    xSemaphoreTake(BinarySemaphore,0);          //初始化清零信号量，确保可用

    //创建LED任务，用于心跳灯检测系统运行状态
    xTaskCreate(
        (TaskFunction_t)led_task,         //回调函数
        (const char*)"led_task",          //任务名称
        (uint16_t)LED_TASK_SIZE,          //任务堆栈的大小
        (void*)NULL,                        //任务传入的参数
        (UBaseType_t)LED_TASK_PRIO,       //任务优先级
        (TaskHandle_t*)&LEDTask_Handler,  //任务句柄
    );

    //创建UI任务，用于显示或者交互
    xTaskCreate(
        (TaskFunction_t)ui_task,         //回调函数
        (const char*)"ui_task",          //任务名称
        (uint16_t)UI_TASK_SIZE,          //任务堆栈的大小
        (void*)NULL,                        //任务传入的参数
        (UBaseType_t)UI_TASK_PRIO,       //任务优先级
        (TaskHandle_t*)&UITask_Handler,  //任务句柄
    );

    //创建测速任务，用于检测轮子速度
    xTaskCreate(
        (TaskFunction_t)speed_task,         //回调函数
        (const char*)"speed_task",          //任务名称
        (uint16_t)SPEED_TASK_SIZE,          //任务堆栈的大小
        (void*)NULL,                        //任务传入的参数
        (UBaseType_t)SPEED_TASK_PRIO,       //任务优先级
        (TaskHandle_t*)&SPEEDTask_Handler,  //任务句柄
    );

    //创建电机任务
    xTaskCreate(
        (TaskFunction_t)moto_task,         //回调函数
        (const char*)"moto_task",          //任务名称
        (uint16_t)MOTO_TASK_SIZE,          //任务堆栈的大小
        (void*)NULL,                        //任务传入的参数
        (UBaseType_t)MOTO_TASK_PRIO,       //任务优先级
        (TaskHandle_t*)&MOTOTask_Handler,  //任务句柄
    );

    // 创建WiFi控制任务，通过串口2与ESP8266通信
    xTaskCreate(
        (TaskFunction_t)usart2_task,
        (const char*)"usart2_task",
        (uint16_t)USART2_STK_SIZE, 
        (void*)NULL,
        (UBaseType_t)USART2_TASK_PRIO,
        (TaskHandle_t*)&USART2Task_Handler
    ); 

    // 创建LCD显示任务，用于更新LCD屏幕内容
    xTaskCreate(
        (TaskFunction_t)lcd_task,
        (const char*)"lcd_task",
        (uint16_t)LCD_STK_SIZE, 
        (void*)NULL,
        (UBaseType_t)LCD_TASK_PRIO,
        (TaskHandle_t*)&LCDTask_Handler
    );

    vTaskDelet(StartTask_Handler);
    taskEXIT_CRITICAL();
}


//******************************************
// 函数名称：usart2_task
// 函数功能：WiFi控制任务，通过串口2与ESP8266模块通信
// 参数：pvParameters - 任务参数（此处未使用）
// 返回值：无
//******************************************
void usart2_task(void *pvParameters){
    while(1){
        //等待二值信号量，无限期阻塞直到信号量可用
        xSemaphoreTake(BinarySemaphore,portMAX_DELAY);

        //调用WiFi控制函数，通过串口2与ESP8266通信
        USART2_WIFI_CLRT();
        vTaskDelay(1);
    }
}


//******************************************
// 函数名称：lcd_task
// 函数功能：LCD显示任务，实时更新LCD屏幕上的陀螺仪数据（pitch, roll, yaw）
// 参数：pvParameters - 任务参数（此处未使用）
// 返回值：无
//******************************************
void lcd_task(void *pvParameters){
    while(1){
        //格式化陀螺仪数据（pitch, roll, yaw）为字符串
        sprintf(pitch_str,"pitch:%.2f",pitch);
        sprintf(roll_str,"roll:%.2f",roll);
        sprintf(yaw_str,"yaw:%.2f",yaw);

        //在LCD上显示格式化的字符串
        LCD_DrawFont_GBK24(0, 10, BLACK, WHITE, (u8 *)pitch_str);
        LCD_DrawFont_GBK24(0, 40, BLACK, WHITE, (u8 *)roll_str);
        LCD_DrawFont_GBK24(0, 70, BLACK, WHITE, (u8 *)yaw_str);
        vTaskDelay(1); // 延时1个tick，释放CPU给其他任务
    }
}


//******************************************
// 函数名称：led_task
// 函数功能：LED任务，实现心跳灯功能，用于检测系统是否正常运行
// 参数：pvParameters - 任务参数（此处未使用）
// 返回值：无
//******************************************
void led_task(void *pvParameters){
    while(1){
        // LED0=~LED0;     //反转LED0状态，实现心跳灯
        // Feed_dog();     //喂狗
        // ADC_Check();    //检查ADC状态
        vTaskDelay(1);  //延时1个tick
    }
}


u8 u=0;     //定义全局变量u，可以用于UI任务的计数或状态（用途未明确）
//******************************************
// 函数名称：ui_task
// 函数功能：UI任务，控制界面显示并可能包含交互逻辑
// 参数：pvParameters - 任务参数（此处未使用）
// 返回值：无
//******************************************
void ui_task(void *pvParameters){
    while(1){
        LED0 = !LED0; // 翻转LED0状态，可能用于指示UI任务运行
        UI();         // 调用UI函数，处理界面逻辑
        vTaskDelay(2); // 延时2个tick，释放CPU给其他任务
    }
}


//******************************************
// 函数名称：speed_task
// 函数功能：测速任务，实时读取陀螺仪数据和电机速度，控制平衡车运动
// 参数：pvParameters - 任务参数（此处未使用）
// 返回值：无
//******************************************
u8 times = 0; // 定义计数变量，用于控制测速频率
void speed_task(void *pvParameters){
    while(1){
        timer++;                //计数器增加
        MPU6050_Data_read();    //读取MPU6050陀螺仪数据
        if(timer>=5){
            times=0;
            Moto_Speed_Read(1); //读取电机1速度
            Moto_Speed_Read(2); //读取电机2速度
        }
        Smart_Car_Task();       //执行平衡车运动控制任务
        vTaskDelay(1);
    }
}


//******************************************
// 函数名称：moto_task
// 函数功能：电机控制任务，负责电机相关操作（当前功能未实现）
// 参数：pvParameters - 任务参数（此处未使用）
// 返回值：无
//******************************************
void moto_task(void *pvParameters)
{
    while(1)
    {    
        vTaskDelay(10); // 延时10个tick，释放CPU给其他任务
    }
}
#ifndef __MY_TASK_H
#define __MY_TASK_H	 
//#include "pbdata.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
//*******任务优先级0-31,越大越优先*******************
#define START_TASK_PRIO		  1
#define LED_TASK_PRIO		  2
#define MOTO_TASK_PRIO		  2
#define LCD_TASK_PRIO         3
#define UI_TASK_PRIO		  4
#define USART2_TASK_PRIO      4
#define SPEED_TASK_PRIO		  5

//*******任务堆栈大小	单位字 *4的单位为字节 1024字节为1KB**********
#define START_STK_SIZE 		 100     
#define LED_STK_SIZE 		 500  
#define UI_STK_SIZE          500  
#define SPEED_STK_SIZE       1000 
#define MOTO_STK_SIZE 	     500
#define USART2_STK_SIZE      500
#define LCD_STK_SIZE         500

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

//********任务函数*******************
void FreeRTOS_Task_Start(void);
void start_task(void *pvParameters);
void led_task(void *pvParameters);
void ui_task(void *pvParameters);	
void speed_task(void *pvParameters);	
void moto_task(void *pvParameters);
void usart2_task(void *pvParameters);	
void lcd_task(void *pvParameters);	
void api_task(void *pvParameters);
void apitime_task(void *pvParameters);

#endif
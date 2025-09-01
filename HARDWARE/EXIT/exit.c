// #include "exit.h"
// #include "led.h"
// #include "key.h"
// #include "delay.h"
// #include "FreeRTOS.h"
// #include "task.h"


// //外部中断0服务程序
// void EXITX_Init(void){
//     EXIT_InitTypeDef EXIT_InitStructure;        //定义外部中断初始化结构体
//     NVIC_InitTypeDef NVIC_InitStructure;        //定义嵌套向量中断控制器初始化结构体

//     KEY_Init();                                             //按键端口初始化
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);     //使能复用功能时钟

//     //GPIOE .4中断线以及中断初始化配置  下降沿触发  //KEY0
//     GPIO_EXITLineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);

//     EXTI_InitStructure.EXTI_Line = EXTI_Line4;                   //选择外部中断线4
//     EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	        //设置为中断模式（而不是事件模式）
//     EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;      //配置为下降沿触发（即按键从高电平变为低电平时触发中断）
//     EXTI_InitStructure.EXTI_LineCmd = ENABLE;                    //启用中断线
//     EXTI_Init(&EXTI_InitStructure);                              //将配置写入EXTI寄存器，完成中断线配置

//     NVIC_InitStructure.NVIC_IRQChannel=EXIT4_IRQn;               //使能按键KEY0所在的外部中断通道
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;	//抢占优先级6
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//子优先级0 
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
//     NVIC_Init(&NVIC_InitStructure);
// }


// //任务句柄
// extern TaskHandle_t Task2Task_Handle;

// //外部中断4服务程序
// void EXIT4_IRQHandler(void){
//     BaseType_t YieldRequired;

//     delay_xms(20);      //消抖
//     if(KEY0==0){
//         YieldRequired=xTaskResumeFromISR(Task2Task_Handle);
//         printf("恢复任务2的运行!\r\n");

//         if(YieldRequired==pdTRUE){
//             portYIELD_FROM_ISR(YieldRequired);
//         }
//     }

//     //@brief  Clears the EXTI's line pending bits.
//     EXTI_ClearITPendingBit(EXTI_Line4);         //清除LINE4上的中断标志位
// }
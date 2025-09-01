/**
  ******************************************************************************
  * @file    stm32f10x_it.c
  * @author  用户
  * @version V1.0.0
  * @date    2024
  * @brief   STM32F10x中断服务程序文件
  * @note    包含所有中断处理函数的实现
  ******************************************************************************
  * @attention
  *
  * 本文件包含STM32F10x微控制器的所有中断服务程序
  * 主要中断包括：
  * - 系统异常中断（NMI、HardFault、MemManage等）
  * - 系统滴答定时器中断（SysTick）
  * - 外设中断（USART、Timer、EXTI等）
  *
  ******************************************************************************
  */

/* 头文件包含 */
#include "stm32f10x_it.h"
#include "FreeRTOS.h"
#include "task.h"

/**
  * @brief  不可屏蔽中断服务程序
  * @param  None
  * @retval None
  * @note   处理不可屏蔽中断，通常用于系统严重错误
  */
void NMI_Handler(void)
{
    /* 不可屏蔽中断处理 */
    while(1)
    {
        /* 系统错误，进入死循环 */
    }
}

/**
  * @brief  硬件故障中断服务程序
  * @param  None
  * @retval None
  * @note   处理硬件故障，如总线错误、内存访问错误等
  */
void HardFault_Handler(void)
{
    /* 硬件故障中断处理 */
    while(1)
    {
        /* 硬件错误，进入死循环 */
    }
}

/**
  * @brief  内存管理故障中断服务程序
  * @param  None
  * @retval None
  * @note   处理内存管理单元（MPU）相关的故障
  */
void MemManage_Handler(void)
{
    /* 内存管理故障中断处理 */
    while(1)
    {
        /* 内存管理错误，进入死循环 */
    }
}

/**
  * @brief  总线故障中断服务程序
  * @param  None
  * @retval None
  * @note   处理总线相关的故障，如AHB、APB总线错误
  */
void BusFault_Handler(void)
{
    /* 总线故障中断处理 */
    while(1)
    {
        /* 总线错误，进入死循环 */
    }
}

/**
  * @brief  用法故障中断服务程序
  * @param  None
  * @retval None
  * @note   处理指令执行相关的故障，如未定义指令、非法状态等
  */
void UsageFault_Handler(void)
{
    /* 用法故障中断处理 */
    while(1)
    {
        /* 用法错误，进入死循环 */
    }
}

/**
  * @brief  软件中断服务程序
  * @param  None
  * @retval None
  * @note   处理软件产生的中断，通常用于系统调用
  */
void SVC_Handler(void)
{
    /* 软件中断处理 */
}

/**
  * @brief  调试监视器中断服务程序
  * @param  None
  * @retval None
  * @note   处理调试相关的监视器中断
  */
void DebugMon_Handler(void)
{
    /* 调试监视器中断处理 */
}

/**
  * @brief  挂起服务中断服务程序
  * @param  None
  * @retval None
  * @note   FreeRTOS上下文切换中断，由FreeRTOS自动处理
  */
void PendSV_Handler(void)
{
    /* FreeRTOS上下文切换中断处理 */
    /* 此函数由FreeRTOS自动处理，无需用户干预 */
}

/**
  * @brief  系统滴答定时器中断服务程序
  * @param  None
  * @retval None
  * @note   处理系统滴答定时器中断，为FreeRTOS提供时间基准
  */
void SysTick_Handler(void)
{
    /* 系统滴答定时器中断处理 */
    /* 为FreeRTOS提供时间基准 */
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}

/**
  * @brief  USART1全局中断服务程序
  * @param  None
  * @retval None
  * @note   处理USART1的接收、发送、错误等中断
  */
void USART1_IRQHandler(void)
{
    /* USART1中断处理 */
    /* TODO: 添加串口数据处理代码 */
}

/**
  * @brief  USART2全局中断服务程序
  * @param  None
  * @retval None
  * @note   处理USART2的中断，通常用于ESP8266通信
  */
void USART2_IRQHandler(void)
{
    /* USART2中断处理 */
    /* TODO: 添加ESP8266通信处理代码 */
}

/**
  * @brief  定时器2全局中断服务程序
  * @param  None
  * @retval None
  * @note   处理定时器2的中断，可用于PWM生成或定时任务
  */
void TIM2_IRQHandler(void)
{
    /* 定时器2中断处理 */
    /* TODO: 添加定时器中断处理代码 */
}

/**
  * @brief  外部中断线0中断服务程序
  * @param  None
  * @retval None
  * @note   处理外部中断线0的中断，通常用于按键输入
  */
void EXTI0_IRQHandler(void)
{
    /* 外部中断线0中断处理 */
    /* TODO: 添加按键中断处理代码 */
    
    /* 清除中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line0);
}

/**
  * @brief  外部中断线1中断服务程序
  * @param  None
  * @retval None
  * @note   处理外部中断线1的中断
  */
void EXTI1_IRQHandler(void)
{
    /* 外部中断线1中断处理 */
    /* TODO: 添加外部中断1处理代码 */
    
    /* 清除中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line1);
}

/**
  * @brief  外部中断线2中断服务程序
  * @param  None
  * @retval None
  * @note   处理外部中断线2的中断
  */
void EXTI2_IRQHandler(void)
{
    /* 外部中断线2中断处理 */
    /* TODO: 添加外部中断2处理代码 */
    
    /* 清除中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line2);
}

/**
  * @brief  外部中断线3中断服务程序
  * @param  None
  * @retval None
  * @note   处理外部中断线3的中断
  */
void EXTI3_IRQHandler(void)
{
    /* 外部中断线3中断处理 */
    /* TODO: 添加外部中断3处理代码 */
    
    /* 清除中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line3);
}

/**
  * @brief  外部中断线4中断服务程序
  * @param  None
  * @retval None
  * @note   处理外部中断线4的中断
  */
void EXTI4_IRQHandler(void)
{
    /* 外部中断线4中断处理 */
    /* TODO: 添加外部中断4处理代码 */
    
    /* 清除中断标志位 */
    EXTI_ClearITPendingBit(EXTI_Line4);
}

/**
  * @brief  外部中断线9-5中断服务程序
  * @param  None
  * @retval None
  * @note   处理外部中断线9-5的中断
  */
void EXTI9_5_IRQHandler(void)
{
    /* 外部中断线9-5中断处理 */
    /* TODO: 添加外部中断9-5处理代码 */
    
    /* 清除中断标志位 */
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
    if(EXTI_GetITStatus(EXTI_Line8) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

/**
  * @brief  外部中断线15-10中断服务程序
  * @param  None
  * @retval None
  * @note   处理外部中断线15-10的中断
  */
void EXTI15_10_IRQHandler(void)
{
    /* 外部中断线15-10中断处理 */
    /* TODO: 添加外部中断15-10处理代码 */
    
    /* 清除中断标志位 */
    if(EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    if(EXTI_GetITStatus(EXTI_Line11) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    if(EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if(EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if(EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}

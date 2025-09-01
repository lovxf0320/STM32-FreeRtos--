/**
  ******************************************************************************
  * @file    stm32f10x_it.h 
  * @author  用户
  * @version V1.0.0
  * @date    2024
  * @brief   本文件包含中断处理函数的声明
  * @note    基于STM32F10x微控制器和FreeRTOS实时操作系统
  ******************************************************************************
  * @attention
  *
  * 本文件定义了STM32F10x微控制器的所有中断服务程序函数声明
  * 包括系统异常中断、外设中断等
  * 与FreeRTOS兼容，支持实时任务调度
  *
  ******************************************************************************
  */

/* 防止重复包含 */
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

/* 包含的头文件 */
#include "stm32f10x.h"

/* 导出的类型定义 */
/* 导出的常量定义 */
/* 导出的宏定义 */

/* 导出的函数声明 */

/**
  * @brief  不可屏蔽中断服务程序
  * @param  None
  * @retval None
  * @note   处理不可屏蔽中断，通常用于系统严重错误
  */
void NMI_Handler(void);

/**
  * @brief  硬件故障中断服务程序
  * @param  None
  * @retval None
  * @note   处理硬件故障，如总线错误、内存访问错误等
  */
void HardFault_Handler(void);

/**
  * @brief  内存管理故障中断服务程序
  * @param  None
  * @retval None
  * @note   处理内存管理单元（MPU）相关的故障
  */
void MemManage_Handler(void);

/**
  * @brief  总线故障中断服务程序
  * @param  None
  * @retval None
  * @note   处理总线相关的故障，如AHB、APB总线错误
  */
void BusFault_Handler(void);

/**
  * @brief  用法故障中断服务程序
  * @param  None
  * @retval None
  * @note   处理指令执行相关的故障，如未定义指令、非法状态等
  */
void UsageFault_Handler(void);

/**
  * @brief  软件中断服务程序
  * @param  None
  * @retval None
  * @note   处理软件产生的中断，通常用于系统调用
  */
void SVC_Handler(void);

/**
  * @brief  调试监视器中断服务程序
  * @param  None
  * @retval None
  * @note   处理调试相关的监视器中断
  */
void DebugMon_Handler(void);

/**
  * @brief  挂起服务中断服务程序
  * @param  None
  * @retval None
  * @note   FreeRTOS上下文切换中断，由FreeRTOS自动处理
  */
void PendSV_Handler(void);

/**
  * @brief  系统滴答定时器中断服务程序
  * @param  None
  * @retval None
  * @note   处理系统滴答定时器中断，为FreeRTOS提供时间基准
  */
void SysTick_Handler(void);

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

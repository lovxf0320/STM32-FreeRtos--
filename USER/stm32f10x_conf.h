/**
  ******************************************************************************
  * @file    stm32f10x_conf.h 
  * @author  用户
  * @version V1.0.0
  * @date    2024
  * @brief   库配置文件
  * @note    配置STM32F10x标准外设库中启用的外设模块
  ******************************************************************************
  * @attention
  *
  * 本文件用于配置STM32F10x标准外设库中启用的外设模块
  * 通过注释/取消注释相应的行来启用/禁用特定外设
  * 只包含实际使用的外设可以减小代码体积
  *
  ******************************************************************************
  */

/* 防止重复包含 */
#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

/* 包含的头文件 */
/* 注释/取消注释下面的行来启用/禁用外设头文件包含 */

/* 模拟数字转换器 */
#include "stm32f10x_adc.h"      // ADC模数转换器
/* 备份寄存器 */
#include "stm32f10x_bkp.h"      // 备份寄存器和RTC校准
/* 控制器局域网 */
#include "stm32f10x_can.h"      // CAN总线控制器
/* 消费电子控制 */
#include "stm32f10x_cec.h"      // 消费电子控制接口
/* 循环冗余校验 */
#include "stm32f10x_crc.h"      // CRC计算单元
/* 数字模拟转换器 */
#include "stm32f10x_dac.h"      // DAC数模转换器
/* 调试微控制器 */
#include "stm32f10x_dbgmcu.h"   // 调试MCU支持
/* 直接内存访问 */
#include "stm32f10x_dma.h"      // DMA控制器
/* 外部中断/事件控制器 */
#include "stm32f10x_exti.h"     // 外部中断线
/* 闪存编程 */
#include "stm32f10x_flash.h"    // 闪存控制器
/* 灵活的静态存储控制器 */
#include "stm32f10x_fsmc.h"     // FSMC控制器
/* 通用输入输出 */
#include "stm32f10x_gpio.h"     // GPIO端口
/* 内部集成电路 */
#include "stm32f10x_i2c.h"      // I2C接口
/* 独立看门狗 */
#include "stm32f10x_iwdg.h"     // 独立看门狗
/* 电源控制 */
#include "stm32f10x_pwr.h"      // 电源控制
/* 复位和时钟控制 */
#include "stm32f10x_rcc.h"      // RCC时钟控制
/* 实时时钟 */
#include "stm32f10x_rtc.h"      // RTC实时时钟
/* 安全数字输入输出接口 */
#include "stm32f10x_sdio.h"     // SDIO接口
/* 串行外设接口 */
#include "stm32f10x_spi.h"      // SPI接口
/* 通用定时器 */
#include "stm32f10x_tim.h"      // 定时器
/* 通用同步异步收发器 */
#include "stm32f10x_usart.h"    // USART串口
/* 窗口看门狗 */
#include "stm32f10x_wwdg.h"     // 窗口看门狗

/* 高级功能 */
#include "misc.h"                // NVIC和SysTick的高级函数（CMSIS函数的附加功能）

/* 导出的类型定义 */
/* 导出的常量定义 */

/* 导出的宏定义 */
/* 注释下面的行来扩展"assert_param"宏在标准外设库驱动代码中的使用 */
/* #define USE_FULL_ASSERT    1 */

#ifdef  USE_FULL_ASSERT

/**
  * @brief  assert_param宏用于函数参数检查
  * @param  expr: 如果expr为假，它调用assert_failed函数，该函数报告
  *         调用失败的源文件名和源行号。如果expr为真，则不返回值
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))

/* 导出的函数 */
  void assert_failed(uint8_t* file, uint32_t line);

#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __STM32F10x_CONF_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

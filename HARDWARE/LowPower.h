/**
  ******************************************************************************
  * @file   LowPower.h
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Header File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#ifndef __LOW_POWER_H
#define __LOW_POWER_H	 

#include "led.h"
#include "stm32f4xx.h"
#include <stdbool.h>
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"


/* FreeRTOS空闲钩子函数 -------------------------------------------------------
 * 功能：在系统空闲任务中自动进入低功耗模式
 * 典型实现：
 *     1. 关闭外设时钟
 *     2. 调用WFI/WFE指令
 *     3. 唤醒后恢复时钟
 * 注意事项：
 *     需在FreeRTOSConfig.h中配置configUSE_IDLE_HOOK=1
 *----------------------------------------------------------------------------*/
void vApplicationIdleHook( void );

/* RTC基础配置 ----------------------------------------------------------------
 * 配置内容：
 *     1. 启用PWR和BKP域时钟
 *     2. 选择LSE(32.768KHz)作为RTC时钟源
 *     3. 初始化日期时间(默认值)
 *     4. 使能RTC唤醒中断
 * 注意：
 *     需提前焊接外部低速晶振
 *----------------------------------------------------------------------------*/
void RTC_Config(void);

/* 外设节能管理 ---------------------------------------------------------------
 * 功能：禁用所有非必要外设时钟
 * 关闭的外设包括：
 *     - 所有GPIO端口时钟(唤醒后需重新使能)
 *     - ADC/DAC
 *     - 定时器(TIM1-14)
 *     - 通信接口(USART/I2C/SPI)
 * 保留的外设：
 *     - RCC
 *     - RTC
 *     - EXTI
 *----------------------------------------------------------------------------*/
void LowPower_DisableAllPeripherals(void);

/* RTC唤醒时间设置 ------------------------------------------------------------
 * 参数说明：
 *     hour   : 0-23小时制 
 *     minute : 0-59分钟
 *     second : 0-59秒
 * 实现原理：
 *     1. 计算当前时间到目标时间的秒数差
 *     2. 配置RTC唤醒计数器
 *     3. 使能EXTI线22中断
 * 注意：
 *     唤醒后系统时钟会复位，需重新配置
 *----------------------------------------------------------------------------*/
void RTC_SetWakeup_HMS(uint16_t hour, uint16_t minute, uint16_t second);

/* FreeRTOS任务块延迟管理接口 --------------------------------------------------
 * 功能说明：
 *     vPortProcessPendingBlocks  : 处理延迟的任务块(唤醒后立即调用)
 *     xPortGetPendingBlockCount  : 获取挂起任务块数量(用于唤醒决策)
 *     vPortSetDeferredCoalescing : 启用/禁用延迟合并(TRUE启用节电模式)
 * 使用场景：
 *     进入停止模式前禁用延迟合并，唤醒后处理积压任务块
 *----------------------------------------------------------------------------*/
extern void vPortProcessPendingBlocks(void);                // 处理挂起块
extern size_t xPortGetPendingBlockCount( void );            // 获取挂起计数 
extern void vPortSetDeferredCoalescing( BaseType_t xEnable ); // 延迟合并控制
#endif



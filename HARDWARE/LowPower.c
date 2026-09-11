/**
  ******************************************************************************
  * @file    LowPower.c
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Source Code File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#include "LowPower.h"


// 低功耗模式控制标志（注意volatile确保内存可见性）
volatile uint8_t isReadyForStandby = 0;   // 0-睡眠模式(Sleep) | 1-待机模式(Standby)
volatile uint16_t Hour=2;    // 唤醒时间-小时（默认2小时） 
volatile uint16_t Minute=0;  // 唤醒时间-分钟
volatile uint16_t Second=0;  // 唤醒时间-秒

/**
 * 函数功能：RTC时钟初始化（仅在系统启动时执行一次）
 * 关键点：
 * - 使用外部低速晶振(LSE 32.768kHz)
 * - 配置为24小时制
 * - 异步分频127，同步分频255 → 1Hz时基
 */
void RTC_Config(void)
{
    // 1. 启用PWR和备份域访问
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);  // 开启电源控制时钟
    PWR_BackupAccessCmd(ENABLE);  // 允许访问备份域寄存器
    
    // 2. 初始化LSE晶振（需硬件连接32.768kHz晶体）
    RCC_LSEConfig(RCC_LSE_ON);  // 开启LSE时钟
    while (!RCC_GetFlagStatus(RCC_FLAG_LSERDY));  // 等待LSE稳定
    
    // 3. 配置RTC时钟源
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  // 选择LSE作为RTC时钟源
    RCC_RTCCLKCmd(ENABLE);   // 使能RTC时钟
    RTC_WaitForSynchro();    // 等待RTC寄存器同步
    
    // 4. RTC基本参数配置
    RTC_InitTypeDef rtcInit; 
    rtcInit.RTC_HourFormat = RTC_HourFormat_24;  // 24小时制
    rtcInit.RTC_AsynchPrediv = 127;   // 异步预分频系数（LSE/(127+1)=256Hz）
    rtcInit.RTC_SynchPrediv  = 255;   // 同步预分频系数（256/(255+1)=1Hz）
    RTC_Init(&rtcInit);  // 应用配置
}

/**
 * 函数功能：关闭所有外设以降低功耗
 * 设计要点：
 * - 将所有GPIO设为模拟模式（最低功耗）
 * - 禁用所有外设时钟（除必要外设）
 */
void LowPower_DisableAllPeripherals(void)
{
    // 步骤1：启用所有GPIO组的时钟以配置引脚
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | 
                          RCC_AHB1Periph_GPIOB |
                          RCC_AHB1Periph_GPIOC |
                          RCC_AHB1Periph_GPIOD |
                          RCC_AHB1Periph_GPIOE |
                          RCC_AHB1Periph_GPIOH, ENABLE);

    // 步骤2：配置所有GPIO为模拟模式+无上拉/下拉
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode  = GPIO_Mode_AN;    // 模拟模式（最低功耗）
    gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL; // 禁用上下拉电阻
    gpio.GPIO_Speed = GPIO_Speed_2MHz; // 低速模式
    gpio.GPIO_Pin   = GPIO_Pin_All;    // 所有引脚

    // 批量初始化所有GPIO端口
    GPIO_Init(GPIOA, &gpio);
    GPIO_Init(GPIOB, &gpio);
    GPIO_Init(GPIOC, &gpio);
    GPIO_Init(GPIOD, &gpio);
    GPIO_Init(GPIOE, &gpio);
    GPIO_Init(GPIOH, &gpio);

    /* 步骤3：关闭所有外设时钟*/
    // APB1总线外设（低速率外设）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | 
                          RCC_APB1Periph_TIM3 |
                          RCC_APB1Periph_TIM4 |
                          RCC_APB1Periph_TIM5 |
                          RCC_APB1Periph_CAN1 |
                          RCC_APB1Periph_I2C1 |
                          RCC_APB1Periph_I2C2 |
                          RCC_APB1Periph_USART2 |  
                          RCC_APB1Periph_USART3 |
                          RCC_APB1Periph_UART4 |
                          RCC_APB1Periph_UART5, DISABLE);

    // APB2总线外设（高速外设）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 |
                          RCC_APB2Periph_ADC2 |
                          RCC_APB2Periph_ADC3 |
                          RCC_APB2Periph_TIM1 |
                          RCC_APB2Periph_TIM8 |
                          RCC_APB2Periph_TIM9 |
                          RCC_APB2Periph_TIM10 |
                          RCC_APB2Periph_TIM11 |
                          RCC_APB2Periph_SPI1 |
                          RCC_APB2Periph_USART1, DISABLE);

    // AHB1总线外设（DMA等）
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 |
                          RCC_AHB1Periph_DMA2, DISABLE);
}

/**
 * 函数功能：配置RTC唤醒参数
 * 参数说明：
 *   hour    - 唤醒时间小时（0-18，超限自动设最大值）
 *   minute  - 唤醒时间分钟
 *   second  - 唤醒时间秒
 * 注意：
 * - 使用RTC WakeUp定时器，最大支持18小时
 * - 唤醒后系统会复位重启
 */
void RTC_SetWakeup_HMS(uint16_t hour, uint16_t minute, uint16_t second)
{
    // 将时间转换为总秒数（防止溢出）
    uint32_t total_seconds = hour * 3600 + minute * 60 + second;
    if (total_seconds > 65535)  // RTC WakeUp计数器的最大值
        total_seconds = 65535;  // 对应约18小时12分钟

    // 清除之前的中断标志
    EXTI_ClearITPendingBit(EXTI_Line22);  // 清除EXTI线22中断标志
    RTC_ClearITPendingBit(RTC_IT_WUT);    // 清除RTC WakeUp中断标志
    RTC_WakeUpCmd(DISABLE);              // 先禁用WakeUp功能

    // 配置WakeUp时钟源和计数值
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits); // 1Hz时钟
    RTC_SetWakeUpCounter(total_seconds);  // 设置唤醒时间

    // 启用中断通道
    RTC_ITConfig(RTC_IT_WUT, ENABLE);    // 使能RTC WakeUp中断
    RTC_WakeUpCmd(ENABLE);              // 使能WakeUp功能

    // 配置EXTI线22连接至RTC WakeUp事件
    EXTI_InitTypeDef exti;
    exti.EXTI_Line = EXTI_Line22;      // 对应RTC WakeUp事件
    exti.EXTI_Mode = EXTI_Mode_Interrupt;  // 中断模式
    exti.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
    exti.EXTI_LineCmd = ENABLE;         // 使能中断线
    EXTI_Init(&exti);                  // 应用配置

    // 使能NVIC中断
    NVIC_EnableIRQ(RTC_WKUP_IRQn);  // 使能RTC WakeUp中断通道
}

/**
 * 函数功能：重新配置系统时钟（退出低功耗后恢复主频）
 * 关键点：
 * - 重新启用HSE外部高速晶振
 * - 配置PLL恢复72MHz主频
 */
void SystemClock_ReConfig(void)
{
    RCC_HSEConfig(RCC_HSE_ON);  // 启动HSE时钟
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET); // 等待HSE就绪
    
    RCC_PLLCmd(ENABLE);        // 启用PLL
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // 等待PLL锁定
    
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // 切换系统时钟到PLL
    while (RCC_GetSYSCLKSource() != 0x08);      // 确认时钟切换完成（0x08表示PLL作为时钟源）
}

/**
 * FreeRTOS空闲任务钩子函数（关键低功耗入口）
 * 行为逻辑：
 * - 根据标志位选择低功耗模式
 * - isReadyForStandby=1时进入Standby模式（深度睡眠）
 * - 默认进入Sleep模式（普通睡眠）
 */
void vApplicationIdleHook( void )
{
    if (isReadyForStandby)  // 待机模式触发条件
    {
        vPortProcessPendingBlocks(); // FreeRTOS PLUS内存管理：处理待合并内存块
        
        LowPower_DisableAllPeripherals();  // 关闭非必要外设
        
        // 配置RTC唤醒定时器
        RTC_SetWakeup_HMS(Hour, Minute, Second);  // 设置唤醒时间
        
        // 清除唤醒标志位
        PWR_ClearFlag(PWR_FLAG_WU);  // 清除唤醒标志
        PWR_ClearFlag(PWR_FLAG_SB);  // 清除待机标志
        RTC_ClearITPendingBit(RTC_IT_WUT); // 再次确认清除RTC标志
        
        // 进入待机模式（将导致系统复位）
        PWR_EnterSTANDBYMode();  // 此函数不会返回
    }
    else  // 普通睡眠模式
    {    
        SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk); // 清除深度睡眠位
        __WFI();  // 进入Sleep模式，等待中断唤醒（唤醒后继续运行）
    }
}

 /*-----------------------------------------------------------*/






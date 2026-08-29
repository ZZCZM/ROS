/**
  ******************************************************************************
  * @file   encoder.h
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
#ifndef __ENCODER_H
#define __ENCODER_H
#include <sys.h>	 
#include "system.h"
/****** 编码器定时器全局配置 ******/
/* 定时器最大计数值（根据STM32F103 16位定时器设定）
   - 正交编码器模式下，每个信号边沿触发计数
   - 溢出处理：启用定时器更新中断进行溢出计数 */
#define ENCODER_TIM_PERIOD (u16)(65535)  // 0xFFFF（实际使用32位扩展计数）

/****** 电机A编码器接口配置（TIM2） ******/
/*-- 相位A信号配置 --*/
#define MOTORA1_PORT    GPIOA        // 相位A GPIO端口
#define MOTORA1_PIN        GPIO_Pin_15  // PA15（物理引脚：DIP64封装Pin42）
#define MOTORA1_PIN_Source GPIO_PinSource15//引脚源编号15 
#define MOTORA1_GPIO_CLK RCC_AHB1Periph_GPIOA  // GPIOA时钟使能（AHB1总线）
#define MOTORA1_ADC_CLK RCC_APB1Periph_TIM2    // TIM2时钟使能（APB1总线，实际应为定时器时钟）
#define MOTORA1_ID TIM2              // 使用TIM2定时器
#define MOTORA1_AF GPIO_AF_TIM2     // 复用功能映射到TIM2_CH1

/*-- 相位B信号配置 --*/
#define MOTORA2_PORT    GPIOB        // 相位B GPIO端口  
#define MOTORA2_PIN        GPIO_Pin_3    // PB3（DIP64 Pin51）
#define MOTORA2_PIN_Source GPIO_PinSource3//引脚源编号3
#define MOTORA2_GPIO_CLK RCC_AHB1Periph_GPIOB  
#define MOTORA2_ADC_CLK RCC_APB1Periph_TIM2    // TIM2时钟复用
#define MOTORA2_ID TIM2              
#define MOTORA2_AF GPIO_AF_TIM2     // TIM2_CH2

/****** 电机B编码器接口配置（TIM3） ******/
/*-- 相位A信号配置 --*/
#define MOTORB1_PORT    GPIOB        // PB4（DIP64 Pin52）
#define MOTORB1_PIN        GPIO_Pin_4    
#define MOTORB1_PIN_Source GPIO_PinSource4//引脚源编号4
#define MOTORB1_GPIO_CLK RCC_AHB1Periph_GPIOB  
#define MOTORB1_ADC_CLK RCC_APB1Periph_TIM3    // TIM3时钟
#define MOTORB1_ID TIM3              
#define MOTORB1_AF GPIO_AF_TIM3     // TIM3_CH1

/*-- 相位B信号配置 --*/
#define MOTORB2_PORT    GPIOB        // PB5（DIP64 Pin53）
#define MOTORB2_PIN        GPIO_Pin_5    
#define MOTORB2_PIN_Source GPIO_PinSource5//引脚源编号5
#define MOTORB2_GPIO_CLK RCC_AHB1Periph_GPIOB  
#define MOTORB2_ADC_CLK RCC_APB1Periph_TIM3    
#define MOTORB2_ID TIM3              
#define MOTORB2_AF GPIO_AF_TIM3     // TIM3_CH2

/****** 电机C编码器接口配置（TIM4） ******/
/*-- 相位A信号配置 --*/
#define MOTORC1_PORT    GPIOB        // PB6（DIP64 Pin54）
#define MOTORC1_PIN        GPIO_Pin_6    
#define MOTORC1_PIN_Source GPIO_PinSource6//引脚源编号6
#define MOTORC1_GPIO_CLK RCC_AHB1Periph_GPIOB  
#define MOTORC1_ADC_CLK RCC_APB1Periph_TIM4    // TIM4时钟
#define MOTORC1_ID TIM4              
#define MOTORC1_AF GPIO_AF_TIM4     // TIM4_CH1

/****** 电机C编码器接口配置（TIM4） ******/
/*-- 相位B信号配置 --*/
#define MOTORC2_PORT    GPIOB        // PB7（DIP64 Pin55）
#define MOTORC2_PIN        GPIO_Pin_7    
#define MOTORC2_PIN_Source GPIO_PinSource7//引脚源编号7
#define MOTORC2_GPIO_CLK RCC_AHB1Periph_GPIOB  
#define MOTORC2_ADC_CLK RCC_APB1Periph_TIM4    // 修正为APB1（原APB2错误）
#define MOTORC2_ID TIM4              
#define MOTORC2_AF GPIO_AF_TIM4     // TIM4_CH2

/****** 电机D编码器接口配置（TIM5 32位定时器） ******/
/*-- 相位A信号配置 --*/
#define MOTORD1_PORT    GPIOA        // PA0（DIP64 Pin10）
#define MOTORD1_PIN        GPIO_Pin_0    
#define MOTORD1_PIN_Source GPIO_PinSource0//引脚源编号0
#define MOTORD1_GPIO_CLK RCC_AHB1Periph_GPIOA  
#define MOTORD1_ADC_CLK RCC_APB1Periph_TIM5    // TIM5时钟（APB1总线）
#define MOTORD1_ID TIM5              // 使用32位定时器
#define MOTORD1_AF GPIO_AF_TIM5     // TIM5_CH1

/*-- 相位B信号配置 --*/
#define MOTORD2_PORT    GPIOA        // PA1（DIP64 Pin11）
#define MOTORD2_PIN        GPIO_Pin_1    
#define MOTORD2_PIN_Source GPIO_PinSource1//引脚源编号1
#define MOTORD2_GPIO_CLK RCC_AHB1Periph_GPIOA  
#define MOTORD2_ADC_CLK RCC_APB1Periph_TIM5    
#define MOTORD2_ID TIM5              
#define MOTORD2_AF GPIO_AF_TIM5     // TIM5_CH2

/****** 编码器初始化函数声明 ******/
void Encoder_Init_TIM2(void);  // 初始化TIM2为编码器模式（电机A）
void Encoder_Init_TIM3(void);  // 初始化TIM3为编码器模式（电机B）
void Encoder_Init_TIM4(void);  // 初始化TIM4为编码器模式（电机C）
void Encoder_Init_TIM5(void);  // 初始化TIM5为32位编码器模式（电机D）

/****** 编码器数据读取接口 ******/
/* 获取扩展32位编码器计数值
   @param TIMX 定时器编号（2:TIM2,3:TIM3,4:TIM4,5:TIM5）
   @return int32_t 累计计数值（溢出处理后的32位数据） */
int Read_Encoder(u8 TIMX); 

/****** 定时器溢出中断处理函数 ******/
void TIM2_IRQHandler(void);  // TIM2溢出中断（电机A）
void TIM3_IRQHandler(void);  // TIM3溢出中断（电机B）
void TIM4_IRQHandler(void);  // TIM4溢出中断（电机C）
void TIM5_IRQHandler(void);  // TIM5溢出中断（电机D）
void TIM8_BRK_TIM12_IRQHandler(void); // 备用中断（当前未使用）


#endif

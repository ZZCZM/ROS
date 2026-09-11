/**
  ******************************************************************************
  * @file   timer.h
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
#ifndef __TIMER_H
#define __TIMER_H
#include "system.h"


/* 舵机控制端口统一配置 -----------------------------------------------*/
#define STEER_PORT GPIOC          // 舵机组统一使用GPIOC端口
#define STEER_GPIO_CLK RCC_AHB1Periph_GPIOC  // GPIOC时钟（AHB1总线）
#define STEER_TIM_CLK RCC_APB2Periph_TIM8    // TIM8时钟（APB2总线）

/* 舵机1配置（TIM8通道1） --------------------------------------------*/
#define STEER1_PORT GPIOC           // 端口C
#define STEER1_PIN GPIO_Pin_6       // PC6引脚（物理引脚：根据封装查询）
#define STEER1_PIN_Source GPIO_PinSource6 //引脚源编号6 
#define STEER1_GPIO_CLK RCC_AHB1Periph_GPIOC // 端口时钟
#define STEER1_TIM_CLK RCC_APB2Periph_TIM8   // 定时器时钟
#define STEER1_Channel TIM_Channel_1 // 使用TIM8通道1

/* 舵机2配置（TIM8通道2） --------------------------------------------*/
#define STEER2_PORT GPIOC
#define STEER2_PIN GPIO_Pin_7        // PC7
#define STEER2_PIN_Source GPIO_PinSource7 //引脚源编号7 
#define STEER2_GPIO_CLK RCC_AHB1Periph_GPIOC
#define STEER2_TIM_CLK RCC_APB2Periph_TIM8
#define STEER2_Channel TIM_Channel_2

/* 舵机3配置（TIM8通道3） --------------------------------------------*/
#define STEER3_PORT GPIOC
#define STEER3_PIN GPIO_Pin_8        // PC8
#define STEER3_PIN_Source GPIO_PinSource8 //引脚源编号8 
#define STEER3_GPIO_CLK RCC_AHB1Periph_GPIOC
#define STEER3_TIM_CLK RCC_APB2Periph_TIM8
#define STEER3_Channel TIM_Channel_3

/* 舵机4配置（TIM8通道4） --------------------------------------------*/
#define STEER4_PORT GPIOC
#define STEER4_PIN GPIO_Pin_9        // PC9
#define STEER4_PIN_Source GPIO_PinSource9 //引脚源编号9 
#define STEER4_GPIO_CLK RCC_AHB1Periph_GPIOC
#define STEER4_TIM_CLK RCC_APB2Periph_TIM8
#define STEER4_Channel TIM_Channel_4

/* 共用定时器配置 ---------------------------------------------------*/
#define STEER_AF GPIO_AF_TIM8       // 引脚复用为TIM8功能
#define STEER_ID TIM8                // 使用TIM8定时器
#define STEER_IRQn TIM8_CC_IRQn     // 捕获比较中断号

/* 函数声明 ---------------------------------------------------------*/
void TIM8_Cap_Init(u16 arr, u16 psc);   // TIM8输入捕获初始化（遥控器信号解码）
void TIM8_SERVO_Init(u16 arr, u16 psc); // TIM8 PWM输出初始化（舵机控制）

/* 全局变量声明 -----------------------------------------------------*/
extern int L_Remoter_Ch1,L_Remoter_Ch2,L_Remoter_Ch3,L_Remoter_Ch4; // 上次测量值（滤波用）
extern int Remoter_Ch1,Remoter_Ch2,Remoter_Ch3,Remoter_Ch4;        // 当前PWM脉宽值

#endif


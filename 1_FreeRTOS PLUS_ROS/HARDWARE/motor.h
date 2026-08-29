/**
  ******************************************************************************
  * @file   motor.h
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
#ifndef __MOTOR_H
#define __MOTOR_H

#include "system.h"
/****** 电机A驱动配置（双PWM通道独立控制） ******/
/*-- A相上桥臂控制（TIM10_CH1） --*/
#define DRIVE_A1_PORT GPIOF        // 端口F
#define DRIVE_A1_PIN GPIO_Pin_6    // PF6（物理引脚：LQFP176封装Pin83）
#define DRIVE_A1_PIN_Source GPIO_PinSource6 //引脚源编号6
#define DRIVE_A1_GPIO_CLK RCC_AHB1Periph_GPIOF  // GPIOF时钟
#define DRIVE_A1_TIM_CLK RCC_APB2Periph_TIM10   // TIM10时钟（APB2总线）
#define PWMA1 TIM10->CCR1          // PWM占空比寄存器（范围：0~ARR）
#define DRIVE_A1_AF GPIO_AF_TIM10  // 复用功能映射到TIM10
#define DRIVE_A1_ID TIM10          // 使用TIM10实例

/****** 电机A驱动配置（TIM10&TIM11）******/
/*-- A相下桥臂控制（TIM11_CH1） -----------------------------------------------
 * 功能：控制H桥下臂，实现电流反向
 * 引脚分配：PF7（物理引脚84，LQFP100封装）
 * 定时器特性：基本定时器，无死区生成功能
 */
#define DRIVE_A2_PORT GPIOF        // GPIOF端口
#define DRIVE_A2_PIN GPIO_Pin_7     // PF7引脚 
#define DRIVE_A2_PIN_Source GPIO_PinSource7//引脚源编号7
#define DRIVE_A2_GPIO_CLK RCC_AHB1Periph_GPIOF  // AHB1总线时钟
#define DRIVE_A2_TIM_CLK RCC_APB2Periph_TIM11   // APB2总线时钟（84MHz）
#define PWMA2 TIM11->CCR1          // 占空比寄存器（范围：0~ARR）
#define DRIVE_A2_AF GPIO_AF_TIM11   // 复用功能映射到TIM11_CH1
#define DRIVE_A2_ID TIM11           // 定时器实例

/****** 电机B驱动配置（TIM9双通道）*******************************************/
/*-- B相上桥臂控制（TIM9_CH1） -----------------------------------------------
 * 功能：控制H桥上臂，实现正向电流
 * 硬件连接：PE5（引脚78）连接驱动芯片IN1
 */
#define DRIVE_B1_PORT GPIOE        
#define DRIVE_B1_PIN GPIO_Pin_5   
#define DRIVE_B1_PIN_Source GPIO_PinSource5//引脚源编号5
#define DRIVE_B1_GPIO_CLK RCC_AHB1Periph_GPIOE  
#define DRIVE_B1_TIM_CLK RCC_APB2Periph_TIM9    
#define PWMB1 TIM9->CCR1           
#define DRIVE_B1_AF GPIO_AF_TIM9   
#define DRIVE_B1_ID TIM9           

/*-- B相下桥臂控制（TIM9_CH2） -----------------------------------------------
 * 互补特性：应与上臂PWM反相
 * 保护机制：需软件实现互锁，防止上下臂直通
 */
#define DRIVE_B2_PORT GPIOE        
#define DRIVE_B2_PIN GPIO_Pin_6    
#define DRIVE_B2_PIN_Source GPIO_PinSource6//引脚源编号6
#define DRIVE_B2_GPIO_CLK RCC_AHB1Periph_GPIOE  
#define DRIVE_B2_TIM_CLK RCC_APB2Periph_TIM9    
#define PWMB2 TIM9->CCR2           
#define DRIVE_B2_AF GPIO_AF_TIM9   
#define DRIVE_B2_ID TIM9           

/****** 电机C驱动配置（TIM1通道1&2）*****************************************/
/*-- C相上桥臂控制（TIM1_CH1） -----------------------------------------------
 * 高级定时器特性：支持死区插入、刹车功能
 * 引脚：PA8（引脚29），TIM1_CH1N互补输出可用
 */
#define DRIVE_C1_PORT GPIOA        
#define DRIVE_C1_PIN GPIO_Pin_8    
#define DRIVE_C1_PIN_Source GPIO_PinSource8 
#define DRIVE_C1_GPIO_CLK RCC_AHB1Periph_GPIOA  
#define DRIVE_C1_TIM_CLK RCC_APB2Periph_TIM1    
#define PWMC1 TIM1->CCR1          
#define DRIVE_C1_AF GPIO_AF_TIM1  
#define DRIVE_C1_ID TIM1          

/*-- C相下桥臂控制（TIM1_CH2） -----------------------------------------------
 * 推荐配置：使用互补通道CH2N实现硬件互锁
 * 死区时间：需通过BDTR寄存器配置（典型值500ns）
 */
#define DRIVE_C2_PORT GPIOA        
#define DRIVE_C2_PIN GPIO_Pin_9    
#define DRIVE_C2_PIN_Source GPIO_PinSource9
#define DRIVE_C2_GPIO_CLK RCC_AHB1Periph_GPIOA  
#define DRIVE_C2_TIM_CLK RCC_APB2Periph_TIM1    
#define PWMC2 TIM1->CCR2          
#define DRIVE_C2_AF GPIO_AF_TIM1  
#define DRIVE_C2_ID TIM1          

/****** 电机D驱动配置（TIM1通道3&4）*****************************************/
/*-- D相上桥臂控制（TIM1_CH3） -----------------------------------------------
 * 通道特性：TIM1_CH3可连接至刹车输入
 * 最大频率：APB2时钟84MHz时可达42MHz PWM
 */
#define DRIVE_D1_PORT GPIOA        
#define DRIVE_D1_PIN GPIO_Pin_10   
#define DRIVE_D1_PIN_Source GPIO_PinSource10
#define DRIVE_D1_GPIO_CLK RCC_AHB1Periph_GPIOA  
#define DRIVE_D1_TIM_CLK RCC_APB2Periph_TIM1    
#define PWMD1 TIM1->CCR3          
#define DRIVE_D1_AF GPIO_AF_TIM1  
#define DRIVE_D1_ID TIM1          

/*-- D相下桥臂控制（TIM1_CH4） -----------------------------------------------
 * 特殊功能：通道4可独立配置输出极性
 * 应用建议：使用TIM1->CCER寄存器独立控制
 */
#define DRIVE_D2_PORT GPIOA        
#define DRIVE_D2_PIN GPIO_Pin_11   
#define DRIVE_D2_PIN_Source GPIO_PinSource11
#define DRIVE_D2_GPIO_CLK RCC_AHB1Periph_GPIOA  
#define DRIVE_D2_TIM_CLK RCC_APB2Periph_TIM1    
#define PWMD2 TIM1->CCR4          
#define DRIVE_D2_AF GPIO_AF_TIM1  
#define DRIVE_D2_ID TIM1          

/*------------------------------------*/
//#define EN     PDin(3)  
/****** 舵机PWM控制配置（需补全STEER_ID定义） ******/
/* 舵机信号通道定义（假设STEER_ID为TIM8） 
   实际使用前需确认STEER_ID的具体定时器 */
//#define Servo_PWM6  TIM12->CCR2  // 保留通道
//#define Servo_PWM5  TIM12->CCR1  
#define Servo_PWM4  STEER_ID->CCR4  // 舵机4 PWM寄存器
#define Servo_PWM3  STEER_ID->CCR3  // 舵机3 PWM寄存器
#define Servo_PWM2  STEER_ID->CCR2  // 舵机2 PWM寄存器
#define Servo_PWM1  STEER_ID->CCR1  // 舵机1 PWM寄存器

/* 舵机中立位脉冲宽度（1.5ms @ 50Hz） 
   计算方式：ARR=20000-1时，1500对应1.5ms */
#define SERVO_INIT 1500  // 单位：定时器计数周期

/****** 硬件使能与PWM初始化函数声明 ******/
void Enable_Pin(void);  // 使能所有PWM输出引脚（推挽输出模式）

/* PWM定时器时基配置函数
   @param arr 自动重装载值（决定PWM周期）
   @param psc 预分频系数（决定定时器时钟）
   示例：TIM1_PWM_Init(19999, 83) 
         → 84MHz/(83+1)/20000 = 50Hz */
void TIM1_PWM_Init(u16 arr,u16 psc);  // 高级定时器TIM1配置
void TIM9_PWM_Init(u16 arr,u16 psc);  // 通用定时器TIM9
void TIM10_PWM_Init(u16 arr,u16 psc); // 通用定时器TIM10
void TIM11_PWM_Init(u16 arr,u16 psc); // 通用定时器TIM11
//void TIM12_PWM_Init(u16 arr,u16 psc); // 保留配置
//void TIM13_PWM_Init(u16 arr,u16 psc); // 保留配置

	


#endif

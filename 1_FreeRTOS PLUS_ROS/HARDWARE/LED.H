/**
  ******************************************************************************
  * @file   led.h
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
#ifndef __LED_H
#define __LED_H
#include "sys.h"
#include "system.h"
/****** LED任务配置参数 ******/
#define LED_TASK_PRIO        3       // 任务优先级（数值越小优先级越低）
#define LED_STK_SIZE         128     // 任务堆栈大小（单位：字，实际内存=128*4=512字节）
#define Led_Fast_Count  100          // 快速闪烁周期（单位：系统节拍，默认100=100ms）
#define Led_Init_Count   500         // 初始化呼吸灯周期（单位：系统节拍，默认500ms）

/****** 蜂鸣器硬件配置（当前禁用，嫌太吵了，其实应该是PG7引脚） ******/
/*-- PG6引脚配置（实际未连接蜂鸣器） --*/
#define Buzzer_PORT GPIOG        // 端口G
#define Buzzer_PIN GPIO_Pin_6   // PG6（物理引脚：LQFP144封装PIN_57）
#define Buzzer_GPIO_CLK RCC_AHB1Periph_GPIOG  // GPIOG时钟
#define Buzzer PGout(6)         // 蜂鸣器控制宏（当前接非蜂鸣器引脚）
/* 注意：实际硬件未连接蜂鸣器，配置仅为保留接口 */

/****** LED硬件配置（PC3驱动） ******/
#define LED_PORT GPIOC        // 端口C
#define LED_PIN GPIO_Pin_3    // PC3（物理位置：板载用户LED）
#define LED_GPIO_CLK RCC_AHB1Periph_GPIOC  // GPIOC时钟
#define LED PCout(3)          // LED控制宏（0-灭 1-亮）

/****** LED控制函数声明 ******/
void LED_Init(void);         // LED GPIO初始化（推挽输出模式）
void Buzzer_Init(void);      // 蜂鸣器GPIO初始化（当前为空实现）
void Led_Flash(u16 time);    // 阻塞式LED闪烁控制（time：闪烁周期，单位ms）
void Led_task(void *pvParameters); // FreeRTOS LED任务（呼吸灯效果）

/****** 全局变量声明 ******/
extern int Led_Count;  // LED效果控制参数（0-关闭 1-呼吸 2-快闪 3-慢闪）

#endif

/**
  ******************************************************************************
  * @file   key.h
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
#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "system.h"
/****** 按键功能函数声明 ******/
void KEY_Init(void);      // 按键GPIO初始化（配置PC13为上拉输入）
u8 click(void);           // 单击检测（返回1表示有效单击）
void Delay_ms(void);      // 阻塞式延时去抖（约20ms）
u8 click_N_Double (u8 time);       // 双击检测（time:两次点击最大间隔，单位10ms）
u8 click_N_Double_MPU6050 (u8 time); // 带姿态校验的双击（防止运动误触发）
u8 Long_Press(void);      // 长按检测（保持>1秒返回1）

/****** 按键硬件配置（开发板用户按键） ******/
#define KEY_PORT    GPIOC        // 按键所在端口：GPIOC
#define KEY_PIN        GPIO_Pin_13  // PC13（物理位置：板载蓝色按钮）
#define KEY_GPIO_CLK RCC_AHB1Periph_GPIOC  // 使能GPIOC时钟（AHB1总线）
#define KEY            PCin(13)     // 按键输入口（PC13）

/* 按键状态定义：
   - 按下：低电平（PC13内部上拉）
   - 释放：高电平 */

/*----------------------------------*/

#endif 

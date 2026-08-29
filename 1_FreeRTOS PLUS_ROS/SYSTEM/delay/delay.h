/**
  ******************************************************************************
  * @file   delay.h
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
#ifndef __DELAY_H
#define __DELAY_H 			   
#include <sys.h>	  
 
// 获取系统滴答计数值
// 该函数用于获取系统滴答计时器的当前计数值
u32 getSysTickCnt(void);

// 延时初始化函数
// 该函数用于初始化延时系统
// @param SYSCLK 系统时钟频率，单位为MHz
void delay_init(u8 SYSCLK);

// 微秒级延时函数
// 该函数用于产生指定微秒数的延时
// @param nus 要延时的微秒数
void delay_us(u32 nus);

// 毫秒级延时函数
// 该函数用于产生指定毫秒数的延时
// @param nms 要延时的毫秒数
void delay_ms(u32 nms);

// 扩展毫秒级延时函数
// 该函数用于产生指定毫秒数的延时，支持更大的延时值
// @param nms 要延时的毫秒数
void delay_xms(u32 nms);

#endif






























/**
  ******************************************************************************
  * @file    exti.h
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Header File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#ifndef __EXTI_H
#define __EXIT_H	 

#include "sys.h"      
// 定义INT为PB5引脚
// 该宏定义表示PB5引脚连接到MPU6050的中断引脚
#define INT PBin(15)   //PB5连接到MPU6050的中断引脚

// 外部中断初始化函数
// 该函数用于初始化外部中断
void EXTI_Init(void);	//外部中断初始化

#endif
		 					    


























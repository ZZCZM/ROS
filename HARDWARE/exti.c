#include "exti.h"
/**
  ******************************************************************************
  * @file    exti.c
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
/**************************************************************************
函数功能：外部中断初始化
入口参数：无
返回  值：无 
**************************************************************************/
void EXTI1_Init(void)
{
//	RCC->APB2ENR|=1<<3;    //使能PORTB时钟	   	 
//	GPIOB->CRH&=0X0FFFFFFF; 
//	GPIOB->CRH|=0X80000000;//PB5上拉输入
//  GPIOB->ODR|=1<<15;      //PB5上拉	
//	Ex_NVIC_Config(GPIO_B,15,FTIR);		//下降沿触发
//	MY_NVIC_Init(2,1,EXTI15_10_IRQn,2);  	//抢占2，子优先级1，组2
}











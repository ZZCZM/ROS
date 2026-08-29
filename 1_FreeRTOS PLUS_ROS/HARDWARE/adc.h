/**
  ******************************************************************************
  * @file   adc.h
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
#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
#include "system.h"
/*------------------------------------------
【ADC通道定义】传感器信号输入配置
------------------------------------------*/
#define Battery_Ch    8 // 电池电压检测通道（对应ADC2_IN8）
                        // 硬件连接：PB0 → 电压分压电路（10K+10K）
#define Potentiometer 9  // 电位器模拟输入通道（对应ADC1_IN9）
                        // 硬件连接：PB1 → 10KΩ旋转电位器

/*------------------------------------------
【电源电压ADC硬件配置】使用ADC2通道8
------------------------------------------*/
#define PWR_ADC_PORT    GPIOB       // 电压检测端口：GPIOB
#define PWR_ADC_PIN        GPIO_Pin_0  // 使用PB0引脚（物理引脚编号：30）
#define PWR_ADC_GPIO_CLK RCC_AHB1Periph_GPIOB  // 使能GPIOB时钟（AHB1总线）
#define PWR_ADC_ADC_CLK RCC_APB2Periph_ADC2   // 使能ADC2时钟（APB2总线）
#define PWR_ADC_ID ADC2             // ADC实例：ADC2

/*------------------------------------------
【电位器ADC硬件配置】使用ADC1通道9
------------------------------------------*/
#define POTEN_ADC_PORT    GPIOB       // 电位器检测端口：GPIOB
#define POTEN_ADC_PIN        GPIO_Pin_1   // 使用PB1引脚（物理引脚编号：31）
#define POTEN_ADC_GPIO_CLK RCC_AHB1Periph_GPIOB  // 复用GPIOB时钟
#define POTEN_ADC_ADC_CLK RCC_APB2Periph_ADC1   // 使能ADC1时钟
#define ADC_ID ADC1                   // ADC实例：ADC1

/*------------------------------------------
【ADC功能函数声明】
------------------------------------------*/
void Adc_Init(void);        // ADC全局初始化（时钟/模式/校准）
void Adc_POWER_Init(void);  // 电源电压ADC专用初始化（精度12bit/独立模式）
u16 Get_Adc(u8 ch);         // 获取ADC1指定通道原始值（通道范围：0-15）
u16 Get_Adc2(u8 ch);        // 获取ADC2指定通道原始值（通道范围：0-15）
float Get_battery_volt(void);  // 计算电池电压（单位：伏特）
u16 Get_adc_Average(u8 chn, u8 times); // 多次采样平均值滤波（抗噪声）

/*------------------------------------------
【电压计算全局变量】
------------------------------------------*/
extern float Voltage;        // 实时电压计算值（滤波后）
extern float Voltage_Count;  // 电压采样累加器（用于滑动平均）
extern float Voltage_All;    // 电压历史数据总和（用于大数据量平均）
	
#endif 



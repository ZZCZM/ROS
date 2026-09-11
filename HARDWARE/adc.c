/**
  ******************************************************************************
  * @file    adc.c
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
#include "adc.h"

float Voltage, Voltage_Count, Voltage_All;  // 变量相关于电池电压采样 //电池电压采样相关的变量  
const float Revise = 0.99;  // 修正系数，用于电压校准

/**************************************************************************
【函数名称】Adc_Init
【功能概述】ADC初始化电位器电压检测
            - 初始化ADC1及相关GPIO
            - 设置ADC参数以进行单通道采样
【输入参数】none
【返回值】none
【关键变量】Voltage: 电位器
            Voltage_Count: 单次ADC采样值
            Voltage_All: 多次采样累加值
【关键函数】RCC_AHB1PeriphClockCmd: 使能GPIO时钟
            RCC_APB2PeriphClockCmd: 使能ADC时钟
            GPIO_Init: 初始化GPIO端口
            ADC_CommonInit: 初始化ADC公共参数
            ADC_Init: 初始化ADC通道参数
            ADC_Cmd: 启动ADC转换器
**************************************************************************/
void Adc_Init(void)
{  
    GPIO_InitTypeDef GPIO_InitStructure;  // GPIO初始化结构体
    ADC_CommonInitTypeDef ADC_CommonInitStructure;  // ADC公共初始化结构体
    ADC_InitTypeDef ADC_InitStructure;  // ADC初始化结构体

    // 使能GPIOB时钟
    RCC_AHB1PeriphClockCmd(POTEN_ADC_GPIO_CLK, ENABLE);  // 使能GPIOB时钟
    // 使能ADC1时钟
    RCC_APB2PeriphClockCmd(POTEN_ADC_ADC_CLK, ENABLE);  // 使能ADC1时钟

    // 配置GPIOB的PB1引脚为模拟输入
    GPIO_InitStructure.GPIO_Pin = POTEN_ADC_PIN;  // PB1 通道9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  // 模拟输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 不带上下拉
    GPIO_Init(POTEN_ADC_PORT, &GPIO_InitStructure);  // 初始化GPIOB的PB1引脚

    // ADC1复位
    RCC_APB2PeriphResetCmd(POTEN_ADC_ADC_CLK, ENABLE);  // ADC1复位
    RCC_APB2PeriphResetCmd(POTEN_ADC_ADC_CLK, DISABLE);  // 复位结束

    // 设置ADC公共参数
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;  // 独立模式
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  // 两个采样阶段之间的延迟5个时钟
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;  // DMA失能
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6;  // 预分频6分频。ADCCLK=PCLK2/6=84/6=14MHz,ADC时钟最好不要超过36MHz
    ADC_CommonInit(&ADC_CommonInitStructure);  // 初始化ADC公共参数

    // 设置ADC1通道参数
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  // 12位模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;  // 非扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // 关闭连续转换
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  // 禁止触发检测，使用软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 右对齐
    ADC_InitStructure.ADC_NbrOfConversion = 1;  // 1个转换在规则序列中，也就是只转换规则序列1
    ADC_Init(ADC_ID, &ADC_InitStructure);  // 初始化ADC1通道参数
    ADC_Cmd(ADC_ID, ENABLE);  // 开启ADC转换器	 
}		
/**
  * @brief  ADC2初始化函数（用于电源电压采样）
  * @details 本函数完成以下配置：
  *          1. 使能GPIOB和ADC2的时钟
  *          2. 配置PB0引脚为模拟输入模式
  *          3. 复位并初始化ADC2外设
  *          4. 配置ADC公共参数和通道参数
  *          5. 使能ADC2转换器
  * @note 重要说明：
  *       - 使用ADC2通道8（对应PB0引脚）
  *       - ADC时钟配置为14MHz（基于84MHz PCLK2 6分频）
  *       - 单次转换模式，需手动触发采样
  *       - 12位分辨率，右对齐数据格式
  */
void Adc_POWER_Init(void)  
{  
    /* 定义初始化结构体 */
    GPIO_InitTypeDef GPIO_InitStructure;   // GPIO端口配置结构体
    ADC_CommonInitTypeDef ADC_CommonInitStructure;  // ADC通用配置结构体（影响所有ADC）
    ADC_InitTypeDef ADC_InitStructure;     // ADC通道配置结构体

    /* 【时钟使能阶段】 */
    // 使能GPIOB时钟（AHB1总线）
    // PWR_ADC_GPIO_CLK 宏应定义为 RCC_AHB1Periph_GPIOB
    RCC_AHB1PeriphClockCmd(PWR_ADC_GPIO_CLK, ENABLE);
    
    // 使能ADC2时钟（APB2总线）
    // PWR_ADC_ADC_CLK 宏应定义为 RCC_APB2Periph_ADC2
    RCC_APB2PeriphClockCmd(PWR_ADC_ADC_CLK, ENABLE);

    /* 【GPIO配置阶段】 */
    // 配置PB0引脚（对应ADC2通道8）
    GPIO_InitStructure.GPIO_Pin = PWR_ADC_PIN;     // 选择PB0引脚（GPIO_Pin_0）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;   // 模拟输入模式（ADC专用）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 禁用上下拉电阻（避免影响采样精度）
    GPIO_Init(PWR_ADC_PORT, &GPIO_InitStructure);  // PWR_ADC_PORT 应定义为 GPIOB

    /* 【ADC复位阶段】 */
    // 通过复位ADC2确保寄存器恢复默认值
    RCC_APB2PeriphResetCmd(PWR_ADC_ADC_CLK, ENABLE);  // 启动复位
    RCC_APB2PeriphResetCmd(PWR_ADC_ADC_CLK, DISABLE); // 结束复位（复位脉冲需保持至少2个时钟周期）

    /* 【ADC通用参数配置】 */
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;  // 独立模式（ADC2独立工作，不与其他ADC同步）
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; // 双重模式采样间隔（本配置未使用双重模式）
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; // 禁用DMA传输（单次采样手动读取）
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6; // 时钟6分频（84MHz/6=14MHz，低于ADC最大允许36MHz）
    ADC_CommonInit(&ADC_CommonInitStructure);  // 应用通用配置（影响整个ADC模块）

    /* 【ADC2通道参数配置】 */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  // 12位分辨率（最大精度）
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;    // 禁用扫描模式（单通道采样）
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // 关闭连续转换（每次采样需手动触发）
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // 禁用硬件触发（使用软件触发ADCSTART）
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 数据右对齐（便于数值计算）
    ADC_InitStructure.ADC_NbrOfConversion = 1;       // 转换序列长度（仅使用规则通道1）
    ADC_Init(PWR_ADC_ID, &ADC_InitStructure);        // PWR_ADC_ID 应定义为 ADC2

    /* 【使能ADC】 */
    ADC_Cmd(PWR_ADC_ID, ENABLE);  // 启动ADC2转换器
    /* 注意：ADC首次使能后需要约10us的启动时间才能进行校准或采样 */
}

/**************************************************************************
【函数名称】Get_Adc
【功能概述】AD采样
            - 根据指定的ADC通道进行一次采样
            - 使用软件触发转换
【输入参数】ch: ADC通道
【返回值】u16: AD转换结果
【关键函数】ADC_RegularChannelConfig: 配置规则组通道
            ADC_SoftwareStartConv: 软件启动ADC转换
            ADC_GetFlagStatus: 检查转换完成标志
            ADC_GetConversionValue: 获取转换结果
**************************************************************************/
u16 Get_Adc(u8 ch)   
{
    // 设置指定ADC的规则组通道，一个序列，采样时间为480周期
    // Sets the specified ADC rule group channel, one sequence, and sampling time
    ADC_RegularChannelConfig(ADC_ID, ch, 1, ADC_SampleTime_480Cycles);
    // 使能指定的ADC软件转换启动功能
    // Enable the specified ADC software transformation startup function
    ADC_SoftwareStartConv(ADC_ID);
    // 等待转换结束
    // Wait for the conversion to finish
    while(!ADC_GetFlagStatus(ADC_ID, ADC_FLAG_EOC));
    // 返回最近一次ADC规则组的转换结果
    // Returns the result of the last ADC rule group conversion
    return ADC_GetConversionValue(ADC_ID);	
}

/**
  * @brief  执行单通道ADC采样并获取转换结果
  * @param  ch : ADC通道编号（0-18，具体取决于芯片型号）
  * @note 功能流程：
  * 1. 配置规则组通道参数
  * 2. 启动软件触发转换
  * 3. 阻塞等待转换完成
  * 4. 读取转换结果
  * @note 硬件特性：
  * - 使用规则组单次转换模式
  * - 固定采用480个时钟周期的采样时间（适合高阻抗信号源）
  * - 通过PWR_ADC_ID宏指定具体ADC外设（如ADC1/ADC2）
  * @warning 本函数为阻塞式操作，转换期间CPU保持等待
  * @retval 12位ADC转换结果（0-4095对应0-VREF电压）
  */
u16 Get_Adc2(u8 ch)   
{
    /* 配置规则组通道参数 */
    // 参数分解：
    // PWR_ADC_ID → 预定义的ADC外设（如ADC1）
    // ch → 目标通道号（如ADC_Channel_5对应PA5引脚）
    // 1 → 规则组序列中的排序位置（单次转换设为1）
    // ADC_SampleTime_480Cycles → 最长采样时钟周期（提高采样精度）
    ADC_RegularChannelConfig(PWR_ADC_ID, ch, 1, ADC_SampleTime_480Cycles);

    /* 启动软件触发转换 */
    // 通过写控制寄存器触发单次转换：
    // 该操作置位SWSTART位，启动转换流程
    ADC_SoftwareStartConv(PWR_ADC_ID);

    /* 轮询等待转换完成 */
    // 持续检测EOC（End Of Conversion）标志位：
    // - 转换完成时硬件自动置位该标志
    // - 典型等待时间：480 + 12.5 = 492.5个ADC时钟周期
    while(!ADC_GetFlagStatus(PWR_ADC_ID, ADC_FLAG_EOC));

    /* 读取转换结果寄存器 */
    // 获取规则组数据寄存器(DR)的值：
    // - 12位有效数据（右对齐）
    // - 读取操作会自动清除EOC标志
    return ADC_GetConversionValue(PWR_ADC_ID);	
}


/**************************************************************************
【函数名称】Get_adc_Average
【功能概述】采集多次ADC值求平均值函数
            - 根据指定的ADC通道采集多次ADC值
            - 计算并返回平均值
【输入参数】chn: ADC通道
            times: 采集次数
【返回值】u16: AD转换结果的平均值
【关键函数】Get_Adc: 单次ADC采样函数
            delay_ms: 毫秒级延时函数
**************************************************************************/
u16 Get_adc_Average(u8 chn, u8 times)
{
    u32 temp_val = 0;  // 累加ADC采样值
    u8 t;
    for(t = 0; t < times; t++)
    {
        temp_val += Get_Adc(chn);  // 采集一次ADC值并累加
        delay_ms(5);  // 每次采样后延时5ms
    }
    return temp_val / times;  // 计算并返回平均值
}

/**************************************************************************
【函数名称】Get_battery_volt
【功能概述】读取电池电压
            - 通过ADC2采集电池电压
            - 根据电阻分压公式计算实际电压
【输入参数】none
【返回值】float: 电池电压（单位：mV）
【关键变量】Revise: 修正系数，用于电压校准
【关键函数】Get_Adc2: 从ADC2获取转换结果
**************************************************************************/
float Get_battery_volt(void)   
{  
    float Volt;
    
    // 根据原理图计算电池电压
    // The resistance partial voltage can be obtained by simple analysis according to the schematic diagram
    // 电阻分压，具体根据原理图简单分析可以得到	
    Volt = Get_Adc2(Battery_Ch) * 3.3f * 11.0f * Revise / 1.0f / 4096.0f;	
    return Volt;
}





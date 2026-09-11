/**
  ******************************************************************************
  * @file    motor.c
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
#include "motor.h"


/**************************************************************************
函数功能：电机驱动PWM初始化（TIM1通道，驱动电机C/D）
硬件配置：
- 使用TIM1高级定时器产生4路PWM
- 驱动电机C和D的正反转控制
- 支持PWM模式1，推挽输出模式
参数说明：
- arr：自动重装载值（决定PWM频率）
- psc：预分频系数（时钟分频）
计算公式：
PWM频率 = 定时器时钟 / [(arr+1)*(psc+1)]
典型配置：
@72MHz主频，arr=7199，psc=0 → 10kHz PWM
**************************************************************************/
void TIM1_PWM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 外设时钟使能 */
    RCC_AHB1PeriphClockCmd(DRIVE_C1_GPIO_CLK, ENABLE);   // 使能GPIO端口时钟
    RCC_APB2PeriphClockCmd(DRIVE_C1_TIM_CLK, ENABLE);    // 使能TIM1时钟（APB2总线）

    /* GPIO复用配置 */
    GPIO_PinAFConfig(DRIVE_C1_PORT, DRIVE_C1_PIN_Source, DRIVE_C1_AF);  // 电机C1引脚复用为TIM1_CH1——PA8
    GPIO_PinAFConfig(DRIVE_C2_PORT, DRIVE_C2_PIN_Source, DRIVE_C2_AF);  // 电机C2引脚复用为TIM1_CH2——PA9
    GPIO_PinAFConfig(DRIVE_D1_PORT, DRIVE_D1_PIN_Source, DRIVE_D1_AF);   // 电机D1引脚复用为TIM1_CH3——PA10
    GPIO_PinAFConfig(DRIVE_D2_PORT, DRIVE_D2_PIN_Source, DRIVE_D2_AF);   // 电机D2引脚复用为TIM1_CH4——PA11

    /* GPIO参数配置 */
    GPIO_InitStructure.GPIO_Pin = DRIVE_C1_PIN | DRIVE_C2_PIN | DRIVE_D1_PIN | DRIVE_D2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         // 复用功能模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   // 高速模式（确保PWM波形质量）
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       // 推挽输出（增强驱动能力）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          // 上拉电阻（防止悬空状态）
    GPIO_Init(DRIVE_C1_PORT, &GPIO_InitStructure);        // 应用配置到GPIO端口

    /* 定时器时基配置 */
    TIM_TimeBaseStructure.TIM_Period = arr;              // 自动重装载值（决定PWM周期）
    TIM_TimeBaseStructure.TIM_Prescaler = psc;            // 预分频系数（时钟分频）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分割（与外部触发相关）
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(DRIVE_C1_ID, &TIM_TimeBaseStructure); // 初始化TIM1时基

    /* PWM输出通道配置 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     // PWM模式1（CNT<CCR时有效）
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 高电平有效
    
    // 初始化四个通道（CH1-CH4）
    TIM_OC1Init(DRIVE_C1_ID, &TIM_OCInitStructure);       // 电机C1通道
    TIM_OC2Init(DRIVE_C2_ID, &TIM_OCInitStructure);       // 电机C2通道 
    TIM_OC3Init(DRIVE_D1_ID, &TIM_OCInitStructure);        // 电机D1通道
    TIM_OC4Init(DRIVE_D2_ID, &TIM_OCInitStructure);       // 电机D2通道

    /* 高级定时器特殊配置 */
    TIM_CtrlPWMOutputs(DRIVE_C1_ID, ENABLE);              // 必须使能PWM主输出（MOE）

    /* 预装载使能配置 */
    TIM_OC1PreloadConfig(DRIVE_C1_ID, TIM_OCPreload_Enable);  // 通道1预装载
    TIM_OC2PreloadConfig(DRIVE_C1_ID, TIM_OCPreload_Enable);  // 通道2预装载
    TIM_OC3PreloadConfig(DRIVE_C1_ID, TIM_OCPreload_Enable);  // 通道3预装载
    TIM_OC4PreloadConfig(DRIVE_C1_ID, TIM_OCPreload_Enable);  // 通道4预装载

    TIM_ARRPreloadConfig(DRIVE_C1_ID, ENABLE);            // 自动重装载预装载使能

    /* 启动定时器 */
    TIM_Cmd(DRIVE_C1_ID, ENABLE);                        // 使能TIM1计数器
}

/**************************************************************************
函数功能：电机驱动PWM初始化（TIM9通道，驱动电机B）
设计特点：
- 使用TIM9基本定时器
- 双通道PWM输出
- 支持电机B的正反转控制
参数说明同TIM1_PWM_Init
硬件连接：
- 电机B使用TIM9_CH1和TIM9_CH2
- 需确保DRIVE_B1_ID宏定义为TIM9
**************************************************************************/
void TIM9_PWM_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 时钟使能 */
    RCC_AHB1PeriphClockCmd(DRIVE_B1_GPIO_CLK, ENABLE);   // GPIO时钟
    RCC_APB2PeriphClockCmd(DRIVE_B1_TIM_CLK, ENABLE);     // TIM9时钟

    /* 引脚复用配置 */
    GPIO_PinAFConfig(DRIVE_B1_PORT, DRIVE_B1_PIN_Source, DRIVE_B1_AF); // TIM9_CH1
    GPIO_PinAFConfig(DRIVE_B1_PORT, DRIVE_B2_PIN_Source, DRIVE_B2_AF); // TIM9_CH2

    /* GPIO参数配置 */
    GPIO_InitStructure.GPIO_Pin = DRIVE_B1_PIN | DRIVE_B2_PIN; //PE5,PE6
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(DRIVE_B1_PORT, &GPIO_InitStructure);        // 应用配置

    /* 定时器时基配置 */
    TIM_TimeBaseInitStructure.TIM_Period = arr;          // PWM周期
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;        // 预分频
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(DRIVE_B1_ID, &TIM_TimeBaseInitStructure); // 初始化TIM9

    /* PWM通道配置 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    TIM_OC1Init(DRIVE_B1_ID, &TIM_OCInitStructure);      // 通道1
    TIM_OC2Init(DRIVE_B2_ID, &TIM_OCInitStructure);      // 通道2

    /* 预装载配置 */
    TIM_OC1PreloadConfig(DRIVE_B1_ID, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(DRIVE_B2_ID, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(DRIVE_B1_ID, ENABLE);           // ARR预装载

    /* 启动定时器 */
    TIM_Cmd(DRIVE_B1_ID, ENABLE);                        // 使能TIM9
}

/**************************************************************************
函数功能：电机A驱动PWM初始化（TIM10通道）
硬件配置：
- 使用TIM10基本定时器
- 单通道PWM输出（DRIVE_A1_PIN）
- 支持电机A的单方向速度控制
参数说明：
- arr：自动重装载值（决定PWM频率）
- psc：预分频系数
频率公式：
PWM频率 = 定时器时钟 / [(arr+1)*(psc+1)]
典型配置：
@84MHz时钟，arr=16799=0 → 10kHz PWM
**************************************************************************/
void TIM10_PWM_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 时钟使能 */
    RCC_AHB1PeriphClockCmd(DRIVE_A1_GPIO_CLK, ENABLE);   // 使能GPIO端口时钟（假设DRIVE_A1_PORT为GPIOF）
    RCC_APB2PeriphClockCmd(DRIVE_A1_TIM_CLK, ENABLE);    // 使能TIM10时钟（APB2总线）

    /* 引脚复用配置 */
    GPIO_PinAFConfig(DRIVE_A1_PORT, DRIVE_A1_PIN_Source, DRIVE_A1_AF);  // 将PF6复用为TIM10_CH1

    /* GPIO参数初始化 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      // 推挽输出（增强驱动能力）
    GPIO_InitStructure.GPIO_Pin = DRIVE_A1_PIN;         // 选择PF6引脚
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        // 上拉电阻（防止悬空）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   // 高速模式（确保PWM边沿质量）
    GPIO_Init(DRIVE_A1_PORT, &GPIO_InitStructure);       // 应用配置到GPIOF

    /* 定时器时基配置 */
    TIM_TimeBaseInitStructure.TIM_Period = arr;         // 自动重装载值（决定PWM周期）
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;      // 预分频系数（时钟分频）
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分割（与输入捕获相关）
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(DRIVE_A1_ID, &TIM_TimeBaseInitStructure); // 初始化TIM10

    /* PWM通道配置 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  // PWM模式1（CNT<CCR时有效）
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    // 高电平有效
    TIM_OC1Init(DRIVE_A1_ID, &TIM_OCInitStructure);      // 初始化通道1

    /* 预装载配置 */
    TIM_OC1PreloadConfig(DRIVE_A1_ID, TIM_OCPreload_Enable); // 使能通道1预装载

    /* 特别注意：TIM10作为基本定时器无需以下配置 */
    // TIM_CtrlPWMOutputs(TIM10,ENABLE);  // 该配置仅高级定时器需要（TIM1/TIM8）

    TIM_ARRPreloadConfig(DRIVE_A1_ID, ENABLE);          // 使能ARR预装载

    /* 启动定时器 */
    TIM_Cmd(DRIVE_A1_ID, ENABLE);                       // 使能TIM10计数器
}
/**************************************************************************
函数功能：初始化TIM11通道1产生PWM信号驱动电机A辅助控制
入口参数：
    arr：自动重装载值（决定PWM周期）
    psc：预分频系数（时钟分频因子）
返回值：无
技术要点：
1. PWM频率计算：Fpwm = Fclk / (arr+1) / (psc+1)
2. 占空比计算：Duty = CCR / (arr+1)
3. 使用PF7引脚作为TIM11_CH1输出
硬件连接：
- PF7引脚需连接电机驱动器的PWM输入端子
- 需配合TIM10形成互补输出时需外接死区电路
注意事项：
1. 该定时器无硬件死区功能，需外接RC电路或软件补偿
2. 确保DRIVE_A2_GPIO_CLK和DRIVE_A2_TIM_CLK已正确定义
3. 修改参数前应先停止定时器
4. 建议PWM频率设置在10kHz-20kHz之间避免可闻噪声
**************************************************************************/
void TIM11_PWM_Init(u16 arr, u16 psc)
{
    /* 结构体声明 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;  // 定时器时基配置结构体
    GPIO_InitTypeDef GPIO_InitStructure;                // GPIO配置结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;              // 输出比较配置结构体

    /* 时钟使能配置 -----------------------------------------------------------
     * DRIVE_A2_GPIO_CLK：PF端口时钟（AHB1总线）
     * DRIVE_A2_TIM_CLK：TIM11时钟（APB2总线）
     * 注：STM32F4中TIM1/TIM8-TIM11挂在APB2，其他定时器在APB1
     */
    RCC_AHB1PeriphClockCmd(DRIVE_A2_GPIO_CLK, ENABLE);  // 使能GPIOF时钟
    RCC_APB2PeriphClockCmd(DRIVE_A2_TIM_CLK, ENABLE);   // 使能TIM11时钟

    /* 引脚复用配置 -----------------------------------------------------------
     * 功能：将PF7引脚映射到TIM11通道1
     * DRIVE_A2_AF：应定义为GPIO_AF_TIM11
     * 复用原理：通过GPIOx_AFRL/AFRH寄存器配置
     */
    GPIO_PinAFConfig(DRIVE_A2_PORT, DRIVE_A2_PIN_Source, DRIVE_A2_AF); // PF7复用为TIM11_CH1

    /* GPIO模式配置 ----------------------------------------------------------
     * 推挽输出：提供强驱动能力
     * 上拉电阻：确保默认高电平（安全状态）
     * 高速模式：适应PWM高频切换需求
     */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       // 推挽输出
    GPIO_InitStructure.GPIO_Pin = DRIVE_A2_PIN;         // 操作PF7引脚
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        // 内部上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  // 100MHz高速模式
    GPIO_Init(DRIVE_A2_PORT, &GPIO_InitStructure);      // 应用配置到PF7

    /* 定时器时基配置 ---------------------------------------------------------
     * 周期计算：T = (arr+1)*(psc+1)/Fclk
     * 计数模式：递增计数（PWM波形生成标准模式）
     */
    TIM_TimeBaseInitStructure.TIM_Period = arr;         // 自动重装载值（决定PWM周期）
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;      // 预分频系数（时钟分频）
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟不分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseInit(DRIVE_A2_ID, &TIM_TimeBaseInitStructure); // 初始化TIM11

    /* PWM通道参数配置 --------------------------------------------------------
     * PWM模式1：CNT < CCR时输出有效电平
     * 极性高：CCR值越大，高电平时间越长
     */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  // PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 高电平有效
    TIM_OC1Init(DRIVE_A2_ID, &TIM_OCInitStructure);      // 初始化通道1

    /* 预装载配置 -------------------------------------------------------------
     * 使能CCR预装载：可在周期更新时同步修改占空比
     * 使能ARR预装载：确保周期参数平滑切换
     */
    TIM_OC1PreloadConfig(DRIVE_A2_ID, TIM_OCPreload_Enable); // 使能CCR1预装载
    TIM_ARRPreloadConfig(DRIVE_A2_ID, ENABLE);          // 使能ARR预装载

    /* 高级定时器专用使能（TIM11为基本定时器，故注释）*/
    //TIM_CtrlPWMOutputs(DRIVE_A2_ID, ENABLE);          // 主输出使能（TIM1/TIM8需要）

    /* 启动定时器 -----------------------------------------------------------*/
    TIM_Cmd(DRIVE_A2_ID, ENABLE);                       // 使能TIM11计数器
}


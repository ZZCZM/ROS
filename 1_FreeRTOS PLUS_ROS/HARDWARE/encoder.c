/**
  ******************************************************************************
  * @file    encoder.c
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
#include "encoder.h"

/**************************************************************************
函数功能：把TIM2初始化为编码器接口模式
入口参数：无
返 回 值：无
描述：本函数完成TIM2定时器的编码器接口模式配置，包含GPIO复用配置、定时器基本参数设置、
       编码器模式选择及中断使能。适用于AB相增量式编码器信号采集。
硬件依赖：PA15和PB3引脚需连接编码器AB相输出信号线
配置特性：
- GPIO复用为TIM2通道1/2
- 无预分频（直接输入脉冲计数）
- 自动重装载值由ENCODER_TIM_PERIOD宏定义
- 使用编码器模式3（TI1和TI2边沿均触发计数）
- 开启定时器更新中断
**************************************************************************/
void Encoder_Init_TIM2(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  // 定时器时基结构体
    TIM_ICInitTypeDef TIM_ICInitStructure;          // 输入捕获结构体
    GPIO_InitTypeDef GPIO_InitStructure;             // GPIO初始化结构体
    
    /* 时钟使能配置 */
    RCC_AHB1PeriphClockCmd(MOTORA1_GPIO_CLK|MOTORA2_GPIO_CLK, ENABLE);  // 使能GPIOA和GPIOB时钟（PA15和PB3）
    RCC_APB1PeriphClockCmd(MOTORA1_ADC_CLK, ENABLE);   // 使能TIM2外设时钟（APB1总线）

    /* PA15配置（TIM2_CH1） */
    GPIO_InitStructure.GPIO_Pin = MOTORA1_PIN;        // 选择PA15引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 高速模式（编码器信号需要快速响应）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;     // 开漏输出（兼容不同电平设备）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉电阻使能（确保无信号时高电平）
    GPIO_Init(MOTORA1_PORT, &GPIO_InitStructure);      // 应用配置到GPIOA
    
    /* PB3配置（TIM2_CH2） */
    GPIO_InitStructure.GPIO_Pin = MOTORA2_PIN;        // 选择PB3引脚
    GPIO_Init(MOTORA2_PORT, &GPIO_InitStructure);      // 应用相同配置到GPIOB

    /* 引脚复用功能配置 */
    GPIO_PinAFConfig(MOTORA1_PORT,MOTORA1_PIN_Source,MOTORA1_AF);   // PA15复用为TIM2_CH1
    GPIO_PinAFConfig(MOTORA2_PORT,MOTORA2_PIN_Source,MOTORA2_AF);   // PB3复用为TIM2_CH2
    
    /* 定时器时基参数初始化 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);          // 加载默认时基参数
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;                // 预分频器清零（不分频）
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;    // 自动重装载值（根据编码器分辨率设定）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   // 时钟分频因子为1
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(MOTORA1_ID, &TIM_TimeBaseStructure);     // 初始化TIM2时基单元

    /* 编码器接口配置 */
    TIM_EncoderInterfaceConfig(MOTORA1_ID, 
        TIM_EncoderMode_TI12,                // 编码器模式3（TI1和TI2边沿均触发）
        TIM_ICPolarity_Rising,               // 通道1上升沿有效
        TIM_ICPolarity_Rising);              // 通道2上升沿有效
    
    /* 输入捕获参数配置 */
    TIM_ICStructInit(&TIM_ICInitStructure);   // 加载默认输入捕获参数
    TIM_ICInitStructure.TIM_ICFilter = 0;     // 输入滤波器关闭（最大响应速度）
    TIM_ICInit(MOTORA1_ID, &TIM_ICInitStructure);  // 应用输入捕获配置

    /* 定时器中断配置 */
    TIM_ClearFlag(MOTORA1_ID, TIM_FLAG_Update);      // 清除更新中断标志
    TIM_ITConfig(MOTORA1_ID, TIM_IT_Update, ENABLE); // 使能定时器更新中断
    
    /* 启动定时器 */
    TIM_SetCounter(MOTORA1_ID,0);       // 计数器归零（初始位置校准）
    TIM_Cmd(MOTORA1_ID, ENABLE);        // 使能TIM2计数器
}

/**
 * @brief 配置TIM3为正交编码器接口模式
 * @param None
 * @retval None
 * @detail 硬件连接：
 *         - PB4: TIM3_CH1（编码器A相）
 *         - PB5: TIM3_CH2（编码器B相）
 * 特性说明：
 *         - 4倍频计数（TI12模式，双沿触发）
 *         - 16位向上计数（0~ENCODER_TIM_PERIOD）
 *         - 无输入滤波（响应频率最高）
 *         - 内部上拉电阻使能
 * 注意事项：
 *         - ENCODER_TIM_PERIOD需根据编码器线数设置
 *         - MOTORB1_ID宏必须定义为TIM3
 */
void Encoder_Init_TIM3(void)
{
    /* 外设结构体声明（三组配置参数） */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  // 定时器时基配置
    TIM_ICInitTypeDef TIM_ICInitStructure;          // 输入捕获通道配置
    GPIO_InitTypeDef GPIO_InitStructure;            // GPIO端口配置

    /* 时钟使能配置（精确控制节能） */
    RCC_AHB1PeriphClockCmd(MOTORB1_GPIO_CLK, ENABLE);  // 使能GPIOB时钟（AHB1总线）
    RCC_APB1PeriphClockCmd(MOTORB1_ADC_CLK, ENABLE);   // 使能TIM3时钟（APB1总线）

    /* GPIO复用配置（编码器信号输入） */
    GPIO_InitStructure.GPIO_Pin = MOTORB1_PIN | MOTORB2_PIN;  // 同时配置PB4和PB5
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;        // 高速响应（适合脉冲信号）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;              // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;            // 开漏输出（总线安全设计）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;              // 内部上拉（抗干扰）
    GPIO_Init(MOTORB1_PORT, &GPIO_InitStructure);             // 应用配置到GPIOB

    /* 引脚功能映射（绑定TIM3通道） */
    GPIO_PinAFConfig(MOTORB1_PORT, MOTORB1_PIN_Source, MOTORB1_AF);  // PB4复用为TIM3_CH1
    GPIO_PinAFConfig(MOTORB2_PORT, MOTORB2_PIN_Source, MOTORB2_AF);  // PB5复用为TIM3_CH2

    /* 定时器基础参数配置（核心计数设置） */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);               // 加载默认参数
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;                    // 无分频（72MHz直接驱动）
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;         // 自动重装载值（根据编码器分辨率设定）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;        // 时钟不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    // 向上计数模式
    TIM_TimeBaseInit(MOTORB1_ID, &TIM_TimeBaseStructure);          // 应用配置到TIM3

    /* 编码器接口模式配置（正交解码核心） */
    TIM_EncoderInterfaceConfig(MOTORB1_ID, 
                              TIM_EncoderMode_TI12,              // 双通道模式（4倍频）
                              TIM_ICPolarity_Rising,             // CH1上升沿有效
                              TIM_ICPolarity_Rising);            // CH2上升沿有效
    
    /* 输入捕获通道配置（信号滤波参数） */
    TIM_ICStructInit(&TIM_ICInitStructure);                     // 初始化默认参数
    TIM_ICInitStructure.TIM_ICFilter = 0;                       // 无输入滤波（0~0xF可设置采样窗口）
    TIM_ICInit(MOTORB1_ID, &TIM_ICInitStructure);                // 应用配置到TIM3

    /* 定时器状态初始化 */
    TIM_ClearFlag(MOTORB1_ID, TIM_FLAG_Update);          // 清除更新中断标志
    TIM_ITConfig(MOTORB1_ID, TIM_IT_Update, ENABLE);    // 使能定时器更新中断（用于溢出统计）
    TIM_SetCounter(MOTORB1_ID, 0);                      // 计数器归零（位置校准）
    TIM_Cmd(MOTORB1_ID, ENABLE);                        // 启动定时器计数
}
/**************************************************************************
函数功能：初始化TIM4为编码器接口模式
入口参数：无
返 回 值：无
工作流程：
1. 配置GPIO为复用功能模式
2. 初始化定时器基本参数
3. 设置编码器接口模式
4. 配置输入捕获参数
5. 启动定时器与相关中断
注意事项：
1. GPIO配置需匹配硬件连接（PB6=CH1，PB7=CH2）
2. ENCODER_TIM_PERIOD决定计数范围（建议值：线数×4）
3. 开漏输出避免总线冲突，上拉确保默认高电平
4. 滤波器设为0时最高响应速度
5. 必须使能更新中断处理计数器溢出
**************************************************************************/
void Encoder_Init_TIM4(void)
{
    // 定义初始化结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  // 定时器时基配置结构体
    TIM_ICInitTypeDef TIM_ICInitStructure;          // 输入捕获配置结构体
    GPIO_InitTypeDef GPIO_InitStructure;            // GPIO配置结构体
    
    /* GPIO端口时钟使能 */
    RCC_AHB1PeriphClockCmd(MOTORC1_GPIO_CLK, ENABLE);  // 启用GPIOB的AHB1总线时钟
    RCC_APB1PeriphClockCmd(MOTORC1_ADC_CLK, ENABLE);   // 启用TIM4的APB1总线时钟
    
    /* 编码器引脚配置 */
    GPIO_InitStructure.GPIO_Pin = MOTORC1_PIN | MOTORC2_PIN;  // 同时配置PB6和PB7
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;        // 高速模式适应编码器信号
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;              // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;            // 开漏输出防止总线冲突
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;              // 内部上拉确保默认高电平
    GPIO_Init(MOTORC1_PORT, &GPIO_InitStructure);              // 应用GPIO配置
    
    /* 引脚复用映射 */
    GPIO_PinAFConfig(MOTORC1_PORT,MOTORC1_PIN_Source,MOTORC1_AF); // PB6复用为TIM4_CH1
    GPIO_PinAFConfig(MOTORC2_PORT,MOTORC2_PIN_Source,MOTORC2_AF); // PB7复用为TIM4_CH2
    
    /* 定时器基本参数配置 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);          // 初始化时基结构体为默认值
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;                // 无预分频（直接使用输入频率）
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;   // 自动重装载值（决定计数范围）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式（编码器模式自动覆盖）  
    TIM_TimeBaseInit(MOTORC1_ID, &TIM_TimeBaseStructure);      // 应用时基配置
    
    /* 编码器接口配置 */
    TIM_EncoderInterfaceConfig(MOTORC1_ID,                   // 选择TIM4定时器
        TIM_EncoderMode_TI12,                                // 使用TI1和TI2双通道模式
        TIM_ICPolarity_Rising,                               // CH1上升沿有效
        TIM_ICPolarity_Rising);                              // CH2上升沿有效
    
    /* 输入捕获参数设置 */
    TIM_ICStructInit(&TIM_ICInitStructure);           // 初始化输入捕获结构体为默认值
    TIM_ICInitStructure.TIM_ICFilter = 0;             // 不启用输入滤波器（快速响应）
    TIM_ICInit(MOTORC1_ID, &TIM_ICInitStructure);      // 应用输入捕获配置
    
    /* 定时器启动准备 */
    TIM_ClearFlag(MOTORC1_ID, TIM_FLAG_Update);        // 清除更新中断标志（防止首次误触发）
    TIM_ITConfig(MOTORC1_ID, TIM_IT_Update, ENABLE);  // 使能定时器更新中断（用于溢出计数）
    TIM_SetCounter(MOTORC1_ID,0);                      // 重置计数器初始值为0
    TIM_Cmd(MOTORC1_ID, ENABLE);                       // 启动TIM4计数器
}

/**************************************************************************
函数功能：把TIM5初始化为编码器接口模式
入口参数：无
返 回 值：无
硬件配置：
- 使用TIM5作为正交编码器接口
- 引脚配置：MOTORD1_PIN和MOTORD2_PIN（具体引脚由宏定义决定）
- 工作模式：编码器模式3（TI1和TI2边沿均计数）
- 计数器周期：ENCODER_TIM_PERIOD
注意事项：
1. GPIO需预先配置为复用功能模式
2. 需要确保MOTORD1_ID宏正确定义为TIM5
3. 使用前需在头文件定义ENCODER_TIM_PERIOD值
**************************************************************************/
void Encoder_Init_TIM5(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  // 定时器时基结构体
    TIM_ICInitTypeDef TIM_ICInitStructure;          // 输入捕获结构体
    GPIO_InitTypeDef GPIO_InitStructure;            // GPIO配置结构体
    
    /* 外设时钟使能 */
    RCC_AHB1PeriphClockCmd(MOTORD1_GPIO_CLK, ENABLE);  // 使能GPIO端口时钟（PB组）
    RCC_APB1PeriphClockCmd(MOTORD1_ADC_CLK, ENABLE);   // 使能TIM5时钟（APB1总线）

    /* 引脚复用配置（需在GPIO初始化前设置） */
    GPIO_PinAFConfig(MOTORD1_PORT,MOTORD1_PIN_Source,MOTORD1_AF); // 将MOTORD1_PIN复用为TIM5_CH1
    GPIO_PinAFConfig(MOTORD2_PORT,MOTORD2_PIN_Source,MOTORD2_AF); // 将MOTORD2_PIN复用为TIM5_CH2
    
    /* GPIO端口配置 */
    GPIO_InitStructure.GPIO_Pin = MOTORD1_PIN | MOTORD2_PIN; // 同时配置两个编码器输入引脚 PA10,PA1
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 中速模式（满足编码器信号要求）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;           // 开漏输出（防止总线冲突）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // 上拉电阻使能（保证默认高电平）
    GPIO_Init(MOTORD1_PORT, &GPIO_InitStructure);           // 应用配置到指定GPIO端口

    /* 定时器基本参数配置 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);          // 加载默认时基参数
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;               // 预分频器清零（直接使用输入频率）
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;   // 自动重装载值（决定计数范围）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   // 时钟分频因子为1（不分频）
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(MOTORD1_ID, &TIM_TimeBaseStructure);    // 初始化TIM5时基单元

    /* 编码器接口模式配置 */
    TIM_EncoderInterfaceConfig(MOTORD1_ID,                // 选择TIM5
        TIM_EncoderMode_TI12,                // 编码器模式3（TI1和TI2边沿均触发计数）
        TIM_ICPolarity_Rising,               // 通道1上升沿有效
        TIM_ICPolarity_Rising);              // 通道2上升沿有效
    
    /* 输入捕获参数初始化 */
    TIM_ICStructInit(&TIM_ICInitStructure);   // 加载默认输入捕获参数
    TIM_ICInitStructure.TIM_ICFilter = 0;     // 无输入滤波（最大响应速度）
    TIM_ICInit(MOTORD1_ID, &TIM_ICInitStructure);  // 应用输入捕获配置

    /* 中断标志处理 */
    TIM_ClearFlag(MOTORD1_ID, TIM_FLAG_Update);    // 清除更新中断标志（避免误触发）
    TIM_ITConfig(MOTORD1_ID, TIM_IT_Update, ENABLE); // 使能定时器更新中断

    /* 启动计数器 */
    TIM_SetCounter(MOTORD1_ID,0);       // 计数器归零（初始化位置）
    TIM_Cmd(MOTORD1_ID, ENABLE);        // 使能TIM5计数器
}

/**************************************************************************
函数功能：读取编码器计数值并复位计数器
入口参数：TIMX - 定时器编号（2/3/4/5）
返 回 值：int - 编码器相对计数值（有符号数）
工作流程：
1. 读取指定定时器的计数器值（16位有符号数）
2. 立即将计数器清零
3. 返回读取的脉冲计数值
注意事项：
1. 使用short类型转换保证正确处理负数（反转计数）
2. 返回值范围：-ENCODER_TIM_PERIOD/2 到 +ENCODER_TIM_PERIOD/2
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
    int Encoder_TIM;    
    switch(TIMX)
    {
        case 2:  
            Encoder_TIM = (short)TIM2->CNT;   // 读取TIM2的16位有符号计数值
            TIM2->CNT = 0;                   // 立即清零计数器
            break;
        case 3:  
            Encoder_TIM = (short)TIM3->CNT;   // 读取TIM3计数值
            TIM3->CNT = 0;
            break;
        case 4:  
            Encoder_TIM = (short)TIM4->CNT;   // 读取TIM4计数值
            TIM4->CNT = 0;
            break;    
        case 5:  
            Encoder_TIM = (short)TIM5->CNT;   // 读取TIM5计数值
            TIM5->CNT = 0;
            break;    
        default: 
            Encoder_TIM = 0;                 // 错误参数返回0
    }
    return Encoder_TIM;  // 返回带符号的脉冲差值
}

/**************************************************************************
函数功能：TIM2中断服务函数（溢出处理）
设计说明：
1. 当计数器超过ENCODER_TIM_PERIOD时触发
2. 主要用于检测计数器溢出情况
3. 当前实现仅清除标志位，可根据需要扩展功能
**************************************************************************/
void TIM2_IRQHandler(void)
{                       
    if(TIM2->SR & 0X0001) // 检测更新中断标志（UIF位）
    {                                            
        // 可在此处添加溢出处理逻辑
        // 例如：g_overflow_count++;  // 全局溢出计数器
    }                   
    TIM2->SR &= ~(1<<0); // 清除更新中断标志（必须操作）
}

// TIM3中断服务函数（实现逻辑与TIM2相同）
void TIM3_IRQHandler(void)
{
    if(TIM3->SR & 0X0001) 
    {                        
        // 溢出处理逻辑（可选）
    }
    TIM3->SR &= ~(1<<0);  // 清除中断标志
}

// TIM4中断服务函数
void TIM4_IRQHandler(void)
{
    if(TIM4->SR & 0X0001)
    {
        // 可添加特定应用逻辑
    }
    TIM4->SR &= ~(1<<0);  // 必须清除标志位
}

// TIM5中断服务函数
void TIM5_IRQHandler(void)
{
    if(TIM5->SR & 0X0001) 
    {
        // 处理定时器溢出事件
    }
    TIM5->SR &= ~(1<<0);  // 清除中断请求标志
}

/**************************************************************************
函数功能：TIM8刹车事件&TIM12全局中断服务函数
硬件关联：
- 处理TIM8刹车事件和TIM12定时器溢出中断
- 共享中断向量（根据STM32中断映射设计）
注意事项：
1. 本中断服务函数同时响应两个事件：
   - TIM8刹车事件（Break event）
   - TIM12定时器溢出中断
2. 实际使用中需通过SR寄存器区分中断源
3. 清除标志位是必要操作，否则会持续进入中断
**************************************************************************/
void TIM8_BRK_TIM12_IRQHandler(void)
{
    /* 检测TIM12溢出中断标志（Update Interrupt Flag）*/
    if(TIM12->SR & 0X0001)  // 0X0001对应TIM_SR_UIF位（第0位）
    {    
        /* 可在此处添加中断处理逻辑，例如：
        - 编码器计数溢出处理
        - 系统保护机制
        - 异常状态监测
        */
    }
    
    /* 强制清除中断标志位（必须操作）*/
    TIM12->SR &= ~(1<<0);  // 清除TIM12更新中断标志
    
    /* 注意：
    1. 如需处理TIM8刹车事件，应在此处添加：
       if(TIM8->SR & TIM_SR_BIF) 
       {
           // 刹车事件处理
           TIM8->SR &= ~TIM_SR_BIF;
       }
    2. 本示例仅处理TIM12中断，保持原有代码结构不变
    */
}


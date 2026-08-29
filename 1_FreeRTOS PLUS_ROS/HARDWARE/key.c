/**
  ******************************************************************************
  * @file    key.c
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
#include "key.h"

/**************************************************************************
函数功能：按键硬件初始化
硬件配置：
- 使用指定的KEY_PIN引脚
- 上拉输入模式（默认高电平，按下接地）
- 100MHz GPIO速度（适应快速检测）
注意事项：
1. 需在头文件正确定义：
   - KEY_GPIO_CLK：按键所在GPIO组的时钟
   - KEY_PIN：按键对应的引脚编号
   - KEY_PORT：按键所在的GPIO端口
2. 按键硬件需接上拉电阻，按下时接地
**************************************************************************/
void KEY_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    // 使能GPIO时钟（根据硬件连接选择AHB1总线）
    RCC_AHB1PeriphClockCmd(KEY_GPIO_CLK, ENABLE);
    
    // 配置按键引脚参数
    GPIO_InitStructure.GPIO_Pin = KEY_PIN;        // 选择按键对应引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   // 普通输入模式（非中断）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 高速模式（响应更及时）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    // 内部上拉（默认高电平）
    
    // 应用GPIO配置
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);  // 初始化按键引脚
}

/**************************************************************************
函数功能：简易按键单击检测
返回逻辑：
- 当检测到下降沿（按下动作）返回1
- 松开后自动复位检测标志
- 支持单次触发（需松开后才能再次检测）
消抖机制：依赖调用频率（需在主循环中快速轮询）
**************************************************************************/
u8 click(void)
{
    static u8 flag_key = 1;  // 按键状态标志（1：可检测状态）
    
    if(flag_key && (KEY == 0))  // 检测下降沿（从高到低）
    {
        flag_key = 0;  // 锁定检测标志
        return 1;      // 返回按下事件
    }
    else if(KEY == 1)  // 检测按键释放
    {
        flag_key = 1;  // 重置检测标志
    }
    return 0;  // 无有效动作
}

/**************************************************************************
函数功能：简易毫秒级延时
延时估算：基于72MHz主频
          内循环约执行4条指令（约0.055us）
          总延时 ≈ 50 * 50 * 0.055us ≈ 137.5us
注意事项：
1. 实际延时时间与编译器优化等级相关
2. 不适用于精确时序控制
3. 建议改用定时器实现精确延时
**************************************************************************/
void Delay_ms(void)
{
    int ii, i;    
    for(ii = 0; ii < 50; ii++)  // 外层循环
    {
        for(i = 0; i < 50; i++);  // 内层循环（空指令实现延时）
    }    
}

/**************************************************************************
函数功能：支持单击/双击检测的增强型按键扫描
参数说明：
- time：双击检测时间窗口（单位：主循环周期次数）
返回值：
0：无动作
1：单击
2：双击
工作原理：
1. 通过Forever_count检测长按（超过time值则重置状态）
2. 在指定时间窗口内检测两次按下判定为双击
3. 超过时间窗口的单独按下判定为单击
**************************************************************************/
u8 click_N_Double(u8 time)
{
    // 状态保持变量（static确保保持状态）
    static u8 flag_key = 0;       // 按下标志
    static u8 count_key = 0;      // 按键计数锁
    static u8 double_key = 0;    // 双击检测标志
    static u16 count_single = 0;  // 单击计时器
    static u16 Forever_count = 0;// 长按计时器

    // 实时更新长按计时
    if(KEY == 0)  Forever_count++;   // 按键持续按下时递增
    else          Forever_count = 0;  // 松开时重置

    // 下降沿检测
    if((KEY == 0) && (flag_key == 0)) 
    {
        flag_key = 1;  // 标记按键按下
    }

    // 首次按下处理
    if(count_key == 0)
    {
        if(flag_key == 1) 
        {
            double_key++;        // 按下次数递增
            count_key = 1;       // 锁定计数
        }
        
        // 双击判定
        if(double_key == 2) 
        {
            double_key = 0;       // 重置双击计数器
            count_single = 0;    // 重置单击计时
            return 2;            // 返回双击事件
        }
    }

    // 按键释放处理
    if(KEY == 1)
    {
        flag_key = 0;   // 清除按下标志
        count_key = 0;  // 解锁计数
    }

    // 单击超时处理
    if(double_key == 1)
    {
        count_single++;  // 单击计时递增
        
        // 单击有效判定
        if((count_single > time) && (Forever_count < time))
        {
            double_key = 0;
            count_single = 0;    
            return 1;  // 返回单击事件
        }
        
        // 长按超时重置
        if(Forever_count > time)
        {
            double_key = 0;
            count_single = 0;    
        }
    }
    
    return 0;  // 默认返回无动作
}
/**************************************************************************
函数功能：MPU6050专用按键双击检测（不可重入版本）
设计特性：
- 基于静态变量的状态机实现
- 支持单击/双击判别
- 专为MPU6050模块操作优化（防抖参数适配）
参数说明：
- time：双击时间窗口（单位：主循环周期次数）
          实际时间 = time × 主循环周期
注意事项：
1. 由于使用静态变量，多个按键需创建不同函数实例
2. 建议主循环调用间隔10-20ms
3. MPU6050应用场景需适当增大time值（防止运动误触）
**************************************************************************/
u8 click_N_Double_MPU6050(u8 time)
{
    // 状态变量（保持检测连续性）
    static u8 flag_key = 0;      // 下降沿检测标志
    static u8 count_key = 0;     // 按键锁定标志
    static u8 double_key = 0;    // 双击事件计数器
    static u16 count_single = 0; // 单击有效计时
    static u16 Forever_count = 0;// 持续按下计时

    /* 实时更新持续按下计时器 */
    if(KEY == 0)  Forever_count++;  // 按键按下时递增
    else          Forever_count = 0;// 松开重置

    /* 下降沿检测（按键按下瞬间） */
    if((KEY == 0) && (flag_key == 0)) 
    {
        flag_key = 1;  // 标记按键按下事件
    }

    /* 首次按键处理 */
    if(count_key == 0) // 未锁定状态
    {
        if(flag_key == 1) // 检测到有效按下
        {
            double_key++;  // 按下次数累计
            count_key = 1; // 进入锁定状态
        }
        
        /* 双击判定（快速连续两次按下） */
        if(double_key == 2) 
        {
            double_key = 0;     // 重置双击计数器
            count_single = 0;   // 重置单击计时
            return 2;           // 返回双击事件
        }
    }

    /* 按键释放处理 */
    if(KEY == 1) // 检测按键松开
    {
        flag_key = 0;   // 清除按下标志
        count_key = 0;  // 解除锁定状态
    }

    /* 单击超时处理 */
    if(double_key == 1) // 处于单击待确认状态
    {
        count_single++;  // 递增单击计时
        
        /* 单击有效条件 */
        if((count_single > time) && (Forever_count < time))
        {
            double_key = 0;
            count_single = 0;    
            return 1;  // 返回单击事件
        }
        
        /* 长按超时重置 */
        if(Forever_count > time) 
        {
            double_key = 0;
            count_single = 0;    
        }
    }
    
    return 0; // 默认无事件
}

/**************************************************************************
函数功能：固定时长长按检测（3秒）
实现原理：
- 持续统计按键按下周期数
- 超过阈值判定为长按
硬件要求：
- 依赖主循环调用周期稳定（示例按200ms周期设计）
改进建议：
1. 改用时间戳计算提升精度
2. 增加参数化时长设定
3. 添加松开后返回机制
**************************************************************************/
u8 Long_Press(void)
{
    // 状态保持变量
    static u16 Long_Press_count = 0; // 持续按下计数器
    static u8 Long_Press = 0;        // 长按事件标志

    /* 按键按下时递增计数器 */
    if((Long_Press == 0) && (KEY == 0)) 
    {
        Long_Press_count++;  // 每个循环周期+1
    }
    else
    {
        Long_Press_count = 0; // 松开或已触发时重置
    }

    /* 长按触发条件（约3秒 @200ms/次） */
    if(Long_Press_count > 15)    
    {
        Long_Press = 1;          // 设置标志位
        Long_Press_count = 0;    // 重置计数器
        return 1;                // 返回长按事件
    }                
    
    /* 长按事件复位 */
    if(Long_Press == 1) 
    {
        Long_Press = 0;  // 单次触发后复位
    }
    
    return 0; // 默认无长按
}

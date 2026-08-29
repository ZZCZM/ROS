/**
  ******************************************************************************
  * @file    LED.c
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
#include "led.h"
extern uint8_t isReadyForStop;  // 外部变量声明（用于系统停止准备状态）
int Led_Count = Led_Init_Count; // LED闪烁周期控制（单位：FreeRTOS心跳节拍）

/**************************************************************************
函数功能：LED硬件接口初始化
硬件配置：
- 配置LED引脚为高速推挽输出
- 初始状态设置为熄灭（根据硬件设计可能相反）
关键参数：
- LED_PIN：实际连接的LED引脚宏定义
- LED_GPIO_CLK：GPIO所在总线时钟宏定义
注意事项：
1. 需在头文件正确定义LED_PIN和LED_PORT
2. 若LED低电平点亮，需将GPIO_ResetBits改为GPIO_SetBits
**************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO时钟（根据硬件连接选择AHB1总线）
    RCC_AHB1PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    
    // 配置LED引脚参数
    GPIO_InitStructure.GPIO_Pin = LED_PIN;        // 选择LED对应引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // 推挽输出（强驱动能力）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 高速模式（快速响应）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     // 上拉（确保默认高电平）
    
    // 应用GPIO配置
    GPIO_Init(LED_PORT, &GPIO_InitStructure);  // 初始化LED引脚
    
    // 初始化LED状态（根据硬件设计选择置位或复位）
    GPIO_ResetBits(LED_PORT, LED_PIN);  // 假设低电平点亮LED
    
    // 初始化闪烁周期计数器
    Led_Count = Led_Init_Count;  // 从宏定义加载初始闪烁间隔
}

/**************************************************************************
函数功能：蜂鸣器硬件接口初始化
硬件特性：
- 使用推挽输出模式驱动有源蜂鸣器
- 100MHz驱动速度确保快速响应
接线说明：
- 蜂鸣器正极接VCC，负极接三极管由GPIO控制高低电平
- 需串联限流电阻（通常220Ω-1kΩ）
**************************************************************************/
void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO时钟
    RCC_AHB1PeriphClockCmd(Buzzer_GPIO_CLK, ENABLE);
    
    // 配置蜂鸣器引脚参数
    GPIO_InitStructure.GPIO_Pin = Buzzer_PIN;     // 蜂鸣器控制引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // 推挽输出（直接驱动蜂鸣器）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 高速驱动
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;     // 下拉（默认静音）
    
    // 应用GPIO配置
    GPIO_Init(Buzzer_PORT, &GPIO_InitStructure);  // 初始化蜂鸣器控制引脚
}

/**************************************************************************
函数功能：LED闪烁任务（FreeRTOS任务函数）
运行机制：
- 周期性切换LED状态
- 使用系统延时保证时序精度
- 任务优先级应设为较低级别
参数说明：
- pvParameters：任务参数（本例未使用）
注意事项：
1. 需在FreeRTOSConfig.h中配置正确的时间片大小
2. Led_Count变量应在其他任务中修改以实现动态频率控制
**************************************************************************/
void Led_task(void *pvParameters)
{
    while(1)
    {
        // 翻转LED状态（硬件相关操作）
        LED = ~LED;  // 等效于GPIO_ToggleBits(LED_PORT, LED_PIN)
        
        // 系统延时（单位：FreeRTOS心跳节拍）
        vTaskDelay(Led_Count);  // 使用系统延时保证任务调度
        
    }
}

/**************************************************************************
函数功能：简易LED闪烁控制
实现原理：
- 通过静态变量累计调用次数
- 达到指定周期后切换LED状态
参数说明：
- time：闪烁周期（单位：函数调用次数）
          0表示常亮，非零值控制闪烁频率
使用注意：
1. 需在定时中断或主循环中定期调用
2. 实际闪烁频率 = 系统调用频率 / (time*2)
**************************************************************************/
void Led_Flash(u16 time)
{
    static int temp = 0;  // 周期计数器（保持调用间隔）
    
    if(0 == time) 
    {
        LED = 0;  // 强制常亮模式
    }
    else if(++temp == time) 
    {
        LED = ~LED;  // 达到周期时切换状态
        temp = 0;     // 重置计数器
    }
}


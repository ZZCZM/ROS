/**
  ******************************************************************************
  * @file    delay.c
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
#include "delay.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用OS,则包括下面的头文件即可
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h"
#endif


static u8  fac_us = 0;                  // us延时倍乘数
static u16 fac_ms = 0;                  // ms延时倍乘数,在OS下,代表每个节拍的ms数

static u32 sysTickCnt = 0;              // 系统节拍计数器
extern void xPortSysTickHandler(void);    // FreeRTOS系统节拍处理函数

// systick中断服务函数,使用OS时用到
void SysTick_Handler(void)
{	
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) // 系统已经运行
    {
        xPortSysTickHandler();  // 调用FreeRTOS系统节拍处理函数
    }
    sysTickCnt++;  // 递增系统节拍计数器（未使用OS时提供基准）
}

/********************************************************
* getSysTickCnt()
* 调度开启之前 返回 sysTickCnt
* 调度开启之后 返回 xTaskGetTickCount()
*********************************************************/
u32 getSysTickCnt(void)
{
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) /* 系统已经运行 */
    {
        return xTaskGetTickCount();  // 返回FreeRTOS的节拍计数
    }
    else
    {
        return sysTickCnt;  // 返回自定义的节拍计数
    }
}

// 初始化延迟函数
// SYSTICK的时钟固定为AHB时钟，基础例程里面SYSTICK时钟频率为AHB/8
// 这里为了兼容FreeRTOS，所以将SYSTICK的时钟频率改为AHB的频率！
// SYSCLK: 系统时钟频率（MHz）
void delay_init(u8 SYSCLK)
{
    u32 reload;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);  // 设置SYSTICK时钟源为AHB时钟
    fac_us = SYSCLK;  // 不论是否使用OS，fac_us都需要使用（每微秒的节拍数）
    reload = SYSCLK;  // 每秒钟的计数次数，单位为M
    reload *= 1000000 / configTICK_RATE_HZ;  // 根据configTICK_RATE_HZ设定溢出时间
                                              // reload为24位寄存器,最大值:16777216,在168MHz下,约合0.0998s左右
    fac_ms = 1000 / configTICK_RATE_HZ;  // 代表OS可以延时的最少单位（每毫秒的节拍数）
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // 开启SYSTICK中断
    SysTick->LOAD = reload;  // 每1/configTICK_RATE_HZ断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  // 开启SYSTICK
}

// 延时nus
// nus: 要延时的us数
// nus: 0~204522252 (最大值即2^32/fac_us@fac_us=168)
void delay_us(u32 nus)
{		
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;  // LOAD的值
    ticks = nus * fac_us;  // 需要的节拍数
    told = SysTick->VAL;  // 刚进入时的计数器值
    while(1)
    {
        tnow = SysTick->VAL;  // 当前计数器值
        if(tnow != told)
        {	    
            if(tnow < told) 
                tcnt += told - tnow;  // SYSTICK是一个递减的计数器
            else 
                tcnt += reload - tnow + told;
            told = tnow;
            if(tcnt >= ticks) break;  // 时间超过或等于要延迟的时间,则退出.
        }  
    };										    
}

// 延时nms
// nms: 要延时的ms数
// nms: 0~65535
void delay_ms(u32 nms)
{	
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) // 系统已经运行
    {		
        if(nms >= fac_ms)  // 延时的时间大于OS的最少时间周期
        { 
            vTaskDelay(nms / fac_ms);  // FreeRTOS延时
        }
        nms %= fac_ms;  // OS已经无法提供这么小的延时了,采用普通方式延时    
    }
    delay_us((u32)(nms * 1000));  // 普通方式延时
}

// 延时nms,不会引起任务调度
// nms: 要延时的ms数
void delay_xms(u32 nms)
{
    u32 i;
    for(i = 0; i < nms; i++) delay_us(1000);  // 每次延时1ms
}

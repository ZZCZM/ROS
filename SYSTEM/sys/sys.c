/**
  ******************************************************************************
  * @file    sys.c
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
#include "sys.h"  
// THUMB指令不支持汇编内联
// 采用如下方法实现执行汇编指令WFI  
// WFI指令用于等待中断，节省功耗
__asm void WFI_SET(void)
{
    WFI;  // 执行WFI指令，进入等待中断状态
}

// 关闭所有中断（但是不包括fault和NMI中断）
// CPSID I指令用于关闭可屏蔽中断
__asm void INTX_DISABLE(void)
{
    CPSID   I  // 关闭所有可屏蔽中断（中断优先级低于fault和NMI中断）
    BX      LR  // 返回调用者
}

// 开启所有中断
// CPSIE I指令用于开启可屏蔽中断
__asm void INTX_ENABLE(void)
{
    CPSIE   I  // 开启所有可屏蔽中断
    BX      LR  // 返回调用者
}

// 设置栈顶地址
// addr: 栈顶地址
// MSR指令用于设置主堆栈指针（MSP）
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0  // 将r0中的值设置为主堆栈指针（MSP）
    BX r14       // 返回调用者
}
















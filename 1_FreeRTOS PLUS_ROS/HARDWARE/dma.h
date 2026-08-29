/**
  ******************************************************************************
  * @file    dma.h
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
#ifndef __DMA_H
#define __DMA_H

#include "stm32f10x_dma.h"  
#include "sys.h"            
#include "system.h"         
// DMA初始化函数
// 该函数用于初始化指定的DMA通道
// @param DMA_CHx DMA通道指针，例如 DMA1_Channel4
// @param cpar 控制寄存器地址 (例如 &USART1->DR)
// @param cmar 内存地址
// @param cndtr 控制寄存器中的数据传输数量，为16位值
// @detail 初始化步骤：
//         1. 设置外设地址/内存地址
//         2. 设置数据传输方向（外设到内存或内存到外设）
//         3. 设置数据传输数量寄存器默值为16位
//         4. 设置外设模式
//         5. 设置通道DMA为不循环
//         6. 停止内存模式
void MYDMA_Init2(DMA_Channel_TypeDef* DMA_CHx, u32 cpar, u32 cmar, u16 cndtr);

// 使能DMA通道
// 该函数用于使能指定的DMA通道
// @param DMA_CHx DMA通道指针
// @detail 执行步骤：
//         1. 使能通道DMA
//         2. 设置数据传输方向
//         3. 使能通道
//         4. 等待数据传输完成
// @warning 在为中断式执行时，获取数据传输完成标志
void MYDMA_Enable2(DMA_Channel_TypeDef* DMA_CHx);

// 使用DMA进行格式化输出
// 该函数用于使用DMA进行格式化输出，类似于printf功能
// @param format 格式字符串，支持 %d, %f, %s 等标准格式
// @param ... 可变参数列表
// @detail 执行步骤：
//         1. 使能DMA2_Channel7实现平台输出
//         2. 设置缓冲区大小为512字节
//         3. 支持DMA忙时等待
// @note 该函数应与SART1使能系统配套使用
void DMA_printf(const char *format, ...);

// 使用DMA发送缓冲区数据
// 该函数用于通过DMA发送指定长度的缓冲区数据
// @param buffer 内存地址指针
// @param length 数据长度
// @return u32 返回DMA传输的字节数
// @detail 模式说明：
//         1. 在DMA传输时，DMA不会自动填充数据
//         2. 在DMA忙时返回0，表示没有成功传输数据
//         3. 最大传输4096字节的数据
// @warning 调用该函数时应注意，有效直必须小于
u32 USART_SendBuffer(const char* buffer, u32 length);


#endif



/**
  ******************************************************************************
  * @file   can.h
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
#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
#include "system.h"
 
/****** CAN总线中断配置 ******/
// 接收中断使能配置（影响CAN_RX0_IRQHandler实现）
// 开发阶段建议启用，量产时根据实际需求调整
#define CAN_RX0_INT_ENABLE	1  // 1=使能RX0接收中断（需实现中断服务函数）

/****** CAN总线硬件配置（使用CAN2） ******/
/*-- 发送引脚配置 --*/
#define CAN_TX_PORT GPIOB        // TX引脚端口：GPIOB
#define CAN_TX_PIN GPIO_Pin_13   // TX引脚：PB13（物理引脚：DIP64）
#define CAN_TX_PIN_Source GPIO_PinSource13//引脚源编号13
#define CAN_TX_GPIO_CLK RCC_AHB1Periph_GPIOB  // GPIO时钟（AHB1总线）
#define CAN_TX_USART_CLK RCC_APB1Periph_CAN2  // CAN外设时钟（APB1总线）

/*-- 接收引脚配置 --*/
#define CAN_RX_PORT GPIOB        // RX引脚端口：GPIOB  
#define CAN_RX_PIN GPIO_Pin_12    // RX引脚：PB12（物理引脚：DIP63）
#define CAN_RX_PIN_Source GPIO_PinSource12//引脚源编号12
#define CAN_RX_GPIO_CLK RCC_AHB1Periph_GPIOB  
#define CAN_RX_USART_CLK RCC_APB1Periph_CAN2

/*-- 复用功能配置 --*/  
#define CAN_AF GPIO_AF_CAN2       // 引脚复用功能映射（参见数据手册）
#define CAN_ID CAN2               // 使用CAN2外设
#define CAN_RX0_IRQn CAN2_RX0_IRQn // 接收中断编号（需在stm32f4xx_it.c实现）

/****** CAN总线控制函数声明 ******/
/* 模式初始化函数
   @param tsjw  同步跳转宽度 1-4 (推荐1)
   @param tbs2  时间段2长度 1-8 
   @param tbs1  时间段1长度 1-16
   @param brp   波特率分频因子 1-1024
   @param mode  工作模式（0:普通模式 1:环回模式 2:静默模式 3:静默环回） */
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);

/* 报文发送核心函数 
   @param id    报文ID（标准帧：0x000~0x7FF，扩展帧：0x00000000~0x1FFFFFFF）
   @param ide   标识符扩展 0-标准帧 1-扩展帧
   @param rtr   远程帧请求 0-数据帧 1-远程帧
   @param len   数据长度（0-8字节）
   @param dat   数据缓冲区指针 */
u8 CAN_Tx_Msg(u32 id,u8 ide,u8 rtr,u8 len,u8 *dat);  

/* 接收报文查询函数
   @param fifox 邮箱号（0:FIFO0 1:FIFO1）
   @return 0-无数据 1-有数据待读取 */
u8 CAN_Msg_Pend(u8 fifox);  

/* 接收报文解析函数（需先调用CAN_Msg_Pend）
   @param fifox 邮箱号
   @param id    输出报文ID
   @param ide   输出标识符类型
   @param rtr   输出帧类型
   @param len   输出数据长度
   @param dat   输出数据缓冲区 */
void CAN_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat);

/* 发送状态查询函数 
   @param mbox  邮箱号（0-2）
   @return 0-发送中 1-发送成功 2-发送失败 */
u8 CAN_Tx_Staus(u8 mbox);  // 注意：函数名疑似拼写错误（应为Status）

/* 快速发送接口（封装标准帧发送） 
   @param msg   数据指针（首字节为ID高8位）
   @param len   数据长度
   @return 0-失败 1-成功 */
u8 CAN_Send_Msg(u8* msg,u8 len);  

/* 快速接收接口（标准帧接收）
   @param buf   数据缓冲区（首字节为ID高8位）
   @return 接收数据长度 */
u8 CAN_Receive_Msg(u8 *buf);  

/****** 调试专用函数 ******/
u8 CAN_Send_MsgTEST(u8* msg,u8 len);  // 测试用发送（带LED指示）
u8 CAN_Send_Num(u32 id,u8* msg);      // 数字编码发送（自定义协议）


#endif


















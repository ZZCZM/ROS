/**
  ******************************************************************************
  * @file    can.c
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
#include "can.h"
#include "system.h"
/**************************************************************************
Function: CAN initialization
Input   : tsjw：Resynchronize the jump time unit, Scope: 1 ~ 3;
 			    tbs2：Time unit of time period 2, range :1~8;
 			    tbs1：Time unit of time period 1, range :1~16;
 			    brp ：Baud rate divider, range :1 to 1024;(We're actually going to add 1, which is 1 to 1024) tq=(brp)*tpclk1
 			    mode：0, normal mode;1. Loop mode;
Output  : 0- Initialization successful;Other - initialization failed
Note: none of the entry parameters (except mode) can be 0
函数功能：CAN初始化
入口参数：tsjw：重新同步跳跃时间单元，范围:1~3;
 			    tbs2：时间段2的时间单元，范围:1~8;
 			    tbs1：时间段1的时间单元，范围:1~16;
 			    brp ：波特率分频器，范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
 			    mode：0,普通模式;1,回环模式;
返回  值：0-初始化成功; 其他-初始化失败
注意：入口参数(除了mode)均不能为0
波特率/Baud rate=Fpclk1/((tbs1+tbs2+1)*brp)，Fpclk1为36M
                =42M/((3+2+1)*6)
						    =1M
**************************************************************************/
u8 CAN_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;  // GPIO初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;    // NVIC初始化结构体
    u16 i = 0;                              // 计数器，用于超时检测

    // 检查参数有效性
    if(tsjw == 0 || tbs2 == 0 || tbs1 == 0 || brp == 0) return 1;  // 参数无效，返回1

    // 参数调整（减1用于设置）
    tsjw -= 1;  // 先减去1，再用于设置
    tbs2 -= 1;
    tbs1 -= 1;
    brp -= 1;

    // 使能相关时钟
    RCC_AHB1PeriphClockCmd(CAN_TX_GPIO_CLK, ENABLE);  // 使能GPIOB时钟
    RCC_APB1PeriphClockCmd(CAN_TX_USART_CLK, ENABLE);  // 使能CAN2时钟

    // 初始化GPIO
    GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN | CAN_RX_PIN;  // PB12 PB13
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  // 复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  // 上拉
    GPIO_Init(CAN_TX_PORT, &GPIO_InitStructure);  // 初始化PB12 PB13

    // 引脚复用映射配置
    GPIO_PinAFConfig(CAN_TX_PORT, CAN_TX_PIN_Source, CAN_AF);  // GPIOB13复用为CAN2
    GPIO_PinAFConfig(CAN_RX_PORT, CAN_RX_PIN_Source, CAN_AF);  // GPIOB12复用为CAN2

    // 退出睡眠模式（同时设置所有位为0）
    CAN_ID->MCR = 0x0000;
    // 请求CAN进入初始化模式
    CAN_ID->MCR |= 1 << 0;
    // 检查是否成功进入初始化模式
    while((CAN_ID->MSR & 1 << 0) == 0)
    {
        i++;
        if(i > 100) return 2;  // 进入初始化模式失败，返回2
    }

    // 非时间触发通信模式
    CAN_ID->MCR |= 0 << 7;
    // 软件自动离线管理
    CAN_ID->MCR |= 0 << 6;
    // 睡眠模式通过软件唤醒（清除CAN->MCR的SLEEP位）
    CAN_ID->MCR |= 0 << 5;
    // 禁止报文自动传送
    CAN_ID->MCR |= 1 << 4;
    // 报文不锁定，新的覆盖旧的
    CAN_ID->MCR |= 0 << 3;
    // 优先级由报文标识符决定
    CAN_ID->MCR |= 0 << 2;

    // 清除原来的设置
    CAN_ID->BTR = 0x00000000;
    // 模式设置：0,普通模式; 1,回环模式;
    CAN_ID->BTR |= (mode << 30);
    // 重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
    CAN_ID->BTR |= (tsjw << 24);
    // Tbs2 = Tbs2 + 1个时间单位
    CAN_ID->BTR |= (tbs2 << 20);
    // Tbs1 = Tbs1 + 1个时间单位
    CAN_ID->BTR |= (tbs1 << 16);
    // 分频系数(Fdiv)为brp+1，波特率: Fpclk1 / ((Tbs1 + Tbs2 + 1) * Fdiv)
    CAN_ID->BTR |= (brp << 0);

    // 请求CAN退出初始化模式
    CAN_ID->MCR &= ~(1 << 0);
    // 检查是否成功退出初始化模式
    while((CAN_ID->MSR & 1 << 0) == 1)
    {
        i++;
        if(i > 0XFFF0) return 3;  // 退出初始化模式失败，返回3
    }

    /*** 过滤器初始化 ***/
    // 过滤器组工作在初始化模式
    CAN_ID->FMR |= 1 << 0;
    // 过滤器0不激活
    CAN_ID->FA1R &= ~(1 << 0);
    // 过滤器位宽为32位
    CAN_ID->FS1R |= 1 << 0;
    // 过滤器0工作在标识符屏蔽位模式
    CAN_ID->FM1R |= 0 << 0;
    // 过滤器0关联到FIFO0
    CAN_ID->FFA1R |= 0 << 0;
    // 32位ID
    CAN_ID->sFilterRegister[0].FR1 = 0X00000000;
    // 32位MASK
    CAN_ID->sFilterRegister[0].FR2 = 0X00000000;
    // 激活过滤器0
    CAN_ID->FA1R |= 1 << 0;
    // 过滤器组进入正常模式
    CAN_ID->FMR &= 0 << 0;

#if CAN_RX0_INT_ENABLE
    // 使能中断接收
    CAN_ID->IER |= 1 << 1;  // FIFO0消息挂号中断允许

    // 配置CAN接收中断
    NVIC_InitStructure.NVIC_IRQChannel = CAN_RX0_IRQn;  // CAN_RX0中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);  // 根据指定的参数初始化NVIC寄存器

    CAN_ITConfig(CAN_ID, CAN_IT_FMP0, ENABLE);  // 使能FIFO0满中断
#endif

    return 0;  // 初始化成功，返回0
}
 
/**************************************************************************
函数功能：CAN发送数据
入口参数：id:标准ID(11位)/扩展ID(11位+18位)	    
			    ide:0,标准帧;1,扩展帧
			    rtr:0,数据帧;1,远程帧
			    len:要发送的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
			    *dat:数据指针.
返回  值：0~3,邮箱编号.0XFF,无有效邮箱
**************************************************************************/
u8 CAN_Tx_Msg(u32 id, u8 ide, u8 rtr, u8 len, u8 *dat)
{	   
    u8 mbox;	  
    // 检查邮箱是否为空
    if(CAN_ID->TSR & (1 << 26)) mbox = 0;  // 邮箱0为空
    else if(CAN_ID->TSR & (1 << 27)) mbox = 1;  // 邮箱1为空
    else if(CAN_ID->TSR & (1 << 28)) mbox = 2;  // 邮箱2为空
    else return 0XFF;  // 无空邮箱,无法发送

    // 清除之前的设置
    CAN_ID->sTxMailBox[mbox].TIR = 0;

    // 设置标识符
    if(ide == 0)  // 标准帧
    {
        id &= 0x7ff;  // 取低11位stdid
        id <<= 21;    // 标识符左移21位
    }
    else          // 扩展帧
    {
        id &= 0X1FFFFFFF;  // 取低32位extid
        id <<= 3;          // 标识符左移3位
    }
    CAN_ID->sTxMailBox[mbox].TIR |= id;
    CAN_ID->sTxMailBox[mbox].TIR |= (ide << 2);  // 设置IDE位
    CAN_ID->sTxMailBox[mbox].TIR |= (rtr << 1);  // 设置RTR位

    // 设置数据长度代码（DLC）
    len &= 0X0F;  // 取低4位
    CAN_ID->sTxMailBox[mbox].TDTR &= ~(0X0000000F);
    CAN_ID->sTxMailBox[mbox].TDTR |= len;

    // 存储待发送的数据到邮箱
    CAN_ID->sTxMailBox[mbox].TDHR = (((u32)dat[7] << 24) |
                                      ((u32)dat[6] << 16) |
                                      ((u32)dat[5] << 8) |
                                      ((u32)dat[4]));
    CAN_ID->sTxMailBox[mbox].TDLR = (((u32)dat[3] << 24) |
                                      ((u32)dat[2] << 16) |
                                      ((u32)dat[1] << 8) |
                                      ((u32)dat[0]));

    // 请求发送邮箱数据
    CAN_ID->sTxMailBox[mbox].TIR |= (1 << 0);
    return mbox;
}

/**************************************************************************
【函数名称】CAN_Tx_Staus
【功能概述】获得发送状态
            - 根据指定的邮箱编号获取发送状态
            - 返回发送完成、发送失败或挂起状态
【输入参数】mbox: 邮箱编号
【返回值】0, 挂起; 0X05, 发送失败; 0X07, 发送成功
【关键寄存器】TSR: 发送状态寄存器
【关键位】RQCP0, TXOK0, TME0: 邮箱0的请求完成、发送成功标志和传输邮件标志
         RQCP1, TXOK1, TME1: 邮箱1的请求完成、发送成功标志和传输邮件标志
         RQCP2, TXOK2, TME2: 邮箱2的请求完成、发送成功标志和传输邮件标志
**************************************************************************/
u8 CAN_Tx_Staus(u8 mbox)
{	
    u8 sta = 0;					    
    switch (mbox)
    {
        case 0: 
            sta |= (CAN_ID->TSR & (1 << 0));  // RQCP0 (请求完成标志)
            sta |= (CAN_ID->TSR & (1 << 1));  // TXOK0 (传输成功标志)
            sta |= ((CAN_ID->TSR & (1 << 26)) >> 24);  // TME0 (传输邮件标志)
            break;
        case 1: 
            sta |= (CAN_ID->TSR & (1 << 8)) >> 8;  // RQCP1 (请求完成标志)
            sta |= (CAN_ID->TSR & (1 << 9)) >> 8;  // TXOK1 (传输成功标志)
            sta |= ((CAN_ID->TSR & (1 << 27)) >> 25);  // TME1 (传输邮件标志)
            break;
        case 2: 
            sta |= (CAN_ID->TSR & (1 << 16)) >> 16;  // RQCP2 (请求完成标志)
            sta |= (CAN_ID->TSR & (1 << 17)) >> 16;  // TXOK2 (传输成功标志)
            sta |= ((CAN_ID->TSR & (1 << 28)) >> 26);  // TME2 (传输邮件标志)
            break;
        default:
            sta = 0X05;  // 邮箱号不对,失败
            break;
    }
    return sta;
} 

/**************************************************************************
【函数名称】CAN_Msg_Pend
【功能概述】得到在FIFO0/FIFO1中接收到的报文个数
            - 根据指定的FIFO编号获取接收到的报文个数
【输入参数】fifox: FIFO编号（0、1）
【返回值】FIFO0/FIFO1中的报文个数
【关键寄存器】RF0R: FIFO0寄存器
         RF1R: FIFO1寄存器
【关键位】FMP0, FMP1: FIFO0和FIFO1中的报文个数
**************************************************************************/
u8 CAN_Msg_Pend(u8 fifox)
{
    if(fifox == 0) return CAN_ID->RF0R & 0x03;  // FIFO0中的报文个数
    else if(fifox == 1) return CAN_ID->RF1R & 0x03;  // FIFO1中的报文个数
    else return 0;  // FIFO编号错误
}

/**************************************************************************
【函数名称】CAN_Rx_Msg
【功能概述】接收数据
            - 根据指定的FIFO编号接收数据
            - 提取并存储接收到的标识符、标识符类型、远程传输请求、数据长度和数据
【输入参数】fifox: 邮箱号
            id: 标准ID(11位)/扩展ID(11位+18位)
            ide: 0,标准帧;1,扩展帧
            rtr: 0,数据帧;1,远程帧
            len: 接收到的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
            dat: 数据缓存区
【返回值】none
【关键寄存器】RF0R: FIFO0寄存器
         RF1R: FIFO1寄存器
         RIR: 标识符寄存器
         RDTR: 数据长度寄存器
         RDLR: 数据低寄存器
         RDHR: 数据高寄存器
**************************************************************************/
void CAN_Rx_Msg(u8 fifox, u32 *id, u8 *ide, u8 *rtr, u8 *len, u8 *dat)
{	   
    // 获取标识符选择位的值
    *ide = CAN_ID->sFIFOMailBox[fifox].RIR & 0x04;  // 得到IDE位的值

    // 提取标识符
    if(*ide == 0)  // 标准标识符
    {
        *id = CAN_ID->sFIFOMailBox[fifox].RIR >> 21;  // 取低11位stdid
    }
    else          // 扩展标识符
    {
        *id = CAN_ID->sFIFOMailBox[fifox].RIR >> 3;  // 取低32位extid
    }

    // 获取远程发送请求值
    *rtr = CAN_ID->sFIFOMailBox[fifox].RIR & 0x02;  // 得到RTR位的值

    // 获取数据长度代码（DLC）
    *len = CAN_ID->sFIFOMailBox[fifox].RDTR & 0x0F;  // 得到DLC

    // 接收数据
    dat[0] = CAN_ID->sFIFOMailBox[fifox].RDLR & 0XFF;
    dat[1] = (CAN_ID->sFIFOMailBox[fifox].RDLR >> 8) & 0XFF;
    dat[2] = (CAN_ID->sFIFOMailBox[fifox].RDLR >> 16) & 0XFF;
    dat[3] = (CAN_ID->sFIFOMailBox[fifox].RDLR >> 24) & 0XFF;
    dat[4] = CAN_ID->sFIFOMailBox[fifox].RDHR & 0XFF;
    dat[5] = (CAN_ID->sFIFOMailBox[fifox].RDHR >> 8) & 0XFF;
    dat[6] = (CAN_ID->sFIFOMailBox[fifox].RDHR >> 16) & 0XFF;
    dat[7] = (CAN_ID->sFIFOMailBox[fifox].RDHR >> 24) & 0XFF;

    // 释放FIFO邮箱
    if(fifox == 0) CAN_ID->RF0R |= 0X20;  // 释放FIFO0邮箱
    else if(fifox == 1) CAN_ID->RF1R |= 0X20;  // 释放FIFO1邮箱
}
/**************************************************************************
【函数名称】CAN2_RX0_IRQHandler
【功能概述】CAN接收中断服务函数，条件编译
            - 处理CAN2接收中断，提取接收到的数据并根据ID进行相应的处理
【输入参数】none
【返回值】none
【关键寄存器】RF0R: FIFO0寄存器
         RIR: 标识符寄存器
         RDTR: 数据长度寄存器
         RDLR: 数据低寄存器
         RDHR: 数据高寄存器
【关键变量】CAN_ON_Flag, PS2_ON_Flag, Remote_ON_Flag, APP_ON_Flag, Usart1_ON_Flag, Usart5_ON_Flag: 控制模式标志
            command_lost_count: 命令丢失计数
            Move_X, Move_Y, Move_Z: 三轴目标速度
【关键函数】CAN_Rx_Msg: 接收数据函数
**************************************************************************/
#if CAN_RX0_INT_ENABLE	// 使能RX0中断
void CAN2_RX0_IRQHandler(void)
{
    u32 id;
    u8 ide, rtr, len;      // 标识符类型, 远程传输请求, 数据长度

    u8 temp_rxbuf[8];  // 接收数据缓冲区

    CAN_Rx_Msg(0, &id, &ide, &rtr, &len, temp_rxbuf);  // 从FIFO0接收数据

    if(id == 0x181)  // 检查ID是否为0x181
    {
        // 设置控制模式标志
        CAN_ON_Flag = 1; PS2_ON_Flag = 0; Remote_ON_Flag = 0; APP_ON_Flag = 0; Usart1_ON_Flag = 0; Usart5_ON_Flag = 0;
        command_lost_count = 0;  // 重置命令丢失计数

        // 计算三轴目标速度，单位: m/s
        Move_X = ((float)((short)((temp_rxbuf[0] << 8) + (temp_rxbuf[1])))) / 1000;
        Move_Y = ((float)((short)((temp_rxbuf[2] << 8) + (temp_rxbuf[3])))) / 1000;
        Move_Z = ((float)((short)((temp_rxbuf[4] << 8) + (temp_rxbuf[5])))) / 1000;
    }
}
#endif

/**************************************************************************
【函数名称】CAN_Send_Msg
【功能概述】CAN发送一组数据（固定格式：ID为0X601,标准帧,数据帧）
            - 根据指定的ID发送一组数据
【输入参数】msg: 数据指针
            len: 数据长度（最大为8）
【返回值】0, 成功；其他, 失败
【关键函数】CAN_Tx_Msg: 发送数据函数
            CAN_Tx_Staus: 获取发送状态函数
**************************************************************************/
u8 CAN_Send_Msg(u8* msg, u8 len)
{	
    u8 mbox;  // 邮箱编号
    u16 i = 0;  // 计数器，用于超时检测

    mbox = CAN_Tx_Msg(0X601, 0, 0, len, msg);  // 发送数据到ID为0X601的邮箱

    // 等待发送结束
    while((CAN_Tx_Staus(mbox) != 0X07) && (i < 0XFFF)) i++;  // 等待发送结束
    if(i >= 0XFFF) return 1;  // 发送失败
    return 0;  // 发送成功
}

/**************************************************************************
【函数名称】CAN_Receive_Msg
【功能概述】CAN口接收数据查询
            - 查询CAN口是否接收到数据，并将接收到的数据存储到缓冲区
【输入参数】buf: 数据缓存区
【返回值】0, 无数据被收到；其他, 接收的数据长度
【关键函数】CAN_Msg_Pend: 检查FIFO中报文个数函数
            CAN_Rx_Msg: 接收数据函数
**************************************************************************/
u8 CAN_Receive_Msg(u8 *buf)
{		   		   	
    u32 id;
    u8 ide, rtr, len;  // 标识符类型, 远程传输请求, 数据长度

    if(CAN_Msg_Pend(0) == 0) return 0;  // 没有接收到数据,直接退出

    CAN_Rx_Msg(0, &id, &ide, &rtr, &len, buf);  // 从FIFO0接收数据

    if(id != 0x12 || ide != 0 || rtr != 0) len = 0;  // 接收错误

    return len;  // 返回接收的数据长度
}

/**************************************************************************
【函数名称】CAN_Send_MsgTEST
【功能概述】CAN发送一组数据测试
            - 根据指定的ID发送一组测试数据
【输入参数】msg: 数据指针
            len: 数据长度（最大为8）
【返回值】0, 成功；1, 失败
【关键函数】CAN_Tx_Msg: 发送数据函数
            CAN_Tx_Staus: 获取发送状态函数
**************************************************************************/
u8 CAN_Send_MsgTEST(u8* msg, u8 len)
{	
    u8 mbox;  // 邮箱编号
    u16 i = 0;  // 计数器，用于超时检测

    mbox = CAN_Tx_Msg(0X701, 0, 0, len, msg);  // 发送数据到ID为0X701的邮箱

    // 等待发送结束
    while((CAN_Tx_Staus(mbox) != 0X07) && (i < 0XFFF)) i++;  // 等待发送结束
    if(i >= 0XFFF) return 1;  // 发送失败
    return 0;  // 发送成功
}

/**************************************************************************
【函数名称】CAN_Send_Num
【功能概述】给给定的ID发送一个数组的命令
            - 根据指定的ID发送一组数据
【输入参数】id: ID号
            msg: 被发送数据指针
【返回值】0, 成功；1, 失败
【关键函数】CAN_Tx_Msg: 发送数据函数
            CAN_Tx_Staus: 获取发送状态函数
**************************************************************************/
u8 CAN_Send_Num(u32 id, u8* msg)
{
    u8 mbox;  // 邮箱编号
    u16 i = 0;  // 计数器，用于超时检测

    mbox = CAN_Tx_Msg(id, 0, 0, 8, msg);  // 发送数据到指定的邮箱

    // 等待发送结束
    while((CAN_Tx_Staus(mbox) != 0X07) && (i < 0XFFF)) i++;  // 等待发送结束
    if(i >= 0XFFF) return 1;  // 发送失败
    return 0;  // 发送成功
}

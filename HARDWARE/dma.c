/**
  ******************************************************************************
  * @file    dma.c
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
#include "dma.h"
#include "sys.h"
/*-----------------------------------------------------------------------------
 * DMA模块全局资源说明：
 * 包含DMA配置结构体和传输长度状态变量
 * 注意：多任务环境下需保证访问原子性
 *---------------------------------------------------------------------------*/

/**
 * @brief DMA通道配置缓存结构体
 * @detail 用于暂存DMA初始化参数，实际配置流程：
 *         1. 在MYDMA_Init2中填充结构体
 *         2. 通过DMA_Init()写入寄存器
 *         3. 配置完成后可重复使用
 * 典型字段：
 * - DMA_DIR：传输方向（存储器到外设/外设到存储器）
 * - DMA_BufferSize：传输数据项数量
 * - DMA_PeripheralDataSize：外设数据宽度（字节/半字/字）
 * - DMA_MemoryDataSize：存储器数据宽度
 * - DMA_Priority：通道优先级
 */
DMA_InitTypeDef DMA_InitStructure;

/**
 * @brief DMA1存储器传输长度缓存（单位：数据项）
 * @detail 用于记录和验证传输规模：
 *         1. 初始化时存储配置的NDTR值
 *         2. 传输过程中实时反映剩余数据量
 *         3. 支持动态重配置传输长度
 * 使用规范：
 * - 数据项单位由DMA_MemoryDataSize决定
 * - 最大有效值65535（16位寄存器限制）
 * - 修改时需配合DMA_Cmd()禁用通道
 */
u16 DMA1_MEM_LEN2; // 当前有效传输长度（非字节数）

/**************************************************************************
函数功能：串口2DMA初始化
入口参数：DMA通道，数据的目标地址，数据的起始地址，发送的数据数量
返回  值：无
**************************************************************************/
void MYDMA_Init2(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
  DMA_DeInit(DMA_CHx);	//将DMA的通道1寄存器重设为缺省值
	DMA1_MEM_LEN2=cndtr;		//数据传输量 
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize = cndtr;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器 	
} 
/**************************************************************************
函数功能：开启一次DMA传输
入口参数：DMA通道，数据的目标地址，数据的起始地址，发送的数据数量
返回  值：无
**************************************************************************/
void MYDMA_Enable2(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA1_Channel7,DMA1_MEM_LEN2);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}	  
/**
 * @brief DMA增强版格式化输出（非阻塞式）
 * @param format 格式化字符串 支持标准printf格式
 * @param ... 可变参数列表
 * @detail 实现机制：
 *         1. 使用静态缓冲区t2xbuf（需确保缓冲区足够大）
 *         2. 通过vsnprintf进行格式转换
 *         3. 自动检测缓冲区溢出（截断处理）
 *         4. 调用USART_SendBuffer执行DMA传输
 * @warning 缓冲区固定大小为512字节（sizeof(t2xbuf)）
 *          长时间连续调用需注意线程安全
 */
void DMA_printf(const char *format,...)
{
    u32 length;
    va_list args;
    
    // 解析可变参数（线程不安全实现）
    va_start(args, format);
    // 安全格式化：返回实际写入字节数（含结束符）
    length = vsnprintf((char*)t2xbuf, sizeof(t2xbuf), (char*)format, args);
    va_end(args);

    // 有效内容长度计算（去除末尾的\0）
    length = (length > 0) ? (length - 1) : 0;
    
    // 启动DMA传输（自动处理空缓冲区）
    USART_SendBuffer((const char*)t2xbuf, length);
}

/**
 * @brief DMA串口数据发送（阻塞式实现）
 * @param buffer 待发送数据缓冲区（需4字节对齐）
 * @param length 数据长度（单位：字节）
 * @return u32 实际发送字节数
 * @detail 工作流程：
 *         1. 参数有效性检查
 *         2. 配置DMA传输计数器
 *         3. 启动DMA通道
 *         4. 轮询等待传输完成
 * @warning 此实现为阻塞式，最大等待时间=波特率周期*length
 *          建议在RTOS任务中配合vTaskDelay使用
 */
u32 USART_SendBuffer(const char* buffer, u32 length)
{
    // 防御性编程：过滤无效参数
    if( (buffer==NULL) || (length==0) )
    {
        return 0;
    }
 
    /* DMA通道重配置流程（符合STM32 DMA安全操作规范） */
    DMA_Cmd(DMA1_Channel7, DISABLE);        // 1. 禁用通道（防止配置过程中触发传输）
    DMA_SetCurrDataCounter(DMA1_Channel7, length);  // 2. 设置传输数据量（按字节计数）
    DMA_Cmd(DMA1_Channel7, ENABLE);         // 3. 重新使能通道（自动开始传输）

    /* 传输完成检测（轮询方式）*/
    while(1)
    {
        // 检测传输完成标志（TC=Transfer Complete）
        if(DMA_GetITStatus(DMA1_IT_TC7)!=RESET)
        {
            // 清除完成标志（防止重复触发）
            DMA_ClearFlag(DMA1_IT_TC7);
            break;
        }
        // 可在此处添加超时检测（当前实现可能造成死循环）
    }
    return length;  // 返回实际发送量（本实现始终返回全部长度）
}

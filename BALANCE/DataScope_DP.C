 /**
  ******************************************************************************
  * @file    DataScope_DP.c
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
#include "DataScope_DP.h"

unsigned char DataScope_OutPut_Buffer[42] = {0};	   //串口发送缓冲区
/* 数据可视化协议处理模块 - 实现DataScope V1.0协议帧生成 */

/**
 * @brief 浮点数转字节序列（支持IEEE754单精度）
 * @param target 待转换浮点数指针
 * @param buf 目标缓冲区（需保证至少4字节空间）
 * @param beg 写入起始位置（基于0的索引）
 * @note 采用小端模式(Little-Endian)存储，适用于大多数ARM平台
 *        内存布局：buf[beg]   = 字节0（最低有效字节）
 *                 buf[beg+3] = 字节3（最高有效字节）
 */
void Float2Byte(float *target, unsigned char *buf, unsigned char beg)
{
    /* 通过类型转换直接访问浮点数的内存表示 */
    unsigned char *point = (unsigned char*)target;  // 获取浮点数二进制地址
    
    /* 按小端顺序写入缓冲区（适用于STM32系列MCU） */
    buf[beg]   = point[0];  // 写入最低有效字节（LSB）
    buf[beg+1] = point[1];  // 次低有效字节
    buf[beg+2] = point[2];  // 次高有效字节
    buf[beg+3] = point[3];  // 写入最高有效字节（MSB）
}

/**
 * @brief 多通道数据打包函数（支持10个独立通道）
 * @param Data 通道数据值（单精度浮点）
 * @param Channel 通道选择（1-10对应协议通道号）
 * @note 数据缓冲区结构：
 *       字节0: 帧头0xAA
 *       字节1-4: 通道1数据（小端格式）
 *       字节5-8: 通道2数据
 *       ... 
 *       字节37-40: 通道10数据
 *       字节41: 校验和（未实现）
 */
void DataScope_Get_Channel_Data(float Data, unsigned char Channel)
{
    /* 通道号有效性检查（防御式编程） */
    if (Channel == 0 || Channel > 10) 
        return;  // 非法通道号直接返回，防止缓冲区溢出
    
    /* 根据协议规范映射通道到缓冲区位置 */
    switch (Channel) {
        case 1:  // 通道1: 缓冲区[1-4]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 1); 
            break;
        case 2:  // 通道2: 缓冲区[5-8]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 5);
            break;
        case 3:  // 通道3: 缓冲区[9-12]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 9);
            break;
        case 4:  // 通道4: 缓冲区[13-16]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 13);
            break;
        case 5:  // 通道5: 缓冲区[17-20]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 17);
            break;
        case 6:  // 通道6: 缓冲区[21-24]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 21);
            break;
        case 7:  // 通道7: 缓冲区[25-28]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 25);
            break;
        case 8:  // 通道8: 缓冲区[29-32]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 29);
            break;
        case 9:  // 通道9: 缓冲区[33-36]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 33);
            break;
        case 10: // 通道10: 缓冲区[37-40]
            Float2Byte(&Data, DataScope_OutPut_Buffer, 37);
            break;
    }
}

/**
 * @brief 生成DataScope协议帧（当前实现存在逻辑问题，需检查）
 * @param Channel_Number 实际使用通道数（1-10）
 * @return 帧数据长度（字节数），0表示生成失败
 * @warning 当前实现存在协议格式错误，缓冲区填充逻辑需重新验证
 * @note 标准DataScopeV1.0协议结构：
 *        [帧头][通道数][数据段][校验和]
 *        当前实现未严格遵循协议规范，以下为问题说明
 */
unsigned char DataScope_Data_Generate(unsigned char Channel_Number)
{
    /* 通道数有效性检查（防御性编程） */
    if (Channel_Number == 0 || Channel_Number > 10) 
        return 0;  // 通道号非法直接返回，防止后续操作越界
    
    /* 设置协议帧头（标准协议应使用0xAA而非'$'） */
    DataScope_OutPut_Buffer[0] = '$';  // 帧头字符（ASCII码0x24）
    
    /* 问题实现说明：当前代码试图通过索引值设置数据长度，但存在以下问题：
     * 1. 未正确设置通道数量字段（标准协议第2字节应为通道数）
     * 2. 数据段填充位置计算错误（case操作不符合协议规范）
     * 3. 校验和字段缺失（标准协议末字节应为校验和）
     */
    switch(Channel_Number) {
        case 1:   
            DataScope_OutPut_Buffer[5] = 5;  // 错误实现：将索引5设为值5
            return 6;   // 返回帧长度（实际应为：1+1+4+1=7字节）
        case 2:   
            DataScope_OutPut_Buffer[9] = 9;  // 疑似试图标记数据段结束位置
            return 10;   // 返回长度与标准协议不匹配
        // ... 其他case存在相同问题（以下为典型错误模式注释）
        case 10:  
            DataScope_OutPut_Buffer[41] = 41; // 错误填充末字节（应为校验和）
            return 42;   // 返回整个缓冲区长度
    }
    return 0;  // 冗余返回（逻辑已由switch覆盖）
}

/* 建议的标准协议实现伪代码：
void DataScope_Data_Generate_Correct(uchar ch_num) {
    buffer[0] = 0xAA;                    // 标准帧头
    buffer[1] = ch_num;                   // 通道数量
    uchar checksum = 0xAA + ch_num;      
    for(int i=0; i<ch_num*4; i++) {      // 计算校验和（示例算法）
        checksum += buffer[2+i];         
    }
    buffer[2 + ch_num*4] = checksum;     // 末字节校验和
    return 3 + ch_num*4;                 // 总长度=1+1+4*N+1
}
*/










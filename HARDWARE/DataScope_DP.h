/**
  ******************************************************************************
  * @file    DataScope_DP.h
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
#ifndef __DATA_PRTOCOL_H
#define __DATA_PRTOCOL_H
#include "system.h"
 
// DataScope_OutPut_Buffer 是一个外部声明的42字节字符数组，用于存储输出的数据帧内容
extern unsigned char DataScope_OutPut_Buffer[42];	  

// DataScope_Get_Channel_Data 函数用于获取指定通道的数据
// 参数 Data 是要写入通道的浮点数数据
// 参数 Channel 是通道编号，用于指定要写入数据的通道
void DataScope_Get_Channel_Data(float Data, unsigned char Channel);   
// DataScope_Data_Generate 函数用于生成指定通道的数据
// 参数 Channel_Number 是通道编号，用于指定要生成数据的通道
// 返回值是一个字节，表示生成数据的状态或结果
unsigned char DataScope_Data_Generate(unsigned char Channel_Number);  

 
#endif 




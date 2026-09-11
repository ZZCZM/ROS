/**
  ******************************************************************************
  * @file   oled.h
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
#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "system.h"
/****** OLED引脚控制宏（4线SPI接口） ******/
/* 复位引脚控制（PD12） */
#define OLED_RST_Clr() PDout(12)=0   // RST拉低（复位有效）
#define OLED_RST_Set() PDout(12)=1   // RST置高（正常工作）

/* 数据/命令选择（PD11） */
#define OLED_RS_Clr()  PDout(11)=0   // DC=0：传输命令
#define OLED_RS_Set()  PDout(11)=1   // DC=1：传输数据

/* 时钟线（PD14） */
#define OLED_SCLK_Clr() PDout(14)=0  // SCL下降沿
#define OLED_SCLK_Set() PDout(14)=1  // SCL上升沿

/* 数据线（PD13） */
#define OLED_SDIN_Clr() PDout(13)=0  // SDA写0
#define OLED_SDIN_Set() PDout(13)=1  // SDA写1

/* 传输模式选择 */
#define OLED_CMD  0	// 命令模式（写寄存器）
#define OLED_DATA 1	// 数据模式（写显存）

/****** OLED基本控制函数 ******/
void OLED_WR_Byte(u8 dat,u8 cmd);  // 单字节传输（dat：数据 cmd：模式选择）
void OLED_Display_On(void);       // 开启显示（退出睡眠模式）
void OLED_Display_Off(void);      // 关闭显示（进入睡眠模式）
void OLED_Refresh_Gram(void);     // 刷新显存到屏幕（全屏更新）
void OLED_Init(void);             // OLED初始化（SPI配置+寄存器初始化）
void OLED_Clear(void);            // 清屏（填充全0）

/****** 图形绘制基础函数 ******/
void OLED_DrawPoint(u8 x,u8 y,u8 t); // 画点（x:0-127 y:0-63 t:0擦除/1绘制）
void OLED_Set_Pos(u8 x, u8 y);      // 设置光标位置（x:列 y:页地址，每页8行）

/****** 文本显示功能 ******/
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode); // 显示ASCII字符
/* 参数说明：
   - x,y: 起始坐标（像素级）
   - chr: 字符（ASCII码）
   - size: 字号（12/16/24）
   - mode: 0-正常显示 1-反色显示 */

void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size); // 显示数字
/* 参数说明：
   - num: 显示数值（最大4294967295）
   - len: 显示位数（不足补零）
   - size: 字号 */

void OLED_ShowString(u8 x,u8 y,const u8 *p); // 显示字符串（ASCII）

/****** 中文显示配置（需要外部字库） ******/
#define CNSizeWidth  16    // 汉字宽度（像素）
#define CNSizeHeight 16    // 汉字高度（像素）
//extern char Hzk16[][16];  // 16x16点阵字库（需外部定义）
void OLED_ShowCHinese(u8 x,u8 y,u8 no,u8 font_width,u8 font_height); 
/* 参数说明：
   - no: 汉字在字库中的索引
   - font_width/height: 字体尺寸（需与字库匹配） */

#endif  
	 

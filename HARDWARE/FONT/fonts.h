/**
? ******************************************************************************
? * @file ? fonts.h
? * @author ?The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
? * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb ? ? NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
? * @version V1.0.0
? * @date ? ?8-June-2025
? * @brief ? Header File
? ******************************************************************************
? * @attention
? * Copyright (c) 2025 The Daily Life of An Engineering Girl.
? * All rights reserved.
? *
? * *----------------------------------------------------------------------------*/
#ifndef __FONT_H
#define __FONT_H       

#include "stm32f4xx.h"


/* 字体管理模块头文件 - 定义字库数据结构及尺寸规范 */

/**
 * @brief 字体结构体（描述字符显示属性）
 * @param table   字模数据指针（指向字库数组首地址）
 *                - 数据格式：每个位表示一个像素点（1-点亮，0-熄灭）
 *                - 存储方式：纵向取模，字节倒序（具体依赖显示驱动实现）
 * @param Width  字符宽度（像素数，决定水平占位空间）
 * @param Height 字符高度（像素数，决定垂直占位空间）
 * @note 典型应用场景：
 * - 英文字符：等宽字体（如8x16）
 * - 中文字符：方形字体（如32x32）
 */
typedef struct _tFont
{    
  const uint8_t *table;  // 字模数据存储地址（需4字节对齐）
  uint16_t Width;        // 字符物理宽度（单位：像素）
  uint16_t Height;       // 字符物理高度（单位：像素）
} sFONT;

/*---------------------- 预定义字体规格声明 ----------------------*/
/**
 * @brief 24x48像素字体（超大标题字体）
 * @note 适用场景：
 * - LCD屏状态栏大号标题
 * - 关键参数突出显示
 */
extern sFONT Font24x48;

/**
 * @brief 16x32像素字体（中等强调字体）
 * @note 适用场景：
 * - 二级菜单标题
 * - 重要状态信息显示
 */
extern sFONT Font16x32;

/**
 * @brief 8x16像素字体（基础正文字体）
 * @note 适用场景：
 * - 常规文本显示
 * - 多行密集信息排版
 * - 系统日志输出
 */
extern sFONT Font8x16;

/*--------------------- 中文字符显示规范 ---------------------*/
/** 
 * @def 中文字符显示尺寸（方形字库）
 * @note 当前系统采用32x32像素中文字库
 * 扩展说明：如需支持其他尺寸（如24x24），需：
 * 1. 添加对应字库数组
 * 2. 修改DispChar_CH函数的取模逻辑
 */
#define WIDTH_CH_CHAR    32  // 中文字符宽度（像素） 
#define HEIGHT_CH_CHAR   32  // 中文字符高度（像素）

/* 如果另外制作使用比如24*24字模 需在 ILI9806G_DispChar_CH 函数内需改取字节数据过程为对应字节个数：

    //取出3个字节的数据，在lcd上即是一个汉字的一行
		usTemp = ucBuffer [ rowCount * 4 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ rowCount * 4 + 1 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ rowCount * 4 + 2 ];

*/

#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))

//LINEY统一使用汉字字模的高度
#define LINEY(x) ((x) * (WIDTH_CH_CHAR))




//0表示使用SD卡字模，非零表示FLASH字模,由于SD卡字模有文件系统，速度慢很多。

#define GBKCODE_FLASH 						1

#if GBKCODE_FLASH
	/*使用FLASH字模*/
	/*中文字库存储在FLASH的起始地址*/
	/*FLASH*/
	#define GBKCODE_START_ADDRESS   1254*4096


	/*获取字库的函数*/
	//定义获取中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符（国标码）
	#define      GetGBKCode( ucBuffer, usChar )  GetGBKCode_from_EXFlash( ucBuffer, usChar )  
	int GetGBKCode_from_EXFlash( uint8_t * pBuffer, uint16_t c);

#else
	/*使用SD字模*/


	/*SD卡字模路径*/
	#define GBKCODE_FILE_NAME			"0:/Font/GB2312_H3232.FON"


	/*获取字库的函数*/
	//定义获取中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符（国标码）

	#define GetGBKCode( ucBuffer, usChar )  GetGBKCode_from_sd( ucBuffer, usChar )
	int GetGBKCode_from_sd ( uint8_t * pBuffer, uint16_t c);

#endif



#endif /*end of __FONT_H    */

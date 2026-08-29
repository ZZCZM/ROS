/**
? ******************************************************************************
? * @file ? bsp_ili9806g_lcd.h
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
#ifndef      __BSP_ILI9806G_LCD_H
#define	     __BSP_ILI9806G_LCD_H


#include "stm32f4xx.h"
#include "fonts.h"


/***************************************************************************************
2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

选择BANK1-BORSRAM3 连接 TFT，地址范围为0X6800 0000 ~ 0X6BFF FFFF
FSMC_A0 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X6C00 0000
RAM基地址 = 0X6D00 0000 = 0X6C00 0000+2^0*2 = 0X6800 0000 + 0X2 = 0X6800 0002
当选择不同的地址线时，地址要重新计算  
****************************************************************************************/

/******************************* ILI9806G 显示屏的 FSMC 参数定义 ***************************/
//FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define      FSMC_Addr_ILI9806G_CMD         ( ( uint32_t ) 0x68000000 )

//FSMC_Bank1_NORSRAM用于LCD数据操作的地址      
#define      FSMC_Addr_ILI9806G_DATA        ( ( uint32_t ) 0x68000002 )

//由片选引脚决定的NOR/SRAM块
#define      FSMC_Bank1_NORSRAMx           FSMC_Bank1_NORSRAM3

/* ILI9806G LCD 8080并行接口硬件配置 */

/*--------------------- FSMC复用功能配置 ---------------------*/
#define FSMC_AF GPIO_AF_FSMC  // GPIO复用为FSMC功能

/*--------------------- 片选信号(CS)配置 ---------------------*/
#define ILI9806G_CS_CLK  RCC_AHB1Periph_GPIOG  // 端口时钟使能
#define ILI9806G_CS_PORT GPIOG                 // 端口G
#define ILI9806G_CS_PIN  GPIO_Pin_10           // PG10引脚
#define ILI9806G_CS_PinSource GPIO_PinSource10 // 引脚源配置
/* 功能说明：选择FSMC的BANK1区域（块3） */

/*--------------------- 数据/命令选择(DC)配置 ---------------------*/
#define ILI9806G_DC_CLK  RCC_AHB1Periph_GPIOF  // 端口时钟使能 
#define ILI9806G_DC_PORT GPIOF                 // 端口F
#define ILI9806G_DC_PIN  GPIO_Pin_0            // PF0引脚（映射为FSMC_A0地址线）
#define ILI9806G_DC_PinSource GPIO_PinSource0
/* 操作原理：
   - 写命令地址：0x6C000000 (A0=0)
   - 写数据地址：0x6C000800 (A0=1)
   通过地址总线A0电平切换命令/数据模式 */

/*--------------------- 写使能(WR)配置 ---------------------*/
#define ILI9806G_WR_CLK  RCC_AHB1Periph_GPIOD   // 端口时钟使能
#define ILI9806G_WR_PORT GPIOD                  // 端口D  
#define ILI9806G_WR_PIN  GPIO_Pin_5            // PD5引脚
#define ILI9806G_WR_PinSource GPIO_PinSource5
/* 功能说明：FSMC自动控制写使能信号时序 */

/*--------------------- 读使能(RD)配置 ---------------------*/ 
#define ILI9806G_RD_CLK  RCC_AHB1Periph_GPIOD
#define ILI9806G_RD_PORT GPIOD
#define ILI9806G_RD_PIN  GPIO_Pin_4            // PD4引脚
#define ILI9806G_RD_PinSource GPIO_PinSource4
/* 注意：读操作需配置FSMC为异步模式 */

/*--------------------- 复位信号(RST)配置 ---------------------*/
#define ILI9806G_RST_CLK RCC_AHB1Periph_GPIOF
#define ILI9806G_RST_PORT GPIOF
#define ILI9806G_RST_PIN GPIO_Pin_11           // PF11引脚
/* 复位时序要求：
   - 低电平保持至少10μs
   - 释放后延迟5ms再初始化 */

/*--------------------- 背光控制(BK)配置 ---------------------*/
#define ILI9806G_BK_CLK  RCC_AHB1Periph_GPIOF
#define ILI9806G_BK_PORT GPIOF
#define ILI9806G_BK_PIN  GPIO_Pin_9           // PF9引脚
/* 推荐控制方式：
   - PWM调光（频率1-10KHz）
   - 默认高电平点亮 */
/*--------------------- 16位数据线配置（FSMC D0-D15） ---------------------*/

/* 数据线D0配置（FSMC_D0） */
#define ILI9806G_D0_CLK    RCC_AHB1Periph_GPIOD   // 端口D时钟使能
#define ILI9806G_D0_PORT   GPIOD                  // 端口D
#define ILI9806G_D0_PIN    GPIO_Pin_14            // PD14引脚
#define ILI9806G_D0_PinSource GPIO_PinSource14     // 复用功能引脚源配置

/* 数据线D1配置（FSMC_D1） */
#define ILI9806G_D1_CLK    RCC_AHB1Periph_GPIOD
#define ILI9806G_D1_PORT   GPIOD
#define ILI9806G_D1_PIN    GPIO_Pin_15            // PD15引脚
#define ILI9806G_D1_PinSource GPIO_PinSource15

/* 数据线D2配置（FSMC_D2） */
#define ILI9806G_D2_CLK    RCC_AHB1Periph_GPIOD
#define ILI9806G_D2_PORT   GPIOD
#define ILI9806G_D2_PIN    GPIO_Pin_0             // PD0引脚（复用前需关闭JTAG功能）
#define ILI9806G_D2_PinSource GPIO_PinSource0

/* 数据线D3配置（FSMC_D3） */
#define ILI9806G_D3_CLK    RCC_AHB1Periph_GPIOD
#define ILI9806G_D3_PORT   GPIOD
#define ILI9806G_D3_PIN    GPIO_Pin_1             // PD1引脚
#define ILI9806G_D3_PinSource GPIO_PinSource1

/* 数据线D4配置（FSMC_D4） */
#define ILI9806G_D4_CLK    RCC_AHB1Periph_GPIOE   // 端口E时钟使能
#define ILI9806G_D4_PORT   GPIOE
#define ILI9806G_D4_PIN    GPIO_Pin_7             // PE7引脚
#define ILI9806G_D4_PinSource GPIO_PinSource7

/* 数据线D5配置（FSMC_D5） */
#define ILI9806G_D5_CLK    RCC_AHB1Periph_GPIOE
#define ILI9806G_D5_PORT   GPIOE
#define ILI9806G_D5_PIN    GPIO_Pin_8             // PE8引脚
#define ILI9806G_D5_PinSource GPIO_PinSource8

/* 数据线D6配置（FSMC_D6） */
#define ILI9806G_D6_CLK    RCC_AHB1Periph_GPIOE
#define ILI9806G_D6_PORT   GPIOE
#define ILI9806G_D6_PIN    GPIO_Pin_9             // PE9引脚
#define ILI9806G_D6_PinSource GPIO_PinSource9

/* 数据线D7配置（FSMC_D7） */
#define ILI9806G_D7_CLK    RCC_AHB1Periph_GPIOE
#define ILI9806G_D7_PORT   GPIOE
#define ILI9806G_D7_PIN    GPIO_Pin_10            // PE10引脚
#define ILI9806G_D7_PinSource GPIO_PinSource10

/* 数据线D8配置（FSMC_D8） */
#define ILI9806G_D8_CLK    RCC_AHB1Periph_GPIOE
#define ILI9806G_D8_PORT   GPIOE
#define ILI9806G_D8_PIN    GPIO_Pin_11            // PE11引脚
#define ILI9806G_D8_PinSource GPIO_PinSource11

/* 数据线D9配置（FSMC_D9） */
#define ILI9806G_D9_CLK    RCC_AHB1Periph_GPIOE
#define ILI9806G_D9_PORT   GPIOE
#define ILI9806G_D9_PIN    GPIO_Pin_12            // PE12引脚
#define ILI9806G_D9_PinSource GPIO_PinSource12

/* 数据线D10配置（FSMC_D10） */
#define ILI9806G_D10_CLK   RCC_AHB1Periph_GPIOE
#define ILI9806G_D10_PORT  GPIOE
#define ILI9806G_D10_PIN   GPIO_Pin_13            // PE13引脚
#define ILI9806G_D10_PinSource GPIO_PinSource13

/* 数据线D11配置（FSMC_D11） */
#define ILI9806G_D11_CLK   RCC_AHB1Periph_GPIOE
#define ILI9806G_D11_PORT  GPIOE
#define ILI9806G_D11_PIN   GPIO_Pin_14            // PE14引脚
#define ILI9806G_D11_PinSource GPIO_PinSource14

/* 数据线D12配置（FSMC_D12） */
#define ILI9806G_D12_CLK   RCC_AHB1Periph_GPIOE
#define ILI9806G_D12_PORT  GPIOE
#define ILI9806G_D12_PIN   GPIO_Pin_15            // PE15引脚
#define ILI9806G_D12_PinSource GPIO_PinSource15

/* 数据线D13配置（FSMC_D13） */
#define ILI9806G_D13_CLK   RCC_AHB1Periph_GPIOD
#define ILI9806G_D13_PORT  GPIOD
#define ILI9806G_D13_PIN   GPIO_Pin_8             // PD8引脚
#define ILI9806G_D13_PinSource GPIO_PinSource8

/* 数据线D14配置（FSMC_D14） */
#define ILI9806G_D14_CLK   RCC_AHB1Periph_GPIOD
#define ILI9806G_D14_PORT  GPIOD
#define ILI9806G_D14_PIN   GPIO_Pin_9             // PD9引脚
#define ILI9806G_D14_PinSource GPIO_PinSource9

/* 数据线D15配置（FSMC_D15） */
#define ILI9806G_D15_CLK   RCC_AHB1Periph_GPIOD
#define ILI9806G_D15_PORT  GPIOD
#define ILI9806G_D15_PIN   GPIO_Pin_10            // PD10引脚
#define ILI9806G_D15_PinSource GPIO_PinSource10

/* 硬件设计注意事项：
   1. 数据线分组：
      - D0-D1  : GPIOD
      - D2-D3  : GPIOD（与JTAG引脚复用，需关闭调试接口）
      - D4-D12 : GPIOE
      - D13-D15: GPIOD
   2. 信号完整性：
      - 数据线等长处理（长度差<150mil）
      - 串联33Ω电阻匹配阻抗
   3. 电源滤波：
      - 每个端口VDD接0.1μF去耦电容
      - 避免数据线与高频信号（如PWM）平行走线
   4. FSMC时序：
      - 数据建立时间：15ns（对应72MHz时钟）
      - 保持时间：5ns
*/


/*************************************** 调试预用 ******************************************/
#define      DEBUG_DELAY()               Delay(0x5000)

/***************************** ILI934 显示区域的起始坐标和总行列数 ***************************/
#define      ILI9806G_DispWindow_X_Star		    0     //起始点的X坐标
#define      ILI9806G_DispWindow_Y_Star		    0     //起始点的Y坐标

#define 			ILI9806G_LESS_PIXEL	  		480			//液晶屏较短方向的像素宽度
#define 			ILI9806G_MORE_PIXEL	 		800			//液晶屏较长方向的像素宽度

//根据液晶扫描方向而变化的XY像素宽度
//调用ILI9806G_GramScan函数设置方向时会自动更改
extern uint16_t LCD_X_LENGTH,LCD_Y_LENGTH; 

//液晶屏扫描模式
//参数可选值为0-7
extern uint8_t LCD_SCAN_MODE;

/*--------------------- ILI9806G LCD 颜色定义（RGB565格式） ---------------------*/

/* 背景默认颜色 */
#define BACKGROUND       BLACK   // 默认背景色（黑色）

/* 基础色系 */
#define WHITE            0xFFFF  // 白色（R=31, G=63, B=31）
#define BLACK            0x0000  // 纯黑（R=0, G=0, B=0）
#define GREY             0xF7DE  // 灰色（R=30, G=61, B=30），接近#D3D3D3
#define BLUE             0x001F  // 标准蓝色（R=0, G=0, B=31）
#define BLUE2            0x051F  // 浅蓝色（R=0, G=16, B=31）
#define RED              0xF800  // 纯红色（R=31, G=0, B=0）
#define MAGENTA          0xF81F  // 品红色/洋红（R=31, G=0, B=31）
#define GREEN            0x07E0  // 标准绿色（R=0, G=63, B=0）
#define CYAN             0x7FFF  // 青色（R=15, G=63, B=31）实际值偏蓝绿色
#define YELLOW           0xFFE0  // 纯黄色（R=31, G=63, B=0）

/* 混合色系（需核对实际显示效果） */
#define BRED             0xF81F  // 蓝调红色（实际与MAGENTA相同，可能定义重复）
#define GRED             0xFFE0  // 黄绿色（R=31, G=63, B=0，实际与YELLOW相同）
#define GBLUE            0x07FF  // 青蓝色（R=0, G=63, B=31）

/* 颜色使用场景建议：
   - WHITE/BLACK : 文本与背景色
   - RED         : 错误提示、警告标志
   - GREEN       : 成功状态、通行指示
   - BLUE        : 超链接、高亮信息
   - YELLOW      : 注意警告、未完成状态
   - GREY        : 禁用状态、次级信息
   - CYAN        : 水位/温度等冷色系标识
   - MAGENTA     : 特殊提示、图表区分
*/




/******************************* 定义 ILI934 常用命令 ********************************/
#define      CMD_SetCoordinateX		 		    0x2A	     //设置X坐标
#define      CMD_SetCoordinateY		 		    0x2B	     //设置Y坐标
#define      CMD_SetPixel		 		          0x2C	     //填充像素



/*--------------------- ILI9806G LCD 高级功能接口声明 ---------------------*/

/**
 * @brief 初始化LCD控制器及外围电路
 * @note 包含步骤：
 * 1. GPIO时钟使能
 * 2. FSMC接口配置
 * 3. 发送初始化指令序列
 * 4. 设置默认扫描方向
 */
void ILI9806G_Init(void);

/**
 * @brief 硬件复位LCD控制器
 * @note 复位时序：
 * 1. 拉低RST引脚至少10μs
 * 2. 释放后延迟120ms等待稳定
 */
void ILI9806G_Rst(void);

/**
 * @brief 背光控制
 * @param enumState 功能状态（ENABLE-点亮/DISABLE-关闭）
 * @note 实际通过PWM调光实现亮度分级控制
 */
void ILI9806G_BackLed_Control(FunctionalState enumState);

/**
 * @brief 设置显存扫描方向
 * @param ucOtion 方向选项（0-7对应8种旋转模式）
 * @note 影响坐标系定义及显示方向
 */
void ILI9806G_GramScan(uint8_t ucOtion);

/**
 * @brief 开窗设置（限定操作区域）
 * @param usX 起始X坐标
 * @param usY 起始Y坐标
 * @param usWidth 区域宽度
 * @param usHeight 区域高度
 * @warning 后续操作仅影响该窗口区域
 */
void ILI9806G_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);

/**
 * @brief 清屏/清区域操作
 * @param usX 起始X坐标
 * @param usY 起始Y坐标
 * @param usWidth 区域宽度
 * @param usHeight 区域高度
 * @note 实际使用背景色填充指定区域
 */
void ILI9806G_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);

/**
 * @brief 绘制单个像素点
 * @param usX X坐标（0-799）
 * @param usY Y坐标（0-479）
 * @note 使用当前前景色绘制
 */
void ILI9806G_SetPointPixel(uint16_t usX, uint16_t usY);

/**
 * @brief 读取指定坐标像素颜色
 * @return RGB565格式颜色值
 * @warning 需提前执行连续读GRAM指令
 */
uint16_t ILI9806G_GetPointPixel(uint16_t usX, uint16_t usY);

/**
 * @brief 绘制直线（Bresenham算法）
 * @param usX1 起点X坐标
 * @param usY1 起点Y坐标
 * @param usX2 终点X坐标
 * @param usY2 终点Y坐标
 * @note 线宽固定为1像素
 */
void ILI9806G_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2);

/**
 * @brief 绘制矩形（可填充）
 * @param usX_Start 左上角X坐标
 * @param usY_Start 左上角Y坐标
 * @param usWidth 矩形宽度
 * @param usHeight 矩形高度
 * @param ucFilled 填充标志（1-填充，0-空心）
 */
void ILI9806G_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled);

/**
 * @brief 绘制圆形（可填充）
 * @param usX_Center 圆心X坐标
 * @param usY_Center 圆心Y坐标
 * @param usRadius 圆半径
 * @param ucFilled 填充标志（1-填充，0-空心）
 * @note 使用中点圆算法实现
 */
void ILI9806G_DrawCircle(uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled);

/*--------------------- 英文字符显示接口 ---------------------*/
/**
 * @brief 显示单个ASCII字符
 * @param usX 起始X坐标
 * @param usY 起始Y坐标
 * @param cChar 要显示的字符（ASCII码）
 * @note 使用当前字体配置（默认8x16）
 */
void ILI9806G_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar);

/**
 * @brief 在指定行显示英文字符串
 * @param line 行号（0-29对应480像素高度）
 * @param pStr 字符串指针（ASCII）
 * @note 自动换行处理，行高取决于当前字体
 */
void ILI9806G_DispStringLine_EN(uint16_t line, char *pStr);

/**
 * @brief 在指定坐标显示英文字符串
 * @param usX 起始X坐标
 * @param usY 起始Y坐标
 * @param pStr 字符串指针（ASCII）
 * @note 支持自动换行（根据屏幕右边界）
 */
void ILI9806G_DispString_EN(uint16_t usX, uint16_t usY, char *pStr);

/**
 * @brief 垂直方向显示英文字符串
 * @param usX 起始X坐标
 * @param usY 起始Y坐标
 * @param pStr 字符串指针（ASCII）
 * @note 每个字符向下排列，用于特殊排版
 */
void ILI9806G_DispString_EN_YDir(uint16_t usX, uint16_t usY, char *pStr);

/*--------------------- 中文字符显示接口 ---------------------*/
/**
 * @brief 显示单个中文字符（GB2312编码）
 * @param usX 起始X坐标
 * @param usY 起始Y坐标
 * @param usChar GB2312编码字符
 * @note 字库需预先加载到SPI Flash
 */
void ILI9806G_DispChar_CH(uint16_t usX, uint16_t usY, uint16_t usChar);

/**
 * @brief 显示中文字符串
 * @param usX 起始X坐标
 * @param usY 起始Y坐标
 * @param pStr GB2312编码字符串
 * @warning 字符串需以双字节形式存储
 */
void ILI9806G_DispString_CH(uint16_t usX, uint16_t usY, char *pStr);

/*--------------------- 混合字符显示接口 ---------------------*/
/**
 * @brief 显示中英文混合字符串
 * @param usX 起始X坐标
 * @param usY 起始Y坐标
 * @param pStr 混合字符串（自动识别编码）
 * @note 内部自动切换ASCII/GB2312解析
 */
void ILI9806G_DispString_EN_CH(uint16_t usX, uint16_t usY, char *pStr);

/**
 * @brief 指定行显示混合字符串
 * @param line 行号（0-29）
 * @param pStr 混合字符串指针
 * @note 支持自动换行和行高适配
 */
void ILI9806G_DispStringLine_EN_CH(uint16_t line, char *pStr);

/**
 * @brief 垂直方向显示混合字符串
 * @param usX 起始X坐标
 * @param usY 起始Y坐标
 * @param pStr 混合字符串指针
 * @note 适用于竖排显示场景
 */
void ILI9806G_DispString_EN_CH_YDir(uint16_t usX, uint16_t usY, char *pStr);
/*--------------------- 显示属性管理接口 ---------------------*/

/**
 * @brief 设置当前显示字体
 * @param fonts 字体结构体指针（需包含字模表及尺寸信息）
 * @note 支持动态切换不同尺寸字体（8x16/16x32等）
 * 示例：LCD_SetFont(&Font16x32);
 */
void LCD_SetFont(sFONT *fonts);

/**
 * @brief 获取当前使用字体配置
 * @return 字体结构体指针
 */
sFONT* LCD_GetFont(void);

/**
 * @brief 清除指定文本行内容
 * @param Line 行号（0起始，根据字体高度计算总行数）
 * @note 实现原理：用背景色填充整行区域
 */
void ILI9806G_ClearLine(uint16_t Line);

/**
 * @brief 设置文本背景色
 * @param Color RGB565颜色值
 * @note 影响后续所有文本输出背景
 */
void LCD_SetBackColor(uint16_t Color);

/**
 * @brief 设置文本前景色
 * @param Color RGB565颜色值
 * @note 影响后续所有文本输出颜色
 */
void LCD_SetTextColor(uint16_t Color);

/**
 * @brief 同时设置前景和背景色
 * @param TextColor 文本颜色
 * @param BackColor 背景颜色
 * @note 等同于依次调用SetTextColor和SetBackColor
 */
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor);

/**
 * @brief 获取当前颜色配置
 * @param TextColor 输出参数，存储当前文本颜色
 * @param BackColor 输出参数，存储当前背景颜色
 */
void LCD_GetColors(uint16_t* TextColor, uint16_t* BackColor);

/* 接口别名宏（提升代码可读性） */
#define LCD_ClearLine ILI9806G_ClearLine // 清除行内容的简化调用方式

void ILI9806G_DisplayStringEx(uint16_t x, 		//字符显示位置x
																 uint16_t y, 				//字符显示位置y
																 uint16_t Font_width,	//要显示的字体宽度，英文字符在此基础上/2。注意为偶数
																 uint16_t Font_Height,	//要显示的字体高度，注意为偶数
																 uint8_t *ptr,					//显示的字符内容
																 uint16_t DrawModel);  //是否反色显示

void ILI9806G_DisplayStringEx_YDir(uint16_t x, 		//字符显示位置x
																			 uint16_t y, 				//字符显示位置y
																			 uint16_t Font_width,	//要显示的字体宽度，英文字符在此基础上/2。注意为偶数
																			 uint16_t Font_Height,	//要显示的字体高度，注意为偶数
																			 uint8_t *ptr,					//显示的字符内容
																			 uint16_t DrawModel);  //是否反色显示
/**
 * @brief LCD显示功能自检程序
 * @note 该函数执行一系列预定义测试模式，用于验证显示屏硬件及驱动的完整性
 * 包含测试项：
 * 1. 全屏基础颜色填充测试（红/绿/蓝/白/黑）
 * 2. 渐变色过渡测试（彩虹渐变）
 * 3. 几何图形绘制测试（同心圆、网格线、对角线）
 * 4. 中英文字符集渲染测试（不同字体尺寸）
 * 5. 像素点逐行扫描测试（检测坏点）
 * 6. 触摸校准测试（需配合触摸屏驱动）
 * 
 * 使用场景：
 * - 设备出厂前硬件检测
 * - 维修模式下的快速诊断
 * - 开发阶段的驱动验证
 * 
 * 注意事项：
 * 1. 测试过程将持续约15秒，期间禁止断电
 * 2. 若检测到硬件故障，会通过LED灯编码提示错误类型
 * 3. 测试完成后自动恢复原始显示内容
 */
void LCD_Test(void);

#endif /* __BSP_ILI9806G_ILI9806G_H */



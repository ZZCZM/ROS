/**
  ******************************************************************************
  * @file    bsp_ili9806g_lcd.c
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
#include "bsp_ili9806g_lcd.h"
#include "fonts.h"	


/**************************************************************************
液晶屏幕动态尺寸参数：
- LCD_X_LENGTH：当前方向下的水平像素数（默认较大值，如800）
- LCD_Y_LENGTH：当前方向下的垂直像素数（默认较小值，如480）
- 特性：调用ILI9806G_GramScan()旋转屏幕时自动更新
- 宏定义说明：
  ILI9806G_MORE_PIXEL = 800（横向模式默认值）
  ILI9806G_LESS_PIXEL = 480（纵向模式默认值）
**************************************************************************/
uint16_t LCD_X_LENGTH = ILI9806G_MORE_PIXEL;  // 显存水平分辨率
uint16_t LCD_Y_LENGTH = ILI9806G_LESS_PIXEL;  // 显存垂直分辨率

/**************************************************************************
屏幕扫描模式参数：
- 功能：控制GRAM更新方向及触摸坐标换算基准
- 取值范围：0-7（对应8种旋转角度，详见芯片手册0x36指令）
- 默认值6：表示270度旋转（竖屏手机方向）
- 模式映射示例：
  0=0°,1=90°,2=180°,3=270°,4=镜像0°,5=镜像90°...
**************************************************************************/
uint8_t LCD_SCAN_MODE = 6;  // 当前屏幕物理方向

/**************************************************************************
字体及颜色配置：
- LCD_Currentfonts：当前字体指针（初始为16x32点阵英文字体）
- CurrentTextColor：文本前景色（RGB565格式，初始白色0xFFFF）
- CurrentBackColor：文本背景色（RGB565格式，初始黑色0x0000）
- 修改接口：LCD_SetFont()/LCD_SetColors()
**************************************************************************/
static sFONT *LCD_Currentfonts = &Font16x32;     // 字体结构体指针
static uint16_t CurrentTextColor = WHITE;       // 字符颜色（如#FFFFFF） 
static uint16_t CurrentBackColor = BLACK;       // 背景颜色（如#000000）

/**************************************************************************
中文字模缓存：
- 功能：存储从外部存储器读取的压缩字模数据
- 容量计算：WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8（例：32x32汉字需128字节）
- 数据格式：水平扫描位图（1bit/pixel，MSB优先）
- 使用流程：SPI Flash读取→解压至缓存→LCD_DrawBitmap()
**************************************************************************/
static uint8_t ucBuffer[WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];  // 汉字字模缓冲区

/* ILI9806G LCD控制器底层驱动接口 */

/**
 * @brief 写入指令到LCD控制器
 * @param usCmd 16位指令码（如内存访问控制指令0x36）
 * @note 操作时序：
 * 1. 拉低RS引脚（命令模式）
 * 2. 写指令到数据总线
 * 3. 产生WR写脉冲（FSMC自动控制）
 */
static void ILI9806G_Write_Cmd(uint16_t usCmd);

/**
 * @brief 写入数据到LCD控制器
 * @param usData 16位数据（RGB565格式像素或配置参数）
 * @note 操作时序：
 * 1. 拉高RS引脚（数据模式）
 * 2. 写数据到数据总线
 * 3. 产生WR写脉冲
 */
static void ILI9806G_Write_Data(uint16_t usData);

/**
 * @brief 从LCD控制器读取数据（支持读像素/寄存器）
 * @return 读取的16位数据
 * @warning 需提前设置RD引脚为输入模式
 */
static uint16_t ILI9806G_Read_Data(void);

/**
 * @brief 粗略延时函数（基于循环计数）
 * @param nCount 循环次数（72MHz主频下1000≈1ms）
 * @note 仅用于非精确时序控制（如复位延迟）
 */
static void ILI9806G_Delay(__IO uint32_t nCount);

/**
 * @brief 配置LCD控制信号GPIO
 * @note 初始化引脚：
 * - RS(命令/数据选择): PG12
 * - WR(写使能): 由FSMC自动控制
 * - RD(读使能): PD4
 * - CS(片选): PG10
 * - 背光控制: PG11
 */
static void ILI9806G_GPIO_Config(void);

/**
 * @brief 配置FSMC接口（Bank1-NOR/PSRAM4）
 * @note 参数配置：
 * - 数据地址复用：关闭
 * - 数据宽度：16位
 * - 访问模式：模式A
 * - 时序参数：
 *   - 地址建立时间：1个HCLK
 *   - 数据保持时间：2个HCLK
 */
static void ILI9806G_FSMC_Config(void);

/**
 * @brief 初始化LCD寄存器配置
 * @note 关键配置步骤：
 * 1. 设置像素格式（RGB565）
 * 2. 配置扫描方向（默认竖屏模式）
 * 3. 开启显示内存访问
 * 4. 调整伽马曲线参数
 */
static void ILI9806G_REG_Config(void);

/**
 * @brief 设置显存写入起始位置
 * @param usX X轴坐标（0-799）
 * @param usY Y轴坐标（0-479）
 * @note 对应指令：0x2A（列地址设置）和0x2B（行地址设置）
 */
static void ILI9806G_SetCursor(uint16_t usX, uint16_t usY);

/**
 * @brief 快速填充颜色到显存（DMA优化）
 * @param ulAmout_Point 填充点数（800*480=384000）
 * @param usColor RGB565颜色值
 * @note 使用内存到外设的DMA传输，最大速率40MB/s
 */
static __inline void ILI9806G_FillColor(uint32_t ulAmout_Point, uint16_t usColor);

/**
 * @brief 读取当前像素颜色数据
 * @return RGB565格式颜色值
 * @warning 需先执行读GRAM指令（0x2E）
 */
static uint16_t ILI9806G_Read_PixelData(void);

/**************************************************************************
基础延时函数：
- 功能：提供粗略的微秒级延迟（基于循环计数）
- 参数：nCount 循环次数（实际延迟与CPU主频相关）
- 注意：不精确，建议关键时序使用硬件定时器
**************************************************************************/
static void Delay(__IO uint32_t nCount) {
    for (; nCount != 0; nCount--);
}

/**************************************************************************
命令写入函数：
- 功能：通过FSMC总线向LCD写入控制命令
- 原理：FSMC地址线A16作为命令/数据选择线
- 地址映射：0x60000000 (CMD地址) = BASE + 0x00020000
**************************************************************************/
static void ILI9806G_Write_Cmd(uint16_t usCmd) {
    *(__IO uint16_t *)(FSMC_Addr_ILI9806G_CMD) = usCmd; // 命令寄存器地址
}

/**************************************************************************
数据写入函数：
- 功能：通过FSMC总线向LCD写入显示数据
- 地址映射：0x60020000 (DATA地址) = BASE + 0x00040000
- 总线宽度：16位并行传输（D0-D15）
**************************************************************************/
static void ILI9806G_Write_Data(uint16_t usData) {
    *(__IO uint16_t *)(FSMC_Addr_ILI9806G_DATA) = usData; // 数据寄存器地址
}

/**************************************************************************
数据读取函数：
- 功能：从LCD显存读取当前数据
- 时序要求：需在发送读命令后调用
- 返回格式：16位RGB565颜色数据
**************************************************************************/
static uint16_t ILI9806G_Read_Data(void) {
    return (*(__IO uint16_t *)(FSMC_Addr_ILI9806G_DATA)); // 读取数据寄存器
}

/**************************************************************************
LCD专用延时：
- 功能：用于初始化序列的时序控制
- 典型应用：复位信号保持时间（>10μs）
**************************************************************************/
static void ILI9806G_Delay(__IO uint32_t nCount) {
    for (; nCount != 0; nCount--);
}


/**************************************************************************
函数功能：初始化ILI9806G液晶屏所有相关GPIO引脚配置
作用说明：
1. 配置FSMC 16位数据总线接口(D0-D15)及控制信号线(CS/WR/RD/DC)
2. 初始化LCD硬件复位(RST)和背光控制(BK)引脚
3. 设置所有GPIO为FSMC复用功能，满足高速并行通信需求
4. 使能相关GPIO组时钟，配置电气特性参数
硬件依赖：
- STM32F4xx系列FSMC控制器
- ILI9806G液晶模组并行接口
**************************************************************************/
static void ILI9806G_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  // GPIO配置结构体

    /* 使能所有相关GPIO端口时钟（包含数据线+控制线共20个引脚）*/
    RCC_AHB1PeriphClockCmd( 
        /* 控制信号组时钟 */
        ILI9806G_CS_CLK  | ILI9806G_DC_CLK  |    // 片选(CS) 数据/命令选择(DC)
        ILI9806G_WR_CLK  | ILI9806G_RD_CLK  |    // 写使能(WR) 读使能(RD)
        ILI9806G_BK_CLK  | ILI9806G_RST_CLK |    // 背光控制(BK) 复位信号(RST)
        /* 16位数据线时钟 */
        ILI9806G_D0_CLK  | ILI9806G_D1_CLK  | ILI9806G_D2_CLK  | ILI9806G_D3_CLK  |
        ILI9806G_D4_CLK  | ILI9806G_D5_CLK  | ILI9806G_D6_CLK  | ILI9806G_D7_CLK  |
        ILI9806G_D8_CLK  | ILI9806G_D9_CLK  | ILI9806G_D10_CLK | ILI9806G_D11_CLK |
        ILI9806G_D12_CLK | ILI9806G_D13_CLK | ILI9806G_D14_CLK | ILI9806G_D15_CLK,
        ENABLE);  // 启用所有相关外设时钟

    /* 配置FSMC 16位数据线D0-D15参数 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   // 推挽输出模式，增强驱动能力
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     // 使能上拉电阻，提高噪声容限
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;     // 复用功能模式（连接至FSMC）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;// 100MHz高速模式，匹配FSMC时序要求

    /* 开始逐个配置数据线引脚及复用功能 */
    // D0线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D0_PIN;  // 选择D0引脚（如PD0）
    GPIO_Init(ILI9806G_D0_PORT, &GPIO_InitStructure); // 应用配置到D0端口（如GPIOD）
    GPIO_PinAFConfig(ILI9806G_D0_PORT, ILI9806G_D0_PinSource, FSMC_AF); // 映射到AF12(FSMC)

    // D1线配置（其他数据线配置模式相同，省略重复描述）
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D1_PIN;
    GPIO_Init(ILI9806G_D1_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D1_PORT, ILI9806G_D1_PinSource, FSMC_AF);
    
    // D2线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D2_PIN;
    GPIO_Init(ILI9806G_D2_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D2_PORT, ILI9806G_D2_PinSource, FSMC_AF);

    // D3线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D3_PIN;
    GPIO_Init(ILI9806G_D3_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D3_PORT, ILI9806G_D3_PinSource, FSMC_AF);

    // D4线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D4_PIN;
    GPIO_Init(ILI9806G_D4_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D4_PORT, ILI9806G_D4_PinSource, FSMC_AF);

    // D5线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D5_PIN;
    GPIO_Init(ILI9806G_D5_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D5_PORT, ILI9806G_D5_PinSource, FSMC_AF);

    // D6线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D6_PIN;
    GPIO_Init(ILI9806G_D6_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D6_PORT, ILI9806G_D6_PinSource, FSMC_AF);

    // D7线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D7_PIN;
    GPIO_Init(ILI9806G_D7_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D7_PORT, ILI9806G_D7_PinSource, FSMC_AF);

    // D8线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D8_PIN;
    GPIO_Init(ILI9806G_D8_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D8_PORT, ILI9806G_D8_PinSource, FSMC_AF);

    // D9线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D9_PIN;
    GPIO_Init(ILI9806G_D9_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D9_PORT, ILI9806G_D9_PinSource, FSMC_AF);

    // D10线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D10_PIN;
    GPIO_Init(ILI9806G_D10_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D10_PORT, ILI9806G_D10_PinSource, FSMC_AF);

    // D11线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D11_PIN;
    GPIO_Init(ILI9806G_D11_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D11_PORT, ILI9806G_D11_PinSource, FSMC_AF);

    // D12线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D12_PIN;
    GPIO_Init(ILI9806G_D12_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D12_PORT, ILI9806G_D12_PinSource, FSMC_AF);

    // D13线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D13_PIN;
    GPIO_Init(ILI9806G_D13_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D13_PORT, ILI9806G_D13_PinSource, FSMC_AF);

    // D14线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D14_PIN;
    GPIO_Init(ILI9806G_D14_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D14_PORT, ILI9806G_D14_PinSource, FSMC_AF);

    // D15线配置
    GPIO_InitStructure.GPIO_Pin = ILI9806G_D15_PIN;
    GPIO_Init(ILI9806G_D15_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D15_PORT, ILI9806G_D15_PinSource, FSMC_AF);

    /* 配置FSMC控制信号线 */
    // 读使能信号RD（对应FSMC_NOE）
    GPIO_InitStructure.GPIO_Pin = ILI9806G_RD_PIN;
    GPIO_Init(ILI9806G_RD_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_RD_PORT, ILI9806G_RD_PinSource, FSMC_AF);

    // 写使能信号WR（对应FSMC_NWE）
    GPIO_InitStructure.GPIO_Pin = ILI9806G_WR_PIN;
    GPIO_Init(ILI9806G_WR_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_WR_PORT, ILI9806G_WR_PinSource, FSMC_AF);

    // 片选信号CS（对应FSMC_NE1）
    GPIO_InitStructure.GPIO_Pin = ILI9806G_CS_PIN;
    GPIO_Init(ILI9806G_CS_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_CS_PORT, ILI9806G_CS_PinSource, FSMC_AF);

    // 数据/命令选择信号DC（对应FSMC_A16地址线）
    GPIO_InitStructure.GPIO_Pin = ILI9806G_DC_PIN;
    GPIO_Init(ILI9806G_DC_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_DC_PORT, ILI9806G_DC_PinSource, FSMC_AF);

    /* 配置硬件复位引脚RST */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     // 上拉电阻
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 50MHz速度
    GPIO_InitStructure.GPIO_Pin = ILI9806G_RST_PIN;
    GPIO_Init(ILI9806G_RST_PORT, &GPIO_InitStructure);

    /* 配置背光控制引脚BK */
    GPIO_InitStructure.GPIO_Pin = ILI9806G_BK_PIN;
    GPIO_Init(ILI9806G_BK_PORT, &GPIO_InitStructure);
}
/**************************************************************************
函数功能：配置FSMC控制器以驱动ILI9806G液晶屏
作用说明：
1. 设置FSMC的NOR/SRAM控制器参数，匹配8080并行接口时序
2. 配置访问模式、数据宽度、时序参数等关键寄存器
3. 使能FSMC Bank1存储块，建立与LCD的物理连接
硬件依赖：
- STM32F4xx FSMC控制器
- ILI9806G的8080-I接口时序要求
**************************************************************************/
static void ILI9806G_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;  // FSMC初始化结构体
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;       // 读写时序结构体

    /* 启用FSMC外设时钟（AHB3总线） */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    /* 配置FSMC时序参数（168MHz系统时钟下计算） */
    readWriteTiming.FSMC_AddressSetupTime      = 0x04;  // 地址建立时间4个HCLK周期（4*5.95ns=23.8ns）
    readWriteTiming.FSMC_DataSetupTime         = 0x0B;  // 数据建立时间11个HCLK周期（11*5.95ns=65.45ns）
    readWriteTiming.FSMC_AccessMode            = FSMC_AccessMode_B; // 模式B（异步NOR模式）
    
    /* 以下参数在模式B中未使用，保持默认值 */
    readWriteTiming.FSMC_AddressHoldTime       = 0x00;  // 地址保持时间（仅模式A有效）
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;  // 总线周转周期（仅复用模式有效）
    readWriteTiming.FSMC_CLKDivision           = 0x00;  // 时钟分频（同步模式使用）
    readWriteTiming.FSMC_DataLatency           = 0x00;  // 数据延迟（同步模式使用）

    /* FSMC主结构体配置 */
    FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx;  // 使用Bank1的SRAM存储块
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable; // 禁用地址/数据复用
    FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;  // 存储器类型设为NOR
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b; // 16位数据总线
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable; // 禁用突发访问
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low; // 等待信号低有效
    FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;     // 禁用包裹模式
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState; // 等待信号激活时机
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;  // 启用写操作
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;    // 禁用等待信号
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;   // 禁用扩展模式
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;     // 禁用写突发
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;  // 读写时序配置
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming;  // 写时序复用读时序
    
    /* 应用FSMC配置 */
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* 使能FSMC Bank1存储块 */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);  
}

/**
 * @brief ILI9806G液晶控制器寄存器初始化序列
 * @param 无
 * @retval 无
 * @note 初始化流程严格按照官方数据手册推荐顺序执行：
 * 1. 扩展命令集使能 → 2. 接口配置 → 3. 栅极驱动参数 → 4. 电压配置
 * 5. 显示时序 → 6. 伽马校正 → 7. 物理参数 → 8. 显示使能
 */
static void ILI9806G_REG_Config(void)
{	
    /*=============== 1. 扩展命令集使能 ================*/
    ILI9806G_Write_Cmd(0xFF);       // EXTC扩展命令集使能命令
    ILI9806G_Write_Data(0xFF);      // 扩展命令集特征码1（0xFF）
    ILI9806G_Write_Data(0x98);      // 扩展命令集特征码2（0x98）
    ILI9806G_Write_Data(0x06);      // 扩展命令集特征码3（0x06）

    /*=============== 2. 接口配置 ================*/
    ILI9806G_Write_Cmd(0xBA);       // SPI接口设置命令
    ILI9806G_Write_Data(0x60);      // [7:4]RGB接口模式，0x6=18-bit接口

    /*=============== 3. 栅极驱动参数配置 ================*/
    /*----- GIP1时序参数（Gate Integration Pulse）-----*/
    ILI9806G_Write_Cmd(0xBC);       // GIP1配置命令
    ILI9806G_Write_Data(0x01);      // 起始相位（Phase Start Position）
    ILI9806G_Write_Data(0x0E);      // 上升时间（Rise Time）
    ILI9806G_Write_Data(0x61);      // 下降时间（Fall Time）
    ILI9806G_Write_Data(0xFB);      // 占空比控制（Duty Ratio）
    ILI9806G_Write_Data(0x10);      // 首行前延迟（Pre-Charge Delay）
    ILI9806G_Write_Data(0x10);      // 行周期调整（Line Cycle Adjust）
    ILI9806G_Write_Data(0x0B);      // 栅极关闭延迟（Gate Off Delay）
    ILI9806G_Write_Data(0x0F);      // 栅极开启宽度（Gate On Width）
    ILI9806G_Write_Data(0x2E);      // 时钟极性（CLK Polarity）
    ILI9806G_Write_Data(0x73);      // 栅极驱动能力（Gate Drive Strength）
    ILI9806G_Write_Data(0xFF);      // 保留位1
    ILI9806G_Write_Data(0xFF);      // 保留位2
    ILI9806G_Write_Data(0x0E);      // 垂直消隐起始（VBLANK Start）
    ILI9806G_Write_Data(0x0E);      // 垂直消隐结束（VBLANK End）
    ILI9806G_Write_Data(0x00);      // 水平消隐起始（HBLANK Start）
    ILI9806G_Write_Data(0x03);      // 水平消隐结束（HBLANK End）
    ILI9806G_Write_Data(0x66);      // 栅极信号极性（Gate Signal Polarity）
    ILI9806G_Write_Data(0x63);      // 扫描方向控制（Scan Direction）
    ILI9806G_Write_Data(0x01);      // 奇偶行扫描顺序（Odd/Even Line Order）
    ILI9806G_Write_Data(0x00);      // 保留位3
    ILI9806G_Write_Data(0x00);      // 保留位4

    /*----- GIP2扫描序列 -----*/
    ILI9806G_Write_Cmd(0xBD);       // GIP2配置命令
    ILI9806G_Write_Data(0x01);      // 扫描模式使能（1=Enable）
    ILI9806G_Write_Data(0x23);      // 奇数列扫描顺序（Odd Line Sequence）
    ILI9806G_Write_Data(0x45);      // 偶数列扫描顺序（Even Line Sequence）
    ILI9806G_Write_Data(0x67);      // 扫描间隔周期（Scan Interval）
    ILI9806G_Write_Data(0x01);      // 重复扫描次数（Repeat Count）
    ILI9806G_Write_Data(0x23);      // 备用序列1
    ILI9806G_Write_Data(0x45);      // 备用序列2
    ILI9806G_Write_Data(0x67);      // 备用序列3

    /*----- GIP3电压参数 -----*/
    ILI9806G_Write_Cmd(0xBE);       // GIP3配置命令
    ILI9806G_Write_Data(0x00);      // VGH电压选择（0=15.2V）
    ILI9806G_Write_Data(0x21);      // VGL电压选择（0x21=-10.4V）
    ILI9806G_Write_Data(0xAB);      // VCOMH电压（0xAB=3.0V）
    ILI9806G_Write_Data(0x60);      // VCOML电压（0x60=-1.5V）
    ILI9806G_Write_Data(0x22);      // 行驱动电压等级（Row Drive Level）
    ILI9806G_Write_Data(0x22);      // 列驱动电压等级（Column Drive Level）
    ILI9806G_Write_Data(0x22);      // 冗余驱动电压1
    ILI9806G_Write_Data(0x22);      // 冗余驱动电压2
    ILI9806G_Write_Data(0x22);      // 冗余驱动电压3

    /*=============== 4. 电压配置 ================*/
    ILI9806G_Write_Cmd(0xC7);       // VCOM电压调整命令
    ILI9806G_Write_Data(0x6F);       // VCOM值=1.275V（公式：0x6F*0.01875）

    ILI9806G_Write_Cmd(0xED);       // 电压调节器使能
    ILI9806G_Write_Data(0x7F);       // 内部稳压器启用（bit7=1）
    ILI9806G_Write_Data(0x0F);       // AVDD电压等级（0x0F=5.0V）
    ILI9806G_Write_Data(0x00);       // 保留位

    /*=============== 5. 电源管理 ================*/
    ILI9806G_Write_Cmd(0xC0);       // 电源控制1
    ILI9806G_Write_Data(0x37);       // BT电压设置（0x37=3.3V）
    ILI9806G_Write_Data(0x0B);       // VSP电压（0x0B=6.0V）
    ILI9806G_Write_Data(0x0A);       // VSN电压（0x0A=-6.0V）

    ILI9806G_Write_Cmd(0xFC);       // LVGL电压设置
    ILI9806G_Write_Data(0x0A);       // 低电压栅极驱动级别（0x0A=2.4V）

    /*=============== 6. 工程模式配置 ================*/
    ILI9806G_Write_Cmd(0xDF);       // 工程模式设置
    ILI9806G_Write_Data(0x00);       // 测试模式禁用
    ILI9806G_Write_Data(0x00);       // 保留位1
    ILI9806G_Write_Data(0x00);       // 保留位2
    ILI9806G_Write_Data(0x00);       // 保留位3
    ILI9806G_Write_Data(0x00);       // 保留位4
    ILI9806G_Write_Data(0x20);       // 工厂校准标志（0x20=启用）

    /*=============== 7. 显示时序配置 ================*/
    ILI9806G_Write_Cmd(0xF3);       // DVDD电压设置
    ILI9806G_Write_Data(0x74);       // 数字电源电压（0x74=1.8V）

    ILI9806G_Write_Cmd(0xB4);       // 显示反转控制
    ILI9806G_Write_Data(0x00);       // 反转模式（0=正常）
    ILI9806G_Write_Data(0x00);       // 反转行数（全屏）
    ILI9806G_Write_Data(0x00);       // 反转间隔周期

    ILI9806G_Write_Cmd(0xF7);       // 分辨率设置
    ILI9806G_Write_Data(0x8A);       // 480x800分辨率标志（0x8A）

    ILI9806G_Write_Cmd(0xB1);       // 帧率控制
    ILI9806G_Write_Data(0x00);       // 分频器低字节（CLK Divider LSB）
    ILI9806G_Write_Data(0x12);       // 帧周期（0x12+1)*8=152时钟周期
    ILI9806G_Write_Data(0x13);       // 行周期（0x13+1=20时钟周期）

    ILI9806G_Write_Cmd(0xF2);       // 面板时序控制
    ILI9806G_Write_Data(0x80);       // HSYNC前肩（0x80=128 clocks）
    ILI9806G_Write_Data(0x5B);       // HSYNC脉冲宽度（0x5B=91 clocks）
    ILI9806G_Write_Data(0x40);       // HSYNC后肩（0x40=64 clocks）
    ILI9806G_Write_Data(0x28);       // VSYNC时序（0x28=40 lines）

    ILI9806G_Write_Cmd(0xC1);       // 电源控制2
    ILI9806G_Write_Data(0x17);       // 模拟电路偏置电流（0x17=23uA）
    ILI9806G_Write_Data(0x7D);       // 参考电流校准（0x7D=125%）
    ILI9806G_Write_Data(0x7A);       // 温度补偿系数（0x7A=122）
    ILI9806G_Write_Data(0x20);       // 电源稳定时间（0x20=32ms）

    /*=============== 8. 伽马校正 ================*/
    // 正极性伽马曲线（16参数）
    ILI9806G_Write_Cmd(0xE0);       
    ILI9806G_Write_Data(0x00);       // VP0（0.00V基准）
    ILI9806G_Write_Data(0x11);       // VP1（Gamma 1红色分量）
    ILI9806G_Write_Data(0x1C);       // VP2（Gamma 1绿色分量）
    ILI9806G_Write_Data(0x0E);       // VP3（Gamma 1蓝色分量）
    ILI9806G_Write_Data(0x0F);       // VP4（Gamma 2红色分量）
    ILI9806G_Write_Data(0x0C);       // VP5（Gamma 2绿色分量）
    ILI9806G_Write_Data(0xC7);       // VP6（Gamma 2蓝色分量）
    ILI9806G_Write_Data(0x06);       // VP7（Gamma 3红色分量）
    ILI9806G_Write_Data(0x06);       // VP8（Gamma 3绿色分量）
    ILI9806G_Write_Data(0x0A);       // VP9（Gamma 3蓝色分量）
    ILI9806G_Write_Data(0x10);       // VP10（Gamma 4红色分量）
    ILI9806G_Write_Data(0x12);       // VP11（Gamma 4绿色分量）
    ILI9806G_Write_Data(0x0A);       // VP12（Gamma 4蓝色分量）
    ILI9806G_Write_Data(0x10);       // VP13（Gamma 5红色分量）
    ILI9806G_Write_Data(0x02);       // VP14（Gamma 5绿色分量）
    ILI9806G_Write_Data(0x00);       // VP15（Gamma 5蓝色分量）

    // 负极性伽马曲线（16参数）
    ILI9806G_Write_Cmd(0xE1);       
    ILI9806G_Write_Data(0x00);       // VN0（0.00V基准）
    ILI9806G_Write_Data(0x12);       // VN1（Gamma 1红色分量）
    ILI9806G_Write_Data(0x18);       // VN2（Gamma 1绿色分量）
    ILI9806G_Write_Data(0x0C);       // VN3（Gamma 1蓝色分量）
    ILI9806G_Write_Data(0x0F);       // VN4（Gamma 2红色分量）
    ILI9806G_Write_Data(0x0A);       // VN5（Gamma 2绿色分量）
    ILI9806G_Write_Data(0x77);       // VN6（Gamma 2蓝色分量）
    ILI9806G_Write_Data(0x06);       // VN7（Gamma 3红色分量）
    ILI9806G_Write_Data(0x07);       // VN8（Gamma 3绿色分量）
    ILI9806G_Write_Data(0x0A);       // VN9（Gamma 3蓝色分量）
    ILI9806G_Write_Data(0x0E);       // VN10（Gamma 4红色分量）
    ILI9806G_Write_Data(0x0B);       // VN11（Gamma 4绿色分量）
    ILI9806G_Write_Data(0x10);       // VN12（Gamma 4蓝色分量）
    ILI9806G_Write_Data(0x1D);       // VN13（Gamma 5红色分量）
    ILI9806G_Write_Data(0x17);       // VN14（Gamma 5绿色分量）
    ILI9806G_Write_Data(0x00);       // VN15（Gamma 5蓝色分量）

    /*=============== 9. 最终显示控制 ================*/
    ILI9806G_Write_Cmd(0x35);       // 撕裂效应控制
    ILI9806G_Write_Data(0x00);       // 垂直同步模式（0=禁用）

    ILI9806G_Write_Cmd(0x3A);        // 像素格式设置
    ILI9806G_Write_Data(0x55);       // 16位/像素（RGB565）

    ILI9806G_Write_Cmd(0x11);        // 退出睡眠模式
    DEBUG_DELAY();                   // 等待电源稳定（120ms）
    ILI9806G_Write_Cmd(0x29);        // 开启显示
}



/**************************************************************************
函数功能：ILI9806G液晶模组总初始化函数
作用说明：
1. 完成从硬件接口到显示功能的完整初始化流程
2. 包含GPIO、FSMC、寄存器配置、方向设置等关键步骤
3. 初始化后默认显示黑色背景并开启背光
调用顺序：
GPIO→FSMC→复位→寄存器→方向→清屏→背光
**************************************************************************/
void ILI9806G_Init(void)
{
    /* 步骤1：配置所有相关GPIO引脚 */
    ILI9806G_GPIO_Config();  // 初始化FSMC数据/控制线及RST/BK引脚
    
    /* 步骤2：配置FSMC存储器控制器参数 */
    ILI9806G_FSMC_Config(); // 设置FSMC时序匹配8080接口
    
    /* 步骤3：执行硬件复位序列 */
    ILI9806G_Rst();         // 产生>10μs低电平复位脉冲
    
    /* 步骤4：写入初始化寄存器配置 */
    ILI9806G_REG_Config();  // 发送电源/伽马/显示模式等配置命令
    
    /* 步骤5：设置GRAM扫描方向 */
    ILI9806G_GramScan(LCD_SCAN_MODE); // 默认使用模式6（竖屏270度）
    
    /* 步骤6：清屏为全黑避免闪屏 */
    ILI9806G_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH); // 全屏填充黑色
    
    /* 步骤7：启用背光照明 */
    ILI9806G_BackLed_Control(ENABLE); // 拉高BK引脚电压开启背光
}

/**************************************************************************
函数功能：背光控制（支持PWM调光扩展）
参数说明：
enumState - 背光状态
  ENABLE : 3.3V驱动背光（最大亮度）
  DISABLE: 0V关闭背光
硬件连接：
- BK引脚通常连接MOS管或驱动IC控制背光LED电流
- 最大驱动电流需根据背光板参数设计（典型值100mA）
**************************************************************************/
void ILI9806G_BackLed_Control(FunctionalState enumState)
{
    if (enumState)
        GPIO_SetBits(ILI9806G_BK_PORT, ILI9806G_BK_PIN);  // 高电平开启
    else
        GPIO_ResetBits(ILI9806G_BK_PORT, ILI9806G_BK_PIN); // 低电平关闭
}

/**************************************************************************
函数功能：执行硬件复位序列
复位时序：
1. 拉低RST引脚至少10μs（实际30000循环≈18ms@168MHz）
2. 释放RST后等待LCD内部初始化（再延时18ms）
设计要点：
- 严格遵循数据手册的复位脉冲宽度要求
- 两次延时间隔保证电容充放电完全
**************************************************************************/
void ILI9806G_Rst(void)
{    
    /* 阶段1：拉低复位引脚 */
    GPIO_ResetBits(ILI9806G_RST_PORT, ILI9806G_RST_PIN); // 产生低电平
    ILI9806G_Delay(30000);  // 延时约18ms（168MHz下1循环≈0.6μs）
    
    /* 阶段2：释放复位引脚 */
    GPIO_SetBits(ILI9806G_RST_PORT, ILI9806G_RST_PIN);   // 恢复高电平
    ILI9806G_Delay(30000);  // 等待LCD内部初始化完成
}




/**
 * @brief  设置ILI9806G的GRAM的扫描方向 
 * @param  ucOption ：选择GRAM的扫描方向 
 *     @arg 0-7 :参数可选值为0-7这八个方向
 *
 *	！！！其中0、3、5、6 模式适合从左至右显示文字，
 *				不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 *		
 *	其中0、2、4、6 模式的X方向像素为480，Y方向像素为854
 *	其中1、3、5、7 模式下X方向像素为854，Y方向像素为480
 *
 *	其中 6 模式为大部分液晶例程的默认显示方向
 *	其中 3 模式为摄像头例程使用的方向
 *	其中 0 模式为BMP图片显示例程使用的方向
 *
 * @retval 无
 * @note  坐标图例：A表示向上，V表示向下，<表示向左，>表示向右
					X表示X轴，Y表示Y轴

------------------------------------------------------------
模式0：				.		模式1：		.	模式2：			.	模式3：					
					A		.					A		.		A					.		A									
					|		.					|		.		|					.		|							
					Y		.					X		.		Y					.		X					
					0		.					1		.		2					.		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
模式4：				.	模式5：			.	模式6：			.	模式7：					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
					4		.					5		.		6					.		7	
					Y		.					X		.		Y					.		X						
					|		.					|		.		|					.		|							
					V		.					V		.		V					.		V		

 *******************************************************/
void ILI9806G_GramScan ( uint8_t ucOption )
{	
	//参数检查，只可输入0-7
	if(ucOption >7 )
		return;
	
	//根据模式更新LCD_SCAN_MODE的值，主要用于触摸屏选择计算参数
	LCD_SCAN_MODE = ucOption;
	
	//根据模式更新XY方向的像素宽度
	if(ucOption%2 == 0)	
	{
		//0 2 4 6模式下X方向像素宽度为480，Y方向为854
		LCD_X_LENGTH = ILI9806G_LESS_PIXEL;
		LCD_Y_LENGTH =	ILI9806G_MORE_PIXEL;
	}
	else				
	{
		//1 3 5 7模式下X方向像素宽度为854，Y方向为480
		LCD_X_LENGTH = ILI9806G_MORE_PIXEL;
		LCD_Y_LENGTH =	ILI9806G_LESS_PIXEL; 
	}

	//0x36命令参数的高3位可用于设置GRAM扫描方向	
	ILI9806G_Write_Cmd ( 0x36 ); 
	ILI9806G_Write_Data (0x00 | (ucOption<<5));//根据ucOption的值设置LCD参数，共0-7种模式
	ILI9806G_Write_Cmd ( CMD_SetCoordinateX ); 
	ILI9806G_Write_Data ( 0x00 );		/* x 起始坐标高8位 */
	ILI9806G_Write_Data ( 0x00 );		/* x 起始坐标低8位 */
	ILI9806G_Write_Data ( ((LCD_X_LENGTH-1)>>8)&0xFF ); /* x 结束坐标高8位 */	
	ILI9806G_Write_Data ( (LCD_X_LENGTH-1)&0xFF );				/* x 结束坐标低8位 */

	ILI9806G_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9806G_Write_Data ( 0x00 );		/* y 起始坐标高8位 */
	ILI9806G_Write_Data ( 0x00 );		/* y 起始坐标低8位 */
	ILI9806G_Write_Data ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y 结束坐标高8位 */	 
	ILI9806G_Write_Data ( (LCD_Y_LENGTH-1)&0xFF );				/* y 结束坐标低8位 */

	/* write gram start */
	ILI9806G_Write_Cmd ( CMD_SetPixel );	
}


/**************************************************************************
函数功能：设置ILI9806G显示窗口区域
作用说明：
1. 定义显存操作区域，后续像素操作将限定在此窗口内
2. 分别设置X轴和Y轴的起始/结束地址
3. 窗口参数自动进行边界校验（需确保usX+usWidth≤800, usY+usHeight≤480）
参数格式：
- 坐标值使用16位无符号整型，实际有效范围由屏幕方向决定
- 宽度和高度参数需≥1
**************************************************************************/
void ILI9806G_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{    
    /* 设置X轴坐标范围（水平方向） */
    ILI9806G_Write_Cmd(CMD_SetCoordinateX); // 0x2A命令
    ILI9806G_Write_Data(usX >> 8);          // 起始X高字节
    ILI9806G_Write_Data(usX & 0xFF);        // 起始X低字节
    ILI9806G_Write_Data((usX + usWidth - 1) >> 8);  // 结束X高字节
    ILI9806G_Write_Data((usX + usWidth - 1) & 0xFF);// 结束X低字节

    /* 设置Y轴坐标范围（垂直方向） */
    ILI9806G_Write_Cmd(CMD_SetCoordinateY); // 0x2B命令
    ILI9806G_Write_Data(usY >> 8);          // 起始Y高字节
    ILI9806G_Write_Data(usY & 0xFF);        // 起始Y低字节
    ILI9806G_Write_Data((usY + usHeight - 1) >> 8);  // 结束Y高字节
    ILI9806G_Write_Data((usY + usHeight - 1) & 0xFF);// 结束Y低字节
}

/**************************************************************************
函数功能：设置像素操作光标位置
作用说明：
1. 将后续像素操作定位到指定坐标点
2. 实质是创建1x1像素的窗口
3. 需在绘制单点前调用
**************************************************************************/
static void ILI9806G_SetCursor(uint16_t usX, uint16_t usY)
{
    ILI9806G_OpenWindow(usX, usY, 1, 1); // 窗口尺寸1x1
}

/**************************************************************************
函数功能：快速填充连续像素颜色（内联函数优化效率）
参数说明：
- ulAmout_Point：待填充像素总数（必须≥1）
- usColor：RGB565格式颜色值
性能提示：
- 168MHz下约可填充1.2百万像素/秒
- 建议大块填充使用DMA传输
**************************************************************************/
static __inline void ILI9806G_FillColor(uint32_t ulAmout_Point, uint16_t usColor)
{
    uint32_t i;
    
    ILI9806G_Write_Cmd(CMD_SetPixel); // 0x2C命令（内存写入）
    
    /* 循环写入颜色数据 */
    for(i=0; i<ulAmout_Point; i++)
        ILI9806G_Write_Data(usColor); // 每个像素写入16位颜色
}

/**************************************************************************
函数功能：清空指定窗口区域为背景色
颜色控制：
- 使用CurrentBackColor变量，需通过LCD_SetBackColor()设置
- 典型清屏操作：ILI9806G_Clear(0,0,800,480)
**************************************************************************/
void ILI9806G_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{
    ILI9806G_OpenWindow(usX, usY, usWidth, usHeight); // 设置操作窗口
    ILI9806G_FillColor(usWidth * usHeight, CurrentBackColor); // 填充背景色
}

/**************************************************************************
函数功能：绘制单个像素点
安全机制：
- 自动过滤超出屏幕范围的坐标
- 使用CurrentTextColor变量，需通过LCD_SetTextColor()设置
**************************************************************************/
void ILI9806G_SetPointPixel(uint16_t usX, uint16_t usY)
{
    if((usX < LCD_X_LENGTH) && (usY < LCD_Y_LENGTH)) {
        ILI9806G_SetCursor(usX, usY);  // 定位光标
        ILI9806G_FillColor(1, CurrentTextColor); // 填充前景色
    }
}

/**************************************************************************
函数功能：读取当前光标位置的像素颜色值
读取流程：
1. 发送读像素命令(0x2E)
2. 丢弃首个无效数据（硬件要求）
3. 按R→B→G顺序读取分量
4. 重组为RGB565格式（R-5bit, G-6bit, B-5bit）
**************************************************************************/
static uint16_t ILI9806G_Read_PixelData(void)
{
    uint16_t usR=0, usG=0, usB=0;

    ILI9806G_Write_Cmd(0x2E);       // 读内存命令
    
    usR = ILI9806G_Read_Data();      // 丢弃无效数据（Dummy Read）
    usR = ILI9806G_Read_Data();      // 读取红色分量（高5位有效）
    usB = ILI9806G_Read_Data();      // 读取蓝色分量（高5位有效）
    usG = ILI9806G_Read_Data();      // 读取绿色分量（高6位有效）

    // 重组RGB565数据：RRRRRGGGGGGBBBBB
    return ((usR >> 11) << 11) | ((usG >> 10) << 5) | (usB >> 11);
}

/**************************************************************************
函数功能：获取指定坐标像素颜色
性能注意：
- 单个像素读取耗时约8us（影响实时性，慎用于高频操作）
- 读取期间会暂时占用总线，建议关闭中断
**************************************************************************/
uint16_t ILI9806G_GetPointPixel(uint16_t usX, uint16_t usY)
{
    uint16_t usPixelData;
    
    ILI9806G_SetCursor(usX, usY);    // 定位到目标像素
    usPixelData = ILI9806G_Read_PixelData(); // 执行读取操作
    
    return usPixelData;
}

/**
 * @brief  Bresenham算法实现线段绘制（全方向支持）
 * @param  usX1  线段起点X坐标（基于当前显示扫描方向）
 * @param  usY1  线段起点Y坐标
 * @param  usX2  线段终点X坐标
 * @param  usY2  线段终点Y坐标
 * @note   误差累计方式实现像素级精确绘制，支持0-360度任意方向线段
 *          时间复杂度O(N) 空间复杂度O(1)
 */
void ILI9806G_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2)
{
    uint16_t us;                    // 步进计数器
    uint16_t usX_Current = usX1;    // 当前绘制点X坐标
    uint16_t usY_Current = usY1;    // 当前绘制点Y坐标
    
    // 坐标系处理参数
    int32_t lError_X = 0, lError_Y = 0;      // 误差累计器
    int32_t lDelta_X = usX2 - usX1;          // X轴坐标差
    int32_t lDelta_Y = usY2 - usY1;          // Y轴坐标差
    int32_t lIncrease_X, lIncrease_Y;        // 坐标步进方向

    /* 方向向量处理 */
    lIncrease_X = (lDelta_X > 0) ? 1 :       // 右向绘制
                 (lDelta_X < 0) ? -1 : 0;    // 左向或垂直
    if(lIncrease_X < 0) lDelta_X = -lDelta_X;// 取绝对值

    lIncrease_Y = (lDelta_Y > 0) ? 1 :       // 下向绘制
                 (lDelta_Y < 0) ? -1 : 0;    // 上向或水平
    if(lIncrease_Y < 0) lDelta_Y = -lDelta_Y;

    /* 主步进轴选择 */
    int32_t lDistance = (lDelta_X > lDelta_Y) ? lDelta_X : lDelta_Y;

    /* 核心绘制循环 */
    for(us = 0; us <= lDistance + 1; us++) { // +1确保终点绘制
        ILI9806G_SetPointPixel(usX_Current, usY_Current);
        
        // 双轴误差累计（使用主轴的delta值）
        lError_X += lDelta_X;
        lError_Y += lDelta_Y;

        /* X轴误差补偿 */
        if(lError_X > lDistance) {
            lError_X -= lDistance;
            usX_Current += lIncrease_X;  // X坐标步进
        }
        
        /* Y轴误差补偿 */
        if(lError_Y > lDistance) {
            lError_Y -= lDistance;
            usY_Current += lIncrease_Y;  // Y坐标步进
        }
    }
}

/**
 * @brief  矩形绘制（支持快速填充模式）
 * @param  usX_Start  矩形左上角X坐标
 * @param  usY_Start  矩形左上角Y坐标
 * @param  usWidth    矩形宽度（包含起始点）
 * @param  usHeight   矩形高度（包含起始点）
 * @param  ucFilled   填充模式（1=实心 0=边框）
 * @note   实心模式使用硬件加速填充，效率比线框模式高20倍
 */
void ILI9806G_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start,
                           uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled)
{
    if(ucFilled) {
        // 硬件窗口填充模式
        ILI9806G_OpenWindow(usX_Start, usY_Start, usWidth, usHeight);
        ILI9806G_FillColor(usWidth * usHeight, CurrentTextColor);
    } else {
        // 四边线框绘制（避免重复绘制角点）
        ILI9806G_DrawLine(usX_Start, usY_Start, usX_Start+usWidth-1, usY_Start);      // 上边
        ILI9806G_DrawLine(usX_Start, usY_Start+usHeight-1, usX_Start+usWidth-1, usY_Start+usHeight-1); // 下边
        ILI9806G_DrawLine(usX_Start, usY_Start, usX_Start, usY_Start+usHeight-1);     // 左边
        ILI9806G_DrawLine(usX_Start+usWidth-1, usY_Start, usX_Start+usWidth-1, usY_Start+usHeight-1); // 右边
    }
}

/**
 * @brief  Bresenham圆绘制（支持快速填充）
 * @param  usX_Center 圆心X坐标
 * @param  usY_Center 圆心Y坐标
 * @param  usRadius   半径（像素）
 * @param  ucFilled   填充模式（1=实心 0=边框）
 * @note   实心模式通过绘制直径线实现，时间复杂度O(N^2)
 */
void ILI9806G_DrawCircle(uint16_t usX_Center, uint16_t usY_Center,
                        uint16_t usRadius, uint8_t ucFilled)
{
    int16_t sCurrentX = 0;
    int16_t sCurrentY = usRadius;
    int16_t sError = 3 - (usRadius << 1); // 初始误差项
    
    while(sCurrentX <= sCurrentY) { // 八分之一圆弧绘制
        if(ucFilled) {
            // 直径线填充模式
            for(int16_t y = sCurrentX; y <= sCurrentY; y++) {
                /* 八象限对称点填充 */
                ILI9806G_SetPointPixel(usX_Center+sCurrentX, usY_Center+y); // 第一象限
                ILI9806G_SetPointPixel(usX_Center-sCurrentX, usY_Center+y); // 第二
                ILI9806G_SetPointPixel(usX_Center-y, usY_Center+sCurrentX); // 第三
                ILI9806G_SetPointPixel(usX_Center-y, usY_Center-sCurrentX); // 第四
                ILI9806G_SetPointPixel(usX_Center-sCurrentX, usY_Center-y); // 第五
                ILI9806G_SetPointPixel(usX_Center+sCurrentX, usY_Center-y); // 第六
                ILI9806G_SetPointPixel(usX_Center+y, usY_Center-sCurrentX); // 第七
                ILI9806G_SetPointPixel(usX_Center+y, usY_Center+sCurrentX); // 第八
            }
        } else {
            /* 八点对称绘制 */
            ILI9806G_SetPointPixel(usX_Center+sCurrentX, usY_Center+sCurrentY);
            ILI9806G_SetPointPixel(usX_Center-sCurrentX, usY_Center+sCurrentY);
            ILI9806G_SetPointPixel(usX_Center-sCurrentY, usY_Center+sCurrentX);
            ILI9806G_SetPointPixel(usX_Center-sCurrentY, usY_Center-sCurrentX);
            ILI9806G_SetPointPixel(usX_Center-sCurrentX, usY_Center-sCurrentY);
            ILI9806G_SetPointPixel(usX_Center+sCurrentX, usY_Center-sCurrentY);
            ILI9806G_SetPointPixel(usX_Center+sCurrentY, usY_Center-sCurrentX);
            ILI9806G_SetPointPixel(usX_Center+sCurrentY, usY_Center+sCurrentX);
        }

        // Bresenham误差更新
        sError += (sError < 0) ? (4*sCurrentX + 6) : (4*(sCurrentX-- - sCurrentY) + 10);
        sCurrentX++;
    }
}

/**
 * @brief  ASCII字符渲染引擎（支持可变尺寸字体）
 * @param  usX     字符左下角X坐标（基于当前扫描方向）
 * @param  usY     字符左下角Y坐标
 * @param  cChar   ASCII字符（32-126可显示）
 * @note   采用位平面绘制技术，支持8x8至32x32任意尺寸字体
 *          字模存储要求：宽度x高度必须是8的倍数
 */
void ILI9806G_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar)
{
    uint8_t byteCount, bitCount, fontLength;
    uint16_t ucRelativePosition;
    uint8_t *Pfont;

    // 字模表偏移计算（跳过ASCII 0-31控制字符）
    // 示例：'A' ASCII=65，65-32=33，对应字模表第33个字符
    ucRelativePosition = cChar - ' ';  // 等效cChar - 0x20

    // 计算字模数据长度（单位：字节）
    // 公式：(宽度像素 * 高度像素) / 8位/字节
    // 例如：8x16字体：(8*16)/8 = 16字节/字符
    fontLength = (LCD_Currentfonts->Width * LCD_Currentfonts->Height) / 8;

    // 计算字模数据起始地址
    // 字模表结构：连续存储每个字符的位图数据
    Pfont = (uint8_t*)&LCD_Currentfonts->table[ucRelativePosition * fontLength];

    // 设置硬件渲染窗口（一次性优化）
    // 窗口尺寸精确匹配字体尺寸，减少后续绘制命令
    ILI9806G_OpenWindow(usX, usY, 
                       LCD_Currentfonts->Width, 
                       LCD_Currentfonts->Height);
    
    // 进入显存写入模式（0x2C命令）
    // 后续Write_Data操作将直接写入显存
    ILI9806G_Write_Cmd(CMD_SetPixel);

    // 双层循环结构：字节循环->位循环
    // 时间复杂度：O(n) n=字体像素总数/8
    for (byteCount = 0; byteCount < fontLength; byteCount++) {
        // 处理单个字节的8个位（MSB优先）
        // 示例：字节0x80对应最左边像素
        for (bitCount = 0; bitCount < 8; bitCount++) {
            // 位掩码检测（从高位到低位）
            // 0x80>>bitCount 生成序列：0x80,0x40,0x20...0x01
            if (Pfont[byteCount] & (0x80 >> bitCount)) {
                // 前景色像素写入（硬件自动递增地址）
                ILI9806G_Write_Data(CurrentTextColor);
            } else {
                // 背景色像素写入（透明绘制优化）
                ILI9806G_Write_Data(CurrentBackColor);
            }
            // 注：像素填充顺序由LCD扫描模式决定
        }
        // 硬件特性：自动换行由OpenWindow参数保证
    }
}

/**
 * @brief  流式文本布局引擎（行号基准模式）
 * @param  line   起始垂直位置（基于LINE宏计算的行号）
 * @param  pStr   ASCII字符串指针（需UTF-8编码）
 * @note   智能换行算法核心组件，时间复杂度O(n)
 *         支持软换行与循环滚动两种显示模式
 */
void ILI9806G_DispStringLine_EN(uint16_t line, char *pStr)
{
    // 初始化光标X坐标为窗口起始X位置
    uint16_t cursorX = ILI9806G_DispWindow_X_Star;
    
    // 计算实际显示区域的最大坐标（考虑窗口偏移）
    // maxX = 窗口X起点 + LCD_X方向有效像素数
    const uint16_t maxX = LCD_X_LENGTH + ILI9806G_DispWindow_X_Star;
    
    // 计算实际显示区域的最大Y坐标（考虑窗口偏移）
    // maxY = 窗口Y起点 + LCD_Y方向有效像素数
    const uint16_t maxY = LCD_Y_LENGTH + ILI9806G_DispWindow_Y_Star;

    // 遍历字符串直至空终止符
    while (*pStr) {
        /* 水平边界检测策略 */
        // 预测式检测：计算若渲染当前字符后是否越界
        // 算法公式：当前X + 字符宽度 > 最大X坐标
        if ((cursorX + LCD_Currentfonts->Width) > maxX) {
            cursorX = ILI9806G_DispWindow_X_Star;  // 复位到窗口左边界
            line += LCD_Currentfonts->Height;       // 垂直位置下移字体高度（软换行）
        }

        /* 垂直边界检测策略 */
        // 计算换行后的新行位置是否超出显示区域
        // 算法公式：当前行号 + 字体高度 > 最大Y坐标
        if ((line + LCD_Currentfonts->Height) > maxY) {
            cursorX = ILI9806G_DispWindow_X_Star;  // 复位到窗口左上角
            line = ILI9806G_DispWindow_Y_Star;     // 垂直位置复位（循环滚动模式）
        }

        // 调用字符渲染引擎绘制当前字符
        // 使用后缀++操作符实现指针递增与绘制的原子操作
        ILI9806G_DispChar_EN(cursorX, line, *pStr++);

        // 更新水平光标位置（右移一个字符宽度）
        // 字间距=字体宽度，支持等宽/变宽字体自动适配
        cursorX += LCD_Currentfonts->Width;

        // 性能优化点：可在此添加批量传输模式判断
    }
}

/**
 * @brief  横向流式文本布局引擎（支持自动换行）
 * @param  usX  起始X坐标(基于当前显示窗口配置)
 * @param  usY  起始Y坐标
 * @param  pStr ASCII字符串指针
 * @note   采用贪婪算法进行布局，时间复杂度O(n)
 *         换行策略：优先保持单词完整，当前实现为暴力换行
 */
void ILI9806G_DispString_EN(uint16_t usX, uint16_t usY, char *pStr)
{
    // 遍历字符串直至终止符
    while (*pStr != '\0') {
        // 行内溢出检测：当前字符宽度是否超出显示区域
        // LCD_X_LENGTH = 显示区域宽度 - 1
        if ((usX - ILI9806G_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH) {
            usX = ILI9806G_DispWindow_X_Star;  // X坐标复位到窗口起点
            usY += LCD_Currentfonts->Height;    // Y坐标下移字体高度（软换行）
        }
        
        // 垂直溢出检测：换行后是否超出显示区域底部
        // 采用循环策略而非截断，适合滚动显示场景
        if ((usY - ILI9806G_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH) {
            usX = ILI9806G_DispWindow_X_Star;  // 复位到窗口左上角
            usY = ILI9806G_DispWindow_Y_Star;  // 实现循环显示效果
        }
        
        // 调用底层字符渲染引擎
        ILI9806G_DispChar_EN(usX, usY, *pStr);
        
        pStr++;  // 移动字符串指针
        usX += LCD_Currentfonts->Width;  // X坐标递增字体宽度
        
        // 注意：未处理TAB等特殊字符，需上层保证输入纯净
    }
}

/**
 * @brief  纵向文本布局引擎（适用于状态栏等场景）
 * @param  usX  起始X坐标
 * @param  usY  起始Y坐标
 * @param  pStr ASCII字符串指针
 * @note   垂直布局算法，时间复杂度O(n)
 *         换列策略：当垂直空间不足时向右换列
 */
void ILI9806G_DispString_EN_YDir(uint16_t usX, uint16_t usY, char *pStr)
{    
    while (*pStr != '\0') {
        // 列内溢出检测：当前字符高度是否超出显示区域
        if ((usY - ILI9806G_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH) {
            usY = ILI9806G_DispWindow_Y_Star;  // Y坐标复位到顶部
            usX += LCD_Currentfonts->Width;     // X坐标右移字体宽度（换列）
        }
        
        // 水平溢出检测：换列后是否超出显示区域右侧
        // 采用循环策略，适合信息看板场景
        if ((usX - ILI9806G_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH) {
            usX = ILI9806G_DispWindow_X_Star;  // 复位到窗口左上角
            usY = ILI9806G_DispWindow_Y_Star;  // 开启新循环周期
        }
        
        ILI9806G_DispChar_EN(usX, usY, *pStr);
        pStr++;
        usY += LCD_Currentfonts->Height;  // Y坐标递增字体高度
        
        // 注意：垂直布局时字符旋转方向取决于LCD扫描模式
    }
}

/**
 * @brief  GB2312汉字渲染引擎（32x32点阵）
 * @param  usX     起始X坐标
 * @param  usY     起始Y坐标
 * @param  usChar  GB2312编码汉字（双字节）
 * @note   使用4字节/行的压缩存储格式
 *         显存写入优化：采用窗口模式减少命令发送次数
 */
void ILI9806G_DispChar_CH(uint16_t usX, uint16_t usY, uint16_t usChar)
{
    uint8_t rowCount, bitCount;
    uint32_t usTemp;  // 32位缓冲区用于存储每行点阵数据
    
    // 设置32x32像素的渲染窗口（硬件加速）
    ILI9806G_OpenWindow(usX, usY, WIDTH_CH_CHAR, HEIGHT_CH_CHAR);
    
    // 发送像素数据写入命令（0x2C）
    ILI9806G_Write_Cmd(CMD_SetPixel);
    
    // 获取GBK字模数据到全局缓冲区（避免栈溢出）
    // ucBuffer需要预定义为全局数组：uint8_t ucBuffer[128]
    GetGBKCode(ucBuffer, usChar);  // 字库查询函数
    
    // 逐行渲染32x32点阵
    for (rowCount = 0; rowCount < HEIGHT_CH_CHAR; rowCount++) {
        // 组合4个字节为32位数据（每行32点）
        // 存储格式：Big-endian，MSB对应最左侧像素
        usTemp = (ucBuffer[rowCount*4] << 24)   |  // 第一个字节占高8位
                 (ucBuffer[rowCount*4+1] << 16) |
                 (ucBuffer[rowCount*4+2] << 8)  |
                  ucBuffer[rowCount*4+3];        // 第四个字节占低8位
        
        // 逐位解析像素数据（从最高位到最低位）
        for (bitCount = 0; bitCount < WIDTH_CH_CHAR; bitCount++) {
            // 使用位掩码检测每个bit的状态（1=前景色，0=背景色）
            // 0x80000000对应第0位像素（最左侧）
            if (usTemp & (0x80000000 >> bitCount)) {
                ILI9806G_Write_Data(CurrentTextColor);  // 前景色写入
            } else {
                ILI9806G_Write_Data(CurrentBackColor);  // 背景色写入
            }
        }
        
        // 优化点：可改为批量写入模式提升速度
    }
}

/**
 * @brief  中文流式文本布局引擎（GBK编码支持）
 * @param  usX  起始X坐标（基于当前显示窗口配置）
 * @param  usY  起始Y坐标
 * @param  pStr GBK编码字符串指针（双字节字符）
 * @note   自动处理汉字双字节解析，支持循环滚动显示
 *         每个汉字固定尺寸：WIDTH_CH_CHAR x HEIGHT_CH_CHAR
 */
void ILI9806G_DispString_CH(uint16_t usX, uint16_t usY, char *pStr)
{	
    uint16_t usCh;  // GBK编码缓冲区
    
    // 遍历字符串直至空终止符（双字节步进）
    while(*pStr != '\0') {		
        // 水平边界检测：当前X位置+汉字宽度是否超出显示区域
        if((usX - ILI9806G_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH) {
            usX = ILI9806G_DispWindow_X_Star;  // 复位到窗口左边界
            usY += HEIGHT_CH_CHAR;              // 下移一个汉字高度（垂直换行）
        }
        
        // 垂直边界检测：换行后是否超出显示区域底部
        if((usY - ILI9806G_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH) {
            usX = ILI9806G_DispWindow_X_Star;  // 复位到窗口左上角
            usY = ILI9806G_DispWindow_Y_Star;  // 垂直位置复位（循环滚动）
        }	
        
        // 提取GBK编码（小端序转大端序）
        // 示例：字符串"中"的GBK编码为0xD6D0，存储为D0 D6时需要转换
        usCh = *((uint16_t *)pStr);            // 读取双字节
        usCh = (usCh << 8) | (usCh >> 8);     // 字节序转换
        
        // 渲染当前汉字
        ILI9806G_DispChar_CH(usX, usY, usCh);
        
        usX += WIDTH_CH_CHAR;  // 水平移动汉字宽度
        pStr += 2;             // 跳过双字节编码
        
        // 注意：要求字符串长度必须为偶数（无单独半角字符）
    }	   
}

/**
 * @brief  混合文字流式布局引擎（行号基准模式）
 * @param  line 起始垂直位置（基于LINE宏计算的行号）
 * @param  pStr 中英混合字符串指针（ASCII+GBK）
 * @note   智能字符编码识别，支持动态布局切换
 *         字体约束：英文字体高度必须与汉字高度对齐
 */
void ILI9806G_DispStringLine_EN_CH(uint16_t line, char *pStr)
{
    uint16_t usCh;
    uint16_t usX = 0;  // 相对窗口起始X坐标的偏移量
    
    while(*pStr != '\0') {
        // ASCII字符处理（单字节，值<=126）
        if(*pStr <= 126) {	            
            // 水平边界检测：英文宽度溢出判断
            if((usX - ILI9806G_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH) {
                usX = ILI9806G_DispWindow_X_Star;
                line += LCD_Currentfonts->Height;  // 按英文字体高度换行
            }
            
            // 垂直边界检测：换行后是否超出底部
            if((line - ILI9806G_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH) {
                usX = ILI9806G_DispWindow_X_Star;
                line = ILI9806G_DispWindow_Y_Star;  // 垂直位置复位
            }			
        
            // 渲染英文字符（自动处理字模）
            ILI9806G_DispChar_EN(usX, line, *pStr);
            
            usX += LCD_Currentfonts->Width;  // 移动英文字宽
            pStr++;                         // 单字节步进
        } 
        // 汉字字符处理（双字节，值>126） 
        else {	                            
            // 水平边界检测：汉字宽度溢出判断
            if((usX - ILI9806G_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH) {
                usX = ILI9806G_DispWindow_X_Star;
                line += HEIGHT_CH_CHAR;  // 按汉字高度换行
            }
            
            // 垂直边界检测（使用汉字高度参数）
            if((line - ILI9806G_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH) {
                usX = ILI9806G_DispWindow_X_Star;
                line = ILI9806G_DispWindow_Y_Star;  // 垂直复位
            }	
            
            // 提取并转换GBK编码（小端序转大端序）
            usCh = *((uint16_t *)pStr);	
            usCh = (usCh << 8) | (usCh >> 8);  // 等效交换高低字节
            
            // 渲染汉字字符
            ILI9806G_DispChar_CH(usX, line, usCh);
            
            usX += WIDTH_CH_CHAR;  // 移动汉字宽度
            pStr += 2;              // 跳过双字节编码
            
            // 安全机制：需确保字符串不会出现奇数长度
        }		
    }	
}

/**
 * @brief 在 ILI9806G 显示器上混合显示中英文字符串（横向排列布局）
 * @param usX : 基于当前扫描方向的起始X坐标（单位：像素）
 * @param usY : 基于当前扫描方向的起始Y坐标（单位：像素）
 * @param pStr : 待显示字符串指针（支持GB2312编码汉字和ASCII字符混合）
 * @note 使用前需通过LCD_SetXXX系列函数设置颜色参数
 * @note 自动换行逻辑：横向排列时X轴满换行到下一行，Y轴满则重置到起始位置
 * @retval 无
 */
void ILI9806G_DispString_EN_CH(uint16_t usX, uint16_t usY, char *pStr)
{
    uint16_t usCh;  // 汉字双字节存储缓冲区
    
    // 字符串遍历主循环（支持中英文混排）
    while (*pStr != '\0')  // 逐字节解析直到空终止符
    {
        // ASCII字符处理分支（单字节）
        if (*pStr <= 126)  // 判断标准ASCII字符范围（0-126）
        {
            // 横向溢出检测：当前字符宽度是否超出显示区域右边界
            if ((usX - ILI9806G_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
            {
                usX = ILI9806G_DispWindow_X_Star;  // X坐标复位到区域起始
                usY += LCD_Currentfonts->Height;   // Y坐标下移一行（英文字体高度）
            }
            
            // 纵向溢出检测：当前字符高度是否超出显示区域下边界
            if ((usY - ILI9806G_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
            {
                usX = ILI9806G_DispWindow_X_Star;  // X坐标复位
                usY = ILI9806G_DispWindow_Y_Star;  // Y坐标复位（实现循环显示）
            }
            
            // 绘制英文字符（调用底层字符绘制函数）
            ILI9806G_DispChar_EN(usX, usY, *pStr);
            
            usX += LCD_Currentfonts->Width;  // X坐标右移字符宽度
            pStr++;  // 指针递增1字节（ASCII单字节）
        }
        // 汉字处理分支（双字节）
        else
        {
            // 横向溢出检测：汉字宽度是否超出显示区域
            if ((usX - ILI9806G_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH)
            {
                usX = ILI9806G_DispWindow_X_Star;  // X复位
                usY += HEIGHT_CH_CHAR;  // Y下移汉字高度（通常大于英文字体）
            }
            
            // 纵向溢出检测：汉字高度是否超出显示区域
            if ((usY - ILI9806G_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH)
            {
                usX = ILI9806G_DispWindow_X_Star;
                usY = ILI9806G_DispWindow_Y_Star;
            }
            
            // 汉字编码处理（GB2312编码需处理大小端）
            usCh = *(uint16_t *)pStr;         // 直接读取两个字节
            usCh = (usCh << 8) + (usCh >> 8); // 字节序转换（小端转大端）
            
            // 绘制汉字字符（调用汉字专用绘制函数）
            ILI9806G_DispChar_CH(usX, usY, usCh);
            
            usX += WIDTH_CH_CHAR;  // X坐标右移汉字固定宽度
            pStr += 2;  // 指针递增2字节（GB2312双字节）
        }
    }
}

/**
 * @brief 在 ILI9806G 显示器上垂直排列显示中英文字符串（纵向布局）
 * @param usX : 基于当前扫描方向的起始X坐标
 * @param usY : 基于当前扫描方向的起始Y坐标
 * @param pStr : 待显示字符串指针
 * @note 换行逻辑：纵向排列时Y轴满换列，X轴满则重置到起始位置
 * @note 统一使用汉字尺寸计算布局，确保中英混排对齐
 */
void ILI9806G_DispString_EN_CH_YDir(uint16_t usX, uint16_t usY, char *pStr)
{
    uint16_t usCh;  // 汉字编码存储
    
    while (*pStr != '\0')
    {
        // 纵向溢出检测（使用汉字高度作为行距基准）
        if ((usY - ILI9806G_DispWindow_Y_Star + HEIGHT_CH_CHAR) > LCD_Y_LENGTH)
        {
            usY = ILI9806G_DispWindow_Y_Star;  // Y复位
            usX += WIDTH_CH_CHAR;  // X右移汉字宽度（换列）
        }
        
        // 横向溢出检测（防止超出显示区域右侧）
        if ((usX - ILI9806G_DispWindow_X_Star + WIDTH_CH_CHAR) > LCD_X_LENGTH)
        {
            usX = ILI9806G_DispWindow_X_Star;  // X复位
            usY = ILI9806G_DispWindow_Y_Star;  // Y复位（区域循环）
        }
        
        // ASCII字符处理（纵向布局）
        if (*pStr <= 126)
        {
            ILI9806G_DispChar_EN(usX, usY, *pStr);  // 绘制英文字符
            pStr++;  // 单字节递增
            usY += HEIGHT_CH_CHAR;  // Y下移汉字高度（保持行距统一）
        }
        // 汉字处理（纵向布局）
        else
        {
            usCh = *(uint16_t *)pStr;         // 获取双字节
            usCh = (usCh << 8) + (usCh >> 8); // 大小端转换
            
            ILI9806G_DispChar_CH(usX, usY, usCh);  // 绘制汉字
            
            usY += HEIGHT_CH_CHAR;  // Y下移汉字高度
            pStr += 2;  // 双字节递增
        }
    }
}

/*********************** 字体缩放处理模块 ****************************/
/*
 * 功能说明：本模块定义字体缩放所需的缓冲区及相关配置参数
 * 设计要点：
 * 1. 采用双缓冲机制：zoomBuff用于存放最终缩放结果，zoomTempBuff用于临时处理
 * 2. 支持多种缩放算法：通过配置缩放因子实现不同倍率的字体缩放
 * 3. 内存优化设计：通过合理分配缓冲区大小平衡内存使用和性能
 */

/* 最大缩放缓冲区尺寸配置（128x128像素点阵） 
 * 计算方式：128像素宽 × 128像素高 × 1字节/8像素 = 2048字节
 * 实际分配16384字节以支持扩展需求，提供8倍缩放余量 */
#define ZOOMMAXBUFF 16384

/* 主缩放缓冲区（按字节寻址的位映射存储）
 * 存储格式：每个字节表示8个水平连续像素点（MSB在左，LSB在右）
 * 示例：0xF0表示连续4个白点(1)接4个黑点(0) 
 * 初始化：全零状态表示空白区域 */
uint8_t zoomBuff[ZOOMMAXBUFF] = {0};	// 最终缩放结果缓存（支持最大128×128点阵）

/* 临时处理缓冲区（用于缩放过程中的中间计算）
 * 功能：存储临时位操作结果，避免直接修改主缓冲区
 * 容量设计：1024字节支持处理32×32像素的临时操作
 * 典型应用：行缓存、列缓存、矩阵转置等中间过程 */
uint8_t zoomTempBuff[1024] = {0};  // 缩放算法临时工作缓冲区
/**
 * @brief 字体缩放处理核心算法
 * @param in_width 原始字符宽度（像素）
 * @param in_heig 原始字符高度（像素）
 * @param out_width 目标字符宽度（必须≤128）
 * @param out_heig 目标字符高度（必须≤128）
 * @param in_ptr 源字模指针（1bit/像素紧凑存储）
 * @param out_ptr 输出缓冲区指针（实际未使用，改用了全局zoomBuff）
 * @param en_cn 字符类型标记（0:英文，1:中文）
 * @note 算法特点：
 * 1. 采用定点数缩放算法（16位小数精度）
 * 2. 支持双向非整数倍缩放
 * 3. 输入输出缓冲区分离设计
 */
void ILI9806G_zoomChar(uint16_t in_width, uint16_t in_heig,
                       uint16_t out_width, uint16_t out_heig,
                       uint8_t *in_ptr, uint8_t *out_ptr, uint8_t en_cn)
{
    uint8_t *pts, *ots;
    /* 计算缩放比例因子（定点数运算优化）
     * 公式：缩放步长 = (原始尺寸 << 16) / 目标尺寸
     * 示例：原始48px缩放至128px时，步长 = (48<<16)/128 = 0x6000 */
    unsigned int xrIntFloat_16 = (in_width << 16)/out_width + 1; // X轴缩放步长
    unsigned int yrIntFloat_16 = (in_heig << 16)/out_heig + 1;  // Y轴缩放步长
    
    unsigned int srcy_16 = 0; // Y轴源坐标（高16位为小数部分）
    unsigned int y, x;        // 目标坐标计数器
    uint8_t *pSrcLine;        // 当前处理行指针
    
    uint16_t byteCount, bitCount; // 位操作计数器
    
    // 参数安全校验（防御性编程）
    if(in_width > 48) return;               // 限制最大输入宽度48px
    if(in_width * in_heig == 0) return;      // 防止零尺寸输入
    if(in_width * in_heig > 48*48 ) return;  // 限制最大输入面积48x48
    
    if(out_width * out_heig == 0) return;    // 防止零尺寸输出
    if(out_width * out_heig >= ZOOMMAXBUFF ) return; // 防止缓冲区溢出
    
    pts = (uint8_t*)&zoomTempBuff; // 指向临时缓冲区的操作指针
    
    /* 位到字节的转换（1bit/px → 8bit/px）
     * 目的：将紧凑存储的字模数据转换为易处理的字节格式
     * 转换规则：
     * - 源数据每个bit表示一个像素（0:背景，1:前景）
     * - 转换后每个字节表示一个像素（0x00:背景，0x01:前景）*/
    if(en_cn == 0x00) // 英文字符处理
    {
        // 遍历所有字节（每个字节包含8个水平像素）
        for(byteCount=0; byteCount<in_heig*in_width/8; byteCount++)
        {
            // 分解字节中的每个bit（MSB优先处理）
            for(bitCount=0; bitCount<8; bitCount++)
            {                        
                // 位提取操作：检测当前bit是否为1
                // 0x80>>bitCount 生成从高位到低位的掩码
                *pts++ = (in_ptr[byteCount] & (0x80 >> bitCount)) ? 1 : 0;
            }
        }                
    }
    else // 中文字符处理（当前实现与英文相同）
    {            
        for(byteCount=0; byteCount<in_heig*in_width/8; byteCount++)
        {
            for(bitCount=0; bitCount<8; bitCount++)
            {                        
                *pts++ = (in_ptr[byteCount] & (0x80 >> bitCount)) ? 1 : 0;
            }
        }        
    }

    // 缩放处理核心算法（最近邻插值法）
    pts = (uint8_t*)&zoomTempBuff; // 重置源指针
    ots = (uint8_t*)&zoomBuff;     // 指向输出缓冲
    
    for (y=0; y<out_heig; y++)     // 垂直方向遍历
    {
        unsigned int srcx_16 = 0;  // X轴源坐标（定点数）
        // 计算当前源行：srcy_16高16位为整数部分
        pSrcLine = pts + in_width*(srcy_16 >> 16);
        
        for (x=0; x<out_width; x++) // 水平方向遍历
        {
            // 最近邻采样：取整数部分作为源坐标
            ots[x] = pSrcLine[srcx_16 >> 16];
            // 更新源X坐标（定点数加法）
            srcx_16 += xrIntFloat_16;
        }
        // 更新源Y坐标（定点数加法）
        srcy_16 += yrIntFloat_16;
        ots += out_width; // 移动到下一行
    }
    
    /* 输出处理（注意设计缺陷）
     * 问题：函数参数out_ptr未实际使用，直接操作了全局变量zoomBuff
     * 临时解决方案：强制使用全局缓冲区指针 */
    out_ptr = (uint8_t*)&zoomBuff;  // 此赋值实际上不生效（参数传递问题）
    
    /* 抑制编译器警告的无效操作 */
    out_ptr++; 
}

/**
 * @brief 高精度字符显示函数（支持缩放后字模）
 * @param usX 显示起始X坐标（像素）
 * @param usY 显示起始Y坐标（像素）
 * @param Font_width 字符实际显示宽度
 * @param Font_Height 字符实际显示高度
 * @param c 字模数据指针（需预先缩放处理）
 * @param DrawModel 显示模式（0:正常，1:反色）
 * @note 工作原理：
 * 1. 设置指定大小的显示窗口
 * 2. 按像素逐个写入颜色数据
 * 3. 自动处理横向换行（依赖ILI9806G的窗口模式）
 */
void ILI9806G_DrawChar_Ex(uint16_t usX, uint16_t usY,
                          uint16_t Font_width, uint16_t Font_Height,
                          uint8_t *c, uint16_t DrawModel)
{
    uint32_t index = 0, counter = 0;

    // 设置显示区域（激活LCD的窗口模式）
    ILI9806G_OpenWindow(usX, usY, Font_width, Font_Height);
    
    // 发送像素数据写入命令
    ILI9806G_Write_Cmd(CMD_SetPixel);        
    
    // 逐行扫描写入数据
    for (index = 0; index < Font_Height; index++) // 行循环
    {
        // 行内像素处理（注意：窗口模式自动处理换行）
        for (counter = 0; counter < Font_width; counter++) // 列循环
        {
            /* 像素渲染决策：
             * - 字模数据为0x01表示绘制前景色
             * - 字模数据为0x00表示绘制背景色
             * - DrawModel=1时反转显示逻辑 */
            if (*c++ == DrawModel)
                ILI9806G_Write_Data(CurrentBackColor); // 绘制背景色
            else
                ILI9806G_Write_Data(CurrentTextColor); // 绘制前景色
        }    
    }
}

/**
 * @brief 自适应缩放字符串显示函数（横向布局）
 * @param x 起始X坐标（基于当前扫描方向）
 * @param y 起始Y坐标（基于当前扫描方向）
 * @param Font_width 中文字符宽度（英文字符实际宽度为该值一半）
 * @param Font_Height 字符显示高度（需为偶数）
 * @param ptr 待显示字符串指针（支持GB2312编码）
 * @param DrawModel 显示模式（0:正常，1:反色）
 * @note 功能特性：
 * 1. 自动处理中英文混排
 * 2. 智能换行处理（基于显示区域边界）
 * 3. 动态字符宽度调整（中文全宽/英文半宽）
 */
void ILI9806G_DisplayStringEx(uint16_t x, uint16_t y,
                              uint16_t Font_width, uint16_t Font_Height,
                              uint8_t *ptr, uint16_t DrawModel)
{
    uint16_t Charwidth = Font_width; // 当前字符显示宽度（动态调整）
    uint8_t *psr;                    // 缩放结果指针（实际未初始化使用）
    uint8_t Ascii;                   // ASCII字符缓存
    uint16_t usCh;                   // 汉字编码缓存
    
    // 主字符串解析循环
    while (*ptr != '\0')
    {
        /**** 智能换行处理 ****/
        // X轴溢出检测：当前字符宽度是否超出显示区域
        if ((x - ILI9806G_DispWindow_X_Star + Charwidth) > LCD_X_LENGTH)
        {
            x = ILI9806G_DispWindow_X_Star; // X复位到区域起始
            y += Font_Height;                // Y下移一行
        }
        
        // Y轴溢出检测：当前字符高度是否超出显示区域
        if ((y - ILI9806G_DispWindow_Y_Star + Font_Height) > LCD_Y_LENGTH)
        {
            x = ILI9806G_DispWindow_X_Star; // XY复位实现循环显示
            y = ILI9806G_DispWindow_Y_Star;
        }    
        
        // 汉字处理分支（首字节>0x80为GB2312编码特征）
        if (*ptr > 0x80)
        {            
            Charwidth = Font_width; // 中文字符全宽显示
            // 组合双字节汉字编码（大小端转换）
            usCh = *(uint16_t *)ptr;
            usCh = (usCh << 8) + (usCh >> 8);
            
            GetGBKCode(ucBuffer, usCh);  // 获取原始字模数据（32x32像素）
            
            // 执行字体缩放（从32x32缩放到目标尺寸）
            ILI9806G_zoomChar(WIDTH_CH_CHAR, HEIGHT_CH_CHAR,
                             Charwidth, Font_Height,
                             (uint8_t *)&ucBuffer, psr, 1);
            
            // 显示缩放后的字符
            ILI9806G_DrawChar_Ex(x, y, Charwidth, Font_Height,
                                (uint8_t*)&zoomBuff, DrawModel);
            
            x += Charwidth;  // X右移完整字符宽度
            ptr += 2;        // 跳过双字节汉字
        }
        // 英文字符处理分支
        else
        {
            Charwidth = Font_width / 2; // 英文字符半宽显示
            Ascii = *ptr - 32;          // ASCII码转换（字模表偏移）
            
            // 缩放英文字模（从16x32缩放到目标尺寸）
            ILI9806G_zoomChar(16, 32, Charwidth, Font_Height,
                             (uint8_t *)&Font16x32.table[Ascii * Font16x32.Height*Font16x32.Width/8],
                             psr, 0);
            
            // 显示缩放后的字符
            ILI9806G_DrawChar_Ex(x, y, Charwidth, Font_Height,
                                (uint8_t*)&zoomBuff, DrawModel);
            
            x += Charwidth;  // X右移半宽
            ptr++;           // 前进单字节
        }
    }
}

/**
 * @brief 垂直布局缩放字符串显示函数（纵向排列）
 * @param x 起始X坐标（垂直方向为列基准）
 * @param y 起始Y坐标（垂直方向为行基准）
 * @param Font_width 字符显示宽度（实际作为列高度）
 * @param Font_Height 字符显示高度（实际作为行宽度）
 * @param ptr 待显示字符串指针
 * @param DrawModel 显示模式（0:正常，1:反色）
 * @note 设计特点：
 * 1. 垂直方向自动换列
 * 2. 统一使用汉字尺寸计算布局
 * 3. 坐标轴转换逻辑（X/Y角色互换）
 */
void ILI9806G_DisplayStringEx_YDir(uint16_t x, uint16_t y,
                                   uint16_t Font_width, uint16_t Font_Height,
                                   uint8_t *ptr, uint16_t DrawModel)
{
    uint16_t Charwidth = Font_width; 
    uint8_t *psr;
    uint8_t Ascii;
    uint16_t usCh;
    uint8_t ucBuffer [ WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8 ]; // 局部字模缓冲区
    
    while (*ptr != '\0')
    {            
        /**** 垂直换列处理 ****/
        // Y轴溢出检测（垂直方向换列逻辑）
        if ((y - ILI9806G_DispWindow_X_Star + Font_width) > LCD_X_LENGTH)
        {
            y = ILI9806G_DispWindow_X_Star; // Y复位
            x += Font_width;                // X右移换列
        }
        
        // X轴溢出检测（垂直方向换行逻辑）
        if ((x - ILI9806G_DispWindow_Y_Star + Font_Height) > LCD_Y_LENGTH)
        {
            y = ILI9806G_DispWindow_X_Star; // XY复位循环
            x = ILI9806G_DispWindow_Y_Star;
        }    
        
        // 汉字处理分支
        if (*ptr > 0x80)
        {            
            Charwidth = Font_width;
            usCh = *(uint16_t *)ptr;        // 获取双字节
            usCh = (usCh << 8) + (usCh >> 8); // 大小端转换
            
            GetGBKCode(ucBuffer, usCh);     // 获取16x16原始字模
            
            // 执行垂直方向缩放
            ILI9806G_zoomChar(WIDTH_CH_CHAR, HEIGHT_CH_CHAR,
                             Charwidth, Font_Height,
                             (uint8_t *)&ucBuffer, psr, 1);
            
            // 垂直显示字符
            ILI9806G_DrawChar_Ex(x, y, Charwidth, Font_Height,
                                (uint8_t*)&zoomBuff, DrawModel);
            
            y += Font_Height;  // Y下移字符高度（垂直方向）
            ptr += 2;           // 跳过汉字双字节
        }
        // 英文字符处理
        else
        {
            Charwidth = Font_width / 2;
            Ascii = *ptr - 32; // 字模表索引计算
            
            // 缩放英文字模（16x24原始尺寸）
            ILI9806G_zoomChar(16, 24, Charwidth, Font_Height,
                             (uint8_t *)&Font16x32.table[Ascii * Font16x32.Height*Font16x32.Width/8],
                             psr, 0);
            
            // 垂直显示字符
            ILI9806G_DrawChar_Ex(x, y, Charwidth, Font_Height,
                                (uint8_t*)&zoomBuff, DrawModel);
            
            y += Font_Height;  // Y下移字符高度
            ptr++;             // 前进单字节
        }
    }
}

/*********************** 字体管理模块 ****************************/

/**
 * @brief 设置LCD显示字体
 * @param fonts 字体结构体指针，可选值：
 *     - &Font24x32: 24x32像素字体
 *     - &Font16x24: 16x24像素字体 
 *     - &Font8x16: 8x16像素字体
 * @note 字体结构体需包含字模数据及尺寸信息
 */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts; // 更新全局字体指针
}

/**
 * @brief 获取当前使用字体
 * @retval 返回当前字体结构体指针
 */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts; // 返回全局字体指针
}

/*********************** 颜色管理模块 ****************************/

/**
 * @brief 设置双色模式参数（前景色+背景色）
 * @param TextColor 文本颜色（RGB565格式）
 * @param BackColor 背景颜色（RGB565格式）
 * @note RGB565格式说明：
 *     - 高5位红色（0b1111100000000000）
 *     - 中6位绿色（0b0000011111100000） 
 *     - 低5位蓝色（0b0000000000011111）
 */
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) 
{
  CurrentTextColor = TextColor;  // 更新全局前景色
  CurrentBackColor = BackColor;  // 更新全局背景色
}

/**
 * @brief 获取当前颜色配置
 * @param TextColor 用于接收前景色的指针
 * @param BackColor 用于接收背景色的指针
 */
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
  *TextColor = CurrentTextColor; // 输出当前前景色
  *BackColor = CurrentBackColor; // 输出当前背景色
}

/**
 * @brief 设置文本颜色（单色模式）
 * @param Color RGB565格式颜色值
 */
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color; // 仅更新前景色
}

/**
 * @brief 设置背景颜色（单色模式） 
 * @param Color RGB565格式颜色值
 */
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color; // 仅更新背景色
}

/*********************** 显示控制模块 ****************************/

/**
 * @brief 清除指定文本行
 * @param Line 行号（使用LINE(x)宏指定）
 * @note LINE宏根据当前字体高度计算物理坐标：
 *     LINE(x) = x * 字体高度（像素）
 */
void ILI9806G_ClearLine(uint16_t Line)
{
  // 清空指定行区域（黑色填充）
  ILI9806G_Clear(0, Line, LCD_X_LENGTH, ((sFONT *)LCD_GetFont())->Height);
}
/**
 * @brief LCD全功能演示测试函数
 * @note 本函数依次演示以下功能：
 * 1. 多行中英文字符串显示
 * 2. 动态变量更新显示（需配合sprintf）
 * 3. 基本图形绘制（直线/矩形/圆形）
 * 4. 多颜色模式切换
 * 5. 局部/全局清屏操作
 * 演示流程分四个阶段，每个阶段后保留2秒显示时间
 */
void LCD_Test(void)
{
    /* 测试计数器（静态变量保持状态） */
    static uint8_t testCNT = 0;  // 演示计数器，用于变量显示示例
    testCNT++;                   // 每次调用递增
    
    /************** 第一阶段：基础字符串显示 **************/
    LCD_SetFont(&Font16x32);       // 使用16x32像素字体
    LCD_SetColors(RED, BLACK);     // 设置红字黑底
    
    // 显示设备参数信息（LINE宏根据字体高度计算行位置）
    ILI9806G_DispStringLine_EN_CH(LINE(0), "4.3寸LCD参数：");         // 第0行
    ILI9806G_DispStringLine_EN_CH(LINE(2), "分辨率：480x800 px");    // 第2行（跳过1行）
    ILI9806G_DispStringLine_EN_CH(LINE(3), "ILI9806G液晶驱动");       // 第3行
    ILI9806G_DispStringLine_EN_CH(LINE(4), "GT917S触摸屏驱动");       // 第4行

    /************** 第二阶段：动态变量显示（注释状态） **************/
    LCD_SetTextColor(GREEN);  // 设置绿色文本
    ILI9806G_ClearLine(LINE(7));  // 清除第7行内容
    
    // 变量显示示例（需启用下方代码）
    // char dispBuff[100];
    // sprintf(dispBuff,"显示变量计数 : %d ",testCNT);  // 格式化字符串
    // ILI9806G_DispStringLine_EN(LINE(7), dispBuff);    // 显示在第7行

    /************** 第三阶段：直线绘制演示 **************/
    ILI9806G_ClearLine(LINE(7));  // 准备绘制区域
    LCD_SetTextColor(BLUE);       // 设置蓝色提示文字
    ILI9806G_DispStringLine_EN_CH(LINE(7), "画直线:");  // 第7行显示标题
    
    // 绘制红色直线组（坐标格式：起点X,Y 终点X,Y）
    LCD_SetTextColor(RED);
    ILI9806G_DrawLine(50, 270, 420, 275);  // 近水平线
    ILI9806G_DrawLine(50, 300, 420, 375);  // 右下斜线
    
    // 绘制绿色直线组
    LCD_SetTextColor(GREEN);
    ILI9806G_DrawLine(50, 370, 420, 475);  
    ILI9806G_DrawLine(50, 400, 420, 475);  // 不同斜率直线
    
    // 绘制蓝色直线组
    LCD_SetTextColor(BLUE);
    ILI9806G_DrawLine(50, 420, 420, 325);  // 右上斜线
    ILI9806G_DrawLine(50, 450, 420, 395);
    
    Delay(0x2FFFFFF);  // 延时约2秒（基于CPU频率）
    ILI9806G_Clear(0, 32*7, LCD_X_LENGTH, LCD_Y_LENGTH-32*7);  // 清除第7行以下区域

    /************** 第四阶段：矩形绘制演示 **************/
    ILI9806G_ClearLine(LINE(7));  // 重置绘制区域
    LCD_SetTextColor(BLUE);
    ILI9806G_DispStringLine_EN_CH(LINE(7), "画矩形:");  // 更新标题
    
    // 实心矩形（坐标X,Y 宽度,高度 填充模式1）
    LCD_SetTextColor(RED);
    ILI9806G_DrawRectangle(50, 300, 200, 100, 1);  
    
    // 空心矩形（填充模式0）
    LCD_SetTextColor(GREEN);
    ILI9806G_DrawRectangle(100, 300, 200, 120, 0);  
    
    // 大尺寸实心矩形
    LCD_SetTextColor(BLUE);
    ILI9806G_DrawRectangle(250, 300, 200, 150, 1);
    
    Delay(0x2FFFFFF);
    ILI9806G_Clear(0, 32*7, LCD_X_LENGTH, LCD_Y_LENGTH-32*7);  // 局部清屏

    /************** 第五阶段：圆形绘制演示 **************/
    ILI9806G_ClearLine(LINE(7));  // 准备圆形绘制区域
    LCD_SetTextColor(BLUE);
    ILI9806G_DispStringLine_EN_CH(LINE(7), "画圆:");  // 标题更新
    
    // 实心圆（中心X,Y 半径60 填充模式1）
    LCD_SetTextColor(RED);
    ILI9806G_DrawCircle(150, 400, 60, 1);  
    
    // 空心圆（填充模式0）
    LCD_SetTextColor(GREEN);
    ILI9806G_DrawCircle(250, 400, 60, 0);  
    
    // 第二个实心圆
    LCD_SetTextColor(BLUE);
    ILI9806G_DrawCircle(350, 400, 60, 1);
    
    Delay(0x2FFFFFF);
    ILI9806G_Clear(0, 32*7, LCD_X_LENGTH, LCD_Y_LENGTH-32*7);  // 结束清屏
}

/*********************end of file*************************/




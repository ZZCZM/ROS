/**
  ******************************************************************************
  * @file    stm32f4xx_ltdc.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the LTDC controller (LTDC) peripheral:
  *           + Initialization and configuration
  *           + Interrupts and flags management
  *           
  *  @verbatim
  
 ===============================================================================
                      ##### How to use this driver #####
 ===============================================================================
    [..]
        (#) Enable LTDC clock using 
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_LTDC, ENABLE) function.
        (#) Configures LTDC
          (++) Configure the required Pixel clock following the panel datasheet
          (++) Configure the Synchronous timings: VSYNC, HSYNC, Vertical and 
              Horizontal back proch, active data area and the front proch 
              timings 
          (++) Configure the synchronous signals and clock polarity in the 
              LTDC_GCR register
        (#) Configures Layer1/2 parameters
          (++) The Layer window horizontal and vertical position in the LTDC_LxWHPCR and 
               LTDC_WVPCR registers. The layer window must be in the active data area.
          (++) The pixel input format in the LTDC_LxPFCR register
          (++) The color frame buffer start address in the LTDC_LxCFBAR register
          (++) The line length and pitch of the color frame buffer in the 
               LTDC_LxCFBLR register
          (++) The number of lines of the color frame buffer in 
               the LTDC_LxCFBLNR register
          (++) if needed, load the CLUT with the RGB values and the address 
               in the LTDC_LxCLUTWR register
          (++) If needed, configure the default color and the blending factors 
               respectively in the LTDC_LxDCCR and LTDC_LxBFCR registers 

          (++) If needed, Dithering and color keying can be be enabled respectively 
               in the LTDC_GCR and LTDC_LxCKCR registers. It can be also enabled 
               on the fly.    
        (#) Enable Layer1/2 and if needed the CLUT in the LTDC_LxCR register 
  
        (#) Reload the shadow registers to active register through 
            the LTDC_SRCR register.
          -@- All layer parameters can be be modified on the fly except the CLUT. 
              The new configuration has to be either reloaded immediately 
              or during vertical blanking period by configuring the LTDC_SRCR register.
        (#) Call the LTDC_Cmd() to enable the LTDC controller.

    @endverbatim
  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_ltdc.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup LTDC 
  * @brief LTDC driver modules
  * @{
  */

/*
 * LTDC驱动私有类型和宏定义区
 */

/* 全局控制寄存器掩码定义
 * 用于保护写入LTDC全局控制寄存器（GCR）时关键配置位的完整性
 * 位掩码组成：0x0FFE888F
 *   [31:28] 保留位           0x0 (掩码禁止修改)
 *   [27:12] 有效控制字段      0xF E8 88 
 *   [11:0]  保留位           0xF (掩码禁止修改)
 * 具体对应寄存器位：
 *   HSYNC宽度控制（12-28位）等显示时序参数位
 *   HSYNC/VSYNC极性控制位
 */
#define GCR_MASK ((uint32_t)0x0FFE888F)  /* LTDC GCR寄存器有效位掩码 */

/*
 * 典型应用场景：当需要修改GCR寄存器时
 * 应遵循以下步骤：
 * temp = LTDC->GCR & ~GCR_MASK;     // 保留不需要修改的位
 * temp |= new_configuration;        // 合并新配置
 * LTDC->GCR = temp;                  // 整体写入
 */


/** @defgroup LTDC_Private_Functions
  * @{
  */

/** @defgroup LTDC_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim
 ===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the LTDC
      (+) Enable or Disable Dither
      (+) Define the position of the line interrupt
      (+) reload layers registers with new parameters
      (+) Initialize and configure layer1 and layer2
      (+) Set and configure the color keying functionality
      (+) Configure and Enables or disables CLUT 
      
@endverbatim
  * @{
  */
/**
  * @brief  复位LTDC外设至默认状态
  * @param  无
  * @retval 无
  * @note 通过RCC复位控制器实现硬件级复位
  */
void LTDC_DeInit(void)
{
  /* 使能LTDC硬件复位 */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_LTDC, ENABLE);
  /* 释放LTDC复位状态 */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_LTDC, DISABLE);
}

/**
  * @brief  LTDC显示控制器初始化
  * @param  LTDC_InitStruct: 包含显示参数的初始化结构体
  * @retval 无
  * @warning 必须在LTDC禁用状态下调用本函数
  * @details 配置内容包含：
  *          - 同步信号时序
  *          - 显示区域尺寸
  *          - 信号极性
  *          - 背景颜色
  */
void LTDC_Init(LTDC_InitTypeDef* LTDC_InitStruct)
{
  /* 局部变量声明 */
  uint32_t horizontalsync = 0;         // 水平同步参数缓存
  uint32_t accumulatedHBP = 0;          // 水平后沿累计值
  uint32_t accumulatedactiveW = 0;      // 有效宽度累计值
  uint32_t totalwidth = 0;              // 总宽度参数
  uint32_t backgreen = 0;               // 绿色背景分量
  uint32_t backred = 0;                 // 红色背景分量

  /* 参数合法性检查（所有参数范围校验）*/
  assert_param(IS_LTDC_HSYNC(LTDC_InitStruct->LTDC_HorizontalSync));        // 水平同步脉冲宽度（0-0xFFF）
  assert_param(IS_LTDC_VSYNC(LTDC_InitStruct->LTDC_VerticalSync));          // 垂直同步脉冲宽度（0-0x7FF）
  assert_param(IS_LTDC_AHBP(LTDC_InitStruct->LTDC_AccumulatedHBP));        // 累计水平后沿（HSYNC至有效像素）
  assert_param(IS_LTDC_AVBP(LTDC_InitStruct->LTDC_AccumulatedVBP));         // 累计垂直后沿（VSYNC至有效行）
  assert_param(IS_LTDC_AAH(LTDC_InitStruct->LTDC_AccumulatedActiveH));      // 垂直有效显示高度
  assert_param(IS_LTDC_AAW(LTDC_InitStruct->LTDC_AccumulatedActiveW));      // 水平有效显示宽度
  assert_param(IS_LTDC_TOTALH(LTDC_InitStruct->LTDC_TotalHeigh));           // 垂直总扫描线数
  assert_param(IS_LTDC_TOTALW(LTDC_InitStruct->LTDC_TotalWidth));          // 水平总时钟数
  assert_param(IS_LTDC_HSPOL(LTDC_InitStruct->LTDC_HSPolarity));           // HSYNC极性（0:低电平有效）
  assert_param(IS_LTDC_VSPOL(LTDC_InitStruct->LTDC_VSPolarity));           // VSYNC极性
  assert_param(IS_LTDC_DEPOL(LTDC_InitStruct->LTDC_DEPolarity));            // 数据使能信号极性
  assert_param(IS_LTDC_PCPOL(LTDC_InitStruct->LTDC_PCPolarity));           // 像素时钟极性（上升/下降沿采样）
  assert_param(IS_LTDC_BackBlueValue(LTDC_InitStruct->LTDC_BackgroundBlueValue));  // 背景蓝色分量（0-0xFF）
  assert_param(IS_LTDC_BackGreenValue(LTDC_InitStruct->LTDC_BackgroundGreenValue));// 背景绿色分量
  assert_param(IS_LTDC_BackRedValue(LTDC_InitStruct->LTDC_BackgroundRedValue));    // 背景红色分量

  /*----- 同步脉冲配置 -----*/
  /* 清除原同步参数 */
  LTDC->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);          // 清除垂直/水平同步寄存器位
  /* 配置水平同步脉冲宽度（存储到高位）和垂直同步脉冲宽度（低位） */
  horizontalsync = (LTDC_InitStruct->LTDC_HorizontalSync << 16); // HSYNCH[11:0]移位至bit16-27
  LTDC->SSCR |= (horizontalsync | LTDC_InitStruct->LTDC_VerticalSync);  // VSYNCH[10:0]存入bit0-10

  /*----- 后沿配置 -----*/
  /* 清除原后沿参数 */
  LTDC->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);        // 清除累计后沿寄存器
  /* 配置垂直/水平后沿 */
  accumulatedHBP = (LTDC_InitStruct->LTDC_AccumulatedHBP << 16); // 水平后沿（HBP+1）<<16
  LTDC->BPCR |= (accumulatedHBP | LTDC_InitStruct->LTDC_AccumulatedVBP); 

  /*----- 有效显示区域配置 -----*/
  /* 清除原有显示区域设置 */
  LTDC->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);          // [31:16]有效高度，[15:0]有效宽度
  /* 设置水平有效宽度和垂直有效高度 */
  accumulatedactiveW = (LTDC_InitStruct->LTDC_AccumulatedActiveW << 16); 
  LTDC->AWCR |= (accumulatedactiveW | LTDC_InitStruct->LTDC_AccumulatedActiveH);

  /*----- 总尺寸配置 -----*/
  /* 清除原尺寸参数 */
  LTDC->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);    // [31:16]垂直总数，[15:0]水平总数
  /* 设置总宽度和高度 */
  totalwidth = (LTDC_InitStruct->LTDC_TotalWidth << 16);    // TOTALW[15:0] <<16
  LTDC->TWCR |= (totalwidth | LTDC_InitStruct->LTDC_TotalHeigh); 

  /*----- 全局控制寄存器配置 -----*/
  /* 保留受保护的寄存器位 */
  LTDC->GCR &= (uint32_t)GCR_MASK;                         // 使用预设掩码保留关键位
  /* 配置同步信号极性和时钟极性 */
  LTDC->GCR |= (LTDC_InitStruct->LTDC_HSPolarity | LTDC_InitStruct->LTDC_VSPolarity | \
                LTDC_InitStruct->LTDC_DEPolarity | LTDC_InitStruct->LTDC_PCPolarity);

  /*----- 背景颜色配置 -----*/
  /* 将颜色分量移位到寄存器对应位置 */
  backgreen = (LTDC_InitStruct->LTDC_BackgroundGreenValue << 8);   // 绿色分量bit8-15
  backred = (LTDC_InitStruct->LTDC_BackgroundRedValue << 16);     // 红色分量bit16-23
  /* 清除原背景色设置 */
  LTDC->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
  /* 合并颜色分量并写入寄存器 */
  LTDC->BCCR |= (backred | backgreen | LTDC_InitStruct->LTDC_BackgroundBlueValue);
}
/**
  * @brief  初始化LTDC配置结构体为默认值
  * @param  LTDC_InitStruct: 要初始化的结构体指针
  * @retval None
  * @details 默认配置：
  *          - 同步信号低电平有效
  *          - 时钟在像素上升沿采样
  *          - 所有尺寸参数清零
  *          - 背景色黑色（RGB=0x0）
  */
void LTDC_StructInit(LTDC_InitTypeDef* LTDC_InitStruct)
{
  /*--------------- 重置所有参数为默认值 ----------------*/
  LTDC_InitStruct->LTDC_HSPolarity = LTDC_HSPolarity_AL;      /*!< 水平同步极性：低电平有效 */ 
  LTDC_InitStruct->LTDC_VSPolarity = LTDC_VSPolarity_AL;      /*!< 垂直同步极性：低电平有效 */
  LTDC_InitStruct->LTDC_DEPolarity = LTDC_DEPolarity_AL;      /*!< 数据使能极性：低电平有效 */
  LTDC_InitStruct->LTDC_PCPolarity = LTDC_PCPolarity_IPC;     /*!< 像素时钟极性：上升沿采样 */
  
  LTDC_InitStruct->LTDC_HorizontalSync = 0x00;                /*!< 水平同步宽度 = 0 */
  LTDC_InitStruct->LTDC_VerticalSync = 0x00;                 /*!< 垂直同步宽度 = 0 */
  LTDC_InitStruct->LTDC_AccumulatedHBP = 0x00;               /*!< 水平后沿累计值 = 0 */
  LTDC_InitStruct->LTDC_AccumulatedVBP = 0x00;               /*!< 垂直后沿累计值 = 0 */
  LTDC_InitStruct->LTDC_AccumulatedActiveW = 0x00;            /*!< 有效显示宽度 = 0 */
  LTDC_InitStruct->LTDC_AccumulatedActiveH = 0x00;            /*!< 有效显示高度 = 0 */
  LTDC_InitStruct->LTDC_TotalWidth = 0x00;                    /*!< 总水平宽度 = 0 */
  LTDC_InitStruct->LTDC_TotalHeigh = 0x00;                    /*!< 总垂直高度 = 0 */
  
  LTDC_InitStruct->LTDC_BackgroundRedValue = 0x00;           /*!< 背景红色分量 = 0 */
  LTDC_InitStruct->LTDC_BackgroundGreenValue = 0x00;         /*!< 背景绿色分量 = 0 */
  LTDC_InitStruct->LTDC_BackgroundBlueValue = 0x00;           /*!< 背景蓝色分量 = 0 */
}

/**
  * @brief  启用/禁用LTDC控制器
  * @param  NewState: 新状态(ENABLE/DISABLE)
  * @retval None
  * @warning 在显示输出期间突然禁用可能导致显示异常
  */
void LTDC_Cmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 通过设置GCR的LTDCEN位启用控制器 */
    LTDC->GCR |= (uint32_t)LTDC_GCR_LTDCEN; // 开启显示输出
  }
  else
  {
    /* 清除LTDCEN位立即停止显示输出 */
    LTDC->GCR &= ~(uint32_t)LTDC_GCR_LTDCEN; // 可能引起屏幕闪烁
  }
}

/**
  * @brief  启用/禁用颜色抖动功能
  * @param  NewState: 新状态(ENABLE/DISABLE)
  * @retval None
  * @details 抖动功能适用于：
  *          - RGB565等低位宽模式
  *          - 平滑颜色过渡伪影
  */
void LTDC_DitherCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 启用抖动算法 */
    LTDC->GCR |= (uint32_t)LTDC_GCR_DTEN; // 增加约5%的GPU负载
  }
  else
  {
    /* 禁用抖动（直接输出原始颜色值） */
    LTDC->GCR &= ~(uint32_t)LTDC_GCR_DTEN;
  }
}

/**
  * @brief  获取当前RGB颜色位宽配置
  * @retval 包含RGB位宽的结构体
  * @note 颜色位宽通常由图层配置决定
  */
LTDC_RGBTypeDef LTDC_GetRGBWidth(void)
{
  LTDC_RGBTypeDef LTDC_RGB_InitStruct;

  /* 保留受保护的寄存器位 */
  LTDC->GCR &= (uint32_t)GCR_MASK;

  /* 从GCR寄存器提取位宽设置 */
  LTDC_RGB_InitStruct.LTDC_BlueWidth  = (uint32_t)((LTDC->GCR >> 4) & 0x7);  // bit4-6: 蓝色位宽(0-7)
  LTDC_RGB_InitStruct.LTDC_GreenWidth = (uint32_t)((LTDC->GCR >> 8) & 0x7);  // bit8-10: 绿色位宽
  LTDC_RGB_InitStruct.LTDC_RedWidth   = (uint32_t)((LTDC->GCR >> 12) & 0x7); // bit12-14: 红色位宽

  return LTDC_RGB_InitStruct;
}
/**
  * @brief  初始化RGB颜色位宽结构体为默认值
  * @param  LTDC_RGB_InitStruct: 要初始化的结构体指针
  * @retval None
  * @details 默认配置：
  *          - 各颜色分量位宽=3 (实际值=2+1)
  *          适用于ARGB8888等8位颜色模式
  */
void LTDC_RGBStructInit(LTDC_RGBTypeDef* LTDC_RGB_InitStruct)
{
  LTDC_RGB_InitStruct->LTDC_BlueWidth = 0x02;   // 蓝色位宽=3 (0x02 + 1)
  LTDC_RGB_InitStruct->LTDC_GreenWidth = 0x02;  // 绿色位宽=3
  LTDC_RGB_InitStruct->LTDC_RedWidth = 0x02;    // 红色位宽=3
}

/**
  * @brief  配置行中断位置
  * @param  LTDC_LIPositionConfig: 行中断触发位置（0-0x7FF）
  * @retval None
  * @warning 行中断在垂直同步期间被锁定
  * @note 用于实现vsync事件检测或屏幕撕裂防止
  */
void LTDC_LIPConfig(uint32_t LTDC_LIPositionConfig)
{
  assert_param(IS_LTDC_LIPOS(LTDC_LIPositionConfig)); // 校验参数范围0-0x7FF
  
  /* 设置LIPCR寄存器触发位置 */
  LTDC->LIPCR = LTDC_LIPositionConfig; // 当当前行号等于此值时触发中断
}

/**
  * @brief  配置图层参数重载方式
  * @param  LTDC_Reload: 重载类型
  *          LTDC_IMReload  : 立即重载（可能造成画面撕裂）
  *          LTDC_VBReload  : 垂直消隐期重载（推荐）  
  * @retval None
  * @note 修改图层配置后必须调用此函数生效
  */
void LTDC_ReloadConfig(uint32_t LTDC_Reload)
{
  assert_param(IS_LTDC_RELOAD(LTDC_Reload));
  
  /* 写入重载控制寄存器 */
  LTDC->SRCR = LTDC_Reload;  // 0x02:立即重载；0x04:垂直消隐重载
}

/**
  * @brief  初始化指定图层参数
  * @param  LTDC_Layerx     : 图层选择(LTDC_Layer1/LTDC_Layer2)
  * @param  LTDC_Layer_InitStruct: 包含图层参数的初始化结构体
  * @retval None
  * @warning 必须在LTDC禁用状态下配置
  * @details 配置内容包含：
  *          - 显示窗口位置
  *          - 颜色格式
  *          - 混合模式
  *          - 帧缓冲区参数
  */
void LTDC_LayerInit(LTDC_Layer_TypeDef* LTDC_Layerx, 
                   LTDC_Layer_InitTypeDef* LTDC_Layer_InitStruct)
{
  uint32_t whsppos = 0, wvsppos = 0;
  uint32_t dcgreen = 0, dcred = 0, dcalpha = 0;
  uint32_t cfbp = 0;

  /* 参数有效性验证 */
  assert_param(IS_LTDC_Pixelformat(LTDC_Layer_InitStruct->LTDC_PixelFormat)); // 校验ARGB888等格式
  assert_param(IS_LTDC_BlendingFactor1(LTDC_Layer_InitStruct->LTDC_BlendingFactor_1)); // 混合因子1校验
  assert_param(IS_LTDC_BlendingFactor2(LTDC_Layer_InitStruct->LTDC_BlendingFactor_2)); // 混合因子2校验
  assert_param(IS_LTDC_HCONFIGST(LTDC_Layer_InitStruct->LTDC_HorizontalStart)); // 水平起始位置校验
  assert_param(IS_LTDC_HCONFIGSP(LTDC_Layer_InitStruct->LTDC_HorizontalStop));  // 水平结束位置校验
  assert_param(IS_LTDC_VCONFIGST(LTDC_Layer_InitStruct->LTDC_VerticalStart));  // 垂直起始位置校验
  assert_param(IS_LTDC_VCONFIGSP(LTDC_Layer_InitStruct->LTDC_VerticalStop));    // 垂直结束位置校验  
  assert_param(IS_LTDC_DEFAULTCOLOR(LTDC_Layer_InitStruct->LTDC_DefaultColorBlue));    // 蓝色分量范围校验
  assert_param(IS_LTDC_DEFAULTCOLOR(LTDC_Layer_InitStruct->LTDC_DefaultColorGreen));   // 绿色分量范围校验
  assert_param(IS_LTDC_DEFAULTCOLOR(LTDC_Layer_InitStruct->LTDC_DefaultColorRed));     // 红色分量范围校验
  assert_param(IS_LTDC_DEFAULTCOLOR(LTDC_Layer_InitStruct->LTDC_DefaultColorAlpha));  // 透明分量范围校验
  assert_param(IS_LTDC_CFBP(LTDC_Layer_InitStruct->LTDC_CFBPitch));         // 行间距校验（字节对齐）
  assert_param(IS_LTDC_CFBLL(LTDC_Layer_InitStruct->LTDC_CFBLineLength));  // 每行字节数校验
  assert_param(IS_LTDC_CFBLNBR(LTDC_Layer_InitStruct->LTDC_CFBLineNumber));// 总行数校验

  /*----- 窗口位置配置 -----*/
  /* 水平起始/结束位置（结束位置<<16） */
  whsppos = LTDC_Layer_InitStruct->LTDC_HorizontalStop << 16;
  LTDC_Layerx->WHPCR = (LTDC_Layer_InitStruct->LTDC_HorizontalStart | whsppos);

  /* 垂直起始/结束位置（结束位置<<16） */
  wvsppos = LTDC_Layer_InitStruct->LTDC_VerticalStop << 16;
  LTDC_Layerx->WVPCR  = (LTDC_Layer_InitStruct->LTDC_VerticalStart | wvsppos);

  /*----- 像素格式配置 -----*/
  LTDC_Layerx->PFCR = LTDC_Layer_InitStruct->LTDC_PixelFormat; // 设置如LTDC_Pixelformat_RGB565

  /*----- 默认颜色配置 -----*/
  dcgreen = LTDC_Layer_InitStruct->LTDC_DefaultColorGreen << 8;
  dcred = LTDC_Layer_InitStruct->LTDC_DefaultColorRed << 16;
  dcalpha = LTDC_Layer_InitStruct->LTDC_DefaultColorAlpha << 24;
  LTDC_Layerx->DCCR = (LTDC_Layer_InitStruct->LTDC_DefaultColorBlue | dcgreen | dcred | dcalpha);

  /*----- 常量透明度配置 -----*/
  LTDC_Layerx->CACR = LTDC_Layer_InitStruct->LTDC_ConstantAlpha; // 0-255透明度（0全透明）

  /*----- 混合因子配置 -----*/
  LTDC_Layerx->BFCR = (LTDC_Layer_InitStruct->LTDC_BlendingFactor_1 |  // 如LTDC_BlendingFactor1_PAxCA
                       LTDC_Layer_InitStruct->LTDC_BlendingFactor_2); // 如LTDC_BlendingFactor2_PAxCA

  /*----- 帧缓冲区配置 -----*/
  LTDC_Layerx->CFBAR = LTDC_Layer_InitStruct->LTDC_CFBStartAdress; // 显存起始地址

  cfbp = LTDC_Layer_InitStruct->LTDC_CFBPitch << 16;              // 行间距（单位：字节）
  LTDC_Layerx->CFBLR = (LTDC_Layer_InitStruct->LTDC_CFBLineLength | cfbp); // 行长度=有效像素+间隔

  LTDC_Layerx->CFBLNR = LTDC_Layer_InitStruct->LTDC_CFBLineNumber; // 总显示行数
}

/**
  * @brief  初始化LTDC图层配置结构体为默认值
  * @param  LTDC_Layer_InitStruct: 要初始化的图层结构体指针
  * @retval None
  * @details 默认配置：
  *          - 水平起始和结束位置 = 0
  *          - 垂直起始和结束位置 = 0
  *          - 像素格式 = ARGB8888
  *          - 常量透明度 = 0xFF（完全不透明）
  *          - 默认颜色 = 黑色（RGB=0x0）
  *          - 混合因子1 = PAxCA
  *          - 混合因子2 = PAxCA
  *          - 帧缓冲区起始地址 = 0x00
  *          - 帧缓冲区行长度 = 0x00
  *          - 帧缓冲区行间距 = 0x00
  *          - 帧缓冲区行数 = 0x00
  *          适用于配置新图层时的初始状态设置
  */
void LTDC_LayerStructInit(LTDC_Layer_InitTypeDef * LTDC_Layer_InitStruct)
{
  /*--------------- 重置图层结构体参数值 -------------------*/

  /*!< 初始化水平限制参数 */
  LTDC_Layer_InitStruct->LTDC_HorizontalStart = 0x00;  // 水平起始位置
  LTDC_Layer_InitStruct->LTDC_HorizontalStop = 0x00;   // 水平结束位置

  /*!< 初始化垂直限制参数 */
  LTDC_Layer_InitStruct->LTDC_VerticalStart = 0x00;     // 垂直起始位置
  LTDC_Layer_InitStruct->LTDC_VerticalStop = 0x00;     // 垂直结束位置

  /*!< 初始化像素格式参数 */
  LTDC_Layer_InitStruct->LTDC_PixelFormat = LTDC_Pixelformat_ARGB8888;  // 默认ARGB8888格式

  /*!< 初始化常量透明度参数 */
  LTDC_Layer_InitStruct->LTDC_ConstantAlpha = 0xFF;  // 默认完全不透明

  /*!< 初始化默认颜色参数 */
  LTDC_Layer_InitStruct->LTDC_DefaultColorBlue = 0x00;  // 默认蓝色分量
  LTDC_Layer_InitStruct->LTDC_DefaultColorGreen = 0x00; // 默认绿色分量
  LTDC_Layer_InitStruct->LTDC_DefaultColorRed = 0x00;   // 默认红色分量
  LTDC_Layer_InitStruct->LTDC_DefaultColorAlpha = 0x00; // 默认透明度（不透明）

  /*!< 初始化混合因子参数 */
  LTDC_Layer_InitStruct->LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;  // 混合因子1
  LTDC_Layer_InitStruct->LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;  // 混合因子2

  /*!< 初始化帧缓冲区起始地址 */
  LTDC_Layer_InitStruct->LTDC_CFBStartAdress = 0x00;  // 显存起始地址

  /*!< 初始化帧缓冲区行长度和行间距 */
  LTDC_Layer_InitStruct->LTDC_CFBLineLength = 0x00;   // 每行字节数
  LTDC_Layer_InitStruct->LTDC_CFBPitch = 0x00;       // 行间距（字节）

  /*!< 初始化帧缓冲区行数 */
  LTDC_Layer_InitStruct->LTDC_CFBLineNumber = 0x00;  // 总行数
}

/**
  * @brief  启用/禁用指定的LTDC图层
  * @param  LTDC_Layerx: 选择要配置的图层，可以是LTDC_Layer1或LTDC_Layer2
  * @param  NewState: 新状态(ENABLE/DISABLE)
  * @retval None
  * @details 硬件特性：
  *          - 设置LEN位启用图层
  *          - 清除LEN位禁用图层
  *          - 必须在LTDC禁用状态下配置图层
  */
void LTDC_LayerCmd(LTDC_Layer_TypeDef* LTDC_Layerx, FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 启用指定图层：设置LEN位 */
    LTDC_Layerx->CR |= (uint32_t)LTDC_LxCR_LEN;  // 使能图层
  }
  else
  {
    /* 禁用指定图层：清除LEN位 */
    LTDC_Layerx->CR &= ~(uint32_t)LTDC_LxCR_LEN; // 禁用图层
  }
}

/**
  * @brief  获取当前显示位置
  * @param  LTDC_Pos_InitStruct: 指向包含当前显示位置的LTDC_PosTypeDef结构体指针
  * @retval None
  * @details 返回值：
  *          - LTDC_POSX: 当前水平位置
  *          - LTDC_POSY: 当前垂直位置
  *          用于调试和同步操作
  */
LTDC_PosTypeDef LTDC_GetPosStatus(void)
{
  LTDC_PosTypeDef LTDC_Pos_InitStruct;

  /* 所有位域保留，仅读取当前位置 */
  LTDC->CPSR &= ~(LTDC_CPSR_CYPOS | LTDC_CPSR_CXPOS);

  /* 提取水平位置（bit16-31） */
  LTDC_Pos_InitStruct.LTDC_POSX = (uint32_t)(LTDC->CPSR >> 16);

  /* 提取垂直位置（bit0-15） */
  LTDC_Pos_InitStruct.LTDC_POSY = (uint32_t)(LTDC->CPSR & 0xFFFF);

  return LTDC_Pos_InitStruct;
}

/**
  * @brief  初始化LTDC显示位置结构体为默认值
  * @param  LTDC_Pos_InitStruct: 指向要初始化的LTDC_PosTypeDef结构体指针
  * @retval None
  * @details 默认配置：
  *          - LTDC_POSX: 0x00
  *          - LTDC_POSY: 0x00
  *          适用于调试和位置初始化
  */
void LTDC_PosStructInit(LTDC_PosTypeDef* LTDC_Pos_InitStruct)
{
  LTDC_Pos_InitStruct->LTDC_POSX = 0x00;  // 默认水平位置
  LTDC_Pos_InitStruct->LTDC_POSY = 0x00;  // 默认垂直位置
}
/**
  * @brief  检查指定的LTDC标志是否置位
  * @param  LTDC_CD: 要检查的标志
  *         可选值：
  *           LTDC_CD_VDES: 垂直数据使能当前状态
  *           LTDC_CD_HDES: 水平数据使能当前状态
  *           LTDC_CD_VSYNC: 垂直同步当前状态
  *           LTDC_CD_HSYNC: 水平同步当前状态
  * @retval 标志状态（SET或RESET）
  * @details 用于检查LTDC控制器的同步状态
  */
FlagStatus LTDC_GetCDStatus(uint32_t LTDC_CD)
{
  FlagStatus bitstatus;

  /* 校验标志参数有效性 */
  assert_param(IS_LTDC_GET_CD(LTDC_CD));

  /* 检查指定标志位是否置位 */
  if ((LTDC->CDSR & LTDC_CD) != (uint32_t)RESET)
  {
    bitstatus = SET;  // 标志位置位，返回SET
  }
  else
  {
    bitstatus = RESET; // 标志位未置位，返回RESET
  }
  return bitstatus;
}

/**
  * @brief  配置和设置颜色键控
  * @param  LTDC_colorkeying_InitStruct: 包含颜色键控配置的结构体指针
  * @param  LTDC_layerx: 选择要配置的图层（LTDC_Layer1或LTDC_Layer2）
  * @param  NewState: 新状态(ENABLE/DISABLE)
  * @retval None
  * @details 颜色键控用于指定的一种颜色作为透明颜色
  *          - 启用时，指定颜色的像素将不显示
  *          - 需要指定图层和颜色键控参数
  */
void LTDC_ColorKeyingConfig(LTDC_Layer_TypeDef* LTDC_Layerx, 
                           LTDC_ColorKeying_InitTypeDef* LTDC_colorkeying_InitStruct, 
                           FunctionalState NewState)
{ 
  uint32_t ckgreen = 0, ckred = 0;

  /* 校验状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 校验颜色键控参数有效性 */
  assert_param(IS_LTDC_CKEYING(LTDC_colorkeying_InitStruct->LTDC_ColorKeyBlue));
  assert_param(IS_LTDC_CKEYING(LTDC_colorkeying_InitStruct->LTDC_ColorKeyGreen));
  assert_param(IS_LTDC_CKEYING(LTDC_colorkeying_InitStruct->LTDC_ColorKeyRed));
  
  if (NewState != DISABLE)
  {
    /* 启用颜色键控：设置COLKEN位 */
    LTDC_Layerx->CR |= (uint32_t)LTDC_LxCR_COLKEN;  // 使能颜色键控

    /* 设置颜色键控值（红、绿、蓝） */
    ckgreen = (LTDC_colorkeying_InitStruct->LTDC_ColorKeyGreen << 8);
    ckred = (LTDC_colorkeying_InitStruct->LTDC_ColorKeyRed << 16);
    LTDC_Layerx->CKCR &= ~(LTDC_LxCKCR_CKBLUE | LTDC_LxCKCR_CKGREEN | LTDC_LxCKCR_CKRED); // 清除原有颜色键控值
    LTDC_Layerx->CKCR |= (LTDC_colorkeying_InitStruct->LTDC_ColorKeyBlue | ckgreen | ckred); // 设置新的颜色键控值
  }
  else
  {
    /* 禁用颜色键控：清除COLKEN位 */
    LTDC_Layerx->CR &= ~(uint32_t)LTDC_LxCR_COLKEN; // 禁用颜色键控
  }
  
  /* 立即重载配置 */
  LTDC->SRCR = LTDC_IMReload;  // 触发立即重载
}

/**
  * @brief  初始化颜色键控结构体为默认值
  * @param  LTDC_colorkeying_InitStruct: 要初始化的结构体指针
  * @retval None
  * @details 默认配置：
  *          - LTDC_ColorKeyBlue: 0x00
  *          - LTDC_ColorKeyGreen: 0x00
  *          - LTDC_ColorKeyRed: 0x00
  *          适用于配置颜色键控前的初始化
  */
void LTDC_ColorKeyingStructInit(LTDC_ColorKeying_InitTypeDef* LTDC_colorkeying_InitStruct)
{
  /* 初始化颜色键控值 */
  LTDC_colorkeying_InitStruct->LTDC_ColorKeyBlue = 0x00;  // 默认蓝色分量
  LTDC_colorkeying_InitStruct->LTDC_ColorKeyGreen = 0x00; // 默认绿色分量
  LTDC_colorkeying_InitStruct->LTDC_ColorKeyRed = 0x00;   // 默认红色分量
}

/**
  * @brief  启用/禁用CLUT（颜色查找表）
  * @param  LTDC_layerx: 选择要配置的图层（LTDC_Layer1或LTDC_Layer2）
  * @param  NewState: 新状态(ENABLE/DISABLE)
  * @retval None
  * @details CLUT用于将索引颜色转换为实际RGB值
  *          - 启用后，图层使用CLUT配置的颜色转换
  *          - 需要配置图层和颜色查找表
  */
void LTDC_CLUTCmd(LTDC_Layer_TypeDef* LTDC_Layerx, FunctionalState NewState)
{
  /* 校验状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 启用CLUT：设置CLUTEN位 */
    LTDC_Layerx->CR |= (uint32_t)LTDC_LxCR_CLUTEN;  // 使能颜色查找表
  }
  else
  {
    /* 禁用CLUT：清除CLUTEN位 */
    LTDC_Layerx->CR &= ~(uint32_t)LTDC_LxCR_CLUTEN; // 禁用颜色查找表
  }
  
  /* 立即重载配置 */
  LTDC->SRCR = LTDC_IMReload;  // 触发立即重载
}

/**
  * @brief  配置颜色查找表（CLUT）
  * @param  LTDC_CLUT_InitStruct: 包含CLUT配置的结构体指针
  * @param  LTDC_layerx: 选择要配置的图层（LTDC_Layer1或LTDC_Layer2）
  * @retval None
  * @details 设置CLUT以将索引颜色转换为实际RGB值
  *          - 需要指定图层和颜色查找表配置
  *          - 适用于索引颜色模式（如L8、ARGB8888）
  */
void LTDC_CLUTInit(LTDC_Layer_TypeDef* LTDC_Layerx, LTDC_CLUT_InitTypeDef* LTDC_CLUT_InitStruct)
{  
  uint32_t green = 0;
  uint32_t red = 0;
  uint32_t clutadd = 0;

  /* 校验CLUT地址和RGB值的有效性 */
  assert_param(IS_LTDC_CLUTWR(LTDC_CLUT_InitStruct->LTDC_CLUTAdress));
  assert_param(IS_LTDC_CLUTWR(LTDC_CLUT_InitStruct->LTDC_RedValue));
  assert_param(IS_LTDC_CLUTWR(LTDC_CLUT_InitStruct->LTDC_GreenValue));
  assert_param(IS_LTDC_CLUTWR(LTDC_CLUT_InitStruct->LTDC_BlueValue));
    
  /* 组合CLUT地址和RGB值 */
  green = (LTDC_CLUT_InitStruct->LTDC_GreenValue << 8);  // 绿色分量移位至bit8-15
  red = (LTDC_CLUT_InitStruct->LTDC_RedValue << 16);     // 红色分量移位至bit16-23
  clutadd = (LTDC_CLUT_InitStruct->LTDC_CLUTAdress << 24); // CLUT地址移位至bit24-31
  LTDC_Layerx->CLUTWR = (clutadd | LTDC_CLUT_InitStruct->LTDC_BlueValue | green | red);
}

/**
  * @brief  初始化LTDC_CLUT_InitStruct结构体为默认值
  * @param  LTDC_CLUT_InitStruct: 要初始化的结构体指针
  * @retval None
  * @details 默认配置：
  *          - LTDC_CLUTAdress: 0x00
  *          - LTDC_BlueValue: 0x00
  *          - LTDC_GreenValue: 0x00
  *          - LTDC_RedValue: 0x00
  *          适用于配置CLUT前的初始化
  */
void LTDC_CLUTStructInit(LTDC_CLUT_InitTypeDef* LTDC_CLUT_InitStruct)
{
  /* 初始化CLUT地址和RGB值 */
  LTDC_CLUT_InitStruct->LTDC_CLUTAdress = 0x00;  // 默认CLUT地址
  LTDC_CLUT_InitStruct->LTDC_BlueValue = 0x00;     // 默认蓝色分量
  LTDC_CLUT_InitStruct->LTDC_GreenValue = 0x00;   // 默认绿色分量
  LTDC_CLUT_InitStruct->LTDC_RedValue = 0x00;      // 默认红色分量
}

/**
  * @brief  重新配置图层位置
  * @param  OffsetX: 水平偏移量（从起始有效宽度开始）
  * @param  OffsetY: 垂直偏移量（从起始有效高度开始）
  * @param  LTDC_layerx: 选择要配置的图层（LTDC_Layer1或LTDC_Layer2）
  * @retval 重新加载影子寄存器以应用新的位置配置
  * @details 调整图层在显示区域中的位置
  *          - 适用于需要动态调整图层位置的场景
  *          - 配置完成后需调用LTDC_ReloadConfig以应用更改
  */
void LTDC_LayerPosition(LTDC_Layer_TypeDef* LTDC_Layerx, uint16_t OffsetX, uint16_t OffsetY)
{
  uint32_t tempreg, temp;
  uint32_t horizontal_start;
  uint32_t horizontal_stop;
  uint32_t vertical_start;
  uint32_t vertical_stop;
  
  /* 清除原有的起始和结束位置配置 */
  LTDC_Layerx->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
  LTDC_Layerx->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
  
  /* 重新配置水平起始位置 */
  tempreg = LTDC->BPCR;
  horizontal_start = (tempreg >> 16) + 1 + OffsetX;  // 计算水平起始位置

  /* 重新配置垂直起始位置 */
  vertical_start = (tempreg & 0xFFFF) + 1 + OffsetY;  // 计算垂直起始位置

  /* 重新配置水平结束位置 */
  /* 获取每像素字节数 */
  tempreg = LTDC_Layerx->PFCR;
  
  if (tempreg == LTDC_Pixelformat_ARGB8888)
  {
    temp = 4;  // ARGB8888格式每像素4字节
  }
  else if (tempreg == LTDC_Pixelformat_RGB888)
  {
    temp = 3;  // RGB888格式每像素3字节
  }
  else if ((tempreg == LTDC_Pixelformat_ARGB4444) || 
           (tempreg == LTDC_Pixelformat_RGB565)    ||  
           (tempreg == LTDC_Pixelformat_ARGB1555)  ||
           (tempreg == LTDC_Pixelformat_AL88))
  {
    temp = 2;  // 16位格式每像素2字节
  }
  else
  {
    temp = 1;  // 8位格式每像素1字节
  }  
  
  /* 计算水平结束位置 */
  tempreg = LTDC_Layerx->CFBLR;
  horizontal_stop = (((tempreg & 0x1FFF) - 3) / temp) + horizontal_start - 1;  // 计算水平结束位置

  /* 重新配置垂直结束位置 */
  tempreg = LTDC_Layerx->CFBLNR;
  vertical_stop = (tempreg & 0x7FF) + vertical_start - 1;  // 计算垂直结束位置
  
  /* 写入新的水平起始和结束位置 */
  LTDC_Layerx->WHPCR = horizontal_start | (horizontal_stop << 16);

  /* 写入新的垂直起始和结束位置 */
  LTDC_Layerx->WVPCR = vertical_start | (vertical_stop << 16);  
}

  /**
  * @brief  重新配置图层常量透明度
  * @param  ConstantAlpha: 常量透明度值（0-255）
  * @param  LTDC_layerx: 选择要配置的图层（LTDC_Layer1或LTDC_Layer2）
  * @retval 重新加载影子寄存器以应用新的透明度配置
  * @details 设置图层的常量透明度值
  *          - 常量透明度值范围为0-255，0表示完全透明，255表示完全不透明
  *          - 适用于需要整体透明度调整的场景
  */
void LTDC_LayerAlpha(LTDC_Layer_TypeDef* LTDC_Layerx, uint8_t ConstantAlpha)
{  
  /* 重新配置常量透明度值 */
  LTDC_Layerx->CACR = ConstantAlpha;  // 设置常量透明度
}

/**
  * @brief  重新配置图层地址
  * @param  Address: 颜色帧缓冲区起始地址
  * @param  LTDC_layerx: 选择要配置的图层（LTDC_Layer1或LTDC_Layer2）
  * @retval 重新加载影子寄存器以应用新的地址配置
  * @details 设置图层的帧缓冲区起始地址
  *          - 适用于需要动态更改显示数据地址的场景
  *          - 配置完成后需调用LTDC_ReloadConfig以应用更改
  */
void LTDC_LayerAddress(LTDC_Layer_TypeDef* LTDC_Layerx, uint32_t Address)
{
  /* 重新配置颜色帧缓冲区起始地址 */
  LTDC_Layerx->CFBAR = Address;  // 设置帧缓冲区起始地址
}

/**
  * @brief  重新配置图层大小
  * @param  Width: 图层窗口宽度
  * @param  Height: 图层窗口高度
  * @param  LTDC_layerx: 选择要配置的图层（LTDC_Layer1或LTDC_Layer2）
  * @retval 重新加载影子寄存器以应用新的大小配置
  * @details 设置图层的显示窗口大小
  *          - 适用于需要动态调整图层显示区域大小的场景
  *          - 配置完成后需调用LTDC_ReloadConfig以应用更改
  */
void LTDC_LayerSize(LTDC_Layer_TypeDef* LTDC_Layerx, uint32_t Width, uint32_t Height)
{
  uint8_t temp;
  uint32_t tempreg;
  uint32_t horizontal_start;
  uint32_t horizontal_stop;
  uint32_t vertical_start;
  uint32_t vertical_stop;  
  
  /* 根据像素格式计算每像素字节数 */
  tempreg = LTDC_Layerx->PFCR;
  
  if (tempreg == LTDC_Pixelformat_ARGB8888)
  {
    temp = 4;  // ARGB8888格式每像素4字节
  }
  else if (tempreg == LTDC_Pixelformat_RGB888)
  {
    temp = 3;  // RGB888格式每像素3字节
  }
  else if ((tempreg == LTDC_Pixelformat_ARGB4444) || 
           (tempreg == LTDC_Pixelformat_RGB565)    ||  
           (tempreg == LTDC_Pixelformat_ARGB1555)  ||
           (tempreg == LTDC_Pixelformat_AL88))
  {
    temp = 2;  // 16位格式每像素2字节
  }
  else
  {
    temp = 1;  // 8位格式每像素1字节
  }

  /* 更新水平起始和结束位置 */
  tempreg = LTDC_Layerx->WHPCR;
  horizontal_start = (tempreg & 0x1FFF);  // 提取当前水平起始位置
  horizontal_stop = Width + horizontal_start - 1;  // 计算新的水平结束位置

  /* 更新垂直起始和结束位置 */
  tempreg = LTDC_Layerx->WVPCR;
  vertical_start = (tempreg & 0x1FFF);  // 提取当前垂直起始位置
  vertical_stop = Height + vertical_start - 1;  // 计算新的垂直结束位置

  LTDC_Layerx->WHPCR = horizontal_start | (horizontal_stop << 16);  // 写入新的水平起始和结束位置
  LTDC_Layerx->WVPCR = vertical_start | (vertical_stop << 16);  // 写入新的垂直起始和结束位置

  /* 重新配置颜色帧缓冲区行间距（CFBLR寄存器） */
  LTDC_Layerx->CFBLR &= ~(LTDC_LxCFBLR_CFBP);  // 清除原有行间距配置
  LTDC_Layerx->CFBLR |= (Width * temp) << 16;  // 设置新的行间距

  /* 重新配置颜色帧缓冲区行数（CFBLNR寄存器） */
  LTDC_Layerx->CFBLNR &= ~(LTDC_LxCFBLNR_CFBLNBR);  // 清除原有行数配置
  LTDC_Layerx->CFBLNR = Height;  // 设置新的行数
}
/**
  * @brief  重新配置LTDC层的像素格式
  * @details 该函数用于动态修改指定图层的像素格式配置，包含以下主要操作：
  *          - 根据当前像素格式计算每像素字节数
  *          - 重新计算帧缓冲区行间距
  *          - 更新帧缓冲区参数寄存器和像素格式配置寄存器
  * @param  LTDC_Layerx: 要配置的LTDC层指针，有效值为 LTDC_Layer1 或 LTDC_Layer2
  * @param  PixelFormat: 新像素格式，取值范围见 @ref LTDC_Pixelformat 枚举
  * @retval 需要调用 LTDC_ReloadConfig() 触发影子寄存器重载才能使配置生效
  * @note 操作顺序说明：
  *       1. 首先获取当前像素格式对应的字节数
  *       2. 计算当前行间距参数
  *       3. 根据新像素格式确定新字节数
  *       4. 重新组合行间距寄存器值
  *       5. 更新像素格式配置寄存器
  */
void LTDC_LayerPixelFormat(LTDC_Layer_TypeDef* LTDC_Layerx, uint32_t PixelFormat)
{
    // 定义临时变量：
    uint8_t temp;       // 存储每个像素的字节数
    uint32_t tempreg;   // 用于寄存器值中间计算

    /* ----- 第一阶段：处理当前像素格式 ----- */
    // 从像素格式寄存器(PFCR)获取当前配置值
    tempreg = LTDC_Layerx->PFCR;
    
    // 根据当前像素格式确定每像素字节数
    if (tempreg == LTDC_Pixelformat_ARGB8888)  // 32位色（4字节）
    {
        temp = 4;
    }
    else if (tempreg == LTDC_Pixelformat_RGB888)  // 24位色（3字节）
    {
        temp = 3;
    }
    // 处理16位色/特殊格式（2字节）
    else if ((tempreg == LTDC_Pixelformat_ARGB4444) || \
            (tempreg == LTDC_Pixelformat_RGB565)    || \
            (tempreg == LTDC_Pixelformat_ARGB1555)  || \
            (tempreg == LTDC_Pixelformat_AL88))  
    {
        temp = 2;  
    }
    else  // 默认处理8位色（1字节）
    {
        temp = 1;
    }
    
    /* ----- 第二阶段：计算行间距参数 ----- */
    // 从CFBLR寄存器获取当前行间距配置（取高16位CFBPL字段）
    tempreg = (LTDC_Layerx->CFBLR >> 16);
    // 计算实际像素宽度（行间距字节数 / 每像素字节数）
    tempreg = (tempreg / temp); 

    /* ----- 第三阶段：处理新像素格式 ----- */
    // 根据新像素格式确定每像素字节数（逻辑与第一阶段相同）
    if (PixelFormat == LTDC_Pixelformat_ARGB8888)
    {
        temp = 4;
    }
    else if (PixelFormat == LTDC_Pixelformat_RGB888)
    {
        temp = 3;
    }
    else if ((PixelFormat == LTDC_Pixelformat_ARGB4444) || \
            (PixelFormat == LTDC_Pixelformat_RGB565)    || \
            (PixelFormat == LTDC_Pixelformat_ARGB1555)  || \
            (PixelFormat == LTDC_Pixelformat_AL88))
    {
        temp = 2;  
    }
    else
    {
        temp = 1;
    }
    
    /* ----- 第四阶段：更新寄存器 ----- */
    // 重新计算并设置CFBLR寄存器：
    // 高16位：像素宽度 × 每像素字节数 → CFBPL（行间距字节数）
    // 低16位：行首地址偏移量（计算结果+3，保证4字节对齐）
    LTDC_Layerx->CFBLR  = ((tempreg * temp) << 16) | ((tempreg * temp) + 3);  

    // 更新像素格式配置寄存器（PFCR）
    LTDC_Layerx->PFCR = PixelFormat;
}

    
/**
  * @}
  */

/** @defgroup LTDC_Group2 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim
 ===============================================================================
            ##### Interrupts and flags management functions #####
 ===============================================================================

    [..] This section provides functions allowing to configure the LTDC Interrupts 
         and to get the status and clear flags and Interrupts pending bits.
  
    [..] The LTDC provides 4 Interrupts sources and 4 Flags
    
    *** Flags ***
    =============
    [..]
      (+) LTDC_FLAG_LI:   Line Interrupt flag.
      (+) LTDC_FLAG_FU:   FIFO Underrun Interrupt flag.
      (+) LTDC_FLAG_TERR: Transfer Error Interrupt flag.
      (+) LTDC_FLAG_RR:   Register Reload interrupt flag.
      
    *** Interrupts ***
    ==================
    [..]
      (+) LTDC_IT_LI: Line Interrupt is generated when a programmed line 
                      is reached. The line interrupt position is programmed in 
                      the LTDC_LIPR register.
      (+) LTDC_IT_FU: FIFO Underrun interrupt is generated when a pixel is requested 
                      from an empty layer FIFO
      (+) LTDC_IT_TERR: Transfer Error interrupt is generated when an AHB bus 
                        error occurs during data transfer.
      (+) LTDC_IT_RR: Register Reload interrupt is generated when the shadow 
                      registers reload was performed during the vertical blanking 
                      period.
               
@endverbatim
  * @{
  */
/**
  * @brief  配置LTDC中断使能状态
  * @details 该函数用于控制LTDC各中断源的使能状态，包含以下操作：
  *          - 参数有效性校验（通过assert_param实现）
  *          - 根据NewState设置/清除中断使能寄存器IER的对应位
  * @param  LTDC_IT: 中断源选择，支持以下值的位或组合：
  *     @arg LTDC_IT_LI:   行中断（当扫描到指定行时触发）
  *     @arg LTDC_IT_FU:   FIFO欠载中断（数据传输速度超过像素时钟时触发）
  *     @arg LTDC_IT_TERR: 传输错误中断（DMA传输错误时触发）
  *     @arg LTDC_IT_RR:   寄存器重载中断（影子寄存器重载完成时触发）
  * @param  NewState: 中断状态，取值：
  *     ENABLE  - 使能指定中断
  *     DISABLE - 禁用指定中断
  * @retval 无
  */
void LTDC_ITConfig(uint32_t LTDC_IT, FunctionalState NewState)
{
  /* 参数有效性校验 */
  assert_param(IS_LTDC_IT(LTDC_IT));          // 验证中断类型参数有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));// 验证状态参数有效性

  /* 根据状态参数设置中断使能寄存器 */
  if (NewState != DISABLE)
  {
    LTDC->IER |= LTDC_IT;  // 设置IER寄存器对应位（置1使能中断）
  }
  else
  {
    LTDC->IER &= (uint32_t)~LTDC_IT; // 清除IER寄存器对应位（置0禁用中断）
  }
}

/**
  * @brief  获取LTDC状态标志位
  * @details 该函数用于查询指定的硬件状态标志，包含以下操作：
  *          - 参数有效性校验
  *          - 读取中断状态寄存器ISR的对应位
  * @param  LTDC_FLAG: 要查询的状态标志，取值：
  *     @arg LTDC_FLAG_LI:   行中断标志
  *     @arg LTDC_FLAG_FU:   FIFO欠载标志
  *     @arg LTDC_FLAG_TERR: 传输错误标志
  *     @arg LTDC_FLAG_RR:   寄存器重载标志
  * @retval 标志位状态：
  *     SET   - 标志位已置位
  *     RESET - 标志位未置位
  */
FlagStatus LTDC_GetFlagStatus(uint32_t LTDC_FLAG)
{
  FlagStatus bitstatus = RESET;

  /* 参数有效性校验 */
  assert_param(IS_LTDC_FLAG(LTDC_FLAG)); // 验证标志位参数有效性

  /* 读取中断状态寄存器对应位 */
  if ((LTDC->ISR & LTDC_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;   // 标志位为1时返回SET
  }
  else
  {
    bitstatus = RESET; // 标志位为0时返回RESET
  }
  return bitstatus;
}

/**
  * @brief  清除LTDC状态标志位
  * @details 该函数用于清除指定的硬件状态标志，包含以下操作：
  *          - 参数有效性校验
  *          - 写中断清除寄存器ICR触发标志清除
  * @param  LTDC_FLAG: 要清除的状态标志，支持位或组合：
  *     @arg LTDC_FLAG_LI:   行中断标志
  *     @arg LTDC_FLAG_FU:   FIFO欠载标志
  *     @arg LTDC_FLAG_TERR: 传输错误标志
  *     @arg LTDC_FLAG_RR:   寄存器重载标志
  * @retval 无
  * @note 清除标志通过向ICR寄存器写入对应位实现（写1清除）
  */
void LTDC_ClearFlag(uint32_t LTDC_FLAG)
{
  /* 参数有效性校验 */
  assert_param(IS_LTDC_FLAG(LTDC_FLAG)); // 验证标志位参数有效性

  /* 写入中断清除寄存器（ICR）触发标志清除 */
  LTDC->ICR = (uint32_t)LTDC_FLAG;  // 向ICR写入对应位会自动清除ISR中的标志位
}

/**
  * @brief  获取LTDC中断状态
  * @details 该函数用于检测有效中断状态，包含双重检查：
  *          - 检查中断标志是否置位（ISR寄存器）
  *          - 检查中断是否使能（IER寄存器）
  * @param  LTDC_IT: 要查询的中断类型，取值：
  *     @arg LTDC_IT_LI:   行中断
  *     @arg LTDC_IT_FU:   FIFO欠载中断
  *     @arg LTDC_IT_TERR: 传输错误中断
  *     @arg LTDC_IT_RR:   寄存器重载中断
  * @retval 中断状态：
  *     SET   - 中断有效（标志位置位且中断使能）
  *     RESET - 中断无效
  */
ITStatus LTDC_GetITStatus(uint32_t LTDC_IT)
{
  ITStatus bitstatus = RESET;

  /* 参数有效性校验 */
  assert_param(IS_LTDC_IT(LTDC_IT)); // 验证中断类型参数有效性

  /* 第一阶段：检查中断标志位 */
  if ((LTDC->ISR & LTDC_IT) != (uint32_t)RESET)
  {
    bitstatus = SET;   // 中断标志位置位
  }
  else
  {
    bitstatus = RESET; // 中断标志位未置位
  }

  /* 第二阶段：验证中断使能状态 */
  if (((LTDC->IER & LTDC_IT) != (uint32_t)RESET) && // 检查IER使能位
      (bitstatus != (uint32_t)RESET))                // 且ISR标志位置位
  {
    bitstatus = SET;   // 双重条件满足返回SET
  }
  else
  {
    bitstatus = RESET; // 任一条件不满足返回RESET
  }
  return bitstatus;
}

/**
  * @brief  清除LTDC中断挂起标志
  * @details 该函数用于清除指定的中断挂起状态，包含以下操作：
  *          - 参数有效性校验（通过assert_param实现）
  *          - 写中断清除寄存器ICR触发标志清除
  * @param  LTDC_IT: 要清除的中断类型，支持位或组合：
  *     @arg LTDC_IT_LIE:    行中断（水平同步期间触发）
  *     @arg LTDC_IT_FUIE:   FIFO欠载中断（像素传输速率超过总线带宽时触发）
  *     @arg LTDC_IT_TERRIE: 传输错误中断（DMA传输异常时触发）
  *     @arg LTDC_IT_RRIE:   寄存器重载中断（影子寄存器重载完成时触发）
  * @retval 无
  * @note 重要说明：
  *        - 清除操作通过向ICR寄存器写入对应位实现（写1清除机制）
  *        - 该操作会同时清除ISR中的标志位和IER中的中断挂起状态
  *        - 应在中断服务程序中进行清除操作
  */
void LTDC_ClearITPendingBit(uint32_t LTDC_IT)
{
  /* 参数有效性校验 */
  // 使用IS_LTDC_IT宏验证输入参数的合法性，确保不超出允许的中断类型范围
  assert_param(IS_LTDC_IT(LTDC_IT));

  /* 清除对应中断标志 */
  // 通过写入ICR寄存器实现标志清除：
  // - ICR寄存器采用写1清除机制
  // - 写入值应与中断标志位模式匹配
  // - 该操作具有原子性，立即生效
  LTDC->ICR = (uint32_t)LTDC_IT;  
}

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
  ******************************************************************************
  * @file    stm32f4xx_dma2d.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the DMA2D controller (DMA2D) peripheral:
  *           + Initialization and configuration
  *           + Interrupts and flags management
  *           
  @verbatim  
 ===============================================================================
                      ##### How to use this driver #####
 ===============================================================================
    [..]
        (#) Enable DMA2D clock using 
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_DMA2D, ENABLE) function.
            
        (#) Configures DMA2D
          (++) transfer mode 
          (++) pixel format, line_number, pixel_per_line
          (++) output memory address
          (++) alpha value
          (++) output offset
          (++) Default color (RGB)
           
        (#) Configures Foreground or/and background
          (++) memory address
          (++) alpha value
          (++) offset and default color
  
        (#) Call the DMA2D_Start() to enable the DMA2D controller.
        
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
#include "stm32f4xx_dma2d.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup DMA2D 
  * @brief DMA2D driver modules
  * @{
  */
/* DMA2D模块私有宏定义 -------------------------------------------------------*/
/**
  * @defgroup DMA2D_Private_Constants DMA2D模块私有常量
  * @brief 寄存器位操作掩码定义，用于安全配置DMA2D寄存器
  * @note 这些掩码根据STM32F4xx参考手册寄存器描述精确定义
  */

/* 控制寄存器（CR）位掩码
   保留可写位：
   - MODE[1:0]:   DMA2D工作模式选择位（00=存储器到存储器，01=寄存器到存储器等）
   - CEIE/TEIE/CTCIE/CAEIE/CWIE: 各种中断使能位
   - START: 传输启动位
   屏蔽保留位：0xFFFCE0FC 对应保留位清零保护 */
#define CR_MASK                     ((uint32_t)0xFFFCE0FC)  

/* 前景层颜色格式配置寄存器（FGPFCCR）位掩码 
   保留可写位：
   - CM[3:0]:    颜色模式选择位（RGB565, ARGB8888等）
   - AM[1:0]:    Alpha混合模式选择位
   - ALPHA:      固定Alpha值设置位
   屏蔽保留位：0x00FC00C0 确保不修改：
   - 保留位[31:24] 
   - 保留位[15:8]
   - 保留位[6:0] */
#define PFCCR_MASK                  ((uint32_t)0x00FC00C0)  

/* 目标地址寄存器（OOR）位掩码
   保留可写位：
   - LO[13:0]:  行偏移量配置位（单位：像素）
   屏蔽保留位：0xFFFF00FE 用于：
   - 保留地址对齐位[31:16] 
   - 保留位[7:1] */
#define DEAD_MASK                   ((uint32_t)0xFFFF00FE)


/** @defgroup DMA2D_Private_Functions
  * @{
  */

/** @defgroup DMA2D_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim
 ===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the DMA2D
      (+) Start/Abort/Suspend Transfer
      (+) Initialize, configure and set Foreground and background
      (+) configure and enable DeadTime
      (+) configure lineWatermark
    
    
@endverbatim
  * @{
  */
/**
  * @brief  DMA2D外设反初始化函数
  * @功能   通过硬件复位将DMA2D寄存器恢复至默认状态
  * @原理   控制RCC的AHB1复位寄存器实现硬件级复位
  * @注意   - 复位期间DMA2D无法访问
  *         - 复位完成后需重新初始化才能使用
  * @参数   None
  * @retval None
  */
void DMA2D_DeInit(void)
{
  /* 使能DMA2D硬件复位（触发复位电路） */
  RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_DMA2D, ENABLE);
  /* 释放DMA2D复位（复位电路恢复常态） */
  RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_DMA2D, DISABLE);
}

/**
  * @brief  DMA2D外设初始化函数
  * @功能   配置DMA2D工作模式、输出格式及图像参数
  * @参数   DMA2D_InitStruct 初始化结构体指针，包含：
  *         - Mode: 传输模式（M2M/M2M_PFC等）
  *         - CMode: 输出颜色格式（RGB565/ARGB8888等）
  *         - 输出颜色分量（红/绿/蓝/Alpha）
  *         - 输出内存地址、行偏移、分辨率等
  * @注意   - 必须在DMA2D禁用状态下调用
  *         - 输出地址需按颜色格式对齐（ARGB8888需4字节对齐）
  */
void DMA2D_Init(DMA2D_InitTypeDef* DMA2D_InitStruct)
{
  uint32_t outgreen = 0;
  uint32_t outred   = 0;
  uint32_t outalpha = 0;
  uint32_t pixline  = 0;

  /* 参数合法性验证 */
  assert_param(IS_DMA2D_MODE(DMA2D_InitStruct->DMA2D_Mode));        // 模式检查
  assert_param(IS_DMA2D_CMODE(DMA2D_InitStruct->DMA2D_CMode));     // 颜色格式
  assert_param(IS_DMA2D_OGREEN(DMA2D_InitStruct->DMA2D_OutputGreen)); // 绿色分量范围
  assert_param(IS_DMA2D_ORED(DMA2D_InitStruct->DMA2D_OutputRed));    // 红色分量范围
  assert_param(IS_DMA2D_OBLUE(DMA2D_InitStruct->DMA2D_OutputBlue));   // 蓝色分量范围
  assert_param(IS_DMA2D_OALPHA(DMA2D_InitStruct->DMA2D_OutputAlpha)); // Alpha值范围
  assert_param(IS_DMA2D_OUTPUT_OFFSET(DMA2D_InitStruct->DMA2D_OutputOffset)); // 行偏移限制
  assert_param(IS_DMA2D_LINE(DMA2D_InitStruct->DMA2D_NumberOfLine));  // 行数限制（0-FFFF）
  assert_param(IS_DMA2D_PIXEL(DMA2D_InitStruct->DMA2D_PixelPerLine)); // 像素/行限制（0-3FFF）

  /* 配置CR寄存器（控制寄存器） */
  DMA2D->CR &= CR_MASK;            // 保留位清零
  DMA2D->CR |= DMA2D_InitStruct->DMA2D_Mode; // 设置工作模式

  /* 配置OPFCCR寄存器（输出颜色格式） */
  DMA2D->OPFCCR &= ~DMA2D_OPFCCR_CM;  // 清除原有颜色模式
  DMA2D->OPFCCR |= DMA2D_InitStruct->DMA2D_CMode; // 设置新颜色模式

  /* 根据颜色格式处理输出颜色分量 */
  switch(DMA2D_InitStruct->DMA2D_CMode) {
    case DMA2D_ARGB8888:  // 32位真彩色带Alpha
      outgreen = DMA2D_InitStruct->DMA2D_OutputGreen << 8;   // 绿色分量在8-15位
      outred = DMA2D_InitStruct->DMA2D_OutputRed << 16;      // 红色分量在16-23位
      outalpha = DMA2D_InitStruct->DMA2D_OutputAlpha << 24;  // Alpha在24-31位
      break;
    case DMA2D_RGB888:     // 24位真彩色
      outgreen = DMA2D_InitStruct->DMA2D_OutputGreen << 8;
      outred = DMA2D_InitStruct->DMA2D_OutputRed << 16;
      break;
    case DMA2D_RGB565:     // 16位高彩色
      outgreen = DMA2D_InitStruct->DMA2D_OutputGreen << 5;  // 绿色5位（5-10位）
      outred = DMA2D_InitStruct->DMA2D_OutputRed << 11;       // 红色5位（11-15位）
      break;
    case DMA2D_ARGB1555:   // 16位带1位Alpha
      outgreen = DMA2D_InitStruct->DMA2D_OutputGreen << 5;  
      outred = DMA2D_InitStruct->DMA2D_OutputRed << 10;
      outalpha = DMA2D_InitStruct->DMA2D_OutputAlpha << 15;   // Alpha在最高位
      break;
    default: /* DMA2D_ARGB4444 */ // 16位带4位Alpha
      outgreen = DMA2D_InitStruct->DMA2D_OutputGreen << 4;    // 绿色4位（4-7位）
      outred = DMA2D_InitStruct->DMA2D_OutputRed << 8;        // 红色4位（8-11位）
      outalpha = DMA2D_InitStruct->DMA2D_OutputAlpha << 12;  // Alpha在12-15位
  }
  /* 写入OCOLR寄存器（输出颜色寄存器） */
  DMA2D->OCOLR = outgreen | outred | DMA2D_InitStruct->DMA2D_OutputBlue | outalpha;

  /* 配置输出内存地址（OMAR寄存器） */
  DMA2D->OMAR = DMA2D_InitStruct->DMA2D_OutputMemoryAdd;  // 需满足地址对齐要求

  /* 配置行偏移（OOR寄存器） */
  DMA2D->OOR = DMA2D_InitStruct->DMA2D_OutputOffset;      // 行间像素偏移量

  /* 配置分辨率参数（NLR寄存器） */
  pixline = DMA2D_InitStruct->DMA2D_PixelPerLine << 16;    // 每行像素数存高16位
  DMA2D->NLR = DMA2D_InitStruct->DMA2D_NumberOfLine | pixline; // 行数存低16位
}
/**
  * @brief  DMA2D配置结构体默认值初始化函数
  * @功能   将DMA2D_InitTypeDef结构体成员初始化为安全默认值
  * @应用   防止未初始化结构体导致的配置错误
  * @注意   初始化后的结构体需根据实际需求修改成员值
  * @param  DMA2D_InitStruct 要初始化的DMA2D配置结构体指针
  * @retval None
  */
void DMA2D_StructInit(DMA2D_InitTypeDef* DMA2D_InitStruct)
{
  /* 默认传输模式：存储器到存储器（无颜色转换） */
  DMA2D_InitStruct->DMA2D_Mode = DMA2D_M2M;

  /* 默认输出格式：32位ARGB8888（兼容大多数场景） */
  DMA2D_InitStruct->DMA2D_CMode = DMA2D_ARGB8888;

  /* 颜色分量清零（黑色全透明） */
  DMA2D_InitStruct->DMA2D_OutputGreen = 0x00;
  DMA2D_InitStruct->DMA2D_OutputBlue = 0x00;
  DMA2D_InitStruct->DMA2D_OutputRed = 0x00;
  DMA2D_InitStruct->DMA2D_OutputAlpha = 0x00;

  /* 输出地址清零（必须显式配置有效地址） */
  DMA2D_InitStruct->DMA2D_OutputMemoryAdd = 0x00;

  /* 行偏移清零（连续存储模式） */
  DMA2D_InitStruct->DMA2D_OutputOffset = 0x00;

  /* 分辨率参数清零（必须显式配置） */
  DMA2D_InitStruct->DMA2D_NumberOfLine = 0x00;  // 行数
  DMA2D_InitStruct->DMA2D_PixelPerLine = 0x00;  // 每行像素数
}

/**
  * @brief  DMA2D传输启动函数
  * @功能   触发DMA2D开始图形数据传输
  * @原理   设置CR寄存器的START位（bit0），启动后硬件自动清除该位
  * @注意   - 调用前必须完成DMA2D初始化
  *         - 启动后需等待传输完成标志或使用中断
  * @retval None
  */
void DMA2D_StartTransfer(void)
{
    /* 设置CR[0] START位启动传输 
       （硬件传输完成后自动清零该位） */
    DMA2D->CR |= DMA2D_CR_START;
}

/**
  * @brief  DMA2D传输中止函数
  * @功能   强制终止正在进行的DMA2D传输
  * @原理   设置CR寄存器的ABORT位（bit30），立即停止传输
  * @注意   - 中止操作不可逆，可能造成目标数据不完整
  *         - 中止后建议重新初始化DMA2D配置
  * @retval None
  */
void DMA2D_AbortTransfer(void)
{
    /* 设置CR[30] ABORT位中止传输 
       （需手动清除该位或复位DMA2D） */
    DMA2D->CR |= DMA2D_CR_ABORT;
}

/**
  * @brief  DMA2D传输暂停/恢复控制函数
  * @功能   临时暂停或恢复DMA2D传输
  * @原理   通过CR寄存器的SUSP位（bit1）控制传输状态
  * @应用   需要总线带宽优先处理其他外设时暂停传输
  * @param  NewState 传输状态：ENABLE(暂停)/DISABLE(恢复)
  * @注意   - 暂停期间DMA2D保持当前状态
  *         - 多次暂停需对应多次恢复才能继续传输
  * @retval None
  */
void DMA2D_Suspend(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE) {
    /* 设置CR[1] SUSP位暂停传输（保持现场） */
    DMA2D->CR |= DMA2D_CR_SUSP;
  } else {
    /* 清除CR[1] SUSP位恢复传输 */
    DMA2D->CR &= ~DMA2D_CR_SUSP;
  }
}
/**
  * @brief  前景图层配置函数
  * @功能   配置DMA2D前景图层的存储参数、颜色格式及混合模式
  * @参数   DMA2D_FG_InitStruct 前景配置结构体，包含：
  *         - FGMA: 前景数据内存地址
  *         - FGO: 行偏移量（像素单位）
  *         - FGCM: 前景颜色模式（RGB565/ARGB8888等）
  *         - CLUT配置：颜色表模式、大小、内存地址
  *         - Alpha混合模式及数值
  *         - 默认颜色值（用于无Alpha通道格式）
  * @注意   - 必须在DMA2D传输停止状态下调用
  *         - CLUT地址需按颜色模式对齐（ARGB8888需4字节对齐）
  */
void DMA2D_FGConfig(DMA2D_FG_InitTypeDef* DMA2D_FG_InitStruct)
{
  uint32_t fg_clutcolormode = 0;
  uint32_t fg_clutsize = 0;
  uint32_t fg_alpha_mode = 0;
  uint32_t fg_alphavalue = 0;
  uint32_t fg_colorgreen = 0;
  uint32_t fg_colorred = 0;

  /* 参数合法性验证（范围检查） */
  assert_param(IS_DMA2D_FGO(DMA2D_FG_InitStruct->DMA2D_FGO));          // 行偏移范围0-3FFF
  assert_param(IS_DMA2D_FGCM(DMA2D_FG_InitStruct->DMA2D_FGCM));        // 颜色模式检查
  assert_param(IS_DMA2D_FG_CLUT_CM(DMA2D_FG_InitStruct->DMA2D_FG_CLUT_CM)); // CLUT颜色模式
  assert_param(IS_DMA2D_FG_CLUT_SIZE(DMA2D_FG_InitStruct->DMA2D_FG_CLUT_SIZE)); // CLUT大小0-255
  assert_param(IS_DMA2D_FG_ALPHA_MODE(DMA2D_FG_InitStruct->DMA2D_FGPFC_ALPHA_MODE)); // Alpha模式
  assert_param(IS_DMA2D_FG_ALPHA_VALUE(DMA2D_FG_InitStruct->DMA2D_FGPFC_ALPHA_VALUE)); // Alpha值0-255
  assert_param(IS_DMA2D_FGC_BLUE(DMA2D_FG_InitStruct->DMA2D_FGC_BLUE));    // 蓝色分量范围
  assert_param(IS_DMA2D_FGC_GREEN(DMA2D_FG_InitStruct->DMA2D_FGC_GREEN));  // 绿色分量范围
  assert_param(IS_DMA2D_FGC_RED(DMA2D_FG_InitStruct->DMA2D_FGC_RED));     // 红色分量范围

  /* 设置前景数据内存地址（FGMAR寄存器） */
  DMA2D->FGMAR = DMA2D_FG_InitStruct->DMA2D_FGMA;  // 需满足颜色格式对齐要求

  /* 配置前景行偏移（FGOR寄存器，低14位有效） */
  DMA2D->FGOR = (DMA2D->FGOR & ~DMA2D_FGOR_LO) | DMA2D_FG_InitStruct->DMA2D_FGO;

  /* 配置前景像素格式转换器（FGPFCCR寄存器） */
  DMA2D->FGPFCCR &= PFCCR_MASK;  // 保留位清零
  fg_clutcolormode = DMA2D_FG_InitStruct->DMA2D_FG_CLUT_CM << 4;   // CLUT颜色模式[7:4]
  fg_clutsize = DMA2D_FG_InitStruct->DMA2D_FG_CLUT_SIZE << 8;      // CLUT大小[15:8]
  fg_alpha_mode = DMA2D_FG_InitStruct->DMA2D_FGPFC_ALPHA_MODE << 16; // Alpha模式[19:16]
  fg_alphavalue = DMA2D_FG_InitStruct->DMA2D_FGPFC_ALPHA_VALUE << 24; // Alpha值[31:24]
  DMA2D->FGPFCCR |= (DMA2D_FG_InitStruct->DMA2D_FGCM | fg_clutcolormode | 
                    fg_clutsize | fg_alpha_mode | fg_alphavalue);

  /* 配置前景默认颜色（FGCOLR寄存器） */
  fg_colorgreen = DMA2D_FG_InitStruct->DMA2D_FGC_GREEN << 8;  // 绿色分量[15:8]
  fg_colorred = DMA2D_FG_InitStruct->DMA2D_FGC_RED << 16;    // 红色分量[23:16]
  DMA2D->FGCOLR = DMA2D_FG_InitStruct->DMA2D_FGC_BLUE | fg_colorgreen | fg_colorred;

  /* 配置CLUT内存地址（FGCMAR寄存器） */
  DMA2D->FGCMAR = DMA2D_FG_InitStruct->DMA2D_FGCMAR;  // CLUT表起始地址
}

/**
  * @brief  前景配置结构体初始化函数
  * @功能   重置DMA2D前景配置参数为安全默认值
  * @参数   DMA2D_FG_InitStruct 待初始化的前景配置结构体
  * @注意   - 默认使用ARGB8888格式，透明黑色
  *         - CLUT功能默认关闭
  */
void DMA2D_FG_StructInit(DMA2D_FG_InitTypeDef* DMA2D_FG_InitStruct)
{
  /* 前景数据地址清零（必须显式配置） */
  DMA2D_FG_InitStruct->DMA2D_FGMA = 0x00;

  /* 行偏移清零（连续存储） */
  DMA2D_FG_InitStruct->DMA2D_FGO = 0x00;

  /* 默认颜色格式：32位带透明度（兼容性强） */
  DMA2D_FG_InitStruct->DMA2D_FGCM = CM_ARGB8888;

  /* CLUT默认配置：ARGB8888格式，颜色表大小0 */
  DMA2D_FG_InitStruct->DMA2D_FG_CLUT_CM = CLUT_CM_ARGB8888;
  DMA2D_FG_InitStruct->DMA2D_FG_CLUT_SIZE = 0x00;

  /* Alpha混合默认：不修改Alpha值（使用原始像素数据） */
  DMA2D_FG_InitStruct->DMA2D_FGPFC_ALPHA_MODE = NO_MODIF_ALPHA_VALUE;
  DMA2D_FG_InitStruct->DMA2D_FGPFC_ALPHA_VALUE = 0x00;

  /* 默认颜色值：黑色全透明 */
  DMA2D_FG_InitStruct->DMA2D_FGC_BLUE = 0x00;
  DMA2D_FG_InitStruct->DMA2D_FGC_GREEN = 0x00;
  DMA2D_FG_InitStruct->DMA2D_FGC_RED = 0x00;

  /* CLUT内存地址清零 */
  DMA2D_FG_InitStruct->DMA2D_FGCMAR = 0x00;
}

/**
  * @brief  背景图层配置函数
  * @功能   配置DMA2D背景图层的存储参数、颜色格式及混合模式
  * @参数   DMA2D_BG_InitStruct 背景配置结构体，包含：
  *         - BGMA: 背景数据内存地址
  *         - BGO: 行偏移量（像素单位）
  *         - BGCM: 背景颜色模式（RGB565/ARGB8888等）
  *         - CLUT配置：颜色表模式、大小、内存地址
  *         - Alpha混合模式及数值
  *         - 默认颜色值（用于无Alpha通道格式）
  * @注意   - 必须在DMA2D传输停止状态下调用
  *         - CLUT地址需按颜色模式对齐（ARGB8888需4字节对齐）
  */
void DMA2D_BGConfig(DMA2D_BG_InitTypeDef* DMA2D_BG_InitStruct)
{
  uint32_t bg_clutcolormode = 0;
  uint32_t bg_clutsize = 0;
  uint32_t bg_alpha_mode = 0;
  uint32_t bg_alphavalue = 0;
  uint32_t bg_colorgreen = 0;
  uint32_t bg_colorred = 0;

  /* 参数合法性验证（范围检查） */
  assert_param(IS_DMA2D_BGO(DMA2D_BG_InitStruct->DMA2D_BGO));          // 行偏移范围0-3FFF
  assert_param(IS_DMA2D_BGCM(DMA2D_BG_InitStruct->DMA2D_BGCM));        // 颜色模式检查
  assert_param(IS_DMA2D_BG_CLUT_CM(DMA2D_BG_InitStruct->DMA2D_BG_CLUT_CM)); // CLUT颜色模式
  assert_param(IS_DMA2D_BG_CLUT_SIZE(DMA2D_BG_InitStruct->DMA2D_BG_CLUT_SIZE)); // CLUT大小0-255
  assert_param(IS_DMA2D_BG_ALPHA_MODE(DMA2D_BG_InitStruct->DMA2D_BGPFC_ALPHA_MODE)); // Alpha模式
  assert_param(IS_DMA2D_BG_ALPHA_VALUE(DMA2D_BG_InitStruct->DMA2D_BGPFC_ALPHA_VALUE)); // Alpha值0-255
  assert_param(IS_DMA2D_BGC_BLUE(DMA2D_BG_InitStruct->DMA2D_BGC_BLUE));    // 蓝色分量范围
  assert_param(IS_DMA2D_BGC_GREEN(DMA2D_BG_InitStruct->DMA2D_BGC_GREEN));  // 绿色分量范围
  assert_param(IS_DMA2D_BGC_RED(DMA2D_BG_InitStruct->DMA2D_BGC_RED));     // 红色分量范围

  /* 设置背景数据内存地址（BGMAR寄存器） */
  DMA2D->BGMAR = DMA2D_BG_InitStruct->DMA2D_BGMA;  // 需满足颜色格式对齐要求

  /* 配置背景行偏移（BGOR寄存器，低14位有效） */
  DMA2D->BGOR = (DMA2D->BGOR & ~DMA2D_BGOR_LO) | DMA2D_BG_InitStruct->DMA2D_BGO;

  /* 配置背景像素格式转换器（BGPFCCR寄存器） */
  DMA2D->BGPFCCR &= PFCCR_MASK;  // 保留位清零
  bg_clutcolormode = DMA2D_BG_InitStruct->DMA2D_BG_CLUT_CM << 4;   // CLUT颜色模式[7:4]
  bg_clutsize = DMA2D_BG_InitStruct->DMA2D_BG_CLUT_SIZE << 8;      // CLUT大小[15:8]
  bg_alpha_mode = DMA2D_BG_InitStruct->DMA2D_BGPFC_ALPHA_MODE << 16; // Alpha模式[19:16]
  bg_alphavalue = DMA2D_BG_InitStruct->DMA2D_BGPFC_ALPHA_VALUE << 24; // Alpha值[31:24]
  DMA2D->BGPFCCR |= (DMA2D_BG_InitStruct->DMA2D_BGCM | bg_clutcolormode | 
                    bg_clutsize | bg_alpha_mode | bg_alphavalue);

  /* 配置背景默认颜色（BGCOLR寄存器） */
  bg_colorgreen = DMA2D_BG_InitStruct->DMA2D_BGC_GREEN << 8;  // 绿色分量[15:8]
  bg_colorred = DMA2D_BG_InitStruct->DMA2D_BGC_RED << 16;    // 红色分量[23:16]
  DMA2D->BGCOLR = DMA2D_BG_InitStruct->DMA2D_BGC_BLUE | bg_colorgreen | bg_colorred;

  /* 配置CLUT内存地址（BGCMAR寄存器） */
  DMA2D->BGCMAR = DMA2D_BG_InitStruct->DMA2D_BGCMAR;  // CLUT表起始地址
}

/**
  * @brief  背景配置结构体初始化函数
  * @功能   重置DMA2D背景配置参数为安全默认值
  * @参数   DMA2D_BG_InitStruct 待初始化的背景配置结构体
  * @注意   - 默认使用ARGB8888格式，透明黑色
  *         - CLUT功能默认关闭
  */
void DMA2D_BG_StructInit(DMA2D_BG_InitTypeDef* DMA2D_BG_InitStruct)
{
  /* 背景数据地址清零（必须显式配置） */
  DMA2D_BG_InitStruct->DMA2D_BGMA = 0x00;

  /* 行偏移清零（连续存储） */
  DMA2D_BG_InitStruct->DMA2D_BGO = 0x00;

  /* 默认颜色格式：32位带透明度（兼容性强） */
  DMA2D_BG_InitStruct->DMA2D_BGCM = CM_ARGB8888;

  /* CLUT默认配置：ARGB8888格式，颜色表大小0 */
  DMA2D_BG_InitStruct->DMA2D_BG_CLUT_CM = CLUT_CM_ARGB8888;
  DMA2D_BG_InitStruct->DMA2D_BG_CLUT_SIZE = 0x00;

  /* Alpha混合默认：不修改Alpha值（使用原始像素数据） */
  DMA2D_BG_InitStruct->DMA2D_BGPFC_ALPHA_MODE = NO_MODIF_ALPHA_VALUE;
  DMA2D_BG_InitStruct->DMA2D_BGPFC_ALPHA_VALUE = 0x00;

  /* 默认颜色值：黑色全透明 */
  DMA2D_BG_InitStruct->DMA2D_BGC_BLUE = 0x00;
  DMA2D_BG_InitStruct->DMA2D_BGC_GREEN = 0x00;
  DMA2D_BG_InitStruct->DMA2D_BGC_RED = 0x00;

  /* CLUT内存地址清零 */
  DMA2D_BG_InitStruct->DMA2D_BGCMAR = 0x00;
}
/**
  * @brief  启动或停止前景层CLUT（颜色查找表）的自动加载
  * @param  NewState: 外围设备状态控制
  *   @arg ENABLE:  启动CLUT自动加载
  *   @arg DISABLE: 终止当前传输操作
  * @retval 无
  * @note   该函数通过修改FGPFCCR寄存器的START位来控制DMA2D前景层操作，
  *         当启用时会触发硬件自动加载CLUT颜色表
  */
void DMA2D_FGStart(FunctionalState NewState) 
{
  /* 参数有效性检查：验证输入是否为合法的功能状态（ENABLE/DISABLE） */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 状态判断分支 */
  if (NewState != DISABLE)
  {
    /* 设置FGPFCCR寄存器的START位（第5位）为1
       启动CLUT自动加载操作 */
    DMA2D->FGPFCCR |= DMA2D_FGPFCCR_START;
  }
  else
  {
    /* 清除FGPFCCR寄存器的START位（第5位）为0
       终止当前CLUT传输操作 */
    DMA2D->FGPFCCR &= (uint32_t)~DMA2D_FGPFCCR_START;
  }
}

/**
  * @brief  启动或停止背景层CLUT（颜色查找表）的自动加载
  * @param  NewState: 外围设备状态控制
  *   @arg ENABLE:  启动CLUT自动加载
  *   @arg DISABLE: 终止当前传输操作
  * @retval 无
  * @note   该函数通过修改BGPFCCR寄存器的START位来控制DMA2D背景层操作，
  *         功能与前景层控制相似但作用于不同的硬件寄存器
  */
void DMA2D_BGStart(FunctionalState NewState) 
{
  /* 参数有效性检查：验证输入是否为合法的功能状态（ENABLE/DISABLE） */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 设置BGPFCCR寄存器的START位（第5位）为1
       启动背景层CLUT加载 */
    DMA2D->BGPFCCR |= DMA2D_BGPFCCR_START;
  }
  else
  {
    /* 清除BGPFCCR寄存器的START位（第5位）为0
       终止背景层传输 */
    DMA2D->BGPFCCR &= (uint32_t)~DMA2D_BGPFCCR_START;
  }
}

/**
  * @brief  配置DMA2D死区时间（显示刷新间隔）
  * @param  DMA2D_DeadTime: 死区时间值，范围0x0-0xFF（8位有效值）
  * @param  NewState: 功能使能状态
  *   @arg ENABLE:  启用指定死区时间
  *   @arg DISABLE: 关闭死区时间功能
  * @retval 无
  * @note   死区时间用于控制两次DMA2D操作之间的最小间隔，
  *         实际时间值 = (DMA2D_DeadTime + 1) × 16个DMA2D时钟周期
  */
void DMA2D_DeadTimeConfig(uint32_t DMA2D_DeadTime, FunctionalState NewState)
{
   uint32_t DeadTime;

  /* 参数有效性检查：验证时间值是否在8位范围内 */
  assert_param(IS_DMA2D_DEAD_TIME(DMA2D_DeadTime));
  /* 验证功能状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 清除AMTCR寄存器原有配置（保留低8位，清除高24位）*/
    DMA2D->AMTCR &= (uint32_t)DEAD_MASK;
    
    /* 将输入时间值左移8位对齐到寄存器的高24位区域
       AMTCR寄存器结构：[31:8]-死区时间值 [0]-使能位 */
    DeadTime = DMA2D_DeadTime << 8;
    
    /* 组合时间值和使能位写入寄存器 */
    DMA2D->AMTCR |= (DeadTime | DMA2D_AMTCR_EN);
  }
  else
  {
     /* 关闭死区时间功能：清除使能位（第0位）*/
     DMA2D->AMTCR &= ~(uint32_t)DMA2D_AMTCR_EN;
  }
}

/**
  * @brief  配置DMA2D行水印（Line Watermark）
  * @param  DMA2D_LWatermarkConfig: 水印值配置
  *   @arg 范围0x2-0x7FF（11位有效值）
  * @retval 无
  * @note   行水印用于设置DMA2D开始传输前需要缓存的完整行数，
  *         该值影响流水线效率和内存带宽使用
  */
void DMA2D_LineWatermarkConfig(uint32_t DMA2D_LWatermarkConfig)
{
  /* 参数有效性检查：验证输入值是否符合水印范围要求 */
  assert_param(IS_DMA2D_LineWatermark(DMA2D_LWatermarkConfig));

  /* 直接将配置值写入LWR寄存器
     LWR寄存器结构：[10:0]-行水印值 */
  DMA2D->LWR = (uint32_t)DMA2D_LWatermarkConfig;
}


/**
  * @}
  */

/** @defgroup DMA2D_Group2 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim
 ===============================================================================
            ##### Interrupts and flags management functions #####
 ===============================================================================

    [..] This section provides functions allowing to configure the DMA2D 
         Interrupts and to get the status and clear flags and Interrupts 
         pending bits.
    [..] The DMA2D provides 6 Interrupts sources and 6 Flags
    
    *** Flags ***
    =============
    [..]
      (+) DMA2D_FLAG_CE : Configuration Error Interrupt flag
      (+) DMA2D_FLAG_CAE: CLUT Access Error Interrupt flag
      (+) DMA2D_FLAG_TW:  Transfer Watermark Interrupt flag
      (+) DMA2D_FLAG_TC:  Transfer Complete interrupt flag
      (+) DMA2D_FLAG_TE:  Transfer Error interrupt flag
      (+) DMA2D_FLAG_CTC: CLUT Transfer Complete Interrupt flag
      
    *** Interrupts ***
    ==================
    [..]
      (+) DMA2D_IT_CE: Configuration Error Interrupt is generated when a wrong 
                       configuration is detected
      (+) DMA2D_IT_CAE: CLUT Access Error Interrupt
      (+) DMA2D_IT_TW: Transfer Watermark Interrupt is generated when 
                       the programmed watermark is reached 
      (+) DMA2D_IT_TE: Transfer Error interrupt is generated when the CPU trying 
                       to access the CLUT while a CLUT loading or a DMA2D1 transfer 
                       is on going       
      (+) DMA2D_IT_CTC: CLUT Transfer Complete Interrupt 
      (+) DMA2D_IT_TC: Transfer Complete interrupt         
@endverbatim
  * @{
  */
/**
  * @brief  Enables or disables the specified DMA2D's interrupts.
  * @param  DMA2D_IT: specifies the DMA2D interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg DMA2D_IT_CE:   Configuration Error Interrupt Enable.
  *     @arg DMA2D_IT_CTC:  CLUT Transfer Complete Interrupt Enable.
  *     @arg DMA2D_IT_CAE:  CLUT Access Error Interrupt Enable.
  *     @arg DMA2D_IT_TW:   Transfer Watermark Interrupt Enable.
  *     @arg DMA2D_IT_TC:   Transfer Complete interrupt enable.
  *     @arg DMA2D_IT_TE:   Transfer Error interrupt enable.
  * @param NewState: new state of the specified DMA2D interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  DMA2D中断使能配置函数
  * @param  DMA2D_IT: 需要配置的DMA2D中断类型
  *   @arg 可取值（按位组合）：
  *     DMA2D_IT_CE   配置错误中断
  *     DMA2D_IT_CTC  CLUT传输完成中断
  *     DMA2D_IT_CAE  CLUT访问错误中断
  *     DMA2D_IT_TW   传输水印中断
  *     DMA2D_IT_TC   传输完成中断
  *     DMA2D_IT_TE   传输错误中断
  * @param  NewState: 中断状态控制
  *   @arg ENABLE:  开启指定中断
  *   @arg DISABLE: 关闭指定中断
  * @retval 无
  * @note   该函数通过修改CR寄存器的中断使能位实现控制，
  *         支持多个中断类型的位掩码组合配置
  */
void DMA2D_ITConfig(uint32_t DMA2D_IT, FunctionalState NewState)
{
  /* 参数有效性校验：验证输入的中断类型是否合法 */
  assert_param(IS_DMA2D_IT(DMA2D_IT));
  /* 验证功能状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 使用位或操作设置CR寄存器的中断使能位
       当NewState为ENABLE时，开启指定中断通道 */
    DMA2D->CR |= DMA2D_IT;
  }
  else
  {
    /* 使用位与操作清除CR寄存器的中断使能位
       当NewState为DISABLE时，关闭指定中断通道 */
    DMA2D->CR &= (uint32_t)~DMA2D_IT;
  }
}

/**
  * @brief  获取DMA2D状态标志位
  * @param  DMA2D_FLAG: 需要查询的状态标志
  *   @arg 可取值：
  *     DMA2D_FLAG_CE  配置错误标志（ISR[0]）
  *     DMA2D_FLAG_CTC CLUT传输完成标志（ISR[1]）
  *     DMA2D_FLAG_CAE CLUT访问错误标志（ISR[2]）
  *     DMA2D_FLAG_TW  传输水印标志（ISR[3]）
  *     DMA2D_FLAG_TC  传输完成标志（ISR[4]）
  *     DMA2D_FLAG_TE  传输错误标志（ISR[5]）
  * @retval FlagStatus 标志状态
  *   @arg SET:   标志位置位（事件已发生）
  *   @arg RESET: 标志位复位（事件未发生）
  * @note   通过读取ISR寄存器状态位实现，
  *         各标志位在事件发生时由硬件自动置位
  */
FlagStatus DMA2D_GetFlagStatus(uint32_t DMA2D_FLAG)
{
  FlagStatus bitstatus = RESET;
  
  /* 验证输入标志类型是否合法 */
  assert_param(IS_DMA2D_GET_FLAG(DMA2D_FLAG));
  
  /* 读取ISR寄存器并与目标标志位进行位与运算 */
  if (((DMA2D->ISR) & DMA2D_FLAG) != (uint32_t)RESET)
  {
    /* 当计算结果非零时，表示目标标志位已置位 */
    bitstatus = SET;
  }
  else
  {
    /* 计算结果为零，标志位保持复位状态 */
    bitstatus = RESET;
  }
  /* 返回当前标志位状态 */
  return bitstatus;
}

/**
  * @brief  清除DMA2D状态标志位
  * @param  DMA2D_FLAG: 需要清除的标志位组合
  *   @arg 可取值（按位或组合）：
  *     DMA2D_FLAG_CE | DMA2D_FLAG_CTC | DMA2D_FLAG_CAE |
  *     DMA2D_FLAG_TW | DMA2D_FLAG_TC | DMA2D_FLAG_TE
  * @retval 无
  * @note   通过写IFCR寄存器实现标志清除，
  *         写入的值会直接清除对应的ISR状态位，
  *         需要特别注意该操作具有"写1清零"特性
  */
void DMA2D_ClearFlag(uint32_t DMA2D_FLAG)
{
  /* 验证输入标志类型是否合法 */
  assert_param(IS_DMA2D_GET_FLAG(DMA2D_FLAG));
    
  /* 将目标标志位值写入IFCR寄存器
     寄存器特性：对应位写1会清除ISR寄存器的相应标志位
     注意：该操作不可逆，执行后相关中断状态将被清除 */
  DMA2D->IFCR = (uint32_t)DMA2D_FLAG;
}

/**
  * @brief  检查DMA2D中断状态（带中断使能状态验证）
  * @param  DMA2D_IT: 需要检查的中断类型
  *   @arg 可取值（按位组合）：
  *     DMA2D_IT_CE   配置错误中断（CR[0]）
  *     DMA2D_IT_CTC CLUT传输完成中断（CR[1]）
  *     DMA2D_IT_CAE CLUT访问错误中断（CR[2]）
  *     DMA2D_IT_TW   传输水印中断（CR[3]）
  *     DMA2D_IT_TC   传输完成中断（CR[4]）
  *     DMA2D_IT_TE   传输错误中断（CR[5]）
  * @retval ITStatus 中断状态
  *   @arg SET:   中断已发生且被使能
  *   @arg RESET: 中断未发生或未被使能
  * @note   该函数执行双重校验：
  *         1. 检查ISR寄存器中的中断触发标志
  *         2. 验证CR寄存器中对应的中断使能位
  *         只有两者同时有效时才返回SET状态
  */
ITStatus DMA2D_GetITStatus(uint32_t DMA2D_IT)
{
  ITStatus bitstatus = RESET;
  /* 计算实际的中断标志位偏移量：
     CR寄存器使能位与ISR状态标志存在8位偏移
     例如：CR[0]的使能位对应ISR[0]的状态位 */
  uint32_t DMA2D_IT_FLAG = DMA2D_IT >> 8;
  
  /* 验证输入中断类型参数有效性 */
  assert_param(IS_DMA2D_IT(DMA2D_IT));

  /* 第一阶段检查：ISR寄存器状态位检测 */
  if ((DMA2D->ISR & DMA2D_IT_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  
  /* 第二阶段检查：CR寄存器中断使能状态验证 */
  if (((DMA2D->CR & DMA2D_IT) != (uint32_t)RESET) && (bitstatus != (uint32_t)RESET))
  {
    /* 仅当同时满足：中断已触发 且 中断使能打开 */
    bitstatus = SET;
  }
  else
  {
    /* 任一条件不满足则返回复位状态 */
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  清除DMA2D中断挂起状态
  * @param  DMA2D_IT: 需要清除的中断类型
  *   @arg 可取值（按位组合）：
  *     DMA2D_IT_CE | DMA2D_IT_CTC | DMA2D_IT_CAE |
  *     DMA2D_IT_TW | DMA2D_IT_TC | DMA2D_IT_TE
  * @retval 无
  * @note   清除操作需要对应IFCR寄存器的位偏移，
  *         实际执行的操作是向IFCR寄存器写入状态位，
  *         采用"写1清零"机制，操作不可逆
  */
void DMA2D_ClearITPendingBit(uint32_t DMA2D_IT)
{
  /* 验证输入参数有效性 */
  assert_param(IS_DMA2D_IT(DMA2D_IT));
  
  /* 调整中断位偏移量：
     输入参数对应CR寄存器的使能位位置（低8位）
     实际需要操作IFCR寄存器的状态标志位（高8位） */
  DMA2D_IT = DMA2D_IT >> 8;
    
  /* 将调整后的中断标志写入IFCR寄存器
     该操作会同时清除ISR寄存器的对应位 
     注意：IFCR寄存器采用写1清零机制 */
  DMA2D->IFCR = (uint32_t)DMA2D_IT;
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

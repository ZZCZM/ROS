/**
  ******************************************************************************
  * @file    stm32f4xx_dma2d.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the DMA2D firmware 
  *          library.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_DMA2D_H
#define __STM32F4xx_DMA2D_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup DMA2D
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
 /** 
  * @brief  DMA2D 初始化结构体定义
  * @note   用于配置DMA2D图形加速器的基本工作模式和输出参数
  */
typedef struct
{
  uint32_t DMA2D_Mode;       /*!< 配置DMA2D传输模式:
                             @arg @ref DMA2D_MODE_M2M         存储器到存储器模式
                             @arg @ref DMA2D_MODE_M2M_PFC      带像素格式转换的存储器到存储器模式
                             @arg @ref DMA2D_MODE_M2M_BLEND    带混合的存储器到存储器模式
                             @arg @ref DMA2D_MODE_R2M         寄存器到存储器模式 */

  uint32_t DMA2D_CMode;      /*!< 配置输出图像颜色格式:
                             @arg @ref DMA2D_ARGB8888         32位ARGB格式
                             @arg @ref DMA2D_RGB888           24位RGB格式
                             @arg @ref DMA2D_RGB565           16位RGB格式
                             @arg @ref DMA2D_ARGB1555         16位ARGB格式
                             @arg @ref DMA2D_ARGB4444         16位ARGB格式 */
  
  uint32_t DMA2D_OutputBlue; /*!< 输出图像的蓝色分量配置:
                             - ARGB8888模式: 0x00~0xFF (8位)
                             - RGB888模式:   0x00~0xFF (8位)
                             - RGB565模式:   0x00~0x1F (5位)
                             - ARGB1555模式: 0x00~0x1F (5位)
                             - ARGB4444模式: 0x00~0x0F (4位) */

  uint32_t DMA2D_OutputGreen;/*!< 输出图像的绿色分量配置:
                             - ARGB8888模式: 0x00~0xFF (8位)
                             - RGB888模式:   0x00~0xFF (8位)
                             - RGB565模式:   0x00~0x3F (6位)
                             - ARGB1555模式: 0x00~0x1F (5位)
                             - ARGB4444模式: 0x00~0x0F (4位) */
            
  uint32_t DMA2D_OutputRed;  /*!< 输出图像的红色分量配置:
                             - ARGB8888模式: 0x00~0xFF (8位)
                             - RGB888模式:   0x00~0xFF (8位)
                             - RGB565模式:   0x00~0x1F (5位)
                             - ARGB1555模式: 0x00~0x1F (5位)
                             - ARGB4444模式: 0x00~0x0F (4位) */
  
  uint32_t DMA2D_OutputAlpha;/*!< 输出图像的Alpha通道配置:
                             - ARGB8888模式: 0x00~0xFF (8位透明度)
                             - ARGB1555模式: 0x00~0x01 (1位透明度)
                             - ARGB4444模式: 0x00~0x0F (4位透明度)
                             - 其他模式:     该值无效 */

  uint32_t DMA2D_OutputMemoryAdd; /*!< 输出缓冲区内存地址:
                                 - 必须32位对齐
                                 - 地址范围: 0x00000000~0xFFFFFFFF
                                 - 实际有效地址取决于硬件内存映射 */

  uint32_t DMA2D_OutputOffset;  /*!< 输出行偏移量（像素间距）:
                               - 范围: 0x0000~0x3FFF
                               - 单位: 像素个数
                               - 计算公式: Offset = (显示宽度 - 传输宽度) */

  uint32_t DMA2D_NumberOfLine; /*!< 传输区域行数配置:
                               - 范围: 0x0000~0xFFFF
                               - 实际最大行数受硬件内存限制 */
            
  uint32_t DMA2D_PixelPerLine; /*!< 每行传输像素数配置:
                               - 范围: 0x0000~0x3FFF 
                               - 最大支持16383像素/行 */
} DMA2D_InitTypeDef;

/**
  * @brief  DMA2D前景层配置结构体
  * @note   用于配置前景层的颜色混合、CLUT等高级特性
  */
typedef struct
{
  uint32_t DMA2D_FGMA;       /*!< 前景层内存起始地址:
                             - 必须与颜色模式对齐（ARGB8888需4字节对齐）
                             - 范围: 0x00000000~0xFFFFFFFF */
  
  uint32_t DMA2D_FGO;        /*!< 前景层行偏移量:
                             - 范围: 0x0000~0x3FFF
                             - 单位: 像素个数 */

  uint32_t DMA2D_FGCM;       /*!< 前景层颜色模式:
                             @arg @ref DMA2D_FGCM_ARGB8888  
                             @arg @ref DMA2D_FGCM_RGB888    
                             @arg @ref DMA2D_FGCM_L8        256色索引模式
                             @arg @ref DMA2D_FGCM_AL44      16色带4位Alpha
                             @arg @ref DMA2D_FGCM_AL88      256色带8位Alpha */

  uint32_t DMA2D_FG_CLUT_CM; /*!< 前景CLUT颜色模式:
                             @arg @ref DMA2D_CLUT_ARGB8888
                             @arg @ref DMA2D_CLUT_RGB888 */
            
  uint32_t DMA2D_FG_CLUT_SIZE; /*!< 前景CLUT大小:
                             - 范围: 0x00~0xFF 
                             - 实际颜色数 = 配置值 + 1 */
  
  uint32_t DMA2D_FGPFC_ALPHA_MODE; /*!< 前景Alpha混合模式:
                                  @arg @ref DMA2D_NO_MODIF_ALPHA   不使用Alpha
                                  @arg @ref DMA2D_REPLACE_ALPHA    替换Alpha值
                                  @arg @ref DMA2D_COMBINE_ALPHA    与像素Alpha混合 */

  uint32_t DMA2D_FGPFC_ALPHA_VALUE; /*!< 固定Alpha值:
                                   - 范围: 0x00（全透明）~0xFF（不透明）
                                   - 仅在替换/混合模式有效 */

  uint32_t DMA2D_FGC_BLUE;    /*!< 前景恒定颜色蓝色分量:
                             - 范围: 0x00~0xFF 
                             - 仅常量颜色模式有效 */

  uint32_t DMA2D_FGC_GREEN;   /*!< 前景恒定颜色绿色分量:
                             - 范围: 0x00~0xFF */

  uint32_t DMA2D_FGC_RED;     /*!< 前景恒定颜色红色分量:
                             - 范围: 0x00~0xFF */
            
  uint32_t DMA2D_FGCMAR;      /*!< 前景CLUT内存地址:
                             - 必须32位对齐
                             - 地址范围: 0x00000000~0xFFFFFFFF */
} DMA2D_FG_InitTypeDef;

/**
  * @brief  DMA2D背景层配置结构体
  * @note   配置参数与前景层结构相似，用于背景混合处理
  */
typedef struct
{
  uint32_t DMA2D_BGMA;        /*!< 背景层内存地址，配置要求同DMA2D_FGMA */
  
  uint32_t DMA2D_BGO;         /*!< 背景层行偏移，配置要求同DMA2D_FGO */

  uint32_t DMA2D_BGCM;        /*!< 背景层颜色模式，选项同DMA2D_FGCM */

  uint32_t DMA2D_BG_CLUT_CM;  /*!< 背景CLUT颜色模式，选项同DMA2D_FG_CLUT_CM */
            
  uint32_t DMA2D_BG_CLUT_SIZE;/*!< 背景CLUT大小，配置规则同前景层 */
  
  uint32_t DMA2D_BGPFC_ALPHA_MODE; /*!< 背景Alpha混合模式，选项同前景层 */

  uint32_t DMA2D_BGPFC_ALPHA_VALUE;/*!< 背景固定Alpha值，配置规则同前景层 */

  uint32_t DMA2D_BGC_BLUE;    /*!< 背景恒定颜色蓝色分量，配置规则同前景层 */

  uint32_t DMA2D_BGC_GREEN;   /*!< 背景恒定颜色绿色分量 */

  uint32_t DMA2D_BGC_RED;     /*!< 背景恒定颜色红色分量 */
            
  uint32_t DMA2D_BGCMAR;      /*!< 背景CLUT内存地址，配置要求同前景层 */
} DMA2D_BG_InitTypeDef;


/* 导出常量 --------------------------------------------------------*/

/** @defgroup DMA2D_Exported_Constants DMA2D导出常量
  * @{
  */  

/** @defgroup DMA2D_MODE DMA2D工作模式
  * @brief 配置DMA2D数据传输的工作模式
  * @{
  */
#define DMA2D_M2M            ((uint32_t)0x00000000)  /*!< 存储器到存储器基础传输模式 */
#define DMA2D_M2M_PFC        ((uint32_t)0x00010000)  /*!< 带像素格式转换的存储器到存储器模式 */
#define DMA2D_M2M_BLEND      ((uint32_t)0x00020000)  /*!< 带Alpha混合的存储器到存储器模式 */
#define DMA2D_R2M            ((uint32_t)0x00030000)  /*!< 寄存器到存储器模式（恒定颜色填充） */

/**
  * @brief 验证DMA2D模式有效性
  * @param MODE: 输入模式值
  * @retval 有效性状态（True/False）
  */
#define IS_DMA2D_MODE(MODE) (((MODE) == DMA2D_M2M) ||         /* 基础模式检查 */ \
                             ((MODE) == DMA2D_M2M_PFC) ||    /* 像素格式转换模式 */ \
                             ((MODE) == DMA2D_M2M_BLEND) || /* 混合模式检查 */ \
                             ((MODE) == DMA2D_R2M))            /* 寄存器模式检查 */
/**
  * @}
  */  

/** @defgroup DMA2D_CMODE 输出颜色格式
  * @brief 配置输出图像的颜色编码格式
  * @{
  */
#define DMA2D_ARGB8888       ((uint32_t)0x00000000)  /*!< 32位ARGB格式: 8位Alpha+8位R/G/B */
#define DMA2D_RGB888         ((uint32_t)0x00000001)  /*!< 24位RGB格式: 8位R/G/B (内存占用32位/像素) */
#define DMA2D_RGB565         ((uint32_t)0x00000002)  /*!< 16位RGB格式: 5-6-5位R/G/B分布 */
#define DMA2D_ARGB1555       ((uint32_t)0x00000003)  /*!< 16位ARGB格式: 1位Alpha+5位R/G/B */
#define DMA2D_ARGB4444       ((uint32_t)0x00000004)  /*!< 16位ARGB格式: 4位Alpha+R/G/B */

/**
  * @brief 验证颜色格式有效性
  * @param MODE_ARGB: 输入颜色格式
  * @retval 有效性状态
  */
#define IS_DMA2D_CMODE(MODE_ARGB) (((MODE_ARGB) == DMA2D_ARGB8888) ||  /* ARGB32检查 */ \
                                   ((MODE_ARGB) == DMA2D_RGB888) ||    /* RGB24检查 */ \
                                   ((MODE_ARGB) == DMA2D_RGB565) ||    /* RGB16检查 */ \
                                   ((MODE_ARGB) == DMA2D_ARGB1555) ||  /* ARGB1555检查 */ \
                                   ((MODE_ARGB) == DMA2D_ARGB4444))    /* ARGB4444检查 */
/**
  * @}
  */  

/** @defgroup DMA2D_OUTPUT_COLOR 输出颜色分量
  * @brief 输出颜色分量范围验证
  * @{
  */
#define DMA2D_Output_Color   ((uint32_t)0x000000FF)  /*!< 颜色分量通用掩码（8位最大值） */

/* 颜色分量范围验证宏 */
#define IS_DMA2D_OGREEN(OGREEN) ((OGREEN) <= DMA2D_Output_Color)  /* 绿色分量验证（实际位宽依赖CMODE） */
#define IS_DMA2D_ORED(ORED)     ((ORED) <= DMA2D_Output_Color)    /* 红色分量验证 */
#define IS_DMA2D_OBLUE(OBLUE)   ((OBLUE) <= DMA2D_Output_Color)    /* 蓝色分量验证 */
#define IS_DMA2D_OALPHA(OALPHA) ((OALPHA) <= DMA2D_Output_Color)   /* Alpha分量验证 */
/**
  * @}
  */  

/** @defgroup DMA2D_OUTPUT_OFFSET 输出行偏移
  * @brief 配置输出缓冲区行偏移量
  * @{
  */
#define DMA2D_OUTPUT_OFFSET  ((uint32_t)0x00003FFF)  /*!< 最大行偏移量（16383像素） */

/**
  * @brief 验证行偏移量有效性
  * @param OOFFSET: 输入偏移值
  * @retval 有效性状态
  * @note 实际值计算：Offset = (显示宽度 - 传输宽度) 
  */
#define IS_DMA2D_OUTPUT_OFFSET(OOFFSET) ((OOFFSET) <= DMA2D_OUTPUT_OFFSET)
/**
  * @}
  */  

/**
  * @}
  */  
/** @defgroup DMA2D_SIZE 传输区域尺寸
  * @brief 配置DMA2D传输区域的尺寸限制参数
  * @{
  */
#define DMA2D_pixel          ((uint32_t)0x00003FFF)  /*!< 单行最大像素数: 16383像素/行 */
#define DMA2D_Line           ((uint32_t)0x0000FFFF)   /*!< 最大传输行数: 65535行 */

/* 尺寸验证宏 ------------------------------------------------------*/
/**
  * @brief 验证传输行数有效性
  * @param LINE: 输入行数值（0-65535）
  */
#define IS_DMA2D_LINE(LINE)  ((LINE) <= DMA2D_Line)      // 检查是否超出最大行数限制

/**
  * @brief 验证单行像素数有效性
  * @param PIXEL: 输入像素数值（0-16383）
  */
#define IS_DMA2D_PIXEL(PIXEL) ((PIXEL) <= DMA2D_pixel)   // 检查单行是否超过最大像素数
/** @} */  

/** @defgroup DMA2D_OFFSET 层偏移量
  * @brief 配置前景/背景层行偏移参数
  * @{
  */
#define OFFSET               ((uint32_t)0x00003FFF)  /*!< 行偏移最大值: 16383像素偏移 */

/* 偏移量验证宏 ----------------------------------------------------*/
/**
  * @brief 验证前景层行偏移有效性
  * @param FGO: 前景偏移量（0-16383）
  */
#define IS_DMA2D_FGO(FGO)  ((FGO) <= OFFSET)          // 前景层行偏移检查

/**
  * @brief 验证背景层行偏移有效性
  * @param BGO: 背景偏移量（0-16383）
  */
#define IS_DMA2D_BGO(BGO)  ((BGO) <= OFFSET)          // 背景层行偏移检查
/** @} */

/** @defgroup DMA2D_FGCM 前景颜色模式
  * @brief 配置前景层图像颜色编码格式
  * @{
  */
#define CM_ARGB8888        ((uint32_t)0x00000000)  /*!< 32位真彩色: 8位A+R+G+B */
#define CM_RGB888          ((uint32_t)0x00000001)  /*!< 24位真彩色: R-G-B各8位（存储为32位）*/
#define CM_RGB565          ((uint32_t)0x00000002)  /*!< 16位高彩色: R(5)-G(6)-B(5) */
#define CM_ARGB1555        ((uint32_t)0x00000003)  /*!< 16位带透明: 1位A+R(5)-G(5)-B(5) */
#define CM_ARGB4444        ((uint32_t)0x00000004)  /*!< 16位带透明: 4位A+R(4)-G(4)-B(4) */
#define CM_L8              ((uint32_t)0x00000005)  /*!< 8位索引色: 256色CLUT模式 */
#define CM_AL44            ((uint32_t)0x00000006)  /*!< 8位Alpha混合: 4位Alpha + 4位亮度 */
#define CM_AL88            ((uint32_t)0x00000007)  /*!< 16位Alpha混合: 8位Alpha + 8位亮度 */
#define CM_L4              ((uint32_t)0x00000008)  /*!< 4位索引色: 16色CLUT模式 */
#define CM_A8              ((uint32_t)0x00000009)  /*!< 8位Alpha通道: 256级透明度 */
#define CM_A4              ((uint32_t)0x0000000A)  /*!< 4位Alpha通道: 16级透明度 */

/* 模式验证宏 ------------------------------------------------------*/
/**
  * @brief 验证前景颜色模式有效性
  * @param FGCM: 输入颜色模式值
  * @retval 有效性状态
  */
// 32位真彩色模式检查
// 24位真彩色检查
// 16位高彩色检查
// 16位透明模式检查
// 16位透明模式检查
// 256索引色检查
// 8位Alpha混合检查
// 16位Alpha混合检查
// 16索引色检查
// 8位Alpha通道
// 4位Alpha通道
#define IS_DMA2D_FGCM(FGCM) (((FGCM) == CM_ARGB8888) || \
                             ((FGCM) == CM_RGB888) || \
                             ((FGCM) == CM_RGB565) || \
                             ((FGCM) == CM_ARGB1555) || \
                             ((FGCM) == CM_ARGB4444) || \
                             ((FGCM) == CM_L8) || \
                             ((FGCM) == CM_AL44) || \
                             ((FGCM) == CM_AL88) || \
                             ((FGCM) == CM_L4) || \
                             ((FGCM) == CM_A8) || \
                             ((FGCM) == CM_A4))      

/**
  * @brief 验证背景颜色模式有效性（检查项同前景层）
  */
#define IS_DMA2D_BGCM(BGCM) (((BGCM) == CM_ARGB8888) || \
                             ((BGCM) == CM_RGB888) || \
                             ((BGCM) == CM_RGB565) || \
                             ((BGCM) == CM_ARGB1555) || \
                             ((BGCM) == CM_ARGB4444) || \
                             ((BGCM) == CM_L8) || \
                             ((BGCM) == CM_AL44) || \
                             ((BGCM) == CM_AL88) || \
                             ((BGCM) == CM_L4) || \
                             ((BGCM) == CM_A8) || \
                             ((BGCM) == CM_A4))
/** @} */
/** @defgroup DMA2D_FG_CLUT_CM CLUT颜色模式
  * @brief 配置前景/背景层颜色查找表(CLUT)的存储格式
  * @{
  */
#define CLUT_CM_ARGB8888    ((uint32_t)0x00000000)  /*!< CLUT采用32位ARGB格式存储每个颜色项 */
#define CLUT_CM_RGB888      ((uint32_t)0x00000001)  /*!< CLUT采用24位RGB格式存储（实际占用32位内存） */

/**
  * @brief 验证前景CLUT颜色模式
  * @param FG_CLUT_CM 输入颜色模式值
  * @retval 有效性状态（True表示ARGB8888或RGB888格式）
  */
#define IS_DMA2D_FG_CLUT_CM(FG_CLUT_CM) (((FG_CLUT_CM) == CLUT_CM_ARGB8888) || \
                                        ((FG_CLUT_CM) == CLUT_CM_RGB888))

/**
  * @brief 验证背景CLUT颜色模式（检查项同前景层）
  */
#define IS_DMA2D_BG_CLUT_CM(BG_CLUT_CM) (((BG_CLUT_CM) == CLUT_CM_ARGB8888) || \
                                        ((BG_CLUT_CM) == CLUT_CM_RGB888))
/** @} */

/** @defgroup DMA2D_FG_COLOR_VALUE 颜色分量验证
  * @brief 颜色分量范围及CLUT尺寸验证
  * @{
  */
#define COLOR_VALUE ((uint32_t)0x000000FF)  /*!< 8位颜色分量最大值（0-255）*/

/* CLUT尺寸验证 ----------------------------------------------------*/
/**
  * @brief 验证前景CLUT尺寸（颜色条目数-1）
  * @param FG_CLUT_SIZE 输入CLUT尺寸值（0-255对应256色）
  */
#define IS_DMA2D_FG_CLUT_SIZE(FG_CLUT_SIZE) ((FG_CLUT_SIZE) <= COLOR_VALUE)

/* 前景颜色分量验证 ------------------------------------------------*/
/**
  * @brief 验证前景固定Alpha值（0-255）
  */
#define IS_DMA2D_FG_ALPHA_VALUE(FG_ALPHA_VALUE) ((FG_ALPHA_VALUE) <= COLOR_VALUE)
#define IS_DMA2D_FGC_BLUE(FGC_BLUE)   ((FGC_BLUE) <= COLOR_VALUE)   /*!< 蓝分量检查 */
#define IS_DMA2D_FGC_GREEN(FGC_GREEN) ((FGC_GREEN) <= COLOR_VALUE)   /*!< 绿分量检查 */
#define IS_DMA2D_FGC_RED(FGC_RED)     ((FGC_RED) <= COLOR_VALUE)     /*!< 红分量检查 */

/* 背景验证宏 ----------------------------------------------------*/
/**
  * @brief 验证背景CLUT尺寸（检查规则同前景）
  */
#define IS_DMA2D_BG_CLUT_SIZE(BG_CLUT_SIZE) ((BG_CLUT_SIZE) <= COLOR_VALUE)

/**
  * @brief 验证背景层颜色分量（检查规则同前景）
  */
#define IS_DMA2D_BG_ALPHA_VALUE(BG_ALPHA_VALUE) ((BG_ALPHA_VALUE) <= COLOR_VALUE)
#define IS_DMA2D_BGC_BLUE(BGC_BLUE)   ((BGC_BLUE) <= COLOR_VALUE)    // 背景蓝分量
#define IS_DMA2D_BGC_GREEN(BGC_GREEN) ((BGC_GREEN) <= COLOR_VALUE)   // 背景绿分量
#define IS_DMA2D_BGC_RED(BGC_RED)     ((BGC_RED) <= COLOR_VALUE)    // 背景红分量
/** @} */

/** @defgroup DMA2D_FGPFC_ALPHA_MODE Alpha混合模式
  * @brief 配置前景像素格式转换中的Alpha处理方式
  * @{
  */
#define NO_MODIF_ALPHA_VALUE    ((uint32_t)0x00000000)  /*!< 保留原始Alpha值 */
#define REPLACE_ALPHA_VALUE     ((uint32_t)0x00000001)  /*!< 使用配置的固定Alpha值替换 */
#define COMBINE_ALPHA_VALUE     ((uint32_t)0x00000002)  /*!< 将配置的Alpha与原始值混合 */

/**
  * @brief 验证前景Alpha模式有效性
  * @param FG_ALPHA_MODE 输入模式值
  * @retval 有效性状态
  */
// 保留原始模式检查
// 替换模式检查
// 混合模式检查
#define IS_DMA2D_FG_ALPHA_MODE(FG_ALPHA_MODE) (((FG_ALPHA_MODE) == NO_MODIF_ALPHA_VALUE) || \
                                              ((FG_ALPHA_MODE) == REPLACE_ALPHA_VALUE) || \
                                              ((FG_ALPHA_MODE) == COMBINE_ALPHA_VALUE))     


/** @defgroup DMA2D_Interrupts 中断类型
  * @brief DMA2D中断源配置
  * @{
  */
#define DMA2D_IT_CE      DMA2D_CR_CEIE  /*!< 配置错误中断：当DMA2D配置寄存器写入非法值时触发 */
#define DMA2D_IT_CTC     DMA2D_CR_CTCIE /*!< CLUT传输完成中断：颜色查找表加载完成时触发 */
#define DMA2D_IT_CAE     DMA2D_CR_CAEIE /*!< CLUT访问错误中断：访问未初始化的CLUT时触发 */
#define DMA2D_IT_TW      DMA2D_CR_TWIE  /*!< 传输水印中断：当剩余传输行数低于设定值时触发 */ 
#define DMA2D_IT_TC      DMA2D_CR_TCIE  /*!< 传输完成中断：整体DMA传输完成时触发 */
#define DMA2D_IT_TE      DMA2D_CR_TEIE  /*!< 传输错误中断：DMA传输过程中发生错误时触发 */

/**
  * @brief 验证中断类型有效性
  * @param IT 输入中断类型值
  * @retval 有效性状态
  */
// CLUT传输完成中断检查
// CLUT访问错误中断检查
// 水印中断检查
// 传输完成中断检查
// 传输错误中断检查
// 配置错误中断检查
#define IS_DMA2D_IT(IT) (((IT) == DMA2D_IT_CTC) || \
                        ((IT) == DMA2D_IT_CAE) || \
                        ((IT) == DMA2D_IT_TW) || \
                        ((IT) == DMA2D_IT_TC) || \
                        ((IT) == DMA2D_IT_TE) || \
                        ((IT) == DMA2D_IT_CE))   
/** @} */

/** @defgroup DMA2D_Flag 状态标志位
  * @brief DMA2D状态寄存器标志位定义
  * @{
  */
#define DMA2D_FLAG_CE      DMA2D_ISR_CEIF  /*!< 配置错误标志：检测到配置参数非法时置位 */
#define DMA2D_FLAG_CTC     DMA2D_ISR_CTCIF /*!< CLUT传输完成标志：CLUT加载完成后置位 */
#define DMA2D_FLAG_CAE     DMA2D_ISR_CAEIF /*!< CLUT访问错误标志：无效CLUT访问时置位 */
#define DMA2D_FLAG_TW      DMA2D_ISR_TWIF  /*!< 传输水印标志：剩余行数低于阈值时置位 */
#define DMA2D_FLAG_TC      DMA2D_ISR_TCIF  /*!< 传输完成标志：全部数据传输完成时置位 */
#define DMA2D_FLAG_TE      DMA2D_ISR_TEIF  /*!< 传输错误标志：传输过程中发生错误时置位 */

/**
  * @brief 验证状态标志有效性
  * @param FLAG 输入标志值
  * @retval 有效性状态
  */
// CLUT完成标志检查
// CLUT访问错误检查
// 水印标志检查
// 传输完成检查
// 传输错误检查
// 配置错误检查
#define IS_DMA2D_GET_FLAG(FLAG) (((FLAG) == DMA2D_FLAG_CTC) || \
                                ((FLAG) == DMA2D_FLAG_CAE) || \
                                ((FLAG) == DMA2D_FLAG_TW) || \
                                ((FLAG) == DMA2D_FLAG_TC) || \
                                ((FLAG) == DMA2D_FLAG_TE) || \
                                ((FLAG) == DMA2D_FLAG_CE))   



/**
  * @brief 验证状态标志有效性
  * @param FLAG 输入要检查的状态标志
  * @retval 布尔值，True表示有效的标志类型
  * @note 检查范围覆盖DMA2D所有状态标志位：
  *        - DMA2D_FLAG_CTC: CLUT传输完成标志 @ref DMA2D_Flag
  *        - DMA2D_FLAG_CAE: CLUT访问错误标志
  *        - DMA2D_FLAG_TW: 传输水印标志
  *        - DMA2D_FLAG_TC: 整体传输完成标志
  *        - DMA2D_FLAG_TE: 传输错误标志
  *        - DMA2D_FLAG_CE: 配置错误标志
  */
#define IS_DMA2D_GET_FLAG(FLAG) (((FLAG) == DMA2D_FLAG_CTC) || \
                                ((FLAG) == DMA2D_FLAG_CAE) || \
                                ((FLAG) == DMA2D_FLAG_TW) || \
                                ((FLAG) == DMA2D_FLAG_TC) || \
                                ((FLAG) == DMA2D_FLAG_TE) || \
                                ((FLAG) == DMA2D_FLAG_CE))     



/** @defgroup DMA2D_DeadTime 死区时间配置
  * @brief 配置DMA2D时序控制参数（用于低功耗模式下的时钟管理）
  * @{
  */
#define DEADTIME                  ((uint32_t)0x000000FF)  /*!< 最大死区时钟周期数：255个AHB时钟周期 */

/**
  * @brief 验证死区时间有效性
  * @param DEAD_TIME 输入死区时间值（0-255个时钟周期）
  * @note  该参数影响CLUT加载与像素传输的时序间隔
  */
#define IS_DMA2D_DEAD_TIME(DEAD_TIME) ((DEAD_TIME) <= DEADTIME)

/** @defgroup DMA2D_Watermark 水印阈值
  * @brief 配置传输过程中断触发阈值
  */
#define LINE_WATERMARK            DMA2D_LWR_LW  /*!< 水印最大行数值：0-255行 */

/**
  * @brief 验证水印行数有效性
  * @param LineWatermark 输入水印阈值（0-255） 
  * @note 当剩余传输行数≤此值时触发TW中断
  */
#define IS_DMA2D_LineWatermark(LineWatermark) ((LineWatermark) <= LINE_WATERMARK)
/** @} */

/* 导出函数 ------------------------------------------------------- */
/**
  * @brief 复位DMA2D所有寄存器至默认值
  * @note 会清除所有配置，包括前景/背景层和CLUT设置
  */
void DMA2D_DeInit(void);

/**
  * @brief 初始化DMA2D主控制器参数
  * @param DMA2D_InitStruct 初始化结构体指针，包含：
  *        - 工作模式
  *        - 输出颜色格式
  *        - 传输尺寸参数等
  */
void DMA2D_Init(DMA2D_InitTypeDef* DMA2D_InitStruct);

/**
  * @brief 初始化DMA2D配置结构体为默认值
  * @param DMA2D_InitStruct 要初始化的结构体指针
  * @note 默认参数：
  *        - 模式: DMA2D_M2M
  *        - 输出格式: ARGB8888
  *        - 传输尺寸: 0x0像素
  */
void DMA2D_StructInit(DMA2D_InitTypeDef* DMA2D_InitStruct);
/* 传输控制函数 *****************************************************/
/**
  * @brief 启动DMA2D图形传输
  * @note 调用前需完成所有配置，触发后立即开始传输
  * @warning 传输过程中修改配置寄存器可能导致硬件异常
  */
void DMA2D_StartTransfer(void);

/**
  * @brief 强制中止当前传输
  * @note 立即停止DMA2D引擎，残留数据可能留在输出缓冲区
  * @retval 无
  */
void DMA2D_AbortTransfer(void);

/**
  * @brief 暂停/恢复DMA2D传输
  * @param NewState 新状态: ENABLE(暂停)/DISABLE(恢复)
  * @note 暂停时可修改配置，恢复后从断点继续传输
  */
void DMA2D_Suspend(FunctionalState NewState);

/* 前景层配置函数 ***************************************************/
/**
  * @brief 配置前景层参数
  * @param DMA2D_FG_InitStruct 前景配置结构体指针 @ref DMA2D_FG_InitTypeDef
  * @note 需在传输停止时调用，配置立即生效
  */
void DMA2D_FGConfig(DMA2D_FG_InitTypeDef* DMA2D_FG_InitStruct);

/**
  * @brief 初始化前景层结构体为默认值
  * @param DMA2D_FG_InitStruct 结构体指针
  * @note 默认值：
  *        - 颜色模式: CM_ARGB8888
  *        - CLUT尺寸: 0
  *        - Alpha模式: NO_MODIF_ALPHA_VALUE
  */
void DMA2D_FG_StructInit(DMA2D_FG_InitTypeDef* DMA2D_FG_InitStruct);

/* 背景层配置函数 ***************************************************/
/**
  * @brief 配置背景层参数
  * @param DMA2D_BG_InitStruct 背景配置结构体指针 @ref DMA2D_BG_InitTypeDef
  * @note 配置影响混合操作，需与前景层配合使用
  */
void DMA2D_BGConfig(DMA2D_BG_InitTypeDef* DMA2D_BG_InitStruct);

/**
  * @brief 初始化背景层结构体为默认值
  * @param DMA2D_BG_InitStruct 结构体指针
  * @note 默认值与前景层结构体初始化相同
  */
void DMA2D_BG_StructInit(DMA2D_BG_InitTypeDef* DMA2D_BG_InitStruct);

/* 层控制函数 *******************************************************/
/**
  * @brief 启用/禁用前景CLUT加载
  * @param NewState ENABLE(立即加载CLUT)/DISABLE(保持当前)
  * @note 需在配置CLUT参数后调用使能
  */
void DMA2D_FGStart(FunctionalState NewState);

/**
  * @brief 启用/禁用背景CLUT加载
  * @param NewState 操作状态，参数说明同前景层
  */
void DMA2D_BGStart(FunctionalState NewState);

/* 高级配置函数 *****************************************************/
/**
  * @brief 配置死区时钟周期
  * @param DMA2D_DeadTime 死区时间值（0-255个时钟周期）
  * @param NewState ENABLE(启用)/DISABLE(关闭)
  * @note 用于低功耗模式下的时序补偿
  */
void DMA2D_DeadTimeConfig(uint32_t DMA2D_DeadTime, FunctionalState NewState);

/**
  * @brief 设置传输水印行阈值
  * @param DMA2D_LWatermarkConfig 水印值（0-255行）
  * @note 当剩余传输行数≤此值时触发TW中断
  */
void DMA2D_LineWatermarkConfig(uint32_t DMA2D_LWatermarkConfig);

/* 中断与标志管理 **************************************************/
/**
  * @brief 使能/禁用指定DMA2D中断
  * @param DMA2D_IT 中断类型 @ref DMA2D_Interrupts
  * @param NewState 中断状态
  */
void DMA2D_ITConfig(uint32_t DMA2D_IT, FunctionalState NewState);

/**
  * @brief 获取指定状态标志位
  * @param DMA2D_FLAG 标志类型 @ref DMA2D_Flag
  * @retval SET/RESET 标志状态
  * @note 标志位需手动清除
  */
FlagStatus DMA2D_GetFlagStatus(uint32_t DMA2D_FLAG);

/**
  * @brief 清除指定状态标志
  * @param DMA2D_FLAG 要清除的标志位
  */
void DMA2D_ClearFlag(uint32_t DMA2D_FLAG);

/**
  * @brief 获取中断状态（含中断使能状态）
  * @param DMA2D_IT 中断类型
  * @retval 中断有效状态
  */
ITStatus DMA2D_GetITStatus(uint32_t DMA2D_IT);

/**
  * @brief 清除中断挂起标志
  * @param DMA2D_IT 要清除的中断类型
  */
void DMA2D_ClearITPendingBit(uint32_t DMA2D_IT);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_DMA2D_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

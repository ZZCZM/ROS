/**
  ******************************************************************************
  * @file    stm32f4xx_dcmi.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the DCMI firmware library.
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
#ifndef __STM32F4xx_DCMI_H
#define __STM32F4xx_DCMI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup DCMI
  * @{
  */ 
/*
 * 模块名称：STM32F4xx数字摄像头接口(DCMI)配置参数
 * 功能描述：本头文件定义数字摄像头接口的初始化结构体及工作模式参数，包含：
 *          - 基础捕获模式配置
 *          - 图像裁剪区域设置
 *          - 嵌入式同步码定义
 *          - 捕获模式选择
 * 应用场景：用于配置摄像头接口的同步信号、数据格式和采集范围，适配不同图像传感器
 */

/* 导出类型定义 ------------------------------------------------------------*/

/** 
 * @brief DCMI基础初始化结构体
 * @note 用于配置摄像头接口的核心工作参数
 */
typedef struct
{
  /**
   * @brief 捕获模式选择
   * @see DCMI_Capture_Mode 可取值：连续模式/快照模式
   */
  uint16_t DCMI_CaptureMode;      /*!< 捕获模式：
                                       - 连续模式：持续传输图像数据
                                       - 快照模式：单帧捕获 */

  /**
   * @brief 同步模式选择
   * @see DCMI_Synchronization_Mode 可取值：硬件同步/嵌入式同步
   */
  uint16_t DCMI_SynchroMode;      /*!< 同步方式：
                                       - 硬件同步：使用HSYNC/VSYNC信号
                                       - 嵌入式同步：通过数据码识别帧/行 */

  /**
   * @brief 像素时钟极性
   * @see DCMI_PIXCK_Polarity 可取值：上升沿/下降沿采样
   */
  uint16_t DCMI_PCKPolarity;      /*!< 像素时钟边沿：
                                       - 上升沿捕获
                                       - 下降沿捕获 */

  /**
   * @brief 垂直同步极性
   * @see DCMI_VSYNC_Polarity 可取值：高有效/低有效
   */
  uint16_t DCMI_VSPolarity;       /*!< VSYNC有效电平：
                                       - 高电平表示垂直同步
                                       - 低电平表示垂直同步 */

  /**
   * @brief 水平同步极性
   * @see DCMI_HSYNC_Polarity 可取值：高有效/低有效
   */
  uint16_t DCMI_HSPolarity;       /*!< HSYNC有效电平：
                                       - 高电平表示行开始
                                       - 低电平表示行开始 */

  /**
   * @brief 捕获速率
   * @see DCMI_Capture_Rate 可取值：全帧/半帧/四分之一帧
   */
  uint16_t DCMI_CaptureRate;      /*!< 帧采样率：
                                       - 全帧捕获
                                       - 每2个帧捕获1次
                                       - 每4个帧捕获1次 */

  /**
   * @brief 数据扩展模式
   * @see DCMI_Extended_Data_Mode 可取值：8/10/12/14位数据宽度
   */
  uint16_t DCMI_ExtendedDataMode; /*!< 数据位宽：
                                       - 8位：直接传输
                                       - 10位：高位对齐
                                       - 12位：高位对齐
                                       - 14位：高位对齐 */
} DCMI_InitTypeDef;

/** 
 * @brief DCMI图像裁剪配置结构体
 * @note 用于设置感兴趣区域(ROI)捕获
 */
typedef struct
{
  uint16_t DCMI_VerticalStartLine;      /*!< 垂直起始行（0-8191）*/
  uint16_t DCMI_HorizontalOffsetCount; /*!< 水平偏移像素数（0-16383）*/
  uint16_t DCMI_VerticalLineCount;     /*!< 捕获行数（0-16383）*/
  uint16_t DCMI_CaptureCount;           /*!< 每行捕获像素数（0-16383）*/
} DCMI_CROPInitTypeDef;

/** 
 * @brief DCMI嵌入式同步码配置结构体
 * @note 用于自定义帧/行起始结束标识符
 */
typedef struct
{
  uint8_t DCMI_FrameStartCode; /*!< 帧起始标识码（通常0x00）*/
  uint8_t DCMI_LineStartCode;  /*!< 行起始标识码 */
  uint8_t DCMI_LineEndCode;    /*!< 行结束标识码 */
  uint8_t DCMI_FrameEndCode;   /*!< 帧结束标识码 */
} DCMI_CodesInitTypeDef;

/* 导出常量定义 ------------------------------------------------------------*/

/** @defgroup DCMI_Capture_Mode 捕获模式
  * @{
  */ 
#define DCMI_CaptureMode_Continuous    ((uint16_t)0x0000) /*!< 连续捕获模式：持续DMA传输，适合视频流 */
#define DCMI_CaptureMode_SnapShot      ((uint16_t)0x0002) /*!< 快照模式：捕获单帧后自动停止，适合拍照 */
/* 捕获模式校验宏 */
#define IS_DCMI_CAPTURE_MODE(MODE)(((MODE) == DCMI_CaptureMode_Continuous) || \
                                   ((MODE) == DCMI_CaptureMode_SnapShot))
/**
  * @brief 数字摄像头接口(DCMI)配置参数定义
  * @details 包含DCMI模块的各种工作模式配置选项和参数校验宏
  *          用于配置摄像头接口的同步方式、时钟极性、数据格式等关键参数
  */

/** @defgroup DCMI_Synchronization_Mode 同步模式配置组
  * @{
  */ 
#define DCMI_SynchroMode_Hardware    ((uint16_t)0x0000) /*!< 硬件同步模式：通过HSYNC/VSYNC信号同步帧/行的开始和结束 
                                                            适用于带硬件同步信号的摄像头模组 */
#define DCMI_SynchroMode_Embedded    ((uint16_t)0x0010) /*!< 嵌入式同步模式：通过数据流中的同步码进行帧同步
                                                            适用于采用数据包格式的摄像头协议 */
/* 同步模式参数校验宏 */
#define IS_DCMI_SYNCHRO(MODE)(((MODE) == DCMI_SynchroMode_Hardware) || \
                              ((MODE) == DCMI_SynchroMode_Embedded))
/**
  * @}
  */

/** @defgroup DCMI_PIXCK_Polarity 像素时钟极性配置组
  * @{
  */ 
#define DCMI_PCKPolarity_Falling    ((uint16_t)0x0000) /*!< 下降沿有效：像素数据在时钟下降沿被采样 
                                                            适用于摄像头PCLK信号的下降沿输出数据 */
#define DCMI_PCKPolarity_Rising     ((uint16_t)0x0020) /*!< 上升沿有效：像素数据在时钟上升沿被采样
                                                            适用于摄像头PCLK信号的上升沿输出数据 */
/* 像素时钟极性校验宏 */
#define IS_DCMI_PCKPOLARITY(POLARITY)(((POLARITY) == DCMI_PCKPolarity_Falling) || \
                                      ((POLARITY) == DCMI_PCKPolarity_Rising))
/**
  * @}
  */

/** @defgroup DCMI_VSYNC_Polarity 垂直同步极性配置组
  * @{
  */ 
#define DCMI_VSPolarity_Low     ((uint16_t)0x0000) /*!< 低电平有效：VSYNC低电平时表示垂直同步期间
                                                            适用于VSYNC信号低电平触发的摄像头 */
#define DCMI_VSPolarity_High    ((uint16_t)0x0080) /*!< 高电平有效：VSYNC高电平时表示垂直同步期间
                                                            适用于VSYNC信号高电平触发的摄像头 */
/* 垂直同步极性校验宏 */
#define IS_DCMI_VSPOLARITY(POLARITY)(((POLARITY) == DCMI_VSPolarity_Low) || \
                                     ((POLARITY) == DCMI_VSPolarity_High))
/**
  * @}
  */

/** @defgroup DCMI_HSYNC_Polarity 水平同步极性配置组
  * @{
  */ 
#define DCMI_HSPolarity_Low     ((uint16_t)0x0000) /*!< 低电平有效：HSYNC低电平时表示水平同步期间
                                                            适用于HSYNC信号低电平触发的摄像头 */
#define DCMI_HSPolarity_High    ((uint16_t)0x0040) /*!< 高电平有效：HSYNC高电平时表示水平同步期间
                                                            适用于HSYNC信号高电平触发的摄像头 */
/* 水平同步极性校验宏 */
#define IS_DCMI_HSPOLARITY(POLARITY)(((POLARITY) == DCMI_HSPolarity_Low) || \
                                     ((POLARITY) == DCMI_HSPolarity_High))
/**
  * @}
  */

/** @defgroup DCMI_Capture_Rate 帧捕获速率配置组
  * @{
  */ 
#define DCMI_CaptureRate_All_Frame     ((uint16_t)0x0000) /*!< 全帧捕获：不跳过任何帧数据
                                                            适用于需要最大帧率的应用场景 */
#define DCMI_CaptureRate_1of2_Frame    ((uint16_t)0x0100) /*!< 1/2帧捕获：每2帧捕获1帧
                                                            用于降低带宽消耗或处理高帧率数据 */
#define DCMI_CaptureRate_1of4_Frame    ((uint16_t)0x0200) /*!< 1/4帧捕获：每4帧捕获1帧
                                                            适用于极低带宽要求的应用场景 */
/* 捕获速率校验宏 */
#define IS_DCMI_CAPTURE_RATE(RATE) (((RATE) == DCMI_CaptureRate_All_Frame) || \
                                    ((RATE) == DCMI_CaptureRate_1of2_Frame) ||\
                                    ((RATE) == DCMI_CaptureRate_1of4_Frame))
/**
  * @}
  */

/** @defgroup DCMI_Extended_Data_Mode 扩展数据模式配置组
  * @{
  */ 
#define DCMI_ExtendedDataMode_8b     ((uint16_t)0x0000) /*!< 8位模式：每个像素时钟采集8位数据
                                                            适用于OV7670等8位接口摄像头 */
#define DCMI_ExtendedDataMode_10b    ((uint16_t)0x0400) /*!< 10位模式：每个像素时钟采集10位数据
                                                            支持高位宽图像传感器 */
#define DCMI_ExtendedDataMode_12b    ((uint16_t)0x0800) /*!< 12位模式：每个像素时钟采集12位数据
                                                            用于高精度图像采集场景 */
#define DCMI_ExtendedDataMode_14b    ((uint16_t)0x0C00) /*!< 14位模式：每个像素时钟采集14位数据
                                                            适用于医疗成像等专业领域 */
/* 数据位宽校验宏 */
#define IS_DCMI_EXTENDED_DATA(DATA)(((DATA) == DCMI_ExtendedDataMode_8b) || \
                                    ((DATA) == DCMI_ExtendedDataMode_10b) ||\
                                    ((DATA) == DCMI_ExtendedDataMode_12b) ||\
                                    ((DATA) == DCMI_ExtendedDataMode_14b))

/**
  * @}
  */ 

/**
  * @brief 数字摄像头接口(DCMI)中断与状态标志配置
  * @details 包含DCMI模块的中断源配置、状态标志定义及校验宏
  *          用于配置摄像头接口的中断触发条件和状态监控
  */

/** @defgroup DCMI_interrupt_sources 中断源配置组
  * @{
  */ 
#define DCMI_IT_FRAME    ((uint16_t)0x0001) /*!< 帧中断：当完整帧传输完成时触发 */
#define DCMI_IT_OVF      ((uint16_t)0x0002) /*!< 溢出中断：数据溢出时触发 */
#define DCMI_IT_ERR      ((uint16_t)0x0004) /*!< 错误中断：同步信号错误时触发 */
#define DCMI_IT_VSYNC    ((uint16_t)0x0008) /*!< 垂直同步中断：VSYNC信号触发 */
#define DCMI_IT_LINE     ((uint16_t)0x0010) /*!< 行中断：每完成一行数据传输触发 */

/* 中断配置参数校验宏（检查有效位范围） */
#define IS_DCMI_CONFIG_IT(IT) ((((IT) & (uint16_t)0xFFE0) == 0x0000) && ((IT) != 0x0000))

/* 中断类型校验宏（确认有效中断类型） */ 
#define IS_DCMI_GET_IT(IT) (((IT) == DCMI_IT_FRAME) || \
                            ((IT) == DCMI_IT_OVF) || \
                            ((IT) == DCMI_IT_ERR) || \
                            ((IT) == DCMI_IT_VSYNC) || \
                            ((IT) == DCMI_IT_LINE))
/**
  * @}
  */

/** @defgroup DCMI_Flags 状态标志配置组
  * @{
  */ 
/** 
  * @brief 状态寄存器(SR)标志位 
  */
#define DCMI_FLAG_HSYNC     ((uint16_t)0x2001) /*!< 水平同步状态：指示HSYNC信号状态 */
#define DCMI_FLAG_VSYNC     ((uint16_t)0x2002) /*!< 垂直同步状态：指示VSYNC信号状态 */
#define DCMI_FLAG_FNE       ((uint16_t)0x2004) /*!< FIFO非空：数据FIFO缓冲区有可用数据 */

/** 
  * @brief 原始中断状态寄存器(RISR)标志位
  */
#define DCMI_FLAG_FRAMERI    ((uint16_t)0x0001) /*!< 帧原始中断：未屏蔽的帧完成状态 */
#define DCMI_FLAG_OVFRI      ((uint16_t)0x0002) /*!< 溢出原始中断：未屏蔽的溢出状态 */
#define DCMI_FLAG_ERRRI      ((uint16_t)0x0004) /*!< 错误原始中断：未屏蔽的错误状态 */
#define DCMI_FLAG_VSYNCRI    ((uint16_t)0x0008) /*!< 垂直同步原始中断：未屏蔽的VSYNC状态 */
#define DCMI_FLAG_LINERI     ((uint16_t)0x0010) /*!< 行中断原始状态：未屏蔽的行传输完成状态 */

/** 
  * @brief 屏蔽中断状态寄存器(MISR)标志位 
  */
#define DCMI_FLAG_FRAMEMI    ((uint16_t)0x1001) /*!< 帧屏蔽中断：使能后的帧完成中断状态 */
#define DCMI_FLAG_OVFMI      ((uint16_t)0x1002) /*!< 溢出屏蔽中断：使能后的溢出中断状态 */
#define DCMI_FLAG_ERRMI      ((uint16_t)0x1004) /*!< 错误屏蔽中断：使能后的错误中断状态 */
#define DCMI_FLAG_VSYNCMI    ((uint16_t)0x1008) /*!< 垂直同步屏蔽中断：使能后的VSYNC中断状态 */
#define DCMI_FLAG_LINEMI     ((uint16_t)0x1010) /*!< 行屏蔽中断：使能后的行传输完成中断状态 */

/* 状态标志校验宏（所有有效标志位检查） */
#define IS_DCMI_GET_FLAG(FLAG) (((FLAG) == DCMI_FLAG_HSYNC) || \
                                ((FLAG) == DCMI_FLAG_VSYNC) || \
                                ((FLAG) == DCMI_FLAG_FNE) || \
                                ((FLAG) == DCMI_FLAG_FRAMERI) || \
                                ((FLAG) == DCMI_FLAG_OVFRI) || \
                                ((FLAG) == DCMI_FLAG_ERRRI) || \
                                ((FLAG) == DCMI_FLAG_VSYNCRI) || \
                                ((FLAG) == DCMI_FLAG_LINERI) || \
                                ((FLAG) == DCMI_FLAG_FRAMEMI) || \
                                ((FLAG) == DCMI_FLAG_OVFMI) || \
                                ((FLAG) == DCMI_FLAG_ERRMI) || \
                                ((FLAG) == DCMI_FLAG_VSYNCMI) || \
                                ((FLAG) == DCMI_FLAG_LINEMI))

/* 标志清除校验宏（检查可清除标志范围） */
#define IS_DCMI_CLEAR_FLAG(FLAG) ((((FLAG) & (uint16_t)0xFFE0) == 0x0000) && ((FLAG) != 0x0000))
/**
  * @}
  */
/**
  * @brief 数字摄像头接口(DCMI)功能函数声明
  * @details 包含DCMI模块的初始化配置、图像捕获控制及中断管理等功能函数
  */

/* 导出宏 ------------------------------------------------------------*/
/* 导出函数 --------------------------------------------------------*/ 

/*---------------------------- 硬件复位函数 ----------------------------*/
/*!< 将DCMI所有寄存器恢复为默认复位值 */
void DCMI_DeInit(void);

/*----------------------- 初始化与配置函数组 -------------------------*/
/*!< 基础初始化：根据结构体参数配置DCMI工作模式 */
void DCMI_Init(DCMI_InitTypeDef* DCMI_InitStruct);

/*!< 结构体默认值初始化：填充DCMI_InitTypeDef默认参数 */
void DCMI_StructInit(DCMI_InitTypeDef* DCMI_InitStruct);

/*!< 裁剪窗口配置：设置图像传感器的有效采集区域 */
void DCMI_CROPConfig(DCMI_CROPInitTypeDef* DCMI_CROPInitStruct);

/*!< 裁剪功能使能：ENABLE/DISABLE 图像裁剪功能 */
void DCMI_CROPCmd(FunctionalState NewState);

/*!< 嵌入式同步码设置：配置数据流中的帧起始/结束同步码 */
void DCMI_SetEmbeddedSynchroCodes(DCMI_CodesInitTypeDef* DCMI_CodesInitStruct);

/*!< JPEG模式使能：启用JPEG格式图像数据封装 */
void DCMI_JPEGCmd(FunctionalState NewState);

/*----------------------- 图像捕获控制函数组 -------------------------*/
/*!< DCMI总使能：控制整个摄像头接口的使能状态 */
void DCMI_Cmd(FunctionalState NewState);

/*!< 捕获控制：启动/停止图像数据采集流程 */
void DCMI_CaptureCmd(FunctionalState NewState);

/*!< 数据读取：从DCMI数据寄存器获取图像数据（32位访问） */
uint32_t DCMI_ReadData(void);

/*------------------- 中断与状态管理函数组 -------------------*/
/*!< 中断配置：使能/禁用指定类型的中断源 */
void DCMI_ITConfig(uint16_t DCMI_IT, FunctionalState NewState);

/*!< 标志状态获取：查询指定硬件标志位的状态 */
FlagStatus DCMI_GetFlagStatus(uint16_t DCMI_FLAG);

/*!< 标志清除：清除指定的状态标志位 */
void DCMI_ClearFlag(uint16_t DCMI_FLAG);

/*!< 中断状态获取：检查指定中断是否触发 */
ITStatus DCMI_GetITStatus(uint16_t DCMI_IT);

/*!< 中断挂起位清除：清除已触发的中断标志 */
void DCMI_ClearITPendingBit(uint16_t DCMI_IT);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_DCMI_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

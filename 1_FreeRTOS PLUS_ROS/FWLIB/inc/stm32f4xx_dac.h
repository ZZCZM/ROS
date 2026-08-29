/**
  ******************************************************************************
  * @file    stm32f4xx_dac.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the DAC firmware 
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
#ifndef __STM32F4xx_DAC_H
#define __STM32F4xx_DAC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup DAC
  * @{
  */
/*
 * 模块名称：STM32F4xx DAC模块配置参数
 * 功能描述：本头文件定义数模转换器(DAC)的以下配置项：
 *          - 初始化结构体参数
 *          - 触发信号源选择
 *          - 波形生成模式
 *          - 输出缓冲控制
 * 应用场景：用于配置DAC通道的转换触发方式、波形生成特性及输出参数
 */

/* 导出类型定义 ------------------------------------------------------------*/

/** 
 * @brief  DAC初始化结构体定义
 * @note   用于DAC_Init()函数的参数传递，配置DAC工作模式
 */
typedef struct
{
  /**
   * @brief 触发源选择
   * @see DAC_trigger_selection 可取值：DAC_Trigger_None/定时器触发/外部中断触发/软件触发
   */
  uint32_t DAC_Trigger;                      /*!< 指定DAC通道的外部触发源：
                                                   - 0x00: 无触发（自动转换）
                                                   - 0x04: TIM6_TRGO触发
                                                   - 0x0C: TIM8_TRGO触发
                                                   - 其他定时器或外部事件触发 */

  /**
   * @brief 波形生成模式
   * @see DAC_wave_generation 可取值：无波形/噪声波/三角波
   */
  uint32_t DAC_WaveGeneration;               /*!< 波形生成控制：
                                                   - 0x00: 禁用波形生成
                                                   - 0x40: 噪声波形生成
                                                   - 0x80: 三角波形生成 */

  /**
   * @brief LFSR掩码或三角波幅度
   * @see DAC_lfsrunmask_triangleamplitude 
   */
  uint32_t DAC_LFSRUnmask_TriangleAmplitude; /*!< 当启用波形生成时：
                                                   - 噪声波：指定LFSR掩码位(0x000-0xFFF)
                                                   - 三角波：设置幅度等级(1-4095) */

  /**
   * @brief 输出缓冲控制
   * @see DAC_output_buffer 可取值：ENABLE/DISABLE
   */
  uint32_t DAC_OutputBuffer;                 /*!< 输出缓冲使能：
                                                   - 0x00: 禁用输出缓冲（高阻抗）
                                                   - 0x01: 启用缓冲（降低输出阻抗） */
}DAC_InitTypeDef;

/* 导出常量定义 ------------------------------------------------------------*/

/** @defgroup DAC_Exported_Constants DAC模块常量定义
  * @{
  */

/** @defgroup DAC_trigger_selection 触发源选择
  * @brief DAC转换触发信号源配置
  * @{
  */
#define DAC_Trigger_None                   ((uint32_t)0x00000000) /*!< 无外部触发，写入DHR寄存器后立即转换 */
#define DAC_Trigger_T2_TRGO                ((uint32_t)0x00000024) /*!< 使用TIM2触发输出(TIM_TRGO)作为触发源 */
#define DAC_Trigger_T4_TRGO                ((uint32_t)0x0000002C) /*!< 使用TIM4触发输出 */
#define DAC_Trigger_T5_TRGO                ((uint32_t)0x0000001C) /*!< 使用TIM5触发输出 */
#define DAC_Trigger_T6_TRGO                ((uint32_t)0x00000004) /*!< 使用TIM6触发输出 */
#define DAC_Trigger_T7_TRGO                ((uint32_t)0x00000014) /*!< 使用TIM7触发输出 */
#define DAC_Trigger_T8_TRGO                ((uint32_t)0x0000000C) /*!< 使用TIM8触发输出 */                                                                       
#define DAC_Trigger_Ext_IT9                ((uint32_t)0x00000034) /*!< 使用外部中断线9(EXTI9)事件触发 */
#define DAC_Trigger_Software               ((uint32_t)0x0000003C) /*!< 软件触发，通过DAC_SWTRIGR寄存器触发 */

/* 触发源校验宏：确保配置值在允许范围内 */
#define IS_DAC_TRIGGER(TRIGGER) (((TRIGGER) == DAC_Trigger_None) || \
                                 ((TRIGGER) == DAC_Trigger_T6_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T8_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T7_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T5_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T2_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T4_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_Ext_IT9) || \
                                 ((TRIGGER) == DAC_Trigger_Software))

/*
 * 模块名称：STM32F4xx DAC波形生成配置
 * 功能描述：本部分定义DAC模块波形生成相关参数，包含：
 *          - 波形类型选择（无波形/噪声波/三角波）
 *          - 噪声波LFSR掩码配置
 *          - 三角波幅度等级设定
 * 核心作用：通过配置DAC_InitTypeDef结构体的WaveGeneration和LFSRUnmask_TriangleAmplitude字段，
 *          实现可编程波形输出功能，用于信号生成、测试模式等场景
 */

/** @defgroup DAC_wave_generation 波形生成模式
  * @brief 选择DAC通道的波形生成类型
  * @{
  */
#define DAC_WaveGeneration_None            ((uint32_t)0x00000000)  /* 禁用波形生成，直接输出DHR寄存器值 */
#define DAC_WaveGeneration_Noise           ((uint32_t)0x00000040)  /* 启用伪随机噪声波生成（基于LFSR） */
#define DAC_WaveGeneration_Triangle        ((uint32_t)0x00000080)  /* 启用三角波生成 */
/* 波形模式校验宏：确认配置值合法 */
#define IS_DAC_GENERATE_WAVE(WAVE) (((WAVE) == DAC_WaveGeneration_None) || \
                                    ((WAVE) == DAC_WaveGeneration_Noise) || \
                                    ((WAVE) == DAC_WaveGeneration_Triangle))
/**
  * @}
  */

/** @defgroup DAC_lfsrunmask_triangleamplitude LFSR掩码与三角波幅度
  * @brief 噪声波/三角波的具体参数配置
  * @{
  */

/* 噪声波LFSR掩码配置 -----------------------------------------------------*/
#define DAC_LFSRUnmask_Bit0                ((uint32_t)0x00000000) /* 使用LFSR位0生成噪声（周期=2^1-1） */
#define DAC_LFSRUnmask_Bits1_0             ((uint32_t)0x00000100) /* 使用位[1:0]，周期=2^2-1 */
#define DAC_LFSRUnmask_Bits2_0             ((uint32_t)0x00000200) /* 使用位[2:0]，周期=2^3-1 */
#define DAC_LFSRUnmask_Bits3_0             ((uint32_t)0x00000300) /* 使用位[3:0]，周期=2^4-1=15 */
#define DAC_LFSRUnmask_Bits4_0             ((uint32_t)0x00000400) /* 周期=2^5-1=31 */
#define DAC_LFSRUnmask_Bits5_0             ((uint32_t)0x00000500) /* 周期=63 */
#define DAC_LFSRUnmask_Bits6_0             ((uint32_t)0x00000600) /* 周期=127 */
#define DAC_LFSRUnmask_Bits7_0             ((uint32_t)0x00000700) /* 周期=255 */
#define DAC_LFSRUnmask_Bits8_0             ((uint32_t)0x00000800) /* 周期=511 */
#define DAC_LFSRUnmask_Bits9_0             ((uint32_t)0x00000900) /* 周期=1023 */
#define DAC_LFSRUnmask_Bits10_0            ((uint32_t)0x00000A00) /* 周期=2047 */
#define DAC_LFSRUnmask_Bits11_0            ((uint32_t)0x00000B00) /* 最大周期=2^12-1=4095 */

/* 三角波幅度等级配置 -----------------------------------------------------*/
#define DAC_TriangleAmplitude_1            ((uint32_t)0x00000000) /* 峰值幅度=1（最小三角波） */
#define DAC_TriangleAmplitude_3            ((uint32_t)0x00000100) /* 峰值=3，对应CRYP_MAMPx寄存器设置 */
#define DAC_TriangleAmplitude_7            ((uint32_t)0x00000200) /* 峰值=7 */
#define DAC_TriangleAmplitude_15           ((uint32_t)0x00000300) /* 峰值=15 */
#define DAC_TriangleAmplitude_31           ((uint32_t)0x00000400) /* 峰值=31 */
#define DAC_TriangleAmplitude_63           ((uint32_t)0x00000500) /* 峰值=63 */
#define DAC_TriangleAmplitude_127          ((uint32_t)0x00000600) /* 峰值=127 */
#define DAC_TriangleAmplitude_255          ((uint32_t)0x00000700) /* 峰值=255 */
#define DAC_TriangleAmplitude_511          ((uint32_t)0x00000800) /* 峰值=511 */
#define DAC_TriangleAmplitude_1023         ((uint32_t)0x00000900) /* 峰值=1023 */
#define DAC_TriangleAmplitude_2047         ((uint32_t)0x00000A00) /* 峰值=2047 */
#define DAC_TriangleAmplitude_4095         ((uint32_t)0x00000B00) /* 最大峰值=4095（满量程） */

/*
 * 模块名称：DAC参数校验宏
 * 功能说明：本宏定义用于验证波形生成参数的有效性，确保传入DAC配置结构体的LFSR掩码或三角波幅度参数为合法值
 * 关联配置项：必须与DAC_lfsrunmask_triangleamplitude分组中的定义配合使用
 */

/**
 * @brief LFSR掩码/三角波幅度参数校验宏
 * @param VALUE 待验证的参数值（来自DAC_LFSRUnmask_xxx或DAC_TriangleAmplitude_xxx系列）
 * @return 布尔值：true-参数合法 / false-参数非法
 * @note 此宏同时涵盖噪声波LFSR掩码(0x00000000-0x00000B00)和三角波幅度参数(0x00000000-0x00000B00)
 */
#define IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(VALUE) ( \
    /* 检查是否属于LFSR噪声波掩码配置 */                \
    ((VALUE) == DAC_LFSRUnmask_Bit0)     || /* 验证bit0掩码 */       \
    ((VALUE) == DAC_LFSRUnmask_Bits1_0)  || /* 验证bits[1:0]掩码 */   \
    ((VALUE) == DAC_LFSRUnmask_Bits2_0)  || /* ...逐位扩展掩码 */    \
    ((VALUE) == DAC_LFSRUnmask_Bits3_0)  ||                         \
    ((VALUE) == DAC_LFSRUnmask_Bits4_0)  ||                         \
    ((VALUE) == DAC_LFSRUnmask_Bits5_0)  ||                         \
    ((VALUE) == DAC_LFSRUnmask_Bits6_0)  ||                         \
    ((VALUE) == DAC_LFSRUnmask_Bits7_0)  ||                         \
    ((VALUE) == DAC_LFSRUnmask_Bits8_0)  ||                         \
    ((VALUE) == DAC_LFSRUnmask_Bits9_0)  ||                         \
    ((VALUE) == DAC_LFSRUnmask_Bits10_0) ||                         \
    ((VALUE) == DAC_LFSRUnmask_Bits11_0) || /* 最大LFSR掩码配置 */   \
    /* 检查是否属于三角波幅度参数 */                             \
    ((VALUE) == DAC_TriangleAmplitude_1)    || /* 最小幅度1 */      \
    ((VALUE) == DAC_TriangleAmplitude_3)    ||                      \
    ((VALUE) == DAC_TriangleAmplitude_7)    ||                      \
    ((VALUE) == DAC_TriangleAmplitude_15)   ||                      \
    ((VALUE) == DAC_TriangleAmplitude_31)   ||                      \
    ((VALUE) == DAC_TriangleAmplitude_63)   ||                      \
    ((VALUE) == DAC_TriangleAmplitude_127)  ||                      \
    ((VALUE) == DAC_TriangleAmplitude_255)  ||                      \
    ((VALUE) == DAC_TriangleAmplitude_511)  ||                      \
    ((VALUE) == DAC_TriangleAmplitude_1023) ||                      \
    ((VALUE) == DAC_TriangleAmplitude_2047) ||                      \
    ((VALUE) == DAC_TriangleAmplitude_4095)   /* 最大幅度4095 */    \
)
/*
 * 模块名称：STM32F4xx DAC模块综合配置
 * 功能描述：本部分定义DAC模块的多样化配置参数，包含：
 *          - 输出缓冲控制
 *          - 通道选择
 *          - 数据对齐方式
 *          - 波形生成类型
 *          - 数据有效性校验
 *          - 中断与状态标志管理
 * 核心应用：为DAC初始化结构体提供完整的参数校验体系，确保硬件配置的合法性
 */

/** @defgroup DAC_output_buffer 输出缓冲控制
  * @brief 配置DAC输出放大器的工作状态
  * @{
  */
#define DAC_OutputBuffer_Enable  ((uint32_t)0x00000000)  /* 启用输出缓冲（默认），降低输出阻抗 */
#define DAC_OutputBuffer_Disable ((uint32_t)0x00000002)  /* 禁用缓冲，直接输出（高阻抗模式） */
/* 缓冲状态校验宏 */
#define IS_DAC_OUTPUT_BUFFER_STATE(STATE) (((STATE) == DAC_OutputBuffer_Enable) || \
                                           ((STATE) == DAC_OutputBuffer_Disable))
/**
  * @}
  */

/** @defgroup DAC_Channel_selection 通道选择
  * @brief 多通道DAC芯片选择控制
  * @{
  */
#define DAC_Channel_1  ((uint32_t)0x00000000)  /* 选择DAC通道1（对应DAC_OUT1引脚） */
#define DAC_Channel_2  ((uint32_t)0x00000010)  /* 选择DAC通道2（对应DAC_OUT2引脚） */
/* 通道校验宏 */
#define IS_DAC_CHANNEL(CHANNEL) (((CHANNEL) == DAC_Channel_1) || \
                                 ((CHANNEL) == DAC_Channel_2))
/**
  * @}
  */

/** @defgroup DAC_data_alignement 数据对齐方式
  * @brief 设置DAC数据寄存器的对齐格式
  * @{
  */
#define DAC_Align_12b_R  ((uint32_t)0x00000000)  /* 12位右对齐（DORx[11:0]有效） */
#define DAC_Align_12b_L  ((uint32_t)0x00000004)  /* 12位左对齐（DORx[15:4]有效） */
#define DAC_Align_8b_R   ((uint32_t)0x00000008)  /* 8位右对齐（DORx[7:0]有效） */
/* 对齐方式校验宏 */
#define IS_DAC_ALIGN(ALIGN) (((ALIGN) == DAC_Align_12b_R) || \
                             ((ALIGN) == DAC_Align_12b_L) || \
                             ((ALIGN) == DAC_Align_8b_R))
/**
  * @}
  */

/** @defgroup DAC_wave_generation 波形生成类型（冗余定义）
  * @warning 此分组与先前定义存在重复，建议确认硬件库版本
  * @{
  */
#define DAC_Wave_Noise    ((uint32_t)0x00000040)  /* 噪声波生成（同DAC_WaveGeneration_Noise） */
#define DAC_Wave_Triangle ((uint32_t)0x00000080)  /* 三角波生成（同DAC_WaveGeneration_Triangle） */
#define IS_DAC_WAVE(WAVE) (((WAVE) == DAC_Wave_Noise) || \
                           ((WAVE) == DAC_Wave_Triangle))
/**
  * @}
  */

/** @defgroup DAC_data 数据范围校验
  * @brief 验证写入DHR寄存器的数据有效性
  * @{
  */
#define IS_DAC_DATA(DATA) ((DATA) <= 0xFFF0)  /* 最大允许值：0xFFF0（12位模式下为0xFFF） */
/**
  * @}
  */

/** @defgroup DAC_interrupts_definition 中断配置
  * @brief DAC中断类型定义（当前仅支持DMA下溢中断）
  * @{
  */   
#define DAC_IT_DMAUDR  ((uint32_t)0x00002000)  /* DMA下溢中断：当DMA请求被拒绝时触发 */
#define IS_DAC_IT(IT) (((IT) == DAC_IT_DMAUDR)) 
/**
  * @}
  */ 

/** @defgroup DAC_flags_definition 状态标志位
  * @brief DAC状态标志定义
  * @{
  */  
#define DAC_FLAG_DMAUDR  ((uint32_t)0x00002000)  /* DMA下溢标志：DMA请求失败时置位 */
#define IS_DAC_FLAG(FLAG) (((FLAG) == DAC_FLAG_DMAUDR))  
/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */
/*
 * 模块名称：STM32F4xx DAC模块驱动接口
 * 功能描述：本部分提供DAC模块的完整操作接口，包含以下功能组：
 *          - 模块初始化与硬件复位
 *          - 通道数据配置
 *          - 波形生成控制
 *          - DMA传输管理
 *          - 中断与状态标志处理
 * 调用说明：需先初始化DAC通道后，才能执行数据写入或触发操作
 */

/* 导出函数声明 ------------------------------------------------------------*/

/* 硬件复位与初始化 ********************************************************/
/** 
 * @brief 复位DAC模块到默认状态
 * @note 清除所有通道配置，复位后需重新初始化才能使用
 */
void DAC_DeInit(void);

/**
 * @brief DAC通道初始化
 * @param DAC_Channel 通道选择：DAC_Channel_1/2
 * @param DAC_InitStruct 初始化结构体指针（包含触发源/波形/缓冲等参数）
 */
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct);

/**
 * @brief 初始化结构体默认值加载
 * @param DAC_InitStruct 需初始化的结构体指针
 */
void DAC_StructInit(DAC_InitTypeDef* DAC_InitStruct);

/* 通道操作控制 ************************************************************/
/**
 * @brief 使能/禁用指定DAC通道
 * @param NewState ENABLE（启动转换）/DISABLE（关闭通道）
 */
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState);

/**
 * @brief 软件触发控制（单通道）
 * @note 触发后立即启动转换（需配置为软件触发模式）
 */
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);

/**
 * @brief 双通道同步软件触发
 * @warning 仅当双通道都启用且配置相同触发源时有效
 */
void DAC_DualSoftwareTriggerCmd(FunctionalState NewState);

/* 波形与数据管理 **********************************************************/
/**
 * @brief 波形生成控制
 * @param DAC_Wave 波形类型：DAC_Wave_Noise/Triangle
 * @param NewState ENABLE（启动波形生成）/DISABLE
 */
void DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState);

/**
 * @brief 设置通道1数据
 * @param DAC_Align 对齐方式：DAC_Align_12b_R/12b_L/8b_R
 * @param Data 原始数据（12位模式：0-0xFFF，8位模式：0-0xFF）
 */
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);

/**
 * @brief 设置通道2数据（参数同通道1）
 */
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data);

/**
 * @brief 双通道数据同步写入
 * @param Data2 通道2数据（写入DHR8RD或DHR12RD高位）
 * @param Data1 通道1数据（写入低位）
 */
void DAC_SetDualChannelData(uint32_t DAC_Align, uint16_t Data2, uint16_t Data1);

/**
 * @brief 获取当前通道输出值
 * @return uint16_t 实际输出数值（读取DORx寄存器）
 */
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel);

/* DMA传输控制 *************************************************************/
/**
 * @brief 启用/禁用通道DMA请求
 * @note 需配合DAC_IT_DMAUDR中断处理下溢情况
 */
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState);

/* 中断与标志管理 **********************************************************/
/**
 * @brief DAC中断使能控制
 * @param DAC_IT 中断类型：DAC_IT_DMAUDR
 */
void DAC_ITConfig(uint32_t DAC_Channel, uint32_t DAC_IT, FunctionalState NewState);

/**
 * @brief 查询状态标志位
 * @param DAC_FLAG 标志类型：DAC_FLAG_DMAUDR
 * @return FlagStatus SET（标志置位）/RESET
 */
FlagStatus DAC_GetFlagStatus(uint32_t DAC_Channel, uint32_t DAC_FLAG);

/**
 * @brief 清除指定标志位
 */
void DAC_ClearFlag(uint32_t DAC_Channel, uint32_t DAC_FLAG);

/**
 * @brief 获取中断状态
 * @return ITStatus SET（中断挂起）/RESET
 */
ITStatus DAC_GetITStatus(uint32_t DAC_Channel, uint32_t DAC_IT);

/**
 * @brief 清除中断挂起位
 */
void DAC_ClearITPendingBit(uint32_t DAC_Channel, uint32_t DAC_IT);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_DAC_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

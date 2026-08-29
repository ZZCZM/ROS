/**
  ******************************************************************************
  * @file    stm32f4xx_adc.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the ADC firmware 
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
#ifndef __STM32F4xx_ADC_H
#define __STM32F4xx_ADC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup ADC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/** 
  * @brief  ADC初始化结构体定义
  * @note   该结构体用于配置ADC模块的基本工作参数，包含分辨率、转换模式、触发方式等关键设置
  *         适用于单个ADC实例的初始化配置，需通过HAL_ADC_Init()函数加载配置
  *         所有参数需参照STM32F4xx参考手册的ADC章节进行合理配置
  */
typedef struct
{
  /* ADC分辨率配置（影响转换精度和转换时间） 
     可取值参见@ref ADC_resolution 定义的枚举值：
     - ADC_Resolution_12b: 12位分辨率（最大精度）
     - ADC_Resolution_10b: 10位分辨率  
     - ADC_Resolution_8b:  8位分辨率
     - ADC_Resolution_6b:  6位分辨率 */
  uint32_t ADC_Resolution;                

  /* 扫描转换模式使能标志（用于多通道采集）
     - ENABLE:  启用扫描模式，按预置通道序列自动转换多个通道
     - DISABLE: 单次转换模式，每次只转换单个指定通道 */
  FunctionalState ADC_ScanConvMode;       

  /* 连续转换模式使能标志
     - ENABLE:  自动重启转换，持续采集数据
     - DISABLE: 单次转换模式，需要外部触发或软件启动每次转换 */
  FunctionalState ADC_ContinuousConvMode; 

  /* 外部触发边沿选择（配置硬件触发方式）
     可取值参见@ref ADC_external_trigger_edge_for_regular_channels_conversion：
     - 无触发
     - 上升沿触发
     - 下降沿触发
     - 双边沿触发 */
  uint32_t ADC_ExternalTrigConvEdge;      

  /* 外部触发源选择（需与具体硬件连接匹配）
     可取值参见@ref ADC_external_trigger_sources_for_regular_channels_conversion：
     - 定时器触发源（TIMx_TRGO）
     - EXTI线触发
     - 软件触发等 */
  uint32_t ADC_ExternalTrigConv;          

  /* 数据对齐方式配置
     - ADC_DataAlign_Right: 右对齐（默认）
     - ADC_DataAlign_Left:  左对齐（需配合分辨率使用） */
  uint32_t ADC_DataAlign;                 

  /* 规则通道转换次数配置（扫描模式有效）
     - 取值范围：1~16（实际受ADC硬件通道数量限制）
     - 需与ADC_RegularChannelConfig()配置的通道数一致 */
  uint8_t  ADC_NbrOfConversion;          

}ADC_InitTypeDef;

/** 
  * @brief  ADC通用初始化结构体定义
  * @note   该结构体用于配置多个ADC协同工作时的公共参数
  *         主要应用于双ADC/三ADC模式下的时钟、DMA等共享资源配置
  *         需通过HAL_ADC_ConfigChannel()等函数配合使用
  */
typedef struct 
{
  /* ADC工作模式选择（多ADC协同配置）
     可取值参见@ref ADC_Common_mode：
     - 独立模式（单个ADC工作）
     - 双重/三重交替采样模式
     - 同步注入模式等 */
  uint32_t ADC_Mode;                      

  /* ADC预分频器配置（控制ADC时钟频率）
     可取值参见@ref ADC_Prescaler：
     - 分频系数2/4/6/8（需保证ADC时钟≤36MHz） */
  uint32_t ADC_Prescaler;                 

  /* DMA访问模式配置（多ADC数据存储方式）
     可取值参见@ref ADC_Direct_memory_access_mode_for_multi_mode：
     - 禁用DMA
     - 单DMA通道模式
     - 双DMA通道模式（用于大吞吐量传输） */
  uint32_t ADC_DMAAccessMode;             

  /* 双重采样间隔时间配置（双重模式专用）
     可取值参见@ref ADC_delay_between_2_sampling_phases：
     - 5~20个ADC时钟周期的延迟设置 */
  uint32_t ADC_TwoSamplingDelay;          

}ADC_CommonInitTypeDef;

/* 外设有效性检查宏（用于参数校验）
   验证输入的外设指针是否属于有效的ADC实例
   - PERIPH: 外设句柄（ADC1/ADC2/ADC3）
   - 返回值：TRUE（有效） 或 FALSE（无效） */
#define IS_ADC_ALL_PERIPH(PERIPH) (((PERIPH) == ADC1) || \
                                   ((PERIPH) == ADC2) || \
                                   ((PERIPH) == ADC3))  
/**
  * @defgroup ADC_Common_mode
  * @brief ADC工作模式配置组
  * @details 定义多ADC协同工作模式，包含独立模式/双重模式/三重模式的17种组合配置
  *          这些模式决定多个ADC实例之间的协作关系和数据采集方式
  */
  
/* 独立工作模式（单个ADC运行） */
#define ADC_Mode_Independent                       ((uint32_t)0x00000000)       

/* 双重ADC模式配置 -----------------------------------------------------------*/
#define ADC_DualMode_RegSimult_InjecSimult         ((uint32_t)0x00000001)  /*!< 规则组同步+注入组同步模式 */
#define ADC_DualMode_RegSimult_AlterTrig           ((uint32_t)0x00000002)  /*!< 规则组同步+交替触发模式 */
#define ADC_DualMode_InjecSimult                   ((uint32_t)0x00000005)  /*!< 仅注入组同步模式 */
#define ADC_DualMode_RegSimult                     ((uint32_t)0x00000006)  /*!< 仅规则组同步模式 */
#define ADC_DualMode_Interl                        ((uint32_t)0x00000007)  /*!< 交替采集模式（规则组） */
#define ADC_DualMode_AlterTrig                     ((uint32_t)0x00000009)  /*!< 交替触发模式（规则组） */

/* 三重ADC模式配置 -----------------------------------------------------------*/ 
#define ADC_TripleMode_RegSimult_InjecSimult       ((uint32_t)0x00000011)  /*!< 三ADC规则组同步+注入组同步 */
#define ADC_TripleMode_RegSimult_AlterTrig         ((uint32_t)0x00000012)  /*!< 三ADC规则组同步+交替触发 */
#define ADC_TripleMode_InjecSimult                 ((uint32_t)0x00000015)  /*!< 三ADC注入组同步模式 */
#define ADC_TripleMode_RegSimult                   ((uint32_t)0x00000016)  /*!< 三ADC规则组同步模式 */
#define ADC_TripleMode_Interl                      ((uint32_t)0x00000017)  /*!< 三ADC交替采集模式 */
#define ADC_TripleMode_AlterTrig                   ((uint32_t)0x00000019)  /*!< 三ADC交替触发模式 */

/* 模式参数校验宏（用于库函数参数检查）*/
  // 验证模式参数有效性
#define IS_ADC_MODE(MODE) (((MODE) == ADC_Mode_Independent) || \
                           ((MODE) >= ADC_DualMode_RegSimult_InjecSimult && (MODE) <= ADC_TripleMode_AlterTrig))
/**
  * @}
  */

/**
  * @defgroup ADC_Prescaler 
  * @brief ADC时钟预分频配置组
  * @details 控制ADC模块时钟(ADCCLK)的分频系数，需保证最终时钟不超过36MHz
  *          分频系数选择依赖于APB2总线时钟频率（PCLK2）
  */
#define ADC_Prescaler_Div2                         ((uint32_t)0x00000000)  /*!< ADCCLK = PCLK2/2 */
#define ADC_Prescaler_Div4                         ((uint32_t)0x00010000)  /*!< ADCCLK = PCLK2/4 */
#define ADC_Prescaler_Div6                         ((uint32_t)0x00020000)  /*!< ADCCLK = PCLK2/6 */
#define ADC_Prescaler_Div8                         ((uint32_t)0x00030000)  /*!< ADCCLK = PCLK2/8 */

/* 分频系数校验宏 */
  // 验证预分频参数有效性
#define IS_ADC_PRESCALER(PRESCALER) (((PRESCALER) == ADC_Prescaler_Div2) || \
                                     ((PRESCALER) == ADC_Prescaler_Div4) || \
                                     ((PRESCALER) == ADC_Prescaler_Div6) || \
                                     ((PRESCALER) == ADC_Prescaler_Div8))
/**
  * @}
  */

/**
  * @defgroup ADC_Direct_memory_access_mode_for_multi_mode 
  * @brief 多ADC模式DMA传输配置组
  * @details 定义在多ADC协同工作时的DMA数据传输模式，控制采样数据的存储顺序和方式
  */
#define ADC_DMAAccessMode_Disabled      ((uint32_t)0x00000000)     /*!< 禁用DMA传输 */
#define ADC_DMAAccessMode_1             ((uint32_t)0x00004000)     /*!< 模式1：顺序存储（ADC1→ADC2→ADC3）*/
#define ADC_DMAAccessMode_2             ((uint32_t)0x00008000)     /*!< 模式2：交替半字存储（ADC1&ADC2 → ADC2&ADC3）*/
#define ADC_DMAAccessMode_3             ((uint32_t)0x0000C000)     /*!< 模式3：交替字节存储（优化大数据量传输）*/

/* DMA模式校验宏 */
  // 验证DMA模式参数有效性
#define IS_ADC_DMA_ACCESS_MODE(MODE) (((MODE) == ADC_DMAAccessMode_Disabled) || \
                                      ((MODE) == ADC_DMAAccessMode_1) || \
                                      ((MODE) == ADC_DMAAccessMode_2) || \
                                      ((MODE) == ADC_DMAAccessMode_3))

                                     
/**
  * @}
  */ 
/**
  * @defgroup ADC_delay_between_2_sampling_phases 
  * @brief 双采样间隔延迟配置组
  * @details 在双重/三重ADC模式下，定义两个采样阶段之间的间隔周期数
  *          该参数影响多ADC协作时的时序同步精度，需根据传感器特性调整
  */
#define ADC_TwoSamplingDelay_5Cycles               ((uint32_t)0x00000000)  /*!< 5个ADC时钟周期的延迟 */
#define ADC_TwoSamplingDelay_6Cycles               ((uint32_t)0x00000100)  /*!< 6个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_7Cycles               ((uint32_t)0x00000200)  /*!< 7个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_8Cycles               ((uint32_t)0x00000300)  /*!< 8个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_9Cycles               ((uint32_t)0x00000400)  /*!< 9个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_10Cycles              ((uint32_t)0x00000500)  /*!< 10个ADC时钟周期延迟（典型值）*/
#define ADC_TwoSamplingDelay_11Cycles              ((uint32_t)0x00000600)  /*!< 11个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_12Cycles              ((uint32_t)0x00000700)  /*!< 12个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_13Cycles              ((uint32_t)0x00000800)  /*!< 13个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_14Cycles              ((uint32_t)0x00000900)  /*!< 14个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_15Cycles              ((uint32_t)0x00000A00)  /*!< 15个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_16Cycles              ((uint32_t)0x00000B00)  /*!< 16个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_17Cycles              ((uint32_t)0x00000C00)  /*!< 17个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_18Cycles              ((uint32_t)0x00000D00)  /*!< 18个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_19Cycles              ((uint32_t)0x00000E00)  /*!< 19个ADC时钟周期延迟 */
#define ADC_TwoSamplingDelay_20Cycles              ((uint32_t)0x00000F00)  /*!< 最大20个ADC时钟周期延迟 */

/* 延迟参数校验宏（范围5-20周期）*/
  // 验证延迟参数有效性
#define IS_ADC_SAMPLING_DELAY(DELAY) (((DELAY) >= ADC_TwoSamplingDelay_5Cycles) && \
                                      ((DELAY) <= ADC_TwoSamplingDelay_20Cycles))
/**
  * @}
  */

/**
  * @defgroup ADC_resolution 
  * @brief ADC分辨率配置组
  * @details 配置模数转换的精度级别，分辨率越高转换时间越长
  *          选择需权衡精度与转换速度的关系
  */
#define ADC_Resolution_12b                         ((uint32_t)0x00000000)  /*!< 12位分辨率（1.5个时钟周期/位） */
#define ADC_Resolution_10b                         ((uint32_t)0x01000000)  /*!< 10位分辨率（1.2个时钟周期/位） */
#define ADC_Resolution_8b                          ((uint32_t)0x02000000)  /*!< 8位分辨率（1.0个时钟周期/位） */
#define ADC_Resolution_6b                          ((uint32_t)0x03000000)  /*!< 6位分辨率（0.8个时钟周期/位） */

/* 分辨率参数校验宏 */
  // 验证分辨率参数有效性
#define IS_ADC_RESOLUTION(RESOLUTION) (((RESOLUTION) == ADC_Resolution_12b) || \
                                       ((RESOLUTION) == ADC_Resolution_10b) || \
                                       ((RESOLUTION) == ADC_Resolution_8b) || \
                                       ((RESOLUTION) == ADC_Resolution_6b))
/**
  * @}
  */

/**
  * @defgroup ADC_external_trigger_edge_for_regular_channels_conversion 
  * @brief 外部触发边沿配置组
  * @details 定义启动规则通道转换的外部触发信号的有效边沿
  *          需配合具体硬件触发源（定时器/EXTI等）使用
  */
#define ADC_ExternalTrigConvEdge_None          ((uint32_t)0x00000000)  /*!< 禁用外部触发（使用软件触发） */
#define ADC_ExternalTrigConvEdge_Rising        ((uint32_t)0x10000000)  /*!< 上升沿触发转换 */
#define ADC_ExternalTrigConvEdge_Falling       ((uint32_t)0x20000000)  /*!< 下降沿触发转换 */
#define ADC_ExternalTrigConvEdge_RisingFalling ((uint32_t)0x30000000)  /*!< 双边沿触发转换 */

/* 触发边沿校验宏 */
  // 验证触发边沿参数有效性
#define IS_ADC_EXT_TRIG_EDGE(EDGE) (((EDGE) == ADC_ExternalTrigConvEdge_None) || \
                             ((EDGE) == ADC_ExternalTrigConvEdge_Rising) || \
                             ((EDGE) == ADC_ExternalTrigConvEdge_Falling) || \
                             ((EDGE) == ADC_ExternalTrigConvEdge_RisingFalling))

/**
  * @}
  */ 

/**
  * @defgroup ADC_extrenal_trigger_sources_for_regular_channels_conversion 
  * @brief ADC外部触发源配置组
  * @details 定义规则通道转换的硬件触发源，支持15种定时器事件和1个外部中断线
  *          触发源需配合具体外设(TIMx)配置使用，实现硬件级同步采样
  */
#define ADC_ExternalTrigConv_T1_CC1                ((uint32_t)0x00000000)  /*!< TIM1 CC1事件触发（通道1比较匹配） */
#define ADC_ExternalTrigConv_T1_CC2                ((uint32_t)0x01000000)  /*!< TIM1 CC2事件触发（通道2比较匹配） */
#define ADC_ExternalTrigConv_T1_CC3                ((uint32_t)0x02000000)  /*!< TIM1 CC3事件触发（通道3比较匹配） */
#define ADC_ExternalTrigConv_T2_CC2                ((uint32_t)0x03000000)  /*!< TIM2 CC2事件触发 */
#define ADC_ExternalTrigConv_T2_CC3                ((uint32_t)0x04000000)  /*!< TIM2 CC3事件触发 */
#define ADC_ExternalTrigConv_T2_CC4                ((uint32_t)0x05000000)  /*!< TIM2 CC4事件触发 */
#define ADC_ExternalTrigConv_T2_TRGO               ((uint32_t)0x06000000)  /*!< TIM2 TRGO事件（定时器主模式输出） */
#define ADC_ExternalTrigConv_T3_CC1                ((uint32_t)0x07000000)  /*!< TIM3 CC1事件触发 */
#define ADC_ExternalTrigConv_T3_TRGO               ((uint32_t)0x08000000)  /*!< TIM3 TRGO事件触发 */
#define ADC_ExternalTrigConv_T4_CC4                ((uint32_t)0x09000000)  /*!< TIM4 CC4事件触发 */
#define ADC_ExternalTrigConv_T5_CC1                ((uint32_t)0x0A000000)  /*!< TIM5 CC1事件触发（32位定时器） */
#define ADC_ExternalTrigConv_T5_CC2                ((uint32_t)0x0B000000)  /*!< TIM5 CC2事件触发 */
#define ADC_ExternalTrigConv_T5_CC3                ((uint32_t)0x0C000000)  /*!< TIM5 CC3事件触发 */
#define ADC_ExternalTrigConv_T8_CC1                ((uint32_t)0x0D000000)  /*!< TIM8 CC1事件触发（高级定时器） */
#define ADC_ExternalTrigConv_T8_TRGO               ((uint32_t)0x0E000000)  /*!< TIM8 TRGO事件触发 */
#define ADC_ExternalTrigConv_Ext_IT11              ((uint32_t)0x0F000000)  /*!< 外部中断线11触发（EXTI11） */

/* 触发源参数校验宏 */
  // 验证16种有效触发源
#define IS_ADC_EXT_TRIG(REGTRIG) (((REGTRIG) == ADC_ExternalTrigConv_T1_CC1) || \
                                  ((REGTRIG) <= ADC_ExternalTrigConv_Ext_IT11))     // 利用连续地址特性优化校验
/**
  * @}
  */

/**
  * @defgroup ADC_data_align 
  * @brief ADC数据对齐方式配置组
  * @details 控制转换结果在数据寄存器中的存储格式
  *          需根据后续数据处理需求选择对齐方式
  */
#define ADC_DataAlign_Right                        ((uint32_t)0x00000000)  /*!< 右对齐（默认方式，数据高位在寄存器高位） */
#define ADC_DataAlign_Left                         ((uint32_t)0x00000800)  /*!< 左对齐（数据高位在寄存器低位，方便快速读取） */

/* 对齐方式参数校验宏 */
  // 验证对齐参数有效性
#define IS_ADC_DATA_ALIGN(ALIGN) (((ALIGN) == ADC_DataAlign_Right) || \
                                  ((ALIGN) == ADC_DataAlign_Left))

/**
  * @}
  */ 

/**
  * @defgroup ADC_channels 
  * @brief ADC通道选择配置组
  * @details 定义19个可选的模拟输入通道，包含16个外部引脚通道和3个内部信号通道
  *          特殊通道（温度/参考电压）需在CubeMX中启用相关功能才能正常使用
  */
#define ADC_Channel_0                               ((uint8_t)0x00)  /*!< 通道0 对应GPIO引脚PA0 */
#define ADC_Channel_1                               ((uint8_t)0x01)  /*!< 通道1 对应GPIO引脚PA1 */
#define ADC_Channel_2                               ((uint8_t)0x02)  /*!< 通道2 对应GPIO引脚PA2 */
#define ADC_Channel_3                               ((uint8_t)0x03)  /*!< 通道3 对应GPIO引脚PA3 */
#define ADC_Channel_4                               ((uint8_t)0x04)  /*!< 通道4 对应GPIO引脚PA4 */
#define ADC_Channel_5                               ((uint8_t)0x05)  /*!< 通道5 对应GPIO引脚PA5 */
#define ADC_Channel_6                               ((uint8_t)0x06)  /*!< 通道6 对应GPIO引脚PA6 */
#define ADC_Channel_7                               ((uint8_t)0x07)  /*!< 通道7 对应GPIO引脚PA7 */
#define ADC_Channel_8                               ((uint8_t)0x08)  /*!< 通道8 对应GPIO引脚PB0 */
#define ADC_Channel_9                               ((uint8_t)0x09)  /*!< 通道9 对应GPIO引脚PB1 */
#define ADC_Channel_10                              ((uint8_t)0x0A)  /*!< 通道10 对应GPIO引脚PC0 */
#define ADC_Channel_11                              ((uint8_t)0x0B)  /*!< 通道11 对应GPIO引脚PC1 */
#define ADC_Channel_12                              ((uint8_t)0x0C)  /*!< 通道12 对应GPIO引脚PC2 */
#define ADC_Channel_13                              ((uint8_t)0x0D)  /*!< 通道13 对应GPIO引脚PC3 */
#define ADC_Channel_14                              ((uint8_t)0x0E)  /*!< 通道14 对应GPIO引脚PC4 */
#define ADC_Channel_15                              ((uint8_t)0x0F)  /*!< 通道15 对应GPIO引脚PC5 */
#define ADC_Channel_16                              ((uint8_t)0x10)  /*!< 通道16 内部温度传感器（芯片型号相关）*/
#define ADC_Channel_17                              ((uint8_t)0x11)  /*!< 通道17 内部参考电压VREFINT */
#define ADC_Channel_18                              ((uint8_t)0x12)  /*!< 通道18 电池电压监测VBAT */

/* 芯片型号特定的温度传感器通道配置 */
#if defined (STM32F40_41xxx)
#define ADC_Channel_TempSensor                      ((uint8_t)ADC_Channel_16)  /*!< F40x/41x系列温度传感器通道 */
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx) || defined (STM32F401xx) || defined (STM32F411xE)
#define ADC_Channel_TempSensor                      ((uint8_t)ADC_Channel_18)  /*!< F427/429/401/411系列温度传感器通道 */
#endif /* STM32F427_437xx || STM32F429_439xx || STM32F401xx || STM32F411xE */

/* 固定内部信号通道 */
#define ADC_Channel_Vrefint                         ((uint8_t)ADC_Channel_17)  /*!< 内部1.2V参考电压，用于校准测量 */
#define ADC_Channel_Vbat                            ((uint8_t)ADC_Channel_18)  /*!< 电池供电电压监测，需连接外部滤波电路 */
/** 
  * @brief  ADC通道参数校验宏
  * @param  CHANNEL 待验证的ADC通道号
  * @retval 布尔类型，true表示有效通道，false表示无效通道
  * @note   该宏用于库函数参数校验，确保传入的通道号在硬件支持范围内
  *         涵盖所有19个有效通道（0-18），包含常规GPIO通道和特殊内部通道
  */
#define IS_ADC_CHANNEL(CHANNEL) ( \
    ((CHANNEL) == ADC_Channel_0)  ||  /* 校验通道0（PA0）有效性 */ \
    ((CHANNEL) == ADC_Channel_1)  ||  /* 校验通道1（PA1）有效性 */ \
    ((CHANNEL) == ADC_Channel_2)  ||  /* 校验通道2（PA2）有效性 */ \
    ((CHANNEL) == ADC_Channel_3)  ||  /* 校验通道3（PA3）有效性 */ \
    ((CHANNEL) == ADC_Channel_4)  ||  /* 校验通道4（PA4）有效性 */ \
    ((CHANNEL) == ADC_Channel_5)  ||  /* 校验通道5（PA5）有效性 */ \
    ((CHANNEL) == ADC_Channel_6)  ||  /* 校验通道6（PA6）有效性 */ \
    ((CHANNEL) == ADC_Channel_7)  ||  /* 校验通道7（PA7）有效性 */ \
    ((CHANNEL) == ADC_Channel_8)  ||  /* 校验通道8（PB0）有效性 */ \
    ((CHANNEL) == ADC_Channel_9)  ||  /* 校验通道9（PB1）有效性 */ \
    ((CHANNEL) == ADC_Channel_10) ||  /* 校验通道10（PC0）有效性 */ \
    ((CHANNEL) == ADC_Channel_11) ||  /* 校验通道11（PC1）有效性 */ \
    ((CHANNEL) == ADC_Channel_12) ||  /* 校验通道12（PC2）有效性 */ \
    ((CHANNEL) == ADC_Channel_13) ||  /* 校验通道13（PC3）有效性 */ \
    ((CHANNEL) == ADC_Channel_14) ||  /* 校验通道14（PC4）有效性 */ \
    ((CHANNEL) == ADC_Channel_15) ||  /* 校验通道15（PC5）有效性 */ \
    ((CHANNEL) == ADC_Channel_16) ||  /* 校验内部温度传感器通道（芯片相关） */ \
    ((CHANNEL) == ADC_Channel_17) ||  /* 校验内部基准电压VREFINT通道 */ \
    ((CHANNEL) == ADC_Channel_18)     /* 校验电池电压VBAT监测通道 */ \
)


/**
  * @defgroup ADC_sampling_times 
  * @brief ADC采样时间配置组
  * @details 定义模拟信号采样阶段的持续时间，采样时间越长转换精度越高但速度越慢
  *          选择时需平衡信号源阻抗与转换速率需求，遵循公式：t_samp = (周期数 + 12.5)/ADCCLK
  */
#define ADC_SampleTime_3Cycles                    ((uint8_t)0x00)  /*!< 3周期采样（最快速度，适合低阻抗信号源） */
#define ADC_SampleTime_15Cycles                   ((uint8_t)0x01)  /*!< 15周期采样（常规快速采样） */
#define ADC_SampleTime_28Cycles                   ((uint8_t)0x02)  /*!< 28周期采样（中等精度） */
#define ADC_SampleTime_56Cycles                   ((uint8_t)0x03)  /*!< 56周期采样（推荐用于标准阻抗1-10kΩ） */
#define ADC_SampleTime_84Cycles                   ((uint8_t)0x04)  /*!< 84周期采样（高精度模式） */
#define ADC_SampleTime_112Cycles                  ((uint8_t)0x05)  /*!< 112周期采样（高阻抗信号源>10kΩ） */
#define ADC_SampleTime_144Cycles                  ((uint8_t)0x06)  /*!< 144周期采样（超高阻抗信号源） */
#define ADC_SampleTime_480Cycles                  ((uint8_t)0x07)  /*!< 480周期采样（特殊传感器应用，如温度检测） */

/* 采样时间参数校验宏 */
#define IS_ADC_SAMPLE_TIME(TIME) ( \
    ((TIME) == ADC_SampleTime_3Cycles)   ||  /* 验证3周期模式 */ \
    ((TIME) == ADC_SampleTime_15Cycles)  ||  /* 验证15周期模式 */ \
    ((TIME) == ADC_SampleTime_28Cycles)  ||  /* 验证28周期模式 */ \
    ((TIME) == ADC_SampleTime_56Cycles)  ||  /* 验证56周期模式 */ \
    ((TIME) == ADC_SampleTime_84Cycles)  ||  /* 验证84周期模式 */ \
    ((TIME) == ADC_SampleTime_112Cycles)||  /* 验证112周期模式 */ \
    ((TIME) == ADC_SampleTime_144Cycles)||  /* 验证144周期模式 */ \
    ((TIME) == ADC_SampleTime_480Cycles)    /* 验证480周期模式 */ \
)

/**
  * @defgroup ADC_external_trigger_edge_for_injected_channels_conversion 
  * @brief 注入通道外部触发边沿配置组
  * @details 配置注入通道转换的硬件触发条件，可设置4种触发边沿模式
  *          注入通道触发具有中断抢占特性，适用于紧急采样任务
  */
#define ADC_ExternalTrigInjecConvEdge_None          ((uint32_t)0x00000000)  /*!< 禁用注入通道硬件触发 */
#define ADC_ExternalTrigInjecConvEdge_Rising        ((uint32_t)0x00100000)  /*!< 上升沿触发注入转换 */
#define ADC_ExternalTrigInjecConvEdge_Falling       ((uint32_t)0x00200000)  /*!< 下降沿触发注入转换 */
#define ADC_ExternalTrigInjecConvEdge_RisingFalling ((uint32_t)0x00300000)  /*!< 双边沿触发注入转换 */

/* 注入触发边沿校验宏 */
#define IS_ADC_EXT_INJEC_TRIG_EDGE(EDGE) ( \
    ((EDGE) == ADC_ExternalTrigInjecConvEdge_None) ||  /* 验证无触发模式 */ \
    ((EDGE) == ADC_ExternalTrigInjecConvEdge_Rising) || /* 验证上升沿触发 */ \
    ((EDGE) == ADC_ExternalTrigInjecConvEdge_Falling) || /* 验证下降沿触发 */ \
    ((EDGE) == ADC_ExternalTrigInjecConvEdge_RisingFalling) /* 验证双边沿触发 */ \
)

/**
  * @}
  */ 

/**
  * @defgroup ADC_extrenal_trigger_sources_for_injected_channels_conversion 
  * @brief 注入通道外部触发源配置组
  * @details 定义16种注入通道专用硬件触发源，包含定时器比较事件/TRGO事件/外部中断
  *          触发源配置需与对应定时器的工作模式严格匹配，建议通过CubeMX工具配置
  */
#define ADC_ExternalTrigInjecConv_T1_CC4            ((uint32_t)0x00000000)  /*!< TIM1通道4比较匹配事件触发（高级定时器） */
#define ADC_ExternalTrigInjecConv_T1_TRGO           ((uint32_t)0x00010000)  /*!< TIM1主模式输出触发（TRGO事件） */
#define ADC_ExternalTrigInjecConv_T2_CC1            ((uint32_t)0x00020000)  /*!< TIM2通道1比较匹配事件触发 */
#define ADC_ExternalTrigInjecConv_T2_TRGO           ((uint32_t)0x00030000)  /*!< TIM2主模式输出触发 */
#define ADC_ExternalTrigInjecConv_T3_CC2            ((uint32_t)0x00040000)  /*!< TIM3通道2比较匹配事件触发 */
#define ADC_ExternalTrigInjecConv_T3_CC4            ((uint32_t)0x00050000)  /*!< TIM3通道4比较匹配事件触发 */
#define ADC_ExternalTrigInjecConv_T4_CC1            ((uint32_t)0x00060000)  /*!< TIM4通道1比较匹配事件触发 */
#define ADC_ExternalTrigInjecConv_T4_CC2            ((uint32_t)0x00070000)  /*!< TIM4通道2比较匹配事件触发 */
#define ADC_ExternalTrigInjecConv_T4_CC3            ((uint32_t)0x00080000)  /*!< TIM4通道3比较匹配事件触发 */
#define ADC_ExternalTrigInjecConv_T4_TRGO           ((uint32_t)0x00090000)  /*!< TIM4主模式输出触发 */
#define ADC_ExternalTrigInjecConv_T5_CC4            ((uint32_t)0x000A0000)  /*!< TIM5通道4比较匹配事件触发（32位定时器） */
#define ADC_ExternalTrigInjecConv_T5_TRGO           ((uint32_t)0x000B0000)  /*!< TIM5主模式输出触发 */
#define ADC_ExternalTrigInjecConv_T8_CC2            ((uint32_t)0x000C0000)  /*!< TIM8通道2比较匹配事件触发（高级定时器） */
#define ADC_ExternalTrigInjecConv_T8_CC3            ((uint32_t)0x000D0000)  /*!< TIM8通道3比较匹配事件触发 */
#define ADC_ExternalTrigInjecConv_T8_CC4            ((uint32_t)0x000E0000)  /*!< TIM8通道4比较匹配事件触发 */
#define ADC_ExternalTrigInjecConv_Ext_IT15          ((uint32_t)0x000F0000)  /*!< 外部中断线15触发（EXTI15） */

/* 触发源参数校验宏（严格匹配16种预设值）*/
#define IS_ADC_EXT_INJEC_TRIG(INJTRIG) ( \
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T1_CC4) ||     /* 校验TIM1 CC4触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T1_TRGO) ||     /* 校验TIM1 TRGO触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T2_CC1) ||      /* 校验TIM2 CC1触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T2_TRGO) ||     /* 校验TIM2 TRGO触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T3_CC2) ||      /* 校验TIM3 CC2触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T3_CC4) ||      /* 校验TIM3 CC4触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T4_CC1) ||      /* 校验TIM4 CC1触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T4_CC2) ||      /* 校验TIM4 CC2触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T4_CC3) ||      /* 校验TIM4 CC3触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T4_TRGO) ||     /* 校验TIM4 TRGO触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T5_CC4) ||      /* 校验TIM5 CC4触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T5_TRGO) ||     /* 校验TIM5 TRGO触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T8_CC2) ||      /* 校验TIM8 CC2触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T8_CC3) ||      /* 校验TIM8 CC3触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_T8_CC4) ||      /* 校验TIM8 CC4触发 */\
    ((INJTRIG) == ADC_ExternalTrigInjecConv_Ext_IT15)       /* 校验EXTI15触发 */ \
)

/**
  * @defgroup ADC_injected_channel_selection 
  * @brief 注入通道序号配置组
  * @details 定义4个注入通道的配置偏移量，用于设置JSQR寄存器
  *          注入通道具有抢占式转换特性，可中断常规通道转换
  */
#define ADC_InjectedChannel_1                       ((uint8_t)0x14)  /*!< 注入序列1（JSQ1位域偏移量20） */
#define ADC_InjectedChannel_2                       ((uint8_t)0x18)  /*!< 注入序列2（JSQ2位域偏移量24） */
#define ADC_InjectedChannel_3                       ((uint8_t)0x1C)  /*!< 注入序列3（JSQ3位域偏移量28） */
#define ADC_InjectedChannel_4                       ((uint8_t)0x20)  /*!< 注入序列4（JSQ4位域偏移量32） */

/* 注入通道序号校验宏 */

#define IS_ADC_INJECTED_CHANNEL(CHANNEL) ( \
    ((CHANNEL) == ADC_InjectedChannel_1) ||     /* 校验注入通道1配置值 */\
    ((CHANNEL) == ADC_InjectedChannel_2) ||      /* 校验注入通道2配置值 */\
    ((CHANNEL) == ADC_InjectedChannel_3) ||      /* 校验注入通道3配置值 */\
    ((CHANNEL) == ADC_InjectedChannel_4)          /* 校验注入通道4配置值 */ \
)

/**
  * @}
  */ 

/**
  * @defgroup ADC_analog_watchdog_selection 
  * @brief 模拟看门狗模式配置组
  * @details 配置电压监测范围，当被监测通道的转换值超出设定阈值时触发中断
  *          支持单通道/全通道、规则/注入通道组合监测模式
  */
#define ADC_AnalogWatchdog_SingleRegEnable         ((uint32_t)0x00800200)  /*!< 监测单个规则通道（需设定通道号及阈值） */
#define ADC_AnalogWatchdog_SingleInjecEnable       ((uint32_t)0x00400200)  /*!< 监测单个注入通道（需设定通道号及阈值） */
#define ADC_AnalogWatchdog_SingleRegOrInjecEnable  ((uint32_t)0x00C00200)  /*!< 同时监测单个规则+注入通道 */
#define ADC_AnalogWatchdog_AllRegEnable            ((uint32_t)0x00800000)  /*!< 监测所有规则通道（需设定阈值范围） */
#define ADC_AnalogWatchdog_AllInjecEnable          ((uint32_t)0x00400000)  /*!< 监测所有注入通道（需设定阈值范围） */
#define ADC_AnalogWatchdog_AllRegAllInjecEnable    ((uint32_t)0x00C00000)  /*!< 监测所有规则+注入通道 */
#define ADC_AnalogWatchdog_None                    ((uint32_t)0x00000000)  /*!< 禁用模拟看门狗功能 */

/* 看门狗模式校验宏 */
#define IS_ADC_ANALOG_WATCHDOG(WATCHDOG) ( \
    ((WATCHDOG) == ADC_AnalogWatchdog_SingleRegEnable) || /* 单规则通道模式 */ \
    ((WATCHDOG) == ADC_AnalogWatchdog_SingleInjecEnable) || /* 单注入通道模式 */ \
    ((WATCHDOG) == ADC_AnalogWatchdog_SingleRegOrInjecEnable) || /* 单规则+单注入模式 */ \
    ((WATCHDOG) == ADC_AnalogWatchdog_AllRegEnable) || /* 全规则通道模式 */ \
    ((WATCHDOG) == ADC_AnalogWatchdog_AllInjecEnable) || /* 全注入通道模式 */ \
    ((WATCHDOG) == ADC_AnalogWatchdog_AllRegAllInjecEnable) || /* 全规则+全注入模式 */ \
    ((WATCHDOG) == ADC_AnalogWatchdog_None)                        /* 禁用模式 */ \
)

/**
  * @defgroup ADC_interrupts_definition 
  * @brief ADC中断类型配置组
  * @details 定义4种可屏蔽中断类型，需配合NVIC配置使用
  */
#define ADC_IT_EOC                                 ((uint16_t)0x0205)  /*!< 规则通道转换完成中断（EOC置位时触发） */
#define ADC_IT_AWD                                 ((uint16_t)0x0106)  /*!< 模拟看门狗阈值越界中断（AWD置位时触发） */
#define ADC_IT_JEOC                                ((uint16_t)0x0407)  /*!< 注入通道转换完成中断（JEOC置位时触发） */
#define ADC_IT_OVR                                 ((uint16_t)0x201A)  /*!< 数据溢出中断（DR寄存器未读取时新数据覆盖） */

/* 中断类型校验宏 */
#define IS_ADC_IT(IT) ( \
    ((IT) == ADC_IT_EOC) ||/* 规则转换完成中断 */ \
    ((IT) == ADC_IT_AWD) || /* 看门狗越界中断 */ \
    ((IT) == ADC_IT_JEOC)|| /* 注入转换完成中断 */ \
    ((IT) == ADC_IT_OVR)         /* 数据溢出中断 */ \
)

/**
  * @defgroup ADC_flags_definition 
  * @brief ADC状态标志位定义组
  * @details 反映ADC工作状态的6个标志位，通过SR寄存器访问
  */
#define ADC_FLAG_AWD                               ((uint8_t)0x01)  /*!< 模拟看门狗触发标志（SR寄存器位0） */
#define ADC_FLAG_EOC                               ((uint8_t)0x02)  /*!< 规则通道转换完成标志（SR寄存器位1） */
#define ADC_FLAG_JEOC                              ((uint8_t)0x04)  /*!< 注入通道转换完成标志（SR寄存器位2） */
#define ADC_FLAG_JSTRT                             ((uint8_t)0x08)  /*!< 注入通道转换启动标志（SR寄存器位3） */
#define ADC_FLAG_STRT                              ((uint8_t)0x10)  /*!< 规则通道转换启动标志（SR寄存器位4） */
#define ADC_FLAG_OVR                               ((uint8_t)0x20)  /*!< 数据溢出标志（SR寄存器位5） */

/* 标志清除校验宏（仅允许清除可写标志） */
#define IS_ADC_CLEAR_FLAG(FLAG) ( \
    (((FLAG) & 0xC0) == 0x00) && \  /* 屏蔽保留位 */ \
    ((FLAG) != 0x00)               /* 排除空值 */ \
)

/* 标志读取校验宏 */
#define IS_ADC_GET_FLAG(FLAG) ( \
    ((FLAG) == ADC_FLAG_AWD) ||/* 看门狗标志 */ \
    ((FLAG) == ADC_FLAG_EOC) || /* 规则完成标志 */ \
    ((FLAG) == ADC_FLAG_JEOC) ||/* 注入完成标志 */ \
    ((FLAG) == ADC_FLAG_JSTRT) ||/* 注入启动标志 */ \
    ((FLAG) == ADC_FLAG_STRT) || /* 规则启动标志 */ \
    ((FLAG) == ADC_FLAG_OVR)       /* 溢出标志 */ \
)

/**
  * @}
  */ 
/**
  * @defgroup ADC_thresholds 
  * @brief 模拟看门狗阈值校验宏
  * @param THRESHOLD 待验证的阈值数值（0x000-0xFFF）
  * @retval 布尔值，true表示有效12位阈值
  * @note 阈值需根据ADC分辨率设置，12位模式下最大值为4095
  */
#define IS_ADC_THRESHOLD(THRESHOLD) ((THRESHOLD) <= 0xFFF)  /* 12位阈值范围校验 */

/**
  * @defgroup ADC_injected_offset 
  * @brief 注入通道偏移量校验宏
  * @param OFFSET 待验证的偏移量（0x000-0xFFF）
  * @retval 布尔值，true表示有效12位偏移量
  * @note 偏移量用于注入通道的数值补偿计算
  */
#define IS_ADC_OFFSET(OFFSET) ((OFFSET) <= 0xFFF)  /* 12位偏移量范围校验 */

/**
  * @defgroup ADC_injected_length 
  * @brief 注入通道序列长度校验宏
  * @param LENGTH 待验证的序列长度（1-4）
  * @retval 布尔值，true表示有效注入序列长度
  * @note 对应JSQR寄存器的JL[1:0]位配置，最大支持4通道注入序列
  */
#define IS_ADC_INJECTED_LENGTH(LENGTH) (((LENGTH) >= 0x1) && ((LENGTH) <= 0x4))  /* 注入序列长度范围校验 */

/**
  * @defgroup ADC_injected_rank 
  * @brief 注入通道序号校验宏
  * @param RANK 待验证的通道序号（1-4）
  * @retval 布尔值，true表示有效注入通道序号
  * @note 对应JSQx寄存器配置，序号需与序列长度匹配
  */
#define IS_ADC_INJECTED_RANK(RANK) (((RANK) >= 0x1) && ((RANK) <= 0x4))  /* 注入通道序号范围校验 */

/**
  * @defgroup ADC_regular_length 
  * @brief 规则通道序列长度校验宏
  * @param LENGTH 待验证的序列长度（1-16）
  * @retval 布尔值，true表示有效规则序列长度
  * @note 对应SQR1寄存器的L[3:0]位配置，最大支持16通道规则序列
  */
#define IS_ADC_REGULAR_LENGTH(LENGTH) (((LENGTH) >= 0x1) && ((LENGTH) <= 0x10))  /* 规则序列长度范围校验 */

/**
  * @defgroup ADC_regular_rank 
  * @brief 规则通道序号校验宏
  * @param RANK 待验证的通道序号（1-16）
  * @retval 布尔值，true表示有效规则通道序号
  * @note 对应SQx寄存器配置，序号需与序列长度匹配
  */
#define IS_ADC_REGULAR_RANK(RANK) (((RANK) >= 0x1) && ((RANK) <= 0x10))  /* 规则通道序号范围校验 */

/**
  * @defgroup ADC_regular_discontinuous_mode_number 
  * @brief 规则通道不连续模式分组数校验宏
  * @param NUMBER 待验证的分组数量（1-8）
  * @retval 布尔值，true表示有效分组数量
  * @note 控制不连续模式下每次触发的转换次数，对应CR1寄存器的DISCNUM[2:0]位
  */
#define IS_ADC_REGULAR_DISC_NUMBER(NUMBER) (((NUMBER) >= 0x1) && ((NUMBER) <= 0x8))  /* 不连续模式分组数校验 */

/**
  * @}
  */ 


/**
  * @}
  */ 
/*--------------------------- 导出宏定义 ----------------------------*/
/*--------------------------- 导出函数 ----------------------------*/  

/**
  * @brief  将ADC配置恢复为默认复位状态
  * @note   复位所有ADC寄存器，清除所有配置
  * @retval 无
  */
void ADC_DeInit(void);

/*---------------------- 初始化与配置函数组 ----------------------*/
/**
  * @brief  ADC模块初始化
  * @param  ADCx 目标ADC外设(ADC1/ADC2/ADC3)
  * @param  ADC_InitStruct 初始化参数结构体指针
  * @retval 无
  * @note   需先调用ADC_StructInit初始化结构体
  */
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);

/**
  * @brief  初始化ADC_InitTypeDef结构体为默认值
  * @param  ADC_InitStruct 结构体指针
  * @retval 无
  * @note   默认参数：12位分辨率、单次转换模式、软件触发
  */
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct);

/**
  * @brief  多ADC模式公共参数初始化
  * @param  ADC_CommonInitStruct 公共参数结构体指针
  * @retval 无
  * @note   用于配置ADC预分频器、DMA模式等共享参数
  */
void ADC_CommonInit(ADC_CommonInitTypeDef* ADC_CommonInitStruct);

/**
  * @brief  初始化ADC_CommonInitTypeDef结构体为默认值
  * @param  ADC_CommonInitStruct 结构体指针
  * @retval 无
  * @note   默认参数：独立模式、PCLK2二分频
  */
void ADC_CommonStructInit(ADC_CommonInitTypeDef* ADC_CommonInitStruct);

/**
  * @brief  ADC外设使能控制
  * @param  ADCx 目标ADC外设
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   使能后ADC进入待机模式，需执行校准后启动转换
  */
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/*------------------- 模拟看门狗配置函数组 -------------------*/
/**
  * @brief  配置模拟看门狗工作模式
  * @param  ADCx 目标ADC外设
  * @param  ADC_AnalogWatchdog 看门狗模式选择
  * @retval 无
  * @note   需配合ADC_AnalogWatchdogThresholdsConfig设置阈值
  */
void ADC_AnalogWatchdogCmd(ADC_TypeDef* ADCx, uint32_t ADC_AnalogWatchdog);

/**
  * @brief  设置模拟看门狗阈值
  * @param  ADCx 目标ADC外设
  * @param  HighThreshold 高阈值(0-4095)
  * @param  LowThreshold 低阈值(0-4095)
  * @retval 无
  * @note   阈值范围根据ADC分辨率调整，12位模式最大4095
  */
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef* ADCx, uint16_t HighThreshold,uint16_t LowThreshold);

/**
  * @brief  配置单通道看门狗监测
  * @param  ADCx 目标ADC外设
  * @param  ADC_Channel 监测通道号(0-18)
  * @retval 无
  * @note   仅在单通道看门狗模式下有效
  */
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel);

/*-------------- 内部传感器管理函数组 --------------*/
/**
  * @brief  温度传感器和内部参考电压使能控制
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   使能后需等待TSVREFE位稳定（约10us）
  */
void ADC_TempSensorVrefintCmd(FunctionalState NewState);

/**
  * @brief  VBAT电池电压通道使能控制
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   使能后需配置通道18进行采样
  */
void ADC_VBATCmd(FunctionalState NewState);

/*----------------- 规则通道配置函数组 -----------------*/
/**
  * @brief  配置规则通道转换参数
  * @param  ADCx 目标ADC外设
  * @param  ADC_Channel 通道号(0-18)
  * @param  Rank 转换序列位置(1-16)
  * @param  ADC_SampleTime 采样时间(3-480周期)
  * @retval 无
  * @note   需在ADC_Init之后调用，配置SQRx和SMPx寄存器
  */
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);

/**
  * @brief  启动软件触发转换
  * @param  ADCx 目标ADC外设
  * @retval 无
  * @note   仅当配置为软件触发模式时有效
  */
void ADC_SoftwareStartConv(ADC_TypeDef* ADCx);

/**
  * @brief  获取软件触发状态
  * @param  ADCx 目标ADC外设
  * @retval FlagStatus 触发状态(SET/RESET)
  * @note   用于检测SWSTART位是否被置位
  */
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef* ADCx);

/**
  * @brief  使能每个规则通道转换后产生EOC中断
  * @param  ADCx 目标ADC外设
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   默认在序列转换完成后产生一次EOC
  */
void ADC_EOCOnEachRegularChannelCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
/*------------------------ 规则通道高级配置函数组 ------------------------*/
/**
  * @brief  使能ADC连续转换模式
  * @param  ADCx 目标ADC外设
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   使能后ADC自动重启转换，需配合DMA或中断使用
  */
void ADC_ContinuousModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/**
  * @brief  配置不连续模式通道分组数
  * @param  ADCx 目标ADC外设
  * @param  Number 分组数量(1-8)
  * @retval 无
  * @note   设置每次触发转换的通道数，对应CR1寄存器的DISCNUM[2:0]
  */
void ADC_DiscModeChannelCountConfig(ADC_TypeDef* ADCx, uint8_t Number);

/**
  * @brief  使能规则通道不连续转换模式
  * @param  ADCx 目标ADC外设
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   需先配置分组数量，每个触发信号转换指定数量的通道
  */
void ADC_DiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/**
  * @brief  获取规则通道转换结果
  * @param  ADCx 目标ADC外设
  * @retval uint16_t 12位转换结果（右对齐）
  * @note   读取后自动清除EOC标志，需在EOC标志置位后调用
  */
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);

/**
  * @brief  获取多ADC模式下的组合转换结果
  * @retval uint32_t 32位组合数据（ADC1低16位，ADC2高16位）
  * @note   仅在双/三ADC同步模式下有效，需使能DMA传输
  */
uint32_t ADC_GetMultiModeConversionValue(void);

/*----------------------- 规则通道DMA配置函数组 -----------------------*/
/**
  * @brief  使能ADC DMA传输
  * @param  ADCx 目标ADC外设
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   使能后每次EOC事件触发DMA请求，需配置DMA通道
  */
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/**
  * @brief  配置DMA在最后一次传输后继续请求
  * @param  ADCx 目标ADC外设
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   防止DMA传输完成后因无请求导致的溢出问题
  */
void ADC_DMARequestAfterLastTransferCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/**
  * @brief  配置多ADC模式DMA请求模式
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   仅在多ADC模式下有效，控制DMA请求触发条件
  */
void ADC_MultiModeDMARequestAfterLastTransferCmd(FunctionalState NewState);

/*--------------------- 注入通道配置函数组 ---------------------*/
/**
  * @brief  配置注入通道转换参数
  * @param  ADCx 目标ADC外设
  * @param  ADC_Channel 通道号(0-18)
  * @param  Rank 注入序列位置(1-4)
  * @param  ADC_SampleTime 采样时间(3-480周期)
  * @retval 无
  * @note   配置JSQR寄存器的JSQx和JSQR[JL]位
  */
void ADC_InjectedChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);

/**
  * @brief  设置注入通道序列长度
  * @param  ADCx 目标ADC外设
  * @param  Length 序列长度(1-4)
  * @retval 无
  * @note   对应JSQR寄存器的JL[1:0]位配置
  */
void ADC_InjectedSequencerLengthConfig(ADC_TypeDef* ADCx, uint8_t Length);

/**
  * @brief  设置注入通道数据偏移量
  * @param  ADCx 目标ADC外设
  * @param  ADC_InjectedChannel 注入通道序号(1-4)
  * @param  Offset 12位偏移值(0-4095)
  * @retval 无
  * @note   偏移量会从转换结果中自动减去
  */
void ADC_SetInjectedOffset(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset);

/**
  * @brief  配置注入通道外部触发源
  * @param  ADCx 目标ADC外设
  * @param  ADC_ExternalTrigInjecConv 触发源选择
  * @retval 无
  * @note   触发源需与定时器配置匹配，参考@ref ADC_extrenal_trigger_sources_for_injected_channels_conversion
  */
void ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef* ADCx, uint32_t ADC_ExternalTrigInjecConv);

/**
  * @brief  配置注入通道触发边沿
  * @param  ADCx 目标ADC外设
  * @param  ADC_ExternalTrigInjecConvEdge 触发边沿选择
  * @retval 无
  * @note   可选无触发/上升沿/下降沿/双边沿触发
  */
void ADC_ExternalTrigInjectedConvEdgeConfig(ADC_TypeDef* ADCx, uint32_t ADC_ExternalTrigInjecConvEdge);

/**
  * @brief  软件启动注入通道转换
  * @param  ADCx 目标ADC外设
  * @retval 无
  * @note   立即置位JSWSTART位启动注入转换
  */
void ADC_SoftwareStartInjectedConv(ADC_TypeDef* ADCx);

/**
  * @brief  获取注入转换软件启动状态
  * @param  ADCx 目标ADC外设
  * @retval FlagStatus 启动状态(SET/RESET)
  * @note   检测JSWSTART位是否被清除
  */
FlagStatus ADC_GetSoftwareStartInjectedConvCmdStatus(ADC_TypeDef* ADCx);

/**
  * @brief  使能自动注入转换模式
  * @param  ADCx 目标ADC外设
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   规则通道转换完成后自动启动注入转换
  */
void ADC_AutoInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/**
  * @brief  使能注入通道不连续模式
  * @param  ADCx 目标ADC外设
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   每次触发仅转换注入序列中的一个通道
  */
void ADC_InjectedDiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/**
  * @brief  获取注入通道转换结果
  * @param  ADCx 目标ADC外设
  * @param  ADC_InjectedChannel 注入通道序号(1-4)
  * @retval uint16_t 转换结果（应用偏移量后的值）
  * @note   读取JDRx寄存器，不会清除JEOC标志
  */
uint16_t ADC_GetInjectedConversionValue(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel);

/*------------------- 中断与标志管理函数组 -------------------*/
/**
  * @brief  配置ADC中断使能
  * @param  ADCx 目标ADC外设
  * @param  ADC_IT 中断类型（EOC/AWD/JEOC/OVR组合）
  * @param  NewState 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   需配合NVIC配置中断控制器
  */
void ADC_ITConfig(ADC_TypeDef* ADCx, uint16_t ADC_IT, FunctionalState NewState);

/**
  * @brief  获取ADC状态标志
  * @param  ADCx 目标ADC外设
  * @param  ADC_FLAG 状态标志（AWD/EOC/JEOC/STRT/OVR等）
  * @retval FlagStatus 标志状态(SET/RESET)
  * @note   直接读取SR寄存器对应位
  */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);

/**
  * @brief  清除ADC状态标志
  * @param  ADCx 目标ADC外设
  * @param  ADC_FLAG 待清除标志位
  * @retval 无
  * @note   通过写1到SR寄存器的对应位清除
  */
void ADC_ClearFlag(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);

/**
  * @brief  获取ADC中断状态
  * @param  ADCx 目标ADC外设
  * @param  ADC_IT 中断类型
  * @retval ITStatus 中断状态(SET/RESET)
  * @note   同时检查CR寄存器的中断使能位和SR寄存器的标志位
  */
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, uint16_t ADC_IT);

/**
  * @brief  清除ADC中断挂起标志
  * @param  ADCx 目标ADC外设
  * @param  ADC_IT 待清除中断类型
  * @retval 无
  * @note   清除SR寄存器标志位及中断挂起状态
  */
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, uint16_t ADC_IT);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_ADC_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

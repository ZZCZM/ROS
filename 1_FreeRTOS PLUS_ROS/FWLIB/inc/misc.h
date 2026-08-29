/**
  ******************************************************************************
  * @file    misc.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the miscellaneous
  *          firmware library functions (add-on to CMSIS functions).
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
#ifndef __MISC_H
#define __MISC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup MISC
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/******************************************************************************/
/* STM32 嵌套向量中断控制器(NVIC)配置结构及宏定义
功能：
  - NVIC_InitTypeDef：定义中断通道配置参数结构体，用于设置中断优先级和使能状态
  - 向量表基址宏：配置中断向量表在内存中的存储位置（RAM/FLASH）
  - 低功耗模式宏：配置深度睡眠、立即休眠等低功耗特性

作用：
  通过结构体初始化NVIC实现中断优先级管理，通过宏配置优化系统中断响应与功耗控制
*/

/** 
  * @brief  NVIC 初始化结构体定义  
  * @note   用于配置单个中断通道的优先级参数和使能状态
  */
typedef struct
{
  uint8_t NVIC_IRQChannel;                    /*!< 指定要配置的中断通道编号
                                                   [取值范围] 对应@ref IRQn_Type枚举值（如EXTI0_IRQn等）
                                                   [参考] 完整中断列表参见stm32f4xx.h */

  uint8_t NVIC_IRQChannelPreemptionPriority;  /*!< 设置中断抢占优先级（主优先级）
                                                   [范围0-15] 根据优先级分组设置有效位
                                                   [注意] 数值越小优先级越高 */

  uint8_t NVIC_IRQChannelSubPriority;         /*!< 设置中断子优先级（响应优先级）
                                                   [范围0-15] 在相同主优先级中排序
                                                   [配置] 需配合NVIC_PriorityGroupConfig使用 */

  FunctionalState NVIC_IRQChannelCmd;         /*!< 中断通道使能控制
                                                   [可选值] ENABLE（启用中断）/DISABLE（关闭中断）
                                                   [操作] 通过NVIC_Init()函数生效 */   
} NVIC_InitTypeDef;

/* 导出常量 ----------------------------------------------------------------*/

/** 
  * @defgroup 中断向量表基地址配置
  * @brief    设置中断向量表存储位置
  */
#define NVIC_VectTab_RAM             ((uint32_t)0x20000000)  /*!< 向量表存储在RAM
                                                   [用途] 调试时允许动态修改中断处理函数 */
#define NVIC_VectTab_FLASH           ((uint32_t)0x08000000)  /*!< 向量表存储在FLASH
                                                   [默认] 正常运行时使用 */
#define IS_NVIC_VECTTAB(VECTTAB) (((VECTTAB) == NVIC_VectTab_RAM) || \
                                  ((VECTTAB) == NVIC_VectTab_FLASH)) /*!< 向量表地址验证宏
                                                   [用法] 用于assert_param参数检查 */

/** 
  * @defgroup 系统低功耗配置选项
  * @brief    Cortex-M4低功耗模式配置标志
  */
#define NVIC_LP_SEVONPEND            ((uint8_t)0x10)  /*!< 挂起事件唤醒
                                                   [位4] 1-只唤醒使能的中断，0-唤醒所有挂起中断 */
#define NVIC_LP_SLEEPDEEP            ((uint8_t)0x04)  /*!< 深度睡眠模式
                                                   [位2] 1-进入停止模式，0-普通睡眠 */
#define NVIC_LP_SLEEPONEXIT          ((uint8_t)0x02)  /*!< 退出时立即休眠
                                                   [位1] 1-中断处理完成后自动返回睡眠 */
#define IS_NVIC_LP(LP) (((LP) == NVIC_LP_SEVONPEND) || \
                        ((LP) == NVIC_LP_SLEEPDEEP) || \
                        ((LP) == NVIC_LP_SLEEPONEXIT)) /*!< 低功耗配置验证宏
                                                   [应用] 用于SCB->SCR寄存器设置检查 */

/**
  * @}
  */

/** @defgroup MISC_Preemption_Priority_Group 
  * @{
  */
/******************************************************************************/
/* STM32 中断优先级分组及系统滴答定时器配置宏定义
功能：
  - NVIC优先级分组：定义中断优先级位域分配方案，决定抢占优先级和子优先级的位数比例
  - SysTick时钟源：配置系统定时器的时钟基准（HCLK直接或8分频）
作用：
  通过优先级分组实现多级中断嵌套管理，通过SysTick时钟源选择平衡定时精度与功耗
*/

/** 
  * @defgroup 中断优先级分组配置
  * @brief    定义5种优先级位分配方案，需通过NVIC_PriorityGroupConfig()函数应用
  */
#define NVIC_PriorityGroup_0         ((uint32_t)0x700) /*!< 优先级分组0：
                                                   [抢占优先级] 0位(全部同级)
                                                   [子优先级] 4位(16级) 
                                                   适用场景：无中断嵌套的简单系统 */

#define NVIC_PriorityGroup_1         ((uint32_t)0x600) /*!< 优先级分组1：
                                                   [抢占] 1位(2级) 
                                                   [子优先级] 3位(8级)
                                                   适用场景：基础的两级嵌套系统 */

#define NVIC_PriorityGroup_2         ((uint32_t)0x500) /*!< 优先级分组2：
                                                   [抢占] 2位(4级) 
                                                   [子优先级] 2位(4级)
                                                   典型应用：中等复杂度中断管理系统 */

#define NVIC_PriorityGroup_3         ((uint32_t)0x400) /*!< 优先级分组3：
                                                   [抢占] 3位(8级) 
                                                   [子优先级] 1位(2级)
                                                   适用场景：需要精细抢占控制的应用 */

#define NVIC_PriorityGroup_4         ((uint32_t)0x300) /*!< 优先级分组4：
                                                   [抢占] 4位(16级) 
                                                   [子优先级] 0位(无)
                                                   适用场景：全抢占无子优先级的复杂系统 */

/* 验证宏 ----------------------------------------------------------------*/
#define IS_NVIC_PRIORITY_GROUP(GROUP) (((GROUP) == NVIC_PriorityGroup_0) || \
                                       ((GROUP) == NVIC_PriorityGroup_1) || \
                                       ((GROUP) == NVIC_PriorityGroup_2) || \
                                       ((GROUP) == NVIC_PriorityGroup_3) || \
                                       ((GROUP) == NVIC_PriorityGroup_4)) /*!< 分组有效性检查
                                                   [应用] 用于NVIC_PriorityGroupConfig参数验证 */

#define IS_NVIC_PREEMPTION_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10) /*!< 抢占优先级值校验
                                                   [范围] 必须小于16（4位最大值） */

#define IS_NVIC_SUB_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10) /*!< 子优先级值校验
                                                   [范围] 必须小于16（4位最大值） */

#define IS_NVIC_OFFSET(OFFSET)  ((OFFSET) < 0x000FFFFF) /*!< 中断向量表偏移量校验
                                                   [范围] 最大1MB对齐偏移量 */

/** 
  * @defgroup 系统滴答定时器时钟源
  * @brief    配置SysTick定时器的时钟基准源
  */
#define SysTick_CLKSource_HCLK_Div8    ((uint32_t)0xFFFFFFFB) /*!< 使用HCLK的8分频
                                                   [特点] 低频时钟，适合低功耗场景
                                                   [示例] HCLK=168MHz时，时钟为21MHz */

#define SysTick_CLKSource_HCLK         ((uint32_t)0x00000004) /*!< 直接使用HCLK
                                                   [特点] 全速时钟，定时精度更高
                                                   [注意] 需通过SysTick_CLKSourceConfig配置 */
#define IS_SYSTICK_CLK_SOURCE(SOURCE) (((SOURCE) == SysTick_CLKSource_HCLK) || \
                                       ((SOURCE) == SysTick_CLKSource_HCLK_Div8)) /*!< 时钟源有效性检查
                                                   [应用] 用于SysTick时钟配置函数参数验证 */

/**
  * @}
  */

/**
  * @}
  */
/******************************************************************************/
/* STM32 中断控制器(NVIC)及系统定时器关键功能函数声明
功能：
  - NVIC优先级分组配置、中断通道初始化、向量表重定位
  - 系统低功耗模式控制、SysTick时钟源选择
作用：
  提供中断系统初始化API，支持动态调整中断处理策略及低功耗管理
*/

/* 导出宏 ----------------------------------------------------------------*/
/* 导出函数原型 ----------------------------------------------------------*/

/**
  * @brief  配置NVIC中断优先级分组方式
  * @param  NVIC_PriorityGroup: 优先级分组选择，取值为@ref NVIC_PriorityGroup_0至NVIC_PriorityGroup_4
  * @note   必须在中断初始化(NVIC_Init)前调用，全局生效
  */
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);

/**
  * @brief  初始化指定中断通道参数
  * @param  NVIC_InitStruct: 指向NVIC_InitTypeDef结构的指针，包含：
  *         - IRQChannel: 中断通道编号 @ref IRQn_Type
  *         - PreemptionPriority: 抢占优先级(根据分组确定有效位数)
  *         - SubPriority: 子优先级(根据分组确定有效位数)
  *         - Cmd: 中断使能状态(ENABLE/DISABLE)
  * @note   需先设置优先级分组，每个中断需单独配置
  */
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);

/**
  * @brief  设置中断向量表基址和偏移量
  * @param  NVIC_VectTab: 向量表基址 @ref NVIC_VectTab_RAM/NVIC_VectTab_FLASH
  * @param  Offset: 偏移量(必须对齐到0x200)
  * @note   用于IAP升级或OS环境切换中断处理程序
  */
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);

/**
  * @brief  配置系统低功耗模式特性
  * @param  LowPowerMode: 低功耗模式选择 @ref NVIC_LP_SEVONPEND/NVIC_LP_SLEEPDEEP/NVIC_LP_SLEEPONEXIT
  * @param  NewState: 使能状态(ENABLE/DISABLE)
  * @note   影响Cortex-M4的SCR寄存器，需配合电源管理外设使用
  */
void NVIC_SystemLPConfig(uint8_t LowPowerMode, FunctionalState NewState);

/**
  * @brief  选择SysTick定时器时钟源
  * @param  SysTick_CLKSource: 时钟源选择 @ref SysTick_CLKSource_HCLK/HCLK_Div8
  * @note   直接影响SysTick_Config()生成的定时周期精度
  */
void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource);


#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

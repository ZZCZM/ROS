/**
  ******************************************************************************
  * @file    stm32f4xx_exti.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the EXTI firmware
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
#ifndef __STM32F4xx_EXTI_H
#define __STM32F4xx_EXTI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */
/* 外部中断控制器(EXTI)配置模块 */
/* 本模块提供EXTI中断/事件线路的配置接口，包含模式设置、触发方式设置和初始化结构体定义 */

/* 外部中断模式枚举 ------------------------------------------------------*/
/**
  * @brief  EXTI工作模式类型定义
  * @note   决定EXTI线路是产生中断请求还是事件脉冲
  */
typedef enum
{
  EXTI_Mode_Interrupt = 0x00,  /*!< 中断模式：触发时产生中断请求，需要NVIC配合 */
  EXTI_Mode_Event = 0x04       /*!< 事件模式：触发时产生事件脉冲，用于唤醒内核或触发外设 */
}EXTIMode_TypeDef;

/* 模式有效性验证宏 */
#define IS_EXTI_MODE(MODE) (((MODE) == EXTI_Mode_Interrupt) || ((MODE) == EXTI_Mode_Event))

/* 外部中断触发方式枚举 --------------------------------------------------*/
/**
  * @brief  EXTI触发边沿类型定义
  * @note   决定在什么信号边沿触发中断/事件
  */
typedef enum
{
  EXTI_Trigger_Rising = 0x08,        /*!< 上升沿触发：信号从低到高跳变时触发 */
  EXTI_Trigger_Falling = 0x0C,       /*!< 下降沿触发：信号从高到低跳变时触发 */
  EXTI_Trigger_Rising_Falling = 0x10 /*!< 双边沿触发：上升沿和下降沿都会触发 */
}EXTITrigger_TypeDef;

/* 触发方式有效性验证宏 */
#define IS_EXTI_TRIGGER(TRIGGER) (((TRIGGER) == EXTI_Trigger_Rising) || \
                                  ((TRIGGER) == EXTI_Trigger_Falling) || \
                                  ((TRIGGER) == EXTI_Trigger_Rising_Falling))

/* 外部中断初始化结构体 --------------------------------------------------*/
/**
  * @brief  EXTI配置结构体
  * @note   用于完整配置一个EXTI线路的工作参数
  */
typedef struct
{
  uint32_t EXTI_Line;       /*!< 选择要配置的EXTI线路 
                             * 取值：EXTI_Line0~EXTI_Line22的按位或组合 
                             * @see EXTI_Lines 定义 */

  EXTIMode_TypeDef EXTI_Mode; /*!< 设置工作模式：中断或事件 
                             * @see EXTIMode_TypeDef */

  EXTITrigger_TypeDef EXTI_Trigger; /*!< 设置触发方式：上升沿/下降沿/双边沿 
                                     * @see EXTITrigger_TypeDef */

  FunctionalState EXTI_LineCmd; /*!< 使能或禁用该EXTI线路 
                                 * 取值：ENABLE(激活配置)/DISABLE(关闭线路) */
}EXTI_InitTypeDef;
/* EXTI中断线路映射定义 -------------------------------------------------*/
/**
  * @brief 外部中断线路编号宏定义
  * @note 每个线路对应特定的硬件中断源，线路0-15可映射到GPIO引脚，16-22为专用系统事件
  *        使用前必须通过SYSCFG_EXTILineConfig()配置引脚映射关系（适用于线路0-15）
  *        线路值采用位掩码格式，支持多线路组合配置
  */

#define EXTI_Line0       ((uint32_t)0x00001)     /* PA0/PB0/PC0等GPIO的Pin0中断 */
#define EXTI_Line1       ((uint32_t)0x00002)     /* PA1/PB1/PC1等GPIO的Pin1中断 */
#define EXTI_Line2       ((uint32_t)0x00004)     /* PA2/PB2/PC2等GPIO的Pin2中断 */
#define EXTI_Line3       ((uint32_t)0x00008)     /* PA3/PB3/PC3等GPIO的Pin3中断 */
#define EXTI_Line4       ((uint32_t)0x00010)     /* PA4/PB4/PC4等GPIO的Pin4中断 */
#define EXTI_Line5       ((uint32_t)0x00020)     /* PA5/PB5/PC5等GPIO的Pin5中断 */
#define EXTI_Line6       ((uint32_t)0x00040)     /* PA6/PB6/PC6等GPIO的Pin6中断 */
#define EXTI_Line7       ((uint32_t)0x00080)     /* PA7/PB7/PC7等GPIO的Pin7中断 */
#define EXTI_Line8       ((uint32_t)0x00100)     /* PA8/PB8/PC8等GPIO的Pin8中断 */
#define EXTI_Line9       ((uint32_t)0x00200)     /* PA9/PB9/PC9等GPIO的Pin9中断 */
#define EXTI_Line10      ((uint32_t)0x00400)     /* PA10/PB10/PC10等GPIO的Pin10中断 */
#define EXTI_Line11      ((uint32_t)0x00800)     /* PA11/PB11/PC11等GPIO的Pin11中断 */
#define EXTI_Line12      ((uint32_t)0x01000)     /* PA12/PB12/PC12等GPIO的Pin12中断 */
#define EXTI_Line13      ((uint32_t)0x02000)     /* PA13/PB13/PC13等GPIO的Pin13中断 */
#define EXTI_Line14      ((uint32_t)0x04000)     /* PA14/PB14/PC14等GPIO的Pin14中断 */
#define EXTI_Line15      ((uint32_t)0x08000)     /* PA15/PB15/PC15等GPIO的Pin15中断 */

/* 系统专用事件线路 -----------------------------------------------------*/
#define EXTI_Line16      ((uint32_t)0x10000)     /* 可编程电压检测器(PVD)输出中断 */
#define EXTI_Line17      ((uint32_t)0x20000)     /* RTC闹钟事件中断（实时时钟报警） */
#define EXTI_Line18      ((uint32_t)0x40000)     /* USB OTG FS唤醒中断（从挂起模式唤醒） */
#define EXTI_Line19      ((uint32_t)0x80000)     /* 以太网唤醒事件中断（用于网络唤醒功能） */
#define EXTI_Line20      ((uint32_t)0x00100000)  /* USB OTG HS唤醒中断（高速USB唤醒） */
#define EXTI_Line21      ((uint32_t)0x00200000)  /* RTC侵入检测和时间戳事件中断 */
#define EXTI_Line22      ((uint32_t)0x00400000)  /* RTC唤醒定时器事件中断 */
    /* EXTI模块功能验证宏 ---------------------------------------------------*/
/**
  * @brief 线路有效性组合验证宏
  * @param LINE 要验证的EXTI线路值
  * @note 通过位掩码验证线路值的合法性：
  *       1. 高位24-31位必须为0（保证在32位地址空间的有效性）
  *       2. 线路值不能为0（至少选择一条有效线路）
  * @warning 该宏允许线路组合验证（如 Line0 | Line1）
  */
#define IS_EXTI_LINE(LINE) ((((LINE) & (uint32_t)0xFF800000) == 0x00) && ((LINE) != (uint16_t)0x00))

/**
  * @brief 单线路有效性验证宏
  * @param LINE 要验证的EXTI线路值
  * @note 严格验证是否为预定义的22条独立线路之一
  *       用于需要精确指定单条线路的场景（如清除中断标志）
  */

#define IS_GET_EXTI_LINE(LINE) (((LINE) == EXTI_Line0) || ((LINE) == EXTI_Line1) || \
                                ((LINE) == EXTI_Line2) || ((LINE) == EXTI_Line3) || \
                                ((LINE) == EXTI_Line4) || ((LINE) == EXTI_Line5) || \
                                ((LINE) == EXTI_Line6) || ((LINE) == EXTI_Line7) || \
                                ((LINE) == EXTI_Line8) || ((LINE) == EXTI_Line9) || \
                                ((LINE) == EXTI_Line10) || ((LINE) == EXTI_Line11) || \
                                ((LINE) == EXTI_Line12) || ((LINE) == EXTI_Line13) || \
                                ((LINE) == EXTI_Line14) || ((LINE) == EXTI_Line15) || \
                                ((LINE) == EXTI_Line16) || ((LINE) == EXTI_Line17) || \
                                ((LINE) == EXTI_Line18) || ((LINE) == EXTI_Line19) || \
                                ((LINE) == EXTI_Line20) || ((LINE) == EXTI_Line21) ||\
                                ((LINE) == EXTI_Line22))

/* EXTI模块功能函数声明 -------------------------------------------------*/
/**
  * @brief 复位EXTI寄存器到默认值
  * @note 会清除所有EXTI配置，包括：
  *       - 中断/事件模式设置
  *       - 触发边沿设置
  *       - 线路使能状态
  */
void EXTI_DeInit(void);

/**
  * @brief EXTI初始化函数
  * @param EXTI_InitStruct 指向配置结构体的指针
  * @note 使用流程：
  *       1. 定义EXTI_InitTypeDef结构体
  *       2. 调用EXTI_StructInit初始化默认值
  *       3. 修改需要配置的成员
  *       4. 调用本函数应用配置
  */
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);

/**
  * @brief 初始化结构体默认值
  * @param EXTI_InitStruct 要初始化的结构体指针
  * @note 默认配置：
  *       - LineCmd = DISABLE
  *       - Mode = EXTI_Mode_Interrupt
  *       - Trigger = EXTI_Trigger_Falling
  */
void EXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct);

/**
  * @brief 软件触发中断
  * @param EXTI_Line 要触发的中断线路
  * @note 可用于测试中断服务程序，无需实际物理信号
  * @warning 需要对应的中断线路已配置为中断模式
  */
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line);

/* 中断状态管理函数组 ---------------------------------------------------*/
/**
  * @brief 获取标志位状态（无论中断是否使能）
  * @param EXTI_Line 要查询的线路
  * @retval SET/RESET
  * @note 适用于事件模式和中断模式的标志查询
  */
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line);

/**
  * @brief 清除事件/中断标志位
  * @param EXTI_Line 要清除的线路
  * @note 对以下寄存器操作：
  *       - 清除EXTI_PR挂起寄存器对应位
  */
void EXTI_ClearFlag(uint32_t EXTI_Line);

/**
  * @brief 获取中断状态（需中断已使能）
  * @param EXTI_Line 要查询的线路
  * @retval SET/RESET
  * @note 仅在同时满足以下条件时返回SET：
  *       1. 中断标志位已置起
  *       2. 对应中断已在NVIC使能
  */
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);

/**
  * @brief 清除中断挂起位
  * @param EXTI_Line 要清除的线路
  * @note 必须在中端服务程序(ISR)中调用
  *       清除顺序建议：
  *       1. EXTI_GetITStatus() 检查中断源
  *       2. 执行中断处理
  *       3. EXTI_ClearITPendingBit() 清除标志
  */
void EXTI_ClearITPendingBit(uint32_t EXTI_Line);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_EXTI_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

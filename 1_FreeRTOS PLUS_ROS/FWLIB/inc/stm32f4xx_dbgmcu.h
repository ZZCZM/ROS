/**
  ******************************************************************************
  * @file    stm32f4xx_dbgmcu.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the DBGMCU firmware library.
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
#ifndef __STM32F4xx_DBGMCU_H
#define __STM32F4xx_DBGMCU_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup DBGMCU
  * @{
  */ 
/*
 * 模块名称：STM32F4xx调试单元(DBGMCU)配置
 * 功能描述：本头文件定义调试模式下MCU行为控制参数，主要功能包括：
 *          - 低功耗模式下的调试接口保持
 *          - 调试期间外设时钟冻结控制
 * 核心作用：在芯片调试过程中维持调试器连接，并控制外设状态便于问题诊断
 * 特殊说明：这些配置通常通过DBGMCU_APBxPeriphConfig()函数应用
 */

/* 导出常量定义 ------------------------------------------------------------*/

/** @defgroup DBGMCU_Exported_Constants 调试单元配置参数
  * @{
  */

/* 低功耗调试模式配置 ------------------------------------------------------*/
#define DBGMCU_SLEEP                 ((uint32_t)0x00000001)  /* 睡眠模式下保持调试接口活动 */
#define DBGMCU_STOP                  ((uint32_t)0x00000002)  /* 停止模式下保持调试功能 */
#define DBGMCU_STANDBY               ((uint32_t)0x00000004)  /* 待机模式下保持调试功能 */
/* 低功耗模式参数校验宏（允许值：0x01/0x02/0x04及其合法组合） */
#define IS_DBGMCU_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFFF8) == 0x00) && ((PERIPH) != 0x00))

/* APB1总线外设冻结控制 ---------------------------------------------------*/
#define DBGMCU_TIM2_STOP             ((uint32_t)0x00000001)  /* 调试时冻结TIM2计数器 */
#define DBGMCU_TIM3_STOP             ((uint32_t)0x00000002)  /* 冻结TIM3 */
#define DBGMCU_TIM4_STOP             ((uint32_t)0x00000004)  /* 冻结TIM4 */
#define DBGMCU_TIM5_STOP             ((uint32_t)0x00000008)  /* 冻结TIM5 */
#define DBGMCU_TIM6_STOP             ((uint32_t)0x00000010)  /* 冻结TIM6 */
#define DBGMCU_TIM7_STOP             ((uint32_t)0x00000020)  /* 冻结TIM7 */
#define DBGMCU_TIM12_STOP            ((uint32_t)0x00000040)  /* 冻结TIM12 */
#define DBGMCU_TIM13_STOP            ((uint32_t)0x00000080)  /* 冻结TIM13 */
#define DBGMCU_TIM14_STOP            ((uint32_t)0x00000100)  /* 冻结TIM14 */
#define DBGMCU_RTC_STOP              ((uint32_t)0x00000400)  /* 冻结RTC时钟 */
#define DBGMCU_WWDG_STOP             ((uint32_t)0x00000800)  /* 冻结窗口看门狗 */
#define DBGMCU_IWDG_STOP             ((uint32_t)0x00001000)  /* 冻结独立看门狗 */
#define DBGMCU_I2C1_SMBUS_TIMEOUT    ((uint32_t)0x00200000)  /* 启用I2C1 SMBUS超时检测 */
#define DBGMCU_I2C2_SMBUS_TIMEOUT    ((uint32_t)0x00400000)  /* 启用I2C2超时检测 */
#define DBGMCU_I2C3_SMBUS_TIMEOUT    ((uint32_t)0x00800000)  /* 启用I2C3超时检测 */
#define DBGMCU_CAN1_STOP             ((uint32_t)0x02000000)  /* 冻结CAN1总线 */
#define DBGMCU_CAN2_STOP             ((uint32_t)0x04000000)  /* 冻结CAN2总线 */
/* APB1外设参数校验宏（位掩码0xF91FE200过滤非法值） */
#define IS_DBGMCU_APB1PERIPH(PERIPH) ((((PERIPH) & 0xF91FE200) == 0x00) && ((PERIPH) != 0x00))

/* APB2总线外设冻结控制 ---------------------------------------------------*/
#define DBGMCU_TIM1_STOP             ((uint32_t)0x00000001)  /* 冻结TIM1 */
#define DBGMCU_TIM8_STOP             ((uint32_t)0x00000002)  /* 冻结TIM8 */
#define DBGMCU_TIM9_STOP             ((uint32_t)0x00010000)  /* 冻结TIM9 */
#define DBGMCU_TIM10_STOP            ((uint32_t)0x00020000)  /* 冻结TIM10 */
#define DBGMCU_TIM11_STOP            ((uint32_t)0x00040000)  /* 冻结TIM11 */
/* APB2外设参数校验宏（位掩码0xFFF8FFFC过滤非法值） */
#define IS_DBGMCU_APB2PERIPH(PERIPH) ((((PERIPH) & 0xFFF8FFFC) == 0x00) && ((PERIPH) != 0x00))
/*
 * 模块名称：STM32F4xx调试单元(DBGMCU)驱动接口
 * 功能描述：本部分提供调试控制单元的操作接口，主要功能包括：
 *          - 芯片版本信息获取
 *          - 低功耗调试模式配置
 *          - 外设调试冻结控制
 * 应用场景：用于调试过程中维持MCU状态，防止外设干扰调试过程
 */

/* 导出函数声明 ------------------------------------------------------------*/

/**
 * @brief 获取芯片修订版本标识符
 * @return uint32_t 修订版本号（参考芯片手册RevID字段）
 * @note 返回值示例：0x1001表示Rev 1.1
 */
uint32_t DBGMCU_GetREVID(void);

/**
 * @brief 获取芯片设备标识符
 * @return uint32_t 设备ID（参考芯片手册DevID字段）
 * @note STM32F4系列典型值：0x413（适用于F405/415等）
 */
uint32_t DBGMCU_GetDEVID(void);

/**
 * @brief 配置低功耗调试模式
 * @param DBGMCU_Periph 模式选择：DBGMCU_SLEEP/STOP/STANDBY的组合
 * @param NewState ENABLE（启用调试保持）/DISABLE
 * @example 使能STOP模式调试：DBGMCU_Config(DBGMCU_STOP, ENABLE)
 */
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState);

/**
 * @brief 配置APB1总线外设调试冻结
 * @param DBGMCU_Periph 外设选择：DBGMCU_TIMx_STOP/CANx_STOP等APB1设备
 * @param NewState ENABLE（冻结外设）/DISABLE
 * @warning 需与IS_DBGMCU_APB1PERIPH宏配合校验参数
 */
void DBGMCU_APB1PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState);

/**
 * @brief 配置APB2总线外设调试冻结
 * @param DBGMCU_Periph 外设选择：DBGMCU_TIM1_STOP等APB2设备
 * @note 支持TIM1/TIM8/TIM9-11等高速外设
 */
void DBGMCU_APB2PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_DBGMCU_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

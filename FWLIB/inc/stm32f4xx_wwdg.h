/**
  ******************************************************************************
  * @file    stm32f4xx_wwdg.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the WWDG firmware
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
#ifndef __STM32F4xx_WWDG_H
#define __STM32F4xx_WWDG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup WWDG
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup WWDG_Exported_Constants
  * @{
  */ 
  
/** @defgroup WWDG_Prescaler 
  * @{
  */
  /** @defgroup WWDG_Prescaler 看门狗预分频器配置
  * @brief 窗口看门狗（WWDG）时钟预分频系数设置
  * @{
  */
  
/* 预分频器配置宏 -----------------------------------------------------------*/
#define WWDG_Prescaler_1    ((uint32_t)0x00000000)  /*!< 时钟不分频，PCLK直接作为WWDG时钟 */
#define WWDG_Prescaler_2    ((uint32_t)0x00000080)  /*!< 2分频模式，WWDG时钟 = PCLK/2 */
#define WWDG_Prescaler_4    ((uint32_t)0x00000100)  /*!< 4分频模式，WWDG时钟 = PCLK/4 */
#define WWDG_Prescaler_8    ((uint32_t)0x00000180)  /*!< 8分频模式，WWDG时钟 = PCLK/8 */

/* 参数校验宏 --------------------------------------------------------------*/
/**
  * @brief 验证预分频系数是否合法
  * @param PRESCALER 待验证的分频系数
  * @retval 校验结果：TRUE(有效值) 或 FALSE(无效值)
  */
#define IS_WWDG_PRESCALER(PRESCALER) (((PRESCALER) == WWDG_Prescaler_1) || \
                                      ((PRESCALER) == WWDG_Prescaler_2) || \
                                      ((PRESCALER) == WWDG_Prescaler_4) || \
                                      ((PRESCALER) == WWDG_Prescaler_8))

/**
  * @brief 验证窗口值是否合法
  * @param VALUE 待验证的窗口值（0x00-0x7F）
  * @retval 校验结果：TRUE(有效值) 或 FALSE(无效值)
  */
#define IS_WWDG_WINDOW_VALUE(VALUE) ((VALUE) <= 0x7F)

/**
  * @brief 验证计数器值是否合法
  * @param COUNTER 待验证的计数器值（0x40-0x7F）
  * @retval 校验结果：TRUE(有效值) 或 FALSE(无效值)
  */
#define IS_WWDG_COUNTER(COUNTER) (((COUNTER) >= 0x40) && ((COUNTER) <= 0x7F))

/** @} */  /* 结束WWDG_Prescaler分组 */

/** @} */  /* 结束上级分组 */

/* 导出宏 ----------------------------------------------------------------*/
/* 导出函数原型 ----------------------------------------------------------*/

/**
  * @brief 复位WWDG配置到默认状态
  * @note 将WWDG控制寄存器和状态寄存器恢复为复位值
  */
void WWDG_DeInit(void);

/* 预分频器、窗口值和计数器配置函数组 --------------------------------------*/
/**
  * @brief 设置预分频系数
  * @param WWDG_Prescaler 分频系数，必须使用WWDG_Prescaler_x宏定义值
  */
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler);

/**
  * @brief 设置窗口阈值
  * @param WindowValue 窗口比较值（0x00-0x7F）
  * @note 当计数器值低于此值时允许刷新
  */
void WWDG_SetWindowValue(uint8_t WindowValue);

/**
  * @brief 使能提前唤醒中断
  * @note 当计数器值达到0x40时触发中断
  */
void WWDG_EnableIT(void);

/**
  * @brief 设置计数器初始值
  * @param Counter 初始计数值（0x40-0x7F）
  * @note 必须在允许刷新窗口内调用
  */
void WWDG_SetCounter(uint8_t Counter);

/* WWDG激活函数 ---------------------------------------------------------*/
/**
  * @brief 启动窗口看门狗
  * @param Counter 初始计数值（0x40-0x7F）
  * @note 同时设置计数器初始值和使能WWDG
  */
void WWDG_Enable(uint8_t Counter);

/* 中断和标志管理函数组 -------------------------------------------------*/
/**
  * @brief 获取提前唤醒中断标志状态
  * @retval SET/RESET 中断标志状态
  */
FlagStatus WWDG_GetFlagStatus(void);

/**
  * @brief 清除提前唤醒中断标志
  * @note 必须在中断服务程序中调用以清除中断标志
  */
void WWDG_ClearFlag(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_WWDG_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
  ******************************************************************************
  * @file    stm32f4xx_crc.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the CRC firmware 
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
#ifndef __STM32F4xx_CRC_H
#define __STM32F4xx_CRC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup CRC
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup CRC_Exported_Constants
  * @{
  */

/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief 重置CRC数据寄存器
  * @note 此函数将CRC数据寄存器（DR）重置为默认值
  */
void CRC_ResetDR(void);

/**
  * @brief 计算单个数据的CRC
  * @param Data 需要计算CRC的数据（32位无符号整数）
  * @return 计算得到的CRC值
  * @note 此函数用于计算单个32位数据的CRC
  */
uint32_t CRC_CalcCRC(uint32_t Data);

/**
  * @brief 计算数据块的CRC
  * @param pBuffer 指向数据块的指针
  * @param BufferLength 数据块长度（32位无符号整数）
  * @return 计算得到的CRC值
  * @note 此函数用于计算多个32位数据的CRC值
  */
uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength);

/**
  * @brief 获取当前的CRC值
  * @return 当前的CRC数据寄存器（DR）值
  * @note 此函数用于获取当前计算的CRC值
  */
uint32_t CRC_GetCRC(void);

/**
  * @brief 设置CRC独立数据寄存器（IDR）
  * @param IDValue 独立数据寄存器的值（8位无符号整数）
  * @note 此函数用于设置CRC独立数据寄存器（IDR），通常用于特定应用
  */
void CRC_SetIDRegister(uint8_t IDValue);

/**
  * @brief 获取CRC独立数据寄存器（IDR）的值
  * @return 独立数据寄存器（IDR）的值
  * @note 此函数用于获取CRC独立数据寄存器（IDR）的当前值
  */
uint8_t CRC_GetIDRegister(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_CRC_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

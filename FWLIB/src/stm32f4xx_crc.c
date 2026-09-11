/**
  ******************************************************************************
  * @file    stm32f4xx_crc.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides all the CRC firmware functions.
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
#include "stm32f4xx_crc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup CRC 
  * @brief CRC driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CRC_Private_Functions
  * @{
  */
/* CRC 控制寄存器操作 */
/**
  * @brief  复位CRC数据寄存器(DR)并重新初始化CRC计算器
  * @param  无
  * @retval 无
  * @note   1. 通过写CR寄存器的复位位(CRC_CR_RESET)实现硬件级复位
  *         2. 该操作会清除DR寄存器的所有数据
  *         3. 复位后CRC多项式恢复默认值(STM32F4xx的CRC-32)
  */
void CRC_ResetDR(void)
{
  /* 触发硬件CRC生成器复位 */
  CRC->CR = CRC_CR_RESET;  // 写1到CR寄存器第0位（复位位）
}

/* 单字CRC计算 */
/**
  * @brief  计算单个32位数据的CRC值
  * @param  Data: 待计算的数据字（32位整型）
  * @retval uint32_t 计算得到的32位CRC值
  * @note   1. 写入DR寄存器会自动触发硬件CRC计算
  *         2. 每次计算使用前应当先调用CRC_ResetDR初始化
  *         3. 数据按小端模式处理（取决于CR寄存器的REV_IN设置）
  */
uint32_t CRC_CalcCRC(uint32_t Data)
{
  CRC->DR = Data;          // 写入数据触发自动计算
  
  return (CRC->DR);        // 返回计算完成的CRC值
}

/* 数据块CRC计算 */
/**
  * @brief  计算32位数据块的累积CRC值
  * @param  pBuffer: 数据缓冲区首地址（32位对齐数组）
  * @param  BufferLength: 数据长度（以32位字为单位）
  * @retval uint32_t 最终计算的32位CRC值
  * @note   1. 采用流式计算模式，连续写入多个数据字
  *         2. 数据按写入顺序进行累积计算
  *         3. 建议使用DMA传输大数据块以提高效率
  */
uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength)
{
  uint32_t index = 0;
  
  /* 遍历整个数据缓冲区 */
  for(index = 0; index < BufferLength; index++)
  {
    CRC->DR = pBuffer[index];  // 依次写入每个数据字
  }
  return (CRC->DR);            // 返回最终累积结果
}

/* CRC状态获取 */
/**
  * @brief  获取当前CRC数据寄存器的值
  * @param  无
  * @retval uint32_t 当前DR寄存器的32位数值
  * @note   1. 直接读取DR寄存器不会影响计算状态
  *         2. 可用于获取中间计算结果
  */
uint32_t CRC_GetCRC(void)
{
  return (CRC->DR);  // 简单返回寄存器当前值
}

/* 独立数据寄存器操作 */
/**
  * @brief  设置独立数据寄存器(IDR)的值
  * @param  IDValue: 8位配置值（0x00-0xFF）
  * @retval 无
  * @note   1. IDR寄存器不参与CRC计算
  *         2. 可用于存储用户自定义配置参数
  *         3. 保持最后写入值直到下次修改
  */
void CRC_SetIDRegister(uint8_t IDValue)
{
  CRC->IDR = IDValue;  // 写入8位数据到IDR低8位
}

/**
  * @brief  读取独立数据寄存器(IDR)的当前值
  * @param  无
  * @retval uint8_t IDR寄存器的8位数值
  * @note   1. 读取自动屏蔽高24位
  *         2. IDR寄存器内容在CRC复位时保持不变
  */
uint8_t CRC_GetIDRegister(void)
{
  return (CRC->IDR);  // 返回IDR寄存器的低8位
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

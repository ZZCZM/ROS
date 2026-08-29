/**
  ******************************************************************************
  * @file    stm32f4xx_hash_sha1.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides high level functions to compute the HASH SHA1 and
  *          HMAC SHA1 Digest of an input message.
  *          It uses the stm32f4xx_hash.c/.h drivers to access the STM32F4xx HASH
  *          peripheral.
  *
@verbatim
 ===================================================================
                 ##### How to use this driver #####
 ===================================================================
 [..]
   (#) Enable The HASH controller clock using 
       RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, ENABLE); function.
  
   (#) Calculate the HASH SHA1 Digest using HASH_SHA1() function.
  
   (#) Calculate the HMAC SHA1 Digest using HMAC_SHA1() function.
  
@endverbatim
  *
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
#include "stm32f4xx_hash.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup HASH 
  * @brief HASH driver modules
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/**
  * @brief SHA1硬件模块忙状态检测超时阈值
  * @note  该宏定义用于HASH模块进行SHA1运算时的忙状态轮询超时保护
  *        应用场景：
  *        - 当HASH处理器执行SHA1运算时，轮询HASH_SR寄存器的BUSY位
  *        - 防止因硬件故障导致程序进入死循环
  *        计算依据：
  *        - 基于168MHz系统时钟，约对应65536次循环等待（约0.39ms）
  *        - 实际超时时间需根据CPU时钟频率调整
  */
#define SHA1BUSY_TIMEOUT    ((uint32_t) 0x00010000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HASH_Private_Functions
  * @{
  */ 

/** @defgroup HASH_Group6 High Level SHA1 functions
 *  @brief   High Level SHA1 Hash and HMAC functions 
 *
@verbatim   
 ===============================================================================
               ##### High Level SHA1 Hash and HMAC functions #####
 ===============================================================================


@endverbatim
  * @{
  */
/**
  * @brief  SHA1哈希计算函数
  * @param  Input: 输入数据缓冲区指针（8位对齐）
  * @param  Ilen: 输入数据字节长度
  * @param  Output: 输出摘要缓冲区（需至少20字节空间）
  * @retval ErrorStatus: 执行状态
  *         - SUCCESS: 计算成功
  *         - ERROR: 计算超时或失败
  * @note   函数流程说明：
  *         - 初始化HASH模块为SHA1模式
  *         - 处理输入数据对齐（最后4字节有效位数计算）
  *         - 将输入数据写入硬件FIFO
  *         - 启动硬件哈希计算并等待完成
  *         - 获取结果并转换字节序输出
  *         注意事项：
  *         - 大端序处理：使用__REV宏转换字节顺序
  *         - 输入数据长度必须小于2^64位（SHA1规范）
  */
ErrorStatus HASH_SHA1(uint8_t *Input, uint32_t Ilen, uint8_t Output[20])
{
  HASH_InitTypeDef SHA1_HASH_InitStructure;   // HASH配置结构体
  HASH_MsgDigest SHA1_MessageDigest;         // 摘要存储结构体
  __IO uint16_t nbvalidbitsdata = 0;         // 最后一个字的有效位数
  uint32_t i = 0;                            // 循环计数器
  __IO uint32_t counter = 0;                 // 超时计数器
  uint32_t busystatus = 0;                    // 忙状态标志
  ErrorStatus status = SUCCESS;               // 函数执行状态
  uint32_t inputaddr  = (uint32_t)Input;       // 输入缓冲区物理地址
  uint32_t outputaddr = (uint32_t)Output;      // 输出缓冲区物理地址

  /* 计算最后一个4字节块的有效位数（用于非对齐数据）*/
  // 示例：Ilen=5字节时，5%4=1 → 8*1=8bits有效
  nbvalidbitsdata = 8 * (Ilen % 4);           

  /* 复位HASH模块到默认状态（清除所有寄存器）*/
  HASH_DeInit();                              

  /* 配置HASH工作参数 */
  SHA1_HASH_InitStructure.HASH_AlgoSelection = HASH_AlgoSelection_SHA1;  // 算法选择SHA1
  SHA1_HASH_InitStructure.HASH_AlgoMode = HASH_AlgoMode_HASH;           // 标准HASH模式
  SHA1_HASH_InitStructure.HASH_DataType = HASH_DataType_8b;              // 按字节处理数据
  HASH_Init(&SHA1_HASH_InitStructure);        // 写入配置寄存器

  /* 设置最后一个数据字的有效位数 */
  HASH_SetLastWordValidBitsNbr(nbvalidbitsdata); // 配置HASH_STR寄存器

  /* 将输入数据按4字节块写入HASH FIFO */
  for(i=0; i<Ilen; i+=4)                       
  {
    /* 强制类型转换后写入数据寄存器（注意地址对齐问题） */
    HASH_DataIn(*(uint32_t*)inputaddr);        // 写入HASH_DIN寄存器
    inputaddr+=4;                              // 移动输入指针（4字节步进）
  }

  /* 启动硬件哈希计算（触发HASH_STR寄存器START位） */
  HASH_StartDigest();                          

  /* 轮询BUSY标志直到计算完成或超时 */
  do
  {
    busystatus = HASH_GetFlagStatus(HASH_FLAG_BUSY); // 读取SR寄存器BUSY位
    counter++;                                       // 累计等待周期
  }while ((counter != SHA1BUSY_TIMEOUT) && (busystatus != RESET));

  /* 超时错误处理 */
  if (busystatus != RESET)
  {
     status = ERROR;  // 设置错误状态（HASH模块长时间无响应）
  }
  else
  {
    /* 从硬件寄存器获取160位摘要（5个32位字） */
    HASH_GetDigest(&SHA1_MessageDigest);       // 读取HASH_HR0-4寄存器
    
    /* 处理大端序转换并写入输出缓冲区 */
    // 使用__REV宏反转32位数据的字节序
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[0]); // 写第0-3字节
    outputaddr+=4;                                                 
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[1]); // 写第4-7字节
    outputaddr+=4;
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[2]); // 写第8-11字节
    outputaddr+=4;
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[3]); // 写第12-15字节
    outputaddr+=4;
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[4]); // 写第16-19字节
  }
  return status;  // 返回最终执行状态
}

/**
  * @brief  HMAC-SHA1认证码生成函数
  * @param  Key: HMAC密钥指针（8位对齐）
  * @param  Keylen: 密钥字节长度
  * @param  Input: 输入数据缓冲区指针
  * @param  Ilen: 输入数据字节长度
  * @param  Output: 输出缓冲区（需至少20字节空间）
  * @retval ErrorStatus: 执行状态（SUCCESS/ERROR）
  * @note   基于RFC 2104标准的HMAC实现：
  *         - 密钥长度超过64字节将被哈希缩短
  *         - 包含三次硬件哈希运算：H(key^ipad), H(key^opad || H(data))
  *         硬件工作流程：
  *         - 第1阶段：处理密钥生成中间值
  *         - 第2阶段：处理输入数据
  *         - 第3阶段：处理带密钥的最终哈希
  */
ErrorStatus HMAC_SHA1(uint8_t *Key, uint32_t Keylen, uint8_t *Input,
                      uint32_t Ilen, uint8_t Output[20])
{
  HASH_InitTypeDef SHA1_HASH_InitStructure;    // HASH配置结构体
  HASH_MsgDigest SHA1_MessageDigest;           // 摘要存储结构体
  __IO uint16_t nbvalidbitsdata = 0;           // 数据末尾有效位数
  __IO uint16_t nbvalidbitskey = 0;            // 密钥末尾有效位数
  uint32_t i = 0;                              // 循环计数器
  __IO uint32_t counter = 0;                   // 超时计数器
  uint32_t busystatus = 0;                      // 忙状态标志
  ErrorStatus status = SUCCESS;                 // 执行状态
  uint32_t keyaddr    = (uint32_t)Key;          // 密钥物理地址
  uint32_t inputaddr  = (uint32_t)Input;         // 输入数据物理地址
  uint32_t outputaddr = (uint32_t)Output;       // 输出缓冲物理地址

  /* 计算输入数据最后一个4字节块的有效位数 */
  nbvalidbitsdata = 8 * (Ilen % 4);            // 例如：15字节数据 → 3字节有效

  /* 计算密钥最后一个4字节块的有效位数 */
  nbvalidbitskey = 8 * (Keylen % 4);            // 处理非4倍数密钥长度

  /* 复位HASH控制器到初始状态 */
  HASH_DeInit();                                // 清空所有配置寄存器

  /* 配置HASH为HMAC-SHA1模式 */
  SHA1_HASH_InitStructure.HASH_AlgoSelection = HASH_AlgoSelection_SHA1;  // SHA1算法
  SHA1_HASH_InitStructure.HASH_AlgoMode = HASH_AlgoMode_HMAC;           // HMAC模式
  SHA1_HASH_InitStructure.HASH_DataType = HASH_DataType_8b;              // 字节数据模式
  /* 根据密钥长度选择密钥类型 */
  if(Keylen > 64) {
    /* 长密钥模式：硬件自动进行初始哈希 */
    SHA1_HASH_InitStructure.HASH_HMACKeyType = HASH_HMACKeyType_LongKey;  // 密钥长度>64字节
  } else {
    /* 短密钥模式：直接使用原密钥 */
    SHA1_HASH_InitStructure.HASH_HMACKeyType = HASH_HMACKeyType_ShortKey; 
  }
  HASH_Init(&SHA1_HASH_InitStructure);          // 应用配置到寄存器

  /* 设置密钥末尾有效位数 */
  HASH_SetLastWordValidBitsNbr(nbvalidbitskey);  // 配置HASH_STR寄存器

  /* 写入密钥到HASH FIFO（第一阶段运算） */
  for(i=0; i<Keylen; i+=4) {                     // 按4字节块处理
    HASH_DataIn(*(uint32_t*)keyaddr);            // 写入密钥块
    keyaddr+=4;                                  // 地址指针递增
  }

  /* 启动第一阶段HMAC运算（H(K^ipad)处理） */
  HASH_StartDigest();                            // 触发运算开始

  /* 等待第一阶段完成或超时 */
  do {
    busystatus = HASH_GetFlagStatus(HASH_FLAG_BUSY); // 监控BUSY标志
    counter++;
  } while ((counter != SHA1BUSY_TIMEOUT) && (busystatus != RESET));

  if (busystatus != RESET) {
     status = ERROR;                             // 硬件未响应，返回错误
  } else {
    /* 配置输入数据有效位数 */
    HASH_SetLastWordValidBitsNbr(nbvalidbitsdata); // 准备数据阶段
    
    /* 写入输入数据到FIFO（第二阶段运算：H(K^ipad || data)） */
    for(i=0; i<Ilen; i+=4) {
      HASH_DataIn(*(uint32_t*)inputaddr);        // 写入数据块
      inputaddr+=4;                             // 地址递增
    }

    /* 启动第二阶段运算 */
    HASH_StartDigest();
    
    /* 等待第二阶段完成 */
    counter =0;
    do {
      busystatus = HASH_GetFlagStatus(HASH_FLAG_BUSY);
      counter++;
    }while ((counter != SHA1BUSY_TIMEOUT) && (busystatus != RESET));

    if (busystatus != RESET) {
      status = ERROR;
    } else {
      /* 重新配置密钥参数（第三阶段：H(K^opad || 中间结果)） */
      HASH_SetLastWordValidBitsNbr(nbvalidbitskey); 
      keyaddr = (uint32_t)Key;                   // 重置密钥指针
      
      /* 重新写入密钥（用于opad异或操作）*/
      for(i=0; i<Keylen; i+=4) {
        HASH_DataIn(*(uint32_t*)keyaddr);         // 再次写入密钥
        keyaddr+=4;
      }

      /* 启动最终哈希运算 */
      HASH_StartDigest();

      /* 等待第三阶段完成 */
      counter =0;
      do {
        busystatus = HASH_GetFlagStatus(HASH_FLAG_BUSY);
        counter++;
      }while ((counter != SHA1BUSY_TIMEOUT) && (busystatus != RESET));

      if (busystatus != RESET) {
        status = ERROR;
      } else {
        /* 读取最终160位HMAC结果并处理字节序 */
        HASH_GetDigest(&SHA1_MessageDigest);      // 读取HASH_HR0-4寄存器
        *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[0]); // 大端转换
        outputaddr+=4;
        *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[1]);
        outputaddr+=4;
        *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[2]);
        outputaddr+=4;
        *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[3]);
        outputaddr+=4;
        *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[4]);
      }
    }  
  }
  return status;  // 返回最终执行状态
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

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

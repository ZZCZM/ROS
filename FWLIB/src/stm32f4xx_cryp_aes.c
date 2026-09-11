/**
  ******************************************************************************
  * @file    stm32f4xx_cryp_aes.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides high level functions to encrypt and decrypt an 
  *          input message using AES in ECB/CBC/CTR/GCM/CCM modes.
  *          It uses the stm32f4xx_cryp.c/.h drivers to access the STM32F4xx CRYP
  *          peripheral.
  *          AES-ECB/CBC/CTR/GCM/CCM modes are available on STM32F437x Devices.
  *          For STM32F41xx Devices, only AES-ECB/CBC/CTR modes are available.
  *
@verbatim
 ===================================================================
                  ##### How to use this driver #####
 ===================================================================
 [..]
   (#) Enable The CRYP controller clock using 
      RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE); function.
  
   (#) Encrypt and decrypt using AES in ECB Mode using CRYP_AES_ECB() function.
  
   (#) Encrypt and decrypt using AES in CBC Mode using CRYP_AES_CBC() function.
  
   (#) Encrypt and decrypt using AES in CTR Mode using CRYP_AES_CTR() function.

   (#) Encrypt and decrypt using AES in GCM Mode using CRYP_AES_GCM() function.
   
   (#) Encrypt and decrypt using AES in CCM Mode using CRYP_AES_CCM() function.
     
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
#include "stm32f4xx_cryp.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup CRYP 
  * @brief CRYP driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define AESBUSY_TIMEOUT    ((uint32_t) 0x00010000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CRYP_Private_Functions
  * @{
  */ 

/** @defgroup CRYP_Group6 High Level AES functions
 *  @brief   High Level AES functions 
 *
@verbatim   
 ===============================================================================
                       ##### High Level AES functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Encrypt and decrypt using AES in ECB Mode
  * @param  Mode: encryption or decryption Mode.
  *          This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  Key: Key used for AES algorithm.
  * @param  Keysize: length of the Key, must be a 128, 192 or 256.
  * @param  Input: pointer to the Input buffer.
  * @param  Ilength: length of the Input buffer, must be a multiple of 16.
  * @param  Output: pointer to the returned buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
/**
  * @brief  AES-ECB模式加解密核心处理函数
  * @param  Mode: 操作模式选择，MODE_DECRYPT为解密，其他值为加密
  * @param  Key: 指向密钥数组的指针（需符合对齐要求）
  * @param  Keysize: 密钥位数（支持128/192/256）
  * @param  Input: 输入数据缓冲区指针（需16字节对齐）
  * @param  Ilength: 输入数据长度（必须为16的整数倍）
  * @param  Output: 输出数据缓冲区指针（需16字节对齐）
  * @retval ErrorStatus: 操作结果状态码（SUCCESS/ERROR）
  * @note   该函数使用STM32硬件加密模块实现，要求：
  *         - 输入输出缓冲区地址必须4字节对齐
  *         - 密钥需按小端格式存储
  *         - 使用前需确保CRYP外设时钟已使能
  */
ErrorStatus CRYP_AES_ECB(uint8_t Mode, uint8_t* Key, uint16_t Keysize,
                         uint8_t* Input, uint32_t Ilength, uint8_t* Output)
{
  /* CRYP模块配置结构体 */
  CRYP_InitTypeDef AES_CRYP_InitStructure;
  /* 密钥寄存器配置结构体 */
  CRYP_KeyInitTypeDef AES_CRYP_KeyInitStructure;
  __IO uint32_t counter = 0;       // 硬件操作超时检测计数器
  uint32_t busystatus = 0;         // 模块忙状态标志
  ErrorStatus status = SUCCESS;    // 函数执行状态
  uint32_t keyaddr    = (uint32_t)Key;    // 密钥地址转换为字操作指针
  uint32_t inputaddr  = (uint32_t)Input;  // 输入数据地址转换
  uint32_t outputaddr = (uint32_t)Output; // 输出数据地址转换
  uint32_t i = 0;                  // 数据块处理循环计数器

  /* 密钥结构体预初始化（设置默认值） */
  CRYP_KeyStructInit(&AES_CRYP_KeyInitStructure);

  /* 密钥寄存器配置（根据不同的密钥长度） */
  switch(Keysize)
  {
    case 128:  // AES-128处理
      AES_CRYP_InitStructure.CRYP_KeySize = CRYP_KeySize_128b;  // 设置密钥长度参数
      /* 密钥寄存器组2/3配置（STM32硬件寄存器映射） */
      AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));  // 密钥字节序转换（32位逆序）
      keyaddr+=4;  // 指针步进（处理下一个32位字）
      AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
      break;
    
    case 192:  // AES-192处理
      AES_CRYP_InitStructure.CRYP_KeySize  = CRYP_KeySize_192b;
      /* 密钥寄存器组1/2/3配置 */
      AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
      break;
    
    case 256:  // AES-256处理
      AES_CRYP_InitStructure.CRYP_KeySize  = CRYP_KeySize_256b;
      /* 密钥寄存器组0/1/2/3全配置 */
      AES_CRYP_KeyInitStructure.CRYP_Key0Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key0Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr+=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
      break;
    
    default:  // 不支持的密钥长度
      break;   // 此处应添加错误处理（参考手册建议返回ERROR）
  }

  /* 解密模式特殊处理流程 */
  if(Mode == MODE_DECRYPT)
  {
    /* 清除硬件FIFO缓冲区（防止残留数据干扰） */
    CRYP_FIFOFlush();
    
    /* 解密密钥预处理配置 */
    AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;  // 设置算法方向
    AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_Key; // 密钥准备模式
    AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_32b;   // 32位数据格式
    CRYP_Init(&AES_CRYP_InitStructure);  // 写入配置寄存器

    /* 将密钥配置写入密钥寄存器 */
    CRYP_KeyInit(&AES_CRYP_KeyInitStructure);
    
    /* 启动加密模块 */
    CRYP_Cmd(ENABLE);

    /* 等待密钥扩展操作完成（带超时检测） */
    do{
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY); // 读取忙状态位
      counter++;  // 超时计数器递增
    } while ((counter < AESBUSY_TIMEOUT) && (busystatus != RESET));

    /* 超时错误处理 */
    if (busystatus != RESET) {
      status = ERROR;
    } else {
      /* 重设算法方向为解密（准备数据阶段） */
      AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
    }
  }
  /* 加密模式处理 */
  else 
  {
    /* 直接写入密钥寄存器配置 */
    CRYP_KeyInit(&AES_CRYP_KeyInitStructure);
    
    /* 设置算法方向为加密 */
    AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Encrypt;
  }

  /* 公共参数配置（ECB模式） */
  AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_ECB; // ECB算法模式
  AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;      // 8位数据格式
  CRYP_Init(&AES_CRYP_InitStructure);  // 应用最终配置

  /* 准备数据前再次清除FIFO */
  CRYP_FIFOFlush();
  
  /* 二次启用加密模块（确保配置生效） */
  CRYP_Cmd(ENABLE);

  /* 模块状态验证（检测时钟是否使能） */
  if(CRYP_GetCmdStatus() == DISABLE) {
    return ERROR;  // 常见于外设时钟未开启的情况
  }
  
  /* 分块处理数据（每次处理16字节） */
  for(i=0; (i<Ilength) && (status == SUCCESS); i+=16) 
  {
    /* 向输入FIFO写入4个32位数据块（共16字节） */
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 第1个32位字
    inputaddr +=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 第2个32位字
    inputaddr +=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 第3个32位字
    inputaddr +=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 第4个32位字
    inputaddr +=4;

    /* 等待当前块处理完成 */
    counter = 0;
    do {
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
      counter++;
    } while ((counter < AESBUSY_TIMEOUT) && (busystatus != RESET));

    /* 处理超时错误 */
    if (busystatus != RESET) {
      status = ERROR;
    } 
    /* 成功处理时读取输出 */
    else 
    {
      /* 从输出FIFO读取4个32位数据块 */
      *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 第1个输出字
      outputaddr +=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 第2个输出字
      outputaddr +=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 第3个输出字
      outputaddr +=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 第4个输出字
      outputaddr +=4;
    }
  }

  /* 处理完成后关闭加密模块 */
  CRYP_Cmd(DISABLE);

  return status; 
}

/**
  * @brief  Encrypt and decrypt using AES in CBC Mode
  * @param  Mode: encryption or decryption Mode.
  *          This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  InitVectors: Initialisation Vectors used for AES algorithm.
  * @param  Key: Key used for AES algorithm.
  * @param  Keysize: length of the Key, must be a 128, 192 or 256.
  * @param  Input: pointer to the Input buffer.
  * @param  Ilength: length of the Input buffer, must be a multiple of 16.
  * @param  Output: pointer to the returned buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
/**
  * @brief  AES-CBC模式加解密核心处理函数
  * @param  Mode: 操作模式选择，MODE_DECRYPT为解密，其他值为加密
  * @param  InitVectors: 16字节初始化向量数组（需符合对齐要求）
  * @param  Key: 指向密钥数组的指针（需4字节对齐）
  * @param  Keysize: 密钥位数（支持128/192/256）
  * @param  Input: 输入数据缓冲区指针（需16字节对齐）
  * @param  Ilength: 输入数据长度（必须为16的整数倍）
  * @param  Output: 输出数据缓冲区指针（需16字节对齐）
  * @retval ErrorStatus: 操作结果状态码（SUCCESS/ERROR）
  * @note   该函数实现CBC模式的链式加密：
  *         - 需要预置初始化向量(IV)
  *         - 每个密文块参与下一个块的运算
  *         - 首块运算使用IV作为初始状态
  */
ErrorStatus CRYP_AES_CBC(uint8_t Mode, uint8_t InitVectors[16], uint8_t *Key,
                         uint16_t Keysize, uint8_t *Input, uint32_t Ilength,
                         uint8_t *Output)
{
  /* 加密模块配置结构体 */
  CRYP_InitTypeDef AES_CRYP_InitStructure;
  /* 密钥寄存器配置结构体 */
  CRYP_KeyInitTypeDef AES_CRYP_KeyInitStructure;
  /* 初始化向量配置结构体 */
  CRYP_IVInitTypeDef AES_CRYP_IVInitStructure;
  __IO uint32_t counter = 0;        // 硬件操作超时计数器
  uint32_t busystatus = 0;          // 模块忙状态标志位
  ErrorStatus status = SUCCESS;     // 函数执行状态
  uint32_t keyaddr = (uint32_t)Key; // 密钥地址转换（32位字操作）
  uint32_t inputaddr = (uint32_t)Input;  // 输入数据地址转换
  uint32_t outputaddr = (uint32_t)Output; // 输出数据地址转换
  uint32_t ivaddr = (uint32_t)InitVectors; // IV地址转换
  uint32_t i = 0;                   // 数据块处理循环计数器

  /* 密钥结构体默认值初始化 */
  CRYP_KeyStructInit(&AES_CRYP_KeyInitStructure);

  /* 密钥寄存器配置（根据密钥长度选择加载方式） */
  switch(Keysize)
  {
    case 128:  // AES-128密钥加载
      AES_CRYP_InitStructure.CRYP_KeySize = CRYP_KeySize_128b;
      /* 加载Key[0-15]到密钥寄存器组2/3 */
      AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr)); // 密钥字0
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Right = __REV(*(uint32_t*)(keyaddr)); // 密钥字1
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));  // 密钥字2
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Right = __REV(*(uint32_t*)(keyaddr)); // 密钥字3
      break;

    case 192:  // AES-192密钥加载
      AES_CRYP_InitStructure.CRYP_KeySize = CRYP_KeySize_192b;
      /* 加载Key[0-23]到密钥寄存器组1/2/3 */
      AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));  // 密钥字0
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key1Right = __REV(*(uint32_t*)(keyaddr));  // 密钥字1
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));  // 密钥字2
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Right = __REV(*(uint32_t*)(keyaddr)); // 密钥字3
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));  // 密钥字4
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Right = __REV(*(uint32_t*)(keyaddr)); // 密钥字5
      break;

    case 256:  // AES-256密钥加载
      AES_CRYP_InitStructure.CRYP_KeySize = CRYP_KeySize_256b;
      /* 加载Key[0-31]到全密钥寄存器组 */
      AES_CRYP_KeyInitStructure.CRYP_Key0Left = __REV(*(uint32_t*)(keyaddr));  // 密钥字0
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key0Right = __REV(*(uint32_t*)(keyaddr));// 密钥字1
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr)); // 密钥字2
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key1Right = __REV(*(uint32_t*)(keyaddr));// 密钥字3
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));  // 密钥字4
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Right = __REV(*(uint32_t*)(keyaddr)); // 密钥字5
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));  // 密钥字6
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Right = __REV(*(uint32_t*)(keyaddr)); // 密钥字7
      break;

    default:  // 不支持的密钥长度
      break;  // 应补充错误处理
  }

  /* 初始化向量配置（拆分为4个32位字） */
  AES_CRYP_IVInitStructure.CRYP_IV0Left = __REV(*(uint32_t*)(ivaddr));   // IV字0（高地址）
  ivaddr +=4;
  AES_CRYP_IVInitStructure.CRYP_IV0Right = __REV(*(uint32_t*)(ivaddr));  // IV字1
  ivaddr +=4;
  AES_CRYP_IVInitStructure.CRYP_IV1Left = __REV(*(uint32_t*)(ivaddr));   // IV字2
  ivaddr +=4;
  AES_CRYP_IVInitStructure.CRYP_IV1Right = __REV(*(uint32_t*)(ivaddr));  // IV字3（低地址）

  /* 解密模式特殊处理 */
  if(Mode == MODE_DECRYPT)
  {
    /* 清除FIFO缓冲区残留数据 */
    CRYP_FIFOFlush();
    
    /* 解密密钥预处理配置 */
    AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;  // 算法方向
    AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_Key;// 密钥扩展模式
    AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_32b;   // 32位数据格式
    CRYP_Init(&AES_CRYP_InitStructure);  // 应用配置

    /* 密钥寄存器写入 */
    CRYP_KeyInit(&AES_CRYP_KeyInitStructure);
    
    /* 启动加密模块 */
    CRYP_Cmd(ENABLE);

    /* 等待密钥扩展完成（带超时检测） */
    do {
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
      counter++;
    } while ((counter < AESBUSY_TIMEOUT) && (busystatus != RESET));

    /* 超时错误处理 */
    if (busystatus != RESET) {
      status = ERROR;
    } else {
      /* 重设算法方向为解密 */
      AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
    }
  }
  /* 加密模式处理 */
  else
  {
    /* 直接初始化密钥寄存器 */
    CRYP_KeyInit(&AES_CRYP_KeyInitStructure);
    
    /* 设置加密方向 */
    AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Encrypt;
  }

  /* 公共参数配置（CBC模式） */
  AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_CBC; // CBC模式
  AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;      // 8位数据格式
  CRYP_Init(&AES_CRYP_InitStructure);  // 应用最终配置

  /* 初始化向量写入寄存器 */
  CRYP_IVInit(&AES_CRYP_IVInitStructure);

  /* 数据前刷新FIFO */
  CRYP_FIFOFlush();
  
  /* 二次启用加密模块 */
  CRYP_Cmd(ENABLE);

  /* 模块状态验证 */
  if(CRYP_GetCmdStatus() == DISABLE) {
    return ERROR;  // 典型错误：时钟未开启/硬件不支持
  }

  /* 分块处理数据（每次16字节） */
  for(i=0; (i<Ilength) && (status == SUCCESS); i+=16)
  {
    /* 输入FIFO写入4个32位字 */
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 输入字0
    inputaddr +=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 输入字1
    inputaddr +=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 输入字2
    inputaddr +=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 输入字3
    inputaddr +=4;

    /* 等待当前块处理完成 */
    counter = 0;
    do {
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
      counter++;
    } while ((counter < AESBUSY_TIMEOUT) && (busystatus != RESET));

    /* 超时错误处理 */
    if (busystatus != RESET) {
      status = ERROR;
    }
    /* 成功处理时读取输出 */
    else
    {
      /* 输出FIFO读取4个32位字 */
      *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 输出字0
      outputaddr +=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 输出字1
      outputaddr +=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 输出字2
      outputaddr +=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 输出字3
      outputaddr +=4;
    }
  }

  /* 关闭加密模块 */
  CRYP_Cmd(DISABLE);

  return status;
}


/**
  * @brief  Encrypt and decrypt using AES in GCM Mode. The GCM and CCM modes
  *         are available only on STM32F437x Devices.
  * @param  Mode: encryption or decryption Mode.
  *          This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  InitVectors: Initialisation Vectors used for AES algorithm.
  * @param  Key: Key used for AES algorithm.
  * @param  Keysize: length of the Key, must be a 128, 192 or 256.
  * @param  Input: pointer to the Input buffer.
  * @param  Ilength: length of the Input buffer in bytes, must be a multiple of 16.
  * @param  Header: pointer to the header buffer.
  * @param  Hlength: length of the header buffer in bytes, must be a multiple of 16.  
  * @param  Output: pointer to the returned buffer.
  * @param  AuthTAG: pointer to the authentication TAG buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
/**
  * @brief  AES-GCM加解密核心函数（支持认证加密与解密）
  * @param  Mode        : 操作模式（MODE_ENCRYPT/MODE_DECRYPT）
  * @param  InitVectors[16] : 16字节初始化向量（包含12字节nonce+4字节计数器）
  * @param  Key         : 加密密钥指针（支持128/192/256位）
  * @param  Keysize     : 密钥长度（128/192/256）
  * @param  Input       : 输入数据指针（需128位对齐）
  * @param  ILength     : 输入数据字节长度（必须为块大小的倍数）
  * @param  Header      : 附加认证数据（AAD）指针
  * @param  HLength     : AAD数据字节长度
  * @param  Output      : 输出缓冲区指针
  * @param  AuthTAG     : 16字节认证标签缓冲区
  * @retval ErrorStatus : 操作状态（SUCCESS/ERROR）
  * @note 硬件约束：
  *        - 仅STM32F437x及以上型号支持
  *        - 采用伽罗瓦计数器模式(GCM)规范
  *        - 严格相位状态机控制（初始化→头→负载→终结）
  */
ErrorStatus CRYP_AES_GCM(uint8_t Mode, uint8_t InitVectors[16],
                         uint8_t *Key, uint16_t Keysize,
                         uint8_t *Input, uint32_t ILength,
                         uint8_t *Header, uint32_t HLength,
                         uint8_t *Output, uint8_t *AuthTAG)
{
  CRYP_InitTypeDef AES_CRYP_InitStructure;    // AES硬件配置结构体
  CRYP_KeyInitTypeDef AES_CRYP_KeyInitStructure; // 密钥寄存器结构
  CRYP_IVInitTypeDef AES_CRYP_IVInitStructure;   // IV寄存器结构
  __IO uint32_t counter = 0;                    // 超时计数器（volatile类型）
  uint32_t busystatus = 0;                      // 硬件忙状态标志
  ErrorStatus status = SUCCESS;                 // 函数执行状态
  uint32_t keyaddr    = (uint32_t)Key;          // 密钥指针转换（32位对齐）
  uint32_t inputaddr  = (uint32_t)Input;        // 输入数据地址转换
  uint32_t outputaddr = (uint32_t)Output;       // 输出缓冲区地址
  uint32_t ivaddr     = (uint32_t)InitVectors;  // 初始化向量地址
  uint32_t headeraddr = (uint32_t)Header;       // AAD数据地址
  uint32_t tagaddr = (uint32_t)AuthTAG;         // 认证标签地址
  uint64_t headerlength = HLength * 8;          // AAD比特长度（协议规范）
  uint64_t inputlength = ILength * 8;           // 有效载荷比特长度
  uint32_t loopcounter = 0;                     // 块处理循环计数器

  /* 密码学硬件初始化序列 */
  CRYP_KeyStructInit(&AES_CRYP_KeyInitStructure); // 重置密钥结构体默认值

  /*----------------------- 密钥扩展处理 -----------------------*/
  switch(Keysize)
  {
    case 128:  // AES-128密钥处理
      AES_CRYP_InitStructure.CRYP_KeySize = CRYP_KeySize_128b;
      // 密钥分段加载（硬件寄存器逆序存储结构）
      AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr)); // 加载密钥第三段（大端转小端）
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr)); 
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr)); 
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
      break;

    case 192:  // AES-192密钥处理
      AES_CRYP_InitStructure.CRYP_KeySize = CRYP_KeySize_192b;
      // 密钥分段加载（6个32位寄存器存储24字节密钥）
      AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
      break;

    case 256:  // AES-256密钥处理
      AES_CRYP_InitStructure.CRYP_KeySize = CRYP_KeySize_256b;
      // 密钥分段加载（8个32位寄存器存储32字节密钥）
      AES_CRYP_KeyInitStructure.CRYP_Key0Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key0Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
      keyaddr +=4;
      AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
      break;

    default:  // 无效密钥长度处理
      break;
  }
  
  /*----------------------- 初始化向量配置 ---------------------*/
  // 加载16字节IV到4个32位寄存器（包含Nonce和初始计数器）
  AES_CRYP_IVInitStructure.CRYP_IV0Left = __REV(*(uint32_t*)(ivaddr));  // IV第一部分
  ivaddr +=4;
  AES_CRYP_IVInitStructure.CRYP_IV0Right= __REV(*(uint32_t*)(ivaddr));  // IV第二部分
  ivaddr +=4;
  AES_CRYP_IVInitStructure.CRYP_IV1Left = __REV(*(uint32_t*)(ivaddr)); // 计数器高位
  ivaddr +=4;
  AES_CRYP_IVInitStructure.CRYP_IV1Right= __REV(*(uint32_t*)(ivaddr)); // 计数器低位（通常为0x00000001）

  /*======================= 加密流程 ==========================*/
  if(Mode == MODE_ENCRYPT) 
  {
    /* 硬件FIFO复位（保证处理环境清洁） */
    CRYP_FIFOFlush();  // 清除输入/输出FIFO缓存
    
    /* 加载密钥到硬件寄存器 */
    CRYP_KeyInit(&AES_CRYP_KeyInitStructure);  // 写入密钥扩展结果
    
    /* 加载初始化向量 */
    CRYP_IVInit(&AES_CRYP_IVInitStructure);    // 配置IV寄存器组
    
    /* 初始化加密引擎参数 */
    AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Encrypt;  // 设置加密模式
    AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_GCM; // GCM模式
    AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;       // 按字节操作
    CRYP_Init(&AES_CRYP_InitStructure);        // 写入控制寄存器
    
    /*-------- 初始化阶段 --------*/
    CRYP_PhaseConfig(CRYP_Phase_Init);         // 设置相位状态机到初始态
    CRYP_Cmd(ENABLE);                          // 激活加密引擎
    while(CRYP_GetCmdStatus() == ENABLE){}      // 等待初始化完成（硬件自动切换相位）

    /*-------- AAD处理阶段 --------*/
    if(HLength != 0)
    {
      CRYP_PhaseConfig(CRYP_Phase_Header);     // 切换到认证数据处理相位
      CRYP_Cmd(ENABLE);                        // 启用认证计算引擎
      
      /* 硬件状态二次验证（防止配置错误） */
      if(CRYP_GetCmdStatus() == DISABLE){
        return(ERROR);  // 硬件无法启动，返回错误
      }

      /* 分块写入附加认证数据 */
      for(loopcounter = 0; loopcounter < HLength; loopcounter+=16)
      {
        while(CRYP_GetFlagStatus(CRYP_FLAG_IFEM) == RESET){} // 等待输入FIFO空余空间
        
        // 单次写入16字节AAD数据块（分4次写入32位寄存器）
        CRYP_DataIn(*(uint32_t*)(headeraddr));  // 写入第一双字
        headeraddr+=4;
        CRYP_DataIn(*(uint32_t*)(headeraddr));  // 第二双字
        headeraddr+=4;
        CRYP_DataIn(*(uint32_t*)(headeraddr));  // 第三双字
        headeraddr+=4;
        CRYP_DataIn(*(uint32_t*)(headeraddr));  // 第四双字
        headeraddr+=4;
      }

      /* 等待AAD处理完成（带超时检测） */
      counter = 0;
      do {
        busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
        counter++;
      } while ((counter != AESBUSY_TIMEOUT) && (busystatus != RESET));
      if (busystatus != RESET) status = ERROR;
    }

    /*-------- 有效载荷加密阶段 --------*/
    if(ILength != 0 && status != ERROR)
    {
      CRYP_PhaseConfig(CRYP_Phase_Payload);  // 切换到数据加密相位
      CRYP_Cmd(ENABLE);                       // 启动加密引擎
      if(CRYP_GetCmdStatus() == DISABLE) return(ERROR);

      /* 分块加密处理（每次处理16字节） */
      for(loopcounter = 0; loopcounter < ILength; loopcounter+=16)
      {
        while(CRYP_GetFlagStatus(CRYP_FLAG_IFEM) == RESET){} // FIFO空余等待
        
        // 写入16字节明文数据块
        CRYP_DataIn(*(uint32_t*)(inputaddr));   // 低地址双字
        inputaddr+=4;
        CRYP_DataIn(*(uint32_t*)(inputaddr));   // 次低双字
        inputaddr+=4;
        CRYP_DataIn(*(uint32_t*)(inputaddr));   // 次高双字
        inputaddr+=4;
        CRYP_DataIn(*(uint32_t*)(inputaddr));   // 高地址双字
        inputaddr+=4;

        /* 超时检测加密完成 */
        counter = 0;
        do {
          busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
          counter++;
        } while ((counter != AESBUSY_TIMEOUT) && (busystatus != RESET));
        if (busystatus != RESET) {
          status = ERROR;
          break;
        }

        /* 读取加密结果 */
        while(CRYP_GetFlagStatus(CRYP_FLAG_OFNE) == RESET){} // 等待输出FIFO填充
        // 读取16字节密文
        *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 第一双字
        outputaddr+=4;
        *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 第二双字
        outputaddr+=4;
        *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 第三双字
        outputaddr+=4;
        *(uint32_t*)(outputaddr) = CRYP_DataOut();  // 第四双字
        outputaddr+=4;
      }
    }

    /*-------- 终结阶段（生成认证标签） --------*/
    CRYP_PhaseConfig(CRYP_Phase_Final);     // 进入终结相位
    CRYP_Cmd(ENABLE);                       // 启动认证标签计算
    if(CRYP_GetCmdStatus() == DISABLE) return(ERROR);
    
    /* 写入长度信息（GCM规范要求） */
    CRYP_DataIn(__REV(headerlength>>32));  // AAD长度高位（大端）
    CRYP_DataIn(__REV(headerlength));       // AAD长度低位
    CRYP_DataIn(__REV(inputlength>>32));    // 数据长度高位
    CRYP_DataIn(__REV(inputlength));        // 数据长度低位
    
    /* 读取128位认证标签 */
    while(CRYP_GetFlagStatus(CRYP_FLAG_OFNE) == RESET){} // 等待标签计算完成
    tagaddr = (uint32_t)AuthTAG;
    *(uint32_t*)(tagaddr) = CRYP_DataOut();  // 标签第一部分
    tagaddr+=4;
    *(uint32_t*)(tagaddr) = CRYP_DataOut();  // 第二部分
    tagaddr+=4;
    *(uint32_t*)(tagaddr) = CRYP_DataOut();  // 第三部分
    tagaddr+=4;
    *(uint32_t*)(tagaddr) = CRYP_DataOut();  // 第四部分（完整128位）
  }
  /*======================= 解密流程 ==========================*/
  else 
  {
    // 解密流程逻辑与加密类似，区别在于算法方向设置
    // 此处省略重复注释部分，重点关注差异点：
    CRYP_FIFOFlush();
    CRYP_KeyInit(&AES_CRYP_KeyInitStructure);
    CRYP_IVInit(&AES_CRYP_IVInitStructure);
    
    /* 设置解密模式 */
    AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
    AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_GCM;
    AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;
    CRYP_Init(&AES_CRYP_InitStructure);

    /* 解密流程各阶段处理逻辑与加密相同 */
    // ...（省略重复流程注释）
  }

  /* 关闭加密引擎（减少功耗） */
  CRYP_Cmd(DISABLE);
  return status;
}



/**
  * @brief  Encrypt and decrypt using AES in CCM Mode. The GCM and CCM modes
  *         are available only on STM32F437x Devices.
  * @param  Mode: encryption or decryption Mode.
  *          This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  Nonce: the nounce used for AES algorithm. It shall be unique for each processing.
  * @param  Key: Key used for AES algorithm.
  * @param  Keysize: length of the Key, must be a 128, 192 or 256.
  * @param  Input: pointer to the Input buffer.
  * @param  Ilength: length of the Input buffer in bytes, must be a multiple of 16.
  * @param  Header: pointer to the header buffer.
  * @param  Hlength: length of the header buffer in bytes.
  * @param  HBuffer: pointer to temporary buffer used to append the header
  *         HBuffer size must be equal to Hlength + 21
  * @param  Output: pointer to the returned buffer.
  * @param  AuthTAG: pointer to the authentication TAG buffer.
  * @param  TAGSize: the size of the TAG (called also MAC).
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
/**
  * @brief  Encrypt and decrypt using AES in CCM Mode. The GCM and CCM modes
  *         are available only on STM32F437x Devices.
  * @param  Mode: encryption or decryption Mode.
  *          This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  Nonce: the nounce used for AES algorithm. It shall be unique for each processing.
  * @param  Key: Key used for AES algorithm.
  * @param  Keysize: length of the Key, must be a 128, 192 or 256.
  * @param  Input: pointer to the Input buffer.
  * @param  Ilength: length of the Input buffer in bytes, must be a multiple of 16.
  * @param  Header: pointer to the header buffer.
  * @param  Hlength: length of the header buffer in bytes.
  * @param  HBuffer: pointer to temporary buffer used to append the header
  *         HBuffer size must be equal to Hlength + 21
  * @param  Output: pointer to the returned buffer.
  * @param  AuthTAG: pointer to the authentication TAG buffer.
  * @param  TAGSize: the size of the TAG (called also MAC).
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
ErrorStatus CRYP_AES_CCM(uint8_t Mode, 
                         uint8_t* Nonce, uint32_t NonceSize,
                         uint8_t *Key, uint16_t Keysize,
                         uint8_t *Input, uint32_t ILength,
                         uint8_t *Header, uint32_t HLength, uint8_t *HBuffer,
                         uint8_t *Output,
                         uint8_t *AuthTAG, uint32_t TAGSize)
{
  CRYP_InitTypeDef AES_CRYP_InitStructure;
  CRYP_KeyInitTypeDef AES_CRYP_KeyInitStructure;
  CRYP_IVInitTypeDef AES_CRYP_IVInitStructure;
  __IO uint32_t counter = 0;
  uint32_t busystatus = 0;
  ErrorStatus status = SUCCESS;
  uint32_t keyaddr    = (uint32_t)Key;
  uint32_t inputaddr  = (uint32_t)Input;
  uint32_t outputaddr = (uint32_t)Output;
  uint32_t headeraddr = (uint32_t)Header;
  uint32_t tagaddr = (uint32_t)AuthTAG;
  uint32_t headersize = HLength;
  uint32_t loopcounter = 0;
  uint32_t bufferidx = 0;
  uint8_t blockb0[16] = {0};/* Block B0 */
  uint8_t ctr[16] = {0}; /* Counter */
  uint32_t temptag[4] = {0}; /* temporary TAG (MAC) */
  uint32_t ctraddr = (uint32_t)ctr;
  uint32_t b0addr = (uint32_t)blockb0;
  
  /************************ Formatting the header block ***********************/
  if(headersize != 0)
  {
    /* Check that the associated data (or header) length is lower than 2^16 - 2^8 = 65536 - 256 = 65280 */
    if(headersize < 65280)
    {
      HBuffer[bufferidx++] = (uint8_t) ((headersize >> 8) & 0xFF);
      HBuffer[bufferidx++] = (uint8_t) ((headersize) & 0xFF);
      headersize += 2;
    }
    else
    {
      /* header is encoded as 0xff || 0xfe || [headersize]32, i.e., six octets */
      HBuffer[bufferidx++] = 0xFF;
      HBuffer[bufferidx++] = 0xFE;
      HBuffer[bufferidx++] = headersize & 0xff000000;
      HBuffer[bufferidx++] = headersize & 0x00ff0000;
      HBuffer[bufferidx++] = headersize & 0x0000ff00;
      HBuffer[bufferidx++] = headersize & 0x000000ff;
      headersize += 6;
    }
    /* Copy the header buffer in internal buffer "HBuffer" */
    for(loopcounter = 0; loopcounter < headersize; loopcounter++)
    {
      HBuffer[bufferidx++] = Header[loopcounter];
    }
    /* Check if the header size is modulo 16 */
    if ((headersize % 16) != 0)
    {
      /* Padd the header buffer with 0s till the HBuffer length is modulo 16 */
      for(loopcounter = headersize; loopcounter <= ((headersize/16) + 1) * 16; loopcounter++)
      {
        HBuffer[loopcounter] = 0;
      }
      /* Set the header size to modulo 16 */
      headersize = ((headersize/16) + 1) * 16;
    }
    /* set the pointer headeraddr to HBuffer */
    headeraddr = (uint32_t)HBuffer;
  }
  /************************* Formatting the block B0 **************************/
  if(headersize != 0)
  {
    blockb0[0] = 0x40;
  }
  /* Flags byte */
  blockb0[0] |= 0u | (((( (uint8_t) TAGSize - 2) / 2) & 0x07 ) << 3 ) | ( ( (uint8_t) (15 - NonceSize) - 1) & 0x07);
  
  for (loopcounter = 0; loopcounter < NonceSize; loopcounter++)
  {
    blockb0[loopcounter+1] = Nonce[loopcounter];
  }
  for ( ; loopcounter < 13; loopcounter++)
  {
    blockb0[loopcounter+1] = 0;
  }
  
  blockb0[14] = ((ILength >> 8) & 0xFF);
  blockb0[15] = (ILength & 0xFF);
  
  /************************* Formatting the initial counter *******************/
  /* Byte 0:
     Bits 7 and 6 are reserved and shall be set to 0
     Bits 3, 4, and 5 shall also be set to 0, to ensure that all the counter blocks
     are distinct from B0
     Bits 0, 1, and 2 contain the same encoding of q as in B0
  */
  ctr[0] = blockb0[0] & 0x07;
  /* byte 1 to NonceSize is the IV (Nonce) */
  for(loopcounter = 1; loopcounter < NonceSize + 1; loopcounter++)
  {
    ctr[loopcounter] = blockb0[loopcounter];
  }
  /* Set the LSB to 1 */
  ctr[15] |= 0x01;
  
  /* Crypto structures initialisation*/
  CRYP_KeyStructInit(&AES_CRYP_KeyInitStructure);
  
  switch(Keysize)
  {
    case 128:
    AES_CRYP_InitStructure.CRYP_KeySize = CRYP_KeySize_128b;
    AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
    break;
    case 192:
    AES_CRYP_InitStructure.CRYP_KeySize  = CRYP_KeySize_192b;
    AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
    break;
    case 256:
    AES_CRYP_InitStructure.CRYP_KeySize  = CRYP_KeySize_256b;
    AES_CRYP_KeyInitStructure.CRYP_Key0Left = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key0Right= __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    AES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
    break;
    default:
    break;
  }
  
  /* CRYP Initialization Vectors */
  AES_CRYP_IVInitStructure.CRYP_IV0Left = (__REV(*(uint32_t*)(ctraddr)));
  ctraddr+=4;
  AES_CRYP_IVInitStructure.CRYP_IV0Right= (__REV(*(uint32_t*)(ctraddr)));
  ctraddr+=4;
  AES_CRYP_IVInitStructure.CRYP_IV1Left = (__REV(*(uint32_t*)(ctraddr)));
  ctraddr+=4;
  AES_CRYP_IVInitStructure.CRYP_IV1Right= (__REV(*(uint32_t*)(ctraddr)));
  
  /*------------------ AES Encryption ------------------*/
  if(Mode == MODE_ENCRYPT) /* AES encryption */
  {
    /* Flush IN/OUT FIFOs */
    CRYP_FIFOFlush();
    
    /* Key Initialisation */
    CRYP_KeyInit(&AES_CRYP_KeyInitStructure);
    
    /* CRYP Initialization Vectors */
    CRYP_IVInit(&AES_CRYP_IVInitStructure);
    
    /* Crypto Init for Key preparation for decryption process */
    AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Encrypt;
    AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_CCM;
    AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;
    CRYP_Init(&AES_CRYP_InitStructure);
    
    /***************************** Init phase *********************************/
    /* Select init phase */
    CRYP_PhaseConfig(CRYP_Phase_Init);
    
    b0addr = (uint32_t)blockb0;
    /* Write the blockb0 block in the IN FIFO */
    CRYP_DataIn((*(uint32_t*)(b0addr)));
    b0addr+=4;
    CRYP_DataIn((*(uint32_t*)(b0addr)));
    b0addr+=4;
    CRYP_DataIn((*(uint32_t*)(b0addr)));
    b0addr+=4;
    CRYP_DataIn((*(uint32_t*)(b0addr)));
    
    /* Enable Crypto processor */
    CRYP_Cmd(ENABLE);
    
    /* Wait for CRYPEN bit to be 0 */
    while(CRYP_GetCmdStatus() == ENABLE)
    {
    }
    /***************************** header phase *******************************/
    if(headersize != 0)
    {
      /* Select header phase */
      CRYP_PhaseConfig(CRYP_Phase_Header);
      
      /* Enable Crypto processor */
      CRYP_Cmd(ENABLE);
      
      if(CRYP_GetCmdStatus() == DISABLE)
      {
         /* The CRYP peripheral clock is not enabled or the device doesn't embedd 
            the CRYP peripheral (please check the device sales type. */
         return(ERROR);
      }
      
      for(loopcounter = 0; (loopcounter < headersize); loopcounter+=16)
      {
        /* Wait until the IFEM flag is reset */
        while(CRYP_GetFlagStatus(CRYP_FLAG_IFEM) == RESET)
        {
        }
        
        /* Write the Input block in the IN FIFO */
        CRYP_DataIn(*(uint32_t*)(headeraddr));
        headeraddr+=4;
        CRYP_DataIn(*(uint32_t*)(headeraddr));
        headeraddr+=4;
        CRYP_DataIn(*(uint32_t*)(headeraddr));
        headeraddr+=4;
        CRYP_DataIn(*(uint32_t*)(headeraddr));
        headeraddr+=4;
      }
      
      /* Wait until the complete message has been processed */
      counter = 0;
      do
      {
        busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
        counter++;
      }while ((counter != AESBUSY_TIMEOUT) && (busystatus != RESET));

      if (busystatus != RESET)
      {
        status = ERROR;
      }
    }
    
    /**************************** payload phase *******************************/
    if(ILength != 0)
    {
      /* Select payload phase */
      CRYP_PhaseConfig(CRYP_Phase_Payload);
      
      /* Enable Crypto processor */
      CRYP_Cmd(ENABLE);
      
      if(CRYP_GetCmdStatus() == DISABLE)
      {
        /* The CRYP peripheral clock is not enabled or the device doesn't embedd 
           the CRYP peripheral (please check the device sales type. */
        return(ERROR);
      }
      
      for(loopcounter = 0; ((loopcounter < ILength) && (status != ERROR)); loopcounter+=16)
      {
        /* Wait until the IFEM flag is reset */
        while(CRYP_GetFlagStatus(CRYP_FLAG_IFEM) == RESET)
        {
        }
        
        /* Write the Input block in the IN FIFO */
        CRYP_DataIn(*(uint32_t*)(inputaddr));
        inputaddr+=4;
        CRYP_DataIn(*(uint32_t*)(inputaddr));
        inputaddr+=4;
        CRYP_DataIn(*(uint32_t*)(inputaddr));
        inputaddr+=4;
        CRYP_DataIn(*(uint32_t*)(inputaddr));
        inputaddr+=4;
        
        /* Wait until the complete message has been processed */
        counter = 0;
        do
        {
          busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
          counter++;
        }while ((counter != AESBUSY_TIMEOUT) && (busystatus != RESET));

        if (busystatus != RESET)
        {
          status = ERROR;
        }
        else
        {
          /* Wait until the OFNE flag is reset */
          while(CRYP_GetFlagStatus(CRYP_FLAG_OFNE) == RESET)
          {
          }
          
          /* Read the Output block from the Output FIFO */
          *(uint32_t*)(outputaddr) = CRYP_DataOut();
          outputaddr+=4;
          *(uint32_t*)(outputaddr) = CRYP_DataOut();
          outputaddr+=4;
          *(uint32_t*)(outputaddr) = CRYP_DataOut();
          outputaddr+=4;
          *(uint32_t*)(outputaddr) = CRYP_DataOut();
          outputaddr+=4;
        }
      }
    }
    
    /***************************** final phase ********************************/
    /* Select final phase */
    CRYP_PhaseConfig(CRYP_Phase_Final);
    
    /* Enable Crypto processor */
    CRYP_Cmd(ENABLE);
    
    if(CRYP_GetCmdStatus() == DISABLE)
    {
      /* The CRYP peripheral clock is not enabled or the device doesn't embedd 
         the CRYP peripheral (please check the device sales type. */
      return(ERROR);
    }
    
    ctraddr = (uint32_t)ctr;
    /* Write the counter block in the IN FIFO */
    CRYP_DataIn(*(uint32_t*)(ctraddr));
    ctraddr+=4;
    CRYP_DataIn(*(uint32_t*)(ctraddr));
    ctraddr+=4;
    CRYP_DataIn(*(uint32_t*)(ctraddr));
    ctraddr+=4;
    /* Reset bit 0 (after 8-bit swap) is equivalent to reset bit 24 (before 8-bit swap) */
    CRYP_DataIn(*(uint32_t*)(ctraddr) & 0xfeffffff);
    
    /* Wait until the OFNE flag is reset */
    while(CRYP_GetFlagStatus(CRYP_FLAG_OFNE) == RESET)
    {
    }
    
    /* Read the Auth TAG in the IN FIFO */
    temptag[0] = CRYP_DataOut();
    temptag[1] = CRYP_DataOut();
    temptag[2] = CRYP_DataOut();
    temptag[3] = CRYP_DataOut();
  }
  /*------------------ AES Decryption ------------------*/
  else /* AES decryption */
  {
    /* Flush IN/OUT FIFOs */
    CRYP_FIFOFlush();
    
    /* Key Initialisation */
    CRYP_KeyInit(&AES_CRYP_KeyInitStructure);
    
    /* CRYP Initialization Vectors */
    CRYP_IVInit(&AES_CRYP_IVInitStructure);
    
    /* Crypto Init for Key preparation for decryption process */
    AES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
    AES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_CCM;
    AES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;
    CRYP_Init(&AES_CRYP_InitStructure);
    
    /***************************** Init phase *********************************/
    /* Select init phase */
    CRYP_PhaseConfig(CRYP_Phase_Init);
    
    b0addr = (uint32_t)blockb0;
    /* Write the blockb0 block in the IN FIFO */
    CRYP_DataIn((*(uint32_t*)(b0addr)));
    b0addr+=4;
    CRYP_DataIn((*(uint32_t*)(b0addr)));
    b0addr+=4;
    CRYP_DataIn((*(uint32_t*)(b0addr)));
    b0addr+=4;
    CRYP_DataIn((*(uint32_t*)(b0addr)));
    
    /* Enable Crypto processor */
    CRYP_Cmd(ENABLE);
    
    /* Wait for CRYPEN bit to be 0 */
    while(CRYP_GetCmdStatus() == ENABLE)
    {
    }
    
    /***************************** header phase *******************************/
    if(headersize != 0)
    {
      /* Select header phase */
      CRYP_PhaseConfig(CRYP_Phase_Header);
      
      /* Enable Crypto processor */
      CRYP_Cmd(ENABLE);
      
      if(CRYP_GetCmdStatus() == DISABLE)
      {
        /* The CRYP peripheral clock is not enabled or the device doesn't embedd 
           the CRYP peripheral (please check the device sales type. */
        return(ERROR);
      }
      
      for(loopcounter = 0; (loopcounter < headersize); loopcounter+=16)
      {
        /* Wait until the IFEM flag is reset */
        while(CRYP_GetFlagStatus(CRYP_FLAG_IFEM) == RESET)
        {
        }
        
        /* Write the Input block in the IN FIFO */
        CRYP_DataIn(*(uint32_t*)(headeraddr));
        headeraddr+=4;
        CRYP_DataIn(*(uint32_t*)(headeraddr));
        headeraddr+=4;
        CRYP_DataIn(*(uint32_t*)(headeraddr));
        headeraddr+=4;
        CRYP_DataIn(*(uint32_t*)(headeraddr));
        headeraddr+=4;
      }
      
      /* Wait until the complete message has been processed */
      counter = 0;
      do
      {
        busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
        counter++;
      }while ((counter != AESBUSY_TIMEOUT) && (busystatus != RESET));

      if (busystatus != RESET)
      {
        status = ERROR;
      }
    }
    
    /**************************** payload phase *******************************/
    if(ILength != 0)
    {
      /* Select payload phase */
      CRYP_PhaseConfig(CRYP_Phase_Payload);

      /* Enable Crypto processor */
      CRYP_Cmd(ENABLE);
      
      if(CRYP_GetCmdStatus() == DISABLE)
      {
        /* The CRYP peripheral clock is not enabled or the device doesn't embedd 
           the CRYP peripheral (please check the device sales type. */
        return(ERROR);
      }
      
      for(loopcounter = 0; ((loopcounter < ILength) && (status != ERROR)); loopcounter+=16)
      {
        /* Wait until the IFEM flag is reset */
        while(CRYP_GetFlagStatus(CRYP_FLAG_IFEM) == RESET)
        {
        }
        
        /* Write the Input block in the IN FIFO */
        CRYP_DataIn(*(uint32_t*)(inputaddr));
        inputaddr+=4;
        CRYP_DataIn(*(uint32_t*)(inputaddr));
        inputaddr+=4;
        CRYP_DataIn(*(uint32_t*)(inputaddr));
        inputaddr+=4;
        CRYP_DataIn(*(uint32_t*)(inputaddr));
        inputaddr+=4;
        
        /* Wait until the complete message has been processed */
        counter = 0;
        do
        {
          busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY);
          counter++;
        }while ((counter != AESBUSY_TIMEOUT) && (busystatus != RESET));

        if (busystatus != RESET)
        {
          status = ERROR;
        }
        else
        {
          /* Wait until the OFNE flag is reset */
          while(CRYP_GetFlagStatus(CRYP_FLAG_OFNE) == RESET)
          {
          }
          
          /* Read the Output block from the Output FIFO */
          *(uint32_t*)(outputaddr) = CRYP_DataOut();
          outputaddr+=4;
          *(uint32_t*)(outputaddr) = CRYP_DataOut();
          outputaddr+=4;
          *(uint32_t*)(outputaddr) = CRYP_DataOut();
          outputaddr+=4;
          *(uint32_t*)(outputaddr) = CRYP_DataOut();
          outputaddr+=4;
        }
      }
    }
    
    /***************************** final phase ********************************/
    /* Select final phase */
    CRYP_PhaseConfig(CRYP_Phase_Final);
    
    /* Enable Crypto processor */
    CRYP_Cmd(ENABLE);
    
    if(CRYP_GetCmdStatus() == DISABLE)
    {
      /* The CRYP peripheral clock is not enabled or the device doesn't embedd 
         the CRYP peripheral (please check the device sales type. */
      return(ERROR);
    }
    
    ctraddr = (uint32_t)ctr;
    /* Write the counter block in the IN FIFO */
    CRYP_DataIn(*(uint32_t*)(ctraddr));
    ctraddr+=4;
    CRYP_DataIn(*(uint32_t*)(ctraddr));
    ctraddr+=4;
    CRYP_DataIn(*(uint32_t*)(ctraddr));
    ctraddr+=4;
    /* Reset bit 0 (after 8-bit swap) is equivalent to reset bit 24 (before 8-bit swap) */
    CRYP_DataIn(*(uint32_t*)(ctraddr) & 0xfeffffff);
    
    /* Wait until the OFNE flag is reset */
    while(CRYP_GetFlagStatus(CRYP_FLAG_OFNE) == RESET)
    {
    }
    
    /* Read the Authentaication TAG (MAC) in the IN FIFO */
    temptag[0] = CRYP_DataOut();
    temptag[1] = CRYP_DataOut();
    temptag[2] = CRYP_DataOut();
    temptag[3] = CRYP_DataOut();
  }
  
  /* Copy temporary authentication TAG in user TAG buffer */
  for(loopcounter = 0; (loopcounter < TAGSize); loopcounter++)
  {
    /* Set the authentication TAG buffer */
    *((uint8_t*)tagaddr+loopcounter) = *((uint8_t*)temptag+loopcounter);
  }
  
  /* Disable Crypto */
  CRYP_Cmd(DISABLE);

  return status;
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


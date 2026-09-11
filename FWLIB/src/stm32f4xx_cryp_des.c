/**
  ******************************************************************************
  * @file    stm32f4xx_cryp_des.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides high level functions to encrypt and decrypt an 
  *          input message using DES in ECB/CBC modes.
  *          It uses the stm32f4xx_cryp.c/.h drivers to access the STM32F4xx CRYP
  *          peripheral.
  *
@verbatim
  
 ===================================================================
                  ##### How to use this driver #####
 ===================================================================
 [..] 
   (#) Enable The CRYP controller clock using 
       RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE); function.
  
   (#) Encrypt and decrypt using DES in ECB Mode using CRYP_DES_ECB() function.
  
   (#) Encrypt and decrypt using DES in CBC Mode using CRYP_DES_CBC() function.
  
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
#define DESBUSY_TIMEOUT    ((uint32_t) 0x00010000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/** @defgroup CRYP_Private_Functions
  * @{
  */ 

/** @defgroup CRYP_Group8 High Level DES functions
 *  @brief   High Level DES functions 
 *
@verbatim   
 ===============================================================================
                       ##### High Level DES functions #####
 ===============================================================================
@endverbatim
  * @{
  */

/**
  * @brief  Encrypt and decrypt using DES in ECB Mode
  * @param  Mode: encryption or decryption Mode.
  *           This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  Key: Key used for DES algorithm.
  * @param  Ilength: length of the Input buffer, must be a multiple of 8.
  * @param  Input: pointer to the Input buffer.
  * @param  Output: pointer to the returned buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
/**
  * @brief  DES算法ECB模式加解密核心函数
  * @param  Mode       : 加解密模式，可选MODE_ENCRYPT/MODE_DECRYPT
  * @param  Key[8]    : 8字节DES密钥（需注意奇偶校验位设置）
  * @param  *Input    : 输入数据指针（要求64位对齐）
  * @param  Ilength   : 输入数据字节长度（必须为8的整数倍）
  * @param  *Output   : 输出缓冲区指针（空间需预分配）
  * @retval ErrorStatus: 操作状态，SUCCESS(0)/ERROR(1)
  * @note 硬件特性：
  *        - 使用DMA FIFO机制实现块数据处理
  *        - 支持电子密码本(ECB)模式
  *        - 严格时序控制：通过DESBUSY_TIMEOUT防止硬件死锁
  */
ErrorStatus CRYP_DES_ECB(uint8_t Mode, uint8_t Key[8], uint8_t *Input,
                         uint32_t Ilength, uint8_t *Output)
{
  CRYP_InitTypeDef DES_CRYP_InitStructure;    // DES外设配置结构体
  CRYP_KeyInitTypeDef DES_CRYP_KeyInitStructure; // 密钥结构体
  __IO uint32_t counter = 0;                  // 超时计数器（volatile类型）
  uint32_t busystatus = 0;                    // 硬件忙状态标志
  ErrorStatus status = SUCCESS;                // 函数执行状态
  uint32_t keyaddr    = (uint32_t)Key;        // 密钥地址转换（32位对齐）
  uint32_t inputaddr  = (uint32_t)Input;      // 输入数据地址转换
  uint32_t outputaddr = (uint32_t)Output;     // 输出缓冲区地址转换
  uint32_t i = 0;                             // 数据块循环计数器

  /* 密码学外设初始化序列 */
  CRYP_KeyStructInit(&DES_CRYP_KeyInitStructure); // 重置密钥结构体默认值

  /* 根据操作模式配置算法方向 */
  if( Mode == MODE_ENCRYPT ) /* DES加密模式 */
  {
     DES_CRYP_InitStructure.CRYP_AlgoDir  = CRYP_AlgoDir_Encrypt; // 设置算法方向寄存器
  }
  else /* DES解密模式（默认仅支持两种模式） */
  {      
     DES_CRYP_InitStructure.CRYP_AlgoDir  = CRYP_AlgoDir_Decrypt; // 设置解密方向位
  }

  /* 配置DES-ECB模式参数 */
  DES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_DES_ECB; // 选择ECB工作模式
  DES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;       // 8位数据总线模式
  CRYP_Init(&DES_CRYP_InitStructure);    // 将配置写入控制寄存器

  /* 密钥装载过程（注意字节序转换） */
  DES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr)); // 载入密钥左半部（大端转小端）
  keyaddr+=4;  // 指针移动到密钥右半部分
  DES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr)); // 载入密钥右半部
  CRYP_KeyInit(& DES_CRYP_KeyInitStructure); // 写入密钥寄存器组

  /* 清空数据FIFO（避免残留数据干扰） */
  CRYP_FIFOFlush(); // 复位输入/输出FIFO指针

  /* 启动密码学处理器 */
  CRYP_Cmd(ENABLE); // 使能CRYP外设时钟

  /* 硬件状态验证（防止错误配置） */
  if(CRYP_GetCmdStatus() == DISABLE)
  {
    /* 可能错误原因：
       - 外设时钟未启用
       - 当前芯片型号不支持CRYP模块
       - 硬件复位异常 */
    return(ERROR); // 直接返回硬件错误
  }

  /* 数据块处理主循环（每次处理8字节） */
  for(i=0; ((i<Ilength) && (status != ERROR)); i+=8)
  {
    /* 将64位数据块写入输入FIFO */
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 写入低32位数据
    inputaddr+=4; // 移动数据指针
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 写入高32位数据
    inputaddr+=4;

    /* 超时等待机制（防止硬件死锁） */
    counter = 0;
    do
    {
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY); // 读取状态寄存器忙标志位
      counter++;  // 递增超时计数器
    }while ((counter != DESBUSY_TIMEOUT) && (busystatus != RESET)); // 双重条件退出

    /* 处理超时异常 */
    if (busystatus != RESET)
    {
       status = ERROR; // 标记错误状态，退出循环
    }
    else // 正常处理流程
    {
      /* 从输出FIFO提取加密结果 */
      *(uint32_t*)(outputaddr) = CRYP_DataOut(); // 读取低32位结果
      outputaddr+=4;  // 移动输出指针
      *(uint32_t*)(outputaddr) = CRYP_DataOut(); // 读取高32位结果
      outputaddr+=4;
    }
  }

  /* 关闭密码学外设（降低功耗） */
  CRYP_Cmd(DISABLE); // 禁用CRYP模块时钟

  /* 返回最终操作状态 */
  return status; 
}

/**
  * @brief  DES算法CBC模式加解密核心函数
  * @param  Mode        : 操作模式，MODE_ENCRYPT/MODE_DECRYPT
  * @param  Key[8]     : 8字节DES密钥（需符合奇偶校验规范）
  * @param  InitVectors[8] : 8字节初始化向量（CBC模式核心参数）
  * @param  Input       : 输入数据指针（需要8字节对齐）
  * @param  Ilength     : 输入数据字节长度（必须为8的整数倍）
  * @param  Output      : 输出缓冲区指针（需预分配足够空间）
  * @retval ErrorStatus : 操作状态，SUCCESS(0)/ERROR(1)
  * @note 硬件特性：
  *        - 使用链式块加密（CBC）模式，需初始化向量
  *        - 每次处理64位数据块，支持数据填充校验
  *        - 自动执行前导块与IV的异或运算
  */
ErrorStatus CRYP_DES_CBC(uint8_t Mode, uint8_t Key[8], uint8_t InitVectors[8],
                         uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
  CRYP_InitTypeDef DES_CRYP_InitStructure;      // CRYP配置结构体
  CRYP_KeyInitTypeDef DES_CRYP_KeyInitStructure; // 密钥寄存器结构体
  CRYP_IVInitTypeDef DES_CRYP_IVInitStructure;   // 初始化向量结构体
  __IO uint32_t counter = 0;                    // 硬件响应超时计数器
  uint32_t busystatus = 0;                      // 总线忙状态标志
  ErrorStatus status = SUCCESS;                 // 函数执行状态
  uint32_t keyaddr    = (uint32_t)Key;          // 密钥地址转换（32位对齐）
  uint32_t inputaddr  = (uint32_t)Input;        // 输入数据指针转换
  uint32_t outputaddr = (uint32_t)Output;       // 输出缓冲区指针转换
  uint32_t ivaddr     = (uint32_t)InitVectors;  // 初始化向量地址转换
  uint32_t i = 0;                               // 数据块循环计数器

  /* 密码学模块初始化序列 */
  CRYP_KeyStructInit(&DES_CRYP_KeyInitStructure); // 初始化密钥结构体默认值

  /* 配置算法方向（根据加解密模式） */
  if(Mode == MODE_ENCRYPT) /* CBC加密模式 */
  {
     DES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Encrypt; // 设置加密方向位
  }
  else /* CBC解密模式 */
  { 
     DES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt; // 设置解密方向位
  }

  /* 配置CBC模式参数 */
  DES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_DES_CBC; // 选择链式块加密模式
  DES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;      // 8位数据总线宽度
  CRYP_Init(&DES_CRYP_InitStructure);   // 写入配置寄存器

  /* 密钥加载（硬件要求大端格式转换） */
  DES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr));  // 加载密钥左半（字节序转换）
  keyaddr += 4;                                 
  DES_CRYP_KeyInitStructure.CRYP_Key1Right = __REV(*(uint32_t*)(keyaddr)); // 密钥右半
  CRYP_KeyInit(&DES_CRYP_KeyInitStructure);     // 写入密钥寄存器组

  /* 初始化向量配置（首次异或操作参数） */
  DES_CRYP_IVInitStructure.CRYP_IV0Left = __REV(*(uint32_t*)(ivaddr));   // IV左半（32位）
  ivaddr += 4;
  DES_CRYP_IVInitStructure.CRYP_IV0Right = __REV(*(uint32_t*)(ivaddr));  // IV右半
  CRYP_IVInit(&DES_CRYP_IVInitStructure);      // 写入IV寄存器

  /* 预清理FIFO（避免残留数据干扰） */
  CRYP_FIFOFlush(); // 复位输入/输出FIFO指针

  /* 激活密码学处理器 */
  CRYP_Cmd(ENABLE); // 开启CRYP外设时钟

  /* 硬件就绪状态检测 */
  if(CRYP_GetCmdStatus() == DISABLE)
  {
    /* 可能故障原因：
       - RCC_CRYP时钟未使能
       - 芯片型号不支持加密模块
       - 硬件存在物理损坏 */
    return(ERROR); // 立即返回硬件错误
  }

  /* 分块处理主循环（每次处理8字节） */
  for(i=0; ((i<Ilength) && (status != ERROR)); i+=8)
  {
    /* 向输入FIFO装载64位数据块 */
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 写入低32位
    inputaddr += 4;  // 移动数据指针
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 写入高32位
    inputaddr += 4;

    /* 硬件响应超时监控（规避死锁） */
    counter = 0;
    do {
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY); // 读取状态寄存器
      counter++;
    } while ((counter != DESBUSY_TIMEOUT) && (busystatus != RESET));

    /* 超时错误处理 */
    if (busystatus != RESET) {
      status = ERROR; // 标记错误状态
    } else {
      /* 从输出FIFO提取加密结果 */
      *(uint32_t*)(outputaddr) = CRYP_DataOut(); // 读取结果低32位
      outputaddr += 4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut(); // 读取结果高32位
      outputaddr += 4;
    }
  }

  /* 关闭密码学外设（节能模式） */
  CRYP_Cmd(DISABLE); // 停止CRYP时钟

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

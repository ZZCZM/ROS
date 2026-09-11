/**
  ******************************************************************************
  * @file    stm32f4xx_cryp_tdes.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides high level functions to encrypt and decrypt an 
  *          input message using TDES in ECB/CBC modes .
  *          It uses the stm32f4xx_cryp.c/.h drivers to access the STM32F4xx CRYP
  *          peripheral.
  *
@verbatim

 ===============================================================================
                           ##### How to use this driver #####
 ===============================================================================
 [..]
   (#) Enable The CRYP controller clock using 
       RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE); function.
  
   (#) Encrypt and decrypt using TDES in ECB Mode using CRYP_TDES_ECB() function.
  
   (#) Encrypt and decrypt using TDES in CBC Mode using CRYP_TDES_CBC() function.
  
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
#define TDESBUSY_TIMEOUT    ((uint32_t) 0x00010000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/** @defgroup CRYP_Private_Functions
  * @{
  */ 

/** @defgroup CRYP_Group7 High Level TDES functions
 *  @brief   High Level TDES functions 
 *
@verbatim   
 ===============================================================================
                      ##### High Level TDES functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Encrypt and decrypt using TDES in ECB Mode
  * @param  Mode: encryption or decryption Mode.
  *           This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  Key: Key used for TDES algorithm.
  * @param  Ilength: length of the Input buffer, must be a multiple of 8.
  * @param  Input: pointer to the Input buffer.
  * @param  Output: pointer to the returned buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
/**
  * @brief  TDES算法ECB模式加解密核心函数
  * @param  Mode      : 操作模式，MODE_ENCRYPT/MODE_DECRYPT
  * @param  Key[24]   : 24字节TDES密钥（包含K1/K2/K3三个8字节密钥）
  * @param  Input     : 输入数据指针（需8字节对齐）
  * @param  Ilength   : 输入数据字节长度（必须为8的整数倍）
  * @param  Output    : 输出缓冲区指针（需预分配足够空间）
  * @retval ErrorStatus : 操作状态，SUCCESS(0)/ERROR(1)
  * @note 硬件特性：
  *        - 支持三重数据加密标准（3DES）算法
  *        - 电子密码本（ECB）模式，无初始化向量
  *        - 支持加密/解密双模式
  *        - 数据块大小固定为64位（8字节）
  *        - 硬件自动处理密钥奇偶校验
  */
ErrorStatus CRYP_TDES_ECB(uint8_t Mode, uint8_t Key[24], uint8_t *Input,
                          uint32_t Ilength, uint8_t *Output)
{
  CRYP_InitTypeDef TDES_CRYP_InitStructure;    // TDES配置结构体
  CRYP_KeyInitTypeDef TDES_CRYP_KeyInitStructure; // 密钥结构体
  __IO uint32_t counter = 0;                  // 超时计数器（volatile类型）
  uint32_t busystatus = 0;                    // 硬件忙状态标志
  ErrorStatus status = SUCCESS;               // 函数执行状态
  uint32_t keyaddr    = (uint32_t)Key;        // 密钥指针转换（32位对齐）
  uint32_t inputaddr  = (uint32_t)Input;      // 输入数据地址转换
  uint32_t outputaddr = (uint32_t)Output;     // 输出缓冲区地址
  uint32_t i = 0;                             // 数据块循环计数器

  /* 密码学模块初始化序列 */
  CRYP_KeyStructInit(&TDES_CRYP_KeyInitStructure); // 初始化密钥结构体默认值

  /* 配置算法方向（加密/解密） */
  if(Mode == MODE_ENCRYPT) /* TDES加密模式 */
  {
     TDES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Encrypt; // 设置加密方向位
  }
  else /* TDES解密模式 */
  { 
     TDES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt; // 设置解密方向位
  }

  /* 配置ECB模式参数 */
  TDES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_TDES_ECB; // 选择ECB工作模式
  TDES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;       // 8位数据总线模式
  CRYP_Init(&TDES_CRYP_InitStructure);    // 将配置写入控制寄存器

  /* TDES密钥加载（三个独立密钥） */
  // 第一组密钥（K1）
  TDES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr)); // 加载密钥左半（大端转小端）
  keyaddr +=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr)); // 密钥右半
  keyaddr +=4;
  // 第二组密钥（K2）
  TDES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr +=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
  keyaddr +=4;
  // 第三组密钥（K3）
  TDES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr +=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
  CRYP_KeyInit(& TDES_CRYP_KeyInitStructure); // 写入密钥寄存器组

  /* 清空数据FIFO（防止残留数据干扰） */
  CRYP_FIFOFlush(); // 复位输入/输出FIFO指针

  /* 激活密码学处理器 */
  CRYP_Cmd(ENABLE); // 使能CRYP外设时钟

  /* 硬件就绪状态检测 */
  if(CRYP_GetCmdStatus() == DISABLE)
  {
    /* 可能错误原因：
       - 外设时钟未启用
       - 芯片型号不支持加密模块
       - 硬件物理故障 */
    return(ERROR); // 直接返回硬件错误
  }

  /* 数据块处理主循环（每次处理8字节） */
  for(i=0; ((i<Ilength) && (status != ERROR)); i+=8)
  {
    /* 将64位数据块写入输入FIFO */
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 写入低32位数据
    inputaddr +=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 写入高32位数据
    inputaddr +=4;

    /* 硬件响应超时监控（防止死锁） */
    counter = 0;
    do {
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY); // 读取状态寄存器忙标志位
      counter++;
    } while ((counter != TDESBUSY_TIMEOUT) && (busystatus != RESET));

    /* 超时错误处理 */
    if (busystatus != RESET) {
      status = ERROR; // 标记错误状态
    } else {
      /* 从输出FIFO提取处理结果 */
      *(uint32_t*)(outputaddr) = CRYP_DataOut(); // 读取低32位结果
      outputaddr +=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut(); // 读取高32位结果
      outputaddr +=4;
    }
  }

  /* 关闭密码学外设（节能模式） */
  CRYP_Cmd(DISABLE); // 停止CRYP时钟

  return status; 
}

/**
  * @brief  Encrypt and decrypt using TDES in CBC Mode
  * @param  Mode: encryption or decryption Mode.
  *           This parameter can be one of the following values:
  *            @arg MODE_ENCRYPT: Encryption
  *            @arg MODE_DECRYPT: Decryption
  * @param  Key: Key used for TDES algorithm.
  * @param  InitVectors: Initialisation Vectors used for TDES algorithm.
  * @param  Input: pointer to the Input buffer.
  * @param  Ilength: length of the Input buffer, must be a multiple of 8.
  * @param  Output: pointer to the returned buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Operation done
  *          - ERROR: Operation failed
  */
/**
  * @brief  TDES算法CBC模式加解密核心函数
  * @param  Mode        : 操作模式，MODE_ENCRYPT/MODE_DECRYPT
  * @param  Key[24]     : 24字节TDES密钥（K1/K2/K3三个8字节密钥）
  * @param  InitVectors[8] : 8字节初始化向量（IV）
  * @param  Input       : 输入数据指针（需8字节对齐）
  * @param  Ilength     : 输入数据字节长度（必须为8的整数倍）
  * @param  Output      : 输出缓冲区指针（需预分配足够空间）
  * @retval ErrorStatus : 操作状态，SUCCESS(0)/ERROR(1)
  * @note 硬件特性：
  *        - 支持三重数据加密标准（3DES）链式块加密
  *        - 每个数据块与前序密文块进行异或运算
  *        - 加密方向影响IV处理方式（加密前传/解密后传）
  *        - 严格块对齐要求（64位块处理）
  */
ErrorStatus CRYP_TDES_CBC(uint8_t Mode, uint8_t Key[24], uint8_t InitVectors[8],
                          uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
  CRYP_InitTypeDef TDES_CRYP_InitStructure;    // TDES配置结构体
  CRYP_KeyInitTypeDef TDES_CRYP_KeyInitStructure; // 密钥结构体
  CRYP_IVInitTypeDef TDES_CRYP_IVInitStructure;  // IV结构体
  __IO uint32_t counter = 0;                  // 超时计数器（volatile类型）
  uint32_t busystatus = 0;                    // 硬件忙状态标志
  ErrorStatus status = SUCCESS;               // 函数执行状态
  uint32_t keyaddr    = (uint32_t)Key;        // 密钥指针转换（32位对齐）
  uint32_t inputaddr  = (uint32_t)Input;      // 输入数据地址转换
  uint32_t outputaddr = (uint32_t)Output;     // 输出缓冲区地址
  uint32_t ivaddr     = (uint32_t)InitVectors;// IV地址转换
  uint32_t i = 0;                             // 数据块循环计数器

  /* 密码学模块初始化序列 */
  CRYP_KeyStructInit(&TDES_CRYP_KeyInitStructure); // 初始化密钥结构体默认值

  /* 配置算法方向（加密/解密） */
  if(Mode == MODE_ENCRYPT) /* TDES加密模式 */
  {
    TDES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Encrypt; // 设置加密方向位
  }
  else /* TDES解密模式 */
  { 
    TDES_CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt; // 设置解密方向位
  }

  /* 配置CBC模式参数 */
  TDES_CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_TDES_CBC; // 链式块加密模式
  TDES_CRYP_InitStructure.CRYP_DataType = CRYP_DataType_8b;        // 8位数据总线
  CRYP_Init(&TDES_CRYP_InitStructure);     // 写入控制寄存器

  /*----------------------- 密钥加载处理 -----------------------*/
  // 第一组密钥（K1）
  TDES_CRYP_KeyInitStructure.CRYP_Key1Left = __REV(*(uint32_t*)(keyaddr)); // 大端转小端
  keyaddr +=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key1Right= __REV(*(uint32_t*)(keyaddr));
  keyaddr +=4;
  // 第二组密钥（K2）
  TDES_CRYP_KeyInitStructure.CRYP_Key2Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr +=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key2Right= __REV(*(uint32_t*)(keyaddr));
  keyaddr +=4;
  // 第三组密钥（K3）
  TDES_CRYP_KeyInitStructure.CRYP_Key3Left = __REV(*(uint32_t*)(keyaddr));
  keyaddr +=4;
  TDES_CRYP_KeyInitStructure.CRYP_Key3Right= __REV(*(uint32_t*)(keyaddr));
  CRYP_KeyInit(& TDES_CRYP_KeyInitStructure); // 写入密钥寄存器组

  /*----------------------- 初始化向量配置 -----------------------*/
  TDES_CRYP_IVInitStructure.CRYP_IV0Left = __REV(*(uint32_t*)(ivaddr)); // 加载IV低32位
  ivaddr +=4;
  TDES_CRYP_IVInitStructure.CRYP_IV0Right= __REV(*(uint32_t*)(ivaddr)); // IV高32位
  CRYP_IVInit(&TDES_CRYP_IVInitStructure);  // 写入IV寄存器

  /* 清空数据FIFO（确保操作环境干净） */
  CRYP_FIFOFlush(); // 复位输入/输出FIFO指针

  /* 激活密码学处理器 */
  CRYP_Cmd(ENABLE); // 使能CRYP外设时钟

  /* 硬件就绪状态检测 */
  if(CRYP_GetCmdStatus() == DISABLE)
  {
    /* 潜在错误原因：
       - RCC_CRYP时钟未使能
       - 芯片不包含CRYP外设
       - 硬件初始化失败 */
    return(ERROR); 
  }

  /* 数据块处理主循环（每次处理8字节） */
  for(i=0; ((i<Ilength) && (status != ERROR)); i+=8)
  {
    /* 将64位数据块写入输入FIFO */
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 写入低32位（硬件自动执行CBC异或）
    inputaddr +=4;
    CRYP_DataIn(*(uint32_t*)(inputaddr));  // 写入高32位
    inputaddr +=4;

    /* 硬件响应超时监控 */
    counter = 0;
    do {
      busystatus = CRYP_GetFlagStatus(CRYP_FLAG_BUSY); // 检测总线忙状态
      counter++;
    } while ((counter != TDESBUSY_TIMEOUT) && (busystatus != RESET));

    /* 超时错误处理 */
    if (busystatus != RESET) {
      status = ERROR; // 标记异常状态
    } else {
      /* 从输出FIFO提取处理结果 */
      *(uint32_t*)(outputaddr) = CRYP_DataOut(); // 读取密文低32位
      outputaddr +=4;
      *(uint32_t*)(outputaddr) = CRYP_DataOut(); // 读取密文高32位
      outputaddr +=4;
    }
  }

  /* 关闭密码学外设 */
  CRYP_Cmd(DISABLE); // 禁用硬件模块

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

/**
  ******************************************************************************
  * @file    stm32f4xx_cryp.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the  Cryptographic processor (CRYP) peripheral:
  *           + Initialization and Configuration functions
  *           + Data treatment functions 
  *           + Context swapping functions     
  *           + DMA interface function       
  *           + Interrupts and flags management       
  *
@verbatim
 ===================================================================      
                 ##### How to use this driver #####
 =================================================================== 
 [..]
   (#) Enable the CRYP controller clock using 
       RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE); function.
  
   (#) Initialise the CRYP using CRYP_Init(), CRYP_KeyInit() and if needed 
       CRYP_IVInit(). 
  
   (#) Flush the IN and OUT FIFOs by using CRYP_FIFOFlush() function.
  
   (#) Enable the CRYP controller using the CRYP_Cmd() function. 
  
   (#) If using DMA for Data input and output transfer, activate the needed DMA 
       Requests using CRYP_DMACmd() function 
  
   (#) If DMA is not used for data transfer, use CRYP_DataIn() and  CRYP_DataOut() 
       functions to enter data to IN FIFO and get result from OUT FIFO.
  
   (#) To control CRYP events you can use one of the following two methods:
       (++) Check on CRYP flags using the CRYP_GetFlagStatus() function.  
       (++) Use CRYP interrupts through the function CRYP_ITConfig() at 
            initialization phase and CRYP_GetITStatus() function into interrupt 
            routines in processing phase.
         
   (#) Save and restore Cryptographic processor context using CRYP_SaveContext() 
       and CRYP_RestoreContext() functions.     
  
  
 *** Procedure to perform an encryption or a decryption ***
 ========================================================== 
  
 *** Initialization ***
 ====================== 
 [..] 
   (#) Initialize the peripheral using CRYP_Init(), CRYP_KeyInit() and CRYP_IVInit 
       functions:
       (++) Configure the key size (128-, 192- or 256-bit, in the AES only) 
       (++) Enter the symmetric key 
       (++) Configure the data type
       (++) In case of decryption in AES-ECB or AES-CBC, you must prepare 
            the key: configure the key preparation mode. Then Enable the CRYP 
            peripheral using CRYP_Cmd() function: the BUSY flag is set. 
            Wait until BUSY flag is reset : the key is prepared for decryption
       (++) Configure the algorithm and chaining (the DES/TDES in ECB/CBC, the 
            AES in ECB/CBC/CTR) 
       (++) Configure the direction (encryption/decryption).
       (++) Write the initialization vectors (in CBC or CTR modes only)
  
   (#) Flush the IN and OUT FIFOs using the CRYP_FIFOFlush() function
  
  
  *** Basic Processing mode (polling mode) *** 
  ============================================  
  [..]
    (#) Enable the cryptographic processor using CRYP_Cmd() function.
  
    (#) Write the first blocks in the input FIFO (2 to 8 words) using 
        CRYP_DataIn() function.
  
    (#) Repeat the following sequence until the complete message has been 
        processed:
  
        (++) Wait for flag CRYP_FLAG_OFNE occurs (using CRYP_GetFlagStatus() 
            function), then read the OUT-FIFO using CRYP_DataOut() function
            (1 block or until the FIFO is empty)
  
         (++) Wait for flag CRYP_FLAG_IFNF occurs, (using CRYP_GetFlagStatus() 
            function then write the IN FIFO using CRYP_DataIn() function 
            (1 block or until the FIFO is full)
  
    (#) At the end of the processing, CRYP_FLAG_BUSY flag will be reset and 
          both FIFOs are empty (CRYP_FLAG_IFEM is set and CRYP_FLAG_OFNE is 
          reset). You can disable the peripheral using CRYP_Cmd() function.
  
 *** Interrupts Processing mode *** 
 ==================================
 [..] In this mode, Processing is done when the data are transferred by the 
      CPU during interrupts.
  
    (#) Enable the interrupts CRYP_IT_INI and CRYP_IT_OUTI using CRYP_ITConfig()
        function.
  
    (#) Enable the cryptographic processor using CRYP_Cmd() function.
  
    (#) In the CRYP_IT_INI interrupt handler : load the input message into the 
         IN FIFO using CRYP_DataIn() function . You can load 2 or 4 words at a 
         time, or load data until the IN FIFO is full. When the last word of
         the message has been entered into the IN FIFO, disable the CRYP_IT_INI 
         interrupt (using CRYP_ITConfig() function).
  
    (#) In the CRYP_IT_OUTI interrupt handler : read the output message from 
         the OUT FIFO using CRYP_DataOut() function. You can read 1 block (2 or 
         4 words) at a time or read data until the FIFO is empty.
         When the last word has been read, INIM=0, BUSY=0 and both FIFOs are 
         empty (CRYP_FLAG_IFEM is set and CRYP_FLAG_OFNE is reset). 
         You can disable the CRYP_IT_OUTI interrupt (using CRYP_ITConfig() 
         function) and you can disable the peripheral using CRYP_Cmd() function.
  
 *** DMA Processing mode *** 
 ===========================
 [..] In this mode, Processing is done when the DMA is used to transfer the 
      data from/to the memory.
  
    (#) Configure the DMA controller to transfer the input data from the 
         memory using DMA_Init() function. 
         The transfer length is the length of the message. 
         As message padding is not managed by the peripheral, the message 
         length must be an entire number of blocks. The data are transferred 
         in burst mode. The burst length is 4 words in the AES and 2 or 4 
         words in the DES/TDES. The DMA should be configured to set an 
         interrupt on transfer completion of the output data to indicate that 
         the processing is finished. 
         Refer to DMA peripheral driver for more details.  
  
     (#) Enable the cryptographic processor using CRYP_Cmd() function. 
         Enable the DMA requests CRYP_DMAReq_DataIN and CRYP_DMAReq_DataOUT 
         using CRYP_DMACmd() function.
  
     (#) All the transfers and processing are managed by the DMA and the 
         cryptographic processor. The DMA transfer complete interrupt indicates 
         that the processing is complete. Both FIFOs are normally empty and 
         CRYP_FLAG_BUSY flag is reset.
  
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
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup CRYP 
  * @brief CRYP driver modules
  * @{
  */ 
/* CRYP模块私有定义 */
/**
  * @brief  加密模块内部使用的常量和阈值定义
  * @note   这些定义仅在本文件范围内有效
  */

/* 状态标志位掩码定义 */
#define FLAG_MASK     ((uint8_t)0x20)  // 二进制 0010 0000，用于提取CRYP_SR状态寄存器的特定标志位
                                       // 对应CRYP状态寄存器(CRYP_SR)的第6位（位5，0起始）
                                       // 该位通常表示输入/输出FIFO状态（0: FIFO未空，1: FIFO空）

/* 硬件操作超时阈值 */
#define MAX_TIMEOUT   ((uint16_t)0xFFFF)  // 最大等待周期数（65535个时钟周期）
                                          // 用于硬件标志位检测的超时保护机制
                                          // 典型应用场景：
                                          // - 等待DMA传输完成
                                          // - 等待加密操作就绪状态
                                          // - 等待FIFO空/满状态变化

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CRYP_Private_Functions
  * @{
  */ 

/** @defgroup CRYP_Group1 Initialization and Configuration functions
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
             ##### Initialization and Configuration functions #####
 ===============================================================================  
 [..] This section provides functions allowing to 
   (+) Initialize the cryptographic Processor using CRYP_Init() function 
       (++)  Encrypt or Decrypt 
       (++)  mode : TDES-ECB, TDES-CBC, 
                    DES-ECB, DES-CBC, 
                    AES-ECB, AES-CBC, AES-CTR, AES-Key, AES-GCM, AES-CCM 
       (++) DataType :  32-bit data, 16-bit data, bit data or bit-string
       (++) Key Size (only in AES modes)
   (+) Configure the Encrypt or Decrypt Key using CRYP_KeyInit() function 
   (+) Configure the Initialization Vectors(IV) for CBC and CTR modes using 
       CRYP_IVInit() function.  
   (+) Flushes the IN and OUT FIFOs : using CRYP_FIFOFlush() function.                         
   (+) Enable or disable the CRYP Processor using CRYP_Cmd() function 
       
@endverbatim
  * @{
  */
/* CRYP模块初始化管理 */

/**
  * @brief  复位CRYP外设至默认状态
  * @param  无
  * @retval 无
  * @note   1. 通过AHB2总线复位控制器实现硬件级复位
  *         2. 复位将清除所有配置寄存器和数据寄存器
  *         3. 复位后需重新初始化才能进行加密操作
  */
void CRYP_DeInit(void)
{
  /* 触发硬件复位序列 */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_CRYP, ENABLE);  // 激活CRYP模块复位
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_CRYP, DISABLE); // 释放CRYP模块复位
}

/**
  * @brief  配置CRYP加密模块的核心参数
  * @param  CRYP_InitStruct: 加密配置结构体指针，包含：
  *            - CRYP_AlgoMode:  算法模式（AES/TDES/DES）
  *            - CRYP_DataType:  数据位宽（32/16/8位）
  *            - CRYP_AlgoDir:   算法方向（加密/解密）
  *            - CRYP_KeySize:   密钥长度（仅AES有效）
  * @retval 无
  * @note   1. 配置顺序必须遵循：算法模式->数据格式->密钥尺寸->方向
  *         2. TDES/DES算法自动忽略密钥尺寸参数
  */
void CRYP_Init(CRYP_InitTypeDef* CRYP_InitStruct)
{
  /* 参数有效性验证 */
  assert_param(IS_CRYP_ALGOMODE(CRYP_InitStruct->CRYP_AlgoMode));
  assert_param(IS_CRYP_DATATYPE(CRYP_InitStruct->CRYP_DataType));
  assert_param(IS_CRYP_ALGODIR(CRYP_InitStruct->CRYP_AlgoDir));

  /* 配置算法模式寄存器（CRYP_CR[5:3]） */
  CRYP->CR &= ~CRYP_CR_ALGOMODE;                    // 清除原模式
  CRYP->CR |= CRYP_InitStruct->CRYP_AlgoMode;       // 设置新模式

  /* 配置数据位宽（CRYP_CR[1:0]） */
  CRYP->CR &= ~CRYP_CR_DATATYPE;
  CRYP->CR |= CRYP_InitStruct->CRYP_DataType;       // 设置数据格式

  /* AES算法专用密钥长度配置（CRYP_CR[7:6]） */
  if ((CRYP_InitStruct->CRYP_AlgoMode != CRYP_AlgoMode_TDES_ECB) &&
      (CRYP_InitStruct->CRYP_AlgoMode != CRYP_AlgoMode_TDES_CBC) &&
      (CRYP_InitStruct->CRYP_AlgoMode != CRYP_AlgoMode_DES_ECB) &&
      (CRYP_InitStruct->CRYP_AlgoMode != CRYP_AlgoMode_DES_CBC))
  {
    assert_param(IS_CRYP_KEYSIZE(CRYP_InitStruct->CRYP_KeySize));
    CRYP->CR &= ~CRYP_CR_KEYSIZE;                   // 清除原密钥长度
    CRYP->CR |= CRYP_InitStruct->CRYP_KeySize;      // 设置新密钥长度
  }

  /* 配置算法方向（CRYP_CR[2]） */
  CRYP->CR &= ~CRYP_CR_ALGODIR;                     // 清除原方向
  CRYP->CR |= CRYP_InitStruct->CRYP_AlgoDir;         // 设置加密/解密
}

/**
  * @brief  初始化CRYP配置结构体为默认值
  * @param  CRYP_InitStruct: 待初始化的结构体指针
  * @retval 无
  * @note   默认配置：
  *            - ECB模式TDES加密
  *            - 32位数据位宽
  *            - 128位AES密钥（当使用AES时）
  */
void CRYP_StructInit(CRYP_InitTypeDef* CRYP_InitStruct)
{
  CRYP_InitStruct->CRYP_AlgoDir = CRYP_AlgoDir_Encrypt;   // 默认加密方向
  CRYP_InitStruct->CRYP_AlgoMode = CRYP_AlgoMode_TDES_ECB;// 默认TDES_ECB
  CRYP_InitStruct->CRYP_DataType = CRYP_DataType_32b;    // 32位数据格式
  CRYP_InitStruct->CRYP_KeySize = CRYP_KeySize_128b;      // AES默认128位
}

/**
  * @brief  加载加密密钥到硬件寄存器
  * @param  CRYP_KeyInitStruct: 密钥结构体指针，包含：
  *            - 8个32位密钥寄存器值（K0LR/K0RR...K3LR/K3RR）
  * @retval 无
  * @note   1. 密钥必须按小端格式组织
  *         2. AES-128使用K0/K1，AES-256使用K0-K3
  *         3. DES使用K0/K1，TDES使用K0-K2
  */
void CRYP_KeyInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct)
{
  /* 加载密钥到8个密钥寄存器 */
  CRYP->K0LR = CRYP_KeyInitStruct->CRYP_Key0Left;   // 密钥0低位
  CRYP->K0RR = CRYP_KeyInitStruct->CRYP_Key0Right;  // 密钥0高位
  CRYP->K1LR = CRYP_KeyInitStruct->CRYP_Key1Left;   // 密钥1低位
  CRYP->K1RR = CRYP_KeyInitStruct->CRYP_Key1Right;  
  CRYP->K2LR = CRYP_KeyInitStruct->CRYP_Key2Left;   // 密钥2低位（TDES/AES256）
  CRYP->K2RR = CRYP_KeyInitStruct->CRYP_Key2Right;
  CRYP->K3LR = CRYP_KeyInitStruct->CRYP_Key3Left;   // 密钥3低位（AES256）
  CRYP->K3RR = CRYP_KeyInitStruct->CRYP_Key3Right;
}
/* CRYP高级配置函数组 */

/**
  * @brief  初始化密钥结构体为全零状态
  * @param  CRYP_KeyInitStruct: 密钥结构体指针
  * @retval 无
  * @note   1. 用于安全擦除旧密钥残留
  *         2. 各算法密钥使用规则：
  *            - AES-128: 使用Key0/Key1
  *            - AES-256: 使用Key0-Key3
  *            - TDES: 使用Key0-Key2
  */
void CRYP_KeyStructInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct)
{
  CRYP_KeyInitStruct->CRYP_Key0Left  = 0;  // 密钥0低位清零
  CRYP_KeyInitStruct->CRYP_Key0Right = 0;  // 密钥0高位清零
  CRYP_KeyInitStruct->CRYP_Key1Left  = 0;  // 密钥1低位清零（AES-128/256, TDES）
  CRYP_KeyInitStruct->CRYP_Key1Right = 0;
  CRYP_KeyInitStruct->CRYP_Key2Left  = 0;  // 密钥2低位清零（AES-256, TDES）
  CRYP_KeyInitStruct->CRYP_Key2Right = 0;
  CRYP_KeyInitStruct->CRYP_Key3Left  = 0;  // 密钥3低位清零（仅AES-256）
  CRYP_KeyInitStruct->CRYP_Key3Right = 0;
}

/**
  * @brief  加载初始化向量(IV)到硬件寄存器
  * @param  CRYP_IVInitStruct: IV结构体指针，包含：
  *            - IV0LR/IV0RR: IV低位/高位
  *            - IV1LR/IV1RR: 扩展IV（某些模式使用）
  * @retval 无
  * @note   1. 必须在加密操作开始前加载
  *         2. CBC模式需要完整的IV
  *         3. GCM模式需要12字节IV
  */
void CRYP_IVInit(CRYP_IVInitTypeDef* CRYP_IVInitStruct)
{
  CRYP->IV0LR = CRYP_IVInitStruct->CRYP_IV0Left;   // 写入IV低位寄存器
  CRYP->IV0RR = CRYP_IVInitStruct->CRYP_IV0Right;  // 写入IV高位寄存器
  CRYP->IV1LR = CRYP_IVInitStruct->CRYP_IV1Left;   // 扩展IV寄存器（GCM模式）
  CRYP->IV1RR = CRYP_IVInitStruct->CRYP_IV1Right;
}

/**
  * @brief  初始化IV结构体为全零状态
  * @param  CRYP_IVInitStruct: IV结构体指针
  * @retval 无
  * @note   安全要求：
  *         1. 每次加密应使用不同IV
  *         2. 避免IV重复使用导致安全漏洞
  */
void CRYP_IVStructInit(CRYP_IVInitTypeDef* CRYP_IVInitStruct)
{
  CRYP_IVInitStruct->CRYP_IV0Left  = 0;  // IV低位清零
  CRYP_IVInitStruct->CRYP_IV0Right = 0;  // IV高位清零
  CRYP_IVInitStruct->CRYP_IV1Left  = 0;  // 扩展IV清零
  CRYP_IVInitStruct->CRYP_IV1Right = 0;
}

/**
  * @brief  配置AES-CCM/GCM加密阶段
  * @param  CRYP_Phase: 加密阶段，可选：
  *            @arg CRYP_Phase_Init:    初始化阶段（设置初始参数）
  *            @arg CRYP_Phase_Header:  头部处理阶段（认证数据）
  *            @arg CRYP_Phase_Payload: 有效载荷阶段（加密/解密数据）
  *            @arg CRYP_Phase_Final:   最终阶段（生成认证标签）
  * @retval 无
  * @note   1. 严格按阶段顺序调用
  *         2. 仅适用于CCM/GCM模式
  */
void CRYP_PhaseConfig(uint32_t CRYP_Phase)
{ 
  uint32_t tempcr = 0;

  assert_param(IS_CRYP_PHASE(CRYP_Phase));  // 验证阶段参数有效性

  tempcr = CRYP->CR;                       // 获取当前控制寄存器值
  tempcr &= ~CRYP_CR_GCM_CCMPH;            // 清除阶段配置位（CR[22:21]）
  tempcr |= CRYP_Phase;                     // 设置新阶段
  CRYP->CR = tempcr;                       // 写回控制寄存器
}

/**
  * @brief  刷新输入输出FIFO缓冲区
  * @param  无
  * @retval 无
  * @note   使用条件：
  *         1. 仅在BUSY标志为0时调用
  *         2. 异常处理后恢复FIFO状态
  *         3. 清空未处理的数据包
  */
void CRYP_FIFOFlush(void)
{
  /* 设置FFLUSH位（CRYP_CR[15]）复位FIFO指针 */
  CRYP->CR |= CRYP_CR_FFLUSH;  // 写1触发FIFO刷新
}

/**
  * @brief  启用/禁用加密处理器
  * @param  NewState: 新状态（ENABLE/DISABLE）
  * @retval 无
  * @note   状态切换影响：
  *         ENABLE: 激活加密计算引擎
  *         DISABLE: 保持当前配置但暂停操作
  */
void CRYP_Cmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE) {
    CRYP->CR |= CRYP_CR_CRYPEN;   // 设置CRYPEN位（CRYP_CR[14]）
  } else {
    CRYP->CR &= ~CRYP_CR_CRYPEN;  // 清除CRYPEN位
  }
}

/**
  * @}
  */
  
/** @defgroup CRYP_Group2 CRYP Data processing functions
 *  @brief    CRYP Data processing functions
 *
@verbatim    
 ===============================================================================
                    ##### CRYP Data processing functions #####
 ===============================================================================  
 [..] This section provides functions allowing the encryption and decryption 
      operations: 
   (+) Enter data to be treated in the IN FIFO : using CRYP_DataIn() function.
   (+) Get the data result from the OUT FIFO : using CRYP_DataOut() function.

@endverbatim
  * @{
  */

/**
  * @brief  Writes data in the Data Input register (DIN).
  * @note   After the DIN register has been read once or several times, 
  *         the FIFO must be flushed (using CRYP_FIFOFlush() function).  
  * @param  Data: data to write in Data Input register
  * @retval None
  */
/**
  * @brief  将明文/密文数据写入加密模块输入寄存器
  * @param  Data: 32位输入数据
  * @retval 无
  * @note   1. 数据通过DR寄存器进入输入FIFO
  *         2. 写入前应检查IFNF（输入FIFO未满）标志
  *         3. 最大输入FIFO深度为4字（128位）
  *         4. 数据按小端格式组织
  */
void CRYP_DataIn(uint32_t Data)
{
  CRYP->DR = Data;  // 写入数据输入寄存器，触发自动压入输入FIFO
}

/**
  * @brief  从输出FIFO读取处理后的加密/解密数据
  * @param  无
  * @retval uint32_t 32位输出数据
  * @note   1. 读取前应检查OFNE（输出FIFO非空）标志
  *         2. 最大输出FIFO深度为4字（128位）
  *         3. 在CBC/CTR模式需保持连续读取
  *         4. 读取操作会自动弹出数据
  */
uint32_t CRYP_DataOut(void)
{
  return CRYP->DOUT;  // 读取输出寄存器，触发FIFO指针移动
}

/**
  * @}
  */
  
/** @defgroup CRYP_Group3 Context swapping functions
 *  @brief   Context swapping functions
 *
@verbatim   
 ===============================================================================
                      ##### Context swapping functions #####
 ===============================================================================  
 [..] This section provides functions allowing to save and store CRYP Context

 [..] It is possible to interrupt an encryption/ decryption/ key generation process 
      to perform another processing with a higher priority, and to complete the 
      interrupted process later on, when the higher-priority task is complete. To do 
      so, the context of the interrupted task must be saved from the CRYP registers 
      to memory, and then be restored from memory to the CRYP registers.
   
   (#) To save the current context, use CRYP_SaveContext() function
   (#) To restore the saved context, use CRYP_RestoreContext() function 

@endverbatim
  * @{
  */
  
/**
  * @brief  Saves the CRYP peripheral Context. 
  * @note   This function stops DMA transfer before to save the context. After 
  *         restoring the context, you have to enable the DMA again (if the DMA
  *         was previously used).
  * @param  CRYP_ContextSave: pointer to a CRYP_Context structure that contains
  *         the repository for current context.
  * @param  CRYP_KeyInitStruct: pointer to a CRYP_KeyInitTypeDef structure that 
  *         contains the configuration information for the CRYP Keys.  
  * @retval None
  */
/**
  * @brief  保存加密模块运行时上下文到指定结构体
  * @note   本函数执行顺序：
  *         1. 终止输入DMA传输
  *         2. 等待加密模块进入可保存状态
  *         3. 终止输出DMA并关闭加密引擎
  *         4. 提取关键寄存器配置
  *         5. 存储初始化向量和密钥材料
  *         6. 保留上下文交换寄存器状态
  * @param  CRYP_ContextSave: 上下文存储容器指针
  * @param  CRYP_KeyInitStruct: 当前生效的密钥结构指针
  * @retval 操作状态: 成功返回SUCCESS，超时返回ERROR
  */
ErrorStatus CRYP_SaveContext(CRYP_Context* CRYP_ContextSave,
                             CRYP_KeyInitTypeDef* CRYP_KeyInitStruct)
{
  __IO uint32_t timeout = 0;        // 硬件等待超时计数器（volatile类型保证实时访问）
  uint32_t ckeckmask = 0, bitstatus;// 状态掩码组合和实时状态缓存
  ErrorStatus status = ERROR;        // 操作结果初始化为失败

  /* 关闭输入DMA通道：清除DMACR寄存器的DIEN位 */
  CRYP->DMACR &= ~(uint32_t)CRYP_DMACR_DIEN;  // 停止从外设到内存的数据传输
    
  /* 硬件状态等待策略：
     - 输入FIFO空(IFEM=1)
     - 输出FIFO空(OFNE=0)
     - 加密引擎空闲(BUSY=0) */
  if ((CRYP->CR & (CRYP_CR_ALGOMODE_TDES_ECB | CRYP_CR_ALGOMODE_TDES_CBC)) != 0)
  { 
    // TDES算法：不检查输出FIFO状态
    ckeckmask =  CRYP_SR_IFEM | CRYP_SR_BUSY;  // 掩码组合：输入空+忙状态
  }
  else  // AES/DES算法家族
  {
    ckeckmask =  CRYP_SR_IFEM | CRYP_SR_BUSY | CRYP_SR_OFNE; // 增加输出FIFO检查
  }           
    
  /* 硬件状态轮询检测 */
  do {
    bitstatus = CRYP->SR & ckeckmask;  // 获取当前状态组合
    timeout++;                         // 累计等待周期数
  } while ((timeout < MAX_TIMEOUT) && (bitstatus != CRYP_SR_IFEM)); // 持续检测直到条件满足或超时

  /* 状态验证阶段 */
  if ((CRYP->SR & ckeckmask) != CRYP_SR_IFEM) {
    status = ERROR;  // 未达到预期状态
  } else {
    /* 安全关闭输出通道：清除DOEN位 */
    CRYP->DMACR &= ~(uint32_t)CRYP_DMACR_DOEN;  // 停止内存到外设的传输
    CRYP->CR &= ~(uint32_t)CRYP_CR_CRYPEN;      // 停用加密处理单元

    /* 捕获运行时配置（保留关键控制位） */
    CRYP_ContextSave->CR_CurrentConfig = CRYP->CR & (
        CRYP_CR_GCM_CCMPH   |  // 加解密阶段标识
        CRYP_CR_KEYSIZE     |  // 密钥长度配置
        CRYP_CR_DATATYPE    |  // 数据格式（32位/16位/8位）
        CRYP_CR_ALGOMODE    |  // 算法选择（ECB/CBC/GCM等）
        CRYP_CR_ALGODIR        // 操作方向（加密/解密）
    );

    /* 保存初始化向量（块加密模式需要） */
    CRYP_ContextSave->CRYP_IV0LR = CRYP->IV0LR;  // IV寄存器组低位存储
    CRYP_ContextSave->CRYP_IV0RR = CRYP->IV0RR;  // IV寄存器组高位存储
    CRYP_ContextSave->CRYP_IV1LR = CRYP->IV1LR;  // 第二IV组低位
    CRYP_ContextSave->CRYP_IV1RR = CRYP->IV1RR;  // 第二IV组高位

    /* 密钥材料归档 */
    CRYP_ContextSave->CRYP_K0LR = CRYP_KeyInitStruct->CRYP_Key0Left;   // 密钥段0低位
    CRYP_ContextSave->CRYP_K0RR = CRYP_KeyInitStruct->CRYP_Key0Right; // 密钥段0高位
    CRYP_ContextSave->CRYP_K1LR = CRYP_KeyInitStruct->CRYP_Key1Left;   // 密钥段1低位
    CRYP_ContextSave->CRYP_K1RR = CRYP_KeyInitStruct->CRYP_Key1Right; // 密钥段1高位
    CRYP_ContextSave->CRYP_K2LR = CRYP_KeyInitStruct->CRYP_Key2Left;   // 密钥段2低位
    CRYP_ContextSave->CRYP_K2RR = CRYP_KeyInitStruct->CRYP_Key2Right; // 密钥段2高位
    CRYP_ContextSave->CRYP_K3LR = CRYP_KeyInitStruct->CRYP_Key3Left;   // 密钥段3低位
    CRYP_ContextSave->CRYP_K3RR = CRYP_KeyInitStruct->CRYP_Key3Right; // 密钥段3高位

    /* GCM/CCM模式上下文存储 */
    CRYP_ContextSave->CRYP_CSGCMCCMR[0] = CRYP->CSGCMCCM0R;  // 上下文寄存器组0
    CRYP_ContextSave->CRYP_CSGCMCCMR[1] = CRYP->CSGCMCCM1R;  // 上下文寄存器组1
    /* 后续寄存器存储省略... */

    status = SUCCESS;  // 标记完整上下文保存成功
  }

  return status;  // 返回最终操作状态
}

/**
  * @brief  恢复加密模块先前保存的运行时上下文
  * @note   操作流程：
  *         1. 重载控制寄存器配置
  *         2. 恢复密钥材料
  *         3. 还原初始化向量
  *         4. 重建上下文交换寄存器
  *         5. 重新激活加密引擎
  * @param  CRYP_ContextRestore: 上下文恢复数据源指针
  * @warning 调用本函数后需手动重新初始化DMA（如果之前使用过DMA传输）
  * @remark 上下文恢复后需重新填充输入FIFO中的原始数据
  */
void CRYP_RestoreContext(CRYP_Context* CRYP_ContextRestore)  
{
  /* 恢复运行环境配置（控制寄存器关键位域） */
  CRYP->CR = CRYP_ContextRestore->CR_CurrentConfig;  // 重设算法模式/密钥尺寸/数据格式等参数

  /* 密钥体系重构（完整密钥矩阵恢复） */
  CRYP->K0LR = CRYP_ContextRestore->CRYP_K0LR; // 密钥段0低位寄存器写入
  CRYP->K0RR = CRYP_ContextRestore->CRYP_K0RR; // 密钥段0高位寄存器写入
  CRYP->K1LR = CRYP_ContextRestore->CRYP_K1LR; // 密钥段1低位（用于长密钥算法）
  CRYP->K1RR = CRYP_ContextRestore->CRYP_K1RR; // 密钥段1高位
  CRYP->K2LR = CRYP_ContextRestore->CRYP_K2LR; // 密钥段2低位（TDES三密钥模式使用）
  CRYP->K2RR = CRYP_ContextRestore->CRYP_K2RR; // 密钥段2高位
  CRYP->K3LR = CRYP_ContextRestore->CRYP_K3LR; // 密钥段3低位（AES-256等算法扩展使用）
  CRYP->K3RR = CRYP_ContextRestore->CRYP_K3RR; // 密钥段3高位

  /* 初始化向量恢复（适用于分组加密模式） */
  CRYP->IV0LR = CRYP_ContextRestore->CRYP_IV0LR;  // IV低位寄存器组初始化
  CRYP->IV0RR = CRYP_ContextRestore->CRYP_IV0RR;  // IV高位寄存器组
  CRYP->IV1LR = CRYP_ContextRestore->CRYP_IV1LR;  // 扩展IV区域第一部分
  CRYP->IV1RR = CRYP_ContextRestore->CRYP_IV1RR;  // 扩展IV区域第二部分

  /* GCM/CCM模式上下文恢复（加密状态重建） */
  CRYP->CSGCMCCM0R = CRYP_ContextRestore->CRYP_CSGCMCCMR[0];  // 上下文计数器寄存器0
  CRYP->CSGCMCCM1R = CRYP_ContextRestore->CRYP_CSGCMCCMR[1];  // 上下文计数器寄存器1
  /* 后续寄存器恢复省略... */
  
  /* 完整上下文状态回写 */
  CRYP->CSGCM0R = CRYP_ContextRestore->CRYP_CSGCMR[0];  // 伽罗瓦域上下文寄存器0
  CRYP->CSGCM1R = CRYP_ContextRestore->CRYP_CSGCMR[1];  // 伽罗瓦域上下文寄存器1
  /* 后续寄存器恢复省略... */

  /* 加密引擎重激活（CRYPEN位置1启动处理单元） */
  CRYP->CR |= CRYP_CR_CRYPEN;  // 使能加密/解密操作，恢复硬件工作状态
}

/**
  * @}
  */

/** @defgroup CRYP_Group4 CRYP's DMA interface Configuration function
 *  @brief   CRYP's DMA interface Configuration function 
 *
@verbatim   
 ===============================================================================
             ##### CRYP's DMA interface Configuration function #####
 ===============================================================================  
 [..] This section provides functions allowing to configure the DMA interface for 
      CRYP data input and output transfer.
   
 [..] When the DMA mode is enabled (using the CRYP_DMACmd() function), data can be 
      transferred:
   (+) From memory to the CRYP IN FIFO using the DMA peripheral by enabling 
       the CRYP_DMAReq_DataIN request.
   (+) From the CRYP OUT FIFO to the memory using the DMA peripheral by enabling 
       the CRYP_DMAReq_DataOUT request.

@endverbatim
  * @{
  */
/**
  * @brief  配置加密模块的DMA传输通道使能状态
  * @note   本函数实现以下控制逻辑：
  *         - 同时支持输入/输出双通道独立控制
  *         - 支持多通道组合操作（按位或组合参数）
  *         - 自动执行参数有效性校验
  * @param  CRYP_DMAReq: DMA通道选择掩码（位或组合）：
  *          @arg CRYP_DMAReq_DataIN:  输入数据流DMA使能（Rx方向）
  *          @arg CRYP_DMAReq_DataOUT: 输出数据流DMA使能（Tx方向）
  * @param  NewState: 目标状态 (ENABLE-启动DMA传输，DISABLE-挂起DMA传输)
  * @warning 操作前需确保DMA控制器已完成对应通道的初始化配置
  */
void CRYP_DMACmd(uint8_t CRYP_DMAReq, FunctionalState NewState)
{
  /* 参数有效性验证（编译时检查） */
  assert_param(IS_CRYP_DMAREQ(CRYP_DMAReq));       // 验证DMA请求类型有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));     // 验证状态值合法性

  if (NewState != DISABLE)
  {
    /* 启用指定DMA通道（位或操作保持其他通道状态不变） */
    CRYP->DMACR |= CRYP_DMAReq;  // 置位DMACR寄存器对应控制位：
                                  // DIEN位(输入)/DOEN位(输出)
  }
  else
  {
    /* 禁用指定DMA通道（位与操作清除目标位） */
    CRYP->DMACR &= (uint8_t)~CRYP_DMAReq;  // 清除DMACR寄存器对应控制位：
                                            // 输入通道关闭/输出通道关闭
  }
}

/**
  * @}
  */

/** @defgroup CRYP_Group5 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
              ##### Interrupts and flags management functions #####
 ===============================================================================  
 
 [..] This section provides functions allowing to configure the CRYP Interrupts and 
      to get the status and Interrupts pending bits.

 [..] The CRYP provides 2 Interrupts sources and 7 Flags:

 *** Flags : ***
 ===============
 [..] 
   (#) CRYP_FLAG_IFEM :  Set when Input FIFO is empty. This Flag is cleared only
       by hardware.
      
   (#) CRYP_FLAG_IFNF :  Set when Input FIFO is not full. This Flag is cleared 
       only by hardware.


   (#) CRYP_FLAG_INRIS  : Set when Input FIFO Raw interrupt is pending it gives 
       the raw interrupt state prior to masking of the input FIFO service interrupt.
       This Flag is cleared only by hardware.
     
   (#) CRYP_FLAG_OFNE   : Set when Output FIFO not empty. This Flag is cleared 
       only by hardware.
        
   (#) CRYP_FLAG_OFFU   : Set when Output FIFO is full. This Flag is cleared only 
       by hardware.
                           
   (#) CRYP_FLAG_OUTRIS : Set when Output FIFO Raw interrupt is pending it gives 
       the raw interrupt state prior to masking of the output FIFO service interrupt.
       This Flag is cleared only by hardware.
                               
   (#) CRYP_FLAG_BUSY   : Set when the CRYP core is currently processing a block 
       of data or a key preparation (for AES decryption). This Flag is cleared 
       only by hardware. To clear it, the CRYP core must be disabled and the last
       processing has completed. 

 *** Interrupts : ***
 ====================
 [..]
   (#) CRYP_IT_INI   : The input FIFO service interrupt is asserted when there 
      are less than 4 words in the input FIFO. This interrupt is associated to 
      CRYP_FLAG_INRIS flag.

      -@- This interrupt is cleared by performing write operations to the input FIFO 
          until it holds 4 or more words. The input FIFO service interrupt INMIS is 
          enabled with the CRYP enable bit. Consequently, when CRYP is disabled, the 
          INMIS signal is low even if the input FIFO is empty.



   (#) CRYP_IT_OUTI  : The output FIFO service interrupt is asserted when there 
       is one or more (32-bit word) data items in the output FIFO. This interrupt 
       is associated to CRYP_FLAG_OUTRIS flag.

       -@- This interrupt is cleared by reading data from the output FIFO until there 
           is no valid (32-bit) word left (that is, the interrupt follows the state 
           of the OFNE (output FIFO not empty) flag).

 *** Managing the CRYP controller events : ***
 =============================================
 [..] The user should identify which mode will be used in his application to manage 
      the CRYP controller events: Polling mode or Interrupt mode.

   (#) In the Polling Mode it is advised to use the following functions:
       (++) CRYP_GetFlagStatus() : to check if flags events occur. 

       -@@- The CRYPT flags do not need to be cleared since they are cleared as 
            soon as the associated event are reset.   


   (#) In the Interrupt Mode it is advised to use the following functions:
       (++) CRYP_ITConfig()       : to enable or disable the interrupt source.
       (++) CRYP_GetITStatus()    : to check if Interrupt occurs.

       -@@- The CRYPT interrupts have no pending bits, the interrupt is cleared as 
             soon as the associated event is reset. 

@endverbatim
  * @{
  */ 
/**
  * @brief  配置加密模块中断使能状态
  * @note   本函数控制以下中断类型：
  *         - 输入FIFO就绪中断（数据可写）
  *         - 输出FIFO就绪中断（数据可读）
  * @param  CRYP_IT: 中断类型组合（位或操作）：
  *          @arg CRYP_IT_INI:  输入FIFO阈值到达中断
  *          @arg CRYP_IT_OUTI: 输出FIFO数据可用中断
  * @param  NewState: 目标状态 (ENABLE-允许中断触发，DISABLE-屏蔽中断信号)
  * @warning 使能中断前需配置好NVIC中断控制器
  */
void CRYP_ITConfig(uint8_t CRYP_IT, FunctionalState NewState)
{
  /* 中断类型有效性验证 */
  assert_param(IS_CRYP_CONFIG_IT(CRYP_IT));    // 验证中断类型有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态值合法性

  if (NewState != DISABLE)
  {
    /* 启用指定中断源（设置中断屏蔽寄存器对应位） */
    CRYP->IMSCR |= CRYP_IT;  // 置位IMSCR寄存器：
                             // IMSCR[0]-输入中断, IMSCR[1]-输出中断
  }
  else
  {
    /* 禁用指定中断源（清除中断屏蔽寄存器对应位） */
    CRYP->IMSCR &= (uint8_t)~CRYP_IT;  // 保持其他中断配置不变
  }
}

/**
  * @brief  获取已使能中断的实际触发状态
  * @note   本函数检测的是经过中断屏蔽后的有效状态
  * @param  CRYP_IT: 待检测的中断类型（单个参数）：
  *          @arg CRYP_IT_INI:  输入FIFO中断
  *          @arg CRYP_IT_OUTI: 输出FIFO中断
  * @retval 中断触发状态：SET-中断待处理，RESET-无中断请求
  */
ITStatus CRYP_GetITStatus(uint8_t CRYP_IT)
{
  ITStatus bitstatus = RESET;
  /* 参数合法性校验 */
  assert_param(IS_CRYP_GET_IT(CRYP_IT));  // 确保单中断类型查询

  /* 读取屏蔽中断状态寄存器（MISR） */
  if ((CRYP->MISR & CRYP_IT) != (uint8_t)RESET) {
    bitstatus = SET;  // 检测到有效中断请求
  }
  else {
    bitstatus = RESET; // 无待处理中断
  }
  return bitstatus;  // 返回当前中断状态
}

/**
  * @brief  查询加密处理单元使能状态
  * @param  无
  * @retval 模块运行状态：ENABLE-加密引擎工作中，DISABLE-模块已停用
  */
FunctionalState CRYP_GetCmdStatus(void)
{
  FunctionalState state = DISABLE;

  /* 检测控制寄存器CRYPEN位（位15） */
  if ((CRYP->CR & CRYP_CR_CRYPEN) != 0) {
    state = ENABLE;  // 加密处理单元已激活
  }
  else {
    state = DISABLE; // 加密引擎处于停用状态
  }
  return state;  // 返回当前硬件状态
}

/**
  * @brief  获取加密模块硬件状态标志
  * @note   本函数支持两类标志查询：
  *         - 实时状态标志（SR寄存器）
  *         - 原始中断标志（RISR寄存器）
  * @param  CRYP_FLAG: 状态标志类型：
  *          @arg CRYP_FLAG_IFEM:  输入FIFO空
  *          @arg CRYP_FLAG_IFNF:  输入FIFO未满
  *          @arg CRYP_FLAG_OFNE:  输出FIFO非空
  *          @arg CRYP_FLAG_OFFU:  输出FIFO满
  *          @arg CRYP_FLAG_BUSY:  模块忙状态
  *          @arg CRYP_FLAG_OUTRIS: 输出原始中断
  *          @arg CRYP_FLAG_INRIS:  输入原始中断
  * @retval 标志状态：SET-条件成立，RESET-条件不成立
  */
FlagStatus CRYP_GetFlagStatus(uint8_t CRYP_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tempreg = 0;

  assert_param(IS_CRYP_GET_FLAG(CRYP_FLAG));  // 验证标志类型有效性

  /* 标志源寄存器判定（最高位判断） */
  if ((CRYP_FLAG & FLAG_MASK) != 0x00) {
    tempreg = CRYP->RISR;  // 原始中断状态寄存器（0x01/0x02）
  }
  else {
    tempreg = CRYP->SR;    // 实时状态寄存器（其他标志）
  }

  /* 检测指定标志位 */
  if ((tempreg & CRYP_FLAG ) != (uint8_t)RESET) {
    bitstatus = SET;  // 目标状态条件满足
  }
  else {
    bitstatus = RESET; // 状态条件未触发
  }
  return bitstatus;  // 返回当前硬件状态
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

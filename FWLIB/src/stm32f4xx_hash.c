/**
  ******************************************************************************
  * @file    stm32f4xx_hash.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HASH / HMAC Processor (HASH) peripheral:           
  *           - Initialization and Configuration functions
  *           - Message Digest generation functions
  *           - context swapping functions   
  *           - DMA interface function       
  *           - Interrupts and flags management       
  *         
@verbatim
 ===================================================================      
                 ##### How to use this driver #####
 ===================================================================
            
 *** HASH operation : *** 
 ========================                 
 [..]
   (#) Enable the HASH controller clock using 
       RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, ENABLE) function.
             
   (#) Initialise the HASH using HASH_Init() function. 
                 
   (#) Reset the HASH processor core, so that the HASH will be ready 
       to compute he message digest of a new message by using HASH_Reset() function.
  
   (#) Enable the HASH controller using the HASH_Cmd() function. 
                  
   (#) if using DMA for Data input transfer, Activate the DMA Request 
       using HASH_DMACmd() function 
                      
   (#) if DMA is not used for data transfer, use HASH_DataIn() function 
       to enter data to IN FIFO.
               
            
   (#) Configure the Number of valid bits in last word of the message 
       using HASH_SetLastWordValidBitsNbr() function.
               
   (#) if the message length is not an exact multiple of 512 bits, 
       then the function HASH_StartDigest() must be called to launch the computation
       of the final digest.     
               
   (#) Once computed, the digest can be read using HASH_GetDigest() function.         
                     
   (#) To control HASH events you can use one of the following wo methods:
       (++) Check on HASH flags using the HASH_GetFlagStatus() function.  
       (++) Use HASH interrupts through the function HASH_ITConfig() at 
            initialization phase and HASH_GetITStatus() function into 
            interrupt routines in hashing phase.
            After checking on a flag you should clear it using HASH_ClearFlag()
            function. And after checking on an interrupt event you should 
            clear it using HASH_ClearITPendingBit() function.     
                       
   (#) Save and restore hash processor context using 
       HASH_SaveContext() and HASH_RestoreContext() functions.     
                
  
              
 *** HMAC operation : *** 
 ========================
 [..] The HMAC algorithm is used for message authentication, by 
      irreversibly binding the message being processed to a key chosen 
      by the user. 
      For HMAC specifications, refer to "HMAC: keyed-hashing for message 
      authentication, H. Krawczyk, M. Bellare, R. Canetti, February 1997"
            
 [..] Basically, the HMAC algorithm consists of two nested hash operations:
      HMAC(message) = Hash[((key | pad) XOR 0x5C) | Hash(((key | pad) XOR 0x36) | message)]
      where:
      (+) "pad" is a sequence of zeroes needed to extend the key to the 
          length of the underlying hash function data block (that is 
          512 bits for both the SHA-1 and MD5 hash algorithms)
      (+) "|"   represents the concatenation operator 
            
           
 [..]To compute the HMAC, four different phases are required:                  
   (#) Initialise the HASH using HASH_Init() function to do HMAC 
       operation. 
                  
   (#) The key (to be used for the inner hash function) is then given to the core. 
       This operation follows the same mechanism as the one used to send the 
       message in the hash operation (that is, by HASH_DataIn() function and, 
       finally, HASH_StartDigest() function.
            
   (#) Once the last word has been entered and computation has started, 
       the hash processor elaborates the key. It is then ready to accept the message
       text using the same mechanism as the one used to send the message in the
       hash operation.
         
   (#) After the first hash round, the hash processor returns "ready" to indicate 
       that it is ready to receive the key to be used for the outer hash function 
       (normally, this key is the same as the one used for the inner hash function). 
       When the last word of the key is entered and computation starts, the HMAC 
       result is made available using HASH_GetDigest() function.

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
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup HASH 
  * @brief HASH driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/ 

/** @defgroup HASH_Private_Functions
  * @{
  */ 

/** @defgroup HASH_Group1 Initialization and Configuration functions
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and Configuration functions #####
 ===============================================================================  
 [..] This section provides functions allowing to 
   (+) Initialize the HASH peripheral
   (+) Configure the HASH Processor 
   (+) MD5/SHA1, 
   (+) HASH/HMAC, 
   (+) datatype 
   (+) HMAC Key (if mode = HMAC)
   (+) Reset the HASH Processor 
   
@endverbatim
  * @{
  */
  /**
  * @brief  HASH外设去初始化函数
  * @功能   复位HASH外设寄存器到默认值，完成硬件复位操作
  * @细节   通过RCC时钟控制单元先复位后释放的方式实现硬件级复位
  *         - 使用AHB2总线复位控制
  *         - 保持寄存器恢复默认状态
  * @param  None
  * @retval None
  */
void HASH_DeInit(void)
{
  /* 使能HASH外设复位（触发硬件复位信号） */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_HASH, ENABLE);
  
  /* 关闭HASH外设复位（结束复位状态，恢复正常工作） */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_HASH, DISABLE);
}

/**
  * @brief  HASH外设初始化函数
  * @功能   根据初始化结构体配置HASH工作参数
  * @细节   包含算法选择、数据格式、操作模式等核心配置
  *         - 自动执行处理器复位操作
  *         - 支持HMAC模式密钥类型配置
  * @注意   
  *         - 调用本函数将自动重置哈希处理器，无需额外调用HASH_Reset()
  *         - HMAC模式下必须配置HASH_HMACKeyType字段
  * @param  HASH_InitStruct: 初始化配置结构体指针，包含：
  *         - HASH_AlgoSelection: 算法选择(SHA1/MD5等)
  *         - HASH_DataType: 输入数据格式(8/16/32位)
  *         - HASH_AlgoMode: 算法模式(HASH/HMAC)
  *         - HASH_HMACKeyType: HMAC密钥类型（仅在HMAC模式需要）
  * @retval None
  */
void HASH_Init(HASH_InitTypeDef* HASH_InitStruct)
{
  /* 参数合法性检查 */
  assert_param(IS_HASH_ALGOSELECTION(HASH_InitStruct->HASH_AlgoSelection)); // 验证算法选择有效性
  assert_param(IS_HASH_DATATYPE(HASH_InitStruct->HASH_DataType));           // 验证数据类型有效性
  assert_param(IS_HASH_ALGOMODE(HASH_InitStruct->HASH_AlgoMode));           // 验证算法模式有效性
  
  /* 清除并重新配置控制寄存器（CR）的关键位 */
  HASH->CR &= ~ (HASH_CR_ALGO | HASH_CR_DATATYPE | HASH_CR_MODE);  // 清除算法、数据类型、模式位
               // 设置算法选择位
                                 // 设置数据类型位
  HASH->CR |= (HASH_InitStruct->HASH_AlgoSelection | \
               HASH_InitStruct->HASH_DataType | \
               HASH_InitStruct->HASH_AlgoMode);                    // 设置算法模式位
  
  /* HMAC模式特殊处理 */
  if(HASH_InitStruct->HASH_AlgoMode == HASH_AlgoMode_HMAC) 
  {
    /* HMAC密钥类型配置 */
    assert_param(IS_HASH_HMAC_KEYTYPE(HASH_InitStruct->HASH_HMACKeyType)); // 验证密钥类型有效性
    HASH->CR &= ~HASH_CR_LKEY;                // 清除长密钥模式位
    HASH->CR |= HASH_InitStruct->HASH_HMACKeyType; // 设置密钥类型（短密钥/长密钥）
  }

  /* 初始化哈希处理器核心 */
  HASH->CR |= HASH_CR_INIT;  // 设置INIT位触发以下操作：
                             // - 清除哈希上下文寄存器
                             // - 准备接收新消息摘要
                             // - 保持当前配置参数不变
}
/**
  * @brief  HASH初始化结构体默认值加载函数
  * @功能   预置HASH配置参数的默认值集合，为结构体成员提供安全初始状态
  * @细节   包含四个核心参数的默认配置：
  *         - SHA1算法（兼容性最佳）
  *         - 标准HASH处理模式
  *         - 32位数据输入格式
  *         - HMAC短密钥类型（128位）
  * @注意   适用于快速初始化场景，建议在动态配置参数前首先调用本函数
  * @param  HASH_InitStruct: 待初始化的HASH配置结构体指针
  * @retval None
  */
void HASH_StructInit(HASH_InitTypeDef* HASH_InitStruct)
{
  /* 设置默认加密算法：SHA1（安全哈希算法第1代） */
  HASH_InitStruct->HASH_AlgoSelection = HASH_AlgoSelection_SHA1;

  /* 配置默认工作模式：标准哈希处理模式（非HMAC） */
  HASH_InitStruct->HASH_AlgoMode = HASH_AlgoMode_HASH;

  /* 定义默认数据位宽：32位字对齐（最优处理宽度） */
  HASH_InitStruct->HASH_DataType = HASH_DataType_32b;

  /* 初始化HMAC密钥类型：短密钥（128位，适用于大多数应用场景） */
  HASH_InitStruct->HASH_HMACKeyType = HASH_HMACKeyType_ShortKey;
}

/**
  * @brief  HASH处理器核心复位函数
  * @功能   执行硬件级复位操作，清空当前计算上下文
  * @细节   主要完成以下操作：
  *         - 清除所有中间计算结果寄存器
  *         - 复位数据输入计数器
  *         - 清除完成中断标志（HASH_SR_DCIS）
  * @注意   调用后处理器进入就绪状态，可立即开始新数据流的处理
  *         该操作会触发以下状态变化：
  *         - HASH_IT_DCI 中断状态清除
  *         - HASH_FLAG_DCIS 标志位清除
  * @param  None
  * @retval None
  */
void HASH_Reset(void)
{
  /* 通过设置控制寄存器INIT位触发硬件复位序列 
     (该操作具有自动清除机制，无需手动复位该标志位) */
  HASH->CR |= HASH_CR_INIT;  // 写入1触发复位操作：
                             // - 重置内部状态机
                             // - 清空输入缓冲区
                             // - 保留当前配置参数不变
}

/**
  * @}
  */
 
/** @defgroup HASH_Group2 Message Digest generation functions
 *  @brief    Message Digest generation functions
 *
@verbatim    
 ===============================================================================
                  ##### Message Digest generation functions #####
 ===============================================================================  
 [..] This section provides functions allowing the generation of message digest: 
   (+) Push data in the IN FIFO : using HASH_DataIn()
   (+) Get the number of words set in IN FIFO, use HASH_GetInFIFOWordsNbr()  
   (+) set the last word valid bits number using HASH_SetLastWordValidBitsNbr() 
   (+) start digest calculation : using HASH_StartDigest()
   (+) Get the Digest message : using HASH_GetDigest()
 
@endverbatim
  * @{
  */

/**
  * @brief  末端数据有效位设置函数
  * @功能   配置消息最后一个字中有效的比特位数
  * @细节   控制信息填充阶段的位填充规则
  * @注意   必须在启动哈希计算前设置（预处理阶段）
  * @param  ValidNumber: 有效比特数（0-31）
  *           - 0x00: 32位全有效（完整字）
  *           - 0x1F: 仅最低31位有效（高位1bit无效）
  * @retval None
  */
void HASH_SetLastWordValidBitsNbr(uint16_t ValidNumber)
{
  /* 验证参数有效性（0 <= ValidNumber <= 31） */
  assert_param(IS_HASH_VALIDBITSNUMBER(ValidNumber));
  
  /* 配置STR寄存器的NBW位域 */
  HASH->STR &= ~(HASH_STR_NBW);  // 清除原有有效位配置
  HASH->STR |= ValidNumber;      // 设置新的有效位数值
}

/**
  * @brief  哈希数据输入函数
  * @功能   向数据输入FIFO写入32位字数据
  * @细节   每次写入将推进输入队列指针
  * @注意   需遵循配置的数据类型位宽（8/16/32位）
  * @param  Data: 待处理的32位原始数据
  * @retval None
  */
void HASH_DataIn(uint32_t Data)
{
  /* 将数据写入数据输入寄存器（DIN）
     根据HASH_DataType配置自动处理对齐 */
  HASH->DIN = Data;  // 写入后数据进入输入FIFO队列
}

/**
  * @brief  输入FIFO状态查询函数
  * @功能   获取已存入输入队列的完整字数
  * @细节   通过CR寄存器的NBW字段读取
  * @注意   返回值范围：0-15（4bit寄存器空间）
  * @param  None
  * @retval 已存入FIFO的32位字数量
  */
uint8_t HASH_GetInFIFOWordsNbr(void)
{
  /* 从CR寄存器提取NBW字段（位8-11） */
  return ((HASH->CR & HASH_CR_NBW) >> 8); // 右移8位获取实际计数值
}

/**
  * @brief  摘要结果获取函数
  * @功能   从输出寄存器读取最终哈希计算结果
  * @细节   根据算法模式自动获取有效结果长度：
  *         - MD5: 使用Data[0]-Data[3]
  *         - SHA1: 使用Data[0]-Data[4]
  *         - SHA224: 使用Data[0]-Data[6]
  *         - SHA256: 使用全部Data[0]-Data[7]
  * @param  HASH_MessageDigest: 摘要结果存储结构体指针
  * @retval None
  */
void HASH_GetDigest(HASH_MsgDigest* HASH_MessageDigest)
{
  /* 依次读取哈希结果寄存器组HR[0]-HR[4] */
  HASH_MessageDigest->Data[0] = HASH->HR[0];  // 结果段0（低32位）
  HASH_MessageDigest->Data[1] = HASH->HR[1];  // 结果段1
  HASH_MessageDigest->Data[2] = HASH->HR[2];  // 结果段2
  HASH_MessageDigest->Data[3] = HASH->HR[3];  // 结果段3
  HASH_MessageDigest->Data[4] = HASH->HR[4];  // 结果段4（SHA1最高位）
  
  /* 读取扩展结果寄存器（HASH_DIGEST区域） */
  HASH_MessageDigest->Data[5] = HASH_DIGEST->HR[5]; // SHA224/256扩展段5
  HASH_MessageDigest->Data[6] = HASH_DIGEST->HR[6]; // SHA224/256扩展段6
  HASH_MessageDigest->Data[7] = HASH_DIGEST->HR[7]; // SHA256扩展段7
}

/**
  * @brief  摘要计算启动函数
  * @功能   触发最终的数据处理及填充操作
  * @细节   执行以下操作：
  *         1. 补位操作（根据有效位设置）
  *         2. 消息长度追加
  *         3. 启动最终哈希计算
  * @注意   调用后将自动锁定输入FIFO，等待计算完成
  * @param  None
  * @retval None
  */
void HASH_StartDigest(void)
{
  /* 设置STR寄存器的DCAL位启动最终计算 */
  HASH->STR |= HASH_STR_DCAL;  // 触发自动补位及哈希计算流程
                               // 置位后硬件自动处理后续流程
}

/**
  * @}
  */

/** @defgroup HASH_Group3 Context swapping functions
 *  @brief   Context swapping functions
 *
@verbatim   
 ===============================================================================
                      ##### Context swapping functions #####
 ===============================================================================  
 
 [..] This section provides functions allowing to save and store HASH Context
  
 [..] It is possible to interrupt a HASH/HMAC process to perform another processing 
      with a higher priority, and to complete the interrupted process later on, when 
      the higher priority task is complete. To do so, the context of the interrupted 
      task must be saved from the HASH registers to memory, and then be restored 
      from memory to the HASH registers.
  
   (#) To save the current context, use HASH_SaveContext() function
   (#) To restore the saved context, use HASH_RestoreContext() function 
  

@endverbatim
  * @{
  */
  /**
  * @brief  哈希上下文保存函数
  * @功能   保存当前HASH处理器的完整工作状态
  * @细节   捕获以下关键寄存器组：
  *         - 中断掩码寄存器（IMR）
  *         - 状态/控制寄存器对（STR+CR）
  *         - 上下文交换寄存器组CSR[0-53]
  * @注意   调用前必须确保：
  *         - 无数据块正在处理（DINIS=1或NBW≠0）
  *         - 输入FIFO处于安全状态
  * @param  HASH_ContextSave: 上下文存储结构体指针
  * @retval None
  */
void HASH_SaveContext(HASH_Context* HASH_ContextSave)
{
  uint8_t i = 0;
  
  /* 保存中断配置状态 */
  HASH_ContextSave->HASH_IMR = HASH->IMR;  // 保存中断屏蔽寄存器
  
  /* 保存操作状态 */
  HASH_ContextSave->HASH_STR = HASH->STR;  // 状态寄存器（含NBW/DCAL等标志）  
  HASH_ContextSave->HASH_CR  = HASH->CR;  // 控制寄存器（算法配置参数）
  
  /* 完整保存计算上下文寄存器组（共54个32位寄存器） */
  for(i=0; i<=53;i++)
  {
     HASH_ContextSave->HASH_CSR[i] = HASH->CSR[i];  // 存储中间计算结果及上下文
  }   
}

/**
  * @brief  哈希上下文恢复函数
  * @功能   载入先前保存的处理器工作状态
  * @细节   执行顺序：
  *         1. 恢复中断配置
  *         2. 加载控制参数
  *         3. 重置处理器核心
  *         4. 恢复计算中间状态
  * @注意   恢复后需重新检查外设状态，必要时重启DMA传输
  * @param  HASH_ContextRestore: 上下文恢复结构体指针
  * @retval None
  */
void HASH_RestoreContext(HASH_Context* HASH_ContextRestore)  
{
  uint8_t i = 0;
  
  /* 恢复中断管理系统 */
  HASH->IMR = HASH_ContextRestore->HASH_IMR;  // 恢复中断屏蔽设置
  
  /* 加载操作状态参数 */
  HASH->STR = HASH_ContextRestore->HASH_STR;  // 恢复状态寄存器配置
  HASH->CR = HASH_ContextRestore->HASH_CR;    // 恢复控制寄存器配置
  
  /* 硬件初始化序列（确保配置生效） */
  HASH->CR |= HASH_CR_INIT;  // 触发计算核心复位，保留配置参数
  
  /* 恢复计算中间状态（上下文寄存器组） */
  for(i=0; i<=53;i++)
  {
     HASH->CSR[i] = HASH_ContextRestore->HASH_CSR[i];  // 写入54个上下文寄存器
  }   
}

/**
  * @brief  DMA自动摘要模式配置
  * @功能   控制DMA传输完成后的自动摘要计算行为
  * @细节   当ENABLE时：
  *         - DMA传输结束自动执行消息填充
  *         - 自动触发摘要计算
  * @注意   影响CR寄存器的MDMAT位状态：
  *         - 0: 自动开始计算（ENABLE）
  *         - 1: 禁用自动计算（DISABLE）
  * @param  NewState: 功能状态（ENABLE/DISABLE）
  * @retval None
  */
void HASH_AutoStartDigest(FunctionalState NewState)
{
  /* 验证状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 根据配置设置MDMAT位 */
  if (NewState != DISABLE)
  {
    /* 使能自动启动：清除MDMAT位
       （DMA传输结束自动触发最终计算） */
    HASH->CR &= ~HASH_CR_MDMAT;  // MDMAT=0 -> 自动开始
  }
  else
  {
    /* 禁用自动启动：设置MDMAT位
       （需要手动调用HASH_StartDigest） */
    HASH->CR |= HASH_CR_MDMAT;   // MDMAT=1 -> 手动模式
  }
}

  /**
  * @brief  HASH DMA传输控制函数
  * @功能   启用或禁用HASH外设的DMA数据请求功能
  * @细节   控制CR寄存器的DMAE位状态：
  *         - 使能时：HASH生成DMA请求信号，允许DMA向输入FIFO传输数据
  *         - 禁用时：停止DMA请求，需手动数据写入
  * @注意   DMA传输完成后硬件自动禁用请求（单次传输模式）
  *         如需连续传输，需在每次传输前重新使能
  * @param  NewState: DMA功能状态
  *         - ENABLE: 激活DMA数据通道
  *         - DISABLE: 关闭DMA数据通道
  * @retval None
  */
void HASH_DMACmd(FunctionalState NewState)
{
  /* 验证功能状态参数合法性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 设置DMA使能位（DMAE=1），启动DMA数据传输通道 */
    HASH->CR |= HASH_CR_DMAE;  // 允许HASH触发DMA请求信号
  }
  else
  {
    /* 清除DMA使能位（DMAE=0），停止DMA数据传输 */
    HASH->CR &= ~HASH_CR_DMAE; // 禁止HASH产生DMA请求
  }
}

/**
  * @}
  */

/** @defgroup HASH_Group5 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
               ##### Interrupts and flags management functions #####
 ===============================================================================  

 [..] This section provides functions allowing to configure the HASH Interrupts and 
      to get the status and clear flags and Interrupts pending bits.
  
 [..] The HASH provides 2 Interrupts sources and 5 Flags:
  
 *** Flags : ***
 =============== 
 [..]
   (#) HASH_FLAG_DINIS : set when 16 locations are free in the Data IN FIFO 
      which means that a  new block (512 bit) can be entered into the input buffer.
                          
   (#) HASH_FLAG_DCIS :  set when Digest calculation is complete
      
   (#) HASH_FLAG_DMAS :  set when HASH's DMA interface is enabled (DMAE=1) or 
       a transfer is ongoing. This Flag is cleared only by hardware.
                           
   (#) HASH_FLAG_BUSY :  set when The hash core is processing a block of data
       This Flag is cleared only by hardware. 
                           
   (#) HASH_FLAG_DINNE : set when Data IN FIFO is not empty which means that 
       the Data IN FIFO contains at least one word of data. This Flag is cleared 
       only by hardware.
     
 *** Interrupts : ***
 ====================
 [..]   
   (#) HASH_IT_DINI  : if enabled, this interrupt source is pending when 16 
       locations are free in the Data IN FIFO  which means that a new block (512 bit)
       can be entered into the input buffer. This interrupt source is cleared using 
       HASH_ClearITPendingBit(HASH_IT_DINI) function.
   
   (#) HASH_IT_DCI   : if enabled, this interrupt source is pending when Digest 
       calculation is complete. This interrupt source is cleared using 
       HASH_ClearITPendingBit(HASH_IT_DCI) function.

 *** Managing the HASH controller events : ***
 =============================================
 [..] The user should identify which mode will be used in his application to manage 
      the HASH controller events: Polling mode or Interrupt mode.
  
   (#) In the Polling Mode it is advised to use the following functions:
       (++) HASH_GetFlagStatus() : to check if flags events occur. 
       (++) HASH_ClearFlag()     : to clear the flags events.
    
   (#)  In the Interrupt Mode it is advised to use the following functions:
       (++) HASH_ITConfig()       : to enable or disable the interrupt source.
       (++) HASH_GetITStatus()    : to check if Interrupt occurs.
       (++) HASH_ClearITPendingBit() : to clear the Interrupt pending Bit 
            (corresponding Flag). 

@endverbatim
  * @{
  */ 
  /**
  * @brief  哈希中断配置函数
  * @功能   控制HASH外设的指定中断源使能状态
  * @细节   操作中断屏蔽寄存器（IMR）的位域：
  *         - DINI: 输入FIFO就绪中断（数据空间可用）
  *         - DCI: 摘要计算完成中断
  * @注意   支持多中断源组合配置（按位或操作）
  * @param  HASH_IT: 中断源选择，可选组合：
  *         - HASH_IT_DINI: 数据输入中断（FIFO非满时触发）
  *         - HASH_IT_DCI: 摘要计算完成中断
  * @param  NewState: 中断状态（ENABLE/DISABLE）
  * @retval None
  */
void HASH_ITConfig(uint32_t HASH_IT, FunctionalState NewState)
{
  /* 验证中断类型参数合法性 */
  assert_param(IS_HASH_IT(HASH_IT));
  /* 验证功能状态参数合法性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 设置IMR寄存器对应位，启用指定中断 */
    HASH->IMR |= HASH_IT;  // 位或操作保持其他中断状态不变
  }
  else
  {
    /* 清除IMR寄存器对应位，禁用指定中断 */
    HASH->IMR &= (uint32_t)(~HASH_IT); // 位取反后与操作实现位清除
  }
}

/**
  * @brief  哈希状态标志查询函数
  * @功能   检测指定硬件标志位的当前状态
  * @细节   根据标志类型自动选择寄存器：
  *         - DINNE标志查询CR寄存器
  *         - 其他标志查询SR寄存器
  * @注意   BUSY标志反映哈希处理器工作状态（1=忙，0=空闲）
  * @param  HASH_FLAG: 待查询的状态标志，可选：
  *         - HASH_FLAG_DINIS: 输入中断状态（FIFO可写）
  *         - HASH_FLAG_DCIS: 计算完成状态
  *         - HASH_FLAG_BUSY: 处理器忙状态
  *         - HASH_FLAG_DMAS: DMA传输状态
  *         - HASH_FLAG_DINNE: 输入寄存器非空状态
  * @retval FlagStatus: 标志状态（SET/RESET）
  */
FlagStatus HASH_GetFlagStatus(uint32_t HASH_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tempreg = 0;

  /* 验证标志类型参数合法性 */
  assert_param(IS_HASH_GET_FLAG(HASH_FLAG));

  /* 判断标志寄存器来源 */
  if ((HASH_FLAG & HASH_FLAG_DINNE) != (uint32_t)RESET) 
  {
    /* DINNE标志位于控制寄存器（CR）的NBW字段 */
    tempreg = HASH->CR;  // 读取整个控制寄存器
  }
  else 
  {
    /* 其他标志位于状态寄存器（SR） */
    tempreg = HASH->SR;  // 读取整个状态寄存器
  }

  /* 检测指定标志位状态 */
  if ((tempreg & HASH_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;  // 标志位置位状态
  }
  else
  {
    bitstatus = RESET; // 标志位复位状态
  }

  return  bitstatus;
}

/**
  * @brief  哈希状态标志清除函数
  * @功能   清除指定的状态标志位
  * @细节   通过写1清除（W1C）机制操作状态寄存器（SR）
  * @注意   仅适用于可清除标志：
  *         - DINIS（数据输入标志）
  *         - DCIS（计算完成标志）
  * @param  HASH_FLAG: 待清除的标志位组合
  * @retval None
  */
void HASH_ClearFlag(uint32_t HASH_FLAG)
{
  /* 验证可清除标志参数合法性 */
  assert_param(IS_HASH_CLEAR_FLAG(HASH_FLAG));
  
  /* 通过写1清除机制清除标志位 */
  HASH->SR = ~(uint32_t)HASH_FLAG;  // 取反后写入实现指定位写1清除
                                    // 其他位写0保持原值不变
}
/**
  * @brief  哈希中断状态查询函数
  * @功能   检测指定中断源的触发状态（需同时满足中断使能+标志置位）
  * @细节   联合检查IMR（中断使能）和SR（中断标志）寄存器：
  *         - 只有当中断被使能且标志置位时返回SET
  *         - 提供原子级中断状态检测
  * @注意   该函数用于判断实际触发的中断事件，区别于单纯的标志位查询
  * @param  HASH_IT: 中断源类型，可选：
  *         - HASH_IT_DINI: 输入FIFO就绪中断
  *         - HASH_IT_DCI: 摘要计算完成中断
  * @retval ITStatus: 中断触发状态（SET/RESET）
  */
ITStatus HASH_GetITStatus(uint32_t HASH_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t tmpreg = 0;

  /* 验证中断类型参数合法性 */
  assert_param(IS_HASH_GET_IT(HASH_IT));  

  /* 获取当前中断标志状态 */
  tmpreg =  HASH->SR;  // 读取状态寄存器当前值

  /* 联合判断中断使能状态和标志状态 */
  if (((HASH->IMR & tmpreg) & HASH_IT) != RESET) 
  {
    bitstatus = SET;  // 中断已使能且标志置位
  }
  else
  {
    bitstatus = RESET; // 中断未触发（可能未使能或标志未置位）
  }
  
  return bitstatus;
}

/**
  * @brief  哈希中断挂起标志清除函数
  * @功能   清除指定中断源的挂起状态
  * @细节   通过写1清除（W1C）机制操作状态寄存器（SR）
  * @注意   该操作不影响中断使能状态（IMR寄存器）
  * @param  HASH_IT: 中断源类型，可选组合：
  *         - HASH_IT_DINI: 清除输入中断挂起状态
  *         - HASH_IT_DCI: 清除计算完成中断挂起状态
  * @retval None
  */
void HASH_ClearITPendingBit(uint32_t HASH_IT)
{
  /* 验证中断类型参数合法性 */
  assert_param(IS_HASH_IT(HASH_IT));

  /* 通过位取反实现写1清除操作 */
  HASH->SR = (uint32_t)(~HASH_IT);  // 对目标位写1清除挂起状态
                                    // 其他位写0保持原值不变
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

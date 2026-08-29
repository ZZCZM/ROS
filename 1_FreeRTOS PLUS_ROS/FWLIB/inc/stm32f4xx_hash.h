/**
  ******************************************************************************
  * @file    stm32f4xx_hash.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the HASH 
  *          firmware library.
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
#ifndef __STM32F4xx_HASH_H
#define __STM32F4xx_HASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup HASH
  * @{
  */ 
/* Exported types ------------------------------------------------------------*/

/**
  * @brief HASH模块初始化结构体定义
  * @details 该结构体用于配置HASH模块的工作参数，包含算法选择、工作模式、数据类型和HMAC密钥类型等配置项
  */
typedef struct
{
  uint32_t HASH_AlgoSelection; /*!< 哈希算法选择：可选SHA-1/SHA-224/SHA-256/MD5。
                                    取值范围参考@ref HASH_Algo_Selection 定义 */
  uint32_t HASH_AlgoMode;      /*!< 算法工作模式：HASH模式或HMAC模式。
                                    取值范围参考@ref HASH_processor_Algorithm_Mode */ 
  uint32_t HASH_DataType;      /*!< 输入数据类型：32位/16位/8位数据或位串。
                                    取值范围参考@ref HASH_Data_Type */
  uint32_t HASH_HMACKeyType;   /*!< HMAC密钥类型：短密钥或长密钥（仅在HMAC模式下有效）。
                                    取值范围参考@ref HASH_HMAC_Long_key_only_for_HMAC_mode */
} HASH_InitTypeDef;

/**
  * @brief HASH消息摘要结果结构体定义
  * @details 存储不同算法计算得到的最终哈希值，根据算法不同使用不同长度的数组存储
  */
typedef struct
{
  uint32_t Data[8];      /*!< 消息摘要结果存储数组：
                            - SHA-256: 使用8个32位字（256位）
                            - SHA-224: 使用7个32位字（224位） 
                            - SHA-1:   使用5个32位字（160位）
                            - MD5:     使用4个32位字（128位） */
} HASH_MsgDigest;

/**
  * @brief HASH上下文保存结构体定义
  * @details 用于保存HASH处理器的当前状态，实现上下文切换功能
  * @note 包含所有需要保存的寄存器值，用于中断处理或任务切换时保存恢复现场
  */
typedef struct
{
  uint32_t HASH_IMR;  /*!< 中断屏蔽寄存器（HASH_IMR）值 */
  uint32_t HASH_STR;  /*!< 状态寄存器（HASH_STR）值 */    
  uint32_t HASH_CR;   /*!< 控制寄存器（HASH_CR）值 */    
  uint32_t HASH_CSR[54]; /*!< 上下文保存寄存器数组，用于保存HASH处理器计算中间状态
                            共54个32位寄存器，对应HASH_CS0到HASH_CS53寄存器 */      
} HASH_Context;

/* Exported constants --------------------------------------------------------*/

/** @defgroup HASH_Exported_Constants HASH模块导出常量
  * @{
  */

/** @defgroup HASH_Algo_Selection 哈希算法选择
  * @brief 定义HASH模块支持的加密算法类型
  * @{
  */
#define HASH_AlgoSelection_SHA1      ((uint32_t)0x0000) /*!< 选择SHA-1算法，对应CR寄存器ALGO[1:0]=00 */
#define HASH_AlgoSelection_SHA224    HASH_CR_ALGO_1     /*!< 选择SHA-224算法，对应CR寄存器ALGO[1:0]=01 */
#define HASH_AlgoSelection_SHA256    HASH_CR_ALGO       /*!< 选择SHA-256算法，对应CR寄存器ALGO[1:0]=10 */ 
#define HASH_AlgoSelection_MD5       HASH_CR_ALGO_0     /*!< 选择MD5算法，对应CR寄存器ALGO[1:0]=11 */

/**
  * @brief 算法选择参数验证宏
  * @param ALGOSELECTION 需要验证的算法选择参数
  * @retval 有效性检查结果（TRUE/FALSE）
  * @note 用于编译时参数检查，确保传入的算法参数为有效值
  */
#define IS_HASH_ALGOSELECTION(ALGOSELECTION) (((ALGOSELECTION) == HASH_AlgoSelection_SHA1) || \
                                              ((ALGOSELECTION) == HASH_AlgoSelection_SHA224) || \
                                              ((ALGOSELECTION) == HASH_AlgoSelection_SHA256) || \
                                              ((ALGOSELECTION) == HASH_AlgoSelection_MD5))

/**
  * @}
  */
/**
  * @defgroup HASH_processor_Algorithm_Mode HASH处理器算法模式
  * @brief 定义HASH模块的工作模式选择
  * @{
  */
#define HASH_AlgoMode_HASH         ((uint32_t)0x00000000) /*!< 普通哈希计算模式，直接进行哈希运算 */
#define HASH_AlgoMode_HMAC         HASH_CR_MODE           /*!< HMAC认证模式，需要配合密钥使用 */

/**
  * @brief 算法模式验证宏
  * @param ALGOMODE 需要验证的算法模式参数
  * @retval 有效性检查结果（TRUE/FALSE）
  * @note 检查输入参数是否为合法的HASH/HMAC模式选择
  */
#define IS_HASH_ALGOMODE(ALGOMODE) (((ALGOMODE) == HASH_AlgoMode_HASH) || \
                                    ((ALGOMODE) == HASH_AlgoMode_HMAC))
/**
  * @}
  */

/**
  * @defgroup HASH_Data_Type 输入数据类型
  * @brief 定义待处理数据的存储格式
  * @details 指定数据在内存中的存储方式及是否需要字节交换处理
  * @{
  */  
#define HASH_DataType_32b          ((uint32_t)0x0000) /*!< 32位数据（无交换）：直接按字处理，不进行字节序调整 */
#define HASH_DataType_16b          HASH_CR_DATATYPE_0 /*!< 16位数据（半字交换）：每个半字进行字节序交换 */
#define HASH_DataType_8b           HASH_CR_DATATYPE_1 /*!< 8位数据（全字节交换）：所有字节进行顺序交换 */
#define HASH_DataType_1b           HASH_CR_DATATYPE   /*!< 位数据（位级交换）：在字内进行位顺序反转处理 */

/**
  * @brief 数据类型验证宏
  * @param DATATYPE 需要验证的数据类型参数
  * @retval 有效性检查结果（TRUE/FALSE）
  * @note 确保输入参数为支持的4种数据类型之一
  */
#define IS_HASH_DATATYPE(DATATYPE) (((DATATYPE) == HASH_DataType_32b)|| \
                                    ((DATATYPE) == HASH_DataType_16b)|| \
                                    ((DATATYPE) == HASH_DataType_8b) || \
                                    ((DATATYPE) == HASH_DataType_1b))
/**
  * @}
  */

/**
  * @defgroup HASH_HMAC_Long_key_only_for_HMAC_mode HMAC密钥类型
  * @brief 定义HMAC模式下的密钥长度分类
  * @details 根据密钥长度选择不同的处理方式
  * @{
  */ 
#define HASH_HMACKeyType_ShortKey      ((uint32_t)0x00000000) /*!< 短密钥模式：密钥长度≤64字节 */
#define HASH_HMACKeyType_LongKey       HASH_CR_LKEY           /*!< 长密钥模式：密钥长度＞64字节 */

/**
  * @brief HMAC密钥类型验证宏
  * @param KEYTYPE 需要验证的密钥类型参数
  * @retval 有效性检查结果（TRUE/FALSE）
  */
#define IS_HASH_HMAC_KEYTYPE(KEYTYPE) (((KEYTYPE) == HASH_HMACKeyType_ShortKey) || \
                                       ((KEYTYPE) == HASH_HMACKeyType_LongKey))
/**
  * @}
  */

/**
  * @defgroup Number_of_valid_bits_in_last_word_of_the_message 消息末尾有效位定义
  * @brief 配置消息最后一个字中的有效位数
  * @details 用于处理非字节对齐的位数据输入
  * @{
  */  
#define IS_HASH_VALIDBITSNUMBER(VALIDBITS) ((VALIDBITS) <= 0x1F) /*!< 有效位检查宏（最大允许31位）*/

/**
  * @}
  */

/**
  * @defgroup HASH_interrupts_definition HASH中断定义
  * @brief 配置HASH模块的中断源
  * @{
  */  
#define HASH_IT_DINI               HASH_IMR_DINIM  /*!< 输入缓冲就绪中断：当输入缓冲区可接收新数据块时触发 */
#define HASH_IT_DCI                HASH_IMR_DCIM   /*!< 摘要计算完成中断：当哈希计算完成时触发 */

/* 中断参数验证宏 */
#define IS_HASH_IT(IT) ((((IT) & (uint32_t)0xFFFFFFFC) == 0x00000000) && ((IT) != 0x00000000)) /*!< 中断类型合法性检查 */
#define IS_HASH_GET_IT(IT) (((IT) == HASH_IT_DINI) || ((IT) == HASH_IT_DCI)) /*!< 中断状态获取合法性检查 */

		/**
  * @defgroup HASH_flags_definition HASH状态标志定义
  * @brief 哈希模块的状态标志位定义
  * @details 这些标志用于监控HASH处理器的实时状态，可通过状态寄存器(SR)和控制寄存器(CR)访问
  * @{
  */  
#define HASH_FLAG_DINIS            HASH_SR_DINIS  /*!< 输入缓冲区空闲标志（16字空闲）：
                                                       当该位置1时，表示DIN缓冲区至少有16个空闲字位置，
                                                       可以接收新的数据块输入 */
#define HASH_FLAG_DCIS             HASH_SR_DCIS   /*!< 摘要计算完成标志：
                                                       当哈希计算完成时自动置1，
                                                       需要软件清零 */
#define HASH_FLAG_DMAS             HASH_SR_DMAS   /*!< DMA传输状态标志：
                                                       当DMA接口启用(DMAE=1)或
                                                       正在进行DMA传输时置1 */
#define HASH_FLAG_BUSY             HASH_SR_BUSY   /*!< 哈希核心忙标志：
                                                       当哈希处理器正在处理数据块时置1，
                                                       处理完成后自动清零 */
#define HASH_FLAG_DINNE            HASH_CR_DINNE  /*!< 输入缓冲区非空标志（控制寄存器位）：
                                                       当DIN缓冲区包含至少1个有效数据字时置1 */

/**
  * @brief 状态标志获取验证宏
  * @param FLAG 需要验证的状态标志参数
  * @retval 有效性检查结果（TRUE/FALSE）
  * @note 检查输入参数是否为合法的状态标志位
  */
#define IS_HASH_GET_FLAG(FLAG) (((FLAG) == HASH_FLAG_DINIS) || \
                                ((FLAG) == HASH_FLAG_DCIS)  || \
                                ((FLAG) == HASH_FLAG_DMAS)  || \
                                ((FLAG) == HASH_FLAG_BUSY)  || \
                                ((FLAG) == HASH_FLAG_DINNE)) 

/**
  * @brief 可清除标志验证宏
  * @param FLAG 需要验证的清除标志参数
  * @retval 有效性检查结果（TRUE/FALSE）
  * @note 仅DINIS和DCIS标志支持软件清零，其他标志为只读状态
  */
#define IS_HASH_CLEAR_FLAG(FLAG)(((FLAG) == HASH_FLAG_DINIS) || \
                                 ((FLAG) == HASH_FLAG_DCIS)) 
                                

/**
  * @}
  */ 

/**
  * @}
  */ 
/* 导出宏 --------------------------------------------------------------*/
/* 导出函数 ------------------------------------------------------------*/

/**
  * @brief HASH模块硬件复位函数
  * @details 将HASH处理器所有寄存器恢复为默认复位值
  */
void HASH_DeInit(void);

/* HASH配置函数组 ******************************************************/
/**
  * @brief 初始化HASH处理器
  * @param HASH_InitStruct 指向配置结构体的指针，包含算法选择/模式/数据类型等参数
  */
void HASH_Init(HASH_InitTypeDef* HASH_InitStruct);

/**
  * @brief 初始化结构体默认值加载函数
  * @param HASH_InitStruct 需要初始化的结构体指针
  * @details 将结构体成员设为默认值：SHA1算法/HASH模式/32位数据/短密钥
  */
void HASH_StructInit(HASH_InitTypeDef* HASH_InitStruct);

/**
  * @brief HASH处理器软复位函数
  * @details 复位哈希计算单元，清空输入缓冲区，不影响配置寄存器
  */
void HASH_Reset(void);

/* 消息摘要生成函数组 **************************************************/
/**
  * @brief 数据输入函数
  * @param Data 待处理的32位数据字
  * @note 需确保输入缓冲区(DIN)未满（通过HASH_FLAG_DINIS标志判断）
  */
void HASH_DataIn(uint32_t Data);

/**
  * @brief 获取输入FIFO中已存字数
  * @retval uint8_t 当前输入缓冲区中存储的有效字数（0-15）
  */
uint8_t HASH_GetInFIFOWordsNbr(void);

/**
  * @brief 设置消息最后一个字的有效位数
  * @param ValidNumber 有效位数（0-31）
  * @note 用于处理非对齐的位数据输入，应在启动计算前配置
  */
void HASH_SetLastWordValidBitsNbr(uint16_t ValidNumber);

/**
  * @brief 手动启动摘要计算
  * @details 当输入缓冲区数据准备完成后，触发哈希计算流程
  */
void HASH_StartDigest(void);

/**
  * @brief 自动启动摘要计算配置
  * @param NewState 使能状态（ENABLE/DISABLE）
  * @details 使能后，当输入缓冲区满时自动触发计算
  */
void HASH_AutoStartDigest(FunctionalState NewState);

/**
  * @brief 获取计算完成的摘要结果
  * @param HASH_MessageDigest 指向摘要结果结构体的指针
  * @note 应在计算完成标志（HASH_FLAG_DCIS）置位后调用
  */
void HASH_GetDigest(HASH_MsgDigest* HASH_MessageDigest);

/* 上下文切换函数组 ****************************************************/
/**
  * @brief 保存HASH处理器上下文
  * @param HASH_ContextSave 指向上下文保存结构体的指针
  * @details 保存当前IMR/STR/CR/CSR寄存器组状态
  */
void HASH_SaveContext(HASH_Context* HASH_ContextSave);

/**
  * @brief 恢复HASH处理器上下文
  * @param HASH_ContextRestore 指向待恢复上下文结构体的指针
  */
void HASH_RestoreContext(HASH_Context* HASH_ContextRestore);

/* DMA接口函数 *********************************************************/
/**
  * @brief DMA传输使能控制
  * @param NewState 使能状态（ENABLE/DISABLE）
  * @details 控制HASH_DMACONF寄存器的DMAE位
  */
void HASH_DMACmd(FunctionalState NewState);

/* 中断管理函数组 ******************************************************/
/**
  * @brief 中断使能控制
  * @param HASH_IT 指定中断源（HASH_IT_DINI/HASH_IT_DCI）
  * @param NewState 使能状态（ENABLE/DISABLE）
  */
void HASH_ITConfig(uint32_t HASH_IT, FunctionalState NewState);

/**
  * @brief 获取状态标志位状态
  * @param HASH_FLAG 指定状态标志（HASH_FLAG_xxx）
  * @retval FlagStatus 标志位状态（SET/RESET）
  */
FlagStatus HASH_GetFlagStatus(uint32_t HASH_FLAG);

/**
  * @brief 清除可写状态标志
  * @param HASH_FLAG 指定标志位（仅支持HASH_FLAG_DINIS和HASH_FLAG_DCIS）
  */
void HASH_ClearFlag(uint32_t HASH_FLAG);

/**
  * @brief 获取中断挂起状态
  * @param HASH_IT 指定中断源
  * @retval ITStatus 中断状态（SET/RESET）
  */
ITStatus HASH_GetITStatus(uint32_t HASH_IT);

/**
  * @brief 清除中断挂起位
  * @param HASH_IT 指定中断源
  */
void HASH_ClearITPendingBit(uint32_t HASH_IT);

/* 高级SHA1函数 *******************************************************/
/**
  * @brief SHA1哈希计算全流程封装函数
  * @param Input 输入数据指针
  * @param Ilen 输入数据长度（字节）
  * @param Output 输出缓冲区（20字节数组）
  * @retval ErrorStatus 操作状态（SUCCESS/ERROR）
  */
ErrorStatus HASH_SHA1(uint8_t *Input, uint32_t Ilen, uint8_t Output[20]);

/**
  * @brief HMAC-SHA1认证计算封装函数
  * @param Key HMAC密钥指针
  * @param Keylen 密钥长度（字节）
  * @param Input 输入数据指针
  * @param Ilen 输入数据长度（字节）
  * @param Output 输出缓冲区（20字节数组）
  * @retval ErrorStatus 操作状态（SUCCESS/ERROR）
  */
ErrorStatus HMAC_SHA1(uint8_t *Key, uint32_t Keylen,
                      uint8_t *Input, uint32_t Ilen,
                      uint8_t Output[20]);

/* 高级MD5函数 *********************************************************/
/**
  * @brief MD5哈希计算全流程封装函数
  * @param Input 输入数据指针
  * @param Ilen 输入数据长度（字节）
  * @param Output 输出缓冲区（16字节数组）
  * @retval ErrorStatus 操作状态（SUCCESS/ERROR）
  */
ErrorStatus HASH_MD5(uint8_t *Input, uint32_t Ilen, uint8_t Output[16]);

/**
  * @brief HMAC-MD5认证计算封装函数
  * @param Key HMAC密钥指针
  * @param Keylen 密钥长度（字节）
  * @param Input 输入数据指针
  * @param Ilen 输入数据长度（字节）
  * @param Output 输出缓冲区（16字节数组）
  * @retval ErrorStatus 操作状态（SUCCESS/ERROR）
  */
ErrorStatus HMAC_MD5(uint8_t *Key, uint32_t Keylen,
                     uint8_t *Input, uint32_t Ilen,
                     uint8_t Output[16]);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_HASH_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

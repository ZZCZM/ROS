/**
  ******************************************************************************
  * @file    stm32f4xx_cryp.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the Cryptographic
  *          processor(CRYP) firmware library.
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
#ifndef __STM32F4xx_CRYP_H
#define __STM32F4xx_CRYP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup CRYP
  * @{
  */ 
typedef struct
{
  /* 加密或解密方向
     - 可取值：CRYP_AlgoDir_Encrypt 或 CRYP_AlgoDir_Decrypt
     - 详见@ref CRYP_Algorithm_Direction 枚举定义 */
  uint32_t CRYP_AlgoDir;

  /* 加密算法模式
     - 可取值：TDES-ECB, TDES-CBC, DES-ECB, DES-CBC, AES-ECB, AES-CBC, AES-CTR, AES-Key, AES-GCM, AES-CCM
     - 详见@ref CRYP_Algorithm_Mode 枚举定义 */
  uint32_t CRYP_AlgoMode;

  /* 数据类型
     - 可取值：32位数据、16位数据、位数据或位字符串
     - 详见@ref CRYP_Data_Type 枚举定义 */
  uint32_t CRYP_DataType;

  /* AES密钥长度（仅在AES模式中有效）
     - 可取值：128位、192位或256位
     - 详见@ref CRYP_Key_Size_for_AES_only 枚举定义 */
  uint32_t CRYP_KeySize;
} CRYP_InitTypeDef;

/**
  * @brief   CRYP Key(s) structure definition  
  */
typedef struct
{
  /* 密钥0左半部分 */
  uint32_t CRYP_Key0Left;

  /* 密钥0右半部分 */
  uint32_t CRYP_Key0Right;

  /* 密钥1左半部分 */
  uint32_t CRYP_Key1Left;

  /* 密钥1右半部分 */
  uint32_t CRYP_Key1Right;

  /* 密钥2左半部分 */
  uint32_t CRYP_Key2Left;

  /* 密钥2右半部分 */
  uint32_t CRYP_Key2Right;

  /* 密钥3左半部分 */
  uint32_t CRYP_Key3Left;

  /* 密钥3右半部分 */
  uint32_t CRYP_Key3Right;
} CRYP_KeyInitTypeDef;

/**
  * @brief   CRYP Initialization Vectors (IV) structure definition  
  */
typedef struct
{
  /* 初始化向量0左半部分 */
  uint32_t CRYP_IV0Left;

  /* 初始化向量0右半部分 */
  uint32_t CRYP_IV0Right;

  /* 初始化向量1左半部分 */
  uint32_t CRYP_IV1Left;

  /* 初始化向量1右半部分 */
  uint32_t CRYP_IV1Right;
} CRYP_IVInitTypeDef;

/**
  * @brief  CRYP context swapping structure definition  
  */
typedef struct
{
  /* 当前配置 */
  uint32_t CR_CurrentConfig;

  /* 初始化向量IV0 */
  uint32_t CRYP_IV0LR;
  uint32_t CRYP_IV0RR;

  /* 初始化向量IV1 */
  uint32_t CRYP_IV1LR;
  uint32_t CRYP_IV1RR;

  /* 密钥Key0 */
  uint32_t CRYP_K0LR;
  uint32_t CRYP_K0RR;

  /* 密钥Key1 */
  uint32_t CRYP_K1LR;
  uint32_t CRYP_K1RR;

  /* 密钥Key2 */
  uint32_t CRYP_K2LR;
  uint32_t CRYP_K2RR;

  /* 密钥Key3 */
  uint32_t CRYP_K3LR;
  uint32_t CRYP_K3RR;

  /* GCM/CCM上下文 */
  uint32_t CRYP_CSGCMCCMR[8];
  uint32_t CRYP_CSGCMR[8];
} CRYP_Context;

/**
  * @}
  */
/*
 * 模块名称：STM32F4xx加密模块(CRYP)配置参数
 * 功能描述：本头文件定义了STM32F4xx系列芯片加密外设的配置参数，包括：
 *          - 加密算法方向（加密/解密）
 *          - 支持的各种加密算法模式（TDES/DES/AES）
 *          - AES-GCM/CCM加密的阶段控制
 *          - 数据输入/输出的位宽类型
 * 主要用途：用于配置CRYP外设的工作模式、算法选择、数据格式等参数，
 *          这些定义直接对应硬件寄存器的位设置。
 */

/* 导出常量定义 ------------------------------------------------------------*/

/**
  * @defgroup CRYP_Exported_Constants
  * @{
  */

/**
  * @defgroup CRYP_Algorithm_Direction 算法方向控制
  * @brief 加密算法操作方向配置（加密/解密）
  * @{
  */
#define CRYP_AlgoDir_Encrypt      ((uint16_t)0x0000)  /* 加密模式，对应CRYP_CR寄存器DIR位清零 */
#define CRYP_AlgoDir_Decrypt      ((uint16_t)0x0004)  /* 解密模式，对应CRYP_CR寄存器DIR位置1（0x4=bit2） */
/* 参数校验宏：验证输入是否为合法的算法方向值 */
#define IS_CRYP_ALGODIR(ALGODIR) (((ALGODIR) == CRYP_AlgoDir_Encrypt) || \
                                  ((ALGODIR) == CRYP_AlgoDir_Decrypt))
/**
  * @}
  */ 
 
/**
  * @defgroup CRYP_Algorithm_Mode 算法模式选择
  * @brief 加密算法工作模式配置，支持TDES/DES/AES多种模式
  * @{
  */

/* TDES算法模式 -------------------------------------------------*/
#define CRYP_AlgoMode_TDES_ECB    ((uint32_t)0x00000000)  /* TDES ECB模式（电子密码本模式） */
#define CRYP_AlgoMode_TDES_CBC    ((uint32_t)0x00000008)  /* TDES CBC模式（密码块链接模式） */

/* DES算法模式 --------------------------------------------------*/
#define CRYP_AlgoMode_DES_ECB     ((uint32_t)0x00000010)  /* DES ECB模式 */
#define CRYP_AlgoMode_DES_CBC     ((uint32_t)0x00000018)  /* DES CBC模式 */

/* AES算法模式 --------------------------------------------------*/
#define CRYP_AlgoMode_AES_ECB     ((uint32_t)0x00000020)  /* AES ECB基础模式 */
#define CRYP_AlgoMode_AES_CBC     ((uint32_t)0x00000028)  /* AES CBC链式模式 */
#define CRYP_AlgoMode_AES_CTR     ((uint32_t)0x00000030)  /* AES计数器模式 */
#define CRYP_AlgoMode_AES_Key     ((uint32_t)0x00000038)  /* AES密钥派生模式 */
#define CRYP_AlgoMode_AES_GCM     ((uint32_t)0x00080000)  /* AES-GCM认证加密模式 */
#define CRYP_AlgoMode_AES_CCM     ((uint32_t)0x00080008)  /* AES-CCM认证加密模式 */

/* 参数校验宏：验证是否为支持的算法模式 */
#define IS_CRYP_ALGOMODE(ALGOMODE) (((ALGOMODE) == CRYP_AlgoMode_TDES_ECB) || \
                                   ((ALGOMODE) == CRYP_AlgoMode_TDES_CBC)|| \
                                   ((ALGOMODE) == CRYP_AlgoMode_DES_ECB) || \
                                   ((ALGOMODE) == CRYP_AlgoMode_DES_CBC) || \
                                   ((ALGOMODE) == CRYP_AlgoMode_AES_ECB) || \
                                   ((ALGOMODE) == CRYP_AlgoMode_AES_CBC) || \
                                   ((ALGOMODE) == CRYP_AlgoMode_AES_CTR) || \
                                   ((ALGOMODE) == CRYP_AlgoMode_AES_Key) || \
                                   ((ALGOMODE) == CRYP_AlgoMode_AES_GCM) || \
                                   ((ALGOMODE) == CRYP_AlgoMode_AES_CCM))
/**
  * @}
  */ 

/**
  * @defgroup CRYP_Phase 加密阶段控制
  * @brief 专门用于AES-GCM/CCM模式的多阶段操作控制
  * @{
  */

/* 加密阶段定义（仅对AES-GCM/CCM有效）--------------------------*/
#define CRYP_Phase_Init           ((uint32_t)0x00000000)  /* 初始化阶段：设置初始向量 */
#define CRYP_Phase_Header         CRYP_CR_GCM_CCMPH_0     /* 头部处理阶段：0x00010000，处理认证数据 */
#define CRYP_Phase_Payload        CRYP_CR_GCM_CCMPH_1      /* 负载处理阶段：0x00020000，处理加密数据 */
#define CRYP_Phase_Final          CRYP_CR_GCM_CCMPH        /* 最终处理阶段：0x00030000，生成认证标签 */

/* 参数校验宏：验证是否为有效的阶段控制值 */
#define IS_CRYP_PHASE(PHASE) (((PHASE) == CRYP_Phase_Init)    || \
                              ((PHASE) == CRYP_Phase_Header)  || \
                              ((PHASE) == CRYP_Phase_Payload) || \
                              ((PHASE) == CRYP_Phase_Final))

/**
  * @}
  */ 

/**
  * @defgroup CRYP_Data_Type 数据位宽配置
  * @brief 设置加密数据的输入/输出位宽格式
  * @{
  */
#define CRYP_DataType_32b         ((uint16_t)0x0000)  /* 32位数据格式，适用于字对齐操作 */
#define CRYP_DataType_16b         ((uint16_t)0x0040)  /* 16位半字格式，对应CRYP_CR寄存器DATATYPE位设置 */
#define CRYP_DataType_8b          ((uint16_t)0x0080)  /* 8位字节格式 */
#define CRYP_DataType_1b          ((uint16_t)0x00C0)  /* 1位序列格式（特殊用途） */
/* 参数校验宏：验证数据位宽设置是否合法 */
#define IS_CRYP_DATATYPE(DATATYPE) (((DATATYPE) == CRYP_DataType_32b) || \
                                    ((DATATYPE) == CRYP_DataType_16b)|| \
                                    ((DATATYPE) == CRYP_DataType_8b)|| \
                                    ((DATATYPE) == CRYP_DataType_1b))  

/**
  * @}
  */
       /*
 * 模块名称：STM32F4xx加密模块(CRYP)扩展配置
 * 功能描述：本部分定义加密模块的以下配置参数：
 *          - AES密钥长度选择
 *          - 硬件状态标志位定义
 *          - 中断控制参数
 *          - 加解密模式开关
 *          - DMA传输请求配置
 * 重要说明：以下定义直接对应CRYP外设的寄存器位设置，用于实时状态监控和数据传输控制
 */

/** @defgroup CRYP_Key_Size_for_AES_only AES密钥长度配置
  * @brief 专门用于AES算法的密钥长度选择（单位：比特）
  * @{
  */
#define CRYP_KeySize_128b         ((uint16_t)0x0000)  /* 128位密钥，对应CRYP_CR寄存器KEYSIZE[9:8]=00 */
#define CRYP_KeySize_192b         ((uint16_t)0x0100)  /* 192位密钥，KEYSIZE位设置为01（0x0100=bit8） */
#define CRYP_KeySize_256b         ((uint16_t)0x0200)  /* 256位密钥，KEYSIZE位设置为10（0x0200=bit9） */
/* 参数校验宏：验证是否为标准AES密钥长度 */
#define IS_CRYP_KEYSIZE(KEYSIZE) (((KEYSIZE) == CRYP_KeySize_128b)|| \
                                  ((KEYSIZE) == CRYP_KeySize_192b)|| \
                                  ((KEYSIZE) == CRYP_KeySize_256b))
/**
  * @}
  */

/** @defgroup CRYP_flags_definition 状态标志位定义
  * @brief 加密模块实时状态标志位（通过CRYP_SR寄存器读取）
  * @{
  */
#define CRYP_FLAG_BUSY            ((uint8_t)0x10)  /* 忙标志：1=加解密操作进行中/密钥准备中 */
#define CRYP_FLAG_IFEM            ((uint8_t)0x01)  /* 输入FIFO空标志：1=输入缓冲区可写入新数据 */
#define CRYP_FLAG_IFNF            ((uint8_t)0x02)  /* 输入FIFO未满标志：1=可继续写入数据 */
#define CRYP_FLAG_INRIS           ((uint8_t)0x22)  /* 输入中断原始状态：1=输入FIFO达到触发阈值 */
#define CRYP_FLAG_OFNE            ((uint8_t)0x04)  /* 输出FIFO非空标志：1=输出缓冲区有待读取数据 */
#define CRYP_FLAG_OFFU            ((uint8_t)0x08)  /* 输出FIFO满标志：1=输出缓冲区已满 */
#define CRYP_FLAG_OUTRIS          ((uint8_t)0x21)  /* 输出中断原始状态：1=输出FIFO达到触发阈值 */

/* 状态标志校验宏：验证是否为有效标志位组合 */
#define IS_CRYP_GET_FLAG(FLAG) (((FLAG) == CRYP_FLAG_IFEM)  || \
                                ((FLAG) == CRYP_FLAG_IFNF)  || \
                                ((FLAG) == CRYP_FLAG_OFNE)  || \
                                ((FLAG) == CRYP_FLAG_OFFU)  || \
                                ((FLAG) == CRYP_FLAG_BUSY)  || \
                                ((FLAG) == CRYP_FLAG_OUTRIS)|| \
                                ((FLAG) == CRYP_FLAG_INRIS))
/**
  * @}
  */

/** @defgroup CRYP_interrupts_definition 中断控制配置
  * @brief 加密模块中断源配置（通过CRYP_IMSCR寄存器设置）
  * @{
  */
#define CRYP_IT_INI               ((uint8_t)0x01) /* 输入FIFO中断：当输入FIFO到达阈值时触发 */
#define CRYP_IT_OUTI              ((uint8_t)0x02) /* 输出FIFO中断：当输出FIFO有数据可读时触发 */
/* 中断配置校验宏：验证中断类型有效性（允许值：0x01或0x02） */
#define IS_CRYP_CONFIG_IT(IT) ((((IT) & (uint8_t)0xFC) == 0x00) && ((IT) != 0x00))
/* 中断状态校验宏：确认中断来源 */
#define IS_CRYP_GET_IT(IT) (((IT) == CRYP_IT_INI) || ((IT) == CRYP_IT_OUTI))

/**
  * @}
  */

/** @defgroup CRYP_Encryption_Decryption_modes_definition 工作模式开关
  * @brief 加解密全局模式选择（用于上层API控制）
  * @{
  */
#define MODE_ENCRYPT             ((uint8_t)0x01)  /* 全局加密模式使能 */
#define MODE_DECRYPT             ((uint8_t)0x00)  /* 全局解密模式使能 */
/**
  * @}
  */

/** @defgroup CRYP_DMA_transfer_requests DMA传输控制
  * @brief 数据直接存储器访问传输请求配置
  * @{
  */
#define CRYP_DMAReq_DataIN             ((uint8_t)0x01)  /* 输入DMA请求：当输入FIFO有空位时触发DMA传输 */
#define CRYP_DMAReq_DataOUT            ((uint8_t)0x02)  /* 输出DMA请求：当输出FIFO有数据时触发DMA读取 */
/* DMA请求校验宏：验证请求类型有效性（允许值：0x01或0x02） */
#define IS_CRYP_DMAREQ(DMAREQ) ((((DMAREQ) & (uint8_t)0xFC) == 0x00) && ((DMAREQ) != 0x00))

/**
  * @}
  */ 

/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/*
 * 模块名称：STM32F4xx加密模块(CRYP)驱动接口
 * 功能描述：本部分定义加密模块的核心操作函数，包含以下功能组：
 *          - 模块初始化与硬件复位
 *          - 密钥/IV初始化配置
 *          - 数据流处理控制
 *          - 上下文保存与恢复
 *          - DMA传输控制
 *          - 中断管理接口
 * 调用说明：函数通过操作CRYP硬件寄存器实现加密功能，需按顺序调用初始化函数配置参数后再进行数据操作
 */

/* 函数声明 ---------------------------------------------------------------*/

/* 硬件复位函数组 **********************************************************/
/** 
 * @brief 将CRYP模块复位到默认状态
 * @details 清除所有配置寄存器，复位后需重新初始化才能使用
 */
void CRYP_DeInit(void);

/* 初始化配置函数组 ********************************************************/
/**
 * @brief 核心初始化函数
 * @param CRYP_InitStruct 初始化结构体指针，包含算法模式/方向/数据位宽等参数
 */
void CRYP_Init(CRYP_InitTypeDef* CRYP_InitStruct);

/**
 * @brief 初始化结构体默认值加载
 * @param CRYP_InitStruct 需初始化的结构体指针，函数会填入预设默认值
 */
void CRYP_StructInit(CRYP_InitTypeDef* CRYP_InitStruct);

/* 密钥管理函数 ************************************************************/
/**
 * @brief 密钥初始化接口
 * @param CRYP_KeyInitStruct 密钥结构体指针，包含密钥数组及尺寸参数
 */
void CRYP_KeyInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct);

/**
 * @brief 密钥结构体默认值加载
 * @param CRYP_KeyInitStruct 需初始化的密钥结构体指针
 */
void CRYP_KeyStructInit(CRYP_KeyInitTypeDef* CRYP_KeyInitStruct);

/* 初始化向量(IV)管理函数 **************************************************/
/**
 * @brief IV初始化接口
 * @param CRYP_IVInitStruct IV结构体指针，包含初始化向量数组
 */
void CRYP_IVInit(CRYP_IVInitTypeDef* CRYP_IVInitStruct);

/**
 * @brief IV结构体默认值加载
 * @param CRYP_IVInitStruct 需初始化的IV结构体指针
 */
void CRYP_IVStructInit(CRYP_IVInitTypeDef* CRYP_IVInitStruct);

/* 模块操作控制函数 ********************************************************/
/**
 * @brief 使能/禁用CRYP模块
 * @param NewState ENABLE(启动加密操作)/DISABLE(关闭模块)
 */
void CRYP_Cmd(FunctionalState NewState);

/**
 * @brief 配置AES-GCM/CCM操作阶段
 * @param CRYP_Phase 阶段选择：Init/Header/Payload/Final
 */
void CRYP_PhaseConfig(uint32_t CRYP_Phase);

/**
 * @brief 清空数据FIFO缓冲区
 * @warning 执行后输入/输出缓冲区数据将全部丢失
 */
void CRYP_FIFOFlush(void);

/* 数据流处理接口 **********************************************************/
/**
 * @brief 写入待加密数据到输入FIFO
 * @param Data 32位格式的原始数据（实际位宽由CRYP_DataType配置决定）
 */
void CRYP_DataIn(uint32_t Data);

/**
 * @brief 从输出FIFO读取处理结果
 * @return uint32_t 32位格式的加密/解密结果
 */
uint32_t CRYP_DataOut(void);

/* 上下文管理函数 **********************************************************/
/**
 * @brief 保存当前加密上下文（用于任务切换）
 * @param CRYP_ContextSave 上下文存储结构指针
 * @param CRYP_KeyInitStruct 密钥结构指针（需配合保存）
 * @return ErrorStatus 操作状态：SUCCESS/ERROR
 */
ErrorStatus CRYP_SaveContext(CRYP_Context* CRYP_ContextSave,
                             CRYP_KeyInitTypeDef* CRYP_KeyInitStruct);

/**
 * @brief 恢复先前保存的加密上下文
 * @param CRYP_ContextRestore 上下文存储结构指针
 */
void CRYP_RestoreContext(CRYP_Context* CRYP_ContextRestore);

/* DMA传输控制接口 *********************************************************/
/**
 * @brief 启用/禁用DMA传输请求
 * @param CRYP_DMAReq DMA请求类型：DataIN/DataOUT
 * @param NewState ENABLE(启用DMA)/DISABLE(关闭DMA)
 */
void CRYP_DMACmd(uint8_t CRYP_DMAReq, FunctionalState NewState);

/* 中断管理函数组 **********************************************************/
/**
 * @brief 中断使能控制
 * @param CRYP_IT 中断类型：INI（输入中断）/OUTI（输出中断）
 * @param NewState ENABLE/DISABLE
 */
void CRYP_ITConfig(uint8_t CRYP_IT, FunctionalState NewState);

/**
 * @brief 获取中断状态
 * @param CRYP_IT 待查询的中断类型
 * @return ITStatus SET/RESET
 */
ITStatus CRYP_GetITStatus(uint8_t CRYP_IT);

/**
 * @brief 获取模块使能状态
 * @return FunctionalState ENABLE(运行中)/DISABLE(已停止)
 */
FunctionalState CRYP_GetCmdStatus(void);

/**
 * @brief 查询状态标志位
 * @param CRYP_FLAG 待查询的状态标志
 * @return FlagStatus SET/RESET
 */
FlagStatus CRYP_GetFlagStatus(uint8_t CRYP_FLAG);
/*
 * 模块名称：STM32F4xx加密模块(CRYP)高级算法接口
 * 功能描述：本部分提供高级加密算法的一站式操作接口，支持以下标准：
 *          - AES全模式：ECB/CBC/CTR/GCM/CCM
 *          - TDES算法：ECB/CBC模式
 *          - DES算法：ECB/CBC模式
 * 特点说明：这些函数封装了底层硬件操作，实现完整加密流程，支持带认证的加密模式
 *          （GCM/CCM），开发者无需手动处理数据分块和状态管理
 */

/* AES算法高级接口 **********************************************************/
/**
 * @brief AES ECB模式加解密（基础电子密码本模式）
 * @param Mode 模式选择：MODE_ENCRYPT/MODE_DECRYPT
 * @param Key 密钥指针（长度根据Keysize决定）
 * @param Keysize 密钥长度：128/192/256位（使用CRYP_KeySize_XXX宏定义）
 * @param Input 输入数据指针（需32位对齐）
 * @param Ilength 输入数据字节长度（必须为16的倍数）
 * @param Output 输出缓冲区指针（空间≥Ilength）
 * @return ErrorStatus 操作结果：SUCCESS/ERROR
 */
ErrorStatus CRYP_AES_ECB(uint8_t Mode,
                         uint8_t *Key, uint16_t Keysize,
                         uint8_t *Input, uint32_t Ilength,
                         uint8_t *Output);

/**
 * @brief AES CBC模式加解密（密码块链式模式）
 * @param InitVectors 16字节初始化向量（建议使用随机数）
 * @param 其他参数同AES_ECB
 */
ErrorStatus CRYP_AES_CBC(uint8_t Mode,
                         uint8_t InitVectors[16],
                         uint8_t *Key, uint16_t Keysize,
                         uint8_t *Input, uint32_t Ilength,
                         uint8_t *Output);

/**
 * @brief AES CTR模式加解密（计数器模式，支持流加密）
 * @param InitVectors 16字节计数器（格式：Nonce + Counter）
 */
ErrorStatus CRYP_AES_CTR(uint8_t Mode,
                         uint8_t InitVectors[16],
                         uint8_t *Key, uint16_t Keysize,
                         uint8_t *Input, uint32_t Ilength,
                         uint8_t *Output);

/**
 * @brief AES-GCM认证加密模式（同时提供加密和完整性保护）
 * @param Header 认证头数据指针（不加密但参与认证计算）
 * @param HLength 认证头数据字节长度
 * @param AuthTAG 认证标签输出缓冲区（16字节）
 */
ErrorStatus CRYP_AES_GCM(uint8_t Mode, uint8_t InitVectors[16],
                         uint8_t *Key, uint16_t Keysize,
                         uint8_t *Input, uint32_t ILength,
                         uint8_t *Header, uint32_t HLength,
                         uint8_t *Output, uint8_t *AuthTAG);

/**
 * @brief AES-CCM认证加密模式（适用于受限环境）
 * @param Nonce 随机数指针（长度7-13字节，由NonceSize指定）
 * @param NonceSize 随机数长度：7~13字节
 * @param HBuffer 头部缓存区（用于预计算头部认证）
 * @param TAGSize 认证标签长度：4/6/8/12/14/16字节
 */
ErrorStatus CRYP_AES_CCM(uint8_t Mode, 
                         uint8_t* Nonce, uint32_t NonceSize,
                         uint8_t* Key, uint16_t Keysize,
                         uint8_t* Input, uint32_t ILength,
                         uint8_t* Header, uint32_t HLength, uint8_t *HBuffer,
                         uint8_t* Output,
                         uint8_t* AuthTAG, uint32_t TAGSize);

/* TDES算法高级接口 ********************************************************/
/**
 * @brief TDES ECB模式加解密（三重DES电子密码本）
 * @param Key 24字节密钥（实际使用3个DES密钥：KEY1-KEY2-KEY1）
 * @param Ilength 输入长度（必须为8的倍数）
 */
ErrorStatus CRYP_TDES_ECB(uint8_t Mode,
                           uint8_t Key[24], 
                           uint8_t *Input, uint32_t Ilength,
                           uint8_t *Output);

/**
 * @brief TDES CBC模式加解密（链式模式）
 * @param InitVectors 8字节初始化向量
 */
ErrorStatus CRYP_TDES_CBC(uint8_t Mode,
                          uint8_t Key[24],
                          uint8_t InitVectors[8],
                          uint8_t *Input, uint32_t Ilength,
                          uint8_t *Output);

/* DES算法高级接口 *********************************************************/
/**
 * @brief DES ECB模式加解密（单重DES基础模式）
 * @param Key 8字节DES密钥
 */
ErrorStatus CRYP_DES_ECB(uint8_t Mode,
                         uint8_t Key[8],
                         uint8_t *Input, uint32_t Ilength,
                         uint8_t *Output);

/**
 * @brief DES CBC模式加解密（链式加密）
 * @param InitVectors 8字节初始化向量
 */
ErrorStatus CRYP_DES_CBC(uint8_t Mode,
                         uint8_t Key[8],
                         uint8_t InitVectors[8],
                         uint8_t *Input,uint32_t Ilength,
                         uint8_t *Output);

#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_CRYP_H */

/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

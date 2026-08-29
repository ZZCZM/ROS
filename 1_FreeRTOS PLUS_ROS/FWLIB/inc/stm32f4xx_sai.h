/**
  ******************************************************************************
  * @file    stm32f4xx_sai.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the SAI 
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
#ifndef __STM32F4xx_SAI_H
#define __STM32F4xx_SAI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */
/** @addtogroup SAI 
  * @{
  * @brief 串行音频接口(SAI)配置结构体定义
  */

/* 导出类型 ------------------------------------------------------------*/

/** 
  * @brief SAI模块初始化结构体
  * @note 用于配置SAI模块的全局工作参数
  */
typedef struct
{
  uint32_t SAI_AudioMode;           /*!< 指定SAI模块工作模式
                                         @可取值参考 @ref SAI_Block_Mode 
                                         - 主模式: 生成时钟信号
                                         - 从模式: 接收外部时钟信号 */

  uint32_t SAI_Protocol;           /*!< 指定通信协议类型
                                         @可取值参考 @ref SAI_Block_Protocol 
                                         @警告 选择AC'97或SPDIF协议时，部分参数将被忽略 */

  uint32_t SAI_DataSize;           /*!< 设置数据位宽
                                         @可取值参考 @ref SAI_Block_Data_Size
                                         @范围 8位/10位/16位/20位/24位/32位
                                         @注意 AC'97和SPDIF协议下此参数无效 */

  uint32_t SAI_FirstBit;           /*!< 指定数据传输起始位
                                         @可取值参考 @ref SAI_Block_MSB_LSB_transmission
                                         - MSB_FIRST: 高位优先传输
                                         - LSB_FIRST: 低位优先传输
                                         @注意 使用AC'97或SPDIF协议时此参数无效 */

  uint32_t SAI_ClockStrobing;      /*!< 时钟信号边沿控制
                                         @可取值参考 @ref SAI_Block_Clock_Strobing
                                         - 上升沿采样/下降沿保持
                                         - 下降沿采样/上升沿保持 */

  uint32_t SAI_Synchro;            /*!< 多模块同步配置
                                         @可取值参考 @ref SAI_Block_Synchronization
                                         - 异步模式：独立工作
                                         - 主从同步：多模块时钟同步 */

  uint32_t SAI_OUTDRIV;            /*!< 输出驱动模式
                                         @可取值参考 @ref SAI_Block_Output_Drive
                                         - 推挽输出
                                         - 开漏输出
                                         @注意 需在模块使能前配置 */

  uint32_t SAI_NoDivider;          /*!< 主时钟分频控制
                                         @可取值参考 @ref SAI_Block_NoDivider
                                         - 禁用分频：MCK直接使用系统时钟
                                         - 使能分频：使用下方MasterDivider参数 */

  uint32_t SAI_MasterDivider;      /*!< 主时钟分频系数
                                         @范围 0-15，实际分频值 = (MCKDIV+1)*2
                                         @公式 MCLK频率 = SAI_CK / ((MCKDIV+1)*2) */

  uint32_t SAI_FIFOThreshold;     /*!< FIFO阈值设置
                                         @可取值参考 @ref SAI_Block_Fifo_Threshold
                                         - 1/4 FIFO大小触发
                                         - 1/2 FIFO大小触发
                                         - 3/4 FIFO大小触发 */
}SAI_InitTypeDef;

/** 
  * @brief SAI帧结构初始化结构体
  * @note 配置音频帧的时序参数
  */
typedef struct
{
  uint32_t SAI_FrameLength;        /*!< 帧长度（SCK周期数）
                                         @范围 8-256
                                         @注意 当MCLK作为输出时，建议设为2的幂次方 */

  uint32_t SAI_ActiveFrameLength;  /*!< 有效帧同步信号长度
                                         @范围 1-128
                                         @说明 定义FS信号有效电平持续时间 */

  uint32_t SAI_FSDefinition;       /*!< 帧同步信号定义模式
                                         @可取值参考 @ref SAI_Block_FS_Definition
                                         - 起始位对齐
                                         - 数据包对齐 */

  uint32_t SAI_FSPolarity;         /*!< 帧同步信号极性
                                         @可取值参考 @ref SAI_Block_FS_Polarity
                                         - 低电平有效
                                         - 高电平有效 */

  uint32_t SAI_FSOffset;           /*!< 帧同步信号偏移配置
                                         @可取值参考 @ref SAI_Block_FS_Offset
                                         - 帧同步在第一个数据位前
                                         - 帧同步与第一个数据位同步 */
}SAI_FrameInitTypeDef;

/** 
  * @brief SAI时隙配置结构体
  * @note 定义多时隙传输的参数
  */
typedef struct
{
  uint32_t SAI_FirstBitOffset;     /*!< 时隙内首数据位偏移
                                         @范围 0-24位
                                         @说明 定义数据在时隙中的起始位置 */

  uint32_t SAI_SlotSize;           /*!< 单个时隙位宽
                                         @可取值参考 @ref SAI_Block_Slot_Size
                                         - 16位时隙
                                         - 32位时隙 */

  uint32_t SAI_SlotNumber;         /*!< 每帧包含的时隙数量
                                         @范围 1-16
                                         @说明 支持多时隙分时传输 */

  uint32_t SAI_SlotActive;         /*!< 激活时隙配置
                                         @可取值参考 @ref SAI_Block_Slot_Active
                                         - 时隙位掩码配置
                                         - 全时隙激活 */
}SAI_SlotInitTypeDef;

/**
  * @}
  */

/* 导出常量 --------------------------------------------------------*/

/** @defgroup SAI_Exported_Constants SAI模块常量定义
  * @{
  */

/**
  * @brief SAI外设实例验证宏
  * @param PERIPH 外设实例
  * @retval 有效性检查结果
  * @note 当前仅支持SAI1实例
  */
#define IS_SAI_PERIPH(PERIPH) ((PERIPH) == SAI1)

/**
  * @brief SAI子模块验证宏
  * @param PERIPH 子模块实例
  * @retval 有效性检查结果
  * @note SAI1支持Block_A和Block_B两个独立子模块
  */
#define IS_SAI_BLOCK_PERIPH(PERIPH) (((PERIPH) == SAI1_Block_A) || \
                                     ((PERIPH) == SAI1_Block_B))

/** @defgroup SAI_Block_Mode SAI工作模式
  * @{
  */
#define SAI_Mode_MasterTx               ((uint32_t)0x00000000) /*!< 主发送模式：生成时钟并发送数据 */
#define SAI_Mode_MasterRx               ((uint32_t)0x00000001)  /*!< 主接收模式：生成时钟并接收数据 */
#define SAI_Mode_SlaveTx                ((uint32_t)0x00000002) /*!< 从发送模式：使用外部时钟发送数据 */
#define SAI_Mode_SlaveRx                ((uint32_t)0x00000003) /*!< 从接收模式：使用外部时钟接收数据 */
/**
  * @brief 工作模式验证宏
  * @param MODE 模式选择
  * @retval 有效性检查结果
  */
#define IS_SAI_BLOCK_MODE(MODE) (((MODE) == SAI_Mode_MasterTx) || \
                                 ((MODE) == SAI_Mode_MasterRx) || \
                                 ((MODE) == SAI_Mode_SlaveTx)  || \
                                 ((MODE) == SAI_Mode_SlaveRx))
/**
  * @}
  */

/** @defgroup SAI_Block_Protocol 通信协议类型
  * @{
  */
#define SAI_Free_Protocol                 ((uint32_t)0x00000000) /*!< 自由协议模式 */
#define SAI_SPDIF_Protocol                ((uint32_t)SAI_xCR1_PRTCFG_0) /*!< IEC 60958 SPDIF协议 */
#define SAI_AC97_Protocol                 ((uint32_t)SAI_xCR1_PRTCFG_1) /*!< AC'97协议 */
/**
  * @brief 协议类型验证宏
  * @param PROTOCOL 协议选择
  * @retval 有效性检查结果
  * @note 协议选择将影响数据格式和时钟配置
  */
#define IS_SAI_BLOCK_PROTOCOL(PROTOCOL) (((PROTOCOL) == SAI_Free_Protocol)  || \
                                         ((PROTOCOL) == SAI_SPDIF_Protocol) || \
                                         ((PROTOCOL) == SAI_AC97_Protocol))
/**
  * @}
  */

/** @defgroup SAI_Block_Data_Size 数据位宽配置
  * @{
  */
#define SAI_DataSize_8b                   ((uint32_t)0x00000040)  /*!< 8位数据格式 */
#define SAI_DataSize_10b                  ((uint32_t)0x00000060) /*!< 10位数据格式 */
#define SAI_DataSize_16b                  ((uint32_t)0x00000080) /*!< 16位数据格式 */
#define SAI_DataSize_20b                  ((uint32_t)0x000000A0) /*!< 20位数据格式 */
#define SAI_DataSize_24b                  ((uint32_t)0x000000C0) /*!< 24位数据格式 */
#define SAI_DataSize_32b                  ((uint32_t)0x000000E0) /*!< 32位数据格式 */
/**
  * @brief 数据位宽验证宏
  * @param DATASIZE 位宽选择
  * @retval 有效性检查结果
  * @note 在AC'97/SPDIF协议下此参数无效
  */
#define IS_SAI_BLOCK_DATASIZE(DATASIZE) (((DATASIZE) == SAI_DataSize_8b)  || \
                                         ((DATASIZE) == SAI_DataSize_10b) || \
                                         ((DATASIZE) == SAI_DataSize_16b) || \
                                         ((DATASIZE) == SAI_DataSize_20b) || \
                                         ((DATASIZE) == SAI_DataSize_24b) || \
                                         ((DATASIZE) == SAI_DataSize_32b))
/** @defgroup SAI_Block_MSB_LSB_transmission 数据传输起始位配置
  * @{
  * @brief 定义数据位的传输顺序
  */
#define SAI_FirstBit_MSB                  ((uint32_t)0x00000000) /*!< 高位优先传输(MSB First) */
#define SAI_FirstBit_LSB                  ((uint32_t)SAI_xCR1_LSBFIRST) /*!< 低位优先传输(LSB First) */
/**
  * @brief 起始位验证宏
  * @param BIT 起始位配置
  * @retval 有效性检查结果
  * @note 在I2S协议下必须配置为LSB优先
  */
#define IS_SAI_BLOCK_FIRST_BIT(BIT) (((BIT) == SAI_FirstBit_MSB) || \
                                     ((BIT) == SAI_FirstBit_LSB))
/**
  * @}
  */

/** @defgroup SAI_Block_Clock_Strobing 时钟触发边沿配置
  * @{
  * @brief 定义数据采样时钟边沿
  */
#define SAI_ClockStrobing_FallingEdge     ((uint32_t)0x00000000) /*!< 下降沿采样数据 */
#define SAI_ClockStrobing_RisingEdge      ((uint32_t)SAI_xCR1_CKSTR) /*!< 上升沿采样数据 */
/**
  * @brief 时钟边沿验证宏
  * @param CLOCK 时钟边沿配置
  * @retval 有效性检查结果
  * @note 需与连接的音频设备保持时序一致
  */
#define IS_SAI_BLOCK_CLOCK_STROBING(CLOCK) (((CLOCK) == SAI_ClockStrobing_FallingEdge) || \
                                            ((CLOCK) == SAI_ClockStrobing_RisingEdge))
/**
  * @}
  */

/** @defgroup SAI_Block_Synchronization 同步模式配置
  * @{
  * @brief 多SAI模块同步控制
  */
#define SAI_Asynchronous                   ((uint32_t)0x00000000) /*!< 异步模式：独立工作 */
#define SAI_Synchronous                    ((uint32_t)SAI_xCR1_SYNCEN_0) /*!< 同步模式：与其他SAI模块时钟同步 */
/**
  * @brief 同步模式验证宏
  * @param SYNCHRO 同步配置
  * @retval 有效性检查结果
  * @note 同步模式下需配置主从关系
  */
#define IS_SAI_BLOCK_SYNCHRO(SYNCHRO) (((SYNCHRO) == SAI_Synchronous) || \
                                       ((SYNCHRO) == SAI_Asynchronous))
/**
  * @}
  */ 

/** @defgroup SAI_Block_Output_Drive 输出驱动模式
  * @{
  * @brief 控制数据线的输出驱动类型
  */
#define SAI_OutputDrive_Disabled          ((uint32_t)0x00000000) /*!< 禁用输出驱动（高阻态） */
#define SAI_OutputDrive_Enabled           ((uint32_t)SAI_xCR1_OUTDRIV) /*!< 启用推挽输出驱动 */
/**
  * @brief 输出驱动验证宏
  * @param DRIVE 驱动配置
  * @retval 有效性检查结果
  * @note 启用驱动前需配置GPIO为复用推挽模式
  */
#define IS_SAI_BLOCK_OUTPUT_DRIVE(DRIVE) (((DRIVE) == SAI_OutputDrive_Disabled) || \
                                          ((DRIVE) == SAI_OutputDrive_Enabled))
/**
  * @}
  */ 

/** @defgroup SAI_Block_NoDivider 主时钟分频控制
  * @{
  * @brief 主时钟分频器开关控制
  */
#define SAI_MasterDivider_Enabled         ((uint32_t)0x00000000) /*!< 启用主时钟分频器 */
#define SAI_MasterDivider_Disabled        ((uint32_t)SAI_xCR1_NODIV) /*!< 直通主时钟（禁用分频） */
/**
  * @brief 分频控制验证宏
  * @param NODIVIDER 分频配置
  * @retval 有效性检查结果
  * @note 禁用分频时需保证外部时钟符合速率要求
  */
#define IS_SAI_BLOCK_NODIVIDER(NODIVIDER) (((NODIVIDER) == SAI_MasterDivider_Enabled) || \
                                           ((NODIVIDER) == SAI_MasterDivider_Disabled))
/**
  * @}
  */

/**
  * @}
  */
  
/** @defgroup SAI_Block_Master_Divider 主时钟分频系数
  * @{
  * @brief 主时钟分频系数验证
  */
#define IS_SAI_BLOCK_MASTER_DIVIDER(DIVIDER) ((DIVIDER) <= 15) /*!< 分频系数范围0-15，实际分频值=(DIVIDER+1)*2 */
/**
  * @}
  */
  
/** @defgroup SAI_Block_Frame_Length 帧长度配置
  * @{
  * @brief 定义单个音频帧包含的SCK周期数
  */
#define IS_SAI_BLOCK_FRAME_LENGTH(LENGTH) ((8 <= (LENGTH)) && ((LENGTH) <= 256)) /*!< 有效范围8-256个时钟周期 */
/**
  * @}
  */
    
/** @defgroup SAI_Block_Active_FrameLength 有效帧同步长度
  * @{
  * @brief 定义帧同步信号有效电平持续时间
  */
#define IS_SAI_BLOCK_ACTIVE_FRAME(LENGTH) ((1 <= (LENGTH)) && ((LENGTH) <= 128)) /*!< 有效范围1-128个时钟周期 */
/**
  * @}
  */

/** @defgroup SAI_Block_FS_Definition 帧同步模式
  * @{
  * @brief 定义帧同步信号作用模式
  */
#define SAI_FS_StartFrame                 ((uint32_t)0x00000000) /*!< 标准帧同步模式 */
#define I2S_FS_ChannelIdentification      ((uint32_t)SAI_xFRCR_FSDEF) /*!< I2S通道识别模式 */
/**
  * @brief 帧同步模式验证宏
  * @param DEFINITION 模式选择
  * @retval 有效性检查结果
  * @note 选择ChannelIdentification时需启用I2S协议
  */
#define IS_SAI_BLOCK_FS_DEFINITION(DEFINITION) (((DEFINITION) == SAI_FS_StartFrame) || \
                                                ((DEFINITION) == I2S_FS_ChannelIdentification))
/**
  * @}
  */

/** @defgroup SAI_Block_FS_Polarity 帧同步极性
  * @{
  * @brief 定义帧同步信号有效电平
  */
#define SAI_FS_ActiveLow                  ((uint32_t)0x00000000) /*!< 低电平有效 */
#define SAI_FS_ActiveHigh                 ((uint32_t)SAI_xFRCR_FSPO) /*!< 高电平有效 */
/**
  * @brief 极性验证宏
  * @param POLARITY 极性配置
  * @retval 有效性检查结果
  * @note 需与从设备配置保持一致
  */
#define IS_SAI_BLOCK_FS_POLARITY(POLARITY) (((POLARITY) == SAI_FS_ActiveLow) || \
                                            ((POLARITY) == SAI_FS_ActiveHigh))
/**
  * @}
  */
            
/** @defgroup SAI_Block_FS_Offset 帧同步偏移
  * @{
  * @brief 定义帧同步信号与数据位的时序关系
  */
#define SAI_FS_FirstBit                   ((uint32_t)0x00000000) /*!< 同步信号与首数据位同时开始 */
#define SAI_FS_BeforeFirstBit             ((uint32_t)SAI_xFRCR_FSOFF) /*!< 同步信号在首数据位前开始 */
/**
  * @brief 偏移模式验证宏
  * @param OFFSET 偏移配置
  * @retval 有效性检查结果
  * @note 影响数据采样窗口位置
  */
#define IS_SAI_BLOCK_FS_OFFSET(OFFSET) (((OFFSET) == SAI_FS_FirstBit) || \
                                        ((OFFSET) == SAI_FS_BeforeFirstBit))
/**
  * @}
  */
  
/** @defgroup SAI_Block_Slot_FirstBit_Offset 时隙首数据位偏移
  * @{
  * @brief 定义数据在时隙中的起始位置
  */
#define IS_SAI_BLOCK_FIRSTBIT_OFFSET(OFFSET) ((OFFSET) <= 24) /*!< 最大允许24位偏移 */
/**
  * @}
  */

/** @defgroup SAI_Block_Slot_Size 时隙位宽配置
  * @{
  * @brief 定义单个时隙的容量大小
  */
#define SAI_SlotSize_DataSize             ((uint32_t)0x00000000)  /*!< 时隙大小等于数据位宽 */
#define SAI_SlotSize_16b                  ((uint32_t)SAI_xSLOTR_SLOTSZ_0) /*!< 固定16位时隙 */
#define SAI_SlotSize_32b                  ((uint32_t)SAI_xSLOTR_SLOTSZ_1) /*!< 固定32位时隙 */
/**
  * @brief 时隙大小验证宏
  * @param SIZE 位宽配置
  * @retval 有效性检查结果
  * @note 选择DataSize时实际使用SAI_DataSize参数值
  */
#define IS_SAI_BLOCK_SLOT_SIZE(SIZE) (((SIZE) == SAI_SlotSize_DataSize) || \
                                      ((SIZE) == SAI_SlotSize_16b)      || \
                                      ((SIZE) == SAI_SlotSize_32b))
/**
  * @}
  */


/**
  * @}
  */
/** @defgroup SAI_Block_Slot_Number 时隙数量配置
  * @{
  * @brief 验证每帧包含的时隙数量有效性
  */
#define IS_SAI_BLOCK_SLOT_NUMBER(NUMBER) ((1 <= (NUMBER)) && ((NUMBER) <= 16)) /*!< 有效时隙数范围1-16 */
/**
  * @}
  */

/** @defgroup SAI_Block_Slot_Active 时隙激活配置
  * @{
  * @brief 时隙激活位掩码定义
  */
#define SAI_Slot_NotActive           ((uint32_t)0x00000000)  /*!< 禁用所有时隙（空配置） */
#define SAI_SlotActive_0             ((uint32_t)0x00010000)  /*!< 激活时隙0（位16置1） */
#define SAI_SlotActive_1             ((uint32_t)0x00020000)  /*!< 激活时隙1（位17置1） */
#define SAI_SlotActive_2             ((uint32_t)0x00040000)  /*!< 激活时隙2（位18置1） */
#define SAI_SlotActive_3             ((uint32_t)0x00080000)  /*!< 激活时隙3（位19置1） */
#define SAI_SlotActive_4             ((uint32_t)0x00100000)  /*!< 激活时隙4（位20置1） */
#define SAI_SlotActive_5             ((uint32_t)0x00200000)  /*!< 激活时隙5（位21置1） */
#define SAI_SlotActive_6             ((uint32_t)0x00400000)  /*!< 激活时隙6（位22置1） */
#define SAI_SlotActive_7             ((uint32_t)0x00800000)  /*!< 激活时隙7（位23置1） */
#define SAI_SlotActive_8             ((uint32_t)0x01000000)  /*!< 激活时隙8（位24置1） */
#define SAI_SlotActive_9             ((uint32_t)0x02000000)  /*!< 激活时隙9（位25置1） */
#define SAI_SlotActive_10            ((uint32_t)0x04000000)  /*!< 激活时隙10（位26置1） */
#define SAI_SlotActive_11            ((uint32_t)0x08000000)  /*!< 激活时隙11（位27置1） */
#define SAI_SlotActive_12            ((uint32_t)0x10000000)  /*!< 激活时隙12（位28置1） */
#define SAI_SlotActive_13            ((uint32_t)0x20000000)  /*!< 激活时隙13（位29置1） */
#define SAI_SlotActive_14            ((uint32_t)0x40000000)  /*!< 激活时隙14（位30置1） */
#define SAI_SlotActive_15            ((uint32_t)0x80000000)  /*!< 激活时隙15（位31置1） */
#define SAI_SlotActive_ALL           ((uint32_t)0xFFFF0000)  /*!< 激活全部16个时隙（位16-31置1） */

/**
  * @brief 时隙激活状态验证宏
  * @param ACTIVE 激活配置值
  * @retval 有效性检查结果
  * @note 必须至少激活一个时隙，多时隙激活使用位或(|)操作组合
  */
#define IS_SAI_SLOT_ACTIVE(ACTIVE) ((ACTIVE) != 0)
/**
  * @}
  */
/** @defgroup SAI_Mono_Streo_Mode 音频通道模式
  * @{
  * @brief 配置音频数据的通道数量
  */
#define SAI_MonoMode                      ((uint32_t)SAI_xCR1_MONO) /*!< 单声道模式：左右声道数据相同 */
#define SAI_StreoMode                     ((uint32_t)0x00000000)   /*!< 立体声模式：独立左右声道数据 */
/**
  * @brief 通道模式验证宏
  * @param MODE 模式选择
  * @retval 有效性检查结果
  * @note 单声道模式可节省总线带宽
  */
#define IS_SAI_BLOCK_MONO_STREO_MODE(MODE) (((MODE) == SAI_MonoMode) ||\
                                            ((MODE) == SAI_StreoMode))
/**
  * @}
  */

/** @defgroup SAI_TRIState_Management 三态管理
  * @{
  * @brief 控制数据线空闲状态
  */
#define SAI_Output_NotReleased              ((uint32_t)0x00000000) /*!< 保持最后输出状态 */
#define SAI_Output_Released                 ((uint32_t)SAI_xCR2_TRIS) /*!< 高阻态释放总线 */
/**
  * @brief 三态模式验证宏
  * @param STATE 状态选择
  * @retval 有效性检查结果
  * @note 总线共享时应配置为Released模式
  */
#define IS_SAI_BLOCK_TRISTATE_MANAGEMENT(STATE) (((STATE) == SAI_Output_NotReleased) ||\
                                                 ((STATE) == SAI_Output_Released))
/**
  * @}
  */

/** @defgroup SAI_Block_Fifo_Threshold FIFO阈值配置
  * @{
  * @brief 定义FIFO状态触发条件
  */
#define SAI_Threshold_FIFOEmpty           ((uint32_t)0x00000000) /*!< FIFO完全空时触发 */
#define SAI_FIFOThreshold_1QuarterFull    ((uint32_t)0x00000001) /*!< FIFO填充1/4时触发 */
#define SAI_FIFOThreshold_HalfFull        ((uint32_t)0x00000002)  /*!< FIFO半满时触发 */
#define SAI_FIFOThreshold_3QuartersFull   ((uint32_t)0x00000003) /*!< FIFO填充3/4时触发 */
#define SAI_FIFOThreshold_Full            ((uint32_t)0x00000004) /*!< FIFO完全满时触发 */
/**
  * @brief 阈值验证宏
  * @param THRESHOLD 阈值选择
  * @retval 有效性检查结果
  * @note 影响DMA请求和中断触发频率
  */
#define IS_SAI_BLOCK_FIFO_THRESHOLD(THRESHOLD) (((THRESHOLD) == SAI_Threshold_FIFOEmpty)         || \
                                                ((THRESHOLD) == SAI_FIFOThreshold_1QuarterFull)  || \
                                                ((THRESHOLD) == SAI_FIFOThreshold_HalfFull)      || \
                                                ((THRESHOLD) == SAI_FIFOThreshold_3QuartersFull) || \
                                                ((THRESHOLD) == SAI_FIFOThreshold_Full))
/**
  * @}
  */
  
/** @defgroup SAI_Block_Companding_Mode 压扩模式
  * @{
  * @brief 选择音频压缩扩展算法
  */
#define SAI_NoCompanding                  ((uint32_t)0x00000000) /*!< 禁用压扩处理 */
#define SAI_ULaw_1CPL_Companding          ((uint32_t)0x00008000) /*!< μ-law 13折线压缩（北美标准） */
#define SAI_ALaw_1CPL_Companding          ((uint32_t)0x0000C000) /*!< A-law 13折线压缩（欧洲标准） */
#define SAI_ULaw_2CPL_Companding          ((uint32_t)0x0000A000) /*!< μ-law 8折线压缩（特殊应用） */
#define SAI_ALaw_2CPL_Companding          ((uint32_t)0x0000E000) /*!< A-law 8折线压缩（特殊应用） */
/**
  * @brief 压扩模式验证宏
  * @param MODE 模式选择
  * @retval 有效性检查结果
  * @note 启用压扩时数据位宽强制为8位
  */
#define IS_SAI_BLOCK_COMPANDING_MODE(MODE)    (((MODE) == SAI_NoCompanding)        || \
                                              ((MODE) == SAI_ULaw_1CPL_Companding) || \
                                              ((MODE) == SAI_ALaw_1CPL_Companding) || \
                                              ((MODE) == SAI_ULaw_2CPL_Companding) || \
                                              ((MODE) == SAI_ALaw_2CPL_Companding))
/**
  * @}
  */

/**
  * @}
  */
/** @defgroup SAI_Block_Mute_Value 静音输出值配置
  * @{
  * @brief 定义静音状态下输出的数据值
  */
#define SAI_ZeroValue                     ((uint32_t)0x00000000) /*!< 输出零值（0x00000000） */
#define SAI_LastSentValue                 ((uint32_t)SAI_xCR2_MUTEVAL) /*!< 输出最后一次发送的数据值 */
/**
  * @brief 静音值验证宏
  * @param VALUE 静音值选择
  * @retval 有效性检查结果
  * @note 静音模式启用时持续输出指定值
  */
#define IS_SAI_BLOCK_MUTE_VALUE(VALUE)    (((VALUE) == SAI_ZeroValue)     || \
                                           ((VALUE) == SAI_LastSentValue))
/**
  * @}
  */

/** @defgroup SAI_Block_Mute_Frame_Counter 静音帧计数器
  * @{
  * @brief 配置静音持续时间（单位：音频帧）
  */
#define IS_SAI_BLOCK_MUTE_COUNTER(COUNTER) ((COUNTER) <= 63) /*!< 有效范围0-63帧 */
/**
  * @}
  */

/** @defgroup SAI_Block_Interrupts_Definition 中断源定义
  * @{
  * @brief 配置可启用的中断事件类型
  */
#define SAI_IT_OVRUDR                     ((uint32_t)SAI_xIMR_OVRUDRIE) /*!< 接收溢出/欠载中断 */
#define SAI_IT_MUTEDET                    ((uint32_t)SAI_xIMR_MUTEDETIE) /*!< 静音检测中断 */
#define SAI_IT_WCKCFG                     ((uint32_t)SAI_xIMR_WCKCFGIE) /*!< 错误时钟配置中断 */
#define SAI_IT_FREQ                       ((uint32_t)SAI_xIMR_FREQIE) /*!< 帧同步错误中断 */
#define SAI_IT_CNRDY                      ((uint32_t)SAI_xIMR_CNRDYIE) /*!< 编解码器就绪中断 */
#define SAI_IT_AFSDET                     ((uint32_t)SAI_xIMR_AFSDETIE) /*!< 帧同步提前检测中断 */
#define SAI_IT_LFSDET                     ((uint32_t)SAI_xIMR_LFSDETIE) /*!< 帧同步滞后检测中断 */
/**
  * @brief 中断类型验证宏
  * @param IT 中断类型选择
  * @retval 有效性检查结果
  * @note 多中断使能需使用位或(|)操作组合
  */
#define IS_SAI_BLOCK_CONFIG_IT(IT) (((IT) == SAI_IT_OVRUDR)  || \
                                    ((IT) == SAI_IT_MUTEDET) || \
                                    ((IT) == SAI_IT_WCKCFG)  || \
                                    ((IT) == SAI_IT_FREQ)    || \
                                    ((IT) == SAI_IT_CNRDY)   || \
                                    ((IT) == SAI_IT_AFSDET)  || \
                                    ((IT) == SAI_IT_LFSDET))
/**
  * @}
  */

/** @defgroup SAI_Block_Flags_Definition 状态标志定义
  * @{
  * @brief 表示模块当前状态的标志位
  */
#define SAI_FLAG_OVRUDR                   ((uint32_t)SAI_xSR_OVRUDR) /*!< 溢出/欠载标志 */
#define SAI_FLAG_MUTEDET                  ((uint32_t)SAI_xSR_MUTEDET) /*!< 静音检测标志 */
#define SAI_FLAG_WCKCFG                   ((uint32_t)SAI_xSR_WCKCFG) /*!< 错误时钟配置标志 */
#define SAI_FLAG_FREQ                     ((uint32_t)SAI_xSR_FREQ) /*!< 帧同步错误标志 */
#define SAI_FLAG_CNRDY                    ((uint32_t)SAI_xSR_CNRDY) /*!< 编解码器就绪标志 */
#define SAI_FLAG_AFSDET                   ((uint32_t)SAI_xSR_AFSDET) /*!< 过早帧同步检测标志 */
#define SAI_FLAG_LFSDET                   ((uint32_t)SAI_xSR_LFSDET) /*!< 延迟帧同步检测标志 */
/**
  * @}
  */
/** @defgroup SAI_Block_Flags_Management 标志位管理
  * @{
  */

/**
  * @brief 状态标志获取验证宏
  * @param FLAG 要查询的状态标志
  * @retval 有效性检查结果
  * @note 用于SDIO_GetFlagStatus()参数校验
  */
#define IS_SAI_BLOCK_GET_FLAG(FLAG) (((FLAG) == SAI_FLAG_OVRUDR)  || \
                                    ((FLAG) == SAI_FLAG_MUTEDET) || \
                                    ((FLAG) == SAI_FLAG_WCKCFG)  || \
                                    ((FLAG) == SAI_FLAG_FREQ)    || \
                                    ((FLAG) == SAI_FLAG_CNRDY)   || \
                                    ((FLAG) == SAI_FLAG_AFSDET)  || \
                                    ((FLAG) == SAI_FLAG_LFSDET))

/**
  * @brief 状态标志清除验证宏
  * @param FLAG 要清除的状态标志
  * @retval 有效性检查结果
  * @note 用于SDIO_ClearFlag()参数校验
  */
#define IS_SAI_BLOCK_CLEAR_FLAG(FLAG) (((FLAG) == SAI_FLAG_OVRUDR)  || \
                                       ((FLAG) == SAI_FLAG_MUTEDET) || \
                                       ((FLAG) == SAI_FLAG_WCKCFG)  || \
                                       ((FLAG) == SAI_FLAG_FREQ)    || \
                                       ((FLAG) == SAI_FLAG_CNRDY)   || \
                                       ((FLAG) == SAI_FLAG_AFSDET)  || \
                                       ((FLAG) == SAI_FLAG_LFSDET))
/**
  * @}
  */

/** @defgroup SAI_Block_Fifo_Status_Level FIFO状态级别
  * @{
  * @brief 定义FIFO缓冲区的填充状态
  */
#define SAI_FIFOStatus_Empty              ((uint32_t)0x00000000)  /*!< FIFO完全空（0数据） */
#define SAI_FIFOStatus_Less1QuarterFull   ((uint32_t)0x00010000)  /*!< FIFO填充量＜1/4 */
#define SAI_FIFOStatus_1QuarterFull       ((uint32_t)0x00020000)  /*!< FIFO填充≥1/4 */
#define SAI_FIFOStatus_HalfFull           ((uint32_t)0x00030000)  /*!< FIFO填充≥1/2 */
#define SAI_FIFOStatus_3QuartersFull      ((uint32_t)0x00040000)  /*!< FIFO填充≥3/4 */
#define SAI_FIFOStatus_Full               ((uint32_t)0x00050000)  /*!< FIFO完全满 */

/**
  * @brief FIFO状态验证宏
  * @param STATUS 状态级别
  * @retval 有效性检查结果
  * @note 用于获取当前FIFO的填充状态
  */
#define IS_SAI_BLOCK_FIFO_STATUS(STATUS) (((STATUS) == SAI_FIFOStatus_Less1QuarterFull ) || \
                                          ((STATUS) == SAI_FIFOStatus_HalfFull)          || \
                                          ((STATUS) == SAI_FIFOStatus_1QuarterFull)      || \
                                          ((STATUS) == SAI_FIFOStatus_3QuartersFull)     || \
                                          ((STATUS) == SAI_FIFOStatus_Full)              || \
                                          ((STATUS) == SAI_FIFOStatus_Empty))
/**
  * @}
  */

/**
  * @}
  */

  
/**
  * @}
  */
/* 导出宏 ------------------------------------------------------------*/
/* 导出函数 --------------------------------------------------------*/ 

/** @addtogroup SAI_Exported_Functions
  * @{
  */

/* 复位配置函数 *******************************************************/
/**
  * @brief  复位SAI外设所有寄存器到默认值
  * @param  SAIx 指定SAI实例，SAI1或SAI2
  * @note   会关闭所有时钟和中断，清除所有配置
  */
void SAI_DeInit(SAI_TypeDef* SAIx);

/* 初始化与配置函数组 *************************************************/
/**
  * @brief  主初始化函数，配置SAI工作模式
  * @param  SAI_Block_x 指定SAI子模块(Block_A/Block_B)
  * @param  SAI_InitStruct 初始化结构体指针
  * @note   需配合SAI_StructInit()初始化结构体
  */
void SAI_Init(SAI_Block_TypeDef* SAI_Block_x, SAI_InitTypeDef* SAI_InitStruct);

/**
  * @brief  帧结构初始化函数
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_FrameInitStruct 帧配置结构体指针
  * @note   配置帧同步信号参数
  */
void SAI_FrameInit(SAI_Block_TypeDef* SAI_Block_x, SAI_FrameInitTypeDef* SAI_FrameInitStruct);

/**
  * @brief  时隙结构初始化函数
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_SlotInitStruct 时隙配置结构体指针
  * @note   配置多时隙传输参数
  */
void SAI_SlotInit(SAI_Block_TypeDef* SAI_Block_x, SAI_SlotInitTypeDef* SAI_SlotInitStruct);

/**
  * @brief  初始化主配置结构体为默认值
  * @param  SAI_InitStruct 主配置结构体指针
  */
void SAI_StructInit(SAI_InitTypeDef* SAI_InitStruct);

/**
  * @brief  初始化帧配置结构体为默认值
  * @param  SAI_FrameInitStruct 帧配置结构体指针
  */
void SAI_FrameStructInit(SAI_FrameInitTypeDef* SAI_FrameInitStruct);

/**
  * @brief  初始化时隙配置结构体为默认值
  * @param  SAI_SlotInitStruct 时隙配置结构体指针
  */
void SAI_SlotStructInit(SAI_SlotInitTypeDef* SAI_SlotInitStruct);

/* 模块控制函数 *******************************************************/
/**
  * @brief  启用/禁用SAI模块
  * @param  SAI_Block_x 指定SAI子模块
  * @param  NewState ENABLE/DISABLE
  * @note   启用前需完成所有配置
  */
void SAI_Cmd(SAI_Block_TypeDef* SAI_Block_x, FunctionalState NewState);

/**
  * @brief  配置单声道/立体声模式
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_Mono_StreoMode 模式选择 @ref SAI_Mono_Streo_Mode
  */
void SAI_MonoModeConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_Mono_StreoMode);

/**
  * @brief  配置三态管理模式
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_TRIState 三态选择 @ref SAI_TRIState_Management
  */
void SAI_TRIStateConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_TRIState);

/**
  * @brief  配置压扩模式
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_CompandingMode 压扩算法选择 @ref SAI_Block_Companding_Mode
  * @note   启用压扩时自动设置数据位宽为8位
  */
void SAI_CompandingModeConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_CompandingMode);

/**
  * @brief  启用/禁用静音模式
  * @param  SAI_Block_x 指定SAI子模块
  * @param  NewState ENABLE/DISABLE
  * @note   静音时持续输出指定值
  */
void SAI_MuteModeCmd(SAI_Block_TypeDef* SAI_Block_x, FunctionalState NewState);

/**
  * @brief  配置静音输出值
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_MuteValue 静音值 @ref SAI_Block_Mute_Value
  */
void SAI_MuteValueConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_MuteValue);

/**
  * @brief  配置静音持续时间
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_MuteCounter 静音帧数(0-63)
  * @note   达到指定帧数后自动退出静音
  */
void SAI_MuteFrameCounterConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_MuteCounter);

/**
  * @brief  清空FIFO缓冲区
  * @param  SAI_Block_x 指定SAI子模块
  * @note   丢弃所有未处理数据，重置读写指针
  */
void SAI_FlushFIFO(SAI_Block_TypeDef* SAI_Block_x);
/** @addtogroup SAI_Exported_Functions
  * @{
  */

/* 数据传输函数组 *******************************************************/
/**
  * @brief  向FIFO写入数据
  * @param  SAI_Block_x 指定SAI子模块(Block_A/Block_B)
  * @param  Data 待发送的32位数据
  * @note   数据实际位宽由SAI_DataSize配置决定，高位截断
  */
void SAI_SendData(SAI_Block_TypeDef* SAI_Block_x, uint32_t Data);

/**
  * @brief  从FIFO读取数据
  * @param  SAI_Block_x 指定SAI子模块
  * @retval 接收到的32位数据
  * @note   读取后自动递减数据计数器
  */
uint32_t SAI_ReceiveData(SAI_Block_TypeDef* SAI_Block_x);

/* DMA传输管理函数组 ***************************************************/
/**
  * @brief  启用/禁用DMA传输
  * @param  SAI_Block_x 指定SAI子模块
  * @param  NewState ENABLE/DISABLE
  * @note   启用前需配置DMA控制器和对应通道
  */
void SAI_DMACmd(SAI_Block_TypeDef* SAI_Block_x, FunctionalState NewState);

/* 中断与标志管理函数组 ************************************************/
/**
  * @brief  配置中断使能
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_IT 中断类型 @ref SAI_Block_Interrupts_Definition
  * @param  NewState ENABLE/DISABLE
  */
void SAI_ITConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_IT, FunctionalState NewState);

/**
  * @brief  获取状态标志
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_FLAG 标志类型 @ref SAI_Block_Flags_Definition
  * @retval SET/RESET
  * @note   部分标志需手动清除
  */
FlagStatus SAI_GetFlagStatus(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_FLAG);

/**
  * @brief  清除状态标志
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_FLAG 标志类型 @ref SAI_Block_Flags_Definition
  * @note   仅可清除写1清零的标志位
  */
void SAI_ClearFlag(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_FLAG);

/**
  * @brief  获取中断状态
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_IT 中断类型
  * @retval SET/RESET
  * @note   需同时满足中断使能和标志置位
  */
ITStatus SAI_GetITStatus(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_IT);

/**
  * @brief  清除中断挂起位
  * @param  SAI_Block_x 指定SAI子模块
  * @param  SAI_IT 中断类型
  */
void SAI_ClearITPendingBit(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_IT);

/**
  * @brief  获取模块使能状态
  * @param  SAI_Block_x 指定SAI子模块
  * @retval ENABLE/DISABLE
  */
FunctionalState SAI_GetCmdStatus(SAI_Block_TypeDef* SAI_Block_x);

/**
  * @brief  获取FIFO当前状态
  * @param  SAI_Block_x 指定SAI子模块
  * @retval FIFO状态 @ref SAI_Block_Fifo_Status_Level
  * @note   用于判断数据缓冲区填充量
  */
uint32_t SAI_GetFIFOStatus(SAI_Block_TypeDef* SAI_Block_x);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_SAI_H */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

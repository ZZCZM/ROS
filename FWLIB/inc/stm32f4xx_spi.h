/**
  ******************************************************************************
  * @file    stm32f4xx_spi.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the SPI 
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
#ifndef __STM32F4xx_SPI_H
#define __STM32F4xx_SPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup SPI
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/**
 * @brief SPI初始化配置结构体
 * @details 包含SPI外设所有可配置参数，用于SPI_Init()函数初始化
 *          需根据具体硬件连接和通信需求配置各成员变量
 */
typedef struct
{
  uint16_t SPI_Direction;           /*!< 数据传输方向配置:
                                         @arg SPI_Direction_2Lines_FullDuplex: 全双工
                                         @arg SPI_Direction_2Lines_RxOnly: 双线只接收
                                         @arg SPI_Direction_1Line_Rx: 单线接收
                                         @arg SPI_Direction_1Line_Tx: 单线发送 */

  uint16_t SPI_Mode;                /*!< 主从模式选择:
                                         @arg SPI_Mode_Master: 主机模式(生成SCK信号)
                                         @arg SPI_Mode_Slave: 从机模式 */

  uint16_t SPI_DataSize;            /*!< 数据帧长度:
                                         @arg SPI_DataSize_8b:  8位数据帧
                                         @arg SPI_DataSize_16b: 16位数据帧 */

  uint16_t SPI_CPOL;                /*!< 时钟极性:
                                         @arg SPI_CPOL_Low:  空闲时SCK低电平
                                         @arg SPI_CPOL_High: 空闲时SCK高电平 */

  uint16_t SPI_CPHA;                /*!< 时钟相位:
                                         @arg SPI_CPHA_1Edge: 第1个时钟边沿采样
                                         @arg SPI_CPHA_2Edge: 第2个时钟边沿采样 */

  uint16_t SPI_NSS;                 /*!< 片选管理方式:
                                         @arg SPI_NSS_Hard: 硬件自动管理NSS引脚
                                         @arg SPI_NSS_Soft: 软件控制SSI位 */

  uint16_t SPI_BaudRatePrescaler;   /*!< 波特率分频系数:
                                         @arg SPI_BaudRatePrescaler_2~256 
                                         @note 实际波特率=APB时钟/(分频系数*2) */

  uint16_t SPI_FirstBit;            /*!< 数据传输位序:
                                         @arg SPI_FirstBit_MSB: 高位先传
                                         @arg SPI_FirstBit_LSB: 低位先传 */

  uint16_t SPI_CRCPolynomial;       /*!< CRC多项式寄存器值(7~65535)
                                         @note 仅CRC计算模式启用时有效 */
}SPI_InitTypeDef;

/**
 * @brief I2S初始化配置结构体  
 * @details 配置I2S音频协议相关参数，需与外部编解码器设置匹配
 */
typedef struct
{
  uint16_t I2S_Mode;         /*!< I2S工作模式:
                                  @arg I2S_Mode_SlaveTx: 从发送模式
                                  @arg I2S_Mode_SlaveRx: 从接收模式
                                  @arg I2S_Mode_MasterTx: 主发送模式 
                                  @arg I2S_Mode_MasterRx: 主接收模式 */

  uint16_t I2S_Standard;     /*!< 音频协议标准:
                                  @arg I2S_Standard_Phillips: 飞利浦标准
                                  @arg I2S_Standard_MSB: MSB对齐格式
                                  @arg I2S_Standard_LSB: LSB对齐格式
                                  @arg I2S_Standard_PCMShort: PCM短帧同步 */

  uint16_t I2S_DataFormat;   /*!< 数据位宽配置:
                                  @arg I2S_DataFormat_16b: 16位数据
                                  @arg I2S_DataFormat_16bextend: 16位扩展为32
                                  @arg I2S_DataFormat_24b: 24位数据
                                  @arg I2S_DataFormat_32b: 32位数据 */

  uint16_t I2S_MCLKOutput;   /*!< 主时钟输出控制:
                                  @arg I2S_MCLKOutput_Enable: 输出MCK时钟
                                  @arg I2S_MCLKOutput_Disable: 关闭MCK输出 */

  uint32_t I2S_AudioFreq;    /*!< 音频采样频率(Hz):
                                  @arg I2S_AudioFreq_8k~192k 
                                  @note 实际频率=I2S时钟/(32*通道位宽*2) */

  uint16_t I2S_CPOL;         /*!< 空闲状态时钟极性:
                                  @arg I2S_CPOL_Low: 空闲低电平
                                  @arg I2S_CPOL_High: 空闲高电平 */
}I2S_InitTypeDef;
/**
 * @defgroup SPI外设验证宏组
 * @brief SPI/I2S外设实例有效性验证工具集
 */

/** 
 * @def IS_SPI_ALL_PERIPH
 * @brief 验证基础SPI外设实例
 * @param PERIPH 外设指针
 * @retval 有效返回1，否则0
 * @note 覆盖全系列SPI1-SPI6，具体可用范围依赖芯片型号
 *       例：F401系列仅支持SPI1-SPI3
 */
#define IS_SPI_ALL_PERIPH(PERIPH) (((PERIPH) == SPI1) || \
                                   ((PERIPH) == SPI2) || \
                                   ((PERIPH) == SPI3) || \
                                   ((PERIPH) == SPI4) || \
                                   ((PERIPH) == SPI5) || \
                                   ((PERIPH) == SPI6))

/**
 * @def IS_SPI_ALL_PERIPH_EXT
 * @brief 验证SPI及扩展I2S外设实例
 * @details 包含I2S2ext/I2S3ext等扩展模块
 *          适用于支持多音频通道的增强型芯片
 */
#define IS_SPI_ALL_PERIPH_EXT(PERIPH) (((PERIPH) == SPI1)    || \
                                       ((PERIPH) == SPI2)    || \
                                       ((PERIPH) == SPI3)    || \
                                       ((PERIPH) == SPI4)    || \
                                       ((PERIPH) == SPI5)    || \
                                       ((PERIPH) == SPI6)    || \
                                       ((PERIPH) == I2S2ext) || \
                                       ((PERIPH) == I2S3ext))

/** 
 * @def IS_SPI_23_PERIPH
 * @brief 验证SPI2/SPI3基础外设
 * @note 用于需要单独处理SPI2/3的场景，如DMA通道分配冲突时
 */
#define IS_SPI_23_PERIPH(PERIPH)  (((PERIPH) == SPI2) || \
                                   ((PERIPH) == SPI3))

/**
 * @def IS_SPI_23_PERIPH_EXT
 * @brief 验证SPI2/3及其扩展I2S模块
 * @details 包含I2S2ext/I2S3ext，用于多声道音频系统配置
 */
#define IS_SPI_23_PERIPH_EXT(PERIPH)  (((PERIPH) == SPI2)    || \
                                       ((PERIPH) == SPI3)    || \
                                       ((PERIPH) == I2S2ext) || \
                                       ((PERIPH) == I2S3ext))

/**
 * @def IS_I2S_EXT_PERIPH
 * @brief 专用I2S扩展模块验证
 * @note 仅检查I2S2ext/I2S3ext，用于音频从模式配置
 */
#define IS_I2S_EXT_PERIPH(PERIPH)  (((PERIPH) == I2S2ext) || \
                                    ((PERIPH) == I2S3ext))
/** 
  * @defgroup SPI_data_direction 
  * @brief SPI数据传输方向配置组
  * @details 设置SPI通信线路的工作模式，决定数据传输通道的数量和方向
  */
#define SPI_Direction_2Lines_FullDuplex ((uint16_t)0x0000)  /*!< 双线全双工模式，独立MOSI/MISO引脚 */
#define SPI_Direction_2Lines_RxOnly     ((uint16_t)0x0400)  /*!< 双线只接收模式，MISO引脚单向传输 */
#define SPI_Direction_1Line_Rx          ((uint16_t)0x8000)  /*!< 单线接收模式，数据通过MISO传输 */
#define SPI_Direction_1Line_Tx          ((uint16_t)0xC000)  /*!< 单线发送模式，数据通过MOSI传输 */
#define IS_SPI_DIRECTION_MODE(MODE) (/*!< 方向模式验证，确保为0x0000/0x0400/0x8000/0xC000 */ \
                                     ((MODE) == SPI_Direction_2Lines_FullDuplex) || \
                                     ((MODE) == SPI_Direction_2Lines_RxOnly) || \
                                     ((MODE) == SPI_Direction_1Line_Rx) || \
                                     ((MODE) == SPI_Direction_1Line_Tx))

/**
  * @defgroup SPI_mode 
  * @brief SPI主从模式配置组
  * @details 确定SPI在总线中的角色，影响SCK时钟生成和NSS信号控制
  */
#define SPI_Mode_Master                 ((uint16_t)0x0104)  /*!< 主机模式，控制SCK时钟和NSS信号 */
#define SPI_Mode_Slave                  ((uint16_t)0x0000)  /*!< 从机模式，接收外部SCK时钟 */
#define IS_SPI_MODE(MODE) (/*!< 模式有效性检查，排除非法模式值 */ \
                           ((MODE) == SPI_Mode_Master) || \
                           ((MODE) == SPI_Mode_Slave))

/**
  * @defgroup SPI_data_size 
  * @brief 数据帧长度配置组
  * @details 设置单次传输的数据位宽，影响DR寄存器的位宽处理
  */
#define SPI_DataSize_16b                ((uint16_t)0x0800)  /*!< 16位数据帧，需使用半字访问DR寄存器 */
#define SPI_DataSize_8b                 ((uint16_t)0x0000)  /*!< 8位数据帧，使用字节访问DR寄存器 */
#define IS_SPI_DATASIZE(DATASIZE) (/*!< 数据位宽合法性验证 */ \
                                   ((DATASIZE) == SPI_DataSize_16b) || \
                                   ((DATASIZE) == SPI_DataSize_8b))

/**
  * @defgroup SPI_Clock_Polarity 
  * @brief 时钟极性配置组
  * @details 定义SCK时钟在空闲状态的电平，需与从设备设置严格同步
  */
#define SPI_CPOL_Low                    ((uint16_t)0x0000)  /*!< 空闲时SCK保持低电平 */
#define SPI_CPOL_High                   ((uint16_t)0x0002)  /*!< 空闲时SCK保持高电平 */
#define IS_SPI_CPOL(CPOL) (/*!< 极性参数检查，防止非法值写入CR1寄存器 */ \
                           ((CPOL) == SPI_CPOL_Low) || \
                           ((CPOL) == SPI_CPOL_High))

/**
  * @defgroup SPI_Clock_Phase 
  * @brief 时钟相位配置组
  * @details 决定数据采样的时钟边沿，影响数据建立/保持时间的计算
  */
#define SPI_CPHA_1Edge                  ((uint16_t)0x0000)  /*!< 第1个时钟边沿采样（上升沿/下降沿取决于CPOL） */
#define SPI_CPHA_2Edge                  ((uint16_t)0x0001)  /*!< 第2个时钟边沿采样 */
#define IS_SPI_CPHA(CPHA) (/*!< 相位参数合法性验证 */ \
                           ((CPHA) == SPI_CPHA_1Edge) || \
                           ((CPHA) == SPI_CPHA_2Edge))

/**
  * @defgroup SPI_Slave_Select_management 
  * @brief 片选信号管理配置组
  * @details 控制NSS引脚的工作模式，决定片选信号由硬件自动管理还是软件控制
  */
#define SPI_NSS_Soft                    ((uint16_t)0x0200)  /*!< 软件NSS模式，通过SPI_CR1的SSI位控制 */
#define SPI_NSS_Hard                    ((uint16_t)0x0000)  /*!< 硬件NSS模式，自动管理NSS引脚电平 */
#define IS_SPI_NSS(NSS) (/*!< NSS模式有效性检查 */ \
                         ((NSS) == SPI_NSS_Soft) || \
                         ((NSS) == SPI_NSS_Hard))
/**
  * @defgroup SPI_BaudRate_Prescaler 
  * @brief SPI波特率分频系数配置组
  * @details 通过APB时钟分频产生SCK信号，计算公式：
  *          SCK频率 = fAPB / (2 * Prescaler)
  *          实际分频系数 = 2^(Prescaler数值+1)
  */
#define SPI_BaudRatePrescaler_2         ((uint16_t)0x0000)  /*!< 分频系数2，对应SCK=APB/2 */
#define SPI_BaudRatePrescaler_4         ((uint16_t)0x0008)  /*!< 分频系数4，适合高速FLASH通信 */
#define SPI_BaudRatePrescaler_8         ((uint16_t)0x0010)  /*!< 分频系数8，平衡速度与稳定性 */
#define SPI_BaudRatePrescaler_16        ((uint16_t)0x0018)  /*!< 分频系数16，常用默认配置 */
#define SPI_BaudRatePrescaler_32        ((uint16_t)0x0020)  /*!< 分频系数32，中低速传感器应用 */
#define SPI_BaudRatePrescaler_64        ((uint16_t)0x0028)  /*!< 分频系数64，抑制信号反射场景 */
#define SPI_BaudRatePrescaler_128       ((uint16_t)0x0030)  /*!< 分频系数128，超低速调试模式 */
#define SPI_BaudRatePrescaler_256       ((uint16_t)0x0038)  /*!< 分频系数256，仅应急场景使用 */
#define IS_SPI_BAUDRATE_PRESCALER(PRESCALER) (/*!< 分频参数校验，防止无效分频值配置 */ \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_2) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_4) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_8) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_16) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_32) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_64) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_128) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_256))

/**
  * @defgroup SPI_MSB_LSB_transmission 
  * @brief 数据传输位序配置组
  * @details 影响数据寄存器的移位方向，需与通信双方设备设置一致
  */
#define SPI_FirstBit_MSB                ((uint16_t)0x0000)  /*!< 高位(MSB)先传，符合多数SPI设备协议 */
#define SPI_FirstBit_LSB                ((uint16_t)0x0080)  /*!< 低位(LSB)先传，用于特定IC设计 */
#define IS_SPI_FIRST_BIT(BIT) (/*!< 位序参数校验，避免配置冲突 */ \
                               ((BIT) == SPI_FirstBit_MSB) || \
                               ((BIT) == SPI_FirstBit_LSB))

/**
  * @defgroup SPI_I2S_Mode 
  * @brief I2S主从模式及传输方向配置组
  * @details 配置I2S总线角色和音频流方向，需与编解码器设置匹配
  */
#define I2S_Mode_SlaveTx                ((uint16_t)0x0000)  /*!< 从发送模式：外部提供WS/SCK，发送音频数据 */
#define I2S_Mode_SlaveRx                ((uint16_t)0x0100)  /*!< 从接收模式：外部控制时钟，接收音频流 */
#define I2S_Mode_MasterTx               ((uint16_t)0x0200)  /*!< 主发送模式：生成WS/SCK/MCK，驱动DAC等设备 */
#define I2S_Mode_MasterRx               ((uint16_t)0x0300)  /*!< 主接收模式：控制时钟，采集ADC数据 */
#define IS_I2S_MODE(MODE) (/*!< I2S模式有效性校验 */ \
                           ((MODE) == I2S_Mode_SlaveTx) || \
                           ((MODE) == I2S_Mode_SlaveRx) || \
                           ((MODE) == I2S_Mode_MasterTx)|| \
                           ((MODE) == I2S_Mode_MasterRx))

/**
  * @}
  */
  /**
  * @defgroup SPI_I2S_Standard 
  * @brief I2S协议标准配置组
  * @details 定义音频数据的帧结构格式，需与外部编解码器配置匹配
  */
#define I2S_Standard_Phillips           ((uint16_t)0x0000)  /*!< 飞利浦标准：左右声道交替传输，WS为通道选择 */
#define I2S_Standard_MSB                ((uint16_t)0x0010)  /*!< MSB对齐格式：适用于早期PCM设备 */
#define I2S_Standard_LSB                ((uint16_t)0x0020)  /*!< LSB对齐格式：用于特殊DSP应用场景 */
#define I2S_Standard_PCMShort           ((uint16_t)0x0030)  /*!< PCM短帧同步：1个WS周期传输1个样本 */
#define I2S_Standard_PCMLong            ((uint16_t)0x00B0)  /*!< PCM长帧同步：用于TDM多通道传输 */
#define IS_I2S_STANDARD(STANDARD) (/*!< 协议标准有效性检查，防止非法格式设置 */ \
                                   ((STANDARD) == I2S_Standard_Phillips) || \
                                   ((STANDARD) == I2S_Standard_MSB) || \
                                   ((STANDARD) == I2S_Standard_LSB) || \
                                   ((STANDARD) == I2S_Standard_PCMShort) || \
                                   ((STANDARD) == I2S_Standard_PCMLong))

/**
  * @defgroup SPI_I2S_Data_Format 
  * @brief I2S数据位宽配置组
  * @details 设置音频样本的量化精度，影响DR寄存器的填充方式
  */
#define I2S_DataFormat_16b              ((uint16_t)0x0000)  /*!< 16位标准精度，每个通道传输16bit数据 */
#define I2S_DataFormat_16bextended      ((uint16_t)0x0001)  /*!< 16位扩展模式：32位时钟周期传输16bit数据 */
#define I2S_DataFormat_24b              ((uint16_t)0x0003)  /*!< 24位高精度：使用双字对齐传输 */
#define I2S_DataFormat_32b              ((uint16_t)0x0005)  /*!< 32位模式：用于32位DSP处理器对接 */
#define IS_I2S_DATA_FORMAT(FORMAT) (/*!< 数据格式合法性验证 */ \
                                    ((FORMAT) == I2S_DataFormat_16b) || \
                                    ((FORMAT) == I2S_DataFormat_16bextended) || \
                                    ((FORMAT) == I2S_DataFormat_24b) || \
                                    ((FORMAT) == I2S_DataFormat_32b))

/**
  * @defgroup SPI_I2S_MCLK_Output 
  * @brief I2S主时钟输出控制组
  * @details 管理MCK引脚的主时钟输出，用于同步外部音频设备
  */
#define I2S_MCLKOutput_Enable           ((uint16_t)0x0200)  /*!< 使能MCK输出：输出频率=256*fs (fs为采样率) */
#define I2S_MCLKOutput_Disable          ((uint16_t)0x0000)  /*!< 禁用MCK输出：节省功耗模式 */
#define IS_I2S_MCLK_OUTPUT(OUTPUT) (/*!< 主时钟输出配置校验 */ \
                                    ((OUTPUT) == I2S_MCLKOutput_Enable) || \
                                    ((OUTPUT) == I2S_MCLKOutput_Disable))

/**
  * @defgroup SPI_I2S_Audio_Frequency 
  * @brief I2S音频采样率配置组
  * @details 设置音频流采样频率，实际频率由I2S时钟分频计算得出
  */
#define I2S_AudioFreq_192k               ((uint32_t)192000)  /*!< 192kHz 高保真采样率 */
#define I2S_AudioFreq_96k                ((uint32_t)96000)   /*!< 96kHz 专业音频设备标准 */
#define I2S_AudioFreq_48k                ((uint32_t)48000)   /*!< 48kHz 通用数字音频 */
#define I2S_AudioFreq_44k                ((uint32_t)44100)   /*!< 44.1kHz CD音质标准 */
#define I2S_AudioFreq_32k                ((uint32_t)32000)   /*!< 32kHz 语音通信应用 */
#define I2S_AudioFreq_22k                ((uint32_t)22050)   /*!< 22.05kHz 低质量音频流 */
#define I2S_AudioFreq_16k                ((uint32_t)16000)   /*!< 16kHz 电话语音质量 */
#define I2S_AudioFreq_11k                ((uint32_t)11025)   /*!< 11.025kHz 低带宽场景 */
#define I2S_AudioFreq_8k                 ((uint32_t)8000)    /*!< 8kHz 传统电话系统 */
#define I2S_AudioFreq_Default            ((uint32_t)2)       /*!< 默认模式：使用外部时钟源输入 */

#define IS_I2S_AUDIO_FREQ(FREQ) (/*!< 采样率范围检查，支持8k-192k或外部时钟模式 */ \
                                 (((FREQ) >= I2S_AudioFreq_8k) && \
                                 ((FREQ) <= I2S_AudioFreq_192k)) || \
                                 ((FREQ) == I2S_AudioFreq_Default))
/**
  * @defgroup SPI_I2S_Clock_Polarity 
  * @brief I2S时钟极性配置组
  * @details 定义WS信号无效时的SCK时钟状态，需与编解码器设置同步
  */
#define I2S_CPOL_Low                    ((uint16_t)0x0000)  /*!< WS无效时SCK保持低电平，标准I2S配置 */
#define I2S_CPOL_High                   ((uint16_t)0x0008)  /*!< WS无效时SCK保持高电平，特殊DSP应用 */
#define IS_I2S_CPOL(CPOL) (/*!< 极性参数检查，防止无效时钟配置 */ \
                           ((CPOL) == I2S_CPOL_Low) || \
                           ((CPOL) == I2S_CPOL_High))

/**
  * @defgroup SPI_I2S_DMA_transfer_requests 
  * @brief DMA传输请求配置组
  * @details 控制DMA控制器在特定事件触发传输，实现高效数据搬移
  */
#define SPI_I2S_DMAReq_Tx               ((uint16_t)0x0002)  /*!< 发送DMA请求：TXE事件触发DMA传输 */
#define SPI_I2S_DMAReq_Rx               ((uint16_t)0x0001)  /*!< 接收DMA请求：RXNE事件触发DMA传输 */
#define IS_SPI_I2S_DMAREQ(DMAREQ) (/*!< DMA请求有效性校验，支持0x0001/0x0002组合 */ \
                                   (((DMAREQ) & (uint16_t)0xFFFC) == 0x00) && ((DMAREQ) != 0x00))

/**
  * @defgroup SPI_NSS_internal_software_management 
  * @brief NSS信号软件控制组
  * @details 当使用软件NSS模式时，通过寄存器位替代物理引脚控制
  */
#define SPI_NSSInternalSoft_Set         ((uint16_t)0x0100)  /*!< 强制NSS为低电平（选中状态） */
#define SPI_NSSInternalSoft_Reset       ((uint16_t)0xFEFF)  /*!< 恢复NSS为高电平（释放设备） */
#define IS_SPI_NSS_INTERNAL(INTERNAL) (/*!< 软件NSS模式参数校验 */ \
                                       ((INTERNAL) == SPI_NSSInternalSoft_Set) || \
                                       ((INTERNAL) == SPI_NSSInternalSoft_Reset))

/**
  * @defgroup SPI_CRC_Transmit_Receive 
  * @brief CRC校验方向配置组
  * @details 启用指定方向的CRC校验（需提前配置CRC多项式寄存器）
  */
#define SPI_CRC_Tx                      ((uint8_t)0x00)  /*!< 发送CRC校验：自动附加CRC码到数据末尾 */
#define SPI_CRC_Rx                      ((uint8_t)0x01)  /*!< 接收CRC校验：自动验证接收数据CRC值 */
#define IS_SPI_CRC(CRC) (/*!< CRC方向参数有效性检查 */ \
                        ((CRC) == SPI_CRC_Tx) || ((CRC) == SPI_CRC_Rx))

/**
  * @defgroup SPI_direction_transmit_receive 
  * @brief 单向传输模式配置组
  * @details 在半双工模式下强制指定传输方向，需与SPI_Direction模式配合使用
  */
#define SPI_Direction_Rx                ((uint16_t)0xBFFF)  /*!< 强制接收模式，关闭发送电路 */
#define SPI_Direction_Tx                ((uint16_t)0x4000)  /*!< 强制发送模式，关闭接收电路 */
#define IS_SPI_DIRECTION(DIRECTION) (/*!< 方向强制模式参数检查 */ \
                                     ((DIRECTION) == SPI_Direction_Rx) || \
                                     ((DIRECTION) == SPI_Direction_Tx))

/**
  * @defgroup SPI_I2S_interrupts_definition 
  * @brief 中断类型定义组
  * @details 配置使能的中断类型，需配合NVIC设置中断优先级
  */
#define SPI_I2S_IT_TXE                  ((uint8_t)0x71)  /*!< 发送缓冲区空中断（可写入新数据） */
#define SPI_I2S_IT_RXNE                 ((uint8_t)0x60)  /*!< 接收缓冲区非空中断（可读取数据） */
#define SPI_I2S_IT_ERR                  ((uint8_t)0x50)  /*!< 主错误中断：包含OVR/UDR/MODF等错误 */
#define I2S_IT_UDR                      ((uint8_t)0x53)  /*!< I2S下溢错误：发送速率超过采样率 */
#define SPI_I2S_IT_TIFRFE               ((uint8_t)0x58)  /*!< TI帧格式错误中断 */
#define IS_SPI_I2S_CONFIG_IT(IT) (/*!< 可配置中断类型检查（用户可主动启用的中断）*/ \
                                  ((IT) == SPI_I2S_IT_TXE) || \
                                  ((IT) == SPI_I2S_IT_RXNE) || \
                                  ((IT) == SPI_I2S_IT_ERR))
                                  
/* 以下为自动触发的标志型中断 */
#define SPI_I2S_IT_OVR                  ((uint8_t)0x56)  /*!< 溢出错误：接收缓冲区未及时读取 */
#define SPI_IT_MODF                     ((uint8_t)0x55)  /*!< 模式错误：NSS引脚冲突 */
#define SPI_IT_CRCERR                   ((uint8_t)0x54)  /*!< CRC校验失败 */
/**
  * @defgroup SPI_I2S_Interrupt_Clear_Flags
  * @brief 中断清除标志定义组
  * @details 指定需要通过软件手动清除的中断标志类型
  */
#define IS_SPI_I2S_CLEAR_IT(IT) (((IT) == SPI_IT_CRCERR))  /*!< 仅CRC错误标志需手动清除 */

/**
  * @defgroup SPI_I2S_Interrupt_Get_Flags
  * @brief 中断状态获取标志组
  * @details 允许查询的所有中断类型，包含主动触发和错误标志
  */
#define IS_SPI_I2S_GET_IT(IT) (                         \
    ((IT) == SPI_I2S_IT_RXNE) || ((IT) == SPI_I2S_IT_TXE) || \
    ((IT) == SPI_IT_CRCERR)  || ((IT) == SPI_IT_MODF) || \
    ((IT) == SPI_I2S_IT_OVR) || ((IT) == I2S_IT_UDR) || \
    ((IT) == SPI_I2S_IT_TIFRFE))                        /*!< 覆盖所有可查询中断类型 */

/**
  * @defgroup SPI_I2S_flags_definition 
  * @brief 硬件状态标志定义组
  * @details 反映SPI/I2S外设实时工作状态的寄存器标志位
  */
#define SPI_I2S_FLAG_RXNE               ((uint16_t)0x0001)  /*!< 接收缓冲区非空：存在待读取数据 */
#define SPI_I2S_FLAG_TXE                ((uint16_t)0x0002)  /*!< 发送缓冲区空：可写入新数据 */
#define I2S_FLAG_CHSIDE                 ((uint16_t)0x0004)  /*!< 当前通道标志：0=左声道，1=右声道（仅I2S） */
#define I2S_FLAG_UDR                    ((uint16_t)0x0008)  /*!< 下溢错误：发送速度超过音频采样率 */
#define SPI_FLAG_CRCERR                 ((uint16_t)0x0010)  /*!< CRC校验失败：检测到数据传输错误 */
#define SPI_FLAG_MODF                   ((uint16_t)0x0020)  /*!< 模式错误：NSS引脚电平与模式冲突 */
#define SPI_I2S_FLAG_OVR                ((uint16_t)0x0040)  /*!< 溢出错误：接收数据未及时读取 */
#define SPI_I2S_FLAG_BSY                ((uint16_t)0x0080)  /*!< 总线忙状态：正在进行数据传输 */
#define SPI_I2S_FLAG_TIFRFE             ((uint16_t)0x0100)  /*!< TI帧格式错误：帧长度不匹配 */

/**
  * @defgroup SPI_I2S_Clear_Flag_Checks
  * @brief 可清除标志校验组
  * @details 指定允许通过写操作清除的状态标志类型
  */
#define IS_SPI_I2S_CLEAR_FLAG(FLAG) (((FLAG) == SPI_FLAG_CRCERR))  /*!< 仅CRC错误标志可手动清除 */

/**
  * @defgroup SPI_I2S_Get_Flag_Checks
  * @brief 状态标志合法性校验组
  * @details 验证状态标志查询请求的合法性
  */
#define IS_SPI_I2S_GET_FLAG(FLAG) (                                \
    ((FLAG) == SPI_I2S_FLAG_BSY) || ((FLAG) == SPI_I2S_FLAG_OVR) || \
    ((FLAG) == SPI_FLAG_MODF) || ((FLAG) == SPI_FLAG_CRCERR) ||    \
    ((FLAG) == I2S_FLAG_UDR) || ((FLAG) == I2S_FLAG_CHSIDE) ||     \
    ((FLAG) == SPI_I2S_FLAG_TXE) || ((FLAG) == SPI_I2S_FLAG_RXNE) || \
    ((FLAG) == SPI_I2S_FLAG_TIFRFE))                               /*!< 包含所有有效状态标志 */

/**
  * @}
  */
/**
  * @defgroup SPI_CRC_polynomial 
  * @brief CRC多项式合法性校验
  * @details 校验CRC多项式寄存器值的有效性，多项式值范围：0x0001~0xFFFF
  */
#define IS_SPI_CRC_POLYNOMIAL(POLYNOMIAL) ((POLYNOMIAL) >= 0x1) /*!< 多项式值必须≥1，实际使用时应为奇数 */

/**
  * @defgroup SPI_I2S_Legacy 
  * @brief 旧版SPI宏兼容定义组
  * @details 为保持历史代码兼容性，将I2S相关的新版宏重定义为传统SPI宏名称
  * @warning 新开发建议直接使用SPI_I2S_xxx系列宏
  */

/* DMA请求兼容定义 */
#define SPI_DMAReq_Tx                SPI_I2S_DMAReq_Tx  /*!< 兼容旧版发送DMA请求宏（已过时） */
#define SPI_DMAReq_Rx                SPI_I2S_DMAReq_Rx  /*!< 兼容旧版接收DMA请求宏 */

/* 中断类型兼容定义 */  
#define SPI_IT_TXE                   SPI_I2S_IT_TXE     /*!< 兼容旧版发送中断宏 */
#define SPI_IT_RXNE                  SPI_I2S_IT_RXNE    /*!< 兼容旧版接收中断宏 */
#define SPI_IT_ERR                   SPI_I2S_IT_ERR     /*!< 兼容旧版错误中断宏 */
#define SPI_IT_OVR                   SPI_I2S_IT_OVR     /*!< 兼容旧版溢出中断宏 */

/* 状态标志兼容定义 */
#define SPI_FLAG_RXNE                SPI_I2S_FLAG_RXNE  /*!< 兼容旧版接收标志宏 */
#define SPI_FLAG_TXE                 SPI_I2S_FLAG_TXE   /*!< 兼容旧版发送标志宏 */ 
#define SPI_FLAG_OVR                 SPI_I2S_FLAG_OVR  /*!< 兼容旧版溢出标志宏 */
#define SPI_FLAG_BSY                 SPI_I2S_FLAG_BSY   /*!< 兼容旧版忙标志宏 */

/* 函数接口兼容定义 */
#define SPI_DeInit                   SPI_I2S_DeInit            /*!< 兼容旧版初始化函数 */
#define SPI_ITConfig                 SPI_I2S_ITConfig          /*!< 兼容旧版中断配置函数 */
#define SPI_DMACmd                   SPI_I2S_DMACmd            /*!< 兼容旧版DMA控制函数 */
#define SPI_SendData                 SPI_I2S_SendData          /*!< 兼容旧版数据发送函数 */
#define SPI_ReceiveData              SPI_I2S_ReceiveData       /*!< 兼容旧版数据接收函数 */
#define SPI_GetFlagStatus            SPI_I2S_GetFlagStatus     /*!< 兼容旧版状态获取函数 */
#define SPI_ClearFlag                SPI_I2S_ClearFlag         /*!< 兼容旧版标志清除函数 */
#define SPI_GetITStatus              SPI_I2S_GetITStatus      /*!< 兼容旧版中断状态函数 */
#define SPI_ClearITPendingBit        SPI_I2S_ClearITPendingBit /*!< 兼容旧版中断清除函数 */

/**
  * @}
  */
  
/**
  * @}
  */
/* Exported functions --------------------------------------------------------*/ 

/**
  * @brief 复位SPI/I2S外设寄存器
  * @param SPIx 目标外设实例，范围：SPI1/SPI2/SPI3/SPI4/SPI5/SPI6
  * @note 影响CR1/CR2等所有配置寄存器，复位后外设处于禁用状态
  */
void SPI_I2S_DeInit(SPI_TypeDef* SPIx);

/**
  * @brief SPI外设初始化
  * @param SPIx 目标实例
  * @param SPI_InitStruct 配置结构体指针，含方向/模式/时钟等参数
  * @warning 调用前需确保外设时钟已使能
  */
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);

/**
  * @brief I2S外设初始化
  * @param SPIx 目标实例（仅支持具有I2S功能的SPI2/SPI3）
  * @param I2S_InitStruct 音频配置结构体指针
  * @note 需先调用RCC_APB1PeriphClockCmd使能时钟
  */
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct);

/**
  * @brief 填充SPI初始化结构体默认值
  * @param SPI_InitStruct 结构体指针
  * @details 默认配置：全双工/主机模式/8位数据/CPOL=0/CPHA=0
  */
void SPI_StructInit(SPI_InitTypeDef* SPI_InitStruct);

/**
  * @brief 填充I2S初始化结构体默认值
  * @param I2S_InitStruct 结构体指针
  * @details 默认配置：从发送模式/飞利浦标准/16位数据/禁用MCK
  */
void I2S_StructInit(I2S_InitTypeDef* I2S_InitStruct);

/**
  * @brief 使能/禁用SPI外设
  * @param SPIx 目标实例
  * @param NewState ENABLE(启动传输) 或 DISABLE(进入低功耗)
  * @note 禁用时会自动清空发送缓冲区
  */
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);

/**
  * @brief 使能/禁用I2S音频传输
  * @param SPIx 目标实例
  * @param NewState ENABLE(启动音频流) 或 DISABLE(停止传输)
  * @warning 禁用I2S时需确保无正在进行的数据传输
  */
void I2S_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);

/**
  * @brief 动态调整SPI数据位宽
  * @param SPIx 目标实例
  * @param SPI_DataSize 数据长度：SPI_DataSize_8b 或 SPI_DataSize_16b
  * @note 必须在SPI禁用状态下调用
  */
void SPI_DataSizeConfig(SPI_TypeDef* SPIx, uint16_t SPI_DataSize);

/**
  * @brief 配置SPI单向传输方向
  * @param SPIx 目标实例
  * @param SPI_Direction 传输方向：SPI_Direction_Tx 或 SPI_Direction_Rx
  * @warning 需与初始化时配置的SPI_Direction模式兼容
  */
void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, uint16_t SPI_Direction);

/**
  * @brief 配置软件NSS信号状态
  * @param SPIx 目标实例
  * @param SPI_NSSInternalSoft NSS状态：Set(拉低) 或 Reset(释放)
  * @note 仅当SPI_NSS_Soft模式启用时有效
  */
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, uint16_t SPI_NSSInternalSoft);

/**
  * @brief 控制从模式下的SS输出
  * @param SPIx 目标实例
  * @param NewState ENABLE(使能SS引脚电平控制) 或 DISABLE(高阻态)
  * @details 用于多从机级联场景，使从机能控制下级设备的SS
  */
void SPI_SSOutputCmd(SPI_TypeDef* SPIx, FunctionalState NewState);

/**
  * @brief 启用TI帧格式模式
  * @param SPIx 目标实例
  * @param NewState ENABLE(启用TI模式)/DISABLE(标准SPI模式)
  * @note TI模式下NSS信号由硬件自动管理
  */
void SPI_TIModeCmd(SPI_TypeDef* SPIx, FunctionalState NewState);

/**
  * @brief 配置I2S全双工扩展模式
  * @param I2Sxext 扩展实例（I2S2ext/I2S3ext）
  * @param I2S_InitStruct 音频配置结构体指针
  * @details 需与主I2S模块配合使用，共同组成全双工音频通道
  */
void I2S_FullDuplexConfig(SPI_TypeDef* I2Sxext, I2S_InitTypeDef* I2S_InitStruct);

/* Data transfers functions ***************************************************/ 

/**
  * @brief 发送单个数据
  * @param SPIx 目标实例
  * @param Data 待发送数据（8/16位根据SPI_DataSize配置）
  * @note 数据实际传输由硬件自动完成，函数立即返回
  * @warning 发送前需检测TXE标志，否则会覆盖未发送数据
  */
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);

/**
  * @brief 接收当前数据
  * @param SPIx 目标实例
  * @retval 接收到的数据（8/16位根据配置）
  * @note 读取后自动清除RXNE标志
  * @warning 需在RXNE标志置位时调用，否则返回无效数据
  */
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);

/* Hardware CRC Calculation functions *****************************************/

/**
  * @brief 启用/禁用硬件CRC计算
  * @param SPIx 目标实例
  * @param NewState ENABLE(自动附加CRC)/DISABLE(普通模式)
  * @note 需在SPI禁用状态下配置CRC多项式
  */
void SPI_CalculateCRC(SPI_TypeDef* SPIx, FunctionalState NewState);

/**
  * @brief 手动触发CRC传输
  * @param SPIx 目标实例
  * @details 在CRC模式下，将预计算的CRC值插入数据流
  * @warning 仅当SPI_DataSize_8b时有效
  */
void SPI_TransmitCRC(SPI_TypeDef* SPIx);

/**
  * @brief 获取CRC计算结果
  * @param SPIx 目标实例
  * @param SPI_CRC 选择CRC寄存器：SPI_CRC_Tx/Rx
  * @retval 计算得到的CRC值
  * @note 接收CRC需在传输完成后调用
  */
uint16_t SPI_GetCRC(SPI_TypeDef* SPIx, uint8_t SPI_CRC);

/**
  * @brief 获取当前CRC多项式
  * @param SPIx 目标实例
  * @retval 当前配置的CRC多项式值
  */
uint16_t SPI_GetCRCPolynomial(SPI_TypeDef* SPIx);

/* DMA transfers management functions *****************************************/

/**
  * @brief 控制DMA请求使能
  * @param SPIx 目标实例
  * @param SPI_I2S_DMAReq DMA请求类型：SPI_I2S_DMAReq_Tx/Rx 或组合
  * @param NewState ENABLE(触发DMA)/DISABLE(关闭DMA请求)
  * @note 需先配置DMA通道再启用
  */
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);

/* Interrupts and flags management functions **********************************/

/**
  * @brief 配置中断使能
  * @param SPIx 目标实例
  * @param SPI_I2S_IT 中断类型：TXE/RXNE/ERR等
  * @param NewState ENABLE/DISABLE
  * @warning 需同时配置NVIC中断控制器
  */
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);

/**
  * @brief 获取状态标志
  * @param SPIx 目标实例
  * @param SPI_I2S_FLAG 检查的标志：BSY/OVR/UDR等
  * @retval SET/RESET
  * @note 部分标志需手动清除
  */
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);

/**
  * @brief 清除指定标志位
  * @param SPIx 目标实例
  * @param SPI_I2S_FLAG 需清除的标志：仅支持OVR/UDR/CRCERR
  * @note 对BSY等只读标志无效
  */
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);

/**
  * @brief 获取中断状态（标志位+中断使能）
  * @param SPIx 目标实例
  * @param SPI_I2S_IT 检查的中断类型
  * @retval SET/RESET
  * @note 需同时满足标志置位和中断使能
  */
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);

/**
  * @brief 清除中断挂起位
  * @param SPIx 目标实例
  * @param SPI_I2S_IT 需清除的中断类型
  * @note 对于CRCERR等需手动清除的中断有效
  */
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_SPI_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

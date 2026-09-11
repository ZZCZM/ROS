/**
  ******************************************************************************
  * @file    stm32f4xx_sdio.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the SDIO firmware
  *          library.
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
#ifndef __STM32F4xx_SDIO_H
#define __STM32F4xx_SDIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup SDIO
  * @{
  */
/* Exported types ------------------------------------------------------------*/

/**
  * @brief SDIO初始化配置结构体
  * @details 包含SDIO控制器全局配置参数，用于设置时钟特性、总线宽度等基础属性
  */
typedef struct
{
  uint32_t SDIO_ClockEdge;            /*!< 时钟采样边沿选择：
                                           @arg SDIO_ClockEdge_Rising: 上升沿采样
                                           @arg SDIO_ClockEdge_Falling: 下降沿采样 */

  uint32_t SDIO_ClockBypass;          /*!< 时钟分频旁路模式：
                                           @arg SDIO_ClockBypass_Enable: 直接使用HCLK
                                           @arg SDIO_ClockBypass_Disable: 使用分频时钟 */

  uint32_t SDIO_ClockPowerSave;       /*!< 时钟节能模式：
                                           @arg SDIO_ClockPowerSave_Enable: 空闲时关闭时钟
                                           @arg SDIO_ClockPowerSave_Disable: 持续输出时钟 */

  uint32_t SDIO_BusWide;              /*!< 总线宽度配置：
                                           @arg SDIO_BusWide_1b: 1位模式（默认）
                                           @arg SDIO_BusWide_4b: 4位模式 */

  uint32_t SDIO_HardwareFlowControl;  /*!< 硬件流控制：
                                           @arg SDIO_HardwareFlowControl_Enable: 防止FIFO溢出
                                           @arg SDIO_HardwareFlowControl_Disable: 禁用流控 */

  uint8_t SDIO_ClockDiv;              /*!< 时钟分频系数(0-255)，实际时钟频率计算：
                                           SDIOCLK = HCLK / (2 * (ClockDiv + 1)) */                                           
} SDIO_InitTypeDef;

/**
  * @brief SDIO命令配置结构体
  * @details 用于发送SD/MMC标准命令时的参数封装，包含CMD线控制相关配置
  */
typedef struct
{
  uint32_t SDIO_Argument;  /*!< 32位命令参数，根据MMC/SD协议不同命令填写：
                              如CMD16(设置块长度)时该值为块大小 */

  uint32_t SDIO_CmdIndex;  /*!< 6位命令索引(0x00-0x3F)，对应SD卡规范命令编号：
                               如CMD0:0x00, CMD17:0x11 */

  uint32_t SDIO_Response;  /*!< 响应类型配置：
                                @arg SDIO_Response_No: 无响应
                                @arg SDIO_Response_Short: 48位短响应
                                @arg SDIO_Response_Long: 136位长响应 */

  uint32_t SDIO_Wait;      /*!< 等待中断配置：
                                @arg SDIO_Wait_Enable: 等待来自卡的中断请求
                                @arg SDIO_Wait_Disable: 不等待中断 */

  uint32_t SDIO_CPSM;      /*!< 命令路径状态机控制：
                                @arg SDIO_CPSM_Enable: 启动命令传输
                                @arg SDIO_CPSM_Disable: 保持空闲状态 */
} SDIO_CmdInitTypeDef;

/**
  * @brief SDIO数据传输配置结构体
  * @details 配置数据通道参数，支持单块/多块、读写操作及超时管理
  */
typedef struct
{
  uint32_t SDIO_DataTimeOut;    /*!< 数据超时时间（单位：卡总线时钟周期），
                                     建议值：0xFFFFFFFF表示最大超时 */

  uint32_t SDIO_DataLength;     /*!< 传输数据总字节数（单位：字节），
                                     需为块大小的整数倍 */

  uint32_t SDIO_DataBlockSize;  /*!< 数据块大小配置：
                                     @arg SDIO_DataBlockSize_1b: 1字节（特殊场景）
                                     @arg SDIO_DataBlockSize_512b: 512字节（标准块操作）*/

  uint32_t SDIO_TransferDir;    /*!< 数据传输方向：
                                     @arg SDIO_TransferDir_ToCard: 主机到卡（写操作）
                                     @arg SDIO_TransferDir_ToSDIO: 卡到主机（读操作） */
 
  uint32_t SDIO_TransferMode;   /*!< 传输模式选择：
                                     @arg SDIO_TransferMode_Block: 块传输（带起始/停止指令）
                                     @arg SDIO_TransferMode_Stream: 流传输（无起始/停止） */
 
  uint32_t SDIO_DPSM;           /*!< 数据路径状态机控制：
                                     @arg SDIO_DPSM_Enable: 启动数据传输
                                     @arg SDIO_DPSM_Disable: 保持空闲状态 */
} SDIO_DataInitTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup SDIO_Exported_Constants
  * @{
  */

/** @defgroup SDIO_Clock_Edge 
  * @brief 时钟采样边沿配置组
  * @details 定义数据在时钟信号边沿的捕获时机，需根据卡特性选择
  */
#define SDIO_ClockEdge_Rising               ((uint32_t)0x00000000)  /*!< 上升沿采样（适合高速模式） */
#define SDIO_ClockEdge_Falling              ((uint32_t)0x00002000)  /*!< 下降沿采样（特殊时序要求） */
#define IS_SDIO_CLOCK_EDGE(EDGE) (/*!< 边沿参数校验，防止通信时序错误 */ \
                                  ((EDGE) == SDIO_ClockEdge_Rising) || \
                                  ((EDGE) == SDIO_ClockEdge_Falling))

/** @defgroup SDIO_Clock_Bypass 
  * @brief 时钟分频旁路配置组
  * @details 控制时钟分频器的启用状态，直接影响SDIOCLK频率
  */
#define SDIO_ClockBypass_Disable             ((uint32_t)0x00000000)  /*!< 使用分频时钟：SDIOCLK=HCLK/(2*(DIV+1)) */
#define SDIO_ClockBypass_Enable              ((uint32_t)0x00000400)    /*!< 直接使用HCLK：SDIOCLK=HCLK（超频模式） */  
#define IS_SDIO_CLOCK_BYPASS(BYPASS) (/*!< 旁路模式参数校验 */ \
                                     ((BYPASS) == SDIO_ClockBypass_Disable) || \
                                     ((BYPASS) == SDIO_ClockBypass_Enable))

/** @defgroup SDIO_Clock_Power_Save 
  * @brief 时钟节能模式配置组
  * @details 控制总线空闲时的时钟输出状态，影响功耗管理
  */
#define SDIO_ClockPowerSave_Disable         ((uint32_t)0x00000000)  /*!< 持续输出时钟（快速响应但功耗高） */
#define SDIO_ClockPowerSave_Enable          ((uint32_t)0x00000200)  /*!< 空闲时停止时钟（节电模式） */
#define IS_SDIO_CLOCK_POWER_SAVE(SAVE) (/*!< 节能模式参数校验 */ \
                                        ((SAVE) == SDIO_ClockPowerSave_Disable) || \
                                        ((SAVE) == SDIO_ClockPowerSave_Enable))

/** @defgroup SDIO_Bus_Wide 
  * @brief 数据总线宽度配置组
  * @details 设置SDIO数据线数量，直接影响传输带宽
  */
#define SDIO_BusWide_1b                     ((uint32_t)0x00000000)  /*!< 1位模式（兼容性模式，低速率） */
#define SDIO_BusWide_4b                     ((uint32_t)0x00000800)  /*!< 4位模式（标准SD卡操作） */
#define SDIO_BusWide_8b                     ((uint32_t)0x00001000)  /*!< 8位模式（eMMC器件专用） */
#define IS_SDIO_BUS_WIDE(WIDE) (/*!< 总线宽度参数校验，禁用非法组合 */ \
                                ((WIDE) == SDIO_BusWide_1b) || \
                                ((WIDE) == SDIO_BusWide_4b) || \
                                ((WIDE) == SDIO_BusWide_8b))

/** @defgroup SDIO_Hardware_Flow_Control 
  * @brief 硬件流控制配置组
  * @details 防止FIFO溢出/欠载，确保可靠数据传输
  */
#define SDIO_HardwareFlowControl_Disable    ((uint32_t)0x00000000)  /*!< 禁用流控（需软件管理数据流） */
#define SDIO_HardwareFlowControl_Enable     ((uint32_t)0x00004000)  /*!< 启用流控（自动暂停传输） */
#define IS_SDIO_HARDWARE_FLOW_CONTROL(CONTROL) (/*!< 流控参数校验，防止配置冲突 */ \
                                                ((CONTROL) == SDIO_HardwareFlowControl_Disable) || \
                                                ((CONTROL) == SDIO_HardwareFlowControl_Enable))

/** @defgroup SDIO_Power_State 
  * @brief 控制器电源状态配置组
  * @details 控制SDIO控制器的供电状态，影响功耗和唤醒时间
  */
#define SDIO_PowerState_OFF                 ((uint32_t)0x00000000)  /*!< 关闭电源（深度睡眠模式） */
#define SDIO_PowerState_ON                  ((uint32_t)0x00000003)  /*!< 开启电源（正常操作状态） */
#define IS_SDIO_POWER_STATE(STATE) (/*!< 电源状态参数校验 */ \
                                   ((STATE) == SDIO_PowerState_OFF) || \
                                   ((STATE) == SDIO_PowerState_ON))

/**
  * @}
  */ 

/**
  * @defgroup SDIO_Interrupt_sources
  * @brief SDIO中断源定义组
  * @details 包含所有可配置的SDIO中断类型，涵盖命令/数据传输状态及错误检测
  */

/* 命令相关中断 */
#define SDIO_IT_CCRCFAIL    ((uint32_t)0x00000001) /*!< 命令响应CRC校验失败 */
#define SDIO_IT_DCRCFAIL    ((uint32_t)0x00000002) /*!< 数据块CRC校验失败 */
#define SDIO_IT_CTIMEOUT    ((uint32_t)0x00000004) /*!< 命令响应超时（NCR时限） */
#define SDIO_IT_CMDREND     ((uint32_t)0x00000040) /*!< 收到有效命令响应（无CRC错误）*/
#define SDIO_IT_CMDSENT     ((uint32_t)0x00000080) /*!< 命令传输完成（无响应命令）*/

/* 数据传输中断 */
#define SDIO_IT_DTIMEOUT    ((uint32_t)0x00000008) /*!< 数据超时（传输未在DDTIMEOUT内完成）*/
#define SDIO_IT_TXUNDERR    ((uint32_t)0x00000010) /*!< 发送FIFO下溢（SDIO过早请求数据）*/
#define SDIO_IT_RXOVERR     ((uint32_t)0x00000020) /*!< 接收FIFO溢出（数据未及时读取）*/
#define SDIO_IT_DATAEND     ((uint32_t)0x00000100) /*!< 数据传送完成（读/写操作结束）*/
#define SDIO_IT_DBCKEND     ((uint32_t)0x00000400) /*!< 数据块传输完成（多块传输中单个块结束）*/

/* 总线状态中断 */
#define SDIO_IT_STBITERR    ((uint32_t)0x00000200) /*!< 起始位检测错误（数据包起始位为0）*/
#define SDIO_IT_CMDACT      ((uint32_t)0x00000800) /*!< 命令传输进行中（CPSM激活）*/
#define SDIO_IT_TXACT       ((uint32_t)0x00001000) /*!< 数据发送进行中（DPSM激活）*/
#define SDIO_IT_RXACT       ((uint32_t)0x00002000) /*!< 数据接收进行中（DPSM激活）*/

/* FIFO状态中断 */
#define SDIO_IT_TXFIFOHE    ((uint32_t)0x00004000) /*!< 发送FIFO半空（可填充数据）*/
#define SDIO_IT_RXFIFOHF    ((uint32_t)0x00008000) /*!< 接收FIFO半满（需读取数据）*/
#define SDIO_IT_TXFIFOF     ((uint32_t)0x00010000) /*!< 发送FIFO满（停止写入数据）*/
#define SDIO_IT_RXFIFOF     ((uint32_t)0x00020000) /*!< 接收FIFO满（必须立即读取）*/
#define SDIO_IT_TXFIFOE     ((uint32_t)0x00040000) /*!< 发送FIFO空（无可用数据）*/
#define SDIO_IT_RXFIFOE     ((uint32_t)0x00080000) /*!< 接收FIFO空（无数据可读）*/

/* 数据可用中断 */
#define SDIO_IT_TXDAVL      ((uint32_t)0x00100000) /*!< 发送FIFO存在可用空间（可继续写入）*/
#define SDIO_IT_RXDAVL      ((uint32_t)0x00200000) /*!< 接收FIFO有数据可读 */

/* 特殊功能中断 */
#define SDIO_IT_SDIOIT      ((uint32_t)0x00400000) /*!< 收到SDIO卡的特定中断（IO设备专用）*/
#define SDIO_IT_CEATAEND    ((uint32_t)0x00800000) /*!< CE-ATA命令完成（需手动清除）*/

/**
  * @brief 中断源合法性校验
  * @param IT 待校验的中断标志组合
  * @retval 1-有效 / 0-非法
  * @note 校验规则：
  *        - 高8位必须为0（保留位）
  *        - 不能为全0值
  */
#define IS_SDIO_IT(IT) ( (((IT) & 0xFF000000) == 0x00) && ((IT) != 0x00) )

/**
  * @}
  */ 
/** @defgroup SDIO_Command_Index
  * @brief 命令索引校验宏
  * @details 确保命令索引符合SDIO规范（0x00-0x3F）
  */
#define IS_SDIO_CMD_INDEX(INDEX) ((INDEX) < 0x40) /*!< 有效范围0x00(CMD0)到0x3F(CMD63) */

/** @defgroup SDIO_Response_Type
  * @brief 命令响应类型配置组
  * @details 定义主机如何解析来自卡的响应数据格式
  */
#define SDIO_Response_No    ((uint32_t)0x00000000) /*!< 无响应：适用于CMD0等无返回命令 */
#define SDIO_Response_Short ((uint32_t)0x00000040) /*!< 48位短响应：CMD8/CMD58等标准响应 */
#define SDIO_Response_Long  ((uint32_t)0x000000C0) /*!< 136位长响应：CID/CSD寄存器读取 */
#define IS_SDIO_RESPONSE(RESPONSE) (/*!< 响应类型合法性检查 */ \
    ((RESPONSE) == SDIO_Response_No) || \
    ((RESPONSE) == SDIO_Response_Short) || \
    ((RESPONSE) == SDIO_Response_Long))

/** @defgroup SDIO_Wait_Interrupt_State
  * @brief 命令等待模式配置组
  * @details 控制命令执行期间的总线等待行为
  */
#define SDIO_Wait_No    ((uint32_t)0x00000000) /*!< 不等待：立即执行命令（默认） */
#define SDIO_Wait_IT    ((uint32_t)0x00000100) /*!< 等待中断：用于异步事件处理 */
#define SDIO_Wait_Pend  ((uint32_t)0x00000200) /*!< 等待传输结束：阻塞直到操作完成 */
#define IS_SDIO_WAIT(WAIT) (/*!< 等待模式参数校验 */ \
    ((WAIT) == SDIO_Wait_No) || \
    ((WAIT) == SDIO_Wait_IT) || \
    ((WAIT) == SDIO_Wait_Pend))

/** @defgroup SDIO_CPSM_State
  * @brief 命令路径状态机控制组
  * @details 控制命令发送流程的启动与停止
  */
#define SDIO_CPSM_Disable ((uint32_t)0x00000000) /*!< 禁用CPSM：保持空闲状态 */
#define SDIO_CPSM_Enable  ((uint32_t)0x00000400) /*!< 启用CPSM：启动命令传输序列 */
#define IS_SDIO_CPSM(CPSM) (/*!< 状态机控制参数检查 */ \
    ((CPSM) == SDIO_CPSM_Enable) || \
    ((CPSM) == SDIO_CPSM_Disable))

/** @defgroup SDIO_Response_Registers
  * @brief 响应数据寄存器映射组
  * @details 存储从卡返回的不同响应数据段
  */
#define SDIO_RESP1 ((uint32_t)0x00000000) /*!< RESP1：存储短响应的状态位[31:0] */
#define SDIO_RESP2 ((uint32_t)0x00000004) /*!< RESP2：长响应第二部分（CSD/CID[127:96]） */
#define SDIO_RESP3 ((uint32_t)0x00000008) /*!< RESP3：长响应第三部分（CSD/CID[95:64]） */
#define SDIO_RESP4 ((uint32_t)0x0000000C) /*!< RESP4：长响应第四部分（CSD/CID[63:32]） */
#define IS_SDIO_RESP(RESP) (/*!< 响应寄存器地址合法性验证 */ \
    ((RESP) == SDIO_RESP1) || \
    ((RESP) == SDIO_RESP2) || \
    ((RESP) == SDIO_RESP3) || \
    ((RESP) == SDIO_RESP4))

/**
  * @}
  */

/** @defgroup SDIO_Data_Length 
  * @{
  */

#define IS_SDIO_DATA_LENGTH(LENGTH) ((LENGTH) <= 0x01FFFFFF)
/**
  * @}
  */

/** @defgroup SDIO_Data_Block_Size 
  * @{
  */
/**
  * @defgroup SDIO_Data_Block_Size 
  * @brief SDIO数据块大小配置组
  * @details 定义单次数据传输的块大小，影响FIFO使用效率及传输稳定性
  *          需根据存储设备特性选择，标准SD卡建议使用512字节配置
  */
#define SDIO_DataBlockSize_1b               ((uint32_t)0x00000000)  /*!< 1字节块（特殊调试用途，非常规模式） */
#define SDIO_DataBlockSize_2b               ((uint32_t)0x00000010)  /*!< 2字节块（低速设备测试） */
#define SDIO_DataBlockSize_4b               ((uint32_t)0x00000020)  /*!< 4字节块（自定义协议设备） */
#define SDIO_DataBlockSize_8b               ((uint32_t)0x00000030)  /*!< 8字节块（非标准存储介质） */
#define SDIO_DataBlockSize_16b              ((uint32_t)0x00000040)  /*!< 16字节块（低分辨率音频流） */
#define SDIO_DataBlockSize_32b              ((uint32_t)0x00000050)  /*!< 32字节块（传感器批量数据传输） */
#define SDIO_DataBlockSize_64b              ((uint32_t)0x00000060)  /*!< 64字节块（中等速率IO设备） */
#define SDIO_DataBlockSize_128b             ((uint32_t)0x00000070)  /*!< 128字节块（嵌入式文件系统优化） */
#define SDIO_DataBlockSize_256b             ((uint32_t)0x00000080)  /*!< 256字节块（高带宽视频流片段） */
#define SDIO_DataBlockSize_512b             ((uint32_t)0x00000090)  /*!< 512字节块（标准SD卡操作，FAT32兼容） */
#define SDIO_DataBlockSize_1024b            ((uint32_t)0x000000A0)  /*!< 1024字节块（eMMC高速模式） */
#define SDIO_DataBlockSize_2048b            ((uint32_t)0x000000B0)  /*!< 2048字节块（DVD扇区仿真模式） */
#define SDIO_DataBlockSize_4096b            ((uint32_t)0x000000C0)  /*!< 4096字节块（固态存储擦除单元） */
#define SDIO_DataBlockSize_8192b            ((uint32_t)0x000000D0)  /*!< 8192字节块（数据库事务处理块） */
#define SDIO_DataBlockSize_16384b           ((uint32_t)0x000000E0)  /*!< 16384字节块（内存虚拟磁盘操作） */

/**
  * @brief 数据块大小参数校验
  * @param SIZE 待验证的块大小配置值
  * @retval 1=有效 0=非法
  * @note 检查参数是否为预定义的16种块大小配置之一
  *       防止设置非标数据块导致DMA传输异常
  */
#define IS_SDIO_BLOCK_SIZE(SIZE) ( \
    ((SIZE) == SDIO_DataBlockSize_1b) ||     /* 校验1字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_2b) ||    /* 校验2字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_4b) ||     /* 校验4字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_8b) ||     /* 校验8字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_16b) ||   /* 校验16字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_32b) ||    /* 校验32字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_64b) ||    /* 校验64字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_128b) ||   /* 校验128字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_256b) ||   /* 校验256字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_512b) ||   /* 校验512字节标准块 */ \
    ((SIZE) == SDIO_DataBlockSize_1024b) ||  /* 校验1024字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_2048b) ||  /* 校验2048字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_4096b) ||  /* 校验4096字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_8192b) ||  /* 校验8192字节模式 */ \
    ((SIZE) == SDIO_DataBlockSize_16384b))   /* 校验16384字节大块 */

/**
  * @}
  */
/** @defgroup SDIO_Transfer_Direction 
  * @brief 数据传输方向配置组
  * @details 定义数据在主机与存储卡之间的传输方向
  */
#define SDIO_TransferDir_ToCard             ((uint32_t)0x00000000) /*!< 主机到存储卡（写操作），用于文件存储场景 */
#define SDIO_TransferDir_ToSDIO             ((uint32_t)0x00000002) /*!< 存储卡到主机（读操作），用于数据读取场景 */
#define IS_SDIO_TRANSFER_DIR(DIR) (/*!< 方向参数合法性检查 */ \
    ((DIR) == SDIO_TransferDir_ToCard) || \
    ((DIR) == SDIO_TransferDir_ToSDIO))

/** @defgroup SDIO_Transfer_Type 
  * @brief 数据传输模式配置组
  * @details 选择块传输或流传输模式，影响数据包的组织方式
  */
#define SDIO_TransferMode_Block             ((uint32_t)0x00000000) /*!< 块传输模式：带起始/结束标志，用于标准存储操作 */
#define SDIO_TransferMode_Stream            ((uint32_t)0x00000004) /*!< 流传输模式：连续数据流，适合音频采集等实时场景 */
#define IS_SDIO_TRANSFER_MODE(MODE) (/*!< 传输模式参数校验 */ \
    ((MODE) == SDIO_TransferMode_Stream) || \
    ((MODE) == SDIO_TransferMode_Block))

/** @defgroup SDIO_DPSM_State 
  * @brief 数据路径状态机控制组
  * @details 控制数据传输流程的启动与停止
  */
#define SDIO_DPSM_Disable                    ((uint32_t)0x00000000) /*!< 禁用DPSM：暂停数据传输流程 */
#define SDIO_DPSM_Enable                     ((uint32_t)0x00000001) /*!< 启用DPSM：自动处理数据传输序列 */
#define IS_SDIO_DPSM(DPSM) (/*!< 状态机控制参数检查 */ \
    ((DPSM) == SDIO_DPSM_Enable) || \
    ((DPSM) == SDIO_DPSM_Disable))

/**
  * @}
  */
/**
  * @defgroup SDIO_Flags SDIO状态标志位定义及验证
  * @{
  * @brief 定义SDIO模块所有可能的状态标志位（32位掩码），并提供标志位有效性检查宏
  * @details 包含命令响应、数据传输、FIFO状态、总线状态等各个维度的硬件状态标志，
  *          每个标志对应SDIO状态寄存器（STA）的特定bit位，通过读取寄存器值并按位与操作可检测状态。
  *          最后提供IS_SDIO_FLAG宏用于验证输入标志位的合法性
  */

/* 命令相关错误标志 -----------------------------------------------------------*/
#define SDIO_FLAG_CCRCFAIL                  ((uint32_t)0x00000001) /*!< 命令响应CRC校验失败 
                                                                       位位置：0 
                                                                       触发条件：接收到CRC校验错误的命令响应 */

#define SDIO_FLAG_DCRCFAIL                  ((uint32_t)0x00000002) /*!< 数据块CRC校验失败 
                                                                       位位置：1 
                                                                       触发条件：数据传输时检测到CRC错误 */

/* 超时错误标志 --------------------------------------------------------------*/
#define SDIO_FLAG_CTIMEOUT                  ((uint32_t)0x00000004) /*!< 命令响应超时 
                                                                       位位置：2 
                                                                       触发条件：未在预定时间内收到CMD响应 */

#define SDIO_FLAG_DTIMEOUT                  ((uint32_t)0x00000008) /*!< 数据传输超时 
                                                                       位位置：3 
                                                                       触发条件：数据端口在预定时间内未完成传输 */

/* 数据传输错误标志 -----------------------------------------------------------*/
#define SDIO_FLAG_TXUNDERR                  ((uint32_t)0x00000010) /*!< 发送器下溢错误 
                                                                       位位置：4 
                                                                       触发条件：FIFO为空时尝试发送数据 */

#define SDIO_FLAG_RXOVERR                   ((uint32_t)0x00000020) /*!< 接收器上溢错误 
                                                                       位位置：5 
                                                                       触发条件：FIFO已满时继续接收数据 */

/* 命令执行状态标志 -----------------------------------------------------------*/
#define SDIO_FLAG_CMDREND                   ((uint32_t)0x00000040) /*!< 命令响应接收完成 
                                                                       位位置：6 
                                                                       触发条件：成功接收到CMD响应（包括CRC校验通过） */

#define SDIO_FLAG_CMDSENT                   ((uint32_t)0x00000080) /*!< 命令发送完成 
                                                                       位位置：7 
                                                                       触发条件：CMD命令已通过CPSM传输到卡 */

/* 数据块传输状态 ------------------------------------------------------------*/
#define SDIO_FLAG_DATAEND                   ((uint32_t)0x00000100) /*!< 数据传输结束标志 
                                                                       位位置：8 
                                                                       触发条件：完成预定数据块数量的传输 */

#define SDIO_FLAG_STBITERR                  ((uint32_t)0x00000200) /*!< 起始位检测错误 
                                                                       位位置：9 
                                                                       触发条件：在数据包起始位置未检测到低电平 */

#define SDIO_FLAG_DBCKEND                   ((uint32_t)0x00000400) /*!< 数据块传输结束 
                                                                       位位置：10 
                                                                       触发条件：完成单个数据块的传输 */

/* 总线状态标志 --------------------------------------------------------------*/
#define SDIO_FLAG_CMDACT                    ((uint32_t)0x00000800) /*!< 命令传输进行中 
                                                                       位位置：11 
                                                                       触发条件：CPSM处于非空闲状态 */

#define SDIO_FLAG_TXACT                     ((uint32_t)0x00001000) /*!< 数据发送进行中 
                                                                       位位置：12 
                                                                       触发条件：DPSM处于发送状态 */

#define SDIO_FLAG_RXACT                     ((uint32_t)0x00002000) /*!< 数据接收进行中 
                                                                       位位置：13 
                                                                       触发条件：DPSM处于接收状态 */

/* FIFO状态标志 --------------------------------------------------------------*/
#define SDIO_FLAG_TXFIFOHE                  ((uint32_t)0x00004000) /*!< 发送FIFO半空 
                                                                       位位置：14 
                                                                       触发条件：发送FIFO数据量 ≤ 半满阈值 */

#define SDIO_FLAG_RXFIFOHF                  ((uint32_t)0x00008000) /*!< 接收FIFO半满 
                                                                       位位置：15 
                                                                       触发条件：接收FIFO数据量 ≥ 半满阈值 */

#define SDIO_FLAG_TXFIFOF                   ((uint32_t)0x00010000) /*!< 发送FIFO满 
                                                                       位位置：16 
                                                                       触发条件：发送FIFO剩余空间=0 */

#define SDIO_FLAG_RXFIFOF                   ((uint32_t)0x00020000) /*!< 接收FIFO满 
                                                                       位位置：17 
                                                                       触发条件：接收FIFO剩余空间=0 */

#define SDIO_FLAG_TXFIFOE                   ((uint32_t)0x00040000) /*!< 发送FIFO空 
                                                                       位位置：18 
                                                                       触发条件：发送FIFO数据量=0 */

#define SDIO_FLAG_RXFIFOE                   ((uint32_t)0x00080000) /*!< 接收FIFO空 
                                                                       位位置：19 
                                                                       触发条件：接收FIFO数据量=0 */

/* 数据可用性标志 ------------------------------------------------------------*/
#define SDIO_FLAG_TXDAVL                    ((uint32_t)0x00100000) /*!< 发送数据可用 
                                                                       位位置：20 
                                                                       触发条件：发送FIFO中有可用空间 */

#define SDIO_FLAG_RXDAVL                    ((uint32_t)0x00200000) /*!< 接收数据可用 
                                                                       位位置：21 
                                                                       触发条件：接收FIFO中有待读取数据 */

/* 中断标志 ------------------------------------------------------------------*/
#define SDIO_FLAG_SDIOIT                    ((uint32_t)0x00400000) /*!< SDIO中断 
                                                                       位位置：22 
                                                                       触发条件：收到SDIO卡特定中断信号 */

#define SDIO_FLAG_CEATAEND                  ((uint32_t)0x00800000) /*!< CE-ATA命令完成 
                                                                       位位置：23 
                                                                       触发条件：CE-ATA设备完成命令执行 */

/**
  * @brief 验证SDIO标志位合法性
  * @param FLAG 要检查的标志位数值
  * @retval 布尔值，true表示是有效标志位，false表示无效
  * @details 通过穷举所有预定义的标志位进行匹配检查，确保输入值符合SDIO规范。
  *          注意：多个标志位不能通过位或(|)组合使用，必须单独检查每个标志位
  */
// 检查CRC失败类标志
// 检查数据CRC错误
// 检查命令超时
// 检查数据超时
// 检查发送下溢
// 检查接收上溢
// 检查命令响应完成
// 检查命令发送完成
// 检查数据结束
// 检查起始位错误
// 检查数据块结束
// 检查命令活动状态
// 检查发送活动状态
// 检查接收活动状态
// 检查发送FIFO半空
// 检查接收FIFO半满
// 检查发送FIFO满
// 检查接收FIFO满
// 检查发送FIFO空
// 检查接收FIFO空
// 检查发送数据可用
// 检查接收数据可用
// 检查SDIO中断
// 检查CE-ATA结束
#define IS_SDIO_FLAG(FLAG) (((FLAG)  == SDIO_FLAG_CCRCFAIL) || \
                            ((FLAG)  == SDIO_FLAG_DCRCFAIL) || \
                            ((FLAG)  == SDIO_FLAG_CTIMEOUT) || \
                            ((FLAG)  == SDIO_FLAG_DTIMEOUT) || \
                            ((FLAG)  == SDIO_FLAG_TXUNDERR) || \
                            ((FLAG)  == SDIO_FLAG_RXOVERR) || \
                            ((FLAG)  == SDIO_FLAG_CMDREND) || \
                            ((FLAG)  == SDIO_FLAG_CMDSENT) || \
                            ((FLAG)  == SDIO_FLAG_DATAEND) || \
                            ((FLAG)  == SDIO_FLAG_STBITERR) || \
                            ((FLAG)  == SDIO_FLAG_DBCKEND) || \
                            ((FLAG)  == SDIO_FLAG_CMDACT) || \
                            ((FLAG)  == SDIO_FLAG_TXACT) || \
                            ((FLAG)  == SDIO_FLAG_RXACT) || \
                            ((FLAG)  == SDIO_FLAG_TXFIFOHE) || \
                            ((FLAG)  == SDIO_FLAG_RXFIFOHF) || \
                            ((FLAG)  == SDIO_FLAG_TXFIFOF) || \
                            ((FLAG)  == SDIO_FLAG_RXFIFOF) || \
                            ((FLAG)  == SDIO_FLAG_TXFIFOE) || \
                            ((FLAG)  == SDIO_FLAG_RXFIFOE) || \
                            ((FLAG)  == SDIO_FLAG_TXDAVL) || \
                            ((FLAG)  == SDIO_FLAG_RXDAVL) || \
                            ((FLAG)  == SDIO_FLAG_SDIOIT) || \
                            ((FLAG)  == SDIO_FLAG_CEATAEND))    

/**
  * @brief  SDIO清除标志验证宏
  * @param  FLAG: 需要清除的SDIO标志位
  * @retval 布尔值，表示标志位是否合法可清除
  * @note   该宏通过位掩码0xFF3FF800验证保留位，确保：
  *         - 不修改保留位（保留位必须保持0）
  *         - 至少有一个有效标志位被设置（FLAG != 0）
  */
#define IS_SDIO_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFF3FF800) == 0x00) && ((FLAG) != (uint32_t)0x00))

/**
  * @brief  SDIO中断源验证宏
  * @param  IT: 需要检查的SDIO中断类型
  * @retval 布尔值，表示中断类型是否有效
  * @note   该宏验证23种SDIO规范定义的中断类型，包含：
  */
#define IS_SDIO_GET_IT(IT) (((IT)  == SDIO_IT_CCRCFAIL) ||   /* 命令响应CRC失败中断 */ \
                            ((IT)  == SDIO_IT_DCRCFAIL) ||   /* 数据块CRC失败中断 */\
                            ((IT)  == SDIO_IT_CTIMEOUT) ||   /* 命令响应超时中断 */\
                            ((IT)  == SDIO_IT_DTIMEOUT) ||   /* 数据超时中断 */\
                            ((IT)  == SDIO_IT_TXUNDERR) ||   /* 发送FIFO下溢中断 */\
                            ((IT)  == SDIO_IT_RXOVERR) ||    /* 接收FIFO上溢中断 */\
                            ((IT)  == SDIO_IT_CMDREND) ||    /* 命令响应接收完成中断 */\
                            ((IT)  == SDIO_IT_CMDSENT) ||    /* 命令已发送中断 */\
                            ((IT)  == SDIO_IT_DATAEND) ||    /* 数据块传输结束中断 */\
                            ((IT)  == SDIO_IT_STBITERR) ||   /* 数据传输开始位错误中断 */\
                            ((IT)  == SDIO_IT_DBCKEND) ||    /* 数据块结束标志中断 */\
                            ((IT)  == SDIO_IT_CMDACT) ||     /* 命令传输进行中状态 */\
                            ((IT)  == SDIO_IT_TXACT) ||      /* 数据发送进行中状态 */\
                            ((IT)  == SDIO_IT_RXACT) ||      /* 数据接收进行中状态 */\
                            ((IT)  == SDIO_IT_TXFIFOHE) ||   /* 发送FIFO半空状态 */\
                            ((IT)  == SDIO_IT_RXFIFOHF) ||   /* 接收FIFO半满状态 */\
                            ((IT)  == SDIO_IT_TXFIFOF) ||    /* 发送FIFO满状态 */\
                            ((IT)  == SDIO_IT_RXFIFOF) ||    /* 接收FIFO满状态 */\
                            ((IT)  == SDIO_IT_TXFIFOE) ||    /* 发送FIFO空状态 */\
                            ((IT)  == SDIO_IT_RXFIFOE) ||    /* 接收FIFO空状态 */\
                            ((IT)  == SDIO_IT_TXDAVL) ||     /* 发送FIFO数据可用状态 */\
                            ((IT)  == SDIO_IT_RXDAVL) ||     /* 接收FIFO数据可用状态 */\
                            ((IT)  == SDIO_IT_SDIOIT) ||     /* SDIO专用中断 */\
                            ((IT)  == SDIO_IT_CEATAEND))      /* CE-ATA命令完成中断 */

/**
  * @brief  SDIO中断清除验证宏
  * @param  IT: 需要清除的中断标志
  * @retval 布尔值，表示中断标志是否合法可清除
  * @note   该宏通过位掩码0xFF3FF800验证：
  *         - 不修改保留位（保留位必须保持0）
  *         - 至少有一个有效中断标志被设置（IT != 0）
  */
#define IS_SDIO_CLEAR_IT(IT) ((((IT) & (uint32_t)0xFF3FF800) == 0x00) && ((IT) != (uint32_t)0x00))


/**
  * @}
  */
/** @defgroup SDIO_Read_Wait_Mode SDIO读等待模式
  * @{
  * @brief SDIO在读取挂起期间的等待模式配置选项
  */

/**
  * @brief 使用DATA2[位]检测读等待模式
  * @note 当选择此模式时：
  *        - 在数据传输期间持续监测DATA2线电平
  *        - 当DATA2线变低时暂停数据传输
  */
#define SDIO_ReadWaitMode_DATA2             ((uint32_t)0x00000000)

/**
  * @brief 使用时钟停止方式实现读等待模式
  * @note 当选择此模式时：
  *        - 通过停止SDIO_CK时钟实现等待
  *        - 需要主机控制器支持时钟暂停功能
  */
#define SDIO_ReadWaitMode_CLK               ((uint32_t)0x00000001)

/**
  * @brief 读等待模式验证宏
  * @param MODE 需要验证的读等待模式
  * @retval 布尔值，表示是否为有效模式
  * @note 有效模式包括：
  *        - SDIO_ReadWaitMode_DATA2 (0x00000000)
  *        - SDIO_ReadWaitMode_CLK   (0x00000001)
  */
#define IS_SDIO_READWAIT_MODE(MODE) (((MODE) == SDIO_ReadWaitMode_CLK) || \
                                     ((MODE) == SDIO_ReadWaitMode_DATA2))
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief 复位SDIO外设配置到默认状态
  * @note 该函数将：
  *        - 复位所有控制寄存器
  *        - 关闭SDIO时钟
  *        - 清除所有挂起标志
  */
void SDIO_DeInit(void);

/* 初始化与配置函数组 *********************************************************/
/**
  * @brief 初始化SDIO外设
  * @param SDIO_InitStruct 指向SDIO_InitTypeDef结构的指针，包含配置参数
  * @note 可配置参数包括：
  *        - 时钟分频系数
  *        - 时钟边沿
  *        - 硬件流控制
  *        - 总线宽度等
  */
void SDIO_Init(SDIO_InitTypeDef* SDIO_InitStruct);

/**
  * @brief 初始化SDIO配置结构体为默认值
  * @param SDIO_InitStruct 需要初始化的结构体指针
  * @note 默认参数：
  *        - 时钟分频器: 0x00
  *        - 时钟边沿: 上升沿
  *        - 硬件流控制: 禁用
  *        - 总线宽度: 1位模式
  */
void SDIO_StructInit(SDIO_InitTypeDef* SDIO_InitStruct);

/**
  * @brief SDIO时钟使能控制
  * @param NewState 时钟状态 (ENABLE 或 DISABLE)
  * @note 在数据传输前必须使能时钟
  */
void SDIO_ClockCmd(FunctionalState NewState);

/**
  * @brief 设置SDIO电源状态
  * @param SDIO_PowerState 电源状态：
  *        - SDIO_PowerState_OFF (0x00000000)
  *        - SDIO_PowerState_ON  (0x00000003)
  */
void SDIO_SetPowerState(uint32_t SDIO_PowerState);

/**
  * @brief 获取当前电源状态
  * @retval 当前电源状态寄存器值
  */
uint32_t SDIO_GetPowerState(void);

/* 命令路径状态机(CPSM)管理函数 ***********************************************/
/**
  * @brief 发送SDIO命令
  * @param SDIO_CmdInitStruct 命令配置结构体指针
  * @note 结构体包含：
  *        - 命令索引
  *        - 响应类型
  *        - 等待控制
  *        - CPSM状态机控制位
  */
void SDIO_SendCommand(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);

/**
  * @brief 初始化命令结构体为默认值
  * @param SDIO_CmdInitStruct 需要初始化的命令结构体指针
  */
void SDIO_CmdStructInit(SDIO_CmdInitTypeDef* SDIO_CmdInitStruct);

/**
  * @brief 获取最后接收到的命令响应索引
  * @retval 最近接收到的命令响应索引(0x00-0x3F)
  */
uint8_t SDIO_GetCommandResponse(void);

/**
  * @brief 获取完整的命令响应数据
  * @param SDIO_RESP 响应寄存器选择：
  *        - SDIO_RESP1 (0x00000000)
  *        - SDIO_RESP2 (0x00000004)
  *        - SDIO_RESP3 (0x00000008)
  *        - SDIO_RESP4 (0x0000000C)
  * @retval 对应响应寄存器的32位数据
  */
uint32_t SDIO_GetResponse(uint32_t SDIO_RESP);

/* 数据路径状态机(DPSM)管理函数 ***********************************************/
/**
  * @brief 配置数据通信参数
  * @param SDIO_DataInitStruct 数据配置结构体指针
  * @note 可配置参数包括：
  *        - 数据长度
  *        - 数据传输方向
  *        - 数据块大小
  *        - DMA使能状态
  *        - 数据传输超时
  */
void SDIO_DataConfig(SDIO_DataInitTypeDef* SDIO_DataInitStruct);

/**
  * @brief 初始化数据配置结构体为默认值
  * @param SDIO_DataInitStruct 数据配置结构体指针
  * @note 默认参数：
  *        - 数据长度: 0x00
  *        - 传输方向: 控制器到卡
  *        - 块大小: 1字节
  */
void SDIO_DataStructInit(SDIO_DataInitTypeDef* SDIO_DataInitStruct);

/**
  * @brief 获取剩余数据传输量
  * @retval 剩余待传输数据字节数
  */
uint32_t SDIO_GetDataCounter(void);

/**
  * @brief 从FIFO读取32位数据
  * @retval 32位数据内容
  * @note 每次读取自动递减数据计数器
  */
uint32_t SDIO_ReadData(void);

/**
  * @brief 向FIFO写入32位数据
  * @param Data 待写入的32位数据
  */
void SDIO_WriteData(uint32_t Data);

/**
  * @brief 获取FIFO当前数据量
  * @retval FIFO中剩余32位字的数量
  */
uint32_t SDIO_GetFIFOCount(void);

/* SDIO IO卡模式管理函数 ******************************************************/
/**
  * @brief 启动SDIO读等待操作
  * @param NewState 新状态 (ENABLE/DISABLE)
  * @note 用于在挂起期间保持数据传输连续性
  */
void SDIO_StartSDIOReadWait(FunctionalState NewState);

/**
  * @brief 停止SDIO读等待操作
  * @param NewState 新状态 (ENABLE/DISABLE)
  */
void SDIO_StopSDIOReadWait(FunctionalState NewState);

/**
  * @brief 设置读等待检测模式
  * @param SDIO_ReadWaitMode 检测模式：
  *        - SDIO_ReadWaitMode_CLK   时钟停止模式
  *        - SDIO_ReadWaitMode_DATA2 数据线监测模式
  */
void SDIO_SetSDIOReadWaitMode(uint32_t SDIO_ReadWaitMode);

/**
  * @brief 控制SDIO特殊操作模式
  * @param NewState 新状态 (ENABLE/DISABLE)
  * @note 用于使能SDIO特定功能操作
  */
void SDIO_SetSDIOOperation(FunctionalState NewState);

/**
  * @brief 发送挂起传输命令
  * @param NewState 新状态 (ENABLE/DISABLE)
  * @note 允许临时暂停数据传输
  */
void SDIO_SendSDIOSuspendCmd(FunctionalState NewState);

/* CE-ATA模式管理函数 *********************************************************/
/**
  * @brief 控制命令完成信号
  * @param NewState 新状态 (ENABLE/DISABLE)
  * @note 用于通知CE-ATA设备命令完成
  */
void SDIO_CommandCompletionCmd(FunctionalState NewState);

/**
  * @brief CE-ATA中断控制
  * @param NewState 新状态 (ENABLE/DISABLE)
  * @note 使能/禁用CE-ATA专用中断
  */
void SDIO_CEATAITCmd(FunctionalState NewState);

/**
  * @brief 发送CE-ATA特殊命令
  * @param NewState 新状态 (ENABLE/DISABLE)
  */
void SDIO_SendCEATACmd(FunctionalState NewState);

/* DMA传输管理函数 ************************************************************/
/**
  * @brief DMA传输控制
  * @param NewState 新状态 (ENABLE/DISABLE)
  * @note 使能前需配置DMA控制器
  */
void SDIO_DMACmd(FunctionalState NewState);

/* 中断与标志管理函数 *********************************************************/
/**
  * @brief 中断源配置
  * @param SDIO_IT 中断类型，参考SDIO_IT_xx定义
  * @param NewState 新状态 (ENABLE/DISABLE)
  */
void SDIO_ITConfig(uint32_t SDIO_IT, FunctionalState NewState);

/**
  * @brief 获取标志位状态
  * @param SDIO_FLAG 待查询标志位，参考SDIO_FLAG_xx定义
  * @retval 标志位状态 (SET/RESET)
  */
FlagStatus SDIO_GetFlagStatus(uint32_t SDIO_FLAG);

/**
  * @brief 清除指定标志位
  * @param SDIO_FLAG 需清除的标志位
  * @note 部分标志位需要特殊清除顺序
  */
void SDIO_ClearFlag(uint32_t SDIO_FLAG);

/**
  * @brief 获取中断状态
  * @param SDIO_IT 中断类型
  * @retval 中断状态 (SET/RESET)
  */
ITStatus SDIO_GetITStatus(uint32_t SDIO_IT);

/**
  * @brief 清除中断挂起位
  * @param SDIO_IT 需清除的中断类型
  */
void SDIO_ClearITPendingBit(uint32_t SDIO_IT);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_SDIO_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
  ******************************************************************************
  * @file    stm32f4xx_fmc.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the FMC firmware 
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
#ifndef __STM32F4xx_FMC_H
#define __STM32F4xx_FMC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup FMC
  * @{
  */
/** @addtogroup FMC
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  NOR/SRAM存储器时序配置结构体
  * @note   用于配置FSMC控制器的访问时序参数，包含6个关键时序参数和访问模式设置
  *         适用于异步NOR Flash、SRAM、PSRAM等存储器类型
  */
typedef struct
{
  /**
   * @brief 地址建立时间配置
   * @note 单位：HCLK周期数，范围0-15个周期
   * @note 同步NOR Flash无需配置此参数
   * @note 该参数定义地址信号有效后到读/写操作开始的时间间隔
   */
  uint32_t FMC_AddressSetupTime;

  /**
   * @brief 地址保持时间配置
   * @note 单位：HCLK周期数，范围1-15个周期
   * @note 同步NOR Flash无需配置此参数
   * @note 定义读/写操作结束后地址信号的保持时间
   */
  uint32_t FMC_AddressHoldTime;

  /**
   * @brief 数据建立时间配置
   * @note 单位：HCLK周期数，范围1-255个周期
   * @note 适用于SRAM、ROM和异步多路复用NOR Flash
   * @note 定义读操作中数据采样窗口长度或写操作数据保持时间
   */
  uint32_t FMC_DataSetupTime;

  /**
   * @brief 总线周转时间配置
   * @note 单位：HCLK周期数，范围0-15个周期
   * @note 仅用于多路复用NOR Flash
   * @note 定义读操作到写操作切换时的总线空闲时间
   */
  uint32_t FMC_BusTurnAroundDuration;

  /**
   * @brief 时钟分频系数
   * @note 分频值范围1-15，实际时钟周期数=参数值+1
   * @note 仅用于同步突发访问模式
   * @note 控制CLK时钟输出信号的频率，影响同步存储器的操作时序
   */
  uint32_t FMC_CLKDivision;

  /**
   * @brief 数据延迟周期数
   * @note 配置范围0-15个时钟周期
   * @note 不同存储器的应用场景：
   *        - CRAM必须设置为0
   *        - 异步访问模式无需配置
   *        - 同步NOR Flash需根据存储器规格书设置
   * @note 定义发出命令到第一个数据有效之间的等待周期
   */
  uint32_t FMC_DataLatency;

  /**
   * @brief 异步访问模式选择
   * @note 具体模式参考@ref FMC_Access_Mode枚举定义
   * @note 决定地址/数据总线的复用方式和控制信号时序
   */
  uint32_t FMC_AccessMode;
}FMC_NORSRAMTimingInitTypeDef;

/**
  * @brief  FMC NOR/SRAM初始化结构体
  * @note   包含存储器类型、数据宽度、操作模式等15个配置参数
  *         通过组合时序参数结构体实现灵活的存储器接口配置
  */
typedef struct
{
  /**
   * @brief 存储区块选择
   * @note 具体值参考@ref FMC_NORSRAM_Bank定义
   * @note 指定配置的存储区块号（Bank1-Bank4）
   */
  uint32_t FMC_Bank;

  /**
   * @brief 地址数据复用配置
   * @note 参考@ref FMC_Data_Address_Bus_Multiplexing
   * @note 当使能时，地址和数据信号共用数据总线
   */
  uint32_t FMC_DataAddressMux;

  /**
   * @brief 存储器类型选择
   * @note 参考@ref FMC_Memory_Type
   * @note 可选SRAM、PSRAM、NOR Flash等类型
   */
  uint32_t FMC_MemoryType;

  /**
   * @brief 存储器数据总线宽度
   * @note 参考@ref FMC_NORSRAM_Data_Width
   * @note 支持8/16位总线配置，需与硬件连接匹配
   */
  uint32_t FMC_MemoryDataWidth;

  /**
   * @brief 突发访问模式使能
   * @note 参考@ref FMC_Burst_Access_Mode
   * @note 仅同步Flash支持突发模式
   */
  uint32_t FMC_BurstAccessMode;

  /**
   * @brief 等待信号极性配置
   * @note 参考@ref FMC_Wait_Signal_Polarity
   * @note 决定WAIT信号的有效电平（高/低）
   */
  uint32_t FMC_WaitSignalPolarity;

  /**
   * @brief 回环突发模式配置
   * @note 参考@ref FMC_Wrap_Mode
   * @note 优化顺序数据访问的突发传输效率
   */
  uint32_t FMC_WrapMode;

  /**
   * @brief 等待信号有效时机
   * @note 参考@ref FMC_Wait_Timing
   * @note 配置WAIT信号在等待状态前还是期间有效
   */
  uint32_t FMC_WaitSignalActive;

  /**
   * @brief 写操作使能控制
   * @note 参考@ref FMC_Write_Operation
   * @note 禁用时可防止意外写入操作
   */
  uint32_t FMC_WriteOperation;

  /**
   * @brief 等待信号插入控制
   * @note 参考@ref FMC_Wait_Signal
   * @note 使能后通过WAIT引脚插入等待状态
   */
  uint32_t FMC_WaitSignal;

  /**
   * @brief 扩展模式使能
   * @note 参考@ref FMC_Extended_Mode
   * @note 使能后支持独立的读写时序配置
   */
  uint32_t FMC_ExtendedMode;

  /**
   * @brief 异步等待使能
   * @note 参考@ref FMC_AsynchronousWait
   * @note 使能异步传输期间的等待状态插入
   */
  uint32_t FMC_AsynchronousWait;

  /**
   * @brief 突发写操作配置
   * @note 参考@ref FMC_Write_Burst
   * @note 使能后提高连续写入操作的效率
   */
  uint32_t FMC_WriteBurst;

  /**
   * @brief 连续时钟输出控制
   * @note 参考@ref FMC_Continous_Clock
   * @note 仅在Bank1有效，控制CLK时钟是否持续输出
   */
  uint32_t FMC_ContinousClock;

  /**
   * @brief 读写共用时序配置
   * @note 当扩展模式禁用时，读写操作使用此时序
   * @note 必须指向有效的时序配置结构体
   */
  FMC_NORSRAMTimingInitTypeDef* FMC_ReadWriteTimingStruct;

  /**
   * @brief 独立写时序配置
   * @note 当扩展模式使能时，写操作使用此时序
   * @note 需与读时序配合使用优化性能
   */
  FMC_NORSRAMTimingInitTypeDef* FMC_WriteTimingStruct;
}FMC_NORSRAMInitTypeDef;

/**
  * @brief  NAND/PCCARD存储器时序配置结构体
  * @note   包含4个关键时序参数，用于配置NAND Flash和PCCARD存储器的访问时序
  *         适用于Common/Attribute/I/O三种存储空间的时序配置
  */
typedef struct
{
  /**
   * @brief 地址/命令建立时间
   * @note 单位：HCLK周期数，范围0-255
   * @note 定义地址信号有效到命令信号有效的时间间隔
   * @note 对应NAND操作时序图中的tCLS/tALS参数配置
   */
  uint32_t FMC_SetupTime;

  /**
   * @brief 命令有效保持时间
   * @note 单位：HCLK周期数，范围0-255
   * @note 定义命令信号保持有效的最小持续时间
   * @note 对应NAND时序中的tWP/tRP信号宽度要求
   */
  uint32_t FMC_WaitSetupTime;

  /**
   * @brief 地址/数据保持时间
   * @note 单位：HCLK周期数，范围0-255
   * @note 定义命令失效后地址/数据信号的保持时间
   * @note 对应NAND时序中的tCLH/tALH参数配置
   */
  uint32_t FMC_HoldSetupTime;

  /**
   * @brief 数据总线高阻态时间
   * @note 单位：HCLK周期数，范围0-255
   * @note 定义写操作后总线保持高阻态的等待时间
   * @note 防止总线冲突，对应tDH时序参数
   */
  uint32_t FMC_HiZSetupTime;
}FMC_NAND_PCCARDTimingInitTypeDef;

/**
  * @brief  FMC NAND初始化结构体
  * @note   包含NAND存储器的控制参数和时序配置，支持8种硬件特性配置
  *         支持Common/Attribute双存储空间的独立时序配置
  */
typedef struct
{
  /**
   * @brief 存储区块选择
   * @note 参考@ref FMC_NAND_Bank定义
   * @note 指定配置的NAND存储区块（Bank2/Bank3）
   */
  uint32_t FMC_Bank;

  /**
   * @brief 等待特性使能
   * @note 参考@ref FMC_Wait_feature
   * @note 使能后通过nWAIT引脚插入等待状态
   */
  uint32_t FMC_Waitfeature;

  /**
   * @brief 存储器数据总线宽度
   * @note 参考@ref FMC_NAND_Data_Width
   * @note 支持8/16位配置，需与NAND芯片位宽匹配
   */
  uint32_t FMC_MemoryDataWidth;

  /**
   * @brief ECC校验使能
   * @note 参考@ref FMC_ECC
   * @note 使能后自动计算ECC校验码，需配合ECCPageSize使用
   */
  uint32_t FMC_ECC;

  /**
   * @brief ECC页大小配置
   * @note 参考@ref FMC_ECC_Page_Size
   * @note 定义ECC计算的数据块大小（256/512/1024/2048字节）
   */
  uint32_t FMC_ECCPageSize;

  /**
   * @brief CLE到RE的延迟时间
   * @note 单位：HCLK周期数，范围0-255
   * @note 定义命令锁存使能(CLE)到读使能(RE)的延迟
   * @note 对应NAND时序参数tCLR
   */
  uint32_t FMC_TCLRSetupTime;

  /**
   * @brief ALE到RE的延迟时间
   * @note 单位：HCLK周期数，范围0-255
   * @note 定义地址锁存使能(ALE)到读使能(RE)的延迟
   * @note 对应NAND时序参数tAR
   */
  uint32_t FMC_TARSetupTime;

  /**
   * @brief 公共空间时序配置
   * @note 指向Common存储空间的时序结构体
   * @note 配置命令/地址/数据的基本操作时序
   */
  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_CommonSpaceTimingStruct;

  /**
   * @brief 属性空间时序配置
   * @note 指向Attribute存储空间的时序结构体
   * @note 用于扩展属性操作的时序配置
   */
  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_AttributeSpaceTimingStruct;
}FMC_NANDInitTypeDef;

/**
  * @brief  FMC PCCARD初始化结构体
  * @note   包含PCCARD控制器的3种存储空间时序配置，支持CF卡、PCMCIA设备
  *         提供IO空间独立时序配置能力，优化多功能卡访问性能
  */
typedef struct
{
  /**
   * @brief 等待特性使能
   * @note 参考@ref FMC_Wait_feature
   * @note 控制nWAIT信号在突发传输中的使用
   */
  uint32_t FMC_Waitfeature;

  /**
   * @brief CLE到RE延迟时间
   * @note 单位：HCLK周期数，范围0-255
   * @note 定义命令锁存到读使能的间隔，对应PCCARD时序tCLR
   */
  uint32_t FMC_TCLRSetupTime;

  /**
   * @brief ALE到RE延迟时间
   * @note 单位：HCLK周期数，范围0-255
   * @note 定义地址锁存到读使能的间隔，对应时序tAR
   */
  uint32_t FMC_TARSetupTime;

  /**
   * @brief 公共空间时序配置
   * @note 配置PCCARD公共存储区域的操作时序
   * @note 影响I/O模式下的命令传输时序
   */
  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_CommonSpaceTimingStruct;

  /**
   * @brief 属性空间时序配置
   * @note 配置PCCARD属性存储区域的操作时序
   * @note 用于扩展寄存器访问时序控制
   */
  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_AttributeSpaceTimingStruct;

  /**
   * @brief IO空间时序配置
   * @note 配置PCCARD IO空间的特殊操作时序
   * @note 控制PCMCIA接口的I/O模式数据传输
   */
  FMC_NAND_PCCARDTimingInitTypeDef*  FMC_IOSpaceTimingStruct;
}FMC_PCCARDInitTypeDef;

/**
  * @brief  SDRAM时序配置结构体
  * @note   包含7个关键时序参数，影响SDRAM的刷新、预充电等操作
  *         所有参数单位均为SDRAM时钟周期数，范围1-16个周期
  */
typedef struct
{
  /**
   * @brief 加载模式寄存器到激活延迟
   * @note 对应JEDEC标准tMRD参数
   * @note 定义LMR命令后到激活命令的最小间隔
   */
  uint32_t FMC_LoadToActiveDelay;

  /**
   * @brief 自刷新退出延迟
   * @note 对应JEDEC标准tXSR参数
   * @note 自刷新模式退出到发送激活命令的等待时间
   */
  uint32_t FMC_ExitSelfRefreshDelay;

  /**
   * @brief 自刷新周期时间
   * @note 对应JEDEC标准tRFC参数
   * @note 定义两次自刷新命令之间的最小间隔
   */
  uint32_t FMC_SelfRefreshTime;

  /**
   * @brief 行周期延迟
   * @note 对应JEDEC标准tRC参数
   * @note 定义行激活到预充电的最小周期数
   */
  uint32_t FMC_RowCycleDelay;

  /**
   * @brief 写恢复时间
   * @note 对应JEDEC标准tWR参数
   * @note 写操作完成后到预充电命令的最小间隔
   */
  uint32_t FMC_WriteRecoveryTime;

  /**
   * @brief 行预充电延迟
   * @note 对应JEDEC标准tRP参数
   * @note 预充电命令到行激活命令的最小间隔
   */
  uint32_t FMC_RPDelay;

  /**
   * @brief 行到列延迟
   * @note 对应JEDEC标准tRCD参数
   * @note 行激活到读/写命令的最小间隔
   */
  uint32_t FMC_RCDDelay;
}FMC_SDRAMTimingInitTypeDef;

/**
  * @brief  SDRAM命令配置结构体
  * @note   用于发送特定命令到SDRAM存储器，支持4种命令参数配置
  *         可配置自动刷新次数和模式寄存器内容
  */
typedef struct
{
  /**
   * @brief 命令模式选择
   * @note 参考@ref FMC_Command_Mode
   * @note 可选：正常模式、时钟配置使能、预充电、自刷新、加载模式寄存器等
   */
  uint32_t FMC_CommandMode;

  /**
   * @brief 目标存储区块
   * @note 参考@ref FMC_Command_Target
   * @note 选择命令作用的SDRAM Bank（Bank1/Bank2/双Bank）
   */
  uint32_t FMC_CommandTarget;

  /**
   * @brief 自动刷新次数
   * @note 范围1-16次连续刷新
   * @note 用于初始化阶段的自动刷新序列配置
   */
  uint32_t FMC_AutoRefreshNumber;

  /**
   * @brief 模式寄存器配置值
   * @note 定义CAS延迟、突发类型等关键参数
   * @note 需根据SDRAM芯片规格书计算配置值
   */
  uint32_t FMC_ModeRegisterDefinition;
}FMC_SDRAMCommandTypeDef;

/**
  * @brief  FMC SDRAM初始化结构体
  * @note   包含SDRAM物理特性和操作模式的12项关键配置
  *         支持SDRAM芯片的全功能参数配置
  */
typedef struct
{
  /**
   * @brief 存储区块选择
   * @note 参考@ref FMC_SDRAM_Bank
   * @note 指定配置的SDRAM存储区块（Bank1/Bank2）
   */
  uint32_t FMC_Bank;

  /**
   * @brief 列地址位数
   * @note 参考@ref FMC_ColumnBits_Number
   * @note 可选8/9/10/11位，需与SDRAM芯片规格匹配
   */
  uint32_t FMC_ColumnBitsNumber;

  /**
   * @brief 行地址位数
   * @note 参考@ref FMC_RowBits_Number
   * @note 可选11/12/13位，决定存储阵列的行数
   */
  uint32_t FMC_RowBitsNumber;

  /**
   * @brief 存储器数据总线宽度
   * @note 参考@ref FMC_SDMemory_Data_Width
   * @note 支持8/16/32位配置，需与硬件连接一致
   */
  uint32_t FMC_SDMemoryDataWidth;

  /**
   * @brief 内部存储Bank数量
   * @note 参考@ref FMC_InternalBank_Number
   * @note 通常配置4个内部Bank（对应2^2）
   */
  uint32_t FMC_InternalBankNumber;

  /**
   * @brief CAS潜伏周期
   * @note 参考@ref FMC_CAS_Latency
   * @note 可配置1/2/3个时钟周期，影响读取延迟
   */
  uint32_t FMC_CASLatency;

  /**
   * @brief 写保护使能
   * @note 参考@ref FMC_Write_Protection
   * @note 禁用时可防止意外写入操作
   */
  uint32_t FMC_WriteProtection;

  /**
   * @brief 时钟周期配置
   * @note 参考@ref FMC_SDClock_Period
   * @note 控制SDRAM时钟使能和频率切换时的稳定性
   */
  uint32_t FMC_SDClockPeriod;

  /**
   * @brief 读突发模式使能
   * @note 参考@ref FMC_Read_Burst
   * @note 使能后预取数据到读FIFO提升性能
   */
  uint32_t FMC_ReadBurst;

  /**
   * @brief 读管道延迟
   * @note 参考@ref FMC_ReadPipe_Delay
   * @note 补偿时钟域交叉的延迟，通常保持默认2周期
   */
  uint32_t FMC_ReadPipeDelay;

  /**
   * @brief 时序参数配置
   * @note 指向FMC_SDRAMTimingInitTypeDef结构体
   * @note 包含tRCD、tRP、tWR等关键时序参数
   */
  FMC_SDRAMTimingInitTypeDef* FMC_SDRAMTimingStruct;
}FMC_SDRAMInitTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup FMC_Exported_Constants FMC外设常量定义
  * @{
  */ 

/** @defgroup FMC_NORSRAM_Bank NOR/SRAM存储库选择
  * @{
  */
#define FMC_Bank1_NORSRAM1    ((uint32_t)0x00000000) /*!< Bank1 NOR/SRAM1 存储区 */
#define FMC_Bank1_NORSRAM2    ((uint32_t)0x00000002) /*!< Bank1 NOR/SRAM2 存储区 */
#define FMC_Bank1_NORSRAM3    ((uint32_t)0x00000004) /*!< Bank1 NOR/SRAM3 存储区 */
#define FMC_Bank1_NORSRAM4    ((uint32_t)0x00000006) /*!< Bank1 NOR/SRAM4 存储区 */

/* 参数有效性验证宏：检查NOR/SRAM存储库选择是否合法 */
#define IS_FMC_NORSRAM_BANK(BANK) (((BANK) == FMC_Bank1_NORSRAM1) || \
                                   ((BANK) == FMC_Bank1_NORSRAM2) || \
                                   ((BANK) == FMC_Bank1_NORSRAM3) || \
                                   ((BANK) == FMC_Bank1_NORSRAM4))
/**
  * @}
  */

/** @defgroup FMC_NAND_Bank NAND存储库选择
  * @{
  */  
#define FMC_Bank2_NAND        ((uint32_t)0x00000010) /*!< Bank2 NAND存储区 */
#define FMC_Bank3_NAND        ((uint32_t)0x00000100) /*!< Bank3 NAND存储区 */

/* 验证NAND存储库选择合法性 */
#define IS_FMC_NAND_BANK(BANK) (((BANK) == FMC_Bank2_NAND) || \
                                ((BANK) == FMC_Bank3_NAND))
/**
  * @}
  */

/** @defgroup FMC_PCCARD_Bank PCCARD存储库选择
  * @{
  */    
#define FMC_Bank4_PCCARD      ((uint32_t)0x00001000) /*!< Bank4 PCCARD存储区 */
/**
  * @}                                                         
  */

/** @defgroup FMC_SDRAM_Bank SDRAM存储库选择
  * @{
  */
#define FMC_Bank1_SDRAM       ((uint32_t)0x00000000) /*!< Bank1 SDRAM存储区 */
#define FMC_Bank2_SDRAM       ((uint32_t)0x00000001) /*!< Bank2 SDRAM存储区 */

/* 验证SDRAM存储库选择合法性 */
#define IS_FMC_SDRAM_BANK(BANK) (((BANK) == FMC_Bank1_SDRAM) || \
                                 ((BANK) == FMC_Bank2_SDRAM))
/**
  * @}
  */                               
                          
/** @defgroup FMC_Data_Address_Bus_Multiplexing 数据地址总线复用模式
  * @{
  */
#define FMC_DataAddressMux_Disable  ((uint32_t)0x00000000) /*!< 禁用地址数据复用 */
#define FMC_DataAddressMux_Enable   ((uint32_t)0x00000002) /*!< 使能地址数据复用 */

/* 验证复用模式参数合法性 */
#define IS_FMC_MUX(MUX) (((MUX) == FMC_DataAddressMux_Disable) || \
                         ((MUX) == FMC_DataAddressMux_Enable))
/**
  * @}
  */

/** @defgroup FMC_Memory_Type 存储器类型选择
  * @{
  */
#define FMC_MemoryType_SRAM    ((uint32_t)0x00000000) /*!< SRAM存储器 */
#define FMC_MemoryType_PSRAM   ((uint32_t)0x00000004) /*!< PSRAM伪静态存储器 */ 
#define FMC_MemoryType_NOR     ((uint32_t)0x00000008) /*!< NOR闪存存储器 */

/* 验证存储器类型参数合法性 */
#define IS_FMC_MEMORY(MEMORY) (((MEMORY) == FMC_MemoryType_SRAM) || \
                               ((MEMORY) == FMC_MemoryType_PSRAM)|| \
                               ((MEMORY) == FMC_MemoryType_NOR))
/**
  * @}
  */

/** @defgroup FMC_NORSRAM_Data_Width NOR/SRAM数据总线宽度
  * @{
  */
#define FMC_NORSRAM_MemoryDataWidth_8b  ((uint32_t)0x00000000) /*!< 8位数据总线 */
#define FMC_NORSRAM_MemoryDataWidth_16b ((uint32_t)0x00000010) /*!< 16位数据总线 */
#define FMC_NORSRAM_MemoryDataWidth_32b ((uint32_t)0x00000020) /*!< 32位数据总线 */

/* 验证总线宽度参数合法性 */
#define IS_FMC_NORSRAM_MEMORY_WIDTH(WIDTH) (((WIDTH) == FMC_NORSRAM_MemoryDataWidth_8b)  || \
                                            ((WIDTH) == FMC_NORSRAM_MemoryDataWidth_16b) || \
                                            ((WIDTH) == FMC_NORSRAM_MemoryDataWidth_32b))
/** @defgroup FMC_Burst_Access_Mode 突发访问模式
  * @{
  */
#define FMC_BurstAccessMode_Disable  ((uint32_t)0x00000000) /*!< 禁用突发传输模式 */
#define FMC_BurstAccessMode_Enable   ((uint32_t)0x00000100) /*!< 使能突发传输模式 */

/* 验证突发模式参数合法性 */
#define IS_FMC_BURSTMODE(STATE) (((STATE) == FMC_BurstAccessMode_Disable) || \
                                  ((STATE) == FMC_BurstAccessMode_Enable))
/**
  * @}
  */
    
/** @defgroup FMC_AsynchronousWait 异步等待模式
  * @{
  */
#define FMC_AsynchronousWait_Disable ((uint32_t)0x00000000) /*!< 禁用异步传输等待 */
#define FMC_AsynchronousWait_Enable  ((uint32_t)0x00008000) /*!< 使能异步传输等待 */

/* 验证异步等待参数合法性 */
#define IS_FMC_ASYNWAIT(STATE) (((STATE) == FMC_AsynchronousWait_Disable) || \
                                 ((STATE) == FMC_AsynchronousWait_Enable))
/**
  * @}
  */

/** @defgroup FMC_Wait_Signal_Polarity 等待信号极性
  * @{
  */
#define FMC_WaitSignalPolarity_Low   ((uint32_t)0x00000000) /*!< WAIT信号低电平有效 */
#define FMC_WaitSignalPolarity_High  ((uint32_t)0x00000200) /*!< WAIT信号高电平有效 */

/* 验证等待信号极性合法性 */
#define IS_FMC_WAIT_POLARITY(POLARITY) (((POLARITY) == FMC_WaitSignalPolarity_Low) || \
                                         ((POLARITY) == FMC_WaitSignalPolarity_High))
/**
  * @}
  */

/** @defgroup FMC_Wrap_Mode 包裹突发模式
  * @{
  */
#define FMC_WrapMode_Disable         ((uint32_t)0x00000000) /*!< 禁用地址回环 */
#define FMC_WrapMode_Enable          ((uint32_t)0x00000400) /*!< 使能地址回环 */

/* 验证包裹模式参数合法性 */
#define IS_FMC_WRAP_MODE(MODE) (((MODE) == FMC_WrapMode_Disable) || \
                                 ((MODE) == FMC_WrapMode_Enable))
/**
  * @}
  */

/** @defgroup FMC_Wait_Timing 等待信号激活时机
  * @{
  */
#define FMC_WaitSignalActive_BeforeWaitState ((uint32_t)0x00000000) /*!< 在等待状态前断言WAIT */
#define FMC_WaitSignalActive_DuringWaitState ((uint32_t)0x00000800) /*!< 在等待状态期间断言WAIT */

/* 验证等待信号时机参数合法性 */
#define IS_FMC_WAIT_SIGNAL_ACTIVE(ACTIVE) (((ACTIVE) == FMC_WaitSignalActive_BeforeWaitState) || \
                                            ((ACTIVE) == FMC_WaitSignalActive_DuringWaitState))
/**
  * @}
  */

/** @defgroup FMC_Write_Operation 写操作控制
  * @{
  */
#define FMC_WriteOperation_Disable  ((uint32_t)0x00000000) /*!< 禁用存储区写操作 */
#define FMC_WriteOperation_Enable   ((uint32_t)0x00001000) /*!< 启用存储区写操作 */

/* 验证写操作控制参数合法性 */
#define IS_FMC_WRITE_OPERATION(OPERATION) (((OPERATION) == FMC_WriteOperation_Disable) || \
                                            ((OPERATION) == FMC_WriteOperation_Enable))
      /** @defgroup FMC_Wait_Signal 等待信号使能
  * @{
  */
#define FMC_WaitSignal_Disable    ((uint32_t)0x00000000) /*!< 禁用等待信号插入 */
#define FMC_WaitSignal_Enable     ((uint32_t)0x00002000) /*!< 使能等待信号插入 */

/* 验证等待信号使能参数合法性 */
#define IS_FMC_WAITE_SIGNAL(SIGNAL) (((SIGNAL) == FMC_WaitSignal_Disable) || \
                                      ((SIGNAL) == FMC_WaitSignal_Enable))
/**
  * @}
  */

/** @defgroup FMC_Extended_Mode 扩展模式
  * @{
  */
#define FMC_ExtendedMode_Disable  ((uint32_t)0x00000000) /*!< 禁用扩展模式 */
#define FMC_ExtendedMode_Enable   ((uint32_t)0x00004000) /*!< 使能扩展模式 */

/* 验证扩展模式参数合法性 */
#define IS_FMC_EXTENDED_MODE(MODE) (((MODE) == FMC_ExtendedMode_Disable) || \
                                     ((MODE) == FMC_ExtendedMode_Enable))
/**
  * @}
  */

/** @defgroup FMC_Write_Burst 写突发模式
  * @{
  */
#define FMC_WriteBurst_Disable    ((uint32_t)0x00000000) /*!< 禁用写突发传输 */
#define FMC_WriteBurst_Enable     ((uint32_t)0x00080000) /*!< 使能写突发传输 */

/* 验证写突发模式参数合法性 */
#define IS_FMC_WRITE_BURST(BURST) (((BURST) == FMC_WriteBurst_Disable) || \
                                    ((BURST) == FMC_WriteBurst_Enable))
/**
  * @}
  */
  
/** @defgroup FMC_Continous_Clock 连续时钟配置
  * @{
  */
#define FMC_CClock_SyncOnly       ((uint32_t)0x00000000) /*!< 仅同步时钟模式 */
#define FMC_CClock_SyncAsync      ((uint32_t)0x00100000) /*!< 同步+异步时钟模式 */

/* 验证时钟模式参数合法性 */
#define IS_FMC_CONTINOUS_CLOCK(CCLOCK) (((CCLOCK) == FMC_CClock_SyncOnly) || \
                                        ((CCLOCK) == FMC_CClock_SyncAsync))
/**
  * @}
  */  

/** @defgroup FMC_Address_Setup_Time 地址建立时间验证
  * @{
  */
#define IS_FMC_ADDRESS_SETUP_TIME(TIME) ((TIME) <= 15) /*!< 范围校验：0-15个HCLK周期 */
/**
  * @}
  */

/** @defgroup FMC_Address_Hold_Time 地址保持时间验证
  * @{
  */
#define IS_FMC_ADDRESS_HOLD_TIME(TIME) (((TIME) > 0) && ((TIME) <= 15)) /*!< 范围校验：1-15周期 */
/**
  * @}
  */

/** @defgroup FMC_Data_Setup_Time 数据建立时间验证
  * @{
  */
#define IS_FMC_DATASETUP_TIME(TIME) (((TIME) > 0) && ((TIME) <= 255)) /*!< 范围校验：1-255周期 */
/**
  * @}
  */

/** @defgroup FMC_Bus_Turn_around_Duration 总线周转时间验证
  * @{
  */
#define IS_FMC_TURNAROUND_TIME(TIME) ((TIME) <= 15) /*!< 范围校验：0-15周期 */
/**
  * @}
  */

/** @defgroup FMC_CLK_Division 时钟分频验证
  * @{
  */
#define IS_FMC_CLK_DIV(DIV) (((DIV) > 0) && ((DIV) <= 15)) /*!< 范围校验：1-15分频 */
/**
  * @}
  */

/** @defgroup FMC_Data_Latency 数据延迟验证
  * @{
  */
#define IS_FMC_DATA_LATENCY(LATENCY) ((LATENCY) <= 15) /*!< 范围校验：0-15周期 */

/**
  * @}
  */
/** @defgroup FMC_Access_Mode NOR/SRAM访问模式
  * @{
  */
#define FMC_AccessMode_A  ((uint32_t)0x00000000) /*!< 模式A：控制信号独立时序 */
#define FMC_AccessMode_B  ((uint32_t)0x10000000) /*!< 模式B：地址在NWE前有效 */ 
#define FMC_AccessMode_C  ((uint32_t)0x20000000) /*!< 模式C：地址在NOE前有效 */
#define FMC_AccessMode_D  ((uint32_t)0x30000000) /*!< 模式D：延长地址保持时间 */

/* 验证访问模式参数合法性 */
#define IS_FMC_ACCESS_MODE(MODE) (((MODE) == FMC_AccessMode_A)  || \
                                   ((MODE) == FMC_AccessMode_B) || \
                                   ((MODE) == FMC_AccessMode_C) || \
                                   ((MODE) == FMC_AccessMode_D))
/**
  * @}
  */

/** @defgroup FMC_NAND_PCCARD_Controller NAND/PCCARD控制器配置
  * @{
  */

/** @defgroup FMC_Wait_feature 等待特性使能
  * @{
  */
#define FMC_Waitfeature_Disable  ((uint32_t)0x00000000) /*!< 禁用等待特性 */
#define FMC_Waitfeature_Enable   ((uint32_t)0x00000002) /*!< 使能等待特性 */

/* 验证等待特性参数合法性 */
#define IS_FMC_WAIT_FEATURE(FEATURE) (((FEATURE) == FMC_Waitfeature_Disable) || \
                                       ((FEATURE) == FMC_Waitfeature_Enable))
/**
  * @}
  */

/** @defgroup FMC_NAND_Data_Width NAND数据总线宽度
  * @{
  */
#define FMC_NAND_MemoryDataWidth_8b  ((uint32_t)0x00000000) /*!< 8位NAND总线 */
#define FMC_NAND_MemoryDataWidth_16b ((uint32_t)0x00000010) /*!< 16位NAND总线 */

/* 验证总线宽度参数合法性 */
#define IS_FMC_NAND_MEMORY_WIDTH(WIDTH) (((WIDTH) == FMC_NAND_MemoryDataWidth_8b) || \
                                         ((WIDTH) == FMC_NAND_MemoryDataWidth_16b))
/**
  * @}
  */

/** @defgroup FMC_ECC ECC校验控制
  * @{
  */
#define FMC_ECC_Disable  ((uint32_t)0x00000000) /*!< 禁用ECC校验 */
#define FMC_ECC_Enable   ((uint32_t)0x00000040) /*!< 使能ECC校验 */

/* 验证ECC状态参数合法性 */
#define IS_FMC_ECC_STATE(STATE) (((STATE) == FMC_ECC_Disable) || \
                                  ((STATE) == FMC_ECC_Enable))
/** @defgroup FMC_ECC_Page_Size ECC页大小配置
  * @{
  */
#define FMC_ECCPageSize_256Bytes  ((uint32_t)0x00000000) /*!< 256字节页，每页3字节ECC */
#define FMC_ECCPageSize_512Bytes  ((uint32_t)0x00020000) /*!< 512字节页，每页3字节ECC */
#define FMC_ECCPageSize_1024Bytes ((uint32_t)0x00040000) /*!< 1024字节页，每页4字节ECC */
#define FMC_ECCPageSize_2048Bytes ((uint32_t)0x00060000) /*!< 2048字节页，每页4字节ECC */
#define FMC_ECCPageSize_4096Bytes ((uint32_t)0x00080000) /*!< 4096字节页，每页5字节ECC */
#define FMC_ECCPageSize_8192Bytes ((uint32_t)0x000A0000) /*!< 8192字节页，每功6字节ECC */

/* 验证ECC页大小参数合法性 */
#define IS_FMC_ECCPAGE_SIZE(SIZE) (((SIZE) == FMC_ECCPageSize_256Bytes)   || \
                                    ((SIZE) == FMC_ECCPageSize_512Bytes)  || \
                                    ((SIZE) == FMC_ECCPageSize_1024Bytes) || \
                                    ((SIZE) == FMC_ECCPageSize_2048Bytes) || \
                                    ((SIZE) == FMC_ECCPageSize_4096Bytes) || \
                                    ((SIZE) == FMC_ECCPageSize_8192Bytes))

/**
  * @}
  */
/** @defgroup FMC_TCLR_Setup_Time CLE到RE延迟验证
  * @{
  */
#define IS_FMC_TCLR_TIME(TIME) ((TIME) <= 255) /*!< 范围校验：0-255个HCLK周期 */
/**
  * @brief 定义CLE信号有效到RE信号有效的延迟时间
  * @note 对应NAND时序参数tCLR，影响命令锁存时序
  */

/** @defgroup FMC_TAR_Setup_Time ALE到RE延迟验证
  * @{
  */
#define IS_FMC_TAR_TIME(TIME) ((TIME) <= 255) /*!< 范围校验：0-255个HCLK周期 */
/**
  * @brief 定义ALE信号有效到RE信号有效的延迟时间
  * @note 对应NAND时序参数tAR，影响地址锁存时序
  */

/** @defgroup FMC_Setup_Time 建立时间通用验证
  * @{
  */
#define IS_FMC_SETUP_TIME(TIME) ((TIME) <= 255) /*!< 通用建立时间范围校验 */
/**
  * @brief 通用信号建立时间基础验证
  * @note 用于地址/命令信号的建立时间校验
  */

/** @defgroup FMC_Wait_Setup_Time 等待时间验证
  * @{
  */
#define IS_FMC_WAIT_TIME(TIME) ((TIME) <= 255) /*!< 等待状态周期数校验 */
/**
  * @brief 定义等待状态的持续时间
  * @note 影响nWAIT信号的断言周期数
  */

/** @defgroup FMC_Hold_Setup_Time 保持时间验证
  * @{
  */
#define IS_FMC_HOLD_TIME(TIME) ((TIME) <= 255) /*!< 信号保持时间范围校验 */
/**
  * @brief 通用信号保持时间验证
  * @note 确保信号在失效后保持足够时间
  */

/** @defgroup FMC_HiZ_Setup_Time 高阻态时间验证
  * @{
  */
#define IS_FMC_HIZ_TIME(TIME) ((TIME) <= 255) /*!< 高阻态持续时间校验 */
/**
  * @brief 定义写操作后总线保持高阻态的时间
  * @note 防止总线竞争，对应时序参数tHZ
  */

/** @defgroup FMC_ColumnBits_Number 列地址位数配置
  * @{
  */
#define FMC_ColumnBits_Number_8b   ((uint32_t)0x00000000) /*!< 8位列地址 (256列) */
#define FMC_ColumnBits_Number_9b   ((uint32_t)0x00000001) /*!< 9位列地址 (512列) */
#define FMC_ColumnBits_Number_10b  ((uint32_t)0x00000002) /*!< 10位列地址 (1024列) */
#define FMC_ColumnBits_Number_11b  ((uint32_t)0x00000003) /*!< 11位列地址 (2048列) */

#define IS_FMC_COLUMNBITS_NUMBER(COLUMN) (((COLUMN) == FMC_ColumnBits_Number_8b)  || \
                                          ((COLUMN) == FMC_ColumnBits_Number_9b)  || \
                                          ((COLUMN) == FMC_ColumnBits_Number_10b) || \
                                          ((COLUMN) == FMC_ColumnBits_Number_11b))
/**
  * @brief 列地址位数与存储容量的关系：
  *        列数 = 2^ColumnBits 
  * @note 必须与存储器芯片规格书中的列地址位数严格匹配
  */

/** @defgroup FMC_RowBits_Number 行地址位数配置
  * @{
  */
#define FMC_RowBits_Number_11b  ((uint32_t)0x00000000) /*!< 11位行地址 (2048行) */
#define FMC_RowBits_Number_12b  ((uint32_t)0x00000004) /*!< 12位行地址 (4096行) */
#define FMC_RowBits_Number_13b  ((uint32_t)0x00000008) /*!< 13位行地址 (8192行) */

#define IS_FMC_ROWBITS_NUMBER(ROW) (((ROW) == FMC_RowBits_Number_11b) || \
                                    ((ROW) == FMC_RowBits_Number_12b) || \
                                    ((ROW) == FMC_RowBits_Number_13b))
/**
  * @brief 行地址位数与存储容量的关系：
  *        行数 = 2^RowBits 
  * @note 需与SDRAM芯片规格严格匹配，错误配置会导致寻址异常
  */

/** @defgroup FMC_SDMemory_Data_Width SDRAM数据总线宽度
  * @{
  */
#define FMC_SDMemory_Width_8b   ((uint32_t)0x00000000) /*!< 8位总线 (需Bank并联) */
#define FMC_SDMemory_Width_16b  ((uint32_t)0x00000010) /*!< 16位总线 */
#define FMC_SDMemory_Width_32b  ((uint32_t)0x00000020) /*!< 32位总线 (双16位Bank并联) */

#define IS_FMC_SDMEMORY_WIDTH(WIDTH) (((WIDTH) == FMC_SDMemory_Width_8b)  || \
                                      ((WIDTH) == FMC_SDMemory_Width_16b) || \
                                      ((WIDTH) == FMC_SDMemory_Width_32b))
/**
  * @brief 总线宽度与硬件连接关系：
  *        32位模式需使用两个16位SDRAM芯片并联
  */

/** @defgroup FMC_InternalBank_Number 内部存储Bank数量
  * @{
  */
#define FMC_InternalBank_Number_2  ((uint32_t)0x00000000) /*!< 2个内部Bank */
#define FMC_InternalBank_Number_4  ((uint32_t)0x00000040) /*!< 4个内部Bank */

#define IS_FMC_INTERNALBANK_NUMBER(NUMBER) (((NUMBER) == FMC_InternalBank_Number_2) || \
                                            ((NUMBER) == FMC_InternalBank_Number_4))
/**
  * @brief 影响存储阵列结构：
  *        4个Bank可减少行激活冲突，提升随机访问性能
  */

/** @defgroup FMC_CAS_Latency CAS潜伏周期
  * @{
  */
#define FMC_CAS_Latency_1  ((uint32_t)0x00000080) /*!< 1个时钟周期 */
#define FMC_CAS_Latency_2  ((uint32_t)0x00000100) /*!< 2个时钟周期 (最常见) */
#define FMC_CAS_Latency_3  ((uint32_t)0x00000180) /*!< 3个时钟周期 */

#define IS_FMC_CAS_LATENCY(LATENCY) (((LATENCY) == FMC_CAS_Latency_1) || \
                                     ((LATENCY) == FMC_CAS_Latency_2) || \
                                     ((LATENCY) == FMC_CAS_Latency_3))
/**
  * @brief CAS延迟与时钟频率的关系：
  *        高频率需选择更高延迟值，需参考SDRAM规格书
  */

/** @defgroup FMC_Write_Protection 写保护配置
  * @{
  */
#define FMC_Write_Protection_Disable  ((uint32_t)0x00000000) /*!< 允许写入操作 */
#define FMC_Write_Protection_Enable   ((uint32_t)0x00000200) /*!< 禁止写入操作 */

#define IS_FMC_WRITE_PROTECTION(WRITE) (((WRITE) == FMC_Write_Protection_Disable) || \
                                        ((WRITE) == FMC_Write_Protection_Enable))
/**
  * @brief 写保护应用场景：
  *        保护引导程序区域，防止意外擦写
  */

/** @defgroup FMC_SDClock_Period 时钟周期配置
  * @{
  */
#define FMC_SDClock_Disable     ((uint32_t)0x00000000) /*!< 禁用时钟输出 */
#define FMC_SDClock_Period_2    ((uint32_t)0x00000800) /*!< 2个HCLK周期 */
#define FMC_SDClock_Period_3    ((uint32_t)0x00000C00) /*!< 3个HCLK周期 */

#define IS_FMC_SDCLOCK_PERIOD(PERIOD) (((PERIOD) == FMC_SDClock_Disable) || \
                                       ((PERIOD) == FMC_SDClock_Period_2) || \
                                       ((PERIOD) == FMC_SDClock_Period_3))
/**
  * @brief 时钟频率计算公式：
  *        SDClock = HCLK / (2 + Period)
  *        例如HCLK=100MHz，Period=2 → 25MHz
  */
/** @defgroup FMC_Read_Burst 读取突发模式
  * @{
  */
#define FMC_Read_Burst_Disable  ((uint32_t)0x00000000) /*!< 禁用突发预取机制 */
#define FMC_Read_Burst_Enable   ((uint32_t)0x00001000) /*!< 使能突发预取机制 */

#define IS_FMC_READ_BURST(RBURST) (((RBURST) == FMC_Read_Burst_Disable) || \
                                   ((RBURST) == FMC_Read_Burst_Enable))
/**
  * @brief 突发预取模式：
  *        - 使能后预取后续地址数据到读FIFO
  *        - 提升连续地址访问效率
  *        - 增加约10%功耗
  */

/** @defgroup FMC_ReadPipe_Delay 读取管道延迟
  * @{
  */
#define FMC_ReadPipe_Delay_0  ((uint32_t)0x00000000) /*!< 无额外延迟 */
#define FMC_ReadPipe_Delay_1  ((uint32_t)0x00002000) /*!< 1周期延迟 */
#define FMC_ReadPipe_Delay_2  ((uint32_t)0x00004000) /*!< 2周期延迟 */

#define IS_FMC_READPIPE_DELAY(DELAY) (((DELAY) == FMC_ReadPipe_Delay_0) || \
                                      ((DELAY) == FMC_ReadPipe_Delay_1) || \
                                      ((DELAY) == FMC_ReadPipe_Delay_2))
/**
  * @brief 补偿跨时钟域延迟：
  *        - 当时钟频率 > 100MHz时建议使用Delay1/2
  *        - 解决建立/保持时间违例问题
  */

/** @defgroup FMC_LoadToActive_Delay 加载到激活延迟
  * @{
  */
#define IS_FMC_LOADTOACTIVE_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))
/**
  * @brief 加载模式寄存器到行激活的最小延迟
  * @note 对应JEDEC标准tMRD参数，典型值2-5个周期
  */

/** @defgroup FMC_ExitSelfRefresh_Delay 自刷新退出延迟
  * @{
  */
#define IS_FMC_EXITSELFREFRESH_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))
/**
  * @brief 自刷新模式退出到有效操作的最小等待
  * @note 对应tXSR参数，典型值5-10个周期
  */

/** @defgroup FMC_SelfRefresh_Time 自刷新周期
  * @{
  */  
#define IS_FMC_SELFREFRESH_TIME(TIME) (((TIME) > 0) && ((TIME) <= 16))
/**
  * @brief 自刷新命令间隔周期数
  * @note 防止DRAM单元数据丢失，典型值7.8μs对应64个周期@64MHz
  */

/** @defgroup FMC_RowCycle_Delay 行循环延迟
  * @{
  */  
#define IS_FMC_ROWCYCLE_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))
/**
  * @brief 行激活到预充电的最小间隔
  * @note 对应tRC参数，决定存储阵列刷新率
  */

/** @defgroup FMC_Write_Recovery_Time 写恢复时间
  * @{
  */  
#define IS_FMC_WRITE_RECOVERY_TIME(TIME) (((TIME) > 0) && ((TIME) <= 16))
/**
  * @brief 写操作到预充电的最小间隔
  * @note 对应tWR参数，确保数据写入存储单元
  */

/** @defgroup FMC_RP_Delay 行预充电延迟
  * @{
  */  
#define IS_FMC_RP_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))
/**
  * @brief 行预充电命令的有效持续时间
  * @note 对应tRP参数，影响存储阵列关闭速度
  */

/** @defgroup FMC_RCD_Delay 行到列延迟
  * @{
  */  
#define IS_FMC_RCD_DELAY(DELAY) (((DELAY) > 0) && ((DELAY) <= 16))
/**
  * @brief 行激活到读/写命令的最小间隔
  * @note 对应tRCD参数，决定存储阵列访问速度
  */

/**
  * @}
  */  
  /** @defgroup FMC_Command_Mode SDRAM命令模式
  * @{
  */
#define FMC_Command_Mode_normal         ((uint32_t)0x00000000) /*!< 正常操作模式 */
#define FMC_Command_Mode_CLK_Enabled     ((uint32_t)0x00000001) /*!< 时钟使能模式 */
#define FMC_Command_Mode_PALL            ((uint32_t)0x00000002) /*!< 预充电所有Bank */
#define FMC_Command_Mode_AutoRefresh     ((uint32_t)0x00000003) /*!< 自动刷新命令 */
#define FMC_Command_Mode_LoadMode        ((uint32_t)0x00000004) /*!< 加载模式寄存器 */
#define FMC_Command_Mode_Selfrefresh     ((uint32_t)0x00000005) /*!< 进入自刷新模式 */
#define FMC_Command_Mode_PowerDown       ((uint32_t)0x00000006) /*!< 进入低功耗模式 */

#define IS_FMC_COMMAND_MODE(COMMAND) (((COMMAND) == FMC_Command_Mode_normal)    || \
                                      ((COMMAND) == FMC_Command_Mode_CLK_Enabled) || \
                                      ((COMMAND) == FMC_Command_Mode_PALL)      || \
                                      ((COMMAND) == FMC_Command_Mode_AutoRefresh) || \
                                      ((COMMAND) == FMC_Command_Mode_LoadMode)  || \
                                      ((COMMAND) == FMC_Command_Mode_Selfrefresh) || \
                                      ((COMMAND) == FMC_Command_Mode_PowerDown))
/**
  * @brief 命令模式详解：
  * - CLK_Enabled：在初始化前使能时钟输出
  * - PALL：关闭所有已激活的行（Bank预充电）
  * - LoadMode：配置模式寄存器(Burst长度/CAS延迟等)
  * - Selfrefresh：保持数据的同时降低功耗
  */

/** @defgroup FMC_Command_Target 命令作用目标
  * @{
  */
#define FMC_Command_Target_bank2        ((uint32_t)0x00000008) /*!< 仅Bank2 */
#define FMC_Command_Target_bank1        ((uint32_t)0x00000010) /*!< 仅Bank1 */
#define FMC_Command_Target_bank1_2      ((uint32_t)0x00000018) /*!< Bank1和Bank2 */

#define IS_FMC_COMMAND_TARGET(TARGET) (((TARGET) == FMC_Command_Target_bank1) || \
                                       ((TARGET) == FMC_Command_Target_bank2) || \
                                       ((TARGET) == FMC_Command_Target_bank1_2))
/**
  * @brief 多Bank操作场景：
  * - 初始化时通常选择bank1_2同时配置
  * - 刷新操作需作用于所有Bank
  */
/**
  * @brief FMC（Flexible Memory Controller）模块配置参数及状态定义
  * @功能说明：
  * 1. 定义自动刷新周期数有效性校验宏
  * 2. 定义模式寄存器内容有效性校验宏 
  * 3. 定义SDRAM控制器工作状态常量及校验宏
  * @应用场景：
  * - 配置SDRAM刷新参数
  * - 设置模式寄存器值
  * - 检测控制器工作状态
  */

/** @defgroup FMC_AutoRefresh_Number
  * @{
  */  
/* 自动刷新周期数校验宏
   @参数 NUMBER: 自动刷新周期数值（范围1-16）
   @说明: STM32F4的FMC硬件限制最大支持16个自动刷新周期 */
#define IS_FMC_AUTOREFRESH_NUMBER(NUMBER) (((NUMBER) > 0) && ((NUMBER) <= 16))

/**
  * @}
  */

/** @defgroup FMC_ModeRegister_Definition
  * @{
  */
/* 模式寄存器内容校验宏
   @参数 CONTENT: 13位模式寄存器值（范围0-8191）
   @说明: 8191对应二进制13个1，与SDRAM模式寄存器13位宽度对应 */
#define IS_FMC_MODE_REGISTER(CONTENT) ((CONTENT) <= 8191)

/**
  * @}
  */
  
/** @defgroup FMC_Mode_Status 
  * @{
  */
/* 正常工作模式标志（模式位全0） */
#define FMC_NormalMode_Status                     ((uint32_t)0x00000000)
/* 自刷新模式标志（对应FMC_SDSR寄存器的MODES1[0]位） */
#define FMC_SelfRefreshMode_Status                FMC_SDSR_MODES1_0  
/* 低功耗模式标志（对应FMC_SDSR寄存器的MODES1[1]位） */
#define FMC_PowerDownMode_Status                  FMC_SDSR_MODES1_1

/* 工作状态校验宏
   @参数 STATUS: 要检测的状态标志
   @返回值: true-有效状态 / false-无效状态
   @注意: 必须严格匹配三个预定义状态值之一 */
#define IS_FMC_MODE_STATUS(STATUS) (((STATUS) == FMC_NormalMode_Status)       || \
                                    ((STATUS) == FMC_SelfRefreshMode_Status)  || \
                                    ((STATUS) == FMC_PowerDownMode_Status))


/**
  * @}
  */      
/**
  * @brief FMC中断控制相关定义
  * @功能说明：
  * 1. 定义FMC支持的中断触发类型
  * 2. 提供中断参数有效性校验宏
  * 3. 定义中断所属存储块校验
  * @应用场景：
  * - 配置FMC中断触发方式
  * - 清除/获取中断标志
  * - 校验中断参数合法性
  */

/** @defgroup FMC_Interrupt_sources 
  * @{
  */
/* 上升沿触发中断（对应FMC_CIR寄存器的第3位） */
#define FMC_IT_RisingEdge                       ((uint32_t)0x00000008)
/* 高电平触发中断（对应FMC_CIR寄存器的第4位） */  
#define FMC_IT_Level                            ((uint32_t)0x00000010)
/* 下降沿触发中断（对应FMC_CIR寄存器的第5位） */
#define FMC_IT_FallingEdge                      ((uint32_t)0x00000020)
/* SDRAM自动刷新中断（对应FMC_SDSR寄存器的第14位） */
#define FMC_IT_Refresh                          ((uint32_t)0x00004000)

/* 中断标志校验宏
   @参数 IT: 中断标志组合值
   @说明: 
   - 0xFFFFBFC7掩码保留寄存器位（bit1-2,6-7,9-13,15-31）
   - 组合值不能为0 */
#define IS_FMC_IT(IT) ((((IT) & (uint32_t)0xFFFFBFC7) == 0x00000000) && ((IT) != 0x00000000))

/* 中断类型获取校验宏
   @参数 IT: 要获取的中断类型
   @返回值: true-有效中断类型 / false-无效类型
   @注意: 必须匹配四种预定义中断类型之一 */
#define IS_FMC_GET_IT(IT) (((IT) == FMC_IT_RisingEdge)  || \
                           ((IT) == FMC_IT_Level)       || \
                           ((IT) == FMC_IT_FallingEdge) || \
                           ((IT) == FMC_IT_Refresh)) 
                           
/* 中断所属存储块校验宏
   @参数 BANK: 存储块编号
   @说明: 支持NAND Flash、PCCARD和SDRAM存储块
   - FMC_Bank1_SDRAM: SDRAM存储块1
   - FMC_Bank2_SDRAM: SDRAM存储块2 */
#define IS_FMC_IT_BANK(BANK) (((BANK) == FMC_Bank2_NAND)   || \
                              ((BANK) == FMC_Bank3_NAND)   || \
                              ((BANK) == FMC_Bank4_PCCARD) || \
                              ((BANK) == FMC_Bank1_SDRAM)  || \
                              ((BANK) == FMC_Bank2_SDRAM)) 
                          
/**
  * @}
  */
/**
  * @brief FMC状态标志相关定义
  * @功能说明：
  * 1. 定义FMC状态寄存器标志位
  * 2. 提供标志位有效性校验宏
  * 3. 定义标志位所属存储块校验
  * @应用场景：
  * - 检测控制器工作状态
  * - 清除状态标志
  * - 多存储块联合操作
  */

/** @defgroup FMC_Flags 
  * @{
  */
/* 上升沿触发标志（对应FMC_SR寄存器的第0位） */
#define FMC_FLAG_RisingEdge                     ((uint32_t)0x00000001)
/* 高电平状态标志（对应FMC_SR寄存器的第1位） */
#define FMC_FLAG_Level                          ((uint32_t)0x00000002)
/* 下降沿触发标志（对应FMC_SR寄存器的第2位） */
#define FMC_FLAG_FallingEdge                    ((uint32_t)0x00000004)
/* FIFO空标志（对应FMC_SR寄存器的第6位，FEMPT=FIFO Empty） */
#define FMC_FLAG_FEMPT                          ((uint32_t)0x00000040)
/* 自动刷新就绪标志（直接映射FMC_SDSR寄存器的RE位） */
#define FMC_FLAG_Refresh                        FMC_SDSR_RE
/* 控制器忙状态标志（直接映射FMC_SDSR寄存器的BUSY位） */
#define FMC_FLAG_Busy                           FMC_SDSR_BUSY

/* 标志位获取校验宏
   @参数 FLAG: 要检测的状态标志
   @返回值: true-有效标志 / false-无效标志
   @注意: 包含所有可检测的硬件状态标志 */
#define IS_FMC_GET_FLAG(FLAG) (((FLAG) == FMC_FLAG_RisingEdge)       || \
                               ((FLAG) == FMC_FLAG_Level)            || \
                               ((FLAG) == FMC_FLAG_FallingEdge)      || \
                               ((FLAG) == FMC_FLAG_FEMPT)            || \
                               ((FLAG) == FMC_FLAG_Refresh)          || \
                               ((FLAG) == FMC_SDSR_BUSY))

/* 标志位存储块校验宏
   @参数 BANK: 存储块编号或组合
   @特殊说明: 
   - 允许单个存储块检测（FMC_BankX_XXX）
   - 支持双SDRAM存储块联合检测（位或组合）
   - 位组合方式：FMC_Bank1_SDRAM | FMC_Bank2_SDRAM */
#define IS_FMC_GETFLAG_BANK(BANK) (((BANK) == FMC_Bank2_NAND)    || \
                                   ((BANK) == FMC_Bank3_NAND)    || \
                                   ((BANK) == FMC_Bank4_PCCARD)  || \
                                   ((BANK) == FMC_Bank1_SDRAM)   || \
                                   ((BANK) == FMC_Bank2_SDRAM)   || \
                                   ((BANK) == (FMC_Bank1_SDRAM | FMC_Bank2_SDRAM)))

/**
  * @brief FMC标志清除及刷新参数校验定义
  * @功能说明：
  * 1. 定义标志清除操作的合法性校验
  * 2. 设置SDRAM刷新计数器有效范围校验
  * @应用场景：
  * - 清除中断/状态标志时参数校验
  * - 配置SDRAM自动刷新参数时范围校验
  */

/* 标志清除操作校验宏
   @参数 FLAG: 要清除的标志组合值
   @校验规则：
   - 0xFFFFFFF8掩码保留高位（bit3-31），允许操作bit0-2和bit6（对应Rising/Level/FallingEdge和FEMPT）
   - 组合值不能为0（至少包含一个有效标志）
   @注意：刷新标志(FMC_FLAG_Refresh)和忙标志(FMC_FLAG_Busy)不可通过此操作清除 */
#define IS_FMC_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFF8) == 0x00000000) && ((FLAG) != 0x00000000))

/** @defgroup FMC_Refresh_count
  * @{
  */
/* 自动刷新计数器校验宏
   @参数 COUNT: 刷新计数值（范围0-8191）
   @硬件限制：对应FMC_SDRTR寄存器的13位COUNT字段（2^13 -1 = 8191）
   @应用：设置SDRAM自动刷新间隔周期 */
#define IS_FMC_REFRESH_COUNT(COUNT) ((COUNT) <= 8191)


/**
  * @}
  */

/**
  * @}
  */

/**
  * @brief FMC控制器驱动函数接口声明
  * @模块说明：
  * 包含NOR/SRAM、NAND、PCCARD三种存储控制器的操作接口
  * @功能分类：
  * - 控制器初始化/反初始化
  * - 配置结构体操作
  * - 设备使能控制
  * - 特殊功能操作（ECC校验等）
  */

/* NOR/SRAM控制器函数组 ****************************************************/
/* 复位指定NOR/SRAM存储块配置到默认值 */
void FMC_NORSRAMDeInit(uint32_t FMC_Bank);
/* 根据结构体参数初始化NOR/SRAM控制器 */
void FMC_NORSRAMInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct);
/* 初始化NOR/SRAM配置结构体为默认值 */
void FMC_NORSRAMStructInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct);
/* 使能/禁用指定NOR/SRAM存储块 */
void FMC_NORSRAMCmd(uint32_t FMC_Bank, FunctionalState NewState);

/* NAND控制器函数组 ********************************************************/
/* 复位指定NAND存储块配置 */
void     FMC_NANDDeInit(uint32_t FMC_Bank);
/* 根据结构体参数初始化NAND控制器 */
void     FMC_NANDInit(FMC_NANDInitTypeDef* FMC_NANDInitStruct);
/* 初始化NAND配置结构体为默认值 */  
void     FMC_NANDStructInit(FMC_NANDInitTypeDef* FMC_NANDInitStruct);
/* 使能/禁用指定NAND存储块 */
void     FMC_NANDCmd(uint32_t FMC_Bank, FunctionalState NewState);
/* 启用/关闭NAND ECC校验功能 */
void     FMC_NANDECCCmd(uint32_t FMC_Bank, FunctionalState NewState);
/* 获取指定NAND存储块的ECC校验值 */
uint32_t FMC_GetECC(uint32_t FMC_Bank);

/* PCCARD控制器函数组 ******************************************************/
/* 复位PCCARD控制器配置 */
void FMC_PCCARDDeInit(void);
/* 根据结构体参数初始化PCCARD控制器 */
void FMC_PCCARDInit(FMC_PCCARDInitTypeDef* FMC_PCCARDInitStruct);
/* 初始化PCCARD配置结构体为默认值 */
void FMC_PCCARDStructInit(FMC_PCCARDInitTypeDef* FMC_PCCARDInitStruct);
/* 使能/禁用PCCARD控制器 */
void FMC_PCCARDCmd(FunctionalState NewState);

/**
  * @brief SDRAM控制器及中断标志管理函数接口声明
  * @模块说明：
  * 包含SDRAM存储控制器操作接口和全模块中断/标志管理接口
  * @核心功能：
  * - SDRAM初始化配置及工作模式控制
  * - 刷新参数动态设置
  * - 中断系统全局管理
  */

/* SDRAM控制器函数组 *******************************************************/
/* 复位指定SDRAM存储块配置（Bank1/2） */
void     FMC_SDRAMDeInit(uint32_t FMC_Bank);
/* 根据结构体参数初始化SDRAM控制器 */
void     FMC_SDRAMInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct);
/* 初始化SDRAM配置结构体为默认值 */
void     FMC_SDRAMStructInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct);
/* 配置SDRAM命令（模式寄存器设置、自动刷新等） */
void     FMC_SDRAMCmdConfig(FMC_SDRAMCommandTypeDef* FMC_SDRAMCommandStruct);
/* 获取指定SDRAM存储块的工作模式状态 */
uint32_t FMC_GetModeStatus(uint32_t SDRAM_Bank);
/* 设置SDRAM刷新计数器值（需满足IS_FMC_REFRESH_COUNT限制） */ 
void     FMC_SetRefreshCount(uint32_t FMC_Count);
/* 设置自动刷新周期数（需满足IS_FMC_AUTOREFRESH_NUMBER限制） */
void     FMC_SetAutoRefresh_Number(uint32_t FMC_Number);
/* 配置指定SDRAM存储块的写保护状态 */
void     FMC_SDRAMWriteProtectionConfig(uint32_t SDRAM_Bank, FunctionalState NewState);

/* 中断与标志管理函数组 ****************************************************/
/* 配置指定存储块的中断源（FMC_IT_XXX组合） */
void       FMC_ITConfig(uint32_t FMC_Bank, uint32_t FMC_IT, FunctionalState NewState);
/* 获取指定存储块的状态标志（FMC_FLAG_XXX） */
FlagStatus FMC_GetFlagStatus(uint32_t FMC_Bank, uint32_t FMC_FLAG);
/* 清除指定存储块的状态标志（仅可清除标志见IS_FMC_CLEAR_FLAG） */
void       FMC_ClearFlag(uint32_t FMC_Bank, uint32_t FMC_FLAG);
/* 获取指定存储块的中断状态（需先使能对应中断） */
ITStatus   FMC_GetITStatus(uint32_t FMC_Bank, uint32_t FMC_IT);
/* 清除指定存储块的中断挂起标志 */
void       FMC_ClearITPendingBit(uint32_t FMC_Bank, uint32_t FMC_IT);

#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_FMC_H */
/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

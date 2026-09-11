/**
  ******************************************************************************
  * @file    stm32f4xx_fsmc.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the FSMC firmware 
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
#ifndef __STM32F4xx_FSMC_H
#define __STM32F4xx_FSMC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup FSMC
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/**
  * @brief FSMC NOR/SRAM存储器时序及初始化配置结构体
  * @功能说明：
  * 1. 定义NOR/SRAM存储器的时序参数配置
  * 2. 封装完整的NOR/SRAM控制器配置参数
  * @核心参数：
  * - 地址/数据建立时间
  * - 总线周转周期
  * - 存储器类型及数据宽度
  * - 突发访问模式配置
  * @应用场景：
  * - 配置外部SRAM接口时序
  * - 初始化NOR Flash存储器访问参数
  * - 设置总线复用模式及等待信号策略
  */

/** 
  * @brief NOR/SRAM时序参数结构体
  * @详细说明：配置存储器访问时序相关参数，所有时间单位均为HCLK周期数
  */
typedef struct
{
  /* 地址建立时间配置（0-15个HCLK周期）
     @应用：异步NOR/SRAM的地址有效到片选有效的时间间隔
     @注意：同步NOR Flash不使用此参数 */
  uint32_t FSMC_AddressSetupTime;       

  /* 地址保持时间配置（0-15个HCLK周期）
     @应用：片选无效后地址保持的时间
     @注意：同步NOR Flash不使用此参数 */  
  uint32_t FSMC_AddressHoldTime;        

  /* 数据建立时间配置（0-255个HCLK周期）
     @应用：写操作时数据有效持续时间，读操作时数据采样窗口
     @范围：SRAM建议值>=2，NOR Flash建议值>=5 */
  uint32_t FSMC_DataSetupTime;          

  /* 总线周转时间配置（0-15个HCLK周期）
     @应用：读写操作切换时的总线空闲周期
     @注意：仅用于复用总线NOR Flash */  
  uint32_t FSMC_BusTurnAroundDuration;  

  /* CLK时钟分频系数（1-15分频）
     @应用：同步模式下的时钟输出频率 = HCLK/(2*FSMC_CLKDivision)
     @注意：异步模式不使用此参数 */
  uint32_t FSMC_CLKDivision;            

  /* 数据延迟周期（0-15个CLK周期）
     @应用：同步突发读操作的预取延迟
     @特殊：CRAM必须设为0，异步模式不生效 */
  uint32_t FSMC_DataLatency;            

  /* 存储器访问模式选择
     @可选：FSMC_AccessMode_A/B/C/D
     @影响：控制地址/数据信号的锁存时序 */
  uint32_t FSMC_AccessMode;             
}FSMC_NORSRAMTimingInitTypeDef;

/** 
  * @brief NOR/SRAM初始化配置结构体
  * @详细说明：封装存储器控制器的完整配置参数
  */
typedef struct
{
  /* 存储块选择（Bank1-4）
     @示例：FSMC_Bank1_NORSRAM1 */  
  uint32_t FSMC_Bank;                

  /* 地址数据复用使能
     @使能：FSMC_DataAddressMux_Enable
     @应用：减少引脚占用，用于数据/地址线复用的存储器 */
  uint32_t FSMC_DataAddressMux;      

  /* 存储器类型选择
     @可选：NOR/SRAM/PSRAM/ROM
     @影响：控制器内部时序处理逻辑 */  
  uint32_t FSMC_MemoryType;          

  /* 存储器数据总线宽度
     @可选：8/16位
     @注意：需与硬件连接匹配 */  
  uint32_t FSMC_MemoryDataWidth;     

  /* 突发访问模式使能
     @使能：FSMC_BurstAccessMode_Enable
     @应用：提高同步存储器的访问效率 */  
  uint32_t FSMC_BurstAccessMode;     

  /* 异步等待信号使能
     @使能：FSMC_AsynchronousWait_Enable 
     @应用：延长NOR Flash访问周期 */  
  uint32_t FSMC_AsynchronousWait;     

  /* 等待信号极性配置
     @可选：高有效/低有效
     @同步：需与存储器规格书一致 */  
  uint32_t FSMC_WaitSignalPolarity;  

  /* 回环突发模式使能
     @使能：FSMC_WrapMode_Enable
     @应用：优化顺序数据访问效率 */  
  uint32_t FSMC_WrapMode;            

  /* 等待信号激活时序
     @模式：FSMC_WaitSignalActive_BeforeWaitState（提前1周期）
           FSMC_WaitSignalActive_DuringWaitState（同步激活） */  
  uint32_t FSMC_WaitSignalActive;    

  /* 写操作使能
     @禁用：FSMC_WriteOperation_Disable 可设为只读模式 */  
  uint32_t FSMC_WriteOperation;      

  /* 等待信号插入控制
     @使能：在突发传输中插入等待周期 */  
  uint32_t FSMC_WaitSignal;          

  /* 扩展模式使能
     @使能：FSMC_ExtendedMode_Enable 可独立配置读写时序 */  
  uint32_t FSMC_ExtendedMode;        

  /* 写突发使能
     @使能：FSMC_WriteBurst_Enable 提高连续写效率 */  
  uint32_t FSMC_WriteBurst;          

  /* 基础模式时序配置（扩展模式禁用时生效）
     @包含：读写共用时序参数 */  
  FSMC_NORSRAMTimingInitTypeDef* FSMC_ReadWriteTimingStruct; 

  /* 写操作独立时序配置（扩展模式使能时生效）
     @注意：启用扩展模式后，读时序使用FSMC_ReadWriteTimingStruct
           写时序使用本结构体 */  
  FSMC_NORSRAMTimingInitTypeDef* FSMC_WriteTimingStruct;     
}FSMC_NORSRAMInitTypeDef;
/**
  * @brief FSMC NAND/PCCARD控制器时序及初始化配置结构体
  * @功能说明：
  * 1. 定义NAND/PCCARD存储器的时序参数配置
  * 2. 封装完整的NAND/PCCARD控制器配置参数
  * @核心参数：
  * - 命令/地址建立时间
  * - ECC校验配置
  * - 多空间时序分离配置
  * @应用场景：
  * - 配置NAND Flash接口时序
  * - 初始化PCCARD存储卡控制参数
  * - 设置多存储空间独立时序
  */

/** 
  * @brief NAND/PCCARD时序参数结构体
  * @详细说明：配置存储器访问时序相关参数，时间单位均为HCLK周期数
  */
typedef struct
{
  /* 地址建立时间（0-255周期）
     @应用：命令有效前的地址稳定时间
     @场景：适用于Common/Attribute/I/O空间的读写操作 */
  uint32_t FSMC_SetupTime;      

  /* 等待信号建立时间（0-255周期）
     @作用：命令信号保持最小有效时间
     @注意：影响存储器操作可靠性 */  
  uint32_t FSMC_WaitSetupTime;  

  /* 数据保持时间（0-255周期）
     @功能：命令失效后地址/数据的保持时间
     @写操作：同时保持数据总线稳定 */  
  uint32_t FSMC_HoldSetupTime;  

  /* 高阻态建立时间（0-255周期）
     @用途：写操作后数据总线高阻态维持时间
     @防止：总线竞争冲突 */  
  uint32_t FSMC_HiZSetupTime;   
}FSMC_NAND_PCCARDTimingInitTypeDef;

/** 
  * @brief NAND初始化配置结构体
  * @详细说明：配置NAND Flash控制器的完整参数
  */
typedef struct
{
  /* 存储块选择（Bank2/3）
     @示例：FSMC_Bank2_NAND */  
  uint32_t FSMC_Bank;              

  /* 等待功能使能
     @使能：FSMC_Waitfeature_Enable
     @作用：插入等待周期适配低速存储器 */  
  uint32_t FSMC_Waitfeature;      

  /* 存储器数据总线宽度
     @可选：8/16位
     @注意：需与NAND芯片规格严格一致 */  
  uint32_t FSMC_MemoryDataWidth;   

  /* ECC校验使能
     @使能：FSMC_ECC_Enable
     @注意：启用后自动计算校验码 */  
  uint32_t FSMC_ECC;              

  /* ECC校验页大小
     @可选：256/512/1024/2048字节
     @影响：每页生成的ECC字节数 */  
  uint32_t FSMC_ECCPageSize;     

  /* CLE到RE延迟时间（0-255周期）
     @定义：命令锁存使能(CLE)后到读使能(RE)的间隔
     @关键：影响命令有效时间 */  
  uint32_t FSMC_TCLRSetupTime;   

  /* ALE到RE延迟时间（0-255周期）
     @定义：地址锁存使能(ALE)后到读使能(RE)的间隔
     @时序：控制地址建立时机 */  
  uint32_t FSMC_TARSetupTime;    

  /* Common空间时序配置
     @内容：存储命令操作时序参数 */  
  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_CommonSpaceTimingStruct;   

  /* Attribute空间时序配置
     @应用：扩展属性操作（如K9F系列附加命令） */  
  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_AttributeSpaceTimingStruct; 
}FSMC_NANDInitTypeDef;

/** 
  * @brief PCCARD初始化配置结构体
  * @详细说明：配置PCCARD存储卡的完整参数
  */
typedef struct
{
  /* 等待功能使能
     @使能：适配低速PCCARD设备 */  
  uint32_t FSMC_Waitfeature;    

  /* CLE到RE延迟时间（同NAND时序） */  
  uint32_t FSMC_TCLRSetupTime;  

  /* ALE到RE延迟时间（同NAND时序） */  
  uint32_t FSMC_TARSetupTime;   

  /* Common空间时序配置
     @应用：普通读写操作时序 */  
  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_CommonSpaceTimingStruct; 

  /* Attribute空间时序配置
     @应用：属性信息访问时序 */  
  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_AttributeSpaceTimingStruct;  

  /* I/O空间时序配置
     @专用：PCCARD的I/O模式访问时序
     @注意：与其他空间时序独立配置 */  
  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_IOSpaceTimingStruct;  
}FSMC_PCCARDInitTypeDef;

/**
  * @brief FSMC存储块定义及验证宏
  * @功能说明：
  * 1. 定义各类型存储器的存储块标识
  * 2. 提供存储块参数有效性校验宏
  * @核心功能：
  * - 标识NOR/SRAM/NAND/PCCARD的物理存储块
  * - 验证函数参数的存储块合法性
  * @应用场景：
  * - 初始化时选择目标存储块
  * - 中断/标志操作时的存储块校验
  */

/*-------------------------- 存储块定义 -----------------------------*/
/** @defgroup FSMC_NORSRAM_Bank NOR/SRAM存储块 */
#define FSMC_Bank1_NORSRAM1  ((uint32_t)0x00000000) /* Bank1-SRAM/NOR块1 基址0x60000000 */
#define FSMC_Bank1_NORSRAM2  ((uint32_t)0x00000002) /* Bank1-SRAM/NOR块2 基址0x64000000 */
#define FSMC_Bank1_NORSRAM3  ((uint32_t)0x00000004) /* Bank1-SRAM/NOR块3 基址0x68000000 */ 
#define FSMC_Bank1_NORSRAM4  ((uint32_t)0x00000006) /* Bank1-SRAM/NOR块4 基址0x6C000000 */

/** @defgroup FSMC_NAND_Bank NAND存储块 */
#define FSMC_Bank2_NAND      ((uint32_t)0x00000010) /* Bank2-NAND块 基址0x70000000 */
#define FSMC_Bank3_NAND      ((uint32_t)0x00000100) /* Bank3-NAND块 基址0x80000000 */

/** @defgroup FSMC_PCCARD_Bank PCCARD存储块 */  
#define FSMC_Bank4_PCCARD    ((uint32_t)0x00001000) /* Bank4-PCCARD块 基址0x90000000 */

/*-------------------------- 校验宏定义 -----------------------------*/
/* NOR/SRAM存储块合法性校验（Bank1的1-4号块） */
#define IS_FSMC_NORSRAM_BANK(BANK) (((BANK) == FSMC_Bank1_NORSRAM1) || \
                                    ((BANK) == FSMC_Bank1_NORSRAM2) || \
                                    ((BANK) == FSMC_Bank1_NORSRAM3) || \
                                    ((BANK) == FSMC_Bank1_NORSRAM4))

/* NAND存储块合法性校验（Bank2/3） */                                    
#define IS_FSMC_NAND_BANK(BANK) (((BANK) == FSMC_Bank2_NAND) || \
                                 ((BANK) == FSMC_Bank3_NAND))

/* 支持获取标志位的存储块校验（NAND/PCCARD） */
#define IS_FSMC_GETFLAG_BANK(BANK) (((BANK) == FSMC_Bank2_NAND) || \
                                    ((BANK) == FSMC_Bank3_NAND) || \
                                    ((BANK) == FSMC_Bank4_PCCARD))

/* 支持中断操作的存储块校验（NAND/PCCARD） */                                    
#define IS_FSMC_IT_BANK(BANK) (((BANK) == FSMC_Bank2_NAND) || \
                               ((BANK) == FSMC_Bank3_NAND) || \
                               ((BANK) == FSMC_Bank4_PCCARD))


/**
  * @brief FSMC NOR/SRAM控制器配置参数定义
  * @功能说明：
  * 1. 定义存储器总线复用模式
  * 2. 指定连接的存储器类型
  * 3. 配置数据总线宽度及访问模式
  * @核心配置：
  * - 地址/数据总线复用选择
  * - SRAM/PSRAM/NOR存储器类型指定
  * - 8/16位数据总线配置
  * - 突发传输模式控制
  * - 异步等待信号管理
  */

/** @defgroup FSMC_Data_Address_Bus_Multiplexing 地址数据总线复用模式 */
#define FSMC_DataAddressMux_Disable  ((uint32_t)0x00000000) /* 禁用总线复用（独立地址/数据线） */
#define FSMC_DataAddressMux_Enable   ((uint32_t)0x00000002) /* 启用总线复用（共用地址/数据线） */
/* 总线复用模式校验（需匹配硬件设计） */
#define IS_FSMC_MUX(MUX) (((MUX) == FSMC_DataAddressMux_Disable) || \
                          ((MUX) == FSMC_DataAddressMux_Enable))

/** @defgroup FSMC_Memory_Type 存储器类型定义 */
#define FSMC_MemoryType_SRAM  ((uint32_t)0x00000000)  /* 标准SRAM存储器 */
#define FSMC_MemoryType_PSRAM ((uint32_t)0x00000004)  /* 伪静态PSRAM存储器 */ 
#define FSMC_MemoryType_NOR   ((uint32_t)0x00000008)  /* NOR Flash存储器 */
/* 存储器类型合法性校验 */
#define IS_FSMC_MEMORY(MEMORY) (((MEMORY) == FSMC_MemoryType_SRAM) || \
                                ((MEMORY) == FSMC_MemoryType_PSRAM)|| \
                                ((MEMORY) == FSMC_MemoryType_NOR))

/** @defgroup FSMC_Data_Width 数据总线宽度配置 */
#define FSMC_MemoryDataWidth_8b  ((uint32_t)0x00000000) /* 8位总线（需连接D0-D7） */
#define FSMC_MemoryDataWidth_16b ((uint32_t)0x00000010) /* 16位总线（需连接D0-D15） */
/* 总线宽度校验（需与实际硬件连接一致） */
#define IS_FSMC_MEMORY_WIDTH(WIDTH) (((WIDTH) == FSMC_MemoryDataWidth_8b) || \
                                     ((WIDTH) == FSMC_MemoryDataWidth_16b))

/** @defgroup FSMC_Burst_Access_Mode 突发访问模式 */
#define FSMC_BurstAccessMode_Disable ((uint32_t)0x00000000) /* 禁用突发传输 */
#define FSMC_BurstAccessMode_Enable  ((uint32_t)0x00000100) /* 启用突发传输（连续地址优化） */
/* 突发模式状态校验 */
#define IS_FSMC_BURSTMODE(STATE) (((STATE) == FSMC_BurstAccessMode_Disable) || \
                                  ((STATE) == FSMC_BurstAccessMode_Enable))

/** @defgroup FSMC_AsynchronousWait 异步等待功能 */
#define FSMC_AsynchronousWait_Disable ((uint32_t)0x00000000) /* 禁用等待信号插入 */
#define FSMC_AsynchronousWait_Enable  ((uint32_t)0x00008000) /* 启用等待信号（适配低速设备） */
/* 等待功能状态校验 */
#define IS_FSMC_ASYNWAIT(STATE) (((STATE) == FSMC_AsynchronousWait_Disable) || \
                                 ((STATE) == FSMC_AsynchronousWait_Enable))
/**
  * @brief FSMC高级控制及工作模式配置参数
  * @功能说明：
  * 1. 定义等待信号特性及突发传输模式
  * 2. 控制写操作使能与扩展模式
  * 3. 配置复杂访问场景下的时序策略
  * @核心配置：
  * - 等待信号极性及时序相位
  * - 突发传输地址回环模式
  * - 读写操作独立控制
  * - 扩展时序模式使能
  */

/** @defgroup FSMC_Wait_Signal_Polarity 等待信号极性 */
#define FSMC_WaitSignalPolarity_Low  ((uint32_t)0x00000000) /* 低电平有效（NWAIT=0时插入等待） */
#define FSMC_WaitSignalPolarity_High ((uint32_t)0x00000200) /* 高电平有效（NWAIT=1时插入等待） */
#define IS_FSMC_WAIT_POLARITY(POLARITY) (((POLARITY) == FSMC_WaitSignalPolarity_Low) || \
                                         ((POLARITY) == FSMC_WaitSignalPolarity_High))

/** @defgroup FSMC_Wrap_Mode 突发地址回环模式 */
#define FSMC_WrapMode_Disable  ((uint32_t)0x00000000) /* 线性突发（地址连续递增） */
#define FSMC_WrapMode_Enable   ((uint32_t)0x00000400) /* 回环突发（地址循环递增） */
#define IS_FSMC_WRAP_MODE(MODE) (((MODE) == FSMC_WrapMode_Disable) || \
                                 ((MODE) == FSMC_WrapMode_Enable))

/** @defgroup FSMC_Wait_Timing 等待信号激活时机 */
#define FSMC_WaitSignalActive_BeforeWaitState ((uint32_t)0x00000000) /* 等待状态前生效 */
#define FSMC_WaitSignalActive_DuringWaitState ((uint32_t)0x00000800) /* 等待状态期间生效 */ 
#define IS_FSMC_WAIT_SIGNAL_ACTIVE(ACTIVE) (((ACTIVE) == FSMC_WaitSignalActive_BeforeWaitState) || \
                                            ((ACTIVE) == FSMC_WaitSignalActive_DuringWaitState))

/** @defgroup FSMC_Write_Operation 写操作控制 */
#define FSMC_WriteOperation_Disable ((uint32_t)0x00000000) /* 禁止写操作（只读模式） */
#define FSMC_WriteOperation_Enable  ((uint32_t)0x00001000) /* 允许写操作 */
#define IS_FSMC_WRITE_OPERATION(OPERATION) (((OPERATION) == FSMC_WriteOperation_Disable) || \
                                            ((OPERATION) == FSMC_WriteOperation_Enable))

/** @defgroup FSMC_Wait_Signal 等待信号使能 */
#define FSMC_WaitSignal_Disable ((uint32_t)0x00000000) /* 禁用NWAIT信号检测 */
#define FSMC_WaitSignal_Enable  ((uint32_t)0x00002000) /* 启用NWAIT信号检测 */
#define IS_FSMC_WAITE_SIGNAL(SIGNAL) (((SIGNAL) == FSMC_WaitSignal_Disable) || \
                                      ((SIGNAL) == FSMC_WaitSignal_Enable))

/** @defgroup FSMC_Extended_Mode 扩展模式 */
#define FSMC_ExtendedMode_Disable ((uint32_t)0x00000000) /* 统一读写时序 */
#define FSMC_ExtendedMode_Enable  ((uint32_t)0x00004000) /* 独立配置读写时序 */
#define IS_FSMC_EXTENDED_MODE(MODE) (((MODE) == FSMC_ExtendedMode_Disable) || \
                                     ((MODE) == FSMC_ExtendedMode_Enable))
/**
  * @brief FSMC时序参数验证宏及写突发模式配置
  * @功能说明：
  * 1. 定义存储器时序参数的合法性校验规则
  * 2. 控制突发写入操作模式
  * @核心功能：
  * - 确保时序参数在硬件允许范围内
  * - 突发模式提升大数据块写入效率
  * @应用场景：
  * - 初始化时验证时序结构体参数
  * - 配置高速连续写入模式
  */

/** @defgroup FSMC_Write_Burst 写突发模式 */
#define FSMC_WriteBurst_Disable  ((uint32_t)0x00000000) /* 禁用写突发（单次传输模式） */
#define FSMC_WriteBurst_Enable   ((uint32_t)0x00080000) /* 启用写突发（连续地址自动递增） */
/* 写突发模式校验（需存储器支持突发操作） */
#define IS_FSMC_WRITE_BURST(BURST) (((BURST) == FSMC_WriteBurst_Disable) || \
                                    ((BURST) == FSMC_WriteBurst_Enable))

/** @defgroup FSMC_Address_Setup_Time 地址建立时间校验 */
#define IS_FSMC_ADDRESS_SETUP_TIME(TIME) ((TIME) <= 0xF) 
/* 参数范围：0-15个HCLK周期（对应寄存器4位字段） */

/** @defgroup FSMC_Address_Hold_Time 地址保持时间校验 */
#define IS_FSMC_ADDRESS_HOLD_TIME(TIME) ((TIME) <= 0xF)  
/* 参数范围：0-15个HCLK周期（NOR/SRAM时序配置用） */

/** @defgroup FSMC_Data_Setup_Time 数据建立时间校验 */ 
#define IS_FSMC_DATASETUP_TIME(TIME) (((TIME) > 0) && ((TIME) <= 0xFF))  
/* 参数范围：1-255周期（注：同步模式需要最少2个周期） */

/** @defgroup FSMC_Bus_Turn_around_Duration 总线周转时间校验 */
#define IS_FSMC_TURNAROUND_TIME(TIME) ((TIME) <= 0xF)   
/* 参数范围：0-15周期（用于复用总线切换方向时的延迟） */

/** @defgroup FSMC_CLK_Division 时钟分频校验 */
#define IS_FSMC_CLK_DIV(DIV) ((DIV) <= 0xF)            
/* 分频系数：1-15（CLK输出频率 = HCLK/(2*(DIV+1))） */

/** @defgroup FSMC_Data_Latency 数据延迟校验 */
#define IS_FSMC_DATA_LATENCY(LATENCY) ((LATENCY) <= 0xF)
/* 延迟周期：0-15（同步模式下数据预取延迟） */
/**
  * @brief FSMC访问模式及NAND控制器功能配置
  * @功能说明：
  * 1. 定义存储器访问时序模式
  * 2. 配置NAND控制器的等待功能及ECC校验
  * 3. 设置ECC校验的存储单元粒度
  * @核心功能：
  * - 多种访问时序模式选择
  * - NAND错误校验功能控制
  * - ECC校验单元尺寸配置
  */

/** @defgroup FSMC_Access_Mode 存储器访问模式 */
#define FSMC_AccessMode_A ((uint32_t)0x00000000) /* 模式A：基础异步时序（适合大多数SRAM） */
#define FSMC_AccessMode_B ((uint32_t)0x10000000) /* 模式B：CLK延后地址锁存（适配特定NOR Flash） */
#define FSMC_AccessMode_C ((uint32_t)0x20000000) /* 模式C：读信号同步时序（用于PSRAM） */
#define FSMC_AccessMode_D ((uint32_t)0x30000000) /* 模式D：完全同步突发模式（DDR接口） */
#define IS_FSMC_ACCESS_MODE(MODE) (((MODE) == FSMC_AccessMode_A) || \
                                   ((MODE) == FSMC_AccessMode_B) || \
                                   ((MODE) == FSMC_AccessMode_C) || \
                                   ((MODE) == FSMC_AccessMode_D))

/** @defgroup FSMC_Wait_feature NAND等待功能 */
#define FSMC_Waitfeature_Disable  ((uint32_t)0x00000000) /* 禁用等待信号检测 */
#define FSMC_Waitfeature_Enable   ((uint32_t)0x00000002) /* 启用NWAIT信号监控 */
#define IS_FSMC_WAIT_FEATURE(FEATURE) (((FEATURE) == FSMC_Waitfeature_Disable) || \
                                       ((FEATURE) == FSMC_Waitfeature_Enable))

/** @defgroup FSMC_ECC ECC校验控制 */
#define FSMC_ECC_Disable  ((uint32_t)0x00000000) /* 关闭ECC计算（默认状态） */
#define FSMC_ECC_Enable   ((uint32_t)0x00000040) /* 启用ECC自动计算（需设置页大小） */
#define IS_FSMC_ECC_STATE(STATE) (((STATE) == FSMC_ECC_Disable) || \
                                  ((STATE) == FSMC_ECC_Enable))

/** @defgroup FSMC_ECC_Page_Size ECC页大小配置 */
#define FSMC_ECCPageSize_256Bytes  ((uint32_t)0x00000000)  /* 每256字节计算ECC（小页NAND） */
#define FSMC_ECCPageSize_512Bytes  ((uint32_t)0x00020000)  /* 512字节（标准页NAND） */
#define FSMC_ECCPageSize_1024Bytes ((uint32_t)0x00040000)  /* 1KB（大页NAND） */
#define FSMC_ECCPageSize_2048Bytes ((uint32_t)0x00060000)  /* 2KB（超大页NAND） */
#define FSMC_ECCPageSize_4096Bytes ((uint32_t)0x00080000)  /* 4KB（先进NAND） */
#define FSMC_ECCPageSize_8192Bytes ((uint32_t)0x000A0000)  /* 8KB（企业级存储） */
#define IS_FSMC_ECCPAGE_SIZE(SIZE) (((SIZE) == FSMC_ECCPageSize_256Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_512Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_1024Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_2048Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_4096Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_8192Bytes))
/**
  * @brief FSMC NAND/PCCARD时序校验及中断配置
  * @功能说明：
  * 1. 定义NAND/PCCARD时序参数的合法性校验规则
  * 2. 配置中断触发类型及标志校验
  * @核心功能：
  * - 精确控制NAND接口时序参数
  * - 管理存储器操作中断事件
  * @应用场景：
  * - NAND命令时序精细调整
  * - 异步操作状态监控
  */

/*------------------------ 时序参数校验宏 ------------------------*/
/** @defgroup FSMC_TCLR_Setup_Time CLE到RE延迟校验 */
#define IS_FSMC_TCLR_TIME(TIME) ((TIME) <= 0xFF)
/* 参数范围：0-255周期（CLE有效到RE有效的最短间隔） */

/** @defgroup FSMC_TAR_Setup_Time ALE到RE延迟校验 */
#define IS_FSMC_TAR_TIME(TIME) ((TIME) <= 0xFF)
/* 参数范围：0-255周期（ALE有效到RE有效的最短间隔） */

/** @defgroup FSMC_Setup_Time 命令建立时间校验 */
#define IS_FSMC_SETUP_TIME(TIME) ((TIME) <= 0xFF)
/* 参数范围：0-255周期（命令/地址有效到WE#的间隔） */

/** @defgroup FSMC_Wait_Setup_Time 等待时间校验 */
#define IS_FSMC_WAIT_TIME(TIME) ((TIME) <= 0xFF)
/* 参数范围：0-255周期（等待信号有效持续时间） */

/** @defgroup FSMC_Hold_Setup_Time 保持时间校验 */
#define IS_FSMC_HOLD_TIME(TIME) ((TIME) <= 0xFF)
/* 参数范围：0-255周期（WE#无效后信号保持时间） */

/** @defgroup FSMC_HiZ_Setup_Time 高阻态时间校验 */
#define IS_FSMC_HIZ_TIME(TIME) ((TIME) <= 0xFF)
/* 参数范围：0-255周期（数据总线高阻态维持时间） */

/*-------------------------- 中断配置 --------------------------*/
/** @defgroup FSMC_Interrupt_sources 中断源定义 */
#define FSMC_IT_RisingEdge  ((uint32_t)0x00000008) /* 上升沿触发（CLE/ALE边沿检测） */
#define FSMC_IT_Level       ((uint32_t)0x00000010) /* 电平触发（NWAIT状态监测） */
#define FSMC_IT_FallingEdge ((uint32_t)0x00000020) /* 下降沿触发（写操作完成检测） */

/* 中断标志合法性校验（屏蔽保留位后必须非零） */
#define IS_FSMC_IT(IT) ((((IT) & 0xFFFFFFC7) == 0) && ((IT) != 0))

/* 有效中断类型校验（仅支持三种边沿类型） */ 
#define IS_FSMC_GET_IT(IT) (((IT) == FSMC_IT_RisingEdge) || \
                            ((IT) == FSMC_IT_Level) || \
                            ((IT) == FSMC_IT_FallingEdge))

/**
  * @}
  *//**
  * @brief FSMC状态标志定义及操作校验
  * @功能说明：
  * 1. 定义NAND/PCCARD控制器的状态标志位
  * 2. 提供标志位合法性校验及清除规则
  * @核心功能：
  * - 检测存储器操作状态事件
  * - 确保标志操作符合硬件规范
  * @应用场景：
  * - 轮询模式下的状态检测
  * - 中断服务程序中的标志清除
  */

/** @defgroup FSMC_Flags 状态标志位定义 */
#define FSMC_FLAG_RisingEdge  ((uint32_t)0x00000001) /* 上升沿事件标志（CLE/ALE边沿触发） */
#define FSMC_FLAG_Level       ((uint32_t)0x00000002) /* 电平状态标志（NWAIT持续有效） */
#define FSMC_FLAG_FallingEdge ((uint32_t)0x00000004) /* 下降沿事件标志（WE#/RE#边沿触发） */
#define FSMC_FLAG_FEMPT       ((uint32_t)0x00000040) /* FIFO空标志（数据缓冲区空） */

/* 标志位获取校验（支持单标志查询） */
#define IS_FSMC_GET_FLAG(FLAG) (((FLAG) == FSMC_FLAG_RisingEdge) || \
                                ((FLAG) == FSMC_FLAG_Level) || \
                                ((FLAG) == FSMC_FLAG_FallingEdge) || \
                                ((FLAG) == FSMC_FLAG_FEMPT))

/* 标志位清除校验（允许清除边沿类标志） */
#define IS_FSMC_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFF8) == 0x00000000) && ((FLAG) != 0x00000000))
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 
/**
  * @brief FSMC控制器驱动函数接口声明
  * @模块说明：
  * 包含NOR/SRAM、NAND、PCCARD三种存储控制器的完整操作接口
  * @功能分类：
  * - 控制器初始化/反初始化
  * - 配置结构体操作
  * - 设备使能控制
  * - 中断与状态管理
  */

/*------------------------ NOR/SRAM控制器函数组 ------------------------*/
/* 复位指定NOR/SRAM存储块配置（Bank1的1-4号块） */
void FSMC_NORSRAMDeInit(uint32_t FSMC_Bank);

/* 根据结构体参数初始化NOR/SRAM控制器（需先调用StructInit） */
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct);

/* 初始化NOR/SRAM配置结构体为默认值（防野指针） */
void FSMC_NORSRAMStructInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct);

/* 使能/禁用指定NOR/SRAM存储块（ENABLE-上电 DISABLE-断电） */
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState);

/*------------------------ NAND控制器函数组 --------------------------*/
/* 复位指定NAND存储块配置（Bank2/3） */
void FSMC_NANDDeInit(uint32_t FSMC_Bank);

/* 根据结构体参数初始化NAND控制器 */
void FSMC_NANDInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct);

/* 初始化NAND配置结构体为默认值 */
void FSMC_NANDStructInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct);

/* 使能/禁用NAND存储块 */
void FSMC_NANDCmd(uint32_t FSMC_Bank, FunctionalState NewState);

/* 启用/关闭NAND ECC校验功能（需先配置ECCPageSize） */
void FSMC_NANDECCCmd(uint32_t FSMC_Bank, FunctionalState NewState);

/* 获取NAND存储块的ECC校验值（需在读取操作后调用） */
uint32_t FSMC_GetECC(uint32_t FSMC_Bank);

/*----------------------- PCCARD控制器函数组 --------------------------*/
/* 复位PCCARD控制器配置（Bank4专用） */
void FSMC_PCCARDDeInit(void);

/* 根据结构体参数初始化PCCARD控制器 */
void FSMC_PCCARDInit(FSMC_PCCARDInitTypeDef* FSMC_PCCARDInitStruct);

/* 初始化PCCARD配置结构体为默认值 */
void FSMC_PCCARDStructInit(FSMC_PCCARDInitTypeDef* FSMC_PCCARDInitStruct);

/* 使能/禁用PCCARD控制器 */
void FSMC_PCCARDCmd(FunctionalState NewState);

/*--------------------- 中断与标志管理函数组 -------------------------*/
/* 配置存储块中断源（FSMC_IT_XXX组合，NewState-ENABLE/DISABLE） */
void FSMC_ITConfig(uint32_t FSMC_Bank, uint32_t FSMC_IT, FunctionalState NewState);

/* 获取指定存储块的状态标志（FLAG参数见FSMC_FLAG_XXX） */
FlagStatus FSMC_GetFlagStatus(uint32_t FSMC_Bank, uint32_t FSMC_FLAG);

/* 清除存储块的状态标志（仅可清除边沿类标志） */
void FSMC_ClearFlag(uint32_t FSMC_Bank, uint32_t FSMC_FLAG);

/* 获取存储块的中断状态（需先使能对应中断） */
ITStatus FSMC_GetITStatus(uint32_t FSMC_Bank, uint32_t FSMC_IT);

/* 清除存储块的中断挂起标志 */
void FSMC_ClearITPendingBit(uint32_t FSMC_Bank, uint32_t FSMC_IT);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_FSMC_H */
/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

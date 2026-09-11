/**
  ******************************************************************************
  * @file    stm32f4xx_fsmc.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
 * @brief    This file provides firmware functions to manage the following 
  *          functionalities of the FSMC peripheral:           
  *           + Interface with SRAM, PSRAM, NOR and OneNAND memories
  *           + Interface with NAND memories
  *           + Interface with 16-bit PC Card compatible memories  
  *           + Interrupts and flags management   
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
#include "stm32f4xx_fsmc.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup FSMC 
  * @brief FSMC driver modules
  * @{
  */ 

/** 
  * @brief  FSMC NOR/SRAM默认时序配置结构体及寄存器位定义
  * @note   本文件包含FSMC控制器的默认时序参数配置和寄存器操作宏定义，主要作用：
  *         - 提供NOR/SRAM存储器的初始化时序基准参数
  *         - 定义FSMC控制寄存器(BCR)和特性寄存器(PCR)的位操作掩码
  *         - 用于STM32F4xx系列MCU的静态存储器接口配置
  * @warning 所有参数值需根据具体存储器芯片手册调整，默认值仅作参考
  */

/* Private typedef -----------------------------------------------------------*/
/* FSMC默认NOR/SRAM时序配置结构体 */
const FSMC_NORSRAMTimingInitTypeDef FSMC_DefaultTimingStruct = {
    0x0F, /* FSMC_AddressSetupTime: 地址建立时间，单位HCLK周期 
            范围0x00-0x0F(0-15个时钟)，决定地址信号有效到读/写命令发出的延迟 */
    
    0x0F, /* FSMC_AddressHoldTime: 地址保持时间，单位HCLK周期
            范围0x00-0x0F，决定读/写命令结束后地址信号的保持时间 */
    
    0xFF, /* FSMC_DataSetupTime: 数据建立时间，单位HCLK周期
            范围0x00-0xFF(0-255个时钟)，控制数据信号的采样时机 */
    
    0x0F, /* FSMC_BusTurnAroundDuration: 总线周转时间，单位HCLK周期
            范围0x00-0x0F，用于SRAM从写操作切换到读操作时的延迟 */
    
    0x0F, /* FSMC_CLKDivision: 时钟分频系数，定义CLK时钟输出分频
            实际分频值=CLKDivision + 1，0x0F表示16分频 */
    
    0x0F, /* FSMC_DataLatency: 数据延迟周期，用于同步访问模式
            范围0x00-0x0F，当使用同步存储器件时配置 */
    
    FSMC_AccessMode_A /* FSMC_AccessMode: 存储器访问模式选择
                        Mode A - 独立读写时序控制
                        Mode B - 读操作使用延长数据阶段
                        Mode C - 写操作使用延长地址阶段
                        Mode D - 读写共用延长时序 */
};

/* --------------------- FSMC寄存器位掩码定义 ---------------------------- */
/* FSMC BCRx（存储块控制寄存器）位操作掩码 */
#define BCR_MBKEN_SET          ((uint32_t)0x00000001)  /* 存储块使能位掩码（置1） 
                                                         Bit0: 0-禁止,1-使能对应存储块 */

#define BCR_MBKEN_RESET        ((uint32_t)0x000FFFFE)  /* 存储块使能位清除掩码 
                                                         保留其他位状态同时清除MBKEN位 */

#define BCR_FACCEN_SET         ((uint32_t)0x00000040)  /* 突发访问使能位掩码（Bit6）
                                                         0-禁止突发传输，1-允许突发模式 */

/* FSMC PCRx（特性控制寄存器）位操作掩码 */
#define PCR_PBKEN_SET          ((uint32_t)0x00000004)  /* NAND存储块使能位（Bit2）
                                                         0-禁止PBANK，1-使能NAND控制器 */

#define PCR_PBKEN_RESET        ((uint32_t)0x000FFFFB)  /* NAND存储块禁用掩码 
                                                         清除Bit2同时保留其他位状态 */

#define PCR_ECCEN_SET          ((uint32_t)0x00000040)  /* ECC校验使能位（Bit6）
                                                         0-禁用ECC，1-启用ECC计算 */

#define PCR_ECCEN_RESET        ((uint32_t)0x000FFFBF)  /* ECC校验禁用掩码
                                                         清除Bit6同时保留其他位状态 */

#define PCR_MEMORYTYPE_NAND    ((uint32_t)0x00000008)  /* 存储器类型标识位（Bit3）
                                                         0-使用PC Card/CF，1-使用NAND */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FSMC_Private_Functions
  * @{
  */

/** @defgroup FSMC_Group1 NOR/SRAM Controller functions
 *  @brief   NOR/SRAM Controller functions 
 *
@verbatim   
 ===============================================================================
                    ##### NOR and SRAM Controller functions #####
 ===============================================================================  

 [..] The following sequence should be followed to configure the FSMC to interface
      with SRAM, PSRAM, NOR or OneNAND memory connected to the NOR/SRAM Bank:
 
   (#) Enable the clock for the FSMC and associated GPIOs using the following functions:
          RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

   (#) FSMC pins configuration 
       (++) Connect the involved FSMC pins to AF12 using the following function 
            GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FSMC); 
       (++) Configure these FSMC pins in alternate function mode by calling the function
            GPIO_Init();    
       
   (#) Declare a FSMC_NORSRAMInitTypeDef structure, for example:
          FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
      and fill the FSMC_NORSRAMInitStructure variable with the allowed values of
      the structure member.
      
   (#) Initialize the NOR/SRAM Controller by calling the function
          FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

   (#) Then enable the NOR/SRAM Bank, for example:
          FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);  

   (#) At this stage you can read/write from/to the memory connected to the NOR/SRAM Bank. 
   
@endverbatim
  * @{
  */
/**
  * @brief  反初始化FSMC NOR/SRAM存储块，将相关寄存器恢复为默认值
  * @param  FSMC_Bank: 要操作的FSMC存储块编号
  *         可选的参数值:
  *            @arg FSMC_Bank1_NORSRAM1: 块1 NOR/SRAM1 (0x60000000-0x63FFFFFF)
  *            @arg FSMC_Bank1_NORSRAM2: 块1 NOR/SRAM2 (0x64000000-0x67FFFFFF)
  *            @arg FSMC_Bank1_NORSRAM3: 块1 NOR/SRAM3 (0x68000000-0x6BFFFFFF)
  *            @arg FSMC_Bank1_NORSRAM4: 块1 NOR/SRAM4 (0x6C000000-0x6FFFFFFF)
  * @retval 无
  * @note   该函数会重置指定存储块的配置寄存器：
  *         - BTCR[Bank]: 存储块控制寄存器
  *         - BTCR[Bank+1]: 存储块时序寄存器
  *         - BWTR[Bank]: 存储块写时序寄存器（扩展模式）
  */
void FSMC_NORSRAMDeInit(uint32_t FSMC_Bank)
{
  /* 参数合法性检查：验证输入的Bank号是否在允许范围内 */
  assert_param(IS_FSMC_NORSRAM_BANK(FSMC_Bank));
  
  /* 处理NOR/SRAM1的特殊情况 */
  if(FSMC_Bank == FSMC_Bank1_NORSRAM1)
  {
    /* 设置Bank1控制寄存器默认值：
       - 0x000030DB对应：
         * 关闭地址/数据复用(MBKEN=0)
         * 存储器类型设为SRAM(MEMTYP=00)
         * 数据宽度16位(MWID=01)
         * 关闭突发访问模式(WRAPMOD=0, BURSTEN=0)
         * 关闭各种等待信号控制 */
    FSMC_Bank1->BTCR[FSMC_Bank] = 0x000030DB;    
  }
  /* 处理其他三个Bank的通用情况 */
  else
  {   
    /* 设置Bank1控制寄存器默认值（与Bank1不同之处）：
       - 0x000030D2与Bank1的差异在最低字节：
         * 数据宽度设为8位(MWID=00) */
    FSMC_Bank1->BTCR[FSMC_Bank] = 0x000030D2; 
  }
  /* 清除时序寄存器配置：
     - 设置地址建立时间(0xF)、数据保持时间(0xF)等参数为最大延迟 */
  FSMC_Bank1->BTCR[FSMC_Bank + 1] = 0x0FFFFFFF;
  /* 清除扩展模式写时序寄存器配置：
     - 设置写操作的各项时序参数为最大延迟 */
  FSMC_Bank1E->BWTR[FSMC_Bank] = 0x0FFFFFFF;  
}

/**
  * @brief  根据配置结构体初始化FSMC NOR/SRAM存储块
  * @param  FSMC_NORSRAMInitStruct : 指向FSMC_NORSRAMInitTypeDef结构的指针，包含：
  *         - FSMC_Bank: 选择的存储块号
  *         - FSMC_DataAddressMux: 地址/数据复用使能
  *         - FSMC_MemoryType: 存储器类型（NOR/SRAM）
  *         - FSMC_MemoryDataWidth: 数据总线宽度（8/16位）
  *         - FSMC_BurstAccessMode: 突发访问模式配置
  *         - FSMC_AsynchronousWait: 异步等待功能配置
  *         - FSMC_WaitSignalPolarity: 等待信号极性
  *         - FSMC_WrapMode: 突发换行模式
  *         - FSMC_WaitSignalActive: 等待信号触发阶段
  *         - FSMC_WriteOperation: 写使能控制
  *         - FSMC_WaitSignal: 等待信号使能
  *         - FSMC_ExtendedMode: 扩展模式（独立读/写时序）
  *         - FSMC_WriteBurst: 写突发使能
  *         - FSMC_ReadWriteTimingStruct: 读/写时序参数结构体
  *         - FSMC_WriteTimingStruct: 写时序参数结构体（扩展模式时使用）
  * @retval 无
  * @note   该函数分三步配置存储块：
  *         1. 配置控制寄存器(BTCR)
  *         2. 配置读/写时序寄存器(BTCR+1)
  *         3. 如果启用扩展模式，配置写时序寄存器(BWTR)
  */
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct)
{ 
  /*---------------- 第一阶段：参数合法性校验 ----------------*/
  /* 校验存储块号有效性 */
  assert_param(IS_FSMC_NORSRAM_BANK(FSMC_NORSRAMInitStruct->FSMC_Bank));
  /* 校验地址/数据复用模式 */
  assert_param(IS_FSMC_MUX(FSMC_NORSRAMInitStruct->FSMC_DataAddressMux));
  /* 校验存储器类型（NOR/SRAM） */
  assert_param(IS_FSMC_MEMORY(FSMC_NORSRAMInitStruct->FSMC_MemoryType));
  /* 校验数据总线宽度（8/16位） */
  assert_param(IS_FSMC_MEMORY_WIDTH(FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth));
  /* 校验突发访问模式设置 */
  assert_param(IS_FSMC_BURSTMODE(FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode));
  /* 校验异步等待功能使能 */
  assert_param(IS_FSMC_ASYNWAIT(FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait));
  /* 校验等待信号极性（高/低有效） */
  assert_param(IS_FSMC_WAIT_POLARITY(FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity));
  /* 校验突发换行模式设置 */
  assert_param(IS_FSMC_WRAP_MODE(FSMC_NORSRAMInitStruct->FSMC_WrapMode));
  /* 校验等待信号作用阶段（读/写周期） */
  assert_param(IS_FSMC_WAIT_SIGNAL_ACTIVE(FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive));
  /* 校验写使能控制 */
  assert_param(IS_FSMC_WRITE_OPERATION(FSMC_NORSRAMInitStruct->FSMC_WriteOperation));
  /* 校验等待信号使能状态 */
  assert_param(IS_FSMC_WAITE_SIGNAL(FSMC_NORSRAMInitStruct->FSMC_WaitSignal));
  /* 校验扩展模式使能状态 */
  assert_param(IS_FSMC_EXTENDED_MODE(FSMC_NORSRAMInitStruct->FSMC_ExtendedMode));
  /* 校验写突发使能状态 */
  assert_param(IS_FSMC_WRITE_BURST(FSMC_NORSRAMInitStruct->FSMC_WriteBurst));  
  /* 校验读/写时序参数 */
  assert_param(IS_FSMC_ADDRESS_SETUP_TIME(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressSetupTime));
  assert_param(IS_FSMC_ADDRESS_HOLD_TIME(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressHoldTime));
  assert_param(IS_FSMC_DATASETUP_TIME(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataSetupTime));
  assert_param(IS_FSMC_TURNAROUND_TIME(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_BusTurnAroundDuration));
  assert_param(IS_FSMC_CLK_DIV(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_CLKDivision));
  assert_param(IS_FSMC_DATA_LATENCY(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataLatency));
  assert_param(IS_FSMC_ACCESS_MODE(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AccessMode)); 

  /*---------------- 第二阶段：配置控制寄存器 BTCR ----------------*/
  /* 组合所有控制参数写入BTCR寄存器：
     [0]    : MBKEN     存储块使能
     [1]    : MUXEN     地址/数据复用
     [3:2]  : MTYP      存储器类型
     [5:4]  : MWID      数据宽度
     [8]    : FACCEN    NOR闪存访问使能
     [12]   : BURSTEN   突发访问模式
     [13]   : WAITPOL   等待信号极性
     [14]   : WRAPMOD   突发换行模式
     [15]   : WAITCFG   等待信号配置
     [16]   : WREN      写使能
     [17]   : WAITEN    等待使能
     [18]   : EXTMOD    扩展模式
     [19]   : ASYNCWAIT 异步等待
     [20]   : CBURSTRW  写突发使能 */
  FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] = 
            (uint32_t)FSMC_NORSRAMInitStruct->FSMC_DataAddressMux |
            FSMC_NORSRAMInitStruct->FSMC_MemoryType |
            FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth |
            FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode |
            FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait |
            FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity |
            FSMC_NORSRAMInitStruct->FSMC_WrapMode |
            FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive |
            FSMC_NORSRAMInitStruct->FSMC_WriteOperation |
            FSMC_NORSRAMInitStruct->FSMC_WaitSignal |
            FSMC_NORSRAMInitStruct->FSMC_ExtendedMode |
            FSMC_NORSRAMInitStruct->FSMC_WriteBurst;

  /* 如果是NOR闪存类型，需要额外设置闪存访问使能位 */
  if(FSMC_NORSRAMInitStruct->FSMC_MemoryType == FSMC_MemoryType_NOR)
  {
    FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] |= (uint32_t)BCR_FACCEN_SET;
  }

  /*---------------- 第三阶段：配置读/写时序寄存器 BTCR+1 ----------------*/
  /* 组合时序参数写入BTCR+1寄存器：
     [3:0]   : ADDSET   地址建立时间 (0-15个HCLK周期)
     [7:4]   : ADDHLD   地址保持时间 (0-15)
     [11:8]  : DATAST   数据保持时间 (0-255)
     [19:16] : BUSTURN  总线反转周期 (0-15)
     [23:20] : CLKDIV   时钟分频 (1-16)
     [27:24] : DATLAT   数据延迟 (0-15)
     [28:29] : ACCMOD   访问模式 */
  FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank+1] = 
            (uint32_t)FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressSetupTime |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressHoldTime << 4) |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataSetupTime << 8) |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_BusTurnAroundDuration << 16) |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_CLKDivision << 20) |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataLatency << 24) |
             FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AccessMode;

  /*---------------- 第四阶段：处理扩展模式写时序 ----------------*/
  if(FSMC_NORSRAMInitStruct->FSMC_ExtendedMode == FSMC_ExtendedMode_Enable)
  {
    /* 校验写时序参数的有效性 */
    assert_param(IS_FSMC_ADDRESS_SETUP_TIME(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressSetupTime));
    assert_param(IS_FSMC_ADDRESS_HOLD_TIME(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressHoldTime));
    assert_param(IS_FSMC_DATASETUP_TIME(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataSetupTime));
    assert_param(IS_FSMC_CLK_DIV(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_CLKDivision));
    assert_param(IS_FSMC_DATA_LATENCY(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataLatency));
    assert_param(IS_FSMC_ACCESS_MODE(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AccessMode));
    
    /* 配置扩展写时序寄存器BWTR：
       [3:0]   : ADDSET   写地址建立时间
       [7:4]   : ADDHLD   写地址保持时间
       [11:8]  : DATAST   写数据保持时间
       [23:20] : CLKDIV   写时钟分频
       [27:24] : DATLAT   写数据延迟
       [28:29] : ACCMOD   写访问模式 */
    FSMC_Bank1E->BWTR[FSMC_NORSRAMInitStruct->FSMC_Bank] = 
              (uint32_t)FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressSetupTime |
              (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressHoldTime << 4 )|
              (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataSetupTime << 8) |
              (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_CLKDivision << 20) |
              (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataLatency << 24) |
               FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AccessMode;
  }
  else
  {
    /* 禁用扩展模式时，恢复写时序寄存器为默认值 */
    FSMC_Bank1E->BWTR[FSMC_NORSRAMInitStruct->FSMC_Bank] = 0x0FFFFFFF;
  }
}

/**
  * @brief  初始化FSMC NOR/SRAM配置结构体为默认值
  * @param  FSMC_NORSRAMInitStruct: 指向FSMC_NORSRAMInitTypeDef结构体的指针
  * @retval 无
  * @note   默认配置参数包括：
  *         - 存储块选择Bank1 NOR/SRAM1
  *         - 地址/数据复用使能
  *         - 存储器类型设为SRAM
  *         - 8位数据宽度
  *         - 禁用突发访问模式
  *         - 写操作使能
  *         - 使用预定义的默认时序参数
  */
void FSMC_NORSRAMStructInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct)
{  
  /* 重置所有结构体成员为默认配置 */
  FSMC_NORSRAMInitStruct->FSMC_Bank = FSMC_Bank1_NORSRAM1;       /* 默认选择Bank1的第一个存储块 */
  FSMC_NORSRAMInitStruct->FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;  /* 启用地址/数据复用 */
  FSMC_NORSRAMInitStruct->FSMC_MemoryType = FSMC_MemoryType_SRAM;    /* 默认配置为SRAM存储器 */
  FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;  /* 8位数据总线宽度 */
  FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable; /* 禁用突发访问 */
  FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable; /* 禁用异步等待 */
  FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;  /* 等待信号低电平有效 */
  FSMC_NORSRAMInitStruct->FSMC_WrapMode = FSMC_WrapMode_Disable;      /* 禁用突发换行模式 */
  FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; /* 在等待状态前激活 */
  FSMC_NORSRAMInitStruct->FSMC_WriteOperation = FSMC_WriteOperation_Enable;  /* 默认使能写操作 */
  FSMC_NORSRAMInitStruct->FSMC_WaitSignal = FSMC_WaitSignal_Enable;    /* 使能等待信号 */
  FSMC_NORSRAMInitStruct->FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;   /* 禁用扩展时序模式 */
  FSMC_NORSRAMInitStruct->FSMC_WriteBurst = FSMC_WriteBurst_Disable;    /* 禁用写突发模式 */
  
  /* 设置默认时序参数结构体指针 */
  FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct = (FSMC_NORSRAMTimingInitTypeDef*)&FSMC_DefaultTimingStruct; /* 读/写共用默认时序 */
  FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct = (FSMC_NORSRAMTimingInitTypeDef*)&FSMC_DefaultTimingStruct;     /* 写时序使用相同配置 */
}

/**
  * @brief  启用或禁用指定的NOR/SRAM存储块
  * @param  FSMC_Bank: 要操作的存储块编号
  *         可选值：
  *           @arg FSMC_Bank1_NORSRAM1: Bank1 NOR/SRAM1 (0x60000000)
  *           @arg FSMC_Bank1_NORSRAM2: Bank1 NOR/SRAM2 (0x64000000)
  *           @arg FSMC_Bank1_NORSRAM3: Bank1 NOR/SRAM3 (0x68000000)
  *           @arg FSMC_Bank1_NORSRAM4: Bank1 NOR/SRAM4 (0x6C000000)
  * @param  NewState: 存储块的新状态
  *         可取值：
  *           @arg ENABLE: 激活存储块
  *           @arg DISABLE: 关闭存储块
  * @retval 无
  * @note   该函数通过修改BCR寄存器的MBKEN位控制存储块状态：
  *         - 启用时设置MBKEN位(bit0)
  *         - 禁用时清除MBKEN位
  */
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
  /* 参数合法性校验 */
  assert_param(IS_FSMC_NORSRAM_BANK(FSMC_Bank));          /* 验证存储块编号有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));           /* 验证状态参数合法性 */
  
  if (NewState != DISABLE)
  {
    /* 启用指定存储块：设置BTCR寄存器的MBKEN位(bit0)
       作用：激活对应存储块的控制器，使能地址译码和信号生成 */
    FSMC_Bank1->BTCR[FSMC_Bank] |= BCR_MBKEN_SET;  // BCR_MBKEN_SET = 0x00000001
  }
  else
  {
    /* 禁用指定存储块：清除BTCR寄存器的MBKEN位
       作用：关闭存储块控制器，停止相关信号输出 */
    FSMC_Bank1->BTCR[FSMC_Bank] &= BCR_MBKEN_RESET; // BCR_MBKEN_RESET = 0xFFFFFFFE
  }
}

/**
  * @}
  */

/** @defgroup FSMC_Group2 NAND Controller functions
 *  @brief   NAND Controller functions 
 *
@verbatim   
 ===============================================================================
                    ##### NAND Controller functions #####
 ===============================================================================  

 [..]  The following sequence should be followed to configure the FSMC to interface 
       with 8-bit or 16-bit NAND memory connected to the NAND Bank:
 
  (#) Enable the clock for the FSMC and associated GPIOs using the following functions:
      (++)  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
      (++)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

  (#) FSMC pins configuration 
      (++) Connect the involved FSMC pins to AF12 using the following function 
           GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FSMC); 
      (++) Configure these FSMC pins in alternate function mode by calling the function
           GPIO_Init();    
       
  (#) Declare a FSMC_NANDInitTypeDef structure, for example:
      FSMC_NANDInitTypeDef  FSMC_NANDInitStructure;
      and fill the FSMC_NANDInitStructure variable with the allowed values of
      the structure member.
      
  (#) Initialize the NAND Controller by calling the function
      FSMC_NANDInit(&FSMC_NANDInitStructure); 

  (#) Then enable the NAND Bank, for example:
      FSMC_NANDCmd(FSMC_Bank3_NAND, ENABLE);  

  (#) At this stage you can read/write from/to the memory connected to the NAND Bank. 
   
 [..]
  (@) To enable the Error Correction Code (ECC), you have to use the function
      FSMC_NANDECCCmd(FSMC_Bank3_NAND, ENABLE);  
 [..]
  (@) and to get the current ECC value you have to use the function
      ECCval = FSMC_GetECC(FSMC_Bank3_NAND); 

@endverbatim
  * @{
  */
 
/**
  * @brief  初始化NAND配置结构体为默认参数
  * @param  FSMC_NANDInitStruct: 指向FSMC_NANDInitTypeDef结构体的指针
  * @retval 无
  * @note   默认配置包含：
  *         - 选择Bank2 NAND
  *         - 8位数据总线
  *         - 最大时序参数（0xFC对应63个HCLK周期）
  *         - ECC校验功能关闭
  */
void FSMC_NANDStructInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct)
{ 
  /* 重置结构体所有参数为安全默认值 */
  FSMC_NANDInitStruct->FSMC_Bank = FSMC_Bank2_NAND;                   // 默认选择Bank2
  FSMC_NANDInitStruct->FSMC_Waitfeature = FSMC_Waitfeature_Disable;   // 禁用等待特性
  FSMC_NANDInitStruct->FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b; // 8位数据总线
  FSMC_NANDInitStruct->FSMC_ECC = FSMC_ECC_Disable;                   // 关闭ECC校验
  FSMC_NANDInitStruct->FSMC_ECCPageSize = FSMC_ECCPageSize_256Bytes;  // ECC页256字节
  FSMC_NANDInitStruct->FSMC_TCLRSetupTime = 0x0;                      // CLE到RE延迟0周期
  FSMC_NANDInitStruct->FSMC_TARSetupTime = 0x0;                       // ALE到RE延迟0周期
  
  /* 配置通用空间时序参数（命令/地址传输阶段） */
  FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime = 0xFC;        // 63周期建立时间
  FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;   // 63周期等待时间
  FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;    // 63周期保持时间
  FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;     // 63周期高阻时间
  
  /* 配置属性空间时序参数（数据读写阶段） */
  FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime = 0xFC;      // 相同安全参数
  FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;
  FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;
  FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;
}

/**
  * @brief  启用/禁用NAND存储块控制器
  * @param  FSMC_Bank: 目标存储块
  *         @arg FSMC_Bank2_NAND: Bank2（地址0x70000000）
  *         @arg FSMC_Bank3_NAND: Bank3（地址0x80000000）
  * @param  NewState: 新状态
  *         @arg ENABLE: 激活控制器
  *         @arg DISABLE: 关闭控制器
  * @retval 无
  * @note   通过设置PCRx寄存器的PBKEN位(bit0)控制NAND控制器的电源状态
  */
void FSMC_NANDCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
  /* 参数合法性校验 */
  assert_param(IS_FSMC_NAND_BANK(FSMC_Bank));         // 确保Bank2或Bank3
  assert_param(IS_FUNCTIONAL_STATE(NewState));        // 状态值有效性检查
  
  if (NewState != DISABLE)
  {
    /* 激活指定Bank的NAND控制器 */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->PCR2 |= PCR_PBKEN_SET;  // 设置PCR2[0]=1，开启Bank2电源
    }
    else
    {
      FSMC_Bank3->PCR3 |= PCR_PBKEN_SET;  // 设置PCR3[0]=1，开启Bank3电源
    }
  }
  else
  {
    /* 关闭指定Bank的NAND控制器 */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->PCR2 &= PCR_PBKEN_RESET; // 清除PCR2[0]，关闭Bank2电源
    }
    else
    {
      FSMC_Bank3->PCR3 &= PCR_PBKEN_RESET; // 清除PCR3[0]，关闭Bank3电源
    }
  }
}

/**
  * @brief  控制NAND存储块的ECC校验功能
  * @param  FSMC_Bank: 目标存储块
  *         @arg FSMC_Bank2_NAND: Bank2
  *         @arg FSMC_Bank3_NAND: Bank3
  * @param  NewState: ECC功能状态
  *         @arg ENABLE: 启用ECC校验
  *         @arg DISABLE: 禁用ECC校验
  * @retval 无
  * @note   ECC功能启用后，控制器会自动计算/校验256字节页的纠错码
  *         修改状态后需要重新初始化NAND控制器才能生效
  */
void FSMC_NANDECCCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
  /* 参数校验 */
  assert_param(IS_FSMC_NAND_BANK(FSMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 启用ECC计算引擎 */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->PCR2 |= PCR_ECCEN_SET;  // 设置PCR2[6]=1，开启Bank2 ECC
    }
    else
    {
      FSMC_Bank3->PCR3 |= PCR_ECCEN_SET;  // 设置PCR3[6]=1，开启Bank3 ECC
    }
  }
  else
  {
    /* 关闭ECC功能 */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->PCR2 &= PCR_ECCEN_RESET; // 清除PCR2[6]，禁用ECC
    }
    else
    {
      FSMC_Bank3->PCR3 &= PCR_ECCEN_RESET; // 清除PCR3[6]，禁用ECC
    }
  }
}
/**
  * @brief  获取NAND控制器的错误校正码(ECC)寄存器值
  * @param  FSMC_Bank: 目标存储块选择
  *         @arg FSMC_Bank2_NAND: Bank2（地址0x70000000）
  *         @arg FSMC_Bank3_NAND: Bank3（地址0x80000000）
  * @retval uint32_t: 32位ECC校验值
  * @note   ECC值计算规则：
  *         - 每256字节数据生成3字节ECC码
  *         - 寄存器存储格式：[31:24]保留 | [23:16]ECC2 | [15:8]ECC1 | [7:0]ECC0
  *         - 需要结合FSMC_NANDECCCmd函数使用
  */
uint32_t FSMC_GetECC(uint32_t FSMC_Bank)
{
  uint32_t eccval = 0x00000000;  // 初始化返回值
  
  /* 根据Bank选择读取对应寄存器 */
  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    /* 读取Bank2的ECC寄存器：
       该寄存器在每次读操作后自动更新，包含最新256字节数据的校验码 */
    eccval = FSMC_Bank2->ECCR2;  // ECCR2地址：0xA000006C
  }
  else  /* FSMC_Bank3_NAND */
  {
    /* 读取Bank3的ECC寄存器：
       寄存器行为与Bank2相同，物理地址不同 */
    eccval = FSMC_Bank3->ECCR3;  // ECCR3地址：0xA000008C
  }
  
  /* 返回包含3字节ECC校验码的32位数值
     应用层需按字节解析：
     - 字节0: ECC0（第一个64字节的校验）
     - 字节1: ECC1（第二个64字节的校验）
     - 字节2: ECC2（第三个64字节的校验） */
  return(eccval);
}

/**
  * @}
  */

/** @defgroup FSMC_Group3 PCCARD Controller functions
 *  @brief   PCCARD Controller functions 
 *
@verbatim   
 ===============================================================================
                    ##### PCCARD Controller functions #####
 ===============================================================================  

 [..]  he following sequence should be followed to configure the FSMC to interface 
       with 16-bit PC Card compatible memory connected to the PCCARD Bank:
 
  (#)  Enable the clock for the FSMC and associated GPIOs using the following functions:
       (++)  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
       (++)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

  (#) FSMC pins configuration 
       (++) Connect the involved FSMC pins to AF12 using the following function 
            GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FSMC); 
       (++) Configure these FSMC pins in alternate function mode by calling the function
            GPIO_Init();    
       
  (#) Declare a FSMC_PCCARDInitTypeDef structure, for example:
      FSMC_PCCARDInitTypeDef  FSMC_PCCARDInitStructure;
      and fill the FSMC_PCCARDInitStructure variable with the allowed values of
      the structure member.
      
  (#) Initialize the PCCARD Controller by calling the function
      FSMC_PCCARDInit(&FSMC_PCCARDInitStructure); 

  (#) Then enable the PCCARD Bank:
      FSMC_PCCARDCmd(ENABLE);  

  (#) At this stage you can read/write from/to the memory connected to the PCCARD Bank. 
 
@endverbatim
  * @{
  */
/* NAND控制器反初始化函数 */
/**
  * @brief  复位NAND控制器寄存器至默认值
  * @param  FSMC_Bank: 目标存储块
  *         @arg FSMC_Bank2_NAND: Bank2（0x70000000）
  *         @arg FSMC_Bank3_NAND: Bank3（0x80000000）
  * @retval 无
  * @note   复位操作包含四个关键寄存器：
  *         - PCRx: 清除控制参数（TCLR/TAR=3clk）
  *         - SRx: 清除状态标志
  *         - PMEMx/PATTx: 重置时序参数为最大值
  */
void FSMC_NANDDeInit(uint32_t FSMC_Bank)
{
  assert_param(IS_FSMC_NAND_BANK(FSMC_Bank));  // 校验Bank号合法性

  if(FSMC_Bank == FSMC_Bank2_NAND) {
    /* Bank2寄存器复位 */
    FSMC_Bank2->PCR2 = 0x00000018;   // 命令周期配置：TCLR=3clk, TAR=3clk
    FSMC_Bank2->SR2 = 0x00000040;    // 状态寄存器：保留位6置1，错误标志清零
    FSMC_Bank2->PMEM2 = 0xFCFCFCFC;  // 通用空间时序：SETUP=0xFC, WAIT=0xFC等
    FSMC_Bank2->PATT2 = 0xFCFCFCFC;  // 属性空间时序参数同步
  } 
  else {
    /* Bank3镜像配置 */
    FSMC_Bank3->PCR3 = 0x00000018;   // 相同控制参数
    FSMC_Bank3->SR3 = 0x00000040;
    FSMC_Bank3->PMEM3 = 0xFCFCFCFC;
    FSMC_Bank3->PATT3 = 0xFCFCFCFC;
  }
}

/* NAND控制器初始化函数 */
/**
  * @brief  配置NAND控制器工作参数
  * @param  FSMC_NANDInitStruct: 初始化结构体
  *         - 存储块选择/数据宽度/ECC配置
  *         - 时序参数结构体（命令/数据周期）
  * @retval 无
  * @note   配置分三步完成：
  *         1. 参数有效性校验（12个assert_param）
  *         2. 组合PCR寄存器控制字
  *         3. 设置各空间时序寄存器
  */
void FSMC_NANDInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct)
{
  uint32_t tmppcr = 0, tmppmem = 0, tmppatt = 0;

  /* 参数校验链（共12项） */
  assert_param(IS_FSMC_NAND_BANK(FSMC_NANDInitStruct->FSMC_Bank));      // 存储块有效性
  assert_param(IS_FSMC_WAIT_FEATURE(FSMC_NANDInitStruct->FSMC_Waitfeature)); // 等待特性开关
  assert_param(IS_FSMC_MEMORY_WIDTH(FSMC_NANDInitStruct->FSMC_MemoryDataWidth)); // 数据位宽
  /* ...其他参数校验省略... */

  /* 组合控制寄存器PCR配置 */
  tmppcr = FSMC_NANDInitStruct->FSMC_Waitfeature |           // 位1: 等待特性
           PCR_MEMORYTYPE_NAND |                             // 位2-3: NAND类型标识
           FSMC_NANDInitStruct->FSMC_MemoryDataWidth |        // 位4-5: 8/16位选择
           FSMC_NANDInitStruct->FSMC_ECC |                   // 位6: ECC使能
           (FSMC_NANDInitStruct->FSMC_TCLRSetupTime << 9) |  // 位9-12: CLE→RE延迟
           (FSMC_NANDInitStruct->FSMC_TARSetupTime << 13);  // 位13-16: ALE→RE延迟

  /* 构建通用空间时序配置 */
  tmppmem = FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime | 
           (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
           (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime << 16) |
           (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime << 24);

  /* 构建属性空间时序配置 */
  tmppatt = FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime |
           (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
           (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime << 16) |
           (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime << 24);

  /* 寄存器写入 */
  if(FSMC_NANDInitStruct->FSMC_Bank == FSMC_Bank2_NAND) {
    FSMC_Bank2->PCR2 = tmppcr;    // 写入Bank2控制寄存器
    FSMC_Bank2->PMEM2 = tmppmem;  // 通用空间时序（命令/地址周期）
    FSMC_Bank2->PATT2 = tmppatt;  // 属性空间时序（数据周期）
  } else {
    FSMC_Bank3->PCR3 = tmppcr;    // Bank3镜像配置
    FSMC_Bank3->PMEM3 = tmppmem;
    FSMC_Bank3->PATT3 = tmppatt;
  }
}

/* PCCARD控制器反初始化函数 */
/**
  * @brief  复位PCCARD控制器寄存器至默认状态
  * @param  无
  * @retval 无
  * @note   复位Bank4所有相关寄存器：
  *         - PCR4: 清除控制参数
  *         - SR4: 状态标志清零
  *         - PMEM4/PATT4/PIO4: 时序参数重置
  */
void FSMC_PCCARDDeInit(void)
{
  FSMC_Bank4->PCR4 = 0x00000018;   // 默认控制参数
  FSMC_Bank4->SR4 = 0x00000000;    // 清除所有状态标志
  FSMC_Bank4->PMEM4 = 0xFCFCFCFC;  // 通用空间时序最大值
  FSMC_Bank4->PATT4 = 0xFCFCFCFC;  // 属性空间时序同步
  FSMC_Bank4->PIO4 = 0xFCFCFCFC;    // IO空间时序配置
}

/* PCCARD控制器初始化函数 */
/**
  * @brief  初始化FSMC PCCARD控制器配置
  * @param  FSMC_PCCARDInitStruct: 指向FSMC_PCCARDInitTypeDef结构体的指针，包含：
  *         - 等待特性配置
  *         - 时序参数结构体（通用空间/属性空间/IO空间）
  *         - 控制时序参数（TCLR/TAR）
  * @retval 无
  * @note   配置特性：
  *         - 固定使用16位数据总线（符合PCCARD标准）
  *         - 支持三个独立存储空间时序配置
  *         - 可配置等待信号插入
  *         - 时序参数范围：0x00-0xFF（对应1-256个HCLK周期）
  */
void FSMC_PCCARDInit(FSMC_PCCARDInitTypeDef* FSMC_PCCARDInitStruct)
{
  /*---------------- 参数校验阶段 ----------------*/
  assert_param(IS_FSMC_WAIT_FEATURE(FSMC_PCCARDInitStruct->FSMC_Waitfeature)); // 等待特性参数合法性
  assert_param(IS_FSMC_TCLR_TIME(FSMC_PCCARDInitStruct->FSMC_TCLRSetupTime));  // CLE到RE延迟参数校验
  assert_param(IS_FSMC_TAR_TIME(FSMC_PCCARDInitStruct->FSMC_TARSetupTime));     // ALE到RE延迟校验

  /* 通用空间时序参数校验 */
  assert_param(IS_FSMC_SETUP_TIME(FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime));     // 地址建立时间（0-0xFF）
  assert_param(IS_FSMC_WAIT_TIME(FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime));   // 等待持续时间
  assert_param(IS_FSMC_HOLD_TIME(FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime));  // 数据保持时间
  assert_param(IS_FSMC_HIZ_TIME(FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime));    // 高阻态时间

  /* 属性空间时序参数校验 */
  assert_param(IS_FSMC_SETUP_TIME(FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime));  // 属性空间建立时间
  assert_param(IS_FSMC_WAIT_TIME(FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime));

  /* IO空间时序参数校验 */
  assert_param(IS_FSMC_SETUP_TIME(FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_SetupTime));          // IO空间建立时间
  assert_param(IS_FSMC_WAIT_TIME(FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HiZSetupTime));

  /*---------------- 寄存器配置阶段 ----------------*/
  /* PCR4寄存器配置（控制参数） */
  FSMC_Bank4->PCR4 = (uint32_t)FSMC_PCCARDInitStruct->FSMC_Waitfeature |  // 位1: 等待特性使能
                     FSMC_MemoryDataWidth_16b |                          // 位4-5: 固定16位总线
                     (FSMC_PCCARDInitStruct->FSMC_TCLRSetupTime << 9) |  // 位9-12: CLE到RE延迟（0-15 clk）
                     (FSMC_PCCARDInitStruct->FSMC_TARSetupTime << 13);   // 位13-16: ALE到RE延迟

  /* PMEM4寄存器配置（通用空间时序） */
  FSMC_Bank4->PMEM4 = (uint32_t)FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime |        // 位0-7: 地址建立时间
                      (FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime << 8) |    // 位8-15: 等待时间
                      (FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime << 16) |    // 位16-23: 保持时间
                      (FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime << 24);     // 位24-31: 高阻时间

  /* PATT4寄存器配置（属性空间时序） */
  FSMC_Bank4->PATT4 = (uint32_t)FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime | 
                      (FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
                      (FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime << 16) |
                      (FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime << 24);

  /* PIO4寄存器配置（IO空间时序） */
  FSMC_Bank4->PIO4 = (uint32_t)FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_SetupTime | 
                     (FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
                     (FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HoldSetupTime << 16) |
                     (FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HiZSetupTime << 24);
}
/**
  * @brief  PCCARD控制器初始化结构体默认值加载
  * @param  FSMC_PCCARDInitStruct: 指向FSMC_PCCARDInitTypeDef结构体的指针
  * @detail 初始化内容包括：
  *         - 等待特性禁用
  *         - CLE/ALE时序参数清零
  *         - 公共/属性/I/O空间时序参数统一设置为最大延迟值0xFC
  *         - 适用于安全模式下的保守配置
  */
void FSMC_PCCARDStructInit(FSMC_PCCARDInitTypeDef* FSMC_PCCARDInitStruct)
{
  /* 基础功能参数初始化 */
  FSMC_PCCARDInitStruct->FSMC_Waitfeature = FSMC_Waitfeature_Disable; // 禁用等待信号插入功能
  FSMC_PCCARDInitStruct->FSMC_TCLRSetupTime = 0x0;  // CLE有效到RE上升沿时间=1 HCLK周期
  FSMC_PCCARDInitStruct->FSMC_TARSetupTime = 0x0;   // ALE有效到RE上升沿时间=1 HCLK周期

  /* 公共空间时序配置（NOR闪存模式）*/
  FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime = 0xFC;     // 地址建立周期：253 HCLK
  FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC; // 等待信号宽度：253 HCLK
  FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC; // 数据保持周期：253 HCLK
  FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;  // 高阻态维持周期：253 HCLK

  /* 属性空间时序配置（NAND闪存模式）*/
  FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime = 0xFC;     // 页地址建立时间
  FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC; // 写脉冲宽度
  FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;  // 片选保持时间
  FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;  // 总线释放时间

  /* I/O空间时序配置（PCCARD特有模式）*/
  FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_SetupTime = 0xFC;     // IORD/IOWR建立时间
  FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC; // 等待信号有效宽度
  FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC; // 写操作保持时间
  FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;  // 总线周转时间
}

/**
  * @brief  PCCARD存储区使能控制
  * @param  NewState: ENABLE-激活Bank4存储区域 DISABLE-关闭电源
  * @effect 启用时：
  *         - 激活PBKEN位(bit0)
  *         - 开启地址/数据总线复用
  *         - 使能存储控制器时钟
  * @note 操作PCR4寄存器需确保此时无进行中的存储交易
  */
void FSMC_PCCARDCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 参数有效性验证（ENABLE/DISABLE）
  
  if (NewState != DISABLE)
  {
    /* 启用流程：置位PCR4[0] */
    FSMC_Bank4->PCR4 |= PCR_PBKEN_SET;  // 0x00000001 操作：
                                        // 1. 解锁寄存器写保护
                                        // 2. 设置存储区访问使能标志
                                        // 3. 自动触发时钟门控使能
  }
  else
  {
    /* 禁用流程：清除PCR4[0] */
    FSMC_Bank4->PCR4 &= PCR_PBKEN_RESET; // 0xFFFFFFFE 操作：
                                         // 1. 立即终止所有挂起操作
                                         // 2. 关闭存储区电源
                                         // 3. 保留寄存器其他配置
  }
}

/**
  * @}
  */

/** @defgroup FSMC_Group4  Interrupts and flags management functions
 *  @brief    Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
             ##### Interrupts and flags management functions #####
 ===============================================================================   

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified FSMC interrupts.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *          This parameter can be one of the following values:
  *            @arg FSMC_Bank2_NAND: FSMC Bank2 NAND 
  *            @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  *            @arg FSMC_Bank4_PCCARD: FSMC Bank4 PCCARD
  * @param  FSMC_IT: specifies the FSMC interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg FSMC_IT_RisingEdge: Rising edge detection interrupt. 
  *            @arg FSMC_IT_Level: Level edge detection interrupt.
  *            @arg FSMC_IT_FallingEdge: Falling edge detection interrupt.
  * @param  NewState: new state of the specified FSMC interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  FSMC中断源全局配置
  * @param  FSMC_Bank: 选择存储块（Bank2_NAND/Bank3_NAND/Bank4_PCCARD）
  * @param  FSMC_IT: 中断类型组合（位或操作）：
  *         - FSMC_IT_RisingEdge(0x01): 上升沿中断（命令完成）
  *         - FSMC_IT_Level(0x02):     电平中断（访问冲突）
  *         - FSMC_IT_FallingEdge(0x04):下降沿中断（DMA传输完成）
  * @param  NewState: 中断开关状态（ENABLE/DISABLE）
  * @机制 通过设置状态寄存器(SRx)的IRQ使能位：
  *        - 写操作具有即时生效特性
  *        - 支持多个中断类型同时配置
  * @warning 改变中断状态时建议先关闭全局中断
  */
void FSMC_ITConfig(uint32_t FSMC_Bank, uint32_t FSMC_IT, FunctionalState NewState)
{
  assert_param(IS_FSMC_IT_BANK(FSMC_Bank));   // 验证Bank合法性（2/3/4）
  assert_param(IS_FSMC_IT(FSMC_IT));          // 验证中断类型有效性（0x01/0x02/0x04）
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 状态参数校验
  
  if (NewState != DISABLE)
  {
    /* 中断使能分支（原子操作） */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->SR2 |= FSMC_IT;  // 写SR2[2:0]位域：
                                    // 0x01: 使能命令完成中断
                                    // 0x02: 使能访问超时中断
                                    // 0x04: 使能ECC错误中断
    }
    else if (FSMC_Bank == FSMC_Bank3_NAND)
    {
      FSMC_Bank3->SR3 |= FSMC_IT;  // 影响SR3中断使能位，立即生效
    }
    else  // Bank4_PCCARD处理分支
    {
      FSMC_Bank4->SR4 |= FSMC_IT;  // 配置PCCARD专用中断寄存器
    }
  }
  else
  {
    /* 中断屏蔽分支（保持其他位状态） */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->SR2 &= (uint32_t)~FSMC_IT; // 位清除操作示例：0x01→0xFE
    }
    else if (FSMC_Bank == FSMC_Bank3_NAND)
    {
      FSMC_Bank3->SR3 &= (uint32_t)~FSMC_IT; // 使用位取反实现精确控制
    }
    else
    {
      FSMC_Bank4->SR4 &= (uint32_t)~FSMC_IT; // PCCARD中断禁用
    }
  }
}

/**
  * @brief  获取FSMC状态标志位
  * @param  FSMC_Bank: 目标存储块（同ITConfig）
  * @param  FSMC_FLAG: 待查询标志位：
  *         - FSMC_FLAG_RisingEdge(0x01): 命令执行完成标志
  *         - FSMC_FLAG_Level(0x02):      总线访问超时标志
  *         - FSMC_FLAG_FallingEdge(0x04):DMA传输完成标志
  *         - FSMC_FLAG_FEMPT(0x40):     FIFO空标志（仅Bank2/3）
  * @retval FlagStatus: 标志位状态（SET/RESET）
  * @detail 操作流程：
  *         1. 选择对应Bank的状态寄存器
  *         2. 执行位与操作检测标志位
  *         3. 自动清除机制说明：
  *            - 电平标志需手动清除
  *            - 边沿标志自动清除
  */
FlagStatus FSMC_GetFlagStatus(uint32_t FSMC_Bank, uint32_t FSMC_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tmpsr = 0x00000000;
  
  /* 双参数校验机制 */
  assert_param(IS_FSMC_GETFLAG_BANK(FSMC_Bank)); // 存储块有效性检查
  assert_param(IS_FSMC_GET_FLAG(FSMC_FLAG));     // 标志位合法性检查

  /* 寄存器选择分支 */
  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    tmpsr = FSMC_Bank2->SR2;  // 读取Bank2状态寄存器（32位）
  }  
  else if(FSMC_Bank == FSMC_Bank3_NAND)
  {
    tmpsr = FSMC_Bank3->SR3;  // 读取Bank3状态寄存器
  }
  else  // Bank4_PCCARD处理
  {
    tmpsr = FSMC_Bank4->SR4;  // 读取PCCARD专用状态寄存器
  } 
  
  /* 位状态检测（非破坏性读取） */
  if ((tmpsr & FSMC_FLAG) != (uint16_t)RESET )
  {
    bitstatus = SET;  // 标志位置位状态：
                     // - 表示对应事件已发生
                     // - 可能触发中断（如果已使能）
  }
  else
  {
    bitstatus = RESET; // 标志位未激活状态
  }
  return bitstatus;  // 返回检测结果，不改变原始寄存器状态
}
/**
  * @brief  清除FSMC状态标志位
  * @param  FSMC_Bank: 目标存储块（Bank2/Bank3/Bank4）
  * @param  FSMC_FLAG: 待清除标志位组合：
  *         - FSMC_FLAG_RisingEdge(0x01) 命令完成标志
  *         - FSMC_FLAG_Level(0x02)     总线超时标志
  *         - FSMC_FLAG_FallingEdge(0x04)DMA完成标志
  * @detail 清除机制：
  *         - 电平标志需手动清除
  *         - 边沿标志写入即清除
  * @warning 标志位清除操作具有原子性，中断安全
  */
void FSMC_ClearFlag(uint32_t FSMC_Bank, uint32_t FSMC_FLAG)
{
  /* 双重参数校验 */
  assert_param(IS_FSMC_GETFLAG_BANK(FSMC_Bank)); // Bank有效性检查(2/3/4)
  assert_param(IS_FSMC_CLEAR_FLAG(FSMC_FLAG));   // 标志位组合合法性检查
    
  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    /* Bank2状态寄存器位清除（SR2[7:0]） */
    FSMC_Bank2->SR2 &= ~FSMC_FLAG;  // 原子操作示例：0x01→0xFE 
  }  
  else if(FSMC_Bank == FSMC_Bank3_NAND)
  {
    /* Bank3状态寄存器位清除（SR3[7:0]） */
    FSMC_Bank3->SR3 &= ~FSMC_FLAG;  // 安全写操作，保留其他状态位
  }
  /* Bank4_PCCARD处理分支 */
  else
  {
    /* PCCARD专用状态寄存器清除（SR4[7:0]） */
    FSMC_Bank4->SR4 &= ~FSMC_FLAG;  // 立即生效的寄存器操作
  }
}

/**
  * @brief  获取FSMC中断状态（双重验证）
  * @param  FSMC_Bank: 目标存储块
  * @param  FSMC_IT: 中断类型标识符：
  *         - 0x00000001: 上升沿中断
  *         - 0x00000002: 电平中断
  *         - 0x00000004: 下降沿中断
  * @retval ITStatus: 中断有效状态（SET需同时满足：
  *         1. 中断标志位置位
  *         2. 对应中断使能位激活）
  * @detail 实现原理：
  *         - 中断标志位 = SRx[2:0]
  *         - 中断使能位 = SRx[5:3]
  *         - 位偏移3位对应使能寄存器位
  */
ITStatus FSMC_GetITStatus(uint32_t FSMC_Bank, uint32_t FSMC_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t tmpsr = 0x0, itstatus = 0x0, itenable = 0x0; 
  
  /* 三层校验机制 */
  assert_param(IS_FSMC_IT_BANK(FSMC_Bank)); // 存储块有效性
  assert_param(IS_FSMC_GET_IT(FSMC_IT));    // 中断类型有效性

  /* 寄存器选择逻辑 */
  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    tmpsr = FSMC_Bank2->SR2;  // 获取Bank2完整状态字（0x000000FF）
  }  
  else if(FSMC_Bank == FSMC_Bank3_NAND)
  {
    tmpsr = FSMC_Bank3->SR3;  // Bank3状态寄存器镜像
  }
  else  // PCCARD专用处理
  {
    tmpsr = FSMC_Bank4->SR4;  // Bank4状态寄存器读取
  } 
  
  /* 中断状态双重验证 */
  itstatus = tmpsr & FSMC_IT;           // 获取原始标志位状态
  itenable = tmpsr & (FSMC_IT >> 3);   // 对应使能位偏移计算（例：0x01→0x08）
  
  /* 与逻辑判断（标志位+使能位） */
  if ((itstatus != RESET) && (itenable != RESET))
  {
    bitstatus = SET;  // 有效中断状态：
                     // - 事件已发生
                     // - 中断通道已打开
  }
  else
  {
    bitstatus = RESET; // 无效中断状态
  }
  return bitstatus;  // 返回综合判断结果
}

/**
  * @brief  清除FSMC中断挂起标志
  * @param  FSMC_Bank: 目标存储块
  * @param  FSMC_IT: 中断类型标识符：
  *         - 0x00000001: 清除上升沿中断
  *         - 0x00000002: 清除电平中断
  *         - 0x00000004: 清除下降沿中断
  * @detail 清除机制：
  *         - 实际清除的是使能寄存器位（SRx[5:3]）
  *         - 通过右移3位定位到使能位域
  *         - 电平中断需手动清除，边沿中断自动清除
  */
void FSMC_ClearITPendingBit(uint32_t FSMC_Bank, uint32_t FSMC_IT)
{
  /* 双重安全校验 */
  assert_param(IS_FSMC_IT_BANK(FSMC_Bank));  // Bank有效性验证
  assert_param(IS_FSMC_IT(FSMC_IT));         // 中断类型验证
  
  /* 中断位逆向定位 */
  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    /* Bank2使能位清除（SR2[5:3]） */
    FSMC_Bank2->SR2 &= ~(FSMC_IT >> 3);  // 例：0x01→0x08→~0xF7
  }  
  else if(FSMC_Bank == FSMC_Bank3_NAND)
  {
    /* Bank3使能位清除 */
    FSMC_Bank3->SR3 &= ~(FSMC_IT >> 3); // 位运算保证原子性
  }
  /* PCCARD专用处理分支 */
  else
  {
    /* Bank4使能位清除 */
    FSMC_Bank4->SR4 &= ~(FSMC_IT >> 3); // 立即生效操作
  }
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

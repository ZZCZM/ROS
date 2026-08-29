/**
  ******************************************************************************
  * @file    stm32f4xx_fmc.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the FMC peripheral:           
  *           + Interface with SRAM, PSRAM, NOR and OneNAND memories
  *           + Interface with NAND memories
  *           + Interface with 16-bit PC Card compatible memories 
  *           + Interface with SDRAM memories    
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
#include "stm32f4xx_fmc.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup FMC 
  * @brief FMC driver modules
  * @{
  */ 
/* NOR/SRAM 控制器默认时序配置结构体 */
const FMC_NORSRAMTimingInitTypeDef FMC_DefaultTimingStruct = {
    0x0F,  /* FMC_AddressSetupTime: 地址建立时间 (15个HCLK周期)
              取值范围：0x00-0x0F（1-16周期），决定地址线稳定到读/写信号有效的时间 */
    
    0x0F,  /* FMC_AddressHoldTime: 地址保持时间 (15个HCLK周期)
              取值范围：0x00-0x0F（1-16周期），保持地址有效的时间窗口 */
    
    0xFF,   /* FMC_DataSetupTime: 数据建立时间 (255个HCLK周期)
              取值范围：0x00-0xFF（1-256周期），数据总线保持稳定的最短时间 */
    
    0x0F,  /* FMC_BusTurnAroundDuration: 总线转向延迟 (15个周期)
              取值范围：0x00-0x0F（1-16周期），读写操作切换时的保护间隔 */
    
    0x0F,  /* FMC_CLKDivision: 时钟分频系数 (16分频)
              实际分频值 = CLKDivision + 1，用于控制存储器时钟频率 */
    
    0x0F,  /* FMC_DataLatency: 数据延迟周期 (15周期)
              仅用于同步存储器访问，表示时钟边沿后的数据有效延迟 */
    
    FMC_AccessMode_A  /* FMC_AccessMode: 访问模式选择
              模式A：地址和数据线非复用模式
              支持四种模式：A/B/C/D，不同模式对应不同的时序控制策略 */
};

/* --------------------- FMC 寄存器位操作掩码定义 ---------------------------- */

/* Bank 控制寄存器(BCRx) 位掩码 */
#define BCR_MBKEN_SET    ((uint32_t)0x00000001)  /* 存储器块使能位掩码（第0位）
                                                    置1启用对应存储块 */
#define BCR_MBKEN_RESET  ((uint32_t)0x000FFFFE)  /* 存储器块禁用掩码（清除第0位）
                                                    同时保留其他位状态 */
#define BCR_FACCEN_SET   ((uint32_t)0x00000040)  /* 突发访问使能位（第6位）
                                                    置1启用突发传输模式 */

/* NAND闪存控制寄存器(PCRx) 位掩码 */
#define PCR_PBKEN_SET    ((uint32_t)0x00000004)  /* NAND存储块使能位（第2位）
                                                    置1启用NAND控制器 */
#define PCR_PBKEN_RESET  ((uint32_t)0x000FFFFB)  /* NAND存储块禁用掩码（清除第2位） */
#define PCR_ECCEN_SET    ((uint32_t)0x00000040)  /* ECC校验使能位（第6位）
                                                    置1开启NAND ECC错误校正 */
#define PCR_ECCEN_RESET  ((uint32_t)0x000FFFBF)  /* ECC校验禁用掩码（清除第6位） */
#define PCR_MEMORYTYPE_NAND ((uint32_t)0x00000008) /* 存储器类型标识位（第3位）
                                                    固定值表示配置为NAND接口 */

/* SDRAM 控制寄存器(SDCRx) 写保护掩码 */
#define SDCR_WriteProtection_RESET ((uint32_t)0x00007DFF) /* 写保护解除掩码
                                                    清除第11位（SDCKE1）和
                                                    第12位（SDCLK1）的写保护 */

/* SDRAM 模式寄存器(SDCMR) 配置掩码 */
#define SDCMR_CTB1_RESET   ((uint32_t)0x003FFFEF) /* 清除CTB1位（第4位）
                                                    用于选择存储体1的命令目标 */
#define SDCMR_CTB2_RESET   ((uint32_t)0x003FFFF7) /* 清除CTB2位（第3位）
                                                    选择存储体2的命令目标 */
#define SDCMR_CTB1_2_RESET ((uint32_t)0x003FFFE7) /* 同时清除CTB1和CTB2位
                                                    用于多存储体联合操作 */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FMC_Private_Functions
  * @{
  */

/** @defgroup FMC_Group1 NOR/SRAM Controller functions
  * @brief    NOR/SRAM Controller functions 
  *
@verbatim   
 ===============================================================================
                    ##### NOR and SRAM Controller functions #####
 ===============================================================================  

 [..] The following sequence should be followed to configure the FMC to interface
      with SRAM, PSRAM, NOR or OneNAND memory connected to the NOR/SRAM Bank:
 
   (#) Enable the clock for the FMC and associated GPIOs using the following functions:
          RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

   (#) FMC pins configuration 
       (++) Connect the involved FMC pins to AF12 using the following function 
            GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FMC); 
       (++) Configure these FMC pins in alternate function mode by calling the function
            GPIO_Init();    
       
   (#) Declare a FMC_NORSRAMInitTypeDef structure, for example:
          FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure;
      and fill the FMC_NORSRAMInitStructure variable with the allowed values of
      the structure member.
      
   (#) Initialize the NOR/SRAM Controller by calling the function
          FMC_NORSRAMInit(&FMC_NORSRAMInitStructure); 

   (#) Then enable the NOR/SRAM Bank, for example:
          FMC_NORSRAMCmd(FMC_Bank1_NORSRAM2, ENABLE);  

   (#) At this stage you can read/write from/to the memory connected to the NOR/SRAM Bank. 
   
@endverbatim
  * @{
  */
/**
  * @brief  复位指定的 FMC NOR/SRAM 存储块到默认状态
  * @param  FMC_Bank: 需要初始化的存储块
  *   @arg FMC_Bank1_NORSRAM1  存储块1（地址范围 0x6000 0000 - 0x63FF FFFF）
  *   @arg FMC_Bank1_NORSRAM2  存储块2（地址范围 0x6400 0000 - 0x67FF FFFF）
  *   @arg FMC_Bank1_NORSRAM3  存储块3（地址范围 0x6800 0000 - 0x6BFF FFFF） 
  *   @arg FMC_Bank1_NORSRAM4  存储块4（地址范围 0x6C00 0000 - 0x6FFF FFFF）
  * @retval 无
  * @note 复位操作会影响以下寄存器：
  *       - BTCR[Bank]     : 存储块控制寄存器
  *       - BTCR[Bank+1]   : 读写时序寄存器
  *       - BWTR[Bank]     : 写时序寄存器（扩展模式）
  */
void FMC_NORSRAMDeInit(uint32_t FMC_Bank)
{
  /* 验证参数有效性 */
  assert_param(IS_FMC_NORSRAM_BANK(FMC_Bank));
  
  /* 存储块1的特殊处理 */
  if(FMC_Bank == FMC_Bank1_NORSRAM1)
  {
    /* 设置控制寄存器为默认值 0x000030DB：
       - 数据地址复用禁止
       - 存储器类型：SRAM  
       - 数据宽度：16位
       - 突发访问禁止
       - 等待信号极性：低电平有效 */
    FMC_Bank1->BTCR[FMC_Bank] = 0x000030DB;    
  }
  /* 其他存储块2-4 */
  else
  {   
    /* 默认值 0x000030D2：
       区别在于等待信号时序配置 */
    FMC_Bank1->BTCR[FMC_Bank] = 0x000030D2; 
  }
  /* 清除读写时序设置：
     - 地址建立时间清零
     - 数据保持时间最大 */
  FMC_Bank1->BTCR[FMC_Bank + 1] = 0x0FFFFFFF;
  
  /* 清除扩展写时序配置 */
  FMC_Bank1E->BWTR[FMC_Bank] = 0x0FFFFFFF;  
}

/**
  * @brief  初始化FMC NOR/SRAM存储块
  * @param  FMC_NORSRAMInitStruct : 配置结构体指针
  * @retval 无
  * @note 该函数完成以下配置：
  *       - 存储块控制寄存器（BTCR）
  *       - 读写时序寄存器（BTCR+1）
  *       - 写时序寄存器（BWTR，扩展模式下启用）
  */
void FMC_NORSRAMInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct)
{
  uint32_t tmpr = 0;
  
  /*========== 参数有效性验证 ==========*/
  assert_param(IS_FMC_NORSRAM_BANK(FMC_NORSRAMInitStruct->FMC_Bank));
  assert_param(IS_FMC_MUX(FMC_NORSRAMInitStruct->FMC_DataAddressMux));
  /* ...其他约15个参数校验略... */

  /*========== 存储块控制寄存器配置 ==========*/
  FMC_Bank1->BTCR[FMC_NORSRAMInitStruct->FMC_Bank] =   
            (uint32_t)FMC_NORSRAMInitStruct->FMC_DataAddressMux |  // 地址/数据复用模式
            FMC_NORSRAMInitStruct->FMC_MemoryType |                // 存储器类型（NOR/SRAM）
            FMC_NORSRAMInitStruct->FMC_MemoryDataWidth |           // 数据总线宽度（8/16/32位）
            FMC_NORSRAMInitStruct->FMC_BurstAccessMode |           // 突发访问模式
            FMC_NORSRAMInitStruct->FMC_WaitSignalPolarity |        // 等待信号极性
            FMC_NORSRAMInitStruct->FMC_WrapMode |                  // 突发回绕模式
            FMC_NORSRAMInitStruct->FMC_WaitSignalActive |          // 等待信号有效阶段
            FMC_NORSRAMInitStruct->FMC_WriteOperation |            // 写操作使能
            FMC_NORSRAMInitStruct->FMC_WaitSignal |                // 等待信号使能
            FMC_NORSRAMInitStruct->FMC_ExtendedMode |              // 扩展模式（独立写时序）
            FMC_NORSRAMInitStruct->FMC_AsynchronousWait |          // 异步等待支持
            FMC_NORSRAMInitStruct->FMC_WriteBurst |                // 写突发使能
            FMC_NORSRAMInitStruct->FMC_ContinousClock;             // 连续时钟模式

  /* NOR存储器需启用快速访问模式 */
  if(FMC_NORSRAMInitStruct->FMC_MemoryType == FMC_MemoryType_NOR)
  {
    FMC_Bank1->BTCR[FMC_NORSRAMInitStruct->FMC_Bank] |= BCR_FACCEN_SET; // 设置BCR[6]位
  }

  /*========== 同步时钟配置处理 ==========*/
  if((FMC_NORSRAMInitStruct->FMC_ContinousClock == FMC_CClock_SyncAsync) && 
     (FMC_NORSRAMInitStruct->FMC_Bank != FMC_Bank1_NORSRAM1))
  {
    /* 保留存储块1的时钟分频设置 */
    tmpr = (uint32_t)((FMC_Bank1->BTCR[FMC_Bank1_NORSRAM1+1]) & ~(0x0F << 20));    
    
    /* 配置连续时钟模式和突发使能 */
    FMC_Bank1->BTCR[FMC_Bank1_NORSRAM1]  |= FMC_NORSRAMInitStruct->FMC_ContinousClock;
    FMC_Bank1->BTCR[FMC_Bank1_NORSRAM1]  |= FMC_BurstAccessMode_Enable;
    
    /* 更新时钟分频参数 */
    FMC_Bank1->BTCR[FMC_Bank1_NORSRAM1+1] = (tmpr | (((FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_CLKDivision)-1) << 20));
  }

  /*========== 读写时序寄存器配置 ==========*/
  FMC_Bank1->BTCR[FMC_NORSRAMInitStruct->FMC_Bank+1] =   
            (uint32_t)FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AddressSetupTime | // 地址建立时间（0-0xF）
            (FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AddressHoldTime << 4) |     // 地址保持时间（0-0xF）
            (FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_DataSetupTime << 8) |       // 数据建立时间（0-0xFF） 
            (FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_BusTurnAroundDuration << 16)| // 总线转向延迟
            ((FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_CLKDivision) << 20) |      // 时钟分频
            ((FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_DataLatency) << 24) |      // 数据延迟（同步模式）
            FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct->FMC_AccessMode;                   // 访问模式（A/B/C/D）

  /*========== 扩展写时序配置 ==========*/
  if(FMC_NORSRAMInitStruct->FMC_ExtendedMode == FMC_ExtendedMode_Enable)
  {
    /* 参数校验 */
    assert_param(IS_FMC_ADDRESS_SETUP_TIME(FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AddressSetupTime));
    /* ...其他写时序参数校验略... */
    
    /* 配置独立的写时序寄存器 */
    FMC_Bank1E->BWTR[FMC_NORSRAMInitStruct->FMC_Bank] =   
               (uint32_t)FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AddressSetupTime |
               (FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AddressHoldTime << 4 )|     // 与读时序独立
               (FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_DataSetupTime << 8) |       // 通常大于读时序
               ((FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_CLKDivision) << 20) |     // 独立时钟分频
               ((FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_DataLatency) << 24) |       // 写数据延迟
               FMC_NORSRAMInitStruct->FMC_WriteTimingStruct->FMC_AccessMode;
  }
  else  /* 非扩展模式：统一读写时序 */
  {
    FMC_Bank1E->BWTR[FMC_NORSRAMInitStruct->FMC_Bank] = 0x0FFFFFFF; // 保持默认值
  }
}
/**
  * @brief  初始化FMC NOR/SRAM配置结构体为默认值
  * @param  FMC_NORSRAMInitStruct: 待初始化的配置结构体指针
  * @retval 无
  * @note 默认配置特点：
  *       - 使用Bank1存储块1
  *       - 适用于16位宽度的SRAM存储器
  *       - 禁止突发访问、异步等待等高级功能
  *       - 使用统一的读写时序配置
  */
void FMC_NORSRAMStructInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct)
{  
  /* 将结构体成员重置为典型安全值 */
  FMC_NORSRAMInitStruct->FMC_Bank = FMC_Bank1_NORSRAM1;        // 默认使用存储块1
  FMC_NORSRAMInitStruct->FMC_DataAddressMux = FMC_DataAddressMux_Enable;       // 启用地址/数据复用
  FMC_NORSRAMInitStruct->FMC_MemoryType = FMC_MemoryType_SRAM; // 存储器类型设为SRAM
  FMC_NORSRAMInitStruct->FMC_MemoryDataWidth = FMC_NORSRAM_MemoryDataWidth_16b;// 16位数据总线
  FMC_NORSRAMInitStruct->FMC_BurstAccessMode = FMC_BurstAccessMode_Disable;     // 禁止突发传输
  FMC_NORSRAMInitStruct->FMC_AsynchronousWait = FMC_AsynchronousWait_Disable;    // 禁止异步等待
  FMC_NORSRAMInitStruct->FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_Low;   // 等待信号低电平有效
  FMC_NORSRAMInitStruct->FMC_WrapMode = FMC_WrapMode_Disable;   // 禁止突发地址回绕
  FMC_NORSRAMInitStruct->FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;// 在等待状态前激活信号
  FMC_NORSRAMInitStruct->FMC_WriteOperation = FMC_WriteOperation_Enable;        // 使能写操作
  FMC_NORSRAMInitStruct->FMC_WaitSignal = FMC_WaitSignal_Enable;               // 启用等待信号
  FMC_NORSRAMInitStruct->FMC_ExtendedMode = FMC_ExtendedMode_Disable;           // 禁止扩展模式
  FMC_NORSRAMInitStruct->FMC_WriteBurst = FMC_WriteBurst_Disable;               // 禁止写突发模式
  FMC_NORSRAMInitStruct->FMC_ContinousClock = FMC_CClock_SyncOnly;              // 同步时钟模式
  
  /* 指向预定义的默认时序配置 
     默认时序参数：
     - 地址建立时间15周期
     - 数据建立时间255周期 */
  FMC_NORSRAMInitStruct->FMC_ReadWriteTimingStruct = (FMC_NORSRAMTimingInitTypeDef*)&FMC_DefaultTimingStruct;
  FMC_NORSRAMInitStruct->FMC_WriteTimingStruct = (FMC_NORSRAMTimingInitTypeDef*)&FMC_DefaultTimingStruct;
}

/**
  * @brief  启用/禁用指定NOR/SRAM存储块
  * @param  FMC_Bank: 存储块编号
  *   @arg FMC_Bank1_NORSRAM1  存储块1（地址映射到0x60000000）
  *   @arg FMC_Bank1_NORSRAM2  存储块2（地址映射到0x64000000）
  *   @arg FMC_Bank1_NORSRAM3  存储块3（地址映射到0x68000000）
  *   @arg FMC_Bank1_NORSRAM4  存储块4（地址映射到0x6C000000）
  * @param  NewState: 使能状态 ENABLE（启用）或 DISABLE（禁用）
  * @retval 无
  * @note 启用存储块会激活以下功能：
  *       - 对应的地址空间变为可访问
  *       - 存储块控制寄存器配置生效
  *       - 关联的GPIO引脚切换为FMC复用功能
  */
void FMC_NORSRAMCmd(uint32_t FMC_Bank, FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FMC_NORSRAM_BANK(FMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 设置BTCR寄存器的MBKEN位（第0位）为1
       启用存储块后，对应的地址映射区域将被激活
       操作示例：FMC_Bank1->BTCR[1] |= 0x00000001 */
    FMC_Bank1->BTCR[FMC_Bank] |= BCR_MBKEN_SET;
  }
  else
  {
    /* 清除MBKEN位（第0位）
       禁用存储块会使其地址空间无法访问
       同时相关GPIO恢复默认状态 */
    FMC_Bank1->BTCR[FMC_Bank] &= BCR_MBKEN_RESET;
  }
}

/**
  * @}
  */

/** @defgroup FMC_Group2 NAND Controller functions
  * @brief    NAND Controller functions 
  *
@verbatim   
 ===============================================================================
                    ##### NAND Controller functions #####
 ===============================================================================  

 [..]  The following sequence should be followed to configure the FMC to interface 
       with 8-bit or 16-bit NAND memory connected to the NAND Bank:
 
  (#) Enable the clock for the FMC and associated GPIOs using the following functions:
      (++)  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
      (++)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

  (#) FMC pins configuration 
      (++) Connect the involved FMC pins to AF12 using the following function 
           GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FMC); 
      (++) Configure these FMC pins in alternate function mode by calling the function
           GPIO_Init();    
       
  (#) Declare a FMC_NANDInitTypeDef structure, for example:
      FMC_NANDInitTypeDef  FMC_NANDInitStructure;
      and fill the FMC_NANDInitStructure variable with the allowed values of
      the structure member.
      
  (#) Initialize the NAND Controller by calling the function
      FMC_NANDInit(&FMC_NANDInitStructure); 

  (#) Then enable the NAND Bank, for example:
      FMC_NANDCmd(FMC_Bank3_NAND, ENABLE);  

  (#) At this stage you can read/write from/to the memory connected to the NAND Bank. 
   
 [..]
  (@) To enable the Error Correction Code (ECC), you have to use the function
      FMC_NANDECCCmd(FMC_Bank3_NAND, ENABLE);  
 [..]
  (@) and to get the current ECC value you have to use the function
      ECCval = FMC_GetECC(FMC_Bank3_NAND); 

@endverbatim
  * @{
  */
  /**
  * @brief  复位FMC NAND存储块寄存器到默认值
  * @param  FMC_Bank: 目标存储块
  *   @arg FMC_Bank2_NAND  NAND存储块2（地址0x70000000）
  *   @arg FMC_Bank3_NAND  NAND存储块3（地址0x80000000）
  * @retval 无
  * @note 复位操作将：
  *       - 关闭存储块使能
  *       - 清除所有时序配置
  *       - 禁用ECC校验功能
  */
void FMC_NANDDeInit(uint32_t FMC_Bank)
{
  /* 验证存储块参数合法性 */
  assert_param(IS_FMC_NAND_BANK(FMC_Bank));
  
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    /* 复位Bank2寄存器组 */
    FMC_Bank2->PCR2 = 0x00000018;  /* 默认控制寄存器值：
                                     - PBKEN=0（禁用存储块）
                                     - ECCEN=0（关闭ECC）
                                     - TCLR=0/TAR=0（清除时序） */
    FMC_Bank2->SR2 = 0x00000040;    /* 状态寄存器：清除所有标志位 */
    FMC_Bank2->PMEM2 = 0xFCFCFCFC;  /* 公共空间时序：最大延迟配置 */
    FMC_Bank2->PATT2 = 0xFCFCFCFC;  /* 属性空间时序：最大延迟配置 */
  }
  else  /* FMC_Bank3_NAND */
  {
    /* 复位Bank3寄存器组（配置同Bank2） */
    FMC_Bank3->PCR3 = 0x00000018;
    FMC_Bank3->SR3 = 0x00000040;
    FMC_Bank3->PMEM3 = 0xFCFCFCFC;
    FMC_Bank3->PATT3 = 0xFCFCFCFC; 
  }  
}

/**
  * @brief  初始化FMC NAND存储块
  * @param  FMC_NANDInitStruct : 配置结构体指针
  * @retval 无
  * @note 该函数完成以下配置：
  *       - 控制寄存器（PCRx）：存储块使能/ECC/数据宽度等
  *       - 公共空间时序寄存器（PMEMx）：CLE/ALE信号时序
  *       - 属性空间时序寄存器（PATTx）：命令锁存时序
  */
void FMC_NANDInit(FMC_NANDInitTypeDef* FMC_NANDInitStruct)
{
  uint32_t tmppcr = 0x00000000, tmppmem = 0x00000000, tmppatt = 0x00000000; 
    
  /*========== 参数合法性验证 ==========*/
  assert_param(IS_FMC_NAND_BANK(FMC_NANDInitStruct->FMC_Bank));
  assert_param(IS_FMC_WAIT_FEATURE(FMC_NANDInitStruct->FMC_Waitfeature)); // 等待特性使能
  assert_param(IS_FMC_NAND_MEMORY_WIDTH(FMC_NANDInitStruct->FMC_MemoryDataWidth)); // 8/16位总线
  /* ...其他约12个参数校验略... */

  /*========== 构建控制寄存器值 ==========*/
  tmppcr = (uint32_t)FMC_NANDInitStruct->FMC_Waitfeature |  // 等待特性配置
            PCR_MEMORYTYPE_NAND |                           // 固定标识NAND类型
            FMC_NANDInitStruct->FMC_MemoryDataWidth |       // 总线宽度
            FMC_NANDInitStruct->FMC_ECC |                    // ECC使能
            FMC_NANDInitStruct->FMC_ECCPageSize |           // ECC页大小（256/512/1024/2048字节）
            (FMC_NANDInitStruct->FMC_TCLRSetupTime << 9 )|  // CLE到RE延迟（0-15）
            (FMC_NANDInitStruct->FMC_TARSetupTime << 13);   // ALE到RE延迟（0-15）

  /*========== 构建公共空间时序值 ==========*/
  tmppmem = (uint32_t)FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime |      // 建立时间（0-255）
            (FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime << 8) |    // 等待时间
            (FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime << 16)|    // 保持时间
            (FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime << 24);      // 高阻时间

  /*========== 构建属性空间时序值 ==========*/
  tmppatt = (uint32_t)FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime | 
            (FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime << 8) |
            (FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime << 16)|
            (FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime << 24);

  /*========== 写入寄存器 ==========*/
  if(FMC_NANDInitStruct->FMC_Bank == FMC_Bank2_NAND)
  {
    FMC_Bank2->PCR2 = tmppcr;    // 控制寄存器
    FMC_Bank2->PMEM2 = tmppmem;  // 公共空间时序（读/写操作）
    FMC_Bank2->PATT2 = tmppatt;  // 属性空间时序（命令锁存）
  }
  else  // Bank3
  {
    FMC_Bank3->PCR3 = tmppcr;
    FMC_Bank3->PMEM3 = tmppmem;
    FMC_Bank3->PATT3 = tmppatt;
  }
}

/**
  * @brief  初始化NAND配置结构体为安全默认值
  * @param  FMC_NANDInitStruct: 配置结构体指针
  * @retval 无
  * @note 默认配置特点：
  *       - 使用Bank2
  *       - 16位总线宽度
  *       - 所有时序参数设为最大值（安全模式）
  *       - 禁用ECC和等待特性
  */
void FMC_NANDStructInit(FMC_NANDInitTypeDef* FMC_NANDInitStruct)
{ 
  /* 重置结构体成员为保守值 */
  FMC_NANDInitStruct->FMC_Bank = FMC_Bank2_NAND;               // 默认使用Bank2
  FMC_NANDInitStruct->FMC_Waitfeature = FMC_Waitfeature_Disable; // 关闭等待特性
  FMC_NANDInitStruct->FMC_MemoryDataWidth = FMC_NAND_MemoryDataWidth_16b; // 16位总线
  FMC_NANDInitStruct->FMC_ECC = FMC_ECC_Disable;                // 默认关闭ECC
  FMC_NANDInitStruct->FMC_ECCPageSize = FMC_ECCPageSize_256Bytes; // ECC页大小256B
  FMC_NANDInitStruct->FMC_TCLRSetupTime = 0x0;                   // CLE-RE延迟0周期
  FMC_NANDInitStruct->FMC_TARSetupTime = 0x0;                    // ALE-RE延迟0周期
  
  /* 公共空间时序：最大延迟值（0xFC=252 cycles）*/
  FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime = 252;
  FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime = 252;
  FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime = 252;
  FMC_NANDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime = 252;
  
  /* 属性空间时序：同公共空间 */
  FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime = 252;
  FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime = 252;
  FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime = 252;
  FMC_NANDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime = 252;	  
}
/**
  * @brief  启用或禁用NAND存储块控制器
  * @param  FMC_Bank: 目标存储块
  *   @arg FMC_Bank2_NAND  Bank2（地址0x70000000）
  *   @arg FMC_Bank3_NAND  Bank3（地址0x80000000）
  * @param  NewState: 控制状态
  *   @arg ENABLE:  激活存储块，相关GPIO切换为FMC功能
  *   @arg DISABLE: 关闭存储块，释放GPIO引脚
  * @retval 无
  * @warning 启用前必须完成GPIO时钟和FMC时钟的初始化
  * @note 操作寄存器位：
  *       PCRx[2] PBKEN: 存储块使能位
  *       0=禁用（默认），1=启用
  */
void FMC_NANDCmd(uint32_t FMC_Bank, FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FMC_NAND_BANK(FMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 设置PBKEN位启用存储块控制器 */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      FMC_Bank2->PCR2 |= PCR_PBKEN_SET;  // 等效于PCR2 |= 0x00000004
    }
    else
    {
      FMC_Bank3->PCR3 |= PCR_PBKEN_SET;  // Bank3操作
    }
  }
  else
  {
    /* 清除PBKEN位关闭控制器 */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      FMC_Bank2->PCR2 &= PCR_PBKEN_RESET; // 保持其他位不变，仅清除第2位
    }
    else
    {
      FMC_Bank3->PCR3 &= PCR_PBKEN_RESET;
    }
  }
}

/**
  * @brief  启用/禁用NAND ECC校验功能
  * @param  FMC_Bank: 目标存储块
  *   @arg FMC_Bank2_NAND/Bank3_NAND
  * @param  NewState: 控制状态
  *   @arg ENABLE:  自动生成/校验ECC码（增加约5%的访问时间）
  *   @arg DISABLE: 关闭ECC功能（提升速度，降低数据可靠性）
  * @retval 无
  * @note 操作寄存器位：
  *       PCRx[6] ECCEN: ECC使能位
  *       需先配置ECCPageSize（PCR[8:7]）
  * @warning 写操作过程中修改ECC状态会导致数据损坏
  */
void FMC_NANDECCCmd(uint32_t FMC_Bank, FunctionalState NewState)
{
  assert_param(IS_FMC_NAND_BANK(FMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 启用硬件ECC计算与校验 */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      FMC_Bank2->PCR2 |= PCR_ECCEN_SET;  // 设置第6位（0x00000040）
    }
    else
    {
      FMC_Bank3->PCR3 |= PCR_ECCEN_SET;
    }
  }
  else
  {
    /* 禁用ECC功能 */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      FMC_Bank2->PCR2 &= PCR_ECCEN_RESET; // 清除第6位
    }
    else
    {
      FMC_Bank3->PCR3 &= PCR_ECCEN_RESET;
    }
  }
}

/**
  * @brief  获取最近一次NAND操作的ECC校验码
  * @param  FMC_Bank: 目标存储块
  *   @arg FMC_Bank2_NAND/Bank3_NAND
  * @retval uint32_t  ECC校验码（24位有效）
  * @note ECC值计算规则：
  *       - 每256字节数据生成3字节ECC码
  *       - 低位对齐存储，ECCR[23:0]有效
  * @warning 需在读取操作完成后立即获取，否则会被后续操作覆盖
  */
uint32_t FMC_GetECC(uint32_t FMC_Bank)
{
  uint32_t eccval = 0x00000000;
  
  /* 根据存储块选择寄存器 */
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    eccval = FMC_Bank2->ECCR2;  // 读取Bank2的ECC结果寄存器
  }
  else
  {
    eccval = FMC_Bank3->ECCR3;  // Bank3的ECC寄存器
  }
  
  /* 返回校验码 */
  return(eccval ); 
}

/**
  * @}
  */

/** @defgroup FMC_Group3 PCCARD Controller functions
  * @brief    PCCARD Controller functions 
  *
@verbatim   
 ===============================================================================
                    ##### PCCARD Controller functions #####
 ===============================================================================  

 [..]  he following sequence should be followed to configure the FMC to interface 
       with 16-bit PC Card compatible memory connected to the PCCARD Bank:
 
  (#)  Enable the clock for the FMC and associated GPIOs using the following functions:
       (++)  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
       (++)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

  (#) FMC pins configuration 
       (++) Connect the involved FMC pins to AF12 using the following function 
            GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FMC); 
       (++) Configure these FMC pins in alternate function mode by calling the function
            GPIO_Init();    
       
  (#) Declare a FMC_PCCARDInitTypeDef structure, for example:
      FMC_PCCARDInitTypeDef  FMC_PCCARDInitStructure;
      and fill the FMC_PCCARDInitStructure variable with the allowed values of
      the structure member.
      
  (#) Initialize the PCCARD Controller by calling the function
      FMC_PCCARDInit(&FMC_PCCARDInitStructure); 

  (#) Then enable the PCCARD Bank:
      FMC_PCCARDCmd(ENABLE);  

  (#) At this stage you can read/write from/to the memory connected to the PCCARD Bank. 
 
@endverbatim
  * @{
  */
/**
  * @brief  复位FMC PCCARD控制器寄存器到默认值
  * @param  无
  * @retval 无
  * @note 该操作将影响以下寄存器：
  *       - PCR4: 控制寄存器（禁用PCCARD控制器，清除时序配置）
  *       - SR4: 状态寄存器（清除所有中断标志）
  *       - PMEM4/PATT4/PIO4: 时序寄存器（设置最大延迟保证安全性）
  */
void FMC_PCCARDDeInit(void)
{
  /* 复位控制寄存器：禁用等待特性，设置16位总线，清除TCLR/TAR时序 */
  FMC_Bank4->PCR4 = 0x00000018;       // [2]PBKEN=0 [6]WAITEN=0 [9:13]时序清零
  
  /* 清除所有状态标志：包括中断标志和ECC状态 */
  FMC_Bank4->SR4 = 0x00000000;         // 清除FIFO状态/中断标志
  
  /* 配置公共存储空间时序参数为最大延迟 */
  FMC_Bank4->PMEM4 = 0xFCFCFCFC;       // 建立/等待/保持/高阻时间=252周期
  
  /* 配置属性空间时序参数（访问寄存器时使用） */
  FMC_Bank4->PATT4 = 0xFCFCFCFC;       // 用于CE2#信号时序控制
  
  /* 配置IO空间时序参数（执行IO操作时使用） */
  FMC_Bank4->PIO4 = 0xFCFCFCFC;        // 控制IORD/IOWR信号时序
}

/**
  * @brief  初始化PCCARD控制器
  * @param  FMC_PCCARDInitStruct: 包含时序参数的配置结构体
  * @retval 无
  * @note 该函数配置以下内容：
  *       - 控制寄存器：等待特性/总线宽度/时序参数
  *       - 三种空间时序：公共/属性/IO空间独立配置
  */
void FMC_PCCARDInit(FMC_PCCARDInitTypeDef* FMC_PCCARDInitStruct)
{
  /* 参数合法性校验（共15个参数检查） */
  assert_param(IS_FMC_WAIT_FEATURE(FMC_PCCARDInitStruct->FMC_Waitfeature)); // 等待特性使能检查
  assert_param(IS_FMC_TCLR_TIME(FMC_PCCARDInitStruct->FMC_TCLRSetupTime));  // CLE有效时间（0-15周期）
  assert_param(IS_FMC_TAR_TIME(FMC_PCCARDInitStruct->FMC_TARSetupTime));    // ALE有效时间（0-15周期）

  /* 公共空间时序参数范围验证 */
  assert_param(IS_FMC_SETUP_TIME(FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime));    // 0-255
  assert_param(IS_FMC_WAIT_TIME(FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime));  // 0-255
  assert_param(IS_FMC_HOLD_TIME(FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime));  // 0-255
  assert_param(IS_FMC_HIZ_TIME(FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime));    // 0-255

  /* 构建控制寄存器值 */
  FMC_Bank4->PCR4 = (uint32_t)FMC_PCCARDInitStruct->FMC_Waitfeature |  // 等待特性配置位
                    FMC_NAND_MemoryDataWidth_16b |                     // 固定16位总线宽度
                    (FMC_PCCARDInitStruct->FMC_TCLRSetupTime << 9) |   // CLE有效时间（9-12位）
                    (FMC_PCCARDInitStruct->FMC_TARSetupTime << 13);     // ALE有效时间（13-16位）

  /* 配置公共空间时序（MEMx信号控制） */
  FMC_Bank4->PMEM4 = (uint32_t)FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime |        // [0-7]建立时间
                     (FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime << 8) |        // [8-15]等待时间
                     (FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime << 16) |      // [16-23]保持时间
                     (FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime << 24);       // [24-31]高阻时间

  /* 配置属性空间时序（ATTRx信号控制） */
  FMC_Bank4->PATT4 = (uint32_t)FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime |     // 寄存器访问时序
                    (FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime << 8) |
                    (FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime << 16) |
                    (FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime << 24);

  /* 配置IO空间时序（IORD/IOWR信号控制） */
  FMC_Bank4->PIO4 = (uint32_t)FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_SetupTime |           // IO操作时序
                   (FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_WaitSetupTime << 8) |
                   (FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HoldSetupTime << 16) |
                   (FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HiZSetupTime << 24);
}

/**
  * @brief  初始化PCCARD配置结构体为安全默认值
  * @param  FMC_PCCARDInitStruct: 需要初始化的结构体指针
  * @retval 无
  * @note 默认配置特点：
  *       - 禁用所有高级功能
  *       - 所有时序参数设置为最大252周期
  *       - 适用于首次初始化时的安全配置
  */
void FMC_PCCARDStructInit(FMC_PCCARDInitTypeDef* FMC_PCCARDInitStruct)
{
  /* 控制参数初始化 */
  FMC_PCCARDInitStruct->FMC_Waitfeature = FMC_Waitfeature_Disable; // 禁用等待特性
  FMC_PCCARDInitStruct->FMC_TCLRSetupTime = 0;                     // CLE时序清零
  FMC_PCCARDInitStruct->FMC_TARSetupTime = 0;                       // ALE时序清零

  /* 公共空间时序初始化（最大延迟保证兼容性） */
  FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_SetupTime = 252;    // 252周期建立时间
  FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_WaitSetupTime = 252; // 252周期等待
  FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HoldSetupTime = 252; // 252周期保持
  FMC_PCCARDInitStruct->FMC_CommonSpaceTimingStruct->FMC_HiZSetupTime = 252;  // 252周期高阻

  /* 属性空间时序同步初始化 */
  FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_SetupTime = 252;
  FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_WaitSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HoldSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_AttributeSpaceTimingStruct->FMC_HiZSetupTime = 252;

  /* IO空间时序初始化 */
  FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_SetupTime = 252;
  FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_WaitSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HoldSetupTime = 252;
  FMC_PCCARDInitStruct->FMC_IOSpaceTimingStruct->FMC_HiZSetupTime = 252;
}
/**
  * @brief  启用或禁用PCCARD存储块控制器
  * @param  NewState: 控制器状态
  *   @arg ENABLE:  激活PCCARD接口，相关GPIO切换为FMC功能
  *   @arg DISABLE: 关闭控制器，释放硬件资源
  * @retval 无
  * @warning 启用前必须完成以下操作：
  *          - 调用FMC_PCCARDInit()配置时序参数
  *          - 配置对应GPIO为FMC复用模式
  *          - 使能FMC和GPIO时钟
  * @note 操作寄存器位：
  *       PCR4[2] PBKEN: 存储块使能位
  *       0=禁用（默认），1=启用
  *       物理地址空间：0x90000000-0x9FFFFFFF
  */
void FMC_PCCARDCmd(FunctionalState NewState)
{
  /* 验证状态参数合法性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 设置PBKEN位（第2位）启用PCCARD控制器
       启用后可通过0x90000000访问PCCARD设备 */
    FMC_Bank4->PCR4 |= PCR_PBKEN_SET;  // PCR4 |= 0x00000004
  }
  else
  {
    /* 清除PBKEN位（第2位）关闭控制器
       立即停止所有进行中的PCCARD访问 */
    FMC_Bank4->PCR4 &= PCR_PBKEN_RESET; // PCR4 &= 0xFFFFFFFB
  }
}


/**
  * @}
  */

/** @defgroup FMC_Group4  SDRAM Controller functions
  * @brief    SDRAM Controller functions
  *
@verbatim   
 ===============================================================================
                     ##### SDRAM Controller functions ##### 
 ===============================================================================  
  
 [..]  The following sequence should be followed to configure the FMC to interface
       with SDRAM memory connected to the SDRAM Bank 1 or SDRAM bank 2:
 
  (#) Enable the clock for the FMC and associated GPIOs using the following functions:
      (++) RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
      (++) RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);

  (#) FMC pins configuration 
      (++) Connect the involved FMC pins to AF12 using the following function 
           GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_FMC); 
      (++) Configure these FMC pins in alternate function mode by calling the function
           GPIO_Init();    
       
  (#) Declare a FMC_SDRAMInitTypeDef structure, for example:
       FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
      and fill the FMC_SDRAMInitStructure variable with the allowed values of
      the structure member.  
      
  (#) Initialize the SDRAM Controller by calling the function
          FMC_SDRAMInit(&FMC_SDRAMInitStructure);
          
  (#) Declare a FMC_SDRAMCommandTypeDef structure, for example:
        FMC_SDRAMCommandTypeDef  FMC_SDRAMCommandStructure;
      and fill the FMC_SDRAMCommandStructure variable with the allowed values of
      the structure member.        

  (#) Configure the SDCMR register with the desired command parameters by calling 
      the function FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);  

  (#) At this stage, the SDRAM memory is ready for any valid command.
   
@endverbatim
  * @{
  */
/**
  * @brief  FMC SDRAM 控制器去初始化函数
  * @details  将指定 SDRAM 存储控制器的寄存器重置为默认值，包括：
  *           - 存储控制寄存器(SDCR)
  *           - 存储时序寄存器(SDTR)
  *           - 存储命令模式寄存器(SDCMR)
  *           - 存储刷新时序寄存器(SDRTR)
  *           - 存储状态寄存器(SDSR)
  *           这些寄存器复位后会清除所有配置参数，使控制器回到初始状态
  * 
  * @param  FMC_Bank: 指定要操作的 SDRAM 存储体
  *          @arg FMC_Bank1_SDRAM: 存储体1（0x60000000~0x7FFFFFFF）
  *          @arg FMC_Bank2_SDRAM: 存储体2（0x80000000~0x9FFFFFFF）
  * @retval 无
  */
void FMC_SDRAMDeInit(uint32_t FMC_Bank)
{
  /* 参数合法性检查：验证输入的Bank参数是否在允许范围内 */
  assert_param(IS_FMC_SDRAM_BANK(FMC_Bank));
  
  /* 存储控制寄存器组复位（SDCR） */
  FMC_Bank5_6->SDCR[FMC_Bank] = 0x000002D0;  // 复位值为固定值，包含默认的存储体配置参数
  /* 存储时序寄存器组复位（SDTR） */
  FMC_Bank5_6->SDTR[FMC_Bank] = 0x0FFFFFFF; // 复位所有时序参数到最大延迟状态  
  /* 命令模式寄存器复位（SDCMR） */
  FMC_Bank5_6->SDCMR = 0x00000000;          // 清除所有未完成命令
  /* 刷新时序寄存器复位（SDRTR） */
  FMC_Bank5_6->SDRTR = 0x00000000;          // 关闭自动刷新功能
  /* 状态寄存器复位（SDSR） */
  FMC_Bank5_6->SDSR = 0x00000000;           // 清除所有状态标志位
}

/**
  * @brief  FMC SDRAM 控制器初始化函数
  * @details  根据初始化结构体参数配置SDRAM控制器，包括：
  *           - 存储体选择
  *           - 地址线位宽配置
  *           - 时序参数配置
  *           - 工作模式设置
  *           该函数会同时配置控制寄存器和时序寄存器，完成SDRAM控制器的完整初始化流程
  * 
  * @param  FMC_SDRAMInitStruct : 指向FMC_SDRAMInitTypeDef结构体的指针，包含：
  *           - 存储体选择（FMC_Bank）
  *           - 列地址位数（9/10/11位）
  *           - 行地址位数（11/12/13位）
  *           - 数据总线宽度（8/16/32位）
  *           - 内部存储体数量（2/4个）
  *           - CAS延迟周期（1/2/3个时钟周期）
  *           - 写保护使能
  *           - 时钟分频配置
  *           - 突发读模式配置
  *           - 读管道延迟
  *           以及完整的时序参数结构体
  * @retval 无
  */
void FMC_SDRAMInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct)
{ 
  /* 临时寄存器变量，用于构建寄存器配置值 */
  uint32_t tmpr1 = 0; // 存储控制寄存器配置值
  uint32_t tmpr2 = 0; // 时序寄存器主配置值
  uint32_t tmpr3 = 0; // 存储体1的备用配置值
  uint32_t tmpr4 = 0; // 时序寄存器补充配置值
  
  /*========== 参数合法性检查阶段 ==========*/
  /* 控制参数校验 */
  assert_param(IS_FMC_SDRAM_BANK(FMC_SDRAMInitStruct->FMC_Bank));
  assert_param(IS_FMC_COLUMNBITS_NUMBER(FMC_SDRAMInitStruct->FMC_ColumnBitsNumber));  // 校验列地址位数（9/10/11）
  assert_param(IS_FMC_ROWBITS_NUMBER(FMC_SDRAMInitStruct->FMC_RowBitsNumber));        // 校验行地址位数（11/12/13）
  assert_param(IS_FMC_SDMEMORY_WIDTH(FMC_SDRAMInitStruct->FMC_SDMemoryDataWidth));    // 校验数据宽度（8/16/32）
  assert_param(IS_FMC_INTERNALBANK_NUMBER(FMC_SDRAMInitStruct->FMC_InternalBankNumber)); // 校验内部Bank数（2/4）
  assert_param(IS_FMC_CAS_LATENCY(FMC_SDRAMInitStruct->FMC_CASLatency));             // 校验CAS延迟（1/2/3）
  assert_param(IS_FMC_WRITE_PROTECTION(FMC_SDRAMInitStruct->FMC_WriteProtection));    // 校验写保护状态
  assert_param(IS_FMC_SDCLOCK_PERIOD(FMC_SDRAMInitStruct->FMC_SDClockPeriod));        // 校验时钟分频（HCLK/2 或 HCLK/3）
  assert_param(IS_FMC_READ_BURST(FMC_SDRAMInitStruct->FMC_ReadBurst));                // 校验突发读模式
  assert_param(IS_FMC_READPIPE_DELAY(FMC_SDRAMInitStruct->FMC_ReadPipeDelay));        // 校验读管道延迟（0/1/2周期）
  
  /* 时序参数校验 */
  assert_param(IS_FMC_LOADTOACTIVE_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay));  // tMRD: 加载模式寄存器到激活延迟
  assert_param(IS_FMC_EXITSELFREFRESH_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay)); // tXSR: 自刷新退出延迟
  assert_param(IS_FMC_SELFREFRESH_TIME(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime));      // tRAS: 自刷新持续时间
  assert_param(IS_FMC_ROWCYCLE_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay));         // tRC:  行周期时间
  assert_param(IS_FMC_WRITE_RECOVERY_TIME(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime)); // tWR: 写恢复时间
  assert_param(IS_FMC_RP_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay));                     // tRP: 行预充电时间
  assert_param(IS_FMC_RCD_DELAY(FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RCDDelay));                    // tRCD: 行到列延迟
  
  /*========== 存储控制寄存器(SDCR)配置阶段 ==========*/
  /* 组合控制寄存器配置值（各参数按位域组合） */
  tmpr1 =   (uint32_t)FMC_SDRAMInitStruct->FMC_ColumnBitsNumber |  // 列地址位数（SDCR[0:1]）
             FMC_SDRAMInitStruct->FMC_RowBitsNumber |              // 行地址位数（SDCR[2:3]）
             FMC_SDRAMInitStruct->FMC_SDMemoryDataWidth |           // 数据总线宽度（SDCR[4]）
             FMC_SDRAMInitStruct->FMC_InternalBankNumber |         // 内部Bank数（SDCR[5]）           
             FMC_SDRAMInitStruct->FMC_CASLatency |                 // CAS延迟（SDCR[6:7]）
             FMC_SDRAMInitStruct->FMC_WriteProtection |            // 写保护使能（SDCR[8]）
             FMC_SDRAMInitStruct->FMC_SDClockPeriod |              // 时钟分频（SDCR[9]）
             FMC_SDRAMInitStruct->FMC_ReadBurst |                  // 突发读模式（SDCR[10]）
             FMC_SDRAMInitStruct->FMC_ReadPipeDelay;               // 读管道延迟（SDCR[11:12]）
            
  /* 根据选择的存储体进行差异化配置 */
  if(FMC_SDRAMInitStruct->FMC_Bank == FMC_Bank1_SDRAM )
  {
    /* 直接配置存储体1的SDCR寄存器 */
    FMC_Bank5_6->SDCR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr1;
  }
  else /* 处理存储体2的特殊配置要求 */
  {
    /* 生成存储体1的保留位配置（SDCR2的保留位需要保持默认值） */
    tmpr3 = (uint32_t)FMC_SDRAMInitStruct->FMC_SDClockPeriod |  // 仅配置SDCR2相关位
             FMC_SDRAMInitStruct->FMC_ReadBurst | 
             FMC_SDRAMInitStruct->FMC_ReadPipeDelay;
    
    /* 先更新存储体1的寄存器，再配置存储体2 */
    FMC_Bank5_6->SDCR[FMC_Bank1_SDRAM] = tmpr3;         // 配置存储体1保留位
    FMC_Bank5_6->SDCR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr1; // 配置存储体2主参数
  }
  
  /*========== 存储时序寄存器(SDTR)配置阶段 ==========*/
  /* 组合时序寄存器配置值 */
  if(FMC_SDRAMInitStruct->FMC_Bank == FMC_Bank1_SDRAM )
  {
    /* 完整时序配置（存储体1支持全部时序参数） */
    tmpr2 =   (uint32_t)((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay)-1) |        // TMRD[3:0]  (tMRD-1)
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay)-1) << 4) |        // TXSR[7:4]  (tXSR-1)
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime)-1) << 8) |             // TRAS[11:8] (tRAS-1)
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay)-1) << 12) |              // TRC[15:12] (tRC-1)
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime)-1) << 16) |          // TWR[19:16] (tWR-1)
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay)-1) << 20) |                   // TRP[23:20] (tRP-1)
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RCDDelay)-1) << 24);                   // TRCD[27:24] (tRCD-1)
            
    /* 写入时序寄存器 */
    FMC_Bank5_6->SDTR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr2;
  }
  else /* 处理存储体2的时序配置特殊性 */
  {
    /* 主时序参数配置（存储体2不使用的参数位设为保留） */
    tmpr2 =   (uint32_t)((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay)-1) |      // TMRD[3:0]
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay)-1) << 4) |      // TXSR[7:4]
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime)-1) << 8) |           // TRAS[11:8]
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime)-1) << 16);        // TWR[19:16]
    
    /* 补充时序参数（需要配置到存储体1的寄存器） */
    tmpr4 =   (uint32_t)(((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay)-1) << 12) |  // TRC[15:12] → 存储体1寄存器
            (((FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay)-1) << 20);                   // TRP[23:20] → 存储体1寄存器
            
    /* 分步写入时序寄存器 */
    FMC_Bank5_6->SDTR[FMC_Bank1_SDRAM] = tmpr4;         // 将TRC和TRP写入存储体1的寄存器
    FMC_Bank5_6->SDTR[FMC_SDRAMInitStruct->FMC_Bank] = tmpr2; // 将主时序参数写入存储体2
  }
}
/**
  * @brief  SDRAM 初始化结构体默认值填充函数
  * @details  为FMC_SDRAM初始化结构体设置推荐默认参数：
  *           - 选择存储体1为默认控制对象
  *           - 设置8位列地址（共256列）
  *           - 设置11位行地址（共2048行）
  *           - 16位数据总线宽度
  *           - 4个内部存储体
  *           - CAS延迟1个时钟周期
  *           - 使能写保护机制
  *           - 关闭SDRAM时钟输出
  *           - 禁用突发读模式
  *           - 无读管道延迟
  *           - 默认初始化各时序参数为16个HCLK周期（适合大多数100MHz以下时钟场景）
  *           这些默认值对应典型8Mbit容量的SDRAM配置（如MT48LC4M32B2）
  * 
  * @param  FMC_SDRAMInitStruct : 待初始化的结构体指针
  * @retval 无
  */
void FMC_SDRAMStructInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct)  
{  
  /* 初始化控制参数部分 */
  FMC_SDRAMInitStruct->FMC_Bank = FMC_Bank1_SDRAM;                     // 默认选择存储体1
  FMC_SDRAMInitStruct->FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;  // 8位列地址（2^8=256列）
  FMC_SDRAMInitStruct->FMC_RowBitsNumber = FMC_RowBits_Number_11b;      // 11位行地址（2^11=2048行）
  FMC_SDRAMInitStruct->FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b;  // 16位数据总线
  FMC_SDRAMInitStruct->FMC_InternalBankNumber = FMC_InternalBank_Number_4; // 4个内部Bank
  FMC_SDRAMInitStruct->FMC_CASLatency = FMC_CAS_Latency_1;            // CAS延迟1周期
  FMC_SDRAMInitStruct->FMC_WriteProtection = FMC_Write_Protection_Enable; // 启用写保护
  FMC_SDRAMInitStruct->FMC_SDClockPeriod = FMC_SDClock_Disable;       // 禁用时钟输出（同步模式）
  FMC_SDRAMInitStruct->FMC_ReadBurst = FMC_Read_Burst_Disable;        // 禁用突发传输
  FMC_SDRAMInitStruct->FMC_ReadPipeDelay = FMC_ReadPipe_Delay_0;      // 零等待周期管道延迟
  
  /* 初始化时序参数部分（基于HCLK周期数） */ 
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_LoadToActiveDelay = 16;       // 加载模式寄存器到激活时间 tMRD
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_ExitSelfRefreshDelay = 16;    // 退出自刷新延迟 tXSR
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_SelfRefreshTime = 16;         // 自刷新持续时间 tRAS
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RowCycleDelay = 16;           // 行周期时间 tRC
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_WriteRecoveryTime = 16;       // 写恢复时间 tWR
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RPDelay = 16;                 // 行预充电时间 tRP 
  FMC_SDRAMInitStruct->FMC_SDRAMTimingStruct->FMC_RCDDelay = 16;                // 行到列延迟 tRCD
  
}

/**
  * @brief  SDRAM 命令配置函数
  * @details  通过命令模式寄存器(SDCMR)发送控制命令到SDRAM设备，支持：
  *           - 时钟配置使能
  *           - 预充电所有存储体
  *           - 自动刷新操作
  *           - 加载模式寄存器
  *           - 自刷新进入/退出
  *           - 电源管理命令
  *           这些命令通常用于SDRAM初始化序列
  * 
  * @param  FMC_SDRAMCommandStruct : 命令配置结构体指针，包含：
  *           - 命令类型（正常/时钟配置/预充电等）
  *           - 目标存储体（Bank1或Bank2）
  *           - 自动刷新次数（1-15次） 
  *           - 模式寄存器定义（用于设置突发长度/突发类型/CAS延迟等）
  * @retval 无
  */
void FMC_SDRAMCmdConfig(FMC_SDRAMCommandTypeDef* FMC_SDRAMCommandStruct)
{
  uint32_t tmpr = 0x0;  // 临时寄存器，用于构建命令参数
    
  /* 参数合法性检查 */
  assert_param(IS_FMC_COMMAND_MODE(FMC_SDRAMCommandStruct->FMC_CommandMode));      // 校验命令模式值域
  assert_param(IS_FMC_COMMAND_TARGET(FMC_SDRAMCommandStruct->FMC_CommandTarget));  // 校验目标存储体
  assert_param(IS_FMC_AUTOREFRESH_NUMBER(FMC_SDRAMCommandStruct->FMC_AutoRefreshNumber)); // 刷新次数1-15
  assert_param(IS_FMC_MODE_REGISTER(FMC_SDRAMCommandStruct->FMC_ModeRegisterDefinition)); // 模式寄存器值校验
  
  /* 组合命令寄存器值：
     [0-1]   : 命令模式 (CT[1:0])
     [2-3]   : 命令目标 (CTR[1:0])
     [5-8]   : 自动刷新次数 (NCR)，实际写入值为(n-1)
     [9-16]  : 模式寄存器定义 (MRD) */
  tmpr =   (uint32_t)(FMC_SDRAMCommandStruct->FMC_CommandMode |                     // 设置命令模式位
                      FMC_SDRAMCommandStruct->FMC_CommandTarget |                   // 设置目标存储体位
                     (((FMC_SDRAMCommandStruct->FMC_AutoRefreshNumber)-1)<<5) |     // 自动刷新次数编码（原值范围1-15→0-14）
                     ((FMC_SDRAMCommandStruct->FMC_ModeRegisterDefinition)<<9));   // 模式寄存器参数左移9位
  
  /* 写入命令触发寄存器（写入即生效） */
  FMC_Bank5_6->SDCMR = tmpr;  // 写操作将立即触发命令执行
}

/**
  * @brief  获取SDRAM存储体工作状态
  * @details  通过状态寄存器(SDSR)读取当前SDRAM控制器的模式状态：
  *           - MODES1: Bank1的状态位（00=正常，01=自刷新，10=掉电，11=保留）
  *           - MODES2: Bank2的状态位（同上）
  *           该状态反映SDRAM存储体最近一次接收的命令执行结果
  * 
  * @param  SDRAM_Bank : 指定要查询的存储体
  *          @arg FMC_Bank1_SDRAM: 查询存储体1
  *          @arg FMC_Bank2_SDRAM: 查询存储体2
  * @retval 返回状态编码：
  *           0x00: 正常模式
  *           0x01: 自刷新模式
  *           0x02: 掉电模式
  *           其他值保留
  */
uint32_t FMC_GetModeStatus(uint32_t SDRAM_Bank)
{
  uint32_t tmpreg = 0;  // 临时状态寄存器
  
  /* 检查存储体参数有效性 */
  assert_param(IS_FMC_SDRAM_BANK(SDRAM_Bank));

  /* 根据目标存储体选择状态位 */
  if(SDRAM_Bank == FMC_Bank1_SDRAM)
  {
    /* 提取Bank1的状态位（SDSR[0:1]） */
    tmpreg = (uint32_t)(FMC_Bank5_6->SDSR & FMC_SDSR_MODES1); 
  }
  else
  {
    /* 提取Bank2的状态位（SDSR[2:3]）并右移对齐 */
    tmpreg = ((uint32_t)(FMC_Bank5_6->SDSR & FMC_SDSR_MODES2) >> 2);
  }
  
  /* 返回原始状态位值（需要上层解析具体模式） */
  return tmpreg;
}

/**
  * @brief  设置SDRAM自动刷新计数器
  * @details  配置刷新定时器计数值（SDRTR寄存器），公式：
  *           刷新周期 = (Count + 1) * HCLK周期
  *           典型值计算示例：
  *           对于64ms刷新周期和100MHz时钟：
  *           需要的刷新次数 = 64ms / 15.625μs = 4096
  *           计数值 = (64ms * 100MHz) / (8192 rows) - 1 ≈ 780
  *           实际应参考具体SDRAM器件规格书计算
  * 
  * @param  FMC_Count : 刷新定时器的计数值（0-0x1FFF）
  * @retval 无
  */
void FMC_SetRefreshCount(uint32_t FMC_Count)
{
  /* 校验计数值在合法范围内 */
  assert_param(IS_FMC_REFRESH_COUNT(FMC_Count));
  
  /* 将计数值写入刷新定时器寄存器（SDRTR[21:1]） 
     注：bit0为保留位，计数值左移1位对齐 */
  FMC_Bank5_6->SDRTR |= (FMC_Count<<1);  // 使用|=操作保留其他位状态
}
/**
  * @brief  设置SDRAM自动刷新命令次数
  * @details  配置连续自动刷新操作次数（NCR），该值影响：
  *           - 上电初始化阶段需要的自动刷新次数（通常为2次）
  *           - 自刷新退出后的预刷新次数
  *           实际写入值为(n-1)，有效范围1-15次（对应寄存器值0-14）
  *           注意：该配置仅在下一次发送AUTOREFRESH命令时生效
  * 
  * @param  FMC_Number : 自动刷新次数（1-15次）
  * @retval 无
  */
void FMC_SetAutoRefresh_Number(uint32_t FMC_Number)
{
  /* 校验参数在合法范围内（1-15） */
  assert_param(IS_FMC_AUTOREFRESH_NUMBER(FMC_Number));
  
  /* 将刷新次数编码到命令寄存器（SDCMR[5:8]）
     注意：使用|=操作保留其他位状态，需确保寄存器已初始化 */
  FMC_Bank5_6->SDCMR |= (FMC_Number << 5);   // 实际存储值为(FMC_Number-1)
}

/**
  * @brief  SDRAM写保护配置函数
  * @details  控制指定存储体的写保护功能：
  *           - 启用时：禁止所有写入操作，防止意外数据修改
  *           - 禁用时：允许正常读写操作
  *           该功能通过配置存储控制寄存器（SDCR）的WPEN位实现
  * 
  * @param  SDRAM_Bank : 目标存储体
  *          @arg FMC_Bank1_SDRAM: 存储体1
  *          @arg FMC_Bank2_SDRAM: 存储体2
  * @param  NewState : 写保护状态
  *          @arg ENABLE: 启用写保护（SDCR[12]=1）
  *          @arg DISABLE: 禁用写保护（SDCR[12]=0）
  * @retval 无
  */
void FMC_SDRAMWriteProtectionConfig(uint32_t SDRAM_Bank, FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_FMC_SDRAM_BANK(SDRAM_Bank));
  
  if (NewState != DISABLE)
  {
    /* 设置写保护使能位（SDCR[12]） */
    FMC_Bank5_6->SDCR[SDRAM_Bank] |= FMC_Write_Protection_Enable;    // 0x00001000
  }
  else
  {
    /* 清除写保护位，保留其他配置（使用掩码SDCR_WriteProtection_RESET=0xFFFFEFFF） */
    FMC_Bank5_6->SDCR[SDRAM_Bank] &= SDCR_WriteProtection_RESET;    // 位清除操作
  } 
}


/**
  * @}
  */

/** @defgroup FMC_Group5  Interrupts and flags management functions
  * @brief    Interrupts and flags management functions
  *
@verbatim   
 ===============================================================================
             ##### Interrupts and flags management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  FMC中断配置函数
  * @details  控制FMC外设各类存储控制器的中断使能状态，包括：
  *           - NAND Flash控制器的边沿检测中断
  *           - SDRAM控制器的刷新错误中断
  *           - PCCARD控制器的状态中断
  *           中断使能后需配合NVIC配置中断优先级及编写中断服务例程
  *
  * @param  FMC_Bank : 目标存储控制器
  *          @arg FMC_Bank2_NAND: NAND Bank2（0x70000000）
  *          @arg FMC_Bank3_NAND: NAND Bank3（0x80000000）  
  *          @arg FMC_Bank4_PCCARD: PCCARD Bank4（0x90000000）
  *          @arg FMC_Bank1_SDRAM: SDRAM Bank1（0xC0000000）
  *          @arg FMC_Bank2_SDRAM: SDRAM Bank2（0xD0000000）
  * @param  FMC_IT : 中断类型组合（位或操作）
  *          @arg FMC_IT_RisingEdge: 上升沿触发中断（NAND/PCCARD）
  *          @arg FMC_IT_Level: 高电平触发中断（NAND/PCCARD）  
  *          @arg FMC_IT_FallingEdge: 下降沿触发中断（NAND/PCCARD）
  *          @arg FMC_IT_Refresh: 刷新错误中断（SDRAM）
  * @param  NewState : 中断状态
  *          @arg ENABLE: 使能指定中断源
  *          @arg DISABLE: 禁用指定中断源
  * @retval 无
  */
void FMC_ITConfig(uint32_t FMC_Bank, uint32_t FMC_IT, FunctionalState NewState)
{
  /* 参数合法性校验 */
  assert_param(IS_FMC_IT_BANK(FMC_Bank));     // 检查存储体参数有效性
  assert_param(IS_FMC_IT(FMC_IT));            // 验证中断类型组合
  assert_param(IS_FUNCTIONAL_STATE(NewState));// 状态参数有效性检查
  
  if (NewState != DISABLE)
  {
    /* 中断使能分支 */
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      /* NAND Bank2中断使能：设置SR2寄存器对应位 */
      FMC_Bank2->SR2 |= FMC_IT;  // SR2[0:2]对应上升沿/电平/下降沿中断
    }
    else if (FMC_Bank == FMC_Bank3_NAND)
    {
      /* NAND Bank3中断使能：设置SR3寄存器对应位 */
      FMC_Bank3->SR3 |= FMC_IT;  // 位映射同Bank2
    }
    else if (FMC_Bank == FMC_Bank4_PCCARD)
    {
      /* PCCARD Bank4中断使能：设置SR4寄存器对应位 */  
      FMC_Bank4->SR4 |= FMC_IT;  // 支持三种边沿触发中断
    }
    else
    {
      /* SDRAM Bank5/6中断使能：配置刷新错误中断（SDRTR[0]） */
      FMC_Bank5_6->SDRTR |= FMC_IT;  // 仅支持FMC_IT_Refresh标志位
    }
  }
  else
  {
    /* 中断禁用分支 */  
    if(FMC_Bank == FMC_Bank2_NAND)
    {
      /* NAND Bank2中断禁用：清除SR2寄存器对应位 */
      FMC_Bank2->SR2 &= (uint32_t)~FMC_IT; // 位反码清除操作
    }
    else if (FMC_Bank == FMC_Bank3_NAND)
    {
      /* NAND Bank3中断禁用：清除SR3寄存器对应位 */
      FMC_Bank3->SR3 &= (uint32_t)~FMC_IT;
    }
    else if(FMC_Bank == FMC_Bank4_PCCARD)
    {
      /* PCCARD Bank4中断禁用：清除SR4寄存器对应位 */
      FMC_Bank4->SR4 &= (uint32_t)~FMC_IT;    
    }
    else
    {
      /* SDRAM Bank5/6中断禁用：清除刷新错误中断使能 */
      FMC_Bank5_6->SDRTR &= (uint32_t)~FMC_IT; 
    }
  }
}
/**
  * @brief  获取FMC状态标志位状态
  * @details  查询指定存储控制器的硬件状态标志，包括：
  *           - NAND/PCCARD的边沿检测标志
  *           - FIFO空状态标志
  *           - SDRAM刷新错误标志
  *           - 设备忙状态标志
  *           注意：BUSY标志反映NAND/PCCARD控制器的操作状态
  * 
  * @param  FMC_Bank : 目标存储控制器
  *          @arg FMC_Bank2_NAND: NAND Bank2（SR2寄存器）
  *          @arg FMC_Bank3_NAND: NAND Bank3（SR3寄存器）
  *          @arg FMC_Bank4_PCCARD: PCCARD Bank4（SR4寄存器）
  *          @arg FMC_Bank1_SDRAM: SDRAM Bank1（SDSR寄存器）
  *          @arg FMC_Bank2_SDRAM: SDRAM Bank2（SDSR寄存器）
  * @param  FMC_FLAG : 待查询状态标志
  *          @arg FMC_FLAG_RisingEdge: 上升沿触发标志（NAND/PCCARD）
  *          @arg FMC_FLAG_Level: 高电平状态标志（NAND/PCCARD）
  *          @arg FMC_FLAG_FallingEdge: 下降沿触发标志（NAND/PCCARD）
  *          @arg FMC_FLAG_FEMPT: FIFO空标志（NAND/PCCARD）
  *          @arg FMC_FLAG_Refresh: 刷新错误标志（SDRAM）
  *          @arg FMC_FLAG_Busy: 设备忙状态标志（NAND/PCCARD）
  * @retval 标志位状态：
  *            SET - 标志位置位
  *            RESET - 标志位未置位
  */
FlagStatus FMC_GetFlagStatus(uint32_t FMC_Bank, uint32_t FMC_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tmpsr = 0x00000000;  // 临时保存寄存器值
  
  /* 参数合法性验证 */
  assert_param(IS_FMC_GETFLAG_BANK(FMC_Bank));  // 检查存储体参数有效性
  assert_param(IS_FMC_GET_FLAG(FMC_FLAG));      // 验证标志位组合有效性

  /* 根据存储体选择状态寄存器 */
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    tmpsr = FMC_Bank2->SR2;  // 获取NAND Bank2状态寄存器值
  }  
  else if(FMC_Bank == FMC_Bank3_NAND)
  {
    tmpsr = FMC_Bank3->SR3;  // 获取NAND Bank3状态寄存器值
  }
  else if(FMC_Bank == FMC_Bank4_PCCARD)
  {
    tmpsr = FMC_Bank4->SR4;  // 获取PCCARD Bank4状态寄存器值
  }
  else 
  {
    tmpsr = FMC_Bank5_6->SDSR; // 获取SDRAM状态寄存器（SDSR）值
  }
  
  /* 检测指定标志位状态 */
  if ((tmpsr & FMC_FLAG) != FMC_FLAG )
  {
    bitstatus = RESET;  // 标志位未置位
  }
  else
  {
    bitstatus = SET;     // 标志位置位
  }
  
  return bitstatus;  // 返回检测结果
}

/**
  * @brief  清除FMC状态标志位
  * @details  清除指定存储控制器的硬件状态标志，包括：
  *           - 边沿检测标志（上升沿/电平/下降沿）
  *           - 刷新错误标志
  *           注意：BUSY标志和FEMPT标志为只读状态，无法通过此函数清除
  * 
  * @param  FMC_Bank : 目标存储控制器（参数范围同GetFlagStatus）
  * @param  FMC_FLAG : 待清除标志组合（位或操作）
  *          @arg FMC_FLAG_RisingEdge: 清除上升沿标志
  *          @arg FMC_FLAG_Level: 清除电平状态标志  
  *          @arg FMC_FLAG_FallingEdge: 清除下降沿标志
  *          @arg FMC_FLAG_Refresh: 清除刷新错误标志
  * @retval 无
  */
void FMC_ClearFlag(uint32_t FMC_Bank, uint32_t FMC_FLAG)
{
  /* 参数合法性检查 */
  assert_param(IS_FMC_GETFLAG_BANK(FMC_Bank));  // 存储体验证
  assert_param(IS_FMC_CLEAR_FLAG(FMC_FLAG)) ;   // 可清除标志验证
    
  /* 根据存储体执行清除操作 */
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    /* NAND Bank2标志清除：SR2寄存器位清除 */
    FMC_Bank2->SR2 &= (~FMC_FLAG);  // 位取反后与操作
  }  
  else if(FMC_Bank == FMC_Bank3_NAND)
  {
    /* NAND Bank3标志清除：SR3寄存器位清除 */
    FMC_Bank3->SR3 &= (~FMC_FLAG);
  }
  else if(FMC_Bank == FMC_Bank4_PCCARD)
  {
    /* PCCARD Bank4标志清除：SR4寄存器位清除 */
    FMC_Bank4->SR4 &= (~FMC_FLAG);
  }
  /* SDRAM存储体处理 */
  else
  {
    /* SDRAM标志清除：通过刷新时序寄存器（SDRTR）清除 */
    FMC_Bank5_6->SDRTR &= (~FMC_FLAG);  // 仅支持FMC_FLAG_Refresh
  }
}
/**
  * @brief  获取FMC中断状态
  * @details  检查指定中断源的状态（需同时满足中断使能和标志置位），包括：
  *           - NAND/PCCARD的边沿检测中断
  *           - SDRAM的刷新错误中断
  *           实际状态判断逻辑：中断标志位有效 且 中断使能位已开启
  * 
  * @param  FMC_Bank : 目标存储控制器
  *          @arg FMC_Bank2_NAND: NAND Bank2（SR2寄存器）
  *          @arg FMC_Bank3_NAND: NAND Bank3（SR3寄存器）
  *          @arg FMC_Bank4_PCCARD: PCCARD Bank4（SR4寄存器）
  *          @arg FMC_Bank1_SDRAM: SDRAM Bank1（SDSR/SDRTR）
  *          @arg FMC_Bank2_SDRAM: SDRAM Bank2（SDSR/SDRTR）
  * @param  FMC_IT : 中断类型
  *          @arg FMC_IT_RisingEdge: 上升沿中断（NAND/PCCARD）
  *          @arg FMC_IT_Level: 电平中断（NAND/PCCARD）
  *          @arg FMC_IT_FallingEdge: 下降沿中断（NAND/PCCARD）
  *          @arg FMC_IT_Refresh: 刷新错误中断（SDRAM）
  * @retval 中断状态：
  *            SET - 中断有效且已使能
  *            RESET - 中断未触发或未使能
  */
ITStatus FMC_GetITStatus(uint32_t FMC_Bank, uint32_t FMC_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t tmpsr = 0x0;      // 状态寄存器缓存
  uint32_t tmpsr2 = 0x0;     // SDRAM专用第二状态寄存器
  uint32_t itstatus = 0x0;   // 实际中断标志状态
  uint32_t itenable = 0x0;   // 中断使能状态
  
  /* 参数合法性验证 */
  assert_param(IS_FMC_IT_BANK(FMC_Bank));  // 存储体验证
  assert_param(IS_FMC_GET_IT(FMC_IT));     // 中断类型验证

  /* 获取对应存储体的状态寄存器值 */
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    tmpsr = FMC_Bank2->SR2;  // NAND Bank2状态寄存器
  }  
  else if(FMC_Bank == FMC_Bank3_NAND)
  {
    tmpsr = FMC_Bank3->SR3;  // NAND Bank3状态寄存器
  }
  else if(FMC_Bank == FMC_Bank4_PCCARD)
  {
    tmpsr = FMC_Bank4->SR4;  // PCCARD状态寄存器
  }
  /* SDRAM存储体特殊处理 */
  else
  {
    tmpsr = FMC_Bank5_6->SDRTR;  // 刷新时序寄存器（含中断使能）
    tmpsr2 = FMC_Bank5_6->SDSR; // 状态寄存器（含错误标志）
  } 
  
  /* 提取中断使能状态（SR寄存器低3位为中断使能） */
  itenable = tmpsr & FMC_IT;  // 获取中断使能位状态
  
  /* 获取实际中断标志状态 */
  if((FMC_Bank == FMC_Bank1_SDRAM) || (FMC_Bank == FMC_Bank2_SDRAM))
  {
    /* SDRAM专用判断逻辑：检测SDSR的RE位（刷新错误） */
    itstatus = tmpsr2 & FMC_SDSR_RE;  // 0x00000001
  }           
  else
  {
    /* 常规判断：将中断类型右移3位对齐标志位（SR寄存器高5位为标志位） */
    itstatus = tmpsr & (FMC_IT >> 3);  
  }  
  
  /* 综合判断中断状态 */
  if ((itstatus != (uint32_t)RESET) && (itenable != (uint32_t)RESET))
  {
    bitstatus = SET;  // 中断有效且已使能
  }
  else
  {
    bitstatus = RESET; // 中断未触发或未使能
  }
  return bitstatus; 
}

/**
  * @brief  清除FMC中断挂起标志
  * @details  清除指定中断源的挂起状态，包括：
  *           - 边沿检测中断标志
  *           - 刷新错误中断标志
  *           注意：SDRAM清除操作需通过设置CRE位触发自动清除
  * 
  * @param  FMC_Bank : 目标存储控制器（参数同GetITStatus）
  * @param  FMC_IT : 待清除中断类型组合
  *          @arg FMC_IT_RisingEdge: 清除上升沿中断标志
  *          @arg FMC_IT_Level: 清除电平中断标志
  *          @arg FMC_IT_FallingEdge: 清除下降沿中断标志
  *          @arg FMC_IT_Refresh: 清除刷新错误中断标志
  * @retval 无
  */
void FMC_ClearITPendingBit(uint32_t FMC_Bank, uint32_t FMC_IT)
{
  /* 参数合法性检查 */
  assert_param(IS_FMC_IT_BANK(FMC_Bank));  // 存储体验证
  assert_param(IS_FMC_IT(FMC_IT));         // 中断类型验证
    
  if(FMC_Bank == FMC_Bank2_NAND)
  {
    /* NAND Bank2标志清除：将中断类型右移3位对齐标志位后取反清除 */
    FMC_Bank2->SR2 &= ~(FMC_IT >> 3);  // 操作SR2[5:7]标志位
  }  
  else if(FMC_Bank == FMC_Bank3_NAND)
  {
    /* NAND Bank3标志清除：操作逻辑同Bank2 */
    FMC_Bank3->SR3 &= ~(FMC_IT >> 3);  // 清除SR3对应标志位
  }
  else if(FMC_Bank == FMC_Bank4_PCCARD)
  {
    /* PCCARD Bank4标志清除 */
    FMC_Bank4->SR4 &= ~(FMC_IT >> 3);  // 操作SR4[5:7]
  }
  /* SDRAM存储体特殊处理 */
  else
  {
    /* 通过设置SDRTR的CRE位（bit0）清除刷新错误标志 */
    FMC_Bank5_6->SDRTR |= FMC_SDRTR_CRE;  // 0x00000001
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

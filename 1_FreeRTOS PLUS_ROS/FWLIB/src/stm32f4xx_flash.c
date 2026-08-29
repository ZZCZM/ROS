/**
  ******************************************************************************
  * @file    stm32f4xx_flash.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the FLASH peripheral:
  *            + FLASH Interface configuration
  *            + FLASH Memory Programming
  *            + Option Bytes Programming
  *            + Interrupts and flags management
  *  
 @verbatim    
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
    [..]                             
      This driver provides functions to configure and program the FLASH memory 
      of all STM32F4xx devices. These functions are split in 4 groups:
   
      (#) FLASH Interface configuration functions: this group includes the
          management of the following features:
        (++) Set the latency
        (++) Enable/Disable the prefetch buffer
        (++) Enable/Disable the Instruction cache and the Data cache
        (++) Reset the Instruction cache and the Data cache
    
      (#) FLASH Memory Programming functions: this group includes all needed
          functions to erase and program the main memory:
        (++) Lock and Unlock the FLASH interface
        (++) Erase function: Erase sector, erase all sectors
        (++) Program functions: byte, half word, word and double word
    
      (#) Option Bytes Programming functions: this group includes all needed
          functions to manage the Option Bytes:
        (++) Set/Reset the write protection
        (++) Set the Read protection Level
        (++) Set the BOR level
        (++) Program the user Option Bytes
        (++) Launch the Option Bytes loader
    
      (#) Interrupts and flags management functions: this group 
          includes all needed functions to:
        (++) Enable/Disable the FLASH interrupt sources
        (++) Get flags status
        (++) Clear flags
        (++) Get FLASH operation status
        (++) Wait for last FLASH operation   
 @endverbatim
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
#include "stm32f4xx_flash.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup FLASH 
  * @brief FLASH driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
#define SECTOR_MASK               ((uint32_t)0xFFFFFF07)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FLASH_Private_Functions
  * @{
  */ 

/** @defgroup FLASH_Group1 FLASH Interface configuration functions
  *  @brief   FLASH Interface configuration functions 
 *

@verbatim   
 ===============================================================================
              ##### FLASH Interface configuration functions #####
 ===============================================================================
    [..]
      This group includes the following functions:
      (+) void FLASH_SetLatency(uint32_t FLASH_Latency)
          To correctly read data from FLASH memory, the number of wait states (LATENCY) 
          must be correctly programmed according to the frequency of the CPU clock 
          (HCLK) and the supply voltage of the device.
    [..]      
      For STM32F405xx/07xx and STM32F415xx/17xx devices
 +-------------------------------------------------------------------------------------+
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96 |66 < HCLK <= 88  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |4WS(5CPU cycle)|120< HCLK <= 150|96 < HCLK <= 120|88 < HCLK <= 110 |80 < HCLK <= 100 |
 |---------------|----------------|----------------|-----------------|-----------------|
 |5WS(6CPU cycle)|150< HCLK <= 168|120< HCLK <= 144|110 < HCLK <= 132|100 < HCLK <= 120|
 |---------------|----------------|----------------|-----------------|-----------------|
 |6WS(7CPU cycle)|      NA        |144< HCLK <= 168|132 < HCLK <= 154|120 < HCLK <= 140|
 |---------------|----------------|----------------|-----------------|-----------------|
 |7WS(8CPU cycle)|      NA        |      NA        |154 < HCLK <= 168|140 < HCLK <= 160|
 +---------------|----------------|----------------|-----------------|-----------------+

    [..]      
      For STM32F42xxx/43xxx devices
 +-------------------------------------------------------------------------------------+
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96 |66 < HCLK <= 88  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |4WS(5CPU cycle)|120< HCLK <= 150|96 < HCLK <= 120|88 < HCLK <= 110 |80 < HCLK <= 100 |
 |---------------|----------------|----------------|-----------------|-----------------|
 |5WS(6CPU cycle)|120< HCLK <= 180|120< HCLK <= 144|110 < HCLK <= 132|100 < HCLK <= 120|
 |---------------|----------------|----------------|-----------------|-----------------|
 |6WS(7CPU cycle)|      NA        |144< HCLK <= 168|132 < HCLK <= 154|120 < HCLK <= 140|
 |---------------|----------------|----------------|-----------------|-----------------|
 |7WS(8CPU cycle)|      NA        |168< HCLK <= 180|154 < HCLK <= 176|140 < HCLK <= 160|
 |---------------|----------------|----------------|-----------------|-----------------|
 |8WS(9CPU cycle)|      NA        |      NA        |176 < HCLK <= 180|160 < HCLK <= 168|
 +-------------------------------------------------------------------------------------+
   
    [..]
    For STM32F401x devices
 +-------------------------------------------------------------------------------------+
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |2WS(3CPU cycle)|60 < HCLK <= 84 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |3WS(4CPU cycle)|      NA        |72 < HCLK <= 84 |66 < HCLK <= 84  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |4WS(5CPU cycle)|      NA        |      NA        |      NA         |80 < HCLK <= 84  |
 +-------------------------------------------------------------------------------------+

    [..]
    For STM32F411xE devices
 +-------------------------------------------------------------------------------------+
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 18   |0 < HCLK <= 16   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |1WS(2CPU cycle)|30 < HCLK <= 64 |24 < HCLK <= 48 |18 < HCLK <= 36  |16 < HCLK <= 32  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |2WS(3CPU cycle)|64 < HCLK <= 90 |48 < HCLK <= 72 |36 < HCLK <= 54  |32 < HCLK <= 48  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |3WS(4CPU cycle)|90 < HCLK <= 100|72 < HCLK <= 96 |54 < HCLK <= 72  |48 < HCLK <= 64  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |4WS(5CPU cycle)|      NA        |96 < HCLK <= 100|72 < HCLK <= 90  |64 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |5WS(6CPU cycle)|      NA        |       NA       |90 < HCLK <= 100 |80 < HCLK <= 96  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |6WS(7CPU cycle)|      NA        |       NA       |        NA       |96 < HCLK <= 100 |
 +-------------------------------------------------------------------------------------+
 
 [..]
 +-------------------------------------------------------------------------------------------------------------------+
 |               | voltage range  | voltage range  | voltage range   | voltage range   | voltage range 2.7 V - 3.6 V |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   | with External Vpp = 9V      |
 |---------------|----------------|----------------|-----------------|-----------------|-----------------------------|
 |Max Parallelism|      x32       |               x16                |       x8        |          x64                |
 |---------------|----------------|----------------|-----------------|-----------------|-----------------------------|
 |PSIZE[1:0]     |      10        |               01                 |       00        |           11                |
 +-------------------------------------------------------------------------------------------------------------------+

      -@- On STM32F405xx/407xx and STM32F415xx/417xx devices: 
           (++) when VOS = '0' Scale 2 mode, the maximum value of fHCLK = 144MHz. 
           (++) when VOS = '1' Scale 1 mode, the maximum value of fHCLK = 168MHz. 
          [..] 
          On STM32F42xxx/43xxx devices:
           (++) when VOS[1:0] = '0x01' Scale 3 mode, the maximum value of fHCLK is 120MHz.
           (++) when VOS[1:0] = '0x10' Scale 2 mode, the maximum value of fHCLK is 144MHz if OverDrive OFF and 168MHz if OverDrive ON.
           (++) when VOS[1:0] = '0x11' Scale 1 mode, the maximum value of fHCLK is 168MHz if OverDrive OFF and 180MHz if OverDrive ON. 
          [..]
          On STM32F401x devices:
           (++) when VOS[1:0] = '0x01' Scale 3 mode, the maximum value of fHCLK is 60MHz.
           (++) when VOS[1:0] = '0x10' Scale 2 mode, the maximum value of fHCLK is 84MHz.
          [..]  
          On STM32F411xE devices:
           (++) when VOS[1:0] = '0x01' Scale 3 mode, the maximum value of fHCLK is 64MHz.
           (++) when VOS[1:0] = '0x10' Scale 2 mode, the maximum value of fHCLK is 84MHz.
           (++) when VOS[1:0] = '0x11' Scale 1 mode, the maximum value of fHCLK is 100MHz.

        For more details please refer product DataSheet 
           You can use PWR_MainRegulatorModeConfig() function to control VOS bits.

      (+) void FLASH_PrefetchBufferCmd(FunctionalState NewState)
      (+) void FLASH_InstructionCacheCmd(FunctionalState NewState)
      (+) void FLASH_DataCacheCmd(FunctionalState NewState)
      (+) void FLASH_InstructionCacheReset(void)
      (+) void FLASH_DataCacheReset(void)
      
    [..]   
      The unlock sequence is not needed for these functions.
 
@endverbatim
  * @{
  */
 
/**
  * @brief  Sets the code latency value.  
  * @param  FLASH_Latency: specifies the FLASH Latency value.
  *          This parameter can be one of the following values:
  *            @arg FLASH_Latency_0: FLASH Zero Latency cycle
  *            @arg FLASH_Latency_1: FLASH One Latency cycle
  *            @arg FLASH_Latency_2: FLASH Two Latency cycles
  *            @arg FLASH_Latency_3: FLASH Three Latency cycles
  *            @arg FLASH_Latency_4: FLASH Four Latency cycles 
  *            @arg FLASH_Latency_5: FLASH Five Latency cycles 
  *            @arg FLASH_Latency_6: FLASH Six Latency cycles
  *            @arg FLASH_Latency_7: FLASH Seven Latency cycles 
  *            @arg FLASH_Latency_8: FLASH Eight Latency cycles
  *            @arg FLASH_Latency_9: FLASH Nine Latency cycles
  *            @arg FLASH_Latency_10: FLASH Teen Latency cycles 
  *            @arg FLASH_Latency_11: FLASH Eleven Latency cycles 
  *            @arg FLASH_Latency_12: FLASH Twelve Latency cycles
  *            @arg FLASH_Latency_13: FLASH Thirteen Latency cycles
  *            @arg FLASH_Latency_14: FLASH Fourteen Latency cycles
  *            @arg FLASH_Latency_15: FLASH Fifteen Latency cycles 
  *              
  * @note For STM32F405xx/407xx, STM32F415xx/417xx and STM32F401xx/411xE devices this parameter
  *       can be a value between FLASH_Latency_0 and FLASH_Latency_7.
  *
  * @note For STM32F42xxx/43xxx devices this parameter can be a value between 
  *       FLASH_Latency_0 and FLASH_Latency_15. 
  *         
  * @retval None
  */
/**
  * @brief  设置Flash存储器访问的等待周期（Latency）
  * @param  FLASH_Latency 等待周期值，取值范围：
  *         @arg FLASH_Latency_0: 无等待周期（≤30MHz）
  *         @arg FLASH_Latency_1: 1个等待周期（≤60MHz）
  *         @arg FLASH_Latency_2: 2个等待周期（≤90MHz） 
  *         @arg ...（根据具体芯片型号扩展）
  * @note   该函数通过直接操作FLASH_ACR寄存器的低字节来配置等待周期，
  *         必须与系统时钟配置同步使用，确保时钟频率与等待周期匹配
  */
void FLASH_SetLatency(uint32_t FLASH_Latency)
{
  /* 参数合法性检查，IS_FLASH_LATENCY宏会验证参数是否符合芯片支持的范围 */
  assert_param(IS_FLASH_LATENCY(FLASH_Latency));
  
  /* 通过字节操作方式（8位访问）直接写入ACR寄存器的最低字节：
   * 1. ACR_BYTE0_ADDRESS是ACR寄存器低字节的地址
   * 2. 使用uint8_t类型强制转换确保单字节写入
   * 3. __IO表示volatile访问，防止编译器优化 */
  *(__IO uint8_t *)ACR_BYTE0_ADDRESS = (uint8_t)FLASH_Latency;
}

/**
  * @brief  控制预取缓冲区的启用/禁用状态
  * @param  NewState 功能状态，取值为：
  *         @arg ENABLE: 启用预取缓冲区
  *         @arg DISABLE: 禁用预取缓冲区
  * @note   预取缓冲区可以加速指令读取，建议在系统时钟超过24MHz时启用，
  *         修改后需要等待总线操作完成才能生效
  */
void FLASH_PrefetchBufferCmd(FunctionalState NewState)
{
  /* 验证输入参数是否为合法的功能状态 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 根据新状态设置ACR寄存器的PRFTEN位（预取使能位）：
   * - 使能时：用位或操作置位第5位（PRFTEN）
   * - 禁用时：用位与操作清除第5位 */
  if(NewState != DISABLE)
  {
    FLASH->ACR |= FLASH_ACR_PRFTEN;  // 设置PRFTEN位为1
  }
  else
  {
    FLASH->ACR &= (~FLASH_ACR_PRFTEN); // 清除PRFTEN位
  }
}

/**
  * @brief  控制指令缓存的启用/禁用状态
  * @param  NewState 功能状态，取值为：
  *         @arg ENABLE: 启用指令缓存
  *         @arg DISABLE: 禁用指令缓存
  * @note   指令缓存可提高程序执行效率，建议在初始化阶段启用，
  *         修改缓存状态前需要确保没有正在进行的缓存操作
  */
void FLASH_InstructionCacheCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 操作ACR寄存器的ICEN位（指令缓存使能位）：
   * 位1控制指令缓存，置1启用，清0禁用 */
  if(NewState != DISABLE)
  {
    FLASH->ACR |= FLASH_ACR_ICEN;    // 设置ICEN位为1
  }
  else
  {
    FLASH->ACR &= (~FLASH_ACR_ICEN); // 清除ICEN位
  }
}

/**
  * @brief  控制数据缓存的启用/禁用状态
  * @param  NewState 功能状态，取值为：
  *         @arg ENABLE: 启用数据缓存
  *         @arg DISABLE: 禁用数据缓存
  * @note   数据缓存可提高数据访问效率，但需要注意缓存一致性，
  *         当进行DMA操作或内存数据修改时应考虑缓存同步问题
  */
void FLASH_DataCacheCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 操作ACR寄存器的DCEN位（数据缓存使能位）：
   * 位0控制数据缓存，置1启用，清0禁用 */
  if(NewState != DISABLE)
  {
    FLASH->ACR |= FLASH_ACR_DCEN;    // 设置DCEN位为1
  }
  else
  {
    FLASH->ACR &= (~FLASH_ACR_DCEN); // 清除DCEN位
  }
}

/**
  * @brief  复位指令缓存
  * @note   必须在指令缓存禁用状态下调用该函数，
  *         复位操作会清空缓存内容，重新初始化缓存控制器，
  *         用于解决缓存一致性问题或异常恢复
  */
void FLASH_InstructionCacheReset(void)
{
  /* 设置ACR寄存器的ICRST位（指令缓存复位位）：
   * 位2置1触发复位操作，硬件自动清除该位 */
  FLASH->ACR |= FLASH_ACR_ICRST;
}

/**
  * @brief  复位数据缓存
  * @note   必须在数据缓存禁用状态下调用该函数，
  *         复位操作会清空缓存内容，重新初始化缓存控制器，
  *         用于解决数据一致性问题或系统恢复
  */
void FLASH_DataCacheReset(void)
{
  /* 设置ACR寄存器的DCRST位（数据缓存复位位）：
   * 位3置1触发复位操作，硬件自动清除该位 */
  FLASH->ACR |= FLASH_ACR_DCRST;
}

/**
  * @}
  */

/** @defgroup FLASH_Group2 FLASH Memory Programming functions
 *  @brief   FLASH Memory Programming functions
 *
@verbatim   
 ===============================================================================
                ##### FLASH Memory Programming functions #####
 ===============================================================================   
    [..]
      This group includes the following functions:
      (+) void FLASH_Unlock(void)
      (+) void FLASH_Lock(void)
      (+) FLASH_Status FLASH_EraseSector(uint32_t FLASH_Sector, uint8_t VoltageRange)
      (+) FLASH_Status FLASH_EraseAllSectors(uint8_t VoltageRange)       
      (+) FLASH_Status FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data)
      (+) FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
      (+) FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
      (+) FLASH_Status FLASH_ProgramByte(uint32_t Address, uint8_t Data)
          The following functions can be used only for STM32F42xxx/43xxx devices. 
      (+) FLASH_Status FLASH_EraseAllBank1Sectors(uint8_t VoltageRange)
      (+) FLASH_Status FLASH_EraseAllBank2Sectors(uint8_t VoltageRange)    
    [..]   
      Any operation of erase or program should follow these steps:
      (#) Call the FLASH_Unlock() function to enable the FLASH control register access

      (#) Call the desired function to erase sector(s) or program data

      (#) Call the FLASH_Lock() function to disable the FLASH control register access
          (recommended to protect the FLASH memory against possible unwanted operation)
    
@endverbatim
  * @{
  */
/**
  * @brief  解锁FLASH控制寄存器访问权限
  * @note   在执行擦除/编程操作前必须调用此函数
  * @param  None
  * @retval None
  * @detail 通过连续写入两个特定密钥值解除FLASH_CR寄存器的写保护，
  *         解锁后允许执行擦除、编程等敏感操作。硬件设计要求两个密钥必须
  *         按顺序连续写入才会生效
  */
void FLASH_Unlock(void)
{
  /* 检查LOCK位状态（bit7），当LOCK=1时表示寄存器处于锁定状态 */
  if((FLASH->CR & FLASH_CR_LOCK) != RESET)
  {
    /* 第一步解锁密钥：0x45670123（具体值由FLASH_KEY1定义） 
     * 写入密钥寄存器会触发硬件解锁机制 */
    FLASH->KEYR = FLASH_KEY1;
    
    /* 第二步解锁密钥：0xCDEF89AB（具体值由FLASH_KEY2定义）
     * 必须在第一步之后立即写入才会生效 */
    FLASH->KEYR = FLASH_KEY2;
  }  
}

/**
  * @brief  锁定FLASH控制寄存器访问权限
  * @note   在完成FLASH操作后必须调用此函数恢复保护
  * @param  None
  * @retval None
  * @detail 通过设置LOCK位重新激活FLASH_CR寄存器的写保护，
  *         防止意外修改FLASH控制设置，确保系统安全
  */
void FLASH_Lock(void)
{
  /* 设置CR寄存器的LOCK位（bit7）为1，立即禁止寄存器写操作
   * 使用位或操作保持其他控制位的现有状态 */
  FLASH->CR |= FLASH_CR_LOCK;
}


/**
  * @brief  Erases a specified FLASH Sector.
  *
  * @note   If an erase and a program operations are requested simustaneously,    
  *         the erase operation is performed before the program one.
  *
  * @param  FLASH_Sector: The Sector number to be erased.
  *
  *  @note  For STM32F405xx/407xx and STM32F415xx/417xx devices this parameter can 
  *         be a value between FLASH_Sector_0 and FLASH_Sector_11.
  *
  *         For STM32F42xxx/43xxx devices this parameter can be a value between 
  *         FLASH_Sector_0 and FLASH_Sector_23.
  *
  *         For STM32F401xx devices this parameter can be a value between 
  *         FLASH_Sector_0 and FLASH_Sector_5.
  *
  *         For STM32F411xE devices this parameter can be a value between 
  *         FLASH_Sector_0 and FLASH_Sector_7.
  *
  * @param  VoltageRange: The device voltage range which defines the erase parallelism.  
  *          This parameter can be one of the following values:
  *            @arg VoltageRange_1: when the device voltage range is 1.8V to 2.1V, 
  *                                  the operation will be done by byte (8-bit) 
  *            @arg VoltageRange_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg VoltageRange_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg VoltageRange_4: when the device voltage range is 2.7V to 3.6V + External Vpp, 
  *                                  the operation will be done by double word (64-bit)
  *       
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
/**
  * @brief  擦除指定FLASH存储扇区
  * @param  FLASH_Sector 要擦除的扇区编号，具体取值范围根据芯片型号定义
  *         @arg 对于STM32F42xxx/43xxx系列，取值范围为Sector_0~Sector_11
  * @param  VoltageRange 擦除操作的电压范围，决定编程位宽：
  *         @arg VoltageRange_1: 2.7-3.6V (8位字节操作)
  *         @arg VoltageRange_2: 2.4-2.7V (16位半字操作)
  *         @arg VoltageRange_3: 2.1-2.4V (32位字操作)
  *         @arg VoltageRange_4: 1.8-2.1V (64位双字操作)
  * @retval FLASH状态，可能返回值包括：
  *         @arg FLASH_COMPLETE: 操作成功完成
  *         @arg FLASH_BUSY:     FLASH忙状态
  *         @arg FLASH_ERROR_XXX: 各种错误状态（编程错误/写保护错误/操作错误）
  * @note   操作前必须调用FLASH_Unlock解锁，擦除时间取决于扇区大小和系统时钟
  */
FLASH_Status FLASH_EraseSector(uint32_t FLASH_Sector, uint8_t VoltageRange)
{
  uint32_t tmp_psize = 0x0;
  FLASH_Status status = FLASH_COMPLETE;

  /* 验证输入参数合法性：
   * IS_FLASH_SECTOR 检查扇区编号有效性
   * IS_VOLTAGERANGE 检查电压范围有效性 */
  assert_param(IS_FLASH_SECTOR(FLASH_Sector));
  assert_param(IS_VOLTAGERANGE(VoltageRange));
  
  /* 根据电压范围设置编程位宽：
   * 不同电压范围对应不同的物理擦除单元大小 */
  if(VoltageRange == VoltageRange_1)
  {
     tmp_psize = FLASH_PSIZE_BYTE;     // 8位字节操作
  }
  else if(VoltageRange == VoltageRange_2)
  {
    tmp_psize = FLASH_PSIZE_HALF_WORD; // 16位半字操作
  }
  else if(VoltageRange == VoltageRange_3)
  {
    tmp_psize = FLASH_PSIZE_WORD;      // 32位字操作
  }
  else
  {
    tmp_psize = FLASH_PSIZE_DOUBLE_WORD; // 64位双字操作
  }
  
  /* 等待先前操作完成（如正在进行的编程/擦除操作）：
   * 该函数会轮询状态寄存器直到操作完成或超时 */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  { 
    /* 配置控制寄存器(CR)进行扇区擦除：
     * 1. 清除编程位宽设置(PSIZE) */
    FLASH->CR &= CR_PSIZE_MASK;
    
    /* 2. 设置新的编程位宽 */
    FLASH->CR |= tmp_psize;
    
    /* 3. 清除当前扇区选择设置 */
    FLASH->CR &= SECTOR_MASK;
    
    /* 4. 设置扇区擦除模式(SER)并指定目标扇区 */
    FLASH->CR |= FLASH_CR_SER | FLASH_Sector;
    
    /* 5. 触发擦除操作（STRT位自动清除） */
    FLASH->CR |= FLASH_CR_STRT;
    
    /* 等待擦除操作完成 */
    status = FLASH_WaitForLastOperation();
    
    /* 擦除完成后清理控制寄存器：
     * 1. 禁用扇区擦除模式 */
    FLASH->CR &= (~FLASH_CR_SER);
    
    /* 2. 清除扇区选择设置 */
    FLASH->CR &= SECTOR_MASK; 
  }
  
  /* 返回最终操作状态 */
  return status;
}

/**
  * @brief  Erases all FLASH Sectors.
  *
  * @note   If an erase and a program operations are requested simustaneously,    
  *         the erase operation is performed before the program one.
  *  
  * @param  VoltageRange: The device voltage range which defines the erase parallelism.  
  *          This parameter can be one of the following values:
  *            @arg VoltageRange_1: when the device voltage range is 1.8V to 2.1V, 
  *                                  the operation will be done by byte (8-bit) 
  *            @arg VoltageRange_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg VoltageRange_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg VoltageRange_4: when the device voltage range is 2.7V to 3.6V + External Vpp, 
  *                                  the operation will be done by double word (64-bit)
  *       
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
/**
  * @brief  擦除所有FLASH存储扇区（全片擦除）
  * @param  VoltageRange 擦除操作的电压范围，决定编程位宽：
  *         @arg VoltageRange_1: 2.7-3.6V (8位字节操作)
  *         @arg VoltageRange_2: 2.4-2.7V (16位半字操作)
  *         @arg VoltageRange_3: 2.1-2.4V (32位字操作)
  *         @arg VoltageRange_4: 1.8-2.1V (64位双字操作)
  * @retval FLASH状态，可能返回值包括：
  *         @arg FLASH_COMPLETE: 操作成功完成
  *         @arg FLASH_ERROR_XXX: 各种错误状态
  * @note   该操作将清除整个FLASH存储器内容，慎用！
  *         必须确保系统有正确的断电保护措施，且操作前已调用FLASH_Unlock
  */
FLASH_Status FLASH_EraseAllSectors(uint8_t VoltageRange)
{
  uint32_t tmp_psize = 0x0;
  FLASH_Status status = FLASH_COMPLETE;
  
  /* 等待所有正在进行的FLASH操作完成 */
  status = FLASH_WaitForLastOperation();
  
  /* 验证电压范围参数合法性 */
  assert_param(IS_VOLTAGERANGE(VoltageRange));
  
  /* 根据电压范围确定编程位宽（与FLASH_EraseSector相同） */
  if(VoltageRange == VoltageRange_1)
  {
     tmp_psize = FLASH_PSIZE_BYTE;      // 8位操作
  }
  else if(VoltageRange == VoltageRange_2)
  {
    tmp_psize = FLASH_PSIZE_HALF_WORD;  // 16位操作
  }
  else if(VoltageRange == VoltageRange_3)
  {
    tmp_psize = FLASH_PSIZE_WORD;       // 32位操作
  }
  else
  {
    tmp_psize = FLASH_PSIZE_DOUBLE_WORD;// 64位操作
  }  
  
  if(status == FLASH_COMPLETE)
  {
/*---------- 针对不同芯片系列的条件编译 ----------*/
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)    
    /* 适用于F427/437和F429/439系列的双BANK擦除流程 */
    
    /* 1. 清除并设置编程位宽 */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= tmp_psize;
    
    /* 2. 同时使能两个存储区的整片擦除（MER1和MER2） */
    FLASH->CR |= (FLASH_CR_MER1 | FLASH_CR_MER2);
    
    /* 3. 触发擦除操作 */
    FLASH->CR |= FLASH_CR_STRT;
    
    /* 4. 等待擦除完成 */
    status = FLASH_WaitForLastOperation();
    
    /* 5. 清除双BANK擦除标志 */
    FLASH->CR &= ~(FLASH_CR_MER1 | FLASH_CR_MER2);
#endif 

#if defined (STM32F40_41xxx) || defined (STM32F401xx) || defined (STM32F411xE) 
    /* 适用于F40x/41x、F401和F411系列的单BANK擦除流程 */
    
    /* 1. 清除并设置编程位宽 */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= tmp_psize;
    
    /* 2. 使能整片擦除模式（MER） */
    FLASH->CR |= FLASH_CR_MER;
    
    /* 3. 触发擦除操作 */
    FLASH->CR |= FLASH_CR_STRT;
    
    /* 4. 等待擦除完成 */
    status = FLASH_WaitForLastOperation();
    
    /* 5. 禁用整片擦除模式 */
    FLASH->CR &= (~FLASH_CR_MER);
#endif 
  }   
  
  /* 返回最终操作状态 */
  return status;
}

/**
  * @brief  Erases all FLASH Sectors in Bank 1.
  *
  * @note   This function can be used only for STM32F42xxx/43xxx devices.
  *      
  * @note   If an erase and a program operations are requested simultaneously,    
  *         the erase operation is performed before the program one. 
  *  
  * @param  VoltageRange: The device voltage range which defines the erase parallelism.  
  *          This parameter can be one of the following values:
  *            @arg VoltageRange_1: when the device voltage range is 1.8V to 2.1V, 
  *                                  the operation will be done by byte (8-bit) 
  *            @arg VoltageRange_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg VoltageRange_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg VoltageRange_4: when the device voltage range is 2.7V to 3.6V + External Vpp, 
  *                                  the operation will be done by double word (64-bit)
  *       
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
/**
  * @brief  擦除FLASH存储器的Bank1所有扇区
  * @note   该函数仅适用于支持双BANK架构的STM32F42xxx/43xxx系列器件
  * @param  VoltageRange 擦除操作的电压范围，决定编程位宽：
  *         @arg VoltageRange_1: 2.7-3.6V (8位字节操作)
  *         @arg VoltageRange_2: 2.4-2.7V (16位半字操作)
  *         @arg VoltageRange_3: 2.1-2.4V (32位字操作)
  *         @arg VoltageRange_4: 1.8-2.1V (64位双字操作)
  * @retval FLASH状态，可能返回值包括：
  *         @arg FLASH_COMPLETE: 操作成功完成
  *         @arg FLASH_ERROR_XXX: 各种错误状态
  * @note   操作前必须解锁FLASH，擦除范围仅限于Bank1存储区域
  */
FLASH_Status FLASH_EraseAllBank1Sectors(uint8_t VoltageRange)
{
  uint32_t tmp_psize = 0x0;
  FLASH_Status status = FLASH_COMPLETE;
  
  /* 等待所有正在进行的FLASH操作完成 */
  status = FLASH_WaitForLastOperation();
  
  /* 验证电压范围参数合法性 */
  assert_param(IS_VOLTAGERANGE(VoltageRange));
  
  /* 根据电压范围确定编程位宽 */
  if(VoltageRange == VoltageRange_1)
  {
     tmp_psize = FLASH_PSIZE_BYTE;      // 8位操作
  }
  else if(VoltageRange == VoltageRange_2)
  {
    tmp_psize = FLASH_PSIZE_HALF_WORD;  // 16位操作
  }
  else if(VoltageRange == VoltageRange_3)
  {
    tmp_psize = FLASH_PSIZE_WORD;       // 32位操作
  }
  else
  {
    tmp_psize = FLASH_PSIZE_DOUBLE_WORD;// 64位操作
  }  
  
  if(status == FLASH_COMPLETE)
  {
    /* Bank1全擦除操作流程 */
    
    /* 1. 清除并设置编程位宽 */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= tmp_psize;
    
    /* 2. 使能Bank1整片擦除（MER1位） */
    FLASH->CR |= FLASH_CR_MER1;
    
    /* 3. 触发擦除操作 */
    FLASH->CR |= FLASH_CR_STRT;
    
    /* 4. 等待擦除完成 */
    status = FLASH_WaitForLastOperation();
    
    /* 5. 清除Bank1擦除标志 */
    FLASH->CR &= (~FLASH_CR_MER1);
  }   
  
  /* 返回最终操作状态 */
  return status;
}

/**
  * @brief  擦除FLASH存储器的Bank2所有扇区
  * @note   该函数仅适用于支持双BANK架构的STM32F42xxx/43xxx系列器件
  * @param  VoltageRange 擦除操作的电压范围，决定编程位宽：
  *         @arg VoltageRange_1: 2.7-3.6V (8位字节操作)
  *         @arg VoltageRange_2: 2.4-2.7V (16位半字操作)
  *         @arg VoltageRange_3: 2.1-2.4V (32位字操作)
  *         @arg VoltageRange_4: 1.8-2.1V (64位双字操作)
  * @retval FLASH状态，可能返回值包括：
  *         @arg FLASH_COMPLETE: 操作成功完成
  *         @arg FLASH_ERROR_XXX: 各种错误状态
  * @note   操作前必须解锁FLASH，擦除范围仅限于Bank2存储区域
  */
FLASH_Status FLASH_EraseAllBank2Sectors(uint8_t VoltageRange)
{
  uint32_t tmp_psize = 0x0;
  FLASH_Status status = FLASH_COMPLETE;
  
  /* 等待所有正在进行的FLASH操作完成 */
  status = FLASH_WaitForLastOperation();
  
  /* 验证电压范围参数合法性 */
  assert_param(IS_VOLTAGERANGE(VoltageRange));
  
  /* 根据电压范围确定编程位宽 */
  if(VoltageRange == VoltageRange_1)
  {
     tmp_psize = FLASH_PSIZE_BYTE;      // 8位操作
  }
  else if(VoltageRange == VoltageRange_2)
  {
    tmp_psize = FLASH_PSIZE_HALF_WORD;  // 16位操作
  }
  else if(VoltageRange == VoltageRange_3)
  {
    tmp_psize = FLASH_PSIZE_WORD;       // 32位操作
  }
  else
  {
    tmp_psize = FLASH_PSIZE_DOUBLE_WORD;// 64位操作
  }  
  
  if(status == FLASH_COMPLETE)
  {
    /* Bank2全擦除操作流程 */
    
    /* 1. 清除并设置编程位宽 */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= tmp_psize;
    
    /* 2. 使能Bank2整片擦除（MER2位） */
    FLASH->CR |= FLASH_CR_MER2;
    
    /* 3. 触发擦除操作 */
    FLASH->CR |= FLASH_CR_STRT;
    
    /* 4. 等待擦除完成 */
    status = FLASH_WaitForLastOperation();
    
    /* 5. 清除Bank2擦除标志 */
    FLASH->CR &= (~FLASH_CR_MER2);
  }   
  
  /* 返回最终操作状态 */
  return status;
}

/**
  * @brief  Programs a double word (64-bit) at a specified address.
  * @note   This function must be used when the device voltage range is from
  *         2.7V to 3.6V and an External Vpp is present.
  *
  * @note   If an erase and a program operations are requested simustaneously,    
  *         the erase operation is performed before the program one.
  *  
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
/**
  * @brief  编程双字（64位）数据到指定FLASH地址
  * @param  Address 目标编程地址，必须是8字节对齐的合法FLASH地址
  * @param  Data 要写入的64位数据
  * @retval FLASH状态，可能返回值包括：
  *         @arg FLASH_COMPLETE: 操作成功完成
  *         @arg FLASH_ERROR_XXX: 各种错误状态
  * @note   该函数要求：
  *         1. 工作电压范围2.7V-3.6V且需外部Vpp支持
  *         2. 目标地址必须经过擦除处理（值为0xFFFFFFFFFFFFFFFF）
  *         3. 必须在前序FLASH操作完成后调用
  */
FLASH_Status FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* 验证地址参数是否在有效FLASH地址范围内 */
  assert_param(IS_FLASH_ADDRESS(Address));

  /* 等待所有正在进行的FLASH操作完成 */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  {
    /* 配置控制寄存器进行双字编程操作 */
    
    /* 1. 清除并设置编程位宽为64位 */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_DOUBLE_WORD;
    
    /* 2. 使能编程模式（PG位置1） */
    FLASH->CR |= FLASH_CR_PG;
  
    /* 3. 执行实际数据写入（硬件自动处理双字编程） */
    *(__IO uint64_t*)Address = Data;
        
    /* 等待编程操作完成 */
    status = FLASH_WaitForLastOperation();

    /* 4. 关闭编程模式（PG位清零） */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  
  return status;
}

/**
  * @brief  编程单字（32位）数据到指定FLASH地址
  * @param  Address 目标编程地址，必须是4字节对齐的合法FLASH地址
  * @param  Data 要写入的32位数据
  * @retval FLASH状态，可能返回值包括：
  *         @arg FLASH_COMPLETE: 操作成功完成
  *         @arg FLASH_ERROR_XXX: 各种错误状态
  * @note   该函数要求：
  *         1. 工作电压范围2.7V-3.6V
  *         2. 目标地址必须经过擦除处理（值为0xFFFFFFFF）
  *         3. 必须在前序FLASH操作完成后调用
  */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  assert_param(IS_FLASH_ADDRESS(Address));

  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  {
    /* 配置控制寄存器进行单字编程操作 */
    
    /* 1. 清除并设置编程位宽为32位 */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_WORD;
    
    /* 2. 使能编程模式 */
    FLASH->CR |= FLASH_CR_PG;
  
    /* 3. 执行实际数据写入 */
    *(__IO uint32_t*)Address = Data;
        
    /* 等待编程操作完成 */
    status = FLASH_WaitForLastOperation();

    /* 4. 关闭编程模式 */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  
  return status;
}

/**
  * @brief  Programs a half word (16-bit) at a specified address. 
  * @note   This function must be used when the device voltage range is from 2.1V to 3.6V. 
  *
  * @note   If an erase and a program operations are requested simustaneously,    
  *         the erase operation is performed before the program one.
  * 
  * @param  Address: specifies the address to be programmed.
  *         This parameter can be any address in Program memory zone or in OTP zone.  
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PROGRAM,
  *                       FLASH_ERROR_WRP, FLASH_ERROR_OPERATION or FLASH_COMPLETE.
  */
/**
  * @brief  编程半字（16位）数据到指定FLASH地址
  * @param  Address 目标编程地址，必须是2字节对齐的合法FLASH地址
  * @param  Data 要写入的16位数据
  * @retval FLASH状态，可能返回值包括：
  *         @arg FLASH_COMPLETE: 操作成功完成
  *         @arg FLASH_ERROR_PROGRAM: 编程错误
  *         @arg FLASH_ERROR_WRP: 写保护错误
  *         @arg FLASH_ERROR_OPERATION: 操作错误
  * @note   该函数要求：
  *         1. 工作电压范围2.1V-3.6V
  *         2. 目标地址必须经过擦除处理（值为0xFFFF）
  *         3. 必须在前序FLASH操作完成后调用
  */
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* 验证地址是否在有效FLASH地址范围内 */
  assert_param(IS_FLASH_ADDRESS(Address));

  /* 等待所有正在进行的FLASH操作完成 */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  {
    /* 配置控制寄存器进行半字编程操作 */
    
    /* 1. 清除并设置编程位宽为16位 */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_HALF_WORD;
    
    /* 2. 使能编程模式（PG位置1） */
    FLASH->CR |= FLASH_CR_PG;
  
    /* 3. 执行实际数据写入（硬件自动处理半字编程） */
    *(__IO uint16_t*)Address = Data;
        
    /* 等待编程操作完成 */
    status = FLASH_WaitForLastOperation();

    /* 4. 关闭编程模式（PG位清零） */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  
  return status;
}

/**
  * @brief  编程字节（8位）数据到指定FLASH地址
  * @param  Address 目标编程地址，可以是任意字节对齐的合法FLASH地址
  * @param  Data 要写入的8位数据
  * @retval FLASH状态，可能返回值包括：
  *         @arg FLASH_COMPLETE: 操作成功完成
  *         @arg FLASH_ERROR_PROGRAM: 编程错误
  *         @arg FLASH_ERROR_WRP: 写保护错误
  *         @arg FLASH_ERROR_OPERATION: 操作错误
  * @note   该函数特点：
  *         1. 支持全电压范围（1.8V-3.6V）
  *         2. 地址无需严格对齐（支持单字节操作）
  *         3. 目标地址必须经过擦除处理（值为0xFF）
  */
FLASH_Status FLASH_ProgramByte(uint32_t Address, uint8_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* 验证地址参数合法性 */
  assert_param(IS_FLASH_ADDRESS(Address));

  /* 等待所有正在进行的FLASH操作完成 */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  {
    /* 配置控制寄存器进行字节编程操作 */
    
    /* 1. 清除并设置编程位宽为8位 */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_BYTE;
    
    /* 2. 使能编程模式 */
    FLASH->CR |= FLASH_CR_PG;
  
    /* 3. 执行实际数据写入（支持任意字节地址） */
    *(__IO uint8_t*)Address = Data;
        
    /* 等待编程操作完成（典型时间约40μs） */
    status = FLASH_WaitForLastOperation();

    /* 4. 关闭编程模式 */
    FLASH->CR &= (~FLASH_CR_PG);
  } 

  return status;
}

/**
  * @}
  */

/** @defgroup FLASH_Group3 Option Bytes Programming functions
 *  @brief   Option Bytes Programming functions 
 *
@verbatim   
 ===============================================================================
                ##### Option Bytes Programming functions #####
 ===============================================================================  
    [..]
      This group includes the following functions:
      (+) void FLASH_OB_Unlock(void)
      (+) void FLASH_OB_Lock(void)
      (+) void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState)
      (+) void FLASH_OB_WRP1Config(uint32_t OB_WRP, FunctionalState NewState)  
      (+) void FLASH_OB_PCROPSelectionConfig(uint8_t OB_PCROPSelect)
      (+) void FLASH_OB_PCROPConfig(uint32_t OB_PCROP, FunctionalState NewState)
      (+) void FLASH_OB_PCROP1Config(uint32_t OB_PCROP, FunctionalState NewState) 
      (+) void FLASH_OB_RDPConfig(uint8_t OB_RDP)
      (+) void FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP, uint8_t OB_STDBY)
      (+) void FLASH_OB_BORConfig(uint8_t OB_BOR)
      (+) FLASH_Status FLASH_ProgramOTP(uint32_t Address, uint32_t Data)
      (+) FLASH_Status FLASH_OB_Launch(void)
      (+) uint32_t FLASH_OB_GetUser(void)
      (+) uint8_t FLASH_OB_GetWRP(void)
      (+) uint8_t FLASH_OB_GetWRP1(void)
      (+) uint8_t FLASH_OB_GetPCROP(void)
      (+) uint8_t FLASH_OB_GetPCROP1(void)
      (+) uint8_t FLASH_OB_GetRDP(void)
      (+) uint8_t FLASH_OB_GetBOR(void)
    [..]  
      The following function can be used only for STM32F42xxx/43xxx devices. 
      (+) void FLASH_OB_BootConfig(uint8_t OB_BOOT)
    [..]   
     Any operation of erase or program should follow these steps:
      (#) Call the FLASH_OB_Unlock() function to enable the FLASH option control 
          register access

      (#) Call one or several functions to program the desired Option Bytes:
        (++) void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState) 
             => to Enable/Disable the desired sector write protection
        (++) void FLASH_OB_RDPConfig(uint8_t OB_RDP) => to set the desired read 
             Protection Level
        (++) void FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP, uint8_t OB_STDBY) 
             => to configure the user Option Bytes.
        (++) void FLASH_OB_BORConfig(uint8_t OB_BOR) => to set the BOR Level 			 

      (#) Once all needed Option Bytes to be programmed are correctly written, 
          call the FLASH_OB_Launch() function to launch the Option Bytes 
          programming process.
     
      -@- When changing the IWDG mode from HW to SW or from SW to HW, a system 
          reset is needed to make the change effective.  

      (#) Call the FLASH_OB_Lock() function to disable the FLASH option control 
          register access (recommended to protect the Option Bytes against 
          possible unwanted operations)
    
@endverbatim
  * @{
  */

/**
  * @brief  Unlocks the FLASH Option Control Registers access.
  * @param  None
  * @retval None
  */
void FLASH_OB_Unlock(void)
{
  if((FLASH->OPTCR & FLASH_OPTCR_OPTLOCK) != RESET)
  {
    /* Authorizes the Option Byte register programming */
    FLASH->OPTKEYR = FLASH_OPT_KEY1;
    FLASH->OPTKEYR = FLASH_OPT_KEY2;
  }  
}

/**
  * @brief  Locks the FLASH Option Control Registers access.
  * @param  None
  * @retval None
  */
void FLASH_OB_Lock(void)
{
  /* Set the OPTLOCK Bit to lock the FLASH Option Byte Registers access */
  FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;
}

/**
  * @brief  Enables or disables the write protection of the desired sectors, for the first
  *         1 Mb of the Flash  
  *
  * @note   When the memory read protection level is selected (RDP level = 1), 
  *         it is not possible to program or erase the flash sector i if CortexM4  
  *         debug features are connected or boot code is executed in RAM, even if nWRPi = 1 
  * @note   Active value of nWRPi bits is inverted when PCROP mode is active (SPRMOD =1).   
  * 
  * @param  OB_WRP: specifies the sector(s) to be write protected or unprotected.
  *          This parameter can be one of the following values:
  *            @arg OB_WRP: A value between OB_WRP_Sector0 and OB_WRP_Sector11                      
  *            @arg OB_WRP_Sector_All
  * @param  Newstate: new state of the Write Protection.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None  
  */
/**
  * @brief  配置主存储区(Bank1)的写保护选项字节
  * @param  OB_WRP 要操作的扇区掩码，取值：
  *         @arg OB_WRP_Sector0~OB_WRP_Sector11: 单独扇区操作
  *         @arg OB_WRP_Sector_All: 全扇区操作
  * @param  NewState 写保护状态：
  *         @arg ENABLE: 解除指定扇区写保护
  *         @arg DISABLE: 启用指定扇区写保护
  * @retval 无
  * @note   操作生效需执行选项字节加载(OB_Launch)
  *         写保护状态与nWRPi位值相反（0=保护，1=解除）
  */
void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState)
{ 
  FLASH_Status status = FLASH_COMPLETE;
  
  /* 验证参数有效性 */
  assert_param(IS_OB_WRP(OB_WRP));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
    
  /* 等待前序Flash操作完成 */
  status = FLASH_WaitForLastOperation();

  if(status == FLASH_COMPLETE)
  { 
    /* 通过字节地址访问OPTCR寄存器第2字节（nWRPi控制位） */
    if(NewState != DISABLE)
    {
      /* 清除保护位：将nWRPi位置1，解除对应扇区写保护 */
      *(__IO uint16_t*)OPTCR_BYTE2_ADDRESS &= (~OB_WRP);
    }
    else
    {
      /* 设置保护位：将nWRPi位清0，启用对应扇区写保护 */
      *(__IO uint16_t*)OPTCR_BYTE2_ADDRESS |= (uint16_t)OB_WRP;
    }
  }
}

/**
  * @brief  配置第二存储区(Bank2)的写保护选项字节
  * @note   仅适用于STM32F42xxx/43xxx双Bank器件
  * @param  OB_WRP 要操作的扇区掩码，取值：
  *         @arg OB_WRP_Sector12~OB_WRP_Sector23: Bank2扇区
  *         @arg OB_WRP_Sector_All: 全扇区操作
  * @param  NewState 写保护状态：
  *         @arg ENABLE: 解除指定扇区写保护
  *         @arg DISABLE: 启用指定扇区写保护
  * @retval 无
  * @note   操作生效需执行选项字节加载(OB_Launch)
  *         实际访问OPTCR1寄存器（Bank2配置寄存器）
  */
void FLASH_OB_WRP1Config(uint32_t OB_WRP, FunctionalState NewState)
{ 
  FLASH_Status status = FLASH_COMPLETE;
  
  assert_param(IS_OB_WRP(OB_WRP));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
    
  status = FLASH_WaitForLastOperation();

  if(status == FLASH_COMPLETE)
  { 
    /* 通过字节地址访问OPTCR1寄存器第2字节 */
    if(NewState != DISABLE)
    {
      *(__IO uint16_t*)OPTCR1_BYTE2_ADDRESS &= (~OB_WRP);
    }
    else
    {
      *(__IO uint16_t*)OPTCR1_BYTE2_ADDRESS |= (uint16_t)OB_WRP;
    }
  }
}

/**
  * @brief  选择PCROP保护模式
  * @param  OB_PcROP 保护模式选择：
  *         @arg OB_PcROP_Disable: 常规写保护模式（nWRPi控制写保护）
  *         @arg OB_PcROP_Enable: 增强保护模式（nWRPi控制读/写保护）
  * @retval 无
  * @note   该操作不可逆，激活后只能通过全片擦除恢复
  *         PCROP模式下：
  *         - 读取被保护扇区会触发RDERR标志
  *         - 写入被保护扇区会触发WRPERR标志
  *         配置步骤：
  *         1. 禁用所有扇区的PCROP保护
  *         2. 启用指定扇区的PCROP
  *         3. 最后激活PCROP模式
  */
void FLASH_OB_PCROPSelectionConfig(uint8_t OB_PcROP)
{  
  uint8_t optiontmp = 0xFF;
      
  /* 验证参数有效性 */
  assert_param(IS_OB_PCROP_SELECT(OB_PcROP));
  
  /* 读取OPTCR寄存器第3字节，保留除SPRMOD位外的其他位 */
  optiontmp =  (uint8_t)((*(__IO uint8_t *)OPTCR_BYTE3_ADDRESS) & (uint8_t)0x7F); 
  
  /* 设置SPRMOD位（位7）并写回寄存器 */
  *(__IO uint8_t *)OPTCR_BYTE3_ADDRESS = (uint8_t)(OB_PcROP | optiontmp); 
    
}


/**
  * @brief  Enables or disables the read/write protection (PCROP) of the desired 
  *         sectors, for the first 1 MB of the Flash.
  *           
  * @note   This function can be used only for STM32F42xxx/43xxx and STM32F401xx/411xE devices. 
  *   
  * @param  OB_PCROP: specifies the sector(s) to be read/write protected or unprotected.
  *          This parameter can be one of the following values:
  *            @arg OB_PCROP: A value between OB_PCROP_Sector0 and OB_PCROP_Sector11 for 
  *                           STM32F42xxx/43xxx devices and between OB_PCROP_Sector0 and 
  *                           OB_PCROP_Sector5 for STM32F401xx/411xE devices.
  *            @arg OB_PCROP_Sector_All
  * @param  Newstate: new state of the Write Protection.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None  
  */
/**
  * @brief  配置主存储区(Bank1)的PCROP读/写保护选项
  * @param  OB_PCROP 要操作的扇区掩码，取值：
  *         @arg OB_PCROP_Sector0~OB_PCROP_Sector11: 单独扇区操作
  *         @arg OB_PCROP_Sector_All: 全扇区操作
  * @param  NewState PCROP保护状态：
  *         @arg ENABLE: 启用指定扇区读/写保护
  *         @arg DISABLE: 禁用指定扇区读/写保护
  * @retval 无
  * @note   操作生效需执行选项字节加载(OB_Launch)
  *         PCROP模式下nWRPi位逻辑反转（1=保护，0=解除）
  */
void FLASH_OB_PCROPConfig(uint32_t OB_PCROP, FunctionalState NewState)
{ 
  FLASH_Status status = FLASH_COMPLETE;
  
  /* 验证参数有效性 */
  assert_param(IS_OB_PCROP(OB_PCROP));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
    
  /* 等待前序Flash操作完成 */
  status = FLASH_WaitForLastOperation();

  if(status == FLASH_COMPLETE)
  { 
    /* 通过字节地址访问OPTCR寄存器第2字节（PCROP控制位） */
    if(NewState != DISABLE)
    {
      /* 设置保护位：nWRPi位置1，启用读/写保护 */
      *(__IO uint16_t*)OPTCR_BYTE2_ADDRESS |= (uint16_t)OB_PCROP;    
    }
    else
    {
      /* 清除保护位：nWRPi位清0，禁用读/写保护 */
      *(__IO uint16_t*)OPTCR_BYTE2_ADDRESS &= (~OB_PCROP);
    }
  }
}

/**
  * @brief  配置第二存储区(Bank2)的PCROP读/写保护选项
  * @note   仅适用于STM32F42xxx/43xxx双Bank器件
  * @param  OB_PCROP 要操作的扇区掩码，取值：
  *         @arg OB_PCROP_Sector12~OB_PCROP_Sector23: Bank2扇区
  *         @arg OB_PCROP_Sector_All: 全扇区操作
  * @param  NewState PCROP保护状态：
  *         @arg ENABLE: 启用指定扇区读/写保护
  *         @arg DISABLE: 禁用指定扇区读/写保护
  * @retval 无
  * @note   实际访问OPTCR1寄存器（Bank2配置寄存器）
  *         PCROP模式下需先调用FLASH_OB_PCROPSelectionConfig启用模式
  */
void FLASH_OB_PCROP1Config(uint32_t OB_PCROP, FunctionalState NewState)
{ 
  FLASH_Status status = FLASH_COMPLETE;
  
  assert_param(IS_OB_PCROP(OB_PCROP));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
    
  status = FLASH_WaitForLastOperation();

  if(status == FLASH_COMPLETE)
  { 
    /* 通过字节地址访问OPTCR1寄存器第2字节 */
    if(NewState != DISABLE)
    {
      *(__IO uint16_t*)OPTCR1_BYTE2_ADDRESS |= (uint16_t)OB_PCROP;
    }
    else
    {
      *(__IO uint16_t*)OPTCR1_BYTE2_ADDRESS &= (~OB_PCROP);
    }
  }
}

/**
  * @brief  设置Flash读保护等级
  * @param  OB_RDP 读保护等级：
  *         @arg OB_RDP_Level_0: 无保护（默认状态）
  *         @arg OB_RDP_Level_1: 启用读保护（禁止调试访问）
  *         @arg OB_RDP_Level_2: 永久保护（不可逆操作）
  * @retval 无
  * @note   Level 2设置后：
  *         - 无法通过调试接口访问Flash
  *         - 无法降级到Level 0/1
  *         - 只能通过全片擦除恢复（会同时清除Flash内容）
  */
void FLASH_OB_RDPConfig(uint8_t OB_RDP)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* 验证读保护等级参数 */
  assert_param(IS_OB_RDP(OB_RDP));

  /* 等待前序操作完成 */
  status = FLASH_WaitForLastOperation();

  if(status == FLASH_COMPLETE)
  {
    /* 直接写入OPTCR寄存器第1字节（RDP位） */
    *(__IO uint8_t*)OPTCR_BYTE1_ADDRESS = OB_RDP;
  }
}

/**
  * @brief  Programs the FLASH User Option Byte: IWDG_SW / RST_STOP / RST_STDBY.    
  * @param  OB_IWDG: Selects the IWDG mode
  *          This parameter can be one of the following values:
  *            @arg OB_IWDG_SW: Software IWDG selected
  *            @arg OB_IWDG_HW: Hardware IWDG selected
  * @param  OB_STOP: Reset event when entering STOP mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_STOP_NoRST: No reset generated when entering in STOP
  *            @arg OB_STOP_RST: Reset generated when entering in STOP
  * @param  OB_STDBY: Reset event when entering Standby mode.
  *          This parameter  can be one of the following values:
  *            @arg OB_STDBY_NoRST: No reset generated when entering in STANDBY
  *            @arg OB_STDBY_RST: Reset generated when entering in STANDBY
  * @retval None
  */
/**
  * @brief  配置用户选项字节（看门狗/停机/待机设置）
  * @param  OB_IWDG 看门狗模式：
  *         @arg OB_IWDG_SW: 软件看门狗
  *         @arg OB_IWDG_HW: 硬件看门狗
  * @param  OB_STOP 停机模式复位配置：
  *         @arg OB_STOP_NoRST: 停机模式不产生复位
  *         @arg OB_STOP_RST: 停机模式产生复位
  * @param  OB_STDBY 待机模式复位配置：
  *         @arg OB_STDBY_NoRST: 待机模式不产生复位
  *         @arg OB_STDBY_RST: 待机模式产生复位
  * @retval 无
  * @note   配置需调用FLASH_OB_Launch生效
  *         不同芯片型号的寄存器掩码不同
  */
void FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP, uint8_t OB_STDBY)
{
  uint8_t optiontmp = 0xFF;
  FLASH_Status status = FLASH_COMPLETE; 

  /* 参数有效性检查 */
  assert_param(IS_OB_IWDG_SOURCE(OB_IWDG));
  assert_param(IS_OB_STOP_SOURCE(OB_STOP));
  assert_param(IS_OB_STDBY_SOURCE(OB_STDBY));

  /* 等待前序操作完成 */
  status = FLASH_WaitForLastOperation();
  
  if(status == FLASH_COMPLETE)
  { 
/* 不同芯片系列的掩码处理 */
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
    /* 保留OPTCR[4:0]位（BFB2/BOR_LEV/保留位） */
    optiontmp =  (uint8_t)((*(__IO uint8_t *)OPTCR_BYTE0_ADDRESS) & 0x1F);
#endif 

#if defined (STM32F40_41xxx) || defined (STM32F401xx) || defined (STM32F411xE)
    /* 保留OPTCR[3:0]位（BOR_LEV/保留位） */
    optiontmp =  (uint8_t)((*(__IO uint8_t *)OPTCR_BYTE0_ADDRESS) & 0x0F); 
#endif 

    /* 组合配置参数并写入寄存器 */
    *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS = OB_IWDG | (OB_STDBY | (OB_STOP | optiontmp)); 
  }  
}

/**
  * @brief  配置双Bank启动模式
  * @note   仅适用于STM32F42xxx/43xxx系列
  * @param  OB_BOOT 双Bank启动选项：
  *         @arg OB_Dual_BootEnabled: Bank2作为启动区
  *         @arg OB_Dual_BootDisabled: 默认启动模式
  * @retval 无
  * @note   修改OPTCR寄存器的BFB2位（位4）
  */
void FLASH_OB_BootConfig(uint8_t OB_BOOT)
{
  assert_param(IS_OB_BOOT(OB_BOOT));

  /* 清除并设置BFB2位 */
  *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS &= ~FLASH_OPTCR_BFB2;
  *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS |= OB_BOOT;
}

/**
  * @brief  配置掉电复位（BOR）阈值电压
  * @param  OB_BOR BOR级别：
  *         @arg OB_BOR_LEVEL3: 2.7-3.6V
  *         @arg OB_BOR_LEVEL2: 2.4-2.7V 
  *         @arg OB_BOR_LEVEL1: 2.1-2.4V
  *         @arg OB_BOR_OFF: 1.62-2.1V
  * @retval 无
  * @note   影响芯片复位电压阈值，配置需调用Launch生效
  */
void FLASH_OB_BORConfig(uint8_t OB_BOR)
{
  assert_param(IS_OB_BOR(OB_BOR));

  /* 清除并设置BOR_LEV位（OPTCR[1:0]） */
  *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS &= ~FLASH_OPTCR_BOR_LEV;
  *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS |= OB_BOR;
}

/**
  * @brief  触发选项字节加载（使配置生效）
  * @param  无
  * @retval FLASH状态
  * @note   设置OPTSTRT位后自动开始加载
  *         加载过程会导致系统复位
  */
FLASH_Status FLASH_OB_Launch(void)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* 设置OPTSTRT位（OPTCR[6]）启动加载 */
  *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS |= FLASH_OPTCR_OPTSTRT;

  /* 等待加载操作完成 */
  status = FLASH_WaitForLastOperation();

  return status;
}

/**
  * @brief  获取当前用户选项字节配置
  * @param  无
  * @retval 用户选项字节状态（bit2-0）：
  *         bit0: IWDG_SW（看门狗模式）
  *         bit1: RST_STOP（停机复位）
  *         bit2: RST_STDBY（待机复位）
  */
uint8_t FLASH_OB_GetUser(void)
{
  /* 提取OPTCR[7:5]位 */
  return (uint8_t)(FLASH->OPTCR >> 5);
}

/**
  * @brief  获取主存储区写保护状态
  * @param  无
  * @retval 写保护位图（bit11-0对应扇区0-11）
  */
uint16_t FLASH_OB_GetWRP(void)
{
  /* 返回OPTCR[31:16]的值 */
  return (*(__IO uint16_t *)(OPTCR_BYTE2_ADDRESS));
}

/**
  * @brief  获取第二存储区写保护状态
  * @note   仅适用于STM32F42xxx/43xxx系列
  * @param  无
  * @retval 写保护位图（bit23-12对应扇区12-23）
  */
uint16_t FLASH_OB_GetWRP1(void)
{
  /* 返回OPTCR1[31:16]的值 */
  return (*(__IO uint16_t *)(OPTCR1_BYTE2_ADDRESS));
}


/**
  * @brief  Returns the FLASH PC Read/Write Protection Option Bytes value.
  *   
  * @note   This function can be used only for STM32F42xxx/43xxx devices and STM32F401xx/411xE devices.
  *   
  * @param  None
  * @retval The FLASH PC Read/Write Protection Option Bytes value
  */
/**
  * @brief  获取主存储区(Bank1)的PCROP保护状态
  * @param  无
  * @retval PCROP保护位图，bit0-11对应扇区0-11
  *         @arg 位值为1表示启用读/写保护
  * @note   返回值需结合SPRMOD位状态解读：
  *         - 若SPRMOD=0: 1=写保护启用
  *         - 若SPRMOD=1: 1=读/写保护启用
  */
uint16_t FLASH_OB_GetPCROP(void)
{
  /* 返回OPTCR[31:16]寄存器值（nWRPi位状态） */
  return (*(__IO uint16_t *)(OPTCR_BYTE2_ADDRESS));
}

/**
  * @brief  获取第二存储区(Bank2)的PCROP保护状态
  * @note   仅适用于STM32F42xxx/43xxx双Bank器件
  * @param  无
  * @retval PCROP保护位图，bit12-23对应扇区12-23
  *         @arg 位值为1表示启用读/写保护
  */
uint16_t FLASH_OB_GetPCROP1(void)
{
  /* 返回OPTCR1[31:16]寄存器值 */
  return (*(__IO uint16_t *)(OPTCR1_BYTE2_ADDRESS));
}

/**
  * @brief  检测读保护等级是否启用
  * @param  无
  * @retval 保护状态：
  *         @arg SET: 已启用读保护（Level1/Level2）
  *         @arg RESET: 未启用读保护（Level0）
  * @note   无法通过此函数区分Level1和Level2状态
  */
FlagStatus FLASH_OB_GetRDP(void)
{
  FlagStatus readstatus = RESET;

  /* 检查RDP寄存器值是否为默认Level0（0xAA） */
  if ((*(__IO uint8_t*)(OPTCR_BYTE1_ADDRESS) != (uint8_t)OB_RDP_Level_0))
  {
    readstatus = SET;  // 检测到非默认保护等级
  }
  else
  {
    readstatus = RESET; // 处于未保护状态
  }
  return readstatus;
}

/**
  * @brief  获取当前BOR（掉电复位）配置级别
  * @param  无
  * @retval BOR级别：
  *         @arg OB_BOR_LEVEL3: 2.7-3.6V
  *         @arg OB_BOR_LEVEL2: 2.4-2.7V
  *         @arg OB_BOR_LEVEL1: 2.1-2.4V 
  *         @arg OB_BOR_OFF: 1.62-2.1V
  * @note   实际阈值参考芯片数据手册的精确电压值
  */
uint8_t FLASH_OB_GetBOR(void)
{
  /* 提取OPTCR[1:0]位并转换为标准枚举值 */
  return (uint8_t)(*(__IO uint8_t *)(OPTCR_BYTE0_ADDRESS) & 0x0C);
}

/**
  * @}
  */

/** @defgroup FLASH_Group4 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
              ##### Interrupts and flags management functions #####
 ===============================================================================  
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified FLASH interrupts.
  * @param  FLASH_IT: specifies the FLASH interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg FLASH_IT_ERR: FLASH Error Interrupt
  *            @arg FLASH_IT_EOP: FLASH end of operation Interrupt
  * @retval None 
  */
/**
  * @brief  启用或禁用指定的Flash中断源
  * @param  FLASH_IT 要配置的中断类型，可选组合：
  *         @arg FLASH_IT_EOP: 操作完成中断
  *         @arg FLASH_IT_ERR: 错误中断（包含所有错误类型）
  * @param  NewState 中断状态：
  *         @arg ENABLE: 启用中断
  *         @arg DISABLE: 禁用中断
  * @retval 无
  * @note   错误中断包括以下所有错误类型：
  *         - 操作错误(OPERR)
  *         - 写保护错误(WRPERR)
  *         - 对齐错误(PGAERR)
  *         - 并行错误(PGPERR)
  *         - 序列错误(PGSERR)
  *         - 读保护错误(RDERR)
  */
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_FLASH_IT(FLASH_IT)); 
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if(NewState != DISABLE)
  {
    /* 设置CR寄存器对应中断使能位 */
    FLASH->CR |= FLASH_IT;
  }
  else
  {
    /* 清除CR寄存器对应中断使能位 */
    FLASH->CR &= ~(uint32_t)FLASH_IT;
  }
}

/**
  * @brief  获取指定Flash状态标志的状态
  * @param  FLASH_FLAG 要查询的状态标志，可选值：
  *         @arg FLASH_FLAG_EOP: 操作完成标志
  *         @arg FLASH_FLAG_OPERR: 操作错误标志
  *         @arg FLASH_FLAG_WRPERR: 写保护错误标志
  *         @arg FLASH_FLAG_PGAERR: 对齐错误标志
  *         @arg FLASH_FLAG_PGPERR: 并行操作错误标志
  *         @arg FLASH_FLAG_PGSERR: 操作序列错误标志
  *         @arg FLASH_FLAG_RDERR: 读保护错误标志
  *         @arg FLASH_FLAG_BSY: Flash忙状态标志
  * @retval 标志状态：
  *         @arg SET: 标志置位
  *         @arg RESET: 标志未置位
  * @note   查询BSY标志可判断Flash操作是否正在进行
  */
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* 参数合法性验证 */
  assert_param(IS_FLASH_GET_FLAG(FLASH_FLAG));

  /* 检查SR寄存器对应标志位 */
  if((FLASH->SR & FLASH_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus; 
}

/**
  * @brief  清除指定的Flash状态标志
  * @param  FLASH_FLAG 要清除的标志，可选组合：
  *         @arg FLASH_FLAG_EOP
  *         @arg FLASH_FLAG_OPERR
  *         @arg FLASH_FLAG_WRPERR
  *         @arg FLASH_FLAG_PGAERR
  *         @arg FLASH_FLAG_PGPERR
  *         @arg FLASH_FLAG_PGSERR
  *         @arg FLASH_FLAG_RDERR
  * @retval 无
  * @note   清除标志需向对应位写入1
  *         操作完成后必须清除EOP标志
  */
void FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
  /* 参数合法性验证 */
  assert_param(IS_FLASH_CLEAR_FLAG(FLASH_FLAG));
  
  /* 通过写1清除标志位（写入SR寄存器） */
  FLASH->SR = FLASH_FLAG;
}

/**
  * @brief  获取当前Flash操作的综合状态
  * @param  无
  * @retval Flash操作状态，可能返回值：
  *         @arg FLASH_BUSY: 操作进行中
  *         @arg FLASH_ERROR_WRP: 写保护错误
  *         @arg FLASH_ERROR_RD: 读保护错误（PCROP模式下触发）
  *         @arg FLASH_ERROR_PROGRAM: 编程错误（对齐/并行/序列错误）
  *         @arg FLASH_ERROR_OPERATION: 未定义的操作错误
  *         @arg FLASH_COMPLETE: 操作成功完成
  * @note   错误检测优先级顺序：
  *         1. 写保护错误 > 2. 读保护错误 > 3. 编程错误 > 4. 操作错误
  */
FLASH_Status FLASH_GetStatus(void)
{
  FLASH_Status flashstatus = FLASH_COMPLETE;
  
  /* 首先检测BSY标志（位0）判断是否处于操作状态 */
  if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
  {
    flashstatus = FLASH_BUSY;
  }
  else 
  {  
    /* 检查写保护错误标志（位14） */
    if((FLASH->SR & FLASH_FLAG_WRPERR) != 0x00)
    { 
      flashstatus = FLASH_ERROR_WRP;
    }
    else
    {
      /* 检查读保护错误标志（位13） */
      if((FLASH->SR & FLASH_FLAG_RDERR) != 0x00)
      { 
        flashstatus = FLASH_ERROR_RD;
      } 
      else 
      {
        /* 检查编程相关错误（位3-5,7） */
        if((FLASH->SR & 0xEF) != 0x00)  // 0xEF对应位掩码11101111
        {
          flashstatus = FLASH_ERROR_PROGRAM; 
        }
        else
        {
          /* 检查未分类操作错误（位1） */
          if((FLASH->SR & FLASH_FLAG_OPERR) != 0x00)
          {
            flashstatus = FLASH_ERROR_OPERATION;
          }
          else
          {
            flashstatus = FLASH_COMPLETE;
          }
        }
      }
    }
  }
  return flashstatus;
}

/**
  * @brief  阻塞等待直到Flash操作完成
  * @param  无
  * @retval 最终操作状态（同FLASH_GetStatus返回值）
  * @note   该函数通过轮询机制等待操作完成
  *         典型使用场景：
  *         - 擦除/编程操作后等待完成
  *         - 配置选项字节前等待空闲状态
  */
FLASH_Status FLASH_WaitForLastOperation(void)
{ 
  __IO FLASH_Status status = FLASH_COMPLETE;
   
  /* 获取初始状态 */
  status = FLASH_GetStatus();

  /* 循环检测BUSY标志直到操作完成 */
  while(status == FLASH_BUSY)
  {
    status = FLASH_GetStatus();  // 每次循环约消耗3-5个时钟周期
  }
  
  /* 返回最终状态（可能包含错误码） */
  return status;
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

/**
  ******************************************************************************
  * @file    stm32f4xx_spi.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Serial peripheral interface (SPI):
  *           + Initialization and Configuration
  *           + Data transfers functions
  *           + Hardware CRC Calculation
  *           + DMA transfers management
  *           + Interrupts and flags management 
  *           
@verbatim

 ===================================================================
                  ##### How to use this driver #####
 ===================================================================
 [..]
   (#) Enable peripheral clock using the following functions 
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE) for SPI1
       RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE) for SPI2
       RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE) for SPI3
       RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE) for SPI4
       RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE) for SPI5
       RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE) for SPI6.
  
   (#) Enable SCK, MOSI, MISO and NSS GPIO clocks using RCC_AHB1PeriphClockCmd()
       function. In I2S mode, if an external clock source is used then the I2S 
       CKIN pin GPIO clock should also be enabled.
  
   (#) Peripherals alternate function: 
       (++) Connect the pin to the desired peripherals' Alternate Function (AF) 
            using GPIO_PinAFConfig() function
       (++) Configure the desired pin in alternate function by: 
            GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
       (++) Select the type, pull-up/pull-down and output speed via GPIO_PuPd, 
            GPIO_OType and GPIO_Speed members
       (++) Call GPIO_Init() function In I2S mode, if an external clock source is 
            used then the I2S CKIN pin should be also configured in Alternate 
            function Push-pull pull-up mode. 
          
   (#) Program the Polarity, Phase, First Data, Baud Rate Prescaler, Slave 
       Management, Peripheral Mode and CRC Polynomial values using the SPI_Init()
       function.
       In I2S mode, program the Mode, Standard, Data Format, MCLK Output, Audio 
       frequency and Polarity using I2S_Init() function. For I2S mode, make sure 
       that either:
       (++) I2S PLL is configured using the functions 
            RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S), RCC_PLLI2SCmd(ENABLE) and 
            RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY); or 
       (++) External clock source is configured using the function 
            RCC_I2SCLKConfig(RCC_I2S2CLKSource_Ext) and after setting correctly 
            the define constant I2S_EXTERNAL_CLOCK_VAL in the stm32f4xx_conf.h file. 
  
   (#) Enable the NVIC and the corresponding interrupt using the function 
       SPI_ITConfig() if you need to use interrupt mode. 
  
   (#) When using the DMA mode 
       (++) Configure the DMA using DMA_Init() function
       (++) Active the needed channel Request using SPI_I2S_DMACmd() function
   
   (#) Enable the SPI using the SPI_Cmd() function or enable the I2S using
       I2S_Cmd().
   
   (#) Enable the DMA using the DMA_Cmd() function when using DMA mode. 
  
   (#) Optionally, you can enable/configure the following parameters without
       re-initialization (i.e there is no need to call again SPI_Init() function):
       (++) When bidirectional mode (SPI_Direction_1Line_Rx or SPI_Direction_1Line_Tx)
            is programmed as Data direction parameter using the SPI_Init() function
            it can be possible to switch between SPI_Direction_Tx or SPI_Direction_Rx
            using the SPI_BiDirectionalLineConfig() function.
       (++) When SPI_NSS_Soft is selected as Slave Select Management parameter 
            using the SPI_Init() function it can be possible to manage the 
            NSS internal signal using the SPI_NSSInternalSoftwareConfig() function.
       (++) Reconfigure the data size using the SPI_DataSizeConfig() function  
       (++) Enable or disable the SS output using the SPI_SSOutputCmd() function  
            
    (#) To use the CRC Hardware calculation feature refer to the Peripheral 
        CRC hardware Calculation subsection.
     
  
 [..] It is possible to use SPI in I2S full duplex mode, in this case, each SPI 
      peripheral is able to manage sending and receiving data simultaneously
      using two data lines. Each SPI peripheral has an extended block called I2Sxext
      (ie. I2S2ext for SPI2 and I2S3ext for SPI3).
      The extension block is not a full SPI IP, it is used only as I2S slave to
      implement full duplex mode. The extension block uses the same clock sources
      as its master.          
      To configure I2S full duplex you have to:
              
      (#) Configure SPIx in I2S mode (I2S_Init() function) as described above. 
             
      (#) Call the I2S_FullDuplexConfig() function using the same strucutre passed to  
          I2S_Init() function.
              
      (#) Call I2S_Cmd() for SPIx then for its extended block.
            
      (#) To configure interrupts or DMA requests and to get/clear flag status, 
          use I2Sxext instance for the extension block.
               
 [..] Functions that can be called with I2Sxext instances are: I2S_Cmd(), 
      I2S_FullDuplexConfig(), SPI_I2S_ReceiveData(), SPI_I2S_SendData(), 
      SPI_I2S_DMACmd(), SPI_I2S_ITConfig(), SPI_I2S_GetFlagStatus(), 
      SPI_I2S_ClearFlag(), SPI_I2S_GetITStatus() and SPI_I2S_ClearITPendingBit().
                   
      Example: To use SPI3 in Full duplex mode (SPI3 is Master Tx, I2S3ext is Slave Rx):
              
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);   
      I2S_StructInit(&I2SInitStruct);
      I2SInitStruct.Mode = I2S_Mode_MasterTx;     
      I2S_Init(SPI3, &I2SInitStruct);
      I2S_FullDuplexConfig(SPI3ext, &I2SInitStruct)
      I2S_Cmd(SPI3, ENABLE);
      I2S_Cmd(SPI3ext, ENABLE);
      ...
      while (SPI_I2S_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET)
      {}
      SPI_I2S_SendData(SPI3, txdata[i]);
      ...  
      while (SPI_I2S_GetFlagStatus(I2S3ext, SPI_FLAG_RXNE) == RESET)
      {}
      rxdata[i] = SPI_I2S_ReceiveData(I2S3ext);
      ...          
                
 [..]       
   (@) In I2S mode: if an external clock is used as source clock for the I2S,  
       then the define I2S_EXTERNAL_CLOCK_VAL in file stm32f4xx_conf.h should 
       be enabled and set to the value of the source clock frequency (in Hz).
   
   (@) In SPI mode: To use the SPI TI mode, call the function SPI_TIModeCmd() 
       just after calling the function SPI_Init().
  
@endverbatim  
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
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup SPI 
  * @brief SPI driver modules
  * @{
  */ 
/**
  * @brief SPI/I2S寄存器配置宏定义
  * @功能 定义SPI/I2S相关寄存器的位掩码和配置参数，用于：
  *        - 清除控制寄存器中的特定配置位
  *        - 配置RCC时钟系统的PLL参数
  *        - 定义SPI帧格式和状态标志位
  * @说明 这些宏主要服务于STM32F4xx系列SPI外设的底层寄存器操作，
  *       通过位掩码实现对寄存器位的精确控制
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* SPI寄存器掩码 ------------------------------------------------------------*/
/* SPI registers Masks */
/* CR1控制寄存器清除掩码（保留位13、12、6） 
   [二进制 0011 0000 0100 0000]
   用于清除以下位：
   - 位13: 保留位
   - 位12: 保留位
   - 位6:  SPI使能位(SPE) */
#define CR1_CLEAR_MASK            ((uint16_t)0x3040)

/* I2S配置寄存器清除掩码 [二进制 1111 0000 0100 0000]
   用于清除：
   - I2S模式选择位(I2SMOD)
   - I2S配置相关位(ASTRTEN, PCMSYNC等) */
#define I2SCFGR_CLEAR_MASK        ((uint16_t)0xF040)

/* RCC PLL配置掩码 ---------------------------------------------------------*/
/* RCC PLLs masks */
/* PLL配置寄存器(PLLCFGR)的PLLR分频系数掩码（位29-28-27）
   [二进制 0111 0000 0000 0000 0000 0000 0000 0000]
   对应PLLR[2:0]位，控制主PLL分频系数 */
#define PLLCFGR_PPLR_MASK         ((uint32_t)0x70000000)

/* PLLN倍频系数掩码（位14-13-12-11-10-9-6） 
   [二进制 0000 0000 0000 0000 0111 1111 1100 0000]
   对应PLLN[8:0]位，设置主PLL倍频系数 */
#define PLLCFGR_PPLN_MASK         ((uint32_t)0x00007FC0)

/* SPI特殊功能寄存器位定义 -------------------------------------------------*/
/* 控制寄存器2帧格式位（位4）
   当设置该位时选择TI帧格式 */
#define SPI_CR2_FRF               ((uint16_t)0x0010)

/* 状态寄存器TI帧格式错误标志（位8）
   当检测到TI模式下的帧格式错误时置位 */
#define SPI_SR_TIFRFE             ((uint16_t)0x0100)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup SPI_Private_Functions
  * @{
  */

/** @defgroup SPI_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
             ##### Initialization and Configuration functions ##### 
 ===============================================================================  
 [..] This section provides a set of functions allowing to initialize the SPI 
      Direction, SPI Mode, SPI Data Size, SPI Polarity, SPI Phase, SPI NSS 
      Management, SPI Baud Rate Prescaler, SPI First Bit and SPI CRC Polynomial.
  
 [..] The SPI_Init() function follows the SPI configuration procedures for Master 
      mode and Slave mode (details for these procedures are available in reference 
      manual (RM0090)).
  
@endverbatim
  * @{
  */

/**
  * @brief  De-initialize the SPIx peripheral registers to their default reset values.
  * @param  SPIx: To select the SPIx/I2Sx peripheral, where x can be: 1, 2, 3, 4, 5 or 6 
  *         in SPI mode or 2 or 3 in I2S mode.   
  *         
  * @note   The extended I2S blocks (ie. I2S2ext and I2S3ext blocks) are de-initialized
  *         when the relative I2S peripheral is de-initialized (the extended block's clock
  *         is managed by the I2S peripheral clock).
  *             
  * @retval None
  */
/**
  * @brief  SPI/I2S外设反初始化函数
  * @功能  重置指定SPI/I2S外设的所有寄存器到默认值，通过执行以下操作：
  *        1. 使能外设复位信号
  *        2. 禁用复位信号完成复位操作
  * @参数  SPIx: 指定要反初始化的SPI/I2S外设，支持SPI1-SPI6
  * @注意  复位操作会：
  *        - 清除所有配置寄存器
  *        - 复位数据缓冲区指针
  *        - 禁用SPI/I2S时钟
  *        - 复位所有状态标志位
  * @硬件依赖 不同SPI实例位于不同总线：
  *        - SPI1/4/5/6在APB2总线
  *        - SPI2/3在APB1总线
  */
void SPI_I2S_DeInit(SPI_TypeDef* SPIx)
{
  /* 参数合法性检查：验证SPIx属于有效外设实例 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));

  /* 根据不同的SPI实例选择复位策略 */
  if (SPIx == SPI1)
  {
    /* APB2总线上的SPI1复位操作 */
    /* 触发SPI1硬件复位（保持复位状态） */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
    /* 释放SPI1复位（完成复位过程） */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);
  }
  else if (SPIx == SPI2)
  {
    /* APB1总线上的SPI2复位操作 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, DISABLE);
  }
  else if (SPIx == SPI3)
  {
    /* APB1总线上的SPI3复位操作 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, DISABLE);
  }
  else if (SPIx == SPI4)
  {
    /* APB2总线上的SPI4复位操作 */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI4, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI4, DISABLE);
  }
  else if (SPIx == SPI5)
  {
    /* APB2总线上的SPI5复位操作 */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI5, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI5, DISABLE);
  }
  else  /* 处理特殊实例SPI6 */
  {
    if (SPIx == SPI6)
    {
      /* APB2总线上的SPI6复位操作 */
      RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI6, ENABLE);
      RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI6, DISABLE);
    }
    /* 注意：此处没有默认错误处理，假定调用前已通过assert_param验证参数 */
  }
}
/**
  * @brief  SPI外设初始化函数
  * @功能  根据配置结构体参数初始化SPI控制器，主要完成：
  *        - 通信方向配置（全双工/半双工）
  *        - 工作模式设置（主/从模式）
  *        - 数据帧格式配置（数据位宽、时钟极性、相位）
  *        - NSS信号管理（硬件/软件控制）
  *        - 波特率预分频器设置
  *        - CRC校验多项式配置
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @参数  SPI_InitStruct: 包含所有配置参数的结构体指针
  * @注意  该函数将：
  *        - 重置CR1寄存器相关配置位
  *        - 强制清除I2S模式设置
  *        - 配置CRC多项式寄存器
  */
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct)
{
  uint16_t tmpreg = 0;
  
  /* 外设实例有效性检查 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  
  /* 配置参数合法性验证（9项关键参数检查） */
  assert_param(IS_SPI_DIRECTION_MODE(SPI_InitStruct->SPI_Direction));  // 通信方向模式
  assert_param(IS_SPI_MODE(SPI_InitStruct->SPI_Mode));                  // 主/从模式
  assert_param(IS_SPI_DATASIZE(SPI_InitStruct->SPI_DataSize));          // 数据位宽（8/16位）
  assert_param(IS_SPI_CPOL(SPI_InitStruct->SPI_CPOL));                  // 时钟极性（空闲状态高/低）
  assert_param(IS_SPI_CPHA(SPI_InitStruct->SPI_CPHA));                  // 时钟相位（第1/2个边沿采样）
  assert_param(IS_SPI_NSS(SPI_InitStruct->SPI_NSS));                    // 片选信号管理方式
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_InitStruct->SPI_BaudRatePrescaler)); // 波特率分频系数
  assert_param(IS_SPI_FIRST_BIT(SPI_InitStruct->SPI_FirstBit));          // 数据传输顺序（MSB/LSB优先）
  assert_param(IS_SPI_CRC_POLYNOMIAL(SPI_InitStruct->SPI_CRCPolynomial)); // CRC多项式系数

/*-------------------------- SPI控制寄存器1（CR1）配置 -----------------------*/
  /* 获取当前CR1寄存器值 */
  tmpreg = SPIx->CR1;
  
  /* 清除CR1中需要配置的位域（使用预定义掩码0x3040）
     清除位包括：
     - 位15: BIDIMODE（双向数据模式）
     - 位14: BIDIOE（双向输出使能） 
     - 位10: RXONLY（仅接收模式）
     - 位9:  SSM（软件从机管理）
     - 位8:  SSI（内部从机选择）
     - 位7:  LSBFirst（低位优先）
     - 位5-3: BR[2:0]（波特率分频）
     - 位2:  MSTR（主设备选择）
     - 位1:  CPOL（时钟极性）
     - 位0:  CPHA（时钟相位） */
  tmpreg &= CR1_CLEAR_MASK;

  /* 组合配置参数到临时寄存器值 */
  tmpreg |= (uint16_t)((uint32_t)SPI_InitStruct->SPI_Direction |  // 通信方向（位15,14,10）
                      SPI_InitStruct->SPI_Mode |                  // 工作模式（位2,9,8）
                      SPI_InitStruct->SPI_DataSize |              // 数据位宽（位11）
                      SPI_InitStruct->SPI_CPOL |                  // 时钟极性（位1）
                      SPI_InitStruct->SPI_CPHA |                  // 时钟相位（位0）
                      SPI_InitStruct->SPI_NSS |                   // 片选管理（位9,8）
                      SPI_InitStruct->SPI_BaudRatePrescaler |     // 波特率分频（位5-3）
                      SPI_InitStruct->SPI_FirstBit);              // 传输顺序（位7）

  /* 将新配置写入CR1寄存器 */
  SPIx->CR1 = tmpreg;

  /* 强制进入SPI模式：清除I2SCFGR寄存器的I2S模式位（位11）
     当该位为0时表示使用SPI模式 */
  SPIx->I2SCFGR &= (uint16_t)~((uint16_t)SPI_I2SCFGR_I2SMOD);

/*-------------------------- SPI CRC多项式寄存器配置 ------------------------*/
  /* 配置CRC校验多项式值（用于硬件CRC计算） */
  SPIx->CRCPR = SPI_InitStruct->SPI_CRCPolynomial;  // 写入CRCPR寄存器
}

/**
  * @brief  Initializes the SPIx peripheral according to the specified 
  *         parameters in the I2S_InitStruct.
  * @param  SPIx: where x can be  2 or 3 to select the SPI peripheral (configured in I2S mode).
  * @param  I2S_InitStruct: pointer to an I2S_InitTypeDef structure that
  *         contains the configuration information for the specified SPI peripheral
  *         configured in I2S mode.
  *           
  * @note   The function calculates the optimal prescaler needed to obtain the most 
  *         accurate audio frequency (depending on the I2S clock source, the PLL values 
  *         and the product configuration). But in case the prescaler value is greater 
  *         than 511, the default value (0x02) will be configured instead.    
  * 
  * @note   if an external clock is used as source clock for the I2S, then the define
  *         I2S_EXTERNAL_CLOCK_VAL in file stm32f4xx_conf.h should be enabled and set
  *         to the value of the the source clock frequency (in Hz).
  *  
  * @retval None
  */
/**
  * @brief  I2S外设初始化函数
  * @功能  配置I2S音频协议参数，主要实现：
  *        - 时钟源选择（内部PLLI2S/外部时钟）
  *        - 分频系数计算（根据目标音频频率）
  *        - 工作模式配置（主/从模式）
  *        - 协议标准设置（Philips/MSB/LSB等）
  *        - 数据格式定义（16/32位数据帧）
  *        - 主时钟输出控制（MCLK使能）
  * @参数  SPIx: 目标SPI外设（仅支持SPI2/SPI3）
  * @参数  I2S_InitStruct: 包含I2S配置参数的结构体指针
  * @注意  该函数将：
  *        - 自动处理PLL时钟树配置
  *        - 执行分频系数边界检查
  *        - 强制设置I2S模式（覆盖SPI模式）
  */
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct)
{
  uint16_t tmpreg = 0, i2sdiv = 2, i2sodd = 0, packetlength = 1;
  uint32_t tmp = 0, i2sclk = 0;
#ifndef I2S_EXTERNAL_CLOCK_VAL
  uint32_t pllm = 0, plln = 0, pllr = 0; // PLL分频系数存储变量
#endif /* I2S_EXTERNAL_CLOCK_VAL */
  
  /* 参数合法性验证（7项关键检查） */
  assert_param(IS_SPI_23_PERIPH(SPIx));               // 外设实例限制检查（仅SPI2/3支持I2S）
  assert_param(IS_I2S_MODE(I2S_InitStruct->I2S_Mode));// 主/从模式验证
  assert_param(IS_I2S_STANDARD(I2S_InitStruct->I2S_Standard)); // 协议标准验证
  assert_param(IS_I2S_DATA_FORMAT(I2S_InitStruct->I2S_DataFormat)); // 数据位宽验证
  assert_param(IS_I2S_MCLK_OUTPUT(I2S_InitStruct->I2S_MCLKOutput)); // MCLK输出控制
  assert_param(IS_I2S_AUDIO_FREQ(I2S_InitStruct->I2S_AudioFreq));  // 音频频率范围检查
  assert_param(IS_I2S_CPOL(I2S_InitStruct->I2S_CPOL)); // 时钟极性验证

/*----------------------- I2S配置寄存器初始化 -----------------------------*/
  /* 清除I2SCFGR寄存器关键位（使用预定义掩码0xF040）：
     - 位11: I2S模式使能(I2SMOD)
     - 位10: I2S使能(I2SE)
     - 位9-8: I2S配置(I2SCFG)
     - 位7:   PCM同步(PCMSYNC)
     - 位5-4: 标准选择(I2SSTD)
     - 位3:   时钟极性(CKPOL)
     - 位2-1: 数据长度(DATLEN)
     - 位0:   通道长度(CHLEN) */
  SPIx->I2SCFGR &= I2SCFGR_CLEAR_MASK; 
  
  /* 初始化I2S预分频寄存器（设置默认分频系数2） */
  SPIx->I2SPR = 0x0002;

  /* 获取当前I2SCFGR寄存器值用于后续配置 */
  tmpreg = SPIx->I2SCFGR;

  /* 处理默认音频频率情况（48kHz） */
  if(I2S_InitStruct->I2S_AudioFreq == I2S_AudioFreq_Default)
  {
    i2sodd = 0;   // 奇数分频系数标志
    i2sdiv = 2;   // 默认分频系数
  }
  /* 自定义音频频率处理 */
  else
  {
    /* 确定数据包长度（影响分频计算） */
    if(I2S_InitStruct->I2S_DataFormat == I2S_DataFormat_16b)
    {
      packetlength = 1; // 16位数据包（实际传输32位时钟周期）
    }
    else
    {
      packetlength = 2; // 32位数据包（实际传输64位时钟周期）
    }

    /* 时钟源选择逻辑 -------------------------------------------------------*/
  #ifdef I2S_EXTERNAL_CLOCK_VAL     
    /* 外部时钟模式配置 */
    if ((RCC->CFGR & RCC_CFGR_I2SSRC) == 0)
    {
      RCC->CFGR |= (uint32_t)RCC_CFGR_I2SSRC; // 启用外部时钟源
    }
    i2sclk = I2S_EXTERNAL_CLOCK_VAL;          // 使用预定义外部时钟频率

  #else /* 内部PLLI2S时钟配置 */
    if ((RCC->CFGR & RCC_CFGR_I2SSRC) != 0)
    {
      RCC->CFGR &= ~(uint32_t)RCC_CFGR_I2SSRC; // 清除外部时钟选择
    }    
    
    /* 提取PLLI2S配置参数 */
    plln = (uint32_t)(((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> 6) & 0x1FF); // 倍频系数N
    pllr = (uint32_t)(((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> 28) & 0x7); // 分频系数R
    pllm = (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM); // 主PLL分频系数M

    /* 计算PLLI2S输出频率 */
    if((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) == RCC_PLLCFGR_PLLSRC_HSE)
    {
      i2sclk = (uint32_t)(((HSE_VALUE / pllm) * plln) / pllr); // 外部晶振时钟源
    }
    else
    { 
      i2sclk = (uint32_t)(((HSI_VALUE / pllm) * plln) / pllr); // 内部RC时钟源
    }
  #endif /* 时钟源选择结束 */

    /* 分频系数计算 ---------------------------------------------------------*/
    if(I2S_InitStruct->I2S_MCLKOutput == I2S_MCLKOutput_Enable)
    {
      /* MCLK输出时的分频计算（公式：i2sclk/(256*fs)） */
      tmp = (uint16_t)(((((i2sclk / 256) * 10) / I2S_InitStruct->I2S_AudioFreq)) + 5);
    }
    else
    {
      /* 无MCLK输出时的分频计算（公式：i2sclk/(32*packetlength*fs)） */
      tmp = (uint16_t)(((((i2sclk / (32 * packetlength)) *10 ) / I2S_InitStruct->I2S_AudioFreq)) + 5);
    }
    
    /* 精度调整和奇偶处理 */
    tmp = tmp / 10;          // 去除小数精度（+5用于四舍五入）
    i2sodd = (tmp & 0x0001); // 判断奇数分频
    i2sdiv = (tmp - i2sodd)/2; // 计算实际分频系数
    i2sodd <<= 8;            // 移位到I2SPR寄存器的ODD位（位8）
  }

  /* 分频系数边界检查（2-255） */
  if ((i2sdiv < 2) || (i2sdiv > 0xFF))
  {
    i2sdiv = 2; // 复位为默认值
    i2sodd = 0;
  }

  /* 写入I2S预分频寄存器（组合配置） */
  SPIx->I2SPR = (uint16_t)(i2sdiv | i2sodd | I2S_InitStruct->I2S_MCLKOutput);

  /* 配置I2SCFGR寄存器 -----------------------------------------------------*/
  tmpreg |= (uint16_t)(SPI_I2SCFGR_I2SMOD |          // 强制I2S模式（位11）
                      I2S_InitStruct->I2S_Mode |    // 主/从模式（位10:9）
                      I2S_InitStruct->I2S_Standard | // 协议标准（位5:4）
                      I2S_InitStruct->I2S_DataFormat | // 数据格式（位2:1）
                      I2S_InitStruct->I2S_CPOL);     // 时钟极性（位3）

  /* 应用寄存器配置 */
  SPIx->I2SCFGR = tmpreg;
}
/**
  * @brief  SPI配置结构体初始化函数
  * @功能  将SPI_InitTypeDef结构体所有成员重置为默认值，用于快速初始化配置参数
  * @参数  SPI_InitStruct: 待初始化的SPI配置结构体指针
  * @注意  默认配置为：
  *        - 全双工双线模式
  *        - 从机模式
  *        - 8位数据帧
  *        - 低电平空闲时钟
  *        - 第一个时钟边沿采样
  *        - 硬件NSS管理
  *        - 2分频波特率
  *        - MSB优先传输
  *        - CRC多项式值为7
  */
void SPI_StructInit(SPI_InitTypeDef* SPI_InitStruct)
{
/*--------------- SPI配置结构体默认值初始化 -----------------*/
  /* 通信方向：双线全双工（MOSI+MISO） */
  SPI_InitStruct->SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  
  /* 工作模式：从机模式（依赖NSS引脚信号） */
  SPI_InitStruct->SPI_Mode = SPI_Mode_Slave;
  
  /* 数据帧格式：8位数据（对应CR1寄存器DFF位=0） */
  SPI_InitStruct->SPI_DataSize = SPI_DataSize_8b;
  
  /* 时钟极性：低电平空闲（CPOL=0） */
  SPI_InitStruct->SPI_CPOL = SPI_CPOL_Low;
  
  /* 时钟相位：第一个时钟边沿采样（CPHA=0） */
  SPI_InitStruct->SPI_CPHA = SPI_CPHA_1Edge;
  
  /* 片选管理：硬件模式（NSS引脚由硬件自动控制） */
  SPI_InitStruct->SPI_NSS = SPI_NSS_Hard;
  
  /* 波特率分频：2分频（最高速配置） */
  SPI_InitStruct->SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  
  /* 数据传输顺序：高位优先（MSB First） */
  SPI_InitStruct->SPI_FirstBit = SPI_FirstBit_MSB;
  
  /* CRC多项式：默认值7（对应x⁸ + x² + x + 1） */
  SPI_InitStruct->SPI_CRCPolynomial = 7;
}

/**
  * @brief  I2S配置结构体初始化函数 
  * @功能  将I2S_InitTypeDef结构体成员重置为默认配置参数
  * @参数  I2S_InitStruct: 待初始化的I2S配置结构体指针
  * @注意  默认配置为：
  *        - 从机发送模式
  *        - 飞利浦I2S标准
  *        - 16位数据格式
  *        - 禁用MCLK输出
  *        - 默认音频频率（48kHz）
  *        - 低电平时钟极性
  */
void I2S_StructInit(I2S_InitTypeDef* I2S_InitStruct)
{
/*--------------- I2S配置结构体默认值初始化 -----------------*/
  /* 工作模式：从机发送模式（SCK由外部主机提供） */
  I2S_InitStruct->I2S_Mode = I2S_Mode_SlaveTx;
  
  /* 协议标准：飞利浦I2S标准（左对齐数据） */
  I2S_InitStruct->I2S_Standard = I2S_Standard_Phillips;
  
  /* 数据格式：16位数据（32位时钟周期） */
  I2S_InitStruct->I2S_DataFormat = I2S_DataFormat_16b;
  
  /* 主时钟输出：禁用（节省功耗） */
  I2S_InitStruct->I2S_MCLKOutput = I2S_MCLKOutput_Disable;
  
  /* 音频频率：默认48kHz（需配合时钟配置） */
  I2S_InitStruct->I2S_AudioFreq = I2S_AudioFreq_Default;
  
  /* 时钟极性：低电平空闲（与SPI模式兼容） */
  I2S_InitStruct->I2S_CPOL = I2S_CPOL_Low;
}

/**
  * @brief  SPI外设使能控制函数
  * @功能  启用或禁用指定SPI外设的核心功能
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @参数  NewState: 新状态（ENABLE-启用，DISABLE-禁用）
  * @注意  该操作直接影响CR1寄存器的SPE位：
  *        - 使能时会激活SPI时钟和I/O端口
  *        - 禁用时保留寄存器配置但停止传输
  */
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));           // 检查SPI实例有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));     // 检查状态值合法性
  
  if (NewState != DISABLE)
  {
    /* 使能SPI外设：设置CR1寄存器的SPE位（位6） */
    SPIx->CR1 |= SPI_CR1_SPE;  // SPE = 1，激活SPI功能
  }
  else
  {
    /* 禁用SPI外设：清除CR1寄存器的SPE位 */
    SPIx->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_SPE); // SPE = 0，停止所有SPI操作
  }
}
/**
  * @brief  I2S模式使能控制函数
  * @功能  激活或关闭指定SPI外设的I2S模式
  * @参数  SPIx: 目标外设（仅支持SPI2/SPI3及其扩展）
  * @参数  NewState: 新状态（ENABLE-启用I2S，DISABLE-禁用I2S）
  * @注意  该操作直接影响I2SCFGR寄存器的I2SE位（位10）：
  *        - 启用时会激活I2S时钟和音频协议处理
  *        - 禁用时保留配置但停止I2S通信
  */
void I2S_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_SPI_23_PERIPH_EXT(SPIx));  // 验证SPI2/3及其扩展实例
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 状态值有效性检查

  if (NewState != DISABLE)
  {
    /* 启用I2S模式：设置I2SCFGR寄存器的I2SE位（位10） */
    SPIx->I2SCFGR |= SPI_I2SCFGR_I2SE; // I2SE=1，启动I2S协议引擎
  }
  else
  {
    /* 禁用I2S模式：清除I2SCFGR寄存器的I2SE位 */
    SPIx->I2SCFGR &= (uint16_t)~((uint16_t)SPI_I2SCFGR_I2SE); // I2SE=0，停止I2S功能
  }
}

/**
  * @brief  SPI数据帧长度配置函数
  * @功能  设置SPI通信的数据位宽度（8位或16位）
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @参数  SPI_DataSize: 数据位宽选择（SPI_DataSize_8b/16b）
  * @注意  该操作修改CR1寄存器的DFF位（位11）：
  *        - 8位模式：DFF=0，每个数据项占用1字节
  *        - 16位模式：DFF=1，每个数据项占用2字节
  */
void SPI_DataSizeConfig(SPI_TypeDef* SPIx, uint16_t SPI_DataSize)
{
  /* 参数检查 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));         // 验证外设实例有效性
  assert_param(IS_SPI_DATASIZE(SPI_DataSize));   // 数据位宽参数验证

  /* 清除DFF位（位11），准备设置新值 */
  SPIx->CR1 &= (uint16_t)~SPI_DataSize_16b;  // 使用掩码0xF7FF清除位11
  
  /* 设置新的数据位宽配置 */
  SPIx->CR1 |= SPI_DataSize;  // 写入新值（0x0000或0x0800）
}

/**
  * @brief  SPI双向模式数据方向配置
  * @功能  在单线双向模式下控制数据传输方向
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @参数  SPI_Direction: 传输方向（发送Tx/接收Rx）
  * @注意  该操作修改CR1寄存器的BIDIOE位（位14）：
  *        - Tx方向：BIDIOE=1，启用MOSI输出
  *        - Rx方向：BIDIOE=0，关闭MOSI进入高阻
  */
void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, uint16_t SPI_Direction)
{
  /* 参数验证 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));         // 外设实例检查
  assert_param(IS_SPI_DIRECTION(SPI_Direction));// 方向参数合法性

  if (SPI_Direction == SPI_Direction_Tx)
  {
    /* 设置为发送模式：设置BIDIOE位（位14） */
    SPIx->CR1 |= SPI_Direction_Tx;  // 0x4000，启用输出驱动器
  }
  else
  {
    /* 设置为接收模式：清除BIDIOE位 */
    SPIx->CR1 &= SPI_Direction_Rx;  // 使用掩码0xBFFF清除位14
  }
}

/**
  * @brief  SPI内部NSS信号配置函数
  * @功能  在软件NSS模式下设置内部NSS信号电平
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @参数  SPI_NSSInternalSoft: 内部NSS状态（Set-高电平/Reset-低电平）
  * @注意  该操作修改CR1寄存器的SSI位（位8）：
  *        - 仅在SSM=1（软件从机管理）时有效
  *        - Set: SSI=1，模拟NSS高电平（从机不激活）
  *        - Reset: SSI=0，模拟NSS低电平（从机激活）
  */
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, uint16_t SPI_NSSInternalSoft)
{
  /* 参数检查 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));             // 外设实例验证
  assert_param(IS_SPI_NSS_INTERNAL(SPI_NSSInternalSoft)); // 参数范围检查

  if (SPI_NSSInternalSoft != SPI_NSSInternalSoft_Reset)
  {
    /* 设置内部NSS为高电平：设置SSI位（位8） */
    SPIx->CR1 |= SPI_NSSInternalSoft_Set;  // 0x0100，模拟主机模式
  }
  else
  {
    /* 设置内部NSS为低电平：清除SSI位 */
    SPIx->CR1 &= SPI_NSSInternalSoft_Reset; // 使用掩码0xFEFF清除位8
  }
}
/**
  * @brief  SPI主模式NSS引脚输出控制函数
  * @功能  控制主设备模式下NSS引脚的电平输出状态，用于：
  *        - 自动管理多从机系统的片选信号
  *        - 硬件模式下的从机选择控制
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @参数  NewState: 新状态（ENABLE-自动拉低NSS，DISABLE-释放NSS控制）
  * @注意  该操作修改CR2寄存器的SSOE位（位2）：
  *        - 使能时：主设备自动控制NSS为低电平
  *        - 禁用时：NSS引脚由GPIO控制
  */
void SPI_SSOutputCmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));         // 检查SPI实例有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));   // 状态值有效性检查

  if (NewState != DISABLE)
  {
    /* 使能NSS引脚自动输出：设置CR2的SSOE位（位2） */
    SPIx->CR2 |= (uint16_t)SPI_CR2_SSOE;  // 0x0004，主模式下自动拉低NSS
  }
  else
  {
    /* 禁用NSS引脚自动输出：清除SSOE位 */
    SPIx->CR2 &= (uint16_t)~((uint16_t)SPI_CR2_SSOE); // 恢复GPIO控制NSS引脚
  }
}

/**
  * @brief  SPI TI帧格式模式控制函数
  * @功能  启用或禁用TI兼容的帧格式传输模式，该模式：
  *        - 使用特定的帧头识别机制
  *        - 支持多主设备总线管理
  *        - 自动配置SSM/SSI等控制位
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @参数  NewState: 新状态（ENABLE-启用TI模式，DISABLE-标准SPI模式）
  * @注意  该操作修改CR2寄存器的FRF位（位4）：
  *        - TI模式启用时忽略部分配置（SSM/SSI/CPOL/CPHA）
  *        - 模式切换后需重新配置相关参数
  */
void SPI_TIModeCmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* 参数检查 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));         // 外设实例验证
  assert_param(IS_FUNCTIONAL_STATE(NewState));   // 状态有效性验证

  if (NewState != DISABLE)
  {
    /* 启用TI帧格式模式：设置FRF位（位4） */
    SPIx->CR2 |= SPI_CR2_FRF;  // 0x0010，切换至TI协议模式
  }
  else
  {
    /* 恢复标准SPI模式：清除FRF位 */
    SPIx->CR2 &= (uint16_t)~SPI_CR2_FRF;  // 0xFFEF，返回Motorola模式
  }
}

/**
  * @brief  I2S全双工扩展配置函数
  * @功能  配置I2S扩展模块实现全双工通信，主要完成：
  *        - 主从模式自动配对（发送/接收互补）
  *        - 协议参数同步配置（标准/数据格式/时钟极性）
  *        - 扩展模块的从模式强制配置
  * @参数  I2Sxext: I2S扩展外设（仅支持I2S2ext/I2S3ext）
  * @参数  I2S_InitStruct: 主I2S模块的配置参数指针
  * @注意  扩展模块的配置特点：
  *        - 强制工作于从模式
  *        - 传输方向与主模块互补
  *        - 共享主模块的时钟和协议参数
  */
void I2S_FullDuplexConfig(SPI_TypeDef* I2Sxext, I2S_InitTypeDef* I2S_InitStruct)
{
  uint16_t tmpreg = 0, tmp = 0;
  
  /* 参数合法性验证 */
  assert_param(IS_I2S_EXT_PERIPH(I2Sxext));                // 检查扩展模块有效性
  assert_param(IS_I2S_MODE(I2S_InitStruct->I2S_Mode));    // 主模块模式验证
  assert_param(IS_I2S_STANDARD(I2S_InitStruct->I2S_Standard)); // 协议标准检查
  assert_param(IS_I2S_DATA_FORMAT(I2S_InitStruct->I2S_DataFormat)); // 数据格式检查
  assert_param(IS_I2S_CPOL(I2S_InitStruct->I2S_CPOL));     // 时钟极性验证

/*----------------------- I2S扩展模块寄存器初始化 -------------------------*/
  /* 清除I2SCFGR寄存器配置位（使用掩码0xF040） */
  I2Sxext->I2SCFGR &= I2SCFGR_CLEAR_MASK; 
  
  /* 初始化预分频寄存器为默认值（分频系数2） */
  I2Sxext->I2SPR = 0x0002;

  /* 获取当前I2SCFGR寄存器值用于后续配置 */
  tmpreg = I2Sxext->I2SCFGR;

  /* 根据主模块工作模式确定扩展模块方向 */
  if ((I2S_InitStruct->I2S_Mode == I2S_Mode_MasterTx) || 
      (I2S_InitStruct->I2S_Mode == I2S_Mode_SlaveTx))
  {
    /* 主模块为发送模式：扩展模块配置为从接收 */
    tmp = I2S_Mode_SlaveRx;  // 0x0200
  }
  else
  {
    if ((I2S_InitStruct->I2S_Mode == I2S_Mode_MasterRx) || 
        (I2S_InitStruct->I2S_Mode == I2S_Mode_SlaveRx))
    {
      /* 主模块为接收模式：扩展模块配置为从发送 */
      tmp = I2S_Mode_SlaveTx;  // 0x0100
    }
  }

  /* 组合配置参数到寄存器值 */
  // 强制I2S模式（位11）
  // 从模式方向配置
  // 协议标准（位5:4）
  // 数据格式（位2:1）
  tmpreg |= (uint16_t)(SPI_I2SCFGR_I2SMOD |
                      tmp |
                      I2S_InitStruct->I2S_Standard |
                      I2S_InitStruct->I2S_DataFormat |
                      I2S_InitStruct->I2S_CPOL);    // 时钟极性（位3）

  /* 应用配置到扩展模块 */
  I2Sxext->I2SCFGR = tmpreg;
}


/**
  * @}
  */

/** @defgroup SPI_Group2 Data transfers functions
 *  @brief   Data transfers functions
 *
@verbatim   
 ===============================================================================
                      ##### Data transfers functions #####
 ===============================================================================  

 [..] This section provides a set of functions allowing to manage the SPI data 
      transfers. In reception, data are received and then stored into an internal 
      Rx buffer while. In transmission, data are first stored into an internal Tx 
      buffer before being transmitted.

 [..] The read access of the SPI_DR register can be done using the SPI_I2S_ReceiveData()
      function and returns the Rx buffered value. Whereas a write access to the SPI_DR 
      can be done using SPI_I2S_SendData() function and stores the written data into 
      Tx buffer.

@endverbatim
  * @{
  */
/**
  * @brief  SPI/I2S数据接收函数
  * @功能  从指定外设的数据寄存器(DR)中读取最新接收到的数据
  * @参数  SPIx: 目标外设（支持SPI1-6，I2S2/3及其扩展全双工模式）
  * @返回值 uint16_t: 从数据寄存器读取的16位数据
  * @注意  该函数：
  *        - 直接访问DR寄存器，不检查RXNE状态标志
  *        - 在接收缓冲区为空时读取将返回未定义值
  *        - 适用于查询式和中断式数据接收
  */
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx)
{
  /* 验证外设实例有效性（支持SPI全系列和I2S扩展） */
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));
  
  /* 直接返回数据寄存器值（硬件自动维护双缓冲机制） */
  return SPIx->DR;  // 读取操作会自动清除RXNE状态标志
}

/**
  * @brief  SPI/I2S数据发送函数
  * @功能  向指定外设的数据寄存器(DR)写入待发送数据
  * @参数  SPIx: 目标外设（支持SPI1-6，I2S2/3及其扩展全双工模式）
  * @参数  Data: 要发送的16位数据（实际位数由DFF位配置决定）
  * @注意  该函数：
  *        - 直接写入DR寄存器，不检查TXE状态标志
  *        - 在发送缓冲区未就绪时写入会覆盖未发送数据
  *        - 数据宽度自动适配（8位时高字节被忽略）
  */
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data)
{
  /* 验证外设实例有效性 */
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));
  
  /* 将数据写入发送缓冲区（触发发送流程） */
  SPIx->DR = Data;  // 写入操作会置位TXE标志（当数据转移到移位寄存器后）
}


/**
  * @}
  */

/** @defgroup SPI_Group3 Hardware CRC Calculation functions
 *  @brief   Hardware CRC Calculation functions
 *
@verbatim   
 ===============================================================================
                 ##### Hardware CRC Calculation functions #####
 ===============================================================================  

 [..] This section provides a set of functions allowing to manage the SPI CRC hardware 
      calculation

 [..] SPI communication using CRC is possible through the following procedure:
   (#) Program the Data direction, Polarity, Phase, First Data, Baud Rate Prescaler, 
       Slave Management, Peripheral Mode and CRC Polynomial values using the SPI_Init()
       function.
   (#) Enable the CRC calculation using the SPI_CalculateCRC() function.
   (#) Enable the SPI using the SPI_Cmd() function
   (#) Before writing the last data to the TX buffer, set the CRCNext bit using the 
       SPI_TransmitCRC() function to indicate that after transmission of the last 
       data, the CRC should be transmitted.
   (#) After transmitting the last data, the SPI transmits the CRC. The SPI_CR1_CRCNEXT
        bit is reset. The CRC is also received and compared against the SPI_RXCRCR 
        value. 
        If the value does not match, the SPI_FLAG_CRCERR flag is set and an interrupt
        can be generated when the SPI_I2S_IT_ERR interrupt is enabled.

 [..]
   (@) It is advised not to read the calculated CRC values during the communication.

   (@) When the SPI is in slave mode, be careful to enable CRC calculation only 
       when the clock is stable, that is, when the clock is in the steady state. 
       If not, a wrong CRC calculation may be done. In fact, the CRC is sensitive 
       to the SCK slave input clock as soon as CRCEN is set, and this, whatever 
       the value of the SPE bit.

   (@) With high bitrate frequencies, be careful when transmitting the CRC.
       As the number of used CPU cycles has to be as low as possible in the CRC 
       transfer phase, it is forbidden to call software functions in the CRC 
       transmission sequence to avoid errors in the last data and CRC reception. 
       In fact, CRCNEXT bit has to be written before the end of the transmission/reception 
       of the last data.

   (@) For high bit rate frequencies, it is advised to use the DMA mode to avoid the
       degradation of the SPI speed performance due to CPU accesses impacting the 
       SPI bandwidth.

   (@) When the STM32F4xx is configured as slave and the NSS hardware mode is 
       used, the NSS pin needs to be kept low between the data phase and the CRC 
       phase.

   (@) When the SPI is configured in slave mode with the CRC feature enabled, CRC
       calculation takes place even if a high level is applied on the NSS pin. 
       This may happen for example in case of a multi-slave environment where the 
       communication master addresses slaves alternately.

   (@) Between a slave de-selection (high level on NSS) and a new slave selection 
       (low level on NSS), the CRC value should be cleared on both master and slave
       sides in order to resynchronize the master and slave for their respective 
       CRC calculation.

   (@) To clear the CRC, follow the procedure below:
       (#@) Disable SPI using the SPI_Cmd() function
       (#@) Disable the CRC calculation using the SPI_CalculateCRC() function.
       (#@) Enable the CRC calculation using the SPI_CalculateCRC() function.
       (#@) Enable SPI using the SPI_Cmd() function.

@endverbatim
  * @{
  */
/**
  * @brief  SPI CRC计算功能控制函数
  * @功能  启用或禁用硬件CRC校验功能，影响：
  *        - 发送端自动附加CRC校验码
  *        - 接收端自动校验CRC
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @参数  NewState: 新状态（ENABLE-启用CRC校验，DISABLE-禁用）
  * @注意  该操作修改CR1寄存器的CRCEN位（位3）：
  *        - 启用后需先配置CRC多项式寄存器
  *        - 模式切换时会重置CRC计算器
  */
void SPI_CalculateCRC(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));         // 外设实例检查
  assert_param(IS_FUNCTIONAL_STATE(NewState));   // 状态有效性检查

  if (NewState != DISABLE)
  {
    /* 启用CRC计算：设置CRCEN位（位3） */
    SPIx->CR1 |= SPI_CR1_CRCEN;  // 0x0008，同时激活收发端CRC处理
  }
  else
  {
    /* 禁用CRC计算：清除CRCEN位 */
    SPIx->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_CRCEN); // 停止所有CRC相关操作
  }
}

/**
  * @brief  SPI CRC强制传输函数
  * @功能  在下次数据传输时附加CRC校验码，用于：
  *        - 手动触发CRC校验序列
  *        - 主设备发送CRC值前预处理
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @注意  该操作修改CR1寄存器的CRCNEXT位（位4）：
  *        - 单次生效，发送后自动清除
  *        - 需确保CRC功能已启用
  */
void SPI_TransmitCRC(SPI_TypeDef* SPIx)
{
  /* 参数检查 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));  // 检查外设有效性
  
  /* 设置下次传输发送CRC：置位CRCNEXT位（位4） */
  SPIx->CR1 |= SPI_CR1_CRCNEXT;  // 0x0010，触发CRC发送序列
}

/**
  * @brief  SPI CRC寄存器读取函数
  * @功能  获取最近计算的CRC校验值，支持：
  *        - 发送端CRC值（TXCRCR寄存器）
  *        - 接收端CRC值（RXCRCR寄存器）
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @参数  SPI_CRC: 选择CRC寄存器（SPI_CRC_Tx/Rx）
  * @返回值 uint16_t: 对应CRC寄存器的16位值
  * @注意  接收CRC值仅在校验失败时有效
  */
uint16_t SPI_GetCRC(SPI_TypeDef* SPIx, uint8_t SPI_CRC)
{
  uint16_t crcreg = 0;
  /* 参数验证 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));       // 外设有效性检查
  assert_param(IS_SPI_CRC(SPI_CRC));           // CRC类型参数检查

  if (SPI_CRC != SPI_CRC_Rx)
  {
    /* 获取发送端CRC（最后计算的输出CRC） */
    crcreg = SPIx->TXCRCR;  // 保存最近发送数据计算的CRC值
  }
  else
  {
    /* 获取接收端CRC（来自最后接收的数据包） */
    crcreg = SPIx->RXCRCR;  // 存储实际接收数据的CRC计算结果
  }
  return crcreg;
}

/**
  * @brief  SPI CRC多项式获取函数
  * @功能  读取当前配置的CRC多项式寄存器(CRCPR)值
  * @参数  SPIx: 目标SPI外设（SPI1-SPI6）
  * @返回值 uint16_t: 配置的CRC多项式值
  * @注意  标准多项式通常为：
  *        - CRC-16: 0x1021
  *        - CRC-8:  0x107（移位后0x07）
  */
uint16_t SPI_GetCRCPolynomial(SPI_TypeDef* SPIx)
{
  /* 参数检查 */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));  // 确认外设有效性
  
  /* 返回配置的CRC多项式（多项式宽度由DFF位决定） */
  return SPIx->CRCPR;  // 存储当前用于计算的CRC多项式
}

/**
  * @}
  */

/** @defgroup SPI_Group4 DMA transfers management functions
 *  @brief   DMA transfers management functions
  *
@verbatim   
 ===============================================================================
                   ##### DMA transfers management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the SPIx/I2Sx DMA interface.
  * @param  SPIx: To select the SPIx/I2Sx peripheral, where x can be: 1, 2, 3, 4, 5 or 6 
  *         in SPI mode or 2 or 3 in I2S mode or I2Sxext for I2S full duplex mode. 
  * @param  SPI_I2S_DMAReq: specifies the SPI DMA transfer request to be enabled or disabled. 
  *          This parameter can be any combination of the following values:
  *            @arg SPI_I2S_DMAReq_Tx: Tx buffer DMA transfer request
  *            @arg SPI_I2S_DMAReq_Rx: Rx buffer DMA transfer request
  * @param  NewState: new state of the selected SPI DMA transfer request.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  SPI/I2S DMA请求控制函数
  * @功能  启用或禁用指定方向的DMA传输请求，支持：
  *        - 发送缓冲区空时触发DMA请求(TXE)
  *        - 接收缓冲区非空时触发DMA请求(RXNE)
  * @参数  SPIx: 目标外设（支持SPI1-6，I2S2/3及其扩展模式）
  * @参数  SPI_I2S_DMAReq: DMA请求方向选择，可选：
  *            @arg SPI_I2S_DMAReq_Tx: 发送方向DMA请求（CR2位1）
  *            @arg SPI_I2S_DMAReq_Rx: 接收方向DMA请求（CR2位0）
  * @参数  NewState: 新状态（ENABLE-启用DMA请求，DISABLE-禁用）
  * @注意  该操作直接影响CR2寄存器的TXDMAEN/RXDMAEN位：
  *        - 启用DMA前需正确配置DMA通道
  *        - 需与DMA控制器中断配合使用
  */
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState)
{
  /* 三重参数合法性检查 */
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));         // 验证支持全双工扩展实例
  assert_param(IS_FUNCTIONAL_STATE(NewState));       // 状态值有效性检查
  assert_param(IS_SPI_I2S_DMAREQ(SPI_I2S_DMAReq));   // 检查请求方向参数合法性（0x0001/0x0002）

  if (NewState != DISABLE)
  {
    /* 启用指定方向DMA请求：设置CR2对应控制位 */
    SPIx->CR2 |= SPI_I2S_DMAReq;  // 位操作：0x0001(RX) 或 0x0002(TX)
  }
  else
  {
    /* 禁用指定方向DMA请求：清除CR2对应控制位 */
    SPIx->CR2 &= (uint16_t)~SPI_I2S_DMAReq; // 位掩码取反操作（例：~0x0002=0xFFFD）
  }
}

/**
  * @}
  */

/** @defgroup SPI_Group5 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
  *
@verbatim   
 ===============================================================================
            ##### Interrupts and flags management functions #####
 ===============================================================================  
 
 [..] This section provides a set of functions allowing to configure the SPI Interrupts 
      sources and check or clear the flags or pending bits status.
      The user should identify which mode will be used in his application to manage 
      the communication: Polling mode, Interrupt mode or DMA mode. 
    
 *** Polling Mode ***
 ====================
[..] In Polling Mode, the SPI/I2S communication can be managed by 9 flags:
  (#) SPI_I2S_FLAG_TXE : to indicate the status of the transmit buffer register
  (#) SPI_I2S_FLAG_RXNE : to indicate the status of the receive buffer register
  (#) SPI_I2S_FLAG_BSY : to indicate the state of the communication layer of the SPI.
  (#) SPI_FLAG_CRCERR : to indicate if a CRC Calculation error occur              
  (#) SPI_FLAG_MODF : to indicate if a Mode Fault error occur
  (#) SPI_I2S_FLAG_OVR : to indicate if an Overrun error occur
  (#) I2S_FLAG_TIFRFE: to indicate a Frame Format error occurs.
  (#) I2S_FLAG_UDR: to indicate an Underrun error occurs.
  (#) I2S_FLAG_CHSIDE: to indicate Channel Side.

  (@) Do not use the BSY flag to handle each data transmission or reception. It is
      better to use the TXE and RXNE flags instead.

 [..] In this Mode it is advised to use the following functions:
   (+) FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
   (+) void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);

 *** Interrupt Mode ***
 ======================
 [..] In Interrupt Mode, the SPI communication can be managed by 3 interrupt sources
      and 7 pending bits: 
   (+) Pending Bits:
       (##) SPI_I2S_IT_TXE : to indicate the status of the transmit buffer register
       (##) SPI_I2S_IT_RXNE : to indicate the status of the receive buffer register
       (##) SPI_IT_CRCERR : to indicate if a CRC Calculation error occur (available in SPI mode only)            
       (##) SPI_IT_MODF : to indicate if a Mode Fault error occur (available in SPI mode only)
       (##) SPI_I2S_IT_OVR : to indicate if an Overrun error occur
       (##) I2S_IT_UDR : to indicate an Underrun Error occurs (available in I2S mode only).
       (##) I2S_FLAG_TIFRFE : to indicate a Frame Format error occurs (available in TI mode only).

   (+) Interrupt Source:
       (##) SPI_I2S_IT_TXE: specifies the interrupt source for the Tx buffer empty 
            interrupt.  
       (##) SPI_I2S_IT_RXNE : specifies the interrupt source for the Rx buffer not 
            empty interrupt.
       (##) SPI_I2S_IT_ERR : specifies the interrupt source for the errors interrupt.

 [..] In this Mode it is advised to use the following functions:
   (+) void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
   (+) ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
   (+) void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);

 *** DMA Mode ***
 ================
 [..] In DMA Mode, the SPI communication can be managed by 2 DMA Channel requests:
   (#) SPI_I2S_DMAReq_Tx: specifies the Tx buffer DMA transfer request
   (#) SPI_I2S_DMAReq_Rx: specifies the Rx buffer DMA transfer request

 [..] In this Mode it is advised to use the following function:
   (+) void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState 
       NewState);

@endverbatim
  * @{
  */
/**
  * @brief  SPI/I2S中断源配置函数
  * @功能  启用或禁用指定的中断源，涵盖：
  *        - 发送缓冲区空中断（TXE）
  *        - 接收缓冲区非空中断（RXNE）
  *        - 错误中断（包括溢出、模式错误等）
  * @参数  SPIx: 目标外设（支持SPI1-6，I2S全双工扩展）
  * @参数  SPI_I2S_IT: 中断类型选择，可选：
  *            @arg SPI_I2S_IT_TXE: 发送中断（CR2位7）
  *            @arg SPI_I2S_IT_RXNE: 接收中断（CR2位6）
  *            @arg SPI_I2S_IT_ERR: 错误中断（CR2位5）
  * @参数  NewState: 新状态（ENABLE-启用中断，DISABLE-禁用）
  * @注意  中断参数需右移4位获取实际位位置（例：0x70>>4=7）
  *        启用中断后需配置NVIC控制器
  */
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState)
{
  uint16_t itpos = 0, itmask = 0 ;
  
  /* 三重参数验证 */
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));         // 支持全双工扩展实例
  assert_param(IS_FUNCTIONAL_STATE(NewState));       // 状态有效性检查
  assert_param(IS_SPI_I2S_CONFIG_IT(SPI_I2S_IT));   // 中断类型合法性检查（0x70/0x60/0x50）

  /* 计算中断位位置：参数值右移4位（0x70->0x07） */
  itpos = SPI_I2S_IT >> 4;  // 获取实际位偏移（7/6/5）

  /* 生成位掩码：1左移计算结果位 */
  itmask = (uint16_t)1 << itpos;  // 例：位7对应0x0080

  if (NewState != DISABLE)
  {
    /* 启用中断：设置CR2对应位 */
    SPIx->CR2 |= itmask;  // 置位操作（例：0x0080使能TXEIE）
  }
  else
  {
    /* 禁用中断：清除CR2对应位 */
    SPIx->CR2 &= (uint16_t)~itmask; // 掩码取反操作（例：~0x0080=0xFF7F）
  }
}

/**
  * @brief  SPI/I2S状态标志获取函数
  * @功能  检测指定的状态标志位是否置位，包括：
  *        - 传输状态标志（BSY）
  *        - 错误标志（OVR, MODF, CRCERR等）
  *        - 数据状态标志（TXE, RXNE）
  * @参数  SPIx: 目标外设（支持SPI1-6，I2S全双工扩展）
  * @参数  SPI_I2S_FLAG: 要检测的状态标志，可选：
  *            @arg SPI_I2S_FLAG_TXE:  发送寄存器空（SR位1）
  *            @arg SPI_I2S_FLAG_RXNE: 接收寄存器非空（SR位0）
  *            @arg SPI_I2S_FLAG_BSY:  总线忙（SR位7）
  *            @arg SPI_I2S_FLAG_OVR:  溢出错误（SR位6）
  *            @arg SPI_FLAG_MODF:     模式错误（SR位5）
  *            @arg SPI_FLAG_CRCERR:   CRC错误（SR位4）
  * @返回值 FlagStatus: 标志状态（SET-置位，RESET-未置位）
  * @注意  读取BSY标志时需禁用外设防止竞态条件
  */
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* 双重参数验证 */
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));              // 外设实例检查
  assert_param(IS_SPI_I2S_GET_FLAG(SPI_I2S_FLAG));        // 标志合法性检查

  /* 通过SR寄存器检测标志位 */
  if ((SPIx->SR & SPI_I2S_FLAG) != (uint16_t)RESET)
  {
    bitstatus = SET;  // 标志置位（对应硬件事件发生）
  }
  else
  {
    bitstatus = RESET; // 标志未置位（无事件发生）
  }
  return bitstatus;
}

/**
  * @brief  SPI/I2S状态标志清除函数
  * @功能  清除指定的状态标志位，当前仅支持清除CRC错误标志(CRCERR)
  * @参数  SPIx: 目标外设（支持SPI1-6，I2S全双工扩展）
  * @参数  SPI_I2S_FLAG: 需清除的标志，当前仅支持：
  *            @arg SPI_FLAG_CRCERR: CRC校验错误标志（SR位4）
  * @注意  其他标志清除方法：
  *        - OVR标志：先读DR再读SR
  *        - MODF标志：读SR后重新使能SPI
  *        - UDR标志：直接读SR
  */
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG)
{
  /* 双重参数检查 */
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));          // 验证外设实例有效性
  assert_param(IS_SPI_I2S_CLEAR_FLAG(SPI_I2S_FLAG)); // 仅允许CRCERR标志（0x0010）

  /* 清除CRC错误标志：向SR寄存器写入标志位的反码 */
  SPIx->SR = (uint16_t)~SPI_I2S_FLAG;  // 例：CRCERR=0x0010，写入0xFFEF
}

/**
  * @brief  SPI/I2S中断状态检查函数
  * @功能  检测指定中断是否触发且被使能，综合判断中断状态
  * @参数  SPIx: 目标外设（支持SPI1-6，I2S全双工扩展）
  * @参数  SPI_I2S_IT: 中断类型，可选：
  *            @arg SPI_I2S_IT_TXE:   发送中断（SR位1 & CR2位7）
  *            @arg SPI_I2S_IT_RXNE:  接收中断（SR位0 & CR2位6）
  *            @arg SPI_I2S_IT_OVR:   溢出中断（SR位6 & CR2位5）
  * @返回值 ITStatus: 中断状态（SET-中断有效，RESET-未触发或未使能）
  * @注意  中断参数编码规则：
  *        - 低4位表示SR寄存器位位置
  *        - 高4位表示CR2寄存器使能位位置
  */
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT)
{
  ITStatus bitstatus = RESET;
  uint16_t itpos = 0, itmask = 0, enablestatus = 0;

  /* 参数验证 */
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));          // 外设实例检查
  assert_param(IS_SPI_I2S_GET_IT(SPI_I2S_IT));        // 中断类型合法性检查

  /* 计算SR检测位：参数低4位决定 */
  itpos = 0x01 << (SPI_I2S_IT & 0x0F);  // 例：TXE中断参数0x71 → 0x01<<1=0x0002

  /* 计算CR2使能位：参数高4位决定 */
  itmask = SPI_I2S_IT >> 4;             // 0x71>>4=0x07
  itmask = 0x01 << itmask;              // 0x01<<7=0x0080（CR2_TXEIE位）

  /* 检查中断使能状态 */
  enablestatus = (SPIx->CR2 & itmask);  // 获取CR2对应中断使能位状态

  /* 综合判断中断状态（标志置位且使能有效） */
  if (((SPIx->SR & itpos) != 0) && enablestatus) 
  {
    bitstatus = SET;  // 中断有效（已触发且被使能）
  }
  else
  {
    bitstatus = RESET; // 中断未触发或未使能
  }
  return bitstatus;
}

/**
  * @brief  SPI/I2S中断挂起位清除函数
  * @功能  清除指定的中断挂起标志，当前仅支持CRC错误中断
  * @参数  SPIx: 目标外设（支持SPI1-6，I2S全双工扩展）
  * @参数  SPI_I2S_IT: 中断类型，仅支持：
  *            @arg SPI_IT_CRCERR: CRC错误中断（SR位4）
  * @注意  其他中断清除方法：
  *        - OVR中断：读DR后读SR
  *        - UDR中断：直接读SR
  *        - MODF中断：读SR后重新使能SPI
  */
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT)
{
  uint16_t itpos = 0;
  /* 参数检查 */
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));        // 外设实例验证
  assert_param(IS_SPI_I2S_CLEAR_IT(SPI_I2S_IT));    // 仅允许CRCERR中断（0x14）

  /* 计算中断位位置：参数低4位决定 */
  itpos = 0x01 << (SPI_I2S_IT & 0x0F);  // 0x14→0x04→0x01<<4=0x0010

  /* 清除中断挂起位：向SR写入位反码 */
  SPIx->SR = (uint16_t)~itpos;  // 写入0xFFEF清除CRCERR标志
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

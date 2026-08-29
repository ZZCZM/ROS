/**
  ******************************************************************************
  * @file    stm32f4xx_sai.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014  
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Serial Audio Interface (SAI):
  *           + Initialization and Configuration
  *           + Data transfers functions
  *           + DMA transfers management
  *           + Interrupts and flags management 
  *           
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..] 
    
       (#) Enable peripheral clock using the following functions 
           RCC_APB2PeriphClockCmd(RCC_APB2Periph_SAI1, ENABLE) for SAI1
  
       (#) For each SAI Block A/B enable SCK, SD, FS and MCLK GPIO clocks 
           using RCC_AHB1PeriphClockCmd() function.
  
       (#) Peripherals alternate function: 
           (++) Connect the pin to the desired peripherals' Alternate 
                Function (AF) using GPIO_PinAFConfig() function.
           (++) Configure the desired pin in alternate function by:
                GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
           (++) Select the type, pull-up/pull-down and output speed via 
                GPIO_PuPd, GPIO_OType and GPIO_Speed members
           (++) Call GPIO_Init() function
           -@@- If an external clock source is used then the I2S CKIN pin should be 
               also configured in Alternate function Push-pull pull-up mode.
                
      (#) The SAI clock can be generated from different clock source :
          PLL I2S, PLL SAI or external clock source.
          (++) The PLL I2S is configured using the following functions RCC_PLLI2SConfig(), 
               RCC_PLLI2SCmd(ENABLE), RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) and 
               RCC_SAIPLLI2SClkDivConfig() or;
              
          (++) The PLL SAI is configured using the following functions RCC_PLLSAIConfig(), 
               RCC_PLLSAICmd(ENABLE), RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) and 
               RCC_SAIPLLSAIClkDivConfig()or;          
              
          (++) External clock source is configured using the function 
               RCC_I2SCLKConfig(RCC_I2S2CLKSource_Ext) and after setting correctly the 
               define constant I2S_EXTERNAL_CLOCK_VAL in the stm32f4xx_conf.h file.      
                
      (#) Each SAI Block A or B has its own clock generator to make these two blocks 
          completely independent. The Clock generator is configured using RCC_SAIBlockACLKConfig() and 
          RCC_SAIBlockBCLKConfig() functions.
                  
      (#) Each SAI Block A or B can be configured separetely : 
          (++) Program the Master clock divider, Audio mode, Protocol, Data Length, Clock Strobing Edge, 
               Synchronous mode, Output drive and FIFO Thresold using SAI_Init() function.   
               In case of master mode, program the Master clock divider (MCKDIV) using 
               the following formula :  
               (+++) MCLK_x = SAI_CK_x / (MCKDIV * 2) with MCLK_x = 256 * FS
               (+++) FS = SAI_CK_x / (MCKDIV * 2) * 256
               (+++) MCKDIV = SAI_CK_x / FS * 512
         (++) Program the Frame Length, Frame active Length, FS Definition, FS Polarity, 
              FS Offset using SAI_FrameInit() function.    
         (++) Program the Slot First Bit Offset, Slot Size, Slot Number, Slot Active 
              using SAI_SlotInit() function. 
                   
      (#) Enable the NVIC and the corresponding interrupt using the function 
          SAI_ITConfig() if you need to use interrupt mode. 
  
      (#) When using the DMA mode 
          (++) Configure the DMA using DMA_Init() function
          (++) Active the needed channel Request using SAI_DMACmd() function
   
      (#) Enable the SAI using the SAI_Cmd() function.
   
      (#) Enable the DMA using the DMA_Cmd() function when using DMA mode. 
  
      (#) The SAI has some specific functions which can be useful depending 
          on the audio protocol selected.  
          (++) Enable Mute mode when the audio block is a transmitter using SAI_MuteModeCmd()
               function and configure the value transmitted during mute using SAI_MuteValueConfig().  
          (++) Detect the Mute mode when audio block is a receiver using SAI_MuteFrameCounterConfig().             
          (++) Enable the MONO mode without any data preprocessing in memory when the number
               of slot is equal to 2 using SAI_MonoModeConfig() function.
          (++) Enable data companding algorithm (U law and A law) using SAI_CompandingModeConfig().
          (++) Choose the behavior of the SD line in output when an inactive slot is sent 
               on the data line using SAI_TRIStateConfig() function.   
  [..]               
   (@)    In master TX mode: enabling the audio block immediately generates the bit clock 
          for the external slaves even if there is no data in the FIFO, However FS signal 
          generation is conditioned by the presence of data in the FIFO.
                 
   (@)    In master RX mode: enabling the audio block immediately generates the bit clock 
          and FS signal for the external slaves. 
                
   (@)    It is mandatory to respect the following conditions in order to avoid bad SAI behavior: 
            (+@)  First bit Offset <= (SLOT size - Data size)
            (+@)  Data size <= SLOT size
            (+@)  Number of SLOT x SLOT size = Frame length
            (+@)  The number of slots should be even when bit FSDEF in the SAI_xFRCR is set.    
  
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
#include "stm32f4xx_sai.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup SAI 
  * @brief SAI driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* ------------ SAI寄存器位操作掩码定义 ----------- */

/* 
 * SAI控制寄存器1（CR1）清除掩码
 * 掩码值：0xFF07C010（二进制1111 1111 0000 0111 1100 0000 0001 0000）
 * 作用：清除关键配置位时保留寄存器其他设置
 * 清除的位域包含：
 *   位4:    SYNCEN[1:0]  - 同步使能配置
 *   位8:    MCKDIV[2:0]  - 主时钟分频器
 *   位16-20: OSLR[3:0]    - 输出驱动强度
 *   位23:   DMAEN        - DMA使能位
 * 应用场景：修改音频模式前清空原配置
 */
#define CR1_CLEAR_MASK            ((uint32_t)0xFF07C010)

/* 
 * SAI帧配置寄存器（FRCR）清除掩码
 * 掩码值：0xFFF88000（二进制1111 1111 1111 1000 1000 0000 0000 0000）
 * 作用：重置帧结构相关配置时保留参数
 * 清除的位域包含：
 *   位19-21: FSALL[6:0]  - 帧同步有效长度
 *   位22:    FSPOL       - 帧同步极性
 *   位23-24: FSDEF       - 帧同步定义模式
 * 典型应用：切换采样率前初始化帧参数
 */
#define FRCR_CLEAR_MASK           ((uint32_t)0xFFF88000)

/* 
 * SAI时隙寄存器（SLOTR）清除掩码
 * 掩码值：0x0000F020（二进制0000 0000 0000 0000 1111 0000 0010 0000）
 * 作用：清除时隙分配配置
 * 清除的位域包含：
 *   位5:     SLOTEN      - 基础时隙使能
 *   位12-15: NBSLOT[3:0] - 激活的时隙数量
 *   位16-19: SLOTSZ[1:0] - 时隙位宽配置
 * 操作说明：切换多声道配置时必须使用此掩码
 */
#define SLOTR_CLEAR_MASK          ((uint32_t)0x0000F020)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup SAI_Private_Functions
  * @{
  */

/** @defgroup SAI_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================  
  [..]
  This section provides a set of functions allowing to initialize the SAI Audio 
  Block Mode, Audio Protocol, Data size, Synchronization between audio block, 
  Master clock Divider, Fifo threshold, Frame configuration, slot configuration,
  Tristate mode, Companding mode and Mute mode.  
  [..] 
  The SAI_Init(), SAI_FrameInit() and SAI_SlotInit() functions follows the SAI Block
  configuration procedures for Master mode and Slave mode (details for these procedures 
  are available in reference manual(RM0090).
  
@endverbatim
  * @{
  */
/**
  * @brief  复位SAI外设至默认状态
  * @param  SAIx: SAI实例，支持SAI1等实例（具体由芯片型号决定）
  * @retval 无
  * @note   通过RCC外设控制器的复位功能实现硬复位
  *         执行流程：置位复位->延迟->解除复位
  */
void SAI_DeInit(SAI_TypeDef* SAIx)
{
  /* 验证输入参数是否为有效的SAI实例 */
  assert_param(IS_SAI_PERIPH(SAIx));

  /* 使能SAI1的复位状态（启动硬复位）*/
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_SAI1, ENABLE);
  /* 解除SAI1的复位状态（复位完成后恢复运行）*/
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_SAI1, DISABLE);  
}

/**
  * @brief  初始化SAI音频模块配置
  * @param  SAI_Block_x: SAI子模块，A或B块（SAI_Block_A / SAI_Block_B）
  * @param  SAI_InitStruct: 初始化配置结构体指针
  *         - SAI_AudioMode:   主/从模式配置
  *         - SAI_Protocol:    音频协议（I2S/LSB/MSB等）
  *         - SAI_DataSize:    数据位宽（16/24/32位等） 
  *         - SAI_FirstBit:   位序（MSB优先/LSB优先）
  *         - SAI_ClockStrobing: 时钟触发边沿
  *         - SAI_Synchro:     同步模式（内部/外部同步）
  *         - SAI_OUTDRIV:    输出驱动模式
  *         - SAI_NoDivider:   是否旁路分频器
  *         - SAI_MasterDivider:主时钟分频系数 
  *         - SAI_FIFOThreshold:FIFO阈值设置
  * @retval 无
  * @note   该函数将按照配置结构体参数初始化SAI模块的：
  *         - 工作模式     - 时钟配置
  *         - 数据传输格式 - FIFO设置
  *         - 同步机制     - 驱动特性
  */
void SAI_Init(SAI_Block_TypeDef* SAI_Block_x, SAI_InitTypeDef* SAI_InitStruct)
{
  uint32_t tmpreg = 0;
  
  /* 验证SAI子模块有效性（A/B块） */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  
  /* 验证所有输入参数的合法性 */
  assert_param(IS_SAI_BLOCK_MODE(SAI_InitStruct->SAI_AudioMode));       // 工作模式
  assert_param(IS_SAI_BLOCK_PROTOCOL(SAI_InitStruct->SAI_Protocol));    // 协议类型
  assert_param(IS_SAI_BLOCK_DATASIZE(SAI_InitStruct->SAI_DataSize));     // 数据宽度
  assert_param(IS_SAI_BLOCK_FIRST_BIT(SAI_InitStruct->SAI_FirstBit));    // 位序设置
  assert_param(IS_SAI_BLOCK_CLOCK_STROBING(SAI_InitStruct->SAI_ClockStrobing));//时钟边沿
  assert_param(IS_SAI_BLOCK_SYNCHRO(SAI_InitStruct->SAI_Synchro));       // 同步模式
  assert_param(IS_SAI_BLOCK_OUTPUT_DRIVE(SAI_InitStruct->SAI_OUTDRIV));  // 驱动强度
  assert_param(IS_SAI_BLOCK_NODIVIDER(SAI_InitStruct->SAI_NoDivider));  // 分频旁路
  assert_param(IS_SAI_BLOCK_MASTER_DIVIDER(SAI_InitStruct->SAI_MasterDivider));//主时钟分频
  assert_param(IS_SAI_BLOCK_FIFO_THRESHOLD(SAI_InitStruct->SAI_FIFOThreshold));//FIFO阈值

  /* 配置控制寄存器1（CR1） */
  tmpreg = SAI_Block_x->CR1;  // 获取当前寄存器值
  /* 清除关键配置位域（保留非配置相关位） */
  tmpreg &= CR1_CLEAR_MASK;    // 使用预定义掩码0xFF07C010
  
  /* 组合新的配置参数（将结构体参数转换为寄存器位模式）*/
  tmpreg |= (uint32_t)(SAI_InitStruct->SAI_AudioMode      | // 模式位[0:1]
                       SAI_InitStruct->SAI_Protocol       | // 协议位[2:3] 
                       SAI_InitStruct->SAI_DataSize       | // 数据位宽位[5:6]
                       SAI_InitStruct->SAI_FirstBit       | // 位序位[7]
                       SAI_InitStruct->SAI_ClockStrobing  | // 时钟边沿位[8]
                       SAI_InitStruct->SAI_Synchro        | // 同步位[10:11]
                       SAI_InitStruct->SAI_OUTDRIV        | // 驱动模式位[12]
                       SAI_InitStruct->SAI_NoDivider      | // 分频旁路位[18]
                       ((SAI_InitStruct->SAI_MasterDivider) << 20)); // 分频系数位[20:23]
  
  SAI_Block_x->CR1 = tmpreg;  // 写入配置后的CR1寄存器

  /* 配置控制寄存器2（CR2）- FIFO控制 */
  tmpreg = SAI_Block_x->CR2;            // 获取当前寄存器值
  tmpreg &= ~(SAI_xCR2_FTH);            // 清空FIFO阈值位[0:2]
  tmpreg |= SAI_InitStruct->SAI_FIFOThreshold; // 设置新的FIFO阈值
  SAI_Block_x->CR2 = tmpreg;            // 写入配置后的CR2寄存器
}
/**
  * @brief  初始化SAI音频帧结构配置
  * @param  SAI_Block_x: SAI子模块(A/B块)
  * @param  SAI_FrameInitStruct: 帧配置结构体指针，包含：
  *         - SAI_FrameLength:      总帧长度（单位：槽位数）
  *         - SAI_ActiveFrameLength: 有效数据槽位数
  *         - SAI_FSDefinition:     帧同步信号生成方式
  *         - SAI_FSPolarity:      帧同步极性（高/低有效）
  *         - SAI_FSOffset:       帧同步偏移位置（帧开始前/后）
  * @retval 无
  * @note   适用于I2S/PCM等协议，对AC'97和SPDIF协议无效
  */
void SAI_FrameInit(SAI_Block_TypeDef* SAI_Block_x, SAI_FrameInitTypeDef* SAI_FrameInitStruct)
{
  uint32_t tmpreg = 0;
  
  /* 验证SAI子模块有效性 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  
  /* 验证帧参数合法性 */
  assert_param(IS_SAI_BLOCK_FRAME_LENGTH(SAI_FrameInitStruct->SAI_FrameLength));       // 帧总长度[1-256]
  assert_param(IS_SAI_BLOCK_ACTIVE_FRAME(SAI_FrameInitStruct->SAI_ActiveFrameLength));// 有效帧长度[1-128]
  assert_param(IS_SAI_BLOCK_FS_DEFINITION(SAI_FrameInitStruct->SAI_FSDefinition));     // 同步信号生成方式
  assert_param(IS_SAI_BLOCK_FS_POLARITY(SAI_FrameInitStruct->SAI_FSPolarity));         // 同步极性
  assert_param(IS_SAI_BLOCK_FS_OFFSET(SAI_FrameInitStruct->SAI_FSOffset));            // 同步偏移

  /* 配置帧控制寄存器(FRCR) */
  tmpreg = SAI_Block_x->FRCR;       // 获取当前寄存器值
  tmpreg &= FRCR_CLEAR_MASK;       // 使用预定义掩码0xFFF88000清空配置位
  
  /* 组合新的帧配置（带偏移和运算）*/
  tmpreg |= (uint32_t)( (SAI_FrameInitStruct->SAI_FrameLength - 1)       | // 总帧长位[0:7] (值范围0-255)
                       SAI_FrameInitStruct->SAI_FSOffset                | // 同步偏移位[16]
                       SAI_FrameInitStruct->SAI_FSDefinition          | // 同步定义位[17] 
                       SAI_FrameInitStruct->SAI_FSPolarity             | // 同步极性位[18]
                       ((SAI_FrameInitStruct->SAI_ActiveFrameLength - 1) << 8) ); // 有效帧位[8:14]
  
  SAI_Block_x->FRCR = tmpreg;      // 写入配置后的FRCR寄存器
}

/**
  * @brief  初始化SAI音频时隙结构配置
  * @param  SAI_Block_x: SAI子模块(A/B块)
  * @param  SAI_SlotInitStruct: 时隙配置结构体指针，包含：
  *         - SAI_FirstBitOffset: 首有效位偏移（0-31）
  *         - SAI_SlotSize:       单个时隙位宽（16/32位） 
  *         - SAI_SlotNumber:     总时隙数量（1-16）
  *         - SAI_SlotActive:     激活的时隙位图（0x0000-0xFFFF）
  * @retval 无
  * @note   用于TDM等多通道音频配置，每个时隙对应一个音频通道
  */
void SAI_SlotInit(SAI_Block_TypeDef* SAI_Block_x, SAI_SlotInitTypeDef* SAI_SlotInitStruct)
{
  uint32_t tmpreg = 0;
  
  /* 验证SAI子模块有效性 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  
  /* 验证时隙参数合法性 */
  assert_param(IS_SAI_BLOCK_FIRSTBIT_OFFSET(SAI_SlotInitStruct->SAI_FirstBitOffset)); // 首字节偏移[0-31]
  assert_param(IS_SAI_BLOCK_SLOT_SIZE(SAI_SlotInitStruct->SAI_SlotSize));            // 时隙尺寸(16/32)
  assert_param(IS_SAI_BLOCK_SLOT_NUMBER(SAI_SlotInitStruct->SAI_SlotNumber));       // 总时隙数[1-16]
  assert_param(IS_SAI_SLOT_ACTIVE(SAI_SlotInitStruct->SAI_SlotActive));             // 时隙激活位图

  /* 配置时隙寄存器(SLOTR) */
  tmpreg = SAI_Block_x->SLOTR;     // 获取当前寄存器值
  tmpreg &= SLOTR_CLEAR_MASK;      // 使用预定义掩码0x0000F020清空配置位
  
  /* 组合时隙配置 */
  tmpreg |= (uint32_t)( SAI_SlotInitStruct->SAI_FirstBitOffset  | // 首字节偏移位[0:4]
                       SAI_SlotInitStruct->SAI_SlotSize        | // 时隙尺寸位[6:7]
                       SAI_SlotInitStruct->SAI_SlotActive      | // 时隙激活位[16:31]
                       ((SAI_SlotInitStruct->SAI_SlotNumber - 1) << 8) ); // 总时隙数位[8:11]
  
  SAI_Block_x->SLOTR = tmpreg;     // 写入配置后的SLOTR寄存器
}
/**
  * @brief  初始化SAI主配置结构体为默认值
  * @param  SAI_InitStruct: 要初始化的SAI_InitTypeDef结构体指针
  * @retval 无
  * @note   默认配置适用于基本的主发送模式：
  *         - 主发送模式
  *         - 自由协议（无预设帧结构）
  *         - 8位数据宽度
  *         - MSB优先
  *         - 时钟下降沿采样
  *         - 异步工作模式
  *         - 禁用输出驱动增强
  *         - 使能主分频器
  *         - FIFO空时触发中断
  */
void SAI_StructInit(SAI_InitTypeDef* SAI_InitStruct)
{
  /* 重置所有参数为安全默认值 */
  SAI_InitStruct->SAI_AudioMode = SAI_Mode_MasterTx;       // 默认主发送模式
  SAI_InitStruct->SAI_Protocol = SAI_Free_Protocol;        // 自由协议（需自定义帧结构）
  SAI_InitStruct->SAI_DataSize = SAI_DataSize_8b;          // 8位数据位宽
  SAI_InitStruct->SAI_FirstBit = SAI_FirstBit_MSB;         // 高位优先传输
  SAI_InitStruct->SAI_ClockStrobing = SAI_ClockStrobing_FallingEdge; // 下降沿采样
  SAI_InitStruct->SAI_Synchro = SAI_Asynchronous;          // 异步时钟模式
  SAI_InitStruct->SAI_OUTDRIV = SAI_OutputDrive_Disabled;  // 关闭驱动增强
  SAI_InitStruct->SAI_NoDivider = SAI_MasterDivider_Enabled; // 使能主时钟分频
  SAI_InitStruct->SAI_MasterDivider = 0;                   // 分频系数0（不分频）
  SAI_InitStruct->SAI_FIFOThreshold = SAI_Threshold_FIFOEmpty; // FIFO空时触发
}

/**
  * @brief  初始化SAI帧配置结构体为默认值
  * @param  SAI_FrameInitStruct: 要初始化的SAI_FrameInitTypeDef结构体指针
  * @retval 无
  * @note   默认帧配置特点：
  *         - 8时隙标准帧结构
  *         - 1个有效数据时隙
  *         - 帧起始生成同步信号
  *         - 低电平有效同步
  *         - 同步信号与首数据位对齐
  */
void SAI_FrameStructInit(SAI_FrameInitTypeDef* SAI_FrameInitStruct)
{
  SAI_FrameInitStruct->SAI_FrameLength = 8;            // 8时隙标准帧
  SAI_FrameInitStruct->SAI_ActiveFrameLength = 1;       // 仅第1时隙有效
  SAI_FrameInitStruct->SAI_FSDefinition = SAI_FS_StartFrame; // 帧起始生成同步
  SAI_FrameInitStruct->SAI_FSPolarity = SAI_FS_ActiveLow;    // 同步低有效
  SAI_FrameInitStruct->SAI_FSOffset = SAI_FS_FirstBit;      // 同步与首数据位对齐
}

/**
  * @brief  初始化SAI时隙配置结构体为默认值
  * @param  SAI_SlotInitStruct: 要初始化的SAI_SlotInitTypeDef结构体指针
  * @retval 无
  * @note   默认时隙配置特点：
  *         - 无位偏移（从时隙起始传输）
  *         - 时隙大小等于数据位宽
  *         - 单时隙配置
  *         - 所有时隙默认禁用
  */
void SAI_SlotStructInit(SAI_SlotInitTypeDef* SAI_SlotInitStruct)
{
  SAI_SlotInitStruct->SAI_FirstBitOffset = 0;           // 时隙起始位0
  SAI_SlotInitStruct->SAI_SlotSize = SAI_SlotSize_DataSize; // 时隙=数据位宽
  SAI_SlotInitStruct->SAI_SlotNumber = 1;               // 单时隙配置
  SAI_SlotInitStruct->SAI_SlotActive = SAI_Slot_NotActive; // 禁用所有时隙
}

/**
  * @brief  SAI模块使能控制函数
  * @param  SAI_Block_x: SAI子模块(A/B块)
  * @param  NewState: 模块状态
  *         ENABLE  - 启用SAI模块，开始数据传输
  *         DISABLE - 禁用SAI模块，停止所有传输
  * @retval 无
  * @note   修改CR1寄存器的SAIEN位(位0)
  *         启用前必须完成所有配置
  *         禁用模块将立即终止进行中的传输
  */
void SAI_Cmd(SAI_Block_TypeDef* SAI_Block_x, FunctionalState NewState)
{
  /* 验证输入参数 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 使能SAI模块 */
    /* 置位CR1.SAIEN(位0)激活模块 */
    SAI_Block_x->CR1 |= SAI_xCR1_SAIEN;
  }
  else
  {
    /* 禁用SAI模块 */
    /* 清零CR1.SAIEN(位0)停止模块 */
    SAI_Block_x->CR1 &= ~(SAI_xCR1_SAIEN);
  }
}

/**
  * @brief  配置SAI单声道/立体声模式
  * @param  SAI_Block_x: SAI子模块(A/B块)
  * @param  SAI_Mono_StreoMode: 音频模式选择
  *         SAI_MonoMode   - 单声道模式（双时隙合并）
  *         SAI_StereoMode - 立体声模式（双时隙独立）
  * @retval 无
  * @note   仅当配置双时隙时有效
  *         单声道模式下：将两个时隙数据合并处理
  *         操作CR1寄存器的MONO位(位4)
  * @warning 需要先配置时隙数NBSLOT=2才能正确生效
  */
void SAI_MonoModeConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_Mono_StreoMode)
{
  /* 参数合法性验证 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  assert_param(IS_SAI_BLOCK_MONO_STREO_MODE(SAI_MonoMode));
  
  /* 清除当前MONO位配置 */
  SAI_Block_x->CR1 &= ~(SAI_xCR1_MONO);  // CR1位4清零
  /* 设置新的音频模式 */
  SAI_Block_x->CR1 |= SAI_MonoMode; 
}

/**
  * @brief  配置SAI数据线三态控制
  * @param  SAI_Block_x: SAI子模块(A/B块)
  * @param  SAI_TRIState: 三态管理模式
  *         SAI_Output_NotReleased - 保持数据线驱动
  *         SAI_Output_Released    - 数据线高阻态
  * @retval 无
  * @note   仅对发送器模式有效
  *         控制CR1寄存器的TRIS位(位5)
  * @warning 当前实现存在代码错误，实际应操作TRIS位而非MONO位
  *          需要更正为CR1寄存器位5的操作
  */
void SAI_TRIStateConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_TRIState)
{
  /* 参数合法性验证 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  assert_param(IS_SAI_BLOCK_TRISTATE_MANAGEMENT(SAI_TRIState));
  
  /* 错误代码：应操作TRIS位(位5)，但实际修改了MONO位 */
  SAI_Block_x->CR1 &= ~(SAI_xCR1_MONO); // 
  SAI_Block_x->CR1 |= SAI_MonoMode;      // 
  
  /* 正确实现应为：
     SAI_Block_x->CR1 &= ~SAI_xCR1_TRIS; 
     SAI_Block_x->CR1 |= SAI_TRIState;
  */
}

/**
  * @brief  配置SAI模块的压扩模式
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  SAI_CompandingMode: 压扩模式选择，取值范围：
  *         @arg SAI_NoCompanding           禁用压扩算法
  *         @arg SAI_ULaw_1CPL_Companding   U律（1的补码表示）
  *         @arg SAI_ALaw_1CPL_Companding   A律（1的补码表示）
  *         @arg SAI_ULaw_2CPL_Companding   U律（2的补码表示）
  *         @arg SAI_ALaw_2CPL_Companding   A律（2的补码表示）
  * @note 压扩模式选择取决于SAI模块是发送端还是接收端
  *        数据压缩/扩展方向由模块的工作模式决定
  */
void SAI_CompandingModeConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_CompandingMode)
{
  /* 参数有效性验证：检查是否为有效的SAI模块 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  /* 参数有效性验证：检查压扩模式参数是否在允许范围内 */
  assert_param(IS_SAI_BLOCK_COMPANDING_MODE(SAI_CompandingMode));
  
  /* 清除CR2寄存器中的压扩模式设置位（COMP位域）*/
  SAI_Block_x->CR2 &= ~(SAI_xCR2_COMP);
  
  /* 将新的压扩模式值写入CR2寄存器 */
  SAI_Block_x->CR2 |= SAI_CompandingMode;
}

/**
  * @brief  启用/禁用SAI模块的静音模式
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  NewState: 功能状态（ENABLE/DISABLE）
  * @note 仅对发送模块有效
  *        静音模式作用于整个帧的所有有效时隙
  *        设置静音位后将在当前帧结束时生效
  *        取消静音也在设置位所在的帧结束时生效
  */
void SAI_MuteModeCmd(SAI_Block_TypeDef* SAI_Block_x, FunctionalState NewState)
{
  /* 参数有效性验证：检查是否为有效的SAI模块 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  /* 参数有效性验证：检查功能状态参数是否合法 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)  // 启用静音模式
  {
    /* 设置CR2寄存器的MUTE位（静音使能）*/
    SAI_Block_x->CR2 |= SAI_xCR2_MUTE;
  }
  else  // 禁用静音模式
  {
    /* 清除CR2寄存器的MUTE位（静音禁用）*/
    SAI_Block_x->CR2 &= ~(SAI_xCR2_MUTE);
  }
}

/**
  * @brief  配置SAI模块的静音输出值
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  SAI_MuteValue: 静音值选项：
  *         @arg SAI_ZeroValue     静音时输出0值
  *         @arg SAI_LastSentValue 静音时输出最后发送的值
  * @note 仅对发送模块有效
  *        当启用静音模式时有效
  *        当使用时隙数≤2时，最后发送值的配置才有意义
  */
void SAI_MuteValueConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_MuteValue)
{
  /* 参数有效性验证：检查是否为有效的SAI模块 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  /* 参数有效性验证：检查静音值参数是否合法 */
  assert_param(IS_SAI_BLOCK_MUTE_VALUE(SAI_MuteValue));
  
  /* 清除CR2寄存器的静音值设置位（MUTEVAL位域）*/
  SAI_Block_x->CR2 &= ~(SAI_xCR2_MUTEVAL);
  
  /* 将新的静音值设置写入CR2寄存器 */
  SAI_Block_x->CR2 |= SAI_MuteValue;
}

/**
  * @brief  配置静音帧计数器
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  SAI_MuteCounter: 静音帧计数值（取值范围0-63）
  *         - 该参数对应CR2寄存器的MUTECNT[5:0]位域
  *         - 计数值决定静音模式持续的时间长度（以帧为单位）
  * @note 该功能仅在接收端模块有效
  *        静音帧计数器用于控制静音状态的持续时间
  */
void SAI_MuteFrameCounterConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_MuteCounter)
{
  /* 参数有效性验证：检查是否为有效的SAI模块 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  /* 参数有效性验证：检查计数值是否在0-63范围内 */
  assert_param(IS_SAI_BLOCK_MUTE_COUNTER(SAI_MuteCounter));
  
  /* 清除CR2寄存器的静音计数器位域（MUTECNT[5:0]）*/
  SAI_Block_x->CR2 &= ~(SAI_xCR2_MUTECNT);
  
  /* 将计数值左移7位对齐到MUTECNT位域，并写入CR2寄存器 */
  SAI_Block_x->CR2 |= (SAI_MuteCounter << 7);  // MUTECNT位域位于CR2寄存器的bit7-bit12
}

/**
  * @brief  刷新FIFO缓冲区
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @note 立即重置FIFO读写指针到初始状态
  *        当前FIFO中未处理的数据将永久丢失
  *        该操作会立即生效，不需要等待帧结束
  */
void SAI_FlushFIFO(SAI_Block_TypeDef* SAI_Block_x)
{
  /* 参数有效性验证：检查是否为有效的SAI模块 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  
  /* 设置CR2寄存器的FFLUSH位（bit13）触发FIFO刷新操作 */
  SAI_Block_x->CR2 |= SAI_xCR2_FFLUSH;  // 该位为自动清零位，硬件自动复位
}

/**
  * @}
  */

/** @defgroup SAI_Group2 Data transfers functions
 *  @brief   Data transfers functions
 *
@verbatim   
 ===============================================================================
                       ##### Data transfers functions #####
 ===============================================================================  
  [..]
  This section provides a set of functions allowing to manage the SAI data transfers.
  [..]
  In reception, data are received and then stored into an internal FIFO while 
  In transmission, data are first stored into an internal FIFO before being 
  transmitted.
  [..]
  The read access of the SAI_xDR register can be done using the SAI_ReceiveData()
  function and returns the Rx buffered value. Whereas a write access to the SAI_DR 
  can be done using SAI_SendData() function and stores the written data into 
  Tx buffer.

@endverbatim
  * @{
  */
/**
  * @brief  读取SAI模块接收数据寄存器值
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @retval 32位接收数据寄存器(DR)的当前值
  * @note 该函数直接返回DR寄存器内容
  *        - 当模块配置为接收模式时有效
  *        - 读取操作不会清除数据寄存器状态
  *        - 建议在数据就绪标志置位后调用
  */
uint32_t SAI_ReceiveData(SAI_Block_TypeDef* SAI_Block_x)
{
  /* 参数有效性验证：检查是否为有效的SAI模块 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  
  /* 直接返回数据寄存器(DR)的32位内容 */
  return SAI_Block_x->DR;  // DR寄存器地址偏移：0x00（相对于模块基地址）
}

/**
  * @brief  写入发送数据到SAI模块
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  Data: 要发送的32位音频数据
  * @note 该函数直接操作数据寄存器(DR)
  *        - 当模块配置为发送模式时有效
  *        - 写入操作会触发数据传输
  *        - 建议在发送就绪标志置位后调用
  */
void SAI_SendData(SAI_Block_TypeDef* SAI_Block_x, uint32_t Data)
{
  /* 参数有效性验证：检查是否为有效的SAI模块 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  
  /* 将32位数据写入数据寄存器(DR) */
  SAI_Block_x->DR = Data;  // DR寄存器地址偏移：0x00（相对于模块基地址）
}

/**
  * @}
  */

/** @defgroup SAI_Group3 DMA transfers management functions
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
  * @brief  启用/禁用SAI模块的DMA传输功能
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  NewState: DMA功能状态（ENABLE/DISABLE）
  * @note 该函数控制DMA传输请求的使能状态
  *        - 启用后SAI模块将在数据就绪事件（发送/接收）时触发DMA请求
  *        - 禁用将停止所有与DMA控制器的交互
  *        - 需要配合DMA控制器配置使用
  */
void SAI_DMACmd(SAI_Block_TypeDef* SAI_Block_x, FunctionalState NewState)
{
  /* 参数有效性验证：检查是否为有效的SAI模块 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));
  /* 参数有效性验证：检查功能状态参数是否合法 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)  // 启用DMA传输
  {
    /* 设置CR1寄存器的DMAEN位（bit0）使能DMA请求 */
    SAI_Block_x->CR1 |= SAI_xCR1_DMAEN;  // DMA使能后，发送/接收事件将触发DMA传输
  }
  else  // 禁用DMA传输
  {
    /* 清除CR1寄存器的DMAEN位（bit0）禁用DMA请求 */
    SAI_Block_x->CR1 &= ~(SAI_xCR1_DMAEN);  // 立即停止所有DMA传输活动
  }
}

/**
  * @}
  */

/** @defgroup SAI_Group4 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
  *
@verbatim   
 ===============================================================================
            ##### Interrupts and flags management functions #####
 ===============================================================================  
  [..]
  This section provides a set of functions allowing to configure the SAI Interrupts 
  sources and check or clear the flags or pending bits status.
  The user should identify which mode will be used in his application to manage 
  the communication: Polling mode, Interrupt mode or DMA mode. 
    
  *** Polling Mode ***
  ====================
  [..]
  In Polling Mode, the SAI communication can be managed by 7 flags:
     (#) SAI_FLAG_FREQ : to indicate if there is a FIFO Request to write or to read.
     (#) SAI_FLAG_MUTEDET : to indicate if a MUTE frame detected
     (#) SAI_FLAG_OVRUDR : to indicate if an Overrun or Underrun error occur
     (#) SAI_FLAG_AFSDET : to indicate if there is the detection of a audio frame 
                          synchronisation (FS) earlier than expected
     (#) SAI_FLAG_LFSDET : to indicate if there is the detection of a audio frame 
                          synchronisation (FS) later than expected              
     (#) SAI_FLAG_CNRDY : to indicate if  the codec is not ready to communicate during 
                         the reception of the TAG 0 (slot0) of the AC97 audio frame 
     (#) SAI_FLAG_WCKCFG: to indicate if wrong clock configuration in master mode 
                         error occurs.
  [..]
  In this Mode it is advised to use the following functions:
     (+) FlagStatus SAI_GetFlagStatus(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_FLAG);
     (+) void SAI_ClearFlag(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_FLAG);

  *** Interrupt Mode ***
  ======================
  [..]
  In Interrupt Mode, the SAI communication can be managed by 7 interrupt sources
  and 7 pending bits: 
  (+) Pending Bits:
     (##) SAI_IT_FREQ : to indicate if there is a FIFO Request to write or to read.
     (##) SAI_IT_MUTEDET : to indicate if a MUTE frame detected.
     (##) SAI_IT_OVRUDR : to indicate if an Overrun or Underrun error occur.
     (##) SAI_IT_AFSDET : to indicate if there is the detection of a audio frame 
                          synchronisation (FS) earlier than expected.
     (##) SAI_IT_LFSDET : to indicate if there is the detection of a audio frame 
                          synchronisation (FS) later than expected.              
     (##) SAI_IT_CNRDY : to indicate if  the codec is not ready to communicate during 
                         the reception of the TAG 0 (slot0) of the AC97 audio frame. 
     (##) SAI_IT_WCKCFG: to indicate if wrong clock configuration in master mode 
                         error occurs.

  (+) Interrupt Source:
     (##) SAI_IT_FREQ : specifies the interrupt source for FIFO Request.
     (##) SAI_IT_MUTEDET : specifies the interrupt source for MUTE frame detected.
     (##) SAI_IT_OVRUDR : specifies the interrupt source for overrun or underrun error.
     (##) SAI_IT_AFSDET : specifies the interrupt source for anticipated frame synchronization
                          detection interrupt.
     (##) SAI_IT_LFSDET : specifies the interrupt source for late frame synchronization
                          detection interrupt.             
     (##) SAI_IT_CNRDY : specifies the interrupt source for codec not ready interrupt
     (##) SAI_IT_WCKCFG: specifies the interrupt source for wrong clock configuration
                         interrupt.
  [..]                     
  In this Mode it is advised to use the following functions:
     (+) void SAI_ITConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_IT, FunctionalState NewState);
     (+) ITStatus SAI_GetITStatus(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_IT);
     (+) void SAI_ClearITPendingBit(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_IT);

  *** DMA Mode ***
  ================
  [..]
  In DMA Mode, each SAI audio block has an independent DMA interface in order to 
  read or to write into the SAI_xDR register (to hit the internal FIFO). 
  There is one DMA channel by audio block following basic DMA request/acknowledge 
  protocol.
  [..]
  In this Mode it is advised to use the following function:
    (+) void SAI_DMACmd(SAI_Block_TypeDef* SAI_Block_x, FunctionalState NewState);
  [..]
  This section provides also functions allowing to
   (+) Check the SAI Block enable status
   (+)Check the FIFO status 
   
  *** SAI Block Enable status ***
  ===============================
  [..]
  After disabling a SAI Block, it is recommended to check (or wait until) the SAI Block 
  is effectively disabled. If a Block is disabled while an audio frame transfer is ongoing
  the current frame will be transferred and the block will be effectively disabled only at 
  the end of audio frame. 
  To monitor this state it is possible to use the following function:
    (+) FunctionalState SAI_GetCmdStatus(SAI_Block_TypeDef* SAI_Block_x); 
 
  *** SAI Block FIFO status ***
  =============================
  [..]
  It is possible to monitor the FIFO status when a transfer is ongoing using the following 
  function:
    (+) uint32_t SAI_GetFIFOStatus(SAI_Block_TypeDef* SAI_Block_x);
    
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified SAI Block interrupts.
  * @param  SAI_Block_x: where x can be A or B to select the SAI Block peripheral. 
  * @param  SAI_IT: specifies the SAI interrupt source to be enabled or disabled. 
  *          This parameter can be one of the following values:
  *            @arg SAI_IT_FREQ: FIFO Request interrupt mask
  *            @arg SAI_IT_MUTEDET: MUTE detection interrupt mask
  *            @arg SAI_IT_OVRUDR: overrun/underrun interrupt mask
  *            @arg SAI_IT_AFSDET: anticipated frame synchronization detection 
  *                                interrupt mask  
  *            @arg SAI_IT_LFSDET: late frame synchronization detection interrupt 
  *                                mask
  *            @arg SAI_IT_CNRDY: codec not ready interrupt mask
  *            @arg SAI_IT_WCKCFG: wrong clock configuration interrupt mask      
  * @param  NewState: new state of the specified SAI interrupt.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  配置SAI模块的中断使能状态
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  SAI_IT: 中断类型选择，支持以下组合：
  *         @arg SAI_IT_FREQ    FIFO请求中断
  *         @arg SAI_IT_MUTEDET 静音检测中断
  *         @arg SAI_IT_OVRUDR  溢出/欠载中断
  *         @arg SAI_IT_WCKCFG  时钟配置错误中断
  *         @arg SAI_IT_CNRDY   编解码器就绪中断
  *         @arg SAI_IT_AFSDET  超前帧同步检测中断
  *         @arg SAI_IT_LFSDET  滞后帧同步检测中断
  * @param  NewState: 中断使能状态（ENABLE/DISABLE）
  * @note 中断使能状态通过IMR寄存器控制
  *        实际中断触发需配合NVIC配置
  */
void SAI_ITConfig(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_IT, FunctionalState NewState)
{
  /* 三重参数验证 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));       // 验证模块有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));          // 验证状态参数合法性
  assert_param(IS_SAI_BLOCK_CONFIG_IT(SAI_IT));         // 验证中断类型合法性

  if (NewState != DISABLE)  // 启用指定中断
  {
    /* 设置中断屏蔽寄存器(IMR)对应位 */
    SAI_Block_x->IMR |= SAI_IT;  // 位或操作使能单个或多个中断源
  }
  else  // 禁用指定中断
  {
    /* 清除中断屏蔽寄存器(IMR)对应位 */
    SAI_Block_x->IMR &= ~(SAI_IT);  // 位与操作禁用单个或多个中断源
  }
}

/**
  * @brief  获取SAI模块状态标志位
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  SAI_FLAG: 要查询的状态标志，支持：
  *         @arg SAI_FLAG_FREQ    FIFO请求标志
  *         @arg SAI_FLAG_MUTEDET 静音检测标志
  *         @arg SAI_FLAG_OVRUDR  溢出/欠载标志
  *         @arg SAI_FLAG_WCKCFG  时钟配置错误标志
  *         @arg SAI_FLAG_CNRDY   编解码器就绪标志
  *         @arg SAI_FLAG_AFSDET  超前帧同步标志
  *         @arg SAI_FLAG_LFSDET  滞后帧同步标志
  * @retval SET(1)/RESET(0) 标志位当前状态
  * @note 通过状态寄存器(SR)实时读取标志位
  *        FREQ标志状态与FIFO阈值设置相关
  */
FlagStatus SAI_GetFlagStatus(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_FLAG)
{
  FlagStatus bitstatus = RESET;
  
  /* 双重参数验证 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));       // 验证模块有效性
  assert_param(IS_SAI_BLOCK_GET_FLAG(SAI_FLAG));        // 验证标志类型合法性
  
  /* 状态寄存器(SR)位与检测 */
  if ((SAI_Block_x->SR & SAI_FLAG) != (uint32_t)RESET) // 位与操作检测特定标志位
  {
    bitstatus = SET;    // 标志位置位状态
  }
  else
  {
    bitstatus = RESET;  // 标志位复位状态
  }
  return  bitstatus;    // 返回当前标志状态
}

/**
  * @brief  清除SAI模块状态标志位
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  SAI_FLAG: 要清除的标志位（排除FREQ标志）
  *         支持清除的标志：
  *         @arg SAI_FLAG_MUTEDET 静音检测标志
  *         @arg SAI_FLAG_OVRUDR  溢出/欠载标志
  *         @arg SAI_FLAG_WCKCFG  时钟配置错误标志
  *         @arg SAI_FLAG_CNRDY   编解码器就绪标志
  *         @arg SAI_FLAG_AFSDET  超前帧同步标志
  *         @arg SAI_FLAG_LFSDET  滞后帧同步标志
  * @note FREQ标志自动清除条件：
  *        - 发送模式：FIFO满或单数据缓冲模式（取决于FTH位）
  *        - 接收模式：FIFO非空时自动更新
  *        需通过硬件条件自动清除，不可手动清除
  */
void SAI_ClearFlag(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_FLAG)
{
  /* 双重参数验证 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));       // 验证模块有效性
  assert_param(IS_SAI_BLOCK_CLEAR_FLAG(SAI_FLAG));      // 验证可清除标志类型
  
  /* 向清除标志寄存器(CLRFR)写入指定标志 */
  SAI_Block_x->CLRFR |= SAI_FLAG;  // 写1清除机制，实际通过硬件自动复位标志位
}
/**
  * @brief  获取SAI模块中断触发状态
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  SAI_IT: 中断类型，支持：
  *         @arg SAI_IT_FREQ     FIFO请求中断
  *         @arg SAI_IT_MUTEDET  静音检测中断
  *         @arg SAI_IT_OVRUDR   溢出/欠载中断
  *         @arg SAI_IT_AFSDET   超前帧同步中断
  *         @arg SAI_IT_LFSDET   滞后帧同步中断
  *         @arg SAI_IT_CNRDY    编解码器就绪中断
  *         @arg SAI_IT_WCKCFG   时钟配置错误中断
  * @retval SET(触发)/RESET(未触发) 中断状态
  * @note 实际中断状态需同时满足：
  *        - 中断使能（IMR对应位为1）
  *        - 中断标志置位（SR对应位为1）
  */
ITStatus SAI_GetITStatus(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t  enablestatus = 0;

  /* 双重参数验证 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));       // 验证模块有效性
  assert_param(IS_SAI_BLOCK_CONFIG_IT(SAI_IT));         // 验证中断类型合法性
  
  /* 获取中断使能状态：读取中断屏蔽寄存器(IMR) */
  enablestatus = (SAI_Block_x->IMR & SAI_IT);          // 获取指定中断的使能状态

  /* 组合判断：状态寄存器(SR) & 使能状态 */
  if (((SAI_Block_x->SR & SAI_IT) != (uint32_t)RESET) && // 检查中断标志位
      (enablestatus != (uint32_t)RESET))                 // 检查中断使能位
  {
    bitstatus = SET;    // 中断处于有效触发状态
  }
  else
  {
    bitstatus = RESET;  // 中断未触发或未使能
  }
  return bitstatus;      // 返回综合判断结果
}

/**
  * @brief  清除SAI模块中断挂起标志
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @param  SAI_IT: 中断类型（排除FREQ中断）
  *         支持清除的中断：
  *         @arg SAI_IT_MUTEDET  静音检测中断
  *         @arg SAI_IT_OVRUDR   溢出/欠载中断
  *         @arg SAI_IT_WCKCFG   时钟配置错误中断
  *         @arg SAI_IT_CNRDY    编解码器就绪中断
  *         @arg SAI_IT_AFSDET   超前帧同步中断
  *         @arg SAI_IT_LFSDET   滞后帧同步中断
  * @note FREQ中断标志的特殊清除机制：
  *        - 发送模式：FIFO满或单缓冲模式（FTH位控制）
  *        - 接收模式：FIFO非空时自动清除
  */
void SAI_ClearITPendingBit(SAI_Block_TypeDef* SAI_Block_x, uint32_t SAI_IT)
{
  /* 双重参数验证 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));       // 验证模块有效性
  assert_param(IS_SAI_BLOCK_CONFIG_IT(SAI_IT));        // 验证中断类型合法性
  
  /* 清除中断挂起标志：写入清除标志寄存器(CLRFR) */
  SAI_Block_x->CLRFR |= SAI_IT;  // 写1清除机制，硬件自动复位对应标志位
}

/**
  * @brief  获取SAI模块使能状态
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @retval ENABLE(运行中)/DISABLE(已停止)
  * @note 检测CR1寄存器的SAIEN位状态
  *        - 禁用模块时建议循环检测直到返回DISABLE
  *        - 模块禁用后仍可能完成当前帧传输
  */
FunctionalState SAI_GetCmdStatus(SAI_Block_TypeDef* SAI_Block_x)
{
  FunctionalState state = DISABLE;

  /* 参数有效性验证 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));       // 验证模块有效性
  
  /* 检测CR1寄存器的SAIEN位（bit0） */
  if ((SAI_Block_x->CR1 & SAI_xCR1_SAIEN) != 0)        // 位与操作检测使能位
  {
    state = ENABLE;    // 模块处于激活状态（可能正在传输最后一帧）
  }
  else
  {
    state = DISABLE;   // 模块完全停止
  }
  return state;         // 返回当前模块状态
}

/**
  * @brief  获取SAI模块FIFO缓冲区填充状态
  * @param  SAI_Block_x: 选择SAI模块(A或B)，例如SAI_Block_A
  * @retval FIFO填充状态（6种状态）：
  *         @arg SAI_FIFOStatus_Empty            FIFO空
  *         @arg SAI_FIFOStatus_Less1QuarterFull  FIFO数据量＜25%且非空
  *         @arg SAI_FIFOStatus_1QuarterFull     FIFO数据量≥25%
  *         @arg SAI_FIFOStatus_HalfFull         FIFO数据量≥50%
  *         @arg SAI_FIFOStatus_3QuartersFull    FIFO数据量≥75%
  *         @arg SAI_FIFOStatus_Full            FIFO满
  * @note 通过状态寄存器(SR)的FLVL[2:0]位域实时获取
  *        状态变化与数据收发操作同步更新
  */
uint32_t SAI_GetFIFOStatus(SAI_Block_TypeDef* SAI_Block_x)
{
  uint32_t tmpreg = 0;

  /* 参数有效性验证：检查是否为有效的SAI模块 */
  assert_param(IS_SAI_BLOCK_PERIPH(SAI_Block_x));  // 验证模块选择合法性
  
  /* 读取状态寄存器的FIFO级别位域（FLVL[2:0]）*/
  tmpreg = (uint32_t)((SAI_Block_x->SR & SAI_xSR_FLVL));  // 位掩码提取3bit状态值
  
  return tmpreg;  // 返回原始位域值（需与状态枚举常量配合使用）
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

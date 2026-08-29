/**
  ******************************************************************************
  * @file    stm32f4xx_dma.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Direct Memory Access controller (DMA):           
  *           + Initialization and Configuration
  *           + Data Counter
  *           + Double Buffer mode configuration and command  
  *           + Interrupts and flags management
  *           
  @verbatim      
 ===============================================================================      
                       ##### How to use this driver #####
 ===============================================================================
    [..] 
      (#) Enable The DMA controller clock using RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_DMA1, ENABLE)
          function for DMA1 or using RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_DMA2, ENABLE)
          function for DMA2.
  
      (#) Enable and configure the peripheral to be connected to the DMA Stream
          (except for internal SRAM / FLASH memories: no initialization is 
          necessary). 
          
      (#) For a given Stream, program the required configuration through following parameters:   
          Source and Destination addresses, Transfer Direction, Transfer size, Source and Destination 
          data formats, Circular or Normal mode, Stream Priority level, Source and Destination 
          Incrementation mode, FIFO mode and its Threshold (if needed), Burst 
          mode for Source and/or Destination (if needed) using the DMA_Init() function.
          To avoid filling unneccessary fields, you can call DMA_StructInit() function
          to initialize a given structure with default values (reset values), the modify
          only necessary fields 
          (ie. Source and Destination addresses, Transfer size and Data Formats).
  
      (#) Enable the NVIC and the corresponding interrupt(s) using the function 
          DMA_ITConfig() if you need to use DMA interrupts. 
  
      (#) Optionally, if the Circular mode is enabled, you can use the Double buffer mode by configuring 
          the second Memory address and the first Memory to be used through the function 
          DMA_DoubleBufferModeConfig(). Then enable the Double buffer mode through the function
          DMA_DoubleBufferModeCmd(). These operations must be done before step 6.
      
      (#) Enable the DMA stream using the DMA_Cmd() function. 
                  
      (#) Activate the needed Stream Request using PPP_DMACmd() function for
          any PPP peripheral except internal SRAM and FLASH (ie. SPI, USART ...)
          The function allowing this operation is provided in each PPP peripheral
          driver (ie. SPI_DMACmd for SPI peripheral).
          Once the Stream is enabled, it is not possible to modify its configuration
          unless the stream is stopped and disabled.
          After enabling the Stream, it is advised to monitor the EN bit status using
          the function DMA_GetCmdStatus(). In case of configuration errors or bus errors
          this bit will remain reset and all transfers on this Stream will remain on hold.      
  
      (#) Optionally, you can configure the number of data to be transferred
          when the Stream is disabled (ie. after each Transfer Complete event
          or when a Transfer Error occurs) using the function DMA_SetCurrDataCounter().
          And you can get the number of remaining data to be transferred using 
          the function DMA_GetCurrDataCounter() at run time (when the DMA Stream is
          enabled and running).  
                     
      (#) To control DMA events you can use one of the following two methods:
        (##) Check on DMA Stream flags using the function DMA_GetFlagStatus().  
        (##) Use DMA interrupts through the function DMA_ITConfig() at initialization
             phase and DMA_GetITStatus() function into interrupt routines in
             communication phase.
    [..]     
          After checking on a flag you should clear it using DMA_ClearFlag()
          function. And after checking on an interrupt event you should 
          clear it using DMA_ClearITPendingBit() function.    
                
      (#) Optionally, if Circular mode and Double Buffer mode are enabled, you can modify
          the Memory Addresses using the function DMA_MemoryTargetConfig(). Make sure that
          the Memory Address to be modified is not the one currently in use by DMA Stream.
          This condition can be monitored using the function DMA_GetCurrentMemoryTarget().
                
      (#) Optionally, Pause-Resume operations may be performed:
          The DMA_Cmd() function may be used to perform Pause-Resume operation. 
          When a transfer is ongoing, calling this function to disable the 
          Stream will cause the transfer to be paused. All configuration registers 
          and the number of remaining data will be preserved. When calling again 
          this function to re-enable the Stream, the transfer will be resumed from 
          the point where it was paused.          
                   
      -@- Memory-to-Memory transfer is possible by setting the address of the memory into
           the Peripheral registers. In this mode, Circular mode and Double Buffer mode
           are not allowed.
    
      -@- The FIFO is used mainly to reduce bus usage and to allow data 
           packing/unpacking: it is possible to set different Data Sizes for 
           the Peripheral and the Memory (ie. you can set Half-Word data size 
           for the peripheral to access its data register and set Word data size
           for the Memory to gain in access time. Each two Half-words will be 
           packed and written in a single access to a Word in the Memory).
      
      -@- When FIFO is disabled, it is not allowed to configure different 
           Data Sizes for Source and Destination. In this case the Peripheral 
           Data Size will be applied to both Source and Destination.               
  
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
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup DMA 
  * @brief DMA driver modules
  * @{
  */ 
/* DMA模块私有宏定义 ---------------------------------------------------------*/
/**
  * @defgroup DMA_Private_Defines DMA模块私有宏定义
  * @brief DMA中断标志位掩码及控制寄存器位操作宏
  * @note 这些宏用于DMA流控制寄存器的位操作，对应STM32F4xx参考手册第10.3.3节
  */

/* 传输中断使能位组合掩码 -----------------------------------------------------
   包含四种DMA中断使能位：
   - TCIE: 传输完成中断使能
   - HTIE: 半传输中断使能
   - TEIE: 传输错误中断使能
   - DMEIE: 直接模式错误中断使能
   该掩码用于同时配置多个中断使能位 */
#define TRANSFER_IT_ENABLE_MASK (uint32_t)(DMA_SxCR_TCIE | DMA_SxCR_HTIE | \
                                           DMA_SxCR_TEIE | DMA_SxCR_DMEIE)

/* DMA流0中断标志位掩码 ------------------------------------------------------
   对应LISR寄存器中的流0标志位：
   - FEIF0: FIFO错误标志
   - DMEIF0: 直接模式错误标志 
   - TEIF0: 传输错误标志
   - HTIF0: 半传输完成标志
   - TCIF0: 传输完成标志
   偏移位置参考RM0090手册表43 */
#define DMA_Stream0_IT_MASK     (uint32_t)(DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
                                           DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
                                           DMA_LISR_TCIF0)

/* 各DMA流中断标志位偏移计算 --------------------------------------------------
   根据流编号在寄存器中的位置进行位偏移：
   - 流1标志位比流0右移6位（LISR/HISR寄存器布局）
   - 流2标志位比流0右移16位
   - 流3标志位比流0右移22位 */
#define DMA_Stream1_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 6)  // 流1标志位偏移
#define DMA_Stream2_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 16) // 流2标志位偏移
#define DMA_Stream3_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 22) // 流3标志位偏移

/* 高位流中断标志组合掩码 -----------------------------------------------------
   流4-7的中断标志位于HISR寄存器，需要组合：
   - 0x20000000对应HISR寄存器偏移
   - 流4使用流0掩码组合高位标志
   - 流5使用流1掩码组合高位标志 */
#define DMA_Stream4_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK | (uint32_t)0x20000000) // 流4:HISR[0]
#define DMA_Stream5_IT_MASK     (uint32_t)(DMA_Stream1_IT_MASK | (uint32_t)0x20000000) // 流5:HISR[6]
#define DMA_Stream6_IT_MASK     (uint32_t)(DMA_Stream2_IT_MASK | (uint32_t)0x20000000) // 流6:HISR[16]
#define DMA_Stream7_IT_MASK     (uint32_t)(DMA_Stream3_IT_MASK | (uint32_t)0x20000000) // 流7:HISR[22]

/* 全局中断标志掩码 ----------------------------------------------------------
   0x0F3C0F3C对应所有流的传输完成(TC)、半传输(HT)、传输错误(TE)标志位
   用于批量清除中断标志 */
#define TRANSFER_IT_MASK        (uint32_t)0x0F3C0F3C

/* 高位中断状态掩码 ----------------------------------------------------------
   0x20000000对应HISR寄存器的全局偏移量
   用于区分LISR（低8流）和HISR（高8流）寄存器组 */
#define HIGH_ISR_MASK           (uint32_t)0x20000000

/* 保留位保护掩码 ------------------------------------------------------------
   0x0F7D0F7D对应所有保留位的位掩码
   用于寄存器写操作时保护保留位不被修改 */
#define RESERVED_MASK           (uint32_t)0x0F7D0F7D


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/** @defgroup DMA_Private_Functions
  * @{
  */

/** @defgroup DMA_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions
 *
@verbatim   
 ===============================================================================
                ##### Initialization and Configuration functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to initialize the DMA Stream source
    and destination addresses, incrementation and data sizes, transfer direction, 
    buffer size, circular/normal mode selection, memory-to-memory mode selection 
    and Stream priority value.
    [..]
    The DMA_Init() function follows the DMA configuration procedures as described in
    reference manual (RM0090) except the first point: waiting on EN bit to be reset.
    This condition should be checked by user application using the function DMA_GetCmdStatus()
    before calling the DMA_Init() function.

@endverbatim
  * @{
  */
/**
  * @brief  DMA流反初始化函数，用于将指定DMA流的寄存器恢复为默认复位值
  * @param  DMAy_Streamx: 需要反初始化的DMA流，y可以是1或2选择DMA控制器，
  *         x可以是0-7选择具体的DMA流。例如DMA1_Stream0表示DMA1控制器的0号流
  * @retval 无
  * @note   该函数将执行以下操作：
  *         1. 禁用指定DMA流
  *         2. 复位流控制寄存器(CR)
  *         3. 复位数据传输数量寄存器(NDTR)
  *         4. 复位外设地址寄存器(PAR)
  *         5. 复位存储器地址寄存器(M0AR/M1AR)
  *         6. 复位FIFO控制寄存器(FCR)
  *         7. 清除对应流的中断挂起标志
  *         使用前应确保DMA流已停止（通过DMA_GetCmdStatus检查）
  */
void DMA_DeInit(DMA_Stream_TypeDef* DMAy_Streamx)
{
  /* 参数合法性检查：验证传入的DMA流指针是否有效 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));

  /* 禁用当前DMA流：清除CR寄存器的EN使能位（第0位），停止DMA传输 */
  DMAy_Streamx->CR &= ~((uint32_t)DMA_SxCR_EN);

  /* 复位控制寄存器：将CR寄存器所有位清零，关闭所有配置选项 */
  DMAy_Streamx->CR  = 0;
  
  /* 复位数据传输计数器：将NDTR寄存器清零，清除剩余传输数量 */
  DMAy_Streamx->NDTR = 0;
  
  /* 复位外设地址寄存器：清除PAR寄存器存储的外设地址 */
  DMAy_Streamx->PAR  = 0;
  
  /* 复位存储器0地址寄存器：清除M0AR寄存器存储的存储器地址0 */
  DMAy_Streamx->M0AR = 0;

  /* 复位存储器1地址寄存器：清除M1AR寄存器存储的存储器地址1（用于双缓冲模式） */
  DMAy_Streamx->M1AR = 0;

  /* 配置FIFO控制寄存器：
     - 设置FTH[1:0]为01b（1/4 FIFO阈值）
     - 清除DMDIS位（使能直接模式）
     - 复位FEIE位（禁用FIFO错误中断）
     - 默认值0x21对应二进制00100001，即保留默认复位值 */
  DMAy_Streamx->FCR = (uint32_t)0x00000021; 

  /* 根据不同的DMA流选择对应的中断标志清除寄存器 */
  /* DMA1的低8位流中断清除寄存器(LIFCR)处理 */
  if (DMAy_Streamx == DMA1_Stream0)
  {
    /* 清除DMA1流0的所有中断标志：TCIF/HTIF/TEIF/DMEIF/FEIF */
    DMA1->LIFCR = DMA_Stream0_IT_MASK;
  }
  else if (DMAy_Streamx == DMA1_Stream1)
  {
    /* 清除DMA1流1的中断标志 */
    DMA1->LIFCR = DMA_Stream1_IT_MASK;
  }
  else if (DMAy_Streamx == DMA1_Stream2)
  {
    /* 清除DMA1流2的中断标志 */
    DMA1->LIFCR = DMA_Stream2_IT_MASK;
  }
  else if (DMAy_Streamx == DMA1_Stream3)
  {
    /* 清除DMA1流3的中断标志 */
    DMA1->LIFCR = DMA_Stream3_IT_MASK;
  }
  /* DMA1的高8位流中断清除寄存器(HIFCR)处理 */
  else if (DMAy_Streamx == DMA1_Stream4)
  {
    /* 清除DMA1流4的中断标志 */
    DMA1->HIFCR = DMA_Stream4_IT_MASK;
  }
  else if (DMAy_Streamx == DMA1_Stream5)
  {
    /* 清除DMA1流5的中断标志 */
    DMA1->HIFCR = DMA_Stream5_IT_MASK;
  }
  else if (DMAy_Streamx == DMA1_Stream6)
  {
    /* 清除DMA1流6的中断标志 */
    DMA1->HIFCR = (uint32_t)DMA_Stream6_IT_MASK;
  }
  else if (DMAy_Streamx == DMA1_Stream7)
  {
    /* 清除DMA1流7的中断标志 */
    DMA1->HIFCR = DMA_Stream7_IT_MASK;
  }
  /* DMA2的低8位流中断清除寄存器(LIFCR)处理 */
  else if (DMAy_Streamx == DMA2_Stream0)
  {
    /* 清除DMA2流0的中断标志 */
    DMA2->LIFCR = DMA_Stream0_IT_MASK;
  }
  else if (DMAy_Streamx == DMA2_Stream1)
  {
    /* 清除DMA2流1的中断标志 */
    DMA2->LIFCR = DMA_Stream1_IT_MASK;
  }
  else if (DMAy_Streamx == DMA2_Stream2)
  {
    /* 清除DMA2流2的中断标志 */
    DMA2->LIFCR = DMA_Stream2_IT_MASK;
  }
  else if (DMAy_Streamx == DMA2_Stream3)
  {
    /* 清除DMA2流3的中断标志 */
    DMA2->LIFCR = DMA_Stream3_IT_MASK;
  }
  /* DMA2的高8位流中断清除寄存器(HIFCR)处理 */
  else if (DMAy_Streamx == DMA2_Stream4)
  {
    /* 清除DMA2流4的中断标志 */
    DMA2->HIFCR = DMA_Stream4_IT_MASK;
  }
  else if (DMAy_Streamx == DMA2_Stream5)
  {
    /* 清除DMA2流5的中断标志 */
    DMA2->HIFCR = DMA_Stream5_IT_MASK;
  }
  else if (DMAy_Streamx == DMA2_Stream6)
  {
    /* 清除DMA2流6的中断标志 */
    DMA2->HIFCR = DMA_Stream6_IT_MASK;
  }
  /* 特殊处理DMA2流7的情况 */
  else 
  {
    if (DMAy_Streamx == DMA2_Stream7)
    {
      /* 清除DMA2流7的中断标志 */
      DMA2->HIFCR = DMA_Stream7_IT_MASK;
    }
  }
}
/**
  * @brief  DMA流初始化函数，根据配置结构体参数初始化指定DMA流的硬件寄存器
  * @param  DMAy_Streamx: 目标DMA流，y=1/2选择DMA控制器，x=0-7选择具体流
  * @param  DMA_InitStruct: 初始化配置结构体指针，包含以下关键配置：
  *         - 通道选择/传输方向/缓冲区大小
  *         - 外设&存储器地址增量模式
  *         - 数据宽度/循环模式/优先级
  *         - 突发传输/FIFO配置
  * @retval 无
  * @note   操作流程：
  *         1. 必须先用DMA_GetCmdStatus()确认流已禁用
  *         2. 配置顺序：CR->FCR->NDTR->PAR->M0AR
  *         3. M1AR需在双缓冲模式下单独设置
  *         4. 直接模式与FIFO模式互斥（通过DMA_FIFOMode配置）
  */
void DMA_Init(DMA_Stream_TypeDef* DMAy_Streamx, DMA_InitTypeDef* DMA_InitStruct)
{
  uint32_t tmpreg = 0;  // 临时寄存器用于位操作

  /* 参数合法性检查：12项配置参数验证 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));  // DMA流指针有效性
  assert_param(IS_DMA_CHANNEL(DMA_InitStruct->DMA_Channel));  // 通道号0-7
  assert_param(IS_DMA_DIRECTION(DMA_InitStruct->DMA_DIR));  // 传输方向：外设<->存储器
  assert_param(IS_DMA_BUFFER_SIZE(DMA_InitStruct->DMA_BufferSize));  // 传输计数器值0-65535
  assert_param(IS_DMA_PERIPHERAL_INC_STATE(DMA_InitStruct->DMA_PeripheralInc));  // 外设地址递增使能
  assert_param(IS_DMA_MEMORY_INC_STATE(DMA_InitStruct->DMA_MemoryInc));  // 存储器地址递增使能
  assert_param(IS_DMA_PERIPHERAL_DATA_SIZE(DMA_InitStruct->DMA_PeripheralDataSize));  // 外设数据宽度：8/16/32位
  assert_param(IS_DMA_MEMORY_DATA_SIZE(DMA_InitStruct->DMA_MemoryDataSize));  // 存储器数据宽度：8/16/32位
  assert_param(IS_DMA_MODE(DMA_InitStruct->DMA_Mode));  // 循环模式使能
  assert_param(IS_DMA_PRIORITY(DMA_InitStruct->DMA_Priority));  // 流优先级：低/中/高/最高
  assert_param(IS_DMA_FIFO_MODE_STATE(DMA_InitStruct->DMA_FIFOMode));  // FIFO模式使能
  assert_param(IS_DMA_FIFO_THRESHOLD(DMA_InitStruct->DMA_FIFOThreshold));  // FIFO阈值：1/4, 1/2, 3/4, 满
  assert_param(IS_DMA_MEMORY_BURST(DMA_InitStruct->DMA_MemoryBurst));  // 存储器突发传输配置
  assert_param(IS_DMA_PERIPHERAL_BURST(DMA_InitStruct->DMA_PeripheralBurst));  // 外设突发传输配置

  /*----------------------- CR寄存器配置（控制寄存器）-----------------------*/
  /* 获取当前CR寄存器值（保留未配置的位） */
  tmpreg = DMAy_Streamx->CR;

  /* 清除需要配置的位域（共10个配置字段）：
     CHSEL[2:0]  通道选择       | MBURST[1:0] 存储器突发
     PBURST[1:0] 外设突发       | PL[1:0]    优先级
     MSIZE[1:0]  存储器数据宽度  | PSIZE[1:0] 外设数据宽度
     MINC        存储器地址递增  | PINC       外设地址递增
     CIRC        循环模式       | DIR[1:0]   传输方向 */
  tmpreg &= ~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST | 
              DMA_SxCR_PL | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE | 
              DMA_SxCR_MINC | DMA_SxCR_PINC | DMA_SxCR_CIRC | 
              DMA_SxCR_DIR);

  /* 组合新的配置参数（按位或操作）：
     注意：以下结构体成员实际为预定义的位掩码常量 */
  tmpreg |= DMA_InitStruct->DMA_Channel          // 通道选择位
          | DMA_InitStruct->DMA_DIR              // 传输方向
          | DMA_InitStruct->DMA_PeripheralInc    // 外设地址递增
          | DMA_InitStruct->DMA_MemoryInc        // 存储器地址递增
          | DMA_InitStruct->DMA_PeripheralDataSize // 外设数据宽度
          | DMA_InitStruct->DMA_MemoryDataSize   // 存储器数据宽度
          | DMA_InitStruct->DMA_Mode             // 循环模式
          | DMA_InitStruct->DMA_Priority         // 流优先级
          | DMA_InitStruct->DMA_MemoryBurst      // 存储器突发传输
          | DMA_InitStruct->DMA_PeripheralBurst; // 外设突发传输

  /* 写入配置到CR寄存器 */
  DMAy_Streamx->CR = tmpreg;

  /*----------------------- FCR寄存器配置（FIFO控制寄存器）-------------------*/
  tmpreg = DMAy_Streamx->FCR;  // 获取当前FCR寄存器值

  /* 清除配置位：
     DMDIS：直接模式禁用（0-启用直接模式，1-必须使用FIFO）
     FTH[1:0]：FIFO阈值选择 */
  tmpreg &= ~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);

  /* 设置FIFO相关配置：
     DMA_FIFOMode：使能/禁用FIFO模式
     DMA_FIFOThreshold：设置触发阈值 */
  tmpreg |= DMA_InitStruct->DMA_FIFOMode        // FIFO模式控制
          | DMA_InitStruct->DMA_FIFOThreshold; // FIFO阈值设置

  /* 更新FCR寄存器 */
  DMAy_Streamx->FCR = tmpreg;

  /*----------------------- NDTR寄存器配置（数据传输数量）--------------------*/
  /* 设置传输计数器：指定需要传输的数据项数量 */
  DMAy_Streamx->NDTR = DMA_InitStruct->DMA_BufferSize;

  /*----------------------- PAR寄存器配置（外设地址）-------------------------*/
  /* 设置外设基地址：数据交换的外设端口地址 */
  DMAy_Streamx->PAR = DMA_InitStruct->DMA_PeripheralBaseAddr;

  /*----------------------- M0AR寄存器配置（存储器地址0）---------------------*/
  /* 设置存储器基地址0：数据交换的存储器起始地址
     （当使用双缓冲模式时，需额外设置M1AR） */
  DMAy_Streamx->M0AR = DMA_InitStruct->DMA_Memory0BaseAddr;
}

/**
  * @brief  DMA配置结构体初始化函数
  * @功能   将DMA配置结构体成员初始化为安全默认值
  * @原则   保证未显式赋值的结构体不会引发意外DMA操作
  * @注意   应在声明结构体变量后立即调用本函数
  * @param  DMA_InitStruct 要初始化的DMA配置结构体指针
  * @retval None
  */
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct)
{
  /*------------------ DMA配置参数重置为默认值 ----------------------*/
  
  /* DMA通道选择（0-7对应DMA1，8-15对应DMA2）
     默认0通道，实际使用必须显式配置 */
  DMA_InitStruct->DMA_Channel = 0;

  /* 外设基地址初始化（防止野指针）
     实际使用需配置为USART_DR等外设数据寄存器地址 */
  DMA_InitStruct->DMA_PeripheralBaseAddr = 0;

  /* 存储器基地址0初始化（双缓冲模式需额外配置M1AR）
     实际使用需指向有效内存区域 */
  DMA_InitStruct->DMA_Memory0BaseAddr = 0;

  /* 数据传输方向（默认外设→存储器，常见于ADC采集等场景）
     可选：
     - DMA_DIR_PeripheralToMemory
     - DMA_DIR_MemoryToPeripheral 
     - DMA_DIR_MemoryToMemory */
  DMA_InitStruct->DMA_DIR = DMA_DIR_PeripheralToMemory;

  /* 缓冲区大小清零（必须显式配置有效值）
     实际传输数量=BufferSize+1 */
  DMA_InitStruct->DMA_BufferSize = 0;

  /* 外设地址自增禁用（适用于固定地址外设如USART_DR）
     使能后自动递增外设地址，用于访问连续外设寄存器 */
  DMA_InitStruct->DMA_PeripheralInc = DMA_PeripheralInc_Disable;

  /* 存储器地址自增禁用（适用于单目标地址传输）
     使能后自动递增存储地址，用于填充数组等场景 */
  DMA_InitStruct->DMA_MemoryInc = DMA_MemoryInc_Disable;

  /* 外设数据宽度默认1字节（8位）
     需与实际外设数据宽度匹配（如ADC 16位需改为HalfWord） */
  DMA_InitStruct->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

  /* 存储器数据宽度默认1字节（8位）
     需与内存变量类型匹配（如uint32_t数组应设为Word） */
  DMA_InitStruct->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

  /* 传输模式设为单次（Normal），循环模式需显式启用
     循环模式适用于持续采集等场景 */
  DMA_InitStruct->DMA_Mode = DMA_Mode_Normal;

  /* DMA请求优先级设为低（4个流组间优先级）
     同一流组内优先级由硬件固定 */
  DMA_InitStruct->DMA_Priority = DMA_Priority_Low;

  /* FIFO模式默认禁用（直接模式）
     使能后可通过FIFO缓冲数据，需配合阈值设置 */
  DMA_InitStruct->DMA_FIFOMode = DMA_FIFOMode_Disable;

  /* FIFO阈值设为1/4满（默认值，实际无效因FIFO模式禁用）
     使能FIFO后需根据数据流量调整 */
  DMA_InitStruct->DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;

  /* 存储器突发传输设为单次（默认安全模式）
     支持INCR4/8/16等突发模式提升效率 */
  DMA_InitStruct->DMA_MemoryBurst = DMA_MemoryBurst_Single;

  /* 外设突发传输设为单次（默认安全模式）
     突发模式需外设支持连续访问 */
  DMA_InitStruct->DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
}

/**
  * @brief  DMA流使能控制函数，用于启动或停止指定的DMA流传输
  * @param  DMAy_Streamx: 目标DMA流，y=1/2选择DMA控制器，x=0-7选择具体流
  * @param  NewState: 新状态
  *         - ENABLE: 激活DMA流传输
  *         - DISABLE: 停止/暂停DMA流传输
  * @retval 无
  * @note   关键行为说明：
  *         ENABLE操作：
  *         - 执行硬件级启动，可能触发DMA传输
  *         - 调用前需完成初始化（DMA_Init）
  *         - 建议后续调用DMA_GetCmdStatus()验证实际启动状态
  *         
  *         DISABLE操作：
  *         - 实现"硬件暂停"，已配置参数不会丢失
  *         - 若传输中进行暂停，会完成当前数据单元传输
  *         - 完全停止需要等待DMA_GetCmdStatus返回DISABLE
  */
void DMA_Cmd(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState)
{
  /* 参数有效性双重验证 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));  // 验证DMA流对象有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));     // 验证使能状态参数合法性

  if (NewState != DISABLE)
  {
    /* 使能目标DMA流：
       通过设置CR寄存器的EN位（第0位）激活DMA传输 
       - 当流未初始化时操作无效
       - 允许在暂停状态恢复传输（复用该函数实现继续传输） */
    DMAy_Streamx->CR |= (uint32_t)DMA_SxCR_EN;
  }
  else
  {
    /* 禁用目标DMA流：
       清除CR寄存器的EN位（第0位）暂停/停止DMA
       - 立即生效但传输可能不会立即终止
       - 若传输中调用，会完成当前数据单元的传输后进入禁用状态
       - 保持NDTR计数器值，保留传输进度信息 */
    DMAy_Streamx->CR &= ~(uint32_t)DMA_SxCR_EN;
  }
}
/**
  * @brief  DMA外设地址递增偏移配置函数
  * @功能   设置外设地址递增模式下的偏移量规则
  * @应用   需要非连续访问外设寄存器时（如32位对齐的寄存器阵列）
  * @原理   通过CR寄存器的PINCOS位控制地址递增逻辑
  * @注意   - 仅在DMA_PeripheralInc=ENABLE时生效
  *         - 使用32位对齐模式需确保外设寄存器地址4字节对齐
  * @param  DMAy_Streamx 目标DMA流（如DMA2_Stream3）
  * @param  DMA_Pincos 递增偏移模式：
  *            @arg DMA_PINCOS_Psize: 按数据宽度递增（字节/半字/字）
  *            @arg DMA_PINCOS_WordAligned: 固定+4字节递增（32位对齐）
  * @retval None
  */
void DMA_PeriphIncOffsetSizeConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_Pincos)
{
  /* 验证DMA流有效性（DMA1/DMA2的Stream0-7） */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  
  /* 验证递增模式参数合法性 */
  assert_param(IS_DMA_PINCOS_SIZE(DMA_Pincos));

  /* 配置外设地址递增偏移模式 */
  if(DMA_Pincos != DMA_PINCOS_Psize) // 需要设置32位对齐模式
  {
    /* 使能PINCOS位（CR[15]），地址按4字节递增 */
    DMAy_Streamx->CR |= (uint32_t)DMA_SxCR_PINCOS;     
  }
  else  // 常规数据尺寸对齐模式
  {
    /* 清除PINCOS位（CR[15]），地址按PSIZE配置递增 
       例如：当PSIZE=HalfWord时，每次递增2字节 */
    DMAy_Streamx->CR &= ~(uint32_t)DMA_SxCR_PINCOS;    
  }
}

/**
  * @brief  DMA流控制器配置函数
  * @功能   指定DMA传输流程的控制权归属
  * @应用   - 存储器控制：DMA自主传输（默认）
  *         - 外设控制：需外设触发传输（如SPI/TIM事件驱动）
  * @原理   通过CR寄存器的PFCTRL位选择流控主体
  * @注意   - 使用外设流控需确认外设硬件支持
  *         - 改变配置需先禁用DMA流
  * @param  DMAy_Streamx 目标DMA流（如DMA1_Stream5）
  * @param  DMA_FlowCtrl 流控制模式：
  *            @arg DMA_FlowCtrl_Memory: DMA自主控制（默认）
  *            @arg DMA_FlowCtrl_Peripheral: 外设事件驱动传输
  * @retval None
  */
void DMA_FlowControllerConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FlowCtrl)
{
  /* 验证DMA流有效性 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  
  /* 验证流控模式参数合法性 */
  assert_param(IS_DMA_FLOW_CTRL(DMA_FlowCtrl));

  /* 配置流控模式 */
  if(DMA_FlowCtrl != DMA_FlowCtrl_Memory) // 选择外设流控模式
  {
    /* 设置PFCTRL位（CR[5]），传输由外设发起
       每个传输需等待外设硬件信号（如TIM更新事件） */
    DMAy_Streamx->CR |= (uint32_t)DMA_SxCR_PFCTRL;   
  }
  else  // 选择存储器流控模式（DMA自主传输）
  {
    /* 清除PFCTRL位（CR[5]），DMA按配置自动发起传输 */
    DMAy_Streamx->CR &= ~(uint32_t)DMA_SxCR_PFCTRL;    
  }
}

/**
  * @}
  */

/** @defgroup DMA_Group2 Data Counter functions
 *  @brief   Data Counter functions 
 *
@verbatim   
 ===============================================================================
                      ##### Data Counter functions #####
 ===============================================================================  
    [..]
    This subsection provides function allowing to configure and read the buffer size
    (number of data to be transferred). 
    [..]
    The DMA data counter can be written only when the DMA Stream is disabled 
    (ie. after transfer complete event).
    [..]
    The following function can be used to write the Stream data counter value:
      (+) void DMA_SetCurrDataCounter(DMA_Stream_TypeDef* DMAy_Streamx, uint16_t Counter);
      -@- It is advised to use this function rather than DMA_Init() in situations 
          where only the Data buffer needs to be reloaded.
      -@- If the Source and Destination Data Sizes are different, then the value 
          written in data counter, expressing the number of transfers, is relative 
          to the number of transfers from the Peripheral point of view.
          ie. If Memory data size is Word, Peripheral data size is Half-Words, 
          then the value to be configured in the data counter is the number 
          of Half-Words to be transferred from/to the peripheral.
    [..]
    The DMA data counter can be read to indicate the number of remaining transfers for
    the relative DMA Stream. This counter is decremented at the end of each data 
    transfer and when the transfer is complete: 
      (+) If Normal mode is selected: the counter is set to 0.
      (+) If Circular mode is selected: the counter is reloaded with the initial value
          (configured before enabling the DMA Stream)
     [..]
     The following function can be used to read the Stream data counter value:
       (+) uint16_t DMA_GetCurrDataCounter(DMA_Stream_TypeDef* DMAy_Streamx);

@endverbatim
  * @{
  */

/**
  * @brief  Writes the number of data units to be transferred on the DMAy Streamx.
  * @param  DMAy_Streamx: where y can be 1 or 2 to select the DMA and x can be 0
  *          to 7 to select the DMA Stream.
  * @param  Counter: Number of data units to be transferred (from 0 to 65535) 
  *          Number of data items depends only on the Peripheral data format.
  *            
  * @note   If Peripheral data format is Bytes: number of data units is equal 
  *         to total number of bytes to be transferred.
  *           
  * @note   If Peripheral data format is Half-Word: number of data units is  
  *         equal to total number of bytes to be transferred / 2.
  *           
  * @note   If Peripheral data format is Word: number of data units is equal 
  *         to total  number of bytes to be transferred / 4.
  *      
  * @note   In Memory-to-Memory transfer mode, the memory buffer pointed by 
  *         DMAy_SxPAR register is considered as Peripheral.
  *      
  * @retval The number of remaining data units in the current DMAy Streamx transfer.
  */
/**
  * @brief  DMA当前传输数据计数器设置函数
  * @功能   配置DMA流传输的数据单元总数，用于初始化或动态修改传输量
  * @原理   NDTR寄存器决定传输数据单元数量，传输时自动递减至0
  * @注意   - 需在DMA流禁用状态下调用（CR.EN=0）
  *         - 数据单元大小由PSIZE/MSIZE决定（字节/半字/字）
  * @场景   - 动态调整后续传输长度
  *         - 重新启动已完成的传输流
  * @param  DMAy_Streamx 目标DMA流（如DMA1_Stream3）
  * @param  Counter 要传输的数据单元数（实际传输次数=Counter+1）
  * @示例   设置256字节传输（PSIZE=Byte）: Counter=255
  * @retval None
  */
void DMA_SetCurrDataCounter(DMA_Stream_TypeDef* DMAy_Streamx, uint16_t Counter)
{
  /* 验证DMA流有效性（y=1/2，x=0-7） */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));

  /* 写NDTR寄存器（数据传输计数器） 
     - 写入值=期望传输次数-1 
     - 写入即生效，传输开始后自动递减 */
  DMAy_Streamx->NDTR = (uint16_t)Counter;
}

/**
  * @brief  DMA当前剩余数据计数器读取函数
  * @功能   获取当前剩余的待传输数据单元数量
  * @特性   - 实时反映NDTR寄存器值（传输中持续变化）
  *         - 在循环模式中返回当前块剩余量
  * @应用   - 监测传输进度
  *         - 计算剩余传输时间
  * @注意   - 读取时传输可能正在进行，数值为瞬时值
  *         - 双缓冲模式下仅反映当前活动的缓冲区计数
  * @param  DMAy_Streamx 目标DMA流（如DMA2_Stream5）
  * @retval 剩余的数据单元数（实际值=返回值+1）
  * @示例   返回255表示还剩256个数据单元待传输
  */
uint16_t DMA_GetCurrDataCounter(DMA_Stream_TypeDef* DMAy_Streamx)
{
  /* 验证DMA流有效性 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));

  /* 读取NDTR寄存器并强制转换（返回剩余计数+1）
     因NDTR寄存器设计为：初始值N对应N+1次传输 */
  return ((uint16_t)(DMAy_Streamx->NDTR));
}

/**
  * @}
  */

/** @defgroup DMA_Group3 Double Buffer mode functions
 *  @brief   Double Buffer mode functions 
 *
@verbatim   
 ===============================================================================
                    ##### Double Buffer mode functions #####
 ===============================================================================  
    [..]
    This subsection provides function allowing to configure and control the double 
    buffer mode parameters.
    
    [..]
    The Double Buffer mode can be used only when Circular mode is enabled.
    The Double Buffer mode cannot be used when transferring data from Memory to Memory.
    
    [..]
    The Double Buffer mode allows to set two different Memory addresses from/to which
    the DMA controller will access alternatively (after completing transfer to/from 
    target memory 0, it will start transfer to/from target memory 1).
    This allows to reduce software overhead for double buffering and reduce the CPU
    access time.
    
    [..]
    Two functions must be called before calling the DMA_Init() function:
      (+) void DMA_DoubleBufferModeConfig(DMA_Stream_TypeDef* DMAy_Streamx, 
          uint32_t Memory1BaseAddr, uint32_t DMA_CurrentMemory);
      (+) void DMA_DoubleBufferModeCmd(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState);
      
    [..]
    DMA_DoubleBufferModeConfig() is called to configure the Memory 1 base address 
    and the first Memory target from/to which the transfer will start after 
    enabling the DMA Stream. Then DMA_DoubleBufferModeCmd() must be called 
    to enable the Double Buffer mode (or disable it when it should not be used).
  
    [..]
    Two functions can be called dynamically when the transfer is ongoing (or when the DMA Stream is 
    stopped) to modify on of the target Memories addresses or to check wich Memory target is currently
    used:
      (+) void DMA_MemoryTargetConfig(DMA_Stream_TypeDef* DMAy_Streamx, 
                uint32_t MemoryBaseAddr, uint32_t DMA_MemoryTarget);
      (+) uint32_t DMA_GetCurrentMemoryTarget(DMA_Stream_TypeDef* DMAy_Streamx);
      
    [..]
    DMA_MemoryTargetConfig() can be called to modify the base address of one of 
    the two target Memories.
    The Memory of which the base address will be modified must not be currently 
    be used by the DMA Stream (ie. if the DMA Stream is currently transferring 
    from Memory 1 then you can only modify base address of target Memory 0 and vice versa).
    To check this condition, it is recommended to use the function DMA_GetCurrentMemoryTarget() which
    returns the index of the Memory target currently in use by the DMA Stream.

@endverbatim
  * @{
  */
  /**
  * @brief  DMA双缓冲模式配置函数
  * @功能   初始化双缓冲模式下的第二缓冲区地址并指定起始缓冲区
  * @原理   配置M1AR寄存器及CR.CT位实现双缓冲内存管理
  * @场景   需要持续数据传输且避免内存拷贝的场景（如音频流、图像采集）
  * @注意   - 必须在DMA流禁用状态下调用
  *         - 第一缓冲区地址通过DMA_Init()中的M0AR设置
  * @param  DMAy_Streamx 目标DMA流（如DMA2_Stream4）
  * @param  Memory1BaseAddr 第二缓冲区物理地址（需4字节对齐）
  * @param  DMA_CurrentMemory 初始活动缓冲区：
  *            @arg DMA_Memory_0: 从Memory0开始传输
  *            @arg DMA_Memory_1: 从Memory1开始传输
  * @retval None
  */
void DMA_DoubleBufferModeConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t Memory1BaseAddr,
                                uint32_t DMA_CurrentMemory)
{  
  /* 验证DMA流有效性 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  
  /* 验证当前内存选择参数 */
  assert_param(IS_DMA_CURRENT_MEM(DMA_CurrentMemory));

  /* 设置当前活动缓冲区 */
  if (DMA_CurrentMemory != DMA_Memory_0) 
  {
    /* 设置CR[19] CT位，选择Memory1为起始缓冲区 */
    DMAy_Streamx->CR |= (uint32_t)(DMA_SxCR_CT);    
  }
  else
  {
    /* 清除CR[19] CT位，选择Memory0为起始缓冲区 */
    DMAy_Streamx->CR &= ~(uint32_t)(DMA_SxCR_CT);    
  }

  /* 配置第二缓冲区基地址到M1AR寄存器 
     （硬件会自动在缓冲区间切换） */
  DMAy_Streamx->M1AR = Memory1BaseAddr;
}

/**
  * @brief  DMA双缓冲模式开关函数
  * @功能   全局启用/禁用双缓冲模式
  * @特性   - 禁用时会退回到单缓冲模式（仅使用M0AR）
  *         - 切换状态必须在DMA流禁用时进行
  * @参数   DMAy_Streamx 目标DMA流
  * @param  NewState 双缓冲模式状态（ENABLE/DISABLE）
  * @注意   切换模式前必须调用DMA_DoubleBufferModeConfig完成初始化
  * @retval None
  */
void DMA_DoubleBufferModeCmd(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState)
{  
  /* 验证DMA流有效性 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  
  /* 验证功能状态参数 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 配置双缓冲模式开关 */
  if (NewState != DISABLE)
  {
    /* 设置CR[18] DBM位，使能双缓冲模式 */
    DMAy_Streamx->CR |= (uint32_t)DMA_SxCR_DBM;
  }
  else
  {
    /* 清除CR[18] DBM位，禁用双缓冲模式 */
    DMAy_Streamx->CR &= ~(uint32_t)DMA_SxCR_DBM;
  }
}

/**
  * @brief  DMA双缓冲目标内存动态配置函数
  * @功能   运行时动态更新非活动缓冲区的内存地址
  * @原理   在传输过程中更新当前未使用的缓冲区地址
  * @场景   需要循环重用或动态切换缓冲区的实时系统
  * @注意   - 只能修改非当前活动的缓冲区地址
  *         - 更新地址需内存对齐（与PSIZE/MSIZE匹配）
  * @param  DMAy_Streamx 目标DMA流
  * @param  MemoryBaseAddr 新缓冲区物理地址
  * @param  DMA_MemoryTarget 要更新的缓冲区：
  *            @arg DMA_Memory_0: 更新Memory0地址（仅当当前使用Memory1时）
  *            @arg DMA_Memory_1: 更新Memory1地址（仅当当前使用Memory0时）
  * @retval None
  */
void DMA_MemoryTargetConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t MemoryBaseAddr,
                           uint32_t DMA_MemoryTarget)
{
  /* 验证DMA流有效性 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  
  /* 验证内存目标参数 */
  assert_param(IS_DMA_CURRENT_MEM(DMA_MemoryTarget));
    
  /* 安全更新非活动缓冲区地址 */
  if (DMA_MemoryTarget != DMA_Memory_0)
  {
    /* 更新第二缓冲区地址（M1AR） */
    DMAy_Streamx->M1AR = MemoryBaseAddr;    
  }  
  else
  {
    /* 更新第一缓冲区地址（M0AR） */
    DMAy_Streamx->M0AR = MemoryBaseAddr;  
  }
}

/**
  * @brief  DMA当前内存目标查询函数
  * @功能   获取双缓冲模式下当前活动的内存缓冲区编号
  * @原理   通过CR寄存器的CT位状态判断当前使用缓冲区
  * @应用   在中断服务中确定有效数据位置（如DMA半传输/传输完成中断）
  * @注意   - 仅在双缓冲模式启用时有效
  *         - 结果反映查询时刻状态，可能在返回后立即切换
  * @param  DMAy_Streamx 目标DMA流（如DMA1_Stream6）
  * @retval 当前活动缓冲区编号：
  *         - 0: Memory0 (M0AR指向的缓冲区)
  *         - 1: Memory1 (M1AR指向的缓冲区)
  */
uint32_t DMA_GetCurrentMemoryTarget(DMA_Stream_TypeDef* DMAy_Streamx)
{
  uint32_t tmp = 0;
  
  /* 验证DMA流有效性（防止访问非法寄存器） */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));

  /* 读取CR[19] CT位状态判断当前缓冲区 */
  if ((DMAy_Streamx->CR & DMA_SxCR_CT) != 0)
  {
    /* CT位=1：硬件下一事务将使用/当前正在使用Memory1 
       （此时用户应处理Memory0数据） */
    tmp = 1;
  }  
  else
  {
    /* CT位=0：硬件下一事务将使用/当前正在使用Memory0 
       （此时用户应处理Memory1数据） */
    tmp = 0;    
  }
  return tmp;
}

/**
  * @}
  */

/** @defgroup DMA_Group4 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions 
 *
@verbatim   
 ===============================================================================
              ##### Interrupts and flags management functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to
      (+) Check the DMA enable status
      (+) Check the FIFO status 
      (+) Configure the DMA Interrupts sources and check or clear the flags or 
          pending bits status.  
           
    [..]
      (#) DMA Enable status:
          After configuring the DMA Stream (DMA_Init() function) and enabling 
          the stream, it is recommended to check (or wait until) the DMA Stream 
          is effectively enabled. A Stream may remain disabled if a configuration 
          parameter is wrong. After disabling a DMA Stream, it is also recommended 
          to check (or wait until) the DMA Stream is effectively disabled. 
          If a Stream is disabled while a data transfer is ongoing, the current 
          data will be transferred and the Stream will be effectively disabled 
          only after this data transfer completion.
          To monitor this state it is possible to use the following function:
        (++) FunctionalState DMA_GetCmdStatus(DMA_Stream_TypeDef* DMAy_Streamx); 
 
      (#) FIFO Status:
          It is possible to monitor the FIFO status when a transfer is ongoing 
          using the following function:
        (++) uint32_t DMA_GetFIFOStatus(DMA_Stream_TypeDef* DMAy_Streamx); 
 
      (#) DMA Interrupts and Flags:
          The user should identify which mode will be used in his application 
          to manage the DMA controller events: Polling mode or Interrupt mode. 
    
    *** Polling Mode ***
    ====================
    [..]
    Each DMA stream can be managed through 4 event Flags:
    (x : DMA Stream number )
      (#) DMA_FLAG_FEIFx  : to indicate that a FIFO Mode Transfer Error event occurred.
      (#) DMA_FLAG_DMEIFx : to indicate that a Direct Mode Transfer Error event occurred.
      (#) DMA_FLAG_TEIFx  : to indicate that a Transfer Error event occurred.
      (#) DMA_FLAG_HTIFx  : to indicate that a Half-Transfer Complete event occurred.
      (#) DMA_FLAG_TCIFx  : to indicate that a Transfer Complete event occurred .       
    [..]
    In this Mode it is advised to use the following functions:
      (+) FlagStatus DMA_GetFlagStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG);
      (+) void DMA_ClearFlag(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG);

    *** Interrupt Mode ***
    ======================
    [..]
    Each DMA Stream can be managed through 4 Interrupts:

    *** Interrupt Source ***
    ========================
    [..]
      (#) DMA_IT_FEIFx  : specifies the interrupt source for the  FIFO Mode Transfer Error event.
      (#) DMA_IT_DMEIFx : specifies the interrupt source for the Direct Mode Transfer Error event.
      (#) DMA_IT_TEIFx  : specifies the interrupt source for the Transfer Error event.
      (#) DMA_IT_HTIFx  : specifies the interrupt source for the Half-Transfer Complete event.
      (#) DMA_IT_TCIFx  : specifies the interrupt source for the a Transfer Complete event. 
    [..]
    In this Mode it is advised to use the following functions:
      (+) void DMA_ITConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT, FunctionalState NewState);
      (+) ITStatus DMA_GetITStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT);
      (+) void DMA_ClearITPendingBit(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT);

@endverbatim
  * @{
  */

/**
  * @brief  获取指定DMA流的使能状态
  * @param  DMAy_Streamx: 需要查询的DMA流，y可以是1或2选择DMA控制器，
  *         x可以是0-7选择具体的DMA流
  * @retval FunctionalState 返回状态：
  *         - ENABLE: 流正在运行/数据传输中
  *         - DISABLE: 流已停止且传输完成
  * @note   重要应用场景：
  *         1. 在调用DMA_Init()初始化后，应检查该状态确认流是否成功激活
  *         2. 在调用DMA_Cmd(DISABLE)禁用后，需轮询该状态直到返回DISABLE
  *         3. 当配置参数错误时，流可能无法真正激活（保持DISABLE状态）
  *         4. 禁用操作具有延迟效果，需等待当前传输完成才能完全停止
  */
FunctionalState DMA_GetCmdStatus(DMA_Stream_TypeDef* DMAy_Streamx)
{
  FunctionalState state = DISABLE;

  /* 硬件参数有效性验证：确保传入的是合法的DMA流指针 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));

  /* 通过位掩码DMA_SxCR_EN（0x00000001）检测控制寄存器CR的使能位状态
     - 当EN位=1：DMA流处于激活状态，可能正在传输数据或保持就绪状态
     - 当EN位=0：DMA流处于非活动状态，且所有传输事务已完全终止 */
  if ((DMAy_Streamx->CR & (uint32_t)DMA_SxCR_EN) != 0)
  {
    /* 检测到使能位有效的情况：
       此时DMA控制器可能处于：
       - 正在进行数据传输
       - 处于挂起状态等待传输触发
       - 已完成配置但尚未开始传输 */
    state = ENABLE;
  }
  else
  {
    /* 使能位无效的情况：
       表示DMA流已经完全停止，且满足以下条件之一：
       - 从未被启用过
       - 已被成功禁用且最后一包数据传输完成
       - 硬件复位后的初始状态 */
    state = DISABLE;
  }
  
  /* 返回状态值供上层逻辑判断：
     - 当返回ENABLE时，禁止修改流配置寄存器
     - 当返回DISABLE时，可以安全进行配置修改或重新初始化 */
  return state;
}

/**
  * @brief  DMA FIFO状态查询函数
  * @功能   获取指定DMA流的FIFO缓冲区填充状态
  * @原理   读取FCR寄存器的FS[3:0]位域值
  * @应用   流量控制、防止FIFO溢出/欠载
  * @注意   - 仅在FIFO模式启用时有效（CR.DBM=1）
  *         - 状态值反映查询时刻的瞬时状态
  * @param  DMAy_Streamx 目标DMA流（如DMA2_Stream3）
  * @retval FIFO填充状态：
  *         - DMA_FIFOStatus_Empty:        FIFO空
  *         - DMA_FIFOStatus_Less1QuarterFull: 填充量<1/4且非空
  *         - DMA_FIFOStatus_1QuarterFull:  填充量≥1/4
  *         - DMA_FIFOStatus_HalfFull:     填充量≥1/2
  *         - DMA_FIFOStatus_3QuartersFull:填充量≥3/4
  *         - DMA_FIFOStatus_Full:         FIFO已满
  */
uint32_t DMA_GetFIFOStatus(DMA_Stream_TypeDef* DMAy_Streamx)
{
  uint32_t tmpreg = 0;
 
  /* 验证DMA流有效性 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  
  /* 提取FCR[3:0] FS位域（FIFO Status） */
  tmpreg = (uint32_t)((DMAy_Streamx->FCR & DMA_SxFCR_FS));
  
  return tmpreg;
}

/**
  * @brief  DMA标志位状态查询函数
  * @功能   检测指定DMA流的事件标志位状态
  * @原理   通过LISR/HISR寄存器查询事件标志
  * @应用   中断服务程序中的事件判断
  * @注意   - 标志位状态查询后不会自动清除
  *         - 需配合DMA_ClearFlag()使用
  * @param  DMAy_Streamx 目标DMA流（如DMA1_Stream5）
  * @param  DMA_FLAG 要检测的标志位：
  *            @arg DMA_FLAG_TCIFx:  传输完成标志（x=0-7）
  *            @arg DMA_FLAG_HTIFx:  半传输完成标志
  *            @arg DMA_FLAG_TEIFx:  传输错误标志
  *            @arg DMA_FLAG_DMEIFx: 直接模式错误标志
  *            @arg DMA_FLAG_FEIFx:  FIFO错误标志
  * @retval 标志位状态：SET(1)-标志置位 / RESET(0)-标志未置
  */
FlagStatus DMA_GetFlagStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG)
{
  FlagStatus bitstatus = RESET;
  DMA_TypeDef* DMAy;
  uint32_t tmpreg = 0;

  /* 验证DMA流和标志位参数有效性 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  assert_param(IS_DMA_GET_FLAG(DMA_FLAG));

  /* 确定DMA控制器归属（DMA1/DMA2） */
  if (DMAy_Streamx < DMA2_Stream0)  // 通过流基地址判断控制器
  {
    DMAy = DMA1;  // 属于DMA1控制器
  } 
  else 
  {
    DMAy = DMA2;  // 属于DMA2控制器
  }

  /* 判断标志位属于高位还是低位状态寄存器 */
  if ((DMA_FLAG & HIGH_ISR_MASK) != 0)  // 高位标志（流4-7）
  {
    tmpreg = DMAy->HISR;  // 读取高位中断状态寄存器
  }
  else  // 低位标志（流0-3）
  {
    tmpreg = DMAy->LISR;  // 读取低位中断状态寄存器
  }   
 
  /* 屏蔽保留位（避免误判） */
  tmpreg &= RESERVED_MASK;

  /* 检测指定标志位状态 */
  if ((tmpreg & DMA_FLAG) != 0) 
  {
    bitstatus = SET;  // 标志位置位
  }
  else
  {
    bitstatus = RESET; // 标志位未置
  }

  return bitstatus;
}
/**
  * @brief  DMA标志位清除函数
  * @功能   清除指定DMA流的中断/事件标志位
  * @原理   通过写1到LIFCR/HIFCR寄存器的对应位实现标志清除
  * @注意   - 必须在中断服务程序中调用以重置标志
  *         - 清除操作不可逆，需确保已完成事件处理
  * @param  DMAy_Streamx 目标DMA流（如DMA2_Stream1）
  * @param  DMA_FLAG 要清除的标志位组合，支持以下按位或：
  *            @arg DMA_FLAG_TCIFx: 传输完成标志
  *            @arg DMA_FLAG_HTIFx: 半传输标志  
  *            @arg DMA_FLAG_TEIFx: 传输错误标志
  *            @arg DMA_FLAG_DMEIFx: 直接模式错误标志
  *            @arg DMA_FLAG_FEIFx: FIFO错误标志
  *            （x=0-7对应具体流编号）
  * @retval None
  */
void DMA_ClearFlag(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG)
{
  DMA_TypeDef* DMAy;

  /* 验证DMA流和标志位参数合法性 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  assert_param(IS_DMA_CLEAR_FLAG(DMA_FLAG));

  /* 确定DMA控制器归属（DMA1/DMA2） */
  if (DMAy_Streamx < DMA2_Stream0)  // 通过流基地址判断控制器
  {
    DMAy = DMA1;  // 目标属于DMA1控制器
  } 
  else 
  {
    DMAy = DMA2;  // 目标属于DMA2控制器
  }

  /* 根据标志位类型选择清除寄存器 */
  if ((DMA_FLAG & HIGH_ISR_MASK) != 0)  // 高位标志（流4-7）
  {
    /* 写入HIFCR寄存器清除高位标志（写1有效）
       保留位掩码确保不修改无关位 */
    DMAy->HIFCR = (uint32_t)(DMA_FLAG & RESERVED_MASK);
  }
  else  // 低位标志（流0-3）
  {
    /* 写入LIFCR寄存器清除低位标志（写1有效）
       保留位掩码确保不修改无关位 */  
    DMAy->LIFCR = (uint32_t)(DMA_FLAG & RESERVED_MASK);
  }    
}

/**
  * @brief  DMA流中断配置函数，用于启用或禁用指定类型的中断请求
  * @param  DMAy_Streamx: 目标DMA流，y=1/2选择DMA控制器，x=0-7选择具体流
  * @param DMA_IT: 中断类型组合，可用以下值按位或：
  *         - DMA_IT_TC  传输完成中断
  *         - DMA_IT_HT  半传输完成中断
  *         - DMA_IT_TE  传输错误中断
  *         - DMA_IT_FE  FIFO错误中断
  * @param  NewState: 中断状态
  *         - ENABLE: 使能指定中断
  *         - DISABLE: 禁用指定中断
  * @retval 无
  * @note   重要说明：
  *         1. FE(FIFO错误)中断通过FCR寄存器控制，其他中断通过CR寄存器控制
  *         2. 多个中断类型可组合配置（如DMA_IT_TC|DMA_IT_HT）
  *         3. 启用中断前需配置NVIC中断控制器
  *         4. 实际中断触发需满足：全局中断使能+流中断使能+事件发生
  */
void DMA_ITConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT, FunctionalState NewState)
{
  /* 三重参数校验 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));  // DMA流有效性检查
  assert_param(IS_DMA_CONFIG_IT(DMA_IT));         // 中断类型合法性检查（0x1E00范围）
  assert_param(IS_FUNCTIONAL_STATE(NewState));    // 状态参数有效性检查

  /* FIFO错误中断单独处理（DMA_IT_FE = 0x00000080） */
  if ((DMA_IT & DMA_IT_FE) != 0)
  {
    if (NewState != DISABLE)
    {
      /* 使能FIFO错误中断：设置FCR寄存器的FEIE位（第7位）
         - 当FIFO下溢/上溢时触发
         - 需同时启用流传输错误中断(TE)以捕获错误源 */
      DMAy_Streamx->FCR |= (uint32_t)DMA_IT_FE;
    }    
    else 
    {
      /* 禁用FIFO错误中断：清除FCR寄存器的FEIE位 */
      DMAy_Streamx->FCR &= ~(uint32_t)DMA_IT_FE;  
    }
  }

  /* 处理传输相关中断（TC/HT/TE） */
  if (DMA_IT != DMA_IT_FE)  // 排除已经处理的FE中断
  {
    if (NewState != DISABLE)
    {
      /* 启用传输中断：通过CR寄存器设置
         - TRANSFER_IT_ENABLE_MASK(0x1E00)过滤有效位
         - 可能的位操作：
           TCIE(位4): 传输完成中断
           HTIE(位3): 半传输中断 
           TEIE(位2): 传输错误中断 */
      DMAy_Streamx->CR |= (uint32_t)(DMA_IT  & TRANSFER_IT_ENABLE_MASK);
    }
    else
    {
      /* 禁用传输中断：清除CR寄存器对应位
         - 位清除操作不影响其他已启用的中断类型 */
      DMAy_Streamx->CR &= ~(uint32_t)(DMA_IT & TRANSFER_IT_ENABLE_MASK);
    }    
  }
}

/**
  * @brief  DMA中断状态查询函数
  * @功能   检测指定DMA流的中断是否发生（需中断使能且标志置位）
  * @原理   双重验证机制：中断使能位+状态标志位
  * @注意   - 仅当对应中断使能且标志置位时返回SET
  *         - 适用于精确判断有效中断事件
  * @param  DMAy_Streamx 目标DMA流（如DMA1_Stream4）
  * @param  DMA_IT 中断类型，支持以下按位或：
  *            @arg DMA_IT_TCIFx: 传输完成中断
  *            @arg DMA_IT_HTIFx: 半传输中断
  *            @arg DMA_IT_TEIFx: 传输错误中断
  *            @arg DMA_IT_DMEIFx: 直接模式错误中断
  *            @arg DMA_IT_FEIFx: FIFO错误中断
  * @retval 中断状态：SET(有效中断)/RESET(无中断或未使能)
  */
ITStatus DMA_GetITStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT)
{
  ITStatus bitstatus = RESET;
  DMA_TypeDef* DMAy;
  uint32_t tmpreg = 0, enablestatus = 0;

  /* 验证DMA流和中断类型合法性 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  assert_param(IS_DMA_GET_IT(DMA_IT));
 
  /* 确定DMA控制器归属（DMA1/DMA2） */
  if (DMAy_Streamx < DMA2_Stream0) 
  {
    DMAy = DMA1;  // 属于DMA1控制器
  } 
  else 
  {
    DMAy = DMA2;  // 属于DMA2控制器
  }

  /* 检查中断使能位位置（CR或FCR寄存器） */
  if ((DMA_IT & TRANSFER_IT_MASK) != 0)  // 传输相关中断（TC/HT/TE/DME）
  {
    /* 计算CR寄存器中的中断使能位偏移：
       DMA_IT值右移11位对齐CR寄存器位位置 */
    tmpreg = (uint32_t)((DMA_IT >> 11) & TRANSFER_IT_ENABLE_MASK);   
    
    /* 读取CR寄存器中的中断使能状态 */
    enablestatus = (uint32_t)(DMAy_Streamx->CR & tmpreg);
  }
  else  // FIFO错误中断（FE）
  {
    /* 检查FCR寄存器中的FEIE使能位 */
    enablestatus = (uint32_t)(DMAy_Streamx->FCR & DMA_IT_FE); 
  }
 
  /* 根据中断类型选择LISR/HISR状态寄存器 */
  if ((DMA_IT & HIGH_ISR_MASK) != 0)  // 高位中断（流4-7）
  {
    tmpreg = DMAy->HISR ;  // 读取高位中断状态
  }
  else  // 低位中断（流0-3）
  {
    tmpreg = DMAy->LISR ;  // 读取低位中断状态
  } 

  /* 屏蔽保留位（防止误判） */
  tmpreg &= RESERVED_MASK;

  /* 双重验证：标志位置位且中断使能 */
  if (((tmpreg & DMA_IT) != 0) && (enablestatus != 0))
  {
    bitstatus = SET;  // 有效中断
  }
  else
  {
    bitstatus = RESET; // 无效或未使能中断
  }

  return bitstatus;
}

/**
  * @brief  DMA中断挂起位清除函数
  * @功能   清除指定DMA流的中断挂起标志
  * @原理   通过写1到LIFCR/HIFCR寄存器对应位清除标志
  * @注意   - 必须在中断服务程序中调用
  *         - 清除操作不可逆，需先完成事件处理
  * @param  DMAy_Streamx 目标DMA流（如DMA2_Stream5）
  * @param  DMA_IT 要清除的中断标志组合
  * @retval None
  */
void DMA_ClearITPendingBit(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT)
{
  DMA_TypeDef* DMAy;

  /* 验证DMA流和中断类型参数 */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  assert_param(IS_DMA_CLEAR_IT(DMA_IT));

  /* 确定DMA控制器归属 */
  if (DMAy_Streamx < DMA2_Stream0)
  {
    DMAy = DMA1;  // 属于DMA1
  } 
  else 
  {
    DMAy = DMA2;  // 属于DMA2
  }

  /* 选择高位或低位清除寄存器 */
  if ((DMA_IT & HIGH_ISR_MASK) != 0) 
  {
    /* 写入HIFCR清除高位中断标志 */
    DMAy->HIFCR = (uint32_t)(DMA_IT & RESERVED_MASK);
  }
  else 
  {
    /* 写入LIFCR清除低位中断标志 */
    DMAy->LIFCR = (uint32_t)(DMA_IT & RESERVED_MASK);
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

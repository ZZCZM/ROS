/**
  ******************************************************************************
  * @file    stm32f4xx_usart.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Universal synchronous asynchronous receiver
  *          transmitter (USART):           
  *           + Initialization and Configuration
  *           + Data transfers
  *           + Multi-Processor Communication
  *           + LIN mode
  *           + Half-duplex mode
  *           + Smartcard mode
  *           + IrDA mode
  *           + DMA transfers management
  *           + Interrupts and flags management 
  *           
  @verbatim       
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
    [..]
      (#) Enable peripheral clock using the following functions
          RCC_APB2PeriphClockCmd(RCC_APB2Periph_USARTx, ENABLE) for USART1 and USART6 
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_USARTx, ENABLE) for USART2, USART3, 
          UART4 or UART5.
  
      (#) According to the USART mode, enable the GPIO clocks using 
          RCC_AHB1PeriphClockCmd() function. (The I/O can be TX, RX, CTS, 
          or/and SCLK). 
  
      (#) Peripheral's alternate function: 
        (++) Connect the pin to the desired peripherals' Alternate 
            Function (AF) using GPIO_PinAFConfig() function
        (++) Configure the desired pin in alternate function by:
            GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
        (++) Select the type, pull-up/pull-down and output speed via 
            GPIO_PuPd, GPIO_OType and GPIO_Speed members
        (++) Call GPIO_Init() function
          
      (#) Program the Baud Rate, Word Length , Stop Bit, Parity, Hardware 
          flow control and Mode(Receiver/Transmitter) using the USART_Init()
          function.
  
      (#) For synchronous mode, enable the clock and program the polarity,
          phase and last bit using the USART_ClockInit() function.
  
      (#) Enable the NVIC and the corresponding interrupt using the function 
         USART_ITConfig() if you need to use interrupt mode. 
  
      (#) When using the DMA mode 
        (++) Configure the DMA using DMA_Init() function
        (++) Active the needed channel Request using USART_DMACmd() function
   
      (#) Enable the USART using the USART_Cmd() function.
   
      (#) Enable the DMA using the DMA_Cmd() function, when using DMA mode. 
    
      -@- Refer to Multi-Processor, LIN, half-duplex, Smartcard, IrDA sub-sections
          for more details
    
    [..]        
    In order to reach higher communication baudrates, it is possible to
    enable the oversampling by 8 mode using the function USART_OverSampling8Cmd().
    This function should be called after enabling the USART clock (RCC_APBxPeriphClockCmd())
    and before calling the function USART_Init().
            
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
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup USART 
  * @brief USART driver modules
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*!< USART CR1寄存器清除掩码 ((~(uint16_t)0xE9F3)) */
#define CR1_CLEAR_MASK            ((uint16_t)(USART_CR1_M | USART_CR1_PCE | \
                                              USART_CR1_PS | USART_CR1_TE | \
                                              USART_CR1_RE))
/* 功能：用于清除CR1寄存器关键配置位的掩码
 * 包含位：
 * - USART_CR1_M    : 数据位长度配置（0: 8位 / 1: 9位）
 * - USART_CR1_PCE   : 校验控制使能位
 * - USART_CR1_PS    : 校验类型选择（0: 偶校验 / 1: 奇校验）
 * - USART_CR1_TE    : 发送器使能
 * - USART_CR1_RE    : 接收器使能
 * 使用场景：重新配置通信参数前清除旧设置 */

/*!< USART CR2寄存器时钟位清除掩码 ((~(uint16_t)0xF0FF)) */
#define CR2_CLOCK_CLEAR_MASK      ((uint16_t)(USART_CR2_CLKEN | USART_CR2_CPOL | \
                                              USART_CR2_CPHA | USART_CR2_LBCL))
/* 功能：清除CR2中时钟相关配置位的掩码
 * 包含位：
 * - USART_CR2_CLKEN : 时钟使能（同步模式）
 * - USART_CR2_CPOL  : 时钟极性（0: 低电平空闲 / 1: 高电平空闲）
 * - USART_CR2_CPHA  : 时钟相位（0: 第1边沿采样 / 1: 第2边沿采样）
 * - USART_CR2_LBCL  : 最后位时钟脉冲
 * 使用场景：切换同步/异步模式时重置时钟配置 */

/*!< USART CR3寄存器清除掩码 ((~(uint16_t)0xFCFF)) */
#define CR3_CLEAR_MASK            ((uint16_t)(USART_CR3_RTSE | USART_CR3_CTSE))
/* 功能：清除CR3中硬件流控制位的掩码
 * 包含位：
 * - USART_CR3_RTSE  : RTS硬件流控制使能
 * - USART_CR3_CTSE  : CTS硬件流控制使能
 * 使用场景：修改流控制配置前清除旧设置 */

/*!< USART中断掩码 */
#define IT_MASK                   ((uint16_t)0x001F)
/* 功能：USART中断使能位组合掩码
 * 包含中断位：
 * 0x001F = 0000 0000 0001 1111
 * 对应位：
 * - bit0: PEIE  奇偶错误中断
 * - bit1: TXEIE 发送缓冲区空中断
 * - bit2: TCIE  传输完成中断
 * - bit3: RXNEIE接收缓冲区非空中断
 * - bit4: IDLEIE空闲线路中断
 * 使用场景：批量控制中断使能状态 */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup USART_Private_Functions
  * @{
  */

/** @defgroup USART_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to initialize the USART 
    in asynchronous and in synchronous modes.
      (+) For the asynchronous mode only these parameters can be configured: 
        (++) Baud Rate
        (++) Word Length 
        (++) Stop Bit
        (++) Parity: If the parity is enabled, then the MSB bit of the data written
             in the data register is transmitted but is changed by the parity bit.
             Depending on the frame length defined by the M bit (8-bits or 9-bits),
             the possible USART frame formats are as listed in the following table:
   +-------------------------------------------------------------+     
   |   M bit |  PCE bit  |            USART frame                |
   |---------------------|---------------------------------------|             
   |    0    |    0      |    | SB | 8 bit data | STB |          |
   |---------|-----------|---------------------------------------|  
   |    0    |    1      |    | SB | 7 bit data | PB | STB |     |
   |---------|-----------|---------------------------------------|  
   |    1    |    0      |    | SB | 9 bit data | STB |          |
   |---------|-----------|---------------------------------------|  
   |    1    |    1      |    | SB | 8 bit data | PB | STB |     |
   +-------------------------------------------------------------+            
        (++) Hardware flow control
        (++) Receiver/transmitter modes

    [..]
    The USART_Init() function follows the USART  asynchronous configuration 
    procedure (details for the procedure are available in reference manual (RM0090)).

     (+) For the synchronous mode in addition to the asynchronous mode parameters these 
         parameters should be also configured:
        (++) USART Clock Enabled
        (++) USART polarity
        (++) USART phase
        (++) USART LastBit
  
    [..]
    These parameters can be configured using the USART_ClockInit() function.

@endverbatim
  * @{
  */
  /**
  * @brief  USART/UART外设硬件复位函数
  * @param  USARTx: 选择USART/UART实例，x可为1-8
  * @retval 无
  * @note 功能特性：
  * - 通过RCC控制器执行硬件级复位
  * - 复位后所有寄存器恢复默认值
  * - 影响范围包括数据寄存器、状态寄存器、配置寄存器
  * - 需注意不同USART实例挂载的总线不同：
  *   APB2总线：USART1, USART6
  *   APB1总线：USART2, USART3, UART4-UART8
  */
void USART_DeInit(USART_TypeDef* USARTx)
{
  /* 参数有效性验证，确保传入有效的外设指针 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));

  /* USART1复位流程（APB2总线设备）*/
  if (USARTx == USART1)
  {
    /* 启动USART1硬件复位（置位复位标志）*/
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);
    /* 清除复位标志，完成复位操作 */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);
  }
  /* USART2复位流程（APB1总线设备）*/
  else if (USARTx == USART2)
  {
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE);
  }
  /* USART3复位流程（APB1总线设备）*/  
  else if (USARTx == USART3)
  {
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE);
  }    
  /* UART4复位流程（APB1总线设备）*/
  else if (USARTx == UART4)
  {
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, DISABLE);
  }
  /* UART5复位流程（APB1总线设备）*/  
  else if (USARTx == UART5)
  {
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART5, DISABLE);
  }  
  /* USART6复位流程（APB2总线设备）*/
  else if (USARTx == USART6)
  {
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART6, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART6, DISABLE);
  }
  /* UART7复位流程（APB1总线设备）*/  
  else if (USARTx == UART7)
  {
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART7, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART7, DISABLE);
  }     
  /* 处理非常规情况（部分型号支持UART8）*/
  else
  {
    /* UART8复位流程（APB1总线设备）*/
    if (USARTx == UART8)
    { 
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART8, ENABLE);
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART8, DISABLE);
    }
  }
}

/**
  * @brief  USART外设初始化函数
  * 
  * 本函数用于配置STM32F4系列芯片的USART/UART外设，主要完成以下功能：
  * - 配置通信参数：波特率、数据位长度、停止位、校验模式
  * - 设置工作模式：发送/接收使能
  * - 配置硬件流控制（CTS/RTS）
  * - 计算并设置波特率寄存器值
  * - 支持USART1/2/3/4/5/6/7/8等全系列外设
  * 
  * @param  USARTx: 指定要配置的USART/UART外设，取值范围：
  *         USART1, USART2, USART3, UART4, UART5, USART6, UART7, UART8
  * @param  USART_InitStruct: 初始化参数结构体指针，包含以下配置信息：
  *         - USART_BaudRate:    波特率（单位：bps）
  *         - USART_WordLength:  数据位长度（8/9位）
  *         - USART_StopBits:    停止位长度（1/0.5/2/1.5位）
  *         - USART_Parity:      奇偶校验模式（无/奇/偶）
  *         - USART_Mode:        收发模式配置（发送使能/接收使能）
  *         - USART_HardwareFlowControl: 硬件流控制配置
  * @retval None
  * 
  * @note 1. 硬件流控制仅支持USART1/2/3/6
  *       2. 波特率计算依赖RCC时钟配置，需提前配置系统时钟
  *       3. 使用assert_param进行参数合法性校验
  */
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct)
{
  uint32_t tmpreg = 0x00, apbclock = 0x00;   // 临时寄存器变量，用于位操作
  uint32_t integerdivider = 0x00;            // 波特率整数分频系数
  uint32_t fractionaldivider = 0x00;         // 波特率小数分频系数
  RCC_ClocksTypeDef RCC_ClocksStatus;        // 用于存储时钟频率信息

  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));                        // 验证USART外设选择合法性
  assert_param(IS_USART_BAUDRATE(USART_InitStruct->USART_BaudRate)); // 检查波特率是否在有效范围
  assert_param(IS_USART_WORD_LENGTH(USART_InitStruct->USART_WordLength)); // 验证数据位长度
  assert_param(IS_USART_STOPBITS(USART_InitStruct->USART_StopBits)); // 检查停止位配置
  assert_param(IS_USART_PARITY(USART_InitStruct->USART_Parity));    // 验证校验模式
  assert_param(IS_USART_MODE(USART_InitStruct->USART_Mode));         // 检查收发模式配置
  assert_param(IS_USART_HARDWARE_FLOW_CONTROL(USART_InitStruct->USART_HardwareFlowControl)); // 流控制配置检查

  /* 硬件流控制仅限于USART1/2/3/6外设 */
  if (USART_InitStruct->USART_HardwareFlowControl != USART_HardwareFlowControl_None)
  {
    assert_param(IS_USART_1236_PERIPH(USARTx));  // 验证外设是否支持硬件流控制
  }

/*---------------------------- USART CR2 寄存器配置 -----------------------*/
  tmpreg = USARTx->CR2;  // 读取当前CR2寄存器值

  /* 清除停止位设置位（位13-12）*/
  tmpreg &= (uint32_t)~((uint32_t)USART_CR2_STOP);  // 使用掩码清除STOP位

  /* 配置停止位：
   * 根据USART_StopBits参数设置停止位长度
   * 支持配置：1位，0.5位，2位，1.5位停止位 */
  tmpreg |= (uint32_t)USART_InitStruct->USART_StopBits;  // 设置新的停止位参数
  
  /* 写回CR2寄存器 */
  USARTx->CR2 = (uint16_t)tmpreg;  // 更新寄存器配置

/*---------------------------- USART CR1 寄存器配置 -----------------------*/
  tmpreg = USARTx->CR1;  // 读取当前CR1寄存器值

  /* 清除以下配置位：
   * - M位（字长控制位，位12）
   * - PCE（校验使能位，位10）
   * - PS（校验选择位，位9）
   * - TE（发送使能位，位3）
   * - RE（接收使能位，位2） */
  tmpreg &= (uint32_t)~((uint32_t)CR1_CLEAR_MASK);  // 使用预定义掩码清除相关位

  /* 配置通信参数：
   * - 设置字长（M位）
   * - 设置校验模式（PCE+PS位）
   * - 使能发送/接收（TE+RE位） */
  tmpreg |= (uint32_t)USART_InitStruct->USART_WordLength | USART_InitStruct->USART_Parity |
            USART_InitStruct->USART_Mode;  // 组合配置参数

  /* 写回CR1寄存器 */
  USARTx->CR1 = (uint16_t)tmpreg;  // 应用新的配置

/*---------------------------- USART CR3 寄存器配置 -----------------------*/  
  tmpreg = USARTx->CR3;  // 读取当前CR3寄存器值

  /* 清除硬件流控制位：
   * - CTSE（CTS使能位，位9）
   * - RTSE（RTS使能位，位8） */
  tmpreg &= (uint32_t)~((uint32_t)CR3_CLEAR_MASK);  // 使用预定义掩码清除

  /* 配置硬件流控制 */
  tmpreg |= USART_InitStruct->USART_HardwareFlowControl;  // 设置流控制参数

  /* 写回CR3寄存器 */
  USARTx->CR3 = (uint16_t)tmpreg;  // 应用流控制配置

/*---------------------------- USART BRR 波特率寄存器配置 -----------------------*/
  /* 获取当前APB总线时钟 */
  RCC_GetClocksFreq(&RCC_ClocksStatus);  // 获取系统时钟配置

  /* 确定USART挂载的APB总线：
   * USART1/6挂载在APB2，其他挂载在APB1 */
  if ((USARTx == USART1) || (USARTx == USART6))
  {
    apbclock = RCC_ClocksStatus.PCLK2_Frequency;  // 使用APB2时钟
  }
  else
  {
    apbclock = RCC_ClocksStatus.PCLK1_Frequency;  // 使用APB1时钟
  }
  
  /* 计算整数分频系数 */
  if ((USARTx->CR1 & USART_CR1_OVER8) != 0)  // 检查是否启用了8倍过采样模式
  {
    /* 8倍过采样模式下的整数分频计算公式：
     * BRR = (8 * APBclock) / (16 * BaudRate) 
     * 公式优化为：(25 * APBclock) / (2 * BaudRate) */
    integerdivider = ((25 * apbclock) / (2 * (USART_InitStruct->USART_BaudRate)));    
  }
  else  // 默认使用16倍过采样模式
  {
    /* 16倍过采样模式下的整数分频计算：
     * BRR = (16 * APBclock) / (16 * BaudRate) = APBclock / BaudRate
     * 公式优化为：(25 * APBclock) / (4 * BaudRate) */
    integerdivider = ((25 * apbclock) / (4 * (USART_InitStruct->USART_BaudRate)));    
  }
  tmpreg = (integerdivider / 100) << 4;  // 提取整数部分并左移4位（BRR[15:4]）

  /* 计算小数分频系数 */
  fractionaldivider = integerdivider - (100 * (tmpreg >> 4));  // 计算余数部分

  /* 配置小数分频寄存器 */
  if ((USARTx->CR1 & USART_CR1_OVER8) != 0)  // 8倍过采样模式
  {
    /* 小数部分计算：
     * Fraction = (8 * fractionaldivider + 50) / 100 
     * 结果取3位（DIV_Fraction[2:0]）*/
    tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
  }
  else  // 16倍过采样模式
  {
    /* 小数部分计算：
     * Fraction = (16 * fractionaldivider + 50) / 100
     * 结果取4位（DIV_Fraction[3:0]）*/
    tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
  }
  
  /* 写入波特率寄存器 */
  USARTx->BRR = (uint16_t)tmpreg;  // 配置最终波特率参数
}
/**
  * @brief  USART初始化结构体默认值加载函数
  * 
  * 本函数用于为USART初始化结构体设置默认参数值，包含以下默认配置：
  * - 波特率：9600bps
  * - 数据位长度：8位
  * - 停止位：1位
  * - 无奇偶校验
  * - 使能接收和发送模式
  * - 禁用硬件流控制
  * 
  * @param  USART_InitStruct: 待初始化的USART初始化结构体指针
  * @retval None
  */
void USART_StructInit(USART_InitTypeDef* USART_InitStruct)
{
  /* 初始化结构体成员默认值 */
  USART_InitStruct->USART_BaudRate = 9600;                         // 标准通信波特率
  USART_InitStruct->USART_WordLength = USART_WordLength_8b;        // 8位数据格式
  USART_InitStruct->USART_StopBits = USART_StopBits_1;             // 1位停止位
  USART_InitStruct->USART_Parity = USART_Parity_No ;               // 无校验位
  USART_InitStruct->USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    // 同时使能接收和发送
  USART_InitStruct->USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 禁用硬件流控制
}

/**
  * @brief  USART时钟特性配置函数
  * 
  * 本函数用于配置USART的时钟相关参数，主要功能包括：
  * - 使能/禁用时钟输出
  * - 设置时钟极性（CPOL）
  * - 设置时钟相位（CPHA）
  * - 配置最后数据位时钟脉冲
  * 
  * @param  USARTx: 指定要配置的USART外设（仅支持USART1/2/3/6）
  * @param  USART_ClockInitStruct: 时钟配置结构体指针，包含以下参数：
  *         - USART_Clock:  时钟使能控制
  *         - USART_CPOL:   时钟极性（高/低电平有效）
  *         - USART_CPHA:   时钟相位（第1/2边沿采样）
  *         - USART_LastBit:最后数据位的时钟脉冲控制
  * @note 同步模式和智能卡模式不适用于UART4/5
  * @retval None
  */
void USART_ClockInit(USART_TypeDef* USARTx, USART_ClockInitTypeDef* USART_ClockInitStruct)
{
  uint32_t tmpreg = 0x00;  // 临时寄存器变量，用于位操作
  /* 参数有效性检查 */
  assert_param(IS_USART_1236_PERIPH(USARTx));              // 验证外设是否支持时钟配置
  assert_param(IS_USART_CLOCK(USART_ClockInitStruct->USART_Clock));  // 检查时钟使能参数
  assert_param(IS_USART_CPOL(USART_ClockInitStruct->USART_CPOL));    // 验证时钟极性
  assert_param(IS_USART_CPHA(USART_ClockInitStruct->USART_CPHA));    // 验证时钟相位
  assert_param(IS_USART_LASTBIT(USART_ClockInitStruct->USART_LastBit)); // 检查最后位时钟配置
  
/*---------------------------- USART CR2寄存器时钟配置 -----------------------*/
  tmpreg = USARTx->CR2;  // 读取当前CR2寄存器值
  /* 清除时钟相关配置位：
   * - CLKEN（时钟使能位，位11）
   * - CPOL（时钟极性位，位10）
   * - CPHA（时钟相位位，位9）
   * - LBCL（最后位时钟脉冲位，位8） */
  tmpreg &= (uint32_t)~((uint32_t)CR2_CLOCK_CLEAR_MASK);  // 使用预定义掩码清除

  /* 组合配置参数：
   * - 设置CLKEN位控制时钟输出
   * - 配置CPOL位定义时钟极性
   * - 配置CPHA位定义采样边沿
   * - 设置LBCL位控制最后数据位的时钟行为 */
  tmpreg |= (uint32_t)USART_ClockInitStruct->USART_Clock | USART_ClockInitStruct->USART_CPOL | 
               USART_ClockInitStruct->USART_CPHA | USART_ClockInitStruct->USART_LastBit;

  /* 写回CR2寄存器 */
  USARTx->CR2 = (uint16_t)tmpreg;  // 应用新的时钟配置
}

/**
  * @brief  USART时钟配置结构体默认值加载函数
  * 
  * 本函数用于为USART时钟配置结构体设置默认参数值，包含以下默认配置：
  * - 禁用时钟输出
  * - 时钟极性低电平
  * - 第1个边沿采样
  * - 禁用最后位时钟脉冲
  * 
  * @param  USART_ClockInitStruct: 待初始化的时钟配置结构体指针
  * @retval None
  */
void USART_ClockStructInit(USART_ClockInitTypeDef* USART_ClockInitStruct)
{
  /* 初始化时钟配置结构体成员默认值 */
  USART_ClockInitStruct->USART_Clock = USART_Clock_Disable;  // 默认关闭时钟输出
  USART_ClockInitStruct->USART_CPOL = USART_CPOL_Low;        // 空闲时钟低电平
  USART_ClockInitStruct->USART_CPHA = USART_CPHA_1Edge;      // 第1个边沿采样数据
  USART_ClockInitStruct->USART_LastBit = USART_LastBit_Disable; // 最后位不输出时钟脉冲
}

/**
  * @brief  USART外设使能控制函数
  * 
  * 本函数用于启用或禁用指定的USART外设，主要功能：
  * - 通过设置CR1寄存器的UE位控制外设开关
  * - 启用后外设开始工作
  * - 禁用后外设进入低功耗状态
  * 
  * @param  USARTx: 指定要操作的USART/UART外设
  * @param  NewState: 外设新状态（ENABLE/DISABLE）
  * @retval None
  */
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));           // 验证外设选择合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));         // 检查状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 通过设置CR1寄存器的UE位（位13）来使能外设 */
    USARTx->CR1 |= USART_CR1_UE;  // UE位置1，启动USART
  }
  else
  {
    /* 清除UE位来禁用外设 */
    USARTx->CR1 &= (uint16_t)~((uint16_t)USART_CR1_UE);  // UE位清零，关闭USART
  }
}
/**
  * @brief  USART预分频器设置函数
  * 
  * 本函数用于配置USART的Guard Time预分频器，主要功能：
  * - 设置IrDA模式下的预分频系数
  * - 调整SIR（串行红外）编码解码的时序精度
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  USART_Prescaler: 预分频系数（0-255）
  * @note  该功能主要用于UART4/5的IrDA模式
  * @retval None
  */
void USART_SetPrescaler(USART_TypeDef* USARTx, uint8_t USART_Prescaler)
{ 
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));  // 验证外设选择合法性
  
  /* 清除GTPR寄存器中的预分频位（位7-0） */
  USARTx->GTPR &= USART_GTPR_GT;  // 保留Guard Time值（高8位），清除预分频值（低8位）
  /* 设置新的预分频系数 */
  USARTx->GTPR |= USART_Prescaler;  // 将预分频值写入GTPR低8位
}

/**
  * @brief  USART 8倍过采样模式控制函数
  * 
  * 本函数用于启用/禁用8倍过采样模式，主要影响：
  * - 波特率计算的分频系数
  * - 接收数据采样精度
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  NewState: 新状态（ENABLE:8倍采样，DISABLE:16倍采样）
  * @note 1. 必须在USART_Init之前调用本函数
  *       2. 8倍采样模式可提升高速通信时的波特率精度
  * @retval None
  */
void USART_OverSampling8Cmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));          // 验证外设选择合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));        // 检查状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 通过设置CR1寄存器的OVER8位（位15）启用8倍过采样 */
    USARTx->CR1 |= USART_CR1_OVER8;  // 1: 8倍过采样，0: 16倍过采样
  }
  else
  {
    /* 清除OVER8位恢复16倍过采样模式 */
    USARTx->CR1 &= (uint16_t)~((uint16_t)USART_CR1_OVER8);  // OVER8位清零
  }
}  

/**
  * @brief  USART单比特采样模式控制函数
  * 
  * 本函数用于启用/禁用单比特采样模式，主要功能：
  * - 在噪声环境下使用单次采样代替三次采样
  * - 提高通信速率但降低抗噪能力
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  NewState: 新状态（ENABLE:单次采样，DISABLE:三次采样）
  * @note 适用于高波特率低噪声环境
  * @retval None
  */
void USART_OneBitMethodCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));      // 验证外设选择合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));    // 检查状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 通过设置CR3寄存器的ONEBIT位（位11）启用单次采样 */
    USARTx->CR3 |= USART_CR3_ONEBIT;  // 1: 单次采样检测，0: 三次多数判决
  }
  else
  {
    /* 清除ONEBIT位恢复三次采样模式 */
    USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_ONEBIT);  // ONEBIT位清零
  }
}

/**
  * @}
  */

/** @defgroup USART_Group2 Data transfers functions
 *  @brief   Data transfers functions 
 *
@verbatim   
 ===============================================================================
                      ##### Data transfers functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the USART data 
    transfers.
    [..]
    During an USART reception, data shifts in least significant bit first through 
    the RX pin. In this mode, the USART_DR register consists of a buffer (RDR) 
    between the internal bus and the received shift register.
    [..]
    When a transmission is taking place, a write instruction to the USART_DR register 
    stores the data in the TDR register and which is copied in the shift register 
    at the end of the current transmission.
    [..]
    The read access of the USART_DR register can be done using the USART_ReceiveData()
    function and returns the RDR buffered value. Whereas a write access to the USART_DR 
    can be done using USART_SendData() function and stores the written data into 
    TDR buffer.

@endverbatim
  * @{
  */
/**
  * @brief  USART数据发送函数
  * 
  * 本函数用于通过USART外设发送单个数据帧，主要功能：
  * - 将数据写入数据寄存器(DR)
  * - 自动处理数据帧格式（包括校验位）
  * - 支持最大9位数据长度
  * 
  * @param  USARTx: 指定操作的USART/UART外设
  * @param  Data: 待发送数据（0x0000-0x01FF）
  * @retval None
  */
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));        // 验证外设选择合法性
  assert_param(IS_USART_DATA(Data));                // 检查数据是否超出9位范围
    
  /* 数据发送操作：
   * 将数据写入DR寄存器低9位（位8-0）
   * 高位数据自动忽略 */
  USARTx->DR = (Data & (uint16_t)0x01FF);  // 使用掩码确保只写入9位数据
}

/**
  * @brief  USART数据接收函数
  * 
  * 本函数用于读取USART接收到的数据帧，主要功能：
  * - 从数据寄存器(DR)读取数据
  * - 自动过滤状态标志位
  * - 返回9位有效数据
  * 
  * @param  USARTx: 指定操作的USART/UART外设
  * @retval 接收到的数据（0x0000-0x01FF）
  */
uint16_t USART_ReceiveData(USART_TypeDef* USARTx)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));  // 验证外设选择合法性
  
  /* 数据接收操作：
   * 读取DR寄存器并提取低9位有效数据
   * 自动忽略PE/FE/NF等状态标志位 */
  return (uint16_t)(USARTx->DR & (uint16_t)0x01FF);  // 使用掩码获取有效数据位
}

/**
  * @brief  USART节点地址设置函数
  * 
  * 本函数用于配置多处理器通信中的设备地址，主要功能：
  * - 设置地址识别机制
  * - 配置CR2寄存器的ADD位域
  * - 支持4位地址编码（0-15）
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  USART_Address: 节点地址（0x00-0x0F）
  * @retval None
  */
void USART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));          // 验证外设选择合法性
  assert_param(IS_USART_ADDRESS(USART_Address));      // 检查地址是否在4位范围内
    
  /* 清除CR2寄存器中的地址位（位3-0） */
  USARTx->CR2 &= (uint16_t)~((uint16_t)USART_CR2_ADD);  // 使用掩码清除原有地址
  /* 设置新的节点地址 */
  USARTx->CR2 |= USART_Address;  // 将地址写入CR2的低4位
}

/**
  * @brief  USART接收器唤醒控制函数
  * 
  * 本函数用于控制多处理器通信中的静默模式，主要功能：
  * - 启用/禁用接收器静默状态
  * - 通过CR1寄存器的RWU位控制
  * - 从机模式下用于过滤非地址帧
  * 
  * @param  USARTx: 指定要操作的USART/UART外设
  * @param  NewState: 新状态（ENABLE:静默模式，DISABLE:正常模式）
  * @retval None
  */
void USART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));      // 验证外设选择合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));    // 检查状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 通过设置CR1寄存器的RWU位（位13）进入静默模式 */
    USARTx->CR1 |= USART_CR1_RWU;  // 接收器暂停，仅监测唤醒条件
  }
  else
  {
    /* 清除RWU位恢复正常接收模式 */
    USARTx->CR1 &= (uint16_t)~((uint16_t)USART_CR1_RWU);  // 退出静默模式
  }
}
/**
  * @brief  USART唤醒方式配置函数
  * 
  * 本函数用于设置多处理器通信中的唤醒检测方式，主要功能：
  * - 选择空闲线路检测或地址标记检测唤醒模式
  * - 配置CR1寄存器的WAKE位
  * - 决定从机设备唤醒条件
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  USART_WakeUp: 唤醒检测方式，可选值：
  *         @arg USART_WakeUp_IdleLine:  空闲线路检测唤醒（持续1字节时间的空闲状态）
  *         @arg USART_WakeUp_AddressMark: 地址标记唤醒（MSB为1的数据帧）
  * @note 该配置需结合USART_ReceiverWakeUpCmd函数使用
  * @retval None
  */
void USART_WakeUpConfig(USART_TypeDef* USARTx, uint16_t USART_WakeUp)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));        // 验证外设选择合法性
  assert_param(IS_USART_WAKEUP(USART_WakeUp));      // 检查唤醒模式参数有效性
  
  /* 清除CR1寄存器中的WAKE位（位11）*/
  USARTx->CR1 &= (uint16_t)~((uint16_t)USART_CR1_WAKE);  // 使用掩码清除原有配置
  
  /* 设置新的唤醒检测方式 */
  USARTx->CR1 |= USART_WakeUp;  // 将选择的唤醒模式写入WAKE位
  /* 位11:
   * 0: 空闲线路唤醒 (USART_WakeUp_IdleLine)
   * 1: 地址标记唤醒 (USART_WakeUp_AddressMark) */
}


/**
  * @}
  */

/** @defgroup USART_Group4 LIN mode functions
 *  @brief   LIN mode functions 
 *
@verbatim   
 ===============================================================================
                        ##### LIN mode functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the USART LIN 
    Mode communication.
    [..]
    In LIN mode, 8-bit data format with 1 stop bit is required in accordance with 
    the LIN standard.
    [..]
    Only this LIN Feature is supported by the USART IP:
      (+) LIN Master Synchronous Break send capability and LIN slave break detection
          capability :  13-bit break generation and 10/11 bit break detection

    [..]
    USART LIN Master transmitter communication is possible through the following 
    procedure:
      (#) Program the Baud rate, Word length = 8bits, Stop bits = 1bit, Parity, 
        Mode transmitter or Mode receiver and hardware flow control values using 
        the USART_Init() function.
      (#) Enable the USART using the USART_Cmd() function.
      (#) Enable the LIN mode using the USART_LINCmd() function.
      (#) Send the break character using USART_SendBreak() function.
    [..]
    USART LIN Master receiver communication is possible through the following procedure:
      (#) Program the Baud rate, Word length = 8bits, Stop bits = 1bit, Parity, 
          Mode transmitter or Mode receiver and hardware flow control values using 
          the USART_Init() function.
      (#) Enable the USART using the USART_Cmd() function.
      (#) Configures the break detection length using the USART_LINBreakDetectLengthConfig()
          function.
      (#) Enable the LIN mode using the USART_LINCmd() function.

      -@- In LIN mode, the following bits must be kept cleared:
       (+@) CLKEN in the USART_CR2 register,
       (+@) STOP[1:0], SCEN, HDSEL and IREN in the USART_CR3 register.

@endverbatim
  * @{
  */
/**
  * @brief  LIN总线断点检测长度配置函数
  * 
  * 本函数用于设置LIN总线通信的中断信号检测长度，主要功能：
  * - 配置CR2寄存器的LBDL位（断点检测长度位）
  * - 选择10位或11位中断检测模式
  * - 符合LIN 1.3/2.0协议规范要求
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  USART_LINBreakDetectLength: 中断检测长度，可选值：
  *         @arg USART_LINBreakDetectLength_10b: 10位中断检测（最小10位低电平）
  *         @arg USART_LINBreakDetectLength_11b: 11位中断检测（最小11位低电平）
  * @note 用于LIN总线的主机模式配置
  * @retval None
  */
void USART_LINBreakDetectLengthConfig(USART_TypeDef* USARTx, uint16_t USART_LINBreakDetectLength)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));                          // 验证外设选择合法性
  assert_param(IS_USART_LIN_BREAK_DETECT_LENGTH(USART_LINBreakDetectLength)); // 检查长度参数有效性
  
  /* 清除CR2寄存器的LBDL位（位5）*/
  USARTx->CR2 &= (uint16_t)~((uint16_t)USART_CR2_LBDL);  // 使用掩码清除原有配置
  /* 设置新的中断检测长度 */
  USARTx->CR2 |= USART_LINBreakDetectLength;             // 配置LBDL位（0:10位，1:11位）
}

/**
  * @brief  LIN总线模式使能控制函数
  * 
  * 本函数用于启用/禁用LIN总线通信模式，主要功能：
  * - 控制CR2寄存器的LINEN位
  * - 使能LIN协议硬件解析
  * - 自动处理帧头中断检测
  * 
  * @param  USARTx: 指定要操作的USART/UART外设
  * @param  NewState: 新状态（ENABLE:启用LIN模式，DISABLE:禁用LIN模式）
  * @note 启用后需要配置相关的中断检测长度和波特率
  * @retval None
  */
void USART_LINCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));        // 验证外设选择合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));      // 检查状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 通过设置CR2寄存器的LINEN位（位14）启用LIN模式 */
    USARTx->CR2 |= USART_CR2_LINEN;  // 硬件自动识别LIN帧头结构
  }
  else
  {
    /* 清除LINEN位禁用LIN模式 */
    USARTx->CR2 &= (uint16_t)~((uint16_t)USART_CR2_LINEN);  // 恢复普通UART模式
  }
}

/**
  * @brief  LIN总线中断字符发送函数
  * 
  * 本函数用于生成LIN总线协议要求的中断信号，主要功能：
  * - 设置CR1寄存器的SBK位（发送中断位）
  * - 发送13位低电平中断信号
  * - 自动插入正常停止位（高电平）
  * 
  * @param  USARTx: 指定要操作的USART/UART外设
  * @retval None
  * @note 1. 应在LIN模式下使用
  *       2. 发送完成后硬件自动清除SBK位
  */
void USART_SendBreak(USART_TypeDef* USARTx)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));  // 验证外设选择合法性
  
  /* 发送中断字符：
   * 设置CR1的SBK位（位12）触发中断发送
   * 硬件自动发送13位低电平+1位高电平停止位 */
  USARTx->CR1 |= USART_CR1_SBK;  // SBK位置1后自动清零，仅需单次设置
}

/**
  * @}
  */

/** @defgroup USART_Group5 Halfduplex mode function
 *  @brief   Half-duplex mode function 
 *
@verbatim   
 ===============================================================================
                    ##### Half-duplex mode function #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the USART 
    Half-duplex communication.
    [..]
    The USART can be configured to follow a single-wire half-duplex protocol where 
    the TX and RX lines are internally connected.
    [..]
    USART Half duplex communication is possible through the following procedure:
      (#) Program the Baud rate, Word length, Stop bits, Parity, Mode transmitter 
          or Mode receiver and hardware flow control values using the USART_Init()
          function.
      (#) Configures the USART address using the USART_SetAddress() function.
      (#) Enable the USART using the USART_Cmd() function.
      (#) Enable the half duplex mode using USART_HalfDuplexCmd() function.


    -@- The RX pin is no longer used
    -@- In Half-duplex mode the following bits must be kept cleared:
      (+@) LINEN and CLKEN bits in the USART_CR2 register.
      (+@) SCEN and IREN bits in the USART_CR3 register.

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the USART's Half Duplex communication.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  NewState: new state of the USART Communication.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  USART半双工模式控制函数
  * 
  * 本函数用于配置USART的单线半双工通信模式，主要功能：
  * - 通过CR3寄存器的HDSEL位控制通信方向
  * - 启用后TX和RX引脚合并使用
  * - 需要配合发送/接收使能位使用
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  NewState: 新状态（ENABLE:半双工模式，DISABLE:全双工模式）
  * @note 1. 模式切换后建议重新初始化USART参数
  *       2. 半双工模式下需手动切换收发状态
  * @retval None
  */
void USART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));      // 验证外设选择合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));    // 检查状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 通过设置CR3寄存器的HDSEL位（位3）启用半双工模式 */
    USARTx->CR3 |= USART_CR3_HDSEL;  // 合并使用TX/RX引脚（单线通信）
    /* 启用后：
     * - 发送时自动禁用接收
     * - 接收前需关闭发送器 */
  }
  else
  {
    /* 清除HDSEL位恢复全双工模式 */
    USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_HDSEL);  // 恢复独立TX/RX引脚
  }
}


/**
  * @}
  */


/** @defgroup USART_Group6 Smartcard mode functions
 *  @brief   Smartcard mode functions 
 *
@verbatim   
 ===============================================================================
                              ##### Smartcard mode functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the USART 
    Smartcard communication.
    [..]
    The Smartcard interface is designed to support asynchronous protocol Smartcards as
    defined in the ISO 7816-3 standard.
    [..]
    The USART can provide a clock to the smartcard through the SCLK output.
    In smartcard mode, SCLK is not associated to the communication but is simply derived 
    from the internal peripheral input clock through a 5-bit prescaler.
    [..]
    Smartcard communication is possible through the following procedure:
      (#) Configures the Smartcard Prescaler using the USART_SetPrescaler() function.
      (#) Configures the Smartcard Guard Time using the USART_SetGuardTime() function.
      (#) Program the USART clock using the USART_ClockInit() function as following:
        (++) USART Clock enabled
        (++) USART CPOL Low
        (++) USART CPHA on first edge
        (++) USART Last Bit Clock Enabled
      (#) Program the Smartcard interface using the USART_Init() function as following:
        (++) Word Length = 9 Bits
        (++) 1.5 Stop Bit
        (++) Even parity
        (++) BaudRate = 12096 baud
        (++) Hardware flow control disabled (RTS and CTS signals)
        (++) Tx and Rx enabled
      (#) POptionally you can enable the parity error interrupt using the USART_ITConfig()
          function
      (#) PEnable the USART using the USART_Cmd() function.
      (#) PEnable the Smartcard NACK using the USART_SmartCardNACKCmd() function.
      (#) PEnable the Smartcard interface using the USART_SmartCardCmd() function.

    Please refer to the ISO 7816-3 specification for more details.

      -@- It is also possible to choose 0.5 stop bit for receiving but it is recommended 
          to use 1.5 stop bits for both transmitting and receiving to avoid switching 
          between the two configurations.
      -@- In smartcard mode, the following bits must be kept cleared:
        (+@) LINEN bit in the USART_CR2 register.
        (+@) HDSEL and IREN bits in the USART_CR3 register.
      -@- Smartcard mode is available on USART peripherals only (not available on UART4 
          and UART5 peripherals).

@endverbatim
  * @{
  */
/**
  * @brief  智能卡保护时间设置函数
  * 
  * 本函数用于配置智能卡模式的保护时间间隔，主要功能：
  * - 设置GTPR寄存器的保护时间值（高8位）
  * - 调整时钟停止位持续时间
  * - 满足ISO/IEC 7816-3智能卡协议要求
  * 
  * @param  USARTx: 指定要配置的USART外设（仅支持USART1/2/3/6）
  * @param  USART_GuardTime: 保护时间值（0x00-0xFF）
  *         对应计算公式：GuardTime = (FUART clock周期) * (USART_GuardTime + 1)
  * @note 需配合智能卡模式使用
  * @retval None
  */
void USART_SetGuardTime(USART_TypeDef* USARTx, uint8_t USART_GuardTime)
{    
  /* 参数有效性检查 */
  assert_param(IS_USART_1236_PERIPH(USARTx));  // 验证外设支持智能卡模式
  
  /* 清除GTPR寄存器的高8位（保护时间位） */
  USARTx->GTPR &= USART_GTPR_PSC;  // 保留低8位预分频值（位7-0）
  /* 设置新的保护时间值到高8位（位15-8）*/
  USARTx->GTPR |= (uint16_t)((uint16_t)USART_GuardTime << 0x08);  // 左移8位写入保护时间
}

/**
  * @brief  智能卡模式使能控制函数
  * 
  * 本函数用于启用/禁用智能卡通信模式，主要功能：
  * - 控制CR3寄存器的SCEN位
  * - 启用后自动生成智能卡协议要求的时钟控制
  * - 支持T=0/T=1通信协议
  * 
  * @param  USARTx: 指定要配置的USART外设（仅支持USART1/2/3/6）
  * @param  NewState: 新状态（ENABLE:智能卡模式，DISABLE:普通模式）
  * @note 启用前需配置保护时间和正确时钟分频
  * @retval None
  */
void USART_SmartCardCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_1236_PERIPH(USARTx));   // 验证外设支持智能卡
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 检查状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 通过设置CR3寄存器的SCEN位（位5）启用智能卡模式 */
    USARTx->CR3 |= USART_CR3_SCEN;  // 硬件自动管理时钟信号
  }
  else
  {
    /* 清除SCEN位禁用智能卡模式 */
    USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_SCEN);  // 恢复正常UART操作
  }
}

/**
  * @brief  智能卡NACK响应使能函数
  * 
  * 本函数用于控制错误数据帧的响应方式，主要功能：
  * - 配置CR3寄存器的NACK位
  * - 启用后在奇偶校验错误时自动发送NACK信号
  * - 符合智能卡错误重传机制要求
  * 
  * @param  USARTx: 指定要配置的USART外设（仅支持USART1/2/3/6）
  * @param  NewState: 新状态（ENABLE:启用NACK响应，DISABLE:禁用NACK响应）
  * @note 仅在智能卡模式有效
  * @retval None
  */
void USART_SmartCardNACKCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_1236_PERIPH(USARTx));   // 验证外设支持智能卡
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 检查状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 通过设置CR3寄存器的NACK位（位6）启用自动NACK */
    USARTx->CR3 |= USART_CR3_NACK;  // 检测到错误时自动发送非确认信号
  }
  else
  {
    /* 清除NACK位禁用自动响应 */
    USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_NACK);  // 出现错误时不自动响应
  }
}


/**
  * @}
  */

/** @defgroup USART_Group7 IrDA mode functions
 *  @brief   IrDA mode functions 
 *
@verbatim   
 ===============================================================================
                        ##### IrDA mode functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the USART 
    IrDA communication.
    [..]
    IrDA is a half duplex communication protocol. If the Transmitter is busy, any data
    on the IrDA receive line will be ignored by the IrDA decoder and if the Receiver 
    is busy, data on the TX from the USART to IrDA will not be encoded by IrDA.
    While receiving data, transmission should be avoided as the data to be transmitted
    could be corrupted.
    [..]
    IrDA communication is possible through the following procedure:
      (#) Program the Baud rate, Word length = 8 bits, Stop bits, Parity, Transmitter/Receiver 
          modes and hardware flow control values using the USART_Init() function.
      (#) Enable the USART using the USART_Cmd() function.
      (#) Configures the IrDA pulse width by configuring the prescaler using  
          the USART_SetPrescaler() function.
      (#) Configures the IrDA  USART_IrDAMode_LowPower or USART_IrDAMode_Normal mode
          using the USART_IrDAConfig() function.
      (#) Enable the IrDA using the USART_IrDACmd() function.

      -@- A pulse of width less than two and greater than one PSC period(s) may or may
          not be rejected.
      -@- The receiver set up time should be managed by software. The IrDA physical layer
          specification specifies a minimum of 10 ms delay between transmission and 
          reception (IrDA is a half duplex protocol).
      -@- In IrDA mode, the following bits must be kept cleared:
        (+@) LINEN, STOP and CLKEN bits in the USART_CR2 register.
        (+@) SCEN and HDSEL bits in the USART_CR3 register.

@endverbatim
  * @{
  */
/**
  * @brief  IrDA红外通信模式配置函数
  * 
  * 本函数用于设置IrDA红外通信的工作模式，主要功能：
  * - 选择低功耗模式或标准模式
  * - 配置CR3寄存器的IRLP位（红外低功耗模式位）
  * - 调整红外脉冲宽度
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  USART_IrDAMode: 红外工作模式，可选值：
  *         @arg USART_IrDAMode_LowPower: 低功耗模式（3/16脉冲宽度）
  *         @arg USART_IrDAMode_Normal:   标准模式（正常脉冲宽度）
  * @note 需配合USART_IrDACmd函数启用红外模式
  * @retval None
  */
void USART_IrDAConfig(USART_TypeDef* USARTx, uint16_t USART_IrDAMode)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));            // 验证外设选择合法性
  assert_param(IS_USART_IRDA_MODE(USART_IrDAMode));      // 检查红外模式参数有效性
    
  /* 清除CR3寄存器的IRLP位（位1）*/
  USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_IRLP);  // 使用掩码清除原有模式配置
  /* 设置新的红外工作模式 */
  USARTx->CR3 |= USART_IrDAMode;                         // IRLP位：0-标准，1-低功耗
}

/**
  * @brief  IrDA红外通信模式使能控制函数
  * 
  * 本函数用于启用/禁用IrDA红外通信功能，主要功能：
  * - 控制CR3寄存器的IREN位（红外使能位）
  * - 启用后TX引脚输出调制后的红外信号
  * - 接收端需配置红外解码电路
  * 
  * @param  USARTx: 指定要操作的USART/UART外设
  * @param  NewState: 新状态（ENABLE:红外模式，DISABLE:普通串口模式）
  * @note 1. 启用前需配置正确的波特率和IrDA模式
  *       2. 需外接红外收发器硬件
  * @retval None
  */
void USART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));          // 验证外设选择合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));        // 检查状态参数有效性
    
  if (NewState != DISABLE)
  {
    /* 通过设置CR3寄存器的IREN位（位0）启用红外模式 */
    USARTx->CR3 |= USART_CR3_IREN;  // 使能红外编解码电路
  }
  else
  {
    /* 清除IREN位禁用红外功能 */
    USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_IREN);  // 恢复普通电平信号
  }
}

/**
  * @}
  */

/** @defgroup USART_Group8 DMA transfers management functions
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
  * @brief  Enables or disables the USART's DMA interface.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  USART_DMAReq: specifies the DMA request.
  *          This parameter can be any combination of the following values:
  *            @arg USART_DMAReq_Tx: USART DMA transmit request
  *            @arg USART_DMAReq_Rx: USART DMA receive request
  * @param  NewState: new state of the DMA Request sources.
  *          This parameter can be: ENABLE or DISABLE.   
  * @retval None
  */
/**
  * @brief  USART DMA传输请求控制函数
  * 
  * 本函数用于管理USART的DMA数据传输请求，主要功能：
  * - 启用/禁用发送和接收的DMA请求
  * - 控制CR3寄存器的DMAT/DMAR位
  * - 支持同时配置发送和接收DMA通道
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  USART_DMAReq: DMA请求类型，可选值或其组合：
  *         @arg USART_DMAReq_Tx: 发送DMA请求（DMAT位）
  *         @arg USART_DMAReq_Rx: 接收DMA请求（DMAR位）
  * @param  NewState: 新状态（ENABLE:启用DMA请求，DISABLE:禁用DMA请求）
  * @note 1. 需先配置DMA控制器相关通道
  *       2. 可同时操作发送和接收请求（如：USART_DMAReq_Tx|USART_DMAReq_Rx）
  * @retval None
  */
void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));        // 验证外设选择合法性
  assert_param(IS_USART_DMAREQ(USART_DMAReq));      // 检查DMA请求类型有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));      // 检查状态参数合法性

  if (NewState != DISABLE)
  {
    /* 通过设置CR3寄存器的DMAT（位7）和/或DMAR（位6）位启用DMA请求 */
    USARTx->CR3 |= USART_DMAReq;  // 位掩码操作可同时启用发送和接收请求
    /* 启用后：
     * - 发送时DR寄存器空触发DMA传输（DMAT）
     * - 接收时DR寄存器满触发DMA传输（DMAR） */
  }
  else
  {
    /* 清除DMAT/DMAR位禁用DMA请求 */
    USARTx->CR3 &= (uint16_t)~USART_DMAReq;  // 按位取反后清除指定请求位
  }
}


/**
  * @}
  */
  
/** @defgroup USART_Group9 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions 
 *
@verbatim   
 ===============================================================================
            ##### Interrupts and flags management functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to configure the USART 
    Interrupts sources, DMA channels requests and check or clear the flags or 
    pending bits status.
    The user should identify which mode will be used in his application to manage 
    the communication: Polling mode, Interrupt mode or DMA mode. 
    
    *** Polling Mode ***
    ====================
    [..]
    In Polling Mode, the SPI communication can be managed by 10 flags:
      (#) USART_FLAG_TXE : to indicate the status of the transmit buffer register
      (#) USART_FLAG_RXNE : to indicate the status of the receive buffer register
      (#) USART_FLAG_TC : to indicate the status of the transmit operation
      (#) USART_FLAG_IDLE : to indicate the status of the Idle Line             
      (#) USART_FLAG_CTS : to indicate the status of the nCTS input
      (#) USART_FLAG_LBD : to indicate the status of the LIN break detection
      (#) USART_FLAG_NE : to indicate if a noise error occur
      (#) USART_FLAG_FE : to indicate if a frame error occur
      (#) USART_FLAG_PE : to indicate if a parity error occur
      (#) USART_FLAG_ORE : to indicate if an Overrun error occur
    [..]
    In this Mode it is advised to use the following functions:
      (+) FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);
      (+) void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);

    *** Interrupt Mode ***
    ======================
    [..]
    In Interrupt Mode, the USART communication can be managed by 8 interrupt sources
    and 10 pending bits: 

      (#) Pending Bits:

        (##) USART_IT_TXE : to indicate the status of the transmit buffer register
        (##) USART_IT_RXNE : to indicate the status of the receive buffer register
        (##) USART_IT_TC : to indicate the status of the transmit operation
        (##) USART_IT_IDLE : to indicate the status of the Idle Line             
        (##) USART_IT_CTS : to indicate the status of the nCTS input
        (##) USART_IT_LBD : to indicate the status of the LIN break detection
        (##) USART_IT_NE : to indicate if a noise error occur
        (##) USART_IT_FE : to indicate if a frame error occur
        (##) USART_IT_PE : to indicate if a parity error occur
        (##) USART_IT_ORE : to indicate if an Overrun error occur

      (#) Interrupt Source:

        (##) USART_IT_TXE : specifies the interrupt source for the Tx buffer empty 
                            interrupt. 
        (##) USART_IT_RXNE : specifies the interrupt source for the Rx buffer not 
                             empty interrupt.
        (##) USART_IT_TC : specifies the interrupt source for the Transmit complete 
                           interrupt. 
        (##) USART_IT_IDLE : specifies the interrupt source for the Idle Line interrupt.             
        (##) USART_IT_CTS : specifies the interrupt source for the CTS interrupt. 
        (##) USART_IT_LBD : specifies the interrupt source for the LIN break detection
                            interrupt. 
        (##) USART_IT_PE : specifies the interrupt source for the parity error interrupt. 
        (##) USART_IT_ERR :  specifies the interrupt source for the errors interrupt.

      -@@- Some parameters are coded in order to use them as interrupt source 
          or as pending bits.
    [..]
    In this Mode it is advised to use the following functions:
      (+) void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
      (+) ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
      (+) void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);

    *** DMA Mode ***
    ================
    [..]
    In DMA Mode, the USART communication can be managed by 2 DMA Channel requests:
      (#) USART_DMAReq_Tx: specifies the Tx buffer DMA transfer request
      (#) USART_DMAReq_Rx: specifies the Rx buffer DMA transfer request
    [..]
    In this Mode it is advised to use the following function:
      (+) void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState);

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified USART interrupts.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  USART_IT: specifies the USART interrupt sources to be enabled or disabled.
  *          This parameter can be one of the following values:
  *            @arg USART_IT_CTS:  CTS change interrupt
  *            @arg USART_IT_LBD:  LIN Break detection interrupt
  *            @arg USART_IT_TXE:  Transmit Data Register empty interrupt
  *            @arg USART_IT_TC:   Transmission complete interrupt
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_PE:   Parity Error interrupt
  *            @arg USART_IT_ERR:  Error interrupt(Frame error, noise error, overrun error)
  * @param  NewState: new state of the specified USARTx interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  USART中断源配置函数
  * 
  * 本函数用于动态管理USART的各种中断源，主要功能：
  * - 支持34种中断类型的使能控制
  * - 自动定位CR1/CR2/CR3寄存器中的中断使能位
  * - 特殊处理CTS中断的硬件限制
  * 
  * @param  USARTx: 指定要配置的USART/UART外设
  * @param  USART_IT: 中断类型，取值示例：
  *         @arg USART_IT_TXE:  发送寄存器空中断
  *         @arg USART_IT_RXNE: 接收数据非空中断
  *         @arg USART_IT_CTS:  CTS状态变化中断（仅USART1/2/3/6）
  *         （完整列表参见IS_USART_CONFIG_IT宏定义）
  * @param  NewState: 新状态（ENABLE/DISABLE）
  * @note 1. CTS中断不可用于UART4/5
  *       2. 中断类型编码规则：高3位表示寄存器(CR1=1,CR2=2,CR3=3)，低5位表示位位置
  * @retval None
  */
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState)
{
  uint32_t usartreg = 0x00, itpos = 0x00, itmask = 0x00;
  uint32_t usartxbase = 0x00;
  
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));            // 验证外设合法性
  assert_param(IS_USART_CONFIG_IT(USART_IT));           // 检查中断类型有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));          // 检查状态参数合法性

  /* 特殊处理CTS中断的硬件限制 */
  if (USART_IT == USART_IT_CTS) 
  {
    assert_param(IS_USART_1236_PERIPH(USARTx));        // 验证是否支持CTS中断的外设
  } 
    
  usartxbase = (uint32_t)USARTx;  // 将外设基地址转换为32位整型

  /* 解析中断类型寄存器索引（高3位）*/
  usartreg = (((uint8_t)USART_IT) >> 0x05);  // 右移5位获取寄存器标识（0x01:CR1, 0x02:CR2, 0x03:CR3）

  /* 计算中断位位置及掩码 */
  itpos = USART_IT & IT_MASK;                // 与0x1F得到低5位（位位置）
  itmask = (((uint32_t)0x01) << itpos);      // 生成对应位掩码（例如位5：0x00000020）
    
  /* 计算目标寄存器偏移地址 */
  if (usartreg == 0x01)        /* 中断位在CR1寄存器 */
  {
    usartxbase += 0x0C;        // CR1寄存器偏移地址：USART_BASE + 0x0C
  }
  else if (usartreg == 0x02)   /* 中断位在CR2寄存器 */
  {
    usartxbase += 0x10;        // CR2寄存器偏移地址：USART_BASE + 0x10
  }
  else                         /* 中断位在CR3寄存器 */
  {
    usartxbase += 0x14;        // CR3寄存器偏移地址：USART_BASE + 0x14
  }

  /* 执行位操作 */
  if (NewState != DISABLE)
  {
    /* 通过指针操作设置对应中断使能位 */
    *(__IO uint32_t*)usartxbase  |= itmask;  // 等效于USARTx->CRn |= itmask
  }
  else
  {
    /* 清除中断使能位 */
    *(__IO uint32_t*)usartxbase &= ~itmask;  // 等效于USARTx->CRn &= ~itmask
  }
}


/**
  * @brief  Checks whether the specified USART flag is set or not.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  USART_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg USART_FLAG_CTS:  CTS Change flag (not available for UART4 and UART5)
  *            @arg USART_FLAG_LBD:  LIN Break detection flag
  *            @arg USART_FLAG_TXE:  Transmit data register empty flag
  *            @arg USART_FLAG_TC:   Transmission Complete flag
  *            @arg USART_FLAG_RXNE: Receive data register not empty flag
  *            @arg USART_FLAG_IDLE: Idle Line detection flag
  *            @arg USART_FLAG_ORE:  OverRun Error flag
  *            @arg USART_FLAG_NE:   Noise Error flag
  *            @arg USART_FLAG_FE:   Framing Error flag
  *            @arg USART_FLAG_PE:   Parity Error flag
  * @retval The new state of USART_FLAG (SET or RESET).
  */
/**
  * @brief  USART状态标志获取函数
  * 
  * 本函数用于读取USART状态寄存器的各种标志位，主要功能：
  * - 检测传输状态（发送完成、接收就绪等）
  * - 识别通信错误（溢出错误、帧错误等）
  * - 特殊处理CTS标志的硬件限制
  * 
  * @param  USARTx: 指定要查询的USART/UART外设
  * @param  USART_FLAG: 要检测的状态标志，可选值：
  *         @arg USART_FLAG_TXE:  发送寄存器空标志
  *         @arg USART_FLAG_RXNE: 接收数据非空标志  
  *         @arg USART_FLAG_CTS:  CTS状态标志（仅USART1/2/3/6）
  *         （完整标志列表参见IS_USART_FLAG宏定义）
  * @retval 标志位状态：SET（置位）或 RESET（复位）
  * 
  * @note 1. CTS标志不可用于UART4/5
  *       2. 部分标志需手动清除（如ORE、NE、FE等）
  */
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG)
{
  FlagStatus bitstatus = RESET;  // 默认返回复位状态
  
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));      // 验证外设合法性
  assert_param(IS_USART_FLAG(USART_FLAG));        // 检查标志参数有效性

  /* 特殊处理CTS标志的硬件限制 */
  if (USART_FLAG == USART_FLAG_CTS)
  {
    assert_param(IS_USART_1236_PERIPH(USARTx));  // 验证CTS标志的可用性
  } 
    
  /* 读取状态寄存器并检测标志位 */
  if ((USARTx->SR & USART_FLAG) != (uint16_t)RESET)  // 位与操作检测标志
  {
    bitstatus = SET;  // 标志位置位
  }
  else
  {
    bitstatus = RESET; // 标志位复位
  }
  
  return bitstatus;  // 返回检测结果
}

/**
  * @brief  Clears the USARTx's pending flags.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  USART_FLAG: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg USART_FLAG_CTS:  CTS Change flag (not available for UART4 and UART5).
  *            @arg USART_FLAG_LBD:  LIN Break detection flag.
  *            @arg USART_FLAG_TC:   Transmission Complete flag.
  *            @arg USART_FLAG_RXNE: Receive data register not empty flag.
  *   
  * @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (OverRun 
  *          error) and IDLE (Idle line detected) flags are cleared by software 
  *          sequence: a read operation to USART_SR register (USART_GetFlagStatus()) 
  *          followed by a read operation to USART_DR register (USART_ReceiveData()).
  * @note   RXNE flag can be also cleared by a read to the USART_DR register 
  *          (USART_ReceiveData()).
  * @note   TC flag can be also cleared by software sequence: a read operation to 
  *          USART_SR register (USART_GetFlagStatus()) followed by a write operation
  *          to USART_DR register (USART_SendData()).
  * @note   TXE flag is cleared only by a write to the USART_DR register 
  *          (USART_SendData()).
  *   
  * @retval None
  */
/**
  * @brief  USART状态标志清除函数
  * 
  * 本函数用于清除指定的状态寄存器标志位，主要功能：
  * - 清除通信错误标志（溢出错误、帧错误等）
  * - 复位状态检测标志（如CTS状态变化）
  * - 特殊处理CTS标志的硬件限制
  * 
  * @param  USARTx: 指定要操作的USART/UART外设
  * @param  USART_FLAG: 需清除的标志位，可选值：
  *         @arg USART_FLAG_CTS:  CTS状态标志（仅USART1/2/3/6）
  *         @arg USART_FLAG_LBD:  LIN中断检测标志
  *         @arg USART_FLAG_TC:   传输完成标志
  *         （完整列表参见IS_USART_CLEAR_FLAG宏定义）
  * @note 1. TXE标志不能通过此函数清除
  *       2. 清除方式为向状态寄存器写入标志位取反值
  * @retval None
  */
void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG)
{
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));          // 验证外设合法性
  assert_param(IS_USART_CLEAR_FLAG(USART_FLAG));      // 检查可清除标志有效性

  /* 特殊处理CTS标志的硬件限制 */
  if ((USART_FLAG & USART_FLAG_CTS) == USART_FLAG_CTS)
  {
    assert_param(IS_USART_1236_PERIPH(USARTx));      // 验证CTS标志的可用性
  } 
       
  /* 清除标志操作：
   * 将标志位取反后写入SR寄存器（写1清标志位）*/
  USARTx->SR = (uint16_t)~USART_FLAG;  // 例如：清除ORE标志时写入0xFFFFF7FF
}

/**
  * @brief  Checks whether the specified USART interrupt has occurred or not.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  USART_IT: specifies the USART interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg USART_IT_CTS:  CTS change interrupt (not available for UART4 and UART5)
  *            @arg USART_IT_LBD:  LIN Break detection interrupt
  *            @arg USART_IT_TXE:  Transmit Data Register empty interrupt
  *            @arg USART_IT_TC:   Transmission complete interrupt
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_ORE_RX : OverRun Error interrupt if the RXNEIE bit is set
  *            @arg USART_IT_ORE_ER : OverRun Error interrupt if the EIE bit is set  
  *            @arg USART_IT_NE:   Noise Error interrupt
  *            @arg USART_IT_FE:   Framing Error interrupt
  *            @arg USART_IT_PE:   Parity Error interrupt
  * @retval The new state of USART_IT (SET or RESET).
  */
/**
  * @brief  USART中断状态查询函数
  * 
  * 本函数用于检测指定中断源是否触发，主要功能：
  * - 联合检查中断使能位和状态标志位
  * - 支持所有可屏蔽中断源的状态查询
  * - 符合双重条件检测机制（使能位+状态位）
  * 
  * @param  USARTx: 指定要查询的USART/UART外设
  * @param  USART_IT: 中断类型，示例：
  *         @arg USART_IT_RXNE: 接收中断（需CR1[RXNEIE] & SR[RXNE]）
  *         @arg USART_IT_CTS:  CTS中断（仅USART1/2/3/6）
  *         （完整列表参见IS_USART_GET_IT宏定义）
  * @retval 中断状态：SET（中断挂起）或 RESET（未触发）
  * 
  * @note 1. CTS中断不可用于UART4/5
  *       2. 需同时满足中断使能和状态标志置位才返回SET
  */
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT)
{
  uint32_t bitpos = 0x00, itmask = 0x00, usartreg = 0x00;
  ITStatus bitstatus = RESET;  // 默认返回未触发状态
  
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));        // 验证外设合法性
  assert_param(IS_USART_GET_IT(USART_IT));          // 检查中断类型有效性

  /* 特殊处理CTS中断的硬件限制 */
  if (USART_IT == USART_IT_CTS)
  {
    assert_param(IS_USART_1236_PERIPH(USARTx));    // 验证CTS中断的可用性
  } 

  /*---------------- 第一阶段：检查中断使能位 ----------------*/
  /* 解析中断使能寄存器信息 */
  usartreg = (((uint8_t)USART_IT) >> 0x05);        // 高3位：0x01=CR1,0x02=CR2,0x03=CR3
  itmask = USART_IT & IT_MASK;                     // 获取中断使能位位置（低5位）
  itmask = (uint32_t)0x01 << itmask;               // 生成中断使能位掩码

  /* 检查对应CR寄存器中的使能位 */
  if (usartreg == 0x01)         /* CR1寄存器 */
  {
    itmask &= USARTx->CR1;      // 例如：USART_IT_TXE -> CR1[TXEIE]
  }
  else if (usartreg == 0x02)    /* CR2寄存器 */
  {
    itmask &= USARTx->CR2;      // 例如：USART_IT_LBD -> CR2[LBDIE]
  }
  else                          /* CR3寄存器 */
  {
    itmask &= USARTx->CR3;      // 例如：USART_IT_CTS -> CR3[CTSIE]
  }

  /*---------------- 第二阶段：检查状态标志位 ----------------*/
  bitpos = USART_IT >> 0x08;                      // 获取状态标志位位置（参数高8位中的低5位）
  bitpos = (uint32_t)0x01 << bitpos;              // 生成状态标志位掩码
  bitpos &= USARTx->SR;                           // 从SR寄存器读取标志位

  /*---------------- 综合判断双重条件 ----------------*/
  if ((itmask != (uint16_t)RESET) && (bitpos != (uint16_t)RESET))
  {
    bitstatus = SET;  // 仅当使能位和状态位同时置位时返回触发状态
  }
  
  return bitstatus;
}


/**
  * @brief  Clears the USARTx's interrupt pending bits.
  * @param  USARTx: where x can be 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  USART_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be one of the following values:
  *            @arg USART_IT_CTS:  CTS change interrupt (not available for UART4 and UART5)
  *            @arg USART_IT_LBD:  LIN Break detection interrupt
  *            @arg USART_IT_TC:   Transmission complete interrupt. 
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt.
  *
  * @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (OverRun 
  *          error) and IDLE (Idle line detected) pending bits are cleared by 
  *          software sequence: a read operation to USART_SR register 
  *          (USART_GetITStatus()) followed by a read operation to USART_DR register 
  *          (USART_ReceiveData()).
  * @note   RXNE pending bit can be also cleared by a read to the USART_DR register 
  *          (USART_ReceiveData()).
  * @note   TC pending bit can be also cleared by software sequence: a read 
  *          operation to USART_SR register (USART_GetITStatus()) followed by a write 
  *          operation to USART_DR register (USART_SendData()).
  * @note   TXE pending bit is cleared only by a write to the USART_DR register 
  *          (USART_SendData()).
  *  
  * @retval None
  */
/**
  * @brief  USART中断挂起标志清除函数
  * 
  * 本函数用于清除指定的中断状态标志，主要功能：
  * - 清除状态寄存器中的中断触发标志
  * - 防止重复处理已触发的中断
  * - 特殊处理CTS中断的硬件限制
  * 
  * @param  USARTx: 指定要操作的USART/UART外设
  * @param  USART_IT: 需清除的中断类型，示例：
  *         @arg USART_IT_TC:   传输完成中断标志
  *         @arg USART_IT_LBD:  LIN中断检测标志
  *         @arg USART_IT_CTS:  CTS中断标志（仅USART1/2/3/6）
  *         （完整列表参见IS_USART_CLEAR_IT宏定义）
  * @note 1. TXE标志不能通过此函数清除
  *       2. 实际清除的是状态寄存器中的标志位，不影响中断使能位
  * @retval None
  */
void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT)
{
  uint16_t bitpos = 0x00, itmask = 0x00;
  
  /* 参数有效性检查 */
  assert_param(IS_USART_ALL_PERIPH(USARTx));        // 验证外设合法性
  assert_param(IS_USART_CLEAR_IT(USART_IT));        // 检查可清除中断类型有效性

  /* 特殊处理CTS中断的硬件限制 */
  if (USART_IT == USART_IT_CTS)
  {
    assert_param(IS_USART_1236_PERIPH(USARTx));    // 验证CTS中断的可用性
  } 
    
  /* 从中断类型参数中提取状态标志位位置 */
  bitpos = USART_IT >> 0x08;                      // 参数高8位存储标志位位置
  itmask = ((uint16_t)0x01 << (uint16_t)bitpos);   // 生成状态标志位掩码
  
  /* 清除状态标志位操作：
   * 向SR寄存器写入取反后的掩码（写1清标志）*/
  USARTx->SR = (uint16_t)~itmask;  // 例如：清除TC标志时写入0xFFFFBFFF
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

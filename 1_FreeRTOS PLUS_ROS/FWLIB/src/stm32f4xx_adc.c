/**
  ******************************************************************************
  * @file    stm32f4xx_adc.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Analog to Digital Convertor (ADC) peripheral:
  *           + Initialization and Configuration (in addition to ADC multi mode 
  *             selection)
  *           + Analog Watchdog configuration
  *           + Temperature Sensor & Vrefint (Voltage Reference internal) & VBAT
  *             management 
  *           + Regular Channels Configuration
  *           + Regular Channels DMA Configuration
  *           + Injected channels Configuration
  *           + Interrupts and flags management
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]
    (#) Enable the ADC interface clock using 
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADCx, ENABLE); 
       
    (#) ADC pins configuration
         (++) Enable the clock for the ADC GPIOs using the following function:
             RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);   
         (++) Configure these ADC pins in analog mode using GPIO_Init();  
  
     (#) Configure the ADC Prescaler, conversion resolution and data 
         alignment using the ADC_Init() function.
     (#) Activate the ADC peripheral using ADC_Cmd() function.
  
     *** Regular channels group configuration ***
     ============================================
     [..]    
       (+) To configure the ADC regular channels group features, use 
           ADC_Init() and ADC_RegularChannelConfig() functions.
       (+) To activate the continuous mode, use the ADC_continuousModeCmd()
           function.
       (+) To configurate and activate the Discontinuous mode, use the 
           ADC_DiscModeChannelCountConfig() and ADC_DiscModeCmd() functions.
       (+) To read the ADC converted values, use the ADC_GetConversionValue()
           function.
  
     *** Multi mode ADCs Regular channels configuration ***
     ======================================================
     [..]
       (+) Refer to "Regular channels group configuration" description to
           configure the ADC1, ADC2 and ADC3 regular channels.        
       (+) Select the Multi mode ADC regular channels features (dual or 
           triple mode) using ADC_CommonInit() function and configure 
           the DMA mode using ADC_MultiModeDMARequestAfterLastTransferCmd() 
           functions.        
       (+) Read the ADCs converted values using the 
           ADC_GetMultiModeConversionValue() function.
  
     *** DMA for Regular channels group features configuration ***
     ============================================================= 
     [..]
       (+) To enable the DMA mode for regular channels group, use the 
           ADC_DMACmd() function.
       (+) To enable the generation of DMA requests continuously at the end
           of the last DMA transfer, use the ADC_DMARequestAfterLastTransferCmd() 
           function.
  
     *** Injected channels group configuration ***
     =============================================    
     [..]
       (+) To configure the ADC Injected channels group features, use 
           ADC_InjectedChannelConfig() and  ADC_InjectedSequencerLengthConfig()
           functions.
       (+) To activate the continuous mode, use the ADC_continuousModeCmd()
           function.
       (+) To activate the Injected Discontinuous mode, use the 
           ADC_InjectedDiscModeCmd() function.  
       (+) To activate the AutoInjected mode, use the ADC_AutoInjectedConvCmd() 
           function.        
       (+) To read the ADC converted values, use the ADC_GetInjectedConversionValue() 
           function.
  
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
#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup ADC 
  * @brief ADC driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/*
 * @文件说明 ADC寄存器位域控制宏集
 * @功能   定义STM32F4 ADC模块寄存器操作所需的位操作宏
 * @包含：
 * 1. 控制寄存器(CR1/CR2)字段掩码
 * 2. 序列寄存器(SQR/JSQR)通道配置
 * 3. 采样时间寄存器(SMPR)位掩码
 * 4. 数据寄存器地址映射
 * @特性  寄存器掩码设计特点：
 * - 采用按位取反的掩码形式（0xFFFF1FFF表示保留bit13-15）
 * - 高位在前编码方式（EXTSEL位在CR2的bit24-26）
 * - 多字段组合掩码（CR1_CLEAR_MASK合并多个保留位清除）
 */

/* 规则通道扫描计数器掩码（DISCNUM[2:0]）-> CR1 bit13-15 
 * 典型应用：配置扫描转换次数 (最大值8次扫描) */
#define CR1_DISCNUM_RESET         ((uint32_t)0xFFFF1FFF)  // 二进制：1111 1111 1111 1111 0001 1111 1111 1111

/* 模拟看门狗通道选择掩码（AWDCH[4:0]）-> CR1 bit0-4
 * 适用场景：监测指定通道的模拟电压是否超出阈值 */
#define CR1_AWDCH_RESET           ((uint32_t)0xFFFFFFE0)  // 末5位清零，保留通道选择字段

/* 看门狗模式复合掩码（AWDSGL位bit9 + AWDEN位bit23）*/
#define CR1_AWDMode_RESET         ((uint32_t)0xFF3FFDFF)  // 清除bit9（单通道模式）和bit23（看门狗使能）

/* CR1全寄存器安全掩码（保护保留位） 
 * 位分布：0xFCFFFEFF = 1111 1100 1111 1111 1111 1110 1111 1111 */
#define CR1_CLEAR_MASK            ((uint32_t)0xFCFFFEFF)  // 过滤CR1中所有保留位

/* 规则通道外部触发极性掩码（EXTEN[1:0]）-> CR2 bit28-29 
 * 值域：00=关闭触发 01=上升沿 10=下降沿 11=双沿触发 */
#define CR2_EXTEN_RESET           ((uint32_t)0xCFFFFFFF)  // 高4位掩码保留，清空触发极性设置

/* 注入通道触发极性复合掩码（JEXTEN[1:0] + JEXTSEL[3:0]）*/ 
#define CR2_JEXTEN_RESET          ((uint32_t)0xFFCFFFFF)  // 清空bit20-21（JEXTEN）
#define CR2_JEXTSEL_RESET         ((uint32_t)0xFFF0FFFF)  // 清空bit16-19（JEXTSEL）
#define CR2_CLEAR_MASK            ((uint32_t)0xC0FFF7FD)  // 位30-31保留位 + EXTSEL/JEXTEN等字段

/* 规则序列配置掩码组 --------------------------------------------------------
 * SQR1（总转换数L[3:0] bit20-23）：*/
#define SQR1_L_RESET              ((uint32_t)0xFF0FFFFF)  // 二进制：1111 1111 0000 1111 1111 1111 1111 
/* 通道位置配置掩码（SQ1-16的5位通道编号）*/
#define SQR3_SQ_SET               ((uint32_t)0x0000001F)  // SQR3存储SQ1-SQ6（bit0-4,5-9,...,25-29）
#define SQR2_SQ_SET               ((uint32_t)0x0000001F)  // SQ7-SQ12 
#define SQR1_SQ_SET               ((uint32_t)0x0000001F)  // SQ13-SQ16 

/* 注入序列配置 ------------------------------------------------------------
 * JSQR（注入通道数JL[1:0] bit20-21）：*/
#define JSQR_JL_SET               ((uint32_t)0x00300000)  // 值对应：00=1转换 01=2转换 10=3转换 11=4转换
#define JSQR_JL_RESET             ((uint32_t)0xFFCFFFFF)  // JL位清除掩码
#define JSQR_JSQ_SET              ((uint32_t)0x0000001F)  // 注入序列JSQ1-JSQ4的通道号（每5位一个）

/* 采样时间配置（SMPx[2:0] 每通道3位） 
 * 值对应：000=3周期 001=15周期... 111=480周期 */
#define SMPR1_SMP_SET             ((uint32_t)0x00000007)  // 通道0-9（SMPR1的bit0-2,3-5...） 
#define SMPR2_SMP_SET             ((uint32_t)0x00000007)  // 通道10-19 

/* 数据寄存器地址系统 -----------------------------------------------------
 * 注入数据寄存器偏移地址（相对ADC基地址）：
 * JDR1偏移量0x28，后续寄存器按4字节递增 */
#define JDR_OFFSET                ((uint8_t)0x28)  // 计算示例：ADC1基址+0x28 = JDR1地址
/* 双ADC模式共用数据寄存器绝对地址 */
#define CDR_ADDRESS               ((uint32_t)0x40012308)  // 包含ADC1和ADC2的转换结果 

/* 公共配置寄存器掩码 */
#define CR_CLEAR_MASK             ((uint32_t)0xFFFC30E0)  // 温度/电池监测使能位等保留位清除
  

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup ADC_Private_Functions
  * @{
  */ 

/** @defgroup ADC_Group1 Initialization and Configuration functions
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the ADC Prescaler
      (+) ADC Conversion Resolution (12bit..6bit)
      (+) Scan Conversion Mode (multichannel or one channel) for regular group
      (+) ADC Continuous Conversion Mode (Continuous or Single conversion) for 
          regular group
      (+) External trigger Edge and source of regular group, 
      (+) Converted data alignment (left or right)
      (+) The number of ADC conversions that will be done using the sequencer for 
          regular channel group
      (+) Multi ADC mode selection
      (+) Direct memory access mode selection for multi ADC mode  
      (+) Delay between 2 sampling phases (used in dual or triple interleaved modes)
      (+) Enable or disable the ADC peripheral   
@endverbatim
  * @{
  */
/**
  * @brief  ADC模块全局初始化与复位控制
  * @功能   
  *   - ADC_DeInit: 复位ADC外设至默认状态
  *   - ADC_Init: 配置ADC工作模式及通道参数
  * @特性   包含：
  *   1. 硬件复位控制（APB2总线级复位）
  *   2. 多寄存器协同配置（CR1/CR2/SQR1）
  *   3. 参数有效性全维度检查（8个assert_param验证点）
  * @注意   使用流程：
  *   1. 必须先调用ADC_DeInit复位外设
  *   2. 再调用ADC_Init进行参数配置
  */

/**
  * @brief  复位ADC外设寄存器至默认值
  * @实现   通过APB2总线复位控制器实现硬件级复位
  * @原理   先使能复位信号（ENABLE）再解除（DISABLE）完成复位脉冲
  */
void ADC_DeInit(void)
{
  /* 激活ADC模块硬件复位（APB2总线级复位） */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC, ENABLE);  // 拉低ADC所有寄存器
  
  /* 释放ADC模块复位状态 */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC, DISABLE); // 恢复寄存器可访问状态
}

/**
  * @brief  初始化ADC核心工作参数
  * @参数   ADCx: ADC实例（ADC1/ADC2/ADC3）
  * @参数   ADC_InitStruct: 包含以下配置项的结构体指针：
  *         - 分辨率(12/10/8/6位)
  *         - 扫描模式使能
  *         - 连续转换模式
  *         - 外部触发边沿及信号源
  *         - 数据对齐方式（左/右对齐）
  *         - 规则通道转换数量(1-16)
  * @实现   分三步配置控制寄存器：
  *         1. CR1: 扫描模式与分辨率
  *         2. CR2: 触发模式与数据对齐
  *         3. SQR1: 转换序列长度
  */
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct)
{
  uint32_t tmpreg1 = 0;
  uint8_t tmpreg2 = 0;
  
  /* 参数有效性验证链 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));                          // 验证ADC实例有效性
  assert_param(IS_ADC_RESOLUTION(ADC_InitStruct->ADC_Resolution)); // 分辨率参数检查
  assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_ScanConvMode)); // 扫描模式布尔值
  assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_ContinuousConvMode)); // 连续模式
  assert_param(IS_ADC_EXT_TRIG_EDGE(ADC_InitStruct->ADC_ExternalTrigConvEdge)); // 触发边沿
  assert_param(IS_ADC_EXT_TRIG(ADC_InitStruct->ADC_ExternalTrigConv));    // 触发源
  assert_param(IS_ADC_DATA_ALIGN(ADC_InitStruct->ADC_DataAlign));  // 数据对齐方式
  assert_param(IS_ADC_REGULAR_LENGTH(ADC_InitStruct->ADC_NbrOfConversion)); // 转换次数1-16
  
  /*---------------------------- CR1 寄存器配置 ------------------------*/
  /* 操作流程：
   * 1. 读取CR1当前值
   * 2. 清除目标配置位（分辨率RES[1:0] + 扫描模式SCAN）
   * 3. 写入新配置 */
  tmpreg1 = ADCx->CR1;  // 获取当前控制寄存器1状态
  tmpreg1 &= CR1_CLEAR_MASK; // 使用预定义掩码清除RES[1:0](bit24-25)和SCAN(bit8)
  
  /* 组合扫描模式与分辨率配置位：
   * - 扫描模式SCAN位(bit8) 左移8位对齐
   * - 分辨率RES[1:0] 直接写入bit24-25 */
  tmpreg1 |= (uint32_t)( ((uint32_t)ADC_InitStruct->ADC_ScanConvMode << 8) | \
                                  ADC_InitStruct->ADC_Resolution);
  ADCx->CR1 = tmpreg1;  // 写回更新后的CR1值

  /*---------------------------- CR2 寄存器配置 ------------------------*/
  tmpreg1 = ADCx->CR2;  // 获取当前控制寄存器2状态
  tmpreg1 &= CR2_CLEAR_MASK; // 清除CONT(bit1)/ALIGN(bit11)/EXTEN[1:0](bit28-29)/EXTSEL[3:0](bit24-27)
  
  /* 组合多配置项：
   * - 数据对齐ALIGN(bit11)
   * - 外部触发源EXTSEL[3:0](bit24-27)
   * - 外部触发边沿EXTEN[1:0](bit28-29)
   * - 连续模式CONT(bit1) 左移1位对齐 */
  tmpreg1 |= (uint32_t)( ADC_InitStruct->ADC_DataAlign | \
                        ADC_InitStruct->ADC_ExternalTrigConv | 
                        ADC_InitStruct->ADC_ExternalTrigConvEdge | \
                        ((uint32_t)ADC_InitStruct->ADC_ContinuousConvMode << 1));
  ADCx->CR2 = tmpreg1;  // 提交CR2配置

  /*---------------------------- SQR1 规则序列配置 ---------------------*/
  tmpreg1 = ADCx->SQR1;     // 获取当前序列寄存器状态
  tmpreg1 &= SQR1_L_RESET; // 清除转换数量配置位L[3:0](bit20-23)
  
  /* 计算转换次数配置值：
   * - 实际写入值为转换次数减1（例：16次转换写入0x0F）
   * - 左移20位对齐到L[3:0]位域 */
  tmpreg2 = (uint8_t)(ADC_InitStruct->ADC_NbrOfConversion - (uint8_t)1); 
  tmpreg1 |= ((uint32_t)tmpreg2 << 20);  // 组合到寄存器值
  ADCx->SQR1 = tmpreg1;  // 更新规则通道转换总数
}
/**
  * @brief  ADC初始化结构体默认值加载器
  * @功能   为ADC_InitTypeDef结构体提供安全的默认配置
  * @特性   默认配置特点：
  *         - 12位分辨率
  *         - 单通道单次转换模式
  *         - 右对齐数据
  *         - 禁用外部触发
  * @注意   该函数应在用户配置前调用，确保结构体参数完整性
  */
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct)
{
  /* 分辨率默认12位（最高精度） */
  ADC_InitStruct->ADC_Resolution = ADC_Resolution_12b;

  /* 禁用扫描模式（单通道工作） */
  ADC_InitStruct->ADC_ScanConvMode = DISABLE;

  /* 禁用连续转换（单次转换模式） */
  ADC_InitStruct->ADC_ContinuousConvMode = DISABLE;

  /* 关闭外部触发边沿检测 */
  ADC_InitStruct->ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;

  /* 默认触发源设为定时器1通道1（即使未启用触发边沿） */
  ADC_InitStruct->ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;

  /* 数据右对齐（符合常规数据格式） */
  ADC_InitStruct->ADC_DataAlign = ADC_DataAlign_Right;

  /* 默认1个规则通道转换（最小序列长度） */
  ADC_InitStruct->ADC_NbrOfConversion = 1;
}

/**
  * @brief  ADC多模式全局配置
  * @功能   配置多ADC协同工作参数及共用时钟设置
  * @参数   ADC_CommonInitStruct 包含：
  *         - 多ADC工作模式（独立/双/三重交替等）
  *         - ADC时钟分频系数（PCLK2的分频）
  *         - DMA访问模式（多ADC场景下的数据流管理）
  *         - 交替采样延迟周期（双ADC交替模式使用）
  * @实现   通过配置ADC通用控制寄存器(CCR)
  * @注意   该配置影响所有ADC实例，应在各ADC独立配置前调用
  */
void ADC_CommonInit(ADC_CommonInitTypeDef* ADC_CommonInitStruct)
{
  uint32_t tmpreg1 = 0;
  
  /* 参数有效性四重验证 */
  assert_param(IS_ADC_MODE(ADC_CommonInitStruct->ADC_Mode));          // 多模式有效性检查
  assert_param(IS_ADC_PRESCALER(ADC_CommonInitStruct->ADC_Prescaler)); // 时钟分频参数
  assert_param(IS_ADC_DMA_ACCESS_MODE(ADC_CommonInitStruct->ADC_DMAAccessMode)); // DMA模式
  assert_param(IS_ADC_SAMPLING_DELAY(ADC_CommonInitStruct->ADC_TwoSamplingDelay)); // 交替延迟

  /*---------------------------- CCR寄存器配置流程 --------------------*/
  tmpreg1 = ADC->CCR;  // 获取当前共用控制寄存器值
  
  /* 清除多模式相关配置位：
   * MULTI[4:0](bit13-17) | DELAY[3:0](bit8-11) 
   * DMA[1:0](bit16-17) | ADCPRE[1:0](bit16-17) */
  tmpreg1 &= CR_CLEAR_MASK;  // 使用预定义掩码0xFFFC30E0

  /* 组合配置参数：
   * - 多ADC模式（决定ADC协同工作方式）
   * - 时钟预分频（设置ADC时钟频率）
   * - DMA访问模式（控制多ADC数据流传输）
   * - 交替采样延迟（双ADC交替模式的时间间隔） */
  tmpreg1 |= (uint32_t)(ADC_CommonInitStruct->ADC_Mode | 
                      ADC_CommonInitStruct->ADC_Prescaler | 
                      ADC_CommonInitStruct->ADC_DMAAccessMode | 
                      ADC_CommonInitStruct->ADC_TwoSamplingDelay);
                      
  /* 写入更新后的共用控制寄存器 */
  ADC->CCR = tmpreg1;  // 影响所有ADC实例的全局配置
}

/**
  * @brief  ADC共用配置结构体默认值加载器
  * @功能   为多ADC协同工作参数提供安全初始化基准
  * @默认配置策略：
  *         - 独立工作模式（无多ADC协作）
  *         - 时钟2分频（平衡速度与精度）
  *         - 禁用DMA传输
  *         - 交替采样间隔5周期（即使未启用双模式）
  * @注意   该默认配置侧重系统稳定性，实际使用应根据场景调整
  */
void ADC_CommonStructInit(ADC_CommonInitTypeDef* ADC_CommonInitStruct)
{
  /* 独立工作模式（各ADC互不影响） */
  ADC_CommonInitStruct->ADC_Mode = ADC_Mode_Independent;

  /* 时钟2分频（假设PCLK2=84MHz时ADC时钟为42MHz） */
  ADC_CommonInitStruct->ADC_Prescaler = ADC_Prescaler_Div2;

  /* 禁用DMA传输（手动模式数据采集） */
  ADC_CommonInitStruct->ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;

  /* 交替采样间隔设为5个时钟周期（双ADC模式生效） */
  ADC_CommonInitStruct->ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
}

/**
  * @brief  ADC硬件使能控制器
  * @功能   控制ADC模拟电路的电源状态
  * @原理   通过CR2寄存器的ADON位进行电源控制：
  *         - 首次置位ADON：启动ADC电源（唤醒过程）
  *         - 后续置位ADON：触发转换（需配合其他配置）
  * @注意   禁用ADC将关闭模拟电路以降低功耗
  */
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* 参数双重验证 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));          // 验证ADC实例有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));    // 检查使能状态合法性

  if (NewState != DISABLE)
  {
    /* 启用ADC电源/触发转换（ADON位功能复用） 
     * 首次设置：启动ADC供电（约需要tSTAB时间稳定）
     * 后续设置：启动常规转换（需已配置通道） */
    ADCx->CR2 |= (uint32_t)ADC_CR2_ADON;  // 位或操作设置ADON(bit0)
  }
  else
  {
    /* 关闭ADC电源（进入低功耗状态） 
     * 注意：立即终止进行中的转换 */
    ADCx->CR2 &= (uint32_t)(~ADC_CR2_ADON);  // 位与操作清除ADON(bit0)
  }
}

/**
  * @}
  */

/** @defgroup ADC_Group2 Analog Watchdog configuration functions
 *  @brief    Analog Watchdog configuration functions 
 *
@verbatim   
 ===============================================================================
             ##### Analog Watchdog configuration functions #####
 ===============================================================================  
    [..] This section provides functions allowing to configure the Analog Watchdog
         (AWD) feature in the ADC.
  
    [..] A typical configuration Analog Watchdog is done following these steps :
      (#) the ADC guarded channel(s) is (are) selected using the 
          ADC_AnalogWatchdogSingleChannelConfig() function.
      (#) The Analog watchdog lower and higher threshold are configured using the  
          ADC_AnalogWatchdogThresholdsConfig() function.
      (#) The Analog watchdog is enabled and configured to enable the check, on one
          or more channels, using the  ADC_AnalogWatchdogCmd() function.
@endverbatim
  * @{
  */
  
/**
  * @brief  Enables or disables the analog watchdog on single/all regular or 
  *         injected channels
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_AnalogWatchdog: the ADC analog watchdog configuration.
  *         This parameter can be one of the following values:
  *            @arg ADC_AnalogWatchdog_SingleRegEnable: Analog watchdog on a single regular channel
  *            @arg ADC_AnalogWatchdog_SingleInjecEnable: Analog watchdog on a single injected channel
  *            @arg ADC_AnalogWatchdog_SingleRegOrInjecEnable: Analog watchdog on a single regular or injected channel
  *            @arg ADC_AnalogWatchdog_AllRegEnable: Analog watchdog on all regular channel
  *            @arg ADC_AnalogWatchdog_AllInjecEnable: Analog watchdog on all injected channel
  *            @arg ADC_AnalogWatchdog_AllRegAllInjecEnable: Analog watchdog on all regular and injected channels
  *            @arg ADC_AnalogWatchdog_None: No channel guarded by the analog watchdog
  * @retval None	  
  */
/**
  * @brief  模拟看门狗模式配置器
  * @功能   设定ADC模拟看门狗的监控范围及工作模式
  * @参数   ADC_AnalogWatchdog: 监控模式选择，可选：
  *         - 单规则通道/单注入通道监控
  *         - 全部规则或注入通道监控
  *         - 完全禁用看门狗功能
  * @实现   通过CR1寄存器的AWDEN/JAWDEN/AWDSGL位组合配置
  * @注意   使用前需已通过ADC_AnalogWatchdogThresholdsConfig设置阈值
  */
void ADC_AnalogWatchdogCmd(ADC_TypeDef* ADCx, uint32_t ADC_AnalogWatchdog)
{
  uint32_t tmpreg = 0;
  /* 参数双重验证 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));                   // ADC实例有效性检查
  assert_param(IS_ADC_ANALOG_WATCHDOG(ADC_AnalogWatchdog)); // 监控模式参数范围验证
  
  /* 寄存器操作三部曲 */
  tmpreg = ADCx->CR1;                 // 获取当前控制寄存器1状态
  
  /* 清除看门狗控制位组（AWDEN/JAWDEN/AWDSGL）：
   * AWDEN: 规则通道看门狗使能位(bit23)
   * JAWDEN: 注入通道看门狗使能位(bit22)
   * AWDSGL: 单通道监控选择位(bit9) */
  tmpreg &= CR1_AWDMode_RESET;         // 使用预定义掩码0xFF3FFDFF
  
  /* 合并新配置模式：
   * 参数ADC_AnalogWatchdog包含三个控制位的组合值 */
  tmpreg |= ADC_AnalogWatchdog;        // 位或操作写入新配置
  
  ADCx->CR1 = tmpreg;                  // 提交修改后的寄存器值
}

/**
  * @brief  模拟看门狗阈值设定器
  * @功能   配置电压监测的上下限值
  * @参数   HighThreshold: 触发中断的电压上限（12位原始值，0-4095）
  *         LowThreshold:  触发中断的电压下限（12位原始值，0-4095）
  * @注意   实际电压值需根据参考电压换算：
  *         V_actual = (Threshold * Vref) / 4096
  */
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef* ADCx, uint16_t HighThreshold,
                                        uint16_t LowThreshold)
{
  /* 三重参数检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));             // 验证ADC实例
  assert_param(IS_ADC_THRESHOLD(HighThreshold));     // 上限值12位验证
  assert_param(IS_ADC_THRESHOLD(LowThreshold));      // 下限值12位验证
  
  /* 写入高阈值寄存器（HTR） */
  ADCx->HTR = HighThreshold;  // 当转换值 > HTR时触发超限事件
  
  /* 写入低阈值寄存器（LTR） */
  ADCx->LTR = (uint16_t)LowThreshold;  // 当转换值 < LTR时触发欠限事件
}

/**
  * @brief  Configures the analog watchdog guarded single channel
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure for the analog watchdog. 
  *          This parameter can be one of the following values:
  *            @arg ADC_Channel_0: ADC Channel0 selected
  *            @arg ADC_Channel_1: ADC Channel1 selected
  *            @arg ADC_Channel_2: ADC Channel2 selected
  *            @arg ADC_Channel_3: ADC Channel3 selected
  *            @arg ADC_Channel_4: ADC Channel4 selected
  *            @arg ADC_Channel_5: ADC Channel5 selected
  *            @arg ADC_Channel_6: ADC Channel6 selected
  *            @arg ADC_Channel_7: ADC Channel7 selected
  *            @arg ADC_Channel_8: ADC Channel8 selected
  *            @arg ADC_Channel_9: ADC Channel9 selected
  *            @arg ADC_Channel_10: ADC Channel10 selected
  *            @arg ADC_Channel_11: ADC Channel11 selected
  *            @arg ADC_Channel_12: ADC Channel12 selected
  *            @arg ADC_Channel_13: ADC Channel13 selected
  *            @arg ADC_Channel_14: ADC Channel14 selected
  *            @arg ADC_Channel_15: ADC Channel15 selected
  *            @arg ADC_Channel_16: ADC Channel16 selected
  *            @arg ADC_Channel_17: ADC Channel17 selected
  *            @arg ADC_Channel_18: ADC Channel18 selected
  * @retval None
  */
/**
  * @brief  模拟看门狗单通道选择器
  * @功能   指定特定ADC通道作为模拟看门狗监控目标
  * @应用场景 当启用AWDSGL模式时，用于精确定位被监测通道
  * @参数   ADC_Channel: 目标通道编号（0-18对应具体硬件通道）
  *         例如：
  *         - ADC_Channel_0：温度传感器通道
  *         - ADC_Channel_16：内部参考电压通道
  */
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel)
{
  uint32_t tmpreg = 0;
  /* 参数双重验证 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));          // 检查ADC实例有效性(ADC1/ADC2/ADC3)
  assert_param(IS_ADC_CHANNEL(ADC_Channel));      // 验证通道号范围（0≤channel≤18）
  
  /* 安全寄存器操作流程：读-改-写 */
  tmpreg = ADCx->CR1;  // 获取当前控制寄存器1状态
  
  /* 清除原有通道配置（AWDCH[4:0]位）
   * 使用掩码CR1_AWDCH_RESET（0xFFFFFFE0）清除bit0-4 */
  tmpreg &= CR1_AWDCH_RESET;  // 二进制：11111111 11111111 11111111 11100000
  
  /* 设置新的监测通道 */
  tmpreg |= ADC_Channel;  // 直接写入0-18值到AWDCH位域（实际占用低5位）
  
  /* 提交通道配置到寄存器 */
  ADCx->CR1 = tmpreg;  // 当AWDSGL=1时，该通道将被监控
}

/**
  * @}
  */

/** @defgroup ADC_Group3 Temperature Sensor, Vrefint (Voltage Reference internal) 
 *            and VBAT (Voltage BATtery) management functions
 *  @brief   Temperature Sensor, Vrefint and VBAT management functions 
 *
@verbatim   
 ===============================================================================
      ##### Temperature Sensor, Vrefint and VBAT management functions #####
 ===============================================================================  
    [..] This section provides functions allowing to enable/ disable the internal 
         connections between the ADC and the Temperature Sensor, the Vrefint and 
         the Vbat sources.
     
    [..] A typical configuration to get the Temperature sensor and Vrefint channels 
         voltages is done following these steps :
      (#) Enable the internal connection of Temperature sensor and Vrefint sources 
          with the ADC channels using ADC_TempSensorVrefintCmd() function. 
      (#) Select the ADC_Channel_TempSensor and/or ADC_Channel_Vrefint using 
          ADC_RegularChannelConfig() or  ADC_InjectedChannelConfig() functions 
      (#) Get the voltage values, using ADC_GetConversionValue() or  
          ADC_GetInjectedConversionValue().

    [..] A typical configuration to get the VBAT channel voltage is done following 
         these steps :
      (#) Enable the internal connection of VBAT source with the ADC channel using 
          ADC_VBATCmd() function. 
      (#) Select the ADC_Channel_Vbat using ADC_RegularChannelConfig() or  
          ADC_InjectedChannelConfig() functions 
      (#) Get the voltage value, using ADC_GetConversionValue() or  
          ADC_GetInjectedConversionValue().
 
@endverbatim
  * @{
  */
  
  /**
  * @brief  内部传感器通道控制器
  * @功能   启用/禁用温度传感器和内部参考电压通道（VREFINT）
  * @原理   通过ADC通用控制寄存器(CCR)的TSVREFE位控制
  * @特性   启用后可通过以下通道访问：
  *         - 温度传感器：ADC_Channel_16
  *         - 内部参考电压：ADC_Channel_17
  * @注意   启用后需等待tSTART时间（约10us）才能稳定采样
  */
void ADC_TempSensorVrefintCmd(FunctionalState NewState)                
{
  /* 参数有效性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证ENABLE/DISABLE参数合法性
  
  if (NewState != DISABLE)
  {
    /* 启用内部传感器供电：
     * 设置CCR寄存器的TSVREFE位(bit23)
     * 同时激活温度传感器和Vrefint电路 */
    ADC->CCR |= (uint32_t)ADC_CCR_TSVREFE;  // 位或操作启用
  }
  else
  {
    /* 关闭内部传感器以降低功耗：
     * 清除CCR寄存器的TSVREFE位 */
    ADC->CCR &= (uint32_t)(~ADC_CCR_TSVREFE);  // 位与操作禁用
  }
}

/**
  * @brief  电池电压监测通道控制器
  * @功能   启用/禁用VBAT（备用电池电压）监测通道
  * @硬件特性 不同系列电压分压比：
  *         - F40x/F41x：VBAT输入电压 = 实际电压/2
  *         - F42x/F43x：VBAT输入电压 = 实际电压/4
  * @应用场景 用于电池供电系统的电压监控（RTC备份电源等）
  */
void ADC_VBATCmd(FunctionalState NewState)                             
{
  /* 参数有效性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 启用VBAT通道：
     * 设置CCR寄存器的VBATE位(bit22)
     * 激活内部电压分压器电路 */
    ADC->CCR |= (uint32_t)ADC_CCR_VBATE;  // 对应通道号为ADC_Channel_18
  }
  else
  {
    /* 禁用VBAT监测以节省能耗：
     * 典型应用下建议间歇性启用检测 */
    ADC->CCR &= (uint32_t)(~ADC_CCR_VBATE);  
  }
}

/**
  * @}
  */

/** @defgroup ADC_Group4 Regular Channels Configuration functions
 *  @brief   Regular Channels Configuration functions 
 *
@verbatim   
 ===============================================================================
             ##### Regular Channels Configuration functions #####
 ===============================================================================  

    [..] This section provides functions allowing to manage the ADC's regular channels,
         it is composed of 2 sub sections : 
  
      (#) Configuration and management functions for regular channels: This subsection 
          provides functions allowing to configure the ADC regular channels :    
         (++) Configure the rank in the regular group sequencer for each channel
         (++) Configure the sampling time for each channel
         (++) select the conversion Trigger for regular channels
         (++) select the desired EOC event behavior configuration
         (++) Activate the continuous Mode  (*)
         (++) Activate the Discontinuous Mode 
         -@@- Please Note that the following features for regular channels 
             are configurated using the ADC_Init() function : 
           (+@@) scan mode activation 
           (+@@) continuous mode activation (**) 
           (+@@) External trigger source  
           (+@@) External trigger edge 
           (+@@) number of conversion in the regular channels group sequencer.
     
         -@@- (*) and (**) are performing the same configuration
     
      (#) Get the conversion data: This subsection provides an important function in 
          the ADC peripheral since it returns the converted data of the current 
          regular channel. When the Conversion value is read, the EOC Flag is 
          automatically cleared.
     
          -@- For multi ADC mode, the last ADC1, ADC2 and ADC3 regular conversions 
              results data (in the selected multi mode) can be returned in the same 
              time using ADC_GetMultiModeConversionValue() function. 
         
@endverbatim
  * @{
  */
/**
  * @brief  Configures for the selected ADC regular channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure. 
  *          This parameter can be one of the following values:
  *            @arg ADC_Channel_0: ADC Channel0 selected
  *            @arg ADC_Channel_1: ADC Channel1 selected
  *            @arg ADC_Channel_2: ADC Channel2 selected
  *            @arg ADC_Channel_3: ADC Channel3 selected
  *            @arg ADC_Channel_4: ADC Channel4 selected
  *            @arg ADC_Channel_5: ADC Channel5 selected
  *            @arg ADC_Channel_6: ADC Channel6 selected
  *            @arg ADC_Channel_7: ADC Channel7 selected
  *            @arg ADC_Channel_8: ADC Channel8 selected
  *            @arg ADC_Channel_9: ADC Channel9 selected
  *            @arg ADC_Channel_10: ADC Channel10 selected
  *            @arg ADC_Channel_11: ADC Channel11 selected
  *            @arg ADC_Channel_12: ADC Channel12 selected
  *            @arg ADC_Channel_13: ADC Channel13 selected
  *            @arg ADC_Channel_14: ADC Channel14 selected
  *            @arg ADC_Channel_15: ADC Channel15 selected
  *            @arg ADC_Channel_16: ADC Channel16 selected
  *            @arg ADC_Channel_17: ADC Channel17 selected
  *            @arg ADC_Channel_18: ADC Channel18 selected                       
  * @param  Rank: The rank in the regular group sequencer.
  *          This parameter must be between 1 to 16.
  * @param  ADC_SampleTime: The sample time value to be set for the selected channel. 
  *          This parameter can be one of the following values:
  *            @arg ADC_SampleTime_3Cycles: Sample time equal to 3 cycles
  *            @arg ADC_SampleTime_15Cycles: Sample time equal to 15 cycles
  *            @arg ADC_SampleTime_28Cycles: Sample time equal to 28 cycles
  *            @arg ADC_SampleTime_56Cycles: Sample time equal to 56 cycles	
  *            @arg ADC_SampleTime_84Cycles: Sample time equal to 84 cycles	
  *            @arg ADC_SampleTime_112Cycles: Sample time equal to 112 cycles	
  *            @arg ADC_SampleTime_144Cycles: Sample time equal to 144 cycles	
  *            @arg ADC_SampleTime_480Cycles: Sample time equal to 480 cycles	
  * @retval None
  */
/**
  * @brief  规则通道参数配置器
  * @功能   配置指定规则通道的采样时间及转换序列位置
  * @参数   ADC_Channel: 目标通道编号（0-18对应具体物理引脚）
  *         Rank: 转换序列位置（1-16，对应SQ1-SQ16）
  *         ADC_SampleTime: 采样时钟周期数（3-480周期）
  * @实现   分两步配置：
  *         1. 采样时间写入SMPR1/SMPR2寄存器
  *         2. 通道序列位置写入SQR1/SQR2/SQR3寄存器
  */
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
  uint32_t tmpreg1 = 0, tmpreg2 = 0;
  
  /* 四重参数有效性验证 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));          // 验证ADC实例
  assert_param(IS_ADC_CHANNEL(ADC_Channel));     // 通道号0-18检查
  assert_param(IS_ADC_REGULAR_RANK(Rank));       // 转换序列位置1-16检查
  assert_param(IS_ADC_SAMPLE_TIME(ADC_SampleTime)); // 采样时间有效性

  /*---------------- 采样时间配置（SMPR1/SMPR2） ----------------*/
  if (ADC_Channel > ADC_Channel_9)  // 处理通道10-18
  {
    tmpreg1 = ADCx->SMPR1;  // 获取SMPR1当前值（通道10-18采样时间寄存器）
    
    /* 计算位偏移量（每个通道占3位，通道10从bit0开始）：
     * 示例：通道10 -> 3*(10-10)=0bit偏移
     *       通道11 -> 3*(11-10)=3bit偏移 */
    tmpreg2 = SMPR1_SMP_SET << (3 * (ADC_Channel - 10));  // 生成3bit掩码
    
    tmpreg1 &= ~tmpreg2;  // 清除原有采样时间配置
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * (ADC_Channel - 10));  // 新值移位对齐
    tmpreg1 |= tmpreg2;   // 合并新采样时间
    
    ADCx->SMPR1 = tmpreg1;  // 更新SMPR1寄存器
  }
  else  // 处理通道0-9
  {
    tmpreg1 = ADCx->SMPR2;  // 获取SMPR2当前值（通道0-9采样时间寄存器）
    
    /* 计算位偏移量（通道0从bit0开始，每通道占3位）：
     * 示例：通道0 -> 3*0=0bit偏移
     *       通道5 -> 3*5=15bit偏移 */
    tmpreg2 = SMPR2_SMP_SET << (3 * ADC_Channel);  // 生成3bit掩码
    
    tmpreg1 &= ~tmpreg2;  // 清除旧值
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);  // 新值对齐
    tmpreg1 |= tmpreg2;   // 设置新采样时间
    
    ADCx->SMPR2 = tmpreg1;  // 更新SMPR2寄存器
  }

  /*---------------- 转换序列配置（SQR1/SQR2/SQR3） ----------------*/
  if (Rank < 7)  // 处理SQ1-SQ6（SQR3寄存器）
  {
    tmpreg1 = ADCx->SQR3;  // 获取当前序列寄存器3的值
    
    /* 计算5位通道位置（每个SQ占5bit）：
     * SQ1起始位0，每个SQ间隔5bit */
    tmpreg2 = SQR3_SQ_SET << (5 * (Rank - 1));  // 生成5bit掩码
    
    tmpreg1 &= ~tmpreg2;  // 清除原有通道配置
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 1));  // 新通道号移位对齐
    tmpreg1 |= tmpreg2;   // 设置通道到指定SQ位置
    
    ADCx->SQR3 = tmpreg1;  // 更新SQR3寄存器
  }
  else if (Rank < 13)  // 处理SQ7-SQ12（SQR2寄存器）
  {
    tmpreg1 = ADCx->SQR2;
    
    /* SQ7起始位0，每个SQ间隔5bit */
    tmpreg2 = SQR2_SQ_SET << (5 * (Rank - 7));  // 示例：SQ7 -> 0bit偏移
    
    tmpreg1 &= ~tmpreg2;
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 7));
    tmpreg1 |= tmpreg2;
    
    ADCx->SQR2 = tmpreg1;
  }
  else  // 处理SQ13-SQ16（SQR1寄存器）
  {
    tmpreg1 = ADCx->SQR1;
    
    /* SQ13起始位0，每个SQ间隔5bit */
    tmpreg2 = SQR1_SQ_SET << (5 * (Rank - 13));  // 示例：SQ13 -> 0bit偏移
    
    tmpreg1 &= ~tmpreg2;
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 13));
    tmpreg1 |= tmpreg2;
    
    ADCx->SQR1 = tmpreg1;
  }
}

/**
  * @brief  规则通道转换启动器
  * @功能   通过软件触发启动规则通道组的转换
  * @原理   设置CR2寄存器的SWSTART位(bit30)触发单次转换
  * @注意   使用条件：
  *         1. 必须已配置至少一个规则通道
  *         2. 未启用外部触发模式
  */
void ADC_SoftwareStartConv(ADC_TypeDef* ADCx)
{
  /* 参数有效性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));  // 验证ADC实例
  
  /* 设置SWSTART位启动转换：
   * 该位在转换开始后由硬件自动清除 */
  ADCx->CR2 |= (uint32_t)ADC_CR2_SWSTART;  // bit30置1
}

/**
  * @brief  软件触发状态检测器
  * @功能   获取SWSTART位的当前状态
  * @返回值 SET: 转换正在进行/已触发未启动
  *         RESET: 转换未激活
  * @注意   该状态位在以下情况自动清零：
  *         - 转换实际开始（当CONT=0时）
  *         - 写入0（软件清除）
  */
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef* ADCx)
{
  FlagStatus bitstatus = RESET;
  assert_param(IS_ADC_ALL_PERIPH(ADCx));  // 参数验证
  
  /* 检测CR2寄存器的SWSTART位状态 */
  if ((ADCx->CR2 & ADC_CR2_SWSTART) != (uint32_t)RESET) {
    bitstatus = SET;    // 转换已触发但可能未开始
  } else {
    bitstatus = RESET;  // 转换未激活或已完成
  }
  
  return bitstatus;
}

/**
  * @brief  转换结束事件模式控制器
  * @功能   配置每个规则通道转换完成后是否产生EOC事件
  * @参数   NewState: 
  *         ENABLE - 每个通道转换完成触发EOC
  *         DISABLE - 整个序列完成后触发EOC
  * @应用   实时获取每个通道数据时建议启用
  */
void ADC_EOCOnEachRegularChannelCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 启用逐通道EOC触发（CR2 bit10=1） */
    ADCx->CR2 |= (uint32_t)ADC_CR2_EOCS;  // 每次转换结束置位EOC标志
  } else {
    /* 序列完成触发EOC（CR2 bit10=0） */
    ADCx->CR2 &= (uint32_t)(~ADC_CR2_EOCS); // 仅在序列结束时触发
  }
}

/**
  * @brief  连续转换模式控制器
  * @功能   启用/禁用自动循环转换模式
  * @原理   通过CR2寄存器的CONT位(bit1)控制
  * @注意   启用后需配合触发源：
  *         - 软件触发：需循环调用启动函数
  *         - 硬件触发：自动连续转换
  */
void ADC_ContinuousModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 启用连续转换（自动重启转换过程） */
    ADCx->CR2 |= (uint32_t)ADC_CR2_CONT;  // bit1置1
  } else {
    /* 单次转换模式（完成序列后停止） */
    ADCx->CR2 &= (uint32_t)(~ADC_CR2_CONT); // bit1清零
  }
}

/**
  * @brief  配置ADC规则组通道的间断模式通道数量
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  Number: 间断模式通道数量，取值范围1-8
  * @retval 无
  * @note
  * - 操作CR1寄存器的DISCNUM[15:13]位域
  * - 实际写入值为(Number-1)的二进制形式
  * - 需先调用ADC_DiscModeCmd使能间断模式才能生效
  * - 与连续转换模式互斥，需通过ADC_ContinuousModeCmd配置
  */
void ADC_DiscModeChannelCountConfig(ADC_TypeDef* ADCx, uint8_t Number)
{
  uint32_t tmpreg1 = 0;  // 临时寄存器用于CR1的修改
  uint32_t tmpreg2 = 0;  // 临时计算数值寄存器
  
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));          // 验证ADC外设有效性
  assert_param(IS_ADC_REGULAR_DISC_NUMBER(Number)); // 验证通道数量在1-8范围内
  
  /* 获取当前CR1寄存器值 */
  tmpreg1 = ADCx->CR1;  // 读取控制寄存器1当前配置
  
  /* 清除原有的间断模式通道数量设置 */
  tmpreg1 &= CR1_DISCNUM_RESET;  // 使用预定义掩码0xFFFFE7FF清空13-15位
  
  /* 计算并设置新的通道数量值 */
  tmpreg2 = Number - 1;         // 根据手册要求，实际存储值为n-1
  tmpreg1 |= tmpreg2 << 13;     // 将数值左移13位对齐到DISCNUM位域(13-15位)
  
  /* 更新寄存器配置 */
  ADCx->CR1 = tmpreg1;  // 将修改后的值写回控制寄存器1
}

/**
  * @brief  使能或禁用ADC规则通道组的间断模式
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  NewState: 功能状态，ENABLE(使能) 或 DISABLE(禁用)
  * @retval 无
  * @note
  * - 操作CR1寄存器的DISCEN位(11位)
  * - 需配合ADC_DiscModeChannelCountConfig设置通道数量
  * - 使能后按照设定通道数进行间断采样
  */
void ADC_DiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));       // 验证ADC外设有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 验证功能状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 使能规则通道间断模式 */
    ADCx->CR1 |= (uint32_t)ADC_CR1_DISCEN;  // 设置CR1第11位为1
  }
  else
  {
    /* 禁用规则通道间断模式 */
    ADCx->CR1 &= (uint32_t)(~ADC_CR1_DISCEN); // 清除CR1第11位
  }
}

/**
  * @brief  获取指定ADC的规则通道转换结果
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @retval uint16_t: 12位ADC转换结果值（实际精度取决于ADC配置）
  * @note
  * - 直接读取数据寄存器(DR)
  * - 在单次转换模式下需等待转换完成后再读取
  * - 在连续转换模式下会持续更新寄存器值
  */
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx)
{
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));  // 验证ADC外设有效性
  
  /* 返回数据寄存器当前值 */
  return (uint16_t) ADCx->DR;  // 强制转换为16位，实际有效位可能为12/10/8位
}

/**
  * @brief  获取多ADC模式下的联合转换结果（双/三ADC模式专用）
  * @param  无
  * @retval uint32_t: 组合转换结果数据，具体格式取决于模式：
  *         - 双模式: [31:16]ADC2数据，[15:0]ADC1数据
  *         - 三模式: 数据交替存储，需结合转换序列解析
  * @note
  * - 访问公共数据寄存器(CDR)
  * - 仅适用于配置了双ADC或三ADC模式的场景
  * - 需要先配置ADC_MultiModeTypeDef结构体启用多模式
  */
uint32_t ADC_GetMultiModeConversionValue(void)
{
  /* 通过类型转换访问联合数据寄存器 */
  return (*(__IO uint32_t *) CDR_ADDRESS);  // CDR_ADDRESS为预定义的多ADC数据寄存器地址
  // __IO表示volatile访问，确保直接操作硬件寄存器
}

/**
  * @}
  */

/** @defgroup ADC_Group5 Regular Channels DMA Configuration functions
 *  @brief   Regular Channels DMA Configuration functions 
 *
@verbatim   
 ===============================================================================
            ##### Regular Channels DMA Configuration functions #####
 ===============================================================================  
    [..] This section provides functions allowing to configure the DMA for ADC 
         regular channels.
         Since converted regular channel values are stored into a unique data 
         register, it is useful to use DMA for conversion of more than one regular 
         channel. This avoids the loss of the data already stored in the ADC 
         Data register.   
         When the DMA mode is enabled (using the ADC_DMACmd() function), after each
         conversion of a regular channel, a DMA request is generated.
    [..] Depending on the "DMA disable selection for Independent ADC mode" 
         configuration (using the ADC_DMARequestAfterLastTransferCmd() function), 
         at the end of the last DMA transfer, two possibilities are allowed:
      (+) No new DMA request is issued to the DMA controller (feature DISABLED) 
      (+) Requests can continue to be generated (feature ENABLED).  
    [..] Depending on the "DMA disable selection for multi ADC mode" configuration 
         (using the void ADC_MultiModeDMARequestAfterLastTransferCmd() function), 
         at the end of the last DMA transfer, two possibilities are allowed:
        (+) No new DMA request is issued to the DMA controller (feature DISABLED) 
        (+) Requests can continue to be generated (feature ENABLED).

@endverbatim
  * @{
  */
  /**
  * @brief  启用或禁用指定ADC的DMA请求功能
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  NewState: 功能状态，ENABLE(使能) 或 DISABLE(禁用)
  * @retval 无
  * @note
  * - 操作CR2寄存器的DMA位（第8位）
  * - 启用后ADC转换完成时自动触发DMA传输
  * - 需配合DMA控制器配置使用
  * - 在DMA模式禁用时才能修改此配置
  */
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));       // 检查ADC外设有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 验证状态参数合法性

  if (NewState != DISABLE)
  {
    /* 启用指定ADC的DMA请求 */
    ADCx->CR2 |= (uint32_t)ADC_CR2_DMA;  // 设置CR2第8位(DMA使能位)
  }
  else
  {
    /* 禁用指定ADC的DMA请求 */
    ADCx->CR2 &= (uint32_t)(~ADC_CR2_DMA); // 清除CR2第8位
  }
}

/**
  * @brief  配置单ADC模式下DMA请求在最后一次传输后的行为
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  NewState: 功能状态，ENABLE(持续请求) 或 DISABLE(停止请求)
  * @retval 无
  * @note
  * - 操作CR2寄存器的DDS位（第9位）
  * - ENABLE: 转换完成后继续产生DMA请求
  * - DISABLE: 最后一次传输后停止DMA请求
  * - 需在DMA模式启用时配置
  */
void ADC_DMARequestAfterLastTransferCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));       // 检查ADC外设有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 验证状态参数合法性

  if (NewState != DISABLE)
  {
    /* 启用最后一次传输后持续DMA请求 */
    ADCx->CR2 |= (uint32_t)ADC_CR2_DDS;  // 设置CR2第9位(持续请求位)
  }
  else
  {
    /* 禁用最后一次传输后DMA请求 */
    ADCx->CR2 &= (uint32_t)(~ADC_CR2_DDS); // 清除CR2第9位
  }
}

/**
  * @brief  配置多ADC模式下DMA请求在最后一次传输后的行为
  * @param  NewState: 功能状态，ENABLE(持续请求) 或 DISABLE(停止请求)
  * @retval 无
  * @note
  * - 操作共用寄存器CCR的DDS位（第13位）
  * - 仅适用于多ADC模式（双/三重模式）
  * - 需要先通过ADC_CommonInit()配置DMA访问模式
  * - ENABLE: 在多ADC模式下持续产生DMA请求
  * - DISABLE: 完成设定转换次数后停止请求
  */
void ADC_MultiModeDMARequestAfterLastTransferCmd(FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 验证状态参数合法性

  if (NewState != DISABLE)
  {
    /* 启用多模式下的持续DMA请求 */
    ADC->CCR |= (uint32_t)ADC_CCR_DDS;  // 设置CCR第13位(多ADC持续请求位)
  }
  else
  {
    /* 禁用多模式下的持续DMA请求 */
    ADC->CCR &= (uint32_t)(~ADC_CCR_DDS); // 清除CCR第13位
  }
}

/**
  * @}
  */

/** @defgroup ADC_Group6 Injected channels Configuration functions
 *  @brief   Injected channels Configuration functions 
 *
@verbatim   
 ===============================================================================
              ##### Injected channels Configuration functions #####
 ===============================================================================  

    [..] This section provide functions allowing to configure the ADC Injected channels,
         it is composed of 2 sub sections : 
    
      (#) Configuration functions for Injected channels: This subsection provides 
          functions allowing to configure the ADC injected channels :    
        (++) Configure the rank in the injected group sequencer for each channel
        (++) Configure the sampling time for each channel    
        (++) Activate the Auto injected Mode  
        (++) Activate the Discontinuous Mode 
        (++) scan mode activation  
        (++) External/software trigger source   
        (++) External trigger edge 
        (++) injected channels sequencer.
    
      (#) Get the Specified Injected channel conversion data: This subsection 
          provides an important function in the ADC peripheral since it returns the 
          converted data of the specific injected channel.

@endverbatim
  * @{
  */ 
/**
  * @brief  Configures for the selected ADC injected channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure. 
  *          This parameter can be one of the following values:
  *            @arg ADC_Channel_0: ADC Channel0 selected
  *            @arg ADC_Channel_1: ADC Channel1 selected
  *            @arg ADC_Channel_2: ADC Channel2 selected
  *            @arg ADC_Channel_3: ADC Channel3 selected
  *            @arg ADC_Channel_4: ADC Channel4 selected
  *            @arg ADC_Channel_5: ADC Channel5 selected
  *            @arg ADC_Channel_6: ADC Channel6 selected
  *            @arg ADC_Channel_7: ADC Channel7 selected
  *            @arg ADC_Channel_8: ADC Channel8 selected
  *            @arg ADC_Channel_9: ADC Channel9 selected
  *            @arg ADC_Channel_10: ADC Channel10 selected
  *            @arg ADC_Channel_11: ADC Channel11 selected
  *            @arg ADC_Channel_12: ADC Channel12 selected
  *            @arg ADC_Channel_13: ADC Channel13 selected
  *            @arg ADC_Channel_14: ADC Channel14 selected
  *            @arg ADC_Channel_15: ADC Channel15 selected
  *            @arg ADC_Channel_16: ADC Channel16 selected
  *            @arg ADC_Channel_17: ADC Channel17 selected
  *            @arg ADC_Channel_18: ADC Channel18 selected                       
  * @param  Rank: The rank in the injected group sequencer. 
  *          This parameter must be between 1 to 4.
  * @param  ADC_SampleTime: The sample time value to be set for the selected channel. 
  *          This parameter can be one of the following values:
  *            @arg ADC_SampleTime_3Cycles: Sample time equal to 3 cycles
  *            @arg ADC_SampleTime_15Cycles: Sample time equal to 15 cycles
  *            @arg ADC_SampleTime_28Cycles: Sample time equal to 28 cycles
  *            @arg ADC_SampleTime_56Cycles: Sample time equal to 56 cycles	
  *            @arg ADC_SampleTime_84Cycles: Sample time equal to 84 cycles	
  *            @arg ADC_SampleTime_112Cycles: Sample time equal to 112 cycles	
  *            @arg ADC_SampleTime_144Cycles: Sample time equal to 144 cycles	
  *            @arg ADC_SampleTime_480Cycles: Sample time equal to 480 cycles	
  * @retval None
  */
/**
  * @brief  配置ADC注入通道的采样时间和序列顺序
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_Channel: 注入通道编号（0-18取决于具体型号）
  * @param  Rank: 通道在注入序列中的位置（1-4）
  * @param  ADC_SampleTime: 采样时间（@ref ADC_sampling_times）
  * @retval 无
  * @note
  * - 操作SMPR1/SMPR2寄存器设置采样时间
  * - 操作JSQR寄存器配置注入序列顺序
  * - 通道0-9使用SMPR2，10-18使用SMPR1
  * - 实际Rank位置由当前序列长度决定
  */
void ADC_InjectedChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
  uint32_t tmpreg1 = 0, tmpreg2 = 0, tmpreg3 = 0;
  
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));            // 验证ADC外设有效性
  assert_param(IS_ADC_CHANNEL(ADC_Channel));        // 验证通道号有效性
  assert_param(IS_ADC_INJECTED_RANK(Rank));         // 验证序列位置有效性（1-4）
  assert_param(IS_ADC_SAMPLE_TIME(ADC_SampleTime)); // 验证采样时间参数有效性

  /* 处理高编号通道（10-18） */
  if (ADC_Channel > ADC_Channel_9)
  {
    /* 配置SMPR1寄存器（通道10-18） */
    tmpreg1 = ADCx->SMPR1;  // 获取当前采样时间寄存器1的值
    tmpreg2 = SMPR1_SMP_SET << (3*(ADC_Channel - 10));  // 计算3位掩码位置（每个通道占3位）
    tmpreg1 &= ~tmpreg2;     // 清空目标通道的原有采样时间
    tmpreg2 = (uint32_t)ADC_SampleTime << (3*(ADC_Channel - 10)); // 计算新采样时间位置
    tmpreg1 |= tmpreg2;      // 设置新的采样时间
    ADCx->SMPR1 = tmpreg1;  // 更新采样时间寄存器1
  }
  /* 处理低编号通道（0-9） */
  else
  {
    /* 配置SMPR2寄存器（通道0-9） */
    tmpreg1 = ADCx->SMPR2;  // 获取当前采样时间寄存器2的值
    tmpreg2 = SMPR2_SMP_SET << (3 * ADC_Channel);  // 计算3位掩码位置
    tmpreg1 &= ~tmpreg2;     // 清空目标通道的原有采样时间
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel); // 计算新采样时间位置
    tmpreg1 |= tmpreg2;      // 设置新的采样时间
    ADCx->SMPR2 = tmpreg1;  // 更新采样时间寄存器2
  }

  /* 配置注入序列顺序（JSQR寄存器） */
  tmpreg1 = ADCx->JSQR;              // 获取当前注入序列寄存器值
  tmpreg3 = (tmpreg1 & JSQR_JL_SET) >> 20; // 提取当前序列长度JL值（0-3对应1-4通道）
  
  /* 计算目标Rank的位偏移 */
  tmpreg2 = JSQR_JSQ_SET << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1))); // 计算5位掩码位置（每个序列项占5位）
  tmpreg1 &= ~tmpreg2;              // 清空目标Rank位置的原有通道配置
  tmpreg2 = (uint32_t)ADC_Channel << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1))); // 计算新通道位置
  tmpreg1 |= tmpreg2;               // 设置新的通道编号
  ADCx->JSQR = tmpreg1;             // 更新注入序列寄存器
}

/**
  * @brief  配置注入通道的转换序列长度
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  Length: 注入序列长度（1-4）
  * @retval 无
  * @note
  * - 操作JSQR寄存器的JL[21:20]位
  * - 实际写入值为Length-1
  * - 影响注入序列的排序方式
  * - 修改后需要重新配置各通道Rank
  */
void ADC_InjectedSequencerLengthConfig(ADC_TypeDef* ADCx, uint8_t Length)
{
  uint32_t tmpreg1 = 0;
  uint32_t tmpreg2 = 0;
  
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));       // 验证ADC外设有效性
  assert_param(IS_ADC_INJECTED_LENGTH(Length)); // 验证序列长度有效性（1-4）

  tmpreg1 = ADCx->JSQR;        // 获取当前注入序列寄存器值
  tmpreg1 &= JSQR_JL_RESET;    // 清空JL位（21:20）
  tmpreg2 = Length - 1;        // 转换为寄存器值（0=1通道，3=4通道）
  tmpreg1 |= tmpreg2 << 20;    // 设置新的序列长度值
  ADCx->JSQR = tmpreg1;        // 更新注入序列寄存器
}

/**
  * @brief  设置注入通道的转换结果偏移值
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_InjectedChannel: 注入通道选择（1-4）
  * @param  Offset: 12位偏移值（0-0xFFF）
  * @retval 无
  * @note
  * - 直接操作JOFR1-JOFR4寄存器
  * - 偏移值在转换结果计算时自动扣除
  * - 用于校准或补偿特定通道的测量值
  */
void ADC_SetInjectedOffset(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset)
{
    __IO uint32_t tmp = 0;
  
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));                // 验证ADC外设有效性
  assert_param(IS_ADC_INJECTED_CHANNEL(ADC_InjectedChannel)); // 验证通道选择有效性（1-4）
  assert_param(IS_ADC_OFFSET(Offset));                   // 验证偏移值有效性（≤0xFFF）

  tmp = (uint32_t)ADCx;                   // 获取ADC外设基地址
  tmp += ADC_InjectedChannel;            // 计算偏移寄存器地址（JOFRx = ADCx + 0x14 + (ch-1)*4）
  
  /* 写入偏移寄存器 */
  *(__IO uint32_t *) tmp = (uint32_t)Offset; // 直接操作寄存器指针写入偏移值
  // 注：JOFR寄存器实际为16位，但使用32位访问保证原子操作
}

 /**
  * @brief  Configures the ADCx external trigger for injected channels conversion.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_ExternalTrigInjecConv: specifies the ADC trigger to start injected conversion.
  *          This parameter can be one of the following values:                    
  *            @arg ADC_ExternalTrigInjecConv_T1_CC4: Timer1 capture compare4 selected 
  *            @arg ADC_ExternalTrigInjecConv_T1_TRGO: Timer1 TRGO event selected 
  *            @arg ADC_ExternalTrigInjecConv_T2_CC1: Timer2 capture compare1 selected 
  *            @arg ADC_ExternalTrigInjecConv_T2_TRGO: Timer2 TRGO event selected 
  *            @arg ADC_ExternalTrigInjecConv_T3_CC2: Timer3 capture compare2 selected 
  *            @arg ADC_ExternalTrigInjecConv_T3_CC4: Timer3 capture compare4 selected 
  *            @arg ADC_ExternalTrigInjecConv_T4_CC1: Timer4 capture compare1 selected                       
  *            @arg ADC_ExternalTrigInjecConv_T4_CC2: Timer4 capture compare2 selected 
  *            @arg ADC_ExternalTrigInjecConv_T4_CC3: Timer4 capture compare3 selected                        
  *            @arg ADC_ExternalTrigInjecConv_T4_TRGO: Timer4 TRGO event selected 
  *            @arg ADC_ExternalTrigInjecConv_T5_CC4: Timer5 capture compare4 selected                        
  *            @arg ADC_ExternalTrigInjecConv_T5_TRGO: Timer5 TRGO event selected                        
  *            @arg ADC_ExternalTrigInjecConv_T8_CC2: Timer8 capture compare2 selected
  *            @arg ADC_ExternalTrigInjecConv_T8_CC3: Timer8 capture compare3 selected                        
  *            @arg ADC_ExternalTrigInjecConv_T8_CC4: Timer8 capture compare4 selected 
  *            @arg ADC_ExternalTrigInjecConv_Ext_IT15: External interrupt line 15 event selected                          
  * @retval None
  */
/**
  * @brief  配置注入通道的外部触发源
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_ExternalTrigInjecConv: 外部触发源选择（参见@ref ADC_External_trigger_sources_for_injected_channels）
  * @retval 无
  * @note
  * - 操作CR2寄存器的JEXTSEL[14:12]位
  * - 支持定时器TRGO事件或外部中断线触发
  * - 需配合ADC_ExternalTrigInjectedConvEdgeConfig设置触发边沿
  */
void ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef* ADCx, uint32_t ADC_ExternalTrigInjecConv)
{
  uint32_t tmpreg = 0;
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));                // 验证ADC外设有效性
  assert_param(IS_ADC_EXT_INJEC_TRIG(ADC_ExternalTrigInjecConv)); // 验证触发源有效性
  
  tmpreg = ADCx->CR2;          // 获取当前控制寄存器2的值
  tmpreg &= CR2_JEXTSEL_RESET; // 清空JEXTSEL位域（14:12位）
  tmpreg |= ADC_ExternalTrigInjecConv; // 设置新的触发源值
  ADCx->CR2 = tmpreg;          // 更新控制寄存器2
}

/**
  * @brief  配置注入通道外部触发边沿检测
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_ExternalTrigInjecConvEdge: 触发边沿选项：
  *          @arg ADC_ExternalTrigInjecConvEdge_None     禁用触发
  *          @arg ADC_ExternalTrigInjecConvEdge_Rising   上升沿触发
  *          @arg ADC_ExternalTrigInjecConvEdge_Falling  下降沿触发
  *          @arg ADC_ExternalTrigInjecConvEdge_RisingFalling 双边沿触发
  * @retval 无
  * @note
  * - 操作CR2寄存器的JEXTEN[11:10]位
  * - 必须先配置外部触发源才能生效
  * - 边沿检测与触发源功能需硬件支持
  */
void ADC_ExternalTrigInjectedConvEdgeConfig(ADC_TypeDef* ADCx, uint32_t ADC_ExternalTrigInjecConvEdge)
{
  uint32_t tmpreg = 0;
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));                    // 验证ADC外设有效性
  assert_param(IS_ADC_EXT_INJEC_TRIG_EDGE(ADC_ExternalTrigInjecConvEdge)); // 验证边沿参数有效性
  
  tmpreg = ADCx->CR2;          // 获取当前控制寄存器2的值
  tmpreg &= CR2_JEXTEN_RESET;  // 清空JEXTEN位域（11:10位）
  tmpreg |= ADC_ExternalTrigInjecConvEdge; // 设置新的边沿检测模式
  ADCx->CR2 = tmpreg;          // 更新控制寄存器2
}

/**
  * @brief  软件启动注入通道转换
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @retval 无
  * @note
  * - 操作CR2寄存器的JSWSTART位（第22位）
  * - 立即触发一次注入序列转换
  * - 在单次转换模式下仅执行一次
  * - 优先级高于外部硬件触发
  */
void ADC_SoftwareStartInjectedConv(ADC_TypeDef* ADCx)
{
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));  // 验证ADC外设有效性
  
  ADCx->CR2 |= (uint32_t)ADC_CR2_JSWSTART; // 设置软件启动位（自动清零）
  // 注：此位由硬件在转换开始时自动清除
}

/**
  * @brief  获取软件启动注入转换状态
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @retval FlagStatus: 转换启动状态（SET表示正在启动，RESET表示空闲）
  * @note
  * - 检测CR2寄存器的JSWSTART位状态
  * - SET表示软件触发正在等待转换启动
  * - RESET表示没有待处理的软件触发请求
  * - 该状态位由硬件自动清除
  */
FlagStatus ADC_GetSoftwareStartInjectedConvCmdStatus(ADC_TypeDef* ADCx)
{
  FlagStatus bitstatus = RESET;
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx)); // 验证ADC外设有效性
  
  if ((ADCx->CR2 & ADC_CR2_JSWSTART) != (uint32_t)RESET)
  {
    bitstatus = SET;  // JSWSTART位置1表示存在待处理请求
  }
  else
  {
    bitstatus = RESET; // JSWSTART位为0表示空闲状态
  }
  return  bitstatus; // 返回当前触发状态
}
/**
  * @brief  启用或禁用规则组转换后自动执行注入组转换
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  NewState: 功能状态，ENABLE(自动注入) 或 DISABLE(手动注入)
  * @retval 无
  * @note
  * - 操作CR1寄存器的JAUTO位（第12位）
  * - 使能后规则组转换结束自动触发注入组转换
  * - 注入序列长度需提前配置
  * - 自动注入模式下忽略外部触发配置
  */
void ADC_AutoInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));        // 验证ADC外设有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态参数有效性

  if (NewState != DISABLE)
  {
    /* 启用规则组后自动注入转换 */
    ADCx->CR1 |= (uint32_t)ADC_CR1_JAUTO;  // 设置CR1第12位（自动注入使能）
  }
  else
  {
    /* 禁用自动注入转换 */
    ADCx->CR1 &= (uint32_t)(~ADC_CR1_JAUTO); // 清除CR1第12位
  }
}

/**
  * @brief  配置注入通道的间断采样模式
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  NewState: 功能状态，ENABLE(间断模式) 或 DISABLE(连续模式)
  * @retval 无
  * @note
  * - 操作CR1寄存器的JDISCEN位（第13位）
  * - 使能后按设定通道数进行间断采样
  * - 需配合ADC_DiscModeChannelCountConfig设置通道数量
  * - 间断模式优先级高于自动注入模式
  */
void ADC_InjectedDiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));        // 验证ADC外设有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态参数有效性

  if (NewState != DISABLE)
  {
    /* 启用注入通道间断采样模式 */
    ADCx->CR1 |= (uint32_t)ADC_CR1_JDISCEN;  // 设置CR1第13位（间断模式使能）
  }
  else
  {
    /* 禁用注入通道间断采样模式 */
    ADCx->CR1 &= (uint32_t)(~ADC_CR1_JDISCEN); // 清除CR1第13位
  }
}

/**
  * @brief  获取指定注入通道的转换结果
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_InjectedChannel: 注入通道选择（1-4）
  * @retval uint16_t: 12位ADC转换结果（实际精度取决于配置）
  * @note
  * - 直接读取JDR1-JDR4寄存器
  * - 结果已包含注入偏移值计算
  * - 建议在转换完成标志置位后读取
  */
uint16_t ADC_GetInjectedConversionValue(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel)
{
  __IO uint32_t tmp = 0;
  
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));                // 验证ADC外设有效性
  assert_param(IS_ADC_INJECTED_CHANNEL(ADC_InjectedChannel)); // 验证通道选择有效性（1-4）

  /* 计算注入数据寄存器地址偏移 */
  tmp = (uint32_t)ADCx;                      // 获取ADC外设基地址
  tmp += ADC_InjectedChannel + JDR_OFFSET;   // 计算JDRx地址（基地址+0x3C + (ch-1)*4）
  
  /* 返回指定注入通道的转换结果 */
  return (uint16_t) (*(__IO uint32_t*) tmp); // 强制转换为16位访问（实际有效位12位）
  // 注：使用__IO确保直接访问硬件寄存器，防止编译器优化
}

/**
  * @}
  */

/** @defgroup ADC_Group7 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
            ##### Interrupts and flags management functions #####
 ===============================================================================  

    [..] This section provides functions allowing to configure the ADC Interrupts 
         and to get the status and clear flags and Interrupts pending bits.
  
    [..] Each ADC provides 4 Interrupts sources and 6 Flags which can be divided
        into 3 groups:
  
  *** Flags and Interrupts for ADC regular channels ***
  =====================================================
    [..]
      (+) Flags :
        (##) ADC_FLAG_OVR : Overrun detection when regular converted data are lost

        (##) ADC_FLAG_EOC : Regular channel end of conversion ==> to indicate 
             (depending on EOCS bit, managed by ADC_EOCOnEachRegularChannelCmd() )
             the end of:
             (+++) a regular CHANNEL conversion 
             (+++) sequence of regular GROUP conversions .

        (##) ADC_FLAG_STRT: Regular channel start ==> to indicate when regular 
             CHANNEL conversion starts.
    [..]
      (+) Interrupts :
        (##) ADC_IT_OVR : specifies the interrupt source for Overrun detection 
             event.  
        (##) ADC_IT_EOC : specifies the interrupt source for Regular channel end
             of conversion event.
  
  
  *** Flags and Interrupts for ADC Injected channels ***
  ======================================================
    [..]
      (+) Flags :
        (##) ADC_FLAG_JEOC : Injected channel end of conversion ==> to indicate 
             at the end of injected GROUP conversion  
              
        (##) ADC_FLAG_JSTRT: Injected channel start ==> to indicate hardware when 
             injected GROUP conversion starts.
    [..]
      (+) Interrupts :
        (##) ADC_IT_JEOC : specifies the interrupt source for Injected channel 
             end of conversion event.     

  *** General Flags and Interrupts for the ADC ***
  ================================================ 
    [..]
      (+)Flags :
        (##) ADC_FLAG_AWD: Analog watchdog ==> to indicate if the converted voltage 
             crosses the programmed thresholds values.
    [..]          
      (+) Interrupts :
        (##) ADC_IT_AWD : specifies the interrupt source for Analog watchdog event. 

  
    [..] The user should identify which mode will be used in his application to 
         manage the ADC controller events: Polling mode or Interrupt mode.
  
    [..] In the Polling Mode it is advised to use the following functions:
      (+) ADC_GetFlagStatus() : to check if flags events occur. 
      (+) ADC_ClearFlag()     : to clear the flags events.
      
    [..] In the Interrupt Mode it is advised to use the following functions:
      (+) ADC_ITConfig()          : to enable or disable the interrupt source.
      (+) ADC_GetITStatus()       : to check if Interrupt occurs.
      (+) ADC_ClearITPendingBit() : to clear the Interrupt pending Bit 
                                   (corresponding Flag). 
@endverbatim
  * @{
  */ 
/**
  * @brief  Enables or disables the specified ADC interrupts.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupt sources to be enabled or disabled. 
  *          This parameter can be one of the following values:
  *            @arg ADC_IT_EOC: End of conversion interrupt mask
  *            @arg ADC_IT_AWD: Analog watchdog interrupt mask
  *            @arg ADC_IT_JEOC: End of injected conversion interrupt mask
  *            @arg ADC_IT_OVR: Overrun interrupt enable                       
  * @param  NewState: new state of the specified ADC interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  配置ADC中断使能状态
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_IT: 中断类型选择，支持以下位组合：
  *          @arg ADC_IT_EOC  规则通道转换结束中断
  *          @arg ADC_IT_JEOC 注入通道转换结束中断
  *          @arg ADC_IT_AWD  模拟看门狗中断
  *          @arg ADC_IT_OVR  溢出中断
  * @param  NewState: 中断状态，ENABLE(使能) 或 DISABLE(禁用)
  * @retval 无
  * @note
  * - 操作CR1寄存器中断使能位
  * - 需配合NVIC配置中断优先级
  * - 中断标志需通过ADC_ClearFlag清除
  */
void ADC_ITConfig(ADC_TypeDef* ADCx, uint16_t ADC_IT, FunctionalState NewState)  
{
  uint32_t itmask = 0;
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));         // 验证ADC外设有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));   // 验证状态参数有效性
  assert_param(IS_ADC_IT(ADC_IT));               // 验证中断类型有效性

  /* 生成中断位掩码 */
  itmask = (uint8_t)ADC_IT;                      // 获取中断位索引（宏定义对应位位置）
  itmask = (uint32_t)0x01 << itmask;             // 计算实际位掩码（如EOC对应第5位）

  if (NewState != DISABLE)
  {
    /* 使能指定ADC中断 */
    ADCx->CR1 |= itmask;  // 设置CR1对应中断使能位
  }
  else
  {
    /* 禁用指定ADC中断 */
    ADCx->CR1 &= (~(uint32_t)itmask); // 清除CR1对应中断使能位
  }
}

/**
  * @brief  获取ADC标志位状态
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_FLAG: 状态标志位选择，支持以下值：
  *          @arg ADC_FLAG_AWD   模拟看门狗触发
  *          @arg ADC_FLAG_EOC   规则通道转换完成
  *          @arg ADC_FLAG_JEOC  注入通道转换完成
  *          @arg ADC_FLAG_JSTRT 注入转换开始
  *          @arg ADC_FLAG_STRT  规则转换开始
  *          @arg ADC_FLAG_OVR   数据溢出
  * @retval FlagStatus: 标志位状态（SET置位/RESET复位）
  * @note
  * - 读取SR寄存器状态位
  * - EOC/JEOC标志可由硬件自动清除
  * - OVR标志需要软件清除
  */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));       // 验证ADC外设有效性
  assert_param(IS_ADC_GET_FLAG(ADC_FLAG));     // 验证标志位有效性

  /* 检查指定标志位状态 */
  if ((ADCx->SR & ADC_FLAG) != (uint8_t)RESET) // 与操作检测标志位
  {
    bitstatus = SET;  // 标志位置1表示事件发生
  }
  else
  {
    bitstatus = RESET; // 标志位为0表示无事件
  }
  return  bitstatus;  // 返回当前标志状态
}

/**
  * @brief  清除ADC状态标志位
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_FLAG: 要清除的标志位组合（支持位或操作）
  *          @arg ADC_FLAG_AWD | ADC_FLAG_EOC | ADC_FLAG_JEOC 
  *          @arg ADC_FLAG_JSTRT | ADC_FLAG_STRT | ADC_FLAG_OVR
  * @retval 无
  * @note
  * - 通过写1清除SR寄存器标志位
  * - OVR标志必须手动清除
  * - STRAT/JSTRT标志为只读，自动清除
  */
void ADC_ClearFlag(ADC_TypeDef* ADCx, uint8_t ADC_FLAG)
{
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));          // 验证ADC外设有效性
  assert_param(IS_ADC_CLEAR_FLAG(ADC_FLAG));      // 验证可清除标志组合

  /* 清除指定标志位（写1清零） */
  ADCx->SR = ~(uint32_t)ADC_FLAG;  // 取反后写入实现位清除
  // 例：清除OVR标志（第5位），写入0xFFFFFFDF等效于~0x20
}

/**
  * @brief  检查ADC中断是否触发且已使能
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_IT: 中断类型，支持：
  *          @arg ADC_IT_EOC  规则转换结束中断
  *          @arg ADC_IT_JEOC 注入转换结束中断
  *          @arg ADC_IT_AWD  模拟看门狗中断
  *          @arg ADC_IT_OVR  溢出中断
  * @retval ITStatus: 中断状态（SET: 中断有效触发，RESET: 未触发或未使能）
  * @note
  * - 同时检查SR标志位和CR1中断使能位
  * - 与ADC_GetFlagStatus的区别在于会验证中断是否实际使能
  * - 需在中断服务例程中调用
  */
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, uint16_t ADC_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t itmask = 0, enablestatus = 0;

  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));    // 验证ADC外设有效性
  assert_param(IS_ADC_IT(ADC_IT));           // 验证中断类型有效性

  /* 提取标志位掩码（高字节包含标志位偏移）*/
  itmask = ADC_IT >> 8;                     // 示例：ADC_IT_EOC=0x0220，itmask=0x02对应SR第5位

  /* 获取中断使能状态 */
  enablestatus = (ADCx->CR1 & ((uint32_t)0x01 << (uint8_t)ADC_IT)); // 检查CR1对应中断使能位

  /* 双重验证：标志位存在且中断已使能 */
  if (((ADCx->SR & itmask) != (uint32_t)RESET) && enablestatus)
  {
    bitstatus = SET;  // 有效中断状态（标志位置位且中断使能）
  }
  else
  {
    bitstatus = RESET; // 非有效中断状态
  }
  return  bitstatus;
}

/**
  * @brief  清除ADC中断挂起标志
  * @param  ADCx: 指定ADC外设，可取ADC1、ADC2、ADC3
  * @param  ADC_IT: 中断类型，支持位或组合：
  *          @arg ADC_IT_EOC | ADC_IT_JEOC | ADC_IT_AWD | ADC_IT_OVR
  * @retval 无
  * @note
  * - 通过写1清除SR寄存器对应标志位
  * - 必须在中断服务例程末尾调用
  * - OVR标志必须手动清除，否则后续转换被禁止
  */
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, uint16_t ADC_IT)
{
  uint8_t itmask = 0;
  /* 参数合法性检查 */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));    // 验证ADC外设有效性
  assert_param(IS_ADC_IT(ADC_IT));           // 验证中断类型有效性

  /* 提取标志位掩码（同GetITStatus处理方式）*/
  itmask = (uint8_t)(ADC_IT >> 8);           // 获取SR寄存器位偏移
  
  /* 清除中断挂起标志（写1清零）*/
  ADCx->SR = ~(uint32_t)itmask;             // 示例：ADC_IT_EOC清除时写入0xFFFFFFFD(~0x02)
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

/**
  ******************************************************************************
  * @file    stm32f4xx_dac.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
   * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Digital-to-Analog Converter (DAC) peripheral: 
  *           + DAC channels configuration: trigger, output buffer, data format
  *           + DMA management      
  *           + Interrupts and flags management
  *
 @verbatim      
 ===============================================================================
                      ##### DAC Peripheral features #####
 ===============================================================================
    [..]        
      *** DAC Channels ***
      ====================  
    [..]  
    The device integrates two 12-bit Digital Analog Converters that can 
    be used independently or simultaneously (dual mode):
      (#) DAC channel1 with DAC_OUT1 (PA4) as output
      (#) DAC channel2 with DAC_OUT2 (PA5) as output
  
      *** DAC Triggers ***
      ====================
    [..]
    Digital to Analog conversion can be non-triggered using DAC_Trigger_None
    and DAC_OUT1/DAC_OUT2 is available once writing to DHRx register 
    using DAC_SetChannel1Data() / DAC_SetChannel2Data() functions.
    [..] 
    Digital to Analog conversion can be triggered by:
      (#) External event: EXTI Line 9 (any GPIOx_Pin9) using DAC_Trigger_Ext_IT9.
          The used pin (GPIOx_Pin9) must be configured in input mode.
  
      (#) Timers TRGO: TIM2, TIM4, TIM5, TIM6, TIM7 and TIM8 
          (DAC_Trigger_T2_TRGO, DAC_Trigger_T4_TRGO...)
          The timer TRGO event should be selected using TIM_SelectOutputTrigger()
  
      (#) Software using DAC_Trigger_Software
  
      *** DAC Buffer mode feature ***
      =============================== 
      [..] 
      Each DAC channel integrates an output buffer that can be used to 
      reduce the output impedance, and to drive external loads directly
      without having to add an external operational amplifier.
      To enable, the output buffer use  
      DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
      [..]           
      (@) Refer to the device datasheet for more details about output 
          impedance value with and without output buffer.
            
       *** DAC wave generation feature ***
       =================================== 
       [..]     
       Both DAC channels can be used to generate
         (#) Noise wave using DAC_WaveGeneration_Noise
         (#) Triangle wave using DAC_WaveGeneration_Triangle
          
          -@-  Wave generation can be disabled using DAC_WaveGeneration_None
  
       *** DAC data format ***
       =======================
       [..]   
       The DAC data format can be:
         (#) 8-bit right alignment using DAC_Align_8b_R
         (#) 12-bit left alignment using DAC_Align_12b_L
         (#) 12-bit right alignment using DAC_Align_12b_R
  
       *** DAC data value to voltage correspondence ***  
       ================================================ 
       [..] 
       The analog output voltage on each DAC channel pin is determined
       by the following equation: 
       DAC_OUTx = VREF+ * DOR / 4095
       with  DOR is the Data Output Register
          VEF+ is the input voltage reference (refer to the device datasheet)
        e.g. To set DAC_OUT1 to 0.7V, use
          DAC_SetChannel1Data(DAC_Align_12b_R, 868);
          Assuming that VREF+ = 3.3V, DAC_OUT1 = (3.3 * 868) / 4095 = 0.7V
  
       *** DMA requests  ***
       =====================
       [..]    
       A DMA1 request can be generated when an external trigger (but not
       a software trigger) occurs if DMA1 requests are enabled using
       DAC_DMACmd()
       [..]
       DMA1 requests are mapped as following:
         (#) DAC channel1 : mapped on DMA1 Stream5 channel7 which must be 
             already configured
         (#) DAC channel2 : mapped on DMA1 Stream6 channel7 which must be 
             already configured
  
      
                      ##### How to use this driver #####
 ===============================================================================
    [..]          
      (+) DAC APB clock must be enabled to get write access to DAC
          registers using
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE)
      (+) Configure DAC_OUTx (DAC_OUT1: PA4, DAC_OUT2: PA5) in analog mode.
      (+) Configure the DAC channel using DAC_Init() function
      (+) Enable the DAC channel using DAC_Cmd() function
   
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
#include "stm32f4xx_dac.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup DAC 
  * @brief DAC driver modules
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* CR寄存器清除掩码 - 用于重置控制寄存器配置位 
   掩码二进制：00000000 00000000 00001111 11111110
   作用：清除通道1/2的EN位、TSELx位、WAVEx位及DMA相关配置位
*/
#define CR_CLEAR_MASK              ((uint32_t)0x00000FFE)

/* 双通道软件触发控制掩码 
   SET掩码：   00000000 00000000 00000000 00000011 
               同时置位通道1(SWTRIG1)和通道2(SWTRIG2)触发位
   RESET掩码： 11111111 11111111 11111111 11111100 
               清除双通道的触发位（保留其他位状态）
*/
#define DUAL_SWTRIG_SET            ((uint32_t)0x00000003)
#define DUAL_SWTRIG_RESET          ((uint32_t)0xFFFFFFFC)

/* 数据保持寄存器偏移量（相对于DAC基地址）
   DHR12R1：右对齐12位通道1数据保持寄存器偏移 (+0x08)
   DHR12R2：右对齐12位通道2数据保持寄存器偏移 (+0x14)
   DHR12RD：双通道右对齐12位数据保持寄存器偏移 (+0x20)
*/
#define DHR12R1_OFFSET             ((uint32_t)0x00000008)
#define DHR12R2_OFFSET             ((uint32_t)0x00000014)
#define DHR12RD_OFFSET             ((uint32_t)0x00000020)

/* 数据输出寄存器偏移量（DOR）
   读取该寄存器可获得实际转换电压值（12位右对齐）
   偏移地址计算：DAC基地址 + 0x2C
*/
#define DOR_OFFSET                 ((uint32_t)0x0000002C)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup DAC_Private_Functions
  * @{
  */

/** @defgroup DAC_Group1 DAC channels configuration
 *  @brief   DAC channels configuration: trigger, output buffer, data format 
 *
@verbatim   
 ===============================================================================
   ##### DAC channels configuration: trigger, output buffer, data format #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  重置DAC外设寄存器至默认初始状态
  * @param  无
  * @retval 无
  * @note   通过APB1总线复位机制实现寄存器硬复位
  */
void DAC_DeInit(void)
{
  /* 激活DAC模块硬件复位 */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_DAC, ENABLE);  // 触发DAC外设异步复位
  /* 解除DAC模块复位状态 */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_DAC, DISABLE); // 恢复DAC时钟正常运行
}

/**
  * @brief  按指定参数初始化DAC通道
  * @param  DAC_Channel: 选择要配置的DAC通道
  *         可取值:
  *           @arg DAC_Channel_1: 通道1（对应PA4引脚）
  *           @arg DAC_Channel_2: 通道2（对应PA5引脚）
  * @param  DAC_InitStruct: 指向DAC配置结构体的指针
  *         包含以下配置信息：
  *           - 触发源选择
  *           - 波形生成配置
  *           - LFSR掩码/三角波幅度
  *           - 输出缓冲使能状态
  * @retval 无
  */
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct)
{
  uint32_t tmpreg1 = 0, tmpreg2 = 0;

  /* 参数合法性校验 */
  assert_param(IS_DAC_TRIGGER(DAC_InitStruct->DAC_Trigger));          // 验证触发源有效性
  assert_param(IS_DAC_GENERATE_WAVE(DAC_InitStruct->DAC_WaveGeneration));      // 验证波形模式
  assert_param(IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude)); // 验证幅度参数
  assert_param(IS_DAC_OUTPUT_BUFFER_STATE(DAC_InitStruct->DAC_OutputBuffer));   // 验证缓冲状态

/*---------------------- DAC控制寄存器(CR)配置流程 ---------------------*/
  /* 步骤1：获取当前控制寄存器值 */
  tmpreg1 = DAC->CR;  // 读取整个控制寄存器当前状态
  
  /* 步骤2：清空目标通道相关配置位（使用CR_CLEAR_MASK掩码） */
  tmpreg1 &= ~(CR_CLEAR_MASK << DAC_Channel);  // 移位操作定位到指定通道的配置区域
  
  /* 步骤3：构建新配置值 */
  // 合并触发源、波形模式、LFSR/幅度、输出缓冲四个配置参数
  tmpreg2 = (DAC_InitStruct->DAC_Trigger | 
            DAC_InitStruct->DAC_WaveGeneration |
            DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude | 
            DAC_InitStruct->DAC_OutputBuffer);
  
  /* 步骤4：将新配置值移位到指定通道位域 */
  tmpreg1 |= tmpreg2 << DAC_Channel;  // 将组合配置对齐到通道对应的寄存器位
  
  /* 步骤5：写入更新后的寄存器值 */
  DAC->CR = tmpreg1;  // 原子操作更新控制寄存器
}

/**
  * @brief  初始化DAC配置结构体为默认值
  * @param  DAC_InitStruct: 指向要初始化的配置结构体的指针
  * @retval 无
  * @note   默认配置为：
  *         - 无硬件触发（软件触发模式）
  *         - 关闭波形生成器
  *         - LFSR掩码设置为最低位
  *         - 使能输出缓冲
  */
void DAC_StructInit(DAC_InitTypeDef* DAC_InitStruct)
{
  /* 重置所有结构体成员为安全默认值 */
  DAC_InitStruct->DAC_Trigger = DAC_Trigger_None;         // 触发源：无（软件触发）
  DAC_InitStruct->DAC_WaveGeneration = DAC_WaveGeneration_None; // 波形生成：关闭
  DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; // 最小幅度设置
  DAC_InitStruct->DAC_OutputBuffer = DAC_OutputBuffer_Enable;    // 输出缓冲：启用
}

/**
  * @brief  控制DAC通道的启用状态
  * @param  DAC_Channel: 要操作的通道
  *         可取值:
  *           @arg DAC_Channel_1: 通道1
  *           @arg DAC_Channel_2: 通道2
  * @param  NewState: 通道新状态（ENABLE/DISABLE）
  * @warning 通道启用后不可修改触发源配置，需先禁用再修改
  * @retval 无
  */
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState)
{
  /* 参数校验 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));      // 验证通道选择有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));     // 验证状态参数合法性

  if (NewState != DISABLE)
  {
    /* 启用指定DAC通道（设置对应EN位） */
    DAC->CR |= (DAC_CR_EN1 << DAC_Channel);  // 通过移位激活指定通道使能位
  }
  else
  {
    /* 禁用指定DAC通道（清除对应EN位） */
    DAC->CR &= ~(DAC_CR_EN1 << DAC_Channel);  // 精确清除目标通道使能位
  }
}
/**
  * @brief  控制DAC通道的软件触发使能状态
  * @details 该函数用于启用或禁用指定DAC通道的软件触发功能。
  *         当软件触发使能时，通过写入SWTRIGR寄存器触发DAC转换。
  * @param  DAC_Channel: 选择的DAC通道
  *         参数取值范围：
  *           - DAC_Channel_1: DAC通道1
  *           - DAC_Channel_2: DAC通道2 
  * @param  NewState: 触发状态
  *          参数取值：
  *           - ENABLE: 启用软件触发
  *           - DISABLE: 禁用软件触发
  * @retval 无
  */
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));        // 验证通道参数有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));      // 验证状态参数有效性

  if (NewState != DISABLE)
  {
    /* 通过位操作设置SWTRIGR寄存器对应触发位 */
    // DAC_SWTRIGR_SWTRIG1是触发位掩码，通过通道号计算位偏移
    // 通道参数右移4位处理（假设通道定义DAC_Channel_1=0x00，DAC_Channel_2=0x10）
    DAC->SWTRIGR |= (uint32_t)DAC_SWTRIGR_SWTRIG1 << (DAC_Channel >> 4); 
  }
  else
  {
    /* 通过位操作清除SWTRIGR寄存器对应触发位 */
    // 使用按位非和与操作清除指定触发位
    DAC->SWTRIGR &= ~((uint32_t)DAC_SWTRIGR_SWTRIG1 << (DAC_Channel >> 4));
  }
}

/**
  * @brief  控制双DAC通道的同步软件触发
  * @details 该函数同时启用/禁用两个DAC通道的软件触发功能，
  *         用于需要双通道同步触发的场景
  * @param  NewState: 触发状态
  *          参数取值：
  *           - ENABLE: 同时启用双通道触发
  *           - DISABLE: 同时禁用双通道触发
  * @retval 无
  */
void DAC_DualSoftwareTriggerCmd(FunctionalState NewState)
{
  /* 参数检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态参数有效性

  if (NewState != DISABLE)
  {
    /* 设置双通道触发位 */
    // DUAL_SWTRIG_SET应定义为同时设置两个触发位的掩码（如0x00050005）
    DAC->SWTRIGR |= DUAL_SWTRIG_SET;  
  }
  else
  {
    /* 清除双通道触发位 */
    // DUAL_SWTRIG_RESET应定义为同时清除两个触发位的掩码（如0xFFFAFFFA）
    DAC->SWTRIGR &= DUAL_SWTRIG_RESET; 
  }
}

/**
  * @brief  控制DAC通道的波形生成功能
  * @details 该函数用于启用/禁用指定DAC通道的噪声波或三角波生成功能，
  *         当启用波形生成时，DAC输出将自动产生指定波形
  * @param  DAC_Channel: 选择的DAC通道
  *          参数取值范围：
  *            - DAC_Channel_1: 通道1
  *            - DAC_Channel_2: 通道2
  * @param  DAC_Wave: 波形类型
  *          参数取值范围：
  *            - DAC_Wave_Noise: 噪声波生成
  *            - DAC_Wave_Triangle: 三角波生成
  * @param  NewState: 波形生成状态
  *          参数取值：
  *            - ENABLE: 启用波形生成
  *            - DISABLE: 禁用波形生成
  * @retval 无
  */
void DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState)
{
  /* 参数三重验证 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));    // 验证通道参数有效性
  assert_param(IS_DAC_WAVE(DAC_Wave));          // 验证波形类型有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态参数有效性

  if (NewState != DISABLE)
  {
    /* 设置波形生成控制位 */
    // 将波形类型标志左移到对应通道的控制区域
    // 通道参数直接作为位偏移量（假设通道1=0，通道2=16）
    DAC->CR |= DAC_Wave << DAC_Channel;  
  }
  else
  {
    /* 清除波形生成控制位 */
    // 使用按位非和与操作清除指定波形控制位
    DAC->CR &= ~(DAC_Wave << DAC_Channel);
  }
}

/**
  * @brief  设置DAC通道1的数据保持寄存器值
  * @details 该函数根据指定的对齐方式，将数据写入通道1对应的数据保持寄存器。
  *         数据对齐方式决定寄存器的偏移地址和存储格式。
  * @param  DAC_Align: 数据对齐方式
  *         参数取值：
  *           - DAC_Align_8b_R:  8位右对齐（数据存低8位）
  *           - DAC_Align_12b_L: 12位左对齐（数据存高12位） 
  *           - DAC_Align_12b_R: 12位右对齐（数据存低12位）
  * @param  Data: 要写入的12位有效数据（实际写入值会根据对齐方式自动处理高位）
  * @retval 无
  */
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data)
{  
  __IO uint32_t tmp = 0;  // 定义易变寄存器操作指针
  
  /* 参数有效性验证 */
  assert_param(IS_DAC_ALIGN(DAC_Align));  // 确保对齐参数在合法范围内
  assert_param(IS_DAC_DATA(Data));        // 确保数据值不超过12位(0x0FFF)

  /* 计算目标寄存器地址 */
  tmp = (uint32_t)DAC_BASE;          // 获取DAC外设基地址（通常为0x40007400）
  tmp += DHR12R1_OFFSET;            // 加上通道1数据保持寄存器基础偏移量（通常为0x08）
  tmp += DAC_Align;                 // 叠加对齐方式带来的地址偏移

  /* 写入数据到目标寄存器 */
  // 将计算出的地址转换为易变指针并进行32位写操作
  // 实际写入操作将根据对齐方式自动处理数据位置
  *(__IO uint32_t *) tmp = Data;     // 示例：DAC_DHR12R1 = Data
}

/**
  * @brief  设置DAC通道2的数据保持寄存器值
  * @details 功能同通道1，但操作对象为通道2的寄存器
  * @param  DAC_Align: 数据对齐方式（同通道1参数）
  * @param  Data: 要写入的12位有效数据
  * @retval 无
  */
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data)
{
  __IO uint32_t tmp = 0;

  /* 参数检查流程同通道1 */
  assert_param(IS_DAC_ALIGN(DAC_Align));
  assert_param(IS_DAC_DATA(Data));

  /* 通道2专用地址计算 */
  tmp = (uint32_t)DAC_BASE;          // DAC基地址
  tmp += DHR12R2_OFFSET;            // 通道2基础偏移量（通常为0x14）
  tmp += DAC_Align;                 // 叠加对齐偏移

  /* 数据写入操作 */
  *(__IO uint32_t *)tmp = Data;      // 示例：DAC_DHR12R2 = Data
}

/**
  * @brief  设置双通道DAC数据保持寄存器值
  * @details 该函数实现双通道数据原子写入，确保两个通道的数据同步更新
  *         使用共用寄存器同时存储两个通道数据，避免单独写入造成的时序差
  * @param  DAC_Align: 数据对齐方式
  *         参数取值同单通道函数
  * @param  Data2: 通道2的12位数据（实际存储位置取决于对齐方式）
  * @param  Data1: 通道1的12位数据（存储在前端数据位）
  * @note 重要提示：
  *        - 双通道模式下必须使用本函数保证数据同步写入
  *        - 数据打包格式根据对齐方式不同而变化：
  *          8位右对齐：Data2[7:0] << 8 | Data1[7:0]
  *          12位对齐： Data2[11:0] << 16 | Data1[11:0]
  * @retval 无
  */
void DAC_SetDualChannelData(uint32_t DAC_Align, uint16_t Data2, uint16_t Data1)
{
  uint32_t data = 0, tmp = 0;
  
  /* 三重参数验证 */
  assert_param(IS_DAC_ALIGN(DAC_Align));
  assert_param(IS_DAC_DATA(Data1));  // 验证Data1 <= 0x0FFF
  assert_param(IS_DAC_DATA(Data2));  // 验证Data2 <= 0x0FFF

  /* 数据打包处理 */
  if (DAC_Align == DAC_Align_8b_R) 
  {
    // 8位右对齐模式：通道2数据存高8位，通道1存低8位
    data = ((uint32_t)Data2 << 8) | Data1;  // 结果格式：0x0000DDD2_DDD1
  }
  else 
  {
    // 12位对齐模式：通道2数据存高12位，通道1存低12位
    data = ((uint32_t)Data2 << 16) | Data1; // 结果格式：0xDDD2DDD1
  }
  
  /* 计算双通道寄存器地址 */
  tmp = (uint32_t)DAC_BASE;          // DAC基地址
  tmp += DHR12RD_OFFSET;            // 双通道寄存器偏移（通常为0x20）
  tmp += DAC_Align;                 // 叠加对齐偏移

  /* 原子写入双通道数据 */
  // 单次32位写入操作同时更新两个通道数据
  *(__IO uint32_t *)tmp = data;      // 示例：DAC_DHR12RD = data
}

/**
  * @brief  获取DAC通道当前输出值
  * @details 该函数通过读取数据输出寄存器(DORx)，获取指定通道的当前DAC转换值。
  *         该值为实际输出到管脚的模拟量对应的数字量
  * @param  DAC_Channel: 目标通道
  *         参数取值范围：
  *           - DAC_Channel_1: 通道1
  *           - DAC_Channel_2: 通道2
  * @retval 当前输出值（12位有效数据）
  */
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel)
{
  __IO uint32_t tmp = 0;  // 定义易变寄存器指针
  
  /* 参数合法性验证 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));  // 确保通道参数为合法值

  /* 计算数据输出寄存器地址 */
  tmp = (uint32_t) DAC_BASE ;         // DAC外设基地址（0x40007400）
  tmp += DOR_OFFSET;                  // 加上数据输出寄存器基础偏移（通常通道1为0x2C）
  tmp += ((uint32_t)DAC_Channel >> 2); // 通道号右移2位处理（假设通道1=0x00→0，通道2=0x10→4）

  /* 读取并返回寄存器值 */
  // 转换为32位指针访问，实际返回低12位有效数据
  return (uint16_t) (*(__IO uint32_t*) tmp);  // 示例：return DAC_DOR1
}

/**
  * @brief  控制DAC通道的DMA请求功能
  * @details 该函数启用/禁用指定通道的DMA传输功能。当启用时，DAC在外部触发事件
  *         发生时自动发起DMA请求，用于波形发生器等高数据吞吐量场景。
  * @warning 使用前必须完成以下配置：
  *          - 通道1：DMA1 Stream5（通道7）已配置
  *          - 通道2：DMA1 Stream6（通道7）已配置
  * @param  DAC_Channel: 目标通道
  *         参数取值范围同DAC_GetDataOutputValue
  * @param  NewState: DMA请求状态
  *          参数取值：
  *            - ENABLE: 允许自动DMA请求
  *            - DISABLE: 禁止DMA请求
  * @note 仅支持硬件触发模式（TIMx/EXTI9），软件触发无法产生DMA请求
  */
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState)
{
  /* 双重参数验证 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));    // 验证通道参数
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态参数

  if (NewState != DISABLE)
  {
    /* 启用指定通道的DMA请求功能 */
    // DAC_CR_DMAEN1为DMA使能位掩码（如0x00001000）
    // 通道号作为位偏移量（通道1=0，通道2=1）
    DAC->CR |= (DAC_CR_DMAEN1 << DAC_Channel);  // 示例：设置CR寄存器的DMAEN1位
  }
  else
  {
    /* 禁用DMA请求功能 */
    // 使用位取反和与操作清除对应位
    DAC->CR &= (~(DAC_CR_DMAEN1 << DAC_Channel)); 
  }
}

/**
  * @}
  */

/** @defgroup DAC_Group3 Interrupts and flags management functions
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
  * @brief  Enables or disables the specified DAC interrupts.
  * @param  DAC_Channel: The selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_IT: specifies the DAC interrupt sources to be enabled or disabled. 
  *          This parameter can be the following values:
  *            @arg DAC_IT_DMAUDR: DMA underrun interrupt mask
  * @note   The DMA underrun occurs when a second external trigger arrives before the 
  *         acknowledgement for the first external trigger is received (first request).
  * @param  NewState: new state of the specified DAC interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */ 
/**
  * @brief  配置DAC通道中断使能状态
  * @details 该函数用于启用或禁用指定DAC通道的中断类型，目前仅支持DMA下溢中断
  * @param  DAC_Channel: 目标通道
  *         取值：
  *           - DAC_Channel_1: 通道1
  *           - DAC_Channel_2: 通道2
  * @param  DAC_IT: 中断类型
  *         当前仅支持：
  *           - DAC_IT_DMAUDR: DMA下溢中断
  * @param  NewState: 中断状态
  *           - ENABLE: 允许中断触发
  *           - DISABLE: 禁止中断
  * @note 使用前需配置NVIC中断控制器，建议在DMA配置完成后启用此中断
  */
void DAC_ITConfig(uint32_t DAC_Channel, uint32_t DAC_IT, FunctionalState NewState)  
{
  /* 三重参数验证 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));    // 验证通道参数合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态参数有效性
  assert_param(IS_DAC_IT(DAC_IT));              // 验证中断类型有效性（当前仅支持DMAUDR）

  if (NewState != DISABLE)
  {
    /* 设置控制寄存器(CR)的中断使能位 */
    // 将中断标志左移通道号对应的位偏移量（通道1=0，通道2=1）
    // 例如：DAC_IT_DMAUDR = 0x00000200，通道2操作时变为0x00000400
    DAC->CR |=  (DAC_IT << DAC_Channel);  
  }
  else
  {
    /* 清除控制寄存器(CR)的中断使能位 */
    // 使用位取反和与操作清除指定中断位
    DAC->CR &= (~(uint32_t)(DAC_IT << DAC_Channel));  
  }
}

/**
  * @brief  获取DAC通道状态标志
  * @details 该函数用于检测DMA下溢等异常状态标志位
  * @param  DAC_Channel: 目标通道（取值同前）
  * @param  DAC_FLAG: 状态标志类型
  *         当前仅支持：
  *           - DAC_FLAG_DMAUDR: DMA下溢标志
  * @return 标志状态：
  *           - SET: 标志位有效
  *           - RESET: 标志位无效
  * @note DMA下溢通常因外部触发速率超过DMA传输速度导致
  */
FlagStatus DAC_GetFlagStatus(uint32_t DAC_Channel, uint32_t DAC_FLAG)
{
  FlagStatus bitstatus = RESET;  // 默认状态为未触发
  
  /* 参数双重验证 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));  // 通道有效性检查
  assert_param(IS_DAC_FLAG(DAC_FLAG));        // 标志类型检查（当前仅DMAUDR有效）

  /* 检测状态寄存器(SR)标志位 */
  // 将标志位左移通道号对应的位偏移量（通道1=0，通道2=1）
  // 例如：DAC_FLAG_DMAUDR = 0x00002000，通道2操作时变为0x00004000
  if ((DAC->SR & (DAC_FLAG << DAC_Channel)) != (uint8_t)RESET)
  {
    bitstatus = SET;  // 检测到标志位置位
  }
  else
  {
    bitstatus = RESET; // 标志位未置位
  }
  return  bitstatus;  // 返回标志状态
}

/**
  * @brief  清除DAC通道状态标志
  * @details 清除指定通道的DMA下溢等异常标志
  * @param  DAC_Channel: 目标通道（取值同前）
  * @param  DAC_FLAG: 需清除的标志（当前仅支持DMAUDR）
  * @note 清除标志通过写1完成，需直接操作状态寄存器(SR)
  */
void DAC_ClearFlag(uint32_t DAC_Channel, uint32_t DAC_FLAG)
{
  /* 参数双重验证 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));  // 验证通道参数
  assert_param(IS_DAC_FLAG(DAC_FLAG));        // 验证标志类型

  /* 清除标志位操作 */
  // 将标志位左移通道号对应的位偏移量后写入SR寄存器
  // 例如：写入DAC_FLAG_DMAUDR<<DAC_Channel_2 = 0x00004000
  // 寄存器特性：写1清除对应标志位
  DAC->SR = (DAC_FLAG << DAC_Channel);  
}
/**
  * @brief  获取DAC中断状态（组合状态）
  * @details 该函数同时检测中断标志位和中断使能位状态，用于判断实际触发的中断
  *         需满足两个条件：中断使能位开启 且 中断标志位置位
  * @param  DAC_Channel: 目标通道
  *         取值：
  *           - DAC_Channel_1: 通道1
  *           - DAC_Channel_2: 通道2
  * @param  DAC_IT: 中断类型
  *         当前仅支持：
  *           - DAC_IT_DMAUDR: DMA下溢中断
  * @return 中断状态：
  *           - SET: 有效中断请求
  *           - RESET: 无有效中断
  * @note 该函数用于中断服务程序(ISR)中判断具体中断源
  */
ITStatus DAC_GetITStatus(uint32_t DAC_Channel, uint32_t DAC_IT)
{
  ITStatus bitstatus = RESET;       // 默认状态为无中断
  uint32_t enablestatus = 0;        // 中断使能状态缓存
  
  /* 参数双重验证 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));  // 验证通道有效性
  assert_param(IS_DAC_IT(DAC_IT));            // 验证中断类型合法性

  /* 获取中断使能位状态 */
  // 从控制寄存器(CR)读取中断使能位状态
  // 示例：DAC_IT_DMAUDR=0x00000200，通道2操作时掩码为0x00000400
  enablestatus = (DAC->CR & (DAC_IT << DAC_Channel)) ; 

  /* 组合状态检测 */
  // 同时满足以下条件返回SET：
  // 1. 状态寄存器(SR)标志位置位（硬件触发）
  // 2. 控制寄存器(CR)中断使能位有效（软件使能）
  if (((DAC->SR & (DAC_IT << DAC_Channel)) != (uint32_t)RESET) && enablestatus)
  {
    bitstatus = SET;  // 有效中断请求
  }
  else
  {
    bitstatus = RESET; // 无有效中断
  }
  return  bitstatus;   // 返回复合状态
}

/**
  * @brief  清除DAC中断挂起标志
  * @details 该函数用于清除指定通道的中断标志位，通常在中断服务程序(ISR)末尾调用
  * @param  DAC_Channel: 目标通道（取值同前）
  * @param  DAC_IT: 中断类型（当前仅支持DMAUDR）
  * @note 清除操作通过写1完成，需严格遵循1次清除1个标志的原则
  */
void DAC_ClearITPendingBit(uint32_t DAC_Channel, uint32_t DAC_IT)
{
  /* 参数双重验证 */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));  // 通道有效性检查
  assert_param(IS_DAC_IT(DAC_IT));            // 中断类型检查

  /* 清除中断挂起标志 */
  // 将中断标志左移通道号对应的位偏移量后写入SR寄存器
  // 示例：DAC_IT_DMAUDR=0x00000200，通道2操作时写入0x00004000
  // 寄存器特性：写1清除对应位（实际完成两个操作：检测标志位+清除标志位）
  DAC->SR = (DAC_IT << DAC_Channel);  
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

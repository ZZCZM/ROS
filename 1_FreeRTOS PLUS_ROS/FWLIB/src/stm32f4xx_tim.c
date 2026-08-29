/**
  ******************************************************************************
  * @file    stm32f4xx_tim.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the TIM peripheral:
  *            + TimeBase management
  *            + Output Compare management
  *            + Input Capture management
  *            + Advanced-control timers (TIM1 and TIM8) specific features  
  *            + Interrupts, DMA and flags management
  *            + Clocks management
  *            + Synchronization management
  *            + Specific interface management
  *            + Specific remapping management      
  *              
  @verbatim   
 ===============================================================================
                   #####  How to use this driver #####
 ===============================================================================
    [..]
    This driver provides functions to configure and program the TIM 
    of all STM32F4xx devices.
    These functions are split in 9 groups: 
     
      (#) TIM TimeBase management: this group includes all needed functions 
          to configure the TM Timebase unit:
        (++) Set/Get Prescaler
        (++) Set/Get Autoreload  
        (++) Counter modes configuration
        (++) Set Clock division  
        (++) Select the One Pulse mode
        (++) Update Request Configuration
        (++) Update Disable Configuration
        (++) Auto-Preload Configuration 
        (++) Enable/Disable the counter     
                   
      (#) TIM Output Compare management: this group includes all needed 
          functions to configure the Capture/Compare unit used in Output 
          compare mode: 
        (++) Configure each channel, independently, in Output Compare mode
        (++) Select the output compare modes
        (++) Select the Polarities of each channel
        (++) Set/Get the Capture/Compare register values
        (++) Select the Output Compare Fast mode 
        (++) Select the Output Compare Forced mode  
        (++) Output Compare-Preload Configuration 
        (++) Clear Output Compare Reference
        (++) Select the OCREF Clear signal
        (++) Enable/Disable the Capture/Compare Channels    
                     
      (#) TIM Input Capture management: this group includes all needed 
          functions to configure the Capture/Compare unit used in 
          Input Capture mode:
        (++) Configure each channel in input capture mode
        (++) Configure Channel1/2 in PWM Input mode
        (++) Set the Input Capture Prescaler
        (++) Get the Capture/Compare values      
                     
      (#) Advanced-control timers (TIM1 and TIM8) specific features
        (++) Configures the Break input, dead time, Lock level, the OSSI,
             the OSSR State and the AOE(automatic output enable)
        (++) Enable/Disable the TIM peripheral Main Outputs
        (++) Select the Commutation event
        (++) Set/Reset the Capture Compare Preload Control bit
                                
      (#) TIM interrupts, DMA and flags management
        (++) Enable/Disable interrupt sources
        (++) Get flags status
        (++) Clear flags/ Pending bits
        (++) Enable/Disable DMA requests 
        (++) Configure DMA burst mode
        (++) Select CaptureCompare DMA request  
                
      (#) TIM clocks management: this group includes all needed functions 
          to configure the clock controller unit:
        (++) Select internal/External clock
        (++) Select the external clock mode: ETR(Mode1/Mode2), TIx or ITRx
           
      (#) TIM synchronization management: this group includes all needed 
          functions to configure the Synchronization unit:
        (++) Select Input Trigger  
        (++) Select Output Trigger  
        (++) Select Master Slave Mode 
        (++) ETR Configuration when used as external trigger   
       
      (#) TIM specific interface management, this group includes all 
          needed functions to use the specific TIM interface:
        (++) Encoder Interface Configuration
        (++) Select Hall Sensor   
           
      (#) TIM specific remapping management includes the Remapping 
          configuration of specific timers               
     
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
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup TIM 
  * @brief TIM driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* ---------------------- TIM寄存器位掩码定义 ------------------------ */
/* SMCR寄存器外部触发(ETR)配置位掩码（低8位有效） */
#define SMCR_ETR_MASK      ((uint16_t)0x00FF)  // 用于配置ETR预分频器、极性等参数

/* 捕获/比较模式寄存器(CCMR)偏移量 */
#define CCMR_OFFSET        ((uint16_t)0x0018)   // TIMx_CCMR1和CCMR2的地址偏移量

/* 捕获/比较使能寄存器(CCER)位配置 */
#define CCER_CCE_SET       ((uint16_t)0x0001)   // 通道使能位掩码（例：CC1E）
#define CCER_CCNE_SET      ((uint16_t)0x0004)   // 互补通道使能位掩码（例：CC1NE）

/* 输出比较模式位掩码 */
#define CCMR_OC13M_MASK    ((uint16_t)0xFF8F)   // 通道1/3输出模式配置位掩码（清除OCxM位）
#define CCMR_OC24M_MASK    ((uint16_t)0x8FFF)   // 通道2/4输出模式配置位掩码

/* 私有函数声明 ----------------------------------------------------*/
/**
  * @brief  TIM输入通道1配置函数（内部使用）
  * @param  TIMx: TIM实例
  * @param  TIM_ICPolarity: 输入极性（上升沿/下降沿/双沿）
  * @param  TIM_ICSelection: 输入映射方式（直连/交叉/TRC）
  * @param  TIM_ICFilter: 输入滤波器配置（0x0-0xF） 
  */
static void TI1_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, 
                      uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);

/**
  * @brief  TIM输入通道2配置函数（内部使用）
  * 参数说明同TI1_Config
  */
static void TI2_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity,
                      uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);

/**
  * @brief  TIM输入通道3配置函数（内部使用）
  * 参数说明同TI1_Config  
  */
static void TI3_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity,
                      uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);

/**
  * @brief  TIM输入通道4配置函数（内部使用）
  * 参数说明同TI1_Config
  */
static void TI4_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity,
                      uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);

/* Private functions ---------------------------------------------------------*/

/** @defgroup TIM_Private_Functions
  * @{
  */

/** @defgroup TIM_Group1 TimeBase management functions
 *  @brief   TimeBase management functions 
 *
@verbatim   
 ===============================================================================
                     ##### TimeBase management functions #####
 ===============================================================================  
  
     
            ##### TIM Driver: how to use it in Timing(Time base) Mode #####
 ===============================================================================
    [..] 
    To use the Timer in Timing(Time base) mode, the following steps are mandatory:
       
      (#) Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function
                    
      (#) Fill the TIM_TimeBaseInitStruct with the desired parameters.
       
      (#) Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
          with the corresponding configuration
          
      (#) Enable the NVIC if you need to generate the update interrupt. 
          
      (#) Enable the corresponding interrupt using the function TIM_ITConfig(TIMx, TIM_IT_Update) 
       
      (#) Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
             
       -@- All other functions can be used separately to modify, if needed,
           a specific feature of the Timer. 

@endverbatim
  * @{
  */
  
/**
  * @brief  Deinitializes the TIMx peripheral registers to their default reset values.
  * @param  TIMx: where x can be 1 to 14 to select the TIM peripheral.
  * @retval None

  */
/**
  * @brief  复位指定的TIM外设寄存器到默认初始值
  * @param  TIMx: 定时器外设指针，取值范围TIM1-TIM14
  * @retval 无
  * 
  * @功能说明：
  * 本函数通过复位对应TIM外设的APB总线时钟来实现寄存器复位。
  * 采用先使能外设复位，再立即关闭复位的方式完成复位操作。
  * 根据不同的TIM实例选择对应的APB总线：
  * - APB2总线：TIM1, TIM8, TIM9, TIM10, TIM11
  * - APB1总线：TIM2-TIM7, TIM12-TIM14
  * 
  * @实现机制：
  * 1. 使用断言检查TIMx参数的有效性
  * 2. 通过条件判断选择对应的APB总线
  * 3. 依次执行使能复位和禁用复位操作
  */
void TIM_DeInit(TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  /* 参数有效性验证，使用标准库的断言机制 */
  assert_param(IS_TIM_ALL_PERIPH(TIMx)); 

  /* 根据不同的定时器实例选择对应的复位控制总线 */
  if (TIMx == TIM1)
  {
    /* APB2总线上的TIM1复位 */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, ENABLE);  /* 启动TIM1外设复位 */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, DISABLE); /* 结束TIM1外设复位 */
  } 
  else if (TIMx == TIM2) 
  {     
    /* APB1总线上的TIM2复位 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);
  }  
  /* 后续每个else if分支结构相似，处理不同TIM实例的复位操作 */
  else if (TIMx == TIM3)
  { 
    /* APB1总线上的TIM3复位 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);
  }  
  // ... 中间类似分支省略...
  else if (TIMx == TIM11) 
  {     
    /* APB2总线上的TIM11复位 */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM11, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM11, DISABLE);  
  }  
  else if (TIMx == TIM12)
  {      
    /* APB1总线上的TIM12复位 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM12, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM12, DISABLE);  
  }  
  else if (TIMx == TIM13) 
  {       
    /* APB1总线上的TIM13复位 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM13, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM13, DISABLE);  
  }  
  else  /* 处理未明确列出的剩余情况 */
  { 
    /* 特别处理TIM14的情况 */
    if (TIMx == TIM14) 
    {     
      /* APB1总线上的TIM14复位 */
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM14, ENABLE);
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM14, DISABLE); 
    }   
    /* 注：此处else结构设计用于可能的未来扩展，
       当前仅处理TIM14，其他无效参数不会执行操作 */
  }
}
/**
  * @brief  初始化TIM定时器时基单元
  * @param  TIMx: 定时器外设指针，范围TIM1-TIM14
  * @param  TIM_TimeBaseInitStruct: 时基配置结构体指针，包含以下配置项：
  *         - TIM_CounterMode: 计数器模式（上/下/中央对齐）
  *         - TIM_ClockDivision: 时钟分频因子
  *         - TIM_Period: 自动重装载值（ARR）
  *         - TIM_Prescaler: 预分频值（PSC）
  *         - TIM_RepetitionCounter: 重复计数器值（仅高级定时器有效）
  * @retval 无
  *
  * @功能说明：
  * 本函数完成定时器的核心配置，包含五部分操作：
  * 1. 配置计数器工作模式（CR1寄存器）
  * 2. 设置时钟分频（CR1寄存器）
  * 3. 写入自动重载值（ARR寄存器）
  * 4. 设置预分频值（PSC寄存器）
  * 5. 配置重复计数器（高级定时器RCR寄存器）
  * 6. 触发立即更新事件（EGR寄存器）
  *
  * @特别说明：
  * - 基本定时器（TIM6/7）不支持计数器模式配置
  * - 重复计数器仅TIM1/TIM8有效
  */
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
  uint16_t tmpcr1 = 0;  // 临时寄存器，用于批量修改CR1设置

  /* Check the parameters */
  /* 三层参数验证确保配置安全 */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));  // 验证定时器实例有效性
  assert_param(IS_TIM_COUNTER_MODE(TIM_TimeBaseInitStruct->TIM_CounterMode));  // 验证计数模式
  assert_param(IS_TIM_CKD_DIV(TIM_TimeBaseInitStruct->TIM_ClockDivision));  // 验证时钟分频值

  tmpcr1 = TIMx->CR1;  // 获取当前CR1寄存器值

  /* 处理计数器模式配置 */
  /* 仅适用于通用/高级定时器（TIM1-5,8），基本定时器无此功能 */
  if((TIMx == TIM1) || (TIMx == TIM8)||
     (TIMx == TIM2) || (TIMx == TIM3)||
     (TIMx == TIM4) || (TIMx == TIM5)) 
  {
    /* Select the Counter Mode */
    /* 二进制操作步骤：
       1. 清除方向位(DIR)和中央对齐模式选择位(CMS)
       2. 写入新的计数模式 */
    tmpcr1 &= (uint16_t)(~(TIM_CR1_DIR | TIM_CR1_CMS)); // 清空原模式位
    tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_CounterMode; // 置入新模式
  }
 
  /* 配置时钟分频参数 */
  /* 基本定时器（TIM6/7）不包含时钟分频功能 */
  if((TIMx != TIM6) && (TIMx != TIM7))
  {
    /* Set the clock division */
    /* 操作原理：
       - CKD[1:0]位控制采样频率
       - 清空原分频设置后写入新值 */
    tmpcr1 &=  (uint16_t)(~TIM_CR1_CKD);  // 清除原分频设置
    tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_ClockDivision; // 设置新分频
  }

  TIMx->CR1 = tmpcr1;  // 将修改后的配置写回CR1寄存器

  /* 配置自动重载寄存器 */
  /* ARR值决定定时周期，与PSC配合计算公式：
     定时周期 = (ARR+1)*(PSC+1)/定时器时钟频率 */
  TIMx->ARR = TIM_TimeBaseInitStruct->TIM_Period;  // 写入自动重载值
 
  /* 配置预分频寄存器 */
  /* PSC实现时钟分频，有效分频系数= PSC+1 */
  TIMx->PSC = TIM_TimeBaseInitStruct->TIM_Prescaler;  // 设置预分频系数
    
  /* 配置重复计数器（RCR） */
  /* 高级定时器特有功能，用于控制PWM重复更新次数 */
  if ((TIMx == TIM1) || (TIMx == TIM8))  // 仅处理高级定时器
  {
    /* Set the Repetition Counter value */
    TIMx->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;  // 写入重复计数
  }

  /* Generate an update event to reload the Prescaler 
     and the repetition counter(only for TIM1 and TIM8) value immediatly */
  /* 立即更新触发机制：
     通过写EGR寄存器的UG位，实现参数立即生效
     注意：该操作会生成更新事件，可能触发中断/DMA */
  TIMx->EGR = TIM_PSCReloadMode_Immediate;  // 强制立即更新
} 
/**
  * @brief  初始化时基配置结构体为默认值
  * @param  TIM_TimeBaseInitStruct: 时基配置结构体指针
  * @retval 无
  *
  * @默认配置说明：
  * - TIM_Period: 最大32位周期值（0xFFFFFFFF）
  * - TIM_Prescaler: 无预分频（0x0000）
  * - TIM_ClockDivision: 时钟不分频（DIV1）
  * - TIM_CounterMode: 上数模式
  * - TIM_RepetitionCounter: 禁用重复计数（0）
  *
  * @应用场景：
  * 通常在初始化定时器配置前调用，确保结构体字段处于已知状态
  */
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
  /* 初始化结构体为安全默认值 */
  TIM_TimeBaseInitStruct->TIM_Period = 0xFFFFFFFF;       // ARR最大值，避免意外触发
  TIM_TimeBaseInitStruct->TIM_Prescaler = 0x0000;         // 1:1分频，保持原频率
  TIM_TimeBaseInitStruct->TIM_ClockDivision = TIM_CKD_DIV1; // 输入捕获滤波器无分频
  TIM_TimeBaseInitStruct->TIM_CounterMode = TIM_CounterMode_Up; // 默认上数模式
  TIM_TimeBaseInitStruct->TIM_RepetitionCounter = 0x0000; // 禁用重复计数（高级定时器）
}

/**
  * @brief  配置定时器预分频器及重载模式
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @param  Prescaler: 预分频系数（实际分频=PSC+1）
  * @param  TIM_PSCReloadMode: 预分频重载模式
  *         TIM_PSCReloadMode_Update: 延迟更新（下次溢出时生效）
  *         TIM_PSCReloadMode_Immediate: 立即生效
  * @retval 无
  *
  * @操作原理：
  * 1. 写入预分频缓冲区（PSC影子寄存器）
  * 2. 通过EGR寄存器决定立即生效或延迟生效
  */
void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));  // 验证定时器合法性
  assert_param(IS_TIM_PRESCALER_RELOAD(TIM_PSCReloadMode)); // 模式有效性检查

  /* 分两步更新预分频器配置 */
  TIMx->PSC = Prescaler;   // 设置预分频影子寄存器
  TIMx->EGR = TIM_PSCReloadMode; // 触发更新事件选择立即/延迟生效
}

/**
  * @brief  配置定时器计数模式
  * @param  TIMx: 支持模式的定时器（TIM1,2,3,4,5,8）
  * @param  TIM_CounterMode: 计数模式枚举值
  * @retval 无
  *
  * @原理说明：
  * - 通过CR1寄存器的DIR(方向位)和CMS(中央对齐模式选择位)组合实现
  * - 操作流程：读-改-写操作保证其他配置位不受影响
  */
void TIM_CounterModeConfig(TIM_TypeDef* TIMx, uint16_t TIM_CounterMode)
{
  uint16_t tmpcr1 = 0;

  /* 双重验证机制确保参数合法 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));  // 检测支持的定时器范围
  assert_param(IS_TIM_COUNTER_MODE(TIM_CounterMode)); // 校验模式有效性

  tmpcr1 = TIMx->CR1;  // 暂存当前CR1配置

  /* 清除相关配置位 */
  tmpcr1 &= (uint16_t)~(TIM_CR1_DIR | TIM_CR1_CMS); // DIR=0,CMS=00（默认上数模式）

  /* 设置新工作模式 */
  tmpcr1 |= TIM_CounterMode;  // 注入目标模式位

  TIMx->CR1 = tmpcr1;  // 写入更新后的CR1值
}

/**
  * @brief  设置定时器计数器当前值
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @param  Counter: 新的计数值（32位范围）
  * @retval 无
  *
  * @注意：
  * - 直接写入CNT寄存器可能触发UEV事件
  * - 运行时修改需要同步考虑ARR/PSC配置
  */
void TIM_SetCounter(TIM_TypeDef* TIMx, uint32_t Counter)
{
  assert_param(IS_TIM_ALL_PERIPH(TIMx)); // 参数有效性验证
  
  /* 原子操作直接写入计数寄存器 */
  TIMx->CNT = Counter;  // 重置计数器为指定值
}

/**
  * @brief  设置自动重载值
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @param  Autoreload: 新的ARR值（32位范围）
  * @retval 无
  *
  * @特别说明：
  * - 实际生效取决ARR预装载使能状态
  * - 配合预装载功能可实现周期同步更新
  */
void TIM_SetAutoreload(TIM_TypeDef* TIMx, uint32_t Autoreload)
{
  assert_param(IS_TIM_ALL_PERIPH(TIMx));  // 设备实例验证
  
  TIMx->ARR = Autoreload;  // 写入新的定时周期值
}

/**
  * @brief  获取当前计数器值
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @retval 当前CNT寄存器值
  *
  * @使用注意：
  * - 在运行中读取需考虑值可能正被更新
  * - 建议搭配捕获/比较功能进行精确时间测量
  */
uint32_t TIM_GetCounter(TIM_TypeDef* TIMx)
{
  assert_param(IS_TIM_ALL_PERIPH(TIMx)); // 设备有效性检查
  
  return TIMx->CNT;  // 直接返回计数器瞬时值
}
/**
  * @brief  获取定时器当前预分频系数
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @retval PSC寄存器当前值（0x0000-0xFFFF）
  *
  * @特别注意：
  * - 返回值需+1得到实际分频系数（实际分频比 = PSC+1）
  * - 返回的是影子寄存器的缓存值，并非当前运行中的寄存器值
  */
uint16_t TIM_GetPrescaler(TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));  // 设备合法性校验
  
  return TIMx->PSC;  // 直接返回预分频寄存器值
}

/**
  * @brief  使能/禁用定时器更新事件
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @param  NewState: 新状态（ENABLE-禁用更新，DISABLE-允许更新）
  * @retval 无
  *
  * @控制原理：
  * - UDIS位=1: 更新事件被禁止，仍然可以触发中断/DMA请求
  * - UDIS位=0: 允许更新事件生成
  * - 主要应用场景：用于多参数同步更新的场合
  */
void TIM_UpdateDisableConfig(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* 双重安全检查 */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 设置更新禁止位 */
    TIMx->CR1 |= TIM_CR1_UDIS;  // 通过位或操作置位UDIS位
    /* 此时：计数器仍继续运行但UEV事件被禁止 */
  }
  else
  {
    /* 清除更新禁止位 */
    TIMx->CR1 &= (uint16_t)~TIM_CR1_UDIS; // 位与操作清除UDIS位
    /* 重置后：允许产生UEV事件 */
  }
}

/**
  * @brief  配置定时器更新事件触发源
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @param  TIM_UpdateSource: 触发源类型
  *         TIM_UpdateSource_Global: 所有更新源（溢出/UG位/从模式触发）
  *         TIM_UpdateSource_Regular: 仅计数器溢出触发
  * @retval 无
  *
  * @功能说明：
  * URS位控制策略：
  * - 当URS=0时，任何触发源都可以产生更新中断
  * - 当URS=1时，只有计数器溢出才会产生中断
  */
void TIM_UpdateRequestConfig(TIM_TypeDef* TIMx, uint16_t TIM_UpdateSource)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_UPDATE_SOURCE(TIM_UpdateSource));

  if (TIM_UpdateSource != TIM_UpdateSource_Global)
  {
    /* 配置为仅溢出触发模式 */
    TIMx->CR1 |= TIM_CR1_URS;  // 置位URS位
    /* 此时：UG位写操作不会触发更新事件 */
  }
  else
  {
    /* 允许全局触发源 */
    TIMx->CR1 &= (uint16_t)~TIM_CR1_URS; // URS位清零
    /* 可触发事件类型包括：
       - 计数器溢出/欠压
       - 手动设置UG位
       - 从模式控制器触发 */
  }
}

/**
  * @brief  配置自动重载预装载功能
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @param  NewState: 预装载使能状态（ENABLE/DISABLE）
  * @retval 无
  *
  * @ARPE功能说明：
  * - 启用时（ARPE=1）：ARR修改写入预装载寄存器，在下个周期生效
  * - 禁用时（ARPE=0）：ARR直接写入活动寄存器，立即生效
  * - 推荐运行时修改ARR值时启用该功能以保证时序稳定性
  */
void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 启用ARR预装载缓冲 */
    TIMx->CR1 |= TIM_CR1_ARPE;  // 设置ARPE位
    /* 此时修改ARR不会立即影响当前计数周期 */
  }
  else
  {
    /* 禁用预装载缓冲 */
    TIMx->CR1 &= (uint16_t)~TIM_CR1_ARPE; // 清除ARPE位
    /* ARR修改将直接作用于当前周期 */
  }
}

/**
  * @brief  配置定时器单脉冲模式
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @param  TIM_OPMode: 单脉冲模式选项
  *         TIM_OPMode_Single: 单次模式（计数到ARR后停止）
  *         TIM_OPMode_Repetitive: 重复模式（持续运行）
  * @retval 无
  *
  * @实现原理：
  * 通过CR1寄存器的OPM位控制模式：
  * - OPM=1: 计数器到达ARR值时自动停止
  * - OPM=0: 正常连续工作模式
  * 
  * @典型应用：
  * 用于生成精确的单个脉冲信号，配合触发模式实现外部事件激活
  */
void TIM_SelectOnePulseMode(TIM_TypeDef* TIMx, uint16_t TIM_OPMode)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));  // 验证定时器有效性
  assert_param(IS_TIM_OPM_MODE(TIM_OPMode)); // 检查模式参数合法性

  /* 清除原有OPM配置 */
  TIMx->CR1 &= (uint16_t)~TIM_CR1_OPM;  // CR1[3]位清零

  /* 注入新模式配置 */
  TIMx->CR1 |= TIM_OPMode;  // 设置CR1[3]位状态
  /* 注：实际OPM位仅CR1[3]一位，模式参数应为0或0x0008 */
}

/**
  * @brief  设置定时器时钟分频系数
  * @param  TIMx: 定时器实例（TIM1-5,8-14）
  * @param  TIM_CKD: 时钟分频系数
  *         TIM_CKD_DIV1: 采样时钟=定时器时钟（默认）
  *         TIM_CKD_DIV2: 采样时钟=定时器时钟/2 
  *         TIM_CKD_DIV4: 采样时钟=定时器时钟/4
  * @retval 无
  *
  * @功能说明：
  * 该配置影响输入捕获的数字滤波采样频率
  * 分频后的TDTS时钟用于输入捕获噪声抑制
  *
  * @硬件限制：
  * 基本定时器TIM6/7无此功能
  */
void TIM_SetClockDivision(TIM_TypeDef* TIMx, uint16_t TIM_CKD)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));  // 排除TIM6/7
  assert_param(IS_TIM_CKD_DIV(TIM_CKD));    // 校验分频系数

  /* 清除CKD位域（CR1[8:9]） */
  TIMx->CR1 &= (uint16_t)(~TIM_CR1_CKD);  // 二进制掩码操作

  /* 设置新的时钟分频 */
  TIMx->CR1 |= TIM_CKD;  // 写入CR1[9:8]位
  /* 实际生效延迟：取决于CR1寄存器的写入时序 */
}

/**
  * @brief  使能/禁用定时器计数器
  * @param  TIMx: 定时器实例（TIM1-TIM14）
  * @param  NewState: 定时器工作状态（ENABLE-启动，DISABLE-停止）
  * @retval 无
  *
  * @控制逻辑：
  * - 使能时：立即开始计数，CNT值从当前值开始变化
  * - 禁用时：停止计数，保持当前CNT值
  * - 状态切换不影响其他配置（ARR/PSC等保持原值）
  *
  * @注意事项：
  * 重复调用使能不会重置计数器，需配合TIM_SetCounter使用
  */
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));  // 校验设备实例
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 状态参数检查
  
  if (NewState != DISABLE)
  {
    /* 启动定时器计数器 */
    TIMx->CR1 |= TIM_CR1_CEN;  // 设置CR1[0]位
    /* 计数器从当前CNT值开始递增/递减 */
  }
  else
  {
    /* 停止定时器计数器 */
    TIMx->CR1 &= (uint16_t)~TIM_CR1_CEN; // 清除CR1[0]位
    /* CNT值保持最后状态，外设时钟保持供电 */
  }
}

/**
  * @}
  */

/** @defgroup TIM_Group2 Output Compare management functions
 *  @brief    Output Compare management functions 
 *
@verbatim   
 ===============================================================================
              ##### Output Compare management functions #####
 ===============================================================================  
   
      
        ##### TIM Driver: how to use it in Output Compare Mode #####
 ===============================================================================
    [..] 
    To use the Timer in Output Compare mode, the following steps are mandatory:
       
      (#) Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) 
          function
       
      (#) Configure the TIM pins by configuring the corresponding GPIO pins
       
      (#) Configure the Time base unit as described in the first part of this driver, 
        (++) if needed, else the Timer will run with the default configuration:
            Autoreload value = 0xFFFF
        (++) Prescaler value = 0x0000
        (++) Counter mode = Up counting
        (++) Clock Division = TIM_CKD_DIV1
          
      (#) Fill the TIM_OCInitStruct with the desired parameters including:
        (++) The TIM Output Compare mode: TIM_OCMode
        (++) TIM Output State: TIM_OutputState
        (++) TIM Pulse value: TIM_Pulse
        (++) TIM Output Compare Polarity : TIM_OCPolarity
       
      (#) Call TIM_OCxInit(TIMx, &TIM_OCInitStruct) to configure the desired 
          channel with the corresponding configuration
       
      (#) Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
       
      -@- All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 
          
      -@- In case of PWM mode, this function is mandatory:
          TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_ENABLE); 
              
      -@- If the corresponding interrupt or DMA request are needed, the user should:
        (+@) Enable the NVIC (or the DMA) to use the TIM interrupts (or DMA requests). 
        (+@) Enable the corresponding interrupt (or DMA request) using the function 
             TIM_ITConfig(TIMx, TIM_IT_CCx) (or TIM_DMA_Cmd(TIMx, TIM_DMA_CCx))   

@endverbatim
  * @{
  */
/**
  * @brief  初始化定时器输出比较通道1
  * @param  TIMx: 定时器实例（TIM1-TIM5,TIM8-TIM14）
  * @param  TIM_OCInitStruct: 输出比较配置结构体指针，包含：
  *         - TIM_OCMode: 输出比较模式（PWM/翻转/强制等）
  *         - TIM_OutputState: 主输出使能状态
  *         - TIM_OCPolarity: 输出极性
  *         - TIM_Pulse: 比较匹配值（CCR1寄存器值）
  *         - 高级定时器特有参数：互补通道配置
  * @retval 无
  *
  * @实现流程：
  * 1. 禁用通道防止配置过程中误触发
  * 2. 配置通道模式寄存器（CCMR1）
  * 3. 配置通道使能/极性寄存器（CCER）
  * 4. 高级定时器特有配置（互补通道/刹车功能）
  * 5. 写入比较寄存器（CCR1）
  *
  * @注意：
  * - 基本定时器（TIM6/7）不支持此功能
  * - 高级定时器需要额外配置刹车和死区时间
  */
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* 参数四层校验确保配置安全 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));  // 排除TIM6/7
  assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));       // 校验8种OC模式
  assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState)); // 输出使能状态
  assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));   // 极性校验

  /* 安全禁用通道输出 */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC1E; // 清除CC1E位（bit0）
  
  /* 获取当前寄存器状态（读-改-写模式） */
  tmpccer = TIMx->CCER;  // 暂存CCER（通道使能/极性）
  tmpcr2 =  TIMx->CR2;   // 暂存CR2（主模式/输出空闲状态）
  tmpccmrx = TIMx->CCMR1; // 暂存CCMR1（通道模式/预装载）
    
  /* 配置CCMR1寄存器（通道模式部分） */
  tmpccmrx &= (uint16_t)~TIM_CCMR1_OC1M;  // 清除OC1M[2:0]位（bit6-4）
  tmpccmrx &= (uint16_t)~TIM_CCMR1_CC1S;  // 清除CC1S[1:0]位（bit1-0）
  tmpccmrx |= TIM_OCInitStruct->TIM_OCMode; // 设置输出比较模式（如PWM模式1）

  /* 配置CCER寄存器（输出极性） */
  tmpccer &= (uint16_t)~TIM_CCER_CC1P;    // 清除CC1P位（bit1）
  tmpccer |= TIM_OCInitStruct->TIM_OCPolarity; // 设置输出极性（高有效/低有效）

  /* 配置输出使能状态 */
  tmpccer |= TIM_OCInitStruct->TIM_OutputState; // 设置CC1E位（使能输出）

  /* 高级定时器（TIM1/TIM8）互补通道配置 */
  if((TIMx == TIM1) || (TIMx == TIM8))  // 检测是否高级定时器
  {
    /* 互补通道参数校验 */
    assert_param(IS_TIM_OUTPUTN_STATE(TIM_OCInitStruct->TIM_OutputNState));
    assert_param(IS_TIM_OCN_POLARITY(TIM_OCInitStruct->TIM_OCNPolarity));
    assert_param(IS_TIM_OCNIDLE_STATE(TIM_OCInitStruct->TIM_OCNIdleState));
    assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));
    
    /* 配置互补通道极性（CC1NP位） */
    tmpccer &= (uint16_t)~TIM_CCER_CC1NP;  // 清除CC1NP位（bit3）
    tmpccer |= TIM_OCInitStruct->TIM_OCNPolarity;  // 设置互补通道极性
    
    /* 配置互补通道使能（CC1NE位） */
    tmpccer &= (uint16_t)~TIM_CCER_CC1NE;  // 清除CC1NE位（bit2）
    tmpccer |= TIM_OCInitStruct->TIM_OutputNState; // 使能互补输出
    
    /* 配置空闲状态（CR2寄存器） */
    tmpcr2 &= (uint16_t)~TIM_CR2_OIS1;    // 清除OIS1位（bit8）
    tmpcr2 &= (uint16_t)~TIM_CR2_OIS1N;   // 清除OIS1N位（bit9）
    tmpcr2 |= TIM_OCInitStruct->TIM_OCIdleState;  // 主通道空闲状态
    tmpcr2 |= TIM_OCInitStruct->TIM_OCNIdleState; // 互补通道空闲状态
  }

  /* 批量写入寄存器（保证配置原子性） */
  TIMx->CR2 = tmpcr2;    // 先写CR2（高级定时器特性配置）
  TIMx->CCMR1 = tmpccmrx; // 写通道模式寄存器
  TIMx->CCR1 = TIM_OCInitStruct->TIM_Pulse; // 设置比较值（占空比计算依据）
  TIMx->CCER = tmpccer;  // 最后写CCER（使能输出）
}

/**
  * @brief  初始化定时器输出比较通道2
  * @param  TIMx: 定时器实例（TIM1-5,8,9,12）
  * @param  TIM_OCInitStruct: 输出比较配置结构体指针，包含：
  *         - TIM_OCMode: 输出比较模式（PWM/翻转/强制等）
  *         - TIM_OutputState: 主输出使能状态
  *         - TIM_OCPolarity: 输出极性
  *         - TIM_Pulse: 比较匹配值（CCR2寄存器值）
  *         - 高级定时器特有参数：互补通道配置
  * @retval 无
  *
  * @实现要点：
  * 1. 通道2使用CCMR1寄存器的高8位配置
  * 2. 所有通道相关位需进行4位偏移操作（相比通道1）
  * 3. 高级定时器需配置互补通道的刹车和死区
  *
  * @硬件差异：
  * - TIM9/12等基础型号无互补通道功能
  */
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* 四层参数校验体系 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));  // 校验支持通道2的定时器
  assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
  assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

  /* 安全关闭通道输出 */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC2E; // 清除CC2E位（bit4）

  /* 获取寄存器当前状态 */
  tmpccer = TIMx->CCER;  // 暂存CCER（通道使能/极性）
  tmpcr2 =  TIMx->CR2;   // 暂存CR2（主模式/空闲状态）
  tmpccmrx = TIMx->CCMR1; // 暂存CCMR1（通道模式）

  /* 配置CCMR1寄存器（通道2部分） */
  tmpccmrx &= (uint16_t)~TIM_CCMR1_OC2M;  // 清除OC2M[10:8]位（bit10-8）
  tmpccmrx &= (uint16_t)~TIM_CCMR1_CC2S;  // 清除CC2S[9:8]位（bit9-8）
  tmpccmrx |= (uint16_t)(TIM_OCInitStruct->TIM_OCMode << 8); // 模式左移8位对齐

  /* 配置CCER寄存器（通道2极性） */
  tmpccer &= (uint16_t)~TIM_CCER_CC2P;    // 清除CC2P位（bit5）
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 4); // 极性左移4位

  /* 配置输出使能状态 */
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 4); // 使能位左移4位

  /* 高级定时器特殊配置（TIM1/TIM8） */
  if((TIMx == TIM1) || (TIMx == TIM8)) 
  {
    /* 互补通道参数校验 */
    assert_param(IS_TIM_OUTPUTN_STATE(TIM_OCInitStruct->TIM_OutputNState));
    assert_param(IS_TIM_OCN_POLARITY(TIM_OCInitStruct->TIM_OCNPolarity));
    assert_param(IS_TIM_OCNIDLE_STATE(TIM_OCInitStruct->TIM_OCNIdleState));
    assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));

    /* 配置互补通道极性（CC2NP位=bit7） */
    tmpccer &= (uint16_t)~TIM_CCER_CC2NP;
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCNPolarity << 4); // 左移4位对齐
    
    /* 配置互补通道使能（CC2NE位=bit6） */
    tmpccer &= (uint16_t)~TIM_CCER_CC2NE;
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputNState << 4); // 左移4位
    
    /* 配置空闲状态（CR2寄存器） */
    tmpcr2 &= (uint16_t)~TIM_CR2_OIS2;   // 清除OIS2（bit10）
    tmpcr2 &= (uint16_t)~TIM_CR2_OIS2N;  // 清除OIS2N（bit11）
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 2); // 左移2位对齐
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCNIdleState << 2); // 左移2位
  }

  /* 原子化写入配置 */
  TIMx->CR2 = tmpcr2;     // 先写CR2（高级功能配置）
  TIMx->CCMR1 = tmpccmrx; // 写通道模式寄存器
  TIMx->CCR2 = TIM_OCInitStruct->TIM_Pulse; // 设置比较值（占空比基数）
  TIMx->CCER = tmpccer;   // 最后使能输出（避免中间状态）
}
/**
  * @brief  初始化定时器输出比较通道3
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_OCInitStruct: 输出比较配置结构体指针，包含：
  *         - TIM_OCMode: 输出比较模式（PWM/翻转/强制等）
  *         - TIM_OutputState: 主输出使能状态
  *         - TIM_OCPolarity: 输出极性
  *         - TIM_Pulse: 比较匹配值（CCR3寄存器值）
  *         - 高级定时器特有参数：互补通道配置
  * @retval 无
  *
  * @核心差异：
  * 1. 使用CCMR2寄存器配置通道3（bit0-7:OC3FE/OC3PE, bit8-15:OC3M/CC3S）
  * 2. CCER寄存器通道3控制位偏移8位（CC3E=bit8, CC3P=bit9, CC3NP=bit11）
  * 3. 高级定时器空闲状态位在CR2寄存器的bit12-13
  */
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* 五层安全校验机制 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));  // 校验支持通道3的定时器
  assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
  assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

  /* 禁用通道防止配置干扰 */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC3E; // 清除CC3E位（bit8）

  /* 获取寄存器当前状态 */
  tmpccer = TIMx->CCER;   // 暂存CCER（通道使能/极性）
  tmpcr2 = TIMx->CR2;     // 暂存CR2（空闲状态配置）
  tmpccmrx = TIMx->CCMR2; // 暂存CCMR2（通道3模式）

  /* 配置CCMR2寄存器（通道3模式） */
  tmpccmrx &= (uint16_t)~TIM_CCMR2_OC3M;  // 清除OC3M[2:0]位（bit6-4）
  tmpccmrx &= (uint16_t)~TIM_CCMR2_CC3S;  // 清除CC3S[1:0]位（bit1-0）
  tmpccmrx |= TIM_OCInitStruct->TIM_OCMode; // 直接写入模式（无需位移）

  /* 配置CCER寄存器（通道3特性） */
  tmpccer &= (uint16_t)~TIM_CCER_CC3P;    // 清除CC3P位（bit9）
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 8); // 左移8位对齐

  /* 配置输出使能状态 */
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 8); // 左移8位

  /* 高级定时器互补通道配置 */
  if((TIMx == TIM1) || (TIMx == TIM8)) 
  {
    /* 互补通道参数四重校验 */
    assert_param(IS_TIM_OUTPUTN_STATE(TIM_OCInitStruct->TIM_OutputNState));
    assert_param(IS_TIM_OCN_POLARITY(TIM_OCInitStruct->TIM_OCNPolarity));
    assert_param(IS_TIM_OCNIDLE_STATE(TIM_OCInitStruct->TIM_OCNIdleState));
    assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));

    /* 配置互补通道极性（CC3NP=bit11） */
    tmpccer &= (uint16_t)~TIM_CCER_CC3NP;
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCNPolarity << 8); // 左移8位
    
    /* 配置互补通道使能（CC3NE=bit10） */
    tmpccer &= (uint16_t)~TIM_CCER_CC3NE;
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputNState << 8); // 左移8位
    
    /* 配置空闲状态（CR2寄存器bit12-13） */
    tmpcr2 &= (uint16_t)~TIM_CR2_OIS3;   // 清除OIS3（bit12）
    tmpcr2 &= (uint16_t)~TIM_CR2_OIS3N;  // 清除OIS3N（bit13）
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 4);  // 左移4位对齐
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCNIdleState << 4); // 左移4位
  }

  /* 批量写入顺序优化 */
  TIMx->CR2 = tmpcr2;     // 先写CR2（高级功能配置）
  TIMx->CCMR2 = tmpccmrx; // 写通道模式寄存器
  TIMx->CCR3 = TIM_OCInitStruct->TIM_Pulse; // 设置比较值（占空比基数）
  TIMx->CCER = tmpccer;   // 最后使能输出（避免毛刺）
}
/**
  * @brief  初始化定时器输出比较通道4
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_OCInitStruct: 输出比较配置结构体指针，包含：
  *         - TIM_OCMode: 输出比较模式（PWM/翻转/强制等）
  *         - TIM_OutputState: 主输出使能状态
  *         - TIM_OCPolarity: 输出极性
  *         - TIM_Pulse: 比较匹配值（CCR4寄存器值）
  *         - 高级定时器特有参数：空闲状态配置
  * @retval 无
  *
  * @硬件特性说明：
  * 1. 通道4使用CCMR2寄存器的高8位（OC4M[14:12], CC4S[11:10]）
  * 2. CCER寄存器通道4控制位偏移12位（CC4E=bit12, CC4P=bit13）
  * 3. 高级定时器空闲状态位OIS4在CR2寄存器的bit14
  */
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* 四层参数校验防护 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));  // 校验支持通道4的定时器
  assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
  assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

  /* 安全禁用通道输出 */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC4E; // 清除CC4E位（bit12）

  /* 获取寄存器当前状态 */
  tmpccer = TIMx->CCER;   // CCER当前值（通道使能/极性）
  tmpcr2 = TIMx->CR2;     // CR2当前值（高级功能配置）
  tmpccmrx = TIMx->CCMR2; // CCMR2当前值（通道4模式）

  /* 配置CCMR2寄存器（通道4模式） */
  tmpccmrx &= (uint16_t)~TIM_CCMR2_OC4M;  // 清除OC4M[14:12]位
  tmpccmrx &= (uint16_t)~TIM_CCMR2_CC4S;  // 清除CC4S[11:10]位
  tmpccmrx |= (uint16_t)(TIM_OCInitStruct->TIM_OCMode << 8); // 模式左移8位对齐

  /* 配置CCER寄存器（通道4特性） */
  tmpccer &= (uint16_t)~TIM_CCER_CC4P;    // 清除CC4P位（bit13）
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 12); // 左移12位对齐

  /* 配置输出使能状态 */
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 12); // 左移12位

  /* 高级定时器空闲状态配置 */
  if((TIMx == TIM1) || (TIMx == TIM8))
  {
    assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));
    /* 清除原空闲状态配置 */
    tmpcr2 &= (uint16_t)~TIM_CR2_OIS4;  // 清除OIS4位（bit14）
    /* 设置新的空闲状态 */
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 6); // 左移6位对齐
  }

  /* 原子化写入顺序 */
  TIMx->CR2 = tmpcr2;      // 先写CR2（高级功能配置）
  TIMx->CCMR2 = tmpccmrx;  // 写通道模式寄存器
  TIMx->CCR4 = TIM_OCInitStruct->TIM_Pulse; // 设置比较值（占空比基数）
  TIMx->CCER = tmpccer;    // 最后使能输出（防止中间状态）
}

/**
  * @brief  初始化输出比较配置结构体为默认值
  * @param  TIM_OCInitStruct: 输出比较配置结构体指针
  * @retval 无
  *
  * @默认配置策略：
  * - 模式：定时模式（无PWM输出）
  * - 主/互补输出：禁用
  * - 极性：高电平有效
  * - 比较值：0x00000000
  * - 空闲状态：复位状态
  *
  * @应用场景：
  * 在创建新的输出比较配置前调用，确保结构体字段处于安全状态
  */
void TIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  /* 安全默认配置 */
  TIM_OCInitStruct->TIM_OCMode = TIM_OCMode_Timing;      // 定时模式（无输出）
  TIM_OCInitStruct->TIM_OutputState = TIM_OutputState_Disable;  // 禁用主输出
  TIM_OCInitStruct->TIM_OutputNState = TIM_OutputNState_Disable; // 禁用互补输出
  TIM_OCInitStruct->TIM_Pulse = 0x00000000;             // 最小比较值
  TIM_OCInitStruct->TIM_OCPolarity = TIM_OCPolarity_High;      // 高电平有效
  TIM_OCInitStruct->TIM_OCNPolarity = TIM_OCPolarity_High;     // 互补通道默认极性
  TIM_OCInitStruct->TIM_OCIdleState = TIM_OCIdleState_Reset;   // 空闲时输出低
  TIM_OCInitStruct->TIM_OCNIdleState = TIM_OCNIdleState_Reset; // 互补通道空闲状态
}

/**
  * @brief  Selects the TIM Output Compare Mode.
  * @note   This function disables the selected channel before changing the Output
  *         Compare Mode. If needed, user has to enable this channel using
  *         TIM_CCxCmd() and TIM_CCxNCmd() functions.
  * @param  TIMx: where x can be 1 to 14 except 6 and 7, to select the TIM peripheral.
  * @param  TIM_Channel: specifies the TIM Channel
  *          This parameter can be one of the following values:
  *            @arg TIM_Channel_1: TIM Channel 1
  *            @arg TIM_Channel_2: TIM Channel 2
  *            @arg TIM_Channel_3: TIM Channel 3
  *            @arg TIM_Channel_4: TIM Channel 4
  * @param  TIM_OCMode: specifies the TIM Output Compare Mode.
  *           This parameter can be one of the following values:
  *            @arg TIM_OCMode_Timing
  *            @arg TIM_OCMode_Active
  *            @arg TIM_OCMode_Toggle
  *            @arg TIM_OCMode_PWM1
  *            @arg TIM_OCMode_PWM2
  *            @arg TIM_ForcedAction_Active
  *            @arg TIM_ForcedAction_InActive
  * @retval None
  */
/**
  * @brief  动态配置指定通道的输出比较模式
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_Channel: 目标通道（TIM_Channel_1~4）
  * @param  TIM_OCMode: 输出比较模式（PWM1/2,翻转等）
  * @retval 无
  *
  * @实现机制：
  * 1. 动态计算目标CCMR寄存器地址：
  *    - 通道1/3对应CCMR1，通道2/4对应CCMR2
  * 2. 采用位掩码清除旧模式
  * 3. 根据通道奇偶性选择位域偏移：
  *    - 通道1/3使用寄存器低8位
  *    - 通道2/4使用寄存器高8位（需左移8位）
  * @注意：
  * 本函数会临时禁用通道输出，配置完成后需手动重新使能
  */
void TIM_SelectOCxM(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode)
{
  uint32_t tmp = 0;
  uint16_t tmp1 = 0;

  /* 三重参数校验 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));    // 检查是否支持输出比较
  assert_param(IS_TIM_CHANNEL(TIM_Channel));   // 通道号有效性检查
  assert_param(IS_TIM_OCM(TIM_OCMode));        // 模式参数合法性

  tmp = (uint32_t) TIMx;           // 获取定时器基地址
  tmp += CCMR_OFFSET;              // 计算CCMR寄存器组偏移地址（0x18）

  /* 生成通道使能位掩码 */
  tmp1 = CCER_CCE_SET << (uint16_t)TIM_Channel; // 计算CCER中对应通道使能位

  /* 安全禁用通道输出 */
  TIMx->CCER &= (uint16_t) ~tmp1;  // 清除CCER中的CCxE位

  /* 通道分组处理 */
  if((TIM_Channel == TIM_Channel_1) ||(TIM_Channel == TIM_Channel_3))
  {
    /* 通道1/3处理流程 */
    tmp += (TIM_Channel>>1);  // 通道1: +0, 通道3: +1（指向CCMR1+4=CCMR2）

    /* 清除原OC模式配置 */
    *(__IO uint32_t *) tmp &= CCMR_OC13M_MASK; // 应用0xFFFF8FFF掩码
    
    /* 写入新模式 */
    *(__IO uint32_t *) tmp |= TIM_OCMode;      // 直接写入模式位（低8位）
  }
  else
  {
    /* 通道2/4处理流程 */
    tmp += (uint16_t)(TIM_Channel - (uint16_t)4)>> (uint16_t)1; // 通道2: +1, 通道4: +2

    /* 清除原OC模式配置 */
    *(__IO uint32_t *) tmp &= CCMR_OC24M_MASK; // 应用0xFFF88FFF掩码
    
    /* 写入新模式（高8位需左移） */
    *(__IO uint32_t *) tmp |= (uint16_t)(TIM_OCMode << 8); 
  }
}

/**
  * @brief  设置通道1比较寄存器值
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  Compare1: 新的比较值（0x0000-0xFFFF）
  * @retval 无
  *
  * @功能说明：
  * 直接修改CCR1寄存器，立即影响：
  * - PWM模式：占空比= (CCR1/ARR)*100%
  * - 输出比较模式：触发比较事件
  */
void TIM_SetCompare1(TIM_TypeDef* TIMx, uint32_t Compare1)
{
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));  // 校验定时器支持通道1
  
  TIMx->CCR1 = Compare1;  // 原子操作写入比较值
}

/**
  * @brief  设置通道2比较寄存器值
  * @param  TIMx: 定时器实例（TIM1-5,8,9,12）
  * @param  Compare2: 新的比较值（0x0000-0xFFFF）
  * @retval 无
  *
  * @特别说明：
  * TIM9/12等基础定时器仅支持通道1/2，调用前需确认硬件支持
  */
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint32_t Compare2)
{
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));  // 校验定时器支持通道2
  
  TIMx->CCR2 = Compare2;  // 直接写入CCR2寄存器
}

/**
  * @brief  设置通道3比较寄存器值
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  Compare3: 新的比较值（0x0000-0xFFFF）
  * @retval 无
  *
  * @硬件限制：
  * TIM3等通用定时器通道3不可用作PWM输出时，写入无效
  */
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint32_t Compare3)
{
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));  // 校验定时器支持通道3
  
  TIMx->CCR3 = Compare3;  // 原子操作写入CCR3
}

/**
  * @brief  设置通道4比较寄存器值
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  Compare4: 新的比较值（0x0000-0xFFFF）
  * @retval 无
  *
  * @应用场景：
  * 用于动态调整通道4的PWM占空比或比较触发点
  * 注意：需确保定时器已配置为PWM或输出比较模式
  */
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint32_t Compare4)
{
  /* 校验定时器支持通道4 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx)); // TIM1-5,8
  
  /* 原子操作写入CCR4寄存器 */
  TIMx->CCR4 = Compare4; // 立即生效，影响输出波形
}

/**
  * @brief  强制通道1输出指定电平
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_ForcedAction: 强制模式选项
  *         TIM_ForcedAction_Active:   强制高电平（OC1REF=1）
  *         TIM_ForcedAction_InActive: 强制低电平（OC1REF=0）
  * @retval 无
  *
  * @实现原理：
  * 通过设置CCMR1的OC1M[2:0]=110（Active）或111（Inactive）
  * 强制输出不受计数器影响，直到模式改变
  */
void TIM_ForcedOC1Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction)
{
  uint16_t tmpccmr1 = 0;

  /* 双重参数校验 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx)); // 支持通道1的定时器
  assert_param(IS_TIM_FORCED_ACTION(TIM_ForcedAction)); // 强制模式合法性

  tmpccmr1 = TIMx->CCMR1; // 获取当前CCMR1配置

  /* 清除原输出比较模式（OC1M[2:0]） */
  tmpccmr1 &= (uint16_t)~TIM_CCMR1_OC1M; // 掩码0xFFF8
  
  /* 设置强制输出模式 */
  tmpccmr1 |= TIM_ForcedAction; // 写入110或111到OC1M位域

  /* 写回寄存器 */
  TIMx->CCMR1 = tmpccmr1; // 立即生效，无需更新事件
}

/**
  * @brief  强制通道2输出指定电平
  * @param  TIMx: 定时器实例（TIM1-5,8,9,12）
  * @param  TIM_ForcedAction: 强制模式选项
  *         TIM_ForcedAction_Active:   强制高电平（OC2REF=1）
  *         TIM_ForcedAction_InActive: 强制低电平（OC2REF=0）
  * @retval 无
  *
  * @位域操作：
  * 通道2模式位OC2M位于CCMR1的高8位，需左移8位对齐
  */
void TIM_ForcedOC2Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction)
{
  uint16_t tmpccmr1 = 0;

  assert_param(IS_TIM_LIST2_PERIPH(TIMx)); // 支持通道2的定时器
  assert_param(IS_TIM_FORCED_ACTION(TIM_ForcedAction));

  tmpccmr1 = TIMx->CCMR1; 

  /* 清除原模式（OC2M[10:8]） */
  tmpccmr1 &= (uint16_t)~TIM_CCMR1_OC2M; // 掩码0xF8FF
  
  /* 设置强制模式并移位 */
  tmpccmr1 |= (uint16_t)(TIM_ForcedAction << 8); // 左移8位到OC2M位域

  TIMx->CCMR1 = tmpccmr1; 
}

/**
  * @brief  强制通道3输出指定电平
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_ForcedAction: 强制模式选项
  *         TIM_ForcedAction_Active:   强制高电平（OC3REF=1）
  *         TIM_ForcedAction_InActive: 强制低电平（OC3REF=0）
  * @retval 无
  *
  * @寄存器说明：
  * 通道3模式配置使用CCMR2寄存器的低8位（OC3M[6:4]）
  */
void TIM_ForcedOC3Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction)
{
  uint16_t tmpccmr2 = 0;

  assert_param(IS_TIM_LIST3_PERIPH(TIMx)); // 支持通道3的定时器
  assert_param(IS_TIM_FORCED_ACTION(TIM_ForcedAction));

  tmpccmr2 = TIMx->CCMR2; // 获取CCMR2当前值

  /* 清除原模式（OC3M[6:4]） */
  tmpccmr2 &= (uint16_t)~TIM_CCMR2_OC3M; // 掩码0xFF8F
  
  /* 设置强制模式 */
  tmpccmr2 |= TIM_ForcedAction; // 直接写入OC3M位域

  TIMx->CCMR2 = tmpccmr2; 
}
/**
  * @brief  强制通道4输出指定电平
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_ForcedAction: 强制模式选项
  *         TIM_ForcedAction_Active:   强制高电平（OC4REF=1）
  *         TIM_ForcedAction_InActive: 强制低电平（OC4REF=0）
  * @retval 无
  *
  * @寄存器操作：
  * 1. CCMR2寄存器高8位（OC4M[14:12]）配置强制模式
  * 2. 强制模式编码：0x6000（Active）或0x7000（Inactive）
  * 3. 配置后立即生效，无需等待更新事件
  */
void TIM_ForcedOC4Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction)
{
  uint16_t tmpccmr2 = 0;

  /* 双重参数校验 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx)); // 支持通道4的定时器
  assert_param(IS_TIM_FORCED_ACTION(TIM_ForcedAction));

  tmpccmr2 = TIMx->CCMR2; // 获取CCMR2当前值

  /* 清除原模式（OC4M[14:12]） */
  tmpccmr2 &= (uint16_t)~TIM_CCMR2_OC4M; // 掩码0x8FFF

  /* 设置强制模式并移位 */
  tmpccmr2 |= (uint16_t)(TIM_ForcedAction << 8); // 左移8位到高字节

  /* 写入配置 */
  TIMx->CCMR2 = tmpccmr2; // 立即生效，中断产生不受影响
}

/**
  * @brief  配置通道1比较寄存器预装载功能
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_OCPreload: 预装载使能状态
  *         TIM_OCPreload_Enable:   CCR1使用预装载缓冲
  *         TIM_OCPreload_Disable:  CCR1直接写入生效
  * @retval 无
  *
  * @功能说明：
  * 启用预装载后，CCR1的修改将在下一个更新事件生效
  * 避免PWM周期中修改占空比导致波形撕裂
  */
void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr1 = 0;

  /* 参数校验 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx)); // 支持通道1的定时器
  assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));

  tmpccmr1 = TIMx->CCMR1; // 获取当前CCMR1配置

  /* 清除预装载使能位（OC1PE=bit3） */
  tmpccmr1 &= (uint16_t)(~TIM_CCMR1_OC1PE); // 掩码0xFFF7

  /* 设置预装载状态 */
  tmpccmr1 |= TIM_OCPreload; // 写入0x0008或0x0000

  /* 写回寄存器 */
  TIMx->CCMR1 = tmpccmr1; // 配置立即生效，但CCR1需更新事件同步
}

/**
  * @brief  配置通道2比较寄存器预装载功能
  * @param  TIMx: 定时器实例（TIM1-5,8,9,12）
  * @param  TIM_OCPreload: 预装载使能状态
  *         TIM_OCPreload_Enable:   CCR2使用预装载缓冲
  *         TIM_OCPreload_Disable:  CCR2直接写入生效
  * @retval 无
  *
  * @位域操作：
  * 通道2预装载位OC2PE位于CCMR1[11]，需左移8位对齐
  * 实际配置值：0x0800（Enable）或0x0000（Disable）
  */
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr1 = 0;

  /* 参数校验 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx)); // 支持通道2的定时器
  assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));

  tmpccmr1 = TIMx->CCMR1; 

  /* 清除预装载使能位（OC2PE=bit11） */
  tmpccmr1 &= (uint16_t)(~TIM_CCMR1_OC2PE); // 掩码0xF7FF

  /* 设置预装载并移位 */
  tmpccmr1 |= (uint16_t)(TIM_OCPreload << 8); // 左移8位到bit11

  /* 写回配置 */
  TIMx->CCMR1 = tmpccmr1; // 修改立即生效，CCR2值需ARR更新同步
}
/**
  * @brief  配置通道3比较寄存器预装载功能
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_OCPreload: 预装载使能状态
  *         TIM_OCPreload_Enable:   CCR3使用预装载缓冲
  *         TIM_OCPreload_Disable:  CCR3直接写入生效
  * @retval 无
  *
  * @实现原理：
  * 通过CCMR2寄存器的OC3PE位（bit2）控制预装载
  * 启用后CCR3的修改将在下一个更新事件生效，确保波形完整性
  */
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr2 = 0;

  /* 参数校验 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx)); // 仅支持TIM1-5,8
  assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));

  tmpccmr2 = TIMx->CCMR2; 

  /* 清除OC3PE位（bit2）保留其他配置 */
  tmpccmr2 &= (uint16_t)(~TIM_CCMR2_OC3PE); // 掩码0xFFFB（1111 1111 1111 1011）

  /* 注入新的预装载配置 */
  tmpccmr2 |= TIM_OCPreload; // 0x0004启用/0x0000禁用

  TIMx->CCMR2 = tmpccmr2;   // 配置生效后CCR3更新需等待UEV事件
}

/**
  * @brief  配置通道4比较寄存器预装载功能
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_OCPreload: 预装载使能状态
  *         TIM_OCPreload_Enable:   CCR4使用预装载缓冲
  *         TIM_OCPreload_Disable:  CCR4直接写入生效
  * @retval 无
  *
  * @位域操作：
  * OC4PE位在CCMR2[10]，需将配置值左移8位对齐
  * 0x0400表示启用，0x0000表示禁用
  */
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr2 = 0;

  /* 校验定时器支持通道4 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx)); 
  assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));

  tmpccmr2 = TIMx->CCMR2;

  /* 清除OC4PE位（bit10） */
  tmpccmr2 &= (uint16_t)(~TIM_CCMR2_OC4PE); // 掩码0xFBFF（1111 1011 1111 1111）

  /* 配置值移位并写入 */
  tmpccmr2 |= (uint16_t)(TIM_OCPreload << 8); // 左移8位到bit10

  TIMx->CCMR2 = tmpccmr2; // 修改立即生效，CCR4需ARR更新同步
}

/**
  * @brief  启用/禁用通道1输出比较快速模式
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_OCFast: 快速模式状态
  *         TIM_OCFast_Enable: 比较事件立即改变输出
  *         TIM_OCFast_Disable: 正常模式（1周期延迟）
  * @retval 无
  *
  * @技术细节：
  * 快速模式通过OC1FE位（CCMR1[2]）控制
  * 启用后输出跳变延迟从1个APB周期缩短至当前周期
  */
void TIM_OC1FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast)
{
  uint16_t tmpccmr1 = 0;

  assert_param(IS_TIM_LIST1_PERIPH(TIMx));
  assert_param(IS_TIM_OCFAST_STATE(TIM_OCFast));

  tmpccmr1 = TIMx->CCMR1;

  /* 清除OC1FE位（bit2） */
  tmpccmr1 &= (uint16_t)~TIM_CCMR1_OC1FE; // 掩码0xFFFB（1111 1111 1111 1011）

  /* 设置快速模式 */
  tmpccmr1 |= TIM_OCFast; // 0x0004启用/0x0000禁用

  TIMx->CCMR1 = tmpccmr1; // 配置立即生效，影响后续所有比较事件
}

/**
  * @brief  配置通道2输出比较快速功能
  * @param  TIMx: 定时器实例（TIM1-5,8,9,12）
  * @param  TIM_OCFast: 快速模式状态
  *         TIM_OCFast_Enable: 比较事件立即改变输出
  *         TIM_OCFast_Disable: 正常模式（1周期延迟）
  * @retval 无
  *
  * @位域操作：
  * OC2FE位在CCMR1[10]，需左移8位对齐配置值
  * 0x0400表示启用，0x0000表示禁用
  */
void TIM_OC2FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast)
{
  uint16_t tmpccmr1 = 0;

  /* 校验定时器支持通道2 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx)); // 包括TIM9/12
  assert_param(IS_TIM_OCFAST_STATE(TIM_OCFast));

  tmpccmr1 = TIMx->CCMR1;

  /* 清除OC2FE位（bit10） */
  tmpccmr1 &= (uint16_t)(~TIM_CCMR1_OC2FE); // 掩码0xFBFF（1111101111111111）

  /* 设置快速模式并移位 */
  tmpccmr1 |= (uint16_t)(TIM_OCFast << 8); // 左移8位到bit10

  TIMx->CCMR1 = tmpccmr1; // 配置立即生效
}

/**
  * @brief  配置通道3输出比较快速功能
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_OCFast: 快速模式状态
  *         TIM_OCFast_Enable: 比较事件立即改变输出
  *         TIM_OCFast_Disable: 正常模式（1周期延迟）
  * @retval 无
  *
  * @硬件细节：
  * OC3FE位在CCMR2[2]，直接写入无需移位
  * 启用后输出跳变与比较事件同步，无延迟
  */
void TIM_OC3FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast)
{
  uint16_t tmpccmr2 = 0;

  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCFAST_STATE(TIM_OCFast));

  tmpccmr2 = TIMx->CCMR2;

  /* 清除OC3FE位（bit2） */
  tmpccmr2 &= (uint16_t)~TIM_CCMR2_OC3FE; // 掩码0xFFFB（1111111110111111）

  /* 设置快速模式 */
  tmpccmr2 |= TIM_OCFast; // 0x0004启用/0x0000禁用

  TIMx->CCMR2 = tmpccmr2; // 立即影响后续比较事件
}

/**
  * @brief  配置通道4输出比较快速功能
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_OCFast: 快速模式状态
  *         TIM_OCFast_Enable: 比较事件立即改变输出
  *         TIM_OCFast_Disable: 正常模式（1周期延迟）
  * @retval 无
  *
  * @技术说明：
  * OC4FE位在CCMR2[10]，需左移8位配置
  * 快速模式消除比较器输出延迟，精度达1个时钟周期
  */
void TIM_OC4FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast)
{
  uint16_t tmpccmr2 = 0;

  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCFAST_STATE(TIM_OCFast));

  tmpccmr2 = TIMx->CCMR2;

  /* 清除OC4FE位（bit10） */
  tmpccmr2 &= (uint16_t)(~TIM_CCMR2_OC4FE); // 掩码0xFBFF（1111101111111111）

  /* 设置快速模式并移位 */
  tmpccmr2 |= (uint16_t)(TIM_OCFast << 8); // 左移8位到bit10

  TIMx->CCMR2 = tmpccmr2; // 配置立即生效
}

/**
  * @brief  配置通道1输出清除功能
  * @param  TIMx: 定时器实例（TIM1-5,8）
  * @param  TIM_OCClear: 清除功能使能状态
  *         TIM_OCClear_Enable:  允许外部事件清除OC1REF
  *         TIM_OCClear_Disable: 禁用清除功能
  * @retval 无
  *
  * @应用场景：
  * 用于紧急停机保护，当指定外部事件（如刹车信号）触发时
  * 立即强制OC1REF为低电平，切断功率输出
  */
void TIM_ClearOC1Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear)
{
  uint16_t tmpccmr1 = 0;

  assert_param(IS_TIM_LIST1_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(TIM_OCClear));

  tmpccmr1 = TIMx->CCMR1;

  /* 清除OC1CE位（bit7） */
  tmpccmr1 &= (uint16_t)~TIM_CCMR1_OC1CE; // 掩码0xFF7F（1111111101111111）

  /* 设置清除功能 */
  tmpccmr1 |= TIM_OCClear; // 0x0080启用/0x0000禁用

  TIMx->CCMR1 = tmpccmr1; // 配置立即生效
}
/**
  * @brief  清除或保护外部事件中的OCREF2信号
  * @note   该函数通过配置TIMx的CCMR1寄存器，控制输出比较通道2的参考信号清除功能。
  *         当配置为清除模式时，外部事件(ECE)将OCREF2信号置为无效电平
  * @param  TIMx: 定时器外设，支持TIM1/2/3/4/5/8/9/12
  * @param  TIM_OCClear: 输出比较清除使能状态
  *         @arg TIM_OCClear_Enable:  允许通过外部事件清除输出
  *         @arg TIM_OCClear_Disable: 禁用清除功能，保护输出信号
  * @retval 无
  * @warning 需要确保传入的TIMx支持通道2功能
  * @details 操作流程：
  *          1. 读取当前CCMR1寄存器值
  *          2. 清除OC2CE位（通道2清除使能位）
  *          3. 根据输入参数设置新的OC2CE状态
  *          4. 将配置写回寄存器
  */
void TIM_ClearOC2Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear)
{
  uint16_t tmpccmr1 = 0;  // 定义临时变量用于保存CCMR1寄存器值

  /* 参数有效性验证 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));       // 验证TIMx是否属于支持通道2的定时器
  assert_param(IS_TIM_OCCLEAR_STATE(TIM_OCClear)); // 验证输入参数合法性

  tmpccmr1 = TIMx->CCMR1;  // 读取当前CCMR1寄存器配置值

  /* 清除OC2CE位（通道2清除使能位）*/
  tmpccmr1 &= (uint16_t)~TIM_CCMR1_OC2CE;  // 使用位掩码清除第15位（OC2CE位）

  /* 设置输出比较清除功能 */
  tmpccmr1 |= (uint16_t)(TIM_OCClear << 8); // 将输入参数左移8位对齐到OC2CE位（位15）

  /* 更新寄存器配置 */
  TIMx->CCMR1 = tmpccmr1;  // 将新配置写入CCMR1寄存器，立即生效
}

/**
  * @brief  清除或保护外部事件中的OCREF3信号
  * @note   该函数通过配置TIMx的CCMR2寄存器，控制输出比较通道3的参考信号清除功能
  * @param  TIMx: 定时器外设，支持TIM1/2/3/4/5/8
  * @param  TIM_OCClear: 输出清除使能状态（同通道2）
  * @retval 无
  * @details 操作流程：
  *          1. 读取CCMR2当前值
  *          2. 清除OC3CE位（通道3清除位）
  *          3. 设置新的清除状态
  *          4. 写回寄存器
  */
void TIM_ClearOC3Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear)
{
  uint16_t tmpccmr2 = 0;  // CCMR2寄存器临时变量

  /* 参数校验（TIMx需支持通道3） */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(TIM_OCClear));

  tmpccmr2 = TIMx->CCMR2;  // 读取当前CCMR2配置

  /* 清除OC3CE位（位7） */
  tmpccmr2 &= (uint16_t)~TIM_CCMR2_OC3CE;  // TIM_CCMR2_OC3CE对应位7

  /* 设置清除功能（直接赋值，因为OC3CE在位7） */
  tmpccmr2 |= TIM_OCClear;  // TIM_OCClear参数应已对齐到bit7

  /* 应用配置 */
  TIMx->CCMR2 = tmpccmr2;  // 更新CCMR2寄存器
}

/**
  * @brief  清除或保护外部事件中的OCREF4信号
  * @note   操作CCMR2寄存器控制通道4的清除功能，实现原理与通道3类似
  * @param  TIMx: 定时器外设，支持TIM1/2/3/4/5/8
  * @param  TIM_OCClear: 清除使能状态
  * @retval 无
  * @details 注意OC4CE位在位15（高8位区域），需要左移8位对齐
  */
void TIM_ClearOC4Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear)
{
  uint16_t tmpccmr2 = 0;

  assert_param(IS_TIM_LIST3_PERIPH(TIMx));  // 复用通道3的定时器验证
  assert_param(IS_TIM_OCCLEAR_STATE(TIM_OCClear));

  tmpccmr2 = TIMx->CCMR2;  // 获取当前寄存器状态

  /* 清除OC4CE位（位15） */
  tmpccmr2 &= (uint16_t)~TIM_CCMR2_OC4CE;  // TIM_CCMR2_OC4CE对应位15

  /* 设置新状态（需要左移8位到高字节区域） */
  tmpccmr2 |= (uint16_t)(TIM_OCClear << 8);  // 参数左移8位到位15

  /* 应用配置 */
  TIMx->CCMR2 = tmpccmr2;  // 写入更新后的配置
}
/**
  * @brief  配置TIMx通道1输出极性
  * @note   通过修改CCER寄存器的CC1P位控制输出比较通道1的有效电平
  * @param  TIMx: 定时器外设，支持TIM1~TIM14（除TIM6、TIM7）
  * @param  TIM_OCPolarity: 输出极性选择
  *         @arg TIM_OCPolarity_High: 输出高电平为有效状态
  *         @arg TIM_OCPolarity_Low:  输出低电平为有效状态
  * @retval 无
  * @details 工作流程：
  *          1. 读取当前CCER寄存器配置
  *          2. 清除原有CC1P位设置
  *          3. 应用新的极性配置
  *          4. 更新寄存器使配置生效
  */
void TIM_OC1PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)
{
  uint16_t tmpccer = 0;  // 定义临时变量存储CCER寄存器值

  /* 参数有效性验证 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));         // 验证TIMx是否属于通道1支持的定时器
  assert_param(IS_TIM_OC_POLARITY(TIM_OCPolarity)); // 验证极性参数合法性

  tmpccer = TIMx->CCER;  // 读取当前捕获/比较使能寄存器(CCER)的值

  /* 清除CC1P位（通道1输出极性位） */
  tmpccer &= (uint16_t)(~TIM_CCER_CC1P);  // TIM_CCER_CC1P对应位1，取反后与操作实现位清除

  /* 设置新的极性配置 */
  tmpccer |= TIM_OCPolarity;  // 将参数值直接写入位1（TIM_OCPolarity参数已对齐）

  /* 更新寄存器配置 */
  TIMx->CCER = tmpccer;  // 将新配置写入CCER寄存器，立即生效
}

/**
  * @brief  配置TIMx通道1N互补输出极性
  * @note   用于高级定时器的互补通道输出极性设置（如TIM1/TIM8）
  * @param  TIMx: 定时器外设，仅支持TIM1/TIM8
  * @param  TIM_OCNPolarity: 互补通道输出极性
  *         @arg TIM_OCNPolarity_High: 互补输出高电平有效
  *         @arg TIM_OCNPolarity_Low: 互补输出低电平有效
  * @retval 无
  * @warning 仅适用于带死区控制的互补输出通道
  */
void TIM_OC1NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity)
{
  uint16_t tmpccer = 0;
  /* 参数校验 */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));         // 验证是否高级定时器（TIM1/8）
  assert_param(IS_TIM_OCN_POLARITY(TIM_OCNPolarity)); // 验证互补通道参数有效性
  
  tmpccer = TIMx->CCER;  // 读取当前CCER寄存器状态

  /* 清除CC1NP位（通道1N极性位） */
  tmpccer &= (uint16_t)~TIM_CCER_CC1NP;  // TIM_CCER_CC1NP对应位3

  /* 设置互补通道极性 */
  tmpccer |= TIM_OCNPolarity;  // 参数直接写入位3（参数已对齐）

  /* 应用新配置 */
  TIMx->CCER = tmpccer;  // 更新CCER寄存器，配置立即生效
}

/**
  * @brief  配置TIMx通道2输出极性
  * @note   通过CCER寄存器的CC2P位控制通道2输出有效电平
  * @param  TIMx: 定时器外设，支持TIM1/2/3/4/5/8/9/12
  * @param  TIM_OCPolarity: 输出极性设置
  *         @arg TIM_OCPolarity_High: 高电平有效
  *         @arg TIM_OCPolarity_Low: 低电平有效
  * @retval 无
  * @details 位操作说明：
  *          CC2P位在CCER寄存器位5，需要参数左移4位对齐
  */
void TIM_OC2PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)
{
  uint16_t tmpccer = 0;

  /* 参数有效性检查 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));         // 验证TIMx是否支持通道2
  assert_param(IS_TIM_OC_POLARITY(TIM_OCPolarity)); // 验证极性参数合法性

  tmpccer = TIMx->CCER;  // 获取当前CCER寄存器值

  /* 清除原有CC2P位设置（位5） */
  tmpccer &= (uint16_t)(~TIM_CCER_CC2P);  // 使用位掩码清除位5

  /* 设置新的极性配置（左移4位对齐到CC2P位） */
  tmpccer |= (uint16_t)(TIM_OCPolarity << 4);  // 参数值左移4位到位5

  /* 更新寄存器 */
  TIMx->CCER = tmpccer;  // 写入新配置，立即影响输出行为
}

/**
  * @brief  配置TIMx通道2N互补输出极性
  * @note   用于高级定时器(TIM1/TIM8)的互补通道极性设置，配合死区控制使用
  * @param  TIMx: 定时器外设，仅支持TIM1/TIM8
  * @param  TIM_OCNPolarity: 互补通道输出极性
  *         @arg TIM_OCNPolarity_High: 互补输出高电平有效
  *         @arg TIM_OCNPolarity_Low: 互补输出低电平有效
  * @retval 无
  * @warning 需在PWM模式使能前配置，配合刹车功能使用
  */
void TIM_OC2NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity)
{
  uint16_t tmpccer = 0;

  /* 参数有效性验证 */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));         // 确认TIMx为支持互补输出的高级定时器
  assert_param(IS_TIM_OCN_POLARITY(TIM_OCNPolarity)); // 验证参数合法性
  
  tmpccer = TIMx->CCER;  // 读取当前捕获/比较使能寄存器值

  /* 清除CC2NP位（通道2N极性位，位7） */
  tmpccer &= (uint16_t)~TIM_CCER_CC2NP;  // TIM_CCER_CC2NP对应位7（0x0080）

  /* 设置新的极性配置（左移4位对齐到CC2NP位） */
  tmpccer |= (uint16_t)(TIM_OCNPolarity << 4);  // 参数值左移4位到位7

  /* 更新寄存器配置 */
  TIMx->CCER = tmpccer;  // 写入新配置，影响互补输出行为
}

/**
  * @brief  配置TIMx通道3输出极性
  * @note   通过CCER寄存器的CC3P位（位9）控制通道3有效电平
  * @param  TIMx: 定时器外设，支持TIM1/2/3/4/5/8
  * @param  TIM_OCPolarity: 输出极性选择
  *         @arg TIM_OCPolarity_High: 高电平为有效状态
  *         @arg TIM_OCPolarity_Low: 低电平为有效状态
  * @retval 无
  * @details 位操作说明：
  *          CC3P位在CCER寄存器位9，需要参数左移8位对齐
  */
void TIM_OC3PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)
{
  uint16_t tmpccer = 0;

  /* 参数校验 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));         // 验证TIMx是否支持通道3
  assert_param(IS_TIM_OC_POLARITY(TIM_OCPolarity)); // 验证极性参数合法性

  tmpccer = TIMx->CCER;  // 获取当前CCER寄存器状态

  /* 清除原有CC3P位设置（位9） */
  tmpccer &= (uint16_t)~TIM_CCER_CC3P;  // TIM_CCER_CC3P对应位9（0x0200）

  /* 设置新的极性配置 */
  tmpccer |= (uint16_t)(TIM_OCPolarity << 8);  // 参数左移8位到位9

  /* 应用配置更新 */
  TIMx->CCER = tmpccer;  // 写入寄存器，立即影响通道3输出
}

/**
  * @brief  配置TIMx通道3N互补输出极性
  * @note   用于高级定时器(TIM1/TIM8)通道3的互补输出极性设置
  * @param  TIMx: 定时器外设，仅支持TIM1/TIM8
  * @param  TIM_OCNPolarity: 互补通道输出极性
  *         @arg TIM_OCNPolarity_High: 高电平有效
  *         @arg TIM_OCNPolarity_Low: 低电平有效
  * @retval 无
  * @details 硬件关联：
  *          CC3NP位在CCER寄存器位11，需左移8位对齐参数
  */
void TIM_OC3NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity)
{
  uint16_t tmpccer = 0;
 
  /* 参数验证 */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));         // 确认高级定时器
  assert_param(IS_TIM_OCN_POLARITY(TIM_OCNPolarity)); // 验证互补参数合法性
    
  tmpccer = TIMx->CCER;  // 读取当前CCER寄存器值

  /* 清除CC3NP位（位11） */
  tmpccer &= (uint16_t)~TIM_CCER_CC3NP;  // TIM_CCER_CC3NP对应位11（0x0800）

  /* 设置新极性（左移8位到位11） */
  tmpccer |= (uint16_t)(TIM_OCNPolarity << 8);  // 参数左移8位对齐

  /* 更新硬件配置 */
  TIMx->CCER = tmpccer;  // 配置生效，影响通道3N输出
}

/**
  * @brief  配置TIMx通道4输出极性
  * @note   控制CCER寄存器的CC4P位（位13）设置通道4有效电平
  * @param  TIMx: 定时器外设，支持TIM1/2/3/4/5/8
  * @param  TIM_OCPolarity: 输出极性选择
  *         @arg TIM_OCPolarity_High: 高电平有效
  *         @arg TIM_OCPolarity_Low: 低电平有效
  * @retval 无
  * @details 位操作说明：
  *          CC4P位在CCER寄存器位13，需要参数左移12位对齐
  */
void TIM_OC4PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)
{
  uint16_t tmpccer = 0;

  /* 参数检查 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));         // 验证TIMx支持通道4
  assert_param(IS_TIM_OC_POLARITY(TIM_OCPolarity)); // 验证参数合法性

  tmpccer = TIMx->CCER;  // 获取当前CCER寄存器值

  /* 清除原有CC4P位（位13） */
  tmpccer &= (uint16_t)~TIM_CCER_CC4P;  // TIM_CCER_CC4P对应位13（0x2000）

  /* 设置新的极性配置 */
  tmpccer |= (uint16_t)(TIM_OCPolarity << 12);  // 参数左移12位到位13

  /* 应用寄存器更新 */
  TIMx->CCER = tmpccer;  // 写入配置，立即改变通道4输出行为
}

/**
  * @brief  启用或禁用指定通道的捕获/比较功能
  * @note   通过操作CCER寄存器的CCxE位控制通道主输出使能
  * @param  TIMx: 定时器外设，支持TIM1-TIM14（除TIM6/7）
  * @param  TIM_Channel: 目标通道编号（1-4）
  *         @arg TIM_Channel_1: 通道1
  *         @arg TIM_Channel_2: 通道2
  *         @arg TIM_Channel_3: 通道3
  *         @arg TIM_Channel_4: 通道4
  * @param  TIM_CCx: 通道使能状态
  *         @arg TIM_CCx_Enable: 使能通道比较/捕获功能
  *         @arg TIM_CCx_Disable: 禁用通道功能
  * @retval 无
  * @warning 通道编号与物理引脚对应关系需参考数据手册
  */
void TIM_CCxCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx)
{
  uint16_t tmp = 0;

  /* 参数有效性验证 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));       // 确认TIMx属于基础定时器系列
  assert_param(IS_TIM_CHANNEL(TIM_Channel));     // 验证通道号在1-4之间
  assert_param(IS_TIM_CCX(TIM_CCx));             // 验证使能/禁用参数有效性

  /* 生成位掩码（每个通道占4位间隔：CC1E@位0,CC2E@位4,...） */
  tmp = CCER_CCE_SET << TIM_Channel;  // 基础掩码0x0001根据通道号移位生成

  /* 清除目标通道的使能位 */
  TIMx->CCER &= (uint16_t)~ tmp;  // 通过位取反实现目标位清零

  /* 设置新的使能状态（使能位精确对齐） */
  TIMx->CCER |=  (uint16_t)(TIM_CCx << TIM_Channel); // 参数值移位到对应通道位
}

/**
  * @brief  启用或禁用互补通道的捕获/比较功能
  * @note   控制CCER寄存器的CCxNE位，用于高级定时器的互补输出通道
  * @param  TIMx: 定时器外设，仅支持TIM1/TIM8
  * @param  TIM_Channel: 目标互补通道编号（1-3）
  *         @arg TIM_Channel_1: 通道1N
  *         @arg TIM_Channel_2: 通道2N
  *         @arg TIM_Channel_3: 通道3N
  * @param  TIM_CCxN: 互补通道使能状态
  *         @arg TIM_CCxN_Enable: 使能互补输出
  *         @arg TIM_CCxN_Disable: 禁用互补通道
  * @retval 无
  * @warning 需配合死区控制使用，防止功率器件直通
  */
void TIM_CCxNCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCxN)
{
  uint16_t tmp = 0;

  /* 参数检查 */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));              // 仅高级定时器支持互补通道
  assert_param(IS_TIM_COMPLEMENTARY_CHANNEL(TIM_Channel)); // 验证通道号1-3
  assert_param(IS_TIM_CCXN(TIM_CCxN));                  // 验证使能参数合法性

  /* 生成互补通道掩码（CC1NE@位2,CC2NE@位6,...） */
  tmp = CCER_CCNE_SET << TIM_Channel;  // 基础掩码0x0004根据通道号移位

  /* 清除原互补通道使能位 */
  TIMx->CCER &= (uint16_t) ~tmp;  // 按位取反后与操作清零目标位

  /* 设置新的互补通道状态 */
  TIMx->CCER |=  (uint16_t)(TIM_CCxN << TIM_Channel); // 参数值精准对齐到目标位
}

/**
  * @}
  */

/** @defgroup TIM_Group3 Input Capture management functions
 *  @brief    Input Capture management functions 
 *
@verbatim   
 ===============================================================================
                  ##### Input Capture management functions #####
 ===============================================================================  
         
            ##### TIM Driver: how to use it in Input Capture Mode #####
 ===============================================================================
    [..]    
    To use the Timer in Input Capture mode, the following steps are mandatory:
       
      (#) Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) 
          function
       
      (#) Configure the TIM pins by configuring the corresponding GPIO pins
       
      (#) Configure the Time base unit as described in the first part of this driver,
          if needed, else the Timer will run with the default configuration:
        (++) Autoreload value = 0xFFFF
        (++) Prescaler value = 0x0000
        (++) Counter mode = Up counting
        (++) Clock Division = TIM_CKD_DIV1
          
      (#) Fill the TIM_ICInitStruct with the desired parameters including:
        (++) TIM Channel: TIM_Channel
        (++) TIM Input Capture polarity: TIM_ICPolarity
        (++) TIM Input Capture selection: TIM_ICSelection
        (++) TIM Input Capture Prescaler: TIM_ICPrescaler
        (++) TIM Input CApture filter value: TIM_ICFilter
       
      (#) Call TIM_ICInit(TIMx, &TIM_ICInitStruct) to configure the desired channel 
          with the corresponding configuration and to measure only frequency 
          or duty cycle of the input signal, or, Call TIM_PWMIConfig(TIMx, &TIM_ICInitStruct) 
          to configure the desired channels with the corresponding configuration 
          and to measure the frequency and the duty cycle of the input signal
          
      (#) Enable the NVIC or the DMA to read the measured frequency. 
          
      (#) Enable the corresponding interrupt (or DMA request) to read the Captured 
          value, using the function TIM_ITConfig(TIMx, TIM_IT_CCx) 
          (or TIM_DMA_Cmd(TIMx, TIM_DMA_CCx)) 
       
      (#) Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
       
      (#) Use TIM_GetCapturex(TIMx); to read the captured value.
       
      -@- All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 

@endverbatim
  * @{
  */
/**
  * @brief  初始化定时器输入捕获通道配置
  * @note   根据TIM_ICInitTypeDef结构体参数配置指定通道的输入捕获参数，
  *         包括极性、输入映射、滤波器和预分频器
  * @param  TIMx: 定时器外设，支持TIM1-TIM14（除TIM6/7）
  * @param  TIM_ICInitStruct: 输入捕获配置结构体指针，包含以下字段：
  *         - TIM_Channel: 通道选择（1-4）
  *         - TIM_ICPolarity: 输入极性（上升沿/下降沿/双沿）
  *         - TIM_ICSelection: 输入映射（直接/间接/TRC）
  *         - TIM_ICPrescaler: 捕获预分频（每N个事件捕获一次）
  *         - TIM_ICFilter: 输入滤波器值（0x0-0xF）
  * @retval 无
  * @warning 不同通道支持的定时器型号不同，需参考参考手册
  */
void TIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct)
{
  /* 参数有效性验证 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));  // 基础定时器型号验证
  assert_param(IS_TIM_IC_POLARITY(TIM_ICInitStruct->TIM_ICPolarity)); // 输入极性校验
  assert_param(IS_TIM_IC_SELECTION(TIM_ICInitStruct->TIM_ICSelection)); // 输入映射校验
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICInitStruct->TIM_ICPrescaler)); // 预分频系数校验
  assert_param(IS_TIM_IC_FILTER(TIM_ICInitStruct->TIM_ICFilter));       // 滤波器值校验
  
  /* 根据配置的通道号选择初始化路径 */
  if (TIM_ICInitStruct->TIM_Channel == TIM_Channel_1)
  {
    /* 通道1配置流程 */
    TI1_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,  // 配置CCMR1寄存器的IC1F/IC1PSC/CC1S位
               TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* 设置输入捕获预分频器（CCMR1寄存器的IC1PSC位） */
    TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
  else if (TIM_ICInitStruct->TIM_Channel == TIM_Channel_2)
  {
    /* 通道2配置流程 */
    assert_param(IS_TIM_LIST2_PERIPH(TIMx));  // 验证TIMx是否支持通道2
    TI2_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,  // 配置CCMR1寄存器的IC2F/IC2PSC/CC2S位
               TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* 设置通道2预分频（CCMR1寄存器的IC2PSC位） */
    TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
  else if (TIM_ICInitStruct->TIM_Channel == TIM_Channel_3)
  {
    /* 通道3配置流程 */
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));  // 验证TIMx是否支持通道3
    TI3_Config(TIMx,  TIM_ICInitStruct->TIM_ICPolarity,  // 配置CCMR2寄存器的IC3F/IC3PSC/CC3S位
               TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* 设置通道3预分频（CCMR2寄存器的IC3PSC位） */
    TIM_SetIC3Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
  else
  {
    /* 通道4配置流程 */
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));  // 验证TIMx是否支持通道4
    TI4_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,  // 配置CCMR2寄存器的IC4F/IC4PSC/CC4S位
               TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* 设置通道4预分频（CCMR2寄存器的IC4PSC位） */
    TIM_SetIC4Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
}
/**
  * @brief  初始化输入捕获配置结构体为默认值
  * @note   为TIM_ICInitTypeDef结构体提供安全默认配置：
  *         - 通道1、上升沿触发、直接输入映射、无预分频、无滤波
  * @param  TIM_ICInitStruct: 待初始化的输入捕获配置结构体指针
  * @retval 无
  * @warning 在调用TIM_ICInit前必须初始化该结构体
  */
void TIM_ICStructInit(TIM_ICInitTypeDef* TIM_ICInitStruct)
{
  /* 设置默认配置 */
  TIM_ICInitStruct->TIM_Channel = TIM_Channel_1;          // 默认使用通道1
  TIM_ICInitStruct->TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿触发
  TIM_ICInitStruct->TIM_ICSelection = TIM_ICSelection_DirectTI; // 直接输入映射
  TIM_ICInitStruct->TIM_ICPrescaler = TIM_ICPSC_DIV1;     // 每个事件都捕获
  TIM_ICInitStruct->TIM_ICFilter = 0x00;                  // 无输入滤波
}

/**
  * @brief  配置定时器进行PWM输入测量
  * @note   通过两个关联通道实现PWM周期和占空比测量：
  *         主通道测量周期，互补通道测量脉宽
  * @param  TIMx: 定时器外设，支持TIM1/2/3/4/5/8/9/12
  * @param  TIM_ICInitStruct: 输入捕获配置结构体指针，包含：
  *         - TIM_Channel: 主测量通道（1或2）
  *         - 其他参数用于主通道配置
  * @retval 无
  * @warning 必须使用两个关联通道（如TI1+TI2），需正确连接PWM信号
  */
void TIM_PWMIConfig(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct)
{
  uint16_t icoppositepolarity = TIM_ICPolarity_Rising;  // 互补通道极性
  uint16_t icoppositeselection = TIM_ICSelection_DirectTI; // 互补通道输入映射

  /* 参数校验 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));  // 验证支持PWM输入模式的定时器

  /* 生成互补极性配置（与主通道相反） */
  icoppositepolarity = (TIM_ICInitStruct->TIM_ICPolarity == TIM_ICPolarity_Rising) ? 
                      TIM_ICPolarity_Falling : TIM_ICPolarity_Rising;

  /* 生成互补输入映射配置 */
  icoppositeselection = (TIM_ICInitStruct->TIM_ICSelection == TIM_ICSelection_DirectTI) ?
                       TIM_ICSelection_IndirectTI : TIM_ICSelection_DirectTI;

  /* 通道配置分支 */
  if (TIM_ICInitStruct->TIM_Channel == TIM_Channel_1)
  {
    /*---- 主通道1配置 ----*/
    /* 配置TI1输入参数 */
    TI1_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
              TIM_ICInitStruct->TIM_ICSelection,
              TIM_ICInitStruct->TIM_ICFilter);
    /* 设置通道1预分频 */
    TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);

    /*---- 互补通道2配置 ----*/
    /* 配置TI2为反向参数测量占空比 */
    TI2_Config(TIMx, icoppositepolarity,  // 反向极性捕获下降沿
              icoppositeselection,       // 间接输入映射
              TIM_ICInitStruct->TIM_ICFilter);
    /* 设置通道2预分频 */
    TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
  else  // 当主通道为2时
  {
    /*---- 主通道2配置 ----*/
    TI2_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
              TIM_ICInitStruct->TIM_ICSelection,
              TIM_ICInitStruct->TIM_ICFilter);
    TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);

    /*---- 互补通道1配置 ----*/
    TI1_Config(TIMx, icoppositepolarity,  // 反向极性
              icoppositeselection,       // 间接输入
              TIM_ICInitStruct->TIM_ICFilter);
    TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
}

/**
  * @brief  获取TIMx输入通道1的捕获值（输入捕获模式/PWM输入模式）
  * @param  TIMx: 定时器外设指针，支持TIM1-TIM5, TIM8-TIM14（其中TIM6/TIM7除外）
  * @retval uint32_t: 返回通道1捕获/比较寄存器(CCR1)的当前值
  * @note   在输入捕获模式下返回最后捕获的计数器值
  *         在PWM输入模式下可测量频率/占空比
  *         必须使用支持通道1的TIM外设实例
  */
uint32_t TIM_GetCapture1(TIM_TypeDef* TIMx)
{
  /* 参数有效性检查：验证TIMx是否属于支持通道1的定时器（TIM1-TIM5, TIM8-TIM14，排除TIM6/TIM7） */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));

  /* 直接读取CCR1寄存器值：该寄存器存储输入捕获事件发生时计数器的值，或比较匹配时的预装载值 */
  return TIMx->CCR1;  // 返回32位无符号整型的寄存器值
}

/**
  * @brief  获取TIMx输入通道2的捕获值
  * @param  TIMx: 定时器外设指针，支持TIM1-TIM5, TIM8, TIM9, TIM12
  * @retval uint32_t: 返回通道2捕获/比较寄存器(CCR2)的当前值
  * @note   适用于：
  *         - 测量脉冲宽度（配合通道1使用）
  *         - 生成PWM输出时的当前比较值
  */
uint32_t TIM_GetCapture2(TIM_TypeDef* TIMx)
{
  /* 参数有效性检查：验证TIMx是否属于支持通道2的定时器（TIM1-TIM5, TIM8, TIM9, TIM12） */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));

  /* 读取CCR2寄存器：该寄存器的物理地址偏移量为0x34（相对于TIMx基地址） */
  return TIMx->CCR2;  // 返回寄存器原始二进制数值
}

/**
  * @brief  获取TIMx输入通道3的捕获值
  * @param  TIMx: 定时器外设指针，支持TIM1-TIM5, TIM8
  * @retval uint32_t: 返回通道3捕获/比较寄存器(CCR3)的当前值
  * @note   典型应用场景：
  *         - 三相电机控制
  *         - 多通道数据采集时序控制
  */
uint32_t TIM_GetCapture3(TIM_TypeDef* TIMx)
{
  /* 参数有效性检查：验证TIMx是否属于支持通道3的定时器（TIM1-TIM5, TIM8） */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx)); 

  /* 读取CCR3寄存器：该寄存器具有影子寄存器特性，实际可能根据TIMx_CCMR2寄存器的OC3M位配置工作在不同模式 */
  return TIMx->CCR3;  // 返回值范围0x0000 0000 - 0xFFFF FFFF
}

/**
  * @brief  获取TIMx输入通道4的捕获值
  * @param  TIMx: 定时器外设指针，支持TIM1-TIM5, TIM8
  * @retval uint32_t: 返回通道4捕获/比较寄存器(CCR4)的当前值
  * @note   特殊功能：
  *         - 可用于触发ADC转换
  *         - 支持DMA传输捕获结果
  */
uint32_t TIM_GetCapture4(TIM_TypeDef* TIMx)
{
  /* 参数有效性检查：验证TIMx是否属于支持通道4的定时器（与通道3支持范围相同） */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));

  /* 读取CCR4寄存器：注意该寄存器在不同定时器中的可用性差异（如TIM2没有CCR4寄存器） */
  return TIMx->CCR4;  // 返回值在边沿对齐模式下为0-ARR值，中心对齐模式下为0-2*ARR值
}

/**
  * @brief  配置输入通道1的预分频器（用于降低捕获事件频率）
  * @param  TIMx: 定时器外设指针，支持TIM1-TIM5, TIM8-TIM14（排除TIM6/TIM7）
  * @param  TIM_ICPSC: 预分频系数，可选值：
  *            @arg TIM_ICPSC_DIV1: 每个有效边沿都触发捕获（无分频）
  *            @arg TIM_ICPSC_DIV2: 每2个有效边沿触发一次捕获
  *            @arg TIM_ICPSC_DIV4: 每4个有效边沿触发一次捕获
  *            @arg TIM_ICPSC_DIV8: 每8个有效边沿触发一次捕获
  * @retval 无
  * @note   预分频器作用于输入滤波后的信号，用于处理高频信号时降低捕获频率
  */
void TIM_SetIC1Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* 双重参数校验：验证TIM实例有效性 + 分频系数合法性 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));  // 检查是否支持通道1的定时器
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICPSC));  // 检查分频值是否在0x0000-0x0003之间

  /* 清除原配置：CCMR1寄存器的IC1PSC位域（第2-3bit） */
  TIMx->CCMR1 &= (uint16_t)~TIM_CCMR1_IC1PSC;  // 使用位掩码0xFFF3清零操作

  /* 写入新配置：将分频系数写入IC1PSC位域 */
  TIMx->CCMR1 |= TIM_ICPSC;  // 直接赋值模式，因为IC1PSC位于寄存器低8位
}

/**
  * @brief  配置输入通道2的预分频器（支持级联配置）
  * @param  TIMx: 定时器外设指针，支持TIM1-TIM5, TIM8, TIM9, TIM12
  * @param  TIM_ICPSC: 预分频系数（同通道1参数）
  * @retval 无
  * @note   特别说明：
  *         - 与通道1共享CCMR1寄存器但位于高8位
  *         - 可配合通道1实现双边沿捕获
  */
void TIM_SetIC2Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* 参数双重验证：检查TIM实例是否支持通道2 + 分频值合法性 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));  // 验证TIMx是否在TIM1-TIM5,8,9,12范围内
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICPSC));

  /* 清除原有配置：CCMR1寄存器的IC2PSC位域（第10-11bit） */
  TIMx->CCMR1 &= (uint16_t)~TIM_CCMR1_IC2PSC;  // 位掩码0xFCFF（二进制1111110011111111）

  /* 设置新分频值：需要左移8位写入高字节区域 */
  TIMx->CCMR1 |= (uint16_t)(TIM_ICPSC << 8);  // 左移操作使数值对齐到IC2PSC位域
}

/**
  * @brief  配置输入通道3的预分频器（适用复杂波形分析）
  * @param  TIMx: 定时器外设指针，支持TIM1-TIM5, TIM8
  * @param  TIM_ICPSC: 预分频系数（同前序参数）
  * @retval 无
  * @note   硬件特性：
  *         - 使用CCMR2寄存器的低8位区域
  *         - 与通道4配置共享同一寄存器
  */
void TIM_SetIC3Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* 参数有效性验证：检查TIM实例是否支持通道3 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));  // 验证TIM1-TIM5,8的合法性
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICPSC));

  /* 清除CCMR2寄存器的IC3PSC位域（第2-3bit） */
  TIMx->CCMR2 &= (uint16_t)~TIM_CCMR2_IC3PSC;  // 掩码0xFFF3（二进制1111111111110011）

  /* 写入新的预分频配置值 */
  TIMx->CCMR2 |= TIM_ICPSC;  // 直接赋值到寄存器低8位区域
}

/**
  * @brief  配置输入通道4的预分频器（支持高频信号处理）
  * @param  TIMx: 定时器外设指针，支持TIM1-TIM5, TIM8
  * @param  TIM_ICPSC: 预分频系数（同其他通道参数）
  * @retval 无
  * @note   特殊说明：
  *         - 配置位位于CCMR2寄存器的高8位
  *         - 需要特别注意TIM2/TIM5等高级定时器的可用性
  */
void TIM_SetIC4Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{  
  /* 参数校验：验证TIM实例是否支持通道4 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));  // 使用与通道3相同的校验宏
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICPSC));

  /* 清除IC4PSC位域（第10-11bit）的原有配置 */
  TIMx->CCMR2 &= (uint16_t)~TIM_CCMR2_IC4PSC;  // 掩码0xFCFF（二进制1111110011111111）

  /* 设置新值：需要左移8位写入高字节区域 */
  TIMx->CCMR2 |= (uint16_t)(TIM_ICPSC << 8);  // 左移8位对齐到寄存器高8位区域
}

/**
  * @}
  */

/** @defgroup TIM_Group4 Advanced-control timers (TIM1 and TIM8) specific features
 *  @brief   Advanced-control timers (TIM1 and TIM8) specific features
 *
@verbatim   
 ===============================================================================
      ##### Advanced-control timers (TIM1 and TIM8) specific features #####
 ===============================================================================  
        
             ##### TIM Driver: how to use the Break feature #####
 ===============================================================================
    [..] 
    After configuring the Timer channel(s) in the appropriate Output Compare mode: 
                         
      (#) Fill the TIM_BDTRInitStruct with the desired parameters for the Timer
          Break Polarity, dead time, Lock level, the OSSI/OSSR State and the 
          AOE(automatic output enable).
               
      (#) Call TIM_BDTRConfig(TIMx, &TIM_BDTRInitStruct) to configure the Timer
          
      (#) Enable the Main Output using TIM_CtrlPWMOutputs(TIM1, ENABLE) 
          
      (#) Once the break even occurs, the Timer's output signals are put in reset
          state or in a known state (according to the configuration made in
          TIM_BDTRConfig() function).

@endverbatim
  * @{
  */
/**
  * @brief  配置刹车与死区功能（适用于电机控制应用）
  * @param  TIMx: 定时器指针，仅限高级定时器TIM1/TIM8
  * @param  TIM_BDTRInitStruct: 刹车与死区配置结构体指针，包含以下成员：
  *         - TIM_OSSRState: 运行模式下的关闭状态选择
  *         - TIM_OSSIState: 空闲模式下的关闭状态选择
  *         - TIM_LOCKLevel: 寄存器锁级别（防止意外修改）
  *         - TIM_DeadTime: 死区时间值（0-0xFF，单位=tdts）
  *         - TIM_Break: 刹车输入使能
  *         - TIM_BreakPolarity: 刹车信号有效极性
  *         - TIM_AutomaticOutput: 自动输出使能
  * @retval 无
  * @warning 此配置直接影响功率器件安全，需严格计算死区时间和保护逻辑
  */
void TIM_BDTRConfig(TIM_TypeDef* TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct)
{
  /* 参数验证链式检查 */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));        // 验证TIM1/TIM8高级定时器
  assert_param(IS_TIM_OSSR_STATE(TIM_BDTRInitStruct->TIM_OSSRState));      // 检查OSSR状态参数合法性
  assert_param(IS_TIM_OSSI_STATE(TIM_BDTRInitStruct->TIM_OSSIState));      // 检查OSSI状态参数合法性
  assert_param(IS_TIM_LOCK_LEVEL(TIM_BDTRInitStruct->TIM_LOCKLevel));      // 检查锁级别[0-3]
  assert_param(IS_TIM_BREAK_STATE(TIM_BDTRInitStruct->TIM_Break));         // 验证刹车使能状态
  assert_param(IS_TIM_BREAK_POLARITY(TIM_BDTRInitStruct->TIM_BreakPolarity)); // 检查刹车极性（高/低有效）
  assert_param(IS_TIM_AUTOMATIC_OUTPUT_STATE(TIM_BDTRInitStruct->TIM_AutomaticOutput)); // 自动输出模式验证

  /* 整合所有配置到BDTR寄存器（32位配置）:
     BIT[15:8]  = 死区时间值（DTG[7:0]） 
     BIT[7]     = 自动输出使能(AOE)
     BIT[6]     = 刹车极性(BKP)
     BIT[5]     = 刹车使能(BKE)
     BIT[4]     = 关闭状态运行模式选择(OSSR)
     BIT[3]     = 关闭状态空闲模式选择(OSSI)
     BIT[2:1]   = 锁电平配置
     BIT[0]     : 保留位 */
  TIMx->BDTR = (uint32_t)TIM_BDTRInitStruct->TIM_OSSRState | TIM_BDTRInitStruct->TIM_OSSIState |
             TIM_BDTRInitStruct->TIM_LOCKLevel | TIM_BDTRInitStruct->TIM_DeadTime |
             TIM_BDTRInitStruct->TIM_Break | TIM_BDTRInitStruct->TIM_BreakPolarity |
             TIM_BDTRInitStruct->TIM_AutomaticOutput;  // 位或组合各配置参数
}

/**
  * @brief  初始化BDTR配置结构体为安全默认值
  * @param  TIM_BDTRInitStruct: 待初始化的刹车死区配置结构体指针
  * @retval 无
  * @note   默认配置特点：
  *         - 关闭所有保护功能
  *         - 死区时间设为0
  *         - 低电平有效刹车信号
  *         - 寄存器锁级别最低
  */
void TIM_BDTRStructInit(TIM_BDTRInitTypeDef* TIM_BDTRInitStruct)
{
  /* 安全初始化策略 */
  TIM_BDTRInitStruct->TIM_OSSRState = TIM_OSSRState_Disable;    // OSSR功能关闭
  TIM_BDTRInitStruct->TIM_OSSIState = TIM_OSSIState_Disable;    // OSSI功能关闭
  TIM_BDTRInitStruct->TIM_LOCKLevel = TIM_LOCKLevel_OFF;       // 无寄存器锁定保护
  TIM_BDTRInitStruct->TIM_DeadTime = 0x00;                      // 死区时间清零
  TIM_BDTRInitStruct->TIM_Break = TIM_Break_Disable;            // 禁用刹车输入
  TIM_BDTRInitStruct->TIM_BreakPolarity = TIM_BreakPolarity_Low; // 默认低电平触发刹车
  TIM_BDTRInitStruct->TIM_AutomaticOutput = TIM_AutomaticOutput_Disable; // 禁用自动输出
}

/**
  * @brief  使能/禁用PWM主输出（控制功率器件的关键开关）
  * @param  TIMx: 定时器指针，仅限TIM1/TIM8
  * @param  NewState: 新状态（ENABLE/DISABLE）
  * @retval 无
  * @warning MOE位会在以下情况自动清零：
  *         - 刹车事件发生
  *         - 锁电平错误
  *         - 系统复位
  */
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* 高级定时器有效性验证 */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));      // 仅允许TIM1/TIM8
  assert_param(IS_FUNCTIONAL_STATE(NewState));   // 参数有效性检查

  if (NewState != DISABLE)
  {
    /* 使能主输出：置位BDTR寄存器MOE位（BIT[15]） */
    TIMx->BDTR |= TIM_BDTR_MOE;  // 0x8000，使能所有PWM通道输出
  }
  else
  {
    /* 紧急关闭输出：清除MOE位（硬件级快速关断） */
    TIMx->BDTR &= (uint16_t)~TIM_BDTR_MOE;  // 强制所有输出进入无效状态
  }  
}

/**
  * @brief  控制换相事件触发（适用于三相电机驱动）
  * @param  TIMx: 定时器指针，仅限TIM1/TIM8
  * @param  NewState: 换相事件触发状态（ENABLE=允许换相触发）
  * @retval 无
  * @note   换相事件将：
  *         - 更新CCR寄存器值
  *         - 触发中断/DMA请求
  *         - 重置计数器分频
  */
void TIM_SelectCOM(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* 参数双重验证 */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));      // 仅高级定时器有换相功能
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 使能换相事件：置位CR2寄存器CCUS位（BIT[2]） */
    TIMx->CR2 |= TIM_CR2_CCUS;  // 允许通过COM事件更新CCR寄存器
  }
  else
  {
    /* 禁用换相事件：清除CCUS位 */
    TIMx->CR2 &= (uint16_t)~TIM_CR2_CCUS;  // 正常模式下禁止COM事件更新
  }
}

/**
  * @brief  使能/禁用捕获比较通道的预装载功能（消除PWM毛刺的关键配置）
  * @param  TIMx: 定时器指针，仅限高级定时器TIM1/TIM8
  * @param  NewState: 新状态（ENABLE=启用预装载，DISABLE=直接写入生效）
  * @retval 无
  * @note   工作机制：
  *         - 启用时：CCR寄存器的修改在下一个更新事件生效
  *         - 禁用时：CCR寄存器立即生效，可能产生输出抖动
  * @warning 必须配合TIM_ARRPreloadConfig()使用才能保证同步更新
  */
void TIM_CCPreloadControl(TIM_TypeDef* TIMx, FunctionalState NewState)
{ 
  /* 双重安全验证 */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));       // 验证TIM1/TIM8合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));    // 检查状态参数有效性

  if (NewState != DISABLE)
  {
    /* 启用预装载机制：置位CR2寄存器CCPC位（BIT[0]） */
    TIMx->CR2 |= TIM_CR2_CCPC;  // 0x0001，使能所有CCRx寄存器的缓冲功能
  }
  else
  {
    /* 禁用预装载：清除CCPC位（立即写入模式） */
    TIMx->CR2 &= (uint16_t)~TIM_CR2_CCPC;  // 0xFFFE，关闭寄存器缓冲
  }
}

/**
  * @}
  */

/** @defgroup TIM_Group5 Interrupts DMA and flags management functions
 *  @brief    Interrupts, DMA and flags management functions 
 *
@verbatim   
 ===============================================================================
          ##### Interrupts, DMA and flags management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified TIM interrupts.
  * @param  TIMx: where x can be 1 to 14 to select the TIMx peripheral.
  * @param  TIM_IT: specifies the TIM interrupts sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg TIM_IT_Update: TIM update Interrupt source
  *            @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *            @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *            @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *            @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *            @arg TIM_IT_COM: TIM Commutation Interrupt source
  *            @arg TIM_IT_Trigger: TIM Trigger Interrupt source
  *            @arg TIM_IT_Break: TIM Break Interrupt source
  *  
  * @note   For TIM6 and TIM7 only the parameter TIM_IT_Update can be used
  * @note   For TIM9 and TIM12 only one of the following parameters can be used: TIM_IT_Update,
  *          TIM_IT_CC1, TIM_IT_CC2 or TIM_IT_Trigger. 
  * @note   For TIM10, TIM11, TIM13 and TIM14 only one of the following parameters can
  *          be used: TIM_IT_Update or TIM_IT_CC1   
  * @note   TIM_IT_COM and TIM_IT_Break can be used only with TIM1 and TIM8 
  *        
  * @param  NewState: new state of the TIM interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  定时器中断源配置（控制特定事件的中断触发）
  * @param  TIMx: 定时器实例，支持TIM1-TIM14全系列
  * @param  TIM_IT: 中断类型，可选值：
  *            @arg TIM_IT_Update   更新中断
  *            @arg TIM_IT_CC1/2/3/4 捕获比较通道中断
  *            @arg TIM_IT_Trigger 触发中断
  *            @arg TIM_IT_Break   刹车中断(仅TIM1/8)
  * @param  NewState: 中断使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   实际中断使能需要配合NVIC配置
  * @warning 高频率中断可能引发系统负载问题
  */
void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState)
{  
  /* 三重参数验证 */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));       // 验证TIM1-TIM14有效性
  assert_param(IS_TIM_IT(TIM_IT));             // 检查中断类型合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 状态参数校验
  
  if (NewState != DISABLE)
  {
    /* 使能指定中断源：置位DIER寄存器对应位 */
    TIMx->DIER |= TIM_IT;  // 位或操作，不影响其他中断使能状态
  }
  else
  {
    /* 禁用中断源：清除DIER寄存器对应位 */
    TIMx->DIER &= (uint16_t)~TIM_IT;  // 位与取反操作实现位清除
  }
}

/**
  * @brief  软件触发定时器事件（强制生成特定硬件事件）
  * @param  TIMx: 定时器实例，支持TIM1-TIM14
  * @param  TIM_EventSource: 事件类型，可组合以下值：
  *            @arg TIM_EventSource_Update   生成更新事件
  *            @arg TIM_EventSource_CC1/2/3/4 生成捕获比较事件
  *            @arg TIM_EventSource_COM      换相事件(仅TIM1/8)
  *            @arg TIM_EventSource_Trigger  触发事件
  *            @arg TIM_EventSource_Break   刹车事件(仅TIM1/8)
  * @retval 无
  * @note   该操作将立即生效，常用于：
  *         - 强制计数器复位
  *         - 手动触发ADC同步
  *         - 测试中断服务程序
  */
void TIM_GenerateEvent(TIM_TypeDef* TIMx, uint16_t TIM_EventSource)
{ 
  /* 双重安全验证 */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));          // 检查定时器有效性
  assert_param(IS_TIM_EVENT_SOURCE(TIM_EventSource)); // 验证事件源合法性

  /* 写入事件生成寄存器：直接赋值将立即触发事件 */
  TIMx->EGR = TIM_EventSource;  // EGR寄存器写1清0机制，需直接赋值事件位
}


/**
  * @brief  Checks whether the specified TIM flag is set or not.
  * @param  TIMx: where x can be 1 to 14 to select the TIM peripheral.
  * @param  TIM_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg TIM_FLAG_Update: TIM update Flag
  *            @arg TIM_FLAG_CC1: TIM Capture Compare 1 Flag
  *            @arg TIM_FLAG_CC2: TIM Capture Compare 2 Flag
  *            @arg TIM_FLAG_CC3: TIM Capture Compare 3 Flag
  *            @arg TIM_FLAG_CC4: TIM Capture Compare 4 Flag
  *            @arg TIM_FLAG_COM: TIM Commutation Flag
  *            @arg TIM_FLAG_Trigger: TIM Trigger Flag
  *            @arg TIM_FLAG_Break: TIM Break Flag
  *            @arg TIM_FLAG_CC1OF: TIM Capture Compare 1 over capture Flag
  *            @arg TIM_FLAG_CC2OF: TIM Capture Compare 2 over capture Flag
  *            @arg TIM_FLAG_CC3OF: TIM Capture Compare 3 over capture Flag
  *            @arg TIM_FLAG_CC4OF: TIM Capture Compare 4 over capture Flag
  *
  * @note   TIM6 and TIM7 can have only one update flag. 
  * @note   TIM_FLAG_COM and TIM_FLAG_Break are used only with TIM1 and TIM8.    
  *
  * @retval The new state of TIM_FLAG (SET or RESET).
  */
/**
  * @brief  获取定时器状态标志位（轮询方式检测硬件事件）
  * @param  TIMx: 定时器实例，支持TIM1-TIM14全系列
  * @param  TIM_FLAG: 待查询的标志位，可选值：
  *            @arg TIM_FLAG_Update   更新事件标志
  *            @arg TIM_FLAG_CC1/2/3/4 捕获比较标志
  *            @arg TIM_FLAG_Trigger  触发事件标志
  *            @arg TIM_FLAG_Break    刹车事件标志(仅TIM1/8)
  *            @arg TIM_FLAG_COM      换相事件标志(仅TIM1/8)
  * @retval FlagStatus: 标志位状态(SET=事件发生，RESET=无事件)
  * @note   部分标志位会自动清除，部分需要手动清除
  * @warning 在中断服务中应优先使用中断标志查询函数
  */
FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, uint16_t TIM_FLAG)
{ 
  ITStatus bitstatus = RESET;  // 初始化状态为未触发
  
  /* 双重安全验证 */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));        // 确认定时器型号有效
  assert_param(IS_TIM_GET_FLAG(TIM_FLAG));      // 检查标志位参数合法性（0x0001-0x1F1F）

  /* SR寄存器状态检测（注意不同标志位位置）：
     BIT[0]  = 更新事件标志
     BIT[1]  = CC1捕获比较标志
     BIT[2]  = CC2捕获比较标志
     BIT[3]  = CC3捕获比较标志
     BIT[4]  = CC4捕获比较标志
     BIT[6]  = 触发事件标志
     BIT[7]  = 刹车输入标志
     BIT[9]  = 换相事件标志 */
  if ((TIMx->SR & TIM_FLAG) != (uint16_t)RESET) // 位与操作检测指定标志位
  {
    bitstatus = SET;  // 标志位置位，表示对应事件已发生
  }
  else
  {
    bitstatus = RESET; // 标志位未置位，事件未发生或已清除
  }
  return bitstatus;    // 返回当前标志位状态
}


/**
  * @brief  Clears the TIMx's pending flags.
  * @param  TIMx: where x can be 1 to 14 to select the TIM peripheral.
  * @param  TIM_FLAG: specifies the flag bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM_FLAG_Update: TIM update Flag
  *            @arg TIM_FLAG_CC1: TIM Capture Compare 1 Flag
  *            @arg TIM_FLAG_CC2: TIM Capture Compare 2 Flag
  *            @arg TIM_FLAG_CC3: TIM Capture Compare 3 Flag
  *            @arg TIM_FLAG_CC4: TIM Capture Compare 4 Flag
  *            @arg TIM_FLAG_COM: TIM Commutation Flag
  *            @arg TIM_FLAG_Trigger: TIM Trigger Flag
  *            @arg TIM_FLAG_Break: TIM Break Flag
  *            @arg TIM_FLAG_CC1OF: TIM Capture Compare 1 over capture Flag
  *            @arg TIM_FLAG_CC2OF: TIM Capture Compare 2 over capture Flag
  *            @arg TIM_FLAG_CC3OF: TIM Capture Compare 3 over capture Flag
  *            @arg TIM_FLAG_CC4OF: TIM Capture Compare 4 over capture Flag
  *
  * @note   TIM6 and TIM7 can have only one update flag. 
  * @note   TIM_FLAG_COM and TIM_FLAG_Break are used only with TIM1 and TIM8.
  *    
  * @retval None
  */
/**
  * @brief  清除定时器状态标志位（硬件事件确认后清除）
  * @param  TIMx: 定时器实例，支持TIM1-TIM14全系列
  * @param  TIM_FLAG: 待清除的标志位组合，可同时清除多个标志
  * @retval 无
  * @warning SR寄存器采用写0清除机制，必须使用位取反操作
  */
void TIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG)
{  
  /* 参数有效性验证 */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));  // 检查TIMx是否合法定时器实例
  
  /* SR寄存器写操作特性：
     - 写1无效果
     - 写0清除对应标志位
     - 必须使用位取反方式生成掩码 */
  TIMx->SR = (uint16_t)~TIM_FLAG;  // 示例：清除Update标志 TIM_FLAG=0x0001 → 写入0xFFFE
}

/**
  * @brief  获取有效中断状态（同时验证中断使能与触发状态）
  * @param  TIMx: 定时器实例，支持TIM1-TIM14
  * @param  TIM_IT: 中断类型，可组合以下值：
  *            @arg TIM_IT_Update   更新中断
  *            @arg TIM_IT_CC1/2/3/4 通道中断
  *            @arg TIM_IT_Trigger  触发中断
  *            @arg TIM_IT_Break   刹车中断(仅TIM1/8)
  *            @arg TIM_IT_COM     换相中断(仅TIM1/8)
  * @retval ITStatus: 中断有效状态(SET=已使能且已触发)
  * @note   必须在中断服务函数中使用此函数
  */
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
  ITStatus bitstatus = RESET;  
  uint16_t itstatus = 0x0, itenable = 0x0;
  
  /* 三重安全验证 */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));       // 定时器实例合法性
  assert_param(IS_TIM_GET_IT(TIM_IT));          // 中断类型合法性检查

  /* 获取原始中断触发状态 */
  itstatus = TIMx->SR & TIM_IT;  // 从状态寄存器提取目标标志位
  
  /* 获取中断使能状态 */
  itenable = TIMx->DIER & TIM_IT;  // 从中断使能寄存器获取使能状态

  /* 双重条件判断：中断已触发 且 已使能 */
  if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
  {
    bitstatus = SET;  // 确认有效中断事件
  }
  else
  {
    bitstatus = RESET; // 伪中断或未使能的中断
  }
  return bitstatus;  // 返回经过验证的中断状态
}

/**
  * @brief  清除中断挂起标志（中断服务程序必要操作）
  * @param  TIMx: 定时器实例，支持TIM1-TIM14
  * @param  TIM_IT: 待清除的中断标志组合
  * @retval 无
  * @warning 必须在中断处理完成后调用，否则会导致重复进入中断
  */
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
  /* 参数有效性验证 */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));  // 确认定时器型号合法性

  /* 清除中断挂起标志（同TIM_ClearFlag机制）：
     - 实际清除的是SR寄存器中的事件标志
     - 清除操作必须在中断处理逻辑完成后执行 */
  TIMx->SR = (uint16_t)~TIM_IT;  // 注意此操作会影响所有指定标志位
}

/**
  * @brief  Configures the TIMx's DMA interface.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_DMABase: DMA Base address.
  *          This parameter can be one of the following values:
  *            @arg TIM_DMABase_CR1  
  *            @arg TIM_DMABase_CR2
  *            @arg TIM_DMABase_SMCR
  *            @arg TIM_DMABase_DIER
  *            @arg TIM1_DMABase_SR
  *            @arg TIM_DMABase_EGR
  *            @arg TIM_DMABase_CCMR1
  *            @arg TIM_DMABase_CCMR2
  *            @arg TIM_DMABase_CCER
  *            @arg TIM_DMABase_CNT   
  *            @arg TIM_DMABase_PSC   
  *            @arg TIM_DMABase_ARR
  *            @arg TIM_DMABase_RCR
  *            @arg TIM_DMABase_CCR1
  *            @arg TIM_DMABase_CCR2
  *            @arg TIM_DMABase_CCR3  
  *            @arg TIM_DMABase_CCR4
  *            @arg TIM_DMABase_BDTR
  *            @arg TIM_DMABase_DCR
  * @param  TIM_DMABurstLength: DMA Burst length. This parameter can be one value
  *         between: TIM_DMABurstLength_1Transfer and TIM_DMABurstLength_18Transfers.
  * @retval None
  */
/**
  * @brief  配置定时器DMA传输参数（设置DMA访问地址与突发长度）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8
  * @param  TIM_DMABase: DMA访问基地址，可选值：
  *            @arg TIM_DMABase_CTRL1:   控制寄存器1
  *            @arg TIM_DMABase_CTRL2:   控制寄存器2
  *            @arg TIM_DMABase_SMCR:    从模式控制寄存器
  *            @arg TIM_DMABase_DIER:    中断使能寄存器
  *            @arg TIM_DMABase_SR:      状态寄存器
  *            @arg TIM_DMABase_EGR:     事件生成寄存器
  *            @arg TIM_DMABase_CCMR1:   通道1/2模式寄存器
  *            @arg TIM_DMABase_CCMR2:   通道3/4模式寄存器
  *            @arg TIM_DMABase_CCER:    捕获比较使能寄存器
  *            @arg TIM_DMABase_CNT:     计数器寄存器
  *            @arg TIM_DMABase_PSC:     预分频寄存器
  *            @arg TIM_DMABase_ARR:     自动重载寄存器
  *            @arg TIM_DMABase_CCR1:    通道1比较寄存器
  *            @arg TIM_DMABase_CCR2:    通道2比较寄存器
  *            @arg TIM_DMABase_CCR3:    通道3比较寄存器
  *            @arg TIM_DMABase_CCR4:    通道4比较寄存器
  *            @arg TIM_DMABase_DCR:     DMA控制寄存器（本配置）
  *            @arg TIM_DMABase_BDTR:    刹车死区寄存器
  * @param  TIM_DMABurstLength: DMA突发传输长度（1-18次传输）
  * @retval 无
  * @note   实际DMA传输地址 = TIM_DMABase值 * 2 + TIMx基地址
  */
void TIM_DMAConfig(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength)
{
  /* 三重参数验证 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));          // 验证TIM1-TIM5,8合法性
  assert_param(IS_TIM_DMA_BASE(TIM_DMABase));        // 检查基地址参数范围(0x0000-0x0044)
  assert_param(IS_TIM_DMA_LENGTH(TIM_DMABurstLength)); // 突发长度范围检查(0x0000-0x0012)

  /* 配置DCR寄存器：
     BIT[4:0] = DMABase（目标寄存器偏移地址/2）
     BIT[7:5] = 保留位
     BIT[12:8] = DBL（突发传输长度-1）*/
  TIMx->DCR = TIM_DMABase | TIM_DMABurstLength;  // 组合配置值写入DMA控制寄存器
}

/**
  * @brief  使能/禁用定时器DMA请求源（触发DMA传输）
  * @param  TIMx: 定时器实例，支持TIM1-TIM8
  * @param  TIM_DMASource: DMA请求源，可组合以下值：
  *            @arg TIM_DMA_Update:   更新事件触发DMA
  *            @arg TIM_DMA_CC1/2/3/4: 通道捕获比较触发DMA
  *            @arg TIM_DMA_COM:      换相事件触发DMA（仅TIM1/8）
  *            @arg TIM_DMA_Trigger:  触发事件触发DMA
  * @param  NewState: 使能状态(ENABLE/DISABLE)
  * @retval 无
  * @note   实际DMA通道需要额外配置DMA控制器
  */
void TIM_DMACmd(TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState)
{ 
  /* 三重安全验证 */
  assert_param(IS_TIM_LIST5_PERIPH(TIMx));        // 验证TIM1-TIM8合法性
  assert_param(IS_TIM_DMA_SOURCE(TIM_DMASource));  // 检查DMA请求源合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));     // 状态参数校验

  if (NewState != DISABLE)
  {
    /* 使能DMA请求：置位DIER寄存器对应位（与中断使能共用寄存器） */
    TIMx->DIER |= TIM_DMASource;  // 例如：TIM_DMA_CC1对应DIER[1]
  }
  else
  {
    /* 禁用DMA请求：清除DIER寄存器对应位 */
    TIMx->DIER &= (uint16_t)~TIM_DMASource; // 位掩码取反操作
  }
}

/**
  * @brief  选择捕获比较DMA触发时机（更新事件/立即触发）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8
  * @param  NewState: 触发时机选择：
  *            ENABLE - 仅在更新事件时触发DMA
  *            DISABLE - 捕获比较事件立即触发DMA
  * @retval 无
  * @note   影响CCRx寄存器的DMA传输时机
  */
void TIM_SelectCCDMA(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* 参数双重验证 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));       // 验证TIM1-TIM5,8合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState));    // 状态参数校验

  if (NewState != DISABLE)
  {
    /* 设置CCDS位：DMA请求延迟到更新事件 */
    TIMx->CR2 |= TIM_CR2_CCDS;  // 置位CR2[3]，用于同步多个DMA传输
  }
  else
  {
    /* 清除CCDS位：立即触发DMA请求 */
    TIMx->CR2 &= (uint16_t)~TIM_CR2_CCDS; // 适用于实时性要求高的场景
  }
}

/**
  * @}
  */

/** @defgroup TIM_Group6 Clocks management functions
 *  @brief    Clocks management functions
 *
@verbatim   
 ===============================================================================
                  ##### Clocks management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  配置定时器使用内部时钟源（默认时钟配置）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8,9,12
  * @retval 无
  * @note   通过清除从模式控制寄存器(SMCR)的SMS位，
  *         使定时器工作在主模式，时钟直接来自内部APB总线
  */
void TIM_InternalClockConfig(TIM_TypeDef* TIMx)
{
  /* 验证定时器型号合法性 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));  // 检查TIM1,2,3,4,5,8,9,12

  /* 清除从模式控制寄存器SMS位域（BIT[2:0]）
     设置为0x0000表示禁用所有从模式，使用内部时钟 */
  TIMx->SMCR &=  (uint16_t)~TIM_SMCR_SMS;  // 位掩码0xFFF8
}

/**
  * @brief  配置定时器使用内部触发源作为外部时钟（级联定时器）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8,9,12
  * @param  TIM_InputTriggerSource: 内部触发源，可选：
  *            @arg TIM_TS_ITR0: 来自定时器1的触发信号
  *            @arg TIM_TS_ITR1: 来自定时器2的触发信号 
  *            @arg TIM_TS_ITR2: 来自定时器3的触发信号
  *            @arg TIM_TS_ITR3: 来自定时器4的触发信号
  * @retval 无
  * @note   典型应用场景：定时器级联，主定时器触发从定时器计数
  */
void TIM_ITRxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource)
{
  /* 参数双重验证 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));  // 验证TIM1-TIM5,8,9,12
  assert_param(IS_TIM_INTERNAL_TRIGGER_SELECTION(TIM_InputTriggerSource)); // 检查触发源参数

  /* 配置触发源（设置SMCR寄存器的TS位域）：
     BIT[6:4] = 触发源选择（ITR0-ITR3对应0x0000-0x0030） */
  TIM_SelectInputTrigger(TIMx, TIM_InputTriggerSource);  // 内部函数设置TS位

  /* 启用外部时钟模式1：
     设置SMCR的SMS位域为0x0007（二进制111）*/
  TIMx->SMCR |= TIM_SlaveMode_External1;  // 0x0007，时钟来自选定的触发输入
}

/**
  * @brief  配置外部引脚输入作为定时器时钟源（通过输入捕获通道）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5,8,9,10,11,12,13,14
  * @param  TIM_TIxExternalCLKSource: 外部时钟输入源
  *            @arg TIM_TIxExternalCLK1Source_TI1ED: TI1边沿检测（无需预分频）
  *            @arg TIM_TIxExternalCLK1Source_TI1: 滤波后的TI1输入
  *            @arg TIM_TIxExternalCLK1Source_TI2: 滤波后的TI2输入
  * @param  TIM_ICPolarity: 输入极性（上升沿/下降沿触发计数）
  * @param  ICFilter: 数字滤波器系数（0x0-0xF对应1-16个时钟周期）
  * @retval 无
  * @warning 外部时钟频率必须小于定时器时钟的1/4（考虑滤波器延迟）
  */
void TIM_TIxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_TIxExternalCLKSource,
                                uint16_t TIM_ICPolarity, uint16_t ICFilter)
{
  /* 参数四重验证 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));        // 检查更广泛的定时器支持列表
  assert_param(IS_TIM_IC_POLARITY(TIM_ICPolarity));  // 极性参数检查（0x0000/0x0004）
  assert_param(IS_TIM_IC_FILTER(ICFilter));        // 滤波器值范围检查（0x0000-0x000F）

  /* 根据输入源选择配置TI1或TI2通道 */
  if (TIM_TIxExternalCLKSource == TIM_TIxExternalCLK1Source_TI2)  // 0x0060
  {
    /* 配置TI2通道参数：
       - 输入极性
       - 直连模式（不交叉）
       - 滤波器值 */
    TI2_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter);  // 0x0001
  }
  else  // 默认处理TI1相关配置
  {
    /* 配置TI1通道参数：
       - 边沿检测或滤波模式由TIM_TIxExternalCLKSource决定 */
    TI1_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter);
  }

  /* 选择外部时钟触发源（设置SMCR的TS位域） */
  TIM_SelectInputTrigger(TIMx, TIM_TIxExternalCLKSource); 

  /* 启用外部时钟模式1（设置SMCR的SMS位域为0x0007） */
  TIMx->SMCR |= TIM_SlaveMode_External1;  // 时钟来自选定的TIx输入
}

/**
  * @brief  Configures the External clock Mode1
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ExtTRGPrescaler: The external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM_ExtTRGPSC_OFF: ETRP Prescaler OFF.
  *            @arg TIM_ExtTRGPSC_DIV2: ETRP frequency divided by 2.
  *            @arg TIM_ExtTRGPSC_DIV4: ETRP frequency divided by 4.
  *            @arg TIM_ExtTRGPSC_DIV8: ETRP frequency divided by 8.
  * @param  TIM_ExtTRGPolarity: The external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM_ExtTRGPolarity_Inverted: active low or falling edge active.
  *            @arg TIM_ExtTRGPolarity_NonInverted: active high or rising edge active.
  * @param  ExtTRGFilter: External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
/**
  * @brief  配置外部时钟模式1（ETR引脚作为独立时钟源）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8
  * @param  TIM_ExtTRGPrescaler: ETR预分频系数，可选：
  *            @arg TIM_ExtTRGPSC_OFF:   无分频
  *            @arg TIM_ExtTRGPSC_DIV2: 2分频
  *            @arg TIM_ExtTRGPSC_DIV4: 4分频
  *            @arg TIM_ExtTRGPSC_DIV8: 8分频
  * @param  TIM_ExtTRGPolarity: ETR信号极性
  *            @arg TIM_ExtTRGPolarity_Inverted:   下降沿有效
  *            @arg TIM_ExtTRGPolarity_NonInverted:上升沿有效
  * @param  ExtTRGFilter: 数字滤波器系数（0x0-0xF对应1-16个时钟周期）
  * @retval 无
  * @note   模式特点：
  *         - ETR引脚专门作为时钟输入
  *         - 每个有效边沿触发计数器递增
  */
void TIM_ETRClockMode1Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
                            uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter)
{
  uint16_t tmpsmcr = 0;

  /* 四重参数验证 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));            // 检查TIM1-TIM5,8合法性
  assert_param(IS_TIM_EXT_PRESCALER(TIM_ExtTRGPrescaler)); // 预分频参数检查
  assert_param(IS_TIM_EXT_POLARITY(TIM_ExtTRGPolarity));    // 极性参数检查
  assert_param(IS_TIM_EXT_FILTER(ExtTRGFilter));            // 滤波器值范围验证

  /* 配置ETR通道参数（设置SMCR的ETP、ETPS、ETF位）：
     - 写入预分频、极性和滤波器配置 */
  TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);
  
  /* 获取当前SMCR寄存器值以进行位操作 */
  tmpsmcr = TIMx->SMCR;

  /* 清除SMS位域（BIT[2:0]），准备设置从模式 */
  tmpsmcr &= (uint16_t)~TIM_SMCR_SMS;  // 掩码0xFFF8

  /* 设置为外部时钟模式1（SMS=111） */
  tmpsmcr |= TIM_SlaveMode_External1;  // 0x0007

  /* 清除TS位域（BIT[6:4]），准备设置触发源 */
  tmpsmcr &= (uint16_t)~TIM_SMCR_TS;  // 掩码0xFF8F

  /* 设置触发源为ETR引脚（TS=111） */
  tmpsmcr |= TIM_TS_ETRF;              // 0x0070

  /* 将配置写入SMCR寄存器 */
  TIMx->SMCR = tmpsmcr;  // 最终SMCR值包含：SMS=111, TS=111, ECE=0
}

/**
  * @brief  配置外部时钟模式2（ETR引脚同时用于触发和时钟）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8
  * @param  TIM_ExtTRGPrescaler: ETR预分频系数（同模式1）
  * @param  TIM_ExtTRGPolarity: ETR信号极性（同模式1）
  * @param  ExtTRGFilter: 滤波器系数（同模式1）
  * @retval 无
  * @note   模式特点：
  *         - ETR信号可同时作为触发输入和时钟源
  *         - 需要外部触发信号来驱动计数器
  *         - 适用于需要时钟与外部事件同步的场景
  */
void TIM_ETRClockMode2Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
                             uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter)
{
  /* 四重参数验证（同模式1） */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_EXT_PRESCALER(TIM_ExtTRGPrescaler));
  assert_param(IS_TIM_EXT_POLARITY(TIM_ExtTRGPolarity));
  assert_param(IS_TIM_EXT_FILTER(ExtTRGFilter));

  /* 配置ETR通道参数（与模式1相同） */
  TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);

  /* 启用外部时钟模式2：置位SMCR的ECE位（BIT[14]） */
  TIMx->SMCR |= TIM_SMCR_ECE;  // 0x4000
}

/**
  * @}
  */

/** @defgroup TIM_Group7 Synchronization management functions
 *  @brief    Synchronization management functions 
 *
@verbatim   
 ===============================================================================
                ##### Synchronization management functions #####
 ===============================================================================  
                         
          ##### TIM Driver: how to use it in synchronization Mode #####
 ===============================================================================
    [..] 
    
    *** Case of two/several Timers ***
    ==================================
    [..]
      (#) Configure the Master Timers using the following functions:
        (++) void TIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource); 
        (++) void TIM_SelectMasterSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_MasterSlaveMode);  
      (#) Configure the Slave Timers using the following functions: 
        (++) void TIM_SelectInputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);  
        (++) void TIM_SelectSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode); 
          
    *** Case of Timers and external trigger(ETR pin) ***
    ====================================================
    [..]           
      (#) Configure the External trigger using this function:
        (++) void TIM_ETRConfig(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                               uint16_t ExtTRGFilter);
      (#) Configure the Slave Timers using the following functions: 
        (++) void TIM_SelectInputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);  
        (++) void TIM_SelectSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode); 

@endverbatim
  * @{
  */

/**
  * @brief  Selects the Input Trigger source
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13 or 14  
  *         to select the TIM peripheral.
  * @param  TIM_InputTriggerSource: The Input Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM_TS_ITR0: Internal Trigger 0
  *            @arg TIM_TS_ITR1: Internal Trigger 1
  *            @arg TIM_TS_ITR2: Internal Trigger 2
  *            @arg TIM_TS_ITR3: Internal Trigger 3
  *            @arg TIM_TS_TI1F_ED: TI1 Edge Detector
  *            @arg TIM_TS_TI1FP1: Filtered Timer Input 1
  *            @arg TIM_TS_TI2FP2: Filtered Timer Input 2
  *            @arg TIM_TS_ETRF: External Trigger input
  * @retval None
  */
/**
  * @brief  配置定时器触发输入源（设置从模式触发信号来源）
  * @param  TIMx: 定时器实例，支持TIM1-TIM14全系列
  * @param  TIM_InputTriggerSource: 触发源选择，可选：
  *            @arg TIM_TS_ITR0: 内部触发线0（TIM1→TIM2）
  *            @arg TIM_TS_ITR1: 内部触发线1（TIM2→TIM3）
  *            @arg TIM_TS_ITR2: 内部触发线2（TIM3→TIM4）
  *            @arg TIM_TS_ITR3: 内部触发线3（TIM4→TIM5）
  *            @arg TIM_TS_TI1F_ED: TI1边沿检测器
  *            @arg TIM_TS_TI1FP1: 滤波后的TI1输入
  *            @arg TIM_TS_TI2FP2: 滤波后的TI2输入
  *            @arg TIM_TS_ETRF: 外部触发输入
  * @retval 无
  * @note   该配置影响从模式控制器的触发信号来源
  */
void TIM_SelectInputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource)
{
  uint16_t tmpsmcr = 0;

  /* 双重参数验证 */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));               // 验证TIM1-TIM14合法性
  assert_param(IS_TIM_TRIGGER_SELECTION(TIM_InputTriggerSource)); // 触发源合法性检查

  /* 获取当前SMCR寄存器值用于位操作 */
  tmpsmcr = TIMx->SMCR;

  /* 清除TS位域（BIT[6:4]），准备设置新触发源 */
  tmpsmcr &= (uint16_t)~TIM_SMCR_TS;  // 掩码0xFF8F

  /* 设置新的触发源到TS位域 */
  tmpsmcr |= TIM_InputTriggerSource;   // 例如TIM_TS_ITR0对应0x0000

  /* 将配置写回SMCR寄存器 */
  TIMx->SMCR = tmpsmcr;
}

/**
  * @brief  配置主模式触发输出源（TRGO信号生成规则）
  * @param  TIMx: 定时器实例，支持TIM1-TIM8
  * @param  TIM_TRGOSource: 触发输出源选择，可选：
  *            @arg TIM_TRGOSource_Reset: 生成TRGO时复位计数器（调试用）
  *            @arg TIM_TRGOSource_Enable: 计数器使能时触发（CEN信号）
  *            @arg TIM_TRGOSource_Update: 更新事件时触发
  *            @arg TIM_TRGOSource_OC1: 通道1比较匹配时触发
  *            @arg TIM_TRGOSource_OC1Ref: 通道1参考信号触发
  *            @arg TIM_TRGOSource_OC2Ref: 通道2参考信号触发
  *            @arg TIM_TRGOSource_OC3Ref: 通道3参考信号触发
  *            @arg TIM_TRGOSource_OC4Ref: 通道4参考信号触发
  * @retval 无
  * @note   TRGO信号可用于同步其他定时器或触发DAC/ADC
  */
void TIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource)
{
  /* 双重参数验证 */
  assert_param(IS_TIM_LIST5_PERIPH(TIMx));        // 检查TIM1-TIM8合法性
  assert_param(IS_TIM_TRGO_SOURCE(TIM_TRGOSource)); // 触发源合法性检查

  /* 清除CR2寄存器的MMS位域（BIT[6:4]）*/
  TIMx->CR2 &= (uint16_t)~TIM_CR2_MMS;  // 掩码0xFF8F

  /* 设置新的主模式输出源 */
  TIMx->CR2 |=  TIM_TRGOSource;  // 例如TIM_TRGOSource_Update对应0x0000
}

/**
  * @brief  配置定时器从模式（定义如何响应TRGI信号）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8,9,12
  * @param  TIM_SlaveMode: 从模式选择，可选：
  *            @arg TIM_SlaveMode_Reset:  触发信号复位计数器并生成更新事件
  *            @arg TIM_SlaveMode_Gated:   触发信号高电平时使能计数器
  *            @arg TIM_SlaveMode_Trigger: 触发信号上升沿启动计数器
  *            @arg TIM_SlaveMode_External1: 每个触发信号边沿驱动计数器
  * @retval 无
  * @note   必须配合TIM_SelectInputTrigger使用以指定触发源
  */
void TIM_SelectSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode)
{
  /* 双重参数验证 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));        // 检查TIM1-TIM5,8,9,12
  assert_param(IS_TIM_SLAVE_MODE(TIM_SlaveMode));  // 模式参数合法性检查

  /* 清除SMS位域（BIT[2:0]）准备设置从模式 */
  TIMx->SMCR &= (uint16_t)~TIM_SMCR_SMS;  // 掩码0xFFF8

  /* 设置新的从模式 */
  TIMx->SMCR |= TIM_SlaveMode;  // 例如TIM_SlaveMode_Reset对应0x0004
}

/**
  * @brief  使能/禁用主从同步模式（控制定时器级联同步）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8,9,12
  * @param  TIM_MasterSlaveMode: 主从模式状态
  *            @arg TIM_MasterSlaveMode_Enable: 允许主定时器触发从定时器
  *            @arg TIM_MasterSlaveMode_Disable: 禁止主从同步
  * @retval 无
  * @note   启用时，主定时器的TRGO信号可以同步从定时器的操作
  */
void TIM_SelectMasterSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_MasterSlaveMode)
{
  /* 参数双重验证 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));        // 检查TIM1-TIM5,8,9,12合法性
  assert_param(IS_TIM_MSM_STATE(TIM_MasterSlaveMode));  // 模式参数检查

  /* 清除SMCR寄存器的MSM位（BIT[7]） */
  TIMx->SMCR &= (uint16_t)~TIM_SMCR_MSM;  // 掩码0xFF7F

  /* 设置主从模式状态 */
  TIMx->SMCR |= TIM_MasterSlaveMode;  // TIM_MasterSlaveMode_Enable对应0x0080
}

/**
  * @brief  配置外部触发ETR参数（预分频/极性/滤波）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8
  * @param  TIM_ExtTRGPrescaler: ETR预分频系数
  *            @arg TIM_ExtTRGPSC_OFF: 无分频
  *            @arg TIM_ExtTRGPSC_DIV2: 2分频
  *            @arg TIM_ExtTRGPSC_DIV4: 4分频
  *            @arg TIM_ExtTRGPSC_DIV8: 8分频
  * @param  TIM_ExtTRGPolarity: ETR信号有效边沿
  *            @arg TIM_ExtTRGPolarity_Inverted: 下降沿有效
  *            @arg TIM_ExtTRGPolarity_NonInverted: 上升沿有效
  * @param  ExtTRGFilter: 数字滤波器系数（0x0-0xF对应1-16个采样周期）
  * @retval 无
  * @warning 滤波器会引入延迟，需根据信号频率合理配置
  */
void TIM_ETRConfig(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
                   uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter)
{
  uint16_t tmpsmcr = 0;

  /* 四重参数验证 */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));            // 检查TIM1-TIM5,8合法性
  assert_param(IS_TIM_EXT_PRESCALER(TIM_ExtTRGPrescaler));  // 预分频参数检查
  assert_param(IS_TIM_EXT_POLARITY(TIM_ExtTRGPolarity));     // 极性参数检查
  assert_param(IS_TIM_EXT_FILTER(ExtTRGFilter));            // 滤波器参数检查

  /* 获取当前SMCR寄存器值用于位操作 */
  tmpsmcr = TIMx->SMCR;

  /* 清除ETR相关配置位（BIT[15:8]）：
     ETRP分频(ETPS[1:0]) 
     ETR极性(ETP) 
     ETR滤波器(ETF[3:0]) */
  tmpsmcr &= SMCR_ETR_MASK;  // 掩码0x00FF（清除高字节）

  /* 组合配置参数到寄存器高字节：
     BIT[15:12] = 保留位
     BIT[11:10] = ETR预分频（ETPS）
     BIT[9]     = ETR极性（ETP）
     BIT[8]     = 保留位
     BIT[7:4]   = ETR滤波器（ETF）*/
  tmpsmcr |= (uint16_t)(TIM_ExtTRGPrescaler |       // 预分频占位BIT[11:10]
                      TIM_ExtTRGPolarity |          // 极性占位BIT[9]
                      (ExtTRGFilter << 8));          // 滤波器左移8位到BIT[7:4]

  /* 将配置写入SMCR寄存器 */
  TIMx->SMCR = tmpsmcr;
}

/**
  * @}
  */

/** @defgroup TIM_Group8 Specific interface management functions
 *  @brief    Specific interface management functions 
 *
@verbatim   
 ===============================================================================
            ##### Specific interface management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Configures the TIMx Encoder Interface.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9 or 12 to select the TIM 
  *         peripheral.
  * @param  TIM_EncoderMode: specifies the TIMx Encoder Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
  *            @arg TIM_EncoderMode_TI2: Counter counts on TI2FP2 edge depending on TI1FP1 level.
  *            @arg TIM_EncoderMode_TI12: Counter counts on both TI1FP1 and TI2FP2 edges depending
  *                                       on the level of the other input.
  * @param  TIM_IC1Polarity: specifies the IC1 Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM_ICPolarity_Falling: IC Falling edge.
  *            @arg TIM_ICPolarity_Rising: IC Rising edge.
  * @param  TIM_IC2Polarity: specifies the IC2 Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM_ICPolarity_Falling: IC Falling edge.
  *            @arg TIM_ICPolarity_Rising: IC Rising edge.
  * @retval None
  */
/**
  * @brief  配置定时器正交编码器接口（支持增量式编码器接入）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8,9,12
  * @param  TIM_EncoderMode: 编码器模式，可选：
  *            @arg TIM_EncoderMode_TI1: 仅在TI1边沿计数
  *            @arg TIM_EncoderMode_TI2: 仅在TI2边沿计数
  *            @arg TIM_EncoderMode_TI12: 在TI1和TI2边沿计数（正交模式）
  * @param  TIM_IC1Polarity: 通道1输入极性（上升沿/下降沿）
  * @param  TIM_IC2Polarity: 通道2输入极性（上升沿/下降沿）
  * @retval 无
  * @note   编码器模式下计数器根据编码器信号自动增减
  */
void TIM_EncoderInterfaceConfig(TIM_TypeDef* TIMx, uint16_t TIM_EncoderMode,
                                uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity)
{
  uint16_t tmpsmcr = 0;
  uint16_t tmpccmr1 = 0;
  uint16_t tmpccer = 0;
    
  /* 四重参数验证 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));          // 检查TIM1-TIM5,8,9,12
  assert_param(IS_TIM_ENCODER_MODE(TIM_EncoderMode));// 编码模式合法性检查
  assert_param(IS_TIM_IC_POLARITY(TIM_IC1Polarity)); // 通道1极性检查
  assert_param(IS_TIM_IC_POLARITY(TIM_IC2Polarity));// 通道2极性检查

  /* 获取相关寄存器原始值用于位操作 */
  tmpsmcr = TIMx->SMCR;   // 从模式控制寄存器
  tmpccmr1 = TIMx->CCMR1; // 捕获比较模式寄存器1
  tmpccer = TIMx->CCER;   // 捕获比较使能寄存器

  /* 配置编码器模式（设置SMCR的SMS位域） */
  tmpsmcr &= (uint16_t)~TIM_SMCR_SMS;  // 清除SMS位（BIT[2:0]）
  tmpsmcr |= TIM_EncoderMode;          // 设置新编码模式（例：0x0001为TI1计数）

  /* 配置CCMR1寄存器：
     - 通道1和通道2设置为输入模式
     - CC1S=01（通道1映射到TI1输入）
     - CC2S=01（通道2映射到TI2输入） */
  tmpccmr1 &= ((uint16_t)~TIM_CCMR1_CC1S) & ((uint16_t)~TIM_CCMR1_CC2S); // 清除原有配置
  tmpccmr1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;  // 设置CC1S=01, CC2S=01

  /* 配置输入通道极性（设置CCER寄存器）：
     - CC1P位控制通道1极性
     - CC2P位控制通道2极性 */
  tmpccer &= ((uint16_t)~TIM_CCER_CC1P) & ((uint16_t)~TIM_CCER_CC2P); // 清除原有极性
  tmpccer |= (uint16_t)(TIM_IC1Polarity | (TIM_IC2Polarity << 4));     // 组合极性配置

  /* 将配置写入寄存器 */
  TIMx->SMCR = tmpsmcr;   // 写入编码模式配置
  TIMx->CCMR1 = tmpccmr1; // 写入输入通道映射配置
  TIMx->CCER = tmpccer;   // 写入输入极性配置
}

/**
  * @brief  启用/禁用霍尔传感器接口（三相无刷电机控制）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8,9,12
  * @param  NewState: 新状态（ENABLE=霍尔模式，DISABLE=普通模式）
  * @retval 无
  * @note   启用后将通道1配置为霍尔传感器输入，需配合编码器模式使用
  */
void TIM_SelectHallSensor(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* 参数双重验证 */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));      // 检查TIM1-TIM5,8,9,12
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 状态参数检查

  if (NewState != DISABLE)
  {
    /* 启用霍尔接口：置位CR2的TI1S位（BIT[7]） */
    TIMx->CR2 |= TIM_CR2_TI1S;  // 通道1输入变为TI1F_ED（三输入异或）
  }
  else
  {
    /* 禁用霍尔接口：清除TI1S位 */
    TIMx->CR2 &= (uint16_t)~TIM_CR2_TI1S; // 恢复通道1为独立输入
  }
}

/**
  * @}
  */

/** @defgroup TIM_Group9 Specific remapping management function
 *  @brief   Specific remapping management function
 *
@verbatim   
 ===============================================================================
              ##### Specific remapping management function #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Configures the TIM2, TIM5 and TIM11 Remapping input capabilities.
  * @param  TIMx: where x can be 2, 5 or 11 to select the TIM peripheral.
  * @param  TIM_Remap: specifies the TIM input remapping source.
  *          This parameter can be one of the following values:
  *            @arg TIM2_TIM8_TRGO: TIM2 ITR1 input is connected to TIM8 Trigger output(default)
  *            @arg TIM2_ETH_PTP:   TIM2 ITR1 input is connected to ETH PTP trogger output.
  *            @arg TIM2_USBFS_SOF: TIM2 ITR1 input is connected to USB FS SOF. 
  *            @arg TIM2_USBHS_SOF: TIM2 ITR1 input is connected to USB HS SOF. 
  *            @arg TIM5_GPIO:      TIM5 CH4 input is connected to dedicated Timer pin(default)
  *            @arg TIM5_LSI:       TIM5 CH4 input is connected to LSI clock.
  *            @arg TIM5_LSE:       TIM5 CH4 input is connected to LSE clock.
  *            @arg TIM5_RTC:       TIM5 CH4 input is connected to RTC Output event.
  *            @arg TIM11_GPIO:     TIM11 CH4 input is connected to dedicated Timer pin(default) 
  *            @arg TIM11_HSE:      TIM11 CH4 input is connected to HSE_RTC clock
  *                                 (HSE divided by a programmable prescaler)  
  * @retval None
  */
/**
  * @brief  配置定时器引脚重映射功能（改变外设信号路由）
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8-TIM11
  * @param  TIM_Remap: 重映射配置选项，可选值取决于具体定时器：
  *            @arg TIM_TIM1_ETR_ADC1/2/3: TIM1 ETR引脚映射到ADC接口
  *            @arg TIM_TIM1_TI1_GPIO:     TIM1通道1映射到默认GPIO
  *            @arg TIM_TIM1_TI1_HSE:      TIM1通道1映射到HSE时钟
  *            （其他定时器有类似选项，具体参考芯片参考手册）
  * @retval 无
  * @warning 重映射操作可能影响其他外设功能，需谨慎使用
  */
void TIM_RemapConfig(TIM_TypeDef* TIMx, uint16_t TIM_Remap)
{
 /* 参数双重验证 */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));  // 检查TIM1-TIM5,8-11合法性
  assert_param(IS_TIM_REMAP(TIM_Remap));    // 重映射参数合法性检查

  /* 直接写入选项寄存器(OR)设置重映射配置 
     OR寄存器位定义因定时器型号不同而有所差异 */
  TIMx->OR =  TIM_Remap;  // 例如TIM_TIM1_TI1_HSE对应0x0008
}

/**
  * @brief  （内部函数）配置定时器输入通道1参数
  * @param  TIMx: 定时器实例，支持TIM1-TIM5,8-14
  * @param  TIM_ICPolarity: 输入极性，可选：
  *            @arg TIM_ICPolarity_Rising: 上升沿捕获
  *            @arg TIM_ICPolarity_Falling: 下降沿捕获
  *            @arg TIM_ICPolarity_BothEdge: 双边沿捕获
  * @param  TIM_ICSelection: 输入源选择，可选：
  *            @arg TIM_ICSelection_DirectTI: 直连到TI1引脚
  *            @arg TIM_ICSelection_IndirectTI: 交叉连接到TI2引脚
  *            @arg TIM_ICSelection_TRC: 连接到内部触发控制器
  * @param  TIM_ICFilter: 输入滤波器系数（0x00-0x0F对应1-16个时钟周期）
  * @retval 无
  * @note   该函数被TIM_ICInit等函数内部调用
  */
static void TI1_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr1 = 0, tmpccer = 0;

  /* 禁用通道1捕获功能（避免配置过程中误触发） */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC1E;  // 清除CCER寄存器的CC1E位（BIT[0]）

  /* 获取当前寄存器状态用于位操作 */
  tmpccmr1 = TIMx->CCMR1;  // 捕获/比较模式寄存器1
  tmpccer = TIMx->CCER;     // 捕获/比较使能寄存器

  /* 配置输入源和滤波器（CCMR1寄存器操作）：
     BIT[1:0] CC1S   - 输入源选择
     BIT[7:4] IC1F[3:0] - 滤波器设置 */
  tmpccmr1 &= ((uint16_t)~TIM_CCMR1_CC1S) & ((uint16_t)~TIM_CCMR1_IC1F);  // 清除原有配置
  tmpccmr1 |= (uint16_t)(TIM_ICSelection | (TIM_ICFilter << 4));  // 组合输入源和滤波器配置

  /* 配置输入极性并重新使能通道（CCER寄存器操作）：
     BIT[1] CC1P   - 极性选择
     BIT[3] CC1NP  - 互补通道极性（本函数未使用）
     BIT[0] CC1E   - 通道使能位 */
  tmpccer &= (uint16_t)~(TIM_CCER_CC1P | TIM_CCER_CC1NP);  // 清除原有极性配置
  tmpccer |= (uint16_t)(TIM_ICPolarity | TIM_CCER_CC1E);   // 设置极性并重新使能通道

  /* 将配置写入寄存器 */
  TIMx->CCMR1 = tmpccmr1;  // 输入源和滤波器生效
  TIMx->CCER = tmpccer;     // 极性和通道使能生效
}


/**
  * @brief  Configure the TI2 as Input.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9 or 12 to select the TIM 
  *         peripheral.
  * @param  TIM_ICPolarity : The Input Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM_ICPolarity_Rising
  *            @arg TIM_ICPolarity_Falling
  *            @arg TIM_ICPolarity_BothEdge   
  * @param  TIM_ICSelection: specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM_ICSelection_DirectTI: TIM Input 2 is selected to be connected to IC2.
  *            @arg TIM_ICSelection_IndirectTI: TIM Input 2 is selected to be connected to IC1.
  *            @arg TIM_ICSelection_TRC: TIM Input 2 is selected to be connected to TRC.
  * @param  TIM_ICFilter: Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
/**
  * @brief  （内部函数）配置定时器输入通道2参数
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8
  * @param  TIM_ICPolarity: 输入极性，可选：
  *            @arg TIM_ICPolarity_Rising: 上升沿捕获
  *            @arg TIM_ICPolarity_Falling: 下降沿捕获
  *            @arg TIM_ICPolarity_BothEdge: 双边沿捕获（特殊模式）
  * @param  TIM_ICSelection: 输入源选择，可选：
  *            @arg TIM_ICSelection_DirectTI: 直连到TI2引脚
  *            @arg TIM_ICSelection_IndirectTI: 交叉连接到TI1引脚
  *            @arg TIM_ICSelection_TRC: 连接到内部触发控制器
  * @param  TIM_ICFilter: 输入滤波器系数（0x00-0x0F对应1-16个时钟周期）
  * @retval 无
  */
static void TI2_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr1 = 0, tmpccer = 0, tmp = 0;

  /* 禁用通道2捕获功能（防止配置过程中产生误触发） */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC2E;  // 清除CCER寄存器的CC2E位（BIT[4]）

  /* 获取当前寄存器状态用于位操作 */
  tmpccmr1 = TIMx->CCMR1;  // 捕获/比较模式寄存器1（通道1/2配置）
  tmpccer = TIMx->CCER;     // 捕获/比较使能寄存器
  tmp = (uint16_t)(TIM_ICPolarity << 4);  // 极性参数左移4位对齐到CC2P位（BIT[5:4]）

  /* 配置输入源和滤波器（CCMR1寄存器高字节操作）：
     BIT[9:8] CC2S   - 输入源选择
     BIT[15:12] IC2F - 滤波器设置 */
  tmpccmr1 &= ((uint16_t)~TIM_CCMR1_CC2S) & ((uint16_t)~TIM_CCMR1_IC2F); // 清除原有配置
  tmpccmr1 |= (uint16_t)(TIM_ICFilter << 12);  // 滤波器值左移12位
  tmpccmr1 |= (uint16_t)(TIM_ICSelection << 8); // 输入源选择左移8位

  /* 配置输入极性并重新使能通道：
     BIT[5] CC2P   - 极性选择
     BIT[7] CC2NP  - 互补通道极性（本函数未使用）
     BIT[4] CC2E   - 通道使能位 */
  tmpccer &= (uint16_t)~(TIM_CCER_CC2P | TIM_CCER_CC2NP); // 清除原有极性
  tmpccer |=  (uint16_t)(tmp | TIM_CCER_CC2E); // 组合极性配置并重新使能通道

  /* 写入配置 */
  TIMx->CCMR1 = tmpccmr1;  // 输入源和滤波器生效
  TIMx->CCER = tmpccer;    // 极性和使能配置生效
}

/**
  * @brief  （内部函数）配置定时器输入通道3参数
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8
  * @param  TIM_ICPolarity: 输入极性（同通道2）
  * @param  TIM_ICSelection: 输入源选择（同通道2）
  * @param  TIM_ICFilter: 滤波器系数（同通道2）
  * @retval 无
  */
static void TI3_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

  /* 禁用通道3捕获功能 */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC3E;  // 清除CCER的CC3E位（BIT[8]）

  /* 获取寄存器当前值 */
  tmpccmr2 = TIMx->CCMR2;  // 捕获/比较模式寄存器2（通道3/4配置）
  tmpccer = TIMx->CCER;
  tmp = (uint16_t)(TIM_ICPolarity << 8);  // 极性参数左移8位对齐到CC3P位（BIT[9:8]）

  /* 配置输入源和滤波器（CCMR2低字节操作）：
     BIT[1:0] CC3S   - 输入源选择
     BIT[7:4] IC3F   - 滤波器设置 */
  tmpccmr2 &= ((uint16_t)~TIM_CCMR2_CC3S) & ((uint16_t)~TIM_CCMR2_IC3F);
  tmpccmr2 |= (uint16_t)(TIM_ICSelection | (TIM_ICFilter << 4));

  /* 配置极性并重新使能通道 */
  tmpccer &= (uint16_t)~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
  tmpccer |= (uint16_t)(tmp | TIM_CCER_CC3E);

  /* 写入配置 */
  TIMx->CCMR2 = tmpccmr2;
  TIMx->CCER = tmpccer;
}

/**
  * @brief  （内部函数）配置定时器输入通道4参数
  * @param  TIMx: 定时器实例，支持TIM1-TIM5, TIM8
  * @param  TIM_ICPolarity: 输入极性（同通道2）
  * @param  TIM_ICSelection: 输入源选择（同通道2）
  * @param  TIM_ICFilter: 滤波器系数（同通道2）
  * @retval 无
  */
static void TI4_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

  /* 禁用通道4捕获功能 */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC4E;  // 清除CCER的CC4E位（BIT[12]）

  /* 获取寄存器当前值 */
  tmpccmr2 = TIMx->CCMR2;
  tmpccer = TIMx->CCER;
  tmp = (uint16_t)(TIM_ICPolarity << 12);  // 极性参数左移12位对齐到CC4P位（BIT[13:12]）

  /* 配置输入源和滤波器（CCMR2高字节操作）：
     BIT[9:8] CC4S   - 输入源选择
     BIT[15:12] IC4F - 滤波器设置 */
  tmpccmr2 &= ((uint16_t)~TIM_CCMR2_CC4S) & ((uint16_t)~TIM_CCMR2_IC4F);
  tmpccmr2 |= (uint16_t)(TIM_ICSelection << 8);
  tmpccmr2 |= (uint16_t)(TIM_ICFilter << 12);

  /* 配置极性并重新使能通道 */
  tmpccer &= (uint16_t)~(TIM_CCER_CC4P | TIM_CCER_CC4NP);
  tmpccer |= (uint16_t)(tmp | TIM_CCER_CC4E);

  /* 写入配置 */
  TIMx->CCMR2 = tmpccmr2;
  TIMx->CCER = tmpccer ;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

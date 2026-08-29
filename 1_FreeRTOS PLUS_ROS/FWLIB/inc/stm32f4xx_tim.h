/**
  ******************************************************************************
  * @file    stm32f4xx_tim.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the TIM firmware 
  *          library.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_TIM_H
#define __STM32F4xx_TIM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup TIM
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/** 
  * @brief  TIM时基初始化结构体
  * @note   适用于除TIM6/TIM7外的所有TIMx定时器
  * @note   主配置：预分频器 → 计数模式 → 周期 → 时钟分频 → 重复计数
  */
typedef struct
{
  /**
    * @brief 预分频器值
    * @note 计算公式：分频系数 = TIM_Prescaler + 1
    * @value 范围：0x0000-0xFFFF
    * @note 实际频率 = 输入时钟 / (TIM_Prescaler + 1)
    */
  uint16_t TIM_Prescaler;

  /**
    * @brief 计数模式配置
    * @value 参见 @ref TIM_Counter_Mode
    *       - TIM_CounterMode_Up: 上数模式
    *       - TIM_CounterMode_Down: 下数模式  
    *       - TIM_CounterMode_CenterAligned1/2/3: 中央对齐模式
    */
  uint16_t TIM_CounterMode;

  /**
    * @brief 自动重载周期值
    * @note 溢出周期 = (TIM_Period + 1) × 分频后的时钟周期
    * @value 范围：0x0000-0xFFFF
    * @example 72MHz时钟，分频系数7200，周期10000：频率=72M/(7200×10000)=1Hz
    */
  uint32_t TIM_Period;

  /**
    * @brief 时钟分频因子
    * @value 参见 @ref TIM_Clock_Division_CKD
    *       - TIM_CKD_DIV1: 不分频
    *       - TIM_CKD_DIV2: 2分频
    *       - TIM_CKD_DIV4: 4分频
    * @note 用于数字滤波采样时钟配置
    */
  uint16_t TIM_ClockDivision;

  /**
    * @brief 重复计数器值（高级控制定时器）
    * @note 仅TIM1/TIM8有效，更新事件频率 = (TIM_Period+1) × (TIM_RepetitionCounter+1)
    * @value 范围：0x00-0xFF
    * @example 周期=1000，RCR=1：每2000计数产生更新事件
    */
  uint8_t TIM_RepetitionCounter;
} TIM_TimeBaseInitTypeDef;

/** 
  * @brief  输出比较初始化结构体
  * @note   用于PWM生成/单脉冲模式等配置
  */
typedef struct
{
  /**
    * @brief 输出比较模式选择
    * @value 参见 @ref TIM_Output_Compare_and_PWM_modes
    *       - TIM_OCMode_Timing: 定时模式
    *       - TIM_OCMode_Active: 强制高电平
    *       - TIM_OCMode_PWM1/2: PWM模式
    */
  uint16_t TIM_OCMode;

  /**
    * @brief 主输出通道使能
    * @value 参见 @ref TIM_Output_Compare_State
    *       - TIM_OutputState_Enable: 使能输出  
    *       - TIM_OutputState_Disable: 禁用输出
    */
  uint16_t TIM_OutputState;

  /**
    * @brief 互补输出通道使能
    * @value 参见 @ref TIM_Output_Compare_N_State
    * @note 仅TIM1/TIM8支持互补输出
    */
  uint16_t TIM_OutputNState;

  /**
    * @brief 比较寄存器值（占空比控制）
    * @note 占空比 = TIM_Pulse / (TIM_Period + 1)
    * @value 范围：0x0000-0xFFFF
    */
  uint32_t TIM_Pulse;

  /**
    * @brief 主输出极性配置
    * @value 参见 @ref TIM_Output_Compare_Polarity
    *       - TIM_OCPolarity_High: 高电平有效
    *       - TIM_OCPolarity_Low: 低电平有效
    */
  uint16_t TIM_OCPolarity;

  /**
    * @brief 互补输出极性配置
    * @value 参见 @ref TIM_Output_Compare_N_Polarity
    * @note 需配置TIM_OutputNState为Enable
    */
  uint16_t TIM_OCNPolarity;

  /**
    * @brief 空闲状态下主输出电平
    * @value 参见 @ref TIM_Output_Compare_Idle_State
    * @note 在刹车或MOE位禁用时生效
    */
  uint16_t TIM_OCIdleState;

  /**
    * @brief 空闲状态下互补输出电平
    * @value 参见 @ref TIM_Output_Compare_N_Idle_State
    * @note 用于电机驱动的安全状态配置
    */
  uint16_t TIM_OCNIdleState;
} TIM_OCInitTypeDef;

/** 
  * @brief  输入捕获初始化结构体
  * @note   用于脉冲宽度测量/编码器接口等应用
  */
typedef struct
{
  /**
    * @brief 选择定时器通道
    * @value 参见 @ref TIM_Channel
    *       - TIM_Channel_1/2/3/4: 通道1-4
    */
  uint16_t TIM_Channel;

  /**
    * @brief 输入捕获触发极性
    * @value 参见 @ref TIM_Input_Capture_Polarity
    *       - TIM_ICPolarity_Rising/Falling/BothEdge
    */
  uint16_t TIM_ICPolarity;

  /**
    * @brief 输入信号映射选择
    * @value 参见 @ref TIM_Input_Capture_Selection
    *       - TIM_ICSelection_DirectTI: 直接连接  
    *       - TIM_ICSelection_IndirectTI: 间接连接
    *       - TIM_ICSelection_TRC: 内部触发
    */
  uint16_t TIM_ICSelection;

  /**
    * @brief 输入捕获预分频系数
    * @value 参见 @ref TIM_Input_Capture_Prescaler
    *       - TIM_ICPSC_DIV1/2/4/8: 捕获每1/2/4/8个事件触发
    */
  uint16_t TIM_ICPrescaler;

  /**
    * @brief 输入滤波系数
    * @value 范围：0x0（无滤波）~0xF（8分频时钟采样的8次事件）
    * @note 滤波器长度 = (TIM_ICFilter + 1)个时钟周期
    */
  uint16_t TIM_ICFilter;
} TIM_ICInitTypeDef;
/** 
  * @brief  BDTR刹车与死区配置结构体
  * @note   专用于高级控制定时器TIM1/TIM8，适用电机控制等需要安全保护的场景
  * @note   涉及功能：输出状态管理、死区时间、刹车输入、寄存器锁保护等
  */
typedef struct
{
  /**
    * @brief 运行模式下的关闭状态选择
    * @value 参见 @ref TIM_OSSR_Off_State_Selection_for_Run_mode_state
    *       - TIM_OSSRState_Enable: 运行模式下关闭时保持最后有效输出
    *       - TIM_OSSRState_Disable: 运行模式下关闭时输出禁止状态
    * @note 用于功率器件安全关闭时的电平控制
    */
  uint16_t TIM_OSSRState;        

  /**
    * @brief 空闲模式下的关闭状态选择
    * @value 参见 @ref TIM_OSSI_Off_State_Selection_for_Idle_mode_state
    *       - TIM_OSSIState_Enable: 空闲模式下输出预定电平
    *       - TIM_OSSIState_Disable: 空闲模式下保持当前状态
    * @note 定义定时器未激活时的输出行为
    */
  uint16_t TIM_OSSIState;        

  /**
    * @brief 寄存器锁定等级配置
    * @value 参见 @ref TIM_Lock_level
    *       - TIM_LOCKLevel_OFF: 无保护
    *       - TIM_LOCKLevel_1:   写保护等级1
    *       - TIM_LOCKLevel_2:   写保护等级2
    *       - TIM_LOCKLevel_3:   写保护等级3
    * @note 等级越高，解锁所需步骤越多，防止意外配置修改
    */
  uint16_t TIM_LOCKLevel;        

  /**
    * @brief 死区时间配置值
    * @note 单位：定时器时钟周期，公式：
    *       T_dts = 1 / (TIMx_CLK / prescaler) 
    *       实际死区时间 = DT[7:0] × T_dts
    * @value 范围0x00-0xFF，通常配置为1-100个时钟周期
    * @warning 死区过小可能引发功率管直通损坏
    */
  uint16_t TIM_DeadTime;         

  /**
    * @brief 刹车输入使能控制
    * @value 参见 @ref TIM_Break_Input_enable_disable
    *       - TIM_Break_Enable:  检测到刹车信号立即关闭输出
    *       - TIM_Break_Disable: 忽略刹车输入
    * @note 连接到外部故障信号（如过流检测）
    */
  uint16_t TIM_Break;            

  /**
    * @brief 刹车输入信号极性
    * @value 参见 @ref TIM_Break_Polarity
    *       - TIM_BreakPolarity_Low:  低电平触发刹车
    *       - TIM_BreakPolarity_High: 高电平触发刹车
    * @note 根据硬件保护电路设计选择合适极性
    */
  uint16_t TIM_BreakPolarity;    

  /**
    * @brief 自动输出使能配置
    * @value 参见 @ref TIM_AOE_Bit_Set_Reset
    *       - TIM_AutomaticOutput_Enable: 刹车解除后自动恢复输出
    *       - TIM_AutomaticOutput_Disable: 需手动清除刹车标志
    * @note 增强系统故障恢复的自动化能力
    */
  uint16_t TIM_AutomaticOutput;  
} TIM_BDTRInitTypeDef;

/* 外设有效性校验宏组 *******************************************************/

/** 
  * @brief 校验TIM外设实例是否有效（全型号列表） 
  * @note 包含F4系列所有定时器型号：
  *       TIM1-TIM14（除TIM6/TIM7外的通用/高级/基本定时器）
  */
#define IS_TIM_ALL_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                   ((PERIPH) == TIM2) || \
                                   ((PERIPH) == TIM3) || \
                                   ((PERIPH) == TIM4) || \
                                   ((PERIPH) == TIM5) || \
                                   ((PERIPH) == TIM6) || \
                                   ((PERIPH) == TIM7) || \
                                   ((PERIPH) == TIM8) || \
                                   ((PERIPH) == TIM9) || \
                                   ((PERIPH) == TIM10) || \
                                   ((PERIPH) == TIM11) || \
                                   ((PERIPH) == TIM12) || \
                                   (((PERIPH) == TIM13) || \
                                   ((PERIPH) == TIM14)))

/** 
  * @brief 校验TIM外设是否属于列表1 
  * @note 列表1特性：支持输入捕获/输出比较等高级功能
  * @note 包含型号：TIM1,2,3,4,5,8,9,10,11,12,13,14
  */                                         
#define IS_TIM_LIST1_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM8) || \
                                     ((PERIPH) == TIM9) || \
                                     ((PERIPH) == TIM10) || \
                                     ((PERIPH) == TIM11) || \
                                     ((PERIPH) == TIM12) || \
                                     ((PERIPH) == TIM13) || \
                                     ((PERIPH) == TIM14))

                /* TIM外设分组验证宏 **********************************************************/

/** 
  * @brief 校验TIM外设是否属于列表2 
  * @note 列表2特性：支持编码器接口/霍尔传感器的定时器
  * @note 包含型号：TIM1,2,3,4,5,8,9,12
  */
#define IS_TIM_LIST2_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM8) || \
                                     ((PERIPH) == TIM9) || \
                                     ((PERIPH) == TIM12))

/** 
  * @brief 校验TIM外设是否属于列表3 
  * @note 列表3特性：支持DMA请求的定时器
  * @note 包含型号：TIM1,2,3,4,5,8
  */
#define IS_TIM_LIST3_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM8))

/** 
  * @brief 校验TIM外设是否属于列表4 
  * @note 列表4特性：具有互补输出的高级控制定时器
  * @note 包含型号：TIM1,8
  */
#define IS_TIM_LIST4_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM8))

/** 
  * @brief 校验TIM外设是否属于列表5 
  * @note 列表5特性：支持主从模式的定时器
  * @note 包含型号：TIM1,2,3,4,5,6,7,8
  */
#define IS_TIM_LIST5_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM6) || \
                                     ((PERIPH) == TIM7) || \
                                     ((PERIPH) == TIM8))

/** 
  * @brief 校验TIM外设是否属于列表6 
  * @note 列表6特性：支持外部时钟源2的定时器
  * @note 包含型号：TIM2,5,11
  */
#define IS_TIM_LIST6_PERIPH(TIMx)(((TIMx) == TIM2) || \
                                 ((TIMx) == TIM5) || \
                                 ((TIMx) == TIM11))

/** @defgroup TIM_Output_Compare_and_PWM_modes 输出比较与PWM模式组
  * @brief 定义定时器输出比较工作模式
  * @note 模式配置寄存器：CCMRx[7:4]/CCMRx[15:12]
  */
#define TIM_OCMode_Timing      ((uint16_t)0x0000)  /*!< 定时模式：CCR只用于触发事件 */
#define TIM_OCMode_Active      ((uint16_t)0x0010)  /*!< 激活模式：匹配时输出有效电平 */
#define TIM_OCMode_Inactive    ((uint16_t)0x0020)  /*!< 非激活模式：匹配时输出无效电平 */
#define TIM_OCMode_Toggle      ((uint16_t)0x0030)  /*!< 翻转模式：匹配时输出电平翻转 */
#define TIM_OCMode_PWM1        ((uint16_t)0x0060)  /*!< PWM模式1：向上计数时ARR>CCR有效 */
#define TIM_OCMode_PWM2        ((uint16_t)0x0070)  /*!< PWM模式2：向上计数时ARR<CCR有效 */

/** 
  * @brief 校验标准输出比较模式有效性
  * @note 用于常规通道模式配置检查
  */
#define IS_TIM_OC_MODE(MODE) (((MODE) == TIM_OCMode_Timing) || \
                              ((MODE) == TIM_OCMode_Active) || \
                              ((MODE) == TIM_OCMode_Inactive) || \
                              ((MODE) == TIM_OCMode_Toggle)|| \
                              ((MODE) == TIM_OCMode_PWM1) || \
                              ((MODE) == TIM_OCMode_PWM2))

/** 
  * @brief 扩展校验输出比较模式（含强制输出）
  * @note 用于高级控制模式检查
  */
#define IS_TIM_OCM(MODE) (((MODE) == TIM_OCMode_Timing) || \
                          ((MODE) == TIM_OCMode_Active) || \
                          ((MODE) == TIM_OCMode_Inactive) || \
                          ((MODE) == TIM_OCMode_Toggle)|| \
                          ((MODE) == TIM_OCMode_PWM1) || \
                          ((MODE) == TIM_OCMode_PWM2) ||	\
                          ((MODE) == TIM_ForcedAction_Active) || \
                          ((MODE) == TIM_ForcedAction_InActive))

/**
  * @}
  */
/** @defgroup TIM_One_Pulse_Mode 单脉冲模式配置组
  * @brief 控制定时器脉冲生成模式
  * @note 用于需要精确单次或重复触发的外部事件控制
  * @{
  */
#define TIM_OPMode_Single      ((uint16_t)0x0008)  /*!< 单脉冲模式：触发后仅输出一次有效脉冲 */
#define TIM_OPMode_Repetitive   ((uint16_t)0x0000)  /*!< 重复模式：持续输出周期性脉冲 */
/**
  * @brief 验证单脉冲模式参数有效性
  * @param MODE 模式配置值
  * @note 单脉冲模式典型应用：精确控制步进电机步进角
  */
#define IS_TIM_OPM_MODE(MODE) (((MODE) == TIM_OPMode_Single) || \
                               ((MODE) == TIM_OPMode_Repetitive))
/** @} */  // 结束单脉冲模式组

/** @defgroup TIM_Channel 定时器通道选择组
  * @brief 选择定时器的硬件通道
  * @note 每个通道对应独立的比较/捕获寄存器
  * @{
  */
#define TIM_Channel_1    ((uint16_t)0x0000)  /*!< 通道1，对应GPIO引脚如PA8(TIM1_CH1) */
#define TIM_Channel_2    ((uint16_t)0x0004)  /*!< 通道2，对应GPIO引脚如PA1(TIM2_CH2) */
#define TIM_Channel_3    ((uint16_t)0x0008)  /*!< 通道3，对应GPIO引脚如PB0(TIM3_CH3) */
#define TIM_Channel_4    ((uint16_t)0x000C)  /*!< 通道4，对应GPIO引脚如PB7(TIM4_CH4) */

/**
  * @brief 验证基础通道参数有效性
  * @param CHANNEL 通道选择值
  */
#define IS_TIM_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_1) || \
                                 ((CHANNEL) == TIM_Channel_2) || \
                                 ((CHANNEL) == TIM_Channel_3) || \
                                 ((CHANNEL) == TIM_Channel_4))

/**
  * @brief 验证PWM输入模式通道有效性
  * @note 仅通道1/2支持PWM输入模式
  */
#define IS_TIM_PWMI_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_1) || \
                                      ((CHANNEL) == TIM_Channel_2))

/**
  * @brief 验证互补通道有效性
  * @note 仅高级定时器(TIM1/8)通道1-3具有互补输出
  */
#define IS_TIM_COMPLEMENTARY_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_1) || \
                                               ((CHANNEL) == TIM_Channel_2) || \
                                               ((CHANNEL) == TIM_Channel_3))
/** @} */  // 结束通道选择组

/** @defgroup TIM_Clock_Division_CKD 时钟分频配置组
  * @brief 配置输入捕获数字滤波器采样频率
  * @note 分频系数影响抗噪能力与响应速度
  * @{
  */
#define TIM_CKD_DIV1     ((uint16_t)0x0000)  /*!< 无分频，最高采样率 */
#define TIM_CKD_DIV2     ((uint16_t)0x0100)  /*!< 2分频，平衡噪声抑制与响应 */
#define TIM_CKD_DIV4     ((uint16_t)0x0200)  /*!< 4分频，最强抗噪能力 */
/**
  * @brief 验证时钟分频参数有效性
  * @param DIV 分频系数配置值
  * @note 分频公式：F_sampling = TIMx_CLK / (CKD × 滤波器长度)
  */
#define IS_TIM_CKD_DIV(DIV) (((DIV) == TIM_CKD_DIV1) || \
                             ((DIV) == TIM_CKD_DIV2) || \
                             ((DIV) == TIM_CKD_DIV4))
/** @} */  // 结束时钟分频组

/** @defgroup TIM_Counter_Mode 计数器模式组
  * @brief 配置定时器计数方向及对齐方式
  * @note 中央对齐模式影响PWM中心对称性
  * @{
  */
#define TIM_CounterMode_Up           ((uint16_t)0x0000)  /*!< 上数模式：0→ARR */
#define TIM_CounterMode_Down         ((uint16_t)0x0010)  /*!< 下数模式：ARR→0 */
#define TIM_CounterMode_CenterAligned1 ((uint16_t)0x0020) /*!< 中央对齐模式1：先上后下，中断在峰顶 */
#define TIM_CounterMode_CenterAligned2 ((uint16_t)0x0040) /*!< 中央对齐模式2：先下后上，中断在谷底 */
#define TIM_CounterMode_CenterAligned3 ((uint16_t)0x0060) /*!< 中央对齐模式3：交替计数，中断在峰谷 */
/**
  * @brief 验证计数模式参数有效性
  * @param MODE 计数模式配置值
  * @note 中央对齐模式适用于电机控制减少EMI
  */
#define IS_TIM_COUNTER_MODE(MODE) (((MODE) == TIM_CounterMode_Up) ||  \
                                   ((MODE) == TIM_CounterMode_Down) || \
                                   ((MODE) == TIM_CounterMode_CenterAligned1) || \
                                   ((MODE) == TIM_CounterMode_CenterAligned2) || \
                                   ((MODE) == TIM_CounterMode_CenterAligned3))
/** @} */  // 结束计数器模式组

/** @defgroup TIM_Output_Compare_Polarity 主输出极性组
  * @brief 配置主输出通道有效电平极性
  * @{
  */
#define TIM_OCPolarity_High    ((uint16_t)0x0000)  /*!< 高电平为有效状态 */
#define TIM_OCPolarity_Low     ((uint16_t)0x0002)  /*!< 低电平为有效状态 */
/**
  * @brief 验证主输出极性参数
  * @param POLARITY 极性配置值
  * @note 需与功率器件驱动逻辑匹配
  */
#define IS_TIM_OC_POLARITY(POLARITY) (((POLARITY) == TIM_OCPolarity_High) || \
                                      ((POLARITY) == TIM_OCPolarity_Low))
/** @} */  // 结束主输出极性组

/** @defgroup TIM_Output_Compare_N_Polarity 互补输出极性组
  * @brief 配置互补输出通道有效电平极性
  * @note 仅TIM1/TIM8有效
  * @{
  */
#define TIM_OCNPolarity_High   ((uint16_t)0x0000)  /*!< 互补输出高有效 */
#define TIM_OCNPolarity_Low    ((uint16_t)0x0008)  /*!< 互补输出低有效 */
/**
  * @brief 验证互补输出极性参数
  * @param POLARITY 极性配置值
  * @note 通常与主输出极性相反以避免直通
  */
#define IS_TIM_OCN_POLARITY(POLARITY) (((POLARITY) == TIM_OCNPolarity_High) || \
                                       ((POLARITY) == TIM_OCNPolarity_Low))
/** @} */  // 结束互补输出极性组

/** @defgroup TIM_Output_Compare_State 输出使能组
  * @brief 控制主输出通道的启用状态
  * @{
  */
#define TIM_OutputState_Disable  ((uint16_t)0x0000)  /*!< 禁用输出引脚驱动 */
#define TIM_OutputState_Enable   ((uint16_t)0x0001)  /*!< 使能输出引脚驱动 */
/**
  * @brief 验证输出状态参数有效性
  * @param STATE 使能状态配置值
  * @note 禁用状态可降低功耗
  */
#define IS_TIM_OUTPUT_STATE(STATE) (((STATE) == TIM_OutputState_Disable) || \
                                    ((STATE) == TIM_OutputState_Enable))
/** @} */  // 结束输出使能组

/**
  * @}
  */ 
/** @defgroup TIM_Output_Compare_N_State 互补输出使能组
  * @brief 控制互补输出通道的启用状态
  * @note 仅高级定时器(TIM1/8)有效
  * @{
  */
#define TIM_OutputNState_Disable  ((uint16_t)0x0000)  /*!< 禁用互补输出：CCxN引脚高阻 */
#define TIM_OutputNState_Enable   ((uint16_t)0x0004)  /*!< 使能互补输出：生成互补PWM */
/**
  * @brief 验证互补输出状态有效性
  * @param STATE 状态配置值
  * @note 需配合主输出使能使用
  */
#define IS_TIM_OUTPUTN_STATE(STATE) (((STATE) == TIM_OutputNState_Disable) || \
                                     ((STATE) == TIM_OutputNState_Enable))
/** @} */  // 结束互补输出使能组

/** @defgroup TIM_Capture_Compare_State 捕获比较通道使能组
  * @brief 启用/禁用通道的捕获比较功能
  * @{
  */
#define TIM_CCx_Enable    ((uint16_t)0x0001)  /*!< 通道使能：参与比较/捕获操作 */
#define TIM_CCx_Disable   ((uint16_t)0x0000)  /*!< 通道禁用：硬件逻辑忽略该通道 */
/**
  * @brief 验证通道使能参数有效性
  * @param CCX 通道状态值
  * @note 禁用通道可降低功耗
  */
#define IS_TIM_CCX(CCX) (((CCX) == TIM_CCx_Enable) || \
                         ((CCX) == TIM_CCx_Disable))
/** @} */  // 结束通道使能组

/** @defgroup TIM_Capture_Compare_N_State 互补通道使能组
  * @brief 控制互补通道的捕获比较功能
  * @note 仅TIM1/8具有互补通道
  * @{
  */
#define TIM_CCxN_Enable   ((uint16_t)0x0004)  /*!< 互补通道使能 */
#define TIM_CCxN_Disable  ((uint16_t)0x0000)  /*!< 互补通道禁用 */
/**
  * @brief 验证互补通道状态有效性
  * @param CCXN 状态值
  * @note 需与主通道配合使用
  */
#define IS_TIM_CCXN(CCXN) (((CCXN) == TIM_CCxN_Enable) || \
                           ((CCXN) == TIM_CCxN_Disable))
/** @} */  // 结束互补通道使能组

/** @defgroup TIM_Break_Input_enable_disable 刹车输入使能组
  * @brief 配置外部刹车信号的检测功能
  * @note 触发刹车信号将强制关闭输出
  * @{
  */
#define TIM_Break_Enable   ((uint16_t)0x1000)  /*!< 刹车功能使能：响应BRK引脚信号 */
#define TIM_Break_Disable  ((uint16_t)0x0000)  /*!< 刹车功能禁用：忽略BRK输入 */
/**
  * @brief 验证刹车状态参数有效性
  * @param STATE 使能状态值
  * @note 需配置BRK引脚复用功能
  */
#define IS_TIM_BREAK_STATE(STATE) (((STATE) == TIM_Break_Enable) || \
                                   ((STATE) == TIM_Break_Disable))
/** @} */  // 结束刹车使能组

/** @defgroup TIM_Break_Polarity 刹车信号极性组
  * @brief 定义刹车触发信号的逻辑电平
  * @{
  */
#define TIM_BreakPolarity_Low   ((uint16_t)0x0000)  /*!< 低电平触发刹车：BRK=0时触发 */
#define TIM_BreakPolarity_High  ((uint16_t)0x2000)  /*!< 高电平触发刹车：BRK=1时触发 */
/**
  * @brief 验证刹车极性参数有效性
  * @param POLARITY 极性值
  * @note 需与实际硬件保护电路匹配
  */
#define IS_TIM_BREAK_POLARITY(POLARITY) (((POLARITY) == TIM_BreakPolarity_Low) || \
                                         ((POLARITY) == TIM_BreakPolarity_High))
/** @} */  // 结束刹车极性组

/** @defgroup TIM_AOE_Bit_Set_Reset 自动输出使能组
  * @brief 控制刹车解除后的输出恢复行为
  * @{
  */
#define TIM_AutomaticOutput_Enable  ((uint16_t)0x4000)  /*!< 自动恢复：刹车信号解除后自动重启输出 */
#define TIM_AutomaticOutput_Disable ((uint16_t)0x0000)  /*!< 手动恢复：需清除刹车标志后手动开启 */
/**
  * @brief 验证自动输出参数有效性
  * @param STATE 状态值
  * @note 使能时需配置MOE=1
  */
#define IS_TIM_AUTOMATIC_OUTPUT_STATE(STATE) (((STATE) == TIM_AutomaticOutput_Enable) || \
                                              ((STATE) == TIM_AutomaticOutput_Disable))
/** @} */  // 结束自动输出组

/** @defgroup TIM_Lock_level 寄存器锁等级组
  * @brief 配置关键寄存器的写保护级别
  * @note 防止意外修改PWM参数引发危险
  * @{
  */
#define TIM_LOCKLevel_OFF  ((uint16_t)0x0000)  /*!< 无保护：允许自由修改寄存器 */
#define TIM_LOCKLevel_1    ((uint16_t)0x0100)  /*!< 等级1：需写LOCK_KEY=0x0001解锁 */
#define TIM_LOCKLevel_2    ((uint16_t)0x0200)  /*!< 等级2：需写LOCK_KEY=0x0002解锁 */
#define TIM_LOCKLevel_3    ((uint16_t)0x0300)  /*!< 等级3：需写LOCK_KEY=0x0003解锁 */
/**
  * @brief 验证锁定等级有效性
  * @param LEVEL 锁定等级值
  * @note 等级越高，解锁步骤越复杂
  */
#define IS_TIM_LOCK_LEVEL(LEVEL) (((LEVEL) == TIM_LOCKLevel_OFF) || \
                                  ((LEVEL) == TIM_LOCKLevel_1) || \
                                  ((LEVEL) == TIM_LOCKLevel_2) || \
                                  ((LEVEL) == TIM_LOCKLevel_3))
/** @} */  // 结束锁等级组

/** @defgroup TIM_OSSI_Off_State_Selection_for_Idle_mode_state 空闲模式关闭状态组
  * @brief 定义定时器未启用时的输出行为
  * @note 适用于低功耗模式配置
  * @{
  */
#define TIM_OSSIState_Enable   ((uint16_t)0x0400)  /*!< 输出预定电平：由CCxE/CCxNE位确定 */
#define TIM_OSSIState_Disable  ((uint16_t)0x0000)  /*!< 保持最后有效输出状态 */
/**
  * @brief 验证空闲状态参数有效性
  * @param STATE 状态值
  * @note 预定电平通过MOE=0时CCxE位的状态确定
  */
#define IS_TIM_OSSI_STATE(STATE) (((STATE) == TIM_OSSIState_Enable) || \
                                  ((STATE) == TIM_OSSIState_Disable))
/** @} */  // 结束空闲状态组

/** @defgroup TIM_OSSR_Off_State_Selection_for_Run_mode_state 运行模式关闭状态组
  * @brief 配置输出关闭时的引脚状态
  * @note 在运行模式下发生刹车/关闭时生效
  * @{
  */
#define TIM_OSSRState_Enable   ((uint16_t)0x0800)  /*!< 强制进入空闲预定状态 */
#define TIM_OSSRState_Disable  ((uint16_t)0x0000)  /*!< 保持关闭前状态 */
/**
  * @brief 验证运行关闭状态参数
  * @param STATE 状态值
  * @note 紧急关断时确保功率器件安全
  */
#define IS_TIM_OSSR_STATE(STATE) (((STATE) == TIM_OSSRState_Enable) || \
                                  ((STATE) == TIM_OSSRState_Disable))

/**
  * @}
  */ 
/** @defgroup TIM_Output_Compare_Idle_State 主输出空闲状态配置组
  * @brief 定义主输出通道在定时器空闲/禁用时的电平状态
  * @note 通过BDTR寄存器的MOE=0触发
  * @{
  */
#define TIM_OCIdleState_Set    ((uint16_t)0x0100)  /*!< 空闲时输出由OCxPolarity确定的有效电平 */
#define TIM_OCIdleState_Reset  ((uint16_t)0x0000)  /*!< 空闲时输出非有效电平（极性反向） */
/**
  * @brief 验证主输出空闲状态参数有效性
  * @param STATE 状态配置值
  * @note Set状态下：OCx极性=高则空闲时高电平
  */
#define IS_TIM_OCIDLE_STATE(STATE) (((STATE) == TIM_OCIdleState_Set) || \
                                    ((STATE) == TIM_OCIdleState_Reset))
/** @} */  // 结束主输出空闲状态组


/** @defgroup TIM_Output_Compare_N_Idle_State 互补输出空闲状态组
  * @brief 定义互补输出通道在空闲/禁用时的电平
  * @note 仅TIM1/TIM8有效
  * @{
  */
#define TIM_OCNIdleState_Set   ((uint16_t)0x0200)  /*!< 空闲时互补输出有效电平 */
#define TIM_OCNIdleState_Reset ((uint16_t)0x0000)  /*!< 空闲时互补输出非有效电平 */
/**
  * @brief 验证互补输出空闲参数
  * @param STATE 状态值
  * @note 需同时配置主通道空闲状态
  */
#define IS_TIM_OCNIDLE_STATE(STATE) (((STATE) == TIM_OCNIdleState_Set) || \
                                     ((STATE) == TIM_OCNIdleState_Reset))
/** @} */  // 结束互补输出空闲组


/** @defgroup TIM_Input_Capture_Polarity 输入捕获触发边沿组
  * @brief 选择捕获单元的触发边沿
  * @note 配置触发方式以测量脉宽或频率
  * @{
  */
#define TIM_ICPolarity_Rising     ((uint16_t)0x0000)  /*!< 上升沿触发捕获 */
#define TIM_ICPolarity_Falling    ((uint16_t)0x0002)  /*!< 下降沿触发捕获 */
#define TIM_ICPolarity_BothEdge   ((uint16_t)0x000A)  /*!< 双边沿均触发捕获 */
/**
  * @brief 验证触发边沿参数有效性
  * @param POLARITY 触发边沿配置值
  * @note 双边沿模式需与预分频配合使用
  */
#define IS_TIM_IC_POLARITY(POLARITY) (((POLARITY) == TIM_ICPolarity_Rising) || \
                                      ((POLARITY) == TIM_ICPolarity_Falling)|| \
                                      ((POLARITY) == TIM_ICPolarity_BothEdge))
/** @} */  // 结束输入边沿组


/** @defgroup TIM_Input_Capture_Selection 输入通道映射组
  * @brief 配置输入信号到捕获单元的连接方式
  * @note 支持跨通道映射实现特殊信号处理
  * @{
  */
#define TIM_ICSelection_DirectTI    ((uint16_t)0x0001)  /*!< 直连模式：TI1→IC1, TI2→IC2 */
#define TIM_ICSelection_IndirectTI ((uint16_t)0x0002)  /*!< 交叉模式：TI1→IC2, TI2→IC1 */
#define TIM_ICSelection_TRC         ((uint16_t)0x0003)  /*!< 内部触发模式：连接TRC信号 */
/**
  * @brief 验证输入映射参数有效性
  * @param SELECTION 输入选择模式值
  * @note 交叉模式用于正交编码器接口
  */
#define IS_TIM_IC_SELECTION(SELECTION) (((SELECTION) == TIM_ICSelection_DirectTI) || \
                                        ((SELECTION) == TIM_ICSelection_IndirectTI) || \
                                        ((SELECTION) == TIM_ICSelection_TRC))
/** @} */  // 结束输入映射组


/** @defgroup TIM_Input_Capture_Prescaler 输入捕获预分频组
  * @brief 设置输入信号的触发分频系数
  * @note 用于降噪或减少中断频率
  * @{
  */
#define TIM_ICPSC_DIV1    ((uint16_t)0x0000)  /*!< 无分频：每个边沿均触发 */
#define TIM_ICPSC_DIV2    ((uint16_t)0x0004)  /*!< 每2个事件触发一次 */
#define TIM_ICPSC_DIV4    ((uint16_t)0x0008)  /*!< 每4个事件触发一次 */
#define TIM_ICPSC_DIV8    ((uint16_t)0x000C)  /*!< 每8个事件触发一次 */
/**
  * @brief 验证预分频参数有效性
  * @param PRESCALER 分频配置值
  * @note 分频公式：触发间隔=N+1个事件周期
  */
#define IS_TIM_IC_PRESCALER(PRESCALER) (((PRESCALER) == TIM_ICPSC_DIV1) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV2) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV4) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV8))
/** @} */  // 结束输入预分频组

/**
  * @}
  */ 

/** @defgroup TIM_interrupt_sources 
  * @{
  */
/** @defgroup TIM_interrupt_sources 定时器中断源
  * @brief 定义定时器模块支持的中断类型及标志位掩码
  * @note 用于TIM_ITConfig()和TIM_GetITStatus()函数的中断管理
  */
#define TIM_IT_Update      ((uint16_t)0x0001)  /*!< 更新中断：计数器溢出/下溢/重载时触发 */
#define TIM_IT_CC1         ((uint16_t)0x0002)  /*!< 通道1捕获或比较匹配时触发 */
#define TIM_IT_CC2         ((uint16_t)0x0004)  /*!< 通道2捕获或比较匹配时触发 */
#define TIM_IT_CC3         ((uint16_t)0x0008)  /*!< 通道3捕获或比较匹配时触发 */
#define TIM_IT_CC4         ((uint16_t)0x0010)  /*!< 通道4捕获或比较匹配时触发 */
#define TIM_IT_COM         ((uint16_t)0x0020)  /*!< 互补输出换相时触发（高级定时器） */
#define TIM_IT_Trigger     ((uint16_t)0x0040)  /*!< 触发事件（TRGO）发生时触发 */
#define TIM_IT_Break       ((uint16_t)0x0080)  /*!< 刹车输入有效时触发 */

/**
  * @brief 验证中断类型掩码有效性（组合模式）
  * @param IT 中断标志组合，如 TIM_IT_Update | TIM_IT_CC1
  * @note 校验规则：仅使用低8位且非零
  */
#define IS_TIM_IT(IT) ((((IT) & (uint16_t)0xFF00) == 0x0000) && ((IT) != 0x0000))

/**
  * @brief 验证单中断类型有效性（单一模式）
  * @param IT 要检查的中断类型
  * @note 用于查询单个中断状态的参数校验
  */
#define IS_TIM_GET_IT(IT) (((IT) == TIM_IT_Update) || \
                           ((IT) == TIM_IT_CC1) || \
                           ((IT) == TIM_IT_CC2) || \
                           ((IT) == TIM_IT_CC3) || \
                           ((IT) == TIM_IT_CC4) || \
                           ((IT) == TIM_IT_COM) || \
                           ((IT) == TIM_IT_Trigger) || \
                           ((IT) == TIM_IT_Break))

/** @defgroup TIM_DMA_Base_address DMA基地址
  * @brief 定义TIM寄存器在DMA外设地址空间中的偏移量
  * @note 实际地址 = TIMx基地址 + 偏移量×4（因寄存器间隔4字节）
  */
#define TIM_DMABase_CR1    ((uint16_t)0x0000)  /*!< 控制寄存器1，偏移0x00 */
#define TIM_DMABase_CR2    ((uint16_t)0x0001)  /*!< 控制寄存器2，偏移0x04 */
#define TIM_DMABase_SMCR   ((uint16_t)0x0002)  /*!< 从模式控制寄存器，偏移0x08 */
#define TIM_DMABase_DIER   ((uint16_t)0x0003)  /*!< 中断使能寄存器，偏移0x0C */
#define TIM_DMABase_SR     ((uint16_t)0x0004)  /*!< 状态寄存器，偏移0x10 */
#define TIM_DMABase_EGR    ((uint16_t)0x0005)  /*!< 事件生成寄存器，偏移0x14 */
#define TIM_DMABase_CCMR1  ((uint16_t)0x0006)  /*!< 通道1/2模式寄存器，偏移0x18 */
#define TIM_DMABase_CCMR2  ((uint16_t)0x0007)  /*!< 通道3/4模式寄存器，偏移0x1C */
#define TIM_DMABase_CCER   ((uint16_t)0x0008)  /*!< 捕获比较使能寄存器，偏移0x20 */
#define TIM_DMABase_CNT    ((uint16_t)0x0009)  /*!< 计数器寄存器，偏移0x24 */
#define TIM_DMABase_PSC    ((uint16_t)0x000A)  /*!< 预分频寄存器，偏移0x28 */
#define TIM_DMABase_ARR    ((uint16_t)0x000B)  /*!< 自动重载寄存器，偏移0x2C */
#define TIM_DMABase_RCR    ((uint16_t)0x000C)  /*!< 重复计数寄存器，偏移0x30 */
#define TIM_DMABase_CCR1   ((uint16_t)0x000D)  /*!< 通道1比较/捕获寄存器，偏移0x34 */
#define TIM_DMABase_CCR2   ((uint16_t)0x000E)  /*!< 通道2比较/捕获寄存器，偏移0x38 */
#define TIM_DMABase_CCR3   ((uint16_t)0x000F)  /*!< 通道3比较/捕获寄存器，偏移0x3C */
#define TIM_DMABase_CCR4   ((uint16_t)0x0010)  /*!< 通道4比较/捕获寄存器，偏移0x40 */
#define TIM_DMABase_BDTR   ((uint16_t)0x0011)  /*!< 刹车/死区寄存器，偏移0x44 */
#define TIM_DMABase_DCR    ((uint16_t)0x0012)  /*!< DMA控制寄存器，偏移0x48 */
#define TIM_DMABase_OR     ((uint16_t)0x0013)  /*!< 选项寄存器，偏移0x4C */

/**
  * @brief 验证DMA基地址参数合法性
  * @param BASE 要检查的基地址标识符
  * @note 用于配置TIM_DMA_Config()的参数校验
  */
#define IS_TIM_DMA_BASE(BASE) (((BASE) == TIM_DMABase_CR1) || \
                               ((BASE) == TIM_DMABase_CR2) || \
                               ((BASE) == TIM_DMABase_SMCR) || \
                               ((BASE) == TIM_DMABase_DIER) || \
                               ((BASE) == TIM_DMABase_SR) || \
                               ((BASE) == TIM_DMABase_EGR) || \
                               ((BASE) == TIM_DMABase_CCMR1) || \
                               ((BASE) == TIM_DMABase_CCMR2) || \
                               ((BASE) == TIM_DMABase_CCER) || \
                               ((BASE) == TIM_DMABase_CNT) || \
                               ((BASE) == TIM_DMABase_PSC) || \
                               ((BASE) == TIM_DMABase_ARR) || \
                               ((BASE) == TIM_DMABase_RCR) || \
                               ((BASE) == TIM_DMABase_CCR1) || \
                               ((BASE) == TIM_DMABase_CCR2) || \
                               ((BASE) == TIM_DMABase_CCR3) || \
                               ((BASE) == TIM_DMABase_CCR4) || \
                               ((BASE) == TIM_DMABase_BDTR) || \
                               ((BASE) == TIM_DMABase_DCR) || \
                               ((BASE) == TIM_DMABase_OR))
                  
/**
  * @}
  */ 

/** @defgroup TIM_DMA_Burst_Length 
  * @{
  */

/** @defgroup TIM_DMA_Burst_Length 
  * @brief TIM模块DMA突发传输长度配置组
  * @{
  * 定义DMA单次触发传输的连续数据包数量配置（1-18个传输）
  * 数值编码规则：实际传输次数N对应寄存器值(N-1)<<8，映射到TIMx_DCR寄存器[15:8]位域
  * 技术特性：
  *   - 支持突发传输模式下的批量数据传输
  *   - 每个传输对应外设寄存器到内存的完整数据搬运过程
  *   - 最大支持18级连续传输，满足不同带宽需求
  */

/* 基本传输单位配置（步进值0x0100对应1个传输量级） */
#define TIM_DMABurstLength_1Transfer           ((uint16_t)0x0000)  // 1次传输（0x00<<8）
#define TIM_DMABurstLength_2Transfers          ((uint16_t)0x0100)  // 2次传输（0x01<<8）
#define TIM_DMABurstLength_3Transfers          ((uint16_t)0x0200)  // 3次传输（0x02<<8）
#define TIM_DMABurstLength_4Transfers          ((uint16_t)0x0300)  // 4次传输（0x03<<8）
#define TIM_DMABurstLength_5Transfers          ((uint16_t)0x0400)  // 5次传输（0x04<<8）
#define TIM_DMABurstLength_6Transfers          ((uint16_t)0x0500)  // 6次传输（0x05<<8）
#define TIM_DMABurstLength_7Transfers          ((uint16_t)0x0600)  // 7次传输（0x06<<8）
#define TIM_DMABurstLength_8Transfers          ((uint16_t)0x0700)  // 8次传输（0x07<<8）
#define TIM_DMABurstLength_9Transfers          ((uint16_t)0x0800)  // 9次传输（0x08<<8）
#define TIM_DMABurstLength_10Transfers         ((uint16_t)0x0900)  // 10次传输（0x09<<8）
#define TIM_DMABurstLength_11Transfers         ((uint16_t)0x0A00)  // 11次传输（0x0A<<8）
#define TIM_DMABurstLength_12Transfers         ((uint16_t)0x0B00)  // 12次传输（0x0B<<8）
#define TIM_DMABurstLength_13Transfers         ((uint16_t)0x0C00)  // 13次传输（0x0C<<8）
#define TIM_DMABurstLength_14Transfers         ((uint16_t)0x0D00)  // 14次传输（0x0D<<8）
#define TIM_DMABurstLength_15Transfers         ((uint16_t)0x0E00)  // 15次传输（0x0E<<8）
#define TIM_DMABurstLength_16Transfers         ((uint16_t)0x0F00)  // 16次传输（0x0F<<8）
#define TIM_DMABurstLength_17Transfers         ((uint16_t)0x1000)  // 17次传输（0x10<<8）
#define TIM_DMABurstLength_18Transfers         ((uint16_t)0x1100)  // 18次传输（0x11<<8，最大值）

/**
  * @brief DMA突发传输长度参数校验宏
  * @param LENGTH 待验证的传输长度参数值
  * @retval 布尔类型校验结果
  * 核心功能：
  *    - 验证输入参数是否为预定义的18种合法传输长度值
  *    - 确保TIM_DCR寄存器配置的突发传输长度在1-18次范围内
  *    - 用于DMA配置时的参数合法性检查
  */
// 校验1次传输配置(0x0000)
// 校验2次传输配置(0x0100)
// 校验3次传输配置(0x0200)
// 校验4次传输配置(0x0300)
// 校验5次传输配置(0x0400)
// 校验6次传输配置(0x0500)
// 校验7次传输配置(0x0600)
// 校验8次传输配置(0x0700)
// 校验9次传输配置(0x0800)
// 校验10次传输配置(0x0900)
// 校验11次传输配置(0x0A00)
// 校验12次传输配置(0x0B00)
// 校验13次传输配置(0x0C00)
// 校验14次传输配置(0x0D00)
// 校验15次传输配置(0x0E00)
// 校验16次传输配置(0x0F00)
// 校验17次传输配置(0x1000)
#define IS_TIM_DMA_LENGTH(LENGTH) ( \
    ((LENGTH) == TIM_DMABurstLength_1Transfer) || \
    ((LENGTH) == TIM_DMABurstLength_2Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_3Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_4Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_5Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_6Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_7Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_8Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_9Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_10Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_11Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_12Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_13Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_14Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_15Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_16Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_17Transfers) || \
    ((LENGTH) == TIM_DMABurstLength_18Transfers))        // 校验18次传输配置(0x1100)

/**
  * @}
  */ 

/** @defgroup TIM_DMA_sources 
  * @brief TIM DMA请求触发源配置组
  * @{
  * 功能说明：
  *    - 定义8种DMA请求触发事件类型
  *    - 使用位标志方式配置DIER寄存器DMA使能位
  *    - 每个配置对应特定硬件事件触发的DMA传输
  */
#define TIM_DMA_Update     ((uint16_t)0x0100)  // 定时器溢出更新事件触发（位8）
#define TIM_DMA_CC1        ((uint16_t)0x0200)  // 通道1比较/捕获事件触发（位9）
#define TIM_DMA_CC2        ((uint16_t)0x0400)  // 通道2比较/捕获事件触发（位10）
#define TIM_DMA_CC3        ((uint16_t)0x0800)  // 通道3比较/捕获事件触发（位11）
#define TIM_DMA_CC4        ((uint16_t)0x1000)  // 通道4比较/捕获事件触发（位12）
#define TIM_DMA_COM        ((uint16_t)0x2000)  // 互补通道事件触发（位13）
#define TIM_DMA_Trigger    ((uint16_t)0x4000)  // 触发事件触发（位14）

/**
  * @brief DMA触发源校验宏
  * @param SOURCE 待校验的触发源组合值
  * 实现原理：通过位掩码(0x80FF)过滤非法位，并排除零值
  */
#define IS_TIM_DMA_SOURCE(SOURCE) ((((SOURCE) & 0x80FF) == 0x0000) && ((SOURCE) != 0x0000))
/**
  * @}
  */ 
/** @defgroup TIM_External_Trigger_Prescaler 
  * @brief 外部触发信号预分频配置组
  * @{
  * 功能说明：
  *    - 配置外部触发输入信号的频率分频系数（1/2/4/8分频）
  *    - 对应SMCR寄存器ETPS[1:0]位域（位12-13）
  *    - 应用场景：需要降低外部触发信号频率时使用
  */
#define TIM_ExtTRGPSC_OFF    ((uint16_t)0x0000)  // 关闭分频（00：每次边沿都触发）
#define TIM_ExtTRGPSC_DIV2   ((uint16_t)0x1000)  // 2分频（01：每2个边沿触发）
#define TIM_ExtTRGPSC_DIV4   ((uint16_t)0x2000)  // 4分频（10：每4个边沿触发）
#define TIM_ExtTRGPSC_DIV8   ((uint16_t)0x3000)  // 8分频（11：每8个边沿触发）

/**
  * @brief 外部触发预分频参数校验宏
  * @param PRESCALER 待验证的分频系数值
  * 实现原理：验证输入值是否为预定义的四种分频模式
  */
// 校验关闭分频模式
// 校验2分频模式
// 校验4分频模式
// 校验8分频模式
#define IS_TIM_EXT_PRESCALER(PRESCALER) ( \
    ((PRESCALER) == TIM_ExtTRGPSC_OFF) || \
    ((PRESCALER) == TIM_ExtTRGPSC_DIV2) || \
    ((PRESCALER) == TIM_ExtTRGPSC_DIV4) || \
    ((PRESCALER) == TIM_ExtTRGPSC_DIV8))   

/** @defgroup TIM_Internal_Trigger_Selection 
  * @brief 内部触发源选择配置组
  * @{
  * 功能说明：
  *    - 定义定时器级联时的内部触发线路选择（ITR0-ITR3）
  *    - 对应SMCR寄存器TS[2:0]位域（位4-6）
  *    - 工作模式：主定时器通过TRGO输出触发从定时器
  */
#define TIM_TS_ITR0       ((uint16_t)0x0000)  // 内部触发线路0（000：ITR0连接）
#define TIM_TS_ITR1       ((uint16_t)0x0010)  // 内部触发线路1（001：ITR1连接）
#define TIM_TS_ITR2       ((uint16_t)0x0020)  // 内部触发线路2（010：ITR2连接）
#define TIM_TS_ITR3       ((uint16_t)0x0030)  // 内部触发线路3（011：ITR3连接）
#define TIM_TS_TI1F_ED    ((uint16_t)0x0040)  // TI1边沿检测触发（100）
#define TIM_TS_TI1FP1     ((uint16_t)0x0050)  // 滤波后TI1输入触发（101）
#define TIM_TS_TI2FP2     ((uint16_t)0x0060)  // 滤波后TI2输入触发（110）
#define TIM_TS_ETRF       ((uint16_t)0x0070)  // 外部触发输入触发（111）

/**
  * @brief 触发源选择参数校验宏（全模式）
  * @param SELECTION 待验证的触发源值
  * 覆盖范围：所有8种触发源模式（包含内部线路和外部输入）
  */
// 校验ITR0触发
// 校验ITR1触发      
// 校验ITR2触发      
// 校验ITR3触发  
// 校验TI1边沿检测
// 校验滤波后TI1输入
// 校验滤波后TI2输入
// 校验外部触发输入
#define IS_TIM_TRIGGER_SELECTION(SELECTION) ( \
    ((SELECTION) == TIM_TS_ITR0) || \
    ((SELECTION) == TIM_TS_ITR1) || \
    ((SELECTION) == TIM_TS_ITR2) || \
    ((SELECTION) == TIM_TS_ITR3) || \
    ((SELECTION) == TIM_TS_TI1F_ED) || \
    ((SELECTION) == TIM_TS_TI1FP1) || \
    ((SELECTION) == TIM_TS_TI2FP2) || \
    ((SELECTION) == TIM_TS_ETRF))          

/**
  * @brief 内部触发源选择专用校验宏
  * @param SELECTION 待验证的触发源值
  * 受限范围：仅ITR0-ITR3四种内部触发线路
  */
// 校验ITR0线路    // 校验ITR1线路    // 校验ITR2线路    // 校验ITR3线路
#define IS_TIM_INTERNAL_TRIGGER_SELECTION(SELECTION) ( \
    ((SELECTION) == TIM_TS_ITR0) || \
    ((SELECTION) == TIM_TS_ITR1) || \
    ((SELECTION) == TIM_TS_ITR2) || \
    ((SELECTION) == TIM_TS_ITR3))    

/**
  * @}
  */ 
/** @defgroup TIM_TIx_External_Clock_Source 
  * @brief 外部时钟源模式1输入选择配置组
  * @{
  * 功能说明：
  *    - 选择特定输入作为外部时钟源
  *    - 对应SMCR寄存器的SMS[2:0]位域（模式1编码）
  *    - 应用场景：需要外部信号驱动定时器时钟
  */
#define TIM_TIxExternalCLK1Source_TI1   ((uint16_t)0x0050)  // TI1输入作为外部时钟（SMS=111）
#define TIM_TIxExternalCLK1Source_TI2   ((uint16_t)0x0060)  // TI2输入作为外部时钟（SMS=111）
#define TIM_TIxExternalCLK1Source_TI1ED ((uint16_t)0x0040)  // TI1边沿检测作为时钟（SMS=110）

/** @defgroup TIM_External_Trigger_Polarity 
  * @brief 外部触发信号极性配置组
  * @{
  * 功能说明：
  *    - 设置外部触发输入的有效边沿极性
  *    - 对应SMCR寄存器ETP位（位15）
  *    - 影响捕获/触发事件检测方式
  */
#define TIM_ExtTRGPolarity_Inverted    ((uint16_t)0x8000)  // 下降沿有效（位15置1）
#define TIM_ExtTRGPolarity_NonInverted ((uint16_t)0x0000)  // 上升沿有效（位15清零）
/**
  * @brief 外部触发极性参数校验宏
  * @param POLARITY 待验证的极性参数值
  * @retval 布尔类型校验结果
  * 核心功能：
  *    - 验证输入参数是否为合法极性配置
  *    - 确保SMCR寄存器ETP位(位15)配置有效性
  *    - 硬件关联：控制外部触发信号边沿检测电路
  */
// 校验下降沿有效模式（0x8000）
// 校验上升沿有效模式（0x0000）
#define IS_TIM_EXT_POLARITY(POLARITY) ( \
    ((POLARITY) == TIM_ExtTRGPolarity_Inverted) || \
    ((POLARITY) == TIM_ExtTRGPolarity_NonInverted)) 
/**
  * @}
  */
/** @defgroup TIM_Prescaler_Reload_Mode 
  * @brief 预分频器重载模式配置组
  * @{
  * 功能说明：
  *    - 控制预分频值(PSC)的更新时序
  *    - 对应EGR寄存器UG位触发机制（位0）
  *    - 应用场景：需要同步更新分频值时使用
  */
#define TIM_PSCReloadMode_Update    ((uint16_t)0x0000)  // 缓冲模式（UG位自动置1）
#define TIM_PSCReloadMode_Immediate ((uint16_t)0x0001)  // 即时模式（UG位手动置1）

/**
  * @brief 预分频重载模式校验宏
  * @param RELOAD 待验证的重载模式值
  * 模式说明：
  *   - Update模式：新预分频值在下个UEV事件生效
  *   - Immediate模式：新预分频值立即生效
  */
     // 校验缓冲更新模式     // 校验即时生效模式
#define IS_TIM_PRESCALER_RELOAD(RELOAD) ( \
    ((RELOAD) == TIM_PSCReloadMode_Update) || \
    ((RELOAD) == TIM_PSCReloadMode_Immediate)) 

/** @defgroup TIM_Forced_Action 
  * @brief 通道强制输出配置组
  * @{
  * 功能说明：
  *    - 手动覆盖通道输出比较状态
  *    - 对应CR2寄存器MMS位域（位4-6）
  *    - 紧急控制场景：如过载保护时强制关闭输出
  */
#define TIM_ForcedAction_Active   ((uint16_t)0x0050)  // 强制输出有效电平（MMS=101）
#define TIM_ForcedAction_InActive ((uint16_t)0x0040)  // 强制输出无效电平（MMS=100）

/** @defgroup TIM_Encoder_Mode 
  * @brief 增量编码器接口模式配置组
  * @{
  * 功能说明：
  *    - 配置正交编码器的信号解码规则
  *    - 对应SMCR寄存器SMS[2:0]位域（位0-2）
  *    - 支持AB相/TI1/TI2独立输入检测
  */
#define TIM_EncoderMode_TI1  ((uint16_t)0x0001)  // TI1边沿检测模式（SMS=001）
#define TIM_EncoderMode_TI2  ((uint16_t)0x0002)  // TI2边沿检测模式（SMS=010）
#define TIM_EncoderMode_TI12 ((uint16_t)0x0003)  // TI1/TI2正交模式（SMS=011）

/**
  * @brief 编码器模式参数校验宏
  * @param MODE 待验证的编码器模式值
  * 验证要求：必须为TI1/TI2独立模式或TI1-TI2正交模式
  */
    // 校验TI1单通道模式    // 校验TI2单通道模式      // 校验双通道正交模式
#define IS_TIM_ENCODER_MODE(MODE) ( \
    ((MODE) == TIM_EncoderMode_TI1) || \
    ((MODE) == TIM_EncoderMode_TI2) || \
    ((MODE) == TIM_EncoderMode_TI12)) 

/**
  * @}
  */ 


/** @defgroup TIM_Event_Source 
  * @{
  */

/** @defgroup TIM_Event_Source 
  * @brief 定时器事件触发源配置组
  * @{
  * 功能说明：
  *    - 定义8种硬件事件类型用于触发中断/DMA请求
  *    - 对应DIER寄存器位0-7，支持位或组合配置
  *    - 应用场景：需要实时响应特定硬件事件时使用
  */
#define TIM_EventSource_Update    ((uint16_t)0x0001)  // 更新事件（DIER.0）
#define TIM_EventSource_CC1       ((uint16_t)0x0002)  // 通道1比较事件（DIER.1）
#define TIM_EventSource_CC2       ((uint16_t)0x0004)  // 通道2比较事件（DIER.2）
#define TIM_EventSource_CC3       ((uint16_t)0x0008)  // 通道3比较事件（DIER.3）
#define TIM_EventSource_CC4       ((uint16_t)0x0010)  // 通道4比较事件（DIER.4）
#define TIM_EventSource_COM       ((uint16_t)0x0020)  // 互补输出事件（DIER.5）
#define TIM_EventSource_Trigger   ((uint16_t)0x0040)  // 触发事件（DIER.6）
#define TIM_EventSource_Break     ((uint16_t)0x0080)  // 断路输入事件（DIER.7）

/**
  * @brief 事件源参数校验宏
  * @param SOURCE 待验证的事件源组合值
  * 实现原理：
  *    - 0xFF00掩码过滤高位无效位（DIER寄存器仅使用低8位）
  *    - 排除0x0000无效组合（至少选择1个事件源）
  */
// 校验高位无干扰位
// 排除零值配置
#define IS_TIM_EVENT_SOURCE(SOURCE) ( \
    (((SOURCE) & 0xFF00) == 0x0000) && \
    ((SOURCE) != 0x0000))             

/** @defgroup TIM_Update_Source 
  * @brief 更新事件触发源配置组
  * @{
  * 功能说明：
  *    - 控制更新事件(UEV)的生成条件
  *    - 对应CR1寄存器URS位（位2）
  *    - 影响ARR/PSC寄存器更新同步性
  */
#define TIM_UpdateSource_Global   ((uint16_t)0x0000)  /* 全局触发源（URS=0）
                                                        * 包含：
                                                        * - 计数器溢出/下溢
                                                        * - 手动设置UG位
                                                        * - 从模式控制器触发 */
#define TIM_UpdateSource_Regular  ((uint16_t)0x0001)  /* 规则触发源（URS=1）
                                                        * 仅由计数器溢出/下溢触发 */

/**
  * @brief 更新源参数校验宏
  * @param SOURCE 待验证的更新源模式
  * 技术规范：必须为Global或Regular两种模式之一
  */
// 校验全局更新模式
// 校验规则更新模式
#define IS_TIM_UPDATE_SOURCE(SOURCE) ( \
    ((SOURCE) == TIM_UpdateSource_Global) || \
    ((SOURCE) == TIM_UpdateSource_Regular)) 
/** @defgroup TIM_Output_Compare_Preload_State 
  * @brief 输出比较预加载配置组
  * @{
  * 功能说明：
  *    - 控制CCR寄存器的双缓冲机制
  *    - 对应CCMRx寄存器OCxPE位（位3）
  *    - 启用后写入CCR的值将在更新事件后生效
  */
#define TIM_OCPreload_Enable  ((uint16_t)0x0008)  // 启用预加载（OCxPE=1）
#define TIM_OCPreload_Disable ((uint16_t)0x0000)  // 禁用预加载（OCxPE=0）

/**
  * @brief 预加载状态校验宏
  * @param STATE 待验证的预加载配置值
  * 实现原理：验证参数是否为合法预加载模式
  */
// 校验启用状态
// 校验禁用状态
#define IS_TIM_OCPRELOAD_STATE(STATE) ( \
    ((STATE) == TIM_OCPreload_Enable) || \
    ((STATE) == TIM_OCPreload_Disable))   

/** @defgroup TIM_Output_Compare_Fast_State 
  * @brief 输出比较快速模式配置组
  * @{
  * 功能说明：
  *    - 控制比较匹配事件响应速度
  *    - 对应CCMRx寄存器OCxFE位（位2）
  *    - 快速模式可减少输出响应延迟
  */
#define TIM_OCFast_Enable   ((uint16_t)0x0004)  // 启用快速模式（OCxFE=1）
#define TIM_OCFast_Disable  ((uint16_t)0x0000)  // 禁用快速模式（OCxFE=0）

/** @defgroup TIM_Output_Compare_Clear_State 
  * @brief 输出清除功能配置组
  * @{
  * 功能说明：
  *    - 配置比较输出自动清除机制
  *    - 对应CCER寄存器CCxP位（位1）
  *    - 与断路功能配合实现硬件保护
  */
#define TIM_OCClear_Enable  ((uint16_t)0x0080)  // 启用自动清除（CCxP=1）
#define TIM_OCClear_Disable ((uint16_t)0x0000)  // 禁用自动清除（CCxP=0）

/** @defgroup TIM_Trigger_Output_Source 
  * @brief 主模式触发输出源配置组
  * @{
  * 功能说明：
  *    - 选择TRGO引脚输出信号源
  *    - 对应CR2寄存器MMS[2:0]位域（位4-6）
  *    - 用于定时器级联或外设同步
  */
#define TIM_TRGOSource_Reset    ((uint16_t)0x0000)  // 复位事件触发（MMS=000）
#define TIM_TRGOSource_Enable   ((uint16_t)0x0010)  // 定时器使能触发（MMS=001）
#define TIM_TRGOSource_Update   ((uint16_t)0x0020)  // 更新事件触发（MMS=010）
#define TIM_TRGOSource_OC1     ((uint16_t)0x0030)  // 通道1比较触发（MMS=011）
#define TIM_TRGOSource_OC1Ref  ((uint16_t)0x0040)  // 通道1REF信号触发（MMS=100）
#define TIM_TRGOSource_OC2Ref  ((uint16_t)0x0050)  // 通道2REF信号触发（MMS=101）
#define TIM_TRGOSource_OC3Ref  ((uint16_t)0x0060)  // 通道3REF信号触发（MMS=110）
#define TIM_TRGOSource_OC4Ref  ((uint16_t)0x0070)  // 通道4REF信号触发（MMS=111）

/**
  * @brief 触发源参数校验宏
  * @param SOURCE 待验证的触发源模式
  * 验证范围：8种预设触发源配置
  */
// 校验复位触发
// 校验使能触发
// 校验更新触发
// 校验OC1原始信号
// 校验OC1REF信号
// 校验OC2REF信号
// 校验OC3REF信号
// 校验OC4REF信号
#define IS_TIM_TRGO_SOURCE(SOURCE) ( \
    ((SOURCE) == TIM_TRGOSource_Reset) || \
    ((SOURCE) == TIM_TRGOSource_Enable) || \
    ((SOURCE) == TIM_TRGOSource_Update) || \
    ((SOURCE) == TIM_TRGOSource_OC1) || \
    ((SOURCE) == TIM_TRGOSource_OC1Ref) || \
    ((SOURCE) == TIM_TRGOSource_OC2Ref) || \
    ((SOURCE) == TIM_TRGOSource_OC3Ref) || \
    ((SOURCE) == TIM_TRGOSource_OC4Ref)) 

/**
  * @}
  */ 
/** @defgroup TIM_Slave_Mode 
  * @brief 定时器从模式配置组
  * @{
  * 功能说明：
  *    - 定义从定时器对触发信号的响应方式
  *    - 对应SMCR寄存器SMS[2:0]位域（位0-2）
  *    - 应用场景：主从定时器级联时配置从定时器行为
  */
#define TIM_SlaveMode_Reset        ((uint16_t)0x0004)  // 复位模式（SMS=100）
#define TIM_SlaveMode_Gated        ((uint16_t)0x0005)  // 门控模式（SMS=101）
#define TIM_SlaveMode_Trigger      ((uint16_t)0x0006)  // 触发模式（SMS=110）
#define TIM_SlaveMode_External1    ((uint16_t)0x0007)  // 外部时钟模式1（SMS=111）

/**
  * @brief 从模式参数校验宏
  * @param MODE 待验证的从模式配置值
  * 验证范围：4种标准从模式配置
  */
// 校验复位模式
// 校验门控模式
// 校验触发模式
// 校验外部时钟模式
#define IS_TIM_SLAVE_MODE(MODE) ( \
    ((MODE) == TIM_SlaveMode_Reset) || \
    ((MODE) == TIM_SlaveMode_Gated) || \
    ((MODE) == TIM_SlaveMode_Trigger) || \
    ((MODE) == TIM_SlaveMode_External1)) 

/** @defgroup TIM_Master_Slave_Mode 
  * @brief 主从模式使能配置组
  * @{
  * 功能说明：
  *    - 控制主从定时器的同步功能
  *    - 对应SMCR寄存器MSM位（位7）
  *    - 启用后主定时器TRGO信号触发从定时器
  */
#define TIM_MasterSlaveMode_Enable  ((uint16_t)0x0080)  // 启用主从同步（MSM=1）
#define TIM_MasterSlaveMode_Disable ((uint16_t)0x0000)  // 禁用主从同步（MSM=0）

/** @defgroup TIM_Remap 
  * @brief 定时器引脚重映射配置组
  * @{
  * 功能说明：
  *    - 配置定时器外部信号输入源的重映射
  *    - 对应AFIO寄存器配置位
  *    - 允许连接特殊功能信号（如USB SOF、RTC等）
  */
/* TIM2重映射配置 */
#define TIM2_TIM8_TRGO     ((uint16_t)0x0000)  // ETR连接到TIM8_TRGO（默认）
#define TIM2_ETH_PTP       ((uint16_t)0x0400)  // ETR连接到以太网PTP时间戳
#define TIM2_USBFS_SOF     ((uint16_t)0x0800)  // ETR连接到USB FS SOF
#define TIM2_USBHS_SOF     ((uint16_t)0x0C00)  // ETR连接到USB HS SOF

/* TIM5重映射配置 */
#define TIM5_GPIO          ((uint16_t)0x0000)  // ETR连接到GPIO（默认）
#define TIM5_LSI           ((uint16_t)0x0040)  // ETR连接到内部低速时钟(LSI)
#define TIM5_LSE           ((uint16_t)0x0080)  // ETR连接到外部低速时钟(LSE)
#define TIM5_RTC           ((uint16_t)0x00C0)  // ETR连接到RTC时钟

/* TIM11重映射配置 */
#define TIM11_GPIO         ((uint16_t)0x0000)  // ETR连接到GPIO（默认）
#define TIM11_HSE          ((uint16_t)0x0002)  // ETR连接到高速外部时钟(HSE)

/**
  * @brief 重映射参数校验宏
  * @param TIM_REMAP 待验证的重映射配置值
  * 覆盖范围：所有定时器的重映射选项
  */
// 校验TIM2默认映射
// 校验TIM2以太网映射
// 校验TIM2 USB FS映射
// 校验TIM2 USB HS映射
// 校验TIM5默认映射
// 校验TIM5 LSI映射
// 校验TIM5 LSE映射
// 校验TIM5 RTC映射
// 校验TIM11默认映射
// 校验TIM11 HSE映射
#define IS_TIM_REMAP(TIM_REMAP) ( \
    ((TIM_REMAP) == TIM2_TIM8_TRGO) || \
    ((TIM_REMAP) == TIM2_ETH_PTP) || \
    ((TIM_REMAP) == TIM2_USBFS_SOF) || \
    ((TIM_REMAP) == TIM2_USBHS_SOF) || \
    ((TIM_REMAP) == TIM5_GPIO) || \
    ((TIM_REMAP) == TIM5_LSI) || \
    ((TIM_REMAP) == TIM5_LSE) || \
    ((TIM_REMAP) == TIM5_RTC) || \
    ((TIM_REMAP) == TIM11_GPIO) || \
    ((TIM_REMAP) == TIM11_HSE))  


/**
  * @}
  */ 
/** @defgroup TIM_Flags 
  * @{
  */
/** @defgroup TIM_Flags 
  * @brief 定时器状态标志配置组
  * @{
  * 功能说明：
  *    - 定义定时器状态寄存器(SR)的各类事件标志位
  *    - 用于查询中断/DMA请求状态及溢出检测
  *    - 清除标志需手动写0操作
  */
#define TIM_FLAG_Update     ((uint16_t)0x0001)  // 更新事件标志（SR.0）
#define TIM_FLAG_CC1        ((uint16_t)0x0002)  // 通道1捕获/比较标志（SR.1）
#define TIM_FLAG_CC2        ((uint16_t)0x0004)  // 通道2捕获/比较标志（SR.2）
#define TIM_FLAG_CC3        ((uint16_t)0x0008)  // 通道3捕获/比较标志（SR.3）
#define TIM_FLAG_CC4        ((uint16_t)0x0010)  // 通道4捕获/比较标志（SR.4）
#define TIM_FLAG_COM        ((uint16_t)0x0020)  // 互补事件标志（SR.5）
#define TIM_FLAG_Trigger    ((uint16_t)0x0040)  // 触发事件标志（SR.6）
#define TIM_FLAG_Break      ((uint16_t)0x0080)  // 断路输入标志（SR.7）
#define TIM_FLAG_CC1OF      ((uint16_t)0x0200)  // 通道1溢出标志（SR.9）
#define TIM_FLAG_CC2OF      ((uint16_t)0x0400)  // 通道2溢出标志（SR.10）
#define TIM_FLAG_CC3OF      ((uint16_t)0x0800)  // 通道3溢出标志（SR.11）
#define TIM_FLAG_CC4OF      ((uint16_t)0x1000)  // 通道4溢出标志（SR.12）

/**
  * @brief 状态标志校验宏
  * @param FLAG 待验证的状态标志值
  * 验证范围：所有12种预定义标志位
  */
// 校验更新事件标志        // 校验通道1事件标志        // 校验通道2事件标志
// 校验通道3事件标志        // 校验通道4事件标志        // 校验互补事件标志
// 校验触发事件标志      // 校验断路输入标志      // 校验通道1溢出标志
// 校验通道2溢出标志      // 校验通道3溢出标志   // 校验通道4溢出标志
#define IS_TIM_GET_FLAG(FLAG) ( \
    ((FLAG) == TIM_FLAG_Update) || \
    ((FLAG) == TIM_FLAG_CC1) || \
    ((FLAG) == TIM_FLAG_CC2) || \
    ((FLAG) == TIM_FLAG_CC3) || \
    ((FLAG) == TIM_FLAG_CC4) || \
    ((FLAG) == TIM_FLAG_COM) || \
    ((FLAG) == TIM_FLAG_Trigger) || \
    ((FLAG) == TIM_FLAG_Break) || \
    ((FLAG) == TIM_FLAG_CC1OF) || \
    ((FLAG) == TIM_FLAG_CC2OF) || \
    ((FLAG) == TIM_FLAG_CC3OF) || \
    ((FLAG) == TIM_FLAG_CC4OF))       

/** @defgroup TIM_Input_Capture_Filer_Value 
  * @brief 输入捕获滤波器配置组
  * @{
  * 功能说明：
  *    - 配置输入信号的数字滤波器采样参数
  *    - 对应CCMRx寄存器ICxF[3:0]位域
  *    - 滤波系数N= (f_DTS频率)/(f_SAMPLING采样频率)
  */
#define IS_TIM_IC_FILTER(ICFILTER) ((ICFILTER) <= 0xF) // 滤波器值范围0x0-0xF

/** @defgroup TIM_External_Trigger_Filter 
  * @brief 外部触发滤波器配置组
  * @{
  * 功能说明：
  *    - 配置外部触发信号的噪声滤波参数
  *    - 对应SMCR寄存器ETF[3:0]位域
  *    - 有效降低触发信号抖动影响
  */
#define IS_TIM_EXT_FILTER(EXTFILTER) ((EXTFILTER) <= 0xF) // 滤波器值范围0x0-0xF
/**
  * @defgroup TIM_Legacy 
  * @brief TIM模块历史兼容性定义组
  * @{
  * 功能说明：
  *    - 提供基于字节单位的DMA突发长度旧版宏定义
  *    - 实际映射到以寄存器传输次数为单位的现代宏
  *    - 保持与早期版本代码的兼容性（1个传输=4字节）
  * 重要说明：
  *    - 新工程建议直接使用xTransfers系列宏
  *    - 该组宏将在未来版本中废弃
  */

/* 字节单位与传输次数换算公式：N字节 = (N+3)/4 次传输 */
#define TIM_DMABurstLength_1Byte           TIM_DMABurstLength_1Transfer  // 1字节=1次传输（实际传输4字节）
#define TIM_DMABurstLength_2Bytes          TIM_DMABurstLength_2Transfers  // 2字节=2次传输（实际传输8字节）
#define TIM_DMABurstLength_3Bytes          TIM_DMABurstLength_3Transfers  // 3字节=3次传输（实际传输12字节）
#define TIM_DMABurstLength_4Bytes          TIM_DMABurstLength_4Transfers  // 4字节=4次传输（实际传输16字节）
#define TIM_DMABurstLength_5Bytes          TIM_DMABurstLength_5Transfers  // 5字节=5次传输（实际传输20字节）
#define TIM_DMABurstLength_6Bytes          TIM_DMABurstLength_6Transfers  // 6字节=6次传输（实际传输24字节）
#define TIM_DMABurstLength_7Bytes          TIM_DMABurstLength_7Transfers  // 7字节=7次传输（实际传输28字节）
#define TIM_DMABurstLength_8Bytes          TIM_DMABurstLength_8Transfers  // 8字节=8次传输（实际传输32字节）
#define TIM_DMABurstLength_9Bytes          TIM_DMABurstLength_9Transfers  // 9字节=9次传输（实际传输36字节）
#define TIM_DMABurstLength_10Bytes         TIM_DMABurstLength_10Transfers  // 10字节=10次传输（实际传输40字节）
#define TIM_DMABurstLength_11Bytes         TIM_DMABurstLength_11Transfers  // 11字节=11次传输（实际传输44字节）
#define TIM_DMABurstLength_12Bytes         TIM_DMABurstLength_12Transfers  // 12字节=12次传输（实际传输48字节）
#define TIM_DMABurstLength_13Bytes         TIM_DMABurstLength_13Transfers  // 13字节=13次传输（实际传输52字节）
#define TIM_DMABurstLength_14Bytes         TIM_DMABurstLength_14Transfers  // 14字节=14次传输（实际传输56字节）
#define TIM_DMABurstLength_15Bytes         TIM_DMABurstLength_15Transfers  // 15字节=15次传输（实际传输60字节）
#define TIM_DMABurstLength_16Bytes         TIM_DMABurstLength_16Transfers  // 16字节=16次传输（实际传输64字节）
#define TIM_DMABurstLength_17Bytes         TIM_DMABurstLength_17Transfers  // 17字节=17次传输（实际传输68字节）
#define TIM_DMABurstLength_18Bytes         TIM_DMABurstLength_18Transfers  // 18字节=18次传输（实际传输72字节）

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup TIM_TimeBase_Management
  * @brief 定时器时基配置函数组
  * @{
  * 功能说明：
  *    - 包含定时器基础配置、计数模式设置等核心操作
  *    - 控制计数器的启动/停止、重载值配置等核心功能
  */
void TIM_DeInit(TIM_TypeDef* TIMx);                              // 复位定时器外设
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, 
                     TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct); // 初始化时基单元
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct); // 初始化时基结构体
void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, 
                        uint16_t TIM_PSCReloadMode);            // 配置预分频器及重载模式
void TIM_CounterModeConfig(TIM_TypeDef* TIMx, 
                          uint16_t TIM_CounterMode);           // 设置计数模式（上/下/中央对齐）
void TIM_SetCounter(TIM_TypeDef* TIMx, uint32_t Counter);      // 直接写入计数器值
void TIM_SetAutoreload(TIM_TypeDef* TIMx, uint32_t Autoreload); // 设置自动重载值
uint32_t TIM_GetCounter(TIM_TypeDef* TIMx);                     // 读取当前计数值
uint16_t TIM_GetPrescaler(TIM_TypeDef* TIMx);                  // 获取当前预分频值
void TIM_UpdateDisableConfig(TIM_TypeDef* TIMx, 
                            FunctionalState NewState);         // 使能/禁用更新事件
void TIM_UpdateRequestConfig(TIM_TypeDef* TIMx, 
                            uint16_t TIM_UpdateSource);        // 配置更新请求源
void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, 
                         FunctionalState NewState);            // ARR预加载使能控制
void TIM_SelectOnePulseMode(TIM_TypeDef* TIMx, 
                           uint16_t TIM_OPMode);             // 选择单脉冲模式
void TIM_SetClockDivision(TIM_TypeDef* TIMx, uint16_t TIM_CKD); // 设置时钟分频因子
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);    // 启动/停止定时器
/** @} */


/** @addtogroup TIM_OC_Management
  * @brief 输出比较功能管理函数组
  * @{
  * 功能说明：
  *    - 配置PWM输出、强制输出等通道级操作
  *    - 控制占空比、输出极性等参数
  */
void TIM_OC1Init(TIM_TypeDef* TIMx, 
                TIM_OCInitTypeDef* TIM_OCInitStruct);         // 初始化通道1输出比较
void TIM_OC2Init(TIM_TypeDef* TIMx, 
                TIM_OCInitTypeDef* TIM_OCInitStruct);         // 初始化通道2输出比较
void TIM_OC3Init(TIM_TypeDef* TIMx, 
                TIM_OCInitTypeDef* TIM_OCInitStruct);         // 初始化通道3输出比较
void TIM_OC4Init(TIM_TypeDef* TIMx, 
                TIM_OCInitTypeDef* TIM_OCInitStruct);         // 初始化通道4输出比较
void TIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct);   // 初始化OC结构体默认值
void TIM_SelectOCxM(TIM_TypeDef* TIMx, uint16_t TIM_Channel, 
                   uint16_t TIM_OCMode);                     // 动态切换输出比较模式
void TIM_SetCompare1(TIM_TypeDef* TIMx, uint32_t Compare1);    // 设置通道1比较值
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint32_t Compare2);    // 设置通道2比较值  
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint32_t Compare3);    // 设置通道3比较值
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint32_t Compare4);    // 设置通道4比较值

/* TIM输出比较模块函数组 -------------------------------------------------------*/
/**
  * @brief  强制配置输出比较通道1模式
  * @param  TIMx 定时器外设指针，如TIM1、TIM2等
  * @param  TIM_ForcedAction 强制输出动作，可选：
  *         TIM_ForcedAction_Active  强制输出有效电平
  *         TIM_ForcedAction_Inactive 强制输出无效电平
  * @note   该配置会覆盖当前输出比较模式，通常用于紧急控制输出状态
  */
void TIM_ForcedOC1Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);

// 以下为通道2-4的强制输出配置函数，功能逻辑与通道1相同
void TIM_ForcedOC2Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);
void TIM_ForcedOC3Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);
void TIM_ForcedOC4Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);

/**
  * @brief  配置输出比较通道1预加载功能
  * @param  TIMx 定时器外设指针
  * @param  TIM_OCPreload 预加载使能选项：
  *         TIM_OCPreload_Enable  使能预加载寄存器
  *         TIM_OCPreload_Disable 禁用预加载寄存器
  * @note   使能后CCR寄存器值将在更新事件发生时才会生效，避免中间值干扰
  */
void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);

// 通道2-4预加载配置（功能同通道1）
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);

/**
  * @brief  配置输出比较通道1快速模式
  * @param  TIMx 定时器外设指针
  * @param  TIM_OCFast 快速模式使能：
  *         TIM_OCFast_Enable  比较匹配时立即改变输出
  *         TIM_OCFast_Disable 输出在定时器周期结束后更新
  * @note   快速模式可减少输出响应延迟，适用于需要快速切换输出的场景
  */
void TIM_OC1FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);

// 通道2-4快速模式配置（功能同通道1）
void TIM_OC2FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);
void TIM_OC3FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);
void TIM_OC4FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);

/**
  * @brief  清除输出比较通道1的参考信号
  * @param  TIMx 定时器外设指针
  * @param  TIM_OCClear 清除方式选项：
  *         TIM_OCClear_Enable  强制输出低电平
  *         TIM_OCClear_Disable 保持当前输出
  * @note   用于在特定条件下（如刹车事件）立即清除输出信号
  */
void TIM_ClearOC1Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);

// 通道2-4参考信号清除（功能同通道1）
void TIM_ClearOC2Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);
void TIM_ClearOC3Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);
void TIM_ClearOC4Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);

/* 输出极性配置 --------------------------------------------------------------*/
/**
  * @brief  配置输出通道1主输出极性
  * @param  TIMx 定时器外设指针
  * @param  TIM_OCPolarity 输出极性：
  *         TIM_OCPolarity_High 高电平有效
  *         TIM_OCPolarity_Low  低电平有效
  * @note   影响PWM输出的有效电平极性，需与硬件电路设计匹配
  */
void TIM_OC1PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);

/**
  * @brief  配置输出通道1互补输出极性
  * @param  TIMx 定时器外设指针
  * @param  TIM_OCNPolarity 互补输出极性
  * @note   用于高级定时器的互补PWM输出配置（如TIM1/TIM8）
  */
void TIM_OC1NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity);

// 通道2-3主输出/互补输出极性配置（功能同通道1）
void TIM_OC2PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);
void TIM_OC2NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity);
void TIM_OC3PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);
void TIM_OC3NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity);

/**
  * @brief  配置输出通道4极性（无互补通道）
  * @param  TIMx 定时器外设指针
  * @param  TIM_OCPolarity 输出极性
  */
void TIM_OC4PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);

/* 通道使能控制 --------------------------------------------------------------*/
/**
  * @brief  使能/禁用指定输出通道
  * @param  TIMx 定时器外设指针
  * @param  TIM_Channel 通道选择：
  *         TIM_Channel_1/2/3/4
  * @param  TIM_CCx 通道状态：
  *         TIM_CCx_Enable  使能通道
  *         TIM_CCx_Disable 禁用通道
  * @note   禁用通道后对应的输出引脚将不受定时器控制
  */
void TIM_CCxCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx);

/**
  * @brief  使能/禁用互补输出通道
  * @param  TIMx 定时器外设指针
  * @param  TIM_Channel 主通道选择（1-3）
  * @param  TIM_CCxN 互补通道状态
  * @note   仅适用于具有互补输出的高级定时器（TIM1/TIM8）
  */
void TIM_CCxNCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCxN);
/* 输入捕获管理模块 **********************************************************/
/**
  * @brief  初始化定时器输入捕获通道
  * @param  TIMx 定时器外设指针
  * @param  TIM_ICInitStruct 输入捕获配置结构体指针
  * @note   配置捕获边沿、滤波器、分频器等参数，支持PWM输入模式配置
  */
void TIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);

/**
  * @brief  初始化输入捕获配置结构体默认值
  * @param  TIM_ICInitStruct 待初始化的结构体指针
  * @note   将通道配置为上升沿捕获，滤波器关闭，分频器1分频
  */
void TIM_ICStructInit(TIM_ICInitTypeDef* TIM_ICInitStruct);

/**
  * @brief  配置PWM输入模式
  * @param  TIMx 定时器外设指针
  * @param  TIM_ICInitStruct 输入捕获配置结构体
  * @note   自动配置两个通道（通道1/2或通道3/4）实现PWM周期占空比测量
  */
void TIM_PWMIConfig(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);

/* 捕获值读取 ---------------------------------------------------------------*/
/**
  * @brief  获取输入捕获通道1的捕获值
  * @param  TIMx 定时器外设指针
  * @retval 最近一次成功捕获的计数器值
  */
uint32_t TIM_GetCapture1(TIM_TypeDef* TIMx);

// 通道2-4捕获值读取函数（功能同通道1）
uint32_t TIM_GetCapture2(TIM_TypeDef* TIMx);
uint32_t TIM_GetCapture3(TIM_TypeDef* TIMx);
uint32_t TIM_GetCapture4(TIM_TypeDef* TIMx);

/* 输入预分频配置 -----------------------------------------------------------*/
/**
  * @brief  设置输入捕获通道1预分频系数
  * @param  TIMx 定时器外设指针
  * @param  TIM_ICPSC 预分频系数：
  *         TIM_ICPSC_DIV1/2/4/8
  * @note   用于降低输入信号的采样频率，实现噪声滤波
  */
void TIM_SetIC1Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);

// 通道2-4输入预分频配置（功能同通道1）
void TIM_SetIC2Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
void TIM_SetIC3Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
void TIM_SetIC4Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);


/* 高级定时器特性配置（TIM1/TIM8专用）*****************************************/
/**
  * @brief  配置刹车和死区时间寄存器（BDTR）
  * @param  TIMx 高级定时器指针（仅TIM1/TIM8）
  * @param  TIM_BDTRInitStruct BDTR配置结构体指针
  * @note   配置死区时间、刹车信号极性、自动输出使能等高级功能
  */
void TIM_BDTRConfig(TIM_TypeDef* TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);

/**
  * @brief  初始化BDTR配置结构体默认值
  * @param  TIM_BDTRInitStruct 待初始化的结构体指针
  * @note   默认关闭所有刹车功能，死区时间设为0
  */
void TIM_BDTRStructInit(TIM_BDTRInitTypeDef* TIM_BDTRInitStruct);

/**
  * @brief  控制PWM输出使能状态
  * @param  TIMx 高级定时器指针（TIM1/TIM8）
  * @param  NewState 输出使能状态（ENABLE/DISABLE）
  * @note   主输出使能（MOE）位控制，禁用时所有PWM输出进入无效状态
  */
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);

/**
  * @brief  选择捕获比较控制寄存器预装载功能
  * @param  TIMx 定时器外设指针
  * @param  NewState 预装载使能状态（ENABLE/DISABLE）
  * @note   使能时CCR/CER寄存器在更新事件时才会加载新值
  */
void TIM_CCPreloadControl(TIM_TypeDef* TIMx, FunctionalState NewState);


/* 中断与事件管理 ************************************************************/
/**
  * @brief  配置定时器中断使能
  * @param  TIMx 定时器外设指针
  * @param  TIM_IT 中断源选择，如：
  *         TIM_IT_Update/CC1/Trigger等
  * @param  NewState 中断使能状态（ENABLE/DISABLE）
  */
void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);

/**
  * @brief  生成定时器软件事件
  * @param  TIMx 定时器外设指针
  * @param  TIM_EventSource 事件源选择，如：
  *         TIM_EventSource_Update/CC1/Trigger等
  * @note   用于手动触发更新事件或捕获比较事件
  */
void TIM_GenerateEvent(TIM_TypeDef* TIMx, uint16_t TIM_EventSource);

/* 标志位管理 ---------------------------------------------------------------*/
/**
  * @brief  获取定时器标志位状态
  * @param  TIMx 定时器外设指针
  * @param  TIM_FLAG 待查询的标志位，如：
  *         TIM_FLAG_Update/CC1/Trigger等
  * @retval SET/RESET 标志位状态
  */
FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);

/**
  * @brief  清除定时器标志位
  * @param  TIMx 定时器外设指针
  * @param  TIM_FLAG 待清除的标志位
  */
void TIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);

/* 中断状态管理 -------------------------------------------------------------*/
/**
  * @brief  获取定时器中断状态
  * @param  TIMx 定时器外设指针
  * @param  TIM_IT 中断源查询
  * @retval SET/RESET 中断状态
  * @note   需同时检查标志位和中断使能位的状态
  */
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);

/**
  * @brief  清除定时器中断挂起位
  * @param  TIMx 定时器外设指针
  * @param  TIM_IT 待清除的中断源
  */
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);


/* DMA管理配置 ***************************************************************/
/**
  * @brief  配置定时器DMA基础地址和突发长度
  * @param  TIMx 定时器外设指针
  * @param  TIM_DMABase DMA基础地址选择：
  *         TIM_DMABase_CR1/DIER等（寄存器基地址偏移）
  * @param  TIM_DMABurstLength DMA突发传输长度（1-18）
  */
void TIM_DMAConfig(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength);

/**
  * @brief  使能/禁用定时器DMA请求
  * @param  TIMx 定时器外设指针
  * @param  TIM_DMASource DMA请求源选择：
  *         TIM_DMA_Update/CC1/Trigger等
  * @param  NewState DMA使能状态（ENABLE/DISABLE）
  */
void TIM_DMACmd(TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState);

/**
  * @brief  选择捕获比较DMA请求源
  * @param  TIMx 定时器外设指针
  * @param  NewState 选择状态（ENABLE：CCx事件触发DMA）
  */
void TIM_SelectCCDMA(TIM_TypeDef* TIMx, FunctionalState NewState);

/* 时钟源管理模块 ***********************************************************/
/**
  * @brief  配置定时器使用内部时钟源
  * @param  TIMx 定时器外设指针
  * @note   默认时钟配置，使用APB总线时钟作为定时器时钟源
  */
void TIM_InternalClockConfig(TIM_TypeDef* TIMx);

/**
  * @brief  配置内部触发输入作为外部时钟源
  * @param  TIMx 定时器外设指针
  * @param  TIM_InputTriggerSource 内部触发源选择：
  *         TIM_TS_ITR0/1/2/3（对应不同定时器级联）
  * @note   用于定时器级联场景，例如TIM2接收TIM1的触发信号
  */
void TIM_ITRxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);

/**
  * @brief  配置外部输入引脚作为时钟源
  * @param  TIMx 定时器外设指针
  * @param  TIM_TIxExternalCLKSource 输入通道选择：
  *         TIM_TIxExternalCLK1Source_TI1/2（对应通道1/2输入）
  * @param  TIM_ICPolarity 输入极性
  * @param  ICFilter 输入滤波器系数（0x0-0xF）
  * @note   适用于外部时钟模式2，通过TI1/TI2边沿触发计数器
  */
void TIM_TIxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_TIxExternalCLKSource,
                                uint16_t TIM_ICPolarity, uint16_t ICFilter);

/* 外部触发时钟模式 ---------------------------------------------------------*/
/**
  * @brief  配置ETR引脚时钟模式1（复位模式）
  * @param  TIMx 定时器外设指针
  * @param  TIM_ExtTRGPrescaler 外部触发预分频：
  *         ETR_Prescaler_OFF/2/4/8
  * @param  TIM_ExtTRGPolarity 触发极性：
  *         ETRPolarity_Inverted/NonInverted
  * @param  ExtTRGFilter 滤波器系数（0x0-0xF）
  * @note   每个有效边沿触发计数器复位并重新计数
  */
void TIM_ETRClockMode1Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, 
                            uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);

/**
  * @brief  配置ETR引脚时钟模式2（门控模式）
  * @param  TIMx 定时器外设指针
  * @param  参数同模式1配置
  * @note   外部信号作为计数使能门控，高电平期间允许计数
  */
void TIM_ETRClockMode2Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
                            uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);


/* 定时器同步管理 ***********************************************************/
/**
  * @brief  选择输入触发源
  * @param  TIMx 定时器外设指针
  * @param  TIM_InputTriggerSource 触发源选择：
  *         TIM_TS_ITR0/1/2/3（内部）或 TI1F_ED/TI1FP1/TI2FP2（外部）
  */
void TIM_SelectInputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);

/**
  * @brief  选择输出触发源
  * @param  TIMx 定时器外设指针
  * @param  TIM_TRGOSource 触发输出源选择：
  *         TIM_TRGOSource_Reset/Enable/Update等
  * @note   用于级联定时器或触发外部设备
  */
void TIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource);

/**
  * @brief  配置从模式
  * @param  TIMx 定时器外设指针
  * @param  TIM_SlaveMode 从模式选择：
  *         TIM_SlaveMode_Reset/Gated/Trigger等
  * @note   定义定时器如何响应来自主定时器的触发信号
  */
void TIM_SelectSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode);

/**
  * @brief  主从模式使能控制
  * @param  TIMx 定时器外设指针
  * @param  TIM_MasterSlaveMode 模式选择：
  *         TIM_MasterSlaveMode_Enable/Disable
  * @note   使能后定时器可作为主设备触发其他定时器
  */
void TIM_SelectMasterSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_MasterSlaveMode);

/**
  * @brief  外部触发配置（非时钟模式）
  * @param  TIMx 定时器外设指针
  * @param  参数与ETRClockMode配置相同
  * @note   用于普通外部触发场景，非时钟模式应用
  */
void TIM_ETRConfig(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
                  uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);


/* 专用接口配置 *************************************************************/
/**
  * @brief  配置编码器接口模式
  * @param  TIMx 支持编码器的定时器指针（如TIM2-TIM5）
  * @param  TIM_EncoderMode 编码模式：
  *         TIM_EncoderMode_TI1/TI2/TI1_TI2
  * @param  TIM_IC1Polarity 通道1输入极性
  * @param  TIM_IC2Polarity 通道2输入极性
  * @note   配置正交编码器接口，支持X2/X4计数模式
  */
void TIM_EncoderInterfaceConfig(TIM_TypeDef* TIMx, uint16_t TIM_EncoderMode,
                                uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity);

/**
  * @brief  使能霍尔传感器接口
  * @param  TIMx 定时器外设指针
  * @param  NewState 使能状态（ENABLE/DISABLE）
  * @note   需配合霍尔传感器三路输入使用，自动触发捕获事件
  */
void TIM_SelectHallSensor(TIM_TypeDef* TIMx, FunctionalState NewState);


/* 重映射配置 ***************************************************************/
/**
  * @brief  配置定时器引脚重映射
  * @param  TIMx 定时器外设指针
  * @param  TIM_Remap 重映射选项：
  *         TIM_Remap_LPTIM/TIM2/TIM3等具体映射配置
  * @note   需参考具体型号的AFIO重映射表，改变外设引脚分配
  */
void TIM_RemapConfig(TIM_TypeDef* TIMx, uint16_t TIM_Remap);

#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_TIM_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

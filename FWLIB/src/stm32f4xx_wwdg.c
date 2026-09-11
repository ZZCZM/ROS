/**
  ******************************************************************************
  * @file    stm32f4xx_wwdg.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Window watchdog (WWDG) peripheral:           
  *           + Prescaler, Refresh window and Counter configuration
  *           + WWDG activation
  *           + Interrupts and flags management
  *             
 @verbatim    
 ===============================================================================
                           ##### WWDG features #####
 ===============================================================================
    [..]                                      
        Once enabled the WWDG generates a system reset on expiry of a programmed
        time period, unless the program refreshes the counter (downcounter) 
        before to reach 0x3F value (i.e. a reset is generated when the counter
        value rolls over from 0x40 to 0x3F). 
        An MCU reset is also generated if the counter value is refreshed
        before the counter has reached the refresh window value. This 
        implies that the counter must be refreshed in a limited window.
              
        Once enabled the WWDG cannot be disabled except by a system reset.
          
        WWDGRST flag in RCC_CSR register can be used to inform when a WWDG
        reset occurs.
             
        The WWDG counter input clock is derived from the APB clock divided 
        by a programmable prescaler.
                
        WWDG counter clock = PCLK1 / Prescaler
        WWDG timeout = (WWDG counter clock) * (counter value)
                       
        Min-max timeout value @42 MHz(PCLK1): ~97.5 us / ~49.9 ms
                             
                      ##### How to use this driver #####
 ===============================================================================
    [..]
      (#) Enable WWDG clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE) function
              
      (#) Configure the WWDG prescaler using WWDG_SetPrescaler() function
                             
      (#) Configure the WWDG refresh window using WWDG_SetWindowValue() function
              
      (#) Set the WWDG counter value and start it using WWDG_Enable() function.
          When the WWDG is enabled the counter value should be configured to 
          a value greater than 0x40 to prevent generating an immediate reset.     
              
      (#) Optionally you can enable the Early wakeup interrupt which is 
          generated when the counter reach 0x40.
          Once enabled this interrupt cannot be disabled except by a system reset.
                  
      (#) Then the application program must refresh the WWDG counter at regular
          intervals during normal operation to prevent an MCU reset, using
          WWDG_SetCounter() function. This operation must occur only when
          the counter value is lower than the refresh window value, 
          programmed using WWDG_SetWindowValue().         
  
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
#include "stm32f4xx_wwdg.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup WWDG 
  * @brief WWDG driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
  * @brief  WWDG寄存器位地址和位掩码定义
  * @note   该部分定义窗口看门狗(WWDG)相关的寄存器位操作地址和配置掩码
  *         包含位带别名区地址计算和寄存器位掩码宏定义
  *         用于对WWDG控制寄存器(CFR)的位操作和窗口值配置
  */

/* ----------- WWDG registers bit address in the alias region ----------- */
/* 计算WWDG相对于外设基址的偏移量（WWDG属于APB1外设） */
#define WWDG_OFFSET       (WWDG_BASE - PERIPH_BASE)

/* 控制寄存器(CFR)的偏移地址（相对于WWDG基址） */
#define CFR_OFFSET        (WWDG_OFFSET + 0x04)

/* 提前唤醒中断(EWI)在CFR寄存器中的位位置（第9位） */
#define EWI_BitNumber     0x09

/* 计算EWI位在位带别名区的地址（用于位级原子操作） 
   PERIPH_BB_BASE: 外设位带别名区基地址
   32: 每个寄存器位对应别名区的4字节（32位系统）
   4:  每个位在别名区占4字节 */
#define CFR_EWI_BB        (PERIPH_BB_BASE + (CFR_OFFSET * 32) + (EWI_BitNumber * 4))


/* --------------------- WWDG registers bit mask ------------------------ */
/* CFR寄存器位掩码 */
/* 用于清除定时器基值位(WDGTB[2:0])的掩码（保留第7位EWI） 
   0xFFFFFE7F = 1111 1111 1111 1111 1111 1110 0111 1111 */
#define CFR_WDGTB_MASK    ((uint32_t)0xFFFFFE7F)

/* 窗口值配置掩码（保留高7位窗口值，清除低7位计数器值） 
   0xFFFFFF80 = 1111 1111 1111 1111 1111 1111 1000 0000 */
#define CFR_W_MASK        ((uint32_t)0xFFFFFF80)

/* 7位计数器值掩码（最大值为0x7F） 
   用于确保计数器值不超过0x7F（WWDG要求） */
#define BIT_MASK          ((uint8_t)0x7F)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup WWDG_Private_Functions
  * @{
  */

/** @defgroup WWDG_Group1 Prescaler, Refresh window and Counter configuration functions
 *  @brief   Prescaler, Refresh window and Counter configuration functions 
 *
@verbatim   
 ===============================================================================
    ##### Prescaler, Refresh window and Counter configuration functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  窗口看门狗(WWDG)外设复位函数
  * @note   通过APB1总线复位控制器对WWDG进行硬件复位
  * @param  无
  * @retval 无
  * @warning 该操作会重置所有WWDG寄存器到默认值，包括配置和计数器
  */
void WWDG_DeInit(void)
{
  /* 使能WWDG外设复位（置位复位标志）*/
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, ENABLE);
  /* 禁用WWDG外设复位（清除复位标志，完成复位操作）*/
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, DISABLE);
}

/**
  * @brief  设置窗口看门狗时钟预分频器
  * @param  WWDG_Prescaler: 预分频系数选择
  *         @arg WWDG_Prescaler_1: 时钟 = (PCLK1/4096)/1 
  *         @arg WWDG_Prescaler_2: 时钟 = (PCLK1/4096)/2
  *         @arg WWDG_Prescaler_4: 时钟 = (PCLK1/4096)/4
  *         @arg WWDG_Prescaler_8: 时钟 = (PCLK1/4096)/8
  * @retval 无
  * @note   预分频值直接影响看门狗计数器递减速度，分频越大超时时间越长
  */
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler)
{
  uint32_t tmpreg = 0;
  /* 参数合法性检查（确保输入值在预定义的范围内）*/
  assert_param(IS_WWDG_PRESCALER(WWDG_Prescaler));
  
  /* 保留CFR寄存器其他位配置，清除WDGTB[1:0]位（定时器基值位） 
   * 使用CFR_WDGTB_MASK(0xFFFFFE7F)进行位清除操作 */
  tmpreg = WWDG->CFR & CFR_WDGTB_MASK;
  
  /* 将新的预分频值写入WDGTB[1:0]位 
   * 注意：WWDG_Prescaler参数必须已经对齐到寄存器位位置 */
  tmpreg |= WWDG_Prescaler;
  
  /* 更新配置寄存器（实际写入硬件寄存器）*/
  WWDG->CFR = tmpreg;
}

/**
  * @brief  设置窗口看门狗的窗口值
  * @param  WindowValue: 窗口比较值（0x00-0x7F）
  * @retval 无
  * @note   窗口值决定允许喂狗的有效时间范围，必须满足：
  *         - 计数器初始值 > 窗口值 > 0x3F（根据WWDG工作特性）
  *         - 窗口值必须小于0x80（强制7位有效）
  */
void WWDG_SetWindowValue(uint8_t WindowValue)
{
  __IO uint32_t tmpreg = 0;  /* '__IO'表示易变变量，防止编译器优化 */

  /* 验证窗口值有效性（必须 < 0x80）*/
  assert_param(IS_WWDG_WINDOW_VALUE(WindowValue));
  
  /* 保留CFR寄存器其他位配置，清除W[6:0]位（窗口值位）
   * 使用CFR_W_MASK(0xFFFFFF80)进行位清除操作 */
  tmpreg = WWDG->CFR & CFR_W_MASK;

  /* 将新窗口值写入W[6:0]位，并用BIT_MASK(0x7F)确保值有效性
   * 强制转换为uint32_t保证位操作正确性 */
  tmpreg |= WindowValue & (uint32_t) BIT_MASK;

  /* 更新配置寄存器（立即生效）*/
  WWDG->CFR = tmpreg;
}

/**
  * @brief  使能窗口看门狗早期唤醒中断(EWI)
  * @note   该中断一旦使能只能通过系统复位关闭
  * @param  无
  * @retval 无
  * @warning 必须在计数器到达0x40前处理中断并喂狗，否则将触发复位
  */
void WWDG_EnableIT(void)
{
  /* 通过位带操作直接设置EWI中断使能位 
   * CFR_EWI_BB: 预先计算好的位带地址
   * 等效操作：WWDG->CFR |= WWDG_CFR_EWI */
  *(__IO uint32_t *) CFR_EWI_BB = (uint32_t)ENABLE;
}

/**
  * @brief  设置窗口看门狗计数器初始值
  * @param  Counter: 计数器初始值 (0x40-0x7F)
  * @retval 无
  * @note   该值决定：
  *         - 初始递减计数器值
  *         - 实际有效位为bit6-bit0（BIT_MASK过滤）
  * @warning 设置值小于0x40会立即触发复位
  */
void WWDG_SetCounter(uint8_t Counter)
{
  /* 验证参数是否在安全范围（0x40 ≤ Counter ≤ 0x7F）*/
  assert_param(IS_WWDG_COUNTER(Counter));
  
  /* 直接写入控制寄存器(CR)，操作特性：
   * 1. WDGA位（使能位）不受影响
   * 2. 写入后计数器立即开始递减 
   * 3. 使用BIT_MASK(0x7F)确保只写入低7位 */
  WWDG->CR = Counter & BIT_MASK;
}

/**
  * @brief  使能窗口看门狗并加载计数器
  * @param  Counter: 计数器初始值 (0x40-0x7F)
  * @retval 无
  * @note   该操作将同时：
  *         - 设置WDGA位使能看门狗
  *         - 初始化计数器值
  *         - 启动递减计数
  */
void WWDG_Enable(uint8_t Counter)
{
  /* 参数安全检查（与SetCounter使用相同的校验）*/
  assert_param(IS_WWDG_COUNTER(Counter));
  
  /* 组合WDGA使能位和计数器值写入控制寄存器
   * WWDG_CR_WDGA: 看门狗激活位（bit7）
   * 写入后立即生效，看门狗开始工作 */
  WWDG->CR = WWDG_CR_WDGA | Counter;
}

/**
  * @brief  获取早期唤醒中断(EWI)标志状态
  * @param  无
  * @retval 中断标志状态：SET(触发)/RESET(未触发)
  * @note   标志位特性：
  *         - 当计数器达到0x40时自动置位
  *         - 只能通过系统复位或喂狗操作清除
  *         - 标志置位后会持续产生中断请求
  */
FlagStatus WWDG_GetFlagStatus(void)
{
  FlagStatus bitstatus = RESET;
    
  /* 读取状态寄存器(SR)的值：
   * SR寄存器只有bit0有效（EWI标志位）
   * 非零值表示中断已触发 */
  if ((WWDG->SR) != (uint32_t)RESET)
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
  * @brief  清除窗口看门狗早期唤醒中断(EWI)标志位
  * @note   该操作将复位状态寄存器(SR)的EWI标志位
  * @param  无
  * @retval 无
  * @warning 必须在中断服务例程中调用该函数以避免标志位持续触发中断
  * @note 硬件特性：状态寄存器SR为只写寄存器，实际表现为：
  *       - 写入0: 清除EWI标志位
  *       - 写入1: 无效操作（不改变寄存器状态）
  *       该设计可防止意外修改寄存器状态
  */
void WWDG_ClearFlag(void)
{
  /* 通过向SR寄存器写入复位值(0x00000000)清除中断标志
   * 实际效果：
   * 1. 清除SR寄存器的bit0（EWI标志位）
   * 2. 终止正在进行的EWI中断请求
   * 操作特性：
   * - 立即生效，无需等待总线周期完成
   * - 不需要读-修改-写操作模式 */
  WWDG->SR = (uint32_t)RESET;
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

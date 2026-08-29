/**
  ******************************************************************************
  * @file    stm32f4xx_exti.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the EXTI peripheral:           
  *           + Initialization and Configuration
  *           + Interrupts and flags management
  *
@verbatim  

 ===============================================================================
                              ##### EXTI features #####
 ===============================================================================

 [..] External interrupt/event lines are mapped as following:
   (#) All available GPIO pins are connected to the 16 external 
       interrupt/event lines from EXTI0 to EXTI15.
   (#) EXTI line 16 is connected to the PVD Output
   (#) EXTI line 17 is connected to the RTC Alarm event
   (#) EXTI line 18 is connected to the USB OTG FS Wakeup from suspend event
   (#) EXTI line 19 is connected to the Ethernet Wakeup event
   (#) EXTI line 20 is connected to the USB OTG HS (configured in FS) Wakeup event 
   (#) EXTI line 21 is connected to the RTC Tamper and Time Stamp events
   (#) EXTI line 22 is connected to the RTC Wakeup event

                       ##### How to use this driver #####
 ===============================================================================
 
 [..] In order to use an I/O pin as an external interrupt source, follow steps 
      below:
   (#) Configure the I/O in input mode using GPIO_Init()
   (#) Select the input source pin for the EXTI line using SYSCFG_EXTILineConfig()
   (#) Select the mode(interrupt, event) and configure the trigger 
       selection (Rising, falling or both) using EXTI_Init()
   (#) Configure NVIC IRQ channel mapped to the EXTI line using NVIC_Init()

 [..]     
   (@) SYSCFG APB clock must be enabled to get write access to SYSCFG_EXTICRx
       registers using RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

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
#include "stm32f4xx_exti.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup EXTI 
  * @brief EXTI driver modules
  * @{
  */

/**
  * @defgroup EXTI_Private_Definitions 私有定义部分
  * @brief EXTI驱动模块内部使用的私有类型和宏定义
  * @note 这些定义仅在当前驱动文件内部使用，不对外部暴露
  */

/**
  * @def EXTI_LINENONE
  * @brief 中断线路空选择标识
  * @details 当需要取消EXTI线路选择时使用的特殊值，具有以下特性：
  * - 对应32位无符号整型值0x00000000
  * - 二进制表示为全0值，用于清除线路选择位
  * - 在寄存器操作中表示不选择任何中断线路
  * @usage 在配置EXTI线路时用于初始化状态或重置操作，例如：
  * @code
  * EXTI->IMR = EXTI_LINENONE; // 禁用所有中断线路
  * @endcode
  */
#define EXTI_LINENONE    ((uint32_t)0x00000)  /* No interrupt selected */

/* 私有宏操作区域 ------------------------------------------------------------
   此处定义模块内部使用的复杂功能宏，可能包含：
   - 寄存器位操作宏
   - 硬件特性检测宏
   - 内部校验宏
----------------------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* 模块级全局变量区域 --------------------------------------------------------
   此处定义模块内部使用的静态全局变量，包含：
   - 硬件寄存器实例指针
   - 中断回调函数指针数组
   - 运行时状态标志
   注：这些变量使用static修饰，作用域限制在本文件内
----------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/



/**
  * @brief  EXTI外设反初始化函数
  * @功能   将EXTI所有配置寄存器恢复为复位默认值
  * @作用   清除所有中断/事件配置，禁用所有线路的触发，清除挂起标志
  * @细节   通过写寄存器实现以下操作：
  *         - 禁用中断屏蔽（IMR）
  *         - 禁用事件屏蔽（EMR）
  *         - 关闭上升沿触发（RTSR）
  *         - 关闭下降沿触发（FTSR）
  *         - 清除所有挂起标志位（PR）
  * @参数   None
  * @返回值 None
  */
void EXTI_DeInit(void)
{
  /* 关闭所有线路的中断请求（IMR: Interrupt Mask Register）
     0x00000000表示所有位清零，禁用全部22条EXTI线的中断 */
  EXTI->IMR = 0x00000000;
  
  /* 关闭所有线路的事件请求（EMR: Event Mask Register）
     0x00000000表示所有位清零，禁用全部22条EXTI线的事件触发 */
  EXTI->EMR = 0x00000000;
  
  /* 清除所有上升沿触发配置（RTSR: Rising Trigger Selection Register）
     0x00000000表示关闭所有EXTI线的上升沿检测 */
  EXTI->RTSR = 0x00000000;
  
  /* 清除所有下降沿触发配置（FTSR: Falling Trigger Selection Register）
     0x00000000表示关闭所有EXTI线的下降沿检测 */
  EXTI->FTSR = 0x00000000;
  
  /* 清除所有挂起标志（PR: Pending Register）
     写入0x007FFFFF（二进制23个1）表示清除EXTI0~EXTI22共23条线的中断挂起状态
     通过写1清除对应位的挂起标志，这是STM32的特殊设计 */
  EXTI->PR = 0x007FFFFF;
}
/**
  * @brief  EXTI外设初始化函数
  * @功能   根据配置结构体参数初始化外部中断/事件控制器
  * @操作   配置EXTI线路的工作模式、触发方式和使能状态
  * @流程   参数检查 → 寄存器原子操作 → 触发模式配置
  * @param  EXTI_InitStruct 指向EXTI配置结构体的指针，包含：
  *         - EXTI_Line:   要配置的EXTI线路位掩码（EXTI_Line0~Line22）
  *         - EXTI_Mode:   工作模式（中断/事件）
  *         - EXTI_Trigger:触发方式（上升沿/下降沿/双边沿）
  *         - EXTI_LineCmd:使能状态（ENABLE/DISABLE）
  * @retval None
  */
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct)
{
  uint32_t tmp = 0;

  /* 参数合法性验证（使用标准外设库的断言机制） */
  assert_param(IS_EXTI_MODE(EXTI_InitStruct->EXTI_Mode));       // 验证模式参数
  assert_param(IS_EXTI_TRIGGER(EXTI_InitStruct->EXTI_Trigger)); // 验证触发方式
  assert_param(IS_EXTI_LINE(EXTI_InitStruct->EXTI_Line));       // 验证线路范围
  assert_param(IS_FUNCTIONAL_STATE(EXTI_InitStruct->EXTI_LineCmd)); // 验证使能状态

  tmp = (uint32_t)EXTI_BASE;  // 获取EXTI寄存器基地址（0x40013C00）

  if (EXTI_InitStruct->EXTI_LineCmd != DISABLE)  // 启用线路配置模式
  {
    /* 清除目标线路的原有配置（原子操作保证线程安全） */
    EXTI->IMR &= ~EXTI_InitStruct->EXTI_Line; // 中断屏蔽寄存器清除目标位
    EXTI->EMR &= ~EXTI_InitStruct->EXTI_Line; // 事件屏蔽寄存器清除目标位
    
    tmp += EXTI_InitStruct->EXTI_Mode;  // 计算模式寄存器偏移量：
                                        // EXTI_Mode_Interrupt → IMR(0x00)
                                        // EXTI_Mode_Event → EMR(0x04)

    /* 设置目标线路到指定模式寄存器 */
    *(__IO uint32_t *) tmp |= EXTI_InitStruct->EXTI_Line; // 通过指针运算访问IMR/EMR

    /* 清除原有的触发边沿配置 */
    EXTI->RTSR &= ~EXTI_InitStruct->EXTI_Line; // 清除上升沿触发配置
    EXTI->FTSR &= ~EXTI_InitStruct->EXTI_Line; // 清除下降沿触发配置
    
    /* 配置触发边沿检测 */
    if (EXTI_InitStruct->EXTI_Trigger == EXTI_Trigger_Rising_Falling) // 双边沿触发
    {
      EXTI->RTSR |= EXTI_InitStruct->EXTI_Line;  // 使能上升沿检测
      EXTI->FTSR |= EXTI_InitStruct->EXTI_Line;  // 使能下降沿检测
    }
    else  // 单边沿触发模式
    {
      tmp = (uint32_t)EXTI_BASE;  // 重置基地址
      tmp += EXTI_InitStruct->EXTI_Trigger; // 计算触发寄存器偏移量：
                                            // EXTI_Trigger_Rising → RTSR(0x08)
                                            // EXTI_Trigger_Falling → FTSR(0x0C)
      
      *(__IO uint32_t *) tmp |= EXTI_InitStruct->EXTI_Line; // 设置单边沿触发
    }
  }
  else  // 禁用线路配置模式
  {
    tmp += EXTI_InitStruct->EXTI_Mode;  // 计算模式寄存器偏移量

    /* 在对应模式寄存器中清除目标线路 */
    *(__IO uint32_t *) tmp &= ~EXTI_InitStruct->EXTI_Line; // 禁用中断/事件
  }
}

/**
  * @brief  EXTI配置结构体初始化函数
  * @功能   将EXTI初始化结构体恢复为安全默认值
  * @作用   防止未初始化结构体导致的配置错误
  * @细节   默认配置：
  *         - 不选择任何EXTI线路
  *         - 中断模式
  *         - 下降沿触发
  *         - 禁用状态
  * @param  EXTI_InitStruct 要初始化的EXTI配置结构体指针
  * @retval None
  */
void EXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct)
{
  EXTI_InitStruct->EXTI_Line = EXTI_LINENONE;        // 线路掩码清零（0x00000000）
  EXTI_InitStruct->EXTI_Mode = EXTI_Mode_Interrupt;  // 默认使用中断模式
  EXTI_InitStruct->EXTI_Trigger = EXTI_Trigger_Falling; // 默认下降沿触发
  EXTI_InitStruct->EXTI_LineCmd = DISABLE;           // 默认关闭EXTI线路
}

/**
  * @brief  软件触发外部中断函数
  * @功能   通过写寄存器手动生成指定EXTI线路的中断请求
  * @特性   - 立即触发中断（无需实际边沿信号）
  *         - 适用于软件模拟中断场景
  *         - 触发后SWIER位自动清零
  * @注意   需配合NVIC中断使能使用
  * @param  EXTI_Line 要触发的中断线路位掩码（EXTI_Line0~Line22）
  * @retval None
  */
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line)
{
  /* 验证线路参数有效性 */
  assert_param(IS_EXTI_LINE(EXTI_Line));  
  
  /* 写SWIER寄存器触发软件中断（写1有效，自动清零）
     该操作将立即产生中断请求，效果等同于硬件触发 */
  EXTI->SWIER |= EXTI_Line;  // 设置软件中断事件寄存器
}

/**
  * @}
  */

/** @defgroup EXTI_Group2 Interrupts and flags management functions
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
  * @brief  EXTI标志位状态查询函数
  * @功能   检测指定EXTI线路的挂起标志位状态
  * @特性   - 非中断安全查询
  *         - 仅反映PR寄存器当前状态
  *         - 不改变寄存器实际值
  * @param  EXTI_Line 要检测的EXTI线路（单线模式，如EXTI_Line5）
  * @retval SET(1)-标志位挂起 / RESET(0)-无挂起
  */
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line)
{
  FlagStatus bitstatus = RESET;
  /* 验证线路参数合法性（单线模式检查） */
  assert_param(IS_GET_EXTI_LINE(EXTI_Line));  // 确保参数为单线路掩码
  
  /* 位与操作检测挂起状态（PR寄存器特性：挂起位自动置1） */
  if ((EXTI->PR & EXTI_Line) != (uint32_t)RESET)
  {
    bitstatus = SET;  // 检测到目标线路有挂起请求
  }
  else
  {
    bitstatus = RESET; // 目标线路无挂起状态
  }
  return bitstatus;
}

/**
  * @brief  EXTI标志位清除函数
  * @功能   清除指定EXTI线路的挂起标志
  * @原理   通过写1清除机制（STM32特有设计）
  * @注意   - 可同时清除多个线路标志
  *         - 写0无效，必须写1清除
  * @param  EXTI_Line 要清除的线路掩码（支持多线组合，如EXTI_Line3|EXTI_Line5）
  * @retval None
  */
void EXTI_ClearFlag(uint32_t EXTI_Line)
{
  /* 验证线路参数范围（允许组合线路） */
  assert_param(IS_EXTI_LINE(EXTI_Line));
  
  /* 原子操作清除挂起标志（PR寄存器特性：写1清零对应位） */
  EXTI->PR = EXTI_Line;  // 同时清除多个线路标志位
}

/**
  * @brief  EXTI中断状态查询函数
  * @功能   检测指定EXTI线路的中断挂起状态
  * @区别   与GetFlagStatus的区别：
  *         - 应在中断服务程序中使用
  *         - 查询前自动清除挂起标志（需配合NVIC使用）
  * @param  EXTI_Line 要检测的EXTI线路（单线模式）
  * @retval SET-中断挂起 / RESET-无中断请求
  */
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line)
{
  FlagStatus bitstatus = RESET;
  /* 验证单线路参数合法性 */
  assert_param(IS_GET_EXTI_LINE(EXTI_Line));
  
  /* 中断状态检测（需配合NVIC中断使能） */
  if ((EXTI->PR & EXTI_Line) != (uint32_t)RESET)
  {
    bitstatus = SET;  // 检测到有效中断请求
  }
  else
  {
    bitstatus = RESET; // 无有效中断请求
  }
  return bitstatus;
}

/**
  * @brief  EXTI中断挂起位清除函数
  * @功能   清除指定EXTI线路的中断挂起标志
  * @注意   - 必须在中断服务程序中调用
  *         - 清除操作应在中断处理完成后执行
  * @param  EXTI_Line 要清除的中断线路（支持多线组合）
  * @retval None
  */
void EXTI_ClearITPendingBit(uint32_t EXTI_Line)
{
  /* 验证线路参数有效性 */
  assert_param(IS_EXTI_LINE(EXTI_Line));
  
  /* 写PR寄存器清除中断标志（防止重复进入中断） */
  EXTI->PR = EXTI_Line;  // 同时清除多个中断标志
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

/**
  ******************************************************************************
  * @file    stm32f4xx_rng.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief This file provides firmware functions to manage the following 
  *          functionalities of the Random Number Generator (RNG) peripheral:           
  *           + Initialization and Configuration 
  *           + Get 32 bit Random number      
  *           + Interrupts and flags management       
  *         
@verbatim
                                 
 ===================================================================      
                 ##### How to use this driver #####
 ===================================================================          
 [..]
   (#) Enable The RNG controller clock using 
       RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE) function.
                
   (#) Activate the RNG peripheral using RNG_Cmd() function.
            
   (#) Wait until the 32 bit Random number Generator contains a valid  random data
      (using polling/interrupt mode). For more details, refer to "Interrupts and 
      flags management functions" module description.
             
   (#) Get the 32 bit Random number using RNG_GetRandomNumber() function
            
   (#) To get another 32 bit Random number, go to step 3.       
         
                
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
#include "stm32f4xx_rng.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup RNG 
  * @brief RNG driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup RNG_Private_Functions
  * @{
  */ 

/** @defgroup RNG_Group1 Initialization and Configuration functions
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
             ##### Initialization and Configuration functions #####
 ===============================================================================  
 [..] This section provides functions allowing to 
   (+) Initialize the RNG peripheral
   (+) Enable or disable the RNG peripheral
   
@endverbatim
  * @{
  */
/**
  * @brief  复位RNG外设寄存器到默认值
  * @param  无
  * @note 通过AHB2总线复位控制器实现硬件复位
  *        - 先使能复位（保持复位状态）
  *        - 再解除复位（完成复位操作）
  *        - 复位期间所有寄存器值将被清除
  */
void RNG_DeInit(void)
{
  /* 通过RCC模块使能RNG硬件复位 */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_RNG, ENABLE);  // 启动复位信号，RNG进入复位状态

  /* 通过RCC模块禁用RNG硬件复位 */
  RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_RNG, DISABLE); // 关闭复位信号，RNG开始正常工作
}

/**
  * @brief  启用/禁用随机数生成器
  * @param  NewState: 外设状态（ENABLE/DISABLE）
  * @note 操作控制寄存器(CR)的RNGEN位：
  *        - 使能后需要等待SEIS/CECS标志稳定
  *        - 禁用时立即停止随机数生成
  *        操作前需确保已配置RNG时钟
  */
void RNG_Cmd(FunctionalState NewState)
{
  /* 参数有效性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 校验状态参数合法性

  if (NewState != DISABLE)  // 启用RNG
  {
    /* 设置控制寄存器的RNGEN位（bit2）*/
    RNG->CR |= RNG_CR_RNGEN;  // 使能后开始模拟电路供电，启动熵收集过程
  }
  else  // 禁用RNG
  {
    /* 清除控制寄存器的RNGEN位（bit2）*/
    RNG->CR &= ~RNG_CR_RNGEN; // 立即断开模拟电路供电，停止随机数生成
  }
}

/**
  * @}
  */

/** @defgroup RNG_Group2 Get 32 bit Random number function
 *  @brief    Get 32 bit Random number function 
 *

@verbatim    
 ===============================================================================
                 ##### Get 32 bit Random number function #####
 ===============================================================================  
 [..] This section provides a function allowing to get the 32 bit Random number  
  
   (@)  Before to call this function you have to wait till DRDY flag is set,
        using RNG_GetFlagStatus(RNG_FLAG_DRDY) function. 
   
@endverbatim
  * @{
  */


/**
  * @brief  Returns a 32-bit random number.
  *   
  * @note   Before to call this function you have to wait till DRDY (data ready)
  *         flag is set, using RNG_GetFlagStatus(RNG_FLAG_DRDY) function.
  * @note   Each time the the Random number data is read (using RNG_GetRandomNumber()
  *         function), the RNG_FLAG_DRDY flag is automatically cleared.
  * @note   In the case of a seed error, the generation of random numbers is 
  *         interrupted for as long as the SECS bit is '1'. If a number is 
  *         available in the RNG_DR register, it must not be used because it may 
  *         not have enough entropy. In this case, it is recommended to clear the 
  *         SEIS bit(using RNG_ClearFlag(RNG_FLAG_SECS) function), then disable 
  *         and enable the RNG peripheral (using RNG_Cmd() function) to 
  *         reinitialize and restart the RNG.
  * @note   In the case of a clock error, the RNG is no more able to generate 
  *         random numbers because the PLL48CLK clock is not correct. User have 
  *         to check that the clock controller is correctly configured to provide
  *         the RNG clock and clear the CEIS bit (using RNG_ClearFlag(RNG_FLAG_CECS) 
  *         function) . The clock error has no impact on the previously generated 
  *         random numbers, and the RNG_DR register contents can be used.
  *         
  * @param  None
  * @retval 32-bit random number.
  */
/**
  * @brief  获取32位真随机数
  * @param  无
  * @retval 32位随机数值（直接来自数据寄存器DR）
  * @note 使用前提：
  *        - RNG外设已通过RNG_Cmd()使能
  *        - 需通过RNG_GetFlagStatus()确认DRDY标志置位
  *        - 读取后DRDY标志自动清除
  *        若未满足条件可能返回无效数据
  */
uint32_t RNG_GetRandomNumber(void)
{
  /* 直接读取数据寄存器(DR)的32位内容 */
  return RNG->DR;  // DR寄存器地址偏移：0x08（相对于RNG基地址）
}


/**
  * @}
  */

/** @defgroup RNG_Group3 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
             ##### Interrupts and flags management functions #####
 ===============================================================================  

 [..] This section provides functions allowing to configure the RNG Interrupts and 
      to get the status and clear flags and Interrupts pending bits.
  
 [..] The RNG provides 3 Interrupts sources and 3 Flags:
  
 *** Flags : ***
 ===============
 [..] 
    (#) RNG_FLAG_DRDY :  In the case of the RNG_DR register contains valid 
        random data. it is cleared by reading the valid data(using 
        RNG_GetRandomNumber() function).

    (#) RNG_FLAG_CECS : In the case of a seed error detection. 
      
    (#) RNG_FLAG_SECS : In the case of a clock error detection.
              
 *** Interrupts ***
 ==================
 [..] If enabled, an RNG interrupt is pending :
    
   (#) In the case of the RNG_DR register contains valid random data. 
       This interrupt source is cleared once the RNG_DR register has been read 
       (using RNG_GetRandomNumber() function) until a new valid value is 
       computed; or 
   (#) In the case of a seed error : One of the following faulty sequences has 
       been detected:
       (++) More than 64 consecutive bits at the same value (0 or 1)
       (++) More than 32 consecutive alternance of 0 and 1 (0101010101...01)
       This interrupt source is cleared using RNG_ClearITPendingBit(RNG_IT_SEI)
       function; or
   (#) In the case of a clock error : the PLL48CLK (RNG peripheral clock source) 
       was not correctly detected (fPLL48CLK< fHCLK/16). This interrupt source is
       cleared using RNG_ClearITPendingBit(RNG_IT_CEI) function.
       -@- note In this case, User have to check that the clock controller is 
           correctly configured to provide the RNG clock. 

 *** Managing the RNG controller events : ***
 ============================================
 [..] The user should identify which mode will be used in his application to manage 
      the RNG controller events: Polling mode or Interrupt mode.
  
   (#) In the Polling Mode it is advised to use the following functions:
       (++) RNG_GetFlagStatus() : to check if flags events occur. 
       (++) RNG_ClearFlag()     : to clear the flags events.
  
       -@@- RNG_FLAG_DRDY can not be cleared by RNG_ClearFlag(). it is cleared only 
            by reading the Random number data.      
  
   (#)  In the Interrupt Mode it is advised to use the following functions:
        (++) RNG_ITConfig()       : to enable or disable the interrupt source.
        (++) RNG_GetITStatus()    : to check if Interrupt occurs.
        (++) RNG_ClearITPendingBit() : to clear the Interrupt pending Bit 
             (corresponding Flag). 
  
@endverbatim
  * @{
  */ 
/**
  * @brief  启用/禁用RNG全局中断
  * @param  NewState: 中断总使能状态（ENABLE/DISABLE）
  * @note 控制CR寄存器的IE位（bit3）：
  *        - 使能后DRDY/CEI/SEI中断均可触发
  *        - 实际中断使能需配合具体标志位状态
  *        该设置影响所有RNG中断源
  */
void RNG_ITConfig(FunctionalState NewState)
{
  /* 参数有效性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 校验状态参数合法性

  if (NewState != DISABLE)  // 启用全局中断
  {
    /* 设置控制寄存器的IE位（bit3）*/
    RNG->CR |= RNG_CR_IE;  // 使能所有中断源的总开关
  }
  else  // 禁用全局中断
  {
    /* 清除控制寄存器的IE位（bit3）*/
    RNG->CR &= ~RNG_CR_IE; // 关闭所有中断触发能力
  }
}

/**
  * @brief  获取RNG状态标志位
  * @param  RNG_FLAG: 要查询的标志位，支持：
  *         @arg RNG_FLAG_DRDY 数据就绪标志（bit0）
  *         @arg RNG_FLAG_CECS 当前时钟错误标志（bit1）
  *         @arg RNG_FLAG_SECS 当前种子错误标志（bit2）
  * @retval SET(置位)/RESET(复位) 标志状态
  * @note DRDY标志在读取DR寄存器后自动清除
  */
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* 参数有效性验证 */
  assert_param(IS_RNG_GET_FLAG(RNG_FLAG));  // 验证标志类型合法性
  
  /* 通过状态寄存器(SR)检测标志位 */
  if ((RNG->SR & RNG_FLAG) != (uint8_t)RESET)  // 位与操作检测指定标志
  {
    bitstatus = SET;    // 检测到标志置位
  }
  else
  {
    bitstatus = RESET;  // 标志未置位
  }
  return bitstatus;      // 返回当前状态
}

/**
  * @brief  清除RNG错误状态标志
  * @param  RNG_FLAG: 要清除的标志位组合，支持：
  *         @arg RNG_FLAG_CECS 时钟错误标志（bit1）
  *         @arg RNG_FLAG_SECS 种子错误标志（bit2）
  * @note DRDY标志不可手动清除
  *        错误标志清除后需重新初始化RNG
  */
void RNG_ClearFlag(uint8_t RNG_FLAG)
{
  /* 参数有效性验证 */
  assert_param(IS_RNG_CLEAR_FLAG(RNG_FLAG));  // 验证可清除标志类型
  
  /* 通过写入状态寄存器(SR)清除标志位（bit5/bit6）*/
  RNG->SR = ~(uint32_t)(((uint32_t)RNG_FLAG) << 4);  // 错误标志位映射到SR的bit5/bit6
}

/**
  * @brief  获取RNG中断触发状态
  * @param  RNG_IT: 中断类型，支持：
  *         @arg RNG_IT_CEI 时钟错误中断（bit5）
  *         @arg RNG_IT_SEI 种子错误中断（bit6）
  * @retval SET(中断挂起)/RESET(无中断)
  * @note 实际中断触发需同时满足：
  *        - CR寄存器的IE位使能（全局中断）
  *        - SR寄存器对应错误标志置位
  */
ITStatus RNG_GetITStatus(uint8_t RNG_IT)
{
  ITStatus bitstatus = RESET;
  /* 参数有效性验证 */
  assert_param(IS_RNG_GET_IT(RNG_IT));  // 验证中断类型合法性
  
  /* 检测状态寄存器(SR)的中断标志位 */
  if ((RNG->SR & RNG_IT) != (uint8_t)RESET)  // 直接检测bit5/bit6
  {
    bitstatus = SET;    // 中断条件满足
  }
  else
  {
    bitstatus = RESET;  // 无中断请求
  }
  return bitstatus;      // 返回中断状态
}

/**
  * @brief  清除RNG中断挂起标志
  * @param  RNG_IT: 要清除的中断标志组合，支持：
  *         @arg RNG_IT_CEI 时钟错误中断（对应SR寄存器的bit5）
  *         @arg RNG_IT_SEI 种子错误中断（对应SR寄存器的bit6）
  * @note 清除机制：
  *        - 写入SR寄存器的对应位取反值实现清除
  *        - 该操作不影响原始错误标志位（CECS/SECS）
  *        清除后需检查错误状态是否持续
  */
void RNG_ClearITPendingBit(uint8_t RNG_IT)
{
  /* 参数有效性验证 */
  assert_param(IS_RNG_IT(RNG_IT));  // 验证是否为CEI/SEI组合

  /* 通过写1清除机制清除中断挂起位：
     - 将中断标志位取反后写入SR寄存器
     - 实际清除的是SR寄存器的bit5(CEI)/bit6(SEI) */
  RNG->SR = (uint8_t)~RNG_IT;  // 例：RNG_IT=0x20(CEI) → 写入0xDF清除bit5
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

/**
  ******************************************************************************
  * @file    stm32f4xx_dbgmcu.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides all the DBGMCU firmware functions.
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
#include "stm32f4xx_dbgmcu.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup DBGMCU 
  * @brief DBGMCU driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFF)
/**
  * @brief  获取芯片修订版本标识符
  * @details 从DBGMCU的IDCODE寄存器中提取芯片修订版本号，位于寄存器高16位
  *         用于识别硅片版本（如A版、B版等），对应芯片勘误表版本
  * @param  无
  * @retval 4位修订编号（实际有效位可能根据芯片型号变化）
  * @example 
  *         返回值0x1001表示Major=1，Minor=1
  * @note 此函数在固件升级版本兼容性检查时常用
  */
uint32_t DBGMCU_GetREVID(void)
{
   /* 访问IDCODE寄存器（地址0xE0042000）并右移16位提取高16位 */
   /* IDCODE寄存器结构：[31:28]芯片版本，[27:16]Part编号，[15:0]Device ID */
   return(DBGMCU->IDCODE >> 16);  // 示例：0x413 -> STM32F405xx/07xx A版本
}

/**
  * @brief  获取设备唯一标识符
  * @details 从IDCODE寄存器中提取12位设备分类编码，用于识别芯片型号大类
  *         与DBGMCU_GetDEVID()配合可精确识别具体型号
  * @param  无
  * @retval 12位设备分类编码（掩码0x00000FFF）
  * @example 
  *         0x413表示STM32F4xx系列中的具体子型号
  * @note 不同型号的FLASH大小、外设配置等可根据此ID区分
  */
uint32_t DBGMCU_GetDEVID(void)
{
   /* 应用预定义掩码提取低12位有效数据 */
   /* 掩码定义：#define IDCODE_DEVID_MASK 0x00000FFF */
   return(DBGMCU->IDCODE & IDCODE_DEVID_MASK);  // 示例：0x463 -> STM32F415xx/17xx
}

/**
  * @brief  配置调试模式下的低功耗行为
  * @details 该函数用于控制在MCU进入低功耗模式时，是否保持与调试器的连接
  *         使能后可实现在SLEEP/STOP/STANDBY模式下维持调试连接
  * @param  DBGMCU_Periph: 低功耗模式配置
  *         可组合以下参数（按位或操作）：
  *           - DBGMCU_SLEEP:  在SLEEP模式下保持调试连接
  *           - DBGMCU_STOP:   在STOP模式下保持调试连接
  *           - DBGMCU_STANDBY:在STANDBY模式下保持调试连接
  * @param  NewState: 配置状态
  *         参数取值：
  *           - ENABLE: 启用调试保持
  *           - DISABLE: 禁用调试保持
  * @note 典型应用场景：
  *        - 调试低功耗应用时维持JTAG/SWD连接
  *        - 生产代码中建议禁用以降低功耗
  * @warning 启用STANDBY调试会显著增加待机功耗
  */
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_DBGMCU_PERIPH(DBGMCU_Periph)); // 验证模式组合合法性（0x00000007）
  assert_param(IS_FUNCTIONAL_STATE(NewState));    // 验证状态参数有效性

  if (NewState != DISABLE)
  {
    /* 设置调试控制寄存器(CR)对应位 */
    // 通过位或操作设置指定模式位，不影响其他控制位
    // 示例：DBGMCU_SLEEP|DBGMCU_STOP = 0x00000003
    DBGMCU->CR |= DBGMCU_Periph;  
  }
  else
  {
    /* 清除调试控制寄存器(CR)对应位 */
    // 通过位与操作和掩码取反清除指定模式位
    DBGMCU->CR &= ~DBGMCU_Periph; 
  }
}


/**
  * @brief  Configures APB1 peripheral behavior when the MCU is in Debug mode.
  * @param  DBGMCU_Periph: specifies the APB1 peripheral.
  *   This parameter can be any combination of the following values:        
  *     @arg DBGMCU_TIM2_STOP: TIM2 counter stopped when Core is halted          
  *     @arg DBGMCU_TIM3_STOP: TIM3 counter stopped when Core is halted          
  *     @arg DBGMCU_TIM4_STOP: TIM4 counter stopped when Core is halted
  *     @arg DBGMCU_TIM5_STOP: TIM5 counter stopped when Core is halted          
  *     @arg DBGMCU_TIM6_STOP: TIM6 counter stopped when Core is halted          
  *     @arg DBGMCU_TIM7_STOP: TIM7 counter stopped when Core is halted
  *     @arg DBGMCU_TIM12_STOP: TIM12 counter stopped when Core is halted  
  *     @arg DBGMCU_TIM13_STOP: TIM13 counter stopped when Core is halted  
  *     @arg DBGMCU_TIM14_STOP: TIM14 counter stopped when Core is halted 
  *     @arg DBGMCU_RTC_STOP: RTC Calendar and Wakeup counter stopped when Core is halted.                                                                                
  *     @arg DBGMCU_WWDG_STOP: Debug WWDG stopped when Core is halted
  *     @arg DBGMCU_IWDG_STOP: Debug IWDG stopped when Core is halted        
  *     @arg DBGMCU_I2C1_SMBUS_TIMEOUT: I2C1 SMBUS timeout mode stopped when Core is halted
  *     @arg DBGMCU_I2C2_SMBUS_TIMEOUT: I2C2 SMBUS timeout mode stopped when Core is halted
  *     @arg DBGMCU_I2C3_SMBUS_TIMEOUT: I2C3 SMBUS timeout mode stopped when Core is halted
  *     @arg DBGMCU_CAN2_STOP: Debug CAN1 stopped when Core is halted           
  *     @arg DBGMCU_CAN1_STOP: Debug CAN2 stopped when Core is halted        
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  配置APB1外设调试模式行为
  * @details 当内核暂停(如遇到断点)时，控制APB1总线上的外设是否继续工作
  *         启用后相关外设在调试时冻结，便于观察状态
  * @param  DBGMCU_Periph: APB1外设选择，可组合以下值（按位或）：
  *           - DBGMCU_TIM2_STOP  : TIM2定时器停止  
  *           - DBGMCU_TIM3_STOP  : TIM3定时器停止
  *           - DBGMCU_TIM4_STOP  : TIM4定时器停止
  *           - DBGMCU_TIM5_STOP  : TIM5定时器停止
  *           - DBGMCU_TIM6_STOP  : TIM6定时器停止
  *           - DBGMCU_TIM7_STOP  : TIM7定时器停止
  *           - DBGMCU_TIM12_STOP : TIM12定时器停止
  *           - DBGMCU_TIM13_STOP : TIM13定时器停止 
  *           - DBGMCU_TIM14_STOP : TIM14定时器停止
  *           - DBGMCU_RTC_STOP   : RTC时钟停止
  *           - DBGMCU_WWDG_STOP  : 窗口看门狗停止
  *           - DBGMCU_IWDG_STOP  : 独立看门狗停止
  *           - DBGMCU_I2C1_SMBUS_TIMEOUT_STOP : I2C1超时定时器停止
  *           - DBGMCU_I2C2_SMBUS_TIMEOUT_STOP : I2C2超时定时器停止
  *           - DBGMCU_I2C3_SMBUS_TIMEOUT_STOP : I2C3超时定时器停止
  * @param  NewState: 冻结使能状态
  *           - ENABLE: 调试时外设冻结
  *           - DISABLE: 保持外设正常运行
  * @example 
  *         调试TIM2和TIM3：DBGMCU_APB1PeriphConfig(DBGMCU_TIM2_STOP|DBGMCU_TIM3_STOP, ENABLE)
  * @note 外设冻结会暂停计数器、通信状态机等，但不影响GPIO电平
  */
void DBGMCU_APB1PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_DBGMCU_APB1PERIPH(DBGMCU_Periph)); // 验证APB1外设掩码合法性(0xFFFFFFFF)
  assert_param(IS_FUNCTIONAL_STATE(NewState));        // 状态参数必须为ENABLE/DISABLE

  if (NewState != DISABLE)
  {
    /* 设置APB1冻结寄存器对应位（置1冻结）*/
    // 通过位或操作设置指定外设冻结位，其他位保持不变
    DBGMCU->APB1FZ |= DBGMCU_Periph;  // 示例：0x00000001设置TIM2冻结
  }
  else
  {
    /* 清除APB1冻结寄存器对应位（置0恢复） */
    // 通过位与操作和掩码取反清除冻结位
    DBGMCU->APB1FZ &= ~DBGMCU_Periph; // 示例：0xFFFFFFFE清除TIM2冻结
  }
}

/**
  * @brief  配置APB2外设调试模式行为
  * @details 功能同APB1版本，针对APB2总线上的外设
  * @param  DBGMCU_Periph: APB2外设选择，可组合以下值：
  *           - DBGMCU_TIM1_STOP  : TIM1定时器停止  
  *           - DBGMCU_TIM8_STOP  : TIM8定时器停止
  *           - DBGMCU_TIM9_STOP  : TIM9定时器停止
  *           - DBGMCU_TIM10_STOP: TIM10定时器停止
  *           - DBGMCU_TIM11_STOP: TIM11定时器停止
  * @param  NewState: 冻结使能状态（同APB1版本）
  * @warning TIM1/TIM8等高级定时器冻结会影响PWM输出状态
  * @example 
  *         调试电机控制PWM：DBGMCU_APB2PeriphConfig(DBGMCU_TIM1_STOP, ENABLE)
  */
void DBGMCU_APB2PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
  /* 参数检查流程与APB1版本相同 */
  assert_param(IS_DBGMCU_APB2PERIPH(DBGMCU_Periph)); // 验证APB2外设掩码(0x001F0000)
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 设置APB2冻结寄存器对应位 */
    DBGMCU->APB2FZ |= DBGMCU_Periph;  // 示例：0x00010000冻结TIM1
  }
  else
  {
    /* 清除APB2冻结寄存器对应位 */
    DBGMCU->APB2FZ &= ~DBGMCU_Periph; // 示例：0xFFFEFFFF解除TIM1冻结
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

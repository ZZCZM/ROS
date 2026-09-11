/**
  ******************************************************************************
  * @file    stm32f4xx_iwdg.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Independent watchdog (IWDG) peripheral:           
  *           + Prescaler and Counter configuration
  *           + IWDG activation
  *           + Flag management
  *
    @verbatim    
 ===============================================================================
                          ##### IWDG features #####
 ===============================================================================
    [..]  
      The IWDG can be started by either software or hardware (configurable
      through option byte).
              
      The IWDG is clocked by its own dedicated low-speed clock (LSI) and
      thus stays active even if the main clock fails.
      Once the IWDG is started, the LSI is forced ON and cannot be disabled
      (LSI cannot be disabled too), and the counter starts counting down from 
      the reset value of 0xFFF. When it reaches the end of count value (0x000)
      a system reset is generated.
      The IWDG counter should be reloaded at regular intervals to prevent
      an MCU reset.
                             
      The IWDG is implemented in the VDD voltage domain that is still functional
      in STOP and STANDBY mode (IWDG reset can wake-up from STANDBY).          
              
      IWDGRST flag in RCC_CSR register can be used to inform when a IWDG
      reset occurs.
              
      Min-max timeout value @32KHz (LSI): ~125us / ~32.7s
      The IWDG timeout may vary due to LSI frequency dispersion. STM32F4xx
      devices provide the capability to measure the LSI frequency (LSI clock
      connected internally to TIM5 CH4 input capture). The measured value
      can be used to have an IWDG timeout with an acceptable accuracy. 
      For more information, please refer to the STM32F4xx Reference manual
            
                     ##### How to use this driver #####
 ===============================================================================
    [..]
      (#) Enable write access to IWDG_PR and IWDG_RLR registers using
          IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable) function
                 
      (#) Configure the IWDG prescaler using IWDG_SetPrescaler() function
              
      (#) Configure the IWDG counter value using IWDG_SetReload() function.
          This value will be loaded in the IWDG counter each time the counter
          is reloaded, then the IWDG will start counting down from this value.
              
      (#) Start the IWDG using IWDG_Enable() function, when the IWDG is used
          in software mode (no need to enable the LSI, it will be enabled
          by hardware)
               
      (#) Then the application program must reload the IWDG counter at regular
          intervals during normal operation to prevent an MCU reset, using
          IWDG_ReloadCounter() function.      
            
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
#include "stm32f4xx_iwdg.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup IWDG 
  * @brief IWDG driver modules
  * @{
  */ 
/** 
 * @file    stm32f4xx_iwdg.c
 * @brief   独立看门狗(IWDG)驱动模块的寄存器键值定义
 * @note    关键寄存器操作说明：
 *          - KR寄存器(键寄存器)是IWDG的核心控制寄存器
 *          - 必须严格按照时序要求写入特定键值才能执行相应操作
 *          - 写入错误值将导致寄存器访问保护
 */

/*---------------------------------- 看门狗控制键值定义 ----------------------------------*/
/* 以下宏定义用于对KR寄存器进行安全写操作，遵循STM32的寄存器访问保护机制 */

/**
 * @brief 看门狗计数器重载键值
 * @details 
 * - 作用：当写入该值到KR寄存器时，重载看门狗计数器(CNT)
 * - 行为：将重载寄存器(RLR)的值装载到计数器，防止看门狗超时复位
 * - 使用场景：必须在看门狗超时前周期性写入（通常放在主循环）
 * - 二进制模式：1010 1010 1010 1010
 * - 十六进制：0xAAAA
 */
#define KR_KEY_RELOAD    ((uint16_t)0xAAAA)  // 重载键值（喂狗操作）

/**
 * @brief 看门狗使能键值
 * @details
 * - 作用：启动独立看门狗功能
 * - 行为：使能看门狗时钟，激活复位功能
 * - 使用场景：系统初始化阶段配置后写入（通常只执行一次）
 * - 二进制模式：1100 1100 1100 1100  
 * - 十六进制：0xCCCC
 * @warning 写入该值后将立即启动看门狗，必须在配置超时参数后使用
 */
#define KR_KEY_ENABLE    ((uint16_t)0xCCCC)  // 使能键值（激活看门狗）

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup IWDG_Private_Functions
  * @{
  */

/** @defgroup IWDG_Group1 Prescaler and Counter configuration functions
 *  @brief   Prescaler and Counter configuration functions
 *
@verbatim   
 ===============================================================================
              ##### Prescaler and Counter configuration functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  独立看门狗(IWDG)寄存器写访问控制函数
  * @param  IWDG_WriteAccess: 写访问控制标志
  *         - 参数可选值：
  *           @arg IWDG_WriteAccess_Enable:  允许写入PR和RLR寄存器（0x5555）
  *           @arg IWDG_WriteAccess_Disable: 禁止写入PR和RLR寄存器（0x0000）
  * @retval 无
  * @note   该操作通过向密钥寄存器(KR)写入特定值来解锁/锁定寄存器保护
  *         必须先解锁才能修改预分频器(PR)和重载值(RLR)寄存器
  */
void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess)
{
  /* 参数有效性检查：验证输入是否在允许的取值范围内 */
  assert_param(IS_IWDG_WRITE_ACCESS(IWDG_WriteAccess));
  
  /* 向密钥寄存器写入指定值，用于开启/关闭寄存器写保护 */
  IWDG->KR = IWDG_WriteAccess;  // 0x5555=解锁，0x0000=锁定
}

/**
  * @brief  设置独立看门狗预分频器
  * @param  IWDG_Prescaler: 预分频系数
  *         - 参数可选值：
  *           @arg IWDG_Prescaler_4   4分频   (0x00)
  *           @arg IWDG_Prescaler_8   8分频   (0x01) 
  *           @arg IWDG_Prescaler_16  16分频  (0x02)
  *           @arg IWDG_Prescaler_32  32分频  (0x03)
  *           @arg IWDG_Prescaler_64  64分频  (0x04)
  *           @arg IWDG_Prescaler_128 128分频 (0x05)
  *           @arg IWDG_Prescaler_256 256分频 (0x06)
  * @retval 无
  * @note   预分频器决定看门狗时钟频率，与重载值共同决定超时时间
  *         修改前需先通过IWDG_WriteAccessCmd()解锁写保护
  */
void IWDG_SetPrescaler(uint8_t IWDG_Prescaler)
{
  /* 验证输入参数是否符合预定义的预分频值 */
  assert_param(IS_IWDG_PRESCALER(IWDG_Prescaler));
  
  /* 将有效参数写入预分频寄存器(PR) */
  IWDG->PR = IWDG_Prescaler;  // 写入后立即生效，但需要等待硬件更新完成
}

/**
  * @brief  设置独立看门狗重载值
  * @param  Reload: 12位重载值（0-0xFFF）
  *         - 该值决定看门狗超时时间，实际值=Reload + 1
  * @retval 无
  * @note   计算公式：Timeout = (Reload + 1) * Prescaler / LSI_Frequency
  *         修改前需先解锁写保护，修改后需调用IWDG_ReloadCounter()生效
  */
void IWDG_SetReload(uint16_t Reload)
{
  /* 验证重载值是否在12位有效范围内（0-0xFFF） */
  assert_param(IS_IWDG_RELOAD(Reload));
  
  /* 将重载值写入重载寄存器(RLR) */
  IWDG->RLR = Reload;  // 该值会在下次重载时生效
}

/**
  * @brief  重载看门狗计数器（喂狗操作）
  * @param  无
  * @retval 无
  * @note   写入0xAAAA到KR寄存器会使当前RLR值重新加载到计数器
  *         必须在计数器归零前执行，否则触发系统复位
  */
void IWDG_ReloadCounter(void)
{
  /* 向密钥寄存器写入重载指令（0xAAAA） */
  IWDG->KR = KR_KEY_RELOAD;  // 该操作同时会锁定PR和RLR寄存器
}

/**
  * @brief  启动独立看门狗
  * @param  无
  * @retval 无
  * @note   写入0xCCCC到KR寄存器后：
  *         - 看门狗立即开始递减计数
  *         - PR和RLR寄存器变为只读状态
  *         - 无法通过软件停止，只有硬件复位才能关闭
  */
void IWDG_Enable(void)
{
  /* 向密钥寄存器写入启动指令（0xCCCC） */
  IWDG->KR = KR_KEY_ENABLE;  // 该操作会立即启动看门狗
}

/**
  * @brief  获取独立看门狗状态标志
  * @param  IWDG_FLAG: 要检查的状态标志
  *         - 参数可选值：
  *           @arg IWDG_FLAG_PVU: 预分频值更新中标志（Bit0）
  *           @arg IWDG_FLAG_RVU: 重载值更新中标志（Bit1）
  * @retval 标志状态：SET（1）或 RESET（0）
  * @note   当PVU/RVU=1时，表示对应寄存器正在更新，此时不能修改寄存器值
  *         硬件自动置位，更新完成后自动清零
  */
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG)
{
  FlagStatus bitstatus = RESET;
  
  /* 验证输入标志是否合法 */
  assert_param(IS_IWDG_FLAG(IWDG_FLAG));
  
  /* 检查状态寄存器(SR)对应标志位 */
  if ((IWDG->SR & IWDG_FLAG) != (uint32_t)RESET)  // 按位与操作检测标志
  {
    bitstatus = SET;    // 标志位置位
  }
  else
  {
    bitstatus = RESET;  // 标志位未置位
  }
  
  /* 返回检测结果 */
  return bitstatus;
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

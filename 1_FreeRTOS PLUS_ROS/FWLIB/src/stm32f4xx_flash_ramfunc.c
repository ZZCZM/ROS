/**
  ******************************************************************************
  * @file    stm32f4xx_flash_ramfunc.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   FLASH RAMFUNC module driver.
  *          This file provides a FLASH firmware functions which should be 
  *          executed from internal SRAM
  *            + Stop/Start the flash interface while System Run
  *            + Enable/Disable the flash sleep while System Run
  *  
 @verbatim    
 ==============================================================================
                    ##### APIs executed from Internal RAM #####
  ==============================================================================
  [..]
    *** ARM Compiler ***
    --------------------
    [..] RAM functions are defined using the toolchain options. 
         Functions that are be executed in RAM should reside in a separate
         source module. Using the 'Options for File' dialog you can simply change
         the 'Code / Const' area of a module to a memory space in physical RAM.
         Available memory areas are declared in the 'Target' tab of the 
         Options for Target' dialog.

    *** ICCARM Compiler ***
    -----------------------
    [..] RAM functions are defined using a specific toolchain keyword "__ramfunc".

    *** GNU Compiler ***
    --------------------
    [..] RAM functions are defined using a specific toolchain attribute
         "__attribute__((section(".RamFunc")))".
  
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
#include "stm32f4xx_flash_ramfunc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup FLASH RAMFUNC 
  * @brief FLASH RAMFUNC driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FLASH_RAMFUNC_Private_Functions
  * @{
  */

/** @defgroup FLASH_RAMFUNC_Group1 Peripheral features functions executed from internal RAM 
  *  @brief Peripheral Extended features functions 
  *
@verbatim   

 ===============================================================================
                      ##### ramfunc functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions that should be executed from RAM 
    transfers.
    
@endverbatim
  * @{
  */
/**
  * @brief  系统运行时闪存接口启停控制
  * @param  NewState: 接口控制状态
  *   @arg ENABLE:  启动闪存接口（关闭接口停止位）
  *   @arg DISABLE: 停止闪存接口（激活接口停止位）
  * @retval 无
  * @note   该函数仅适用于STM32F411xx系列设备
  * @warning 必须从RAM执行该操作！当从闪存执行此操作时，
  *          会导致总线错误（BUS FAULT）
  * @note 操作寄存器：
  *       PWR->CR的第12位（FISSR）：
  *       0=启动物理接口，1=停止物理接口
  *       寄存器映射地址：0x4000_7000
  */
__RAM_FUNC FLASH_FlashInterfaceCmd(FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* 清除FISSR位（设置位12为0）
       当系统时钟保持运行时打开闪存接口 */
    CLEAR_BIT(PWR->CR, PWR_CR_FISSR); // 等效于 PWR->CR &= ~(1 << 12)
  }
  else
  {
    /* 设置FISSR位（设置位12为1）
       在系统运行期间关闭闪存物理接口 */
    SET_BIT(PWR->CR, PWR_CR_FISSR);  // 等效于 PWR->CR |= (1 << 12)
  }
}

/**
  * @brief  闪存睡眠模式控制
  * @param  NewState: 睡眠模式状态
  *   @arg ENABLE:  进入节能模式（功耗优先）
  *   @arg DISABLE: 保持正常模式（性能优先）
  * @retval 无
  * @note   该函数仅适用于STM32F411xx系列设备
  * @warning 必须从RAM执行！当闪存处于访问状态时修改此设置
  *          会导致不可预知的行为
  * @note 操作寄存器：
  *       PWR->CR的第11位（FMSSR）：
  *       0=禁用睡眠模式，1=启用睡眠模式
  *       寄存器映射地址：0x4000_7000
  * @note 睡眠模式下：
  *       - 闪存访问延迟可能增加
  *       - 典型功耗降低约30μA/MHz
  */
__RAM_FUNC FLASH_FlashSleepModeCmd(FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* 设置FMSSR位（设置位11为1）
       在系统运行时进入低功耗睡眠模式 */
    SET_BIT(PWR->CR, PWR_CR_FMSSR);  // 等效于 PWR->CR |= (1 << 11)
  }
  else
  {
    /* 清除FMSSR位（设置位11为0）
       退出睡眠模式恢复全性能运行 */
    CLEAR_BIT(PWR->CR, PWR_CR_FMSSR); // 等效于 PWR->CR &= ~(1 << 11)
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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

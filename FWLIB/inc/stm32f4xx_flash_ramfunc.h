/**
  ******************************************************************************
  * @file    stm32f4xx_flash_ramfunc.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Header file of FLASH RAMFUNC driver.
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
#ifndef __STM32F4xx_FLASH_RAMFUNC_H
#define __STM32F4xx_FLASH_RAMFUNC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup FLASH RAMFUNC
  * @{
  */ 
/** @addtogroup FLASH RAMFUNC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** 
  * @brief  __RAM_FUNC 定义（跨编译器RAM函数声明规范）
  * @note   用于声明需要在RAM中执行的函数，针对不同编译器做差异化定义
  * @note   当操作FLASH时，CPU不能同时执行FLASH中的代码，因此需要将相关函数放在RAM中运行
  */ 
#if defined ( __CC_ARM   )
/* ARM Compiler（MDK-ARM）
   ------------
   RAM函数通过工程选项配置实现：
   1. 需要在单独源文件中定义RAM函数
   2. 通过文件选项对话框修改模块的代码/常量区域为物理RAM空间
   3. 可用内存区域在工程选项的Target选项卡中声明
*/
#define __RAM_FUNC void  // 定义空类型限定符，实际通过工程配置实现位置分配

#elif defined ( __ICCARM__ )
/* ICCARM Compiler（IAR）
   ---------------
   使用编译器的特有关键字__ramfunc声明RAM函数：
   - 强制将函数代码放置在RAM中
   - 函数执行时不需要暂停Flash操作
*/
#define __RAM_FUNC __ramfunc void  // IAR专用RAM函数声明关键字

#elif defined   (  __GNUC__  )
/* GNU Compiler（GCC）
   ------------
   使用section属性指定函数段：
   - 通过链接脚本将.RamFunc段分配到RAM地址空间
   - 函数运行时不会触发Flash访问冲突
*/
#define __RAM_FUNC void  __attribute__((section(".RamFunc")))  // GCC段属性声明

#endif

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  控制Flash接口时钟
  * @param  NewState : 功能状态
  *         @arg ENABLE : 开启Flash接口时钟
  *         @arg DISABLE : 关闭Flash接口时钟
  * @note   该函数必须在RAM中执行，用于在编程/擦除操作期间控制时钟状态
  */
__RAM_FUNC FLASH_FlashInterfaceCmd(FunctionalState NewState);

/**
  * @brief  配置Flash睡眠模式
  * @param  NewState : 功能状态
  *         @arg ENABLE : 进入低功耗睡眠模式
  *         @arg DISABLE : 退出低功耗模式
  * @note   该函数影响Flash模块在低功耗模式下的行为，必须在RAM中执行
  */
__RAM_FUNC FLASH_FlashSleepModeCmd(FunctionalState NewState);



#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_FLASH_RAMFUNC_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


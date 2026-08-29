/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.h 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains the headers of the interrupt handlers.
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
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* 异常处理函数 --------------------------------------------------------------*/
/**
  * @brief 不可屏蔽中断处理函数（Non-Maskable Interrupt）
  * @note  处理时钟安全系统(CSS)、外部NMI引脚等产生的最高优先级中断
  *        通常用于处理最严重的硬件异常，无法被常规中断屏蔽
  */
void NMI_Handler(void);

/**
  * @brief 硬件错误异常处理函数
  * @note  处理存储器访问错误、总线错误等严重系统错误
  *        常见触发场景：
  *        - 访问未对齐的内存地址
  *        - 执行未定义的指令
  *        - 从无效地址取指
  * @warning 此错误发生后系统通常处于不可恢复状态，建议记录错误信息后复位系统
  */
void HardFault_Handler(void);

/**
  * @brief 存储器管理异常处理函数
  * @note  处理MPU（内存保护单元）访问违规错误，包括：
  *        - 访问未授权的内存区域
  *        - 向只读区域执行写操作
  *        - 用户模式访问特权级内存
  */
void MemManage_Handler(void);

/**
  * @brief 总线错误异常处理函数
  * @note  处理AHB总线矩阵访问错误，典型场景：
  *        - 访问不存在的外设寄存器
  *        - DMA传输目标地址越界
  *        - 总线超时未响应
  */
void BusFault_Handler(void);

/**
  * @brief 用法错误异常处理函数
  * @note  处理指令执行相关的错误，包括：
  *        - 执行未定义的Thumb指令（UNDEF）
  *        - 尝试进入无效的中断状态（EPSR）
  *        - 除零错误（需启用DIV_0_TRP控制位）
  */
void UsageFault_Handler(void);

/**
  * @brief 系统服务调用处理函数（SuperVisor Call）
  * @note  用于实现用户模式到特权模式的切换
  *        典型应用：RTOS的系统调用接口
  * @note Cortex-M4通过SVC指令触发，指令操作数可传递服务号
  */
void SVC_Handler(void);

/**
  * @brief 调试监视器处理函数
  * @note  处理调试子系统相关事件，当：
  *        - 使能调试监视器时发生断点
  *        - 观察点匹配时触发
  *        仅在调试状态下有效
  */
void DebugMon_Handler(void);

/**
  * @brief 可挂起的系统调用处理函数（Pendable Service Call）
  * @note  用于延迟执行的高优先级请求
  *        典型应用：RTOS的上下文切换
  *        通过设置PENDSVSET位软件触发
  */
void PendSV_Handler(void);

/**
  * @brief 系统节拍定时器处理函数
  * @note  处理SysTick定时器中断（每1ms触发一次）
  *        典型应用：
  *        - 提供系统时间基准
  *        - RTOS的任务调度触发器
  * @note 通过SysTick_Config()函数配置间隔
  */
void SysTick_Handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

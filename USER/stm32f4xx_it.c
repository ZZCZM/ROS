/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Source Code File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
 

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
/******************************************************************************/
/*            Cortex-M4 处理器异常处理程序实现                                 */
/******************************************************************************/

/**
  * @brief  不可屏蔽中断处理函数（NMI）实现
  * @param  None
  * @retval None
  * @note   - 默认空实现，实际应用时应根据NMI来源（时钟安全系统/CSS、外部NMI引脚等）添加处理逻辑
  *         - 典型处理场景：恢复时钟源失败时切换内部时钟
  * @warning 此中断优先级最高（-1），不可被其他中断抢占
  */
void NMI_Handler(void)
{
  /* 建议添加：
     1. 读取RCC_CIR寄存器检查中断源
     2. 清除中断标志位
     3. 系统恢复/报警处理 */
}

/**
  * @brief  硬件错误异常处理实现
  * @param  None
  * @retval None
  * @note   - 触发后应立即保存现场信息（通过SCB寄存器组）
  *         - 典型调试方法：
  *           a) 检查HFSR寄存器（HardFault Status）
  *           b) 分析CFSR（Configurable Fault Status）子状态寄存器
  *           c) 查看BFAR/MFAR故障地址寄存器
  */
void HardFault_Handler(void)
{
  /* 进入诊断循环（调试时可在此设置断点） */
  while (1)
  {
    /* 建议添加：
       1. 错误信息记录（通过串口/内存转储）
       2. LED报警指示
       3. 看门狗复位机制 */
  }
}

/**
  * @brief  内存管理异常处理实现
  * @param  None
  * @retval None
  * @note   - 主要检查MMFSR寄存器（MemManage Fault Status）
  *         - 常见错误类型：
  *           IACCVIOL：指令访问违规
  *           DACCVIOL：数据访问违规
  *           MUNSTKERR：出栈时的内存错误
  */
void MemManage_Handler(void)
{
  while (1)  // MPU保护触发后系统通常不可恢复
  {
    /* 建议处理：
       __disable_irq();  // 关闭全局中断
       SystemReset();    // 触发系统复位 */
  }
}

/**
  * @brief  总线错误异常处理实现
  * @param  None
  * @retval None
  * @note   - 查看BFAR寄存器获取错误访问地址
  *         - 分析BFSR寄存器（BusFault Status）：
  *           IBUSERR：指令总线错误
  *           PRECISERR：精确数据总线错误
  *           IMPRECISERR：非精确总线错误
  */
void BusFault_Handler(void)
{
  while (1)  // 总线错误通常指示硬件连接异常
  {
    /* 典型处理：
       1. 记录错误地址（(uint32_t)SCB->BFAR）
       2. 检查外设时钟使能状态
       3. 验证外设寄存器映射地址 */
  }
}

/**
  * @brief  用法错误异常处理实现
  * @param  None
  * @retval None
  * @note   - 检查UFSR寄存器（UsageFault Status）：
  *           UNDEFINSTR：未定义指令
  *           INVSTATE：无效的EPSR状态
  *           INVPC：非法的EXC_RETURN值
  *           DIVBYZERO：除零错误（需启用SCB_CCR.DIV_0_TRP）
  */
void UsageFault_Handler(void)
{
  while (1)  // 指令级错误通常需要固件更新
  {
    /* 建议处理：
       1. 反汇编出错地址（通过PC寄存器）
       2. 检查栈溢出情况
       3. 验证编译器优化级别 */
  }
}

// 注意：SVC_Handler默认被注释，启用需取消注释
#if 0
/**
  * @brief  系统调用处理程序实现
  * @param  None
  * @retval None
  * @note   - 通过SVC指令触发（例：SVC 0x01）
  *         - 应用场景：
  *           a) RTOS系统服务入口
  *           b) 特权级操作封装
  *         - 通过R0-R3寄存器传递参数，PC获取服务号
  */
void SVC_Handler(void)
{
  /* 典型实现：
     uint8_t svc_num = ((uint8_t*)__get_PSP())[-2]; // 提取服务号
     switch(svc_num) {
       case 0: // 系统服务1
         break;
       case 1: // 系统服务2
         break;
     } */
}
#endif

/**
  * @brief  调试监视器处理程序实现
  * @param  None
  * @retval None
  * @note   - 仅在调试模式激活时触发
  *         - 用于实现硬件断点/观察点的高级调试
  *         - 正常运行时不应触发此中断
  */
void DebugMon_Handler(void)
{
  /* 调试专用处理，通常保持空实现 */
}


///**
//  * @brief  This function handles PendSVC exception.
//  * @param  None
//  * @retval None
//  */
//void PendSV_Handler(void)
//{
//}

///**
//  * @brief  This function handles SysTick Handler.
//  * @param  None
//  * @retval None
//  */
//void SysTick_Handler(void)
//{
// 
//}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

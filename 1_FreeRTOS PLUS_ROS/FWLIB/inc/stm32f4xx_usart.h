/**
  ******************************************************************************
  * @file    stm32f4xx_usart.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the USART 
  *          firmware library.    
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
#ifndef __STM32F4xx_USART_H
#define __STM32F4xx_USART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup USART
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/** 
  * @brief  USART初始化结构体定义
  * @note   用于配置USART/UART通信基础参数
  */
typedef struct
{
  /**
    * @brief 波特率设置
    * @note 计算公式：
    *       - 整数分频 = PCLKx / (8*(OVR8+1)*BaudRate)
    *       - 小数分频 = (整数分频的小数部分 * 8*(OVR8+1)) + 0.5
    *       OVR8为CR1寄存器的过采样8倍模式位(第15位)
    * @note 常用波特率参考：
    *       - 115200bps @ PCLK=45MHz → 45M/(8*16*115200) ≈ 3.05
    */
  uint32_t USART_BaudRate;

  /**
    * @brief 数据字长配置
    * @value 可取值 @ref USART_Word_Length
    *       - USART_WordLength_8b: 8位数据帧
    *       - USART_WordLength_9b: 9位数据帧
    * @note 9位模式时用于奇偶校验位或自定义用途
    */
  uint16_t USART_WordLength;

  /**
    * @brief 停止位配置
    * @value 可取值 @ref USART_Stop_Bits
    *       - USART_StopBits_1:   1位停止位
    *       - USART_StopBits_0_5: 0.5位停止位
    *       - USART_StopBits_2:   2位停止位 
    *       - USART_StopBits_1_5: 1.5位停止位
    * @note 实际停止位持续时间取决于过采样率
    */
  uint16_t USART_StopBits;

  /**
    * @brief 奇偶校验模式
    * @value 可取值 @ref USART_Parity
    *       - USART_Parity_No:  无校验
    *       - USART_Parity_Even:偶校验
    *       - USART_Parity_Odd: 奇校验
    * @note 启用校验时数据位包含校验位：
    *       - 8位字长模式：第9位为校验位
    *       - 9位字长模式：使用全部9位数据
    */
  uint16_t USART_Parity;

  /**
    * @brief 工作模式配置
    * @value 可取值 @ref USART_Mode 的组合
    *       - USART_Mode_Rx: 接收使能
    *       - USART_Mode_Tx: 发送使能
    * @example 同时启用接收和发送：
    *       USART_Mode_Rx | USART_Mode_Tx
    */
  uint16_t USART_Mode;

  /**
    * @brief 硬件流控制配置
    * @value 可取值 @ref USART_Hardware_Flow_Control
    *       - USART_HardwareFlowControl_None: 禁用流控
    *       - USART_HardwareFlowControl_RTS:  使能RTS流控
    *       - USART_HardwareFlowControl_CTS:  使能CTS流控
    *       - USART_HardwareFlowControl_RTS_CTS: 全流控
    * @note 需要外部硬件电路支持
    */
  uint16_t USART_HardwareFlowControl;
} USART_InitTypeDef;

/** 
  * @brief  USART时钟配置结构体
  * @note   用于同步模式下的时钟参数配置
  */
typedef struct
{
  /**
    * @brief 时钟使能控制
    * @value 可取值 @ref USART_Clock
    *       - USART_Clock_Disable: 关闭时钟输出
    *       - USART_Clock_Enable:  使能时钟输出
    */
  uint16_t USART_Clock;

  /**
    * @brief 时钟极性配置
    * @value 可取值 @ref USART_Clock_Polarity
    *       - USART_CPOL_Low:  空闲时时钟低电平
    *       - USART_CPOL_High: 空闲时时钟高电平
    */
  uint16_t USART_CPOL;

  /**
    * @brief 时钟相位配置
    * @value 可取值 @ref USART_Clock_Phase
    *       - USART_CPHA_1Edge: 在第一个边沿捕获数据
    *       - USART_CPHA_2Edge: 在第二个边沿捕获数据
    */
  uint16_t USART_CPHA;

  /**
    * @brief 最后一位时钟脉冲控制
    * @value 可取值 @ref USART_Last_Bit
    *       - USART_LastBit_Disable: 最后一位无时钟脉冲
    *       - USART_LastBit_Enable:  输出最后一位时钟脉冲
    * @note 影响SCLK引脚的最后一位时钟输出
    */
  uint16_t USART_LastBit;
} USART_ClockInitTypeDef;

/* 外设验证宏 --------------------------------------------------*/
/**
  * @brief 验证所有USART/UART外设实例
  * @param PERIPH 外设实例
  * @retval 有效性检查结果
  */
#define IS_USART_ALL_PERIPH(PERIPH) (((PERIPH) == USART1) || \
                                     ((PERIPH) == USART2) || \
                                     ((PERIPH) == USART3) || \
                                     ((PERIPH) == UART4)  || \
                                     ((PERIPH) == UART5)  || \
                                     ((PERIPH) == USART6) || \
                                     ((PERIPH) == UART7)  || \
                                     ((PERIPH) == UART8))

/**
  * @brief 验证USART1/2/3/6外设
  * @note 这些是完整的USART外设，支持同步模式
  */
#define IS_USART_1236_PERIPH(PERIPH) (((PERIPH) == USART1) || \
                                      ((PERIPH) == USART2) || \
                                      ((PERIPH) == USART3) || \
                                      ((PERIPH) == USART6))

/* 数据位长度配置 ----------------------------------------------*/
#define USART_WordLength_8b  ((uint16_t)0x0000)  /*!< 8位数据帧，CR1[12]=0 */
#define USART_WordLength_9b  ((uint16_t)0x1000) /*!< 9位数据帧，CR1[12]=1 */

/**
  * @brief 验证数据位长度参数
  * @param LENGTH 数据位长度配置
  * @retval 参数有效性
  */
#define IS_USART_WORD_LENGTH(LENGTH) (((LENGTH) == USART_WordLength_8b) || \
                                      ((LENGTH) == USART_WordLength_9b))

/**
  * @}
  */ 
/** @defgroup USART_Stop_Bits 停止位配置组
  * @brief 配置串口通信的停止位长度
  * @note 停止位长度影响帧格式和时序容错性
  * @{
  */ 
  
#define USART_StopBits_1     ((uint16_t)0x0000)  /*!< 1位停止位，CR2[13:12]=00 */
#define USART_StopBits_0_5   ((uint16_t)0x1000)  /*!< 0.5位停止位(智能卡模式)，CR2[13:12]=01 */
#define USART_StopBits_2     ((uint16_t)0x2000)  /*!< 2位停止位，CR2[13:12]=10 */
#define USART_StopBits_1_5   ((uint16_t)0x3000)  /*!< 1.5位停止位，CR2[13:12]=11 */

/**
  * @brief 验证停止位参数有效性
  * @param STOPBITS 停止位配置值
  * @retval 参数有效性检查结果
  * @note 实际物理停止位时长计算：
  *       T_stop = (STOPBITS+1) * T_bit
  */
#define IS_USART_STOPBITS(STOPBITS) (((STOPBITS) == USART_StopBits_1) || \
                                     ((STOPBITS) == USART_StopBits_0_5) || \
                                     ((STOPBITS) == USART_StopBits_2) || \
                                     ((STOPBITS) == USART_StopBits_1_5))
/** @} */  // 结束停止位配置组

/** @defgroup USART_Parity 奇偶校验配置组
  * @brief 配置数据帧的奇偶校验模式
  * @note 启用校验时自动添加/检测校验位
  * @{
  */ 
  
#define USART_Parity_No    ((uint16_t)0x0000)  /*!< 无校验，CR1[10:9]=00 */
#define USART_Parity_Even  ((uint16_t)0x0400)  /*!< 偶校验，CR1[10:9]=10 */
#define USART_Parity_Odd   ((uint16_t)0x0600)   /*!< 奇校验，CR1[10:9]=11 */

/**
  * @brief 验证奇偶校验参数有效性
  * @param PARITY 校验模式配置值
  * @note 校验位位置：
  *       - 8位数据模式：第9位为校验位
  *       - 9位数据模式：使用全部9位数据
  */
#define IS_USART_PARITY(PARITY) (((PARITY) == USART_Parity_No) || \
                                 ((PARITY) == USART_Parity_Even) || \
                                 ((PARITY) == USART_Parity_Odd))
/** @} */  // 结束奇偶校验组

/** @defgroup USART_Mode 工作模式配置组
  * @brief 启用接收/发送功能
  * @note 可组合使用实现全双工通信
  * @{
  */ 
  
#define USART_Mode_Rx  ((uint16_t)0x0004)  /*!< 接收使能，CR1[2]=1 */
#define USART_Mode_Tx  ((uint16_t)0x0008)  /*!< 发送使能，CR1[3]=1 */

/**
  * @brief 验证工作模式参数有效性
  * @param MODE 工作模式配置值
  * @note 有效组合示例：
  *       - 只接收：USART_Mode_Rx
  *       - 全双工：USART_Mode_Rx | USART_Mode_Tx
  */
#define IS_USART_MODE(MODE) ((((MODE) & (uint16_t)0xFFF3) == 0x00) && ((MODE) != (uint16_t)0x00))
/** @} */  // 结束工作模式组

/** @defgroup USART_Hardware_Flow_Control 硬件流控制配置组
  * @brief 配置硬件流控制信号
  * @note 需要外部电路支持RTS/CTS引脚连接
  * @{
  */ 
#define USART_HardwareFlowControl_None    ((uint16_t)0x0000)  /*!< 禁用流控，CR3[8:9]=00 */
#define USART_HardwareFlowControl_RTS     ((uint16_t)0x0100)  /*!< 使能RTS流控，CR3[9]=1 */
#define USART_HardwareFlowControl_CTS     ((uint16_t)0x0200)  /*!< 使能CTS流控，CR3[8]=1 */
#define USART_HardwareFlowControl_RTS_CTS ((uint16_t)0x0300)  /*!< 全流控，CR3[8:9]=11 */

/**
  * @brief 验证流控制参数有效性
  * @param CONTROL 流控配置值
  * @note RTS/CTS电平逻辑：
  *       - RTS: 表示本端准备好接收
  *       - CTS: 允许对方发送数据
  */
#define IS_USART_HARDWARE_FLOW_CONTROL(CONTROL)\
                              (((CONTROL) == USART_HardwareFlowControl_None) || \
                               ((CONTROL) == USART_HardwareFlowControl_RTS) || \
                               ((CONTROL) == USART_HardwareFlowControl_CTS) || \
                               ((CONTROL) == USART_HardwareFlowControl_RTS_CTS))
/** @} */  // 结束硬件流控组

/** @defgroup USART_Clock 时钟输出控制组
  * @brief 同步模式下的时钟使能控制
  * @note 仅适用于USART同步主模式
  * @{
  */ 
#define USART_Clock_Disable  ((uint16_t)0x0000)  /*!< 关闭时钟输出，CR2[11]=0 */
#define USART_Clock_Enable   ((uint16_t)0x0800)  /*!< 使能时钟输出，CR2[11]=1 */

/**
  * @brief 验证时钟控制参数有效性
  * @param CLOCK 时钟控制配置值
  * @note 同步模式下必须使能时钟输出
  */
#define IS_USART_CLOCK(CLOCK) (((CLOCK) == USART_Clock_Disable) || \
                               ((CLOCK) == USART_Clock_Enable))
/** @} */  // 结束时钟控制组

/** @defgroup USART_Clock_Polarity 时钟极性配置组
  * @brief 配置同步时钟的空闲状态电平
  * @{
  */ 
#define USART_CPOL_Low   ((uint16_t)0x0000)  /*!< 空闲低电平，CR2[10]=0 */
#define USART_CPOL_High  ((uint16_t)0x0400)  /*!< 空闲高电平，CR2[10]=1 */

/**
  * @brief 验证时钟极性参数
  * @note 影响数据采样时刻：
  *       CPOL=0: 上升沿捕获数据
  *       CPOL=1: 下降沿捕获数据
  */
#define IS_USART_CPOL(CPOL) (((CPOL) == USART_CPOL_Low) || ((CPOL) == USART_CPOL_High))
/** @} */  // 结束时钟极性组

/** @defgroup USART_Clock_Phase 时钟相位配置组
  * @brief 配置数据采样边沿
  * @{
  */
#define USART_CPHA_1Edge  ((uint16_t)0x0000)  /*!< 第一个边沿采样，CR2[9]=0 */
#define USART_CPHA_2Edge  ((uint16_t)0x0200)  /*!< 第二个边沿采样，CR2[9]=1 */

/**
  * @brief 验证时钟相位参数
  * @note 与CPOL组合形成四种模式：
  *       Mode 0: CPOL=0 + CPHA=0
  *       Mode 1: CPOL=0 + CPHA=1
  *       Mode 2: CPOL=1 + CPHA=0
  *       Mode 3: CPOL=1 + CPHA=1
  */
#define IS_USART_CPHA(CPHA) (((CPHA) == USART_CPHA_1Edge) || ((CPHA) == USART_CPHA_2Edge))
/** @} */  // 结束时钟相位组

/** @defgroup USART_Last_Bit 最后一位时钟控制组
  * @brief 控制最后数据位的时钟脉冲输出
  * @{
  */
#define USART_LastBit_Disable  ((uint16_t)0x0000)  /*!< 禁用最后位时钟，CR2[12]=0 */
#define USART_LastBit_Enable   ((uint16_t)0x0100)  /*!< 使能最后位时钟，CR2[12]=1 */

/**
  * @brief 验证最后位时钟参数
  * @note 影响SCLK引脚的最后时钟脉冲：
  *       - 使能时：完整输出所有时钟周期
  *       - 禁用时：提前结束时钟输出
  */
#define IS_USART_LASTBIT(LASTBIT) (((LASTBIT) == USART_LastBit_Disable) || \
                                   ((LASTBIT) == USART_LastBit_Enable))
/** @} */  // 结束最后位控制组

/**
  * @}
  */ 
/** @defgroup USART_Interrupt_definition 中断类型定义组
  * @brief USART/UART支持的中断类型配置
  * @note 中断源与CR1/CR3寄存器使能位关联
  * @{
  */
  
#define USART_IT_PE      ((uint16_t)0x0028)  /*!< 奇偶校验错误中断，CR1[8]=PEIE */
#define USART_IT_TXE     ((uint16_t)0x0727)  /*!< 发送数据寄存器空中断，CR1[7]=TXEIE */
#define USART_IT_TC      ((uint16_t)0x0626)  /*!< 传输完成中断，CR1[6]=TCIE */
#define USART_IT_RXNE    ((uint16_t)0x0525)  /*!< 接收数据非空中断，CR1[5]=RXNEIE */
#define USART_IT_ORE_RX  ((uint16_t)0x0325)  /*!< 接收溢出中断（RXNEIE使能时有效） */
#define USART_IT_IDLE    ((uint16_t)0x0424)  /*!< 空闲线路检测中断，CR1[4]=IDLEIE */
#define USART_IT_LBD     ((uint16_t)0x0846)  /*!< LIN断开检测中断，CR2[6]=LBDIE */
#define USART_IT_CTS     ((uint16_t)0x096A)  /*!< CTS标志变化中断，CR3[10]=CTSIE */
#define USART_IT_ERR     ((uint16_t)0x0060)  /*!< 错误中断总开关（FE/NE/ORE） */
#define USART_IT_ORE_ER  ((uint16_t)0x0360)  /*!< 溢出错误中断（EIE使能时有效） */
#define USART_IT_NE      ((uint16_t)0x0260)  /*!< 噪声错误中断 */
#define USART_IT_FE      ((uint16_t)0x0160)  /*!< 帧错误中断 */

/** @defgroup USART_Legacy 历史定义兼容组
  * @brief 保持旧版本代码兼容的宏定义
  * @{
  */
#define USART_IT_ORE  USART_IT_ORE_ER  /*!< 旧版溢出错误中断宏（建议使用USART_IT_ORE_ER）*/
/** @} */  // 结束历史定义组

/* 中断配置校验 -------------------------------------------------------*/
/**
  * @brief 验证中断配置参数有效性
  * @param IT 中断类型配置值
  * @note 用于USART_ITConfig()函数输入校验
  */
#define IS_USART_CONFIG_IT(IT) (((IT) == USART_IT_PE) || ((IT) == USART_IT_TXE) || \
                                ((IT) == USART_IT_TC) || ((IT) == USART_IT_RXNE) || \
                                ((IT) == USART_IT_IDLE) || ((IT) == USART_IT_LBD) || \
                                ((IT) == USART_IT_CTS) || ((IT) == USART_IT_ERR))

/**
  * @brief 验证中断状态获取参数
  * @param IT 中断类型值
  * @note 用于USART_GetITStatus()函数输入校验
  */
#define IS_USART_GET_IT(IT) (((IT) == USART_IT_PE) || ((IT) == USART_IT_TXE) || \
                             ((IT) == USART_IT_TC) || ((IT) == USART_IT_RXNE) || \
                             ((IT) == USART_IT_IDLE) || ((IT) == USART_IT_LBD) || \
                             ((IT) == USART_IT_CTS) || ((IT) == USART_IT_ORE) || \
                             ((IT) == USART_IT_ORE_RX) || ((IT) == USART_IT_ORE_ER) || \
                             ((IT) == USART_IT_NE) || ((IT) == USART_IT_FE))

/**
  * @brief 验证中断清除参数有效性
  * @param IT 中断类型值
  * @note 用于USART_ClearITPendingBit()函数输入校验
  */
#define IS_USART_CLEAR_IT(IT) (((IT) == USART_IT_TC) || ((IT) == USART_IT_RXNE) || \
                               ((IT) == USART_IT_LBD) || ((IT) == USART_IT_CTS))
/** @} */  // 结束中断定义组

/** @defgroup USART_DMA_Requests DMA请求配置组
  * @brief 配置USART的DMA传输请求
  * @note 需要配合DMA控制器使用
  * @{
  */
#define USART_DMAReq_Tx  ((uint16_t)0x0080)  /*!< 发送DMA使能，CR3[7]=DMAT */
#define USART_DMAReq_Rx  ((uint16_t)0x0040)  /*!< 接收DMA使能，CR3[6]=DMAR */

/**
  * @brief 验证DMA请求参数有效性
  * @param DMAREQ DMA请求配置值
  * @note 可组合使用：USART_DMAReq_Tx | USART_DMAReq_Rx
  */
#define IS_USART_DMAREQ(DMAREQ) ((((DMAREQ) & (uint16_t)0xFF3F) == 0x00) && ((DMAREQ) != (uint16_t)0x00))
/** @} */  // 结束DMA请求组

/** @defgroup USART_WakeUp_methods 唤醒方法配置组
  * @brief 配置USART的唤醒检测方式
  * @note 用于低功耗模式唤醒场景
  * @{
  */
#define USART_WakeUp_IdleLine    ((uint16_t)0x0000)  /*!< 空闲线路唤醒，CR1[11:12]=00 */
#define USART_WakeUp_AddressMark ((uint16_t)0x0800)  /*!< 地址标记唤醒，CR1[11:12]=10 */

/**
  * @brief 验证唤醒方法参数有效性
  * @param WAKEUP 唤醒配置值
  * @note 地址标记模式下数据MSB置1表示地址帧
  */
#define IS_USART_WAKEUP(WAKEUP) (((WAKEUP) == USART_WakeUp_IdleLine) || \
                                 ((WAKEUP) == USART_WakeUp_AddressMark))
/** @} */  // 结束唤醒方法组

/**
  * @}
  */
/** @defgroup USART_LIN_Break_Detection_Length LIN断点检测配置组
  * @brief 配置LIN总线断点检测长度
  * @note 断点检测用于LIN总线主模式
  * @{
  */
#define USART_LINBreakDetectLength_10b  ((uint16_t)0x0000)  /*!< 10位断点检测，CR2[5]=0 */
#define USART_LINBreakDetectLength_11b  ((uint16_t)0x0020)  /*!< 11位断点检测，CR2[5]=1 */

/**
  * @brief 验证LIN断点长度参数
  * @param LENGTH 断点长度配置值
  * @note LIN协议要求断点为至少10位低电平
  */
#define IS_USART_LIN_BREAK_DETECT_LENGTH(LENGTH) \
                               (((LENGTH) == USART_LINBreakDetectLength_10b) || \
                                ((LENGTH) == USART_LINBreakDetectLength_11b))
/** @} */  // 结束LIN断点检测组

/** @defgroup USART_IrDA_Low_Power IrDA模式配置组
  * @brief 配置IrDA红外通信模式
  * @note 需配合IrDA编解码器使用
  * @{
  */
#define USART_IrDAMode_LowPower  ((uint16_t)0x0004)  /*!< 低功耗模式，CR3[5]=1，脉冲宽度3/16位 */
#define USART_IrDAMode_Normal    ((uint16_t)0x0000)  /*!< 常规模式，CR3[5]=0，脉冲宽度3倍时钟 */

/**
  * @brief 验证IrDA模式参数
  * @param MODE 工作模式配置值
  * @note 低功耗模式传输距离较短但节能
  */
#define IS_USART_IRDA_MODE(MODE) (((MODE) == USART_IrDAMode_LowPower) || \
                                  ((MODE) == USART_IrDAMode_Normal))
/** @} */  // 结束IrDA模式组

/** @defgroup USART_Flags 状态标志组
  * @brief USART状态寄存器(SR)标志位定义
  * @note 通过USART_GetFlagStatus()查询状态
  * @{
  */
#define USART_FLAG_CTS    ((uint16_t)0x0200)  /*!< CTS标志位，SR[9] */
#define USART_FLAG_LBD    ((uint16_t)0x0100)  /*!< LIN断点检测标志，SR[8] */
#define USART_FLAG_TXE    ((uint16_t)0x0080)  /*!< 发送寄存器空，SR[7] */
#define USART_FLAG_TC     ((uint16_t)0x0040)  /*!< 传输完成，SR[6] */
#define USART_FLAG_RXNE   ((uint16_t)0x0020)  /*!< 接收寄存器非空，SR[5] */
#define USART_FLAG_IDLE   ((uint16_t)0x0010)  /*!< 空闲线路检测，SR[4] */
#define USART_FLAG_ORE    ((uint16_t)0x0008)  /*!< 溢出错误，SR[3] */
#define USART_FLAG_NE     ((uint16_t)0x0004)  /*!< 噪声错误，SR[2] */
#define USART_FLAG_FE     ((uint16_t)0x0002)  /*!< 帧错误，SR[1] */
#define USART_FLAG_PE     ((uint16_t)0x0001)  /*!< 奇偶校验错误，SR[0] */

/**
  * @brief 验证状态标志有效性
  * @param FLAG 状态标志值
  * @note 用于USART_GetFlagStatus()参数校验
  */
#define IS_USART_FLAG(FLAG) (((FLAG) == USART_FLAG_PE) || ((FLAG) == USART_FLAG_TXE) || \
                             ((FLAG) == USART_FLAG_TC) || ((FLAG) == USART_FLAG_RXNE) || \
                             ((FLAG) == USART_FLAG_IDLE) || ((FLAG) == USART_FLAG_LBD) || \
                             ((FLAG) == USART_FLAG_CTS) || ((FLAG) == USART_FLAG_ORE) || \
                             ((FLAG) == USART_FLAG_NE) || ((FLAG) == USART_FLAG_FE))

/**
  * @brief 验证可清除标志有效性
  * @param FLAG 状态标志值
  * @note 部分标志需特殊操作清除：
  *       - TC: 读SR写DR
  *       - RXNE: 读DR
  *       - LBD: 读SR后写DR
  */
#define IS_USART_CLEAR_FLAG(FLAG) ((((FLAG) & (uint16_t)0xFC9F) == 0x00) && ((FLAG) != (uint16_t)0x00))

/* 参数有效性验证 --------------------------------------------------*/
/**
  * @brief 验证波特率范围
  * @param BAUDRATE 波特率值(0.0625k~7.5Mbps)
  * @note 实际有效范围取决于PCLK频率和分频系数
  */
#define IS_USART_BAUDRATE(BAUDRATE) (((BAUDRATE) > 0) && ((BAUDRATE) < 7500001))

/**
  * @brief 验证设备地址
  * @param ADDRESS 4位节点地址(0x0~0xF)
  * @note 用于多机通信的地址筛选
  */
#define IS_USART_ADDRESS(ADDRESS) ((ADDRESS) <= 0xF)

/**
  * @brief 验证9位数据值
  * @param DATA 数据值(0x000~0x1FF)
  * @note 当配置为9位数据模式时有效
  */
#define IS_USART_DATA(DATA) ((DATA) <= 0x1FF)
/** @} */  // 结束状态标志组


/**
  * @}
  */ 

/**
  * @}
  */ 
/* 导出函数原型 ------------------------------------------------------------*/

/**
  * @brief  复位USART外设到默认状态
  * @param  USARTx 目标USART实例（USART1-USART8）
  * @note   复位CR1/CR2/CR3寄存器，关闭时钟输出
  * @warning 操作前需停止DMA传输
  */
void USART_DeInit(USART_TypeDef* USARTx);

/* 初始化配置函数组 *********************************************************/
/**
  * @brief  USART通信参数初始化
  * @param  USARTx 目标USART实例
  * @param  USART_InitStruct 配置结构体指针
  * @note   需先使能USART时钟（RCC_APBxPeriphClockCmd）
  * @note   配置顺序：波特率 → 字长 → 停止位 → 校验 → 模式 → 流控
  */
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);

/**
  * @brief  初始化配置结构体为默认值
  * @param  USART_InitStruct 结构体指针
  * @note   默认配置：115200bps/8N1/无流控/禁用收发
  */
void USART_StructInit(USART_InitTypeDef* USART_InitStruct);

/**
  * @brief  同步时钟参数配置
  * @param  USARTx 目标USART实例
  * @param  USART_ClockInitStruct 时钟配置结构体指针
  * @note   仅适用于同步主模式（CR2[CLKEN]=1）
  */
void USART_ClockInit(USART_TypeDef* USARTx, USART_ClockInitTypeDef* USART_ClockInitStruct);

/**
  * @brief  初始化时钟配置结构体为默认值
  * @param  USART_ClockInitStruct 结构体指针
  * @note   默认配置：时钟禁用/CPOL=0/CPHA=0/最后位时钟禁用
  */
void USART_ClockStructInit(USART_ClockInitTypeDef* USART_ClockInitStruct);

/**
  * @brief  使能/禁用USART外设
  * @param  USARTx 目标USART实例
  * @param  NewState 新状态（ENABLE/DISABLE）
  * @note   禁用时自动停止DMA传输
  */
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);

/**
  * @brief  设置预分频系数
  * @param  USARTx 目标USART实例
  * @param  USART_Prescaler 分频值（0x00-0xFF）
  * @note   实际分频系数 = (USART_Prescaler + 1)
  * @note   与过采样模式共同影响波特率计算
  */
void USART_SetPrescaler(USART_TypeDef* USARTx, uint8_t USART_Prescaler);

/**
  * @brief  启用8倍过采样模式
  * @param  USARTx 目标USART实例
  * @param  NewState 新状态
  * @note   提高抗噪能力但降低最大波特率
  * @note   配置后需重新计算波特率
  */
void USART_OverSampling8Cmd(USART_TypeDef* USARTx, FunctionalState NewState);

/**
  * @brief  启用单线半双工模式
  * @param  USARTx 目标USART实例
  * @param  NewState 新状态
  * @note   复用TX引脚实现半双工通信
  * @warning 需外部电路配合切换收发方向
  */
void USART_OneBitMethodCmd(USART_TypeDef* USARTx, FunctionalState NewState);

/* 数据传输函数组 ***********************************************************/
/**
  * @brief  发送单帧数据
  * @param  USARTx 目标USART实例
  * @param  Data 发送数据（8/9位有效）
  * @note   阻塞式发送，等待TXE标志置位
  * @warning 高9位数据自动截断
  */
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);

/**
  * @brief  接收单帧数据
  * @param  USARTx 目标USART实例
  * @retval 接收数据（8/9位有效）
  * @note   非阻塞读取，需先检查RXNE标志
  * @warning 读取后自动清除RXNE标志
  */
uint16_t USART_ReceiveData(USART_TypeDef* USARTx);

/* 多机通信函数组 ***********************************************************/
/**
  * @brief  设置本机地址
  * @param  USARTx 目标USART实例
  * @param  USART_Address 4位节点地址（0x0-0xF）
  * @note   用于从机模式的地址过滤
  * @warning 需配合USART_WakeUpConfig使用
  */
void USART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address);

/**
  * @brief  配置唤醒检测方式
  * @param  USARTx 目标USART实例
  * @param  USART_WakeUp 唤醒模式（空闲线/地址标记）
  * @note   地址标记模式下MSB=1为地址帧
  */
void USART_WakeUpConfig(USART_TypeDef* USARTx, uint16_t USART_WakeUp);

/**
  * @brief  控制接收器唤醒状态
  * @param  USARTx 目标USART实例
  * @param  NewState 新状态
  * @note   禁用时忽略地址匹配检测
  */
void USART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState);

/* LIN模式函数组 ***********************************************************/
/**
  * @brief  配置LIN断点检测长度
  * @param  USARTx 目标USART实例
  * @param  USART_LINBreakDetectLength 断点长度（10/11位）
  * @note   需先使能LIN模式（USART_LINCmd）
  */
void USART_LINBreakDetectLengthConfig(USART_TypeDef* USARTx, uint16_t USART_LINBreakDetectLength);

/**
  * @brief  启用/禁用LIN模式
  * @param  USARTx 目标USART实例
  * @param  NewState 新状态
  * @note   LIN模式自动启用断点检测
  */
void USART_LINCmd(USART_TypeDef* USARTx, FunctionalState NewState);

/**
  * @brief  发送LIN断点信号
  * @param  USARTx 目标USART实例
  * @note   发送13位低电平同步间隔符
  * @warning 需配置为LIN主模式
  */
void USART_SendBreak(USART_TypeDef* USARTx);

/* 半双工模式函数组 *******************************************************/
/**
  * @brief  启用/禁用半双工模式
  * @param  USARTx 目标USART实例
  * @param  NewState 新状态（ENABLE/DISABLE）
  * @note   复用TX引脚实现双向通信
  * @warning 需外部分时切换收发方向
  * @note   禁止同时使用流控功能
  */
void USART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState);

/* 智能卡模式函数组 *******************************************************/
/**
  * @brief  启用/禁用智能卡模式
  * @param  USARTx 目标USART实例
  * @param  NewState 新状态
  * @note   符合ISO7816-3标准
  * @warning 需配置时钟为372分频系数
  */
void USART_SmartCardCmd(USART_TypeDef* USARTx, FunctionalState NewState);

/**
  * @brief  控制NACK（非应答）生成
  * @param  USARTx 目标USART实例
  * @param  NewState 新状态
  * @note   检测到奇偶错误时自动发送NACK脉冲
  */
void USART_SmartCardNACKCmd(USART_TypeDef* USARTx, FunctionalState NewState);

/**
  * @brief  设置保护时间（Guard Time）
  * @param  USARTx 目标USART实例
  * @param  USART_GuardTime 保护时间值（0-255）
  * @note   定义最后数据位结束到NACK脉冲前的时间：
  *         T_guard = (GT[7:0] + 1) × 32 × T_clk
  */
void USART_SetGuardTime(USART_TypeDef* USARTx, uint8_t USART_GuardTime);

/* IrDA模式函数组 *********************************************************/
/**
  * @brief  配置IrDA红外模式参数
  * @param  USARTx 目标USART实例
  * @param  USART_IrDAMode 模式选择（常规/低功耗）
  * @note   低功耗模式脉冲宽度为3/16位时间
  */
void USART_IrDAConfig(USART_TypeDef* USARTx, uint16_t USART_IrDAMode);

/**
  * @brief  启用/禁用IrDA模式
  * @param  USARTx 目标USART实例
  * @param  NewState 新状态
  * @note   需配置TX/RX引脚为开漏模式
  */
void USART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState);

/* DMA管理函数 ***********************************************************/
/**
  * @brief  控制DMA请求使能
  * @param  USARTx 目标USART实例
  * @param  USART_DMAReq DMA请求类型（发送/接收）
  * @param  NewState 新状态
  * @note   对应DMA通道需预先配置
  * @note   发送DMA使能位：CR3[7]=DMAT
  * @note   接收DMA使能位：CR3[6]=DMAR
  */
void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState);

/* 中断和标志管理函数组 ***************************************************/
/**
  * @brief  配置中断源使能
  * @param  USARTx 目标USART实例
  * @param  USART_IT 中断类型（如USART_IT_RXNE）
  * @param  NewState 新状态
  * @note   需在NVIC中配置中断优先级
  * @warning 修改配置前建议禁用全局中断
  */
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);

/**
  * @brief  获取状态标志
  * @param  USARTx 目标USART实例
  * @param  USART_FLAG 要查询的标志位
  * @retval SET/RESET 标志状态
  * @note   查询前需等待至少2个APB时钟周期
  */
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);

/**
  * @brief  清除指定状态标志
  * @param  USARTx 目标USART实例
  * @param  USART_FLAG 要清除的标志位
  * @note   仅可清除TC/RXNE/LBD/CTS标志
  * @note   部分标志通过读操作自动清除：
  *         - PE/RXNE：读DR寄存器
  *         - TXE：写DR寄存器
  */
void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);

/**
  * @brief  获取中断状态
  * @param  USARTx 目标USART实例
  * @param  USART_IT 中断类型
  * @retval SET/RESET 中断状态
  * @note   结合状态标志位和中断使能位判断
  */
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);

/**
  * @brief  清除中断挂起标志
  * @param  USARTx 目标USART实例
  * @param  USART_IT 中断类型
  * @note   实质清除对应的状态标志位
  * @warning 部分中断需按顺序操作清除：
  *         ORE_ER需先读SR再读DR
  */
void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_USART_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

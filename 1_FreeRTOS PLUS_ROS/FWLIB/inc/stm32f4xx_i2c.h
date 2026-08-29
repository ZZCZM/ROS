/**
  ******************************************************************************
  * @file    stm32f4xx_i2c.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the I2C firmware 
  *          library.
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
#ifndef __STM32F4xx_I2C_H
#define __STM32F4xx_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup I2C
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* 导出类型 ------------------------------------------------------------*/

/**
  * @brief I2C初始化结构体定义
  * @details 用于配置I2C通信接口的工作参数
  */
typedef struct
{
  uint32_t I2C_ClockSpeed;          /*!< 指定SCL时钟频率（单位：Hz）
                                          最大值为400kHz，实际值需根据APB时钟分频计算确定
                                          注：实际通信速率可能受线缆电容和上拉电阻影响 */

  uint16_t I2C_Mode;                /*!< 工作模式选择：
                                          - I2C_Mode_I2C：标准I2C模式
                                          - I2C_Mode_SMBusDevice：SMBus从设备模式
                                          - I2C_Mode_SMBusHost：SMBus主控模式
                                          取值范围参考@ref I2C_mode */

  uint16_t I2C_DutyCycle;           /*!< 快速模式占空比配置：
                                          - I2C_DutyCycle_16_9：Tlow/Thigh = 16/9（标准模式）
                                          - I2C_DutyCycle_2：Tlow/Thigh = 2（Fm+模式）
                                          取值范围参考@ref I2C_duty_cycle_in_fast_mode */

  uint16_t I2C_OwnAddress1;         /*!< 第一设备自身地址配置：
                                          支持7位（0-0x7F）或10位（0-0x3FF）地址格式
                                          注：需与I2C_AcknowledgedAddress参数配合使用 */

  uint16_t I2C_Ack;                 /*!< 应答机制使能控制：
                                          - I2C_Ack_Enable：启用应答（正常模式）
                                          - I2C_Ack_Disable：禁用应答（调试用）
                                          取值范围参考@ref I2C_acknowledgement */

  uint16_t I2C_AcknowledgedAddress; /*!< 地址识别模式配置：
                                          指定识别7位或10位地址格式
                                          取值范围参考@ref I2C_acknowledged_address */
} I2C_InitTypeDef;

/* 导出常量 ------------------------------------------------------------*/

/** @defgroup I2C_Exported_Constants I2C模块导出常量
  * @{
  */

/**
  * @brief I2C外设实例验证宏
  * @param PERIPH 外设实例（I2C1/I2C2/I2C3）
  */
#define IS_I2C_ALL_PERIPH(PERIPH) (((PERIPH) == I2C1) || \
                                   ((PERIPH) == I2C2) || \
                                   ((PERIPH) == I2C3))

/** @defgroup I2C_Digital_Filter 数字滤波器配置
  * @brief 配置I2C数字滤波器去除信号噪声
  * @{
  */
#define IS_I2C_DIGITAL_FILTER(FILTER) ((FILTER) <= 0x0000000F) /*!< 滤波器系数范围0-15 
                                                                    对应时钟周期数 = (FILTER + 1) x tI2CCLK */
/**
  * @}
  */

/** @defgroup I2C_mode 工作模式定义
  * @brief 选择I2C/SMBus协议模式
  * @{
  */
#define I2C_Mode_I2C            ((uint16_t)0x0000)  /*!< 标准I2C模式，兼容Philips规范 */
#define I2C_Mode_SMBusDevice    ((uint16_t)0x0002)   /*!< SMBus从设备模式，支持系统管理总线协议 */
#define I2C_Mode_SMBusHost      ((uint16_t)0x000A)  /*!< SMBus主控模式，支持主机告警协议 */

/* 模式参数验证宏 */
#define IS_I2C_MODE(MODE) (((MODE) == I2C_Mode_I2C) || \
                           ((MODE) == I2C_Mode_SMBusDevice) || \
                           ((MODE) == I2C_Mode_SMBusHost))
/**
  * @}
  */

/** @defgroup I2C_duty_cycle_in_fast_mode 快速模式占空比
  * @brief 配置快速模式下的时钟占空比
  * @{
  */
#define I2C_DutyCycle_16_9  ((uint16_t)0x4000) /*!< 16/9占空比（标准快速模式，400kHz时Thigh=250ns） */
#define I2C_DutyCycle_2     ((uint16_t)0xBFFF) /*!< 2:1占空比（快速模式+，最高1MHz时钟） */

/* 占空比参数验证 */
#define IS_I2C_DUTY_CYCLE(CYCLE) (((CYCLE) == I2C_DutyCycle_16_9) || \
                                  ((CYCLE) == I2C_DutyCycle_2))
/**
  * @}
  */

/** @defgroup I2C_acknowledgement 应答机制配置
  * @brief 控制I2C总线应答(ACK/NACK)行为
  * @{
  */
#define I2C_Ack_Enable   ((uint16_t)0x0400)  /*!< 使能应答：正常通信时需保持使能 */
#define I2C_Ack_Disable  ((uint16_t)0x0000)  /*!< 禁用应答：通常用于调试或特殊传输场景 */

/* ACK状态验证 */
#define IS_I2C_ACK_STATE(STATE) (((STATE) == I2C_Ack_Enable) || \
                                 ((STATE) == I2C_Ack_Disable))
/** @defgroup I2C_transfer_direction 数据传输方向
  * @brief 定义I2C通信方向（主设备作为发送器/接收器）
  * @{
  */

#define I2C_Direction_Transmitter ((uint8_t)0x00) /*!< 主设备作为发送器模式：
                                                       用于向从设备写入数据操作 */
#define I2C_Direction_Receiver    ((uint8_t)0x01) /*!< 主设备作为接收器模式：
                                                       用于从从设备读取数据操作 */

/* 传输方向参数验证 */
#define IS_I2C_DIRECTION(DIRECTION) (((DIRECTION) == I2C_Direction_Transmitter) || \
                                     ((DIRECTION) == I2C_Direction_Receiver))
/**
  * @}
  */

/** @defgroup I2C_acknowledged_address 地址识别模式
  * @brief 配置I2C地址识别长度（7位/10位地址格式）
  * @{
  */

#define I2C_AcknowledgedAddress_7bit  ((uint16_t)0x4000) /*!< 7位地址模式：
                                                              需配合I2C_OwnAddress1的低7位使用 */
#define I2C_AcknowledgedAddress_10bit ((uint16_t)0xC000) /*!< 10位地址模式：
                                                              需配合I2C_OwnAddress1的低10位使用 */

/* 地址模式验证 */
#define IS_I2C_ACKNOWLEDGE_ADDRESS(ADDRESS) (((ADDRESS) == I2C_AcknowledgedAddress_7bit) || \
                                             ((ADDRESS) == I2C_AcknowledgedAddress_10bit))
/**
  * @}
  */

/** @defgroup I2C_registers 寄存器偏移量
  * @brief I2C外设寄存器地址偏移量定义
  * @details 基地址+偏移量访问寄存器的标准方式
  * @{
  */

#define I2C_Register_CR1   ((uint8_t)0x00)  /*!< 控制寄存器1偏移地址（I2C_CR1） */
#define I2C_Register_CR2   ((uint8_t)0x04)  /*!< 控制寄存器2偏移地址（I2C_CR2） */
#define I2C_Register_OAR1  ((uint8_t)0x08)  /*!< 自身地址寄存器1偏移地址（I2C_OAR1） */
#define I2C_Register_OAR2  ((uint8_t)0x0C)  /*!< 自身地址寄存器2偏移地址（I2C_OAR2） */
#define I2C_Register_DR    ((uint8_t)0x10)  /*!< 数据寄存器偏移地址（I2C_DR） */
#define I2C_Register_SR1   ((uint8_t)0x14)  /*!< 状态寄存器1偏移地址（I2C_SR1） */
#define I2C_Register_SR2   ((uint8_t)0x18)  /*!< 状态寄存器2偏移地址（I2C_SR2） */
#define I2C_Register_CCR   ((uint8_t)0x1C)  /*!< 时钟控制寄存器偏移地址（I2C_CCR） */
#define I2C_Register_TRISE ((uint8_t)0x20)  /*!< 上升时间寄存器偏移地址（I2C_TRISE） */

/* 寄存器偏移量验证 */
#define IS_I2C_REGISTER(REGISTER) (((REGISTER) == I2C_Register_CR1) || \
                                   ((REGISTER) == I2C_Register_CR2) || \
                                   ((REGISTER) == I2C_Register_OAR1) || \
                                   ((REGISTER) == I2C_Register_OAR2) || \
                                   ((REGISTER) == I2C_Register_DR) || \
                                   ((REGISTER) == I2C_Register_SR1) || \
                                   ((REGISTER) == I2C_Register_SR2) || \
                                   ((REGISTER) == I2C_Register_CCR) || \
                                   ((REGISTER) == I2C_Register_TRISE))
/** @defgroup I2C_NACK_position NACK应答位置
  * @brief 配置NACK应答的发生位置
  * @{
  */

#define I2C_NACKPosition_Next    ((uint16_t)0x0800) /*!< 下一个字节发送NACK：
                                                          用于多字节接收时提前结束传输 */
#define I2C_NACKPosition_Current ((uint16_t)0xF7FF)  /*!< 当前字节发送NACK：
                                                          立即终止当前数据传输 */

/* NACK位置验证宏 */
#define IS_I2C_NACK_POSITION(POSITION)  (((POSITION) == I2C_NACKPosition_Next) || \
                                         ((POSITION) == I2C_NACKPosition_Current))
/**
  * @}
  */

/** @defgroup I2C_SMBus_alert_pin_level SMBus警报引脚电平
  * @brief 配置SMBus警报引脚(ALERT)的电平状态
  * @{
  */

#define I2C_SMBusAlert_Low  ((uint16_t)0x2000) /*!< 警报引脚低电平有效：
                                                    触发SMBus主机告警协议 */
#define I2C_SMBusAlert_High ((uint16_t)0xDFFF) /*!< 警报引脚高电平有效：
                                                    正常无警报状态 */

/* 警报电平验证 */
#define IS_I2C_SMBUS_ALERT(ALERT) (((ALERT) == I2C_SMBusAlert_Low) || \
                                   ((ALERT) == I2C_SMBusAlert_High))
/**
  * @}
  */

/** @defgroup I2C_PEC_position PEC错误校验位置
  * @brief 配置包错误校验(PEC)字节的传输位置
  * @{
  */

#define I2C_PECPosition_Next     ((uint16_t)0x0800) /*!< PEC字节跟随数据字节之后传输 */
#define I2C_PECPosition_Current  ((uint16_t)0xF7FF) /*!< PEC字节替换当前数据字节位置 */

/* PEC位置验证 */
#define IS_I2C_PEC_POSITION(POSITION) (((POSITION) == I2C_PECPosition_Next) || \
                                       ((POSITION) == I2C_PECPosition_Current))
/**
  * @}
  */

/** @defgroup I2C_interrupts_definition 中断类型定义
  * @brief I2C中断源分类（事件/缓冲区/错误中断）
  * @{
  */

#define I2C_IT_BUF ((uint16_t)0x0400)  /*!< 缓冲区中断：TXE/RXNE事件触发 */
#define I2C_IT_EVT ((uint16_t)0x0200)  /*!< 事件中断：STOPF/ADDR/BTF等状态触发 */ 
#define I2C_IT_ERR ((uint16_t)0x0100)  /*!< 错误中断：BERR/ARLO/OVR等错误触发 */

/* 中断类型验证宏 */
#define IS_I2C_CONFIG_IT(IT) ((((IT) & (uint16_t)0xF8FF) == 0x00) && ((IT) != 0x00))
/**
  * @}
  */

/** @defgroup I2C_interrupts_definition 具体中断源定义
  * @brief I2C具体中断标志位定义
  * @{
  */

#define I2C_IT_SMBALERT  ((uint32_t)0x01008000) /*!< SMBus警报中断（仅SMBus模式） */
#define I2C_IT_TIMEOUT   ((uint32_t)0x01004000) /*!< 超时检测中断（时钟延长超时） */
#define I2C_IT_PECERR    ((uint32_t)0x01001000) /*!< PEC校验错误中断 */
#define I2C_IT_OVR       ((uint32_t)0x01000800) /*!< 溢出/下溢错误中断 */
#define I2C_IT_AF        ((uint32_t)0x01000400) /*!< 应答失败中断（NACK接收） */
#define I2C_IT_ARLO      ((uint32_t)0x01000200) /*!< 仲裁丢失中断 */
#define I2C_IT_BERR      ((uint32_t)0x01000100) /*!< 总线错误中断 */
#define I2C_IT_TXE       ((uint32_t)0x06000080) /*!< 发送数据寄存器空中断 */
#define I2C_IT_RXNE      ((uint32_t)0x06000040) /*!< 接收数据寄存器非空中断 */
#define I2C_IT_STOPF     ((uint32_t)0x02000010) /*!< STOP条件检测中断 */
#define I2C_IT_ADD10     ((uint32_t)0x02000008) /*!< 10位地址头发送完成中断 */
#define I2C_IT_BTF       ((uint32_t)0x02000004) /*!< 字节传输完成中断 */
#define I2C_IT_ADDR      ((uint32_t)0x02000002) /*!< 地址匹配中断（从模式） */
#define I2C_IT_SB        ((uint32_t)0x02000001) /*!< 起始位检测中断（主模式） */

/* 中断清除验证宏 --------------------------------------------------------*/
/**
  * @brief 可清除中断标志验证宏
  * @param IT 待验证的中断标志
  * @retval 有效性检查结果（TRUE/FALSE）
  * @note 该宏验证传入的中断标志是否为软件可清除类型
  *       过滤掉不可清除的中断标志位（保留位0x20FF掩码）
  */
#define IS_I2C_CLEAR_IT(IT) ((((IT) & (uint16_t)0x20FF) == 0x00) && ((IT) != (uint16_t)0x00))

/* 中断获取验证宏 --------------------------------------------------------*/
/**
  * @brief 中断标志有效性验证宏
  * @param IT 待验证的中断标志
  * @retval 有效性检查结果（TRUE/FALSE）
  * @details 验证所有21个合法中断标志：
  *          包括错误中断、状态中断和缓冲区中断
  */
#define IS_I2C_GET_IT(IT) (((IT) == I2C_IT_SMBALERT) || \
                           ((IT) == I2C_IT_TIMEOUT) || \
                           ((IT) == I2C_IT_PECERR) || \
                           ((IT) == I2C_IT_OVR) || \
                           ((IT) == I2C_IT_AF) || \
                           ((IT) == I2C_IT_ARLO) || \
                           ((IT) == I2C_IT_BERR) || \
                           ((IT) == I2C_IT_TXE) || \
                           ((IT) == I2C_IT_RXNE) || \
                           ((IT) == I2C_IT_STOPF) || \
                           ((IT) == I2C_IT_ADD10) || \
                           ((IT) == I2C_IT_BTF) || \
                           ((IT) == I2C_IT_ADDR) || \
                           ((IT) == I2C_IT_SB))

/** @defgroup I2C_flags_definition 状态标志定义
  * @brief I2C状态寄存器(SR1/SR2)标志位定义
  * @{
  */

/** 
  * @brief SR2寄存器标志（状态寄存器2）
  */
#define I2C_FLAG_DUALF    ((uint32_t)0x00800000) /*!< 双地址模式标志：
                                                     当DUALF=1时启用双地址模式 */
#define I2C_FLAG_SMBHOST  ((uint32_t)0x00400000) /*!< SMBus主机模式标志：
                                                     SMBHOST=1表示处于SMBus主机模式 */
#define I2C_FLAG_SMBDEFAULT ((uint32_t)0x00200000)/*!< SMBus默认地址应答标志：
                                                     收到默认地址时置位 */
#define I2C_FLAG_GENCALL  ((uint32_t)0x00100000)  /*!< 广播呼叫标志：
                                                     收到广播地址时置位 */
#define I2C_FLAG_TRA      ((uint32_t)0x00040000)  /*!< 传输方向标志：
                                                     TRA=1表示处于发送模式 */
#define I2C_FLAG_BUSY     ((uint32_t)0x00020000)  /*!< 总线忙状态标志：
                                                     BUSY=1表示总线被占用 */
#define I2C_FLAG_MSL      ((uint32_t)0x00010000)  /*!< 主从模式标志：
                                                     MSL=1表示处于主模式 */
/** @defgroup I2C_SR1_flags SR1状态寄存器标志
  * @brief I2C状态寄存器1(SR1)标志位定义
  * @{
  */

#define I2C_FLAG_SMBALERT   ((uint32_t)0x10008000) /*!< SMBus警报标志：
                                                        当SMBus设备发出警报信号时置1
                                                        清除方式：软件写0 */
#define I2C_FLAG_TIMEOUT    ((uint32_t)0x10004000) /*!< 时钟延长超时标志：
                                                        检测到超时事件时置1
                                                        清除方式：软件写0 */
#define I2C_FLAG_PECERR     ((uint32_t)0x10001000) /*!< PEC校验错误标志：
                                                        接收的PEC字节不匹配时置1
                                                        清除方式：软件写0 */
#define I2C_FLAG_OVR        ((uint32_t)0x10000800) /*!< 溢出错误标志：
                                                        在DR寄存器未读取时收到新数据置1
                                                        清除方式：读SR1后读DR */
#define I2C_FLAG_AF         ((uint32_t)0x10000400) /*!< 应答失败标志(NACK)：
                                                        从机未返回ACK时置1
                                                        清除方式：软件写0 */
#define I2C_FLAG_ARLO       ((uint32_t)0x10000200) /*!< 仲裁丢失标志：
                                                        总线仲裁失败时置1
                                                        清除方式：软件写0 */
#define I2C_FLAG_BERR       ((uint32_t)0x10000100) /*!< 总线错误标志：
                                                        检测到非法的起停条件时置1
                                                        清除方式：软件写0 */
#define I2C_FLAG_TXE        ((uint32_t)0x10000080) /*!< 发送寄存器空标志：
                                                        DR寄存器为空时可写入新数据
                                                        清除方式：写入DR寄存器 */
#define I2C_FLAG_RXNE       ((uint32_t)0x10000040) /*!< 接收寄存器非空标志：
                                                        DR寄存器包含接收数据
                                                        清除方式：读取DR寄存器 */
#define I2C_FLAG_STOPF      ((uint32_t)0x10000010) /*!< 停止条件检测标志（从模式）：
                                                        检测到停止条件时置1
                                                        清除方式：读SR1后写CR1 */
#define I2C_FLAG_ADD10      ((uint32_t)0x10000008) /*!< 10位地址头已发送标志：
                                                        主模式10位地址发送完成
                                                        清除方式：读SR1后读SR2 */
#define I2C_FLAG_BTF        ((uint32_t)0x10000004) /*!< 字节传输完成标志：
                                                        数据字节传输完成时置1
                                                        清除方式：读SR1后读DR或写DR */
#define I2C_FLAG_ADDR       ((uint32_t)0x10000002) /*!< 地址匹配标志（从模式）：
                                                        接收地址与自身地址匹配时置1
                                                        清除方式：读SR1后读SR2 */
#define I2C_FLAG_SB         ((uint32_t)0x10000001) /*!< 起始位生成标志（主模式）：
                                                        成功生成起始条件时置1
                                                        清除方式：读SR1后写DR */

/**
  * @brief 可清除标志验证宏
  * @param FLAG 需要验证的状态标志
  * @retval 有效性检查结果（TRUE/FALSE）
  * @note 验证标志是否为软件可清除类型，使用0x20FF掩码过滤保留位
  */
#define IS_I2C_CLEAR_FLAG(FLAG) ((((FLAG) & (uint16_t)0x20FF) == 0x00) && ((FLAG) != (uint16_t)0x00))
/**
  * @brief 状态标志有效性验证宏
  * @param FLAG 需要验证的状态标志
  * @retval 有效性检查结果（TRUE/FALSE）
  * @details 验证所有21个I2C状态标志的合法性，覆盖SR1/SR2寄存器所有可检测标志
  *          应用场景：HASH_GetFlagStatus()函数中的参数检查
  */
#define IS_I2C_GET_FLAG(FLAG) ( \
    ((FLAG) == I2C_FLAG_DUALF)      || /* 双地址模式标志（SR2） */ \
    ((FLAG) == I2C_FLAG_SMBHOST)    || /* SMBus主机标志（SR2） */ \
    ((FLAG) == I2C_FLAG_SMBDEFAULT)|| /* SMBus默认地址标志（SR2） */ \
    ((FLAG) == I2C_FLAG_GENCALL)   || /* 广播呼叫标志（SR2） */ \
    ((FLAG) == I2C_FLAG_TRA)       || /* 传输方向标志（SR2） */ \
    ((FLAG) == I2C_FLAG_BUSY)      || /* 总线忙标志（SR2） */ \
    ((FLAG) == I2C_FLAG_MSL)       || /* 主从模式标志（SR2） */ \
    ((FLAG) == I2C_FLAG_SMBALERT)  || /* SMBus警报标志（SR1） */ \
    ((FLAG) == I2C_FLAG_TIMEOUT)   || /* 超时标志（SR1） */ \
    ((FLAG) == I2C_FLAG_PECERR)    || /* PEC错误标志（SR1） */ \
    ((FLAG) == I2C_FLAG_OVR)       || /* 溢出错误标志（SR1） */ \
    ((FLAG) == I2C_FLAG_AF)        || /* 应答失败标志（SR1） */ \
    ((FLAG) == I2C_FLAG_ARLO)      || /* 仲裁丢失标志（SR1） */ \
    ((FLAG) == I2C_FLAG_BERR)      || /* 总线错误标志（SR1） */ \
    ((FLAG) == I2C_FLAG_TXE)       || /* 发送寄存器空标志（SR1） */ \
    ((FLAG) == I2C_FLAG_RXNE)      || /* 接收寄存器非空标志（SR1） */ \
    ((FLAG) == I2C_FLAG_STOPF)     || /* 停止条件标志（SR1） */ \
    ((FLAG) == I2C_FLAG_ADD10)     || /* 10位地址头标志（SR1） */ \
    ((FLAG) == I2C_FLAG_BTF)       || /* 字节传输完成标志（SR1） */ \
    ((FLAG) == I2C_FLAG_ADDR)      || /* 地址匹配标志（SR1） */ \
    ((FLAG) == I2C_FLAG_SB)          /* 起始位生成标志（SR1） */ \
)

/**
  * @}
  */

/** @defgroup I2C_Events 
  * @{
  */

/**
 ===============================================================================
               I2C Master Events (Events grouped in order of communication)
 ===============================================================================
 */
/** 
  * @brief 主模式通信事件定义
  * @details 用于I2C主模式状态机管理，通过检查特定标志组合确定通信阶段
  */

/* --EV5 主模式选择事件 ----------------------------------------------------*/
/**
  * @brief 起始条件生成成功事件
  * @value 0x00030001 (BUSY|MSL|SB)
  * @note 发送START信号后检测，确认总线空闲并进入主模式
  * @应用场景：调用I2C_GenerateSTART()后等待此事件
  */
#define I2C_EVENT_MASTER_MODE_SELECT ((uint32_t)0x00030001)

/* --EV6 地址应答事件 ------------------------------------------------------*/
/**
  * @brief 主发送模式选择成功事件
  * @value 0x00070082 (BUSY|MSL|ADDR|TXE|TRA)
  * @note 7位地址发送后检测，从机已应答且数据寄存器就绪
  * @应用场景：发送从机地址(写方向)后等待此事件
  */
#define I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ((uint32_t)0x00070082)

/**
  * @brief 主接收模式选择成功事件 
  * @value 0x00030002 (BUSY|MSL|ADDR)
  * @note 7位地址发送后检测，从机已应答且进入接收模式
  * @应用场景：发送从机地址(读方向)后等待此事件
  */
#define I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ((uint32_t)0x00030002)

/* --EV9 10位地址模式首字节应答事件 -----------------------------------------*/
/**
  * @brief 10位地址首字节应答事件
  * @value 0x00030008 (BUSY|MSL|ADD10)
  * @note 10位地址模式发送首字节(11110xx)后检测
  * @应用场景：10位地址模式下发送首字节后等待此事件
  */
#define I2C_EVENT_MASTER_MODE_ADDRESS10 ((uint32_t)0x00030008)

/* --EV7 主接收模式数据接收事件 ---------------------------------------------*/
/**
  * @brief 主模式单字节接收完成事件
  * @value 0x00030040 (BUSY|MSL|RXNE)
  * @note DR寄存器已包含有效接收数据
  * @应用场景：接收数据时轮询此事件，触发后立即读取DR
  */
#define I2C_EVENT_MASTER_BYTE_RECEIVED ((uint32_t)0x00030040)

/* --EV8 主发送模式数据传输事件 ---------------------------------------------*/
/**
  * @brief 主模式数据寄存器空事件（传输中）
  * @value 0x00070080 (BUSY|MSL|TRA|TXE)
  * @note 数据已从DR寄存器移入移位寄存器，可写入新数据
  * @应用场景：连续发送时检查此事件，写入后续数据
  */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING ((uint32_t)0x00070080)

/**
  * @brief 主模式字节传输完成事件（已发出）
  * @value 0x00070084 (BUSY|MSL|TRA|TXE|BTF)
  * @note 数据已物理传输到总线，完成一个字节传输
  * @应用场景：最后一次数据发送或需要确认物理传输完成时使用
  */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTED ((uint32_t)0x00070084)


/** 
  * @brief 从模式通信事件定义
  * @details 用于I2C从模式状态机管理，通过检查特定标志组合确定通信阶段
  */

/* --EV1 地址匹配事件组 ----------------------------------------------------*/
/**
  * @brief 单地址接收模式匹配事件
  * @value 0x00020002 (BUSY|ADDR)
  * @note 检测到主机发送的地址与本机地址1匹配（接收方向）
  * @应用场景：从机接收模式初始化后等待地址匹配
  */
#define I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED ((uint32_t)0x00020002)

/**
  * @brief 单地址发送模式匹配事件
  * @value 0x00060082 (BUSY|ADDR|TRA|TXE)
  * @note 检测到主机发送的地址与本机地址1匹配（发送方向）
  * @应用场景：从机发送模式初始化后等待地址匹配
  */
#define I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED ((uint32_t)0x00060082)

/**
  * @brief 双地址接收模式匹配事件
  * @value 0x00820000 (DUALF|BUSY)
  * @note 检测到主机发送的地址与本机地址2匹配（接收方向）
  * @应用场景：启用双地址模式时等待第二个地址匹配
  */
#define I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED ((uint32_t)0x00820000)

/**
  * @brief 双地址发送模式匹配事件
  * @value 0x00860080 (DUALF|TRA|BUSY|TXE)
  * @note 检测到主机发送的地址与本机地址2匹配（发送方向）
  * @应用场景：启用双地址模式时等待第二个地址匹配
  */
#define I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED ((uint32_t)0x00860080)

/**
  * @brief 广播呼叫地址匹配事件
  * @value 0x00120000 (GENCALL|BUSY)
  * @note 检测到主机发送广播地址(0x00)
  * @应用场景：启用广播呼叫功能后等待处理广播数据
  */
#define I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED ((uint32_t)0x00120000)

/* --EV2/EV4 从接收模式事件 -------------------------------------------------*/
/**
  * @brief 从模式数据接收完成事件
  * @value 0x00020040 (BUSY|RXNE)
  * @note DR寄存器包含新接收的数据字节
  * @应用场景：接收数据时循环检测此事件
  */
#define I2C_EVENT_SLAVE_BYTE_RECEIVED ((uint32_t)0x00020040)

/**
  * @brief 从模式停止条件检测事件
  * @value 0x00000010 (STOPF)
  * @note 检测到主机发送的停止条件
  * @应用场景：接收结束时检测停止信号
  */
#define I2C_EVENT_SLAVE_STOP_DETECTED ((uint32_t)0x00000010)

/* --EV3 从发送模式事件 ----------------------------------------------------*/
/**
  * @brief 从模式字节发送完成事件（已确认）
  * @value 0x00060084 (TRA|BUSY|TXE|BTF)
  * @note 数据已成功传输并收到主机ACK
  * @应用场景：连续发送时等待字节传输完成
  */
#define I2C_EVENT_SLAVE_BYTE_TRANSMITTED ((uint32_t)0x00060084)

/**
  * @brief 从模式字节发送中事件
  * @value 0x00060080 (TRA|BUSY|TXE)
  * @note 数据已从DR寄存器移入移位寄存器
  * @应用场景：准备发送下一个字节前检查
  */
#define I2C_EVENT_SLAVE_BYTE_TRANSMITTING ((uint32_t)0x00060080)

/**
  * @brief 从模式应答失败事件
  * @value 0x00000400 (AF)
  * @note 主机返回NACK应答
  * @应用场景：检测到NACK后终止发送流程
  */
#define I2C_EVENT_SLAVE_ACK_FAILURE ((uint32_t)0x00000400)

/**
  * @brief I2C事件有效性验证宏
  * @param EVENT 待验证的事件标识符
  * @retval 有效性检查结果（TRUE/FALSE）
  * @details 该宏用于验证传入的事件参数是否属于标准I2C事件集合，覆盖所有主从模式事件及其组合事件
  */
#define IS_I2C_EVENT(EVENT) ( \
    /* 从模式地址匹配事件 */ \
    ((EVENT) == I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED)    || /* 从发送地址匹配事件 */ \
    ((EVENT) == I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED)       || /* 从接收地址匹配事件 */ \
    /* 从模式双地址匹配事件 */ \
    ((EVENT) == I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED) || /* 第二地址发送匹配 */ \
    ((EVENT) == I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED)  || /* 第二地址接收匹配 */ \
    /* 广播呼叫事件 */ \
    ((EVENT) == I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED)      || /* 广播呼叫地址匹配 */ \
    /* 从接收模式数据事件 */ \
    ((EVENT) == I2C_EVENT_SLAVE_BYTE_RECEIVED)                   || /* 从接收基础事件 */ \
    ((EVENT) == (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_DUALF))  || /* 双地址模式接收事件 */ \
    ((EVENT) == (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_GENCALL))|| /* 广播模式接收事件 */ \
    /* 从发送模式数据事件 */ \
    ((EVENT) == I2C_EVENT_SLAVE_BYTE_TRANSMITTED)                || /* 从发送基础事件 */ \
    ((EVENT) == (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_DUALF)) || /* 双地址发送完成事件 */ \
    ((EVENT) == (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_GENCALL))|| /* 广播模式发送完成事件 */ \
    /* 总线状态检测事件 */ \
    ((EVENT) == I2C_EVENT_SLAVE_STOP_DETECTED)                  || /* 停止位检测事件 */ \
    /* 主模式事件 */ \
    ((EVENT) == I2C_EVENT_MASTER_MODE_SELECT)                   || /* 主模式选择成功事件 */ \
    ((EVENT) == I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)     || /* 主发送模式建立事件 */ \
    ((EVENT) == I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)        || /* 主接收模式建立事件 */ \
    ((EVENT) == I2C_EVENT_MASTER_BYTE_RECEIVED)                 || /* 主接收数据事件 */ \
    ((EVENT) == I2C_EVENT_MASTER_BYTE_TRANSMITTED)             || /* 主发送完成事件 */ \
    ((EVENT) == I2C_EVENT_MASTER_BYTE_TRANSMITTING)            || /* 主发送中事件 */ \
    ((EVENT) == I2C_EVENT_MASTER_MODE_ADDRESS10)               || /* 主模式10位地址事件 */ \
    /* 错误事件 */ \
    ((EVENT) == I2C_EVENT_SLAVE_ACK_FAILURE)                     /* 从模式NACK应答失败事件 */ \
)
/** @defgroup I2C_own_address1 本机地址1校验
  * @brief 验证I2C自身地址1参数的合法性
  * @details 根据I2C协议规定，10位地址最大值为0x3FF
  * @note 该地址值将被写入I2C_OAR1寄存器
  */
#define IS_I2C_OWN_ADDRESS1(ADDRESS1) ((ADDRESS1) <= 0x3FF)  /*!< 10位地址验证宏：
                                                                 7位地址范围：0x00-0x7F
                                                                 10位地址范围：0x000-0x3FF
                                                                 参数应为uint16_t类型 */

/** @defgroup I2C_clock_speed 时钟速度校验 
  * @brief 验证I2C通信速率参数的合法性
  * @details 该值将用于计算TIMINGR寄存器的分频系数
  * @note 标准模式最高100kHz，快速模式最高400kHz
  */
#define IS_I2C_CLOCK_SPEED(SPEED) (((SPEED) >= 0x1) && ((SPEED) <= 400000)) /*!< 时钟频率范围检查：
                                                                              单位：Hz
                                                                              有效范围：1Hz~400kHz
                                                                              注：实际通信速率会受APB时钟限制 */

/**
  * @}
  */

/**
  * @}
  */
/* 导出宏 --------------------------------------------------------------*/
/* 导出函数 ------------------------------------------------------------*/

/**
  * @brief I2C硬件复位函数
  * @param I2Cx I2C外设实例（I2C1/I2C2/I2C3）
  * @note 复位所有寄存器至默认值，需重新初始化配置
  */
void I2C_DeInit(I2C_TypeDef* I2Cx);

/* 初始化配置函数组 ****************************************************/
/**
  * @brief I2C参数初始化
  * @param I2Cx I2C外设实例
  * @param I2C_InitStruct 指向配置结构体的指针
  * @details 配置时钟速度、模式、地址等核心参数
  */
void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);

/**
  * @brief 初始化结构体默认值加载
  * @param I2C_InitStruct 需要初始化的结构体指针
  * @note 默认配置：标准模式100kHz/7位地址/应答使能
  */
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct);

/**
  * @brief I2C外设使能控制
  * @param NewState 使能状态（ENABLE/DISABLE）
  * @note 禁用时自动进入低功耗状态
  */
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* 滤波器配置函数 ******************************************************/
/**
  * @brief 数字滤波器配置
  * @param I2C_DigitalFilter 滤波系数（0-15）
  * @note 用于抑制SCL/SDA线上的尖峰脉冲，每个单位对应1个I2C时钟周期
  */
void I2C_DigitalFilterConfig(I2C_TypeDef* I2Cx, uint16_t I2C_DigitalFilter);

/**
  * @brief 模拟滤波器使能控制
  * @param NewState 使能状态
  * @note 默认使能，可滤除50ns以下的噪声
  */
void I2C_AnalogFilterCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* 总线控制函数 *******************************************************/
/**
  * @brief 生成START条件
  * @param NewState 启动控制（ENABLE-生成START/DISABLE-无操作）
  * @warning 重复调用可能导致总线冲突
  */
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);

/**
  * @brief 生成STOP条件
  * @param NewState 停止控制（ENABLE-生成STOP/DISABLE-无操作）
  * @note 主模式下自动释放总线控制权
  */
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);

/**
  * @brief 发送7位从机地址
  * @param Address 目标地址（7位右对齐）
  * @param I2C_Direction 传输方向（I2C_Direction_Transmitter/Receiver）
  * @note 地址自动左移1位，最低位表示方向
  */
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);

/* 应答控制函数 *******************************************************/
/**
  * @brief 应答机制使能控制
  * @param NewState 使能状态
  * @note 禁用应答用于调试或特殊协议
  */
void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* 多地址配置函数 ****************************************************/
/**
  * @brief 设置第二自身地址
  * @param Address 7位地址值
  * @note 需配合I2C_DualAddressCmd()使用
  */
void I2C_OwnAddress2Config(I2C_TypeDef* I2Cx, uint8_t Address);

/**
  * @brief 双地址模式使能控制
  * @param NewState 使能状态
  * @note 使能后同时响应地址1和地址2
  */
void I2C_DualAddressCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* 广播呼叫配置 ******************************************************/
/**
  * @brief 广播呼叫响应使能
  * @param NewState 使能状态
  * @note 使能后响应地址0x00的广播呼叫
  */
void I2C_GeneralCallCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* 高级控制函数 ******************************************************/
/**
  * @brief 软件复位触发
  * @param NewState 复位控制
  * @note 复位后需重新初始化外设
  */
void I2C_SoftwareResetCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/**
  * @brief 时钟拉伸使能控制
  * @param NewState 使能状态
  * @note 禁用时从机无法延长时钟周期
  */
void I2C_StretchClockCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/**
  * @brief 快速模式占空比配置
  * @param I2C_DutyCycle 占空比模式（I2C_DutyCycle_16_9/2）
  */
void I2C_FastModeDutyCycleConfig(I2C_TypeDef* I2Cx, uint16_t I2C_DutyCycle);

/**
  * @brief NACK位置配置
  * @param I2C_NACKPosition NACK位置（Next/Current）
  * @note 控制NACK在下一个字节还是当前字节发送
  */
void I2C_NACKPositionConfig(I2C_TypeDef* I2Cx, uint16_t I2C_NACKPosition);

/**
  * @brief SMBus警报引脚配置
  * @param I2C_SMBusAlert 警报电平（High/Low）
  */
void I2C_SMBusAlertConfig(I2C_TypeDef* I2Cx, uint16_t I2C_SMBusAlert);

/**
  * @brief 自动重装地址使能
  * @param NewState 使能状态
  * @note 用于DMA传输时的地址自动重载
  */
void I2C_ARPCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* 数据传输函数组 ****************************************************/
/**
  * @brief 发送单字节数据
  * @param Data 待发送的8位数据
  * @note 需在TXE标志置位时调用，发送后自动清除TXE
  */
void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);

/**
  * @brief 接收单字节数据
  * @retval 接收到的8位数据
  * @note 需在RXNE标志置位时调用，读取后自动清除RXNE
  */
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);

/* PEC校验管理函数 **************************************************/
/**
  * @brief PEC传输使能控制
  * @param NewState 使能状态
  * @note 使能后自动生成/校验PEC字节
  */
void I2C_TransmitPEC(I2C_TypeDef* I2Cx, FunctionalState NewState);

/**
  * @brief PEC字节位置配置
  * @param I2C_PECPosition PEC位置（Next/Current）
  * @note 选择PEC作为独立字节传输或替换最后一个数据字节
  */
void I2C_PECPositionConfig(I2C_TypeDef* I2Cx, uint16_t I2C_PECPosition);

/**
  * @brief 触发PEC计算
  * @param NewState 触发控制（ENABLE-开始计算）
  * @note 在接收模式下用于重新计算PEC值
  */
void I2C_CalculatePEC(I2C_TypeDef* I2Cx, FunctionalState NewState);

/**
  * @brief 获取当前PEC值
  * @retval 8位PEC校验码
  * @note 应在传输完成后调用
  */
uint8_t I2C_GetPEC(I2C_TypeDef* I2Cx);

/* DMA传输控制函数 **************************************************/
/**
  * @brief DMA传输使能控制
  * @param NewState 使能状态
  * @note 需配合DMA控制器配置使用
  */
void I2C_DMACmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/**
  * @brief 最后DMA传输标记控制
  * @param NewState 使能状态
  * @note 使能后当前DMA传输为最后一个，传输完成后自动关闭DMA
  */
void I2C_DMALastTransferCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* 中断管理函数 ******************************************************/
/**
  * @brief 读取指定寄存器值
  * @param I2C_Register 寄存器标识（I2C_Register_xxx）
  * @retval 16位寄存器值
  * @note 用于直接访问CR1/CR2等寄存器
  */
uint16_t I2C_ReadRegister(I2C_TypeDef* I2Cx, uint8_t I2C_Register);

/**
  * @brief 中断使能控制
  * @param I2C_IT 中断类型（I2C_IT_xxx）
  * @param NewState 使能状态
  * @note 需配合NVIC配置使用
  */
void I2C_ITConfig(I2C_TypeDef* I2Cx, uint16_t I2C_IT, FunctionalState NewState);


/* 
 ===============================================================================
                          I2C State Monitoring Functions
 ===============================================================================
  This I2C driver provides three different ways for I2C state monitoring
  depending on the application requirements and constraints:
         
   
     1. Basic state monitoring (Using I2C_CheckEvent() function)
     -----------------------------------------------------------
        It compares the status registers (SR1 and SR2) content to a given event
        (can be the combination of one or more flags).
        It returns SUCCESS if the current status includes the given flags 
        and returns ERROR if one or more flags are missing in the current status.

          - When to use
             - This function is suitable for most applications as well as for startup 
               activity since the events are fully described in the product reference 
               manual (RM0090).
             - It is also suitable for users who need to define their own events.

          - Limitations
             - If an error occurs (ie. error flags are set besides to the monitored 
               flags), the I2C_CheckEvent() function may return SUCCESS despite 
               the communication hold or corrupted real state. 
               In this case, it is advised to use error interrupts to monitor 
               the error events and handle them in the interrupt IRQ handler.
         
     Note 
         For error management, it is advised to use the following functions:
           - I2C_ITConfig() to configure and enable the error interrupts (I2C_IT_ERR).
           - I2Cx_ER_IRQHandler() which is called when the error interrupt occurs.
             Where x is the peripheral instance (I2C1, I2C2 ...)
           - I2C_GetFlagStatus() or I2C_GetITStatus()  to be called into the 
             I2Cx_ER_IRQHandler() function in order to determine which error occurred.
           - I2C_ClearFlag() or I2C_ClearITPendingBit() and/or I2C_SoftwareResetCmd() 
             and/or I2C_GenerateStop() in order to clear the error flag and source 
             and return to correct  communication status.
             
 
     2. Advanced state monitoring (Using the function I2C_GetLastEvent())
     -------------------------------------------------------------------- 
        Using the function I2C_GetLastEvent() which returns the image of both status 
        registers in a single word (uint32_t) (Status Register 2 value is shifted left 
        by 16 bits and concatenated to Status Register 1).

          - When to use
             - This function is suitable for the same applications above but it 
               allows to overcome the mentioned limitation of I2C_GetFlagStatus() 
               function.
             - The returned value could be compared to events already defined in 
               this file or to custom values defined by user.
               This function is suitable when multiple flags are monitored at the 
               same time.
             - At the opposite of I2C_CheckEvent() function, this function allows 
               user to choose when an event is accepted (when all events flags are 
               set and no other flags are set or just when the needed flags are set 
               like I2C_CheckEvent() function.

          - Limitations
             - User may need to define his own events.
             - Same remark concerning the error management is applicable for this 
               function if user decides to check only regular communication flags 
               (and ignores error flags).
      
 
     3. Flag-based state monitoring (Using the function I2C_GetFlagStatus())
     -----------------------------------------------------------------------
     
      Using the function I2C_GetFlagStatus() which simply returns the status of 
      one single flag (ie. I2C_FLAG_RXNE ...). 

          - When to use
             - This function could be used for specific applications or in debug 
               phase.
             - It is suitable when only one flag checking is needed (most I2C 
               events are monitored through multiple flags).
          - Limitations: 
             - When calling this function, the Status register is accessed. 
               Some flags are cleared when the status register is accessed. 
               So checking the status of one Flag, may clear other ones.
             - Function may need to be called twice or more in order to monitor 
               one single event.           
 */

/*
 ===============================================================================
                          1. Basic state monitoring
 ===============================================================================
 */
/* 状态监测函数组 ****************************************************/
/**
  * @brief 基础事件检查
  * @param I2C_EVENT 预定义事件标识（I2C_EVENT_xxx）
  * @retval ErrorStatus 事件状态（SUCCESS/ERROR）
  * @note 检查多个标志位的组合状态，适用于标准通信流程的轮询检查
  * @示例：I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)
  */
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);

/* 高级状态监测 ******************************************************/
/**
  * @brief 获取当前所有有效事件标志
  * @retval 32位事件状态字（包含SR1/SR2寄存器组合状态）
  * @note 返回值的低16位对应SR1，高16位对应SR2
  * @应用场景：需要全面分析总线状态时使用
  */
uint32_t I2C_GetLastEvent(I2C_TypeDef* I2Cx);

/* 标志位管理函数 ***************************************************/
/**
  * @brief 获取单个标志位状态
  * @param I2C_FLAG 标志位定义（I2C_FLAG_xxx）
  * @retval FlagStatus 标志位状态（SET/RESET）
  * @note 支持所有SR1/SR2寄存器标志位查询
  */
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);

/**
  * @brief 清除指定标志位
  * @param I2C_FLAG 可清除标志位（如STOPF/BTF等）
  * @warning 只能清除软件可写标志位
  */
void I2C_ClearFlag(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);

/**
  * @brief 获取中断状态
  * @param I2C_IT 中断类型（I2C_IT_xxx）
  * @retval ITStatus 中断状态（SET/RESET）
  * @note 检查中断使能位和标志位的联合状态
  */
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, uint32_t I2C_IT);

/**
  * @brief 清除中断挂起位
  * @param I2C_IT 中断类型
  * @note 需在中断服务例程结束前调用
  */
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, uint32_t I2C_IT);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_I2C_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

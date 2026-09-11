/**
  ******************************************************************************
  * @file   I2C.h
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Header File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#ifndef _I2C_H_
#define _I2C_H_

#include "system.h"
#include "sys.h"
#include "math.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
/* I2C硬件配置及基础操作宏定义 */

/*------------------- 硬件引脚配置 -------------------*/
#define SCL_ID 8             // SCL引脚在GPIOB中的位编号（PB8）
#define SDA_ID 9             // SDA引脚在GPIOB中的位编号（PB9）

#define SCL_PORT    GPIOB    // SCL端口（必须与SCL_ID对应）
#define SCL_PIN     GPIO_Pin_8 // SCL引脚号
#define SDA_PORT    GPIOB    // SDA端口（必须与SDA_ID对应）
#define SDA_PIN     GPIO_Pin_9 // SDA引脚号
#define SCL_GPIO_CLK RCC_AHB1Periph_GPIOB // 端口时钟使能（AHB1总线）

/*------------------- GPIO模式控制 -------------------*/
// SDA输入模式配置（MODER寄存器位操作）
#define SDA_IN()  {SDA_PORT->MODER &= ~(3<<(SDA_ID*2)); \
                   SDA_PORT->MODER |= 0<<SDA_ID*2;}  // 00:输入模式

// SDA输出模式配置（MODER寄存器位操作）                   
#define SDA_OUT() {SDA_PORT->MODER &= ~(3<<(SDA_ID*2)); \
                   SDA_PORT->MODER |= 1<<SDA_ID*2;}  // 01:通用输出模式

/*------------------- 信号线操作宏 -------------------*/
#define IIC_SCL    PBout(SCL_ID) // SCL线电平控制（0/1）
#define IIC_SDA    PBout(SDA_ID) // SDA线输出电平控制
#define READ_SDA   PBin(SDA_ID)  // SDA线输入状态读取

/*------------------- I2C传输方向定义 -------------------*/
#ifndef I2C_Direction_Transmitter
#define I2C_Direction_Transmitter ((uint8_t)0x00) // 主机发送模式
#endif

#ifndef I2C_Direction_Receiver
#define I2C_Direction_Receiver    ((uint8_t)0x01) // 主机接收模式
#endif

/*------------------- 应答状态枚举 -------------------*/
enum {
    I2C_ACK  = 0,   // 确认应答（ACK）
    I2C_NACK = 1     // 非确认应答（NACK）
};

/*--------------------- I2C 基础操作函数声明 ---------------------*/

/**
 * @brief 设置I2C SDA引脚模式（输入或输出）
 * @param Mode 模式选择（0-输入，1-输出）
 */
void I2C_SDAMode(uint8_t Mode);

/**
 * @brief 发送I2C启动信号
 */
void I2C_Start(void);

/**
 * @brief 发送I2C停止信号
 */
void I2C_Stop(void);

/**
 * @brief 等待I2C应答信号
 * @return 是否接收到应答信号（true-接收到ACK，false-接收到NACK）
 */
bool I2C_WaiteForAck(void);

/**
 * @brief 发送I2C确认应答（ACK）
 */
void I2C_Ack(void);

/**
 * @brief 发送I2C非确认应答（NACK）
 */
void I2C_NAck(void);

/*--------------------- I2C 数据写入函数声明 ---------------------*/

/**
 * @brief 写入单个位到指定设备寄存器
 * @param DevAddr 设备地址（7位）
 * @param RegAddr 寄存器地址（8位）
 * @param BitNum 位号（0-7）
 * @param Data 写入数据的位值（0或1）
 * @return 操作是否成功（true-成功，false-失败）
 */
bool I2C_WriteOneBit(uint8_t DevAddr, uint8_t RegAddr, uint8_t BitNum, uint8_t Data);

/**
 * @brief 写入多个连续位到指定设备寄存器
 * @param DevAddr 设备地址（7位）
 * @param RegAddr 寄存器地址（8位）
 * @param BitStart 起始位号（0-7）
 * @param Length 要写入的位数（1-8）
 * @param Data 写入数据的位值（高位对应BitStart）
 * @return 操作是否成功（true-成功，false-失败）
 */
bool I2C_WriteBits(uint8_t DevAddr, uint8_t RegAddr, uint8_t BitStart, uint8_t Length, uint8_t Data);

/**
 * @brief 写入单个字节到指定设备寄存器
 * @param Data 写入的数据（8位）
 */
void I2C_WriteByte(uint8_t Data);

/*--------------------- I2C 数据读取函数声明 ---------------------*/

/**
 * @brief 从I2C读取单个字节
 * @param Ack 应答标志（0-发送ACK，1-发送NACK）
 * @return 读取的数据（8位）
 */
uint8_t I2C_ReadByte(uint8_t Ack);

/**
 * @brief 写入单个字节到指定设备寄存器
 * @param DevAddr 设备地址（7位）
 * @param RegAddr 寄存器地址（8位）
 * @param Data 写入的数据（8位）
 * @return 操作是否成功（0-成功，非0-失败）
 */
u8 I2C_WriteOneByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t Data);

/**
 * @brief 从指定设备寄存器读取单个字节
 * @param DevAddr 设备地址（7位）
 * @param RegAddr 寄存器地址（8位）
 * @return 读取的数据（8位）
 */
uint8_t I2C_ReadOneByte(uint8_t DevAddr, uint8_t RegAddr);

/**
 * @brief 写入多个字节到指定设备寄存器
 * @param DevAddr 设备地址（7位）
 * @param RegAddr 寄存器地址（8位）
 * @param Num 要写入的字节数
 * @param pBuff 数据缓冲区指针
 * @return 操作是否成功（true-成功，false-失败）
 */
bool I2C_WriteBuff(uint8_t DevAddr, uint8_t RegAddr, uint8_t Num, uint8_t *pBuff);

/**
 * @brief 从指定设备寄存器读取多个字节
 * @param DevAddr 设备地址（7位）
 * @param RegAddr 寄存器地址（8位）
 * @param Num 要读取的字节数
 * @param pBuff 数据缓冲区指针
 * @return 操作是否成功（true-成功，false-失败）
 */
bool I2C_ReadBuff(uint8_t DevAddr, uint8_t RegAddr, uint8_t Num, uint8_t *pBuff);

/*--------------------- I2C GPIO初始化函数声明 ---------------------*/

/**
 * @brief 初始化I2C使用的GPIO引脚
 * @note 配置SCL和SDA引脚的时钟使能、模式、复用功能等
 */
void I2C_GPIOInit(void);


#endif


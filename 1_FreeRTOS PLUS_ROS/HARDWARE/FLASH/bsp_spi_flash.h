/**
  ******************************************************************************
  * @file    bsp_spi_flash.h
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Header File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f4xx.h"
#include <stdio.h>
/* SPI Flash驱动头文件 - 含W25Q系列芯片支持及硬件接口配置 */

/*--------------------- 存储芯片型号配置 ---------------------*/
// W25Q系列芯片ID定义（生产商ID + 设备ID组合）
// #define sFLASH_ID 0xEF3015  // W25X16芯片ID（旧型号）
// #define sFLASH_ID 0xEF4015  // W25Q16芯片ID（支持Quad SPI）
// #define sFLASH_ID 0xEF4017  // W25Q64芯片ID（64Mbit/8MB容量）
#define sFLASH_ID 0xEF4018      // W25Q128芯片ID（128Mbit/16MB容量，当前启用）

/*--------------------- 存储参数配置 ---------------------*/
#define SPI_FLASH_PageSize        256     // 页编程大小（字节），W25Q全系支持256B页编程
#define SPI_FLASH_PerWritePageSize 256     // 单次写入最大长度（不超过页大小）

/*--------------------- SPI指令集定义 ---------------------*/
/* W25Q系列标准SPI指令（兼容SPI Mode 0/3） */
#define W25X_WriteEnable         0x06    // 写使能指令（必须先发送才能执行编程操作）
#define W25X_WriteDisable        0x04    // 写禁止指令（保护状态）
#define W25X_ReadStatusReg       0x05    // 读状态寄存器（可轮询BUSY位）
#define W25X_WriteStatusReg      0x01    // 写状态寄存器（设置写保护位）
#define W25X_ReadData            0x03    // 标准读数据指令（单线模式）
#define W25X_FastReadData        0x0B    // 快速读指令（最高104MHz时钟）
#define W25X_FastReadDual        0x3B    // 双线快速读（需要芯片支持）
#define W25X_PageProgram         0x02    // 页编程指令（必须写使能后使用）
#define W25X_BlockErase          0xD8    // 块擦除（64KB块）
#define W25X_SectorErase         0x20    // 扇区擦除（4KB最小擦除单位）
#define W25X_ChipErase           0xC7    // 整片擦除指令（谨慎使用）
#define W25X_PowerDown           0xB9    // 进入低功耗模式（电流<5μA）
#define W25X_ReleasePowerDown   0xAB    // 唤醒芯片（需保持CS低电平）
#define W25X_DeviceID           0xAB    // 读设备ID（单线模式）
#define W25X_ManufactDeviceID   0x90    // 读厂商+设备ID（兼容旧型号）
#define W25X_JedecDeviceID       0x9F    // JEDEC标准ID读取（推荐使用）

/* 状态寄存器标志位 */
#define WIP_Flag 0x01  // 忙状态位（Bit0）：0-空闲，1-正在执行写/擦除操作
#define Dummy_Byte 0xFF // 空操作字节（用于快速读指令填充时钟）

/*--------------------- 硬件接口配置 ---------------------*/
/* SPI外设选择（配置为SPI1） */
#define FLASH_SPI                SPI1    // 使用SPI1外设
#define FLASH_SPI_CLK            RCC_APB2Periph_SPI1  // SPI1时钟源
#define FLASH_SPI_CLK_INIT       RCC_APB2PeriphClockCmd  // 时钟使能函数

/* SCK引脚配置（PG3复用为SPI1_SCK） */
#define FLASH_SPI_SCK_PIN        GPIO_Pin_3       // SCK引脚号
#define FLASH_SPI_SCK_GPIO_PORT  GPIOB            // 端口B
#define FLASH_SPI_SCK_GPIO_CLK   RCC_AHB1Periph_GPIOB  // 端口时钟
#define FLASH_SPI_SCK_PINSOURCE  GPIO_PinSource3  // 引脚源
#define FLASH_SPI_SCK_AF         GPIO_AF_SPI1     // 复用功能映射

/* MISO引脚配置（PG4复用为SPI1_MISO） */
#define FLASH_SPI_MISO_PIN       GPIO_Pin_4       // MISO引脚号
#define FLASH_SPI_MISO_GPIO_PORT GPIOB            // 端口B
#define FLASH_SPI_MISO_GPIO_CLK  RCC_AHB1Periph_GPIOB  
#define FLASH_SPI_MISO_PINSOURCE GPIO_PinSource4  
#define FLASH_SPI_MISO_AF        GPIO_AF_SPI1     

/* MOSI引脚配置（PG5复用为SPI1_MOSI） */
#define FLASH_SPI_MOSI_PIN       GPIO_Pin_5     
#define FLASH_SPI_MOSI_GPIO_PORT GPIOB          
#define FLASH_SPI_MOSI_GPIO_CLK  RCC_AHB1Periph_GPIOB
#define FLASH_SPI_MOSI_PINSOURCE GPIO_PinSource5
#define FLASH_SPI_MOSI_AF        GPIO_AF_SPI1    

/* 片选引脚配置（PG6作为GPIO控制） */
#define FLASH_CS_PIN             GPIO_Pin_6     // CS引脚号
#define FLASH_CS_GPIO_PORT       GPIOG          // 端口G
#define FLASH_CS_GPIO_CLK        RCC_AHB1Periph_GPIOG  // 端口时钟

/* 片选信号控制宏（低电平有效） */
#define SPI_FLASH_CS_LOW()  {FLASH_CS_GPIO_PORT->BSRRH = FLASH_CS_PIN;} // 选中芯片
#define SPI_FLASH_CS_HIGH() {FLASH_CS_GPIO_PORT->BSRRL = FLASH_CS_PIN;} // 释放芯片

/*--------------------- 调试及超时控制配置 ---------------------*/
/* SPI传输状态轮询超时周期（基于系统时钟计算） */
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)    // 基础超时周期（4096个时钟周期）
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT)) // 长超时（用于擦除/写入操作）

/* 调试信息输出配置（生产环境建议关闭DEBUG输出） */
#define FLASH_DEBUG_ON         1    // 调试开关：1-启用 0-关闭

/* 分级日志输出宏（适配RS232/RS485输出） */
#define FLASH_INFO(fmt,arg...)   printf("<<-FLASH-INFO->> "fmt"\n",##arg)  // 关键操作记录
#define FLASH_ERROR(fmt,arg...)  printf("<<-FLASH-ERROR->> "fmt"\n",##arg) // 错误事件记录

/* 增强型调试宏（带代码行号定位） */
#define FLASH_DEBUG(fmt,arg...)  do{                                      \
                                     if(FLASH_DEBUG_ON)                   \
                                         printf("<<-FLASH-DEBUG->> [%d]"  \
                                             fmt"\n",__LINE__, ##arg);    \
                                 }while(0)  // 安全封装宏，避免if-else悬挂问题



/*--------------------- SPI Flash 操作接口声明 ---------------------*/

/**
 * @brief 初始化SPI Flash控制器及GPIO
 * @note 配置内容：
 * - SPI时钟模式（Mode 0/3）
 * - 时钟分频系数（PCLK2/2）
 * - GPIO复用功能配置
 * - CS引脚初始状态（高电平）
 */
void SPI_FLASH_Init(void);

/**
 * @brief 擦除指定扇区（4KB）
 * @param SectorAddr 扇区地址（需4KB对齐，即低12位为0）
 * @warning 擦除时间约100-400ms，需调用WaitForWriteEnd等待完成
 */
void SPI_FLASH_SectorErase(u32 SectorAddr);

/**
 * @brief 整片擦除（谨慎使用）
 * @note 擦除时间长达15-30秒，期间禁止断电
 */
void SPI_FLASH_BulkErase(void);

/**
 * @brief 页编程写入（最大256字节）
 * @param pBuffer 源数据缓冲区指针
 * @param WriteAddr 写入地址（需256字节对齐）
 * @param NumByteToWrite 写入字节数（1-256）
 * @note 地址跨页时自动截断，建议使用BufferWrite处理多页写入
 */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);

/**
 * @brief 缓冲区写入（自动处理跨页）
 * @param pBuffer 源数据缓冲区指针
 * @param WriteAddr 起始写入地址（任意地址）
 * @param NumByteToWrite 写入字节数（最大16MB）
 * @note 内部自动拆分多页并处理地址对齐
 */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);

/**
 * @brief 读取数据到缓冲区
 * @param pBuffer 目标缓冲区指针
 * @param ReadAddr 读取起始地址
 * @param NumByteToRead 读取字节数
 * @note 支持全地址范围读取，无对齐限制
 */
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);

/**
 * @brief 读取JEDEC标准ID
 * @return 设备ID（格式：0x00EF4018）
 *         EFh-厂商ID（Winbond）
 *         4018-设备ID（W25Q128）
 */
u32 SPI_FLASH_ReadID(void);

/**
 * @brief 读取旧版设备ID（兼容W25X系列）
 * @return 设备ID（格式：0xEF15）
 */
u32 SPI_FLASH_ReadDeviceID(void);

/**
 * @brief 启动连续读取序列（提升读取效率）
 * @param ReadAddr 起始地址
 * @note 需配合ReadByte连续调用，适合大数据块读取
 */
void SPI_FLASH_StartReadSequence(u32 ReadAddr);

/**
 * @brief 进入深度掉电模式（功耗<1μA）
 * @note 唤醒需调用WAKEUP并延迟3us以上
 */
void SPI_Flash_PowerDown(void);

/**
 * @brief 从掉电模式唤醒
 */
void SPI_Flash_WAKEUP(void);

/*--------------------- 底层SPI通信接口 ---------------------*/
/**
 * @brief 接收单字节数据（标准SPI模式）
 * @return 接收到的字节数据
 */
u8 SPI_FLASH_ReadByte(void);

/**
 * @brief 发送单字节指令/数据
 * @param byte 待发送字节
 * @return 接收到的字节（全双工特性）
 */
u8 SPI_FLASH_SendByte(u8 byte);

/**
 * @brief 发送16位数据（用于快速读指令）
 * @param HalfWord 待发送的半字数据
 * @return 接收到的半字数据
 */
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);

/**
 * @brief 使能写操作（必要前置条件）
 * @note 在执行编程/擦除前必须调用
 */
void SPI_FLASH_WriteEnable(void);

/**
 * @brief 等待写操作完成
 * @note 通过轮询状态寄存器BUSY位实现
 *       典型等待时间：页写入3ms，扇区擦除400ms
 */
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_H */


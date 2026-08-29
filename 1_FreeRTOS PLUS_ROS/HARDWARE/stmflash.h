/**
? ******************************************************************************
? * @file ? stmflash.h
? * @author ?The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
? * @version V1.0.0
? * @date ? ?8-June-2025
? * @brief ? Header File
? ******************************************************************************
? * @attention
? * Copyright (c) 2025 The Daily Life of An Engineering Girl.
? * All rights reserved.
? *
? * *----------------------------------------------------------------------------*/
#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h" 
#include "system.h"


/* FLASH基础配置 -----------------------------------------------------------------
 * STM32_FLASH_SIZE：芯片实际Flash容量（单位KB）
 * STM32_FLASH_WREN：写使能开关（生产环境建议设为0）
 */
#define STM32_FLASH_SIZE     64      // STM32F407VG配置为64KB（实际为1MB，需修正为1024）
#define STM32_FLASH_WREN     1       // 写使能标志（0:禁止写入，1:允许写入）

/* FLASH物理地址定义 */
#define STM32_FLASH_BASE     0x08000000  // Flash起始地址（固定值）

/* Flash解锁密钥（参考STM32编程手册） */
#define FLASH_KEY1           0x45670123  // 解锁密钥1
#define FLASH_KEY2           0xCDEF89AB  // 解锁密钥2

/* 函数声明 --------------------------------------------------------------------*/
/* 安全控制接口 */
void STMFLASH_Unlock(void);        // 解除Flash写保护（必须配对使用Lock）
void STMFLASH_Lock(void);          // 重新启用写保护

/* 状态监测接口 */
u8 STMFLASH_GetStatus(void);       // 获取操作状态（返回FLASH_FLAG值）
u8 STMFLASH_WaitDone(u16 time);    // 等待操作完成（超时时间单位ms）

/* 存储介质操作 */
u8 STMFLASH_ErasePage(u32 paddr);  // 擦除指定扇区（需4KB对齐地址）
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat); // 写入半字数据（地址必须偶数对齐）

/* 数据存取接口 */
u16 STMFLASH_ReadHalfWord(u32 faddr);          // 读取半字数据
void STMFLASH_WriteLenByte(u32 WriteAddr, u32 DataToWrite, u16 Len); // 变长数据写入
u32 STMFLASH_ReadLenByte(u32 ReadAddr, u16 Len);                     // 变长数据读取

/* 批量数据传输 */
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);  // 写入数据块
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);      // 读取数据块

/* 调试测试接口 */
void Test_Write(u32 WriteAddr, u16 WriteData);  // 写入测试模式（慎用）

/* 应用层接口 */
void Flash_Read(void);    // 从Flash加载系统参数
void Flash_Write(void);   // 保存系统参数到Flash

#endif


















/**
  ******************************************************************************
  * @file   F4_flash.h
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
#ifndef __F4_FLASH_H__
#define __F4_FLASH_H__
#include "sys.h"
#include "system.h"


 

//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//扇区11起始地址,128 Kbytes  

/* Flash存储地址定义 ---------------------------------------------------------
 * STM32_FLASH_BASE   : 主存储区起始地址(0x08000000)
 * FLASH_SAVE_ADDR    : 参数存储区起始地址(Sector5起始地址)
 * FLASH_SECTOR_SIZE  : Sector5容量(128KB)
 * 注意事项：
 *     1. 修改存储地址时需确保不覆盖程序存储区
 *     2. 写入前必须执行扇区擦除操作
 *----------------------------------------------------------------------------*/
#define STM32_FLASH_BASE ADDR_FLASH_SECTOR_0     // Flash起始地址(0x08000000)
#define FLASH_SAVE_ADDR  ADDR_FLASH_SECTOR_5     // 参数存储区(Sector5起始地址)
#define FLASH_Sector FLASH_Sector_5              // 使用Sector5作为存储扇区
#define FLASH_SECTOR_SIZE       0x20000          // Sector5容量128KB
#define Reserved_Address_Number 4                // 地址偏移保留量

/* 参数存储结构体 -------------------------------------------------------------
 * 功能：组织需要持久化存储的控制参数
 * 存储策略：
 *     1. flash_address_mark作为数据有效性标识(建议使用魔术数字)
 *     2. 舵机角度采用uint16存储，实际角度值=数值/10 (精度0.1度)
 *     3. 结构体总大小需保持4字节对齐
 * 扩展建议：
 *     可增加CRC校验字段确保数据完整性
 *----------------------------------------------------------------------------*/
typedef struct
{
    uint32_t flash_address_mark;  // 数据有效性标识(如0xAA55DD77)
    uint16_t Flash_Moveit_Angle1_init;  // 舵机1初始角度(单位：0.1度)
    uint16_t Flash_Moveit_Angle2_init;  // 舵机2初始角度
    uint16_t Flash_Moveit_Angle3_init;  // 舵机3初始角度 
    uint16_t Flash_Moveit_Angle4_init;  // 舵机4初始角度
    // 预留给未来扩展的字段
} Flash_Parameter;

/* Flash操作API --------------------------------------------------------------*/
uint8_t Write_ST_Flash_address_mark(uint32_t address, uint8_t *ptr, uint32_t nbyte); // 带地址标记的写入(返回0成功)
void Read_ST_Flash32(uint32_t addr, uint32_t *ptr, uint16_t nword);  // 32位数据块读取(需4字节对齐)

/* 底层Flash驱动函数 */
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u32 NumToWrite);  // 半字写入(地址必须偶数对齐)
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u32 NumToRead);     // 半字读取(支持任意地址)

/* 参数存取接口 */
void Flash_Read(void);                               // 从Flash加载所有参数到内存
void Flash_Write(uint32_t begin_adress, uint32_t nbyte); // 将内存参数写入Flash(需先擦除)

/* 运动控制参数声明 -----------------------------------------------------------
 * 存储规范：
 *     1. 浮点参数使用IEEE754单精度格式存储
 *     2. 整型参数使用补码格式存储
 *     3. 所有参数应在系统初始化时从Flash加载
 *----------------------------------------------------------------------------*/
extern short Moveit_Angle1_init,Moveit_Angle2_init,Moveit_Angle3_init,Moveit_Angle4_init; // 内存中的舵机初始角度
extern float Position1,Position2,Position3,Position4;    // 目标位置(单位：度)
extern float Velocity1,Velocity2,Velocity3,Velocity4;    // 运动速度(单位：度/秒)
extern float Position_KP,Position_KI,Position_KD;        // PID控制参数
extern int Servo_init_angle_adjust;                       // 零点校准标志(0:正常 1:需要校准)

#endif








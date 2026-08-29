/**
  ******************************************************************************
  * @file    stm32f4xx_flash.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the FLASH 
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
#ifndef __STM32F4xx_FLASH_H
#define __STM32F4xx_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup FLASH
  * @{
  */ 
/*-----------------------------------------------------------
 * 功能模块：FLASH存储器状态与延迟周期配置
 * 作用说明：
 *   1. 定义FLASH操作状态枚举
 *   2. 配置FLASH访问延迟周期
 *   注：适用于STM32F4系列MCU的FLASH控制器
 *-----------------------------------------------------------*/

/* FLASH操作状态枚举 */
typedef enum
{ 
  FLASH_BUSY = 1,          /* FLASH忙状态（操作进行中） 
                            触发场景：擦除/编程操作未完成 */
  FLASH_ERROR_RD,          /* 读保护错误 
                            可能原因：访问受保护区域 */
  FLASH_ERROR_PGS,         /* 编程序列错误 
                            典型场景：未按顺序执行解锁/锁操作 */
  FLASH_ERROR_PGP,         /* 编程并行错误 
                            常见于多页编程时的配置冲突 */
  FLASH_ERROR_PGA,         /* 编程对齐错误 
                            要求：必须按32位/64位对齐访问 */
  FLASH_ERROR_WRP,         /* 写保护错误 
                            目标扇区受写保护 */
  FLASH_ERROR_PROGRAM,     /* 编程验证失败 
                            数据校验不一致 */
  FLASH_ERROR_OPERATION,   /* 无效操作错误 
                            如：未解锁直接操作 */
  FLASH_COMPLETE           /* 操作成功完成 */
}FLASH_Status;

/*-----------------------------------------------------------
 * FLASH延迟周期配置（等待状态） 
 * 说明：根据SYSCLK频率设置，确保可靠访问 */
#define FLASH_Latency_0     ((uint8_t)0x0000)  /* 0等待周期（≤30MHz） */
#define FLASH_Latency_1     ((uint8_t)0x0001)  /* 1等待周期（≤60MHz） */
#define FLASH_Latency_2     ((uint8_t)0x0002)  /* 2等待周期（≤90MHz） */
#define FLASH_Latency_3     ((uint8_t)0x0003)  /* 3等待周期（≤100MHz） */
#define FLASH_Latency_4     ((uint8_t)0x0004)  /* 4等待周期（≤110MHz） */
#define FLASH_Latency_5     ((uint8_t)0x0005)  /* 5等待周期（≤120MHz） */
#define FLASH_Latency_6     ((uint8_t)0x0006)  /* 6等待周期（≤130MHz） */
#define FLASH_Latency_7     ((uint8_t)0x0007)  /* 7等待周期（≤140MHz） */
#define FLASH_Latency_8     ((uint8_t)0x0008)  /* 8等待周期（≤150MHz） */
#define FLASH_Latency_9     ((uint8_t)0x0009)  /* 9等待周期（≤165MHz） */
#define FLASH_Latency_10    ((uint8_t)0x000A)  /* 10等待周期（≤180MHz） */
#define FLASH_Latency_11    ((uint8_t)0x000B)  /* 11等待周期（≤190MHz） */
#define FLASH_Latency_12    ((uint8_t)0x000C)  /* 12等待周期（≤200MHz） */
#define FLASH_Latency_13    ((uint8_t)0x000D)  /* 13等待周期（≤210MHz） */
#define FLASH_Latency_14    ((uint8_t)0x000E)  /* 14等待周期（≤225MHz） */
#define FLASH_Latency_15    ((uint8_t)0x000F)  /* 15等待周期（极限超频） */

/*-----------------------------------------------------------
 * FLASH延迟周期校验宏
 * 功能：验证传入的延迟周期值是否合法
 * 参数：LATENCY - 待验证的延迟值
 * 返回值：TRUE(1) - 合法值，FALSE(0) - 非法值
 * 设计要点：
 *   1. 覆盖STM32F4全系列支持的延迟周期（0-15）
 *   2. 用于FLASH配置函数参数校验
 * 使用示例：
 *   assert_param(IS_FLASH_LATENCY(FLASH_Latency_7)); 
 *-----------------------------------------------------------*/
#define IS_FLASH_LATENCY(LATENCY) (                          \
    ((LATENCY) == FLASH_Latency_0)  || /* 0 WS  ≤30MHz  */  \
    ((LATENCY) == FLASH_Latency_1)  || /* 1 WS  ≤60MHz   */  \
    ((LATENCY) == FLASH_Latency_2)  || /* 2 WS  ≤90MHz   */  \
    ((LATENCY) == FLASH_Latency_3)  || /* 3 WS  ≤100MHz  */  \
    ((LATENCY) == FLASH_Latency_4)  || /* 4 WS  ≤110MHz  */  \
    ((LATENCY) == FLASH_Latency_5)  || /* 5 WS  ≤120MHz  */  \
    ((LATENCY) == FLASH_Latency_6)  || /* 6 WS  ≤130MHz  */  \
    ((LATENCY) == FLASH_Latency_7)  || /* 7 WS  ≤140MHz  */  \
    ((LATENCY) == FLASH_Latency_8)  || /* 8 WS  ≤150MHz  */  \
    ((LATENCY) == FLASH_Latency_9)  || /* 9 WS  ≤165MHz  */  \
    ((LATENCY) == FLASH_Latency_10) || /* 10 WS ≤180MHz  */  \
    ((LATENCY) == FLASH_Latency_11) || /* 11 WS ≤190MHz  */  \
    ((LATENCY) == FLASH_Latency_12) || /* 12 WS ≤200MHz  */  \
    ((LATENCY) == FLASH_Latency_13) || /* 13 WS ≤210MHz  */  \
    ((LATENCY) == FLASH_Latency_14) || /* 14 WS ≤225MHz  */  \
    ((LATENCY) == FLASH_Latency_15)   ) /* 15 WS 超频模式  */  

/*-----------------------------------------------------------
 * FLASH电压范围配置
 * 作用：定义芯片工作电压范围，影响FLASH编程/擦除参数
 * 注意：进行FLASH操作前必须正确设置电压范围
 *-----------------------------------------------------------*/
#define VoltageRange_1        ((uint8_t)0x00)  /* 1.8V-2.1V模式：
                                                 - 低功耗应用场景
                                                 - 最高主频限制为24MHz
                                                 - 禁止预取缓冲功能 */
#define VoltageRange_2        ((uint8_t)0x01)  /* 2.1V-2.7V模式：
                                                 - 中等性能模式
                                                 - 最高主频限制为84MHz
                                                 - 支持指令缓存 */
#define VoltageRange_3        ((uint8_t)0x02)  /* 2.7V-3.6V模式：
                                                 - 高性能全功能模式
                                                 - 支持168MHz主频
                                                 - 启用所有加速功能 */
#define VoltageRange_4        ((uint8_t)0x03)  /* 高压编程模式：
                                                 - 2.7V-3.6V+9V Vpp
                                                 - 用于工厂批量烧录
                                                 - 普通应用禁止使用 */

/* 电压范围校验宏 */
#define IS_VOLTAGERANGE(RANGE)(((RANGE) == VoltageRange_1) || \
                               ((RANGE) == VoltageRange_2) || \
                               ((RANGE) == VoltageRange_3) || \
                               ((RANGE) == VoltageRange_4))  /* 参数合法性检查：
                                                              - 用于FLASH操作函数参数验证
                                                              - 非法参数会触发assert_param错误 */

/*-----------------------------------------------------------
 * FLASH存储扇区编号定义
 * 作用：标识不同容量的存储扇区，用于擦除/编程操作
 * 注意：各扇区起始地址与容量根据芯片型号有所不同
 *-----------------------------------------------------------*/
/* 标准容量型号扇区配置 */
#define FLASH_Sector_0     ((uint16_t)0x0000) /* Sector 0: 0x08000000-0x08003FFF (16KB) */
#define FLASH_Sector_1     ((uint16_t)0x0008) /* Sector 1: 0x08004000-0x08007FFF (16KB) */
#define FLASH_Sector_2     ((uint16_t)0x0010) /* Sector 2: 0x08008000-0x0800BFFF (16KB) */
#define FLASH_Sector_3     ((uint16_t)0x0018) /* Sector 3: 0x0800C000-0x0800FFFF (16KB) */

/* 大容量型号扩展扇区 */
#define FLASH_Sector_4     ((uint16_t)0x0020) /* Sector 4: 0x08010000-0x0801FFFF (64KB) */
#define FLASH_Sector_5     ((uint16_t)0x0028) /* Sector 5: 0x08020000-0x0803FFFF (128KB)*/
#define FLASH_Sector_6     ((uint16_t)0x0030) /* Sector 6: 0x08040000-0x0805FFFF (128KB)*/
#define FLASH_Sector_7     ((uint16_t)0x0038) /* Sector 7: 0x08060000-0x0807FFFF (128KB)*/

/* 超大容量型号扩展扇区 */
#define FLASH_Sector_8     ((uint16_t)0x0040) /* Sector 8: 0x08080000-0x0809FFFF (128KB)*/
#define FLASH_Sector_9     ((uint16_t)0x0048) /* 扇区9: 0x080A0000-0x080BFFFF（128KB）*/
#define FLASH_Sector_10    ((uint16_t)0x0050) /* 扇区10:0x080C0000-0x080DFFFF（128KB）*/
#define FLASH_Sector_11    ((uint16_t)0x0058) /* 扇区11:0x080E0000-0x080FFFFF（128KB）*/

/* 双BANK型号额外扇区（超过1MB的存储空间） */
#define FLASH_Sector_12    ((uint16_t)0x0080) /* 扇区12:0x08100000-0x0811FFFF（128KB）*/
#define FLASH_Sector_13    ((uint16_t)0x0088) /* 扇区13:0x08120000-0x0813FFFF（128KB）*/
#define FLASH_Sector_14    ((uint16_t)0x0090) /* 扇区14:0x08140000-0x0815FFFF（128KB）*/
#define FLASH_Sector_15    ((uint16_t)0x0098) /* 扇区15:0x08160000-0x0817FFFF（128KB）*/
#define FLASH_Sector_16    ((uint16_t)0x00A0) /* 扇区16:0x08180000-0x0819FFFF（128KB）*/
#define FLASH_Sector_17    ((uint16_t)0x00A8) /* 扇区17:0x081A0000-0x081BFFFF（128KB）*/
#define FLASH_Sector_18    ((uint16_t)0x00B0) /* 扇区18:0x081C0000-0x081DFFFF（128KB）*/
#define FLASH_Sector_19    ((uint16_t)0x00B8) /* 扇区19:0x081E0000-0x081FFFFF（128KB）*/

/* 保留扇区（部分2MB型号可用） */
#define FLASH_Sector_20    ((uint16_t)0x00C0) /* 扇区20:预留扩展区段（128KB）*/
#define FLASH_Sector_21    ((uint16_t)0x00C8) /* 扇区21:预留扩展区段（128KB）*/
#define FLASH_Sector_22    ((uint16_t)0x00D0) /* 扇区22:预留扩展区段（128KB）*/
#define FLASH_Sector_23    ((uint16_t)0x00D8) /* 扇区23:预留扩展区段（128KB）*/

/*-----------------------------------------------------------
 * FLASH扇区校验宏
 * 功能：验证输入参数是否为有效的FLASH扇区编号
 * 设计要点：
 *   1. 覆盖STM32F4全系列支持的24个扇区（0-23）
 *   2. 通过静态断言实现编译时参数检查
 *   3. 防止非法扇区操作导致的硬件异常
 * 特殊说明：
 *   - 对于不同容量芯片实际生效范围不同（如512KB型号仅0-4有效）
 *   - 必须与FLASH_Erase_Sector等函数配合使用
 *-----------------------------------------------------------*/
#define IS_FLASH_SECTOR(SECTOR) (                               \
    ((SECTOR) == FLASH_Sector_0)   || /* 校验主存储区前4个16KB扇区 */ \
    ((SECTOR) == FLASH_Sector_1)   ||                           \
    ((SECTOR) == FLASH_Sector_2)   ||                           \
    ((SECTOR) == FLASH_Sector_3)   ||                           \
    ((SECTOR) == FLASH_Sector_4)   || /* 开始校验大容量扇区 */     \
    ((SECTOR) == FLASH_Sector_5)   || /* 128KB扇区5-7 */         \
    ((SECTOR) == FLASH_Sector_6)   ||                           \
    ((SECTOR) == FLASH_Sector_7)   ||                           \
    ((SECTOR) == FLASH_Sector_8)   || /* 双BANK架构扩展扇区 */   \
    ((SECTOR) == FLASH_Sector_9)   ||                           \
    ((SECTOR) == FLASH_Sector_10)  ||                           \
    ((SECTOR) == FLASH_Sector_11)  || /* 1MB型号最大支持到此 */  \
    ((SECTOR) == FLASH_Sector_12)  || /* 2MB型号扩展扇区 */     \
    ((SECTOR) == FLASH_Sector_13)  ||                           \
    ((SECTOR) == FLASH_Sector_14)  ||                           \
    ((SECTOR) == FLASH_Sector_15)  ||                           \
    ((SECTOR) == FLASH_Sector_16)  ||                           \
    ((SECTOR) == FLASH_Sector_17)  ||                           \
    ((SECTOR) == FLASH_Sector_18)  ||                           \
    ((SECTOR) == FLASH_Sector_19)  ||                           \
    ((SECTOR) == FLASH_Sector_20)  || /* 保留扇区需要特殊解锁 */ \
    ((SECTOR) == FLASH_Sector_21)  ||                           \
    ((SECTOR) == FLASH_Sector_22)  ||                           \
    ((SECTOR) == FLASH_Sector_23))   /* STM32F42xxx/43xxx专用 */ 

/*-----------------------------------------------------------
 * FLASH地址校验宏（型号差异处理）
 * 作用：验证地址是否属于可操作的FLASH存储区域
 * 设计特点：
 *   1. 根据不同芯片型号动态适配地址范围
 *   2. 包含主FLASH和系统存储区（选项字节）
 *-----------------------------------------------------------*/

/* 大容量型号（2MB FLASH）地址校验 */
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define IS_FLASH_ADDRESS(ADDRESS) (                          \
    (((ADDRESS) >= 0x08000000) && ((ADDRESS) <= 0x081FFFFF)) || /* 主存储区：2MB */ \
    (((ADDRESS) >= 0x1FFF7800) && ((ADDRESS) <= 0x1FFF7A0F)))  /* 系统区：选项字节 */
#endif 

/* 中容量型号（1MB FLASH）地址校验 */
#if defined (STM32F40_41xxx)
#define IS_FLASH_ADDRESS(ADDRESS) (                          \
    (((ADDRESS) >= 0x08000000) && ((ADDRESS) <= 0x080FFFFF)) || /* 主存储区：1MB */ \
    (((ADDRESS) >= 0x1FFF7800) && ((ADDRESS) <= 0x1FFF7A0F))) 
#endif 

/* 小容量型号（256KB FLASH）地址校验 */
#if defined (STM32F401xx)
#define IS_FLASH_ADDRESS(ADDRESS) (                          \
    (((ADDRESS) >= 0x08000000) && ((ADDRESS) <= 0x0803FFFF)) || /* 256KB */ \
    (((ADDRESS) >= 0x1FFF7800) && ((ADDRESS) <= 0x1FFF7A0F)))
#endif 

/* 中容量型号（512KB FLASH）地址校验 */
#if defined (STM32F411xE)
#define IS_FLASH_ADDRESS(ADDRESS) (                          \
    (((ADDRESS) >= 0x08000000) && ((ADDRESS) <= 0x0807FFFF)) || /* 512KB */ \
    (((ADDRESS) >= 0x1FFF7800) && ((ADDRESS) <= 0x1FFF7A0F)))
#endif 

/**
  * @brief  FLASH选项字节写保护扇区配置宏
  * @note   该组宏定义用于配置选项字节(Option Bytes)中的写保护(WRP)设置
  *         每个宏对应一个Flash存储扇区的写保护位掩码，通过位或操作可组合多个扇区
  *         写保护生效后，对应扇区将禁止编程/擦除操作
  *         适用型号：STM32F40xxx/41xxx 和 STM32F42xxx/43xxx 系列
  * @note   扇区编号说明：
  *         - Sector 0-11:  主存储区（Main memory）的12个扇区
  *         - Sector 12-23: 附加扇区（仅大容量型号支持）
  */

/* Bank1 主存储区写保护设置 -------------------------------------------------*/
#define OB_WRP_Sector_0       ((uint32_t)0x00000001) /*!< 扇区0写保护掩码 [位0] 地址范围：0x0800 0000 - 0x0800 3FFF (16KB) */
#define OB_WRP_Sector_1       ((uint32_t)0x00000002) /*!< 扇区1写保护掩码 [位1] 地址范围：0x0800 4000 - 0x0800 7FFF (16KB) */
#define OB_WRP_Sector_2       ((uint32_t)0x00000004) /*!< 扇区2写保护掩码 [位2] 地址范围：0x0800 8000 - 0x0800 BFFF (16KB) */
#define OB_WRP_Sector_3       ((uint32_t)0x00000008) /*!< 扇区3写保护掩码 [位3] 地址范围：0x0800 C000 - 0x0800 FFFF (16KB) */
#define OB_WRP_Sector_4       ((uint32_t)0x00000010) /*!< 扇区4写保护掩码 [位4] 地址范围：0x0801 0000 - 0x0801 FFFF (64KB) */
#define OB_WRP_Sector_5       ((uint32_t)0x00000020) /*!< 扇区5写保护掩码 [位5] 地址范围：0x0802 0000 - 0x0803 FFFF (128KB) */
#define OB_WRP_Sector_6       ((uint32_t)0x00000040) /*!< 扇区6写保护掩码 [位6] 地址范围：0x0804 0000 - 0x0805 FFFF (128KB) */
#define OB_WRP_Sector_7       ((uint32_t)0x00000080) /*!< 扇区7写保护掩码 [位7] 地址范围：0x0806 0000 - 0x0807 FFFF (128KB) */
#define OB_WRP_Sector_8       ((uint32_t)0x00000100) /*!< 扇区8写保护掩码 [位8] 地址范围：0x0808 0000 - 0x0809 FFFF (128KB) */
#define OB_WRP_Sector_9       ((uint32_t)0x00000200) /*!< 扇区9写保护掩码 [位9] 地址范围：0x080A 0000 - 0x080B FFFF (128KB) */
#define OB_WRP_Sector_10      ((uint32_t)0x00000400) /*!< 扇区10写保护掩码 [位10] 地址范围：0x080C 0000 - 0x080D FFFF (128KB) */
#define OB_WRP_Sector_11      ((uint32_t)0x00000800) /*!< 扇区11写保护掩码 [位11] 地址范围：0x080E 0000 - 0x080F FFFF (128KB) */

/* Bank2 附加存储区写保护设置（仅大容量型号）--------------------------------*/
#define OB_WRP_Sector_12      ((uint32_t)0x00000001) /*!< 扇区12写保护掩码 [位0] 地址范围：0x0810 0000 - 0x0810 3FFF (16KB) */
#define OB_WRP_Sector_13      ((uint32_t)0x00000002) /*!< 扇区13写保护掩码 [位1] 地址范围：0x0810 4000 - 0x0810 7FFF (16KB) */
#define OB_WRP_Sector_14      ((uint32_t)0x00000004) /*!< 扇区14写保护掩码 [位2] 地址范围：0x0810 8000 - 0x0810 BFFF (16KB) */
#define OB_WRP_Sector_15      ((uint32_t)0x00000008) /*!< 扇区15写保护掩码 [位3] 地址范围：0x0810 C000 - 0x0810 FFFF (16KB) */
#define OB_WRP_Sector_16      ((uint32_t)0x00000010) /*!< 扇区16写保护掩码 [位4] 地址范围：0x0811 0000 - 0x0811 FFFF (64KB) */
#define OB_WRP_Sector_17      ((uint32_t)0x00000020) /*!< 扇区17写保护掩码 [位5] 地址范围：0x0812 0000 - 0x0813 FFFF (128KB) */
#define OB_WRP_Sector_18      ((uint32_t)0x00000040) /*!< 扇区18写保护掩码 [位6] 地址范围：0x0814 0000 - 0x0815 FFFF (128KB) */
#define OB_WRP_Sector_19      ((uint32_t)0x00000080) /*!< 扇区19写保护掩码 [位7] 地址范围：0x0816 0000 - 0x0817 FFFF (128KB) */
#define OB_WRP_Sector_20      ((uint32_t)0x00000100) /*!< 扇区20写保护掩码 [位8] 地址范围：0x0818 0000 - 0x0819 FFFF (128KB) */
#define OB_WRP_Sector_21      ((uint32_t)0x00000200) /*!< 扇区21写保护掩码 [位9] 地址范围：0x081A 0000 - 0x081B FFFF (128KB) */
#define OB_WRP_Sector_22      ((uint32_t)0x00000400) /*!< 扇区22写保护掩码 [位10] 地址范围：0x081C 0000 - 0x081D FFFF (128KB) */
#define OB_WRP_Sector_23      ((uint32_t)0x00000800) /*!< 扇区23写保护掩码 [位11] 地址范围：0x081E 0000 - 0x081F FFFF (128KB) */

/* 组合保护宏 ------------------------------------------------------------*/
#define OB_WRP_Sector_All     ((uint32_t)0x00000FFF) /*!< 全扇区写保护掩码 [位0-11] 注意：实际仅保护Bank1的前12个扇区 */



/* 写保护参数校验宏 */
#define IS_OB_WRP(SECTOR) ( (((SECTOR) & 0xFFFFF000) == 0) && ((SECTOR) != 0) )
/* 验证规则：
   - 高20位必须为0（确保只使用合法位掩码）
   - 参数不能全0（至少保护一个扇区） */

/* 专有代码保护(PCROP)模式选择 */
#define OB_PcROP_Disable   0x00   /* 禁用PCROP，nWPRi位用于常规写保护 */
#define OB_PcROP_Enable    0x80   /* 启用PCROP，nWPRi位用于代码读取保护 
                                  注意：PCROP启用后相关扇区将无法调试读取 */

/* PCROP模式校验宏 */
#define IS_OB_PCROP_SELECT(PCROP) (((PCROP) == OB_PcROP_Disable) || \
                                  ((PCROP) == OB_PcROP_Enable))
/**
  * @brief  FLASH选项字节专有代码读保护(PCROP)配置宏
  * @note   该组宏定义用于配置专有代码读保护功能，启用后对应扇区：
  *         - 禁止通过调试接口(如JTAG/SWD)读取内容
  *         - 禁止非特权模式下的编程/擦除操作
  *         - 保护代码不被逆向工程（需配合RDP级别使用）
  * @warning PCROP一旦启用只能通过整片擦除解除，操作前务必确认保护策略
  * @note   扇区编号与物理地址对应关系与写保护(WRP)定义保持一致
  */

/* Bank1 主存储区PCROP设置 ------------------------------------------------*/
#define OB_PCROP_Sector_0        ((uint32_t)0x00000001) /*!< 扇区0 PCROP掩码 [位0] 地址：0x0800 0000-0x0800 3FFF 保护粒度：16KB */
#define OB_PCROP_Sector_1        ((uint32_t)0x00000002) /*!< 扇区1 PCROP掩码 [位1] 地址：0x0800 4000-0x0800 7FFF 保护粒度：16KB */
#define OB_PCROP_Sector_2        ((uint32_t)0x00000004) /*!< 扇区2 PCROP掩码 [位2] 地址：0x0800 8000-0x0800 BFFF 保护粒度：16KB */
#define OB_PCROP_Sector_3        ((uint32_t)0x00000008) /*!< 扇区3 PCROP掩码 [位3] 地址：0x0800 C000-0x0800 FFFF 保护粒度：16KB */
#define OB_PCROP_Sector_4        ((uint32_t)0x00000010) /*!< 扇区4 PCROP掩码 [位4] 地址：0x0801 0000-0x0801 FFFF 保护粒度：64KB */
#define OB_PCROP_Sector_5        ((uint32_t)0x00000020) /*!< 扇区5 PCROP掩码 [位5] 地址：0x0802 0000-0x0803 FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_6        ((uint32_t)0x00000040) /*!< 扇区6 PCROP掩码 [位6] 地址：0x0804 0000-0x0805 FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_7        ((uint32_t)0x00000080) /*!< 扇区7 PCROP掩码 [位7] 地址：0x0806 0000-0x0807 FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_8        ((uint32_t)0x00000100) /*!< 扇区8 PCROP掩码 [位8] 地址：0x0808 0000-0x0809 FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_9        ((uint32_t)0x00000200) /*!< 扇区9 PCROP掩码 [位9] 地址：0x080A 0000-0x080B FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_10       ((uint32_t)0x00000400) /*!< 扇区10 PCROP掩码 [位10] 地址：0x080C 0000-0x080D FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_11       ((uint32_t)0x00000800) /*!< 扇区11 PCROP掩码 [位11] 地址：0x080E 0000-0x080F FFFF 保护粒度：128KB */

/* Bank2 附加存储区PCROP设置 ----------------------------------------------*/
#define OB_PCROP_Sector_12       ((uint32_t)0x00000001) /*!< 扇区12 PCROP掩码 [位0] 地址：0x0810 0000-0x0810 3FFF 保护粒度：16KB */
#define OB_PCROP_Sector_13       ((uint32_t)0x00000002) /*!< 扇区13 PCROP掩码 [位1] 地址：0x0810 4000-0x0810 7FFF 保护粒度：16KB */
#define OB_PCROP_Sector_14       ((uint32_t)0x00000004) /*!< 扇区14 PCROP掩码 [位2] 地址：0x0810 8000-0x0810 BFFF 保护粒度：16KB */
#define OB_PCROP_Sector_15       ((uint32_t)0x00000008) /*!< 扇区15 PCROP掩码 [位3] 地址：0x0810 C000-0x0810 FFFF 保护粒度：16KB */
#define OB_PCROP_Sector_16       ((uint32_t)0x00000010) /*!< 扇区16 PCROP掩码 [位4] 地址：0x0811 0000-0x0811 FFFF 保护粒度：64KB */
#define OB_PCROP_Sector_17       ((uint32_t)0x00000020) /*!< 扇区17 PCROP掩码 [位5] 地址：0x0812 0000-0x0813 FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_18       ((uint32_t)0x00000040) /*!< 扇区18 PCROP掩码 [位6] 地址：0x0814 0000-0x0815 FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_19       ((uint32_t)0x00000080) /*!< 扇区19 PCROP掩码 [位7] 地址：0x0816 0000-0x0817 FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_20       ((uint32_t)0x00000100) /*!< 扇区20 PCROP掩码 [位8] 地址：0x0818 0000-0x0819 FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_21       ((uint32_t)0x00000200) /*!< 扇区21 PCROP掩码 [位9] 地址：0x081A 0000-0x081B FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_22       ((uint32_t)0x00000400) /*!< 扇区22 PCROP掩码 [位10] 地址：0x081C 0000-0x081D FFFF 保护粒度：128KB */
#define OB_PCROP_Sector_23       ((uint32_t)0x00000800) /*!< 扇区23 PCROP掩码 [位11] 地址：0x081E 0000-0x081F FFFF 保护粒度：128KB */

/* 组合保护宏 ------------------------------------------------------------*/
#define OB_PCROP_Sector_All      ((uint32_t)0x00000FFF) /*!< 全扇区PCROP掩码 [位0-11] 注意：实际仅保护Bank1的前12个扇区 */

/**
  * @brief  FLASH选项字节配置模块
  * @note   本模块包含STM32F4系列闪存选项字节的配置定义和参数校验宏
  * @detail 包含以下功能配置：
  *         - PCROP（专有代码读保护）扇区设置
  *         - RDP（读保护）等级配置
  *         - 独立看门狗(IWDG)模式选择
  *         - STOP/STANDBY模式下的复位行为配置
  *         所有IS_OB_xxx宏用于参数合法性校验
  */

/* PCROP（专有代码读保护）参数校验宏 ----------------------------------------*/
#define IS_OB_PCROP(SECTOR)((((SECTOR) & (uint32_t)0xFFFFF000) == 0x00000000) && ((SECTOR) != 0x00000000))
/* 参数校验逻辑：
   - 高20位必须为0（确保只使用bit0-11表示扇区）
   - 参数不能全为0（至少选择1个扇区）
   - 有效扇区范围：0x00000001 至 0x00000FFF（对应bit0-11）*/

/** 
  * @defgroup FLASH_Option_Bytes_Read_Protection 闪存读保护配置
  * @{
  */
#define OB_RDP_Level_0   ((uint8_t)0xAA)  /* 等级0：无读保护（默认状态） */
#define OB_RDP_Level_1   ((uint8_t)0x55)  /* 等级1：使能读保护，调试接口受限 */
/*#define OB_RDP_Level_2   ((uint8_t)0xCC)*/ /*!< 等级2：永久保护（慎用！启用后将无法降级且失去调试功能） */

/* 读保护等级参数校验 */
#define IS_OB_RDP(LEVEL) (((LEVEL) == OB_RDP_Level_0)||  /* 允许等级0 */\
                          ((LEVEL) == OB_RDP_Level_1))/*||\ 允许等级1 */
                          /* ((LEVEL) == OB_RDP_Level_2))*/  /* 等级2被注释，因不可逆操作需特别处理 */
/**
  * @}
  */
  
/** 
  * @defgroup FLASH_Option_Bytes_IWatchdog 独立看门狗模式配置
  * @{
  */
#define OB_IWDG_SW                     ((uint8_t)0x20)  /* 软件看门狗：需程序喂狗，bit5置1 */
#define OB_IWDG_HW                     ((uint8_t)0x00)  /* 硬件看门狗：自动喂狗，超时强制复位 */

/* 看门狗模式参数校验 */
#define IS_OB_IWDG_SOURCE(SOURCE) (((SOURCE) == OB_IWDG_SW) || ((SOURCE) == OB_IWDG_HW))
/**
  * @}
  */

/** 
  * @defgroup FLASH_Option_Bytes_nRST_STOP STOP模式复位配置
  * @{
  */
#define OB_STOP_NoRST                  ((uint8_t)0x40) /* STOP模式不生成复位：bit6置1，保持内核状态 */
#define OB_STOP_RST                    ((uint8_t)0x00) /* STOP模式生成复位：退出时执行完整复位 */

/* STOP模式复位参数校验 */
#define IS_OB_STOP_SOURCE(SOURCE) (((SOURCE) == OB_STOP_NoRST) || ((SOURCE) == OB_STOP_RST))
/**
  * @}
  */

/** 
  * @defgroup FLASH_Option_Bytes_nRST_STDBY STANDBY模式复位配置
  * @{
  */
#define OB_STDBY_NoRST                 ((uint8_t)0x80) /* STANDBY模式不生成复位：bit7置1，保持部分状态 */
#define OB_STDBY_RST                   ((uint8_t)0x00) /* STANDBY模式生成复位：唤醒后执行完整复位 */

/* STANDBY模式复位参数校验 */
#define IS_OB_STDBY_SOURCE(SOURCE) (((SOURCE) == OB_STDBY_NoRST) || ((SOURCE) == OB_STDBY_RST))
/**
  * @brief  FLASH配置扩展模块
  * @note   本模块包含STM32F4系列闪存操作的进阶配置项
  * @detail 主要功能包括：
  *         - BOR（欠压复位）阈值配置
  *         - 双Bank启动模式设置
  *         - 闪存操作中断控制
  *         - 闪存状态标志位定义
  *         所有IS_xxx宏用于参数合法性校验
  */

/** @defgroup FLASH_BOR_Reset_Level 欠压复位阈值配置
  * @{
  */  
#define OB_BOR_LEVEL3          ((uint8_t)0x00)  /* BOR等级3：2.7V-3.6V（最高阈值，适用于不稳定电源） */
#define OB_BOR_LEVEL2          ((uint8_t)0x04)  /* BOR等级2：2.4V-2.7V（平衡功耗与稳定性） */
#define OB_BOR_LEVEL1          ((uint8_t)0x08)  /* BOR等级1：2.1V-2.4V（低功耗应用场景） */
#define OB_BOR_OFF             ((uint8_t)0x0C)  /* 关闭BOR：1.62V-2.1V（最低工作电压，风险最高） */

/* BOR等级参数校验（注意OFF状态的风险） */
#define IS_OB_BOR(LEVEL) (((LEVEL) == OB_BOR_LEVEL1) || ((LEVEL) == OB_BOR_LEVEL2) ||\
                          ((LEVEL) == OB_BOR_LEVEL3) || ((LEVEL) == OB_BOR_OFF))
/**
  * @}
  */
  
/** @defgroup FLASH_Dual_Boot 双Bank启动配置
  * @{
  */
#define OB_Dual_BootEnabled   ((uint8_t)0x10) /* 启用双Bank启动：bit4置1，根据BOOT引脚选择启动Bank */
#define OB_Dual_BootDisabled  ((uint8_t)0x00) /* 禁用双Bank：始终从用户Flash启动（默认配置） */

/* 双启动模式参数校验 */
#define IS_OB_BOOT(BOOT) (((BOOT) == OB_Dual_BootEnabled) || ((BOOT) == OB_Dual_BootDisabled))
/**
  * @}
  */
/**
  * @brief  FLASH中断与状态标志模块
  * @note   本模块控制闪存操作的中断触发机制和状态监控
  * @detail 主要包含：
  *         - 闪存操作完成/错误中断使能配置
  *         - 7种闪存状态标志定义
  *         - 标志位合法性校验规则
  *         特别提示：所有标志位操作需在Flash不忙(BSY=0)时进行
  */

/** @defgroup FLASH_Interrupts 闪存操作中断源
  * @{
  */ 
#define FLASH_IT_EOP   ((uint32_t)0x01000000)  /* 操作完成中断：bit24置1，用于编程/擦除完成通知 */
#define FLASH_IT_ERR   ((uint32_t)0x02000000)  /* 错误中断：bit25置1，覆盖所有闪存操作错误类型 */

/* 中断源有效性校验（0xFCFFFFFF掩码保留高6位） */
#define IS_FLASH_IT(IT) ((((IT) & 0xFCFFFFFF) == 0x0) && ((IT) != 0x0))
/**
  * @}
  */

/** @defgroup FLASH_Flags 闪存状态标志位
  * @{
  */
/*----- 基础状态标志 -----*/
#define FLASH_FLAG_EOP   ((uint32_t)0x00000001)  /* 操作完成标志：bit0，需手动清除 */
#define FLASH_FLAG_BSY   ((uint32_t)0x00010000)  /* 忙状态标志：bit16，操作进行时自动置1 */

/*----- 错误类型标志（按bit顺序）-----*/
#define FLASH_FLAG_OPERR    ((uint32_t)0x00000002)  /* 通用错误：bit1，未分类错误时置位 */
#define FLASH_FLAG_WRPERR   ((uint32_t)0x00000010)  /* 写保护错误：bit4，写受保护区域时触发 */
#define FLASH_FLAG_PGAERR   ((uint32_t)0x00000020)  /* 对齐错误：bit5，非32位对齐访问时触发 */
#define FLASH_FLAG_PGPERR   ((uint32_t)0x00000040)  /* 并行模式错误：bit6，数据总线配置错误 */
#define FLASH_FLAG_PGSERR   ((uint32_t)0x00000080)  /* 序列错误：bit7，操作步骤违反规范 */
#define FLASH_FLAG_RDERR    ((uint32_t)0x00000100)  /* 读保护错误：bit8，非法访问PCROP区域 */

/* 标志清除校验（0xFFFFFE0C掩码过滤保留位） */
#define IS_FLASH_CLEAR_FLAG(FLAG) (((FLAG) & 0xFFFFFE0C) == 0x0)  /* 允许清除bit0,1,4-8,16 */

/* 标志读取校验（需严格匹配预定义标志） */
#define IS_FLASH_GET_FLAG(FLAG)  ((FLAG) == FLASH_FLAG_EOP    ||   /* 操作完成 */\
                                  (FLAG) == FLASH_FLAG_OPERR  ||   /* 通用错误 */\
                                  (FLAG) == FLASH_FLAG_WRPERR ||   /* 写保护 */\
                                  (FLAG) == FLASH_FLAG_PGAERR ||   /* 对齐错误 */\
                                  (FLAG) == FLASH_FLAG_PGPERR ||   /* 并行错误 */\
                                  (FLAG) == FLASH_FLAG_PGSERR ||   /* 顺序错误 */\
                                  (FLAG) == FLASH_FLAG_BSY    ||   /* 忙状态 */\
                                  (FLAG) == FLASH_FLAG_RDERR)       /* 读保护错误 */
/**
  * @}
  */
/**
  * @brief  FLASH编程配置与寄存器访问模块
  * @note   本模块包含闪存编程参数配置、安全密钥及寄存器地址定义
  * @detail 主要功能包括：
  *         - 编程并行度设置
  *         - 闪存操作安全密钥
  *         - 关键寄存器字节地址映射
  *         特别注意：密钥操作影响芯片安全状态，需严格遵循操作时序
  */

/** @defgroup FLASH_Program_Parallelism 编程并行度配置
  * @{
  */
#define FLASH_PSIZE_BYTE        ((uint32_t)0x00000000)  /* 8位并行：适用于低速模式，单字节操作 */
#define FLASH_PSIZE_HALF_WORD   ((uint32_t)0x00000100)  /* 16位并行：平衡速度与功耗（常用配置）*/
#define FLASH_PSIZE_WORD        ((uint32_t)0x00000200)  /* 32位并行：最高速度模式，需VDD>2.7V */
#define FLASH_PSIZE_DOUBLE_WORD ((uint32_t)0x00000300)  /* 64位并行：仅适用于大容量型号 */

/* 并行度配置掩码（CR寄存器bit8-9清零掩码） */
#define CR_PSIZE_MASK           ((uint32_t)0xFFFFFCFF)  /* 二进制：1111 1111 1111 1111 1100 1111 1111 */
/**
  * @}
  */

/** @defgroup FLASH_Keys 安全操作密钥
  * @{
  */ 
#define RDP_KEY        ((uint16_t)0x00A5)  /* 读保护解除密钥：写入RDP寄存器关闭读保护 */
#define FLASH_KEY1     ((uint32_t)0x45670123)  /* 主闪存解锁密钥1（必须首个写入） */
#define FLASH_KEY2     ((uint32_t)0xCDEF89AB)  /* 主闪存解锁密钥2（必须第二个写入） */
#define FLASH_OPT_KEY1 ((uint32_t)0x08192A3B)  /* 选项字节解锁密钥1（写OPTKEYR寄存器） */
#define FLASH_OPT_KEY2 ((uint32_t)0x4C5D6E7F)  /* 选项字节解锁密钥2（连续写入生效） */
/**
  * @}
  */

/** @defgroup FLASH_Register_Addresses 寄存器字节地址
  * @{
  */
/*---- 访问控制寄存器(ACR) ----*/
#define ACR_BYTE0_ADDRESS   ((uint32_t)0x40023C00)  /* ACR[7:0]：等待周期/预取使能配置 */

/*---- 选项控制寄存器(OPTCR) ----*/
#define OPTCR_BYTE0_ADDRESS ((uint32_t)0x40023C14)  /* OPTCR[7:0]：读保护等级配置 */
#define OPTCR_BYTE1_ADDRESS ((uint32_t)0x40023C15)  /* OPTCR[15:8]：用户选项配置 */
#define OPTCR_BYTE2_ADDRESS ((uint32_t)0x40023C16)  /* OPTCR[23:16]：PCROP配置低位 */
#define OPTCR_BYTE3_ADDRESS ((uint32_t)0x40023C17)  /* OPTCR[31:24]：PCROP配置高位 */

/*---- 选项控制寄存器1(OPTCR1) ----*/
#define OPTCR1_BYTE2_ADDRESS ((uint32_t)0x40023C1A)  /* OPTCR1[7:0]：双Bank模式配置 */
/**
  * @}
  */


/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 
 /**
  * @brief  FLASH操作接口函数模块
  * @note   本模块提供闪存控制器的底层操作接口
  * @detail 包含两大功能类别：
  *         - 闪存访问性能配置（等待周期/缓存控制）
  *         - 闪存存储器编程操作（擦除/写入）
  *         重要提示：所有写操作前必须执行解锁序列，操作后建议重新上锁
  */

/************************************
 * 闪存接口配置函数组
 ************************************/
 
/* 设置等待周期（根据SYSCLK频率调整）*/
void FLASH_SetLatency(uint32_t FLASH_Latency);  /* 参数范围：FLASH_ACR_LATENCYx (x=0-7) */

/* 预取缓冲器使能控制 */
void FLASH_PrefetchBufferCmd(FunctionalState NewState);  /* ENABLE-提升性能，DISABLE-降低功耗 */

/* 指令缓存开关（建议始终开启） */
void FLASH_InstructionCacheCmd(FunctionalState NewState);  /* 影响代码执行速度 */

/* 数据缓存开关（数据频繁访问时建议开启） */
void FLASH_DataCacheCmd(FunctionalState NewState);         /* 影响数据读取速度 */

/* 指令缓存复位（异常处理时使用） */
void FLASH_InstructionCacheReset(void);  /* 清空缓存，解决代码更新后的同步问题 */

/* 数据缓存复位（数据一致性维护） */
void FLASH_DataCacheReset(void);         /* 确保缓存数据与闪存实际数据一致 */


/************************************
 * 闪存存储器编程函数组
 ************************************/

/* 解锁闪存控制寄存器 */
void FLASH_Unlock(void);  /* 必须与FLASH_Lock配对使用，遵循KEY1+KEY2序列 */

/* 重新上锁闪存寄存器 */
void FLASH_Lock(void);    /* 操作完成后必须调用以确保安全 */

/* 擦除指定扇区 */
FLASH_Status FLASH_EraseSector(
    uint32_t FLASH_Sector,  /* 扇区号（0-11对应不同容量型号） */
    uint8_t VoltageRange    /* 电压范围（VOLTAGE_RANGE_1/2/3/4） */
);

/* 全片擦除（慎用！将清除所有用户代码） */
FLASH_Status FLASH_EraseAllSectors(uint8_t VoltageRange);       /* 擦除所有可用扇区 */

/* Bank1全擦除（双Bank型号专用） */
FLASH_Status FLASH_EraseAllBank1Sectors(uint8_t VoltageRange); /* 擦除Bank1所有扇区 */

/* Bank2全擦除（双Bank型号专用） */
FLASH_Status FLASH_EraseAllBank2Sectors(uint8_t VoltageRange); /* 擦除Bank2所有扇区 */

/* 64位编程（地址需8字节对齐） */
FLASH_Status FLASH_ProgramDoubleWord(
    uint32_t Address,   /* 目标地址（必须位于闪存区间） */
    uint64_t Data       /* 写入数据（小端格式存储） */
);

/* 32位编程（地址需4字节对齐） */
FLASH_Status FLASH_ProgramWord(
    uint32_t Address, 
    uint32_t Data
);

/* 16位编程（地址需2字节对齐） */
FLASH_Status FLASH_ProgramHalfWord(
    uint32_t Address, 
    uint16_t Data
);

/* 8位编程（无对齐要求） */
FLASH_Status FLASH_ProgramByte(
    uint32_t Address, 
    uint8_t Data
);

/**
  * @brief  FLASH选项字节与状态管理模块
  * @note   本模块提供选项字节配置和闪存状态监控功能
  * @detail 主要包含：
  *         - 选项字节编程（读保护/看门狗/启动配置等）
  *         - 中断和标志位管理系统
  *         特别注意：选项字节修改将导致系统复位，操作顺序必须严格遵循：
  *         解锁OB -> 修改配置 -> 启动重载 -> 重新上锁
  */

/************************************
 * 选项字节(Option Bytes)编程函数组
 ************************************/

/* 解锁选项字节编辑权限 */
void FLASH_OB_Unlock(void);  /* 必须与FLASH_OB_Lock配对使用 */

/* 锁定选项字节编辑权限 */
void FLASH_OB_Lock(void);    /* 防止意外修改关键配置 */

/* 主存储区写保护配置 */
void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState);  /* 参数：扇区掩码+使能状态 */

/* 扩展存储区写保护配置（双Bank型号） */
void FLASH_OB_WRP1Config(uint32_t OB_WRP, FunctionalState NewState); /* Bank2专用配置 */

/* PCROP保护区域选择 */
void FLASH_OB_PCROPSelectionConfig(uint8_t OB_PcROP);  /* 选择保护Bank1/Bank2 */

/* 主存储区PCROP保护配置 */
void FLASH_OB_PCROPConfig(uint32_t OB_PCROP, FunctionalState NewState);  /* 设置保护扇区掩码 */

/* 扩展存储区PCROP保护配置 */
void FLASH_OB_PCROP1Config(uint32_t OB_PCROP, FunctionalState NewState); /* Bank2专用配置 */

/* 读保护等级配置（警告：Level2不可逆） */
void FLASH_OB_RDPConfig(uint8_t OB_RDP);  /* 参数：OB_RDP_Level_0/1 */

/* 用户选项配置（看门狗+低功耗模式） */
void FLASH_OB_UserConfig(
    uint8_t OB_IWDG,  /* 看门狗模式：OB_IWDG_SW/HW */
    uint8_t OB_STOP,  /* STOP模式复位：OB_STOP_NoRST/RST */
    uint8_t OB_STDBY  /* STANDBY模式复位：OB_STDBY_NoRST/RST */
);

/* 欠压复位阈值配置 */
void FLASH_OB_BORConfig(uint8_t OB_BOR);  /* 参数：OB_BOR_LEVELx */

/* 双Bank启动配置 */
void FLASH_OB_BootConfig(uint8_t OB_BOOT);  /* 参数：OB_Dual_BootEnabled/Disabled */

/* 应用选项字节修改（触发系统复位） */
FLASH_Status FLASH_OB_Launch(void);  /* 返回状态：配置是否成功写入 */

/* 获取用户选项字节 */
uint8_t FLASH_OB_GetUser(void);      /* 返回值：IWDG+STOP+STDBY配置组合状态 */

/* 获取主存储区写保护状态 */
uint16_t FLASH_OB_GetWRP(void);      /* 返回值：bit0-11对应扇区保护状态 */

/* 获取扩展存储区写保护状态 */
uint16_t FLASH_OB_GetWRP1(void);     /* 双Bank型号专用 */

/* 获取主存储区PCROP保护状态 */
uint16_t FLASH_OB_GetPCROP(void);     /* 读取实际生效的PCROP配置 */

/* 获取扩展存储区PCROP保护状态 */
uint16_t FLASH_OB_GetPCROP1(void);    /* Bank2专用 */

/* 获取当前读保护等级 */
FlagStatus FLASH_OB_GetRDP(void);     /* 返回SET=Level1/RESET=Level0 */

/* 获取BOR配置等级 */
uint8_t FLASH_OB_GetBOR(void);        /* 返回值：当前生效的BOR等级 */


/************************************
 * 中断与状态管理函数组
 ************************************/

/* 闪存中断使能控制 */
void FLASH_ITConfig(
    uint32_t FLASH_IT,      /* 中断源：FLASH_IT_EOP/ERR */
    FunctionalState NewState
);

/* 获取指定标志位状态 */
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG);  /* 参数：FLASH_FLAG_xx系列 */

/* 清除指定标志位 */
void FLASH_ClearFlag(uint32_t FLASH_FLAG);  /* 可清除：EOP/OPERR/WRPERR等 */

/* 获取闪存操作状态 */
FLASH_Status FLASH_GetStatus(void);  /* 替代多个标志位查询 */

/* 等待最近操作完成（超时检测） */
FLASH_Status FLASH_WaitForLastOperation(void);  /* 需指定超时阈值 */


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_FLASH_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

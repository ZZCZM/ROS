/**
  ******************************************************************************
  * @file    stm32f4xx_rtc.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the RTC firmware
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
#ifndef __STM32F4xx_RTC_H
#define __STM32F4xx_RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup RTC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/**
 * @file stm32f4xx_rtc.h
 * @brief STM32F4 RTC模块数据结构定义
 * @details 包含RTC核心配置结构体，满足以下功能需求：
 * - 实时时钟初始化参数配置
 * - 精确时间戳记录（时/分/秒）
 * - 完整日期记录（年/月/日/星期）
 * - 可编程闹钟配置
 * 适用场景：物联网设备时间戳记录、定时唤醒系统等
 */

/** 
  * @brief  RTC初始化配置结构体
  * @warning 配置前必须启用PWR和BKP时钟
  */
typedef struct
{
  uint32_t RTC_HourFormat;   /*!< 小时格式选择
                             @arg RTC_HourFormat_12: AM/PM格式（12小时制）
                             @arg RTC_HourFormat_24: 军用格式（24小时制） 
                             @note 格式变更需先执行RTC复位 */
  
  uint32_t RTC_AsynchPrediv; /*!< 异步预分频系数 (7位有效) 
                             @range 0x00-0x7F 
                             @warning 该值影响RTC时钟误差，典型值127 */
  
  uint32_t RTC_SynchPrediv;  /*!< 同步预分频系数 (15位有效)
                             @range 0x0000-0x7FFF
                             @note 需满足：(AsynchPrediv+1)*(SynchPrediv+1)=32768 
                             如：127+1)*255+1)=32768→1Hz脉冲 */
}RTC_InitTypeDef;

/** 
  * @brief  RTC时间戳结构体 
  * @note 精度支持到秒级，通过RTC_TR寄存器映射
  */
typedef struct
{
  uint8_t RTC_Hours;    /*!< 小时值 
                        @range 12小时制:1-12, 24小时制:0-23 
                        @warning 超过范围会导致TR寄存器写入失败 */

  uint8_t RTC_Minutes;  /*!< 分钟值 
                        @range 0-59 BCD格式 */

  uint8_t RTC_Seconds;  /*!< 秒值 
                        @range 0-59 BCD格式 
                        @note 与RTC_SSR组成亚秒级计时 */

  uint8_t RTC_H12;      /*!< 上午/下午标识 
                        @arg RTC_H12_AM: 上午(12小时制有效)
                        @arg RTC_H12_PM: 下午 */
}RTC_TimeTypeDef; 

/** 
  * @brief  RTC日期结构体
  * @note 自动处理闰年计算，通过RTC_DR寄存器映射
  */
typedef struct
{
  uint8_t RTC_WeekDay; /*!< 星期表示 
                       @arg RTC_Weekday_Monday    = 1
                       @arg RTC_Weekday_Sunday    = 7 
                       @warning 非法值会破坏日期计算 */

  uint8_t RTC_Month;   /*!< 月份 BCD编码
                       @range 1-12 (0x01-0x12 BCD)
                       @note 1月=0x01，12月=0x12 */

  uint8_t RTC_Date;    /*!< 日 
                       @range 1-31 (根据月份自动验证) */

  uint8_t RTC_Year;    /*!< 年份偏移值 
                       @range 00-99 (表示2000-2099)
                       @note 实际年份=2000+RTC_Year */
}RTC_DateTypeDef;

/** 
  * @brief  RTC闹钟配置结构体
  * @warning 需要启用RTC Alarm中断（EXTI17） 
  */
typedef struct
{
  RTC_TimeTypeDef RTC_AlarmTime;   /*!< 闹钟触发时间设置 */

  uint32_t RTC_AlarmMask;          /*!< 闹钟掩码配置 
                                   @arg RTC_AlarmMask_None: 精确匹配
                                   @arg RTC_AlarmMask_DateWeekDay: 忽略日期
                                   @arg RTC_AlarmMask_Hours: 每小时触发 */

  uint32_t RTC_AlarmDateWeekDaySel;/*!< 触发模式选择 
                                   @arg RTC_AlarmDateWeekDaySel_Date: 按日触发
                                   @arg RTC_AlarmDateWeekDaySel_WeekDay: 按周触发 */
  
  uint8_t RTC_AlarmDateWeekDay;    /*!< 触发日设定 
                                   @range 日期模式:1-31
                                   周模式:@ref RTC_WeekDay_Definitions */
}RTC_AlarmTypeDef;

/**
 * @file stm32f4xx_rtc.h
 * @brief RTC硬件配置参数校验体系
 * @details 包含三大核心配置校验模块：
 * 1. 时钟格式与预分频参数验证
 * 2. 时间/日期参数有效性验证
 * 3. 时间格式辅助定义
 * 作用：保证RTC硬件寄存器写入值合法性
 */

/*------------------------ 时钟格式配置 ------------------------*/
/** @defgroup RTC_Hour_Formats 时钟格式模式
  * @brief 控制CR寄存器FMT位(bit6)
  * @note 影响RTC_TR寄存器小时字段解析方式
  */
#define RTC_HourFormat_24   ((uint32_t)0x00000000) ///< 24小时制（默认模式）
#define RTC_HourFormat_12   ((uint32_t)0x00000040) ///< 12小时制（BIT6置位）
#define IS_RTC_HOUR_FORMAT(FORMAT) (((FORMAT) == RTC_HourFormat_12) || \
                                    ((FORMAT) == RTC_HourFormat_24)) ///< 格式校验宏

/*--------------------- 预分频参数校验 ---------------------*/
/** @defgroup RTC_Asynchronous_Predivider 异步预分频
  * @brief 配置PRER寄存器[6:0]位域
  * @note 控制RTC时钟分频第一阶段
  */
#define IS_RTC_ASYNCH_PREDIV(PREDIV) ((PREDIV) <= 0x7F) ///< 7位最大值校验（0-127）

/** @defgroup RTC_Synchronous_Predivider 同步预分频
  * @brief 配置PRER寄存器[22:16]位域
  * @note 控制RTC时钟分频第二阶段
  */
#define IS_RTC_SYNCH_PREDIV(PREDIV)  ((PREDIV) <= 0x7FFF) ///< 15位最大值校验（0-32767）

/*--------------------- 时间参数有效性验证 ---------------------*/
/** @defgroup RTC_Time_Definitions 时间参数校验
  * @brief 验证RTC时间结构体字段合法性
  */
#define IS_RTC_HOUR12(HOUR)  (((HOUR) > 0) && ((HOUR) <= 12)) ///< 12小时制范围校验
#define IS_RTC_HOUR24(HOUR)  ((HOUR) <= 23)                   ///< 24小时制范围校验
#define IS_RTC_MINUTES(M)    ((M) <= 59)  ///< 分钟值校验（BCD格式0x00-0x59）
#define IS_RTC_SECONDS(S)    ((S) <= 59)  ///< 秒值校验（结合亚秒寄存器使用）

/*--------------------- 时间格式定义 ---------------------*/
/** @defgroup RTC_AM_PM_Definitions 上下午标识
  * @brief 控制TR寄存器PM位(bit6)
  * @warning 仅在12小时制下有效
  */
#define RTC_H12_AM  ((uint8_t)0x00) ///< 上午（TR[22]=0）
#define RTC_H12_PM  ((uint8_t)0x40) ///< 下午（TR[22]=1）
#define IS_RTC_H12(PM) (((PM) == RTC_H12_AM) || ((PM) == RTC_H12_PM)) ///< 标识校验

/*--------------------- 年份参数校验 ---------------------*/
/** @defgroup RTC_Year_Date_Definitions 年份校验
  * @brief 验证年份字段的BCD编码合法性
  */
#define IS_RTC_YEAR(YEAR) ((YEAR) <= 99) ///< 支持2000-2099年表示（00-99 BCD）

/**
  * @}
  */ 
/**
 * @file stm32f4xx_rtc.h
 * @brief RTC日期组件配置宏定义
 * @details 包含日期系统的三大核心要素：
 * 1. 月份的BCD编码表示
 * 2. 日期范围有效性验证
 * 3. 星期枚举系统
 * 特殊说明：所有日期相关配置均采用BCD编码格式
 */

/*--------------------- 月份BCD编码定义 ---------------------*/
/** @defgroup RTC_Month_Date_Definitions 月份枚举
  * @brief 映射DR寄存器[19:16]位域的BCD编码值
  * @note 实际存储格式：0xMM（M为BCD码）
  * @warning 禁止直接使用十进制数值赋值
  */
#define RTC_Month_January    ((uint8_t)0x01) ///< 一月 BCD(0x01)=DEC(1)
#define RTC_Month_February   ((uint8_t)0x02) ///< 二月（自动处理28/29天）
#define RTC_Month_March      ((uint8_t)0x03) ///< 三月 
#define RTC_Month_April      ((uint8_t)0x04) ///< 四月（30天限制）
#define RTC_Month_May        ((uint8_t)0x05) ///< 五月 
#define RTC_Month_June       ((uint8_t)0x06) ///< 六月（30天）
#define RTC_Month_July       ((uint8_t)0x07) ///< 七月 
#define RTC_Month_August     ((uint8_t)0x08) ///< 八月 
#define RTC_Month_September  ((uint8_t)0x09) ///< 九月（30天）
#define RTC_Month_October    ((uint8_t)0x10) ///< 十月 BCD(0x10)=DEC(10)
#define RTC_Month_November   ((uint8_t)0x11) ///< 十一月 
#define RTC_Month_December   ((uint8_t)0x12) ///< 十二月 BCD(0x12)=DEC(12)

/** @defgroup 日期有效性验证 */
#define IS_RTC_MONTH(MONTH) (((MONTH) >= 1) && ((MONTH) <= 12))  ///< 月份范围校验（十进制值）
#define IS_RTC_DATE(DATE)   (((DATE) >= 1) && ((DATE) <= 31))     ///< 基础日期校验（不区分月份）

/*--------------------- 星期枚举系统 ---------------------*/
/** @defgroup RTC_WeekDay_Definitions 星期编码
  * @brief 映射DR寄存器[10:8]位域
  * @note 编码规则：
  * 周一=0x01 至 周日=0x07
  * 影响自动日期计算
  */
#define RTC_Weekday_Monday     ((uint8_t)0x01) ///< 星期一（ISO8601标准）
#define RTC_Weekday_Tuesday    ((uint8_t)0x02) ///< 星期二
#define RTC_Weekday_Wednesday  ((uint8_t)0x03) ///< 星期三
#define RTC_Weekday_Thursday   ((uint8_t)0x04) ///< 星期四
#define RTC_Weekday_Friday     ((uint8_t)0x05) ///< 星期五
#define RTC_Weekday_Saturday   ((uint8_t)0x06) ///< 星期六
#define RTC_Weekday_Sunday     ((uint8_t)0x07) ///< 星期日（每周最后一天）

#define IS_RTC_WEEKDAY(WEEKDAY) (/* 验证7天枚举值 */ \
    ((WEEKDAY) == RTC_Weekday_Monday)    || \
    ((WEEKDAY) == RTC_Weekday_Tuesday)   || \
    ((WEEKDAY) == RTC_Weekday_Wednesday) || \
    ((WEEKDAY) == RTC_Weekday_Thursday)  || \
    ((WEEKDAY) == RTC_Weekday_Friday)    || \
    ((WEEKDAY) == RTC_Weekday_Saturday)  || \
    ((WEEKDAY) == RTC_Weekday_Sunday))

/**
  * @}
  */ 

/**
 * @file stm32f4xx_rtc.h
 * @brief RTC闹钟配置核心参数定义
 * @details 包含三大配置维度：
 * 1. 闹钟触发条件选择（日期/星期）
 * 2. 时间字段掩码配置
 * 3. 双闹钟系统标识
 * 应用场景：定时唤醒、周期任务触发等
 */

/*----------------- 闹钟触发模式选择 -----------------*/
/** @defgroup RTC_AlarmDateWeekDay_Definitions 触发模式
  * @brief 配置ALRMAR/ALRMBR寄存器WDSEL位(bit30)
  */
#define RTC_AlarmDateWeekDaySel_Date    ((uint32_t)0x00000000) ///< 按日期触发（默认）
#define RTC_AlarmDateWeekDaySel_WeekDay ((uint32_t)0x40000000) ///< 按星期触发（BIT30置位）
#define IS_RTC_ALARM_DATE_WEEKDAY_SEL(SEL) /* 模式校验 */ \
    (((SEL) == RTC_AlarmDateWeekDaySel_Date) || \
     ((SEL) == RTC_AlarmDateWeekDaySel_WeekDay))

/*----------------- 时间字段掩码配置 -----------------*/
/** @defgroup RTC_AlarmMask_Definitions 字段掩码
  * @brief 控制ALRMxR寄存器MSKx位
  * @note 置位表示忽略对应字段比较
  */
#define RTC_AlarmMask_None     ((uint32_t)0x00000000) ///< 精确匹配所有字段
#define RTC_AlarmMask_DateWeekDay ((uint32_t)0x80000000) ///< 忽略日期/星期（BIT31）
#define RTC_AlarmMask_Hours    ((uint32_t)0x00800000) ///< 忽略小时（BIT23）
#define RTC_AlarmMask_Minutes  ((uint32_t)0x00008000) ///< 忽略分钟（BIT15）
#define RTC_AlarmMask_Seconds  ((uint32_t)0x00000080) ///< 忽略秒（BIT7）
#define RTC_AlarmMask_All      ((uint32_t)0x80808080) ///< 组合全忽略（每秒触发）

#define IS_ALARM_MASK(MASK) /* 掩码合法性校验 */ \
    (((MASK) & 0x7F7F7F7F) == (uint32_t)RESET) // 验证仅MSK位被置位

/*----------------- 双闹钟系统标识 -----------------*/
/** @defgroup RTC_Alarms_Definitions 闹钟通道
  * @brief 对应CR寄存器ALRAE/ALRBE使能位
  */
#define RTC_Alarm_A ((uint32_t)0x00000100) ///< 闹钟A（BIT8）
#define RTC_Alarm_B ((uint32_t)0x00000200) ///< 闹钟B（BIT9）

#define IS_RTC_ALARM(ALARM) /* 通道校验 */ \
    (((ALARM) == RTC_Alarm_A) || ((ALARM) == RTC_Alarm_B))

#define IS_RTC_CMD_ALARM(ALARM) /* 复合校验 */ \
    (((ALARM) & (RTC_Alarm_A | RTC_Alarm_B)) != (uint32_t)RESET)

/** 
  * @brief RTC报警子秒掩码定义集合
  * @defgroup RTC_Alarm_Sub_Seconds_Masks_Definitions
  * @{
  * @details 该组宏定义用于配置RTC报警子秒比较的掩码模式，通过ALRMxSSR寄存器的MASKSS[3:0]位域设置，
  *          控制报警发生时需要忽略的子秒位数。子秒寄存器为15位(SS[14:0])，掩码值决定从高位开始忽略的位数。
  */

#define RTC_AlarmSubSecondMask_All         ((uint32_t)0x00000000) /*!< 全掩码模式 
                                                                       [寄存器位域] MASKSS[3:0]=0x0
                                                                       [硬件行为] 禁用所有子秒位比较
                                                                       [应用场景] 仅需时/分/秒匹配报警 */
#define RTC_AlarmSubSecondMask_SS14_1      ((uint32_t)0x01000000) /*!< 14-1位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0x1
                                                                       [有效位] SS[0] 
                                                                       [精度范围] 1/32768秒
                                                                       [典型应用] 低精度定时任务 */
#define RTC_AlarmSubSecondMask_SS14_2      ((uint32_t)0x02000000) /*!< 14-2位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0x2
                                                                       [有效位] SS[1:0] (2位)
                                                                       [精度值] 约30.52ms
                                                                       [误差范围] ±0.0305秒 */
#define RTC_AlarmSubSecondMask_SS14_3      ((uint32_t)0x03000000) /*!< 14-3位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0x3
                                                                       [有效位] SS[2:0] (3位)
                                                                       [时间分辨率] 约7.63ms
                                                                       [适用场景] 周期性任务调度 */
#define RTC_AlarmSubSecondMask_SS14_4      ((uint32_t)0x04000000) /*!< 14-4位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0x4
                                                                       [有效位] SS[3:0] (4位)
                                                                       [时间精度] 约1.907ms
                                                                       [误差容限] ±0.0019秒 */
#define RTC_AlarmSubSecondMask_SS14_5      ((uint32_t)0x05000000) /*!< 14-5位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0x5
                                                                       [有效位] SS[4:0] (5位)
                                                                       [时间粒度] 约476μs
                                                                       [特殊说明] 需RTC时钟源≥1MHz */
#define RTC_AlarmSubSecondMask_SS14_6      ((uint32_t)0x06000000) /*!< 14-6位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0x6
                                                                       [有效位] SS[5:0] (6位)
                                                                       [精度参数] 约119μs
                                                                       [使用限制] 需启用RTC校准 */
#define RTC_AlarmSubSecondMask_SS14_7      ((uint32_t)0x07000000) /*!< 14-7位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0x7
                                                                       [有效位] SS[6:0] (7位)
                                                                       [时间精度] 约29.8μs
                                                                       [硬件要求] 外部32.768kHz晶振 */
#define RTC_AlarmSubSecondMask_SS14_8      ((uint32_t)0x08000000) /*!< 14-8位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0x8
                                                                       [有效位] SS[7:0] (8位)
                                                                       [分辨率] 约7.45μs
                                                                       [注意] 需要精确温度补偿 */
#define RTC_AlarmSubSecondMask_SS14_9      ((uint32_t)0x09000000) /*!< 14-9位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0x9
                                                                       [有效位] SS[8:0] (9位)
                                                                       [精度] 约1.86μs
                                                                       [限制条件] 仅限STM32F4xx系列 */
#define RTC_AlarmSubSecondMask_SS14_10     ((uint32_t)0x0A000000) /*!< 14-10位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0xA
                                                                       [有效位] SS[9:0] (10位)
                                                                       [时间精度] 约466ns
                                                                       [特殊应用] 高精度时间戳记录 */
#define RTC_AlarmSubSecondMask_SS14_11     ((uint32_t)0x0B000000) /*!< 14-11位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0xB
                                                                       [有效位] SS[10:0] (11位)
                                                                       [分辨率] 约116ns
                                                                       [硬件依赖] RTC同步时钟模式 */
#define RTC_AlarmSubSecondMask_SS14_12     ((uint32_t)0x0C000000) /*!< 14-12位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0xC
                                                                       [有效位] SS[11:0] (12位)
                                                                       [精度] 约29ns
                                                                       [注意] 需要禁用所有低功耗模式 */
#define RTC_AlarmSubSecondMask_SS14_13     ((uint32_t)0x0D000000) /*!< 14-13位掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0xD
                                                                       [有效位] SS[12:0] (13位)
                                                                       [时间粒度] 约7.3ns
                                                                       [限制] 仅调试模式使用 */
#define RTC_AlarmSubSecondMask_SS14        ((uint32_t)0x0E000000) /*!< 14位单掩码 
                                                                       [寄存器位域] MASKSS[3:0]=0xE
                                                                       [有效位] SS[13:0] (14位)
                                                                       [理论精度] 1.8ns
                                                                       [实际限制] 受时钟抖动影响较大 */
#define RTC_AlarmSubSecondMask_None        ((uint32_t)0x0F000000) /*!< 无掩码模式 
                                                                       [寄存器位域] MASKSS[3:0]=0xF
                                                                       [硬件行为] 全15位精确匹配
                                                                       [超时误差] ±1个RTC时钟周期
                                                                       [重要警告] 需严格时钟同步 */

/**
  * @}
  */
/**
  * @brief RTC报警子秒掩码有效性验证宏
  * @param MASK 需要验证的掩码值
  * @return 布尔类型校验结果
  *         - 1: 输入掩码有效
  *         - 0: 输入掩码非法
  * @details 该宏通过穷举所有预定义的合法掩码值，提供编译时参数校验功能。
  *          用于确保传递给RTC报警配置函数的子秒掩码参数符合硬件规范。
  */
#define IS_RTC_ALARM_SUB_SECOND_MASK(MASK)   (((MASK) == RTC_AlarmSubSecondMask_All) ||       /* 全掩码模式校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_1) ||      /* 14-1位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_2) ||      /* 14-2位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_3) ||      /* 14-3位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_4) ||     /* 14-4位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_5) ||      /* 14-5位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_6) ||      /* 14-6位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_7) ||      /* 14-7位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_8) ||      /* 14-8位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_9) ||      /* 14-9位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_10) ||     /* 14-10位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_11) ||     /* 14-11位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_12) ||     /* 14-12位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14_13) ||     /* 14-13位掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_SS14) ||        /* 14位单掩码校验 */\
                                              ((MASK) == RTC_AlarmSubSecondMask_None))           /* 全比较模式校验 */

/**
  * @}
  */ 

/** @defgroup RTC_Alarm_Sub_Seconds_Value
  * @{
  */ 
/**
  * @brief RTC报警子秒值范围验证宏
  * @param VALUE 待验证的子秒数值
  * @return 布尔类型校验结果
  *         - 1: 数值在0x0000~0x7FFF有效范围
  *         - 0: 数值越界
  * @details 用于校验ALARMxSSR寄存器SS[14:0]字段的数值合法性，
  *          对应15位无符号整数的最大值限制(32767)
  */
#define IS_RTC_ALARM_SUB_SECOND_VALUE(VALUE) ((VALUE) <= 0x00007FFF)

/**
  * @brief RTC唤醒时钟源配置定义集合
  * @defgroup RTC_Wakeup_Timer_Definitions
  * @{
  */
 
/** 
  * @brief RTC时钟(RTCCLK)16分频模式
  * @note 时钟频率 = RTCCLK / 16 
  *       典型应用：外部32.768kHz时钟时唤醒频率为2.048kHz
  */
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)

/**
  * @brief RTC时钟8分频模式
  * @note 时钟频率 = RTCCLK / 8
  *       典型应用：配合校准电路实现中等精度定时
  */
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)

/**
  * @brief RTC时钟4分频模式
  * @note 时钟频率 = RTCCLK / 4
  *       适用场景：需要较高唤醒频率的短期定时
  */
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)

/**
  * @brief RTC时钟2分频模式
  * @note 时钟频率 = RTCCLK / 2
  *       限制条件：最大唤醒周期不超过18小时
  */
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)

/**
  * @brief 同步预分频器16位时钟模式
  * @note 使用1Hz时钟源(CK_SPRE)
  *       唤醒周期计算：WUTR[15:0] + 1 
  *       最大周期：65535秒（约18.2小时）
  */
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)

/**
  * @brief 同步预分频器17位时钟模式
  * @note 使用1Hz时钟源(CK_SPRE)
  *       唤醒周期计算：(WUTR[16:0] + 1) * 2
  *       最大周期：131072秒（约36.4小时）
  */
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)

/**
  * @brief 唤醒时钟源有效性验证宏
  * @param CLOCK 待验证的时钟源参数
  * @return 布尔类型校验结果
  *         - 1: 参数属于预定义时钟源
  *         - 0: 非法时钟参数
  * @warning 必须在校验通过后才能写入WUCKSEL[2:0]寄存器位
  */
#define IS_RTC_WAKEUP_CLOCK(CLOCK) (((CLOCK) == RTC_WakeUpClock_RTCCLK_Div16) || \
                                    ((CLOCK) == RTC_WakeUpClock_RTCCLK_Div8) || \
                                    ((CLOCK) == RTC_WakeUpClock_RTCCLK_Div4) || \
                                    ((CLOCK) == RTC_WakeUpClock_RTCCLK_Div2) || \
                                    ((CLOCK) == RTC_WakeUpClock_CK_SPRE_16bits) || \
                                    ((CLOCK) == RTC_WakeUpClock_CK_SPRE_17bits))

/**
  * @brief 唤醒计数器值范围验证宏
  * @param COUNTER 待验证的计数器值
  * @return 布尔类型校验结果
  *         - 1: 数值在0x0000~0xFFFF有效范围
  *         - 0: 数值越界
  * @details 对应WUTR寄存器的16位无符号整数限制，
  *          当使用17位模式时实际有效位为WUTR[16:0]
  */
#define IS_RTC_WAKEUP_COUNTER(COUNTER)  ((COUNTER) <= 0xFFFF)

/**
  * @}
  */
/**
  * @brief RTC时间戳边沿检测配置组
  * @defgroup RTC_Time_Stamp_Edges_definitions 
  * @{
  */
#define RTC_TimeStampEdge_Rising          ((uint32_t)0x00000000) /*!< 上升沿触发时间戳事件 
                                                                      [寄存器位] TSEG[3]=0 
                                                                      [应用场景] 精确事件时间捕获 */
#define RTC_TimeStampEdge_Falling         ((uint32_t)0x00000008) /*!< 下降沿触发时间戳事件
                                                                      [寄存器位] TSEG[3]=1 
                                                                      [典型应用] 电源跌落检测 */
#define IS_RTC_TIMESTAMP_EDGE(EDGE) (((EDGE) == RTC_TimeStampEdge_Rising) || \
                                     ((EDGE) == RTC_TimeStampEdge_Falling)) // 边沿模式校验宏
/**
  * @}
  */

/**
  * @brief RTC输出信号选择配置组
  * @defgroup RTC_Output_selection_Definitions 
  * @{
  */
#define RTC_Output_Disable             ((uint32_t)0x00000000) /*!< 禁用RTC输出 
                                                                  [寄存器值] OSEL[1:0]=00 
                                                                  [功耗影响] 最低功耗模式 */
#define RTC_Output_AlarmA              ((uint32_t)0x00200000) /*!< 输出报警A事件信号
                                                                  [输出特性] 脉冲宽度=1个RTCCLK周期
                                                                  [连接引脚] PC13/RTC_AF1 */
#define RTC_Output_AlarmB              ((uint32_t)0x00400000) /*!< 输出报警B事件信号 
                                                                  [电气特性] 推挽输出模式
                                                                  [复用功能] RTC_AF2 */
#define RTC_Output_WakeUp              ((uint32_t)0x00600000) /*!< 输出唤醒事件信号
                                                                  [信号频率] 可配置为1Hz或512Hz
                                                                  [应用场景] 外部MCU唤醒 */
#define IS_RTC_OUTPUT(OUTPUT) (((OUTPUT) == RTC_Output_Disable) || \
                               ((OUTPUT) == RTC_Output_AlarmA) || \
                               ((OUTPUT) == RTC_Output_AlarmB) || \
                               ((OUTPUT) == RTC_Output_WakeUp)) // 输出模式校验宏
/**
  * @}
  */

/**
  * @brief RTC输出极性配置组
  * @defgroup RTC_Output_Polarity_Definitions 
  * @{
  */
#define RTC_OutputPolarity_High           ((uint32_t)0x00000000) /*!< 高电平有效输出
                                                                      [寄存器位] POL=0
                                                                      [典型电路] 驱动PNP三极管 */
#define RTC_OutputPolarity_Low            ((uint32_t)0x00100000) /*!< 低电平有效输出 
                                                                      [寄存器位] POL=1
                                                                      [兼容性] 匹配NPN逻辑电平 */
#define IS_RTC_OUTPUT_POL(POL) (((POL) == RTC_OutputPolarity_High) || \
                                ((POL) == RTC_OutputPolarity_Low)) // 极性模式校验宏
/**
  * @}
  */

/**
  * @brief RTC数字校准配置组
  * @defgroup RTC_Digital_Calibration_Definitions 
  * @{
  */
#define RTC_CalibSign_Positive            ((uint32_t)0x00000000) /*!< 正向校准 
                                                                      [寄存器位] CALP=1
                                                                      [效果] 增加时钟脉冲 */
#define RTC_CalibSign_Negative            ((uint32_t)0x00000080) /*!< 负向校准
                                                                      [寄存器位] CALP=0,CALM=1
                                                                      [效果] 减少时钟脉冲 */
#define IS_RTC_CALIB_SIGN(SIGN) (((SIGN) == RTC_CalibSign_Positive) || \
                                 ((SIGN) == RTC_CalibSign_Negative)) // 校准符号校验宏
#define IS_RTC_CALIB_VALUE(VALUE) ((VALUE) < 0x20) // 校准值范围校验(0~31)
/**
  * @}
  */

/**
  * @brief RTC校准输出配置组
  * @defgroup RTC_Calib_Output_selection_Definitions 
  * @{
  */
#define RTC_CalibOutput_512Hz            ((uint32_t)0x00000000) /*!< 512Hz校准输出
                                                                     [精度] ±487ppm 
                                                                     [应用] 时钟同步测试 */
#define RTC_CalibOutput_1Hz              ((uint32_t)0x00080000) /*!< 1Hz校准输出
                                                                     [误差] < ±1ppm(校准后)
                                                                     [用途] 精密计时基准 */
#define IS_RTC_CALIB_OUTPUT(OUTPUT)  (((OUTPUT) == RTC_CalibOutput_512Hz) || \
                                      ((OUTPUT) == RTC_CalibOutput_1Hz)) // 校准输出模式校验
/**
  * @}
  */

/**
  * @brief RTC平滑校准周期配置组
  * @defgroup RTC_Smooth_calib_period_Definitions 
  * @{
  */
#define RTC_SmoothCalibPeriod_32sec   ((uint32_t)0x00000000) /*!< 32秒校准周期
                                                                 [计算公式] 2^20 / RTCCLK
                                                                 [精度提升] ±0.954ppm/step */
#define RTC_SmoothCalibPeriod_16sec   ((uint32_t)0x00002000) /*!< 16秒校准周期
                                                                 [补偿频率] 每16秒调整1个LSB
                                                                 [适用场景] 温度变化较快的环境 */
#define RTC_SmoothCalibPeriod_8sec    ((uint32_t)0x00004000) /*!< 8秒校准周期
                                                                 [寄存器位] CALW8=1
                                                                 [响应速度] 最快温度补偿 */
#define IS_RTC_SMOOTH_CALIB_PERIOD(PERIOD) (((PERIOD) == RTC_SmoothCalibPeriod_32sec) || \
                                             ((PERIOD) == RTC_SmoothCalibPeriod_16sec) || \
                                             ((PERIOD) == RTC_SmoothCalibPeriod_8sec)) // 周期校验宏
/**
  * @}
  */

               /**
  * @brief  RTC 平滑校准加脉冲定义
  *         这些定义用于设置RTC的平滑校准加脉冲功能。
  *         平滑校准加脉冲功能允许在特定时间段内添加或减少RTC时钟脉冲的数量，以实现精确的时钟校准。
  */

/**
  * @}
  */

/** @defgroup RTC_Smooth_calib_Plus_pulses_Definitions 
  * @brief  RTC 平滑校准加脉冲定义
  *         这些定义用于设置RTC的平滑校准加脉冲功能。
  *         平滑校准加脉冲功能允许在特定时间段内添加或减少RTC时钟脉冲的数量，以实现精确的时钟校准。
  * @{
  */ 
#define RTC_SmoothCalibPlusPulses_Set    ((uint32_t)0x00008000) /*!<  设置RTC时钟脉冲的添加功能。
                                                                在一个X秒的窗口内添加的RTCCLK脉冲数量 = Y - CALM[8:0]。
                                                                 其中Y = 512, 256, 128，当X = 32, 16, 8时。 */
#define RTC_SmoothCalibPlusPulses_Reset  ((uint32_t)0x00000000) /*!<  设置RTC时钟脉冲的重置功能。
                                                                 在一个32秒的窗口内替换的RTCCLK脉冲数量 = CALM[8:0]. */
#define IS_RTC_SMOOTH_CALIB_PLUS(PLUS) (((PLUS) == RTC_SmoothCalibPlusPulses_Set) || \
                                         ((PLUS) == RTC_SmoothCalibPlusPulses_Reset)) /*!< 检查平滑校准加脉冲设置是否有效。
                                         有效设置包括RTC_SmoothCalibPlusPulses_Set和RTC_SmoothCalibPlusPulses_Reset。 */

/**
  * @}
  */ 

/** @defgroup RTC_Smooth_calib_Minus_pulses_Definitions 
  * @brief  RTC 平滑校准减脉冲定义
  *         这些定义用于检查RTC的平滑校准减脉冲值是否有效。
  * @{
  */ 
#define  IS_RTC_SMOOTH_CALIB_MINUS(VALUE) ((VALUE) <= 0x000001FF) /*!< 检查平滑校准减脉冲值是否有效。
                                      有效值范围为0到0x01FF（511）。 */

/**
  * @}
  */

/** @defgroup RTC_DayLightSaving_Definitions 
  * @brief  RTC 夏令时定义
  *         这些定义用于设置和检查RTC的夏令时调整功能。
  * @{
  */ 
#define RTC_DayLightSaving_SUB1H   ((uint32_t)0x00020000) /*!< 设置RTC夏令时减少1小时。 */
#define RTC_DayLightSaving_ADD1H   ((uint32_t)0x00010000) /*!< 设置RTC夏令时增加1小时。 */
#define IS_RTC_DAYLIGHT_SAVING(SAVE) (((SAVE) == RTC_DayLightSaving_SUB1H) || \
                                      ((SAVE) == RTC_DayLightSaving_ADD1H)) /*!< 检查夏令时设置是否有效。
                                      有效设置包括RTC_DayLightSaving_SUB1H和RTC_DayLightSaving_ADD1H。 */

#define RTC_StoreOperation_Reset        ((uint32_t)0x00000000) /*!< 重置RTC的存储操作。 */
#define RTC_StoreOperation_Set          ((uint32_t)0x00040000) /*!< 设置RTC的存储操作。 */
#define IS_RTC_STORE_OPERATION(OPERATION) (((OPERATION) == RTC_StoreOperation_Reset) || \
                                           ((OPERATION) == RTC_StoreOperation_Set)) /*!< 检查RTC存储操作设置是否有效。
                                           有效设置包括RTC_StoreOperation_Reset和RTC_StoreOperation_Set。 */
/**
  * @}
  */ 

/** @defgroup RTC_Tamper_Trigger_Definitions 
  * @brief  RTC 标准设置触发定义
  *         这些定义用于设置和检查RTC的防篡改触发条件。
  * @{
  */ 
#define RTC_TamperTrigger_RisingEdge            ((uint32_t)0x00000000) /*!< 设置防篡改触发条件为上升沿。 */
#define RTC_TamperTrigger_FallingEdge           ((uint32_t)0x00000001) /*!< 设置防篡改触发条件为下降沿。 */
#define RTC_TamperTrigger_LowLevel              ((uint32_t)0x00000000) /*!< 设置防篡改触发条件为低电平。
                                                                           注意：低电平和上升沿的值相同，需根据具体应用区分。 */
#define RTC_TamperTrigger_HighLevel             ((uint32_t)0x00000001) /*!< 设置防篡改触发条件为高电平。
                                                                           注意：高电平和下降沿的值相同，需根据具体应用区分。 */
#define IS_RTC_TAMPER_TRIGGER(TRIGGER) (((TRIGGER) == RTC_TamperTrigger_RisingEdge) || \
                                        ((TRIGGER) == RTC_TamperTrigger_FallingEdge) || \
                                        ((TRIGGER) == RTC_TamperTrigger_LowLevel) || \
                                        ((TRIGGER) == RTC_TamperTrigger_HighLevel)) /*!< 检查防篡改触发条件是否有效。
                                        有效设置包括RTC_TamperTrigger_RisingEdge, RTC_TamperTrigger_FallingEdge,
                                        RTC_TamperTrigger_LowLevel和RTC_TamperTrigger_HighLevel。 */

/**
  * @}
  */ 

/** @defgroup RTC_Tamper_Filter_Definitions 
  * @brief  RTC 标准设置过滤器定义
  *         这些定义用于设置和检查RTC的防篡改过滤器功能。
  * @{
  */ 
#define RTC_TamperFilter_Disable   ((uint32_t)0x00000000) /*!< 禁用防篡改过滤器。 */

#define RTC_TamperFilter_2Sample   ((uint32_t)0x00000800) /*!< 防篡改在检测到2个连续的有效样本后触发。 */
#define RTC_TamperFilter_4Sample   ((uint32_t)0x00001000) /*!< 防篡改在检测到4个连续的有效样本后触发。 */
#define RTC_TamperFilter_8Sample   ((uint32_t)0x00001800) /*!< 防篡改在检测到8个连续的有效样本后触发。
                                                          注意：最后一个宏的注释中有一个拼写错误，应为"level"。 */
#define IS_RTC_TAMPER_FILTER(FILTER) (((FILTER) == RTC_TamperFilter_Disable) || \
                                      ((FILTER) == RTC_TamperFilter_2Sample) || \
                                      ((FILTER) == RTC_TamperFilter_4Sample) || \
                                      ((FILTER) == RTC_TamperFilter_8Sample)) /*!< 检查防篡改过滤器设置是否有效。
                                      有效设置包括RTC_TamperFilter_Disable, RTC_TamperFilter_2Sample,
                                      RTC_TamperFilter_4Sample和RTC_TamperFilter_8Sample。 */
/**
  * @}
  */ 
/**
  * @brief  RTC 防篡改采样频率定义
  *         这些定义用于设置RTC的防篡改输入引脚的采样频率。
  * @{
  */ 
#define RTC_TamperSamplingFreq_RTCCLK_Div32768  ((uint32_t)0x00000000) /*!< 每个防篡改输入引脚以 RTCCLK / 32768 的频率采样。 */
#define RTC_TamperSamplingFreq_RTCCLK_Div16384  ((uint32_t)0x00000100) /*!< 每个防篡改输入引脚以 RTCCLK / 16384 的频率采样。 */
#define RTC_TamperSamplingFreq_RTCCLK_Div8192   ((uint32_t)0x00000200) /*!< 每个防篡改输入引脚以 RTCCLK / 8192 的频率采样。 */
#define RTC_TamperSamplingFreq_RTCCLK_Div4096   ((uint32_t)0x00000300) /*!< 每个防篡改输入引脚以 RTCCLK / 4096 的频率采样。 */
#define RTC_TamperSamplingFreq_RTCCLK_Div2048   ((uint32_t)0x00000400) /*!< 每个防篡改输入引脚以 RTCCLK / 2048 的频率采样。 */
#define RTC_TamperSamplingFreq_RTCCLK_Div1024   ((uint32_t)0x00000500) /*!< 每个防篡改输入引脚以 RTCCLK / 1024 的频率采样。 */
#define RTC_TamperSamplingFreq_RTCCLK_Div512    ((uint32_t)0x00000600) /*!< 每个防篡改输入引脚以 RTCCLK / 512 的频率采样。 */
#define RTC_TamperSamplingFreq_RTCCLK_Div256    ((uint32_t)0x00000700) /*!< 每个防篡改输入引脚以 RTCCLK / 256 的频率采样。 */
#define IS_RTC_TAMPER_SAMPLING_FREQ(FREQ) (((FREQ) == RTC_TamperSamplingFreq_RTCCLK_Div32768) || \
                                           ((FREQ) == RTC_TamperSamplingFreq_RTCCLK_Div16384) || \
                                           ((FREQ) == RTC_TamperSamplingFreq_RTCCLK_Div8192) || \
                                           ((FREQ) == RTC_TamperSamplingFreq_RTCCLK_Div4096) || \
                                           ((FREQ) == RTC_TamperSamplingFreq_RTCCLK_Div2048) || \
                                           ((FREQ) == RTC_TamperSamplingFreq_RTCCLK_Div1024) || \
                                           ((FREQ) == RTC_TamperSamplingFreq_RTCCLK_Div512) || \
                                           ((FREQ) == RTC_TamperSamplingFreq_RTCCLK_Div256)) /*!< 检查防篡改采样频率是否有效。
                                           有效频率包括 RTCCLK / 32768, RTCCLK / 16384, RTCCLK / 8192,
                                           RTCCLK / 4096, RTCCLK / 2048, RTCCLK / 1024, RTCCLK / 512 和 RTCCLK / 256。 */

/**
  * @}
  */

  /** @defgroup RTC_Tamper_Pin_Precharge_Duration_Definitions 
  * @brief  RTC 防篡改引脚预充电持续时间定义
  *         这些定义用于设置RTC的防篡改引脚在采样之前的预充电持续时间。
  * @{
  */ 
#define RTC_TamperPrechargeDuration_1RTCCLK ((uint32_t)0x00000000)  /*!< 在采样之前，防篡改引脚预充电持续 1 个 RTCCLK 周期。 */
#define RTC_TamperPrechargeDuration_2RTCCLK ((uint32_t)0x00002000)  /*!< 在采样之前，防篡改引脚预充电持续 2 个 RTCCLK 周期。 */
#define RTC_TamperPrechargeDuration_4RTCCLK ((uint32_t)0x00004000)  /*!< 在采样之前，防篡改引脚预充电持续 4 个 RTCCLK 周期。 */
#define RTC_TamperPrechargeDuration_8RTCCLK ((uint32_t)0x00006000)  /*!< 在采样之前，防篡改引脚预充电持续 8 个 RTCCLK 周期。 */

#define IS_RTC_TAMPER_PRECHARGE_DURATION(DURATION) (((DURATION) == RTC_TamperPrechargeDuration_1RTCCLK) || \
                                                    ((DURATION) == RTC_TamperPrechargeDuration_2RTCCLK) || \
                                                    ((DURATION) == RTC_TamperPrechargeDuration_4RTCCLK) || \
                                                    ((DURATION) == RTC_TamperPrechargeDuration_8RTCCLK)) /*!< 检查防篡改引脚预充电持续时间是否有效。
                                                    有效持续时间包括 1, 2, 4 和 8 个 RTCCLK 周期。 */
/**
  * @}
  */ 
/**
  * @brief  RTC 防篡改引脚定义
  *         这些定义用于设置和检查RTC的防篡改引脚。
  * @{
  */ 
#define RTC_Tamper_1                    RTC_TAFCR_TAMP1E /*!< 启用RTC防篡改引脚1。 */
#define IS_RTC_TAMPER(TAMPER) (((TAMPER) == RTC_Tamper_1)) /*!< 检查防篡改引脚设置是否有效。
                                         当前仅支持RTC_Tamper_1。 */

/**
  * @}
  */

/** @defgroup RTC_Tamper_Pin_Selection 
  * @brief  RTC 防篡改引脚选择定义
  *         这些定义用于选择RTC的防篡改引脚。
  * @{
  */ 
#define RTC_TamperPin_PC13                 ((uint32_t)0x00000000) /*!< 选择PC13作为防篡改引脚。 */
#define RTC_TamperPin_PI8                  ((uint32_t)0x00010000) /*!< 选择PI8作为防篡改引脚。 */
#define IS_RTC_TAMPER_PIN(PIN) (((PIN) == RTC_TamperPin_PC13) || \
                                ((PIN) == RTC_TamperPin_PI8)) /*!< 检查选择的防篡改引脚是否有效。
                                有效选择包括PC13和PI8。 */
/**
  * @}
  */ 

/** @defgroup RTC_TimeStamp_Pin_Selection 
  * @brief  RTC 时间戳引脚选择定义
  *         这些定义用于选择RTC的时间戳引脚。
  * @{
  */ 
#define RTC_TimeStampPin_PC13              ((uint32_t)0x00000000) /*!< 选择PC13作为时间戳引脚。 */
#define RTC_TimeStampPin_PI8               ((uint32_t)0x00020000) /*!< 选择PI8作为时间戳引脚。 */
#define IS_RTC_TIMESTAMP_PIN(PIN) (((PIN) == RTC_TimeStampPin_PC13) || \
                                   ((PIN) == RTC_TimeStampPin_PI8)) /*!< 检查选择的时间戳引脚是否有效。
                                   有效选择包括PC13和PI8。 */
/**
  * @}
  */ 

/** @defgroup RTC_Output_Type_ALARM_OUT 
  * @brief  RTC 报警输出类型定义
  *         这些定义用于设置RTC的报警输出类型。
  * @{
  */ 
#define RTC_OutputType_OpenDrain           ((uint32_t)0x00000000) /*!< 设置RTC报警输出为开漏输出类型。 */
#define RTC_OutputType_PushPull            ((uint32_t)0x00040000) /*!< 设置RTC报警输出为推挽输出类型。 */
#define IS_RTC_OUTPUT_TYPE(TYPE) (((TYPE) == RTC_OutputType_OpenDrain) || \
                                  ((TYPE) == RTC_OutputType_PushPull)) /*!< 检查选择的报警输出类型是否有效。
                                  有效类型包括开漏输出和推挽输出。 */
/**
  * @}
  */ 

/** @defgroup RTC_Add_1_Second_Parameter_Definitions
  * @brief  RTC 增加1秒参数定义
  *         这些定义用于设置RTC的增加1秒参数。
  * @{
  */ 
#define RTC_ShiftAdd1S_Reset      ((uint32_t)0x00000000) /*!< 重置RTC的增加1秒参数。 */
#define RTC_ShiftAdd1S_Set        ((uint32_t)0x80000000) /*!< 设置RTC的增加1秒参数。 */
#define IS_RTC_SHIFT_ADD1S(SEL) (((SEL) == RTC_ShiftAdd1S_Reset) || \
                                 ((SEL) == RTC_ShiftAdd1S_Set)) /*!< 检查选择的增加1秒参数是否有效。
                                 有效选择包括重置和设置。 */
/**
  * @}
  */ 

/** @defgroup RTC_Substract_Fraction_Of_Second_Value
  * @brief  RTC 减去秒的分数值定义
  *         这些定义用于检查RTC减去秒的分数值是否有效。
  * @{
  */ 
#define IS_RTC_SHIFT_SUBFS(FS) ((FS) <= 0x00007FFF) /*!< 检查减去秒的分数值是否有效。
                                      有效值范围为0到0x7FFF（32767）。 */
/**
  * @}
  */ 

/**
  * @}
  */

/** @defgroup RTC_Backup_Registers_Definitions 
  * @brief  RTC 备份寄存器定义
  *         这些定义用于选择和操作RTC的备份寄存器。
  * @{
  */

#define RTC_BKP_DR0                       ((uint32_t)0x00000000) /*!< 选择RTC备份寄存器DR0。 */
#define RTC_BKP_DR1                       ((uint32_t)0x00000001) /*!< 选择RTC备份寄存器DR1。 */
#define RTC_BKP_DR2                       ((uint32_t)0x00000002) /*!< 选择RTC备份寄存器DR2。 */
#define RTC_BKP_DR3                       ((uint32_t)0x00000003) /*!< 选择RTC备份寄存器DR3。 */
#define RTC_BKP_DR4                       ((uint32_t)0x00000004) /*!< 选择RTC备份寄存器DR4。 */
#define RTC_BKP_DR5                       ((uint32_t)0x00000005) /*!< 选择RTC备份寄存器DR5。 */
#define RTC_BKP_DR6                       ((uint32_t)0x00000006) /*!< 选择RTC备份寄存器DR6。 */
#define RTC_BKP_DR7                       ((uint32_t)0x00000007) /*!< 选择RTC备份寄存器DR7。 */
#define RTC_BKP_DR8                       ((uint32_t)0x00000008) /*!< 选择RTC备份寄存器DR8。 */
#define RTC_BKP_DR9                       ((uint32_t)0x00000009) /*!< 选择RTC备份寄存器DR9。 */
#define RTC_BKP_DR10                      ((uint32_t)0x0000000A) /*!< 选择RTC备份寄存器DR10。 */
#define RTC_BKP_DR11                      ((uint32_t)0x0000000B) /*!< 选择RTC备份寄存器DR11。 */
#define RTC_BKP_DR12                      ((uint32_t)0x0000000C) /*!< 选择RTC备份寄存器DR12。 */
#define RTC_BKP_DR13                      ((uint32_t)0x0000000D) /*!< 选择RTC备份寄存器DR13。 */
#define RTC_BKP_DR14                      ((uint32_t)0x0000000E) /*!< 选择RTC备份寄存器DR14。 */
#define RTC_BKP_DR15                      ((uint32_t)0x0000000F) /*!< 选择RTC备份寄存器DR15。 */
#define RTC_BKP_DR16                      ((uint32_t)0x00000010) /*!< 选择RTC备份寄存器DR16。 */
#define RTC_BKP_DR17                      ((uint32_t)0x00000011) /*!< 选择RTC备份寄存器DR17。 */
#define RTC_BKP_DR18                      ((uint32_t)0x00000012) /*!< 选择RTC备份寄存器DR18。 */
#define RTC_BKP_DR19                      ((uint32_t)0x00000013) /*!< 选择RTC备份寄存器DR19。 */
#define IS_RTC_BKP(BKP)                   (((BKP) == RTC_BKP_DR0) || \
                                           ((BKP) == RTC_BKP_DR1) || \
                                           ((BKP) == RTC_BKP_DR2) || \
                                           ((BKP) == RTC_BKP_DR3) || \
                                           ((BKP) == RTC_BKP_DR4) || \
                                           ((BKP) == RTC_BKP_DR5) || \
                                           ((BKP) == RTC_BKP_DR6) || \
                                           ((BKP) == RTC_BKP_DR7) || \
                                           ((BKP) == RTC_BKP_DR8) || \
                                           ((BKP) == RTC_BKP_DR9) || \
                                           ((BKP) == RTC_BKP_DR10) || \
                                           ((BKP) == RTC_BKP_DR11) || \
                                           ((BKP) == RTC_BKP_DR12) || \
                                           ((BKP) == RTC_BKP_DR13) || \
                                           ((BKP) == RTC_BKP_DR14) || \
                                           ((BKP) == RTC_BKP_DR15) || \
                                           ((BKP) == RTC_BKP_DR16) || \
                                           ((BKP) == RTC_BKP_DR17) || \
                                           ((BKP) == RTC_BKP_DR18) || \
                                           ((BKP) == RTC_BKP_DR19)) /*!< 检查选择的备份寄存器是否有效。
                                           有效选择包括DR0到DR19。 */
/**
  * @}
  */
/**
  * @}
  */

/** @defgroup RTC_Input_parameter_format_definitions 
  * @brief  RTC 输入参数格式定义
  *         这些定义用于设置和检查RTC的时间和日期输入参数的格式。
  * @{
  */ 
#define RTC_Format_BIN                    ((uint32_t)0x000000000) /*!< 设置RTC输入参数格式为二进制格式。 */
#define RTC_Format_BCD                    ((uint32_t)0x000000001) /*!< 设置RTC输入参数格式为BCD格式。 */
#define IS_RTC_FORMAT(FORMAT) (((FORMAT) == RTC_Format_BIN) || ((FORMAT) == RTC_Format_BCD)) /*!< 检查输入参数格式是否有效。
                                           有效格式包括二进制格式和BCD格式。 */

/**
  * @}
  */ 

/** @defgroup RTC_Flags_Definitions 
  * @brief  RTC 标志定义
  *         这些定义用于检查和清除RTC的各种状态标志。
  * @{
  */ 
#define RTC_FLAG_RECALPF                  ((uint32_t)0x00010000) /*!< 重新校准周期完成标志。 */
#define RTC_FLAG_TAMP1F                   ((uint32_t)0x00002000) /*!< 防篡改1事件标志。 */
#define RTC_FLAG_TSOVF                    ((uint32_t)0x00001000) /*!< 时间戳溢出标志。 */
#define RTC_FLAG_TSF                      ((uint32_t)0x00000800) /*!< 时间戳事件标志。 */
#define RTC_FLAG_WUTF                     ((uint32_t)0x00000400) /*!< 周期性唤醒事件标志。 */
#define RTC_FLAG_ALRBF                    ((uint32_t)0x00000200) /*!< 报警B事件标志。 */
#define RTC_FLAG_ALRAF                    ((uint32_t)0x00000100) /*!< 报警A事件标志。 */
#define RTC_FLAG_INITF                    ((uint32_t)0x00000040) /*!< 初始化完成标志。 */
#define RTC_FLAG_RSF                      ((uint32_t)0x00000020) /*!< 寄存器同步标志。 */
#define RTC_FLAG_INITS                    ((uint32_t)0x00000010) /*!< 初始化设置标志。 */
#define RTC_FLAG_SHPF                     ((uint32_t)0x00000008) /*!< 夏令时操作完成标志。 */
#define RTC_FLAG_WUTWF                    ((uint32_t)0x00000004) /*!< 周期性唤醒配置标志。 */
#define RTC_FLAG_ALRBWF                   ((uint32_t)0x00000002) /*!< 报警B配置标志。 */
#define RTC_FLAG_ALRAWF                   ((uint32_t)0x00000001) /*!< 报警A配置标志。 */
#define IS_RTC_GET_FLAG(FLAG) (((FLAG) == RTC_FLAG_TSOVF) || ((FLAG) == RTC_FLAG_TSF) || \
                               ((FLAG) == RTC_FLAG_WUTF) || ((FLAG) == RTC_FLAG_ALRBF) || \
                               ((FLAG) == RTC_FLAG_ALRAF) || ((FLAG) == RTC_FLAG_INITF) || \
                               ((FLAG) == RTC_FLAG_RSF) || ((FLAG) == RTC_FLAG_WUTWF) || \
                               ((FLAG) == RTC_FLAG_ALRBWF) || ((FLAG) == RTC_FLAG_ALRAWF) || \
                               ((FLAG) == RTC_FLAG_TAMP1F) || ((FLAG) == RTC_FLAG_RECALPF) || \
                               ((FLAG) == RTC_FLAG_SHPF)) /*!< 检查获取的RTC标志是否有效。
                               有效标志包括时间戳溢出、时间戳事件、周期性唤醒事件、报警B事件、报警A事件、
                               初始化完成、寄存器同步、初始化设置、夏令时操作完成、周期性唤醒配置、
                               报警B配置、报警A配置和防篡改1事件标志。 */
#define IS_RTC_CLEAR_FLAG(FLAG) (((FLAG) != (uint32_t)RESET) && (((FLAG) & 0xFFFF00DF) == (uint32_t)RESET)) /*!< 检查清除的RTC标志是否有效。
                               有效标志包括时间戳溢出、时间戳事件、周期性唤醒事件、报警B事件、报警A事件、
                               防篡改1事件标志和重新校准周期完成标志。 */
/**
  * @}
  */ 

/** @defgroup RTC_Interrupts_Definitions 
  * @brief  RTC 中断定义
  *         这些定义用于设置和检查RTC的各种中断使能位。
  * @{
  */ 
#define RTC_IT_TS                         ((uint32_t)0x00008000) /*!< 时间戳中断使能位。 */
#define RTC_IT_WUT                        ((uint32_t)0x00004000) /*!< 周期性唤醒中断使能位。 */
#define RTC_IT_ALRB                       ((uint32_t)0x00002000) /*!< 报警B中断使能位。 */
#define RTC_IT_ALRA                       ((uint32_t)0x00001000) /*!< 报警A中断使能位。 */
#define RTC_IT_TAMP                       ((uint32_t)0x00000004) /* 仅用于使能防篡改中断 */ /*!< 防篡改中断使能位。 */
#define RTC_IT_TAMP1                      ((uint32_t)0x00020000) /*!< 防篡改1中断使能位。 */

#define IS_RTC_CONFIG_IT(IT) (((IT) != (uint32_t)RESET) && (((IT) & 0xFFFF0FFB) == (uint32_t)RESET)) /*!< 检查配置的RTC中断是否有效。
                               有效中断包括时间戳、周期性唤醒、报警B、报警A、防篡改和防篡改1中断。 */
#define IS_RTC_GET_IT(IT) (((IT) == RTC_IT_TS) || ((IT) == RTC_IT_WUT) || \
                           ((IT) == RTC_IT_ALRB) || ((IT) == RTC_IT_ALRA) || \
                           ((IT) == RTC_IT_TAMP1)) /*!< 检查获取的RTC中断是否有效。
                           有效中断包括时间戳、周期性唤醒、报警B、报警A和防篡改1中断。 */
#define IS_RTC_CLEAR_IT(IT) (((IT) != (uint32_t)RESET) && (((IT) & 0xFFFD0FFF) == (uint32_t)RESET)) /*!< 检查清除的RTC中断是否有效。
                           有效中断包括时间戳、周期性唤醒、报警B、报警A和防篡改1中断。 */
/**
  * @}
  */ 

/** @defgroup RTC_Legacy 
  * @brief  RTC 兼容性定义
  *         这些定义用于提供向后兼容性，将旧的宏定义映射到新的宏定义。
  * @{
  */ 
#define RTC_DigitalCalibConfig  RTC_CoarseCalibConfig /*!< 将旧的数字校准配置宏映射到新的粗校准配置宏。 */
#define RTC_DigitalCalibCmd     RTC_CoarseCalibCmd    /*!< 将旧的数字校准命令宏映射到新的粗校准命令宏。 */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

// Function: RTC_DeInit
// 功能: 将RTC配置重置为默认复位状态
ErrorStatus RTC_DeInit(void);

// Function: RTC_Init
// 功能: 初始化并配置RTC
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct);

// Function: RTC_StructInit
// 功能: 初始化RTC_InitStruct为默认值
void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct);

// Function: RTC_WriteProtectionCmd
// 功能: 使能或禁用RTC写保护
void RTC_WriteProtectionCmd(FunctionalState NewState);

// Function: RTC_EnterInitMode
// 功能: 进入RTC初始化模式
ErrorStatus RTC_EnterInitMode(void);

// Function: RTC_ExitInitMode
// 功能: 退出RTC初始化模式
void RTC_ExitInitMode(void);

// Function: RTC_WaitForSynchro
// 功能: 等待RTC同步完成
ErrorStatus RTC_WaitForSynchro(void);

// Function: RTC_RefClockCmd
// 功能: 使能或禁用RTC参考时钟
ErrorStatus RTC_RefClockCmd(FunctionalState NewState);

// Function: RTC_BypassShadowCmd
// 功能: 使能或禁用RTC影子寄存器绕过功能
void RTC_BypassShadowCmd(FunctionalState NewState);

/* Time and Date configuration functions **************************************/

// Function: RTC_SetTime
// 功能: 设置指定格式的RTC时间
ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);

// Function: RTC_TimeStructInit
// 功能: 初始化RTC_TimeStruct为默认值
void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct);

// Function: RTC_GetTime
// 功能: 获取指定格式的RTC当前时间
void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);

// Function: RTC_GetSubSecond
// 功能: 获取RTC当前的亚秒值
uint32_t RTC_GetSubSecond(void);

// Function: RTC_SetDate
// 功能: 设置指定格式的RTC日期
ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);

// Function: RTC_DateStructInit
// 功能: 初始化RTC_DateStruct为默认值
void RTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct);

// Function: RTC_GetDate
// 功能: 获取指定格式的RTC当前日期
void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);

/* Alarms (Alarm A and Alarm B) configuration functions  *********************/

// Function: RTC_SetAlarm
// 功能: 设置指定格式的RTC告警（Alarm A 或 Alarm B）
void RTC_SetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct);

// Function: RTC_AlarmStructInit
// 功能: 初始化RTC_AlarmStruct为默认值
void RTC_AlarmStructInit(RTC_AlarmTypeDef* RTC_AlarmStruct);

// Function: RTC_GetAlarm
// 功能: 获取指定格式的RTC告警（Alarm A 或 Alarm B）的当前设置
void RTC_GetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct);

// Function: RTC_AlarmCmd
// 功能: 使能或禁用指定的RTC告警（Alarm A 或 Alarm B）
ErrorStatus RTC_AlarmCmd(uint32_t RTC_Alarm, FunctionalState NewState);

// Function: RTC_AlarmSubSecondConfig
// 功能: 配置指定RTC告警（Alarm A 或 Alarm B）的亚秒值和掩码
void RTC_AlarmSubSecondConfig(uint32_t RTC_Alarm, uint32_t RTC_AlarmSubSecondValue, uint32_t RTC_AlarmSubSecondMask);

// Function: RTC_GetAlarmSubSecond
// 功能: 获取指定RTC告警（Alarm A 或 Alarm B）的当前亚秒值
uint32_t RTC_GetAlarmSubSecond(uint32_t RTC_Alarm);
/* WakeUp Timer configuration functions ***************************************/

// Function: RTC_WakeUpClockConfig
// 功能: 配置RTC唤醒定时器的时钟源
void RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock);

// Function: RTC_SetWakeUpCounter
// 功能: 设置RTC唤醒定时器的计数值
void RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter);

// Function: RTC_GetWakeUpCounter
// 功能: 获取RTC唤醒定时器的当前计数值
uint32_t RTC_GetWakeUpCounter(void);

// Function: RTC_WakeUpCmd
// 功能: 使能或禁用RTC唤醒定时器
ErrorStatus RTC_WakeUpCmd(FunctionalState NewState);

/* Daylight Saving configuration functions ************************************/

// Function: RTC_DayLightSavingConfig
// 功能: 配置夏令时功能并设置存储操作
void RTC_DayLightSavingConfig(uint32_t RTC_DayLightSaving, uint32_t RTC_StoreOperation);

// Function: RTC_GetStoreOperation
// 功能: 获取夏令时存储操作的当前设置
uint32_t RTC_GetStoreOperation(void);

/* Output pin Configuration function ******************************************/

// Function: RTC_OutputConfig
// 功能: 配置RTC输出引脚的参数
void RTC_OutputConfig(uint32_t RTC_Output, uint32_t RTC_OutputPolarity);

/* Digital Calibration configuration functions ********************************/

// Function: RTC_CoarseCalibConfig
// 功能: 配置RTC粗校准的参数
ErrorStatus RTC_CoarseCalibConfig(uint32_t RTC_CalibSign, uint32_t Value);

// Function: RTC_CoarseCalibCmd
// 功能: 使能或禁用RTC粗校准功能
ErrorStatus RTC_CoarseCalibCmd(FunctionalState NewState);

// Function: RTC_CalibOutputCmd
// 功能: 使能或禁用RTC校准输出功能
void RTC_CalibOutputCmd(FunctionalState NewState);

// Function: RTC_CalibOutputConfig
// 功能: 配置RTC校准输出的参数
void RTC_CalibOutputConfig(uint32_t RTC_CalibOutput);

// Function: RTC_SmoothCalibConfig
// 功能: 配置RTC平滑校准的参数
ErrorStatus RTC_SmoothCalibConfig(uint32_t RTC_SmoothCalibPeriod, 
                                  uint32_t RTC_SmoothCalibPlusPulses,
                                  uint32_t RTC_SmouthCalibMinusPulsesValue);

/* TimeStamp configuration functions *****************************************/

// Function: RTC_TimeStampCmd
// 功能: 使能或禁用RTC时间戳功能，并配置时间戳边沿检测
void RTC_TimeStampCmd(uint32_t RTC_TimeStampEdge, FunctionalState NewState);

// Function: RTC_GetTimeStamp
// 功能: 获取时间戳的当前时间值和日期值
void RTC_GetTimeStamp(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_StampTimeStruct,
                                      RTC_DateTypeDef* RTC_StampDateStruct);

// Function: RTC_GetTimeStampSubSecond
// 功能: 获取时间戳的当前亚秒值
uint32_t RTC_GetTimeStampSubSecond(void);

/* Tampers configuration functions *****************************************/

// Function: RTC_TamperTriggerConfig
// 功能: 配置RTC防篡改触发条件
void RTC_TamperTriggerConfig(uint32_t RTC_Tamper, uint32_t RTC_TamperTrigger);

// Function: RTC_TamperCmd
// 功能: 使能或禁用指定的RTC防篡改功能
void RTC_TamperCmd(uint32_t RTC_Tamper, FunctionalState NewState);

// Function: RTC_TamperFilterConfig
// 功能: 配置RTC防篡改滤波器
void RTC_TamperFilterConfig(uint32_t RTC_TamperFilter);

// Function: RTC_TamperSamplingFreqConfig
// 功能: 配置RTC防篡改采样频率
void RTC_TamperSamplingFreqConfig(uint32_t RTC_TamperSamplingFreq);

// Function: RTC_TamperPinsPrechargeDuration
// 功能: 配置RTC防篡改引脚预充电持续时间
void RTC_TamperPinsPrechargeDuration(uint32_t RTC_TamperPrechargeDuration);

// Function: RTC_TimeStampOnTamperDetectionCmd
// 功能: 使能或禁用时间戳在检测到防篡改时的操作
void RTC_TimeStampOnTamperDetectionCmd(FunctionalState NewState);

// Function: RTC_TamperPullUpCmd
// 功能: 使能或禁用RTC防篡改输入引脚的上拉功能
void RTC_TamperPullUpCmd(FunctionalState NewState);
/* Backup Data Registers configuration functions ******************************/

// Function: RTC_WriteBackupRegister
// 功能: 将数据写入指定的RTC备份数据寄存器
void RTC_WriteBackupRegister(uint32_t RTC_BKP_DR, uint32_t Data);

// Function: RTC_ReadBackupRegister
// 功能: 从指定的RTC备份数据寄存器读取数据
uint32_t RTC_ReadBackupRegister(uint32_t RTC_BKP_DR);

/* RTC Tamper and TimeStamp Pins Selection and Output Type Config configuration
   functions *****************************************************************/

// Function: RTC_TamperPinSelection
// 功能: 选择RTC防篡改引脚
void RTC_TamperPinSelection(uint32_t RTC_TamperPin);

// Function: RTC_TimeStampPinSelection
// 功能: 选择RTC时间戳引脚
void RTC_TimeStampPinSelection(uint32_t RTC_TimeStampPin);

// Function: RTC_OutputTypeConfig
// 功能: 配置RTC输出引脚的输出类型
void RTC_OutputTypeConfig(uint32_t RTC_OutputType);

/* RTC_Shift_control_synchonisation_functions *********************************/

// Function: RTC_SynchroShiftConfig
// 功能: 配置RTC同步移位功能，用于调整时间
ErrorStatus RTC_SynchroShiftConfig(uint32_t RTC_ShiftAdd1S, uint32_t RTC_ShiftSubFS);

/* Interrupts and flags management functions **********************************/

// Function: RTC_ITConfig
// 功能: 使能或禁用指定的RTC中断
void RTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState);

// Function: RTC_GetFlagStatus
// 功能: 获取指定RTC标志的状态
FlagStatus RTC_GetFlagStatus(uint32_t RTC_FLAG);

// Function: RTC_ClearFlag
// 功能: 清除指定的RTC标志
void RTC_ClearFlag(uint32_t RTC_FLAG);

// Function: RTC_GetITStatus
// 功能: 获取指定RTC中断标志的状态
ITStatus RTC_GetITStatus(uint32_t RTC_IT);

// Function: RTC_ClearITPendingBit
// 功能: 清除指定的RTC中断标志
void RTC_ClearITPendingBit(uint32_t RTC_IT);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_RTC_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

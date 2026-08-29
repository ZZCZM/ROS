/**
  ******************************************************************************
  * @file    stm32f4xx_rtc.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Real-Time Clock (RTC) peripheral:
  *           + Initialization
  *           + Calendar (Time and Date) configuration
  *           + Alarms (Alarm A and Alarm B) configuration
  *           + WakeUp Timer configuration
  *           + Daylight Saving configuration
  *           + Output pin Configuration
  *           + Coarse digital Calibration configuration
  *           + Smooth digital Calibration configuration
  *           + TimeStamp configuration
  *           + Tampers configuration
  *           + Backup Data Registers configuration
  *           + Shift control synchronisation    
  *           + RTC Tamper and TimeStamp Pins Selection and Output Type Config configuration
  *           + Interrupts and flags management
  *
@verbatim

 ===================================================================
              ##### Backup Domain Operating Condition #####
 ===================================================================
 [..] The real-time clock (RTC), the RTC backup registers, and the backup 
      SRAM (BKP SRAM) can be powered from the VBAT voltage when the main 
      VDD supply is powered off.
      To retain the content of the RTC backup registers, backup SRAM, and supply 
      the RTC when VDD is turned off, VBAT pin can be connected to an optional 
      standby voltage supplied by a battery or by another source.

 [..] To allow the RTC to operate even when the main digital supply (VDD) is turned
      off, the VBAT pin powers the following blocks:
   (#) The RTC
   (#) The LSE oscillator
   (#) The backup SRAM when the low power backup regulator is enabled
   (#) PC13 to PC15 I/Os, plus PI8 I/O (when available)
  
 [..] When the backup domain is supplied by VDD (analog switch connected to VDD),
      the following functions are available:
   (#) PC14 and PC15 can be used as either GPIO or LSE pins
   (#) PC13 can be used as a GPIO or as the RTC_AF1 pin
   (#) PI8 can be used as a GPIO or as the RTC_AF2 pin
  
 [..] When the backup domain is supplied by VBAT (analog switch connected to VBAT 
      because VDD is not present), the following functions are available:
   (#) PC14 and PC15 can be used as LSE pins only
   (#) PC13 can be used as the RTC_AF1 pin 
   (#) PI8 can be used as the RTC_AF2 pin
  
            
                   ##### Backup Domain Reset #####
 ===================================================================
 [..] The backup domain reset sets all RTC registers and the RCC_BDCR register 
      to their reset values. The BKPSRAM is not affected by this reset. The only
      way of resetting the BKPSRAM is through the Flash interface by requesting 
      a protection level change from 1 to 0.
 [..] A backup domain reset is generated when one of the following events occurs:
   (#) Software reset, triggered by setting the BDRST bit in the 
       RCC Backup domain control register (RCC_BDCR). You can use the
       RCC_BackupResetCmd().
   (#) VDD or VBAT power on, if both supplies have previously been powered off.
  

                   ##### Backup Domain Access #####
 ===================================================================
 [..] After reset, the backup domain (RTC registers, RTC backup data 
      registers and backup SRAM) is protected against possible unwanted write 
      accesses. 
 [..] To enable access to the RTC Domain and RTC registers, proceed as follows:
   (+) Enable the Power Controller (PWR) APB1 interface clock using the
       RCC_APB1PeriphClockCmd() function.
   (+) Enable access to RTC domain using the PWR_BackupAccessCmd() function.
   (+) Select the RTC clock source using the RCC_RTCCLKConfig() function.
   (+) Enable RTC Clock using the RCC_RTCCLKCmd() function.
  
  
                  ##### How to use RTC Driver #####
 ===================================================================
 [..] 
   (+) Enable the RTC domain access (see description in the section above)
   (+) Configure the RTC Prescaler (Asynchronous and Synchronous) and RTC hour 
       format using the RTC_Init() function.
  
 *** Time and Date configuration ***
 ===================================
 [..] 
   (+) To configure the RTC Calendar (Time and Date) use the RTC_SetTime()
       and RTC_SetDate() functions.
   (+) To read the RTC Calendar, use the RTC_GetTime() and RTC_GetDate() functions.
   (+) Use the RTC_DayLightSavingConfig() function to add or sub one
       hour to the RTC Calendar.    
  
 *** Alarm configuration ***
 ===========================
 [..]
   (+) To configure the RTC Alarm use the RTC_SetAlarm() function.
   (+) Enable the selected RTC Alarm using the RTC_AlarmCmd() function
   (+) To read the RTC Alarm, use the RTC_GetAlarm() function.
   (+) To read the RTC alarm SubSecond, use the RTC_GetAlarmSubSecond() function.
  
 *** RTC Wakeup configuration ***
 ================================
 [..] 
   (+) Configure the RTC Wakeup Clock source use the RTC_WakeUpClockConfig()
       function.
   (+) Configure the RTC WakeUp Counter using the RTC_SetWakeUpCounter() function  
   (+) Enable the RTC WakeUp using the RTC_WakeUpCmd() function  
   (+) To read the RTC WakeUp Counter register, use the RTC_GetWakeUpCounter() 
       function.
  
 *** Outputs configuration ***
 =============================
 [..] The RTC has 2 different outputs:
   (+) AFO_ALARM: this output is used to manage the RTC Alarm A, Alarm B
       and WaKeUp signals. To output the selected RTC signal on RTC_AF1 pin, use the 
       RTC_OutputConfig() function.                
   (+) AFO_CALIB: this output is 512Hz signal or 1Hz. To output the RTC Clock on 
       RTC_AF1 pin, use the RTC_CalibOutputCmd() function.
  
 *** Smooth digital Calibration configuration ***
 ================================================    
 [..]
   (+) Configure the RTC Original Digital Calibration Value and the corresponding
       calibration cycle period (32s,16s and 8s) using the RTC_SmoothCalibConfig() 
       function.
  
 *** Coarse digital Calibration configuration ***
 ================================================
 [..]
   (+) Configure the RTC Coarse Calibration Value and the corresponding
       sign using the RTC_CoarseCalibConfig() function.
   (+) Enable the RTC Coarse Calibration using the RTC_CoarseCalibCmd() function  
  
 *** TimeStamp configuration ***
 ===============================
 [..]
   (+) Configure the RTC_AF1 trigger and enables the RTC TimeStamp using the RTC
      _TimeStampCmd() function.
   (+) To read the RTC TimeStamp Time and Date register, use the RTC_GetTimeStamp()
       function.
   (+) To read the RTC TimeStamp SubSecond register, use the 
       RTC_GetTimeStampSubSecond() function.
   (+) The TAMPER1 alternate function can be mapped either to RTC_AF1(PC13)
       or RTC_AF2 (PI8) depending on the value of TAMP1INSEL bit in 
       RTC_TAFCR register. You can use the  RTC_TamperPinSelection() function to
       select the corresponding pin.     
  
 *** Tamper configuration ***
 ============================
 [..]
   (+) Enable the RTC Tamper using the RTC_TamperCmd() function.
   (+) Configure the Tamper filter count using RTC_TamperFilterConfig()
       function. 
   (+) Configure the RTC Tamper trigger Edge or Level according to the Tamper 
       filter (if equal to 0 Edge else Level) value using the RTC_TamperConfig() 
       function.
   (+) Configure the Tamper sampling frequency using RTC_TamperSamplingFreqConfig()
       function.
   (+) Configure the Tamper precharge or discharge duration using 
       RTC_TamperPinsPrechargeDuration() function.
   (+) Enable the Tamper Pull-UP using RTC_TamperPullUpDisableCmd() function.
   (+) Enable the Time stamp on Tamper detection event using  
       TC_TSOnTamperDetecCmd() function.
   (+) The TIMESTAMP alternate function can be mapped to either RTC_AF1 
       or RTC_AF2 depending on the value of the TSINSEL bit in the RTC_TAFCR 
       register. You can use the  RTC_TimeStampPinSelection() function to select 
       the corresponding pin. 
  
 *** Backup Data Registers configuration ***
 ===========================================
 [..]
   (+) To write to the RTC Backup Data registers, use the RTC_WriteBackupRegister()
       function.  
   (+) To read the RTC Backup Data registers, use the RTC_ReadBackupRegister()
       function.
   

                  ##### RTC and low power modes #####
 ===================================================================
 [..] The MCU can be woken up from a low power mode by an RTC alternate 
      function.
 [..] The RTC alternate functions are the RTC alarms (Alarm A and Alarm B), 
      RTC wakeup, RTC tamper event detection and RTC time stamp event detection.
      These RTC alternate functions can wake up the system from the Stop and 
      Standby lowpower modes.
 [..] The system can also wake up from low power modes without depending 
      on an external interrupt (Auto-wakeup mode), by using the RTC alarm 
      or the RTC wakeup events.
 [..] The RTC provides a programmable time base for waking up from the 
      Stop or Standby mode at regular intervals.
      Wakeup from STOP and Standby modes is possible only when the RTC clock source
      is LSE or LSI.
  

          ##### Selection of RTC_AF1 alternate functions #####
 ===================================================================
 [..] The RTC_AF1 pin (PC13) can be used for the following purposes:
   (+) AFO_ALARM output
   (+) AFO_CALIB output
   (+) AFI_TAMPER
   (+) AFI_TIMESTAMP
 
 [..]   
   +-------------------------------------------------------------------------------------------------------------+
   |     Pin         |AFO_ALARM |AFO_CALIB |AFI_TAMPER |AFI_TIMESTAMP | TAMP1INSEL |   TSINSEL    |ALARMOUTTYPE  |
   |  configuration  | ENABLED  | ENABLED  |  ENABLED  |   ENABLED    |TAMPER1 pin |TIMESTAMP pin |  AFO_ALARM   |
   |  and function   |          |          |           |              | selection  |  selection   |Configuration |
   |-----------------|----------|----------|-----------|--------------|------------|--------------|--------------|
   |   Alarm out     |          |          |           |              |    Don't   |     Don't    |              |
   |   output OD     |     1    |Don't care|Don't care | Don't care   |    care    |     care     |      0       |
   |-----------------|----------|----------|-----------|--------------|------------|--------------|--------------|
   |   Alarm out     |          |          |           |              |    Don't   |     Don't    |              |
   |   output PP     |     1    |Don't care|Don't care | Don't care   |    care    |     care     |      1       |
   |-----------------|----------|----------|-----------|--------------|------------|--------------|--------------|
   | Calibration out |          |          |           |              |    Don't   |     Don't    |              |
   |   output PP     |     0    |    1     |Don't care | Don't care   |    care    |     care     |  Don't care  |
   |-----------------|----------|----------|-----------|--------------|------------|--------------|--------------|
   |  TAMPER input   |          |          |           |              |            |     Don't    |              |
   |   floating      |     0    |    0     |     1     |      0       |      0     |     care     |  Don't care  |
   |-----------------|----------|----------|-----------|--------------|------------|--------------|--------------|
   |  TIMESTAMP and  |          |          |           |              |            |              |              |
   |  TAMPER input   |     0    |    0     |     1     |      1       |      0     |      0       |  Don't care  |
   |   floating      |          |          |           |              |            |              |              |
   |-----------------|----------|----------|-----------|--------------|------------|--------------|--------------|
   | TIMESTAMP input |          |          |           |              |    Don't   |              |              |
   |    floating     |     0    |    0     |     0     |      1       |    care    |      0       |  Don't care  |
   |-----------------|----------|----------|-----------|--------------|------------|--------------|--------------|
   |  Standard GPIO  |     0    |    0     |     0     |      0       | Don't care |  Don't care  |  Don't care  |
   +-------------------------------------------------------------------------------------------------------------+

            
        #####  Selection of RTC_AF2 alternate functions #####
 ===================================================================
 [..] The RTC_AF2 pin (PI8) can be used for the following purposes:
   (+) AFI_TAMPER
   (+) AFI_TIMESTAMP
 [..]
   +---------------------------------------------------------------------------------------+
   |     Pin         |AFI_TAMPER |AFI_TIMESTAMP | TAMP1INSEL |   TSINSEL    |ALARMOUTTYPE  |
   |  configuration  |  ENABLED  |   ENABLED    |TAMPER1 pin |TIMESTAMP pin |  AFO_ALARM   |
   |  and function   |           |              | selection  |  selection   |Configuration |
   |-----------------|-----------|--------------|------------|--------------|--------------|
   |  TAMPER input   |           |              |            |     Don't    |              |
   |   floating      |     1     |      0       |      1     |     care     |  Don't care  |
   |-----------------|-----------|--------------|------------|--------------|--------------|
   |  TIMESTAMP and  |           |              |            |              |              |
   |  TAMPER input   |     1     |      1       |      1     |      1       |  Don't care  |
   |   floating      |           |              |            |              |              |
   |-----------------|-----------|--------------|------------|--------------|--------------|
   | TIMESTAMP input |           |              |    Don't   |              |              |
   |    floating     |     0     |      1       |    care    |      1       |  Don't care  |
   |-----------------|-----------|--------------|------------|--------------|--------------|
   |  Standard GPIO  |     0     |      0       | Don't care |  Don't care  |  Don't care  |
   +---------------------------------------------------------------------------------------+   
 
     
@endverbatim
  
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_rtc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup RTC 
  * @brief RTC driver modules
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/******************** RTC寄存器保留位掩码定义 ********************/
/* 时间寄存器(TR)保留位掩码：屏蔽不需要操作的位
   二进制: 0000 0000 0111 1111 0111 1111 0111 1111 
   作用位: 
   - 小时字段高位(bit19) 
   - 保留位(bit15)
   - 分钟字段高位(bit14)
   - 保留位(bit7)
   - 秒字段高位(bit6) */
#define RTC_TR_RESERVED_MASK    ((uint32_t)0x007F7F7F)

/* 日期寄存器(DR)保留位掩码：屏蔽不需要操作的位
   二进制: 0000 0000 1111 1111 1111 1111 0011 1111
   作用位:
   - 年份字段高位(bit23)
   - 月份字段高位(bit19)
   - 日期字段高位(bit15) */
#define RTC_DR_RESERVED_MASK    ((uint32_t)0x00FFFF3F) 

/* 初始化模式全掩码：用于检查所有初始化相关位 */
#define RTC_INIT_MASK           ((uint32_t)0xFFFFFFFF)  

/* 寄存器同步标志掩码：保留同步状态位(bit5) */
#define RTC_RSF_MASK            ((uint32_t)0xFFFFFF5F)

/* 全状态标志组合掩码：包含所有RTC状态标志位
   组成标志：
   - 时间戳溢出(TSOVF) | 时间戳标志(TSF)
   - 唤醒定时器标志(WUTF) | 闹钟B标志(ALRBF) 
   - 闹钟A标志(ALRAF) | 初始化模式标志(INITF)
   - 寄存器同步标志(RSF) | 初始化状态(INITS)
   - 唤醒定时器写标志(WUTWF) | 闹钟B写标志(ALRBWF)
   - 闹钟A写标志(ALRAWF) | 入侵事件标志(TAMP1F)
   - 校准挂起标志(RECALPF) | 移位操作标志(SHPF) */
#define RTC_FLAGS_MASK          ((uint32_t)(RTC_FLAG_TSOVF | RTC_FLAG_TSF | RTC_FLAG_WUTF | \
                                            RTC_FLAG_ALRBF | RTC_FLAG_ALRAF | RTC_FLAG_INITF | \
                                            RTC_FLAG_RSF | RTC_FLAG_INITS | RTC_FLAG_WUTWF | \
                                            RTC_FLAG_ALRBWF | RTC_FLAG_ALRAWF | RTC_FLAG_TAMP1F | \
                                            RTC_FLAG_RECALPF | RTC_FLAG_SHPF))

/******************** 超时配置参数 ********************/
/* 初始化模式最大等待周期数（约65536次循环） */
#define INITMODE_TIMEOUT         ((uint32_t) 0x00010000)

/* 寄存器同步最大等待周期数 */
#define SYNCHRO_TIMEOUT          ((uint32_t) 0x00020000)

/* 校准操作最大等待周期数 */
#define RECALPF_TIMEOUT          ((uint32_t) 0x00020000)

/* 移位操作最大等待周期数（约4096次循环） */
#define SHPF_TIMEOUT             ((uint32_t) 0x00001000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/******************** BCD转换函数声明 ********************/
/* 十进制转BCD码：将0-99的十进制数转换为BCD格式 
   参数：Value 十进制数值（0-99）
   返回：8位BCD码（高4位为十位，低4位为个位） */
static uint8_t RTC_ByteToBcd2(uint8_t Value);

/* BCD码转十进制：将BCD格式数据转换为0-99的十进制数
   参数：Value BCD码数值（0x00-0x99）
   返回：对应的十进制数值 */
static uint8_t RTC_Bcd2ToByte(uint8_t Value);


/* Private functions ---------------------------------------------------------*/

/** @defgroup RTC_Private_Functions
  * @{
  */ 

/** @defgroup RTC_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
             ##### Initialization and Configuration functions #####
 ===============================================================================
 
 [..] This section provide functions allowing to initialize and configure the RTC
      Prescaler (Synchronous and Asynchronous), RTC Hour format, disable RTC registers
      Write protection, enter and exit the RTC initialization mode, RTC registers
      synchronization check and reference clock detection enable.
  
   (#) The RTC Prescaler is programmed to generate the RTC 1Hz time base. It is
       split into 2 programmable prescalers to minimize power consumption.
       (++) A 7-bit asynchronous prescaler and A 13-bit synchronous prescaler.
       (++) When both prescalers are used, it is recommended to configure the 
            asynchronous prescaler to a high value to minimize consumption.

   (#) All RTC registers are Write protected. Writing to the RTC registers
       is enabled by writing a key into the Write Protection register, RTC_WPR.

   (#) To Configure the RTC Calendar, user application should enter initialization
       mode. In this mode, the calendar counter is stopped and its value can be 
       updated. When the initialization sequence is complete, the calendar restarts 
       counting after 4 RTCCLK cycles.

   (#) To read the calendar through the shadow registers after Calendar initialization,
       calendar update or after wakeup from low power modes the software must first 
       clear the RSF flag. The software must then wait until it is set again before 
       reading the calendar, which means that the calendar registers have been 
       correctly copied into the RTC_TR and RTC_DR shadow registers.
       The RTC_WaitForSynchro() function implements the above software sequence 
       (RSF clear and RSF check).

@endverbatim
  * @{
  */
/**
  * @brief  RTC寄存器复位函数
  * @功能  将RTC核心寄存器恢复至默认值，不影响时钟源和备份寄存器
  * @返回值 ErrorStatus: 操作状态（SUCCESS/ERROR）
  * @注意  该操作：
  *        - 保留RTC时钟配置（LSE/LSI等）
  *        - 保持备份域寄存器内容
  *        - 需要先进入初始化模式
  */
ErrorStatus RTC_DeInit(void)
{
  __IO uint32_t wutcounter = 0x00;    // 唤醒定时器写标志等待计数器
  uint32_t wutwfstatus = 0x00;       // 唤醒定时器写标志状态缓存
  ErrorStatus status = ERROR;         // 操作状态变量

  /* 步骤1：解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一阶段解锁码
  RTC->WPR = 0x53;  // 第二阶段解锁码

  /* 步骤2：进入初始化模式 */
  if (RTC_EnterInitMode() == ERROR)  // 尝试进入初始化模式
  {
    status = ERROR;  // 模式进入失败直接返回错误
  }  
  else
  {
    /* 步骤3：重置基础寄存器 */
    RTC->TR = (uint32_t)0x00000000;  // 时间寄存器清零（00:00:00）
    RTC->DR = (uint32_t)0x00002101;  // 日期寄存器设为默认值（01日-01月-2000年）
    /* 控制寄存器保留最低3位（WUTE/ALRBE/ALRAE） */
    RTC->CR &= (uint32_t)0x00000007; 

    /* 步骤4：等待唤醒定时器可写标志（WUTWF） */
    do
    {
      wutwfstatus = RTC->ISR & RTC_ISR_WUTWF;  // 读取标志位
      wutcounter++;   // 循环计数器递增
    } while((wutcounter != INITMODE_TIMEOUT) && (wutwfstatus == 0x00));  // 超时或标志置位退出

    /* 步骤5：超时检测 */
    if ((RTC->ISR & RTC_ISR_WUTWF) == RESET)
    {
      status = ERROR; // 超时未就绪返回错误
    }
    else
    {
      /* 步骤6：重置所有功能寄存器 */
      RTC->CR &= (uint32_t)0x00000000;       // 关闭所有控制功能
      RTC->WUTR = (uint32_t)0x0000FFFF;      // 唤醒定时器设为最大值
      RTC->PRER = (uint32_t)0x007F00FF;      // 异步分频127，同步分频255
      RTC->CALIBR = (uint32_t)0x00000000;    // 校准寄存器清零
      RTC->ALRMAR = (uint32_t)0x00000000;    // 闹钟A寄存器禁用    
      RTC->ALRMBR = (uint32_t)0x00000000;    // 闹钟B寄存器禁用
      RTC->SHIFTR = (uint32_t)0x00000000;    // 移位功能关闭
      RTC->CALR = (uint32_t)0x00000000;      // 校准寄存器复位
      RTC->ALRMASSR = (uint32_t)0x00000000;  // 闹钟A子秒寄存器清零
      RTC->ALRMBSSR = (uint32_t)0x00000000;  // 闹钟B子秒寄存器清零
      
      /* 步骤7：复位状态寄存器并退出初始化模式 */
      RTC->ISR = (uint32_t)0x00000000;       // 清除所有状态标志
      
      /* 入侵检测配置复位 */
      RTC->TAFCR = 0x00000000;  // 关闭所有tamper检测功能
  
      /* 步骤8：等待寄存器同步 */
      if(RTC_WaitForSynchro() == ERROR)  // 等待硬件同步完成
      {
        status = ERROR;
      }
      else
      {
        status = SUCCESS;  // 全部复位成功      
      }
    }
  }
  
  /* 步骤9：重新使能写保护 */
  RTC->WPR = 0xFF;  // 写入任意值重新激活写保护
  
  return status;
}

/**
  * @brief  RTC初始化函数
  * @功能  根据配置结构体初始化RTC核心参数：
  *        - 小时格式（12/24小时制）
  *        - 异步/同步预分频系数
  * @参数  RTC_InitStruct: 包含配置参数的结构体指针
  * @返回值 ErrorStatus: 操作状态（SUCCESS/ERROR）
  * @注意  预分频寄存器(PRER)必须在初始化模式下配置
  */
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct)
{
  ErrorStatus status = ERROR;
  
  /* 参数合法性检查 */
  assert_param(IS_RTC_HOUR_FORMAT(RTC_InitStruct->RTC_HourFormat));      // 12/24小时格式验证
  assert_param(IS_RTC_ASYNCH_PREDIV(RTC_InitStruct->RTC_AsynchPrediv)); // 异步分频值0-0x7F
  assert_param(IS_RTC_SYNCH_PREDIV(RTC_InitStruct->RTC_SynchPrediv));    // 同步分频值0-0x7FFF

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一阶段解锁码
  RTC->WPR = 0x53;  // 第二阶段解锁码

  /* 进入初始化模式 */
  if (RTC_EnterInitMode() == ERROR)  // 尝试进入初始化模式
  {
    status = ERROR;  // 模式进入失败直接返回错误
  } 
  else
  {
    /* 配置小时格式（CR寄存器FMT位） */
    RTC->CR &= ~RTC_CR_FMT;          // 清除原有格式设置（位6）
    RTC->CR |= RTC_InitStruct->RTC_HourFormat;  // 应用新格式（RTC_HourFormat_12/24）
  
    /* 配置预分频寄存器（PRER） */
    RTC->PRER = (uint32_t)(RTC_InitStruct->RTC_SynchPrediv);         // 同步预分频低16位
    RTC->PRER |= (uint32_t)(RTC_InitStruct->RTC_AsynchPrediv << 16); // 异步预分频高16位

    /* 退出初始化模式 */
    RTC_ExitInitMode();  // 自动触发硬件同步

    status = SUCCESS;     // 标记操作成功
  }
  /* 重新使能写保护 */
  RTC->WPR = 0xFF;  // 写入任意值激活保护
  
  return status;
}

/**
  * @brief  RTC配置结构体初始化函数
  * @功能  设置RTC初始化参数的默认值：
  *        - 24小时格式
  *        - 异步预分频127（RTC时钟通常为32768Hz时产生1Hz）
  *        - 同步预分频255
  * @参数  RTC_InitStruct: 待初始化的结构体指针
  */
void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct)
{
  /* 设置24小时制 */
  RTC_InitStruct->RTC_HourFormat = RTC_HourFormat_24;
    
  /* 异步预分频默认值（0x7F对应分频系数128） */
  RTC_InitStruct->RTC_AsynchPrediv = 0x7F;  // 实际分频值 = AsynchPrediv + 1 

  /* 同步预分频默认值（0xFF对应分频系数256） */ 
  RTC_InitStruct->RTC_SynchPrediv = 0xFF;  // 实际分频值 = SynchPrediv + 1
}
/**
  * @brief  RTC写保护控制函数
  * @功能  启用或禁用RTC寄存器的写保护机制
  * @参数  NewState: 新状态（ENABLE-启用保护，DISABLE-禁用）
  * @注意  写保护影响范围：
  *        - 保护所有RTC寄存器（除ISR[13:8]、TAFCR、BKPxR）
  *        - 错误写入将重新激活保护
  *        - 系统复位不影响保护状态
  */
void RTC_WriteProtectionCmd(FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
    
  if (NewState != DISABLE)
  {
    /* 启用写保护：向WPR写入任意非密钥值 */
    RTC->WPR = 0xFF;   // 激活写保护，禁止寄存器修改
  }
  else
  {
    /* 禁用写保护：写入双阶段解锁密钥 */
    RTC->WPR = 0xCA;   // 第一阶段解锁码
    RTC->WPR = 0x53;   // 第二阶段解锁码（必须在50ms内完成）
  }
}

/**
  * @brief  RTC进入初始化模式函数
  * @功能  使RTC进入配置模式以修改关键寄存器
  * @返回值 ErrorStatus: 操作状态（SUCCESS/ERROR）
  * @注意  前置条件：
  *        - 必须禁用写保护
  *        - 初始化模式最多持续4个RTCCLK周期
  */
ErrorStatus RTC_EnterInitMode(void)
{
  __IO uint32_t initcounter = 0x00;  // 初始化模式等待计数器
  ErrorStatus status = ERROR;        // 操作状态
  uint32_t initstatus = 0x00;        // INITF标志状态缓存
     
  /* 检查是否已处于初始化模式 */
  if ((RTC->ISR & RTC_ISR_INITF) == RESET)
  {
    /* 设置初始化模式请求 */
    RTC->ISR = RTC_INIT_MASK;  // 写入初始化模式请求码
    
    /* 等待INITF标志置位 */
    do
    {
      initstatus = RTC->ISR & RTC_ISR_INITF; // 读取状态标志
      initcounter++;  
    } while((initcounter < INITMODE_TIMEOUT) && (initstatus == 0x00));
    
    /* 验证是否成功进入模式 */
    if ((RTC->ISR & RTC_ISR_INITF) != RESET)
    {
      status = SUCCESS;  // 成功进入初始化模式
    }
    else
    {
      status = ERROR;    // 超时未进入模式
    }        
  }
  else
  {
    status = SUCCESS;    // 已处于初始化模式
  } 
    
  return status;  
}

/**
  * @brief  RTC退出初始化模式函数
  * @功能  退出配置模式并使新设置生效
  * @注意  退出后需4个RTCCLK周期日历才会更新
  */
void RTC_ExitInitMode(void)
{ 
  /* 清除初始化模式位 */
  RTC->ISR &= ~RTC_ISR_INIT;  // 清除ISR寄存器bit7
}

/**
  * @brief  等待RTC时间日期寄存器与APB时钟同步
  * @note 使用场景：
  *        - RTC初始化后
  *        - 日历更新操作后
  *        - 从低功耗模式唤醒后
  * @retval 同步状态：
  *          SUCCESS(同步成功)/ERROR(同步超时)
  * @warning 操作流程：
  *          1. 禁用RTC写保护
  *          2. 清除RSF同步标志
  *          3. 轮询等待RSF置位
  *          4. 恢复写保护
  */
ErrorStatus RTC_WaitForSynchro(void)
{
  __IO uint32_t synchrocounter = 0;  // 同步等待计数器
  ErrorStatus status = ERROR;         // 默认返回错误状态
  uint32_t synchrostatus = 0x00;      // 同步状态暂存

  /* 步骤1：解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一阶段解锁密钥
  RTC->WPR = 0x53;  // 第二阶段解锁密钥（必须顺序写入）

  /* 步骤2：清除寄存器同步标志（RSF bit5） */
  RTC->ISR &= (uint32_t)RTC_RSF_MASK;  // RTC_RSF_MASK = ~RTC_ISR_RSF

  /* 步骤3：轮询等待同步完成（最大等待SYNCHRO_TIMEOUT次） */
  do
  {
    synchrostatus = RTC->ISR & RTC_ISR_RSF;  // 读取RSF标志位
    synchrocounter++;                         // 增加超时计数器
  } while((synchrocounter != SYNCHRO_TIMEOUT) && (synchrostatus == 0x00));

  /* 步骤4：验证同步结果 */
  if ((RTC->ISR & RTC_ISR_RSF) != RESET)  // 检查RSF是否置位
  {
    status = SUCCESS;  // 同步成功
  }
  else
  {
    status = ERROR;    // 同步超时
  }        

  /* 步骤5：重新启用RTC写保护 */
  RTC->WPR = 0xFF;  // 任意非密钥值即可锁定

  return (status); 
}
/**
  * @brief  控制RTC参考时钟检测功能
  * @param  NewState: 参考时钟检测的新状态(ENABLE/DISABLE)
  * @retval 操作状态:
  *          - SUCCESS: 成功启用参考时钟检测
  *          - ERROR:   无法进入初始化模式或操作失败
  * @details 操作流程：
  *          1. 解除RTC寄存器写保护
  *          2. 进入初始化模式
  *          3. 配置REFCKON控制位
  *          4. 退出初始化模式
  *          5. 重新启用写保护
  */
ErrorStatus RTC_RefClockCmd(FunctionalState NewState)
{ 
  ErrorStatus status = ERROR;
  
  /* 参数合法性检查，确保NewState是有效功能状态(ENABLE/DISABLE) */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 解除RTC寄存器写保护（STM32要求连续写入两个特定解锁值）*/
  RTC->WPR = 0xCA;  // 第一步解锁值
  RTC->WPR = 0x53;  // 第二步解锁值
    
  /* 尝试进入初始化模式（此时可以修改RTC配置寄存器）*/
  if (RTC_EnterInitMode() == ERROR)
  {
    status = ERROR;  // 进入失败直接返回错误
  } 
  else
  {  
    /* 根据参数设置REFCKON控制位 */
    if (NewState != DISABLE)
    {
      /* 启用参考时钟检测：设置CR寄存器的REFCKON位 */
      RTC->CR |= RTC_CR_REFCKON;   // 使用位或操作开启特定功能位
    }
    else
    {
      /* 禁用参考时钟检测：清除CR寄存器的REFCKON位 */
      RTC->CR &= ~RTC_CR_REFCKON;  // 使用位与操作清除特定功能位
    }
    
    /* 退出初始化模式（锁定配置修改） */
    RTC_ExitInitMode();
    
    status = SUCCESS;  // 标记操作成功
  }
  
  /* 重新启用RTC寄存器写保护（写入任意非解锁值即可） */
  RTC->WPR = 0xFF;  
  
  return status; 
}

/**
  * @brief  控制RTC旁路阴影寄存器功能
  * @param  NewState: 旁路功能的新状态(ENABLE/DISABLE)
  * @retval None
  * @details 当启用旁路时：
  *          - 直接读取日历计数器值而非影子寄存器
  *          - 提高读取速度但可能读到不稳定值
  *          操作流程同参考时钟控制
  */
void RTC_BypassShadowCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 解除写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  
  /* 配置BYPSHAD控制位 */
  if (NewState != DISABLE)
  {
    /* 启用旁路：设置CR寄存器的BYPSHAD位 */
    RTC->CR |= (uint8_t)RTC_CR_BYPSHAD;  // 直接操作寄存器低8位
  }
  else
  {
    /* 禁用旁路：清除CR寄存器的BYPSHAD位 */
    RTC->CR &= (uint8_t)~RTC_CR_BYPSHAD;
  }

  /* 重新启用写保护 */
  RTC->WPR = 0xFF;
}

/**
  * @brief  设置RTC当前时间
  * @param  RTC_Format: 时间格式(RTC_Format_BIN/RTC_Format_BCD)
  * @param  RTC_TimeStruct: 包含时分秒的时间结构体指针
  * @retval 操作状态:
  *          - SUCCESS: 时间设置成功
  *          - ERROR:   初始化失败或同步错误
  * @details 实现步骤：
  *          1. 参数格式校验
  *          2. 时间值有效性检查
  *          3. 格式转换（BIN转BCD）
  *          4. 进入初始化模式写寄存器
  *          5. 等待寄存器同步
  */
ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
{
  uint32_t tmpreg = 0;
  ErrorStatus status = ERROR;
    
  /* 验证时间格式参数合法性 */
  assert_param(IS_RTC_FORMAT(RTC_Format));
  
  /* 二进制格式参数处理 */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* 12小时制模式检查 */
    if ((RTC->CR & RTC_CR_FMT) != (uint32_t)RESET)
    {
      assert_param(IS_RTC_HOUR12(RTC_TimeStruct->RTC_Hours));  // 验证小时数范围(1-12)
      assert_param(IS_RTC_H12(RTC_TimeStruct->RTC_H12));       // 验证AM/PM标识
    } 
    else  /* 24小时制处理 */
    {
      RTC_TimeStruct->RTC_H12 = 0x00;          // 清空12小时制标识
      assert_param(IS_RTC_HOUR24(RTC_TimeStruct->RTC_Hours));  // 验证小时数范围(0-23)
    }
    /* 验证分钟(0-59)和秒(0-59)范围 */
    assert_param(IS_RTC_MINUTES(RTC_TimeStruct->RTC_Minutes));
    assert_param(IS_RTC_SECONDS(RTC_TimeStruct->RTC_Seconds));
  }
  else  /* BCD格式参数处理 */
  {
    /* 将BCD值转换为二进制进行范围校验 */
    if ((RTC->CR & RTC_CR_FMT) != (uint32_t)RESET)
    {
      tmpreg = RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Hours);
      assert_param(IS_RTC_HOUR12(tmpreg));  // 转换后校验12小时制范围
      assert_param(IS_RTC_H12(RTC_TimeStruct->RTC_H12));
    } 
    else
    {
      RTC_TimeStruct->RTC_H12 = 0x00;
      assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Hours)));
    }
    assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Minutes)));
    assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Seconds)));
  }
  
  /* 构建时间寄存器值 */
  if (RTC_Format != RTC_Format_BIN)
  {
    /* BCD格式直接组合寄存器值 */
    tmpreg = (((uint32_t)(RTC_TimeStruct->RTC_Hours) << 16) | \
             ((uint32_t)(RTC_TimeStruct->RTC_Minutes) << 8) | \
             ((uint32_t)RTC_TimeStruct->RTC_Seconds) | \
             ((uint32_t)(RTC_TimeStruct->RTC_H12) << 16)); 
  }  
  else
  {
    /* 二进制转BCD后组合寄存器值 */
    tmpreg = (uint32_t)(((uint32_t)RTC_ByteToBcd2(RTC_TimeStruct->RTC_Hours) << 16) | \
                   ((uint32_t)RTC_ByteToBcd2(RTC_TimeStruct->RTC_Minutes) << 8) | \
                   ((uint32_t)RTC_ByteToBcd2(RTC_TimeStruct->RTC_Seconds)) | \
                   (((uint32_t)RTC_TimeStruct->RTC_H12) << 16));
  }  

  /* 解除写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* 进入初始化模式 */
  if (RTC_EnterInitMode() == ERROR)
  {
    status = ERROR;
  } 
  else
  {
    /* 写入时间寄存器TR */
    RTC->TR = (uint32_t)(tmpreg & RTC_TR_RESERVED_MASK);

    /* 退出初始化模式 */
    RTC_ExitInitMode(); 

    /* 检查是否需要等待同步（旁路模式跳过等待） */
    if ((RTC->CR & RTC_CR_BYPSHAD) == RESET)
    {
      if(RTC_WaitForSynchro() == ERROR)
      {
        status = ERROR;  // 同步失败
      }
      else
      {
        status = SUCCESS;
      }
    }
    else
    {
      status = SUCCESS;  // 旁路模式直接成功
    }
  }
  
  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
    
  return status;
}
/**
  * @brief  初始化RTC时间结构体为默认值（00:00:00）
  * @param  RTC_TimeStruct: 要初始化的时间结构体指针
  * @retval None
  * @details 默认配置：
  *          - 12小时制AM模式
  *          - 小时/分钟/秒均置零
  *          常用于时间变量声明后的初始状态设置
  */
void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct)
{
  /* 设置12小时制标识为AM */
  RTC_TimeStruct->RTC_H12 = RTC_H12_AM;    // RTC_H12_AM宏定义值为0x00
  
  /* 时间参数清零 */
  RTC_TimeStruct->RTC_Hours = 0;    // 小时归零
  RTC_TimeStruct->RTC_Minutes = 0;  // 分钟归零
  RTC_TimeStruct->RTC_Seconds = 0; // 秒归零
}

/**
  * @brief  获取当前RTC时间
  * @param  RTC_Format: 返回数据格式(RTC_Format_BIN/RTC_Format_BCD)
  * @param  RTC_TimeStruct: 存储获取结果的时间结构体指针
  * @retval None
  * @details 实现过程：
  *          1. 从TR寄存器读取原始时间数据
  *          2. 分解时/分/秒和12小时制标识
  *          3. 根据格式要求进行BCD转换
  *          注意：需确保已正确配置时钟格式（12/24小时制）
  */
void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
{
  uint32_t tmpreg = 0;

  /* 验证时间格式参数合法性 */
  assert_param(IS_RTC_FORMAT(RTC_Format));

  /* 从时间寄存器TR获取原始值（保留有效位，过滤保留位） */
  tmpreg = (uint32_t)(RTC->TR & RTC_TR_RESERVED_MASK); 

  /* 分解寄存器值到结构体成员 */
  RTC_TimeStruct->RTC_Hours = (uint8_t)((tmpreg & (RTC_TR_HT | RTC_TR_HU)) >> 16); // 提取小时位：bit16-20
  RTC_TimeStruct->RTC_Minutes = (uint8_t)((tmpreg & (RTC_TR_MNT | RTC_TR_MNU)) >>8); // 提取分钟位：bit8-14
  RTC_TimeStruct->RTC_Seconds = (uint8_t)(tmpreg & (RTC_TR_ST | RTC_TR_SU));       // 提取秒位：bit0-6
  RTC_TimeStruct->RTC_H12 = (uint8_t)((tmpreg & (RTC_TR_PM)) >> 16);               // 提取12小时制标识：bit22

  /* 二进制格式转换处理 */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* BCD转二进制（TR寄存器存储的是BCD格式值） */
    RTC_TimeStruct->RTC_Hours = (uint8_t)RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Hours);
    RTC_TimeStruct->RTC_Minutes = (uint8_t)RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Minutes);
    RTC_TimeStruct->RTC_Seconds = (uint8_t)RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Seconds);
  }
}

/**
  * @brief  获取RTC亚秒值并解冻日历寄存器
  * @param  None
  * @retval 当前亚秒值（0-999）
  * @details 硬件特性：
  *          - 读取SSR会冻结DR日期寄存器
  *          - 读取DR寄存器可解除冻结
  *          典型应用场景：需要原子性读取时间日期时
  */
uint32_t RTC_GetSubSecond(void)
{
  uint32_t tmpreg = 0;
  
  /* 从亚秒寄存器SSR获取当前值（读取后DR寄存器会冻结） */
  tmpreg = (uint32_t)(RTC->SSR);  // SSR寄存器包含0-999的递减计数器值
  
  /* 通过读取日期寄存器DR解除冻结（丢弃返回值，仅用于触发状态变更） */
  (void)(RTC->DR);  // 该操作确保后续日期寄存器访问的准确性
  
  return (tmpreg);  // 返回原始亚秒值（需要时可通过(999 - tmpreg)转换为递增模式）
}
/**
  * @brief  设置RTC当前日期
  * @param  RTC_Format: 日期格式(RTC_Format_BIN/RTC_Format_BCD)
  * @param  RTC_DateStruct: 包含年月日星期信息的日期结构体指针
  * @retval 操作状态:
  *          - SUCCESS: 日期设置成功
  *          - ERROR:   初始化失败或同步错误
  * @details 实现步骤：
  *          1. 参数有效性校验
  *          2. 日期格式转换处理
  *          3. 进入初始化模式写寄存器
  *          4. 等待硬件同步
  *          注意：年份参数范围根据RTC设计可能有限制（通常00-99）
  */
ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
{
  uint32_t tmpreg = 0;
  ErrorStatus status = ERROR;
  
  /* 验证日期格式参数合法性 */
  assert_param(IS_RTC_FORMAT(RTC_Format));

  /* 处理二进制格式月份的特殊情况（兼容硬件设计） */
  if ((RTC_Format == RTC_Format_BIN) && ((RTC_DateStruct->RTC_Month & 0x10) == 0x10))
  {
    /* 将月份的高4位转换为十进制值（处理BCD格式兼容性问题） */
    RTC_DateStruct->RTC_Month = (RTC_DateStruct->RTC_Month & (uint32_t)~(0x10)) + 0x0A; // 例如0x10表示10月
  }  
  
  /* 参数范围校验 */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* 二进制格式直接校验 */
    assert_param(IS_RTC_YEAR(RTC_DateStruct->RTC_Year));     // 验证年份范围(0-99)
    assert_param(IS_RTC_MONTH(RTC_DateStruct->RTC_Month));   // 验证月份范围(1-12)
    assert_param(IS_RTC_DATE(RTC_DateStruct->RTC_Date));     // 验证日期范围(1-31)
  }
  else
  {
    /* BCD格式先转换后校验 */
    assert_param(IS_RTC_YEAR(RTC_Bcd2ToByte(RTC_DateStruct->RTC_Year)));  // 转换后校验年份
    tmpreg = RTC_Bcd2ToByte(RTC_DateStruct->RTC_Month);
    assert_param(IS_RTC_MONTH(tmpreg));  // 转换后校验月份
    tmpreg = RTC_Bcd2ToByte(RTC_DateStruct->RTC_Date);
    assert_param(IS_RTC_DATE(tmpreg));    // 转换后校验日期
  }
  assert_param(IS_RTC_WEEKDAY(RTC_DateStruct->RTC_WeekDay));  // 校验星期范围(1-7)

  /* 构建日期寄存器值 */
  if (RTC_Format != RTC_Format_BIN)
  {
    /* BCD格式直接组合寄存器值 */
    tmpreg = ((((uint32_t)RTC_DateStruct->RTC_Year) << 16) | \
              (((uint32_t)RTC_DateStruct->RTC_Month) << 8) | \
              ((uint32_t)RTC_DateStruct->RTC_Date) | \
              (((uint32_t)RTC_DateStruct->RTC_WeekDay) << 13)); 
  }  
  else
  {
    /* 二进制转BCD后组合寄存器值 */
    tmpreg = (((uint32_t)RTC_ByteToBcd2(RTC_DateStruct->RTC_Year) << 16) | \
              ((uint32_t)RTC_ByteToBcd2(RTC_DateStruct->RTC_Month) << 8) | \
              ((uint32_t)RTC_ByteToBcd2(RTC_DateStruct->RTC_Date)) | \
              ((uint32_t)RTC_DateStruct->RTC_WeekDay << 13));  // 星期存储在bit13-15
  }

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一解锁密钥
  RTC->WPR = 0x53;  // 第二解锁密钥

  /* 进入初始化模式 */
  if (RTC_EnterInitMode() == ERROR)
  {
    status = ERROR;  // 进入失败直接返回错误
  } 
  else
  {
    /* 写入日期寄存器DR（保留有效位，过滤保留位） */
    RTC->DR = (uint32_t)(tmpreg & RTC_DR_RESERVED_MASK);

    /* 退出初始化模式 */
    RTC_ExitInitMode(); 

    /* 同步处理（旁路模式跳过等待） */
    if ((RTC->CR & RTC_CR_BYPSHAD) == RESET)  // 检查是否启用旁路
    {
      if(RTC_WaitForSynchro() == ERROR)  // 等待硬件同步
      {
        status = ERROR;  // 同步超时
      }
      else
      {
        status = SUCCESS;  // 同步成功
      }
    }
    else
    {
      status = SUCCESS;  // 旁路模式直接成功
    }
  }
  
  /* 重新启用写保护 */
  RTC->WPR = 0xFF;   
  
  return status;
}

/**
  * @brief  初始化日期结构体为默认值（周一，1月1日，00年）
  * @param  RTC_DateStruct: 要初始化的日期结构体指针
  * @retval None
  * @details 默认配置：
  *          - 星期：周一（RTC_Weekday_Monday）
  *          - 日期：1日
  *          - 月份：1月（RTC_Month_January）
  *          - 年份：00
  *          常用于日期变量声明后的初始状态设置
  */
void RTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct)
{
  /* 设置默认日期参数 */
  RTC_DateStruct->RTC_WeekDay = RTC_Weekday_Monday;  // 周一对应值0x01
  RTC_DateStruct->RTC_Date = 1;     // 日期设为1号
  RTC_DateStruct->RTC_Month = RTC_Month_January;  // 一月对应值0x01
  RTC_DateStruct->RTC_Year = 0;    // 年份归零
}
/**
  * @brief  获取当前RTC日期
  * @param  RTC_Format: 返回数据格式(RTC_Format_BIN/RTC_Format_BCD)
  * @param  RTC_DateStruct: 存储获取结果的日期结构体指针
  * @retval None
  * @details 实现过程：
  *          1. 从DR寄存器读取原始日期数据
  *          2. 分解年/月/日/星期字段
  *          3. 根据格式要求进行BCD转换
  *          注意：读取日期前建议检查日历寄存器同步状态
  */
void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
{
  uint32_t tmpreg = 0;

  /* 验证日期格式参数合法性 */
  assert_param(IS_RTC_FORMAT(RTC_Format));  // 确保参数为RTC_Format_BIN/RTC_Format_BCD
  
  /* 从日期寄存器DR获取原始值（保留有效位，过滤保留位） */
  tmpreg = (uint32_t)(RTC->DR & RTC_DR_RESERVED_MASK);  // RTC_DR_RESERVED_MASK通常为0x00FFFF3F

  /* 分解寄存器值到结构体成员 */
  RTC_DateStruct->RTC_Year = (uint8_t)((tmpreg & (RTC_DR_YT | RTC_DR_YU)) >> 16);  // 提取年份：bit16-23（BCD十位和个位）
  RTC_DateStruct->RTC_Month = (uint8_t)((tmpreg & (RTC_DR_MT | RTC_DR_MU)) >> 8);  // 提取月份：bit8-15（BCD十位和个位）
  RTC_DateStruct->RTC_Date = (uint8_t)(tmpreg & (RTC_DR_DT | RTC_DR_DU));          // 提取日期：bit0-7（BCD十位和个位）
  RTC_DateStruct->RTC_WeekDay = (uint8_t)((tmpreg & (RTC_DR_WDU)) >> 13);          // 提取星期：bit13-15

  /* 二进制格式转换处理 */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* 将BCD值转换为二进制（DR寄存器存储的是BCD格式值） */
    RTC_DateStruct->RTC_Year = (uint8_t)RTC_Bcd2ToByte(RTC_DateStruct->RTC_Year);
    RTC_DateStruct->RTC_Month = (uint8_t)RTC_Bcd2ToByte(RTC_DateStruct->RTC_Month);
    RTC_DateStruct->RTC_Date = (uint8_t)RTC_Bcd2ToByte(RTC_DateStruct->RTC_Date);
  }
}


/**
  * @}
  */

/** @defgroup RTC_Group3 Alarms configuration functions
 *  @brief   Alarms (Alarm A and Alarm B) configuration functions 
 *
@verbatim   
 ===============================================================================
         ##### Alarms A and B configuration functions #####
 ===============================================================================  
 
 [..] This section provide functions allowing to program and read the RTC Alarms.

@endverbatim
  * @{
  */
/**
  * @brief  配置RTC闹钟参数
  * @param  RTC_Format: 数据格式(RTC_Format_BIN/RTC_Format_BCD)
  * @param  RTC_Alarm: 闹钟选择(RTC_Alarm_A/RTC_Alarm_B)
  * @param  RTC_AlarmStruct: 包含闹钟配置参数的结构体指针
  * @retval None
  * @details 实现步骤：
  *          1. 参数有效性校验
  *          2. 时间/日期格式处理
  *          3. 构建寄存器配置值
  *          4. 写入对应闹钟寄存器
  *          注意：配置前需确保目标闹钟已禁用
  */
void RTC_SetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct)
{
  uint32_t tmpreg = 0;
  
  /* 参数合法性校验 */
  assert_param(IS_RTC_FORMAT(RTC_Format));  // 校验数据格式
  assert_param(IS_RTC_ALARM(RTC_Alarm));    // 校验闹钟选择(A/B)
  assert_param(IS_ALARM_MASK(RTC_AlarmStruct->RTC_AlarmMask));  // 校验闹钟掩码
  assert_param(IS_RTC_ALARM_DATE_WEEKDAY_SEL(RTC_AlarmStruct->RTC_AlarmDateWeekDaySel));  // 校验日期/星期模式

  /* 二进制格式参数处理 */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* 12小时制处理 */
    if ((RTC->CR & RTC_CR_FMT) != (uint32_t)RESET)  // 检查是否12小时模式
    {
      assert_param(IS_RTC_HOUR12(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours));  // 校验小时范围(1-12)
      assert_param(IS_RTC_H12(RTC_AlarmStruct->RTC_AlarmTime.RTC_H12));       // 校验AM/PM标识
    } 
    else  /* 24小时制处理 */
    {
      RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = 0x00;  // 清空12小时制标识
      assert_param(IS_RTC_HOUR24(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours));  // 校验小时范围(0-23)
    }
    /* 校验分钟(0-59)和秒(0-59) */
    assert_param(IS_RTC_MINUTES(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes));
    assert_param(IS_RTC_SECONDS(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds));
    
    /* 日期/星期模式处理 */
    if(RTC_AlarmStruct->RTC_AlarmDateWeekDaySel == RTC_AlarmDateWeekDaySel_Date)
    {
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_DATE(RTC_AlarmStruct->RTC_AlarmDateWeekDay));  // 校验日期范围(1-31)
    }
    else
    {
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(RTC_AlarmStruct->RTC_AlarmDateWeekDay));  // 校验星期范围(1-7)
    }
  }
  else  /* BCD格式参数处理 */
  {
    /* 12小时制处理 */
    if ((RTC->CR & RTC_CR_FMT) != (uint32_t)RESET)
    {
      tmpreg = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours);
      assert_param(IS_RTC_HOUR12(tmpreg));  // 转换后校验小时
      assert_param(IS_RTC_H12(RTC_AlarmStruct->RTC_AlarmTime.RTC_H12));
    } 
    else
    {
      RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = 0x00;
      assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours)));
    }
    
    /* 转换后校验分钟和秒 */
    assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes)));
    assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds)));
    
    /* 日期/星期模式处理 */
    if(RTC_AlarmStruct->RTC_AlarmDateWeekDaySel == RTC_AlarmDateWeekDaySel_Date)
    {
      tmpreg = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmDateWeekDay);
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_DATE(tmpreg));  // 校验日期范围(1-31)    
    }
    else
    {
      tmpreg = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmDateWeekDay);
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(tmpreg));  // 校验星期范围(1-7)    
    }    
  }

  /* 构建寄存器配置值 */
  if (RTC_Format != RTC_Format_BIN)
  {
    /* BCD格式直接组合字段 */
    tmpreg = (((uint32_t)(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours) << 16) | \
              ((uint32_t)(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes) << 8) | \
              ((uint32_t)RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds) | \
              ((uint32_t)(RTC_AlarmStruct->RTC_AlarmTime.RTC_H12) << 16) | \
              ((uint32_t)(RTC_AlarmStruct->RTC_AlarmDateWeekDay) << 24) | \
              ((uint32_t)RTC_AlarmStruct->RTC_AlarmDateWeekDaySel) | \
              ((uint32_t)RTC_AlarmStruct->RTC_AlarmMask)); 
  }  
  else
  {
    /* 二进制转BCD后组合字段 */
    tmpreg = (((uint32_t)RTC_ByteToBcd2(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours) << 16) | \
              ((uint32_t)RTC_ByteToBcd2(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes) << 8) | \
              ((uint32_t)RTC_ByteToBcd2(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds)) | \
              ((uint32_t)(RTC_AlarmStruct->RTC_AlarmTime.RTC_H12) << 16) | \
              ((uint32_t)RTC_ByteToBcd2(RTC_AlarmStruct->RTC_AlarmDateWeekDay) << 24) | \
              ((uint32_t)RTC_AlarmStruct->RTC_AlarmDateWeekDaySel) | \
              ((uint32_t)RTC_AlarmStruct->RTC_AlarmMask)); 
  } 

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一解锁密钥
  RTC->WPR = 0x53;  // 第二解锁密钥

  /* 配置目标闹钟寄存器 */
  if (RTC_Alarm == RTC_Alarm_A)
  {
    RTC->ALRMAR = (uint32_t)tmpreg;  // 写入闹钟A寄存器
  }
  else
  {
    RTC->ALRMBR = (uint32_t)tmpreg;  // 写入闹钟B寄存器
  }

  /* 重新启用写保护 */
  RTC->WPR = 0xFF;   
}
/**
  * @brief  初始化RTC闹钟结构体为默认值
  * @param  RTC_AlarmStruct: 要初始化的闹钟结构体指针
  * @retval None
  * @details 默认配置：
  *          - 时间：00:00:00（AM模式）
  *          - 日期：每月1日
  *          - 掩码：全字段匹配（不屏蔽任何时间元素）
  *          用于初始化新闹钟配置的起点
  */
void RTC_AlarmStructInit(RTC_AlarmTypeDef* RTC_AlarmStruct)
{
  /* 时间参数初始化 */
  RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = RTC_H12_AM;    // 12小时制设为AM
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = 0;          // 小时归零
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = 0;        // 分钟归零
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = 0;       // 秒归零

  /* 日期参数初始化 */
  RTC_AlarmStruct->RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date; // 使用日期模式
  RTC_AlarmStruct->RTC_AlarmDateWeekDay = 1;            // 日期设为每月1日

  /* 掩码参数初始化 */
  RTC_AlarmStruct->RTC_AlarmMask = RTC_AlarmMask_None;    // 全字段匹配模式
}

/**
  * @brief  获取指定RTC闹钟的配置参数
  * @param  RTC_Format: 返回数据格式(RTC_Format_BIN/RTC_Format_BCD)
  * @param  RTC_Alarm: 目标闹钟选择(RTC_Alarm_A/RTC_Alarm_B)
  * @param  RTC_AlarmStruct: 存储获取结果的闹钟结构体指针
  * @retval None
  * @details 实现过程：
  *          1. 从ALRMxR寄存器读取原始配置值
  *          2. 分解时间/日期/掩码字段
  *          3. 根据格式进行BCD转换
  */
void RTC_GetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct)
{
  uint32_t tmpreg = 0;

  /* 参数合法性校验 */
  assert_param(IS_RTC_FORMAT(RTC_Format));      // 校验数据格式
  assert_param(IS_RTC_ALARM(RTC_Alarm));        // 校验闹钟选择

  /* 读取对应闹钟寄存器 */
  if (RTC_Alarm == RTC_Alarm_A)
  {
    tmpreg = (uint32_t)(RTC->ALRMAR);  // 读取闹钟A配置寄存器
  }
  else
  {
    tmpreg = (uint32_t)(RTC->ALRMBR);  // 读取闹钟B配置寄存器
  }

  /* 分解寄存器值到结构体成员 */
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = (uint32_t)((tmpreg & (RTC_ALRMAR_HT | RTC_ALRMAR_HU)) >> 16); // 提取小时(bit16-23)
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = (uint32_t)((tmpreg & (RTC_ALRMAR_MNT | RTC_ALRMAR_MNU)) >> 8); // 提取分钟(bit8-15)
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = (uint32_t)(tmpreg & (RTC_ALRMAR_ST | RTC_ALRMAR_SU));     // 提取秒(bit0-7)
  RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = (uint32_t)((tmpreg & RTC_ALRMAR_PM) >> 16);                  // 提取AM/PM标识(bit16)
  RTC_AlarmStruct->RTC_AlarmDateWeekDay = (uint32_t)((tmpreg & (RTC_ALRMAR_DT | RTC_ALRMAR_DU)) >> 24);  // 提取日期/星期(bit24-31)
  RTC_AlarmStruct->RTC_AlarmDateWeekDaySel = (uint32_t)(tmpreg & RTC_ALRMAR_WDSEL);                     // 提取日期/星期选择位(bit24)
  RTC_AlarmStruct->RTC_AlarmMask = (uint32_t)(tmpreg & RTC_AlarmMask_All);                             // 提取掩码配置(bit0-3)

  /* 二进制格式转换处理 */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* BCD转二进制 */
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours);
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes);
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds);
    RTC_AlarmStruct->RTC_AlarmDateWeekDay = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmDateWeekDay);
  }  
}

/**
  * @brief  启用/禁用指定RTC闹钟
  * @param  RTC_Alarm: 目标闹钟(RTC_Alarm_A/RTC_Alarm_B)
  * @param  NewState: 新状态(ENABLE/DISABLE)
  * @retval 操作状态:
  *          - SUCCESS: 状态切换成功
  *          - ERROR:   禁用操作超时或失败
  * @details 硬件特性：
  *          - 启用直接设置CR寄存器对应位
  *          - 禁用需要等待硬件同步完成
  */
ErrorStatus RTC_AlarmCmd(uint32_t RTC_Alarm, FunctionalState NewState)
{
  __IO uint32_t alarmcounter = 0x00;  // 超时计数器
  uint32_t alarmstatus = 0x00;        // 状态标志
  ErrorStatus status = ERROR;         // 返回状态
    
  /* 参数校验 */
  assert_param(IS_RTC_CMD_ALARM(RTC_Alarm));         // 校验闹钟选择有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));       // 校验状态参数

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一解锁密钥
  RTC->WPR = 0x53;  // 第二解锁密钥

  /* 状态切换处理 */
  if (NewState != DISABLE)
  {
    /* 启用闹钟：设置CR寄存器的对应使能位 */
    RTC->CR |= (uint32_t)RTC_Alarm;  // ALRAE/ALRBE位
    status = SUCCESS;                // 启用操作无需等待立即成功
  }
  else
  { 
    /* 禁用闹钟：清除CR寄存器的对应使能位 */
    RTC->CR &= (uint32_t)~RTC_Alarm;  // 清除ALRAE/ALRBE位
   
    /* 等待硬件同步完成（最大等待INITMODE_TIMEOUT次） */
    do
    {
      alarmstatus = RTC->ISR & (RTC_Alarm >> 8);  // 检查ALRAWF/ALRBWF标志
      alarmcounter++;  
    } while((alarmcounter != INITMODE_TIMEOUT) && (alarmstatus == 0x00));
    
    /* 校验同步状态 */
    if ((RTC->ISR & (RTC_Alarm >> 8)) == RESET)
    {
      status = ERROR;  // 超时未完成同步
    } 
    else
    {
      status = SUCCESS;  // 同步成功完成
    }        
  } 

  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
  
  return status;
}

/**
  * @brief  Configure the RTC AlarmA/B Sub seconds value and mask.*
  * @note   This function is performed only when the Alarm is disabled. 
  * @param  RTC_Alarm: specifies the alarm to be configured.
  *   This parameter can be one of the following values:
  *     @arg RTC_Alarm_A: to select Alarm A
  *     @arg RTC_Alarm_B: to select Alarm B
  * @param  RTC_AlarmSubSecondValue: specifies the Sub seconds value.
  *   This parameter can be a value from 0 to 0x00007FFF.
  * @param  RTC_AlarmSubSecondMask:  specifies the Sub seconds Mask.
  *   This parameter can be any combination of the following values:
  *     @arg RTC_AlarmSubSecondMask_All    : All Alarm SS fields are masked.
  *                                          There is no comparison on sub seconds for Alarm.
  *     @arg RTC_AlarmSubSecondMask_SS14_1 : SS[14:1] are don't care in Alarm comparison.
  *                                          Only SS[0] is compared
  *     @arg RTC_AlarmSubSecondMask_SS14_2 : SS[14:2] are don't care in Alarm comparison.
  *                                          Only SS[1:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_3 : SS[14:3] are don't care in Alarm comparison.
  *                                          Only SS[2:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_4 : SS[14:4] are don't care in Alarm comparison.
  *                                          Only SS[3:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_5 : SS[14:5] are don't care in Alarm comparison.
  *                                          Only SS[4:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_6 : SS[14:6] are don't care in Alarm comparison.
  *                                          Only SS[5:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_7 : SS[14:7] are don't care in Alarm comparison.
  *                                          Only SS[6:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_8 : SS[14:8] are don't care in Alarm comparison.
  *                                          Only SS[7:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_9 : SS[14:9] are don't care in Alarm comparison.
  *                                          Only SS[8:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_10: SS[14:10] are don't care in Alarm comparison.
  *                                          Only SS[9:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_11: SS[14:11] are don't care in Alarm comparison.
  *                                          Only SS[10:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_12: SS[14:12] are don't care in Alarm comparison.
  *                                          Only SS[11:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14_13: SS[14:13] are don't care in Alarm comparison.
  *                                          Only SS[12:0] are compared
  *     @arg RTC_AlarmSubSecondMask_SS14   : SS[14] is don't care in Alarm comparison.
  *                                          Only SS[13:0] are compared
  *     @arg RTC_AlarmSubSecondMask_None   : SS[14:0] are compared and must match
  *                                          to activate alarm
  * @retval None
  */
/**
  * @brief  配置RTC闹钟亚秒参数
  * @param  RTC_Alarm: 目标闹钟(RTC_Alarm_A/RTC_Alarm_B)
  * @param  RTC_AlarmSubSecondValue: 亚秒值(0-32767)
  * @param  RTC_AlarmSubSecondMask: 亚秒掩码(RTC_AlarmSubSecondMask_x)
  * @retval None
  * @details 硬件特性：
  *          - 亚秒寄存器为15位递减计数器
  *          - 掩码决定比较位数（例：掩码0xF表示比较低4位）
  *          用于高精度定时触发（可达1/32768秒精度）
  */
void RTC_AlarmSubSecondConfig(uint32_t RTC_Alarm, uint32_t RTC_AlarmSubSecondValue, 
                             uint32_t RTC_AlarmSubSecondMask)
{
  uint32_t tmpreg = 0;

  /* 参数合法性校验 */
  assert_param(IS_RTC_ALARM(RTC_Alarm));  // 校验闹钟选择有效性
  assert_param(IS_RTC_ALARM_SUB_SECOND_VALUE(RTC_AlarmSubSecondValue)); // 校验亚秒范围(0-0x7FFF)
  assert_param(IS_RTC_ALARM_SUB_SECOND_MASK(RTC_AlarmSubSecondMask)); // 校验掩码有效性
  
  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一解锁密钥
  RTC->WPR = 0x53;  // 第二解锁密钥
  
  /* 组合亚秒值和掩码（SS[14:0] | MASKSS[3:0]） */
  tmpreg = (uint32_t)(RTC_AlarmSubSecondValue) |  // 亚秒值占据bit0-14
           (uint32_t)(RTC_AlarmSubSecondMask);   // 掩码占据bit24-27
  
  /* 写入目标闹钟的亚秒寄存器 */
  if (RTC_Alarm == RTC_Alarm_A)
  {
    RTC->ALRMASSR = tmpreg;  // 写入闹钟A亚秒寄存器
  }
  else
  {
    RTC->ALRMBSSR = tmpreg;  // 写入闹钟B亚秒寄存器
  }

  /* 重新启用写保护 */
  RTC->WPR = 0xFF;
}

/**
  * @brief  获取RTC闹钟当前亚秒值
  * @param  RTC_Alarm: 目标闹钟(RTC_Alarm_A/RTC_Alarm_B)
  * @retval 当前亚秒值(0-32767)
  * @details 注意：
  *          - 返回值包含实际配置的亚秒值，不考虑掩码位
  *          - 需配合RTC_AlarmSubSecondConfig()的掩码参数使用
  */
uint32_t RTC_GetAlarmSubSecond(uint32_t RTC_Alarm)
{
  uint32_t tmpreg = 0;
  
  /* 读取目标闹钟的亚秒寄存器 */
  if (RTC_Alarm == RTC_Alarm_A)
  {
    tmpreg = (uint32_t)((RTC->ALRMASSR) & RTC_ALRMASSR_SS);  // 提取bit0-14
  }
  else
  {
    tmpreg = (uint32_t)((RTC->ALRMBSSR) & RTC_ALRMBSSR_SS);  // 提取bit0-14
  } 
  
  return (tmpreg);  // 返回原始亚秒值（需根据掩码解析有效位）
}

/**
  * @}
  */

/** @defgroup RTC_Group4 WakeUp Timer configuration functions
 *  @brief   WakeUp Timer configuration functions 
 *
@verbatim   
 ===============================================================================
                 ##### WakeUp Timer configuration functions #####
 ===============================================================================  

 [..] This section provide functions allowing to program and read the RTC WakeUp.

@endverbatim
  * @{
  */
/**
  * @brief  配置RTC唤醒时钟源
  * @param  RTC_WakeUpClock: 唤醒时钟源选择
  *         可选值：
  *           RTC_WakeUpClock_RTCCLK_Div16    RTCCLK/16分频
  *           RTC_WakeUpClock_RTCCLK_Div8     RTCCLK/8分频
  *           RTC_WakeUpClock_RTCCLK_Div4     RTCCLK/4分频
  *           RTC_WakeUpClock_RTCCLK_Div2     RTCCLK/2分频
  *           RTC_WakeUpClock_CK_SPRE_16bits  1Hz CK_SPRE时钟(16位计数器)
  *           RTC_WakeUpClock_CK_SPRE_17bits  1Hz CK_SPRE时钟(17位计数器)
  * @retval None
  * @details 注意事项：
  *          - 修改前需确保唤醒定时器已禁用
  *          - CK_SPRE时钟通常由异步预分频器生成1Hz信号
  */
void RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock)
{
  /* 校验时钟源参数合法性 */
  assert_param(IS_RTC_WAKEUP_CLOCK(RTC_WakeUpClock));

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一解锁密钥
  RTC->WPR = 0x53;  // 第二解锁密钥

  /* 清除CR寄存器中现有的时钟源配置 */
  RTC->CR &= (uint32_t)~RTC_CR_WUCKSEL;  // WUCKSEL[2:0]清零

  /* 写入新的时钟源配置 */
  RTC->CR |= (uint32_t)RTC_WakeUpClock;  // 设置WUCKSEL位
  
  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
}

/**
  * @brief  设置RTC唤醒计数器值
  * @param  RTC_WakeUpCounter: 唤醒计数值(0x0000-0xFFFF)
  * @retval None
  * @details 硬件特性：
  *          - 当使用CK_SPRE时钟时，实际范围为0-0x1FFFF（17位）
  *          - 计数器递减到0时触发唤醒事件
  */
void RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter)
{
  /* 校验计数值范围 */
  assert_param(IS_RTC_WAKEUP_COUNTER(RTC_WakeUpCounter));
  
  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  
  /* 写入唤醒定时器寄存器 */
  RTC->WUTR = (uint32_t)RTC_WakeUpCounter;  // WUT[15:0]或WUT[16:0]
  
  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
}

/**
  * @brief  获取当前唤醒计数器值
  * @param  None
  * @retval 唤醒计数器当前值
  * @details 注意：
  *          - 返回值为实时递减的计数值
  *          - 若使用17位模式，需结合CR寄存器的WUCKSEL配置解析
  */
uint32_t RTC_GetWakeUpCounter(void)
{
  /* 读取并返回计数器值（自动屏蔽保留位） */
  return ((uint32_t)(RTC->WUTR & RTC_WUTR_WUT));  // WUT[15:0]或WUT[16:0]
}

/**
  * @brief  启用/禁用RTC唤醒定时器
  * @param  NewState: 新状态(ENABLE/DISABLE)
  * @retval 操作状态:
  *          - SUCCESS: 状态切换成功
  *          - ERROR:   禁用操作超时失败
  * @details 低功耗应用：
  *          - 唤醒定时器用于周期性地退出低功耗模式
  *          - 需配合RTC中断和NVIC配置使用
  */
ErrorStatus RTC_WakeUpCmd(FunctionalState NewState)
{
  __IO uint32_t wutcounter = 0x00;  // 超时计数器
  uint32_t wutwfstatus = 0x00;       // 状态标志
  ErrorStatus status = ERROR;        // 返回状态
  
  /* 校验状态参数合法性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  if (NewState != DISABLE)
  {
    /* 启用唤醒定时器：设置CR寄存器的WUTE位 */
    RTC->CR |= (uint32_t)RTC_CR_WUTE; // 使能唤醒定时器
    status = SUCCESS;                // 启用操作无需等待
  }
  else
  {
    /* 禁用唤醒定时器：清除CR寄存器的WUTE位 */
    RTC->CR &= (uint32_t)~RTC_CR_WUTE;
    
    /* 等待WUTWF标志置位（最大等待INITMODE_TIMEOUT次） */
    do {
      wutwfstatus = RTC->ISR & RTC_ISR_WUTWF; // 检查配置就绪标志
      wutcounter++;  
    } while((wutcounter != INITMODE_TIMEOUT) && (wutwfstatus == 0x00));
    
    /* 校验同步状态 */
    if ((RTC->ISR & RTC_ISR_WUTWF) == RESET) {
      status = ERROR;  // 超时未完成
    } else {
      status = SUCCESS; // 同步完成
    }    
  }

  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
  
  return status;
}

/**
  * @}
  */

/** @defgroup RTC_Group5 Daylight Saving configuration functions
 *  @brief   Daylight Saving configuration functions 
 *
@verbatim   
 ===============================================================================
              ##### Daylight Saving configuration functions #####
 ===============================================================================  

 [..] This section provide functions allowing to configure the RTC DayLight Saving.

@endverbatim
  * @{
  */
/**
  * @brief  配置RTC夏令时调整（±1小时）并存储操作标志
  * @param  RTC_DayLightSaving: 夏令时操作类型
  *          RTC_DayLightSaving_SUB1H : 减少1小时（冬令时）
  *          RTC_DayLightSaving_ADD1H : 增加1小时（夏令时）
  * @param  RTC_StoreOperation: 夏令时操作标志存储方式
  *          RTC_StoreOperation_Reset : 清除BCK标志位
  *          RTC_StoreOperation_Set   : 设置BCK标志位
  * @retval None
  * @details 硬件机制：
  *          - 实际通过修改RTC时间寄存器TR实现小时调整
  *          - BCK标志位用于指示最后一次操作方向
  *          适用于需要手动调整时制的系统
  */
void RTC_DayLightSavingConfig(uint32_t RTC_DayLightSaving, uint32_t RTC_StoreOperation)
{
  /* 参数合法性校验 */
  assert_param(IS_RTC_DAYLIGHT_SAVING(RTC_DayLightSaving));  // 校验操作类型
  assert_param(IS_RTC_STORE_OPERATION(RTC_StoreOperation)); // 校验存储方式

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一解锁密钥
  RTC->WPR = 0x53;  // 第二解锁密钥

  /* 清空CR寄存器的夏令时相关配置位 */
  RTC->CR &= (uint32_t)~(RTC_CR_BCK);  // 清除BCK标志位

  /* 配置夏令时参数和存储方式 */
  RTC->CR |= (uint32_t)(RTC_DayLightSaving | RTC_StoreOperation); 

  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
}

/**
  * @brief  获取最后一次夏令时操作方向标志
  * @param  None
  * @retval 最后一次夏令时存储操作状态：
  *          RTC_StoreOperation_Reset: 最后一次操作为减小时
  *          RTC_StoreOperation_Set:   最后一次操作为增加小时
  * @details BCK标志位特性：
  *          - 0：前次为RTC_DayLightSaving_SUB1H操作
  *          - 1：前次为RTC_DayLightSaving_ADD1H操作
  */
uint32_t RTC_GetStoreOperation(void)
{
  return (RTC->CR & RTC_CR_BCK);  // 直接返回CR寄存器的BCK位状态
}

/**
  * @brief  配置RTC输出引脚功能及极性
  * @param  RTC_Output: 输出信号选择
  *          RTC_Output_Disable: 关闭输出
  *          RTC_Output_AlarmA : 输出AlarmA激活信号
  *          RTC_Output_AlarmB : 输出AlarmB激活信号  
  *          RTC_Output_WakeUp  : 输出WakeUp事件信号
  * @param  RTC_OutputPolarity: 输出极性
  *          RTC_OutputPolarity_High: 有效时输出高电平
  *          RTC_OutputPolarity_Low : 有效时输出低电平
  * @retval None
  * @details 硬件连接：
  *          - 输出引脚通常映射到PC13/TAMPER-RTC引脚
  *          常用于驱动外部电路或触发中断
  */
void RTC_OutputConfig(uint32_t RTC_Output, uint32_t RTC_OutputPolarity)
{
  /* 参数合法性校验 */
  assert_param(IS_RTC_OUTPUT(RTC_Output));        // 校验输出源选择
  assert_param(IS_RTC_OUTPUT_POL(RTC_OutputPolarity)); // 校验极性配置

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* 清除输出配置相关位(OSEL[1:0]和POL位) */
  RTC->CR &= (uint32_t)~(RTC_CR_OSEL | RTC_CR_POL); 

  /* 写入新的输出配置 */
  RTC->CR |= (uint32_t)(RTC_Output | RTC_OutputPolarity); 

  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
}


/**
  * @}
  */

/** @defgroup RTC_Group7 Digital Calibration configuration functions
 *  @brief   Coarse Calibration configuration functions 
 *
@verbatim   
 ===============================================================================
              ##### Digital Calibration configuration functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  配置RTC粗校准参数
  * @param  RTC_CalibSign: 校准符号（正/负）
  * @param  Value: 校准值（范围取决于符号）
  * @retval 操作状态：
  *          - SUCCESS: 校准参数配置成功
  *          - ERROR:   无法进入初始化模式
  * @details 校准规则：
  *          - 负号：范围0-63（2ppm/步长，最多补偿-126ppm）
  *          - 正号：范围0-126（4ppm/步长，最多补偿+504ppm）
  *          用于补偿石英晶振的频率偏差
  */
ErrorStatus RTC_CoarseCalibConfig(uint32_t RTC_CalibSign, uint32_t Value)
{
  ErrorStatus status = ERROR;
   
  /* 校验校准参数有效性 */
  assert_param(IS_RTC_CALIB_SIGN(RTC_CalibSign));  // RTC_CalibSign_Positive/Negative
  assert_param(IS_RTC_CALIB_VALUE(Value));        // 值范围检查

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一解锁密钥
  RTC->WPR = 0x53;  // 第二解锁密钥

  /* 进入初始化模式（此时可修改校准参数） */
  if (RTC_EnterInitMode() == ERROR)
  {
    status = ERROR;  // 初始化失败直接返回错误
  } 
  else
  {
    /* 将校准符号和值组合写入校准寄存器 */
    RTC->CALIBR = (uint32_t)(RTC_CalibSign | Value); 
    /* 退出初始化模式（参数锁定） */
    RTC_ExitInitMode();
    
    status = SUCCESS;  // 标记操作成功
  } 

  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
  
  return status;
}

/**
  * @brief  启用/禁用RTC粗校准功能
  * @param  NewState: 新状态(ENABLE/DISABLE)
  * @retval 操作状态：
  *          - SUCCESS: 状态切换成功
  *          - ERROR:   无法进入初始化模式
  * @details 硬件机制：
  *          - 启用后CALIBR寄存器的值生效
  *          - 校准原理：通过周期性地增减时钟脉冲实现频率微调
  */
ErrorStatus RTC_CoarseCalibCmd(FunctionalState NewState)
{
  ErrorStatus status = ERROR;
  
  /* 校验状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  
  /* 进入初始化模式 */
  if (RTC_EnterInitMode() == ERROR)
  {
    status =  ERROR;
  }
  else
  {
    /* 配置粗校准启用位（DCE） */
    if (NewState != DISABLE)
    {
      RTC->CR |= (uint32_t)RTC_CR_DCE;  // 启用粗校准
    }
    else
    { 
      RTC->CR &= (uint32_t)~RTC_CR_DCE; // 禁用粗校准
    }
    /* 退出初始化模式 */
    RTC_ExitInitMode();
    
    status = SUCCESS;
  } 
  
  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
  
  return status;
}

/**
  * @brief  控制RTC校准信号输出
  * @param  NewState: 新状态(ENABLE/DISABLE)
  * @retval None
  * @details 输出特性：
  *          - 输出引脚：PC13（需配置AFIO复用）
  *          - 输出频率：1Hz或512Hz（由CALIBR寄存器设置）
  *          用于外部校准仪器检测RTC时钟精度
  */
void RTC_CalibOutputCmd(FunctionalState NewState)
{
  /* 校验状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  
  /* 配置校准时钟输出使能位（COE） */
  if (NewState != DISABLE)
  {
    RTC->CR |= (uint32_t)RTC_CR_COE;  // 启用校准信号输出
  }
  else
  { 
    RTC->CR &= (uint32_t)~RTC_CR_COE; // 禁用输出
  }
  
  /* 重新启用写保护 */
  RTC->WPR = 0xFF; 
}
/**
  * @brief  RTC校准信号输出配置
  * @param  RTC_CalibOutput: 校准输出信号选择
  *         - RTC_CalibOutput_512Hz: 输出512Hz方波（用于粗调）
  *         - RTC_CalibOutput_1Hz:   输出1Hz脉冲（用于精调）
  * @detail 硬件特性：
  *         - 输出精度：±0.9537ppm（1Hz模式）
  *         - 驱动能力：需外接上拉电阻
  *         - 引脚分配：通过AFIO映射到PC13
  * @warning 配置前需确保RTC时钟源稳定
  */
void RTC_CalibOutputConfig(uint32_t RTC_CalibOutput)
{
  /* 参数有效性验证（0x00000000/0x00080000） */
  assert_param(IS_RTC_CALIB_OUTPUT(RTC_CalibOutput));  // 检查是否为有效模式

  /* RTC寄存器解锁序列（两步握手） */
  RTC->WPR = 0xCA;  // 第一步解锁密钥
  RTC->WPR = 0x53;  // 第二步解锁密钥
  
  /* 清除校准输出选择位（CR[23]） */
  RTC->CR &= (uint32_t)~(RTC_CR_COSEL);  // 二进制掩码操作：0xFFF7FFFF

  /* 写入新的校准模式配置 */
  RTC->CR |= (uint32_t)RTC_CalibOutput;  // 设置CR[23]位：
                                        // 0x00000000→1Hz
                                        // 0x00080000→512Hz

  /* 重新激活寄存器写保护 */
  RTC->WPR = 0xFF;  // 写任意非密钥值即上锁
}

/**
  * @brief  RTC平滑校准配置
  * @param  RTC_SmoothCalibPeriod: 校准周期选择
  *         - 32秒/16秒/8秒（影响校准分辨率）
  * @param  RTC_SmoothCalibPlusPulses: 正向校准脉冲
  *         - 每2048个周期插入1个脉冲
  * @param  RTC_SmouthCalibMinusPulsesValue: 反向校准值（0-511）
  *         - 每个LSB对应0.9537ppm补偿
  * @retval ErrorStatus: 配置结果（SUCCESS/ERROR）
  * @detail 补偿公式：
  *         Calib = (CALP × 2 - CALM) / (2 × 校准周期)
  * @warning 校准时必须保持RTC时钟稳定
  */
ErrorStatus RTC_SmoothCalibConfig(uint32_t RTC_SmoothCalibPeriod,
                                  uint32_t RTC_SmoothCalibPlusPulses,
                                  uint32_t RTC_SmouthCalibMinusPulsesValue)
{
  ErrorStatus status = ERROR;
  uint32_t recalpfcount = 0;  // 超时计数器（防止死锁）

  /* 三重参数校验 */
  assert_param(IS_RTC_SMOOTH_CALIB_PERIOD(RTC_SmoothCalibPeriod));  // 检查周期参数
  assert_param(IS_RTC_SMOOTH_CALIB_PLUS(RTC_SmoothCalibPlusPulses)); // 检查CALP有效性
  assert_param(IS_RTC_SMOOTH_CALIB_MINUS(RTC_SmouthCalibMinusPulsesValue)); // 检查0x1FF边界

  /* RTC寄存器解锁流程 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  
  /* 检查校准挂起标志 */
  if ((RTC->ISR & RTC_ISR_RECALPF) != RESET)
  {
    /* 等待校准完成（超时保护） */
    while (((RTC->ISR & RTC_ISR_RECALPF) != RESET) && (recalpfcount != RECALPF_TIMEOUT))
    {
      recalpfcount++;  // 每次循环约消耗3个HCLK周期
    }
  }

  /* 确认校准状态可配置 */
  if ((RTC->ISR & RTC_ISR_RECALPF) == RESET)
  {
    /* 组合写入校准寄存器（CALR） */
    RTC->CALR = (uint32_t)(RTC_SmoothCalibPeriod | 
                          RTC_SmoothCalibPlusPulses |
                          RTC_SmouthCalibMinusPulsesValue);
    /* 典型配置示例：
       - 32秒周期 + 添加脉冲 + CALM=256 
       → 补偿值 = (1×2 - 256)/(2×32) = -3.9375 ppm */
    status = SUCCESS;  // 标记配置成功
  }
  else
  {
    status = ERROR;  // 超时或校准未完成
  }

  /* 重新启用寄存器保护 */
  RTC->WPR = 0xFF;
  
  return (ErrorStatus)(status);  // 返回最终操作状态
}

/**
  * @}
  */


/** @defgroup RTC_Group8 TimeStamp configuration functions
 *  @brief   TimeStamp configuration functions 
 *
@verbatim   
 ===============================================================================
                 ##### TimeStamp configuration functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  RTC时间戳功能配置
  * @param  RTC_TimeStampEdge: 时间戳触发边沿
  *         - RTC_TimeStampEdge_Rising:  上升沿触发
  *         - RTC_TimeStampEdge_Falling: 下降沿触发
  * @param  NewState: 时间戳功能状态（ENABLE/DISABLE）
  * @detail 硬件特性：
  *         - 触发事件将冻结当前时间到专用寄存器
  *         - 支持PC13引脚或内部事件触发
  * @warning 配置前需确保RTC时钟源稳定
  */
void RTC_TimeStampCmd(uint32_t RTC_TimeStampEdge, FunctionalState NewState)
{
  uint32_t tmpreg = 0;  // 临时寄存器操作缓存

  /* 参数双重验证 */
  assert_param(IS_RTC_TIMESTAMP_EDGE(RTC_TimeStampEdge));  // 检查边沿类型有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));            // 状态参数合法性校验

  /* 获取CR寄存器并清除目标配置位（TSEDGE[3]/TSE[11]） */
  tmpreg = (uint32_t)(RTC->CR & (uint32_t)~(RTC_CR_TSEDGE | RTC_CR_TSE)); // 掩码：0x00000808→0xFFFFF7F7

  /* 构建新配置 */
  if (NewState != DISABLE)
  {
    /* 启用时间戳：设置边沿类型+使能位 */
    tmpreg |= (uint32_t)(RTC_TimeStampEdge | RTC_CR_TSE);  // 示例：0x00000008 | 0x00000800
  }
  else
  {
    /* 禁用时间戳：仅保留边沿配置（实际无作用） */
    tmpreg |= (uint32_t)(RTC_TimeStampEdge);               // 保持寄存器配置整洁
  }

  /* RTC寄存器解锁序列 */
  RTC->WPR = 0xCA;  // 第一阶段解锁码
  RTC->WPR = 0x53;  // 第二阶段解锁码

  /* 写入新的时间戳配置 */
  RTC->CR = (uint32_t)tmpreg;  // 原子操作更新配置
  
  /* 重新激活写保护 */
  RTC->WPR = 0xFF;  // 写保护立即生效
}

/**
  * @brief  获取RTC时间戳详细数据
  * @param  RTC_Format: 数据格式（BCD或二进制）
  *         - RTC_Format_BIN: 原始二进制格式
  *         - RTC_Format_BCD: BCD编码格式
  * @param  RTC_StampTimeStruct: 时间结构体指针（输出时分秒）
  * @param  RTC_StampDateStruct: 日期结构体指针（输出月日周）
  * @detail 数据结构说明：
  *         - 时间结构体包含12/24小时制标志
  *         - 日期结构体年份固定为0（时间戳不保存年份）
  * @note 读取时刻到处理完成的延迟可能达2个HCLK周期
  */
void RTC_GetTimeStamp(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_StampTimeStruct, 
                                      RTC_DateTypeDef* RTC_StampDateStruct)
{
  uint32_t tmptime = 0, tmpdate = 0;

  /* 格式参数校验 */
  assert_param(IS_RTC_FORMAT(RTC_Format));  // 确保格式参数合法

  /* 读取时间戳寄存器（保留有效位） */
  tmptime = (uint32_t)(RTC->TSTR & RTC_TR_RESERVED_MASK);  // 时间寄存器掩码：0x007F7F7F
  tmpdate = (uint32_t)(RTC->TSDR & RTC_DR_RESERVED_MASK);   // 日期寄存器掩码：0x00FFFF3F

  /* 时间字段提取（保留原始BCD格式） */
  RTC_StampTimeStruct->RTC_Hours = (uint8_t)((tmptime & (RTC_TR_HT | RTC_TR_HU)) >> 16);  // 小时BCD值（bits[21:16]）
  RTC_StampTimeStruct->RTC_Minutes = (uint8_t)((tmptime & (RTC_TR_MNT | RTC_TR_MNU)) >> 8); // 分钟BCD（bits[15:8]）
  RTC_StampTimeStruct->RTC_Seconds = (uint8_t)(tmptime & (RTC_TR_ST | RTC_TR_SU));        // 秒BCD（bits[7:0]）
  RTC_StampTimeStruct->RTC_H12 = (uint8_t)((tmptime & (RTC_TR_PM)) >> 16);               // 上下午标志（bit22）

  /* 日期字段提取 */
  RTC_StampDateStruct->RTC_Year = 0;  // 年份数据未保存
  RTC_StampDateStruct->RTC_Month = (uint8_t)((tmpdate & (RTC_DR_MT | RTC_DR_MU)) >> 8); // 月份BCD（bits[19:8]）
  RTC_StampDateStruct->RTC_Date = (uint8_t)(tmpdate & (RTC_DR_DT | RTC_DR_DU));         // 日BCD（bits[7:0]）
  RTC_StampDateStruct->RTC_WeekDay = (uint8_t)((tmpdate & (RTC_DR_WDU)) >> 13);         // 周几（bits[15:13]）

  /* 格式转换处理 */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* BCD转二进制（各时间字段） */
    RTC_StampTimeStruct->RTC_Hours = (uint8_t)RTC_Bcd2ToByte(RTC_StampTimeStruct->RTC_Hours); // 例：0x23→35
    RTC_StampTimeStruct->RTC_Minutes = (uint8_t)RTC_Bcd2ToByte(RTC_StampTimeStruct->RTC_Minutes);
    RTC_StampTimeStruct->RTC_Seconds = (uint8_t)RTC_Bcd2ToByte(RTC_StampTimeStruct->RTC_Seconds);

    /* BCD转二进制（日期字段） */
    RTC_StampDateStruct->RTC_Month = (uint8_t)RTC_Bcd2ToByte(RTC_StampDateStruct->RTC_Month);
    RTC_StampDateStruct->RTC_Date = (uint8_t)RTC_Bcd2ToByte(RTC_StampDateStruct->RTC_Date);
    RTC_StampDateStruct->RTC_WeekDay = (uint8_t)RTC_Bcd2ToByte(RTC_StampDateStruct->RTC_WeekDay); // 周几范围：1-7
  }
}

/**
  * @brief  获取时间戳子秒值
  * @retval uint32_t: 当前子秒计数值（0-32767）
  * @detail 技术规格：
  *         - 分辨率：1/（PREDIV_S+1）秒
  *         - 典型值：PREDIV_S=255时，精度约3.9ms
  * @note 子秒值在时间戳事件时同步捕获
  */
uint32_t RTC_GetTimeStampSubSecond(void)
{
  /* 直接从子秒寄存器读取原始值 */
  return (uint32_t)(RTC->TSSSR);  // TSSSR[15:0]有效，范围0x0000-0x7FFF
}

/**
  * @}
  */

/** @defgroup RTC_Group9 Tampers configuration functions
 *  @brief   Tampers configuration functions 
 *
@verbatim   
 ===============================================================================
                 ##### Tampers configuration functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  配置侵入检测引脚触发方式
  * @param  RTC_Tamper: 侵入检测通道（仅支持TAMPER1）
  * @param  RTC_TamperTrigger: 触发类型
  *         - 上升沿/下降沿/低电平/高电平触发
  * @detail 硬件特性：
  *         - 侵入事件将重置后备寄存器
  *         - 触发方式影响TAFCR寄存器配置
  * @warning 配置后需锁定篡改检测功能
  */
void RTC_TamperTriggerConfig(uint32_t RTC_Tamper, uint32_t RTC_TamperTrigger)
{
  /* 参数双重校验 */
  assert_param(IS_RTC_TAMPER(RTC_Tamper));              // 目前仅支持RTC_Tamper_1(0x00000001)
  assert_param(IS_RTC_TAMPER_TRIGGER(RTC_TamperTrigger)); // 检查触发类型有效性

  /* 上升沿触发专用处理 */
  if (RTC_TamperTrigger == RTC_TamperTrigger_RisingEdge)
  {  
    /* 清除对应触发位（TAFCR[2] TAMP1TRG） */
    RTC->TAFCR &= (uint32_t)~(RTC_Tamper << 1);  // 0x00000001→左移1位得0x00000002，取反后清除
  }
  else
  { 
    /* 设置其他触发类型（TAFCR[2]） */
    RTC->TAFCR |= (uint32_t)(RTC_Tamper << 1);  // 下降沿/电平触发设置TAMP1TRG位
  }  
}

/**
  * @brief  启用或禁用篡改检测功能
  * @param  RTC_Tamper: 侵入检测通道（仅支持TAMPER1）
  * @param  NewState: 功能状态（ENABLE/DISABLE）
  * @detail 操作机制：
  *         - 启用时激活对应通道的检测电路
  *         - 禁用时停止采样以降低功耗
  * @warning 篡改事件将导致后备寄存器清零
  */
void RTC_TamperCmd(uint32_t RTC_Tamper, FunctionalState NewState)
{
  /* 参数合法性校验 */
  assert_param(IS_RTC_TAMPER(RTC_Tamper));  // 确认通道有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 状态参数校验

  if (NewState != DISABLE)
  {
    /* 设置使能位（TAFCR[0] TAMP1E） */
    RTC->TAFCR |= (uint32_t)RTC_Tamper;  // 0x00000001写入使能位
  }
  else
  {
    /* 清除使能位 */
    RTC->TAFCR &= (uint32_t)~RTC_Tamper;  // 0x00000001取反后清除    
  }  
}

/**
  * @brief  配置侵入检测滤波器
  * @param  RTC_TamperFilter: 滤波器采样次数
  *         - 禁用/2次/4次/8次连续采样
  * @detail 抗干扰机制：
  *         - 需连续N次检测到有效电平才触发
  *         - 采样率由RTCCLK分频决定
  * @note 滤波器可有效防止毛刺误触发
  */
void RTC_TamperFilterConfig(uint32_t RTC_TamperFilter)
{
  /* 参数有效性校验 */
  assert_param(IS_RTC_TAMPER_FILTER(RTC_TamperFilter));  // 检查是否为有效滤波模式
  
  /* 清除原有滤波配置位（TAFCR[1:0] TAMPFLT） */
  RTC->TAFCR &= (uint32_t)~(RTC_TAFCR_TAMPFLT);  // 掩码0x00000003→清除

  /* 写入新的滤波参数 */
  RTC->TAFCR |= (uint32_t)RTC_TamperFilter;  // 配置采样次数阈值
}

/**
  * @brief  Configures the Tampers Sampling Frequency.
  * @param  RTC_TamperSamplingFreq: Specifies the tampers Sampling Frequency.
  *   This parameter can be one of the following values:
  *     @arg RTC_TamperSamplingFreq_RTCCLK_Div32768: Each of the tamper inputs are sampled
  *                                           with a frequency =  RTCCLK / 32768
  *     @arg RTC_TamperSamplingFreq_RTCCLK_Div16384: Each of the tamper inputs are sampled
  *                                           with a frequency =  RTCCLK / 16384
  *     @arg RTC_TamperSamplingFreq_RTCCLK_Div8192: Each of the tamper inputs are sampled
  *                                           with a frequency =  RTCCLK / 8192
  *     @arg RTC_TamperSamplingFreq_RTCCLK_Div4096: Each of the tamper inputs are sampled
  *                                           with a frequency =  RTCCLK / 4096
  *     @arg RTC_TamperSamplingFreq_RTCCLK_Div2048: Each of the tamper inputs are sampled
  *                                           with a frequency =  RTCCLK / 2048
  *     @arg RTC_TamperSamplingFreq_RTCCLK_Div1024: Each of the tamper inputs are sampled
  *                                           with a frequency =  RTCCLK / 1024
  *     @arg RTC_TamperSamplingFreq_RTCCLK_Div512: Each of the tamper inputs are sampled
  *                                           with a frequency =  RTCCLK / 512  
  *     @arg RTC_TamperSamplingFreq_RTCCLK_Div256: Each of the tamper inputs are sampled
  *                                           with a frequency =  RTCCLK / 256  
  * @retval None
  */
/**
  * @brief  配置篡改检测采样频率
  * @param  RTC_TamperSamplingFreq: 采样频率分频系数
  *         - RTCCLK/32768至RTCCLK/256（约1.9Hz-1.63kHz @LSE=32.768kHz）
  * @detail 影响参数：
  *         - 频率越低，功耗越低，响应延迟越大
  *         - 高频采样适合快速检测场景
  * @note 实际采样周期 = 分频系数 × RTCCLK周期
  */
void RTC_TamperSamplingFreqConfig(uint32_t RTC_TamperSamplingFreq)
{
  /* 参数有效性验证（0x00000000-0x00070000） */
  assert_param(IS_RTC_TAMPER_SAMPLING_FREQ(RTC_TamperSamplingFreq));

  /* 清除原采样频率配置位（TAFCR[6:4] TAMPFREQ） */
  RTC->TAFCR &= (uint32_t)~(RTC_TAFCR_TAMPFREQ);  // 掩码0x00070000→0xFFF8FFFF

  /* 写入新的采样频率配置 */
  RTC->TAFCR |= (uint32_t)RTC_TamperSamplingFreq;  // 示例：0x00040000设置分频为RTCCLK/64
}

/**
  * @brief  配置篡改引脚预充电时间
  * @param  RTC_TamperPrechargeDuration: 预充电RTCCLK周期数（1/2/4/8）
  * @detail 功能说明：
  *         - 采样前对引脚进行预充电以稳定电平
  *         - 防止高阻态导致的误触发
  * @warning 环境噪声较大时应增加预充电时间
  */
void RTC_TamperPinsPrechargeDuration(uint32_t RTC_TamperPrechargeDuration)
{
  /* 参数范围校验（0x00000000-0x000000C0） */
  assert_param(IS_RTC_TAMPER_PRECHARGE_DURATION(RTC_TamperPrechargeDuration));
   
  /* 清除预充电配置位（TAFCR[8:7] TAMPPRCH） */
  RTC->TAFCR &= (uint32_t)~(RTC_TAFCR_TAMPPRCH);  // 掩码0x00000180→0xFFFFFE7F

  /* 写入新的预充电配置 */
  RTC->TAFCR |= (uint32_t)RTC_TamperPrechargeDuration;  // 示例：0x000000C0→8周期预充电
}

/**
  * @brief  篡改事件时是否记录时间戳
  * @param  NewState: ENABLE-记录时间戳 DISABLE-不记录
  * @detail 记录信息：
  *         - 时间戳存储到TSTR/TSDR寄存器
  *         - 子秒值存入TSSSR寄存器
  * @note 即使关闭篡改检测功能仍保留最后时间戳
  */
void RTC_TimeStampOnTamperDetectionCmd(FunctionalState NewState)
{
  /* 状态参数有效性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
   
  if (NewState != DISABLE)
  {
    /* 设置时间戳记录使能位（TAFCR[9] TAMPTS） */
    RTC->TAFCR |= (uint32_t)RTC_TAFCR_TAMPTS;  // 允许篡改触发时间戳
  }
  else
  {
    /* 关闭时间戳记录功能 */
    RTC->TAFCR &= (uint32_t)~RTC_TAFCR_TAMPTS;  // 篡改事件不产生时间戳    
  }
}

/**
  * @brief  篡改引脚上拉电阻控制
  * @param  NewState: ENABLE-启用内部上拉 DISABLE-禁用
  * @detail 电气特性：
  *         - 上拉电阻典型值：40kΩ
  *         - 引脚悬空时保持确定电平
  * @warning 外部已接上拉时需禁用此功能
  */
void RTC_TamperPullUpCmd(FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 启用内部上拉（清除TAFCR[2] TAMPPUDIS） */
    RTC->TAFCR &= (uint32_t)~RTC_TAFCR_TAMPPUDIS;  // 允许内部上拉工作
  }
  else
  {
    /* 禁用内部上拉 */
    RTC->TAFCR |= (uint32_t)RTC_TAFCR_TAMPPUDIS;  // 高阻态需外部偏置    
  } 
}


/**
  * @}
  */

/** @defgroup RTC_Group10 Backup Data Registers configuration functions
 *  @brief   Backup Data Registers configuration functions  
 *
@verbatim   
 ===============================================================================
             ##### Backup Data Registers configuration functions ##### 
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  写入RTC后备寄存器
  * @param  RTC_BKP_DR: 目标寄存器编号（0-19）
  * @param  Data: 待写入的32位数据
  * @detail 硬件特性：
  *         - 后备寄存器在VBAT供电下保持数据
  *         - 支持20个独立寄存器（DR0-DR19）
  * @warning 直接地址操作无需解除写保护
  */
void RTC_WriteBackupRegister(uint32_t RTC_BKP_DR, uint32_t Data)
{
  __IO uint32_t tmp = 0;  // 寄存器地址计算缓存
  
  /* 参数有效性验证（0-19） */
  assert_param(IS_RTC_BKP(RTC_BKP_DR));  // 检查寄存器编号范围

  /* 计算后备寄存器物理地址（RTC_BASE + 0x50 + 4*n） */
  tmp = RTC_BASE + 0x50;           // 后备寄存器基地址（0x40002850）
  tmp += (RTC_BKP_DR * 4);          // 寄存器偏移计算（每个寄存器4字节）

  /* 直接内存操作写入数据 */
  *(__IO uint32_t *)tmp = (uint32_t)Data;  // 示例：DR1地址为0x40002854
}

/**
  * @brief  读取RTC后备寄存器
  * @param  RTC_BKP_DR: 源寄存器编号（0-19）
  * @retval uint32_t: 读取到的32位数据
  * @detail 数据持久性：
  *         - 主电源掉电后数据由VBAT维持
  *         - 芯片复位不影响数据内容
  */
uint32_t RTC_ReadBackupRegister(uint32_t RTC_BKP_DR)
{
  __IO uint32_t tmp = 0;  // 寄存器地址计算缓存
  
  /* 参数有效性验证 */
  assert_param(IS_RTC_BKP(RTC_BKP_DR));  // 确保寄存器编号合法

  /* 计算后备寄存器物理地址 */
  tmp = RTC_BASE + 0x50;          // 后备寄存器基地址
  tmp += (RTC_BKP_DR * 4);         // 寄存器偏移

  /* 直接内存操作读取数据 */
  return (*(__IO uint32_t *)tmp);  // 返回寄存器当前值
}

/**
  * @brief  选择侵入检测引脚
  * @param  RTC_TamperPin: 引脚选择（PC13/PI8）
  * @detail 硬件限制：
  *         - PC13为默认侵入检测引脚
  *         - PI8仅在特定封装可用
  * @warning 引脚复用需同时配置GPIO时钟和复用功能
  */
void RTC_TamperPinSelection(uint32_t RTC_TamperPin)
{
  /* 参数有效性检查（0x00000000/0x00010000） */
  assert_param(IS_RTC_TAMPER_PIN(RTC_TamperPin));  // 验证引脚选项合法性
  
  /* 清除原有引脚配置位（TAFCR[18] TAMPINSEL） */
  RTC->TAFCR &= (uint32_t)~(RTC_TAFCR_TAMPINSEL);  // 掩码0x00040000→0xFFFBFFFF
  
  /* 写入新的引脚选择配置 */
  RTC->TAFCR |= (uint32_t)(RTC_TamperPin);         // PC13=0x00000000, PI8=0x00010000
}

/**
  * @brief  选择时间戳引脚
  * @param  RTC_TimeStampPin: 引脚选择（PC13/PI8）
  * @detail 电气连接：
  *         - 引脚需配置为输入浮空模式
  *         - 外部信号需满足RTC电气特性
  * @note 时间戳与侵入检测不可共用同一引脚
  */
void RTC_TimeStampPinSelection(uint32_t RTC_TimeStampPin)
{
  /* 参数合法性验证（0x00000000/0x00020000） */
  assert_param(IS_RTC_TIMESTAMP_PIN(RTC_TimeStampPin));  // 检查引脚选项
  
  /* 清除原有配置位（TAFCR[17] TSINSEL） */
  RTC->TAFCR &= (uint32_t)~(RTC_TAFCR_TSINSEL);  // 掩码0x00020000→0xFFFDFFFF
  
  /* 设置新的时间戳引脚 */
  RTC->TAFCR |= (uint32_t)(RTC_TimeStampPin);    // PC13=0x00000000, PI8=0x00020000
}
/**
  * @brief  配置RTC输出引脚驱动模式
  * @param  RTC_OutputType: 输出类型选择
  *         - RTC_OutputType_OpenDrain: 开漏模式（需外接上拉）
  *         - RTC_OutputType_PushPull:  推挽模式（内部驱动）
  * @detail 电气特性：
  *         - 开漏模式最大输出电流：20mA
  *         - 推挽模式上升时间：10ns（典型值）
  * @warning 改变输出模式需重新配置GPIO复用功能
  */
void RTC_OutputTypeConfig(uint32_t RTC_OutputType)
{
  /* 参数有效性验证（0x00000000/0x00040000） */
  assert_param(IS_RTC_OUTPUT_TYPE(RTC_OutputType));  // 检查模式参数合法性
  
  /* 清除原有输出类型配置位（TAFCR[19] ALARMOUTTYPE） */
  RTC->TAFCR &= (uint32_t)~(RTC_TAFCR_ALARMOUTTYPE);  // 掩码0x00080000→0xFFF7FFFF
  
  /* 写入新的输出模式 */
  RTC->TAFCR |= (uint32_t)(RTC_OutputType);  // 设置输出类型：0x00000000=开漏，0x00080000=推挽
}

/**
  * @brief  RTC时钟同步校准配置
  * @param  RTC_ShiftAdd1S: 是否增加1秒
  *         - SET: 日历增加1秒
  *         - RESET: 不调整整秒
  * @param  RTC_ShiftSubFS: 子秒补偿值（0-32767）
  * @retval ErrorStatus: 操作状态（SUCCESS/ERROR）
  * @detail 校准公式：
  *         总补偿量 = (Add1S × 1) - (SubFS × 1/32768) 秒
  * @warning 校准操作最大间隔：2^31 RTCCLK周期
  */
ErrorStatus RTC_SynchroShiftConfig(uint32_t RTC_ShiftAdd1S, uint32_t RTC_ShiftSubFS)
{
  ErrorStatus status = ERROR;
  uint32_t shpfcount = 0;  // 移位操作超时计数器

  /* 参数双重校验 */
  assert_param(IS_RTC_SHIFT_ADD1S(RTC_ShiftAdd1S));  // 检查秒增参数（0/0x80000000）
  assert_param(IS_RTC_SHIFT_SUBFS(RTC_ShiftSubFS));  // 检查子秒范围（0x00000000-0x00007FFF）

  /* RTC寄存器解锁流程 */
  RTC->WPR = 0xCA;  // 第一阶段解锁密钥
  RTC->WPR = 0x53;  // 第二阶段解锁密钥
  
  /* 检查移位操作挂起状态 */
  if ((RTC->ISR & RTC_ISR_SHPF) != RESET)
  {
    /* 等待前序移位完成（超时保护） */
    while (((RTC->ISR & RTC_ISR_SHPF) != RESET) && (shpfcount != SHPF_TIMEOUT))
    {
      shpfcount++;  // 每次循环约消耗3个HCLK周期
    }
  }

  /* 验证移位状态可操作 */
  if ((RTC->ISR & RTC_ISR_SHPF) == RESET)
  {
    /* 检查参考时钟检测状态 */
    if((RTC->CR & RTC_CR_REFCKON) == RESET)
    {
      /* 组合写入移位寄存器（SHIFTR） */
      RTC->SHIFTR = (uint32_t)(RTC_ShiftSubFS | RTC_ShiftAdd1S);  // 示例：0x00004000|0x80000000
      
      /* 等待寄存器同步完成 */
      if(RTC_WaitForSynchro() == SUCCESS)
      {
        status = SUCCESS;  // 校准操作成功
      }
    }
  }

  /* 重新启用写保护 */
  RTC->WPR = 0xFF;
  
  return (ErrorStatus)(status);  // 返回最终操作状态
}


/**
  * @}
  */

/** @defgroup RTC_Group13 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions  
 *
@verbatim   
 ===============================================================================
              ##### Interrupts and flags management functions #####
 ===============================================================================  
 [..] All RTC interrupts are connected to the EXTI controller.
 
   (+) To enable the RTC Alarm interrupt, the following sequence is required:
       (++) Configure and enable the EXTI Line 17 in interrupt mode and select 
            the rising edge sensitivity using the EXTI_Init() function.
       (++) Configure and enable the RTC_Alarm IRQ channel in the NVIC using the 
            NVIC_Init() function.
       (++) Configure the RTC to generate RTC alarms (Alarm A and/or Alarm B) using
            the RTC_SetAlarm() and RTC_AlarmCmd() functions.

   (+) To enable the RTC Wakeup interrupt, the following sequence is required:
       (++) Configure and enable the EXTI Line 22 in interrupt mode and select the
            rising edge sensitivity using the EXTI_Init() function.
       (++) Configure and enable the RTC_WKUP IRQ channel in the NVIC using the 
            NVIC_Init() function.
       (++) Configure the RTC to generate the RTC wakeup timer event using the 
            RTC_WakeUpClockConfig(), RTC_SetWakeUpCounter() and RTC_WakeUpCmd() 
            functions.

   (+) To enable the RTC Tamper interrupt, the following sequence is required:
       (++) Configure and enable the EXTI Line 21 in interrupt mode and select 
            the rising edge sensitivity using the EXTI_Init() function.
       (++) Configure and enable the TAMP_STAMP IRQ channel in the NVIC using the
            NVIC_Init() function.
       (++) Configure the RTC to detect the RTC tamper event using the 
            RTC_TamperTriggerConfig() and RTC_TamperCmd() functions.

   (+) To enable the RTC TimeStamp interrupt, the following sequence is required:
       (++) Configure and enable the EXTI Line 21 in interrupt mode and select the
            rising edge sensitivity using the EXTI_Init() function.
       (++) Configure and enable the TAMP_STAMP IRQ channel in the NVIC using the 
            NVIC_Init() function.
       (++) Configure the RTC to detect the RTC time stamp event using the 
            RTC_TimeStampCmd() functions.

@endverbatim
  * @{
  */
/**
  * @brief  RTC中断源使能配置
  * @param  RTC_IT: 中断类型组合（位或操作）：
  *         - RTC_IT_ALRA: 闹钟A中断
  *         - RTC_IT_ALRB: 闹钟B中断
  *         - RTC_IT_WUT:  唤醒定时器中断
  *         - RTC_IT_TS:   时间戳中断
  *         - RTC_IT_TAMP: 侵入检测中断
  * @param  NewState: 中断开关状态（ENABLE/DISABLE）
  * @detail 硬件机制：
  *         - 常规中断通过CR寄存器控制
  *         - 侵入中断通过TAFCR寄存器控制
  * @warning 需配合NVIC配置中断优先级
  */
void RTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState)
{
  /* 双参数合法性检查 */
  assert_param(IS_RTC_CONFIG_IT(RTC_IT));   // 验证中断类型组合有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 检查状态参数

  /* 解除RTC寄存器写保护 */
  RTC->WPR = 0xCA;  // 第一阶段解锁
  RTC->WPR = 0x53;  // 第二阶段解锁

  if (NewState != DISABLE)
  {
    /* 常规中断配置（CR寄存器） */
    RTC->CR |= (uint32_t)(RTC_IT & ~RTC_TAFCR_TAMPIE); // 排除侵入中断位
    /* 侵入中断特殊处理（TAFCR寄存器） */
    RTC->TAFCR |= (uint32_t)(RTC_IT & RTC_TAFCR_TAMPIE); // 0x00000001
  }
  else
  {
    /* 常规中断关闭 */
    RTC->CR &= (uint32_t)~(RTC_IT & ~RTC_TAFCR_TAMPIE);
    /* 侵入中断关闭 */
    RTC->TAFCR &= (uint32_t)~(RTC_IT & RTC_TAFCR_TAMPIE);
  }

  /* 恢复写保护 */
  RTC->WPR = 0xFF; 
}

/**
  * @brief  获取RTC状态标志位
  * @param  RTC_FLAG: 待查询标志位（共13种类型）
  * @retval FlagStatus: 标志位状态（SET/RESET）
  * @detail 标志位说明：
  *         - INITF: 初始化模式标志（0x40）
  *         - RSF:   寄存器同步标志（0x20）
  *         - SHPF:  移位挂起标志（0x1000）
  * @note 部分标志位需手动清除
  */
FlagStatus RTC_GetFlagStatus(uint32_t RTC_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tmpreg = 0;
  
  /* 参数有效性验证 */
  assert_param(IS_RTC_GET_FLAG(RTC_FLAG));  // 检查0x0001-0x4000范围
  
  /* 读取ISR寄存器并过滤保留位 */
  tmpreg = (uint32_t)(RTC->ISR & RTC_FLAGS_MASK);  // 有效标志掩码0x0000FFFF
  
  /* 位状态检测 */
  bitstatus = (tmpreg & RTC_FLAG) ? SET : RESET;  // 三目运算简化判断
  
  return bitstatus;  // 返回即时状态（不改变寄存器）
}

/**
  * @brief  清除RTC事件标志位
  * @param  RTC_FLAG: 待清除标志组合（支持多标志同时清除）
  * @detail 特殊处理：
  *         - 保留INITF位状态
  *         - 仅允许清除指定标志位
  * @warning 清除移位挂起标志需等待操作完成
  */
void RTC_ClearFlag(uint32_t RTC_FLAG)
{
  /* 参数合法性检查（排除不可清除标志） */
  assert_param(IS_RTC_CLEAR_FLAG(RTC_FLAG));  // 检查0x0001-0x0800范围
  
  /* 构造清除掩码并保留INIT位 */
  uint32_t mask = ~((RTC_FLAG | RTC_ISR_INIT) & 0x0000FFFF);  // 保留bit6
  RTC->ISR = (RTC->ISR & RTC_ISR_INIT) | (mask & 0x0000FFFF); // 原子操作
}

/**
  * @brief  获取有效中断状态（双重验证）
  * @param  RTC_IT: 中断类型：
  *         - RTC_IT_TAMP: 侵入中断（TAFCR控制）
  *         - 其他中断：CR寄存器控制
  * @retval ITStatus: 中断有效状态（SET需同时满足使能+触发）
  * @detail 实现机制：
  *         - 检查中断使能位（CR/TAFCR）
  *         - 检测事件标志位（ISR）
  */
ITStatus RTC_GetITStatus(uint32_t RTC_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t tmpreg = 0, enablestatus = 0;
 
  /* 参数有效性验证（0x00080000-0x00000008） */
  assert_param(IS_RTC_GET_IT(RTC_IT));  // 检查中断类型合法性
  
  /* 获取侵入中断使能状态 */
  tmpreg = RTC->TAFCR & RTC_TAFCR_TAMPIE;  // 提取TAFCR[2]
  
  /* 合成总使能状态（CR寄存器位+TAFCR位） */
  enablestatus = (RTC->CR & RTC_IT) | ((tmpreg << 15) & RTC_IT); // 对齐位域
  
  /* 获取事件标志状态（ISR寄存器对应位） */
  tmpreg = RTC->ISR & (RTC_IT >> 4);  // 标志位地址偏移计算
  
  /* 双重验证中断有效性 */
  bitstatus = (enablestatus && (tmpreg & 0x0000FFFF)) ? SET : RESET;
  
  return bitstatus;
}

/**
  * @brief  清除RTC模块的中断挂起状态标志
  * @param  RTC_IT: 需要清除的中断标志组合，支持以下值的按位或：
  *            @arg RTC_IT_TS:   时间戳检测中断（EXTI线21）
  *            @arg RTC_IT_WUT:  唤醒定时器溢出中断
  *            @arg RTC_IT_ALRB: 闹钟B触发中断
  *            @arg RTC_IT_ALRA: 闹钟A触发中断
  *            @arg RTC_IT_TAMP1: 防篡改检测中断（TAMPER1引脚事件）
  * @note 实现原理：
  * - 根据中断标志位在ISR寄存器的实际位置进行位掩码操作
  * - 通过位操作同时保留寄存器初始化状态(RTC_ISR_INIT)
  * - 中断标志位地址映射：参数值右移4位对应ISR寄存器位偏移
  * @warning 操作前必须确保RTC处于初始化模式或配置保护已解除
  * @retval 无
  */
void RTC_ClearITPendingBit(uint32_t RTC_IT)
{
  uint32_t tmpreg = 0;  // 临时变量用于构建中断位掩码

  /* 参数有效性验证 */
  // 使用断言检查中断标志组合的合法性
  // IS_RTC_CLEAR_IT宏验证参数是否包含有效标志组合
  assert_param(IS_RTC_CLEAR_IT(RTC_IT));

  /* 生成中断位掩码 */
  // 将输入参数右移4位获取实际ISR寄存器位偏移：
  // 例如RTC_IT_TS(0x00000008)右移4位得0x00000008，对应ISR[3]
  tmpreg = (uint32_t)(RTC_IT >> 4);

  /* 清除中断标志位操作 */
  // 复合位操作表达式分解说明：
  // 1. (tmpreg | RTC_ISR_INIT) → 合并中断掩码和初始化标志位
  // 2. & 0x0000FFFF → 保持低16位有效（ISR寄存器有效位段）
  // 3. ~取反操作 → 生成清除掩码（0表示需要清除的位）
  // 4. | (RTC->ISR & RTC_ISR_INIT) → 保持初始化状态位不变
  RTC->ISR = (uint32_t)((uint32_t)(~((tmpreg | RTC_ISR_INIT) & 0x0000FFFF) | 
                        (uint32_t)(RTC->ISR & RTC_ISR_INIT)));
}

/**
  * @}
  */
/**
  * @brief  两位十进制数转BCD编码
  * @param  Value: 待转换的十进制数（0-99）
  * @retval BCD编码结果（8位）
  * @detail 实现原理：
  *         - 十位数通过循环减法计算
  *         - 个位数保留原始余数
  *         - 组合成标准BCD格式
  * @warning 输入超过99将导致高位丢失
  */
static uint8_t RTC_ByteToBcd2(uint8_t Value)
{
  uint8_t bcdhigh = 0;  // BCD十位数存储
  
  /* 计算十位数值（通过循环减法）*/
  while (Value >= 10)    // 例如Value=35，循环执行3次
  {
    bcdhigh++;           // 十位值递增（结果：3）
    Value -= 10;         // 减去10（结果：5）
  }
  
  /* 组合BCD结果：十位左移4位 | 个位 */
  return  ((uint8_t)(bcdhigh << 4) | Value);  // 0x30 | 0x05 = 0x35
}

/**
  * @brief  BCD编码转两位十进制数
  * @param  Value: BCD编码值（0x00-0x99）
  * @retval 十进制数值（0-99）
  * @detail 解码过程：
  *         - 分离高4位和低4位
  *         - 十位乘10加个位
  * @note 无效BCD码（如0xFA）将产生错误结果
  */
static uint8_t RTC_Bcd2ToByte(uint8_t Value)
{
  uint8_t tmp = 0;
  /* 提取十位并转换为十进制 */
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10; // 0x23→0x02→2*10=20
  /* 加上个位数 */
  return (tmp + (Value & (uint8_t)0x0F)); // 20 + 0x03=23→35
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

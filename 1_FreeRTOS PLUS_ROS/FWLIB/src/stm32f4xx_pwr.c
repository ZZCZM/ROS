/**
  ******************************************************************************
  * @file    stm32f4xx_pwr.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Power Controller (PWR) peripheral:           
  *           + Backup Domain Access
  *           + PVD configuration
  *           + WakeUp pin configuration
  *           + Main and Backup Regulators configuration
  *           + FLASH Power Down configuration
  *           + Low Power modes configuration
  *           + Flags management
  *               
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
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup PWR 
  * @brief PWR driver modules
  * @{
  */ 
/* 电源控制(PWR)模块寄存器位定义 */
/* 以下宏定义用于位带操作，通过别名地址直接访问寄存器的特定位 */

/* 计算PWR模块在总线上的地址偏移量 */
#define PWR_OFFSET               (PWR_BASE - PERIPH_BASE)

/******************** 控制寄存器(CR)位定义 ********************/
/* --- CR寄存器位带别名地址计算 --- */

/* DBP位（禁用写保护）的位带别名地址计算：
   CR寄存器偏移量 = PWR偏移量 + 0x00 */
#define CR_OFFSET                (PWR_OFFSET + 0x00)
#define DBP_BitNumber            0x08  /* DBP位在CR寄存器中的位位置(第8位) */
/* 位带别名地址计算公式：外设位带基址 + (寄存器偏移*32) + (位号*4) */
#define CR_DBP_BB                (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))

/* PVDE位（可编程电压检测使能）的位带别名地址 */
#define PVDE_BitNumber           0x04  /* 第4位 */
#define CR_PVDE_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PVDE_BitNumber * 4))

/* FPDS位（Flash低功耗模式）的位带别名地址 */
#define FPDS_BitNumber           0x09  /* 第9位 */
#define CR_FPDS_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (FPDS_BitNumber * 4))

/* PMODE位（主调节器模式）的位带别名地址 */
#define PMODE_BitNumber          0x0E  /* 第14位 */
#define CR_PMODE_BB              (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PMODE_BitNumber * 4))

/* ODEN位（过驱动使能）的位带别名地址 */
#define ODEN_BitNumber           0x10  /* 第16位 */
#define CR_ODEN_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ODEN_BitNumber * 4))

/* ODSWEN位（过驱动开关使能）的位带别名地址 */
#define ODSWEN_BitNumber         0x11  /* 第17位 */
#define CR_ODSWEN_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ODSWEN_BitNumber * 4))

/* MRLVDS位（主调节器低电压检测）的位带别名地址 */
#define MRLVDS_BitNumber         0x0B  /* 第11位 */
#define CR_MRLVDS_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (MRLVDS_BitNumber * 4))

/* LPLVDS位（低功耗LDO电压检测）的位带别名地址 */
#define LPLVDS_BitNumber         0x0A  /* 第10位 */
#define CR_LPLVDS_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (LPLVDS_BitNumber * 4))

/******************** 控制状态寄存器(CSR)位定义 ********************/
/* --- CSR寄存器位带别名地址计算 --- */

/* EWUP位（使能唤醒引脚）的位带别名地址：
   CSR寄存器偏移量 = PWR偏移量 + 0x04 */
#define CSR_OFFSET               (PWR_OFFSET + 0x04)
#define EWUP_BitNumber           0x08  /* 第8位 */
#define CSR_EWUP_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP_BitNumber * 4))

/* BRE位（备份寄存器写使能）的位带别名地址 */
#define BRE_BitNumber            0x09  /* 第9位 */
#define CSR_BRE_BB               (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (BRE_BitNumber * 4))

/******************** 寄存器位掩码定义 ********************/
/* 用于批量操作寄存器位的掩码宏 */

/* 深度睡眠模式配置掩码（保留位2-9） */
#define CR_DS_MASK               ((uint32_t)0xFFFFF3FC)

/* 可编程电压检测级别选择掩码（保留位5-7） */
#define CR_PLS_MASK              ((uint32_t)0xFFFFFF1F)

/* 电压调节器缩放输出选择掩码（保留位14-15） */
#define CR_VOS_MASK              ((uint32_t)0xFFFF3FFF)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup PWR_Private_Functions
  * @{
  */

/** @defgroup PWR_Group1 Backup Domain Access function 
 *  @brief   Backup Domain Access function  
 *
@verbatim   
 ===============================================================================
                  ##### Backup Domain Access function #####
 ===============================================================================  
    [..]
      After reset, the backup domain (RTC registers, RTC backup data 
      registers and backup SRAM) is protected against possible unwanted 
      write accesses. 
      To enable access to the RTC Domain and RTC registers, proceed as follows:
        (+) Enable the Power Controller (PWR) APB1 interface clock using the
            RCC_APB1PeriphClockCmd() function.
        (+) Enable access to RTC domain using the PWR_BackupAccessCmd() function.

@endverbatim
  * @{
  */
/**
  * @brief  复位电源控制(PWR)外设至默认状态
  * @param  无
  * @retval 无
  * @note 该操作通过复位-释放机制实现：
  *        - 先使能外设复位，强制PWR进入复位状态
  *        - 再禁用复位，使PWR恢复工作状态
  * @warning 复位操作会清除所有PWR配置，包括：
  *          - 电源电压检测设置
  *          - 低功耗模式配置
  *          - 唤醒引脚配置等
  */
void PWR_DeInit(void)
{
  /* 通过RCC模块使能PWR外设的复位信号：
     - 这会复位PWR的所有寄存器至默认值
     - 相当于硬件复位PWR模块 */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);  // 启动复位信号

  /* 立即禁用复位信号，完成复位操作：
     - 复位信号只需要维持一个时钟周期即可生效
     - 该操作序列是STM32标准外设复位标准流程 */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE); // 释放复位信号
}


/**
  * @brief  启用/禁用备份域访问权限
  * @param  NewState: 访问控制状态（ENABLE/DISABLE）
  * @note 关键功能：
  *        - 控制PWR_CR寄存器的DBP位（位8）
  *        - 访问RTC/BKP寄存器前必须使能
  *        - 使用HSE分频作为RTC时钟时必须保持ENABLE状态
  * @warning 错误操作可能导致：
  *          - RTC时钟失效
  *          - 备份寄存器写入失败
  */
void PWR_BackupAccessCmd(FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 校验ENABLE/DISABLE参数
  
  /* 通过位带操作原子性地修改DBP位：
     0x40007000是PWR_CR寄存器的位带别名地址
     0x42000000是位带区域基址
     (0x40007000 - 0x42000000)*32 + 8*4 = 0x21810 计算得出DBP位的位带地址 */
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)NewState; // 直接操作位带地址实现原子访问
}


/**
  * @}
  */

/** @defgroup PWR_Group2 PVD configuration functions
 *  @brief   PVD configuration functions 
 *
@verbatim   
 ===============================================================================
                    ##### PVD configuration functions #####
 ===============================================================================  
    [..]
      (+) The PVD is used to monitor the VDD power supply by comparing it to a 
          threshold selected by the PVD Level (PLS[2:0] bits in the PWR_CR).
      (+) A PVDO flag is available to indicate if VDD/VDDA is higher or lower 
          than the PVD threshold. This event is internally connected to the EXTI 
          line16 and can generate an interrupt if enabled through the EXTI registers.
      (+) The PVD is stopped in Standby mode.

@endverbatim
  * @{
  */

/**
  * @brief  Configures the voltage threshold detected by the Power Voltage Detector(PVD).
  * @param  PWR_PVDLevel: specifies the PVD detection level
  *          This parameter can be one of the following values:
  *            @arg PWR_PVDLevel_0
  *            @arg PWR_PVDLevel_1
  *            @arg PWR_PVDLevel_2
  *            @arg PWR_PVDLevel_3
  *            @arg PWR_PVDLevel_4
  *            @arg PWR_PVDLevel_5
  *            @arg PWR_PVDLevel_6
  *            @arg PWR_PVDLevel_7
  * @note   Refer to the electrical characteristics of your device datasheet for
  *         more details about the voltage threshold corresponding to each 
  *         detection level.
  * @retval None
  */
/**
  * @brief  配置电源电压检测器(PVD)的触发阈值等级
  * @param  PWR_PVDLevel: 电压检测阈值等级，取值范围：
  *            PWR_PVDLevel_0 ~ PWR_PVDLevel_7（对应2.0V~2.9V，步进0.1V）
  * @retval 无
  * @note 阈值等级与芯片实际供电电压相关，具体对应关系需参考芯片数据手册
  * @warning 修改阈值后需重新使能PVD才能生效
  */
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel)
{
  uint32_t tmpreg = 0;  // 临时寄存器操作变量
  
  /* 验证输入参数的有效性 */
  assert_param(IS_PWR_PVD_LEVEL(PWR_PVDLevel));
  
  /* 读取当前CR寄存器值到临时变量 */
  tmpreg = PWR->CR;  // 获取当前控制寄存器状态
  
  /* 清除PLS[7:5]位域（电压等级设置位） */
  tmpreg &= CR_PLS_MASK;  // 使用预定义的位掩码清除旧值
  
  /* 设置新的电压检测阈值等级 */
  tmpreg |= PWR_PVDLevel;  // 通过位或操作写入新值
  
  /* 将新配置写回CR寄存器 */
  PWR->CR = tmpreg;  // 原子操作更新寄存器
}

/**
  * @brief  启用/禁用电源电压检测器(PVD)
  * @param  NewState: 功能状态（ENABLE/DISABLE）
  * @retval 无
  * @note 启用后，当VDD电压低于设定阈值时将触发PVD中断
  * @warning 需要先配置阈值等级再启用检测功能
  */
void PWR_PVDCmd(FunctionalState NewState)
{
  /* 验证状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 通过位带操作直接设置PVDE控制位：
     - 使用CR_PVDE_BB地址直接访问位
     - 实现无需读-修改-写的原子操作 */
  *(__IO uint32_t *) CR_PVDE_BB = (uint32_t)NewState;  // 直接操作位带别名区
}

/**
  * @brief  启用/禁用唤醒引脚功能
  * @param  NewState: 功能状态（ENABLE/DISABLE）
  * @retval 无
  * @note 唤醒引脚特性：
  *        - 固定配置为带下拉的输入模式
  *        - 仅响应上升沿触发
  *        - 物理映射到PA.00引脚
  * @warning 仅在待机模式下有效，正常运行时无作用
  */
void PWR_WakeUpPinCmd(FunctionalState NewState)
{
  /* 验证输入参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 通过位带操作设置EWUP控制位：
     - 访问CSR_EWUP_BB位带地址
     - 实现快速开关唤醒功能 */
  *(__IO uint32_t *) CSR_EWUP_BB = (uint32_t)NewState;  // 原子操作唤醒引脚使能位
}


/**
  * @}
  */

/** @defgroup PWR_Group4 Main and Backup Regulators configuration functions
 *  @brief   Main and Backup Regulators configuration functions 
 *
@verbatim   
 ===============================================================================
          ##### Main and Backup Regulators configuration functions #####
 ===============================================================================  
    [..]
      (+) The backup domain includes 4 Kbytes of backup SRAM accessible only from 
          the CPU, and address in 32-bit, 16-bit or 8-bit mode. Its content is 
          retained even in Standby or VBAT mode when the low power backup regulator
          is enabled. It can be considered as an internal EEPROM when VBAT is 
          always present. You can use the PWR_BackupRegulatorCmd() function to 
          enable the low power backup regulator and use the PWR_GetFlagStatus
          (PWR_FLAG_BRR) to check if it is ready or not. 

      (+) When the backup domain is supplied by VDD (analog switch connected to VDD) 
          the backup SRAM is powered from VDD which replaces the VBAT power supply to 
          save battery life.

      (+) The backup SRAM is not mass erased by an tamper event. It is read 
          protected to prevent confidential data, such as cryptographic private 
          key, from being accessed. The backup SRAM can be erased only through 
          the Flash interface when a protection level change from level 1 to 
          level 0 is requested. 
      -@- Refer to the description of Read protection (RDP) in the reference manual.

      (+) The main internal regulator can be configured to have a tradeoff between 
          performance and power consumption when the device does not operate at 
          the maximum frequency. 
      (+) For STM32F405xx/407xx and STM32F415xx/417xx  Devices, the regulator can be     
          configured on the fly through PWR_MainRegulatorModeConfig() function which  
          configure VOS bit in PWR_CR register:
        (++) When this bit is set (Regulator voltage output Scale 1 mode selected) 
             the System frequency can go up to 168 MHz. 
        (++) When this bit is reset (Regulator voltage output Scale 2 mode selected) 
             the System frequency can go up to 144 MHz.
             
       (+) For STM32F42xxx/43xxx Devices, the regulator can be configured through    
           PWR_MainRegulatorModeConfig() function which configure VOS[1:0] bits in
           PWR_CR register:  
           which configure VOS[1:0] bits in PWR_CR register: 
        (++) When VOS[1:0] = 11 (Regulator voltage output Scale 1 mode selected) 
             the System frequency can go up to 168 MHz. 
        (++) When VOS[1:0] = 10 (Regulator voltage output Scale 2 mode selected) 
             the System frequency can go up to 144 MHz.  
        (++) When VOS[1:0] = 01 (Regulator voltage output Scale 3 mode selected) 
             the System frequency can go up to 120 MHz. 
                          
       (+) For STM32F42xxx/43xxx Devices, the scale can be modified only when the PLL 
           is OFF and the HSI or HSE clock source is selected as system clock. 
           The new value programmed is active only when the PLL is ON.
           When the PLL is OFF, the voltage scale 3 is automatically selected. 
        Refer to the datasheets for more details.
        
       (+) For STM32F42xxx/43xxx Devices, in Run mode: the main regulator has
           2 operating modes available:
        (++) Normal mode: The CPU and core logic operate at maximum frequency at a given 
             voltage scaling (scale 1, scale 2 or scale 3)
        (++) Over-drive mode: This mode allows the CPU and the core logic to operate at a 
            higher frequency than the normal mode for a given voltage scaling (scale 1,  
            scale 2 or scale 3). This mode is enabled through PWR_OverDriveCmd() function and
            PWR_OverDriveSWCmd() function, to enter or exit from Over-drive mode please follow 
            the sequence described in Reference manual.
             
       (+) For STM32F42xxx/43xxx Devices, in Stop mode: the main regulator or low power regulator 
           supplies a low power voltage to the 1.2V domain, thus preserving the content of registers 
           and internal SRAM. 2 operating modes are available:
         (++) Normal mode: the 1.2V domain is preserved in nominal leakage mode. This mode is only 
              available when the main regulator or the low power regulator is used in Scale 3 or 
              low voltage mode.
         (++) Under-drive mode: the 1.2V domain is preserved in reduced leakage mode. This mode is only
              available when the main regulator or the low power regulator is in low voltage mode.
              This mode is enabled through PWR_UnderDriveCmd() function.
            
@endverbatim
  * @{
  */
/**
  * @brief  启用/禁用备份域稳压器
  * @param  NewState: 控制状态（ENABLE/DISABLE）
  * @retval 无
  * @note 备份域稳压器特性：
  *        - 为备份域（RTC、备份寄存器）提供稳定电源
  *        - 在低功耗模式下保持备份域供电
  * @warning 禁用时备份域数据可能丢失
  */
void PWR_BackupRegulatorCmd(FunctionalState NewState)
{
  /* 验证参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 通过位带操作直接控制BRE位：
     - CSR_BRE_BB是备份稳压器使能位的别名地址
     - 原子操作保证寄存器访问安全性 */
  *(__IO uint32_t *) CSR_BRE_BB = (uint32_t)NewState;  // 直接写入位带地址
}

/**
  * @brief  配置主稳压器输出电压等级
  * @param  PWR_Regulator_Voltage: 电压等级，可选值：
  *            PWR_Regulator_Voltage_Scale1 - 最高性能模式（168MHz）
  *            PWR_Regulator_Voltage_Scale2 - 平衡模式（144MHz）
  *            PWR_Regulator_Voltage_Scale3 - 低功耗模式（120MHz，仅F42x/43x）
  * @retval 无
  * @note 电压等级与系统时钟关系：
  *        - Scale1需要更高供电电压
  *        - 降低电压等级可减少功耗但限制最大频率
  * @warning 修改后需重新配置时钟系统
  */
void PWR_MainRegulatorModeConfig(uint32_t PWR_Regulator_Voltage)
{
  uint32_t tmpreg = 0;  // 临时寄存器操作变量
  
  /* 验证电压等级参数有效性 */
  assert_param(IS_PWR_REGULATOR_VOLTAGE(PWR_Regulator_Voltage));

  tmpreg = PWR->CR;  // 读取当前控制寄存器状态
  
  /* 清除VOS[15:14]位域（电压调节位） */
  tmpreg &= CR_VOS_MASK;  // 使用预定义掩码清除旧配置
  
  /* 设置新的电压等级 */
  tmpreg |= PWR_Regulator_Voltage;  // 位或操作写入新值
  
  /* 更新控制寄存器 */
  PWR->CR = tmpreg;  // 原子操作写入新配置
}

/**
  * @brief  启用/禁用过驱动模式
  * @param  NewState: 控制状态（ENABLE/DISABLE）
  * @retval 无
  * @note 过驱动模式特性：
  *        - 仅适用于STM32F42xxx/43xxx系列
  *        - 允许在相同电压下运行更高频率
  *        - 需与PWR_OverDriveSWCmd配合使用
  * @warning 操作时应：
  *          - 关闭外设时钟
  *          - 使用HSI/HSE时钟源
  *          - 避免关键任务执行期间切换
  */
void PWR_OverDriveCmd(FunctionalState NewState)
{
  /* 验证状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 通过位带操作设置ODEN位：
     - 使用CR_ODEN_BB地址直接访问过驱动使能位
     - 实现无竞争条件的原子操作 */
  *(__IO uint32_t *) CR_ODEN_BB = (uint32_t)NewState;  // 直接控制硬件位
}

/**
  * @brief  启用/禁用过驱动模式切换
  * @param  NewState: 控制状态（ENABLE/DISABLE）
  * @retval 无
  * @note 功能特性：
  *        - 仅适用于STM32F42xxx/43xxx系列
  *        - 必须与过驱动模式配合使用
  *        - 控制稳压器输出切换时序
  * @warning 需先使能过驱动模式再操作本功能
  */
void PWR_OverDriveSWCmd(FunctionalState NewState)
{
  /* 验证输入参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 通过位带操作设置ODSWEN位：
     - 使用CR_ODSWEN_BB地址直接访问切换使能位
     - 确保稳压器切换过程可控 */
  *(__IO uint32_t *) CR_ODSWEN_BB = (uint32_t)NewState;  // 原子操作切换使能位
}

/**
  * @brief  启用/禁用欠驱动模式
  * @param  NewState: 控制状态（ENABLE/DISABLE）
  * @retval 无
  * @note 功能特性：
  *        - 仅适用于STM32F42xxx/43xxx系列
  *        - 仅在STOP低功耗模式下有效
  *        - 降低1.2V域功耗但增加唤醒延迟
  * @warning 使用限制：
  *          - 需主/低功耗稳压器处于低电压模式
  *          - 退出STOP模式后自动禁用
  */
void PWR_UnderDriveCmd(FunctionalState NewState)
{
  /* 验证状态参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 设置UDEN[1:0]位使能欠驱动模式：
       - PWR_CR_UDEN对应两个控制位的掩码
       - 进入深度低功耗状态 */
    PWR->CR |= (uint32_t)PWR_CR_UDEN;  // 位或操作设置控制位
  }
  else
  {
    /* 清除UDEN[1:0]位禁用模式：
       - 保留其他位状态
       - 恢复常规功耗模式 */
    PWR->CR &= (uint32_t)(~PWR_CR_UDEN);  // 位与操作清除控制位
  }
}

/**
  * @brief  启用/禁用主稳压器低电压模式
  * @param  NewState: 控制状态（ENABLE/DISABLE）
  * @retval 无
  * @note 功能特性：
  *        - 仅适用于STM32F401xx/411xx系列
  *        - 降低主稳压器输出电压
  *        - 减少功耗但限制性能
  * @warning 需与系统时钟配置匹配
  */
void PWR_MainRegulatorLowVoltageCmd(FunctionalState NewState)
{ 
  /* 验证输入参数有效性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 通过位带操作使能MRLVDS位：
       - 直接访问CR_MRLVDS_BB地址
       - 原子操作保证寄存器安全 */
    *(__IO uint32_t *) CR_MRLVDS_BB = (uint32_t)ENABLE;  // 置位低电压模式
  }
  else
  {
    /* 禁用主稳压器低电压模式：
       - 恢复标准电压输出
       - 需满足当前系统时钟要求 */
    *(__IO uint32_t *) CR_MRLVDS_BB = (uint32_t)DISABLE;  // 清除控制位
  }
}

/**
  * @brief  启用/禁用低功耗稳压器低电压模式
  * @param  NewState: 控制状态（ENABLE/DISABLE）
  * @retval 无
  * @note 功能特性：
  *        - 仅适用于STM32F401xx/411xx系列
  *        - 在低功耗模式下优化能耗
  *        - 需与主稳压器模式配合使用
  * @warning 影响范围：
  *          - 仅影响低功耗运行状态
  *          - 需配置正确的唤醒机制
  */
void PWR_LowRegulatorLowVoltageCmd(FunctionalState NewState)
{
  /* 验证状态参数合法性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 通过位带操作设置LPLVDS位：
       - 访问CR_LPLVDS_BB别名地址
       - 确保低功耗稳压器工作模式 */
    *(__IO uint32_t *) CR_LPLVDS_BB = (uint32_t)ENABLE;  // 进入低电压状态
  }
  else
  {
    /* 禁用低功耗稳压器低电压模式：
       - 恢复默认电压输出
       - 退出低功耗优化状态 */
    *(__IO uint32_t *) CR_LPLVDS_BB = (uint32_t)DISABLE;  // 退出低电压状态
  }
}


/**
  * @}
  */

/** @defgroup PWR_Group5 FLASH Power Down configuration functions
 *  @brief   FLASH Power Down configuration functions 
 *
@verbatim   
 ===============================================================================
             ##### FLASH Power Down configuration functions #####
 ===============================================================================  
    [..]
      (+) By setting the FPDS bit in the PWR_CR register by using the 
          PWR_FlashPowerDownCmd() function, the Flash memory also enters power 
          down mode when the device enters Stop mode. When the Flash memory 
          is in power down mode, an additional startup delay is incurred when 
          waking up from Stop mode.
@endverbatim
  * @{
  */
/**
  * @brief  控制STOP模式下Flash存储器掉电状态
  * @param  NewState: 控制状态（ENABLE/DISABLE）
  * @retval 无
  * @note 功能特性：
  *        - 使能时：STOP模式下关闭Flash电源，显著降低功耗
  *        - 禁用时：保持Flash供电，加快唤醒恢复速度
  * @warning 影响说明：
  *          - 使能后会增加从STOP模式唤醒的时间
  *          - 需在进入STOP模式前配置本功能
  */
void PWR_FlashPowerDownCmd(FunctionalState NewState)
{
  /* 验证输入参数合法性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 通过位带操作设置FPDS控制位：
     - CR_FPDS_BB是CR寄存器第9位的别名地址
     - 直接操作硬件寄存器位，实现原子级修改
     - ENABLE: 进入掉电模式（约节省30μA）
     - DISABLE: 保持正常供电 */
  *(__IO uint32_t *) CR_FPDS_BB = (uint32_t)NewState;  // 单周期完成位操作
}

/**
  * @}
  */

/** @defgroup PWR_Group6 Low Power modes configuration functions
 *  @brief   Low Power modes configuration functions 
 *
@verbatim   
 ===============================================================================
              ##### Low Power modes configuration functions #####
 ===============================================================================  
    [..]
      The devices feature 3 low-power modes:
      (+) Sleep mode: Cortex-M4 core stopped, peripherals kept running.
      (+) Stop mode: all clocks are stopped, regulator running, regulator 
          in low power mode
      (+) Standby mode: 1.2V domain powered off.
   
   *** Sleep mode ***
   ==================
    [..]
      (+) Entry:
        (++) The Sleep mode is entered by using the __WFI() or __WFE() functions.
      (+) Exit:
        (++) Any peripheral interrupt acknowledged by the nested vectored interrupt 
             controller (NVIC) can wake up the device from Sleep mode.

   *** Stop mode ***
   =================
    [..]
      In Stop mode, all clocks in the 1.2V domain are stopped, the PLL, the HSI,
      and the HSE RC oscillators are disabled. Internal SRAM and register contents 
      are preserved.
      The voltage regulator can be configured either in normal or low-power mode.
      To minimize the consumption In Stop mode, FLASH can be powered off before 
      entering the Stop mode. It can be switched on again by software after exiting 
      the Stop mode using the PWR_FlashPowerDownCmd() function. 
   
      (+) Entry:
        (++) The Stop mode is entered using the PWR_EnterSTOPMode(PWR_MainRegulator_ON) 
             function with:
          (+++) Main regulator ON.
          (+++) Low Power regulator ON.
      (+) Exit:
        (++) Any EXTI Line (Internal or External) configured in Interrupt/Event mode.
      
   *** Standby mode ***
   ====================
    [..]
      The Standby mode allows to achieve the lowest power consumption. It is based 
      on the Cortex-M4 deepsleep mode, with the voltage regulator disabled. 
      The 1.2V domain is consequently powered off. The PLL, the HSI oscillator and 
      the HSE oscillator are also switched off. SRAM and register contents are lost 
      except for the RTC registers, RTC backup registers, backup SRAM and Standby 
      circuitry.
   
      The voltage regulator is OFF.
      
      (+) Entry:
        (++) The Standby mode is entered using the PWR_EnterSTANDBYMode() function.
      (+) Exit:
        (++) WKUP pin rising edge, RTC alarm (Alarm A and Alarm B), RTC wakeup,
             tamper event, time-stamp event, external reset in NRST pin, IWDG reset.              

   *** Auto-wakeup (AWU) from low-power mode ***
   =============================================
    [..]
      The MCU can be woken up from low-power mode by an RTC Alarm event, an RTC 
      Wakeup event, a tamper event, a time-stamp event, or a comparator event, 
      without depending on an external interrupt (Auto-wakeup mode).

      (#) RTC auto-wakeup (AWU) from the Stop mode
       
        (++) To wake up from the Stop mode with an RTC alarm event, it is necessary to:
          (+++) Configure the EXTI Line 17 to be sensitive to rising edges (Interrupt 
                or Event modes) using the EXTI_Init() function.
          (+++) Enable the RTC Alarm Interrupt using the RTC_ITConfig() function
          (+++) Configure the RTC to generate the RTC alarm using the RTC_SetAlarm() 
                and RTC_AlarmCmd() functions.
        (++) To wake up from the Stop mode with an RTC Tamper or time stamp event, it 
             is necessary to:
          (+++) Configure the EXTI Line 21 to be sensitive to rising edges (Interrupt 
                or Event modes) using the EXTI_Init() function.
          (+++) Enable the RTC Tamper or time stamp Interrupt using the RTC_ITConfig() 
                function
          (+++) Configure the RTC to detect the tamper or time stamp event using the
                RTC_TimeStampConfig(), RTC_TamperTriggerConfig() and RTC_TamperCmd()
                functions.
        (++) To wake up from the Stop mode with an RTC WakeUp event, it is necessary to:
           (+++) Configure the EXTI Line 22 to be sensitive to rising edges (Interrupt 
                 or Event modes) using the EXTI_Init() function.
           (+++) Enable the RTC WakeUp Interrupt using the RTC_ITConfig() function
           (+++) Configure the RTC to generate the RTC WakeUp event using the RTC_WakeUpClockConfig(), 
                 RTC_SetWakeUpCounter() and RTC_WakeUpCmd() functions.

      (#) RTC auto-wakeup (AWU) from the Standby mode
   
        (++) To wake up from the Standby mode with an RTC alarm event, it is necessary to:
          (+++) Enable the RTC Alarm Interrupt using the RTC_ITConfig() function
          (+++) Configure the RTC to generate the RTC alarm using the RTC_SetAlarm() 
                and RTC_AlarmCmd() functions.
        (++) To wake up from the Standby mode with an RTC Tamper or time stamp event, it 
             is necessary to:
          (+++) Enable the RTC Tamper or time stamp Interrupt using the RTC_ITConfig() 
                function
          (+++) Configure the RTC to detect the tamper or time stamp event using the
                RTC_TimeStampConfig(), RTC_TamperTriggerConfig() and RTC_TamperCmd()
                functions.
        (++) To wake up from the Standby mode with an RTC WakeUp event, it is necessary to:
          (+++) Enable the RTC WakeUp Interrupt using the RTC_ITConfig() function
          (+++) Configure the RTC to generate the RTC WakeUp event using the RTC_WakeUpClockConfig(), 
                RTC_SetWakeUpCounter() and RTC_WakeUpCmd() functions.

@endverbatim
  * @{
  */

/**
  * @brief  Enters STOP mode.
  *   
  * @note   In Stop mode, all I/O pins keep the same state as in Run mode.
  * @note   When exiting Stop mode by issuing an interrupt or a wakeup event, 
  *         the HSI RC oscillator is selected as system clock.
  * @note   When the voltage regulator operates in low power mode, an additional 
  *         startup delay is incurred when waking up from Stop mode. 
  *         By keeping the internal regulator ON during Stop mode, the consumption 
  *         is higher although the startup time is reduced.
  *     
  * @param  PWR_Regulator: specifies the regulator state in STOP mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_MainRegulator_ON: STOP mode with regulator ON
  *            @arg PWR_LowPowerRegulator_ON: STOP mode with low power regulator ON
  * @param  PWR_STOPEntry: specifies if STOP mode in entered with WFI or WFE instruction.
  *          This parameter can be one of the following values:
  *            @arg PWR_STOPEntry_WFI: enter STOP mode with WFI instruction
  *            @arg PWR_STOPEntry_WFE: enter STOP mode with WFE instruction
  * @retval None
  */
/**
  * @brief  使系统进入STOP低功耗模式
  * @param  PWR_Regulator: 稳压器工作模式，可选值：
  *            PWR_Regulator_LowPower - 低功耗稳压器模式
  *            PWR_Regulator_Main - 主稳压器模式
  * @param  PWR_STOPEntry: 进入模式的方式：
  *            PWR_STOPEntry_WFI - 通过WFI指令进入
  *            PWR_STOPEntry_WFE - 通过WFE指令进入
  * @retval 无
  * @note 重要特性：
  *        - 保留所有IO引脚状态
  *        - 唤醒后系统时钟切换至HSI（16MHz）
  *        - 退出后需重新配置时钟系统
  * @warning 进入前需：
  *          - 关闭非必要外设时钟
  *          - 配置唤醒源（EXTI/RTC等）
  */
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
  uint32_t tmpreg = 0;
  
  /* 验证输入参数合法性 */
  assert_param(IS_PWR_REGULATOR(PWR_Regulator));    // 检查稳压器模式参数
  assert_param(IS_PWR_STOP_ENTRY(PWR_STOPEntry));   // 检查进入方式参数
  
  /* 配置STOP模式下的稳压器状态 --------------------------------*/
  tmpreg = PWR->CR;                    // 读取当前电源控制寄存器值
  tmpreg &= CR_DS_MASK;                // 清除PDDS和LPDS位（深度睡眠配置位）
  tmpreg |= PWR_Regulator;             // 设置新的稳压器模式（低功耗/主稳压器）
  PWR->CR = tmpreg;                    // 写回修改后的配置
  
  /* 配置Cortex-M4休眠深度 ------------------------------------*/
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;   // 设置SLEEPDEEP位使能深度睡眠模式
  
  /* 选择进入STOP模式的方式 -----------------------------------*/
  if(PWR_STOPEntry == PWR_STOPEntry_WFI)
  {   
    __WFI();  // 执行等待中断指令，进入停机模式
  }
  else
  {
    __WFE();  // 执行等待事件指令，进入停机模式
  }
  
  /* 退出后恢复系统控制寄存器 --------------------------------*/
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;  // 清除深度睡眠标志位
}

/**
  * @brief  进入欠驱动STOP模式（超低功耗）
  * @param  PWR_Regulator: 稳压器工作模式，可选：
  *            PWR_MainRegulator_UnderDrive_ON - 主稳压器欠驱动+Flash掉电
  *            PWR_LowPowerRegulator_UnderDrive_ON - 低功耗稳压器欠驱动+Flash掉电
  * @param  PWR_STOPEntry: 进入方式（WFI/WFE）
  * @retval 无
  * @note 特殊限制：
  *        - 仅适用于STM32F42xxx/43xxx系列
  *        - 需预先使能欠驱动模式
  *        - 唤醒延迟增加约5μs
  * @warning 硬件特性：
  *          - 退出后自动恢复主稳压器正常模式
  *          - Flash需重新初始化
  */
void PWR_EnterUnderDriveSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
  uint32_t tmpreg = 0;
  
  /* 参数检查（增强型校验） */
  assert_param(IS_PWR_REGULATOR_UNDERDRIVE(PWR_Regulator)); // 校验欠驱动专用参数
  assert_param(IS_PWR_STOP_ENTRY(PWR_STOPEntry));           // 校验进入方式
  
  /* 配置欠驱动稳压器状态 --------------------------------*/
  tmpreg = PWR->CR;                    // 获取当前电源控制状态
  tmpreg &= CR_DS_MASK;                // 清除原有的低功耗配置位
  tmpreg |= PWR_Regulator;             // 设置欠驱动专用位（MRLUDS/LPLUDS）
  PWR->CR = tmpreg;                    // 原子操作更新寄存器
  
  /* 准备深度休眠 ----------------------------------------*/
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;   // 激活Cortex-M4深度休眠特性
  
  /* 执行停机指令 ----------------------------------------*/
  if(PWR_STOPEntry == PWR_STOPEntry_WFI) {
    __WFI();  // 硬件级停机指令，触发后立即进入低功耗状态
  } else {
    __WFE();  // 事件驱动型停机，适合事件唤醒场景
  }
  
  /* 系统恢复后处理 --------------------------------------*/
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;  // 必须清除深度睡眠标志
}

/**
  * @brief  使系统进入待机模式（最低功耗模式）
  * @note 进入条件与系统行为：
  * - 所有IO引脚进入高阻态（除特殊功能引脚外）
  * - 保留功能引脚：
  *   > NRST复位引脚（保持有效）
  *   > PC13(RTC_AF1)：当配置为防篡改/时间戳/闹钟输出/校准输出时
  *   > PI8(RTC_AF2)：当配置为防篡改/时间戳时
  *   > PA0(WKUP1)：当唤醒引脚使能时
  * @note 重要操作前提：
  * - 必须预先清除唤醒标志(WUF)，否则会立即唤醒
  * - 退出待机模式将导致系统复位（程序从main重新执行）
  * @param  无
  * @retval 无
  */
void PWR_EnterSTANDBYMode(void)
{
  /* 配置电源控制寄存器进入待机模式 */
  // 设置PWR_CR_PDDS位（Power Down DeepSleep）
  // 该位为1时，执行WFI/WFE指令后进入待机模式
  PWR->CR |= PWR_CR_PDDS;

  /* 配置Cortex-M4系统控制寄存器 */
  // 设置SLEEPDEEP位使能深度睡眠模式
  // 该位为1时，系统将进入深度睡眠而非普通睡眠
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

  /* 确保存储器写操作完成（仅ARM编译器需要） */
  // __CC_ARM宏检测是否使用ARM Compiler
  // __force_stores()保证所有存储操作完成，防止数据丢失
#if defined ( __CC_ARM )
  __force_stores();
#endif

  /* 触发等待中断指令进入低功耗状态 */
  // 执行WFI(Wait For Interrupt)指令后：
  // - 时钟停止
  // - 内核停止运行
  // - 根据PDDS位设置进入待机模式
  __WFI();
}

/**
  * @}
  */

/** @defgroup PWR_Group7 Flags management functions
 *  @brief   Flags management functions 
 *
@verbatim   
 ===============================================================================
                    ##### Flags management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Checks whether the specified PWR flag is set or not.
  * @param  PWR_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg PWR_FLAG_WU: Wake Up flag. This flag indicates that a wakeup event 
  *                  was received from the WKUP pin or from the RTC alarm (Alarm A 
  *                  or Alarm B), RTC Tamper event, RTC TimeStamp event or RTC Wakeup.
  *                  An additional wakeup event is detected if the WKUP pin is enabled 
  *                  (by setting the EWUP bit) when the WKUP pin level is already high.  
  *            @arg PWR_FLAG_SB: StandBy flag. This flag indicates that the system was
  *                  resumed from StandBy mode.    
  *            @arg PWR_FLAG_PVDO: PVD Output. This flag is valid only if PVD is enabled 
  *                  by the PWR_PVDCmd() function. The PVD is stopped by Standby mode 
  *                  For this reason, this bit is equal to 0 after Standby or reset
  *                  until the PVDE bit is set.
  *            @arg PWR_FLAG_BRR: Backup regulator ready flag. This bit is not reset 
  *                  when the device wakes up from Standby mode or by a system reset 
  *                  or power reset.  
  *            @arg PWR_FLAG_VOSRDY: This flag indicates that the Regulator voltage 
  *                 scaling output selection is ready.
  *            @arg PWR_FLAG_ODRDY: This flag indicates that the Over-drive mode
  *                 is ready (STM32F42xxx/43xxx devices) 
  *            @arg PWR_FLAG_ODSWRDY: This flag indicates that the Over-drive mode
  *                 switcching is ready (STM32F42xxx/43xxx devices) 
  *            @arg PWR_FLAG_UDRDY: This flag indicates that the Under-drive mode
  *                 is enabled in Stop mode (STM32F42xxx/43xxx devices)
  * @retval The new state of PWR_FLAG (SET or RESET).
  */
/**
  * @brief  获取电源控制模块(PWR)的状态标志位
  * @param  PWR_FLAG: 需要查询的状态标志，支持以下值：
  *            @arg PWR_FLAG_WU:    唤醒事件标志（从低功耗模式唤醒）
  *            @arg PWR_FLAG_SB:    待机模式标志
  *            @arg PWR_FLAG_PVDO:  可编程电压检测输出标志
  *            @arg PWR_FLAG_BRR:   备份稳压器就绪标志
  *            @arg PWR_FLAG_VOSRDY: 电压调节器就绪标志
  *            @arg PWR_FLAG_ODRDY: 过驱动模式就绪标志（仅F42x/43x）
  *            @arg PWR_FLAG_ODSWRDY: 过驱动切换就绪标志（仅F42x/43x）
  *            @arg PWR_FLAG_UDRDY: 欠驱动模式就绪标志（仅F42x/43x）
  * @retval FlagStatus 标志位状态：
  *         SET - 标志位已置位（条件成立）
  *         RESET - 标志位未置位（条件不成立）
  * @note 典型应用场景：
  *        - 检测是否发生唤醒事件
  *        - 验证低功耗模式切换状态
  *        - 监控电压调节器工作状态
  */
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG)
{
  FlagStatus bitstatus = RESET;  // 初始化返回值为未置位状态
  
  /* 验证输入参数的有效性 */
  assert_param(IS_PWR_GET_FLAG(PWR_FLAG));  // 检查标志位参数是否在允许范围内
  
  /* 通过位与操作检测指定标志位状态：
     - 读取PWR控制/状态寄存器(CSR)
     - 与输入标志位掩码进行按位与运算
     - 结果非零表示标志位已置位 */
  if ((PWR->CSR & PWR_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;  // 标志位有效，返回置位状态
  }
  else
  {
    bitstatus = RESET; // 标志位无效，保持复位状态
  }
  
  /* 返回最终检测结果 */
  return bitstatus;  // 返回标志位的实时状态
}

/**
  * @brief  清除电源控制模块(PWR)的状态标志位
  * @param  PWR_FLAG: 需要清除的标志位，支持以下值：
  *            @arg PWR_FLAG_WU:   唤醒事件标志（从低功耗模式唤醒）
  *            @arg PWR_FLAG_SB:   待机模式进入标志
  *            @arg PWR_FLAG_UDRDY: 欠压驱动就绪标志（仅STM32F42xxx/43xxx系列）
  * @note 实现原理：
  * - 通过写控制寄存器(CR)或控制状态寄存器(CSR)的特定位置位来清除标志
  * - 不同芯片型号采用不同的寄存器操作策略
  * - 标志位清除机制：通过置位控制寄存器对应标志偏移位实现
  * @warning 操作前必须确保PWR_FLAG参数合法性
  * @retval 无
  */
void PWR_ClearFlag(uint32_t PWR_FLAG)
{
  /* 参数有效性校验 */
  // 使用断言验证标志位参数是否在允许范围内
  // IS_PWR_CLEAR_FLAG宏定义验证PWR_FLAG的有效组合
  assert_param(IS_PWR_CLEAR_FLAG(PWR_FLAG));
  
/* 针对F42x/43x系列的处理 (包含UDRDY标志的特殊处理) */
// 该系列芯片新增了低功耗模式相关标志
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
  // 非UDRDY标志处理流程
  if (PWR_FLAG != PWR_FLAG_UDRDY)
  {
    /* 标准标志清除方式：
     * 将标志位左移2位后写入CR寄存器
     * 原因为CR寄存器标志位偏移地址=标志位值+2
     * 例如：PWR_FLAG_WU(0x01)对应CR寄存器bit3位置位 */
    PWR->CR |=  PWR_FLAG << 2;
  }
  // UDRDY特殊标志处理
  else
  {
    /* 低功耗模式就绪标志特殊处理：
     * 直接设置CSR寄存器的UDRDY位（第3位）
     * 该标志位在控制/状态寄存器中直接对应 */
    PWR->CSR |= PWR_FLAG_UDRDY;
  }
#endif /* STM32F427_437xx ||  STM32F429_439xx */

/* 针对F40x/41x/F401/F411系列的处理 */
// 这些系列没有UDRDY标志，统一处理方式
#if defined (STM32F40_41xxx) || defined (STM32F401xx) || defined (STM32F411xE) 
  /* 通用标志清除方式：
   * 所有标志位统一左移2位写入CR寄存器
   * 例如：清除唤醒标志时，PWR_FLAG_WU(0x01)左移2位后对应CR寄存器bit3 */
  PWR->CR |=  PWR_FLAG << 2;
#endif /* STM32F40_41xxx  || STM32F401xx || STM32F411xE */
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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

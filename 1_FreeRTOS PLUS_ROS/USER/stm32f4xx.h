/**
  ******************************************************************************
  * @file    stm32f4xx.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer Header File. 
  *          This file contains all the peripheral register's definitions, bits 
  *          definitions and memory mapping for STM32F4xx devices.            
  *            
  *          The file is the unique include file that the application programmer
  *          is using in the C source code, usually in main.c. This file contains:
  *           - Configuration section that allows to select:
  *              - The device used in the target application
  *              - To use or not the peripheral抯 drivers in application code(i.e. 
  *                code will be based on direct access to peripheral抯 registers 
  *                rather than drivers API), this option is controlled by 
  *                "#define USE_STDPERIPH_DRIVER"
  *              - To change few application-specific parameters such as the HSE 
  *                crystal frequency
  *           - Data structures and the address mapping for all peripherals
  *           - Peripheral's registers declarations and bits definition
  *           - Macros to access peripheral抯 registers hardware
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

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32f4xx
  * @{
  */
    
#ifndef __STM32F4xx_H
#define __STM32F4xx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
  
/** @addtogroup Library_configuration_section
  * @{
  */
  
/* Uncomment the line below according to the target STM32 device used in your
   application 
  */
/* STM32F4系列芯片型号选择配置 -------------------------------------------------*/
/* 当未定义任何设备宏时触发错误提示 */
#if !defined (STM32F40_41xxx) && !defined (STM32F427_437xx) && !defined (STM32F429_439xx) && !defined (STM32F401xx) && !defined (STM32F411xE)
  /* 预编译阶段可取消注释以下任意宏定义来选择设备系列 */
  /* #define STM32F40_41xxx */   /*!< 定义F40/F41/F42/F47系列设备：
                                     包含型号：F405RG, F405VG, F405ZG, F415RG, F415VG, F415ZG,  
                                     F407VG, F407VE, F407ZG, F407ZE, F407IG, F407IE, 
                                     F417VG, F417VE, F417ZG, F417ZE, F417IG, F417IE */

  /* #define STM32F427_437xx */  /*!< 定义F427/F437系列设备：
                                     包含型号：F427VG, F427VI, F427ZG, F427ZI, F427IG, F427II,  
                                     F437VG, F437VI, F437ZG, F437ZI, F437IG, F437II */

  /* #define STM32F429_439xx */  /*!< 定义F429/F439系列设备：
                                     包含型号：F429VG, F429VI, F429ZG, F429ZI, F429BG, F429BI,  
                                     F429NG, F439NI, F429IG, F429II, F439VG, F439VI, 
                                     F439ZG, F439ZI, F439BG, F439BI, F439NG, F439NI,
                                     F439IG, F439II */

  /* #define STM32F401xx */      /*!< 定义F401系列设备：
                                     包含型号：F401CB, F401CC, F401RB, F401RC, F401VB, F401VC  
                                     F401CD, F401RD, F401VD, F401CExx, F401RE, F401VE */

  /* #define STM32F411xE */      /*!< 定义F411系列设备：
                                     包含型号：F411CD, F411RD, F411VD, F411CE, F411RE, F411VE */
#endif

/* 旧型号宏定义兼容处理 ------------------------------------------------------*/
#ifdef STM32F40XX  /* 兼容旧版本F40XX宏定义 */
  #define STM32F40_41xxx
#endif 

#ifdef STM32F427X  /* 兼容旧版本F427X宏定义 */
  #define STM32F427_437xx
#endif 

/* 开发环境配置提示 -----------------------------------------------------------
 * 建议在编译工具链的预处理器设置中定义设备宏（而非直接修改本文件）
 * 例如：在MDK-ARM的Options for Target -> C/C++ -> Define中添加STM32F40_41xxx
 */

/* 设备宏未定义错误检查 -------------------------------------------------------*/
#if !defined (STM32F40_41xxx) && !defined (STM32F427_437xx) && !defined (STM32F429_439xx) && !defined (STM32F401xx) && !defined (STM32F411xE)
 #error "请先在应用程序中选择目标STM32F4xx设备（在stm32f4xx.h文件中定义）"
#endif

/* 标准外设库启用配置 ---------------------------------------------------------*/
#if !defined  (USE_STDPERIPH_DRIVER)
/**
 * @brief 如果不需要使用ST官方提供的外设驱动库，请注释以下定义
 *        禁用后应用程序需要直接操作寄存器来配置外设
 */
  /*#define USE_STDPERIPH_DRIVER */  /* 取消注释以启用标准外设驱动库 */
#endif 

/* 外部高速晶振（HSE）频率配置 -------------------------------------------------
 * 默认值：25MHz（对应开发板板载晶振）
 * 实际值必须与硬件电路中的HSE晶振频率严格一致
 * 可在编译器预处理器中覆盖此定义（如使用8MHz晶振时定义HSE_VALUE=8000000）
 */
#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    ((uint32_t)25000000) /*!< 外部晶振频率（单位：Hz） */
#endif 
/*------------------------- HSE启动超时配置 -----------------------------------*/
/**
 * @brief 定义HSE时钟启动超时阈值
 * @说明：用于HSE就绪等待循环的最大次数
 * @计算方式：超时时间 ≈ (HSE_STARTUP_TIMEOUT * 循环周期)
 *           例如：在168MHz下，0x5000≈(20480 * 3cycle)/168MHz≈0.37ms
 * @调整建议：低速晶振需增大该值（如32.768KHz晶振需设为0xFFFF）
 */
#if !defined  (HSE_STARTUP_TIMEOUT) 
  #define HSE_STARTUP_TIMEOUT    ((uint16_t)0x05000)   // 默认值：20480次循环
#endif 

/*------------------------- 内部高速时钟配置 ---------------------------------*/
#if !defined  (HSI_VALUE)   
  #define HSI_VALUE    ((uint32_t)16000000) /*!< 内部RC振荡器频率（出厂校准值）
                                              * 实际值会在16MHz±1%范围内波动
                                              * 可通过RCC_CR寄存器的HSITRIM位微调 */
#endif 

/*--------------------- 标准外设驱动库版本信息配置 ---------------------------*/
/**
 * @brief STM32F4标准外设库版本定义（V1.4.0）
 * @格式说明：主版本.子版本1.子版本2-发布候选版本
 * @示例：0x01 04 00 00 → V1.4.0
 */
#define __STM32F4XX_STDPERIPH_VERSION_MAIN   (0x01) // 主版本号（31-24位）
#define __STM32F4XX_STDPERIPH_VERSION_SUB1   (0x04) // 子版本1（23-16位）
#define __STM32F4XX_STDPERIPH_VERSION_SUB2   (0x00) // 子版本2（15-8位）
#define __STM32F4XX_STDPERIPH_VERSION_RC     (0x00) // 发布候选版本（7-0位）
#define __STM32F4XX_STDPERIPH_VERSION        ((__MAIN <<24)|(__SUB1 <<16)|(__SUB2 <<8)|__RC)

/*--------------------- Cortex-M4内核特性配置 --------------------------------*/
/**
 * @brief CMSIS-Core(M4)配置参数
 * @注意：这些参数必须与芯片实际硬件特性严格匹配
 */
#define __CM4_REV                 0x0001  /*!< Cortex-M4核心版本r0p1 
                                              影响调试器识别和某些内核特性 */

#define __MPU_PRESENT             1       /*!< 芯片包含内存保护单元(MPU)
                                              启用需在代码中配置MPU寄存器 */

#define __NVIC_PRIO_BITS          4       /*!< 中断优先级可配置位数（共16级）
                                              0: 不可配置, 4: 2^4=16级优先级 */

#define __Vendor_SysTickConfig    0       /*!< 是否使用自定义SysTick配置
                                              0=使用CMSIS默认实现
                                              1=需提供自定义SysTick_Handler */

#define __FPU_PRESENT             1       /*!< 芯片包含硬件浮点单元(FPU)
                                              需在代码中启用：SCB->CPACR |= 0xF <<20 */

#define __FPU_USED                1       /*!< 编译器是否使用硬件FPU
                                              影响浮点运算指令生成方式 */
/**
 * @brief STM32F4系列芯片中断号枚举定义
 * 
 * 本枚举定义了STM32F4系列微控制器的所有中断源，包括：
 * - Cortex-M4处理器核心异常类型中断
 * - STM32外设相关中断
 * 用于中断优先级配置、中断服务函数声明等场景。开发者应根据具体使用的外设
 * 启用对应的中断，并编写相应的中断服务例程(ISR)。
 * 注意：实际可用中断号取决于具体芯片型号，请参考芯片数据手册。
 */
typedef enum IRQn
{
/********************** Cortex-M4 处理器异常中断号 *******************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 不可屏蔽中断(NMI)，通常由硬件严重错误触发 
                                             无法通过优先级设置屏蔽，用于关键系统故障处理 */
  MemoryManagement_IRQn       = -12,    /*!< 内存管理单元(MMU)故障中断 
                                             由非法内存访问（如访问未对齐地址）触发 */
  BusFault_IRQn               = -11,    /*!< 总线错误中断 
                                             在指令预取或数据访问时检测到总线错误触发 */
  UsageFault_IRQn             = -10,    /*!< 用法错误中断 
                                             由未定义指令或非法状态转换（如无效EPSR）触发 */
  SVCall_IRQn                 = -5,     /*!< 系统服务调用(SVC)中断 
                                             由SVC指令触发，用于实现操作系统系统调用 */
  DebugMonitor_IRQn           = -4,     /*!< 调试监控中断 
                                             在调试状态下触发，用于硬件调试支持 */
  PendSV_IRQn                 = -2,     /*!< 可挂起的系统服务中断 
                                             用于上下文切换，通常由操作系统调度器使用 */
  SysTick_IRQn                = -1,     /*!< 系统节拍定时器中断 
                                             由SysTick定时器周期性触发，用于系统时基 */

/********************** STM32 外设中断号 ***************************************************/
  WWDG_IRQn                   = 0,      /*!< 窗口看门狗中断 
                                             当计数器值超过窗口寄存器或超时时触发 */
  PVD_IRQn                    = 1,      /*!< 可编程电压检测器中断 
                                             通过EXTI线检测电源电压跌落事件 */
  TAMP_STAMP_IRQn             = 2,      /*!< 入侵检测和时间戳中断 
                                             检测到入侵事件或时间戳事件时触发 */
  RTC_WKUP_IRQn               = 3,      /*!< RTC唤醒中断 
                                             RTC警报事件或周期性唤醒信号触发 */
  FLASH_IRQn                  = 4,      /*!< FLASH存储器操作中断 
                                             闪存操作完成或发生错误时触发 */
  RCC_IRQn                    = 5,      /*!< 复位和时钟控制器中断 
                                             时钟安全系统(CSS)或HSE准备就绪时触发 */
  EXTI0_IRQn                  = 6,      /*!< 外部中断线0中断 
                                             对应GPIO引脚PA0-PI0的边沿触发事件 */
  EXTI1_IRQn                  = 7,      /*!< 外部中断线1中断 
                                             对应GPIO引脚PA1-PI1的边沿触发事件 */
  EXTI2_IRQn                  = 8,      /*!< 外部中断线2中断 
                                             对应GPIO引脚PA2-PI2的边沿触发事件 */
  EXTI3_IRQn                  = 9,      /*!< 外部中断线3中断 
                                             对应GPIO引脚PA3-PI3的边沿触发事件 */
  EXTI4_IRQn                  = 10,     /*!< 外部中断线4中断 
                                             对应GPIO引脚PA4-PI4的边沿触发事件 */
  DMA1_Stream0_IRQn           = 11,     /*!< DMA1流0传输完成/错误中断 
                                             用于存储器到外设的DMA传输通知 */
  DMA1_Stream1_IRQn           = 12,     /*!< DMA1流1传输完成/错误中断 
                                             用于外设到存储器的DMA传输通知 */
  DMA1_Stream2_IRQn           = 13,     /*!< DMA1流2传输完成/错误中断 
                                             支持循环模式的双缓冲传输中断 */
  DMA1_Stream3_IRQn           = 14,     /*!< DMA1流3传输完成/错误中断 
                                             常用于SPI/I2S数据传输完成通知 */
  DMA1_Stream4_IRQn           = 15,     /*!< DMA1流4传输完成/错误中断 
                                             用于定时器更新事件触发传输 */
  DMA1_Stream5_IRQn           = 16,     /*!< DMA1流5传输完成/错误中断 
                                             支持外设流控制的高级传输模式 */
  DMA1_Stream6_IRQn           = 17,     /*!< DMA1流6传输完成/错误中断 
                                             用于存储器到存储器传输场景 */
  ADC_IRQn                    = 18,     /*!< ADC全局中断 
                                             包含ADC1/2/3的转换完成或溢出事件 */


#if defined (STM32F40_41xxx)
  CAN1_TX_IRQn                = 19,     /*!< CAN1发送中断 
                                             当CAN1邮箱数据成功发送或发送失败时触发 */
  CAN1_RX0_IRQn               = 20,     /*!< CAN1接收FIFO0中断 
                                             当FIFO0接收到新报文且优先级较高时触发 */
  CAN1_RX1_IRQn               = 21,     /*!< CAN1接收FIFO1中断 
                                             当FIFO1接收到新报文时触发 */
  CAN1_SCE_IRQn               = 22,     /*!< CAN1状态变更中断 
                                             检测到错误状态或状态变化时触发 */

  EXTI9_5_IRQn                = 23,     /*!< 外部中断线9-5复合中断 
                                             对应GPIO引脚PA5-PI9的边沿触发事件 */

  TIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1急停中断 + TIM9全局中断
                                             TIM1急停输入触发时产生
                                             TIM9计数器溢出/触发事件时产生 */

  TIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1更新中断 + TIM10全局中断
                                             TIM1计数器溢出/重装载时触发
                                             TIM10定时周期到达时触发 */

  TIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1触发/换相中断 + TIM11中断
                                             TIM1触发输入事件或换相点时触发
                                             TIM11定时周期到达时触发 */

  TIM1_CC_IRQn                = 27,     /*!< TIM1捕获比较中断 
                                             输入捕获事件或输出比较匹配时触发 */

  TIM2_IRQn                   = 28,     /*!< 通用定时器2全局中断 
                                             包含计数器溢出/捕获比较等各种事件 */

  TIM3_IRQn                   = 29,     /*!< 通用定时器3全局中断 
                                             常用于PWM生成或输入捕获应用 */

  TIM4_IRQn                   = 30,     /*!< 通用定时器4全局中断 
                                             支持编码器接口模式和霍尔传感器检测 */

  I2C1_EV_IRQn                = 31,     /*!< I2C1事件中断 
                                             包含启动检测、地址匹配、数据传输完成等事件 */

  I2C1_ER_IRQn                = 32,     /*!< I2C1错误中断 
                                             检测到总线错误、仲裁丢失、ACK故障等错误 */

  I2C2_EV_IRQn                = 33,     /*!< I2C2事件中断 
                                             用于第二个I2C总线的事件通知 */

  I2C2_ER_IRQn                = 34,     /*!< I2C2错误中断 
                                             第二个I2C总线的错误状态通知 */

  SPI1_IRQn                   = 35,     /*!< SPI1全局中断 
                                             包含传输完成、接收缓冲非空等事件 */

  SPI2_IRQn                   = 36,     /*!< SPI2全局中断 
                                             用于第二个SPI接口的中断事件 */

  USART1_IRQn                 = 37,     /*!< USART1全局中断 
                                             包含发送完成、数据接收、线路空闲等状态 */

  USART2_IRQn                 = 38,     /*!< USART2全局中断 
                                             常用于调试输出或外部设备通信 */

  USART3_IRQn                 = 39,     /*!< USART3全局中断 
                                             支持智能卡模式和LIN总线通信 */

  EXTI15_10_IRQn              = 40,     /*!< 外部中断线15-10复合中断 
                                             对应GPIO引脚PA10-PI15的边沿触发事件 */

  RTC_Alarm_IRQn              = 41,     /*!< RTC闹钟中断 
                                             通过EXTI线17触发，支持闹钟A/B事件 */

  OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG全速模式唤醒中断 
                                             当USB挂起模式下检测到唤醒事件时触发 */

  TIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8急停中断 + TIM12中断
                                             TIM8急停输入触发时产生
                                             TIM12定时事件触发 */

  TIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8更新中断 + TIM13中断
                                             TIM8计数器更新时触发
                                             TIM13定时周期到达时触发 */

  TIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8触发/换相中断 + TIM14中断
                                             TIM8触发输入或换相操作时产生
                                             TIM14定时事件触发 */

  TIM8_CC_IRQn                = 46,     /*!< TIM8捕获比较中断 
                                             用于高级电机控制应用的PWM事件检测 */

  DMA1_Stream7_IRQn           = 47,     /*!< DMA1流7中断 
                                             最后一个DMA1流，用于特定外设数据传输 */

  FSMC_IRQn                   = 48,     /*!< 灵活静态存储器控制器中断 
                                             发生NOR/PSRAM/NAND控制器错误时触发 */

  SDIO_IRQn                   = 49,     /*!< SD/MMC存储卡接口中断 
                                             包含数据传输完成、命令响应接收等事件 */

  TIM5_IRQn                   = 50,     /*!< 高级定时器5全局中断 
                                             支持32位计数器，用于精密定时应用 */

  SPI3_IRQn                   = 51,     /*!< SPI3全局中断 
                                             第三个SPI接口的中断事件通知 */

  UART4_IRQn                  = 52,     /*!< UART4全局中断 
                                             支持IrDA红外通信模式 */

  UART5_IRQn                  = 53,     /*!< UART5全局中断 
                                             支持自动波特率检测功能 */

  TIM6_DAC_IRQn               = 54,     /*!< 基础定时器6中断 + DAC下溢中断
                                             TIM6周期性触发中断
                                             DAC缓冲区欠载时触发 */

  TIM7_IRQn                   = 55,     /*!< 基础定时器7中断 
                                             用于简单定时任务和触发DMA请求 */

  DMA2_Stream0_IRQn           = 56,     /*!< DMA2流0中断 
                                             用于高速外设如Camera接口的数据传输 */

  DMA2_Stream1_IRQn           = 57,     /*!< DMA2流1中断 
                                             支持外设到存储器的批量数据传输 */

  DMA2_Stream2_IRQn           = 58,     /*!< DMA2流2中断 
                                             常用于以太网MAC的数据传输管理 */

  DMA2_Stream3_IRQn           = 59,     /*!< DMA2流3中断 
                                             用于USB OTG HS端点数据传输 */

  DMA2_Stream4_IRQn           = 60,     /*!< DMA2流4中断 
                                             支持存储器到存储器的快速拷贝 */

  ETH_IRQn                    = 61,     /*!< 以太网MAC全局中断 
                                             包含传输完成、接收帧可用、错误检测等 */

  ETH_WKUP_IRQn               = 62,     /*!< 以太网唤醒中断 
                                             检测到Magic Packet或远程唤醒帧时触发 */

  CAN2_TX_IRQn                = 63,     /*!< CAN2发送中断 
                                             第二个CAN控制器的发送事件通知 */

  CAN2_RX0_IRQn               = 64,     /*!< CAN2接收FIFO0中断 
                                             高优先级报文接收通知 */

  CAN2_RX1_IRQn               = 65,     /*!< CAN2接收FIFO1中断 
                                             标准优先级报文接收通知 */

  CAN2_SCE_IRQn               = 66,     /*!< CAN2状态变更中断 
                                             总线错误计数超过阈值时触发 */

  OTG_FS_IRQn                 = 67,     /*!< USB OTG全速模式全局中断 
                                             处理所有USB设备模式相关事件 */

  DMA2_Stream5_IRQn           = 68,     /*!< DMA2流5中断 
                                             用于加密模块的数据输入/输出传输 */

  DMA2_Stream6_IRQn           = 69,     /*!< DMA2流6中断 
                                             支持浮点单元(FPU)的数据预取操作 */

  DMA2_Stream7_IRQn           = 70,     /*!< DMA2流7中断 
                                             DMA2最后一个流，用于特定高带宽外设 */

  USART6_IRQn                 = 71,     /*!< USART6全局中断 
                                             支持高速通信（最高10.5 Mbit/s） */

  I2C3_EV_IRQn                = 72,     /*!< I2C3事件中断 
                                             第三个I2C总线的事件通知 */

  I2C3_ER_IRQn                = 73,     /*!< I2C3错误中断 
                                             第三个I2C总线的错误状态报告 */

  OTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG高速模式端点1输出中断 
                                             批量传输端点输出操作完成通知 */

  OTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG高速模式端点1输入中断 
                                             批量传输端点输入操作完成通知 */

  OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG高速模式唤醒中断 
                                             从挂起状态唤醒时触发 */

  OTG_HS_IRQn                 = 77,     /*!< USB OTG高速模式全局中断 
                                             处理所有高速USB主机/设备模式事件 */

  DCMI_IRQn                   = 78,     /*!< 数字相机接口中断 
                                             帧捕获完成或同步错误时触发 */

  CRYP_IRQn                   = 79,     /*!< 加密处理器中断 
                                             加解密操作完成或输入数据不足时触发 */

  HASH_RNG_IRQn               = 80,     /*!< 哈希处理器 + 随机数发生器中断 
                                             哈希计算完成或随机数就绪时触发 */

  FPU_IRQn                    = 81      /*!< 浮点单元异常中断 
                                             发生浮点运算错误（如除以零）时触发 */
#endif /* STM32F40_41xxx */


#if defined (STM32F427_437xx)
  CAN1_TX_IRQn                = 19,     /*!< CAN1 TX Interrupt                                                 */
  CAN1_RX0_IRQn               = 20,     /*!< CAN1 RX0 Interrupt                                                */
  CAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                                */
  CAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                                */
  EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
  TIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
  TIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
  TIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
  TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
  TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
  TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
  TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
  I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
  I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
  I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
  I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */  
  SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
  SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
  USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
  USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
  USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                                           */
  EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
  RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
  OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */    
  TIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
  TIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
  TIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
  TIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
  DMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
  FMC_IRQn                    = 48,     /*!< FMC global Interrupt                                              */
  SDIO_IRQn                   = 49,     /*!< SDIO global Interrupt                                             */
  TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
  SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
  UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                                            */
  UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                                            */
  TIM6_DAC_IRQn               = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
  TIM7_IRQn                   = 55,     /*!< TIM7 global interrupt                                             */
  DMA2_Stream0_IRQn           = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
  DMA2_Stream1_IRQn           = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
  DMA2_Stream2_IRQn           = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
  DMA2_Stream3_IRQn           = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
  DMA2_Stream4_IRQn           = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
  ETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                                         */
  ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
  CAN2_TX_IRQn                = 63,     /*!< CAN2 TX Interrupt                                                 */
  CAN2_RX0_IRQn               = 64,     /*!< CAN2 RX0 Interrupt                                                */
  CAN2_RX1_IRQn               = 65,     /*!< CAN2 RX1 Interrupt                                                */
  CAN2_SCE_IRQn               = 66,     /*!< CAN2 SCE Interrupt                                                */
  OTG_FS_IRQn                 = 67,     /*!< USB OTG FS global Interrupt                                       */
  DMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
  DMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
  DMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
  USART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
  I2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
  I2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
  OTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
  OTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
  OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
  OTG_HS_IRQn                 = 77,     /*!< USB OTG HS global interrupt                                       */
  DCMI_IRQn                   = 78,     /*!< DCMI global interrupt                                             */
  CRYP_IRQn                   = 79,     /*!< CRYP crypto global interrupt                                      */
  HASH_RNG_IRQn               = 80,     /*!< Hash and Rng global interrupt                                     */
  FPU_IRQn                    = 81,     /*!< FPU global interrupt                                              */
  UART7_IRQn                  = 82,     /*!< UART7 global interrupt                                            */
  UART8_IRQn                  = 83,     /*!< UART8 global interrupt                                            */
  SPI4_IRQn                   = 84,     /*!< SPI4 global Interrupt                                             */
  SPI5_IRQn                   = 85,     /*!< SPI5 global Interrupt                                             */
  SPI6_IRQn                   = 86,     /*!< SPI6 global Interrupt                                             */
  SAI1_IRQn                   = 87,     /*!< SAI1 global Interrupt                                             */
  DMA2D_IRQn                  = 90      /*!< DMA2D global Interrupt                                            */   
#endif /* STM32F427_437xx */
    
#if defined (STM32F429_439xx)
  CAN1_TX_IRQn                = 19,     /*!< CAN1 TX Interrupt                                                 */
  CAN1_RX0_IRQn               = 20,     /*!< CAN1 RX0 Interrupt                                                */
  CAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                                */
  CAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                                */
  EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
  TIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
  TIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
  TIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
  TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
  TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
  TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
  TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
  I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
  I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
  I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
  I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */  
  SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
  SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
  USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
  USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
  USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                                           */
  EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
  RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
  OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */    
  TIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
  TIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
  TIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
  TIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
  DMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
  FMC_IRQn                    = 48,     /*!< FMC global Interrupt                                              */
  SDIO_IRQn                   = 49,     /*!< SDIO global Interrupt                                             */
  TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
  SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
  UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                                            */
  UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                                            */
  TIM6_DAC_IRQn               = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
  TIM7_IRQn                   = 55,     /*!< TIM7 global interrupt                                             */
  DMA2_Stream0_IRQn           = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
  DMA2_Stream1_IRQn           = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
  DMA2_Stream2_IRQn           = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
  DMA2_Stream3_IRQn           = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
  DMA2_Stream4_IRQn           = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
  ETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                                         */
  ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
  CAN2_TX_IRQn                = 63,     /*!< CAN2 TX Interrupt                                                 */
  CAN2_RX0_IRQn               = 64,     /*!< CAN2 RX0 Interrupt                                                */
  CAN2_RX1_IRQn               = 65,     /*!< CAN2 RX1 Interrupt                                                */
  CAN2_SCE_IRQn               = 66,     /*!< CAN2 SCE Interrupt                                                */
  OTG_FS_IRQn                 = 67,     /*!< USB OTG FS global Interrupt                                       */
  DMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
  DMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
  DMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
  USART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
  I2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
  I2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
  OTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
  OTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
  OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
  OTG_HS_IRQn                 = 77,     /*!< USB OTG HS global interrupt                                       */
  DCMI_IRQn                   = 78,     /*!< DCMI global interrupt                                             */
  CRYP_IRQn                   = 79,     /*!< CRYP crypto global interrupt                                      */
  HASH_RNG_IRQn               = 80,     /*!< Hash and Rng global interrupt                                     */
  FPU_IRQn                    = 81,     /*!< FPU global interrupt                                              */
  UART7_IRQn                  = 82,     /*!< UART7 global interrupt                                            */
  UART8_IRQn                  = 83,     /*!< UART8 global interrupt                                            */
  SPI4_IRQn                   = 84,     /*!< SPI4 global Interrupt                                             */
  SPI5_IRQn                   = 85,     /*!< SPI5 global Interrupt                                             */
  SPI6_IRQn                   = 86,     /*!< SPI6 global Interrupt                                             */
  SAI1_IRQn                   = 87,     /*!< SAI1 global Interrupt                                             */
  LTDC_IRQn                   = 88,     /*!< LTDC global Interrupt                                             */
  LTDC_ER_IRQn                = 89,     /*!< LTDC Error global Interrupt                                       */
  DMA2D_IRQn                  = 90      /*!< DMA2D global Interrupt                                            */
#endif /* STM32F429_439xx */
   
#if defined (STM32F401xx) || defined (STM32F411xE)
  EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
  TIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
  TIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
  TIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
  TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
  TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
  TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
  TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
  I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
  I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
  I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
  I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */
  SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
  SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
  USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
  USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
  EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
  RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
  OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
  DMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
  SDIO_IRQn                   = 49,     /*!< SDIO global Interrupt                                             */
  TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
  SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
  DMA2_Stream0_IRQn           = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
  DMA2_Stream1_IRQn           = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
  DMA2_Stream2_IRQn           = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
  DMA2_Stream3_IRQn           = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
  DMA2_Stream4_IRQn           = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
  OTG_FS_IRQn                 = 67,     /*!< USB OTG FS global Interrupt                                       */
  DMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
  DMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
  DMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
  USART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
  I2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
  I2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
  FPU_IRQn                    = 81,      /*!< FPU global interrupt                                             */
#if defined (STM32F401xx)
  SPI4_IRQn                   = 84       /*!< SPI4 global Interrupt                                            */
#endif /* STM32F411xE */
#if defined (STM32F411xE)
  SPI4_IRQn                   = 84,     /*!< SPI4 global Interrupt                                             */
  SPI5_IRQn                   = 85      /*!< SPI5 global Interrupt                                             */
#endif /* STM32F411xE */
#endif /* STM32F401xx || STM32F411xE */

} IRQn_Type;

/**
  * @}
  */

#include "core_cm4.h"             /* Cortex-M4 processor and core peripherals */
#include "system_stm32f4xx.h"
#include <stdint.h>
#include "arm_math.h"
/**
 * @brief STM32F4系列标准外设库类型定义
 * 
 * 本部分定义STM32F4系列芯片开发中使用的标准数据类型，包含：
 * - 定长整型数据类型的重定义(8/16/32位带符号/无符号)
 * - 外设寄存器访问专用类型(volatile限定)
 * - 状态标志枚举类型(FlagStatus/ITStatus)
 * - 功能控制枚举类型(FunctionalState)
 * - 操作结果状态枚举类型(ErrorStatus)
 * 这些类型定义保证跨平台兼容性，统一数据长度，并提高代码可读性
 */

/** @addtogroup Exported_types
  * @{
  */  
/*!< STM32F10x标准外设库旧式类型定义（为兼容旧代码保留）*/
typedef int32_t  s32;  /*!< 带符号32位整型 
                            用于需要大范围整数计算的场景 */
typedef int16_t s16;   /*!< 带符号16位短整型 
                            适合存储传感器原始数据 */
typedef int8_t  s8;    /*!< 带符号8位字节型 
                            用于紧凑数据存储和通信协议 */

/* 常量类型定义 */
typedef const int32_t sc32;  /*!< 只读32位常量整型 
                                  用于存储不可修改的配置参数 */
typedef const int16_t sc16;  /*!< 只读16位常量短整型 
                                  典型应用：查找表条目 */
typedef const int8_t sc8;    /*!< 只读8位常量字节型 
                                  用于固定协议头定义 */

/* 易变类型定义（用于寄存器访问） */
typedef __IO int32_t  vs32;  /*!< 易变32位整型(volatile) 
                                  用于访问可能随时变化的外设寄存器 */
typedef __IO int16_t  vs16;  /*!< 易变16位短整型(volatile)
                                  典型应用：ADC结果寄存器访问 */
typedef __IO int8_t   vs8;   /*!< 易变8位字节型(volatile)
                                  用于状态标志寄存器访问 */

/* 只读易变类型 */
typedef __I int32_t vsc32;  /*!< 只读易变32位整型 
                                 用于读取只写寄存器镜像值 */
typedef __I int16_t vsc16;  /*!< 只读易变16位短整型
                                 常见于DMA传输计数寄存器 */
typedef __I int8_t vsc8;    /*!< 只读易变8位字节型
                                 用于读取硬件状态寄存器 */

/* 无符号基本类型 */
typedef uint32_t  u32;  /*!< 无符号32位整型 
                             用于位操作和地址计算 */
typedef uint16_t u16;   /*!< 无符号16位短整型
                             适合存储PWM占空比值 */
typedef uint8_t  u8;    /*!< 无符号8位字节型
                             用于原始数据缓冲区和通信协议 */

/* 无符号常量类型 */
typedef const uint32_t uc32;  /*!< 只读32位无符号常量 
                                   用于存储只读配置参数表 */
typedef const uint16_t uc16;  /*!< 只读16位无符号常量
                                   典型应用：CRC多项式定义 */
typedef const uint8_t uc8;    /*!< 只读8位无符号常量
                                   用于固定协议特征码定义 */

/* 无符号易变类型 */
typedef __IO uint32_t  vu32;  /*!< 易变32位无符号整型
                                   用于GPIO端口寄存器访问 */
typedef __IO uint16_t vu16;   /*!< 易变16位无符号短整型
                                   常见于定时器计数寄存器 */
typedef __IO uint8_t  vu8;    /*!< 易变8位无符号字节型
                                   用于UART接收数据寄存器 */

/* 只读无符号易变类型 */
typedef __I uint32_t vuc32;  /*!< 只读易变32位无符号 
                                 用于读取DMA配置寄存器 */
typedef __I uint16_t vuc16;  /*!< 只读易变16位无符号
                                 常见于硬件版本寄存器 */
typedef __I uint8_t vuc8;    /*!< 只读易变8位无符号
                                 用于读取设备ID寄存器 */

/* 状态标志枚举 */
typedef enum {
  RESET = 0,     /*!< 复位状态/标志未置位 
                      对应寄存器位0值 */
  SET = !RESET   /*!< 置位状态/标志已激活 
                      对应寄存器位1值 */
} FlagStatus,      /*!< 用于检查外设状态标志 */
  ITStatus;        /*!< 中断状态标识类型 */

/* 功能控制枚举 */
typedef enum {
  DISABLE = 0,    /*!< 关闭外设功能 
                       停止模块时钟 */
  ENABLE = !DISABLE /*!< 使能外设功能 
                       启动模块运行 */
} FunctionalState;  /*!< 外设使能/禁用控制类型 */

/* 功能状态验证宏 */
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
/*!< 验证FunctionalState值有效性 
       用于参数合法性检查，防止非法状态输入 */

/* 操作结果状态枚举 */
typedef enum {
  ERROR = 0,     /*!< 操作执行失败 
                      需检查错误标志寄存器 */
  SUCCESS = !ERROR /*!< 操作成功完成 
                      可继续后续流程 */
} ErrorStatus;     /*!< 外设驱动函数返回类型 */

/**
  * @}
  */


/**
  * @}
  */

/** @addtogroup Peripheral_registers_structures
  * @{
  */   

/** 
  * @brief Analog to Digital Converter  
  */
/**
 * @brief ADC寄存器映射结构体定义
 * 
 * 本结构体按内存地址顺序定义了STM32F4系列模数转换器(ADC)的所有寄存器，
 * 适用于ADC1、ADC2、ADC3等不同实例。每个寄存器使用volatile修饰保证硬件
 * 级访问，成员排列顺序严格对应参考手册中的寄存器地址偏移。
 * 注意：使用时应通过外设基地址指针进行访问，例如：
 * ADC_TypeDef *ADCx = (ADC_TypeDef*)ADC1_BASE;
 */

typedef struct
{
  /* 基本控制与状态寄存器组 */
  __IO uint32_t SR;     /*!< ADC状态寄存器 
                             位[0] EOC：转换完成标志 
                             位[1] AWD：模拟看门狗触发 
                             位[2] JEOC：注入通道转换完成 
                             位[4] OVR：数据溢出标志 
                             地址偏移: 0x00 */
  
  __IO uint32_t CR1;    /*!< 控制寄存器1 
                             位[8:5] RES[1:0]：分辨率选择(12/10/8/6位) 
                             位[11] SCAN：扫描模式使能 
                             位[13] JEOCIE：注入通道中断使能 
                             地址偏移: 0x04 */      
 
  __IO uint32_t CR2;    /*!< 控制寄存器2 
                             位[0] ADON：ADC使能/禁用 
                             位[1] CONT：连续转换模式 
                             位[8:6] EXTSEL[2:0]：触发源选择 
                             位[10] ALIGN：数据对齐方式(右/左对齐) 
                             地址偏移: 0x08 */

  /* 采样时间配置寄存器 */
  __IO uint32_t SMPR1;  /*!< 采样时间寄存器1 
                             配置通道10-18的采样周期(3位/通道)
                             采样周期= (SMPx[2:0]+1) * ADC时钟周期 
                             地址偏移: 0x0C */
  
  __IO uint32_t SMPR2;  /*!< 采样时间寄存器2 
                             配置通道0-9的采样周期(3位/通道)
                             典型设置：SMPx=7时采样周期480周期 
                             地址偏移: 0x10 */

  /* 注入通道偏移寄存器组 */
  __IO uint32_t JOFR1;  /*!< 注入通道1数据偏移寄存器 
                             存储注入通道1的校准偏移值 
                             实际值 = 原始值 + JOFR1[11:0] 
                             地址偏移: 0x14 */
  
  __IO uint32_t JOFR2;  /*!< 注入通道2数据偏移寄存器 
                             用于消除传感器零点漂移 
                             地址偏移: 0x18 */
  
  __IO uint32_t JOFR3;  /*!< 注入通道3数据偏移寄存器 
                             支持偏移值自动加载功能 
                             地址偏移: 0x1C */
  
  __IO uint32_t JOFR4;  /*!< 注入通道4数据偏移寄存器 
                             偏移范围：-2048 ~ +2047 
                             地址偏移: 0x20 */

  /* 看门狗阈值寄存器 */
  __IO uint32_t HTR;    /*!< 高阈值寄存器 
                             设置模拟看门狗上限值(12位有效)
                             当转换结果超过HTR时触发AWD事件 
                             地址偏移: 0x24 */
  
  __IO uint32_t LTR;    /*!< 低阈值寄存器 
                             设置模拟看门狗下限值(12位有效) 
                             地址偏移: 0x28 */

  /* 规则通道序列寄存器组 */
  __IO uint32_t SQR1;   /*!< 规则序列寄存器1 
                             位[23:20] L[3:0]：总转换次数(0-16) 
                             位[19:15] SQ16[4:0]：第16次转换的通道号 
                             地址偏移: 0x2C */
  
  __IO uint32_t SQR2;   /*!< 规则序列寄存器2 
                             位[29:25] SQ15[4:0]：第15次转换通道 
                             ...依次类推至SQ10 
                             地址偏移: 0x30 */
  
  __IO uint32_t SQR3;   /*!< 规则序列寄存器3 
                             配置序列SQ1-SQ9的通道号 
                             每5位对应一个转换顺序(SQ1位[4:0]) 
                             地址偏移: 0x34 */

  /* 注入通道序列寄存器 */
  __IO uint32_t JSQR;   /*!< 注入序列寄存器 
                             位[21:20] JL[1:0]：注入转换次数(0-4) 
                             位[19:15] JSQ4[4:0]：第4次注入通道 
                             地址偏移: 0x38*/

  /* 注入通道数据寄存器组 */
  __IO uint32_t JDR1;   /*!< 注入数据寄存器1 
                             存储注入通道1的转换结果 
                             数据对齐方式由CR1.ALIGN决定 
                             地址偏移: 0x3C */
  
  __IO uint32_t JDR2;   /*!< 注入数据寄存器2 
                             注入通道2转换结果存储器 
                             访问前需检查JEOC标志 
                             地址偏移: 0x40 */
  
  __IO uint32_t JDR3;   /*!< 注入数据寄存器3 
                             注入通道3转换结果存储器 
                             支持DMA传输模式 
                             地址偏移: 0x44 */
  
  __IO uint32_t JDR4;   /*!< 注入数据寄存器4 
                             注入通道4转换结果存储器 
                             地址偏移: 0x48 */

  /* 规则通道数据寄存器 */
  __IO uint32_t DR;     /*!< 规则数据寄存器 
                             存储最近一次规则通道转换结果 
                             支持溢出保护机制 
                             地址偏移: 0x4C */
} ADC_TypeDef;

/**
 * @brief ADC公共寄存器结构体定义
 * 
 * 本结构体用于管理多个ADC模块的共享配置和状态寄存器，
 * 适用于双ADC/三ADC工作模式下的协同操作。
 * 注意：这些寄存器在ADC1外设基地址基础上进行偏移访问
 */
typedef struct
{
  __IO uint32_t CSR;    /*!< 公共状态寄存器 
                             位[19:16] OVR[3:1]：ADC3-1溢出标志 
                             位[5] DMA：多ADC模式DMA状态 
                             地址偏移: ADC1基地址 + 0x300 */
  
  __IO uint32_t CCR;    /*!< 公共控制寄存器 
                             位[16] VBATE：温度传感器和Vbat使能 
                             位[17] TSVREFE：温度传感器和Vrefint使能 
                             位[23:22] ADCPRE[1:0]：ADC预分频器(2/4/6/8分频) 
                             地址偏移: ADC1基地址 + 0x304 */
  
  __IO uint32_t CDR;    /*!< 公共规则数据寄存器（双/三模式） 
                             在双ADC模式下存储ADC2和ADC1的数据组合 
                             位[31:16] DATA2：ADC2转换结果 
                             位[15:0]  DATA1：ADC1转换结果 
                             地址偏移: ADC1基地址 + 0x308 */
} ADC_Common_TypeDef;

/**
 * @brief CAN发送邮箱结构体
 * 
 * 定义CAN总线发送邮箱的寄存器映射，每个邮箱包含：
 * - 消息标识符配置
 * - 数据帧控制参数
 * - 数据负载存储区
 * 注意：发送优先级由邮箱编号决定（邮箱0优先级最高）
 */
typedef struct
{
  __IO uint32_t TIR;  /*!< 发送标识符寄存器 
                           位[31] IDE：标识符扩展（0-标准/1-扩展） 
                           位[30] RTR：远程传输请求 
                           位[28:0] STID/EXID：标准或扩展ID */
  
  __IO uint32_t TDTR; /*!< 数据长度及时戳寄存器 
                           位[15:0] DLC：数据长度码（0-8字节） 
                           位[31:16] TIME：发送时刻时间戳 */
  
  __IO uint32_t TDLR; /*!< 数据低位寄存器 
                           存储数据字节0-3，小端格式：
                           byte0 @位[7:0], byte1 @位[15:8] */
  
  __IO uint32_t TDHR; /*!< 数据高位寄存器 
                           存储数据字节4-7，小端格式：
                           byte4 @位[7:0], byte5 @位[15:8] */
} CAN_TxMailBox_TypeDef;

/**
 * @brief CAN接收FIFO邮箱结构体
 * 
 * 定义接收FIFO邮箱的寄存器结构，用于存储接收到的CAN消息。
 * 每个FIFO包含3个邮箱，按先进先出顺序访问。
 */
typedef struct
{
  __IO uint32_t RIR;  /*!< 接收标识符寄存器 
                           位[31] IDE：接收帧类型标识 
                           位[30] RTR：远程帧标志 
                           位[29] FILHIT：匹配的过滤器编号 */
  
  __IO uint32_t RDTR; /*!< 数据长度及时戳寄存器 
                           位[15:0] DLC：实际接收数据长度 
                           位[31:16] TIME：帧起始采样时间 */
  
  __IO uint32_t RDLR; /*!< 数据低位寄存器 
                           数据存储顺序与TDLR相同 
                           建议使用memcpy进行数据提取 */
  
  __IO uint32_t RDHR; /*!< 数据高位寄存器 
                           高字节数据，当DLC>4时有效 */
} CAN_FIFOMailBox_TypeDef;

/**
 * @brief CAN过滤器寄存器结构体
 * 
 * 每个过滤器组由两个32位寄存器组成，支持两种配置模式：
 * - 标识符掩码模式（过滤器组作为标识符+掩码）
 * - 标识符列表模式（两个独立标识符）
 */
typedef struct
{
  __IO uint32_t FR1; /*!< 过滤器寄存器1 
                          掩码模式：存储标识符 
                          列表模式：第一个要匹配的ID */
  
  __IO uint32_t FR2; /*!< 过滤器寄存器2 
                          掩码模式：存储掩码码 
                          列表模式：第二个要匹配的ID */
} CAN_FilterRegister_TypeDef;

/**
 * @brief CAN控制器寄存器映射结构体
 * 
 * 本结构体完整定义了STM32F4系列CAN控制器的寄存器布局，包含：
 * - 核心控制与状态寄存器
 * - 发送/接收邮箱配置
 * - 过滤器管理系统
 * 注意：访问过滤器寄存器组前需先设置FMR.FINIT=1进入初始化模式
 */
typedef struct
{
  /* 核心控制与状态寄存器组 */
  __IO uint32_t MCR;     /*!< 主控制寄存器 
                              位[0] INRQ：初始化请求 
                              位[6] ABOM：自动离线管理 
                              位[7] TTCM：时间触发通信模式 
                              地址偏移: 0x00 */
  
  __IO uint32_t MSR;     /*!< 主状态寄存器 
                              位[0] INAK：初始化应答 
                              位[3] SLAKI：睡眠模式确认中断 
                              地址偏移: 0x04 */
  
  __IO uint32_t TSR;     /*!< 发送状态寄存器 
                              位[23:21] CODE：发送邮箱编号 
                              位[26] TME0：邮箱0空标志 
                              地址偏移: 0x08 */

  /* 接收FIFO管理 */
  __IO uint32_t RF0R;    /*!< 接收FIFO0寄存器 
                              位[0] FMP0：FIFO0消息计数 
                              位[5] RFOM0：释放输出模式 
                              地址偏移: 0x0C */
  
  __IO uint32_t RF1R;    /*!< 接收FIFO1寄存器 
                              位[1] FULL1：FIFO1满标志 
                              地址偏移: 0x10 */

  /* 中断管理 */
  __IO uint32_t IER;     /*!< 中断使能寄存器 
                              位[17] ERRIE：错误中断使能 
                              位[1] TMEIE：发送邮箱空中断 
                              地址偏移: 0x14 */
  
  __IO uint32_t ESR;     /*!< 错误状态寄存器 
                              位[15:24] TEC：发送错误计数器 
                              位[7:0] LEC：最后错误代码 
                              地址偏移: 0x18 */
  
  __IO uint32_t BTR;     /*!< 位时序寄存器 
                              位[9:0] BRP：波特率分频 
                              位[15:16] TS1：时间段1 
                              地址偏移: 0x1C */

  uint32_t RESERVED0[88]; /*!< 保留区域（0x020-0x17F）
                              用于对齐内存映射 */

  /* 发送邮箱配置 */
  CAN_TxMailBox_TypeDef sTxMailBox[3]; /*!< 发送邮箱数组 
                                             邮箱0优先级最高 
                                             地址范围: 0x180-0x1AC */

  /* 接收FIFO配置 */
  CAN_FIFOMailBox_TypeDef sFIFOMailBox[2]; /*!< 接收FIFO邮箱 
                                                 FIFO0地址:0x1B0-0x1BC 
                                                 FIFO1地址:0x1C0-0x1CC */

  uint32_t RESERVED1[12];  /*!< 保留区域（0x1D0-0x1FF）*/

  /* 过滤器管理单元 */
  __IO uint32_t FMR;     /*!< 过滤器主寄存器 
                              位[0] FINIT：过滤器初始化模式 
                              地址偏移: 0x200 */
  
  __IO uint32_t FM1R;    /*!< 过滤器模式寄存器 
                              每bit对应一个过滤器组的模式 
                              0=掩码模式，1=列表模式 
                              地址偏移: 0x204 */
  
  __IO uint32_t FS1R;    /*!< 过滤器尺寸寄存器 
                              配置过滤器位宽（16/32位） 
                              地址偏移: 0x20C */
  
  __IO uint32_t FFA1R;   /*!< FIFO分配寄存器 
                              决定过滤器组分配到哪个FIFO 
                              地址偏移: 0x214 */
  
  __IO uint32_t FA1R;    /*!< 过滤器激活寄存器 
                              使能/禁用过滤器组 
                              地址偏移: 0x21C */

  CAN_FilterRegister_TypeDef sFilterRegister[28]; /*!< 过滤器寄存器组 
                                                       每个过滤器组占用2个寄存器 
                                                       地址范围:0x240-0x31C */
} CAN_TypeDef;

/**
 * @brief CRC计算单元寄存器结构体
 * 
 * 本结构体定义CRC校验单元的寄存器映射，
 * 支持多种多项式标准（CRC-32/16/CCITT等）
 */
typedef struct
{
  __IO uint32_t DR;     /*!< 数据寄存器 
                              输入数据/计算结果存储 
                              支持8/16/32位写入 
                              地址偏移: 0x00 */
  
  __IO uint8_t IDR;     /*!< 独立数据寄存器 
                              用于临时存储8位数据 
                              不影响CRC计算结果 
                              地址偏移: 0x04 */
  
  uint8_t RESERVED0;    /*!< 保留字节（0x05）*/
  uint16_t RESERVED1;   /*!< 保留半字（0x06）*/
  
  __IO uint32_t CR;     /*!< 控制寄存器 
                              位[3] RESET：CRC复位 
                              位[5:7] POLYSIZE：多项式尺寸 
                              地址偏移: 0x08 */
} CRC_TypeDef;

/**
 * @brief 数字模拟转换器(DAC)寄存器结构体
 * 
 * 本结构体定义DAC双通道的完整寄存器映射，支持：
 * - 独立/同步双通道输出
 * - 8/12位数据对齐方式
 * - 硬件/软件触发模式
 * 注意：使用前需通过RCC启用DAC时钟
 */
typedef struct
{
  __IO uint32_t CR;       /*!< 控制寄存器 
                               位[0] EN1：通道1使能 
                               位[1] BOFF1：通道1输出缓冲关闭 
                               位[3:2] TEN1：通道1触发使能 
                               位[7:4] TSEL1[3:0]：触发源选择 
                               地址偏移: 0x00 */
  
  __IO uint32_t SWTRIGR;  /*!< 软件触发寄存器 
                               位[0] SWTRIG1：通道1软件触发 
                               位[1] SWTRIG2：通道2软件触发 
                               地址偏移: 0x04 */
  
  /* 通道1数据保持寄存器 */
  __IO uint32_t DHR12R1;  /*!< 通道1 12位右对齐数据寄存器 
                                数据范围：0x000-0xFFF → 0-3.3V 
                                地址偏移: 0x08 */
  
  __IO uint32_t DHR12L1;  /*!< 通道1 12位左对齐数据寄存器 
                                数据范围：0x000-0xFFF << 4 
                                地址偏移: 0x0C */
  
  __IO uint32_t DHR8R1;   /*!< 通道1 8位右对齐数据寄存器 
                                数据范围：0x00-0xFF → 0-3.3V 
                                地址偏移: 0x10 */

  /* 通道2数据保持寄存器 */
  __IO uint32_t DHR12R2;  /*!< 通道2 12位右对齐数据寄存器 
                                独立模式时单独使用 
                                地址偏移: 0x14 */
  
  __IO uint32_t DHR12L2;  /*!< 通道2 12位左对齐数据寄存器 
                                数据左移4位存储 
                                地址偏移: 0x18 */
  
  __IO uint32_t DHR8R2;   /*!< 通道2 8位右对齐数据寄存器 
                                地址偏移: 0x1C */

  /* 双通道同步数据寄存器 */
  __IO uint32_t DHR12RD;  /*!< 双通道12位右对齐联合寄存器 
                                位[15:0] DACC1DHR 
                                位[31:16] DACC2DHR 
                                地址偏移: 0x20 */
  
  __IO uint32_t DHR12LD;  /*!< 双通道12位左对齐联合寄存器 
                                数据左移4位存储 
                                地址偏移: 0x24 */
  
  __IO uint32_t DHR8RD;   /*!< 双通道8位右对齐联合寄存器 
                                地址偏移: 0x28 */

  /* 数据输出寄存器 */
  __IO uint32_t DOR1;     /*!< 通道1实际输出电压寄存器 
                                只读，反映当前模拟输出值 
                                地址偏移: 0x2C */
  
  __IO uint32_t DOR2;     /*!< 通道2实际输出电压寄存器 
                                地址偏移: 0x30 */
  
  __IO uint32_t SR;       /*!< 状态寄存器 
                                位[13] DMAUDR1：通道1DMA下溢标志 
                                位[29] DMAUDR2：通道2DMA下溢标志 
                                地址偏移: 0x34 */
} DAC_TypeDef;

/**
 * @brief 调试MCU寄存器结构体
 * 
 * 本结构体用于系统调试功能配置，包含：
 * - 芯片ID识别
 * - 调试模式配置
 * - 低功耗模式调试支持
 * 注意：调试配置应在程序初始化阶段完成
 */
typedef struct
{
  __IO uint32_t IDCODE;  /*!< 设备标识寄存器 
                               位[31:0] DEV_ID：芯片唯一标识 
                               地址偏移: 0x00 */
  
  __IO uint32_t CR;      /*!< 调试配置寄存器 
                               位[0] DBG_STOP：停止模式调试使能 
                               位[1] DBG_STANDBY：待机模式调试使能 
                               位[8] TRACE_IOEN：跟踪引脚使能 
                               地址偏移: 0x04 */
  
  __IO uint32_t APB1FZ;  /*!< APB1外设调试冻结寄存器 
                               位[27] DBG_IWDG_STOP：看门狗调试时冻结 
                               位[30] DBG_TIM8_STOP：TIM8调试暂停 
                               地址偏移: 0x08 */
  
  __IO uint32_t APB2FZ;  /*!< APB2外设调试冻结寄存器 
                               位[0] DBG_TIM1_STOP：TIM1调试暂停 
                               位[9] DBG_TIM9_STOP：TIM9调试暂停 
                               地址偏移: 0x0C */
} DBGMCU_TypeDef;

/** 
  * @brief DCMI
  */

/**
 * @brief 数字摄像头接口(DCMI)寄存器结构体
 * 
 * 本结构体定义DCMI接口的寄存器映射，支持：
 * - 多种摄像头数据格式接收(YUV/RGB/RAW)
 * - 硬件图像裁剪功能
 * - 帧同步与行同步信号处理
 * 注意：使用前需配置好DMA传输通道
 */
typedef struct
{
  __IO uint32_t CR;       /*!< 控制寄存器 
                               位[0] CAPTURE：捕获使能 
                               位[3] CROP：裁剪功能使能 
                               位[5] JPEG：JPEG格式模式 
                               地址偏移: 0x00 */
  
  __IO uint32_t SR;       /*!< 状态寄存器 
                               位[0] HSYNC：行同步状态 
                               位[1] VSYNC：帧同步状态 
                               位[2] FNE：帧缓冲区非空 
                               地址偏移: 0x04 */
  
  __IO uint32_t RISR;     /*!< 原始中断状态寄存器 
                               位[0] FRAME_RIS：原始帧捕获完成标志 
                               位[2] ERR_RIS：同步错误标志 
                               地址偏移: 0x08 */
  
  __IO uint32_t IER;      /*!< 中断使能寄存器 
                               位[0] FRAME_IE：帧中断使能 
                               位[4] LINE_IE：行中断使能 
                               地址偏移: 0x0C */
  
  __IO uint32_t MISR;     /*!< 屏蔽中断状态寄存器 
                               反映IER使能后的中断状态 
                               地址偏移: 0x10 */
  
  __IO uint32_t ICR;      /*!< 中断清除寄存器 
                               位[0] FRAME_ISC：清除帧中断 
                               位[2] ERR_ISC：清除错误中断 
                               地址偏移: 0x14 */
  
  __IO uint32_t ESCR;     /*!< 嵌入式同步代码寄存器 
                               位[7:0] FSC：帧开始代码 
                               位[23:16] LSC：行开始代码 
                               地址偏移: 0x18 */
  
  __IO uint32_t ESUR;     /*!< 嵌入式同步解除寄存器 
                               位[7:0] FEC：帧结束代码 
                               位[23:16] LEC：行结束代码 
                               地址偏移: 0x1C */
  
  __IO uint32_t CWSTRTR;  /*!< 裁剪窗口起始寄存器 
                               位[15:0] HOFFCNT：水平偏移 
                               位[31:16] VST：垂直起始行 
                               地址偏移: 0x20 */
  
  __IO uint32_t CWSIZER;  /*!< 裁剪窗口尺寸寄存器 
                               位[15:0] CAPTURE_W：捕获宽度 
                               位[31:16] CAPTURE_H：捕获高度 
                               地址偏移: 0x24 */
  
  __IO uint32_t DR;       /*!< 数据寄存器 
                               32位像素数据存储（根据格式调整） 
                               地址偏移: 0x28 */
} DCMI_TypeDef;

/**
 * @brief DMA流控制器寄存器结构体
 * 
 * 每个DMA流包含独立配置寄存器，支持：
 * - 循环模式与双缓冲传输
 * - 不同数据宽度配置
 * - 外设到存储器/存储器到外设传输
 */
typedef struct
{
  __IO uint32_t CR;     /*!< 流控制寄存器 
                             位[6:5] DIR[1:0]：传输方向 
                             位[8:10] PSIZE[1:0]：外设数据宽度 
                             位[13:12] MSIZE[1:0]：内存数据宽度 
                             位[18] CIRC：循环模式使能 */
  
  __IO uint32_t NDTR;   /*!< 数据数量寄存器 
                             剩余传输数据项计数（16位有效）
                             每次传输后自动递减 */
  
  __IO uint32_t PAR;    /*!< 外设地址寄存器 
                             存储外设数据寄存器地址 
                             传输过程不可修改 */
  
  __IO uint32_t M0AR;   /*!< 内存地址0寄存器 
                             主要内存缓冲区地址 
                             支持自动递增 */
  
  __IO uint32_t M1AR;   /*!< 内存地址1寄存器 
                             双缓冲模式下的备用缓冲区地址 
                             CIRC模式自动切换 */
  
  __IO uint32_t FCR;    /*!< FIFO控制寄存器 
                             位[2:0] FTH[1:0]：FIFO阈值配置 
                             位[7] DMDIS：直接模式禁用 */
} DMA_Stream_TypeDef;

/**
 * @brief DMA全局控制器寄存器结构体
 * 
 * 管理所有DMA流的全局中断状态，
 * 包含中断标志的查询与清除功能
 */
typedef struct
{
  __IO uint32_t LISR;   /*!< 低半中断状态寄存器 
                              位[5] TCIF0：流0传输完成标志 
                              位[17] TEIF3：流3传输错误标志 
                              地址偏移: 0x00 */
  
  __IO uint32_t HISR;   /*!< 高半中断状态寄存器 
                              位[5] TCIF4：流4传输完成标志 
                              位[21] HTIF7：流7半传输标志 
                              地址偏移: 0x04 */
  
  __IO uint32_t LIFCR;  /*!< 低半中断清除寄存器 
                              位[5] CTCIF0：清除流0完成标志 
                              位[17] CTEIF3：清除流3错误标志 
                              地址偏移: 0x08 */
  
  __IO uint32_t HIFCR;  /*!< 高半中断清除寄存器 
                              位[5] CTCIF4：清除流4完成标志 
                              地址偏移: 0x0C */
} DMA_TypeDef;

 /**
 * @brief DMA2D图形加速控制器寄存器结构体
 * 
 * 本结构体定义DMA2D的全部寄存器，支持：
 * - 硬件图像填充、混合和格式转换
 * - 前景/背景层颜色查找表(CLUT)
 * - 自动生成渐变色和透明混合
 * 注意：操作前需使能DMA2D时钟并配置层参数
 */
typedef struct
{
  /* 控制与状态寄存器组 */
  __IO uint32_t CR;            /*!< 控制寄存器 
                                    位[0] START：启动传输 
                                    位[16:17] MODE[1:0]：操作模式 
                                    00：寄存器到存储器 01：存储器到存储器 
                                    地址偏移: 0x00 */
  
  __IO uint32_t ISR;           /*!< 中断状态寄存器 
                                    位[0] TEIF：传输错误标志 
                                    位[1] TCIF：传输完成标志 
                                    地址偏移: 0x04 */
  
  __IO uint32_t IFCR;          /*!< 中断清除寄存器 
                                    位[1] CTCIF：清除传输完成标志 
                                    地址偏移: 0x08 */

  /* 前景层配置 */
  __IO uint32_t FGMAR;         /*!< 前景内存起始地址 
                                    存储源图像数据的位置 
                                    地址偏移: 0x0C */
  
  __IO uint32_t FGOR;          /*!< 前景行偏移量 
                                    每行结束后的地址增量 
                                    值= (有效宽度 - 实际宽度)*像素尺寸 
                                    地址偏移: 0x10 */

  /* 背景层配置 */
  __IO uint32_t BGMAR;         /*!< 背景内存起始地址 
                                    混合操作时的第二图像源 
                                    地址偏移: 0x14 */
  
  __IO uint32_t BGOR;          /*!< 背景行偏移量 
                                    计算方法同FGOR 
                                    地址偏移: 0x18 */

  /* 像素格式控制 */
  __IO uint32_t FGPFCCR;       /*!< 前景像素格式控制 
                                    位[3:0] CM[3:0]：颜色模式 
                                    0x2：ARGB8888 0x4：RGB565 
                                    位[8] START：CLUT载入启动 
                                    地址偏移: 0x1C */
  
  __IO uint32_t FGCOLR;        /*!< 前景常量颜色 
                                    当使用无alpha混合模式时使用 
                                    ARGB8888格式存储 
                                    地址偏移: 0x20 */

  /* 背景像素格式控制 */  
  __IO uint32_t BGPFCCR;       /*!< 背景像素格式控制 
                                    位[15:12] AM[3:0]：Alpha混合模式 
                                    0：常规混合 1：预乘Alpha 
                                    地址偏移: 0x24 */
  
  __IO uint32_t BGCOLR;        /*!< 背景常量颜色 
                                    RGB888格式（高位保留） 
                                    地址偏移: 0x28 */

  /* 颜色查找表配置 */
  __IO uint32_t FGCMAR;        /*!< 前景CLUT内存地址 
                                    指向256项颜色查找表 
                                    每项32位(ARGB8888) 
                                    地址偏移: 0x2C */
  
  __IO uint32_t BGCMAR;        /*!< 背景CLUT内存地址 
                                    地址对齐到256字节边界 
                                    地址偏移: 0x30 */

  /* 输出配置 */
  __IO uint32_t OPFCCR;        /*!< 输出像素格式控制 
                                    位[3:0] CM[3:0]：目标格式 
                                    0x6：RGB888 0xB：ARGB1555 
                                    地址偏移: 0x34 */
  
  __IO uint32_t OCOLR;         /*!< 输出颜色寄存器 
                                    存储填充模式的填充颜色 
                                    RGB888或根据格式存储 
                                    地址偏移: 0x38 */

  __IO uint32_t OMAR;          /*!< 输出内存起始地址 
                                    目标帧缓冲区的起始地址 
                                    地址对齐需符合总线要求 
                                    地址偏移: 0x3C */
  
  __IO uint32_t OOR;           /*!< 输出行偏移量 
                                    值= (目标存储宽度 - 实际宽度)*像素尺寸 
                                    地址偏移: 0x40 */

  /* 图像维度配置 */
  __IO uint32_t NLR;           /*!< 行数/列宽设置 
                                    位[15:0] NL：总行数 
                                    位[31:16] PL：每行像素数 
                                    地址偏移: 0x44 */
  
  __IO uint32_t LWR;           /*!< 行水印寄存器 
                                    配置中断产生的行间隔 
                                    地址偏移: 0x48 */

  /* 性能优化配置 */  
  __IO uint32_t AMTCR;         /*!< AHB主定时配置 
                                    位[0] EN：超时检测使能 
                                    位[15:8] DT：检测周期（AHB周期数） 
                                    地址偏移: 0x4C */

  uint32_t      RESERVED[236]; /*!< 保留区域(0x50-0x3FF) */

  /* 颜色查找表存储区 */
  __IO uint32_t FGCLUT[256];   /*!< 前景CLUT存储区 
                                    每个条目格式：0xAARRGGBB 
                                    地址范围: 0x400-0x7FF */
  
  __IO uint32_t BGCLUT[256];   /*!< 背景CLUT存储区 
                                    支持颜色索引重映射 
                                    地址范围: 0x800-0xBFF */
} DMA2D_TypeDef;
/**
 * @brief 以太网MAC控制器寄存器结构体
 * 
 * 本结构体定义STM32F4系列以太网控制器的完整寄存器映射，包含：
 * - MAC层控制与状态
 * - 帧过滤与地址匹配
 * - MII接口管理
 * - 精确时间协议(PTP)支持
 * - DMA传输控制
 * 注意：使用前需正确配置PHY接口和时钟
 */
typedef struct
{
  /* MAC层基础控制 */
  __IO uint32_t MACCR;      /*!< MAC控制寄存器 
                                 位[0] RE：接收使能 
                                 位[13] DC：双工模式 
                                 位[14] BL：突发长度设置 
                                 地址偏移: 0x00 */
  
  __IO uint32_t MACFFR;     /*!< 帧过滤寄存器 
                                 位[4] PM：混杂模式 
                                 位[5] DAIF：反向地址过滤 
                                 地址偏移: 0x04 */

  /* 哈希过滤寄存器 */
  __IO uint32_t MACHTHR;    /*!< 哈希表高位寄存器 
                                 存储64位哈希表的高32位 
                                 地址偏移: 0x08 */
  
  __IO uint32_t MACHTLR;    /*!< 哈希表低位寄存器 
                                 地址偏移: 0x0C */

  /* MII接口管理 */
  __IO uint32_t MACMIIAR;   /*!< MII地址寄存器 
                                 位[1:0] CR：时钟分频 
                                 位[11:6] MR：PHY地址 
                                 位[16:12] GR：寄存器地址 
                                 地址偏移: 0x10 */
  
  __IO uint32_t MACMIIDR;   /*!< MII数据寄存器 
                                 读写PHY寄存器时的数据通道 
                                 地址偏移: 0x14 */

  __IO uint32_t MACFCR;     /*!< 流量控制寄存器 
                                 位[0] FCB：流量控制背压 
                                 位[7] PLT：暂停时间设置 
                                 地址偏移: 0x18 */
  
  __IO uint32_t MACVLANTR;  /*!< VLAN标签寄存器 
                                 位[15:0] VLANTI：VLAN标签 
                                 位[16] VLC：VLAN标签过滤 
                                 地址偏移: 0x1C */ 
  
  uint32_t      RESERVED0[2]; /*!< 保留区域 (0x20-0x24) */

  /* 电源管理 */
  __IO uint32_t MACRWUFFR;  /*!< 远程唤醒帧过滤寄存器 
                                 存储唤醒模式匹配规则 
                                 地址偏移: 0x2C */
  
  __IO uint32_t MACPMTCSR;  /*!< 电源管理控制状态寄存器 
                                 位[0] PD：掉电模式 
                                 位[5] WFE：远程唤醒使能 
                                 地址偏移: 0x38 */

  uint32_t      RESERVED1[2];  /*!< 保留区域 (0x3C-0x40)*/

  /* 状态与中断 */
  __IO uint32_t MACSR;      /*!< MAC状态寄存器 
                                 位[2] PMTS：电源管理事件 
                                 位[16] TSTS：时间戳触发 
                                 地址偏移: 0x44 */
  
  __IO uint32_t MACIMR;     /*!< MAC中断屏蔽寄存器 
                                 位[3] PMTIM：电源管理中断 
                                 地址偏移: 0x48 */

  /* MAC地址寄存器组 */
  __IO uint32_t MACA0HR;    /*!< MAC地址0高位 
                                 位[31] AE：地址使能 
                                 地址偏移: 0x4C */
  
  __IO uint32_t MACA0LR;    /*!< MAC地址0低位 
                                 地址偏移: 0x50 */
  
  __IO uint32_t MACA1HR;    /*!< MAC地址1高位 
                                 地址偏移: 0x54 */
  
  __IO uint32_t MACA1LR;    /*!< MAC地址1低位 
                                 地址偏移: 0x58 */
  
  __IO uint32_t MACA2HR;    /*!< MAC地址2高位 
                                 地址偏移: 0x5C */
  
  __IO uint32_t MACA2LR;    /*!< MAC地址2低位 
                                 地址偏移: 0x60 */
  
  __IO uint32_t MACA3HR;    /*!< MAC地址3高位 
                                 地址偏移: 0x64 */
  
  __IO uint32_t MACA3LR;    /*!< MAC地址3低位 
                                 地址偏移: 0x68 */

  uint32_t      RESERVED2[40]; /*!< 保留区域 (0x6C-0xFC) */

  /* MAC管理计数器 */
  __IO uint32_t MMCCR;      /*!< MMC控制寄存器 
                                 位[0] CR：计数器复位 
                                 地址偏移: 0x100 */
  
  __IO uint32_t MMCRIR;     /*!< MMC接收中断寄存器 
                                 位[5] RGUFS：接收良好单帧 
                                 地址偏移: 0x104 */
  
  __IO uint32_t MMCTIR;     /*!< MMC传输中断寄存器 
                                 位[21] TGFMSC：传输成功多帧 
                                 地址偏移: 0x108 */
  
  __IO uint32_t MMCRIMR;    /*!< MMC接收中断屏蔽 
                                 地址偏移: 0x10C */
  
  __IO uint32_t MMCTIMR;    /*!< MMC传输中断屏蔽 
                                 地址偏移: 0x110 */

  uint32_t      RESERVED3[14]; /*!< 保留区域 (0x114-0x144) */

  /* PTP时间戳寄存器组 */
  __IO uint32_t PTPTSCR;    /*!< 时间戳控制寄存器 
                                 位[0] TSE：时间戳使能 
                                 地址偏移: 0x700 */
  
  __IO uint32_t PTPSSIR;    /*!< 次秒增量寄存器 
                                 配置纳秒级时间精度 
                                 地址偏移: 0x704 */
  
  __IO uint32_t PTPTSHR;    /*!< 系统时间高位 
                                 32位秒数的高16位 
                                 地址偏移: 0x708 */
  
  __IO uint32_t PTPTSLR;    /*!< 系统时间低位 
                                 包含秒低16位和纳秒 
                                 地址偏移: 0x70C */

  /* DMA控制器寄存器 */
  __IO uint32_t DMABMR;     /*!< DMA总线模式寄存器 
                                 位[0] SR：软件复位 
                                 位[14:1] PBL：可编程突发长度 
                                 地址偏移: 0x1000 */
  
  __IO uint32_t DMATDLAR;   /*!< 发送描述符列表地址 
                                 需对齐到4字节边界 
                                 地址偏移: 0x1014 */
  
  __IO uint32_t DMARDLAR;   /*!< 接收描述符列表地址 
                                 地址偏移: 0x101C */
  
  __IO uint32_t DMASR;      /*!< DMA状态寄存器 
                                 位[16] TPS：传输暂停状态 
                                 地址偏移: 0x1020 */
  
  __IO uint32_t DMAIER;     /*!< DMA中断使能寄存器 
                                 位[14] NISE：普通中断总使能 
                                 地址偏移: 0x1038 */
} ETH_TypeDef;

/**
 * @brief 外部中断/事件控制器(EXTI)寄存器结构体
 * 
 * 管理16个可屏蔽中断/事件线，支持：
 * - 上升沿/下降沿触发选择
 * - 软件触发中断
 * - 事件生成机制
 * 注意：需配合SYSCFG进行引脚映射配置
 */
typedef struct
{
  __IO uint32_t IMR;    /*!< 中断屏蔽寄存器 
                             位[15:0] MRx：线x中断屏蔽（1-使能）
                             地址偏移: 0x00 */
  
  __IO uint32_t EMR;    /*!< 事件屏蔽寄存器 
                             位[15:0] MRx：线x事件屏蔽（1-使能）
                             地址偏移: 0x04 */
  
  __IO uint32_t RTSR;   /*!< 上升沿触发选择寄存器 
                             位[15:0] TRx：线x上升沿触发使能
                             地址偏移: 0x08 */
  
  __IO uint32_t FTSR;   /*!< 下降沿触发选择寄存器 
                             位[15:0] TRx：线x下降沿触发使能
                             地址偏移: 0x0C */
  
  __IO uint32_t SWIER;  /*!< 软件中断事件寄存器 
                             写1触发对应线中断（自动清除）
                             地址偏移: 0x10 */
  
  __IO uint32_t PR;     /*!< 挂起寄存器 
                             位[15:0] PRx：线x中断挂起标志（写1清除）
                             地址偏移: 0x14 */
} EXTI_TypeDef;

/**
 * @brief Flash存储器控制寄存器结构体
 * 
 * 管理Flash编程、擦除操作及选项字节配置：
 * - 支持扇区擦除和批量擦除
 * - 编程并行度配置
 * - 读写保护机制
 * 警告：错误操作可能导致芯片锁死
 */
typedef struct
{
  __IO uint32_t ACR;      /*!< 访问控制寄存器 
                              位[0] LATENCY：等待周期（0-7WS）
                              位[8] PRFTEN：预取缓冲使能
                              地址偏移: 0x00 */
  
  __IO uint32_t KEYR;     /*!< 键寄存器 
                              写入0x45670123和0xCDEF89AB解锁FLASH_CR
                              地址偏移: 0x04 */
  
  __IO uint32_t OPTKEYR;  /*!< 选项键寄存器 
                              写入0x08192A3B和0x4C5D6E7F解锁选项字节
                              地址偏移: 0x08 */
  
  __IO uint32_t SR;       /*!< 状态寄存器 
                              位[0] EOP：操作完成标志
                              位[5] WRPERR：写保护错误
                              地址偏移: 0x0C */
  
  __IO uint32_t CR;       /*!< 控制寄存器 
                              位[1] PER：页擦除使能
                              位[16] STRT：开始擦除/编程
                              地址偏移: 0x10 */
  
  __IO uint32_t OPTCR;    /*!< 选项控制寄存器 
                              位[5] RDP：读保护级别（0xAA-LEVEL0）
                              位[21] DB1M：双Bank模式（仅1MB Flash）
                              地址偏移: 0x14 */
  
  __IO uint32_t OPTCR1;   /*!< 选项控制寄存器1 
                              位[16] BOOT_ADD0：启动地址0（系统启动选项）
                              地址偏移: 0x18 */
} FLASH_TypeDef;

#if defined (STM32F40_41xxx)
/**
 * @brief FSMC Bank1 NOR/PSRAM控制器结构体
 * 
 * 配置NOR Flash和PSRAM存储器的：
 * - 片选时序参数
 * - 存储器总线宽度
 * - 突发访问模式
 * 注意：需与GPIO复用功能配合使用
 */
typedef struct
{
  __IO uint32_t BTCR[8];  /*!< Bank控制时序寄存器组 
                               BTCR[0]/[2]/[4]/[6]：BCR1-4（控制寄存器）
                               BTCR[1]/[3]/[5]/[7]：BTR1-4（时序寄存器）
                               地址范围: 0x00-0x1C */
} FSMC_Bank1_TypeDef; 

/**
 * @brief FSMC Bank1E写时序配置结构体
 * 
 * 扩展Bank1的写时序配置，支持：
 * - 独立的地址建立/保持时间
 * - 不同存储器类型的时序优化
 */
typedef struct
{
  __IO uint32_t BWTR[7];  /*!< Bank写时序寄存器组 
                               BWTR[0]/[2]/[4]/[6]：保留
                               BWTR[1]/[3]/[5]：对应Bank1-3写时序
                               地址范围: 0x104-0x11C */
} FSMC_Bank1E_TypeDef;
#endif
#if defined (STM32F40_41xxx)
/**
 * @brief FSMC Bank2 NAND Flash控制器结构体
 * 
 * 管理第二个NAND Flash存储器的：
 * - 控制寄存器配置
 * - 时序参数设置
 * - ECC错误校验功能
 * 注意：需在FSMC全局时钟使能后使用
 */
typedef struct
{
  __IO uint32_t PCR2;       /*!< NAND控制寄存器2 
                                 位[0] PWAITEN：等待特性使能
                                 位[2] PBKEN：存储区使能
                                 位[3] PTYP：存储器类型（0-NAND）
                                 地址偏移: 0x60 */
  
  __IO uint32_t SR2;        /*!< 状态寄存器2 
                                 位[6] FEMPT：FIFO空标志
                                 位[5] IFEN：中断使能
                                 地址偏移: 0x64 */
  
  __IO uint32_t PMEM2;      /*!< 公共存储器时序寄存器2 
                                 位[7:0] MEMSETx：地址建立时间（0-255 HCLK）
                                 位[15:8] MEMWAITx：地址保持时间
                                 地址偏移: 0x68 */
  
  __IO uint32_t PATT2;      /*!< 属性存储器时序寄存器2 
                                 配置属性空间时序
                                 （适用于读/写操作的不同阶段）
                                 地址偏移: 0x6C */
  
  uint32_t      RESERVED0;  /*!< 保留区域 (0x70) */
  
  __IO uint32_t ECCR2;      /*!< ECC校验结果寄存器2 
                                 存储256字节数据的ECC校验码
                                 地址偏移: 0x74 */
} FSMC_Bank2_TypeDef;

/**
 * @brief FSMC Bank3 NAND Flash控制器结构体
 * 
 * 管理第三个NAND Flash存储器的配置，
 * 支持多片选信号和独立时序设置
 */
typedef struct
{
  __IO uint32_t PCR3;       /*!< NAND控制寄存器3 
                                 位[13] ECCPS[2:0]：ECC页大小设置
                                 0：256字节 1：512字节
                                 地址偏移: 0x80 */
  
  __IO uint32_t SR3;        /*!< 状态寄存器3 
                                 位[0] IRS：空闲状态标志
                                 地址偏移: 0x84 */
  
  __IO uint32_t PMEM3;      /*!< 公共存储器时序寄存器3 
                                 配置CLR/CLE信号时序
                                 地址偏移: 0x88 */
  
  __IO uint32_t PATT3;      /*!< 属性存储器时序寄存器3 
                                 配置数据采样阶段时序
                                 地址偏移: 0x8C */
  
  uint32_t      RESERVED0;  /*!< 保留区域 (0x90) */
  
  __IO uint32_t ECCR3;      /*!< ECC校验结果寄存器3 
                                 支持硬件自动纠错
                                 地址偏移: 0x94 */
} FSMC_Bank3_TypeDef;

/**
 * @brief FSMC Bank4 PC Card控制器结构体
 * 
 * 支持PC Card/Compact Flash存储器的：
 * - I/O空间时序配置
 * - 异步传输模式管理
 */
typedef struct
{
  __IO uint32_t PCR4;       /*!< PC Card控制寄存器4 
                                 位[4] TCLR[3:0]：清除时间设置
                                 地址偏移: 0xA0 */
  
  __IO uint32_t SR4;        /*!< 状态寄存器4 
                                 位[3] ILMB：I/O空间锁存标志
                                 地址偏移: 0xA4 */
  
  __IO uint32_t PMEM4;      /*!< 公共存储器时序4 
                                 配置CE#信号有效持续时间
                                 地址偏移: 0xA8 */
  
  __IO uint32_t PATT4;      /*!< 属性存储器时序4 
                                 配置OE#/WE#信号时序
                                 地址偏移: 0xAC */
  
  __IO uint32_t PIO4;       /*!< I/O空间时序寄存器4 
                                 位[15:8] IOSET：I/O建立时间
                                 地址偏移: 0xB0 */
} FSMC_Bank4_TypeDef; 
#endif /* STM32F40_41xxx */


#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
/** 
  * @brief Flexible Memory Controller
  */

typedef struct
{
  __IO uint32_t BTCR[8];    /*!< NOR/PSRAM chip-select control register(BCR) and chip-select timing register(BTR), Address offset: 0x00-1C */   
} FMC_Bank1_TypeDef; 

/** 
  * @brief Flexible Memory Controller Bank1E
  */
  
typedef struct
{
  __IO uint32_t BWTR[7];    /*!< NOR/PSRAM write timing registers, Address offset: 0x104-0x11C */
} FMC_Bank1E_TypeDef;

/** 
  * @brief Flexible Memory Controller Bank2
  */
  
typedef struct
{
  __IO uint32_t PCR2;       /*!< NAND Flash control register 2,                       Address offset: 0x60 */
  __IO uint32_t SR2;        /*!< NAND Flash FIFO status and interrupt register 2,     Address offset: 0x64 */
  __IO uint32_t PMEM2;      /*!< NAND Flash Common memory space timing register 2,    Address offset: 0x68 */
  __IO uint32_t PATT2;      /*!< NAND Flash Attribute memory space timing register 2, Address offset: 0x6C */
  uint32_t      RESERVED0;  /*!< Reserved, 0x70                                                            */
  __IO uint32_t ECCR2;      /*!< NAND Flash ECC result registers 2,                   Address offset: 0x74 */
} FMC_Bank2_TypeDef;

/** 
  * @brief Flexible Memory Controller Bank3
  */
  
typedef struct
{
  __IO uint32_t PCR3;       /*!< NAND Flash control register 3,                       Address offset: 0x80 */
  __IO uint32_t SR3;        /*!< NAND Flash FIFO status and interrupt register 3,     Address offset: 0x84 */
  __IO uint32_t PMEM3;      /*!< NAND Flash Common memory space timing register 3,    Address offset: 0x88 */
  __IO uint32_t PATT3;      /*!< NAND Flash Attribute memory space timing register 3, Address offset: 0x8C */
  uint32_t      RESERVED0;  /*!< Reserved, 0x90                                                            */
  __IO uint32_t ECCR3;      /*!< NAND Flash ECC result registers 3,                   Address offset: 0x94 */
} FMC_Bank3_TypeDef;

/** 
  * @brief Flexible Memory Controller Bank4
  */
  
typedef struct
{
  __IO uint32_t PCR4;       /*!< PC Card  control register 4,                       Address offset: 0xA0 */
  __IO uint32_t SR4;        /*!< PC Card  FIFO status and interrupt register 4,     Address offset: 0xA4 */
  __IO uint32_t PMEM4;      /*!< PC Card  Common memory space timing register 4,    Address offset: 0xA8 */
  __IO uint32_t PATT4;      /*!< PC Card  Attribute memory space timing register 4, Address offset: 0xAC */
  __IO uint32_t PIO4;       /*!< PC Card  I/O space timing register 4,              Address offset: 0xB0 */
} FMC_Bank4_TypeDef; 

/** 
  * @brief Flexible Memory Controller Bank5_6
  */
  
typedef struct
{
  __IO uint32_t SDCR[2];        /*!< SDRAM Control registers ,      Address offset: 0x140-0x144  */
  __IO uint32_t SDTR[2];        /*!< SDRAM Timing registers ,       Address offset: 0x148-0x14C  */
  __IO uint32_t SDCMR;       /*!< SDRAM Command Mode register,    Address offset: 0x150  */
  __IO uint32_t SDRTR;       /*!< SDRAM Refresh Timer register,   Address offset: 0x154  */
  __IO uint32_t SDSR;        /*!< SDRAM Status register,          Address offset: 0x158  */
} FMC_Bank5_6_TypeDef; 
#endif /* STM32F427_437xx ||  STM32F429_439xx */
/**
 * @brief GPIO端口寄存器结构体
 * 
 * 管理最多16个GPIO引脚，支持：
 * - 输入/输出/复用/模拟模式配置
 * - 推挽/开漏输出类型
 * - 4种速度等级
 * - 上下拉电阻配置
 * 注意：使用前需启用对应GPIO时钟
 */
typedef struct
{
  __IO uint32_t MODER;    /*!< 模式寄存器 
                               每2位控制一个引脚模式：
                               00：输入 01：输出 10：复用 11：模拟
                               地址偏移: 0x00 */
  
  __IO uint32_t OTYPER;   /*!< 输出类型寄存器 
                               每1位控制一个引脚类型：
                               0：推挽输出 1：开漏输出
                               地址偏移: 0x04 */
  
  __IO uint32_t OSPEEDR;  /*!< 输出速度寄存器 
                               每2位控制一个引脚速度：
                               00：2MHz  01：25MHz
                               10：50MHz 11：100MHz
                               地址偏移: 0x08 */
  
  __IO uint32_t PUPDR;    /*!< 上拉/下拉寄存器 
                               每2位控制一个引脚：
                               00：无上下拉 01：上拉 10：下拉
                               地址偏移: 0x0C */
  
  __IO uint32_t IDR;      /*!< 输入数据寄存器 
                               只读，位[15:0]对应引脚输入电平
                               地址偏移: 0x10 */
  
  __IO uint32_t ODR;      /*!< 输出数据寄存器 
                               位[15:0]设置引脚输出电平
                               地址偏移: 0x14 */
  
  __IO uint16_t BSRRL;    /*!< 位设置/复位低寄存器 
                               写1设置对应引脚高电平（ODR |= 1<<n）
                               地址偏移: 0x18 */
  
  __IO uint16_t BSRRH;    /*!< 位设置/复位高寄存器 
                               写1清除对应引脚电平（ODR &= ~(1<<n)）
                               地址偏移: 0x1A */
  
  __IO uint32_t LCKR;     /*!< 配置锁寄存器 
                               位[15:0] LCKy：引脚锁定使能
                               位[16] LCKK：写1锁定当前配置
                               地址偏移: 0x1C */
  
  __IO uint32_t AFR[2];   /*!< 复用功能选择寄存器 
                               AFR[0]：引脚0-7复用功能（每4位控制1个引脚）
                               AFR[1]：引脚8-15复用功能
                               地址偏移: 0x20-0x24 */
} GPIO_TypeDef;

/**
 * @brief 系统配置控制器寄存器结构体
 * 
 * 管理芯片级配置功能：
 * - 内存重映射
 * - 外部中断引脚映射
 * - I/O补偿单元控制
 * 注意：EXTI中断线配置必须与本结构体配合使用
 */
typedef struct
{
  __IO uint32_t MEMRMP;       /*!< 内存重映射寄存器 
                                  位[2:0] MEM_MODE：主闪存映射模式
                                  000：主闪存 001：系统闪存
                                  地址偏移: 0x00 */
  
  __IO uint32_t PMC;          /*!< 外设模式配置寄存器 
                                  位[23] MII_RMII_SEL：以太网PHY接口选择
                                  0：RMII 1：MII
                                  地址偏移: 0x04 */
  
  __IO uint32_t EXTICR[4];    /*!< 外部中断配置寄存器组 
                                  EXTICR[0]位[3:0]：EXTI0的GPIO源选择（0-15对应PA0-PH0）
                                  每4位配置1个EXTI线
                                  地址偏移: 0x08-0x14 */
  
  uint32_t      RESERVED[2];  /*!< 保留区域 (0x18-0x1C) */
  
  __IO uint32_t CMPCR;        /*!< 补偿单元控制寄存器 
                                  位[0] CMP_PD：I/O补偿单元使能
                                  地址偏移: 0x20 */
} SYSCFG_TypeDef;

/**
 * @brief I2C总线控制器寄存器结构体
 * 
 * 支持标准模式（100kHz）和快速模式（400kHz）
 * 包含时钟配置、地址管理、状态监控等功能
 */
typedef struct
{
  __IO uint16_t CR1;        /*!< 控制寄存器1 
                                 位[0] PE：外设使能
                                 位[15] SWRST：软件复位
                                 地址偏移: 0x00 */
  
  uint16_t      RESERVED0;  /*!< 保留 (0x02) */
  
  __IO uint16_t CR2;        /*!< 控制寄存器2 
                                 位[5:0] FREQ[5:0]：输入时钟频率（MHz）
                                 地址偏移: 0x04 */
  
  uint16_t      RESERVED1;  /*!< 保留 (0x06) */
  
  __IO uint16_t OAR1;       /*!< 自身地址寄存器1 
                                 位[7:1] ADD[7:1]：7位模式地址
                                 位[15] ADDMODE：地址模式（0-7位，1-10位）
                                 地址偏移: 0x08 */
  
  uint16_t      RESERVED2;  /*!< 保留 (0x0A) */
  
  __IO uint16_t OAR2;       /*!< 自身地址寄存器2 
                                 位[7:1] ADD2[7:1]：双地址模式下的第二地址
                                 地址偏移: 0x0C */
  
  uint16_t      RESERVED3;  /*!< 保留 (0x0E) */
  
  __IO uint16_t DR;         /*!< 数据寄存器 
                                 发送/接收的8位数据存储
                                 地址偏移: 0x10 */
  
  uint16_t      RESERVED4;  /*!< 保留 (0x12) */
  
  __IO uint16_t SR1;        /*!< 状态寄存器1 
                                 位[0] SB：起始位（主模式）
                                 位[7] TxE：数据寄存器空
                                 地址偏移: 0x14 */
  
  uint16_t      RESERVED5;  /*!< 保留 (0x16) */
  
  __IO uint16_t SR2;        /*!< 状态寄存器2 
                                 位[0] MSL：主从模式标志
                                 位[2] TRA：收发模式
                                 地址偏移: 0x18 */
  
  uint16_t      RESERVED6;  /*!< 保留 (0x1A) */
  
  __IO uint16_t CCR;        /*!< 时钟控制寄存器 
                                 位[15] F/S：模式选择（0-标准，1-快速）
                                 位[11:0] CCR[11:0]：时钟分频系数
                                 地址偏移: 0x1C */
  
  uint16_t      RESERVED7;  /*!< 保留 (0x1E) */
  
  __IO uint16_t TRISE;      /*!< 上升时间寄存器 
                                 标准模式：TRISE ≥ 1000ns
                                 快速模式：TRISE ≥ 300ns
                                 地址偏移: 0x20 */
  
  uint16_t      RESERVED8;  /*!< 保留 (0x22) */
  
  __IO uint16_t FLTR;       /*!< 噪声滤波寄存器 
                                 位[3:0] DNF[3:0]：数字滤波器（0-15个时钟周期）
                                 地址偏移: 0x24 */
  
  uint16_t      RESERVED9;  /*!< 保留 (0x26) */
} I2C_TypeDef;
/**
 * @brief 独立看门狗(IWDG)寄存器结构体
 * 
 * 提供硬件级系统故障恢复机制，计时器由独立RC振荡器驱动，
 * 即使系统时钟故障仍可工作。需在计数器归零前"喂狗"。
 * 注意：一旦启用无法通过软件关闭，只能硬件复位。
 */
typedef struct
{
  __IO uint32_t KR;   /*!< 密钥寄存器 
                           写入0xCCCC启动看门狗
                           写入0xAAAA重载计数器（喂狗）
                           写入0x5555允许PR/RLR修改
                           地址偏移: 0x00 */
  
  __IO uint32_t PR;   /*!< 预分频寄存器 
                           位[2:0] PR[2:0]：预分频系数
                           000：4分频  001：8分频 
                           010：16分频 ... 110：256分频
                           地址偏移: 0x04 */
  
  __IO uint32_t RLR;  /*!< 重载寄存器 
                           12位有效值（0-0xFFF）
                           超时时间 = (RLR+1) * (PR分频后的周期)
                           地址偏移: 0x08 */
  
  __IO uint32_t SR;   /*!< 状态寄存器 
                           位[0] PVU：预分频更新中（1-等待）
                           位[1] RVU：重载值更新中
                           地址偏移: 0x0C */
} IWDG_TypeDef;

/**
 * @brief LTDC全局控制器寄存器结构体
 * 
 * 管理TFT-LCD显示屏的全局参数：
 * - 时序信号生成（HSYNC/VSYNC/DE）
 * - 背景层配置
 * - 中断控制
 * 注意：需配合DMA2D进行图像数据传输
 */
typedef struct
{
  uint32_t      RESERVED0[2];  /*!< 保留区域 (0x00-0x04) */
  
  __IO uint32_t SSCR;          /*!< 同步尺寸配置寄存器 
                                    位[15:0] HSYNC：水平同步脉宽（像素数-1）
                                    位[31:16] VSYNC：垂直同步脉宽（行数-1）
                                    地址偏移: 0x08 */
  
  __IO uint32_t BPCR;          /*!< 后沿配置寄存器 
                                    位[15:0] AHBP：水平后沿（像素数-1）
                                    位[31:16] AVBP：垂直后沿（行数-1）
                                    地址偏移: 0x0C */
  
  __IO uint32_t AWCR;          /*!< 有效宽度配置寄存器 
                                    位[15:0] AAW：水平有效像素数（-1）
                                    位[31:16] AAH：垂直有效行数（-1）
                                    地址偏移: 0x10 */
  
  __IO uint32_t TWCR;          /*!< 总宽度配置寄存器 
                                    位[15:0] TOTALW：总水平像素（=HSPW+HBP+AAW+HFP-1）
                                    位[31:16] TOTALH：总垂直行数（=VSPW+VBP+AAH+VFP-1）
                                    地址偏移: 0x14 */
  
  __IO uint32_t GCR;           /*!< 全局控制寄存器 
                                    位[0] LTDCEN：LTDC使能
                                    位[12] HSPOL：HSYNC极性（0-低有效）
                                    地址偏移: 0x18 */
  
  uint32_t      RESERVED1[2];  /*!< 保留区域 (0x1C-0x20) */
  
  __IO uint32_t SRCR;          /*!< 影子重载控制寄存器 
                                    位[0] IMR：立即重载配置
                                    地址偏移: 0x24 */
  
  uint32_t      RESERVED2[1];  /*!< 保留区域 (0x28) */
  
  __IO uint32_t BCCR;          /*!< 背景颜色寄存器 
                                    ARGB8888格式背景色
                                    地址偏移: 0x2C */
  
  uint32_t      RESERVED3[1];  /*!< 保留区域 (0x30) */
  
  __IO uint32_t IER;           /*!< 中断使能寄存器 
                                    位[0] LIE：行中断使能
                                    地址偏移: 0x34 */
  
  __IO uint32_t ISR;           /*!< 中断状态寄存器 
                                    位[2] TERRIE：传输错误标志
                                    地址偏移: 0x38 */
  
  __IO uint32_t ICR;           /*!< 中断清除寄存器 
                                    写1清除对应中断标志
                                    地址偏移: 0x3C */
  
  __IO uint32_t LIPCR;         /*!< 行中断位置寄存器 
                                    设置触发中断的行号
                                    地址偏移: 0x40 */
  
  __IO uint32_t CPSR;          /*!< 当前状态寄存器 
                                    位[15:0] CXPOS：当前渲染的X坐标
                                    位[31:16] CYPOS：当前Y坐标
                                    地址偏移: 0x44 */
  
  __IO uint32_t CDSR;         /*!< 当前显示状态寄存器 
                                    位[0] VSYNCS：垂直同步状态
                                    地址偏移: 0x48 */
} LTDC_TypeDef;  

/**
 * @brief LTDC图层控制器寄存器结构体
 * 
 * 配置单个图层的显示参数：
 * - 窗口位置与尺寸
 * - 像素格式与混合模式
 * - 颜色键控与CLUT配置
 */
typedef struct
{  
  __IO uint32_t CR;            /*!< 图层控制寄存器 
                                    位[0] LEN：图层使能
                                    位[9] COLKEN：颜色键控使能
                                    地址偏移: 0x84 */
  
  __IO uint32_t WHPCR;         /*!< 水平窗口位置寄存器 
                                    位[15:0] WHSTPOS：窗口水平起始（+1）
                                    位[31:16] WHSPPOS：窗口水平终止（-1）
                                    地址偏移: 0x88 */
  
  __IO uint32_t WVPCR;         /*!< 垂直窗口位置寄存器 
                                    位[15:0] WVSTPOS：垂直起始行（+1）
                                    位[31:16] WVSPPOS：垂直终止行（-1）
                                    地址偏移: 0x8C */
  
  __IO uint32_t CKCR;          /*!< 颜色键控寄存器 
                                    ARGB8888格式颜色键值
                                    地址偏移: 0x90 */
  
  __IO uint32_t PFCR;          /*!< 像素格式寄存器 
                                    位[2:0] PF：像素格式
                                    000：ARGB8888 001：RGB888
                                    地址偏移: 0x94 */
  
  __IO uint32_t CACR;          /*!< 固定透明度寄存器 
                                    8位固定Alpha值（0x00-0xFF）
                                    地址偏移: 0x98 */
  
  __IO uint32_t DCCR;          /*!< 默认颜色寄存器 
                                    ARGB8888格式，用于无效像素
                                    地址偏移: 0x9C */
  
  __IO uint32_t BFCR;          /*!< 混合因子寄存器 
                                    位[2:0] BF1：源混合因子
                                    位[10:8] BF2：目标混合因子
                                    地址偏移: 0xA0 */
  
  uint32_t      RESERVED0[2];  /*!< 保留区域 (0xA4-0xA8) */
  
  __IO uint32_t CFBAR;         /*!< 帧缓冲地址寄存器 
                                    需对齐到32字节边界
                                    地址偏移: 0xAC */
  
  __IO uint32_t CFBLR;         /*!< 帧缓冲长度寄存器 
                                    位[15:0] CFBLL：行字节数（=像素宽×bpp + 3）
                                    位[31:16] CFBP：行间填充字节
                                    地址偏移: 0xB0 */
  
  __IO uint32_t CFBLNR;        /*!< 帧缓冲行数寄存器 
                                    位[15:0] CFBLNBR：总行数（-1）
                                    地址偏移: 0xB4 */
  
  uint32_t      RESERVED1[3];  /*!< 保留区域 (0xB8-0xC0) */
  
  __IO uint32_t CLUTWR;         /*!< CLUT写入寄存器 
                                    位[23:0] CLUTADD：颜色索引（0-255）
                                    位[31:24] RED：红色分量
                                    地址偏移: 0x144 */
} LTDC_Layer_TypeDef;

/** 
  * @brief Power Control
  */
/**
  * @brief STM32F4xx系列MCU电源控制(PWR)和时钟控制(RCC)寄存器结构体定义
  * @note  本部分包含两个关键外设的寄存器映射结构：
  *        - PWR_TypeDef: 电源控制寄存器组，用于管理芯片电源相关功能
  *        - RCC_TypeDef: 复位与时钟控制寄存器组，负责系统时钟配置和外设时钟管理
  * @warning 这些结构体通过内存映射访问硬件寄存器，禁止修改字段顺序和数据类型
  */

/* 电源控制寄存器组结构 ------------------------------------------------------*/
typedef struct
{
  __IO uint32_t CR;   /*!< PWR电源控制寄存器（可读写）
                        [0]  : 电压调节器使能
                        [1]  : 芯片低功耗模式选择
                        [8]  : 备份域写保护使能
                        [14] : 可编程电压检测配置 */
  
  __IO uint32_t CSR;  /*!< PWR电源控制/状态寄存器（可读写）
                        [0]  : 备份调节器就绪标志
                        [8]  : 备份域电源使能
                        [14] : 内部电压基准检测标志 */
} PWR_TypeDef;

/**
  * @brief 复位和时钟控制寄存器组结构
  * @detail 包含完整的时钟树控制功能，涵盖：
  *         - 系统时钟源配置（HSI/HSE/PLL）
  *         - 外设时钟使能/禁用
  *         - 低功耗模式时钟管理
  *         - 时钟安全系统(CSS)
  *         - 复位控制功能
  */
typedef struct
{
  /* 基础时钟控制 -----------------------------------------------------------*/
  __IO uint32_t CR;            /*!< RCC时钟控制寄存器（可读写）
                                [0]  : 内部高速时钟(HSI)使能
                                [16] : 外部高速时钟(HSE)使能
                                [24] : PLL使能 */
  
  __IO uint32_t PLLCFGR;       /*!< RCC PLL配置寄存器（可读写）
                                包含PLL预分频器、倍频系数和时钟源选择：
                                [0:5]   : PLLM分频系数
                                [6:14]  : PLLN倍频系数
                                [15:16] : PLLP分频系数
                                [22]    : PLL时钟源选择 */
  
  __IO uint32_t CFGR;          /*!< RCC时钟配置寄存器（可读写）
                                [0:1]  : 系统时钟源选择
                                [4:10] : AHB预分频系数
                                [10:13]: APB1预分频系数
                                [13:15]: APB2预分频系数 */
  
  /* 中断管理 ---------------------------------------------------------------*/
  __IO uint32_t CIR;           /*!< RCC时钟中断寄存器（可读写）
                                [0]    : HSI就绪中断使能
                                [8]    : HSE就绪中断清除
                                [16]   : 时钟安全系统中断标志 */

  /* 外设复位控制 -----------------------------------------------------------*/
  __IO uint32_t AHB1RSTR;      /*!< AHB1总线外设复位寄存器（可读写）
                                控制GPIO/DMA/CRC等外设复位：
                                [0]  : GPIOA复位
                                [21] : DMA1复位
                                [25] : USB OTG FS复位 */
  
  __IO uint32_t AHB2RSTR;      /*!< AHB2总线外设复位寄存器（可读写）
                                [7]  : USB OTG HS复位 */
  
  __IO uint32_t AHB3RSTR;      /*!< AHB3总线外设复位寄存器（可读写）
                                [0]  : FSMC复位
                                [1]  : QSPI复位 */

  uint32_t      RESERVED0;     /*!< 保留区域（地址对齐，不可访问）0x1C */

  /* 外设时钟使能 -----------------------------------------------------------*/
  __IO uint32_t APB1RSTR;      /*!< APB1总线外设复位寄存器（可读写）
                                控制定时器/SPI/I2C等外设复位：
                                [0]  : TIM2复位
                                [11] : SPI2复位
                                [21] : I2C1复位 */
  
  __IO uint32_t APB2RSTR;      /*!< APB2总线外设复位寄存器（可读写）
                                [0]  : TIM1复位
                                [7]  : USART1复位
                                [12] : SPI1复位 */

  uint32_t      RESERVED1[2];  /*!< 保留区域（地址对齐）0x28-0x2C */

  /* 外设时钟使能 -----------------------------------------------------------*/
  __IO uint32_t AHB1ENR;       /*!< AHB1外设时钟使能寄存器（可读写）
                                [0]  : GPIOA时钟使能
                                [21] : DMA1时钟使能
                                [25] : USB OTG FS时钟使能 */
  
  __IO uint32_t AHB2ENR;       /*!< AHB2外设时钟使能寄存器（可读写）
                                [7]  : USB OTG HS时钟使能 */
  
  __IO uint32_t AHB3ENR;       /*!< AHB3外设时钟使能寄存器（可读写）
                                [0]  : FSMC时钟使能
                                [1]  : QSPI时钟使能 */

  uint32_t      RESERVED2;     /*!< 保留区域（地址对齐）0x3C */

  __IO uint32_t APB1ENR;       /*!< APB1外设时钟使能寄存器（可读写）
                                [0]  : TIM2时钟使能
                                [11] : SPI2时钟使能
                                [21] : I2C1时钟使能 */
  
  __IO uint32_t APB2ENR;       /*!< APB2外设时钟使能寄存器（可读写）
                                [0]  : TIM1时钟使能
                                [7]  : USART1时钟使能
                                [12] : SPI1时钟使能 */

  uint32_t      RESERVED3[2];  /*!< 保留区域（地址对齐）0x48-0x4C */

  /* 低功耗模式时钟管理 -----------------------------------------------------*/
  __IO uint32_t AHB1LPENR;     /*!< AHB1低功耗时钟使能寄存器（可读写）
                                睡眠模式下保留的外设时钟配置 */
  
  __IO uint32_t AHB2LPENR;     /*!< AHB2低功耗时钟使能寄存器（可读写） */
  
  __IO uint32_t AHB3LPENR;     /*!< AHB3低功耗时钟使能寄存器（可读写） */

  uint32_t      RESERVED4;     /*!< 保留区域（地址对齐）0x5C */

  __IO uint32_t APB1LPENR;     /*!< APB1低功耗时钟使能寄存器（可读写） */
  
  __IO uint32_t APB2LPENR;     /*!< APB2低功耗时钟使能寄存器（可读写） */

  uint32_t      RESERVED5[2];  /*!< 保留区域（地址对齐）0x68-0x6C */

  /* 特殊功能配置 -----------------------------------------------------------*/
  __IO uint32_t BDCR;          /*!< 备份域控制寄存器（可读写）
                                [0]  : 外部低速时钟(LSE)使能
                                [8]  : RTC时钟源选择
                                [15] : 备份域软件复位 */
  
  __IO uint32_t CSR;           /*!< 时钟控制与状态寄存器（可读写）
                                [0]  : 内部低速时钟(LSI)使能
                                [24] : 低功耗复位标志 */

  uint32_t      RESERVED6[2];  /*!< 保留区域（地址对齐）0x78-0x7C */

  /* 高级时钟配置 -----------------------------------------------------------*/
  __IO uint32_t SSCGR;         /*!< 扩频时钟生成寄存器（可读写）
                                用于EMI抑制的时钟调制配置 */
  
  __IO uint32_t PLLI2SCFGR;    /*!< PLLI2S配置寄存器（可读写）
                                I2S专用锁相环参数配置 */
  
  __IO uint32_t PLLSAICFGR;    /*!< PLLSAI配置寄存器（可读写）
                                SAI/液晶控制器专用锁相环配置 */
  
  __IO uint32_t DCKCFGR;       /*!< 专用时钟配置寄存器（可读写）
                                [16] : TIM1时钟选择
                                [24] : HDMI-CEC时钟源选择 */

} RCC_TypeDef;
/**
  * @brief STM32F4实时时钟(RTC)外设寄存器映射结构体
  * @note  完整映射RTC模块功能，包含：
  *        - 时间/日期基本计时功能
  *        - 双闹钟系统（Alarm A/B）
  *        - 精密时钟校准机制
  *        - 时间戳事件记录
  *        - 20个掉电保持的备份寄存器
  * @warning 操作前需使能PWR_CR的DBP位获得寄存器写权限
  */

typedef struct
{
  /* 基础计时单元 -----------------------------------------------------------*/
  __IO uint32_t TR;      /*!< RTC时间寄存器（可读写）0x00
                          BCD格式存储：时(22:16)|分(14:8)|秒(6:0) */
  
  __IO uint32_t DR;      /*!< RTC日期寄存器（可读写）0x04 
                          BCD格式存储：年(23:16)|月(20:16)|日(14:8)|星期(6:0) */

  /* 控制与状态 -------------------------------------------------------------*/
  __IO uint32_t CR;      /*!< 控制寄存器（可读写）0x08
                          [2:0]  : 时间格式（二进制/BCD）
                          [5]    : 输出时钟分频选择 */
  
  __IO uint32_t ISR;     /*!< 初始化状态寄存器（可读写）0x0C
                          [6]    : 初始化模式进入状态
                          [15]   : 自动唤醒标志 */

  /* 时钟分频配置 -----------------------------------------------------------*/
  __IO uint32_t PRER;    /*!< 预分频寄存器（可读写）0x10
                          同步分频器(15:0)|异步分频器(31:16) 范围：0x00000001-0x000F FFFF */
  
  __IO uint32_t WUTR;    /*!< 唤醒定时器（可读写）0x14 
                          16位递减计数器，0值时触发唤醒中断 */

  /* 校准机制 ---------------------------------------------------------------*/
  __IO uint32_t CALIBR;  /*!< 校准寄存器（可读写）0x18
                          [6:0]   : 正负127时钟周期补偿
                          [7]     : 补偿方向（0=+，1=-） */

  /* 闹钟系统 ---------------------------------------------------------------*/
  __IO uint32_t ALRMAR;  /*!< 闹钟A配置（可读写）0x1C
                          [31]    : 闹钟使能位
                          [23:20] : 日期/小时匹配选择位 */
  
  __IO uint32_t ALRMBR;  /*!< 闹钟B配置（可读写）0x20
                          功能同ALRMAR，独立触发通道 */

  /* 安全控制 ---------------------------------------------------------------*/
  __IO uint32_t WPR;     /*!< 写保护（只写）0x24
                          需连续写入0xCA->0x53解除写保护 */

  /* 高精度计时 -------------------------------------------------------------*/
  __IO uint32_t SSR;     /*!< 亚秒计数器（只读）0x28
                          [15:0] : 当前亚秒值（同步预分频器的实时递减值） */
  
  __IO uint32_t SHIFTR;  /*!< 时钟偏移调节（只写）0x2C
                          [30:0] : ±32767时钟周期补偿（秒级调整） */

  /* 时间戳系统 -------------------------------------------------------------*/
  __IO uint32_t TSTR;    /*!< 时间戳时间（只读）0x30
                          外部事件触发瞬间冻结的时间值 */
  
  __IO uint32_t TSDR;    /*!< 时间戳日期（只读）0x34
                          与TSTR对应的日期信息 */
  
  __IO uint32_t TSSSR;   /*!< 时间戳亚秒（只读）0x38
                          事件触发时的精确亚秒计数 */

  /* 高级校准 ---------------------------------------------------------------*/
  __IO uint32_t CALR;    /*!< 周期校准（可读写）0x3C
                          [8:0]  : 每1048576个RTC时钟周期补偿1个时钟 */

  /* 安全监控 ---------------------------------------------------------------*/
  __IO uint32_t TAFCR;   /*!< 侵入检测配置（可读写）0x40
                          [2:0]  : 时间戳触发源选择
                          [11:8] : 侵入检测滤波设置 */

  /* 高精度闹钟 -------------------------------------------------------------*/
  __IO uint32_t ALRMASSR;/*!< 闹钟A亚秒配置（可读写）0x44
                          [14:0] : 亚秒匹配值 
                          [23:16]: 亚秒比较掩码 */
  
  __IO uint32_t ALRMBSSR;/*!< 闹钟B亚秒配置（可读写）0x48
                          功能同ALRMASSR，支持纳秒级触发精度 */

  /* 存储保留区 -------------------------------------------------------------*/
  uint32_t RESERVED7;    /*!< 地址对齐保留区域 0x4C */

  /* 掉电保持区 -------------------------------------------------------------*/
  __IO uint32_t BKP0R;   /*!< 备份寄存器0（可读写）0x50
                          在VBAT供电时保持数据，最大支持80字节用户数据存储 */
  __IO uint32_t BKP1R;   /*!< 备份寄存器1（可读写）0x54 */
  __IO uint32_t BKP2R;   /*!< 备份寄存器2（可读写）0x58 */
  __IO uint32_t BKP3R;   /*!< 备份寄存器3（可读写）0x5C */
  __IO uint32_t BKP4R;   /*!< 备份寄存器4（可读写）0x60 */
  __IO uint32_t BKP5R;   /*!< 备份寄存器5（可读写）0x64 */
  __IO uint32_t BKP6R;   /*!< 备份寄存器6（可读写）0x68 */
  __IO uint32_t BKP7R;   /*!< 备份寄存器7（可读写）0x6C */
  __IO uint32_t BKP8R;   /*!< 备份寄存器8（可读写）0x70 */
  __IO uint32_t BKP9R;   /*!< 备份寄存器9（可读写）0x74 */
  __IO uint32_t BKP10R;  /*!< 备份寄存器10（可读写）0x78 */
  __IO uint32_t BKP11R;  /*!< 备份寄存器11（可读写）0x7C */
  __IO uint32_t BKP12R;  /*!< 备份寄存器12（可读写）0x80 */
  __IO uint32_t BKP13R;  /*!< 备份寄存器13（可读写）0x84 */
  __IO uint32_t BKP14R;  /*!< 备份寄存器14（可读写）0x88 */
  __IO uint32_t BKP15R;  /*!< 备份寄存器15（可读写）0x8C */
  __IO uint32_t BKP16R;  /*!< 备份寄存器16（可读写）0x90 */
  __IO uint32_t BKP17R;  /*!< 备份寄存器17（可读写）0x94 */
  __IO uint32_t BKP18R;  /*!< 备份寄存器18（可读写）0x98 */
  __IO uint32_t BKP19R;  /*!< 备份寄存器19（可读写）0x9C */
} RTC_TypeDef;

/**
  * @brief STM32F4音频接口(SAI)和存储卡接口(SDIO)外设寄存器映射
  * @note  包含两个独立外设模块：
  *        - SAI: 高性能音频接口，支持I2S/LSB/MSB/PCM协议
  *        - SDIO: SD存储卡/MMC卡控制器，兼容SD规范4.0
  * @warning 操作前需通过RCC使能对应外设时钟
  */

/* 串行音频接口(SAI) --------------------------------------------------------*/
typedef struct
{
  __IO uint32_t GCR;      /*!< SAI全局配置寄存器（可读写）0x00
                          [5:0]   : 主时钟分频因子 
                          [8]     : 同步模式使能 */
} SAI_TypeDef;

/* SAI音频块寄存器组 */
typedef struct
{
  __IO uint32_t CR1;      /*!< 配置寄存器1（可读写）0x04
                          [1:0]   : 工作模式（主/从）
                          [5]     : 数据格式（16/32位） */
  
  __IO uint32_t CR2;      /*!< 配置寄存器2（可读写）0x08
                          [0]     : DMA传输使能
                          [13:8]  : FIFO阈值设置 */

  __IO uint32_t FRCR;     /*!< 帧配置寄存器（可读写）0x0C
                          [15:0]  : 帧长度（8-256位）
                          [31:16] : 帧同步极性配置 */

  __IO uint32_t SLOTR;    /*!< 时隙寄存器（可读写）0x10
                          [15:0]  : 有效时隙位掩码
                          [19:16] : 时隙尺寸配置 */

  __IO uint32_t IMR;      /*!< 中断屏蔽寄存器（可读写）0x14
                          [0]     : 传输完成中断使能
                          [6]     : FIFO错误中断使能 */

  __IO uint32_t SR;       /*!< 状态寄存器（只读）0x18
                          [0]     : 数据寄存器空标志
                          [4]     : 上溢错误标志 */

  __IO uint32_t CLRFR;    /*!< 标志清除寄存器（只写）0x1C
                          写1清除对应状态位 */

  __IO uint32_t DR;       /*!< 数据寄存器（可读写）0x20
                          32位数据缓冲区，支持打包模式 */
} SAI_Block_TypeDef;

/**
  * @brief SDIO存储卡接口寄存器组
  * @detail 支持SD卡规范4.0，最高支持48MHz时钟
  *         集成专用FIFO缓冲区和CRC校验单元
  */
typedef struct
{
  /* 基础控制 -------------------------------------------------------------*/
  __IO uint32_t POWER;    /*!< 电源控制（可读写）0x00
                          [1:0]   : 电源状态（0=关,3=开）
                          [9:8]   : 总线电压选择 */
  
  __IO uint32_t CLKCR;    /*!< 时钟控制（可读写）0x04
                          [8:0]   : 时钟分频因子（0=关）
                          [10]    : 电源保存模式 */

  /* 命令传输系统 ---------------------------------------------------------*/
  __IO uint32_t ARG;      /*!< 命令参数（可读写）0x08
                          存储当前命令的32位参数 */
  
  __IO uint32_t CMD;      /*!< 命令寄存器（可读写）0x0C
                          [5:0]   : 命令索引（0-63）
                          [6]     : 等待响应使能 */

  /* 响应接收系统 ---------------------------------------------------------*/
  __I uint32_t  RESPCMD;  /*!< 最后响应命令（只读）0x10
                          存储收到响应的命令索引 */
  
  __I uint32_t  RESP1;    /*!< 响应数据1（只读）0x14
                          CMD8/58等返回的32位响应 */
  
  __I uint32_t  RESP2;    /*!< 响应数据2（只读）0x18 
                          CID/CSD寄存器部分数据 */
  
  __I uint32_t  RESP3;    /*!< 响应数据3（只读）0x1C 
                          CID/CSD寄存器部分数据 */
  
  __I uint32_t  RESP4;    /*!< 响应数据4（只读）0x20 
                          OCR寄存器内容 */

  /* 数据传输系统 ---------------------------------------------------------*/
  __IO uint32_t DTIMER;   /*!< 数据超时（可读写）0x24
                          [31:0]  : 超时周期（卡时钟数） */
  
  __IO uint32_t DLEN;     /*!< 数据长度（可读写）0x28
                          [24:0]  : 传输字节数（0-0x1FFFFFF） */
  
  __IO uint32_t DCTRL;    /*!< 数据控制（可读写）0x2C
                          [1:0]   : 传输方向（0=主机→卡）
                          [3]     : 块传输模式使能 */

  __I uint32_t  DCOUNT;   /*!< 剩余数据量（只读）0x30
                          实时显示未传输字节数 */

  /* 状态监控系统 ---------------------------------------------------------*/
  __I uint32_t  STA;      /*!< 状态寄存器（只读）0x34
                          [0]     : 命令响应超时
                          [5]     : 数据块传输完成 */
  
  __IO uint32_t ICR;      /*!< 中断清除（可读写）0x38
                          写1清除对应状态位 */
  
  __IO uint32_t MASK;     /*!< 中断屏蔽（可读写）0x3C
                          [0]     : 命令超时中断使能 */

  /* 存储保留区 -----------------------------------------------------------*/
  uint32_t      RESERVED0[2]; /*!< 保留区域（地址对齐）0x40-0x44 */

  /* FIFO管理系统 ---------------------------------------------------------*/
  __I uint32_t  FIFOCNT;  /*!< FIFO计数器（只读）0x48
                          [23:0] : 当前FIFO中的字节数 */
  
  uint32_t      RESERVED1[13]; /*!< 保留区域（地址对齐）0x4C-0x7C */
  
  __IO uint32_t FIFO;     /*!< 数据FIFO（可读写）0x80
                          32位访问，支持字节/半字/字操作 */
} SDIO_TypeDef;

/**
  * @brief STM32F4串行外设接口(SPI)和定时器(TIM)外设寄存器映射
  * @note  包含两个独立外设模块：
  *        - SPI: 全双工同步串行接口，支持主/从模式及硬件CRC校验
  *        - TIM: 高级控制定时器，支持PWM输出/输入捕获/编码器接口
  * @warning 定时器配置需注意时钟分频与自动重载值的匹配关系
  */

/* 串行外设接口(SPI) ------------------------------------------------------*/
typedef struct
{
  __IO uint16_t CR1;        /*!< 控制寄存器1（可读写）0x00 
                            [1:0]   : 时钟分频（FPCLK/2 ~ /256）
                            [2]     : 主从模式选择（0=从，1=主） */
  
  uint16_t      RESERVED0;  /*!< 保留区域（地址对齐）0x02 */

  __IO uint16_t CR2;        /*!< 控制寄存器2（可读写）0x04
                            [0]     : Rx缓冲DMA使能
                            [6]     : 硬件从机选择使能 */
  
  uint16_t      RESERVED1;  /*!< 保留区域（地址对齐）0x06 */

  __IO uint16_t SR;         /*!< 状态寄存器（只读）0x08
                            [0]     : 接收缓冲非空标志
                            [1]     : 发送缓冲空标志 */
  
  uint16_t      RESERVED2;  /*!< 保留区域（地址对齐）0x0A */

  __IO uint16_t DR;         /*!< 数据寄存器（可读写）0x0C
                            16位数据收发缓冲区，支持8/16位模式 */
  
  uint16_t      RESERVED3;  /*!< 保留区域（地址对齐）0x0E */

  __IO uint16_t CRCPR;      /*!< CRC多项式寄存器（可读写）0x10
                            配置CRC计算使用的多项式（默认0x107） */
  
  uint16_t      RESERVED4;  /*!< 保留区域（地址对齐）0x12 */

  __IO uint16_t RXCRCR;     /*!< 接收CRC值（只读）0x14
                            存储接收数据的CRC计算结果 */
  
  uint16_t      RESERVED5;  /*!< 保留区域（地址对齐）0x16 */

  __IO uint16_t TXCRCR;     /*!< 发送CRC值（只读）0x18
                            存储发送数据的CRC计算结果 */
  
  uint16_t      RESERVED6;  /*!< 保留区域（地址对齐）0x1A */

  __IO uint16_t I2SCFGR;    /*!< I2S配置寄存器（可读写）0x1C
                            [11]    : I2S模式使能（0=SPI，1=I2S） */
  
  uint16_t      RESERVED7;  /*!< 保留区域（地址对齐）0x1E */

  __IO uint16_t I2SPR;      /*!< I2S预分频器（可读写）0x20
                            [8:0]   : 分频系数（I2S时钟生成） */
  
  uint16_t      RESERVED8;  /*!< 保留区域（地址对齐）0x22 */
} SPI_TypeDef;

/**
  * @brief 高级控制定时器寄存器组
  * @detail 支持：
  *         - 向上/向下/中央对齐计数模式
  *         - 4路独立PWM输出
  *         - 死区时间插入
  *         - 霍尔传感器接口
  */
typedef struct
{
  /* 基础控制 -------------------------------------------------------------*/
  __IO uint16_t CR1;         /*!< 控制寄存器1（可读写）0x00
                             [0]     : 计数器使能
                             [3]     : 单脉冲模式 */
  
  uint16_t      RESERVED0;   /*!< 保留区域（地址对齐）0x02 */

  __IO uint16_t CR2;         /*!< 控制寄存器2（可读写）0x04
                             [4:0]   : 主模式输出选择 */
  
  uint16_t      RESERVED1;   /*!< 保留区域（地址对齐）0x06 */

  /* 同步控制 -------------------------------------------------------------*/
  __IO uint16_t SMCR;        /*!< 从模式控制（可读写）0x08
                             [2:0]   : 触发源选择（TI1/ITR0等） */
  
  uint16_t      RESERVED2;   /*!< 保留区域（地址对齐）0x0A */

  /* 中断系统 -------------------------------------------------------------*/
  __IO uint16_t DIER;        /*!< 中断使能（可读写）0x0C
                             [0]     : 更新中断使能
                             [1]     : 捕获/比较1中断使能 */
  
  uint16_t      RESERVED3;   /*!< 保留区域（地址对齐）0x0E */

  /* 状态监控 -------------------------------------------------------------*/
  __IO uint16_t SR;          /*!< 状态寄存器（只读）0x10
                             [0]     : 更新事件标志
                             [1]     : 捕获/比较1标志 */
  
  uint16_t      RESERVED4;   /*!< 保留区域（地址对齐）0x12 */

  __IO uint16_t EGR;         /*!< 事件生成（只写）0x14
                             写1触发更新事件 */
  
  uint16_t      RESERVED5;   /*!< 保留区域（地址对齐）0x16 */

  /* 通道配置 -------------------------------------------------------------*/
  __IO uint16_t CCMR1;       /*!< 通道1/2模式（可读写）0x18
                             [1:0]   : CC1通道模式（输入/输出） */
  
  uint16_t      RESERVED6;   /*!< 保留区域（地址对齐）0x1A */

  __IO uint16_t CCMR2;       /*!< 通道3/4模式（可读写）0x1C
                             功能同CCMR1，对应通道3/4 */
  
  uint16_t      RESERVED7;   /*!< 保留区域（地址对齐）0x1E */

  __IO uint16_t CCER;        /*!< 通道使能（可读写）0x20
                             [0]     : CC1输出使能 */
  
  uint16_t      RESERVED8;   /*!< 保留区域（地址对齐）0x22 */

  /* 计数系统 -------------------------------------------------------------*/
  __IO uint32_t CNT;         /*!< 计数器值（可读写）0x24
                             实时反映当前计数值 */
  
  __IO uint16_t PSC;         /*!< 预分频器（可读写）0x28
                             0x0000-0xFFFF分频系数 */
  
  uint16_t      RESERVED9;   /*!< 保留区域（地址对齐）0x2A */

  __IO uint32_t ARR;         /*!< 自动重载值（可读写）0x2C
                             设定定时周期 */

  /* 高级功能 -------------------------------------------------------------*/
  __IO uint16_t RCR;         /*!< 重复计数器（可读写）0x30
                             高级定时器更新事件延迟计数 */
  
  uint16_t      RESERVED10;  /*!< 保留区域（地址对齐）0x32 */

  __IO uint32_t CCR1;        /*!< 通道1比较值（可读写）0x34
                             PWM模式下设置占空比 */
  
  __IO uint32_t CCR2;        /*!< 通道2比较值（可读写）0x38 */
  
  __IO uint32_t CCR3;        /*!< 通道3比较值（可读写）0x3C */
  
  __IO uint32_t CCR4;        /*!< 通道4比较值（可读写）0x40 */

  /* 电机控制 -------------------------------------------------------------*/
  __IO uint16_t BDTR;        /*!< 断路死区（可读写）0x44
                             [7:0]   : 死区时间配置 */
  
  uint16_t      RESERVED11;  /*!< 保留区域（地址对齐）0x46 */

  /* DMA系统 --------------------------------------------------------------*/
  __IO uint16_t DCR;         /*!< DMA控制（可读写）0x48
                             [4:0]   : DMA基地址 */
  
  uint16_t      RESERVED12;  /*!< 保留区域（地址对齐）0x4A */

  __IO uint16_t DMAR;        /*!< DMA地址（可读写）0x4C
                             缓冲传输目标地址 */
  
  uint16_t      RESERVED13;  /*!< 保留区域（地址对齐）0x4E */

  /* 复用功能 -------------------------------------------------------------*/
  __IO uint16_t OR;          /*!< 选项寄存器（可读写）0x50
                             [0]     : 输入捕获1重映射 */
  
  uint16_t      RESERVED14;  /*!< 保留区域（地址对齐）0x52 */
} TIM_TypeDef;
/**
  * @brief STM32F4通用串口(USART)、窗口看门狗(WWDG)和加密处理器(CRYP)寄存器映射
  * @note  包含三个关键外设：
  *        - USART: 全双工异步串行通信接口
  *        - WWDG: 窗口式系统看门狗定时器
  *        - CRYP: 硬件加密加速单元（支持AES/DES/TDES等算法）
  * @warning 操作加密处理器前需确保时钟使能并清除敏感数据
  */

/* 通用异步收发器(USART) ---------------------------------------------------*/
typedef struct
{
  __IO uint16_t SR;         /*!< 状态寄存器（可读/部分位可写）0x00
                            [5]     : 发送完成标志
                            [6]     : 发送缓冲空标志 */
  
  uint16_t      RESERVED0;  /*!< 保留区域（地址对齐）0x02 */

  __IO uint16_t DR;         /*!< 数据寄存器（可读写）0x04
                            发送和接收的9位数据存储区 */
  
  uint16_t      RESERVED1;  /*!< 保留区域（地址对齐）0x06 */

  __IO uint16_t BRR;        /*!< 波特率寄存器（可读写）0x08
                            计算公式：BRR = fCK / (16 * Baud) */
  
  uint16_t      RESERVED2;  /*!< 保留区域（地址对齐）0x0A */

  __IO uint16_t CR1;        /*!< 控制寄存器1（可读写）0x0C
                            [2]     : 接收使能
                            [3]     : 发送使能 */
  
  uint16_t      RESERVED3;  /*!< 保留区域（地址对齐）0x0E */

  __IO uint16_t CR2;        /*!< 控制寄存器2（可读写）0x10
                            [11:12] : 停止位配置（00=1位,11=2位） */
  
  uint16_t      RESERVED4;  /*!< 保留区域（地址对齐）0x12 */

  __IO uint16_t CR3;        /*!< 控制寄存器3（可读写）0x14
                            [6]     : DMA接收使能 */
  
  uint16_t      RESERVED5;  /*!< 保留区域（地址对齐）0x16 */

  __IO uint16_t GTPR;       /*!< 保护时间预分频（可读写）0x18
                            [7:0]   : 智能卡模式保护时间 */
  
  uint16_t      RESERVED6;  /*!< 保留区域（地址对齐）0x1A */
} USART_TypeDef;

/* 窗口看门狗(WWDG) --------------------------------------------------------*/
typedef struct
{
  __IO uint32_t CR;   /*!< 控制寄存器（可读写）0x00
                      [6:0]   : 计数器值（0x40-0x7F）
                      [7]     : 看门狗激活位 */
  
  __IO uint32_t CFR;  /*!< 配置寄存器（可读写）0x04
                      [6:0]   : 窗口值（必须>0x40）
                      [8:7]   : 时钟预分频（00=1分频） */
  
  __IO uint32_t SR;   /*!< 状态寄存器（只读）0x08
                      [0]     : 早期唤醒中断标志 */
} WWDG_TypeDef;

/* 加密处理器(CRYP) --------------------------------------------------------*/
typedef struct
{
  /* 控制与状态 -----------------------------------------------------------*/
  __IO uint32_t CR;         /*!< 控制寄存器（可读写）0x00
                            [3:0]   : 算法选择（0010=AES-ECB） */
  
  __IO uint32_t SR;         /*!< 状态寄存器（只读）0x04
                            [3]     : 输入FIFO满标志 */
  
  __IO uint32_t DR;         /*!< 数据输入（可读写）0x08
                            32位明文/密文输入缓冲区 */
  
  __IO uint32_t DOUT;       /*!< 数据输出（只读）0x0C
                            加密/解密结果输出 */

  /* DMA控制 --------------------------------------------------------------*/
  __IO uint32_t DMACR;      /*!< DMA控制（可读写）0x10
                            [0]     : 输入DMA使能 */

  /* 中断系统 -------------------------------------------------------------*/
  __IO uint32_t IMSCR;      /*!< 中断屏蔽（可读写）0x14
                            [0]     : 输入FIFO服务中断使能 */
  
  __I uint32_t  RISR;       /*!< 原始中断状态（只读）0x18 */
  
  __I uint32_t  MISR;       /*!< 屏蔽中断状态（只读）0x1C */

  /* 密钥系统 -------------------------------------------------------------*/
  __IO uint32_t K0LR;       /*!< 密钥左半0（可读写）0x20 AES-128密钥字0 */
  __IO uint32_t K0RR;       /*!< 密钥右半0（可读写）0x24 AES-128密钥字1 */
  __IO uint32_t K1LR;       /*!< 密钥左半1（可读写）0x28 AES-128密钥字2 */
  __IO uint32_t K1RR;       /*!< 密钥右半1（可读写）0x2C AES-128密钥字3 */
  __IO uint32_t K2LR;       /*!< 密钥左半2（可读写）0x30 AES-256扩展密钥 */
  __IO uint32_t K2RR;       /*!< 密钥右半2（可读写）0x34 */
  __IO uint32_t K3LR;       /*!< 密钥左半3（可读写）0x38 */
  __IO uint32_t K3RR;       /*!< 密钥右半3（可读写）0x3C */

  /* 初始化向量 -----------------------------------------------------------*/
  __IO uint32_t IV0LR;      /*!< 初始化向量左半0（可读写）0x40 */
  __IO uint32_t IV0RR;      /*!< 初始化向量右半0（可读写）0x44 */
  __IO uint32_t IV1LR;      /*!< 初始化向量左半1（可读写）0x48 */
  __IO uint32_t IV1RR;      /*!< 初始化向量右半1（可读写）0x4C */

  /* 上下文交换 -----------------------------------------------------------*/
  __IO uint32_t CSGCMCCM0R; /*!< GCM/CCM上下文0（可读写）0x50 */
  __IO uint32_t CSGCMCCM1R; /*!< GCM/CCM上下文1（可读写）0x54 */
  __IO uint32_t CSGCMCCM2R; /*!< GCM/CCM上下文2（可读写）0x58 */
  __IO uint32_t CSGCMCCM3R; /*!< GCM/CCM上下文3（可读写）0x5C */
  __IO uint32_t CSGCMCCM4R; /*!< GCM/CCM上下文4（可读写）0x60 */
  __IO uint32_t CSGCMCCM5R; /*!< GCM/CCM上下文5（可读写）0x64 */
  __IO uint32_t CSGCMCCM6R; /*!< GCM/CCM上下文6（可读写）0x68 */
  __IO uint32_t CSGCMCCM7R; /*!< GCM/CCM上下文7（可读写）0x6C */
  __IO uint32_t CSGCM0R;    /*!< GCM上下文0（可读写）0x70 */
  __IO uint32_t CSGCM1R;    /*!< GCM上下文1（可读写）0x74 */
  __IO uint32_t CSGCM2R;    /*!< GCM上下文2（可读写）0x78 */
  __IO uint32_t CSGCM3R;    /*!< GCM上下文3（可读写）0x7C */
  __IO uint32_t CSGCM4R;    /*!< GCM上下文4（可读写）0x80 */
  __IO uint32_t CSGCM5R;    /*!< GCM上下文5（可读写）0x84 */
  __IO uint32_t CSGCM6R;    /*!< GCM上下文6（可读写）0x88 */
  __IO uint32_t CSGCM7R;    /*!< GCM上下文7（可读写）0x8C */
} CRYP_TypeDef;
/**
  * @brief STM32F4哈希处理器(HASH)、随机数发生器(RNG)及内存映射地址定义
  * @note  包含三个关键部分：
  *        - HASH: 支持SHA-1/MD5等哈希算法
  *        - RNG: 真随机数生成器
  *        - 内存映射: 芯片存储区域基地址定义
  * @warning 操作HASH/RNG前需通过RCC使能对应外设时钟
  */

/* 哈希处理器(HASH) -------------------------------------------------------*/
typedef struct 
{
  __IO uint32_t CR;        /*!< 控制寄存器（可读写）0x00
                           [5:0]   : 算法选择（0=MD5,2=SHA1） */
  
  __IO uint32_t DIN;       /*!< 数据输入（可读写）0x04
                           512位数据块输入缓冲区 */
  
  __IO uint32_t STR;       /*!< 启动寄存器（只写）0x08
                           写1启动哈希计算 */

  __IO uint32_t HR[5];     /*!< 摘要寄存器组（只读）0x0C-0x1C
                           MD5输出4个字，SHA1输出5个字 */

  __IO uint32_t IMR;       /*!< 中断屏蔽（可读写）0x20
                           [0]     : 输入就绪中断使能 */
  
  __IO uint32_t SR;        /*!< 状态寄存器（只读）0x24
                           [0]     : 输入缓冲区就绪标志 */

  uint32_t RESERVED[52];   /*!< 保留区域（地址对齐）0x28-0xF4 */

  __IO uint32_t CSR[54];   /*!< 上下文寄存器组（可读写）0xF8-0x1CC
                           保存中间计算状态，用于多块数据处理 */
} HASH_TypeDef;

/* 哈希摘要扩展结构 -------------------------------------------------------*/
typedef struct 
{
  __IO uint32_t HR[8];     /*!< 扩展摘要寄存器（可读写）0x310-0x32C
                           SHA-256等长摘要算法使用 */
} HASH_DIGEST_TypeDef;

/* 随机数发生器(RNG) -----------------------------------------------------*/
typedef struct 
{
  __IO uint32_t CR;        /*!< 控制寄存器（可读写）0x00
                           [2]     : RNG使能位 */
  
  __IO uint32_t SR;        /*!< 状态寄存器（只读）0x04
                           [0]     : 数据就绪标志
                           [5]     : 时钟错误标志 */
  
  __IO uint32_t DR;        /*!< 数据寄存器（只读）0x08
                           32位真随机数值输出 */
} RNG_TypeDef;

/* 存储区域基地址定义 ----------------------------------------------------*/
#define FLASH_BASE         ((uint32_t)0x08000000) /* 主Flash存储区基地址（最大1MB） */
#define CCMDATARAM_BASE    ((uint32_t)0x10000000) /* 核心耦合内存(CCM)基地址（64KB）*/
#define SRAM1_BASE         ((uint32_t)0x20000000) /* 主SRAM1存储区基地址（112KB） */
#define SRAM2_BASE         ((uint32_t)0x2001C000) /* SRAM2存储区基地址（16KB） */
#define SRAM3_BASE         ((uint32_t)0x20020000) /* SRAM3存储区基地址（64KB） */
#define PERIPH_BASE        ((uint32_t)0x40000000) /* 外设寄存器区基地址 */
#define BKPSRAM_BASE       ((uint32_t)0x40024000) /* 备份SRAM基地址（4KB） */

/* 存储器映射控制器基地址 */
#if defined (STM32F40_41xxx)
#define FSMC_R_BASE        ((uint32_t)0xA0000000) /* FSMC寄存器基地址 */
#endif

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define FMC_R_BASE         ((uint32_t)0xA0000000) /* FMC寄存器基地址 */
#endif

/* 位带别名区基地址 ----------------------------------------------------*/
#define CCMDATARAM_BB_BASE ((uint32_t)0x12000000) /* CCM内存位带别名地址 */
#define SRAM1_BB_BASE      ((uint32_t)0x22000000) /* SRAM1位带别名地址 */
#define SRAM2_BB_BASE      ((uint32_t)0x2201C000) /* SRAM2位带别名地址 */
#define SRAM3_BB_BASE      ((uint32_t)0x22400000) /* SRAM3位带别名地址 */
#define PERIPH_BB_BASE     ((uint32_t)0x42000000) /* 外设位带别名地址 */
#define BKPSRAM_BB_BASE    ((uint32_t)0x42024000) /* 备份SRAM位带别名地址 */
/**
  * @brief STM32F4外设总线地址映射定义（APB1总线）
  * @note  定义APB1总线上的外设基地址偏移量，包含：
  *        - 定时器(TIM2-TIM14)
  *        - 看门狗(WWDG/IWDG)
  *        - 串口通信(USART/UART)
  *        - 总线接口(I2C/CAN)
  *        - 电源控制(PWR)等
  * @warning 访问前需通过RCC使能对应外设时钟
  */

/* 外设总线基地址 ---------------------------------------------------------*/
#define APB1PERIPH_BASE       PERIPH_BASE        /* APB1总线基地址 0x40000000 */
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000) /* APB2总线基地址 */
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000) /* AHB1总线基地址 */
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x10000000) /* AHB2总线基地址 */

/* APB1总线外设地址偏移 --------------------------------------------------*/
/* 定时器组 */
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000)  /* 定时器2寄存器基地址 */
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400)  /* 定时器3寄存器基地址 */
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800)  /* 定时器4寄存器基地址 */
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00)  /* 定时器5寄存器基地址 */
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000)  /* 基础定时器6基地址 */
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400)  /* 基础定时器7基地址 */
#define TIM12_BASE            (APB1PERIPH_BASE + 0x1800)  /* 通用定时器12基地址 */
#define TIM13_BASE            (APB1PERIPH_BASE + 0x1C00)  /* 通用定时器13基地址 */
#define TIM14_BASE            (APB1PERIPH_BASE + 0x2000)  /* 通用定时器14基地址 */

/* 系统外设 */
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800)  /* 实时时钟(RTC)基地址 */
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00)  /* 窗口看门狗寄存器基地址 */
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000)  /* 独立看门狗寄存器基地址 */

/* 通信接口 */
#define I2S2ext_BASE          (APB1PERIPH_BASE + 0x3400)  /* I2S2扩展基地址 */
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800)  /* SPI2寄存器基地址 */
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00)  /* SPI3寄存器基地址 */
#define I2S3ext_BASE          (APB1PERIPH_BASE + 0x4000)  /* I2S3扩展基地址 */
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400)  /* USART2寄存器基地址 */
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800)  /* USART3寄存器基地址 */
#define UART4_BASE            (APB1PERIPH_BASE + 0x4C00)  /* UART4寄存器基地址 */
#define UART5_BASE            (APB1PERIPH_BASE + 0x5000)  /* UART5寄存器基地址 */

/* 总线控制器 */
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400)  /* I2C1寄存器基地址 */
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800)  /* I2C2寄存器基地址 */
#define I2C3_BASE             (APB1PERIPH_BASE + 0x5C00)  /* I2C3寄存器基地址 */
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400)  /* CAN1控制器基地址 */
#define CAN2_BASE             (APB1PERIPH_BASE + 0x6800)  /* CAN2控制器基地址 */

/* 系统控制 */
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000)  /* 电源控制(PWR)寄存器基地址 */
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400)  /* 数模转换器(DAC)基地址 */

/* 扩展串口 */
#define UART7_BASE            (APB1PERIPH_BASE + 0x7800)  /* UART7寄存器基地址 */
#define UART8_BASE            (APB1PERIPH_BASE + 0x7C00)  /* UART8寄存器基地址 */


/**
  * @brief STM32F4外设总线地址映射定义（APB2/AHB1总线）
  * @note  包含两个关键总线外设组：
  *        - APB2: 高速外设总线，承载高级定时器/ADC/SDIO等
  *        - AHB1: 主系统总线，连接GPIO/时钟控制器/存储器接口
  * @warning 外设访问前需通过RCC使能对应总线时钟
  */

/* ---------------------------- APB2总线外设 ---------------------------- */
/* 高级定时器 */
#define TIM1_BASE             (APB2PERIPH_BASE + 0x0000)  /* 高级定时器1寄存器基地址
                                                         支持PWM互补输出和死区控制 */
#define TIM8_BASE             (APB2PERIPH_BASE + 0x0400)  /* 高级定时器8寄存器基地址
                                                         电机控制专用 */

/* 通信接口 */
#define USART1_BASE           (APB2PERIPH_BASE + 0x1000)  /* USART1高速串口基地址
                                                         最大4.5Mbps */
#define USART6_BASE           (APB2PERIPH_BASE + 0x1400)  /* USART6基地址
                                                         支持全双工DMA传输 */

/* 模拟数字转换器 */
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2000)  /* ADC1寄存器基地址
                                                         12位精度，3Msps采样率 */
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2100)  /* ADC2寄存器基地址
                                                         可与ADC1组成双模式 */
#define ADC3_BASE             (APB2PERIPH_BASE + 0x2200)  /* ADC3寄存器基地址
                                                         独立工作模式 */
#define ADC_BASE              (APB2PERIPH_BASE + 0x2300)  /* 公共ADC控制寄存器基地址
                                                         管理注入通道和校准 */

/* 存储卡接口 */
#define SDIO_BASE             (APB2PERIPH_BASE + 0x2C00)  /* SDIO控制器基地址
                                                         支持SD卡规范4.0 */

/* 高速串行接口 */
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000)  /* SPI1寄存器基地址
                                                         主模式最高42MHz */
#define SPI4_BASE             (APB2PERIPH_BASE + 0x3400)  /* SPI4扩展接口基地址
                                                         支持I2S协议 */

/* 系统控制 */
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800)  /* 系统配置寄存器基地址
                                                         IO重映射和中断源选择 */
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00)  /* 外部中断控制器基地址
                                                         管理23个中断/事件线 */

/* 通用定时器 */
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4000)  /* 通用定时器9基地址
                                                         16位向上计数模式 */
#define TIM10_BASE            (APB2PERIPH_BASE + 0x4400)  /* 通用定时器10基地址
                                                         基本计时功能 */
#define TIM11_BASE            (APB2PERIPH_BASE + 0x4800)  /* 通用定时器11基地址
                                                         独立看门狗功能 */

/* 高速接口扩展 */
#define SPI5_BASE             (APB2PERIPH_BASE + 0x5000)  /* SPI5寄存器基地址
                                                         扩展的通信接口 */
#define SPI6_BASE             (APB2PERIPH_BASE + 0x5400)  /* SPI6寄存器基地址
                                                         支持全双工模式 */

/* 音频接口 */
#define SAI1_BASE             (APB2PERIPH_BASE + 0x5800)  /* SAI1音频控制器基地址
                                                         支持I2S/AC97协议 */
#define SAI1_Block_A_BASE     (SAI1_BASE + 0x004)         /* SAI1区块A配置寄存器
                                                         主数据传输通道 */
#define SAI1_Block_B_BASE     (SAI1_BASE + 0x024)         /* SAI1区块B配置寄存器
                                                         从模式同步通道 */

/* 显示控制器 */
#define LTDC_BASE             (APB2PERIPH_BASE + 0x6800)  /* 液晶控制器基地址
                                                         支持RGB888格式 */
#define LTDC_Layer1_BASE      (LTDC_BASE + 0x84)          /* 图层1配置寄存器
                                                         混合Alpha通道设置 */
#define LTDC_Layer2_BASE      (LTDC_BASE + 0x104)         /* 图层2配置寄存器
                                                         支持颜色键控 */

/* ---------------------------- AHB1总线外设 ---------------------------- */
/* GPIO端口 */
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)  /* GPIOA寄存器基地址
                                                         引脚0-15控制 */
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400)  /* GPIOB寄存器基地址 */
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800)  /* GPIOC寄存器基地址 */
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00)  /* GPIOD寄存器基地址 */
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000)  /* GPIOE寄存器基地址 */
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400)  /* GPIOF寄存器基地址 */
#define GPIOG_BASE            (AHB1PERIPH_BASE + 0x1800)  /* GPIOG寄存器基地址 */
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00)  /* GPIOH寄存器基地址 */
#define GPIOI_BASE            (AHB1PERIPH_BASE + 0x2000)  /* GPIOI寄存器基地址 */
#define GPIOJ_BASE            (AHB1PERIPH_BASE + 0x2400)  /* GPIOJ寄存器基地址 */
#define GPIOK_BASE            (AHB1PERIPH_BASE + 0x2800)  /* GPIOK寄存器基地址
                                                         (部分型号专属) */

/* 系统功能 */
#define CRC_BASE              (AHB1PERIPH_BASE + 0x3000)  /* CRC校验单元基地址
                                                         支持32位多项式计算 */
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800)  /* 时钟控制寄存器基地址
                                                         管理系统时钟树配置 */
#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00)  /* Flash接口寄存器基地址
                                                         控制编程擦除操作 */
/**
  * @brief STM32F4 DMA控制器、以太网、图形加速及存储器接口地址定义
  * @note  包含：
  *        - DMA1/DMA2各数据流寄存器地址
  *        - 以太网控制器完整寄存器映射
  *        - DMA2D图形加速器接口
  *        - AHB2总线高速外设
  *        - FSMC/FMC存储控制器Bank定义
  * @warning 操作DMA前需配置通道优先级和传输模式
  */

/* ---------------------------- DMA控制器 ---------------------------- */
/* DMA1控制器基地址 */
#define DMA1_BASE             (AHB1PERIPH_BASE + 0x6000)  /* DMA1主控制器基地址
                                                         8个数据流，各含8个通道 */

/* DMA1数据流寄存器组 */
#define DMA1_Stream0_BASE     (DMA1_BASE + 0x010)  /* 流0寄存器组（如ADC1数据传输） */
#define DMA1_Stream1_BASE     (DMA1_BASE + 0x028)  /* 流1寄存器组（SPI3_RX通道） */
#define DMA1_Stream2_BASE     (DMA1_BASE + 0x040)  /* 流2寄存器组（USART1_TX通道） */
#define DMA1_Stream3_BASE     (DMA1_BASE + 0x058)  /* 流3寄存器组（TIM2_CH3触发） */
#define DMA1_Stream4_BASE     (DMA1_BASE + 0x070)  /* 流4寄存器组（I2C1_RX通道） */
#define DMA1_Stream5_BASE     (DMA1_BASE + 0x088)  /* 流5寄存器组（SDIO数据传输） */
#define DMA1_Stream6_BASE     (DMA1_BASE + 0x0A0)  /* 流6寄存器组（TIM1_UP事件） */
#define DMA1_Stream7_BASE     (DMA1_BASE + 0x0B8)  /* 流7寄存器组（SPI2_TX通道） */

/* DMA2控制器基地址 */
#define DMA2_BASE             (AHB1PERIPH_BASE + 0x6400)  /* DMA2主控制器基地址
                                                         支持存储器到存储器传输 */

/* DMA2数据流寄存器组 */
#define DMA2_Stream0_BASE     (DMA2_BASE + 0x010)  /* 流0寄存器组（ADC3数据传输） */
#define DMA2_Stream1_BASE     (DMA2_BASE + 0x028)  /* 流1寄存器组（SPI4_RX通道） */
#define DMA2_Stream2_BASE     (DMA2_BASE + 0x040)  /* 流2寄存器组（USART6_TX） */
#define DMA2_Stream3_BASE     (DMA2_BASE + 0x058)  /* 流3寄存器组（TIM8_CH1触发） */
#define DMA2_Stream4_BASE     (DMA2_BASE + 0x070)  /* 流4寄存器组（I2C2_RX通道） */
#define DMA2_Stream5_BASE     (DMA2_BASE + 0x088)  /* 流5寄存器组（以太网DMA） */
#define DMA2_Stream6_BASE     (DMA2_BASE + 0x0A0)  /* 流6寄存器组（TIM4_UP事件） */
#define DMA2_Stream7_BASE     (DMA2_BASE + 0x0B8)  /* 流7寄存器组（SPI5_TX通道） */

/* ---------------------------- 以太网控制器 -------------------------- */
#define ETH_BASE              (AHB1PERIPH_BASE + 0x8000)  /* 以太网整体基地址 */
#define ETH_MAC_BASE          (ETH_BASE)                   /* MAC层寄存器组
                                                         数据帧过滤与处理 */
#define ETH_MMC_BASE          (ETH_BASE + 0x0100)          /* MAC管理计数器
                                                         统计网络流量数据 */
#define ETH_PTP_BASE          (ETH_BASE + 0x0700)          /* 精确时间协议寄存器
                                                         支持IEEE1588时间同步 */
#define ETH_DMA_BASE          (ETH_BASE + 0x1000)          /* 以太网DMA控制器
                                                         支持8个发送队列 */

/* ---------------------------- 图形加速器 ---------------------------- */
#define DMA2D_BASE            (AHB1PERIPH_BASE + 0xB000)  /* 2D图形DMA基地址
                                                         支持图层混合与格式转换 */

/* ---------------------------- AHB2总线外设 -------------------------- */
#define DCMI_BASE             (AHB2PERIPH_BASE + 0x50000)  /* 数字摄像头接口基地址
                                                         支持8位/10位YUV格式 */
#define CRYP_BASE             (AHB2PERIPH_BASE + 0x60000)  /* 加密处理器基地址
                                                         支持AES/DES算法 */
#define HASH_BASE             (AHB2PERIPH_BASE + 0x60400)  /* 哈希处理器基地址
                                                         支持SHA-1/MD5 */
#define HASH_DIGEST_BASE      (AHB2PERIPH_BASE + 0x60710)  /* 哈希摘要寄存器组
                                                         存储最终计算结果 */
#define RNG_BASE              (AHB2PERIPH_BASE + 0x60800)  /* 真随机数生成器
                                                         通过熵源生成随机数 */

/* ------------------------- 存储器控制器(FSMC) ----------------------- */
#if defined (STM32F40_41xxx)
/* FSMC存储区定义 */
#define FSMC_Bank1_R_BASE     (FSMC_R_BASE + 0x0000)  /* NOR/PSRAM Bank1寄存器
                                                         支持SRAM接口时序配置 */
#define FSMC_Bank1E_R_BASE    (FSMC_R_BASE + 0x0104)  /* NOR/PSRAM扩展模式
                                                         用于大容量存储设备 */
#define FSMC_Bank2_R_BASE     (FSMC_R_BASE + 0x0060)  /* NAND Bank2寄存器组
                                                         256字节页大小 */
#define FSMC_Bank3_R_BASE     (FSMC_R_BASE + 0x0080)  /* NAND Bank3寄存器组
                                                         支持ECC校验 */
#define FSMC_Bank4_R_BASE     (FSMC_R_BASE + 0x00A0)  /* PC Card模式寄存器
                                                         CFA卡兼容接口 */
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
/*!< FMC存储块寄存器基地址宏定义 */
/*-----------------------------------
  FMC控制器在AHB总线上的基地址:
  - STM32F42x/43x: 0xA0000000 
  - 各Bank寄存器通过基地址偏移进行访问
-----------------------------------*/

/* NOR/SRAM存储块1主控制寄存器基地址（支持8/16/32位总线）*/
#define FMC_Bank1_R_BASE      (FMC_R_BASE + 0x0000)  // +0x0000
/* NOR/SRAM存储块1扩展时序寄存器基地址（支持多个片选信号）*/
#define FMC_Bank1E_R_BASE     (FMC_R_BASE + 0x0104)  // +0x0104（控制4个片选信号）

/* NAND Flash存储块2控制寄存器基地址（支持2个片选）*/
#define FMC_Bank2_R_BASE      (FMC_R_BASE + 0x0060)  // +0x0060（对应NAND Flash控制器）

/* NAND Flash存储块3控制寄存器基地址 */
#define FMC_Bank3_R_BASE      (FMC_R_BASE + 0x0080)  // +0x0080（可连接第2个NAND设备）

/* PC Card（CF卡）存储块4控制寄存器基地址 */
#define FMC_Bank4_R_BASE      (FMC_R_BASE + 0x00A0)  // +0x00A0（支持CF卡/IDE接口）

/* SDRAM存储块5-6组合控制器基地址（支持LPDDR/LPDDR2）*/
#define FMC_Bank5_6_R_BASE    (FMC_R_BASE + 0x0140)  // +0x0140（共享时序配置）

#endif /* STM32F427_437xx ||  STM32F429_439xx */


/**
  * @brief STM32F4外设结构体指针定义
  * @note  将外设基地址转换为类型化指针，用于寄存器访问
  *        通过形如TIM2->CR1的语法直接操作硬件寄存器
  * @warning 操作前需确保外设时钟已使能
  */

/* 调试单元 */
#define DBGMCU_BASE           ((uint32_t )0xE0042000)  /* 调试MCU寄存器基地址 */

/**
  * @}
  */
 /*------------------ 定时器模块 -----------------*/
#define TIM2                ((TIM_TypeDef *) TIM2_BASE)  // 通用定时器2，32位递增计数器，支持编码器接口
#define TIM3                ((TIM_TypeDef *) TIM3_BASE)  // 通用定时器3，带4通道PWM输出能力
#define TIM4                ((TIM_TypeDef *) TIM4_BASE)  // 基本定时器4，常用于系统时基生成
#define TIM5                ((TIM_TypeDef *) TIM5_BASE)  // 通用定时器5，支持32位自动重载寄存器
#define TIM6                ((TIM_TypeDef *) TIM6_BASE)  // 基本定时器6，DAC触发专用
#define TIM7                ((TIM_TypeDef *) TIM7_BASE)  // 基本定时器7，支持主模式触发ADC
#define TIM12               ((TIM_TypeDef *) TIM12_BASE) // 通用定时器12，与TIM13/14共用APB1总线
#define TIM13               ((TIM_TypeDef *) TIM13_BASE) // 高级控制定时器13，支持死区插入
#define TIM14               ((TIM_TypeDef *) TIM14_BASE) // 通用定时器14，带独立看门狗功能

/*------------------ 系统模块 -------------------*/
#define RTC                 ((RTC_TypeDef *) RTC_BASE)   // 实时时钟，带独立供电域
#define WWDG                ((WWDG_TypeDef *) WWDG_BASE) // 窗口看门狗，要求定期刷新
#define IWDG                ((IWDG_TypeDef *) IWDG_BASE) // 独立看门狗，LSI时钟驱动

/*------------------ 通信接口 -------------------*/
#define I2S2ext             ((SPI_TypeDef *) I2S2ext_BASE) // I2S2扩展接口，支持全双工音频
#define SPI2                ((SPI_TypeDef *) SPI2_BASE)   // SPI2主接口，最大30MHz时钟
#define SPI3                ((SPI_TypeDef *) SPI3_BASE)   // SPI3接口，支持TI模式
#define I2S3ext             ((SPI_TypeDef *) I2S3ext_BASE) // I2S3扩展接口，可实现多设备级联
#define USART2              ((USART_TypeDef *) USART2_BASE) // USART2，支持ISO7816智能卡模式
#define USART3              ((USART_TypeDef *) USART3_BASE) // USART3，带硬件流控引脚
#define UART4               ((USART_TypeDef *) UART4_BASE)  // UART4，仅异步通信模式
#define UART5               ((USART_TypeDef *) UART5_BASE)  // UART5，支持IrDA红外模式
#define I2C1                ((I2C_TypeDef *) I2C1_BASE)   // I2C1，支持快速模式+(1MHz)
#define I2C2                ((I2C_TypeDef *) I2C2_BASE)   // I2C2，支持SMBus协议
#define I2C3                ((I2C_TypeDef *) I2C3_BASE)   // I2C3，带双地址匹配功能
/*------------------ 控制模块 -------------------*/
#define CAN1                ((CAN_TypeDef *) CAN1_BASE)   // CAN1控制器，支持2.0B主动模式
#define CAN2                ((CAN_TypeDef *) CAN2_BASE)   // CAN2控制器，与CAN1共享过滤器
#define PWR                 ((PWR_TypeDef *) PWR_BASE)    // 电源控制模块，管理低功耗模式

/*------------------ 模拟模块 -------------------*/
#define DAC                 ((DAC_TypeDef *) DAC_BASE)    // 双通道12位DAC，支持DMA
/*------------------ 高级通信接口 -------------------*/
#define UART7               ((USART_TypeDef *) UART7_BASE)  // UART7，最小封装型号可用，最高波特率5.25Mbps
#define UART8               ((USART_TypeDef *) UART8_BASE)  // UART8，支持LIN主模式，带自动波特率检测

/*------------------ 电机控制定时器 -----------------*/
#define TIM1                ((TIM_TypeDef *) TIM1_BASE)    // TIM1，带8路PWM输出，支持霍尔传感器接口
#define TIM8                ((TIM_TypeDef *) TIM8_BASE)    // TIM8，支持编码器接口和可编程死区控制

/*------------------ 高速串行接口 -------------------*/
#define USART1              ((USART_TypeDef *) USART1_BASE)  // USART1，APB2总线，支持IrDA 4Mbps速率
#define USART6              ((USART_TypeDef *) USART6_BASE)  // USART6，独立DMA通道，支持RS-485模式

/*------------------ 模拟数字转换模块 ----------------*/
#define ADC                 ((ADC_Common_TypeDef *) ADC_BASE)   // ADC共用寄存器组，管理校准和时钟分频
#define ADC1                ((ADC_TypeDef *) ADC1_BASE)      // ADC1，支持三路差分输入（Vrefint/温度传感器）
#define ADC2                ((ADC_TypeDef *) ADC2_BASE)      // ADC2，可同步采样组成双ADC模式
#define ADC3                ((ADC_TypeDef *) ADC3_BASE)      // ADC3，带硬件过采样（最高256倍）

/*------------------ 存储卡接口 ---------------------*/
#define SDIO                ((SDIO_TypeDef *) SDIO_BASE)     // SDIO，支持SDHC 2.0协议，最高48MHz CLK

/*------------------ 高速串行外设接口 -----------------*/
#define SPI1                ((SPI_TypeDef *) SPI1_BASE)     // SPI1，支持TISS模式，MOSI最大速率42MHz
#define SPI4                ((SPI_TypeDef *) SPI4_BASE)     // SPI4，Quad-SPI控制接口，可接Nor Flash
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)  // 系统配置控制器，管理IO重映射、以太网PHY接口
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)       // 外部中断控制器，支持35个可屏蔽中断线
#define TIM9                ((TIM_TypeDef *) TIM9_BASE)        // TIM9，16位通用定时器，与TIM12共用APB2时钟
#define TIM10               ((TIM_TypeDef *) TIM10_BASE)       // TIM10，支持从模式触发ADC
#define TIM11               ((TIM_TypeDef *) TIM11_BASE)       // TIM11，带独立DMA请求通道
#define SPI5                ((SPI_TypeDef *) SPI5_BASE)        // SPI5，独立收发FIFO（各16x32位）
#define SPI6                ((SPI_TypeDef *) SPI6_BASE)        // SPI6，支持DDR模式（双倍数据速率）
#define SAI1                ((SAI_TypeDef *) SAI1_BASE)        // SAI1，支持AC97协议，最高192kHz采样
#define SAI1_Block_A        ((SAI_Block_TypeDef *)SAI1_Block_A_BASE)  // Block A，主传输模式
#define SAI1_Block_B        ((SAI_Block_TypeDef *)SAI1_Block_B_BASE)  // Block B，同步从模式
#define LTDC                ((LTDC_TypeDef *)LTDC_BASE)        // LTDC，支持RGB888格式，1920x1080@60Hz
#define LTDC_Layer1         ((LTDC_Layer_TypeDef *)LTDC_Layer1_BASE) // 图层1，带α混合（0-255级）
#define LTDC_Layer2         ((LTDC_Layer_TypeDef *)LTDC_Layer2_BASE) // 图层2，支持色键透明度控制
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)      // GPIOA端口，最多16个引脚
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)      // GPIOB端口，最多16个引脚
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)      // GPIOC端口，最多16个引脚
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)      // GPIOD端口，最多16个引脚
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)      // GPIOE端口，最多16个引脚
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)      // GPIOF端口，最多16个引脚（仅在某些型号中可用）
#define GPIOG               ((GPIO_TypeDef *) GPIOG_BASE)      // GPIOG端口，最多16个引脚（仅在某些型号中可用）
#define GPIOH               ((GPIO_TypeDef *) GPIOH_BASE)      // GPIOH端口，最多16个引脚（仅在某些型号中可用）
#define GPIOI               ((GPIO_TypeDef *) GPIOI_BASE)      // GPIOI端口，最多16个引脚（仅在某些型号中可用）
#define GPIOJ               ((GPIO_TypeDef *) GPIOJ_BASE)      // GPIOJ端口，最多16个引脚（仅在某些型号中可用）
#define GPIOK               ((GPIO_TypeDef *) GPIOK_BASE)      // GPIOK端口，最多16个引脚（仅在某些型号中可用）
/* 外设寄存器结构体指针定义
 * 功能：将各外设的基地址转换为对应外设寄存器结构体类型的指针
 *       通过宏定义方式提供便捷的外设寄存器访问接口
 * 作用：简化外设寄存器操作，增强代码可读性，避免直接操作内存地址
 *       符合CMSIS规范，提供标准外设访问方式 */
#define CRC                 ((CRC_TypeDef *) CRC_BASE)         /* CRC循环冗余校验单元，用于数据校验计算 */
#define RCC                 ((RCC_TypeDef *) RCC_BASE)         /* 复位和时钟控制单元，管理系统时钟和各外设时钟 */
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)   /* Flash存储器控制器，管理闪存编程/擦除操作 */
#define DMA1                ((DMA_TypeDef *) DMA1_BASE)         /* DMA1控制器，管理直接内存访问通道 */
#define DMA1_Stream0        ((DMA_Stream_TypeDef *) DMA1_Stream0_BASE)  /* DMA1流0，用于外设到内存的高效数据传输 */
#define DMA1_Stream1        ((DMA_Stream_TypeDef *) DMA1_Stream1_BASE)  /* DMA1流1，支持多种外设如ADC、SPI等的DMA传输 */
#define DMA1_Stream2        ((DMA_Stream_TypeDef *) DMA1_Stream2_BASE)  /* DMA1流2，可用于UART、I2C等通信接口 */
#define DMA1_Stream3        ((DMA_Stream_TypeDef *) DMA1_Stream3_BASE)  /* DMA1流3，支持定时器、SDIO等传输 */
#define DMA1_Stream4        ((DMA_Stream_TypeDef *) DMA1_Stream4_BASE)  /* DMA1流4，用于内存到外设或内存到内存传输 */
#define DMA1_Stream5        ((DMA_Stream_TypeDef *) DMA1_Stream5_BASE)  /* DMA1流5，支持多通道数据传输 */
#define DMA1_Stream6        ((DMA_Stream_TypeDef *) DMA1_Stream6_BASE)  /* DMA1流6，可用于音频接口等高速传输 */
#define DMA1_Stream7        ((DMA_Stream_TypeDef *) DMA1_Stream7_BASE)  /* DMA1流7，支持突发传输和循环缓冲区 */
#define DMA2                ((DMA_TypeDef *) DMA2_BASE)         /* DMA2控制器，提供额外DMA通道资源 */
#define DMA2_Stream0        ((DMA_Stream_TypeDef *) DMA2_Stream0_BASE)  /* DMA2流0，通常用于高级外设如以太网 */
#define DMA2_Stream1        ((DMA_Stream_TypeDef *) DMA2_Stream1_BASE)  /* DMA2流1，支持Camera接口等 */
#define DMA2_Stream2        ((DMA_Stream_TypeDef *) DMA2_Stream2_BASE)  /* DMA2流2，用于加密哈希单元数据传输 */
#define DMA2_Stream3        ((DMA_Stream_TypeDef *) DMA2_Stream3_BASE)  /* DMA2流3，支持高带宽外设 */
#define DMA2_Stream4        ((DMA_Stream_TypeDef *) DMA2_Stream4_BASE)  /* DMA2流4，可用于内存加速操作 */
#define DMA2_Stream5        ((DMA_Stream_TypeDef *) DMA2_Stream5_BASE)  /* DMA2流5，支持多缓冲区传输 */
#define DMA2_Stream6        ((DMA_Stream_TypeDef *) DMA2_Stream6_BASE)  /* DMA2流6，用于图形加速外设 */
#define DMA2_Stream7        ((DMA_Stream_TypeDef *) DMA2_Stream7_BASE)  /* DMA2流7，支持链式传输模式 */
#define ETH                 ((ETH_TypeDef *) ETH_BASE)          /* 以太网控制器，实现MAC层功能 */  
#define DMA2D               ((DMA2D_TypeDef *)DMA2D_BASE)       /* 2D图形加速DMA，用于图像处理操作 */
#define DCMI                ((DCMI_TypeDef *) DCMI_BASE)        /* 数字摄像头接口，连接图像传感器 */
#define CRYP                ((CRYP_TypeDef *) CRYP_BASE)         /* 加密处理器，支持AES/DES/TDES算法 */
#define HASH                ((HASH_TypeDef *) HASH_BASE)         /* 哈希处理器，支持SHA-1/MD5等算法 */
#define HASH_DIGEST         ((HASH_DIGEST_TypeDef *) HASH_DIGEST_BASE) /* 哈希摘要寄存器，存储哈希计算结果 */
#define RNG                 ((RNG_TypeDef *) RNG_BASE)           /* 随机数生成器，提供硬件真随机数 */
/* 外设存储器控制器及调试单元定义
 * 功能：根据芯片型号定义不同的静态存储器控制器接口
 *       提供调试模式配置寄存器的访问接口
 * 作用：实现芯片型号兼容性支持，统一外部存储器访问方式
 *       支持调试功能的底层寄存器操作 */

#if defined (STM32F40_41xxx)
/* F40/41系列Flexible Static Memory Controller定义 */
#define FSMC_Bank1          ((FSMC_Bank1_TypeDef *) FSMC_Bank1_R_BASE)        /* NOR/SRAM控制器Bank1，支持PCCARD/CF卡 */
#define FSMC_Bank1E         ((FSMC_Bank1E_TypeDef *) FSMC_Bank1E_R_BASE)       /* 扩展的NOR/SRAM控制器，支持更大地址空间 */
#define FSMC_Bank2          ((FSMC_Bank2_TypeDef *) FSMC_Bank2_R_BASE)         /* NAND闪存/PC卡控制器Bank2 */
#define FSMC_Bank3          ((FSMC_Bank3_TypeDef *) FSMC_Bank3_R_BASE)         /* NAND闪存控制器Bank3 */
#define FSMC_Bank4          ((FSMC_Bank4_TypeDef *) FSMC_Bank4_R_BASE)         /* PC卡控制器Bank4 */
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
/* F427/429系列Flexible Memory Controller定义（增强型） */
#define FMC_Bank1           ((FMC_Bank1_TypeDef *) FMC_Bank1_R_BASE)           /* SDRAM/SRAM控制器Bank1 */
#define FMC_Bank1E          ((FMC_Bank1E_TypeDef *) FMC_Bank1E_R_BASE)          /* 扩展的SDRAM地址空间控制 */
#define FMC_Bank2           ((FMC_Bank2_TypeDef *) FMC_Bank2_R_BASE)            /* NAND闪存/PC卡控制器Bank2 */
#define FMC_Bank3           ((FMC_Bank3_TypeDef *) FMC_Bank3_R_BASE)            /* NAND闪存控制器Bank3 */
#define FMC_Bank4           ((FMC_Bank4_TypeDef *) FMC_Bank4_R_BASE)            /* PC卡控制器Bank4 */
#define FMC_Bank5_6         ((FMC_Bank5_6_TypeDef *) FMC_Bank5_6_R_BASE)        /* SDRAM控制器Bank5-6，支持双片选 */
#endif /* STM32F427_437xx ||  STM32F429_439xx */

/* 调试模式控制单元 */
#define DBGMCU              ((DBGMCU_TypeDef *) DBGMCU_BASE)                   /* 调试MCU单元，控制低功耗模式调试功能 */

/**
  * @}
  */

/** @addtogroup Exported_constants
  * @{
  */
  
  /** @addtogroup Peripheral_Registers_Bits_Definition
  * @{
  */
    
/******************************************************************************/
/*                         Peripheral Registers_Bits_Definition               */
/******************************************************************************/
/* ADC寄存器位定义模块
 * 功能：定义模数转换器(ADC)状态寄存器(ADC_SR)和控制寄存器1(ADC_CR1)的位字段
 * 作用：提供ADC工作状态监控和配置操作的位操作接口
 *       支持中断使能、转换模式选择、看门狗设置等核心功能配置 */

/********************  Bit definition for ADC_SR register  ********************/
#define  ADC_SR_AWD       ((uint8_t)0x01)  /* 模拟看门狗触发标志，当输入电压超出设定阈值时置1 */
#define  ADC_SR_EOC       ((uint8_t)0x02)  /* 规则通道转换完成标志，转换结束时硬件自动置1 */
#define  ADC_SR_JEOC      ((uint8_t)0x04)  /* 注入通道转换完成标志，注入组转换结束时置1 */
#define  ADC_SR_JSTRT     ((uint8_t)0x08)  /* 注入通道转换启动标志，开始转换时置1 */
#define  ADC_SR_STRT      ((uint8_t)0x10)  /* 规则通道转换启动标志，开始转换时置1 */
#define  ADC_SR_OVR       ((uint8_t)0x20)  /* 数据溢出标志，新转换完成时数据寄存器未读取时置1 */

/*******************  Bit definition for ADC_CR1 register  ********************/
/* 通道选择与看门狗配置 */
#define  ADC_CR1_AWDCH     ((uint32_t)0x0000001F)  /* [4:0]位，模拟看门狗监测通道选择(0-18) */
#define  ADC_CR1_AWDCH_0   ((uint32_t)0x00000001)  /* 看门狗通道选择位0 */
#define  ADC_CR1_AWDCH_1   ((uint32_t)0x00000002)  /* 看门狗通道选择位1 */
#define  ADC_CR1_AWDCH_2   ((uint32_t)0x00000004)  /* 看门狗通道选择位2 */
#define  ADC_CR1_AWDCH_3   ((uint32_t)0x00000008)  /* 看门狗通道选择位3 */
#define  ADC_CR1_AWDCH_4   ((uint32_t)0x00000010)  /* 看门狗通道选择位4 */

/* 中断使能控制 */
#define  ADC_CR1_EOCIE     ((uint32_t)0x00000020)  /* 规则通道转换完成中断使能 */
#define  ADC_CR1_AWDIE     ((uint32_t)0x00000040)  /* 模拟看门狗中断使能 */
#define  ADC_CR1_JEOCIE    ((uint32_t)0x00000080)  /* 注入通道转换完成中断使能 */

/* 工作模式配置 */
#define  ADC_CR1_SCAN      ((uint32_t)0x00000100)  /* 扫描模式使能，多通道自动转换 */
#define  ADC_CR1_AWDSGL    ((uint32_t)0x00000200)  /* 单通道看门狗模式，仅监测扫描序列中第一个通道 */
#define  ADC_CR1_JAUTO     ((uint32_t)0x00000400)  /* 自动注入转换，规则组转换后自动执行注入组 */

/* 间断模式配置 */
#define  ADC_CR1_DISCEN    ((uint32_t)0x00000800)  /* 规则通道间断模式使能 */
#define  ADC_CR1_JDISCEN   ((uint32_t)0x00001000)  /* 注入通道间断模式使能 */
#define  ADC_CR1_DISCNUM   ((uint32_t)0x0000E000)  /* [14:12]位，间断模式转换次数设置(1-8次) */
#define  ADC_CR1_DISCNUM_0 ((uint32_t)0x00002000)  /* 间断次数设置位0 */
#define  ADC_CR1_DISCNUM_1 ((uint32_t)0x00004000)  /* 间断次数设置位1 */
#define  ADC_CR1_DISCNUM_2 ((uint32_t)0x00008000)  /* 间断次数设置位2 */

/* 高级看门狗配置 */
#define  ADC_CR1_JAWDEN    ((uint32_t)0x00400000)  /* 注入通道看门狗使能 */
#define  ADC_CR1_AWDEN     ((uint32_t)0x00800000)  /* 规则通道看门狗使能 */

/* 转换分辨率配置 */
#define  ADC_CR1_RES       ((uint32_t)0x03000000)  /* [25:24]位，ADC分辨率选择：00=12位,01=10位,10=8位,11=6位 */
#define  ADC_CR1_RES_0     ((uint32_t)0x01000000)  /* 分辨率选择位0 */
#define  ADC_CR1_RES_1     ((uint32_t)0x02000000)  /* 分辨率选择位1 */
#define  ADC_CR1_OVRIE     ((uint32_t)0x04000000)  /* 溢出中断使能，数据覆盖时产生中断 */

/* ADC控制寄存器及采样时间配置模块
 * 功能：定义ADC_CR2工作模式控制位和ADC_SMPR1通道采样时间配置
 * 作用：管理ADC转换触发机制、数据获取方式及通道采样参数
 *       实现硬件触发/DMA集成，精确控制信号采集过程 */

/******************* ADC_CR2 控制寄存器位定义 ********************/
#define  ADC_CR2_ADON      ((uint32_t)0x00000001)  /* ADC电源控制，1=开启(需维持>4个时钟周期)，0=关闭 */
#define  ADC_CR2_CONT      ((uint32_t)0x00000002)  /* 连续转换使能，1=自动重启转换，0=单次转换 */
#define  ADC_CR2_DMA       ((uint32_t)0x00000100)  /* DMA传输使能，1=转换完成触发DMA请求（扫描模式必须启用） */
#define  ADC_CR2_DDS       ((uint32_t)0x00000200)  /* DMA持续模式，1=DMA持续接收数据，0=最后一次转换后停止 */
#define  ADC_CR2_EOCS      ((uint32_t)0x00000400)  /* EOC标志模式，0=每通道转换后置位，1=全部序列完成置位 */
#define  ADC_CR2_ALIGN     ((uint32_t)0x00000800)  /* 数据对齐方式，0=右对齐(12位有效)，1=左对齐(高8位有效) */

/* 注入组外部触发配置 */
#define  ADC_CR2_JEXTSEL    ((uint32_t)0x000F0000)  /* [19:16]注入触发源选择，0-15对应TIMx_TRG事件 */
#define  ADC_CR2_JEXTSEL_0  ((uint32_t)0x00010000)  /* 触发源编码位0 */
#define  ADC_CR2_JEXTSEL_1  ((uint32_t)0x00020000)  /* 触发源编码位1 */
#define  ADC_CR2_JEXTSEL_2  ((uint32_t)0x00040000)  /* 触发源编码位2 */
#define  ADC_CR2_JEXTSEL_3  ((uint32_t)0x00080000)  /* 触发源编码位3 */
#define  ADC_CR2_JEXTEN     ((uint32_t)0x00300000)  /* [21:20]触发检测方式，00=禁止 01=上升沿 10=下降沿 11=双沿 */
#define  ADC_CR2_JEXTEN_0   ((uint32_t)0x00100000)  /* 触发边沿选择位0 */
#define  ADC_CR2_JEXTEN_1   ((uint32_t)0x00200000)  /* 触发边沿选择位1 */
#define  ADC_CR2_JSWSTART   ((uint32_t)0x00400000)  /* 软件触发注入转换，写1启动（硬件自动清零） */

/* 规则组外部触发配置 */  
#define  ADC_CR2_EXTSEL     ((uint32_t)0x0F000000)  /* [27:24]规则触发源选择，0-15对应TIMx_TRG事件 */
#define  ADC_CR2_EXTSEL_0   ((uint32_t)0x01000000)  /* 触发源编码位0 */
#define  ADC_CR2_EXTSEL_1   ((uint32_t)0x02000000)  /* 触发源编码位1 */
#define  ADC_CR2_EXTSEL_2   ((uint32_t)0x04000000)  /* 触发源编码位2 */
#define  ADC_CR2_EXTSEL_3   ((uint32_t)0x08000000)  /* 触发源编码位3 */
#define  ADC_CR2_EXTEN      ((uint32_t)0x30000000)  /* [29:28]触发检测方式，配置同JEXTEN */
#define  ADC_CR2_EXTEN_0    ((uint32_t)0x10000000)  /* 触发边沿选择位0 */
#define  ADC_CR2_EXTEN_1    ((uint32_t)0x20000000)  /* 触发边沿选择位1 */
#define  ADC_CR2_SWSTART    ((uint32_t)0x40000000)  /* 软件触发规则转换，写1启动（硬件自动清零） */

/****************** ADC_SMPR1 采样时间寄存器位定义 *******************/
/* 通道10-18采样时钟周期配置（3位/通道，值范围0-7对应3/15/28/56/84/112/144/480个时钟周期） */
#define  ADC_SMPR1_SMP10   ((uint32_t)0x00000007)  /* 通道10采样时间，值越大信号稳定时间越长 */
#define  ADC_SMPR1_SMP10_0 ((uint32_t)0x00000001)  /* 采样周期选择位0 */
#define  ADC_SMPR1_SMP10_1 ((uint32_t)0x00000002)  /* 采样周期选择位1 */
#define  ADC_SMPR1_SMP10_2 ((uint32_t)0x00000004)  /* 采样周期选择位2 */

/* 通道11-18配置结构相同，以下为位域定义 */
#define  ADC_SMPR1_SMP11   ((uint32_t)0x00000038)  /* 通道11采样时间 */
#define  ADC_SMPR1_SMP11_0 ((uint32_t)0x00000008)  /* 通道11位0 */
#define  ADC_SMPR1_SMP11_1 ((uint32_t)0x00000010)  /* 通道11位1 */
#define  ADC_SMPR1_SMP11_2 ((uint32_t)0x00000020)  /* 通道11位2 */

#define  ADC_SMPR1_SMP12   ((uint32_t)0x000001C0)  /* 通道12采样时间 */
#define  ADC_SMPR1_SMP12_0 ((uint32_t)0x00000040)  /* 通道12位0 */
#define  ADC_SMPR1_SMP12_1 ((uint32_t)0x00000080)  /* 通道12位1 */
#define  ADC_SMPR1_SMP12_2 ((uint32_t)0x00000100)  /* 通道12位2 */

/****************** ADC_SMPR1 通道13-18采样时间配置 *******************/
/* 注：每个通道使用3个位定义采样周期，值范围0-7对应3/15/28/56/84/112/144/480个ADC时钟周期 */

/* 通道13配置 -------------------------------------------------*/
#define  ADC_SMPR1_SMP13    ((uint32_t)0x00000E00)  /* 通道13采样时间，建议根据信号源阻抗设置 */
#define  ADC_SMPR1_SMP13_0  ((uint32_t)0x00000200)  /* 采样周期选择位0 (LSB) */
#define  ADC_SMPR1_SMP13_1  ((uint32_t)0x00000400)  /* 采样周期选择位1 */
#define  ADC_SMPR1_SMP13_2  ((uint32_t)0x00000800)  /* 采样周期选择位2 (MSB) */

/* 通道14配置 -------------------------------------------------*/  
#define  ADC_SMPR1_SMP14    ((uint32_t)0x00007000)  /* 通道14采样时间，高阻抗信号建议480周期 */
#define  ADC_SMPR1_SMP14_0  ((uint32_t)0x00001000)  /* 位0 */
#define  ADC_SMPR1_SMP14_1  ((uint32_t)0x00002000)  /* 位1 */
#define  ADC_SMPR1_SMP14_2  ((uint32_t)0x00004000)  /* 位2 */

/* 通道15配置 -------------------------------------------------*/
#define  ADC_SMPR1_SMP15    ((uint32_t)0x00038000)  /* 通道15采样时间，高速信号可设3周期 */
#define  ADC_SMPR1_SMP15_0  ((uint32_t)0x00008000)  /* 位0 */
#define  ADC_SMPR1_SMP15_1  ((uint32_t)0x00010000)  /* 位1 */
#define  ADC_SMPR1_SMP15_2  ((uint32_t)0x00020000)  /* 位2 */

/* 通道16配置 -------------------------------------------------*/
#define  ADC_SMPR1_SMP16    ((uint32_t)0x001C0000)  /* 通道16采样时间，温度传感器建议480周期 */
#define  ADC_SMPR1_SMP16_0  ((uint32_t)0x00040000)  /* 位0 */
#define  ADC_SMPR1_SMP16_1  ((uint32_t)0x00080000)  /* 位1 */
#define  ADC_SMPR1_SMP16_2  ((uint32_t)0x00100000)  /* 位2 */

/* 通道17配置 -------------------------------------------------*/
#define  ADC_SMPR1_SMP17    ((uint32_t)0x00E00000)  /* 通道17采样时间，电池监测建议最长周期 */
#define  ADC_SMPR1_SMP17_0  ((uint32_t)0x00200000)  /* 位0 */
#define  ADC_SMPR1_SMP17_1  ((uint32_t)0x00400000)  /* 位1 */
#define  ADC_SMPR1_SMP17_2  ((uint32_t)0x00800000)  /* 位2 */

/* 通道18配置 -------------------------------------------------*/
#define  ADC_SMPR1_SMP18    ((uint32_t)0x07000000)  /* 通道18采样时间，VREFINT需最长稳定时间 */
#define  ADC_SMPR1_SMP18_0  ((uint32_t)0x01000000)  /* 位0 */
#define  ADC_SMPR1_SMP18_1  ((uint32_t)0x02000000)  /* 位1 */
#define  ADC_SMPR1_SMP18_2  ((uint32_t)0x04000000)  /* 位2 */
/******************  Bit definition for ADC_SMPR2 register  *******************/
/*
[功能说明]
ADC采样时间寄存器2（SMPR2）位定义，用于配置ADC通道0-9的采样时间
每个通道占用3个位，支持8种采样周期选择（000=3周期 ~ 111=480周期）
采样时间越长，转换精度越高，但转换速率会降低，建议根据信号源阻抗调整
寄存器位分配如下：
| 通道9 | 通道8 | 通道7 | 通道6 | 通道5 | 通道4 | 通道3 | 通道2 | 通道1 | 通道0 |
每个通道占3bit，从低位到高位依次排列
*/

/* 通道0采样时间选择位（位[2:0]）*/
#define  ADC_SMPR2_SMP0                      ((uint32_t)0x00000007)  /* 通道0的3位掩码（位0-2） */
#define  ADC_SMPR2_SMP0_0                    ((uint32_t)0x00000001)  /* 位0偏移量（LSB） */
#define  ADC_SMPR2_SMP0_1                    ((uint32_t)0x00000002)  /* 位1偏移量       */
#define  ADC_SMPR2_SMP0_2                    ((uint32_t)0x00000004)  /* 位2偏移量（MSB） */

/* 通道1采样时间选择位（位[5:3]）*/
#define  ADC_SMPR2_SMP1                      ((uint32_t)0x00000038)  /* 通道1的3位掩码（位3-5） */
#define  ADC_SMPR2_SMP1_0                    ((uint32_t)0x00000008)  /* 位3偏移量（LSB） */
#define  ADC_SMPR2_SMP1_1                    ((uint32_t)0x00000010)  /* 位4偏移量       */
#define  ADC_SMPR2_SMP1_2                    ((uint32_t)0x00000020)  /* 位5偏移量（MSB） */

/* 通道2采样时间选择位（位[8:6]）*/
#define  ADC_SMPR2_SMP2                      ((uint32_t)0x000001C0)  /* 通道2的3位掩码（位6-8） */
#define  ADC_SMPR2_SMP2_0                    ((uint32_t)0x00000040)  /* 位6偏移量（LSB） */
#define  ADC_SMPR2_SMP2_1                    ((uint32_t)0x00000080)  /* 位7偏移量       */
#define  ADC_SMPR2_SMP2_2                    ((uint32_t)0x00000100)  /* 位8偏移量（MSB） */

/* 通道3采样时间选择位（位[11:9]）*/
#define  ADC_SMPR2_SMP3                      ((uint32_t)0x00000E00)  /* 通道3的3位掩码（位9-11） */
#define  ADC_SMPR2_SMP3_0                    ((uint32_t)0x00000200)  /* 位9偏移量（LSB）  */
#define  ADC_SMPR2_SMP3_1                    ((uint32_t)0x00000400)  /* 位10偏移量        */
#define  ADC_SMPR2_SMP3_2                    ((uint32_t)0x00000800)  /* 位11偏移量（MSB） */

/* 通道4采样时间选择位（位[14:12]）*/
#define  ADC_SMPR2_SMP4                      ((uint32_t)0x00007000)  /* 通道4的3位掩码（位12-14） */
#define  ADC_SMPR2_SMP4_0                    ((uint32_t)0x00001000)  /* 位12偏移量（LSB） */
#define  ADC_SMPR2_SMP4_1                    ((uint32_t)0x00002000)  /* 位13偏移量        */
#define  ADC_SMPR2_SMP4_2                    ((uint32_t)0x00004000)  /* 位14偏移量（MSB） */

/* 通道5采样时间选择位（位[17:15]）*/
#define  ADC_SMPR2_SMP5                      ((uint32_t)0x00038000)  /* 通道5的3位掩码（位15-17） */
#define  ADC_SMPR2_SMP5_0                    ((uint32_t)0x00008000)  /* 位15偏移量（LSB） */
#define  ADC_SMPR2_SMP5_1                    ((uint32_t)0x00010000)  /* 位16偏移量        */
#define  ADC_SMPR2_SMP5_2                    ((uint32_t)0x00020000)  /* 位17偏移量（MSB） */

/* 通道6采样时间选择位（位[20:18]）*/
#define  ADC_SMPR2_SMP6                      ((uint32_t)0x001C0000)  /* 通道6的3位掩码（位18-20） */
#define  ADC_SMPR2_SMP6_0                    ((uint32_t)0x00040000)  /* 位18偏移量（LSB） */
#define  ADC_SMPR2_SMP6_1                    ((uint32_t)0x00080000)  /* 位19偏移量        */
#define  ADC_SMPR2_SMP6_2                    ((uint32_t)0x00100000)  /* 位20偏移量（MSB） */

/* 通道7采样时间选择位（位[23:21]）*/
#define  ADC_SMPR2_SMP7                      ((uint32_t)0x00E00000)  /* 通道7的3位掩码（位21-23） */
#define  ADC_SMPR2_SMP7_0                    ((uint32_t)0x00200000)  /* 位21偏移量（LSB） */
#define  ADC_SMPR2_SMP7_1                    ((uint32_t)0x00400000)  /* 位22偏移量        */
#define  ADC_SMPR2_SMP7_2                    ((uint32_t)0x00800000)  /* 位23偏移量（MSB） */

/* 通道8采样时间选择位（位[26:24]）*/
#define  ADC_SMPR2_SMP8                      ((uint32_t)0x07000000)  /* 通道8的3位掩码（位24-26） */
#define  ADC_SMPR2_SMP8_0                    ((uint32_t)0x01000000)  /* 位24偏移量（LSB） */
#define  ADC_SMPR2_SMP8_1                    ((uint32_t)0x02000000)  /* 位25偏移量        */
#define  ADC_SMPR2_SMP8_2                    ((uint32_t)0x04000000)  /* 位26偏移量（MSB） */

/* 通道9采样时间选择位（位[29:27]）*/
#define  ADC_SMPR2_SMP9                      ((uint32_t)0x38000000)  /* 通道9的3位掩码（位27-29） */
#define  ADC_SMPR2_SMP9_0                    ((uint32_t)0x08000000)  /* 位27偏移量（LSB） */
#define  ADC_SMPR2_SMP9_1                    ((uint32_t)0x10000000)  /* 位28偏移量        */
#define  ADC_SMPR2_SMP9_2                    ((uint32_t)0x20000000)  /* 位29偏移量（MSB） */
/******************  Bit definition for ADC_JOFR1 register  *******************/
/*
[功能说明]
注入数据偏移寄存器组配置，包含以下寄存器定义：
1. JOFR1-JOFR4：注入通道1-4的数据偏移值（12位有效，范围0-4095）
2. HTR/LTR：模拟看门狗高低阈值寄存器（12位有效，需匹配ADC分辨率）
3. SQR1：规则序列寄存器，配置转换序列位置13-16及总转换次数
所有配置值均需在ADC禁用状态下写入
*/

/*---------------------------- 注入通道偏移配置 ----------------------------*/
#define  ADC_JOFR1_JOFFSET1  ((uint16_t)0x0FFF)  /* 注入通道1偏移寄存器（位[11:0]）偏移量范围：0-4095 */
#define  ADC_JOFR2_JOFFSET2  ((uint16_t)0x0FFF)  /* 注入通道2偏移寄存器（位[11:0]）用于校准补偿 */
#define  ADC_JOFR3_JOFFSET3  ((uint16_t)0x0FFF)  /* 注入通道3偏移寄存器（位[11:0]）实际值=原始值+偏移 */
#define  ADC_JOFR4_JOFFSET4  ((uint16_t)0x0FFF)  /* 注入通道4偏移寄存器（位[11:0]）注意符号扩展 */

/*-------------------------- 模拟看门狗阈值配置 ---------------------------*/
#define  ADC_HTR_HT          ((uint16_t)0x0FFF)  /* 高阈值寄存器（位[11:0]）当转换值超过触发中断 */
#define  ADC_LTR_LT          ((uint16_t)0x0FFF)  /* 低阈值寄存器（位[11:0]）阈值范围需小于等于4095 */

/*******************  Bit definition for ADC_SQR1 register  *******************/
/*
[规则序列配置说明]
配置规则转换序列的第13-16个转换通道及总转换次数：
- SQ13-SQ16：分别对应序列位置13-16的通道编号（0-18）
- L[3:0]：总转换次数（实际值=L[3:0]+1，范围1-16）
每个通道选择占用5bit（32种可能，实际通道号0-18有效）
*/
/* 规则序列第13个转换通道选择（位[4:0]） */
#define  ADC_SQR1_SQ13       ((uint32_t)0x0000001F)  /* 通道编号掩码（位0-4） */
#define  ADC_SQR1_SQ13_0     ((uint32_t)0x00000001)  /* 位0偏移 */
#define  ADC_SQR1_SQ13_1     ((uint32_t)0x00000002)  /* 位1偏移 */
#define  ADC_SQR1_SQ13_2     ((uint32_t)0x00000004)  /* 位2偏移 */
#define  ADC_SQR1_SQ13_3     ((uint32_t)0x00000008)  /* 位3偏移 */
#define  ADC_SQR1_SQ13_4     ((uint32_t)0x00000010)  /* 位4偏移（MSB） */

/* 规则序列第14个转换通道选择（位[9:5]） */
#define  ADC_SQR1_SQ14       ((uint32_t)0x000003E0)  /* 通道编号掩码（位5-9） */
#define  ADC_SQR1_SQ14_0     ((uint32_t)0x00000020)  /* 位5偏移（实际位号=5+0） */
#define  ADC_SQR1_SQ14_1     ((uint32_t)0x00000040)  /* 位6偏移 */
#define  ADC_SQR1_SQ14_2     ((uint32_t)0x00000080)  /* 位7偏移 */
#define  ADC_SQR1_SQ14_3     ((uint32_t)0x00000100)  /* 位8偏移 */
#define  ADC_SQR1_SQ14_4     ((uint32_t)0x00000200)  /* 位9偏移（MSB） */

/* 规则序列第15个转换通道选择（位[14:10]） */
#define  ADC_SQR1_SQ15       ((uint32_t)0x00007C00)  /* 通道编号掩码（位10-14） */
#define  ADC_SQR1_SQ15_0     ((uint32_t)0x00000400)  /* 位10偏移 */
#define  ADC_SQR1_SQ15_1     ((uint32_t)0x00000800)  /* 位11偏移 */
#define  ADC_SQR1_SQ15_2     ((uint32_t)0x00001000)  /* 位12偏移 */
#define  ADC_SQR1_SQ15_3     ((uint32_t)0x00002000)  /* 位13偏移 */
#define  ADC_SQR1_SQ15_4     ((uint32_t)0x00004000)  /* 位14偏移（MSB） */

/* 规则序列第16个转换通道选择（位[19:15]） */
#define  ADC_SQR1_SQ16       ((uint32_t)0x000F8000)  /* 通道编号掩码（位15-19） */
#define  ADC_SQR1_SQ16_0     ((uint32_t)0x00008000)  /* 位15偏移 */
#define  ADC_SQR1_SQ16_1     ((uint32_t)0x00010000)  /* 位16偏移 */
#define  ADC_SQR1_SQ16_2     ((uint32_t)0x00020000)  /* 位17偏移 */
#define  ADC_SQR1_SQ16_3     ((uint32_t)0x00040000)  /* 位18偏移 */
#define  ADC_SQR1_SQ16_4     ((uint32_t)0x00080000)  /* 位19偏移（MSB） */

/* 规则通道总转换次数配置（位[23:20]） */
#define  ADC_SQR1_L          ((uint32_t)0x00F00000)  /* 转换次数掩码（值=实际次数-1） */
#define  ADC_SQR1_L_0        ((uint32_t)0x00100000)  /* 位20偏移（LSB） */
#define  ADC_SQR1_L_1        ((uint32_t)0x00200000)  /* 位21偏移 */
#define  ADC_SQR1_L_2        ((uint32_t)0x00400000)  /* 位22偏移 */
#define  ADC_SQR1_L_3        ((uint32_t)0x00800000)  /* 位23偏移（MSB） */

/*******************  Bit definition for ADC_SQR2 register  *******************/
/*
[功能说明]
规则序列寄存器SQR2配置，用于设置ADC规则转换序列中第7-12个转换的通道顺序
每个转换位置占用5bit（32种可能，有效通道号0-18），具体位分配：
| SQ12 | SQ11 | SQ10 | SQ9 | SQ8 | SQ7 |
寄存器需配合SQR1、SQR3共同使用，构建完整1-16转换序列
*/

/* 规则序列第7个转换通道选择（位[4:0]） */
#define  ADC_SQR2_SQ7        ((uint32_t)0x0000001F)  /* 通道编号掩码（位0-4） */
#define  ADC_SQR2_SQ7_0      ((uint32_t)0x00000001)  /* 位0偏移（LSB） */
#define  ADC_SQR2_SQ7_1      ((uint32_t)0x00000002)  /* 位1偏移 */
#define  ADC_SQR2_SQ7_2      ((uint32_t)0x00000004)  /* 位2偏移 */
#define  ADC_SQR2_SQ7_3      ((uint32_t)0x00000008)  /* 位3偏移 */
#define  ADC_SQR2_SQ7_4      ((uint32_t)0x00000010)  /* 位4偏移（MSB） */

/* 规则序列第8个转换通道选择（位[9:5]） */
#define  ADC_SQR2_SQ8        ((uint32_t)0x000003E0)  /* 通道编号掩码（位5-9） */
#define  ADC_SQR2_SQ8_0      ((uint32_t)0x00000020)  /* 位5偏移 */
#define  ADC_SQR2_SQ8_1      ((uint32_t)0x00000040)  /* 位6偏移 */
#define  ADC_SQR2_SQ8_2      ((uint32_t)0x00000080)  /* 位7偏移 */
#define  ADC_SQR2_SQ8_3      ((uint32_t)0x00000100)  /* 位8偏移 */
#define  ADC_SQR2_SQ8_4      ((uint32_t)0x00000200)  /* 位9偏移（MSB） */

/* 规则序列第9个转换通道选择（位[14:10]） */
#define  ADC_SQR2_SQ9        ((uint32_t)0x00007C00)  /* 通道编号掩码（位10-14） */
#define  ADC_SQR2_SQ9_0      ((uint32_t)0x00000400)  /* 位10偏移 */
#define  ADC_SQR2_SQ9_1      ((uint32_t)0x00000800)  /* 位11偏移 */
#define  ADC_SQR2_SQ9_2      ((uint32_t)0x00001000)  /* 位12偏移 */
#define  ADC_SQR2_SQ9_3      ((uint32_t)0x00002000)  /* 位13偏移 */
#define  ADC_SQR2_SQ9_4      ((uint32_t)0x00004000)  /* 位14偏移（MSB） */

/* 规则序列第10个转换通道选择（位[19:15]） */
#define  ADC_SQR2_SQ10       ((uint32_t)0x000F8000)  /* 通道编号掩码（位15-19） */
#define  ADC_SQR2_SQ10_0     ((uint32_t)0x00008000)  /* 位15偏移 */
#define  ADC_SQR2_SQ10_1     ((uint32_t)0x00010000)  /* 位16偏移 */
#define  ADC_SQR2_SQ10_2     ((uint32_t)0x00020000)  /* 位17偏移 */
#define  ADC_SQR2_SQ10_3     ((uint32_t)0x00040000)  /* 位18偏移 */
#define  ADC_SQR2_SQ10_4     ((uint32_t)0x00080000)  /* 位19偏移（MSB） */

/* 规则序列第11个转换通道选择（位[24:20]） */
#define  ADC_SQR2_SQ11       ((uint32_t)0x01F00000)  /* 通道编号掩码（位20-24） */
#define  ADC_SQR2_SQ11_0     ((uint32_t)0x00100000)  /* 位20偏移 */
#define  ADC_SQR2_SQ11_1     ((uint32_t)0x00200000)  /* 位21偏移 */
#define  ADC_SQR2_SQ11_2     ((uint32_t)0x00400000)  /* 位22偏移 */
#define  ADC_SQR2_SQ11_3     ((uint32_t)0x00800000)  /* 位23偏移 */
#define  ADC_SQR2_SQ11_4     ((uint32_t)0x01000000)  /* 位24偏移（MSB） */

/* 规则序列第12个转换通道选择（位[29:25]） */
#define  ADC_SQR2_SQ12       ((uint32_t)0x3E000000)  /* 通道编号掩码（位25-29） */
#define  ADC_SQR2_SQ12_0     ((uint32_t)0x02000000)  /* 位25偏移 */
#define  ADC_SQR2_SQ12_1     ((uint32_t)0x04000000)  /* 位26偏移 */
#define  ADC_SQR2_SQ12_2     ((uint32_t)0x08000000)  /* 位27偏移 */
#define  ADC_SQR2_SQ12_3     ((uint32_t)0x10000000)  /* 位28偏移 */
#define  ADC_SQR2_SQ12_4     ((uint32_t)0x20000000)  /* 位29偏移（MSB） */

/*******************  Bit definition for ADC_SQR3 register  *******************/
/*
[功能说明]
规则序列寄存器SQR3配置，用于设置ADC规则转换序列中第1-6个转换的通道顺序
每个转换位置占用5bit（32种可能，有效通道号0-18），具体位分配：
| SQ6 | SQ5 | SQ4 | SQ3 | SQ2 | SQ1 |
作为规则序列的起始配置，与SQR1、SQR2共同构建完整转换序列
*/

/* 规则序列第1个转换通道选择（位[4:0]） */
#define  ADC_SQR3_SQ1        ((uint32_t)0x0000001F)  /* 通道编号掩码（位0-4） */
#define  ADC_SQR3_SQ1_0      ((uint32_t)0x00000001)  /* 位0偏移（LSB） */
#define  ADC_SQR3_SQ1_1      ((uint32_t)0x00000002)  /* 位1偏移 */
#define  ADC_SQR3_SQ1_2      ((uint32_t)0x00000004)  /* 位2偏移 */
#define  ADC_SQR3_SQ1_3      ((uint32_t)0x00000008)  /* 位3偏移 */
#define  ADC_SQR3_SQ1_4      ((uint32_t)0x00000010)  /* 位4偏移（MSB） */

/* 规则序列第2个转换通道选择（位[9:5]） */
#define  ADC_SQR3_SQ2        ((uint32_t)0x000003E0)  /* 通道编号掩码（位5-9） */
#define  ADC_SQR3_SQ2_0      ((uint32_t)0x00000020)  /* 位5偏移 */
#define  ADC_SQR3_SQ2_1      ((uint32_t)0x00000040)  /* 位6偏移 */
#define  ADC_SQR3_SQ2_2      ((uint32_t)0x00000080)  /* 位7偏移 */
#define  ADC_SQR3_SQ2_3      ((uint32_t)0x00000100)  /* 位8偏移 */
#define  ADC_SQR3_SQ2_4      ((uint32_t)0x00000200)  /* 位9偏移（MSB） */

/* 规则序列第3个转换通道选择（位[14:10]） */
#define  ADC_SQR3_SQ3        ((uint32_t)0x00007C00)  /* 通道编号掩码（位10-14） */
#define  ADC_SQR3_SQ3_0      ((uint32_t)0x00000400)  /* 位10偏移 */
#define  ADC_SQR3_SQ3_1      ((uint32_t)0x00000800)  /* 位11偏移 */
#define  ADC_SQR3_SQ3_2      ((uint32_t)0x00001000)  /* 位12偏移 */
#define  ADC_SQR3_SQ3_3      ((uint32_t)0x00002000)  /* 位13偏移 */
#define  ADC_SQR3_SQ3_4      ((uint32_t)0x00004000)  /* 位14偏移（MSB） */

/* 规则序列第4个转换通道选择（位[19:15]） */
#define  ADC_SQR3_SQ4        ((uint32_t)0x000F8000)  /* 通道编号掩码（位15-19） */
#define  ADC_SQR3_SQ4_0      ((uint32_t)0x00008000)  /* 位15偏移 */
#define  ADC_SQR3_SQ4_1      ((uint32_t)0x00010000)  /* 位16偏移 */
#define  ADC_SQR3_SQ4_2      ((uint32_t)0x00020000)  /* 位17偏移 */
#define  ADC_SQR3_SQ4_3      ((uint32_t)0x00040000)  /* 位18偏移 */
#define  ADC_SQR3_SQ4_4      ((uint32_t)0x00080000)  /* 位19偏移（MSB） */

/* 规则序列第5个转换通道选择（位[24:20]） */
#define  ADC_SQR3_SQ5        ((uint32_t)0x01F00000)  /* 通道编号掩码（位20-24） */
#define  ADC_SQR3_SQ5_0      ((uint32_t)0x00100000)  /* 位20偏移 */
#define  ADC_SQR3_SQ5_1      ((uint32_t)0x00200000)  /* 位21偏移 */
#define  ADC_SQR3_SQ5_2      ((uint32_t)0x00400000)  /* 位22偏移 */
#define  ADC_SQR3_SQ5_3      ((uint32_t)0x00800000)  /* 位23偏移 */
#define  ADC_SQR3_SQ5_4      ((uint32_t)0x01000000)  /* 位24偏移（MSB） */

/* 规则序列第6个转换通道选择（位[29:25]） */
#define  ADC_SQR3_SQ6        ((uint32_t)0x3E000000)  /* 通道编号掩码（位25-29） */
#define  ADC_SQR3_SQ6_0      ((uint32_t)0x02000000)  /* 位25偏移 */
#define  ADC_SQR3_SQ6_1      ((uint32_t)0x04000000)  /* 位26偏移 */
#define  ADC_SQR3_SQ6_2      ((uint32_t)0x08000000)  /* 位27偏移 */
#define  ADC_SQR3_SQ6_3      ((uint32_t)0x10000000)  /* 位28偏移 */
#define  ADC_SQR3_SQ6_4      ((uint32_t)0x20000000)  /* 位29偏移（MSB） */
/*******************  Bit definition for ADC_JSQR register  *******************/
/*
[功能说明]
注入序列寄存器JSQR配置，用于设置ADC注入转换的通道顺序及序列长度
主要功能：
1. JSQ1-JSQ4：配置注入序列1-4个转换通道（每个通道选择占用5bit）
2. JL[1:0]：设置注入转换总次数（00=1次，01=2次，10=3次，11=4次）
注入转换可中断规则转换，适用于紧急或高优先级信号采集
*/

/* 注入序列第1个转换通道选择（位[4:0]） */
#define  ADC_JSQR_JSQ1       ((uint32_t)0x0000001F)  /* 通道编号掩码（位0-4） */
#define  ADC_JSQR_JSQ1_0     ((uint32_t)0x00000001)  /* 位0偏移（LSB） */
#define  ADC_JSQR_JSQ1_1     ((uint32_t)0x00000002)  /* 位1偏移 */
#define  ADC_JSQR_JSQ1_2     ((uint32_t)0x00000004)  /* 位2偏移 */
#define  ADC_JSQR_JSQ1_3     ((uint32_t)0x00000008)  /* 位3偏移 */
#define  ADC_JSQR_JSQ1_4     ((uint32_t)0x00000010)  /* 位4偏移（MSB） */

/* 注入序列第2个转换通道选择（位[9:5]） */
#define  ADC_JSQR_JSQ2       ((uint32_t)0x000003E0)  /* 通道编号掩码（位5-9） */
#define  ADC_JSQR_JSQ2_0     ((uint32_t)0x00000020)  /* 位5偏移 */
#define  ADC_JSQR_JSQ2_1     ((uint32_t)0x00000040)  /* 位6偏移 */
#define  ADC_JSQR_JSQ2_2     ((uint32_t)0x00000080)  /* 位7偏移 */
#define  ADC_JSQR_JSQ2_3     ((uint32_t)0x00000100)  /* 位8偏移 */
#define  ADC_JSQR_JSQ2_4     ((uint32_t)0x00000200)  /* 位9偏移（MSB） */

/* 注入序列第3个转换通道选择（位[14:10]） */
#define  ADC_JSQR_JSQ3       ((uint32_t)0x00007C00)  /* 通道编号掩码（位10-14） */
#define  ADC_JSQR_JSQ3_0     ((uint32_t)0x00000400)  /* 位10偏移 */
#define  ADC_JSQR_JSQ3_1     ((uint32_t)0x00000800)  /* 位11偏移 */
#define  ADC_JSQR_JSQ3_2     ((uint32_t)0x00001000)  /* 位12偏移 */
#define  ADC_JSQR_JSQ3_3     ((uint32_t)0x00002000)  /* 位13偏移 */
#define  ADC_JSQR_JSQ3_4     ((uint32_t)0x00004000)  /* 位14偏移（MSB） */

/* 注入序列第4个转换通道选择（位[19:15]） */
#define  ADC_JSQR_JSQ4       ((uint32_t)0x000F8000)  /* 通道编号掩码（位15-19） */
#define  ADC_JSQR_JSQ4_0     ((uint32_t)0x00008000)  /* 位15偏移 */
#define  ADC_JSQR_JSQ4_1     ((uint32_t)0x00010000)  /* 位16偏移 */
#define  ADC_JSQR_JSQ4_2     ((uint32_t)0x00020000)  /* 位17偏移 */
#define  ADC_JSQR_JSQ4_3     ((uint32_t)0x00040000)  /* 位18偏移 */
#define  ADC_JSQR_JSQ4_4     ((uint32_t)0x00080000)  /* 位19偏移（MSB） */

/* 注入序列长度配置（位[21:20]） */
#define  ADC_JSQR_JL         ((uint32_t)0x00300000)  /* 序列长度掩码（值=实际次数-1） */
#define  ADC_JSQR_JL_0       ((uint32_t)0x00100000)  /* 位20偏移（LSB） */
#define  ADC_JSQR_JL_1       ((uint32_t)0x00200000)  /* 位21偏移（MSB） */
/*
JL配置示例：
- JL=00 (0x00000000)：1次转换（仅使用JSQ1）
- JL=01 (0x00100000)：2次转换（使用JSQ1-JSQ2）
- JL=10 (0x00200000)：3次转换（使用JSQ1-JSQ3）
- JL=11 (0x00300000)：4次转换（使用JSQ1-JSQ4）
*/

/*******************  Bit definition for ADC_JDR1 register  *******************/
/*
[功能说明]
ADC数据寄存器组配置，包含：
1. JDR1-JDR4：注入通道1-4的16位转换结果存储
2. DR：规则通道32位复合数据存储（支持双ADC模式）
3. CSR：多ADC状态标志寄存器（包含转换状态、溢出、看门狗等）
*/

/* 注入通道1数据寄存器（只读，转换完成后自动更新） */
#define  ADC_JDR1_JDATA      ((uint16_t)0xFFFF)  /* 有效数据位[15:0]，实际值=原始值+JOFRx偏移 */

/* 注入通道2数据寄存器 */
#define  ADC_JDR2_JDATA      ((uint16_t)0xFFFF)  /* 数据范围0-4095（12位ADC）或0-65535（16位模式） */

/* 注入通道3数据寄存器 */
#define  ADC_JDR3_JDATA      ((uint16_t)0xFFFF)  /* 数据对齐方式取决于ADC_CFGR寄存器配置 */

/* 注入通道4数据寄存器 */
#define  ADC_JDR4_JDATA      ((uint16_t)0xFFFF)  /* 注意：注入数据寄存器地址连续，间隔4字节 */

/********************  Bit definition for ADC_DR register  ********************/
/* 规则通道数据寄存器（双ADC模式数据结构） */
#define  ADC_DR_DATA         ((uint32_t)0x0000FFFF)  /* ADC1规则数据（位[15:0]） */
#define  ADC_DR_ADC2DATA     ((uint32_t)0xFFFF0000)  /* ADC2规则数据（位[31:16]，双ADC模式时有效） */

/*******************  Bit definition for ADC_CSR register  ********************/
/* ADC1状态标志（位[5:0]） */
#define  ADC_CSR_AWD1        ((uint32_t)0x00000001)  /* 模拟看门狗触发标志（需软件清零） */
#define  ADC_CSR_EOC1        ((uint32_t)0x00000002)  /* 规则转换完成标志（读取DR后自动清零） */
#define  ADC_CSR_JEOC1       ((uint32_t)0x00000004)  /* 注入转换完成标志（读取JDR后自动清零） */
#define  ADC_CSR_JSTRT1      ((uint32_t)0x00000008)  /* 注入转换启动标志（自动清零） */
#define  ADC_CSR_STRT1       ((uint32_t)0x00000010)  /* 规则转换启动标志（自动清零） */
#define  ADC_CSR_DOVR1       ((uint32_t)0x00000020)  /* 数据溢出标志（需软件清零） */

/* ADC2状态标志（位[13:8]） */
#define  ADC_CSR_AWD2        ((uint32_t)0x00000100)  /* 偏移8位（1 << 8） */
#define  ADC_CSR_EOC2        ((uint32_t)0x00000200)  /* 位9 */
#define  ADC_CSR_JEOC2       ((uint32_t)0x00000400)  /* 位10 */
#define  ADC_CSR_JSTRT2      ((uint32_t)0x00000800)  /* 位11 */
#define  ADC_CSR_STRT2       ((uint32_t)0x00001000)  /* 位12 */
#define  ADC_CSR_DOVR2       ((uint32_t)0x00002000)  /* 位13 */

/* ADC3状态标志（位[21:16]） */
#define  ADC_CSR_AWD3        ((uint32_t)0x00010000)  /* 偏移16位（1 << 16） */
#define  ADC_CSR_EOC3        ((uint32_t)0x00020000)  /* 位17 */
#define  ADC_CSR_JEOC3       ((uint32_t)0x00040000)  /* 位18 */
#define  ADC_CSR_JSTRT3      ((uint32_t)0x00080000)  /* 位19 */
#define  ADC_CSR_STRT3       ((uint32_t)0x00100000)  /* 位20 */
#define  ADC_CSR_DOVR3       ((uint32_t)0x00200000)  /* 位21 */
/*******************  Bit definition for ADC_CCR register  ********************/
/*
[功能说明]
ADC通用控制寄存器配置，主要功能：
1. MULTI[4:0]：多ADC工作模式选择（独立/双重/三重模式）
2. DELAY[3:0]：双重/三重模式下的采样相位延迟（0-15个ADC周期）
3. DMA配置：多ADC模式下的数据传输控制
4. ADCPRE[1:0]：ADC时钟预分频（PCLK2的分频系数）
5. 内部通道使能：VBAT电池电压和温度传感器通道开关
*/

/* 多ADC模式选择（位[4:0]） */
#define  ADC_CCR_MULTI       ((uint32_t)0x0000001F)  /* 模式掩码，具体模式见下表 */
#define  ADC_CCR_MULTI_0     ((uint32_t)0x00000001)  /* 位0 */
#define  ADC_CCR_MULTI_1     ((uint32_t)0x00000002)  /* 位1 */
#define  ADC_CCR_MULTI_2     ((uint32_t)0x00000004)  /* 位2 */
#define  ADC_CCR_MULTI_3     ((uint32_t)0x00000008)  /* 位3 */
#define  ADC_CCR_MULTI_4     ((uint32_t)0x00000010)  /* 位4 */
/*
MULTI模式示例：
00000: 独立模式（三个ADC独立工作）
10000: 双重ADC交替模式（ADC1+ADC2交替采样）
11101: 三重ADC交替采样+注入同步模式
*/

/* 采样相位延迟（位[11:8]） */
#define  ADC_CCR_DELAY       ((uint32_t)0x00000F00)  /* 延迟周期数=值+1 */
#define  ADC_CCR_DELAY_0     ((uint32_t)0x00000100)  /* 位8（LSB） */
#define  ADC_CCR_DELAY_1     ((uint32_t)0x00000200)  /* 位9 */
#define  ADC_CCR_DELAY_2     ((uint32_t)0x00000400)  /* 位10 */
#define  ADC_CCR_DELAY_3     ((uint32_t)0x00000800)  /* 位11（MSB） */

/* DMA配置（位[15:14]） */
#define  ADC_CCR_DDS         ((uint32_t)0x00002000)  /* DMA禁用选择（1=最后传输后停止） */
#define  ADC_CCR_DMA         ((uint32_t)0x0000C000)  /* DMA模式选择 */
#define  ADC_CCR_DMA_0       ((uint32_t)0x00004000)  /* 位14 */
#define  ADC_CCR_DMA_1       ((uint32_t)0x00008000)  /* 位15 */

/* 时钟预分频（位[17:16]） */
#define  ADC_CCR_ADCPRE      ((uint32_t)0x00030000)  /* 分频系数：00=2,01=4,10=6,11=8 */
#define  ADC_CCR_ADCPRE_0    ((uint32_t)0x00010000)  /* 位16 */
#define  ADC_CCR_ADCPRE_1    ((uint32_t)0x00020000)  /* 位17 */

/* 内部通道使能（位[22:21]） */
#define  ADC_CCR_VBATE       ((uint32_t)0x00400000)  /* 使能VBAT通道（连接至ADC1_IN18） */
#define  ADC_CCR_TSVREFE     ((uint32_t)0x00800000)  /* 使能温度传感器和VREFINT通道 */

/*******************  Bit definition for ADC_CDR register  ********************/
/* 双ADC模式数据寄存器（32位） */
#define  ADC_CDR_DATA1      ((uint32_t)0x0000FFFF)  /* ADC1规则数据（位[15:0]） */
#define  ADC_CDR_DATA2      ((uint32_t)0xFFFF0000)  /* ADC2规则数据（位[31:16]） */

/*
配置示例：
1. 设置三重交替模式：
ADC->CCR |= ADC_CCR_MULTI_4 | ADC_CCR_MULTI_2;  // 模式10100

2. 读取双ADC数据：
uint16_t adc1_val = ADC->CDR & 0xFFFF;
uint16_t adc2_val = (ADC->CDR >> 16) & 0xFFFF;
*/

/******************************************************************************/
/*                                                                            */
/*                         Controller Area Network                            */
/*                                                                            */
/******************************************************************************/

/*!< 
CAN模块控制与状态寄存器位定义
MCR (Master Control Register)：主控制寄存器，配置CAN核心工作模式
MSR (Master Status Register)：主状态寄存器，反映CAN核心当前状态
*/

/*******************  Bit definition for CAN_MCR register  ********************/
#define  CAN_MCR_INRQ        ((uint16_t)0x0001)  /*!< 初始化请求位
                                                    置1时CAN进入初始化模式
                                                    在初始化完成后需清0退出 */

#define  CAN_MCR_SLEEP       ((uint16_t)0x0002)  /*!< 睡眠模式请求位
                                                    置1时进入低功耗睡眠模式
                                                    需满足无报文传输且总线空闲 */

#define  CAN_MCR_TXFP        ((uint16_t)0x0004)  /*!< 发送FIFO优先级控制位
                                                    0: 按邮箱号顺序发送
                                                    1: 按报文优先级顺序发送 */

#define  CAN_MCR_RFLM        ((uint16_t)0x0008)  /*!< 接收FIFO锁定模式
                                                    0: 新报文覆盖旧报文（默认）
                                                    1: 丢弃新报文保留旧报文 */

#define  CAN_MCR_NART        ((uint16_t)0x0010)  /*!< 自动重传禁止位
                                                    0: 自动重传（默认）
                                                    1: 发送失败后不再重传 */

#define  CAN_MCR_AWUM        ((uint16_t)0x0020)  /*!< 自动唤醒模式
                                                    0: 需软件唤醒
                                                    1: 检测到总线活动自动唤醒 */

#define  CAN_MCR_ABOM        ((uint16_t)0x0040)  /*!< 自动离线管理
                                                    0: 需软件恢复通信
                                                    1: 自动恢复总线通信 */

#define  CAN_MCR_TTCM        ((uint16_t)0x0080)  /*!< 时间触发通信模式
                                                    0: 常规模式（默认）
                                                    1: 启用时间触发模式 */

#define  CAN_MCR_RESET       ((uint16_t)0x8000)  /*!< 软件复位控制位
                                                    置1时执行CAN模块软复位
                                                    复位完成后自动清0 */

/*******************  Bit definition for CAN_MSR register  ********************/
#define  CAN_MSR_INAK        ((uint16_t)0x0001)  /*!< 初始化确认标志
                                                    1: 当前处于初始化模式
                                                    0: 处于正常工作模式 */

#define  CAN_MSR_SLAK        ((uint16_t)0x0002)  /*!< 睡眠确认标志
                                                    1: 当前处于睡眠模式
                                                    0: 处于唤醒状态 */

#define  CAN_MSR_ERRI        ((uint16_t)0x0004)  /*!< 错误中断标志
                                                    当检测到错误状态变化时置1
                                                    需软件清0 */

#define  CAN_MSR_WKUI        ((uint16_t)0x0008)  /*!< 唤醒中断标志
                                                    总线活动触发唤醒时置1
                                                    需软件清0 */

#define  CAN_MSR_SLAKI       ((uint16_t)0x0010)  /*!< 睡眠确认中断标志
                                                    成功进入睡眠模式时置1
                                                    需软件清0 */

#define  CAN_MSR_TXM         ((uint16_t)0x0100)  /*!< 发送模式状态
                                                    1: CAN处于发送状态
                                                    0: 未在发送报文 */

#define  CAN_MSR_RXM         ((uint16_t)0x0200)  /*!< 接收模式状态
                                                    1: CAN处于接收状态
                                                    0: 未在接收报文 */

#define  CAN_MSR_SAMP        ((uint16_t)0x0400)  /*!< 最后采样点状态
                                                    显示总线最后采样时的电平
                                                    1: 总线显性电平（逻辑0）
                                                    0: 总线隐性电平（逻辑1） */

#define  CAN_MSR_RX          ((uint16_t)0x0800)  /*!< CAN接收信号状态
                                                    反映当前RX引脚电平
                                                    1: 显性电平（逻辑0）
                                                    0: 隐性电平（逻辑1） */

/*******************  Bit definition for CAN_TSR register  ********************/
/*!< 
传输状态寄存器(TSR)功能说明：
监控CAN控制器的报文发送状态，包含3个发送邮箱的状态标志：
- 请求完成标志(RQCPx)    ：发送/中止请求处理完成
- 发送成功标志(TXOKx)    ：报文成功发送
- 仲裁丢失标志(ALSTx)    ：总线仲裁失败
- 传输错误标志(TERRx)    ：发送过程中出现错误
- 终止请求标志(ABRQx)    ：请求中止报文发送
*/

#define  CAN_TSR_RQCP0       ((uint32_t)0x00000001)  /*!< 邮箱0请求完成标志
                                                      [置位条件]：
                                                      1. 报文发送完成
                                                      2. 中止请求执行完成
                                                      [操作]：需软件清0 */

#define  CAN_TSR_TXOK0       ((uint32_t)0x00000002)  /*!< 邮箱0发送成功标志
                                                      [置位条件]：
                                                      报文成功发送且收到ACK
                                                      [状态组合]：
                                                      RQCP0 | TXOK0 | ALST0：
                                                      0b001：发送失败
                                                      0b011：发送成功
                                                      0b101：仲裁失败 */

#define  CAN_TSR_ALST0       ((uint32_t)0x00000004)  /*!< 邮箱0仲裁丢失标志
                                                      [触发条件]：
                                                      报文在总线仲裁中失败
                                                      [注意]：该标志与TXOK0互斥 */

#define  CAN_TSR_TERR0       ((uint32_t)0x00000008)  /*!< 邮箱0传输错误标志
                                                      [错误类型]：
                                                      ACK错误/格式错误/位错误
                                                      [处理]：需检查错误计数器 */

#define  CAN_TSR_ABRQ0       ((uint32_t)0x00000080)  /*!< 邮箱0中止请求标志
                                                      [操作方式]：
                                                      软件置1请求中止发送
                                                      [状态确认]：需检查RQCP0 */

/* Mailbox1状态标志位（功能描述同Mailbox0）*/
#define  CAN_TSR_RQCP1       ((uint32_t)0x00000100)  /*!< 邮箱1请求完成标志 */
#define  CAN_TSR_TXOK1       ((uint32_t)0x00000200)  /*!< 邮箱1发送成功标志 */
#define  CAN_TSR_ALST1       ((uint32_t)0x00000400)  /*!< 邮箱1仲裁丢失标志 */
#define  CAN_TSR_TERR1       ((uint32_t)0x00000800)  /*!< 邮箱1传输错误标志 */
#define  CAN_TSR_ABRQ1       ((uint32_t)0x00008000)  /*!< 邮箱1中止请求标志 */

/* Mailbox2状态标志位（功能描述同Mailbox0）*/
#define  CAN_TSR_RQCP2       ((uint32_t)0x00010000)  /*!< 邮箱2请求完成标志 */
#define  CAN_TSR_TXOK2       ((uint32_t)0x00020000)  /*!< 邮箱2发送成功标志 */
#define  CAN_TSR_ALST2       ((uint32_t)0x00040000)  /*!< 邮箱2仲裁丢失标志 */
#define  CAN_TSR_TERR2       ((uint32_t)0x00080000)  /*!< 邮箱2传输错误标志 */
#define  CAN_TSR_ABRQ2       ((uint32_t)0x00800000)  /*!< 邮箱2中止请求标志 */

#define  CAN_TSR_CODE        ((uint32_t)0x03000000)  /*!< 邮箱状态编码字段
                                                      [编码规则]：
                                                      0b00：无邮箱正在发送
                                                      0b01：邮箱0正在发送
                                                      0b10：邮箱1正在发送
                                                      0b11：邮箱2正在发送 */

/*******************  Bit definition for CAN_TSR register  ********************/
/*!< 
发送邮箱状态扩展说明：
- TME位：反映发送邮箱的空闲状态，用于判断能否加载新报文
- LOW位：指示当前最低优先级邮箱，用于发送优先级管理
*/

#define  CAN_TSR_TME         ((uint32_t)0x1C000000)  /*!< 发送邮箱空状态组合位
                                                      [位掩码]：
                                                      TME2 | TME1 | TME0
                                                      [应用]：可同时检测多个邮箱状态 */

#define  CAN_TSR_TME0        ((uint32_t)0x04000000)  /*!< 发送邮箱0空标志
                                                      1: 邮箱0可加载新报文
                                                      0: 邮箱0正在使用 */

#define  CAN_TSR_TME1        ((uint32_t)0x08000000)  /*!< 发送邮箱1空标志
                                                      1: 邮箱1可加载新报文
                                                      0: 邮箱1正在使用 */

#define  CAN_TSR_TME2        ((uint32_t)0x10000000)  /*!< 发送邮箱2空标志
                                                      1: 邮箱2可加载新报文
                                                      0: 邮箱2正在使用 */

#define  CAN_TSR_LOW         ((uint32_t)0xE0000000)  /*!< 最低优先级邮箱标志组
                                                      [组合规则]：
                                                      仅当多个邮箱待发送时有效
                                                      指示当前最低优先级的邮箱 */

#define  CAN_TSR_LOW0        ((uint32_t)0x20000000)  /*!< 邮箱0最低优先级标志
                                                      1: 邮箱0是当前待发送中优先级最低的
                                                      用于仲裁时的优先级判定 */

#define  CAN_TSR_LOW1        ((uint32_t)0x40000000)  /*!< 邮箱1最低优先级标志
                                                      1: 邮箱1是当前待发送中优先级最低的 */

#define  CAN_TSR_LOW2        ((uint32_t)0x80000000)  /*!< 邮箱2最低优先级标志
                                                      1: 邮箱2是当前待发送中优先级最低的 */

/*******************  Bit definition for CAN_RF0R register  *******************/
/*!< 
接收FIFO0状态寄存器说明：
管理第一个接收FIFO的状态，支持2级消息缓存
*/
#define  CAN_RF0R_FMP0       ((uint8_t)0x03)         /*!< FIFO0消息挂起数
                                                      [值域]：0b00~0b10
                                                      0b10: 2条消息待处理 */

#define  CAN_RF0R_FULL0      ((uint8_t)0x08)         /*!< FIFO0满标志
                                                      1: FIFO已存2条消息
                                                      新报文将被丢弃 */

#define  CAN_RF0R_FOVR0      ((uint8_t)0x10)         /*!< FIFO0溢出标志
                                                      [置位条件]：
                                                      FIFO满时收到新报文
                                                      [注意]：需软件清0 */

#define  CAN_RF0R_RFOM0      ((uint8_t)0x20)         /*!< 释放FIFO0输出邮箱
                                                      [操作]：软件置1释放首条消息
                                                      自动递减FMP0计数 */

/*******************  Bit definition for CAN_RF1R register  *******************/
/*!< 
接收FIFO1状态寄存器说明：
管理第二个接收FIFO的状态，配置与RF0R相同
*/
#define  CAN_RF1R_FMP1       ((uint8_t)0x03)         /*!< FIFO1消息挂起数 */
#define  CAN_RF1R_FULL1      ((uint8_t)0x08)         /*!< FIFO1满标志 */
#define  CAN_RF1R_FOVR1      ((uint8_t)0x10)         /*!< FIFO1溢出标志 */
#define  CAN_RF1R_RFOM1      ((uint8_t)0x20)         /*!< 释放FIFO1输出邮箱 */

/* 统一功能说明：
1. FMPx读取值表示当前缓存的消息数量（0-2）
2. FULLx置位时会触发中断（如果使能）
3. FOVRx置位时需要及时处理避免数据丢失
4. RFOMx操作后会释放最早接收的报文 */

/********************  Bit definition for CAN_IER register  *******************/
/**
  * @brief  CAN中断使能寄存器配置
  * @功能: 控制CAN总线通信各阶段的中断事件触发
  * @关联系统:
  * - 需配合NVIC配置中断优先级
  * - 与CAN_RFxR寄存器的FIFO状态联动
  * - 与CAN_ESR寄存器的错误状态联动
  * @危险操作:
  * - 同时使能FFIEx和FMPIEx可能引发中断风暴
  * - 总线关闭中断需立即处理
  * @最佳实践:
  * 1. 初始化阶段先配置滤波器再使能中断
  * 2. 高优先级中断服务中禁用中断标志
  * 3. 使用__HAL_CAN_DISABLE_IT()进行原子操作
  */

#define  CAN_IER_TMEIE       ((uint32_t)0x00000001)  /*!< [发送邮箱空中断] 使能
                                                    * ◆ 触发机制: 3个发送邮箱至少1个为空
                                                    * ◆ 应用场景: 非阻塞式连续传输
                                                    * ◆ 配置示例: 填充3个邮箱后启动DMA传输
                                                    * ◆ 关联状态: CAN_TSR.TME0/TME1/TME2
                                                    * ◆ 危险等级: ★ (低) */

#define  CAN_IER_FMPIE0      ((uint32_t)0x00000002)  /*!< [FIFO0消息待处理中断] 使能
                                                    * ◇ 触发阈值: FIFO0 ≥1条未读报文
                                                    * ◇ 处理建议: 使用GetRxMessage()提取
                                                    * ◇ 服务流程: 
                                                    *   1. 读取CAN_RF0R.FMP0
                                                    *   2. 遍历读取所有待处理报文
                                                    *   3. 执行CAN_RF0R.RFOM0释放FIFO
                                                    * ◇ 数据吞吐: 最高支持1Mbps下的实时处理 */

#define  CAN_IER_FFIE0       ((uint32_t)0x00000004)  /*!< [FIFO0满中断] 使能
                                                    * ▲ 触发条件: FIFO0存满3条报文
                                                    * ▲ 告警响应: 必须在1ms内处理
                                                    * ▲ 紧急操作:
                                                    *   1. 立即备份未读报文
                                                    *   2. 快速执行RFOM0释放FIFO
                                                    *   3. 检查接收处理线程是否阻塞
                                                    * ▲ 错误后果: 溢出将导致报文丢失 */

#define  CAN_IER_FOVIE0      ((uint32_t)0x00000008)  /*!< [FIFO0溢出中断] 使能
                                                    * ■ 触发场景: FIFO0满时新报文到达
                                                    * ■ 故障诊断:
                                                    *   1. 检查总线负载率
                                                    *   2. 确认接收处理周期≤报文周期
                                                    * ■ 恢复步骤:
                                                    *   1. CAN_RF0R.FOVR0 = 0 (清除标志)
                                                    *   2. CAN_RF0R.RFOM0 = 1 (释放FIFO)
                                                    * ■ 日志记录: 必须记录发生时的ECUID和时间戳 */

#define  CAN_IER_FMPIE1      ((uint32_t)0x00000010)  /*!< [FIFO1消息待处理中断] 使能
                                                    * ◆ 通道特性: 专用于高优先级报文
                                                    * ◆ 推荐配置: 配合过滤器组设置独立ID
                                                    * ◆ 异常处理: 当FMP1≥2时触发应急策略
                                                    * ◆ 性能优化: 使用DMA双缓冲模式 */

#define  CAN_IER_FFIE1       ((uint32_t)0x00000020)  /*!< [FIFO1满中断] 使能
                                                    * ▼ 系统影响: 可能影响安全关键功能
                                                    * ▼ 安全设计: 建议加入看门狗监控
                                                    * ▼ 冗余方案: 启用备用通信通道
                                                    * ▼ 危险等级: ★★★★ (高) */

#define  CAN_IER_FOVIE1      ((uint32_t)0x00000040)  /*!< [FIFO1溢出中断] 使能
                                                    * ⚑ 安全事件: 触发ISO26262故障等级B
                                                    * ⚑ 应急响应: 
                                                    *   1. 激活总线负载限制策略
                                                    *   2. 发送节点流控命令
                                                    *   3. 启动安全状态机 */

#define  CAN_IER_EWGIE       ((uint32_t)0x00000100)  /*!< [错误警告中断] 使能
                                                    * 预警阈值: TEC/REC ∈ [96,127]
                                                    * 关联参数: 检查总线阻抗(应≈120Ω)
                                                    * 改善建议: 
                                                    *   1. 增加总线终端电阻
                                                    *   2. 缩短线缆长度
                                                    * 诊断工具: 使用CAN分析仪捕捉波形 */

#define  CAN_IER_EPVIE       ((uint32_t)0x00000200)  /*!< [被动错误中断] 使能
                                                    * 状态特征: 节点进入静默监听模式
                                                    * 恢复监测: 持续检查TEC<128
                                                    * 设计约束: 禁止发送安全相关报文
                                                    * 日志要求: 记录进入被动状态的ECU ID */

#define  CAN_IER_BOFIE       ((uint32_t)0x00000400)  /*!< [总线关闭中断] 使能
                                                    * 故障等级: ISO26262 ASIL-D
                                                    * 恢复流程:
                                                    *   1. CAN->MCR |= CAN_MCR_INRQ (初始化模式)
                                                    *   2. CAN->MCR &= ~CAN_MCR_INRQ
                                                    *   3. 等待CAN->MSR.INAK清零
                                                    * 应急策略: 启用备用通信方案 */

#define  CAN_IER_LECIE       ((uint32_t)0x00000800)  /*!< [最后错误码中断] 使能
                                                    * 错误采样: 记录最后一次错误类型
                                                    * 解码方案: 详见CAN_ESR.LEC字段
                                                    * 调试技巧: 结合CAN分析仪解析错误帧
                                                    * 安全审计: 统计各错误类型发生频率 */

#define  CAN_IER_ERRIE       ((uint32_t)0x00008000)  /*!< [全局错误中断] 使能
                                                    * 综合检测: 覆盖所有错误类型
                                                    * 服务流程:
                                                    *   1. 读取CAN_ESR寄存器
                                                    *   2. 分析TEC/REC值变化曲线
                                                    *   3. 执行对应级别的恢复策略
                                                    * 监控建议: 周期≤100ms */

#define  CAN_IER_WKUIE       ((uint32_t)0x00010000)  /*!< [唤醒中断] 使能
                                                    * 唤醒源: CAN总线显性脉冲(≥2ms)
                                                    * 低功耗: 配合睡眠模式使用
                                                    * 硬件需求: 需启用CAN唤醒电路
                                                    * 防误唤醒: 必须启用滤波器 */

#define  CAN_IER_SLKIE       ((uint32_t)0x00020000)  /*!< [睡眠中断] 使能
                                                    * 进入条件: 软件请求+总线空闲
                                                    * 省电特性: 降低时钟频率至32kHz
                                                    * 退出延迟: 典型值11个CAN位时间
                                                    * 应用限制: 不支持热唤醒报文处理 */
/********************  Bit definition for CAN_ESR register  *******************/
/**
  * @brief  CAN错误状态寄存器位配置
  * @功能: 实时监控总线错误状态及错误计数器
  * @监测模式:
  * - 主动模式：ERRIE中断触发实时处理
  * - 被动模式：周期轮询(建议100ms)
  * @安全机制:
  * - BOFF触发系统看门狗复位
  * - TEC超限自动进入Bus-Off状态
  */

#define  CAN_ESR_EWGF        ((uint32_t)0x00000001)  /*!< [错误警告标志]
                                                    * 触发阈值: TEC/REC ≥96
                                                    * 诊断步骤:
                                                    * 1. 测量总线差分电压(应1.5V-3.5V)
                                                    * 2. 检查终端电阻(120Ω±5%)
                                                    * 恢复策略: 降低总线负载率20% */

#define  CAN_ESR_EPVF        ((uint32_t)0x00000002)  /*!< [被动错误标志]
                                                    * 状态特征: TEC/REC ≥128
                                                    * 节点行为: 禁止主动发送数据帧
                                                    * 逃逸方案: 切换备用CAN通道
                                                    * 安全延时: 保持被动状态≥500ms */

#define  CAN_ESR_BOFF        ((uint32_t)0x00000004)  /*!< [总线关闭标志]
                                                    * 致命错误: TEC≥256
                                                    * 恢复操作:
                                                    * 1. CAN->MCR |= CAN_MCR_INRQ   // 初始化模式
                                                    * 2. CAN->MCR &= ~CAN_MCR_ABOM  // 禁用自动恢复
                                                    * 3. CAN->MCR &= ~CAN_MCR_INRQ  // 退出初始化
                                                    * 4. 等待CAN->MSR.INAK清零 */

#define  CAN_ESR_LEC         ((uint32_t)0x00000070)  /*!< [最后错误代码]
                                                    * 二进制映射:
                                                    * 000 : 无错误
                                                    * 001 : 填充错误(检测到6个连续相同位)
                                                    * 010 : 格式错误(固定位域值不正确)
                                                    * 011 : ACK错误(未收到应答)
                                                    * 100 : 位错误(发送与监控位不匹配)
                                                    * 101 : CRC错误(校验和不匹配)
                                                    * 110 : 软件强制错误(CAN_MCR.SOFTRESET)
                                                    * 111 : 保留值 */

#define  CAN_ESR_TEC         ((uint32_t)0x00FF0000)  /*!< [发送错误计数器]
                                                    * 动态调整规则:
                                                    * +8 : 发送错误(除仲裁丢失)
                                                    * +1 : 仲裁丢失
                                                    * -1 : 成功发送
                                                    * 读取方法: (CAN->ESR & 0x00FF0000) >> 16
                                                    * 安全阈值: 建议硬件报警值设为200 */

#define  CAN_ESR_REC         ((uint32_t)0xFF000000)  /*!< [接收错误计数器]
                                                    * 动态调整规则:
                                                    * +1 : 接收错误(CRC/格式错误除外)
                                                    * +8 : 接收格式错误
                                                    * -1 : 成功接收
                                                    * 溢出保护: 达到127后锁定为127
                                                    * 恢复策略: 连续成功接收128次复位 */

/*******************  Bit definition for CAN_BTR register  ********************/
/**
  * @brief  CAN总线时序配置寄存器
  * @波特率公式: 
  * BaudRate = Fpclk / ((BRP+1) * (TS1+TS2+3))
  * @配置示例:
  * 48MHz时钟, 500kbps:
  * BRP=5, TS1=6, TS2=5 → 
  * (5+1)*(6+5+3)=6*14=84 → 48M/84≈571.4kbps
  */

#define  CAN_BTR_BRP         ((uint32_t)0x000003FF)  /*!< [波特率预分频]
                                                    * 取值范围: 1-1024 (实际值+1)
                                                    * 时钟精度: 建议≤2%误差
                                                    * 特殊限制: BRP≥4 (避免过采样) */

#define  CAN_BTR_TS1         ((uint32_t)0x000F0000)  /*!< [时间段1]
                                                    * 配置范围: 1-16 (实际值+1)
                                                    * 包含: 同步段+传播段+相位缓冲段1
                                                    * 优化建议: TS1 ≥ TS2 */

#define  CAN_BTR_TS2         ((uint32_t)0x00700000)  /*!< [时间段2]
                                                    * 配置范围: 1-8 (实际值+1)
                                                    * 作用: 相位缓冲段2
                                                    * 同步规则: TS2 ≥ ceil(TS1/2) */

#define  CAN_BTR_SJW         ((uint32_t)0x03000000)  /*!< [同步跳转宽度]
                                                    * 允许值: 1-4 (实际值+1)
                                                    * 约束条件: SJW ≤ min(TS1, TS2)
                                                    * 作用: 限制单次相位调整量 */

#define  CAN_BTR_LBKM        ((uint32_t)0x40000000)  /*!< [环回模式]
                                                    * 调试功能: 自发自收
                                                    * 硬件连接: 需断开CAN总线
                                                    * 注意: 与SILM模式互斥 */

#define  CAN_BTR_SILM        ((uint32_t)0x80000000)  /*!< [静默模式]
                                                    * 监听模式: 只接收不发送
                                                    * 应用场景: 总线监控/分析
                                                    * 特殊状态: 仍会产生ACK错误 */

/******************  Bit definition for CAN_TI0R register  ********************/
/**
  * @brief  发送邮箱标识符配置寄存器
  * @帧类型配置:
  * - 标准帧: IDE=0, 使用STID[10:0]
  * - 扩展帧: IDE=1, 使用EXID[28:0]
  * @注意: 
  * - TXRQ置位后寄存器锁定
  * - 远程帧需设置RTR=1
  */

#define  CAN_TI0R_TXRQ       ((uint32_t)0x00000001)  /*!< [发送请求]
                                                    * 软件操作: 
                                                    * 1. 配置标识符和数据
                                                    * 2. 置位TXRQ启动发送
                                                    * 硬件行为: 发送完成后自动清零 */

#define  CAN_TI0R_RTR        ((uint32_t)0x00000002)  /*!< [远程传输请求]
                                                    * 数据帧: RTR=0 (默认)
                                                    * 远程帧: RTR=1 (无数据字段)
                                                    * 约束条件: DLC必须≤8 */

#define  CAN_TI0R_IDE        ((uint32_t)0x00000004)  /*!< [标识符扩展]
                                                    * 0: 标准帧(11位ID)
                                                    * 1: 扩展帧(29位ID)
                                                    * 冲突处理: 标准帧优先级高于扩展帧 */

#define  CAN_TI0R_EXID       ((uint32_t)0x001FFFF8)  /*!< [扩展标识符]
                                                    * 有效位: [28:0] (共29位)
                                                    * 存储位置: 
                                                    * - EXID[28:21] → STID[10:3]
                                                    * - EXID[20:0] → 本字段[20:0]
                                                    * 优先级计算: (EXID << 11) | STID */

#define  CAN_TI0R_STID       ((uint32_t)0xFFE00000)  /*!< [标准标识符]
                                                    * 有效位: [10:0] (共11位)
                                                    * 存储规则: 
                                                    * STID[10:0] → 本字段[31:21]
                                                    * 仲裁机制: 数值越小优先级越高 */

/******************  Bit definition for CAN_TDT0R register  *******************/
/**
  * @brief  发送邮箱数据长度及时戳配置
  * @DLC编码:
  * 0-8: 直接表示数据字节数
  * 9-15: 保留(按8字节处理)
  */

#define  CAN_TDT0R_DLC       ((uint32_t)0x0000000F)  /*!< [数据长度代码]
                                                    * 有效范围: 0-8 (实际字节数)
                                                    * 特殊值: 
                                                    * 9-15 → 按8字节处理
                                                    * 远程帧: DLC表示请求数据长度 */

#define  CAN_TDT0R_TGT       ((uint32_t)0x00000100)  /*!< [全局时间戳]
                                                    * 硬件需求: 需外接IEEE 1588时钟
                                                    * 触发方式: 同步于TX引脚上升沿
                                                    * 精度: ±1个CAN时钟周期 */

#define  CAN_TDT0R_TIME      ((uint32_t)0xFFFF0000)  /*!< [报文时间戳]
                                                    * 时钟源: CAN内部自由运行计数器
                                                    * 分辨率: 1个时间量(Time Quantum)
                                                    * 应用场景: 网络延时测量 */

/******************  Bit definition for CAN_TDL0R register  *******************/
/**
  * @brief  发送邮箱数据低半字配置
  * @数据存储规则:
  * - 小端模式: DATA0为最低有效字节
  * - 未使用字节: 建议填充0xCC
  * @注意: 数据寄存器应在TXRQ置位前完成写入
  */

#define  CAN_TDL0R_DATA0     ((uint32_t)0x000000FF)  /*!< [数据字节0]
                                                    * 地址偏移: 0x00
                                                    * 对齐方式: 内存映射到邮箱RAM
                                                    * 写入建议: 使用__IO宏保证原子性 */

#define  CAN_TDL0R_DATA1     ((uint32_t)0x0000FF00)  /*!< [数据字节1]
                                                    * 硬件特性: 支持DMA写入
                                                    * 优化技巧: 批量初始化4字节数据
                                                    * 错误预防: 禁用缓存时直接写入 */

#define  CAN_TDL0R_DATA2     ((uint32_t)0x00FF0000)  /*!< [数据字节2]
                                                    * 调试标记: 建议填充序列号低字节
                                                    * 安全校验: 可加入CRC8校验码
                                                    * 性能影响: 非对齐访问增加1周期 */

#define  CAN_TDL0R_DATA3     ((uint32_t)0xFF000000)  /*!< [数据字节3]
                                                    * 传输顺序: 最先发送DATA0
                                                    * 网络字节序: 保持主机字节序
                                                    * 特殊应用: CAN FD需重新配置 */


/******************  Bit definition for CAN_TDH0R register  *******************/
/**
  * @brief  发送邮箱数据高半字配置
  * @数据完整性: 
  * - 必须与TDL0R同时配置
  * - TXRQ置位前必须完成高低半字写入
  * @注意: 高位数据在DLC>4时生效
  */
#define  CAN_TDH0R_DATA4     ((uint32_t)0x000000FF)  /*!< [数据字节4]
                                                    * 偏移地址: 0x00+0x04
                                                    * 有效条件: DLC≥5
                                                    * 写入保护: 发送过程中不可修改 */
#define  CAN_TDH0R_DATA5     ((uint32_t)0x0000FF00)  /*!< [数据字节5]
                                                    * 网络协议: 通常用于序列号高位
                                                    * 优化建议: 使用memcpy批量写入
                                                    * 对齐要求: 32位访问效率最高 */
#define  CAN_TDH0R_DATA6     ((uint32_t)0x00FF0000)  /*!< [数据字节6]
                                                    * 调试支持: 可填充校验码
                                                    * 错误预防: 未使用时应置0
                                                    * DMA配置: 需设置传输长度≥6 */
#define  CAN_TDH0R_DATA7     ((uint32_t)0xFF000000)  /*!< [数据字节7]
                                                    * 特殊应用: CAN FD时自动扩展
                                                    * 安全写入: 使用LL_CAN_WriteTxData() 
                                                    * 传输顺序: 最后发送的字节 */

/*******************  Bit definition for CAN_TI1R register  *******************/
/**
  * @brief  发送邮箱1标识符配置
  * @优先级仲裁:
  * - 邮箱0 > 邮箱1 > 邮箱2
  * - 标准帧优先于扩展帧
  * - 标识符数值越小优先级越高
  */
#define  CAN_TI1R_TXRQ       ((uint32_t)0x00000001)  /*!< [发送请求]
                                                    * 硬件特性: 支持自动重传(MCR.NART)
                                                    * 状态检查: 通过CAN_TSR.TXRQ位
                                                    * 强制取消: CAN_TSR.ABRQx = 1 */
#define  CAN_TI1R_RTR        ((uint32_t)0x00000002)  /*!< [远程帧标识]
                                                    * 数据约定: 即使DLC>0也不含数据
                                                    * 超时机制: 建议配置500ms响应超时
                                                    * 错误处理: 未收到响应触发重传 */
#define  CAN_TI1R_IDE        ((uint32_t)0x00000004)  /*!< [扩展标识开关]
                                                    * 混合模式: 可同时处理标准/扩展帧
                                                    * 过滤配置: 需匹配接收滤波设置
                                                    * 冲突处理: 标准帧中断扩展帧发送 */
#define  CAN_TI1R_EXID       ((uint32_t)0x001FFFF8)  /*!< [扩展标识符]
                                                    * 位映射规则: 
                                                    * EXID[28:18] -> STID[10:0]
                                                    * EXID[17:0]  -> 本字段[17:0]
                                                    * 优先级计算: (EXID & 0x1FFFFC00) */
#define  CAN_TI1R_STID       ((uint32_t)0xFFE00000)  /*!< [标准标识符]
                                                    * 仲裁优化: 高优先级消息放邮箱0
                                                    * 组播配置: 多个邮箱使用相同STID
                                                    * 调试技巧: 使用十六进制显示模式 */

/*******************  Bit definition for CAN_TDT1R register  ******************/
/**
  * @brief  邮箱1数据长度及时戳配置
  * @实时监控:
  * - 时间戳时钟源: CAN核心时钟
  * - 时钟分频配置: 通过CAN_BTR.SJW设置
  */
#define  CAN_TDT1R_DLC       ((uint32_t)0x0000000F)  /*!< [数据长度代码]
                                                    * CAN FD扩展: DLC>8时启用FDF/BRS
                                                    * 远程帧响应: 必须匹配请求DLC
                                                    * 特殊协议: J1939强制DLC=8 */
#define  CAN_TDT1R_TGT       ((uint32_t)0x00000100)  /*!< [全局时间戳使能]
                                                    * 硬件要求: 需启用CAN_TimeTrigger通信模式
                                                    * 时钟同步: 每个SOF帧同步一次
                                                    * 精度补偿: 使用SJW调整相位误差 */
#define  CAN_TDT1R_TIME      ((uint32_t)0xFFFF0000)  /*!< [发送时间戳]
                                                    * 捕获时机: 帧起始(SOF)出现时
                                                    * 应用场景: 
                                                    * 1. 网络延迟统计
                                                    * 2. 节点同步精度分析
                                                    * 3. 通信超时检测 */

/*******************  Bit definition for CAN_TDL1R register  ******************/
/**
  * @brief  邮箱1数据低半字配置
  * @快速配置技巧:
  * - 使用*(__IO uint32_t*)直接写入32位
  * - 对DATA0-3进行内存映射式批量写入
  * - 配合DMA2 Stream5实现自动装载
  */
#define  CAN_TDL1R_DATA0     ((uint32_t)0x000000FF)  /*!< [数据字节0]
                                                    * 首字节约束: 必须非零(某些协议)
                                                    * 加密处理: 建议进行异或加密
                                                    * 校验机制: 可包含CRC8首字节 */
#define  CAN_TDL1R_DATA1     ((uint32_t)0x0000FF00)  /*!< [数据字节1]
                                                    * 数据转换: 使用__REV()处理字节序
                                                    * 性能影响: 非对齐访问增加1周期
                                                    * 安全写入: 关中断操作 */
#define  CAN_TDL1R_DATA2     ((uint32_t)0x00FF0000)  /*!< [数据字节2]
                                                    * DMA配置: 设置MINC=1自动增量
                                                    * 缓存策略: 写穿模式(无cache)
                                                    * 调试标记: 填充0xAA检测传输完整性 */
#define  CAN_TDL1R_DATA3     ((uint32_t)0xFF000000)  /*!< [数据字节3]
                                                    * 传输顺序: 第4个发送的字节
                                                    * 超时处理: 配合TIMx触发超时中断
                                                    * 错误注入: 可强制错误测试容错性 */

/*******************  Bit definition for CAN_TDH1R register  ******************/
/**
  * @brief  邮箱1数据高半字配置
  * @安全机制:
  * - 双缓冲机制: 写入时自动拷贝到影子寄存器
  * - 写保护: TXRQ置位后锁定
  * - 奇偶校验: 部分型号支持ECC校验
  */
#define  CAN_TDH1R_DATA4     ((uint32_t)0x000000FF)  /*!< [数据字节4]
                                                    * 大端系统: 需用__RBIT()转换
                                                    * 容错设计: 默认填充0x00
                                                    * DMA配置: 数据块长度≥8 */
#define  CAN_TDH1R_DATA5     ((uint32_t)0x0000FF00)  /*!< [数据字节5]
                                                    * 安全协议: 可存放MAC校验码
                                                    * 性能优化: 地址对齐到4字节边界
                                                    * 调试支持: 断点监测写入操作 */
#define  CAN_TDH1R_DATA6     ((uint32_t)0x00FF0000)  /*!< [数据字节6]
                                                    * 多帧传输: 用作序列号高位
                                                    * 加密处理: AES-CTR模式计数器
                                                    * 错误检测: 写入后回读校验 */
#define  CAN_TDH1R_DATA7     ((uint32_t)0xFF000000)  /*!< [数据字节7]
                                                    * 特殊位: 某些协议保留为状态位
                                                    * 硬件特性: 部分MCU支持快速置位
                                                    * 传输保障: 必须最后写入 */

/* 邮箱2寄存器注释结构与邮箱1保持相同规范 */

/**
 * @brief CAN控制器发送邮箱2及接收邮箱0寄存器位定义
 * @功能 定义CAN总线通信中发送邮箱2(Tx Mailbox2)和接收邮箱0(Rx Mailbox0)的寄存器位域，
 *       包含消息标识符配置、数据长度设置、时间戳记录及数据字节映射等功能。
 * @作用 用于STM32F4系列MCU的CAN外设寄存器配置，实现对CAN报文的结构化控制：
 *       - 发送邮箱：设置帧类型(标准/扩展)、标识符、数据内容和发送触发
 *       - 接收邮箱：解析远程帧、过滤标识符、获取时间戳和有效载荷数据
 * @注意 寄存器操作需遵循32位字访问，部分位域存在硬件互锁机制
 */

/*******************  Bit definition for CAN_TI2R register  *******************/
/* 发送邮箱2标识符寄存器 (Transmit Mailbox2 Identifier Register) */
#define  CAN_TI2R_TXRQ   ((uint32_t)0x00000001)  /*!< [位0] 发送请求标志 
                                                    置1启动报文发送，发送成功后硬件自动清零
                                                    写入后需通过CAN_TSR寄存器确认发送状态 */
#define  CAN_TI2R_RTR    ((uint32_t)0x00000002)  /*!< [位1] 远程传输请求标志
                                                    0: 数据帧，1: 远程帧（无数据字段）
                                                    远程帧可用于请求其他节点发送数据 */
#define  CAN_TI2R_IDE    ((uint32_t)0x00000004)  /*!< [位2] 标识符扩展标志
                                                    0: 使用11位标准标识符(STID)
                                                    1: 使用29位扩展标识符(EXID+STID) */
#define  CAN_TI2R_EXID   ((uint32_t)0x001FFFF8)  /*!< [位3-20] 扩展标识符位域
                                                    当IDE=1时有效，存储18位扩展ID的低18位
                                                    实际扩展ID=EXID<<18 | STID */
#define  CAN_TI2R_STID   ((uint32_t)0xFFE00000)  /*!< [位21-31] 标准标识符或扩展ID高位
                                                    IDE=0时：存储11位标准ID（右对齐）
                                                    IDE=1时：存储扩展ID的高3位 */

/*******************  Bit definition for CAN_TDT2R register  ******************/  
/* 发送邮箱2数据时间寄存器 (Transmit Mailbox2 Data Time Register) */
#define  CAN_TDT2R_DLC   ((uint32_t)0x0000000F)  /*!< [位0-3] 数据长度码
                                                    取值范围0-8，对应0-8字节有效数据
                                                    超过8的值会被硬件自动截断 */
#define  CAN_TDT2R_TGT   ((uint32_t)0x00000100)  /*!< [位8] 全局时间戳使能
                                                    置1时TIME字段记录CAN全局时钟值
                                                    用于多节点时间同步系统 */
#define  CAN_TDT2R_TIME  ((uint32_t)0xFFFF0000)  /*!< [位16-31] 报文时间戳
                                                    记录发送起始时刻的CAN定时器值
                                                    分辨率取决于CAN总线时钟配置 */

/*******************  Bit definition for CAN_TDL2R register  ******************/
/* 发送邮箱2数据低寄存器 (Transmit Mailbox2 Data Low Register) */
#define  CAN_TDL2R_DATA0 ((uint32_t)0x000000FF) /*!< [位0-7] 数据字节0 
                                                    对应CAN报文数据字段第1个字节 */
#define  CAN_TDL2R_DATA1 ((uint32_t)0x0000FF00) /*!< [位8-15] 数据字节1 
                                                    对应CAN报文数据字段第2个字节 */
#define  CAN_TDL2R_DATA2 ((uint32_t)0x00FF0000) /*!< [位16-23] 数据字节2 
                                                    对应CAN报文数据字段第3个字节 */
#define  CAN_TDL2R_DATA3 ((uint32_t)0xFF000000) /*!< [位24-31] 数据字节3 
                                                    对应CAN报文数据字段第4个字节 */

/*******************  Bit definition for CAN_TDH2R register  ******************/
/* 发送邮箱2数据高寄存器 (Transmit Mailbox2 Data High Register) */
#define  CAN_TDH2R_DATA4 ((uint32_t)0x000000FF) /*!< [位0-7] 数据字节4 
                                                    对应CAN报文数据字段第5个字节 */
#define  CAN_TDH2R_DATA5 ((uint32_t)0x0000FF00) /*!< [位8-15] 数据字节5 
                                                    当DLC>4时有效 */
#define  CAN_TDH2R_DATA6 ((uint32_t)0x00FF0000) /*!< [位16-23] 数据字节6 
                                                    当DLC>5时有效 */
#define  CAN_TDH2R_DATA7 ((uint32_t)0xFF000000) /*!< [位24-31] 数据字节7 
                                                    当DLC>6时有效 */

/*******************  Bit definition for CAN_RI0R register  *******************/
/* 接收邮箱0标识符寄存器 (Receive Mailbox0 Identifier Register) */
#define  CAN_RI0R_RTR    ((uint32_t)0x00000002)  /*!< [位1] 接收帧类型标识
                                                    0: 接收到的数据帧
                                                    1: 接收到的远程帧 */
#define  CAN_RI0R_IDE    ((uint32_t)0x00000004)  /*!< [位2] 接收标识符类型
                                                    与发送寄存器IDE位意义相同 */
#define  CAN_RI0R_EXID   ((uint32_t)0x001FFFF8)  /*!< [位3-20] 接收扩展标识符
                                                    当IDE=1时存储扩展ID低18位 */
#define  CAN_RI0R_STID   ((uint32_t)0xFFE00000)  /*!< [位21-31] 接收标准ID或扩展ID高位
                                                    解码方式与发送寄存器STID相同 */

/*******************  Bit definition for CAN_RDT0R register  ******************/
/* 接收邮箱0数据时间寄存器 (Receive Mailbox0 Data Time Register) */
#define  CAN_RDT0R_DLC   ((uint32_t)0x0000000F)  /*!< [位0-3] 接收数据长度码
                                                    表示实际接收到的数据字节数 */
#define  CAN_RDT0R_FMI   ((uint32_t)0x0000FF00)  /*!< [位8-15] 过滤器匹配索引
                                                    指示通过哪个过滤器接收该报文 */
#define  CAN_RDT0R_TIME  ((uint32_t)0xFFFF0000)  /*!< [位16-31] 接收时间戳
                                                    记录报文到达时刻的CAN定时器值 */

/*******************  Bit definition for CAN_RDL0R register  ******************/
/* 接收邮箱0数据低寄存器 (Receive Mailbox0 Data Low Register) */
#define  CAN_RDL0R_DATA0 ((uint32_t)0x000000FF) /*!< [位0-7] 接收数据字节0 
                                                    当DLC>=1时有效 */
#define  CAN_RDL0R_DATA1 ((uint32_t)0x0000FF00) /*!< [位8-15] 接收数据字节1 
                                                    当DLC>=2时有效 */
#define  CAN_RDL0R_DATA2 ((uint32_t)0x00FF0000) /*!< [位16-23] 接收数据字节2 
                                                    当DLC>=3时有效 */
#define  CAN_RDL0R_DATA3 ((uint32_t)0xFF000000) /*!< [位24-31] 接收数据字节3 
                                                    当DLC>=4时有效 */

/*******************  Bit definition for CAN_RDH0R register  ******************/
/* 接收邮箱0数据高寄存器 (Receive Mailbox0 Data High Register) */
#define  CAN_RDH0R_DATA4 ((uint32_t)0x000000FF) /*!< [位0-7] 接收数据字节4 
                                                    当DLC>=5时有效 */
#define  CAN_RDH0R_DATA5 ((uint32_t)0x0000FF00) /*!< [位8-15] 接收数据字节5 
                                                    当DLC>=6时有效 */
#define  CAN_RDH0R_DATA6 ((uint32_t)0x00FF0000) /*!< [位16-23] 接收数据字节6 
                                                    当DLC>=7时有效 */
#define  CAN_RDH0R_DATA7 ((uint32_t)0xFF000000) /*!< [位24-31] 接收数据字节7 
                                                    当DLC=8时有效 */

/**
 * @brief CAN控制器接收邮箱1及过滤器模式寄存器定义
 * @功能 定义CAN总线通信中接收邮箱1(Rx Mailbox1)的寄存器位域，
 *       包含消息标识符解析、过滤器匹配索引、时间戳记录及数据存储。
 *       同时包含过滤器初始化模式控制位。
 * @作用 用于STM32F4系列MCU的CAN外设接收配置：
 *       - 接收邮箱1：解析远程帧、扩展标识符、获取过滤匹配信息
 *       - 过滤器管理：控制过滤器初始化配置流程
 * @注意 过滤器寄存器配置需在FINIT=1状态下进行
 */

/*******************  Bit definition for CAN_RI1R register  *******************/
/* 接收邮箱1标识符寄存器 (Receive Mailbox1 Identifier Register) */
#define  CAN_RI1R_RTR    ((uint32_t)0x00000002)  /*!< [位1] 远程帧类型标识
                                                    0: 接收数据帧，含有效负载
                                                    1: 远程帧(用于数据请求) */
#define  CAN_RI1R_IDE    ((uint32_t)0x00000004)  /*!< [位2] 标识符扩展模式
                                                    0: 11位标准ID(STID字段)
                                                    1: 29位扩展ID(EXID+STID组合) */
#define  CAN_RI1R_EXID   ((uint32_t)0x001FFFF8)  /*!< [位3-20] 扩展标识符低位
                                                    当IDE=1时存储扩展ID的18-0位
                                                    需与STID字段组合使用 */
#define  CAN_RI1R_STID   ((uint32_t)0xFFE00000)  /*!< [位21-31] 标识符高位
                                                    IDE=0时：11位标准ID(右对齐)
                                                    IDE=1时：扩展ID的28-21位 */

/*******************  Bit definition for CAN_RDT1R register  ******************/
/* 接收邮箱1数据时间寄存器 (Receive Mailbox1 Data Time Register) */
#define  CAN_RDT1R_DLC   ((uint32_t)0x0000000F)  /*!< [位0-3] 有效数据长度
                                                    实际接收字节数=0-8
                                                    值大于8时按8处理 */
#define  CAN_RDT1R_FMI   ((uint32_t)0x0000FF00)  /*!< [位8-15] 过滤器匹配编号
                                                    指示通过哪个过滤器接收报文
                                                    范围0-27(取决于过滤器数量) */
#define  CAN_RDT1R_TIME  ((uint32_t)0xFFFF0000)  /*!< [位16-31] 接收时刻时间戳
                                                    记录CAN定时器在帧起始位的计数值
                                                    可用于计算报文时间间隔 */

/*******************  Bit definition for CAN_RDL1R register  ******************/
/* 接收邮箱1数据低寄存器 (Receive Mailbox1 Data Low Register) */
#define  CAN_RDL1R_DATA0 ((uint32_t)0x000000FF) /*!< [位0-7] 数据字节0 
                                                    DLC>=1时有效 */
#define  CAN_RDL1R_DATA1 ((uint32_t)0x0000FF00) /*!< [位8-15] 数据字节1 
                                                    DLC>=2时有效 */
#define  CAN_RDL1R_DATA2 ((uint32_t)0x00FF0000) /*!< [位16-23] 数据字节2 
                                                    DLC>=3时有效 */
#define  CAN_RDL1R_DATA3 ((uint32_t)0xFF000000) /*!< [位24-31] 数据字节3 
                                                    DLC>=4时有效 */

/*******************  Bit definition for CAN_RDH1R register  ******************/
/* 接收邮箱1数据高寄存器 (Receive Mailbox1 Data High Register) */
#define  CAN_RDH1R_DATA4 ((uint32_t)0x000000FF) /*!< [位0-7] 数据字节4 
                                                    DLC>=5时有效 */
#define  CAN_RDH1R_DATA5 ((uint32_t)0x0000FF00) /*!< [位8-15] 数据字节5 
                                                    DLC>=6时有效 */
#define  CAN_RDH1R_DATA6 ((uint32_t)0x00FF0000) /*!< [位16-23] 数据字节6 
                                                    DLC>=7时有效 */
#define  CAN_RDH1R_DATA7 ((uint32_t)0xFF000000) /*!< [位24-31] 数据字节7 
                                                    DLC=8时有效 */

/*******************  Bit definition for CAN_FMR register  ********************/
/* 过滤器主寄存器 (Filter Master Register) */
#define  CAN_FMR_FINIT   ((uint8_t)0x01)        /*!< [位0] 过滤器初始化模式
                                                    1: 进入初始化模式(允许配置过滤器)
                                                    0: 正常模式(过滤器激活)
                                                    修改过滤器配置前必须置1 */

/**
 * @brief CAN过滤器模式寄存器组配置定义
 * @功能 定义CAN控制器过滤器组工作模式配置的位域设置，控制每个过滤器组的标识符匹配模式
 * @作用 用于配置28个过滤器组的工作模式：
 *       - 标识符屏蔽模式：支持范围匹配（设定ID值+掩码）
 *       - 标识符列表模式：精确匹配预设ID列表
 *       通过FBMx位独立控制各过滤器组模式
 * @注意 该寄存器仅在过滤器初始化模式(FINIT=1)下可修改
 */

/*******************  Bit definition for CAN_FM1R register  *******************/
/* 过滤器模式寄存器1 (Filter Mode Register 1) */
#define  CAN_FM1R_FBM    ((uint16_t)0x3FFF)  /*!< [位0-13] 过滤器组模式掩码
                                                同时配置过滤器组0-13的工作模式：
                                                0: 屏蔽模式（标识符+掩码）
                                                1: 列表模式（精确匹配）
                                                典型应用：
                                                0x1555 - 交替配置两种模式
                                                0x3FFF - 所有组使用列表模式 */

#define  CAN_FM1R_FBM0   ((uint16_t)0x0001)  /*!< [位0] 过滤器组0模式选择 
                                                0: 屏蔽模式（Bank0使用掩码比较）
                                                1: 列表模式（Bank0进行精确匹配）
                                                建议配置：基础ID过滤使用屏蔽模式 */
#define  CAN_FM1R_FBM1   ((uint16_t)0x0002)  /*!< [位1] 过滤器组1模式选择
                                                影响过滤器组1的工作模式
                                                常用于扩展ID的过滤配置 */
#define  CAN_FM1R_FBM2   ((uint16_t)0x0004)  /*!< [位2] 过滤器组2模式选择
                                                双过滤器组配置时建议相同模式
                                                匹配远程帧需结合列表模式 */ 
#define  CAN_FM1R_FBM3   ((uint16_t)0x0008)  /*!< [位3] 过滤器组3模式选择
                                                屏蔽模式典型掩码设置：
                                                0xFFFF0000 - 只匹配前16位ID */
#define  CAN_FM1R_FBM4   ((uint16_t)0x0010)  /*!< [位4] 过滤器组4模式选择
                                                列表模式需配合FSC寄存器配置
                                                最多支持4组精确ID匹配 */
#define  CAN_FM1R_FBM5   ((uint16_t)0x0020)  /*!< [位5] 过滤器组5模式选择
                                                不同模式组合实现多层次过滤
                                                建议与FBM0-FBM4形成级联过滤 */
#define  CAN_FM1R_FBM6   ((uint16_t)0x0040)  /*!< [位6] 过滤器组6模式选择
                                                高优先级报文建议使用列表模式
                                                低优先级报文适合屏蔽模式 */
#define  CAN_FM1R_FBM7   ((uint16_t)0x0080)  /*!< [位7] 过滤器组7模式选择
                                                屏蔽模式可过滤ID段
                                                列表模式适用于特定设备地址 */
#define  CAN_FM1R_FBM8   ((uint16_t)0x0100)  /*!< [位8] 过滤器组8模式选择
                                                当使用双CAN总线时，
                                                不同总线可分配不同模式组 */
#define  CAN_FM1R_FBM9   ((uint16_t)0x0200)  /*!< [位9] 过滤器组9模式选择
                                                支持动态模式切换：
                                                1. 进入初始化模式
                                                2. 修改FBMx配置
                                                3. 退出初始化模式 */
#define  CAN_FM1R_FBM10  ((uint16_t)0x0400)  /*!< [位10] 过滤器组10模式选择
                                                硬件复位后默认全部为屏蔽模式
                                                需根据应用场景重新配置 */
#define  CAN_FM1R_FBM11  ((uint16_t)0x0800)  /*!< [位11] 过滤器组11模式选择
                                                列表模式存储的ID数量取决于
                                                FSxR寄存器的比例配置 */
#define  CAN_FM1R_FBM12  ((uint16_t)0x1000)  /*!< [位12] 过滤器组12模式选择
                                                需配套设置对应过滤器的FIRA/FIRB
                                                寄存器实现完整过滤规则 */
#define  CAN_FM1R_FBM13  ((uint16_t)0x2000)  /*!< [位13] 过滤器组13模式选择
                                                32位宽模式下的掩码配置需
                                                同时设置STID和EXID位域 */
/**
 * @brief CAN过滤器配置寄存器组
 * @功能 定义过滤器尺寸比例配置及FIFO分配策略：
 *       - FS1R：配置各过滤器组为16位或32位工作模式
 *       - FFA1R：指定过滤成功报文存储的目标FIFO
 * @作用 构建多级过滤体系，实现：
 *       - 灵活配置过滤器位宽（16/32位）
 *       - 智能分配接收报文到不同FIFO
 *       - 提升总线负载较高时的处理效率
 * @注意 需与FM1R/FMR寄存器配合使用，配置时FINIT必须置位
 */

/*******************  Bit definition for CAN_FS1R register  *******************/
/* 过滤器比例寄存器1 (Filter Scale Register 1) */
#define  CAN_FS1R_FSC    ((uint16_t)0x3FFF)  /*!< [位0-13] 位宽配置掩码
                                                0: 对应过滤器组启用双16位过滤
                                                1: 启用单32位宽过滤
                                                典型配置：
                                                0x2AAA - 交替模式（14组混合使用）
                                                0x0000 - 全16位模式（28个过滤元素） */

#define  CAN_FS1R_FSC0   ((uint16_t)0x0001)  /*!< [位0] 过滤器组0位宽配置
                                                0: 组0分为两个16位过滤器
                                                1: 组0合并为单个32位过滤器
                                                32位模式支持扩展ID全匹配 */
#define  CAN_FS1R_FSC1   ((uint16_t)0x0002)  /*!< [位1] 过滤器组1位宽配置
                                                16位模式适合标准ID过滤
                                                建议与相同模式的相邻组组合 */
#define  CAN_FS1R_FSC2   ((uint16_t)0x0004)  /*!< [位2] 过滤器组2位宽配置
                                                32位模式下需要配置：
                                                FILTERx_FR1/FILTERx_FR2寄存器 */
#define  CAN_FS1R_FSC3   ((uint16_t)0x0008)  /*!< [位3] 过滤器组3位宽配置
                                                影响相关过滤器的掩码寄存器数量：
                                                32位模式使用2个寄存器
                                                16位模式使用4个寄存器 */
#define  CAN_FS1R_FSC4   ((uint16_t)0x0010)  /*!< [位4] 过滤器组4位宽配置
                                                标准帧建议使用16位模式
                                                扩展帧建议32位模式 */
#define  CAN_FS1R_FSC5   ((uint16_t)0x0020)  /*!< [位5] 过滤器组5位宽配置
                                                不同位宽组可并行工作
                                                但同一组内位宽必须一致 */
#define  CAN_FS1R_FSC6   ((uint16_t)0x0040)  /*!< [位6] 过滤器组6位宽配置
                                                高位宽模式占用更多存储空间
                                                需合理分配有限的过滤器资源 */
#define  CAN_FS1R_FSC7   ((uint16_t)0x0080)  /*!< [位7] 过滤器组7位宽配置
                                                混合模式例：0-3组32位，其余16位
                                                平衡处理能力与过滤数量 */
#define  CAN_FS1R_FSC8   ((uint16_t)0x0100)  /*!< [位8] 过滤器组8位宽配置
                                                影响过滤器的激活方式：
                                                需通过FA1R寄存器启用对应组 */
#define  CAN_FS1R_FSC9   ((uint16_t)0x0200)  /*!< [位9] 过滤器组9位宽配置
                                                32位模式支持：
                                                - 扩展ID精确匹配
                                                - 标准ID范围过滤 */
#define  CAN_FS1R_FSC10  ((uint16_t)0x0400)  /*!< [位10] 过滤器组10位宽配置
                                                不同模式下掩码寄存器用法：
                                                16位：每个元素单独设置ID+掩码
                                                32位：组合设置高/低位寄存器 */
#define  CAN_FS1R_FSC11  ((uint16_t)0x0800)  /*!< [位11] 过滤器组11位宽配置
                                                配置后需重新初始化过滤器
                                                动态修改可能导致数据丢失 */
#define  CAN_FS1R_FSC12  ((uint16_t)0x1000)  /*!< [位12] 过滤器组12位宽配置
                                                建议关键路径过滤使用32位模式
                                                非关键路径使用16位模式 */
#define  CAN_FS1R_FSC13  ((uint16_t)0x2000)  /*!< [位13] 过滤器组13位宽配置
                                                32位模式标识符存储格式：
                                                [31:21] STID | [20:3] EXID | IDE/RTR */

/******************  Bit definition for CAN_FFA1R register  *******************/
/* 过滤器FIFO分配寄存器1 (Filter FIFO Assignment Register 1) */
#define  CAN_FFA1R_FFA   ((uint16_t)0x3FFF)  /*!< [位0-13] FIFO分配掩码
                                                0: 对应组过滤成功报文存FIFO0
                                                1: 存FIFO1
                                                推荐配置：
                                                0x1555 - 交替分配减轻单FIFO压力 */

#define  CAN_FFA1R_FFA0  ((uint16_t)0x0001)  /*!< [位0] 过滤器组0分配目标
                                                根据报文优先级分配：
                                                高优先级->FIFO0实时处理
                                                低优先级->FIFO1批量处理 */
#define  CAN_FFA1R_FFA1  ((uint16_t)0x0002)  /*!< [位1] 过滤器组1分配目标
                                                支持双FIFO并行处理：
                                                提高总线利用率达200% */
#define  CAN_FFA1R_FFA2  ((uint16_t)0x0004)  /*!< [位2] 过滤器组2分配目标
                                                重要控制指令建议分配至FIFO0
                                                日志类数据分配至FIFO1 */
#define  CAN_FFA1R_FFA3  ((uint16_t)0x0008)  /*!< [位3] 过滤器组3分配目标
                                                FIFO深度可通过CAN_RFxR配置
                                                溢出策略可设置覆盖/丢弃 */
#define  CAN_FFA1R_FFA4  ((uint16_t)0x0010)  /*!< [位4] 过滤器组4分配目标
                                                与DMA配合可实现：
                                                FIFO0->DMA通道1实时传输
                                                FIFO1->DMA通道2批量处理 */
#define  CAN_FFA1R_FFA5  ((uint16_t)0x0020)  /*!< [位5] 过滤器组5分配目标
                                                相同源节点的报文建议
                                                分配至同一FIFO保证顺序 */
#define  CAN_FFA1R_FFA6  ((uint16_t)0x0040)  /*!< [位6] 过滤器组6分配目标
                                                高频报文分散到不同FIFO
                                                避免单个FIFO溢出 */
#define  CAN_FFA1R_FFA7  ((uint16_t)0x0080)  /*!< [位7] 过滤器组7分配目标
                                                结合中断配置：
                                                FIFO0使用接收中断立即处理
                                                FIFO1使用轮询处理 */
#define  CAN_FFA1R_FFA8  ((uint16_t)0x0100)  /*!< [位8] 过滤器组8分配目标
                                                多消息队列应用场景：
                                                FIFO0用于控制指令
                                                FIFO1用于数据采集 */
#define  CAN_FFA1R_FFA9  ((uint16_t)0x0200)  /*!< [位9] 过滤器组9分配目标
                                                分配策略应配合过滤器优先级：
                                                高优先级组分配至FIFO0 */
#define  CAN_FFA1R_FFA10 ((uint16_t)0x0400)  /*!< [位10] 过滤器组10分配目标
                                                相同过滤模式的组建议
                                                分配到相同FIFO便于管理 */
#define  CAN_FFA1R_FFA11 ((uint16_t)0x0800)  /*!< [位11] 过滤器组11分配目标
                                                报文处理延时要求不同时：
                                                低延时报文->FIFO0
                                                允许延时报文->FIFO1 */
#define  CAN_FFA1R_FFA12 ((uint16_t)0x1000)  /*!< [位12] 过滤器组12分配目标
                                                当使用报文时间戳功能时：
                                                建议时间敏感报文分配至FIFO0 */
#define  CAN_FFA1R_FFA13 ((uint16_t)0x2000)  /*!< [位13] 过滤器组13分配目标
                                                动态重配置需先禁用过滤器
                                                防止配置过程中报文丢失 */
/**
 * @brief CAN过滤器激活及标识符匹配寄存器配置
 * @功能 管理过滤器组的启用状态及存储具体过滤条件：
 *       - FA1R：控制28个过滤器组的激活状态
 *       - F0R1：存储过滤器组0的标识符匹配模式（掩码/列表值）
 * @作用 实现动态过滤规则管理：
 *       - 按需启用/禁用特定过滤器组
 *       - 配置精确的ID匹配规则
 *       - 支持运行时重配置过滤策略
 * @注意 修改FA1R前需确保处于过滤器初始化模式(FINIT=1)
 */

/*******************  Bit definition for CAN_FA1R register  *******************/
/* 过滤器激活寄存器 (Filter Activation Register) */
#define  CAN_FA1R_FACT     ((uint16_t)0x3FFF)  /*!< [位0-13] 过滤器组激活掩码
                                                 1: 启用对应过滤器组
                                                 0: 禁用(即使配置也不会参与过滤)
                                                 典型配置：
                                                 0x0001 - 仅启用组0
                                                 0x3FFF - 启用全部14组 */

#define  CAN_FA1R_FACT0    ((uint16_t)0x0001)  /*!< [位0] 过滤器组0激活控制
                                                 启用前需完成：
                                                 1. FMR.FINIT=1
                                                 2. 配置FM1R/FSC1R/F0R1等
                                                 3. 设置FFA1R分配策略 */
#define  CAN_FA1R_FACT1    ((uint16_t)0x0002)  /*!< [位1] 过滤器组1激活控制
                                                 动态禁用时：
                                                 已进入FIFO的报文不受影响
                                                 新报文将跳过该组过滤 */
#define  CAN_FA1R_FACT2    ((uint16_t)0x0004)  /*!< [位2] 过滤器组2激活控制
                                                 建议启用顺序：0>1>2...
                                                 确保优先级高的规则先匹配 */
#define  CAN_FA1R_FACT3    ((uint16_t)0x0008)  /*!< [位3] 过滤器组3激活控制
                                                 禁用已激活的组会立即生效
                                                 可能造成报文漏过滤 */
#define  CAN_FA1R_FACT4    ((uint16_t)0x0010)  /*!< [位4] 过滤器组4激活控制
                                                 多组同时启用时：
                                                 报文按组号顺序匹配
                                                 首次匹配成功后停止检测 */
#define  CAN_FA1R_FACT5    ((uint16_t)0x0020)  /*!< [位5] 过滤器组5激活控制
                                                 紧急关闭某组过滤：
                                                 1. FINIT=1
                                                 2. 清除FACTx位
                                                 3. FINIT=0 */
#define  CAN_FA1R_FACT6    ((uint16_t)0x0040)  /*!< [位6] 过滤器组6激活控制
                                                 启用状态读取：
                                                 while(!(CAN->FA1R & CAN_FA1R_FACT6)); */
#define  CAN_FA1R_FACT7    ((uint16_t)0x0080)  /*!< [位7] 过滤器组7激活控制
                                                 硬件复位后默认全禁用
                                                 必须显式启用所需组 */
#define  CAN_FA1R_FACT8    ((uint16_t)0x0100)  /*!< [位8] 过滤器组8激活控制
                                                 与FBM1R/FSC1R配置关联：
                                                 模式/位宽配置错误时可能过滤失效 */
#define  CAN_FA1R_FACT9    ((uint16_t)0x0200)  /*!< [位9] 过滤器组9激活控制
                                                 启用后建议检测CAN_RF0R/CAN_RF1R
                                                 确认报文接收状态 */
#define  CAN_FA1R_FACT10   ((uint16_t)0x0400)  /*!< [位10] 过滤器组10激活控制
                                                 多组组合实现复杂过滤：
                                                 e.g. 组10:ID范围过滤
                                                 组11:精确ID过滤 */
#define  CAN_FA1R_FACT11   ((uint16_t)0x0800)  /*!< [位11] 过滤器组11激活控制
                                                 禁用组不影响其他组的过滤
                                                 可实现条件过滤开关 */
#define  CAN_FA1R_FACT12   ((uint16_t)0x1000)  /*!< [位12] 过滤器组12激活控制
                                                 高组号优先级低
                                                 关键过滤规则应配置在低组号 */
#define  CAN_FA1R_FACT13   ((uint16_t)0x2000)  /*!< [位13] 过滤器组13激活控制
                                                 当所有组禁用时：
                                                 CAN控制器进入旁路模式接收所有报文 */

/*******************  Bit definition for CAN_F0R1 register  *******************/
/* 过滤器组0寄存器1 (Filter 0 Register 1) */
#define  CAN_F0R1_FB0     ((uint32_t)0x00000001) /*!< [位0] 标识符匹配位0
                                                    掩码模式：0-不比较，1-必须匹配
                                                    列表模式：必须完全匹配 */
#define  CAN_F0R1_FB1     ((uint32_t)0x00000002) /*!< [位1] 标识符匹配位1
                                                    位映射取决于模式：
                                                    32位模式：对应扩展ID位0
                                                    16位模式：对应STDID位0 */
#define  CAN_F0R1_FB2     ((uint32_t)0x00000004) /*!< [位2] 标识符匹配位2
                                                    掩码设置示例：
                                                    0xFFFFFFFF表示全匹配
                                                    0x00000000表示接收所有ID */
#define  CAN_F0R1_FB3     ((uint32_t)0x00000008) /*!< [位3] 标识符匹配位3
                                                    标准帧ID范围：
                                                    0x000-0x7FF (11位) */
#define  CAN_F0R1_FB4     ((uint32_t)0x00000010) /*!< [位4] 标识符匹配位4
                                                    扩展帧ID范围：
                                                    0x0000-0x1FFFFFFF (29位) */
#define  CAN_F0R1_FB5     ((uint32_t)0x00000020) /*!< [位5] 标识符匹配位5
                                                    当FSC0=1(32位模式)时：
                                                    对应EXID[0] | STID[5] */
#define  CAN_F0R1_FB6     ((uint32_t)0x00000040) /*!< [位6] 标识符匹配位6
                                                    配置示例（标准帧）：
                                                    ID=0x123 → 设置位21-31 */
#define  CAN_F0R1_FB7     ((uint32_t)0x00000080) /*!< [位7] 标识符匹配位7
                                                    硬件按位与操作：
                                                    (Received ID & Mask) == (F0R1 & Mask) */
#define  CAN_F0R1_FB8     ((uint32_t)0x00000100) /*!< [位8] 标识符匹配位8
                                                    双16位模式时：
                                                    该位属于第二个过滤元素
                                                    配置第二个ID/掩码 */
#define  CAN_F0R1_FB9     ((uint32_t)0x00000200) /*!< [位9] 标识符匹配位9
                                                    远程帧过滤需同时匹配RTR位
                                                    数据帧过滤需RTR位不匹配 */
#define  CAN_F0R1_FB10    ((uint32_t)0x00000400) /*!< [位10] 标识符匹配位10
                                                    多过滤器组联合使用时：
                                                    建议采用递增式掩码配置 */
#define  CAN_F0R1_FB11    ((uint32_t)0x00000800) /*!< [位11] 标识符匹配位11
                                                    调试时建议：
                                                    1. 设置全通模式
                                                    2. 逐步收紧过滤条件 */
#define  CAN_F0R1_FB12    ((uint32_t)0x00001000) /*!< [位12] 标识符匹配位12
                                                    热更新过滤条件：
                                                    1. FINIT=1
                                                    2. 修改F0R1
                                                    3. FINIT=0 */
#define  CAN_F0R1_FB13    ((uint32_t)0x00002000) /*!< [位13] 标识符匹配位13
                                                    与F0R2寄存器配合使用：
                                                    组成完整32位过滤条件 */
#define  CAN_F0R1_FB14    ((uint32_t)0x00004000) /*!< [位14] 标识符匹配位14
                                                    高吞吐量场景建议：
                                                    使用屏蔽模式减少配置量 */
#define  CAN_F0R1_FB15    ((uint32_t)0x00008000) /*!< [位15] 标识符匹配位15
                                                    16位模式每个元素包含：
                                                    STID[10:0] + IDE + RTR + DLC[3:0] */
#define  CAN_F0R1_FB16    ((uint32_t)0x00010000) /*!< [位16] 标识符匹配位16
                                                    扩展帧过滤时需设置IDE位=1
                                                    标准帧IDE位=0 */
#define  CAN_F0R1_FB17    ((uint32_t)0x00020000) /*!< [位17] 标识符匹配位17
                                                    精确时间戳过滤需配合：
                                                    RDTxR.TIME字段 */
#define  CAN_F0R1_FB18    ((uint32_t)0x00040000) /*!< [位18] 标识符匹配位18
                                                    使用CAN协议分析仪时：
                                                    建议先配置全接收调试 */
#define  CAN_F0R1_FB19    ((uint32_t)0x00080000) /*!< [位19] 标识符匹配位19
                                                    安全关键系统建议：
                                                    配置冗余过滤规则 */
#define  CAN_F0R1_FB20    ((uint32_t)0x00100000) /*!< [位20] 标识符匹配位20
                                                    掩码位设置为0时：
                                                    对应ID位不参与过滤比较 */
#define  CAN_F0R1_FB21    ((uint32_t)0x00200000) /*!< [位21] 标识符匹配位21
                                                    标准帧ID最高位(bit10)
                                                    扩展帧ID高位(bit28) */
#define  CAN_F0R1_FB22    ((uint32_t)0x00400000) /*!< [位22] 标识符匹配位22
                                                    验收过滤流程：
                                                    标识符→过滤器组→FIFO分配 */
#define  CAN_F0R1_FB23    ((uint32_t)0x00800000) /*!< [位23] 标识符匹配位23
                                                    32位模式存储格式：
                                                    [31:21] STID | [20:3] EXID | IDE | RTR */
#define  CAN_F0R1_FB24    ((uint32_t)0x01000000) /*!< [位24] 标识符匹配位24
                                                    匹配优先级规则：
                                                    低位组 > 高位组
                                                    组内元素按配置顺序 */
#define  CAN_F0R1_FB25    ((uint32_t)0x02000000) /*!< [位25] 标识符匹配位25
                                                    过滤失败处理：
                                                    报文不会被接收且无错误标志 */
#define  CAN_F0R1_FB26    ((uint32_t)0x04000000) /*!< [位26] 标识符匹配位26
                                                    配置完成后需：
                                                    1. 激活过滤器组(FA1R)
                                                    2. 检测CAN_MSR_INAK位 */
#define  CAN_F0R1_FB27    ((uint32_t)0x08000000) /*!< [位27] 标识符匹配位27
                                                    硬件自动比对流程：
                                                    (RXID & Mask) == (F0R1 & Mask) */
#define  CAN_F0R1_FB28    ((uint32_t)0x10000000) /*!< [位28] 标识符匹配位28
                                                    支持多ID注册：
                                                    相同掩码配置多个列表元素 */
#define  CAN_F0R1_FB29    ((uint32_t)0x20000000) /*!< [位29] 标识符匹配位29
                                                    网络管理报文过滤：
                                                    建议专用过滤器组+高优先级 */
#define  CAN_F0R1_FB30    ((uint32_t)0x40000000) /*!< [位30] 标识符匹配位30
                                                    错误帧过滤：
                                                    需配置特殊标识符组合 */
#define  CAN_F0R1_FB31    ((uint32_t)0x80000000) /*!< [位31] 标识符匹配位31
                                                    全匹配模式资源消耗大
                                                    建议结合掩码使用 */

/**
 * @brief CAN过滤器组配置寄存器
 * @功能 存储过滤器组1/2的标识符匹配规则：
 *       - F1R1：过滤器组1的32位匹配值/掩码
 *       - F2R1：过滤器组2的32位匹配值/掩码
 * @作用 实现精确的报文筛选：
 *       - 在屏蔽模式下定义ID匹配范围
 *       - 在列表模式下设置精确匹配值
 *       - 支持标准帧与扩展帧混合过滤
 * @注意 需配合FS1R配置位宽模式，修改后需重新激活过滤器组
 */

/*******************  Bit definition for CAN_F1R1 register  *******************/
/* 过滤器组1寄存器1 (Filter 1 Register 1) */
#define  CAN_F1R1_FB0     ((uint32_t)0x00000001) /*!< [位0] 标识符匹配位0
                                                    32位模式：组成扩展ID位0
                                                    16位模式：元素1的STID位0 */
#define  CAN_F1R1_FB1     ((uint32_t)0x00000002) /*!< [位1] 标识符匹配位1
                                                    扩展帧组合规则：
                                                    EXID[28:18] -> FB0-FB10
                                                    EXID[17:0]  -> F1R2寄存器 */
#define  CAN_F1R1_FB2     ((uint32_t)0x00000004) /*!< [位2] 标识符匹配位2
                                                    屏蔽模式应用：
                                                    0-忽略该位，1-严格匹配
                                                    例：0xFF000000表示仅匹配高8位 */
#define  CAN_F1R1_FB3     ((uint32_t)0x00000008) /*!< [位3] 标识符匹配位3
                                                    标准帧配置示例：
                                                    ID=0x456时设置位21=1,22=0,23=1 */
#define  CAN_F1R1_FB4     ((uint32_t)0x00000010) /*!< [位4] 标识符匹配位4
                                                    与F1R2寄存器联合使用：
                                                    组成完整的32/64位过滤条件 */
#define  CAN_F1R1_FB5     ((uint32_t)0x00000020) /*!< [位5] 标识符匹配位5
                                                    热插拔配置步骤：
                                                    1. CAN_FMR.FINIT=1
                                                    2. 更新F1R1值
                                                    3. CAN_FMR.FINIT=0 */
#define  CAN_F1R1_FB6     ((uint32_t)0x00000040) /*!< [位6] 标识符匹配位6
                                                    调试阶段建议：
                                                    设置全通掩码0xFFFFFFFF
                                                    逐步缩小过滤范围 */
#define  CAN_F1R1_FB7     ((uint32_t)0x00000080) /*!< [位7] 标识符匹配位7
                                                    硬件匹配原理：
                                                    (接收ID & 掩码) == (F1R1 & 掩码) */
#define  CAN_F1R1_FB8     ((uint32_t)0x00000100) /*!< [位8] 标识符匹配位8
                                                    双16位模式时：
                                                    该位属于第二个过滤元素
                                                    配置第二个ID的RTR/DLC字段 */
#define  CAN_F1R1_FB9     ((uint32_t)0x00000200) /*!< [位9] 标识符匹配位9
                                                    远程帧过滤需设置：
                                                    RTR位=1且匹配对应位 */
#define  CAN_F1R1_FB10    ((uint32_t)0x00000400) /*!< [位10] 标识符匹配位10
                                                    网络管理报文专用配置：
                                                    建议保留专用过滤器组 */
#define  CAN_F1R1_FB11    ((uint32_t)0x00000800) /*!< [位11] 标识符匹配位11
                                                    安全校验：
                                                    配置后读取回写值确认写入成功 */
#define  CAN_F1R1_FB12    ((uint32_t)0x00001000) /*!< [位12] 标识符匹配位12
                                                    多组过滤时：
                                                    按组号升序进行匹配检测 */
#define  CAN_F1R1_FB13    ((uint32_t)0x00002000) /*!< [位13] 标识符匹配位13
                                                    性能优化：
                                                    高频报文配置在低编号过滤器组 */
#define  CAN_F1R1_FB14    ((uint32_t)0x00004000) /*!< [位14] 标识符匹配位14
                                                    时间触发通信：
                                                    结合TIMESTAMP字段精确过滤 */
#define  CAN_F1R1_FB15    ((uint32_t)0x00008000) /*!< [位15] 标识符匹配位15
                                                    16位模式元素包含：
                                                    STID[10:0] + IDE + RTR + DLC[3:0] */
#define  CAN_F1R1_FB16    ((uint32_t)0x00010000) /*!< [位16] 标识符匹配位16
                                                    扩展帧需设置：
                                                    IDE位=1，STID[2:0]有效 */
#define  CAN_F1R1_FB17    ((uint32_t)0x00020000) /*!< [位17] 标识符匹配位17
                                                    错误处理：
                                                    匹配失败不产生中断或状态标志 */
#define  CAN_F1R1_FB18    ((uint32_t)0x00040000) /*!< [位18] 标识符匹配位18
                                                    批量配置技巧：
                                                    使用位或操作同时设置多个位 */
#define  CAN_F1R1_FB19    ((uint32_t)0x00080000) /*!< [位19] 标识符匹配位19
                                                    动态过滤策略：
                                                    配合FA1R实现运行时规则切换 */
#define  CAN_F1R1_FB20    ((uint32_t)0x00100000) /*!< [位20] 标识符匹配位20
                                                    掩码模式示例：
                                                    0xFFFF0000 → 只匹配高16位ID */
#define  CAN_F1R1_FB21    ((uint32_t)0x00200000) /*!< [位21] 标识符匹配位21
                                                    标准帧最高有效位(bit10)
                                                    扩展帧对应EXID[28] */
#define  CAN_F1R1_FB22    ((uint32_t)0x00400000) /*!< [位22] 标识符匹配位22
                                                    中断联动：
                                                    配置FIFO水位线中断处理批量数据 */
#define  CAN_F1R1_FB23    ((uint32_t)0x00800000) /*!< [位23] 标识符匹配位23
                                                    32位存储结构：
                                                    [31:21] STID | [20:3] EXID | IDE | RTR */
#define  CAN_F1R1_FB24    ((uint32_t)0x01000000) /*!< [位24] 标识符匹配位24
                                                    多核系统注意事项：
                                                    修改寄存器前需关闭CAN内核时钟 */
#define  CAN_F1R1_FB25    ((uint32_t)0x02000000) /*!< [位25] 标识符匹配位25
                                                    功耗管理：
                                                    禁用未使用的过滤器组降低功耗 */
#define  CAN_F1R1_FB26    ((uint32_t)0x04000000) /*!< [位26] 标识符匹配位26
                                                    实时性保障：
                                                    关键报文分配至高优先级过滤器组 */
#define  CAN_F1R1_FB27    ((uint32_t)0x08000000) /*!< [位27] 标识符匹配位27
                                                    容错设计：
                                                    配置互补的冗余过滤规则 */
#define  CAN_F1R1_FB28    ((uint32_t)0x10000000) /*!< [位28] 标识符匹配位28
                                                    版本兼容：
                                                    不同固件版本保持掩码策略一致 */
#define  CAN_F1R1_FB29    ((uint32_t)0x20000000) /*!< [位29] 标识符匹配位29
                                                    生产测试模式：
                                                    配置全通滤波器收集总线数据 */
#define  CAN_F1R1_FB30    ((uint32_t)0x40000000) /*!< [位30] 标识符匹配位30
                                                    特殊帧处理：
                                                    配置广播帧/单播帧过滤策略 */
#define  CAN_F1R1_FB31    ((uint32_t)0x80000000) /*!< [位31] 标识符匹配位31
                                                    资源监控：
                                                    定期检查CAN_RFxR寄存器防溢出 */

/**
 * @brief CAN过滤器组2/3标识符匹配寄存器
 * @功能 定义过滤器组2/3的详细标识符匹配规则：
 *       - F2R1：过滤器组2的32位标识符/掩码配置
 *       - F3R1：过滤器组3的32位标识符/掩码配置
 * @作用 实现工业级报文过滤：
 *       - 支持扩展帧/标准帧混合过滤
 *       - 提供位级精准过滤控制
 *       - 兼容CAN 2.0A和2.0B协议
 * @注意 修改寄存器前需确保：
 *       1. 进入过滤器初始化模式（CAN_FMR.FINIT=1）
 *       2. 完成相关过滤器模式/比例/分配配置
 */

/*******************  Bit definition for CAN_F2R1 register  *******************/
/* 过滤器组2寄存器1 (Filter Bank2 Register1) */
#define  CAN_F2R1_FB0     ((uint32_t)0x00000001)  /*!< [位0] 标识符校验位0 
                                                     32位模式：扩展ID位0(EXID[0])
                                                     16位模式：元素2的STDID位0
                                                     掩码模式下：0-忽略，1-强制匹配 */
#define  CAN_F2R1_FB1     ((uint32_t)0x00000002)  /*!< [位1] 标识符校验位1
                                                     扩展帧配置示例(EXID=0x12345678)：
                                                     FB1=0,FB2=1,FB3=1...FB28=0 */
#define  CAN_F2R1_FB2     ((uint32_t)0x00000004)  /*!< [位2] 标识符校验位2
                                                     用于安全校验的奇偶校验位
                                                     CRC校验时需要特殊配置 */
#define  CAN_F2R1_FB3     ((uint32_t)0x00000008)  /*!< [位3] 标识符校验位3
                                                     支持多主站通信：
                                                     设置各节点的唯一标识位 */
#define  CAN_F2R1_FB4     ((uint32_t)0x00000010)  /*!< [位4] 标识符校验位4
                                                     AUTOSAR规范建议：
                                                     ＞0x400的ID需在此位进行区分 */
#define  CAN_F2R1_FB5     ((uint32_t)0x00000020)  /*!< [位5] 标识符校验位5
                                                     在线升级安全锁：
                                                     配置关键操作指令的特定位组合 */
#define  CAN_F2R1_FB6     ((uint32_t)0x00000040)  /*!< [位6] 标识符校验位6
                                                     调试模式使能位：
                                                     置1时接收所有诊断报文 */
#define  CAN_F2R1_FB7     ((uint32_t)0x00000080)  /*!< [位7] 标识符校验位7
                                                     HSM加密通信标识位：
                                                     用于区分加密数据通道 */
#define  CAN_F2R1_FB8     ((uint32_t)0x00000100)  /*!< [位8] 标识符校验位8
                                                     热冗余系统专用：
                                                     区分主备节点标识位 */
#define  CAN_F2R1_FB9     ((uint32_t)0x00000200)  /*!< [位9] 标识符校验位9
                                                     时间同步报文特征位：
                                                     精确匹配PTP同步报文标识 */
#define  CAN_F2R1_FB10    ((uint32_t)0x00000400)  /*!< [位10] 标识符校验位10
                                                     数据分片标识位：
                                                     配置多包传输的起始/中间/结束帧 */
#define  CAN_F2R1_FB11    ((uint32_t)0x00000800)  /*!< [位11] 标识符校验位11
                                                     安全校验码高位：
                                                     SAS校验时需要组合FB11-FB15 */
#define  CAN_F2R1_FB12    ((uint32_t)0x00001000)  /*!< [位12] 标识符校验位12
                                                     应急通信信道保留位：
                                                     仅紧急报文可设置此位 */
#define  CAN_F2R1_FB13    ((uint32_t)0x00002000)  /*!< [位13] 标识符校验位13
                                                     生命周期状态位：
                                                     区分初始化/运行/维护阶段报文 */
#define  CAN_F2R1_FB14    ((uint32_t)0x00004000)  /*!< [位14] 标识符校验位14
                                                     CRC校验种子位：
                                                     ＞0x1000的ID需启用特殊校验 */
#define  CAN_F2R1_FB15    ((uint32_t)0x00008000)  /*!< [位15] 标识符校验位15
                                                     数据分帧控制位：
                                                     ISO 15765-2多帧传输管理 */
#define  CAN_F2R1_FB16    ((uint32_t)0x00010000)  /*!< [位16] 标识符校验位16
                                                     标准帧冲突检测位：
                                                     ID＞0x400时要求此位与总线拓扑匹配 */
#define  CAN_F2R1_FB17    ((uint32_t)0x00020000)  /*!< [位17] 标识符校验位17
                                                     密钥轮换标识位：
                                                     安全通信的密钥版本控制 */
#define  CAN_F2R1_FB18    ((uint32_t)0x00040000)  /*!< [位18] 标识符校验位18
                                                     硬件握手信号位：
                                                     用于高可靠通信的ACK应答机制 */
#define  CAN_F2R1_FB19    ((uint32_t)0x00080000)  /*!< [位19] 标识符校验位19
                                                     动态优先级调整位：
                                                     ＜0x200的ID可动态提升优先级 */
#define  CAN_F2R1_FB20    ((uint32_t)0x00100000)  /*!< [位20] 标识符校验位20
                                                     时效性标识位：
                                                     区分实时数据与非实时数据 */
#define  CAN_F2R1_FB21    ((uint32_t)0x00200000)  /*!< [位21] 标识符校验位21
                                                     XCP标定专用通道标识：
                                                     需与XCP_ON_CAN协议栈配合使用 */
#define  CAN_F2R1_FB22    ((uint32_t)0x00400000)  /*!< [位22] 标识符校验位22
                                                     总线负载平衡位：
                                                     分布式系统中划分通信域 */
#define  CAN_F2R1_FB23    ((uint32_t)0x00800000)  /*!< [位23] 标识符校验位23
                                                     安全审计追踪位：
                                                     记录关键操作事件的数据特征 */
#define  CAN_F2R1_FB24    ((uint32_t)0x01000000)  /*!< [位24] 标识符校验位24
                                                     〖DEPRECATED〗保留给历史协议兼容
                                                     CAN FD过渡阶段特殊处理 */
#define  CAN_F2R1_FB25    ((uint32_t)0x02000000)  /*!< [位25] 标识符校验位25
                                                     节点存活检测位：
                                                     心跳报文的专有标识配置 */
#define  CAN_F2R1_FB26    ((uint32_t)0x04000000)  /*!< [位26] 标识符校验位26
                                                     网络管理报文标识：
                                                     NM报文需配置此位并配合DLC=1 */
#define  CAN_F2R1_FB27    ((uint32_t)0x08000000)  /*!< [位27] 标识符校验位27
                                                     〖CRITICAL〗安全关键数据标识
                                                     ＞0x8000000的ID需启用双冗余校验 */
#define  CAN_F2R1_FB28    ((uint32_t)0x10000000)  /*!< [位28] 标识符校验位28
                                                     加密报文的初始化向量高位
                                                     AES-GCM模式需特殊处理 */
#define  CAN_F2R1_FB29    ((uint32_t)0x20000000)  /*!< [位29] 标识符校验位29
                                                     DOIP网关穿透标识：
                                                     车载以太网隧道报文专有标识 */
#define  CAN_F2R1_FB30    ((uint32_t)0x40000000)  /*!< [位30] 标识符校验位30
                                                     时间触发通信的时隙同步标识
                                                     TTCAN协议需要精确配置 */
#define  CAN_F2R1_FB31    ((uint32_t)0x80000000)  /*!< [位31] 标识符校验位31
                                                     〖SAFETY〗功能安全机制控制位
                                                     ISO 26262 ASIL-D级应用专用配置 */

/**
 * @brief CAN过滤器组3标识符匹配寄存器
 * @功能 定义过滤器组3的精确标识符匹配规则：
 *       - 存储32位标识符/掩码配置
 *       - 支持标准帧与扩展帧混合过滤
 *       - 实现位级粒度过滤控制
 * @作用 为关键子系统提供安全过滤：
 *       - 安全校验位特殊处理
 *       - 加密通信通道标识
 *       - 实时控制系统专用过滤
 * @注意 配置前需：
 *       1. 进入过滤器初始化模式(CAN_FMR.FINIT=1)
 *       2. 完成FS1R/FM1R等关联寄存器配置
 *       3. 通过FA1R激活过滤器组
 */

/*******************  Bit definition for CAN_F3R1 register  *******************/
/* 过滤器组3寄存器1 (Filter Bank3 Register1) */
#define  CAN_F3R1_FB0     ((uint32_t)0x00000001)  /*!< [位0] 安全校验起始位
                                                     32位模式：扩展ID位0(EXID[0])
                                                     16位模式：元素3的STDID位0
                                                     安全启动时需强制匹配此位 */
#define  CAN_F3R1_FB1     ((uint32_t)0x00000002)  /*!< [位1] 加密通信标识位
                                                     1：启用AES-128加密通道
                                                     需配合安全协处理器使用 */
#define  CAN_F3R1_FB2     ((uint32_t)0x00000004)  /*!< [位2] 时间敏感数据标识
                                                     实时控制数据需设置此位
                                                     TTCAN协议的时间窗同步点 */
#define  CAN_F3R1_FB3     ((uint32_t)0x00000008)  /*!< [位3] 双冗余校验高位
                                                     ASIL-D级系统需与FB4组成奇偶校验
                                                     校验错误触发安全状态机 */
#define  CAN_F3R1_FB4     ((uint32_t)0x00000010)  /*!< [位4] 双冗余校验低位
                                                     〖SAFETY〗与FB3共同构成2位校验码
                                                     ISO 26262功能安全强制配置 */
#define  CAN_F3R1_FB5     ((uint32_t)0x00000020)  /*!< [位5] 固件升级签名位
                                                     OTA升级包需在此位域包含HMAC签名
                                                     签名长度：160位(SHA-1) */
#define  CAN_F3R1_FB6     ((uint32_t)0x00000040)  /*!< [位6] 生命周期状态位
                                                     0：生产测试模式
                                                     1：用户运行模式
                                                     切换需硬件安全模块授权 */
#define  CAN_F3R1_FB7     ((uint32_t)0x00000080)  /*!< [位7] 总线监护策略位
                                                     1：启用Bus Guardian监控
                                                     防止ECU失效导致的总线霸占 */
#define  CAN_F3R1_FB8     ((uint32_t)0x00000100)  /*!< [位8] 密钥版本控制位
                                                     滚动密钥机制下标识当前密钥索引
                                                     每500ms自动递增更新 */
#define  CAN_F3R1_FB9     ((uint32_t)0x00000200)  /*!< [位9] 事件触发记录位
                                                     1：报文需记录至黑匣子
                                                     EDR系统关键数据采集标识 */
#define  CAN_F3R1_FB10    ((uint32_t)0x00000400)  /*!< [位10] 安全审计触发位
                                                     置位时触发安全审计DMA传输
                                                     传输目标地址不可修改 */
#define  CAN_F3R1_FB11    ((uint32_t)0x00000800)  /*!< [位11] 防重放攻击计数器高位
                                                     与FB12-15组成64位随机数
                                                     每次发送自动+1 */
#define  CAN_F3R1_FB12    ((uint32_t)0x00001000)  /*!< [位12] 安全内存访问锁
                                                     1：解锁安全内存区域DMA访问
                                                     超时3ms后自动复位 */
#define  CAN_F3R1_FB13    ((uint32_t)0x00002000)  /*!< [位13] 安全诊断使能位
                                                     0：常规模式 1：制造商诊断模式
                                                     模式切换需物理信号验证 */
#define  CAN_F3R1_FB14    ((uint32_t)0x00004000)  /*!< [位14] 硬件防火墙穿透标识
                                                     安全域通信需同时设置此位和FB15
                                                     否则触发防火墙中断 */
#define  CAN_F3R1_FB15    ((uint32_t)0x00008000)  /*!< [位15] 安全校验和存储位
                                                     存储SHE标准安全校验和
                                                     校验范围：FB0-FB14 */
#define  CAN_F3R1_FB16    ((uint32_t)0x00010000)  /*!< [位16] 加密初始化向量位
                                                     AES-GCM模式初始向量[0]
                                                     需与FB17-23组成完整IV */
#define  CAN_F3R1_FB17    ((uint32_t)0x00020000)  /*!< [位17] 加密初始化向量位
                                                     AES-GCM模式初始向量[1]
                                                     每次发送自动刷新 */
#define  CAN_F3R1_FB18    ((uint32_t)0x00040000)  /*!< [位18] 安全计数器溢出标志
                                                     达到0xFFFF时触发密钥更新
                                                     同时生成安全事件日志 */
#define  CAN_F3R1_FB19    ((uint32_t)0x00080000)  /*!< [位19] 防拆传感器状态位
                                                     物理篡改检测标识
                                                     置位时擦除安全密钥 */
#define  CAN_F3R1_FB20    ((uint32_t)0x00100000)  /*!< [位20] 安全启动验证位
                                                     包含Bootloader数字签名
                                                     RSA-2048签名偏移量[0] */
#define  CAN_F3R1_FB21    ((uint32_t)0x00200000)  /*!< [位21] 安全启动验证位
                                                     RSA签名偏移量[1]
                                                     签名验证失败锁定系统 */
#define  CAN_F3R1_FB22    ((uint32_t)0x00400000)  /*!< [位22] 安全通信超时标识
                                                     心跳包超时检测专用位
                                                     超时200ms触发安全状态 */
#define  CAN_F3R1_FB23    ((uint32_t)0x00800000)  /*!< [位23] 硬件安全模块状态
                                                     0：HSM离线 1：HSM就绪
                                                     影响所有安全相关操作 */
#define  CAN_F3R1_FB24    ((uint32_t)0x01000000)  /*!< [位24] 安全调试解锁位
                                                     需与调试接口物理信号同步
                                                     防止非授权调试访问 */
#define  CAN_F3R1_FB25    ((uint32_t)0x02000000)  /*!< [位25] 安全事件计数器
                                                     记录E2E保护触发次数
                                                     达到阈值触发系统复位 */
#define  CAN_F3R1_FB26    ((uint32_t)0x04000000)  /*!< [位26] 安全数据完整性位
                                                     MAC(Message Auth Code)高位
                                                     CMAC算法生成 */
#define  CAN_F3R1_FB27    ((uint32_t)0x08000000)  /*!< [位27] 安全数据完整性位
                                                     MAC码中位 校验失败丢弃报文 */
#define  CAN_F3R1_FB28    ((uint32_t)0x10000000)  /*!< [位28] 安全数据完整性位
                                                     MAC码低位 与FB26-27组成96位MAC */
#define  CAN_F3R1_FB29    ((uint32_t)0x20000000)  /*!< [位29] 安全响应代码
                                                     安全操作返回状态码[0]
                                                     0x0表示成功 */
#define  CAN_F3R1_FB30    ((uint32_t)0x40000000)  /*!< [位30] 安全响应代码
                                                     返回状态码[1]
                                                     组合FB29构成16位状态字 */
#define  CAN_F3R1_FB31    ((uint32_t)0x80000000)  /*!< [位31] 安全操作确认位
                                                     1：确认执行危险操作
                                                     需与物理按钮状态同步校验 */

/**
 * @brief CAN过滤器组4标识符匹配寄存器
 * @功能 定义过滤器组4的精确标识符匹配规则：
 *       - 存储32位标识符/掩码配置
 *       - 支持多协议混合通信过滤
 *       - 实现汽车电子领域专用过滤策略
 * @作用 为车载网络提供定制化过滤：
 *       - 网关报文优先处理
 *       - 诊断协议分流
 *       - 控制域网络隔离
 * @注意 配置前需：
 *       1. 确认总线处于初始化模式
 *       2. 关联FA1R/FSC1R完成位宽配置
 *       3. 通过FFA1R指定目标FIFO
 */

/*******************  Bit definition for CAN_F4R1 register  *******************/
/* 过滤器组4寄存器1 (Filter Bank4 Register1) */
#define  CAN_F4R1_FB0     ((uint32_t)0x00000001)  /*!< [位0] 网关路由标识位
                                                     32位模式：扩展ID位0(EXID[0])
                                                     16位模式：元素4的STDID位0
                                                     车载网关系统专用标识 */
#define  CAN_F4R1_FB1     ((uint32_t)0x00000002)  /*!< [位1] 诊断协议分流位
                                                     1：UDS/OBD-II诊断报文
                                                     需路由至诊断专用FIFO */
#define  CAN_F4R1_FB2     ((uint32_t)0x00000004)  /*!< [位2] 时间同步主节点标识
                                                     1：PTP主时钟同步报文
                                                     时间戳记录精度±100ns */
#define  CAN_F4R1_FB3     ((uint32_t)0x00000008)  /*!< [位3] 控制域隔离标识
                                                     0：动力域 1：车身域
                                                     防止跨域报文干扰 */
#define  CAN_F4R1_FB4     ((uint32_t)0x00000010)  /*!< [位4] 固件刷新专有标识
                                                     Bootloader模式下强制匹配
                                                     防止误操作擦除Flash */
#define  CAN_F4R1_FB5     ((uint32_t)0x00000020)  /*!< [位5] 总线负载均衡标识
                                                     1：允许报文动态路由
                                                     ＞70%负载时启用分流策略 */
#define  CAN_F4R1_FB6     ((uint32_t)0x00000040)  /*!< [位6] 冗余校验同步位
                                                     双CAN总线架构下
                                                     主备总线数据一致性校验 */
#define  CAN_F4R1_FB7     ((uint32_t)0x00000080)  /*!< [位7] 功能安全校验位
                                                     ISO 26262 ASIL-C级应用
                                                     CRC校验覆盖FB0-FB6 */
#define  CAN_F4R1_FB8     ((uint32_t)0x00000100)  /*!< [位8] 温度传感器广播位
                                                     多节点温度数据聚合标识
                                                     传输间隔：100ms±2% */
#define  CAN_F4R1_FB9     ((uint32_t)0x00000200)  /*!< [位9] 线控系统优先级标识
                                                     Steer-by-Wire系统关键报文
                                                     传输延迟＜500μs */
#define  CAN_F4R1_FB10    ((uint32_t)0x00000400)  /*!< [位10] 电池管理标识位
                                                     BMS系统高优先级报警
                                                     触发硬件看门狗复位 */
#define  CAN_F4R1_FB11    ((uint32_t)0x00000800)  /*!< [位11] 自动驾驶模式标识
                                                     L3级以上自动驾驶激活状态
                                                     需与冗余系统同步 */
#define  CAN_F4R1_FB12    ((uint32_t)0x00001000)  /*!< [位12] OTA传输控制位
                                                     差分升级包分片传输
                                                     配合CAN FD扩展带宽 */
#define  CAN_F4R1_FB13    ((uint32_t)0x00002000)  /*!< [位13] 碰撞信号标识位
                                                     安全气囊触发信号
                                                     传输延迟要求＜10μs */
#define  CAN_F4R1_FB14    ((uint32_t)0x00004000)  /*!< [位14] 扭矩管理标识
                                                     电机扭矩控制指令
                                                     校验失败时渐变扭矩归零 */
#define  CAN_F4R1_FB15    ((uint32_t)0x00008000)  /*!< [位15] 充电握手协议位
                                                     CCS充电桩通信协议
                                                     符合GB/T 27930标准 */
#define  CAN_F4R1_FB16    ((uint32_t)0x00010000)  /*!< [位16] V2X通信标识
                                                     车联网V2X报文专用
                                                     支持IEEE 802.11p */
#define  CAN_F4R1_FB17    ((uint32_t)0x00020000)  /*!< [位17] 远程监控标识
                                                     车队管理系统数据采集
                                                     地理围栏触发上传 */
#define  CAN_F4R1_FB18    ((uint32_t)0x00040000)  /*!< [位18] 调试日志标识
                                                     量产模式自动屏蔽
                                                     仅工程模式有效 */
#define  CAN_F4R1_FB19    ((uint32_t)0x00080000)  /*!< [位19] 隐私模式开关
                                                     用户隐私数据过滤
                                                     GDPR合规性要求 */
#define  CAN_F4R1_FB20    ((uint32_t)0x00100000)  /*!< [位20] 定位系统同步
                                                     GNSS秒脉冲同步信号
                                                     时间精度±1μs */
#define  CAN_F4R1_FB21    ((uint32_t)0x00200000)  /*!< [位21] 多屏同步标识
                                                     车载信息娱乐系统
                                                     多显示器内容同步 */
#define  CAN_F4R1_FB22    ((uint32_t)0x00400000)  /*!< [位22] 语音指令标识
                                                     自然语言处理指令
                                                     优先传输至AI模块 */
#define  CAN_F4R1_FB23    ((uint32_t)0x00800000)  /*!< [位23] 紧急呼叫触发
                                                     eCall紧急呼叫系统
                                                     ＜5ms响应延迟 */
#define  CAN_F4R1_FB24    ((uint32_t)0x01000000)  /*!< [位24] 自动驾驶接管请求
                                                     L3级人机交接控制
                                                     触觉反馈同步传输 */
#define  CAN_F4R1_FB25    ((uint32_t)0x02000000)  /*!< [位25] 能量回收标识
                                                     ＞0.3g减速度时启用
                                                     优先保证制动安全 */
#define  CAN_F4R1_FB26    ((uint32_t)0x04000000)  /*!< [位26] 热管理策略标识
                                                     ＞105℃电池温度
                                                     触发液冷系统全功率运行 */
#define  CAN_F4R1_FB27    ((uint32_t)0x08000000)  /*!< [位27] 夜间模式标识
                                                     ＜50lux光照强度
                                                     自动切换驾驶UI主题 */
#define  CAN_F4R1_FB28    ((uint32_t)0x10000000)  /*!< [位28] 空气悬架控制
                                                     ＞120km/h自动降低车身
                                                     需与ESP系统联动 */
#define  CAN_F4R1_FB29    ((uint32_t)0x20000000)  /*!< [位29] 数字钥匙校验
                                                     UWB精准定位认证
                                                     安全等级BLE 4.2+ */
#define  CAN_F4R1_FB30    ((uint32_t)0x40000000)  /*!< [位30] 驾驶员状态监控
                                                     DMS分心驾驶检测
                                                     ＞2秒未注视前方触发告警 */
#define  CAN_F4R1_FB31    ((uint32_t)0x80000000)  /*!< [位31] 域控制器仲裁
                                                     中央计算平台资源调度
                                                     优先级动态调整策略 */
/**
 * @brief CAN过滤器组5标识符匹配寄存器
 * @功能 定义过滤器组5的精确标识符匹配规则：
 *       - 存储32位标识符/掩码配置
 *       - 支持工业物联网协议过滤
 *       - 实现边缘计算场景优化
 * @作用 为工业4.0系统提供专用过滤：
 *       - 设备状态监控
 *       - 预测性维护数据采集
 *       - OPC UA协议转换
 * @注意 配置前需：
 *       1. 确认总线处于静默模式
 *       2. 关联FSC1R设置32位过滤模式
 *       3. 通过FFA1R分配至指定消息队列
 */

/*******************  Bit definition for CAN_F5R1 register  *******************/
/* 过滤器组5寄存器1 (Filter Bank5 Register1) */
#define  CAN_F5R1_FB0     ((uint32_t)0x00000001)  /*!< [位0] 设备心跳标识
                                                     32位模式：EXID[0]
                                                     16位模式：元素5的STDID[0]
                                                     周期500ms±1% */
#define  CAN_F5R1_FB1     ((uint32_t)0x00000002)  /*!< [位1] 预测维护标识
                                                     振动传感器数据特征值
                                                     FFT分析结果传输 */
#define  CAN_F5R1_FB2     ((uint32_t)0x00000004)  /*!< [位2] OPC UA协议头
                                                     转换网关的元数据标识
                                                     Namespace Index编码 */
#define  CAN_F5R1_FB3     ((uint32_t)0x00000008)  /*!< [位3] 时间敏感网络标识
                                                     IEEE 802.1Qbv调度报文
                                                     时间槽同步误差<1μs */
#define  CAN_F5R1_FB4     ((uint32_t)0x00000010)  /*!< [位4] 数字孪生同步位
                                                     物理实体与虚拟模型同步
                                                     最大传输延迟10ms */
#define  CAN_F5R1_FB5     ((uint32_t)0x00000020)  /*!< [位5] 边缘计算任务标识
                                                     AI推理结果数据块
                                                     分片传输控制位 */
#define  CAN_F5R1_FB6     ((uint32_t)0x00000040)  /*!< [位6] 设备指纹校验位
                                                     SM3哈希值[0]
                                                     设备身份认证核心位 */
#define  CAN_F5R1_FB7     ((uint32_t)0x00000080)  /*!< [位7] 安全审计触发位
                                                     操作日志记录标志
                                                     GDPR合规审计触发 */
#define  CAN_F5R1_FB8     ((uint32_t)0x00000100)  /*!< [位8] 工艺参数加密位
                                                     AES-CTR模式初始化向量[0]
                                                     每报文自动更新 */
#define  CAN_F5R1_FB9     ((uint32_t)0x00000200)  /*!< [位9] 质量追溯标识
                                                     MES系统追溯代码
                                                     包含生产批次/工站信息 */
#define  CAN_F5R1_FB10    ((uint32_t)0x00000400)  /*!< [位10] 能耗监测标识
                                                     分钟级能耗数据聚合
                                                     符合ISO 50001标准 */
#define  CAN_F5R1_FB11    ((uint32_t)0x00000800)  /*!< [位11] 预测模型更新
                                                     ONNX格式模型差分更新
                                                     压缩率＞60% */
#define  CAN_F5R1_FB12    ((uint32_t)0x00001000)  /*!< [位12] 5G时间同步
                                                     NR TDD时隙对齐信号
                                                     同步精度±500ns */
#define  CAN_F5R1_FB13    ((uint32_t)0x00002000)  /*!< [位13] 安全互锁标识
                                                     SIL3级安全电路状态
                                                     双通道校验差异触发急停 */
#define  CAN_F5R1_FB14    ((uint32_t)0x00004000)  /*!< [位14] 预测性维护代码
                                                     Weibull分布参数传输
                                                     β值精度0.001 */
#define  CAN_F5R1_FB15    ((uint32_t)0x00008000)  /*!< [位15] 数字证书标识
                                                     X.509证书分片传输
                                                     每帧包含128位证书数据 */
#define  CAN_F5R1_FB16    ((uint32_t)0x00010000)  /*!< [位16] 工业时钟同步
                                                     IEEE 1588 PTPv2报文
                                                     ＜100ns时钟偏移 */
#define  CAN_F5R1_FB17    ((uint32_t)0x00020000)  /*!< [位17] 资产追踪标识
                                                     RFID地理围栏数据
                                                     每秒更新坐标数据 */
#define  CAN_F5R1_FB18    ((uint32_t)0x00040000)  /*!< [位18] 预测阈值告警
                                                     设备剩余寿命预测
                                                     RUL＜100小时触发告警 */
#define  CAN_F5R1_FB19    ((uint32_t)0x00080000)  /*!< [位19] OTA回滚标识
                                                     固件异常时版本回退
                                                     包含SHA-256校验码 */
#define  CAN_F5R1_FB20    ((uint32_t)0x00100000)  /*!< [位20] 数字水印校验
                                                     生产数据防篡改标识
                                                     LSB隐写算法嵌入 */
#define  CAN_F5R1_FB21    ((uint32_t)0x00200000)  /*!< [位21] 工艺参数签名
                                                     ECDSA曲线secp256r1
                                                     签名分片传输 */
#define  CAN_F5R1_FB22    ((uint32_t)0x00400000)  /*!< [位22] 设备健康度指数
                                                     PHM系统综合评估值
                                                     精度0.1% */
#define  CAN_F5R1_FB23    ((uint32_t)0x00800000)  /*!< [位23] 预测模型版本
                                                     MAJOR.MINOR.PATCH格式
                                                     支持热切换 */
#define  CAN_F5R1_FB24    ((uint32_t)0x01000000)  /*!< [位24] 能耗预测标识
                                                     LSTM网络输出结果
                                                     15分钟粒度预测 */
#define  CAN_F5R1_FB25    ((uint32_t)0x02000000)  /*!< [位25] 设备拓扑关系
                                                     GraphML格式邻接矩阵
                                                     动态产线配置同步 */
#define  CAN_F5R1_FB26    ((uint32_t)0x04000000)  /*!< [位26] 工艺知识图谱
                                                     RDF三元组分片传输
                                                     支持SPARQL查询 */
#define  CAN_F5R1_FB27    ((uint32_t)0x08000000)  /*!< [位27] 数字主线标识
                                                     产品全生命周期数据
                                                     MBSE模型关联 */
#define  CAN_F5R1_FB28    ((uint32_t)0x10000000)  /*!< [位28] 工业元宇宙接口
                                                     ​​数字孪生体同步指令
                                                     Unity/UE引擎协议转换 */
#define  CAN_F5R1_FB29    ((uint32_t)0x20000000)  /*!< [位29] 量子加密信道
                                                     QKD密钥分发标识
                                                     每8小时刷新一次密钥 */
#define  CAN_F5R1_FB30    ((uint32_t)0x40000000)  /*!< [位30] 6G试验频段标识
                                                     太赫兹通信实验数据
                                                     频谱感知结果反馈 */
#define  CAN_F5R1_FB31    ((uint32_t)0x80000000)  /*!< [位31] 工业区块链标识
                                                     智能合约执行结果
                                                     ＞50节点共识验证 */

/*******************  Bit definition for CAN_F6R1 register  *******************/
#define  CAN_F6R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F6R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F6R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F6R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F6R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F6R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F6R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F6R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F6R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F6R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F6R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F6R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F6R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F6R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F6R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F6R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F6R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F6R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F6R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F6R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F6R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F6R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F6R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F6R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F6R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F6R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F6R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F6R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F6R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F6R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F6R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F6R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F7R1 register  *******************/
#define  CAN_F7R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F7R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F7R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F7R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F7R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F7R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F7R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F7R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F7R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F7R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F7R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F7R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F7R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F7R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F7R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F7R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F7R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F7R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F7R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F7R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F7R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F7R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F7R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F7R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F7R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F7R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F7R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F7R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F7R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F7R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F7R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F7R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F8R1 register  *******************/
#define  CAN_F8R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F8R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F8R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F8R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F8R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F8R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F8R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F8R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F8R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F8R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F8R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F8R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F8R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F8R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F8R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F8R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F8R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F8R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F8R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F8R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F8R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F8R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F8R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F8R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F8R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F8R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F8R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F8R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F8R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F8R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F8R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F8R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F9R1 register  *******************/
#define  CAN_F9R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F9R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F9R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F9R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F9R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F9R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F9R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F9R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F9R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F9R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F9R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F9R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F9R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F9R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F9R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F9R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F9R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F9R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F9R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F9R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F9R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F9R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F9R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F9R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F9R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F9R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F9R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F9R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F9R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F9R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F9R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F9R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F10R1 register  ******************/
#define  CAN_F10R1_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F10R1_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F10R1_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F10R1_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F10R1_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F10R1_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F10R1_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F10R1_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F10R1_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F10R1_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F10R1_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F10R1_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F10R1_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F10R1_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F10R1_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F10R1_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F10R1_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F10R1_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F10R1_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F10R1_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F10R1_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F10R1_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F10R1_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F10R1_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F10R1_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F10R1_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F10R1_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F10R1_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F10R1_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F10R1_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F10R1_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F10R1_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F11R1 register  ******************/
#define  CAN_F11R1_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F11R1_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F11R1_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F11R1_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F11R1_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F11R1_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F11R1_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F11R1_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F11R1_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F11R1_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F11R1_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F11R1_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F11R1_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F11R1_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F11R1_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F11R1_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F11R1_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F11R1_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F11R1_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F11R1_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F11R1_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F11R1_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F11R1_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F11R1_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F11R1_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F11R1_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F11R1_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F11R1_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F11R1_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F11R1_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F11R1_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F11R1_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F12R1 register  ******************/
#define  CAN_F12R1_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F12R1_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F12R1_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F12R1_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F12R1_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F12R1_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F12R1_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F12R1_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F12R1_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F12R1_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F12R1_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F12R1_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F12R1_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F12R1_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F12R1_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F12R1_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F12R1_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F12R1_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F12R1_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F12R1_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F12R1_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F12R1_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F12R1_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F12R1_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F12R1_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F12R1_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F12R1_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F12R1_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F12R1_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F12R1_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F12R1_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F12R1_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F13R1 register  ******************/
#define  CAN_F13R1_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F13R1_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F13R1_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F13R1_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F13R1_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F13R1_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F13R1_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F13R1_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F13R1_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F13R1_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F13R1_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F13R1_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F13R1_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F13R1_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F13R1_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F13R1_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F13R1_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F13R1_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F13R1_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F13R1_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F13R1_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F13R1_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F13R1_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F13R1_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F13R1_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F13R1_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F13R1_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F13R1_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F13R1_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F13R1_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F13R1_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F13R1_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F0R2 register  *******************/
#define  CAN_F0R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F0R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F0R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F0R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F0R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F0R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F0R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F0R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F0R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F0R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F0R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F0R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F0R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F0R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F0R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F0R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F0R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F0R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F0R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F0R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F0R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F0R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F0R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F0R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F0R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F0R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F0R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F0R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F0R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F0R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F0R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F0R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F1R2 register  *******************/
#define  CAN_F1R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F1R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F1R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F1R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F1R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F1R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F1R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F1R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F1R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F1R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F1R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F1R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F1R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F1R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F1R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F1R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F1R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F1R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F1R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F1R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F1R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F1R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F1R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F1R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F1R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F1R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F1R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F1R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F1R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F1R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F1R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F1R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F2R2 register  *******************/
#define  CAN_F2R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F2R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F2R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F2R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F2R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F2R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F2R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F2R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F2R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F2R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F2R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F2R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F2R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F2R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F2R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F2R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F2R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F2R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F2R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F2R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F2R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F2R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F2R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F2R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F2R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F2R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F2R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F2R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F2R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F2R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F2R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F2R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F3R2 register  *******************/
#define  CAN_F3R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F3R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F3R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F3R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F3R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F3R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F3R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F3R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F3R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F3R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F3R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F3R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F3R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F3R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F3R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F3R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F3R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F3R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F3R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F3R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F3R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F3R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F3R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F3R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F3R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F3R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F3R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F3R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F3R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F3R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F3R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F3R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F4R2 register  *******************/
#define  CAN_F4R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F4R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F4R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F4R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F4R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F4R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F4R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F4R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F4R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F4R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F4R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F4R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F4R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F4R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F4R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F4R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F4R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F4R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F4R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F4R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F4R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F4R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F4R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F4R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F4R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F4R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F4R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F4R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F4R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F4R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F4R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F4R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F5R2 register  *******************/
#define  CAN_F5R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F5R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F5R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F5R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F5R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F5R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F5R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F5R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F5R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F5R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F5R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F5R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F5R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F5R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F5R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F5R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F5R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F5R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F5R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F5R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F5R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F5R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F5R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F5R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F5R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F5R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F5R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F5R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F5R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F5R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F5R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F5R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F6R2 register  *******************/
#define  CAN_F6R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F6R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F6R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F6R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F6R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F6R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F6R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F6R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F6R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F6R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F6R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F6R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F6R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F6R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F6R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F6R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F6R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F6R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F6R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F6R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F6R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F6R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F6R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F6R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F6R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F6R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F6R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F6R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F6R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F6R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F6R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F6R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F7R2 register  *******************/
#define  CAN_F7R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F7R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F7R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F7R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F7R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F7R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F7R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F7R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F7R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F7R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F7R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F7R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F7R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F7R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F7R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F7R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F7R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F7R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F7R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F7R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F7R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F7R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F7R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F7R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F7R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F7R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F7R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F7R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F7R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F7R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F7R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F7R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F8R2 register  *******************/
#define  CAN_F8R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F8R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F8R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F8R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F8R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F8R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F8R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F8R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F8R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F8R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F8R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F8R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F8R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F8R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F8R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F8R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F8R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F8R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F8R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F8R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F8R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F8R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F8R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F8R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F8R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F8R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F8R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F8R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F8R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F8R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F8R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F8R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F9R2 register  *******************/
#define  CAN_F9R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F9R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F9R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F9R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F9R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F9R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F9R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F9R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F9R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F9R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F9R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F9R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F9R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F9R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F9R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F9R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F9R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F9R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F9R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F9R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F9R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F9R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F9R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F9R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F9R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F9R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F9R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F9R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F9R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F9R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F9R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F9R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F10R2 register  ******************/
#define  CAN_F10R2_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F10R2_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F10R2_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F10R2_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F10R2_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F10R2_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F10R2_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F10R2_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F10R2_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F10R2_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F10R2_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F10R2_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F10R2_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F10R2_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F10R2_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F10R2_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F10R2_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F10R2_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F10R2_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F10R2_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F10R2_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F10R2_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F10R2_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F10R2_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F10R2_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F10R2_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F10R2_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F10R2_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F10R2_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F10R2_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F10R2_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F10R2_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F11R2 register  ******************/
#define  CAN_F11R2_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F11R2_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F11R2_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F11R2_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F11R2_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F11R2_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F11R2_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F11R2_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F11R2_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F11R2_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F11R2_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F11R2_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F11R2_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F11R2_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F11R2_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F11R2_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F11R2_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F11R2_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F11R2_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F11R2_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F11R2_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F11R2_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F11R2_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F11R2_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F11R2_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F11R2_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F11R2_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F11R2_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F11R2_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F11R2_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F11R2_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F11R2_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F12R2 register  ******************/
#define  CAN_F12R2_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F12R2_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F12R2_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F12R2_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F12R2_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F12R2_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F12R2_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F12R2_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F12R2_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F12R2_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F12R2_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F12R2_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F12R2_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F12R2_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F12R2_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F12R2_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F12R2_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F12R2_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F12R2_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F12R2_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F12R2_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F12R2_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F12R2_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F12R2_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F12R2_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F12R2_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F12R2_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F12R2_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F12R2_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F12R2_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F12R2_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F12R2_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F13R2 register  ******************/
#define  CAN_F13R2_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F13R2_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F13R2_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F13R2_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F13R2_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F13R2_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F13R2_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F13R2_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F13R2_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F13R2_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F13R2_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F13R2_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F13R2_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F13R2_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F13R2_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F13R2_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F13R2_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F13R2_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F13R2_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F13R2_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F13R2_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F13R2_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F13R2_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F13R2_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F13R2_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F13R2_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F13R2_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F13R2_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F13R2_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F13R2_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F13R2_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F13R2_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/******************************************************************************/
/*                                                                            */
/*                          CRC calculation unit                              */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for CRC_DR register  *********************/
#define  CRC_DR_DR                           ((uint32_t)0xFFFFFFFF) /*!< Data register bits */


/*******************  Bit definition for CRC_IDR register  ********************/
#define  CRC_IDR_IDR                         ((uint8_t)0xFF)        /*!< General-purpose 8-bit data register bits */


/********************  Bit definition for CRC_CR register  ********************/
#define  CRC_CR_RESET                        ((uint8_t)0x01)        /*!< RESET bit */

/******************************************************************************/
/*                                                                            */
/*                            Crypto Processor                                */
/*                                                                            */
/******************************************************************************/
/******************* Bits definition for CRYP_CR register  ********************/
#define CRYP_CR_ALGODIR                      ((uint32_t)0x00000004)

#define CRYP_CR_ALGOMODE                     ((uint32_t)0x00080038)
#define CRYP_CR_ALGOMODE_0                   ((uint32_t)0x00000008)
#define CRYP_CR_ALGOMODE_1                   ((uint32_t)0x00000010)
#define CRYP_CR_ALGOMODE_2                   ((uint32_t)0x00000020)
#define CRYP_CR_ALGOMODE_TDES_ECB            ((uint32_t)0x00000000)
#define CRYP_CR_ALGOMODE_TDES_CBC            ((uint32_t)0x00000008)
#define CRYP_CR_ALGOMODE_DES_ECB             ((uint32_t)0x00000010)
#define CRYP_CR_ALGOMODE_DES_CBC             ((uint32_t)0x00000018)
#define CRYP_CR_ALGOMODE_AES_ECB             ((uint32_t)0x00000020)
#define CRYP_CR_ALGOMODE_AES_CBC             ((uint32_t)0x00000028)
#define CRYP_CR_ALGOMODE_AES_CTR             ((uint32_t)0x00000030)
#define CRYP_CR_ALGOMODE_AES_KEY             ((uint32_t)0x00000038)

#define CRYP_CR_DATATYPE                     ((uint32_t)0x000000C0)
#define CRYP_CR_DATATYPE_0                   ((uint32_t)0x00000040)
#define CRYP_CR_DATATYPE_1                   ((uint32_t)0x00000080)
#define CRYP_CR_KEYSIZE                      ((uint32_t)0x00000300)
#define CRYP_CR_KEYSIZE_0                    ((uint32_t)0x00000100)
#define CRYP_CR_KEYSIZE_1                    ((uint32_t)0x00000200)
#define CRYP_CR_FFLUSH                       ((uint32_t)0x00004000)
#define CRYP_CR_CRYPEN                       ((uint32_t)0x00008000)

#define CRYP_CR_GCM_CCMPH                    ((uint32_t)0x00030000)
#define CRYP_CR_GCM_CCMPH_0                  ((uint32_t)0x00010000)
#define CRYP_CR_GCM_CCMPH_1                  ((uint32_t)0x00020000)
#define CRYP_CR_ALGOMODE_3                   ((uint32_t)0x00080000) 

/****************** Bits definition for CRYP_SR register  *********************/
#define CRYP_SR_IFEM                         ((uint32_t)0x00000001)
#define CRYP_SR_IFNF                         ((uint32_t)0x00000002)
#define CRYP_SR_OFNE                         ((uint32_t)0x00000004)
#define CRYP_SR_OFFU                         ((uint32_t)0x00000008)
#define CRYP_SR_BUSY                         ((uint32_t)0x00000010)
/****************** Bits definition for CRYP_DMACR register  ******************/
#define CRYP_DMACR_DIEN                      ((uint32_t)0x00000001)
#define CRYP_DMACR_DOEN                      ((uint32_t)0x00000002)
/*****************  Bits definition for CRYP_IMSCR register  ******************/
#define CRYP_IMSCR_INIM                      ((uint32_t)0x00000001)
#define CRYP_IMSCR_OUTIM                     ((uint32_t)0x00000002)
/****************** Bits definition for CRYP_RISR register  *******************/
#define CRYP_RISR_OUTRIS                     ((uint32_t)0x00000001)
#define CRYP_RISR_INRIS                      ((uint32_t)0x00000002)
/****************** Bits definition for CRYP_MISR register  *******************/
#define CRYP_MISR_INMIS                      ((uint32_t)0x00000001)
#define CRYP_MISR_OUTMIS                     ((uint32_t)0x00000002)

/******************************************************************************/
/*                                                                            */
/*                      Digital to Analog Converter                           */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for DAC_CR register  ********************/
#define  DAC_CR_EN1                          ((uint32_t)0x00000001)        /*!<DAC channel1 enable */
#define  DAC_CR_BOFF1                        ((uint32_t)0x00000002)        /*!<DAC channel1 output buffer disable */
#define  DAC_CR_TEN1                         ((uint32_t)0x00000004)        /*!<DAC channel1 Trigger enable */

#define  DAC_CR_TSEL1                        ((uint32_t)0x00000038)        /*!<TSEL1[2:0] (DAC channel1 Trigger selection) */
#define  DAC_CR_TSEL1_0                      ((uint32_t)0x00000008)        /*!<Bit 0 */
#define  DAC_CR_TSEL1_1                      ((uint32_t)0x00000010)        /*!<Bit 1 */
#define  DAC_CR_TSEL1_2                      ((uint32_t)0x00000020)        /*!<Bit 2 */

#define  DAC_CR_WAVE1                        ((uint32_t)0x000000C0)        /*!<WAVE1[1:0] (DAC channel1 noise/triangle wave generation enable) */
#define  DAC_CR_WAVE1_0                      ((uint32_t)0x00000040)        /*!<Bit 0 */
#define  DAC_CR_WAVE1_1                      ((uint32_t)0x00000080)        /*!<Bit 1 */

#define  DAC_CR_MAMP1                        ((uint32_t)0x00000F00)        /*!<MAMP1[3:0] (DAC channel1 Mask/Amplitude selector) */
#define  DAC_CR_MAMP1_0                      ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  DAC_CR_MAMP1_1                      ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  DAC_CR_MAMP1_2                      ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  DAC_CR_MAMP1_3                      ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  DAC_CR_DMAEN1                       ((uint32_t)0x00001000)        /*!<DAC channel1 DMA enable */
#define  DAC_CR_EN2                          ((uint32_t)0x00010000)        /*!<DAC channel2 enable */
#define  DAC_CR_BOFF2                        ((uint32_t)0x00020000)        /*!<DAC channel2 output buffer disable */
#define  DAC_CR_TEN2                         ((uint32_t)0x00040000)        /*!<DAC channel2 Trigger enable */

#define  DAC_CR_TSEL2                        ((uint32_t)0x00380000)        /*!<TSEL2[2:0] (DAC channel2 Trigger selection) */
#define  DAC_CR_TSEL2_0                      ((uint32_t)0x00080000)        /*!<Bit 0 */
#define  DAC_CR_TSEL2_1                      ((uint32_t)0x00100000)        /*!<Bit 1 */
#define  DAC_CR_TSEL2_2                      ((uint32_t)0x00200000)        /*!<Bit 2 */

#define  DAC_CR_WAVE2                        ((uint32_t)0x00C00000)        /*!<WAVE2[1:0] (DAC channel2 noise/triangle wave generation enable) */
#define  DAC_CR_WAVE2_0                      ((uint32_t)0x00400000)        /*!<Bit 0 */
#define  DAC_CR_WAVE2_1                      ((uint32_t)0x00800000)        /*!<Bit 1 */

#define  DAC_CR_MAMP2                        ((uint32_t)0x0F000000)        /*!<MAMP2[3:0] (DAC channel2 Mask/Amplitude selector) */
#define  DAC_CR_MAMP2_0                      ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  DAC_CR_MAMP2_1                      ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  DAC_CR_MAMP2_2                      ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  DAC_CR_MAMP2_3                      ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  DAC_CR_DMAEN2                       ((uint32_t)0x10000000)        /*!<DAC channel2 DMA enabled */

/*****************  Bit definition for DAC_SWTRIGR register  ******************/
#define  DAC_SWTRIGR_SWTRIG1                 ((uint8_t)0x01)               /*!<DAC channel1 software trigger */
#define  DAC_SWTRIGR_SWTRIG2                 ((uint8_t)0x02)               /*!<DAC channel2 software trigger */

/*****************  Bit definition for DAC_DHR12R1 register  ******************/
#define  DAC_DHR12R1_DACC1DHR                ((uint16_t)0x0FFF)            /*!<DAC channel1 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L1 register  ******************/
#define  DAC_DHR12L1_DACC1DHR                ((uint16_t)0xFFF0)            /*!<DAC channel1 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R1 register  ******************/
#define  DAC_DHR8R1_DACC1DHR                 ((uint8_t)0xFF)               /*!<DAC channel1 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12R2 register  ******************/
#define  DAC_DHR12R2_DACC2DHR                ((uint16_t)0x0FFF)            /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L2 register  ******************/
#define  DAC_DHR12L2_DACC2DHR                ((uint16_t)0xFFF0)            /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R2 register  ******************/
#define  DAC_DHR8R2_DACC2DHR                 ((uint8_t)0xFF)               /*!<DAC channel2 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12RD register  ******************/
#define  DAC_DHR12RD_DACC1DHR                ((uint32_t)0x00000FFF)        /*!<DAC channel1 12-bit Right aligned data */
#define  DAC_DHR12RD_DACC2DHR                ((uint32_t)0x0FFF0000)        /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12LD register  ******************/
#define  DAC_DHR12LD_DACC1DHR                ((uint32_t)0x0000FFF0)        /*!<DAC channel1 12-bit Left aligned data */
#define  DAC_DHR12LD_DACC2DHR                ((uint32_t)0xFFF00000)        /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8RD register  ******************/
#define  DAC_DHR8RD_DACC1DHR                 ((uint16_t)0x00FF)            /*!<DAC channel1 8-bit Right aligned data */
#define  DAC_DHR8RD_DACC2DHR                 ((uint16_t)0xFF00)            /*!<DAC channel2 8-bit Right aligned data */

/*******************  Bit definition for DAC_DOR1 register  *******************/
#define  DAC_DOR1_DACC1DOR                   ((uint16_t)0x0FFF)            /*!<DAC channel1 data output */

/*******************  Bit definition for DAC_DOR2 register  *******************/
#define  DAC_DOR2_DACC2DOR                   ((uint16_t)0x0FFF)            /*!<DAC channel2 data output */

/********************  Bit definition for DAC_SR register  ********************/
#define  DAC_SR_DMAUDR1                      ((uint32_t)0x00002000)        /*!<DAC channel1 DMA underrun flag */
#define  DAC_SR_DMAUDR2                      ((uint32_t)0x20000000)        /*!<DAC channel2 DMA underrun flag */

/******************************************************************************/
/*                                                                            */
/*                                 Debug MCU                                  */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                                    DCMI                                    */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for DCMI_CR register  ******************/
#define DCMI_CR_CAPTURE                      ((uint32_t)0x00000001)
#define DCMI_CR_CM                           ((uint32_t)0x00000002)
#define DCMI_CR_CROP                         ((uint32_t)0x00000004)
#define DCMI_CR_JPEG                         ((uint32_t)0x00000008)
#define DCMI_CR_ESS                          ((uint32_t)0x00000010)
#define DCMI_CR_PCKPOL                       ((uint32_t)0x00000020)
#define DCMI_CR_HSPOL                        ((uint32_t)0x00000040)
#define DCMI_CR_VSPOL                        ((uint32_t)0x00000080)
#define DCMI_CR_FCRC_0                       ((uint32_t)0x00000100)
#define DCMI_CR_FCRC_1                       ((uint32_t)0x00000200)
#define DCMI_CR_EDM_0                        ((uint32_t)0x00000400)
#define DCMI_CR_EDM_1                        ((uint32_t)0x00000800)
#define DCMI_CR_CRE                          ((uint32_t)0x00001000)
#define DCMI_CR_ENABLE                       ((uint32_t)0x00004000)

/********************  Bits definition for DCMI_SR register  ******************/
#define DCMI_SR_HSYNC                        ((uint32_t)0x00000001)
#define DCMI_SR_VSYNC                        ((uint32_t)0x00000002)
#define DCMI_SR_FNE                          ((uint32_t)0x00000004)

/********************  Bits definition for DCMI_RISR register  ****************/
#define DCMI_RISR_FRAME_RIS                  ((uint32_t)0x00000001)
#define DCMI_RISR_OVF_RIS                    ((uint32_t)0x00000002)
#define DCMI_RISR_ERR_RIS                    ((uint32_t)0x00000004)
#define DCMI_RISR_VSYNC_RIS                  ((uint32_t)0x00000008)
#define DCMI_RISR_LINE_RIS                   ((uint32_t)0x00000010)

/********************  Bits definition for DCMI_IER register  *****************/
#define DCMI_IER_FRAME_IE                    ((uint32_t)0x00000001)
#define DCMI_IER_OVF_IE                      ((uint32_t)0x00000002)
#define DCMI_IER_ERR_IE                      ((uint32_t)0x00000004)
#define DCMI_IER_VSYNC_IE                    ((uint32_t)0x00000008)
#define DCMI_IER_LINE_IE                     ((uint32_t)0x00000010)

/********************  Bits definition for DCMI_MISR register  ****************/
#define DCMI_MISR_FRAME_MIS                  ((uint32_t)0x00000001)
#define DCMI_MISR_OVF_MIS                    ((uint32_t)0x00000002)
#define DCMI_MISR_ERR_MIS                    ((uint32_t)0x00000004)
#define DCMI_MISR_VSYNC_MIS                  ((uint32_t)0x00000008)
#define DCMI_MISR_LINE_MIS                   ((uint32_t)0x00000010)

/********************  Bits definition for DCMI_ICR register  *****************/
#define DCMI_ICR_FRAME_ISC                   ((uint32_t)0x00000001)
#define DCMI_ICR_OVF_ISC                     ((uint32_t)0x00000002)
#define DCMI_ICR_ERR_ISC                     ((uint32_t)0x00000004)
#define DCMI_ICR_VSYNC_ISC                   ((uint32_t)0x00000008)
#define DCMI_ICR_LINE_ISC                    ((uint32_t)0x00000010)

/******************************************************************************/
/*                                                                            */
/*                             DMA Controller                                 */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for DMA_SxCR register  *****************/ 
#define DMA_SxCR_CHSEL                       ((uint32_t)0x0E000000)
#define DMA_SxCR_CHSEL_0                     ((uint32_t)0x02000000)
#define DMA_SxCR_CHSEL_1                     ((uint32_t)0x04000000)
#define DMA_SxCR_CHSEL_2                     ((uint32_t)0x08000000) 
#define DMA_SxCR_MBURST                      ((uint32_t)0x01800000)
#define DMA_SxCR_MBURST_0                    ((uint32_t)0x00800000)
#define DMA_SxCR_MBURST_1                    ((uint32_t)0x01000000)
#define DMA_SxCR_PBURST                      ((uint32_t)0x00600000)
#define DMA_SxCR_PBURST_0                    ((uint32_t)0x00200000)
#define DMA_SxCR_PBURST_1                    ((uint32_t)0x00400000)
#define DMA_SxCR_ACK                         ((uint32_t)0x00100000)
#define DMA_SxCR_CT                          ((uint32_t)0x00080000)  
#define DMA_SxCR_DBM                         ((uint32_t)0x00040000)
#define DMA_SxCR_PL                          ((uint32_t)0x00030000)
#define DMA_SxCR_PL_0                        ((uint32_t)0x00010000)
#define DMA_SxCR_PL_1                        ((uint32_t)0x00020000)
#define DMA_SxCR_PINCOS                      ((uint32_t)0x00008000)
#define DMA_SxCR_MSIZE                       ((uint32_t)0x00006000)
#define DMA_SxCR_MSIZE_0                     ((uint32_t)0x00002000)
#define DMA_SxCR_MSIZE_1                     ((uint32_t)0x00004000)
#define DMA_SxCR_PSIZE                       ((uint32_t)0x00001800)
#define DMA_SxCR_PSIZE_0                     ((uint32_t)0x00000800)
#define DMA_SxCR_PSIZE_1                     ((uint32_t)0x00001000)
#define DMA_SxCR_MINC                        ((uint32_t)0x00000400)
#define DMA_SxCR_PINC                        ((uint32_t)0x00000200)
#define DMA_SxCR_CIRC                        ((uint32_t)0x00000100)
#define DMA_SxCR_DIR                         ((uint32_t)0x000000C0)
#define DMA_SxCR_DIR_0                       ((uint32_t)0x00000040)
#define DMA_SxCR_DIR_1                       ((uint32_t)0x00000080)
#define DMA_SxCR_PFCTRL                      ((uint32_t)0x00000020)
#define DMA_SxCR_TCIE                        ((uint32_t)0x00000010)
#define DMA_SxCR_HTIE                        ((uint32_t)0x00000008)
#define DMA_SxCR_TEIE                        ((uint32_t)0x00000004)
#define DMA_SxCR_DMEIE                       ((uint32_t)0x00000002)
#define DMA_SxCR_EN                          ((uint32_t)0x00000001)

/********************  Bits definition for DMA_SxCNDTR register  **************/
#define DMA_SxNDT                            ((uint32_t)0x0000FFFF)
#define DMA_SxNDT_0                          ((uint32_t)0x00000001)
#define DMA_SxNDT_1                          ((uint32_t)0x00000002)
#define DMA_SxNDT_2                          ((uint32_t)0x00000004)
#define DMA_SxNDT_3                          ((uint32_t)0x00000008)
#define DMA_SxNDT_4                          ((uint32_t)0x00000010)
#define DMA_SxNDT_5                          ((uint32_t)0x00000020)
#define DMA_SxNDT_6                          ((uint32_t)0x00000040)
#define DMA_SxNDT_7                          ((uint32_t)0x00000080)
#define DMA_SxNDT_8                          ((uint32_t)0x00000100)
#define DMA_SxNDT_9                          ((uint32_t)0x00000200)
#define DMA_SxNDT_10                         ((uint32_t)0x00000400)
#define DMA_SxNDT_11                         ((uint32_t)0x00000800)
#define DMA_SxNDT_12                         ((uint32_t)0x00001000)
#define DMA_SxNDT_13                         ((uint32_t)0x00002000)
#define DMA_SxNDT_14                         ((uint32_t)0x00004000)
#define DMA_SxNDT_15                         ((uint32_t)0x00008000)

/********************  Bits definition for DMA_SxFCR register  ****************/ 
#define DMA_SxFCR_FEIE                       ((uint32_t)0x00000080)
#define DMA_SxFCR_FS                         ((uint32_t)0x00000038)
#define DMA_SxFCR_FS_0                       ((uint32_t)0x00000008)
#define DMA_SxFCR_FS_1                       ((uint32_t)0x00000010)
#define DMA_SxFCR_FS_2                       ((uint32_t)0x00000020)
#define DMA_SxFCR_DMDIS                      ((uint32_t)0x00000004)
#define DMA_SxFCR_FTH                        ((uint32_t)0x00000003)
#define DMA_SxFCR_FTH_0                      ((uint32_t)0x00000001)
#define DMA_SxFCR_FTH_1                      ((uint32_t)0x00000002)

/********************  Bits definition for DMA_LISR register  *****************/ 
#define DMA_LISR_TCIF3                       ((uint32_t)0x08000000)
#define DMA_LISR_HTIF3                       ((uint32_t)0x04000000)
#define DMA_LISR_TEIF3                       ((uint32_t)0x02000000)
#define DMA_LISR_DMEIF3                      ((uint32_t)0x01000000)
#define DMA_LISR_FEIF3                       ((uint32_t)0x00400000)
#define DMA_LISR_TCIF2                       ((uint32_t)0x00200000)
#define DMA_LISR_HTIF2                       ((uint32_t)0x00100000)
#define DMA_LISR_TEIF2                       ((uint32_t)0x00080000)
#define DMA_LISR_DMEIF2                      ((uint32_t)0x00040000)
#define DMA_LISR_FEIF2                       ((uint32_t)0x00010000)
#define DMA_LISR_TCIF1                       ((uint32_t)0x00000800)
#define DMA_LISR_HTIF1                       ((uint32_t)0x00000400)
#define DMA_LISR_TEIF1                       ((uint32_t)0x00000200)
#define DMA_LISR_DMEIF1                      ((uint32_t)0x00000100)
#define DMA_LISR_FEIF1                       ((uint32_t)0x00000040)
#define DMA_LISR_TCIF0                       ((uint32_t)0x00000020)
#define DMA_LISR_HTIF0                       ((uint32_t)0x00000010)
#define DMA_LISR_TEIF0                       ((uint32_t)0x00000008)
#define DMA_LISR_DMEIF0                      ((uint32_t)0x00000004)
#define DMA_LISR_FEIF0                       ((uint32_t)0x00000001)

/********************  Bits definition for DMA_HISR register  *****************/ 
#define DMA_HISR_TCIF7                       ((uint32_t)0x08000000)
#define DMA_HISR_HTIF7                       ((uint32_t)0x04000000)
#define DMA_HISR_TEIF7                       ((uint32_t)0x02000000)
#define DMA_HISR_DMEIF7                      ((uint32_t)0x01000000)
#define DMA_HISR_FEIF7                       ((uint32_t)0x00400000)
#define DMA_HISR_TCIF6                       ((uint32_t)0x00200000)
#define DMA_HISR_HTIF6                       ((uint32_t)0x00100000)
#define DMA_HISR_TEIF6                       ((uint32_t)0x00080000)
#define DMA_HISR_DMEIF6                      ((uint32_t)0x00040000)
#define DMA_HISR_FEIF6                       ((uint32_t)0x00010000)
#define DMA_HISR_TCIF5                       ((uint32_t)0x00000800)
#define DMA_HISR_HTIF5                       ((uint32_t)0x00000400)
#define DMA_HISR_TEIF5                       ((uint32_t)0x00000200)
#define DMA_HISR_DMEIF5                      ((uint32_t)0x00000100)
#define DMA_HISR_FEIF5                       ((uint32_t)0x00000040)
#define DMA_HISR_TCIF4                       ((uint32_t)0x00000020)
#define DMA_HISR_HTIF4                       ((uint32_t)0x00000010)
#define DMA_HISR_TEIF4                       ((uint32_t)0x00000008)
#define DMA_HISR_DMEIF4                      ((uint32_t)0x00000004)
#define DMA_HISR_FEIF4                       ((uint32_t)0x00000001)

/********************  Bits definition for DMA_LIFCR register  ****************/ 
#define DMA_LIFCR_CTCIF3                     ((uint32_t)0x08000000)
#define DMA_LIFCR_CHTIF3                     ((uint32_t)0x04000000)
#define DMA_LIFCR_CTEIF3                     ((uint32_t)0x02000000)
#define DMA_LIFCR_CDMEIF3                    ((uint32_t)0x01000000)
#define DMA_LIFCR_CFEIF3                     ((uint32_t)0x00400000)
#define DMA_LIFCR_CTCIF2                     ((uint32_t)0x00200000)
#define DMA_LIFCR_CHTIF2                     ((uint32_t)0x00100000)
#define DMA_LIFCR_CTEIF2                     ((uint32_t)0x00080000)
#define DMA_LIFCR_CDMEIF2                    ((uint32_t)0x00040000)
#define DMA_LIFCR_CFEIF2                     ((uint32_t)0x00010000)
#define DMA_LIFCR_CTCIF1                     ((uint32_t)0x00000800)
#define DMA_LIFCR_CHTIF1                     ((uint32_t)0x00000400)
#define DMA_LIFCR_CTEIF1                     ((uint32_t)0x00000200)
#define DMA_LIFCR_CDMEIF1                    ((uint32_t)0x00000100)
#define DMA_LIFCR_CFEIF1                     ((uint32_t)0x00000040)
#define DMA_LIFCR_CTCIF0                     ((uint32_t)0x00000020)
#define DMA_LIFCR_CHTIF0                     ((uint32_t)0x00000010)
#define DMA_LIFCR_CTEIF0                     ((uint32_t)0x00000008)
#define DMA_LIFCR_CDMEIF0                    ((uint32_t)0x00000004)
#define DMA_LIFCR_CFEIF0                     ((uint32_t)0x00000001)

/********************  Bits definition for DMA_HIFCR  register  ****************/ 
#define DMA_HIFCR_CTCIF7                     ((uint32_t)0x08000000)
#define DMA_HIFCR_CHTIF7                     ((uint32_t)0x04000000)
#define DMA_HIFCR_CTEIF7                     ((uint32_t)0x02000000)
#define DMA_HIFCR_CDMEIF7                    ((uint32_t)0x01000000)
#define DMA_HIFCR_CFEIF7                     ((uint32_t)0x00400000)
#define DMA_HIFCR_CTCIF6                     ((uint32_t)0x00200000)
#define DMA_HIFCR_CHTIF6                     ((uint32_t)0x00100000)
#define DMA_HIFCR_CTEIF6                     ((uint32_t)0x00080000)
#define DMA_HIFCR_CDMEIF6                    ((uint32_t)0x00040000)
#define DMA_HIFCR_CFEIF6                     ((uint32_t)0x00010000)
#define DMA_HIFCR_CTCIF5                     ((uint32_t)0x00000800)
#define DMA_HIFCR_CHTIF5                     ((uint32_t)0x00000400)
#define DMA_HIFCR_CTEIF5                     ((uint32_t)0x00000200)
#define DMA_HIFCR_CDMEIF5                    ((uint32_t)0x00000100)
#define DMA_HIFCR_CFEIF5                     ((uint32_t)0x00000040)
#define DMA_HIFCR_CTCIF4                     ((uint32_t)0x00000020)
#define DMA_HIFCR_CHTIF4                     ((uint32_t)0x00000010)
#define DMA_HIFCR_CTEIF4                     ((uint32_t)0x00000008)
#define DMA_HIFCR_CDMEIF4                    ((uint32_t)0x00000004)
#define DMA_HIFCR_CFEIF4                     ((uint32_t)0x00000001)

/******************************************************************************/
/*                                                                            */
/*                         AHB Master DMA2D Controller (DMA2D)                */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for DMA2D_CR register  ******************/

#define DMA2D_CR_START                     ((uint32_t)0x00000001)               /*!< Start transfer */
#define DMA2D_CR_SUSP                      ((uint32_t)0x00000002)               /*!< Suspend transfer */
#define DMA2D_CR_ABORT                     ((uint32_t)0x00000004)               /*!< Abort transfer */
#define DMA2D_CR_TEIE                      ((uint32_t)0x00000100)               /*!< Transfer Error Interrupt Enable */
#define DMA2D_CR_TCIE                      ((uint32_t)0x00000200)               /*!< Transfer Complete Interrupt Enable */
#define DMA2D_CR_TWIE                      ((uint32_t)0x00000400)               /*!< Transfer Watermark Interrupt Enable */
#define DMA2D_CR_CAEIE                     ((uint32_t)0x00000800)               /*!< CLUT Access Error Interrupt Enable */
#define DMA2D_CR_CTCIE                     ((uint32_t)0x00001000)               /*!< CLUT Transfer Complete Interrupt Enable */
#define DMA2D_CR_CEIE                      ((uint32_t)0x00002000)               /*!< Configuration Error Interrupt Enable */
#define DMA2D_CR_MODE                      ((uint32_t)0x00030000)               /*!< DMA2D Mode */

/********************  Bit definition for DMA2D_ISR register  *****************/

#define DMA2D_ISR_TEIF                     ((uint32_t)0x00000001)               /*!< Transfer Error Interrupt Flag */
#define DMA2D_ISR_TCIF                     ((uint32_t)0x00000002)               /*!< Transfer Complete Interrupt Flag */
#define DMA2D_ISR_TWIF                     ((uint32_t)0x00000004)               /*!< Transfer Watermark Interrupt Flag */
#define DMA2D_ISR_CAEIF                    ((uint32_t)0x00000008)               /*!< CLUT Access Error Interrupt Flag */
#define DMA2D_ISR_CTCIF                    ((uint32_t)0x00000010)               /*!< CLUT Transfer Complete Interrupt Flag */
#define DMA2D_ISR_CEIF                     ((uint32_t)0x00000020)               /*!< Configuration Error Interrupt Flag */

/********************  Bit definition for DMA2D_IFSR register  ****************/

#define DMA2D_IFSR_CTEIF                   ((uint32_t)0x00000001)               /*!< Clears Transfer Error Interrupt Flag */
#define DMA2D_IFSR_CTCIF                   ((uint32_t)0x00000002)               /*!< Clears Transfer Complete Interrupt Flag */
#define DMA2D_IFSR_CTWIF                   ((uint32_t)0x00000004)               /*!< Clears Transfer Watermark Interrupt Flag */
#define DMA2D_IFSR_CCAEIF                  ((uint32_t)0x00000008)               /*!< Clears CLUT Access Error Interrupt Flag */
#define DMA2D_IFSR_CCTCIF                  ((uint32_t)0x00000010)               /*!< Clears CLUT Transfer Complete Interrupt Flag */
#define DMA2D_IFSR_CCEIF                   ((uint32_t)0x00000020)               /*!< Clears Configuration Error Interrupt Flag */

/********************  Bit definition for DMA2D_FGMAR register  ***************/

#define DMA2D_FGMAR_MA                     ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_FGOR register  ****************/

#define DMA2D_FGOR_LO                      ((uint32_t)0x00003FFF)               /*!< Line Offset */

/********************  Bit definition for DMA2D_BGMAR register  ***************/

#define DMA2D_BGMAR_MA                     ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGOR register  ****************/

#define DMA2D_BGOR_LO                      ((uint32_t)0x00003FFF)               /*!< Line Offset */

/********************  Bit definition for DMA2D_FGPFCCR register  *************/

#define DMA2D_FGPFCCR_CM                   ((uint32_t)0x0000000F)               /*!< Color mode */
#define DMA2D_FGPFCCR_CCM                  ((uint32_t)0x00000010)               /*!< CLUT Color mode */
#define DMA2D_FGPFCCR_START                ((uint32_t)0x00000020)               /*!< Start */
#define DMA2D_FGPFCCR_CS                   ((uint32_t)0x0000FF00)               /*!< CLUT size */
#define DMA2D_FGPFCCR_AM                   ((uint32_t)0x00030000)               /*!< Alpha mode */
#define DMA2D_FGPFCCR_ALPHA                ((uint32_t)0xFF000000)               /*!< Alpha value */

/********************  Bit definition for DMA2D_FGCOLR register  **************/

#define DMA2D_FGCOLR_BLUE                  ((uint32_t)0x000000FF)               /*!< Blue Value */
#define DMA2D_FGCOLR_GREEN                 ((uint32_t)0x0000FF00)               /*!< Green Value */
#define DMA2D_FGCOLR_RED                   ((uint32_t)0x00FF0000)               /*!< Red Value */   

/********************  Bit definition for DMA2D_BGPFCCR register  *************/

#define DMA2D_BGPFCCR_CM                   ((uint32_t)0x0000000F)               /*!< Color mode */
#define DMA2D_BGPFCCR_CCM                  ((uint32_t)0x00000010)               /*!< CLUT Color mode */
#define DMA2D_BGPFCCR_START                ((uint32_t)0x00000020)               /*!< Start */
#define DMA2D_BGPFCCR_CS                   ((uint32_t)0x0000FF00)               /*!< CLUT size */
#define DMA2D_BGPFCCR_AM                   ((uint32_t)0x00030000)               /*!< Alpha Mode */
#define DMA2D_BGPFCCR_ALPHA                ((uint32_t)0xFF000000)               /*!< Alpha value */

/********************  Bit definition for DMA2D_BGCOLR register  **************/

#define DMA2D_BGCOLR_BLUE                  ((uint32_t)0x000000FF)               /*!< Blue Value */
#define DMA2D_BGCOLR_GREEN                 ((uint32_t)0x0000FF00)               /*!< Green Value */
#define DMA2D_BGCOLR_RED                   ((uint32_t)0x00FF0000)               /*!< Red Value */

/********************  Bit definition for DMA2D_FGCMAR register  **************/

#define DMA2D_FGCMAR_MA                    ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGCMAR register  **************/

#define DMA2D_BGCMAR_MA                    ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_OPFCCR register  **************/

#define DMA2D_OPFCCR_CM                    ((uint32_t)0x00000007)               /*!< Color mode */

/********************  Bit definition for DMA2D_OCOLR register  ***************/

/*!<Mode_ARGB8888/RGB888 */

#define DMA2D_OCOLR_BLUE_1                 ((uint32_t)0x000000FF)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_1                ((uint32_t)0x0000FF00)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_1                  ((uint32_t)0x00FF0000)               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_1                ((uint32_t)0xFF000000)               /*!< Alpha Channel Value */

/*!<Mode_RGB565 */
#define DMA2D_OCOLR_BLUE_2                 ((uint32_t)0x0000001F)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_2                ((uint32_t)0x000007E0)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_2                  ((uint32_t)0x0000F800)               /*!< Red Value */

/*!<Mode_ARGB1555 */
#define DMA2D_OCOLR_BLUE_3                 ((uint32_t)0x0000001F)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_3                ((uint32_t)0x000003E0)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_3                  ((uint32_t)0x00007C00)               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_3                ((uint32_t)0x00008000)               /*!< Alpha Channel Value */

/*!<Mode_ARGB4444 */
#define DMA2D_OCOLR_BLUE_4                 ((uint32_t)0x0000000F)               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_4                ((uint32_t)0x000000F0)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_4                  ((uint32_t)0x00000F00)               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_4                ((uint32_t)0x0000F000)               /*!< Alpha Channel Value */

/********************  Bit definition for DMA2D_OMAR register  ****************/

#define DMA2D_OMAR_MA                      ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_OOR register  *****************/

#define DMA2D_OOR_LO                       ((uint32_t)0x00003FFF)               /*!< Line Offset */

/********************  Bit definition for DMA2D_NLR register  *****************/

#define DMA2D_NLR_NL                       ((uint32_t)0x0000FFFF)               /*!< Number of Lines */
#define DMA2D_NLR_PL                       ((uint32_t)0x3FFF0000)               /*!< Pixel per Lines */

/********************  Bit definition for DMA2D_LWR register  *****************/

#define DMA2D_LWR_LW                       ((uint32_t)0x0000FFFF)               /*!< Line Watermark */

/********************  Bit definition for DMA2D_AMTCR register  ***************/

#define DMA2D_AMTCR_EN                     ((uint32_t)0x00000001)               /*!< Enable */
#define DMA2D_AMTCR_DT                     ((uint32_t)0x0000FF00)               /*!< Dead Time */



/********************  Bit definition for DMA2D_FGCLUT register  **************/
                                                                     
/********************  Bit definition for DMA2D_BGCLUT register  **************/


/******************************************************************************/
/*                                                                            */
/*                    External Interrupt/Event Controller                     */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for EXTI_IMR register  *******************/
#define  EXTI_IMR_MR0                        ((uint32_t)0x00000001)        /*!< Interrupt Mask on line 0 */
#define  EXTI_IMR_MR1                        ((uint32_t)0x00000002)        /*!< Interrupt Mask on line 1 */
#define  EXTI_IMR_MR2                        ((uint32_t)0x00000004)        /*!< Interrupt Mask on line 2 */
#define  EXTI_IMR_MR3                        ((uint32_t)0x00000008)        /*!< Interrupt Mask on line 3 */
#define  EXTI_IMR_MR4                        ((uint32_t)0x00000010)        /*!< Interrupt Mask on line 4 */
#define  EXTI_IMR_MR5                        ((uint32_t)0x00000020)        /*!< Interrupt Mask on line 5 */
#define  EXTI_IMR_MR6                        ((uint32_t)0x00000040)        /*!< Interrupt Mask on line 6 */
#define  EXTI_IMR_MR7                        ((uint32_t)0x00000080)        /*!< Interrupt Mask on line 7 */
#define  EXTI_IMR_MR8                        ((uint32_t)0x00000100)        /*!< Interrupt Mask on line 8 */
#define  EXTI_IMR_MR9                        ((uint32_t)0x00000200)        /*!< Interrupt Mask on line 9 */
#define  EXTI_IMR_MR10                       ((uint32_t)0x00000400)        /*!< Interrupt Mask on line 10 */
#define  EXTI_IMR_MR11                       ((uint32_t)0x00000800)        /*!< Interrupt Mask on line 11 */
#define  EXTI_IMR_MR12                       ((uint32_t)0x00001000)        /*!< Interrupt Mask on line 12 */
#define  EXTI_IMR_MR13                       ((uint32_t)0x00002000)        /*!< Interrupt Mask on line 13 */
#define  EXTI_IMR_MR14                       ((uint32_t)0x00004000)        /*!< Interrupt Mask on line 14 */
#define  EXTI_IMR_MR15                       ((uint32_t)0x00008000)        /*!< Interrupt Mask on line 15 */
#define  EXTI_IMR_MR16                       ((uint32_t)0x00010000)        /*!< Interrupt Mask on line 16 */
#define  EXTI_IMR_MR17                       ((uint32_t)0x00020000)        /*!< Interrupt Mask on line 17 */
#define  EXTI_IMR_MR18                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
#define  EXTI_IMR_MR19                       ((uint32_t)0x00080000)        /*!< Interrupt Mask on line 19 */

/*******************  Bit definition for EXTI_EMR register  *******************/
#define  EXTI_EMR_MR0                        ((uint32_t)0x00000001)        /*!< Event Mask on line 0 */
#define  EXTI_EMR_MR1                        ((uint32_t)0x00000002)        /*!< Event Mask on line 1 */
#define  EXTI_EMR_MR2                        ((uint32_t)0x00000004)        /*!< Event Mask on line 2 */
#define  EXTI_EMR_MR3                        ((uint32_t)0x00000008)        /*!< Event Mask on line 3 */
#define  EXTI_EMR_MR4                        ((uint32_t)0x00000010)        /*!< Event Mask on line 4 */
#define  EXTI_EMR_MR5                        ((uint32_t)0x00000020)        /*!< Event Mask on line 5 */
#define  EXTI_EMR_MR6                        ((uint32_t)0x00000040)        /*!< Event Mask on line 6 */
#define  EXTI_EMR_MR7                        ((uint32_t)0x00000080)        /*!< Event Mask on line 7 */
#define  EXTI_EMR_MR8                        ((uint32_t)0x00000100)        /*!< Event Mask on line 8 */
#define  EXTI_EMR_MR9                        ((uint32_t)0x00000200)        /*!< Event Mask on line 9 */
#define  EXTI_EMR_MR10                       ((uint32_t)0x00000400)        /*!< Event Mask on line 10 */
#define  EXTI_EMR_MR11                       ((uint32_t)0x00000800)        /*!< Event Mask on line 11 */
#define  EXTI_EMR_MR12                       ((uint32_t)0x00001000)        /*!< Event Mask on line 12 */
#define  EXTI_EMR_MR13                       ((uint32_t)0x00002000)        /*!< Event Mask on line 13 */
#define  EXTI_EMR_MR14                       ((uint32_t)0x00004000)        /*!< Event Mask on line 14 */
#define  EXTI_EMR_MR15                       ((uint32_t)0x00008000)        /*!< Event Mask on line 15 */
#define  EXTI_EMR_MR16                       ((uint32_t)0x00010000)        /*!< Event Mask on line 16 */
#define  EXTI_EMR_MR17                       ((uint32_t)0x00020000)        /*!< Event Mask on line 17 */
#define  EXTI_EMR_MR18                       ((uint32_t)0x00040000)        /*!< Event Mask on line 18 */
#define  EXTI_EMR_MR19                       ((uint32_t)0x00080000)        /*!< Event Mask on line 19 */

/******************  Bit definition for EXTI_RTSR register  *******************/
#define  EXTI_RTSR_TR0                       ((uint32_t)0x00000001)        /*!< Rising trigger event configuration bit of line 0 */
#define  EXTI_RTSR_TR1                       ((uint32_t)0x00000002)        /*!< Rising trigger event configuration bit of line 1 */
#define  EXTI_RTSR_TR2                       ((uint32_t)0x00000004)        /*!< Rising trigger event configuration bit of line 2 */
#define  EXTI_RTSR_TR3                       ((uint32_t)0x00000008)        /*!< Rising trigger event configuration bit of line 3 */
#define  EXTI_RTSR_TR4                       ((uint32_t)0x00000010)        /*!< Rising trigger event configuration bit of line 4 */
#define  EXTI_RTSR_TR5                       ((uint32_t)0x00000020)        /*!< Rising trigger event configuration bit of line 5 */
#define  EXTI_RTSR_TR6                       ((uint32_t)0x00000040)        /*!< Rising trigger event configuration bit of line 6 */
#define  EXTI_RTSR_TR7                       ((uint32_t)0x00000080)        /*!< Rising trigger event configuration bit of line 7 */
#define  EXTI_RTSR_TR8                       ((uint32_t)0x00000100)        /*!< Rising trigger event configuration bit of line 8 */
#define  EXTI_RTSR_TR9                       ((uint32_t)0x00000200)        /*!< Rising trigger event configuration bit of line 9 */
#define  EXTI_RTSR_TR10                      ((uint32_t)0x00000400)        /*!< Rising trigger event configuration bit of line 10 */
#define  EXTI_RTSR_TR11                      ((uint32_t)0x00000800)        /*!< Rising trigger event configuration bit of line 11 */
#define  EXTI_RTSR_TR12                      ((uint32_t)0x00001000)        /*!< Rising trigger event configuration bit of line 12 */
#define  EXTI_RTSR_TR13                      ((uint32_t)0x00002000)        /*!< Rising trigger event configuration bit of line 13 */
#define  EXTI_RTSR_TR14                      ((uint32_t)0x00004000)        /*!< Rising trigger event configuration bit of line 14 */
#define  EXTI_RTSR_TR15                      ((uint32_t)0x00008000)        /*!< Rising trigger event configuration bit of line 15 */
#define  EXTI_RTSR_TR16                      ((uint32_t)0x00010000)        /*!< Rising trigger event configuration bit of line 16 */
#define  EXTI_RTSR_TR17                      ((uint32_t)0x00020000)        /*!< Rising trigger event configuration bit of line 17 */
#define  EXTI_RTSR_TR18                      ((uint32_t)0x00040000)        /*!< Rising trigger event configuration bit of line 18 */
#define  EXTI_RTSR_TR19                      ((uint32_t)0x00080000)        /*!< Rising trigger event configuration bit of line 19 */

/******************  Bit definition for EXTI_FTSR register  *******************/
#define  EXTI_FTSR_TR0                       ((uint32_t)0x00000001)        /*!< Falling trigger event configuration bit of line 0 */
#define  EXTI_FTSR_TR1                       ((uint32_t)0x00000002)        /*!< Falling trigger event configuration bit of line 1 */
#define  EXTI_FTSR_TR2                       ((uint32_t)0x00000004)        /*!< Falling trigger event configuration bit of line 2 */
#define  EXTI_FTSR_TR3                       ((uint32_t)0x00000008)        /*!< Falling trigger event configuration bit of line 3 */
#define  EXTI_FTSR_TR4                       ((uint32_t)0x00000010)        /*!< Falling trigger event configuration bit of line 4 */
#define  EXTI_FTSR_TR5                       ((uint32_t)0x00000020)        /*!< Falling trigger event configuration bit of line 5 */
#define  EXTI_FTSR_TR6                       ((uint32_t)0x00000040)        /*!< Falling trigger event configuration bit of line 6 */
#define  EXTI_FTSR_TR7                       ((uint32_t)0x00000080)        /*!< Falling trigger event configuration bit of line 7 */
#define  EXTI_FTSR_TR8                       ((uint32_t)0x00000100)        /*!< Falling trigger event configuration bit of line 8 */
#define  EXTI_FTSR_TR9                       ((uint32_t)0x00000200)        /*!< Falling trigger event configuration bit of line 9 */
#define  EXTI_FTSR_TR10                      ((uint32_t)0x00000400)        /*!< Falling trigger event configuration bit of line 10 */
#define  EXTI_FTSR_TR11                      ((uint32_t)0x00000800)        /*!< Falling trigger event configuration bit of line 11 */
#define  EXTI_FTSR_TR12                      ((uint32_t)0x00001000)        /*!< Falling trigger event configuration bit of line 12 */
#define  EXTI_FTSR_TR13                      ((uint32_t)0x00002000)        /*!< Falling trigger event configuration bit of line 13 */
#define  EXTI_FTSR_TR14                      ((uint32_t)0x00004000)        /*!< Falling trigger event configuration bit of line 14 */
#define  EXTI_FTSR_TR15                      ((uint32_t)0x00008000)        /*!< Falling trigger event configuration bit of line 15 */
#define  EXTI_FTSR_TR16                      ((uint32_t)0x00010000)        /*!< Falling trigger event configuration bit of line 16 */
#define  EXTI_FTSR_TR17                      ((uint32_t)0x00020000)        /*!< Falling trigger event configuration bit of line 17 */
#define  EXTI_FTSR_TR18                      ((uint32_t)0x00040000)        /*!< Falling trigger event configuration bit of line 18 */
#define  EXTI_FTSR_TR19                      ((uint32_t)0x00080000)        /*!< Falling trigger event configuration bit of line 19 */

/******************  Bit definition for EXTI_SWIER register  ******************/
#define  EXTI_SWIER_SWIER0                   ((uint32_t)0x00000001)        /*!< Software Interrupt on line 0 */
#define  EXTI_SWIER_SWIER1                   ((uint32_t)0x00000002)        /*!< Software Interrupt on line 1 */
#define  EXTI_SWIER_SWIER2                   ((uint32_t)0x00000004)        /*!< Software Interrupt on line 2 */
#define  EXTI_SWIER_SWIER3                   ((uint32_t)0x00000008)        /*!< Software Interrupt on line 3 */
#define  EXTI_SWIER_SWIER4                   ((uint32_t)0x00000010)        /*!< Software Interrupt on line 4 */
#define  EXTI_SWIER_SWIER5                   ((uint32_t)0x00000020)        /*!< Software Interrupt on line 5 */
#define  EXTI_SWIER_SWIER6                   ((uint32_t)0x00000040)        /*!< Software Interrupt on line 6 */
#define  EXTI_SWIER_SWIER7                   ((uint32_t)0x00000080)        /*!< Software Interrupt on line 7 */
#define  EXTI_SWIER_SWIER8                   ((uint32_t)0x00000100)        /*!< Software Interrupt on line 8 */
#define  EXTI_SWIER_SWIER9                   ((uint32_t)0x00000200)        /*!< Software Interrupt on line 9 */
#define  EXTI_SWIER_SWIER10                  ((uint32_t)0x00000400)        /*!< Software Interrupt on line 10 */
#define  EXTI_SWIER_SWIER11                  ((uint32_t)0x00000800)        /*!< Software Interrupt on line 11 */
#define  EXTI_SWIER_SWIER12                  ((uint32_t)0x00001000)        /*!< Software Interrupt on line 12 */
#define  EXTI_SWIER_SWIER13                  ((uint32_t)0x00002000)        /*!< Software Interrupt on line 13 */
#define  EXTI_SWIER_SWIER14                  ((uint32_t)0x00004000)        /*!< Software Interrupt on line 14 */
#define  EXTI_SWIER_SWIER15                  ((uint32_t)0x00008000)        /*!< Software Interrupt on line 15 */
#define  EXTI_SWIER_SWIER16                  ((uint32_t)0x00010000)        /*!< Software Interrupt on line 16 */
#define  EXTI_SWIER_SWIER17                  ((uint32_t)0x00020000)        /*!< Software Interrupt on line 17 */
#define  EXTI_SWIER_SWIER18                  ((uint32_t)0x00040000)        /*!< Software Interrupt on line 18 */
#define  EXTI_SWIER_SWIER19                  ((uint32_t)0x00080000)        /*!< Software Interrupt on line 19 */

/*******************  Bit definition for EXTI_PR register  ********************/
#define  EXTI_PR_PR0                         ((uint32_t)0x00000001)        /*!< Pending bit for line 0 */
#define  EXTI_PR_PR1                         ((uint32_t)0x00000002)        /*!< Pending bit for line 1 */
#define  EXTI_PR_PR2                         ((uint32_t)0x00000004)        /*!< Pending bit for line 2 */
#define  EXTI_PR_PR3                         ((uint32_t)0x00000008)        /*!< Pending bit for line 3 */
#define  EXTI_PR_PR4                         ((uint32_t)0x00000010)        /*!< Pending bit for line 4 */
#define  EXTI_PR_PR5                         ((uint32_t)0x00000020)        /*!< Pending bit for line 5 */
#define  EXTI_PR_PR6                         ((uint32_t)0x00000040)        /*!< Pending bit for line 6 */
#define  EXTI_PR_PR7                         ((uint32_t)0x00000080)        /*!< Pending bit for line 7 */
#define  EXTI_PR_PR8                         ((uint32_t)0x00000100)        /*!< Pending bit for line 8 */
#define  EXTI_PR_PR9                         ((uint32_t)0x00000200)        /*!< Pending bit for line 9 */
#define  EXTI_PR_PR10                        ((uint32_t)0x00000400)        /*!< Pending bit for line 10 */
#define  EXTI_PR_PR11                        ((uint32_t)0x00000800)        /*!< Pending bit for line 11 */
#define  EXTI_PR_PR12                        ((uint32_t)0x00001000)        /*!< Pending bit for line 12 */
#define  EXTI_PR_PR13                        ((uint32_t)0x00002000)        /*!< Pending bit for line 13 */
#define  EXTI_PR_PR14                        ((uint32_t)0x00004000)        /*!< Pending bit for line 14 */
#define  EXTI_PR_PR15                        ((uint32_t)0x00008000)        /*!< Pending bit for line 15 */
#define  EXTI_PR_PR16                        ((uint32_t)0x00010000)        /*!< Pending bit for line 16 */
#define  EXTI_PR_PR17                        ((uint32_t)0x00020000)        /*!< Pending bit for line 17 */
#define  EXTI_PR_PR18                        ((uint32_t)0x00040000)        /*!< Pending bit for line 18 */
#define  EXTI_PR_PR19                        ((uint32_t)0x00080000)        /*!< Pending bit for line 19 */

/******************************************************************************/
/*                                                                            */
/*                                    FLASH                                   */
/*                                                                            */
/******************************************************************************/
/*******************  Bits definition for FLASH_ACR register  *****************/
#define FLASH_ACR_LATENCY                    ((uint32_t)0x0000000F)
#define FLASH_ACR_LATENCY_0WS                ((uint32_t)0x00000000)
#define FLASH_ACR_LATENCY_1WS                ((uint32_t)0x00000001)
#define FLASH_ACR_LATENCY_2WS                ((uint32_t)0x00000002)
#define FLASH_ACR_LATENCY_3WS                ((uint32_t)0x00000003)
#define FLASH_ACR_LATENCY_4WS                ((uint32_t)0x00000004)
#define FLASH_ACR_LATENCY_5WS                ((uint32_t)0x00000005)
#define FLASH_ACR_LATENCY_6WS                ((uint32_t)0x00000006)
#define FLASH_ACR_LATENCY_7WS                ((uint32_t)0x00000007)
#define FLASH_ACR_LATENCY_8WS                ((uint32_t)0x00000008)
#define FLASH_ACR_LATENCY_9WS                ((uint32_t)0x00000009)
#define FLASH_ACR_LATENCY_10WS               ((uint32_t)0x0000000A)
#define FLASH_ACR_LATENCY_11WS               ((uint32_t)0x0000000B)
#define FLASH_ACR_LATENCY_12WS               ((uint32_t)0x0000000C)
#define FLASH_ACR_LATENCY_13WS               ((uint32_t)0x0000000D)
#define FLASH_ACR_LATENCY_14WS               ((uint32_t)0x0000000E)
#define FLASH_ACR_LATENCY_15WS               ((uint32_t)0x0000000F)

#define FLASH_ACR_PRFTEN                     ((uint32_t)0x00000100)
#define FLASH_ACR_ICEN                       ((uint32_t)0x00000200)
#define FLASH_ACR_DCEN                       ((uint32_t)0x00000400)
#define FLASH_ACR_ICRST                      ((uint32_t)0x00000800)
#define FLASH_ACR_DCRST                      ((uint32_t)0x00001000)
#define FLASH_ACR_BYTE0_ADDRESS              ((uint32_t)0x40023C00)
#define FLASH_ACR_BYTE2_ADDRESS              ((uint32_t)0x40023C03)

/*******************  Bits definition for FLASH_SR register  ******************/
#define FLASH_SR_EOP                         ((uint32_t)0x00000001)
#define FLASH_SR_SOP                         ((uint32_t)0x00000002)
#define FLASH_SR_WRPERR                      ((uint32_t)0x00000010)
#define FLASH_SR_PGAERR                      ((uint32_t)0x00000020)
#define FLASH_SR_PGPERR                      ((uint32_t)0x00000040)
#define FLASH_SR_PGSERR                      ((uint32_t)0x00000080)
#define FLASH_SR_BSY                         ((uint32_t)0x00010000)

/*******************  Bits definition for FLASH_CR register  ******************/
#define FLASH_CR_PG                          ((uint32_t)0x00000001)
#define FLASH_CR_SER                         ((uint32_t)0x00000002)
#define FLASH_CR_MER                         ((uint32_t)0x00000004)
#define FLASH_CR_MER1                        FLASH_CR_MER
#define FLASH_CR_SNB                         ((uint32_t)0x000000F8)
#define FLASH_CR_SNB_0                       ((uint32_t)0x00000008)
#define FLASH_CR_SNB_1                       ((uint32_t)0x00000010)
#define FLASH_CR_SNB_2                       ((uint32_t)0x00000020)
#define FLASH_CR_SNB_3                       ((uint32_t)0x00000040)
#define FLASH_CR_SNB_4                       ((uint32_t)0x00000040)
#define FLASH_CR_PSIZE                       ((uint32_t)0x00000300)
#define FLASH_CR_PSIZE_0                     ((uint32_t)0x00000100)
#define FLASH_CR_PSIZE_1                     ((uint32_t)0x00000200)
#define FLASH_CR_MER2                        ((uint32_t)0x00008000)
#define FLASH_CR_STRT                        ((uint32_t)0x00010000)
#define FLASH_CR_EOPIE                       ((uint32_t)0x01000000)
#define FLASH_CR_LOCK                        ((uint32_t)0x80000000)

/*******************  Bits definition for FLASH_OPTCR register  ***************/
#define FLASH_OPTCR_OPTLOCK                 ((uint32_t)0x00000001)
#define FLASH_OPTCR_OPTSTRT                 ((uint32_t)0x00000002)
#define FLASH_OPTCR_BOR_LEV_0               ((uint32_t)0x00000004)
#define FLASH_OPTCR_BOR_LEV_1               ((uint32_t)0x00000008)
#define FLASH_OPTCR_BOR_LEV                 ((uint32_t)0x0000000C)
#define FLASH_OPTCR_BFB2                    ((uint32_t)0x00000010)

#define FLASH_OPTCR_WDG_SW                  ((uint32_t)0x00000020)
#define FLASH_OPTCR_nRST_STOP               ((uint32_t)0x00000040)
#define FLASH_OPTCR_nRST_STDBY              ((uint32_t)0x00000080)
#define FLASH_OPTCR_RDP                     ((uint32_t)0x0000FF00)
#define FLASH_OPTCR_RDP_0                   ((uint32_t)0x00000100)
#define FLASH_OPTCR_RDP_1                   ((uint32_t)0x00000200)
#define FLASH_OPTCR_RDP_2                   ((uint32_t)0x00000400)
#define FLASH_OPTCR_RDP_3                   ((uint32_t)0x00000800)
#define FLASH_OPTCR_RDP_4                   ((uint32_t)0x00001000)
#define FLASH_OPTCR_RDP_5                   ((uint32_t)0x00002000)
#define FLASH_OPTCR_RDP_6                   ((uint32_t)0x00004000)
#define FLASH_OPTCR_RDP_7                   ((uint32_t)0x00008000)
#define FLASH_OPTCR_nWRP                    ((uint32_t)0x0FFF0000)
#define FLASH_OPTCR_nWRP_0                  ((uint32_t)0x00010000)
#define FLASH_OPTCR_nWRP_1                  ((uint32_t)0x00020000)
#define FLASH_OPTCR_nWRP_2                  ((uint32_t)0x00040000)
#define FLASH_OPTCR_nWRP_3                  ((uint32_t)0x00080000)
#define FLASH_OPTCR_nWRP_4                  ((uint32_t)0x00100000)
#define FLASH_OPTCR_nWRP_5                  ((uint32_t)0x00200000)
#define FLASH_OPTCR_nWRP_6                  ((uint32_t)0x00400000)
#define FLASH_OPTCR_nWRP_7                  ((uint32_t)0x00800000)
#define FLASH_OPTCR_nWRP_8                  ((uint32_t)0x01000000)
#define FLASH_OPTCR_nWRP_9                  ((uint32_t)0x02000000)
#define FLASH_OPTCR_nWRP_10                 ((uint32_t)0x04000000)
#define FLASH_OPTCR_nWRP_11                 ((uint32_t)0x08000000)

#define FLASH_OPTCR_DB1M                    ((uint32_t)0x40000000) 
#define FLASH_OPTCR_SPRMOD                  ((uint32_t)0x80000000) 
                                             
/******************  Bits definition for FLASH_OPTCR1 register  ***************/
#define FLASH_OPTCR1_nWRP                    ((uint32_t)0x0FFF0000)
#define FLASH_OPTCR1_nWRP_0                  ((uint32_t)0x00010000)
#define FLASH_OPTCR1_nWRP_1                  ((uint32_t)0x00020000)
#define FLASH_OPTCR1_nWRP_2                  ((uint32_t)0x00040000)
#define FLASH_OPTCR1_nWRP_3                  ((uint32_t)0x00080000)
#define FLASH_OPTCR1_nWRP_4                  ((uint32_t)0x00100000)
#define FLASH_OPTCR1_nWRP_5                  ((uint32_t)0x00200000)
#define FLASH_OPTCR1_nWRP_6                  ((uint32_t)0x00400000)
#define FLASH_OPTCR1_nWRP_7                  ((uint32_t)0x00800000)
#define FLASH_OPTCR1_nWRP_8                  ((uint32_t)0x01000000)
#define FLASH_OPTCR1_nWRP_9                  ((uint32_t)0x02000000)
#define FLASH_OPTCR1_nWRP_10                 ((uint32_t)0x04000000)
#define FLASH_OPTCR1_nWRP_11                 ((uint32_t)0x08000000)

#if defined (STM32F40_41xxx)
/******************************************************************************/
/*                                                                            */
/*                       Flexible Static Memory Controller                    */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for FSMC_BCR1 register  *******************/
#define  FSMC_BCR1_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FSMC_BCR1_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FSMC_BCR1_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FSMC_BCR1_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FSMC_BCR1_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FSMC_BCR1_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FSMC_BCR1_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BCR1_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FSMC_BCR1_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable                    */
#define  FSMC_BCR1_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit                       */
#define  FSMC_BCR1_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit               */
#define  FSMC_BCR1_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support             */
#define  FSMC_BCR1_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration              */
#define  FSMC_BCR1_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit                       */
#define  FSMC_BCR1_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit                        */
#define  FSMC_BCR1_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable                   */
#define  FSMC_BCR1_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait                      */
#define  FSMC_BCR1_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable                     */

/******************  Bit definition for FSMC_BCR2 register  *******************/
#define  FSMC_BCR2_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                */
#define  FSMC_BCR2_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FSMC_BCR2_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FSMC_BCR2_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FSMC_BCR2_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FSMC_BCR2_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FSMC_BCR2_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BCR2_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FSMC_BCR2_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable                    */
#define  FSMC_BCR2_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit                       */
#define  FSMC_BCR2_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit               */
#define  FSMC_BCR2_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support             */
#define  FSMC_BCR2_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration              */
#define  FSMC_BCR2_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit                       */
#define  FSMC_BCR2_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit                        */
#define  FSMC_BCR2_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable                   */
#define  FSMC_BCR2_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait                      */
#define  FSMC_BCR2_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable                     */

/******************  Bit definition for FSMC_BCR3 register  *******************/
#define  FSMC_BCR3_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FSMC_BCR3_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FSMC_BCR3_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FSMC_BCR3_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FSMC_BCR3_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FSMC_BCR3_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FSMC_BCR3_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BCR3_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FSMC_BCR3_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable                    */
#define  FSMC_BCR3_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit                       */
#define  FSMC_BCR3_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit               */
#define  FSMC_BCR3_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support             */
#define  FSMC_BCR3_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration              */
#define  FSMC_BCR3_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit                       */
#define  FSMC_BCR3_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit                        */
#define  FSMC_BCR3_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable                   */
#define  FSMC_BCR3_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait                      */
#define  FSMC_BCR3_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable                     */

/******************  Bit definition for FSMC_BCR4 register  *******************/
#define  FSMC_BCR4_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit */
#define  FSMC_BCR4_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FSMC_BCR4_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FSMC_BCR4_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FSMC_BCR4_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FSMC_BCR4_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FSMC_BCR4_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BCR4_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FSMC_BCR4_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable                    */
#define  FSMC_BCR4_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit                       */
#define  FSMC_BCR4_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit               */
#define  FSMC_BCR4_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support             */
#define  FSMC_BCR4_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration              */
#define  FSMC_BCR4_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit                       */
#define  FSMC_BCR4_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit                        */
#define  FSMC_BCR4_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable                   */
#define  FSMC_BCR4_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait                      */
#define  FSMC_BCR4_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable                     */

/******************  Bit definition for FSMC_BTR1 register  ******************/
#define  FSMC_BTR1_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FSMC_BTR1_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_BTR1_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_BTR1_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_BTR1_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FSMC_BTR1_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FSMC_BTR1_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BTR1_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FSMC_BTR1_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FSMC_BTR1_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FSMC_BTR1_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FSMC_BTR1_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_BTR1_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_BTR1_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_BTR1_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FSMC_BTR1_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FSMC_BTR1_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_BTR1_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_BTR1_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_BTR1_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FSMC_BTR1_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FSMC_BTR1_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FSMC_BTR1_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FSMC_BTR1_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FSMC_BTR1_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FSMC_BTR1_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FSMC_BTR1_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_BTR1_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_BTR1_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_BTR1_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FSMC_BTR1_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FSMC_BTR1_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FSMC_BTR1_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FSMC_BTR2 register  *******************/
#define  FSMC_BTR2_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FSMC_BTR2_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_BTR2_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_BTR2_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_BTR2_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FSMC_BTR2_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FSMC_BTR2_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BTR2_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FSMC_BTR2_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FSMC_BTR2_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FSMC_BTR2_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FSMC_BTR2_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_BTR2_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_BTR2_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_BTR2_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FSMC_BTR2_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FSMC_BTR2_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_BTR2_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_BTR2_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_BTR2_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FSMC_BTR2_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FSMC_BTR2_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FSMC_BTR2_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FSMC_BTR2_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FSMC_BTR2_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FSMC_BTR2_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FSMC_BTR2_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_BTR2_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_BTR2_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_BTR2_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FSMC_BTR2_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FSMC_BTR2_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FSMC_BTR2_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/*******************  Bit definition for FSMC_BTR3 register  *******************/
#define  FSMC_BTR3_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FSMC_BTR3_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_BTR3_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_BTR3_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_BTR3_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FSMC_BTR3_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FSMC_BTR3_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BTR3_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FSMC_BTR3_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FSMC_BTR3_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FSMC_BTR3_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FSMC_BTR3_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_BTR3_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_BTR3_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_BTR3_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FSMC_BTR3_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FSMC_BTR3_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_BTR3_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_BTR3_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_BTR3_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FSMC_BTR3_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FSMC_BTR3_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FSMC_BTR3_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FSMC_BTR3_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FSMC_BTR3_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FSMC_BTR3_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FSMC_BTR3_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_BTR3_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_BTR3_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_BTR3_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FSMC_BTR3_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FSMC_BTR3_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FSMC_BTR3_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FSMC_BTR4 register  *******************/
#define  FSMC_BTR4_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FSMC_BTR4_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_BTR4_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_BTR4_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_BTR4_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FSMC_BTR4_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FSMC_BTR4_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BTR4_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FSMC_BTR4_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FSMC_BTR4_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FSMC_BTR4_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FSMC_BTR4_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_BTR4_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_BTR4_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_BTR4_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FSMC_BTR4_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FSMC_BTR4_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_BTR4_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_BTR4_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_BTR4_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FSMC_BTR4_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FSMC_BTR4_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FSMC_BTR4_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FSMC_BTR4_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FSMC_BTR4_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FSMC_BTR4_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FSMC_BTR4_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_BTR4_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_BTR4_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_BTR4_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FSMC_BTR4_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FSMC_BTR4_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FSMC_BTR4_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FSMC_BWTR1 register  ******************/
#define  FSMC_BWTR1_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FSMC_BWTR1_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_BWTR1_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_BWTR1_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_BWTR1_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FSMC_BWTR1_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FSMC_BWTR1_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BWTR1_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FSMC_BWTR1_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FSMC_BWTR1_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FSMC_BWTR1_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FSMC_BWTR1_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_BWTR1_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_BWTR1_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_BWTR1_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FSMC_BWTR1_CLKDIV                   ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FSMC_BWTR1_CLKDIV_0                 ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FSMC_BWTR1_CLKDIV_1                 ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FSMC_BWTR1_CLKDIV_2                 ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FSMC_BWTR1_CLKDIV_3                 ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FSMC_BWTR1_DATLAT                   ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FSMC_BWTR1_DATLAT_0                 ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_BWTR1_DATLAT_1                 ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_BWTR1_DATLAT_2                 ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_BWTR1_DATLAT_3                 ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FSMC_BWTR1_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FSMC_BWTR1_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FSMC_BWTR1_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FSMC_BWTR2 register  ******************/
#define  FSMC_BWTR2_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FSMC_BWTR2_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_BWTR2_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_BWTR2_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_BWTR2_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FSMC_BWTR2_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FSMC_BWTR2_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BWTR2_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FSMC_BWTR2_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FSMC_BWTR2_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FSMC_BWTR2_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FSMC_BWTR2_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_BWTR2_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_BWTR2_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_BWTR2_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FSMC_BWTR2_CLKDIV                   ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FSMC_BWTR2_CLKDIV_0                 ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FSMC_BWTR2_CLKDIV_1                 ((uint32_t)0x00200000)        /*!<Bit 1*/
#define  FSMC_BWTR2_CLKDIV_2                 ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FSMC_BWTR2_CLKDIV_3                 ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FSMC_BWTR2_DATLAT                   ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FSMC_BWTR2_DATLAT_0                 ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_BWTR2_DATLAT_1                 ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_BWTR2_DATLAT_2                 ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_BWTR2_DATLAT_3                 ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FSMC_BWTR2_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FSMC_BWTR2_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FSMC_BWTR2_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FSMC_BWTR3 register  ******************/
#define  FSMC_BWTR3_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FSMC_BWTR3_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_BWTR3_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_BWTR3_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_BWTR3_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FSMC_BWTR3_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FSMC_BWTR3_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BWTR3_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FSMC_BWTR3_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FSMC_BWTR3_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FSMC_BWTR3_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FSMC_BWTR3_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_BWTR3_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_BWTR3_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_BWTR3_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FSMC_BWTR3_CLKDIV                   ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FSMC_BWTR3_CLKDIV_0                 ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FSMC_BWTR3_CLKDIV_1                 ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FSMC_BWTR3_CLKDIV_2                 ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FSMC_BWTR3_CLKDIV_3                 ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FSMC_BWTR3_DATLAT                   ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FSMC_BWTR3_DATLAT_0                 ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_BWTR3_DATLAT_1                 ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_BWTR3_DATLAT_2                 ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_BWTR3_DATLAT_3                 ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FSMC_BWTR3_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FSMC_BWTR3_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FSMC_BWTR3_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FSMC_BWTR4 register  ******************/
#define  FSMC_BWTR4_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FSMC_BWTR4_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_BWTR4_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_BWTR4_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_BWTR4_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FSMC_BWTR4_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FSMC_BWTR4_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_BWTR4_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FSMC_BWTR4_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FSMC_BWTR4_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FSMC_BWTR4_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FSMC_BWTR4_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_BWTR4_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_BWTR4_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_BWTR4_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FSMC_BWTR4_CLKDIV                   ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FSMC_BWTR4_CLKDIV_0                 ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FSMC_BWTR4_CLKDIV_1                 ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FSMC_BWTR4_CLKDIV_2                 ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FSMC_BWTR4_CLKDIV_3                 ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FSMC_BWTR4_DATLAT                   ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FSMC_BWTR4_DATLAT_0                 ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_BWTR4_DATLAT_1                 ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_BWTR4_DATLAT_2                 ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_BWTR4_DATLAT_3                 ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FSMC_BWTR4_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FSMC_BWTR4_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FSMC_BWTR4_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FSMC_PCR2 register  *******************/
#define  FSMC_PCR2_PWAITEN                   ((uint32_t)0x00000002)        /*!<Wait feature enable bit */
#define  FSMC_PCR2_PBKEN                     ((uint32_t)0x00000004)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FSMC_PCR2_PTYP                      ((uint32_t)0x00000008)        /*!<Memory type */

#define  FSMC_PCR2_PWID                      ((uint32_t)0x00000030)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FSMC_PCR2_PWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_PCR2_PWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FSMC_PCR2_ECCEN                     ((uint32_t)0x00000040)        /*!<ECC computation logic enable bit */

#define  FSMC_PCR2_TCLR                      ((uint32_t)0x00001E00)        /*!<TCLR[3:0] bits (CLE to RE delay) */
#define  FSMC_PCR2_TCLR_0                    ((uint32_t)0x00000200)        /*!<Bit 0 */
#define  FSMC_PCR2_TCLR_1                    ((uint32_t)0x00000400)        /*!<Bit 1 */
#define  FSMC_PCR2_TCLR_2                    ((uint32_t)0x00000800)        /*!<Bit 2 */
#define  FSMC_PCR2_TCLR_3                    ((uint32_t)0x00001000)        /*!<Bit 3 */

#define  FSMC_PCR2_TAR                       ((uint32_t)0x0001E000)        /*!<TAR[3:0] bits (ALE to RE delay) */
#define  FSMC_PCR2_TAR_0                     ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FSMC_PCR2_TAR_1                     ((uint32_t)0x00004000)        /*!<Bit 1 */
#define  FSMC_PCR2_TAR_2                     ((uint32_t)0x00008000)        /*!<Bit 2 */
#define  FSMC_PCR2_TAR_3                     ((uint32_t)0x00010000)        /*!<Bit 3 */

#define  FSMC_PCR2_ECCPS                     ((uint32_t)0x000E0000)        /*!<ECCPS[1:0] bits (ECC page size) */
#define  FSMC_PCR2_ECCPS_0                   ((uint32_t)0x00020000)        /*!<Bit 0 */
#define  FSMC_PCR2_ECCPS_1                   ((uint32_t)0x00040000)        /*!<Bit 1 */
#define  FSMC_PCR2_ECCPS_2                   ((uint32_t)0x00080000)        /*!<Bit 2 */

/******************  Bit definition for FSMC_PCR3 register  *******************/
#define  FSMC_PCR3_PWAITEN                   ((uint32_t)0x00000002)        /*!<Wait feature enable bit */
#define  FSMC_PCR3_PBKEN                     ((uint32_t)0x00000004)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FSMC_PCR3_PTYP                      ((uint32_t)0x00000008)        /*!<Memory type */

#define  FSMC_PCR3_PWID                      ((uint32_t)0x00000030)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FSMC_PCR3_PWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_PCR3_PWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FSMC_PCR3_ECCEN                     ((uint32_t)0x00000040)        /*!<ECC computation logic enable bit */

#define  FSMC_PCR3_TCLR                      ((uint32_t)0x00001E00)        /*!<TCLR[3:0] bits (CLE to RE delay) */
#define  FSMC_PCR3_TCLR_0                    ((uint32_t)0x00000200)        /*!<Bit 0 */
#define  FSMC_PCR3_TCLR_1                    ((uint32_t)0x00000400)        /*!<Bit 1 */
#define  FSMC_PCR3_TCLR_2                    ((uint32_t)0x00000800)        /*!<Bit 2 */
#define  FSMC_PCR3_TCLR_3                    ((uint32_t)0x00001000)        /*!<Bit 3 */

#define  FSMC_PCR3_TAR                       ((uint32_t)0x0001E000)        /*!<TAR[3:0] bits (ALE to RE delay) */
#define  FSMC_PCR3_TAR_0                     ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FSMC_PCR3_TAR_1                     ((uint32_t)0x00004000)        /*!<Bit 1 */
#define  FSMC_PCR3_TAR_2                     ((uint32_t)0x00008000)        /*!<Bit 2 */
#define  FSMC_PCR3_TAR_3                     ((uint32_t)0x00010000)        /*!<Bit 3 */

#define  FSMC_PCR3_ECCPS                     ((uint32_t)0x000E0000)        /*!<ECCPS[2:0] bits (ECC page size) */
#define  FSMC_PCR3_ECCPS_0                   ((uint32_t)0x00020000)        /*!<Bit 0 */
#define  FSMC_PCR3_ECCPS_1                   ((uint32_t)0x00040000)        /*!<Bit 1 */
#define  FSMC_PCR3_ECCPS_2                   ((uint32_t)0x00080000)        /*!<Bit 2 */

/******************  Bit definition for FSMC_PCR4 register  *******************/
#define  FSMC_PCR4_PWAITEN                   ((uint32_t)0x00000002)        /*!<Wait feature enable bit */
#define  FSMC_PCR4_PBKEN                     ((uint32_t)0x00000004)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FSMC_PCR4_PTYP                      ((uint32_t)0x00000008)        /*!<Memory type */

#define  FSMC_PCR4_PWID                      ((uint32_t)0x00000030)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FSMC_PCR4_PWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FSMC_PCR4_PWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FSMC_PCR4_ECCEN                     ((uint32_t)0x00000040)        /*!<ECC computation logic enable bit */

#define  FSMC_PCR4_TCLR                      ((uint32_t)0x00001E00)        /*!<TCLR[3:0] bits (CLE to RE delay) */
#define  FSMC_PCR4_TCLR_0                    ((uint32_t)0x00000200)        /*!<Bit 0 */
#define  FSMC_PCR4_TCLR_1                    ((uint32_t)0x00000400)        /*!<Bit 1 */
#define  FSMC_PCR4_TCLR_2                    ((uint32_t)0x00000800)        /*!<Bit 2 */
#define  FSMC_PCR4_TCLR_3                    ((uint32_t)0x00001000)        /*!<Bit 3 */

#define  FSMC_PCR4_TAR                       ((uint32_t)0x0001E000)        /*!<TAR[3:0] bits (ALE to RE delay) */
#define  FSMC_PCR4_TAR_0                     ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FSMC_PCR4_TAR_1                     ((uint32_t)0x00004000)        /*!<Bit 1 */
#define  FSMC_PCR4_TAR_2                     ((uint32_t)0x00008000)        /*!<Bit 2 */
#define  FSMC_PCR4_TAR_3                     ((uint32_t)0x00010000)        /*!<Bit 3 */

#define  FSMC_PCR4_ECCPS                     ((uint32_t)0x000E0000)        /*!<ECCPS[2:0] bits (ECC page size) */
#define  FSMC_PCR4_ECCPS_0                   ((uint32_t)0x00020000)        /*!<Bit 0 */
#define  FSMC_PCR4_ECCPS_1                   ((uint32_t)0x00040000)        /*!<Bit 1 */
#define  FSMC_PCR4_ECCPS_2                   ((uint32_t)0x00080000)        /*!<Bit 2 */

/*******************  Bit definition for FSMC_SR2 register  *******************/
#define  FSMC_SR2_IRS                        ((uint8_t)0x01)               /*!<Interrupt Rising Edge status                */
#define  FSMC_SR2_ILS                        ((uint8_t)0x02)               /*!<Interrupt Level status                      */
#define  FSMC_SR2_IFS                        ((uint8_t)0x04)               /*!<Interrupt Falling Edge status               */
#define  FSMC_SR2_IREN                       ((uint8_t)0x08)               /*!<Interrupt Rising Edge detection Enable bit  */
#define  FSMC_SR2_ILEN                       ((uint8_t)0x10)               /*!<Interrupt Level detection Enable bit        */
#define  FSMC_SR2_IFEN                       ((uint8_t)0x20)               /*!<Interrupt Falling Edge detection Enable bit */
#define  FSMC_SR2_FEMPT                      ((uint8_t)0x40)               /*!<FIFO empty */

/*******************  Bit definition for FSMC_SR3 register  *******************/
#define  FSMC_SR3_IRS                        ((uint8_t)0x01)               /*!<Interrupt Rising Edge status                */
#define  FSMC_SR3_ILS                        ((uint8_t)0x02)               /*!<Interrupt Level status                      */
#define  FSMC_SR3_IFS                        ((uint8_t)0x04)               /*!<Interrupt Falling Edge status               */
#define  FSMC_SR3_IREN                       ((uint8_t)0x08)               /*!<Interrupt Rising Edge detection Enable bit  */
#define  FSMC_SR3_ILEN                       ((uint8_t)0x10)               /*!<Interrupt Level detection Enable bit        */
#define  FSMC_SR3_IFEN                       ((uint8_t)0x20)               /*!<Interrupt Falling Edge detection Enable bit */
#define  FSMC_SR3_FEMPT                      ((uint8_t)0x40)               /*!<FIFO empty */

/*******************  Bit definition for FSMC_SR4 register  *******************/
#define  FSMC_SR4_IRS                        ((uint8_t)0x01)               /*!<Interrupt Rising Edge status                 */
#define  FSMC_SR4_ILS                        ((uint8_t)0x02)               /*!<Interrupt Level status                       */
#define  FSMC_SR4_IFS                        ((uint8_t)0x04)               /*!<Interrupt Falling Edge status                */
#define  FSMC_SR4_IREN                       ((uint8_t)0x08)               /*!<Interrupt Rising Edge detection Enable bit   */
#define  FSMC_SR4_ILEN                       ((uint8_t)0x10)               /*!<Interrupt Level detection Enable bit         */
#define  FSMC_SR4_IFEN                       ((uint8_t)0x20)               /*!<Interrupt Falling Edge detection Enable bit  */
#define  FSMC_SR4_FEMPT                      ((uint8_t)0x40)               /*!<FIFO empty */

/******************  Bit definition for FSMC_PMEM2 register  ******************/
#define  FSMC_PMEM2_MEMSET2                  ((uint32_t)0x000000FF)        /*!<MEMSET2[7:0] bits (Common memory 2 setup time) */
#define  FSMC_PMEM2_MEMSET2_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_PMEM2_MEMSET2_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_PMEM2_MEMSET2_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_PMEM2_MEMSET2_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FSMC_PMEM2_MEMSET2_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FSMC_PMEM2_MEMSET2_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FSMC_PMEM2_MEMSET2_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FSMC_PMEM2_MEMSET2_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FSMC_PMEM2_MEMWAIT2                 ((uint32_t)0x0000FF00)        /*!<MEMWAIT2[7:0] bits (Common memory 2 wait time) */
#define  FSMC_PMEM2_MEMWAIT2_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_PMEM2_MEMWAIT2_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_PMEM2_MEMWAIT2_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_PMEM2_MEMWAIT2_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FSMC_PMEM2_MEMWAIT2_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FSMC_PMEM2_MEMWAIT2_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FSMC_PMEM2_MEMWAIT2_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FSMC_PMEM2_MEMWAIT2_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FSMC_PMEM2_MEMHOLD2                 ((uint32_t)0x00FF0000)        /*!<MEMHOLD2[7:0] bits (Common memory 2 hold time) */
#define  FSMC_PMEM2_MEMHOLD2_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_PMEM2_MEMHOLD2_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_PMEM2_MEMHOLD2_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_PMEM2_MEMHOLD2_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FSMC_PMEM2_MEMHOLD2_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FSMC_PMEM2_MEMHOLD2_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FSMC_PMEM2_MEMHOLD2_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FSMC_PMEM2_MEMHOLD2_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FSMC_PMEM2_MEMHIZ2                  ((uint32_t)0xFF000000)        /*!<MEMHIZ2[7:0] bits (Common memory 2 databus HiZ time) */
#define  FSMC_PMEM2_MEMHIZ2_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_PMEM2_MEMHIZ2_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_PMEM2_MEMHIZ2_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_PMEM2_MEMHIZ2_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FSMC_PMEM2_MEMHIZ2_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FSMC_PMEM2_MEMHIZ2_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FSMC_PMEM2_MEMHIZ2_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FSMC_PMEM2_MEMHIZ2_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FSMC_PMEM3 register  ******************/
#define  FSMC_PMEM3_MEMSET3                  ((uint32_t)0x000000FF)        /*!<MEMSET3[7:0] bits (Common memory 3 setup time) */
#define  FSMC_PMEM3_MEMSET3_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_PMEM3_MEMSET3_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_PMEM3_MEMSET3_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_PMEM3_MEMSET3_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FSMC_PMEM3_MEMSET3_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FSMC_PMEM3_MEMSET3_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FSMC_PMEM3_MEMSET3_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FSMC_PMEM3_MEMSET3_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FSMC_PMEM3_MEMWAIT3                 ((uint32_t)0x0000FF00)        /*!<MEMWAIT3[7:0] bits (Common memory 3 wait time) */
#define  FSMC_PMEM3_MEMWAIT3_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_PMEM3_MEMWAIT3_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_PMEM3_MEMWAIT3_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_PMEM3_MEMWAIT3_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FSMC_PMEM3_MEMWAIT3_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FSMC_PMEM3_MEMWAIT3_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FSMC_PMEM3_MEMWAIT3_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FSMC_PMEM3_MEMWAIT3_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FSMC_PMEM3_MEMHOLD3                 ((uint32_t)0x00FF0000)        /*!<MEMHOLD3[7:0] bits (Common memory 3 hold time) */
#define  FSMC_PMEM3_MEMHOLD3_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_PMEM3_MEMHOLD3_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_PMEM3_MEMHOLD3_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_PMEM3_MEMHOLD3_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FSMC_PMEM3_MEMHOLD3_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FSMC_PMEM3_MEMHOLD3_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FSMC_PMEM3_MEMHOLD3_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FSMC_PMEM3_MEMHOLD3_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FSMC_PMEM3_MEMHIZ3                  ((uint32_t)0xFF000000)        /*!<MEMHIZ3[7:0] bits (Common memory 3 databus HiZ time) */
#define  FSMC_PMEM3_MEMHIZ3_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_PMEM3_MEMHIZ3_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_PMEM3_MEMHIZ3_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_PMEM3_MEMHIZ3_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FSMC_PMEM3_MEMHIZ3_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FSMC_PMEM3_MEMHIZ3_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FSMC_PMEM3_MEMHIZ3_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FSMC_PMEM3_MEMHIZ3_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FSMC_PMEM4 register  ******************/
#define  FSMC_PMEM4_MEMSET4                  ((uint32_t)0x000000FF)        /*!<MEMSET4[7:0] bits (Common memory 4 setup time) */
#define  FSMC_PMEM4_MEMSET4_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_PMEM4_MEMSET4_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_PMEM4_MEMSET4_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_PMEM4_MEMSET4_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FSMC_PMEM4_MEMSET4_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FSMC_PMEM4_MEMSET4_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FSMC_PMEM4_MEMSET4_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FSMC_PMEM4_MEMSET4_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FSMC_PMEM4_MEMWAIT4                 ((uint32_t)0x0000FF00)        /*!<MEMWAIT4[7:0] bits (Common memory 4 wait time) */
#define  FSMC_PMEM4_MEMWAIT4_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_PMEM4_MEMWAIT4_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_PMEM4_MEMWAIT4_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_PMEM4_MEMWAIT4_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FSMC_PMEM4_MEMWAIT4_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FSMC_PMEM4_MEMWAIT4_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FSMC_PMEM4_MEMWAIT4_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FSMC_PMEM4_MEMWAIT4_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FSMC_PMEM4_MEMHOLD4                 ((uint32_t)0x00FF0000)        /*!<MEMHOLD4[7:0] bits (Common memory 4 hold time) */
#define  FSMC_PMEM4_MEMHOLD4_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_PMEM4_MEMHOLD4_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_PMEM4_MEMHOLD4_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_PMEM4_MEMHOLD4_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FSMC_PMEM4_MEMHOLD4_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FSMC_PMEM4_MEMHOLD4_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FSMC_PMEM4_MEMHOLD4_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FSMC_PMEM4_MEMHOLD4_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FSMC_PMEM4_MEMHIZ4                  ((uint32_t)0xFF000000)        /*!<MEMHIZ4[7:0] bits (Common memory 4 databus HiZ time) */
#define  FSMC_PMEM4_MEMHIZ4_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_PMEM4_MEMHIZ4_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_PMEM4_MEMHIZ4_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_PMEM4_MEMHIZ4_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FSMC_PMEM4_MEMHIZ4_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FSMC_PMEM4_MEMHIZ4_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FSMC_PMEM4_MEMHIZ4_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FSMC_PMEM4_MEMHIZ4_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FSMC_PATT2 register  ******************/
#define  FSMC_PATT2_ATTSET2                  ((uint32_t)0x000000FF)        /*!<ATTSET2[7:0] bits (Attribute memory 2 setup time) */
#define  FSMC_PATT2_ATTSET2_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_PATT2_ATTSET2_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_PATT2_ATTSET2_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_PATT2_ATTSET2_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FSMC_PATT2_ATTSET2_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FSMC_PATT2_ATTSET2_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FSMC_PATT2_ATTSET2_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FSMC_PATT2_ATTSET2_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FSMC_PATT2_ATTWAIT2                 ((uint32_t)0x0000FF00)        /*!<ATTWAIT2[7:0] bits (Attribute memory 2 wait time) */
#define  FSMC_PATT2_ATTWAIT2_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_PATT2_ATTWAIT2_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_PATT2_ATTWAIT2_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_PATT2_ATTWAIT2_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FSMC_PATT2_ATTWAIT2_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FSMC_PATT2_ATTWAIT2_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FSMC_PATT2_ATTWAIT2_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FSMC_PATT2_ATTWAIT2_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FSMC_PATT2_ATTHOLD2                 ((uint32_t)0x00FF0000)        /*!<ATTHOLD2[7:0] bits (Attribute memory 2 hold time) */
#define  FSMC_PATT2_ATTHOLD2_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_PATT2_ATTHOLD2_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_PATT2_ATTHOLD2_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_PATT2_ATTHOLD2_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FSMC_PATT2_ATTHOLD2_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FSMC_PATT2_ATTHOLD2_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FSMC_PATT2_ATTHOLD2_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FSMC_PATT2_ATTHOLD2_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FSMC_PATT2_ATTHIZ2                  ((uint32_t)0xFF000000)        /*!<ATTHIZ2[7:0] bits (Attribute memory 2 databus HiZ time) */
#define  FSMC_PATT2_ATTHIZ2_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_PATT2_ATTHIZ2_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_PATT2_ATTHIZ2_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_PATT2_ATTHIZ2_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FSMC_PATT2_ATTHIZ2_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FSMC_PATT2_ATTHIZ2_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FSMC_PATT2_ATTHIZ2_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FSMC_PATT2_ATTHIZ2_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FSMC_PATT3 register  ******************/
#define  FSMC_PATT3_ATTSET3                  ((uint32_t)0x000000FF)        /*!<ATTSET3[7:0] bits (Attribute memory 3 setup time) */
#define  FSMC_PATT3_ATTSET3_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_PATT3_ATTSET3_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_PATT3_ATTSET3_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_PATT3_ATTSET3_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FSMC_PATT3_ATTSET3_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FSMC_PATT3_ATTSET3_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FSMC_PATT3_ATTSET3_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FSMC_PATT3_ATTSET3_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FSMC_PATT3_ATTWAIT3                 ((uint32_t)0x0000FF00)        /*!<ATTWAIT3[7:0] bits (Attribute memory 3 wait time) */
#define  FSMC_PATT3_ATTWAIT3_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_PATT3_ATTWAIT3_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_PATT3_ATTWAIT3_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_PATT3_ATTWAIT3_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FSMC_PATT3_ATTWAIT3_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FSMC_PATT3_ATTWAIT3_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FSMC_PATT3_ATTWAIT3_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FSMC_PATT3_ATTWAIT3_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FSMC_PATT3_ATTHOLD3                 ((uint32_t)0x00FF0000)        /*!<ATTHOLD3[7:0] bits (Attribute memory 3 hold time) */
#define  FSMC_PATT3_ATTHOLD3_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_PATT3_ATTHOLD3_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_PATT3_ATTHOLD3_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_PATT3_ATTHOLD3_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FSMC_PATT3_ATTHOLD3_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FSMC_PATT3_ATTHOLD3_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FSMC_PATT3_ATTHOLD3_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FSMC_PATT3_ATTHOLD3_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FSMC_PATT3_ATTHIZ3                  ((uint32_t)0xFF000000)        /*!<ATTHIZ3[7:0] bits (Attribute memory 3 databus HiZ time) */
#define  FSMC_PATT3_ATTHIZ3_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_PATT3_ATTHIZ3_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_PATT3_ATTHIZ3_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_PATT3_ATTHIZ3_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FSMC_PATT3_ATTHIZ3_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FSMC_PATT3_ATTHIZ3_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FSMC_PATT3_ATTHIZ3_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FSMC_PATT3_ATTHIZ3_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FSMC_PATT4 register  ******************/
#define  FSMC_PATT4_ATTSET4                  ((uint32_t)0x000000FF)        /*!<ATTSET4[7:0] bits (Attribute memory 4 setup time) */
#define  FSMC_PATT4_ATTSET4_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_PATT4_ATTSET4_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_PATT4_ATTSET4_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_PATT4_ATTSET4_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FSMC_PATT4_ATTSET4_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FSMC_PATT4_ATTSET4_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FSMC_PATT4_ATTSET4_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FSMC_PATT4_ATTSET4_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FSMC_PATT4_ATTWAIT4                 ((uint32_t)0x0000FF00)        /*!<ATTWAIT4[7:0] bits (Attribute memory 4 wait time) */
#define  FSMC_PATT4_ATTWAIT4_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_PATT4_ATTWAIT4_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_PATT4_ATTWAIT4_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_PATT4_ATTWAIT4_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FSMC_PATT4_ATTWAIT4_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FSMC_PATT4_ATTWAIT4_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FSMC_PATT4_ATTWAIT4_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FSMC_PATT4_ATTWAIT4_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FSMC_PATT4_ATTHOLD4                 ((uint32_t)0x00FF0000)        /*!<ATTHOLD4[7:0] bits (Attribute memory 4 hold time) */
#define  FSMC_PATT4_ATTHOLD4_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_PATT4_ATTHOLD4_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_PATT4_ATTHOLD4_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_PATT4_ATTHOLD4_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FSMC_PATT4_ATTHOLD4_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FSMC_PATT4_ATTHOLD4_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FSMC_PATT4_ATTHOLD4_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FSMC_PATT4_ATTHOLD4_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FSMC_PATT4_ATTHIZ4                  ((uint32_t)0xFF000000)        /*!<ATTHIZ4[7:0] bits (Attribute memory 4 databus HiZ time) */
#define  FSMC_PATT4_ATTHIZ4_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_PATT4_ATTHIZ4_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_PATT4_ATTHIZ4_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_PATT4_ATTHIZ4_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FSMC_PATT4_ATTHIZ4_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FSMC_PATT4_ATTHIZ4_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FSMC_PATT4_ATTHIZ4_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FSMC_PATT4_ATTHIZ4_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FSMC_PIO4 register  *******************/
#define  FSMC_PIO4_IOSET4                    ((uint32_t)0x000000FF)        /*!<IOSET4[7:0] bits (I/O 4 setup time) */
#define  FSMC_PIO4_IOSET4_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FSMC_PIO4_IOSET4_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FSMC_PIO4_IOSET4_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FSMC_PIO4_IOSET4_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FSMC_PIO4_IOSET4_4                  ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FSMC_PIO4_IOSET4_5                  ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FSMC_PIO4_IOSET4_6                  ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FSMC_PIO4_IOSET4_7                  ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FSMC_PIO4_IOWAIT4                   ((uint32_t)0x0000FF00)        /*!<IOWAIT4[7:0] bits (I/O 4 wait time) */
#define  FSMC_PIO4_IOWAIT4_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FSMC_PIO4_IOWAIT4_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FSMC_PIO4_IOWAIT4_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FSMC_PIO4_IOWAIT4_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FSMC_PIO4_IOWAIT4_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FSMC_PIO4_IOWAIT4_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FSMC_PIO4_IOWAIT4_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FSMC_PIO4_IOWAIT4_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FSMC_PIO4_IOHOLD4                   ((uint32_t)0x00FF0000)        /*!<IOHOLD4[7:0] bits (I/O 4 hold time) */
#define  FSMC_PIO4_IOHOLD4_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FSMC_PIO4_IOHOLD4_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FSMC_PIO4_IOHOLD4_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FSMC_PIO4_IOHOLD4_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FSMC_PIO4_IOHOLD4_4                 ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FSMC_PIO4_IOHOLD4_5                 ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FSMC_PIO4_IOHOLD4_6                 ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FSMC_PIO4_IOHOLD4_7                 ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FSMC_PIO4_IOHIZ4                    ((uint32_t)0xFF000000)        /*!<IOHIZ4[7:0] bits (I/O 4 databus HiZ time) */
#define  FSMC_PIO4_IOHIZ4_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FSMC_PIO4_IOHIZ4_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FSMC_PIO4_IOHIZ4_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FSMC_PIO4_IOHIZ4_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FSMC_PIO4_IOHIZ4_4                  ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FSMC_PIO4_IOHIZ4_5                  ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FSMC_PIO4_IOHIZ4_6                  ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FSMC_PIO4_IOHIZ4_7                  ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FSMC_ECCR2 register  ******************/
#define  FSMC_ECCR2_ECC2                     ((uint32_t)0xFFFFFFFF)        /*!<ECC result */

/******************  Bit definition for FSMC_ECCR3 register  ******************/
#define  FSMC_ECCR3_ECC3                     ((uint32_t)0xFFFFFFFF)        /*!<ECC result */
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
/******************************************************************************/
/*                                                                            */
/*                          Flexible Memory Controller                        */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for FMC_BCR1 register  *******************/
#define  FMC_BCR1_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FMC_BCR1_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR1_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR1_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_BCR1_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_BCR1_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR1_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BCR1_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_BCR1_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable        */
#define  FMC_BCR1_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit           */
#define  FMC_BCR1_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit   */
#define  FMC_BCR1_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support */
#define  FMC_BCR1_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration  */
#define  FMC_BCR1_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit           */
#define  FMC_BCR1_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit            */
#define  FMC_BCR1_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable       */
#define  FMC_BCR1_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait          */
#define  FMC_BCR1_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable         */
#define  FMC_BCR1_CCLKEN                    ((uint32_t)0x00100000)        /*!<Continous clock enable     */

/******************  Bit definition for FMC_BCR2 register  *******************/
#define  FMC_BCR2_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FMC_BCR2_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR2_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR2_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_BCR2_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_BCR2_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR2_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BCR2_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_BCR2_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable        */
#define  FMC_BCR2_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit           */
#define  FMC_BCR2_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit   */
#define  FMC_BCR2_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support */
#define  FMC_BCR2_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration  */
#define  FMC_BCR2_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit           */
#define  FMC_BCR2_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit            */
#define  FMC_BCR2_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable       */
#define  FMC_BCR2_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait          */
#define  FMC_BCR2_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR3 register  *******************/
#define  FMC_BCR3_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FMC_BCR3_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR3_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR3_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_BCR3_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_BCR3_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR3_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BCR3_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_BCR3_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable        */
#define  FMC_BCR3_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit           */
#define  FMC_BCR3_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit   */
#define  FMC_BCR3_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support */
#define  FMC_BCR3_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration  */
#define  FMC_BCR3_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit           */
#define  FMC_BCR3_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit            */
#define  FMC_BCR3_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable       */
#define  FMC_BCR3_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait          */
#define  FMC_BCR3_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR4 register  *******************/
#define  FMC_BCR4_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FMC_BCR4_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR4_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR4_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_BCR4_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_BCR4_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR4_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BCR4_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_BCR4_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable        */
#define  FMC_BCR4_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit           */
#define  FMC_BCR4_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit   */
#define  FMC_BCR4_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support */
#define  FMC_BCR4_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration  */
#define  FMC_BCR4_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit           */
#define  FMC_BCR4_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit            */
#define  FMC_BCR4_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable       */
#define  FMC_BCR4_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait          */
#define  FMC_BCR4_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable         */

/******************  Bit definition for FMC_BTR1 register  ******************/
#define  FMC_BTR1_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR1_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BTR1_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BTR1_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BTR1_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BTR1_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration)  */
#define  FMC_BTR1_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BTR1_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BTR1_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BTR1_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BTR1_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR1_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BTR1_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BTR1_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BTR1_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BTR1_DATAST_4                  ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BTR1_DATAST_5                  ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BTR1_DATAST_6                  ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BTR1_DATAST_7                  ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BTR1_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR1_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BTR1_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BTR1_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BTR1_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BTR1_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR1_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BTR1_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BTR1_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BTR1_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BTR1_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR1_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BTR1_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BTR1_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BTR1_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BTR1_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR1_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BTR1_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BTR2 register  *******************/
#define  FMC_BTR2_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR2_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BTR2_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BTR2_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BTR2_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BTR2_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR2_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BTR2_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BTR2_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BTR2_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BTR2_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR2_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BTR2_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BTR2_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BTR2_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BTR2_DATAST_4                  ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BTR2_DATAST_5                  ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BTR2_DATAST_6                  ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BTR2_DATAST_7                  ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BTR2_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR2_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BTR2_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BTR2_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BTR2_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BTR2_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR2_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BTR2_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BTR2_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BTR2_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BTR2_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR2_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BTR2_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BTR2_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BTR2_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BTR2_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR2_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BTR2_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/*******************  Bit definition for FMC_BTR3 register  *******************/
#define  FMC_BTR3_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR3_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BTR3_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BTR3_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BTR3_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BTR3_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR3_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BTR3_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BTR3_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BTR3_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BTR3_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR3_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BTR3_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BTR3_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BTR3_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BTR3_DATAST_4                  ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BTR3_DATAST_5                  ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BTR3_DATAST_6                  ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BTR3_DATAST_7                  ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BTR3_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR3_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BTR3_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BTR3_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BTR3_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BTR3_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR3_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BTR3_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BTR3_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BTR3_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BTR3_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR3_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BTR3_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BTR3_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BTR3_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BTR3_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR3_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BTR3_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BTR4 register  *******************/
#define  FMC_BTR4_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR4_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BTR4_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BTR4_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BTR4_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BTR4_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR4_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BTR4_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BTR4_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BTR4_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BTR4_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR4_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BTR4_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BTR4_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BTR4_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BTR4_DATAST_4                  ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BTR4_DATAST_5                  ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BTR4_DATAST_6                  ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BTR4_DATAST_7                  ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BTR4_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR4_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BTR4_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BTR4_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BTR4_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BTR4_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR4_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BTR4_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BTR4_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BTR4_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BTR4_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR4_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BTR4_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BTR4_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BTR4_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BTR4_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR4_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BTR4_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR1 register  ******************/
#define  FMC_BWTR1_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR1_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BWTR1_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BWTR1_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BWTR1_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BWTR1_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR1_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BWTR1_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BWTR1_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BWTR1_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BWTR1_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR1_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BWTR1_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BWTR1_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BWTR1_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BWTR1_DATAST_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BWTR1_DATAST_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BWTR1_DATAST_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BWTR1_DATAST_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BWTR1_CLKDIV                   ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BWTR1_CLKDIV_0                 ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BWTR1_CLKDIV_1                 ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BWTR1_CLKDIV_2                 ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BWTR1_CLKDIV_3                 ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BWTR1_DATLAT                   ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BWTR1_DATLAT_0                 ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BWTR1_DATLAT_1                 ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BWTR1_DATLAT_2                 ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BWTR1_DATLAT_3                 ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BWTR1_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR1_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BWTR1_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR2 register  ******************/
#define  FMC_BWTR2_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR2_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BWTR2_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BWTR2_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BWTR2_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BWTR2_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR2_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BWTR2_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BWTR2_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BWTR2_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BWTR2_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR2_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BWTR2_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BWTR2_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BWTR2_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BWTR2_DATAST_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BWTR2_DATAST_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BWTR2_DATAST_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BWTR2_DATAST_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BWTR2_CLKDIV                   ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BWTR2_CLKDIV_0                 ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BWTR2_CLKDIV_1                 ((uint32_t)0x00200000)        /*!<Bit 1*/
#define  FMC_BWTR2_CLKDIV_2                 ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BWTR2_CLKDIV_3                 ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BWTR2_DATLAT                   ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BWTR2_DATLAT_0                 ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BWTR2_DATLAT_1                 ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BWTR2_DATLAT_2                 ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BWTR2_DATLAT_3                 ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BWTR2_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR2_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BWTR2_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR3 register  ******************/
#define  FMC_BWTR3_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR3_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BWTR3_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BWTR3_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BWTR3_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BWTR3_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR3_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BWTR3_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BWTR3_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BWTR3_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BWTR3_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR3_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BWTR3_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BWTR3_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BWTR3_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BWTR3_DATAST_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BWTR3_DATAST_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BWTR3_DATAST_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BWTR3_DATAST_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BWTR3_CLKDIV                   ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BWTR3_CLKDIV_0                 ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BWTR3_CLKDIV_1                 ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BWTR3_CLKDIV_2                 ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BWTR3_CLKDIV_3                 ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BWTR3_DATLAT                   ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BWTR3_DATLAT_0                 ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BWTR3_DATLAT_1                 ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BWTR3_DATLAT_2                 ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BWTR3_DATLAT_3                 ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BWTR3_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR3_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BWTR3_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR4 register  ******************/
#define  FMC_BWTR4_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR4_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BWTR4_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BWTR4_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BWTR4_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BWTR4_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR4_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BWTR4_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BWTR4_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BWTR4_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BWTR4_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR4_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BWTR4_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BWTR4_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BWTR4_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BWTR4_DATAST_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BWTR4_DATAST_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BWTR4_DATAST_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BWTR4_DATAST_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BWTR4_CLKDIV                   ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BWTR4_CLKDIV_0                 ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BWTR4_CLKDIV_1                 ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BWTR4_CLKDIV_2                 ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BWTR4_CLKDIV_3                 ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BWTR4_DATLAT                   ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BWTR4_DATLAT_0                 ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BWTR4_DATLAT_1                 ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BWTR4_DATLAT_2                 ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BWTR4_DATLAT_3                 ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BWTR4_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR4_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BWTR4_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_PCR2 register  *******************/
#define  FMC_PCR2_PWAITEN                   ((uint32_t)0x00000002)        /*!<Wait feature enable bit                   */
#define  FMC_PCR2_PBKEN                     ((uint32_t)0x00000004)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FMC_PCR2_PTYP                      ((uint32_t)0x00000008)        /*!<Memory type                               */

#define  FMC_PCR2_PWID                      ((uint32_t)0x00000030)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FMC_PCR2_PWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_PCR2_PWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_PCR2_ECCEN                     ((uint32_t)0x00000040)        /*!<ECC computation logic enable bit          */

#define  FMC_PCR2_TCLR                      ((uint32_t)0x00001E00)        /*!<TCLR[3:0] bits (CLE to RE delay)          */
#define  FMC_PCR2_TCLR_0                    ((uint32_t)0x00000200)        /*!<Bit 0 */
#define  FMC_PCR2_TCLR_1                    ((uint32_t)0x00000400)        /*!<Bit 1 */
#define  FMC_PCR2_TCLR_2                    ((uint32_t)0x00000800)        /*!<Bit 2 */
#define  FMC_PCR2_TCLR_3                    ((uint32_t)0x00001000)        /*!<Bit 3 */

#define  FMC_PCR2_TAR                       ((uint32_t)0x0001E000)        /*!<TAR[3:0] bits (ALE to RE delay)           */
#define  FMC_PCR2_TAR_0                     ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FMC_PCR2_TAR_1                     ((uint32_t)0x00004000)        /*!<Bit 1 */
#define  FMC_PCR2_TAR_2                     ((uint32_t)0x00008000)        /*!<Bit 2 */
#define  FMC_PCR2_TAR_3                     ((uint32_t)0x00010000)        /*!<Bit 3 */

#define  FMC_PCR2_ECCPS                     ((uint32_t)0x000E0000)        /*!<ECCPS[1:0] bits (ECC page size)           */
#define  FMC_PCR2_ECCPS_0                   ((uint32_t)0x00020000)        /*!<Bit 0 */
#define  FMC_PCR2_ECCPS_1                   ((uint32_t)0x00040000)        /*!<Bit 1 */
#define  FMC_PCR2_ECCPS_2                   ((uint32_t)0x00080000)        /*!<Bit 2 */

/******************  Bit definition for FMC_PCR3 register  *******************/
#define  FMC_PCR3_PWAITEN                   ((uint32_t)0x00000002)        /*!<Wait feature enable bit                   */
#define  FMC_PCR3_PBKEN                     ((uint32_t)0x00000004)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FMC_PCR3_PTYP                      ((uint32_t)0x00000008)        /*!<Memory type                               */

#define  FMC_PCR3_PWID                      ((uint32_t)0x00000030)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FMC_PCR3_PWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_PCR3_PWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_PCR3_ECCEN                     ((uint32_t)0x00000040)        /*!<ECC computation logic enable bit          */

#define  FMC_PCR3_TCLR                      ((uint32_t)0x00001E00)        /*!<TCLR[3:0] bits (CLE to RE delay)          */
#define  FMC_PCR3_TCLR_0                    ((uint32_t)0x00000200)        /*!<Bit 0 */
#define  FMC_PCR3_TCLR_1                    ((uint32_t)0x00000400)        /*!<Bit 1 */
#define  FMC_PCR3_TCLR_2                    ((uint32_t)0x00000800)        /*!<Bit 2 */
#define  FMC_PCR3_TCLR_3                    ((uint32_t)0x00001000)        /*!<Bit 3 */

#define  FMC_PCR3_TAR                       ((uint32_t)0x0001E000)        /*!<TAR[3:0] bits (ALE to RE delay)           */
#define  FMC_PCR3_TAR_0                     ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FMC_PCR3_TAR_1                     ((uint32_t)0x00004000)        /*!<Bit 1 */
#define  FMC_PCR3_TAR_2                     ((uint32_t)0x00008000)        /*!<Bit 2 */
#define  FMC_PCR3_TAR_3                     ((uint32_t)0x00010000)        /*!<Bit 3 */

#define  FMC_PCR3_ECCPS                     ((uint32_t)0x000E0000)        /*!<ECCPS[2:0] bits (ECC page size)           */
#define  FMC_PCR3_ECCPS_0                   ((uint32_t)0x00020000)        /*!<Bit 0 */
#define  FMC_PCR3_ECCPS_1                   ((uint32_t)0x00040000)        /*!<Bit 1 */
#define  FMC_PCR3_ECCPS_2                   ((uint32_t)0x00080000)        /*!<Bit 2 */

/******************  Bit definition for FMC_PCR4 register  *******************/
#define  FMC_PCR4_PWAITEN                   ((uint32_t)0x00000002)        /*!<Wait feature enable bit                   */
#define  FMC_PCR4_PBKEN                     ((uint32_t)0x00000004)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FMC_PCR4_PTYP                      ((uint32_t)0x00000008)        /*!<Memory type                               */

#define  FMC_PCR4_PWID                      ((uint32_t)0x00000030)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FMC_PCR4_PWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_PCR4_PWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_PCR4_ECCEN                     ((uint32_t)0x00000040)        /*!<ECC computation logic enable bit          */

#define  FMC_PCR4_TCLR                      ((uint32_t)0x00001E00)        /*!<TCLR[3:0] bits (CLE to RE delay)          */
#define  FMC_PCR4_TCLR_0                    ((uint32_t)0x00000200)        /*!<Bit 0 */
#define  FMC_PCR4_TCLR_1                    ((uint32_t)0x00000400)        /*!<Bit 1 */
#define  FMC_PCR4_TCLR_2                    ((uint32_t)0x00000800)        /*!<Bit 2 */
#define  FMC_PCR4_TCLR_3                    ((uint32_t)0x00001000)        /*!<Bit 3 */

#define  FMC_PCR4_TAR                       ((uint32_t)0x0001E000)        /*!<TAR[3:0] bits (ALE to RE delay)           */
#define  FMC_PCR4_TAR_0                     ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FMC_PCR4_TAR_1                     ((uint32_t)0x00004000)        /*!<Bit 1 */
#define  FMC_PCR4_TAR_2                     ((uint32_t)0x00008000)        /*!<Bit 2 */
#define  FMC_PCR4_TAR_3                     ((uint32_t)0x00010000)        /*!<Bit 3 */

#define  FMC_PCR4_ECCPS                     ((uint32_t)0x000E0000)        /*!<ECCPS[2:0] bits (ECC page size)           */
#define  FMC_PCR4_ECCPS_0                   ((uint32_t)0x00020000)        /*!<Bit 0 */
#define  FMC_PCR4_ECCPS_1                   ((uint32_t)0x00040000)        /*!<Bit 1 */
#define  FMC_PCR4_ECCPS_2                   ((uint32_t)0x00080000)        /*!<Bit 2 */

/*******************  Bit definition for FMC_SR2 register  *******************/
#define  FMC_SR2_IRS                        ((uint8_t)0x01)               /*!<Interrupt Rising Edge status                */
#define  FMC_SR2_ILS                        ((uint8_t)0x02)               /*!<Interrupt Level status                      */
#define  FMC_SR2_IFS                        ((uint8_t)0x04)               /*!<Interrupt Falling Edge status               */
#define  FMC_SR2_IREN                       ((uint8_t)0x08)               /*!<Interrupt Rising Edge detection Enable bit  */
#define  FMC_SR2_ILEN                       ((uint8_t)0x10)               /*!<Interrupt Level detection Enable bit        */
#define  FMC_SR2_IFEN                       ((uint8_t)0x20)               /*!<Interrupt Falling Edge detection Enable bit */
#define  FMC_SR2_FEMPT                      ((uint8_t)0x40)               /*!<FIFO empty                                  */

/*******************  Bit definition for FMC_SR3 register  *******************/
#define  FMC_SR3_IRS                        ((uint8_t)0x01)               /*!<Interrupt Rising Edge status                */
#define  FMC_SR3_ILS                        ((uint8_t)0x02)               /*!<Interrupt Level status                      */
#define  FMC_SR3_IFS                        ((uint8_t)0x04)               /*!<Interrupt Falling Edge status               */
#define  FMC_SR3_IREN                       ((uint8_t)0x08)               /*!<Interrupt Rising Edge detection Enable bit  */
#define  FMC_SR3_ILEN                       ((uint8_t)0x10)               /*!<Interrupt Level detection Enable bit        */
#define  FMC_SR3_IFEN                       ((uint8_t)0x20)               /*!<Interrupt Falling Edge detection Enable bit */
#define  FMC_SR3_FEMPT                      ((uint8_t)0x40)               /*!<FIFO empty                                  */

/*******************  Bit definition for FMC_SR4 register  *******************/
#define  FMC_SR4_IRS                        ((uint8_t)0x01)               /*!<Interrupt Rising Edge status                */
#define  FMC_SR4_ILS                        ((uint8_t)0x02)               /*!<Interrupt Level status                      */
#define  FMC_SR4_IFS                        ((uint8_t)0x04)               /*!<Interrupt Falling Edge status               */
#define  FMC_SR4_IREN                       ((uint8_t)0x08)               /*!<Interrupt Rising Edge detection Enable bit  */
#define  FMC_SR4_ILEN                       ((uint8_t)0x10)               /*!<Interrupt Level detection Enable bit        */
#define  FMC_SR4_IFEN                       ((uint8_t)0x20)               /*!<Interrupt Falling Edge detection Enable bit */
#define  FMC_SR4_FEMPT                      ((uint8_t)0x40)               /*!<FIFO empty                                  */

/******************  Bit definition for FMC_PMEM2 register  ******************/
#define  FMC_PMEM2_MEMSET2                  ((uint32_t)0x000000FF)        /*!<MEMSET2[7:0] bits (Common memory 2 setup time) */
#define  FMC_PMEM2_MEMSET2_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_PMEM2_MEMSET2_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_PMEM2_MEMSET2_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_PMEM2_MEMSET2_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FMC_PMEM2_MEMSET2_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FMC_PMEM2_MEMSET2_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FMC_PMEM2_MEMSET2_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FMC_PMEM2_MEMSET2_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FMC_PMEM2_MEMWAIT2                 ((uint32_t)0x0000FF00)        /*!<MEMWAIT2[7:0] bits (Common memory 2 wait time) */
#define  FMC_PMEM2_MEMWAIT2_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_PMEM2_MEMWAIT2_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_PMEM2_MEMWAIT2_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_PMEM2_MEMWAIT2_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_PMEM2_MEMWAIT2_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_PMEM2_MEMWAIT2_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_PMEM2_MEMWAIT2_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_PMEM2_MEMWAIT2_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_PMEM2_MEMHOLD2                 ((uint32_t)0x00FF0000)        /*!<MEMHOLD2[7:0] bits (Common memory 2 hold time) */
#define  FMC_PMEM2_MEMHOLD2_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_PMEM2_MEMHOLD2_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_PMEM2_MEMHOLD2_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_PMEM2_MEMHOLD2_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FMC_PMEM2_MEMHOLD2_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FMC_PMEM2_MEMHOLD2_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FMC_PMEM2_MEMHOLD2_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FMC_PMEM2_MEMHOLD2_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FMC_PMEM2_MEMHIZ2                  ((uint32_t)0xFF000000)        /*!<MEMHIZ2[7:0] bits (Common memory 2 databus HiZ time) */
#define  FMC_PMEM2_MEMHIZ2_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_PMEM2_MEMHIZ2_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_PMEM2_MEMHIZ2_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_PMEM2_MEMHIZ2_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FMC_PMEM2_MEMHIZ2_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FMC_PMEM2_MEMHIZ2_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FMC_PMEM2_MEMHIZ2_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FMC_PMEM2_MEMHIZ2_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FMC_PMEM3 register  ******************/
#define  FMC_PMEM3_MEMSET3                  ((uint32_t)0x000000FF)        /*!<MEMSET3[7:0] bits (Common memory 3 setup time) */
#define  FMC_PMEM3_MEMSET3_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_PMEM3_MEMSET3_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_PMEM3_MEMSET3_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_PMEM3_MEMSET3_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FMC_PMEM3_MEMSET3_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FMC_PMEM3_MEMSET3_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FMC_PMEM3_MEMSET3_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FMC_PMEM3_MEMSET3_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FMC_PMEM3_MEMWAIT3                 ((uint32_t)0x0000FF00)        /*!<MEMWAIT3[7:0] bits (Common memory 3 wait time) */
#define  FMC_PMEM3_MEMWAIT3_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_PMEM3_MEMWAIT3_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_PMEM3_MEMWAIT3_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_PMEM3_MEMWAIT3_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_PMEM3_MEMWAIT3_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_PMEM3_MEMWAIT3_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_PMEM3_MEMWAIT3_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_PMEM3_MEMWAIT3_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_PMEM3_MEMHOLD3                 ((uint32_t)0x00FF0000)        /*!<MEMHOLD3[7:0] bits (Common memory 3 hold time) */
#define  FMC_PMEM3_MEMHOLD3_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_PMEM3_MEMHOLD3_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_PMEM3_MEMHOLD3_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_PMEM3_MEMHOLD3_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FMC_PMEM3_MEMHOLD3_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FMC_PMEM3_MEMHOLD3_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FMC_PMEM3_MEMHOLD3_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FMC_PMEM3_MEMHOLD3_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FMC_PMEM3_MEMHIZ3                  ((uint32_t)0xFF000000)        /*!<MEMHIZ3[7:0] bits (Common memory 3 databus HiZ time) */
#define  FMC_PMEM3_MEMHIZ3_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_PMEM3_MEMHIZ3_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_PMEM3_MEMHIZ3_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_PMEM3_MEMHIZ3_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FMC_PMEM3_MEMHIZ3_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FMC_PMEM3_MEMHIZ3_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FMC_PMEM3_MEMHIZ3_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FMC_PMEM3_MEMHIZ3_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FMC_PMEM4 register  ******************/
#define  FMC_PMEM4_MEMSET4                  ((uint32_t)0x000000FF)        /*!<MEMSET4[7:0] bits (Common memory 4 setup time) */
#define  FMC_PMEM4_MEMSET4_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_PMEM4_MEMSET4_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_PMEM4_MEMSET4_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_PMEM4_MEMSET4_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FMC_PMEM4_MEMSET4_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FMC_PMEM4_MEMSET4_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FMC_PMEM4_MEMSET4_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FMC_PMEM4_MEMSET4_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FMC_PMEM4_MEMWAIT4                 ((uint32_t)0x0000FF00)        /*!<MEMWAIT4[7:0] bits (Common memory 4 wait time) */
#define  FMC_PMEM4_MEMWAIT4_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_PMEM4_MEMWAIT4_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_PMEM4_MEMWAIT4_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_PMEM4_MEMWAIT4_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_PMEM4_MEMWAIT4_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_PMEM4_MEMWAIT4_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_PMEM4_MEMWAIT4_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_PMEM4_MEMWAIT4_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_PMEM4_MEMHOLD4                 ((uint32_t)0x00FF0000)        /*!<MEMHOLD4[7:0] bits (Common memory 4 hold time) */
#define  FMC_PMEM4_MEMHOLD4_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_PMEM4_MEMHOLD4_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_PMEM4_MEMHOLD4_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_PMEM4_MEMHOLD4_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FMC_PMEM4_MEMHOLD4_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FMC_PMEM4_MEMHOLD4_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FMC_PMEM4_MEMHOLD4_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FMC_PMEM4_MEMHOLD4_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FMC_PMEM4_MEMHIZ4                  ((uint32_t)0xFF000000)        /*!<MEMHIZ4[7:0] bits (Common memory 4 databus HiZ time) */
#define  FMC_PMEM4_MEMHIZ4_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_PMEM4_MEMHIZ4_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_PMEM4_MEMHIZ4_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_PMEM4_MEMHIZ4_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FMC_PMEM4_MEMHIZ4_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FMC_PMEM4_MEMHIZ4_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FMC_PMEM4_MEMHIZ4_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FMC_PMEM4_MEMHIZ4_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FMC_PATT2 register  ******************/
#define  FMC_PATT2_ATTSET2                  ((uint32_t)0x000000FF)        /*!<ATTSET2[7:0] bits (Attribute memory 2 setup time) */
#define  FMC_PATT2_ATTSET2_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_PATT2_ATTSET2_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_PATT2_ATTSET2_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_PATT2_ATTSET2_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FMC_PATT2_ATTSET2_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FMC_PATT2_ATTSET2_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FMC_PATT2_ATTSET2_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FMC_PATT2_ATTSET2_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FMC_PATT2_ATTWAIT2                 ((uint32_t)0x0000FF00)        /*!<ATTWAIT2[7:0] bits (Attribute memory 2 wait time) */
#define  FMC_PATT2_ATTWAIT2_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_PATT2_ATTWAIT2_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_PATT2_ATTWAIT2_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_PATT2_ATTWAIT2_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_PATT2_ATTWAIT2_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_PATT2_ATTWAIT2_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_PATT2_ATTWAIT2_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_PATT2_ATTWAIT2_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_PATT2_ATTHOLD2                 ((uint32_t)0x00FF0000)        /*!<ATTHOLD2[7:0] bits (Attribute memory 2 hold time) */
#define  FMC_PATT2_ATTHOLD2_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_PATT2_ATTHOLD2_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_PATT2_ATTHOLD2_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_PATT2_ATTHOLD2_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FMC_PATT2_ATTHOLD2_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FMC_PATT2_ATTHOLD2_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FMC_PATT2_ATTHOLD2_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FMC_PATT2_ATTHOLD2_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FMC_PATT2_ATTHIZ2                  ((uint32_t)0xFF000000)        /*!<ATTHIZ2[7:0] bits (Attribute memory 2 databus HiZ time) */
#define  FMC_PATT2_ATTHIZ2_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_PATT2_ATTHIZ2_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_PATT2_ATTHIZ2_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_PATT2_ATTHIZ2_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FMC_PATT2_ATTHIZ2_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FMC_PATT2_ATTHIZ2_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FMC_PATT2_ATTHIZ2_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FMC_PATT2_ATTHIZ2_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FMC_PATT3 register  ******************/
#define  FMC_PATT3_ATTSET3                  ((uint32_t)0x000000FF)        /*!<ATTSET3[7:0] bits (Attribute memory 3 setup time) */
#define  FMC_PATT3_ATTSET3_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_PATT3_ATTSET3_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_PATT3_ATTSET3_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_PATT3_ATTSET3_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FMC_PATT3_ATTSET3_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FMC_PATT3_ATTSET3_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FMC_PATT3_ATTSET3_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FMC_PATT3_ATTSET3_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FMC_PATT3_ATTWAIT3                 ((uint32_t)0x0000FF00)        /*!<ATTWAIT3[7:0] bits (Attribute memory 3 wait time) */
#define  FMC_PATT3_ATTWAIT3_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_PATT3_ATTWAIT3_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_PATT3_ATTWAIT3_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_PATT3_ATTWAIT3_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_PATT3_ATTWAIT3_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_PATT3_ATTWAIT3_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_PATT3_ATTWAIT3_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_PATT3_ATTWAIT3_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_PATT3_ATTHOLD3                 ((uint32_t)0x00FF0000)        /*!<ATTHOLD3[7:0] bits (Attribute memory 3 hold time) */
#define  FMC_PATT3_ATTHOLD3_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_PATT3_ATTHOLD3_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_PATT3_ATTHOLD3_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_PATT3_ATTHOLD3_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FMC_PATT3_ATTHOLD3_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FMC_PATT3_ATTHOLD3_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FMC_PATT3_ATTHOLD3_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FMC_PATT3_ATTHOLD3_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FMC_PATT3_ATTHIZ3                  ((uint32_t)0xFF000000)        /*!<ATTHIZ3[7:0] bits (Attribute memory 3 databus HiZ time) */
#define  FMC_PATT3_ATTHIZ3_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_PATT3_ATTHIZ3_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_PATT3_ATTHIZ3_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_PATT3_ATTHIZ3_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FMC_PATT3_ATTHIZ3_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FMC_PATT3_ATTHIZ3_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FMC_PATT3_ATTHIZ3_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FMC_PATT3_ATTHIZ3_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FMC_PATT4 register  ******************/
#define  FMC_PATT4_ATTSET4                  ((uint32_t)0x000000FF)        /*!<ATTSET4[7:0] bits (Attribute memory 4 setup time) */
#define  FMC_PATT4_ATTSET4_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_PATT4_ATTSET4_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_PATT4_ATTSET4_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_PATT4_ATTSET4_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FMC_PATT4_ATTSET4_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FMC_PATT4_ATTSET4_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FMC_PATT4_ATTSET4_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FMC_PATT4_ATTSET4_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FMC_PATT4_ATTWAIT4                 ((uint32_t)0x0000FF00)        /*!<ATTWAIT4[7:0] bits (Attribute memory 4 wait time) */
#define  FMC_PATT4_ATTWAIT4_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_PATT4_ATTWAIT4_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_PATT4_ATTWAIT4_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_PATT4_ATTWAIT4_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_PATT4_ATTWAIT4_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_PATT4_ATTWAIT4_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_PATT4_ATTWAIT4_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_PATT4_ATTWAIT4_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_PATT4_ATTHOLD4                 ((uint32_t)0x00FF0000)        /*!<ATTHOLD4[7:0] bits (Attribute memory 4 hold time) */
#define  FMC_PATT4_ATTHOLD4_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_PATT4_ATTHOLD4_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_PATT4_ATTHOLD4_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_PATT4_ATTHOLD4_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FMC_PATT4_ATTHOLD4_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FMC_PATT4_ATTHOLD4_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FMC_PATT4_ATTHOLD4_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FMC_PATT4_ATTHOLD4_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FMC_PATT4_ATTHIZ4                  ((uint32_t)0xFF000000)        /*!<ATTHIZ4[7:0] bits (Attribute memory 4 databus HiZ time) */
#define  FMC_PATT4_ATTHIZ4_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_PATT4_ATTHIZ4_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_PATT4_ATTHIZ4_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_PATT4_ATTHIZ4_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FMC_PATT4_ATTHIZ4_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FMC_PATT4_ATTHIZ4_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FMC_PATT4_ATTHIZ4_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FMC_PATT4_ATTHIZ4_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FMC_PIO4 register  *******************/
#define  FMC_PIO4_IOSET4                    ((uint32_t)0x000000FF)        /*!<IOSET4[7:0] bits (I/O 4 setup time) */
#define  FMC_PIO4_IOSET4_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_PIO4_IOSET4_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_PIO4_IOSET4_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_PIO4_IOSET4_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FMC_PIO4_IOSET4_4                  ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FMC_PIO4_IOSET4_5                  ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FMC_PIO4_IOSET4_6                  ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FMC_PIO4_IOSET4_7                  ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FMC_PIO4_IOWAIT4                   ((uint32_t)0x0000FF00)        /*!<IOWAIT4[7:0] bits (I/O 4 wait time) */
#define  FMC_PIO4_IOWAIT4_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_PIO4_IOWAIT4_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_PIO4_IOWAIT4_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_PIO4_IOWAIT4_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_PIO4_IOWAIT4_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_PIO4_IOWAIT4_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_PIO4_IOWAIT4_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_PIO4_IOWAIT4_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_PIO4_IOHOLD4                   ((uint32_t)0x00FF0000)        /*!<IOHOLD4[7:0] bits (I/O 4 hold time) */
#define  FMC_PIO4_IOHOLD4_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_PIO4_IOHOLD4_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_PIO4_IOHOLD4_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_PIO4_IOHOLD4_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FMC_PIO4_IOHOLD4_4                 ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FMC_PIO4_IOHOLD4_5                 ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FMC_PIO4_IOHOLD4_6                 ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FMC_PIO4_IOHOLD4_7                 ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FMC_PIO4_IOHIZ4                    ((uint32_t)0xFF000000)        /*!<IOHIZ4[7:0] bits (I/O 4 databus HiZ time) */
#define  FMC_PIO4_IOHIZ4_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_PIO4_IOHIZ4_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_PIO4_IOHIZ4_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_PIO4_IOHIZ4_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FMC_PIO4_IOHIZ4_4                  ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FMC_PIO4_IOHIZ4_5                  ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FMC_PIO4_IOHIZ4_6                  ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FMC_PIO4_IOHIZ4_7                  ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FMC_ECCR2 register  ******************/
#define  FMC_ECCR2_ECC2                     ((uint32_t)0xFFFFFFFF)        /*!<ECC result */

/******************  Bit definition for FMC_ECCR3 register  ******************/
#define  FMC_ECCR3_ECC3                     ((uint32_t)0xFFFFFFFF)        /*!<ECC result */

/******************  Bit definition for FMC_SDCR1 register  ******************/
#define  FMC_SDCR1_NC                       ((uint32_t)0x00000003)        /*!<NC[1:0] bits (Number of column bits) */
#define  FMC_SDCR1_NC_0                     ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDCR1_NC_1                     ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  FMC_SDCR1_NR                       ((uint32_t)0x0000000C)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR1_NR_0                     ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_SDCR1_NR_1                     ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_SDCR1_MWID                     ((uint32_t)0x00000030)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR1_MWID_0                   ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_SDCR1_MWID_1                   ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_SDCR1_NB                       ((uint32_t)0x00000040)        /*!<Number of internal bank */

#define  FMC_SDCR1_CAS                      ((uint32_t)0x00000180)        /*!<CAS[1:0] bits (CAS latency) */
#define  FMC_SDCR1_CAS_0                    ((uint32_t)0x00000080)        /*!<Bit 0 */
#define  FMC_SDCR1_CAS_1                    ((uint32_t)0x00000100)        /*!<Bit 1 */

#define  FMC_SDCR1_WP                       ((uint32_t)0x00000200)        /*!<Write protection */

#define  FMC_SDCR1_SDCLK                    ((uint32_t)0x00000C00)        /*!<SDRAM clock configuration */
#define  FMC_SDCR1_SDCLK_0                  ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  FMC_SDCR1_SDCLK_1                  ((uint32_t)0x00000800)        /*!<Bit 1 */

#define  FMC_SDCR1_RBURST                   ((uint32_t)0x00001000)        /*!<Read burst */

#define  FMC_SDCR1_RPIPE                    ((uint32_t)0x00006000)        /*!<Write protection */
#define  FMC_SDCR1_RPIPE_0                  ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FMC_SDCR1_RPIPE_1                  ((uint32_t)0x00004000)        /*!<Bit 1 */

/******************  Bit definition for FMC_SDCR2 register  ******************/
#define  FMC_SDCR2_NC                       ((uint32_t)0x00000003)        /*!<NC[1:0] bits (Number of column bits) */
#define  FMC_SDCR2_NC_0                     ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDCR2_NC_1                     ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  FMC_SDCR2_NR                       ((uint32_t)0x0000000C)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR2_NR_0                     ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_SDCR2_NR_1                     ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_SDCR2_MWID                     ((uint32_t)0x00000030)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR2_MWID_0                   ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_SDCR2_MWID_1                   ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_SDCR2_NB                       ((uint32_t)0x00000040)        /*!<Number of internal bank */

#define  FMC_SDCR2_CAS                      ((uint32_t)0x00000180)        /*!<CAS[1:0] bits (CAS latency) */
#define  FMC_SDCR2_CAS_0                    ((uint32_t)0x00000080)        /*!<Bit 0 */
#define  FMC_SDCR2_CAS_1                    ((uint32_t)0x00000100)        /*!<Bit 1 */

#define  FMC_SDCR2_WP                       ((uint32_t)0x00000200)        /*!<Write protection */

#define  FMC_SDCR2_SDCLK                    ((uint32_t)0x00000C00)        /*!<SDCLK[1:0] (SDRAM clock configuration) */
#define  FMC_SDCR2_SDCLK_0                  ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  FMC_SDCR2_SDCLK_1                  ((uint32_t)0x00000800)        /*!<Bit 1 */

#define  FMC_SDCR2_RBURST                   ((uint32_t)0x00001000)        /*!<Read burst */

#define  FMC_SDCR2_RPIPE                    ((uint32_t)0x00006000)        /*!<RPIPE[1:0](Read pipe) */
#define  FMC_SDCR2_RPIPE_0                  ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FMC_SDCR2_RPIPE_1                  ((uint32_t)0x00004000)        /*!<Bit 1 */

/******************  Bit definition for FMC_SDTR1 register  ******************/
#define  FMC_SDTR1_TMRD                     ((uint32_t)0x0000000F)        /*!<TMRD[3:0] bits (Load mode register to active) */
#define  FMC_SDTR1_TMRD_0                   ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDTR1_TMRD_1                   ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_SDTR1_TMRD_2                   ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_SDTR1_TMRD_3                   ((uint32_t)0x00000008)        /*!<Bit 3 */
                                            
#define  FMC_SDTR1_TXSR                     ((uint32_t)0x000000F0)        /*!<TXSR[3:0] bits (Exit self refresh) */
#define  FMC_SDTR1_TXSR_0                   ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_SDTR1_TXSR_1                   ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_SDTR1_TXSR_2                   ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_SDTR1_TXSR_3                   ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_SDTR1_TRAS                     ((uint32_t)0x00000F00)        /*!<TRAS[3:0] bits (Self refresh time) */
#define  FMC_SDTR1_TRAS_0                   ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_SDTR1_TRAS_1                   ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_SDTR1_TRAS_2                   ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_SDTR1_TRAS_3                   ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FMC_SDTR1_TRC                      ((uint32_t)0x0000F000)        /*!<TRC[2:0] bits (Row cycle delay) */
#define  FMC_SDTR1_TRC_0                    ((uint32_t)0x00001000)        /*!<Bit 0 */
#define  FMC_SDTR1_TRC_1                    ((uint32_t)0x00002000)        /*!<Bit 1 */
#define  FMC_SDTR1_TRC_2                    ((uint32_t)0x00004000)        /*!<Bit 2 */

#define  FMC_SDTR1_TWR                      ((uint32_t)0x000F0000)        /*!<TRC[2:0] bits (Write recovery delay) */
#define  FMC_SDTR1_TWR_0                    ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_SDTR1_TWR_1                    ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_SDTR1_TWR_2                    ((uint32_t)0x00040000)        /*!<Bit 2 */

#define  FMC_SDTR1_TRP                      ((uint32_t)0x00F00000)        /*!<TRP[2:0] bits (Row precharge delay) */
#define  FMC_SDTR1_TRP_0                    ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_SDTR1_TRP_1                    ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_SDTR1_TRP_2                    ((uint32_t)0x00400000)        /*!<Bit 2 */

#define  FMC_SDTR1_TRCD                     ((uint32_t)0x0F000000)        /*!<TRP[2:0] bits (Row to column delay) */
#define  FMC_SDTR1_TRCD_0                   ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_SDTR1_TRCD_1                   ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_SDTR1_TRCD_2                   ((uint32_t)0x04000000)        /*!<Bit 2 */

/******************  Bit definition for FMC_SDTR2 register  ******************/
#define  FMC_SDTR2_TMRD                     ((uint32_t)0x0000000F)        /*!<TMRD[3:0] bits (Load mode register to active) */
#define  FMC_SDTR2_TMRD_0                   ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDTR2_TMRD_1                   ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_SDTR2_TMRD_2                   ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_SDTR2_TMRD_3                   ((uint32_t)0x00000008)        /*!<Bit 3 */
                                            
#define  FMC_SDTR2_TXSR                     ((uint32_t)0x000000F0)        /*!<TXSR[3:0] bits (Exit self refresh) */
#define  FMC_SDTR2_TXSR_0                   ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_SDTR2_TXSR_1                   ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_SDTR2_TXSR_2                   ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_SDTR2_TXSR_3                   ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_SDTR2_TRAS                     ((uint32_t)0x00000F00)        /*!<TRAS[3:0] bits (Self refresh time) */
#define  FMC_SDTR2_TRAS_0                   ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_SDTR2_TRAS_1                   ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_SDTR2_TRAS_2                   ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_SDTR2_TRAS_3                   ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FMC_SDTR2_TRC                      ((uint32_t)0x0000F000)        /*!<TRC[2:0] bits (Row cycle delay) */
#define  FMC_SDTR2_TRC_0                    ((uint32_t)0x00001000)        /*!<Bit 0 */
#define  FMC_SDTR2_TRC_1                    ((uint32_t)0x00002000)        /*!<Bit 1 */
#define  FMC_SDTR2_TRC_2                    ((uint32_t)0x00004000)        /*!<Bit 2 */

#define  FMC_SDTR2_TWR                      ((uint32_t)0x000F0000)        /*!<TRC[2:0] bits (Write recovery delay) */
#define  FMC_SDTR2_TWR_0                    ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_SDTR2_TWR_1                    ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_SDTR2_TWR_2                    ((uint32_t)0x00040000)        /*!<Bit 2 */

#define  FMC_SDTR2_TRP                      ((uint32_t)0x00F00000)        /*!<TRP[2:0] bits (Row precharge delay) */
#define  FMC_SDTR2_TRP_0                    ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_SDTR2_TRP_1                    ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_SDTR2_TRP_2                    ((uint32_t)0x00400000)        /*!<Bit 2 */

#define  FMC_SDTR2_TRCD                     ((uint32_t)0x0F000000)        /*!<TRP[2:0] bits (Row to column delay) */
#define  FMC_SDTR2_TRCD_0                   ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_SDTR2_TRCD_1                   ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_SDTR2_TRCD_2                   ((uint32_t)0x04000000)        /*!<Bit 2 */

/******************  Bit definition for FMC_SDCMR register  ******************/
#define  FMC_SDCMR_MODE                     ((uint32_t)0x00000007)        /*!<MODE[2:0] bits (Command mode) */
#define  FMC_SDCMR_MODE_0                   ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDCMR_MODE_1                   ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_SDCMR_MODE_2                   ((uint32_t)0x00000003)        /*!<Bit 2 */
                                            
#define  FMC_SDCMR_CTB2                     ((uint32_t)0x00000008)        /*!<Command target 2 */

#define  FMC_SDCMR_CTB1                     ((uint32_t)0x00000010)        /*!<Command target 1 */

#define  FMC_SDCMR_NRFS                     ((uint32_t)0x000001E0)        /*!<NRFS[3:0] bits (Number of auto-refresh) */
#define  FMC_SDCMR_NRFS_0                   ((uint32_t)0x00000020)        /*!<Bit 0 */
#define  FMC_SDCMR_NRFS_1                   ((uint32_t)0x00000040)        /*!<Bit 1 */
#define  FMC_SDCMR_NRFS_2                   ((uint32_t)0x00000080)        /*!<Bit 2 */
#define  FMC_SDCMR_NRFS_3                   ((uint32_t)0x00000100)        /*!<Bit 3 */

#define  FMC_SDCMR_MRD                      ((uint32_t)0x003FFE00)        /*!<MRD[12:0] bits (Mode register definition) */

/******************  Bit definition for FMC_SDRTR register  ******************/
#define  FMC_SDRTR_CRE                      ((uint32_t)0x00000001)        /*!<Clear refresh error flag */

#define  FMC_SDRTR_COUNT                    ((uint32_t)0x00003FFE)        /*!<COUNT[12:0] bits (Refresh timer count) */

#define  FMC_SDRTR_REIE                     ((uint32_t)0x00004000)        /*!<RES interupt enable */

/******************  Bit definition for FMC_SDSR register  ******************/
#define  FMC_SDSR_RE                        ((uint32_t)0x00000001)        /*!<Refresh error flag */

#define  FMC_SDSR_MODES1                    ((uint32_t)0x00000006)        /*!<MODES1[1:0]bits (Status mode for bank 1) */
#define  FMC_SDSR_MODES1_0                  ((uint32_t)0x00000002)        /*!<Bit 0 */
#define  FMC_SDSR_MODES1_1                  ((uint32_t)0x00000004)        /*!<Bit 1 */

#define  FMC_SDSR_MODES2                    ((uint32_t)0x00000018)        /*!<MODES2[1:0]bits (Status mode for bank 2) */
#define  FMC_SDSR_MODES2_0                  ((uint32_t)0x00000008)        /*!<Bit 0 */
#define  FMC_SDSR_MODES2_1                  ((uint32_t)0x00000010)        /*!<Bit 1 */

#define  FMC_SDSR_BUSY                      ((uint32_t)0x00000020)        /*!<Busy status */

#endif /* STM32F427_437xx ||  STM32F429_439xx */

/******************************************************************************/
/*                                                                            */
/*                            General Purpose I/O                             */
/*                                                                            */
/******************************************************************************/
/******************  Bits definition for GPIO_MODER register  *****************/
#define GPIO_MODER_MODER0                    ((uint32_t)0x00000003)
#define GPIO_MODER_MODER0_0                  ((uint32_t)0x00000001)
#define GPIO_MODER_MODER0_1                  ((uint32_t)0x00000002)

#define GPIO_MODER_MODER1                    ((uint32_t)0x0000000C)
#define GPIO_MODER_MODER1_0                  ((uint32_t)0x00000004)
#define GPIO_MODER_MODER1_1                  ((uint32_t)0x00000008)

#define GPIO_MODER_MODER2                    ((uint32_t)0x00000030)
#define GPIO_MODER_MODER2_0                  ((uint32_t)0x00000010)
#define GPIO_MODER_MODER2_1                  ((uint32_t)0x00000020)

#define GPIO_MODER_MODER3                    ((uint32_t)0x000000C0)
#define GPIO_MODER_MODER3_0                  ((uint32_t)0x00000040)
#define GPIO_MODER_MODER3_1                  ((uint32_t)0x00000080)

#define GPIO_MODER_MODER4                    ((uint32_t)0x00000300)
#define GPIO_MODER_MODER4_0                  ((uint32_t)0x00000100)
#define GPIO_MODER_MODER4_1                  ((uint32_t)0x00000200)

#define GPIO_MODER_MODER5                    ((uint32_t)0x00000C00)
#define GPIO_MODER_MODER5_0                  ((uint32_t)0x00000400)
#define GPIO_MODER_MODER5_1                  ((uint32_t)0x00000800)

#define GPIO_MODER_MODER6                    ((uint32_t)0x00003000)
#define GPIO_MODER_MODER6_0                  ((uint32_t)0x00001000)
#define GPIO_MODER_MODER6_1                  ((uint32_t)0x00002000)

#define GPIO_MODER_MODER7                    ((uint32_t)0x0000C000)
#define GPIO_MODER_MODER7_0                  ((uint32_t)0x00004000)
#define GPIO_MODER_MODER7_1                  ((uint32_t)0x00008000)

#define GPIO_MODER_MODER8                    ((uint32_t)0x00030000)
#define GPIO_MODER_MODER8_0                  ((uint32_t)0x00010000)
#define GPIO_MODER_MODER8_1                  ((uint32_t)0x00020000)

#define GPIO_MODER_MODER9                    ((uint32_t)0x000C0000)
#define GPIO_MODER_MODER9_0                  ((uint32_t)0x00040000)
#define GPIO_MODER_MODER9_1                  ((uint32_t)0x00080000)

#define GPIO_MODER_MODER10                   ((uint32_t)0x00300000)
#define GPIO_MODER_MODER10_0                 ((uint32_t)0x00100000)
#define GPIO_MODER_MODER10_1                 ((uint32_t)0x00200000)

#define GPIO_MODER_MODER11                   ((uint32_t)0x00C00000)
#define GPIO_MODER_MODER11_0                 ((uint32_t)0x00400000)
#define GPIO_MODER_MODER11_1                 ((uint32_t)0x00800000)

#define GPIO_MODER_MODER12                   ((uint32_t)0x03000000)
#define GPIO_MODER_MODER12_0                 ((uint32_t)0x01000000)
#define GPIO_MODER_MODER12_1                 ((uint32_t)0x02000000)

#define GPIO_MODER_MODER13                   ((uint32_t)0x0C000000)
#define GPIO_MODER_MODER13_0                 ((uint32_t)0x04000000)
#define GPIO_MODER_MODER13_1                 ((uint32_t)0x08000000)

#define GPIO_MODER_MODER14                   ((uint32_t)0x30000000)
#define GPIO_MODER_MODER14_0                 ((uint32_t)0x10000000)
#define GPIO_MODER_MODER14_1                 ((uint32_t)0x20000000)

#define GPIO_MODER_MODER15                   ((uint32_t)0xC0000000)
#define GPIO_MODER_MODER15_0                 ((uint32_t)0x40000000)
#define GPIO_MODER_MODER15_1                 ((uint32_t)0x80000000)

/******************  Bits definition for GPIO_OTYPER register  ****************/
#define GPIO_OTYPER_OT_0                     ((uint32_t)0x00000001)
#define GPIO_OTYPER_OT_1                     ((uint32_t)0x00000002)
#define GPIO_OTYPER_OT_2                     ((uint32_t)0x00000004)
#define GPIO_OTYPER_OT_3                     ((uint32_t)0x00000008)
#define GPIO_OTYPER_OT_4                     ((uint32_t)0x00000010)
#define GPIO_OTYPER_OT_5                     ((uint32_t)0x00000020)
#define GPIO_OTYPER_OT_6                     ((uint32_t)0x00000040)
#define GPIO_OTYPER_OT_7                     ((uint32_t)0x00000080)
#define GPIO_OTYPER_OT_8                     ((uint32_t)0x00000100)
#define GPIO_OTYPER_OT_9                     ((uint32_t)0x00000200)
#define GPIO_OTYPER_OT_10                    ((uint32_t)0x00000400)
#define GPIO_OTYPER_OT_11                    ((uint32_t)0x00000800)
#define GPIO_OTYPER_OT_12                    ((uint32_t)0x00001000)
#define GPIO_OTYPER_OT_13                    ((uint32_t)0x00002000)
#define GPIO_OTYPER_OT_14                    ((uint32_t)0x00004000)
#define GPIO_OTYPER_OT_15                    ((uint32_t)0x00008000)

/******************  Bits definition for GPIO_OSPEEDR register  ***************/
#define GPIO_OSPEEDER_OSPEEDR0               ((uint32_t)0x00000003)
#define GPIO_OSPEEDER_OSPEEDR0_0             ((uint32_t)0x00000001)
#define GPIO_OSPEEDER_OSPEEDR0_1             ((uint32_t)0x00000002)

#define GPIO_OSPEEDER_OSPEEDR1               ((uint32_t)0x0000000C)
#define GPIO_OSPEEDER_OSPEEDR1_0             ((uint32_t)0x00000004)
#define GPIO_OSPEEDER_OSPEEDR1_1             ((uint32_t)0x00000008)

#define GPIO_OSPEEDER_OSPEEDR2               ((uint32_t)0x00000030)
#define GPIO_OSPEEDER_OSPEEDR2_0             ((uint32_t)0x00000010)
#define GPIO_OSPEEDER_OSPEEDR2_1             ((uint32_t)0x00000020)

#define GPIO_OSPEEDER_OSPEEDR3               ((uint32_t)0x000000C0)
#define GPIO_OSPEEDER_OSPEEDR3_0             ((uint32_t)0x00000040)
#define GPIO_OSPEEDER_OSPEEDR3_1             ((uint32_t)0x00000080)

#define GPIO_OSPEEDER_OSPEEDR4               ((uint32_t)0x00000300)
#define GPIO_OSPEEDER_OSPEEDR4_0             ((uint32_t)0x00000100)
#define GPIO_OSPEEDER_OSPEEDR4_1             ((uint32_t)0x00000200)

#define GPIO_OSPEEDER_OSPEEDR5               ((uint32_t)0x00000C00)
#define GPIO_OSPEEDER_OSPEEDR5_0             ((uint32_t)0x00000400)
#define GPIO_OSPEEDER_OSPEEDR5_1             ((uint32_t)0x00000800)

#define GPIO_OSPEEDER_OSPEEDR6               ((uint32_t)0x00003000)
#define GPIO_OSPEEDER_OSPEEDR6_0             ((uint32_t)0x00001000)
#define GPIO_OSPEEDER_OSPEEDR6_1             ((uint32_t)0x00002000)

#define GPIO_OSPEEDER_OSPEEDR7               ((uint32_t)0x0000C000)
#define GPIO_OSPEEDER_OSPEEDR7_0             ((uint32_t)0x00004000)
#define GPIO_OSPEEDER_OSPEEDR7_1             ((uint32_t)0x00008000)

#define GPIO_OSPEEDER_OSPEEDR8               ((uint32_t)0x00030000)
#define GPIO_OSPEEDER_OSPEEDR8_0             ((uint32_t)0x00010000)
#define GPIO_OSPEEDER_OSPEEDR8_1             ((uint32_t)0x00020000)

#define GPIO_OSPEEDER_OSPEEDR9               ((uint32_t)0x000C0000)
#define GPIO_OSPEEDER_OSPEEDR9_0             ((uint32_t)0x00040000)
#define GPIO_OSPEEDER_OSPEEDR9_1             ((uint32_t)0x00080000)

#define GPIO_OSPEEDER_OSPEEDR10              ((uint32_t)0x00300000)
#define GPIO_OSPEEDER_OSPEEDR10_0            ((uint32_t)0x00100000)
#define GPIO_OSPEEDER_OSPEEDR10_1            ((uint32_t)0x00200000)

#define GPIO_OSPEEDER_OSPEEDR11              ((uint32_t)0x00C00000)
#define GPIO_OSPEEDER_OSPEEDR11_0            ((uint32_t)0x00400000)
#define GPIO_OSPEEDER_OSPEEDR11_1            ((uint32_t)0x00800000)

#define GPIO_OSPEEDER_OSPEEDR12              ((uint32_t)0x03000000)
#define GPIO_OSPEEDER_OSPEEDR12_0            ((uint32_t)0x01000000)
#define GPIO_OSPEEDER_OSPEEDR12_1            ((uint32_t)0x02000000)

#define GPIO_OSPEEDER_OSPEEDR13              ((uint32_t)0x0C000000)
#define GPIO_OSPEEDER_OSPEEDR13_0            ((uint32_t)0x04000000)
#define GPIO_OSPEEDER_OSPEEDR13_1            ((uint32_t)0x08000000)

#define GPIO_OSPEEDER_OSPEEDR14              ((uint32_t)0x30000000)
#define GPIO_OSPEEDER_OSPEEDR14_0            ((uint32_t)0x10000000)
#define GPIO_OSPEEDER_OSPEEDR14_1            ((uint32_t)0x20000000)

#define GPIO_OSPEEDER_OSPEEDR15              ((uint32_t)0xC0000000)
#define GPIO_OSPEEDER_OSPEEDR15_0            ((uint32_t)0x40000000)
#define GPIO_OSPEEDER_OSPEEDR15_1            ((uint32_t)0x80000000)

/******************  Bits definition for GPIO_PUPDR register  *****************/
#define GPIO_PUPDR_PUPDR0                    ((uint32_t)0x00000003)
#define GPIO_PUPDR_PUPDR0_0                  ((uint32_t)0x00000001)
#define GPIO_PUPDR_PUPDR0_1                  ((uint32_t)0x00000002)

#define GPIO_PUPDR_PUPDR1                    ((uint32_t)0x0000000C)
#define GPIO_PUPDR_PUPDR1_0                  ((uint32_t)0x00000004)
#define GPIO_PUPDR_PUPDR1_1                  ((uint32_t)0x00000008)

#define GPIO_PUPDR_PUPDR2                    ((uint32_t)0x00000030)
#define GPIO_PUPDR_PUPDR2_0                  ((uint32_t)0x00000010)
#define GPIO_PUPDR_PUPDR2_1                  ((uint32_t)0x00000020)

#define GPIO_PUPDR_PUPDR3                    ((uint32_t)0x000000C0)
#define GPIO_PUPDR_PUPDR3_0                  ((uint32_t)0x00000040)
#define GPIO_PUPDR_PUPDR3_1                  ((uint32_t)0x00000080)

#define GPIO_PUPDR_PUPDR4                    ((uint32_t)0x00000300)
#define GPIO_PUPDR_PUPDR4_0                  ((uint32_t)0x00000100)
#define GPIO_PUPDR_PUPDR4_1                  ((uint32_t)0x00000200)

#define GPIO_PUPDR_PUPDR5                    ((uint32_t)0x00000C00)
#define GPIO_PUPDR_PUPDR5_0                  ((uint32_t)0x00000400)
#define GPIO_PUPDR_PUPDR5_1                  ((uint32_t)0x00000800)

#define GPIO_PUPDR_PUPDR6                    ((uint32_t)0x00003000)
#define GPIO_PUPDR_PUPDR6_0                  ((uint32_t)0x00001000)
#define GPIO_PUPDR_PUPDR6_1                  ((uint32_t)0x00002000)

#define GPIO_PUPDR_PUPDR7                    ((uint32_t)0x0000C000)
#define GPIO_PUPDR_PUPDR7_0                  ((uint32_t)0x00004000)
#define GPIO_PUPDR_PUPDR7_1                  ((uint32_t)0x00008000)

#define GPIO_PUPDR_PUPDR8                    ((uint32_t)0x00030000)
#define GPIO_PUPDR_PUPDR8_0                  ((uint32_t)0x00010000)
#define GPIO_PUPDR_PUPDR8_1                  ((uint32_t)0x00020000)

#define GPIO_PUPDR_PUPDR9                    ((uint32_t)0x000C0000)
#define GPIO_PUPDR_PUPDR9_0                  ((uint32_t)0x00040000)
#define GPIO_PUPDR_PUPDR9_1                  ((uint32_t)0x00080000)

#define GPIO_PUPDR_PUPDR10                   ((uint32_t)0x00300000)
#define GPIO_PUPDR_PUPDR10_0                 ((uint32_t)0x00100000)
#define GPIO_PUPDR_PUPDR10_1                 ((uint32_t)0x00200000)

#define GPIO_PUPDR_PUPDR11                   ((uint32_t)0x00C00000)
#define GPIO_PUPDR_PUPDR11_0                 ((uint32_t)0x00400000)
#define GPIO_PUPDR_PUPDR11_1                 ((uint32_t)0x00800000)

#define GPIO_PUPDR_PUPDR12                   ((uint32_t)0x03000000)
#define GPIO_PUPDR_PUPDR12_0                 ((uint32_t)0x01000000)
#define GPIO_PUPDR_PUPDR12_1                 ((uint32_t)0x02000000)

#define GPIO_PUPDR_PUPDR13                   ((uint32_t)0x0C000000)
#define GPIO_PUPDR_PUPDR13_0                 ((uint32_t)0x04000000)
#define GPIO_PUPDR_PUPDR13_1                 ((uint32_t)0x08000000)

#define GPIO_PUPDR_PUPDR14                   ((uint32_t)0x30000000)
#define GPIO_PUPDR_PUPDR14_0                 ((uint32_t)0x10000000)
#define GPIO_PUPDR_PUPDR14_1                 ((uint32_t)0x20000000)

#define GPIO_PUPDR_PUPDR15                   ((uint32_t)0xC0000000)
#define GPIO_PUPDR_PUPDR15_0                 ((uint32_t)0x40000000)
#define GPIO_PUPDR_PUPDR15_1                 ((uint32_t)0x80000000)

/******************  Bits definition for GPIO_IDR register  *******************/
#define GPIO_IDR_IDR_0                       ((uint32_t)0x00000001)
#define GPIO_IDR_IDR_1                       ((uint32_t)0x00000002)
#define GPIO_IDR_IDR_2                       ((uint32_t)0x00000004)
#define GPIO_IDR_IDR_3                       ((uint32_t)0x00000008)
#define GPIO_IDR_IDR_4                       ((uint32_t)0x00000010)
#define GPIO_IDR_IDR_5                       ((uint32_t)0x00000020)
#define GPIO_IDR_IDR_6                       ((uint32_t)0x00000040)
#define GPIO_IDR_IDR_7                       ((uint32_t)0x00000080)
#define GPIO_IDR_IDR_8                       ((uint32_t)0x00000100)
#define GPIO_IDR_IDR_9                       ((uint32_t)0x00000200)
#define GPIO_IDR_IDR_10                      ((uint32_t)0x00000400)
#define GPIO_IDR_IDR_11                      ((uint32_t)0x00000800)
#define GPIO_IDR_IDR_12                      ((uint32_t)0x00001000)
#define GPIO_IDR_IDR_13                      ((uint32_t)0x00002000)
#define GPIO_IDR_IDR_14                      ((uint32_t)0x00004000)
#define GPIO_IDR_IDR_15                      ((uint32_t)0x00008000)
/* Old GPIO_IDR register bits definition, maintained for legacy purpose */
#define GPIO_OTYPER_IDR_0                    GPIO_IDR_IDR_0
#define GPIO_OTYPER_IDR_1                    GPIO_IDR_IDR_1
#define GPIO_OTYPER_IDR_2                    GPIO_IDR_IDR_2
#define GPIO_OTYPER_IDR_3                    GPIO_IDR_IDR_3
#define GPIO_OTYPER_IDR_4                    GPIO_IDR_IDR_4
#define GPIO_OTYPER_IDR_5                    GPIO_IDR_IDR_5
#define GPIO_OTYPER_IDR_6                    GPIO_IDR_IDR_6
#define GPIO_OTYPER_IDR_7                    GPIO_IDR_IDR_7
#define GPIO_OTYPER_IDR_8                    GPIO_IDR_IDR_8
#define GPIO_OTYPER_IDR_9                    GPIO_IDR_IDR_9
#define GPIO_OTYPER_IDR_10                   GPIO_IDR_IDR_10
#define GPIO_OTYPER_IDR_11                   GPIO_IDR_IDR_11
#define GPIO_OTYPER_IDR_12                   GPIO_IDR_IDR_12
#define GPIO_OTYPER_IDR_13                   GPIO_IDR_IDR_13
#define GPIO_OTYPER_IDR_14                   GPIO_IDR_IDR_14
#define GPIO_OTYPER_IDR_15                   GPIO_IDR_IDR_15

/******************  Bits definition for GPIO_ODR register  *******************/
#define GPIO_ODR_ODR_0                       ((uint32_t)0x00000001)
#define GPIO_ODR_ODR_1                       ((uint32_t)0x00000002)
#define GPIO_ODR_ODR_2                       ((uint32_t)0x00000004)
#define GPIO_ODR_ODR_3                       ((uint32_t)0x00000008)
#define GPIO_ODR_ODR_4                       ((uint32_t)0x00000010)
#define GPIO_ODR_ODR_5                       ((uint32_t)0x00000020)
#define GPIO_ODR_ODR_6                       ((uint32_t)0x00000040)
#define GPIO_ODR_ODR_7                       ((uint32_t)0x00000080)
#define GPIO_ODR_ODR_8                       ((uint32_t)0x00000100)
#define GPIO_ODR_ODR_9                       ((uint32_t)0x00000200)
#define GPIO_ODR_ODR_10                      ((uint32_t)0x00000400)
#define GPIO_ODR_ODR_11                      ((uint32_t)0x00000800)
#define GPIO_ODR_ODR_12                      ((uint32_t)0x00001000)
#define GPIO_ODR_ODR_13                      ((uint32_t)0x00002000)
#define GPIO_ODR_ODR_14                      ((uint32_t)0x00004000)
#define GPIO_ODR_ODR_15                      ((uint32_t)0x00008000)
/* Old GPIO_ODR register bits definition, maintained for legacy purpose */
#define GPIO_OTYPER_ODR_0                    GPIO_ODR_ODR_0
#define GPIO_OTYPER_ODR_1                    GPIO_ODR_ODR_1
#define GPIO_OTYPER_ODR_2                    GPIO_ODR_ODR_2
#define GPIO_OTYPER_ODR_3                    GPIO_ODR_ODR_3
#define GPIO_OTYPER_ODR_4                    GPIO_ODR_ODR_4
#define GPIO_OTYPER_ODR_5                    GPIO_ODR_ODR_5
#define GPIO_OTYPER_ODR_6                    GPIO_ODR_ODR_6
#define GPIO_OTYPER_ODR_7                    GPIO_ODR_ODR_7
#define GPIO_OTYPER_ODR_8                    GPIO_ODR_ODR_8
#define GPIO_OTYPER_ODR_9                    GPIO_ODR_ODR_9
#define GPIO_OTYPER_ODR_10                   GPIO_ODR_ODR_10
#define GPIO_OTYPER_ODR_11                   GPIO_ODR_ODR_11
#define GPIO_OTYPER_ODR_12                   GPIO_ODR_ODR_12
#define GPIO_OTYPER_ODR_13                   GPIO_ODR_ODR_13
#define GPIO_OTYPER_ODR_14                   GPIO_ODR_ODR_14
#define GPIO_OTYPER_ODR_15                   GPIO_ODR_ODR_15

/******************  Bits definition for GPIO_BSRR register  ******************/
#define GPIO_BSRR_BS_0                       ((uint32_t)0x00000001)
#define GPIO_BSRR_BS_1                       ((uint32_t)0x00000002)
#define GPIO_BSRR_BS_2                       ((uint32_t)0x00000004)
#define GPIO_BSRR_BS_3                       ((uint32_t)0x00000008)
#define GPIO_BSRR_BS_4                       ((uint32_t)0x00000010)
#define GPIO_BSRR_BS_5                       ((uint32_t)0x00000020)
#define GPIO_BSRR_BS_6                       ((uint32_t)0x00000040)
#define GPIO_BSRR_BS_7                       ((uint32_t)0x00000080)
#define GPIO_BSRR_BS_8                       ((uint32_t)0x00000100)
#define GPIO_BSRR_BS_9                       ((uint32_t)0x00000200)
#define GPIO_BSRR_BS_10                      ((uint32_t)0x00000400)
#define GPIO_BSRR_BS_11                      ((uint32_t)0x00000800)
#define GPIO_BSRR_BS_12                      ((uint32_t)0x00001000)
#define GPIO_BSRR_BS_13                      ((uint32_t)0x00002000)
#define GPIO_BSRR_BS_14                      ((uint32_t)0x00004000)
#define GPIO_BSRR_BS_15                      ((uint32_t)0x00008000)
#define GPIO_BSRR_BR_0                       ((uint32_t)0x00010000)
#define GPIO_BSRR_BR_1                       ((uint32_t)0x00020000)
#define GPIO_BSRR_BR_2                       ((uint32_t)0x00040000)
#define GPIO_BSRR_BR_3                       ((uint32_t)0x00080000)
#define GPIO_BSRR_BR_4                       ((uint32_t)0x00100000)
#define GPIO_BSRR_BR_5                       ((uint32_t)0x00200000)
#define GPIO_BSRR_BR_6                       ((uint32_t)0x00400000)
#define GPIO_BSRR_BR_7                       ((uint32_t)0x00800000)
#define GPIO_BSRR_BR_8                       ((uint32_t)0x01000000)
#define GPIO_BSRR_BR_9                       ((uint32_t)0x02000000)
#define GPIO_BSRR_BR_10                      ((uint32_t)0x04000000)
#define GPIO_BSRR_BR_11                      ((uint32_t)0x08000000)
#define GPIO_BSRR_BR_12                      ((uint32_t)0x10000000)
#define GPIO_BSRR_BR_13                      ((uint32_t)0x20000000)
#define GPIO_BSRR_BR_14                      ((uint32_t)0x40000000)
#define GPIO_BSRR_BR_15                      ((uint32_t)0x80000000)

/******************************************************************************/
/*                                                                            */
/*                                    HASH                                    */
/*                                                                            */
/******************************************************************************/
/******************  Bits definition for HASH_CR register  ********************/
#define HASH_CR_INIT                         ((uint32_t)0x00000004)
#define HASH_CR_DMAE                         ((uint32_t)0x00000008)
#define HASH_CR_DATATYPE                     ((uint32_t)0x00000030)
#define HASH_CR_DATATYPE_0                   ((uint32_t)0x00000010)
#define HASH_CR_DATATYPE_1                   ((uint32_t)0x00000020)
#define HASH_CR_MODE                         ((uint32_t)0x00000040)
#define HASH_CR_ALGO                         ((uint32_t)0x00040080)
#define HASH_CR_ALGO_0                       ((uint32_t)0x00000080)
#define HASH_CR_ALGO_1                       ((uint32_t)0x00040000)
#define HASH_CR_NBW                          ((uint32_t)0x00000F00)
#define HASH_CR_NBW_0                        ((uint32_t)0x00000100)
#define HASH_CR_NBW_1                        ((uint32_t)0x00000200)
#define HASH_CR_NBW_2                        ((uint32_t)0x00000400)
#define HASH_CR_NBW_3                        ((uint32_t)0x00000800)
#define HASH_CR_DINNE                        ((uint32_t)0x00001000)
#define HASH_CR_MDMAT                        ((uint32_t)0x00002000)
#define HASH_CR_LKEY                         ((uint32_t)0x00010000)

/******************  Bits definition for HASH_STR register  *******************/
#define HASH_STR_NBW                         ((uint32_t)0x0000001F)
#define HASH_STR_NBW_0                       ((uint32_t)0x00000001)
#define HASH_STR_NBW_1                       ((uint32_t)0x00000002)
#define HASH_STR_NBW_2                       ((uint32_t)0x00000004)
#define HASH_STR_NBW_3                       ((uint32_t)0x00000008)
#define HASH_STR_NBW_4                       ((uint32_t)0x00000010)
#define HASH_STR_DCAL                        ((uint32_t)0x00000100)

/******************  Bits definition for HASH_IMR register  *******************/
#define HASH_IMR_DINIM                       ((uint32_t)0x00000001)
#define HASH_IMR_DCIM                        ((uint32_t)0x00000002)

/******************  Bits definition for HASH_SR register  ********************/
#define HASH_SR_DINIS                        ((uint32_t)0x00000001)
#define HASH_SR_DCIS                         ((uint32_t)0x00000002)
#define HASH_SR_DMAS                         ((uint32_t)0x00000004)
#define HASH_SR_BUSY                         ((uint32_t)0x00000008)

/******************************************************************************/
/*                                                                            */
/*                      Inter-integrated Circuit Interface                    */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for I2C_CR1 register  ********************/
#define  I2C_CR1_PE                          ((uint16_t)0x0001)            /*!<Peripheral Enable                             */
#define  I2C_CR1_SMBUS                       ((uint16_t)0x0002)            /*!<SMBus Mode                                    */
#define  I2C_CR1_SMBTYPE                     ((uint16_t)0x0008)            /*!<SMBus Type                                    */
#define  I2C_CR1_ENARP                       ((uint16_t)0x0010)            /*!<ARP Enable                                    */
#define  I2C_CR1_ENPEC                       ((uint16_t)0x0020)            /*!<PEC Enable                                    */
#define  I2C_CR1_ENGC                        ((uint16_t)0x0040)            /*!<General Call Enable                           */
#define  I2C_CR1_NOSTRETCH                   ((uint16_t)0x0080)            /*!<Clock Stretching Disable (Slave mode)         */
#define  I2C_CR1_START                       ((uint16_t)0x0100)            /*!<Start Generation                              */
#define  I2C_CR1_STOP                        ((uint16_t)0x0200)            /*!<Stop Generation                               */
#define  I2C_CR1_ACK                         ((uint16_t)0x0400)            /*!<Acknowledge Enable                            */
#define  I2C_CR1_POS                         ((uint16_t)0x0800)            /*!<Acknowledge/PEC Position (for data reception) */
#define  I2C_CR1_PEC                         ((uint16_t)0x1000)            /*!<Packet Error Checking                         */
#define  I2C_CR1_ALERT                       ((uint16_t)0x2000)            /*!<SMBus Alert                                   */
#define  I2C_CR1_SWRST                       ((uint16_t)0x8000)            /*!<Software Reset                                */

/*******************  Bit definition for I2C_CR2 register  ********************/
#define  I2C_CR2_FREQ                        ((uint16_t)0x003F)            /*!<FREQ[5:0] bits (Peripheral Clock Frequency)   */
#define  I2C_CR2_FREQ_0                      ((uint16_t)0x0001)            /*!<Bit 0 */
#define  I2C_CR2_FREQ_1                      ((uint16_t)0x0002)            /*!<Bit 1 */
#define  I2C_CR2_FREQ_2                      ((uint16_t)0x0004)            /*!<Bit 2 */
#define  I2C_CR2_FREQ_3                      ((uint16_t)0x0008)            /*!<Bit 3 */
#define  I2C_CR2_FREQ_4                      ((uint16_t)0x0010)            /*!<Bit 4 */
#define  I2C_CR2_FREQ_5                      ((uint16_t)0x0020)            /*!<Bit 5 */

#define  I2C_CR2_ITERREN                     ((uint16_t)0x0100)            /*!<Error Interrupt Enable  */
#define  I2C_CR2_ITEVTEN                     ((uint16_t)0x0200)            /*!<Event Interrupt Enable  */
#define  I2C_CR2_ITBUFEN                     ((uint16_t)0x0400)            /*!<Buffer Interrupt Enable */
#define  I2C_CR2_DMAEN                       ((uint16_t)0x0800)            /*!<DMA Requests Enable     */
#define  I2C_CR2_LAST                        ((uint16_t)0x1000)            /*!<DMA Last Transfer       */

/*******************  Bit definition for I2C_OAR1 register  *******************/
#define  I2C_OAR1_ADD1_7                     ((uint16_t)0x00FE)            /*!<Interface Address */
#define  I2C_OAR1_ADD8_9                     ((uint16_t)0x0300)            /*!<Interface Address */

#define  I2C_OAR1_ADD0                       ((uint16_t)0x0001)            /*!<Bit 0 */
#define  I2C_OAR1_ADD1                       ((uint16_t)0x0002)            /*!<Bit 1 */
#define  I2C_OAR1_ADD2                       ((uint16_t)0x0004)            /*!<Bit 2 */
#define  I2C_OAR1_ADD3                       ((uint16_t)0x0008)            /*!<Bit 3 */
#define  I2C_OAR1_ADD4                       ((uint16_t)0x0010)            /*!<Bit 4 */
#define  I2C_OAR1_ADD5                       ((uint16_t)0x0020)            /*!<Bit 5 */
#define  I2C_OAR1_ADD6                       ((uint16_t)0x0040)            /*!<Bit 6 */
#define  I2C_OAR1_ADD7                       ((uint16_t)0x0080)            /*!<Bit 7 */
#define  I2C_OAR1_ADD8                       ((uint16_t)0x0100)            /*!<Bit 8 */
#define  I2C_OAR1_ADD9                       ((uint16_t)0x0200)            /*!<Bit 9 */

#define  I2C_OAR1_ADDMODE                    ((uint16_t)0x8000)            /*!<Addressing Mode (Slave mode) */

/*******************  Bit definition for I2C_OAR2 register  *******************/
#define  I2C_OAR2_ENDUAL                     ((uint8_t)0x01)               /*!<Dual addressing mode enable */
#define  I2C_OAR2_ADD2                       ((uint8_t)0xFE)               /*!<Interface address           */

/********************  Bit definition for I2C_DR register  ********************/
#define  I2C_DR_DR                           ((uint8_t)0xFF)               /*!<8-bit Data Register         */

/*******************  Bit definition for I2C_SR1 register  ********************/
#define  I2C_SR1_SB                          ((uint16_t)0x0001)            /*!<Start Bit (Master mode)                         */
#define  I2C_SR1_ADDR                        ((uint16_t)0x0002)            /*!<Address sent (master mode)/matched (slave mode) */
#define  I2C_SR1_BTF                         ((uint16_t)0x0004)            /*!<Byte Transfer Finished                          */
#define  I2C_SR1_ADD10                       ((uint16_t)0x0008)            /*!<10-bit header sent (Master mode)                */
#define  I2C_SR1_STOPF                       ((uint16_t)0x0010)            /*!<Stop detection (Slave mode)                     */
#define  I2C_SR1_RXNE                        ((uint16_t)0x0040)            /*!<Data Register not Empty (receivers)             */
#define  I2C_SR1_TXE                         ((uint16_t)0x0080)            /*!<Data Register Empty (transmitters)              */
#define  I2C_SR1_BERR                        ((uint16_t)0x0100)            /*!<Bus Error                                       */
#define  I2C_SR1_ARLO                        ((uint16_t)0x0200)            /*!<Arbitration Lost (master mode)                  */
#define  I2C_SR1_AF                          ((uint16_t)0x0400)            /*!<Acknowledge Failure                             */
#define  I2C_SR1_OVR                         ((uint16_t)0x0800)            /*!<Overrun/Underrun                                */
#define  I2C_SR1_PECERR                      ((uint16_t)0x1000)            /*!<PEC Error in reception                          */
#define  I2C_SR1_TIMEOUT                     ((uint16_t)0x4000)            /*!<Timeout or Tlow Error                           */
#define  I2C_SR1_SMBALERT                    ((uint16_t)0x8000)            /*!<SMBus Alert                                     */

/*******************  Bit definition for I2C_SR2 register  ********************/
#define  I2C_SR2_MSL                         ((uint16_t)0x0001)            /*!<Master/Slave                              */
#define  I2C_SR2_BUSY                        ((uint16_t)0x0002)            /*!<Bus Busy                                  */
#define  I2C_SR2_TRA                         ((uint16_t)0x0004)            /*!<Transmitter/Receiver                      */
#define  I2C_SR2_GENCALL                     ((uint16_t)0x0010)            /*!<General Call Address (Slave mode)         */
#define  I2C_SR2_SMBDEFAULT                  ((uint16_t)0x0020)            /*!<SMBus Device Default Address (Slave mode) */
#define  I2C_SR2_SMBHOST                     ((uint16_t)0x0040)            /*!<SMBus Host Header (Slave mode)            */
#define  I2C_SR2_DUALF                       ((uint16_t)0x0080)            /*!<Dual Flag (Slave mode)                    */
#define  I2C_SR2_PEC                         ((uint16_t)0xFF00)            /*!<Packet Error Checking Register            */

/*******************  Bit definition for I2C_CCR register  ********************/
#define  I2C_CCR_CCR                         ((uint16_t)0x0FFF)            /*!<Clock Control Register in Fast/Standard mode (Master mode) */
#define  I2C_CCR_DUTY                        ((uint16_t)0x4000)            /*!<Fast Mode Duty Cycle                                       */
#define  I2C_CCR_FS                          ((uint16_t)0x8000)            /*!<I2C Master Mode Selection                                  */

/******************  Bit definition for I2C_TRISE register  *******************/
#define  I2C_TRISE_TRISE                     ((uint8_t)0x3F)               /*!<Maximum Rise Time in Fast/Standard mode (Master mode) */

/******************  Bit definition for I2C_FLTR register  *******************/
#define  I2C_FLTR_DNF                     ((uint8_t)0x0F)                  /*!<Digital Noise Filter */
#define  I2C_FLTR_ANOFF                   ((uint8_t)0x10)                  /*!<Analog Noise Filter OFF */

/******************************************************************************/
/*                                                                            */
/*                           Independent WATCHDOG                             */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for IWDG_KR register  ********************/
#define  IWDG_KR_KEY                         ((uint16_t)0xFFFF)            /*!<Key value (write only, read 0000h)  */

/*******************  Bit definition for IWDG_PR register  ********************/
#define  IWDG_PR_PR                          ((uint8_t)0x07)               /*!<PR[2:0] (Prescaler divider)         */
#define  IWDG_PR_PR_0                        ((uint8_t)0x01)               /*!<Bit 0 */
#define  IWDG_PR_PR_1                        ((uint8_t)0x02)               /*!<Bit 1 */
#define  IWDG_PR_PR_2                        ((uint8_t)0x04)               /*!<Bit 2 */

/*******************  Bit definition for IWDG_RLR register  *******************/
#define  IWDG_RLR_RL                         ((uint16_t)0x0FFF)            /*!<Watchdog counter reload value        */

/*******************  Bit definition for IWDG_SR register  ********************/
#define  IWDG_SR_PVU                         ((uint8_t)0x01)               /*!<Watchdog prescaler value update      */
#define  IWDG_SR_RVU                         ((uint8_t)0x02)               /*!<Watchdog counter reload value update */

/******************************************************************************/
/*                                                                            */
/*                      LCD-TFT Display Controller (LTDC)                     */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for LTDC_SSCR register  *****************/

#define LTDC_SSCR_VSH                       ((uint32_t)0x000007FF)              /*!< Vertical Synchronization Height */
#define LTDC_SSCR_HSW                       ((uint32_t)0x0FFF0000)              /*!< Horizontal Synchronization Width */

/********************  Bit definition for LTDC_BPCR register  *****************/

#define LTDC_BPCR_AVBP                      ((uint32_t)0x000007FF)              /*!< Accumulated Vertical Back Porch */
#define LTDC_BPCR_AHBP                      ((uint32_t)0x0FFF0000)              /*!< Accumulated Horizontal Back Porch */

/********************  Bit definition for LTDC_AWCR register  *****************/

#define LTDC_AWCR_AAH                       ((uint32_t)0x000007FF)              /*!< Accumulated Active heigh */
#define LTDC_AWCR_AAW                       ((uint32_t)0x0FFF0000)              /*!< Accumulated Active Width */

/********************  Bit definition for LTDC_TWCR register  *****************/

#define LTDC_TWCR_TOTALH                    ((uint32_t)0x000007FF)              /*!< Total Heigh */
#define LTDC_TWCR_TOTALW                    ((uint32_t)0x0FFF0000)              /*!< Total Width */

/********************  Bit definition for LTDC_GCR register  ******************/

#define LTDC_GCR_LTDCEN                     ((uint32_t)0x00000001)              /*!< LCD-TFT controller enable bit */
#define LTDC_GCR_DBW                        ((uint32_t)0x00000070)              /*!< Dither Blue Width */
#define LTDC_GCR_DGW                        ((uint32_t)0x00000700)              /*!< Dither Green Width */
#define LTDC_GCR_DRW                        ((uint32_t)0x00007000)              /*!< Dither Red Width */
#define LTDC_GCR_DTEN                       ((uint32_t)0x00010000)              /*!< Dither Enable */
#define LTDC_GCR_PCPOL                      ((uint32_t)0x10000000)              /*!< Pixel Clock Polarity */
#define LTDC_GCR_DEPOL                      ((uint32_t)0x20000000)              /*!< Data Enable Polarity */
#define LTDC_GCR_VSPOL                      ((uint32_t)0x40000000)              /*!< Vertical Synchronization Polarity */
#define LTDC_GCR_HSPOL                      ((uint32_t)0x80000000)              /*!< Horizontal Synchronization Polarity */

/********************  Bit definition for LTDC_SRCR register  *****************/

#define LTDC_SRCR_IMR                      ((uint32_t)0x00000001)               /*!< Immediate Reload */
#define LTDC_SRCR_VBR                      ((uint32_t)0x00000002)               /*!< Vertical Blanking Reload */

/********************  Bit definition for LTDC_BCCR register  *****************/

#define LTDC_BCCR_BCBLUE                    ((uint32_t)0x000000FF)              /*!< Background Blue value */
#define LTDC_BCCR_BCGREEN                   ((uint32_t)0x0000FF00)              /*!< Background Green value */
#define LTDC_BCCR_BCRED                     ((uint32_t)0x00FF0000)              /*!< Background Red value */

/********************  Bit definition for LTDC_IER register  ******************/

#define LTDC_IER_LIE                        ((uint32_t)0x00000001)              /*!< Line Interrupt Enable */
#define LTDC_IER_FUIE                       ((uint32_t)0x00000002)              /*!< FIFO Underrun Interrupt Enable */
#define LTDC_IER_TERRIE                     ((uint32_t)0x00000004)              /*!< Transfer Error Interrupt Enable */
#define LTDC_IER_RRIE                       ((uint32_t)0x00000008)              /*!< Register Reload interrupt enable */

/********************  Bit definition for LTDC_ISR register  ******************/

#define LTDC_ISR_LIF                        ((uint32_t)0x00000001)              /*!< Line Interrupt Flag */
#define LTDC_ISR_FUIF                       ((uint32_t)0x00000002)              /*!< FIFO Underrun Interrupt Flag */
#define LTDC_ISR_TERRIF                     ((uint32_t)0x00000004)              /*!< Transfer Error Interrupt Flag */
#define LTDC_ISR_RRIF                       ((uint32_t)0x00000008)              /*!< Register Reload interrupt Flag */

/********************  Bit definition for LTDC_ICR register  ******************/

#define LTDC_ICR_CLIF                       ((uint32_t)0x00000001)              /*!< Clears the Line Interrupt Flag */
#define LTDC_ICR_CFUIF                      ((uint32_t)0x00000002)              /*!< Clears the FIFO Underrun Interrupt Flag */
#define LTDC_ICR_CTERRIF                    ((uint32_t)0x00000004)              /*!< Clears the Transfer Error Interrupt Flag */
#define LTDC_ICR_CRRIF                      ((uint32_t)0x00000008)              /*!< Clears Register Reload interrupt Flag */

/********************  Bit definition for LTDC_LIPCR register  ****************/

#define LTDC_LIPCR_LIPOS                    ((uint32_t)0x000007FF)              /*!< Line Interrupt Position */

/********************  Bit definition for LTDC_CPSR register  *****************/

#define LTDC_CPSR_CYPOS                     ((uint32_t)0x0000FFFF)              /*!< Current Y Position */
#define LTDC_CPSR_CXPOS                     ((uint32_t)0xFFFF0000)              /*!< Current X Position */

/********************  Bit definition for LTDC_CDSR register  *****************/

#define LTDC_CDSR_VDES                      ((uint32_t)0x00000001)              /*!< Vertical Data Enable Status */
#define LTDC_CDSR_HDES                      ((uint32_t)0x00000002)              /*!< Horizontal Data Enable Status */
#define LTDC_CDSR_VSYNCS                    ((uint32_t)0x00000004)              /*!< Vertical Synchronization Status */
#define LTDC_CDSR_HSYNCS                    ((uint32_t)0x00000008)              /*!< Horizontal Synchronization Status */

/********************  Bit definition for LTDC_LxCR register  *****************/

#define LTDC_LxCR_LEN                       ((uint32_t)0x00000001)              /*!< Layer Enable */
#define LTDC_LxCR_COLKEN                    ((uint32_t)0x00000002)              /*!< Color Keying Enable */
#define LTDC_LxCR_CLUTEN                    ((uint32_t)0x00000010)              /*!< Color Lockup Table Enable */

/********************  Bit definition for LTDC_LxWHPCR register  **************/

#define LTDC_LxWHPCR_WHSTPOS                ((uint32_t)0x00000FFF)              /*!< Window Horizontal Start Position */
#define LTDC_LxWHPCR_WHSPPOS                ((uint32_t)0xFFFF0000)              /*!< Window Horizontal Stop Position */

/********************  Bit definition for LTDC_LxWVPCR register  **************/

#define LTDC_LxWVPCR_WVSTPOS                ((uint32_t)0x00000FFF)              /*!< Window Vertical Start Position */
#define LTDC_LxWVPCR_WVSPPOS                ((uint32_t)0xFFFF0000)              /*!< Window Vertical Stop Position */

/********************  Bit definition for LTDC_LxCKCR register  ***************/

#define LTDC_LxCKCR_CKBLUE                  ((uint32_t)0x000000FF)              /*!< Color Key Blue value */
#define LTDC_LxCKCR_CKGREEN                 ((uint32_t)0x0000FF00)              /*!< Color Key Green value */
#define LTDC_LxCKCR_CKRED                   ((uint32_t)0x00FF0000)              /*!< Color Key Red value */

/********************  Bit definition for LTDC_LxPFCR register  ***************/

#define LTDC_LxPFCR_PF                      ((uint32_t)0x00000007)              /*!< Pixel Format */

/********************  Bit definition for LTDC_LxCACR register  ***************/

#define LTDC_LxCACR_CONSTA                  ((uint32_t)0x000000FF)              /*!< Constant Alpha */

/********************  Bit definition for LTDC_LxDCCR register  ***************/

#define LTDC_LxDCCR_DCBLUE                  ((uint32_t)0x000000FF)              /*!< Default Color Blue */
#define LTDC_LxDCCR_DCGREEN                 ((uint32_t)0x0000FF00)              /*!< Default Color Green */
#define LTDC_LxDCCR_DCRED                   ((uint32_t)0x00FF0000)              /*!< Default Color Red */
#define LTDC_LxDCCR_DCALPHA                 ((uint32_t)0xFF000000)              /*!< Default Color Alpha */
                                
/********************  Bit definition for LTDC_LxBFCR register  ***************/

#define LTDC_LxBFCR_BF2                     ((uint32_t)0x00000007)              /*!< Blending Factor 2 */
#define LTDC_LxBFCR_BF1                     ((uint32_t)0x00000700)              /*!< Blending Factor 1 */

/********************  Bit definition for LTDC_LxCFBAR register  **************/

#define LTDC_LxCFBAR_CFBADD                 ((uint32_t)0xFFFFFFFF)              /*!< Color Frame Buffer Start Address */

/********************  Bit definition for LTDC_LxCFBLR register  **************/

#define LTDC_LxCFBLR_CFBLL                  ((uint32_t)0x00001FFF)              /*!< Color Frame Buffer Line Length */
#define LTDC_LxCFBLR_CFBP                   ((uint32_t)0x1FFF0000)              /*!< Color Frame Buffer Pitch in bytes */

/********************  Bit definition for LTDC_LxCFBLNR register  *************/

#define LTDC_LxCFBLNR_CFBLNBR               ((uint32_t)0x000007FF)              /*!< Frame Buffer Line Number */

/********************  Bit definition for LTDC_LxCLUTWR register  *************/

#define LTDC_LxCLUTWR_BLUE                  ((uint32_t)0x000000FF)              /*!< Blue value */
#define LTDC_LxCLUTWR_GREEN                 ((uint32_t)0x0000FF00)              /*!< Green value */
#define LTDC_LxCLUTWR_RED                   ((uint32_t)0x00FF0000)              /*!< Red value */
#define LTDC_LxCLUTWR_CLUTADD               ((uint32_t)0xFF000000)              /*!< CLUT address */

/******************************************************************************/
/*                                                                            */
/*                             Power Control                                  */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for PWR_CR register  ********************/
#define  PWR_CR_LPDS                         ((uint32_t)0x00000001)     /*!< Low-Power Deepsleep                 */
#define  PWR_CR_PDDS                         ((uint32_t)0x00000002)     /*!< Power Down Deepsleep                */
#define  PWR_CR_CWUF                         ((uint32_t)0x00000004)     /*!< Clear Wakeup Flag                   */
#define  PWR_CR_CSBF                         ((uint32_t)0x00000008)     /*!< Clear Standby Flag                  */
#define  PWR_CR_PVDE                         ((uint32_t)0x00000010)     /*!< Power Voltage Detector Enable       */

#define  PWR_CR_PLS                          ((uint32_t)0x000000E0)     /*!< PLS[2:0] bits (PVD Level Selection) */
#define  PWR_CR_PLS_0                        ((uint32_t)0x00000020)     /*!< Bit 0 */
#define  PWR_CR_PLS_1                        ((uint32_t)0x00000040)     /*!< Bit 1 */
#define  PWR_CR_PLS_2                        ((uint32_t)0x00000080)     /*!< Bit 2 */

/*!< PVD level configuration */
#define  PWR_CR_PLS_LEV0                     ((uint32_t)0x00000000)     /*!< PVD level 0 */
#define  PWR_CR_PLS_LEV1                     ((uint32_t)0x00000020)     /*!< PVD level 1 */
#define  PWR_CR_PLS_LEV2                     ((uint32_t)0x00000040)     /*!< PVD level 2 */
#define  PWR_CR_PLS_LEV3                     ((uint32_t)0x00000060)     /*!< PVD level 3 */
#define  PWR_CR_PLS_LEV4                     ((uint32_t)0x00000080)     /*!< PVD level 4 */
#define  PWR_CR_PLS_LEV5                     ((uint32_t)0x000000A0)     /*!< PVD level 5 */
#define  PWR_CR_PLS_LEV6                     ((uint32_t)0x000000C0)     /*!< PVD level 6 */
#define  PWR_CR_PLS_LEV7                     ((uint32_t)0x000000E0)     /*!< PVD level 7 */

#define  PWR_CR_DBP                          ((uint32_t)0x00000100)     /*!< Disable Backup Domain write protection                     */
#define  PWR_CR_FPDS                         ((uint32_t)0x00000200)     /*!< Flash power down in Stop mode                              */
#define  PWR_CR_LPUDS                        ((uint32_t)0x00000400)     /*!< Low-Power Regulator in Stop under-drive mode               */
#define  PWR_CR_MRUDS                        ((uint32_t)0x00000800)     /*!< Main regulator in Stop under-drive mode                    */
#define  PWR_CR_LPLVDS                       ((uint32_t)0x00000400)     /*!< Low-power regulator Low Voltage in Deep Sleep mode         */
#define  PWR_CR_MRLVDS                       ((uint32_t)0x00000800)     /*!< Main regulator Low Voltage in Deep Sleep mode              */

#define  PWR_CR_ADCDC1                       ((uint32_t)0x00002000)     /*!< Refer to AN4073 on how to use this bit */ 

#define  PWR_CR_VOS                          ((uint32_t)0x0000C000)     /*!< VOS[1:0] bits (Regulator voltage scaling output selection) */
#define  PWR_CR_VOS_0                        ((uint32_t)0x00004000)     /*!< Bit 0 */
#define  PWR_CR_VOS_1                        ((uint32_t)0x00008000)     /*!< Bit 1 */

#define  PWR_CR_ODEN                         ((uint32_t)0x00010000)     /*!< Over Drive enable                   */
#define  PWR_CR_ODSWEN                       ((uint32_t)0x00020000)     /*!< Over Drive switch enabled           */
#define  PWR_CR_UDEN                         ((uint32_t)0x000C0000)     /*!< Under Drive enable in stop mode     */
#define  PWR_CR_UDEN_0                       ((uint32_t)0x00040000)     /*!< Bit 0                               */
#define  PWR_CR_UDEN_1                       ((uint32_t)0x00080000)     /*!< Bit 1                               */

#define  PWR_CR_FMSSR                        ((uint32_t)0x00100000)     /*!< Flash Memory Sleep System Run        */
#define  PWR_CR_FISSR                        ((uint32_t)0x00200000)     /*!< Flash Interface Stop while System Run */

/* Legacy define */
#define  PWR_CR_PMODE                        PWR_CR_VOS

/*******************  Bit definition for PWR_CSR register  ********************/
#define  PWR_CSR_WUF                         ((uint32_t)0x00000001)     /*!< Wakeup Flag                                      */
#define  PWR_CSR_SBF                         ((uint32_t)0x00000002)     /*!< Standby Flag                                     */
#define  PWR_CSR_PVDO                        ((uint32_t)0x00000004)     /*!< PVD Output                                       */
#define  PWR_CSR_BRR                         ((uint32_t)0x00000008)     /*!< Backup regulator ready                           */
#define  PWR_CSR_EWUP                        ((uint32_t)0x00000100)     /*!< Enable WKUP pin                                  */
#define  PWR_CSR_BRE                         ((uint32_t)0x00000200)     /*!< Backup regulator enable                          */
#define  PWR_CSR_VOSRDY                      ((uint32_t)0x00004000)     /*!< Regulator voltage scaling output selection ready */
#define  PWR_CSR_ODRDY                       ((uint32_t)0x00010000)     /*!< Over Drive generator ready                       */
#define  PWR_CSR_ODSWRDY                     ((uint32_t)0x00020000)     /*!< Over Drive Switch ready                          */
#define  PWR_CSR_UDSWRDY                     ((uint32_t)0x000C0000)     /*!< Under Drive ready                                */

/* Legacy define */
#define  PWR_CSR_REGRDY                      PWR_CSR_VOSRDY

/******************************************************************************/
/*                                                                            */
/*                         Reset and Clock Control                            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for RCC_CR register  ********************/
#define  RCC_CR_HSION                        ((uint32_t)0x00000001)
#define  RCC_CR_HSIRDY                       ((uint32_t)0x00000002)

#define  RCC_CR_HSITRIM                      ((uint32_t)0x000000F8)
#define  RCC_CR_HSITRIM_0                    ((uint32_t)0x00000008)/*!<Bit 0 */
#define  RCC_CR_HSITRIM_1                    ((uint32_t)0x00000010)/*!<Bit 1 */
#define  RCC_CR_HSITRIM_2                    ((uint32_t)0x00000020)/*!<Bit 2 */
#define  RCC_CR_HSITRIM_3                    ((uint32_t)0x00000040)/*!<Bit 3 */
#define  RCC_CR_HSITRIM_4                    ((uint32_t)0x00000080)/*!<Bit 4 */

#define  RCC_CR_HSICAL                       ((uint32_t)0x0000FF00)
#define  RCC_CR_HSICAL_0                     ((uint32_t)0x00000100)/*!<Bit 0 */
#define  RCC_CR_HSICAL_1                     ((uint32_t)0x00000200)/*!<Bit 1 */
#define  RCC_CR_HSICAL_2                     ((uint32_t)0x00000400)/*!<Bit 2 */
#define  RCC_CR_HSICAL_3                     ((uint32_t)0x00000800)/*!<Bit 3 */
#define  RCC_CR_HSICAL_4                     ((uint32_t)0x00001000)/*!<Bit 4 */
#define  RCC_CR_HSICAL_5                     ((uint32_t)0x00002000)/*!<Bit 5 */
#define  RCC_CR_HSICAL_6                     ((uint32_t)0x00004000)/*!<Bit 6 */
#define  RCC_CR_HSICAL_7                     ((uint32_t)0x00008000)/*!<Bit 7 */

#define  RCC_CR_HSEON                        ((uint32_t)0x00010000)
#define  RCC_CR_HSERDY                       ((uint32_t)0x00020000)
#define  RCC_CR_HSEBYP                       ((uint32_t)0x00040000)
#define  RCC_CR_CSSON                        ((uint32_t)0x00080000)
#define  RCC_CR_PLLON                        ((uint32_t)0x01000000)
#define  RCC_CR_PLLRDY                       ((uint32_t)0x02000000)
#define  RCC_CR_PLLI2SON                     ((uint32_t)0x04000000)
#define  RCC_CR_PLLI2SRDY                    ((uint32_t)0x08000000)
#define  RCC_CR_PLLSAION                     ((uint32_t)0x10000000)
#define  RCC_CR_PLLSAIRDY                    ((uint32_t)0x20000000)

/********************  Bit definition for RCC_PLLCFGR register  ***************/
#define  RCC_PLLCFGR_PLLM                    ((uint32_t)0x0000003F)
#define  RCC_PLLCFGR_PLLM_0                  ((uint32_t)0x00000001)
#define  RCC_PLLCFGR_PLLM_1                  ((uint32_t)0x00000002)
#define  RCC_PLLCFGR_PLLM_2                  ((uint32_t)0x00000004)
#define  RCC_PLLCFGR_PLLM_3                  ((uint32_t)0x00000008)
#define  RCC_PLLCFGR_PLLM_4                  ((uint32_t)0x00000010)
#define  RCC_PLLCFGR_PLLM_5                  ((uint32_t)0x00000020)

#define  RCC_PLLCFGR_PLLN                     ((uint32_t)0x00007FC0)
#define  RCC_PLLCFGR_PLLN_0                   ((uint32_t)0x00000040)
#define  RCC_PLLCFGR_PLLN_1                   ((uint32_t)0x00000080)
#define  RCC_PLLCFGR_PLLN_2                   ((uint32_t)0x00000100)
#define  RCC_PLLCFGR_PLLN_3                   ((uint32_t)0x00000200)
#define  RCC_PLLCFGR_PLLN_4                   ((uint32_t)0x00000400)
#define  RCC_PLLCFGR_PLLN_5                   ((uint32_t)0x00000800)
#define  RCC_PLLCFGR_PLLN_6                   ((uint32_t)0x00001000)
#define  RCC_PLLCFGR_PLLN_7                   ((uint32_t)0x00002000)
#define  RCC_PLLCFGR_PLLN_8                   ((uint32_t)0x00004000)

#define  RCC_PLLCFGR_PLLP                    ((uint32_t)0x00030000)
#define  RCC_PLLCFGR_PLLP_0                  ((uint32_t)0x00010000)
#define  RCC_PLLCFGR_PLLP_1                  ((uint32_t)0x00020000)

#define  RCC_PLLCFGR_PLLSRC                  ((uint32_t)0x00400000)
#define  RCC_PLLCFGR_PLLSRC_HSE              ((uint32_t)0x00400000)
#define  RCC_PLLCFGR_PLLSRC_HSI              ((uint32_t)0x00000000)

#define  RCC_PLLCFGR_PLLQ                    ((uint32_t)0x0F000000)
#define  RCC_PLLCFGR_PLLQ_0                  ((uint32_t)0x01000000)
#define  RCC_PLLCFGR_PLLQ_1                  ((uint32_t)0x02000000)
#define  RCC_PLLCFGR_PLLQ_2                  ((uint32_t)0x04000000)
#define  RCC_PLLCFGR_PLLQ_3                  ((uint32_t)0x08000000)

/********************  Bit definition for RCC_CFGR register  ******************/
/*!< SW configuration */
#define  RCC_CFGR_SW                         ((uint32_t)0x00000003)        /*!< SW[1:0] bits (System clock Switch) */
#define  RCC_CFGR_SW_0                       ((uint32_t)0x00000001)        /*!< Bit 0 */
#define  RCC_CFGR_SW_1                       ((uint32_t)0x00000002)        /*!< Bit 1 */

#define  RCC_CFGR_SW_HSI                     ((uint32_t)0x00000000)        /*!< HSI selected as system clock */
#define  RCC_CFGR_SW_HSE                     ((uint32_t)0x00000001)        /*!< HSE selected as system clock */
#define  RCC_CFGR_SW_PLL                     ((uint32_t)0x00000002)        /*!< PLL selected as system clock */

/*!< SWS configuration */
#define  RCC_CFGR_SWS                        ((uint32_t)0x0000000C)        /*!< SWS[1:0] bits (System Clock Switch Status) */
#define  RCC_CFGR_SWS_0                      ((uint32_t)0x00000004)        /*!< Bit 0 */
#define  RCC_CFGR_SWS_1                      ((uint32_t)0x00000008)        /*!< Bit 1 */

#define  RCC_CFGR_SWS_HSI                    ((uint32_t)0x00000000)        /*!< HSI oscillator used as system clock */
#define  RCC_CFGR_SWS_HSE                    ((uint32_t)0x00000004)        /*!< HSE oscillator used as system clock */
#define  RCC_CFGR_SWS_PLL                    ((uint32_t)0x00000008)        /*!< PLL used as system clock */

/*!< HPRE configuration */
#define  RCC_CFGR_HPRE                       ((uint32_t)0x000000F0)        /*!< HPRE[3:0] bits (AHB prescaler) */
#define  RCC_CFGR_HPRE_0                     ((uint32_t)0x00000010)        /*!< Bit 0 */
#define  RCC_CFGR_HPRE_1                     ((uint32_t)0x00000020)        /*!< Bit 1 */
#define  RCC_CFGR_HPRE_2                     ((uint32_t)0x00000040)        /*!< Bit 2 */
#define  RCC_CFGR_HPRE_3                     ((uint32_t)0x00000080)        /*!< Bit 3 */

#define  RCC_CFGR_HPRE_DIV1                  ((uint32_t)0x00000000)        /*!< SYSCLK not divided */
#define  RCC_CFGR_HPRE_DIV2                  ((uint32_t)0x00000080)        /*!< SYSCLK divided by 2 */
#define  RCC_CFGR_HPRE_DIV4                  ((uint32_t)0x00000090)        /*!< SYSCLK divided by 4 */
#define  RCC_CFGR_HPRE_DIV8                  ((uint32_t)0x000000A0)        /*!< SYSCLK divided by 8 */
#define  RCC_CFGR_HPRE_DIV16                 ((uint32_t)0x000000B0)        /*!< SYSCLK divided by 16 */
#define  RCC_CFGR_HPRE_DIV64                 ((uint32_t)0x000000C0)        /*!< SYSCLK divided by 64 */
#define  RCC_CFGR_HPRE_DIV128                ((uint32_t)0x000000D0)        /*!< SYSCLK divided by 128 */
#define  RCC_CFGR_HPRE_DIV256                ((uint32_t)0x000000E0)        /*!< SYSCLK divided by 256 */
#define  RCC_CFGR_HPRE_DIV512                ((uint32_t)0x000000F0)        /*!< SYSCLK divided by 512 */

/*!< PPRE1 configuration */
#define  RCC_CFGR_PPRE1                      ((uint32_t)0x00001C00)        /*!< PRE1[2:0] bits (APB1 prescaler) */
#define  RCC_CFGR_PPRE1_0                    ((uint32_t)0x00000400)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE1_1                    ((uint32_t)0x00000800)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE1_2                    ((uint32_t)0x00001000)        /*!< Bit 2 */

#define  RCC_CFGR_PPRE1_DIV1                 ((uint32_t)0x00000000)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE1_DIV2                 ((uint32_t)0x00001000)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE1_DIV4                 ((uint32_t)0x00001400)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE1_DIV8                 ((uint32_t)0x00001800)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE1_DIV16                ((uint32_t)0x00001C00)        /*!< HCLK divided by 16 */

/*!< PPRE2 configuration */
#define  RCC_CFGR_PPRE2                      ((uint32_t)0x0000E000)        /*!< PRE2[2:0] bits (APB2 prescaler) */
#define  RCC_CFGR_PPRE2_0                    ((uint32_t)0x00002000)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE2_1                    ((uint32_t)0x00004000)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE2_2                    ((uint32_t)0x00008000)        /*!< Bit 2 */

#define  RCC_CFGR_PPRE2_DIV1                 ((uint32_t)0x00000000)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE2_DIV2                 ((uint32_t)0x00008000)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE2_DIV4                 ((uint32_t)0x0000A000)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE2_DIV8                 ((uint32_t)0x0000C000)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE2_DIV16                ((uint32_t)0x0000E000)        /*!< HCLK divided by 16 */

/*!< RTCPRE configuration */
#define  RCC_CFGR_RTCPRE                     ((uint32_t)0x001F0000)
#define  RCC_CFGR_RTCPRE_0                   ((uint32_t)0x00010000)
#define  RCC_CFGR_RTCPRE_1                   ((uint32_t)0x00020000)
#define  RCC_CFGR_RTCPRE_2                   ((uint32_t)0x00040000)
#define  RCC_CFGR_RTCPRE_3                   ((uint32_t)0x00080000)
#define  RCC_CFGR_RTCPRE_4                   ((uint32_t)0x00100000)

/*!< MCO1 configuration */
#define  RCC_CFGR_MCO1                       ((uint32_t)0x00600000)
#define  RCC_CFGR_MCO1_0                     ((uint32_t)0x00200000)
#define  RCC_CFGR_MCO1_1                     ((uint32_t)0x00400000)

#define  RCC_CFGR_I2SSRC                     ((uint32_t)0x00800000)

#define  RCC_CFGR_MCO1PRE                    ((uint32_t)0x07000000)
#define  RCC_CFGR_MCO1PRE_0                  ((uint32_t)0x01000000)
#define  RCC_CFGR_MCO1PRE_1                  ((uint32_t)0x02000000)
#define  RCC_CFGR_MCO1PRE_2                  ((uint32_t)0x04000000)

#define  RCC_CFGR_MCO2PRE                    ((uint32_t)0x38000000)
#define  RCC_CFGR_MCO2PRE_0                  ((uint32_t)0x08000000)
#define  RCC_CFGR_MCO2PRE_1                  ((uint32_t)0x10000000)
#define  RCC_CFGR_MCO2PRE_2                  ((uint32_t)0x20000000)

#define  RCC_CFGR_MCO2                       ((uint32_t)0xC0000000)
#define  RCC_CFGR_MCO2_0                     ((uint32_t)0x40000000)
#define  RCC_CFGR_MCO2_1                     ((uint32_t)0x80000000)

/********************  Bit definition for RCC_CIR register  *******************/
#define  RCC_CIR_LSIRDYF                     ((uint32_t)0x00000001)
#define  RCC_CIR_LSERDYF                     ((uint32_t)0x00000002)
#define  RCC_CIR_HSIRDYF                     ((uint32_t)0x00000004)
#define  RCC_CIR_HSERDYF                     ((uint32_t)0x00000008)
#define  RCC_CIR_PLLRDYF                     ((uint32_t)0x00000010)
#define  RCC_CIR_PLLI2SRDYF                  ((uint32_t)0x00000020)
#define  RCC_CIR_PLLSAIRDYF                  ((uint32_t)0x00000040)
#define  RCC_CIR_CSSF                        ((uint32_t)0x00000080)
#define  RCC_CIR_LSIRDYIE                    ((uint32_t)0x00000100)
#define  RCC_CIR_LSERDYIE                    ((uint32_t)0x00000200)
#define  RCC_CIR_HSIRDYIE                    ((uint32_t)0x00000400)
#define  RCC_CIR_HSERDYIE                    ((uint32_t)0x00000800)
#define  RCC_CIR_PLLRDYIE                    ((uint32_t)0x00001000)
#define  RCC_CIR_PLLI2SRDYIE                 ((uint32_t)0x00002000)
#define  RCC_CIR_PLLSAIRDYIE                 ((uint32_t)0x00004000)
#define  RCC_CIR_LSIRDYC                     ((uint32_t)0x00010000)
#define  RCC_CIR_LSERDYC                     ((uint32_t)0x00020000)
#define  RCC_CIR_HSIRDYC                     ((uint32_t)0x00040000)
#define  RCC_CIR_HSERDYC                     ((uint32_t)0x00080000)
#define  RCC_CIR_PLLRDYC                     ((uint32_t)0x00100000)
#define  RCC_CIR_PLLI2SRDYC                  ((uint32_t)0x00200000)
#define  RCC_CIR_PLLSAIRDYC                  ((uint32_t)0x00400000)
#define  RCC_CIR_CSSC                        ((uint32_t)0x00800000)

/********************  Bit definition for RCC_AHB1RSTR register  **************/
#define  RCC_AHB1RSTR_GPIOARST               ((uint32_t)0x00000001)
#define  RCC_AHB1RSTR_GPIOBRST               ((uint32_t)0x00000002)
#define  RCC_AHB1RSTR_GPIOCRST               ((uint32_t)0x00000004)
#define  RCC_AHB1RSTR_GPIODRST               ((uint32_t)0x00000008)
#define  RCC_AHB1RSTR_GPIOERST               ((uint32_t)0x00000010)
#define  RCC_AHB1RSTR_GPIOFRST               ((uint32_t)0x00000020)
#define  RCC_AHB1RSTR_GPIOGRST               ((uint32_t)0x00000040)
#define  RCC_AHB1RSTR_GPIOHRST               ((uint32_t)0x00000080)
#define  RCC_AHB1RSTR_GPIOIRST               ((uint32_t)0x00000100)
#define  RCC_AHB1RSTR_GPIOJRST               ((uint32_t)0x00000200)
#define  RCC_AHB1RSTR_GPIOKRST               ((uint32_t)0x00000400)
#define  RCC_AHB1RSTR_CRCRST                 ((uint32_t)0x00001000)
#define  RCC_AHB1RSTR_DMA1RST                ((uint32_t)0x00200000)
#define  RCC_AHB1RSTR_DMA2RST                ((uint32_t)0x00400000)
#define  RCC_AHB1RSTR_DMA2DRST               ((uint32_t)0x00800000)
#define  RCC_AHB1RSTR_ETHMACRST              ((uint32_t)0x02000000)
#define  RCC_AHB1RSTR_OTGHRST                ((uint32_t)0x10000000)

/********************  Bit definition for RCC_AHB2RSTR register  **************/
#define  RCC_AHB2RSTR_DCMIRST                ((uint32_t)0x00000001)
#define  RCC_AHB2RSTR_CRYPRST                ((uint32_t)0x00000010)
#define  RCC_AHB2RSTR_HASHRST                ((uint32_t)0x00000020)
 /* maintained for legacy purpose */
 #define  RCC_AHB2RSTR_HSAHRST                RCC_AHB2RSTR_HASHRST
#define  RCC_AHB2RSTR_RNGRST                 ((uint32_t)0x00000040)
#define  RCC_AHB2RSTR_OTGFSRST               ((uint32_t)0x00000080)

/********************  Bit definition for RCC_AHB3RSTR register  **************/
#if defined(STM32F40_41xxx)
#define  RCC_AHB3RSTR_FSMCRST                ((uint32_t)0x00000001)
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define  RCC_AHB3RSTR_FMCRST                ((uint32_t)0x00000001)
#endif /* STM32F427_437xx ||  STM32F429_439xx */
/********************  Bit definition for RCC_APB1RSTR register  **************/
#define  RCC_APB1RSTR_TIM2RST                ((uint32_t)0x00000001)
#define  RCC_APB1RSTR_TIM3RST                ((uint32_t)0x00000002)
#define  RCC_APB1RSTR_TIM4RST                ((uint32_t)0x00000004)
#define  RCC_APB1RSTR_TIM5RST                ((uint32_t)0x00000008)
#define  RCC_APB1RSTR_TIM6RST                ((uint32_t)0x00000010)
#define  RCC_APB1RSTR_TIM7RST                ((uint32_t)0x00000020)
#define  RCC_APB1RSTR_TIM12RST               ((uint32_t)0x00000040)
#define  RCC_APB1RSTR_TIM13RST               ((uint32_t)0x00000080)
#define  RCC_APB1RSTR_TIM14RST               ((uint32_t)0x00000100)
#define  RCC_APB1RSTR_WWDGRST                ((uint32_t)0x00000800)
#define  RCC_APB1RSTR_SPI2RST                ((uint32_t)0x00004000)
#define  RCC_APB1RSTR_SPI3RST                ((uint32_t)0x00008000)
#define  RCC_APB1RSTR_USART2RST              ((uint32_t)0x00020000)
#define  RCC_APB1RSTR_USART3RST              ((uint32_t)0x00040000)
#define  RCC_APB1RSTR_UART4RST               ((uint32_t)0x00080000)
#define  RCC_APB1RSTR_UART5RST               ((uint32_t)0x00100000)
#define  RCC_APB1RSTR_I2C1RST                ((uint32_t)0x00200000)
#define  RCC_APB1RSTR_I2C2RST                ((uint32_t)0x00400000)
#define  RCC_APB1RSTR_I2C3RST                ((uint32_t)0x00800000)
#define  RCC_APB1RSTR_CAN1RST                ((uint32_t)0x02000000)
#define  RCC_APB1RSTR_CAN2RST                ((uint32_t)0x04000000)
#define  RCC_APB1RSTR_PWRRST                 ((uint32_t)0x10000000)
#define  RCC_APB1RSTR_DACRST                 ((uint32_t)0x20000000)
#define  RCC_APB1RSTR_UART7RST               ((uint32_t)0x40000000)
#define  RCC_APB1RSTR_UART8RST               ((uint32_t)0x80000000)

/********************  Bit definition for RCC_APB2RSTR register  **************/
#define  RCC_APB2RSTR_TIM1RST                ((uint32_t)0x00000001)
#define  RCC_APB2RSTR_TIM8RST                ((uint32_t)0x00000002)
#define  RCC_APB2RSTR_USART1RST              ((uint32_t)0x00000010)
#define  RCC_APB2RSTR_USART6RST              ((uint32_t)0x00000020)
#define  RCC_APB2RSTR_ADCRST                 ((uint32_t)0x00000100)
#define  RCC_APB2RSTR_SDIORST                ((uint32_t)0x00000800)
#define  RCC_APB2RSTR_SPI1RST                ((uint32_t)0x00001000)
#define  RCC_APB2RSTR_SPI4RST                ((uint32_t)0x00002000)
#define  RCC_APB2RSTR_SYSCFGRST              ((uint32_t)0x00004000)
#define  RCC_APB2RSTR_TIM9RST                ((uint32_t)0x00010000)
#define  RCC_APB2RSTR_TIM10RST               ((uint32_t)0x00020000)
#define  RCC_APB2RSTR_TIM11RST               ((uint32_t)0x00040000)
#define  RCC_APB2RSTR_SPI5RST                ((uint32_t)0x00100000)
#define  RCC_APB2RSTR_SPI6RST                ((uint32_t)0x00200000)
#define  RCC_APB2RSTR_SAI1RST                ((uint32_t)0x00400000)
#define  RCC_APB2RSTR_LTDCRST                ((uint32_t)0x04000000)

/* Old SPI1RST bit definition, maintained for legacy purpose */
#define  RCC_APB2RSTR_SPI1                   RCC_APB2RSTR_SPI1RST

/********************  Bit definition for RCC_AHB1ENR register  ***************/
#define  RCC_AHB1ENR_GPIOAEN                 ((uint32_t)0x00000001)
#define  RCC_AHB1ENR_GPIOBEN                 ((uint32_t)0x00000002)
#define  RCC_AHB1ENR_GPIOCEN                 ((uint32_t)0x00000004)
#define  RCC_AHB1ENR_GPIODEN                 ((uint32_t)0x00000008)
#define  RCC_AHB1ENR_GPIOEEN                 ((uint32_t)0x00000010)
#define  RCC_AHB1ENR_GPIOFEN                 ((uint32_t)0x00000020)
#define  RCC_AHB1ENR_GPIOGEN                 ((uint32_t)0x00000040)
#define  RCC_AHB1ENR_GPIOHEN                 ((uint32_t)0x00000080)
#define  RCC_AHB1ENR_GPIOIEN                 ((uint32_t)0x00000100)
#define  RCC_AHB1ENR_GPIOJEN                 ((uint32_t)0x00000200)
#define  RCC_AHB1ENR_GPIOKEN                 ((uint32_t)0x00000400)
#define  RCC_AHB1ENR_CRCEN                   ((uint32_t)0x00001000)
#define  RCC_AHB1ENR_BKPSRAMEN               ((uint32_t)0x00040000)
#define  RCC_AHB1ENR_CCMDATARAMEN            ((uint32_t)0x00100000)
#define  RCC_AHB1ENR_DMA1EN                  ((uint32_t)0x00200000)
#define  RCC_AHB1ENR_DMA2EN                  ((uint32_t)0x00400000)
#define  RCC_AHB1ENR_DMA2DEN                 ((uint32_t)0x00800000)
#define  RCC_AHB1ENR_ETHMACEN                ((uint32_t)0x02000000)
#define  RCC_AHB1ENR_ETHMACTXEN              ((uint32_t)0x04000000)
#define  RCC_AHB1ENR_ETHMACRXEN              ((uint32_t)0x08000000)
#define  RCC_AHB1ENR_ETHMACPTPEN             ((uint32_t)0x10000000)
#define  RCC_AHB1ENR_OTGHSEN                 ((uint32_t)0x20000000)
#define  RCC_AHB1ENR_OTGHSULPIEN             ((uint32_t)0x40000000)

/********************  Bit definition for RCC_AHB2ENR register  ***************/
#define  RCC_AHB2ENR_DCMIEN                  ((uint32_t)0x00000001)
#define  RCC_AHB2ENR_CRYPEN                  ((uint32_t)0x00000010)
#define  RCC_AHB2ENR_HASHEN                  ((uint32_t)0x00000020)
#define  RCC_AHB2ENR_RNGEN                   ((uint32_t)0x00000040)
#define  RCC_AHB2ENR_OTGFSEN                 ((uint32_t)0x00000080)

/********************  Bit definition for RCC_AHB3ENR register  ***************/

#if defined(STM32F40_41xxx)
#define  RCC_AHB3ENR_FSMCEN                  ((uint32_t)0x00000001)
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define  RCC_AHB3ENR_FMCEN                  ((uint32_t)0x00000001)
#endif /* STM32F427_437xx ||  STM32F429_439xx */

/********************  Bit definition for RCC_APB1ENR register  ***************/
#define  RCC_APB1ENR_TIM2EN                  ((uint32_t)0x00000001)
#define  RCC_APB1ENR_TIM3EN                  ((uint32_t)0x00000002)
#define  RCC_APB1ENR_TIM4EN                  ((uint32_t)0x00000004)
#define  RCC_APB1ENR_TIM5EN                  ((uint32_t)0x00000008)
#define  RCC_APB1ENR_TIM6EN                  ((uint32_t)0x00000010)
#define  RCC_APB1ENR_TIM7EN                  ((uint32_t)0x00000020)
#define  RCC_APB1ENR_TIM12EN                 ((uint32_t)0x00000040)
#define  RCC_APB1ENR_TIM13EN                 ((uint32_t)0x00000080)
#define  RCC_APB1ENR_TIM14EN                 ((uint32_t)0x00000100)
#define  RCC_APB1ENR_WWDGEN                  ((uint32_t)0x00000800)
#define  RCC_APB1ENR_SPI2EN                  ((uint32_t)0x00004000)
#define  RCC_APB1ENR_SPI3EN                  ((uint32_t)0x00008000)
#define  RCC_APB1ENR_USART2EN                ((uint32_t)0x00020000)
#define  RCC_APB1ENR_USART3EN                ((uint32_t)0x00040000)
#define  RCC_APB1ENR_UART4EN                 ((uint32_t)0x00080000)
#define  RCC_APB1ENR_UART5EN                 ((uint32_t)0x00100000)
#define  RCC_APB1ENR_I2C1EN                  ((uint32_t)0x00200000)
#define  RCC_APB1ENR_I2C2EN                  ((uint32_t)0x00400000)
#define  RCC_APB1ENR_I2C3EN                  ((uint32_t)0x00800000)
#define  RCC_APB1ENR_CAN1EN                  ((uint32_t)0x02000000)
#define  RCC_APB1ENR_CAN2EN                  ((uint32_t)0x04000000)
#define  RCC_APB1ENR_PWREN                   ((uint32_t)0x10000000)
#define  RCC_APB1ENR_DACEN                   ((uint32_t)0x20000000)
#define  RCC_APB1ENR_UART7EN                 ((uint32_t)0x40000000)
#define  RCC_APB1ENR_UART8EN                 ((uint32_t)0x80000000)

/********************  Bit definition for RCC_APB2ENR register  ***************/
#define  RCC_APB2ENR_TIM1EN                  ((uint32_t)0x00000001)
#define  RCC_APB2ENR_TIM8EN                  ((uint32_t)0x00000002)
#define  RCC_APB2ENR_USART1EN                ((uint32_t)0x00000010)
#define  RCC_APB2ENR_USART6EN                ((uint32_t)0x00000020)
#define  RCC_APB2ENR_ADC1EN                  ((uint32_t)0x00000100)
#define  RCC_APB2ENR_ADC2EN                  ((uint32_t)0x00000200)
#define  RCC_APB2ENR_ADC3EN                  ((uint32_t)0x00000400)
#define  RCC_APB2ENR_SDIOEN                  ((uint32_t)0x00000800)
#define  RCC_APB2ENR_SPI1EN                  ((uint32_t)0x00001000)
#define  RCC_APB2ENR_SPI4EN                  ((uint32_t)0x00002000)
#define  RCC_APB2ENR_SYSCFGEN                ((uint32_t)0x00004000)
#define  RCC_APB2ENR_TIM9EN                  ((uint32_t)0x00010000)
#define  RCC_APB2ENR_TIM10EN                 ((uint32_t)0x00020000)
#define  RCC_APB2ENR_TIM11EN                 ((uint32_t)0x00040000)
#define  RCC_APB2ENR_SPI5EN                  ((uint32_t)0x00100000)
#define  RCC_APB2ENR_SPI6EN                  ((uint32_t)0x00200000)
#define  RCC_APB2ENR_SAI1EN                  ((uint32_t)0x00400000)
#define  RCC_APB2ENR_LTDCEN                  ((uint32_t)0x04000000)

/********************  Bit definition for RCC_AHB1LPENR register  *************/
#define  RCC_AHB1LPENR_GPIOALPEN             ((uint32_t)0x00000001)
#define  RCC_AHB1LPENR_GPIOBLPEN             ((uint32_t)0x00000002)
#define  RCC_AHB1LPENR_GPIOCLPEN             ((uint32_t)0x00000004)
#define  RCC_AHB1LPENR_GPIODLPEN             ((uint32_t)0x00000008)
#define  RCC_AHB1LPENR_GPIOELPEN             ((uint32_t)0x00000010)
#define  RCC_AHB1LPENR_GPIOFLPEN             ((uint32_t)0x00000020)
#define  RCC_AHB1LPENR_GPIOGLPEN             ((uint32_t)0x00000040)
#define  RCC_AHB1LPENR_GPIOHLPEN             ((uint32_t)0x00000080)
#define  RCC_AHB1LPENR_GPIOILPEN             ((uint32_t)0x00000100)
#define  RCC_AHB1LPENR_GPIOJLPEN             ((uint32_t)0x00000200)
#define  RCC_AHB1LPENR_GPIOKLPEN             ((uint32_t)0x00000400)
#define  RCC_AHB1LPENR_CRCLPEN               ((uint32_t)0x00001000)
#define  RCC_AHB1LPENR_FLITFLPEN             ((uint32_t)0x00008000)
#define  RCC_AHB1LPENR_SRAM1LPEN             ((uint32_t)0x00010000)
#define  RCC_AHB1LPENR_SRAM2LPEN             ((uint32_t)0x00020000)
#define  RCC_AHB1LPENR_BKPSRAMLPEN           ((uint32_t)0x00040000)
#define  RCC_AHB1LPENR_SRAM3LPEN             ((uint32_t)0x00080000)
#define  RCC_AHB1LPENR_DMA1LPEN              ((uint32_t)0x00200000)
#define  RCC_AHB1LPENR_DMA2LPEN              ((uint32_t)0x00400000)
#define  RCC_AHB1LPENR_DMA2DLPEN             ((uint32_t)0x00800000)
#define  RCC_AHB1LPENR_ETHMACLPEN            ((uint32_t)0x02000000)
#define  RCC_AHB1LPENR_ETHMACTXLPEN          ((uint32_t)0x04000000)
#define  RCC_AHB1LPENR_ETHMACRXLPEN          ((uint32_t)0x08000000)
#define  RCC_AHB1LPENR_ETHMACPTPLPEN         ((uint32_t)0x10000000)
#define  RCC_AHB1LPENR_OTGHSLPEN             ((uint32_t)0x20000000)
#define  RCC_AHB1LPENR_OTGHSULPILPEN         ((uint32_t)0x40000000)

/********************  Bit definition for RCC_AHB2LPENR register  *************/
#define  RCC_AHB2LPENR_DCMILPEN              ((uint32_t)0x00000001)
#define  RCC_AHB2LPENR_CRYPLPEN              ((uint32_t)0x00000010)
#define  RCC_AHB2LPENR_HASHLPEN              ((uint32_t)0x00000020)
#define  RCC_AHB2LPENR_RNGLPEN               ((uint32_t)0x00000040)
#define  RCC_AHB2LPENR_OTGFSLPEN             ((uint32_t)0x00000080)

/********************  Bit definition for RCC_AHB3LPENR register  *************/
#if defined(STM32F40_41xxx)
#define  RCC_AHB3LPENR_FSMCLPEN              ((uint32_t)0x00000001)
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define  RCC_AHB3LPENR_FMCLPEN              ((uint32_t)0x00000001)
#endif /* STM32F427_437xx ||  STM32F429_439xx */

/********************  Bit definition for RCC_APB1LPENR register  *************/
#define  RCC_APB1LPENR_TIM2LPEN              ((uint32_t)0x00000001)
#define  RCC_APB1LPENR_TIM3LPEN              ((uint32_t)0x00000002)
#define  RCC_APB1LPENR_TIM4LPEN              ((uint32_t)0x00000004)
#define  RCC_APB1LPENR_TIM5LPEN              ((uint32_t)0x00000008)
#define  RCC_APB1LPENR_TIM6LPEN              ((uint32_t)0x00000010)
#define  RCC_APB1LPENR_TIM7LPEN              ((uint32_t)0x00000020)
#define  RCC_APB1LPENR_TIM12LPEN             ((uint32_t)0x00000040)
#define  RCC_APB1LPENR_TIM13LPEN             ((uint32_t)0x00000080)
#define  RCC_APB1LPENR_TIM14LPEN             ((uint32_t)0x00000100)
#define  RCC_APB1LPENR_WWDGLPEN              ((uint32_t)0x00000800)
#define  RCC_APB1LPENR_SPI2LPEN              ((uint32_t)0x00004000)
#define  RCC_APB1LPENR_SPI3LPEN              ((uint32_t)0x00008000)
#define  RCC_APB1LPENR_USART2LPEN            ((uint32_t)0x00020000)
#define  RCC_APB1LPENR_USART3LPEN            ((uint32_t)0x00040000)
#define  RCC_APB1LPENR_UART4LPEN             ((uint32_t)0x00080000)
#define  RCC_APB1LPENR_UART5LPEN             ((uint32_t)0x00100000)
#define  RCC_APB1LPENR_I2C1LPEN              ((uint32_t)0x00200000)
#define  RCC_APB1LPENR_I2C2LPEN              ((uint32_t)0x00400000)
#define  RCC_APB1LPENR_I2C3LPEN              ((uint32_t)0x00800000)
#define  RCC_APB1LPENR_CAN1LPEN              ((uint32_t)0x02000000)
#define  RCC_APB1LPENR_CAN2LPEN              ((uint32_t)0x04000000)
#define  RCC_APB1LPENR_PWRLPEN               ((uint32_t)0x10000000)
#define  RCC_APB1LPENR_DACLPEN               ((uint32_t)0x20000000)
#define  RCC_APB1LPENR_UART7LPEN             ((uint32_t)0x40000000)
#define  RCC_APB1LPENR_UART8LPEN             ((uint32_t)0x80000000)

/********************  Bit definition for RCC_APB2LPENR register  *************/
#define  RCC_APB2LPENR_TIM1LPEN              ((uint32_t)0x00000001)
#define  RCC_APB2LPENR_TIM8LPEN              ((uint32_t)0x00000002)
#define  RCC_APB2LPENR_USART1LPEN            ((uint32_t)0x00000010)
#define  RCC_APB2LPENR_USART6LPEN            ((uint32_t)0x00000020)
#define  RCC_APB2LPENR_ADC1LPEN              ((uint32_t)0x00000100)
#define  RCC_APB2LPENR_ADC2PEN               ((uint32_t)0x00000200)
#define  RCC_APB2LPENR_ADC3LPEN              ((uint32_t)0x00000400)
#define  RCC_APB2LPENR_SDIOLPEN              ((uint32_t)0x00000800)
#define  RCC_APB2LPENR_SPI1LPEN              ((uint32_t)0x00001000)
#define  RCC_APB2LPENR_SPI4LPEN              ((uint32_t)0x00002000)
#define  RCC_APB2LPENR_SYSCFGLPEN            ((uint32_t)0x00004000)
#define  RCC_APB2LPENR_TIM9LPEN              ((uint32_t)0x00010000)
#define  RCC_APB2LPENR_TIM10LPEN             ((uint32_t)0x00020000)
#define  RCC_APB2LPENR_TIM11LPEN             ((uint32_t)0x00040000)
#define  RCC_APB2LPENR_SPI5LPEN              ((uint32_t)0x00100000)
#define  RCC_APB2LPENR_SPI6LPEN              ((uint32_t)0x00200000)
#define  RCC_APB2LPENR_SAI1LPEN              ((uint32_t)0x00400000)
#define  RCC_APB2LPENR_LTDCLPEN              ((uint32_t)0x04000000)

/********************  Bit definition for RCC_BDCR register  ******************/
#define  RCC_BDCR_LSEON                      ((uint32_t)0x00000001)
#define  RCC_BDCR_LSERDY                     ((uint32_t)0x00000002)
#define  RCC_BDCR_LSEBYP                     ((uint32_t)0x00000004)
#define  RCC_BDCR_LSEMOD                     ((uint32_t)0x00000008)

#define  RCC_BDCR_RTCSEL                    ((uint32_t)0x00000300)
#define  RCC_BDCR_RTCSEL_0                  ((uint32_t)0x00000100)
#define  RCC_BDCR_RTCSEL_1                  ((uint32_t)0x00000200)

#define  RCC_BDCR_RTCEN                      ((uint32_t)0x00008000)
#define  RCC_BDCR_BDRST                      ((uint32_t)0x00010000)

/********************  Bit definition for RCC_CSR register  *******************/
#define  RCC_CSR_LSION                       ((uint32_t)0x00000001)
#define  RCC_CSR_LSIRDY                      ((uint32_t)0x00000002)
#define  RCC_CSR_RMVF                        ((uint32_t)0x01000000)
#define  RCC_CSR_BORRSTF                     ((uint32_t)0x02000000)
#define  RCC_CSR_PADRSTF                     ((uint32_t)0x04000000)
#define  RCC_CSR_PORRSTF                     ((uint32_t)0x08000000)
#define  RCC_CSR_SFTRSTF                     ((uint32_t)0x10000000)
#define  RCC_CSR_WDGRSTF                     ((uint32_t)0x20000000)
#define  RCC_CSR_WWDGRSTF                    ((uint32_t)0x40000000)
#define  RCC_CSR_LPWRRSTF                    ((uint32_t)0x80000000)

/********************  Bit definition for RCC_SSCGR register  *****************/
#define  RCC_SSCGR_MODPER                    ((uint32_t)0x00001FFF)
#define  RCC_SSCGR_INCSTEP                   ((uint32_t)0x0FFFE000)
#define  RCC_SSCGR_SPREADSEL                 ((uint32_t)0x40000000)
#define  RCC_SSCGR_SSCGEN                    ((uint32_t)0x80000000)

/********************  Bit definition for RCC_PLLI2SCFGR register  ************/
#define  RCC_PLLI2SCFGR_PLLI2SM              ((uint32_t)0x0000003F)
#define  RCC_PLLI2SCFGR_PLLI2SM_0            ((uint32_t)0x00000001)
#define  RCC_PLLI2SCFGR_PLLI2SM_1            ((uint32_t)0x00000002)
#define  RCC_PLLI2SCFGR_PLLI2SM_2            ((uint32_t)0x00000004)
#define  RCC_PLLI2SCFGR_PLLI2SM_3            ((uint32_t)0x00000008)
#define  RCC_PLLI2SCFGR_PLLI2SM_4            ((uint32_t)0x00000010)
#define  RCC_PLLI2SCFGR_PLLI2SM_5            ((uint32_t)0x00000020)

/********************  Bit definition for RCC_PLLI2SCFGR register  ************/
#define  RCC_PLLI2SCFGR_PLLI2SN              ((uint32_t)0x00007FC0)
#define  RCC_PLLI2SCFGR_PLLI2SQ              ((uint32_t)0x0F000000)
#define  RCC_PLLI2SCFGR_PLLI2SR              ((uint32_t)0x70000000)

/********************  Bit definition for RCC_PLLSAICFGR register  ************/
#define  RCC_PLLSAICFGR_PLLI2SN              ((uint32_t)0x00007FC0)
#define  RCC_PLLSAICFGR_PLLI2SQ              ((uint32_t)0x0F000000)
#define  RCC_PLLSAICFGR_PLLI2SR              ((uint32_t)0x70000000)

/********************  Bit definition for RCC_DCKCFGR register  ***************/
#define  RCC_DCKCFGR_PLLI2SDIVQ              ((uint32_t)0x0000001F)
#define  RCC_DCKCFGR_PLLSAIDIVQ              ((uint32_t)0x00001F00)
#define  RCC_DCKCFGR_PLLSAIDIVR              ((uint32_t)0x00030000)
#define  RCC_DCKCFGR_SAI1ASRC                ((uint32_t)0x00300000)
#define  RCC_DCKCFGR_SAI1BSRC                ((uint32_t)0x00C00000)
#define  RCC_DCKCFGR_TIMPRE                  ((uint32_t)0x01000000)


/******************************************************************************/
/*                                                                            */
/*                                    RNG                                     */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for RNG_CR register  *******************/
#define RNG_CR_RNGEN                         ((uint32_t)0x00000004)
#define RNG_CR_IE                            ((uint32_t)0x00000008)

/********************  Bits definition for RNG_SR register  *******************/
#define RNG_SR_DRDY                          ((uint32_t)0x00000001)
#define RNG_SR_CECS                          ((uint32_t)0x00000002)
#define RNG_SR_SECS                          ((uint32_t)0x00000004)
#define RNG_SR_CEIS                          ((uint32_t)0x00000020)
#define RNG_SR_SEIS                          ((uint32_t)0x00000040)

/******************************************************************************/
/*                                                                            */
/*                           Real-Time Clock (RTC)                            */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for RTC_TR register  *******************/
#define RTC_TR_PM                            ((uint32_t)0x00400000)
#define RTC_TR_HT                            ((uint32_t)0x00300000)
#define RTC_TR_HT_0                          ((uint32_t)0x00100000)
#define RTC_TR_HT_1                          ((uint32_t)0x00200000)
#define RTC_TR_HU                            ((uint32_t)0x000F0000)
#define RTC_TR_HU_0                          ((uint32_t)0x00010000)
#define RTC_TR_HU_1                          ((uint32_t)0x00020000)
#define RTC_TR_HU_2                          ((uint32_t)0x00040000)
#define RTC_TR_HU_3                          ((uint32_t)0x00080000)
#define RTC_TR_MNT                           ((uint32_t)0x00007000)
#define RTC_TR_MNT_0                         ((uint32_t)0x00001000)
#define RTC_TR_MNT_1                         ((uint32_t)0x00002000)
#define RTC_TR_MNT_2                         ((uint32_t)0x00004000)
#define RTC_TR_MNU                           ((uint32_t)0x00000F00)
#define RTC_TR_MNU_0                         ((uint32_t)0x00000100)
#define RTC_TR_MNU_1                         ((uint32_t)0x00000200)
#define RTC_TR_MNU_2                         ((uint32_t)0x00000400)
#define RTC_TR_MNU_3                         ((uint32_t)0x00000800)
#define RTC_TR_ST                            ((uint32_t)0x00000070)
#define RTC_TR_ST_0                          ((uint32_t)0x00000010)
#define RTC_TR_ST_1                          ((uint32_t)0x00000020)
#define RTC_TR_ST_2                          ((uint32_t)0x00000040)
#define RTC_TR_SU                            ((uint32_t)0x0000000F)
#define RTC_TR_SU_0                          ((uint32_t)0x00000001)
#define RTC_TR_SU_1                          ((uint32_t)0x00000002)
#define RTC_TR_SU_2                          ((uint32_t)0x00000004)
#define RTC_TR_SU_3                          ((uint32_t)0x00000008)

/********************  Bits definition for RTC_DR register  *******************/
#define RTC_DR_YT                            ((uint32_t)0x00F00000)
#define RTC_DR_YT_0                          ((uint32_t)0x00100000)
#define RTC_DR_YT_1                          ((uint32_t)0x00200000)
#define RTC_DR_YT_2                          ((uint32_t)0x00400000)
#define RTC_DR_YT_3                          ((uint32_t)0x00800000)
#define RTC_DR_YU                            ((uint32_t)0x000F0000)
#define RTC_DR_YU_0                          ((uint32_t)0x00010000)
#define RTC_DR_YU_1                          ((uint32_t)0x00020000)
#define RTC_DR_YU_2                          ((uint32_t)0x00040000)
#define RTC_DR_YU_3                          ((uint32_t)0x00080000)
#define RTC_DR_WDU                           ((uint32_t)0x0000E000)
#define RTC_DR_WDU_0                         ((uint32_t)0x00002000)
#define RTC_DR_WDU_1                         ((uint32_t)0x00004000)
#define RTC_DR_WDU_2                         ((uint32_t)0x00008000)
#define RTC_DR_MT                            ((uint32_t)0x00001000)
#define RTC_DR_MU                            ((uint32_t)0x00000F00)
#define RTC_DR_MU_0                          ((uint32_t)0x00000100)
#define RTC_DR_MU_1                          ((uint32_t)0x00000200)
#define RTC_DR_MU_2                          ((uint32_t)0x00000400)
#define RTC_DR_MU_3                          ((uint32_t)0x00000800)
#define RTC_DR_DT                            ((uint32_t)0x00000030)
#define RTC_DR_DT_0                          ((uint32_t)0x00000010)
#define RTC_DR_DT_1                          ((uint32_t)0x00000020)
#define RTC_DR_DU                            ((uint32_t)0x0000000F)
#define RTC_DR_DU_0                          ((uint32_t)0x00000001)
#define RTC_DR_DU_1                          ((uint32_t)0x00000002)
#define RTC_DR_DU_2                          ((uint32_t)0x00000004)
#define RTC_DR_DU_3                          ((uint32_t)0x00000008)

/********************  Bits definition for RTC_CR register  *******************/
#define RTC_CR_COE                           ((uint32_t)0x00800000)
#define RTC_CR_OSEL                          ((uint32_t)0x00600000)
#define RTC_CR_OSEL_0                        ((uint32_t)0x00200000)
#define RTC_CR_OSEL_1                        ((uint32_t)0x00400000)
#define RTC_CR_POL                           ((uint32_t)0x00100000)
#define RTC_CR_COSEL                         ((uint32_t)0x00080000)
#define RTC_CR_BCK                           ((uint32_t)0x00040000)
#define RTC_CR_SUB1H                         ((uint32_t)0x00020000)
#define RTC_CR_ADD1H                         ((uint32_t)0x00010000)
#define RTC_CR_TSIE                          ((uint32_t)0x00008000)
#define RTC_CR_WUTIE                         ((uint32_t)0x00004000)
#define RTC_CR_ALRBIE                        ((uint32_t)0x00002000)
#define RTC_CR_ALRAIE                        ((uint32_t)0x00001000)
#define RTC_CR_TSE                           ((uint32_t)0x00000800)
#define RTC_CR_WUTE                          ((uint32_t)0x00000400)
#define RTC_CR_ALRBE                         ((uint32_t)0x00000200)
#define RTC_CR_ALRAE                         ((uint32_t)0x00000100)
#define RTC_CR_DCE                           ((uint32_t)0x00000080)
#define RTC_CR_FMT                           ((uint32_t)0x00000040)
#define RTC_CR_BYPSHAD                       ((uint32_t)0x00000020)
#define RTC_CR_REFCKON                       ((uint32_t)0x00000010)
#define RTC_CR_TSEDGE                        ((uint32_t)0x00000008)
#define RTC_CR_WUCKSEL                       ((uint32_t)0x00000007)
#define RTC_CR_WUCKSEL_0                     ((uint32_t)0x00000001)
#define RTC_CR_WUCKSEL_1                     ((uint32_t)0x00000002)
#define RTC_CR_WUCKSEL_2                     ((uint32_t)0x00000004)

/********************  Bits definition for RTC_ISR register  ******************/
#define RTC_ISR_RECALPF                      ((uint32_t)0x00010000)
#define RTC_ISR_TAMP1F                       ((uint32_t)0x00002000)
#define RTC_ISR_TSOVF                        ((uint32_t)0x00001000)
#define RTC_ISR_TSF                          ((uint32_t)0x00000800)
#define RTC_ISR_WUTF                         ((uint32_t)0x00000400)
#define RTC_ISR_ALRBF                        ((uint32_t)0x00000200)
#define RTC_ISR_ALRAF                        ((uint32_t)0x00000100)
#define RTC_ISR_INIT                         ((uint32_t)0x00000080)
#define RTC_ISR_INITF                        ((uint32_t)0x00000040)
#define RTC_ISR_RSF                          ((uint32_t)0x00000020)
#define RTC_ISR_INITS                        ((uint32_t)0x00000010)
#define RTC_ISR_SHPF                         ((uint32_t)0x00000008)
#define RTC_ISR_WUTWF                        ((uint32_t)0x00000004)
#define RTC_ISR_ALRBWF                       ((uint32_t)0x00000002)
#define RTC_ISR_ALRAWF                       ((uint32_t)0x00000001)

/********************  Bits definition for RTC_PRER register  *****************/
#define RTC_PRER_PREDIV_A                    ((uint32_t)0x007F0000)
#define RTC_PRER_PREDIV_S                    ((uint32_t)0x00001FFF)

/********************  Bits definition for RTC_WUTR register  *****************/
#define RTC_WUTR_WUT                         ((uint32_t)0x0000FFFF)

/********************  Bits definition for RTC_CALIBR register  ***************/
#define RTC_CALIBR_DCS                       ((uint32_t)0x00000080)
#define RTC_CALIBR_DC                        ((uint32_t)0x0000001F)

/********************  Bits definition for RTC_ALRMAR register  ***************/
#define RTC_ALRMAR_MSK4                      ((uint32_t)0x80000000)
#define RTC_ALRMAR_WDSEL                     ((uint32_t)0x40000000)
#define RTC_ALRMAR_DT                        ((uint32_t)0x30000000)
#define RTC_ALRMAR_DT_0                      ((uint32_t)0x10000000)
#define RTC_ALRMAR_DT_1                      ((uint32_t)0x20000000)
#define RTC_ALRMAR_DU                        ((uint32_t)0x0F000000)
#define RTC_ALRMAR_DU_0                      ((uint32_t)0x01000000)
#define RTC_ALRMAR_DU_1                      ((uint32_t)0x02000000)
#define RTC_ALRMAR_DU_2                      ((uint32_t)0x04000000)
#define RTC_ALRMAR_DU_3                      ((uint32_t)0x08000000)
#define RTC_ALRMAR_MSK3                      ((uint32_t)0x00800000)
#define RTC_ALRMAR_PM                        ((uint32_t)0x00400000)
#define RTC_ALRMAR_HT                        ((uint32_t)0x00300000)
#define RTC_ALRMAR_HT_0                      ((uint32_t)0x00100000)
#define RTC_ALRMAR_HT_1                      ((uint32_t)0x00200000)
#define RTC_ALRMAR_HU                        ((uint32_t)0x000F0000)
#define RTC_ALRMAR_HU_0                      ((uint32_t)0x00010000)
#define RTC_ALRMAR_HU_1                      ((uint32_t)0x00020000)
#define RTC_ALRMAR_HU_2                      ((uint32_t)0x00040000)
#define RTC_ALRMAR_HU_3                      ((uint32_t)0x00080000)
#define RTC_ALRMAR_MSK2                      ((uint32_t)0x00008000)
#define RTC_ALRMAR_MNT                       ((uint32_t)0x00007000)
#define RTC_ALRMAR_MNT_0                     ((uint32_t)0x00001000)
#define RTC_ALRMAR_MNT_1                     ((uint32_t)0x00002000)
#define RTC_ALRMAR_MNT_2                     ((uint32_t)0x00004000)
#define RTC_ALRMAR_MNU                       ((uint32_t)0x00000F00)
#define RTC_ALRMAR_MNU_0                     ((uint32_t)0x00000100)
#define RTC_ALRMAR_MNU_1                     ((uint32_t)0x00000200)
#define RTC_ALRMAR_MNU_2                     ((uint32_t)0x00000400)
#define RTC_ALRMAR_MNU_3                     ((uint32_t)0x00000800)
#define RTC_ALRMAR_MSK1                      ((uint32_t)0x00000080)
#define RTC_ALRMAR_ST                        ((uint32_t)0x00000070)
#define RTC_ALRMAR_ST_0                      ((uint32_t)0x00000010)
#define RTC_ALRMAR_ST_1                      ((uint32_t)0x00000020)
#define RTC_ALRMAR_ST_2                      ((uint32_t)0x00000040)
#define RTC_ALRMAR_SU                        ((uint32_t)0x0000000F)
#define RTC_ALRMAR_SU_0                      ((uint32_t)0x00000001)
#define RTC_ALRMAR_SU_1                      ((uint32_t)0x00000002)
#define RTC_ALRMAR_SU_2                      ((uint32_t)0x00000004)
#define RTC_ALRMAR_SU_3                      ((uint32_t)0x00000008)

/********************  Bits definition for RTC_ALRMBR register  ***************/
#define RTC_ALRMBR_MSK4                      ((uint32_t)0x80000000)
#define RTC_ALRMBR_WDSEL                     ((uint32_t)0x40000000)
#define RTC_ALRMBR_DT                        ((uint32_t)0x30000000)
#define RTC_ALRMBR_DT_0                      ((uint32_t)0x10000000)
#define RTC_ALRMBR_DT_1                      ((uint32_t)0x20000000)
#define RTC_ALRMBR_DU                        ((uint32_t)0x0F000000)
#define RTC_ALRMBR_DU_0                      ((uint32_t)0x01000000)
#define RTC_ALRMBR_DU_1                      ((uint32_t)0x02000000)
#define RTC_ALRMBR_DU_2                      ((uint32_t)0x04000000)
#define RTC_ALRMBR_DU_3                      ((uint32_t)0x08000000)
#define RTC_ALRMBR_MSK3                      ((uint32_t)0x00800000)
#define RTC_ALRMBR_PM                        ((uint32_t)0x00400000)
#define RTC_ALRMBR_HT                        ((uint32_t)0x00300000)
#define RTC_ALRMBR_HT_0                      ((uint32_t)0x00100000)
#define RTC_ALRMBR_HT_1                      ((uint32_t)0x00200000)
#define RTC_ALRMBR_HU                        ((uint32_t)0x000F0000)
#define RTC_ALRMBR_HU_0                      ((uint32_t)0x00010000)
#define RTC_ALRMBR_HU_1                      ((uint32_t)0x00020000)
#define RTC_ALRMBR_HU_2                      ((uint32_t)0x00040000)
#define RTC_ALRMBR_HU_3                      ((uint32_t)0x00080000)
#define RTC_ALRMBR_MSK2                      ((uint32_t)0x00008000)
#define RTC_ALRMBR_MNT                       ((uint32_t)0x00007000)
#define RTC_ALRMBR_MNT_0                     ((uint32_t)0x00001000)
#define RTC_ALRMBR_MNT_1                     ((uint32_t)0x00002000)
#define RTC_ALRMBR_MNT_2                     ((uint32_t)0x00004000)
#define RTC_ALRMBR_MNU                       ((uint32_t)0x00000F00)
#define RTC_ALRMBR_MNU_0                     ((uint32_t)0x00000100)
#define RTC_ALRMBR_MNU_1                     ((uint32_t)0x00000200)
#define RTC_ALRMBR_MNU_2                     ((uint32_t)0x00000400)
#define RTC_ALRMBR_MNU_3                     ((uint32_t)0x00000800)
#define RTC_ALRMBR_MSK1                      ((uint32_t)0x00000080)
#define RTC_ALRMBR_ST                        ((uint32_t)0x00000070)
#define RTC_ALRMBR_ST_0                      ((uint32_t)0x00000010)
#define RTC_ALRMBR_ST_1                      ((uint32_t)0x00000020)
#define RTC_ALRMBR_ST_2                      ((uint32_t)0x00000040)
#define RTC_ALRMBR_SU                        ((uint32_t)0x0000000F)
#define RTC_ALRMBR_SU_0                      ((uint32_t)0x00000001)
#define RTC_ALRMBR_SU_1                      ((uint32_t)0x00000002)
#define RTC_ALRMBR_SU_2                      ((uint32_t)0x00000004)
#define RTC_ALRMBR_SU_3                      ((uint32_t)0x00000008)

/********************  Bits definition for RTC_WPR register  ******************/
#define RTC_WPR_KEY                          ((uint32_t)0x000000FF)

/********************  Bits definition for RTC_SSR register  ******************/
#define RTC_SSR_SS                           ((uint32_t)0x0000FFFF)

/********************  Bits definition for RTC_SHIFTR register  ***************/
#define RTC_SHIFTR_SUBFS                     ((uint32_t)0x00007FFF)
#define RTC_SHIFTR_ADD1S                     ((uint32_t)0x80000000)

/********************  Bits definition for RTC_TSTR register  *****************/
#define RTC_TSTR_PM                          ((uint32_t)0x00400000)
#define RTC_TSTR_HT                          ((uint32_t)0x00300000)
#define RTC_TSTR_HT_0                        ((uint32_t)0x00100000)
#define RTC_TSTR_HT_1                        ((uint32_t)0x00200000)
#define RTC_TSTR_HU                          ((uint32_t)0x000F0000)
#define RTC_TSTR_HU_0                        ((uint32_t)0x00010000)
#define RTC_TSTR_HU_1                        ((uint32_t)0x00020000)
#define RTC_TSTR_HU_2                        ((uint32_t)0x00040000)
#define RTC_TSTR_HU_3                        ((uint32_t)0x00080000)
#define RTC_TSTR_MNT                         ((uint32_t)0x00007000)
#define RTC_TSTR_MNT_0                       ((uint32_t)0x00001000)
#define RTC_TSTR_MNT_1                       ((uint32_t)0x00002000)
#define RTC_TSTR_MNT_2                       ((uint32_t)0x00004000)
#define RTC_TSTR_MNU                         ((uint32_t)0x00000F00)
#define RTC_TSTR_MNU_0                       ((uint32_t)0x00000100)
#define RTC_TSTR_MNU_1                       ((uint32_t)0x00000200)
#define RTC_TSTR_MNU_2                       ((uint32_t)0x00000400)
#define RTC_TSTR_MNU_3                       ((uint32_t)0x00000800)
#define RTC_TSTR_ST                          ((uint32_t)0x00000070)
#define RTC_TSTR_ST_0                        ((uint32_t)0x00000010)
#define RTC_TSTR_ST_1                        ((uint32_t)0x00000020)
#define RTC_TSTR_ST_2                        ((uint32_t)0x00000040)
#define RTC_TSTR_SU                          ((uint32_t)0x0000000F)
#define RTC_TSTR_SU_0                        ((uint32_t)0x00000001)
#define RTC_TSTR_SU_1                        ((uint32_t)0x00000002)
#define RTC_TSTR_SU_2                        ((uint32_t)0x00000004)
#define RTC_TSTR_SU_3                        ((uint32_t)0x00000008)

/********************  Bits definition for RTC_TSDR register  *****************/
#define RTC_TSDR_WDU                         ((uint32_t)0x0000E000)
#define RTC_TSDR_WDU_0                       ((uint32_t)0x00002000)
#define RTC_TSDR_WDU_1                       ((uint32_t)0x00004000)
#define RTC_TSDR_WDU_2                       ((uint32_t)0x00008000)
#define RTC_TSDR_MT                          ((uint32_t)0x00001000)
#define RTC_TSDR_MU                          ((uint32_t)0x00000F00)
#define RTC_TSDR_MU_0                        ((uint32_t)0x00000100)
#define RTC_TSDR_MU_1                        ((uint32_t)0x00000200)
#define RTC_TSDR_MU_2                        ((uint32_t)0x00000400)
#define RTC_TSDR_MU_3                        ((uint32_t)0x00000800)
#define RTC_TSDR_DT                          ((uint32_t)0x00000030)
#define RTC_TSDR_DT_0                        ((uint32_t)0x00000010)
#define RTC_TSDR_DT_1                        ((uint32_t)0x00000020)
#define RTC_TSDR_DU                          ((uint32_t)0x0000000F)
#define RTC_TSDR_DU_0                        ((uint32_t)0x00000001)
#define RTC_TSDR_DU_1                        ((uint32_t)0x00000002)
#define RTC_TSDR_DU_2                        ((uint32_t)0x00000004)
#define RTC_TSDR_DU_3                        ((uint32_t)0x00000008)

/********************  Bits definition for RTC_TSSSR register  ****************/
#define RTC_TSSSR_SS                         ((uint32_t)0x0000FFFF)

/********************  Bits definition for RTC_CAL register  *****************/
#define RTC_CALR_CALP                        ((uint32_t)0x00008000)
#define RTC_CALR_CALW8                       ((uint32_t)0x00004000)
#define RTC_CALR_CALW16                      ((uint32_t)0x00002000)
#define RTC_CALR_CALM                        ((uint32_t)0x000001FF)
#define RTC_CALR_CALM_0                      ((uint32_t)0x00000001)
#define RTC_CALR_CALM_1                      ((uint32_t)0x00000002)
#define RTC_CALR_CALM_2                      ((uint32_t)0x00000004)
#define RTC_CALR_CALM_3                      ((uint32_t)0x00000008)
#define RTC_CALR_CALM_4                      ((uint32_t)0x00000010)
#define RTC_CALR_CALM_5                      ((uint32_t)0x00000020)
#define RTC_CALR_CALM_6                      ((uint32_t)0x00000040)
#define RTC_CALR_CALM_7                      ((uint32_t)0x00000080)
#define RTC_CALR_CALM_8                      ((uint32_t)0x00000100)

/********************  Bits definition for RTC_TAFCR register  ****************/
#define RTC_TAFCR_ALARMOUTTYPE               ((uint32_t)0x00040000)
#define RTC_TAFCR_TSINSEL                    ((uint32_t)0x00020000)
#define RTC_TAFCR_TAMPINSEL                  ((uint32_t)0x00010000)
#define RTC_TAFCR_TAMPPUDIS                  ((uint32_t)0x00008000)
#define RTC_TAFCR_TAMPPRCH                   ((uint32_t)0x00006000)
#define RTC_TAFCR_TAMPPRCH_0                 ((uint32_t)0x00002000)
#define RTC_TAFCR_TAMPPRCH_1                 ((uint32_t)0x00004000)
#define RTC_TAFCR_TAMPFLT                    ((uint32_t)0x00001800)
#define RTC_TAFCR_TAMPFLT_0                  ((uint32_t)0x00000800)
#define RTC_TAFCR_TAMPFLT_1                  ((uint32_t)0x00001000)
#define RTC_TAFCR_TAMPFREQ                   ((uint32_t)0x00000700)
#define RTC_TAFCR_TAMPFREQ_0                 ((uint32_t)0x00000100)
#define RTC_TAFCR_TAMPFREQ_1                 ((uint32_t)0x00000200)
#define RTC_TAFCR_TAMPFREQ_2                 ((uint32_t)0x00000400)
#define RTC_TAFCR_TAMPTS                     ((uint32_t)0x00000080)
#define RTC_TAFCR_TAMPIE                     ((uint32_t)0x00000004)
#define RTC_TAFCR_TAMP1TRG                   ((uint32_t)0x00000002)
#define RTC_TAFCR_TAMP1E                     ((uint32_t)0x00000001)

/********************  Bits definition for RTC_ALRMASSR register  *************/
#define RTC_ALRMASSR_MASKSS                  ((uint32_t)0x0F000000)
#define RTC_ALRMASSR_MASKSS_0                ((uint32_t)0x01000000)
#define RTC_ALRMASSR_MASKSS_1                ((uint32_t)0x02000000)
#define RTC_ALRMASSR_MASKSS_2                ((uint32_t)0x04000000)
#define RTC_ALRMASSR_MASKSS_3                ((uint32_t)0x08000000)
#define RTC_ALRMASSR_SS                      ((uint32_t)0x00007FFF)

/********************  Bits definition for RTC_ALRMBSSR register  *************/
#define RTC_ALRMBSSR_MASKSS                  ((uint32_t)0x0F000000)
#define RTC_ALRMBSSR_MASKSS_0                ((uint32_t)0x01000000)
#define RTC_ALRMBSSR_MASKSS_1                ((uint32_t)0x02000000)
#define RTC_ALRMBSSR_MASKSS_2                ((uint32_t)0x04000000)
#define RTC_ALRMBSSR_MASKSS_3                ((uint32_t)0x08000000)
#define RTC_ALRMBSSR_SS                      ((uint32_t)0x00007FFF)

/********************  Bits definition for RTC_BKP0R register  ****************/
#define RTC_BKP0R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP1R register  ****************/
#define RTC_BKP1R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP2R register  ****************/
#define RTC_BKP2R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP3R register  ****************/
#define RTC_BKP3R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP4R register  ****************/
#define RTC_BKP4R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP5R register  ****************/
#define RTC_BKP5R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP6R register  ****************/
#define RTC_BKP6R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP7R register  ****************/
#define RTC_BKP7R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP8R register  ****************/
#define RTC_BKP8R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP9R register  ****************/
#define RTC_BKP9R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP10R register  ***************/
#define RTC_BKP10R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP11R register  ***************/
#define RTC_BKP11R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP12R register  ***************/
#define RTC_BKP12R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP13R register  ***************/
#define RTC_BKP13R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP14R register  ***************/
#define RTC_BKP14R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP15R register  ***************/
#define RTC_BKP15R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP16R register  ***************/
#define RTC_BKP16R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP17R register  ***************/
#define RTC_BKP17R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP18R register  ***************/
#define RTC_BKP18R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP19R register  ***************/
#define RTC_BKP19R                           ((uint32_t)0xFFFFFFFF)

/******************************************************************************/
/*                                                                            */
/*                          Serial Audio Interface                            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for SAI_GCR register  *******************/
#define  SAI_GCR_SYNCIN                  ((uint32_t)0x00000003)        /*!<SYNCIN[1:0] bits (Synchronization Inputs)   */
#define  SAI_GCR_SYNCIN_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_GCR_SYNCIN_1                ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  SAI_GCR_SYNCOUT                 ((uint32_t)0x00000030)        /*!<SYNCOUT[1:0] bits (Synchronization Outputs) */
#define  SAI_GCR_SYNCOUT_0               ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  SAI_GCR_SYNCOUT_1               ((uint32_t)0x00000020)        /*!<Bit 1 */

/*******************  Bit definition for SAI_xCR1 register  *******************/
#define  SAI_xCR1_MODE                    ((uint32_t)0x00000003)        /*!<MODE[1:0] bits (Audio Block Mode)           */
#define  SAI_xCR1_MODE_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_xCR1_MODE_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  SAI_xCR1_PRTCFG                  ((uint32_t)0x0000000C)        /*!<PRTCFG[1:0] bits (Protocol Configuration)   */
#define  SAI_xCR1_PRTCFG_0                ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  SAI_xCR1_PRTCFG_1                ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  SAI_xCR1_DS                      ((uint32_t)0x000000E0)        /*!<DS[1:0] bits (Data Size) */
#define  SAI_xCR1_DS_0                    ((uint32_t)0x00000020)        /*!<Bit 0 */
#define  SAI_xCR1_DS_1                    ((uint32_t)0x00000040)        /*!<Bit 1 */
#define  SAI_xCR1_DS_2                    ((uint32_t)0x00000080)        /*!<Bit 2 */

#define  SAI_xCR1_LSBFIRST                ((uint32_t)0x00000100)        /*!<LSB First Configuration  */
#define  SAI_xCR1_CKSTR                   ((uint32_t)0x00000200)        /*!<ClocK STRobing edge      */

#define  SAI_xCR1_SYNCEN                  ((uint32_t)0x00000C00)        /*!<SYNCEN[1:0](SYNChronization ENable) */
#define  SAI_xCR1_SYNCEN_0                ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  SAI_xCR1_SYNCEN_1                ((uint32_t)0x00000800)        /*!<Bit 1 */

#define  SAI_xCR1_MONO                    ((uint32_t)0x00001000)        /*!<Mono mode                  */
#define  SAI_xCR1_OUTDRIV                 ((uint32_t)0x00002000)        /*!<Output Drive               */
#define  SAI_xCR1_SAIEN                   ((uint32_t)0x00010000)        /*!<Audio Block enable         */
#define  SAI_xCR1_DMAEN                   ((uint32_t)0x00020000)        /*!<DMA enable                 */
#define  SAI_xCR1_NODIV                   ((uint32_t)0x00080000)        /*!<No Divider Configuration   */

#define  SAI_xCR1_MCKDIV                  ((uint32_t)0x00780000)        /*!<MCKDIV[3:0] (Master ClocK Divider)  */
#define  SAI_xCR1_MCKDIV_0                ((uint32_t)0x00080000)        /*!<Bit 0  */
#define  SAI_xCR1_MCKDIV_1                ((uint32_t)0x00100000)        /*!<Bit 1  */
#define  SAI_xCR1_MCKDIV_2                ((uint32_t)0x00200000)        /*!<Bit 2  */
#define  SAI_xCR1_MCKDIV_3                ((uint32_t)0x00400000)        /*!<Bit 3  */

/*******************  Bit definition for SAI_xCR2 register  *******************/
#define  SAI_xCR2_FTH                     ((uint32_t)0x00000003)        /*!<FTH[1:0](Fifo THreshold)  */
#define  SAI_xCR2_FTH_0                   ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_xCR2_FTH_1                   ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  SAI_xCR2_FFLUSH                  ((uint32_t)0x00000008)        /*!<Fifo FLUSH                       */
#define  SAI_xCR2_TRIS                    ((uint32_t)0x00000010)        /*!<TRIState Management on data line */
#define  SAI_xCR2_MUTE                    ((uint32_t)0x00000020)        /*!<Mute mode                        */
#define  SAI_xCR2_MUTEVAL                 ((uint32_t)0x00000040)        /*!<Muate value                      */

#define  SAI_xCR2_MUTECNT                  ((uint32_t)0x00001F80)       /*!<MUTECNT[5:0] (MUTE counter) */
#define  SAI_xCR2_MUTECNT_0               ((uint32_t)0x00000080)        /*!<Bit 0 */
#define  SAI_xCR2_MUTECNT_1               ((uint32_t)0x00000100)        /*!<Bit 1 */
#define  SAI_xCR2_MUTECNT_2               ((uint32_t)0x00000200)        /*!<Bit 2 */
#define  SAI_xCR2_MUTECNT_3               ((uint32_t)0x00000400)        /*!<Bit 3 */
#define  SAI_xCR2_MUTECNT_4               ((uint32_t)0x00000800)        /*!<Bit 4 */
#define  SAI_xCR2_MUTECNT_5               ((uint32_t)0x00001000)        /*!<Bit 5 */

#define  SAI_xCR2_CPL                     ((uint32_t)0x00080000)        /*!< Complement Bit             */

#define  SAI_xCR2_COMP                    ((uint32_t)0x0000C000)        /*!<COMP[1:0] (Companding mode) */
#define  SAI_xCR2_COMP_0                  ((uint32_t)0x00004000)        /*!<Bit 0 */
#define  SAI_xCR2_COMP_1                  ((uint32_t)0x00008000)        /*!<Bit 1 */

/******************  Bit definition for SAI_xFRCR register  *******************/
#define  SAI_xFRCR_FRL                    ((uint32_t)0x000000FF)        /*!<FRL[1:0](Frame length)  */
#define  SAI_xFRCR_FRL_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_xFRCR_FRL_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  SAI_xFRCR_FRL_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  SAI_xFRCR_FRL_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  SAI_xFRCR_FRL_4                  ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  SAI_xFRCR_FRL_5                  ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  SAI_xFRCR_FRL_6                  ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  SAI_xFRCR_FRL_7                  ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  SAI_xFRCR_FSALL                  ((uint32_t)0x00007F00)        /*!<FRL[1:0] (Frame synchronization active level length)  */
#define  SAI_xFRCR_FSALL_0                ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  SAI_xFRCR_FSALL_1                ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  SAI_xFRCR_FSALL_2                ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  SAI_xFRCR_FSALL_3                ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  SAI_xFRCR_FSALL_4                ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  SAI_xFRCR_FSALL_5                ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  SAI_xFRCR_FSALL_6                ((uint32_t)0x00004000)        /*!<Bit 6 */

#define  SAI_xFRCR_FSDEF                  ((uint32_t)0x00010000)        /*!< Frame Synchronization Definition */
#define  SAI_xFRCR_FSPO                   ((uint32_t)0x00020000)        /*!<Frame Synchronization POLarity    */
#define  SAI_xFRCR_FSOFF                  ((uint32_t)0x00040000)        /*!<Frame Synchronization OFFset      */

/******************  Bit definition for SAI_xSLOTR register  *******************/
#define  SAI_xSLOTR_FBOFF                 ((uint32_t)0x0000001F)        /*!<FRL[4:0](First Bit Offset)  */
#define  SAI_xSLOTR_FBOFF_0               ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_xSLOTR_FBOFF_1               ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  SAI_xSLOTR_FBOFF_2               ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  SAI_xSLOTR_FBOFF_3               ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  SAI_xSLOTR_FBOFF_4               ((uint32_t)0x00000010)        /*!<Bit 4 */
                                     
#define  SAI_xSLOTR_SLOTSZ                ((uint32_t)0x000000C0)        /*!<SLOTSZ[1:0] (Slot size)  */
#define  SAI_xSLOTR_SLOTSZ_0              ((uint32_t)0x00000040)        /*!<Bit 0 */
#define  SAI_xSLOTR_SLOTSZ_1              ((uint32_t)0x00000080)        /*!<Bit 1 */

#define  SAI_xSLOTR_NBSLOT                ((uint32_t)0x00000F00)        /*!<NBSLOT[3:0] (Number of Slot in audio Frame)  */
#define  SAI_xSLOTR_NBSLOT_0              ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  SAI_xSLOTR_NBSLOT_1              ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  SAI_xSLOTR_NBSLOT_2              ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  SAI_xSLOTR_NBSLOT_3              ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  SAI_xSLOTR_SLOTEN                ((uint32_t)0xFFFF0000)        /*!<SLOTEN[15:0] (Slot Enable)  */

/*******************  Bit definition for SAI_xIMR register  *******************/
#define  SAI_xIMR_OVRUDRIE                ((uint32_t)0x00000001)        /*!<Overrun underrun interrupt enable                              */
#define  SAI_xIMR_MUTEDETIE               ((uint32_t)0x00000002)        /*!<Mute detection interrupt enable                                */
#define  SAI_xIMR_WCKCFGIE                ((uint32_t)0x00000004)        /*!<Wrong Clock Configuration interrupt enable                     */
#define  SAI_xIMR_FREQIE                  ((uint32_t)0x00000008)        /*!<FIFO request interrupt enable                                  */
#define  SAI_xIMR_CNRDYIE                 ((uint32_t)0x00000010)        /*!<Codec not ready interrupt enable                               */
#define  SAI_xIMR_AFSDETIE                ((uint32_t)0x00000020)        /*!<Anticipated frame synchronization detection interrupt enable   */
#define  SAI_xIMR_LFSDETIE                ((uint32_t)0x00000040)        /*!<Late frame synchronization detection interrupt enable          */

/********************  Bit definition for SAI_xSR register  *******************/
#define  SAI_xSR_OVRUDR                   ((uint32_t)0x00000001)         /*!<Overrun underrun                               */
#define  SAI_xSR_MUTEDET                  ((uint32_t)0x00000002)         /*!<Mute detection                                 */
#define  SAI_xSR_WCKCFG                   ((uint32_t)0x00000004)         /*!<Wrong Clock Configuration                      */
#define  SAI_xSR_FREQ                     ((uint32_t)0x00000008)         /*!<FIFO request                                   */
#define  SAI_xSR_CNRDY                    ((uint32_t)0x00000010)         /*!<Codec not ready                                */
#define  SAI_xSR_AFSDET                   ((uint32_t)0x00000020)         /*!<Anticipated frame synchronization detection    */
#define  SAI_xSR_LFSDET                   ((uint32_t)0x00000040)         /*!<Late frame synchronization detection           */

#define  SAI_xSR_FLVL                     ((uint32_t)0x00070000)         /*!<FLVL[2:0] (FIFO Level Threshold)               */
#define  SAI_xSR_FLVL_0                   ((uint32_t)0x00010000)         /*!<Bit 0 */
#define  SAI_xSR_FLVL_1                   ((uint32_t)0x00020000)         /*!<Bit 1 */
#define  SAI_xSR_FLVL_2                   ((uint32_t)0x00030000)         /*!<Bit 2 */

/******************  Bit definition for SAI_xCLRFR register  ******************/
#define  SAI_xCLRFR_COVRUDR               ((uint32_t)0x00000001)        /*!<Clear Overrun underrun                               */
#define  SAI_xCLRFR_CMUTEDET              ((uint32_t)0x00000002)        /*!<Clear Mute detection                                 */
#define  SAI_xCLRFR_CWCKCFG               ((uint32_t)0x00000004)        /*!<Clear Wrong Clock Configuration                      */
#define  SAI_xCLRFR_CFREQ                 ((uint32_t)0x00000008)        /*!<Clear FIFO request                                   */
#define  SAI_xCLRFR_CCNRDY                ((uint32_t)0x00000010)        /*!<Clear Codec not ready                                */
#define  SAI_xCLRFR_CAFSDET               ((uint32_t)0x00000020)        /*!<Clear Anticipated frame synchronization detection    */
#define  SAI_xCLRFR_CLFSDET               ((uint32_t)0x00000040)        /*!<Clear Late frame synchronization detection           */

/******************  Bit definition for SAI_xDR register  ******************/
#define  SAI_xDR_DATA                     ((uint32_t)0xFFFFFFFF)        

/******************************************************************************/
/*                                                                            */
/*                          SD host Interface                                 */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for SDIO_POWER register  ******************/
#define  SDIO_POWER_PWRCTRL                  ((uint8_t)0x03)               /*!<PWRCTRL[1:0] bits (Power supply control bits) */
#define  SDIO_POWER_PWRCTRL_0                ((uint8_t)0x01)               /*!<Bit 0 */
#define  SDIO_POWER_PWRCTRL_1                ((uint8_t)0x02)               /*!<Bit 1 */

/******************  Bit definition for SDIO_CLKCR register  ******************/
#define  SDIO_CLKCR_CLKDIV                   ((uint16_t)0x00FF)            /*!<Clock divide factor             */
#define  SDIO_CLKCR_CLKEN                    ((uint16_t)0x0100)            /*!<Clock enable bit                */
#define  SDIO_CLKCR_PWRSAV                   ((uint16_t)0x0200)            /*!<Power saving configuration bit  */
#define  SDIO_CLKCR_BYPASS                   ((uint16_t)0x0400)            /*!<Clock divider bypass enable bit */

#define  SDIO_CLKCR_WIDBUS                   ((uint16_t)0x1800)            /*!<WIDBUS[1:0] bits (Wide bus mode enable bit) */
#define  SDIO_CLKCR_WIDBUS_0                 ((uint16_t)0x0800)            /*!<Bit 0 */
#define  SDIO_CLKCR_WIDBUS_1                 ((uint16_t)0x1000)            /*!<Bit 1 */

#define  SDIO_CLKCR_NEGEDGE                  ((uint16_t)0x2000)            /*!<SDIO_CK dephasing selection bit */
#define  SDIO_CLKCR_HWFC_EN                  ((uint16_t)0x4000)            /*!<HW Flow Control enable          */

/*******************  Bit definition for SDIO_ARG register  *******************/
#define  SDIO_ARG_CMDARG                     ((uint32_t)0xFFFFFFFF)            /*!<Command argument */

/*******************  Bit definition for SDIO_CMD register  *******************/
#define  SDIO_CMD_CMDINDEX                   ((uint16_t)0x003F)            /*!<Command Index                               */

#define  SDIO_CMD_WAITRESP                   ((uint16_t)0x00C0)            /*!<WAITRESP[1:0] bits (Wait for response bits) */
#define  SDIO_CMD_WAITRESP_0                 ((uint16_t)0x0040)            /*!< Bit 0 */
#define  SDIO_CMD_WAITRESP_1                 ((uint16_t)0x0080)            /*!< Bit 1 */

#define  SDIO_CMD_WAITINT                    ((uint16_t)0x0100)            /*!<CPSM Waits for Interrupt Request                               */
#define  SDIO_CMD_WAITPEND                   ((uint16_t)0x0200)            /*!<CPSM Waits for ends of data transfer (CmdPend internal signal) */
#define  SDIO_CMD_CPSMEN                     ((uint16_t)0x0400)            /*!<Command path state machine (CPSM) Enable bit                   */
#define  SDIO_CMD_SDIOSUSPEND                ((uint16_t)0x0800)            /*!<SD I/O suspend command                                         */
#define  SDIO_CMD_ENCMDCOMPL                 ((uint16_t)0x1000)            /*!<Enable CMD completion                                          */
#define  SDIO_CMD_NIEN                       ((uint16_t)0x2000)            /*!<Not Interrupt Enable */
#define  SDIO_CMD_CEATACMD                   ((uint16_t)0x4000)            /*!<CE-ATA command       */

/*****************  Bit definition for SDIO_RESPCMD register  *****************/
#define  SDIO_RESPCMD_RESPCMD                ((uint8_t)0x3F)               /*!<Response command index */

/******************  Bit definition for SDIO_RESP0 register  ******************/
#define  SDIO_RESP0_CARDSTATUS0              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDIO_RESP1 register  ******************/
#define  SDIO_RESP1_CARDSTATUS1              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDIO_RESP2 register  ******************/
#define  SDIO_RESP2_CARDSTATUS2              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDIO_RESP3 register  ******************/
#define  SDIO_RESP3_CARDSTATUS3              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDIO_RESP4 register  ******************/
#define  SDIO_RESP4_CARDSTATUS4              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDIO_DTIMER register  *****************/
#define  SDIO_DTIMER_DATATIME                ((uint32_t)0xFFFFFFFF)        /*!<Data timeout period. */

/******************  Bit definition for SDIO_DLEN register  *******************/
#define  SDIO_DLEN_DATALENGTH                ((uint32_t)0x01FFFFFF)        /*!<Data length value    */

/******************  Bit definition for SDIO_DCTRL register  ******************/
#define  SDIO_DCTRL_DTEN                     ((uint16_t)0x0001)            /*!<Data transfer enabled bit         */
#define  SDIO_DCTRL_DTDIR                    ((uint16_t)0x0002)            /*!<Data transfer direction selection */
#define  SDIO_DCTRL_DTMODE                   ((uint16_t)0x0004)            /*!<Data transfer mode selection      */
#define  SDIO_DCTRL_DMAEN                    ((uint16_t)0x0008)            /*!<DMA enabled bit                   */

#define  SDIO_DCTRL_DBLOCKSIZE               ((uint16_t)0x00F0)            /*!<DBLOCKSIZE[3:0] bits (Data block size) */
#define  SDIO_DCTRL_DBLOCKSIZE_0             ((uint16_t)0x0010)            /*!<Bit 0 */
#define  SDIO_DCTRL_DBLOCKSIZE_1             ((uint16_t)0x0020)            /*!<Bit 1 */
#define  SDIO_DCTRL_DBLOCKSIZE_2             ((uint16_t)0x0040)            /*!<Bit 2 */
#define  SDIO_DCTRL_DBLOCKSIZE_3             ((uint16_t)0x0080)            /*!<Bit 3 */

#define  SDIO_DCTRL_RWSTART                  ((uint16_t)0x0100)            /*!<Read wait start         */
#define  SDIO_DCTRL_RWSTOP                   ((uint16_t)0x0200)            /*!<Read wait stop          */
#define  SDIO_DCTRL_RWMOD                    ((uint16_t)0x0400)            /*!<Read wait mode          */
#define  SDIO_DCTRL_SDIOEN                   ((uint16_t)0x0800)            /*!<SD I/O enable functions */

/******************  Bit definition for SDIO_DCOUNT register  *****************/
#define  SDIO_DCOUNT_DATACOUNT               ((uint32_t)0x01FFFFFF)        /*!<Data count value */

/******************  Bit definition for SDIO_STA register  ********************/
#define  SDIO_STA_CCRCFAIL                   ((uint32_t)0x00000001)        /*!<Command response received (CRC check failed)  */
#define  SDIO_STA_DCRCFAIL                   ((uint32_t)0x00000002)        /*!<Data block sent/received (CRC check failed)   */
#define  SDIO_STA_CTIMEOUT                   ((uint32_t)0x00000004)        /*!<Command response timeout                      */
#define  SDIO_STA_DTIMEOUT                   ((uint32_t)0x00000008)        /*!<Data timeout                                  */
#define  SDIO_STA_TXUNDERR                   ((uint32_t)0x00000010)        /*!<Transmit FIFO underrun error                  */
#define  SDIO_STA_RXOVERR                    ((uint32_t)0x00000020)        /*!<Received FIFO overrun error                   */
#define  SDIO_STA_CMDREND                    ((uint32_t)0x00000040)        /*!<Command response received (CRC check passed)  */
#define  SDIO_STA_CMDSENT                    ((uint32_t)0x00000080)        /*!<Command sent (no response required)           */
#define  SDIO_STA_DATAEND                    ((uint32_t)0x00000100)        /*!<Data end (data counter, SDIDCOUNT, is zero)   */
#define  SDIO_STA_STBITERR                   ((uint32_t)0x00000200)        /*!<Start bit not detected on all data signals in wide bus mode */
#define  SDIO_STA_DBCKEND                    ((uint32_t)0x00000400)        /*!<Data block sent/received (CRC check passed)   */
#define  SDIO_STA_CMDACT                     ((uint32_t)0x00000800)        /*!<Command transfer in progress                  */
#define  SDIO_STA_TXACT                      ((uint32_t)0x00001000)        /*!<Data transmit in progress                     */
#define  SDIO_STA_RXACT                      ((uint32_t)0x00002000)        /*!<Data receive in progress                      */
#define  SDIO_STA_TXFIFOHE                   ((uint32_t)0x00004000)        /*!<Transmit FIFO Half Empty: at least 8 words can be written into the FIFO */
#define  SDIO_STA_RXFIFOHF                   ((uint32_t)0x00008000)        /*!<Receive FIFO Half Full: there are at least 8 words in the FIFO */
#define  SDIO_STA_TXFIFOF                    ((uint32_t)0x00010000)        /*!<Transmit FIFO full                            */
#define  SDIO_STA_RXFIFOF                    ((uint32_t)0x00020000)        /*!<Receive FIFO full                             */
#define  SDIO_STA_TXFIFOE                    ((uint32_t)0x00040000)        /*!<Transmit FIFO empty                           */
#define  SDIO_STA_RXFIFOE                    ((uint32_t)0x00080000)        /*!<Receive FIFO empty                            */
#define  SDIO_STA_TXDAVL                     ((uint32_t)0x00100000)        /*!<Data available in transmit FIFO               */
#define  SDIO_STA_RXDAVL                     ((uint32_t)0x00200000)        /*!<Data available in receive FIFO                */
#define  SDIO_STA_SDIOIT                     ((uint32_t)0x00400000)        /*!<SDIO interrupt received                       */
#define  SDIO_STA_CEATAEND                   ((uint32_t)0x00800000)        /*!<CE-ATA command completion signal received for CMD61 */

/*******************  Bit definition for SDIO_ICR register  *******************/
#define  SDIO_ICR_CCRCFAILC                  ((uint32_t)0x00000001)        /*!<CCRCFAIL flag clear bit */
#define  SDIO_ICR_DCRCFAILC                  ((uint32_t)0x00000002)        /*!<DCRCFAIL flag clear bit */
#define  SDIO_ICR_CTIMEOUTC                  ((uint32_t)0x00000004)        /*!<CTIMEOUT flag clear bit */
#define  SDIO_ICR_DTIMEOUTC                  ((uint32_t)0x00000008)        /*!<DTIMEOUT flag clear bit */
#define  SDIO_ICR_TXUNDERRC                  ((uint32_t)0x00000010)        /*!<TXUNDERR flag clear bit */
#define  SDIO_ICR_RXOVERRC                   ((uint32_t)0x00000020)        /*!<RXOVERR flag clear bit  */
#define  SDIO_ICR_CMDRENDC                   ((uint32_t)0x00000040)        /*!<CMDREND flag clear bit  */
#define  SDIO_ICR_CMDSENTC                   ((uint32_t)0x00000080)        /*!<CMDSENT flag clear bit  */
#define  SDIO_ICR_DATAENDC                   ((uint32_t)0x00000100)        /*!<DATAEND flag clear bit  */
#define  SDIO_ICR_STBITERRC                  ((uint32_t)0x00000200)        /*!<STBITERR flag clear bit */
#define  SDIO_ICR_DBCKENDC                   ((uint32_t)0x00000400)        /*!<DBCKEND flag clear bit  */
#define  SDIO_ICR_SDIOITC                    ((uint32_t)0x00400000)        /*!<SDIOIT flag clear bit   */
#define  SDIO_ICR_CEATAENDC                  ((uint32_t)0x00800000)        /*!<CEATAEND flag clear bit */

/******************  Bit definition for SDIO_MASK register  *******************/
#define  SDIO_MASK_CCRCFAILIE                ((uint32_t)0x00000001)        /*!<Command CRC Fail Interrupt Enable          */
#define  SDIO_MASK_DCRCFAILIE                ((uint32_t)0x00000002)        /*!<Data CRC Fail Interrupt Enable             */
#define  SDIO_MASK_CTIMEOUTIE                ((uint32_t)0x00000004)        /*!<Command TimeOut Interrupt Enable           */
#define  SDIO_MASK_DTIMEOUTIE                ((uint32_t)0x00000008)        /*!<Data TimeOut Interrupt Enable              */
#define  SDIO_MASK_TXUNDERRIE                ((uint32_t)0x00000010)        /*!<Tx FIFO UnderRun Error Interrupt Enable    */
#define  SDIO_MASK_RXOVERRIE                 ((uint32_t)0x00000020)        /*!<Rx FIFO OverRun Error Interrupt Enable     */
#define  SDIO_MASK_CMDRENDIE                 ((uint32_t)0x00000040)        /*!<Command Response Received Interrupt Enable */
#define  SDIO_MASK_CMDSENTIE                 ((uint32_t)0x00000080)        /*!<Command Sent Interrupt Enable              */
#define  SDIO_MASK_DATAENDIE                 ((uint32_t)0x00000100)        /*!<Data End Interrupt Enable                  */
#define  SDIO_MASK_STBITERRIE                ((uint32_t)0x00000200)        /*!<Start Bit Error Interrupt Enable           */
#define  SDIO_MASK_DBCKENDIE                 ((uint32_t)0x00000400)        /*!<Data Block End Interrupt Enable            */
#define  SDIO_MASK_CMDACTIE                  ((uint32_t)0x00000800)        /*!<CCommand Acting Interrupt Enable           */
#define  SDIO_MASK_TXACTIE                   ((uint32_t)0x00001000)        /*!<Data Transmit Acting Interrupt Enable      */
#define  SDIO_MASK_RXACTIE                   ((uint32_t)0x00002000)        /*!<Data receive acting interrupt enabled      */
#define  SDIO_MASK_TXFIFOHEIE                ((uint32_t)0x00004000)        /*!<Tx FIFO Half Empty interrupt Enable        */
#define  SDIO_MASK_RXFIFOHFIE                ((uint32_t)0x00008000)        /*!<Rx FIFO Half Full interrupt Enable         */
#define  SDIO_MASK_TXFIFOFIE                 ((uint32_t)0x00010000)        /*!<Tx FIFO Full interrupt Enable              */
#define  SDIO_MASK_RXFIFOFIE                 ((uint32_t)0x00020000)        /*!<Rx FIFO Full interrupt Enable              */
#define  SDIO_MASK_TXFIFOEIE                 ((uint32_t)0x00040000)        /*!<Tx FIFO Empty interrupt Enable             */
#define  SDIO_MASK_RXFIFOEIE                 ((uint32_t)0x00080000)        /*!<Rx FIFO Empty interrupt Enable             */
#define  SDIO_MASK_TXDAVLIE                  ((uint32_t)0x00100000)        /*!<Data available in Tx FIFO interrupt Enable */
#define  SDIO_MASK_RXDAVLIE                  ((uint32_t)0x00200000)        /*!<Data available in Rx FIFO interrupt Enable */
#define  SDIO_MASK_SDIOITIE                  ((uint32_t)0x00400000)        /*!<SDIO Mode Interrupt Received interrupt Enable */
#define  SDIO_MASK_CEATAENDIE                ((uint32_t)0x00800000)        /*!<CE-ATA command completion signal received Interrupt Enable */

/*****************  Bit definition for SDIO_FIFOCNT register  *****************/
#define  SDIO_FIFOCNT_FIFOCOUNT              ((uint32_t)0x00FFFFFF)        /*!<Remaining number of words to be written to or read from the FIFO */

/******************  Bit definition for SDIO_FIFO register  *******************/
#define  SDIO_FIFO_FIFODATA                  ((uint32_t)0xFFFFFFFF)        /*!<Receive and transmit FIFO data */

/******************************************************************************/
/*                                                                            */
/*                        Serial Peripheral Interface                         */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for SPI_CR1 register  ********************/
#define  SPI_CR1_CPHA                        ((uint16_t)0x0001)            /*!<Clock Phase      */
#define  SPI_CR1_CPOL                        ((uint16_t)0x0002)            /*!<Clock Polarity   */
#define  SPI_CR1_MSTR                        ((uint16_t)0x0004)            /*!<Master Selection */

#define  SPI_CR1_BR                          ((uint16_t)0x0038)            /*!<BR[2:0] bits (Baud Rate Control) */
#define  SPI_CR1_BR_0                        ((uint16_t)0x0008)            /*!<Bit 0 */
#define  SPI_CR1_BR_1                        ((uint16_t)0x0010)            /*!<Bit 1 */
#define  SPI_CR1_BR_2                        ((uint16_t)0x0020)            /*!<Bit 2 */

#define  SPI_CR1_SPE                         ((uint16_t)0x0040)            /*!<SPI Enable                          */
#define  SPI_CR1_LSBFIRST                    ((uint16_t)0x0080)            /*!<Frame Format                        */
#define  SPI_CR1_SSI                         ((uint16_t)0x0100)            /*!<Internal slave select               */
#define  SPI_CR1_SSM                         ((uint16_t)0x0200)            /*!<Software slave management           */
#define  SPI_CR1_RXONLY                      ((uint16_t)0x0400)            /*!<Receive only                        */
#define  SPI_CR1_DFF                         ((uint16_t)0x0800)            /*!<Data Frame Format                   */
#define  SPI_CR1_CRCNEXT                     ((uint16_t)0x1000)            /*!<Transmit CRC next                   */
#define  SPI_CR1_CRCEN                       ((uint16_t)0x2000)            /*!<Hardware CRC calculation enable     */
#define  SPI_CR1_BIDIOE                      ((uint16_t)0x4000)            /*!<Output enable in bidirectional mode */
#define  SPI_CR1_BIDIMODE                    ((uint16_t)0x8000)            /*!<Bidirectional data mode enable      */

/*******************  Bit definition for SPI_CR2 register  ********************/
#define  SPI_CR2_RXDMAEN                     ((uint8_t)0x01)               /*!<Rx Buffer DMA Enable                 */
#define  SPI_CR2_TXDMAEN                     ((uint8_t)0x02)               /*!<Tx Buffer DMA Enable                 */
#define  SPI_CR2_SSOE                        ((uint8_t)0x04)               /*!<SS Output Enable                     */
#define  SPI_CR2_ERRIE                       ((uint8_t)0x20)               /*!<Error Interrupt Enable               */
#define  SPI_CR2_RXNEIE                      ((uint8_t)0x40)               /*!<RX buffer Not Empty Interrupt Enable */
#define  SPI_CR2_TXEIE                       ((uint8_t)0x80)               /*!<Tx buffer Empty Interrupt Enable     */

/********************  Bit definition for SPI_SR register  ********************/
#define  SPI_SR_RXNE                         ((uint8_t)0x01)               /*!<Receive buffer Not Empty */
#define  SPI_SR_TXE                          ((uint8_t)0x02)               /*!<Transmit buffer Empty    */
#define  SPI_SR_CHSIDE                       ((uint8_t)0x04)               /*!<Channel side             */
#define  SPI_SR_UDR                          ((uint8_t)0x08)               /*!<Underrun flag            */
#define  SPI_SR_CRCERR                       ((uint8_t)0x10)               /*!<CRC Error flag           */
#define  SPI_SR_MODF                         ((uint8_t)0x20)               /*!<Mode fault               */
#define  SPI_SR_OVR                          ((uint8_t)0x40)               /*!<Overrun flag             */
#define  SPI_SR_BSY                          ((uint8_t)0x80)               /*!<Busy flag                */

/********************  Bit definition for SPI_DR register  ********************/
#define  SPI_DR_DR                           ((uint16_t)0xFFFF)            /*!<Data Register           */

/*******************  Bit definition for SPI_CRCPR register  ******************/
#define  SPI_CRCPR_CRCPOLY                   ((uint16_t)0xFFFF)            /*!<CRC polynomial register */

/******************  Bit definition for SPI_RXCRCR register  ******************/
#define  SPI_RXCRCR_RXCRC                    ((uint16_t)0xFFFF)            /*!<Rx CRC Register         */

/******************  Bit definition for SPI_TXCRCR register  ******************/
#define  SPI_TXCRCR_TXCRC                    ((uint16_t)0xFFFF)            /*!<Tx CRC Register         */

/******************  Bit definition for SPI_I2SCFGR register  *****************/
#define  SPI_I2SCFGR_CHLEN                   ((uint16_t)0x0001)            /*!<Channel length (number of bits per audio channel) */

#define  SPI_I2SCFGR_DATLEN                  ((uint16_t)0x0006)            /*!<DATLEN[1:0] bits (Data length to be transferred)  */
#define  SPI_I2SCFGR_DATLEN_0                ((uint16_t)0x0002)            /*!<Bit 0 */
#define  SPI_I2SCFGR_DATLEN_1                ((uint16_t)0x0004)            /*!<Bit 1 */

#define  SPI_I2SCFGR_CKPOL                   ((uint16_t)0x0008)            /*!<steady state clock polarity               */

#define  SPI_I2SCFGR_I2SSTD                  ((uint16_t)0x0030)            /*!<I2SSTD[1:0] bits (I2S standard selection) */
#define  SPI_I2SCFGR_I2SSTD_0                ((uint16_t)0x0010)            /*!<Bit 0 */
#define  SPI_I2SCFGR_I2SSTD_1                ((uint16_t)0x0020)            /*!<Bit 1 */

#define  SPI_I2SCFGR_PCMSYNC                 ((uint16_t)0x0080)            /*!<PCM frame synchronization                 */

#define  SPI_I2SCFGR_I2SCFG                  ((uint16_t)0x0300)            /*!<I2SCFG[1:0] bits (I2S configuration mode) */
#define  SPI_I2SCFGR_I2SCFG_0                ((uint16_t)0x0100)            /*!<Bit 0 */
#define  SPI_I2SCFGR_I2SCFG_1                ((uint16_t)0x0200)            /*!<Bit 1 */

#define  SPI_I2SCFGR_I2SE                    ((uint16_t)0x0400)            /*!<I2S Enable         */
#define  SPI_I2SCFGR_I2SMOD                  ((uint16_t)0x0800)            /*!<I2S mode selection */

/******************  Bit definition for SPI_I2SPR register  *******************/
#define  SPI_I2SPR_I2SDIV                    ((uint16_t)0x00FF)            /*!<I2S Linear prescaler         */
#define  SPI_I2SPR_ODD                       ((uint16_t)0x0100)            /*!<Odd factor for the prescaler */
#define  SPI_I2SPR_MCKOE                     ((uint16_t)0x0200)            /*!<Master Clock Output Enable   */

/******************************************************************************/
/*                                                                            */
/*                                 SYSCFG                                     */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for SYSCFG_MEMRMP register  ***************/  
#define SYSCFG_MEMRMP_MEM_MODE          ((uint32_t)0x00000007) /*!< SYSCFG_Memory Remap Config */
#define SYSCFG_MEMRMP_MEM_MODE_0        ((uint32_t)0x00000001) /*!<Bit 0 */
#define SYSCFG_MEMRMP_MEM_MODE_1        ((uint32_t)0x00000002) /*!<Bit 1 */
#define SYSCFG_MEMRMP_MEM_MODE_2        ((uint32_t)0x00000004) /*!<Bit 2 */

#define SYSCFG_MEMRMP_FB_MODE           ((uint32_t)0x00000100) /*!< User Flash Bank mode */

#define SYSCFG_MEMRMP_SWP_FMC           ((uint32_t)0x00000C00) /*!< FMC memory mapping swap */
#define SYSCFG_MEMRMP_SWP_FMC_0         ((uint32_t)0x00000400) /*!<Bit 0 */
#define SYSCFG_MEMRMP_SWP_FMC_1         ((uint32_t)0x00000800) /*!<Bit 1 */


/******************  Bit definition for SYSCFG_PMC register  ******************/
#define SYSCFG_PMC_ADCxDC2              ((uint32_t)0x00070000) /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC1DC2              ((uint32_t)0x00010000) /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC2DC2              ((uint32_t)0x00020000) /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC3DC2              ((uint32_t)0x00040000) /*!< Refer to AN4073 on how to use this bit  */

#define SYSCFG_PMC_MII_RMII_SEL         ((uint32_t)0x00800000) /*!<Ethernet PHY interface selection */
/* Old MII_RMII_SEL bit definition, maintained for legacy purpose */
#define SYSCFG_PMC_MII_RMII             SYSCFG_PMC_MII_RMII_SEL

/*****************  Bit definition for SYSCFG_EXTICR1 register  ***************/
#define SYSCFG_EXTICR1_EXTI0            ((uint16_t)0x000F) /*!<EXTI 0 configuration */
#define SYSCFG_EXTICR1_EXTI1            ((uint16_t)0x00F0) /*!<EXTI 1 configuration */
#define SYSCFG_EXTICR1_EXTI2            ((uint16_t)0x0F00) /*!<EXTI 2 configuration */
#define SYSCFG_EXTICR1_EXTI3            ((uint16_t)0xF000) /*!<EXTI 3 configuration */
/** 
  * @brief   EXTI0 configuration  
  */ 
#define SYSCFG_EXTICR1_EXTI0_PA         ((uint16_t)0x0000) /*!<PA[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PB         ((uint16_t)0x0001) /*!<PB[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PC         ((uint16_t)0x0002) /*!<PC[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PD         ((uint16_t)0x0003) /*!<PD[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PE         ((uint16_t)0x0004) /*!<PE[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PF         ((uint16_t)0x0005) /*!<PF[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PG         ((uint16_t)0x0006) /*!<PG[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PH         ((uint16_t)0x0007) /*!<PH[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PI         ((uint16_t)0x0008) /*!<PI[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PJ         ((uint16_t)0x0009) /*!<PJ[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PK         ((uint16_t)0x000A) /*!<PK[0] pin */

/** 
  * @brief   EXTI1 configuration  
  */ 
#define SYSCFG_EXTICR1_EXTI1_PA         ((uint16_t)0x0000) /*!<PA[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PB         ((uint16_t)0x0010) /*!<PB[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PC         ((uint16_t)0x0020) /*!<PC[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PD         ((uint16_t)0x0030) /*!<PD[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PE         ((uint16_t)0x0040) /*!<PE[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PF         ((uint16_t)0x0050) /*!<PF[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PG         ((uint16_t)0x0060) /*!<PG[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PH         ((uint16_t)0x0070) /*!<PH[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PI         ((uint16_t)0x0080) /*!<PI[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PJ         ((uint16_t)0x0090) /*!<PJ[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PK         ((uint16_t)0x00A0) /*!<PK[1] pin */

/** 
  * @brief   EXTI2 configuration  
  */ 
#define SYSCFG_EXTICR1_EXTI2_PA         ((uint16_t)0x0000) /*!<PA[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PB         ((uint16_t)0x0100) /*!<PB[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PC         ((uint16_t)0x0200) /*!<PC[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PD         ((uint16_t)0x0300) /*!<PD[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PE         ((uint16_t)0x0400) /*!<PE[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PF         ((uint16_t)0x0500) /*!<PF[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PG         ((uint16_t)0x0600) /*!<PG[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PH         ((uint16_t)0x0700) /*!<PH[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PI         ((uint16_t)0x0800) /*!<PI[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PJ         ((uint16_t)0x0900) /*!<PJ[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PK         ((uint16_t)0x0A00) /*!<PK[2] pin */

/** 
  * @brief   EXTI3 configuration  
  */ 
#define SYSCFG_EXTICR1_EXTI3_PA         ((uint16_t)0x0000) /*!<PA[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PB         ((uint16_t)0x1000) /*!<PB[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PC         ((uint16_t)0x2000) /*!<PC[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PD         ((uint16_t)0x3000) /*!<PD[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PE         ((uint16_t)0x4000) /*!<PE[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PF         ((uint16_t)0x5000) /*!<PF[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PG         ((uint16_t)0x6000) /*!<PG[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PH         ((uint16_t)0x7000) /*!<PH[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PI         ((uint16_t)0x8000) /*!<PI[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PJ         ((uint16_t)0x9000) /*!<PJ[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PK         ((uint16_t)0xA000) /*!<PK[3] pin */

/*****************  Bit definition for SYSCFG_EXTICR2 register  ***************/
#define SYSCFG_EXTICR2_EXTI4            ((uint16_t)0x000F) /*!<EXTI 4 configuration */
#define SYSCFG_EXTICR2_EXTI5            ((uint16_t)0x00F0) /*!<EXTI 5 configuration */
#define SYSCFG_EXTICR2_EXTI6            ((uint16_t)0x0F00) /*!<EXTI 6 configuration */
#define SYSCFG_EXTICR2_EXTI7            ((uint16_t)0xF000) /*!<EXTI 7 configuration */
/** 
  * @brief   EXTI4 configuration  
  */ 
#define SYSCFG_EXTICR2_EXTI4_PA         ((uint16_t)0x0000) /*!<PA[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PB         ((uint16_t)0x0001) /*!<PB[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PC         ((uint16_t)0x0002) /*!<PC[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PD         ((uint16_t)0x0003) /*!<PD[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PE         ((uint16_t)0x0004) /*!<PE[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PF         ((uint16_t)0x0005) /*!<PF[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PG         ((uint16_t)0x0006) /*!<PG[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PH         ((uint16_t)0x0007) /*!<PH[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PI         ((uint16_t)0x0008) /*!<PI[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PJ         ((uint16_t)0x0009) /*!<PJ[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PK         ((uint16_t)0x000A) /*!<PK[4] pin */

/** 
  * @brief   EXTI5 configuration  
  */ 
#define SYSCFG_EXTICR2_EXTI5_PA         ((uint16_t)0x0000) /*!<PA[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PB         ((uint16_t)0x0010) /*!<PB[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PC         ((uint16_t)0x0020) /*!<PC[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PD         ((uint16_t)0x0030) /*!<PD[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PE         ((uint16_t)0x0040) /*!<PE[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PF         ((uint16_t)0x0050) /*!<PF[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PG         ((uint16_t)0x0060) /*!<PG[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PH         ((uint16_t)0x0070) /*!<PH[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PI         ((uint16_t)0x0080) /*!<PI[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PJ         ((uint16_t)0x0090) /*!<PJ[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PK         ((uint16_t)0x00A0) /*!<PK[5] pin */

/** 
  * @brief   EXTI6 configuration  
  */ 
#define SYSCFG_EXTICR2_EXTI6_PA         ((uint16_t)0x0000) /*!<PA[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PB         ((uint16_t)0x0100) /*!<PB[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PC         ((uint16_t)0x0200) /*!<PC[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PD         ((uint16_t)0x0300) /*!<PD[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PE         ((uint16_t)0x0400) /*!<PE[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PF         ((uint16_t)0x0500) /*!<PF[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PG         ((uint16_t)0x0600) /*!<PG[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PH         ((uint16_t)0x0700) /*!<PH[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PI         ((uint16_t)0x0800) /*!<PI[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PJ         ((uint16_t)0x0900) /*!<PJ[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PK         ((uint16_t)0x0A00) /*!<PK[6] pin */

/** 
  * @brief   EXTI7 configuration  
  */ 
#define SYSCFG_EXTICR2_EXTI7_PA         ((uint16_t)0x0000) /*!<PA[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PB         ((uint16_t)0x1000) /*!<PB[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PC         ((uint16_t)0x2000) /*!<PC[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PD         ((uint16_t)0x3000) /*!<PD[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PE         ((uint16_t)0x4000) /*!<PE[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PF         ((uint16_t)0x5000) /*!<PF[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PG         ((uint16_t)0x6000) /*!<PG[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PH         ((uint16_t)0x7000) /*!<PH[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PI         ((uint16_t)0x8000) /*!<PI[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PJ         ((uint16_t)0x9000) /*!<PJ[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PK         ((uint16_t)0xA000) /*!<PK[7] pin */

/*****************  Bit definition for SYSCFG_EXTICR3 register  ***************/
#define SYSCFG_EXTICR3_EXTI8            ((uint16_t)0x000F) /*!<EXTI 8 configuration */
#define SYSCFG_EXTICR3_EXTI9            ((uint16_t)0x00F0) /*!<EXTI 9 configuration */
#define SYSCFG_EXTICR3_EXTI10           ((uint16_t)0x0F00) /*!<EXTI 10 configuration */
#define SYSCFG_EXTICR3_EXTI11           ((uint16_t)0xF000) /*!<EXTI 11 configuration */
           
/** 
  * @brief   EXTI8 configuration  
  */ 
#define SYSCFG_EXTICR3_EXTI8_PA         ((uint16_t)0x0000) /*!<PA[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PB         ((uint16_t)0x0001) /*!<PB[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PC         ((uint16_t)0x0002) /*!<PC[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PD         ((uint16_t)0x0003) /*!<PD[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PE         ((uint16_t)0x0004) /*!<PE[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PF         ((uint16_t)0x0005) /*!<PF[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PG         ((uint16_t)0x0006) /*!<PG[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PH         ((uint16_t)0x0007) /*!<PH[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PI         ((uint16_t)0x0008) /*!<PI[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PJ         ((uint16_t)0x0009) /*!<PJ[8] pin */

/** 
  * @brief   EXTI9 configuration  
  */ 
#define SYSCFG_EXTICR3_EXTI9_PA         ((uint16_t)0x0000) /*!<PA[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PB         ((uint16_t)0x0010) /*!<PB[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PC         ((uint16_t)0x0020) /*!<PC[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PD         ((uint16_t)0x0030) /*!<PD[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PE         ((uint16_t)0x0040) /*!<PE[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PF         ((uint16_t)0x0050) /*!<PF[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PG         ((uint16_t)0x0060) /*!<PG[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PH         ((uint16_t)0x0070) /*!<PH[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PI         ((uint16_t)0x0080) /*!<PI[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PJ         ((uint16_t)0x0090) /*!<PJ[9] pin */

/** 
  * @brief   EXTI10 configuration  
  */ 
#define SYSCFG_EXTICR3_EXTI10_PA        ((uint16_t)0x0000) /*!<PA[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PB        ((uint16_t)0x0100) /*!<PB[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PC        ((uint16_t)0x0200) /*!<PC[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PD        ((uint16_t)0x0300) /*!<PD[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PE        ((uint16_t)0x0400) /*!<PE[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PF        ((uint16_t)0x0500) /*!<PF[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PG        ((uint16_t)0x0600) /*!<PG[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PH        ((uint16_t)0x0700) /*!<PH[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PI        ((uint16_t)0x0800) /*!<PI[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PJ        ((uint16_t)0x0900) /*!<PJ[10] pin */

/** 
  * @brief   EXTI11 configuration  
  */ 
#define SYSCFG_EXTICR3_EXTI11_PA        ((uint16_t)0x0000) /*!<PA[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PB        ((uint16_t)0x1000) /*!<PB[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PC        ((uint16_t)0x2000) /*!<PC[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PD        ((uint16_t)0x3000) /*!<PD[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PE        ((uint16_t)0x4000) /*!<PE[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PF        ((uint16_t)0x5000) /*!<PF[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PG        ((uint16_t)0x6000) /*!<PG[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PH        ((uint16_t)0x7000) /*!<PH[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PI        ((uint16_t)0x8000) /*!<PI[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PJ        ((uint16_t)0x9000) /*!<PJ[11] pin */

/*****************  Bit definition for SYSCFG_EXTICR4 register  ***************/
#define SYSCFG_EXTICR4_EXTI12           ((uint16_t)0x000F) /*!<EXTI 12 configuration */
#define SYSCFG_EXTICR4_EXTI13           ((uint16_t)0x00F0) /*!<EXTI 13 configuration */
#define SYSCFG_EXTICR4_EXTI14           ((uint16_t)0x0F00) /*!<EXTI 14 configuration */
#define SYSCFG_EXTICR4_EXTI15           ((uint16_t)0xF000) /*!<EXTI 15 configuration */
/** 
  * @brief   EXTI12 configuration  
  */ 
#define SYSCFG_EXTICR4_EXTI12_PA        ((uint16_t)0x0000) /*!<PA[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PB        ((uint16_t)0x0001) /*!<PB[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PC        ((uint16_t)0x0002) /*!<PC[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PD        ((uint16_t)0x0003) /*!<PD[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PE        ((uint16_t)0x0004) /*!<PE[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PF        ((uint16_t)0x0005) /*!<PF[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PG        ((uint16_t)0x0006) /*!<PG[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PH        ((uint16_t)0x0007) /*!<PH[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PI        ((uint16_t)0x0008) /*!<PI[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PJ        ((uint16_t)0x0009) /*!<PJ[12] pin */

/** 
  * @brief   EXTI13 configuration  
  */ 
#define SYSCFG_EXTICR4_EXTI13_PA        ((uint16_t)0x0000) /*!<PA[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PB        ((uint16_t)0x0010) /*!<PB[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PC        ((uint16_t)0x0020) /*!<PC[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PD        ((uint16_t)0x0030) /*!<PD[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PE        ((uint16_t)0x0040) /*!<PE[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PF        ((uint16_t)0x0050) /*!<PF[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PG        ((uint16_t)0x0060) /*!<PG[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PH        ((uint16_t)0x0070) /*!<PH[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PI        ((uint16_t)0x0008) /*!<PI[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PJ        ((uint16_t)0x0009) /*!<PJ[13] pin */

/** 
  * @brief   EXTI14 configuration  
  */ 
#define SYSCFG_EXTICR4_EXTI14_PA        ((uint16_t)0x0000) /*!<PA[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PB        ((uint16_t)0x0100) /*!<PB[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PC        ((uint16_t)0x0200) /*!<PC[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PD        ((uint16_t)0x0300) /*!<PD[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PE        ((uint16_t)0x0400) /*!<PE[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PF        ((uint16_t)0x0500) /*!<PF[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PG        ((uint16_t)0x0600) /*!<PG[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PH        ((uint16_t)0x0700) /*!<PH[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PI        ((uint16_t)0x0800) /*!<PI[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PJ        ((uint16_t)0x0900) /*!<PJ[14] pin */

/** 
  * @brief   EXTI15 configuration  
  */ 
#define SYSCFG_EXTICR4_EXTI15_PA        ((uint16_t)0x0000) /*!<PA[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PB        ((uint16_t)0x1000) /*!<PB[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PC        ((uint16_t)0x2000) /*!<PC[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PD        ((uint16_t)0x3000) /*!<PD[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PE        ((uint16_t)0x4000) /*!<PE[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PF        ((uint16_t)0x5000) /*!<PF[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PG        ((uint16_t)0x6000) /*!<PG[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PH        ((uint16_t)0x7000) /*!<PH[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PI        ((uint16_t)0x8000) /*!<PI[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PJ        ((uint16_t)0x9000) /*!<PJ[15] pin */

/******************  Bit definition for SYSCFG_CMPCR register  ****************/  
#define SYSCFG_CMPCR_CMP_PD             ((uint32_t)0x00000001) /*!<Compensation cell ready flag */
#define SYSCFG_CMPCR_READY              ((uint32_t)0x00000100) /*!<Compensation cell power-down */

/******************************************************************************/
/*                                                                            */
/*                                    TIM                                     */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for TIM_CR1 register  ********************/
#define  TIM_CR1_CEN                         ((uint16_t)0x0001)            /*!<Counter enable        */
#define  TIM_CR1_UDIS                        ((uint16_t)0x0002)            /*!<Update disable        */
#define  TIM_CR1_URS                         ((uint16_t)0x0004)            /*!<Update request source */
#define  TIM_CR1_OPM                         ((uint16_t)0x0008)            /*!<One pulse mode        */
#define  TIM_CR1_DIR                         ((uint16_t)0x0010)            /*!<Direction             */

#define  TIM_CR1_CMS                         ((uint16_t)0x0060)            /*!<CMS[1:0] bits (Center-aligned mode selection) */
#define  TIM_CR1_CMS_0                       ((uint16_t)0x0020)            /*!<Bit 0 */
#define  TIM_CR1_CMS_1                       ((uint16_t)0x0040)            /*!<Bit 1 */

#define  TIM_CR1_ARPE                        ((uint16_t)0x0080)            /*!<Auto-reload preload enable     */

#define  TIM_CR1_CKD                         ((uint16_t)0x0300)            /*!<CKD[1:0] bits (clock division) */
#define  TIM_CR1_CKD_0                       ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_CR1_CKD_1                       ((uint16_t)0x0200)            /*!<Bit 1 */

/*******************  Bit definition for TIM_CR2 register  ********************/
#define  TIM_CR2_CCPC                        ((uint16_t)0x0001)            /*!<Capture/Compare Preloaded Control        */
#define  TIM_CR2_CCUS                        ((uint16_t)0x0004)            /*!<Capture/Compare Control Update Selection */
#define  TIM_CR2_CCDS                        ((uint16_t)0x0008)            /*!<Capture/Compare DMA Selection            */

#define  TIM_CR2_MMS                         ((uint16_t)0x0070)            /*!<MMS[2:0] bits (Master Mode Selection) */
#define  TIM_CR2_MMS_0                       ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CR2_MMS_1                       ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CR2_MMS_2                       ((uint16_t)0x0040)            /*!<Bit 2 */

#define  TIM_CR2_TI1S                        ((uint16_t)0x0080)            /*!<TI1 Selection */
#define  TIM_CR2_OIS1                        ((uint16_t)0x0100)            /*!<Output Idle state 1 (OC1 output)  */
#define  TIM_CR2_OIS1N                       ((uint16_t)0x0200)            /*!<Output Idle state 1 (OC1N output) */
#define  TIM_CR2_OIS2                        ((uint16_t)0x0400)            /*!<Output Idle state 2 (OC2 output)  */
#define  TIM_CR2_OIS2N                       ((uint16_t)0x0800)            /*!<Output Idle state 2 (OC2N output) */
#define  TIM_CR2_OIS3                        ((uint16_t)0x1000)            /*!<Output Idle state 3 (OC3 output)  */
#define  TIM_CR2_OIS3N                       ((uint16_t)0x2000)            /*!<Output Idle state 3 (OC3N output) */
#define  TIM_CR2_OIS4                        ((uint16_t)0x4000)            /*!<Output Idle state 4 (OC4 output)  */

/*******************  Bit definition for TIM_SMCR register  *******************/
#define  TIM_SMCR_SMS                        ((uint16_t)0x0007)            /*!<SMS[2:0] bits (Slave mode selection)    */
#define  TIM_SMCR_SMS_0                      ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_SMCR_SMS_1                      ((uint16_t)0x0002)            /*!<Bit 1 */
#define  TIM_SMCR_SMS_2                      ((uint16_t)0x0004)            /*!<Bit 2 */

#define  TIM_SMCR_TS                         ((uint16_t)0x0070)            /*!<TS[2:0] bits (Trigger selection)        */
#define  TIM_SMCR_TS_0                       ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_SMCR_TS_1                       ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_SMCR_TS_2                       ((uint16_t)0x0040)            /*!<Bit 2 */

#define  TIM_SMCR_MSM                        ((uint16_t)0x0080)            /*!<Master/slave mode                       */

#define  TIM_SMCR_ETF                        ((uint16_t)0x0F00)            /*!<ETF[3:0] bits (External trigger filter) */
#define  TIM_SMCR_ETF_0                      ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_SMCR_ETF_1                      ((uint16_t)0x0200)            /*!<Bit 1 */
#define  TIM_SMCR_ETF_2                      ((uint16_t)0x0400)            /*!<Bit 2 */
#define  TIM_SMCR_ETF_3                      ((uint16_t)0x0800)            /*!<Bit 3 */

#define  TIM_SMCR_ETPS                       ((uint16_t)0x3000)            /*!<ETPS[1:0] bits (External trigger prescaler) */
#define  TIM_SMCR_ETPS_0                     ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_SMCR_ETPS_1                     ((uint16_t)0x2000)            /*!<Bit 1 */

#define  TIM_SMCR_ECE                        ((uint16_t)0x4000)            /*!<External clock enable     */
#define  TIM_SMCR_ETP                        ((uint16_t)0x8000)            /*!<External trigger polarity */

/*******************  Bit definition for TIM_DIER register  *******************/
#define  TIM_DIER_UIE                        ((uint16_t)0x0001)            /*!<Update interrupt enable */
#define  TIM_DIER_CC1IE                      ((uint16_t)0x0002)            /*!<Capture/Compare 1 interrupt enable   */
#define  TIM_DIER_CC2IE                      ((uint16_t)0x0004)            /*!<Capture/Compare 2 interrupt enable   */
#define  TIM_DIER_CC3IE                      ((uint16_t)0x0008)            /*!<Capture/Compare 3 interrupt enable   */
#define  TIM_DIER_CC4IE                      ((uint16_t)0x0010)            /*!<Capture/Compare 4 interrupt enable   */
#define  TIM_DIER_COMIE                      ((uint16_t)0x0020)            /*!<COM interrupt enable                 */
#define  TIM_DIER_TIE                        ((uint16_t)0x0040)            /*!<Trigger interrupt enable             */
#define  TIM_DIER_BIE                        ((uint16_t)0x0080)            /*!<Break interrupt enable               */
#define  TIM_DIER_UDE                        ((uint16_t)0x0100)            /*!<Update DMA request enable            */
#define  TIM_DIER_CC1DE                      ((uint16_t)0x0200)            /*!<Capture/Compare 1 DMA request enable */
#define  TIM_DIER_CC2DE                      ((uint16_t)0x0400)            /*!<Capture/Compare 2 DMA request enable */
#define  TIM_DIER_CC3DE                      ((uint16_t)0x0800)            /*!<Capture/Compare 3 DMA request enable */
#define  TIM_DIER_CC4DE                      ((uint16_t)0x1000)            /*!<Capture/Compare 4 DMA request enable */
#define  TIM_DIER_COMDE                      ((uint16_t)0x2000)            /*!<COM DMA request enable               */
#define  TIM_DIER_TDE                        ((uint16_t)0x4000)            /*!<Trigger DMA request enable           */

/********************  Bit definition for TIM_SR register  ********************/
#define  TIM_SR_UIF                          ((uint16_t)0x0001)            /*!<Update interrupt Flag              */
#define  TIM_SR_CC1IF                        ((uint16_t)0x0002)            /*!<Capture/Compare 1 interrupt Flag   */
#define  TIM_SR_CC2IF                        ((uint16_t)0x0004)            /*!<Capture/Compare 2 interrupt Flag   */
#define  TIM_SR_CC3IF                        ((uint16_t)0x0008)            /*!<Capture/Compare 3 interrupt Flag   */
#define  TIM_SR_CC4IF                        ((uint16_t)0x0010)            /*!<Capture/Compare 4 interrupt Flag   */
#define  TIM_SR_COMIF                        ((uint16_t)0x0020)            /*!<COM interrupt Flag                 */
#define  TIM_SR_TIF                          ((uint16_t)0x0040)            /*!<Trigger interrupt Flag             */
#define  TIM_SR_BIF                          ((uint16_t)0x0080)            /*!<Break interrupt Flag               */
#define  TIM_SR_CC1OF                        ((uint16_t)0x0200)            /*!<Capture/Compare 1 Overcapture Flag */
#define  TIM_SR_CC2OF                        ((uint16_t)0x0400)            /*!<Capture/Compare 2 Overcapture Flag */
#define  TIM_SR_CC3OF                        ((uint16_t)0x0800)            /*!<Capture/Compare 3 Overcapture Flag */
#define  TIM_SR_CC4OF                        ((uint16_t)0x1000)            /*!<Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM_EGR register  ********************/
#define  TIM_EGR_UG                          ((uint8_t)0x01)               /*!<Update Generation                         */
#define  TIM_EGR_CC1G                        ((uint8_t)0x02)               /*!<Capture/Compare 1 Generation              */
#define  TIM_EGR_CC2G                        ((uint8_t)0x04)               /*!<Capture/Compare 2 Generation              */
#define  TIM_EGR_CC3G                        ((uint8_t)0x08)               /*!<Capture/Compare 3 Generation              */
#define  TIM_EGR_CC4G                        ((uint8_t)0x10)               /*!<Capture/Compare 4 Generation              */
#define  TIM_EGR_COMG                        ((uint8_t)0x20)               /*!<Capture/Compare Control Update Generation */
#define  TIM_EGR_TG                          ((uint8_t)0x40)               /*!<Trigger Generation                        */
#define  TIM_EGR_BG                          ((uint8_t)0x80)               /*!<Break Generation                          */

/******************  Bit definition for TIM_CCMR1 register  *******************/
#define  TIM_CCMR1_CC1S                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define  TIM_CCMR1_CC1S_0                    ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_CCMR1_CC1S_1                    ((uint16_t)0x0002)            /*!<Bit 1 */

#define  TIM_CCMR1_OC1FE                     ((uint16_t)0x0004)            /*!<Output Compare 1 Fast enable                 */
#define  TIM_CCMR1_OC1PE                     ((uint16_t)0x0008)            /*!<Output Compare 1 Preload enable              */

#define  TIM_CCMR1_OC1M                      ((uint16_t)0x0070)            /*!<OC1M[2:0] bits (Output Compare 1 Mode)       */
#define  TIM_CCMR1_OC1M_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR1_OC1M_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR1_OC1M_2                    ((uint16_t)0x0040)            /*!<Bit 2 */

#define  TIM_CCMR1_OC1CE                     ((uint16_t)0x0080)            /*!<Output Compare 1Clear Enable                 */

#define  TIM_CCMR1_CC2S                      ((uint16_t)0x0300)            /*!<CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define  TIM_CCMR1_CC2S_0                    ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_CCMR1_CC2S_1                    ((uint16_t)0x0200)            /*!<Bit 1 */

#define  TIM_CCMR1_OC2FE                     ((uint16_t)0x0400)            /*!<Output Compare 2 Fast enable                 */
#define  TIM_CCMR1_OC2PE                     ((uint16_t)0x0800)            /*!<Output Compare 2 Preload enable              */

#define  TIM_CCMR1_OC2M                      ((uint16_t)0x7000)            /*!<OC2M[2:0] bits (Output Compare 2 Mode)       */
#define  TIM_CCMR1_OC2M_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR1_OC2M_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR1_OC2M_2                    ((uint16_t)0x4000)            /*!<Bit 2 */

#define  TIM_CCMR1_OC2CE                     ((uint16_t)0x8000)            /*!<Output Compare 2 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR1_IC1PSC                    ((uint16_t)0x000C)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define  TIM_CCMR1_IC1PSC_0                  ((uint16_t)0x0004)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1PSC_1                  ((uint16_t)0x0008)            /*!<Bit 1 */

#define  TIM_CCMR1_IC1F                      ((uint16_t)0x00F0)            /*!<IC1F[3:0] bits (Input Capture 1 Filter)      */
#define  TIM_CCMR1_IC1F_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1F_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR1_IC1F_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define  TIM_CCMR1_IC1F_3                    ((uint16_t)0x0080)            /*!<Bit 3 */

#define  TIM_CCMR1_IC2PSC                    ((uint16_t)0x0C00)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler)  */
#define  TIM_CCMR1_IC2PSC_0                  ((uint16_t)0x0400)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2PSC_1                  ((uint16_t)0x0800)            /*!<Bit 1 */

#define  TIM_CCMR1_IC2F                      ((uint16_t)0xF000)            /*!<IC2F[3:0] bits (Input Capture 2 Filter)       */
#define  TIM_CCMR1_IC2F_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2F_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR1_IC2F_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define  TIM_CCMR1_IC2F_3                    ((uint16_t)0x8000)            /*!<Bit 3 */

/******************  Bit definition for TIM_CCMR2 register  *******************/
#define  TIM_CCMR2_CC3S                      ((uint16_t)0x0003)            /*!<CC3S[1:0] bits (Capture/Compare 3 Selection)  */
#define  TIM_CCMR2_CC3S_0                    ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_CCMR2_CC3S_1                    ((uint16_t)0x0002)            /*!<Bit 1 */

#define  TIM_CCMR2_OC3FE                     ((uint16_t)0x0004)            /*!<Output Compare 3 Fast enable           */
#define  TIM_CCMR2_OC3PE                     ((uint16_t)0x0008)            /*!<Output Compare 3 Preload enable        */

#define  TIM_CCMR2_OC3M                      ((uint16_t)0x0070)            /*!<OC3M[2:0] bits (Output Compare 3 Mode) */
#define  TIM_CCMR2_OC3M_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR2_OC3M_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR2_OC3M_2                    ((uint16_t)0x0040)            /*!<Bit 2 */

#define  TIM_CCMR2_OC3CE                     ((uint16_t)0x0080)            /*!<Output Compare 3 Clear Enable */

#define  TIM_CCMR2_CC4S                      ((uint16_t)0x0300)            /*!<CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define  TIM_CCMR2_CC4S_0                    ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_CCMR2_CC4S_1                    ((uint16_t)0x0200)            /*!<Bit 1 */

#define  TIM_CCMR2_OC4FE                     ((uint16_t)0x0400)            /*!<Output Compare 4 Fast enable    */
#define  TIM_CCMR2_OC4PE                     ((uint16_t)0x0800)            /*!<Output Compare 4 Preload enable */

#define  TIM_CCMR2_OC4M                      ((uint16_t)0x7000)            /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_CCMR2_OC4M_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR2_OC4M_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR2_OC4M_2                    ((uint16_t)0x4000)            /*!<Bit 2 */

#define  TIM_CCMR2_OC4CE                     ((uint16_t)0x8000)            /*!<Output Compare 4 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR2_IC3PSC                    ((uint16_t)0x000C)            /*!<IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define  TIM_CCMR2_IC3PSC_0                  ((uint16_t)0x0004)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3PSC_1                  ((uint16_t)0x0008)            /*!<Bit 1 */

#define  TIM_CCMR2_IC3F                      ((uint16_t)0x00F0)            /*!<IC3F[3:0] bits (Input Capture 3 Filter) */
#define  TIM_CCMR2_IC3F_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3F_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR2_IC3F_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define  TIM_CCMR2_IC3F_3                    ((uint16_t)0x0080)            /*!<Bit 3 */

#define  TIM_CCMR2_IC4PSC                    ((uint16_t)0x0C00)            /*!<IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define  TIM_CCMR2_IC4PSC_0                  ((uint16_t)0x0400)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4PSC_1                  ((uint16_t)0x0800)            /*!<Bit 1 */

#define  TIM_CCMR2_IC4F                      ((uint16_t)0xF000)            /*!<IC4F[3:0] bits (Input Capture 4 Filter) */
#define  TIM_CCMR2_IC4F_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4F_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR2_IC4F_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define  TIM_CCMR2_IC4F_3                    ((uint16_t)0x8000)            /*!<Bit 3 */

/*******************  Bit definition for TIM_CCER register  *******************/
#define  TIM_CCER_CC1E                       ((uint16_t)0x0001)            /*!<Capture/Compare 1 output enable                 */
#define  TIM_CCER_CC1P                       ((uint16_t)0x0002)            /*!<Capture/Compare 1 output Polarity               */
#define  TIM_CCER_CC1NE                      ((uint16_t)0x0004)            /*!<Capture/Compare 1 Complementary output enable   */
#define  TIM_CCER_CC1NP                      ((uint16_t)0x0008)            /*!<Capture/Compare 1 Complementary output Polarity */
#define  TIM_CCER_CC2E                       ((uint16_t)0x0010)            /*!<Capture/Compare 2 output enable                 */
#define  TIM_CCER_CC2P                       ((uint16_t)0x0020)            /*!<Capture/Compare 2 output Polarity               */
#define  TIM_CCER_CC2NE                      ((uint16_t)0x0040)            /*!<Capture/Compare 2 Complementary output enable   */
#define  TIM_CCER_CC2NP                      ((uint16_t)0x0080)            /*!<Capture/Compare 2 Complementary output Polarity */
#define  TIM_CCER_CC3E                       ((uint16_t)0x0100)            /*!<Capture/Compare 3 output enable                 */
#define  TIM_CCER_CC3P                       ((uint16_t)0x0200)            /*!<Capture/Compare 3 output Polarity               */
#define  TIM_CCER_CC3NE                      ((uint16_t)0x0400)            /*!<Capture/Compare 3 Complementary output enable   */
#define  TIM_CCER_CC3NP                      ((uint16_t)0x0800)            /*!<Capture/Compare 3 Complementary output Polarity */
#define  TIM_CCER_CC4E                       ((uint16_t)0x1000)            /*!<Capture/Compare 4 output enable                 */
#define  TIM_CCER_CC4P                       ((uint16_t)0x2000)            /*!<Capture/Compare 4 output Polarity               */
#define  TIM_CCER_CC4NP                      ((uint16_t)0x8000)            /*!<Capture/Compare 4 Complementary output Polarity */

/*******************  Bit definition for TIM_CNT register  ********************/
#define  TIM_CNT_CNT                         ((uint16_t)0xFFFF)            /*!<Counter Value            */

/*******************  Bit definition for TIM_PSC register  ********************/
#define  TIM_PSC_PSC                         ((uint16_t)0xFFFF)            /*!<Prescaler Value          */

/*******************  Bit definition for TIM_ARR register  ********************/
#define  TIM_ARR_ARR                         ((uint16_t)0xFFFF)            /*!<actual auto-reload Value */

/*******************  Bit definition for TIM_RCR register  ********************/
#define  TIM_RCR_REP                         ((uint8_t)0xFF)               /*!<Repetition Counter Value */

/*******************  Bit definition for TIM_CCR1 register  *******************/
#define  TIM_CCR1_CCR1                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 1 Value  */

/*******************  Bit definition for TIM_CCR2 register  *******************/
#define  TIM_CCR2_CCR2                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 2 Value  */

/*******************  Bit definition for TIM_CCR3 register  *******************/
#define  TIM_CCR3_CCR3                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 3 Value  */

/*******************  Bit definition for TIM_CCR4 register  *******************/
#define  TIM_CCR4_CCR4                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 4 Value  */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define  TIM_BDTR_DTG                        ((uint16_t)0x00FF)            /*!<DTG[0:7] bits (Dead-Time Generator set-up) */
#define  TIM_BDTR_DTG_0                      ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_BDTR_DTG_1                      ((uint16_t)0x0002)            /*!<Bit 1 */
#define  TIM_BDTR_DTG_2                      ((uint16_t)0x0004)            /*!<Bit 2 */
#define  TIM_BDTR_DTG_3                      ((uint16_t)0x0008)            /*!<Bit 3 */
#define  TIM_BDTR_DTG_4                      ((uint16_t)0x0010)            /*!<Bit 4 */
#define  TIM_BDTR_DTG_5                      ((uint16_t)0x0020)            /*!<Bit 5 */
#define  TIM_BDTR_DTG_6                      ((uint16_t)0x0040)            /*!<Bit 6 */
#define  TIM_BDTR_DTG_7                      ((uint16_t)0x0080)            /*!<Bit 7 */

#define  TIM_BDTR_LOCK                       ((uint16_t)0x0300)            /*!<LOCK[1:0] bits (Lock Configuration) */
#define  TIM_BDTR_LOCK_0                     ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_BDTR_LOCK_1                     ((uint16_t)0x0200)            /*!<Bit 1 */

#define  TIM_BDTR_OSSI                       ((uint16_t)0x0400)            /*!<Off-State Selection for Idle mode */
#define  TIM_BDTR_OSSR                       ((uint16_t)0x0800)            /*!<Off-State Selection for Run mode  */
#define  TIM_BDTR_BKE                        ((uint16_t)0x1000)            /*!<Break enable                      */
#define  TIM_BDTR_BKP                        ((uint16_t)0x2000)            /*!<Break Polarity                    */
#define  TIM_BDTR_AOE                        ((uint16_t)0x4000)            /*!<Automatic Output enable           */
#define  TIM_BDTR_MOE                        ((uint16_t)0x8000)            /*!<Main Output enable                */

/*******************  Bit definition for TIM_DCR register  ********************/
#define  TIM_DCR_DBA                         ((uint16_t)0x001F)            /*!<DBA[4:0] bits (DMA Base Address) */
#define  TIM_DCR_DBA_0                       ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_DCR_DBA_1                       ((uint16_t)0x0002)            /*!<Bit 1 */
#define  TIM_DCR_DBA_2                       ((uint16_t)0x0004)            /*!<Bit 2 */
#define  TIM_DCR_DBA_3                       ((uint16_t)0x0008)            /*!<Bit 3 */
#define  TIM_DCR_DBA_4                       ((uint16_t)0x0010)            /*!<Bit 4 */

#define  TIM_DCR_DBL                         ((uint16_t)0x1F00)            /*!<DBL[4:0] bits (DMA Burst Length) */
#define  TIM_DCR_DBL_0                       ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_DCR_DBL_1                       ((uint16_t)0x0200)            /*!<Bit 1 */
#define  TIM_DCR_DBL_2                       ((uint16_t)0x0400)            /*!<Bit 2 */
#define  TIM_DCR_DBL_3                       ((uint16_t)0x0800)            /*!<Bit 3 */
#define  TIM_DCR_DBL_4                       ((uint16_t)0x1000)            /*!<Bit 4 */

/*******************  Bit definition for TIM_DMAR register  *******************/
#define  TIM_DMAR_DMAB                       ((uint16_t)0xFFFF)            /*!<DMA register for burst accesses                    */

/*******************  Bit definition for TIM_OR register  *********************/
#define TIM_OR_TI4_RMP                       ((uint16_t)0x00C0)            /*!<TI4_RMP[1:0] bits (TIM5 Input 4 remap)             */
#define TIM_OR_TI4_RMP_0                     ((uint16_t)0x0040)            /*!<Bit 0 */
#define TIM_OR_TI4_RMP_1                     ((uint16_t)0x0080)            /*!<Bit 1 */
#define TIM_OR_ITR1_RMP                      ((uint16_t)0x0C00)            /*!<ITR1_RMP[1:0] bits (TIM2 Internal trigger 1 remap) */
#define TIM_OR_ITR1_RMP_0                    ((uint16_t)0x0400)            /*!<Bit 0 */
#define TIM_OR_ITR1_RMP_1                    ((uint16_t)0x0800)            /*!<Bit 1 */


/******************************************************************************/
/*                                                                            */
/*         Universal Synchronous Asynchronous Receiver Transmitter            */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for USART_SR register  *******************/
#define  USART_SR_PE                         ((uint16_t)0x0001)            /*!<Parity Error                 */
#define  USART_SR_FE                         ((uint16_t)0x0002)            /*!<Framing Error                */
#define  USART_SR_NE                         ((uint16_t)0x0004)            /*!<Noise Error Flag             */
#define  USART_SR_ORE                        ((uint16_t)0x0008)            /*!<OverRun Error                */
#define  USART_SR_IDLE                       ((uint16_t)0x0010)            /*!<IDLE line detected           */
#define  USART_SR_RXNE                       ((uint16_t)0x0020)            /*!<Read Data Register Not Empty */
#define  USART_SR_TC                         ((uint16_t)0x0040)            /*!<Transmission Complete        */
#define  USART_SR_TXE                        ((uint16_t)0x0080)            /*!<Transmit Data Register Empty */
#define  USART_SR_LBD                        ((uint16_t)0x0100)            /*!<LIN Break Detection Flag     */
#define  USART_SR_CTS                        ((uint16_t)0x0200)            /*!<CTS Flag                     */

/*******************  Bit definition for USART_DR register  *******************/
#define  USART_DR_DR                         ((uint16_t)0x01FF)            /*!<Data value */

/******************  Bit definition for USART_BRR register  *******************/
#define  USART_BRR_DIV_Fraction              ((uint16_t)0x000F)            /*!<Fraction of USARTDIV */
#define  USART_BRR_DIV_Mantissa              ((uint16_t)0xFFF0)            /*!<Mantissa of USARTDIV */

/******************  Bit definition for USART_CR1 register  *******************/
#define  USART_CR1_SBK                       ((uint16_t)0x0001)            /*!<Send Break                             */
#define  USART_CR1_RWU                       ((uint16_t)0x0002)            /*!<Receiver wakeup                        */
#define  USART_CR1_RE                        ((uint16_t)0x0004)            /*!<Receiver Enable                        */
#define  USART_CR1_TE                        ((uint16_t)0x0008)            /*!<Transmitter Enable                     */
#define  USART_CR1_IDLEIE                    ((uint16_t)0x0010)            /*!<IDLE Interrupt Enable                  */
#define  USART_CR1_RXNEIE                    ((uint16_t)0x0020)            /*!<RXNE Interrupt Enable                  */
#define  USART_CR1_TCIE                      ((uint16_t)0x0040)            /*!<Transmission Complete Interrupt Enable */
#define  USART_CR1_TXEIE                     ((uint16_t)0x0080)            /*!<PE Interrupt Enable                    */
#define  USART_CR1_PEIE                      ((uint16_t)0x0100)            /*!<PE Interrupt Enable                    */
#define  USART_CR1_PS                        ((uint16_t)0x0200)            /*!<Parity Selection                       */
#define  USART_CR1_PCE                       ((uint16_t)0x0400)            /*!<Parity Control Enable                  */
#define  USART_CR1_WAKE                      ((uint16_t)0x0800)            /*!<Wakeup method                          */
#define  USART_CR1_M                         ((uint16_t)0x1000)            /*!<Word length                            */
#define  USART_CR1_UE                        ((uint16_t)0x2000)            /*!<USART Enable                           */
#define  USART_CR1_OVER8                     ((uint16_t)0x8000)            /*!<USART Oversampling by 8 enable         */

/******************  Bit definition for USART_CR2 register  *******************/
#define  USART_CR2_ADD                       ((uint16_t)0x000F)            /*!<Address of the USART node            */
#define  USART_CR2_LBDL                      ((uint16_t)0x0020)            /*!<LIN Break Detection Length           */
#define  USART_CR2_LBDIE                     ((uint16_t)0x0040)            /*!<LIN Break Detection Interrupt Enable */
#define  USART_CR2_LBCL                      ((uint16_t)0x0100)            /*!<Last Bit Clock pulse                 */
#define  USART_CR2_CPHA                      ((uint16_t)0x0200)            /*!<Clock Phase                          */
#define  USART_CR2_CPOL                      ((uint16_t)0x0400)            /*!<Clock Polarity                       */
#define  USART_CR2_CLKEN                     ((uint16_t)0x0800)            /*!<Clock Enable                         */

#define  USART_CR2_STOP                      ((uint16_t)0x3000)            /*!<STOP[1:0] bits (STOP bits) */
#define  USART_CR2_STOP_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  USART_CR2_STOP_1                    ((uint16_t)0x2000)            /*!<Bit 1 */

#define  USART_CR2_LINEN                     ((uint16_t)0x4000)            /*!<LIN mode enable */

/******************  Bit definition for USART_CR3 register  *******************/
#define  USART_CR3_EIE                       ((uint16_t)0x0001)            /*!<Error Interrupt Enable      */
#define  USART_CR3_IREN                      ((uint16_t)0x0002)            /*!<IrDA mode Enable            */
#define  USART_CR3_IRLP                      ((uint16_t)0x0004)            /*!<IrDA Low-Power              */
#define  USART_CR3_HDSEL                     ((uint16_t)0x0008)            /*!<Half-Duplex Selection       */
#define  USART_CR3_NACK                      ((uint16_t)0x0010)            /*!<Smartcard NACK enable       */
#define  USART_CR3_SCEN                      ((uint16_t)0x0020)            /*!<Smartcard mode enable       */
#define  USART_CR3_DMAR                      ((uint16_t)0x0040)            /*!<DMA Enable Receiver         */
#define  USART_CR3_DMAT                      ((uint16_t)0x0080)            /*!<DMA Enable Transmitter      */
#define  USART_CR3_RTSE                      ((uint16_t)0x0100)            /*!<RTS Enable                  */
#define  USART_CR3_CTSE                      ((uint16_t)0x0200)            /*!<CTS Enable                  */
#define  USART_CR3_CTSIE                     ((uint16_t)0x0400)            /*!<CTS Interrupt Enable        */
#define  USART_CR3_ONEBIT                    ((uint16_t)0x0800)            /*!<USART One bit method enable */

/******************  Bit definition for USART_GTPR register  ******************/
#define  USART_GTPR_PSC                      ((uint16_t)0x00FF)            /*!<PSC[7:0] bits (Prescaler value) */
#define  USART_GTPR_PSC_0                    ((uint16_t)0x0001)            /*!<Bit 0 */
#define  USART_GTPR_PSC_1                    ((uint16_t)0x0002)            /*!<Bit 1 */
#define  USART_GTPR_PSC_2                    ((uint16_t)0x0004)            /*!<Bit 2 */
#define  USART_GTPR_PSC_3                    ((uint16_t)0x0008)            /*!<Bit 3 */
#define  USART_GTPR_PSC_4                    ((uint16_t)0x0010)            /*!<Bit 4 */
#define  USART_GTPR_PSC_5                    ((uint16_t)0x0020)            /*!<Bit 5 */
#define  USART_GTPR_PSC_6                    ((uint16_t)0x0040)            /*!<Bit 6 */
#define  USART_GTPR_PSC_7                    ((uint16_t)0x0080)            /*!<Bit 7 */

#define  USART_GTPR_GT                       ((uint16_t)0xFF00)            /*!<Guard time value */

/******************************************************************************/
/*                                                                            */
/*                            Window WATCHDOG                                 */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for WWDG_CR register  ********************/
#define  WWDG_CR_T                           ((uint8_t)0x7F)               /*!<T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define  WWDG_CR_T0                          ((uint8_t)0x01)               /*!<Bit 0 */
#define  WWDG_CR_T1                          ((uint8_t)0x02)               /*!<Bit 1 */
#define  WWDG_CR_T2                          ((uint8_t)0x04)               /*!<Bit 2 */
#define  WWDG_CR_T3                          ((uint8_t)0x08)               /*!<Bit 3 */
#define  WWDG_CR_T4                          ((uint8_t)0x10)               /*!<Bit 4 */
#define  WWDG_CR_T5                          ((uint8_t)0x20)               /*!<Bit 5 */
#define  WWDG_CR_T6                          ((uint8_t)0x40)               /*!<Bit 6 */

#define  WWDG_CR_WDGA                        ((uint8_t)0x80)               /*!<Activation bit */

/*******************  Bit definition for WWDG_CFR register  *******************/
#define  WWDG_CFR_W                          ((uint16_t)0x007F)            /*!<W[6:0] bits (7-bit window value) */
#define  WWDG_CFR_W0                         ((uint16_t)0x0001)            /*!<Bit 0 */
#define  WWDG_CFR_W1                         ((uint16_t)0x0002)            /*!<Bit 1 */
#define  WWDG_CFR_W2                         ((uint16_t)0x0004)            /*!<Bit 2 */
#define  WWDG_CFR_W3                         ((uint16_t)0x0008)            /*!<Bit 3 */
#define  WWDG_CFR_W4                         ((uint16_t)0x0010)            /*!<Bit 4 */
#define  WWDG_CFR_W5                         ((uint16_t)0x0020)            /*!<Bit 5 */
#define  WWDG_CFR_W6                         ((uint16_t)0x0040)            /*!<Bit 6 */

#define  WWDG_CFR_WDGTB                      ((uint16_t)0x0180)            /*!<WDGTB[1:0] bits (Timer Base) */
#define  WWDG_CFR_WDGTB0                     ((uint16_t)0x0080)            /*!<Bit 0 */
#define  WWDG_CFR_WDGTB1                     ((uint16_t)0x0100)            /*!<Bit 1 */

#define  WWDG_CFR_EWI                        ((uint16_t)0x0200)            /*!<Early Wakeup Interrupt */

/*******************  Bit definition for WWDG_SR register  ********************/
#define  WWDG_SR_EWIF                        ((uint8_t)0x01)               /*!<Early Wakeup Interrupt Flag */


/******************************************************************************/
/*                                                                            */
/*                                DBG                                         */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for DBGMCU_IDCODE register  *************/
#define  DBGMCU_IDCODE_DEV_ID                ((uint32_t)0x00000FFF)
#define  DBGMCU_IDCODE_REV_ID                ((uint32_t)0xFFFF0000)

/********************  Bit definition for DBGMCU_CR register  *****************/
#define  DBGMCU_CR_DBG_SLEEP                 ((uint32_t)0x00000001)
#define  DBGMCU_CR_DBG_STOP                  ((uint32_t)0x00000002)
#define  DBGMCU_CR_DBG_STANDBY               ((uint32_t)0x00000004)
#define  DBGMCU_CR_TRACE_IOEN                ((uint32_t)0x00000020)

#define  DBGMCU_CR_TRACE_MODE                ((uint32_t)0x000000C0)
#define  DBGMCU_CR_TRACE_MODE_0              ((uint32_t)0x00000040)/*!<Bit 0 */
#define  DBGMCU_CR_TRACE_MODE_1              ((uint32_t)0x00000080)/*!<Bit 1 */

/********************  Bit definition for DBGMCU_APB1_FZ register  ************/
#define  DBGMCU_APB1_FZ_DBG_TIM2_STOP            ((uint32_t)0x00000001)
#define  DBGMCU_APB1_FZ_DBG_TIM3_STOP            ((uint32_t)0x00000002)
#define  DBGMCU_APB1_FZ_DBG_TIM4_STOP            ((uint32_t)0x00000004)
#define  DBGMCU_APB1_FZ_DBG_TIM5_STOP            ((uint32_t)0x00000008)
#define  DBGMCU_APB1_FZ_DBG_TIM6_STOP            ((uint32_t)0x00000010)
#define  DBGMCU_APB1_FZ_DBG_TIM7_STOP            ((uint32_t)0x00000020)
#define  DBGMCU_APB1_FZ_DBG_TIM12_STOP           ((uint32_t)0x00000040)
#define  DBGMCU_APB1_FZ_DBG_TIM13_STOP           ((uint32_t)0x00000080)
#define  DBGMCU_APB1_FZ_DBG_TIM14_STOP           ((uint32_t)0x00000100)
#define  DBGMCU_APB1_FZ_DBG_RTC_STOP             ((uint32_t)0x00000400)
#define  DBGMCU_APB1_FZ_DBG_WWDG_STOP            ((uint32_t)0x00000800)
#define  DBGMCU_APB1_FZ_DBG_IWDG_STOP            ((uint32_t)0x00001000)
#define  DBGMCU_APB1_FZ_DBG_I2C1_SMBUS_TIMEOUT   ((uint32_t)0x00200000)
#define  DBGMCU_APB1_FZ_DBG_I2C2_SMBUS_TIMEOUT   ((uint32_t)0x00400000)
#define  DBGMCU_APB1_FZ_DBG_I2C3_SMBUS_TIMEOUT   ((uint32_t)0x00800000)
#define  DBGMCU_APB1_FZ_DBG_CAN1_STOP            ((uint32_t)0x02000000)
#define  DBGMCU_APB1_FZ_DBG_CAN2_STOP            ((uint32_t)0x04000000)
/* Old IWDGSTOP bit definition, maintained for legacy purpose */
#define  DBGMCU_APB1_FZ_DBG_IWDEG_STOP           DBGMCU_APB1_FZ_DBG_IWDG_STOP

/********************  Bit definition for DBGMCU_APB1_FZ register  ************/
#define  DBGMCU_APB1_FZ_DBG_TIM1_STOP        ((uint32_t)0x00000001)
#define  DBGMCU_APB1_FZ_DBG_TIM8_STOP        ((uint32_t)0x00000002)
#define  DBGMCU_APB1_FZ_DBG_TIM9_STOP        ((uint32_t)0x00010000)
#define  DBGMCU_APB1_FZ_DBG_TIM10_STOP       ((uint32_t)0x00020000)
#define  DBGMCU_APB1_FZ_DBG_TIM11_STOP       ((uint32_t)0x00040000)

/******************************************************************************/
/*                                                                            */
/*                Ethernet MAC Registers bits definitions                     */
/*                                                                            */
/******************************************************************************/
/* Bit definition for Ethernet MAC Control Register register */
#define ETH_MACCR_WD      ((uint32_t)0x00800000)  /* Watchdog disable */
#define ETH_MACCR_JD      ((uint32_t)0x00400000)  /* Jabber disable */
#define ETH_MACCR_IFG     ((uint32_t)0x000E0000)  /* Inter-frame gap */
#define ETH_MACCR_IFG_96Bit     ((uint32_t)0x00000000)  /* Minimum IFG between frames during transmission is 96Bit */
  #define ETH_MACCR_IFG_88Bit     ((uint32_t)0x00020000)  /* Minimum IFG between frames during transmission is 88Bit */
  #define ETH_MACCR_IFG_80Bit     ((uint32_t)0x00040000)  /* Minimum IFG between frames during transmission is 80Bit */
  #define ETH_MACCR_IFG_72Bit     ((uint32_t)0x00060000)  /* Minimum IFG between frames during transmission is 72Bit */
  #define ETH_MACCR_IFG_64Bit     ((uint32_t)0x00080000)  /* Minimum IFG between frames during transmission is 64Bit */        
  #define ETH_MACCR_IFG_56Bit     ((uint32_t)0x000A0000)  /* Minimum IFG between frames during transmission is 56Bit */
  #define ETH_MACCR_IFG_48Bit     ((uint32_t)0x000C0000)  /* Minimum IFG between frames during transmission is 48Bit */
  #define ETH_MACCR_IFG_40Bit     ((uint32_t)0x000E0000)  /* Minimum IFG between frames during transmission is 40Bit */              
#define ETH_MACCR_CSD     ((uint32_t)0x00010000)  /* Carrier sense disable (during transmission) */
#define ETH_MACCR_FES     ((uint32_t)0x00004000)  /* Fast ethernet speed */
#define ETH_MACCR_ROD     ((uint32_t)0x00002000)  /* Receive own disable */
#define ETH_MACCR_LM      ((uint32_t)0x00001000)  /* loopback mode */
#define ETH_MACCR_DM      ((uint32_t)0x00000800)  /* Duplex mode */
#define ETH_MACCR_IPCO    ((uint32_t)0x00000400)  /* IP Checksum offload */
#define ETH_MACCR_RD      ((uint32_t)0x00000200)  /* Retry disable */
#define ETH_MACCR_APCS    ((uint32_t)0x00000080)  /* Automatic Pad/CRC stripping */
#define ETH_MACCR_BL      ((uint32_t)0x00000060)  /* Back-off limit: random integer number (r) of slot time delays before rescheduling
                                                       a transmission attempt during retries after a collision: 0 =< r <2^k */
  #define ETH_MACCR_BL_10    ((uint32_t)0x00000000)  /* k = min (n, 10) */
  #define ETH_MACCR_BL_8     ((uint32_t)0x00000020)  /* k = min (n, 8) */
  #define ETH_MACCR_BL_4     ((uint32_t)0x00000040)  /* k = min (n, 4) */
  #define ETH_MACCR_BL_1     ((uint32_t)0x00000060)  /* k = min (n, 1) */ 
#define ETH_MACCR_DC      ((uint32_t)0x00000010)  /* Defferal check */
#define ETH_MACCR_TE      ((uint32_t)0x00000008)  /* Transmitter enable */
#define ETH_MACCR_RE      ((uint32_t)0x00000004)  /* Receiver enable */

/* Bit definition for Ethernet MAC Frame Filter Register */
#define ETH_MACFFR_RA     ((uint32_t)0x80000000)  /* Receive all */ 
#define ETH_MACFFR_HPF    ((uint32_t)0x00000400)  /* Hash or perfect filter */ 
#define ETH_MACFFR_SAF    ((uint32_t)0x00000200)  /* Source address filter enable */ 
#define ETH_MACFFR_SAIF   ((uint32_t)0x00000100)  /* SA inverse filtering */ 
#define ETH_MACFFR_PCF    ((uint32_t)0x000000C0)  /* Pass control frames: 3 cases */
  #define ETH_MACFFR_PCF_BlockAll                ((uint32_t)0x00000040)  /* MAC filters all control frames from reaching the application */
  #define ETH_MACFFR_PCF_ForwardAll              ((uint32_t)0x00000080)  /* MAC forwards all control frames to application even if they fail the Address Filter */
  #define ETH_MACFFR_PCF_ForwardPassedAddrFilter ((uint32_t)0x000000C0)  /* MAC forwards control frames that pass the Address Filter. */ 
#define ETH_MACFFR_BFD    ((uint32_t)0x00000020)  /* Broadcast frame disable */ 
#define ETH_MACFFR_PAM    ((uint32_t)0x00000010)  /* Pass all mutlicast */ 
#define ETH_MACFFR_DAIF   ((uint32_t)0x00000008)  /* DA Inverse filtering */ 
#define ETH_MACFFR_HM     ((uint32_t)0x00000004)  /* Hash multicast */ 
#define ETH_MACFFR_HU     ((uint32_t)0x00000002)  /* Hash unicast */
#define ETH_MACFFR_PM     ((uint32_t)0x00000001)  /* Promiscuous mode */

/* Bit definition for Ethernet MAC Hash Table High Register */
#define ETH_MACHTHR_HTH   ((uint32_t)0xFFFFFFFF)  /* Hash table high */

/* Bit definition for Ethernet MAC Hash Table Low Register */
#define ETH_MACHTLR_HTL   ((uint32_t)0xFFFFFFFF)  /* Hash table low */

/* Bit definition for Ethernet MAC MII Address Register */
#define ETH_MACMIIAR_PA   ((uint32_t)0x0000F800)  /* Physical layer address */ 
#define ETH_MACMIIAR_MR   ((uint32_t)0x000007C0)  /* MII register in the selected PHY */ 
#define ETH_MACMIIAR_CR   ((uint32_t)0x0000001C)  /* CR clock range: 6 cases */ 
  #define ETH_MACMIIAR_CR_Div42   ((uint32_t)0x00000000)  /* HCLK:60-100 MHz; MDC clock= HCLK/42 */
  #define ETH_MACMIIAR_CR_Div62   ((uint32_t)0x00000004)  /* HCLK:100-150 MHz; MDC clock= HCLK/62 */
  #define ETH_MACMIIAR_CR_Div16   ((uint32_t)0x00000008)  /* HCLK:20-35 MHz; MDC clock= HCLK/16 */
  #define ETH_MACMIIAR_CR_Div26   ((uint32_t)0x0000000C)  /* HCLK:35-60 MHz; MDC clock= HCLK/26 */
  #define ETH_MACMIIAR_CR_Div102  ((uint32_t)0x00000010)  /* HCLK:150-168 MHz; MDC clock= HCLK/102 */  
#define ETH_MACMIIAR_MW   ((uint32_t)0x00000002)  /* MII write */ 
#define ETH_MACMIIAR_MB   ((uint32_t)0x00000001)  /* MII busy */ 
  
/* Bit definition for Ethernet MAC MII Data Register */
#define ETH_MACMIIDR_MD   ((uint32_t)0x0000FFFF)  /* MII data: read/write data from/to PHY */

/* Bit definition for Ethernet MAC Flow Control Register */
#define ETH_MACFCR_PT     ((uint32_t)0xFFFF0000)  /* Pause time */
#define ETH_MACFCR_ZQPD   ((uint32_t)0x00000080)  /* Zero-quanta pause disable */
#define ETH_MACFCR_PLT    ((uint32_t)0x00000030)  /* Pause low threshold: 4 cases */
  #define ETH_MACFCR_PLT_Minus4   ((uint32_t)0x00000000)  /* Pause time minus 4 slot times */
  #define ETH_MACFCR_PLT_Minus28  ((uint32_t)0x00000010)  /* Pause time minus 28 slot times */
  #define ETH_MACFCR_PLT_Minus144 ((uint32_t)0x00000020)  /* Pause time minus 144 slot times */
  #define ETH_MACFCR_PLT_Minus256 ((uint32_t)0x00000030)  /* Pause time minus 256 slot times */      
#define ETH_MACFCR_UPFD   ((uint32_t)0x00000008)  /* Unicast pause frame detect */
#define ETH_MACFCR_RFCE   ((uint32_t)0x00000004)  /* Receive flow control enable */
#define ETH_MACFCR_TFCE   ((uint32_t)0x00000002)  /* Transmit flow control enable */
#define ETH_MACFCR_FCBBPA ((uint32_t)0x00000001)  /* Flow control busy/backpressure activate */

/* Bit definition for Ethernet MAC VLAN Tag Register */
#define ETH_MACVLANTR_VLANTC ((uint32_t)0x00010000)  /* 12-bit VLAN tag comparison */
#define ETH_MACVLANTR_VLANTI ((uint32_t)0x0000FFFF)  /* VLAN tag identifier (for receive frames) */

/* Bit definition for Ethernet MAC Remote Wake-UpFrame Filter Register */ 
#define ETH_MACRWUFFR_D   ((uint32_t)0xFFFFFFFF)  /* Wake-up frame filter register data */
/* Eight sequential Writes to this address (offset 0x28) will write all Wake-UpFrame Filter Registers.
   Eight sequential Reads from this address (offset 0x28) will read all Wake-UpFrame Filter Registers. */
/* Wake-UpFrame Filter Reg0 : Filter 0 Byte Mask
   Wake-UpFrame Filter Reg1 : Filter 1 Byte Mask
   Wake-UpFrame Filter Reg2 : Filter 2 Byte Mask
   Wake-UpFrame Filter Reg3 : Filter 3 Byte Mask
   Wake-UpFrame Filter Reg4 : RSVD - Filter3 Command - RSVD - Filter2 Command - 
                              RSVD - Filter1 Command - RSVD - Filter0 Command
   Wake-UpFrame Filter Re5 : Filter3 Offset - Filter2 Offset - Filter1 Offset - Filter0 Offset
   Wake-UpFrame Filter Re6 : Filter1 CRC16 - Filter0 CRC16
   Wake-UpFrame Filter Re7 : Filter3 CRC16 - Filter2 CRC16 */

/* Bit definition for Ethernet MAC PMT Control and Status Register */ 
#define ETH_MACPMTCSR_WFFRPR ((uint32_t)0x80000000)  /* Wake-Up Frame Filter Register Pointer Reset */
#define ETH_MACPMTCSR_GU     ((uint32_t)0x00000200)  /* Global Unicast */
#define ETH_MACPMTCSR_WFR    ((uint32_t)0x00000040)  /* Wake-Up Frame Received */
#define ETH_MACPMTCSR_MPR    ((uint32_t)0x00000020)  /* Magic Packet Received */
#define ETH_MACPMTCSR_WFE    ((uint32_t)0x00000004)  /* Wake-Up Frame Enable */
#define ETH_MACPMTCSR_MPE    ((uint32_t)0x00000002)  /* Magic Packet Enable */
#define ETH_MACPMTCSR_PD     ((uint32_t)0x00000001)  /* Power Down */

/* Bit definition for Ethernet MAC Status Register */
#define ETH_MACSR_TSTS      ((uint32_t)0x00000200)  /* Time stamp trigger status */
#define ETH_MACSR_MMCTS     ((uint32_t)0x00000040)  /* MMC transmit status */
#define ETH_MACSR_MMMCRS    ((uint32_t)0x00000020)  /* MMC receive status */
#define ETH_MACSR_MMCS      ((uint32_t)0x00000010)  /* MMC status */
#define ETH_MACSR_PMTS      ((uint32_t)0x00000008)  /* PMT status */

/* Bit definition for Ethernet MAC Interrupt Mask Register */
#define ETH_MACIMR_TSTIM     ((uint32_t)0x00000200)  /* Time stamp trigger interrupt mask */
#define ETH_MACIMR_PMTIM     ((uint32_t)0x00000008)  /* PMT interrupt mask */

/* Bit definition for Ethernet MAC Address0 High Register */
#define ETH_MACA0HR_MACA0H   ((uint32_t)0x0000FFFF)  /* MAC address0 high */

/* Bit definition for Ethernet MAC Address0 Low Register */
#define ETH_MACA0LR_MACA0L   ((uint32_t)0xFFFFFFFF)  /* MAC address0 low */

/* Bit definition for Ethernet MAC Address1 High Register */
#define ETH_MACA1HR_AE       ((uint32_t)0x80000000)  /* Address enable */
#define ETH_MACA1HR_SA       ((uint32_t)0x40000000)  /* Source address */
#define ETH_MACA1HR_MBC      ((uint32_t)0x3F000000)  /* Mask byte control: bits to mask for comparison of the MAC Address bytes */
  #define ETH_MACA1HR_MBC_HBits15_8    ((uint32_t)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA1HR_MBC_HBits7_0     ((uint32_t)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA1HR_MBC_LBits31_24   ((uint32_t)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA1HR_MBC_LBits23_16   ((uint32_t)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA1HR_MBC_LBits15_8    ((uint32_t)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA1HR_MBC_LBits7_0     ((uint32_t)0x01000000)  /* Mask MAC Address low reg bits [7:0] */ 
#define ETH_MACA1HR_MACA1H   ((uint32_t)0x0000FFFF)  /* MAC address1 high */

/* Bit definition for Ethernet MAC Address1 Low Register */
#define ETH_MACA1LR_MACA1L   ((uint32_t)0xFFFFFFFF)  /* MAC address1 low */

/* Bit definition for Ethernet MAC Address2 High Register */
#define ETH_MACA2HR_AE       ((uint32_t)0x80000000)  /* Address enable */
#define ETH_MACA2HR_SA       ((uint32_t)0x40000000)  /* Source address */
#define ETH_MACA2HR_MBC      ((uint32_t)0x3F000000)  /* Mask byte control */
  #define ETH_MACA2HR_MBC_HBits15_8    ((uint32_t)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA2HR_MBC_HBits7_0     ((uint32_t)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA2HR_MBC_LBits31_24   ((uint32_t)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA2HR_MBC_LBits23_16   ((uint32_t)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA2HR_MBC_LBits15_8    ((uint32_t)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA2HR_MBC_LBits7_0     ((uint32_t)0x01000000)  /* Mask MAC Address low reg bits [70] */
#define ETH_MACA2HR_MACA2H   ((uint32_t)0x0000FFFF)  /* MAC address1 high */

/* Bit definition for Ethernet MAC Address2 Low Register */
#define ETH_MACA2LR_MACA2L   ((uint32_t)0xFFFFFFFF)  /* MAC address2 low */

/* Bit definition for Ethernet MAC Address3 High Register */
#define ETH_MACA3HR_AE       ((uint32_t)0x80000000)  /* Address enable */
#define ETH_MACA3HR_SA       ((uint32_t)0x40000000)  /* Source address */
#define ETH_MACA3HR_MBC      ((uint32_t)0x3F000000)  /* Mask byte control */
  #define ETH_MACA3HR_MBC_HBits15_8    ((uint32_t)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA3HR_MBC_HBits7_0     ((uint32_t)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA3HR_MBC_LBits31_24   ((uint32_t)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA3HR_MBC_LBits23_16   ((uint32_t)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA3HR_MBC_LBits15_8    ((uint32_t)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA3HR_MBC_LBits7_0     ((uint32_t)0x01000000)  /* Mask MAC Address low reg bits [70] */
#define ETH_MACA3HR_MACA3H   ((uint32_t)0x0000FFFF)  /* MAC address3 high */

/* Bit definition for Ethernet MAC Address3 Low Register */
#define ETH_MACA3LR_MACA3L   ((uint32_t)0xFFFFFFFF)  /* MAC address3 low */

/******************************************************************************/
/*                Ethernet MMC Registers bits definition                      */
/******************************************************************************/

/* Bit definition for Ethernet MMC Contol Register */
#define ETH_MMCCR_MCFHP      ((uint32_t)0x00000020)  /* MMC counter Full-Half preset */
#define ETH_MMCCR_MCP        ((uint32_t)0x00000010)  /* MMC counter preset */
#define ETH_MMCCR_MCF        ((uint32_t)0x00000008)  /* MMC Counter Freeze */
#define ETH_MMCCR_ROR        ((uint32_t)0x00000004)  /* Reset on Read */
#define ETH_MMCCR_CSR        ((uint32_t)0x00000002)  /* Counter Stop Rollover */
#define ETH_MMCCR_CR         ((uint32_t)0x00000001)  /* Counters Reset */

/* Bit definition for Ethernet MMC Receive Interrupt Register */
#define ETH_MMCRIR_RGUFS     ((uint32_t)0x00020000)  /* Set when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIR_RFAES     ((uint32_t)0x00000040)  /* Set when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIR_RFCES     ((uint32_t)0x00000020)  /* Set when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Register */
#define ETH_MMCTIR_TGFS      ((uint32_t)0x00200000)  /* Set when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIR_TGFMSCS   ((uint32_t)0x00008000)  /* Set when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIR_TGFSCS    ((uint32_t)0x00004000)  /* Set when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Receive Interrupt Mask Register */
#define ETH_MMCRIMR_RGUFM    ((uint32_t)0x00020000)  /* Mask the interrupt when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIMR_RFAEM    ((uint32_t)0x00000040)  /* Mask the interrupt when when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIMR_RFCEM    ((uint32_t)0x00000020)  /* Mask the interrupt when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Mask Register */
#define ETH_MMCTIMR_TGFM     ((uint32_t)0x00200000)  /* Mask the interrupt when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFMSCM  ((uint32_t)0x00008000)  /* Mask the interrupt when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFSCM   ((uint32_t)0x00004000)  /* Mask the interrupt when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmitted Good Frames after Single Collision Counter Register */
#define ETH_MMCTGFSCCR_TGFSCC     ((uint32_t)0xFFFFFFFF)  /* Number of successfully transmitted frames after a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames after More than a Single Collision Counter Register */
#define ETH_MMCTGFMSCCR_TGFMSCC   ((uint32_t)0xFFFFFFFF)  /* Number of successfully transmitted frames after more than a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames Counter Register */
#define ETH_MMCTGFCR_TGFC    ((uint32_t)0xFFFFFFFF)  /* Number of good frames transmitted. */

/* Bit definition for Ethernet MMC Received Frames with CRC Error Counter Register */
#define ETH_MMCRFCECR_RFCEC  ((uint32_t)0xFFFFFFFF)  /* Number of frames received with CRC error. */

/* Bit definition for Ethernet MMC Received Frames with Alignement Error Counter Register */
#define ETH_MMCRFAECR_RFAEC  ((uint32_t)0xFFFFFFFF)  /* Number of frames received with alignment (dribble) error */

/* Bit definition for Ethernet MMC Received Good Unicast Frames Counter Register */
#define ETH_MMCRGUFCR_RGUFC  ((uint32_t)0xFFFFFFFF)  /* Number of good unicast frames received. */

/******************************************************************************/
/*               Ethernet PTP Registers bits definition                       */
/******************************************************************************/

/* Bit definition for Ethernet PTP Time Stamp Contol Register */
#define ETH_PTPTSCR_TSCNT       ((uint32_t)0x00030000)  /* Time stamp clock node type */
#define ETH_PTPTSSR_TSSMRME     ((uint32_t)0x00008000)  /* Time stamp snapshot for message relevant to master enable */
#define ETH_PTPTSSR_TSSEME      ((uint32_t)0x00004000)  /* Time stamp snapshot for event message enable */
#define ETH_PTPTSSR_TSSIPV4FE   ((uint32_t)0x00002000)  /* Time stamp snapshot for IPv4 frames enable */
#define ETH_PTPTSSR_TSSIPV6FE   ((uint32_t)0x00001000)  /* Time stamp snapshot for IPv6 frames enable */
#define ETH_PTPTSSR_TSSPTPOEFE  ((uint32_t)0x00000800)  /* Time stamp snapshot for PTP over ethernet frames enable */
#define ETH_PTPTSSR_TSPTPPSV2E  ((uint32_t)0x00000400)  /* Time stamp PTP packet snooping for version2 format enable */
#define ETH_PTPTSSR_TSSSR       ((uint32_t)0x00000200)  /* Time stamp Sub-seconds rollover */
#define ETH_PTPTSSR_TSSARFE     ((uint32_t)0x00000100)  /* Time stamp snapshot for all received frames enable */

#define ETH_PTPTSCR_TSARU    ((uint32_t)0x00000020)  /* Addend register update */
#define ETH_PTPTSCR_TSITE    ((uint32_t)0x00000010)  /* Time stamp interrupt trigger enable */
#define ETH_PTPTSCR_TSSTU    ((uint32_t)0x00000008)  /* Time stamp update */
#define ETH_PTPTSCR_TSSTI    ((uint32_t)0x00000004)  /* Time stamp initialize */
#define ETH_PTPTSCR_TSFCU    ((uint32_t)0x00000002)  /* Time stamp fine or coarse update */
#define ETH_PTPTSCR_TSE      ((uint32_t)0x00000001)  /* Time stamp enable */

/* Bit definition for Ethernet PTP Sub-Second Increment Register */
#define ETH_PTPSSIR_STSSI    ((uint32_t)0x000000FF)  /* System time Sub-second increment value */

/* Bit definition for Ethernet PTP Time Stamp High Register */
#define ETH_PTPTSHR_STS      ((uint32_t)0xFFFFFFFF)  /* System Time second */

/* Bit definition for Ethernet PTP Time Stamp Low Register */
#define ETH_PTPTSLR_STPNS    ((uint32_t)0x80000000)  /* System Time Positive or negative time */
#define ETH_PTPTSLR_STSS     ((uint32_t)0x7FFFFFFF)  /* System Time sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp High Update Register */
#define ETH_PTPTSHUR_TSUS    ((uint32_t)0xFFFFFFFF)  /* Time stamp update seconds */

/* Bit definition for Ethernet PTP Time Stamp Low Update Register */
#define ETH_PTPTSLUR_TSUPNS  ((uint32_t)0x80000000)  /* Time stamp update Positive or negative time */
#define ETH_PTPTSLUR_TSUSS   ((uint32_t)0x7FFFFFFF)  /* Time stamp update sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp Addend Register */
#define ETH_PTPTSAR_TSA      ((uint32_t)0xFFFFFFFF)  /* Time stamp addend */

/* Bit definition for Ethernet PTP Target Time High Register */
#define ETH_PTPTTHR_TTSH     ((uint32_t)0xFFFFFFFF)  /* Target time stamp high */

/* Bit definition for Ethernet PTP Target Time Low Register */
#define ETH_PTPTTLR_TTSL     ((uint32_t)0xFFFFFFFF)  /* Target time stamp low */

/* Bit definition for Ethernet PTP Time Stamp Status Register */
#define ETH_PTPTSSR_TSTTR    ((uint32_t)0x00000020)  /* Time stamp target time reached */
#define ETH_PTPTSSR_TSSO     ((uint32_t)0x00000010)  /* Time stamp seconds overflow */

/******************************************************************************/
/*                 Ethernet DMA Registers bits definition                     */
/******************************************************************************/

/* Bit definition for Ethernet DMA Bus Mode Register */
#define ETH_DMABMR_AAB       ((uint32_t)0x02000000)  /* Address-Aligned beats */
#define ETH_DMABMR_FPM        ((uint32_t)0x01000000)  /* 4xPBL mode */
#define ETH_DMABMR_USP       ((uint32_t)0x00800000)  /* Use separate PBL */
#define ETH_DMABMR_RDP       ((uint32_t)0x007E0000)  /* RxDMA PBL */
  #define ETH_DMABMR_RDP_1Beat    ((uint32_t)0x00020000)  /* maximum number of beats to be transferred in one RxDMA transaction is 1 */
  #define ETH_DMABMR_RDP_2Beat    ((uint32_t)0x00040000)  /* maximum number of beats to be transferred in one RxDMA transaction is 2 */
  #define ETH_DMABMR_RDP_4Beat    ((uint32_t)0x00080000)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
  #define ETH_DMABMR_RDP_8Beat    ((uint32_t)0x00100000)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
  #define ETH_DMABMR_RDP_16Beat   ((uint32_t)0x00200000)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
  #define ETH_DMABMR_RDP_32Beat   ((uint32_t)0x00400000)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */                
  #define ETH_DMABMR_RDP_4xPBL_4Beat   ((uint32_t)0x01020000)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
  #define ETH_DMABMR_RDP_4xPBL_8Beat   ((uint32_t)0x01040000)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
  #define ETH_DMABMR_RDP_4xPBL_16Beat  ((uint32_t)0x01080000)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
  #define ETH_DMABMR_RDP_4xPBL_32Beat  ((uint32_t)0x01100000)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
  #define ETH_DMABMR_RDP_4xPBL_64Beat  ((uint32_t)0x01200000)  /* maximum number of beats to be transferred in one RxDMA transaction is 64 */
  #define ETH_DMABMR_RDP_4xPBL_128Beat ((uint32_t)0x01400000)  /* maximum number of beats to be transferred in one RxDMA transaction is 128 */  
#define ETH_DMABMR_FB        ((uint32_t)0x00010000)  /* Fixed Burst */
#define ETH_DMABMR_RTPR      ((uint32_t)0x0000C000)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_1_1     ((uint32_t)0x00000000)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_2_1     ((uint32_t)0x00004000)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_3_1     ((uint32_t)0x00008000)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_4_1     ((uint32_t)0x0000C000)  /* Rx Tx priority ratio */  
#define ETH_DMABMR_PBL    ((uint32_t)0x00003F00)  /* Programmable burst length */
  #define ETH_DMABMR_PBL_1Beat    ((uint32_t)0x00000100)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
  #define ETH_DMABMR_PBL_2Beat    ((uint32_t)0x00000200)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
  #define ETH_DMABMR_PBL_4Beat    ((uint32_t)0x00000400)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
  #define ETH_DMABMR_PBL_8Beat    ((uint32_t)0x00000800)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
  #define ETH_DMABMR_PBL_16Beat   ((uint32_t)0x00001000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
  #define ETH_DMABMR_PBL_32Beat   ((uint32_t)0x00002000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */                
  #define ETH_DMABMR_PBL_4xPBL_4Beat   ((uint32_t)0x01000100)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
  #define ETH_DMABMR_PBL_4xPBL_8Beat   ((uint32_t)0x01000200)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
  #define ETH_DMABMR_PBL_4xPBL_16Beat  ((uint32_t)0x01000400)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
  #define ETH_DMABMR_PBL_4xPBL_32Beat  ((uint32_t)0x01000800)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
  #define ETH_DMABMR_PBL_4xPBL_64Beat  ((uint32_t)0x01001000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
  #define ETH_DMABMR_PBL_4xPBL_128Beat ((uint32_t)0x01002000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */
#define ETH_DMABMR_EDE       ((uint32_t)0x00000080)  /* Enhanced Descriptor Enable */
#define ETH_DMABMR_DSL       ((uint32_t)0x0000007C)  /* Descriptor Skip Length */
#define ETH_DMABMR_DA        ((uint32_t)0x00000002)  /* DMA arbitration scheme */
#define ETH_DMABMR_SR        ((uint32_t)0x00000001)  /* Software reset */

/* Bit definition for Ethernet DMA Transmit Poll Demand Register */
#define ETH_DMATPDR_TPD      ((uint32_t)0xFFFFFFFF)  /* Transmit poll demand */

/* Bit definition for Ethernet DMA Receive Poll Demand Register */
#define ETH_DMARPDR_RPD      ((uint32_t)0xFFFFFFFF)  /* Receive poll demand  */

/* Bit definition for Ethernet DMA Receive Descriptor List Address Register */
#define ETH_DMARDLAR_SRL     ((uint32_t)0xFFFFFFFF)  /* Start of receive list */

/* Bit definition for Ethernet DMA Transmit Descriptor List Address Register */
#define ETH_DMATDLAR_STL     ((uint32_t)0xFFFFFFFF)  /* Start of transmit list */

/* Bit definition for Ethernet DMA Status Register */
#define ETH_DMASR_TSTS       ((uint32_t)0x20000000)  /* Time-stamp trigger status */
#define ETH_DMASR_PMTS       ((uint32_t)0x10000000)  /* PMT status */
#define ETH_DMASR_MMCS       ((uint32_t)0x08000000)  /* MMC status */
#define ETH_DMASR_EBS        ((uint32_t)0x03800000)  /* Error bits status */
  /* combination with EBS[2:0] for GetFlagStatus function */
  #define ETH_DMASR_EBS_DescAccess      ((uint32_t)0x02000000)  /* Error bits 0-data buffer, 1-desc. access */
  #define ETH_DMASR_EBS_ReadTransf      ((uint32_t)0x01000000)  /* Error bits 0-write trnsf, 1-read transfr */
  #define ETH_DMASR_EBS_DataTransfTx    ((uint32_t)0x00800000)  /* Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS         ((uint32_t)0x00700000)  /* Transmit process state */
  #define ETH_DMASR_TPS_Stopped         ((uint32_t)0x00000000)  /* Stopped - Reset or Stop Tx Command issued  */
  #define ETH_DMASR_TPS_Fetching        ((uint32_t)0x00100000)  /* Running - fetching the Tx descriptor */
  #define ETH_DMASR_TPS_Waiting         ((uint32_t)0x00200000)  /* Running - waiting for status */
  #define ETH_DMASR_TPS_Reading         ((uint32_t)0x00300000)  /* Running - reading the data from host memory */
  #define ETH_DMASR_TPS_Suspended       ((uint32_t)0x00600000)  /* Suspended - Tx Descriptor unavailabe */
  #define ETH_DMASR_TPS_Closing         ((uint32_t)0x00700000)  /* Running - closing Rx descriptor */
#define ETH_DMASR_RPS         ((uint32_t)0x000E0000)  /* Receive process state */
  #define ETH_DMASR_RPS_Stopped         ((uint32_t)0x00000000)  /* Stopped - Reset or Stop Rx Command issued */
  #define ETH_DMASR_RPS_Fetching        ((uint32_t)0x00020000)  /* Running - fetching the Rx descriptor */
  #define ETH_DMASR_RPS_Waiting         ((uint32_t)0x00060000)  /* Running - waiting for packet */
  #define ETH_DMASR_RPS_Suspended       ((uint32_t)0x00080000)  /* Suspended - Rx Descriptor unavailable */
  #define ETH_DMASR_RPS_Closing         ((uint32_t)0x000A0000)  /* Running - closing descriptor */
  #define ETH_DMASR_RPS_Queuing         ((uint32_t)0x000E0000)  /* Running - queuing the recieve frame into host memory */
#define ETH_DMASR_NIS        ((uint32_t)0x00010000)  /* Normal interrupt summary */
#define ETH_DMASR_AIS        ((uint32_t)0x00008000)  /* Abnormal interrupt summary */
#define ETH_DMASR_ERS        ((uint32_t)0x00004000)  /* Early receive status */
#define ETH_DMASR_FBES       ((uint32_t)0x00002000)  /* Fatal bus error status */
#define ETH_DMASR_ETS        ((uint32_t)0x00000400)  /* Early transmit status */
#define ETH_DMASR_RWTS       ((uint32_t)0x00000200)  /* Receive watchdog timeout status */
#define ETH_DMASR_RPSS       ((uint32_t)0x00000100)  /* Receive process stopped status */
#define ETH_DMASR_RBUS       ((uint32_t)0x00000080)  /* Receive buffer unavailable status */
#define ETH_DMASR_RS         ((uint32_t)0x00000040)  /* Receive status */
#define ETH_DMASR_TUS        ((uint32_t)0x00000020)  /* Transmit underflow status */
#define ETH_DMASR_ROS        ((uint32_t)0x00000010)  /* Receive overflow status */
#define ETH_DMASR_TJTS       ((uint32_t)0x00000008)  /* Transmit jabber timeout status */
#define ETH_DMASR_TBUS       ((uint32_t)0x00000004)  /* Transmit buffer unavailable status */
#define ETH_DMASR_TPSS       ((uint32_t)0x00000002)  /* Transmit process stopped status */
#define ETH_DMASR_TS         ((uint32_t)0x00000001)  /* Transmit status */

/* Bit definition for Ethernet DMA Operation Mode Register */
#define ETH_DMAOMR_DTCEFD    ((uint32_t)0x04000000)  /* Disable Dropping of TCP/IP checksum error frames */
#define ETH_DMAOMR_RSF       ((uint32_t)0x02000000)  /* Receive store and forward */
#define ETH_DMAOMR_DFRF      ((uint32_t)0x01000000)  /* Disable flushing of received frames */
#define ETH_DMAOMR_TSF       ((uint32_t)0x00200000)  /* Transmit store and forward */
#define ETH_DMAOMR_FTF       ((uint32_t)0x00100000)  /* Flush transmit FIFO */
#define ETH_DMAOMR_TTC       ((uint32_t)0x0001C000)  /* Transmit threshold control */
  #define ETH_DMAOMR_TTC_64Bytes       ((uint32_t)0x00000000)  /* threshold level of the MTL Transmit FIFO is 64 Bytes */
  #define ETH_DMAOMR_TTC_128Bytes      ((uint32_t)0x00004000)  /* threshold level of the MTL Transmit FIFO is 128 Bytes */
  #define ETH_DMAOMR_TTC_192Bytes      ((uint32_t)0x00008000)  /* threshold level of the MTL Transmit FIFO is 192 Bytes */
  #define ETH_DMAOMR_TTC_256Bytes      ((uint32_t)0x0000C000)  /* threshold level of the MTL Transmit FIFO is 256 Bytes */
  #define ETH_DMAOMR_TTC_40Bytes       ((uint32_t)0x00010000)  /* threshold level of the MTL Transmit FIFO is 40 Bytes */
  #define ETH_DMAOMR_TTC_32Bytes       ((uint32_t)0x00014000)  /* threshold level of the MTL Transmit FIFO is 32 Bytes */
  #define ETH_DMAOMR_TTC_24Bytes       ((uint32_t)0x00018000)  /* threshold level of the MTL Transmit FIFO is 24 Bytes */
  #define ETH_DMAOMR_TTC_16Bytes       ((uint32_t)0x0001C000)  /* threshold level of the MTL Transmit FIFO is 16 Bytes */
#define ETH_DMAOMR_ST        ((uint32_t)0x00002000)  /* Start/stop transmission command */
#define ETH_DMAOMR_FEF       ((uint32_t)0x00000080)  /* Forward error frames */
#define ETH_DMAOMR_FUGF      ((uint32_t)0x00000040)  /* Forward undersized good frames */
#define ETH_DMAOMR_RTC       ((uint32_t)0x00000018)  /* receive threshold control */
  #define ETH_DMAOMR_RTC_64Bytes       ((uint32_t)0x00000000)  /* threshold level of the MTL Receive FIFO is 64 Bytes */
  #define ETH_DMAOMR_RTC_32Bytes       ((uint32_t)0x00000008)  /* threshold level of the MTL Receive FIFO is 32 Bytes */
  #define ETH_DMAOMR_RTC_96Bytes       ((uint32_t)0x00000010)  /* threshold level of the MTL Receive FIFO is 96 Bytes */
  #define ETH_DMAOMR_RTC_128Bytes      ((uint32_t)0x00000018)  /* threshold level of the MTL Receive FIFO is 128 Bytes */
#define ETH_DMAOMR_OSF       ((uint32_t)0x00000004)  /* operate on second frame */
#define ETH_DMAOMR_SR        ((uint32_t)0x00000002)  /* Start/stop receive */

/* Bit definition for Ethernet DMA Interrupt Enable Register */
#define ETH_DMAIER_NISE      ((uint32_t)0x00010000)  /* Normal interrupt summary enable */
#define ETH_DMAIER_AISE      ((uint32_t)0x00008000)  /* Abnormal interrupt summary enable */
#define ETH_DMAIER_ERIE      ((uint32_t)0x00004000)  /* Early receive interrupt enable */
#define ETH_DMAIER_FBEIE     ((uint32_t)0x00002000)  /* Fatal bus error interrupt enable */
#define ETH_DMAIER_ETIE      ((uint32_t)0x00000400)  /* Early transmit interrupt enable */
#define ETH_DMAIER_RWTIE     ((uint32_t)0x00000200)  /* Receive watchdog timeout interrupt enable */
#define ETH_DMAIER_RPSIE     ((uint32_t)0x00000100)  /* Receive process stopped interrupt enable */
#define ETH_DMAIER_RBUIE     ((uint32_t)0x00000080)  /* Receive buffer unavailable interrupt enable */
#define ETH_DMAIER_RIE       ((uint32_t)0x00000040)  /* Receive interrupt enable */
#define ETH_DMAIER_TUIE      ((uint32_t)0x00000020)  /* Transmit Underflow interrupt enable */
#define ETH_DMAIER_ROIE      ((uint32_t)0x00000010)  /* Receive Overflow interrupt enable */
#define ETH_DMAIER_TJTIE     ((uint32_t)0x00000008)  /* Transmit jabber timeout interrupt enable */
#define ETH_DMAIER_TBUIE     ((uint32_t)0x00000004)  /* Transmit buffer unavailable interrupt enable */
#define ETH_DMAIER_TPSIE     ((uint32_t)0x00000002)  /* Transmit process stopped interrupt enable */
#define ETH_DMAIER_TIE       ((uint32_t)0x00000001)  /* Transmit interrupt enable */

/* Bit definition for Ethernet DMA Missed Frame and Buffer Overflow Counter Register */
#define ETH_DMAMFBOCR_OFOC   ((uint32_t)0x10000000)  /* Overflow bit for FIFO overflow counter */
#define ETH_DMAMFBOCR_MFA    ((uint32_t)0x0FFE0000)  /* Number of frames missed by the application */
#define ETH_DMAMFBOCR_OMFC   ((uint32_t)0x00010000)  /* Overflow bit for missed frame counter */
#define ETH_DMAMFBOCR_MFC    ((uint32_t)0x0000FFFF)  /* Number of frames missed by the controller */

/* Bit definition for Ethernet DMA Current Host Transmit Descriptor Register */
#define ETH_DMACHTDR_HTDAP   ((uint32_t)0xFFFFFFFF)  /* Host transmit descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Descriptor Register */
#define ETH_DMACHRDR_HRDAP   ((uint32_t)0xFFFFFFFF)  /* Host receive descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Transmit Buffer Address Register */
#define ETH_DMACHTBAR_HTBAP  ((uint32_t)0xFFFFFFFF)  /* Host transmit buffer address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Buffer Address Register */
#define ETH_DMACHRBAR_HRBAP  ((uint32_t)0xFFFFFFFF)  /* Host receive buffer address pointer */

/**
  *
  */

 /**
  * @}
  */ 

#ifdef USE_STDPERIPH_DRIVER
  #include "stm32f4xx_conf.h"
#endif /* USE_STDPERIPH_DRIVER */

/** @addtogroup Exported_macro
  * @{
  */
/** @addtogroup Exported_macro
  * @{
  */

/**
  * @brief  STM32F4系列寄存器操作基础宏集
  * @note   本组宏定义提供对MCU寄存器的原子级位操作，包含位设置、清除、读取、寄存器写/读等基础操作
  * @warning 所有参数必须为可寻址的左值(l-value)，使用前需确保寄存器地址有效性
  * @特征:
  * - 支持位掩码操作
  * - 严格遵循读-修改-写顺序
  * - 包含编译屏障保证操作原子性
  */

/* 位操作宏 ---------------------------------------------------------------*/
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))    /*!< 设置寄存器指定位为高电平 
                                                  * @param REG: 目标寄存器地址（如&GPIOA->ODR）
                                                  * @param BIT: 位掩码（如GPIO_PIN_5）
                                                  * @示例: SET_BIT(GPIOA->ODR, GPIO_PIN_5) // 设置PA5引脚输出高电平 */

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))  /*!< 清除寄存器指定位为低电平 
                                                  * @实现原理: 位与操作配合掩码取反
                                                  * @注意: 优先使用硬件复位寄存器代替软件清除
                                                  * @示例: CLEAR_BIT(ADC1->CR2, ADC_CR2_ADON) // 关闭ADC1 */

#define READ_BIT(REG, BIT)    ((REG) & (BIT))    /*!< 读取寄存器指定位状态 
                                                  * @返回值: 0表示位未置位，非0表示位已置位
                                                  * @典型应用: if(READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN)) */

/* 寄存器操作宏 -----------------------------------------------------------*/
#define CLEAR_REG(REG)        ((REG) = (0x0))    /*!< 清空整个寄存器 
                                                  * @警告: 慎用于可能包含保留位的寄存器
                                                  * @替代方案: 使用外设的硬件复位功能
                                                  * @应用场景: 初始化时清空控制寄存器 */

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))    /*!< 直接写寄存器 
                                                  * @特性: 覆盖式写入，不保留原值
                                                  * @适用场景: 配置寄存器初始值
                                                  * @注意: 确保写入值符合寄存器位定义 */

#define READ_REG(REG)         ((REG))            /*!< 直接读取寄存器值 
                                                  * @返回值: 寄存器当前32位数值
                                                  * @优化提示: 编译器可能优化为直接内存访问
                                                  * @典型应用: uint32_t reg_val = READ_REG(DMA1->ISR) */

/* 复合操作宏 -------------------------------------------------------------*/
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
                                                  /*!< 原子化修改寄存器位 
                                                  * @操作流程:
                                                  *   1. 读取当前寄存器值
                                                  *   2. 清除CLEARMASK指定位
                                                  *   3. 设置SETMASK指定位
                                                  *   4. 写回寄存器
                                                  * @优势: 避免读-修改-写过程中被中断打断
                                                  * @示例: MODIFY_REG(USART1->CR1, USART_CR1_PCE, USART_CR1_TE) */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F4xx_H */

/**
  * @}
  */

  /**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

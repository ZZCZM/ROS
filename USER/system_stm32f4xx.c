/**
  ******************************************************************************
  * @file    system_stm32f4xx.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File.
  *          This file contains the system clock configuration for STM32F4xx devices.
  *             
  * 1.  This file provides two functions and one global variable to be called from 
  *     user application:
  *      - SystemInit(): Setups the system clock (System clock source, PLL Multiplier
  *                      and Divider factors, AHB/APBx prescalers and Flash settings),
  *                      depending on the configuration made in the clock xls tool. 
  *                      This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f4xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  * 2. After each device reset the HSI (16 MHz) is used as system clock source.
  *    Then SystemInit() function is called, in "startup_stm32f4xx.s" file, to
  *    configure the system clock before to branch to main program.
  *
  * 3. If the system clock source selected by user fails to startup, the SystemInit()
  *    function will do nothing and HSI still used as system clock source. User can 
  *    add some code to deal with this issue inside the SetSysClock() function.
  *
  * 4. The default value of HSE crystal is set to 25MHz, refer to "HSE_VALUE" define
  *    in "stm32f4xx.h" file. When HSE is used as system clock source, directly or
  *    through PLL, and you are using different crystal you have to adapt the HSE
  *    value to your own configuration.
  *
  * 5. This file configures the system clock as follows:
  *=============================================================================
  *=============================================================================
  *                    Supported STM32F40xxx/41xxx devices
  *-----------------------------------------------------------------------------
  *        System Clock source                    | PLL (HSE)
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 168000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 168000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 4
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 2
  *-----------------------------------------------------------------------------
  *        HSE Frequency(Hz)                      | 25000000
  *-----------------------------------------------------------------------------
  *        PLL_M                                  | 25
  *-----------------------------------------------------------------------------
  *        PLL_N                                  | 336
  *-----------------------------------------------------------------------------
  *        PLL_P                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL_Q                                  | 7
  *-----------------------------------------------------------------------------
  *        PLLI2S_N                               | NA
  *-----------------------------------------------------------------------------
  *        PLLI2S_R                               | NA
  *-----------------------------------------------------------------------------
  *        I2S input clock                        | NA
  *-----------------------------------------------------------------------------
  *        VDD(V)                                 | 3.3
  *-----------------------------------------------------------------------------
  *        Main regulator output voltage          | Scale1 mode
  *-----------------------------------------------------------------------------
  *        Flash Latency(WS)                      | 5
  *-----------------------------------------------------------------------------
  *        Prefetch Buffer                        | ON
  *-----------------------------------------------------------------------------
  *        Instruction cache                      | ON
  *-----------------------------------------------------------------------------
  *        Data cache                             | ON
  *-----------------------------------------------------------------------------
  *        Require 48MHz for USB OTG FS,          | Disabled
  *        SDIO and RNG clock                     |
  *-----------------------------------------------------------------------------
  *=============================================================================
  *=============================================================================
  *                    Supported STM32F42xxx/43xxx devices
  *-----------------------------------------------------------------------------
  *        System Clock source                    | PLL (HSE)
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 180000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 180000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 4
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 2
  *-----------------------------------------------------------------------------
  *        HSE Frequency(Hz)                      | 25000000
  *-----------------------------------------------------------------------------
  *        PLL_M                                  | 25
  *-----------------------------------------------------------------------------
  *        PLL_N                                  | 360
  *-----------------------------------------------------------------------------
  *        PLL_P                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL_Q                                  | 7
  *-----------------------------------------------------------------------------
  *        PLLI2S_N                               | NA
  *-----------------------------------------------------------------------------
  *        PLLI2S_R                               | NA
  *-----------------------------------------------------------------------------
  *        I2S input clock                        | NA
  *-----------------------------------------------------------------------------
  *        VDD(V)                                 | 3.3
  *-----------------------------------------------------------------------------
  *        Main regulator output voltage          | Scale1 mode
  *-----------------------------------------------------------------------------
  *        Flash Latency(WS)                      | 5
  *-----------------------------------------------------------------------------
  *        Prefetch Buffer                        | ON
  *-----------------------------------------------------------------------------
  *        Instruction cache                      | ON
  *-----------------------------------------------------------------------------
  *        Data cache                             | ON
  *-----------------------------------------------------------------------------
  *        Require 48MHz for USB OTG FS,          | Disabled
  *        SDIO and RNG clock                     |
  *-----------------------------------------------------------------------------
  *=============================================================================
  *=============================================================================
  *                         Supported STM32F401xx devices
  *-----------------------------------------------------------------------------
  *        System Clock source                    | PLL (HSE)
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 84000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 84000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 2
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 1
  *-----------------------------------------------------------------------------
  *        HSE Frequency(Hz)                      | 25000000
  *-----------------------------------------------------------------------------
  *        PLL_M                                  | 25
  *-----------------------------------------------------------------------------
  *        PLL_N                                  | 336
  *-----------------------------------------------------------------------------
  *        PLL_P                                  | 4
  *-----------------------------------------------------------------------------
  *        PLL_Q                                  | 7
  *-----------------------------------------------------------------------------
  *        PLLI2S_N                               | NA
  *-----------------------------------------------------------------------------
  *        PLLI2S_R                               | NA
  *-----------------------------------------------------------------------------
  *        I2S input clock                        | NA
  *-----------------------------------------------------------------------------
  *        VDD(V)                                 | 3.3
  *-----------------------------------------------------------------------------
  *        Main regulator output voltage          | Scale1 mode
  *-----------------------------------------------------------------------------
  *        Flash Latency(WS)                      | 2
  *-----------------------------------------------------------------------------
  *        Prefetch Buffer                        | ON
  *-----------------------------------------------------------------------------
  *        Instruction cache                      | ON
  *-----------------------------------------------------------------------------
  *        Data cache                             | ON
  *-----------------------------------------------------------------------------
  *        Require 48MHz for USB OTG FS,          | Disabled
  *        SDIO and RNG clock                     |
  *-----------------------------------------------------------------------------
  *=============================================================================
  *=============================================================================
  *                         Supported STM32F411xx devices
  *-----------------------------------------------------------------------------
  *        System Clock source                    | PLL (HSI)
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 100000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 100000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 2
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 1
  *-----------------------------------------------------------------------------
  *        HSI Frequency(Hz)                      | 16000000
  *-----------------------------------------------------------------------------
  *        PLL_M                                  | 16
  *-----------------------------------------------------------------------------
  *        PLL_N                                  | 400
  *-----------------------------------------------------------------------------
  *        PLL_P                                  | 4
  *-----------------------------------------------------------------------------
  *        PLL_Q                                  | 7
  *-----------------------------------------------------------------------------
  *        PLLI2S_N                               | NA
  *-----------------------------------------------------------------------------
  *        PLLI2S_R                               | NA
  *-----------------------------------------------------------------------------
  *        I2S input clock                        | NA
  *-----------------------------------------------------------------------------
  *        VDD(V)                                 | 3.3
  *-----------------------------------------------------------------------------
  *        Main regulator output voltage          | Scale1 mode
  *-----------------------------------------------------------------------------
  *        Flash Latency(WS)                      | 3
  *-----------------------------------------------------------------------------
  *        Prefetch Buffer                        | ON
  *-----------------------------------------------------------------------------
  *        Instruction cache                      | ON
  *-----------------------------------------------------------------------------
  *        Data cache                             | ON
  *-----------------------------------------------------------------------------
  *        Require 48MHz for USB OTG FS,          | Disabled
  *        SDIO and RNG clock                     |
  *-----------------------------------------------------------------------------
  *=============================================================================
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

/** @addtogroup stm32f4xx_system
  * @{
  */  
  
/** @addtogroup STM32F4xx_System_Private_Includes
  * @{
  */

#include "stm32f4xx.h"

/**
  * @}
  */

/** @addtogroup STM32F4xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F4xx_System_Private_Defines
  * @{
  */

/************************* 杂项配置 ******************************************/
/*!< 取消注释以下行以使用外接SRAM/SDRAM作为数据存储器
  * @适用开发板：
  *    - STM324xG_EVAL（F40/41系列）
  *    - STM324x7I_EVAL（F427/437系列）
  *    - STM324x9I_EVAL（F429/439系列）
  * @硬件要求：
  *    - 确认FSMC/FMC接口已正确连接
  *    - 配置对应的GPIO时钟和复用功能
  * @注意：启用后需在启动文件初始化外部存储器控制器 */
#if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx)
/* #define DATA_IN_ExtSRAM */  // 启用128KB-1MB外部SRAM
#endif 

/* 高容量设备专用SDRAM配置 */
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
/* #define DATA_IN_ExtSDRAM */ // 启用8-16MB SDRAM（例如IS42S16400J）
#endif 

/************************* STM32F411专用配置 **********************************/
#if defined (STM32F411xE)    
/*!< 取消注释以通过ST-LINK的MCO引脚提供外部时钟
  * @应用场景：当板载HSE晶振不可用时
  * @硬件配置步骤（Nucleo开发板）：
  *    1. 移除SB54、SB55跳线帽（断开板载晶振）
  *    2. 拆焊R35电阻
  *    3. 连接SB16、SB50跳线帽
  * @限制：固定8MHz输入，不可调整频率 */
/* #define USE_HSE_BYPASS */  // HSE旁路模式

#if defined (USE_HSE_BYPASS)     
#define HSE_BYPASS_INPUT_FREQUENCY   8000000  // 精确匹配ST-LINK输出频率
#endif 
#endif 

/************************* 向量表配置 *****************************************/
/*!< 取消注释以将向量表重定位到内部SRAM
  * @典型应用：
  *    - 动态更新中断向量（如IAP升级）
  *    - 调试阶段快速修改异常处理
  * @内存地址：0x20000000（需确保SRAM容量足够） */
/* #define VECT_TAB_SRAM */  

/*!< 向量表基地址偏移量（需为0x200整数倍）
  * @应用场景：
  *    - 多固件分区方案（通过偏移量切换不同固件）
  *    - 配合bootloader实现安全启动
  * @示例：0x8000偏移对应32KB的bootloader空间 */
#define VECT_TAB_OFFSET  0x00  // 默认从Flash起始地址开始

/************************* PLL锁相环参数 **************************************/
/* PLL输入时钟 = (HSE或HSI时钟) / PLL_M
   PLL_VCO输出 = 输入时钟 × PLL_N
   @公式：VCO必须在192-432MHz范围内 */
#if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx) || defined (STM32F401xx)
#define PLL_M      25  // 适用于25MHz外部晶振：25MHz /25 = 1MHz → ×432 = 432MHz VCO
#else /* STM32F411xE */
  #if defined (USE_HSE_BYPASS)
  #define PLL_M      8   // 8MHz HSE输入：8MHz /8 = 1MHz → ×384 = 384MHz VCO
  #else    
  #define PLL_M      16  // 16MHz HSI输入：16MHz /16 = 1MHz → ×384 = 384MHz VCO
  #endif 
#endif 
/************************* PLL锁相环详细配置 *********************************/
/*!< USB OTG FS/SDIO/随机数生成器时钟源 = PLL_VCO / PLL_Q
  * @计算示例：当VCO=400MHz时，400/7 ≈ 57.14MHz
  * @限制条件：必须保证输出时钟不超过外设最大频率：
  *    - USB FS: 48MHz ±0.25%
  *    - SDIO: ≤48MHz
  *    - RNG: ≤48MHz */
#define PLL_Q      7  // 固定分频系数，不可随意修改

/*------------------------- STM32F40/41系列配置 -----------------------------*/
#if defined (STM32F40_41xxx)
/*!< PLL倍频系数（VCO = 输入时钟 × PLL_N）
  * @典型配置：当使用25MHz外部晶振时
  *    VCO输入 = 25MHz / PLL_M(25) = 1MHz 
  *    VCO输出 = 1MHz × 336 = 336MHz
  * @注意：VCO必须满足192MHz ≤ VCO ≤ 432MHz */
#define PLL_N      336  

/*!< 系统时钟分频系数（SYSCLK = VCO / PLL_P）
  * @计算结果：336MHz / 2 = 168MHz ← F40/41系列最大系统时钟 */
#define PLL_P      2  
#endif 

/*--------------------- STM32F427/437/429/439系列配置 ------------------------*/
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
/*!< 特殊配置支持180MHz系统时钟：
  *    VCO输入 = 25MHz /25 = 1MHz
  *    VCO输出 = 1MHz × 360 = 360MHz 
  *    SYSCLK = 360MHz /2 = 180MHz */
#define PLL_N      360  
#define PLL_P      2  // 保持与F40系列相同的分频比
#endif 

/*------------------------- STM32F401系列配置 --------------------------------*/
#if defined (STM32F401xx)
/*!< 针对84MHz最大频率优化：
  *    VCO输入 = 16MHz(HSI)/16 = 1MHz 
  *    VCO输出 = 1MHz × 336 = 336MHz
  *    SYSCLK = 336MHz /4 = 84MHz */
#define PLL_N      336  
#define PLL_P      4  // 更大分频比降低主频
#endif 

/*------------------------- STM32F411系列配置 --------------------------------*/
#if defined (STM32F411xE)
/*!< 实现100MHz系统时钟方案：
  * 当使用HSE旁路模式（8MHz输入）：
  *    VCO输入 = 8MHz /8 = 1MHz
  *    VCO输出 = 1MHz × 400 = 400MHz
  *    SYSCLK = 400MHz /4 = 100MHz
  * 当使用HSI（16MHz）时：
  *    VCO输入 = 16MHz /16 = 1MHz
  *    VCO输出 = 1MHz × 400 = 400MHz 
  *    SYSCLK = 400MHz /4 = 100MHz */
#define PLL_N      400  
#define PLL_P      4   // 保证USB时钟=400/7≈57.14MHz符合规范
#endif 


/******************************************************************************/

/**
  * @}
  */

/** @addtogroup STM32F4xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F4xx_System_Private_Variables
  * @{
  */
/** @addtogroup STM32F4xx_系统私有变量
  * @{
  */

/* 系统核心时钟全局变量（单位：Hz）------------------------------------------*/
#if defined (STM32F40_41xxx)
/*!< F40/41系列默认168MHz（最大工作频率）
  * 对应配置：HSE=25MHz → PLL_M=25, PLL_N=336, PLL_P=2
  * 168MHz = (25MHz /25) * 336 /2 */
uint32_t SystemCoreClock = 168000000;
#endif 

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
/*!< 高容量系列默认180MHz（工业级芯片支持）
  * 配置：HSE=25MHz → PLL_M=25, PLL_N=360, PLL_P=2
  * 180MHz = (25/25)*360/2 */
uint32_t SystemCoreClock = 180000000;
#endif 

#if defined (STM32F401xx)
/*!< F401系列默认84MHz（基于HSI时钟）
  * 配置：HSI=16MHz → PLL_M=16, PLL_N=336, PLL_P=4
  * 84MHz = (16/16)*336/4 */
uint32_t SystemCoreClock = 84000000;
#endif 

#if defined (STM32F411xE)
/*!< F411系列默认100MHz（支持USB高速模式）
  * 配置：HSE旁路8MHz → PLL_M=8, PLL_N=400, PLL_P=4
  * 100MHz = (8/8)*400/4 */
uint32_t SystemCoreClock = 100000000;
#endif 

/* AHB总线预分频系数表 ----------------------------------------------------*/
/*!< 寄存器值到分频因子的映射（HPRE[3:0]位域）
  * 索引：0-15对应寄存器设置的4位值
  * 数值：实际分频因子（0表示分频1，9表示分频512）
  * 使用场景：调用RCC_GetClocksFreq()时计算AHB时钟 */
__I uint8_t AHBPrescTable[16] = {
  0, 0, 0, 0,   // 0x0-0x3 → 1分频 (不分频)
  0, 0, 0, 0,   // 0x4-0x7 → 保留（实际同1分频）
  1, 2, 3, 4,   // 0x8-0xB → 2/4/8/16分频
  6, 7, 8, 9    // 0xC-0xF → 64/128/256/512分频
};

/** @} */

/** @addtogroup STM32F4xx_系统私有函数原型
  * @{
  */

/*!< 系统时钟配置核心函数（在SystemInit()中调用）
  * 功能：
  *   1. 切换时钟源（HSI/HSE）
  *   2. 配置PLL参数
  *   3. 设置Flash等待周期
  *   4. 更新SystemCoreClock变量 */
static void SetSysClock(void);

/* 外部存储器控制初始化条件编译 --------------------------------------------*/
#if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
/*!< FSMC/FMC控制器初始化函数
  * 功能：
  *   1. 配置GPIO复用功能
  *   2. 设置存储器时序参数
  *   3. 使能存储器控制器时钟
  * 调用链：SystemInit() → SystemInit_ExtMemCtl() */
static void SystemInit_ExtMemCtl(void); 
#endif


/**
  * @}
  */

/** @addtogroup STM32F4xx_System_Private_Functions
  * @{
  */
/**
  * @brief  微控制器系统初始化（内核第一个执行的函数）
  * @note   执行顺序：
  *         1. 复位时钟配置
  *         2. 初始化外部存储器（若启用）
  *         3. 配置主时钟系统
  *         4. 重定位中断向量表
  * @param  无
  * @retval 无
  */
void SystemInit(void)
{
  /* 浮点运算单元（FPU）配置 -------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    /* 启用CP10和CP11协处理器的完全访问权限（bit20-23）
     * 0x3UL << 10*2：设置CP10（浮点运算）访问权限为全模式
     * 0x3UL << 11*2：设置CP11（保留）访问权限 */
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
  #endif

  /* 复位时钟配置到默认状态 -------------------------------------------------*/
  /* 强制启用HSI（内部16MHz RC振荡器）作为安全时钟源 */
  RCC->CR |= (uint32_t)0x00000001;  // CR寄存器的bit0（HSION）置1

  /* 复位时钟配置寄存器（CFGR），清除所有分频/时钟源设置 */
  RCC->CFGR = 0x00000000;  // 所有位清零

  /* 禁用HSE、CSS（时钟安全系统）和PLL */
  RCC->CR &= (uint32_t)0xFEF6FFFF; // 清除以下位：
                                    // - bit16（HSEON）
                                    // - bit18（CSSON）
                                    // - bit24（PLLON）

  /* 复位PLL配置寄存器（PLLCFGR）到默认值：
     - PLLM = 2（bit0-5）
     - PLLN = 192（bit6-14）
     - PLLP = 0（分频因子2，bit16-17）
     - HSE不作为PLL输入（bit22=0） */
  RCC->PLLCFGR = 0x24003010;

  /* 禁用HSE旁路模式（使用外部晶振而非外部时钟源） */
  RCC->CR &= (uint32_t)0xFFFBFFFF; // 清除bit18（HSEBYP）

  /* 禁用所有时钟相关中断 */
  RCC->CIR = 0x00000000;  // 中断标志位清零

  /* 外部存储器控制器初始化（条件编译）---------------------------------------*/
  #if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
    /* 配置FSMC/FMC时序参数：
       - 初始化GPIO复用功能
       - 设置存储器访问时序
       - 使能存储控制器时钟 */
    SystemInit_ExtMemCtl();  // 必须在系统时钟配置前完成
  #endif

  /* 核心时钟系统配置 ------------------------------------------------------*/
  /* 配置以下参数：
     1. 选择时钟源（HSI/HSE）
     2. 设置PLL倍频/分频系数
     3. 配置AHB/APB预分频器
     4. 设置Flash等待周期 */
  SetSysClock();  // 具体实现在文件后部

  /* 中断向量表重定位配置 ---------------------------------------------------*/
  #ifdef VECT_TAB_SRAM
    /* 将向量表重定位到SRAM起始地址（0x20000000）
       应用场景：动态更新中断处理程序 */
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;  
  #else
    /* 默认向量表位于Flash起始地址（0x08000000）
       VECT_TAB_OFFSET可用于bootloader场景 */
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
  #endif
}


/**
   * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *     
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *             
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *                                              
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *                          
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**) 
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *         
  *         (*) HSI_VALUE is a constant defined in stm32f4xx.h file (default value
  *             16 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.   
  *    
  *         (**) HSE_VALUE is a constant defined in stm32f4xx.h file (default value
  *              25 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *                
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *     
  * @param  None
  * @retval None
  */
/**
  * @brief  更新系统核心时钟频率变量
  * @详细功能：
  *   1. 动态检测当前时钟源配置
  *   2. 根据RCC寄存器重新计算实际时钟频率
  *   3. 更新SystemCoreClock全局变量
  * @调用场景：
  *   - 系统时钟配置变更后（如切换HSI/HSE/PLL）
  *   - 低功耗模式唤醒后时钟恢复
  *   - 需要获取精确时钟值的应用场景
  */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;
  
  /*----------------------- 检测当前系统时钟源 -----------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;  // 读取时钟源选择位（SWS[1:0]）

  switch (tmp)
  {
    case 0x00:  /* HSI作为系统时钟 */
      SystemCoreClock = HSI_VALUE;  // 直接使用定义的HSI值（默认16MHz）
      break;
    case 0x04:  /* HSE作为系统时钟 */
      SystemCoreClock = HSE_VALUE;  // 依赖用户正确定义的HSE_VALUE
      break;
    case 0x08:  /* PLL作为系统时钟 */
      /* PLL参数提取 */
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22; // PLL输入源（0=HSI,1=HSE）
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;    // 获取PLL_M分频系数

      /* 多系列条件编译处理 */
#if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx) || defined (STM32F401xx)
      if (pllsource != 0) {  // HSE作为PLL输入
        pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      } else {                // HSI作为PLL输入
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
      }
#elif defined (STM32F411xE)
  #if defined (USE_HSE_BYPASS)
      if (pllsource != 0) {   // HSE旁路模式特殊处理
        pllvco = (HSE_BYPASS_INPUT_FREQUENCY / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }  
  #else  
      if (pllsource == 0) {   // F411默认HSI输入
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }  
  #endif 
#endif
      
      /* 计算PLL_P分频系数 */
      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;  // 00→P=2,01→P=4,etc
      SystemCoreClock = pllvco / pllp;  // 最终系统时钟频率
      break;
    default:
      SystemCoreClock = HSI_VALUE;  // 异常情况回退到HSI
      break;
  }

  /*----------------------- 计算AHB总线时钟 --------------------------------*/
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];  // 查表获取分频因子
  SystemCoreClock >>= tmp;  // 应用AHB预分频（HCLK = SYSCLK / AHB分频）
}
/**
  * @brief  配置系统时钟源及分频参数（PLL、AHB/APB总线、Flash设置）
  * @详细说明：
  *   1. 主流程：启用HSE → 配置PLL → 切换系统时钟源至PLL
  *   2. 支持STM32F40/41/F42/F429/F401/F411系列芯片配置
  *   3. 包含电压调节器配置、总线分频设置、Flash等待周期调整
  * @注意：
  *   必须在SystemInit()复位时钟配置后调用，全局仅调用一次
  * @参数：无
  * @返回值：无
  */
static void SetSysClock(void)
{
#if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx) || defined (STM32F401xx)
/******************************************************************************/
/*                      使用HSE驱动的PLL作为系统时钟源                        */
/******************************************************************************/
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* 启动HSE（外部高速晶振）*/
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);  // CR寄存器bit16置1，启用HSE
 
  /* 等待HSE就绪或超时（循环检查HSERDY标志）*/
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY; // 检查bit17（HSERDY）
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT)); // HSE_STARTUP_TIMEOUT=5000

  /* HSE状态确认 */
  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;  // 标记HSE启动成功
  }
  else
  {
    HSEStatus = (uint32_t)0x00;  // 标记HSE启动失败
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /*【电压调节器配置】Scale 1模式（高性能模式）*/
    RCC->APB1ENR |= RCC_APB1ENR_PWREN; // 使能PWR时钟（APB1ENR bit28）
    PWR->CR |= PWR_CR_VOS;             // 设置电压调节器输出等级（CR bit14）

    /*【总线分频配置】*/
    /* HCLK = SYSCLK /1（最大168/180MHz）*/
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;   // AHB预分频器=不分频
    
#if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx)      
    /* PCLK2 = HCLK /2（APB2最大84MHz）*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;  // APB2预分频bit13-15=100
    
    /* PCLK1 = HCLK /4（APB1最大42MHz）*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;  // APB1预分频bit10-12=100
#endif 

#if defined (STM32F401xx)
    /* F401特殊总线分频 */
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;  // APB2不分频（最大84MHz）
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;  // APB1二分频（最大42MHz）
#endif 

    /*【PLL参数配置】*/
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
    /* 寄存器位分解说明：
       - PLL_M[5:0]   : PLL输入分频系数（默认25）
       - PLL_N[14:6]  : PLL倍频系数（F40为336，F429为360）
       - PLL_P[17:16] : 系统时钟分频因子（(PLL_P>>1)-1计算，如P=2→0）
       - PLLSRC[22]   : 1=HSE作为PLL输入源
       - PLL_Q[27:24] : USB/SDIO时钟分频因子（默认7） */

    /* 启动PLL并等待锁定 */
    RCC->CR |= RCC_CR_PLLON;        // 使能PLL（CR bit24）
    while((RCC->CR & RCC_CR_PLLRDY) == 0)  // 等待PLLRDY置位
    {
    }
   
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
    /*【超频模式配置】将频率扩展至180MHz*/
    PWR->CR |= PWR_CR_ODEN;       // 启用过驱动模式（CR bit16）
    while((PWR->CSR & PWR_CSR_ODRDY) == 0)  // 等待过驱动就绪
    {
    }
    PWR->CR |= PWR_CR_ODSWEN;     // 切换到过驱动状态
    while((PWR->CSR & PWR_CSR_ODSWRDY) == 0) // 等待切换完成
    {
    }      
    
    /*【Flash配置】5个等待周期（用于180MHz）*/
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
#endif 

#if defined (STM32F40_41xxx)     
    /* 168MHz下Flash配置（5等待周期）*/
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
#endif 

#if defined (STM32F401xx)
    /* 84MHz下Flash配置（2等待周期）*/
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;
#endif 

    /* 切换系统时钟源到PLL */
    RCC->CFGR &= (uint32_t)(~(RCC_CFGR_SW));     // 清除时钟源选择位
    RCC->CFGR |= RCC_CFGR_SW_PLL;               // SW[1:0]=10选择PLL
    
    /* 等待时钟源切换完成 */
    while ((RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL) // 检查SWS位
    {
    }
  }
  else
  { 
    /* HSE启动失败处理（此处需用户添加错误处理代码）*/
  }
#elif defined (STM32F411xE)  // F411系列配置分支
#if defined (USE_HSE_BYPASS) 
/******************************************************************************/
/*                  使用HSE旁路模式下的PLL作为系统时钟源                     */
/******************************************************************************/
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* 启用HSE及旁路模式（直接使用外部时钟）*/
  RCC->CR |= ((uint32_t)RCC_CR_HSEON | RCC_CR_HSEBYP); 
 
  /* 等待HSE就绪（超时逻辑同前）*/
  do{
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  /* 后续配置流程类似F40系列... */
#else  /* F411使用HSI作为PLL时钟源 */
  /* 电压调节器配置（同上）*/
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR |= PWR_CR_VOS;
  
  /* 总线分频设置 */
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;   // HCLK=100MHz
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;  // APB2=100MHz 
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;  // APB1=50MHz
  
  /* PLL参数配置（使用HSI作为源）*/
  RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) | (PLL_Q << 24); 
  
  /* 启动PLL并等待就绪 */
  RCC->CR |= RCC_CR_PLLON;    
  while((RCC->CR & RCC_CR_PLLRDY) == 0){}
  
  /* Flash配置（2等待周期）*/
  FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;
  
  /* 切换至PLL时钟源 */
  RCC->CFGR &= ~RCC_CFGR_SW;         
  RCC->CFGR |= RCC_CFGR_SW_PLL;      
  while ((RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL){}
#endif /* USE_HSE_BYPASS */  
#endif /* 系列选择分支结束 */  
}
/**
  * @brief  外部SRAM存储器控制器初始化
  * @详细功能：
  *   1. 配置GPIO复用功能连接FSMC/FMC总线
  *   2. 设置外部SRAM存储器的访问时序
  *   3. 使能存储器控制器时钟
  * @应用场景：
  *   - 使用STM324xG_EVAL/STM324x7I开发板外接SRAM
  *   - 将外部SRAM作为数据存储器（包含堆栈空间）
  * @硬件连接：
  *   - 使用FSMC Bank1 NOR/PSRAM2（地址范围0x64000000）
  *   - 数据总线宽度16位，支持1MB寻址空间
  */
void SystemInit_ExtMemCtl(void)
{
/*-- GPIO配置 -------------------------------------------------------------*/
/*
 +-------------------+--------------------+------------------+--------------+
 | PD0  <-> FMC_D2  | PE0  <-> FMC_NBL0 | PF0  <-> FMC_A0 | PG0 <-> FMC_A10 | 
 | ...（详见代码注释）
*/
  /* 启用GPIOD/GPIOE/GPIOF/GPIOG时钟（AHB1总线）*/
  RCC->AHB1ENR   |= 0x00000078;  // bit3: GPIOD, bit4:GPIOE, bit5:GPIOF, bit6:GPIOG

  /* 配置GPIOD引脚 -------------------------------------------------------*/
  GPIOD->AFR[0]  = 0x00cc00cc;  // AF12 for PD0-7（FMC_D2/D3/NOE/NWE/D13-15）
  GPIOD->AFR[1]  = 0xcccccccc;  // AF12 for PD8-15（FMC_D13-15/A16-18/D0-1）
  GPIOD->MODER   = 0xaaaa0a0a;  // 引脚模式：PD0-7,PD8-15为复用功能模式（10）
  GPIOD->OSPEEDR = 0xffff0f0f;  // 输出速度：100MHz（11模式）
  GPIOD->OTYPER  = 0x00000000;  // 推挽输出
  GPIOD->PUPDR   = 0x00000000;  // 无上下拉

  /* 配置GPIOE引脚 -------------------------------------------------------*/
  GPIOE->AFR[0]  = 0xcccccccc;  // AF12 for PE0-7（FMC_NBL0-1/A19-20/D4-7）
  GPIOE->AFR[1]  = 0xcccccccc;  // AF12 for PE8-15（FMC_D5-12）
  GPIOE->MODER   = 0xaaaaaaaa;  // 全部引脚复用模式
  GPIOE->OSPEEDR = 0xffffffff;  // 100MHz速度
  GPIOE->OTYPER  = 0x00000000;  // 推挽输出
  GPIOE->PUPDR   = 0x00000000;  // 无上下拉

  /* 配置GPIOF引脚 -------------------------------------------------------*/
  GPIOF->AFR[0]  = 0x00cccccc;  // AF12 for PF0-5（FMC_A0-5）
  GPIOF->AFR[1]  = 0xcccc0000;  // AF12 for PF12-15（FMC_A6-9）
  GPIOF->MODER   = 0xaa000aaa;  // PF0-5,PF12-15为复用模式
  GPIOF->OSPEEDR = 0xff000fff;  // 100MHz速度
  GPIOF->OTYPER  = 0x00000000;  // 推挽输出
  GPIOF->PUPDR   = 0x00000000;  // 无上下拉

  /* 配置GPIOG引脚 -------------------------------------------------------*/
  GPIOG->AFR[0]  = 0x00cccccc;  // AF12 for PG0-5（FMC_A10-15/NE2）
  GPIOG->AFR[1]  = 0x000000c0;  // AF12 for PG9（FMC_NE2）
  GPIOG->MODER   = 0x00080aaa;  // PG0-5,PG9为复用模式
  GPIOG->OSPEEDR = 0x000c0fff;  // 100MHz速度
  GPIOG->OTYPER  = 0x00000000;  // 推挽输出
  GPIOG->PUPDR   = 0x00000000;  // 无上下拉

/*-- 存储器控制器配置 ----------------------------------------------------*/
  /* 启用FMC/FSMC时钟（AHB3总线）*/
  RCC->AHB3ENR |= 0x00000001;  // bit0: FMC/FSMCEN

/* 不同芯片系列配置 */
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
  /* FMC Bank1 NORSRAM2时序配置（SRAM型号：IS61WV51216BLL-10TLI）*/
  FMC_Bank1->BTCR[2]  = 0x00001011;  // 存储器类型：SRAM,数据宽度16位
                                     // 地址建立时间=1个HCLK周期（bit13-16）
  FMC_Bank1->BTCR[3]  = 0x00000201;  // 数据保持时间=2个周期（bit8-15）
  FMC_Bank1E->BWTR[2] = 0x0fffffff;  // 写时序控制：关闭扩展模式

#elif defined (STM32F40_41xxx)
  /* FSMC Bank1配置（与FMC寄存器名称不同但功能相似）*/
  FSMC_Bank1->BTCR[2]  = 0x00001011;  // 模式：Mode A，关闭CLK输出
  FSMC_Bank1->BTCR[3]  = 0x00000201;  // 数据阶段持续时间=2周期
  FSMC_Bank1E->BWTR[2] = 0x0fffffff;  // 写操作时序配置
#endif

/* 等效配置参数说明（与寄存器设置对应）：
   AddressSetupTime      = 1    -- 地址建立时间1个时钟周期
   DataSetupTime         = 2    -- 数据建立时间2个周期
   BusTurnAroundDuration = 0    -- 总线转换周期
   CLKDivision           = 0    -- 时钟分频（禁用）
   AccessMode            = Mode A -- 访问模式A（独立的读写时序）
   DataWidth             = 16bit -- 16位数据总线
   WriteOperation        = Enable -- 启用写操作
   MemoryType            = SRAM -- 存储器类型
*/
}

  
#ifdef DATA_IN_ExtSDRAM
/**
  * @brief  Setup the external memory controller.
  *         Called in startup_stm32f4xx.s before jump to main.
  *         This function configures the external SDRAM mounted on STM324x9I_EVAL board
  *         This SDRAM will be used as program data memory (including heap and stack).
  * @param  None
  * @retval None
  */
void SystemInit_ExtMemCtl(void)
{
  register uint32_t tmpreg = 0, timeout = 0xFFFF;
  register uint32_t index;

  /* Enable GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH and GPIOI interface 
      clock */
  RCC->AHB1ENR |= 0x000001FC;
  
  /* Connect PCx pins to FMC Alternate function */
  GPIOC->AFR[0]  = 0x0000000c;
  GPIOC->AFR[1]  = 0x00007700;
  /* Configure PCx pins in Alternate function mode */  
  GPIOC->MODER   = 0x00a00002;
  /* Configure PCx pins speed to 50 MHz */  
  GPIOC->OSPEEDR = 0x00a00002;
  /* Configure PCx pins Output type to push-pull */  
  GPIOC->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PCx pins */ 
  GPIOC->PUPDR   = 0x00500000;
  
  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0x000000CC;
  GPIOD->AFR[1]  = 0xCC000CCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xA02A000A;
  /* Configure PDx pins speed to 50 MHz */  
  GPIOD->OSPEEDR = 0xA02A000A;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PDx pins */ 
  GPIOD->PUPDR   = 0x00000000;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00000CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAA800A;
  /* Configure PEx pins speed to 50 MHz */ 
  GPIOE->OSPEEDR = 0xAAAA800A;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PEx pins */ 
  GPIOE->PUPDR   = 0x00000000;

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0xcccccccc;
  GPIOF->AFR[1]  = 0xcccccccc;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAA800AAA;
  /* Configure PFx pins speed to 50 MHz */ 
  GPIOF->OSPEEDR = 0xAA800AAA;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PFx pins */ 
  GPIOF->PUPDR   = 0x00000000;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0xcccccccc;
  GPIOG->AFR[1]  = 0xcccccccc;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0xaaaaaaaa;
  /* Configure PGx pins speed to 50 MHz */ 
  GPIOG->OSPEEDR = 0xaaaaaaaa;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PGx pins */ 
  GPIOG->PUPDR   = 0x00000000;
  
  /* Connect PHx pins to FMC Alternate function */
  GPIOH->AFR[0]  = 0x00C0CC00;
  GPIOH->AFR[1]  = 0xCCCCCCCC;
  /* Configure PHx pins in Alternate function mode */ 
  GPIOH->MODER   = 0xAAAA08A0;
  /* Configure PHx pins speed to 50 MHz */ 
  GPIOH->OSPEEDR = 0xAAAA08A0;
  /* Configure PHx pins Output type to push-pull */  
  GPIOH->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PHx pins */ 
  GPIOH->PUPDR   = 0x00000000;
  
  /* Connect PIx pins to FMC Alternate function */
  GPIOI->AFR[0]  = 0xCCCCCCCC;
  GPIOI->AFR[1]  = 0x00000CC0;
  /* Configure PIx pins in Alternate function mode */ 
  GPIOI->MODER   = 0x0028AAAA;
  /* Configure PIx pins speed to 50 MHz */ 
  GPIOI->OSPEEDR = 0x0028AAAA;
  /* Configure PIx pins Output type to push-pull */  
  GPIOI->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PIx pins */ 
  GPIOI->PUPDR   = 0x00000000;
  
/*-- FMC Configuration ------------------------------------------------------*/
  /* Enable the FMC interface clock */
  RCC->AHB3ENR |= 0x00000001;
  
  /* Configure and enable SDRAM bank1 */
  FMC_Bank5_6->SDCR[0] = 0x000039D0;
  FMC_Bank5_6->SDTR[0] = 0x01115351;      
  
  /* SDRAM initialization sequence */
  /* Clock enable command */
  FMC_Bank5_6->SDCMR = 0x00000011; 
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  while((tmpreg != 0) & (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }
  
  /* Delay */
  for (index = 0; index<1000; index++);
  
  /* PALL command */
  FMC_Bank5_6->SDCMR = 0x00000012;           
  timeout = 0xFFFF;
  while((tmpreg != 0) & (timeout-- > 0))
  {
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }
  
  /* Auto refresh command */
  FMC_Bank5_6->SDCMR = 0x00000073;
  timeout = 0xFFFF;
  while((tmpreg != 0) & (timeout-- > 0))
  {
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }
 
  /* MRD register program */
  FMC_Bank5_6->SDCMR = 0x00046014;
  timeout = 0xFFFF;
  while((tmpreg != 0) & (timeout-- > 0))
  {
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  } 
  
  /* Set refresh count */
  tmpreg = FMC_Bank5_6->SDRTR;
  FMC_Bank5_6->SDRTR = (tmpreg | (0x0000027C<<1));
  
  /* Disable write protection */
  tmpreg = FMC_Bank5_6->SDCR[0]; 
  FMC_Bank5_6->SDCR[0] = (tmpreg & 0xFFFFFDFF);
  
/*
  Bank1_SDRAM is configured as follow:

  FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay = 2;      
  FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 6;  
  FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime = 4;        
  FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay = 6;         
  FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime = 2;      
  FMC_SDRAMTimingInitStructure.FMC_RPDelay = 2;                
  FMC_SDRAMTimingInitStructure.FMC_RCDDelay = 2;               

  FMC_SDRAMInitStructure.FMC_Bank = SDRAM_BANK;
  FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;
  FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_11b;
  FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b;
  FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
  FMC_SDRAMInitStructure.FMC_CASLatency = FMC_CAS_Latency_3; 
  FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;
  FMC_SDRAMInitStructure.FMC_SDClockPeriod = FMC_SDClock_Period_2;
  FMC_SDRAMInitStructure.FMC_ReadBurst = FMC_Read_Burst_disable;
  FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
  FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;
*/
  
}
#endif /* DATA_IN_ExtSDRAM */


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

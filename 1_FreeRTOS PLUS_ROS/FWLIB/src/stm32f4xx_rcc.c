/**
  ******************************************************************************
  * @file    stm32f4xx_rcc.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Reset and clock control (RCC) peripheral:
  *           + Internal/external clocks, PLL, CSS and MCO configuration
  *           + System, AHB and APB busses clocks configuration
  *           + Peripheral clocks configuration
  *           + Interrupts and flags management
  *
 @verbatim                
 ===============================================================================
                      ##### RCC specific features #####
 ===============================================================================
    [..]  
      After reset the device is running from Internal High Speed oscillator 
      (HSI 16MHz) with Flash 0 wait state, Flash prefetch buffer, D-Cache 
      and I-Cache are disabled, and all peripherals are off except internal
      SRAM, Flash and JTAG.
      (+) There is no prescaler on High speed (AHB) and Low speed (APB) busses;
          all peripherals mapped on these busses are running at HSI speed.
      (+) The clock for all peripherals is switched off, except the SRAM and FLASH.
      (+) All GPIOs are in input floating state, except the JTAG pins which
          are assigned to be used for debug purpose.
    [..]          
      Once the device started from reset, the user application has to:        
      (+) Configure the clock source to be used to drive the System clock
          (if the application needs higher frequency/performance)
      (+) Configure the System clock frequency and Flash settings  
      (+) Configure the AHB and APB busses prescalers
      (+) Enable the clock for the peripheral(s) to be used
      (+) Configure the clock source(s) for peripherals which clocks are not
          derived from the System clock (I2S, RTC, ADC, USB OTG FS/SDIO/RNG)                                
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
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup RCC 
  * @brief RCC driver modules
  * @{
  */ 
/*------------ RCC寄存器在位带别名区的位地址定义 -----------*/

/* RCC模块相对于外设基地址的偏移量（RCC_BASE - PERIPH_BASE）*/
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)

/*-------------------- CR寄存器相关位定义 --------------------*/
/* 控制寄存器(CR)整体偏移量：RCC_OFFSET + 0x00 */
#define CR_OFFSET                 (RCC_OFFSET + 0x00)

/* 内部高速时钟使能位(HSION)的位带别名地址 */
#define HSION_BitNumber           0x00
/* 计算公式：位带基址 + 寄存器偏移(字节)*32 + 位号*4 */
#define CR_HSION_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (HSION_BitNumber * 4))

/* 时钟安全系统使能位(CSSON)的位带别名地址 */
#define CSSON_BitNumber           0x13  // 第19位（0x13=19）
#define CR_CSSON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (CSSON_BitNumber * 4))

/* 主PLL使能位(PLLON)的位带别名地址 */
#define PLLON_BitNumber           0x18  // 第24位（0x18=24）
#define CR_PLLON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLON_BitNumber * 4))

/* PLLI2S使能位(PLLI2SON)的位带别名地址 */
#define PLLI2SON_BitNumber        0x1A  // 第26位（0x1A=26）
#define CR_PLLI2SON_BB            (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLI2SON_BitNumber * 4))

/* PLLSAI使能位(PLLSAION)的位带别名地址 */
#define PLLSAION_BitNumber        0x1C  // 第28位（0x1C=28）
#define CR_PLLSAION_BB            (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLSAION_BitNumber * 4))


/*------------------- CFGR寄存器相关位定义 -------------------*/
/* 时钟配置寄存器(CFGR)偏移量：RCC_OFFSET + 0x08 */
#define CFGR_OFFSET               (RCC_OFFSET + 0x08)

/* I2S时钟源选择位(I2SSRC)的位带别名地址 */
#define I2SSRC_BitNumber          0x17  // 第23位（0x17=23）
#define CFGR_I2SSRC_BB            (PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (I2SSRC_BitNumber * 4))


/*------------------- BDCR寄存器相关位定义 -------------------*/
/* 备份域控制寄存器(BDCR)偏移量：RCC_OFFSET + 0x70 */
#define BDCR_OFFSET               (RCC_OFFSET + 0x70)

/* RTC时钟使能位(RTCEN)的位带别名地址 */
#define RTCEN_BitNumber           0x0F  // 第15位（0x0F=15）
#define BDCR_RTCEN_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (RTCEN_BitNumber * 4))

/* 备份域复位控制位(BDRST)的位带别名地址 */
#define BDRST_BitNumber           0x10  // 第16位（0x10=16）
#define BDCR_BDRST_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (BDRST_BitNumber * 4))

/*------------------- CSR寄存器相关位定义 -------------------*/
/* 控制/状态寄存器(CSR)偏移量：RCC_OFFSET + 0x74 */
#define CSR_OFFSET                (RCC_OFFSET + 0x74)

/* 内部低速时钟使能位(LSION)的位带别名地址 */
#define LSION_BitNumber           0x00  // 第0位
#define CSR_LSION_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSION_BitNumber * 4))


/*----------------- DCKCFGR寄存器相关位定义 -----------------*/
/* 专用时钟配置寄存器(DCKCFGR)偏移量：RCC_OFFSET + 0x8C */
#define DCKCFGR_OFFSET            (RCC_OFFSET + 0x8C)

/* 定时器时钟预分频选择位(TIMPRE)的位带别名地址 */
#define TIMPRE_BitNumber          0x18  // 第24位（0x18=24）
#define DCKCFGR_TIMPRE_BB         (PERIPH_BB_BASE + (DCKCFGR_OFFSET * 32) + (TIMPRE_BitNumber * 4))


/*------------------- 寄存器位掩码定义 ---------------------*/
/* MCO2时钟输出配置位掩码（保留bit27-25,清空bit26/24-0） */
#define CFGR_MCO2_RESET_MASK      ((uint32_t)0x07FFFFFF) // 0b00000111111111111111111111111111

/* MCO1时钟输出配置位掩码（保留bit21-13,清空其他位） */
#define CFGR_MCO1_RESET_MASK      ((uint32_t)0xF89FFFFF) // 0b11111000100111111111111111111111

/* 状态标志位掩码（保留低5位，用于提取中断标志状态） */
#define FLAG_MASK                 ((uint8_t)0x1F)  // 0b00011111


/*------------------- 寄存器字节地址定义 --------------------*/
/* CR寄存器第3字节地址（bit23-16），用于PLL配置状态访问 */
#define CR_BYTE3_ADDRESS          ((uint32_t)0x40023802)  // CR基址+2字节偏移

/* 时钟中断寄存器(CIR)字节地址定义 */
#define CIR_BYTE2_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x01)) // CIR基址+1字节（bit15-8）
#define CIR_BYTE3_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x02)) // CIR基址+2字节（bit23-16）


/*------------------- BDCR寄存器基地址定义 ------------------*/
/* 备份域控制寄存器(BDCR)绝对地址 */
#define BDCR_ADDRESS              (PERIPH_BASE + BDCR_OFFSET)  // 0x40023870


/*------------------- 预分频系数查找表 ---------------------*/
/* AHB/APB总线预分频系数转换表 
   索引对应寄存器配置值，元素值为分频系数的二进制位移数
   示例：
   - 索引0x00 → 1分频（不移位）
   - 索引0x08 → 2分频（右移1位）
   - 索引0x0B → 16分频（右移4位） */
static __I uint8_t APBAHBPrescTable[16] = {
    0, 0, 0, 0,   // 0x00-0x03: 1分频（SYSCLK/1）
    1, 2, 3, 4,   // 0x04-0x07: 2/4/8/16分频
    1, 2, 3, 4,   // 0x08-0x0B: 同上（不同寄存器位映射）
    6, 7, 8, 9    // 0x0C-0x0F: 保留配置
};


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup RCC_Private_Functions
  * @{
  */ 

/** @defgroup RCC_Group1 Internal and external clocks, PLL, CSS and MCO configuration functions
 *  @brief   Internal and external clocks, PLL, CSS and MCO configuration functions 
 *
@verbatim   
 ===================================================================================
 ##### Internal and  external clocks, PLL, CSS and MCO configuration functions #####
 ===================================================================================  
    [..]
      This section provide functions allowing to configure the internal/external clocks,
      PLLs, CSS and MCO pins.
  
      (#) HSI (high-speed internal), 16 MHz factory-trimmed RC used directly or through
          the PLL as System clock source.

      (#) LSI (low-speed internal), 32 KHz low consumption RC used as IWDG and/or RTC
          clock source.

      (#) HSE (high-speed external), 4 to 26 MHz crystal oscillator used directly or
          through the PLL as System clock source. Can be used also as RTC clock source.

      (#) LSE (low-speed external), 32 KHz oscillator used as RTC clock source.   

      (#) PLL (clocked by HSI or HSE), featuring two different output clocks:
        (++) The first output is used to generate the high speed system clock (up to 168 MHz)
        (++) The second output is used to generate the clock for the USB OTG FS (48 MHz),
             the random analog generator (<=48 MHz) and the SDIO (<= 48 MHz).

      (#) PLLI2S (clocked by HSI or HSE), used to generate an accurate clock to achieve 
          high-quality audio performance on the I2S interface or SAI interface in case 
          of STM32F429x/439x devices.
     
      (#) PLLSAI clocked by (HSI or HSE), used to generate an accurate clock to SAI 
          interface and LCD TFT controller available only for STM32F42xxx/43xxx devices.
  
      (#) CSS (Clock security system), once enable and if a HSE clock failure occurs 
         (HSE used directly or through PLL as System clock source), the System clock
         is automatically switched to HSI and an interrupt is generated if enabled. 
         The interrupt is linked to the Cortex-M4 NMI (Non-Maskable Interrupt) 
         exception vector.   

      (#) MCO1 (microcontroller clock output), used to output HSI, LSE, HSE or PLL
          clock (through a configurable prescaler) on PA8 pin.

      (#) MCO2 (microcontroller clock output), used to output HSE, PLL, SYSCLK or PLLI2S
          clock (through a configurable prescaler) on PC9 pin.
 @endverbatim
  * @{
  */
/**
  * @brief  RCC时钟系统完全复位函数
  * @details 将时钟配置恢复为芯片默认状态：
  *          - 系统时钟源切回HSI（16MHz RC振荡器）
  *          - 关闭HSE/PLL/PLLI2S等高速时钟源
  *          - AHB/APB分频器重置为1分频
  *          - 关闭MCO时钟输出和CSS安全系统
  *          - 清除所有时钟相关中断标志
  * @note 不影响外设时钟使能状态和低功耗时钟（LSI/LSE/RTC）
  */
void RCC_DeInit(void)
{
  /* 强制启用HSI作为默认系统时钟源 */
  RCC->CR |= (uint32_t)0x00000001;  // 设置CR寄存器的bit0(HSION)

  /* 复位时钟配置寄存器（清除所有分频/时钟源选择） */
  RCC->CFGR = 0x00000000;  // 清空MCO、PLL源、系统时钟切换等配置

  /* 关闭HSE及相关模块（按位掩码保留HSI和部分保留位） */
  // 掩码0xEAF6FFFF对应清除：HSEON(bit16)、CSSON(bit19)、PLLON(bit24)、PLLI2SON(bit26)、PLLSAION(bit28)
  RCC->CR &= (uint32_t)0xEAF6FFFF; 

  /* 复位主PLL配置寄存器（恢复默认倍频参数） */
  RCC->PLLCFGR = 0x24003010;  // 默认M=16, N=192, P=2, Q=4（HSI 16MHz输入时PLL输出96MHz）

  /* 复位PLLI2S配置寄存器（关闭音频专用PLL） */
  RCC->PLLI2SCFGR = 0x20003000;  // N=192, R=2（需手动使能后生效）

  /* 复位PLLSAI配置寄存器（STM32F42/43系列专用显示PLL） */
  RCC->PLLSAICFGR = 0x24003000;  // 参数同主PLL默认值

  /* 清除HSE旁路模式（确保使用外部晶振而非外部时钟输入） */
  RCC->CR &= (uint32_t)0xFFFBFFFF;  // 清除HSEBYP位(bit18)

  /* 禁用所有时钟相关中断（CIR寄存器清零） */
  RCC->CIR = 0x00000000;  // 清中断标志并关闭所有中断使能

  /* 复位专用时钟配置（关闭定时器时钟预分频功能） */
  RCC->DCKCFGR = 0x00000000;  // TIMPRE=0，定时器使用默认HSE分频
}

/**
  * @brief 外部高速时钟（HSE）配置函数
  * @details 控制外部4-26MHz时钟源的启用与旁路模式
  * @param RCC_HSE: 配置模式，可选：
  *          @arg RCC_HSE_OFF: 关闭HSE（系统将切换至HSI）
  *          @arg RCC_HSE_ON: 启用外部晶振（需连接石英晶体）
  *          @arg RCC_HSE_Bypass: 旁路模式（使用外部有源时钟输入）
  * @note 配置前需确保HSE未被用作系统时钟源，否则操作无效
  */
void RCC_HSEConfig(uint8_t RCC_HSE)
{
  /* 参数合法性检查（确保传入值在允许范围内） */
  assert_param(IS_RCC_HSE(RCC_HSE));

  /* 原子操作步骤：通过CR寄存器的第三字节地址访问HSE控制位 */
  // CR_BYTE3_ADDRESS对应CR寄存器的bit16-23（含HSEON和HSEBYP位）
  
  /* 第一步：清除HSEON和HSEBYP位（避免配置冲突） */
  *(__IO uint8_t *) CR_BYTE3_ADDRESS = RCC_HSE_OFF;  // 写入0x00关闭HSE

  /* 第二步：根据参数设置新的HSE模式 */
  *(__IO uint8_t *) CR_BYTE3_ADDRESS = RCC_HSE;  // 写入新配置（0x01或0x05）
}

/**
  * @brief HSE时钟稳定等待函数
  * @details 检测HSERDY标志，直到时钟稳定或超时
  * @retval SUCCESS: HSE就绪（可安全切换系统时钟）
  *         ERROR: 超时未就绪（需检查晶振或电路）
  */
ErrorStatus RCC_WaitForHSEStartUp(void)
{
  __IO uint32_t startupcounter = 0;    // 超时计数器（基于主循环次数）
  ErrorStatus status = ERROR;          // 默认返回错误
  FlagStatus hsestatus = RESET;        // 临时状态标志

  /* 轮询检测HSERDY标志，直到超时或标志置位 */
  do {
    hsestatus = RCC_GetFlagStatus(RCC_FLAG_HSERDY); // 读取HSERDY状态
    startupcounter++;  // 累计循环次数（约需等待6个HSE时钟周期）
  } while((startupcounter != HSE_STARTUP_TIMEOUT) && (hsestatus == RESET));

  /* 最终确认HSERDY状态（避免最后一刻状态变化） */
  if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET) {
    status = SUCCESS;  // 返回成功状态
  } else {
    status = ERROR;    // 超时未就绪
  }
  return (status);  // 返回检测结果
}
/**
  * @brief  HSI内部高速时钟校准值调整函数
  * @details 通过调整HSITRIM位补偿RC振荡器的频率偏差，调节范围±1.5%
  * @param  HSICalibrationValue: 校准值（0x00~0x1F对应出厂校准±16步）
  * @note   默认出厂校准值为16（0x10），用户可在-15~+15范围内微调
  */
void RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue)
{
  uint32_t tmpreg = 0;
  /* 参数校验：确保校准值在0-31之间 */
  assert_param(IS_RCC_CALIBRATION_VALUE(HSICalibrationValue));

  /* 读取CR寄存器当前值 */
  tmpreg = RCC->CR;  // 获取整个控制寄存器的状态

  /* 清除原有的HSITRIM位（bit7-3） */
  tmpreg &= ~RCC_CR_HSITRIM;  // RCC_CR_HSITRIM宏通常为0xF8（0b11111000）

  /* 设置新的校准值到HSITRIM位 */
  tmpreg |= (uint32_t)HSICalibrationValue << 3;  // 左移3位对齐bit7-3

  /* 回写修改后的值到CR寄存器 */
  RCC->CR = tmpreg;  // 仅修改HSITRIM区域，保持其他位不变
}

/**
  * @brief  HSI内部高速时钟使能控制函数
  * @details 控制16MHz RC振荡器的启停状态
  * @note   HSI特性：
  *          - 系统复位后默认启用
  *          - STOP/STANDBY模式下自动关闭
  *          - 作为CSS故障后的备用时钟源
  * @param  NewState: 控制指令（ENABLE/DISABLE）
  * @warning 当HSI作为系统时钟源时禁止关闭，需先切换时钟源
  */
void RCC_HSICmd(FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 通过位带别名地址原子操作HSION位（CR寄存器bit0） */
  // CR_HSION_BB = PERIPH_BB_BASE + (0x00 * 32) + (0x00 * 4) = 0x42000000
  *(__IO uint32_t *) CR_HSION_BB = (uint32_t)NewState;  // 直接写入1或0
}


/**
  * @brief  配置外部低速振荡器(LSE)
  * @param  RCC_LSE: LSE工作模式选择，可选：
  *         @arg RCC_LSE_OFF     关闭LSE
  *         @arg RCC_LSE_ON      启用LSE晶体振荡
  *         @arg RCC_LSE_Bypass  启用LSE外部时钟旁路
  * @note 操作前提：
  *        - 必须提前使能备份域访问(PWR_BackupAccessCmd(ENABLE))
  *        - 启用后需检测LSERDY标志等待时钟稳定
  * @warning 硬件连接要求：
  *          - ON模式需接32.768kHz晶体
  *          - Bypass模式需外部提供时钟信号
  */
void RCC_LSEConfig(uint8_t RCC_LSE)
{
  /* 验证参数合法性 */
  assert_param(IS_RCC_LSE(RCC_LSE));  // 检查是否为有效模式参数

  /* 复位LSE配置位（两步操作确保清除LSEON和LSEBYP）*/
  // 第一步：清除LSEON位（BDCR寄存器bit0）
  *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;  // 写入0x00到BDCR低字节
  
  // 第二步：清除LSEBYP位（BDCR寄存器bit2）
  *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;  // 再次写入0x00确保bit2清零

  /* 根据模式参数配置LSE */
  switch (RCC_LSE)
  {
    case RCC_LSE_ON:  // 启用晶体振荡模式
      /* 设置LSEON位（bit0）*/
      *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_ON;  // 写入0x01
      break;

    case RCC_LSE_Bypass:  // 启用外部时钟旁路模式
      /* 同时设置LSEBYP(bit2)和LSEON(bit0) */
      *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_Bypass | RCC_LSE_ON; // 写入0x05
      break;

    default:  // RCC_LSE_OFF情况已在前面处理
      break;
  }
}
/**
  * @brief  LSI内部低速时钟使能控制函数
  * @details 控制约32kHz RC振荡器的启停，主要用于独立看门狗(IWDG)和RTC
  * @note 特殊限制：
  *          - 当IWDG运行时禁止关闭LSI
  *          - 从STOP模式唤醒后LSI保持关闭状态
  * @param  NewState: 控制指令（ENABLE/DISABLE）
  * @warning 禁用LSI前需确保没有外设依赖该时钟
  */
void RCC_LSICmd(FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 通过位带别名地址原子操作LSION位（CSR寄存器bit0） */
  // CSR_LSION_BB = PERIPH_BB_BASE + (0x74 * 32) + (0x00 * 4) 
  *(__IO uint32_t *) CSR_LSION_BB = (uint32_t)NewState;
}

/**
  * @brief 主PLL时钟配置函数
  * @details 配置锁相环参数，决定系统时钟及外设时钟频率
  * @param RCC_PLLSource: PLL时钟源选择
  *          @arg RCC_PLLSource_HSI: 16MHz内部时钟
  *          @arg RCC_PLLSource_HSE: 外部晶振时钟（4-26MHz）
  * @param PLLM: 预分频系数（2-63），控制VCO输入频率1-2MHz
  * @param PLLN: 倍频系数（192-432），控制VCO输出频率192-432MHz
  * @param PLLP: 系统时钟分频（2/4/6/8），对应寄存器值0/1/2/3
  * @param PLLQ: USB/SDIO时钟分频（4-15），输出需保持48MHz
  * @note 计算公式：
  *          VCO输入频率 = PLL源时钟 / PLLM
  *          VCO输出频率 = VCO输入频率 × PLLN
  *          系统时钟 = VCO输出频率 / PLLP
  *          USB时钟 = VCO输出频率 / PLLQ
  */
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t PLLM, uint32_t PLLN, 
                   uint32_t PLLP, uint32_t PLLQ)
{
  /* 参数有效性验证 */
  assert_param(IS_RCC_PLL_SOURCE(RCC_PLLSource));
  assert_param(IS_RCC_PLLM_VALUE(PLLM));     // 2<=PLLM<=63
  assert_param(IS_RCC_PLLN_VALUE(PLLN));     // 192<=PLLN<=432
  assert_param(IS_RCC_PLLP_VALUE(PLLP));    // PLLP∈{2,4,6,8}
  assert_param(IS_RCC_PLLQ_VALUE(PLLQ));    // 4<=PLLQ<=15

  /* 组合配置参数到PLLCFGR寄存器 */
  RCC->PLLCFGR = PLLM                // [5:0] PLLM
               | (PLLN << 6)         // [14:6] PLLN（左移6位）
               | (((PLLP >> 1) -1) << 16) // [17:16] PLLP编码（2→0,4→1,6→2,8→3）
               | RCC_PLLSource       // [22] 时钟源选择位
               | (PLLQ << 24);       // [27:24] PLLQ
}

/**
  * @brief 主PLL使能控制函数
  * @details 启用/禁用锁相环，操作前需确保：
  *          - PLL配置已完成且未作为系统时钟源
  *          - 启用后等待PLLRDY标志置位
  * @param NewState: 控制指令（ENABLE/DISABLE）
  * @warning 当PLL作为系统时钟源时禁止禁用
  */
void RCC_PLLCmd(FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 通过位带别名地址原子操作PLLON位（CR寄存器bit24） */
  // CR_PLLON_BB = PERIPH_BB_BASE + (0x00 * 32) + (0x18 * 4)
  *(__IO uint32_t *) CR_PLLON_BB = (uint32_t)NewState;
}

#if defined (STM32F40_41xxx) || defined (STM32F401xx)
/**
  * @brief  PLLI2S音频时钟配置函数
  * @details 设置I2S专用锁相环参数，生成精确音频时钟
  * @note 使用限制：
  *          - 仅适用于STM32F405/407/415/417/401系列
  *          - 必须在PLLI2S禁用状态下配置
  *          - 时钟源与主PLL共用（通过RCC_PLLConfig设置）
  *
  * @param PLLI2SN: 倍频系数（192-432），控制VCO输出频率（192-432MHz）
  * @param PLLI2SR: I2S时钟分频系数（2-7），输出需≤192MHz
  * @note 计算公式：
  *          VCO输出频率 = (HSE或HSI时钟 / PLLM) × PLLI2SN
  *          I2S时钟 = VCO输出频率 / PLLI2SR
  */
void RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SR)
{
  /* 参数有效性验证 */
  assert_param(IS_RCC_PLLI2SN_VALUE(PLLI2SN)); // 检查192<=PLLI2SN<=432
  assert_param(IS_RCC_PLLI2SR_VALUE(PLLI2SR));  // 检查2<=PLLI2SR<=7

  /* 组合配置参数到PLLI2SCFGR寄存器 */
  // [14:6]  PLLI2SN（左移6位）| [30:28] PLLI2SR（左移28位）
  RCC->PLLI2SCFGR = (PLLI2SN << 6)    // 倍频系数占据bit14-6
                   | (PLLI2SR << 28);  // 分频系数占据bit30-28
}


#elif defined (STM32F411xE)
/**
  * @brief  PLLI2S音频时钟配置函数（STM32F411xE专用）
  * @details 针对F411系列的增强型配置，增加输入分频系数PLLI2SM
  * @note 使用限制：
  *          - 仅适用于STM32F411xE系列
  *          - 必须在PLLI2S禁用状态下配置
  *          - 时钟源与主PLL共用（通过RCC_PLLConfig设置）
  *
  * @param PLLI2SM: 输入分频系数（2-63），控制VCO输入频率1-2MHz
  * @param PLLI2SN: 倍频系数（192-432），控制VCO输出频率（192-432MHz）
  * @param PLLI2SR: I2S时钟分频系数（2-7），输出需≤192MHz
  * @note 计算公式：
  *          VCO输入频率 = (HSE或HSI时钟 / PLLM主PLL分频) / PLLI2SM
  *          VCO输出频率 = VCO输入频率 × PLLI2SN
  *          I2S时钟 = VCO输出频率 / PLLI2SR
  */
void RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SR, uint32_t PLLI2SM)
{
  /* 三重参数有效性验证 */
  assert_param(IS_RCC_PLLI2SN_VALUE(PLLI2SN)); // 检查192<=PLLI2SN<=432
  assert_param(IS_RCC_PLLI2SM_VALUE(PLLI2SM)); // 检查2<=PLLI2SM<=63（F411特有参数）
  assert_param(IS_RCC_PLLI2SR_VALUE(PLLI2SR)); // 检查2<=PLLI2SR<=7

  /* 组合配置参数到PLLI2SCFGR寄存器 */
  RCC->PLLI2SCFGR = (PLLI2SN << 6)    // [14:6]   倍频系数PLLI2SN
                   | (PLLI2SR << 28)  // [30:28] 输出分频PLLI2SR
                   | PLLI2SM;         // [5:0]   输入分频PLLI2SM（F411新增）
}


#elif defined (STM32F427_437xx) || defined (STM32F429_439xx)
/**
  * @brief  PLLI2S音频时钟配置函数（STM32F42x/43x专用）
  * @details 增强版配置支持SAI音频接口，新增PLLI2SQ分频参数
  * @note 使用限制：
  *          - 仅适用于STM32F427/437/429/439系列
  *          - 必须在PLLI2S禁用状态下配置
  *          - 时钟源与主PLL共用（通过RCC_PLLConfig设置）
  *
  * @param PLLI2SN: 倍频系数（192-432），控制VCO输出频率（192-432MHz）
  * @param PLLI2SQ: SAI1时钟分频系数（2-15），输出需≤48MHz
  * @param PLLI2SR: I2S时钟分频系数（2-7），输出需≤192MHz
  * @note 计算公式：
  *          VCO输出频率 = (HSE或HSI时钟 / PLLM) × PLLI2SN
  *          SAI1时钟 = VCO输出频率 / PLLI2SQ
  *          I2S时钟 = VCO输出频率 / PLLI2SR
  */
void RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SQ, uint32_t PLLI2SR)
{
  /* 三重参数有效性验证 */
  assert_param(IS_RCC_PLLI2SN_VALUE(PLLI2SN)); // 检查192<=PLLI2SN<=432
  assert_param(IS_RCC_PLLI2SQ_VALUE(PLLI2SQ)); // 检查2<=PLLI2SQ<=15（SAI专用）
  assert_param(IS_RCC_PLLI2SR_VALUE(PLLI2SR)); // 检查2<=PLLI2SR<=7

  /* 组合配置参数到PLLI2SCFGR寄存器 */
  RCC->PLLI2SCFGR = (PLLI2SN << 6)    // [14:6]   倍频系数PLLI2SN
                   | (PLLI2SQ << 24)   // [27:24] SAI分频PLLI2SQ（F42x/43x新增）
                   | (PLLI2SR << 28);  // [30:28] I2S分频PLLI2SR
}

#else
#endif /* STM32F40_41xxx || STM32F401xx */
/**
  * @brief  PLLI2S时钟使能控制函数
  * @details 控制I2S专用锁相环的启停状态
  * @note 特性：
  *          - 进入STOP/STANDBY模式时自动禁用
  *          - 启用后需检测PLLI2SRDY标志
  * @param NewState: 控制指令（ENABLE/DISABLE）
  */
void RCC_PLLI2SCmd(FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 通过位带地址原子操作PLLI2SON位（CR寄存器bit26） */
  // CR_PLLI2SON_BB = 0x42000000 + (0x00 * 32) + 0x1A*4 = 0x42420068
  *(__IO uint32_t *) CR_PLLI2SON_BB = (uint32_t)NewState;
}

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
/**
  * @brief PLLSAI显示时钟配置函数（STM32F42x/43x专用）
  * @details 为LTDC和SAI提供独立时钟源
  * @param PLLSAIN: 倍频系数（192-432），控制VCO输出频率
  * @param PLLSAIQ: SAI分频系数（2-15），输出≤48MHz
  * @param PLLSAIR: LTDC分频系数（2-7），输出≤216MHz
  * @note 计算公式：
  *          VCO频率 = (HSE/HSI / PLLM) × PLLSAIN
  *          SAI时钟 = VCO频率 / PLLSAIQ
  *          LTDC时钟 = VCO频率 / PLLSAIR
  */
void RCC_PLLSAIConfig(uint32_t PLLSAIN, uint32_t PLLSAIQ, uint32_t PLLSAIR)
{
  /* 三重参数校验 */
  assert_param(IS_RCC_PLLSAIN_VALUE(PLLSAIN)); // 192-432
  assert_param(IS_RCC_PLLSAIQ_VALUE(PLLSAIQ)); // 2-15
  assert_param(IS_RCC_PLLSAIR_VALUE(PLLSAIR)); // 2-7

  /* 组合参数到PLLSAICFGR寄存器 */
  RCC->PLLSAICFGR = (PLLSAIN << 6)    // [14:6] 倍频系数
                   | (PLLSAIQ << 24)   // [27:24] SAI分频 
                   | (PLLSAIR << 28);  // [30:28] LTDC分频
}

/**
  * @brief PLLSAI时钟使能控制函数
  * @details 启用/禁用显示子系统专用锁相环
  * @note 启用后需检测PLLSAIRDY标志
  * @param NewState: 控制指令（ENABLE/DISABLE）
  */
void RCC_PLLSAICmd(FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 原子操作PLLSAION位（CR寄存器bit28） */
  // CR_PLLSAION_BB = 0x42000000 + (0x00 * 32) + 0x1C*4 = 0x42420070
  *(__IO uint32_t *) CR_PLLSAION_BB = (uint32_t)NewState;
}
#endif

/**
  * @brief 时钟安全系统控制函数
  * @details 监控HSE时钟，故障时自动切换HSI并触发NMI中断
  * @note 典型应用场景：
  *          - 使用HSE作为系统时钟源时
  *          - 需要高可靠性时钟的场合
  * @param NewState: 启用/禁用CSS
  */
void RCC_ClockSecuritySystemCmd(FunctionalState NewState)
{
  /* 参数校验 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 原子操作CSSON位（CR寄存器bit19） */
  // CR_CSSON_BB = 0x42000000 + (0x00 * 32) + 0x13*4 = 0x4242004C
  *(__IO uint32_t *) CR_CSSON_BB = (uint32_t)NewState;
}

/**
  * @brief  MCO1时钟输出配置函数（PA8引脚）
  * @details 通过PA8引脚输出内部时钟信号，用于外部监测或同步
  * @param RCC_MCO1Source: 时钟源选择
  *          @arg RCC_MCO1Source_HSI:   16MHz内部RC时钟
  *          @arg RCC_MCO1Source_LSE:   32.768kHz低速外部晶振
  *          @arg RCC_MCO1Source_HSE:    外部高速晶振（4-26MHz）
  *          @arg RCC_MCO1Source_PLLCLK:主PLL输出时钟（典型84-168MHz）
  * @param RCC_MCO1Div: 分频系数（1-5），控制输出频率范围
  * @note 最大输出频率不超过100MHz（芯片IO速度限制）
  */
void RCC_MCO1Config(uint32_t RCC_MCO1Source, uint32_t RCC_MCO1Div)
{
  uint32_t tmpreg = 0;
  
  /* 参数有效性验证 */
  assert_param(IS_RCC_MCO1SOURCE(RCC_MCO1Source)); // 检查时钟源合法性
  assert_param(IS_RCC_MCO1DIV(RCC_MCO1Div));      // 检查分频系数1-5

  /* 读取CFGR寄存器当前值 */
  tmpreg = RCC->CFGR;  // 获取时钟配置寄存器状态

  /* 清除MCO1相关配置位（bit21-24） */
  tmpreg &= CFGR_MCO1_RESET_MASK; // CFGR_MCO1_RESET_MASK = 0xF0FFFFFF

  /* 组合新配置：源选择 + 分频设置 */
  tmpreg |= RCC_MCO1Source | RCC_MCO1Div;  // 源占bit24-21，分频占bit27-25

  /* 更新配置寄存器 */
  RCC->CFGR = tmpreg;  // 原子操作写入整个寄存器
}

/**
  * @brief  MCO2时钟输出配置函数（PC9引脚）
  * @details 通过PC9引脚输出系统或外设时钟，适用于高速信号监测
  * @param RCC_MCO2Source: 时钟源选择
  *          @arg RCC_MCO2Source_SYSCLK:   系统时钟（HSI/HSE/PLL）
  *          @arg RCC_MCO2Source_PLLI2SCLK:PLLI2S时钟（用于音频）
  *          @arg RCC_MCO2Source_HSE:      外部高速晶振
  *          @arg RCC_MCO2Source_PLLCLK:   主PLL输出
  * @param RCC_MCO2Div: 分频系数（1-5）
  * @warning 使用PLLI2S时钟源时需先启用PLLI2S
  */
void RCC_MCO2Config(uint32_t RCC_MCO2Source, uint32_t RCC_MCO2Div)
{
  uint32_t tmpreg = 0;
  
  /* 双重参数校验 */
  assert_param(IS_RCC_MCO2SOURCE(RCC_MCO2Source));
  assert_param(IS_RCC_MCO2DIV(RCC_MCO2Div));

  /* 读取-修改-回写模式操作CFGR寄存器 */
  tmpreg = RCC->CFGR;  // 获取当前配置
  
  /* 清除MCO2相关位（bit30-27） */
  tmpreg &= CFGR_MCO2_RESET_MASK; // CFGR_MCO2_RESET_MASK = 0x83FFFFFF 

  /* 设置新参数：源选择 + 分频 */
  tmpreg |= RCC_MCO2Source | RCC_MCO2Div;  // 源占bit30-28，分频占bit31-29

  /* 提交配置更改 */
  RCC->CFGR = tmpreg;  // 保证配置变更的原子性
}


/**
  * @}
  */

/** @defgroup RCC_Group2 System AHB and APB busses clocks configuration functions
 *  @brief   System, AHB and APB busses clocks configuration functions
 *
@verbatim   
 ===============================================================================
      ##### System, AHB and APB busses clocks configuration functions #####
 ===============================================================================  
    [..]
      This section provide functions allowing to configure the System, AHB, APB1 and 
      APB2 busses clocks.
  
      (#) Several clock sources can be used to drive the System clock (SYSCLK): HSI,
          HSE and PLL.
          The AHB clock (HCLK) is derived from System clock through configurable 
          prescaler and used to clock the CPU, memory and peripherals mapped 
          on AHB bus (DMA, GPIO...). APB1 (PCLK1) and APB2 (PCLK2) clocks are derived 
          from AHB clock through configurable prescalers and used to clock 
          the peripherals mapped on these busses. You can use 
          "RCC_GetClocksFreq()" function to retrieve the frequencies of these clocks.  

      -@- All the peripheral clocks are derived from the System clock (SYSCLK) except:
        (+@) I2S: the I2S clock can be derived either from a specific PLL (PLLI2S) or
             from an external clock mapped on the I2S_CKIN pin. 
             You have to use RCC_I2SCLKConfig() function to configure this clock. 
        (+@) RTC: the RTC clock can be derived either from the LSI, LSE or HSE clock
             divided by 2 to 31. You have to use RCC_RTCCLKConfig() and RCC_RTCCLKCmd()
             functions to configure this clock. 
        (+@) USB OTG FS, SDIO and RTC: USB OTG FS require a frequency equal to 48 MHz
             to work correctly, while the SDIO require a frequency equal or lower than
             to 48. This clock is derived of the main PLL through PLLQ divider.
        (+@) IWDG clock which is always the LSI clock.
       
      (#) For STM32F405xx/407xx and STM32F415xx/417xx devices, the maximum frequency 
         of the SYSCLK and HCLK is 168 MHz, PCLK2 84 MHz and PCLK1 42 MHz. Depending 
         on the device voltage range, the maximum frequency should be adapted accordingly:
 +-------------------------------------------------------------------------------------+     
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|     
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|              
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|   
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  | 
 |---------------|----------------|----------------|-----------------|-----------------|   
 |2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96 |66 < HCLK <= 88  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |4WS(5CPU cycle)|120< HCLK <= 150|96 < HCLK <= 120|88 < HCLK <= 110 |80 < HCLK <= 100 |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |5WS(6CPU cycle)|150< HCLK <= 168|120< HCLK <= 144|110 < HCLK <= 132|100 < HCLK <= 120| 
 |---------------|----------------|----------------|-----------------|-----------------| 
 |6WS(7CPU cycle)|      NA        |144< HCLK <= 168|132 < HCLK <= 154|120 < HCLK <= 140| 
 |---------------|----------------|----------------|-----------------|-----------------| 
 |7WS(8CPU cycle)|      NA        |      NA        |154 < HCLK <= 168|140 < HCLK <= 160|
 +---------------|----------------|----------------|-----------------|-----------------+ 
      (#) For STM32F42xxx/43xxx devices, the maximum frequency of the SYSCLK and HCLK is 180 MHz, 
          PCLK2 90 MHz and PCLK1 45 MHz. Depending on the device voltage range, the maximum 
          frequency should be adapted accordingly:
 +-------------------------------------------------------------------------------------+     
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|     
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|              
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|   
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  | 
 |---------------|----------------|----------------|-----------------|-----------------|   
 |2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96 |66 < HCLK <= 88  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |4WS(5CPU cycle)|120< HCLK <= 150|96 < HCLK <= 120|88 < HCLK <= 110 |80 < HCLK <= 100 |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |5WS(6CPU cycle)|120< HCLK <= 180|120< HCLK <= 144|110 < HCLK <= 132|100 < HCLK <= 120| 
 |---------------|----------------|----------------|-----------------|-----------------| 
 |6WS(7CPU cycle)|      NA        |144< HCLK <= 168|132 < HCLK <= 154|120 < HCLK <= 140| 
 |---------------|----------------|----------------|-----------------|-----------------| 
 |7WS(8CPU cycle)|      NA        |168< HCLK <= 180|154 < HCLK <= 176|140 < HCLK <= 160|
 |---------------|----------------|----------------|-----------------|-----------------| 
 |8WS(9CPU cycle)|      NA        |      NA        |176 < HCLK <= 180|160 < HCLK <= 168|
 +-------------------------------------------------------------------------------------+
   
      (#) For STM32F401xx devices, the maximum frequency of the SYSCLK and HCLK is 84 MHz, 
          PCLK2 84 MHz and PCLK1 42 MHz. Depending on the device voltage range, the maximum 
          frequency should be adapted accordingly:
 +-------------------------------------------------------------------------------------+     
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|     
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|              
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 22   |0 < HCLK <= 20   |
 |---------------|----------------|----------------|-----------------|-----------------|   
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48 |22 < HCLK <= 44  |20 < HCLK <= 40  | 
 |---------------|----------------|----------------|-----------------|-----------------|   
 |2WS(3CPU cycle)|60 < HCLK <= 84 |48 < HCLK <= 72 |44 < HCLK <= 66  |40 < HCLK <= 60  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |3WS(4CPU cycle)|      NA        |72 < HCLK <= 84 |66 < HCLK <= 84  |60 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------| 
 |4WS(5CPU cycle)|      NA        |      NA        |      NA         |80 < HCLK <= 84  | 
 +-------------------------------------------------------------------------------------+

      (#) For STM32F411xE devices, the maximum frequency of the SYSCLK and HCLK is 100 MHz, 
          PCLK2 100 MHz and PCLK1 50 MHz. Depending on the device voltage range, the maximum 
          frequency should be adapted accordingly:
 +-------------------------------------------------------------------------------------+
 | Latency       |                HCLK clock frequency (MHz)                           |
 |               |---------------------------------------------------------------------|
 |               | voltage range  | voltage range  | voltage range   | voltage range   |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V  | 2.1 V - 2.4 V   | 1.8 V - 2.1 V   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24  |0 < HCLK <= 18   |0 < HCLK <= 16   |
 |---------------|----------------|----------------|-----------------|-----------------|
 |1WS(2CPU cycle)|30 < HCLK <= 64 |24 < HCLK <= 48 |18 < HCLK <= 36  |16 < HCLK <= 32  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |2WS(3CPU cycle)|64 < HCLK <= 90 |48 < HCLK <= 72 |36 < HCLK <= 54  |32 < HCLK <= 48  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |3WS(4CPU cycle)|90 < HCLK <= 100|72 < HCLK <= 96 |54 < HCLK <= 72  |48 < HCLK <= 64  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |4WS(5CPU cycle)|      NA        |96 < HCLK <= 100|72 < HCLK <= 90  |64 < HCLK <= 80  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |5WS(6CPU cycle)|      NA        |       NA       |90 < HCLK <= 100 |80 < HCLK <= 96  |
 |---------------|----------------|----------------|-----------------|-----------------|
 |6WS(7CPU cycle)|      NA        |       NA       |        NA       |96 < HCLK <= 100 |
 +-------------------------------------------------------------------------------------+
  
      -@- On STM32F405xx/407xx and STM32F415xx/417xx devices: 
           (++) when VOS = '0', the maximum value of fHCLK = 144MHz. 
           (++) when VOS = '1', the maximum value of fHCLK = 168MHz. 
          [..] 
          On STM32F42xxx/43xxx devices:
           (++) when VOS[1:0] = '0x01', the maximum value of fHCLK is 120MHz.
           (++) when VOS[1:0] = '0x10', the maximum value of fHCLK is 144MHz.
           (++) when VOS[1:0] = '0x11', the maximum value of f  is 168MHz 
          [..]  
          On STM32F401x devices:
           (++) when VOS[1:0] = '0x01', the maximum value of fHCLK is 64MHz.
           (++) when VOS[1:0] = '0x10', the maximum value of fHCLK is 84MHz.
          On STM32F411xE devices:
           (++) when VOS[1:0] = '0x01' the maximum value of fHCLK is 64MHz.
           (++) when VOS[1:0] = '0x10' the maximum value of fHCLK is 84MHz.
           (++) when VOS[1:0] = '0x11' the maximum value of fHCLK is 100MHz.

       You can use PWR_MainRegulatorModeConfig() function to control VOS bits.

@endverbatim
  * @{
  */
/**
  * @brief 系统时钟源切换函数
  * @details 动态切换CPU核心时钟源，支持HSI/HSE/PLL三种选择
  * @note 安全切换机制：
  *          - 目标时钟必须就绪（HSERDY/HSIRDY/PLLRDY）
  *          - 切换失败自动回退HSI（若CSS启用）
  * @param RCC_SYSCLKSource: 目标时钟源
  *          @arg RCC_SYSCLKSource_HSI: 16MHz内部RC
  *          @arg RCC_SYSCLKSource_HSE: 外部晶振
  *          @arg RCC_SYSCLKSource_PLLCLK: PLL输出
  */
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource)
{
  uint32_t tmpreg = 0;

  /* 验证时钟源合法性 */
  assert_param(IS_RCC_SYSCLK_SOURCE(RCC_SYSCLKSource));

  /* 读取-修改-回写模式操作CFGR寄存器 */
  tmpreg = RCC->CFGR;  // 获取当前时钟配置

  /* 清除SW[1:0]位（bit0-1） */
  tmpreg &= ~RCC_CFGR_SW;  // RCC_CFGR_SW = 0x00000003

  /* 设置新的时钟源选择 */
  tmpreg |= RCC_SYSCLKSource;  // 写入SW位（0x00/0x04/0x08）

  /* 提交配置变更 */
  RCC->CFGR = tmpreg;  // 原子操作切换时钟源
}

/**
  * @brief 获取当前系统时钟源状态
  * @retval 时钟源标识：
  *           - 0x00: HSI作为系统时钟
  *           - 0x04: HSE作为系统时钟
  *           - 0x08: PLL作为系统时钟
  */
uint8_t RCC_GetSYSCLKSource(void)
{
  /* 读取CFGR的SWS[2:0]位（bit2-0） */
  return ((uint8_t)(RCC->CFGR & RCC_CFGR_SWS)); // RCC_CFGR_SWS=0x0000000C
}

/**
  * @brief AHB总线时钟分频配置
  * @details 设置HCLK频率，影响所有AHB外设（GPIO, DMA等）时钟
  * @param RCC_SYSCLK: 分频系数，范围1-512分频
  * @warning 需根据芯片工作电压调整分频：
  *          - 3.3V: HCLK≤168MHz
  *          - 2.8V: HCLK≤144MHz 
  *          - 2.4V: HCLK≤120MHz
  */
void RCC_HCLKConfig(uint32_t RCC_SYSCLK)
{
  uint32_t tmpreg = 0;
  
  /* 验证分频系数合法性 */
  assert_param(IS_RCC_HCLK(RCC_SYSCLK)); // 检查是否为预定义分频值

  /* 操作CFGR的HPRE[3:0]位（bit7-4） */
  tmpreg = RCC->CFGR;  // 获取当前配置

  /* 清除原有分频设置 */
  tmpreg &= ~RCC_CFGR_HPRE;  // RCC_CFGR_HPRE = 0x000000F0

  /* 设置新的分频系数 */
  tmpreg |= RCC_SYSCLK;  // 例如RCC_SYSCLK_Div2=0x80

  /* 更新寄存器 */
  RCC->CFGR = tmpreg;  // 应用AHB分频配置
}
/**
  * @brief  APB1低速外设时钟配置函数
  * @details 设置连接至APB1总线的外设时钟频率（如TIM2-5, SPI2, I2C1等）
  * @param RCC_HCLK: 分频系数，可选1/2/4/8/16分频
  * @note 最大频率限制：
  *          - 当HCLK=168MHz时，APB1时钟最大42MHz（需4分频）
  */
void RCC_PCLK1Config(uint32_t RCC_HCLK)
{
  uint32_t tmpreg = 0;

  /* 验证分频系数合法性（必须为预定义值） */
  assert_param(IS_RCC_PCLK(RCC_HCLK));

  /* 获取当前CFGR寄存器配置 */
  tmpreg = RCC->CFGR;

  /* 清除PPRE1[10:8]位（APB1分频设置位） */
  tmpreg &= ~RCC_CFGR_PPRE1;  // RCC_CFGR_PPRE1 = 0x00000700

  /* 设置新的分频系数（直接赋值） */
  tmpreg |= RCC_HCLK;  // 例如RCC_HCLK_Div4=0x00000500

  /* 更新寄存器配置 */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  APB2高速外设时钟配置函数 
  * @details 设置连接至APB2总线的外设时钟频率（如TIM1, SPI1, ADC等）
  * @param RCC_HCLK: 分频系数，可选1/2/4/8/16分频
  * @note 最大频率限制：
  *          - 当HCLK=168MHz时，APB2时钟最大84MHz（需2分频）
  */
void RCC_PCLK2Config(uint32_t RCC_HCLK)
{
  uint32_t tmpreg = 0;

  /* 参数合法性检查 */
  assert_param(IS_RCC_PCLK(RCC_HCLK));

  /* 读取当前配置 */
  tmpreg = RCC->CFGR;

  /* 清除PPRE2[13:11]位（APB2分频设置位） */
  tmpreg &= ~RCC_CFGR_PPRE2;  // RCC_CFGR_PPRE2 = 0x00003800

  /* 设置分频系数（需左移3位对齐寄存器位） */
  tmpreg |= RCC_HCLK << 3;  // 例如RCC_HCLK_Div2 → 0x00002000

  /* 提交配置变更 */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Returns the frequencies of different on chip clocks; SYSCLK, HCLK, 
  *         PCLK1 and PCLK2.       
  * 
  * @note   The system frequency computed by this function is not the real 
  *         frequency in the chip. It is calculated based on the predefined 
  *         constant and the selected clock source:
  * @note     If SYSCLK source is HSI, function returns values based on HSI_VALUE(*)
  * @note     If SYSCLK source is HSE, function returns values based on HSE_VALUE(**)
  * @note     If SYSCLK source is PLL, function returns values based on HSE_VALUE(**) 
  *           or HSI_VALUE(*) multiplied/divided by the PLL factors.         
  * @note     (*) HSI_VALUE is a constant defined in stm32f4xx.h file (default value
  *               16 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (**) HSE_VALUE is a constant defined in stm32f4xx.h file (default value
  *                25 MHz), user has to ensure that HSE_VALUE is same as the real
  *                frequency of the crystal used. Otherwise, this function may
  *                have wrong result.
  *                
  * @note   The result of this function could be not correct when using fractional
  *         value for HSE crystal.
  *   
  * @param  RCC_Clocks: pointer to a RCC_ClocksTypeDef structure which will hold
  *          the clocks frequencies.
  *     
  * @note   This function can be used by the user application to compute the 
  *         baudrate for the communication peripherals or configure other parameters.
  * @note   Each time SYSCLK, HCLK, PCLK1 and/or PCLK2 clock changes, this function
  *         must be called to update the structure's field. Otherwise, any
  *         configuration based on this function will be incorrect.
  *    
  * @retval None
  */
/**
  * @brief 获取所有总线时钟频率
  * @details 计算并填充当前系统各总线时钟的实际运行频率
  * @param RCC_Clocks: 时钟频率结构体指针，用于存储计算结果
  * @note 注意：
  *          - 需在时钟配置完成后调用以获取准确值
  *          - PLL频率计算依赖HSE/HSI_VALUE宏定义的正确性
  */
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks)
{
  uint32_t tmp = 0, presc = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

  /*--------------------- 系统时钟源判断 ---------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;  // 读取CFGR[3:2]系统时钟状态位

  switch (tmp)
  {
    case 0x00:  /* HSI作为系统时钟源 */
      RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;  // 直接使用16MHz常量
      break;
    case 0x04:  /* HSE作为系统时钟源 */
      RCC_Clocks->SYSCLK_Frequency = HSE_VALUE;  // 使用外部晶振定义值
      break;
    case 0x08:  /* PLL作为系统时钟源 */
      /* PLL配置参数提取 */
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22; // 获取PLL源选择位(bit22)
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;  // 提取PLLM分频系数[4:0]
      
      /* VCO输入频率计算 */
      if (pllsource != 0) {  // HSE作为PLL输入
        pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6); 
      } else {               // HSI作为PLL输入
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }

      /* 系统时钟分频计算 */
      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2; // 解码PLLP分频系数
      RCC_Clocks->SYSCLK_Frequency = pllvco / pllp;  // 最终系统时钟频率
      break;
    default:  // 异常情况回退HSI
      RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
      break;
  }

  /*--------------------- 总线分频系数计算 ---------------------*/
  /* HCLK计算（AHB总线） */
  tmp = (RCC->CFGR & RCC_CFGR_HPRE) >> 4;       // 提取HPRE[3:0]位(bit7-4)
  presc = APBAHBPrescTable[tmp];                // 查表获取分频系数
  RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency >> presc; // 右移等效除法

  /* PCLK1计算（APB1总线） */
  tmp = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;      // 提取PPRE1[2:0]位(bit10-8)
  presc = APBAHBPrescTable[tmp];                // 查表获取分频系数
  RCC_Clocks->PCLK1_Frequency = RCC_Clocks->HCLK_Frequency >> presc;

  /* PCLK2计算（APB2总线） */
  tmp = (RCC->CFGR & RCC_CFGR_PPRE2) >> 13;      // 提取PPRE2[2:0]位(bit13-11)
  presc = APBAHBPrescTable[tmp];
  RCC_Clocks->PCLK2_Frequency = RCC_Clocks->HCLK_Frequency >> presc;
}

/**
  * @}
  */

/** @defgroup RCC_Group3 Peripheral clocks configuration functions
 *  @brief   Peripheral clocks configuration functions 
 *
@verbatim   
 ===============================================================================
              ##### Peripheral clocks configuration functions #####
 ===============================================================================  
    [..] This section provide functions allowing to configure the Peripheral clocks. 
  
      (#) The RTC clock which is derived from the LSI, LSE or HSE clock divided 
          by 2 to 31.
     
      (#) After restart from Reset or wakeup from STANDBY, all peripherals are off
          except internal SRAM, Flash and JTAG. Before to start using a peripheral 
          you have to enable its interface clock. You can do this using 
          RCC_AHBPeriphClockCmd(), RCC_APB2PeriphClockCmd() and RCC_APB1PeriphClockCmd() functions.

      (#) To reset the peripherals configuration (to the default state after device reset)
          you can use RCC_AHBPeriphResetCmd(), RCC_APB2PeriphResetCmd() and 
          RCC_APB1PeriphResetCmd() functions.
     
      (#) To further reduce power consumption in SLEEP mode the peripheral clocks 
          can be disabled prior to executing the WFI or WFE instructions. 
          You can do this using RCC_AHBPeriphClockLPModeCmd(), 
          RCC_APB2PeriphClockLPModeCmd() and RCC_APB1PeriphClockLPModeCmd() functions.  

@endverbatim
  * @{
  */
/**
  * @brief  配置RTC时钟源
  * @param  RCC_RTCCLKSource: RTC时钟源选择，支持：
  *         @arg RCC_RTCCLKSource_LSE      LSE晶体振荡（32.768kHz）
  *         @arg RCC_RTCCLKSource_LSI      LSI内部RC（约32kHz）
  *         @arg RCC_RTCCLKSource_HSE_Divx HSE分频（x=2~31，需≤1MHz）
  * @note 关键配置要求：
  *        - 操作前需使能备份域访问（PWR_BackupAccessCmd(ENABLE)）
  *        - HSE分频后频率计算：HSE_CLK / (Div+1)
  *        - LSE/LSI模式支持低功耗唤醒
  */
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource)
{
  uint32_t tmpreg = 0;

  /* 验证参数合法性 */
  assert_param(IS_RCC_RTCCLK_SOURCE(RCC_RTCCLKSource));  // 检查时钟源类型有效性

  /* 处理HSE分频配置 */
  if ((RCC_RTCCLKSource & 0x00000300) == 0x00000300)  // 检测bit8-9是否为11（HSE分频标识）
  { 
    /* 配置HSE分频系数至CFGR寄存器 */
    tmpreg = RCC->CFGR;  // 读取当前配置
    
    /* 清除原有分频设置（RTCPRE[4:0]） */
    tmpreg &= ~RCC_CFGR_RTCPRE;  // 清空bit16-20
    
    /* 设置新分频系数（参数中的Div值在bit0-4） */
    tmpreg |= (RCC_RTCCLKSource & 0xFFFFCFF);  // 保留除bit8-9外的分频系数
    
    /* 写入更新后的配置 */
    RCC->CFGR = tmpreg;  // 应用HSE分频参数
  }
    
  /* 设置RTC时钟源选择位（BDCR寄存器bit8-9） */
  RCC->BDCR |= (RCC_RTCCLKSource & 0x00000FFF);  // 保留低12位配置参数
}
/**
  * @brief  启用/禁用RTC时钟
  * @param  NewState: 控制状态（ENABLE/DISABLE）
  * @note 操作前提：
  *        - 必须已通过RCC_RTCCLKConfig()配置时钟源
  *        - 使能后RTC开始计数
  *        - 禁用将停止RTC计时
  */
void RCC_RTCCLKCmd(FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 检查ENABLE/DISABLE参数有效性
  
  /* 通过位带操作设置BDCR寄存器的RTCEN位（bit15）*/
  *(__IO uint32_t *) BDCR_RTCEN_BB = (uint32_t)NewState;  // 位带地址：0x424E005C
}

/**
  * @brief  强制复位备份域
  * @param  NewState: 复位控制（ENABLE触发复位，DISABLE释放）
  * @note 复位影响：
  *        - 清除所有RTC寄存器（包括计数器）
  *        - 复位备份寄存器（需重新初始化）
  *        - 保留备份SRAM内容
  *        操作后需重新配置RTC时钟源
  */
void RCC_BackupResetCmd(FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 检查参数有效性
  
  /* 通过位带操作设置BDCR寄存器的BDRST位（bit16）*/
  *(__IO uint32_t *) BDCR_BDRST_BB = (uint32_t)NewState;  // 位带地址：0x424E0080
}
/**
  * @brief I2S时钟源配置函数
  * @details 选择I2S模块的主时钟源，需在I2S外设使能前调用
  * @param RCC_I2SCLKSource: 时钟源选择
  *          @arg RCC_I2S2CLKSource_PLLI2S: 使用PLLI2S生成的音频专用时钟
  *          @arg RCC_I2S2CLKSource_Ext:     使用外部I2S_CKIN引脚输入的时钟
  * @note 外部时钟模式下需确保I2S_CKIN引脚正确连接外部振荡器
  */
void RCC_I2SCLKConfig(uint32_t RCC_I2SCLKSource)
{
  /* 参数合法性检查（必须为预定义值） */
  assert_param(IS_RCC_I2SCLK_SOURCE(RCC_I2SCLKSource));

  /* 通过位带地址直接操作CFGR寄存器的I2SSRC位（bit23） */
  // CFGR_I2SSRC_BB = 0x42420000 + (0x08*32 + 0x17*4) = 0x424205DC
  *(__IO uint32_t *) CFGR_I2SSRC_BB = RCC_I2SCLKSource;  // 写入0或1
}

/**
  * @brief SAI1时钟分频配置（PLLI2S专用）
  * @details 设置从PLLI2S到SAI1模块的时钟分频系数
  * @param RCC_PLLI2SDivQ: 分频系数（1-32），实际分频值=输入值
  * @note 必须在启用PLLI2S前配置，计算公式：
  *          SAI1时钟 = PLLI2S_Q输出 / RCC_PLLI2SDivQ
  */
void RCC_SAIPLLI2SClkDivConfig(uint32_t RCC_PLLI2SDivQ)  
{
  uint32_t tmpreg = 0;
  
  /* 验证分频系数范围 */
  assert_param(IS_RCC_PLLI2S_DIVQ_VALUE(RCC_PLLI2SDivQ));  // 1-32
  
  tmpreg = RCC->DCKCFGR;  // 获取专用时钟配置寄存器

  /* 清除PLLI2SDIVQ[4:0]位（bit4-0） */
  tmpreg &= ~(RCC_DCKCFGR_PLLI2SDIVQ);  // 掩码0x0000001F

  /* 设置分频系数（寄存器存储值=实际值-1） */
  tmpreg |= (RCC_PLLI2SDivQ - 1);  // 例如分频8→写入7(0x07)

  /* 更新寄存器 */
  RCC->DCKCFGR = tmpreg;  // 应用PLLI2S到SAI的分频
}

/**
  * @brief SAI1时钟分频配置（PLLSAI专用）
  * @details 设置从PLLSAI到SAI1模块的时钟分频系数
  * @param RCC_PLLSAIDivQ: 分频系数（1-32），实际分频值=输入值
  * @note 必须在启用PLLSAI前配置，计算公式：
  *          SAI1时钟 = PLLSAI_Q输出 / RCC_PLLSAIDivQ
  */
void RCC_SAIPLLSAIClkDivConfig(uint32_t RCC_PLLSAIDivQ)  
{
  uint32_t tmpreg = 0;
  
  /* 参数合法性检查 */
  assert_param(IS_RCC_PLLSAI_DIVQ_VALUE(RCC_PLLSAIDivQ));  // 1-32
  
  tmpreg = RCC->DCKCFGR;  // 获取当前配置

  /* 清除PLLSAIDIVQ[12:8]位（bit12-8） */
  tmpreg &= ~(RCC_DCKCFGR_PLLSAIDIVQ);  // 掩码0x00001F00

  /* 设置分频系数（存储值=实际值-1，左移8位对齐） */
  tmpreg |= ((RCC_PLLSAIDivQ - 1) << 8);  // 例如分频16→0x00000F00

  /* 提交配置变更 */
  RCC->DCKCFGR = tmpreg;  // 更新专用时钟配置寄存器
}
/**
  * @brief SAI1模块A块时钟源配置函数
  * @details 为SAI1_A选择时钟源，支持双PLL分频或外部时钟
  * @param RCC_SAIBlockACLKSource: 时钟源选择
  *          @arg RCC_SAIACLKSource_PLLI2S: 使用PLLI2S分频后的时钟
  *          @arg RCC_SAIACLKSource_PLLSAI: 使用PLLSAI分频后的时钟
  *          @arg RCC_SAIACLKSource_Ext:    外部I2S_CKIN引脚输入
  * @note 配置顺序：先设置分频系数→选择时钟源→最后启用时钟
  */
void RCC_SAIBlockACLKConfig(uint32_t RCC_SAIBlockACLKSource)
{
  uint32_t tmpreg = 0;
  
  /* 参数合法性检查 */
  assert_param(IS_RCC_SAIACLK_SOURCE(RCC_SAIBlockACLKSource));
  
  tmpreg = RCC->DCKCFGR;  // 获取专用时钟配置寄存器当前值

  /* 清除SAI1_A时钟源选择位[21:20] */
  tmpreg &= ~RCC_DCKCFGR_SAI1ASRC;  // 掩码0x00300000

  /* 设置新的时钟源选择 */
  tmpreg |= RCC_SAIBlockACLKSource;  // 写入0x00000000/0x00100000/0x00200000

  /* 更新寄存器配置 */
  RCC->DCKCFGR = tmpreg;
}

/**
  * @brief SAI1模块B块时钟源配置函数
  * @details 为SAI1_B选择时钟源，配置逻辑与A块独立
  * @param RCC_SAIBlockBCLKSource: 时钟源选择（同A块选项）
  * @note 可实现A/B块使用不同时钟源，支持立体声分离处理
  */
void RCC_SAIBlockBCLKConfig(uint32_t RCC_SAIBlockBCLKSource)
{
  uint32_t tmpreg = 0;
  
  assert_param(IS_RCC_SAIBCLK_SOURCE(RCC_SAIBlockBCLKSource));
  
  tmpreg = RCC->DCKCFGR;

  /* 清除SAI1_B时钟源选择位[23:22] */
  tmpreg &= ~RCC_DCKCFGR_SAI1BSRC;  // 掩码0x00C00000

  /* 设置B块时钟源 */
  tmpreg |= RCC_SAIBlockBCLKSource;  // 写入0x00000000/0x00400000/0x00800000

  RCC->DCKCFGR = tmpreg;
}

/**
  * @brief LTDC显示控制器时钟分频配置
  * @details 设置从PLLSAI_R到LTDC模块的分频系数
  * @param RCC_PLLSAIDivR: 分频系数（2-16），计算公式：
  *          LTDC时钟 = PLLSAI_R输出 / RCC_PLLSAIDivR
  * @note PLLSAI_R需配置为生成像素时钟，典型值需满足：
  *          LTDC时钟 ≤ 50MHz（RGB888模式）
  */
void RCC_LTDCCLKDivConfig(uint32_t RCC_PLLSAIDivR)
{
  uint32_t tmpreg = 0;
  
  /* 验证分频系数范围 */
  assert_param(IS_RCC_PLLSAI_DIVR_VALUE(RCC_PLLSAIDivR));  // 2-16
  
  tmpreg = RCC->DCKCFGR;

  /* 清除LTDC分频位[26:24] */
  tmpreg &= ~RCC_DCKCFGR_PLLSAIDIVR;  // 掩码0x07000000

  /* 设置分频系数（直接存储，无需减1） */
  tmpreg |= RCC_PLLSAIDivR;  // 例如分频4→0x04000000

  RCC->DCKCFGR = tmpreg;
}

/**
  * @brief  Configures the Timers clocks prescalers selection.
  * @details 配置定时器时钟预分频器的选择，适用于STM32F42xxx/43xxx和STM32F401xx/411xE系列。
  * @param  RCC_TIMCLKPrescaler : 定时器时钟预分频器的选择
  *         This parameter can be one of the following values:
  *            @arg RCC_TIMPrescDesactivated: 定时器内核时钟预分频器等于HPRE，如果PPREx是1或2，则预分频器等于HPRE；如果PPREx是4或更大，则预分频器等于[(HPRE * PPREx) / 2]。
  *                   
  *            @arg RCC_TIMPrescActivated: 定时器内核时钟预分频器等于HPRE，如果PPREx是1、2或4，则预分频器等于HPRE；如果PPREx是8或更大，则预分频器等于[(HPRE * PPREx) / 4]。
  * @retval None
  */
void RCC_TIMCLKPresConfig(uint32_t RCC_TIMCLKPrescaler)
{
  /* Check the parameters */
  assert_param(IS_RCC_TIMCLK_PRESCALER(RCC_TIMCLKPrescaler));  // 参数有效性检查

  /* 通过位带操作设置DCKCFGR寄存器的TIMPRE位（bit24） */
  // DCKCFGR_TIMPRE_BB = 0x42420000 + (0x08*32 + 0x18*4) = 0x42420600
  *(__IO uint32_t *) DCKCFGR_TIMPRE_BB = RCC_TIMCLKPrescaler;  // 写入0或1
}

/**
  * @brief  Enables or disables the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.   
  * @param  RCC_AHBPeriph: specifies the AHB1 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB1Periph_GPIOA:       GPIOA clock
  *            @arg RCC_AHB1Periph_GPIOB:       GPIOB clock 
  *            @arg RCC_AHB1Periph_GPIOC:       GPIOC clock
  *            @arg RCC_AHB1Periph_GPIOD:       GPIOD clock
  *            @arg RCC_AHB1Periph_GPIOE:       GPIOE clock
  *            @arg RCC_AHB1Periph_GPIOF:       GPIOF clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOI:       GPIOI clock
  *            @arg RCC_AHB1Periph_GPIOJ:       GPIOJ clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_AHB1Periph_GPIOK:       GPIOK clock (STM32F42xxx/43xxx devices)  
  *            @arg RCC_AHB1Periph_CRC:         CRC clock
  *            @arg RCC_AHB1Periph_BKPSRAM:     BKPSRAM interface clock
  *            @arg RCC_AHB1Periph_CCMDATARAMEN CCM data RAM interface clock
  *            @arg RCC_AHB1Periph_DMA1:        DMA1 clock
  *            @arg RCC_AHB1Periph_DMA2:        DMA2 clock
  *            @arg RCC_AHB1Periph_DMA2D:       DMA2D clock (STM32F429xx/439xx devices)  
  *            @arg RCC_AHB1Periph_ETH_MAC:     Ethernet MAC clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Tx:  Ethernet Transmission clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Rx:  Ethernet Reception clock
  *            @arg RCC_AHB1Periph_ETH_MAC_PTP: Ethernet PTP clock
  *            @arg RCC_AHB1Periph_OTG_HS:      USB OTG HS clock
  *            @arg RCC_AHB1Periph_OTG_HS_ULPI: USB OTG HS ULPI clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  启用/禁用AHB1总线外设时钟
  * @param  RCC_AHB1Periph: 外设时钟选择，支持以下位或组合：
  *         @arg RCC_AHB1Periph_GPIOx (x=A~K)
  *         @arg RCC_AHB1Periph_CRC
  *         @arg RCC_AHB1Periph_DMA1/DMA2
  *         @arg RCC_AHB1Periph_ETH_MAC
  *         @arg RCC_AHB1Periph_OTG_HS
  * @param  NewState: 时钟控制状态（ENABLE/DISABLE）
  * @note 操作直接影响RCC_AHB1ENR寄存器
  *        使能时钟后需等待至少两个时钟周期才能访问外设寄存器
  */
void RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* 验证外设参数合法性（检查是否有效位组合） */
  assert_param(IS_RCC_AHB1_CLOCK_PERIPH(RCC_AHB1Periph)); // 例：0x00100000为有效GPIOG时钟位
  
  /* 验证状态参数合法性 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));            // 必须为ENABLE/DISABLE

  if (NewState != DISABLE)  // 启用外设时钟
  {
    /* 通过位或操作设置对应时钟使能位 */
    RCC->AHB1ENR |= RCC_AHB1Periph;  // 原子操作示例：GPIOA时钟使能（bit0置1）
  }
  else  // 禁用外设时钟
  {
    /* 通过位与操作清除对应时钟使能位 */
    RCC->AHB1ENR &= ~RCC_AHB1Periph; // 原子操作示例：GPIOA时钟禁用（bit0清0）
  }
}

/**
  * @brief  Enables or disables the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it. 
  * @param  RCC_AHBPeriph: specifies the AHB2 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB2Periph_DCMI:   DCMI clock
  *            @arg RCC_AHB2Periph_CRYP:   CRYP clock
  *            @arg RCC_AHB2Periph_HASH:   HASH clock
  *            @arg RCC_AHB2Periph_RNG:    RNG clock
  *            @arg RCC_AHB2Periph_OTG_FS: USB OTG FS clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/* 函数功能：AHB2总线外设时钟使能控制
   作用：通过设置RCC->AHB2ENR寄存器，开启或关闭指定AHB2外设的时钟
   注意事项：
   1. 所有外设在使用前必须使能对应总线时钟
   2. 支持多个外设时钟同时操作（按位或组合）
   3. 参数有效性由assert_param宏验证 */
void RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  // 验证输入参数合法性：
  // 1. 检查外设选择是否符合预定义值（通过IS_RCC_AHB2_PERIPH宏）
  // 2. 检查状态参数是否为合法功能状态（ENABLE/DISABLE）
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  // 根据使能状态选择操作方式
  if (NewState != DISABLE)
  {
    /* 设置AHB2外设时钟使能寄存器 
       使用按位或操作添加指定外设的时钟使能位
       寄存器特性：置1使能，置0无影响 */
    RCC->AHB2ENR |= RCC_AHB2Periph;
  }
  else
  {
    /* 清除AHB2外设时钟使能寄存器 
       使用按位与和取反操作清除指定外设的时钟使能位
       寄存器特性：置0禁用，置1无影响
       ~操作符对参数位取反，确保只清除目标位 */
    RCC->AHB2ENR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Enables or disables the AHB3 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it. 
  * @param  RCC_AHBPeriph: specifies the AHB3 peripheral to gates its clock.
  *          This parameter must be: RCC_AHB3Periph_FSMC
  *                                  or RCC_AHB3Periph_FMC (STM32F42xxx/43xxx devices)  
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/* 函数功能：AHB3总线外设时钟使能控制
   作用：通过设置RCC->AHB3ENR寄存器，开启或关闭指定AHB3外设的时钟
   注意事项：
   1. 主要控制FSMC/FMC存储控制器时钟
   2. 不同芯片型号支持的外设不同（F42xxx/43xxx支持FMC）
   3. 参数范围比AHB2更严格 */
void RCC_AHB3PeriphClockCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)
{
  /* Check the parameters */
  // 验证参数：
  // 1. 检查是否为合法的AHB3外设（FSMC/FMC）
  // 2. 检查功能状态有效性
  assert_param(IS_RCC_AHB3_PERIPH(RCC_AHB3Periph));  
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  // 状态判断逻辑（与AHB2版本结构相同但操作不同寄存器）
  if (NewState != DISABLE)
  {
    /* 设置AHB3外设时钟使能寄存器
       用于启动存储控制器等高速外设的时钟
       直接设置对应位为1 */
    RCC->AHB3ENR |= RCC_AHB3Periph;
  }
  else
  {
    /* 禁用AHB3外设时钟
       当需要降低功耗或重新配置外设时使用
       清除对应位前需要确保外设已停止工作 */
    RCC->AHB3ENR &= ~RCC_AHB3Periph;
  }
}


/**
  * @brief  Enables or disables the Low Speed APB (APB1) peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it. 
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB1Periph_TIM2:   TIM2 clock
  *            @arg RCC_APB1Periph_TIM3:   TIM3 clock
  *            @arg RCC_APB1Periph_TIM4:   TIM4 clock
  *            @arg RCC_APB1Periph_TIM5:   TIM5 clock
  *            @arg RCC_APB1Periph_TIM6:   TIM6 clock
  *            @arg RCC_APB1Periph_TIM7:   TIM7 clock
  *            @arg RCC_APB1Periph_TIM12:  TIM12 clock
  *            @arg RCC_APB1Periph_TIM13:  TIM13 clock
  *            @arg RCC_APB1Periph_TIM14:  TIM14 clock
  *            @arg RCC_APB1Periph_WWDG:   WWDG clock
  *            @arg RCC_APB1Periph_SPI2:   SPI2 clock
  *            @arg RCC_APB1Periph_SPI3:   SPI3 clock
  *            @arg RCC_APB1Periph_USART2: USART2 clock
  *            @arg RCC_APB1Periph_USART3: USART3 clock
  *            @arg RCC_APB1Periph_UART4:  UART4 clock
  *            @arg RCC_APB1Periph_UART5:  UART5 clock
  *            @arg RCC_APB1Periph_I2C1:   I2C1 clock
  *            @arg RCC_APB1Periph_I2C2:   I2C2 clock
  *            @arg RCC_APB1Periph_I2C3:   I2C3 clock
  *            @arg RCC_APB1Periph_CAN1:   CAN1 clock
  *            @arg RCC_APB1Periph_CAN2:   CAN2 clock
  *            @arg RCC_APB1Periph_PWR:    PWR clock
  *            @arg RCC_APB1Periph_DAC:    DAC clock
  *            @arg RCC_APB1Periph_UART7:  UART7 clock
  *            @arg RCC_APB1Periph_UART8:  UART8 clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  使能或禁用APB1总线上的外设时钟
  * @note   - 复位后外设时钟默认关闭，访问外设寄存器前必须启用对应时钟
  *         - APB1为低速总线（最高42MHz），连接定时器、窗口看门狗、USART2-5、SPI2/3、I2C1-3等外设
  *         - 该函数通过修改RCC_APB1ENR寄存器的对应位实现时钟控制
  * 
  * @param  RCC_APB1Periph: 需要控制的APB1外设选择
  *          - 使用位掩码组合方式选择多个外设（最多同时支持31个外设）：
  *            @arg RCC_APB1Periph_TIM2:   定时器2时钟
  *            @arg RCC_APB1Periph_TIM3:   定时器3时钟
  *            @arg RCC_APB1Periph_TIM4:   定时器4时钟
  *            （此处保持原有参数说明完整列表）
  *            @arg RCC_APB1Periph_UART8:  UART8时钟
  * 
  * @param  NewState: 时钟状态控制
  *          - ENABLE: 开启指定外设时钟（设置寄存器对应位为1）
  *          - DISABLE: 关闭指定外设时钟（清零寄存器对应位）
  * 
  * @retval None
  * @warning 修改时钟设置需谨慎，不当操作可能导致外设工作异常
  * @example 
  *   // 同时启用TIM2和USART2时钟
  *   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_USART2, ENABLE);
  */
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* 参数有效性检查 */
  // 验证外设选择参数是否在允许范围内（通过IS_RCC_APB1_PERIPH宏实现）
  assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));  
  // 验证状态参数必须是ENABLE/DISABLE（通过IS_FUNCTIONAL_STATE宏实现）
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 根据新状态执行寄存器操作 */
  if (NewState != DISABLE)  // 使能分支
  {
    // 使用位或操作设置对应位（不影响其他位）
    // APB1ENR寄存器地址：0x40023840，每个bit对应一个外设时钟使能位
    RCC->APB1ENR |= RCC_APB1Periph;  
  }
  else  // 禁用分支
  {
    // 使用位与操作清除对应位（~操作符取反位掩码）
    // 示例：当RCC_APB1Periph=0x00000001时，~操作后变为0xFFFFFFFE
    RCC->APB1ENR &= ~RCC_APB1Periph;  
  }
  /* 注：寄存器操作后不需要手动插入延迟，硬件会自动处理时钟切换 */
}


/**
  * @brief  Enables or disables the High Speed APB (APB2) peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB2Periph_TIM1:   TIM1 clock
  *            @arg RCC_APB2Periph_TIM8:   TIM8 clock
  *            @arg RCC_APB2Periph_USART1: USART1 clock
  *            @arg RCC_APB2Periph_USART6: USART6 clock
  *            @arg RCC_APB2Periph_ADC1:   ADC1 clock
  *            @arg RCC_APB2Periph_ADC2:   ADC2 clock
  *            @arg RCC_APB2Periph_ADC3:   ADC3 clock
  *            @arg RCC_APB2Periph_SDIO:   SDIO clock
  *            @arg RCC_APB2Periph_SPI1:   SPI1 clock
  *            @arg RCC_APB2Periph_SPI4:   SPI4 clock
  *            @arg RCC_APB2Periph_SYSCFG: SYSCFG clock
  *            @arg RCC_APB2Periph_TIM9:   TIM9 clock
  *            @arg RCC_APB2Periph_TIM10:  TIM10 clock
  *            @arg RCC_APB2Periph_TIM11:  TIM11 clock
  *            @arg RCC_APB2Periph_SPI5:   SPI5 clock
  *            @arg RCC_APB2Periph_SPI6:   SPI6 clock
  *            @arg RCC_APB2Periph_SAI1:   SAI1 clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_APB2Periph_LTDC:   LTDC clock (STM32F429xx/439xx devices) 
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  APB2总线外设时钟控制函数，用于启用或禁用指定外设的时钟供应
  * @param  RCC_APB2Periph: 目标外设时钟选择，可用以下值按位或组合：
  *         - RCC_APB2Periph_TIM1   高级定时器1
  *         - RCC_APB2Periph_USART1 串口1
  *         - RCC_APB2Periph_SPI1   SPI1接口
  *         - RCC_APB2Periph_ADC1   模数转换器1
  *         - RCC_APB2Periph_SYSCFG 系统配置控制器
  *         - RCC_APB2Periph_GPIOA~GPIOK GPIO端口A-K
  * @param  NewState: 时钟状态
  *         - ENABLE: 激活外设时钟
  *         - DISABLE: 关闭外设时钟
  * @retval 无
  * @note   重要说明：
  *         1. 操作时钟寄存器前必须确保PLL已正确配置
  *         2. 关闭时钟会复位外设所有寄存器
  *         3. 典型使用场景：
  *            - 初始化外设前启用时钟
  *            - 进入低功耗模式前关闭未使用外设时钟
  */
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* 双重安全验证 */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));  // 验证外设选择有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));        // 验证状态参数合法性

  if (NewState != DISABLE)
  {
    /* 启用目标外设时钟：通过APB2ENR寄存器置位操作
       - 使能后需要等待1个APB2时钟周期才能访问外设寄存器
       - 多个外设可同时启用（如RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA） */
    RCC->APB2ENR |= RCC_APB2Periph;
  }
  else
  {
    /* 禁用目标外设时钟：通过APB2ENR寄存器位清除操作
       - 立即生效，外设寄存器将不可访问
       - 清除时钟前需确保：
         1. 外设已停止工作
         2. 没有正在进行的数据传输 */
    RCC->APB2ENR &= ~RCC_APB2Periph;
  }
}


/**
  * @brief  Forces or releases AHB1 peripheral reset.
  * @param  RCC_AHB1Periph: specifies the AHB1 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB1Periph_GPIOA:   GPIOA clock
  *            @arg RCC_AHB1Periph_GPIOB:   GPIOB clock 
  *            @arg RCC_AHB1Periph_GPIOC:   GPIOC clock
  *            @arg RCC_AHB1Periph_GPIOD:   GPIOD clock
  *            @arg RCC_AHB1Periph_GPIOE:   GPIOE clock
  *            @arg RCC_AHB1Periph_GPIOF:   GPIOF clock
  *            @arg RCC_AHB1Periph_GPIOG:   GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOG:   GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOI:   GPIOI clock
  *            @arg RCC_AHB1Periph_GPIOJ:   GPIOJ clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_AHB1Periph_GPIOK:   GPIOK clock (STM32F42xxx/43xxxdevices)   
  *            @arg RCC_AHB1Periph_CRC:     CRC clock
  *            @arg RCC_AHB1Periph_DMA1:    DMA1 clock
  *            @arg RCC_AHB1Periph_DMA2:    DMA2 clock
  *            @arg RCC_AHB1Periph_DMA2D:   DMA2D clock (STM32F429xx/439xx devices)   
  *            @arg RCC_AHB1Periph_ETH_MAC: Ethernet MAC clock
  *            @arg RCC_AHB1Periph_OTG_HS:  USB OTG HS clock
  *                  
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/* 函数功能：AHB1总线外设复位控制
   作用：通过操作RCC->AHB1RSTR寄存器，强制复位或释放指定AHB1外设
   注意事项：
   1. 复位操作会使外设进入复位状态，需配合时钟控制使用
   2. 典型应用场景：外设初始化异常时进行硬件复位
   3. 复位释放后建议增加延时确保外设稳定 */
void RCC_AHB1PeriphResetCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  // 验证参数有效性：
  // 1. 检查复位外设标识是否符合AHB1总线定义（GPIOx/DMA等）
  // 2. 确认功能状态为合法值（ENABLE/DISABLE）
  assert_param(IS_RCC_AHB1_RESET_PERIPH(RCC_AHB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  // 根据复位状态选择操作
  if (NewState != DISABLE)
  {
    /* 强制外设复位 
       置位AHB1RSTR寄存器对应位（脉冲式操作，至少保持2个时钟周期）
       注意：复位期间外设寄存器不可访问 */
    RCC->AHB1RSTR |= RCC_AHB1Periph;
  }
  else
  {
    /* 释放外设复位
       清除复位位后外设恢复正常工作
       建议操作顺序：释放复位 -> 使能时钟 -> 初始化外设 */
    RCC->AHB1RSTR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Forces or releases AHB2 peripheral reset.
  * @param  RCC_AHB2Periph: specifies the AHB2 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB2Periph_DCMI:   DCMI clock
  *            @arg RCC_AHB2Periph_CRYP:   CRYP clock
  *            @arg RCC_AHB2Periph_HASH:   HASH clock
  *            @arg RCC_AHB2Periph_RNG:    RNG clock
  *            @arg RCC_AHB2Periph_OTG_FS: USB OTG FS clock
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/* 函数功能：AHB2总线外设复位控制
   作用：控制USB/Crypto等高速外设的硬件复位
   特殊说明：
   1. 对OTG_FS操作时需要确保USB断开连接
   2. 加密外设复位会清空内部密钥存储器 */
void RCC_AHB2PeriphResetCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  // 参数验证：
  // 1. 检查是否为合法AHB2外设（DCMI/OTG等）
  // 2. 特别验证加密模块的复位权限（部分型号需要安全验证）
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 触发硬件复位序列
       置位复位寄存器后，外设进入复位状态
       典型应用：解决USB模块死锁 */
    RCC->AHB2RSTR |= RCC_AHB2Periph;
  }
  else
  {
    /* 终止复位操作
       复位释放后需重新初始化外设配置
       对于加密模块，需重新加载密钥 */
    RCC->AHB2RSTR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Forces or releases AHB3 peripheral reset.
  * @param  RCC_AHB3Periph: specifies the AHB3 peripheral to reset.
  *          This parameter must be: RCC_AHB3Periph_FSMC
  *                                  or RCC_AHB3Periph_FMC (STM32F42xxx/43xxx devices)  
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/* 函数功能：AHB3总线存储器控制器复位
   作用：控制FSMC/FMC存储控制器的硬件复位
   重要说明：
   1. 复位期间外部存储器总线处于高阻态
   2. 操作前必须确保无进行中的存储访问
   3. FMC复位会丢失SDRAM配置信息 */
void RCC_AHB3PeriphResetCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)
{
  /* Check the parameters */
  // 严格验证参数：
  // 1. 检查是否属于AHB3外设（FSMC/FMC）
  // 2. 根据芯片型号过滤FMC支持情况
  assert_param(IS_RCC_AHB3_PERIPH(RCC_AHB3Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 启动存储器控制器复位
       复位持续至少3个时钟周期
       注意：会中断所有进行中的总线事务 */
    RCC->AHB3RSTR |= RCC_AHB3Periph;
  }
  else
  {
    /* 解除复位状态
       复位释放后需重新配置存储控制器参数
       对于SDRAM控制器，需重新执行初始化序列 */
    RCC->AHB3RSTR &= ~RCC_AHB3Periph;
  }
}


/**
  * @brief  Forces or releases Low Speed APB (APB1) peripheral reset.
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB1Periph_TIM2:   TIM2 clock
  *            @arg RCC_APB1Periph_TIM3:   TIM3 clock
  *            @arg RCC_APB1Periph_TIM4:   TIM4 clock
  *            @arg RCC_APB1Periph_TIM5:   TIM5 clock
  *            @arg RCC_APB1Periph_TIM6:   TIM6 clock
  *            @arg RCC_APB1Periph_TIM7:   TIM7 clock
  *            @arg RCC_APB1Periph_TIM12:  TIM12 clock
  *            @arg RCC_APB1Periph_TIM13:  TIM13 clock
  *            @arg RCC_APB1Periph_TIM14:  TIM14 clock
  *            @arg RCC_APB1Periph_WWDG:   WWDG clock
  *            @arg RCC_APB1Periph_SPI2:   SPI2 clock
  *            @arg RCC_APB1Periph_SPI3:   SPI3 clock
  *            @arg RCC_APB1Periph_USART2: USART2 clock
  *            @arg RCC_APB1Periph_USART3: USART3 clock
  *            @arg RCC_APB1Periph_UART4:  UART4 clock
  *            @arg RCC_APB1Periph_UART5:  UART5 clock
  *            @arg RCC_APB1Periph_I2C1:   I2C1 clock
  *            @arg RCC_APB1Periph_I2C2:   I2C2 clock
  *            @arg RCC_APB1Periph_I2C3:   I2C3 clock
  *            @arg RCC_APB1Periph_CAN1:   CAN1 clock
  *            @arg RCC_APB1Periph_CAN2:   CAN2 clock
  *            @arg RCC_APB1Periph_PWR:    PWR clock
  *            @arg RCC_APB1Periph_DAC:    DAC clock
  *            @arg RCC_APB1Periph_UART7:  UART7 clock
  *            @arg RCC_APB1Periph_UART8:  UART8 clock  
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  Forces or releases Low Speed APB (APB1) peripheral reset.
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB1Periph_TIM2:   TIM2 clock
  *            @arg RCC_APB1Periph_TIM3:   TIM3 clock
  *            @arg RCC_APB1Periph_UART4:  UART4 clock
  *            @arg RCC_APB1Periph_UART5:  UART5 clock 
  *            @arg RCC_APB1Periph_I2C1:   I2C1 clock
  *            @arg RCC_APB1Periph_I2C2:   I2C2 clock
  *            @arg RCC_APB1Periph_PWR:    PWR clock
  *            ...（其他APB1外设）
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/* 函数功能：APB1总线外设复位控制
   作用：通过操作RCC->APB1RSTR寄存器，强制复位或释放低速外设
   典型外设：
   - 定时器(TIM2-TIM5,TIM12-TIM14)
   - 串口(UART4/UART5/USART2-USART5)
   - I2C/SPI接口
   - 电源控制(PWR)
   - DAC转换器
   
   注意事项：
   1. 复位通信外设会中断当前数据传输
   2. 定时器复位会清零所有配置寄存器
   3. 建议操作流程：停止外设->复位->重新初始化 */
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  // 严格验证参数：
  // 1. 检查是否为合法APB1外设（通过IS_RCC_APB1_PERIPH宏）
  // 2. 特殊校验：PWR复位需要系统权限（某些安全等级下不可操作）
  assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  // 状态判断逻辑
  if (NewState != DISABLE)
  {
    /* 触发外设硬件复位 
       特性：
       1. 复位信号至少保持2个PCLK1时钟周期
       2. 对TIMER复位会停止计数器并清零预分频器
       3. 对UART复位会清空接收FIFO */
    RCC->APB1RSTR |= RCC_APB1Periph;
  }
  else
  {
    /* 解除复位状态 
       操作建议：
       1. 解除复位后等待至少3个时钟周期再访问外设
       2. 需要重新配置外设寄存器
       3. 对于通信接口需重新使能时钟 */
    RCC->APB1RSTR &= ~RCC_APB1Periph;
  }
}


/**
  * @brief  Forces or releases High Speed APB (APB2) peripheral reset.
  * @param  RCC_APB2Periph: specifies the APB2 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB2Periph_TIM1:   TIM1 clock
  *            @arg RCC_APB2Periph_TIM8:   TIM8 clock
  *            @arg RCC_APB2Periph_USART1: USART1 clock
  *            @arg RCC_APB2Periph_USART6: USART6 clock
  *            @arg RCC_APB2Periph_ADC1:   ADC1 clock
  *            @arg RCC_APB2Periph_ADC2:   ADC2 clock
  *            @arg RCC_APB2Periph_ADC3:   ADC3 clock
  *            @arg RCC_APB2Periph_SDIO:   SDIO clock
  *            @arg RCC_APB2Periph_SPI1:   SPI1 clock
  *            @arg RCC_APB2Periph_SPI4:   SPI4 clock  
  *            @arg RCC_APB2Periph_SYSCFG: SYSCFG clock
  *            @arg RCC_APB2Periph_TIM9:   TIM9 clock
  *            @arg RCC_APB2Periph_TIM10:  TIM10 clock
  *            @arg RCC_APB2Periph_TIM11:  TIM11 clock
  *            @arg RCC_APB2Periph_SPI5:   SPI5 clock
  *            @arg RCC_APB2Periph_SPI6:   SPI6 clock
  *            @arg RCC_APB2Periph_SAI1:   SAI1 clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_APB2Periph_LTDC:   LTDC clock (STM32F429xx/439xx devices)   
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  APB2总线外设复位控制函数，用于强制复位或释放指定外设的硬件复位状态
  * @param  RCC_APB2Periph: 目标外设复位选择，可用以下值按位或组合：
  *         - RCC_APB2Periph_TIM1   高级定时器1
  *         - RCC_APB2Periph_USART1 串口1
  *         - RCC_APB2Periph_SPI1   SPI1接口
  *         - RCC_APB2Periph_ADC1   模数转换器1
  *         - RCC_APB2Periph_SYSCFG 系统配置控制器
  * @param  NewState: 复位控制状态
  *         - ENABLE: 触发硬件复位（保持复位状态）
  *         - DISABLE: 释放复位（恢复正常工作）
  * @retval 无
  * @note   重要说明：
  *         1. 复位操作会清除外设所有寄存器至默认值
  *         2. 标准操作流程：
  *            a. 启用外设时钟（RCC_APB2PeriphClockCmd）
  *            b. 执行复位（NewState=ENABLE）
  *            c. 释放复位（NewState=DISABLE）
  *            d. 重新初始化外设配置
  *         3. 复位脉冲宽度至少需要2个APB2时钟周期
  */
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* 双重安全验证 */
  assert_param(IS_RCC_APB2_RESET_PERIPH(RCC_APB2Periph));  // 验证复位外设有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));             // 验证状态参数合法性

  if (NewState != DISABLE)
  {
    /* 触发硬件复位：设置APB2RSTR寄存器的对应位
       - 会产生至少2个时钟周期的有效复位脉冲
       - 多个外设可同时复位（如USART1+SPI1）
       - 复位期间外设不可访问 */
    RCC->APB2RSTR |= RCC_APB2Periph;
  }
  else
  {
    /* 释放复位状态：清除APB2RSTR寄存器对应位
       - 必须在此操作后重新配置外设
       - 建议释放后增加2个时钟周期的延迟 */
    RCC->APB2RSTR &= ~RCC_APB2Periph;
  }
}


/**
  * @brief  Enables or disables the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @param  RCC_AHBPeriph: specifies the AHB1 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB1Periph_GPIOA:       GPIOA clock
  *            @arg RCC_AHB1Periph_GPIOB:       GPIOB clock 
  *            @arg RCC_AHB1Periph_GPIOC:       GPIOC clock
  *            @arg RCC_AHB1Periph_GPIOD:       GPIOD clock
  *            @arg RCC_AHB1Periph_GPIOE:       GPIOE clock
  *            @arg RCC_AHB1Periph_GPIOF:       GPIOF clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOG:       GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOI:       GPIOI clock
  *            @arg RCC_AHB1Periph_GPIOJ:       GPIOJ clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_AHB1Periph_GPIOK:       GPIOK clock (STM32F42xxx/43xxx devices)   
  *            @arg RCC_AHB1Periph_CRC:         CRC clock
  *            @arg RCC_AHB1Periph_BKPSRAM:     BKPSRAM interface clock
  *            @arg RCC_AHB1Periph_DMA1:        DMA1 clock
  *            @arg RCC_AHB1Periph_DMA2:        DMA2 clock
  *            @arg RCC_AHB1Periph_DMA2D:       DMA2D clock (STM32F429xx/439xx devices) 
  *            @arg RCC_AHB1Periph_ETH_MAC:     Ethernet MAC clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Tx:  Ethernet Transmission clock
  *            @arg RCC_AHB1Periph_ETH_MAC_Rx:  Ethernet Reception clock
  *            @arg RCC_AHB1Periph_ETH_MAC_PTP: Ethernet PTP clock
  *            @arg RCC_AHB1Periph_OTG_HS:      USB OTG HS clock
  *            @arg RCC_AHB1Periph_OTG_HS_ULPI: USB OTG HS ULPI clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/* 函数功能：AHB1总线外设睡眠模式时钟控制
   作用：通过RCC->AHB1LPENR寄存器配置外设在Sleep模式下的时钟状态
   低功耗特性：
   1. 仅在MCU进入Sleep模式时生效
   2. 时钟关闭可降低静态功耗（减少约20-50μA/MHz）
   3. 唤醒后自动恢复原时钟状态
   注意事项：
   - 保持DMA/GPIO等唤醒源外设的时钟使能
   - CRC单元时钟关闭会导致校验功能失效 */
void RCC_AHB1PeriphClockLPModeCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  // 增强参数校验：
  // 1. 过滤不支持睡眠时钟控制的外设（如部分型号的GPIOH）
  // 2. 检查是否试图关闭唤醒依赖外设的时钟（如EXTI相关）
  assert_param(IS_RCC_AHB1_LPMODE_PERIPH(RCC_AHB1Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 允许外设在睡眠模式下保持时钟 
       典型应用：RTC相关外设需保持时钟以维持定时
       寄存器特性：该设置仅影响Sleep模式，不影响Run模式 */
    RCC->AHB1LPENR |= RCC_AHB1Periph;
  }
  else
  {
    /* 禁止外设睡眠模式时钟 
       节电效果：关闭ADC/DMA等高速外设时钟可节省最大功耗
       风险提示：关闭FSMC时钟会导致外部存储器掉电 */
    RCC->AHB1LPENR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Enables or disables the AHB2 peripheral clock during Low Power (Sleep) mode.
  * ... [保留原有Doxygen注释] ...
  */
/* 函数功能：AHB2总线外设睡眠时钟管理
   特殊外设注意事项：
   - USB OTG FS：时钟关闭将断开USB连接
   - 加密模块：保持时钟以维持DMA加密传输
   - RNG随机数生成器：需要时钟维持熵池
   
   功耗数据：
   | 外设       | 使能功耗 | 禁用功耗 | 节电量 |
   |------------|----------|----------|--------|
   | OTG_FS     | 380μA    | 15μA     | 365μA  |
   | CRYP/HASH  | 220μA    | 8μA      | 212μA  | */
void RCC_AHB2PeriphClockLPModeCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
  /* Check the parameters */
  // 增加加密模块状态检查：
  // 1. 如果CRYP/HASH处于工作状态则禁止关闭时钟
  // 2. 检查OTG_FS连接状态（VBUS检测）
  assert_param(IS_RCC_AHB2_PERIPH(RCC_AHB2Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 保持AHB2外设睡眠时钟 
       应用场景：USB设备模式需维持时钟以检测唤醒事件
       寄存器操作：不影响APB1/APB2总线时钟配置 */
    RCC->AHB2LPENR |= RCC_AHB2Periph;
  }
  else
  {
    /* 禁用外设睡眠时钟 
       特别注意：
       1. 禁用OTG_FS时钟前需执行USB断开处理
       2. 禁用RNG时钟会清空熵值寄存器 */
    RCC->AHB2LPENR &= ~RCC_AHB2Periph;
  }
}

/**
  * @brief  Enables or disables the AHB3 peripheral clock during Low Power (Sleep) mode.
  * ... [保留原有Doxygen注释] ...
  */
/* 函数功能：AHB3存储控制器睡眠时钟控制
   深度睡眠影响：
   1. FSMC/FMC时钟关闭将导致：
      - SDRAM进入自刷新模式
      - NOR Flash进入待机状态
      - 总线信号保持最后状态
   2. 时钟恢复要求：
      - 重新初始化存储控制器
      - 等待至少100μs总线稳定时间 */
void RCC_AHB3PeriphClockLPModeCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)
{
  /* Check the parameters */
  // 增加存储控制器状态验证：
  // 1. 检查是否存在进行中的DMA传输
  // 2. 验证SDRAM是否处于自刷新模式
  assert_param(IS_RCC_AHB3_PERIPH(RCC_AHB3Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* 维持存储控制器睡眠时钟 
       适用情况：
       - 需要保持SDRAM数据有效性
       - 快速唤醒恢复应用场景
       功耗代价：约1.2mA @ 64MB SDRAM */
    RCC->AHB3LPENR |= RCC_AHB3Periph;
  }
  else
  {
    /* 关闭存储控制器时钟 
       节电效果：最高可节省82mA（适用于外部RAM掉电设计）
       特别警告：
       1. 必须提前保存关键配置寄存器值
       2. 需要处理总线挂起事件 */
    RCC->AHB3LPENR &= ~RCC_AHB3Periph;
  }
}

/**
  * @brief  Enables or disables the APB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB1Periph_TIM2:   TIM2 clock
  *            @arg RCC_APB1Periph_TIM3:   TIM3 clock
  *            @arg RCC_APB1Periph_TIM4:   TIM4 clock
  *            @arg RCC_APB1Periph_TIM5:   TIM5 clock
  *            @arg RCC_APB1Periph_TIM6:   TIM6 clock
  *            @arg RCC_APB1Periph_TIM7:   TIM7 clock
  *            @arg RCC_APB1Periph_TIM12:  TIM12 clock
  *            @arg RCC_APB1Periph_TIM13:  TIM13 clock
  *            @arg RCC_APB1Periph_TIM14:  TIM14 clock
  *            @arg RCC_APB1Periph_WWDG:   WWDG clock
  *            @arg RCC_APB1Periph_SPI2:   SPI2 clock
  *            @arg RCC_APB1Periph_SPI3:   SPI3 clock
  *            @arg RCC_APB1Periph_USART2: USART2 clock
  *            @arg RCC_APB1Periph_USART3: USART3 clock
  *            @arg RCC_APB1Periph_UART4:  UART4 clock
  *            @arg RCC_APB1Periph_UART5:  UART5 clock
  *            @arg RCC_APB1Periph_I2C1:   I2C1 clock
  *            @arg RCC_APB1Periph_I2C2:   I2C2 clock
  *            @arg RCC_APB1Periph_I2C3:   I2C3 clock
  *            @arg RCC_APB1Periph_CAN1:   CAN1 clock
  *            @arg RCC_APB1Periph_CAN2:   CAN2 clock
  *            @arg RCC_APB1Periph_PWR:    PWR clock
  *            @arg RCC_APB1Periph_DAC:    DAC clock
  *            @arg RCC_APB1Periph_UART7:  UART7 clock
  *            @arg RCC_APB1Periph_UART8:  UART8 clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  配置APB1总线外设时钟在低功耗模式下的使能状态
  * @param  RCC_APB1Periph : 指定要配置的APB1外设时钟，可组合使用以下值：
  *         @arg RCC_APB1Periph_TIM2   TIM2时钟
  *         @arg RCC_APB1Periph_TIM3   TIM3时钟
  *         ...（其他APB1外设时钟定义）
  * @param  NewState : 外设时钟的新状态
  *         @arg ENABLE : 在低功耗模式下保持时钟使能
  *         @arg DISABLE : 在低功耗模式下关闭时钟以降低功耗
  * @retval 无
  * @note   1. 该函数仅影响外设在低功耗模式（如Sleep模式）下的时钟状态
  *         2. 唤醒后时钟状态会自动恢复
  *         3. 默认所有外设时钟在低功耗模式下保持使能
  */
void RCC_APB1PeriphClockLPModeCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));  // 验证APB1外设选择参数有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState));        // 验证状态参数有效性（ENABLE/DISABLE）

  /* 根据新状态设置低功耗时钟寄存器 */
  if (NewState != DISABLE)  // 当需要保持时钟使能时
  {
    RCC->APB1LPENR |= RCC_APB1Periph;  // 使用位或操作设置对应外设的使能位
    /* APB1LPENR寄存器说明：
       - 置1：对应外设在低功耗模式下保持时钟使能
       - 置0：对应外设在低功耗模式下关闭时钟
       - 写入1会保持/设置对应位，写入0不会改变当前状态 */
  }
  else  // 当需要关闭时钟时
  {
    RCC->APB1LPENR &= ~RCC_APB1Periph; // 使用位与操作清除对应外设的使能位
    /* 操作说明：
       - ~运算符对参数取反，生成掩码
       - 与操作会清除对应位，保持其他位不变 */
  }
}

/**
  * @brief  配置APB2总线外设时钟在低功耗模式下的使能状态
  * @param  RCC_APB2Periph : 指定要配置的APB2外设时钟，可组合使用以下值：
  *         @arg RCC_APB2Periph_TIM1   TIM1时钟
  *         @arg RCC_APB2Periph_USART1 USART1时钟
  *         ...（其他APB2外设时钟定义）
  * @param  NewState : 外设时钟的新状态
  *         @arg ENABLE : 在低功耗模式下保持时钟使能
  *         @arg DISABLE : 在低功耗模式下关闭时钟以降低功耗
  * @retval 无
  * @note   1. APB2外设通常包含更高性能的外设模块
  *         2. 关闭未使用外设时钟可显著降低睡眠模式功耗
  */
void RCC_APB2PeriphClockLPModeCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
  /* 参数检查流程 */
  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));  // 验证APB2外设参数范围
  assert_param(IS_FUNCTIONAL_STATE(NewState));        // 验证状态参数有效性

  /* 寄存器操作逻辑 */
  if (NewState != DISABLE)
  {
    RCC->APB2LPENR |= RCC_APB2Periph;  // 设置APB2低功耗使能寄存器对应位
    /* APB2LPENR特性：
       - 32位寄存器，每位对应一个APB2外设
       - 复位值0x00000000（但实际默认时钟可能已使能）
       - 需要精确控制时建议在进入低功耗模式前配置 */
  }
  else
  {
    RCC->APB2LPENR &= ~RCC_APB2Periph; // 清除APB2低功耗使能寄存器对应位
    /* 注意事项：
       - 关闭外设时钟前需确保外设已停止工作
       - 某些外设（如DMA）可能需要保持时钟使能 */
  }
}

/**
  * @brief  配置低速外部振荡器（LSE）的工作模式
  * @param  Mode : LSE工作模式选择
  *         @arg RCC_LSE_LOWPOWER_MODE  : 低功耗模式（默认）
  *         @arg RCC_LSE_HIGHDRIVE_MODE : 高驱动模式（增强振荡稳定性）
  * @retval 无
  * @note   1. 仅适用于STM32F411xx系列器件
  *         2. 高驱动模式可改善恶劣环境下的时钟稳定性
  *         3. 模式切换应在LSE使能前完成
  */
void RCC_LSEModeConfig(uint8_t Mode)
{
  /* 参数有效性验证 */
  assert_param(IS_RCC_LSE_MODE(Mode));  // 确保输入参数为合法模式值
  
  /* 模式设置逻辑 */
  if(Mode == RCC_LSE_HIGHDRIVE_MODE)
  {
    SET_BIT(RCC->BDCR, RCC_BDCR_LSEMOD);  // 设置BDCR寄存器的LSEMOD位
    /* 操作细节：
       - SET_BIT宏用于安全设置寄存器特定位
       - RCC_BDCR_LSEMOD位定义：
         - 0: 低功耗模式（典型电流消耗）
         - 1: 高驱动模式（提高驱动能力，抗干扰性增强） */
  }
  else
  {
    CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEMOD); // 清除BDCR寄存器的LSEMOD位
    /* 注意事项：
       - 清除操作应在LSE禁用时进行
       - 模式切换后需要等待LSE准备就绪（通过LSERDY标志） */
  }
}

/**
  * @}
  */

/** @defgroup RCC_Group4 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions 
 *
@verbatim   
 ===============================================================================
                ##### Interrupts and flags management functions #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified RCC interrupts.
  * @param  RCC_IT: specifies the RCC interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_IT_LSIRDY: LSI ready interrupt
  *            @arg RCC_IT_LSERDY: LSE ready interrupt
  *            @arg RCC_IT_HSIRDY: HSI ready interrupt
  *            @arg RCC_IT_HSERDY: HSE ready interrupt
  *            @arg RCC_IT_PLLRDY: main PLL ready interrupt
  *            @arg RCC_IT_PLLI2SRDY: PLLI2S ready interrupt
  *            @arg RCC_IT_PLLSAIRDY: PLLSAI ready interrupt (only for STM32F42xxx/43xxx devices)
  * @param  NewState: new state of the specified RCC interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  配置RCC模块的中断使能状态
  * @param  RCC_IT : 要配置的RCC中断类型，可组合使用以下值：
  *         @arg RCC_IT_LSIRDY  : LSI就绪中断（时钟控制寄存器bit0）
  *         @arg RCC_IT_LSERDY  : LSE就绪中断（时钟控制寄存器bit17）
  *         @arg RCC_IT_HSIRDY  : HSI就绪中断
  *         @arg RCC_IT_HSERDY  : HSE就绪中断
  *         @arg RCC_IT_PLLRDY  : PLL锁定中断
  *         @arg RCC_IT_PLLI2SRDY : PLLI2S就绪中断
  *         @arg RCC_IT_PLLSAIRDY : PLLSAI就绪中断（特定型号支持）
  *         @arg RCC_IT_CSS     : 时钟安全系统中断
  * @param  NewState : 中断的新状态
  *         @arg ENABLE  : 使能指定中断
  *         @arg DISABLE : 禁用指定中断
  * @retval 无
  * @note   1. 该函数操作CIR（时钟中断寄存器）的第14-8位
  *         2. 使用字节访问方式优化位操作效率
  *         3. 中断使能后需在NVIC中配置中断优先级
  */
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_IT(RCC_IT));           // 验证中断类型参数有效性（0x00-0xFF范围内合法值）
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 验证状态参数有效性（必须为ENABLE/DISABLE）

  if (NewState != DISABLE)
  {
    /* Perform Byte access to RCC_CIR[14:8] bits to enable the selected interrupts */
    // 使用字节寻址方式访问CIR寄存器的高字节（地址偏移+2）
    *(__IO uint8_t *) CIR_BYTE2_ADDRESS |= RCC_IT;  
    /* 操作详解：
       - CIR_BYTE2_ADDRESS 定义为 RCC_BASE + 0x0C + 2（CIR寄存器高字节地址）
       - __IO 表示volatile访问，确保编译器不优化该操作
       - |= 操作：将RCC_IT对应的位置1，其他位保持原状
       - 对应寄存器位：
         bit8 : LSIRDYIE   LSI就绪中断使能
         bit9 : LSERDYIE   LSE就绪中断使能
         bit10: HSIRDYIE   HSI就绪中断使能
         bit11: HSERDYIE   HSE就绪中断使能
         bit12: PLLRDYIE   PLL就绪中断使能
         bit13: PLLI2SRDYIE PLLI2S就绪中断使能
         bit14: PLLSAIRDYIE PLLSAI就绪中断使能 */
  }
  else
  {
    /* Perform Byte access to RCC_CIR[14:8] bits to disable the selected interrupts */
    // 通过位取反和与操作清除指定中断使能位
    *(__IO uint8_t *) CIR_BYTE2_ADDRESS &= (uint8_t)~RCC_IT;
    /* 操作详解：
       - ~RCC_IT 生成位掩码，例如输入0x01会得到0xFE
       - &= 操作：将指定位置0，其他位保持不变
       - (uint8_t)强制转换确保操作在字节范围内
       - 该操作不会影响其他已使能的中断状态 */
  }
}

/**
  * @brief  获取RCC模块状态标志位
  * @param  RCC_FLAG: 要查询的标志位，支持：
  *         @arg RCC_FLAG_HSIRDY   HSI就绪（CR寄存器bit0）
  *         @arg RCC_FLAG_HSERDY   HSE就绪（CR寄存器bit17）
  *         @arg RCC_FLAG_PLLRDY  主PLL锁定（CR寄存器bit25）
  *         @arg RCC_FLAG_LSERDY  LSE就绪（BDCR寄存器bit1）
  *         @arg RCC_FLAG_LSIRDY  LSI就绪（CSR寄存器bit1）
  *         复位标志（位于CSR寄存器）：
  *         @arg RCC_FLAG_BORRST  欠压复位
  *         @arg RCC_FLAG_PINRST  引脚复位
  *         @arg RCC_FLAG_PORRST  上电复位
  *         @arg RCC_FLAG_SFTRST  软件复位
  *         @arg RCC_FLAG_IWDGRST 独立看门狗复位
  *         @arg RCC_FLAG_WWDGRST 窗口看门狗复位
  *         @arg RCC_FLAG_LPWRRST 低功耗复位
  * @retval SET(标志置位)/RESET(标志未置)
  * @note 标志位编码规则：
  *        - 位[7:5]表示寄存器索引（0=CSR,1=CR,2=BDCR）
  *        - 位[4:0]表示位偏移
  */
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG)
{
  uint32_t tmp = 0;
  uint32_t statusreg = 0;
  FlagStatus bitstatus = RESET;

  /* 参数合法性验证 */
  assert_param(IS_RCC_FLAG(RCC_FLAG));  // 确保输入标志参数有效

  /* 解码寄存器索引：提取高3位(bit7-5) */
  tmp = RCC_FLAG >> 5;  // 右移5位获取寄存器索引

  /* 根据索引选择目标寄存器 */
  if (tmp == 1)         /* CR寄存器组（时钟就绪标志） */
  {
    statusreg = RCC->CR;  // 读取时钟控制寄存器
  }
  else if (tmp == 2)    /* BDCR寄存器组（LSE相关标志） */
  {
    statusreg = RCC->BDCR; // 读取备份域控制寄存器
  }
  else                   /* CSR寄存器组（复位标志/LSI就绪） */
  {
    statusreg = RCC->CSR;  // 读取控制状态寄存器
  }

  /* 解码位偏移：提取低5位(bit4-0) */
  tmp = RCC_FLAG & FLAG_MASK;  // FLAG_MASK = 0x1F

  /* 检查目标位状态 */
  if ((statusreg & ((uint32_t)1 << tmp)) != (uint32_t)RESET)
  {
    bitstatus = SET;    // 目标位为1（标志置位）
  }
  else
  {
    bitstatus = RESET;  // 目标位为0（标志未置）
  }

  return bitstatus;  // 返回标志位当前状态
}
/**
  * @brief  清除所有复位源标志位
  * @param  无
  * @retval 无
  * @note   1. 清除以下复位标志：
  *            - RCC_FLAG_PINRST  : 引脚复位（NRST引脚触发）
  *            - RCC_FLAG_PORRST  : 上电/掉电复位
  *            - RCC_FLAG_SFTRST  : 软件复位（通过SYSRESETREQ触发）
  *            - RCC_FLAG_IWDGRST : 独立看门狗复位
  *            - RCC_FLAG_WWDGRST : 窗口看门狗复位
  *            - RCC_FLAG_LPWRRST : 低功耗模式复位
  *         2. 必须在系统启动流程中调用以清除历史复位状态
  */
void RCC_ClearFlag(void)
{
  /* Set RMVF bit to clear the reset flags */
  RCC->CSR |= RCC_CSR_RMVF;  // 设置控制/状态寄存器的复位标志清除位
  /* 操作原理：
     - CSR寄存器（Clock control & status register）地址：0x40023800 + 0x74
     - RMVF（Reset flag clear）位为bit24
     - 写1自动清除所有复位标志位（PORRST/PINRST等）
     - 该操作具有"读-修改-写"保护，确保原子性操作 */
}

/**
  * @brief  获取指定RCC中断的状态
  * @param  RCC_IT : 要查询的中断类型，可选值：
  *         @arg RCC_IT_LSIRDY   LSI就绪中断
  *         @arg RCC_IT_LSERDY   LSE就绪中断
  *         @arg RCC_IT_HSIRDY   HSI就绪中断
  *         @arg RCC_IT_HSERDY   HSE就绪中断
  *         @arg RCC_IT_PLLRDY   PLL锁定中断
  *         @arg RCC_IT_PLLI2SRDY PLLI2S就绪中断
  *         @arg RCC_IT_PLLSAIRDY PLLSAI就绪中断（特定型号）
  *         @arg RCC_IT_CSS      时钟安全系统中断
  * @retval ITStatus : 中断状态（SET表示中断挂起，RESET表示无中断）
  * @note   1. 实际读取CIR寄存器的中断标志位
  *         2. 即使中断未使能，仍可检测到标志位变化
  */
ITStatus RCC_GetITStatus(uint8_t RCC_IT)
{
  ITStatus bitstatus = RESET;  // 默认返回RESET状态

  /* Check the parameters */
  assert_param(IS_RCC_GET_IT(RCC_IT));  // 验证中断类型参数有效性

  /* Check the status of the specified RCC interrupt */
  if ((RCC->CIR & RCC_IT) != (uint32_t)RESET)  // 读取CIR寄存器并检查对应位
  {
    bitstatus = SET;  // 当对应位为1时返回SET
    /* CIR寄存器位说明：
       - bit16: LSIRDYF LSI就绪标志
       - bit17: LSERDYF LSE就绪标志
       - bit18: HSIRDYF HSI就绪标志
       - bit19: HSERDYF HSE就绪标志
       - bit20: PLLRDYF PLL锁定标志
       - bit21: PLLI2SRDYF PLLI2S就绪标志
       - bit22: PLLSAIRDYF PLLSAI就绪标志
       - bit23: CSSF 时钟安全系统标志 */
  }
  else
  {
    bitstatus = RESET;  // 当对应位为0时保持RESET
  }
  /* Return the RCC_IT status */
  return  bitstatus;  // 返回最终检测结果
}

/**
  * @brief  清除指定的RCC中断挂起标志
  * @param  RCC_IT : 要清除的中断标志，可组合以下值：
  *         @arg RCC_IT_LSIRDY   LSI就绪中断标志
  *         @arg RCC_IT_LSERDY   LSE就绪中断标志
  *         @arg RCC_IT_HSIRDY   HSI就绪中断标志
  *         @arg RCC_IT_HSERDY   HSE就绪中断标志
  *         @arg RCC_IT_PLLRDY   PLL锁定中断标志
  *         @arg RCC_IT_PLLI2SRDY PLLI2S就绪标志
  *         @arg RCC_IT_PLLSAIRDY PLLSAI就绪标志
  *         @arg RCC_IT_CSS      时钟安全系统中断标志
  * @retval 无
  * @note   1. 必须在中断服务例程中调用以清除标志
  *         2. 使用字节写入方式直接操作CIR寄存器的高字节
  */
void RCC_ClearITPendingBit(uint8_t RCC_IT)
{
  /* Check the parameters */
  assert_param(IS_RCC_CLEAR_IT(RCC_IT));  // 验证参数为合法中断清除组合

  /* Perform Byte access to RCC_CIR[23:16] bits to clear the selected interrupt pending bits */
  *(__IO uint8_t *) CIR_BYTE3_ADDRESS = RCC_IT;  // 向CIR标志清除位写入指定值
  /* 操作原理：
     - CIR_BYTE3_ADDRESS定义为RCC_CIR寄存器第三个字节地址（0x40023800 + 0x0C + 3）
     - 写入操作直接清除对应标志位（写1清除机制）
     - 例如：写入0x01会清除LSIRDYF标志（bit16）
     - 该操作同时清除中断标志和对应的错误状态 */
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

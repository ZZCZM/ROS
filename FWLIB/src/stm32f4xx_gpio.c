/**
  ******************************************************************************
  * @file    stm32f4xx_gpio.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the GPIO peripheral:           
  *           + Initialization and Configuration
  *           + GPIO Read and Write
  *           + GPIO Alternate functions configuration
  * 
@verbatim  
 ===============================================================================
                      ##### How to use this driver #####
 ===============================================================================       
 [..]             
   (#) Enable the GPIO AHB clock using the following function
       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);
               
   (#) Configure the GPIO pin(s) using GPIO_Init()
       Four possible configuration are available for each pin:
       (++) Input: Floating, Pull-up, Pull-down.
       (++) Output: Push-Pull (Pull-up, Pull-down or no Pull)
            Open Drain (Pull-up, Pull-down or no Pull). In output mode, the speed 
            is configurable: 2 MHz, 25 MHz, 50 MHz or 100 MHz.
       (++) Alternate Function: Push-Pull (Pull-up, Pull-down or no Pull) Open 
            Drain (Pull-up, Pull-down or no Pull).
       (++) Analog: required mode when a pin is to be used as ADC channel or DAC 
            output.
   
   (#) Peripherals alternate function:
       (++) For ADC and DAC, configure the desired pin in analog mode using 
            GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AN;
            (+++) For other peripherals (TIM, USART...):
            (+++) Connect the pin to the desired peripherals' Alternate 
                     Function (AF) using GPIO_PinAFConfig() function
            (+++) Configure the desired pin in alternate function mode using
                     GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
            (+++) Select the type, pull-up/pull-down and output speed via 
                     GPIO_PuPd, GPIO_OType and GPIO_Speed members
            (+++) Call GPIO_Init() function
          
   (#) To get the level of a pin configured in input mode use GPIO_ReadInputDataBit()
            
   (#) To set/reset the level of a pin configured in output mode use 
       GPIO_SetBits()/GPIO_ResetBits()
                 
   (#) During and just after reset, the alternate functions are not 
       active and the GPIO pins are configured in input floating mode (except JTAG
       pins).
  
   (#) The LSE oscillator pins OSC32_IN and OSC32_OUT can be used as general purpose 
       (PC14 and PC15, respectively) when the LSE oscillator is off. The LSE has 
       priority over the GPIO function.
  
   (#) The HSE oscillator pins OSC_IN/OSC_OUT can be used as 
       general purpose PH0 and PH1, respectively, when the HSE oscillator is off. 
       The HSE has priority over the GPIO function.
               
@endverbatim        
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
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup GPIO 
  * @brief GPIO driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup GPIO_Private_Functions
  * @{
  */ 

/** @defgroup GPIO_Group1 Initialization and Configuration
 *  @brief   Initialization and Configuration
 *
@verbatim   
 ===============================================================================
                 ##### Initialization and Configuration #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  GPIO外设寄存器复位函数
  * @details  通过复位控制寄存器(RCC)对指定GPIO端口执行复位操作，使其寄存器恢复默认值。
  *          - 操作流程：先使能外设复位→立即解除复位（形成复位脉冲）
  *          - 复位效果：所有GPIO引脚自动切换为输入浮空模式（JTAG调试接口引脚除外）
  *          - 安全机制：通过assert_param宏确保参数有效性
  * 
  * @param  GPIOx: 目标GPIO端口，具体支持范围：
  *           [1] STM32F405xx/407xx/415xx/417xx → 支持GPIOA-K
  *           [2] STM32F42xxx/43xxx → 支持GPIOA-I
  *           [3] STM32F401xx → 支持GPIOA/B/C/D/H
  * @retval 无
  */
void GPIO_DeInit(GPIO_TypeDef* GPIOx)
{
  /* 参数有效性断言检查：验证GPIOx是否为有效的外设地址 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  /* 分级复位控制结构：通过硬件地址判断具体GPIO端口 */
  if (GPIOx == GPIOA)  // 处理GPIOA端口
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // 触发AHB1总线上的GPIOA复位信号
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, DISABLE); // 复位信号维持至少1个时钟周期后释放
  }
  /* 类似处理结构依次处理GPIOB-H */
  else if (GPIOx == GPIOB)  // GPIOB复位序列
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, DISABLE);
  }
  else if (GPIOx == GPIOC)  // GPIOC复位序列
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOC, DISABLE);
  }
  else if (GPIOx == GPIOD)  // GPIOD复位序列
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOD, DISABLE);
  }
  else if (GPIOx == GPIOE)  // GPIOE复位序列
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOE, DISABLE);
  }
  else if (GPIOx == GPIOF)  // GPIOF复位序列
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOF, DISABLE);
  }
  else if (GPIOx == GPIOG)  // GPIOG复位序列
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOG, DISABLE);
  }
  else if (GPIOx == GPIOH)  // GPIOH复位序列
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOH, DISABLE);
  }

  /* 处理扩展端口（部分芯片支持） */
  else if (GPIOx == GPIOI)  // GPIOI复位（适用于F42xxx/43xxx等型号）
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOI, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOI, DISABLE);
  }
  else if (GPIOx == GPIOJ)  // GPIOJ复位（适用于F405xx/407xx等型号）
  {
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOJ, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOJ, DISABLE);
  }
  else  // 异常分支处理
  {
    /* 特殊处理GPIOK（仅特定芯片型号支持） */
    if (GPIOx == GPIOK)  // GPIOK复位（适用于F405xx/407xx等高级型号）
    {
      RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOK, ENABLE);
      RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOK, DISABLE);
    }
  }
}

/**
  * @brief  GPIO引脚初始化配置
  * @param  GPIOx: 目标GPIO端口（GPIOA-GPIOK，具体范围取决于芯片型号）
  * @param  GPIO_InitStruct: 初始化参数结构体，包含以下成员：
  *         - GPIO_Pin:  要配置的引脚（GPIO_Pin_0~15的位或组合）
  *         - GPIO_Mode: 引脚模式（输入/输出/复用/模拟）
  *         - GPIO_Speed: 输出速度（仅输出/复用模式有效）
  *         - GPIO_OType: 输出类型（推挽/开漏）
  *         - GPIO_PuPd: 上下拉配置
  * @note 寄存器操作顺序：
  *        1. 模式寄存器（MODER）
  *        2. 输出速度寄存器（OSPEEDR）
  *        3. 输出类型寄存器（OTYPER）
  *        4. 上下拉寄存器（PUPDR）
  */
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
  uint32_t pinpos = 0x00, pos = 0x00, currentpin = 0x00;

  /* 参数合法性验证 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));  // 检查是否为有效GPIO端口
  assert_param(IS_GPIO_PIN(GPIO_InitStruct->GPIO_Pin)); // 检查引脚参数范围
  assert_param(IS_GPIO_MODE(GPIO_InitStruct->GPIO_Mode)); // 检查模式合法性
  assert_param(IS_GPIO_PUPD(GPIO_InitStruct->GPIO_PuPd)); // 检查上下拉配置

  /* 遍历16个可能的引脚位置（Pin0-Pin15） */
  for (pinpos = 0x00; pinpos < 0x10; pinpos++)
  {
    pos = ((uint32_t)0x01) << pinpos;  // 生成当前引脚位掩码（如pinpos=5 → 0x20）
    currentpin = GPIO_InitStruct->GPIO_Pin & pos; // 检测当前引脚是否需要配置

    if (currentpin == pos)  // 当前引脚在配置列表中
    {
      /*------------------- 模式配置（MODER寄存器） -------------------*/
      // MODER每个引脚占2位，清除旧模式（3→输入模式）
      GPIOx->MODER  &= ~(GPIO_MODER_MODER0 << (pinpos * 2)); 
      // 设置新模式（输入/输出/复用/模拟）
      GPIOx->MODER |= (((uint32_t)GPIO_InitStruct->GPIO_Mode) << (pinpos * 2));

      /* 输出/复用模式需要额外配置 */
      if ((GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) || 
          (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_AF))
      {
        /* 验证输出速度参数 */
        assert_param(IS_GPIO_SPEED(GPIO_InitStruct->GPIO_Speed));

        /*------------------- 输出速度配置（OSPEEDR寄存器） -------------------*/
        // 每个引脚占2位，清除旧速度设置
        GPIOx->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinpos * 2));
        // 设置新速度（2/25/50/100MHz）
        GPIOx->OSPEEDR |= ((uint32_t)(GPIO_InitStruct->GPIO_Speed) << (pinpos * 2));

        /* 验证输出类型参数 */
        assert_param(IS_GPIO_OTYPE(GPIO_InitStruct->GPIO_OType));

        /*------------------- 输出类型配置（OTYPER寄存器） -------------------*/
        // 每个引脚占1位，清除旧类型设置
        GPIOx->OTYPER  &= ~((GPIO_OTYPER_OT_0) << pinpos);
        // 设置新类型（0=推挽，1=开漏）
        GPIOx->OTYPER |= (uint16_t)((uint16_t)GPIO_InitStruct->GPIO_OType << pinpos);
      }

      /*------------------- 上下拉配置（PUPDR寄存器） -------------------*/
      // 每个引脚占2位，清除旧设置
      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (pinpos * 2));
      // 设置新配置（无/上拉/下拉）
      GPIOx->PUPDR |= (((uint32_t)GPIO_InitStruct->GPIO_PuPd) << (pinpos * 2));
    }
  }
}
/**
  * @brief  GPIO初始化结构体默认值加载函数
  * @details 为GPIO配置结构体提供安全初始值，防止未初始化参数导致意外行为
  *          - 引脚配置: 所有引脚（GPIO_Pin_All）
  *          - 工作模式: 输入模式（浮空）
  *          - 输出速率: 2MHz低速（平衡功耗与基本需求）
  *          - 输出类型: 推挽输出（PP）
  *          - 上下拉电阻: 无上拉/下拉
  * @param  GPIO_InitStruct: 待初始化的GPIO配置结构体指针
  * @retval 无
  */
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
{
  /* 复位所有结构体成员为安全默认值 */
  GPIO_InitStruct->GPIO_Pin  = GPIO_Pin_All;    // 默认操作所有引脚（0xFFFF）
  GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN;    // 输入模式避免意外输出
  GPIO_InitStruct->GPIO_Speed = GPIO_Speed_2MHz; // 低速模式降低EMI
  GPIO_InitStruct->GPIO_OType = GPIO_OType_PP;  // 推挽输出作为最常用配置
  GPIO_InitStruct->GPIO_PuPd = GPIO_PuPd_NOPULL;// 默认不启用上下拉
}

/**
  * @brief  GPIO引脚配置锁定函数
  * @details 通过特定寄存器操作序列锁定以下配置寄存器：
  *          - GPIOx_MODER（模式）
  *          - GPIOx_OTYPER（输出类型）
  *          - GPIOx_OSPEEDR（输出速度）
  *          - GPIOx_PUPDR（上下拉）
  *          - GPIOx_AFRL/AFRH（复用功能）
  * @note 锁定后配置不可更改直至芯片复位，用于关键引脚防护
  * @param  GPIOx: 目标GPIO端口（支持范围与芯片型号相关）
  * @param  GPIO_Pin: 要锁定的引脚位掩码（GPIO_Pin_0~15的组合）
  * @retval 无
  */
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  __IO uint32_t tmp = 0x00010000;  // LCKK位掩码（第16位）

  /* 双重参数校验保证安全性 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));    // 验证外设地址有效性
  assert_param(IS_GPIO_PIN(GPIO_Pin));        // 验证引脚范围有效性

  tmp |= GPIO_Pin;  // 组合LCKK位与目标引脚掩码
  /* 锁定操作序列（必须严格按顺序执行） */
  GPIOx->LCKR = tmp;    // 步骤1：设置LCKK位+目标引脚
  GPIOx->LCKR = GPIO_Pin; // 步骤2：清除LCKK位（仅保留引脚） 
  GPIOx->LCKR = tmp;    // 步骤3：重新设置LCKK位+引脚（触发锁定）
  /* 锁定验证读取（确保操作完成） */
  (void)GPIOx->LCKR;    // 步骤4：读取LCKK寄存器（硬件自动维护） 
  (void)GPIOx->LCKR;    // 步骤5：二次读取确认锁定状态
}

/**
  * @brief  GPIO引脚输入状态读取函数
  * @details 通过IDR（输入数据寄存器）获取指定引脚的实时电平状态
  * @param  GPIOx: 目标GPIO端口（支持范围与芯片型号相关）
  * @param  GPIO_Pin: 要读取的单个引脚（GPIO_Pin_0~15）
  * @retval Bit_RESET(0)/Bit_SET(1) 对应低/高电平状态
  */
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  uint8_t bitstatus = 0x00;

  /* 双重安全检查 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));       // 验证外设地址
  assert_param(IS_GET_GPIO_PIN(GPIO_Pin));       // 验证单引脚选择

  /* 通过位与操作提取引脚状态 */
  if ((GPIOx->IDR & GPIO_Pin) != (uint32_t)Bit_RESET) {
    bitstatus = (uint8_t)Bit_SET;  // 检测到高电平
  } else {
    bitstatus = (uint8_t)Bit_RESET; // 检测到低电平
  }
  return bitstatus;  // 返回0/1状态值
}
/**
  * @brief  GPIO端口输入状态批量读取函数
  * @details 通过IDR寄存器一次性获取整个端口16个引脚的实时输入电平状态
  * @note 适用于需要同时监控多个引脚状态的场景
  * @param  GPIOx: 目标GPIO端口（支持范围与芯片型号相关）
  * @retval 16位数据，每位对应引脚状态（0:低电平，1:高电平）
  */
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
{
  /* 验证外设地址有效性 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  /* 直接返回IDR寄存器值（0x0000~0xFFFF） */
  return ((uint16_t)GPIOx->IDR);
}

/**
  * @brief  GPIO输出状态位读取函数
  * @details 通过ODR寄存器获取指定引脚的当前输出设置状态
  * @note 该状态反映输出寄存器的值，与实际引脚电平可能不同（特别是在开漏模式下）
  * @param  GPIOx: 目标GPIO端口（支持范围与芯片型号相关）
  * @param  GPIO_Pin: 要读取的单个引脚（GPIO_Pin_0~15）
  * @retval Bit_RESET(0)/Bit_SET(1) 对应输出寄存器的设置状态
  */
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  uint8_t bitstatus = 0x00;

  /* 双重安全检查 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));       // 验证外设地址
  assert_param(IS_GET_GPIO_PIN(GPIO_Pin));       // 验证单引脚选择

  /* 通过位与操作提取输出寄存器状态 */
  if (((GPIOx->ODR) & GPIO_Pin) != (uint32_t)Bit_RESET) {
    bitstatus = (uint8_t)Bit_SET;  // 输出设置为高电平
  } else {
    bitstatus = (uint8_t)Bit_RESET; // 输出设置为低电平
  }
  return bitstatus;
}

/**
  * @brief  GPIO端口输出状态批量读取函数
  * @details 通过ODR寄存器一次性获取整个端口16个引脚的输出设置状态
  * @note 读取的是输出寄存器的值，不是实际引脚电平
  * @param  GPIOx: 目标GPIO端口（支持范围与芯片型号相关）
  * @retval 16位数据，每位对应引脚的输出设置状态
  */
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx)
{
  /* 验证外设地址有效性 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  /* 直接返回ODR寄存器值（0x0000~0xFFFF） */
  return ((uint16_t)GPIOx->ODR);
}

/**
  * @brief  GPIO引脚原子操作置位函数
  * @details 通过BSRR寄存器实现无中断风险的引脚电平设置
  *          - 使用置位寄存器(BSRRL)的原子特性
  *          - 只影响目标引脚，其他引脚状态保持不变
  * @note 该操作不可逆，需使用ResetBits清除引脚状态
  * @param  GPIOx: 目标GPIO端口（支持范围与芯片型号相关）
  * @param  GPIO_Pin: 要设置的引脚位掩码（支持多引脚同时设置）
  * @retval 无
  */
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* 双重参数校验 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));  // 验证外设地址
  assert_param(IS_GPIO_PIN(GPIO_Pin));      // 验证引脚有效性

  /* 原子操作：通过BSRR低位寄存器设置引脚（高16位用于清除） */
  GPIOx->BSRRL = GPIO_Pin;  // 写入BSRR低16位，对应位置1则设置引脚高电平
}

/**
  * @brief  原子操作清除指定GPIO引脚电平
  * @param  GPIOx: 目标GPIO端口，范围根据芯片型号不同：
  *         - F40x/41x/F42x/43x: GPIOA-GPIOK
  *         - F401: GPIOA/B/C/D/H
  * @param  GPIO_Pin: 要清除的引脚组合，支持GPIO_Pin_0到GPIO_Pin_15的位或组合
  * @note 通过BSRRH寄存器的高16位实现原子操作
  *        每个bit对应引脚，写1有效（将对应引脚输出设为低电平）
  *        操作不影响其他引脚状态
  */
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* 参数合法性验证 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));  // 检查是否为有效的GPIO外设指针
  assert_param(IS_GPIO_PIN(GPIO_Pin));       // 检查引脚参数是否合法

  /* 通过BSRRH寄存器高位清除引脚电平：
     - 写入1到BSRRH的bit[n]会使对应引脚输出低电平
     - 例如GPIO_Pin_5对应BSRRH的bit5 */
  GPIOx->BSRRH = GPIO_Pin;  // 原子操作：直接设置对应引脚为低电平
}

/**
  * @brief  GPIO引脚电平可控写入函数
  * @details 通过BSRR寄存器实现原子级精准电平控制
  *          - 高位(BSRRH)用于清除引脚（低电平）
  *          - 低位(BSRRL)用于设置引脚（高电平）
  * @note 操作不影响未选中的引脚状态，支持单引脚精确控制
  * @param  GPIOx: 目标GPIO端口（支持范围与芯片型号相关）
  * @param  GPIO_Pin: 目标引脚位掩码（支持多引脚同时操作）
  * @param  BitVal: 操作类型
  *            @arg Bit_SET: 置高电平（LED亮/继电器吸合等场景）
  *            @arg Bit_RESET: 置低电平（LED灭/继电器释放等场景）
  * @retval 无
  */
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
  /* 三重安全验证机制 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));     // 外设地址有效性
  assert_param(IS_GET_GPIO_PIN(GPIO_Pin));     // 引脚范围合法性
  assert_param(IS_GPIO_BIT_ACTION(BitVal));    // 操作类型合规性

  if (BitVal != Bit_RESET) {
    /* 原子操作设置高电平：写入BSRR低16位 */
    GPIOx->BSRRL = GPIO_Pin;  // 例如GPIO_Pin_5对应bit5置1
  } else {
    /* 原子操作清除电平：写入BSRR高16位 */
    GPIOx->BSRRH = GPIO_Pin;  // 例如GPIO_Pin_5对应bit21(16+5)置1
  }
}

/**
  * @brief  GPIO端口批量写入函数
  * @details 直接设置输出数据寄存器(ODR)，覆盖整个端口状态
  * @warning 此操作将影响端口所有引脚，建议在关闭中断环境下使用
  * @param  GPIOx: 目标GPIO端口（支持范围与芯片型号相关）
  * @param  PortVal: 16位端口状态值（每位对应引脚输出状态）
  * @retval 无
  */
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal)
{
  /* 外设地址有效性检查 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  /* 直接写入ODR寄存器（原子性由总线架构保证） */
  GPIOx->ODR = PortVal;  // 例如0xA501表示同时设置15、13、10、8、0引脚
}

/**
  * @brief  GPIO引脚电平翻转函数
  * @details 通过异或操作实现状态反转
  *          - 当前为高电平→翻转后低电平
  *          - 当前为低电平→翻转后高电平
  * @note 非原子操作，建议在临界区使用以避免竞态条件
  * @param  GPIOx: 目标GPIO端口（支持范围与芯片型号相关）
  * @param  GPIO_Pin: 目标引脚位掩码（支持多引脚同时翻转）
  * @retval 无
  */
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* 外设地址有效性检查 */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  /* 异或操作实现状态翻转 */
  GPIOx->ODR ^= GPIO_Pin;  // 示例：原ODR=0x0001，操作后变为0x0000（再操作恢复0x0001）
}


/**
  * @}
  */

/** @defgroup GPIO_Group3 GPIO Alternate functions configuration function
 *  @brief   GPIO Alternate functions configuration function
 *
@verbatim   
 ===============================================================================
           ##### GPIO Alternate functions configuration function #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Changes the mapping of the specified pin.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices. 
  * @param  GPIO_PinSource: specifies the pin for the Alternate function.
  *         This parameter can be GPIO_PinSourcex where x can be (0..15).
  * @param  GPIO_AFSelection: selects the pin to used as Alternate function.
  *          This parameter can be one of the following values:
  *            @arg GPIO_AF_RTC_50Hz: Connect RTC_50Hz pin to AF0 (default after reset) 
  *            @arg GPIO_AF_MCO: Connect MCO pin (MCO1 and MCO2) to AF0 (default after reset) 
  *            @arg GPIO_AF_TAMPER: Connect TAMPER pins (TAMPER_1 and TAMPER_2) to AF0 (default after reset) 
  *            @arg GPIO_AF_SWJ: Connect SWJ pins (SWD and JTAG)to AF0 (default after reset) 
  *            @arg GPIO_AF_TRACE: Connect TRACE pins to AF0 (default after reset)
  *            @arg GPIO_AF_TIM1: Connect TIM1 pins to AF1
  *            @arg GPIO_AF_TIM2: Connect TIM2 pins to AF1
  *            @arg GPIO_AF_TIM3: Connect TIM3 pins to AF2
  *            @arg GPIO_AF_TIM4: Connect TIM4 pins to AF2
  *            @arg GPIO_AF_TIM5: Connect TIM5 pins to AF2
  *            @arg GPIO_AF_TIM8: Connect TIM8 pins to AF3
  *            @arg GPIO_AF_TIM9: Connect TIM9 pins to AF3
  *            @arg GPIO_AF_TIM10: Connect TIM10 pins to AF3
  *            @arg GPIO_AF_TIM11: Connect TIM11 pins to AF3
  *            @arg GPIO_AF_I2C1: Connect I2C1 pins to AF4
  *            @arg GPIO_AF_I2C2: Connect I2C2 pins to AF4
  *            @arg GPIO_AF_I2C3: Connect I2C3 pins to AF4
  *            @arg GPIO_AF_SPI1: Connect SPI1 pins to AF5
  *            @arg GPIO_AF_SPI2: Connect SPI2/I2S2 pins to AF5
  *            @arg GPIO_AF_SPI4: Connect SPI4 pins to AF5 
  *            @arg GPIO_AF_SPI5: Connect SPI5 pins to AF5 
  *            @arg GPIO_AF_SPI6: Connect SPI6 pins to AF5
  *            @arg GPIO_AF_SAI1: Connect SAI1 pins to AF6 for STM32F42xxx/43xxx devices.       
  *            @arg GPIO_AF_SPI3: Connect SPI3/I2S3 pins to AF6
  *            @arg GPIO_AF_I2S3ext: Connect I2S3ext pins to AF7
  *            @arg GPIO_AF_USART1: Connect USART1 pins to AF7
  *            @arg GPIO_AF_USART2: Connect USART2 pins to AF7
  *            @arg GPIO_AF_USART3: Connect USART3 pins to AF7
  *            @arg GPIO_AF_UART4: Connect UART4 pins to AF8
  *            @arg GPIO_AF_UART5: Connect UART5 pins to AF8
  *            @arg GPIO_AF_USART6: Connect USART6 pins to AF8
  *            @arg GPIO_AF_UART7: Connect UART7 pins to AF8
  *            @arg GPIO_AF_UART8: Connect UART8 pins to AF8
  *            @arg GPIO_AF_CAN1: Connect CAN1 pins to AF9
  *            @arg GPIO_AF_CAN2: Connect CAN2 pins to AF9
  *            @arg GPIO_AF_TIM12: Connect TIM12 pins to AF9
  *            @arg GPIO_AF_TIM13: Connect TIM13 pins to AF9
  *            @arg GPIO_AF_TIM14: Connect TIM14 pins to AF9
  *            @arg GPIO_AF_OTG_FS: Connect OTG_FS pins to AF10
  *            @arg GPIO_AF_OTG_HS: Connect OTG_HS pins to AF10
  *            @arg GPIO_AF_ETH: Connect ETHERNET pins to AF11
  *            @arg GPIO_AF_FSMC: Connect FSMC pins to AF12 
  *            @arg GPIO_AF_FMC: Connect FMC pins to AF12 for STM32F42xxx/43xxx devices.   
  *            @arg GPIO_AF_OTG_HS_FS: Connect OTG HS (configured in FS) pins to AF12
  *            @arg GPIO_AF_SDIO: Connect SDIO pins to AF12
  *            @arg GPIO_AF_DCMI: Connect DCMI pins to AF13
  *            @arg GPIO_AF_LTDC: Connect LTDC pins to AF14 for STM32F429xx/439xx devices. 
  *            @arg GPIO_AF_EVENTOUT: Connect EVENTOUT pins to AF15
  * @retval None
  */
/**
  * @brief  配置GPIO引脚复用功能映射
  * 
  * 本函数用于设置指定GPIO引脚连接到特定的复用功能(AF)控制器。
  * 通过操作GPIO的AFR（Alternate Function Register）寄存器，
  * 将指定引脚源映射到指定的复用功能编号（0-15）。
  *
  * @param GPIOx         GPIO端口基地址（GPIOA-GPIOK）
  * @param GPIO_PinSource 引脚编号（GPIO_PinSource0-GPIO_PinSource15）
  * @param GPIO_AF       复用功能编号（GPIO_AF_0-GPIO_AF15）
  * @retval None
  */
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
{
  /* 定义临时变量用于位操作 */
  uint32_t temp = 0x00;    // 用于存储AF配置值的临时变量
  uint32_t temp_2 = 0x00;  // 用于组合新旧寄存器值的临时变量
  
  /* 参数有效性检查（保留原始注释） */
  /* Check the parameters */
  // 验证GPIO外设是否有效
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  // 验证引脚源编号是否在0-15范围内
  assert_param(IS_GPIO_PIN_SOURCE(GPIO_PinSource));
  // 验证AF编号是否在0-15范围内
  assert_param(IS_GPIO_AF(GPIO_AF));
  
  /* 计算AF值在寄存器中的偏移位置 */
  // 将AF值左移到对应的4bit位置（每个引脚占用4bit）
  // GPIO_PinSource & 0x07 计算引脚在寄存器中的位置（0-7对应AFRL，8-15对应AFRH）
  temp = ((uint32_t)(GPIO_AF) << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4)) ;
  
  /* 清除目标寄存器位的旧值 */
  // 选择AFRL或AFRH寄存器（GPIO_PinSource >> 3：0-7=0，8-15=1）
  // 生成清除掩码（0xF左移对应4bit的位置）
  GPIOx->AFR[GPIO_PinSource >> 0x03] &= ~((uint32_t)0xF << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4)) ;
  
  /* 组合新旧寄存器值 */
  // 获取当前寄存器值并与新AF值进行或运算
  temp_2 = GPIOx->AFR[GPIO_PinSource >> 0x03] | temp;
  
  /* 写回配置到目标寄存器 */
  // 将组合后的值写入对应的AFR寄存器
  GPIOx->AFR[GPIO_PinSource >> 0x03] = temp_2;
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

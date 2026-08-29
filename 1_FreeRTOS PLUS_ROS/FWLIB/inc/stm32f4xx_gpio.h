/**
  ******************************************************************************
  * @file    stm32f4xx_gpio.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the GPIO firmware
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
#ifndef __STM32F4xx_GPIO_H
#define __STM32F4xx_GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup GPIO
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/**
  * @brief GPIO配置参数及外设验证宏定义
  * @功能说明：
  * 1. 定义STM32全系列GPIO外设标识
  * 2. 封装GPIO模式、输出类型、速度等配置参数
  * 3. 提供参数合法性校验规则
  * @应用场景：
  * - 引脚模式初始化配置
  * - 外设复用功能设置
  * - 高低速IO场景选择
  */

/*----------------------- GPIO外设验证宏 -----------------------*/
/* 验证GPIO外设地址合法性（支持A-K共11个GPIO组） */
#define IS_GPIO_ALL_PERIPH(PERIPH) (((PERIPH) == GPIOA) || \
                                    ((PERIPH) == GPIOB) || \
                                    ((PERIPH) == GPIOC) || \
                                    ((PERIPH) == GPIOD) || \
                                    ((PERIPH) == GPIOE) || \
                                    ((PERIPH) == GPIOF) || \
                                    ((PERIPH) == GPIOG) || \
                                    ((PERIPH) == GPIOH) || \
                                    ((PERIPH) == GPIOI) || \
                                    ((PERIPH) == GPIOJ) || \
                                    ((PERIPH) == GPIOK))

/*----------------------- GPIO模式配置枚举 ----------------------*/
/** @defgroup GPIO模式枚举 */
typedef enum { 
  GPIO_Mode_IN   = 0x00, /* 输入模式（浮空/上拉/下拉） */ 
  GPIO_Mode_OUT  = 0x01, /* 通用输出模式（推挽/开漏） */
  GPIO_Mode_AF   = 0x02, /* 复用功能模式（SPI/I2C等外设） */
  GPIO_Mode_AN   = 0x03  /* 模拟模式（ADC/DAC等） */
} GPIOMode_TypeDef;

/* 模式参数校验（必须为四种模式之一） */
#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_Mode_IN)  || \
                            ((MODE) == GPIO_Mode_OUT) || \
                            ((MODE) == GPIO_Mode_AF)  || \
                            ((MODE) == GPIO_Mode_AN))

/*---------------------- GPIO输出类型枚举 ----------------------*/ 
/** @defgroup GPIO输出类型枚举 */
typedef enum {
  GPIO_OType_PP = 0x00,  /* 推挽输出（可输出高低电平） */
  GPIO_OType_OD = 0x01   /* 开漏输出（需外部上拉） */
} GPIOOType_TypeDef;

/* 输出类型校验（PP/OD二选一） */
#define IS_GPIO_OTYPE(OTYPE) (((OTYPE) == GPIO_OType_PP) || \
                              ((OTYPE) == GPIO_OType_OD))

/*---------------------- GPIO速度等级枚举 ----------------------*/
/** @defgroup GPIO速度枚举 */
typedef enum {
  GPIO_Low_Speed    = 0x00, /* 低速（2MHz，低功耗） */
  GPIO_Medium_Speed = 0x01, /* 中速（10MHz） */
  GPIO_Fast_Speed   = 0x02, /* 快速（50MHz） */
  GPIO_High_Speed   = 0x03  /* 高速（100MHz） */
} GPIOSpeed_TypeDef;

/* 兼容旧版速度定义（V3.0之前库版本） */
#define GPIO_Speed_2MHz    GPIO_Low_Speed    /* 低速模式别名 */
#define GPIO_Speed_25MHz   GPIO_Medium_Speed /* 中速模式别名 */
#define GPIO_Speed_50MHz   GPIO_Fast_Speed   /* 快速模式别名 */
#define GPIO_Speed_100MHz  GPIO_High_Speed   /* 高速模式别名 */

/* 速度参数校验（四种速度等级） */
#define IS_GPIO_SPEED(SPEED) (((SPEED) == GPIO_Low_Speed)    || \
                              ((SPEED) == GPIO_Medium_Speed) || \
                              ((SPEED) == GPIO_Fast_Speed)   || \
                              ((SPEED) == GPIO_High_Speed))

/**
  * @brief GPIO上拉/下拉配置及初始化结构体定义
  * @功能说明：
  * 1. 定义GPIO引脚上下拉电阻配置选项
  * 2. 封装完整的GPIO初始化参数结构
  * @核心配置：
  * - 输入模式下的上拉/下拉电阻配置
  * - 引脚操作状态设置
  * - 多参数组合初始化结构
  */

/*------------------ GPIO上拉/下拉配置枚举 -----------------*/
/** @defgroup 上拉下拉配置 */
typedef enum { 
  GPIO_PuPd_NOPULL = 0x00, /* 浮空输入（无上拉/下拉电阻） */
  GPIO_PuPd_UP     = 0x01,  /* 启用内部上拉电阻（默认高电平） */
  GPIO_PuPd_DOWN   = 0x02   /* 启用内部下拉电阻（默认低电平） */
} GPIOPuPd_TypeDef;

/* 上拉配置合法性校验（三选一） */
#define IS_GPIO_PUPD(PUPD) (((PUPD) == GPIO_PuPd_NOPULL) || \
                            ((PUPD) == GPIO_PuPd_UP)    || \
                            ((PUPD) == GPIO_PuPd_DOWN))

/*------------------ GPIO位操作枚举 ----------------------*/
/** @defgroup 引脚状态操作 */
typedef enum {
  Bit_RESET = 0,  /* 清除引脚电平（输出低电平） */
  Bit_SET         /* 设置引脚电平（输出高电平） */
} BitAction;

/* 位操作参数校验（二选一） */
#define IS_GPIO_BIT_ACTION(ACTION) (((ACTION) == Bit_RESET) || \
                                    ((ACTION) == Bit_SET))

/*------------------ GPIO初始化结构体 --------------------*/
/** 
  * @brief GPIO初始化配置结构体
  * @详细说明：封装GPIO引脚所有配置参数
  */
typedef struct {
  uint32_t GPIO_Pin;           /* 目标引脚（可多选组合） 
                                 * 示例：GPIO_Pin_0 | GPIO_Pin_1 
                                 * 取值范围：0x0001-0xFFFF */

  GPIOMode_TypeDef GPIO_Mode;   /* 工作模式选择
                                 * 输入/输出/复用/模拟模式
                                 * @注意：模拟模式下其他参数无效 */

  GPIOSpeed_TypeDef GPIO_Speed; /* 输出驱动速度
                                 * 影响信号边沿速率和EMI
                                 * 输入模式时此参数无效 */

  GPIOOType_TypeDef GPIO_OType; /* 输出类型配置
                                 * 推挽/开漏选择
                                 * 仅在输出/复用模式有效 */

  GPIOPuPd_TypeDef GPIO_PuPd;  /* 上下拉电阻配置
                                 * 输入模式必须配置
                                 * 输出模式建议NOPULL */
} GPIO_InitTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup GPIO_Exported_Constants
  * @{
  */ 
/**
  * @brief GPIO引脚选择宏定义
  * @功能说明：
  * 1. 定义标准GPIO引脚位选择标识
  * 2. 提供多引脚组合配置支持
  * 3. 包含引脚参数合法性校验规则
  * @应用场景：
  * - 单引脚或多引脚配置
  * - 批量操作GPIO端口
  */

/** @defgroup GPIO_pins_define GPIO引脚选择 */
#define GPIO_Pin_0   ((uint16_t)0x0001)  /* 选择引脚0（Port位0） */
#define GPIO_Pin_1   ((uint16_t)0x0002)  /* 选择引脚1（Port位1） */
#define GPIO_Pin_2   ((uint16_t)0x0004)  /* 选择引脚2（Port位2） */
#define GPIO_Pin_3   ((uint16_t)0x0008)  /* 选择引脚3（Port位3） */
#define GPIO_Pin_4   ((uint16_t)0x0010)  /* 选择引脚4（Port位4） */
#define GPIO_Pin_5   ((uint16_t)0x0020)  /* 选择引脚5（Port位5） */
#define GPIO_Pin_6   ((uint16_t)0x0040)  /* 选择引脚6（Port位6） */
#define GPIO_Pin_7   ((uint16_t)0x0080)  /* 选择引脚7（Port位7） */
#define GPIO_Pin_8   ((uint16_t)0x0100)  /* 选择引脚8（Port位8） */
#define GPIO_Pin_9   ((uint16_t)0x0200)  /* 选择引脚9（Port位9） */
#define GPIO_Pin_10  ((uint16_t)0x0400)  /* 选择引脚10（Port位10） */
#define GPIO_Pin_11  ((uint16_t)0x0800)  /* 选择引脚11（Port位11） */
#define GPIO_Pin_12  ((uint16_t)0x1000)  /* 选择引脚12（Port位12） */
#define GPIO_Pin_13  ((uint16_t)0x2000)  /* 选择引脚13（Port位13） */
#define GPIO_Pin_14  ((uint16_t)0x4000)  /* 选择引脚14（Port位14） */
#define GPIO_Pin_15  ((uint16_t)0x8000)  /* 选择引脚15（Port位15） */
#define GPIO_Pin_All ((uint16_t)0xFFFF)  /* 选择所有引脚（0-15位全1） */

/*--------------------- 引脚校验宏 ---------------------*/
#define GPIO_PIN_MASK ((uint32_t)0x0000FFFF) /* 有效引脚位掩码（低16位） */

/* 通用引脚参数校验（允许多引脚组合） */
#define IS_GPIO_PIN(PIN) (((PIN) & GPIO_PIN_MASK) != 0x00)
/**
  * @brief GPIO单引脚有效性校验宏
  * @功能说明：
  * 严格验证参数是否为单个有效GPIO引脚标识
  * @特性：
  * - 仅接受GPIO_Pin_0至GPIO_Pin_15的单个参数
  * - 禁止多引脚组合使用
  * @应用场景：
  * - 读取/写入单个引脚电平状态
  * - 配置外部中断线路
  * - 操作需要精确引脚定位的硬件功能
  */
#define IS_GET_GPIO_PIN(PIN) (((PIN) == GPIO_Pin_0)  ||   /* 严格匹配单个引脚定义 */\
                             ((PIN) == GPIO_Pin_1)  || \
                             ((PIN) == GPIO_Pin_2)  || \
                             ((PIN) == GPIO_Pin_3)  || \
                             ((PIN) == GPIO_Pin_4)  || \
                             ((PIN) == GPIO_Pin_5)  || \
                             ((PIN) == GPIO_Pin_6)  || \
                             ((PIN) == GPIO_Pin_7)  || \
                             ((PIN) == GPIO_Pin_8)  || \
                             ((PIN) == GPIO_Pin_9)  || \
                             ((PIN) == GPIO_Pin_10) || \
                             ((PIN) == GPIO_Pin_11) || \
                             ((PIN) == GPIO_Pin_12) || \
                             ((PIN) == GPIO_Pin_13) || \
                             ((PIN) == GPIO_Pin_14) || \
                             ((PIN) == GPIO_Pin_15))

/**
  * @}
  */ 

/**
  * @brief GPIO引脚源编码定义
  * @功能说明：
  * 定义引脚编号编码系统，用于复用功能映射配置
  * @核心应用：
  * - 外设复用功能（Alternate Function）配置
  * - 事件触发引脚绑定
  * - 引脚重映射操作
  */

/** @defgroup GPIO_Pin_sources 引脚源编码 */
#define GPIO_PinSource0  ((uint8_t)0x00) /* 引脚号0的编码（如PA0/PB0等） */
#define GPIO_PinSource1  ((uint8_t)0x01) /* 引脚号1的编码 */
#define GPIO_PinSource2  ((uint8_t)0x02) /* 引脚号2的编码 */
#define GPIO_PinSource3  ((uint8_t)0x03) /* 引脚号3的编码 */
#define GPIO_PinSource4  ((uint8_t)0x04) /* 引脚号4的编码 */
#define GPIO_PinSource5  ((uint8_t)0x05) /* 引脚号5的编码 */
#define GPIO_PinSource6  ((uint8_t)0x06) /* 引脚号6的编码 */
#define GPIO_PinSource7  ((uint8_t)0x07) /* 引脚号7的编码 */
#define GPIO_PinSource8  ((uint8_t)0x08) /* 引脚号8的编码 */
#define GPIO_PinSource9  ((uint8_t)0x09) /* 引脚号9的编码 */
#define GPIO_PinSource10 ((uint8_t)0x0A) /* 引脚号10的编码（十六进制0x0A）*/
#define GPIO_PinSource11 ((uint8_t)0x0B) /* 引脚号11的编码 */
#define GPIO_PinSource12 ((uint8_t)0x0C) /* 引脚号12的编码 */
#define GPIO_PinSource13 ((uint8_t)0x0D) /* 引脚号13的编码 */
#define GPIO_PinSource14 ((uint8_t)0x0E) /* 引脚号14的编码 */
#define GPIO_PinSource15 ((uint8_t)0x0F) /* 引脚号15的编码 */

/**
  * @brief  GPIO引脚源有效性验证宏
  * @param  PINSOURCE 待验证的GPIO引脚源编号
  * @retval 布尔类型，有效返回1（True），无效返回0（False）
  * @note   该宏用于系统初始化时验证GPIO配置参数合法性
  *         适用于所有GPIO模式配置函数的参数预检查
  *         有效范围：GPIO_PinSource0 ~ GPIO_PinSource15
  */
#define IS_GPIO_PIN_SOURCE(PINSOURCE) (((PINSOURCE) == GPIO_PinSource0) ||   /* 验证GPIO引脚0的AF配置 */ \
                                       ((PINSOURCE) == GPIO_PinSource1) ||   /* 验证GPIO引脚1的AF配置 */ \
                                       ((PINSOURCE) == GPIO_PinSource2) ||   /* 检查EXTI线2对应的引脚 */ \
                                       ((PINSOURCE) == GPIO_PinSource3) ||   /* PWM通道3的引脚源验证 */ \
                                       ((PINSOURCE) == GPIO_PinSource4) ||   /* SPI片选引脚有效性检查 */ \
                                       ((PINSOURCE) == GPIO_PinSource5) ||   /* I2S时钟引脚源确认 */ \
                                       ((PINSOURCE) == GPIO_PinSource6) ||   /* ADC通道6输入引脚验证 */ \
                                       ((PINSOURCE) == GPIO_PinSource7) ||   /* 串口CTS流控引脚检查 */ \
                                       ((PINSOURCE) == GPIO_PinSource8) ||   /* 定时器刹车输入引脚源 */ \
                                       ((PINSOURCE) == GPIO_PinSource9) ||   /* CAN总线接收引脚验证 */ \
                                       ((PINSOURCE) == GPIO_PinSource10) ||  /* 以太网MDIO时钟引脚检查 */ \
                                       ((PINSOURCE) == GPIO_PinSource11) ||  /* SDIO数据线3引脚确认 */ \
                                       ((PINSOURCE) == GPIO_PinSource12) ||  /* 引脚号12的编码 */ \
                                       ((PINSOURCE) == GPIO_PinSource13) ||  /* 引脚号13的编码 */ \
                                       ((PINSOURCE) == GPIO_PinSource14) ||  /* 引脚号14的编码 */ \
                                       ((PINSOURCE) == GPIO_PinSource15))     /* 引脚号15的编码 */

/**
  * @}
  */ 
/**
  * @brief  GPIO复用功能(AF)配置宏定义
  * @note   本组宏定义用于STM32F4系列芯片的GPIO复用功能配置
  *         每个AF编号对应不同外设功能，具体引脚映射关系需参考芯片数据手册
  *         适用于GPIOx_AFR寄存器的配置操作
  * @warning 部分AF编号在不同芯片型号中功能不同，使用时需注意设备兼容性
  */
/** @defgroup GPIO_Alternat_function_selection_define 
  * @{
  */ 

/** 
  * @brief   AF 0选择（系统功能组）
  * @note    AF0通常用于芯片核心功能配置
  */ 
#define GPIO_AF_RTC_50Hz      ((uint8_t)0x00)  /* RTC 50Hz时钟输出功能，用于唤醒定时 */ 
#define GPIO_AF_MCO           ((uint8_t)0x00)  /* 主时钟输出功能，可输出SYSCLK/HSI/HSE等系统时钟 */ 
#define GPIO_AF_TAMPER        ((uint8_t)0x00)  /* 侵入检测功能，用于安全监控应用 */ 
#define GPIO_AF_SWJ           ((uint8_t)0x00)  /* SWD/JTAG调试接口，包含SWCLK/SWDIO/JTAG引脚 */ 
#define GPIO_AF_TRACE         ((uint8_t)0x00)  /* 跟踪调试功能，用于实时指令跟踪 */ 

/** 
  * @brief   AF 1选择（高级定时器组）
  * @note    用于配置TIM1/TIM2等高级控制定时器
  */ 
#define GPIO_AF_TIM1          ((uint8_t)0x01)  /* TIM1通道/PWM输出，支持互补输出和刹车功能 */ 
#define GPIO_AF_TIM2          ((uint8_t)0x01)  /* TIM2通道/编码器接口，支持32位计数 */ 

/** 
  * @brief   AF 2选择（通用定时器组）
  * @note    用于通用定时器功能配置
  */ 
#define GPIO_AF_TIM3          ((uint8_t)0x02)  /* TIM3通道/PWM输出，支持DMA请求 */ 
#define GPIO_AF_TIM4          ((uint8_t)0x02)  /* TIM4通道输出，适用于电机控制 */ 
#define GPIO_AF_TIM5          ((uint8_t)0x02)  /* TIM5通道输入捕获，支持32位计数 */ 

/** 
  * @brief   AF 3选择（扩展定时器组）
  * @note    用于TIM8/TIM9等定时器功能配置
  */ 
#define GPIO_AF_TIM8          ((uint8_t)0x03)  /* TIM8高级定时器，带死区插入功能 */ 
#define GPIO_AF_TIM9          ((uint8_t)0x03)  /* TIM9基本定时器，支持从模式 */ 
#define GPIO_AF_TIM10         ((uint8_t)0x03)  /* TIM10 PWM生成，独立看门狗时钟源 */ 
#define GPIO_AF_TIM11         ((uint8_t)0x03)  /* TIM11 PWM输出，用于蜂鸣器控制 */ 

/** 
  * @brief   AF 4选择（I2C接口组）
  * @note    用于I2C总线接口配置
  */ 
#define GPIO_AF_I2C1          ((uint8_t)0x04)  /* I2C1标准模式(100kHz)/快速模式(400kHz) */ 
#define GPIO_AF_I2C2          ((uint8_t)0x04)  /* I2C2接口，支持SMBus协议 */ 
#define GPIO_AF_I2C3          ((uint8_t)0x04)  /* I2C3接口，支持多主模式 */ 

/** 
  * @brief   AF 5选择（SPI接口组）
  * @note    用于SPI/I2S通信接口配置
  */ 
#define GPIO_AF_SPI1          ((uint8_t)0x05)  /* SPI1主从接口，支持全双工通信 */ 
#define GPIO_AF_SPI2          ((uint8_t)0x05)  /* SPI2接口，支持NSS硬件管理 */ 
#define GPIO_AF5_SPI3         ((uint8_t)0x05)  /* SPI3接口（仅STM32F411xE可用） */ 
#define GPIO_AF_SPI4          ((uint8_t)0x05)  /* SPI4扩展接口，支持最高45MHz */ 
#define GPIO_AF_SPI5          ((uint8_t)0x05)  /* SPI5接口，支持双线全双工模式 */ 
#define GPIO_AF_SPI6          ((uint8_t)0x05)  /* SPI6高速接口，适用于存储器扩展 */ 

/** 
  * @brief   AF 6选择（扩展接口组）
  * @note    特殊功能接口配置
  */ 
#define GPIO_AF_SPI3          ((uint8_t)0x06)  /* SPI3标准接口配置 */ 
#define GPIO_AF6_SPI2         ((uint8_t)0x06)  /* SPI2复用配置（仅STM32F411xE） */ 
#define GPIO_AF6_SPI4         ((uint8_t)0x06)  /* SPI4特殊模式配置 */ 
#define GPIO_AF6_SPI5         ((uint8_t)0x06)  /* SPI5扩展模式配置 */ 
#define GPIO_AF_SAI1          ((uint8_t)0x06)  /* 音频接口配置，支持I2S/AC97 */ 

/** 
  * @brief   AF 7选择（USART接口组）
  * @note    串行通信接口配置
  */ 
#define GPIO_AF_USART1         ((uint8_t)0x07)  /* USART1全功能串口，支持IrDA */ 
#define GPIO_AF_USART2         ((uint8_t)0x07)  /* USART2智能卡模式接口 */ 
#define GPIO_AF_USART3         ((uint8_t)0x07)  /* USART3 LIN主模式接口 */ 
#define GPIO_AF7_SPI3          ((uint8_t)0x07)  /* SPI3扩展模式（I2S外部时钟） */ 

/** 
  * @brief   AF 8选择（扩展串口组）
  * @note    低速串行通信接口
  */ 
#define GPIO_AF_UART4         ((uint8_t)0x08)  /* UART4标准串口，无硬件流控 */ 
#define GPIO_AF_UART5         ((uint8_t)0x08)  /* UART5单线半双工模式 */ 
#define GPIO_AF_USART6        ((uint8_t)0x08)  /* USART6高速模式（10Mbps） */ 
#define GPIO_AF_UART7         ((uint8_t)0x08)  /* UART7多处理器通信接口 */ 
#define GPIO_AF_UART8         ((uint8_t)0x08)  /* UART8 LIN从模式接口 */ 

/** 
  * @brief   AF 9选择（车载网络组）
  * @note    汽车电子相关接口
  */ 
#define GPIO_AF_CAN1          ((uint8_t)0x09)  /* CAN1总线接口，支持2.0B协议 */ 
#define GPIO_AF_CAN2          ((uint8_t)0x09)  /* CAN2冗余总线接口 */ 
#define GPIO_AF_TIM12         ((uint8_t)0x09)  /* TIM12霍尔传感器接口 */ 
#define GPIO_AF_TIM13         ((uint8_t)0x09)  /* TIM13单脉冲模式定时器 */ 
#define GPIO_AF_TIM14         ((uint8_t)0x09)  /* TIM14从模式定时器 */ 
#define GPIO_AF9_I2C2         ((uint8_t)0x09)  /* I2C2特殊模式（F401xx/F411xE专用） */ 
#define GPIO_AF9_I2C3         ((uint8_t)0x09)  /* I2C3快速模式+（F401xx/F411xE专用） */ 

/** 
  * @brief   AF 10选择（USB接口组）
  * @note    USB外设接口配置
  */ 
#define GPIO_AF_OTG_FS         ((uint8_t)0xA)  /* USB OTG全速接口，集成PHY */ 
#define GPIO_AF_OTG_HS         ((uint8_t)0xA)  /* USB OTG高速接口，需外接PHY */ 
/**
  * @brief  GPIO复用功能(AF)扩展配置宏定义
  * @note   本组宏定义涵盖STM32F4系列高级外设接口配置
  *         涉及存储控制器、图像采集、显示接口等特殊功能
  * @warning 不同芯片型号的AF功能存在差异，需根据具体型号选择
  */

/** 
  * @brief   AF 11选择（网络接口组）
  * @note    工业通信接口配置
  */ 
#define GPIO_AF_ETH             ((uint8_t)0x0B)  /* 以太网MAC接口，支持10/100Mbps RMII模式 */

/** 
  * @brief   AF 12选择（存储接口组）
  * @note    外部存储器扩展接口配置
  */
#if defined (STM32F40_41xxx)
#define GPIO_AF_FSMC             ((uint8_t)0xC)  /* 灵活静态存储器控制器（NOR/SRAM），支持16位数据总线 */
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define GPIO_AF_FMC              ((uint8_t)0xC)  /* 高级存储器控制器（支持SDRAM），带时钟域控制 */
#endif /* STM32F427_437xx || STM32F429_439xx */

#define GPIO_AF_OTG_HS_FS        ((uint8_t)0xC)  /* USB OTG高速全速混合模式，内置PHY控制 */
#define GPIO_AF_SDIO             ((uint8_t)0xC)  /* SD/MMC存储卡接口，支持4位数据模式 */

/** 
  * @brief   AF 13选择（图像采集组）
  * @note    数字摄像头接口配置
  */
#define GPIO_AF_DCMI          ((uint8_t)0x0D)  /* 数字摄像头接口，支持YUV/RGB格式，最大48MHz采样 */

/** 
  * @brief   AF 14选择（显示接口组）
  * @note    图形显示控制器配置
  */
#define GPIO_AF_LTDC          ((uint8_t)0x0E)  /* LCD-TFT控制器，支持RGB888格式，最高WXGA分辨率 */

/** 
  * @brief   AF 15选择（系统事件组）
  * @note    芯片级事件控制配置
  */
#define GPIO_AF_EVENTOUT      ((uint8_t)0x0F)  /* 事件触发输出，用于跨外设硬件同步 */


#if defined (STM32F40_41xxx)
#define IS_GPIO_AF(AF)   (((AF) == GPIO_AF_RTC_50Hz)  || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_MCO)       || ((AF) == GPIO_AF_TAMPER)    || \
                          ((AF) == GPIO_AF_SWJ)       || ((AF) == GPIO_AF_TRACE)     || \
                          ((AF) == GPIO_AF_TIM1)      || ((AF) == GPIO_AF_TIM2)      || \
                          ((AF) == GPIO_AF_TIM3)      || ((AF) == GPIO_AF_TIM4)      || \
                          ((AF) == GPIO_AF_TIM5)      || ((AF) == GPIO_AF_TIM8)      || \
                          ((AF) == GPIO_AF_I2C1)      || ((AF) == GPIO_AF_I2C2)      || \
                          ((AF) == GPIO_AF_I2C3)      || ((AF) == GPIO_AF_SPI1)      || \
                          ((AF) == GPIO_AF_SPI2)      || ((AF) == GPIO_AF_TIM13)     || \
                          ((AF) == GPIO_AF_SPI3)      || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_USART1)    || ((AF) == GPIO_AF_USART2)    || \
                          ((AF) == GPIO_AF_USART3)    || ((AF) == GPIO_AF_UART4)     || \
                          ((AF) == GPIO_AF_UART5)     || ((AF) == GPIO_AF_USART6)    || \
                          ((AF) == GPIO_AF_CAN1)      || ((AF) == GPIO_AF_CAN2)      || \
                          ((AF) == GPIO_AF_OTG_FS)    || ((AF) == GPIO_AF_OTG_HS)    || \
                          ((AF) == GPIO_AF_ETH)       || ((AF) == GPIO_AF_OTG_HS_FS) || \
                          ((AF) == GPIO_AF_SDIO)      || ((AF) == GPIO_AF_DCMI)      || \
                          ((AF) == GPIO_AF_EVENTOUT)  || ((AF) == GPIO_AF_FSMC))
#endif /* STM32F40_41xxx */

#if defined (STM32F401xx)
#define IS_GPIO_AF(AF)   (((AF) == GPIO_AF_RTC_50Hz)  || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_MCO)       || ((AF) == GPIO_AF_TAMPER)    || \
                          ((AF) == GPIO_AF_SWJ)       || ((AF) == GPIO_AF_TRACE)     || \
                          ((AF) == GPIO_AF_TIM1)      || ((AF) == GPIO_AF_TIM2)      || \
                          ((AF) == GPIO_AF_TIM3)      || ((AF) == GPIO_AF_TIM4)      || \
                          ((AF) == GPIO_AF_TIM5)      || ((AF) == GPIO_AF_TIM8)      || \
                          ((AF) == GPIO_AF_I2C1)      || ((AF) == GPIO_AF_I2C2)      || \
                          ((AF) == GPIO_AF_I2C3)      || ((AF) == GPIO_AF_SPI1)      || \
                          ((AF) == GPIO_AF_SPI2)      || ((AF) == GPIO_AF_TIM13)     || \
                          ((AF) == GPIO_AF_SPI3)      || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_USART1)    || ((AF) == GPIO_AF_USART2)    || \
                          ((AF) == GPIO_AF_SDIO)      || ((AF) == GPIO_AF_USART6)    || \
                          ((AF) == GPIO_AF_OTG_FS)    || ((AF) == GPIO_AF_OTG_HS)    || \
                          ((AF) == GPIO_AF_EVENTOUT)  || ((AF) == GPIO_AF_SPI4))
#endif /* STM32F401xx */

#if defined (STM32F411xE)
#define IS_GPIO_AF(AF)   (((AF) < 16) && ((AF) != 11) && ((AF) != 13) && ((AF) != 14))
#endif /* STM32F411xE */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define IS_GPIO_AF(AF)   (((AF) == GPIO_AF_RTC_50Hz)  || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_MCO)       || ((AF) == GPIO_AF_TAMPER)    || \
                          ((AF) == GPIO_AF_SWJ)       || ((AF) == GPIO_AF_TRACE)     || \
                          ((AF) == GPIO_AF_TIM1)      || ((AF) == GPIO_AF_TIM2)      || \
                          ((AF) == GPIO_AF_TIM3)      || ((AF) == GPIO_AF_TIM4)      || \
                          ((AF) == GPIO_AF_TIM5)      || ((AF) == GPIO_AF_TIM8)      || \
                          ((AF) == GPIO_AF_I2C1)      || ((AF) == GPIO_AF_I2C2)      || \
                          ((AF) == GPIO_AF_I2C3)      || ((AF) == GPIO_AF_SPI1)      || \
                          ((AF) == GPIO_AF_SPI2)      || ((AF) == GPIO_AF_TIM13)     || \
                          ((AF) == GPIO_AF_SPI3)      || ((AF) == GPIO_AF_TIM14)     || \
                          ((AF) == GPIO_AF_USART1)    || ((AF) == GPIO_AF_USART2)    || \
                          ((AF) == GPIO_AF_USART3)    || ((AF) == GPIO_AF_UART4)     || \
                          ((AF) == GPIO_AF_UART5)     || ((AF) == GPIO_AF_USART6)    || \
                          ((AF) == GPIO_AF_CAN1)      || ((AF) == GPIO_AF_CAN2)      || \
                          ((AF) == GPIO_AF_OTG_FS)    || ((AF) == GPIO_AF_OTG_HS)    || \
                          ((AF) == GPIO_AF_ETH)       || ((AF) == GPIO_AF_OTG_HS_FS) || \
                          ((AF) == GPIO_AF_SDIO)      || ((AF) == GPIO_AF_DCMI)      || \
                          ((AF) == GPIO_AF_EVENTOUT)  || ((AF) == GPIO_AF_SPI4)      || \
                          ((AF) == GPIO_AF_SPI5)      || ((AF) == GPIO_AF_SPI6)      || \
                          ((AF) == GPIO_AF_UART7)     || ((AF) == GPIO_AF_UART8)     || \
                          ((AF) == GPIO_AF_FMC)       ||  ((AF) == GPIO_AF_SAI1)     || \
                          ((AF) == GPIO_AF_LTDC))
#endif /* STM32F427_437xx ||  STM32F429_439xx */
                          
/**
  * @}
  */ 
/**
  * @brief  GPIO模块遗留定义及功能接口
  * @note   本部分包含兼容性宏定义和GPIO操作函数原型
  * @warning 带_Legacy标记的宏为历史版本兼容定义，建议新项目使用新命名
  */

/** @defgroup GPIO_Legacy 历史兼容定义
  * @{
  */
    
#define GPIO_Mode_AIN           GPIO_Mode_AN  /* 模拟输入模式兼容定义（V1.0旧版命名） */

#define GPIO_AF_OTG1_FS         GPIO_AF_OTG_FS    /* USB OTG1全速模式兼容定义（适用于F4系列早期封装） */
#define GPIO_AF_OTG2_HS         GPIO_AF_OTG_HS    /* USB OTG2高速模式兼容定义（双端口设备专用） */
#define GPIO_AF_OTG2_FS         GPIO_AF_OTG_HS_FS /* OTG2全速模式特殊配置（混合PHY设计） */

/**
  * @}
  */

/*------------------------ 导出函数原型声明 --------------------------------*/

/**
  * @brief  复位GPIO配置到默认状态
  * @param  GPIOx 目标GPIO端口（GPIOA~GPIOK）
  * @retval 无
  * @note   会清除端口所有配置寄存器，中断使能将被关闭
  */
void GPIO_DeInit(GPIO_TypeDef* GPIOx);

/*---------------- 初始化配置函数组 ----------------*/

/**
  * @brief  GPIO端口初始化配置
  * @param  GPIOx 目标端口
  * @param  GPIO_InitStruct 包含模式/速度/上下拉的配置结构体指针
  * @retval 无
  * @warning 配置前必须使能对应GPIO时钟（RCC_AHB1ENR）
  */
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);

/**
  * @brief  初始化结构体默认值加载
  * @param  GPIO_InitStruct 待初始化的结构体指针
  * @retval 无
  * @note   默认配置：模拟输入模式/低速/无上拉下拉
  */
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);

/**
  * @brief  GPIO引脚配置锁定
  * @param  GPIOx 目标端口
  * @param  GPIO_Pin 引脚掩码（GPIO_Pin_0~15组合）
  * @retval 无
  * @warning 锁定后配置寄存器不可修改，需系统复位解除
  */
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/*---------------- 数据操作函数组 ----------------*/

uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);  /* 读取指定输入引脚电平（0/1） */
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);                        /* 读取整个端口输入寄存器值 */
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);  /* 读取输出锁存器状态 */
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);                       /* 读取整个端口输出寄存器值 */

void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);    /* 置位指定引脚（原子操作） */
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);  /* 清除指定引脚（原子操作） */
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);  /* 位写入操作 */
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);        /* 整个端口写入（16位掩码） */
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);/* 引脚电平翻转（XOR操作） */

/*---------------- 复用功能配置函数 ----------------*/

/**
  * @brief  GPIO引脚复用功能配置
  * @param  GPIOx 目标端口
  * @param  GPIO_PinSource 引脚源（0-15）
  * @param  GPIO_AF 复用功能号（GPIO_AF_0~15）
  * @retval 无
  * @warning 需先使能对应外设时钟，配置顺序：先GPIO_Init再调用本函数
  */
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_GPIO_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

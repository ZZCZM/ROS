/**
  ******************************************************************************
  * @file    stm32f4xx_syscfg.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the SYSCFG firmware
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
#ifndef __STM32F4xx_SYSCFG_H
#define __STM32F4xx_SYSCFG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup SYSCFG
  * @{
  */ 
/**
 * @defgroup SYSCFG_EXTI_Port_Sources
 * @{
 * 
 * @brief EXTI GPIO端口映射配置集
 * 
 * 定义SYSCFG_EXTICR寄存器中GPIO端口选择位的参数值，用于建立外部中断线
 * (EXTI)与具体GPIO端口之间的硬件连接映射。每个宏对应一个GPIO bank的配置编码。
 */

/* 基本端口定义（A-K）*/
#define EXTI_PortSourceGPIOA       ((uint8_t)0x00)  /* GPIOA端口选择码：0b0000 */
#define EXTI_PortSourceGPIOB       ((uint8_t)0x01)  /* GPIOB端口选择码：0b0001 */ 
#define EXTI_PortSourceGPIOC       ((uint8_t)0x02)  /* GPIOC端口编码，对应EXTICR[3:0]位 */
#define EXTI_PortSourceGPIOD       ((uint8_t)0x03)  /* 端口D选择值，按芯片手册EXTI配置表 */
#define EXTI_PortSourceGPIOE       ((uint8_t)0x04)  /* 端口E的EXTI连接标识值 */
#define EXTI_PortSourceGPIOF       ((uint8_t)0x05)  /* 用于EXTI5_9等多线中断的端口配置 */
#define EXTI_PortSourceGPIOG       ((uint8_t)0x06)  /* 端口G的硬件连接标识 */
#define EXTI_PortSourceGPIOH       ((uint8_t)0x07)  /* 高编号端口H的映射值 */
#define EXTI_PortSourceGPIOI       ((uint8_t)0x08)  /* 端口I选择码（部分型号支持） */
#define EXTI_PortSourceGPIOJ       ((uint8_t)0x09)  /* 扩展端口J配置编码 */ 
#define EXTI_PortSourceGPIOK       ((uint8_t)0x0A)  /* 端口K的十六进制选择值0xA */

/**
 * @def IS_EXTI_PORT_SOURCE
 * @brief 端口源参数验证宏
 * @param PORTSOURCE 待验证的配置值
 * @retval 1=有效GPIO端口配置，0=非法参数
 * 
 * @details 通过逻辑或运算链式校验输入值是否在预定义的GPIO端口编码范围内，
 *          常用于硬件配置函数的参数检查，防止非法的寄存器写入操作。
 *          注意：实际使用时应配合assert_param()等调试机制。
 */
#define IS_EXTI_PORT_SOURCE(PORTSOURCE) ( \
    ((PORTSOURCE) == EXTI_PortSourceGPIOA) || /* 验证A端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOB) || /* 验证B端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOC) || /* 验证C端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOD) || /* 验证D端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOE) || /* 验证E端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOF) || /* 验证F端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOG) || /* 验证G端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOH) || /* 验证H端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOI) || /* 验证I端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOJ) || /* 验证J端口 */ \
    ((PORTSOURCE) == EXTI_PortSourceGPIOK))   /* 验证K端口 */

    /**
 * @defgroup SYSCFG_EXTI_Pin_Sources 
  * @{
  * 
  * @brief EXTI引脚源选择配置集
  * 
  * 定义SYSCFG_EXTICR寄存器中引脚选择位的参数值，用于将GPIO引脚与16个外部中断线(EXTI0~EXTI15)
  * 建立硬件关联。每个宏对应特定GPIO引脚在EXTI配置寄存器中的4位选择编码。
  * 
  * @details 当配置外部中断时，需组合使用端口源(PortSource)和引脚源(PinSource)宏：
  *          - 端口源确定GPIO bank
  *          - 引脚源确定具体引脚编号
  *          共同组成EXTICR寄存器的完整配置值
  */

/* 基础引脚编号定义（0-15）*/
#define EXTI_PinSource0            ((uint8_t)0x00)  /* 引脚0选择码，对应EXTI0中断线 */
#define EXTI_PinSource1            ((uint8_t)0x01)  /* 引脚1配置值，用于EXTI1中断 */
#define EXTI_PinSource2            ((uint8_t)0x02)  /* 引脚2编码，控制EXTI2信号路由 */
#define EXTI_PinSource3            ((uint8_t)0x03)  /* 引脚3选择，影响EXTICR[3:0]位域 */
#define EXTI_PinSource4            ((uint8_t)0x04)  /* 配置引脚4与EXTI4中断的关联 */
#define EXTI_PinSource5            ((uint8_t)0x05)  /* 引脚5选择码，常用于按键中断配置 */
#define EXTI_PinSource6            ((uint8_t)0x06)  /* 引脚6对应值，需配合端口源使用 */
#define EXTI_PinSource7            ((uint8_t)0x07)  /* 引脚7配置，范围0x00-0x0F */
#define EXTI_PinSource8            ((uint8_t)0x08)  /* 高编号引脚起始值，用于EXTI8-EXTI15 */
#define EXTI_PinSource9            ((uint8_t)0x09)  /* 引脚9选择，对应EXTI9中断事件 */
#define EXTI_PinSource10           ((uint8_t)0x0A)  /* 十六进制格式引脚10配置（0xA） */
#define EXTI_PinSource11           ((uint8_t)0x0B)  /* 引脚11编码，用于EXTI11中断线 */
#define EXTI_PinSource12           ((uint8_t)0x0C)  /* 引脚12选择值，范围需小于0x10 */
#define EXTI_PinSource13           ((uint8_t)0x0D)  /* 引脚13配置，对应EXTICR[15:12] */
#define EXTI_PinSource14           ((uint8_t)0x0E)  /* 引脚14参数，用于边沿触发配置 */
#define EXTI_PinSource15           ((uint8_t)0x0F)  /* 最大引脚编号，对应EXTI15中断 */

/**
 * @def IS_EXTI_PIN_SOURCE
 * @brief 引脚源参数有效性验证宏
 * @param PINSOURCE 待验证的引脚选择值
 * @retval 1=有效引脚编号，0=超出允许范围
 * 
 * @details 通过逻辑或链式校验确保输入值在0x00-0x0F范围内，防止配置非法的EXTI引脚映射。
 *          该验证应配合STM32标准外设库的assert_param()机制使用，在调试阶段捕获错误配置。
 */
#define IS_EXTI_PIN_SOURCE(PINSOURCE) ( \
    ((PINSOURCE) == EXTI_PinSource0)  || /* 校验引脚0 */ \
    ((PINSOURCE) == EXTI_PinSource1)  || /* 校验引脚1 */ \
    ((PINSOURCE) == EXTI_PinSource2)  || /* 校验引脚2 */ \
    ((PINSOURCE) == EXTI_PinSource3)  || /* 校验引脚3 */ \
    ((PINSOURCE) == EXTI_PinSource4)  || /* 校验引脚4 */ \
    ((PINSOURCE) == EXTI_PinSource5)  || /* 校验引脚5 */ \
    ((PINSOURCE) == EXTI_PinSource6)  || /* 校验引脚6 */ \
    ((PINSOURCE) == EXTI_PinSource7)  || /* 校验引脚7 */ \
    ((PINSOURCE) == EXTI_PinSource8)  || /* 校验引脚8 */ \
    ((PINSOURCE) == EXTI_PinSource9)  || /* 校验引脚9 */ \
    ((PINSOURCE) == EXTI_PinSource10) || /* 校验引脚10 */ \
    ((PINSOURCE) == EXTI_PinSource11) || /* 校验引脚11 */ \
    ((PINSOURCE) == EXTI_PinSource12) || /* 校验引脚12 */ \
    ((PINSOURCE) == EXTI_PinSource13) || /* 校验引脚13 */ \
    ((PINSOURCE) == EXTI_PinSource14) || /* 校验引脚14 */ \
    ((PINSOURCE) == EXTI_PinSource15))   /* 校验引脚15（最大值） */

/**
  * @}
  */ 

/**
 * @defgroup SYSCFG_Memory_Remap_Config 
  * @{
  * 
  * @brief 存储器地址空间重映射配置
  * 
  * 控制系统启动时的存储器映射方案，通过SYSCFG_MEMRMP寄存器实现不同物理存储器
  * 与系统地址空间的映射关系切换。需在系统初始化阶段配置，直接影响CPU对存储器的访问路径。
  */

/* 标准映射方案 */
#define SYSCFG_MemoryRemap_Flash       ((uint8_t)0x00)  // 默认Flash映射到0x08000000
#define SYSCFG_MemoryRemap_SystemFlash ((uint8_t)0x01)  // 系统Flash（bootloader）映射到0x00000000
#define SYSCFG_MemoryRemap_SRAM        ((uint8_t)0x03)  // 内部SRAM映射到0x00000000（调试用）
#define SYSCFG_MemoryRemap_SDRAM       ((uint8_t)0x04)  // 外部SDRAM映射（仅大容量型号支持）

/* 芯片特定存储器接口 */
#if defined (STM32F40_41xxx)
#define SYSCFG_MemoryRemap_FSMC        ((uint8_t)0x02)  // FSMC控制器映射（F40x/41x系列）
#endif 

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define SYSCFG_MemoryRemap_FMC         ((uint8_t)0x02)  // FMC控制器映射（427/429系列）
#endif

/**
 * @defgroup 存储器重映射验证宏
 * @brief 验证存储器重映射配置的有效性
 * @param REMAP 配置参数值
 * 
 * @details 根据芯片型号进行差异化校验：
 * - F40x/41x系列支持FSMC
 * - F401/F411精简版去除了FSMC支持
 * - 427/429系列支持FMC和SDRAM
 */
#if defined (STM32F40_41xxx)
// 校验默认Flash映射
// 系统Flash启动模式
// SRAM启动模式
// 外部总线控制器映射
#define IS_SYSCFG_MEMORY_REMAP_CONFING(REMAP) ( \
    ((REMAP) == SYSCFG_MemoryRemap_Flash)       || \
    ((REMAP) == SYSCFG_MemoryRemap_SystemFlash) || \
    ((REMAP) == SYSCFG_MemoryRemap_SRAM)        || \
    ((REMAP) == SYSCFG_MemoryRemap_FSMC))            
#endif

#if defined (STM32F401xx) || defined (STM32F411xE)
#define IS_SYSCFG_MEMORY_REMAP_CONFING(REMAP) ( \
    ((REMAP) == SYSCFG_MemoryRemap_Flash)       || \
    ((REMAP) == SYSCFG_MemoryRemap_SystemFlash) || \
    ((REMAP) == SYSCFG_MemoryRemap_SRAM))       // 精简型号不支持FSMC
#endif
// 大容量型号SDRAM支持
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define IS_SYSCFG_MEMORY_REMAP_CONFING(REMAP) ( \
    ((REMAP) == SYSCFG_MemoryRemap_Flash)       || \
    ((REMAP) == SYSCFG_MemoryRemap_SystemFlash) || \
    ((REMAP) == SYSCFG_MemoryRemap_SRAM)        || \
    ((REMAP) == SYSCFG_MemoryRemap_SDRAM)       || \
    ((REMAP) == SYSCFG_MemoryRemap_FMC))             // 升级版存储控制器
#endif

/**
 * @defgroup SYSCFG_ETHERNET_Media_Interface 
  * @{
  * 
  * @brief 以太网物理层接口模式配置
  * 
  * 通过SYSCFG_PMC寄存器选择MII/RMII物理层接口，需与外部PHY芯片的接口类型匹配。
  * 配置时机：以太网外设初始化前，且不可运行时动态修改。
  */

#define SYSCFG_ETH_MediaInterface_MII    ((uint32_t)0x00000000)  // 标准MII接口，需16个信号线
#define SYSCFG_ETH_MediaInterface_RMII   ((uint32_t)0x00000001)  // 精简RMII接口，信号线减半

/**
 * @def IS_SYSCFG_ETH_MEDIA_INTERFACE
 * @brief 以太网接口模式验证
 * @param INTERFACE 配置参数值
 * @retval 1=有效模式 0=非法参数
 * 
 * @details 检查是否为预定义的MII(0)或RMII(1)模式，
 *          防止配置不支持的物理层接口类型
 */
// 校验MII模式
// 校验RMII模式
#define IS_SYSCFG_ETH_MEDIA_INTERFACE(INTERFACE) ( \
    ((INTERFACE) == SYSCFG_ETH_MediaInterface_MII)  || \
    ((INTERFACE) == SYSCFG_ETH_MediaInterface_RMII))    

/**
  * @}
  */ 

/**
  * @}
  */ 
/**
 * @brief 复位SYSCFG模块相关寄存器至默认值
 * @details 将SYSCFG_MEMRMP、CFGR1等寄存器复位为初始状态
 *          - 恢复存储器映射到默认Flash地址
 *          - 清除以太网接口模式配置
 *          - 禁用补偿单元
 */
void SYSCFG_DeInit(void);

/**
 * @brief 配置系统存储器映射方案
 * @param SYSCFG_MemoryRemap 映射配置参数，取值为：
 *        @arg SYSCFG_MemoryRemap_Flash/SYSCFG_MemoryRemap_SystemFlash等
 * @note 该配置直接影响BOOT0引脚无效时的启动地址映射，建议在系统初始化阶段调用
 *       更改后需配合系统复位才能生效
 */
void SYSCFG_MemoryRemapConfig(uint8_t SYSCFG_MemoryRemap);

/**
 * @brief 控制Flash存储器的Bank交换功能
 * @param NewState 使能状态：ENABLE/DISABLE
 * @note 仅支持具有双Bank架构的Flash型号（如F4系列中大容量型号）
 *       操作前需确保已关闭所有中断并遵循ST官方编程时序
 */
void SYSCFG_MemorySwappingBank(FunctionalState NewState);

/**
 * @brief 配置外部中断线(EXTI)与GPIO引脚的硬件关联
 * @param EXTI_PortSourceGPIOx GPIO端口源（使用EXTI_PortSourceGPIOA等宏）
 * @param EXTI_PinSourcex GPIO引脚源（使用EXTI_PinSource0等宏）
 * @details 通过配置SYSCFG_EXTICR寄存器，将特定EXTI线连接到指定GPIO引脚
 *          例如：SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0)
 *          将EXTI0线连接到PA0引脚
 */
void SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex);

/**
 * @brief 配置以太网物理层接口模式
 * @param SYSCFG_ETH_MediaInterface 接口模式选择：
 *        @arg SYSCFG_ETH_MediaInterface_MII/RMII
 * @warning 需在以太网外设初始化前配置，运行时不可动态修改
 *          必须与硬件PHY芯片的接口类型严格匹配
 */
void SYSCFG_ETH_MediaInterfaceConfig(uint32_t SYSCFG_ETH_MediaInterface);

/**
 * @brief 使能/禁用I/O补偿单元
 * @param NewState 使能状态：ENABLE/DISABLE
 * @note 用于高速GPIO信号的信号完整性补偿
 *       使能条件：电源电压在2.4V~3.6V之间
 *       建议上电后优先使能补偿单元再进行GPIO配置
 */
void SYSCFG_CompensationCellCmd(FunctionalState NewState);

/**
 * @brief 获取补偿单元就绪状态
 * @retval SET - 补偿单元已就绪
 *         RESET - 补偿单元未就绪或未启用
 * @note 在使能补偿单元后应检测该状态，确认补偿电路稳定后再进行后续操作
 */
FlagStatus SYSCFG_GetCompensationCellStatus(void);


#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_SYSCFG_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

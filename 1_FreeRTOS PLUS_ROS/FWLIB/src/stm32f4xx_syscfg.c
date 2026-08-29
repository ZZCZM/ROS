/**
  ******************************************************************************
  * @file    stm32f4xx_syscfg.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the SYSCFG peripheral.
  *
 @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..] This driver provides functions for:
            
       (#) Remapping the memory accessible in the code area using SYSCFG_MemoryRemapConfig()
            
       (#) Swapping the internal flash Bank1 and Bank2 this features is only visible for 
           STM32F42xxx/43xxx devices Devices. 
                
       (#) Manage the EXTI lines connection to the GPIOs using SYSCFG_EXTILineConfig()
              
       (#) Select the ETHERNET media interface (RMII/RII) using SYSCFG_ETH_MediaInterfaceConfig()
  
       -@- SYSCFG APB clock must be enabled to get write access to SYSCFG registers,
           using RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
                   
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
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"

/* ------------ 外设寄存器位带别名区域定义 ----------- */

/* 计算SYSCFG模块在外设地址空间中的偏移量(单位：字节) 
   SYSCFG_BASE = 0x40013800, PERIPH_BASE = 0x40000000
   计算结果：0x13800 - 0x00000 = 0x13800 */
#define SYSCFG_OFFSET             (SYSCFG_BASE - PERIPH_BASE)

/* --- MEMRMP寄存器相关定义（存储器重映射控制寄存器） --- */
/* MEMRMP寄存器在SYSCFG模块中的偏移量：0x00 */
#define MEMRMP_OFFSET             SYSCFG_OFFSET  /* 等价于0x13800 */
/* UFB_MODE位在MEMRMP寄存器中的位置：第8位（bit8） 
   该位控制Flash加速模式：
   0: 禁用加速模式
   1: 启用加速模式 */
#define UFB_MODE_BitNumber        ((uint8_t)0x8)
/* 计算UFB_MODE位的位带别名地址 
   公式：0x42000000 + (0x13800 * 32) + (8 * 4) 
   计算结果：0x42000000 + 0x270000 + 0x20 = 0x42270020 */
#define UFB_MODE_BB               (PERIPH_BB_BASE + (MEMRMP_OFFSET * 32) + (UFB_MODE_BitNumber * 4))

/* --- PMC寄存器相关定义（外设模式配置寄存器） --- */
/* PMC寄存器在SYSCFG模块中的偏移量：0x04
   计算：0x13800(SYSCFG) + 0x04 = 0x13804 */
#define PMC_OFFSET                (SYSCFG_OFFSET + 0x04)
/* MII_RMII_SEL位在PMC寄存器中的位置：第23位（bit23）
   该位选择以太网PHY接口类型：
   0: RMII接口
   1: MII接口 */
#define MII_RMII_SEL_BitNumber    ((uint8_t)0x17)  /* 十进制23 */
/* 计算MII_RMII_SEL位的位带别名地址
   公式：0x42000000 + (0x13804 * 32) + (23 * 4)
   计算结果：0x42000000 + 0x2700800 + 0x5C = 0x4470085C */
#define PMC_MII_RMII_SEL_BB       (PERIPH_BB_BASE + (PMC_OFFSET * 32) + (MII_RMII_SEL_BitNumber * 4))

/* --- CMPCR寄存器相关定义（补偿单元控制寄存器） --- */
/* CMPCR寄存器在SYSCFG模块中的偏移量：0x20 
   计算：0x13800(SYSCFG) + 0x20 = 0x13820 */
#define CMPCR_OFFSET              (SYSCFG_OFFSET + 0x20)
/* CMP_PD位在CMPCR寄存器中的位置：第0位（bit0）
   该位控制补偿单元电源：
   0: 补偿单元掉电
   1: 补偿单元上电 */
#define CMP_PD_BitNumber          ((uint8_t)0x00)
/* 计算CMP_PD位的位带别名地址
   公式：0x42000000 + (0x13820 * 32) + (0 * 4)
   计算结果：0x42000000 + 0x2704000 = 0x44704000 */
#define CMPCR_CMP_PD_BB           (PERIPH_BB_BASE + (CMPCR_OFFSET * 32) + (CMP_PD_BitNumber * 4))




/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup SYSCFG 
  * @brief SYSCFG driver modules
  * @{
  */ 
/**
  * @brief  系统配置控制器复位函数
  * @功能   复位SYSCFG模块的所有配置寄存器到默认值
  * @细节   通过APB2总线复位SYSCFG外设，先使能复位后立即关闭
  *         操作流程：
  *         1. 使能SYSCFG模块的软件复位
  *         2. 立即关闭复位以完成复位过程
  *         典型应用场景：系统初始化/配置异常恢复
  */
void SYSCFG_DeInit(void)
{
   /* 通过RCC的APB2外设复位控制器使能SYSCFG复位 */
   /* 参数1：选择SYSCFG外设（RCC_APB2Periph_SYSCFG）*/
   /* 参数2：使能复位操作（ENABLE）*/
   RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, ENABLE);

   /* 立即禁用SYSCFG复位以完成复位过程 */
   /* 参数1：保持选择SYSCFG外设不变 */
   /* 参数2：禁用复位操作（DISABLE）*/
   RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, DISABLE);
}

/**
  * @brief  存储器重映射配置函数
  * @功能   设置STM32的存储器映射方案
  * @细节   通过配置SYSCFG的MEMRMP寄存器实现不同存储区域的地址映射
  * @参数   SYSCFG_MemoryRemap: 存储器重映射配置选项
  *         可选值：
  *         [0x00] SYSCFG_MemoryRemap_Flash       主Flash映射到0x00000000
  *         [0x01] SYSCFG_MemoryRemap_SystemFlash 系统Flash（引导程序）映射到0x00000000
  *         [0x02] SYSCFG_MemoryRemap_FSMC        FSMC Bank1映射（F40x/F41x系列）
  *         [0x12] SYSCFG_MemoryRemap_FMC         FMC Bank1映射（F42x/F43x系列）
  *         [0x03] SYSCFG_MemoryRemap_SRAM        内置SRAM映射到0x00000000
  *         [0x04] SYSCFG_MemoryRemap_SDRAM       FMC SDRAM映射（F42x/F43x系列）
  * @注意   1. 使用断言机制验证参数有效性
  *         2. 直接操作SYSCFG->MEMRMP寄存器
  *         3. 改变映射后需要同步更新相关外设配置
  */
void SYSCFG_MemoryRemapConfig(uint8_t SYSCFG_MemoryRemap)
{
  /* 参数有效性检查，使用预定义的参数校验宏 */
  /* IS_SYSCFG_MEMORY_REMAP_CONFING宏验证输入参数是否在允许范围内 */
  assert_param(IS_SYSCFG_MEMORY_REMAP_CONFING(SYSCFG_MemoryRemap));

  /* 写入配置值到存储器重映射寄存器(MEMRMP) */
  /* 寄存器地址：SYSCFG_BASE + 0x00 */
  /* 32位寄存器，实际使用低8位[7:0] */
  SYSCFG->MEMRMP = SYSCFG_MemoryRemap;
}

/**
  * @brief  Enables or disables the Interal FLASH Bank Swapping.
  *   
  * @note   This function can be used only for STM32F42xxx/43xxx devices. 
  *
  * @param  NewState: new state of Interal FLASH Bank swapping.
  *          This parameter can be one of the following values:
  *            @arg ENABLE: Flash Bank2 mapped at 0x08000000 (and aliased @0x00000000) 
  *                         and Flash Bank1 mapped at 0x08100000 (and aliased at 0x00100000)   
  *            @arg DISABLE:(the default state) Flash Bank1 mapped at 0x08000000 (and aliased @0x0000 0000) 
                            and Flash Bank2 mapped at 0x08100000 (and aliased at 0x00100000)  
  * @retval None
  */
/**
  * @brief  Flash存储体交换控制函数
  * @功能   启用或禁用内部Flash存储体交换功能
  * @细节   通过位带操作修改UFB_MODE控制位
  *         适用场景：双Bank Flash固件升级时切换执行区域
  * @参数   NewState: 功能状态(ENABLE/DISABLE)
  * @注意   1. 仅适用于支持双Bank Flash的STM32F42xxx/43xxx系列
  *         2. 操作前需确保Flash处于非编程状态
  */
void SYSCFG_MemorySwappingBank(FunctionalState NewState)
{
  /* 参数有效性检查(ENABLE/DISABLE) */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 通过位带地址直接操作UFB_MODE控制位 */
  /* UFB_MODE_BB是经过计算的位带别名地址 */
  *(__IO uint32_t *) UFB_MODE_BB = (uint32_t)NewState;
}

/**
  * @brief  EXTI外部中断线映射配置函数
  * @功能   将指定GPIO引脚连接到EXTI中断线
  * @细节   通过配置EXTICR寄存器实现GPIO与中断线的映射
  * @参数   EXTI_PortSourceGPIOx: GPIO端口源(A..K)
  *         EXTI_PinSourcex:      GPIO引脚号(0-15)
  * @操作流程：
  *         1. 清除目标EXTICR寄存器原有配置
  *         2. 写入新的GPIO端口映射
  */
void SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex)
{
  uint32_t tmp = 0x00;

  /* 双参数有效性检查 */
  assert_param(IS_EXTI_PORT_SOURCE(EXTI_PortSourceGPIOx));
  assert_param(IS_EXTI_PIN_SOURCE(EXTI_PinSourcex));

  /* 生成4位清除掩码：0xF左移对应的位位置 */
  tmp = ((uint32_t)0x0F) << (0x04 * (EXTI_PinSourcex & 0x03));
  
  /* 选择对应的EXTICR寄存器（每寄存器管理4个EXTI线）*/
  /* 通过右移2位计算寄存器索引（相当于除以4）*/
  SYSCFG->EXTICR[EXTI_PinSourcex >> 0x02] &= ~tmp;
  
  /* 写入新的端口映射配置 */
  /* 将端口编号左移到正确位域位置 */
  SYSCFG->EXTICR[EXTI_PinSourcex >> 0x02] |= 
    (((uint32_t)EXTI_PortSourceGPIOx) << (0x04 * (EXTI_PinSourcex & 0x03)));
}

/**
  * @brief  以太网物理层接口模式配置
  * @功能   选择ETH外接PHY芯片的接口类型
  * @参数   SYSCFG_ETH_MediaInterface: 接口模式
  *         - SYSCFG_ETH_MediaInterface_MII:  标准MII接口
  *         - SYSCFG_ETH_MediaInterface_RMII: 精简RMII接口
  * @注意   1. 需在ETH初始化前调用
  *         2. 硬件连接必须与配置模式匹配
  */
void SYSCFG_ETH_MediaInterfaceConfig(uint32_t SYSCFG_ETH_MediaInterface) 
{ 
  /* 验证输入参数有效性 */
  assert_param(IS_SYSCFG_ETH_MEDIA_INTERFACE(SYSCFG_ETH_MediaInterface)); 
  
  /* 通过位带操作设置MII/RMII选择位 */
  /* PMC_MII_RMII_SEL_BB是预计算的位带别名地址 */
  *(__IO uint32_t *) PMC_MII_RMII_SEL_BB = SYSCFG_ETH_MediaInterface; 
}

/**
  * @brief  I/O补偿单元控制函数
  * @功能   启用/禁用I/O信号完整性补偿电路
  * @参数   NewState: 补偿单元状态(ENABLE/DISABLE)
  * @注意   1. 仅在2.4-3.6V供电范围内有效
  *         2. 使能后需等待READY标志置位
  *         3. 低功耗模式下自动关闭
  */
void SYSCFG_CompensationCellCmd(FunctionalState NewState)
{
  /* 参数有效性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 通过位带操作CMP_PD控制位 */
  /* CMPCR_CMP_PD_BB是补偿电路电源控制位的别名地址 */
  *(__IO uint32_t *) CMPCR_CMP_PD_BB = (uint32_t)NewState;
}

/**
  * @brief  获取I/O补偿单元状态
  * @功能   检测补偿电路是否准备就绪
  * @返回   FlagStatus: 就绪状态(SET/RESET)
  * @注意   在使能补偿单元后必须检测此状态
  *         典型等待流程：
  *         1. 调用SYSCFG_CompensationCellCmd(ENABLE)
  *         2. 轮询本函数直到返回SET
  */
FlagStatus SYSCFG_GetCompensationCellStatus(void)
{
  FlagStatus bitstatus = RESET;
    
  /* 检查CMPCR寄存器的READY标志位 */
  if ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY ) != (uint32_t)RESET)
  {
    bitstatus = SET;  /* 补偿电路已稳定 */
  }
  else
  {
    bitstatus = RESET; /* 补偿电路未就绪 */
  }
  return bitstatus;
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

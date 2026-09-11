/**
  ******************************************************************************
  * @file    stm32f4xx_sdio.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Secure digital input/output interface (SDIO) 
  *          peripheral:
  *           + Initialization and Configuration
  *           + Command path state machine (CPSM) management
  *           + Data path state machine (DPSM) management
  *           + SDIO IO Cards mode management
  *           + CE-ATA mode management
  *           + DMA transfers management
  *           + Interrupts and flags management
  *
@verbatim

 ===================================================================
                 ##### How to use this driver #####
 ===================================================================
 [..]
   (#) The SDIO clock (SDIOCLK = 48 MHz) is coming from a specific output of PLL 
       (PLL48CLK). Before to start working with SDIO peripheral make sure that the
       PLL is well configured.
       The SDIO peripheral uses two clock signals:
       (++) SDIO adapter clock (SDIOCLK = 48 MHz)
       (++) APB2 bus clock (PCLK2)
       
       -@@- PCLK2 and SDIO_CK clock frequencies must respect the following condition:
           Frequency(PCLK2) >= (3 / 8 x Frequency(SDIO_CK))
  
   (#) Enable peripheral clock using RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE).
  
   (#) According to the SDIO mode, enable the GPIO clocks using 
       RCC_AHB1PeriphClockCmd() function. 
       The I/O can be one of the following configurations:
       (++) 1-bit data length: SDIO_CMD, SDIO_CK and D0.
       (++) 4-bit data length: SDIO_CMD, SDIO_CK and D[3:0].
       (++) 8-bit data length: SDIO_CMD, SDIO_CK and D[7:0].      
  
   (#) Peripheral alternate function: 
       (++) Connect the pin to the desired peripherals' Alternate Function (AF) 
           using GPIO_PinAFConfig() function
       (++) Configure the desired pin in alternate function by: 
           GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
       (++) Select the type, pull-up/pull-down and output speed via GPIO_PuPd, 
           GPIO_OType and GPIO_Speed members
       (++) Call GPIO_Init() function
  
   (#) Program the Clock Edge, Clock Bypass, Clock Power Save, Bus Wide, 
       hardware, flow control and the Clock Divider using the SDIO_Init()
       function.
  
   (#) Enable the Power ON State using the SDIO_SetPowerState(SDIO_PowerState_ON) 
       function.
                
   (#) Enable the clock using the SDIO_ClockCmd() function.
  
   (#) Enable the NVIC and the corresponding interrupt using the function 
       SDIO_ITConfig() if you need to use interrupt mode. 
  
   (#) When using the DMA mode 
       (++) Configure the DMA using DMA_Init() function
       (++) Active the needed channel Request using SDIO_DMACmd() function
  
   (#) Enable the DMA using the DMA_Cmd() function, when using DMA mode. 
  
   (#) To control the CPSM (Command Path State Machine) and send 
       commands to the card use the SDIO_SendCommand(), 
       SDIO_GetCommandResponse() and SDIO_GetResponse() functions. First, user has
       to fill the command structure (pointer to SDIO_CmdInitTypeDef) according 
       to the selected command to be sent.
       The parameters that should be filled are:
       (++) Command Argument
       (++) Command Index
       (++) Command Response type
       (++) Command Wait
       (++) CPSM Status (Enable or Disable).
  
       -@@- To check if the command is well received, read the SDIO_CMDRESP
           register using the SDIO_GetCommandResponse().
           The SDIO responses registers (SDIO_RESP1 to SDIO_RESP2), use the
           SDIO_GetResponse() function.
  
   (#) To control the DPSM (Data Path State Machine) and send/receive 
       data to/from the card use the SDIO_DataConfig(), SDIO_GetDataCounter(), 
       SDIO_ReadData(), SDIO_WriteData() and SDIO_GetFIFOCount() functions.
  
 *** Read Operations ***
 =======================
 [..]
   (#) First, user has to fill the data structure (pointer to
       SDIO_DataInitTypeDef) according to the selected data type to be received.
       The parameters that should be filled are:
       (++) Data TimeOut
       (++) Data Length
       (++) Data Block size
       (++) Data Transfer direction: should be from card (To SDIO)
       (++) Data Transfer mode
       (++) DPSM Status (Enable or Disable)
                                     
   (#) Configure the SDIO resources to receive the data from the card
       according to selected transfer mode (Refer to Step 8, 9 and 10).
  
   (#)  Send the selected Read command (refer to step 11).
                    
   (#) Use the SDIO flags/interrupts to check the transfer status.
  
 *** Write Operations ***
 ========================
 [..]
   (#) First, user has to fill the data structure (pointer to
       SDIO_DataInitTypeDef) according to the selected data type to be received.
       The parameters that should be filled are:
       (++) Data TimeOut
       (++) Data Length
       (++) Data Block size
       (++) Data Transfer direction:  should be to card (To CARD)
       (++) Data Transfer mode
       (++) DPSM Status (Enable or Disable)
  
   (#) Configure the SDIO resources to send the data to the card according to 
       selected transfer mode (Refer to Step 8, 9 and 10).
                     
   (#) Send the selected Write command (refer to step 11).
                    
   (#) Use the SDIO flags/interrupts to check the transfer status.
  
  
@endverbatim
  *
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
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup SDIO 
  * @brief SDIO driver modules
  * @{
  */ 
/**
  * @brief SDIO控制器寄存器位操作定义
  * @note  本部分包含SDIO外设的位带别名地址和寄存器位清除掩码定义，主要用途：
  *         - 通过位带技术实现寄存器位的原子操作
  *         - 定义寄存器操作时的位掩码，用于安全清除无关位
  * @warning 使用位带操作前需确保外设时钟已使能
  */

/* SDIO寄存器相对于外设基地址的偏移量 */
#define SDIO_OFFSET                (SDIO_BASE - PERIPH_BASE)  /* SDIO寄存器组偏移量：0xA0003000 - 0x40000000 */

/* ------------------------- 位带别名地址定义 ------------------------- */
/* CLKCR寄存器位定义 */
#define CLKCR_OFFSET              (SDIO_OFFSET + 0x04)        /* CLKCR寄存器偏移量：SDIO_OFFSET + 0x04 */
#define CLKEN_BitNumber           0x08                         /* 时钟使能位在第8位 */
#define CLKCR_CLKEN_BB            (PERIPH_BB_BASE + (CLKCR_OFFSET * 32) + (CLKEN_BitNumber * 4)) /* 位带地址计算：
                                                                                                 PERIPH_BB_BASE(0x42000000) + 
                                                                                                 (0x3004 * 32) + (8 * 4) = 
                                                                                                 0x424600A0 */

/* CMD寄存器位定义 */
#define CMD_OFFSET                (SDIO_OFFSET + 0x0C)        /* CMD寄存器偏移量：SDIO_OFFSET + 0x0C */
/* SDIO暂停控制位 */
#define SDIOSUSPEND_BitNumber     0x0B                         /* 暂停位在第11位 */
#define CMD_SDIOSUSPEND_BB        (PERIPH_BB_BASE + (CMD_OFFSET * 32) + (SDIOSUSPEND_BitNumber * 4)) /* 0x424600F8 */
/* 命令完成中断使能位 */
#define ENCMDCOMPL_BitNumber      0x0C                         /* 第12位 */
#define CMD_ENCMDCOMPL_BB         (PERIPH_BB_BASE + (CMD_OFFSET * 32) + (ENCMDCOMPL_BitNumber * 4)) /* 0x424600FC */
/* 非中断模式使能位 */
#define NIEN_BitNumber            0x0D                         /* 第13位 */
#define CMD_NIEN_BB               (PERIPH_BB_BASE + (CMD_OFFSET * 32) + (NIEN_BitNumber * 4))        /* 0x42460100 */
/* ATA命令模式位 */
#define ATACMD_BitNumber          0x0E                         /* 第14位 */
#define CMD_ATACMD_BB             (PERIPH_BB_BASE + (CMD_OFFSET * 32) + (ATACMD_BitNumber * 4))     /* 0x42460104 */

/* DCTRL寄存器位定义 */
#define DCTRL_OFFSET              (SDIO_OFFSET + 0x2C)        /* DCTRL寄存器偏移量：SDIO_OFFSET + 0x2C */
/* DMA使能位 */
#define DMAEN_BitNumber           0x03                         /* 第3位 */
#define DCTRL_DMAEN_BB            (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (DMAEN_BitNumber * 4))    /* 0x4246058C */
/* 读写启动位 */
#define RWSTART_BitNumber         0x08                         /* 第8位 */
#define DCTRL_RWSTART_BB          (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (RWSTART_BitNumber * 4))  /* 0x424605AC */
/* 读写停止位 */
#define RWSTOP_BitNumber          0x09                         /* 第9位 */
#define DCTRL_RWSTOP_BB           (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (RWSTOP_BitNumber * 4))   /* 0x424605B0 */
/* 读写模式位 */
#define RWMOD_BitNumber           0x0A                         /* 第10位 */
#define DCTRL_RWMOD_BB            (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (RWMOD_BitNumber * 4))      /* 0x424605B4 */
/* SDIO使能位 */
#define SDIOEN_BitNumber          0x0B                         /* 第11位 */
#define DCTRL_SDIOEN_BB           (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (SDIOEN_BitNumber * 4))   /* 0x424605B8 */

/* ------------------------- 寄存器位清除掩码 ------------------------- */
/* CLKCR寄存器保留位掩码（清零时钟分频和旁路位） */
#define CLKCR_CLEAR_MASK         ((uint32_t)0xFFFF8100)  /* 保留位[15:9][7:0], 清除位[8]CLKEN和[12:10]CLKDIV */

/* PWRCTRL电源控制保留位掩码 */
#define PWR_PWRCTRL_MASK         ((uint32_t)0xFFFFFFFC)  /* 仅保留最低2位(PWRCTRL[1:0]) */

/* DCTRL寄存器保留位掩码 */
#define DCTRL_CLEAR_MASK         ((uint32_t)0xFFFFFF08)  /* 保留位[10:8][2:0], 清除流量控制相关位 */

/* CMD寄存器保留位掩码 */
#define CMD_CLEAR_MASK           ((uint32_t)0xFFFFF800)  /* 保留命令索引位[5:0], 清除其他控制位 */

/* 响应寄存器地址定义 */
#define SDIO_RESP_ADDR           ((uint32_t)(SDIO_BASE + 0x14))  /* 响应寄存器组起始地址：SDIO_BASE + 0x14 */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup SDIO_Private_Functions
  * @{
  */

/** @defgroup SDIO_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
              ##### Initialization and Configuration functions #####
 ===============================================================================

@endverbatim
  * @{
  */
/**
  * @brief  SDIO控制器初始化与复位函数组
  * @note   包含SDIO外设的复位、初始化及结构体默认值设置功能
  * @warning 操作前需确保已启用SDIO时钟（RCC_APB2PeriphClockCmd）
  */

/**
  * @brief  复位SDIO外设至默认状态
  * @param  无
  * @retval 无
  * @note   通过RCC的APB2外设复位控制器实现硬件复位
  */
void SDIO_DeInit(void)
{
  /* 使能SDIO外设复位（置位复位信号） */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, ENABLE);
  /* 禁用SDIO外设复位（释放复位信号，完成复位操作） */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, DISABLE);
}

/**
  * @brief  根据SDIO_InitTypeDef结构体初始化SDIO外设
  * @param  SDIO_InitStruct : 指向SDIO初始化配置结构体的指针
  * @retval 无
  * @note   配置时钟特性、总线宽度和硬件流控制等关键参数
  */
void SDIO_Init(SDIO_InitTypeDef* SDIO_InitStruct)
{
  uint32_t tmpreg = 0;
    
  /* 参数合法性检查 */
  assert_param(IS_SDIO_CLOCK_EDGE(SDIO_InitStruct->SDIO_ClockEdge));      /* 时钟边沿有效性验证 */
  assert_param(IS_SDIO_CLOCK_BYPASS(SDIO_InitStruct->SDIO_ClockBypass));  /* 时钟旁路模式检查 */
  assert_param(IS_SDIO_CLOCK_POWER_SAVE(SDIO_InitStruct->SDIO_ClockPowerSave)); /* 节能模式检查 */
  assert_param(IS_SDIO_BUS_WIDE(SDIO_InitStruct->SDIO_BusWide));         /* 总线宽度有效性验证 */
  assert_param(IS_SDIO_HARDWARE_FLOW_CONTROL(SDIO_InitStruct->SDIO_HardwareFlowControl)); /* 硬件流控检查 */
   
/* ------------------------- SDIO CLKCR寄存器配置流程 -----------------------*/  
  /* 读取当前CLKCR寄存器值 */
  tmpreg = SDIO->CLKCR;  /* CLKCR寄存器地址：0x40012C04 */
  
  /* 清除需要配置的位域（保留其他位状态） */
  tmpreg &= CLKCR_CLEAR_MASK;  /* 应用预定义清除掩码0xFFFF8100 */
  
  /* 组合新的配置参数 */
  tmpreg |= (SDIO_InitStruct->SDIO_ClockDiv         |  /* 时钟分频系数（0-255） */
             SDIO_InitStruct->SDIO_ClockPowerSave   |  /* 时钟节能模式（禁用/启用） */
             SDIO_InitStruct->SDIO_ClockBypass      |  /* 时钟旁路（使用HCLK或分频） */
             SDIO_InitStruct->SDIO_BusWide          |  /* 总线宽度（1位/4位） */
             SDIO_InitStruct->SDIO_ClockEdge        |  /* 时钟边沿（上升沿/下降沿） */
             SDIO_InitStruct->SDIO_HardwareFlowControl); /* 硬件流控（CRC校验使能） */
  
  /* 将新配置写入CLKCR寄存器 */
  SDIO->CLKCR = tmpreg;  /* 更新时钟控制寄存器 */
}

/**
  * @brief  初始化SDIO_InitTypeDef结构体为默认值
  * @param  SDIO_InitStruct: 待初始化的SDIO配置结构体指针
  * @retval 无
  * @note   默认配置为安全模式：低速1位总线，关闭所有高级功能
  */
void SDIO_StructInit(SDIO_InitTypeDef* SDIO_InitStruct)
{
  /* 设置结构体成员默认值 */
  SDIO_InitStruct->SDIO_ClockDiv = 0x00;          /* 时钟分频系数0（时钟=输入时钟/(2*0)=输入时钟） */
  SDIO_InitStruct->SDIO_ClockEdge = SDIO_ClockEdge_Rising; /* 时钟上升沿采样 */
  SDIO_InitStruct->SDIO_ClockBypass = SDIO_ClockBypass_Disable; /* 禁用时钟旁路（使用分频器） */
  SDIO_InitStruct->SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable; /* 禁用时钟节能模式 */
  SDIO_InitStruct->SDIO_BusWide = SDIO_BusWide_1b;     /* 1位总线模式 */
  SDIO_InitStruct->SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable; /* 禁用硬件流控 */
}
/**
  * @brief SDIO控制器时钟与电源管理及命令发送函数组
  * @note  包含SDIO时钟控制、电源状态管理和命令发送核心功能
  * @warning 发送命令前需确保已配置正确时钟和总线模式
  */

/**
  * @brief  启用或禁用SDIO时钟输出
  * @param  NewState: 时钟新状态（ENABLE/DISABLE）
  * @retval 无
  * @note   通过位带操作直接控制CLKCR寄存器的CLKEN位，实时生效
  */
void SDIO_ClockCmd(FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  /* 确认输入为有效功能状态 */
  
  /* 通过位带地址直接操作CLKEN位（原子操作） */
  *(__IO uint32_t *) CLKCR_CLKEN_BB = (uint32_t)NewState;  /* 0x424600A0地址写入0/1 */
}

/**
  * @brief  设置SDIO控制器电源状态
  * @param  SDIO_PowerState: 电源状态（SDIO_PowerState_OFF/OFF/ON）
  * @retval 无
  * @note   电源关闭将复位所有寄存器，操作前应保存必要配置
  */
void SDIO_SetPowerState(uint32_t SDIO_PowerState)
{
  /* 参数有效性验证 */
  assert_param(IS_SDIO_POWER_STATE(SDIO_PowerState));  /* 检查是否为0x00/0x02/0x03 */
  
  /* 直接写入POWER控制寄存器（地址：0x40012C00） */
  SDIO->POWER = SDIO_PowerState;  /* [1:0]位控制电源状态 */
}

/**
  * @brief  获取当前电源状态
  * @param  无
  * @retval 电源状态（0x00=关闭，0x02=上电中，0x03=运行）
  * @note   返回值需与SDIO_PowerState_xxx宏对比使用
  */
uint32_t SDIO_GetPowerState(void)
{
  /* 读取POWER寄存器并过滤保留位 */
  return (SDIO->POWER & (~PWR_PWRCTRL_MASK));  /* 应用掩码0x00000003 */
}

/* ------------------------- 命令路径状态机管理 ------------------------- */
/**
  * @brief  配置并发送SDIO命令
  * @param  SDIO_CmdInitStruct: 命令配置结构体指针
  * @retval 无
  * @note   命令发送将启动CPSM状态机，需等待操作完成或超时
  */
void SDIO_SendCommand(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct)
{
  uint32_t tmpreg = 0;
  
  /* 参数合法性验证 */
  assert_param(IS_SDIO_CMD_INDEX(SDIO_CmdInitStruct->SDIO_CmdIndex));  /* 命令号0-63 */
  assert_param(IS_SDIO_RESPONSE(SDIO_CmdInitStruct->SDIO_Response));   /* 响应类型检查 */
  assert_param(IS_SDIO_WAIT(SDIO_CmdInitStruct->SDIO_Wait));           /* 等待模式验证 */
  assert_param(IS_SDIO_CPSM(SDIO_CmdInitStruct->SDIO_CPSM));           /* CPSM状态检查 */

/* ------------------------- ARG参数寄存器配置 ------------------------- */
  /* 设置命令参数值（32位） */
  SDIO->ARG = SDIO_CmdInitStruct->SDIO_Argument;  /* ARG寄存器地址：0x40012C08 */

/* ------------------------- CMD命令寄存器配置 ------------------------- */  
  /* 读取当前CMD寄存器值 */
  tmpreg = SDIO->CMD;  /* CMD寄存器地址：0x40012C0C */
  
  /* 清除命令相关控制位 */
  tmpreg &= CMD_CLEAR_MASK;  /* 应用掩码0xFFFFF800，保留[10:0]位 */
  
  /* 组合新配置参数 */
  tmpreg |= (uint32_t)SDIO_CmdInitStruct->SDIO_CmdIndex |  /* 命令索引（0-63） */
            SDIO_CmdInitStruct->SDIO_Response |            /* 响应类型（无/短/长） */
            SDIO_CmdInitStruct->SDIO_Wait |                /* 等待中断/传输完成 */
            SDIO_CmdInitStruct->SDIO_CPSM;                 /* 启动命令路径状态机 */
  
  /* 写入配置并触发命令发送 */
  SDIO->CMD = tmpreg;  /* 写入后CPSM自动启动命令处理 */
}

/**
  * @brief SDIO命令响应处理与数据配置函数组
  * @note  包含命令响应获取、数据结构初始化和数据传输配置功能
  * @warning 数据配置前需确保命令通道已建立正确通信
  */

/**
  * @brief  初始化命令配置结构体为默认值
  * @param  SDIO_CmdInitStruct: 命令配置结构体指针
  * @retval 无
  * @note   默认配置为无效命令，需手动填充有效参数
  */
void SDIO_CmdStructInit(SDIO_CmdInitTypeDef* SDIO_CmdInitStruct)
{
  /* 设置结构体成员默认值 */
  SDIO_CmdInitStruct->SDIO_Argument = 0x00;          /* 命令参数清零 */
  SDIO_CmdInitStruct->SDIO_CmdIndex = 0x00;          /* 命令索引0（无效命令） */
  SDIO_CmdInitStruct->SDIO_Response = SDIO_Response_No; /* 无响应模式 */
  SDIO_CmdInitStruct->SDIO_Wait = SDIO_Wait_No;      /* 不等待任何事件 */
  SDIO_CmdInitStruct->SDIO_CPSM = SDIO_CPSM_Disable; /* 禁用命令路径状态机 */
}

/**
  * @brief  获取最后一次响应对应的命令索引
  * @param  无
  * @retval 命令索引（0-63）
  * @note   用于验证卡是否正确响应最后发送的命令
  */
uint8_t SDIO_GetCommandResponse(void)
{
  /* 读取RESPCMD寄存器（地址：0x40012C10）并截取低6位 */
  return (uint8_t)(SDIO->RESPCMD);  /* 位域[5:0]有效 */
}

/**
  * @brief  读取指定响应寄存器的值
  * @param  SDIO_RESP: 响应寄存器选择（SDIO_RESP1-4）
  * @retval 32位响应数据
  * @note   不同寄存器对应不同阶段的响应数据：
  *         RESP1: 卡状态信息（CMD13）
  *         RESP2: CID寄存器值（CMD2,CMD10）
  *         RESP3: CSD寄存器值（CMD9） 
  *         RESP4: OCR寄存器值（CMD58）
  */
uint32_t SDIO_GetResponse(uint32_t SDIO_RESP)
{
  __IO uint32_t tmp = 0;

  /* 参数有效性验证 */
  assert_param(IS_SDIO_RESP(SDIO_RESP));  /* 检查是否为0x00/0x04/0x08/0x0C */
  
  /* 计算响应寄存器物理地址 */
  tmp = SDIO_RESP_ADDR + SDIO_RESP;  /* SDIO_RESP_ADDR=0x40012C14 */
  
  /* 返回对应寄存器的值 */
  return (*(__IO uint32_t *) tmp);   /* 通过指针访问寄存器 */
}

/* ------------------------- 数据路径状态机管理 ------------------------- */

/**
  * @brief  配置SDIO数据传输参数
  * @param  SDIO_DataInitStruct: 数据传输配置结构体指针
  * @retval 无
  * @note   配置后DPSM自动启动，需等待传输完成或超时
  */
void SDIO_DataConfig(SDIO_DataInitTypeDef* SDIO_DataInitStruct)
{
  uint32_t tmpreg = 0;
  
  /* 参数合法性验证 */
  assert_param(IS_SDIO_DATA_LENGTH(SDIO_DataInitStruct->SDIO_DataLength));  /* 数据长度1-0x1FFFFFF */
  assert_param(IS_SDIO_BLOCK_SIZE(SDIO_DataInitStruct->SDIO_DataBlockSize)); /* 块大小（0-9对应2^N字节） */
  assert_param(IS_SDIO_TRANSFER_DIR(SDIO_DataInitStruct->SDIO_TransferDir));  /* 传输方向（读/写） */
  assert_param(IS_SDIO_TRANSFER_MODE(SDIO_DataInitStruct->SDIO_TransferMode)); /* 模式（块/流） */
  assert_param(IS_SDIO_DPSM(SDIO_DataInitStruct->SDIO_DPSM));          /* DPSM使能检查 */

/* ------------------------- DTIMER超时配置 ------------------------- */
  /* 设置数据传输超时值（单位：SDIO_CK周期数） */
  SDIO->DTIMER = SDIO_DataInitStruct->SDIO_DataTimeOut;  /* 地址：0x40012C24 */

/* ------------------------- DLEN数据长度配置 ------------------------- */
  /* 设置总传输数据字节数 */
  SDIO->DLEN = SDIO_DataInitStruct->SDIO_DataLength;     /* 地址：0x40012C28 */

/* ------------------------- DCTRL控制寄存器配置 ------------------------- */  
  /* 读取当前DCTRL寄存器值 */
  tmpreg = SDIO->DCTRL;  /* 地址：0x40012C2C */
  
  /* 清除数据相关控制位 */
  tmpreg &= DCTRL_CLEAR_MASK;  /* 应用掩码0xFFFFFF08 */
  
  /* 组合新配置参数 */
  tmpreg |= (uint32_t)SDIO_DataInitStruct->SDIO_DataBlockSize | /* 块大小（如512字节） */
            SDIO_DataInitStruct->SDIO_TransferDir |       /* 传输方向（控制器→卡或卡→控制器） */
            SDIO_DataInitStruct->SDIO_TransferMode |      /* 块传输或流传输模式 */
            SDIO_DataInitStruct->SDIO_DPSM;              /* 启用数据路径状态机 */
  
  /* 写入配置并启动传输 */
  SDIO->DCTRL = tmpreg;  /* 写入后DPSM自动开始数据传输 */
}
/**
  * @brief SDIO数据传输配置与状态读取函数组
  * @note  包含数据传输结构体初始化、数据计数器获取、FIFO读写及FIFO计数获取功能
  * @warning 操作前需确保已配置正确时钟和总线模式
  */

/**
  * @brief  初始化数据传输配置结构体为默认值
  * @param  SDIO_DataInitStruct: 数据传输配置结构体指针
  * @retval 无
  * @note   默认配置为无效传输，需手动填充有效参数
  */
void SDIO_DataStructInit(SDIO_DataInitTypeDef* SDIO_DataInitStruct)
{
  /* 数据传输配置结构体成员默认值 */
  SDIO_DataInitStruct->SDIO_DataTimeOut = 0xFFFFFFFF;  /* 数据超时设置为最大值 */
  SDIO_DataInitStruct->SDIO_DataLength = 0x00;         /* 数据长度清零 */
  SDIO_DataInitStruct->SDIO_DataBlockSize = SDIO_DataBlockSize_1b;  /* 块大小默认1字节 */
  SDIO_DataInitStruct->SDIO_TransferDir = SDIO_TransferDir_ToCard;  /* 默认传输方向控制器→卡 */
  SDIO_DataInitStruct->SDIO_TransferMode = SDIO_TransferMode_Block;  /* 默认传输模式为块传输 */
  SDIO_DataInitStruct->SDIO_DPSM = SDIO_DPSM_Disable;  /* 禁用数据路径状态机 */
}

/**
  * @brief  获取剩余待传输的数据字节数
  * @param  无
  * @retval 剩余待传输的数据字节数
  * @note   用于监控数据传输进度，确保传输完成
  */
uint32_t SDIO_GetDataCounter(void)
{ 
  /* 读取数据计数器寄存器（地址：0x40012C30） */
  return SDIO->DCOUNT;  /* 返回剩余字节数 */
}

/**
  * @brief  从Rx FIFO读取一个数据字
  * @param  无
  * @retval 接收到的数据字
  * @note   用于接收从SD卡传输的数据
  */
uint32_t SDIO_ReadData(void)
{ 
  /* 读取接收FIFO寄存器（地址：0x40012C80） */
  return SDIO->FIFO;  /* 返回一个32位的数据字 */
}

/**
  * @brief  向Tx FIFO写入一个数据字
  * @param  Data: 待写入的32位数据字
  * @retval 无
  * @note   用于向SD卡发送数据
  */
void SDIO_WriteData(uint32_t Data)
{ 
  /* 写入发送FIFO寄存器（地址：0x40012C80） */
  SDIO->FIFO = Data;  /* 写入一个32位的数据字 */
}

/**
  * @brief  获取FIFO中剩余的数据字数
  * @param  无
  * @retval FIFO中剩余的数据字数
  * @note   用于判断FIFO的状态，避免溢出或读取空数据
  */
uint32_t SDIO_GetFIFOCount(void)
{ 
  /* 读取FIFO计数寄存器（地址：0x40012C48） */
  return SDIO->FIFOCNT;  /* 返回FIFO中的剩余字数 */
}


/**
  * @}
  */

/** @defgroup SDIO_Group4 SDIO IO Cards mode management functions
 *  @brief   SDIO IO Cards mode management functions
 *
@verbatim   
 ===============================================================================
               ##### SDIO IO Cards mode management functions #####
 ===============================================================================  

  This section provide functions allowing to program and read the SDIO IO Cards.

@endverbatim
  * @{
  */

/**
  * @brief  Starts the SD I/O Read Wait operation.	
  * @param  NewState: new state of the Start SDIO Read Wait operation. 
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief SDIO读等待和模式操作管理函数组
  * @note  包含启动/停止读等待、设置读等待模式、启用/禁用SDIO模式及发送暂停命令等功能
  * @warning 操作前需确保已配置正确时钟和总线模式
  */

/**
  * @brief  启动或停止SDIO读等待操作
  * @param  NewState: 新状态（ENABLE/DISABLE）
  * @retval 无
  * @note   通过位带操作直接控制DCTRL寄存器的RWSTART位
  */
void SDIO_StartSDIOReadWait(FunctionalState NewState)
{ 
  /* 参数合法性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  /* 确认输入为有效功能状态 */
  
  /* 通过位带操作直接设置RWSTART位（0x40012C2C + 0x0C * 4 = 0x424605AC） */
  *(__IO uint32_t *) DCTRL_RWSTART_BB = (uint32_t) NewState;  /* 位带地址：0x424605AC */
}

/**
  * @brief  停止或启动SDIO读等待操作
  * @param  NewState: 新状态（ENABLE/DISABLE）
  * @retval 无
  * @note   通过位带操作直接控制DCTRL寄存器的RWSTOP位
  */
void SDIO_StopSDIOReadWait(FunctionalState NewState)
{ 
  /* 参数合法性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  /* 确认输入为有效功能状态 */
  
  /* 通过位带操作直接设置RWSTOP位（0x40012C2C + 0x0D * 4 = 0x424605B0） */
  *(__IO uint32_t *) DCTRL_RWSTOP_BB = (uint32_t) NewState;  /* 位带地址：0x424605B0 */
}

/**
  * @brief  设置SDIO读等待模式
  * @param  SDIO_ReadWaitMode: 读等待模式（SDIO_ReadWaitMode_CLK/DATA2）
  * @retval 无
  * @note   通过位带操作直接控制DCTRL寄存器的RWMOD位
  */
void SDIO_SetSDIOReadWaitMode(uint32_t SDIO_ReadWaitMode)
{
  /* 参数合法性验证 */
  assert_param(IS_SDIO_READWAIT_MODE(SDIO_ReadWaitMode));  /* 确认输入为有效读等待模式 */
  
  /* 通过位带操作直接设置RWMOD位（0x40012C2C + 0x0A * 4 = 0x424605AC） */
  *(__IO uint32_t *) DCTRL_RWMOD_BB = SDIO_ReadWaitMode;  /* 位带地址：0x424605AC */
  /* RWMOD位设置：
     - SDIO_ReadWaitMode_CLK: 使用停止SDIOCLK控制读等待
     - SDIO_ReadWaitMode_DATA2: 使用SDIO_DATA2信号控制读等待 */
}

/**
  * @brief  启用或禁用SDIO模式操作
  * @param  NewState: 新状态（ENABLE/DISABLE）
  * @retval 无
  * @note   通过位带操作直接控制DCTRL寄存器的SDIOEN位
  */
void SDIO_SetSDIOOperation(FunctionalState NewState)
{ 
  /* 参数合法性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  /* 确认输入为有效功能状态 */
  
  /* 通过位带操作直接设置SDIOEN位（0x40012C2C + 0x0B * 4 = 0x424605B4） */
  *(__IO uint32_t *) DCTRL_SDIOEN_BB = (uint32_t)NewState;  /* 位带地址：0x424605B4 */
}

/**
  * @brief  发送或停止发送SDIO暂停命令
  * @param  NewState: 新状态（ENABLE/DISABLE）
  * @retval 无
  * @note   通过位带操作直接控制CMD寄存器的SDIOSUSPEND位
  */
void SDIO_SendSDIOSuspendCmd(FunctionalState NewState)
{ 
  /* 参数合法性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));  /* 确认输入为有效功能状态 */
  
  /* 通过位带操作直接设置SDIOSUSPEND位（0x40012C0C + 0x0B * 4 = 0x424600F8） */
  *(__IO uint32_t *) CMD_SDIOSUSPEND_BB = (uint32_t)NewState;  /* 位带地址：0x424600F8 */
}

/**
  * @}
  */


/**
  * @}
  */

/** @defgroup SDIO_Group5 CE-ATA mode management functions
 *  @brief   CE-ATA mode management functions
 *
@verbatim   
 ===============================================================================
                  ##### CE-ATA mode management functions #####
 ===============================================================================  

  This section provide functions allowing to program and read the CE-ATA card.

@endverbatim
  * @{
  */
/**
  * @brief  SDIO命令完成信号控制函数
  * @param  NewState: 命令完成信号的新状态
  *         ENABLE  - 使能命令完成信号
  *         DISABLE - 禁用命令完成信号
  * @retval 无
  * @note   操作CMD寄存器ENCMDCOMPL位（位带别名CMD_ENCMDCOMPL_BB）
  *         该信号用于指示命令序列的完成状态
  *         当使能时，命令完成后会产生硬件信号
  */
void SDIO_CommandCompletionCmd(FunctionalState NewState)
{ 
  /* 检查参数合法性 */
  /* 使用断言验证输入参数是否为合法功能状态（ENABLE/DISABLE） */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 通过位带操作直接访问CMD寄存器ENCMDCOMPL位 */
  /* 将NewState的值写入命令完成使能位 */
  *(__IO uint32_t *) CMD_ENCMDCOMPL_BB = (uint32_t)NewState;
}

/**
  * @brief  CE-ATA中断控制函数
  * @param  NewState: CE-ATA中断的新状态
  *         ENABLE  - 使能CE-ATA中断
  *         DISABLE - 禁用CE-ATA中断
  * @retval 无
  * @note   操作CMD寄存器NIEN位（位带别名CMD_NIEN_BB）
  *         该中断用于CE-ATA设备的异步事件通知
  *         注意寄存器位是反向逻辑：0=允许中断，1=禁止中断
  */
void SDIO_CEATAITCmd(FunctionalState NewState)
{ 
  /* 检查参数合法性 */
  /* 验证输入参数是否为合法功能状态 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 通过位带操作设置中断使能位 */
  /* 使用逻辑取反操作是因为该位是反向逻辑：
     NewState=ENABLE -> 实际写入0（允许中断）
     NewState=DISABLE -> 实际写入1（禁止中断） */
  *(__IO uint32_t *) CMD_NIEN_BB = (uint32_t)((~((uint32_t)NewState)) & ((uint32_t)0x1));
}

/**
  * @brief  CE-ATA命令发送控制函数
  * @param  NewState: CE-ATA命令发送控制状态
  *         ENABLE  - 触发发送CE-ATA命令（CMD61）
  *         DISABLE - 无操作
  * @retval 无
  * @note   操作CMD寄存器ATACMD位（位带别名CMD_ATACMD_BB）
  *         用于启动CE-ATA设备的特殊命令协议
  *         当设置为ENABLE时，硬件会自动发送CMD61命令
  */
void SDIO_SendCEATACmd(FunctionalState NewState)
{ 
  /* 检查参数合法性 */
  /* 验证输入参数是否为合法功能状态 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 通过位带操作设置ATA命令触发位 */
  /* 当该位被置1时，硬件将自动发送CMD61命令 */
  *(__IO uint32_t *) CMD_ATACMD_BB = (uint32_t)NewState;
}

/**
  * @}
  */

/** @defgroup SDIO_Group6 DMA transfers management functions
 *  @brief   DMA transfers management functions
 *
@verbatim   
 ===============================================================================
                  ##### DMA transfers management functions #####
 ===============================================================================  

  This section provide functions allowing to program SDIO DMA transfer.

@endverbatim
  * @{
  */
/**
  * @brief  SDIO DMA请求使能控制函数
  * @param  NewState: DMA请求的新状态
  *         ENABLE  - 使能SDIO DMA传输请求
  *         DISABLE - 禁用SDIO DMA传输请求
  * @retval 无
  * @note   操作DCTRL寄存器DMAEN位（位带别名DCTRL_DMAEN_BB）
  *         当使能时，SDIO控制器在数据传输时会自动产生DMA请求
  *         该功能用于启用DMA控制器进行批量数据传输
  * @warning 使用前需确保已正确配置DMA控制器相关参数
  */
void SDIO_DMACmd(FunctionalState NewState)
{
  /* 检查参数合法性 */
  /* 使用断言验证输入参数是否为有效功能状态（ENABLE/DISABLE） */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 通过位带操作直接设置DMA使能位 */
  /* 将NewState值写入DCTRL寄存器的DMAEN位（位3） */
  /* 位带别名DCTRL_DMAEN_BB提供原子级位操作能力 */
  *(__IO uint32_t *) DCTRL_DMAEN_BB = (uint32_t)NewState;
}

/**
  * @}
  */

/** @defgroup SDIO_Group7 Interrupts and flags management functions
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
  * @brief  Enables or disables the SDIO interrupts.
  * @param  SDIO_IT: specifies the SDIO interrupt sources to be enabled or disabled.
  *          This parameter can be one or a combination of the following values:
  *            @arg SDIO_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDIO_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDIO_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDIO_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDIO_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDIO_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDIO_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDIO_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDIO_IT_STBITERR: Start bit not detected on all data signals in wide 
  *                                   bus mode interrupt
  *            @arg SDIO_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDIO_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDIO_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDIO_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDIO_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDIO_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDIO_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDIO_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDIO_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDIO_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDIO_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDIO_IT_SDIOIT:   SD I/O interrupt received interrupt
  *            @arg SDIO_IT_CEATAEND: CE-ATA command completion signal received for CMD61 interrupt
  * @param  NewState: new state of the specified SDIO interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None 
  */
/**
  * @brief  SDIO中断配置函数
  * @param  SDIO_IT: 需要配置的SDIO中断源
  *         可取值参见IS_SDIO_IT宏定义的多种中断类型组合，例如：
  *         SDIO_IT_RXOVERR  - 接收溢出中断
  *         SDIO_IT_DCRCFAIL - 数据块CRC校验失败中断
  *         SDIO_IT_CMDREND  - 命令响应接收完成中断
  * @param  NewState: 中断的新状态
  *         ENABLE  - 使能指定中断
  *         DISABLE - 禁用指定中断
  * @retval 无
  * @note   操作SDIO的MASK寄存器（中断屏蔽寄存器）
  *         该寄存器控制哪些中断事件可以触发中断请求
  *         支持多个中断源的组合配置（按位或操作）
  * @warning 修改中断配置前建议先禁用全局中断
  */
void SDIO_ITConfig(uint32_t SDIO_IT, FunctionalState NewState)
{
  /* 检查参数合法性 */
  /* 验证中断类型参数是否符合SDIO_IT_xx定义范围 */
  assert_param(IS_SDIO_IT(SDIO_IT));
  /* 验证状态参数是否为合法使能/禁用状态 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 根据新状态执行中断使能/禁用操作 */
  if (NewState != DISABLE)
  {
    /* 使能指定中断：通过或操作设置MASK寄存器对应位 */
    /* 支持同时使能多个中断源（SDIO_IT可以是位掩码组合） */
    SDIO->MASK |= SDIO_IT;
  }
  else
  {
    /* 禁用指定中断：通过与取反操作清除MASK寄存器对应位 */
    /* 保持其他中断位的状态不受影响 */
    SDIO->MASK &= ~SDIO_IT;
  } 
}

/**
  * @brief  Checks whether the specified SDIO flag is set or not.
  * @param  SDIO_FLAG: specifies the flag to check. 
  *          This parameter can be one of the following values:
  *            @arg SDIO_FLAG_CCRCFAIL: Command response received (CRC check failed)
  *            @arg SDIO_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
  *            @arg SDIO_FLAG_CTIMEOUT: Command response timeout
  *            @arg SDIO_FLAG_DTIMEOUT: Data timeout
  *            @arg SDIO_FLAG_TXUNDERR: Transmit FIFO underrun error
  *            @arg SDIO_FLAG_RXOVERR:  Received FIFO overrun error
  *            @arg SDIO_FLAG_CMDREND:  Command response received (CRC check passed)
  *            @arg SDIO_FLAG_CMDSENT:  Command sent (no response required)
  *            @arg SDIO_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
  *            @arg SDIO_FLAG_STBITERR: Start bit not detected on all data signals in wide bus mode.
  *            @arg SDIO_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
  *            @arg SDIO_FLAG_CMDACT:   Command transfer in progress
  *            @arg SDIO_FLAG_TXACT:    Data transmit in progress
  *            @arg SDIO_FLAG_RXACT:    Data receive in progress
  *            @arg SDIO_FLAG_TXFIFOHE: Transmit FIFO Half Empty
  *            @arg SDIO_FLAG_RXFIFOHF: Receive FIFO Half Full
  *            @arg SDIO_FLAG_TXFIFOF:  Transmit FIFO full
  *            @arg SDIO_FLAG_RXFIFOF:  Receive FIFO full
  *            @arg SDIO_FLAG_TXFIFOE:  Transmit FIFO empty
  *            @arg SDIO_FLAG_RXFIFOE:  Receive FIFO empty
  *            @arg SDIO_FLAG_TXDAVL:   Data available in transmit FIFO
  *            @arg SDIO_FLAG_RXDAVL:   Data available in receive FIFO
  *            @arg SDIO_FLAG_SDIOIT:   SD I/O interrupt received
  *            @arg SDIO_FLAG_CEATAEND: CE-ATA command completion signal received for CMD61
  * @retval The new state of SDIO_FLAG (SET or RESET).
  */
/**
  * @brief  SDIO状态标志获取函数
  * @param  SDIO_FLAG: 需要查询的状态标志
  *         可取值参见IS_SDIO_FLAG宏定义的多种标志类型，例如：
  *         SDIO_FLAG_CCRCFAIL - 命令响应CRC校验失败
  *         SDIO_FLAG_DTIMEOUT - 数据超时状态
  *         SDIO_FLAG_DATAEND  - 数据传输完成标志
  * @retval 标志位当前状态
  *         SET   - 标志位已置位（事件发生）
  *         RESET - 标志位未置位（事件未发生）
  * @note   操作SDIO的STA寄存器（状态寄存器）
  *         该寄存器包含所有硬件状态标志位
  *         部分标志位需要手动清除（参考SDIO_ClearFlag函数）
  * @warning 不支持同时查询多个标志位，建议单独检查每个标志
  */
FlagStatus SDIO_GetFlagStatus(uint32_t SDIO_FLAG)
{ 
  FlagStatus bitstatus = RESET;  // 初始化返回值为未置位状态
  
  /* 检查参数合法性 */
  /* 验证输入标志位参数是否符合SDIO_FLAG_xx定义范围 */
  assert_param(IS_SDIO_FLAG(SDIO_FLAG));
  
  /* 通过位与操作检测指定标志位状态 */
  /* 当STA寄存器的对应位为1时表示标志置位 */
  if ((SDIO->STA & SDIO_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;  // 检测到标志置位，返回SET
  }
  else
  {
    bitstatus = RESET;  // 未检测到标志置位，返回RESET
  }
  return bitstatus;  // 返回最终检测结果
}

/**
  * @brief  Clears the SDIO's pending flags.
  * @param  SDIO_FLAG: specifies the flag to clear.  
  *          This parameter can be one or a combination of the following values:
  *            @arg SDIO_FLAG_CCRCFAIL: Command response received (CRC check failed)
  *            @arg SDIO_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
  *            @arg SDIO_FLAG_CTIMEOUT: Command response timeout
  *            @arg SDIO_FLAG_DTIMEOUT: Data timeout
  *            @arg SDIO_FLAG_TXUNDERR: Transmit FIFO underrun error
  *            @arg SDIO_FLAG_RXOVERR:  Received FIFO overrun error
  *            @arg SDIO_FLAG_CMDREND:  Command response received (CRC check passed)
  *            @arg SDIO_FLAG_CMDSENT:  Command sent (no response required)
  *            @arg SDIO_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
  *            @arg SDIO_FLAG_STBITERR: Start bit not detected on all data signals in wide bus mode
  *            @arg SDIO_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
  *            @arg SDIO_FLAG_SDIOIT:   SD I/O interrupt received
  *            @arg SDIO_FLAG_CEATAEND: CE-ATA command completion signal received for CMD61
  * @retval None
  */
/**
  * @brief  SDIO状态标志清除函数
  * @param  SDIO_FLAG: 需要清除的状态标志
  *         可取值参见IS_SDIO_CLEAR_FLAG宏定义的允许清除标志，例如：
  *         SDIO_FLAG_CCRCFAIL - 清除命令响应CRC失败标志
  *         SDIO_FLAG_DTIMEOUT - 清除数据超时标志
  * @retval 无
  * @note   操作SDIO的ICR寄存器（中断清除寄存器）
  *         写入1到对应位可清除STA寄存器中的状态标志
  *         只能清除允许手动清除的标志位
  * @warning 该操作具有原子性，会立即清除指定标志
  */
void SDIO_ClearFlag(uint32_t SDIO_FLAG)
{ 
  /* 检查参数合法性 */
  /* 验证输入标志是否为可清除类型（部分标志为只读状态） */
  assert_param(IS_SDIO_CLEAR_FLAG(SDIO_FLAG));
   
  /* 通过写ICR寄存器清除指定标志 */
  /* 写入1到对应位实现标志清除操作 */
  SDIO->ICR = SDIO_FLAG;
}

/**
  * @brief  SDIO中断状态查询函数
  * @param  SDIO_IT: 需要查询的中断类型
  *         支持的中断类型包括：
  *         SDIO_IT_CCRCFAIL  - 命令CRC校验失败中断
  *         SDIO_IT_DATAEND   - 数据传输完成中断
  *         SDIO_IT_RXFIFOHF  - 接收FIFO半满中断
  *         其他详见IS_SDIO_GET_IT宏定义
  * @retval 中断状态
  *         SET   - 中断已触发且未被处理
  *         RESET - 中断未触发或已被处理
  * @note   实际检测STA寄存器的中断挂起状态
  *         需配合SDIO_ITConfig使能对应中断才能触发
  * @warning 该检测不区分中断使能状态，仅反映实际发生状态
  */
ITStatus SDIO_GetITStatus(uint32_t SDIO_IT)
{ 
  ITStatus bitstatus = RESET;  // 初始化返回状态
  
  /* 检查参数合法性 */
  /* 验证中断类型参数是否符合SDIO_IT_xx定义范围 */
  assert_param(IS_SDIO_GET_IT(SDIO_IT));

  /* 检测STA寄存器中指定中断位的挂起状态 */
  if ((SDIO->STA & SDIO_IT) != (uint32_t)RESET)  
  {
    bitstatus = SET;  // 检测到中断挂起
  }
  else
  {
    bitstatus = RESET;  // 未检测到中断挂起
  }
  return bitstatus;
}

/**
  * @brief  Clears the SDIO's interrupt pending bits.
  * @param  SDIO_IT: specifies the interrupt pending bit to clear. 
  *          This parameter can be one or a combination of the following values:
  *            @arg SDIO_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDIO_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDIO_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDIO_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDIO_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDIO_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDIO_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDIO_IT_DATAEND:  Data end (data counter, SDIO_DCOUNT, is zero) interrupt
  *            @arg SDIO_IT_STBITERR: Start bit not detected on all data signals in wide 
  *                                   bus mode interrupt
  *            @arg SDIO_IT_SDIOIT:   SD I/O interrupt received interrupt
  *            @arg SDIO_IT_CEATAEND: CE-ATA command completion signal received for CMD61
  * @retval None
  */
/**
  * @brief  SDIO中断挂起标志清除函数
  * @param  SDIO_IT: 需要清除的中断类型
  *         可取值参见IS_SDIO_CLEAR_IT宏定义的允许清除中断，例如：
  *         SDIO_IT_DATAEND  - 清除数据传输完成中断标志
  *         SDIO_IT_CMDREND  - 清除命令响应接收完成中断标志
  *         SDIO_IT_RXFIFOE  - 清除接收FIFO空中断标志
  * @retval 无
  * @note   操作SDIO的ICR寄存器（中断清除寄存器）
  *         通过写入指定中断标志位实现原子性清除操作
  *         只能在中断服务程序中使用以确保实时性
  * @warning 某些中断标志由硬件自动清除（如超时标志），无需手动操作
  */
void SDIO_ClearITPendingBit(uint32_t SDIO_IT)
{ 
  /* 检查参数合法性 */
  /* 验证输入中断类型是否为允许清除的类型（过滤不可写标志） */
  assert_param(IS_SDIO_CLEAR_IT(SDIO_IT));
   
  /* 通过写ICR寄存器清除中断标志 */
  /* 写入操作具有立即生效特性，不会影响其他标志位状态 */
  SDIO->ICR = SDIO_IT;
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

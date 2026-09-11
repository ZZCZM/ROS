/**
  ******************************************************************************
  * @file    misc.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides all the miscellaneous firmware functions (add-on
  *          to CMSIS functions).
  *          
  *  @verbatim   
  *                               
  *          ===================================================================      
  *                        How to configure Interrupts using driver 
  *          ===================================================================      
  * 
  *            This section provide functions allowing to configure the NVIC interrupts (IRQ).
  *            The Cortex-M4 exceptions are managed by CMSIS functions.
  *
  *            1. Configure the NVIC Priority Grouping using NVIC_PriorityGroupConfig()
  *                function according to the following table.
 
  *  The table below gives the allowed values of the pre-emption priority and subpriority according
  *  to the Priority Grouping configuration performed by NVIC_PriorityGroupConfig function
  *    ==========================================================================================================================
  *      NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  |       Description
  *    ==========================================================================================================================
  *     NVIC_PriorityGroup_0  |                0                  |            0-15             | 0 bits for pre-emption priority
  *                           |                                   |                             | 4 bits for subpriority
  *    --------------------------------------------------------------------------------------------------------------------------
  *     NVIC_PriorityGroup_1  |                0-1                |            0-7              | 1 bits for pre-emption priority
  *                           |                                   |                             | 3 bits for subpriority
  *    --------------------------------------------------------------------------------------------------------------------------    
  *     NVIC_PriorityGroup_2  |                0-3                |            0-3              | 2 bits for pre-emption priority
  *                           |                                   |                             | 2 bits for subpriority
  *    --------------------------------------------------------------------------------------------------------------------------    
  *     NVIC_PriorityGroup_3  |                0-7                |            0-1              | 3 bits for pre-emption priority
  *                           |                                   |                             | 1 bits for subpriority
  *    --------------------------------------------------------------------------------------------------------------------------    
  *     NVIC_PriorityGroup_4  |                0-15               |            0                | 4 bits for pre-emption priority
  *                           |                                   |                             | 0 bits for subpriority                       
  *    ==========================================================================================================================     
  *
  *            2. Enable and Configure the priority of the selected IRQ Channels using NVIC_Init()  
  *
  * @note  When the NVIC_PriorityGroup_0 is selected, IRQ pre-emption is no more possible. 
  *        The pending IRQ priority will be managed only by the subpriority.
  *
  * @note  IRQ priority order (sorted by highest to lowest priority):
  *         - Lowest pre-emption priority
  *         - Lowest subpriority
  *         - Lowest hardware priority (IRQ number)
  *
  *  @endverbatim
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
#include "misc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup MISC 
  * @brief MISC driver modules
  * @{
  */

/**
 * @文件说明 MISC模块关键寄存器操作宏定义
 * @功能   提供系统控制寄存器操作所需的安全访问密钥定义
 * @作用   用于安全写入系统关键寄存器时的验证密钥组合
 * @注意   该宏用于防止意外修改核心寄存器，必须与有效配置组合使用
 *         具体应用场景包括但不限于：
 *         - 系统复位控制
 *         - 中断优先级分组配置
 *         - 向量表重定位
 */

/* 模块内部类型定义区 -----------------------------------------------------
 * 此处通常存放模块内使用的结构体、枚举等类型定义
 * 当前文件暂时未定义私有类型 */

/* 模块内部宏定义区 -------------------------------------------------------
 * 存放硬件寄存器操作相关的位掩码和配置参数 */
#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000) 
/* ARM Cortex-M 应用中断/复位控制寄存器(AIRCR)访问密钥
 * 该掩码用于组合写入AIRCR寄存器时的验证密钥与配置参数：
 * [31:16] 必须为0x05FA作为写保护密钥 (VECTKEY字段)
 * [15:0]  用于实际配置位(如优先级分组、系统复位请求等)
 * 使用示例：SCB->AIRCR = AIRCR_VECTKEY_MASK | 其他配置参数
 */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup MISC_Private_Functions
  * @{
  */

/**
  * @brief  [函数功能] 配置中断优先级分组方式（设置抢占优先级和响应优先级的位数分配）
  * @param  NVIC_PriorityGroup: 指定优先级分组方式
  * @details 该函数通过配置SCB->AIRCR寄存器的PRIGROUP[10:8]位域，决定：
  *           - 中断发生时抢占优先级和子优先级的位数分配方案
  *           - 影响中断嵌套能力和优先级解析方式
  * 
  *          参数可选值及对应配置：
  *          +----------------------+---------------------+-------------------+
  *          | 参数值               | 抢占优先级位数      | 子优先级位数      |
  *          +----------------------+---------------------+-------------------+
  *          | NVIC_PriorityGroup_0 | 0位（无抢占）       | 4位（16级）       |
  *          | NVIC_PriorityGroup_1 | 1位（2级）          | 3位（8级）        |
  *          | NVIC_PriorityGroup_2 | 2位（4级）          | 2位（4级）        |
  *          | NVIC_PriorityGroup_3 | 3位（8级）          | 1位（2级）        |
  *          | NVIC_PriorityGroup_4 | 4位（16级）         | 0位（无子优先级） |
  *          +----------------------+---------------------+-------------------+
  * 
  * @note 重要说明：
  *       1. 当选择NVIC_PriorityGroup_0时，系统将禁用抢占优先级，仅通过子优先级管理中断
  *       2. 该配置影响整个系统的中断优先级处理策略，通常应在系统初始化阶段设置
  *       3. 修改分组方式会改变现有中断优先级的解析方式，建议初始化后不要随意更改
  *       4. AIRCR寄存器写入需要配合VECTKEY验证密钥（通过AIRCR_VECTKEY_MASK实现）
  * 
  * @retval 无返回值
  */
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
  /* 参数有效性检查：确保输入参数在合法范围内 */
  /* IS_NVIC_PRIORITY_GROUP宏验证参数是否为预定义的5种分组方式之一 */
  assert_param(IS_NVIC_PRIORITY_GROUP(NVIC_PriorityGroup));

  /* 配置系统控制块(SCB)的应用程序中断及复位控制寄存器(AIRCR) */
  /* 操作说明：
     1. AIRCR_VECTKEY_MASK（0x05FA0000）提供写入密钥，确保安全写入操作
     2. 使用位或操作将密钥与优先级分组参数组合
     3. 写入后：
        - PRIGROUP[10:8]位域被设置为指定分组方式
        - 同时会复位系统（当包含SYSRESETREQ位时），此处仅修改优先级设置
  */
  SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup;
}

/**
  * @brief  嵌套向量中断控制器(NVIC)初始化函数
  * @功能   根据配置参数初始化中断通道，设置抢占优先级和子优先级
  *         或启用/禁用指定中断通道
  * @注意   使用前必须调用 NVIC_PriorityGroupConfig() 设置优先级分组
  *         该函数直接影响处理器中断控制器的硬件寄存器
  * @参数   NVIC_InitStruct: 包含以下配置参数的结构体指针:
  *         - NVIC_IRQChannel: 中断通道标识(如 USART1_IRQn)
  *         - NVIC_IRQChannelPreemptionPriority: 0-15 抢占优先级
  *         - NVIC_IRQChannelSubPriority: 0-15 子优先级
  *         - NVIC_IRQChannelCmd: 中断使能开关(ENABLE/DISABLE)
  * @实现   根据优先级分组计算实际优先级数值，配置IP(中断优先级)寄存器
  *         通过ISER/ICER寄存器组控制中断通道的使能状态
  */
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct)
{
  // 初始化优先级计算临时变量
  uint8_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
  
  /* 参数有效性验证 */
  assert_param(IS_FUNCTIONAL_STATE(NVIC_InitStruct->NVIC_IRQChannelCmd));
  assert_param(IS_NVIC_PREEMPTION_PRIORITY(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority));  
  assert_param(IS_NVIC_SUB_PRIORITY(NVIC_InitStruct->NVIC_IRQChannelSubPriority));
    
  if (NVIC_InitStruct->NVIC_IRQChannelCmd != DISABLE)
  {
    /* 中断优先级计算流程 ---------------------------------------------------*/
    // 获取当前优先级分组设置（从SCB->AIRCR寄存器提取优先级分组位）
    tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700))>> 0x08;
    // 计算优先级位移量（根据分组确定抢占优先级位宽）
    tmppre = (0x4 - tmppriority);
    // 生成子优先级掩码（0x0F右移得到有效位掩码）
    tmpsub = tmpsub >> tmppriority;

    // 组合抢占优先级和子优先级到4位字段
    tmppriority = NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << tmppre;
    tmppriority |=  (uint8_t)(NVIC_InitStruct->NVIC_IRQChannelSubPriority & tmpsub);
        
    // 将4位优先级数值左移4位对齐到寄存器高4位（NVIC优先级寄存器使用高4位）    
    tmppriority = tmppriority << 0x04;
        
    // 写入中断优先级寄存器（IP数组每个元素对应一个中断通道）
    NVIC->IP[NVIC_InitStruct->NVIC_IRQChannel] = tmppriority;
    
    /* 中断使能操作 -------------------------------------------------------*/
    // 计算ISER寄存器索引（每32位寄存器管理32个中断通道）
    // 右移5位相当于除以32，获取寄存器数组下标
    // 左移操作设置对应位来使能中断（1<<(channel%32)）
    NVIC->ISER[NVIC_InitStruct->NVIC_IRQChannel >> 0x05] =
      (uint32_t)0x01 << (NVIC_InitStruct->NVIC_IRQChannel & (uint8_t)0x1F);
  }
  else
  {
    /* 中断禁用操作 -------------------------------------------------------*/
    // 使用ICER寄存器清除对应中断使能位
    // 操作方式与ISER相同，但作用为禁用中断
    NVIC->ICER[NVIC_InitStruct->NVIC_IRQChannel >> 0x05] =
      (uint32_t)0x01 << (NVIC_InitStruct->NVIC_IRQChannel & (uint8_t)0x1F);
  }
}

/**
  * @brief  设置向量表基地址及偏移量
  * @功能   配置Cortex-M处理器的异常/中断向量表位置
  * @参数   NVIC_VectTab: 向量表存储位置
  *         @arg NVIC_VectTab_RAM  内部SRAM
  *         @arg NVIC_VectTab_FLASH 内部FLASH
  *         Offset: 基地址偏移量（需为0x200整数倍）
  * @实现   通过SCB->VTOR寄存器组合基地址与偏移量
  * @注意   偏移量必须与0x1FFFFF80进行掩码操作确保地址对齐
  */
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{ 
  /* 参数有效性验证 */
  assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));  // 检查向量表位置参数合法性
  assert_param(IS_NVIC_OFFSET(Offset));          // 验证偏移量是否满足对齐要求

  /* 组合基地址与偏移量写入VTOR寄存器
   * 0x1FFFFF80掩码作用：
   * - 保留bit7-31有效地址位
   * - 强制偏移量对齐到0x80边界（满足Cortex-M向量表对齐要求） */
  SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}

/**
  * @brief  配置系统低功耗模式参数
  * @功能   控制系统进入低功耗模式的条件
  * @参数   LowPowerMode: 低功耗模式选择
  *         @arg NVIC_LP_SEVONPEND  挂起事件唤醒
  *         @arg NVIC_LP_SLEEPDEEP  深度睡眠模式
  *         @arg NVIC_LP_SLEEPONEXIT 退出时自动睡眠
  *         NewState: 模式启用状态(ENABLE/DISABLE)
  * @实现   通过SCB->SCR寄存器的位操作控制低功耗特性
  */
void NVIC_SystemLPConfig(uint8_t LowPowerMode, FunctionalState NewState)
{
  /* 参数有效性验证 */
  assert_param(IS_NVIC_LP(LowPowerMode));       // 检查低功耗模式参数范围
  assert_param(IS_FUNCTIONAL_STATE(NewState));   // 验证状态参数合法性

  if (NewState != DISABLE)
  {
    /* 设置SCR寄存器的对应控制位 */
    SCB->SCR |= LowPowerMode;  // 通过位或操作启用指定低功耗特性
  }
  else
  {
    /* 清除SCR寄存器的对应控制位 */
    SCB->SCR &= (uint32_t)(~(uint32_t)LowPowerMode);  // 位取反后与操作实现位清除
  }
}

/**
  * @brief  配置SysTick时钟源
  * @功能   选择SysTick定时器的时钟基准
  * @参数   SysTick_CLKSource: 时钟源类型
  *         @arg SysTick_CLKSource_HCLK_Div8 系统时钟8分频
  *         @arg SysTick_CLKSource_HCLK      系统时钟直接使用
  * @实现   通过SysTick->CTRL寄存器的CLKSOURCE位控制
  * @注意   STM32F4时钟树中HCLK频率等于系统时钟频率
  */
void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource)
{
  /* 参数有效性验证 */
  assert_param(IS_SYSTICK_CLK_SOURCE(SysTick_CLKSource));  // 检查时钟源参数合法性

  if (SysTick_CLKSource == SysTick_CLKSource_HCLK)
  {
    /* 设置CLKSOURCE位为1选择HCLK */
    SysTick->CTRL |= SysTick_CLKSource_HCLK;  // 位或操作：CTRL[2]=1
  }
  else
  {
    /* 清除CLKSOURCE位选择HCLK/8 */
    SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8;  // 位与操作：CTRL[2]=0
  }
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

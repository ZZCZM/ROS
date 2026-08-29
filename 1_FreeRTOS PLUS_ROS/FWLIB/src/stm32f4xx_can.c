/**
  ******************************************************************************
  * @file    stm32f4xx_can.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Controller area network (CAN) peripheral:
  *           + Initialization and Configuration 
  *           + CAN Frames Transmission
  *           + CAN Frames Reception
  *           + Operation modes switch
  *           + Error management
  *           + Interrupts and flags
  *
@verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
    [..]
      (#) Enable the CAN controller interface clock using 
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); for CAN1 
          and RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE); for CAN2
      -@- In case you are using CAN2 only, you have to enable the CAN1 clock.
       
      (#) CAN pins configuration
        (++) Enable the clock for the CAN GPIOs using the following function:
             RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);   
        (++) Connect the involved CAN pins to AF9 using the following function 
             GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_CANx); 
        (++) Configure these CAN pins in alternate function mode by calling
             the function  GPIO_Init();
      
      (#) Initialise and configure the CAN using CAN_Init() and 
          CAN_FilterInit() functions.   
                 
      (#) Transmit the desired CAN frame using CAN_Transmit() function.
           
      (#) Check the transmission of a CAN frame using CAN_TransmitStatus()
          function.
                 
      (#) Cancel the transmission of a CAN frame using CAN_CancelTransmit()
          function.  
              
      (#) Receive a CAN frame using CAN_Recieve() function.
           
      (#) Release the receive FIFOs using CAN_FIFORelease() function.
                 
      (#) Return the number of pending received frames using 
          CAN_MessagePending() function.            
                     
      (#) To control CAN events you can use one of the following two methods:
        (++) Check on CAN flags using the CAN_GetFlagStatus() function.  
        (++) Use CAN interrupts through the function CAN_ITConfig() at 
             initialization phase and CAN_GetITStatus() function into 
             interrupt routines to check if the event has occurred or not.
             After checking on a flag you should clear it using CAN_ClearFlag()
             function. And after checking on an interrupt event you should 
             clear it using CAN_ClearITPendingBit() function.            

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
#include "stm32f4xx_can.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup CAN 
  * @brief CAN driver modules
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* CAN 主控制寄存器位定义 */
#define MCR_DBF           ((uint32_t)0x00010000) /* 
* [Debug Freeze] 调试冻结控制位（第16位）:
* - 0: CAN在调试期间继续工作
* - 1: CAN在调试时冻结（保持最后状态）
* 应用场景：仿真调试时查看寄存器状态
*/

/* 邮箱传输请求控制位 */
#define TMIDxR_TXRQ       ((uint32_t)0x00000001) /*
* [Transmit Request] 传输请求标志（第0位）:
* - 置1启动对应邮箱的报文发送
* - 发送完成后由硬件自动清除
* 注意：同时对多个邮箱操作需使用轮询检测
*/

/* 滤波器主寄存器配置位 */
#define FMR_FINIT         ((uint32_t)0x00000001) /*
* [Filter Init Mode] 滤波器初始化模式（第0位）:
* - 1: 进入滤波器配置模式（此时可修改滤波器参数）
* - 0: 退出配置模式，启用生效的滤波器
* 操作流程：设置->配置滤波器->清除
*/

/* 初始化确认超时阈值 */
#define INAK_TIMEOUT      ((uint32_t)0x0000FFFF) /* 
* [Initialization Acknowledge Timeout] 
* 初始化模式切换最大等待周期（65535个时钟周期）
* 应用场景：等待CAN_MCR寄存器INAK位置位
*/

/* 休眠确认超时阈值 */
#define SLAK_TIMEOUT      ((uint32_t)0x0000FFFF) /*
* [Sleep Acknowledge Timeout]
* 休眠模式切换最大等待周期（65535个时钟周期）
* 应用场景：等待CAN_MSR寄存器SLAK位置位
*/

/* 传输状态寄存器标志位掩码 */
#define CAN_FLAGS_TSR     ((uint32_t)0x08000000) /* 
* [Transmit Status Register] 发送状态组合标志：
* 包含RQCPx/TERRx/TME位组合（x=0-2）
* 位分布：RQCP2 | TXOK2 | ALST2 | TERR2 | ...（具体位查看参考手册）
*/

/* 接收FIFO1寄存器标志掩码 */
#define CAN_FLAGS_RF1R    ((uint32_t)0x04000000) /*
* [Receive FIFO1 Register] 接收FIFO1状态：
* 包含FOVR1/FULL1/FMP1位组合
* 典型应用：检测FMP1[1:0]获取接收报文数量
*/

/* 接收FIFO0寄存器标志掩码 */
#define CAN_FLAGS_RF0R    ((uint32_t)0x02000000) /*
* [Receive FIFO0 Register] 接收FIFO0状态：
* 包含FOVR0/FULL0/FMP0位组合
* 注意：两个FIFO的中断需分别处理
*/

/* 主状态寄存器标志掩码 */
#define CAN_FLAGS_MSR     ((uint32_t)0x01000000) /*
* [Master Status Register] 核心状态组合：
* 包含SLAKI/SAMP/SLAK/INAK位组合
* 主要用途：检测控制器工作模式状态
*/

/* 错误状态寄存器标志掩码 */
#define CAN_FLAGS_ESR     ((uint32_t)0x00F00000) /*
* [Error Status Register] 错误状态组合：
* 包含LEC[2:0]/BOFF/EPVF/EWGF位组合
* 位分解：
* - [24]  BOFF: 总线关闭状态
* - [25]  EPVF: 错误被动状态
* - [26]  EWGF: 错误警告状态
* - [28:26] LEC: 最后错误码
*/

/* 发送邮箱编号定义 */
#define CAN_TXMAILBOX_0   ((uint8_t)0x00) /* 邮箱1（优先级最高） */
#define CAN_TXMAILBOX_1   ((uint8_t)0x01) /* 邮箱2 */
#define CAN_TXMAILBOX_2   ((uint8_t)0x02) /* 邮箱3（优先级最低） */

/* 工作模式位掩码 */
#define CAN_MODE_MASK     ((uint32_t)0x00000003) /*
* [Operation Mode Mask] 模式选择掩码：
* 用于提取CAN_MCR寄存器的INRQ/SLEEP位组合
* 可选模式：
* - 0x00: 正常工作模式
* - 0x01: 初始化模式
* - 0x02: 休眠模式 
*/

/* 辅助函数声明（状态检测）*/
static ITStatus CheckITStatus(uint32_t CAN_Reg, uint32_t It_Bit); /*
* 功能：组合检测CAN中断状态
* 参数：CAN_Reg=寄存器地址，It_Bit=位掩码
* 返回值：中断有效状态（SET/RESET）
* 实现逻辑：同时检查标志位和中断使能位
*/

/** @defgroup CAN_Private_Functions
  * @{
  */

/** @defgroup CAN_Group1 Initialization and Configuration functions
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and Configuration functions #####
 ===============================================================================  
    [..] This section provides functions allowing to 
      (+) Initialize the CAN peripherals : Prescaler, operating mode, the maximum 
          number of time quanta to perform resynchronization, the number of time 
          quanta in Bit Segment 1 and 2 and many other modes. 
          Refer to  @ref CAN_InitTypeDef  for more details.
      (+) Configures the CAN reception filter.                                      
      (+) Select the start bank filter for slave CAN.
      (+) Enables or disables the Debug Freeze mode for CAN
      (+)Enables or disables the CAN Time Trigger Operation communication mode
   
@endverbatim
  * @{
  */
  /**
  * @brief  CAN外设反初始化函数（复位寄存器到默认状态）
  * @param  CANx: 选择CAN外设，可以是CAN1或CAN2
  * @retval 无
  * @detail 
  * - 通过APB1总线复位寄存器实现软复位
  * - 执行顺序：使能复位→延时→解除复位
  * - 适用于需要重新配置CAN外设前的复位操作
  */
void CAN_DeInit(CAN_TypeDef* CANx)
{
  /* 参数合法性检查：验证是否有效CAN外设 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
 
  if (CANx == CAN1)
  {
    /* 通过RCC模块复位CAN1 */
    /* 使能CAN1复位（将外设置于复位状态） */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
    /* 禁用CAN1复位（使外设退出复位状态） */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);
  }
  else
  {  
    /* 通过RCC模块复位CAN2 */
    /* 注意：CAN2与CAN1共享部分资源，必须先初始化CAN1才能使用CAN2 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, DISABLE);
  }
}

/**
  * @brief  CAN外设初始化函数（根据结构体参数配置）
  * @param  CANx: 选择CAN外设，可以是CAN1或CAN2
  * @param  CAN_InitStruct: 指向包含配置参数的结构体指针
  * @retval 初始化状态：
  *         - CAN_InitStatus_Failed 初始化失败
  *         - CAN_InitStatus_Success 初始化成功
  * @detail
  * - 配置流程：退出睡眠模式→请求初始化→配置工作模式→设置位时序→退出初始化
  * - 包含两次状态检查（进入和退出初始化模式）
  * - 使用INAK_TIMEOUT机制防止硬件死锁
  */
uint8_t CAN_Init(CAN_TypeDef* CANx, CAN_InitTypeDef* CAN_InitStruct)
{
  uint8_t InitStatus = CAN_InitStatus_Failed;
  uint32_t wait_ack = 0x00000000;  // 超时计数器
  
  /* 参数合法性检查：包含所有配置项的验证 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_TTCM));  // 时间触发模式
  assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_ABOM));   // 自动离线管理
  assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_AWUM));   // 自动唤醒模式
  assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_NART));    // 无自动重传
  assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_RFLM));    // FIFO锁定模式
  assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_TXFP));    // 发送优先级
  assert_param(IS_CAN_MODE(CAN_InitStruct->CAN_Mode));            // 工作模式
  assert_param(IS_CAN_SJW(CAN_InitStruct->CAN_SJW));              // 同步跳转宽度
  assert_param(IS_CAN_BS1(CAN_InitStruct->CAN_BS1));             // 时间段1
  assert_param(IS_CAN_BS2(CAN_InitStruct->CAN_BS2));             // 时间段2
  assert_param(IS_CAN_PRESCALER(CAN_InitStruct->CAN_Prescaler)); // 预分频系数

  /* 退出睡眠模式（清除MCR寄存器的SLEEP位） */
  CANx->MCR &= (~(uint32_t)CAN_MCR_SLEEP);

  /* 请求进入初始化模式（设置INRQ位） */
  CANx->MCR |= CAN_MCR_INRQ ;

  /* 等待初始化模式确认（检查MSR寄存器的INAK位） */
  /* 超时机制防止硬件无响应造成死循环 */
  while (((CANx->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) && (wait_ack != INAK_TIMEOUT))
  {
    wait_ack++;
  }

  /* 检查是否成功进入初始化模式 */
  if ((CANx->MSR & CAN_MSR_INAK) != CAN_MSR_INAK)
  {
    InitStatus = CAN_InitStatus_Failed;
  }
  else 
  {
    /*---------- 配置工作模式参数 ----------*/
    /* 时间触发通信模式（TTCM） */
    CANx->MCR = (CAN_InitStruct->CAN_TTCM == ENABLE) ? 
               (CANx->MCR | CAN_MCR_TTCM) : 
               (CANx->MCR & ~CAN_MCR_TTCM);

    /* 自动离线管理（ABOM） */
    CANx->MCR = (CAN_InitStruct->CAN_ABOM == ENABLE) ? 
               (CANx->MCR | CAN_MCR_ABOM) : 
               (CANx->MCR & ~CAN_MCR_ABOM);

    /* 自动唤醒模式（AWUM） */
    CANx->MCR = (CAN_InitStruct->CAN_AWUM == ENABLE) ? 
               (CANx->MCR | CAN_MCR_AWUM) : 
               (CANx->MCR & ~CAN_MCR_AWUM);

    /* 禁用自动重传（NART） */
    CANx->MCR = (CAN_InitStruct->CAN_NART == ENABLE) ? 
               (CANx->MCR | CAN_MCR_NART) : 
               (CANx->MCR & ~CAN_MCR_NART);

    /* 接收FIFO锁定模式（RFLM） */
    CANx->MCR = (CAN_InitStruct->CAN_RFLM == ENABLE) ? 
               (CANx->MCR | CAN_MCR_RFLM) : 
               (CANx->MCR & ~CAN_MCR_RFLM);

    /* 发送FIFO优先级（TXFP） */
    CANx->MCR = (CAN_InitStruct->CAN_TXFP == ENABLE) ? 
               (CANx->MCR | CAN_MCR_TXFP) : 
               (CANx->MCR & ~CAN_MCR_TXFP);

    /*---------- 配置位时序寄存器 ----------*/
    /* BTR寄存器结构：
       [31:30] 工作模式（静默/环回等）
       [24:25] 同步跳转宽度SJW（1-4Tq）
       [20:22] 时间段2 BS2（1-8Tq）
       [16:19] 时间段1 BS1（1-16Tq）
       [0:9]   预分频系数（1-1024） */
    CANx->BTR = (uint32_t)((uint32_t)CAN_InitStruct->CAN_Mode << 30) | \
                ((uint32_t)CAN_InitStruct->CAN_SJW << 24) | \
                ((uint32_t)CAN_InitStruct->CAN_BS1 << 16) | \
                ((uint32_t)CAN_InitStruct->CAN_BS2 << 20) | \
               ((uint32_t)CAN_InitStruct->CAN_Prescaler - 1);  // 预分频值需要-1写入寄存器

    /* 请求退出初始化模式（清除INRQ位） */
    CANx->MCR &= ~(uint32_t)CAN_MCR_INRQ;

    /* 等待退出初始化模式确认 */
    wait_ack = 0;
    while (((CANx->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) && (wait_ack != INAK_TIMEOUT))
    {
      wait_ack++;
    }

    /* 最终状态检查 */
    InitStatus = ((CANx->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) ? 
                CAN_InitStatus_Failed : 
                CAN_InitStatus_Success;
  }

  return InitStatus;  // 返回初始化最终状态
}
/**
  * @brief  CAN接收过滤器初始化函数
  * @param  CAN_FilterInitStruct: 过滤器配置结构体指针
  * @retval 无
  * @detail
  * - 配置流程：进入初始化模式→禁用过滤器→配置参数→激活过滤器→退出初始化
  * - 支持两种工作模式：标识符屏蔽模式/标识符列表模式
  * - 支持两种尺度：16位（2个ID+掩码）或32位（1个扩展ID+掩码）
  * - 配置步骤：
  *   1. 参数有效性验证
  *   2. 生成过滤器位掩码
  *   3. 进入过滤器初始化模式
  *   4. 配置过滤器参数
  *   5. 激活过滤器
  */
void CAN_FilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct)
{
  uint32_t filter_number_bit_pos = 0;
  
  /* 参数合法性检查 */
  assert_param(IS_CAN_FILTER_NUMBER(CAN_FilterInitStruct->CAN_FilterNumber));      // 过滤器编号0-27
  assert_param(IS_CAN_FILTER_MODE(CAN_FilterInitStruct->CAN_FilterMode));         // 模式：IdMask/IdList
  assert_param(IS_CAN_FILTER_SCALE(CAN_FilterInitStruct->CAN_FilterScale));      // 尺度：16bit/32bit
  assert_param(IS_CAN_FILTER_FIFO(CAN_FilterInitStruct->CAN_FilterFIFOAssignment)); // 分配FIFO0/FIFO1
  assert_param(IS_FUNCTIONAL_STATE(CAN_FilterInitStruct->CAN_FilterActivation));  // 激活状态

  /* 生成过滤器位掩码（例如FilterNumber=5 → 0x00000020）*/
  filter_number_bit_pos = ((uint32_t)1) << CAN_FilterInitStruct->CAN_FilterNumber;

  /* 进入过滤器初始化模式（设置FMR寄存器的FINIT位） */
  CAN_ID->FMR |= FMR_FINIT;

  /* 禁用目标过滤器（清除FA1R对应位）*/
  CAN_ID->FA1R &= ~(uint32_t)filter_number_bit_pos;

  /*---- 16位过滤器配置 ----*/
  if (CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_16bit)
  {
    /* 设置过滤器为16位尺度（清除FS1R对应位） */
    CAN_ID->FS1R &= ~(uint32_t)filter_number_bit_pos;

    /* 配置过滤器寄存器1：
       - 高16位：掩码低16位（CAN_FilterMaskIdLow）
       - 低16位：ID低16位（CAN_FilterIdLow） */
    CAN_ID->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR1 = 
       ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdLow) << 16) |
        (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdLow);

    /* 配置过滤器寄存器2：
       - 高16位：掩码高16位（CAN_FilterMaskIdHigh）
       - 低16位：ID高16位（CAN_FilterIdHigh） */
    CAN_ID->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR2 = 
       ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
        (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdHigh);
  }

  /*---- 32位过滤器配置 ----*/
  if (CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_32bit)
  {
    /* 设置过滤器为32位尺度（置位FS1R对应位） */
    CAN_ID->FS1R |= filter_number_bit_pos;

    /* 配置过滤器寄存器1：
       - 高16位：ID高16位（CAN_FilterIdHigh）
       - 低16位：ID低16位（CAN_FilterIdLow） */
    CAN_ID->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR1 = 
       ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdHigh) << 16) |
        (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdLow);

    /* 配置过滤器寄存器2：
       - 高16位：掩码高16位（CAN_FilterMaskIdHigh）
       - 低16位：掩码低16位（CAN_FilterMaskIdLow） */
    CAN_ID->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR2 = 
       ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
        (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdLow);
  }

  /*---- 工作模式配置 ----*/
  /* 标识符屏蔽模式（IdMask）*/
  if (CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdMask)
  {
    /* 清除FM1R对应位，每个ID位需与掩码位比较 */
    CAN_ID->FM1R &= ~(uint32_t)filter_number_bit_pos;
  }
  /* 标识符列表模式（IdList）*/
  else 
  {
    /* 置位FM1R对应位，直接匹配ID列表 */
    CAN_ID->FM1R |= (uint32_t)filter_number_bit_pos;
  }

  /*---- FIFO分配配置 ----*/
  /* 分配到FIFO0（默认路径）*/
  if (CAN_FilterInitStruct->CAN_FilterFIFOAssignment == CAN_Filter_FIFO0)
  {
    /* 清除FFA1R对应位 */
    CAN_ID->FFA1R &= ~(uint32_t)filter_number_bit_pos;
  }
  /* 分配到FIFO1 */
  else if (CAN_FilterInitStruct->CAN_FilterFIFOAssignment == CAN_Filter_FIFO1)
  {
    /* 置位FFA1R对应位 */
    CAN_ID->FFA1R |= (uint32_t)filter_number_bit_pos;
  }

  /* 激活过滤器（设置FA1R对应位）*/
  if (CAN_FilterInitStruct->CAN_FilterActivation == ENABLE)
  {
    CAN_ID->FA1R |= filter_number_bit_pos;
  }

  /* 退出过滤器初始化模式（清除FMR的FINIT位）*/
  CAN_ID->FMR &= ~FMR_FINIT;
}

/**
  * @brief  CAN初始化结构体默认值初始化函数
  * @param  CAN_InitStruct: 要初始化的CAN_InitTypeDef结构体指针
  * @retval 无
  * @detail
  * - 初始化所有配置参数为安全默认值
  * - 典型应用场景：在用户配置前初始化结构体
  * - 默认配置：
  *   - 普通模式、1tq同步跳转宽度
  *   - 时间段BS1=4tq、BS2=3tq
  *   - 关闭所有高级功能模式
  */
void CAN_StructInit(CAN_InitTypeDef* CAN_InitStruct)
{
  /* 重置所有参数为默认安全值 */
  
  /* 时间触发通信模式：禁用（标准CAN操作） */
  CAN_InitStruct->CAN_TTCM = DISABLE;
  
  /* 自动离线管理：禁用（需要软件处理bus-off状态） */
  CAN_InitStruct->CAN_ABOM = DISABLE;
  
  /* 自动唤醒模式：禁用（需要软件唤醒） */
  CAN_InitStruct->CAN_AWUM = DISABLE;
  
  /* 非自动重传：禁用（自动重传失效报文） */
  CAN_InitStruct->CAN_NART = DISABLE;
  
  /* 接收FIFO锁定模式：禁用（溢出时新报文覆盖旧报文） */
  CAN_InitStruct->CAN_RFLM = DISABLE;
  
  /* 发送FIFO优先级：禁用（按请求顺序发送） */
  CAN_InitStruct->CAN_TXFP = DISABLE;
  
  /* 工作模式：普通模式（正常收发） */
  CAN_InitStruct->CAN_Mode = CAN_Mode_Normal;
  
  /* 同步跳转宽度：1个时间单位（保证最小相位缓冲） */
  CAN_InitStruct->CAN_SJW = CAN_SJW_1tq;
  
  /* 时间段1：4tq（采样点位于时间段1结束处） */
  CAN_InitStruct->CAN_BS1 = CAN_BS1_4tq;
  
  /* 时间段2：3tq（用于位定时调整） */
  CAN_InitStruct->CAN_BS2 = CAN_BS2_3tq;
  
  /* 预分频器：1分频（APB1时钟直接作为CAN时钟源） */
  CAN_InitStruct->CAN_Prescaler = 1;
}

/**
  * @brief  设置从CAN的起始过滤器库编号
  * @param  CAN_BankNumber: 起始库编号（范围1-27）
  * @retval 无
  * @detail
  * - 用于双CAN系统中分配过滤器资源
  * - 主CAN使用0~(n-1)号过滤器库
  * - 从CAN使用n~27号过滤器库
  * - 必须在过滤器初始化前调用
  */
void CAN_SlaveStartBank(uint8_t CAN_BankNumber) 
{
  /* 验证库编号合法性 */
  assert_param(IS_CAN_BANKNUMBER(CAN_BankNumber));
  
  /* 进入过滤器初始化模式（设置FMR的FINIT位） */
  CAN_ID->FMR |= FMR_FINIT;
  
  /* 清除原有库配置（保留[7:4]和[3:0]位） */
  CAN_ID->FMR &= (uint32_t)0xFFFFC0F1 ;
  
  /* 设置新的起始库编号（写入FMR的[15:8]位） */
  CAN_ID->FMR |= (uint32_t)(CAN_BankNumber)<<8;
  
  /* 退出初始化模式（启动新配置） */
  CAN_ID->FMR &= ~FMR_FINIT;
}

/**
  * @brief  CAN调试冻结控制函数
  * @param  CANx: CAN外设选择（CAN1/CAN2）
  * @param  NewState: 新状态（ENABLE-冻结，DISABLE-正常运行）
  * @retval 无
  * @detail
  * - 调试模式下冻结CAN核心时钟
  * - 冻结时：禁止收发，但可访问接收FIFO
  * - 应用场景：调试时观察总线状态而不影响通信
  */
void CAN_DBGFreeze(CAN_TypeDef* CANx, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* 设置调试冻结模式（MCR寄存器的DBF位置1） */
    CANx->MCR |= MCR_DBF;  // 冻结期间：TEC/REC停止计数，禁止收发
  }
  else
  {
    /* 解除调试冻结（恢复正常工作） */
    CANx->MCR &= ~MCR_DBF; // 退出时需要重新同步总线
  }
}

/**
  * @brief  CAN时间触发通信模式配置函数
  * @param  CANx: 选择CAN外设（CAN1/CAN2）
  * @param  NewState: 新状态（ENABLE/DISABLE）
  * @retval 无
  * @detail
  * - 时间触发模式特点：
  *   1. 必须在数据帧中发送时间戳（DLC必须设为8）
  *   2. 时间戳占用最后2字节（数据字节6和7）
  *   3. 需要配合定时器实现时间同步
  * - 寄存器联动操作：需同时配置MCR和所有发送邮箱
  * - 应用场景：TTCAN协议、分布式系统时间同步
  */
void CAN_TTComModeCmd(CAN_TypeDef* CANx, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /*-- 启用TTCM模式 --*/
    /* 设置MCR寄存器的TTCM位（时间触发通信模式使能） */
    CANx->MCR |= CAN_MCR_TTCM;

    /* 配置所有发送邮箱的TGT位（时间触发模式报文标志） */
    /* 发送邮箱0：设置TDTR寄存器的TGT位 */
    CANx->sTxMailBox[0].TDTR |= ((uint32_t)CAN_TDT0R_TGT);
    /* 发送邮箱1：设置TDTR寄存器的TGT位 */    
    CANx->sTxMailBox[1].TDTR |= ((uint32_t)CAN_TDT1R_TGT);
    /* 发送邮箱2：设置TDTR寄存器的TGT位 */
    CANx->sTxMailBox[2].TDTR |= ((uint32_t)CAN_TDT2R_TGT);
  }
  else
  {
    /*-- 禁用TTCM模式 --*/
    /* 清除MCR寄存器的TTCM位 */
    CANx->MCR &= (uint32_t)(~(uint32_t)CAN_MCR_TTCM);

    /* 清除所有发送邮箱的TGT位 */
    CANx->sTxMailBox[0].TDTR &= ((uint32_t)~CAN_TDT0R_TGT);
    CANx->sTxMailBox[1].TDTR &= ((uint32_t)~CAN_TDT1R_TGT);
    CANx->sTxMailBox[2].TDTR &= ((uint32_t)~CAN_TDT2R_TGT);
  }
}

/**
  * @}
  */


/** @defgroup CAN_Group2 CAN Frames Transmission functions
 *  @brief    CAN Frames Transmission functions 
 *
@verbatim    
 ===============================================================================
                ##### CAN Frames Transmission functions #####
 ===============================================================================  
    [..] This section provides functions allowing to 
      (+) Initiate and transmit a CAN frame message (if there is an empty mailbox).
      (+) Check the transmission status of a CAN Frame
      (+) Cancel a transmit request
   
@endverbatim
  * @{
  */

/**
  * @brief  Initiates and transmits a CAN frame message.
  * @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
  * @param  TxMessage: pointer to a structure which contains CAN Id, CAN DLC and CAN data.
  * @retval The number of the mailbox that is used for transmission or
  *         CAN_TxStatus_NoMailBox if there is no empty mailbox.
  */
/**
  * @brief  CAN报文发送函数
  * @param  CANx: 选择CAN外设（CAN1/CAN2）
  * @param  TxMessage: 发送报文结构体指针
  * @retval 
  *   - 0-2: 使用的邮箱编号
  *   - CAN_TxStatus_NoMailBox (0xFF): 所有邮箱忙
  * @detail
  * - 发送流程：选择空邮箱→配置ID→设置DLC→填充数据→触发发送
  * - 支持标准帧（11位ID）和扩展帧（29位ID）
  * - 数据按小端模式打包（Data[0]存放在最低地址）
  */
uint8_t CAN_Transmit(CAN_TypeDef* CANx, CanTxMsg* TxMessage)
{
  uint8_t transmit_mailbox = 0;
  
  /* 参数合法性检查 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_IDTYPE(TxMessage->IDE));  // 标识符类型：标准/扩展
  assert_param(IS_CAN_RTR(TxMessage->RTR));     // 远程帧标志
  assert_param(IS_CAN_DLC(TxMessage->DLC));     // 数据长度0-8

  /* 选择空闲发送邮箱（检查TSR寄存器的TME位） */
  if ((CANx->TSR & CAN_TSR_TME0) == CAN_TSR_TME0) {
    transmit_mailbox = 0;  // 邮箱0空闲
  } else if ((CANx->TSR & CAN_TSR_TME1) == CAN_TSR_TME1) {
    transmit_mailbox = 1;  // 邮箱1空闲
  } else if ((CANx->TSR & CAN_TSR_TME2) == CAN_TSR_TME2) {
    transmit_mailbox = 2;  // 邮箱2空闲
  } else {
    transmit_mailbox = CAN_TxStatus_NoMailBox; // 所有邮箱忙（0xFF）
  }

  if (transmit_mailbox != CAN_TxStatus_NoMailBox) 
  {
    /*---- 配置标识符 ----*/
    /* 清除TIR寄存器（保留TXRQ位） */
    CANx->sTxMailBox[transmit_mailbox].TIR &= TMIDxR_TXRQ;
    
    /* 标准帧配置（11位ID） */
    if (TxMessage->IDE == CAN_Id_Standard) {
      assert_param(IS_CAN_STDID(TxMessage->StdId));  // 验证标准ID范围
      /* 标准ID左移21位 + RTR配置 */
      CANx->sTxMailBox[transmit_mailbox].TIR |= 
        ((TxMessage->StdId << 21) | TxMessage->RTR);
    }
    /* 扩展帧配置（29位ID） */ 
    else {
      assert_param(IS_CAN_EXTID(TxMessage->ExtId)); // 验证扩展ID范围
      /* 扩展ID左移3位 + IDE标志 + RTR配置 */
      CANx->sTxMailBox[transmit_mailbox].TIR |= 
        ((TxMessage->ExtId << 3) | TxMessage->IDE | TxMessage->RTR);
    }

    /*---- 配置数据长度码DLC ----*/
    /* 限制DLC范围为0-8 */
    TxMessage->DLC &= (uint8_t)0x0000000F;
    /* 清除原有DLC值（保留TDTR其他位） */
    CANx->sTxMailBox[transmit_mailbox].TDTR &= 0xFFFFFFF0;
    /* 写入新的DLC值（位于[3:0]位） */
    CANx->sTxMailBox[transmit_mailbox].TDTR |= TxMessage->DLC;

    /*---- 填充数据字段 ----*/
    /* 低4字节数据（数据字节0-3）*/
    CANx->sTxMailBox[transmit_mailbox].TDLR =
      (((uint32_t)TxMessage->Data[3] << 24) |  // 数据字节3 → 最高字节
       ((uint32_t)TxMessage->Data[2] << 16) |   // 数据字节2
       ((uint32_t)TxMessage->Data[1] << 8)  |   // 数据字节1
       ((uint32_t)TxMessage->Data[0]));         // 数据字节0 → 最低字节

    /* 高4字节数据（数据字节4-7）*/    
    CANx->sTxMailBox[transmit_mailbox].TDHR =
      (((uint32_t)TxMessage->Data[7] << 24) |  // 数据字节7 → 最高字节
       ((uint32_t)TxMessage->Data[6] << 16) |   // 数据字节6
       ((uint32_t)TxMessage->Data[5] << 8)  |   // 数据字节5
       ((uint32_t)TxMessage->Data[4]));         // 数据字节4 → 最低字节

    /* 触发发送请求（设置TXRQ位） */
    CANx->sTxMailBox[transmit_mailbox].TIR |= TMIDxR_TXRQ;
  }
  
  return transmit_mailbox;  // 返回使用的邮箱编号或错误状态
}

/**
  * @brief  检查CAN报文发送状态
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @param  TransmitMailbox: 发送邮箱编号(0-2)
  * @retval 发送状态:
  *         - CAN_TxStatus_Pending: 发送挂起中
  *         - CAN_TxStatus_Ok      : 发送成功
  *         - CAN_TxStatus_Failed  : 发送失败
  */
uint8_t CAN_TransmitStatus(CAN_TypeDef* CANx, uint8_t TransmitMailbox)
{
  uint32_t state = 0;

  /* 参数有效性验证 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_TRANSMITMAILBOX(TransmitMailbox));

  /* 通过TSR寄存器获取指定邮箱状态 */
  switch (TransmitMailbox)
  {
    case CAN_TXMAILBOX_0: 
      // 获取邮箱0状态: 请求完成标志 | 发送成功标志 | 邮箱空标志
      state = CANx->TSR & (CAN_TSR_RQCP0 | CAN_TSR_TXOK0 | CAN_TSR_TME0);
      break;
    case CAN_TXMAILBOX_1:
      state = CANx->TSR & (CAN_TSR_RQCP1 | CAN_TSR_TXOK1 | CAN_TSR_TME1);
      break;
    case CAN_TXMAILBOX_2:
      state = CANx->TSR & (CAN_TSR_RQCP2 | CAN_TSR_TXOK2 | CAN_TSR_TME2);
      break;
    default:
      state = CAN_TxStatus_Failed;
      break;
  }

  /* 解析硬件状态标志 */
  switch (state)
  {
    /* 状态0: 所有标志位未置位，发送仍在进行 */
    case 0x0: 
      state = CAN_TxStatus_Pending;
      break;

    /* 失败状态组合: 请求完成+邮箱空，但无成功标志 */
    case (CAN_TSR_RQCP0 | CAN_TSR_TME0):  // 邮箱0异常结束
    case (CAN_TSR_RQCP1 | CAN_TSR_TME1):  // 邮箱1异常结束
    case (CAN_TSR_RQCP2 | CAN_TSR_TME2):  // 邮箱2异常结束
      state = CAN_TxStatus_Failed;
      break;

    /* 成功状态组合: 请求完成+发送成功+邮箱空 */
    case (CAN_TSR_RQCP0 | CAN_TSR_TXOK0 | CAN_TSR_TME0): // 邮箱0成功
    case (CAN_TSR_RQCP1 | CAN_TSR_TXOK1 | CAN_TSR_TME1): // 邮箱1成功
    case (CAN_TSR_RQCP2 | CAN_TSR_TXOK2 | CAN_TSR_TME2): // 邮箱2成功
      state = CAN_TxStatus_Ok;
      break;

    default:
      state = CAN_TxStatus_Failed;
      break;
  }

  return (uint8_t)state;
}

/**
  * @brief  取消指定邮箱的发送请求
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @param  Mailbox: 发送邮箱编号(0-2)
  * @retval 无
  * @detail 注意:
  * - 取消操作不可逆，报文将被丢弃
  * - 邮箱变为空状态后可重新使用
  */
void CAN_CancelTransmit(CAN_TypeDef* CANx, uint8_t Mailbox)
{
  /* 参数合法性检查 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_TRANSMITMAILBOX(Mailbox));

  /* 设置对应邮箱的终止请求位 */
  switch (Mailbox)
  {
    case CAN_TXMAILBOX_0: 
      CANx->TSR |= CAN_TSR_ABRQ0;  // 设置终止请求位0
      break;
    case CAN_TXMAILBOX_1:
      CANx->TSR |= CAN_TSR_ABRQ1;  // 设置终止请求位1
      break;
    case CAN_TXMAILBOX_2:
      CANx->TSR |= CAN_TSR_ABRQ2;  // 设置终止请求位2
      break;
    default:
      break;  // 无效参数不做处理
  }
}

/**
  * @}
  */


/** @defgroup CAN_Group3 CAN Frames Reception functions
 *  @brief    CAN Frames Reception functions 
 *
@verbatim    
 ===============================================================================
                ##### CAN Frames Reception functions #####
 ===============================================================================  
    [..] This section provides functions allowing to 
      (+) Receive a correct CAN frame
      (+) Release a specified receive FIFO (2 FIFOs are available)
      (+) Return the number of the pending received CAN frames
   
@endverbatim
  * @{
  */

/**
  * @brief  Receives a correct CAN frame.
  * @param  CANx: where x can be 1 or 2 to select the CAN peripheral.
  * @param  FIFONumber: Receive FIFO number, CAN_FIFO0 or CAN_FIFO1.
  * @param  RxMessage: pointer to a structure receive frame which contains CAN Id,
  *         CAN DLC, CAN data and FMI number.
  * @retval None
  */
/**
  * @brief  从指定接收FIFO读取CAN报文
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @param  FIFONumber: FIFO编号 (CAN_FIFO0/CAN_FIFO1)
  * @param  RxMessage: 接收报文结构体指针
  * @detail
  * - 报文读取流程：解析标识符→数据长度→数据内容→释放FIFO
  * - 自动处理标准帧和扩展帧
  * - 必须调用本函数后FIFO才能接收新报文
  */
void CAN_Receive(CAN_TypeDef* CANx, uint8_t FIFONumber, CanRxMsg* RxMessage)
{
  /* 参数合法性检查 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_FIFO(FIFONumber));

  /*---- 解析接收标识符寄存器RIR ----*/
  /* 获取帧格式：bit2为IDE标志 (0-标准帧，1-扩展帧) */
  RxMessage->IDE = (uint8_t)(CANx->sFIFOMailBox[FIFONumber].RIR & 0x04);
  
  if (RxMessage->IDE == CAN_Id_Standard) 
  {
    /* 标准帧：11位ID (存储在RIR[31:21]) */
    RxMessage->StdId = (uint32_t)((CANx->sFIFOMailBox[FIFONumber].RIR >> 21) & 0x7FF);
  }
  else 
  {
    /* 扩展帧：29位ID (存储在RIR[31:3]) */
    RxMessage->ExtId = (uint32_t)((CANx->sFIFOMailBox[FIFONumber].RIR >> 3) & 0x1FFFFFFF);
  }
  
  /* 获取远程帧标志：bit1为RTR标志 */
  RxMessage->RTR = (uint8_t)(CANx->sFIFOMailBox[FIFONumber].RIR & 0x02);

  /*---- 解析接收数据长度寄存器RDTR ----*/
  /* 数据长度码DLC (bit3~bit0) */
  RxMessage->DLC = (uint8_t)(CANx->sFIFOMailBox[FIFONumber].RDTR & 0x0F);
  /* 过滤器匹配索引FMI (bit15~bit8) */
  RxMessage->FMI = (uint8_t)((CANx->sFIFOMailBox[FIFONumber].RDTR >> 8) & 0xFF);

  /*---- 解析接收数据寄存器 ----*/
  /* 低4字节数据 (RDLR寄存器) */
  RxMessage->Data[0] = (uint8_t)(CANx->sFIFOMailBox[FIFONumber].RDLR & 0xFF);        // 字节0
  RxMessage->Data[1] = (uint8_t)((CANx->sFIFOMailBox[FIFONumber].RDLR >> 8) & 0xFF); // 字节1
  RxMessage->Data[2] = (uint8_t)((CANx->sFIFOMailBox[FIFONumber].RDLR >> 16) & 0xFF); // 字节2
  RxMessage->Data[3] = (uint8_t)((CANx->sFIFOMailBox[FIFONumber].RDLR >> 24) & 0xFF); // 字节3

  /* 高4字节数据 (RDHR寄存器) */
  RxMessage->Data[4] = (uint8_t)(CANx->sFIFOMailBox[FIFONumber].RDHR & 0xFF);         // 字节4
  RxMessage->Data[5] = (uint8_t)((CANx->sFIFOMailBox[FIFONumber].RDHR >> 8) & 0xFF); // 字节5
  RxMessage->Data[6] = (uint8_t)((CANx->sFIFOMailBox[FIFONumber].RDHR >> 16) & 0xFF); // 字节6
  RxMessage->Data[7] = (uint8_t)((CANx->sFIFOMailBox[FIFONumber].RDHR >> 24) & 0xFF); // 字节7

  /*---- 释放FIFO空间 ----*/
  if (FIFONumber == CAN_FIFO0) 
  {
    /* 设置RF0R寄存器的RFOM0位释放FIFO0 */
    CANx->RF0R |= CAN_RF0R_RFOM0;  // 释放后硬件自动递增接收指针
  } 
  else 
  {
    /* 设置RF1R寄存器的RFOM1位释放FIFO1 */
    CANx->RF1R |= CAN_RF1R_RFOM1;
  }
}

/**
  * @brief  手动释放接收FIFO
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @param  FIFONumber: FIFO编号 (CAN_FIFO0/CAN_FIFO1)
  * @detail
  * - 可用于批量清除接收队列
  * - 与CAN_Receive()中的自动释放机制二选一使用
  */
void CAN_FIFORelease(CAN_TypeDef* CANx, uint8_t FIFONumber)
{
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_FIFO(FIFONumber));

  if (FIFONumber == CAN_FIFO0) {
    CANx->RF0R |= CAN_RF0R_RFOM0;  // 立即释放FIFO0所有报文
  } else {
    CANx->RF1R |= CAN_RF1R_RFOM1;  // 立即释放FIFO1所有报文
  }
}

/**
  * @brief  获取接收FIFO中的待处理报文数量
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @param  FIFONumber: FIFO编号 (CAN_FIFO0/CAN_FIFO1)
  * @retval 报文数量 (0-3)
  * @detail
  * - 实际可用深度取决于硬件设计（F4系列通常为3级）
  * - 当返回3时表示FIFO已满，新报文会覆盖旧数据
  */
uint8_t CAN_MessagePending(CAN_TypeDef* CANx, uint8_t FIFONumber)
{
  uint8_t message_pending = 0;
  
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_FIFO(FIFONumber));

  if (FIFONumber == CAN_FIFO0) 
  {
    /* 读取RF0R[1:0]获取FIFO0报文数 */
    message_pending = (uint8_t)(CANx->RF0R & 0x03);
  } 
  else if (FIFONumber == CAN_FIFO1) 
  {
    /* 读取RF1R[1:0]获取FIFO1报文数 */
    message_pending = (uint8_t)(CANx->RF1R & 0x03);
  }
  
  return message_pending;
}

/**
  * @}
  */


/** @defgroup CAN_Group4 CAN Operation modes functions
 *  @brief    CAN Operation modes functions 
 *
@verbatim    
 ===============================================================================
                    ##### CAN Operation modes functions #####
 ===============================================================================  
    [..] This section provides functions allowing to select the CAN Operation modes
      (+) sleep mode
      (+) normal mode 
      (+) initialization mode
   
@endverbatim
  * @{
  */
  
  
/**
  * @brief  Selects the CAN Operation mode.
  * @param  CAN_OperatingMode: CAN Operating Mode.
  *         This parameter can be one of @ref CAN_OperatingMode_TypeDef enumeration.
  * @retval status of the requested mode which can be 
  *         - CAN_ModeStatus_Failed:  CAN failed entering the specific mode 
  *         - CAN_ModeStatus_Success: CAN Succeed entering the specific mode 
  */
/**
  * @brief  CAN工作模式切换请求
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @param  CAN_OperatingMode: 目标工作模式
  *   - CAN_OperatingMode_Initialization: 初始化模式（配置寄存器）
  *   - CAN_OperatingMode_Normal:        正常模式（收发报文）
  *   - CAN_OperatingMode_Sleep:         睡眠模式（低功耗）
  * @retval 模式切换状态：
  *   - CAN_ModeStatus_Success: 模式切换成功
  *   - CAN_ModeStatus_Failed:  模式切换失败（超时或错误）
  * @detail
  * - 模式切换需要满足总线空闲条件
  * - 初始化模式用于重新配置位定时等参数
  * - 睡眠模式可通过总线活动或软件唤醒
  */
uint8_t CAN_OperatingModeRequest(CAN_TypeDef* CANx, uint8_t CAN_OperatingMode)
{
  uint8_t status = CAN_ModeStatus_Failed;
  uint32_t timeout = INAK_TIMEOUT;  // 模式切换超时计数器（典型值1000-10000）

  /* 参数合法性检查 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_OPERATING_MODE(CAN_OperatingMode));

  /*---- 进入初始化模式 ----*/
  if (CAN_OperatingMode == CAN_OperatingMode_Initialization) 
  {
    /* 设置MCR寄存器：清除睡眠模式标志，请求初始化模式 */
    CANx->MCR = (CANx->MCR & ~CAN_MCR_SLEEP) | CAN_MCR_INRQ;

    /* 等待硬件响应（检查MSR的INAK位） */
    while (((CANx->MSR & CAN_MODE_MASK) != CAN_MSR_INAK) && (timeout != 0)) {
      timeout--;
    }

    /* 验证是否成功进入初始化模式 */
    status = ((CANx->MSR & CAN_MODE_MASK) == CAN_MSR_INAK) ? 
             CAN_ModeStatus_Success : CAN_ModeStatus_Failed;
  }
  /*---- 进入正常模式 ----*/
  else if (CAN_OperatingMode == CAN_OperatingMode_Normal) 
  {
    /* 清除MCR寄存器的睡眠和初始化请求位 */
    CANx->MCR &= ~(CAN_MCR_SLEEP | CAN_MCR_INRQ);

    /* 等待退出初始化/睡眠模式（MSR模式位归零） */
    while (((CANx->MSR & CAN_MODE_MASK) != 0) && (timeout != 0)) {
      timeout--;
    }

    status = ((CANx->MSR & CAN_MODE_MASK) == 0) ? 
             CAN_ModeStatus_Success : CAN_ModeStatus_Failed;
  }
  /*---- 进入睡眠模式 ----*/
  else if (CAN_OperatingMode == CAN_OperatingMode_Sleep) 
  {
    /* 设置MCR寄存器：保持当前模式，请求睡眠模式 */
    CANx->MCR = (CANx->MCR & ~CAN_MCR_INRQ) | CAN_MCR_SLEEP;

    /* 等待睡眠确认（检查MSR的SLAK位） */
    while (((CANx->MSR & CAN_MODE_MASK) != CAN_MSR_SLAK) && (timeout != 0)) {
      timeout--;
    }

    status = ((CANx->MSR & CAN_MODE_MASK) == CAN_MSR_SLAK) ? 
             CAN_ModeStatus_Success : CAN_ModeStatus_Failed;
  }
  else 
  {
    status = CAN_ModeStatus_Failed;
  }

  return (uint8_t)status;
}

/**
  * @brief  进入CAN睡眠模式（低功耗）
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @retval 睡眠状态：
  *   - CAN_Sleep_Ok:    成功进入睡眠
  *   - CAN_Sleep_Failed: 进入失败（总线活动或配置错误）
  * @detail
  * - 进入条件：总线空闲状态
  * - 唤醒方式：总线活动、软件唤醒或中断
  */
uint8_t CAN_Sleep(CAN_TypeDef* CANx)
{
  uint8_t sleepstatus = CAN_Sleep_Failed;

  assert_param(IS_CAN_ALL_PERIPH(CANx));

  /* 设置MCR寄存器：保持当前状态，请求睡眠模式 */
  CANx->MCR = (CANx->MCR & ~CAN_MCR_INRQ) | CAN_MCR_SLEEP;

  /* 检查是否同时处于睡眠和初始化模式（异常情况） */
  if ((CANx->MSR & (CAN_MSR_SLAK | CAN_MSR_INAK)) == CAN_MSR_SLAK) 
  {
    sleepstatus = CAN_Sleep_Ok;  // 成功进入睡眠模式
  }

  return (uint8_t)sleepstatus;
}
/**
  * @brief  从睡眠模式唤醒CAN外设
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @retval 唤醒状态：
  *   - CAN_WakeUp_Ok:    成功退出睡眠模式
  *   - CAN_WakeUp_Failed: 唤醒失败（超时或硬件错误）
  * @detail
  * - 唤醒条件：软件请求或总线活动
  * - 需要重新同步总线时序
  * - 唤醒后自动进入正常模式
  */
uint8_t CAN_WakeUp(CAN_TypeDef* CANx)
{
  uint32_t wait_slak = SLAK_TIMEOUT;  // 唤醒超时计数器（建议值1000-10000）
  uint8_t wakeupstatus = CAN_WakeUp_Failed;

  /* 参数合法性检查 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));

  /*---- 发送唤醒请求 ----*/
  /* 清除MCR寄存器的睡眠模式请求位 */
  CANx->MCR &= ~(uint32_t)CAN_MCR_SLEEP;  // 硬件自动开始唤醒流程

  /*---- 等待唤醒完成 ----*/
  /* 轮询检查SLAK位状态，直到退出睡眠模式或超时 */
  while (((CANx->MSR & CAN_MSR_SLAK) == CAN_MSR_SLAK) && (wait_slak != 0x00)) 
  {
    wait_slak--;  // 每次循环递减超时计数器
  }

  /*---- 验证唤醒结果 ----*/
  if ((CANx->MSR & CAN_MSR_SLAK) != CAN_MSR_SLAK) 
  {
    wakeupstatus = CAN_WakeUp_Ok;  // SLAK位清零表示唤醒成功
  }

  return (uint8_t)wakeupstatus;
}

/**
  * @}
  */


/** @defgroup CAN_Group5 CAN Bus Error management functions
 *  @brief    CAN Bus Error management functions 
 *
@verbatim    
 ===============================================================================
                ##### CAN Bus Error management functions #####
 ===============================================================================  
    [..] This section provides functions allowing to 
      (+) Return the CANx's last error code (LEC)
      (+) Return the CANx Receive Error Counter (REC)
      (+) Return the LSB of the 9-bit CANx Transmit Error Counter(TEC).
   
      -@- If TEC is greater than 255, The CAN is in bus-off state.
      -@- if REC or TEC are greater than 96, an Error warning flag occurs.
      -@- if REC or TEC are greater than 127, an Error Passive Flag occurs.
                        
@endverbatim
  * @{
  */
  
/**
  * @brief  Returns the CANx's last error code (LEC).
  * @param  CANx: where x can be 1 or 2 to select the CAN peripheral.
  * @retval Error code: 
  *          - CAN_ERRORCODE_NoErr: No Error  
  *          - CAN_ERRORCODE_StuffErr: Stuff Error
  *          - CAN_ERRORCODE_FormErr: Form Error
  *          - CAN_ERRORCODE_ACKErr : Acknowledgment Error
  *          - CAN_ERRORCODE_BitRecessiveErr: Bit Recessive Error
  *          - CAN_ERRORCODE_BitDominantErr: Bit Dominant Error
  *          - CAN_ERRORCODE_CRCErr: CRC Error
  *          - CAN_ERRORCODE_SoftwareSetErr: Software Set Error  
  */
/**
  * @brief  获取最后一次CAN通信错误代码
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @retval 错误代码 (CAN_ESR_LEC位段值):
  *   - 0x00: 无错误
  *   - 0x01: 填充错误（位填充规则违反）
  *   - 0x02: 格式错误（固定格式位不正确）
  *   - 0x03: ACK错误（发送时未收到应答）
  *   - 0x04: 隐性位错误（发送显性但监测到隐性）
  *   - 0x05: 显性位错误（发送隐性但监测到显性）
  *   - 0x06: CRC错误（校验和不匹配）
  *   - 0x07: 软件设置错误（通过置位ESR的BOFF位触发）
  * @detail
  * - LEC字段在错误中断时锁定，读取后自动清零
  * - 可用于错误诊断和故障排查
  */
uint8_t CAN_GetLastErrorCode(CAN_TypeDef* CANx)
{
  uint8_t errorcode = 0;
  
  /* 参数合法性检查 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  
  /* 从ESR寄存器提取LEC错误代码（bit[2:0]） */
  errorcode = ((uint8_t)CANx->ESR) & (uint8_t)CAN_ESR_LEC;
  
  return errorcode;
}

/**
  * @brief  获取接收错误计数器当前值
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @retval 接收错误计数器值 (0-255)
  * @detail
  * - REC > 127时进入错误被动状态
  * - 错误计数规则：
  *   成功接收：-1（若REC <= 128）或置为120（REC > 128）
  *   接收错误：+1（格式错误）或+8（其他错误）
  * - 结合TEC判断总线状态
  */
uint8_t CAN_GetReceiveErrorCounter(CAN_TypeDef* CANx)
{
  uint8_t counter = 0;
  
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  
  /* 从ESR[31:24]提取REC值 */
  counter = (uint8_t)((CANx->ESR & CAN_ESR_REC) >> 24);
  
  return counter;
}

/**
  * @brief  获取发送错误计数器低8位
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @retval 发送错误计数器低8位 (TEC[7:0])
  * @detail
  * - TEC是9位计数器，本函数返回低8位
  * - 完整TEC值需结合CAN_GetLSBTransmitErrorCounter()和BOFF状态
  * - 错误计数规则：
  *   发送成功：-1（若TEC > 0）
  *   发送错误：+8（仲裁丢失或总线错误）
  * - TEC > 255时触发总线关闭（BOFF状态）
  */
uint8_t CAN_GetLSBTransmitErrorCounter(CAN_TypeDef* CANx)
{
  uint8_t counter = 0;
  
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  
  /* 从ESR[23:16]提取TEC低8位 */
  counter = (uint8_t)((CANx->ESR & CAN_ESR_TEC) >> 16);
  
  return counter;
}

/**
  * @}
  */

/** @defgroup CAN_Group6 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
              ##### Interrupts and flags management functions #####
 ===============================================================================  

     [..] This section provides functions allowing to configure the CAN Interrupts 
          and to get the status and clear flags and Interrupts pending bits.
  
          The CAN provides 14 Interrupts sources and 15 Flags:

   
  *** Flags ***
  =============
    [..] The 15 flags can be divided on 4 groups: 

      (+) Transmit Flags
        (++) CAN_FLAG_RQCP0, 
        (++) CAN_FLAG_RQCP1, 
        (++) CAN_FLAG_RQCP2  : Request completed MailBoxes 0, 1 and 2  Flags
                               Set when when the last request (transmit or abort)
                               has been performed. 

      (+) Receive Flags


        (++) CAN_FLAG_FMP0,
        (++) CAN_FLAG_FMP1   : FIFO 0 and 1 Message Pending Flags 
                               set to signal that messages are pending in the receive 
                               FIFO.
                               These Flags are cleared only by hardware. 

        (++) CAN_FLAG_FF0,
        (++) CAN_FLAG_FF1    : FIFO 0 and 1 Full Flags
                               set when three messages are stored in the selected 
                               FIFO.                        

        (++) CAN_FLAG_FOV0              
        (++) CAN_FLAG_FOV1   : FIFO 0 and 1 Overrun Flags
                               set when a new message has been received and passed 
                               the filter while the FIFO was full.         

      (+) Operating Mode Flags

        (++) CAN_FLAG_WKU    : Wake up Flag
                               set to signal that a SOF bit has been detected while 
                               the CAN hardware was in Sleep mode. 
        
        (++) CAN_FLAG_SLAK   : Sleep acknowledge Flag
                               Set to signal that the CAN has entered Sleep Mode. 
    
      (+) Error Flags

        (++) CAN_FLAG_EWG    : Error Warning Flag
                               Set when the warning limit has been reached (Receive 
                               Error Counter or Transmit Error Counter greater than 96). 
                               This Flag is cleared only by hardware.
                            
        (++) CAN_FLAG_EPV    : Error Passive Flag
                               Set when the Error Passive limit has been reached 
                               (Receive Error Counter or Transmit Error Counter 
                               greater than 127).
                               This Flag is cleared only by hardware.
                             
        (++) CAN_FLAG_BOF    : Bus-Off Flag
                               set when CAN enters the bus-off state. The bus-off 
                               state is entered on TEC overflow, greater than 255.
                               This Flag is cleared only by hardware.
                                   
        (++) CAN_FLAG_LEC    : Last error code Flag
                               set If a message has been transferred (reception or
                               transmission) with error, and the error code is hold.              
                           
  *** Interrupts ***
  ==================
    [..] The 14 interrupts can be divided on 4 groups: 
  
      (+) Transmit interrupt
  
        (++) CAN_IT_TME   :  Transmit mailbox empty Interrupt
                             if enabled, this interrupt source is pending when 
                             no transmit request are pending for Tx mailboxes.      

      (+) Receive Interrupts
         
        (++) CAN_IT_FMP0,
        (++) CAN_IT_FMP1    :  FIFO 0 and FIFO1 message pending Interrupts
                               if enabled, these interrupt sources are pending 
                               when messages are pending in the receive FIFO.
                               The corresponding interrupt pending bits are cleared 
                               only by hardware.
                
        (++) CAN_IT_FF0,              
        (++) CAN_IT_FF1     :  FIFO 0 and FIFO1 full Interrupts
                               if enabled, these interrupt sources are pending 
                               when three messages are stored in the selected FIFO.
        
        (++) CAN_IT_FOV0,        
        (++) CAN_IT_FOV1    :  FIFO 0 and FIFO1 overrun Interrupts        
                               if enabled, these interrupt sources are pending 
                               when a new message has been received and passed 
                               the filter while the FIFO was full.

      (+) Operating Mode Interrupts
         
        (++) CAN_IT_WKU     :  Wake-up Interrupt
                               if enabled, this interrupt source is pending when 
                               a SOF bit has been detected while the CAN hardware 
                               was in Sleep mode.
                                  
        (++) CAN_IT_SLK     :  Sleep acknowledge Interrupt
                               if enabled, this interrupt source is pending when 
                               the CAN has entered Sleep Mode.       

      (+) Error Interrupts 
        
        (++) CAN_IT_EWG     :  Error warning Interrupt 
                               if enabled, this interrupt source is pending when
                               the warning limit has been reached (Receive Error 
                               Counter or Transmit Error Counter=96). 
                               
        (++) CAN_IT_EPV     :  Error passive Interrupt        
                               if enabled, this interrupt source is pending when
                               the Error Passive limit has been reached (Receive 
                               Error Counter or Transmit Error Counter>127).
                          
        (++) CAN_IT_BOF     :  Bus-off Interrupt
                               if enabled, this interrupt source is pending when
                               CAN enters the bus-off state. The bus-off state is 
                               entered on TEC overflow, greater than 255.
                               This Flag is cleared only by hardware.
                                  
        (++) CAN_IT_LEC     :  Last error code Interrupt        
                               if enabled, this interrupt source is pending  when
                               a message has been transferred (reception or
                               transmission) with error, and the error code is hold.
                          
        (++) CAN_IT_ERR     :  Error Interrupt
                               if enabled, this interrupt source is pending when 
                               an error condition is pending.      
                      
    [..] Managing the CAN controller events :
 
         The user should identify which mode will be used in his application to 
         manage the CAN controller events: Polling mode or Interrupt mode.
  
      (#) In the Polling Mode it is advised to use the following functions:
        (++) CAN_GetFlagStatus() : to check if flags events occur. 
        (++) CAN_ClearFlag()     : to clear the flags events.
  

  
      (#) In the Interrupt Mode it is advised to use the following functions:
        (++) CAN_ITConfig()       : to enable or disable the interrupt source.
        (++) CAN_GetITStatus()    : to check if Interrupt occurs.
        (++) CAN_ClearITPendingBit() : to clear the Interrupt pending Bit 
            (corresponding Flag).
        -@@-  This function has no impact on CAN_IT_FMP0 and CAN_IT_FMP1 Interrupts 
             pending bits since there are cleared only by hardware. 
  
@endverbatim
  * @{
  */ 
/**
  * @brief  Enables or disables the specified CANx interrupts.
  * @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
  * @param  CAN_IT: specifies the CAN interrupt sources to be enabled or disabled.
  *          This parameter can be: 
  *            @arg CAN_IT_TME: Transmit mailbox empty Interrupt 
  *            @arg CAN_IT_FMP0: FIFO 0 message pending Interrupt 
  *            @arg CAN_IT_FF0: FIFO 0 full Interrupt
  *            @arg CAN_IT_FOV0: FIFO 0 overrun Interrupt
  *            @arg CAN_IT_FMP1: FIFO 1 message pending Interrupt 
  *            @arg CAN_IT_FF1: FIFO 1 full Interrupt
  *            @arg CAN_IT_FOV1: FIFO 1 overrun Interrupt
  *            @arg CAN_IT_WKU: Wake-up Interrupt
  *            @arg CAN_IT_SLK: Sleep acknowledge Interrupt  
  *            @arg CAN_IT_EWG: Error warning Interrupt
  *            @arg CAN_IT_EPV: Error passive Interrupt
  *            @arg CAN_IT_BOF: Bus-off Interrupt  
  *            @arg CAN_IT_LEC: Last error code Interrupt
  *            @arg CAN_IT_ERR: Error Interrupt
  * @param  NewState: new state of the CAN interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  CAN中断使能/禁用控制函数
  * @param  CANx: CAN外设选择(CAN1/CAN2)
  * @param  CAN_IT: 中断类型，可选值：
  *   - CAN_IT_TME: 发送邮箱空中断
  *   - CAN_IT_FMP0: FIFO0消息挂起中断
  *   - CAN_IT_FMP1: FIFO1消息挂起中断
  *   - CAN_IT_FF0:  FIFO0满中断
  *   - CAN_IT_FOV0: FIFO0溢出中断
  *   - CAN_IT_FF1:  FIFO1满中断
  *   - CAN_IT_FOV1: FIFO1溢出中断
  *   - CAN_IT_EWG:  错误警告中断（TEC/REC超过阈值）
  *   - CAN_IT_EPV:  错误被动中断（进入错误被动状态）
  *   - CAN_IT_BOF:  总线关闭中断
  *   - CAN_IT_LEC:  最后错误代码改变中断
  *   - CAN_IT_ERR:  错误中断（综合错误）
  * @param  NewState: 中断新状态 (ENABLE/DISABLE)
  * @retval 无
  * @detail
  * - 启用中断后需配置NVIC控制器
  * - 多个中断类型可通过位或操作组合配置
  * - 清除中断标志需调用CAN_ClearITPendingBit()
  */
void CAN_ITConfig(CAN_TypeDef* CANx, uint32_t CAN_IT, FunctionalState NewState)
{
  /* 参数合法性检查 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));
  assert_param(IS_CAN_IT(CAN_IT));           // 验证中断类型有效性
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 验证状态有效性

  if (NewState != DISABLE)
  {
    /* 设置IER寄存器对应位使能中断 */
    CANx->IER |= CAN_IT;  // 使用位或操作添加中断使能位
  }
  else
  {
    /* 清除IER寄存器对应位禁用中断 */
    CANx->IER &= ~CAN_IT; // 使用位与操作移除中断使能位
  }
}

/**
  * @brief  Checks whether the specified CAN flag is set or not.
  * @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
  * @param  CAN_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg CAN_FLAG_RQCP0: Request MailBox0 Flag
  *            @arg CAN_FLAG_RQCP1: Request MailBox1 Flag
  *            @arg CAN_FLAG_RQCP2: Request MailBox2 Flag
  *            @arg CAN_FLAG_FMP0: FIFO 0 Message Pending Flag   
  *            @arg CAN_FLAG_FF0: FIFO 0 Full Flag       
  *            @arg CAN_FLAG_FOV0: FIFO 0 Overrun Flag 
  *            @arg CAN_FLAG_FMP1: FIFO 1 Message Pending Flag   
  *            @arg CAN_FLAG_FF1: FIFO 1 Full Flag        
  *            @arg CAN_FLAG_FOV1: FIFO 1 Overrun Flag     
  *            @arg CAN_FLAG_WKU: Wake up Flag
  *            @arg CAN_FLAG_SLAK: Sleep acknowledge Flag 
  *            @arg CAN_FLAG_EWG: Error Warning Flag
  *            @arg CAN_FLAG_EPV: Error Passive Flag  
  *            @arg CAN_FLAG_BOF: Bus-Off Flag    
  *            @arg CAN_FLAG_LEC: Last error code Flag      
  * @retval The new state of CAN_FLAG (SET or RESET).
  */
/**
  * @brief  获取指定CAN标志位的状态
  * @param  CANx: 指向CAN外设的指针，范围限定为CAN1或CAN2
  * @param  CAN_FLAG: 要检查的标志位，支持以下类型：
  *            - ESR寄存器标志：CAN_FLAG_EWG, CAN_FLAG_EPV, CAN_FLAG_BOF, CAN_FLAG_LEC
  *            - MSR寄存器标志：CAN_FLAG_RX, CAN_FLAG_SLAKI, CAN_FLAG_WKUI, CAN_FLAG_SLAK  
  *            - TSR寄存器标志：CAN_FLAG_TME0, CAN_FLAG_TME1, CAN_FLAG_TME2, LOWx标志
  *            - RF0R/RF1R寄存器标志：FMPx, FULLx, FOVRx, RFOMx
  * @retval FlagStatus: 标志位状态，返回SET表示标志置位，RESET表示标志未置位
  * @note 函数包含参数有效性检查，使用断言机制验证输入参数合法性
  */
FlagStatus CAN_GetFlagStatus(CAN_TypeDef* CANx, uint32_t CAN_FLAG)
{
  FlagStatus bitstatus = RESET;  // 初始化返回状态为RESET
  
  /* 参数有效性验证 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));        // 验证CANx是有效外设(CAN1/CAN2)
  assert_param(IS_CAN_GET_FLAG(CAN_FLAG));     // 验证标志位参数在合法范围内

  /* 判断标志位所属寄存器分组 */
  if((CAN_FLAG & CAN_FLAGS_ESR) != (uint32_t)RESET)  // 当标志位属于错误状态寄存器(ESR)时
  { 
    /* 检查ESR寄存器中指定标志位状态 */
    if ((CANx->ESR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)  // 通过掩码获取有效标志位并与寄存器值按位与
    { 
      bitstatus = SET;  // 标志位置位
    }
    else
    { 
      bitstatus = RESET;  // 标志位未置位
    }
  }
  /* 处理主控制寄存器(MSR)标志 */
  else if((CAN_FLAG & CAN_FLAGS_MSR) != (uint32_t)RESET)  // 当标志位属于主控制寄存器时
  { 
    if ((CANx->MSR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)  // 检查MSR寄存器标志位
    { 
      bitstatus = SET;
    }
    else
    { 
      bitstatus = RESET;
    }
  }
  /* 处理发送状态寄存器(TSR)标志 */
  else if((CAN_FLAG & CAN_FLAGS_TSR) != (uint32_t)RESET)  // 当标志位属于发送状态寄存器时
  { 
    if ((CANx->TSR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)  // 检查TSR寄存器标志位
    { 
      bitstatus = SET;
    }
    else
    { 
      bitstatus = RESET;
    }
  }
  /* 处理接收FIFO0寄存器(RF0R)标志 */
  else if((CAN_FLAG & CAN_FLAGS_RF0R) != (uint32_t)RESET)  // 当标志位属于接收FIFO0寄存器时
  { 
    if ((CANx->RF0R & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)  // 检查RF0R寄存器标志位
    { 
      bitstatus = SET;
    }
    else
    { 
      bitstatus = RESET;
    }
  }
  /* 处理接收FIFO1寄存器(RF1R)标志（最后一个条件分支） */
  else  // 隐含条件：(CAN_FLAG & CAN_FLAGS_RF1R) != RESET
  { 
    if ((uint32_t)(CANx->RF1R & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)  // 检查RF1R寄存器标志位
    { 
      bitstatus = SET;
    }
    else
    { 
      bitstatus = RESET;
    }
  }
  
  return  bitstatus;  // 返回最终检测到的标志位状态
}

/**
  * @brief  清除CAN外设的指定挂起标志位
  * @param  CANx: 指向CAN外设的指针，取值范围为CAN1或CAN2
  * @param  CAN_FLAG: 要清除的标志位，支持以下类型：
  *            @arg CAN_FLAG_RQCP0: 邮箱0请求完成标志
  *            @arg CAN_FLAG_RQCP1: 邮箱1请求完成标志
  *            @arg CAN_FLAG_RQCP2: 邮箱2请求完成标志
  *            @arg CAN_FLAG_FF0: FIFO 0满标志       
  *            @arg CAN_FLAG_FOV0: FIFO 0溢出标志  
  *            @arg CAN_FLAG_FF1: FIFO 1满标志        
  *            @arg CAN_FLAG_FOV1: FIFO 1溢出标志     
  *            @arg CAN_FLAG_WKU: 唤醒事件标志
  *            @arg CAN_FLAG_SLAK: 睡眠模式应答标志    
  *            @arg CAN_FLAG_LEC: 最后错误代码标志        
  * @retval 无
  * @note 1. 清除LEC标志需要特殊处理，直接重置ESR寄存器
  *       2. 其他标志通过写入对应寄存器实现清除
  *       3. 使用断言验证参数合法性
  */
void CAN_ClearFlag(CAN_TypeDef* CANx, uint32_t CAN_FLAG)
{
  uint32_t flagtmp=0;
  
  /* 参数有效性验证 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));      // 验证CANx是有效外设(CAN1/CAN2)
  assert_param(IS_CAN_CLEAR_FLAG(CAN_FLAG)); // 验证清除标志参数合法性
  
  /* 特殊处理LEC错误码标志 */
  if (CAN_FLAG == CAN_FLAG_LEC) /* ESR寄存器处理 */
  {
    /* 通过写入RESET清除ESR寄存器的所有错误标志 */
    CANx->ESR = (uint32_t)RESET;  // 写0清除整个错误状态寄存器
  }
  else /* 处理其他寄存器的标志位 */
  {
    /* 生成有效标志掩码（保留低20位） */
    flagtmp = CAN_FLAG & 0x000FFFFF;  // 提取有效的标志位域
    
    /* 根据标志类型选择目标寄存器 */
    if ((CAN_FLAG & CAN_FLAGS_RF0R)!=(uint32_t)RESET)  // 接收FIFO0相关标志
    {
      /* 清除接收FIFO0的标志位 */
      CANx->RF0R = (uint32_t)(flagtmp);  // 写入标志位会清除对应状态
    }
    else if ((CAN_FLAG & CAN_FLAGS_RF1R)!=(uint32_t)RESET)  // 接收FIFO1相关标志
    {
      /* 清除接收FIFO1的标志位 */
      CANx->RF1R = (uint32_t)(flagtmp);
    }
    else if ((CAN_FLAG & CAN_FLAGS_TSR)!=(uint32_t)RESET)  // 发送状态相关标志
    {
      /* 清除发送邮箱状态标志 */
      CANx->TSR = (uint32_t)(flagtmp);
    }
    else /* 默认处理主控制寄存器(MSR)标志 */
    {
      /* 清除操作模式相关标志（睡眠/唤醒等） */
      CANx->MSR = (uint32_t)(flagtmp);
    }
  }
}


/**
  * @brief  Checks whether the specified CANx interrupt has occurred or not.
  * @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
  * @param  CAN_IT: specifies the CAN interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg CAN_IT_TME: Transmit mailbox empty Interrupt 
  *            @arg CAN_IT_FMP0: FIFO 0 message pending Interrupt 
  *            @arg CAN_IT_FF0: FIFO 0 full Interrupt
  *            @arg CAN_IT_FOV0: FIFO 0 overrun Interrupt
  *            @arg CAN_IT_FMP1: FIFO 1 message pending Interrupt 
  *            @arg CAN_IT_FF1: FIFO 1 full Interrupt
  *            @arg CAN_IT_FOV1: FIFO 1 overrun Interrupt
  *            @arg CAN_IT_WKU: Wake-up Interrupt
  *            @arg CAN_IT_SLK: Sleep acknowledge Interrupt  
  *            @arg CAN_IT_EWG: Error warning Interrupt
  *            @arg CAN_IT_EPV: Error passive Interrupt
  *            @arg CAN_IT_BOF: Bus-off Interrupt  
  *            @arg CAN_IT_LEC: Last error code Interrupt
  *            @arg CAN_IT_ERR: Error Interrupt
  * @retval The current state of CAN_IT (SET or RESET).
  */
/**
  * @brief  获取CAN外设的中断状态
  * @param  CANx: 指向CAN外设的指针，取值范围为CAN1或CAN2
  * @param  CAN_IT: 要查询的中断类型，支持以下类型：
  *            @arg CAN_IT_TME: 发送邮箱空中断
  *            @arg CAN_IT_FMP0: FIFO0消息挂起中断
  *            @arg CAN_IT_FF0: FIFO0满中断
  *            @arg CAN_IT_FOV0: FIFO0溢出中断
  *            @arg CAN_IT_FMP1: FIFO1消息挂起中断
  *            @arg CAN_IT_FF1: FIFO1满中断
  *            @arg CAN_IT_FOV1: FIFO1溢出中断
  *            @arg CAN_IT_WKU: 唤醒中断
  *            @arg CAN_IT_SLK: 睡眠确认中断
  *            @arg CAN_IT_EWG: 错误警告中断
  *            @arg CAN_IT_EPV: 错误被动中断
  *            @arg CAN_IT_BOF: 总线关闭中断
  *            @arg CAN_IT_LEC: 最后错误码中断
  *            @arg CAN_IT_ERR: 错误中断
  * @retval ITStatus: 中断状态，返回SET表示中断发生，RESET表示无中断
  * @note 1. 需先检查中断使能状态（IER寄存器）
  *       2. 使用CheckITStatus辅助函数检测具体状态位
  */
ITStatus CAN_GetITStatus(CAN_TypeDef* CANx, uint32_t CAN_IT)
{
  ITStatus itstatus = RESET;  // 初始化中断状态为RESET
  
  /* 参数有效性验证 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));  // 验证CAN外设有效性
  assert_param(IS_CAN_IT(CAN_IT));       // 验证中断类型参数合法性

  /* 检查中断使能位状态 */
  if((CANx->IER & CAN_IT) != RESET)  // 判断指定中断是否已使能
  {
    /* 根据中断类型选择检测逻辑 */
    switch (CAN_IT)
    {
      case CAN_IT_TME:  /* 发送邮箱空中断 */
        /* 检测TSR寄存器的所有RQCP标志位（0-2号邮箱） */
        itstatus = CheckITStatus(CANx->TSR, CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2);
        break;
        
      case CAN_IT_FMP0:  /* FIFO0消息挂起中断 */
        /* 检测RF0R寄存器的FMP0位（FIFO0消息计数器） */
        itstatus = CheckITStatus(CANx->RF0R, CAN_RF0R_FMP0);
        break;

      case CAN_IT_FF0:  /* FIFO0满中断 */
        /* 检测RF0R寄存器的FULL0位 */
        itstatus = CheckITStatus(CANx->RF0R, CAN_RF0R_FULL0);
        break;

      case CAN_IT_FOV0:  /* FIFO0溢出中断 */
        /* 检测RF0R寄存器的FOVR0位 */
        itstatus = CheckITStatus(CANx->RF0R, CAN_RF0R_FOVR0);
        break;

      case CAN_IT_FMP1:  /* FIFO1消息挂起中断 */
        /* 检测RF1R寄存器的FMP1位 */
        itstatus = CheckITStatus(CANx->RF1R, CAN_RF1R_FMP1);
        break;

      case CAN_IT_FF1:  /* FIFO1满中断 */
        /* 检测RF1R寄存器的FULL1位 */
        itstatus = CheckITStatus(CANx->RF1R, CAN_RF1R_FULL1);
        break;

      case CAN_IT_FOV1:  /* FIFO1溢出中断 */
        /* 检测RF1R寄存器的FOVR1位 */
        itstatus = CheckITStatus(CANx->RF1R, CAN_RF1R_FOVR1);
        break;

      case CAN_IT_WKU:  /* 唤醒中断 */
        /* 检测MSR寄存器的WKUI位（唤醒中断标志） */
        itstatus = CheckITStatus(CANx->MSR, CAN_MSR_WKUI);
        break;

      case CAN_IT_SLK:  /* 睡眠确认中断 */
        /* 检测MSR寄存器的SLAKI位（睡眠确认中断） */
        itstatus = CheckITStatus(CANx->MSR, CAN_MSR_SLAKI);
        break;

      case CAN_IT_EWG:  /* 错误警告中断 */
        /* 检测ESR寄存器的EWGF位（错误警告标志） */
        itstatus = CheckITStatus(CANx->ESR, CAN_ESR_EWGF);
        break;

      case CAN_IT_EPV:  /* 错误被动中断 */
        /* 检测ESR寄存器的EPVF位（错误被动标志） */
        itstatus = CheckITStatus(CANx->ESR, CAN_ESR_EPVF);
        break;

      case CAN_IT_BOF:  /* 总线关闭中断 */
        /* 检测ESR寄存器的BOFF位（总线关闭标志） */
        itstatus = CheckITStatus(CANx->ESR, CAN_ESR_BOFF);
        break;

      case CAN_IT_LEC:  /* 最后错误码中断 */
        /* 检测ESR寄存器的LEC位（最后错误代码） */
        itstatus = CheckITStatus(CANx->ESR, CAN_ESR_LEC);
        break;

      case CAN_IT_ERR:  /* 错误中断 */
        /* 检测MSR寄存器的ERRI位（错误中断标志） */
        itstatus = CheckITStatus(CANx->MSR, CAN_MSR_ERRI);
        break;

      default:  /* 未知中断类型处理 */
        itstatus = RESET;  // 返回默认RESET状态
        break;
    }
  }
  else  /* 中断未使用的情况处理 */
  {
    itstatus  = RESET;  // 当对应中断未启用时直接返回RESET
  }
  
  return  itstatus;  // 返回最终检测到的中断状态
}

/**
  * @brief  Clears the CANx's interrupt pending bits.
  * @param  CANx: where x can be 1 or 2 to to select the CAN peripheral.
  * @param  CAN_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be one of the following values:
  *            @arg CAN_IT_TME: Transmit mailbox empty Interrupt
  *            @arg CAN_IT_FF0: FIFO 0 full Interrupt
  *            @arg CAN_IT_FOV0: FIFO 0 overrun Interrupt
  *            @arg CAN_IT_FF1: FIFO 1 full Interrupt
  *            @arg CAN_IT_FOV1: FIFO 1 overrun Interrupt
  *            @arg CAN_IT_WKU: Wake-up Interrupt
  *            @arg CAN_IT_SLK: Sleep acknowledge Interrupt  
  *            @arg CAN_IT_EWG: Error warning Interrupt
  *            @arg CAN_IT_EPV: Error passive Interrupt
  *            @arg CAN_IT_BOF: Bus-off Interrupt  
  *            @arg CAN_IT_LEC: Last error code Interrupt
  *            @arg CAN_IT_ERR: Error Interrupt 
  * @retval None
  */
/**
  * @brief  清除指定的CAN中断挂起标志位
  * @param  CANx: CAN外设实例指针，取值范围为CAN1或CAN2
  * @param  CAN_IT: 要清除的中断类型，支持以下值：
  *            @arg CAN_IT_TME: 发送邮箱空中断
  *            @arg CAN_IT_FF0: FIFO0满中断
  *            @arg CAN_IT_FOV0: FIFO0溢出中断
  *            @arg CAN_IT_FF1: FIFO1满中断
  *            @arg CAN_IT_FOV1: FIFO1溢出中断
  *            @arg CAN_IT_WKU: 唤醒中断
  *            @arg CAN_IT_SLK: 睡眠确认中断
  *            @arg CAN_IT_EWG: 错误警告中断
  *            @arg CAN_IT_EPV: 错误被动中断
  *            @arg CAN_IT_BOF: 总线关闭中断
  *            @arg CAN_IT_LEC: 最后错误代码中断
  *            @arg CAN_IT_ERR: 错误中断
  * @retval 无
  * @note   1. 清除标志位的原理是通过向对应标志位写1（rc_w1机制）
  *         2. 部分错误标志的最终清除依赖CAN总线的状态变化
  *         3. 操作寄存器前会进行参数有效性检查
  */
void CAN_ClearITPendingBit(CAN_TypeDef* CANx, uint32_t CAN_IT)
{
  /* 参数合法性检查 */
  assert_param(IS_CAN_ALL_PERIPH(CANx));  // 验证CANx是有效外设(CAN1/CAN2)
  assert_param(IS_CAN_CLEAR_IT(CAN_IT));  // 验证中断类型在允许范围内

  /* 根据中断类型执行不同的清除操作 */
  switch (CAN_IT)
  {
    case CAN_IT_TME:
      /* 清除发送请求完成标志（3个发送邮箱的RQCP位） */
      /* 通过向TSR寄存器的RQCP0/RQCP1/RQCP2位写1清零 */
      CANx->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;  
      break;
      
    case CAN_IT_FF0:
      /* 清除FIFO0满标志 */
      /* 向RF0R寄存器的FULL0位写1清零 */
      CANx->RF0R = CAN_RF0R_FULL0; 
      break;
      
    case CAN_IT_FOV0:
      /* 清除FIFO0溢出标志 */
      /* 向RF0R寄存器的FOVR0位写1清零 */
      CANx->RF0R = CAN_RF0R_FOVR0; 
      break;
      
    case CAN_IT_FF1:
      /* 清除FIFO1满标志 */
      /* 向RF1R寄存器的FULL1位写1清零 */
      CANx->RF1R = CAN_RF1R_FULL1;  
      break;
      
    case CAN_IT_FOV1:
      /* 清除FIFO1溢出标志 */
      /* 向RF1R寄存器的FOVR1位写1清零 */
      CANx->RF1R = CAN_RF1R_FOVR1; 
      break;
      
    case CAN_IT_WKU:
      /* 清除唤醒中断标志 */
      /* 向MSR寄存器的WKUI位写1清零 */
      CANx->MSR = CAN_MSR_WKUI;  
      break;
      
    case CAN_IT_SLK:
      /* 清除睡眠确认中断标志 */
      /* 向MSR寄存器的SLAKI位写1清零 */
      CANx->MSR = CAN_MSR_SLAKI;   
      break;
      
    case CAN_IT_EWG:
      /* 清除错误警告中断标志 */
      /* 通过清除MSR寄存器的ERRI位（写1清零）*/
      CANx->MSR = CAN_MSR_ERRI;
      /* 注意：实际错误警告标志(EWGF)的清除依赖于总线恢复正常状态 */
      break;
      
    case CAN_IT_EPV:
      /* 清除错误被动中断标志 */
      /* 通过清除MSR寄存器的ERRI位（写1清零）*/
      CANx->MSR = CAN_MSR_ERRI; 
      /* 注意：实际错误被动标志(EPVF)的清除依赖于总线恢复正常状态 */
      break;
      
    case CAN_IT_BOF:
      /* 清除总线关闭中断标志 */
      /* 通过清除MSR寄存器的ERRI位（写1清零）*/
      CANx->MSR = CAN_MSR_ERRI; 
      /* 注意：总线关闭标志(BOFF)的清除依赖于总线恢复正常状态 */
      break;
      
    case CAN_IT_LEC:
      /* 清除最后错误代码 */
      /* 将ESR寄存器全部清零（LEC字段位于ESR[2:0]）*/
      CANx->ESR = RESET; 
      /* 同时清除错误中断标志 */
      CANx->MSR = CAN_MSR_ERRI; 
      break;
      
    case CAN_IT_ERR:
      /* 清除所有错误相关标志 */
      /* 清除LEC错误代码（ESR寄存器清零）*/
      CANx->ESR = RESET; 
      /* 清除主错误中断标志 */
      CANx->MSR = CAN_MSR_ERRI; 
      /* 注意：BOFF/EPVF/EWGF标志最终由总线状态决定 */
      break;
      
    default:
      /* 处理未定义的中断类型（不会执行任何操作） */
      break;
  }
}

 /**
  * @}
  */

/**
  * @brief  检查指定CAN中断是否发生
  * @param  CAN_Reg: CAN中断寄存器值，通常应为以下寄存器之一：
  *            - CAN_TSR: 发送状态寄存器
  *            - CAN_RF0R: 接收FIFO0寄存器
  *            - CAN_RF1R: 接收FIFO1寄存器
  *            - CAN_MSR: 主状态寄存器
  *            - CAN_ESR: 错误状态寄存器
  * @param  It_Bit: 要检测的中断标志位，取值范围取决于CAN_Reg参数：
  *            - 当CAN_Reg为CAN_TSR时，可以是RQCP0/RQCP1/RQCP2等
  *            - 当CAN_Reg为CAN_RF0R时，可以是FULL0/FOVR0等
  *            - 其他寄存器同理对应各自的中断标志位
  * @retval ITStatus 中断状态：
  *            - SET: 中断标志位置位（中断已发生）
  *            - RESET: 中断标志位复位（中断未发生）
  * @note   1. 该函数不进行参数有效性验证，调用前需确保参数正确
  *         2. 实际使用时应先读取对应中断寄存器值再传入本函数
  */
static ITStatus CheckITStatus(uint32_t CAN_Reg, uint32_t It_Bit)
{
  ITStatus pendingbitstatus = RESET;  // 初始化中断状态为未触发
  
  /* 通过位与运算检测指定中断标志位 */
  if ((CAN_Reg & It_Bit) != (uint32_t)RESET)  // RESET宏定义为0
  {
    /* 当目标位非零时，判定中断已触发 */
    pendingbitstatus = SET;  // SET宏定义为1
  }
  else
  {
    /* 保持中断未触发状态 */
    pendingbitstatus = RESET;  // 显式赋值确保状态明确
  }
  return pendingbitstatus;  // 返回最终检测结果
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

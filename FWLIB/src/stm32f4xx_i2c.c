/**
  ******************************************************************************
  * @file    stm32f4xx_i2c.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Inter-integrated circuit (I2C)
  *           + Initialization and Configuration
  *           + Data transfers
  *           + PEC management
  *           + DMA transfers management
  *           + Interrupts, events and flags management 
  *           
    @verbatim    
 ===============================================================================
                    ##### How to use this driver #####
 ===============================================================================
    [..]
      (#) Enable peripheral clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE)
          function for I2C1, I2C2 or I2C3.
  
      (#) Enable SDA, SCL  and SMBA (when used) GPIO clocks using 
          RCC_AHBPeriphClockCmd() function. 
  
      (#) Peripherals alternate function: 
        (++) Connect the pin to the desired peripherals' Alternate 
             Function (AF) using GPIO_PinAFConfig() function
        (++) Configure the desired pin in alternate function by:
             GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
        (++) Select the type, pull-up/pull-down and output speed via 
             GPIO_PuPd, GPIO_OType and GPIO_Speed members
        (++) Call GPIO_Init() function
             Recommended configuration is Push-Pull, Pull-up, Open-Drain.
             Add an external pull up if necessary (typically 4.7 KOhm).      
          
      (#) Program the Mode, duty cycle , Own address, Ack, Speed and Acknowledged
          Address using the I2C_Init() function.
  
      (#) Optionally you can enable/configure the following parameters without
          re-initialization (i.e there is no need to call again I2C_Init() function):
        (++) Enable the acknowledge feature using I2C_AcknowledgeConfig() function
        (++) Enable the dual addressing mode using I2C_DualAddressCmd() function
        (++) Enable the general call using the I2C_GeneralCallCmd() function
        (++) Enable the clock stretching using I2C_StretchClockCmd() function
        (++) Enable the fast mode duty cycle using the I2C_FastModeDutyCycleConfig()
             function.
        (++) Configure the NACK position for Master Receiver mode in case of 
             2 bytes reception using the function I2C_NACKPositionConfig().  
        (++) Enable the PEC Calculation using I2C_CalculatePEC() function
        (++) For SMBus Mode: 
          (+++) Enable the Address Resolution Protocol (ARP) using I2C_ARPCmd() function
          (+++) Configure the SMBusAlert pin using I2C_SMBusAlertConfig() function
  
      (#) Enable the NVIC and the corresponding interrupt using the function 
          I2C_ITConfig() if you need to use interrupt mode. 
  
      (#) When using the DMA mode 
        (++) Configure the DMA using DMA_Init() function
        (++) Active the needed channel Request using I2C_DMACmd() or
             I2C_DMALastTransferCmd() function.
        -@@- When using DMA mode, I2C interrupts may be used at the same time to
             control the communication flow (Start/Stop/Ack... events and errors).
   
      (#) Enable the I2C using the I2C_Cmd() function.
   
      (#) Enable the DMA using the DMA_Cmd() function when using DMA mode in the 
          transfers. 
  
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
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup I2C 
  * @brief I2C driver modules
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/**
  * @brief I2C控制寄存器1(CR1)清除掩码
  * @note  该掩码用于在修改CR1寄存器时保留必要位（0保留，1清除）
  *        对应二进制：1111 1011 1111 0101
  *        保留位说明：
  *        - 位11: SMbus模式（保留SMBus配置）
  *        - 位9:  SMBus告警（保留告警状态）
  *        - 位8:  保留位（根据手册要求保留）
  *        - 位2:  时钟延展（保留时钟控制配置）
  */
#define CR1_CLEAR_MASK    ((uint16_t)0xFBF5)      

/**
  * @brief I2C状态标志寄存器(SR1/SR2)掩码
  * @note  用于提取24个有效状态标志位（低24位）
  *        对应二进制：0000 0000 1111 1111 1111 1111 1111 1111
  *        应用场景：
  *        - 读取SR1/SR2组合状态时过滤无效位
  *        - 检查特定状态标志前进行位掩码操作
  */
#define FLAG_MASK         ((uint32_t)0x00FFFFFF)  

/**
  * @brief I2C中断使能控制掩码
  * @note  对应CR2寄存器中断使能位（bits 9-11）
  *        二进制：0000 0111 0000 0000 0000 0000 0000 0000
  *        包含三个中断使能位：
  *        - 位10: 缓冲区中断使能（ITEVTEN）
  *        - 位11: 错误中断使能（ITERREN）
  *        - 位9:  总线事件中断使能（ITBUFEN）
  */
#define ITEN_MASK         ((uint32_t)0x07000000)  

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup I2C_Private_Functions
  * @{
  */

/** @defgroup I2C_Group1 Initialization and Configuration functions
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
  * @brief  I2C外设反初始化函数
  * @param  I2Cx: 选择I2C外设实例，可以是I2C1、I2C2或I2C3
  * @retval 无
  * @note   本函数通过复位对应外设实现以下功能：
  *         - 将I2C所有寄存器恢复默认值
  *         - 清除所有配置状态
  *         - 关闭I2C总线时钟
  *         典型应用场景：需要完全重置I2C配置或切换工作模式前
  */
void I2C_DeInit(I2C_TypeDef* I2Cx)
{
  /* 验证输入参数是否为有效的I2C实例 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));

  if (I2Cx == I2C1)
  {
    /* 启动I2C1硬件复位流程（置位APB1总线上的I2C1复位位） */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);  // 使能复位信号，开始复位
    
    /* 结束I2C1复位（清除复位位），此时寄存器恢复默认值 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE); // 复位完成后必须立即释放
  }
  else if (I2Cx == I2C2)
  {
    /* I2C2复位序列：遵循先置位后清除的标准复位流程 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);  // 保持至少2个时钟周期的复位
    
    /* 复位持续时间由总线时钟决定，STM32硬件自动管理最小复位时间 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE); 
  }
  else 
  {
    /* 处理I2C3或其他可能的I2C实例（根据芯片型号扩展） */
    if (I2Cx == I2C3)
    {
      /* 对I2C3执行标准复位操作（APB1总线上的I2C3复位位操作） */
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C3, ENABLE);  // 复位脉冲宽度由硬件保证
      
      /* 复位结束后外设处于禁用状态，需要重新初始化才能使用 */
      RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C3, DISABLE);   
    }
  }
}
/**
  * @brief  I2C外设初始化函数
  * @param  I2Cx: 选择I2C外设实例，可以是I2C1、I2C2或I2C3
  * @param  I2C_InitStruct: 指向I2C配置结构体的指针，包含以下成员：
  *         - I2C_ClockSpeed: 通信时钟频率（标准模式<=100KHz，快速模式<=400KHz）
  *         - I2C_Mode: 工作模式（I2C/从机模式/SMBus主机/SMBus从机）
  *         - I2C_DutyCycle: 快速模式占空比（2:1或16:9）
  *         - I2C_OwnAddress1: 设备自身地址（7位或10位地址格式）
  *         - I2C_Ack: 应答使能控制
  *         - I2C_AcknowledgedAddress: 地址识别模式（7位/10位地址匹配）
  * @retval 无
  * @note   重要约束条件：
  *         - 快速模式400KHz时要求PCLK1必须是10MHz的整数倍
  *         - 初始化前必须配置好APB1总线时钟
  */
void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct)
{
  uint16_t tmpreg = 0, freqrange = 0;
  uint16_t result = 0x04;
  uint32_t pclk1 = 8000000;
  RCC_ClocksTypeDef  rcc_clocks;

  /* 参数有效性验证（7个不同参数的检查） */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));  // 验证外设实例有效性
  assert_param(IS_I2C_CLOCK_SPEED(I2C_InitStruct->I2C_ClockSpeed)); // 时钟范围检查
  assert_param(IS_I2C_MODE(I2C_InitStruct->I2C_Mode)); // 模式合法性检查
  assert_param(IS_I2C_DUTY_CYCLE(I2C_InitStruct->I2C_DutyCycle)); // 占空比选项检查
  assert_param(IS_I2C_OWN_ADDRESS1(I2C_InitStruct->I2C_OwnAddress1)); // 地址范围验证
  assert_param(IS_I2C_ACK_STATE(I2C_InitStruct->I2C_Ack)); // 应答使能状态检查
  assert_param(IS_I2C_ACKNOWLEDGE_ADDRESS(I2C_InitStruct->I2C_AcknowledgedAddress)); // 地址格式检查

/*---------------------------- I2Cx CR2配置 ------------------------*/
  /* 获取当前CR2寄存器值（保留其他位配置） */
  tmpreg = I2Cx->CR2;
  /* 清除频率设置位FREQ[5:0]（CR2的bit0-5） */
  tmpreg &= (uint16_t)~((uint16_t)I2C_CR2_FREQ); // 准备设置新频率
  
  /* 获取APB1总线时钟频率（I2C的输入时钟源） */
  RCC_GetClocksFreq(&rcc_clocks);
  pclk1 = rcc_clocks.PCLK1_Frequency; // 单位为Hz
  
  /* 计算频率范围参数（将MHz值转换为CR2需要的FREQ[5:0]格式） */
  freqrange = (uint16_t)(pclk1 / 1000000); // 例如：42MHz -> 0x2A
  tmpreg |= freqrange; // 合并到CR2寄存器值
  
  /* 将新配置写入CR2寄存器（设置输入时钟频率） */
  I2Cx->CR2 = tmpreg; // 此时I2C仍处于禁用状态

/*---------------------------- I2Cx CCR配置 ------------------------*/
  /* 禁用I2C外设（修改CCR前必须关闭外设） */
  I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_PE); // 清除PE位
  
  tmpreg = 0; // 重置临时寄存器变量

  /* 标准模式配置（时钟<=100KHz） */
  if (I2C_InitStruct->I2C_ClockSpeed <= 100000)
  {
    /* 计算标准模式分频系数：CCR = PCLK1/(2*ClockSpeed) */
    result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed << 1)); // 等价于除以2
    
    /* 最小值保护（确保CCR >= 0x04） */
    if (result < 0x04)
    {
      result = 0x04;  // 根据STM32手册要求的最小值
    }
    
    /* 设置CCR寄存器值（标准模式不需要设置FS位） */
    tmpreg |= result;    
    /* 配置TRISE寄存器（最大上升时间）：TRISE = FREQRANGE + 1 */
    I2Cx->TRISE = freqrange + 1; // 标准模式最大允许1000ns上升时间
  }
  /* 快速模式配置（时钟<=400KHz） */
  else 
  {
    /* 根据占空比选择不同分频公式 */
    if (I2C_InitStruct->I2C_DutyCycle == I2C_DutyCycle_2)
    {
      /* Tlow/Thigh = 2 时的分频系数计算 */
      result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 3)); // 总周期=Thigh+Tlow=3*Thigh
    }
    else /* 16/9占空比模式 */
    {
      /* Tlow/Thigh = 16/9 时的分频系数计算 */
      result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 25)); // 总周期=25*Thigh/9
      /* 设置占空比选择位 */
      result |= I2C_DutyCycle_16_9; // CCR[14]置位
    }

    /* 最小值保护（确保CCR有效位>=1） */
    if ((result & I2C_CCR_CCR) == 0)
    {
      result |= (uint16_t)0x0001;  // 设置最低有效位
    }
    
    /* 合并快速模式标志位FS（CCR[15]） */
    tmpreg |= (uint16_t)(result | I2C_CCR_FS);
    
    /* 配置TRISE寄存器（快速模式最大300ns上升时间） */
    I2Cx->TRISE = (uint16_t)(((freqrange * 300) / 1000) + 1); // 计算公式：TRISE = (FREQRANGE * 300) / 1000 + 1
  }

  /* 将计算好的CCR值写入寄存器 */
  I2Cx->CCR = tmpreg;
  /* 重新使能I2C外设 */
  I2Cx->CR1 |= I2C_CR1_PE; // 设置PE位启动外设

/*---------------------------- I2Cx CR1配置 ------------------------*/
  tmpreg = I2Cx->CR1; // 获取当前CR1寄存器值
  /* 清除ACK、SMBTYPE和SMBUS位（使用预定义掩码CR1_CLEAR_MASK） */
  tmpreg &= CR1_CLEAR_MASK;
  /* 合并工作模式和应答控制位 */
  tmpreg |= (uint16_t)((uint32_t)I2C_InitStruct->I2C_Mode | I2C_InitStruct->I2C_Ack);
  /* 将新配置写入CR1寄存器 */
  I2Cx->CR1 = tmpreg; // 此时所有控制位已生效

/*---------------------------- I2Cx OAR1配置 -----------------------*/
  /* 设置自身地址和地址识别模式 */
  I2Cx->OAR1 = (I2C_InitStruct->I2C_AcknowledgedAddress | I2C_InitStruct->I2C_OwnAddress1);
  /* OAR1寄存器结构：
     - bit0:  地址识别模式（0:7位地址，1:10位地址）
     - bit1:  双地址模式使能
     - bit15: 地址使能位（固定为1）
     其余位根据地址模式设置 */
}

/**
  * @brief  I2C配置结构体默认值初始化函数
  * @param  I2C_InitStruct: 指向需要初始化的I2C_InitTypeDef结构体的指针
  * @retval 无
  * @note   默认配置参数：
  *         - 时钟速度5KHz（低速测试用）
  *         - 标准I2C模式
  *         - 快速模式占空比2:1
  *         - 禁用自身地址
  *         - 关闭应答机制
  *         - 7位地址识别模式
  */
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct)
{
  /* 重置所有结构体成员为安全默认值 */
  I2C_InitStruct->I2C_ClockSpeed = 5000;         // 5KHz低速时钟（需根据实际需求调整）
  I2C_InitStruct->I2C_Mode = I2C_Mode_I2C;       // 标准I2C模式（非SMBus模式）
  I2C_InitStruct->I2C_DutyCycle = I2C_DutyCycle_2; // 快速模式默认占空比2:1
  I2C_InitStruct->I2C_OwnAddress1 = 0;           // 禁用自身地址识别
  I2C_InitStruct->I2C_Ack = I2C_Ack_Disable;     // 关闭自动应答
  I2C_InitStruct->I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位地址模式
}

/**
  * @brief  I2C外设使能控制函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  NewState: 外设状态（ENABLE/DISABLE）
  * @retval 无
  * @note   操作CR1寄存器的PE位（Peripheral Enable）
  *         - 使能时会自动加载所有寄存器配置
  *         - 禁用时会停止所有通信并复位状态机
  */
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  /* 参数有效性验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));        // 检查外设实例合法性
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 检查状态参数合法性

  if (NewState != DISABLE) {
    /* 设置CR1寄存器的PE位（bit0）启用外设 */
    I2Cx->CR1 |= I2C_CR1_PE;  // 注意：外设启用前必须完成所有配置
  } else {
    /* 清除PE位立即禁用外设（中止当前通信） */
    I2Cx->CR1 &= (uint16_t)~I2C_CR1_PE; // 复位所有内部状态机
  }
}

/**
  * @brief  I2C模拟滤波器控制函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  NewState: 滤波器状态（ENABLE/DISABLE）
  * @retval 无
  * @note   仅适用于特定型号（F4系列部分型号）
  *         - 模拟滤波器用于抑制高频噪声
  *         - 默认启用，在强干扰环境中可禁用以提高速度
  *         - 必须在初始化前配置
  */
void I2C_AnalogFilterCmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  /* 参数有效性验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE) {
    /* 清除FLTR寄存器的ANOFF位（bit4）启用模拟滤波器 */
    I2Cx->FLTR &= ~I2C_FLTR_ANOFF;  // 滤波器延迟约260ns
  } else {
    /* 设置ANOFF位禁用模拟滤波器 */
    I2Cx->FLTR |= I2C_FLTR_ANOFF;   // 禁用后需增加数字滤波配置
  }
}

/**
  * @brief  I2C数字滤波器配置函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I3C3）
  * @param  I2C_DigitalFilter: 数字滤波系数（0x0-0xF）
  * @retval 无
  * @note   仅适用于特定型号（F4系列部分型号）
  *         - 数字滤波器通过采样抑制脉冲干扰
  *         - 系数N表示连续N+1个相同采样才被认为有效
  *         - 必须在初始化前配置，与模拟滤波器配合使用
  */
void I2C_DigitalFilterConfig(I2C_TypeDef* I2Cx, uint16_t I2C_DigitalFilter)
{
  uint16_t tmpreg = 0;
  
  /* 参数验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_DIGITAL_FILTER(I2C_DigitalFilter)); // 检查0x00-0x0F范围

  /* 保留FLTR寄存器其他位配置 */
  tmpreg = I2Cx->FLTR;
  
  /* 清除原DNF位（bits3-0）并设置新值 */
  tmpreg = (tmpreg & ~I2C_FLTR_DNF) |          // 使用掩码清除旧值
           (I2C_DigitalFilter & 0x0F);        // 确保只写入低4位

  /* 写入更新后的配置 */
  I2Cx->FLTR = tmpreg;  // 例如：设置0x3表示连续4个相同采样有效
}

/**
  * @brief  I2C起始条件生成控制函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  NewState: 起始条件生成状态（ENABLE/DISABLE）
  * @retval 无
  * @note   起始条件（SDA在SCL高电平时由高变低）作用：
  *         - 初始化数据传输
  *         - 用于主模式下的总线控制权获取
  *         - 必须配合总线检测使用（检查BUSY标志）
  */
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  /* 参数有效性验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 设置CR1寄存器的START位（bit8）生成起始脉冲 */
    I2Cx->CR1 |= I2C_CR1_START;  // 硬件自动生成完整起始时序
  } else {
    /* 紧急情况可取消未发出的起始条件 */
    I2Cx->CR1 &= ~I2C_CR1_START; // 仅在起始条件未发出前有效
  }
}

/**
  * @brief  I2C停止条件生成控制函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  NewState: 停止条件生成状态（ENABLE/DISABLE）
  * @retval 无
  * @note   停止条件（SDA在SCL高电平时由低变高）作用：
  *         - 终止数据传输并释放总线
  *         - 自动清除BUSY状态标志
  *         - 必须确保总线处于非繁忙状态
  */
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 设置CR1寄存器的STOP位（bit9）生成停止脉冲 */
    I2Cx->CR1 |= I2C_CR1_STOP;  // 触发后硬件自动完成停止序列
  } else {
    /* 清除未发出的停止条件（异常处理） */
    I2Cx->CR1 &= ~I2C_CR1_STOP; // 仅当STOP位尚未生效时有效
  }
}

/**
  * @brief  7位从机地址发送函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  Address: 7位从机地址（bit7-1有效，bit0将被覆盖）
  * @param  I2C_Direction: 通信方向（发送器/接收器）
  *         - I2C_Direction_Transmitter: 主机作为发送器
  *         - I2C_Direction_Receiver: 主机作为接收器
  * @retval 无
  * @note   地址字节格式：
  *         - bit7-1: 7位从机地址
  *         - bit0: 读写标志（0=写，1=读）
  *         总线上实际发送字节为 (Address << 1) | Direction
  */
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_DIRECTION(I2C_Direction));
  
  /* 根据通信方向设置地址字节LSB */
  if (I2C_Direction != I2C_Direction_Transmitter) {
    /* 接收模式：设置地址字节bit0为1 */
    Address |= 0x01;  // 从机地址 + 读标志
  } else {
    /* 发送模式：清除地址字节bit0 */
    Address &= 0xFE;  // 从机地址 + 写标志
  }
  
  /* 将组合后的地址写入数据寄存器 */
  I2Cx->DR = Address;  // 触发地址发送流程
}

/**
  * @brief  应答使能控制函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  NewState: 应答机制状态（ENABLE/DISABLE）
  * @retval 无
  * @note   应答机制工作模式：
  *         - 使能时：每个字节传输后自动发送ACK脉冲
  *         - 禁用时：用于接收最后一个字节后发送NACK
  *         必须在数据传输前配置
  */
void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 设置CR1寄存器的ACK位（bit10）启用应答 */
    I2Cx->CR1 |= I2C_CR1_ACK;  // 用于正常数据接收过程
  } else {
    /* 禁用应答（发送NACK信号） */
    I2Cx->CR1 &= ~I2C_CR1_ACK; // 用于接收最后一个字节的场景
  }
}

/**
  * @brief  第二自身地址配置函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  Address: 7位第二地址（bit7-1有效，bit0被忽略）
  * @retval 无
  * @note   双地址模式说明：
  *         - OAR1和OAR2可同时启用
  *         - 地址匹配时产生应答
  *         - 需配合双地址使能位（OAR1的bit1）使用
  */
void I2C_OwnAddress2Config(I2C_TypeDef* I2Cx, uint8_t Address)
{
  uint16_t tmpreg;
  
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
  /* 保留OAR2寄存器其他位配置（如双地址使能位） */
  tmpreg = I2Cx->OAR2;
  
  /* 清除原有地址配置（bits7-1）并设置新地址 */
  tmpreg = (tmpreg & ~I2C_OAR2_ADD2) |       // 使用掩码清除旧地址
           ((Address << 1) & 0x00FE);       // 地址左移对齐到bit7-1
  
  /* 写入更新后的OAR2寄存器值 */
  I2Cx->OAR2 = tmpreg;  // 注意：需同时启用ENDUAL位才能生效
}

/**
  * @brief  双地址模式控制函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  NewState: 双地址模式状态（ENABLE/DISABLE）
  * @retval 无
  * @note   双地址模式特性：
  *         - 允许同时响应两个独立从机地址（OAR1和OAR2）
  *         - 必须预先配置好第二地址（I2C_OwnAddress2Config）
  *         - 地址匹配任一都能触发应答
  */
void I2C_DualAddressCmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 设置OAR2寄存器的ENDUAL位（bit0）启用双地址模式 */
    I2Cx->OAR2 |= I2C_OAR2_ENDUAL;  // 要求OAR1和OAR2地址已正确配置
  } else {
    /* 禁用双地址模式，仅响应主地址 */
    I2Cx->OAR2 &= ~I2C_OAR2_ENDUAL; // 立即生效，无需总线复位
  }
}

/**
  * @brief  广播呼叫（群呼）功能控制
  * @param  I2Cx: 选择I2C外设实例
  * @param  NewState: 广播呼叫状态（ENABLE/DISABLE）
  * @retval 无
  * @note   广播呼叫特性：
  *         - 地址0x00被视为全局广播地址
  *         - 需配合广播地址识别中断使用
  *         - 用于同时控制多个从设备
  */
void I2C_GeneralCallCmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 设置CR1寄存器的ENGC位（bit6）启用广播识别 */
    I2Cx->CR1 |= I2C_CR1_ENGC;  // 从机模式下方可接收广播地址
  } else {
    /* 禁用广播呼叫接收功能 */
    I2Cx->CR1 &= ~I2C_CR1_ENGC; // 默认过滤广播地址
  }
}

/**
  * @brief  I2C软件复位控制函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  NewState: 软件复位状态（ENABLE/DISABLE）
  * @retval 无
  * @note   软件复位特性：
  *         - 重置I2C内部状态机
  *         - 复位期间SDA/SCL引脚变为高阻态
  *         - 用于总线错误恢复，复位后需重新初始化
  */
void I2C_SoftwareResetCmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 设置CR1寄存器的SWRST位（bit7）触发软件复位 */
    I2Cx->CR1 |= I2C_CR1_SWRST;  // 保持至少1个APB时钟周期
  } else {
    /* 结束复位状态，恢复正常工作 */
    I2Cx->CR1 &= ~I2C_CR1_SWRST; // 复位后需重新配置外设
  }
}

/**
  * @brief  时钟延展控制函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  NewState: 时钟延展状态（ENABLE=禁用延展，DISABLE=允许延展）
  * @retval 无
  * @note   时钟延展特性：
  *         - 禁用延展时：主机主动控制SCL时钟
  *         - 启用延展时：从机可拉低SCL实现等待
  *         - 主模式建议禁用，从模式建议启用
  */
void I2C_StretchClockCmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState == DISABLE) {
    /* 设置CR1的NOSTRETCH位（bit8）禁用时钟延展 */
    I2Cx->CR1 |= I2C_CR1_NOSTRETCH;  // 强制主机维持SCL控制权
  } else {
    /* 允许从机通过拉低SCL进行时钟延展 */
    I2Cx->CR1 &= ~I2C_CR1_NOSTRETCH; // 默认允许时钟延展
  }
}
/**
  * @brief  快速模式占空比配置函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  I2C_DutyCycle: 占空比模式
  *         - I2C_DutyCycle_2: Tlow/Thigh = 2:1 (总周期3T)
  *         - I2C_DutyCycle_16_9: Tlow/Thigh = 16/9 (总周期25T/9)
  * @retval 无
  * @note   该配置仅影响快速模式（>100KHz）时序：
  *         - 16/9模式可提供更好的电磁兼容性
  *         - 2:1模式可获得更高通信速率
  *         必须在初始化时通过I2C_Init()配置
  */
void I2C_FastModeDutyCycleConfig(I2C_TypeDef* I2Cx, uint16_t I2C_DutyCycle)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_DUTY_CYCLE(I2C_DutyCycle));
  
  /* 16/9模式需要设置CCR寄存器的DUTY位（bit14） */
  if (I2C_DutyCycle != I2C_DutyCycle_16_9) {
    /* 使用2:1占空比（清除DUTY位） */
    I2Cx->CCR &= I2C_DutyCycle_2;  // 实际操作为清除bit14
  } else {
    /* 设置16/9占空比（DUTY位置位） */
    I2Cx->CCR |= I2C_DutyCycle_16_9;  // 设置CCR[14]
  }
}

/**
  * @brief  NACK位置配置函数（主接收模式专用）
  * @param  I2Cx: 选择I2C外设实例
  * @param  I2C_NACKPosition: NACK发送位置
  *         - I2C_NACKPosition_Next: 下一个字节是最后一个（提前发送NACK）
  *         - I2C_NACKPosition_Current: 当前字节是最后一个（立即发送NACK）
  * @retval 无
  * @note   应用场景：
  *         - 接收2字节数据时，应在第一个字节前配置Next模式
  *         - 影响CR1寄存器的POS位（bit11）
  *         典型流程：接收倒数第二个数据字节时发送NACK
  */
void I2C_NACKPositionConfig(I2C_TypeDef* I2Cx, uint16_t I2C_NACKPosition)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_NACK_POSITION(I2C_NACKPosition));
  
  if (I2C_NACKPosition == I2C_NACKPosition_Next) {
    /* 下一字节是最后数据（POS位置位） */
    I2Cx->CR1 |= I2C_NACKPosition_Next;  // 设置CR1[11]
  } else {
    /* 当前字节是最后数据（POS位清零） */
    I2Cx->CR1 &= I2C_NACKPosition_Current;  // 掩码操作清除CR1[11]
  }
}

/**
  * @brief  SMBus警报引脚电平控制函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  I2C_SMBusAlert: 警报引脚电平
  *         - I2C_SMBusAlert_Low: 强制拉低警报引脚
  *         - I2C_SMBusAlert_High: 恢复高电平
  * @retval 无
  * @note   SMBus专用功能：
  *         - 警报引脚用于设备故障通知
  *         - 需配合SMBus协议使用
  *         - 对应CR1寄存器的ALERT位（bit13）
  */
void I2C_SMBusAlertConfig(I2C_TypeDef* I2Cx, uint16_t I2C_SMBusAlert)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_SMBUS_ALERT(I2C_SMBusAlert));
  
  if (I2C_SMBusAlert == I2C_SMBusAlert_Low) {
    /* 强制SMBUS_ALERT引脚输出低电平 */
    I2Cx->CR1 |= I2C_SMBusAlert_Low;  // 设置CR1[13]
  } else {
    /* 释放引脚控制（恢复高阻态或上拉） */
    I2Cx->CR1 &= I2C_SMBusAlert_High; // 清除CR1[13]
  }
}

/**
  * @brief  地址解析协议（ARP）使能控制
  * @param  I2Cx: 选择I2C外设实例
  * @param  NewState: ARP功能状态（ENABLE/DISABLE）
  * @retval 无
  * @note   SMBus专用功能特性：
  *         - 允许动态更新从机地址
  *         - 需配合SMBUS_HOST配置使用
  *         - 使能后自动响应ARP指令
  */
void I2C_ARPCmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 使能地址解析协议（设置ENARP位） */
    I2Cx->CR1 |= I2C_CR1_ENARP;  // 置位CR1[25]
  } else {
    /* 禁用ARP功能 */
    I2Cx->CR1 &= ~I2C_CR1_ENARP; // 清除CR1[25]
  }
}

/**
  * @}
  */

/** @defgroup I2C_Group2 Data transfers functions
 *  @brief   Data transfers functions 
 *
@verbatim   
 ===============================================================================
                  ##### Data transfers functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
/**
  * @brief  I2C单字节数据发送函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  Data: 待发送的字节数据（0x00-0xFF）
  * @retval 无
  * @note   数据传输特性：
  *         - 写入DR寄存器即启动发送流程
  *         - 需先确认TXE标志位为1（发送寄存器空）
  *         - 数据从高位（MSB）开始传输
  */
void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data)
{
  /* 参数合法性验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
  /* 将数据写入数据寄存器（触发发送流程） */
  I2Cx->DR = Data;  // DR寄存器具有双缓冲机制
}

/**
  * @brief  I2C接收数据读取函数
  * @param  I2Cx: 选择I2C外设实例
  * @retval 接收到的字节数据（0x00-0xFF）
  * @note   数据读取注意事项：
  *         - 必须在RXNE标志置位后调用
  *         - 读取DR寄存器会自动清除RXNE标志
  *         - 数据按接收顺序从低位（LSB）存储
  */
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx)
{
  /* 参数合法性验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
  /* 读取数据寄存器当前值 */
  return (uint8_t)I2Cx->DR;  // 读取操作清除RXNE标志
}

/**
  * @brief  PEC传输使能控制（SMBus模式专用）
  * @param  I2Cx: 选择I2C外设实例
  * @param  NewState: PEC传输状态（ENABLE/DISABLE）
  * @retval 无
  * @note   PEC（Packet Error Check）特性：
  *         - 基于CRC-8校验码的数据完整性校验
  *         - 使能后自动在数据后附加1字节PEC
  *         - 需先配置PEC计算（CRCPEC位）
  */
void I2C_TransmitPEC(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 设置CR1的PEC位（bit12）启用校验传输 */
    I2Cx->CR1 |= I2C_CR1_PEC;  // 发送模式下自动附加PEC值
  } else {
    /* 禁用PEC传输 */
    I2Cx->CR1 &= ~I2C_CR1_PEC; // 需要先停止传输才能修改
  }
}

/**
  * @brief  PEC字节位置配置函数（SMBus模式）
  * @param  I2Cx: 选择I2C外设实例
  * @param  I2C_PECPosition: PEC位置
  *         - I2C_PECPosition_Next: PEC跟随当前数据（立即发送）
  *         - I2C_PECPosition_Current: PEC作为独立数据（下一周期发送）
  * @retval 无
  * @note   配置特性：
  *         - 复用CR1寄存器的POS位（bit11）
  *         - 当前模式检测到PEC位置时会触发对应事件
  *         必须与I2C_TransmitPEC配合使用
  */
void I2C_PECPositionConfig(I2C_TypeDef* I2Cx, uint16_t I2C_PECPosition)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_PEC_POSITION(I2C_PECPosition));
  
  if (I2C_PECPosition == I2C_PECPosition_Next) {
    /* PEC作为独立数据包发送（设置POS位） */
    I2Cx->CR1 |= I2C_PECPosition_Next;  // 共用CR1[11]
  } else {
    /* PEC附加在当前数据包后（清除POS位） */
    I2Cx->CR1 &= I2C_PECPosition_Current; // 配置后立即生效
  }
}

/**
  * @brief  PEC校验计算使能控制（SMBus模式）
  * @param  I2Cx: 选择I2C外设实例
  * @param  NewState: PEC计算状态（ENABLE/DISABLE）
  * @retval 无
  * @note   PEC校验计算机制：
  *         - 基于所有传输字节的CRC-8计算结果
  *         - 使能后自动进行校验计算（结果存SR2）
  *         - 接收模式下自动验证对方PEC
  */
void I2C_CalculatePEC(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 使能PEC计算（设置CR1的ENPEC位bit12） */
    I2Cx->CR1 |= I2C_CR1_ENPEC;  // 启用后每个字节更新PEC
  } else {
    /* 禁用PEC计算并清除当前校验值 */
    I2Cx->CR1 &= ~I2C_CR1_ENPEC; // SR2中的PEC值被冻结
  }
}

/**
  * @brief  获取当前PEC校验值
  * @param  I2Cx: 选择I2C外设实例
  * @retval 当前计算的PEC校验字节（0x00-0xFF）
  * @note   校验值读取说明：
  *         - 存储在SR2寄存器高8位
  *         - 仅当ENPEC使能时有效
  *         - 读操作不会影响计算过程
  */
uint8_t I2C_GetPEC(I2C_TypeDef* I2Cx)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
  /* 从SR2[15:8]提取PEC值 */
  return (uint8_t)(I2Cx->SR2 >> 8);  // 返回PECVAL字段
}

/**
  * @brief  I2C DMA传输控制函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  NewState: DMA传输状态（ENABLE/DISABLE）
  * @retval 无
  * @note   DMA配置要求：
  *         - 预先配置DMA通道和控制寄存器
  *         - 发送模式使用TXE事件触发
  *         - 接收模式使用RXNE事件触发
  */
void I2C_DMACmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 使能CR2的DMAEN位（bit11）启动DMA请求 */
    I2Cx->CR2 |= I2C_CR2_DMAEN;  // 需同时配置DMA控制器
  } else {
    /* 禁用DMA传输并终止当前请求 */
    I2Cx->CR2 &= ~I2C_CR2_DMAEN; // 立即停止DMA传输
  }
}

/**
  * @brief  最后一次DMA传输标记配置
  * @param  I2Cx: 选择I2C外设实例
  * @param  NewState: 最后传输标记（ENABLE=下次是最后一次）
  * @retval 无
  * @note   典型应用场景：
  *         - 准备结束DMA传输时设置LAST位
  *         - 触发后DMA传输结束自动关闭请求
  *         - 接收模式需在倒数第二个字节设置
  */
void I2C_DMALastTransferCmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    /* 设置CR2的LAST位（bit12）标记下次传输为最后 */
    I2Cx->CR2 |= I2C_CR2_LAST;  // 用于正确生成STOP条件
  } else {
    /* 取消最后传输标记 */
    I2Cx->CR2 &= ~I2C_CR2_LAST; // 恢复正常DMA传输模式
  }
}


/**
  * @}
  */

/** @defgroup I2C_Group5 Interrupts events and flags management functions
 *  @brief   Interrupts, events and flags management functions
 *
@verbatim   
 ===============================================================================
          ##### Interrupts, events and flags management functions #####
 ===============================================================================
    [..]
    This section provides functions allowing to configure the I2C Interrupts 
    sources and check or clear the flags or pending bits status.
    The user should identify which mode will be used in his application to manage 
    the communication: Polling mode, Interrupt mode or DMA mode. 


                ##### I2C State Monitoring Functions #####                   
 =============================================================================== 
    [..]  
    This I2C driver provides three different ways for I2C state monitoring
    depending on the application requirements and constraints:
         
   
     (#) Basic state monitoring (Using I2C_CheckEvent() function)
     
        It compares the status registers (SR1 and SR2) content to a given event
        (can be the combination of one or more flags).
        It returns SUCCESS if the current status includes the given flags 
        and returns ERROR if one or more flags are missing in the current status.

          (++) When to use
             (+++) This function is suitable for most applications as well as for startup 
               activity since the events are fully described in the product reference 
               manual (RM0090).
             (+++) It is also suitable for users who need to define their own events.

          (++) Limitations
               If an error occurs (ie. error flags are set besides to the monitored 
               flags), the I2C_CheckEvent() function may return SUCCESS despite 
               the communication hold or corrupted real state. 
               In this case, it is advised to use error interrupts to monitor 
               the error events and handle them in the interrupt IRQ handler.
         
     -@@- For error management, it is advised to use the following functions:
        (+@@) I2C_ITConfig() to configure and enable the error interrupts (I2C_IT_ERR).
        (+@@) I2Cx_ER_IRQHandler() which is called when the error interrupt occurs.
              Where x is the peripheral instance (I2C1, I2C2 ...)
        (+@@) I2C_GetFlagStatus() or I2C_GetITStatus()  to be called into the 
              I2Cx_ER_IRQHandler() function in order to determine which error occurred.
        (+@@) I2C_ClearFlag() or I2C_ClearITPendingBit() and/or I2C_SoftwareResetCmd() 
              and/or I2C_GenerateStop() in order to clear the error flag and source 
              and return to correct  communication status.
             
 
     (#) Advanced state monitoring (Using the function I2C_GetLastEvent())

        Using the function I2C_GetLastEvent() which returns the image of both status 
        registers in a single word (uint32_t) (Status Register 2 value is shifted left 
        by 16 bits and concatenated to Status Register 1).

          (++) When to use
             (+++) This function is suitable for the same applications above but it 
               allows to overcome the mentioned limitation of I2C_GetFlagStatus() 
               function.
             (+++) The returned value could be compared to events already defined in 
               the library (stm32f4xx_i2c.h) or to custom values defined by user.
               This function is suitable when multiple flags are monitored at the 
               same time.
             (+++) At the opposite of I2C_CheckEvent() function, this function allows 
               user to choose when an event is accepted (when all events flags are 
               set and no other flags are set or just when the needed flags are set 
               like I2C_CheckEvent() function.

          (++) Limitations
             (+++) User may need to define his own events.
             (+++) Same remark concerning the error management is applicable for this 
               function if user decides to check only regular communication flags 
               (and ignores error flags).
      
 
     (#) Flag-based state monitoring (Using the function I2C_GetFlagStatus())
     
      Using the function I2C_GetFlagStatus() which simply returns the status of 
      one single flag (ie. I2C_FLAG_RXNE ...). 

          (++) When to use
             (+++) This function could be used for specific applications or in debug 
               phase.
             (+++) It is suitable when only one flag checking is needed (most I2C 
               events are monitored through multiple flags).
          (++) Limitations: 
             (+++) When calling this function, the Status register is accessed. 
               Some flags are cleared when the status register is accessed. 
               So checking the status of one Flag, may clear other ones.
             (+++) Function may need to be called twice or more in order to monitor 
               one single event.
 
   For detailed description of Events, please refer to section I2C_Events in 
   stm32f4xx_i2c.h file.
       
@endverbatim
  * @{
  */
   /**
  * @brief  I2C寄存器读取函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  I2C_Register: 目标寄存器选择（CR1/CR2/OAR1/OAR2/DR/SR1/SR2/CCR/TRISE）
  * @retval 目标寄存器的当前值（16位）
  * @note   寄存器地址计算原理：
  *         - 利用结构体内存布局特性，通过基地址+偏移量访问
  *         - 适用于所有I2C控制/状态/配置寄存器
  *         典型应用场景：调试时直接访问寄存器状态
  */
uint16_t I2C_ReadRegister(I2C_TypeDef* I2Cx, uint8_t I2C_Register)
{
  __IO uint32_t tmp = 0;

  /* 参数有效性验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_REGISTER(I2C_Register)); // 检查寄存器枚举值有效性

  /* 计算寄存器物理地址：外设基地址 + 寄存器偏移量 */
  tmp = (uint32_t) I2Cx;        // 获取外设基地址（如I2C1=0x40005400）
  tmp += I2C_Register;          // 加上寄存器偏移量（如CR1=0x00）

  /* 通过指针转换访问目标寄存器 */
  return (*(__IO uint16_t *) tmp); // 使用volatile访问确保实时读取
}

/**
  * @brief  I2C中断使能控制函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  I2C_IT: 中断类型组合（位或操作）：
  *         - I2C_IT_BUF: 缓冲区中断（TXE/RXNE事件）
  *         - I2C_IT_EVT: 事件中断（SB/ADDR/ADD10/STOPF等）
  *         - I2C_IT_ERR: 错误中断（BERR/ARLO/AF/OVR等）
  * @param  NewState: 中断状态（ENABLE/DISABLE）
  * @retval 无
  * @note   中断配置层级：
  *         - CR2寄存器控制全局中断使能
  *         - 需同时配置NVIC控制器才能接收中断
  *         典型应用：结合中断服务程序实现事件驱动通信
  */
void I2C_ITConfig(I2C_TypeDef* I2Cx, uint16_t I2C_IT, FunctionalState NewState)
{
  /* 参数验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_I2C_CONFIG_IT(I2C_IT)); // 检查中断掩码合法性

  if (NewState != DISABLE) {
    /* 设置CR2寄存器对应中断使能位 */
    I2Cx->CR2 |= I2C_IT;  // 位或操作开启指定中断
  } else {
    /* 清除CR2寄存器对应中断使能位 */
    I2Cx->CR2 &= (uint16_t)~I2C_IT; // 位与操作关闭中断
  }
}


/*
 ===============================================================================
                          1. Basic state monitoring                    
 ===============================================================================  
 */

/**
  * @brief  Checks whether the last I2Cx Event is equal to the one passed
  *         as parameter.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  I2C_EVENT: specifies the event to be checked. 
  *          This parameter can be one of the following values:
  *            @arg I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED: EV1
  *            @arg I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED: EV1
  *            @arg I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED: EV1
  *            @arg I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED: EV1
  *            @arg I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED: EV1
  *            @arg I2C_EVENT_SLAVE_BYTE_RECEIVED: EV2
  *            @arg (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_DUALF): EV2
  *            @arg (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_GENCALL): EV2
  *            @arg I2C_EVENT_SLAVE_BYTE_TRANSMITTED: EV3
  *            @arg (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_DUALF): EV3
  *            @arg (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_GENCALL): EV3
  *            @arg I2C_EVENT_SLAVE_ACK_FAILURE: EV3_2
  *            @arg I2C_EVENT_SLAVE_STOP_DETECTED: EV4
  *            @arg I2C_EVENT_MASTER_MODE_SELECT: EV5
  *            @arg I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: EV6     
  *            @arg I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: EV6
  *            @arg I2C_EVENT_MASTER_BYTE_RECEIVED: EV7
  *            @arg I2C_EVENT_MASTER_BYTE_TRANSMITTING: EV8
  *            @arg I2C_EVENT_MASTER_BYTE_TRANSMITTED: EV8_2
  *            @arg I2C_EVENT_MASTER_MODE_ADDRESS10: EV9
  *     
  * @note   For detailed description of Events, please refer to section I2C_Events
  *         in stm32f4xx_i2c.h file.
  *    
  * @retval An ErrorStatus enumeration value:
  *           - SUCCESS: Last event is equal to the I2C_EVENT
  *           - ERROR: Last event is different from the I2C_EVENT
  */
/**
  * @brief  I2C事件状态检查函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  I2C_EVENT: 预期检查的事件组合（多个事件用位或操作组合）
  * @retval ErrorStatus: 事件状态检查结果
  *         - SUCCESS: 检测到所有指定事件
  *         - ERROR: 未检测到完整事件组合
  * @note   事件检测机制：
  *         - 同时读取SR1和SR2寄存器并合并为32位状态
  *         - 使用FLAG_MASK过滤有效标志位
  *         - 要求所有指定事件位同时有效
  */
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
  uint32_t lastevent = 0;
  uint32_t flag1 = 0, flag2 = 0;
  ErrorStatus status = ERROR;

  /* 参数有效性验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_EVENT(I2C_EVENT)); // 检查事件组合合法性

  /* 读取状态寄存器（原子操作避免状态变化） */
  flag1 = I2Cx->SR1; // 获取SR1寄存器值（事件标志低16位）
  flag2 = I2Cx->SR2; // 获取SR2寄存器值（事件标志高16位）
  flag2 = flag2 << 16; // 将SR2值左移16位对齐到高地址

  /* 合并事件标志并过滤无效位 */
  lastevent = (flag1 | flag2) & FLAG_MASK; // FLAG_MASK=0x00FFFFFF

  /* 检查是否完全匹配预期事件组合 */
  if ((lastevent & I2C_EVENT) == I2C_EVENT) {
    status = SUCCESS; // 所有指定事件位同时有效
  } else {
    status = ERROR;   // 部分或全部事件位未触发
  }
  return status;
}

/**
  * @brief  获取I2C完整事件状态
  * @param  I2Cx: 选择I2C外设实例
  * @retval 32位事件状态字（bit23-0对应SR1/SR2各标志）
  * @note   事件状态字组成：
  *         - bits[15:0]: SR1寄存器标志（低16位）
  *         - bits[23:16]: SR2寄存器标志（高8位）
  *         典型应用：配合I2C_EVENT_xxx宏进行位操作检查
  */
uint32_t I2C_GetLastEvent(I2C_TypeDef* I2Cx)
{
  uint32_t lastevent = 0;
  uint32_t flag1 = 0, flag2 = 0;

  assert_param(IS_I2C_ALL_PERIPH(I2Cx));

  /* 原子读取状态寄存器避免状态变化 */
  flag1 = I2Cx->SR1;       // 示例值：0x00030001（包含SB/ADDR/TXE等标志）
  flag2 = I2Cx->SR2;       // 示例值：0x00000007（包含TRA/BUSY/DUAL等标志）
  flag2 = flag2 << 16;      // 对齐到bits[23:16]

  /* 合并并过滤有效事件标志 */
  lastevent = (flag1 | flag2) & FLAG_MASK; // 结果示例：0x00070001

  return lastevent; // 返回格式：0x00(SR2)FF FF(SR1)FF
}

/*
 ===============================================================================
                          3. Flag-based state monitoring                   
 ===============================================================================  
 */

/**
  * @brief  Checks whether the specified I2C flag is set or not.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  I2C_FLAG: specifies the flag to check. 
  *          This parameter can be one of the following values:
  *            @arg I2C_FLAG_DUALF: Dual flag (Slave mode)
  *            @arg I2C_FLAG_SMBHOST: SMBus host header (Slave mode)
  *            @arg I2C_FLAG_SMBDEFAULT: SMBus default header (Slave mode)
  *            @arg I2C_FLAG_GENCALL: General call header flag (Slave mode)
  *            @arg I2C_FLAG_TRA: Transmitter/Receiver flag
  *            @arg I2C_FLAG_BUSY: Bus busy flag
  *            @arg I2C_FLAG_MSL: Master/Slave flag
  *            @arg I2C_FLAG_SMBALERT: SMBus Alert flag
  *            @arg I2C_FLAG_TIMEOUT: Timeout or Tlow error flag
  *            @arg I2C_FLAG_PECERR: PEC error in reception flag
  *            @arg I2C_FLAG_OVR: Overrun/Underrun flag (Slave mode)
  *            @arg I2C_FLAG_AF: Acknowledge failure flag
  *            @arg I2C_FLAG_ARLO: Arbitration lost flag (Master mode)
  *            @arg I2C_FLAG_BERR: Bus error flag
  *            @arg I2C_FLAG_TXE: Data register empty flag (Transmitter)
  *            @arg I2C_FLAG_RXNE: Data register not empty (Receiver) flag
  *            @arg I2C_FLAG_STOPF: Stop detection flag (Slave mode)
  *            @arg I2C_FLAG_ADD10: 10-bit header sent flag (Master mode)
  *            @arg I2C_FLAG_BTF: Byte transfer finished flag
  *            @arg I2C_FLAG_ADDR: Address sent flag (Master mode) "ADSL"
  *                                Address matched flag (Slave mode)"ENDAD"
  *            @arg I2C_FLAG_SB: Start bit flag (Master mode)
  * @retval The new state of I2C_FLAG (SET or RESET).
  */
/**
  * @brief  I2C标志状态获取函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  I2C_FLAG: 要检查的状态标志（I2C_FLAG_xxx格式）
  * @retval FlagStatus: 标志状态（SET/RESET）
  * @note   标志检测机制：
  *         - 通过标志值高4位判断寄存器类型（SR1/SR2）
  *         - 动态计算寄存器物理地址
  *         - 支持SR1和SR2寄存器所有有效标志位
  */
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG)
{
  FlagStatus bitstatus = RESET;
  __IO uint32_t i2creg = 0, i2cxbase = 0;

  /* 参数有效性验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_GET_FLAG(I2C_FLAG)); // 检查标志合法性

  /* 获取外设基地址（如I2C1=0x40005400） */
  i2cxbase = (uint32_t)I2Cx;
  
  /* 从标志值高4位提取寄存器索引（0=SR2，非0=SR1） */
  i2creg = I2C_FLAG >> 28;  
  I2C_FLAG &= FLAG_MASK;     // 保留标志位低24位

  if(i2creg != 0) {
    /* 处理SR1寄存器标志（基地址+0x14） */
    i2cxbase += 0x14;        // SR1寄存器偏移地址
  } else {
    /* 处理SR2寄存器标志（右移16位对齐） */
    I2C_FLAG >>= 16;         // 将SR2标志移到低8位
    i2cxbase += 0x18;        // SR2寄存器偏移地址
  }

  /* 通过指针转换访问目标寄存器并检查标志位 */
  if((*(__IO uint32_t *)i2cxbase & I2C_FLAG) != RESET) {
    bitstatus = SET;  // 标志置位
  } else {
    bitstatus = RESET;// 标志未置位
  }
  return bitstatus;
}

/**
  * @brief  I2C标志清除函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  I2C_FLAG: 要清除的标志（仅限可写标志）
  * @retval 无
  * @note   特殊标志清除方式：
  *         - 部分标志需要特定操作序列（如读SR1后写CR1）
  *         - 直接写SR1只能清除部分标志（BERR/ARLO/OVR等）
  *         详细清除规则参考STM32参考手册寄存器描述
  */
void I2C_ClearFlag(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG)
{
  uint32_t flagpos = 0;
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_CLEAR_FLAG(I2C_FLAG)); // 检查可清除标志
  
  /* 获取标志位在SR1中的位置 */
  flagpos = I2C_FLAG & FLAG_MASK;
  
  /* 通过写SR1寄存器清除标志（实际为读-修改-写操作） */
  I2Cx->SR1 = (uint16_t)~flagpos;  // 写1清除对应标志位
}

/**
  * @brief  I2C中断状态获取函数
  * @param  I2Cx: 选择I2C外设实例
  * @param  I2C_IT: 要检查的中断类型（I2C_IT_xxx格式）
  * @retval ITStatus: 中断状态（SET=中断挂起，RESET=无中断）
  * @note   中断状态判断逻辑：
  *         - 同时满足标志置位和中断使能
  *         - 需在中断服务程序中调用
  */
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, uint32_t I2C_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t enablestatus = 0;

  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_GET_IT(I2C_IT)); // 验证中断类型合法性

  /* 检查中断是否使能（从CR2获取使能状态） */
  enablestatus = (I2C_IT & ITEN_MASK) >> 16; // 提取中断使能位
  enablestatus &= I2Cx->CR2;                 // 检查CR2对应使能位
  
  /* 获取实际标志位并检查状态 */
  I2C_IT &= FLAG_MASK; // 保留有效标志位
  
  /* 双重验证：标志置位且中断使能 */
  if (((I2Cx->SR1 & I2C_IT) != RESET) && enablestatus) {
    bitstatus = SET;   // 有效中断请求
  } else {
    bitstatus = RESET; // 未触发或未使能
  }
  return bitstatus;
}
/**
  * @brief  I2C中断挂起标志清除函数
  * @param  I2Cx: 选择I2C外设实例（I2C1/I2C2/I2C3）
  * @param  I2C_IT: 要清除的中断标志（I2C_IT_xxx格式）
  * @retval 无
  * @note   重要清除规则：
  *         - 仅适用于通过写SR1直接清除的标志（BERR/ARLO/OVR等）
  *         - 特殊标志需按手册规定顺序操作（如STOPF需先读SR1再写CR1）
  *         使用前请参考STM32参考手册对应标志的清除要求
  */
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, uint32_t I2C_IT)
{
  uint32_t flagpos = 0;
  
  /* 参数合法性验证 */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_CLEAR_IT(I2C_IT)); // 检查是否允许直接清除的中断类型

  /* 获取有效标志位（过滤高8位） */
  flagpos = I2C_IT & FLAG_MASK;        // FLAG_MASK=0x00FFFFFF

  /* 通过写SR1寄存器清除中断标志（实际为读-修改-写操作） */
  I2Cx->SR1 = (uint16_t)~flagpos;      // 写1清除对应标志位
  
  /* 注意：对于ADDR/STOPF/BTF等特殊标志，此操作可能无法正确清除，
     必须按照手册规定的操作序列处理 */
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

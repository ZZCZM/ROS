/**
  ******************************************************************************
  * @file    stm32f4xx_dcmi.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the DCMI peripheral:           
  *           + Initialization and Configuration
  *           + Image capture functions  
  *           + Interrupts and flags management
  *
 @verbatim          
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================  
    [..]       
      The sequence below describes how to use this driver to capture image
      from a camera module connected to the DCMI Interface.
      This sequence does not take into account the configuration of the  
      camera module, which should be made before to configure and enable
      the DCMI to capture images.
             
      (#) Enable the clock for the DCMI and associated GPIOs using the following 
          functions:
          RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);
  
      (#) DCMI pins configuration 
        (++) Connect the involved DCMI pins to AF13 using the following function 
            GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_DCMI); 
        (++) Configure these DCMI pins in alternate function mode by calling 
            the function GPIO_Init();
      
      (#) Declare a DCMI_InitTypeDef structure, for example:
          DCMI_InitTypeDef  DCMI_InitStructure;
          and fill the DCMI_InitStructure variable with the allowed values
          of the structure member.
    
      (#) Initialize the DCMI interface by calling the function
          DCMI_Init(&DCMI_InitStructure); 
    
      (#) Configure the DMA2_Stream1 channel1 to transfer Data from DCMI DR
          register to the destination memory buffer.
    
      (#) Enable DCMI interface using the function
          DCMI_Cmd(ENABLE);
                   
      (#) Start the image capture using the function
          DCMI_CaptureCmd(ENABLE);
                   
      (#) At this stage the DCMI interface waits for the first start of frame,
          then a DMA request is generated continuously/once (depending on the
          mode used, Continuous/Snapshot) to transfer the received data into
          the destination memory. 
     
      -@-  If you need to capture only a rectangular window from the received
           image, you have to use the DCMI_CROPConfig() function to configure 
           the coordinates and size of the window to be captured, then enable 
           the Crop feature using DCMI_CROPCmd(ENABLE);  
           In this case, the Crop configuration should be made before to enable
           and start the DCMI interface. 

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
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @defgroup DCMI 
  * @brief DCMI driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup DCMI_Private_Functions
  * @{
  */ 

/** @defgroup DCMI_Group1 Initialization and Configuration functions
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
  * @brief  复位DCMI外设寄存器到默认值
  * @details 该函数清除所有配置，关闭接口并清除中断标志
  * @note 调用后需重新初始化DCMI才能正常使用
  * @param  无
  * @retval 无
  */
void DCMI_DeInit(void)
{
  /* 关闭DCMI核心功能 */
  DCMI->CR = 0x0;        // 控制寄存器复位
  /* 禁用所有中断源 */
  DCMI->IER = 0x0;       // 中断使能寄存器清零
  /* 清除所有挂起的中断标志 */
  DCMI->ICR = 0x1F;      // 中断标志清除寄存器，写入1清标志（0x1F=0b11111）   
  /* 复位嵌入式同步和裁剪配置 */  
  DCMI->ESCR = 0x0;      // 嵌入式同步代码寄存器复位
  DCMI->ESUR = 0x0;      // 嵌入式同步取消掩码寄存器复位
  /* 清除窗口捕获配置 */
  DCMI->CWSTRTR = 0x0;   // 捕获窗口起始地址寄存器清零
  DCMI->CWSIZER = 0x0;   // 捕获窗口尺寸寄存器清零
}

/**
  * @brief  DCMI外设初始化函数
  * @details 根据配置结构体参数初始化数字摄像头接口
  * @param  DCMI_InitStruct: 初始化参数结构体指针
  *         结构体成员包括：
  *           - DCMI_CaptureMode: 连续/快照模式选择
  *           - DCMI_SynchroMode: 硬件/嵌入式同步选择
  *           - DCMI_PCKPolarity: 像素时钟采样边沿
  *           - DCMI_VSPolarity:  垂直同步极性
  *           - DCMI_HSPolarity:  水平同步极性  
  *           - DCMI_CaptureRate: 全帧/隔行捕获速率
  *           - DCMI_ExtendedDataMode: 数据位宽配置（8/10/12/14位）
  * @note 初始化前必须停止DCMI工作（CR寄存器的ENABLE和CAPTURE位清零）
  * @example 
  *         配置OV2640摄像头：
  *         DCMI_InitStruct.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
  *         DCMI_InitStruct.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  *         DCMI_InitStruct.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
  * @retval 无
  */
void DCMI_Init(DCMI_InitTypeDef* DCMI_InitStruct)
{
  uint32_t temp = 0x0;
  
  /* 参数合法性验证 */
  assert_param(IS_DCMI_CAPTURE_MODE(DCMI_InitStruct->DCMI_CaptureMode)); // 0x04000000 / 0x00000000
  assert_param(IS_DCMI_SYNCHRO(DCMI_InitStruct->DCMI_SynchroMode));      // 0x00000000 / 0x00002000
  assert_param(IS_DCMI_PCKPOLARITY(DCMI_InitStruct->DCMI_PCKPolarity));  // 0x00000000 / 0x00000100
  assert_param(IS_DCMI_VSPOLARITY(DCMI_InitStruct->DCMI_VSPolarity));     // 0x00000000 / 0x00000200
  assert_param(IS_DCMI_HSPOLARITY(DCMI_InitStruct->DCMI_HSPolarity));     // 0x00000000 / 0x00000400
  assert_param(IS_DCMI_CAPTURE_RATE(DCMI_InitStruct->DCMI_CaptureRate)); // 0x00000000 / 0x00010000
  assert_param(IS_DCMI_EXTENDED_DATA(DCMI_InitStruct->DCMI_ExtendedDataMode)); // 0x00~0x03<<8

  /* 禁用DCMI核心功能及捕获 */
  // 清除CR寄存器的使能位(bit14)和捕获位(bit0)
  DCMI->CR &= ~(DCMI_CR_ENABLE | DCMI_CR_CAPTURE);  
   
  /* 保存当前控制寄存器值 */
  temp = DCMI->CR;  // 备份原寄存器值
  
  /* 生成清除掩码（0xFFF98FFF）*/
  temp &= ~((uint32_t)DCMI_CR_CM     |  // 清捕获模式位（bit30）
                     DCMI_CR_ESS    |  // 清同步模式位（bit13）
                     DCMI_CR_PCKPOL |  // 清像素时钟极性位（bit8）
                     DCMI_CR_HSPOL  |  // 清HS极性位（bit10） 
                     DCMI_CR_VSPOL  |  // 清VS极性位（bit9） 
                     DCMI_CR_FCRC_0 |  // 清捕获速率bit0（bit16）
                     DCMI_CR_FCRC_1 |  // 清捕获速率bit1（bit17）
                     DCMI_CR_EDM_0  |  // 清数据模式bit0（bit4）
                     DCMI_CR_EDM_1);   // 清数据模式bit1（bit5）
                  
  /* 组合新配置 */             
  temp |= ((uint32_t)DCMI_InitStruct->DCMI_CaptureMode |        // 设置捕获模式
                     DCMI_InitStruct->DCMI_SynchroMode |        // 设置同步模式
                     DCMI_InitStruct->DCMI_PCKPolarity |        // 设置像素时钟极性
                     DCMI_InitStruct->DCMI_VSPolarity  |        // 设置垂直同步极性
                     DCMI_InitStruct->DCMI_HSPolarity  |        // 设置水平同步极性 
                     DCMI_InitStruct->DCMI_CaptureRate  |       // 设置捕获速率
                     DCMI_InitStruct->DCMI_ExtendedDataMode);  // 设置数据位宽

  /* 更新控制寄存器 */
  DCMI->CR = temp;   // 将新配置写入CR寄存器                             
}

/**
  * @brief  初始化DCMI配置结构体为默认值
  * @details 提供标准配置模板，用户可在其基础上修改
  * @param  DCMI_InitStruct: DCMI配置结构体指针
  * @note 默认配置适用于多数摄像头模块的连续采集模式
  *        - 连续捕获模式
  *        - 硬件同步（HSYNC/VSYNC）
  *        - 像素时钟下降沿采样
  *        - 8位数据宽度  
  * @retval 无
  */
void DCMI_StructInit(DCMI_InitTypeDef* DCMI_InitStruct)
{
  /* 设置默认参数 */
  DCMI_InitStruct->DCMI_CaptureMode = DCMI_CaptureMode_Continuous;  // 连续采集（非单帧）
  DCMI_InitStruct->DCMI_SynchroMode = DCMI_SynchroMode_Hardware;    // 硬件同步（非嵌入式）
  DCMI_InitStruct->DCMI_PCKPolarity = DCMI_PCKPolarity_Falling;     // 像素时钟下降沿捕获数据
  DCMI_InitStruct->DCMI_VSPolarity = DCMI_VSPolarity_Low;           // VS低电平有效
  DCMI_InitStruct->DCMI_HSPolarity = DCMI_HSPolarity_Low;           // HS低电平有效
  DCMI_InitStruct->DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;  // 捕获所有帧
  DCMI_InitStruct->DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b; // 8位数据模式
}
/**
  * @brief  DCMI图像裁剪窗口配置
  * @details 设置摄像头采集的感兴趣区域(ROI)，通过指定起始坐标和窗口尺寸实现局部采集
  * @param  DCMI_CROPInitStruct: 裁剪配置结构体，包含四个参数：
  *           - DCMI_HorizontalOffsetCount: 水平偏移（X起始坐标，0-32767）
  *           - DCMI_VerticalStartLine: 垂直起始行（Y起始坐标，0-8191）  
  *           - DCMI_CaptureCount: 水平捕获像素数（窗口宽度，0-32767）
  *           - DCMI_VerticalLineCount: 垂直捕获行数（窗口高度，0-8191）
  * @note 必须先配置窗口参数再启用CROP功能，配置后通过DCMI_CROPCmd(ENABLE)激活
  * @example 截取QVGA区域：水平320，垂直240，起始点(100,200)
  *          DCIM_CROPInitStruct.DCMI_HorizontalOffsetCount = 100;
  *          DCIM_CROPInitStruct.DCMI_VerticalStartLine = 200;
  *          DCIM_CROPInitStruct.DCMI_CaptureCount = 320-1;  // 寄存器值为实际像素数-1
  *          DCIM_CROPInitStruct.DCMI_VerticalLineCount = 240-1;
  */
void DCMI_CROPConfig(DCMI_CROPInitTypeDef* DCMI_CROPInitStruct)
{  
  /* 组合窗口起始坐标到CWSTRTR寄存器 */
  // [15:0] 水平起始像素：[31:16]垂直起始行号
  DCMI->CWSTRTR = (uint32_t)((uint32_t)DCMI_CROPInitStruct->DCMI_HorizontalOffsetCount |
                  ((uint32_t)DCMI_CROPInitStruct->DCMI_VerticalStartLine << 16));

  /* 组合窗口尺寸到CWSIZER寄存器 */
  // [15:0] 水平像素数：[31:16]垂直行数
  DCMI->CWSIZER = (uint32_t)(DCMI_CROPInitStruct->DCMI_CaptureCount |
                  ((uint32_t)DCMI_CROPInitStruct->DCMI_VerticalLineCount << 16));
}

/**
  * @brief  启用/禁用图像裁剪功能
  * @param  NewState: 裁剪功能状态（ENABLE-启用区域截取，DISABLE-全帧采集）
  * @warning 启用前必须通过DCMI_CROPConfig配置有效的窗口参数
  * @note 启用后会修改CR寄存器的CROP位(bit19)，该操作应在DCMI使能前完成
  */
void DCMI_CROPCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState)); // 校验状态参数合法性
    
  if (NewState != DISABLE)
  {
    /* 设置控制寄存器的CROP位 */
    // 使能后DCMI仅捕获CWSTRTR/CWSIZER定义的区域
    DCMI->CR |= (uint32_t)DCMI_CR_CROP;  // DCMI_CR_CROP=0x00080000
  }
  else
  {
    /* 清除CROP位恢复全帧采集 */
    DCMI->CR &= ~(uint32_t)DCMI_CR_CROP; 
  }
}

/**
  * @brief  配置嵌入式同步码
  * @details 用于无硬件同步信号的图像传感器，通过数据流中的特殊码元进行帧/行同步
  * @param  DCMI_CodesInitStruct: 同步码结构体，包含四个参数：
  *           - DCMI_FrameStartCode: 帧起始码（1字节，推荐0x00）
  *           - DCMI_LineStartCode: 行起始码（1字节，推荐0xAA）
  *           - DCMI_LineEndCode:   行结束码（1字节，推荐0x55）
  *           - DCMI_FrameEndCode: 帧结束码（1字节，推荐0xBB）
  * @note 需同时配置CR寄存器的ESS位(嵌入式同步模式)
  * @example 设置MJPEG流同步码：
  *          DCMI_CodesInitStruct.DCMI_FrameStartCode = 0xFF; // JPEG SOI标记
  */
void DCMI_SetEmbeddedSynchroCodes(DCMI_CodesInitTypeDef* DCMI_CodesInitStruct)
{
  /* 组合四个同步码到ESCR寄存器 */
  // 寄存器结构：[7:0]帧起始 [15:8]行起始 [23:16]行结束 [31:24]帧结束
  DCMI->ESCR = (uint32_t)(DCMI_CodesInitStruct->DCMI_FrameStartCode |
                          ((uint32_t)DCMI_CodesInitStruct->DCMI_LineStartCode << 8)|
                          ((uint32_t)DCMI_CodesInitStruct->DCMI_LineEndCode << 16)|
                          ((uint32_t)DCMI_CodesInitStruct->DCMI_FrameEndCode << 24));
}

/**
  * @brief  启用JPEG图像格式模式
  * @details 用于接收JPEG/MJPEG数据流，格式需符合JPEG帧结构（SOI/EOI标记）
  * @param  NewState: JPEG模式状态（ENABLE-启用JPEG解析，DISABLE-原始数据模式）
  * @warning JPEG模式下必须禁用裁剪(CROP)和嵌入式同步功能
  * @note 启用后DCMI会自动检测0xFFD8FF...帧结构，配合DMA实现流传输
  */
void DCMI_JPEGCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));
 
  if (NewState != DISABLE)
  {
    /* 设置控制寄存器JPEG位(bit11) */
    // 启用JPEG编码模式，数据流按JPEG帧结构解析
    DCMI->CR |= (uint32_t)DCMI_CR_JPEG;  // DCMI_CR_JPEG=0x00000800
  }
  else
  {
    /* 恢复原始数据采集模式 */
    DCMI->CR &= ~(uint32_t)DCMI_CR_JPEG;
  }
}

/**
  * @}
  */

/** @defgroup DCMI_Group2 Image capture functions
 *  @brief   Image capture functions
 *
@verbatim   
 ===============================================================================
                    ##### Image capture functions #####
 ===============================================================================  

@endverbatim
  * @{
  */
  /**
  * @brief  启用或禁用DCMI外设接口
  * @details 该函数控制DCMI核心时钟和基础功能，禁用时所有相关操作停止
  * @param  NewState: 接口状态
  *         取值：
  *           - ENABLE: 启动DCMI时钟，允许配置寄存器
  *           - DISABLE: 关闭DCMI时钟，停止所有操作
  * @note 启用后需额外调用DCMI_CaptureCmd启动实际数据捕获
  * @warning 禁用接口会终止进行中的数据传输，建议先停止捕获再禁用
  */
void DCMI_Cmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 设置CR寄存器的ENABLE位(bit14) */
    // 启动DCMI内核时钟，允许接收同步信号
    DCMI->CR |= (uint32_t)DCMI_CR_ENABLE;  // DCMI_CR_ENABLE=0x00004000
  }
  else
  {
    /* 清除ENABLE位，关闭DCMI内核 */
    DCMI->CR &= ~(uint32_t)DCMI_CR_ENABLE; 
  }
}

/**
  * @brief  启动/停止图像数据捕获
  * @details 控制实际数据采集过程，需在DCMI_Cmd(ENABLE)之后调用
  * @param  NewState: 捕获状态
  *         取值：
  *           - ENABLE: 开始接收图像数据流
  *           - DISABLE: 停止数据捕获
  * @note 首次启动会等待VSYNC信号才开始采集，停止会立即终止当前帧
  * @example 典型工作流程：
  *         DCMI_Cmd(ENABLE);    // 使能接口
  *         DCMI_CaptureCmd(ENABLE); // 开始采集
  */
void DCMI_CaptureCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 校验参数合法性
    
  if (NewState != DISABLE)
  {
    /* 设置CR寄存器的CAPTURE位(bit0) */
    // 使能后根据配置开始捕获数据，需要已配置同步模式和数据格式
    DCMI->CR |= (uint32_t)DCMI_CR_CAPTURE;  // DCMI_CR_CAPTURE=0x00000001
  }
  else
  {
    /* 清除CAPTURE位，停止数据流接收 */
    DCMI->CR &= ~(uint32_t)DCMI_CR_CAPTURE;
  }
}

/**
  * @brief  读取DCMI数据寄存器值
  * @details 获取当前接收到的32位图像数据，通常用于轮询模式
  * @param  无
  * @retval 32位图像数据（实际有效位根据数据模式决定）
  * @note 在DMA模式下通常不需要手动读取，数据直接传输到内存
  * @warning 在高速模式下需及时读取，避免数据溢出覆盖
  */
uint32_t DCMI_ReadData(void)
{
  /* 返回数据寄存器当前值 */
  // DR寄存器会在每个像素时钟周期更新（根据数据位宽模式）
  return DCMI->DR;  // 数据格式：RAW模式下为像素值，JPEG模式下为流数据
}

/**
  * @}
  */

/** @defgroup DCMI_Group3 Interrupts and flags management functions
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
  * @brief  Enables or disables the DCMI interface interrupts.
  * @param  DCMI_IT: specifies the DCMI interrupt sources to be enabled or disabled. 
  *          This parameter can be any combination of the following values:
  *            @arg DCMI_IT_FRAME: Frame capture complete interrupt mask
  *            @arg DCMI_IT_OVF: Overflow interrupt mask
  *            @arg DCMI_IT_ERR: Synchronization error interrupt mask
  *            @arg DCMI_IT_VSYNC: VSYNC interrupt mask
  *            @arg DCMI_IT_LINE: Line interrupt mask
  * @param  NewState: new state of the specified DCMI interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
/**
  * @brief  配置DCMI中断使能状态
  * @details 该函数用于启用/禁用指定类型的中断请求，支持多种DCMI事件中断
  * @param  DCMI_IT: 中断类型选择，可组合以下值（按位或）：
  *           - DCMI_IT_FRAME: 帧捕获完成中断
  *           - DCMI_IT_OVF:    数据溢出中断  
  *           - DCMI_IT_ERR:    同步错误中断
  *           - DCMI_IT_VSYNC: 垂直同步中断
  *           - DCMI_IT_LINE:  行捕获中断
  * @param  NewState: 中断状态
  *           - ENABLE: 允许中断触发
  *           - DISABLE: 禁止中断
  * @note 使用前需配置NVIC中断控制器，建议在DCMI初始化完成后调用
  */
void DCMI_ITConfig(uint16_t DCMI_IT, FunctionalState NewState)
{
  /* 参数合法性验证 */
  assert_param(IS_DCMI_CONFIG_IT(DCMI_IT));     // 校验中断类型掩码合法性（0x13F）
  assert_param(IS_FUNCTIONAL_STATE(NewState));  // 验证状态参数有效性
  
  if (NewState != DISABLE)
  {
    /* 设置中断使能寄存器(IER)对应位 */
    // 通过位或操作使能指定中断源，保留其他中断状态
    DCMI->IER |= DCMI_IT;  // IER寄存器地址：0x50050034
  }
  else
  {
    /* 清除中断使能寄存器对应位 */
    // 通过位与操作和掩码取反禁用中断
    DCMI->IER &= (uint16_t)(~DCMI_IT); 
  }  
}

/**
  * @brief  获取DCMI状态标志位当前状态
  * @details 该函数检测原始/屏蔽后的硬件事件标志，用于中断处理和状态查询
  * @param  DCMI_FLAG: 目标标志位，可选值：
  *           - 原始中断标志（RISR寄存器）：
  *             DCMI_FLAG_FRAMERI, DCMI_FLAG_OVFRI, DCMI_FLAG_ERRRI, 
  *             DCMI_FLAG_VSYNCRI, DCMI_FLAG_LINERI
  *           - 屏蔽中断标志（MISR寄存器）：
  *             DCMI_FLAG_FRAMEMI, DCMI_FLAG_OVFMI, DCMI_FLAG_ERRMI,
  *             DCMI_FLAG_VSYNCMI, DCMI_FLAG_LINEMI
  *           - 状态标志（SR寄存器）：
  *             DCMI_FLAG_HSYNC, DCMI_FLAG_VSYNC, DCMI_FLAG_FNE
  * @retval 标志位状态：SET（置位）/ RESET（复位）
  */
FlagStatus DCMI_GetFlagStatus(uint16_t DCMI_FLAG)
{
  FlagStatus bitstatus = RESET;      // 默认状态为未触发
  uint32_t dcmireg, tempreg = 0;

  /* 验证标志位参数合法性 */
  assert_param(IS_DCMI_GET_FLAG(DCMI_FLAG));  // 校验标志位掩码有效性（0xF1FF）
  
  /* 解析标志位寄存器类型 */
  // 利用标志位高4位判断所属寄存器（RISR/SR/MISR）
  dcmireg = (((uint16_t)DCMI_FLAG) >> 12);  // 右移12位获取寄存器标识
  
  if (dcmireg == 0x00) /* 标志位在原始中断状态寄存器(RISR) */
  {
    tempreg= DCMI->RISR;  // RISR地址：0x5005003C
  }
  else if (dcmireg == 0x02) /* 标志位在状态寄存器(SR) */
  {
    tempreg = DCMI->SR;   // SR地址：0x50050038
  }
  else /* 标志位在屏蔽中断状态寄存器(MISR) */
  {
    tempreg = DCMI->MISR; // MISR地址：0x50050040
  }
  
  /* 检测目标标志位状态 */
  if ((tempreg & DCMI_FLAG) != (uint16_t)RESET )
  {
    bitstatus = SET;  // 标志位置位
  }
  else
  {
    bitstatus = RESET; // 标志位未置位
  }
  return  bitstatus;  // 返回检测结果
}
/**
  * @brief  清除DCMI模块的指定硬件标志位
  * @param  DCMI_FLAG: 需要清除的标志位组合，使用位掩码格式
  *         可选的标志位包括：
  *            DCMI_FLAG_FRAMERI - 帧捕获完成原始标志
  *            DCMI_FLAG_OVFRI   - 数据溢出原始标志
  *            DCMI_FLAG_ERRRI   - 同步错误原始标志
  *            DCMI_FLAG_VSYNCRI - 垂直同步原始标志
  *            DCMI_FLAG_LINERI  - 行捕获原始标志
  * @retval 无
  * @note 该操作直接影响中断状态，清除标志位可能影响后续中断触发
  */
void DCMI_ClearFlag(uint16_t DCMI_FLAG)
{
  /* 参数有效性检查，验证输入标志位是否合法 */
  assert_param(IS_DCMI_CLEAR_FLAG(DCMI_FLAG));
  
  /* 通过写中断清除寄存器(ICR)来清除指定标志位：
     写入1到对应位位置会清除该标志 */
  DCMI->ICR = DCMI_FLAG;  // 直接赋值操作，同时清除多个标志位
}

/**
  * @brief  获取指定DCMI中断的状态
  * @param  DCMI_IT: 需要查询的中断类型，使用位掩码格式
  *         可选的中断类型包括：
  *            DCMI_IT_FRAME - 帧捕获完成中断
  *            DCMI_IT_OVF   - 数据溢出中断
  *            DCMI_IT_ERR   - 同步错误中断
  *            DCMI_IT_VSYNC - 垂直同步中断
  *            DCMI_IT_LINE  - 行捕获中断
  * @retval ITStatus 返回中断状态：
  *         SET - 中断已触发
  *         RESET - 中断未触发
  * @note 该函数检查的是经过屏蔽后的中断状态（MISR寄存器）
  */
ITStatus DCMI_GetITStatus(uint16_t DCMI_IT)
{
  ITStatus bitstatus = RESET;  // 初始化返回状态为未触发
  uint32_t itstatus = 0;       // 用于存储寄存器读取结果
  
  /* 验证输入参数的有效性 */
  assert_param(IS_DCMI_GET_IT(DCMI_IT));
  
  /* 读取屏蔽中断状态寄存器(MISR)并与输入掩码进行按位与操作：
     这样只检查被屏蔽（使能）的中断状态 */
  itstatus = DCMI->MISR & DCMI_IT; 

  /* 判断结果是否为非零值（即是否有中断触发） */
  if ((itstatus != (uint16_t)RESET))  // RESET通常定义为0
  {
    bitstatus = SET;     // 有中断触发，返回SET
  }
  else
  {
    bitstatus = RESET;   // 无中断触发，返回RESET
  }
  return bitstatus;      // 返回最终检测结果
}

/**
  * @brief  清除DCMI模块的中断挂起标志位
  * @param  DCMI_IT: 需要清除的中断类型组合，使用位掩码格式
  *         可选的中断类型包括：
  *            DCMI_IT_FRAME - 帧捕获完成中断
  *            DCMI_IT_OVF   - 数据溢出中断
  *            DCMI_IT_ERR   - 同步错误中断
  *            DCMI_IT_VSYNC - 垂直同步中断
  *            DCMI_IT_LINE  - 行捕获中断
  * @retval 无
  * @note 清除中断挂起位是中断处理的重要步骤，防止重复进入中断
  */
void DCMI_ClearITPendingBit(uint16_t DCMI_IT)
{
  /* 通过写中断清除寄存器(ICR)来清除指定中断挂起位：
     写入1到对应位位置会清除该中断状态 */
  DCMI->ICR = DCMI_IT;  // 直接赋值操作，可同时清除多个中断状态
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

/**
  ******************************************************************************
  * @file    stm32f4xx_dma.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the DMA firmware 
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
#ifndef __STM32F4xx_DMA_H
#define __STM32F4xx_DMA_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup DMA
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/**
  * @brief DMA初始化结构体定义
  * @details 包含DMA通道配置所需的全套参数，用于存储器到外设/存储器间的数据传输控制
  */
typedef struct
{
  /**
  * @brief DMA通道选择
  * @note 可选DMA_Channel_0~7，具体可用的通道需参考芯片手册
  * @warning 通道与流(Stream)的对应关系需参考STM32F4参考手册
  */
  uint32_t DMA_Channel;            

  /**
  * @brief 外设基地址
  * @example 如设置为&ADC1->DR，表示从ADC数据寄存器读取数据
  */
  uint32_t DMA_PeripheralBaseAddr; 

  /**
  * @brief 内存基地址0
  * @note 双缓冲模式禁用时默认使用此地址
  * @example 设置为数组首地址时需进行类型转换：(uint32_t)bufferArray
  */
  uint32_t DMA_Memory0BaseAddr;    

  /**
  * @brief 数据传输方向
  * @value DMA_DIR_PeripheralToMemory 外设到内存
  * @value DMA_DIR_MemoryToPeripheral 内存到外设 
  * @value DMA_DIR_MemoryToMemory     内存到内存(F4系列特有)
  */
  uint32_t DMA_DIR;                

  /**
  * @brief 数据缓冲区大小（以数据单元为单位）
  * @example 当数据宽度为32位时，设置100表示传输100个32位数据
  */
  uint32_t DMA_BufferSize;         

  /**
  * @brief 外设地址自增模式
  * @value DMA_PeripheralInc_Enable 传输后地址自动增加
  * @value DMA_PeripheralInc_Disable 固定外设地址
  */
  uint32_t DMA_PeripheralInc;      

  /**
  * @brief 内存地址自增模式
  * @note 通常内存地址需要自增以实现连续存储
  */
  uint32_t DMA_MemoryInc;          

  /**
  * @brief 外设数据宽度
  * @value DMA_PeripheralDataSize_Byte  8位
  * @value DMA_PeripheralDataSize_HalfWord 16位
  * @value DMA_PeripheralDataSize_Word  32位
  */
  uint32_t DMA_PeripheralDataSize; 

  /**
  * @brief 内存数据宽度
  * @note 需要与外设数据宽度匹配，必要时会自动打包/解包
  */
  uint32_t DMA_MemoryDataSize;     

  /**
  * @brief 工作模式选择
  * @value DMA_Mode_Normal   单次传输模式
  * @value DMA_Mode_Circular 循环缓冲模式
  * @warning 内存到内存传输时禁用循环模式
  */
  uint32_t DMA_Mode;               

  /**
  * @brief DMA通道优先级
  * @value DMA_Priority_Low        低优先级
  * @value DMA_Priority_Medium     中等优先级
  * @value DMA_Priority_High       高优先级
  * @value DMA_Priority_VeryHigh   最高优先级
  */
  uint32_t DMA_Priority;           

  /**
  * @brief FIFO模式选择
  * @value DMA_FIFOMode_Enable  启用FIFO(支持突发传输)
  * @value DMA_FIFOMode_Disable 直通模式
  * @note 内存到内存传输必须使用FIFO模式
  */
  uint32_t DMA_FIFOMode;          

  /**
  * @brief FIFO阈值设定
  * @value DMA_FIFOThreshold_1QuarterFull FIFO达到1/4触发传输
  * @value DMA_FIFOThreshold_HalfFull     FIFO半满触发传输(常用)
  * @value DMA_FIFOThreshold_3QuartersFull FIFO3/4触发传输 
  * @value DMA_FIFOThreshold_Full         FIFO全满触发传输
  */
  uint32_t DMA_FIFOThreshold;      

  /**
  * @brief 内存突发传输配置
  * @value DMA_MemoryBurst_Single  单次传输
  * @value DMA_MemoryBurst_INC4    4节拍的增量突发
  * @value DMA_MemoryBurst_INC8    8节拍的增量突发
  * @value DMA_MemoryBurst_INC16   16节拍的增量突发
  * @note 需开启地址自增模式
  */
  uint32_t DMA_MemoryBurst;        

  /**
  * @brief 外设突发传输配置
  * @note 配置方式同内存突发传输，需要外设支持突发传输
  */
  uint32_t DMA_PeripheralBurst;    
}DMA_InitTypeDef;

/* 外设有效性校验宏 -------------------------------------------------*/
/**
  * @brief 校验DMA流是否有效
  * @note 覆盖STM32F4系列DMA1/DMA2的所有8个流(Stream0-Stream7)
  */
#define IS_DMA_ALL_PERIPH(PERIPH) (((PERIPH) == DMA1_Stream0) || \
                                   ... /* 中间流省略 */ || \
                                   ((PERIPH) == DMA2_Stream7))

/**
  * @brief 校验DMA控制器是否有效
  * @value DMA1 直接存储器访问控制器1
  * @value DMA2 直接存储器访问控制器2
  */
#define IS_DMA_ALL_CONTROLLER(CONTROLLER) (((CONTROLLER) == DMA1) || \
                                           ((CONTROLLER) == DMA2))

/** @defgroup DMA_channel DMA通道配置组
  * @brief 选择DMA请求映射的硬件通道
  * @details 每个通道对应特定的外设请求源，具体映射关系参考芯片手册
  */
#define DMA_Channel_0   ((uint32_t)0x00000000) /*!< 通道0：如SPI3_RX/TIM4_CH1等 */
#define DMA_Channel_1   ((uint32_t)0x02000000) /*!< 通道1：如USART3_TX/I2C2_RX等 */
#define DMA_Channel_2   ((uint32_t)0x04000000) /*!< 通道2：如TIM2_UP/ADC2等 */
#define DMA_Channel_3   ((uint32_t)0x06000000) /*!< 通道3：如I2S3_EXT_TX/DCMI等 */
#define DMA_Channel_4   ((uint32_t)0x08000000) /*!< 通道4：如SPI2_RX/USART1_TX等 */
#define DMA_Channel_5   ((uint32_t)0x0A000000) /*!< 通道5：如TIM7_TRIG/SAI1_A等 */
#define DMA_Channel_6   ((uint32_t)0x0C000000) /*!< 通道6：如TIM1_CH1/LPUART1_RX等 */
#define DMA_Channel_7   ((uint32_t)0x0E000000) /*!< 通道7：如SPDIFRX_IN0/QUADSPI等 */

/* 通道有效性校验（确保通道号在0-7范围内） */
#define IS_DMA_CHANNEL(CHANNEL) (((CHANNEL) == DMA_Channel_0) || \
                                 ... /* 中间通道省略 */ || \
                                 ((CHANNEL) == DMA_Channel_7))

/** @defgroup DMA_data_transfer_direction 传输方向配置组
  * @brief 设定DMA数据传输路径
  */
#define DMA_DIR_PeripheralToMemory  ((uint32_t)0x00000000) /*!< 外设->内存：ADC采集等输入场景 */
#define DMA_DIR_MemoryToPeripheral  ((uint32_t)0x00000040) /*!< 内存->外设：DAC输出等场景 */ 
#define DMA_DIR_MemoryToMemory      ((uint32_t)0x00000080) /*!< 内存->内存：高速数据搬移（需FIFO）*/

/* 方向参数校验（防止非法模式设置） */
#define IS_DMA_DIRECTION(DIRECTION) (((DIRECTION) == DMA_DIR_PeripheralToMemory ) || \
                                     ... /* 参数检查 */ )

/** @defgroup DMA_data_buffer_size 缓冲区尺寸校验组
  * @brief 校验DMA传输数据量合法性
  * @note 实际传输量 = BufferSize × 数据宽度
  */
#define IS_DMA_BUFFER_SIZE(SIZE) (((SIZE) >= 0x1) && ((SIZE) < 0x10000)) /*!< 有效范围：1-65535单元 */

/** @defgroup DMA_peripheral_incremented_mode 外设地址自增模式组
  * @brief 控制外设端地址指针行为
  */
#define DMA_PeripheralInc_Enable  ((uint32_t)0x00000200) /*!< 启用：多寄存器连续访问（如ADC多通道）*/
#define DMA_PeripheralInc_Disable ((uint32_t)0x00000000) /*!< 禁用：固定地址（如串口DR寄存器）*/

/* 自增模式参数校验 */
#define IS_DMA_PERIPHERAL_INC_STATE(STATE) (((STATE) == DMA_PeripheralInc_Enable) || \
                                            ((STATE) == DMA_PeripheralInc_Disable))

/**
  * @}
  */ 

/**
  * @brief DMA数据传输配置参数组
  * @details 该组定义包含DMA传输过程中涉及的核心配置参数，包括内存操作模式、
  *          数据传输尺寸、工作模式及优先级设置，用于精确控制DMA传输行为
  */

/** @defgroup DMA_memory_incremented_mode 内存地址自增模式
  * @{ 
  */ 
#define DMA_MemoryInc_Enable              ((uint32_t)0x00000400) /*!< 内存地址自增使能：用于连续存储场景（如数组填充），
                                                                       每次传输后地址指针自动增加数据尺寸的倍数 */
#define DMA_MemoryInc_Disable             ((uint32_t)0x00000000) /*!< 内存地址自增禁用：用于固定地址操作场景（如循环更新单个变量），
                                                                       地址指针始终保持不变 */
/* 内存地址自增状态校验宏 */
#define IS_DMA_MEMORY_INC_STATE(STATE) (((STATE) == DMA_MemoryInc_Enable) || \
                                        ((STATE) == DMA_MemoryInc_Disable)) /*!< 参数有效性检查，确保传入值必须为0或0x400 */
/**
  * @}
  */ 


/** @defgroup DMA_peripheral_data_size 外设数据尺寸
  * @{
  */ 
#define DMA_PeripheralDataSize_Byte       ((uint32_t)0x00000000)  /*!< 8位传输：适用于UART_DR、GPIO等字节操作外设，
                                                                       每次传输处理1字节数据 */
#define DMA_PeripheralDataSize_HalfWord   ((uint32_t)0x00000800)  /*!< 16位传输：适用于ADC、TIMx_CCR等16位寄存器，
                                                                       每次传输处理2字节数据 */ 
#define DMA_PeripheralDataSize_Word       ((uint32_t)0x00001000)  /*!< 32位传输：适用于FSMC、HASH等32位总线外设，
                                                                       每次传输处理4字节数据 */
/* 外设数据尺寸校验宏 */
#define IS_DMA_PERIPHERAL_DATA_SIZE(SIZE) (((SIZE) == DMA_PeripheralDataSize_Byte)  || \
                                           ((SIZE) == DMA_PeripheralDataSize_HalfWord) || \
                                           ((SIZE) == DMA_PeripheralDataSize_Word)) /*!< 限制参数只能为0/0x800/0x1000三种值 */
/**
  * @}
  */ 


/** @defgroup DMA_memory_data_size 内存数据尺寸
  * @{
  */ 
#define DMA_MemoryDataSize_Byte           ((uint32_t)0x00000000)  /*!< 8位存储：对应uint8_t类型数组，
                                                                       每次传输写入1字节到内存 */ 
#define DMA_MemoryDataSize_HalfWord       ((uint32_t)0x00002000)  /*!< 16位存储：对应uint16_t类型数组，
                                                                       每次传输写入2字节到内存 */ 
#define DMA_MemoryDataSize_Word           ((uint32_t)0x00004000)  /*!< 32位存储：对应uint32_t/float类型变量，
                                                                       每次传输写入4字节到内存 */
/* 内存数据尺寸校验宏 */                                        
#define IS_DMA_MEMORY_DATA_SIZE(SIZE) (((SIZE) == DMA_MemoryDataSize_Byte)  || \
                                       ((SIZE) == DMA_MemoryDataSize_HalfWord) || \
                                       ((SIZE) == DMA_MemoryDataSize_Word )) /*!< 参数范围限制检查，确保尺寸参数合法性 */
/**
  * @}
  */ 


/** @defgroup DMA_circular_normal_mode 传输模式选择
  * @{
  */ 
#define DMA_Mode_Normal                   ((uint32_t)0x00000000)  /*!< 普通模式：完成指定数据量传输后自动停止，
                                                                       需要手动重启传输过程 */ 
#define DMA_Mode_Circular                 ((uint32_t)0x00000100)  /*!< 循环模式：传输计数器自动重载实现连续传输，
                                                                       适用于实时数据流采集（如音频信号） */
/* 传输模式校验宏 */                                        
#define IS_DMA_MODE(MODE) (((MODE) == DMA_Mode_Normal ) || \
                           ((MODE) == DMA_Mode_Circular))  /*!< 模式参数有效性检查，特别禁止内存到内存传输使用循环模式 */
/**
  * @}
  */ 


/** @defgroup DMA_priority_level 通道优先级配置
  * @{
  */ 
#define DMA_Priority_Low                  ((uint32_t)0x00000000) /*!< 低优先级：仅在无其他DMA请求时执行，
                                                                      适用于后台数据处理 */
#define DMA_Priority_Medium               ((uint32_t)0x00010000)  /*!< 中等优先级：标准操作级别，
                                                                      适用于常规外设传输 */ 
#define DMA_Priority_High                 ((uint32_t)0x00020000) /*!< 高优先级：可抢占低优先级通道，
                                                                      适用于时间敏感型操作（如摄像头数据采集） */
#define DMA_Priority_VeryHigh             ((uint32_t)0x00030000) /*!< 最高优先级：立即抢占所有低级请求，
                                                                      用于紧急中断服务 */

/* 优先级参数校验宏 */                                        
#define IS_DMA_PRIORITY(PRIORITY) (((PRIORITY) == DMA_Priority_Low )   || \
                                   ((PRIORITY) == DMA_Priority_Medium) || \
                                   ((PRIORITY) == DMA_Priority_High)   || \
                                   ((PRIORITY) == DMA_Priority_VeryHigh)) /*!< 优先级参数范围校验，
                                                                      0x0000/0x10000/0x20000/0x30000四值有效 */
/**
  * @}
  */

/**
  * @}
  */ 
/**
  * @brief DMA FIFO及突发传输控制参数组
  * @details 该配置组定义DMA传输过程中的FIFO缓冲机制、突发传输模式及状态监测参数，
  *          用于优化大数据量传输的带宽利用率，管理外设与内存之间的数据流节奏，
  *          有效防止总线拥堵并提升系统整体性能
  */

/** @defgroup DMA_fifo_direct_mode FIFO工作模式配置
  * @{
  */ 
#define DMA_FIFOMode_Disable              ((uint32_t)0x00000000) /*!< 禁用FIFO模式：直通传输模式，数据不经过缓冲直接传输，
                                                                       适用于外设与内存速度匹配的简单场景，
                                                                       内存到内存传输时强制要求启用FIFO */
#define DMA_FIFOMode_Enable               ((uint32_t)0x00000004) /*!< 启用FIFO模式：激活4×32位深度缓冲队列，
                                                                       支持数据打包和突发传输模式，
                                                                       可有效提升总线利用率达300% */

/* FIFO模式参数校验宏 */
#define IS_DMA_FIFO_MODE_STATE(STATE) (((STATE) == DMA_FIFOMode_Disable ) || \
                                       ((STATE) == DMA_FIFOMode_Enable))  /*!< 严格检查模式参数合法性，
                                                                               防止配置冲突（特别是内存到内存传输场景） */
/**
  * @}
  */ 

/** @defgroup DMA_fifo_threshold_level FIFO触发阈值配置
  * @{
  */ 
#define DMA_FIFOThreshold_1QuarterFull    ((uint32_t)0x00000000) /*!< 1/4满阈值：当FIFO存储≥8字节时触发传输，
                                                                       适用于高频小数据包传输（如UART 115200bps）*/
#define DMA_FIFOThreshold_HalfFull        ((uint32_t)0x00000001) /*!< 半满阈值：当FIFO存储≥16字节时触发，
                                                                       平衡模式（默认推荐配置） */ 
#define DMA_FIFOThreshold_3QuartersFull   ((uint32_t)0x00000002) /*!< 3/4满阈值：当FIFO存储≥24字节时触发，
                                                                       用于减少中断次数的块传输 */ 
#define DMA_FIFOThreshold_Full            ((uint32_t)0x00000003) /*!< 全满阈值：当FIFO完全满32字节时触发，
                                                                       最大化单次传输量（吞吐量优先模式） */

/* FIFO阈值校验宏 */
#define IS_DMA_FIFO_THRESHOLD(THRESHOLD) (((THRESHOLD) == DMA_FIFOThreshold_1QuarterFull ) || \
                                          ((THRESHOLD) == DMA_FIFOThreshold_HalfFull)      || \
                                          ((THRESHOLD) == DMA_FIFOThreshold_3QuartersFull) || \
                                          ((THRESHOLD) == DMA_FIFOThreshold_Full)) /*!< 阈值参数严格校验，
                                                                                       必须为0x00至0x03之间的有效值 */
/**
  * @}
  */ 

/** @defgroup DMA_memory_burst 内存突发传输配置
  * @{
  */ 
#define DMA_MemoryBurst_Single            ((uint32_t)0x00000000) /*!< 单次传输模式：每个总线事务传输1个数据单元，
                                                                       理论带宽=总线频率×数据宽度 */
#define DMA_MemoryBurst_INC4              ((uint32_t)0x00800000) /*!< 4节拍突发：单事务传输4个连续单元，
                                                                       有效带宽提升至理论值的75% */  
#define DMA_MemoryBurst_INC8              ((uint32_t)0x01000000) /*!< 8节拍突发：适合缓存行填充操作，
                                                                       带宽利用率达90% */ 
#define DMA_MemoryBurst_INC16             ((uint32_t)0x01800000) /*!< 16节拍突发：最大突发长度，
                                                                       带宽利用率接近100%（需128位总线支持） */

/* 内存突发模式校验宏 */
#define IS_DMA_MEMORY_BURST(BURST) (((BURST) == DMA_MemoryBurst_Single) || \
                                    ((BURST) == DMA_MemoryBurst_INC4)  || \
                                    ((BURST) == DMA_MemoryBurst_INC8)  || \
                                    ((BURST) == DMA_MemoryBurst_INC16)) /*!< 突发长度校验，
                                                                              0x00000000/0x00800000/0x01000000/0x01800000有效 */
/**
  * @}
  */ 

/** @defgroup DMA_peripheral_burst 外设突发传输配置
  * @{
  */ 
#define DMA_PeripheralBurst_Single        ((uint32_t)0x00000000) /*!< 外设单次传输：适合低速外设单寄存器访问（如DAC） */
#define DMA_PeripheralBurst_INC4          ((uint32_t)0x00200000) /*!< 4节拍突发：适合摄像头传感器行缓存传输，
                                                                       OV7670等CMOS传感器常用 */  
#define DMA_PeripheralBurst_INC8          ((uint32_t)0x00400000) /*!< 8节拍突发：适用于以太网MAC批量数据包接收，
                                                                       提升TCP/IP传输效率 */ 
#define DMA_PeripheralBurst_INC16         ((uint32_t)0x00600000) /*!< 16节拍突发：需外设端具有≥16单元缓冲，
                                                                       如高分辨率ADC阵列 */

/* 外设突发模式校验宏 */
#define IS_DMA_PERIPHERAL_BURST(BURST) (((BURST) == DMA_PeripheralBurst_Single) || \
                                        ((BURST) == DMA_PeripheralBurst_INC4)  || \
                                        ((BURST) == DMA_PeripheralBurst_INC8)  || \
                                        ((BURST) == DMA_PeripheralBurst_INC16)) /*!< 外设突发参数合法性检查，
                                                                                     需与外设硬件特性匹配 */
/**
  * @}
  */ 

/** @defgroup DMA_fifo_status_level FIFO实时状态监测
  * @{
  */
#define DMA_FIFOStatus_Less1QuarterFull   ((uint32_t)0x00000000 << 3) /*!< FIFO存储量<8字节 */
#define DMA_FIFOStatus_1QuarterFull       ((uint32_t)0x00000001 << 3) /*!< 8字节≤存储量<16字节 */ 
#define DMA_FIFOStatus_HalfFull           ((uint32_t)0x00000002 << 3) /*!< 16字节≤存储量<24字节 */
#define DMA_FIFOStatus_3QuartersFull      ((uint32_t)0x00000003 << 3) /*!< 24字节≤存储量<32字节 */
#define DMA_FIFOStatus_Empty              ((uint32_t)0x00000004 << 3) /*!< FIFO完全空（数据传输完成状态） */
#define DMA_FIFOStatus_Full               ((uint32_t)0x00000005 << 3) /*!< FIFO完全满（32字节） */

/* FIFO状态标志校验宏 */
#define IS_DMA_FIFO_STATUS(STATUS) (((STATUS) == DMA_FIFOStatus_Less1QuarterFull ) || \
                                    ((STATUS) == DMA_FIFOStatus_1QuarterFull)      || \
                                    ((STATUS) == DMA_FIFOStatus_HalfFull)          || \
                                    ((STATUS) == DMA_FIFOStatus_3QuartersFull)     || \
                                    ((STATUS) == DMA_FIFOStatus_Empty)            || \
                                    ((STATUS) == DMA_FIFOStatus_Full)) /*!< 状态标志有效性验证，
                                                                            常用于DMA中断服务程序的状态判断 */
/**
  * @}
  */

/**
  * @}
  */ 
/**
  * @brief DMA状态标志位定义
  * @details 该组定义包含DMA2控制器所有流(Stream0-Stream7)的状态监测标志，
  *          涵盖传输进度、错误类型及FIFO状态，用于实时监控DMA传输状态
  *          标志位组成规则：[31:29]DMA控制器ID [28:25]流编号 [24:22]保留 [21:0]具体标志位
  */

/** @defgroup DMA_flags_definition DMA状态标志组
  * @{
  */
/*---------------------- Stream 0 标志 ----------------------*/
#define DMA_FLAG_FEIF0    ((uint32_t)0x10800001) /*!< Stream0 FIFO错误：检测到FIFO溢出/下溢 (DMA_LISR[0]) */
#define DMA_FLAG_DMEIF0   ((uint32_t)0x10800004) /*!< Stream0直接模式错误：地址未对齐时尝试直接传输 (DMA_LISR[2]) */
#define DMA_FLAG_TEIF0    ((uint32_t)0x10000008) /*!< Stream0传输错误：数据校验失败/外设未响应 (DMA_LISR[3]) */
#define DMA_FLAG_HTIF0    ((uint32_t)0x10000010) /*!< Stream0半传输完成：传输完成50%数据量 (DMA_LISR[4]) */
#define DMA_FLAG_TCIF0    ((uint32_t)0x10000020) /*!< Stream0传输完成：缓冲区计数器归零 (DMA_LISR[5]) */

/*---------------------- Stream 1 标志 ----------------------*/
#define DMA_FLAG_FEIF1    ((uint32_t)0x10000040) /*!< Stream1 FIFO错误 (DMA_LISR[6]) */
#define DMA_FLAG_DMEIF1   ((uint32_t)0x10000100) /*!< Stream1直接模式错误 (DMA_LISR[8]) */
#define DMA_FLAG_TEIF1    ((uint32_t)0x10000200) /*!< Stream1传输错误 (DMA_LISR[9]) */
#define DMA_FLAG_HTIF1    ((uint32_t)0x10000400) /*!< Stream1半传输完成 (DMA_LISR[10]) */
#define DMA_FLAG_TCIF1    ((uint32_t)0x10000800) /*!< Stream1传输完成 (DMA_LISR[11]) */

/*---------------------- Stream 2 标志 ----------------------*/ 
#define DMA_FLAG_FEIF2    ((uint32_t)0x10010000) /*!< Stream2 FIFO错误 (DMA_LISR[16]) */
#define DMA_FLAG_DMEIF2   ((uint32_t)0x10040000) /*!< Stream2直接模式错误 (DMA_LISR[18]) */
#define DMA_FLAG_TEIF2    ((uint32_t)0x10080000) /*!< Stream2传输错误 (DMA_LISR[19]) */
#define DMA_FLAG_HTIF2    ((uint32_t)0x10100000) /*!< Stream2半传输完成 (DMA_LISR[20]) */
#define DMA_FLAG_TCIF2    ((uint32_t)0x10200000) /*!< Stream2传输完成 (DMA_LISR[21]) */

/*---------------------- Stream 3 标志 ----------------------*/
#define DMA_FLAG_FEIF3    ((uint32_t)0x10400000) /*!< Stream3 FIFO错误 (DMA_LISR[22]) */
#define DMA_FLAG_DMEIF3   ((uint32_t)0x11000000) /*!< Stream3直接模式错误 (DMA_LISR[24]) */
#define DMA_FLAG_TEIF3    ((uint32_t)0x12000000) /*!< Stream3传输错误 (DMA_LISR[25]) */
#define DMA_FLAG_HTIF3    ((uint32_t)0x14000000) /*!< Stream3半传输完成 (DMA_LISR[26]) */
#define DMA_FLAG_TCIF3    ((uint32_t)0x18000000) /*!< Stream3传输完成 (DMA_LISR[27]) */

/*---------------------- Stream 4 标志 ----------------------*/
#define DMA_FLAG_FEIF4    ((uint32_t)0x20000001) /*!< Stream4 FIFO错误 (DMA_HISR[0]) */
#define DMA_FLAG_DMEIF4   ((uint32_t)0x20000004) /*!< Stream4直接模式错误 (DMA_HISR[2]) */
#define DMA_FLAG_TEIF4    ((uint32_t)0x20000008) /*!< Stream4传输错误 (DMA_HISR[3]) */
#define DMA_FLAG_HTIF4    ((uint32_t)0x20000010) /*!< Stream4半传输完成 (DMA_HISR[4]) */
#define DMA_FLAG_TCIF4    ((uint32_t)0x20000020) /*!< Stream4传输完成 (DMA_HISR[5]) */

/*---------------------- Stream 5 标志 ----------------------*/
#define DMA_FLAG_FEIF5    ((uint32_t)0x20000040) /*!< Stream5 FIFO错误 (DMA_HISR[6]) */
#define DMA_FLAG_DMEIF5   ((uint32_t)0x20000100) /*!< Stream5直接模式错误 (DMA_HISR[8]) */
#define DMA_FLAG_TEIF5    ((uint32_t)0x20000200) /*!< Stream5传输错误 (DMA_HISR[9]) */
#define DMA_FLAG_HTIF5    ((uint32_t)0x20000400) /*!< Stream5半传输完成 (DMA_HISR[10]) */
#define DMA_FLAG_TCIF5    ((uint32_t)0x20000800) /*!< Stream5传输完成 (DMA_HISR[11]) */

/*---------------------- Stream 6 标志 ----------------------*/
#define DMA_FLAG_FEIF6    ((uint32_t)0x20010000) /*!< Stream6 FIFO错误 (DMA_HISR[16]) */
#define DMA_FLAG_DMEIF6   ((uint32_t)0x20040000) /*!< Stream6直接模式错误 (DMA_HISR[18]) */
#define DMA_FLAG_TEIF6    ((uint32_t)0x20080000) /*!< Stream6传输错误 (DMA_HISR[19]) */
#define DMA_FLAG_HTIF6    ((uint32_t)0x20100000) /*!< Stream6半传输完成 (DMA_HISR[20]) */
#define DMA_FLAG_TCIF6    ((uint32_t)0x20200000) /*!< Stream6传输完成 (DMA_HISR[21]) */

/*---------------------- Stream 7 标志 ----------------------*/
#define DMA_FLAG_FEIF7    ((uint32_t)0x20400000) /*!< Stream7 FIFO错误 (DMA_HISR[22]) */
#define DMA_FLAG_DMEIF7   ((uint32_t)0x21000000) /*!< Stream7直接模式错误 (DMA_HISR[24]) */
#define DMA_FLAG_TEIF7    ((uint32_t)0x22000000) /*!< Stream7传输错误 (DMA_HISR[25]) */
#define DMA_FLAG_HTIF7    ((uint32_t)0x24000000) /*!< Stream7半传输完成 (DMA_HISR[26]) */
#define DMA_FLAG_TCIF7    ((uint32_t)0x28000000) /*!< Stream7传输完成 (DMA_HISR[27]) */


/**
  * @brief DMA标志位校验宏组
  * @details 该组宏定义用于验证DMA状态标志的合法性，确保标志位操作的安全性，
  *          防止因错误标志操作导致寄存器状态异常或程序逻辑错误
  */

/* 清除标志校验宏 */
#define IS_DMA_CLEAR_FLAG(FLAG) ((((FLAG) & 0x30000000) != 0x30000000) && /* 校验DMA控制器标识位：排除非法控制器ID */ \
                                 (((FLAG) & 0x30000000) != 0) &&          /* 验证必须包含有效控制器标识 */ \
                                 (((FLAG) & 0xC002F082) == 0x00) &&      /* 过滤保留位：确保bit22、bit15-11、bit7、bit1等为0 */ \
                                 ((FLAG) != 0x00))                       /* 排除零值：防止误清所有标志位 */

/**
  * @brief DMA状态标志校验参数组
  * @details 该组定义包含DMA2控制器所有流(Stream0-7)的状态标志校验规则，
  *          确保DMA标志获取操作中参数的有效性和安全性
  */

/* 流0-7标志校验宏（按流分组注释） */
#define IS_DMA_GET_FLAG(FLAG) ( \
    /* Stream0标志组 */ \
    ((FLAG) == DMA_FLAG_TCIF0)  || ((FLAG) == DMA_FLAG_HTIF0)  || \
    ((FLAG) == DMA_FLAG_TEIF0)  || ((FLAG) == DMA_FLAG_DMEIF0) || \
    ((FLAG) == DMA_FLAG_FEIF0)  || \
    /* Stream1标志组 */ \
    ((FLAG) == DMA_FLAG_TCIF1)  || ((FLAG) == DMA_FLAG_HTIF1)  || \
    ((FLAG) == DMA_FLAG_TEIF1)  || ((FLAG) == DMA_FLAG_DMEIF1) || \
    ((FLAG) == DMA_FLAG_FEIF1)  || \
    /* Stream2标志组 */ \
    ((FLAG) == DMA_FLAG_TCIF2)  || ((FLAG) == DMA_FLAG_HTIF2)  || \
    ((FLAG) == DMA_FLAG_TEIF2)  || ((FLAG) == DMA_FLAG_DMEIF2) || \
    ((FLAG) == DMA_FLAG_FEIF2)  || \
    /* Stream3标志组 */ \
    ((FLAG) == DMA_FLAG_TCIF3)  || ((FLAG) == DMA_FLAG_HTIF3)  || \
    ((FLAG) == DMA_FLAG_TEIF3)  || ((FLAG) == DMA_FLAG_DMEIF3) || \
    ((FLAG) == DMA_FLAG_FEIF3)  || \
    /* Stream4标志组 */ \
    ((FLAG) == DMA_FLAG_TCIF4)  || ((FLAG) == DMA_FLAG_HTIF4)  || \
    ((FLAG) == DMA_FLAG_TEIF4)  || ((FLAG) == DMA_FLAG_DMEIF4) || \
    ((FLAG) == DMA_FLAG_FEIF4)  || \
    /* Stream5标志组 */ \
    ((FLAG) == DMA_FLAG_TCIF5)  || ((FLAG) == DMA_FLAG_HTIF5)  || \
    ((FLAG) == DMA_FLAG_TEIF5)  || ((FLAG) == DMA_FLAG_DMEIF5) || \
    ((FLAG) == DMA_FLAG_FEIF5)  || \
    /* Stream6标志组 */ \
    ((FLAG) == DMA_FLAG_TCIF6)  || ((FLAG) == DMA_FLAG_HTIF6)  || \
    ((FLAG) == DMA_FLAG_TEIF6)  || ((FLAG) == DMA_FLAG_DMEIF6) || \
    ((FLAG) == DMA_FLAG_FEIF6)  || \
    /* Stream7标志组 */ \
    ((FLAG) == DMA_FLAG_TCIF7)  || ((FLAG) == DMA_FLAG_HTIF7)  || \
    ((FLAG) == DMA_FLAG_TEIF7)  || ((FLAG) == DMA_FLAG_DMEIF7) || \
    ((FLAG) == DMA_FLAG_FEIF7)) /*!< 全量标志校验：覆盖DMA2所有8个流的TC/HT/TE/DME/FE五类事件标志 */

/**
  * @}
  */ 
/**
  * @brief DMA中断使能配置组
  * @details 该组定义包含DMA传输过程中可启用的中断类型，
  *          用于通过中断机制实时监控传输状态并及时处理异常
  */

/** @defgroup DMA_interrupt_enable_definitions 中断使能类型
  * @{
  */ 
#define DMA_IT_TC   ((uint32_t)0x00000010) /*!< 传输完成中断：当DMA_CNDTR寄存器归零时触发 */
#define DMA_IT_HT   ((uint32_t)0x00000008) /*!< 半传输完成中断：传输完成50%数据量时触发 */
#define DMA_IT_TE   ((uint32_t)0x00000004) /*!< 传输错误中断：检测到总线错误或外设未响应时触发 */
#define DMA_IT_DME  ((uint32_t)0x00000002) /*!< 直接模式错误中断：直接模式下地址未对齐时触发 */
#define DMA_IT_FE   ((uint32_t)0x00000080) /*!< FIFO错误中断：FIFO上溢/下溢时触发 */

/* 中断配置校验宏 */
#define IS_DMA_CONFIG_IT(IT) ( ( ((IT) & 0xFFFFFF61) == 0x00 ) && /* 过滤保留位：bit7、bit6、bit5、bit0必须为0 */ \
                               ((IT) != 0x00) )                   /* 排除零值：至少启用一个中断类型 */
/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @brief DMA中断状态标志寄存器映射
  * @details 本组宏精确定义DMA1/DMA2控制器各数据流的中断状态位物理地址
  *          编码规则：32位数值 = [31:29]控制器ID | [28:25]流编号 | [15:0]中断类型偏移
  *          控制器ID：0x10000000(DMA1), 0x20000000(DMA2)
  *          流编号：0-7对应Stream0-7（数值为流编号左移25位）
  *          中断类型偏移：FEIF=0x01, DMEIF=0x04, TEIF=0x08, HTIF=0x10, TCIF=0x20
  */

/** @defgroup DMA_interrupts_definitions 中断状态标志组
  * @{
  */
/*-------------------------- DMA1 Controller ------------------------*/
/* Stream 0 */
#define DMA_IT_FEIF0  ((uint32_t)0x90000001)  /*!< DMA1流0 FIFO错误中断（0x10000000|0<<25|0x01）*/
#define DMA_IT_DMEIF0 ((uint32_t)0x10001004)  /*!< DMA1流0直接模式错误（0x10000000|0<<25|0x04<<8）*/
#define DMA_IT_TEIF0  ((uint32_t)0x10002008)  /*!< DMA1流0传输错误（0x10000000|0<<25|0x08<<8）*/
#define DMA_IT_HTIF0  ((uint32_t)0x10004010)  /*!< DMA1流0半传输完成（0x10000000|0<<25|0x10<<8）*/
#define DMA_IT_TCIF0  ((uint32_t)0x10008020)  /*!< DMA1流0传输完成（0x10000000|0<<25|0x20<<8）*/

/* Stream 1 */
#define DMA_IT_FEIF1  ((uint32_t)0x90000040)  /*!< DMA1流1 FIFO错误（0x10000000|1<<25|0x01）*/
#define DMA_IT_DMEIF1 ((uint32_t)0x10001100)  /*!< DMA1流1直接模式错误（0x10000000|1<<25|0x04<<8）*/
#define DMA_IT_TEIF1  ((uint32_t)0x10002200)  /*!< DMA1流1传输错误（0x10000000|1<<25|0x08<<8）*/
#define DMA_IT_HTIF1  ((uint32_t)0x10004400)  /*!< DMA1流1半传输完成（0x10000000|1<<25|0x10<<8）*/
#define DMA_IT_TCIF1  ((uint32_t)0x10008800)  /*!< DMA1流1传输完成（0x10000000|1<<25|0x20<<8）*/

/* Stream 2 */
#define DMA_IT_FEIF2  ((uint32_t)0x90010000)  /*!< DMA1流2 FIFO错误（0x10000000|2<<25|0x01）*/
#define DMA_IT_DMEIF2 ((uint32_t)0x10041000)  /*!< DMA1流2直接模式错误（0x10000000|2<<25|0x04<<8）*/
#define DMA_IT_TEIF2  ((uint32_t)0x10082000)  /*!< DMA1流2传输错误（0x10000000|2<<25|0x08<<8）*/
#define DMA_IT_HTIF2  ((uint32_t)0x10104000)  /*!< DMA1流2半传输完成（0x10000000|2<<25|0x10<<8）*/
#define DMA_IT_TCIF2  ((uint32_t)0x10208000)  /*!< DMA1流2传输完成（0x10000000|2<<25|0x20<<8）*/

/* Stream 3 */
#define DMA_IT_FEIF3  ((uint32_t)0x90400000)  /*!< DMA1流3 FIFO错误（0x10000000|3<<25|0x01）*/
#define DMA_IT_DMEIF3 ((uint32_t)0x11001000)  /*!< DMA1流3直接模式错误（0x10000000|3<<25|0x04<<8）*/
#define DMA_IT_TEIF3  ((uint32_t)0x12002000)  /*!< DMA1流3传输错误（0x10000000|3<<25|0x08<<8）*/
#define DMA_IT_HTIF3  ((uint32_t)0x14004000)  /*!< DMA1流3半传输完成（0x10000000|3<<25|0x10<<8）*/
#define DMA_IT_TCIF3  ((uint32_t)0x18008000)  /*!< DMA1流3传输完成（0x10000000|3<<25|0x20<<8）*/

/*-------------------------- DMA2 Controller ------------------------*/
/* Stream 4 */
#define DMA_IT_FEIF4  ((uint32_t)0xA0000001)  /*!< DMA2流4 FIFO错误（0x20000000|4<<25|0x01）*/
#define DMA_IT_DMEIF4 ((uint32_t)0x20001004)  /*!< DMA2流4直接模式错误（0x20000000|4<<25|0x04<<8）*/
#define DMA_IT_TEIF4  ((uint32_t)0x20002008)  /*!< DMA2流4传输错误（0x20000000|4<<25|0x08<<8）*/
#define DMA_IT_HTIF4  ((uint32_t)0x20004010)  /*!< DMA2流4半传输完成（0x20000000|4<<25|0x10<<8）*/
#define DMA_IT_TCIF4  ((uint32_t)0x20008020)  /*!< DMA2流4传输完成（0x20000000|4<<25|0x20<<8）*/

/* Stream 5 */
#define DMA_IT_FEIF5  ((uint32_t)0xA0000040)  /*!< DMA2流5 FIFO错误（0x20000000|5<<25|0x01）*/
#define DMA_IT_DMEIF5 ((uint32_t)0x20001100)  /*!< DMA2流5直接模式错误（0x20000000|5<<25|0x04<<8）*/
#define DMA_IT_TEIF5  ((uint32_t)0x20002200)  /*!< DMA2流5传输错误（0x20000000|5<<25|0x08<<8）*/
#define DMA_IT_HTIF5  ((uint32_t)0x20004400)  /*!< DMA2流5半传输完成（0x20000000|5<<25|0x10<<8）*/
#define DMA_IT_TCIF5  ((uint32_t)0x20008800)  /*!< DMA2流5传输完成（0x20000000|5<<25|0x20<<8）*/

/* Stream 6 */
#define DMA_IT_FEIF6  ((uint32_t)0xA0010000)  /*!< DMA2流6 FIFO错误（0x20000000|6<<25|0x01）*/
#define DMA_IT_DMEIF6 ((uint32_t)0x20041000)  /*!< DMA2流6直接模式错误（0x20000000|6<<25|0x04<<8）*/
#define DMA_IT_TEIF6  ((uint32_t)0x20082000)  /*!< DMA2流6传输错误（0x20000000|6<<25|0x08<<8）*/
#define DMA_IT_HTIF6  ((uint32_t)0x20104000)  /*!< DMA2流6半传输完成（0x20000000|6<<25|0x10<<8）*/
#define DMA_IT_TCIF6  ((uint32_t)0x20208000)  /*!< DMA2流6传输完成（0x20000000|6<<25|0x20<<8）*/

/* Stream 7 */
#define DMA_IT_FEIF7  ((uint32_t)0xA0400000)  /*!< DMA2流7 FIFO错误（0x20000000|7<<25|0x01）*/
#define DMA_IT_DMEIF7 ((uint32_t)0x21001000)  /*!< DMA2流7直接模式错误（0x20000000|7<<25|0x04<<8）*/
#define DMA_IT_TEIF7  ((uint32_t)0x22002000)  /*!< DMA2流7传输错误（0x20000000|7<<25|0x08<<8）*/
#define DMA_IT_HTIF7  ((uint32_t)0x24004000)  /*!< DMA2流7半传输完成（0x20000000|7<<25|0x10<<8）*/
#define DMA_IT_TCIF7  ((uint32_t)0x28008000)  /*!< DMA2流7传输完成（0x20000000|7<<25|0x20<<8）*/
/**
  * @}
  */
/**
  * @brief  DMA中断标志清除有效性验证宏
  * @param  IT: 需要清除的DMA中断标志位组合
  * @retval 布尔值，True表示是有效的中断标志组合
  * @note   该宏用于验证传入的中断标志组合是否符合硬件规范，确保不会清除保留位或非法组合
  *         验证逻辑分为四层防护：
  *         1. 高两位不能同时为1（0x30000000掩码）
  *         2. 高两位不能全为0
  *         3. 整体不能为0值
  *         4. 保留位必须为0（0x40820082掩码）
  */
// 检查最高两位是否同时为1（非法状态）
// 确保最高两位不全为0（至少有一位被置位）
// 排除全零输入（无效标志）
// 保留位校验（bit31,23,15,7,1必须为0）
#define IS_DMA_CLEAR_IT(IT) ((((IT) & 0x30000000) != 0x30000000) && \
                             (((IT) & 0x30000000) != 0) && \
                             ((IT) != 0x00) && \
                             (((IT) & 0x40820082) == 0x00))

/**
  * @brief  DMA中断标志获取有效性验证宏
  * @param  IT: 需要查询的DMA中断标志位
  * @retval 布尔值，True表示是有效的中断标志位
  * @note   该宏验证输入是否为合法的单一中断标志位，覆盖DMA所有流的中断类型：
  *         - 传输完成（TCIFx）
  *         - 半传输完成（HTIFx）
  *         - 传输错误（TEIFx）
  *         - 直接模式错误（DMEIFx）
  *         - FIFO错误（FEIFx）
  */
#define IS_DMA_GET_IT(IT) (((IT) == DMA_IT_TCIF0) || ((IT) == DMA_IT_HTIF0)  || \
                           ((IT) == DMA_IT_TEIF0) || ((IT) == DMA_IT_DMEIF0) || \
                           ((IT) == DMA_IT_FEIF0) || ((IT) == DMA_IT_TCIF1)  || \
                           ((IT) == DMA_IT_HTIF1) || ((IT) == DMA_IT_TEIF1)  || \
                           ((IT) == DMA_IT_DMEIF1)|| ((IT) == DMA_IT_FEIF1)  || \
                           ((IT) == DMA_IT_TCIF2) || ((IT) == DMA_IT_HTIF2)  || \
                           ((IT) == DMA_IT_TEIF2) || ((IT) == DMA_IT_DMEIF2) || \
                           ((IT) == DMA_IT_FEIF2) || ((IT) == DMA_IT_TCIF3)  || \
                           ((IT) == DMA_IT_HTIF3) || ((IT) == DMA_IT_TEIF3)  || \
                           ((IT) == DMA_IT_DMEIF3)|| ((IT) == DMA_IT_FEIF3)  || \
                           ((IT) == DMA_IT_TCIF4) || ((IT) == DMA_IT_HTIF4)  || \
                           ((IT) == DMA_IT_TEIF4) || ((IT) == DMA_IT_DMEIF4) || \
                           ((IT) == DMA_IT_FEIF4) || ((IT) == DMA_IT_TCIF5)  || \
                           ((IT) == DMA_IT_HTIF5) || ((IT) == DMA_IT_TEIF5)  || \
                           ((IT) == DMA_IT_DMEIF5)|| ((IT) == DMA_IT_FEIF5)  || \
                           ((IT) == DMA_IT_TCIF6) || ((IT) == DMA_IT_HTIF6)  || \
                           ((IT) == DMA_IT_TEIF6) || ((IT) == DMA_IT_DMEIF6) || \
                           ((IT) == DMA_IT_FEIF6) || ((IT) == DMA_IT_TCIF7)  || \
                           ((IT) == DMA_IT_HTIF7) || ((IT) == DMA_IT_TEIF7)  || \
                           ((IT) == DMA_IT_DMEIF7)|| ((IT) == DMA_IT_FEIF7))

/**
  * @}
  */ 
/**
  * @defgroup DMA_peripheral_increment_offset 
  * @brief 外设地址增量偏移模式配置组
  * @{
  */ 
#define DMA_PINCOS_Psize      ((uint32_t)0x00000000)  /*!< 外设地址按数据宽度递增：每次递增量为外设数据宽度(PSIZE) */
#define DMA_PINCOS_WordAligned ((uint32_t)0x00008000) /*!< 外设地址按字对齐递增：地址增量固定为32位字长(4字节)，与PSIZE无关 */

/**
  * @brief 验证外设增量偏移配置有效性
  * @param SIZE: 需要验证的配置值
  * @retval 布尔值，True表示是有效配置
  */
#define IS_DMA_PINCOS_SIZE(SIZE) (((SIZE) == DMA_PINCOS_Psize) ||       /* 检查是否为按数据宽度递增模式 */\
                                  ((SIZE) == DMA_PINCOS_WordAligned))    /* 或按字对齐递增模式 */
/**
  * @}
  */

/**
  * @defgroup DMA_flow_controller_definitions 
  * @brief DMA流控制器选择配置组
  * @{
  */ 
#define DMA_FlowCtrl_Memory     ((uint32_t)0x00000000)  /*!< 内存端作为流控制器：由内存传输触发DMA请求 */
#define DMA_FlowCtrl_Peripheral ((uint32_t)0x00000020)  /*!< 外设端作为流控制器：由外设事件触发DMA请求 */

/**
  * @brief 验证流控制器配置有效性
  * @param CTRL: 需要验证的配置值
  * @retval 布尔值，True表示是有效配置
  */
#define IS_DMA_FLOW_CTRL(CTRL) (((CTRL) == DMA_FlowCtrl_Memory) ||      /* 检查是否为内存端控制 */\
                                ((CTRL) == DMA_FlowCtrl_Peripheral))    /* 或外设端控制 */
/**
  * @}
  */

/**
  * @defgroup DMA_memory_targets_definitions 
  * @brief 双缓冲区模式内存目标选择组
  * @{
  */ 
#define DMA_Memory_0            ((uint32_t)0x00000000)  /*!< 使用内存缓冲区0 */
#define DMA_Memory_1            ((uint32_t)0x00080000)  /*!< 使用内存缓冲区1（双缓冲区模式时有效） */

/**
  * @brief 验证当前内存目标配置有效性
  * @param MEM: 需要验证的内存目标值
  * @retval 布尔值，True表示是有效配置
  */
#define IS_DMA_CURRENT_MEM(MEM) (((MEM) == DMA_Memory_0) ||    /* 检查是否为缓冲区0 */\
                                 ((MEM) == DMA_Memory_1))       /* 或缓冲区1 */
/**
  * @}
  */

/* 导出宏 ------------------------------------------------------------*/
/* 导出函数 --------------------------------------------------------*/ 

/**
  * @brief 复位DMA流配置到默认状态
  * @param DMAy_Streamx: 指定要复位的DMA流，取值范围：DMA1_Stream0到DMA2_Stream7
  */
void DMA_DeInit(DMA_Stream_TypeDef* DMAy_Streamx);

/**
  * @brief 初始化DMA流参数配置
  * @param DMAy_Streamx: 要配置的DMA流
  * @param DMA_InitStruct: 初始化参数结构体指针，包含：
  *        - 通道选择
  *        - 传输方向
  *        - 优先级
  *        - 数据宽度
  *        - 地址递增模式
  *        - 循环模式等
  */
void DMA_Init(DMA_Stream_TypeDef* DMAy_Streamx, DMA_InitTypeDef* DMA_InitStruct);

/**
  * @brief 设置DMA初始化结构体默认值
  * @param DMA_InitStruct: 要初始化的结构体指针
  */
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct);

/**
  * @brief 启用/禁用指定DMA流
  * @param DMAy_Streamx: 目标DMA流
  * @param NewState: 新状态 ENABLE/DISABLE
  */
void DMA_Cmd(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState);

/* 可选配置函数 *******************************************/
/**
  * @brief 配置外设地址增量偏移模式
  * @param DMAy_Streamx: 目标DMA流
  * @param DMA_Pincos: 增量模式 @ref DMA_peripheral_increment_offset
  */
void DMA_PeriphIncOffsetSizeConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_Pincos);

/**
  * @brief 配置DMA流控制器
  * @param DMAy_Streamx: 目标DMA流
  * @param DMA_FlowCtrl: 流控制器类型 @ref DMA_flow_controller_definitions
  */
void DMA_FlowControllerConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FlowCtrl);

/* 数据计数器操作函数 *****************************************************/
/**
  * @brief 设置DMA流数据传输计数器
  * @param DMAy_Streamx: 目标DMA流，范围DMA1_Stream0~DMA2_Stream7
  * @param Counter: 要设置的传输数据项数量（0-65535）
  * @note  实际最大有效值取决于传输位宽：
  *        - 8位传输: 最大65535
  *        - 16位传输: 最大32767
  *        - 32位传输: 最大16383
  */
void DMA_SetCurrDataCounter(DMA_Stream_TypeDef* DMAy_Streamx, uint16_t Counter);

/**
  * @brief 获取DMA流当前剩余数据传输计数器
  * @param DMAy_Streamx: 目标DMA流
  * @retval 剩余待传输数据项数量
  * @note 该值在传输过程中动态递减，读取时需暂停DMA
  */
uint16_t DMA_GetCurrDataCounter(DMA_Stream_TypeDef* DMAy_Streamx);

/* 双缓冲区模式操作函数 ***********************************************/
/**
  * @brief 配置双缓冲区模式参数
  * @param DMAy_Streamx: 目标DMA流
  * @param Memory1BaseAddr: 第二个内存缓冲区的32位地址（需4字节对齐）
  * @param DMA_CurrentMemory: 初始激活缓冲区 @ref DMA_memory_targets_definitions
  */
void DMA_DoubleBufferModeConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t Memory1BaseAddr,
                                uint32_t DMA_CurrentMemory);

/**
  * @brief 启用/禁用双缓冲区模式
  * @param DMAy_Streamx: 目标DMA流
  * @param NewState: 新状态 ENABLE/DISABLE
  * @note 启用前必须通过DMA_DoubleBufferModeConfig配置缓冲区
  */
void DMA_DoubleBufferModeCmd(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState);

/**
  * @brief 动态切换目标内存缓冲区
  * @param DMAy_Streamx: 目标DMA流
  * @param MemoryBaseAddr: 新缓冲区的32位地址
  * @param DMA_MemoryTarget: 目标缓冲区选择 @ref DMA_memory_targets_definitions
  */
void DMA_MemoryTargetConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t MemoryBaseAddr,
                            uint32_t DMA_MemoryTarget);

/**
  * @brief 获取当前活动的内存缓冲区
  * @param DMAy_Streamx: 目标DMA流
  * @retval 当前活动缓冲区 @ref DMA_memory_targets_definitions
  */
uint32_t DMA_GetCurrentMemoryTarget(DMA_Stream_TypeDef* DMAy_Streamx);

/* 中断与状态管理函数 **********************************/
/**
  * @brief 获取DMA流使能状态
  * @param DMAy_Streamx: 目标DMA流
  * @retval 当前状态 ENABLED/DISABLED
  */
FunctionalState DMA_GetCmdStatus(DMA_Stream_TypeDef* DMAy_Streamx);

/**
  * @brief 获取DMA流FIFO状态
  * @param DMAy_Streamx: 目标DMA流
  * @retval 状态位图，包含：
  *         - DMA_FIFOStatus_InDirectMode: 直接模式状态
  *         - DMA_FIFOStatus_Threshold: FIFO阈值状态
  */
uint32_t DMA_GetFIFOStatus(DMA_Stream_TypeDef* DMAy_Streamx);

/**
  * @brief 检查指定状态标志位
  * @param DMAy_Streamx: 目标DMA流
  * @param DMA_FLAG: 要检查的标志位，可选值：
  *         - DMA_FLAG_TCIFx: 传输完成标志
  *         - DMA_FLAG_HTIFx: 半传输标志
  *         - DMA_FLAG_TEIFx: 传输错误标志
  *         - DMA_FLAG_DMEIFx: 直接模式错误标志
  *         - DMA_FLAG_FEIFx: FIFO错误标志
  * @retval 标志状态 SET/RESET
  */
FlagStatus DMA_GetFlagStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG);

/**
  * @brief 清除指定状态标志
  * @param DMAy_Streamx: 目标DMA流
  * @param DMA_FLAG: 要清除的标志位（支持多个标志位组合）
  */
void DMA_ClearFlag(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG);

/**
  * @brief 配置DMA中断使能
  * @param DMAy_Streamx: 目标DMA流
  * @param DMA_IT: 中断类型，支持组合：
  *         - DMA_IT_TC: 传输完成中断
  *         - DMA_IT_HT: 半传输中断 
  *         - DMA_IT_TE: 传输错误中断
  * @param NewState: 中断状态 ENABLE/DISABLE
  */
void DMA_ITConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT, FunctionalState NewState);

/**
  * @brief 检查中断状态
  * @param DMAy_Streamx: 目标DMA流
  * @param DMA_IT: 中断类型 @ref DMA_IT_TC/DMA_IT_HT/DMA_IT_TE
  * @retval 中断状态 SET/RESET
  */
ITStatus DMA_GetITStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT);

/**
  * @brief 清除中断挂起标志
  * @param DMAy_Streamx: 目标DMA流
  * @param DMA_IT: 要清除的中断类型
  * @note 需先检查中断状态后再清除
  */
void DMA_ClearITPendingBit(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT);

#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_DMA_H */

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

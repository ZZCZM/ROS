/**
  ******************************************************************************
  * @file   usartx.h
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Header File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#ifndef __USRATX_H
#define __USRATX_H 
#include "stdio.h"
#include "sys.h"
#include "system.h"

/* FreeRTOS任务配置 ----------------------------------------------------------
 * RX_DATA_TASK_SIZE：接收任务堆栈大小（单位字）
 * RX_DATA_TASK_PRIO：接收任务优先级（0最低，configMAX_PRIORITIES-1最高）
 */
#define RX_DATA_TASK_SIZE   1024  // 接收任务堆栈（1024*4=4096字节）
#define RX_DATA_TASK_PRIO  1      // 高于空闲任务，低于运动控制

#define TX_DATA_TASK_SIZE   512   // 发送任务堆栈
#define TX_DATA_TASK_PRIO  4      // 中等优先级

/* 数据帧协议定义 ------------------------------------------------------------
 * 帧结构示例：| 帧头 | 数据 | CRC | 帧尾 |
 * 底盘控制帧：0X7B + 数据 + 0X7D
 * 机械臂帧： 0XAA + 数据 + 0XBB
 * 时间帧：   0X6A + 数据 + 0X6B
 */
#define FRAME_HEADER_CAR 0X7B     // 底盘控制帧头（ASCII '{'）
#define FRAME_TAIL_CAR 0X7D       // 帧尾（ASCII '}'）

#define FRAME_HEADER_MOVEIT 0XAA  // 机械臂控制帧头（自定义）
#define FRAME_TAIL_MOVEIT 0XBB    // 帧尾

#define FRAME_HEADER_TIME 0X6A    // 休眠管理帧头（ASCII 'j'）
#define FRAME_TAIL_TIME 0X6B      // 帧尾（ASCII 'k'）

/* 数据传输配置 */
#define SEND_DATA_SIZE    24      // 发送缓冲区大小（字节）
#define RECEIVE_DATA_SIZE 11       // 单帧接收数据长度

/* 工作模式标识 */
#define follower 2         // 跟随模式（从设备）
#define default_mode 1     // 自主模式（主设备）

/* USART3硬件配置 ----------------------------------------------------------*/
// 发送端配置（PB10）
#define USART_TX_PORT GPIOB        
#define USART_TX_PIN GPIO_Pin_10   // 引脚号（根据封装确认物理位置）
#define USART_TX_GPIO_CLK RCC_AHB1Periph_GPIOB // GPIO时钟
#define USART_TX_USART_CLK RCC_APB1Periph_USART3 // USART时钟

// 接收端配置（PB11）
#define USART_RX_PORT GPIOB        
#define USART_RX_PIN GPIO_Pin_11  
#define USART_RX_GPIO_CLK RCC_AHB1Periph_GPIOB
#define USART_RX_USART_CLK RCC_APB1Periph_USART3

// 复用功能及实例
#define USART_AF GPIO_AF_USART3    // 复用功能映射
#define USART_ID USART3            // USART实例
#define USART_IRQn USART3_IRQn     // 中断号

/**
 * @file usartx.h
 * @brief USART3串口及DMA配置头文件
 * @details 该文件主要完成以下功能：
 * 1. 配置USART3的GPIO引脚复用、时钟使能、波特率等参数
 * 2. 定义DMA1数据流及通道配置（Stream3用于发送，Stream1用于接收）
 * 3. 设置DMA传输完成事件标志
 * 4. 定义陀螺仪加速度计三轴数据结构体
 * 适用场景：基于STM32的DMA+USART3通信系统开发
 */

// 复用功能及实例配置区
#define USART_AF GPIO_AF_USART3    // 指定USART3的GPIO复用功能编号
#define USART_ID USART3            // 指定使用的USART外设实例为USART3
#define USART_IRQn USART3_IRQn     // 定义USART3的中断请求编号

/*------------------------- USART3调试接口配置 -------------------------*/
//DMA+USART TX 联合配置
#define DEBUG_USART                       USART3  // 指定调试用USART外设为USART3
#define DEBUG_USART_CLK                   RCC_APB1Periph_USART3 // USART3的时钟源（APB1总线）
#define DEBUG_USART_RX_GPIO_PORT          GPIOB   // RX引脚所在的GPIO端口（GPIOB）
#define DEBUG_USART_RX_GPIO_CLK           RCC_AHB1Periph_GPIOB // GPIOB端口时钟使能（AHB1总线）
#define DEBUG_USART_RX_PIN                GPIO_Pin_11 // RX引脚编号（PB11）
#define DEBUG_USART_RX_AF                 GPIO_AF_USART3 // RX引脚复用功能映射为USART3
#define DEBUG_USART_RX_SOURCE             GPIO_PinSource11 // RX引脚源编号（与引脚号对应）

#define DEBUG_USART_TX_GPIO_PORT          GPIOB   // TX引脚所在的GPIO端口（GPIOB）
#define DEBUG_USART_TX_GPIO_CLK           RCC_AHB1Periph_GPIOB // GPIOB端口时钟使能
#define DEBUG_USART_TX_PIN                GPIO_Pin_10 // TX引脚编号（PB10）
#define DEBUG_USART_TX_AF                 GPIO_AF_USART3 // TX引脚复用功能映射
#define DEBUG_USART_TX_SOURCE             GPIO_PinSource10 // TX引脚源编号

#define DEBUG_USART_BAUDRATE              115200  // 串口通信波特率设置（115200bps）

/*------------------------- DMA传输配置 -------------------------*/
#define DEBUG_USART_DR_BASE               ((uint32_t)&USART3->DR) // USART3数据寄存器地址		
#define SEND_BUFFER_SIZE                  SEND_DATA_SIZE // 定义发送缓冲区大小（依赖SEND_DATA_SIZE宏）
#define RX_BUFFER_SIZE                    RECEIVE_DATA_SIZE  // 定义接收缓冲区大小（依赖RECEIVE_DATA_SIZE宏）

// DMA发送配置
#define DEBUG_USART_DMA_TX_CLK               RCC_AHB1Periph_DMA1 // 发送DMA时钟（DMA1）
#define DEBUG_USART_DMA_TX_CHANNEL           DMA_Channel_4 // 发送通道编号（通道4）
#define DEBUG_USART_DMA_TX_STREAM            DMA1_Stream3 // 发送数据流（流3）

// DMA接收配置 
#define DEBUG_USART_DMA_RX_CLK               RCC_AHB1Periph_DMA1 // 接收DMA时钟
#define DEBUG_USART_DMA_RX_CHANNEL           DMA_Channel_4 // 接收通道编号
#define DEBUG_USART_DMA_RX_STREAM            DMA1_Stream1 // 接收数据流（流1）

#define DEBUG_USART_DMA_RX_STREAM_IRQn       DMA1_Stream1_IRQn // 接收流中断请求编号

/*------------------------- DMA事件标志 -------------------------*/
#define DMA_TRANSFER_COMPLETE (1 << 0)  // DMA传输完成标志位（位0）

/**
 * @file usartx.h
 * @brief 传感器数据通信结构体定义
 * @details 包含三类核心数据结构：
 * 1. MPU6050三轴传感器原始数据结构
 * 2. 串口发送数据结构（包含传感器数据组合包）
 * 3. 串口接收数据结构（包含控制参数）
 * 特点：
 * - 基于字节对齐的内存布局
 * - 帧头帧尾校验机制
 * - 支持复合传感器数据聚合传输
 */

/*****A structure for storing triaxial data of a gyroscope accelerometer*****/
/*****用于存放陀螺仪加速度计三轴数据的结构体*********************************/
/**
 * @struct __Mpu6050_Data_
 * @brief MPU6050三轴原始数据容器
 * @details 存储传感器原始ADC数值，适用于：
 * - 陀螺仪角速度原始值
 * - 加速度计线性加速度原始值
 * 备注：
 * - 每个轴数据占2字节(short类型)
 * - 取值范围：-32768~32767
 * - 需要后续校准和单位转换
 */
typedef struct __Mpu6050_Data_ 
{
    short X_data; ///< X轴原始数据 [范围：-32768, 32767]
    short Y_data; ///< Y轴原始数据（内存地址偏移2字节）
    short Z_data; ///< Z轴原始数据（内存地址偏移4字节）
} Mpu6050_Data;   // 总大小6字节（符合结构体字节对齐规则）

/*******The structure of the serial port sending data************/
/*******串口发送数据的结构体*************************************/
/**
 * @struct _SEND_DATA_
 * @brief 传感器数据发送包结构
 * @details 包含：
 * - 原始字节缓冲区
 * - 格式化传感器数据组合包
 * 数据传输格式：
 * 帧头(1B) | 速度X(2B) | 速度Y(2B) | 速度Z(2B) | 电压(2B)
 * | 加速度计数据(6B) | 陀螺仪数据(6B) | 帧尾(1B)
 */
typedef struct _SEND_DATA_  
{
    unsigned char buffer[SEND_DATA_SIZE]; ///< 原始字节缓冲区（用于DMA直接访问）
    struct _Sensor_Str_                   ///< 结构化传感器数据组合包
    {
        unsigned char Frame_Header;     ///< 帧起始标识（例如0xAA）
        short X_speed;  ///< X轴速度 [单位：cm/s] 范围：±3276.7
        short Y_speed;  ///< Y轴速度（内存地址偏移3字节）
        short Z_speed;  ///< Z轴速度（内存地址偏移5字节）
        short Power_Voltage;  ///< 电源电压 [单位：0.1V] 范围：0-65.535V
        
        Mpu6050_Data Accelerometer; ///< 加速度计三轴数据（内存地址偏移9字节）
        Mpu6050_Data Gyroscope;     ///< 陀螺仪三轴数据（地址偏移15字节）
        
        unsigned char Frame_Tail;   ///< 帧结束标识（例如0x55）
    } Sensor_Str;                   // 总大小：1+2+2+2+2+6+6+1=22字节
} SEND_DATA;  ///< 总结构体大小 SEND_DATA_SIZE + 22字节

/**
 * @struct _RECEIVE_DATA_
 * @brief 控制参数接收数据结构
 * @details 包含：
 * - 原始接收缓冲区
 * - 格式化控制参数包
 * 数据传输格式：
 * 帧头(1B) | X速度(4B) | Y速度(4B) | Z速度(4B) | 帧尾(1B)
 */
typedef struct _RECEIVE_DATA_  
{
    unsigned char buffer[RECEIVE_DATA_SIZE]; ///< 原始接收缓冲区
    struct _Control_Str_                    ///< 结构化控制参数包
    {
        unsigned char Frame_Header;   ///< 帧起始标识（例如0xBB）
        float X_speed;  ///< X轴目标速度 [单位：m/s] IEEE754单精度
        float Y_speed;  ///< Y轴目标速度（内存地址偏移5字节）
        float Z_speed;  ///< Z轴目标速度（地址偏移9字节）
        unsigned char Frame_Tail;     ///< 帧结束标识（例如0x66）
    } Control_Str;        // 总大小：1+4+4+4+1=14字节
} RECEIVE_DATA;  ///< 总结构体大小 RECEIVE_DATA_SIZE + 14字节


/**
 * @file usartx.h
 * @brief 串口系统功能接口集
 * @details 包含五大功能模块：
 * 1. FreeRTOS任务级数据通信接口
 * 2. USART及DMA硬件配置接口
 * 3. 中断服务处理接口
 * 4. 数据协议转换算法
 * 5. 数据完整性校验接口
 * 架构特点：分层设计，支持多串口协同工作
 */

/*--------------------- FreeRTOS任务函数 ---------------------*/
/**
 * @brief 发送数据任务入口
 * @param pvParameters FreeRTOS任务参数指针
 * @note 在RTOS中周期性执行发送数据包操作
 * @warning 需在FreeRTOS任务中调用
 */
void TX_data_task(void *pvParameters);

/**
 * @brief 接收数据任务入口
 * @param pvParameters FreeRTOS任务参数指针
 * @note 实时处理接收缓冲区数据
 */
void Rx_data_task(void *pvParameters);

/*--------------------- 数据处理函数 ---------------------*/
/**
 * @brief 接收数据解析主函数
 * @return 0-校验失败 1-有效控制指令
 * @warning 需在接收中断后调用
 */
int Received_data_processing(void);

/**
 * @brief 传感器原始数据转换协议包
 * @details 执行操作：
 * - 转换MPU6050原始值为工程单位
 * - 封装数据为SEND_DATA格式
 */
void data_transition(void);

/*--------------------- 串口发送驱动 ---------------------*/
///@{
/** 
 * @brief USARTx批量发送接口
 * @note 使用DMA传输模式，自动处理发送缓冲区
 * 触发条件：发送缓冲区就绪时调用
 */
void USART1_SEND(void);  ///< 通过USART1发送完整数据包
void USART3_SEND(void);  ///< 主调试串口发送（使用USART3）
void USART5_SEND(void);  ///< 扩展串口发送接口
///@}

/*--------------------- CAN通信扩展 ---------------------*/
/**
 * @brief CAN总线数据发送接口
 * @note 实现策略：
 * - 转换USART数据为CAN协议格式
 * - 使用CAN2.0B扩展帧
 */
void CAN_SEND(void);

/*--------------------- 硬件初始化 ---------------------*/
///@{
/**
 * @brief USARTx外设初始化
 * @param bound 波特率 (典型值：9600/115200)
 * @details 配置内容：
 * - 时钟使能
 * - GPIO复用配置
 * - 中断优先级设置
 * - DMA通道使能
 */
void uart1_init(u32 bound);  ///< 初始化USART1（RS485通信）
void uart2_init(u32 bound);  ///< 预留串口初始化
void uart3_init(u32 bound);  ///< 主调试串口初始化（115200bps）
void uart5_init(u32 bound);  ///< 扩展串口初始化
///@}

/**
 * @brief DMA传输全配置
 * @note 包含：
 * - 发送/接收数据流配置
 * - 循环模式设置
 * - 传输完成中断使能
 */
void USART_DMA_Config(void);

/*--------------------- 中断服务处理 ---------------------*/
///@{
/**
 * @brief USARTx中断服务入口
 * @return 0-无数据 1-数据就绪
 * @note 执行操作：
 * - 清除中断标志
 * - 接收字节存入环形缓冲区
 * - 触发接收信号量
 */
int USART1_IRQHandler(void);  ///< RS485通信中断处理
int USART2_IRQHandler(void);  ///< 预留中断处理
int USART3_IRQHandler(void);  ///< 主调试串口中断（DMA备用）
int UART5_IRQHandler(void);   ///< 扩展串口中断处理
///@}

/*--------------------- 数据转换算法 ---------------------*/
/**
 * @brief 速度向量转机械臂角度
 * @param Vx X轴速度分量 [m/s]
 * @param Vz Z轴速度分量 [m/s]
 * @return 关节目标角度 [度]
 * @note 采用反正切计算原理
 */
float Vz_to_Akm_Angle(float Vx, float Vz);

/**
 * @brief 速度参数协议解码
 * @param High 高字节数据
 * @param Low 低字节数据
 * @return 实际速度值 [m/s]
 * @warning 数据格式：Q格式16位有符号数
 */
float XYZ_Target_Speed_transition(u8 High, u8 Low);

/*--------------------- 字节级发送接口 ---------------------*/
//
/**
 * @brief USARTx单字节阻塞发送
 * @param data 待发送字节
 * @note 用于调试输出，使用轮询模式
 */
void usart1_send(u8 data);  ///< 兼容旧版调试接口
void usart3_send(u8 data);  // 主调试口printf底层实现+ROS通信
void usart5_send(u8 data);
//

/*--------------------- 数据校验 ---------------------*/
/**
 * @brief 累加和校验算法
 * @param Count_Number 数据长度
 * @param Mode 校验模式：0-累加 1-XOR
 * @return 校验字节
 * @note 支持两种校验方式，默认使用模式1
 */
u8 Check_Sum(unsigned char Count_Number, unsigned char Mode);



#endif


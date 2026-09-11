/**
  ******************************************************************************
  * @file   system.h
  * @author  工科女的日常: The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Header File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl(工科女的日常).
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#ifndef __SYSTEM_H
#define __SYSTEM_H

// Refer to all header files you need
//引用所有需要用到的头文件
#include "FreeRTOSConfig.h"
//FreeRTOS相关头文件 
//FreeRTOS related header files
#include "FreeRTOS.h"
#include "stm32f4xx.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
//The associated header file for the peripheral 
//外设的相关头文件
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "balance.h"
#include "led.h"
#include "oled.h"
#include "usart.h"
#include "usartx.h"
#include "adc.h"
#include "can.h"
#include "motor.h"
#include "timer.h"
#include "encoder.h"
#include "show.h"								   
#include "pstwo.h"
#include "key.h"
#include "robot_select_init.h"
#include "I2C.h"
#include "MPU6050.h"
#include "F4_flash.h"
#include "bsp_ili9806g_lcd.h"
#include "fonts.h"
#include "bsp_spi_flash.h"
#include "LowPower.h"
//C library function related header file
//C库函数的相关头文件
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stdarg.h"
/*------------------------------------------
【车辆类型枚举】定义系统支持的底盘类型
设计目的：统一管理不同机械结构的控制策略
------------------------------------------*/
typedef enum 
{
    Mec_Car = 0,     // 麦克纳姆轮全向底盘（四轮独立驱动）
    FourWheel_Car,   // 四轮差速转向底盘（两驱/四驱模式） 
    Tank_Car         // 履带式差速底盘（坦克转向模式）
} CarMode;
/* 使用说明：
   - 通过CAN总线或拨码开关设置当前车型
   - 影响运动学解算和控制参数加载
   - 枚举值对应参数存储区索引 */

/*------------------------------------------
【电机控制参数结构体】实现闭环速度控制
设计目的：存储单个电机的实时控制数据
------------------------------------------*/
typedef struct  
{
    /* 传感数据区 */
    float Encoder;     // 编码器瞬时速度值（单位：RPM）
                       // 数据来源：TIMx编码器接口捕获的脉冲频率
                       // 采样周期：5ms（与PWM周期同步）
    
    /* 执行输出区 */                   
    float Motor_Pwm;   // PWM占空比（范围：-2500~+2500）
                       // 符号表示转向：正值为正向旋转
                       // 最终写入TIMx->CCRx寄存器的计算值
    
    /* 控制目标区 */                   
    float Target;      // 目标转速设定值（单位：RPM）
                       // 来源：运动学解算模块输出
                       // 特殊值：0-急停，NaN-进入安全模式
    
    /* PID参数区 */
    float Velocity_KP; // 比例系数（典型值范围：0.1~5.0）
                       // 影响系统响应速度，过大会引发振荡
    float Velocity_KI; // 积分系数（典型值范围：0.01~0.5） 
                       // 消除静差，过高会导致积分饱和
} Motor_parameter;
/* 内存分布：
   - 每个电机对应独立实例（例：Motor_parameter motor1） 
   - 结构体对齐方式：4字节（sizeof=20 bytes） */

/*------------------------------------------
【平滑速度结构体】抑制运动控制高频噪声
设计目的：通过滑动窗口平均滤波处理原始速度
------------------------------------------*/
typedef struct  
{
    float VX;  // X轴平移速度（单位：m/s）
               // 正方向：车头前进方向
               // 滤波窗口：最近5个采样点
    
    float VY;  // Y轴平移速度（侧向移动）
               // 仅麦克纳姆轮车型有效
               // 滤波算法：一阶低通滤波（α=0.2）
    
    float VZ;  // Z轴旋转速度（单位：rad/s）
               // 正方向：逆时针旋转
               // 混合滤波：中值+均值滤波
} Smooth_Control;
/* 更新机制：
   - 由1kHz定时器中断服务程序更新
   - 数据消费者：无线遥测模块/运动控制模块 */ 
/****** 全局系统状态变量 ******/
extern u8 Flag_Stop;        // 紧急停止标志位（位控制）
                            // BIT0: 碰撞急停 BIT1: 悬崖检测 BIT2: 遥控器急停
extern int Divisor_Mode;    // 运动控制分频模式（0-同步模式 1-异步模式）
                            // 影响CAN总线报文发送频率（默认值：0）

/****** 底盘控制参数 ******/
extern u8 Car_Mode;         // 当前车型模式（对应CarMode枚举值）
                            // 取值范围：0~2（对应Mec_Car/Tank_Car等）
extern int Servo;           // 舵机转向角度（单位：0.1度）
                            // 取值范围：-900~+900（对应-90°~+90°）

/****** 运动控制参数 ******/
extern float RC_Velocity;   // 遥控器归一化速度输入（范围：-1.0~+1.0）
extern float Move_X;        // X轴平移速度指令（单位：m/s）
extern float Move_Y;        // Y轴平移速度指令（单位：m/s）
extern float Move_Z;        // Z轴旋转速度指令（单位：rad/s）

/****** 电机控制核心参数 ******/
extern float Velocity_KP;   // 全局速度环比例系数（默认值：2.5）
extern float Velocity_KI;   // 全局速度环积分系数（默认值：0.1）
extern Smooth_Control smooth_control;  // 三轴平滑速度（用于运动学解算输入）

/****** 电机实例化对象 ******/
extern Motor_parameter MOTOR_A;  // 左前轮电机参数（CAN ID: 0x201）
extern Motor_parameter MOTOR_B;  // 右前轮电机参数（CAN ID: 0x202） 
extern Motor_parameter MOTOR_C;  // 左后轮电机参数（CAN ID: 0x203）
extern Motor_parameter MOTOR_D;  // 右后轮电机参数（CAN ID: 0x204）

/****** 机械参数配置 ******/
extern float Encoder_precision;  // 编码器线数（脉冲数/转）（默认值：11.0）
extern float Wheel_perimeter;    // 驱动轮周长（单位：米）（默认值：0.157）
extern float Wheel_spacing;      // 同轴轮间距（单位：米）（默认值：0.38）
extern float Axle_spacing;       // 前后轴间距（单位：米）（默认值：0.29）
extern float Omni_turn_radiaus;  // 全向轮转向半径（单位：米）（特殊车型使用）

/****** 设备状态标志位 ******/
extern u8 PS2_ON_Flag;       // PS2手柄连接状态（0-未连接 1-已连接）
extern u8 APP_ON_Flag;       // 手机APP连接状态（BLE通信状态）
extern u8 Remote_ON_Flag;    // 遥控器使能状态（0-禁用 1-使能）
extern u8 CAN_ON_Flag;       // CAN总线激活状态（0-休眠 1-运行）
extern u8 Usart1_ON_Flag;    // 串口1调试使能（0-关闭 1-开启）
extern u8 Usart5_ON_Flag;    // 串口5通信状态（雷达模块连接状态）

/****** 导航控制标志 ******/
extern u8 Flag_Left;         // 左侧障碍物检测（0-无障碍 1-检测到障碍）
extern u8 Flag_Right;        // 右侧障碍物检测
extern u8 Flag_Direction;    // 路径方向标记（0-正向 1-逆向）
extern u8 Turn_Flag;         // 转向动作标记（BIT0: 左转 BIT1: 右转）

/****** 调试与诊断 ******/
extern u8 PID_Send;          // PID参数发布使能（0-关闭 1-开启遥测）
extern u8 car_A_steer_flag;  // 转向舵机故障标记（0-正常 1-卡滞）
extern int PS2_LX,PS2_LY;    // PS2左摇杆坐标（LX: -128~+127, LY: -128~+127）
extern int PS2_RX,PS2_RY;    // PS2右摇杆坐标
extern int PS2_KEY;          // PS2按键状态字（按位解析）

/****** 自检系统变量 ******/
extern int Check;            // 当前自检项目编号（0~15）
extern int Checking;         // 自检进行中标记（0-未开始 1-进行中）
extern int Checked;          // 已完成自检项目数
extern int CheckCount;       // 自检超时计数器（单位：100ms）
extern int CheckPhrase1;     // 第一阶段自检结果（位图）
extern int CheckPhrase2;     // 第二阶段自检结果（位图）
extern long int ErrorCode;   // 系统错误码（按位定义，BIT0~BIT31对应不同错误）

/****** 机械臂控制参数 ******/
extern float Moveit_Angle1;  // 关节1目标角度（单位：度）
extern float Moveit_Angle2;  // 关节2目标角度
extern float Moveit_Angle3;  // 关节3目标角度
extern float Moveit_Angle4;  // 关节4目标角度
extern int Moveit_PWM1;      // 关节1 PWM输出值（0~1000对应0%~100%）
extern int Moveit_PWM2;      // 关节2 PWM输出值
extern int Moveit_PWM3;      // 关节3 PWM输出值
extern int Moveit_PWM4;      // 关节4 PWM输出值

extern void vInitializeResourceMapping(void); // 外部初始化map

/****** 系统函数声明 ******/
void systemInit(void);  // 系统初始化函数（时钟/外设/协议栈初始化）
extern void prvInnovation_Point_2_Test_Task( void *pvParameters );  // 创新点2测试任务
extern void prvScenario2Task( void *pvParameters );  // 场景2任务

/****** 系统安全控制宏 ******/
// 系统启动保护延时（单位：定时器中断周期数）
// 设计目的：防止上电瞬间GPIO状态不稳定导致误动作
// 计算公式：实际时间(秒) = CONTROL_DELAY / 定时器频率(Hz)
// 示例：1000/100Hz = 10秒初始化过程
#define CONTROL_DELAY		1000  // 对应10秒延时（当定时器为100Hz时）

/****** 车型配置宏 ******/
// 支持的底盘类型总数（需与CarMode枚举同步更新）
// 版本变更记录：
// v1.0 - 初始版本（3种车型）
// v2.0 - 新增TwoWheel_Car（需改为4）
#define CAR_NUMBER    3  // 当前实际车型数量：Mec_Car, FourWheel_Car, Tank_Car

/****** 标准频率基准宏 ******/
/* 通用定时器频率定义（单位：Hz）
   应用场景：
   +----------------+-------------------------------+
   | 频率值         | 适用场景                      |
   +----------------+-------------------------------+
   | RATE_1_HZ      | 低功耗心跳检测                |
   | RATE_5_HZ      | 系统状态监控                  |
   | RATE_10_HZ     | 传感器融合更新                | 
   | RATE_20_HZ     | 导航路径规划                  |
   | RATE_25_HZ     | 视频帧率控制                  |
   | RATE_50_HZ     | 机械臂运动插补                |
   | RATE_100_HZ    | 电机闭环控制                  |
   | RATE_200_HZ    | 高动态姿态解算                |
   | RATE_250_HZ    | 串口通信协议处理              |
   | RATE_500_HZ    | 实时避障检测                  |
   | RATE_1000_HZ   | 高速PWM信号生成               |
   +----------------+-------------------------------+ */
#define RATE_1_HZ		  1    // 1Hz定时任务（周期：1000ms）
#define RATE_5_HZ		  5    // 5Hz定时任务（周期：200ms）  
#define RATE_10_HZ		10   // 10Hz定时任务（周期：100ms）
#define RATE_20_HZ		20   // 20Hz定时任务（周期：50ms）
#define RATE_25_HZ		25   // 25Hz定时任务（周期：40ms）
#define RATE_50_HZ		50   // 50Hz定时任务（周期：20ms）
#define RATE_100_HZ		100  // 100Hz定时任务（周期：10ms）
#define RATE_200_HZ 	200  // 200Hz定时任务（周期：5ms）
#define RATE_250_HZ 	250  // 250Hz定时任务（周期：4ms）
#define RATE_500_HZ 	500  // 500Hz定时任务（周期：2ms）
#define RATE_1000_HZ 	1000 // 1000Hz定时任务（周期：1ms）


#endif 

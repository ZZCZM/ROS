/**
  ******************************************************************************
  * @file    DataScope_DP.h
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Header File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"


/* 运动控制核心模块头文件 - 定义控制算法参数及执行器接口 */

//-------------------------- 控制参数宏定义 --------------------------//
#define PI 3.14159265       // 圆周率常量（浮点精度）
#define ZHONGZHI 0          // 机械零点偏移量（单位：脉冲数/角度值）
#define DIFFERENCE 100       // 控制死区阈值（防止微小误差引起的震荡）

//----------------------- 全局控制量声明 ----------------------//
extern int Balance_Pwm;     // 平衡环PWM输出量（单位：PWM占空比）
extern int Velocity_Pwm;    // 速度环PWM输出量
extern int Turn_Pwm;        // 转向环PWM输出量

/* 中断服务函数声明 */
/**
 * @brief EXTI15_10中断处理函数（急停/碰撞检测）
 * @return 中断处理状态码（0-正常处理）
 * @note 处理GPIO_Pin_10到Pin_15的中断事件
 */
int EXTI15_10_IRQHandler(void);

//----------------------- 执行器控制接口 ----------------------//
/**
 * @brief 双电机+单舵机PWM设置
 * @param motor_a 左驱动电机PWM（-1000~+1000对应正反转）
 * @param motor_b 右驱动电机PWM 
 * @param servo   舵机控制PWM（500-2500μs脉宽范围）
 */
void Set_Pwm(int motor_a, int motor_b, int servo);

//----------------------- 运动学解析算法 ----------------------//
/**
 * @brief 基础运动学分析（速度-角度合成）
 * @param Vy 前进方向速度（cm/s）
 * @param angle 转向角度（弧度制）
 */
void Kinematic_Analysis(float Vy, float angle);

/**
 * @brief 增强运动学分析（双自由度速度合成）
 * @param Vy 前进方向速度（cm/s）
 * @param Vz 旋转角速度（rad/s）
 */
void Kinematic_Analysis2(float Vy, float Vz);

//----------------------- 输入设备接口 ----------------------//
/* 实体按键检测（含长按/短按识别） */
void Key(void);

//----------------------- 执行器安全限制 ----------------------//
/**
 * @brief PWM全局幅值限制（防止过压）
 * @param amplitude 最大允许绝对值（建议值：8000）
 */
void Xianfu_Pwm(int amplitude);

/**
 * @brief 四通道独立速度限制
 * @param amplitude_A 通道A最大速度（编码器脉冲/周期）
 * @param amplitude_B 通道B最大速度
 * @param amplitude_C 通道C最大速度
 * @param amplitude_D 通道D最大速度
 */
void Xianfu_Velocity(int amplitude_A, int amplitude_B, int amplitude_C, int amplitude_D);

//----------------------- 系统安全功能 ----------------------//
/**
 * @brief 低压自动关机检测
 * @param voltage 当前电池电压（单位：mV）
 * @return 状态码（0-正常 1-执行关机）
 */
u8 Turn_Off(int voltage);

/* 绝对值计算（适配32位系统） */
u32 myabs(long int a);

//----------------------- 增量式PID控制器 ----------------------//
/**
 * @brief 通道A增量式PI控制器（整型计算版本）
 * @param Encoder 编码器反馈（脉冲数）
 * @param Target 目标控制量
 * @return 控制输出（PWM占空比）
 */
int Incremental_PI_A(int Encoder, int Target);

// 通道B增量式PI控制器（结构同A）
int Incremental_PI_B(int Encoder, int Target);

// 通道C增量式PI控制器（结构同A） 
int Incremental_PI_C(int Encoder, int Target);

// 通道D增量式PI控制器（结构同A）
int Incremental_PI_D(int Encoder, int Target);

//----------------------- 位置式PID控制器 ----------------------//
/**
 * @brief 通道A位置式PID控制器（带积分分离）
 * @param Encoder 当前位置反馈
 * @param Target 目标位置
 * @return 控制输出（单位与执行器匹配）
 */
int Position_PID_A(int Encoder, int Target);

// 通道B位置式PID控制器（参数同A）
int Position_PID_B(int Encoder, int Target);

// 通道C位置式PID控制器（参数同A）  
int Position_PID_C(int Encoder, int Target);

// 通道D位置式PID控制器（参数同A）
int Position_PID_D(int Encoder, int Target);

//----------------------- 通信控制系统 ----------------------//
/**
 * @brief 遥控信号采集接口
 * @param mode 接收模式（0-PPM模式 1-SBUS模式）
 */
void Get_RC(u8 mode);

/**
 * @brief 速度观测器（通过编码器脉冲计算实际速度）
 */
void Count_Velocity(void);

/**
 * @brief CAN总线与串口协同控制协议
 * @note 实现CAN总线优先级仲裁与串口监控功能
 */
void CAN_N_Usart_Control(void);

#endif

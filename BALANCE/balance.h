/**
  ******************************************************************************
  * @file   balance.h
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
#ifndef __BALANCE_H
#define __BALANCE_H			  	 
#include "sys.h"
#include "system.h"
/* 平衡控制模块头文件 - 定义平衡任务参数、运动学参数及控制接口 */

/*----------------------------------------------------------------------------*
 * 【模块功能说明】                                                            *
 * 本模块定义平衡控制任务参数、全向轮底盘运动学参数                           *
 * 声明电机控制相关函数及PID控制器接口                                        *
 *----------------------------------------------------------------------------*/

/* 平衡控制任务参数 */
#define BALANCE_TASK_PRIO		5     // 任务优先级（FreeRTOS优先级，0-31，数值越大优先级越高）
#define BALANCE_STK_SIZE 		512   // 任务堆栈大小（单位：字，FreeRTOS堆栈计量单位）

/* 全向轮运动学参数（麦克纳姆轮/全向轮底盘运动学模型系数） */
#define X_PARAMETER    (sqrt(3)/2.f)   // X轴方向速度分量系数（对应cos(30°)）               
#define Y_PARAMETER    (0.5f)          // Y轴方向速度分量系数（对应sin(30°)）    
#define L_PARAMETER    (1.0f)          // 轮组安装半径（单位：米，影响旋转运动转换）

/* 系统状态变量声明 */
extern short test_num;               // 测试模式编号（用于调试和功能测试）
extern u8 command_lost_count;        // 指令丢失计数器（检测通信异常）
extern int robot_mode_check_flag;    // 机器人模式标志位（标识当前工作模式）

/**
 * @brief 平衡控制主任务函数
 * @param pvParameters FreeRTOS任务参数指针
 */
void Balance_task(void *pvParameters);

/**
 * @brief 设置四电机四舵机PWM输出
 * @param motor_a 电机A PWM值（范围取决于具体电机控制器）
 * @param motor_b 电机B PWM值
 * @param motor_c 电机C PWM值
 * @param motor_d 电机D PWM值
 * @param servo1 舵机1 PWM值（通常500-2500对应0-180度）
 * @param servo2 舵机2 PWM值
 * @param servo3 舵机3 PWM值
 * @param servo4 舵机4 PWM值
 */
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d,int servo1,int servo2,int servo3,int servo4);

/**
 * @brief 限制PWM输出幅值（防止过载）
 * @param amplitude 最大允许幅值（绝对值限制）
 */
void Limit_Pwm(int amplitude);

/**
 * @brief 浮点数限幅函数
 * @param insert 输入值
 * @param low 下限值
 * @param high 上限值
 * @return 限幅后的值
 */
float target_limit_float(float insert,float low,float high);

/**
 * @brief 整型数限幅函数
 * @param insert 输入值
 * @param low 下限值
 * @param high 上限值
 * @return 限幅后的值
 */
int target_limit_int(int insert,int low,int high);

/**
 * @brief 电压检测关机函数
 * @param voltage 当前电压值（单位：mV）
 * @return 状态码：0-正常 1-欠压关机
 */
u8 Turn_Off(int voltage);

/**
 * @brief 绝对值计算函数（32位有符号）
 * @param a 输入值
 * @return 绝对值结果
 */
u32 myabs(long int a);

/* 增量式PI控制器（四个独立通道） */
/**
 * @brief 电机A增量式PI控制器
 * @param Encoder 编码器反馈值
 * @param Target 目标设定值
 * @return PI控制输出量
 */
int Incremental_PI_A (float Encoder,float Target);

// 电机B增量式PI控制器（参数同A）
int Incremental_PI_B (float Encoder,float Target);

// 电机C增量式PI控制器（参数同A）  
int Incremental_PI_C (float Encoder,float Target);

// 电机D增量式PI控制器（参数同A）
int Incremental_PI_D (float Encoder,float Target);

/* 以下为位置式PID控制器声明（具体实现需在源文件中定义） */
float Position_PID1 (float Encoder,float Target); // 通道1位置式PID
float Position_PID2 (float Encoder,float Target); // 通道2位置式PID
float Position_PID3 (float Encoder,float Target); // 通道3位置式PID
float Position_PID4 (float Encoder,float Target); // 通道4位置式PID
float Position_PID5 (float Encoder,float Target); // 通道5位置式PID
float Position_PID6 (float Encoder,float Target); // 通道6位置式PID
/* 机器人控制子系统函数声明 */

/**
 * @brief 获取原始遥控器信号（读取接收机原始通道数据）
 * @note 需配合具体硬件接收机（如PPM/S.BUS接收机）实现
 */
void Get_RC(void);

/**
 * @brief 遥控信号处理主函数（将原始信号转换为控制指令）
 * @note 包含信号滤波、通道映射、控制模式切换等处理
 */
void Remote_Control(void);

/**
 * @brief 三自由度运动合成函数（将速度矢量转换为电机转速）
 * @param Vx X轴平移速度（单位：cm/s）
 * @param Vy Y轴平移速度（单位：cm/s）
 * @param Vz Z轴旋转速度（单位：deg/s）
 * @note 基于运动学模型将速度矢量分解到各电机
 */
void Drive_Motor(float Vx, float Vy, float Vz);

/**
 * @brief 实体按键状态检测（机械按键消抖处理）
 * @note 包含短按/长按状态识别，典型采样周期10ms
 */
void Key(void);

/**
 * @brief 编码器速度解算（通过M法测速计算实时转速）
 * @note 使用定时器捕获模式获取编码器脉冲计数
 */
void Get_Velocity_Form_Encoder(void);

/**
 * @brief 运动平滑滤波器（速度斜坡处理）
 * @param vx X轴目标速度（输入/输出）
 * @param vy Y轴目标速度（输入/输出）
 * @param vz Z轴目标角速度（输入/输出）
 * @note 采用梯形加减速算法防止速度突变
 */
void Smooth_control(float vx, float vy, float vz);

/**
 * @brief PS2手柄协议解析（处理手柄按键和摇杆数据）
 * @note 支持模式：红色模式/绿色模式，含振动反馈控制
 */
void PS2_control(void);

/**
 * @brief 浮点绝对值计算（特化快速算法）
 * @param insert 输入浮点数
 * @return 绝对值结果
 */
float float_abs(float insert);

/**
 * @brief 机器人模式状态机检查（处理模式切换逻辑）
 * @note 状态包括：急停模式/遥控模式/自主模式
 */
void robot_mode_check(void);

/**
 * @brief 舵机角度-PWM转换函数（0-180度对应标准PWM）
 * @param angle 目标角度（单位：度）
 * @return PWM脉宽值（典型范围500-2500μs）
 */
int SERVO_PWM_VALUE(float angle);

/* 机械臂运动安全限制 */
/**
 * @brief 关节角度软限位（防止机械结构碰撞）
 */
void moveit_angle_limit(void);

/**
 * @brief 执行器PWM硬限幅（保护驱动器硬件）
 */
void moveit_pwm_limit(void);

/**
 * @brief 机械臂运动控制主函数（逆运动学解算）
 * @note 包含末端轨迹规划和关节空间插补
 */
void Drive_Robot_Arm(void);

#endif  


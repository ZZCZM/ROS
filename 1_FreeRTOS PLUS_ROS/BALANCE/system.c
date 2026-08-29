/**
  ******************************************************************************
  * @file    system.c
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Source Code File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#include "system.h"

//Robot software fails to flag bits
//机器人软件失能标志位
u8 Flag_Stop=1;   

//The ADC value is variable in segments, depending on the number of car models. Currently there are 6 car models
//ADC值分段变量，取决于小车型号数量，目前有6种小车型号
int Divisor_Mode;

// Robot type variable
//机器人型号变量
//0=Mec_Car，1=Omni_Car，2=Akm_Car，3=Diff_Car，4=FourWheel_Car，5=Tank_Car
u8 Car_Mode=0; 

//Servo control PWM value, Ackerman car special
//舵机控制PWM值，阿克曼小车专用
int Servo;  

//Default speed of remote control car, unit: mm/s
//遥控小车的默认速度，单位：mm/s
float RC_Velocity=500;


//APP遥控时切换 底盘/舵机 的标志位
u8 car_A_steer_flag=1;

//Vehicle three-axis target moving speed, unit: m/s
//小车三轴目标运动速度，单位：m/s
float Move_X, Move_Y, Move_Z;   

//PID parameters of Speed control
//速度控制PID参数
float Velocity_KP=300,Velocity_KI=300; 

//Smooth control of intermediate variables, dedicated to omni-directional moving cars
//平滑控制中间变量，全向移动小车专用
Smooth_Control smooth_control;  

//The parameter structure of the motor
//电机的参数结构体
Motor_parameter MOTOR_A,MOTOR_B,MOTOR_C,MOTOR_D;  

/************ 小车型号相关变量 **************************/
/************ Variables related to car model ************/
//Encoder accuracy
//编码器精度
float Encoder_precision; 
//Wheel circumference, unit: m
//轮子周长，单位：m
float Wheel_perimeter; 
//Drive wheel base, unit: m
//主动轮轮距，单位：m
float Wheel_spacing; 
//The wheelbase of the front and rear axles of the trolley, unit: m
//小车前后轴的轴距，单位：m
float Axle_spacing; 
//All-directional wheel turning radius, unit: m
//全向轮转弯半径，单位：m
float Omni_turn_radiaus; 
/************ 小车型号相关变量 **************************/
/************ Variables related to car model ************/

//PS2 controller, Bluetooth APP, aircraft model controller, CAN communication, serial port 1, serial port 5 communication control flag bit.
//These 6 flag bits are all 0 by default, representing the serial port 3 control mode
//PS2手柄、蓝牙APP、航模手柄、CAN通信、串口1、串口5通信控制标志位。这6个标志位默认都为0，代表串口3控制模式
u8 PS2_ON_Flag=0, APP_ON_Flag=0, Remote_ON_Flag=0, CAN_ON_Flag=0, Usart1_ON_Flag, Usart5_ON_Flag; 

//四自由度机械臂的目标角度
float Moveit_Angle1=0,Moveit_Angle2=-1.57,Moveit_Angle3=1.57,Moveit_Angle4=0;
//四自由度机械臂的目标PWM值
int  Moveit_PWM1,Moveit_PWM2,Moveit_PWM3,Moveit_PWM4;

//Bluetooth remote control associated flag bits
//蓝牙遥控相关的标志位
u8 Flag_Left, Flag_Right, Flag_Direction=0, Turn_Flag; 

//Sends the parameter's flag bit to the Bluetooth APP
//向蓝牙APP发送参数的标志位
u8 PID_Send; 

//The PS2 gamepad controls related variables
//PS2手柄控制相关变量
int PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY; 

//Self-check the relevant flag variables
//自检相关标志变量
int Check=0, Checking=0, Checked=0, CheckCount=0, CheckPhrase1=0, CheckPhrase2=0; 

//Check the result code
//自检结果代码
long int ErrorCode=0; 

/**
 * @brief 初始化系统硬件和软件模块
 * @note 执行流程：
 * 1. 中断优先级分组设置
 * 2. 延时函数初始化
 * 3. 配置RTC
 * 4. 初始化LED、蜂鸣器、按键、ADC、CAN通信、LCD、MPU6050、编码器
 * 5. 初始化电机PWM控制
 */
void systemInit(void)
{
    /*----- 中断优先级分组设置 -----*/
    // 设置中断优先级分组为4，确保中断优先级的精细控制
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    /*----- 延时函数初始化 -----*/
    // 初始化延时函数，参数168对应系统时钟频率（假设168MHz）
    delay_init(168);
    
    /*----- RTC 配置 -----*/
    // 配置实时时钟
    RTC_Config();
    
    /*----- LED 初始化 -----*/
    // 初始化与LED灯连接的硬件接口
    LED_Init();
    
    /*----- 蜂鸣器初始化 -----*/
    // 初始化与蜂鸣器连接的硬件接口
    Buzzer_Init();
    
    /*----- 用户按键初始化 -----*/
    // 初始化与用户按键连接的硬件接口
    KEY_Init();
    
    /*----- 串口初始化 -----*/
    // 初始化串口3，通信波特率115200，用于与ROS端通信
    uart3_init(115200);
    USART_DMA_Config(); // 使用DMA进行串口通信
    
    /*----- ADC1 初始化 -----*/
    // 初始化ADC1引脚，用于读取电位器档位
    Adc_Init();

   /*----- ADC2 初始化 -----*/
    // 初始化ADC2引脚，用于读取电池电压
    Adc_POWER_Init();
    
    /*----- CAN 通信接口初始化 -----*/
    // 初始化CAN通信接口
    CAN_Mode_Init(1, 7, 6, 3, 0);
    
    /*----- LCD 初始化 -----*/
    // 初始化LCD屏幕
    ILI9806G_Init();
    
    /*----- 机器人型号选择 -----*/
    // 根据电位器的档位判断需要适配的是哪一种型号的小车，然后进行对应的参数初始化
    Robot_Select();
    
    /*----- 编码器初始化 -----*/
    // 初始化编码器A，用于读取电机A的实时速度
    Encoder_Init_TIM2();
    
    // 初始化编码器B，用于读取电机B的实时速度
    Encoder_Init_TIM3();
    
    // 初始化编码器C，用于读取电机C的实时速度
    Encoder_Init_TIM4();
    
    // 初始化编码器D，用于读取电机D的实时速度
    Encoder_Init_TIM5();
    
    /*----- 读取Flash中的舵机校准数据 -----*/
    // 读取Flash中存储的舵机上电微调位置
    Flash_Read();
    
    /*----- 舵机接口初始化 -----*/
    // 初始化舵机接口，使用TIM8高级定时器，时钟频率为100Hz
    TIM8_SERVO_Init(9999, 168-1);
    
    /*----- 电机PWM初始化 -----*/
    // 初始化电机速度控制，使用PWM频率为10kHz
    // 电机A PWM初始化
    TIM10_PWM_Init(16799, 0);
    TIM11_PWM_Init(16799, 0);
    
    // 电机B PWM初始化
    TIM9_PWM_Init(16799, 0);
    
    // 电机C和D PWM初始化
    TIM1_PWM_Init(16799, 0);
    
    /*----- IIC 初始化 -----*/
    // 初始化IIC总线，用于MPU6050通信
    I2C_GPIOInit();
		
		//初始化资源映射系统
		vInitializeResourceMapping();
    
    /*----- MPU6050 初始化 -----*/
    // 初始化MPU6050，用于读取车辆的三轴姿态、三轴角速度和三轴加速度信息
    MPU6050_initialize();
}



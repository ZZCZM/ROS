/**
  ******************************************************************************
  * @file   robot_select_init.h
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
#ifndef __ROBOTSELECTINIT_H
#define __ROBOTSELECTINIT_H
#include "sys.h"
#include "system.h"

//Parameter structure of robot
//机器人参数结构体
typedef struct  
{
  float WheelSpacing;      //Wheelspacing, Mec_Car is half wheelspacing //轮距 麦轮车为半轮距
  float AxleSpacing;       //Axlespacing, Mec_Car is half axlespacing //轴距 麦轮车为半轴距	
  int GearRatio;           //Motor_gear_ratio //电机减速比
  int EncoderAccuracy;     //Number_of_encoder_lines //编码器精度(编码器线数)
  float WheelDiameter;     //Diameter of driving wheel //主动轮直径	
  float OmniTurnRadiaus;   //Rotation radius of omnidirectional trolley //全向轮小车旋转半径	
}Robot_Parament_InitTypeDef;

// Encoder structure
//编码器结构体
typedef struct  
{
  int A;      
  int B; 
	int C; 
	int D; 
}Encoder;

//The minimum turning radius of Ackermann models is determined by the mechanical structure: 
//the maximum Angle of the wheelbase, wheelbase and front wheels
//阿克曼车型的最小转弯半径，由机械结构决定：轮距、轴距、前轮最大转角
#define MINI_AKM_MIN_TURN_RADIUS 0.350f 

//Wheelspacing, Mec_Car is half wheelspacing
//轮距 麦轮是一半
//#define MEC_wheelspacing         0.109
#define MEC_wheelspacing         0.0930 //修正2021.03.30
#define Akm_wheelspacing         0.162f
#define Diff_wheelSpacing        0.177f
#define Four_Mortor_wheelSpacing 0.187f
#define Tank_wheelSpacing        0.235f

//Axlespacing, Mec_Car is half axlespacing
//轴距 麦轮是一半
#define MEC_axlespacing           0.085
#define Akm_axlespacing           0.144f
#define Diff_axlespacing          0.155f
#define Four_Mortor__axlespacing  0.173f
#define Tank_axlespacing          0.222f

//Motor_gear_ratio
//电机减速比
#define   HALL_30F    30
#define   HALL_60F    60
#define   MD36N_5_18  5.18
#define   MD36N_27    27
#define   MD36N_51    51
#define   MD36N_71    71
#define   MD60N_18    18
#define   MD60N_47    47

//Number_of_encoder_lines
//编码器精度
#define		Photoelectric_500 500
#define	  Hall_13           13

//Mecanum wheel tire diameter series
//麦轮轮胎直径
#define		Mecanum_60  0.060f
#define		Mecanum_75  0.075f
#define		Mecanum_100 0.100f
#define		Mecanum_127 0.127f
#define		Mecanum_152 0.152f
 
//Omni wheel tire diameter series
//轮径全向轮直径系列
#define	  FullDirecion_60  0.060
#define	  FullDirecion_75  0.075
#define	  FullDirecion_127 0.127
#define	  FullDirecion_152 0.152
#define	  FullDirecion_203 0.203
#define	  FullDirecion_217 0.217

//Black tire, tank_car wheel diameter
//黑色轮胎、履带车轮直径
#define	  Black_WheelDiameter   0.065
//#define	  Tank_WheelDiameter 0.047
#define	  Tank_WheelDiameter 0.043

//Rotation radius of omnidirectional trolley
//全向轮小车旋转半径
#define   Omni_Turn_Radiaus_109 0.109
#define   Omni_Turn_Radiaus_164 0.164
#define   Omni_Turn_Radiaus_180 0.180
#define   Omni_Turn_Radiaus_290 0.290

//The encoder octave depends on the encoder initialization Settings
//编码器倍频数，取决于编码器初始化设置
#define   EncoderMultiples  4
//Encoder data reading frequency
//编码器数据读取频率
#define   CONTROL_FREQUENCY 100

//#define PI 3.1415f  //PI //圆周率
/* 机器人配置管理接口 - 负责机械结构选择与参数初始化 */

/**
 * @brief 机器人类型选择函数（需配合硬件配置调用）
 * @note 功能说明：
 * - 选择机器人底层运动学模型（如全向轮/差速轮/阿克曼转向）
 * - 初始化对应类型的控制参数表
 * - 配置运动学解算器算法版本
 * 调用时机：系统启动时或运行中切换构型前
 */
void Robot_Select(void);

/**
 * @brief 机器人机械参数初始化（构建运动学模型）
 * @param wheelspacing 驱动轮间距（单位：米，double精度确保大尺寸机器人计算精度）
 * @param axlespacing 轴间距（单位：米，前后轮轴距离）
 * @param omni_turn_radiaus 全向模式转向半径（单位：米，建议值≥0.5）
 * @param gearratio 传动比（电机转：轮转，如19.2:1应输入19.2）
 * @param Accuracy 控制精度阈值（单位：米，PID停止调节阈值）
 * @param tyre_diameter 轮胎有效直径（单位：米，含充气变形量）
 * @note 参数关系：
 * 实际线速度 = (电机RPM * tyre_diameter * π) / (gearratio * 60)
 * 最小转弯半径 = wheelspacing / (2 * tan(最大转向角)) 
 */
void Robot_Init(double wheelspacing, float axlespacing, float omni_turn_radiaus, 
               float gearratio, float Accuracy, float tyre_diameter);

#endif

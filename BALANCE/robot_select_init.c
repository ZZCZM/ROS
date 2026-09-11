/**
  ******************************************************************************
  * @file    robot_select_init.c
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
#include "robot_select_init.h"
// 全局机器人参数结构体声明
Robot_Parament_InitTypeDef  Robot_Parament;  // 包含轮距、轴距等关键参数

/**************************************************************************
【函数名称】Robot_Select
【功能概述】车型选择及参数初始化主控函数
            - 通过电位器ADC值选择当前车型
            - 根据车型初始化运动学参数
            - 设置自检流程阶段参数
【硬件依赖】电位器模拟输入（需接ADC通道）
【车型范围】0-2对应三种车型（麦克纳姆轮/四驱车/履带车）
**************************************************************************/
void Robot_Select(void)
{
    /* 电位器ADC值分段处理 */
    // 计算ADC分段阈值（2048对应12位ADC满量程4096的一半）
    Divisor_Mode = 2048/CAR_NUMBER + 2;  // CAR_NUMBER=6时得344
    
    // 获取电位器ADC值并计算车型编号（0-2）
    // 10次采样取平均增强稳定性
    Car_Mode = (int)(Get_adc_Average(Potentiometer,10) / Divisor_Mode);  
    
    // 车型编号安全限制（当前最大支持3种车型）
    if(Car_Mode > 2) Car_Mode = 2;  // 强制限定在0-2范围内

    /* 车型参数初始化 */
    switch(Car_Mode) {
        case Mec_Car:  // 麦克纳姆轮全向车型
            Robot_Init(MEC_wheelspacing,         // 轮距（半轮距值）
                       MEC_axlespacing,          // 轴距（半轴距值）
                       0,                        // 全向轮旋转半径（未使用）
                       HALL_60F,                 // 霍尔编码器60线
                       Photoelectric_500,        // 光电编码500线
                       Mecanum_75);             // 麦克纳姆轮直径75mm
            break;
        
        case FourWheel_Car:  // 四驱差速车型
            Robot_Init(Four_Mortor_wheelSpacing, // 四驱车轮距
                       Four_Mortor__axlespacing, // 四驱车轴距
                       0,                        // 未使用参数
                       HALL_60F,                 // 霍尔编码器
                       Photoelectric_500,        // 光电编码
                       Black_WheelDiameter);     // 标准轮胎直径
            break;
        
        case Tank_Car:  // 履带车型
            Robot_Init(Tank_wheelSpacing,        // 履带轮间距
                       0,                        // 轴距（履带车无轴距概念）
                       0,                       
                       HALL_60F,                 
                       Photoelectric_500,        
                       Tank_WheelDiameter);      // 履带驱动轮直径
            break;
    }

    /* 自检流程阶段参数配置 */
    switch(Car_Mode) {
        case Mec_Car:       // 全向轮车型需要更多检测步骤
            CheckPhrase1 = 8;   // 第一阶段检测项数
            CheckPhrase2 = 14;  // 第二阶段检测项数
            break;
        case FourWheel_Car: // 四驱车中等复杂度检测
            CheckPhrase1 = 8;
            CheckPhrase2 = 11;
            break;
        case Tank_Car:      // 履带车检测相对简单
            CheckPhrase1 = 4;
            CheckPhrase2 = 7;
            break;
    }
}

/**************************************************************************
【函数名称】Robot_Init
【功能概述】机器人运动学参数初始化
            - 存储机械结构参数
            - 计算运动控制关键参数
【核心参数】wheelspacing: 轮距（麦轮车为半轮距）
           axlespacing: 轴距（麦轮车为半轴距）
           tyre_diameter: 驱动轮直径（毫米）
**************************************************************************/
void Robot_Init(double wheelspacing,   // 轮距/半轮距(mm)
                float axlespacing,    // 轴距/半轴距(mm)
                float omni_turn_radiaus, // 全向轮旋转半径(已弃用)
                float gearratio,      // 电机减速比
                float Accuracy,        // 编码器精度(线数)
                float tyre_diameter)   // 轮胎直径(mm)
{
    /* 机械结构参数存储 */
    Robot_Parament.WheelSpacing = wheelspacing;    // 轮距参数
    Robot_Parament.AxleSpacing = axlespacing;      // 轴距参数
    Robot_Parament.OmniTurnRadiaus = omni_turn_radiaus; // 保留参数
    Robot_Parament.GearRatio = gearratio;          // 减速比(如30:1=30)
    Robot_Parament.EncoderAccuracy = Accuracy;     // 编码器线数(PPR)
    Robot_Parament.WheelDiameter = tyre_diameter;  // 轮胎直径(mm)

    /* 运动控制关键参数计算 */
    // 编码器总精度 = 编码器倍数×线数×减速比（例：4×13线×30=1560脉冲/转）
    Encoder_precision = EncoderMultiples * Robot_Parament.EncoderAccuracy * Robot_Parament.GearRatio;
    
    // 轮胎周长 = π×直径 （转换为米单位需要/1000）
    Wheel_perimeter = Robot_Parament.WheelDiameter * PI;  // 单位：mm
    
    // 全局变量赋值（供其他模块调用）
    Wheel_spacing = Robot_Parament.WheelSpacing;  // 轮距全局访问
    Axle_spacing = Robot_Parament.AxleSpacing;    // 轴距全局访问
    Omni_turn_radiaus = Robot_Parament.OmniTurnRadiaus; // 保留参数
}

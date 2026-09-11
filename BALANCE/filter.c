#include "filter.h"
/**
  ******************************************************************************
  * @file    filter.c
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
/* 卡尔曼滤波器核心参数配置及状态变量定义 */

// 互补滤波器增益系数（一阶滤波使用）
float K1 = 0.02;  // 加速度计权重系数（典型值范围0.95-0.99）

/* 滤波器输出量（与filter.h中声明对应）*/
float angle;      // 最优估计角度（单位：弧度）
float angle_dot;  // 最优估计角速度（单位：弧度/秒）

/* 卡尔曼过程噪声参数 */
float Q_angle = 0.001;  // 角度过程噪声协方差（影响状态预测置信度）
float Q_gyro = 0.003;   // 陀螺仪漂移过程噪声协方差（控制偏差估计变化率）
float R_angle = 0.5;    // 测量噪声协方差（反映加速度计测量可靠性）

/* 时间相关参数 */
float dt = 0.005;       // 采样周期（单位：秒，对应200Hz控制频率）

/* 观测矩阵参数 */
char C_0 = 1;           // 观测矩阵C[0]元素（状态量到观测量的转换系数）

/* 滤波器状态量 */
float Q_bias;           // 陀螺仪零偏估计值（单位：弧度/秒）
float Angle_err;        // 角度观测残差（加速度计测量与预测的差值）

/* 协方差矩阵计算中间量 */
float PCt_0, PCt_1;     // P*C^T 矩阵运算中间结果
float E;                // 新息协方差（S = C*P*C^T + R）

/* 卡尔曼增益计算变量 */
float K_0, K_1;         // 卡尔曼增益矩阵K[0]和K[1]
float t_0, t_1;         // 临时计算存储变量

/* 协方差矩阵微分计算 */
float Pdot[4] = {0};    // 协方差矩阵微分dP/dt（按行展开存储）

/* 协方差矩阵初始化 */
float PP[2][2] = { 
    { 1, 0 },          // 初始角度方差（反映初始状态不确定性）
    { 0, 1 }           // 初始角速度偏差方差
};                     // 单位矩阵表示初始最大不确定度

/* 卡尔曼滤波器实现（完成状态预测与更新全过程）*/
/**
 * @brief 卡尔曼滤波器（加速度计+陀螺仪数据融合）
 * @param Accel 加速度计解算角度（单位：弧度）
 * @param Gyro 陀螺仪角速度读数（单位：弧度/秒）
 * @note 采用经典卡尔曼滤波框架，状态变量为[角度, 陀螺零偏]
 */
void Kalman_Filter(float Accel, float Gyro)		
{
    //------------------------ 状态预测阶段 ------------------------//
    /* 角度先验估计：基于陀螺仪积分（补偿零偏） */
    angle += (Gyro - Q_bias) * dt;  // xk_ = F * xk_prev
    
    /* 协方差矩阵微分计算（对应公式：Pdot = F*P + P*F^T + Q） */
    Pdot[0] = Q_angle - PP[0][1] - PP[1][0];  // 第1行第1列微分项
    Pdot[1] = -PP[1][1];                      // 第1行第2列
    Pdot[2] = -PP[1][1];                      // 第2行第1列（对称矩阵）
    Pdot[3] = Q_gyro;                         // 第2行第2列
    
    /* 协方差矩阵积分更新（数值积分法） */
    PP[0][0] += Pdot[0] * dt;  // 积分更新P[0][0]
    PP[0][1] += Pdot[1] * dt;  // 积分更新P[0][1]
    PP[1][0] += Pdot[2] * dt;  // 积分更新P[1][0]
    PP[1][1] += Pdot[3] * dt;  // 积分更新P[1][1]

    //------------------------ 测量更新阶段 ------------------------//
    /* 计算新息（观测残差） */
    Angle_err = Accel - angle;  // zk - H*xk_
    
    /* 计算卡尔曼增益中间量（PCt = P*C^T） */
    PCt_0 = C_0 * PP[0][0];  // 矩阵运算元素(0,0)
    PCt_1 = C_0 * PP[1][0];  // 矩阵运算元素(1,0)
    
    /* 新息协方差计算（S = H*P*H^T + R） */
    E = R_angle + C_0 * PCt_0;  // 标量形式简化计算
    
    /* 卡尔曼增益计算（K = PCt * S^-1） */
    K_0 = PCt_0 / E;  // 卡尔曼增益矩阵元素K[0]
    K_1 = PCt_1 / E;  // 卡尔曼增益矩阵元素K[1]
    
    /* 协方差矩阵更新（P = (I - K*H)*P） */
    t_0 = PCt_0;            // 临时存储用于矩阵运算
    t_1 = C_0 * PP[0][1];   // 计算H*P元素(0,1)
    PP[0][0] -= K_0 * t_0;  // 更新P[0][0]
    PP[0][1] -= K_0 * t_1;  // 更新P[0][1]
    PP[1][0] -= K_1 * t_0;  // 更新P[1][0]
    PP[1][1] -= K_1 * t_1;  // 更新P[1][1]
    
    /* 状态量后验估计 */
    angle   += K_0 * Angle_err;  // 更新最优角度估计
    Q_bias += K_1 * Angle_err;   // 更新陀螺零偏估计
    
    /* 输出角速度（微分最优角度估计） */
    angle_dot = Gyro - Q_bias;  // ω = gyro - bias

}

/* 一阶互补滤波器实现 */
/**
 * @brief 一阶互补滤波器（加速度计与陀螺仪融合）
 * @param angle_m 加速度计角度（单位：弧度）
 * @param gyro_m 陀螺仪角速度（单位：弧度/秒）
 * @note 实现公式：angle = K1*angle_m + (1-K1)*(angle_prev + gyro*dt)
 */
void Yijielvbo(float angle_m, float gyro_m)
{
    // 陀螺仪积分项（角度预测）
    float gyro_integral = angle + gyro_m * 0.005f; 
    
    // 互补融合（加速度计与陀螺仪加权平均）
    angle = K1 * angle_m + (1 - K1) * gyro_integral;
}

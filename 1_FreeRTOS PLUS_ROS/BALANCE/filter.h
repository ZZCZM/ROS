/**
  ******************************************************************************
  * @file    filter.h
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
#ifndef __FILTER_H
#define __FILTER_H
#include "system.h"

/* 传感器滤波处理模块头文件 - 定义姿态解算算法接口 */

/*----------------------------------------------------------------------------*
 * 【模块功能说明】                                                            *
 * 本模块实现传感器数据融合算法，提供稳定可靠的角度解算结果                   *
 * 包含卡尔曼滤波及一阶滤波两种算法实现                                        *
 *----------------------------------------------------------------------------*/

/*--------------------- 滤波处理结果全局变量 ---------------------*/
extern float angle;      // 融合后俯仰角（单位：弧度，-π~+π）
extern float angle_dot;  // 角速度（单位：弧度/秒，动态更新）

/**
 * @brief 卡尔曼滤波器（加速度计+陀螺仪数据融合）
 * @param Accel 加速度计角度计算值（单位：弧度，来自反正切计算）
 * @param Gyro  陀螺仪角速度值（单位：弧度/秒，需乘积分时间）
 * @note 采用经典卡尔曼滤波框架，Q=0.001, R=0.5
 *       状态量：角度值、角速度偏差
 *       实现陀螺仪动态跟踪与加速度计静态校正
 */
void Kalman_Filter(float Accel, float Gyro);

/**
 * @brief 一阶互补滤波器（快速融合算法）
 * @param angle_m 加速度计原始角度（单位：弧度）
 * @param gyro_m  陀螺仪原始角速度（单位：弧度/秒）
 * @note 滤波公式：angle = K*(angle + gyro*dt) + (1-K)*angle_m
 *       典型参数：K=0.98，采样周期dt=0.005s
 *       适用于计算资源有限的实时系统
 */
void Yijielvbo(float angle_m, float gyro_m);

#endif


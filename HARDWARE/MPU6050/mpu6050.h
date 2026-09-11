/**
  ******************************************************************************
  * @file   mpu6050.h
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
#ifndef __MPU6050_H
#define __MPU6050_H

#include "sys.h"
/*--------------------- MPU6050 传感器宏定义 ---------------------*/

/*--------------------- 设备地址定义 ---------------------*/
#define devAddr  0xD0  // 默认设备地址（低7位+读写位），未使用

#define MPU6050_ADDRESS_AD0_LOW     0x68  // 地址引脚AD0低电平时的设备地址（低7位）
#define MPU6050_ADDRESS_AD0_HIGH    0x69  // 地址引脚AD0高电平时的设备地址（低7位）
#define MPU6050_DEFAULT_ADDRESS     MPU6050_ADDRESS_AD0_LOW  // 默认地址（AD0低电平）

/*--------------------- 寄存器地址定义 ---------------------*/
/**
 * @brief X轴陀螺仪偏移校准寄存器（高字节）
 * @note 包含位[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
 */
#define MPU6050_RA_XG_OFFS_TC       0x00

/**
 * @brief Y轴陀螺仪偏移校准寄存器（高字节）
 * @note 包含位[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
 */
#define MPU6050_RA_YG_OFFS_TC       0x01

/**
 * @brief Z轴陀螺仪偏移校准寄存器（高字节）
 * @note 包含位[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
 */
#define MPU6050_RA_ZG_OFFS_TC       0x02

/**
 * @brief X轴陀螺仪微调增益寄存器
 * @note 包含位[7:0] X_FINE_GAIN
 */
#define MPU6050_RA_X_FINE_GAIN      0x03

/**
 * @brief Y轴陀螺仪微调增益寄存器
 * @note 包含位[7:0] Y_FINE_GAIN
 */
#define MPU6050_RA_Y_FINE_GAIN      0x04

/**
 * @brief Z轴陀螺仪微调增益寄存器
 * @note 包含位[7:0] Z_FINE_GAIN
 */
#define MPU6050_RA_Z_FINE_GAIN      0x05

/**
 * @brief X轴加速度偏移校准寄存器（高字节）
 * @note 包含位[15:0] XA_OFFS
 */
#define MPU6050_RA_XA_OFFS_H        0x06

/**
 * @brief X轴加速度偏移校准寄存器（低字节）
 * @note 包含位[15:0] XA_OFFS
 */
#define MPU6050_RA_XA_OFFS_L_TC     0x07

/**
 * @brief Y轴加速度偏移校准寄存器（高字节）
 * @note 包含位[15:0] YA_OFFS
 */
#define MPU6050_RA_YA_OFFS_H        0x08

/**
 * @brief Y轴加速度偏移校准寄存器（低字节）
 * @note 包含位[15:0] YA_OFFS
 */
#define MPU6050_RA_YA_OFFS_L_TC     0x09

/**
 * @brief Z轴加速度偏移校准寄存器（高字节）
 * @note 包含位[15:0] ZA_OFFS
 */
#define MPU6050_RA_ZA_OFFS_H        0x0A

/**
 * @brief Z轴加速度偏移校准寄存器（低字节）
 * @note 包含位[15:0] ZA_OFFS
 */
#define MPU6050_RA_ZA_OFFS_L_TC     0x0B

/**
 * @brief 用户定义X轴陀螺仪偏移校准寄存器（高字节）
 * @note 包含位[15:0] XG_OFFS_USR
 */
#define MPU6050_RA_XG_OFFS_USRH     0x13

/**
 * @brief 用户定义X轴陀螺仪偏移校准寄存器（低字节）
 * @note 包含位[15:0] XG_OFFS_USR
 */
#define MPU6050_RA_XG_OFFS_USRL     0x14

/**
 * @brief 用户定义Y轴陀螺仪偏移校准寄存器（高字节）
 * @note 包含位[15:0] YG_OFFS_USR
 */
#define MPU6050_RA_YG_OFFS_USRH     0x15

/**
 * @brief 用户定义Y轴陀螺仪偏移校准寄存器（低字节）
 * @note 包含位[15:0] YG_OFFS_USR
 */
#define MPU6050_RA_YG_OFFS_USRL     0x16

/**
 * @brief 用户定义Z轴陀螺仪偏移校准寄存器（高字节）
 * @note 包含位[15:0] ZG_OFFS_USR
 */
#define MPU6050_RA_ZG_OFFS_USRH     0x17

/**
 * @brief 用户定义Z轴陀螺仪偏移校准寄存器（低字节）
 * @note 包含位[15:0] ZG_OFFS_USR
 */
#define MPU6050_RA_ZG_OFFS_USRL     0x18

/**
 * @brief 采样率除数寄存器
 * @note 设置采样率：Fs = 8MHz / (1 + SMPLRT_DIV)
 */
#define MPU6050_RA_SMPLRT_DIV       0x19

/**
 * @brief 配置寄存器
 * @note 包含位[7] EXT_SYNC_SET, [6:5] DLPF_CFG, [4:3] FS_SEL, [2] ZA_ST, [1] YA_ST, [0] XA_ST
 */
#define MPU6050_RA_CONFIG           0x1A

/**
 * @brief 陀螺仪配置寄存器
 * @note 包含位[4:3] XG_ST, [2:1] YG_ST, [0] ZG_ST
 */
#define MPU6050_RA_GYRO_CONFIG      0x1B

/**
 * @brief 加速度计配置寄存器
 * @note 包含位[4:3] XA_ST, [2:1] YA_ST, [0] ZA_ST
 */
#define MPU6050_RA_ACCEL_CONFIG     0x1C

/**
 * @brief 自由落体阈值寄存器
 * @note 包含位[7:0] FF_THR
 */
#define MPU6050_RA_FF_THR           0x1D

/**
 * @brief 自由落体时长寄存器
 * @note 包含位[7:0] FF_DUR
 */
#define MPU6050_RA_FF_DUR           0x1E

/**
 * @brief 运动检测阈值寄存器
 * @note 包含位[7:0] MOT_THR
 */
#define MPU6050_RA_MOT_THR          0x1F
/*--------------------- MPU6050 传感器寄存器地址定义 ---------------------*/

/**
 * @brief 运动检测时长寄存器
 * @note 包含位[7:0] MOT_DUR
 * @note 设置运动检测的持续时间（采样周期数）
 */
#define MPU6050_RA_MOT_DUR          0x20

/**
 * @brief 零重力运动检测阈值寄存器
 * @note 包含位[7:0] ZRMOT_THR
 * @note 设置零重力运动检测的阈值
 */
#define MPU6050_RA_ZRMOT_THR        0x21

/**
 * @brief 零重力运动检测时长寄存器
 * @note 包含位[7:0] ZRMOT_DUR
 * @note 设置零重力运动检测的持续时间（采样周期数）
 */
#define MPU6050_RA_ZRMOT_DUR        0x22

/**
 * @brief FIFO使能寄存器
 * @note 包含位[7:0] FIFO_EN
 * @note 设置哪些传感器数据通道进入FIFO缓冲区
 */
#define MPU6050_RA_FIFO_EN          0x23

/**
 * @brief I2C主控制器控制寄存器
 * @note 包含位[7:0] I2C_MST_CTRL
 * @note 控制I2C主控制器的启用、时钟分频等
 */
#define MPU6050_RA_I2C_MST_CTRL     0x24

/**
 * @brief I2C从设备0地址寄存器
 * @note 包含位[6:0] I2C_SLV0_ADDR（低7位地址），位[7] I2C_SLV0_RW（读写标志）
 * @note 设置I2C从设备0的地址和读写模式
 */
#define MPU6050_RA_I2C_SLV0_ADDR    0x25

/**
 * @brief I2C从设备0寄存器地址寄存器
 * @note 包含位[7:0] I2C_SLV0_REG
 * @note 设置I2C从设备0的寄存器地址
 */
#define MPU6050_RA_I2C_SLV0_REG     0x26

/**
 * @brief I2C从设备0控制寄存器
 * @note 包含位[6:0] I2C_SLV0_CTRL（数据长度/操作类型），位[7] I2C_SLV0_EN（启用标志）
 * @note 设置I2C从设备0的数据长度和启用状态
 */
#define MPU6050_RA_I2C_SLV0_CTRL    0x27

/**
 * @brief I2C从设备1地址寄存器
 * @note 包含位[6:0] I2C_SLV1_ADDR（低7位地址），位[7] I2C_SLV1_RW（读写标志）
 * @note 设置I2C从设备1的地址和读写模式
 */
#define MPU6050_RA_I2C_SLV1_ADDR    0x28

/**
 * @brief I2C从设备1寄存器地址寄存器
 * @note 包含位[7:0] I2C_SLV1_REG
 * @note 设置I2C从设备1的寄存器地址
 */
#define MPU6050_RA_I2C_SLV1_REG     0x29

/**
 * @brief I2C从设备1控制寄存器
 * @note 包含位[6:0] I2C_SLV1_CTRL（数据长度/操作类型），位[7] I2C_SLV1_EN（启用标志）
 * @note 设置I2C从设备1的数据长度和启用状态
 */
#define MPU6050_RA_I2C_SLV1_CTRL    0x2A

/**
 * @brief I2C从设备2地址寄存器
 * @note 包含位[6:0] I2C_SLV2_ADDR（低7位地址），位[7] I2C_SLV2_RW（读写标志）
 * @note 设置I2C从设备2的地址和读写模式
 */
#define MPU6050_RA_I2C_SLV2_ADDR    0x2B

/**
 * @brief I2C从设备2寄存器地址寄存器
 * @note 包含位[7:0] I2C_SLV2_REG
 * @note 设置I2C从设备2的寄存器地址
 */
#define MPU6050_RA_I2C_SLV2_REG     0x2C

/**
 * @brief I2C从设备2控制寄存器
 * @note 包含位[6:0] I2C_SLV2_CTRL（数据长度/操作类型），位[7] I2C_SLV2_EN（启用标志）
 * @note 设置I2C从设备2的数据长度和启用状态
 */
#define MPU6050_RA_I2C_SLV2_CTRL    0x2D

/**
 * @brief I2C从设备3地址寄存器
 * @note 包含位[6:0] I2C_SLV3_ADDR（低7位地址），位[7] I2C_SLV3_RW（读写标志）
 * @note 设置I2C从设备3的地址和读写模式
 */
#define MPU6050_RA_I2C_SLV3_ADDR    0x2E

/**
 * @brief I2C从设备3寄存器地址寄存器
 * @note 包含位[7:0] I2C_SLV3_REG
 * @note 设置I2C从设备3的寄存器地址
 */
#define MPU6050_RA_I2C_SLV3_REG     0x2F

/**
 * @brief I2C从设备3控制寄存器
 * @note 包含位[6:0] I2C_SLV3_CTRL（数据长度/操作类型），位[7] I2C_SLV3_EN（启用标志）
 * @note 设置I2C从设备3的数据长度和启用状态
 */
#define MPU6050_RA_I2C_SLV3_CTRL    0x30

/**
 * @brief I2C从设备4地址寄存器
 * @note 包含位[6:0] I2C_SLV4_ADDR（低7位地址），位[7] I2C_SLV4_RW（读写标志）
 * @note 设置I2C从设备4的地址和读写模式
 */
#define MPU6050_RA_I2C_SLV4_ADDR    0x31

/**
 * @brief I2C从设备4寄存器地址寄存器
 * @note 包含位[7:0] I2C_SLV4_REG
 * @note 设置I2C从设备4的寄存器地址
 */
#define MPU6050_RA_I2C_SLV4_REG     0x32

/**
 * @brief I2C从设备4数据输出寄存器
 * @note 包含位[7:0] I2C_SLV4_DO
 * @note 设置I2C从设备4的数据输出
 */
#define MPU6050_RA_I2C_SLV4_DO      0x33

/**
 * @brief I2C从设备4控制寄存器
 * @note 包含位[6:0] I2C_SLV4_CTRL（数据长度/操作类型），位[7] I2C_SLV4_EN（启用标志）
 * @note 设置I2C从设备4的数据长度和启用状态
 */
#define MPU6050_RA_I2C_SLV4_CTRL    0x34

/**
 * @brief I2C从设备4数据输入寄存器
 * @note 包含位[7:0] I2C_SLV4_DI
 * @note 读取I2C从设备4的数据输入
 */
#define MPU6050_RA_I2C_SLV4_DI      0x35

/**
 * @brief I2C主控制器状态寄存器
 * @note 包含位[7:0] I2C_MST_STATUS
 * @note 读取I2C主控制器的状态信息
 */
#define MPU6050_RA_I2C_MST_STATUS   0x36

/**
 * @brief 中断引脚配置寄存器
 * @note 包含位[7:0] INT_PIN_CFG
 * @note 配置中断引脚的极性、电平、类型等
 */
#define MPU6050_RA_INT_PIN_CFG      0x37

/**
 * @brief 中断使能寄存器
 * @note 包含位[7:0] INT_ENABLE
 * @note 启用或禁用特定中断源
 */
#define MPU6050_RA_INT_ENABLE       0x38

/**
 * @brief DMP中断状态寄存器
 * @note 包含位[7:0] DMP_INT_STATUS
 * @note 读取DMP（数字运动处理器）中断状态
 */
#define MPU6050_RA_DMP_INT_STATUS   0x39

/**
 * @brief 中断状态寄存器
 * @note 包含位[7:0] INT_STATUS
 * @note 读取中断状态信息，指示哪些中断源已被触发
 */
#define MPU6050_RA_INT_STATUS       0x3A

/**
 * @brief X轴加速度数据寄存器（高字节）
 * @note 包含位[15:8] ACCEL_XOUT_H
 * @note 读取X轴加速度的高8位数据
 */
#define MPU6050_RA_ACCEL_XOUT_H     0x3B

/**
 * @brief X轴加速度数据寄存器（低字节）
 * @note 包含位[7:0] ACCEL_XOUT_L
 * @note 读取X轴加速度的低8位数据
 */
#define MPU6050_RA_ACCEL_XOUT_L     0x3C

/**
 * @brief Y轴加速度数据寄存器（高字节）
 * @note 包含位[15:8] ACCEL_YOUT_H
 * @note 读取Y轴加速度的高8位数据
 */
#define MPU6050_RA_ACCEL_YOUT_H     0x3D

/**
 * @brief Y轴加速度数据寄存器（低字节）
 * @note 包含位[7:0] ACCEL_YOUT_L
 * @note 读取Y轴加速度的低8位数据
 */
#define MPU6050_RA_ACCEL_YOUT_L     0x3E

/**
 * @brief Z轴加速度数据寄存器（高字节）
 * @note 包含位[15:8] ACCEL_ZOUT_H
 * @note 读取Z轴加速度的高8位数据
 */
#define MPU6050_RA_ACCEL_ZOUT_H     0x3F

/**
 * @brief Z轴加速度数据寄存器（低字节）
 * @note 包含位[7:0] ACCEL_ZOUT_L
 * @note 读取Z轴加速度的低8位数据
 */
#define MPU6050_RA_ACCEL_ZOUT_L     0x40

/**
 * @brief 温度传感器数据寄存器（高字节）
 * @note 包含位[15:8] TEMP_OUT_H
 * @note 读取温度传感器的高8位数据
 */
#define MPU6050_RA_TEMP_OUT_H       0x41

/**
 * @brief 温度传感器数据寄存器（低字节）
 * @note 包含位[7:0] TEMP_OUT_L
 * @note 读取温度传感器的低8位数据
 */
#define MPU6050_RA_TEMP_OUT_L       0x42

/**
 * @brief X轴陀螺仪数据寄存器（高字节）
 * @note 包含位[15:8] GYRO_XOUT_H
 * @note 读取X轴陀螺仪的高8位数据
 */
#define MPU6050_RA_GYRO_XOUT_H      0x43

/**
 * @brief X轴陀螺仪数据寄存器（低字节）
 * @note 包含位[7:0] GYRO_XOUT_L
 * @note 读取X轴陀螺仪的低8位数据
 */
#define MPU6050_RA_GYRO_XOUT_L      0x44

/**
 * @brief Y轴陀螺仪数据寄存器（高字节）
 * @note 包含位[15:8] GYRO_YOUT_H
 * @note 读取Y轴陀螺仪的高8位数据
 */
#define MPU6050_RA_GYRO_YOUT_H      0x45

/**
 * @brief Y轴陀螺仪数据寄存器（低字节）
 * @note 包含位[7:0] GYRO_YOUT_L
 * @note 读取Y轴陀螺仪的低8位数据
 */
#define MPU6050_RA_GYRO_YOUT_L      0x46

/**
 * @brief Z轴陀螺仪数据寄存器（高字节）
 * @note 包含位[15:8] GYRO_ZOUT_H
 * @note 读取Z轴陀螺仪的高8位数据
 */
#define MPU6050_RA_GYRO_ZOUT_H      0x47

/**
 * @brief Z轴陀螺仪数据寄存器（低字节）
 * @note 包含位[7:0] GYRO_ZOUT_L
 * @note 读取Z轴陀螺仪的低8位数据
 */
#define MPU6050_RA_GYRO_ZOUT_L      0x48

/**
 * @brief 外部传感器数据寄存器00
 * @note 包含位[7:0] EXT_SENS_DATA_00
 * @note 读取外部传感器的第一个字节数据
 */
#define MPU6050_RA_EXT_SENS_DATA_00 0x49

/**
 * @brief 外部传感器数据寄存器01
 * @note 包含位[7:0] EXT_SENS_DATA_01
 * @note 读取外部传感器的第二个字节数据
 */
#define MPU6050_RA_EXT_SENS_DATA_01 0x4A

/**
 * @brief 外部传感器数据寄存器02
 * @note 包含位[7:0] EXT_SENS_DATA_02
 * @note 读取外部传感器的第三个字节数据
 */
#define MPU6050_RA_EXT_SENS_DATA_02 0x4B

/**
 * @brief 外部传感器数据寄存器03
 * @note 包含位[7:0] EXT_SENS_DATA_03
 * @note 读取外部传感器的第四个字节数据
 */
#define MPU6050_RA_EXT_SENS_DATA_03 0x4C

/**
 * @brief 外部传感器数据寄存器04
 * @note 包含位[7:0] EXT_SENS_DATA_04
 * @note 读取外部传感器的第五个字节数据
 */
#define MPU6050_RA_EXT_SENS_DATA_04 0x4D

/**
 * @brief 外部传感器数据寄存器05
 * @note 包含位[7:0] EXT_SENS_DATA_05
 * @note 读取外部传感器的第六个字节数据
 */
#define MPU6050_RA_EXT_SENS_DATA_05 0x4E

/**
 * @brief 外部传感器数据寄存器06
 * @note 包含位[7:0] EXT_SENS_DATA_06
 * @note 读取外部传感器的第七个字节数据
 */
#define MPU6050_RA_EXT_SENS_DATA_06 0x4F
/**
 * @file mpu6050.h
 * @brief MPU6050传感器寄存器地址定义头文件
 * @details 
 * 本文件定义了MPU6050传感器的寄存器地址和相关配置常量，用于与传感器硬件寄存器进行交互。
 * 包含外部传感器数据寄存器、I2C主从控制寄存器、运动检测寄存器、电源管理寄存器等关键地址定义。
 * 所有宏定义的命名遵循MPU6050数据手册中的寄存器命名规范，地址值为十六进制格式。
 * 每个寄存器地址对应传感器内部特定的功能模块，通过读写这些地址可实现传感器配置和数据获取。
 */

/* 外部传感器数据寄存器07-23定义 */
/** @brief 外部传感器数据寄存器07 地址0x50，存储外部传感器第7字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_07 0x50
/** @brief 外部传感器数据寄存器08 地址0x51，存储外部传感器第8字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_08 0x51
/** @brief 外部传感器数据寄存器09 地址0x52，存储外部传感器第9字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_09 0x52
/** @brief 外部传感器数据寄存器10 地址0x53，存储外部传感器第10字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_10 0x53
/** @brief 外部传感器数据寄存器11 地址0x54，存储外部传感器第11字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_11 0x54
/** @brief 外部传感器数据寄存器12 地址0x55，存储外部传感器第12字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_12 0x55
/** @brief 外部传感器数据寄存器13 地址0x56，存储外部传感器第13字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_13 0x56
/** @brief 外部传感器数据寄存器14 地址0x57，存储外部传感器第14字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_14 0x57
/** @brief 外部传感器数据寄存器15 地址0x58，存储外部传感器第15字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_15 0x58
/** @brief 外部传感器数据寄存器16 地址0x59，存储外部传感器第16字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_16 0x59
/** @brief 外部传感器数据寄存器17 地址0x5A，存储外部传感器第17字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_17 0x5A
/** @brief 外部传感器数据寄存器18 地址0x5B，存储外部传感器第18字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_18 0x5B
/** @brief 外部传感器数据寄存器19 地址0x5C，存储外部传感器第19字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_19 0x5C
/** @brief 外部传感器数据寄存器20 地址0x5D，存储外部传感器第20字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_20 0x5D
/** @brief 外部传感器数据寄存器21 地址0x5E，存储外部传感器第21字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_21 0x5E
/** @brief 外部传感器数据寄存器22 地址0x5F，存储外部传感器第22字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_22 0x5F
/** @brief 外部传感器数据寄存器23 地址0x60，存储外部传感器第23字节数据 */
#define MPU6050_RA_EXT_SENS_DATA_23 0x60

/* 运动检测相关寄存器 */
/** @brief 运动检测状态寄存器 地址0x61，包含各轴运动检测标志位 */
#define MPU6050_RA_MOT_DETECT_STATUS    0x61

/* I2C从设备控制寄存器 */
/** @brief I2C从设备0数据输出寄存器 地址0x63，配置从设备0的写操作数据 */
#define MPU6050_RA_I2C_SLV0_DO      0x63
/** @brief I2C从设备1数据输出寄存器 地址0x64，配置从设备1的写操作数据 */
#define MPU6050_RA_I2C_SLV1_DO      0x64
/** @brief I2C从设备2数据输出寄存器 地址0x65，配置从设备2的写操作数据 */
#define MPU6050_RA_I2C_SLV2_DO      0x65
/** @brief I2C从设备3数据输出寄存器 地址0x66，配置从设备3的写操作数据 */
#define MPU6050_RA_I2C_SLV3_DO      0x66

/* 主控制与延迟寄存器 */
/** @brief I2C主控延迟控制寄存器 地址0x67，配置各从设备的延迟设置 */
#define MPU6050_RA_I2C_MST_DELAY_CTRL   0x67

/* 信号路径控制寄存器 */
/** @brief 信号路径复位寄存器 地址0x68，用于复位传感器信号路径 */
#define MPU6050_RA_SIGNAL_PATH_RESET    0x68

/* 运动检测控制寄存器 */
/** @brief 运动检测控制寄存器 地址0x69，配置加速度计比较阈值等参数 */
#define MPU6050_RA_MOT_DETECT_CTRL      0x69

/* 用户控制寄存器 */
/** @brief 用户控制寄存器 地址0x6A，主控I2C使能/FIFO使能等全局控制 */
#define MPU6050_RA_USER_CTRL        0x6A

/* 电源管理寄存器 */
/** @brief 电源管理1寄存器 地址0x6B，控制设备唤醒、睡眠、时钟源选择 */
#define MPU6050_RA_PWR_MGMT_1       0x6B
/** @brief 电源管理2寄存器 地址0x6C，控制各传感器通道的待机模式 */
#define MPU6050_RA_PWR_MGMT_2       0x6C

/* 存储区控制寄存器 */
/** @brief 存储体选择寄存器 地址0x6D，选择DMP使用的存储体 */
#define MPU6050_RA_BANK_SEL         0x6D
/** @brief 存储器起始地址寄存器 地址0x6E，设置DMP存储器的访问起始地址 */
#define MPU6050_RA_MEM_START_ADDR   0x6E
/** @brief 存储器读写寄存器 地址0x6F，用于读写DMP存储器数据 */
#define MPU6050_RA_MEM_R_W          0x6F

/* DMP配置寄存器 */
/** @brief DMP配置寄存器1 地址0x70，配置DMP功能参数 */
#define MPU6050_RA_DMP_CFG_1        0x70
/** @brief DMP配置寄存器2 地址0x71，配置DMP功能参数 */
#define MPU6050_RA_DMP_CFG_2        0x71

/* FIFO状态寄存器 */
/** @brief FIFO计数器高位 地址0x72，存储FIFO中有效数据个数的高8位 */
#define MPU6050_RA_FIFO_COUNTH      0x72
/** @brief FIFO计数器低位 地址0x73，存储FIFO中有效数据个数的低8位 */
#define MPU6050_RA_FIFO_COUNTL      0x73
/** @brief FIFO数据读写寄存器 地址0x74，用于读取FIFO数据或写入用户数据 */
#define MPU6050_RA_FIFO_R_W         0x74

/* 设备标识寄存器 */
/** @brief 设备ID寄存器 地址0x75，存储设备识别码（固定值0x68） */
#define MPU6050_RA_WHO_AM_I         0x75

/**
 * @file mpu6050.h
 * @brief MPU6050传感器配置位定义头文件
 * @details
 * 本文件定义了MPU6050传感器的各种配置位和参数选项，包括：
 * - 温度补偿相关控制位
 * - 外部同步信号配置
 * - 数字低通滤波器(DLPF)带宽设置
 * - 陀螺仪和加速度计量程选择
 * - 加速度计自检配置
 * - 模拟数字转换参数
 * 所有配置通过位操作实现，具体使用需结合对应寄存器的位域定义。
 */

/* 温度补偿相关配置 --------------------------------------------------------*/
/** @brief 温度传感器电源模式位（TC_PWR_MODE）寄存器位位置 */
#define MPU6050_TC_PWR_MODE_BIT     7
/** @brief 温度补偿偏移使能位（TC_OFFSET）寄存器位位置 */
#define MPU6050_TC_OFFSET_BIT       6
/** @brief 温度补偿偏移值的位长度 */
#define MPU6050_TC_OFFSET_LENGTH    6
/** @brief OTP存储区有效标志位（TC_OTP_BNK_VLD）寄存器位位置 */
#define MPU6050_TC_OTP_BNK_VLD_BIT  0

/* VDDIO电平配置 ----------------------------------------------------------*/
/** @brief VDDIO电平选择：连接至VLOGIC（0: 低电平） */
#define MPU6050_VDDIO_LEVEL_VLOGIC  0
/** @brief VDDIO电平选择：连接至VDD（1: 高电平） */
#define MPU6050_VDDIO_LEVEL_VDD     1

/* 外部同步与数字低通滤波器配置 --------------------------------------------*/
/** @brief 外部同步设置位（EXT_SYNC_SET）起始位位置 */
#define MPU6050_CFG_EXT_SYNC_SET_BIT    5
/** @brief 外部同步设置位的位长度 */
#define MPU6050_CFG_EXT_SYNC_SET_LENGTH 3
/** @brief 数字低通滤波器配置位（DLPF_CFG）起始位位置 */
#define MPU6050_CFG_DLPF_CFG_BIT    2
/** @brief 数字低通滤波器配置位的位长度 */
#define MPU6050_CFG_DLPF_CFG_LENGTH 3

/* 外部同步信号源选项 ------------------------------------------------------*/
/** @brief 禁用外部同步输入 */
#define MPU6050_EXT_SYNC_DISABLED       0x0
/** @brief 温度传感器数据低字节作为同步源 */
#define MPU6050_EXT_SYNC_TEMP_OUT_L     0x1
/** @brief 陀螺仪X轴数据低字节作为同步源 */
#define MPU6050_EXT_SYNC_GYRO_XOUT_L    0x2
/** @brief 陀螺仪Y轴数据低字节作为同步源 */
#define MPU6050_EXT_SYNC_GYRO_YOUT_L    0x3
/** @brief 陀螺仪Z轴数据低字节作为同步源 */
#define MPU6050_EXT_SYNC_GYRO_ZOUT_L    0x4
/** @brief 加速度计X轴数据低字节作为同步源 */
#define MPU6050_EXT_SYNC_ACCEL_XOUT_L   0x5
/** @brief 加速度计Y轴数据低字节作为同步源 */
#define MPU6050_EXT_SYNC_ACCEL_YOUT_L   0x6
/** @brief 加速度计Z轴数据低字节作为同步源 */
#define MPU6050_EXT_SYNC_ACCEL_ZOUT_L   0x7

/* 数字低通滤波器带宽设置 --------------------------------------------------*/
/** @brief 数字低通滤波器带宽256Hz（加速度计：260Hz，陀螺仪：256Hz） */
#define MPU6050_DLPF_BW_256         0x00
/** @brief 数字低通滤波器带宽188Hz */
#define MPU6050_DLPF_BW_188         0x01
/** @brief 数字低通滤波器带宽98Hz */
#define MPU6050_DLPF_BW_98          0x02
/** @brief 数字低通滤波器带宽42Hz */
#define MPU6050_DLPF_BW_42          0x03
/** @brief 数字低通滤波器带宽20Hz */
#define MPU6050_DLPF_BW_20          0x04
/** @brief 数字低通滤波器带宽10Hz */
#define MPU6050_DLPF_BW_10          0x05
/** @brief 数字低通滤波器带宽5Hz */
#define MPU6050_DLPF_BW_5           0x06

/* 陀螺仪量程配置 ---------------------------------------------------------*/
/** @brief 陀螺仪量程选择位（FS_SEL）起始位位置 */
#define MPU6050_GCONFIG_FS_SEL_BIT      4
/** @brief 陀螺仪量程选择位的位长度 */
#define MPU6050_GCONFIG_FS_SEL_LENGTH   2

/** @brief 陀螺仪量程±250°/s */
#define MPU6050_GYRO_FS_250         0x00
/** @brief 陀螺仪量程±500°/s */
#define MPU6050_GYRO_FS_500         0x01
/** @brief 陀螺仪量程±1000°/s */
#define MPU6050_GYRO_FS_1000        0x02
/** @brief 陀螺仪量程±2000°/s */
#define MPU6050_GYRO_FS_2000        0x03

/* 加速度计配置 -----------------------------------------------------------*/
/** @brief 加速度计X轴自检使能位（XA_ST）寄存器位位置 */
#define MPU6050_ACONFIG_XA_ST_BIT           7
/** @brief 加速度计Y轴自检使能位（YA_ST）寄存器位位置 */
#define MPU6050_ACONFIG_YA_ST_BIT           6
/** @brief 加速度计Z轴自检使能位（ZA_ST）寄存器位位置 */
#define MPU6050_ACONFIG_ZA_ST_BIT           5
/** @brief 加速度计量程选择位（AFS_SEL）起始位位置 */
#define MPU6050_ACONFIG_AFS_SEL_BIT         4
/** @brief 加速度计量程选择位的位长度 */
#define MPU6050_ACONFIG_AFS_SEL_LENGTH      2
/** @brief 加速度计高通滤波器配置位（ACCEL_HPF）起始位位置 */
#define MPU6050_ACONFIG_ACCEL_HPF_BIT       2
/** @brief 加速度计高通滤波器配置位的位长度 */
#define MPU6050_ACONFIG_ACCEL_HPF_LENGTH    3

/**
 * @file mpu6050.h
 * @brief MPU6050传感器功能配置与时钟控制头文件
 * @details
 * 本文件定义了MPU6050传感器的以下配置参数：
 * - 加速度计量程选择
 * - 数字高通滤波器模式配置
 * - FIFO数据流使能控制位
 * - I2C主控制模块配置
 * - 时钟分频系数设置
 * 所有配置均需通过位操作写入对应寄存器实现传感器功能控制。
 */

/* 加速度计量程配置 --------------------------------------------------------*/
/** @brief 加速度计量程±2g（16384 LSB/g） */
#define MPU6050_ACCEL_FS_2          0x00
/** @brief 加速度计量程±4g（8192 LSB/g） */
#define MPU6050_ACCEL_FS_4          0x01
/** @brief 加速度计量程±8g（4096 LSB/g） */
#define MPU6050_ACCEL_FS_8          0x02
/** @brief 加速度计量程±16g（2048 LSB/g） */
#define MPU6050_ACCEL_FS_16         0x03

/* 数字高通滤波器配置 ------------------------------------------------------*/
/** @brief 复位数字高通滤波器（DHPF）到初始状态 */
#define MPU6050_DHPF_RESET          0x00
/** @brief 数字高通滤波器截止频率5Hz */
#define MPU6050_DHPF_5              0x01
/** @brief 数字高通滤波器截止频率2.5Hz */
#define MPU6050_DHPF_2P5            0x02
/** @brief 数字高通滤波器截止频率1.25Hz */
#define MPU6050_DHPF_1P25           0x03
/** @brief 数字高通滤波器截止频率0.63Hz */
#define MPU6050_DHPF_0P63           0x04
/** @brief 保持当前高通滤波器数据不更新 */
#define MPU6050_DHPF_HOLD           0x07

/* FIFO数据流使能控制位 ----------------------------------------------------*/
/** @brief 温度传感器数据写入FIFO使能位（寄存器位7） */
#define MPU6050_TEMP_FIFO_EN_BIT    7
/** @brief 陀螺仪X轴数据写入FIFO使能位（寄存器位6） */
#define MPU6050_XG_FIFO_EN_BIT      6
/** @brief 陀螺仪Y轴数据写入FIFO使能位（寄存器位5） */
#define MPU6050_YG_FIFO_EN_BIT      5
/** @brief 陀螺仪Z轴数据写入FIFO使能位（寄存器位4） */
#define MPU6050_ZG_FIFO_EN_BIT      4
/** @brief 加速度计数据写入FIFO使能位（寄存器位3） */
#define MPU6050_ACCEL_FIFO_EN_BIT   3
/** @brief 从设备2数据写入FIFO使能位（寄存器位2） */
#define MPU6050_SLV2_FIFO_EN_BIT    2
/** @brief 从设备1数据写入FIFO使能位（寄存器位1） */
#define MPU6050_SLV1_FIFO_EN_BIT    1
/** @brief 从设备0数据写入FIFO使能位（寄存器位0） */
#define MPU6050_SLV0_FIFO_EN_BIT    0

/* I2C主控制模块配置 -------------------------------------------------------*/
/** @brief 多主控模式使能位（寄存器位7） */
#define MPU6050_MULT_MST_EN_BIT     7
/** @brief 等待外部传感器数据准备完成位（寄存器位6） */
#define MPU6050_WAIT_FOR_ES_BIT     6
/** @brief 从设备3数据写入FIFO使能位（寄存器位5） */
#define MPU6050_SLV_3_FIFO_EN_BIT   5
/** @brief I2C主控脉冲模式控制位（寄存器位4） */
#define MPU6050_I2C_MST_P_NSR_BIT   4
/** @brief I2C主控时钟频率选择起始位（寄存器位3） */
#define MPU6050_I2C_MST_CLK_BIT     3
/** @brief I2C主控时钟频率选择位长度 */
#define MPU6050_I2C_MST_CLK_LENGTH  4

/* 时钟分频系数配置 --------------------------------------------------------*/
/** @brief 时钟分频系数348（时钟=1MHz/(1+348)=2.865kHz） */
#define MPU6050_CLOCK_DIV_348       0x0
/** @brief 时钟分频系数333（时钟=1MHz/(1+333)=2.994kHz） */
#define MPU6050_CLOCK_DIV_333       0x1
/** @brief 时钟分频系数320（时钟=1MHz/(1+320)=3.115kHz） */
#define MPU6050_CLOCK_DIV_320       0x2
/** @brief 时钟分频系数308（时钟=1MHz/(1+308)=3.236kHz） */
#define MPU6050_CLOCK_DIV_308       0x3
/** @brief 时钟分频系数296（时钟=1MHz/(1+296)=3.367kHz） */
#define MPU6050_CLOCK_DIV_296       0x4
/** @brief 时钟分频系数286（时钟=1MHz/(1+286)=3.484kHz） */
#define MPU6050_CLOCK_DIV_286       0x5
/** @brief 时钟分频系数276（时钟=1MHz/(1+276)=3.610kHz） */
#define MPU6050_CLOCK_DIV_276       0x6
/** @brief 时钟分频系数267（时钟=1MHz/(1+267)=3.731kHz） */
#define MPU6050_CLOCK_DIV_267       0x7
/** @brief 时钟分频系数258（时钟=1MHz/(1+258)=3.861kHz） */
#define MPU6050_CLOCK_DIV_258       0x8
/** @brief 时钟分频系数500（时钟=1MHz/(1+500)=1.996kHz） */
#define MPU6050_CLOCK_DIV_500       0x9
/** @brief 时钟分频系数471（时钟=1MHz/(1+471)=2.119kHz） */
#define MPU6050_CLOCK_DIV_471       0xA
/** @brief 时钟分频系数444（时钟=1MHz/(1+444)=2.249kHz） */
#define MPU6050_CLOCK_DIV_444       0xB
/** @brief 时钟分频系数421（时钟=1MHz/(1+421)=2.370kHz） */
#define MPU6050_CLOCK_DIV_421       0xC
/** @brief 时钟分频系数400（时钟=1MHz/(1+400)=2.494kHz） */
#define MPU6050_CLOCK_DIV_400       0xD
/** @brief 时钟分频系数381（时钟=1MHz/(1+381)=2.617kHz） */
#define MPU6050_CLOCK_DIV_381       0xE
/** @brief 时钟分频系数364（时钟=1MHz/(1+364)=2.740kHz） */
#define MPU6050_CLOCK_DIV_364       0xF

/**
 * @brief MPU6050寄存器位域配置系统
 * @note 该模块包含四类寄存器位定义：
 * [1] I2C从设备控制寄存器位（SLV0-SLV3）
 * [2] 扩展I2C控制寄存器位（SLV4）
 * [3] 主总线状态寄存器位
 * [4] 中断配置寄存器位
 * @warning 使用位操作时需要特别注意：
 * - 多bit字段需使用位掩码组合操作
 * - 长度字段对应数值需要减1写入寄存器
 * - 使能位通常需要最后设置以避免中间状态
 */

/*────────────────────────── I2C从设备控制寄存器位 ────────────────────────*/
/* 寄存器组：0x37-0x3A（SLV0-SLV3）*/
#define MPU6050_I2C_SLV_RW_BIT      7   // 从设备读写模式选择 [1=读从设备，0=写从设备]
#define MPU6050_I2C_SLV_ADDR_BIT    6   // 从设备地址起始位 [6:0]（7位地址）
#define MPU6050_I2C_SLV_ADDR_LENGTH 7   // 从设备地址字段总位数（固定7位I2C地址）
#define MPU6050_I2C_SLV_EN_BIT      7   // 从设备通道使能位 [1=激活该从设备配置]
#define MPU6050_I2C_SLV_BYTE_SW_BIT 6   // 字节序交换控制 [1=高低字节交换]
#define MPU6050_I2C_SLV_REG_DIS_BIT 5   // 寄存器自动递增禁用 [1=禁用地址自动+1]
#define MPU6050_I2C_SLV_GRP_BIT     4   // 寄存器分组使能 [1=启用寄存器组切换]
#define MPU6050_I2C_SLV_LEN_BIT     3   // 传输数据长度起始位 [3:0]（长度范围1-15）
#define MPU6050_I2C_SLV_LEN_LENGTH  4   // 数据长度字段位数（实际长度=寄存器值+1）

/*───────────────────────── 扩展I2C控制寄存器位（SLV4）─────────────────────*/
/* 寄存器地址：0x34（I2C_SLV4_CTRL）*/
#define MPU6050_I2C_SLV4_RW_BIT         7   // SLV4读写模式 [同SLVx_RW_BIT]
#define MPU6050_I2C_SLV4_ADDR_BIT       6   // SLV4地址起始位
#define MPU6050_I2C_SLV4_ADDR_LENGTH    7   // SLV4地址字段长度
#define MPU6050_I2C_SLV4_EN_BIT         7   // SLV4事务使能 [需脉冲式触发]
#define MPU6050_I2C_SLV4_INT_EN_BIT     6   // SLV4完成中断使能 [1=启用]
#define MPU6050_I2C_SLV4_REG_DIS_BIT    5   // SLV4寄存器自增禁用
#define MPU6050_I2C_SLV4_MST_DLY_BIT    4   // 主时钟延迟控制起始位 [4:0]
#define MPU6050_I2C_SLV4_MST_DLY_LENGTH 5   // 延迟时间字段长度（单位：1ms）

/*───────────────────────── 主总线状态寄存器位 ───────────────────────────*/
/* 寄存器地址：0x36（I2C_MST_STATUS）*/
#define MPU6050_MST_PASS_THROUGH_BIT    7   // 主从模式切换 [1=启用旁路模式]
#define MPU6050_MST_I2C_SLV4_DONE_BIT   6   // SLV4事务完成标志 [自动置1]
#define MPU6050_MST_I2C_LOST_ARB_BIT    5   // 总线仲裁丢失标志 [需手动清除]
#define MPU6050_MST_I2C_SLV4_NACK_BIT   4   // SLV4传输NACK错误
#define MPU6050_MST_I2C_SLV3_NACK_BIT   3   // SLV3传输NACK错误
#define MPU6050_MST_I2C_SLV2_NACK_BIT   2   // SLV2传输NACK错误
#define MPU6050_MST_I2C_SLV1_NACK_BIT   1   // SLV1传输NACK错误
#define MPU6050_MST_I2C_SLV0_NACK_BIT   0   // SLV0传输NACK错误

/*───────────────────────── 中断配置寄存器位 ────────────────────────────*/
/* 寄存器地址：0x37（INT_PIN_CFG）*/
#define MPU6050_INTCFG_INT_LEVEL_BIT        7   // 中断电平 [0=高电平有效]
#define MPU6050_INTCFG_INT_OPEN_BIT         6   // 中断引脚模式 [0=推挽，1=开漏]
#define MPU6050_INTCFG_LATCH_INT_EN_BIT     5   // 中断锁存使能 [1=保持直到清除]
#define MPU6050_INTCFG_INT_RD_CLEAR_BIT     4   // 中断清除方式 [1=读操作自动清除]
#define MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT  3   // FSYNC引脚电平 [0=高有效]
#define MPU6050_INTCFG_FSYNC_INT_EN_BIT     2   // FSYNC中断使能 [1=启用]
#define MPU6050_INTCFG_I2C_BYPASS_EN_BIT    1   // I2C主控旁路 [1=直接访问AUX_I2C]
#define MPU6050_INTCFG_CLKOUT_EN_BIT        0   // 时钟输出使能 [1=输出内部8MHz时钟]

/**
 * @brief MPU6050中断系统配置与状态标识
 * @note 本模块包含三类配置：
 * [1] 中断引脚工作模式配置
 * [2] 中断状态标识位
 * [3] 数字运动处理器(DMP)专用中断
 * [4] 运动检测方向状态位
 * [5] 传感器数据路径延迟控制
 * @warning 使用DMP功能时需要特别注意：
 * - 需加载正确的固件库
 * - 部分DMP中断位功能取决于固件版本
 */

/*────────────────────── 中断引脚配置模式 ──────────────────────*/
/* 寄存器地址：0x37（INT_PIN_CFG）*/
#define MPU6050_INTMODE_ACTIVEHIGH  0x00  // 中断有效电平：高电平触发
#define MPU6050_INTMODE_ACTIVELOW   0x01  // 中断有效电平：低电平触发

#define MPU6050_INTDRV_PUSHPULL     0x00  // 推挽输出模式（标准驱动能力）
#define MPU6050_INTDRV_OPENDRAIN    0x01  // 开漏输出模式（需外接上拉电阻）

#define MPU6050_INTLATCH_50USPULSE  0x00  // 中断锁存模式：50us脉冲
#define MPU6050_INTLATCH_WAITCLEAR  0x01  // 中断锁存模式：保持有效直到清除

#define MPU6050_INTCLEAR_STATUSREAD 0x00  // 中断清除方式：读取INT_STATUS寄存器
#define MPU6050_INTCLEAR_ANYREAD    0x01  // 中断清除方式：读取任意寄存器

/*────────────────────── 中断状态寄存器位 ──────────────────────*/
/* 寄存器地址：0x3A（INT_STATUS）*/
#define MPU6050_INTERRUPT_FF_BIT            7   // 自由落体检测中断
#define MPU6050_INTERRUPT_MOT_BIT           6   // 运动检测中断（阈值触发）
#define MPU6050_INTERRUPT_ZMOT_BIT          5   // 零运动检测中断 
#define MPU6050_INTERRUPT_FIFO_OFLOW_BIT    4   // FIFO溢出中断
#define MPU6050_INTERRUPT_I2C_MST_INT_BIT   3   // I2C主控制器错误中断
#define MPU6050_INTERRUPT_PLL_RDY_INT_BIT   2   // PLL时钟稳定中断
#define MPU6050_INTERRUPT_DMP_INT_BIT       1   // DMP处理完成中断
#define MPU6050_INTERRUPT_DATA_RDY_BIT      0   // 新数据就绪中断

/*────────────────────── DMP专用中断位 ───────────────────────*/
/* 寄存器地址：0x38（DMP_INT_STATUS）*/
// 注：以下位功能需配合DMP固件使用，不同固件版本可能有差异
#define MPU6050_DMPINT_5_BIT            5   // DMP姿态识别中断5（预留）
#define MPU6050_DMPINT_4_BIT            4   // DMP计步器中断
#define MPU6050_DMPINT_3_BIT            3   // DMP屏幕方向检测中断
#define MPU6050_DMPINT_2_BIT            2   // DMP手势识别中断
#define MPU6050_DMPINT_1_BIT            1   // DMP运动唤醒中断
#define MPU6050_DMPINT_0_BIT            0   // DMP数据融合完成中断

/*────────────────────── 运动检测方向状态 ─────────────────────*/
/* 寄存器地址：0x1F（MOT_DETECT_STATUS）*/
#define MPU6050_MOTION_MOT_XNEG_BIT     7   // X轴负向运动检测
#define MPU6050_MOTION_MOT_XPOS_BIT     6   // X轴正向运动检测
#define MPU6050_MOTION_MOT_YNEG_BIT     5   // Y轴负向运动检测  
#define MPU6050_MOTION_MOT_YPOS_BIT     4   // Y轴正向运动检测
#define MPU6050_MOTION_MOT_ZNEG_BIT     3   // Z轴负向运动检测
#define MPU6050_MOTION_MOT_ZPOS_BIT     2   // Z轴正向运动检测
#define MPU6050_MOTION_MOT_ZRMOT_BIT    0   // 零运动检测状态

/*────────────────────── 延迟控制寄存器位 ─────────────────────*/
/* 寄存器地址：0x67（I2C_MST_DELAY_CTRL）*/
#define MPU6050_DELAYCTRL_DELAY_ES_SHADOW_BIT   7   // 传感器数据同步延迟使能
#define MPU6050_DELAYCTRL_I2C_SLV4_DLY_EN_BIT   4   // SLV4时钟延迟同步使能
#define MPU6050_DELAYCTRL_I2C_SLV3_DLY_EN_BIT   3   // SLV3时钟延迟同步使能  
#define MPU6050_DELAYCTRL_I2C_SLV2_DLY_EN_BIT   2   // SLV2时钟延迟同步使能
#define MPU6050_DELAYCTRL_I2C_SLV1_DLY_EN_BIT   1   // SLV1时钟延迟同步使能
#define MPU6050_DELAYCTRL_I2C_SLV0_DLY_EN_BIT   0   // SLV0时钟延迟同步使能

/**
 * @brief MPU6050系统控制与配置寄存器位域
 * @note 本模块包含四类核心控制：
 * [1] 传感器信号路径复位控制
 * [2] 运动检测参数配置
 * [3] 用户控制功能配置
 * [4] 电源管理与时钟控制
 * @warning 重要操作注意事项：
 * - 设备复位操作会清除所有寄存器设置
 * - FIFO复位后需重新配置采样参数
 * - DMP启用前必须加载有效固件
 */

/*────────────────── 传感器信号路径复位控制 ───────────────────*/
/* 寄存器地址：0x68（SIGNAL_PATH_RESET）*/
#define MPU6050_PATHRESET_GYRO_RESET_BIT    2   // 陀螺仪模拟信号路径复位（脉冲有效）
#define MPU6050_PATHRESET_ACCEL_RESET_BIT   1   // 加速度计信号路径复位
#define MPU6050_PATHRESET_TEMP_RESET_BIT    0   // 温度传感器路径复位

/*────────────────── 运动检测参数配置 ──────────────────────*/
/* 寄存器地址：0x69（ACCEL_ON_DELAY）*/
#define MPU6050_DETECT_ACCEL_ON_DELAY_BIT       5   // 加速度启用延迟起始位（2位）
#define MPU6050_DETECT_ACCEL_ON_DELAY_LENGTH    2   // 延迟单位：4ms/step
#define MPU6050_DETECT_FF_COUNT_BIT             3   // 自由落体检测计数器位（2位）
#define MPU6050_DETECT_FF_COUNT_LENGTH          2   // 检测持续时间系数
#define MPU6050_DETECT_MOT_COUNT_BIT            1   // 运动检测计数器位（2位）
#define MPU6050_DETECT_MOT_COUNT_LENGTH         2   // 检测持续时间系数

/* 寄存器地址：0x6A（FF_DUR */
#define MPU6050_DETECT_DECREMENT_RESET  0x0  // 检测计数器模式：禁用计数递减
#define MPU6050_DETECT_DECREMENT_1      0x1  // 每1个采样周期递减
#define MPU6050_DETECT_DECREMENT_2      0x2  // 每2个采样周期递减
#define MPU6050_DETECT_DECREMENT_4      0x3  // 每4个采样周期递减

/*────────────────── 用户控制寄存器位 ──────────────────────*/
/* 寄存器地址：0x6A（USER_CTRL）*/
#define MPU6050_USERCTRL_DMP_EN_BIT             7   // DMP数字运动处理器使能
#define MPU6050_USERCTRL_FIFO_EN_BIT            6   // FIFO缓冲区使能
#define MPU6050_USERCTRL_I2C_MST_EN_BIT         5   // I2C主控制器使能
#define MPU6050_USERCTRL_I2C_IF_DIS_BIT         4   // 禁用辅助I2C接口
#define MPU6050_USERCTRL_DMP_RESET_BIT          3   // DMP硬件复位（脉冲有效） 
#define MPU6050_USERCTRL_FIFO_RESET_BIT         2   // FIFO缓冲区复位
#define MPU6050_USERCTRL_I2C_MST_RESET_BIT      1   // I2C主控制器复位
#define MPU6050_USERCTRL_SIG_COND_RESET_BIT     0   // 传感器信号条件复位

/*────────────────── 电源管理配置 ────────────────────────*/
/* 寄存器地址：0x6B（PWR_MGMT_1）*/
#define MPU6050_PWR1_DEVICE_RESET_BIT   7   // 设备全局复位（高脉冲触发）
#define MPU6050_PWR1_SLEEP_BIT          6   // 低功耗睡眠模式使能
#define MPU6050_PWR1_CYCLE_BIT          5   // 周期唤醒模式（与SLEEP互斥）
#define MPU6050_PWR1_TEMP_DIS_BIT       3   // 禁用温度传感器
#define MPU6050_PWR1_CLKSEL_BIT         2   // 时钟源选择起始位（3位）
#define MPU6050_PWR1_CLKSEL_LENGTH      3   // 时钟选择字段位宽

/**
 * @brief MPU6050时钟系统与电源管理模式配置
 * @note 本模块包含四类核心配置：
 * [1] 时钟源选择模式
 * [2] 低功耗与传感器待机控制
 * [3] 存储器接口配置
 * [4] RTOS任务参数配置
 * @warning 重要操作注意事项：
 * - PLL时钟模式需要稳定时钟源
 * - 待机模式会停止传感器数据采集
 * - 内存库切换前需要禁用DMP
 */

/*────────────────── 时钟源配置模式 ───────────────────*/
/* 寄存器地址：0x6B（PWR_MGMT_1）CLKSEL字段 */
#define MPU6050_CLOCK_INTERNAL          0x00  // 内部8MHz RC振荡器
#define MPU6050_CLOCK_PLL_XGYRO         0x01  // X轴陀螺仪作为PLL参考源
#define MPU6050_CLOCK_PLL_YGYRO         0x02  // Y轴陀螺仪作为PLL参考源
#define MPU6050_CLOCK_PLL_ZGYRO         0x03  // Z轴陀螺仪作为PLL参考源
#define MPU6050_CLOCK_PLL_EXT32K        0x04  // 外部32.768kHz晶振
#define MPU6050_CLOCK_PLL_EXT19M        0x05  // 外部19.2MHz晶振
#define MPU6050_CLOCK_KEEP_RESET        0x07  // 保持复位状态（调试用）

/*────────────────── 电源管理2寄存器位 ─────────────────*/
/* 寄存器地址：0x6C（PWR_MGMT_2） */
#define MPU6050_PWR2_LP_WAKE_CTRL_BIT       7   // 低功耗唤醒频率控制起始位
#define MPU6050_PWR2_LP_WAKE_CTRL_LENGTH    2   // 唤醒频率选择字段
#define MPU6050_PWR2_STBY_XA_BIT            5   // X轴加速度计待机模式
#define MPU6050_PWR2_STBY_YA_BIT            4   // Y轴加速度计待机模式
#define MPU6050_PWR2_STBY_ZA_BIT            3   // Z轴加速度计待机模式
#define MPU6050_PWR2_STBY_XG_BIT            2   // X轴陀螺仪待机模式
#define MPU6050_PWR2_STBY_YG_BIT            1   // Y轴陀螺仪待机模式
#define MPU6050_PWR2_STBY_ZG_BIT            0   // Z轴陀螺仪待机模式

/*────────────────── 唤醒频率配置 ────────────────────*/
#define MPU6050_WAKE_FREQ_1P25      0x0  // 1.25Hz 低功耗唤醒频率
#define MPU6050_WAKE_FREQ_2P5       0x1  // 2.5Hz 运动检测采样率
#define MPU6050_WAKE_FREQ_5         0x2  // 5Hz 中等功耗模式
#define MPU6050_WAKE_FREQ_10        0x3  // 10Hz 常规工作模式

/*────────────────── 存储库选择配置 ───────────────────*/
/* 寄存器地址：0x6D（BANK_SEL） */
#define MPU6050_BANKSEL_PRFTCH_EN_BIT       6   // DMP预取使能
#define MPU6050_BANKSEL_CFG_USER_BANK_BIT   5   // 用户存储库选择模式
#define MPU605EL_MEM_SEL_BIT                4   // 内存库选择起始位
#define MPU6050_BANKSEL_MEM_SEL_LENGTH      5   // 内存库选择字段（共32库）

/*────────────────── 设备识别与任务参数 ────────────────*/
#define MPU6050_WHO_AM_I_BIT        6   // 设备ID起始位（寄存器0x75）
#define MPU6050_WHO_AM_I_LENGTH     6   // 设备ID字段长度（固定值0x68）

#define MPU6050_TASK_PRIO        5    // RTOS任务优先级（范围0-31）
#define MPU6050_STK_SIZE         256   // 任务堆栈大小（单位：字）

/*-----------------------------------------------------------------------------
 * 模块功能说明：
 * 本头文件定义MPU6050传感器的数据结构和操作接口
 * 包含陀螺仪/加速度计原始数据访问、传感器初始化、DMP配置、温度读取等功能
 * 提供FreeRTOS任务接口用于传感器数据采集
 *---------------------------------------------------------------------------*/

// 陀螺仪三轴原始角速度值（单位：dps，通过ADC读取的原始数据）
// [0]:X轴 [1]:Y轴 [2]:Z轴
extern short gyro[3], accel[3];  

// 传感器校准偏差计数器（记录校准迭代次数）
extern int Deviation_Count;

// 陀螺仪校准偏差存储数组（单位：LSB）
// 存储传感器校准过程中计算的零点偏移值
extern short Deviation_gyro[3],Original_gyro[3]; 

// 加速度计校准偏差存储数组（单位：LSB）
// 存储加速度计校准基准值和偏移量
extern short Deviation_accel[3],Original_accel[3]; 

// 陀螺仪各轴零点偏移补偿值（单位：LSB）
// 用于实时数据输出的补偿计算
extern int16_t Gx_offset,Gy_offset,Gz_offset;

// 加速度归一化基准值（单位：g）
// 表示传感器1g加速度对应的原始数值
extern float Acc1G_Values;

// 欧拉角输出（单位：度）
// 通过DMP解算得到的姿态角度
extern float Roll,Pitch,Yaw;  // Roll:横滚角 Pitch:俯仰角 Yaw:偏航角

/********************************* 传感器操作API ********************************/

/**
 * @brief 初始化MPU6050传感器
 * @return u8 初始化状态 
 *         @0 成功 
 *         @非0 错误代码（详见传感器手册）
 * @detail 执行以下操作：
 *         1. I2C通信检测
 *         2. 寄存器配置
 *         3. 传感器自检
 *         4. 校准参数加载
 */
u8 MPU6050_initialize(void); 

/**
 * @brief 检测MPU6050设备连接状态
 * @return uint8_t 连接状态
 *         @1 设备正常响应
 *         @0 通信失败
 * @detail 通过WHO_AM_I寄存器验证设备ID
 */
uint8_t MPU6050_testConnection(void);

/**
 * @brief 读取六轴原始数据（加速度+陀螺仪）
 * @param ax 加速度X轴输出指针（单位：LSB）
 * @param ay 加速度Y轴输出指针
 * @param az 加速度Z轴输出指针
 * @param gx 陀螺仪X轴输出指针
 * @param gy 陀螺仪Y轴输出指针
 * @param gz 陀螺仪Z轴输出指针
 * @detail 直接读取传感器寄存器原始值，需配合校准参数使用
 *         数据未经过滤波处理
 */
void MPU6050_getMotion6(int16_t* ax, int16_t* ay, int16_t* az, 
                       int16_t* gx, int16_t* gy, int16_t* gz);

/**
 * @brief 读取最近一次有效六轴数据
 * @param ax 加速度X轴输出指针（单位：LSB）
 * @param ay 加速度Y轴输出指针
 * @param az 加速度Z轴输出指针
 * @param gx 陀螺仪X轴输出指针
 * @param gy 陀螺仪Y轴输出指针
 * @param gz 陀螺仪Z轴输出指针
 * @detail 从缓存区获取数据，适用于需要数据同步的场景
 *         数据已应用最近一次校准参数
 */
void MPU6050_getlastMotion6(int16_t* ax, int16_t* ay, 
                           int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);

/**
 * @brief 获取设备ID
 * @return uint8_t WHO_AM_I寄存器值（正常应为0x68）
 */
uint8_t MPU6050_getDeviceID(void);

/**
 * @brief 陀螺仪零偏校准初始化
 * @detail 执行以下操作：
 *         1. 进入校准模式
 *         2. 采集静态数据样本
 *         3. 计算平均偏差
 *         4. 更新Deviation_gyro数组
 */
void MPU6050_InitGyro_Offset(void);

/**
 * @brief 初始化数字运动处理器(DMP)
 * @detail 配置流程：
 *         1. 加载DMP固件
 *         2. 设置传感器融合参数
 *         3. 启用四元数输出
 *         4. 配置中断引脚
 */
void DMP_Init(void);

/**
 * @brief 读取DMP处理后的姿态数据
 * @detail 获取以下数据：
 *         1. 四元数(Q16.16格式)
 *         2. 欧拉角（写入Roll/Pitch/Yaw）
 *         3. 步数统计（如果启用）
 */
void Read_DMP(void);

/**
 * @brief 读取芯片温度
 * @return int 温度值（单位：0.1摄氏度）
 * @detail 转换公式：
 *         Temperature_degC = (TEMP_OUT / 340.0) + 36.53
 */
int Read_Temperature(void);

/**
 * @brief FreeRTOS传感器任务入口
 * @param pvParameters 任务参数指针
 * @detail 任务功能：
 *         1. 周期读取传感器数据
 *         2. 执行数据滤波
 *         3. 更新全局变量
 *         4. 处理低功耗模式
 */
void MPU6050_task(void *pvParameters);

/**
 * @brief 设置数字低通滤波器(DLPF)
 * @param lpf 滤波器带宽（单位：Hz）
 *         @可选值：0-6（详见MPU6050_Register 26）
 * @return unsigned char 设置状态
 *         @0 成功
 *         @1 参数错误
 */
unsigned char MPU6050_Set_LPF(u16 lpf);

/**
 * @brief 设置采样率分频器
 * @param rate 分频系数（实际采样率 = 1kHz / (1 + rate)）
 * @return unsigned char 设置状态
 *         @0 成功
 *         @1 参数错误
 */
unsigned char MPU6050_Set_Rate(u16 rate);

/**
 * @brief 获取校准后的陀螺仪数据（应用Gx_offset等补偿）
 * @detail 操作流程：
 *         1. 读取原始数据
 *         2. 应用温度补偿
 *         3. 更新gyro数组
 */
void MPU_Get_Gyroscope(void);

/**
 * @brief 获取校准后的加速度数据
 * @detail 操作流程：
 *         1. 读取原始数据
 *         2. 应用Acc1G_Values标定
 *         3. 更新accel数组
 */
void MPU_Get_Accelscope(void);


#endif

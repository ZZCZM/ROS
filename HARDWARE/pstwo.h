/**
  ******************************************************************************
  * @file   pstwo.h
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
#ifndef __PSTWO_H
#define __PSTWO_H
#include "delay.h"
#include "sys.h"
#include "system.h"
#include "F4_flash.h"

/****** PS2手柄任务配置 ******/
#define PS2_TASK_PRIO        4       // FreeRTOS任务优先级（0-31，数值越大优先级越高）
#define PS2_STK_SIZE         256     // 任务堆栈深度（单位：字，实际占用256*4=1024字节）

/****** 硬件接口定义（SPI通信协议） ******/
#define DI   PEin(15)      // 数据输入引脚（MISO）
#define DO_H PEout(12)=1   // 数据输出高电平（MOSI）
#define DO_L PEout(12)=0   // 数据输出低电平
#define CS_H PEout(10)=1   // 片选信号高（无效）
#define CS_L PEout(10)=0   // 片选信号低（有效）
#define CLK_H PEout(8)=1   // 时钟信号上升沿
#define CLK_L PEout(8)=0   // 时钟信号下降沿

/****** 按钮键值定义（对应数据帧位映射） ******/
/* 基础功能键 */
#define PSB_SELECT      1   // 选择键
#define PSB_L3          2   // 左摇杆下压
#define PSB_R3          3   // 右摇杆下压
#define PSB_START       4   // 开始键
/* 方向键 */
#define PSB_PAD_UP      5   // 上方向
#define PSB_PAD_RIGHT   6   // 右方向
#define PSB_PAD_DOWN    7   // 下方向
#define PSB_PAD_LEFT    8   // 左方向
/* 肩部按键 */
#define PSB_L2          9   // 左前肩键
#define PSB_R2          10  // 右前肩键
#define PSB_L1          11  // 左后肩键
#define PSB_R1          12  // 右后肩键
/* 彩色功能键（两种命名方式） */
#define PSB_GREEN       13  // △/三角键（绿色）
#define PSB_RED         14  // ○/圆圈键（红色）
#define PSB_BLUE        15  // ×/叉键（蓝色）
#define PSB_PINK        16  // □/方块键（粉色）
#define PSB_TRIANGLE    13  // 同PSB_GREEN
#define PSB_CIRCLE      14  // 同PSB_RED
#define PSB_CROSS       15  // 同PSB_BLUE
#define PSB_SQUARE      16  // 同PSB_PINK

/****** 摇杆数据索引（Data数组下标） ******/
#define PSS_RX 5   // 右摇杆X轴（0-255，128中点）
#define PSS_RY 6   // 右摇杆Y轴
#define PSS_LX 7   // 左摇杆X轴
#define PSS_LY 8   // 左摇杆Y轴

/****** 全局变量声明 ******/
extern u8 Data[9];           // PS2原始数据帧（包含按钮和摇杆数据）
extern u16 MASK[16];         // 按钮位掩码数组（用于快速状态检测）
extern u16 Handkey;          // 当前按下按钮的位图（按位或组合）
extern int Beep_Control_Flag;// 震动反馈控制标志（0-关闭 1-开启）
/****** PS2手柄核心控制函数 ******/
void PS2_Read(void);         // 执行完整数据读取周期（包括CS信号控制）
void PS2_Init(void);         // 初始化SPI接口及GPIO（默认配置为数字模式）
u8 PS2_RedLight(void);       // 检测连接状态（返回0:已连接 1:未连接）

/****** 数据获取与处理 ******/
void PS2_ReadData(void);     // 读取9字节原始数据到Data数组（需先拉低CS）
u8 PS2_DataKey(void);        // 解析按钮状态（返回16位键值，每位对应一个按键）
u8 PS2_AnologData(u8 button);// 获取模拟量（button参数见PSS_RX等宏）
void PS2_ClearData(void);    // 清空数据缓存（Data数组清零，Handkey复位）

/****** 设备模式配置 ******/
void PS2_EnterConfing(void);    // 进入配置模式（允许修改设备参数）
void PS2_TurnOnAnalogMode(void);// 启用模拟模式（获取摇杆连续值）
void PS2_VibrationMode(void);   // 启用震动功能（需配置模式支持）
void PS2_ExitConfing(void);     // 退出配置模式（保存参数到手柄）
void PS2_SetInit(void);         // 恢复出厂默认设置（需在配置模式下调用）

/****** 震动控制 ******/
void PS2_Vibration(u8 motor1, u8 motor2); 
/* 参数说明：
   - motor1: 小马达强度（0-255，0停止）
   - motor2: 大马达强度（0-255，0停止） */

/****** 系统集成函数 ******/
void PS2_Receive(void);       // 综合接收处理（包含错误重试机制）
void pstwo_task(void *pvParameters); // FreeRTOS任务函数（建议100Hz运行）

/****** 舵机校准扩展功能 ******/
void Servo_init_angle_adjust_mode_check(void); 
// 检测微调模式触发（长按SELECT+START进入）
void Servo_init_angle_adjust_function(void);  
// 摇杆控制舵机角度调整（左右摇杆对应不同舵机）

#endif






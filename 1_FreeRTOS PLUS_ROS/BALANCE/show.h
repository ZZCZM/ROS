/**
  ******************************************************************************
  * @file   show.h
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
#ifndef __SHOW_H
#define __SHOW_H
#include "sys.h"
#include "oled.h"
#include "system.h"
/* 人机交互显示模块头文件 - 定义显示任务参数及显示接口 */

// 显示任务优先级配置（FreeRTOS优先级，0-31，数值越大优先级越高）
#define SHOW_TASK_PRIO      2    // 低于关键控制任务，高于非实时任务
// 显示任务堆栈配置（单位：字，FreeRTOS堆栈计量单位）
#define SHOW_STK_SIZE       512   // 适应图形渲染需求，建议最小400字

/**
 * @brief 显示系统主任务（FreeRTOS任务函数）
 * @param pvParameters 任务参数指针
 * @note 功能说明：
 * - 200ms周期刷新低优先级显示内容
 * - 50ms周期刷新关键数据（如传感器读数）
 * - 协调OLED/LCD显示设备切换
 * - 处理屏幕背光超时关闭逻辑
 */
void Show_task(void *pvParameters);

/* 显示设备驱动层接口 */
/**
 * @brief OLED屏幕全量刷新（128x64像素）
 * @note 实现功能：
 * - 四区域布局：状态栏/波形区/参数区/调试信息
 * - 支持反色显示告警信息
 * - 帧率限制：最大30fps
 */
void oled_show(void);

/**
 * @brief LCD屏幕全量刷新（320x240像素）
 * @note 实现功能：
 * - 多页面支持：状态页/曲线页/配置页
 * - 触摸事件处理中转
 * - 帧率限制：最大15fps（受限于FSMC总线速度）
 */
void lcd_show(void);

/* 应用层显示接口 */
/**
 * @brief 核心数据可视化接口（被控制任务调用）
 * @note 显示内容：
 * - 电机实际转速 vs 目标转速
 * - 陀螺仪姿态角波形
 * - 系统运行时长统计
 * - 网络连接状态指示
 */
void APP_Show(void);

/* 设备自检显示流程 */
/**
 * @brief 自检确认提示界面（等待用户按键确认）
 * @note 显示元素：
 * - "PRESS TO START TEST" 闪烁提示
 * - 倒计时进度条（10秒超时）
 */
void OLED_ShowCheckConfirming(void);

/**
 * @brief 自检执行过程界面（动态进度显示）
 * @note 显示元素：
 * - 旋转菊花动画
 * - 当前检测项目名称（电机/编码器/IMU等）
 * - 已完成的检测项目打钩
 */
void OLED_ShowChecking(void);

/**
 * @brief 自检结果报告界面（颜色编码结果）
 * @note 显示规则：
 * - 绿色：全部项目通过
 * - 黄色：警告项存在
 * - 红色：关键检测失败
 * - 显示详细故障代码（如E101:IMU_TIMEOUT）
 */
void OLED_ShowCheckResult(void);

#endif

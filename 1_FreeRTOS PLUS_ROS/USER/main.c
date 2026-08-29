/**
  ******************************************************************************
  * @file    main.c
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
/*----------------------------------------------------------------------------*/
#define Test_NUM 0 //条件编译控制 1:创新点2代码,0:正常代码 

/* 任务优先级定义（数值越小优先级越低） */
#define START_TASK_PRIO	1       // 启动任务优先级（最低）
#define Innovation_Point_2_Test_Task_PRIO		5     //Task priority //任务优先级

/* 任务堆栈大小定义（单位：字，32位系统为4字节） */
#define START_STK_SIZE 	256     // 启动任务堆栈
#define Innovation_Point_2_Test_STK_SIZE 		512   //Task stack size //任务堆栈大小

/* 任务句柄声明 */
TaskHandle_t StartTask_Handler;          // 启动任务句柄
extern TaskHandle_t MainControlTask_Handler; // 外部声明主控任务句柄

/* 任务函数原型声明 */
void start_task(void *pvParameters);     // 启动任务入口函数

/*----------------------------------------------------------------------------*/
/* 主函数：系统初始化与任务调度启动 */
int main(void)
{ 
    /* 硬件初始化（时钟、外设、中断等） */
    systemInit(); 

    /* 电源状态检测与恢复 */
    if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
    {
        // 从待机模式唤醒后的处理
        PWR_ClearFlag(PWR_FLAG_SB);      // 清除待机标志位
        // 可在此添加状态恢复代码（如外设重初始化）
    }

    /* 创建启动任务 */
    xTaskCreate(
        (TaskFunction_t )start_task,     // 任务函数指针
        (const char*    )"start_task",   // 任务名称（调试用）
        (uint16_t       )START_STK_SIZE,  // 堆栈深度（256*4=1024字节）
        (void*          )NULL,           // 任务参数（无）
        (UBaseType_t    )START_TASK_PRIO,// 任务优先级
        (TaskHandle_t*  )&StartTask_Handler // 任务句柄存储地址
    );

    /* 启动FreeRTOS任务调度器 */
    vTaskStartScheduler();  // 永不返回，除非发生异常

    /* 理论上不会执行到这里 */
    while(1);
}

/*----------------------------------------------------------------------------*/
/* 启动任务函数说明：
   功能：系统初始化后首个执行任务，负责创建所有应用任务并自我删除
   作用：
   1. 在临界区内完成关键任务创建，确保初始化过程不被中断
   2. 根据系统需求创建不同优先级的功能任务
   3. 自身作为临时任务，完成任务创建后立即删除释放资源
   4. 管理任务间优先级分配和资源协调
----------------------------------------------------------------------------*/
void start_task(void *pvParameters)
{
    /* 进入临界区（关闭全局中断）*/
    taskENTER_CRITICAL(); // 保护任务创建过程，防止任务切换导致的不一致
    
    /*----------------------- 核心任务创建流程 ------------------------*/
    // 创建平衡控制任务（关键实时任务）
    xTaskCreate(Balance_task,          // 任务函数指针：平衡算法控制
                "Balance_task",        // 任务名称（用于调试识别）
                BALANCE_STK_SIZE,      // 堆栈大小：128字（128*4=512字节）
                NULL,                  // 任务参数：无
                BALANCE_TASK_PRIO,     // 优先级：5（较高优先级）
                NULL);                 // 任务句柄：不保留句柄
    
    // 创建MPU6050传感器数据采集任务
    xTaskCreate(MPU6050_task,          // 任务函数：多轴传感器数据读取
                "MPU6050_task",        // 任务名称
                MPU6050_STK_SIZE,      // 堆栈：128字
                NULL,                  // 参数
                MPU6050_TASK_PRIO,     // 优先级：5（与平衡任务同级，时间片轮转）
                NULL);
    
    // 创建显示刷新任务（低优先级）
    xTaskCreate(Show_task,             // 显示内容更新函数
                "Show_task",           // 任务名称
                SHOW_STK_SIZE,         // 堆栈：64字（256字节）
                NULL,                  // 参数
                SHOW_TASK_PRIO,        // 优先级：2（低优先级）
                NULL);                 // 仅在系统空闲时运行
    
    // 创建LED状态指示任务
    xTaskCreate(Led_task,              // LED控制函数
                "Led_task",            // 任务名称
                LED_STK_SIZE,          // 堆栈：64字
                NULL,                  // 参数
                LED_TASK_PRIO,         // 优先级：3（中等）
                NULL);                 // 用于系统状态指示
    
		#if (Test_NUM==0)
		{
			// 创建串口数据发送任务
			xTaskCreate(TX_data_task,          // 数据打包发送函数
									"TX_DATA_task",        // 任务名称
									TX_DATA_TASK_SIZE,     // 堆栈：128字
									NULL,                  // 参数
									TX_DATA_TASK_PRIO,     // 优先级：4
									NULL);                 // 20Hz周期发送
			
			// 创建串口数据接收任务（事件驱动）
			xTaskCreate(Rx_data_task,          // 数据接收解析函数
									"RX_DATA_task",        // 任务名称
									RX_DATA_TASK_SIZE,     // 堆栈：128字
									NULL,                  // 参数
									RX_DATA_TASK_PRIO,      // 优先级：4（与发送任务同级）
									NULL);                 // 基于中断事件触发
		}
		#elif (Test_NUM==1)
    {
			/* 关闭推迟合并功能 */
			vPortSetDeferredCoalescing( pdFALSE );//让start_task任务不影响测试结果(vTaskDelete函数会调用vPortFree函数)
			// 主控制任务（保留扩展接口）
			 xTaskCreate(prvInnovation_Point_2_Test_Task,  
									"Innovation_Point_2_Test_Task", 
									 Innovation_Point_2_Test_STK_SIZE,  
									 NULL,  
									 Innovation_Point_2_Test_Task_PRIO,  
									 (TaskHandle_t*)&MainControlTask_Handler); // 使用句柄便于任务管理
		}
		#endif
    /*--------------------- 保留功能（当前注释禁用） --------------------*/

    
    // PS2手柄控制任务（备用输入）
    // xTaskCreate(pstwo_task,    
    //             "PSTWO_task",    
    //             PS2_STK_SIZE,      
    //             NULL,      
    //             PS2_TASK_PRIO,      
    //             NULL);    // 优先级4，用于扩展控制输入

    /*----------------------- 任务创建完成 ------------------------*/
    // 删除启动任务自身（参数：当前任务句柄）
    vTaskDelete(StartTask_Handler);  // 释放TCB和堆栈资源
    
    /* 退出临界区（恢复全局中断）*/
    taskEXIT_CRITICAL(); // 所有关键任务已创建，恢复系统正常中断
}


/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


/*
 * Implementation of pvPortMalloc() and vPortFree() that relies on the
 * compilers own malloc() and free() implementations.
 *
 * This file can only be used if the linker is configured to to generate
 * a heap memory area.
 *
 * See heap_1.c, heap_2.c and heap_4.c for alternative implementations, and the
 * memory management pages of http://www.FreeRTOS.org for more information.
 */

#include <stdlib.h>
/*-----------------------------------------------------------*/
/**
 * @file heap_3.c
 * @brief FreeRTOS内存管理方案3的适配层实现
 * 
 * 方案特性：
 * 1. 基于标准库malloc/free的桥接实现
 * 2. 依赖编译器自带的内存管理
 * 3. 提供线程安全包装
 * 4. 适用于有成熟内存管理的系统
 */

/* 防止MPU包装器被重复定义 */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 包含内核核心头文件 */
#include "FreeRTOS.h"   // 内核配置和数据类型
#include "task.h"      // 任务管理相关声明

/* 取消MPU包装器定义限制 */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 动态分配支持检查 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
    #error 本实现需要configSUPPORT_DYNAMIC_ALLOCATION=1
#endif

/*-----------------------------------------------------------*/
/**
 * @brief 标准库分配器的安全封装
 * @param xWantedSize 请求分配字节数
 * @return void* 分配的内存地址或NULL
 * 
 * 功能说明：
 * 1. 挂起调度器保证线程安全
 * 2. 调用标准库malloc实现分配
 * 3. 提供分配失败钩子支持
 */
void *pvPortMalloc( size_t xWantedSize )
{
void *pvReturn;  // 返回值指针

    /* 挂起所有任务保证原子操作 */
    vTaskSuspendAll();
    {
        /* 调用标准库内存分配 */
        pvReturn = malloc( xWantedSize );  // 使用编译器自带分配器
        
        /* 内存分配追踪（如果启用） */
        traceMALLOC( pvReturn, xWantedSize );  // 记录分配信息
    }
    /* 恢复任务调度 */
    ( void ) xTaskResumeAll();

    /* 分配失败处理钩子 */
    #if( configUSE_MALLOC_FAILED_HOOK == 1 )
    {
        if( pvReturn == NULL )
        {
            extern void vApplicationMallocFailedHook( void );
            vApplicationMallocFailedHook();  // 触发用户定义回调
        }
    }
    #endif

    return pvReturn;
}

/*-----------------------------------------------------------*/
/**
 * @brief 标准库释放器的安全封装
 * @param pv 需要释放的内存指针
 * 
 * 实现说明：
 * 1. 空指针安全检查
 * 2. 线程安全的释放操作
 * 3. 释放操作追踪支持
 */
void vPortFree( void *pv )
{
    if( pv )  // 空指针防御
    {
        /* 挂起所有任务保证原子操作 */
        vTaskSuspendAll();
        {
            /* 调用标准库内存释放 */
            free( pv );  // 使用编译器自带回收器
            
            /* 内存释放追踪（如果启用） */
            traceFREE( pv, 0 );  // 记录释放信息
        }
        /* 恢复任务调度 */
        ( void ) xTaskResumeAll();
    }
}


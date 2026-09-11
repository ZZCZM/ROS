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
 * The simplest possible implementation of pvPortMalloc().  Note that this
 * implementation does NOT allow allocated memory to be freed again.
 *
 * See heap_2.c, heap_3.c and heap_4.c for alternative implementations, and the
 * memory management pages of http://www.FreeRTOS.org for more information.
 */
#include <stdlib.h>
/*-----------------------------------------------------------*/
/**
 * @file heap_1.c
 * @brief FreeRTOS内存管理方案1的初始化配置
 * 
 * 方案特性：
 * 1. 最简单的内存分配实现
 * 2. 仅支持单次分配（不支持内存释放）
 * 3. 适用于不需要动态内存回收的场景
 * 4. 通过静态数组实现堆空间管理
 */

/* 防止MPU包装器被重复定义（当从内核代码包含API时） */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 包含内核核心头文件 */
#include "FreeRTOS.h"   // 内核配置和数据类型定义
#include "task.h"       // 任务管理相关声明

/* 取消MPU包装器定义限制 */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 动态分配支持检查（本文件需要启用动态分配） */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
    #error 本实现需要configSUPPORT_DYNAMIC_ALLOCATION=1
#endif

/* 堆空间调整（补偿地址对齐造成的空间损失） */
#define configADJUSTED_HEAP_SIZE    ( configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT )

/*-----------------------------------------------------------*/
/* 堆存储区定义（两种配置模式） */
#if( configAPPLICATION_ALLOCATED_HEAP == 1 )
    /* 外部声明模式：允许应用自定义堆位置 */
    extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  // 由用户定义存储位置的堆数组
#else
    /* 静态分配模式：默认在BSS段分配 */
    static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  // 系统自动分配堆数组
#endif

/* 堆空间分配指针（跟踪可用内存位置） */
static size_t xNextFreeByte = ( size_t ) 0;  // 初始指向堆起始位置


/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/**
 * @brief 执行单向内存分配（heap_1方案）
 * @param xWantedSize 请求分配的字节数
 * @return void* 成功时返回分配地址，失败返回NULL
 * 
 * 特点：
 * 1. 仅支持顺序分配，不可释放内存
 * 2. 自动处理内存对齐
 * 3. 线程安全但效率较低
 * 4. 适用于初始化阶段的永久内存分配
 */
void *pvPortMalloc( size_t xWantedSize )
{
void *pvReturn = NULL;                    // 返回值初始化
static uint8_t *pucAlignedHeap = NULL;    // 对齐后的堆起始地址（静态缓存）

    /* 内存对齐处理（当对齐要求>1字节时） */
    #if( portBYTE_ALIGNMENT != 1 )
    {
        /* 检查请求大小是否符合对齐要求 */
        if( xWantedSize & portBYTE_ALIGNMENT_MASK )
        {
            /* 计算需要增加的对齐补偿值 
            示例：当请求27字节且对齐要求8字节时：
            portBYTE_ALIGNMENT - (27 & 0x07) = 8 - 3 = 5
            最终分配大小 = 27 + 5 = 32 */
            xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
        }
    }
    #endif

    /* 挂起任务调度保证原子操作 */
    vTaskSuspendAll();
    {
        /* 首次调用时初始化对齐地址 */
        if( pucAlignedHeap == NULL )
        {
            /* 计算堆起始对齐地址：
            1. 取ucHeap[portBYTE_ALIGNMENT]的地址（跳过初始不对齐部分）
            2. 执行位操作清除低位实现对齐 */
            pucAlignedHeap = ( uint8_t * )( 
                ( ( portPOINTER_SIZE_TYPE ) &ucHeap[ portBYTE_ALIGNMENT ] ) 
                & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) 
            );
        }

        /* 内存空间可用性检查（双重验证） */
        if( ( ( xNextFreeByte + xWantedSize ) < configADJUSTED_HEAP_SIZE ) &&  // 容量检查
            ( ( xNextFreeByte + xWantedSize ) > xNextFreeByte ) )              // 溢出检查
        {
            /* 计算分配地址并更新指针 */
            pvReturn = pucAlignedHeap + xNextFreeByte;  // 计算实际内存地址
            xNextFreeByte += xWantedSize;               // 移动空闲指针
        }

        /* 内存分配追踪（如果启用） */
        traceMALLOC( pvReturn, xWantedSize );
    }
    /* 恢复任务调度 */
    ( void ) xTaskResumeAll();

    /* 分配失败处理钩子 */
    #if( configUSE_MALLOC_FAILED_HOOK == 1 )
    {
        if( pvReturn == NULL )
        {
            extern void vApplicationMallocFailedHook( void );
            vApplicationMallocFailedHook();  // 调用用户定义的处理函数
        }
    }
    #endif

    return pvReturn;
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/**
 * @brief 内存释放函数（空实现）
 * @param pv 需要释放的内存指针
 * 
 * 特别说明：
 * 1. heap_1方案不支持内存释放操作
 * 2. 调用此函数将触发断言错误
 * 3. 实际工程中需要移除所有free调用
 */
void vPortFree( void *pv )
{
    /* 方案限制说明（编译时提示） */
    ( void ) pv;  // 显式忽略参数避免警告
    
    /* 强制断言触发（运行时错误检测） */
    configASSERT( pv == NULL );  // 任何非空参数都会触发断言
}

/*-----------------------------------------------------------*/
/**
 * @brief 堆内存初始化函数
 * 
 * 功能：
 * 重置内存分配指针到堆起始位置
 * 适用场景：
 * 1. 系统冷启动
 * 2. 需要完全重置内存状态
 * 3. 配合内存清零操作使用
 */
void vPortInitialiseBlocks( void )
{
    /* 复位分配指针到初始位置 */
    xNextFreeByte = ( size_t ) 0;  // 相当于内存管理系统软复位
}

/*-----------------------------------------------------------*/
/**
 * @brief 获取当前剩余堆空间
 * @return size_t 可用字节数
 * 
 * 计算公式：
 * 剩余空间 = 调整后堆大小 - 已分配字节数
 * 
 * 注意：
 * 返回值包含因对齐产生的碎片空间
 */
size_t xPortGetFreeHeapSize( void )
{
    /* 计算未分配的内存容量 */
    return ( configADJUSTED_HEAP_SIZE - xNextFreeByte ); 
}




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

/*-----------------------------------------------------------
 * Portable layer API.  Each function must be defined for each port.
 *----------------------------------------------------------*/

#ifndef PORTABLE_H
#define PORTABLE_H

/* Each FreeRTOS port has a unique portmacro.h header file.  Originally a
pre-processor definition was used to ensure the pre-processor found the correct
portmacro.h file for the port being used.  That scheme was deprecated in favour
of setting the compiler's include path such that it found the correct
portmacro.h file - removing the need for the constant and allowing the
portmacro.h file to be located anywhere in relation to the port being used.
Purely for reasons of backward compatibility the old method is still valid, but
to make it clear that new projects should not use it, support for the port
specific constants has been moved into the deprecated_definitions.h header
file. */
#include "deprecated_definitions.h"

/* If portENTER_CRITICAL is not defined then including deprecated_definitions.h
did not result in a portmacro.h header file being included - and it should be
included here.  In this case the path to the correct portmacro.h header file
must be set in the compiler's include path. */
#ifndef portENTER_CRITICAL
	#include "portmacro.h"
#endif
/* 内存对齐配置检测 */
#if portBYTE_ALIGNMENT == 32       /* 32字节对齐（适用于256位总线架构）*/
    #define portBYTE_ALIGNMENT_MASK ( 0x001f ) /* 掩码=32-1=0x1F (二进制00011111) */
#endif

#if portBYTE_ALIGNMENT == 16       /* 16字节对齐（SIMD指令集优化） */
    #define portBYTE_ALIGNMENT_MASK ( 0x000f ) /* 16-1=0xF (00001111) */
#endif

#if portBYTE_ALIGNMENT == 8        /* 8字节对齐（64位系统标准配置） */
    #define portBYTE_ALIGNMENT_MASK ( 0x0007 ) /* 8-1=7 (00000111) */
#endif

#if portBYTE_ALIGNMENT == 4        /* 4字节对齐（32位系统标准配置） */
    #define portBYTE_ALIGNMENT_MASK ( 0x0003 ) /* 4-1=3 (00000011) */
#endif

#if portBYTE_ALIGNMENT == 2        /* 2字节对齐（16位嵌入式系统） */
    #define portBYTE_ALIGNMENT_MASK ( 0x0001 ) /* 2-1=1 (00000001) */
#endif

#if portBYTE_ALIGNMENT == 1        /* 无对齐要求（最低兼容模式） */
    #define portBYTE_ALIGNMENT_MASK ( 0x0000 ) /* 1-1=0 (00000000) */
#endif

/* 对齐配置有效性检查 */
#ifndef portBYTE_ALIGNMENT_MASK
    #error "Invalid portBYTE_ALIGNMENT definition" 
    /* 触发条件：portBYTE_ALIGNMENT未设置为1/2/4/8/16/32中的任一值 */
#endif

/* MPU可配置区域默认值 */
#ifndef portNUM_CONFIGURABLE_REGIONS
    #define portNUM_CONFIGURABLE_REGIONS 1  /* 默认每个任务1个可配置内存区域 */
    /* 典型配置值：
       - Cortex-M7 MPU：8 
       - 无MPU设备：1（兼容模式）
       - 需要内存保护时：≥3（代码区/数据区/堆栈区） */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "mpu_wrappers.h"
/*------------------------ 任务栈初始化函数 ------------------------*/
/*
 * 初始化新任务的栈结构，准备调度器接管
 * 寄存器按移植层要求的顺序压栈
 */
#if( portUSING_MPU_WRAPPERS == 1 )
/* MPU版本：包含特权级参数 */
StackType_t *pxPortInitialiseStack( 
    StackType_t *pxTopOfStack,        // [in] 栈顶指针（通常为高地址端）
    TaskFunction_t pxCode,            // [in] 任务函数入口地址
    void *pvParameters,               // [in] 传递给任务的参数指针
    BaseType_t xRunPrivileged         // [in] 特权模式标记（pdTRUE特权模式）
) PRIVILEGED_FUNCTION;
#else
/* 非MPU版本 */
StackType_t *pxPortInitialiseStack(
    StackType_t *pxTopOfStack,
    TaskFunction_t pxCode, 
    void *pvParameters 
) PRIVILEGED_FUNCTION;
#endif

/*------------------------ 堆内存区域定义 ------------------------*/
/* 用于heap_5.c的多堆区管理 */
typedef struct HeapRegion {
    uint8_t *pucStartAddress;        // 堆区起始地址（必须对齐）
    size_t xSizeInBytes;             // 堆区大小（单位：字节）
} HeapRegion_t;

/* 
 * 初始化多堆区内存管理系统
 * 必须在首次内存分配前调用（创建任何内核对象前）
 */
void vPortDefineHeapRegions(
    const HeapRegion_t * const pxHeapRegions // [in] 堆区配置数组，以零大小项结尾
) PRIVILEGED_FUNCTION;

/*------------------------ 内存管理API ------------------------*/
/* 以下函数为移植层必须实现的接口 */
void *pvPortMalloc(                   // 类似标准malloc
    size_t xSize                      // 申请内存大小（字节）
) PRIVILEGED_FUNCTION;               // 返回值保证对齐（portBYTE_ALIGNMENT）

void vPortFree(                       // 类似标准free
    void *pv                         // 要释放的内存指针
) PRIVILEGED_FUNCTION;

void vPortInitialiseBlocks( void          // 初始化内存块（heap_3.c需要）
) PRIVILEGED_FUNCTION;               // 在heap_4/5中为空实现

size_t xPortGetFreeHeapSize(void          // 获取当前空闲堆内存
) PRIVILEGED_FUNCTION;               // 单位：字节

size_t xPortGetMinimumEverFreeHeapSize(void // 获取历史最小空闲堆内存
) PRIVILEGED_FUNCTION;               // 用于检测内存泄漏

/*------------------------ 调度器控制 ------------------------*/
/* 启动调度器（初始化硬件资源） */
BaseType_t xPortStartScheduler(void      // 成功返回pdPASS，失败返回pdFAIL
) PRIVILEGED_FUNCTION;               // 初始化时钟中断/滴答定时器

/* 停止调度器（恢复硬件原始状态） */
void vPortEndScheduler(void              // 停止后无法恢复调度
) PRIVILEGED_FUNCTION;               // 关闭时钟中断/恢复默认中断处理

/*------------------------ MPU配置接口 ------------------------*/
#if( portUSING_MPU_WRAPPERS == 1 )
struct xMEMORY_REGION;
/* 存储任务的MPU配置 */
void vPortStoreTaskMPUSettings(
    xMPU_SETTINGS *xMPUSettings,      // [out] 输出MPU配置结构体
    const struct xMEMORY_REGION * const xRegions, // [in] 内存区域数组
    StackType_t *pxBottomOfStack,     // [in] 栈底指针（用于栈区保护）
    uint32_t ulStackDepth             // [in] 栈深度（单位：字）
) PRIVILEGED_FUNCTION;
#endif


#ifdef __cplusplus
}
#endif

#endif /* PORTABLE_H */


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

#ifndef STACK_MACROS_H
#define STACK_MACROS_H

/*
 * Call the stack overflow hook function if the stack of the task being swapped
 * out is currently overflowed, or looks like it might have overflowed in the
 * past.
 *
 * Setting configCHECK_FOR_STACK_OVERFLOW to 1 will cause the macro to check
 * the current stack state only - comparing the current top of stack value to
 * the stack limit.  Setting configCHECK_FOR_STACK_OVERFLOW to greater than 1
 * will also cause the last few stack bytes to be checked to ensure the value
 * to which the bytes were set when the task was created have not been
 * overwritten.  Note this second test does not guarantee that an overflowed
 * stack will always be recognised.
 */

/*-----------------------------------------------------------*/
/* 堆栈溢出检测宏集合 */
/* 根据不同的堆栈增长方向和检测级别实现不同的检测策略 */

/*-----------------------------------------------------------*/
/* 情况1：堆栈向下增长（高地址->低地址）且检测级别=1 */
#if( ( configCHECK_FOR_STACK_OVERFLOW == 1 ) && ( portSTACK_GROWTH < 0 ) )
    /**
     * @def taskCHECK_FOR_STACK_OVERFLOW
     * @brief 堆栈向下增长时的基础溢出检测（级别1）
     * 
     * 检测原理：
     * 检查当前栈顶指针是否越过栈底边界
     * 适用于ARM Cortex-M等向下增长的堆栈架构
     */
    #define taskCHECK_FOR_STACK_OVERFLOW()                                               \
    {                                                                                    \
        /* 比较当前栈顶指针与栈起始位置 */                                                \
        if( pxCurrentTCB->pxTopOfStack <= pxCurrentTCB->pxStack ) /* 栈顶<=栈底=溢出 */   \
        {                                                                                \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB,               \
                                            pxCurrentTCB->pcTaskName );                  \
        }                                                                                \
    }
#endif /* 结束条件编译 */
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* 情况2：堆栈向上增长（低地址->高地址）且检测级别=1 */
#if( ( configCHECK_FOR_STACK_OVERFLOW == 1 ) && ( portSTACK_GROWTH > 0 ) )
    /**
     * @def taskCHECK_FOR_STACK_OVERFLOW
     * @brief 堆栈向上增长时的基础溢出检测（级别1）
     * 
     * 检测原理：
     * 检查当前栈顶指针是否越过栈顶边界
     * 适用于X86等向上增长的堆栈架构
     */
    #define taskCHECK_FOR_STACK_OVERFLOW()                                               \
    {                                                                                    \
        /* 比较当前栈顶指针与栈结束位置 */                                                \
        if( pxCurrentTCB->pxTopOfStack >= pxCurrentTCB->pxEndOfStack ) /* 栈顶>=栈顶边界=溢出 */ \
        {                                                                                \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB,               \
                                            pxCurrentTCB->pcTaskName );                  \
        }                                                                                \
    }
#endif /* 结束条件编译 */
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* 情况3：堆栈向下增长且检测级别>1（增强检测） */
#if( ( configCHECK_FOR_STACK_GROWTH < 0 ) )
    /**
     * @def taskCHECK_FOR_STACK_OVERFLOW
     * @brief 堆栈向下增长时的魔数验证检测（级别2）
     * 
     * 检测原理：
     * 1. 在任务创建时用0xA5A5A5A5填充堆栈头部
     * 2. 运行时检测这些魔数是否被修改
     * 3. 可检测中间位置的溢出（比级别1更敏感）
     */
    #define taskCHECK_FOR_STACK_OVERFLOW()                                               \
    {                                                                                    \
        const uint32_t * const pulStack = ( uint32_t * ) pxCurrentTCB->pxStack;          \
        const uint32_t ulCheckValue = ( uint32_t ) 0xa5a5a5a5; /* 魔数标识 */            \
                                                                                         \
        /* 检查堆栈头部的4个魔数是否被覆盖 */                                             \
        if( ( pulStack[ 0 ] != ulCheckValue ) || /* 第1个32位字 */                       \
            ( pulStack[ 1 ] != ulCheckValue ) || /* 第2个32位字 */                       \
            ( pulStack[ 2 ] != ulCheckValue ) || /* 第3个32位字 */                       \
            ( pulStack[ 3 ] != ulCheckValue ) )  /* 第4个32位字 */                      \
        {                                                                                \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB,               \
                                            pxCurrentTCB->pcTaskName );                  \
        }                                                                                \
    }
#endif /* 结束条件编译 */
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
#if( ( configCHECK_FOR_STACK_OVERFLOW > 1 ) && ( portSTACK_GROWTH > 0 ) )

/**
 * @def taskCHECK_FOR_STACK_OVERFLOW
 * @brief 堆栈向上增长时的增强溢出检测（检测级别2）
 * 
 * 实现原理：
 * 1. 在堆栈末端保留20字节的填充模式（tskSTACK_FILL_BYTE）
 * 2. 运行时检查填充模式完整性
 * 3. 比级别1检测更早发现栈溢出风险
 * 
 * 内存布局（向上增长）：
 * +---------------------+ 高位地址
 * | 任务堆栈实际使用区域 |
 * | ...                 | 
 * | 20字节魔数填充区     | ← 检测区域开始(pcEndOfStack)
 * +---------------------+ 低位地址（pxEndOfStack初始位置）
 */
#define taskCHECK_FOR_STACK_OVERFLOW()                                                                             \
{                                                                                                                  \
    /* 获取堆栈结束地址指针（需转换为字节指针进行偏移计算）*/                                                       \
    int8_t *pcEndOfStack = ( int8_t * ) pxCurrentTCB->pxEndOfStack;                                                \
                                                                                                                   \
    /* 定义20字节的预期填充模式（tskSTACK_FILL_BYTE=0xA5）*/                                                       \
    static const uint8_t ucExpectedStackBytes[] = {                                                                \
        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, /* 第1-4字节 */            \
        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, /* 第5-8字节 */            \
        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, /* 第9-12字节 */           \
        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, /* 第13-16字节 */          \
        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE  /* 第17-20字节 */          \
    };                                                                                                             \
                                                                                                                   \
    /* 计算检测区域起始地址：从堆栈末端向前偏移20字节 */                                                           \
    pcEndOfStack -= sizeof( ucExpectedStackBytes );                                                               \
                                                                                                                   \
    /* 比对实际内存与预期填充模式 */                                                                                \
    if( memcmp( ( void * ) pcEndOfStack,                                                                           \
                ( void * ) ucExpectedStackBytes,                                                                   \
                sizeof( ucExpectedStackBytes ) ) != 0 )                                                            \
    {                                                                                                              \
        /* 填充模式被破坏时触发溢出回调 */                                                                          \
        vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB, pxCurrentTCB->pcTaskName );                   \
    }                                                                                                              \
}

#endif /* 结束条件编译 */
/*-----------------------------------------------------------*/

/* 未启用溢出检测时定义空宏 */
#ifndef taskCHECK_FOR_STACK_OVERFLOW
    #define taskCHECK_FOR_STACK_OVERFLOW()  /* 空实现 */
#endif



#endif /* STACK_MACROS_H */


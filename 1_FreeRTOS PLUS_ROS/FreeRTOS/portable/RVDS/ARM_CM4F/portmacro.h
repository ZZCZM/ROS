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


#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */
/*-----------------------------------------------------------
 * 文件功能：ARM Cortex-M4F 移植层核心配置
 * 作用说明：
 *   1. 定义处理器相关的数据类型
 *   2. 配置任务调度核心参数
 *   3. 实现上下文切换原语
 *   4. 提供中断控制接口
 *   注：本版本针对带FPU的Cortex-M4内核优化
 *-----------------------------------------------------------*/

/* 基础类型定义（跨平台兼容层）*/
#define portCHAR        char        /* 字符类型（保证8位宽度） */
#define portFLOAT        float       /* 单精度浮点（启用FPU时使用） */
#define portDOUBLE        double      /* 双精度浮点（默认不启用） */
#define portLONG        long        /* 32位长整型 */
#define portSHORT        short       /* 16位短整型 */
#define portSTACK_TYPE    uint32_t    /* 堆栈单元类型（32位对齐） */
#define portBASE_TYPE    long        /* 系统基础数据类型 */

/* FreeRTOS标准类型 */
typedef portSTACK_TYPE StackType_t;  /* 任务堆栈单元类型 */
typedef long BaseType_t;            /* 有符号基础类型（用于状态返回） */
typedef unsigned long UBaseType_t;  /* 无符号基础类型（用于计数） */

/* Tick计时系统配置 */
#if( configUSE_16_BIT_TICKS == 1 )
    /* 16位计时系统（低资源设备） */
    typedef uint16_t TickType_t;              /* 16位时间戳 */
    #define portMAX_DELAY ( TickType_t ) 0xffff /* 最大延迟65535 ticks */
#else
    /* 32位计时系统（默认配置） */
    typedef uint32_t TickType_t;              /* 32位时间戳 */
    #define portMAX_DELAY ( TickType_t ) 0xffffffffUL /* 最大延迟2^32-1 ticks */
    
    /* 原子性保障（32位架构优势） */
    #define portTICK_TYPE_IS_ATOMIC 1  /* tick读取在单指令内完成 */
#endif

/*-----------------------------------------------------------
 * 架构特性配置 */
#define portSTACK_GROWTH            ( -1 )  /* 堆栈方向：-1=向下生长（ARM标准） */
#define portTICK_PERIOD_MS            ( ( TickType_t ) 1000 / configTICK_RATE_HZ ) /* 计算tick周期 
    示例：configTICK_RATE_HZ=1000 → 1ms/tick */
#define portBYTE_ALIGNMENT            8     /* 内存分配对齐（8字节对齐） */

/* 内存屏障参数 */
#define portSY_FULL_READ_WRITE        ( 15 ) /* 全屏障模式：
    0b1111（数据+指令全方向屏障） */

/*-----------------------------------------------------------
 * 上下文切换原语 */
#define portYIELD()                                                    \
{                                                                      \
    /* 触发PendSV异常请求上下文切换 */                              \
    portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;                   \
                                                                       \
    /* 内存屏障确保操作可靠性 */                                    \
    __dsb( portSY_FULL_READ_WRITE );  /* 数据存储器屏障 */           \
    __isb( portSY_FULL_READ_WRITE );  /* 指令存储器屏障 */           \
}

/* NVIC寄存器映射 */
#define portNVIC_INT_CTRL_REG        ( * ( ( volatile uint32_t * ) 0xe000ed04 ) ) /* 中断控制状态寄存器：
    - 地址：0xE000ED04（Cortex-M系统控制块） */
#define portNVIC_PENDSVSET_BIT        ( 1UL << 28UL )  /* PendSV触发位掩码 */

/* 中断上下文切换控制 */
#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired != pdFALSE ) portYIELD() 
    /* 安全切换宏：
    - 参数必须为pdTRUE/pdFALSE
    - 典型用法：xQueueSendFromISR的第三个参数 */
#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )  /* 别名宏 */

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------
 * 功能模块：系统核心控制与优化
 * 作用说明：
 *   1. 实现多级中断屏蔽机制
 *   2. 支持Tickless低功耗模式
 *   3. 优化任务调度选择算法
 *-----------------------------------------------------------*/

/*-----------------------------------------------------------
 * 临界区管理接口 */
extern void vPortEnterCritical( void );  /* 嵌套式进入临界区：
    - 关闭可屏蔽中断
    - 维护嵌套计数器 */
extern void vPortExitCritical( void );   /* 嵌套式退出临界区：
    - 恢复嵌套计数器
    - 计数器归零时恢复中断 */

/* 中断控制宏 */
#define portDISABLE_INTERRUPTS()        vPortRaiseBASEPRI()  /* 提升中断屏蔽级别：
    - 设置BASEPRI为configMAX_SYSCALL_INTERRUPT_PRIORITY */
#define portENABLE_INTERRUPTS()        vPortSetBASEPRI(0)   /* 完全解除中断屏蔽：
    - 设置BASEPRI=0允许所有中断 */

/* 临界区操作封装 */
#define portENTER_CRITICAL()            vPortEnterCritical()  /* 进入临界区标准接口 */
#define portEXIT_CRITICAL()            vPortExitCritical()   /* 退出临界区标准接口 */

/* 中断上下文屏蔽管理 */
#define portSET_INTERRUPT_MASK_FROM_ISR()    ulPortRaiseBASEPRI()  /* 保存并提升屏蔽级别：
    - 返回原始BASEPRI值
    - 用于ISR中的临时屏蔽 */
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPortSetBASEPRI(x)  /* 恢复屏蔽级别：
    - 必须与SET宏成对使用 */

/*-----------------------------------------------------------
 * Tickless低功耗模式 */
#ifndef portSUPPRESS_TICKS_AND_SLEEP
    extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );  
    #define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime ) vPortSuppressTicksAndSleep( xExpectedIdleTime )
        /* 低功耗模式入口：
        - xExpectedIdleTime：预计空闲时间（tick数）
        - 需实现时钟补偿逻辑 */
#endif

/*-----------------------------------------------------------
 * 任务优先级位图优化 */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
    #define configUSE_PORT_OPTIMISED_TASK_SELECTION 1  /* 启用CLZ指令优化：
        ARM Cortex-M4的__clz指令加速查表 */
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
    /* 优先级数量兼容性检查 */
    #if( configMAX_PRIORITIES > 32 )
        #error configUSE_PORT_OPTIMISED_TASK_SELECTION仅支持≤32优先级
        /* 设计依据：
           - 32位变量位图限制
           - 实际系统极少需要超过15个优先级 */
    #endif

    /* 优先级位图操作 */
    #define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) \
        ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )  /* 设置优先级位：
           示例：优先级5 → 设置bit5 */
    
    #define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) \
        ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )  /* 清除优先级位 */

    /* 最高优先级查找算法 */
    #define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) \
        uxTopPriority = ( 31UL - ( uint32_t ) __clz( ( uxReadyPriorities ) ) ) 
            /* CLZ指令工作流程：
               输入：0x00008000（二进制第15位）
               输出：__clz=16 → 31-16=15（最高优先级） */
#endif
/*-----------------------------------------------------------
 * 功能模块：核心中断控制与编译器优化
 * 作用说明：
 *   1. 定义任务函数标准原型
 *   2. 实现中断优先级合法性校验
 *   3. 配置编译器内联优化策略
 *   4. 提供BASEPRI寄存器原子操作
 *-----------------------------------------------------------*/

/* 任务函数原型定义（兼容性需求） */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
    /* 任务声明原型宏：
    - 用于统一不同编译器的函数声明方式 */
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )
    /* 任务实现宏：
    - 实际与PROTO宏相同，保持命名一致性 */

/* 中断优先级验证断言 */
#ifdef configASSERT
    void vPortValidateInterruptPriority( void );
    #define portASSERT_IF_INTERRUPT_PRIORITY_INVALID()    vPortValidateInterruptPriority()
        /* 验证所有中断优先级数值：
        - 必须 ≥ configMAX_SYSCALL_INTERRUPT_PRIORITY
        - 防止在临界区中被高优先级中断打断 */
#endif

/* 编译器优化指令 */
#define portNOP()    /* 空操作占位符 */
#define portINLINE __inline         /* 建议编译器内联展开 */
#ifndef portFORCE_INLINE
    #define portFORCE_INLINE __forceinline  /* 强制内联（关键路径优化） */
#endif

/*-----------------------------------------------------------
 * BASEPRI寄存器原子操作 */

/* 设置BASEPRI值（通用版本） */
static portFORCE_INLINE void vPortSetBASEPRI( uint32_t ulBASEPRI )
{
    __asm
    {
        /* 直接写入目标值（不涉及屏障）：
           适用场景：中断屏蔽级别降级 */
        msr basepri, ulBASEPRI   // 指令周期：~2 cycles
    }
}

/* 提升中断屏蔽级别 */
static portFORCE_INLINE void vPortRaiseBASEPRI( void )
{
    uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
    __asm
    {
        msr basepri, ulNewBASEPRI  // 设置新屏蔽级别
        dsb                        // 数据存储器屏障
        isb                        // 指令存储器屏障
    }
    /* 时序说明：
       写寄存器 → dsb保证写入完成 → isb保证后续指令用新值 */
}

/* 中断服务程序中解除屏蔽 */
static portFORCE_INLINE void vPortClearBASEPRIFromISR( void )
{
    __asm
    {
        msr basepri, #0  // 清零操作（不破坏条件标志） 
    }
    /* ISR特殊性：
       1. 屏蔽时间短，无需屏障
       2. 退出ISR时有自动屏障 */
}
/*-----------------------------------------------------------
 * 功能模块：中断屏蔽与上下文检测
 * 作用说明：
 *   1. 实现原子化的中断屏蔽级别操作
 *   2. 精确检测代码执行上下文
 *-----------------------------------------------------------*/

/* 原子化提升中断屏蔽级别 */
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )
{
    uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
    
    __asm
    {
        /* 原子化操作序列：
           1. 保存当前BASEPRI值
           2. 设置新屏蔽级别
           3. 确保操作全局可见 */
        mrs ulReturn, basepri         // 读取当前屏蔽级别（约4周期）
        msr basepri, ulNewBASEPRI      // 设置新级别（约2周期）
        dsb                           // 数据同步屏障（约4周期）
        isb                           // 指令同步屏障（约4周期）
    }

    return ulReturn;  /* 返回原始BASEPRI值：
                        - 用于后续恢复操作
                        - 典型应用：嵌套临界区管理 */
}

/* 执行上下文检测函数 */
static portFORCE_INLINE BaseType_t xPortIsInsideInterrupt( void )
{
    uint32_t ulCurrentInterrupt;
    BaseType_t xReturn;

    __asm
    {
        /* 读取中断程序状态寄存器：
           - IPSR[8:0] 表示当前中断号
           - 0表示线程模式（非中断） */
        mrs ulCurrentInterrupt, ipsr  // 指令周期：约4
    }

    xReturn = ( ulCurrentInterrupt == 0 ) ? pdFALSE : pdTRUE;
    
    return xReturn;  /* 返回状态标识：
                      pdTRUE  - 当前在中断上下文
                      pdFALSE - 当前在任务上下文 */
}

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */


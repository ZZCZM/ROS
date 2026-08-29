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
 * 文件功能：ARM Cortex-M4 MPU 移植层宏定义
 * 作用说明：
 *   1. 定义与处理器架构相关的数据类型
 *   2. 配置MPU（内存保护单元）相关参数
 *   3. 提供FreeRTOS系统所需的跨平台类型支持
 *   4. 定义内存区域划分策略
 *   注意：本文件专为ARM Cortex-M4处理器设计，包含MPU支持
 *-----------------------------------------------------------*/

/* 基础类型定义（处理器相关类型抽象）*/
#define portCHAR        char        /* 定义字符类型（保证跨平台一致性） */
#define portFLOAT        float       /* 定义浮点类型（用于FPU操作） */
#define portDOUBLE        double      /* 定义双精度浮点类型 */
#define portLONG        long        /* 定义长整型（32位架构下通常为32位） */
#define portSHORT        short       /* 定义短整型（16位） */
#define portSTACK_TYPE    uint32_t    /* 栈存储单元类型（32位无符号） */
#define portBASE_TYPE    long        /* 基础数据类型（用于任务返回值等） */

/* 类型重定义（FreeRTOS标准类型）*/
typedef portSTACK_TYPE StackType_t;    /* 任务堆栈单元类型 */
typedef long BaseType_t;               /* 基础有符号类型（用于状态返回） */
typedef unsigned long UBaseType_t;     /* 基础无符号类型（用于计数等场景） */

/* TickType配置（根据系统时钟分辨率选择）*/
#if( configUSE_16_BIT_TICKS == 1 )
    /* 16位时间片配置（适用于低分辨率时钟系统） */
    typedef uint16_t TickType_t;               /* 定义16位时间戳类型 */
    #define portMAX_DELAY ( TickType_t ) 0xffff /* 最大延时值（16位最大值） */
#else
    /* 32位时间片配置（默认配置） */
    typedef uint32_t TickType_t;               /* 定义32位时间戳类型 */
    #define portMAX_DELAY ( TickType_t ) 0xffffffffUL /* 最大延时值（32位最大值） */
    
    /* 原子性说明：
       在32位架构上，32位的tick类型读操作是原子操作
       无需使用临界区保护 */
    #define portTICK_TYPE_IS_ATOMIC 1 /* 标记tick操作为原子操作 */
#endif

/*-----------------------------------------------------------*/

/* MPU内存保护单元配置 */
#define portUSING_MPU_WRAPPERS        1    /* 启用MPU封装层 */
#define portPRIVILEGE_BIT            ( 0x80000000UL ) /* 特权模式标志位 */

/* 内存区域属性标志（位域配置） */
#define portMPU_REGION_READ_WRITE                ( 0x03UL << 24UL )  /* 全读写权限 */
#define portMPU_REGION_PRIVILEGED_READ_ONLY      ( 0x05UL << 24UL )  /* 特权模式只读 */
#define portMPU_REGION_READ_ONLY                ( 0x06UL << 24UL )  /* 全局只读 */
#define portMPU_REGION_PRIVILEGED_READ_WRITE    ( 0x01UL << 24UL )  /* 特权模式读写 */
#define portMPU_REGION_CACHEABLE_BUFFERABLE        ( 0x07UL << 16UL )  /* 缓存与缓冲配置 */
#define portMPU_REGION_EXECUTE_NEVER            ( 0x01UL << 28UL )  /* 禁止执行保护 */

/* 预定义内存区域编号 */
#define portUNPRIVILEGED_FLASH_REGION        ( 0UL )  /* 非特权Flash访问区域 */
#define portPRIVILEGED_FLASH_REGION            ( 1UL )  /* 特权Flash访问区域 */
#define portPRIVILEGED_RAM_REGION            ( 2UL )  /* 特权RAM访问区域 */
#define portGENERAL_PERIPHERALS_REGION        ( 3UL )  /* 通用外设访问区域 */
#define portSTACK_REGION                    ( 4UL )  /* 任务堆栈专用区域 */
#define portFIRST_CONFIGURABLE_REGION        ( 5UL )  /* 首个可配置区域起始编号 */
#define portLAST_CONFIGURABLE_REGION        ( 7UL )  /* 最后可配置区域编号 */
#define portNUM_CONFIGURABLE_REGIONS        ( ( portLAST_CONFIGURABLE_REGION - portFIRST_CONFIGURABLE_REGION ) + 1 ) /* 可用配置区域总数 */
#define portTOTAL_NUM_REGIONS                ( portNUM_CONFIGURABLE_REGIONS + 1 ) /* 总区域数（包含栈区域） */
/*-----------------------------------------------------------
 * 功能模块：系统模式切换与调度器核心配置
 * 作用说明：
 *   1. 定义用户模式切换接口
 *   2. 配置MPU寄存器存储结构
 *   3. 设置系统架构关键参数
 *   4. 实现任务调度原语
 *   5. 定义中断控制寄存器操作
 *-----------------------------------------------------------*/

/* 用户模式切换接口 */
void vPortSwitchToUserMode( void );  /* 实际切换CPU到用户模式的函数 */
#define portSWITCH_TO_USER_MODE()	vPortSwitchToUserMode() /* 用户模式切换宏封装 */

/* MPU寄存器存储结构 */
typedef struct MPU_REGION_REGISTERS {
    uint32_t ulRegionBaseAddress;  /* 区域基地址（需32字节对齐） */
    uint32_t ulRegionAttribute;    /* 区域属性（包含权限/缓存/共享等配置） */
} xMPU_REGION_REGISTERS;  /* MPU单个区域配置结构体 */

/* MPU全局配置结构（包含所有区域设置） */
typedef struct MPU_SETTINGS {
    xMPU_REGION_REGISTERS xRegion[ portTOTAL_NUM_REGIONS ]; /* 区域配置数组 
        portTOTAL_NUM_REGIONS = 可配置区域数 + 栈区域 */
} xMPU_SETTINGS;

/*-----------------------------------------------------------
 * 架构关键参数配置 */
#define portSTACK_GROWTH			( -1 )  /* 栈增长方向：-1表示向下增长（ARM架构标准行为） */
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ ) /* 计算tick周期 
    示例：当configTICK_RATE_HZ=1000时，每个tick=1ms */
#define portBYTE_ALIGNMENT			8  /* 内存分配对齐要求（8字节对齐） */

/* 内存屏障指令参数 */
#define portSY_FULL_READ_WRITE		( 15 ) /* 全内存屏障模式：
    0b1111表示所有存储/加载操作的屏障 */

/*-----------------------------------------------------------
 * 系统调用(SVC)编号定义 */
#define portSVC_START_SCHEDULER				0  /* 启动调度器服务号 */
#define portSVC_YIELD						1  /* 主动让出CPU服务号 */
#define portSVC_RAISE_PRIVILEGE				2  /* 提升特权级别服务号 */

/*-----------------------------------------------------------
 * 任务调度原语实现 */
#define portYIELD()				__asm{ SVC portSVC_YIELD }  /* 主动触发任务切换：
    通过SVC指令触发系统调用，进入内核态进行上下文切换 */

/* API内部的让步宏（用于临界区中的调度） */
#define portYIELD_WITHIN_API() 													\
{																				\
    /* 设置PendSV挂起位来请求上下文切换（0x10000000） */						\
    portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;								\
    																			\
    /* 内存屏障保证操作原子性（ARMv7-M架构要求） */							\
    __dsb( portSY_FULL_READ_WRITE );  /* 数据同步屏障：确保存储操作完成 */		\
    __isb( portSY_FULL_READ_WRITE );  /* 指令同步屏障：清空流水线 */			\
}

/*-----------------------------------------------------------
 * 功能模块：中断控制与临界区管理
 * 作用说明：
 *   1. 定义中断控制寄存器操作
 *   2. 实现中断服务程序中的上下文切换
 *   3. 提供多级中断屏蔽机制
 *   4. 管理临界区进入/退出操作
 *-----------------------------------------------------------*/

/* NVIC中断控制器寄存器映射 */
#define portNVIC_INT_CTRL_REG        ( * ( ( volatile uint32_t * ) 0xe000ed04 ) )  /* 中断控制状态寄存器：
    - 地址：0xE000ED04（ARM Cortex-M4系统控制块区域）
    - 作用：挂起/清除系统异常 */
#define portNVIC_PENDSVSET_BIT        ( 1UL << 28UL )  /* PendSV挂起位掩码：
    - 第28位对应PendSV异常触发位 */

/* 中断上下文切换控制 */
#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired ) portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET
    /* 条件式上下文切换宏：
    - 参数：xSwitchRequired（非零值时触发切换）
    - 应用场景：中断服务程序退出时的延迟切换 */
#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )  /* 中断内任务切换的别名宏 */

/*-----------------------------------------------------------
 * 临界区管理接口 */
extern void vPortEnterCritical( void );  /* 进入临界区（嵌套安全实现）：
    - 关闭可屏蔽中断
    - 维护嵌套计数器 */
extern void vPortExitCritical( void );   /* 退出临界区（嵌套安全实现）：
    - 恢复嵌套计数器
    - 当计数器归零时恢复中断 */

/* 中断控制宏 */
#define portDISABLE_INTERRUPTS()                vPortRaiseBASEPRI()  /* 提升BASEPRI屏蔽中断：
    - 设置BASEPRI为configMAX_SYSCALL_INTERRUPT_PRIORITY */
#define portENABLE_INTERRUPTS()                    vPortSetBASEPRI(0)  /* 解除中断屏蔽：
    - 设置BASEPRI=0允许所有中断 */

/* 临界区操作封装 */
#define portENTER_CRITICAL()                    vPortEnterCritical()  /* 进入临界区标准接口 */
#define portEXIT_CRITICAL()                        vPortExitCritical()   /* 退出临界区标准接口 */

/* 中断服务程序中的中断屏蔽 */
#define portSET_INTERRUPT_MASK_FROM_ISR()        ulPortRaiseBASEPRI()  /* 保存并提升中断屏蔽级别：
    - 返回原BASEPRI值
    - 新BASEPRI设为最大屏蔽优先级 */
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)    vPortSetBASEPRI(x)  /* 恢复中断屏蔽级别：
    - 参数x：之前保存的BASEPRI值
    - 特别说明：必须与SET宏成对使用 */
/*-----------------------------------------------------------
 * 功能模块：任务调度优化与系统验证
 * 作用说明：
 *   1. 实现硬件加速的任务优先级管理
 *   2. 定义任务函数标准原型
 *   3. 提供中断优先级验证机制
 *   4. 配置编译器优化指令
 *-----------------------------------------------------------*/

/* 任务选择优化配置 */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
    #define configUSE_PORT_OPTIMISED_TASK_SELECTION 1  /* 默认启用基于CLZ指令的优化：
        利用ARM Cortex-M4的领先零计数指令加速最高优先级任务查找 */
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
    /* 优先级数量兼容性检查 */
    #if( configMAX_PRIORITIES > 32 )
        #error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32. 
        /* 错误说明：优化方案使用32位位图，最多支持32个优先级等级
           典型系统实际需求通常不超过15个优先级 */
    #endif

    /* 就绪任务位图操作 */
    #define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) ) 
        /* 设置优先级位图：
        - uxPriority：任务优先级（0~31）
        - uxReadyPriorities：32位就绪任务位图 */
    
    #define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )
        /* 清除优先级位图 */

    /*-----------------------------------------------------------*/
    
    /* 硬件加速的最高优先级查找 */
    #define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - ( uint32_t ) __clz( ( uxReadyPriorities ) ) )
        /* CLZ指令工作流程：
        1. __clz()返回位图中最高有效位前的零位数
        2. 31 - 零位数 = 最高优先级编号
        示例：0x80000000（仅31位）→ __clz=0 → 31-0=31 */

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/*-----------------------------------------------------------
 * 任务函数原型标准化 */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters ) 
    /* 任务函数声明宏（兼容旧版本）：
    - 用于统一不同编译器下的函数声明 */
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters ) 
    /* 任务函数定义宏：
    - 保证与FreeRTOS演示项目兼容 */

/*-----------------------------------------------------------
 * 中断优先级验证 */
#ifdef configASSERT
    void vPortValidateInterruptPriority( void );
    #define portASSERT_IF_INTERRUPT_PRIORITY_INVALID()    vPortValidateInterruptPriority() 
        /* 中断优先级合法性断言：
        - 检查是否所有中断优先级≥configMAX_SYSCALL_INTERRUPT_PRIORITY
        - 防止在临界区中被高优先级中断打断 */
#endif

/* 空操作宏 */
#define portNOP()  /* 架构无关的NOP占位符：
    实际应用中可通过__nop()指令实现 */

/* 编译器优化指令 */
#define portINLINE __inline  /* 建议编译器内联展开 */
#ifndef portFORCE_INLINE
    #define portFORCE_INLINE __forceinline  /* 强制内联展开：
        用于关键路径的性能优化 */
#endif

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------
 * 功能模块：BASEPRI寄存器操作与中断状态检测
 * 作用说明：
 *   1. 实现多级中断屏蔽机制
 *   2. 提供临界区保护的原子操作
 *   3. 检测当前执行上下文（中断/非中断）
 *-----------------------------------------------------------*/

/* 设置BASEPRI寄存器值 */
static portFORCE_INLINE void vPortSetBASEPRI( uint32_t ulBASEPRI )
{
    __asm
    {
        /* 直接写入BASEPRI寄存器（无内存屏障）：
           - 适用于降低屏蔽级别的场景
           - ulBASEPRI范围：0-255（8位掩码）*/
        msr basepri, ulBASEPRI
    }
}

/* 提升中断屏蔽级别 */
static portFORCE_INLINE void vPortRaiseBASEPRI( void )
{
    uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
    __asm
    {
        /* 设置最高系统调用中断优先级：
           - 屏蔽所有优先级≥ulNewBASEPRI的中断
           - 典型值：configMAX_SYSCALL_INTERRUPT_PRIORITY=5 */
        msr basepri, ulNewBASEPRI
        dsb    /* 数据存储器屏障：确保寄存器写入完成 */
        isb    /* 指令同步屏障：清空流水线 */
    }
}

/* 中断服务程序中解除中断屏蔽 */
static portFORCE_INLINE void vPortClearBASEPRIFromISR( void )
{
    __asm
    {
        /* 完全解除中断屏蔽：
           - 专为ISR设计（已知当前在中断上下文）
           - 不适用内存屏障（ISR退出时有自动屏障） */
        msr basepri, #0
    }
}

/* 原子化提升中断屏蔽级别（返回原状态） */
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )
{
    uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
    __asm
    {
        /* 保存原始BASEPRI状态 */
        mrs ulReturn, basepri         // 读取当前BASEPRI值
        msr basepri, ulNewBASEPRI     // 设置新屏蔽级别
        dsb                          // 确保新值生效
        isb                          // 确保后续指令使用新配置
    }
    return ulReturn;  /* 返回原始BASEPRI值用于后续恢复 */
}

/* 检测当前执行环境 */
static portFORCE_INLINE BaseType_t xPortIsInsideInterrupt( void )
{
    uint32_t ulCurrentInterrupt;
    BaseType_t xReturn;

    __asm
    {
        /* 读取中断程序状态寄存器(IPSR)：
           - 0表示线程模式
           - 非0表示正在处理中断 */
        mrs ulCurrentInterrupt, ipsr
    }

    xReturn = ( ulCurrentInterrupt == 0 ) ? pdFALSE : pdTRUE;
    return xReturn;  /* 返回pdTRUE表示当前在中断上下文 */
}
/*-----------------------------------------------------------
 * 功能模块：特权级别控制
 * 作用说明：
 *   1. 根据任务需求切换特权模式
 *   2. 配合MPU实现内存访问权限控制
 *   3. 保障系统关键资源的安全性
 *-----------------------------------------------------------*/

/* 特权级别重置函数 */
portFORCE_INLINE static void vPortResetPrivilege( BaseType_t xRunningPrivileged )
{
    uint32_t ulReg;  /* 临时存储CONTROL寄存器值 */
    
    /* 仅在需要降级到用户模式时执行操作 */
    if( xRunningPrivileged != pdTRUE )  // pdTRUE=1表示保持特权模式
    {
        __asm 
        {
            /* ARM Cortex-M4特权级别控制流程：
               1. 读取当前CONTROL寄存器状态 */
            mrs ulReg, control  // 将CONTROL寄存器值存入ulReg

            /* 2. 设置CONTROL[0]位为1：
               - CONTROL[0]=0: 线程模式特权级
               - CONTROL[0]=1: 线程模式用户级 */
            orr ulReg, #0x01    // 位或操作设置最低位

            /* 3. 写回修改后的值：
               此操作将立即生效，改变处理器模式 */
            msr control, ulReg   // 将ulReg写回CONTROL寄存器
        }
        /* 注意：模式切换后需考虑以下影响：
           - 访问权限受MPU限制
           - 无法直接执行特权指令（如MSR/MRS操作）*/
    }
}

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */


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

#ifndef INC_FREERTOS_H
#define INC_FREERTOS_H

/*
 * Include the generic headers required for the FreeRTOS port being used.
 */
#include <stddef.h>

/*
 * If stdint.h cannot be located then:
 *   + If using GCC ensure the -nostdint options is *not* being used.
 *   + Ensure the project's include path includes the directory in which your
 *     compiler stores stdint.h.
 *   + Set any compiler options necessary for it to support C99, as technically
 *     stdint.h is only mandatory with C99 (FreeRTOS does not require C99 in any
 *     other way).
 *   + The FreeRTOS download includes a simple stdint.h definition that can be
 *     used in cases where none is provided by the compiler.  The files only
 *     contains the typedefs required to build FreeRTOS.  Read the instructions
 *     in FreeRTOS/source/stdint.readme for more information.
 */
#include <stdint.h> /* READ COMMENT ABOVE. */

#ifdef __cplusplus
extern "C" {
#endif

/* Application specific configuration options. */
#include "FreeRTOSConfig.h"

/* Basic FreeRTOS definitions. */
#include "projdefs.h"

/* Definitions specific to the port being used. */
#include "portable.h"

/* 
 * configUSE_NEWLIB_REENTRANT - 启用Newlib可重入支持
 * 默认值：0（禁用）
 * 设置为1时：
 *   - 需要包含Newlib的reent.h头文件
 *   - 每个任务拥有独立的_reent结构体
 * 适用场景：使用Newlib且需多线程安全的标准库函数时
 */
#ifndef configUSE_NEWLIB_REENTRANT
    #define configUSE_NEWLIB_REENTRANT 0  /* 默认禁用Newlib可重入模式 */
#endif

#if ( configUSE_NEWLIB_REENTRANT == 1 )
    #include <reent.h>  /* Newlib可重入结构体定义 */
#endif
/*
 * Check all the required application specific macros have been defined.
 * These macros are application specific and (as downloaded) are defined
 * within FreeRTOSConfig.h.
 */
/* 空闲任务栈大小检查 */
#ifndef configMINIMAL_STACK_SIZE
	#error Missing definition:  configMINIMAL_STACK_SIZE must be defined in FreeRTOSConfig.h.  configMINIMAL_STACK_SIZE defines the size (in words) of the stack allocated to the idle task.  Refer to the demo project provided for your port for a suitable value.
#endif
/* 最大优先级检查 */
#ifndef configMAX_PRIORITIES
	#error Missing definition:  configMAX_PRIORITIES must be defined in FreeRTOSConfig.h.  See the Configuration section of the FreeRTOS API documentation for details.
#endif
/* 抢占式调度配置检查 */
#ifndef configUSE_PREEMPTION
	#error Missing definition:  configUSE_PREEMPTION must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif
/* 空闲任务钩子检查 */
#ifndef configUSE_IDLE_HOOK
	#error Missing definition:  configUSE_IDLE_HOOK must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif
/* 系统节拍钩子检查 */
#ifndef configUSE_TICK_HOOK
	#error Missing definition:  configUSE_TICK_HOOK must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif

/* 时钟节拍位数检查 */
#ifndef configUSE_16_BIT_TICKS
	#error Missing definition:  configUSE_16_BIT_TICKS must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif
/* 最大优先级有效性验证 */
#ifndef configMAX_PRIORITIES
	#error configMAX_PRIORITIES must be defined to be greater than or equal to 1.
#endif
/*
 * FreeRTOS可选功能模块配置宏默认设置
 * 作用：为编译配置选项提供安全默认值，减少用户必须定义的宏数量
 */

/* 协程功能开关 */
#ifndef configUSE_CO_ROUTINES
    #define configUSE_CO_ROUTINES 0  /* 默认禁用协程功能（0=禁用，1=启用） */
#endif

/* 任务管理API开关 --------------------------------------------------------*/
#ifndef INCLUDE_vTaskPrioritySet
    #define INCLUDE_vTaskPrioritySet 0  /* 禁用vTaskPrioritySet API（0=排除，1=包含） */
#endif

#ifndef INCLUDE_uxTaskPriorityGet
    #define INCLUDE_uxTaskPriorityGet 0  /* 禁用uxTaskPriorityGet API */
#endif

#ifndef INCLUDE_vTaskDelete
    #define INCLUDE_vTaskDelete 0  /* 禁用任务删除功能，提高系统安全性 */
#endif

#ifndef INCLUDE_vTaskSuspend
    #define INCLUDE_vTaskSuspend 0  /* 禁用任务挂起功能，防止误用导致死锁 */
#endif

/* 任务延时API控制 */
#ifndef INCLUDE_vTaskDelayUntil
    #define INCLUDE_vTaskDelayUntil 0  /* 禁用绝对时间延时API（适合周期性任务） */
#endif

#ifndef INCLUDE_vTaskDelay
    #define INCLUDE_vTaskDelay 0  /* 禁用相对时间延时API（慎用，可能导致时间漂移） */
#endif

/* 空闲任务访问控制 */
#ifndef INCLUDE_xTaskGetIdleTaskHandle
    #define INCLUDE_xTaskGetIdleTaskHandle 1  /* 默认允许获取空闲任务句柄（用于监控） */
#endif

/* 任务控制扩展功能 */
#ifndef INCLUDE_xTaskAbortDelay
    #define INCLUDE_xTaskAbortDelay 0  /* 禁用中止任务延时状态功能（0=保守设计） */
#endif

/* 互斥量持有者追踪 */
#ifndef INCLUDE_xQueueGetMutexHolder
    #define INCLUDE_xQueueGetMutexHolder 0  /* 禁用查询互斥量持有者功能（节省资源） */
#endif
#ifndef INCLUDE_xSemaphoreGetMutexHolder
    #define INCLUDE_xSemaphoreGetMutexHolder INCLUDE_xQueueGetMutexHolder  /* 与队列实现保持同步 */
#endif

/* 任务信息查询 */
#ifndef INCLUDE_xTaskGetHandle
    #define INCLUDE_xTaskGetHandle 0  /* 禁用通过任务名获取句柄功能（减少内存开销） */
#endif

#ifndef INCLUDE_uxTaskGetStackHighWaterMark
    #define INCLUDE_uxTaskGetStackHighWaterMark 0  /* 禁用栈高水位检测（调试时设为1） */
#endif

#ifndef INCLUDE_eTaskGetState
    #define INCLUDE_eTaskGetState 0  /* 禁用获取任务状态机功能（生产环境建议关闭） */
#endif

/* 中断级任务控制 */
#ifndef INCLUDE_xTaskResumeFromISR
    #define INCLUDE_xTaskResumeFromISR 1  /* 默认允许在ISR中恢复任务（关键实时功能） */
#endif

/* 定时器扩展功能 */
#ifndef INCLUDE_xTimerPendFunctionCall
    #define INCLUDE_xTimerPendFunctionCall 0  /* 禁用定时器延迟函数调用（保持内核精简） */
#endif

/* 调度器状态查询 */
#ifndef INCLUDE_xTaskGetSchedulerState
    #define INCLUDE_xTaskGetSchedulerState 0  /* 禁用调度器状态查询（避免运行时检测） */
#endif

/* 当前任务访问 */
#ifndef INCLUDE_xTaskGetCurrentTaskHandle
    #define INCLUDE_xTaskGetCurrentTaskHandle 0  /* 禁用获取当前任务句柄（按需启用） */
#endif

/* 配置策略说明：
 * 1. 安全性优先：默认关闭危险操作API（如任务删除/挂起）
 * 2. 资源优化：默认排除非核心功能减小代码体积
 * 3. 实时性保障：保留关键中断操作API（如xTaskResumeFromISR）
 * 4. 调试支持：默认开启与系统监控相关的功能（如空闲任务句柄获取）
 *
 * 用户应根据实际需求在FreeRTOSConfig.h中覆盖这些默认设置
 */

/* 协程配置验证 ----------------------------------------------------------*/
#if configUSE_CO_ROUTINES != 0
	#ifndef configMAX_CO_ROUTINE_PRIORITIES
		#error configMAX_CO_ROUTINE_PRIORITIES must be greater than or equal to 1.
	#endif
#endif


/* 系统服务扩展配置 ------------------------------------------------------*/
#ifndef configUSE_DAEMON_TASK_STARTUP_HOOK
    #define configUSE_DAEMON_TASK_STARTUP_HOOK 0  /* 禁用守护任务启动钩子（定时器任务初始化回调） */
#endif

#ifndef configUSE_APPLICATION_TASK_TAG
    #define configUSE_APPLICATION_TASK_TAG 0  /* 禁用任务标签功能（0=节省TCB空间，1=允许vTaskSetApplicationTaskTag） */
#endif

/* 线程本地存储配置 */
#ifndef configNUM_THREAD_LOCAL_STORAGE_POINTERS
    #define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0  /* 每个任务的TLS指针数量（0=禁用，最大portPOINTER_SIZE_TYPE） */
#endif

/* 互斥量功能控制 */
#ifndef configUSE_RECURSIVE_MUTEXES
    #define configUSE_RECURSIVE_MUTEXES 0  /* 禁用递归互斥量（0=减少代码体积，1=支持xSemaphoreCreateRecursiveMutex） */
#endif

#ifndef configUSE_MUTEXES
    #define configUSE_MUTEXES 0  /* 禁用互斥量功能（0=排除互斥量API，1=支持优先级继承） */
#endif

/* 定时器服务配置 */
#ifndef configUSE_TIMERS
    #define configUSE_TIMERS 0  /* 禁用软件定时器（0=排除定时器API，1=需配置守护任务参数） */
#endif

/* 信号量类型控制 */
#ifndef configUSE_COUNTING_SEMAPHORES
    #define configUSE_COUNTING_SEMAPHORES 0  /* 禁用计数信号量（0=仅支持二进制信号量，1=允许创建计数信号量） */
#endif

/* 替代API配置（历史兼容） */
#ifndef configUSE_ALTERNATIVE_API
    #define configUSE_ALTERNATIVE_API 0  /* 禁用替代队列API（0=标准API，1=可能影响性能，不推荐） */
#endif

/* 临界区嵌套计数存储 */
#ifndef portCRITICAL_NESTING_IN_TCB
    #define portCRITICAL_NESTING_IN_TCB 0  /* 临界区计数存储位置（0=全局变量，1=任务TCB中，影响移植代码） */
#endif

/* 任务属性配置 --------------------------------------------------------*/
#ifndef configMAX_TASK_NAME_LEN
    #define configMAX_TASK_NAME_LEN 16  /* 任务名称最大字符数（含结束符，建议8-32之间） */
#endif

/* 空闲任务行为配置 */
#ifndef configIDLE_SHOULD_YIELD
    #define configIDLE_SHOULD_YIELD 1  /* 空闲任务主动让出CPU（1=同优先级任务轮转，0=独占时间片） */
#endif

#if configMAX_TASK_NAME_LEN < 1
	#error configMAX_TASK_NAME_LEN must be set to a minimum of 1 in FreeRTOSConfig.h
#endif

/* 系统断言配置 */
#ifndef configASSERT
    #define configASSERT( x )          /* 默认为空断言（生产环境） */
    #define configASSERT_DEFINED 0     /* 标记未自定义断言 */
#else
    #define configASSERT_DEFINED 1     /* 标记已启用自定义断言 */
#endif

/* The timers module relies on xTaskGetSchedulerState(). */
#if configUSE_TIMERS == 1

	#ifndef configTIMER_TASK_PRIORITY
		#error If configUSE_TIMERS is set to 1 then configTIMER_TASK_PRIORITY must also be defined.
	#endif /* configTIMER_TASK_PRIORITY */

	#ifndef configTIMER_QUEUE_LENGTH
		#error If configUSE_TIMERS is set to 1 then configTIMER_QUEUE_LENGTH must also be defined.
	#endif /* configTIMER_QUEUE_LENGTH */

	#ifndef configTIMER_TASK_STACK_DEPTH
		#error If configUSE_TIMERS is set to 1 then configTIMER_TASK_STACK_DEPTH must also be defined.
	#endif /* configTIMER_TASK_STACK_DEPTH */

#endif /* configUSE_TIMERS */
/*
 * FreeRTOS移植层接口与追踪宏默认设置
 * 作用：为硬件移植和调试追踪提供安全默认实现
 */

/* 中断管理宏（需根据目标MCU移植实现）---------------------------------------*/
#ifndef portSET_INTERRUPT_MASK_FROM_ISR
    #define portSET_INTERRUPT_MASK_FROM_ISR() 0  /* 默认空实现，需移植：返回当前中断状态 */
#endif

#ifndef portCLEAR_INTERRUPT_MASK_FROM_ISR
    #define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedStatusValue ) ( void ) uxSavedStatusValue  /* 默认空实现，需移植：恢复中断状态 */
#endif

/* 任务控制块(TCB)管理钩子 */
#ifndef portCLEAN_UP_TCB
    #define portCLEAN_UP_TCB( pxTCB ) ( void ) pxTCB  /* 任务删除前的资源清理钩子（如释放自定义TCB扩展） */
#endif

#ifndef portPRE_TASK_DELETE_HOOK
    #define portPRE_TASK_DELETE_HOOK( pvTaskToDelete, pxYieldPending )  /* 任务删除前预处理钩子（可添加日志或统计） */
#endif

#ifndef portSETUP_TCB
    #define portSETUP_TCB( pxTCB ) ( void ) pxTCB  /* TCB初始化扩展钩子（用于自定义TCB结构初始化） */
#endif

/* 队列注册表配置（用于调试工具可视化） */
#ifndef configQUEUE_REGISTRY_SIZE
    #define configQUEUE_REGISTRY_SIZE 0U  /* 禁用队列注册功能（0=不记录队列信息，≥1=注册表条目数） */
#endif

#if ( configQUEUE_REGISTRY_SIZE < 1 )
    #define vQueueAddToRegistry( xQueue, pcName )   /* 动态注册队列到注册表（禁用时为空宏） */
    #define vQueueUnregisterQueue( xQueue )         /* 从注册表移除队列 */
    #define pcQueueGetName( xQueue )                 /* 获取队列注册名称 */
#endif

/* 指针类型定义（确保跨平台兼容性） */
#ifndef portPOINTER_SIZE_TYPE
    #define portPOINTER_SIZE_TYPE uint32_t  /* 定义指针大小类型（32位系统：uint32_t，64位：uint64_t） */
#endif

/* 系统追踪宏（默认禁用，需配套Trace工具）----------------------------------*/
#ifndef traceSTART
    #define traceSTART()  /* 追踪系统初始化（如打开日志文件） */
#endif

#ifndef traceEND
    #define traceEND()    /* 追踪系统终止（如关闭日志文件） */
#endif

#ifndef traceTASK_SWITCHED_IN
    #define traceTASK_SWITCHED_IN()  /* 任务切换进入时追踪（参数：pxCurrentTCB） */
#endif

#ifndef traceINCREASE_TICK_COUNT
    #define traceINCREASE_TICK_COUNT( x )  /* Tick计数增加追踪（用于低功耗模式恢复后） */
#endif

#ifndef traceLOW_POWER_IDLE_BEGIN
    #define traceLOW_POWER_IDLE_BEGIN()  /* 进入低功耗空闲模式前追踪 */
#endif

#ifndef traceLOW_POWER_IDLE_END
    #define traceLOW_POWER_IDLE_END()    /* 退出低功耗空闲模式后追踪 */
#endif

#ifndef traceTASK_SWITCHED_OUT
    #define traceTASK_SWITCHED_OUT()  /* 任务切换退出时追踪（参数：pxCurrentTCB） */
#endif

/* 移植与追踪配置说明：
 * 1. 中断管理宏：
 *    - 必须根据目标MCU的中断控制器实现
 *    - 典型实现（ARM Cortex-M）：
 *      #define portSET_INTERRUPT_MASK_FROM_ISR() __get_BASEPRI()
 *      #define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) __set_BASEPRI(x)
 *
 * 2. 队列注册表：
 *    - 调试时设为≥1，通过vQueueAddToRegistry注册队列
 *    - 可在FreeRTOS+Trace中可视化队列状态
 *
 * 3. 追踪宏开发：
 *    启用追踪功能时，应在FreeRTOSConfig.h中覆盖定义，例如：
 *    #define traceTASK_SWITCHED_IN() traceLogTaskSwitch(pxCurrentTCB)
 *
 * 4. 低功耗追踪：
 *    traceLOW_POWER_IDLE_BEGIN/END 可用于统计休眠时间
 */

/*
 * FreeRTOS高级追踪宏与安全配置
 * 作用：提供关键系统事件的追踪点及栈溢出检测配置
 */

/* 互斥量优先级继承追踪 ------------------------------------------------*/
#ifndef traceTASK_PRIORITY_INHERIT
    /**
     * @brief 追踪任务优先级继承事件
     * 触发时机：当低优先级任务尝试获取已被持有的互斥量时
     * @param pxTCBOfMutexHolder 互斥量持有者的TCB指针
     * @param uxInheritedPriority 被继承的目标优先级
     */
    #define traceTASK_PRIORITY_INHERIT( pxTCBOfMutexHolder, uxInheritedPriority )
#endif

#ifndef traceTASK_PRIORITY_DISINHERIT
    /**
     * @brief 追踪任务优先级解除继承事件
     * 触发时机：当任务释放导致优先级继承的互斥量时
     * @param pxTCBOfMutexHolder 正在释放互斥量的任务TCB指针
     * @param uxOriginalPriority 任务原始优先级
     */
    #define traceTASK_PRIORITY_DISINHERIT( pxTCBOfMutexHolder, uxOriginalPriority )
#endif

/* 队列阻塞事件追踪 ----------------------------------------------------*/
#ifndef traceBLOCKING_ON_QUEUE_RECEIVE
    /**
     * @brief 追踪任务因队列接收阻塞事件
     * 触发时机：任务因队列空无法读取而进入阻塞状态
     * @param pxQueue 目标队列指针
     */
    #define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue )
#endif

#ifndef traceBLOCKING_ON_QUEUE_SEND
    /**
     * @brief 追踪任务因队列发送阻塞事件
     * 触发时机：任务因队列满无法写入而进入阻塞状态
     * @param pxQueue 目标队列指针
     */
    #define traceBLOCKING_ON_QUEUE_SEND( pxQueue )
#endif

/* 栈溢出检测配置 ------------------------------------------------------*/
#ifndef configCHECK_FOR_STACK_OVERFLOW
    #define configCHECK_FOR_STACK_OVERFLOW 0  /* 栈溢出检测级别（0=禁用，1=简易检测，2=精确检测） */
#endif

/* 任务状态迁移追踪 ----------------------------------------------------*/
#ifndef traceMOVED_TASK_TO_READY_STATE
    /**
     * @brief 追踪任务进入就绪状态事件
     * 触发时机：任务从阻塞/挂起状态转移到就绪队列时
     * @param pxTCB 迁移任务的TCB指针
     */
    #define traceMOVED_TASK_TO_READY_STATE( pxTCB )
#endif

#ifndef tracePOST_MOVED_TASK_TO_READY_STATE
    /**
     * @brief 后置任务就绪状态追踪
     * 触发时机：在任务真正加入就绪队列后调用
     * @param pxTCB 已就绪任务的TCB指针
     */
    #define tracePOST_MOVED_TASK_TO_READY_STATE( pxTCB )
#endif

/* 队列与互斥量操作追踪 -------------------------------------------------*/
#ifndef traceQUEUE_CREATE
    /**
     * @brief 追踪队列创建成功事件
     * @param pxNewQueue 新创建队列的指针
     */
    #define traceQUEUE_CREATE( pxNewQueue )
#endif

#ifndef traceQUEUE_CREATE_FAILED
    /**
     * @brief 追踪队列创建失败事件
     * @param ucQueueType 队列类型标识（由内核内部使用）
     */
    #define traceQUEUE_CREATE_FAILED( ucQueueType )
#endif

#ifndef traceCREATE_MUTEX
    /**
     * @brief 追踪互斥量创建成功事件
     * @param pxNewQueue 新创建互斥量的指针
     */
    #define traceCREATE_MUTEX( pxNewQueue )
#endif

#ifndef traceCREATE_MUTEX_FAILED
    /**
     * @brief 追踪互斥量创建失败事件
     */
    #define traceCREATE_MUTEX_FAILED()
#endif

/* 递归互斥量操作追踪 ---------------------------------------------------*/
#ifndef traceGIVE_MUTEX_RECURSIVE
    /**
     * @brief 追踪递归互斥量释放事件
     * @param pxMutex 目标互斥量指针
     */
    #define traceGIVE_MUTEX_RECURSIVE( pxMutex )
#endif

#ifndef traceTAKE_MUTEX_RECURSIVE
    /**
     * @brief 追踪递归互斥量获取事件
     * @param pxMutex 目标互斥量指针
     */
    #define traceTAKE_MUTEX_RECURSIVE( pxMutex )
#endif
/* 递归互斥量追踪宏组 ---------------------------------------------------*/
#ifndef traceTAKE_MUTEX_RECURSIVE_FAILED
    /**
     * @brief 追踪递归互斥量获取失败事件
     * @param pxMutex 目标递归互斥量指针（StaticSemaphore_t* 类型）
     * @触发条件
     * - 非递归方式尝试获取已持有的互斥量
     * - 互斥量被其他任务持有且优先级继承失败
     * - 递归深度达到类型最大值（16/32位限制）
     * - 系统内存异常导致信号量结构损坏
     * @调试建议
     * 1. 记录当前任务句柄及互斥量持有者
     * 2. 结合uxSemaphoreGetCount()检查可用计数
     * 3. 追踪互斥量当前递归深度
     * @死锁检测
     * 可与vApplicationDeadlockHook配合实现：
     * if( xSemaphoreGetMutexHolder(pxMutex) == xTaskGetCurrentTaskHandle() )
     *     traceTAKE_MUTEX_RECURSIVE_FAILED(pxMutex);
     */
    #define traceTAKE_MUTEX_RECURSIVE_FAILED( pxMutex )
#endif

#ifndef traceGIVE_MUTEX_RECURSIVE_FAILED
    /**
     * @brief 追踪递归互斥量释放失败事件
     * @param pxMutex 尝试释放的互斥量指针
     * @触发条件
     * - 非持有任务尝试释放
     * - 递归深度未归零时提前释放
     * - 互斥量处于无效状态
     * @调试价值
     * 检测以下类型错误：
     * 1. 未配对的give/take操作
     * 2. 跨任务共享互斥量
     * 3. 内存损坏导致的互斥量状态异常
     * @使用示例
     * 在释放操作前添加状态校验：
     * if( xSemaphoreGetMutexHolder(pxMutex) != xTaskGetCurrentTaskHandle() ){
     *     traceGIVE_MUTEX_RECURSIVE_FAILED(pxMutex);
     *     return errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
     * }
     */
    #define traceGIVE_MUTEX_RECURSIVE_FAILED( pxMutex )
#endif

/* 计数信号量追踪 ------------------------------------------------------*/
#ifndef traceCREATE_COUNTING_SEMAPHORE
    /**
     * @brief 追踪计数信号量创建成功事件
     */
    #define traceCREATE_COUNTING_SEMAPHORE()
#endif

#ifndef traceCREATE_COUNTING_SEMAPHORE_FAILED
    /**
     * @brief 追踪计数信号量创建失败事件
     */
    #define traceCREATE_COUNTING_SEMAPHORE_FAILED()
#endif

/* 配置说明：
 * 1. 栈溢出检测级别：
 *    - 0：禁用检测（生产环境）
 *    - 1：简易模式（检查栈指针越界）
 *    - 2：精确模式（填充魔数并校验）
 *    建议开发阶段设置为2，发布时设为0
 * 
 * 2. 追踪宏开发建议：
 *    启用追踪时在FreeRTOSConfig.h中重定义，例如：
 *    #define traceTASK_PRIORITY_INHERIT(px,pri) dbgLogMutexInherit(px->pcTaskName, pri)
 * 
 * 3. 性能影响：
 *    - 每个追踪宏会增加少量执行时间
 *    - 建议通过编译条件选择性启用关键追踪点
 * 
 * 4. 典型追踪场景：
 *    - 优先级反转调试：启用traceTASK_PRIORITY_INHERIT/DISINHERIT
 *    - 队列阻塞分析：启用BLOCKING_ON_QUEUE系列
 *    - 内存泄漏检测：结合栈溢出检查与任务状态追踪
 */

/*
 * FreeRTOS队列与任务追踪宏配置
 * 作用：提供队列操作和任务创建的关键事件追踪点
 */

/* 队列发送操作追踪 ----------------------------------------------------*/
#ifndef traceQUEUE_SEND
    /**
     * @brief 追踪队列发送成功事件
     * 触发时机：数据成功写入队列后
     * @param pxQueue 目标队列指针
     */
    #define traceQUEUE_SEND( pxQueue )
#endif

#ifndef traceQUEUE_SEND_FAILED
    /**
     * @brief 追踪队列发送失败事件
     * 触发时机：队列满且等待超时/立即返回时
     * @param pxQueue 目标队列指针
     */
    #define traceQUEUE_SEND_FAILED( pxQueue )
#endif

/* 队列接收操作追踪 ----------------------------------------------------*/
#ifndef traceQUEUE_RECEIVE
    /**
     * @brief 追踪队列接收成功事件
     * 触发时机：数据成功从队列读取后
     * @param pxQueue 目标队列指针
     */
    #define traceQUEUE_RECEIVE( pxQueue )
#endif

#ifndef traceQUEUE_RECEIVE_FAILED
    /**
     * @brief 追踪队列接收失败事件
     * 触发时机：队列空且等待超时/立即返回时
     * @param pxQueue 目标队列指针
     */
    #define traceQUEUE_RECEIVE_FAILED( pxQueue )
#endif

/* 队列查看操作追踪 ----------------------------------------------------*/
#ifndef traceQUEUE_PEEK
    /**
     * @brief 追踪队列查看成功事件（不移除数据）
     * 触发时机：成功查看队列头部数据后
     * @param pxQueue 目标队列指针
     */
    #define traceQUEUE_PEEK( pxQueue )
#endif

#ifndef traceQUEUE_PEEK_FROM_ISR
    /**
     * @brief 追踪中断上下文队列查看事件
     * 触发时机：在ISR中成功查看队列数据后
     * @param pxQueue 目标队列指针
     */
    #define traceQUEUE_PEEK_FROM_ISR( pxQueue )
#endif

/* 中断级队列操作追踪 -------------------------------------------------*/
#ifndef traceQUEUE_SEND_FROM_ISR
    /**
     * @brief 追踪中断上下文队列发送成功事件
     * 触发时机：在ISR中成功写入队列后
     * @param pxQueue 目标队列指针
     */
    #define traceQUEUE_SEND_FROM_ISR( pxQueue )
#endif

/* ISR队列操作追踪宏组 -------------------------------------------------*/
#ifndef traceQUEUE_SEND_FROM_ISR_FAILED
    /**
     * @brief 追踪ISR发送队列失败事件
     * @param pxQueue 目标队列指针（QueueHandle_t类型）
     * @触发条件
     * - 队列已满且未指定覆盖写入
     * - 内存不足导致发送失败
     * @调试要点
     * 建议记录：
     * 1. 队列当前项数/最大容量
     * 2. 发送时刻的ISR嵌套层级
     * 3. 阻塞在接收端的任务列表
     * @性能影响
     * 在高速ISR中慎用复杂日志操作
     */
    #define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue )
#endif

#ifndef traceQUEUE_RECEIVE_FROM_ISR
    /**
     * @brief 追踪ISR接收队列项事件
     * @param pxQueue 源队列指针
     * @触发时机 成功从队列取出数据项时
     * @扩展应用
     * - 监控ISR到任务的数据流频率
     * - 校验数据项完整性
     * - 测量ISR响应延迟（结合时间戳）
     * @实现注意
     * 应保证在临界区内快速执行
     */
    #define traceQUEUE_RECEIVE_FROM_ISR( pxQueue )
#endif

#ifndef traceQUEUE_RECEIVE_FROM_ISR_FAILED
    /**
     * @brief 追踪ISR接收队列失败事件
     * @param pxQueue 目标队列指针
     * @典型场景
     * - 队列持续为空导致数据丢失
     * - 高优先级任务未及时处理数据
     * @调试策略
     * 1. 关联发送端trace定位生产瓶颈
     * 2. 检查队列监控统计信息
     * 3. 分析任务调度序列
     */
    #define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue )
#endif

#ifndef traceQUEUE_PEEK_FROM_ISR_FAILED
    /**
     * @brief 追踪ISR查看队列项失败事件
     * @param pxQueue 目标队列指针
     * @特殊说明
     * Peek操作不移除数据项，失败说明：
     * - 队列瞬时为空
     * - 并发访问导致竞争
     * @调试技巧
     * 结合xQueueSpacesAvailable()验证队列状态
     * @安全警告
     * 在流控场景中频繁失败可能指示设计缺陷
     */
    #define traceQUEUE_PEEK_FROM_ISR_FAILED( pxQueue )
#endif


/* 队列生命周期追踪 ----------------------------------------------------*/
#ifndef traceQUEUE_DELETE
    /**
     * @brief 追踪队列删除事件
     * 触发时机：队列被vQueueDelete删除后
     * @param pxQueue 被删除队列指针
     */
    #define traceQUEUE_DELETE( pxQueue )
#endif

/* 任务创建追踪 --------------------------------------------------------*/
#ifndef traceTASK_CREATE
    /**
     * @brief 追踪任务创建成功事件
     * 触发时机：任务通过xTaskCreate成功创建后
     * @param pxNewTCB 新创建任务的TCB指针
     */
    #define traceTASK_CREATE( pxNewTCB )
#endif

/* 追踪宏应用说明：
 * 1. 队列调试场景：
 *    - 结合SEND/RECEIVE_FAILED追踪队列溢出/下溢问题
 *    - 使用PEEK追踪监控数据窥探操作
 * 
 * 2. 中断级操作追踪：
 *    - ISR操作队列时记录时间戳，分析中断响应延迟
 * 
 * 3. 资源泄漏检测：
 *    - 通过QUEUE_DELETE追踪与创建记录的匹配，发现未释放队列
 * 
 * 4. 任务创建监控：
 *    - 统计任务创建成功率
 *    - 跟踪TCB初始化过程的问题
 * 
 * 示例配置：
 * #define traceQUEUE_SEND(px) logQueueOp(px, "SEND_OK")
 * #define traceQUEUE_SEND_FAILED(px) logQueueOp(px, "SEND_FAIL")
 * 
 * 性能注意：
 * 高频队列操作时建议仅启用错误追踪，避免日志过载
 */
/*
 * FreeRTOS任务与队列追踪宏配置
 * 作用：提供任务生命周期和队列操作的关键事件追踪点
 */

/* 任务创建失败追踪 ----------------------------------------------------*/
#ifndef traceTASK_CREATE_FAILED
    /**
     * @brief 追踪任务创建失败事件
     * 触发时机：任务通过xTaskCreate创建失败后
     */
    #define traceTASK_CREATE_FAILED()
#endif

/* 任务删除追踪 --------------------------------------------------------*/
#ifndef traceTASK_DELETE
    /**
     * @brief 追踪任务删除事件
     * 触发时机：任务通过vTaskDelete删除后
     * @param pxTaskToDelete 被删除任务指针
     */
    #define traceTASK_DELETE( pxTaskToDelete )
#endif

/* 任务延迟追踪 --------------------------------------------------------*/
#ifndef traceTASK_DELAY_UNTIL
    /**
     * @brief 追踪任务延迟到指定时间点事件
     * 触发时机：任务调用vTaskDelayUntil后
     * @param x 指定的绝对时间点
     */
    #define traceTASK_DELAY_UNTIL( x )
#endif

#ifndef traceTASK_DELAY
    /**
     * @brief 追踪任务延迟指定时间事件
     * 触发时机：任务调用vTaskDelay后
     */
    #define traceTASK_DELAY()
#endif

/* 任务优先级设置追踪 --------------------------------------------------*/
#ifndef traceTASK_PRIORITY_SET
    /**
     * @brief 追踪任务优先级设置事件
     * 触发时机：任务优先级通过vTaskPrioritySet设置后
     * @param pxTask 目标任务指针
     * @param uxNewPriority 设置的新优先级
     */
    #define traceTASK_PRIORITY_SET( pxTask, uxNewPriority )
#endif

/* 任务挂起追踪 --------------------------------------------------------*/
#ifndef traceTASK_SUSPEND
    /**
     * @brief 追踪任务挂起事件
     * 触发时机：任务通过vTaskSuspend挂起后
     * @param pxTaskToSuspend 被挂起任务指针
     */
    #define traceTASK_SUSPEND( pxTaskToSuspend )
#endif

/* 任务恢复追踪 --------------------------------------------------------*/
#ifndef traceTASK_RESUME
    /**
     * @brief 追踪任务恢复事件
     * 触发时机：任务通过vTaskResume恢复后
     * @param pxTaskToResume 被恢复任务指针
     */
    #define traceTASK_RESUME( pxTaskToResume )
#endif

/* 中断上下文任务恢复追踪 ----------------------------------------------*/
#ifndef traceTASK_RESUME_FROM_ISR
    /**
     * @brief 追踪中断上下文任务恢复事件
     * 触发时机：在中断服务例程中通过xTaskResumeFromISR恢复任务后
     * @param pxTaskToResume 被恢复任务指针
     */
    #define traceTASK_RESUME_FROM_ISR( pxTaskToResume )
#endif

/* 任务时钟节拍递增追踪 ------------------------------------------------*/
#ifndef traceTASK_INCREMENT_TICK
    /**
     * @brief 追踪任务时钟节拍递增事件
     * 触发时机：系统时钟节拍递增时
     * @param xTickCount 当前时钟节拍数
     */
    #define traceTASK_INCREMENT_TICK( xTickCount )
#endif

/* 定时器创建追踪 ------------------------------------------------------*/
#ifndef traceTIMER_CREATE
    /**
     * @brief 追踪定时器创建事件
     * 触发时机：定时器通过xTimerCreate成功创建后
     * @param pxNewTimer 新创建定时器指针
     */
    #define traceTIMER_CREATE( pxNewTimer )
#endif

/* 定时器创建失败追踪 --------------------------------------------------*/
#ifndef traceTIMER_CREATE_FAILED
    /**
     * @brief 追踪定时器创建失败事件
     * 触发时机：定时器通过xTimerCreate创建失败后
     */
    #define traceTIMER_CREATE_FAILED()
#endif

/* 定时器命令发送追踪 --------------------------------------------------*/
#ifndef traceTIMER_COMMAND_SEND
    /**
     * @brief 追踪定时器命令发送事件
     * 触发时机：定时器命令通过xTimerGenericCommand发送后
     * @param xTimer 定时器的句柄
     * @param xMessageID 命令类型（例如，xTimerStart, xTimerStop）
     * @param xMessageValueValue 命令值
     * @param xReturn 命令发送的结果
     */
    #define traceTIMER_COMMAND_SEND( xTimer, xMessageID, xMessageValueValue, xReturn )
#endif

/* 定时器到期追踪 ------------------------------------------------------*/
#ifndef traceTIMER_EXPIRED
    /**
     * @brief 追踪定时器到期事件
     * 触发时机：定时器到期并执行回调函数时
     * @param pxTimer 到期的定时器指针
     */
    #define traceTIMER_EXPIRED( pxTimer )
#endif

/* 定时器命令接收追踪 --------------------------------------------------*/
#ifndef traceTIMER_COMMAND_RECEIVED
    /**
     * @brief 追踪定时器命令接收事件
     * 触发时机：定时器命令在定时器服务任务中被接收时
     * @param pxTimer 定时器的句柄
     * @param xMessageID 命令类型（例如，xTimerStart, xTimerStop）
     * @param xMessageValue 命令值
     */
    #define traceTIMER_COMMAND_RECEIVED( pxTimer, xMessageID, xMessageValue )
#endif

/* 内存分配追踪 --------------------------------------------------------*/
#ifndef traceMALLOC
    /**
     * @brief 追踪内存分配事件
     * 触发时机：内存通过pvPortMalloc分配后
     * @param pvAddress 分配的内存地址
     * @param uiSize 分配的内存大小
     */
    #define traceMALLOC( pvAddress, uiSize )
#endif

/* 内存释放追踪 --------------------------------------------------------*/
#ifndef traceFREE
    /**
     * @brief 追踪内存释放事件
     * 触发时机：内存通过vPortFree释放后
     * @param pvAddress 释放的内存地址
     * @param uiSize 释放的内存大小
     */
    #define traceFREE( pvAddress, uiSize )
#endif

/* 事件组创建追踪 ------------------------------------------------------*/
#ifndef traceEVENT_GROUP_CREATE
    /**
     * @brief 追踪事件组创建事件
     * 触发时机：事件组通过xEventGroupCreate成功创建后
     * @param xEventGroup 新创建事件组指针
     */
    #define traceEVENT_GROUP_CREATE( xEventGroup )
#endif

/* 事件组创建失败追踪 --------------------------------------------------*/
#ifndef traceEVENT_GROUP_CREATE_FAILED
    /**
     * @brief 追踪事件组创建失败事件
     * 触发时机：事件组通过xEventGroupCreate创建失败后
     */
    #define traceEVENT_GROUP_CREATE_FAILED()
#endif

/* 事件组同步阻塞追踪 --------------------------------------------------*/
#ifndef traceEVENT_GROUP_SYNC_BLOCK
    /**
     * @brief 追踪事件组同步阻塞事件
     * 触发时机：任务调用xEventGroupSync进行事件组同步阻塞后
     * @param xEventGroup 事件组指针
     * @param uxBitsToSet 要设置的事件位
     * @param uxBitsToWaitFor 要等待的事件位
     */
    #define traceEVENT_GROUP_SYNC_BLOCK( xEventGroup, uxBitsToSet, uxBitsToWaitFor )
#endif
/*
 * FreeRTOS事件组与任务通知追踪宏配置
 * 作用：提供事件组操作和任务通知的关键事件追踪点
 */

/* 事件组同步结束追踪 --------------------------------------------------*/
#ifndef traceEVENT_GROUP_SYNC_END
    /**
     * @brief 追踪事件组同步结束事件
     * 触发时机：任务通过xEventGroupSync进行事件组同步后
     * @param xEventGroup 事件组指针
     * @param uxBitsToSet 要设置的事件位
     * @param uxBitsToWaitFor 要等待的事件位
     * @param xTimeoutOccurred 同步操作是否超时
     */
    #define traceEVENT_GROUP_SYNC_END( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred ) ( void ) xTimeoutOccurred
#endif

/* 事件组等待位阻塞追踪 ------------------------------------------------*/
#ifndef traceEVENT_GROUP_WAIT_BITS_BLOCK
    /**
     * @brief 追踪事件组等待位阻塞事件
     * 触发时机：任务调用xEventGroupWaitBits进行事件组等待位阻塞时
     * @param xEventGroup 事件组指针
     * @param uxBitsToWaitFor 要等待的事件位
     */
    #define traceEVENT_GROUP_WAIT_BITS_BLOCK( xEventGroup, uxBitsToWaitFor )
#endif

/* 事件组等待位结束追踪 ------------------------------------------------*/
#ifndef traceEVENT_GROUP_WAIT_BITS_END
    /**
     * @brief 追踪事件组等待位结束事件
     * 触发时机：任务通过xEventGroupWaitBits进行事件组等待位后
     * @param xEventGroup 事件组指针
     * @param uxBitsToWaitFor 要等待的事件位
     * @param xTimeoutOccurred 等待操作是否超时
     */
    #define traceEVENT_GROUP_WAIT_BITS_END( xEventGroup, uxBitsToWaitFor, xTimeoutOccurred ) ( void ) xTimeoutOccurred
#endif

/* 事件组清除位追踪 ----------------------------------------------------*/
#ifndef traceEVENT_GROUP_CLEAR_BITS
    /**
     * @brief 追踪事件组清除位事件
     * 触发时机：事件组通过xEventGroupClearBits清除位后
     * @param xEventGroup 事件组指针
     * @param uxBitsToClear 要清除的事件位
     */
    #define traceEVENT_GROUP_CLEAR_BITS( xEventGroup, uxBitsToClear )
#endif

/* 中断上下文事件组清除位追踪 ------------------------------------------*/
#ifndef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR
    /**
     * @brief 追踪中断上下文事件组清除位事件
     * 触发时机：在中断服务例程中通过xEventGroupClearBitsFromISR清除位后
     * @param xEventGroup 事件组指针
     * @param uxBitsToClear 要清除的事件位
     */
    #define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR( xEventGroup, uxBitsToClear )
#endif

/* 事件组设置位追踪 ----------------------------------------------------*/
#ifndef traceEVENT_GROUP_SET_BITS
    /**
     * @brief 追踪事件组设置位事件
     * 触发时机：事件组通过xEventGroupSetBits设置位后
     * @param xEventGroup 事件组指针
     * @param uxBitsToSet 要设置的事件位
     */
    #define traceEVENT_GROUP_SET_BITS( xEventGroup, uxBitsToSet )
#endif

/* 中断上下文事件组设置位追踪 ------------------------------------------*/
#ifndef traceEVENT_GROUP_SET_BITS_FROM_ISR
    /**
     * @brief 追踪中断上下文事件组设置位事件
     * 触发时机：在中断服务例程中通过xEventGroupSetBitsFromISR设置位后
     * @param xEventGroup 事件组指针
     * @param uxBitsToSet 要设置的事件位
     */
    #define traceEVENT_GROUP_SET_BITS_FROM_ISR( xEventGroup, uxBitsToSet )
#endif

/* 事件组删除追踪 ------------------------------------------------------*/
#ifndef traceEVENT_GROUP_DELETE
    /**
     * @brief 追踪事件组删除事件
     * 触发时机：事件组通过vEventGroupDelete删除后
     * @param xEventGroup 被删除事件组指针
     */
    #define traceEVENT_GROUP_DELETE( xEventGroup )
#endif

/* 延迟函数调用追踪 ----------------------------------------------------*/
#ifndef tracePEND_FUNC_CALL
    /**
     * @brief 追踪延迟函数调用事件
     * 触发时机：任务通过xTimerPendFunctionCall进行延迟函数调用后
     * @param xFunctionToPend 延迟调用的函数指针
     * @param pvParameter1 第一个参数
     * @param ulParameter2 第二个参数
     * @param ret 函数调用的结果
     */
    #define tracePEND_FUNC_CALL(xFunctionToPend, pvParameter1, ulParameter2, ret)
#endif

/* 中断上下文延迟函数调用追踪 ------------------------------------------*/
#ifndef tracePEND_FUNC_CALL_FROM_ISR
    /**
     * @brief 追踪中断上下文延迟函数调用事件
     * 触发时机：在中断服务例程中通过xTimerPendFunctionCallFromISR进行延迟函数调用后
     * @param xFunctionToPend 延迟调用的函数指针
     * @param pvParameter1 第一个参数
     * @param ulParameter2 第二个参数
     * @param ret 函数调用的结果
     */
    #define tracePEND_FUNC_CALL_FROM_ISR(xFunctionToPend, pvParameter1, ulParameter2, ret)
#endif

/* 队列注册追踪 --------------------------------------------------------*/
#ifndef traceQUEUE_REGISTRY_ADD
    /**
     * @brief 追踪队列注册事件
     * 触发时机：队列通过vQueueAddToRegistry进行注册后
     * @param xQueue 队列句柄
     * @param pcQueueName 队列名称
     */
    #define traceQUEUE_REGISTRY_ADD(xQueue, pcQueueName)
#endif

/* 任务通知取阻塞追踪 --------------------------------------------------*/
#ifndef traceTASK_NOTIFY_TAKE_BLOCK
    /**
     * @brief 追踪任务通知取阻塞事件
     * 触发时机：任务调用ulTaskNotifyTake进行通知取阻塞时
     */
    #define traceTASK_NOTIFY_TAKE_BLOCK()
#endif

/* 任务通知取追踪 ------------------------------------------------------*/
#ifndef traceTASK_NOTIFY_TAKE
    /**
     * @brief 追踪任务通知取事件
     * 触发时机：任务通过ulTaskNotifyTake进行通知取后
     */
    #define traceTASK_NOTIFY_TAKE()
#endif
/*
 * FreeRTOS任务通知与运行时间统计追踪宏配置
 * 作用：提供任务通知和运行时间统计的关键事件追踪点
 */

/* 任务通知等待阻塞追踪 ------------------------------------------------*/
#ifndef traceTASK_NOTIFY_WAIT_BLOCK
    /**
     * @brief 追踪任务通知等待阻塞事件
     * 触发时机：任务调用ulTaskNotifyTake或ulTaskNotifyWait进行通知等待阻塞时
     */
    #define traceTASK_NOTIFY_WAIT_BLOCK()
#endif

/* 任务通知等待追踪 ----------------------------------------------------*/
#ifndef traceTASK_NOTIFY_WAIT
    /**
     * @brief 追踪任务通知等待事件
     * 触发时机：任务通过ulTaskNotifyTake或ulTaskNotifyWait进行通知等待后
     */
    #define traceTASK_NOTIFY_WAIT()
#endif

/* 任务通知追踪 --------------------------------------------------------*/
#ifndef traceTASK_NOTIFY
    /**
     * @brief 追踪任务通知事件
     * 触发时机：任务通过vTaskNotifyGive或vTaskNotify进行通知后
     */
    #define traceTASK_NOTIFY()
#endif

/* 中断上下文任务通知追踪 ----------------------------------------------*/
#ifndef traceTASK_NOTIFY_FROM_ISR
    /**
     * @brief 追踪中断上下文任务通知事件
     * 触发时机：在中断服务例程中通过xTaskNotifyFromISR或xTaskNotifyGiveFromISR进行通知后
     */
    #define traceTASK_NOTIFY_FROM_ISR()
#endif

/* 中断上下文任务通知发送追踪 ------------------------------------------*/
#ifndef traceTASK_NOTIFY_GIVE_FROM_ISR
    /**
     * @brief 追踪中断上下文任务通知发送事件
     * 触发时机：在中断服务例程中通过xTaskNotifyGiveFromISR发送通知后
     */
    #define traceTASK_NOTIFY_GIVE_FROM_ISR()
#endif

/* 运行时间统计配置 ----------------------------------------------------*/
#ifndef configGENERATE_RUN_TIME_STATS
    /**
     * @brief 配置是否生成运行时间统计信息
     * 默认值：0（不生成）
     */
    #define configGENERATE_RUN_TIME_STATS 0
#endif

#if ( configGENERATE_RUN_TIME_STATS == 1 )

    #ifndef portCONFIGURE_TIMER_FOR_RUN_TIME_STATS
        /**
         * @brief 配置运行时间统计的定时器
         * 如果configGENERATE_RUN_TIME_STATS被定义为1，则必须定义portCONFIGURE_TIMER_FOR_RUN_TIME_STATS。
         * portCONFIGURE_TIMER_FOR_RUN_TIME_STATS应调用一个端口层函数来设置一个外设定时器/计数器，该定时器/计数器可以作为运行时间计数器的时间基准。
         * @error 如果未定义portCONFIGURE_TIMER_FOR_RUN_TIME_STATS，则会触发一个编译错误。
         */
        #error If configGENERATE_RUN_TIME_STATS is defined then portCONFIGURE_TIMER_FOR_RUN_TIME_STATS must also be defined.  portCONFIGURE_TIMER_FOR_RUN_TIME_STATS should call a port layer function to setup a peripheral timer/counter that can then be used as the run time counter time base.
    #endif /* portCONFIGURE_TIMER_FOR_RUN_TIME_STATS */

    #ifndef portGET_RUN_TIME_COUNTER_VALUE
        #ifndef portALT_GET_RUN_TIME_COUNTER_VALUE
            /**
             * @brief 获取运行时间计数器的值
             * 如果configGENERATE_RUN_TIME_STATS被定义为1，则必须定义portGET_RUN_TIME_COUNTER_VALUE或portALT_GET_RUN_TIME_COUNTER_VALUE。
             * @error 如果未定义portGET_RUN_TIME_COUNTER_VALUE和portALT_GET_RUN_TIME_COUNTER_VALUE，则会触发一个编译错误。
             */
            #error If configGENERATE_RUN_TIME_STATS is defined then either portGET_RUN_TIME_COUNTER_VALUE or portALT_GET_RUN_TIME_COUNTER_VALUE must also be defined.  See the examples provided and the FreeRTOS web site for more information.
        #endif /* portALT_GET_RUN_TIME_COUNTER_VALUE */
    #endif /* portGET_RUN_TIME_COUNTER_VALUE */

#endif /* configGENERATE_RUN_TIME_STATS */

/* 配置运行时间统计定时器 ----------------------------------------------*/
#ifndef portCONFIGURE_TIMER_FOR_RUN_TIME_STATS
    /**
     * @brief 配置运行时间统计的定时器
     * 如果configGENERATE_RUN_TIME_STATS被定义为1，则必须定义portCONFIGURE_TIMER_FOR_RUN_TIME_STATS。
     * portCONFIGURE_TIMER_FOR_RUN_TIME_STATS应调用一个端口层函数来设置一个外设定时器/计数器，该定时器/计数器可以作为运行时间计数器的时间基准。
     */
    #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#endif

/*
 * FreeRTOS运行时间统计、内存分配、任务优先级、中断处理、空闲时间处理、队列集合、浮点运算、时间片轮转、应用定义特权函数、统计格式化函数、中断优先级断言、跟踪设施等配置
 * 作用：提供系统关键配置和事件追踪点
 */

/* 配置运行时间统计定时器 ----------------------------------------------*/
#ifndef portCONFIGURE_TIMER_FOR_RUN_TIME_STATS
    /**
     * @brief 配置运行时间统计的定时器
     * 如果configGENERATE_RUN_TIME_STATS被定义为1，则必须定义portCONFIGURE_TIMER_FOR_RUN_TIME_STATS。
     * portCONFIGURE_TIMER_FOR_RUN_TIME_STATS应调用一个端口层函数来设置一个外设定时器/计数器，该定时器/计数器可以作为运行时间计数器的时间基准。
     */
    #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#endif

/* 使用malloc失败钩子 --------------------------------------------------*/
#ifndef configUSE_MALLOC_FAILED_HOOK
    /**
     * @brief 配置是否使用malloc失败钩子
     * 默认值：0（不使用）
     */
    #define configUSE_MALLOC_FAILED_HOOK 0
#endif

/* 任务优先级位 --------------------------------------------------------*/
#ifndef portPRIVILEGE_BIT
    /**
     * @brief 定义任务优先级中的特权位
     * 默认值：0x00（无特权位）
     */
    #define portPRIVILEGE_BIT ( ( UBaseType_t ) 0x00 )
#endif

/* API中的任务让出 ----------------------------------------------------*/
#ifndef portYIELD_WITHIN_API
    /**
     * @brief 在API中使用的任务让出宏
     * 默认值：portYIELD
     */
    #define portYIELD_WITHIN_API portYIELD
#endif

/* 抑制节拍和睡眠 ----------------------------------------------------*/
#ifndef portSUPPRESS_TICKS_AND_SLEEP
    /**
     * @brief 抑制节拍并在指定的空闲时间内进入睡眠模式
     * 触发时机：系统进入空闲状态时
     * @param xExpectedIdleTime 预期的空闲时间
     */
    #define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime )
#endif

/* 预期空闲时间前睡眠 ------------------------------------------------*/
#ifndef configEXPECTED_IDLE_TIME_BEFORE_SLEEP
    /**
     * @brief 配置预期空闲时间前进行睡眠处理的阈值
     * 默认值：2（单位为节拍）
     */
    #define configEXPECTED_IDLE_TIME_BEFORE_SLEEP 2
#endif

/* 检查预期空闲时间是否小于2 -------------------------------------------*/
#if configEXPECTED_IDLE_TIME_BEFORE_SLEEP < 2
    /**
     * @error configEXPECTED_IDLE_TIME_BEFORE_SLEEP必须不小于2
     */
    #error configEXPECTED_IDLE_TIME_BEFORE_SLEEP must not be less than 2
#endif

/* 使用无节拍空闲模式 --------------------------------------------------*/
#ifndef configUSE_TICKLESS_IDLE
    /**
     * @brief 配置是否使用无节拍空闲模式
     * 默认值：0（不使用）
     */
    #define configUSE_TICKLESS_IDLE 0
#endif

/* 睡眠前处理 --------------------------------------------------------*/
#ifndef configPRE_SLEEP_PROCESSING
    /**
     * @brief 定义睡眠前的处理函数
     * 触发时机：系统进入睡眠模式前
     * @param x 预期的空闲时间
     */
    #define configPRE_SLEEP_PROCESSING( x )
#endif

/* 睡眠后处理 --------------------------------------------------------*/
#ifndef configPOST_SLEEP_PROCESSING
    /**
     * @brief 定义睡眠后的处理函数
     * 触发时机：系统从睡眠模式恢复后
     * @param x 实际的睡眠时间
     */
    #define configPOST_SLEEP_PROCESSING( x )
#endif

/* 使用队列集合 --------------------------------------------------------*/
#ifndef configUSE_QUEUE_SETS
    /**
     * @brief 配置是否使用队列集合
     * 默认值：0（不使用）
     */
    #define configUSE_QUEUE_SETS 0
#endif

/* 任务使用浮点运算 ----------------------------------------------------*/
#ifndef portTASK_USES_FLOATING_POINT
    /**
     * @brief 定义任务使用浮点运算的宏
     * 触发时机：任务需要使用浮点运算时
     */
    #define portTASK_USES_FLOATING_POINT()
#endif

/* 使用时间片轮转 ----------------------------------------------------*/
#ifndef configUSE_TIME_SLICING
    /**
     * @brief 配置是否使用时间片轮转调度
     * 默认值：1（使用）
     */
    #define configUSE_TIME_SLICING 1
#endif

/* 包含应用定义的特权函数 ----------------------------------------------*/
#ifndef configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS
    /**
     * @brief 配置是否包含应用定义的特权函数
     * 默认值：0（不包含）
     */
    #define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0
#endif

/* 使用统计格式化函数 --------------------------------------------------*/
#ifndef configUSE_STATS_FORMATTING_FUNCTIONS
    /**
     * @brief 配置是否使用统计格式化函数
     * 默认值：0（不使用）
     */
    #define configUSE_STATS_FORMATTING_FUNCTIONS 0
#endif

/* 断言无效中断优先级 --------------------------------------------------*/
#ifndef portASSERT_IF_INTERRUPT_PRIORITY_INVALID
    /**
     * @brief 断言无效的中断优先级
     * 触发时机：检测到无效的中断优先级时
     */
    #define portASSERT_IF_INTERRUPT_PRIORITY_INVALID()
#endif

/* 使用跟踪设施 --------------------------------------------------------*/
#ifndef configUSE_TRACE_FACILITY
    /**
     * @brief 配置是否使用跟踪设施
     * 默认值：0（不使用）
     */
    #define configUSE_TRACE_FACILITY 0
#endif

/* 覆盖测试标记 --------------------------------------------------------*/
#ifndef mtCOVERAGE_TEST_MARKER
    /**
     * @brief 定义覆盖测试标记宏
     * 触发时机：用于代码覆盖测试
     */
    #define mtCOVERAGE_TEST_MARKER()
#endif
/* FreeRTOS 配置头文件 */
/**
 * @功能概述 本代码段用于定义FreeRTOS的默认配置选项，并提供配置参数的合法性校验。
 * @主要作用
 * 1. 为未明确配置的编译选项提供默认值，确保系统基础功能的可用性
 * 2. 实施配置参数间的逻辑关系检查，防止不兼容的配置组合
 * 3. 定义测试相关的辅助宏，支持代码覆盖率测试和中断上下文验证
 * 4. 管理内存分配策略的编译开关，协调静态/动态内存分配机制
 */

#ifndef mtCOVERAGE_TEST_DELAY
	/* 代码覆盖率测试延迟宏（空实现） 
	 * 用于在覆盖率测试中插入人为延迟，模拟实际执行时序
	 * 默认空操作，实际测试时可重定义为具体延迟实现 */
	#define mtCOVERAGE_TEST_DELAY()
#endif

#ifndef portASSERT_IF_IN_ISR
	/* 中断服务例程断言检查宏（空实现）
	 * 当检测到在中断上下文中执行非法操作时触发断言
	 * 默认不启用，需在移植层根据具体硬件实现 */
	#define portASSERT_IF_IN_ISR()
#endif

#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
	/* 端口优化任务选择开关（默认关闭）
	 * 设置为1时启用特定硬件平台的任务选择优化算法
	 * 保持默认0值以保证跨平台兼容性 */
	#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#endif

#ifndef configAPPLICATION_ALLOCATED_HEAP
	/* 应用托管堆内存开关（默认关闭）
	 * 设置为1时要求应用层提供堆内存，否则使用编译器分配
	 * 默认0表示由FreeRTOS自动管理内存分配 */
	#define configAPPLICATION_ALLOCATED_HEAP 0
#endif

#ifndef configUSE_TASK_NOTIFICATIONS
	/* 任务通知功能开关（默认开启）
	 * 启用轻量级任务间通信机制
	 * 保持1以使用高效的任务通知替代传统队列 */
	#define configUSE_TASK_NOTIFICATIONS 1
#endif

#ifndef portTICK_TYPE_IS_ATOMIC
	/* 节拍类型原子性标识（默认非原子）
	 * 标识TickType_t变量的读写是否需要原子操作
	 * 设置为0时使用临界区保护tick计数访问 */
	#define portTICK_TYPE_IS_ATOMIC 0
#endif

#ifndef configSUPPORT_STATIC_ALLOCATION
	/* 静态内存分配支持（默认关闭）
	 * 启用后允许使用预分配内存创建系统对象
	 * 默认0保持向后兼容，适用于动态分配方案 */
	#define configSUPPORT_STATIC_ALLOCATION 0
#endif

#ifndef configSUPPORT_DYNAMIC_ALLOCATION
	/* 动态内存分配支持（默认开启）
	 * 控制是否允许运行时从堆中分配内存
	 * 默认1保持传统行为，与静态分配形成互补 */
	#define configSUPPORT_DYNAMIC_ALLOCATION 1
#endif

/* 配置参数完整性检查 -------------------------------------------------------*/
#if( configUSE_TICKLESS_IDLE != 0 )
	/* Tickless低功耗模式依赖检查 
	 * 当启用Tickless空闲模式时，必须包含任务挂起功能 */
	#if( INCLUDE_vTaskSuspend != 1 )
		/* 强制依赖错误：vTaskSuspend功能是Tickless模式的必要条件 
		 * 因低功耗状态恢复需要任务挂起/恢复机制支持 */
		#error INCLUDE_vTaskSuspend must be set to 1 if configUSE_TICKLESS_IDLE is not set to 0
	#endif /* INCLUDE_vTaskSuspend */
#endif /* configUSE_TICKLESS_IDLE */

#if( ( configSUPPORT_STATIC_ALLOCATION == 0 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 0 ) )
	/* 内存分配模式矛盾检测
	 * 静态和动态分配模式不能同时关闭
	 * 系统需要至少一种内存分配机制来创建内核对象 */
	#error configSUPPORT_STATIC_ALLOCATION and configSUPPORT_DYNAMIC_ALLOCATION cannot both be 0, but can both be 1.
#endif

#if( ( configUSE_RECURSIVE_MUTEXES == 1 ) && ( configUSE_MUTEXES != 1 ) )
	/* 递归互斥量依赖检查 
	 * 递归互斥量实现依赖基础互斥量功能
	 * 必须先启用普通互斥量才能使用递归版本 */
	#error configUSE_MUTEXES must be set to 1 to use recursive mutexes
#endif

/* FreeRTOS 配置兼容性处理模块 */
/**
 * @功能概述 本代码段实现FreeRTOS核心功能的兼容性处理，包含：
 *          1. 节拍类型访问的原子性控制
 *          2. 版本迭代的API向后兼容支持
 *          3. 已弃用API的编译时检测
 * @核心机制
 *  - 根据硬件原子性能力自动选择临界区保护策略
 *  - 通过类型别名实现新旧版本API的平滑过渡
 *  - 强制检测并阻止使用已移除的替代API
 */

/* 节拍类型原子访问控制 -----------------------------------------------------*/
#if( portTICK_TYPE_IS_ATOMIC == 0 )
	/* 非原子节拍类型处理方案 
	 * 当TickType_t类型变量无法原子访问时：
	 * 1. 定义完整的临界区保护宏
	 * 2. 映射到标准端口临界区操作方法
	 * 3. 确保tick计数访问的线程安全 */
	#define portTICK_TYPE_ENTER_CRITICAL() portENTER_CRITICAL()      // 进入临界区（完整保护）
	#define portTICK_TYPE_EXIT_CRITICAL() portEXIT_CRITICAL()        // 退出临界区
	#define portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()          // ISR中断掩码设置
	#define portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( x ) portCLEAR_INTERRUPT_MASK_FROM_ISR( ( x ) )  // ISR中断掩码清除
#else
	/* 原子节拍类型优化方案 
	 * 当TickType_t支持原子访问时：
	 * 1. 定义空操作宏消除性能开销
	 * 2. 简化中断屏蔽操作返回伪值
	 * 3. 利用硬件特性提升系统性能 */
	#define portTICK_TYPE_ENTER_CRITICAL()          // 空操作（无需保护）
	#define portTICK_TYPE_EXIT_CRITICAL()           // 空操作
	#define portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR() 0     // 返回0伪掩码值
	#define portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( x ) ( void ) x  // 参数转换避免警告
#endif

/* 版本兼容性处理 ---------------------------------------------------------*/
#ifndef configENABLE_BACKWARD_COMPATIBILITY
	/* 向后兼容性开关（默认开启）
	 * 控制是否启用V8之前版本的类型别名
     * 建议新项目设为0，老项目升级期间设为1 */
	#define configENABLE_BACKWARD_COMPATIBILITY 1
#endif

#if configENABLE_BACKWARD_COMPATIBILITY == 1
	/* 类型别名映射表（新旧API兼容层）
	 * 左侧为旧版本标识符，右侧为新版本命名规范 */
	#define eTaskStateGet eTaskGetState              // 任务状态查询函数
	#define portTickType TickType_t                  // 系统节拍类型
	#define xTaskHandle TaskHandle_t                 // 任务句柄
	#define xQueueHandle QueueHandle_t               // 队列句柄
	#define xSemaphoreHandle SemaphoreHandle_t       // 信号量句柄
	#define xQueueSetHandle QueueSetHandle_t         // 队列集合句柄
	#define xQueueSetMemberHandle QueueSetMemberHandle_t  // 队列集合成员句柄
	#define xTimeOutType TimeOut_t                   // 超时参数类型
	#define xMemoryRegion MemoryRegion_t             // 内存区域描述符
	#define xTaskParameters TaskParameters_t        // 任务参数结构
	#define xTaskStatusType	TaskStatus_t            // 任务状态结构
	#define xTimerHandle TimerHandle_t              // 定时器句柄
	#define xCoRoutineHandle CoRoutineHandle_t      // 协程句柄
	#define pdTASK_HOOK_CODE TaskHookFunction_t      // 任务钩子函数类型
	#define portTICK_RATE_MS portTICK_PERIOD_MS      // 节拍周期(ms)
	#define pcTaskGetTaskName pcTaskGetName          // 任务名获取函数
	#define pcTimerGetTimerName pcTimerGetName       // 定时器名获取
	#define pcQueueGetQueueName pcQueueGetName       // 队列名获取
	#define vTaskGetTaskInfo vTaskGetInfo            // 任务信息获取

	/* 调度器内部兼容定义（非公开API）
	 * 保留旧版本数据结构命名以实现代码级兼容 */
	#define tmrTIMER_CALLBACK TimerCallbackFunction_t  // 定时器回调类型
	#define pdTASK_CODE TaskFunction_t               // 任务函数类型
	#define xListItem ListItem_t                     // 链表项
	#define xList List_t                             // 链表结构
#endif /* configENABLE_BACKWARD_COMPATIBILITY */

/* 已弃用API检测 ---------------------------------------------------------*/
#if( configUSE_ALTERNATIVE_API != 0 )
	/* 替代API废止错误检测
	 * V9.0+版本已完全移除替代API
	 * 必须迁移到标准队列API集 */
	#error The alternative API was deprecated some time ago, and was removed in FreeRTOS V9.0 0
#endif

/* FreeRTOS 静态类型定义与浮点支持模块 */
/**
 * @功能概述 本代码段实现以下核心功能：
 *          1. 任务浮点支持配置
 *          2. 静态内存分配所需数据结构的类型定义
 *          3. 跨版本数据隐藏策略的实施
 * @设计理念
 *  - 通过"虚设结构体"保持内存布局的二进制兼容性
 *  - 严格数据封装保护内核数据结构
 *  - 支持不同硬件架构的灵活配置
 */

/* 浮点运算支持配置 -------------------------------------------------------*/
#ifndef configUSE_TASK_FPU_SUPPORT
	/* 任务浮点单元支持开关（默认开启）
	 * 设置为1时允许任务上下文保存FPU寄存器
	 * 即使硬件支持FPU，设为0可节省任务内存开销
	 * 注意：部分移植层可能不支持此配置 */
	#define configUSE_TASK_FPU_SUPPORT 1
#endif

/* 静态数据结构定义 -------------------------------------------------------*/
/*
 * 静态链表项结构（数据隐藏实现）
 * 保证与实际链表项结构尺寸/对齐完全一致
 * 成员命名不反映实际用途，防止不当访问
 */
struct xSTATIC_LIST_ITEM
{
	TickType_t xDummy1;        /* 模拟时间相关字段 */
	void *pvDummy2[ 4 ];       /* 保留指针空间：前向/后向链接+数据存储 */
};
typedef struct xSTATIC_LIST_ITEM StaticListItem_t;

/* 精简静态链表项（用于队列最小化存储） */
struct xSTATIC_MINI_LIST_ITEM
{
	TickType_t xDummy1;        /* 时间基准字段占位 */
	void *pvDummy2[ 2 ];       /* 双指针空间：链表链接基础 */
};
typedef struct xSTATIC_MINI_LIST_ITEM StaticMiniListItem_t;

/* 静态链表容器结构（队列实现基础） */
typedef struct xSTATIC_LIST
{
	UBaseType_t uxDummy1;           /* 链表项计数器占位 */
	void *pvDummy2;                 /* 链表头指针占位 */
	StaticMiniListItem_t xDummy3;   /* 尾项标记（哨兵节点） */
} StaticList_t;

/* 静态任务控制块（TCB）结构体 
 * 保证与实际TCB内存布局完全一致
 * 条件编译字段覆盖所有可能的配置组合 */
typedef struct xSTATIC_TCB
{
	void				*pxDummy1;  /* 栈顶指针占位 */
	#if ( portUSING_MPU_WRAPPERS == 1 )
		xMPU_SETTINGS	xDummy2;    /* MPU内存保护单元设置 */
	#endif
	StaticListItem_t	xDummy3[ 2 ]; /* 事件链表项（就绪/阻塞列表） */
	UBaseType_t			uxDummy5;    /* 运行时优先级占位 */
	void				*pxDummy6;   /* 任务名指针或直接存储 */
	uint8_t				ucDummy7[ configMAX_TASK_NAME_LEN ]; /* 任务名字符缓冲区 */
	#if ( portSTACK_GROWTH > 0 )
		void			*pxDummy8;   /* 栈增长方向标记 */
	#endif
	#if ( portCRITICAL_NESTING_IN_TCB == 1 )
		UBaseType_t		uxDummy9;    /* 临界区嵌套计数器 */
	#endif
	#if ( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t		uxDummy10[ 2 ]; /* 跟踪调试字段 */
	#endif
	#if ( configUSE_MUTEXES == 1 )
		UBaseType_t		uxDummy12[ 2 ]; /* 互斥量继承优先级 */
	#endif
	#if ( configUSE_APPLICATION_TASK_TAG == 1 )
		void			*pxDummy14;  /* 任务标签指针 */
	#endif
	#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
		void			*pvDummy15[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ]; /* TLS指针数组 */
	#endif
	#if ( configGENERATE_RUN_TIME_STATS == 1 )
		uint32_t		ulDummy16;   /* 运行时间统计计数器 */
	#endif
	#if ( configUSE_NEWLIB_REENTRANT == 1 )
		struct	_reent	xDummy17;    /* Newlib可重入结构 */
	#endif
	#if ( configUSE_TASK_NOTIFICATIONS == 1 )
		uint32_t 		ulDummy18;   /* 通知值存储 */
		uint8_t 		ucDummy19;   /* 通知状态标志 */
	#endif
	#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		uint8_t			uxDummy20;   /* 内存分配模式标记 */
	#endif

} StaticTask_t;
/* FreeRTOS 静态内核对象定义模块 */
/**
 * @功能概述 本代码段定义核心内核对象的静态内存分配结构体，包括：
 *          1. 队列/信号量静态类型
 *          2. 事件组静态类型
 *          3. 软件定时器静态类型
 * @设计原则
 *  - 严格内存对齐保证与动态分配对象的一致性
 *  - 通过模糊成员命名阻止直接结构操作
 *  - 条件编译适配不同功能配置方案
 */

/* 静态队列/信号量结构体 ---------------------------------------------------*/
typedef struct xSTATIC_QUEUE
{
	/* 队列核心控制字段 */
	void *pvDummy1[ 3 ];  /* 队列头尾指针+项目计数器占位 */
	
	union {
		void *pvDummy2;      /* 队列写入端指针（通用场景） */
		UBaseType_t uxDummy2; /* 信号量计数存储（信号量专用） */
	} u;  /* 联合体实现队列/信号量的多态存储 */
	
	StaticList_t xDummy3[ 2 ]; /* 队列等待列表（发送/接收阻塞任务） */
	UBaseType_t uxDummy4[ 3 ]; /* 队列长度/项目大小等元数据 */
	uint8_t ucDummy5[ 2 ];     /* 队列锁状态与类型标记 */

	#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		uint8_t ucDummy6;    /* 分配模式标识：0-静态 1-动态 */
	#endif

	#if ( configUSE_QUEUE_SETS == 1 )
		void *pvDummy7;      /* 队列集合关联指针 */
	#endif

	#if ( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t uxDummy8; /* 跟踪ID */
		uint8_t ucDummy9;     /* 调试标记 */
	#endif

} StaticQueue_t;
typedef StaticQueue_t StaticSemaphore_t; /* 信号量复用队列结构 */

/* 静态事件组结构体 -------------------------------------------------------*/
typedef struct xSTATIC_EVENT_GROUP
{
	TickType_t xDummy1;      /* 事件触发时间戳 */
	StaticList_t xDummy2;    /* 事件等待任务列表 */
	
	#if( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t uxDummy3; /* 事件模式跟踪标记 */
	#endif
	
	#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		uint8_t ucDummy4;    /* 内存分配标识位 */
	#endif

} StaticEventGroup_t;

/* 静态软件定时器结构体 ---------------------------------------------------*/
typedef struct xSTATIC_TIMER
{
	void				*pvDummy1;   /* 定时器回调函数指针 */
	StaticListItem_t	xDummy2;     /* 定时器链表节点 */
	TickType_t			xDummy3;     /* 周期/单次模式配置值 */
	UBaseType_t			uxDummy4;    /* 定时器ID */
	void 				*pvDummy5[ 2 ]; /* 参数存储与自动重载标志 */
	
	#if( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t		uxDummy6;    /* 调试跟踪计数器 */
	#endif
	
	#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		uint8_t 		ucDummy7;    /* 创建方式标记 */
	#endif

} StaticTimer_t;

#ifdef __cplusplus
}
#endif

#endif /* INC_FREERTOS_H */


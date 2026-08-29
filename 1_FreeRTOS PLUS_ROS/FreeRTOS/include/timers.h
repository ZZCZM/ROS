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


#ifndef TIMERS_H
#define TIMERS_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h must appear in source files before include timers.h"
#endif

/*lint -e537 This headers are only multiply included if the application code
happens to also be including task.h. */
#include "task.h"
/*lint +e537 */

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/

/* IDs for commands that can be sent/received on the timer queue.  These are to
be used solely through the macros that make up the public software timer API,
as defined below.  The commands that are sent from interrupts must use the
highest numbers as tmrFIRST_FROM_ISR_COMMAND is used to determine if the task
or interrupt version of the queue send function should be used. */
/**
 * @brief 软件定时器命令码宏定义
 * 
 * 这些宏定义了软件定时器内部操作命令，用于通过定时器服务任务队列传递控制指令。
 * 所有命令必须通过xTimer*系列API函数间接使用，禁止直接操作命令值。
 */

/* 回调执行命令（特殊处理类） */
#define tmrCOMMAND_EXECUTE_CALLBACK_FROM_ISR 	( ( BaseType_t ) -2 ) /**< 中断上下文回调触发命令 */
#define tmrCOMMAND_EXECUTE_CALLBACK				( ( BaseType_t ) -1 ) /**< 任务上下文回调触发命令 */

/* 标准定时器操作命令（任务上下文使用） */
#define tmrCOMMAND_START_DONT_TRACE				( ( BaseType_t ) 0 )  /**< 无追踪的启动命令（内部调试用） */
#define tmrCOMMAND_START					    ( ( BaseType_t ) 1 )  /**< 启动/重启定时器命令 */
#define tmrCOMMAND_RESET						( ( BaseType_t ) 2 )  /**< 重置定时器计数命令 */
#define tmrCOMMAND_STOP							( ( BaseType_t ) 3 )  /**< 停止定时器命令 */
#define tmrCOMMAND_CHANGE_PERIOD				( ( BaseType_t ) 4 )  /**< 修改定时周期命令 */
#define tmrCOMMAND_DELETE						( ( BaseType_t ) 5 )  /**< 删除定时器命令 */

/* 中断安全操作命令（从ISR调用） */
#define tmrFIRST_FROM_ISR_COMMAND				( ( BaseType_t ) 6 )  /**< 中断命令起始标识 */
#define tmrCOMMAND_START_FROM_ISR				( ( BaseType_t ) 6 )  /**< 中断安全启动命令 */
#define tmrCOMMAND_RESET_FROM_ISR				( ( BaseType_t ) 7 )  /**< 中断安全重置命令 */
#define tmrCOMMAND_STOP_FROM_ISR				( ( BaseType_t ) 8 )  /**< 中断安全停止命令 */
#define tmrCOMMAND_CHANGE_PERIOD_FROM_ISR		( ( BaseType_t ) 9 )  /**< 中断安全改周期命令 */

/**
 * @note 命令值分配规则：
 * | 命令类型               | 值域范围        | 说明                     |
 * |------------------------|----------------|--------------------------|
 * | 回调命令               | [-2,-1]        | 触发回调的特殊命令        |
 * | 标准任务命令           | [0,5]          | 通过任务上下文API使用     |
 * | 中断安全命令           | ≥6             | 必须通过FromISR后缀API使用|
 *
 * @warning 使用规范：
 * - 带FROM_ISR的命令必须与xTimer*FromISR()函数配套使用
 * - 命令发送前必须验证定时器句柄有效性
 * - 禁止手动构造命令消息包，必须使用官方API
 *
 * @code
 * // 正确用法示例（任务上下文启动定时器）：
 * xTimerStart(xTimerHandle, 0); // 内部转换为tmrCOMMAND_START
 * 
 * // 正确中断上下文用法：
 * BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 * xTimerResetFromISR(xTimerHandle, &xHigherPriorityTaskWoken); // 转换为tmrCOMMAND_RESET_FROM_ISR
 * @endcode
 */


/**
 * @brief 软件定时器句柄类型
 * 
 * 该类型用于引用和管理创建的软件定时器，通过xTimerCreate()返回，
 * 作为所有定时器API函数的操作标识。实际结构体对用户透明，禁止直接访问内部成员。
 * 
 * @note 典型使用流程：
 * @code
 * TimerHandle_t xTimer = xTimerCreate("MyTimer", pdMS_TO_TICKS(100), pdTRUE, NULL, vTimerCallback);
 * xTimerStart(xTimer, 0);
 * @endcode
 */
typedef void * TimerHandle_t;

/**
 * @brief 定时器回调函数原型
 * 
 * 定义软件定时器到期时执行的回调函数格式，
 * 回调在定时器服务任务上下文中执行，需遵循RTOS编程规范。
 * 
 * @param xTimer 触发回调的定时器句柄
 * 
 * @warning 关键限制：
 * - 禁止调用可能导致阻塞的API（如vTaskDelay）
 * - 执行时间应尽量短（建议＜100μs）
 * - 可通过pvTimerGetTimerID()获取关联数据
 */
typedef void (*TimerCallbackFunction_t)( TimerHandle_t xTimer );

/**
 * @brief 延迟执行函数原型
 * 
 * 定义从中断上下文延迟到任务上下文执行的函数格式，
 * 用于在ISR中安全执行复杂逻辑。
 * 
 * @param pvParameter1 自定义参数指针（可NULL）
 * @param ulParameter2 自定义整型参数
 * 
 * @code
 * // 中断中延迟执行示例
 * uint32_t ulData = 0x1234;
 * xTimerPendFunctionCallFromISR(vPendedFunc, &ulData, sizeof(ulData), pxHigherPriorityTaskWoken);
 * @endcode
 */
typedef void (*PendedFunction_t)( void * pvParameter1, uint32_t ulParameter2 );


/**
 * TimerHandle_t xTimerCreate( 	const char * const pcTimerName,
 * 								TickType_t xTimerPeriodInTicks,
 * 								UBaseType_t uxAutoReload,
 * 								void * pvTimerID,
 * 								TimerCallbackFunction_t pxCallbackFunction );
 *
 * Creates a new software timer instance, and returns a handle by which the
 * created software timer can be referenced.
 *
 * Internally, within the FreeRTOS implementation, software timers use a block
 * of memory, in which the timer data structure is stored.  If a software timer
 * is created using xTimerCreate() then the required memory is automatically
 * dynamically allocated inside the xTimerCreate() function.  (see
 * http://www.freertos.org/a00111.html).  If a software timer is created using
 * xTimerCreateStatic() then the application writer must provide the memory that
 * will get used by the software timer.  xTimerCreateStatic() therefore allows a
 * software timer to be created without using any dynamic memory allocation.
 *
 * Timers are created in the dormant state.  The xTimerStart(), xTimerReset(),
 * xTimerStartFromISR(), xTimerResetFromISR(), xTimerChangePeriod() and
 * xTimerChangePeriodFromISR() API functions can all be used to transition a
 * timer into the active state.
 *
 * @param pcTimerName A text name that is assigned to the timer.  This is done
 * purely to assist debugging.  The kernel itself only ever references a timer
 * by its handle, and never by its name.
 *
 * @param xTimerPeriodInTicks The timer period.  The time is defined in tick
 * periods so the constant portTICK_PERIOD_MS can be used to convert a time that
 * has been specified in milliseconds.  For example, if the timer must expire
 * after 100 ticks, then xTimerPeriodInTicks should be set to 100.
 * Alternatively, if the timer must expire after 500ms, then xPeriod can be set
 * to ( 500 / portTICK_PERIOD_MS ) provided configTICK_RATE_HZ is less than or
 * equal to 1000.
 *
 * @param uxAutoReload If uxAutoReload is set to pdTRUE then the timer will
 * expire repeatedly with a frequency set by the xTimerPeriodInTicks parameter.
 * If uxAutoReload is set to pdFALSE then the timer will be a one-shot timer and
 * enter the dormant state after it expires.
 *
 * @param pvTimerID An identifier that is assigned to the timer being created.
 * Typically this would be used in the timer callback function to identify which
 * timer expired when the same callback function is assigned to more than one
 * timer.
 *
 * @param pxCallbackFunction The function to call when the timer expires.
 * Callback functions must have the prototype defined by TimerCallbackFunction_t,
 * which is	"void vCallbackFunction( TimerHandle_t xTimer );".
 *
 * @return If the timer is successfully created then a handle to the newly
 * created timer is returned.  If the timer cannot be created (because either
 * there is insufficient FreeRTOS heap remaining to allocate the timer
 * structures, or the timer period was set to 0) then NULL is returned.
 *
 * Example usage:
 * @verbatim
 * #define NUM_TIMERS 5
 *
 * // An array to hold handles to the created timers.
 * TimerHandle_t xTimers[ NUM_TIMERS ];
 *
 * // An array to hold a count of the number of times each timer expires.
 * int32_t lExpireCounters[ NUM_TIMERS ] = { 0 };
 *
 * // Define a callback function that will be used by multiple timer instances.
 * // The callback function does nothing but count the number of times the
 * // associated timer expires, and stop the timer once the timer has expired
 * // 10 times.
 * void vTimerCallback( TimerHandle_t pxTimer )
 * {
 * int32_t lArrayIndex;
 * const int32_t xMaxExpiryCountBeforeStopping = 10;
 *
 * 	   // Optionally do something if the pxTimer parameter is NULL.
 * 	   configASSERT( pxTimer );
 *
 *     // Which timer expired?
 *     lArrayIndex = ( int32_t ) pvTimerGetTimerID( pxTimer );
 *
 *     // Increment the number of times that pxTimer has expired.
 *     lExpireCounters[ lArrayIndex ] += 1;
 *
 *     // If the timer has expired 10 times then stop it from running.
 *     if( lExpireCounters[ lArrayIndex ] == xMaxExpiryCountBeforeStopping )
 *     {
 *         // Do not use a block time if calling a timer API function from a
 *         // timer callback function, as doing so could cause a deadlock!
 *         xTimerStop( pxTimer, 0 );
 *     }
 * }
 *
 * void main( void )
 * {
 * int32_t x;
 *
 *     // Create then start some timers.  Starting the timers before the scheduler
 *     // has been started means the timers will start running immediately that
 *     // the scheduler starts.
 *     for( x = 0; x < NUM_TIMERS; x++ )
 *     {
 *         xTimers[ x ] = xTimerCreate(    "Timer",       // Just a text name, not used by the kernel.
 *                                         ( 100 * x ),   // The timer period in ticks.
 *                                         pdTRUE,        // The timers will auto-reload themselves when they expire.
 *                                         ( void * ) x,  // Assign each timer a unique id equal to its array index.
 *                                         vTimerCallback // Each timer calls the same callback when it expires.
 *                                     );
 *
 *         if( xTimers[ x ] == NULL )
 *         {
 *             // The timer was not created.
 *         }
 *         else
 *         {
 *             // Start the timer.  No block time is specified, and even if one was
 *             // it would be ignored because the scheduler has not yet been
 *             // started.
 *             if( xTimerStart( xTimers[ x ], 0 ) != pdPASS )
 *             {
 *                 // The timer could not be set into the Active state.
 *             }
 *         }
 *     }
 *
 *     // ...
 *     // Create tasks here.
 *     // ...
 *
 *     // Starting the scheduler will start the timers running as they have already
 *     // been set into the active state.
 *     vTaskStartScheduler();
 *
 *     // Should not reach here.
 *     for( ;; );
 * }
 * @endverbatim
 */
/**
 * @brief 动态创建软件定时器
 * 
 * 该函数在FreeRTOS堆中分配内存并初始化定时器对象，
 * 创建成功后需调用xTimerStart()启动定时器。
 * 
 * @param pcTimerName 定时器标识字符串（调试用）：
 *        - 最大长度受configMAX_TASK_NAME_LEN限制
 *        - 可传入NULL，但影响调试信息可读性
 * @param xTimerPeriodInTicks 定时周期（时钟节拍数）：
 *        - 可用pdMS_TO_TICKS()转换毫秒时间
 *        - 必须＞0
 * @param uxAutoReload 重载模式：
 *        - pdTRUE：自动重载（周期性定时器）
 *        - pdFALSE：单次模式
 * @param pvTimerID 用户自定义标识指针：
 *        - 可通过pvTimerGetTimerID()获取
 *        - 常用于关联定时器与业务数据
 * @param pxCallbackFunction 回调函数指针：
 *        - 定时到期时在守护任务中执行
 *        - 必须符合TimerCallbackFunction_t类型
 * @return TimerHandle_t 定时器句柄：
 *         - 非NULL：创建成功
 *         - NULL：内存不足或参数错误
 * 
 * @note 典型创建流程：
 * @code
 * TimerHandle_t xTimer = xTimerCreate("SensorPoll", 
 *                                   pdMS_TO_TICKS(500),
 *                                   pdTRUE,
 *                                   (void*)0x1234,
 *                                   vSensorPollCallback);
 * if(xTimer != NULL) {
 *     xTimerStart(xTimer, 0);  // 立即启动
 * }
 * @endcode
 * 
 * @warning 重要限制：
 * 1. 创建后必须调用xTimerStart()才会激活
 * 2. 需确保configSUPPORT_DYNAMIC_ALLOCATION=1
 * 3. 回调函数中禁止调用vTaskDelete(NULL)
 */
#if(configSUPPORT_DYNAMIC_ALLOCATION == 1)
    TimerHandle_t xTimerCreate( const char * const pcTimerName,
                                const TickType_t xTimerPeriodInTicks,
                                const UBaseType_t uxAutoReload,
                                void * const pvTimerID,
                                TimerCallbackFunction_t pxCallbackFunction ) PRIVILEGED_FUNCTION;
#endif


/**
 * TimerHandle_t xTimerCreateStatic(const char * const pcTimerName,
 * 									TickType_t xTimerPeriodInTicks,
 * 									UBaseType_t uxAutoReload,
 * 									void * pvTimerID,
 * 									TimerCallbackFunction_t pxCallbackFunction,
 *									StaticTimer_t *pxTimerBuffer );
 *
 * Creates a new software timer instance, and returns a handle by which the
 * created software timer can be referenced.
 *
 * Internally, within the FreeRTOS implementation, software timers use a block
 * of memory, in which the timer data structure is stored.  If a software timer
 * is created using xTimerCreate() then the required memory is automatically
 * dynamically allocated inside the xTimerCreate() function.  (see
 * http://www.freertos.org/a00111.html).  If a software timer is created using
 * xTimerCreateStatic() then the application writer must provide the memory that
 * will get used by the software timer.  xTimerCreateStatic() therefore allows a
 * software timer to be created without using any dynamic memory allocation.
 *
 * Timers are created in the dormant state.  The xTimerStart(), xTimerReset(),
 * xTimerStartFromISR(), xTimerResetFromISR(), xTimerChangePeriod() and
 * xTimerChangePeriodFromISR() API functions can all be used to transition a
 * timer into the active state.
 *
 * @param pcTimerName A text name that is assigned to the timer.  This is done
 * purely to assist debugging.  The kernel itself only ever references a timer
 * by its handle, and never by its name.
 *
 * @param xTimerPeriodInTicks The timer period.  The time is defined in tick
 * periods so the constant portTICK_PERIOD_MS can be used to convert a time that
 * has been specified in milliseconds.  For example, if the timer must expire
 * after 100 ticks, then xTimerPeriodInTicks should be set to 100.
 * Alternatively, if the timer must expire after 500ms, then xPeriod can be set
 * to ( 500 / portTICK_PERIOD_MS ) provided configTICK_RATE_HZ is less than or
 * equal to 1000.
 *
 * @param uxAutoReload If uxAutoReload is set to pdTRUE then the timer will
 * expire repeatedly with a frequency set by the xTimerPeriodInTicks parameter.
 * If uxAutoReload is set to pdFALSE then the timer will be a one-shot timer and
 * enter the dormant state after it expires.
 *
 * @param pvTimerID An identifier that is assigned to the timer being created.
 * Typically this would be used in the timer callback function to identify which
 * timer expired when the same callback function is assigned to more than one
 * timer.
 *
 * @param pxCallbackFunction The function to call when the timer expires.
 * Callback functions must have the prototype defined by TimerCallbackFunction_t,
 * which is "void vCallbackFunction( TimerHandle_t xTimer );".
 *
 * @param pxTimerBuffer Must point to a variable of type StaticTimer_t, which
 * will be then be used to hold the software timer's data structures, removing
 * the need for the memory to be allocated dynamically.
 *
 * @return If the timer is created then a handle to the created timer is
 * returned.  If pxTimerBuffer was NULL then NULL is returned.
 *
 * Example usage:
 * @verbatim
 *
 * // The buffer used to hold the software timer's data structure.
 * static StaticTimer_t xTimerBuffer;
 *
 * // A variable that will be incremented by the software timer's callback
 * // function.
 * UBaseType_t uxVariableToIncrement = 0;
 *
 * // A software timer callback function that increments a variable passed to
 * // it when the software timer was created.  After the 5th increment the
 * // callback function stops the software timer.
 * static void prvTimerCallback( TimerHandle_t xExpiredTimer )
 * {
 * UBaseType_t *puxVariableToIncrement;
 * BaseType_t xReturned;
 *
 *     // Obtain the address of the variable to increment from the timer ID.
 *     puxVariableToIncrement = ( UBaseType_t * ) pvTimerGetTimerID( xExpiredTimer );
 *
 *     // Increment the variable to show the timer callback has executed.
 *     ( *puxVariableToIncrement )++;
 *
 *     // If this callback has executed the required number of times, stop the
 *     // timer.
 *     if( *puxVariableToIncrement == 5 )
 *     {
 *         // This is called from a timer callback so must not block.
 *         xTimerStop( xExpiredTimer, staticDONT_BLOCK );
 *     }
 * }
 *
 *
 * void main( void )
 * {
 *     // Create the software time.  xTimerCreateStatic() has an extra parameter
 *     // than the normal xTimerCreate() API function.  The parameter is a pointer
 *     // to the StaticTimer_t structure that will hold the software timer
 *     // structure.  If the parameter is passed as NULL then the structure will be
 *     // allocated dynamically, just as if xTimerCreate() had been called.
 *     xTimer = xTimerCreateStatic( "T1",             // Text name for the task.  Helps debugging only.  Not used by FreeRTOS.
 *                                  xTimerPeriod,     // The period of the timer in ticks.
 *                                  pdTRUE,           // This is an auto-reload timer.
 *                                  ( void * ) &uxVariableToIncrement,    // A variable incremented by the software timer's callback function
 *                                  prvTimerCallback, // The function to execute when the timer expires.
 *                                  &xTimerBuffer );  // The buffer that will hold the software timer structure.
 *
 *     // The scheduler has not started yet so a block time is not used.
 *     xReturned = xTimerStart( xTimer, 0 );
 *
 *     // ...
 *     // Create tasks here.
 *     // ...
 *
 *     // Starting the scheduler will start the timers running as they have already
 *     // been set into the active state.
 *     vTaskStartScheduler();
 *
 *     // Should not reach here.
 *     for( ;; );
 * }
 * @endverbatim
 */
/**
 * @brief 静态创建软件定时器
 * 
 * 该函数使用预分配的静态内存结构初始化定时器，
 * 适用于禁用动态内存分配或需要精确内存控制的场景。
 * 
 * @param pcTimerName 定时器标识字符串（调试用）
 * @param xTimerPeriodInTicks 定时周期（时钟节拍数）
 * @param uxAutoReload 重载模式（pdTRUE/pdFALSE）
 * @param pvTimerID 用户自定义标识指针
 * @param pxCallbackFunction 定时器回调函数
 * @param pxTimerBuffer 静态内存缓冲区指针：
 *        - 必须指向StaticTimer_t类型变量
 *        - 需保证内存对齐（通常需用__attribute__((aligned)）修饰）
 * @return TimerHandle_t 定时器句柄：
 *         - 非NULL：创建成功
 *         - NULL：缓冲区未对齐或参数错误
 * 
 * @note 典型使用示例：
 * @code
 * // 静态分配定时器结构体（需保证作用域）
 * static StaticTimer_t xTimerBuffer;
 * TimerHandle_t xTimer = xTimerCreateStatic(
 *     "LedBlink",
 *     pdMS_TO_TICKS(1000),
 *     pdTRUE,
 *     (void*)LED_ID_1,
 *     vLedCallback,
 *     &xTimerBuffer
 * );
 * configASSERT(xTimer != NULL); // 生产环境应替换为错误处理
 * @endcode
 * 
 * @warning 关键限制：
 * 1. pxTimerBuffer必须在整个定时器生命周期内有效
 * 2. 静态定时器不可调用xTimerDelete()
 * 3. 需手动保证内存对齐（通常需32字节对齐）
 */
#if(configSUPPORT_STATIC_ALLOCATION == 1)
TimerHandle_t xTimerCreateStatic( const char * const pcTimerName,
                                  const TickType_t xTimerPeriodInTicks,
                                  const UBaseType_t uxAutoReload,
                                  void * const pvTimerID,
                                  TimerCallbackFunction_t pxCallbackFunction,
                                  StaticTimer_t *pxTimerBuffer ) PRIVILEGED_FUNCTION;
#endif
/**
 * @brief 获取定时器用户自定义ID
 * 
 * 该函数用于检索与定时器关联的用户定义标识，
 * 通常用于在共享回调函数中区分不同定时器实例。
 * 
 * @param xTimer 目标定时器句柄
 * @return void* 用户设置的标识指针（默认为xTimerCreate的pvTimerID参数）
 * 
 * @note 典型应用场景：
 * @code
 * void vTimerCallback(TimerHandle_t xTimer) {
 *     int *pID = (int*)pvTimerGetTimerID(xTimer);
 *     printf("Timer %d expired", *pID);
 * }
 * @endcode
 */
void *pvTimerGetTimerID( const TimerHandle_t xTimer ) PRIVILEGED_FUNCTION;

/**
 * @brief 设置定时器用户自定义ID
 * 
 * 动态更新定时器关联的用户标识，用于运行时数据绑定。
 * 
 * @param xTimer 目标定时器句柄
 * @param pvNewID 新标识指针（可指向任意用户数据结构）
 * 
 * @warning 需保证操作原子性：
 * @code
 * vTaskSuspendAll();
 * vTimerSetTimerID(xTimer, pvNewData);
 * xTaskResumeAll();
 * @endcode
 */
void vTimerSetTimerID( TimerHandle_t xTimer, void *pvNewID ) PRIVILEGED_FUNCTION;

/**
 * @brief 查询定时器活跃状态
 * 
 * 检测定时器是否处于可运行状态（已启动且未过期）
 * 
 * @param xTimer 目标定时器句柄
 * @return BaseType_t 状态标识：
 *         - pdTRUE: 活跃（等待或运行中）
 *         - pdFALSE: 休眠（未启动或已停止）
 * 
 * @note 状态转换示意图：
 * [创建]→休眠→[xTimerStart]→活跃→[到期]→休眠（单次模式）
 *                              └─────→活跃（周期模式）
 */
BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer ) PRIVILEGED_FUNCTION;

/**
 * @brief 获取定时器守护任务句柄
 * 
 * 返回定时器服务任务（守护任务）的TCB句柄，
 * 可用于监控任务状态或调整优先级。
 * 
 * @return TaskHandle_t 守护任务句柄：
 *         - 非NULL：有效句柄
 *         - NULL：调度器未启动
 * 
 * @warning 必须在调度器启动后调用
 * 
 * @code
 * // 调整守护任务优先级
 * vTaskPrioritySet(xTimerGetTimerDaemonTaskHandle(), configTIMER_TASK_PRIORITY+1);
 * @endcode
 */
TaskHandle_t xTimerGetTimerDaemonTaskHandle( void ) PRIVILEGED_FUNCTION;


/**
 * BaseType_t xTimerStart( TimerHandle_t xTimer, TickType_t xTicksToWait );
 *
 * Timer functionality is provided by a timer service/daemon task.  Many of the
 * public FreeRTOS timer API functions send commands to the timer service task
 * through a queue called the timer command queue.  The timer command queue is
 * private to the kernel itself and is not directly accessible to application
 * code.  The length of the timer command queue is set by the
 * configTIMER_QUEUE_LENGTH configuration constant.
 *
 * xTimerStart() starts a timer that was previously created using the
 * xTimerCreate() API function.  If the timer had already been started and was
 * already in the active state, then xTimerStart() has equivalent functionality
 * to the xTimerReset() API function.
 *
 * Starting a timer ensures the timer is in the active state.  If the timer
 * is not stopped, deleted, or reset in the mean time, the callback function
 * associated with the timer will get called 'n' ticks after xTimerStart() was
 * called, where 'n' is the timers defined period.
 *
 * It is valid to call xTimerStart() before the scheduler has been started, but
 * when this is done the timer will not actually start until the scheduler is
 * started, and the timers expiry time will be relative to when the scheduler is
 * started, not relative to when xTimerStart() was called.
 *
 * The configUSE_TIMERS configuration constant must be set to 1 for xTimerStart()
 * to be available.
 *
 * @param xTimer The handle of the timer being started/restarted.
 *
 * @param xTicksToWait Specifies the time, in ticks, that the calling task should
 * be held in the Blocked state to wait for the start command to be successfully
 * sent to the timer command queue, should the queue already be full when
 * xTimerStart() was called.  xTicksToWait is ignored if xTimerStart() is called
 * before the scheduler is started.
 *
 * @return pdFAIL will be returned if the start command could not be sent to
 * the timer command queue even after xTicksToWait ticks had passed.  pdPASS will
 * be returned if the command was successfully sent to the timer command queue.
 * When the command is actually processed will depend on the priority of the
 * timer service/daemon task relative to other tasks in the system, although the
 * timers expiry time is relative to when xTimerStart() is actually called.  The
 * timer service/daemon task priority is set by the configTIMER_TASK_PRIORITY
 * configuration constant.
 *
 * Example usage:
 *
 * See the xTimerCreate() API function example usage scenario.
 *
 */
/**
 * @brief 启动/重启软件定时器（任务上下文）
 * 
 * 向定时器服务任务发送启动命令，将定时器置为活跃状态。
 * 若定时器已在运行，效果等同于xTimerReset()。
 * 
 * @param xTimer 目标定时器句柄
 * @param xTicksToWait 命令队列满时的最大阻塞时间：
 *        - 0：非阻塞模式（立即返回）
 *        - portMAX_DELAY：无限等待
 *        - 其他值：指定时钟节拍数
 * @return BaseType_t 命令发送结果：
 *         - pdPASS：成功加入队列
 *         - pdFAIL：队列满且超时
 * 
 * @note 典型调用场景：
 * @code
 * if(xTimerStart(xSensorTimer, pdMS_TO_TICKS(100)) == pdFAIL) {
 *     // 错误处理：记录日志或重启定时器
 * }
 * @endcode
 * 
 * @warning 重要限制：
 * - 必须确保定时器已通过xTimerCreate()创建
 * - 在中断中必须使用xTimerStartFromISR()
 */
#define xTimerStart( xTimer, xTicksToWait ) \
    xTimerGenericCommand( ( xTimer ), tmrCOMMAND_START, ( xTaskGetTickCount() ), NULL, ( xTicksToWait ) )

/**
 * @brief 停止软件定时器（任务上下文）
 * 
 * 向定时器服务任务发送停止命令，将定时器置为休眠状态。
 * 停止后的定时器可通过xTimerStart()重新激活。
 * 
 * @param xTimer 目标定时器句柄
 * @param xTicksToWait 同xTimerStart参数
 * @return BaseType_t 同xTimerStart返回值
 * 
 * @note 状态转换示意图：
 * [运行中] --xTimerStop()--> [休眠]
 * [休眠]   --xTimerStart()--> [运行中]
 */
#define xTimerStop( xTimer, xTicksToWait ) \
    xTimerGenericCommand( ( xTimer ), tmrCOMMAND_STOP, 0U, NULL, ( xTicksToWait ) )

/**
 * BaseType_t xTimerChangePeriod( 	TimerHandle_t xTimer,
 *										TickType_t xNewPeriod,
 *										TickType_t xTicksToWait );
 *
 * Timer functionality is provided by a timer service/daemon task.  Many of the
 * public FreeRTOS timer API functions send commands to the timer service task
 * through a queue called the timer command queue.  The timer command queue is
 * private to the kernel itself and is not directly accessible to application
 * code.  The length of the timer command queue is set by the
 * configTIMER_QUEUE_LENGTH configuration constant.
 *
 * xTimerChangePeriod() changes the period of a timer that was previously
 * created using the xTimerCreate() API function.
 *
 * xTimerChangePeriod() can be called to change the period of an active or
 * dormant state timer.
 *
 * The configUSE_TIMERS configuration constant must be set to 1 for
 * xTimerChangePeriod() to be available.
 *
 * @param xTimer The handle of the timer that is having its period changed.
 *
 * @param xNewPeriod The new period for xTimer. Timer periods are specified in
 * tick periods, so the constant portTICK_PERIOD_MS can be used to convert a time
 * that has been specified in milliseconds.  For example, if the timer must
 * expire after 100 ticks, then xNewPeriod should be set to 100.  Alternatively,
 * if the timer must expire after 500ms, then xNewPeriod can be set to
 * ( 500 / portTICK_PERIOD_MS ) provided configTICK_RATE_HZ is less than
 * or equal to 1000.
 *
 * @param xTicksToWait Specifies the time, in ticks, that the calling task should
 * be held in the Blocked state to wait for the change period command to be
 * successfully sent to the timer command queue, should the queue already be
 * full when xTimerChangePeriod() was called.  xTicksToWait is ignored if
 * xTimerChangePeriod() is called before the scheduler is started.
 *
 * @return pdFAIL will be returned if the change period command could not be
 * sent to the timer command queue even after xTicksToWait ticks had passed.
 * pdPASS will be returned if the command was successfully sent to the timer
 * command queue.  When the command is actually processed will depend on the
 * priority of the timer service/daemon task relative to other tasks in the
 * system.  The timer service/daemon task priority is set by the
 * configTIMER_TASK_PRIORITY configuration constant.
 *
 * Example usage:
 * @verbatim
 * // This function assumes xTimer has already been created.  If the timer
 * // referenced by xTimer is already active when it is called, then the timer
 * // is deleted.  If the timer referenced by xTimer is not active when it is
 * // called, then the period of the timer is set to 500ms and the timer is
 * // started.
 * void vAFunction( TimerHandle_t xTimer )
 * {
 *     if( xTimerIsTimerActive( xTimer ) != pdFALSE ) // or more simply and equivalently "if( xTimerIsTimerActive( xTimer ) )"
 *     {
 *         // xTimer is already active - delete it.
 *         xTimerDelete( xTimer );
 *     }
 *     else
 *     {
 *         // xTimer is not active, change its period to 500ms.  This will also
 *         // cause the timer to start.  Block for a maximum of 100 ticks if the
 *         // change period command cannot immediately be sent to the timer
 *         // command queue.
 *         if( xTimerChangePeriod( xTimer, 500 / portTICK_PERIOD_MS, 100 ) == pdPASS )
 *         {
 *             // The command was successfully sent.
 *         }
 *         else
 *         {
 *             // The command could not be sent, even after waiting for 100 ticks
 *             // to pass.  Take appropriate action here.
 *         }
 *     }
 * }
 * @endverbatim
 */
 /**
 * @brief 修改定时器周期（任务上下文）
 * 
 * 动态调整定时器的触发间隔，可立即影响当前周期。
 * 若定时器已启动，将基于新周期重新计算到期时间。
 * 
 * @param xTimer 目标定时器句柄
 * @param xNewPeriod 新周期（时钟节拍数，必须＞0）
 * @param xTicksToWait 命令队列满时的最大阻塞时间
 * @return BaseType_t 命令发送结果（同xTimerStart）
 * 
 * @note 典型应用场景：
 * @code
 * // 根据负载动态调整采样频率
 * if(bSystemOverload) {
 *     xTimerChangePeriod(xSamplingTimer, pdMS_TO_TICKS(500), 0);
 * } else {
 *     xTimerChangePeriod(xSamplingTimer, pdMS_TO_TICKS(100), 0);
 * }
 * @endcode
 * 
 * @warning 重要限制：
 * - 修改已过期单次定时器会重新激活它
 * - 新周期立即生效，可能中断当前计时
 */
#define xTimerChangePeriod( xTimer, xNewPeriod, xTicksToWait ) \
    xTimerGenericCommand( ( xTimer ), tmrCOMMAND_CHANGE_PERIOD, ( xNewPeriod ), NULL, ( xTicksToWait ) )

/**
 * @brief 删除动态创建的定时器
 * 
 * 释放定时器占用的内存资源并移除所有队列引用，
 * 删除后的句柄将失效，不可再被操作。
 * 
 * @param xTimer 待删除定时器句柄
 * @param xTicksToWait 同xTimerStart参数
 * @return BaseType_t 同xTimerStart返回值
 * 
 * @warning 关键操作规范：
 * 1. 必须在定时器停止后调用（建议先xTimerStop）
 * 2. 删除过程中可能仍在处理回调，需同步机制
 * 3. 静态创建定时器调用此API将导致未定义行为
 * 
 * @code
 * // 安全删除流程
 * if(xTimerStop(xTimer, pdMS_TO_TICKS(100)) == pdPASS) {
 *     if(xTimerDelete(xTimer, pdMS_TO_TICKS(200)) == pdPASS) {
 *         xTimer = NULL; // 防止悬空指针
 *     }
 * }
 * @endcode
 */
#define xTimerDelete( xTimer, xTicksToWait ) \
    xTimerGenericCommand( ( xTimer ), tmrCOMMAND_DELETE, 0U, NULL, ( xTicksToWait ) )

/**
 * BaseType_t xTimerReset( TimerHandle_t xTimer, TickType_t xTicksToWait );
 *
 * Timer functionality is provided by a timer service/daemon task.  Many of the
 * public FreeRTOS timer API functions send commands to the timer service task
 * through a queue called the timer command queue.  The timer command queue is
 * private to the kernel itself and is not directly accessible to application
 * code.  The length of the timer command queue is set by the
 * configTIMER_QUEUE_LENGTH configuration constant.
 *
 * xTimerReset() re-starts a timer that was previously created using the
 * xTimerCreate() API function.  If the timer had already been started and was
 * already in the active state, then xTimerReset() will cause the timer to
 * re-evaluate its expiry time so that it is relative to when xTimerReset() was
 * called.  If the timer was in the dormant state then xTimerReset() has
 * equivalent functionality to the xTimerStart() API function.
 *
 * Resetting a timer ensures the timer is in the active state.  If the timer
 * is not stopped, deleted, or reset in the mean time, the callback function
 * associated with the timer will get called 'n' ticks after xTimerReset() was
 * called, where 'n' is the timers defined period.
 *
 * It is valid to call xTimerReset() before the scheduler has been started, but
 * when this is done the timer will not actually start until the scheduler is
 * started, and the timers expiry time will be relative to when the scheduler is
 * started, not relative to when xTimerReset() was called.
 *
 * The configUSE_TIMERS configuration constant must be set to 1 for xTimerReset()
 * to be available.
 *
 * @param xTimer The handle of the timer being reset/started/restarted.
 *
 * @param xTicksToWait Specifies the time, in ticks, that the calling task should
 * be held in the Blocked state to wait for the reset command to be successfully
 * sent to the timer command queue, should the queue already be full when
 * xTimerReset() was called.  xTicksToWait is ignored if xTimerReset() is called
 * before the scheduler is started.
 *
 * @return pdFAIL will be returned if the reset command could not be sent to
 * the timer command queue even after xTicksToWait ticks had passed.  pdPASS will
 * be returned if the command was successfully sent to the timer command queue.
 * When the command is actually processed will depend on the priority of the
 * timer service/daemon task relative to other tasks in the system, although the
 * timers expiry time is relative to when xTimerStart() is actually called.  The
 * timer service/daemon task priority is set by the configTIMER_TASK_PRIORITY
 * configuration constant.
 *
 * Example usage:
 * @verbatim
 * // When a key is pressed, an LCD back-light is switched on.  If 5 seconds pass
 * // without a key being pressed, then the LCD back-light is switched off.  In
 * // this case, the timer is a one-shot timer.
 *
 * TimerHandle_t xBacklightTimer = NULL;
 *
 * // The callback function assigned to the one-shot timer.  In this case the
 * // parameter is not used.
 * void vBacklightTimerCallback( TimerHandle_t pxTimer )
 * {
 *     // The timer expired, therefore 5 seconds must have passed since a key
 *     // was pressed.  Switch off the LCD back-light.
 *     vSetBacklightState( BACKLIGHT_OFF );
 * }
 *
 * // The key press event handler.
 * void vKeyPressEventHandler( char cKey )
 * {
 *     // Ensure the LCD back-light is on, then reset the timer that is
 *     // responsible for turning the back-light off after 5 seconds of
 *     // key inactivity.  Wait 10 ticks for the command to be successfully sent
 *     // if it cannot be sent immediately.
 *     vSetBacklightState( BACKLIGHT_ON );
 *     if( xTimerReset( xBacklightTimer, 100 ) != pdPASS )
 *     {
 *         // The reset command was not executed successfully.  Take appropriate
 *         // action here.
 *     }
 *
 *     // Perform the rest of the key processing here.
 * }
 *
 * void main( void )
 * {
 * int32_t x;
 *
 *     // Create then start the one-shot timer that is responsible for turning
 *     // the back-light off if no keys are pressed within a 5 second period.
 *     xBacklightTimer = xTimerCreate( "BacklightTimer",           // Just a text name, not used by the kernel.
 *                                     ( 5000 / portTICK_PERIOD_MS), // The timer period in ticks.
 *                                     pdFALSE,                    // The timer is a one-shot timer.
 *                                     0,                          // The id is not used by the callback so can take any value.
 *                                     vBacklightTimerCallback     // The callback function that switches the LCD back-light off.
 *                                   );
 *
 *     if( xBacklightTimer == NULL )
 *     {
 *         // The timer was not created.
 *     }
 *     else
 *     {
 *         // Start the timer.  No block time is specified, and even if one was
 *         // it would be ignored because the scheduler has not yet been
 *         // started.
 *         if( xTimerStart( xBacklightTimer, 0 ) != pdPASS )
 *         {
 *             // The timer could not be set into the Active state.
 *         }
 *     }
 *
 *     // ...
 *     // Create tasks here.
 *     // ...
 *
 *     // Starting the scheduler will start the timer running as it has already
 *     // been set into the active state.
 *     vTaskStartScheduler();
 *
 *     // Should not reach here.
 *     for( ;; );
 * }
 * @endverbatim
 */
/**
 * @brief 重置并启动定时器（任务上下文）
 * 
 * 将定时器的计数器重置为当前时刻，基于原周期重新开始计时。
 * 若定时器未运行，效果等同于xTimerStart()。
 * 
 * @param xTimer 目标定时器句柄
 * @param xTicksToWait 命令队列满时的最大阻塞时间
 * @return BaseType_t 命令发送结果：
 *         - pdPASS：成功加入队列
 *         - pdFAIL：队列满且超时
 * 
 * @note 典型应用场景：
 * @code
 * // 在通信应答后重置超时检测
 * void vOnDataReceived() {
 *     if(xTimerReset(xTimeoutTimer, pdMS_TO_TICKS(50)) != pdPASS) {
 *         vHandleCriticalError(); // 处理队列满错误
 *     }
 * }
 * @endcode
 * 
 * @warning 重要特性：
 * 1. 对单次定时器使用会延长其触发时间，但不会改为周期模式
 * 2. 重置操作不影响定时器的自动重载配置
 * 3. 实际重置时间基准为命令入队时刻，非执行时刻
 */
#define xTimerReset( xTimer, xTicksToWait ) \
    xTimerGenericCommand( ( xTimer ), tmrCOMMAND_RESET, ( xTaskGetTickCount() ), NULL, ( xTicksToWait ) )

/**
 * BaseType_t xTimerStartFromISR( 	TimerHandle_t xTimer,
 *									BaseType_t *pxHigherPriorityTaskWoken );
 *
 * A version of xTimerStart() that can be called from an interrupt service
 * routine.
 *
 * @param xTimer The handle of the timer being started/restarted.
 *
 * @param pxHigherPriorityTaskWoken The timer service/daemon task spends most
 * of its time in the Blocked state, waiting for messages to arrive on the timer
 * command queue.  Calling xTimerStartFromISR() writes a message to the timer
 * command queue, so has the potential to transition the timer service/daemon
 * task out of the Blocked state.  If calling xTimerStartFromISR() causes the
 * timer service/daemon task to leave the Blocked state, and the timer service/
 * daemon task has a priority equal to or greater than the currently executing
 * task (the task that was interrupted), then *pxHigherPriorityTaskWoken will
 * get set to pdTRUE internally within the xTimerStartFromISR() function.  If
 * xTimerStartFromISR() sets this value to pdTRUE then a context switch should
 * be performed before the interrupt exits.
 *
 * @return pdFAIL will be returned if the start command could not be sent to
 * the timer command queue.  pdPASS will be returned if the command was
 * successfully sent to the timer command queue.  When the command is actually
 * processed will depend on the priority of the timer service/daemon task
 * relative to other tasks in the system, although the timers expiry time is
 * relative to when xTimerStartFromISR() is actually called.  The timer
 * service/daemon task priority is set by the configTIMER_TASK_PRIORITY
 * configuration constant.
 *
 * Example usage:
 * @verbatim
 * // This scenario assumes xBacklightTimer has already been created.  When a
 * // key is pressed, an LCD back-light is switched on.  If 5 seconds pass
 * // without a key being pressed, then the LCD back-light is switched off.  In
 * // this case, the timer is a one-shot timer, and unlike the example given for
 * // the xTimerReset() function, the key press event handler is an interrupt
 * // service routine.
 *
 * // The callback function assigned to the one-shot timer.  In this case the
 * // parameter is not used.
 * void vBacklightTimerCallback( TimerHandle_t pxTimer )
 * {
 *     // The timer expired, therefore 5 seconds must have passed since a key
 *     // was pressed.  Switch off the LCD back-light.
 *     vSetBacklightState( BACKLIGHT_OFF );
 * }
 *
 * // The key press interrupt service routine.
 * void vKeyPressEventInterruptHandler( void )
 * {
 * BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *
 *     // Ensure the LCD back-light is on, then restart the timer that is
 *     // responsible for turning the back-light off after 5 seconds of
 *     // key inactivity.  This is an interrupt service routine so can only
 *     // call FreeRTOS API functions that end in "FromISR".
 *     vSetBacklightState( BACKLIGHT_ON );
 *
 *     // xTimerStartFromISR() or xTimerResetFromISR() could be called here
 *     // as both cause the timer to re-calculate its expiry time.
 *     // xHigherPriorityTaskWoken was initialised to pdFALSE when it was
 *     // declared (in this function).
 *     if( xTimerStartFromISR( xBacklightTimer, &xHigherPriorityTaskWoken ) != pdPASS )
 *     {
 *         // The start command was not executed successfully.  Take appropriate
 *         // action here.
 *     }
 *
 *     // Perform the rest of the key processing here.
 *
 *     // If xHigherPriorityTaskWoken equals pdTRUE, then a context switch
 *     // should be performed.  The syntax required to perform a context switch
 *     // from inside an ISR varies from port to port, and from compiler to
 *     // compiler.  Inspect the demos for the port you are using to find the
 *     // actual syntax required.
 *     if( xHigherPriorityTaskWoken != pdFALSE )
 *     {
 *         // Call the interrupt safe yield function here (actual function
 *         // depends on the FreeRTOS port being used).
 *     }
 * }
 * @endverbatim
 */
/**
 * @brief 启动/重置定时器（中断安全版本）
 * 
 * 该宏用于在中断服务例程中启动或重置定时器，
 * 是xTimerStart()的中断安全版本，具有非阻塞特性。
 * 
 * @param xTimer 目标定时器句柄
 * @param pxHigherPriorityTaskWoken 上下文切换标记指针：
 *        - 输出参数，可能被设为pdTRUE
 *        - 退出ISR前需调用portYIELD_FROM_ISR()
 * @return BaseType_t 操作结果：
 *         - pdPASS：命令成功入队
 *         - pdFAIL：定时器队列已满
 * 
 * @note 典型中断使用场景：
 * @code
 * void vUART_ISR(void) {
 *     BaseType_t xHPW = pdFALSE;
 *     
 *     // 收到起始字节后启动超时检测
 *     if(xTimerStartFromISR(xTimeoutTimer, &xHPW) == pdPASS) {
 *         portYIELD_FROM_ISR(xHPW);
 *     } else {
 *         // 处理队列满错误
 *         xUARTErrorFlags |= TIMER_QUEUE_FULL;
 *     }
 * }
 * @endcode
 * 
 * @warning 关键限制：
 * 1. 必须在中断上下文调用（带FromISR后缀）
 * 2. 不能用于静态创建的定时器停止操作
 * 3. 实际启动时间取决于守护任务调度延迟
 */
#define xTimerStartFromISR( xTimer, pxHigherPriorityTaskWoken ) \
    xTimerGenericCommand( ( xTimer ), tmrCOMMAND_START_FROM_ISR, ( xTaskGetTickCountFromISR() ), ( pxHigherPriorityTaskWoken ), 0U )

/**
 * BaseType_t xTimerStopFromISR( 	TimerHandle_t xTimer,
 *									BaseType_t *pxHigherPriorityTaskWoken );
 *
 * A version of xTimerStop() that can be called from an interrupt service
 * routine.
 *
 * @param xTimer The handle of the timer being stopped.
 *
 * @param pxHigherPriorityTaskWoken The timer service/daemon task spends most
 * of its time in the Blocked state, waiting for messages to arrive on the timer
 * command queue.  Calling xTimerStopFromISR() writes a message to the timer
 * command queue, so has the potential to transition the timer service/daemon
 * task out of the Blocked state.  If calling xTimerStopFromISR() causes the
 * timer service/daemon task to leave the Blocked state, and the timer service/
 * daemon task has a priority equal to or greater than the currently executing
 * task (the task that was interrupted), then *pxHigherPriorityTaskWoken will
 * get set to pdTRUE internally within the xTimerStopFromISR() function.  If
 * xTimerStopFromISR() sets this value to pdTRUE then a context switch should
 * be performed before the interrupt exits.
 *
 * @return pdFAIL will be returned if the stop command could not be sent to
 * the timer command queue.  pdPASS will be returned if the command was
 * successfully sent to the timer command queue.  When the command is actually
 * processed will depend on the priority of the timer service/daemon task
 * relative to other tasks in the system.  The timer service/daemon task
 * priority is set by the configTIMER_TASK_PRIORITY configuration constant.
 *
 * Example usage:
 * @verbatim
 * // This scenario assumes xTimer has already been created and started.  When
 * // an interrupt occurs, the timer should be simply stopped.
 *
 * // The interrupt service routine that stops the timer.
 * void vAnExampleInterruptServiceRoutine( void )
 * {
 * BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *
 *     // The interrupt has occurred - simply stop the timer.
 *     // xHigherPriorityTaskWoken was set to pdFALSE where it was defined
 *     // (within this function).  As this is an interrupt service routine, only
 *     // FreeRTOS API functions that end in "FromISR" can be used.
 *     if( xTimerStopFromISR( xTimer, &xHigherPriorityTaskWoken ) != pdPASS )
 *     {
 *         // The stop command was not executed successfully.  Take appropriate
 *         // action here.
 *     }
 *
 *     // If xHigherPriorityTaskWoken equals pdTRUE, then a context switch
 *     // should be performed.  The syntax required to perform a context switch
 *     // from inside an ISR varies from port to port, and from compiler to
 *     // compiler.  Inspect the demos for the port you are using to find the
 *     // actual syntax required.
 *     if( xHigherPriorityTaskWoken != pdFALSE )
 *     {
 *         // Call the interrupt safe yield function here (actual function
 *         // depends on the FreeRTOS port being used).
 *     }
 * }
 * @endverbatim
 */
/**
 * @brief 停止定时器（中断安全版本）
 * 
 * 该宏用于在中断服务例程中安全停止定时器，
 * 是xTimerStop()的中断安全版本，具有非阻塞特性。
 * 
 * @param xTimer 目标定时器句柄
 * @param pxHigherPriorityTaskWoken 上下文切换标记指针：
 *        - 输出参数，可能被设为pdTRUE
 *        - 退出ISR前需调用portYIELD_FROM_ISR()
 * @return BaseType_t 操作结果：
 *         - pdPASS：命令成功入队
 *         - pdFAIL：定时器队列已满
 * 
 * @note 典型中断使用场景：
 * @code
 * void vMotionSensorISR(void) {
 *     BaseType_t xHPW = pdFALSE;
 *     
 *     // 检测到运动后停止休眠定时器
 *     if(xTimerStopFromISR(xSleepTimer, &xHPW) == pdPASS) {
 *         portYIELD_FROM_ISR(xHPW); // 及时切换上下文
 *     } else {
 *         xErrorFlags |= TIMER_STOP_FAILED;
 *     }
 * }
 * @endcode
 * 
 * @warning 关键限制：
 * 1. 停止操作异步执行，实际生效时间取决于守护任务调度
 * 2. 多次停止同一定时器无副作用
 * 3. 停止静态创建的定时器需确保内存有效性
 */
#define xTimerStopFromISR( xTimer, pxHigherPriorityTaskWoken ) \
    xTimerGenericCommand( ( xTimer ), tmrCOMMAND_STOP_FROM_ISR, 0, ( pxHigherPriorityTaskWoken ), 0U )

/**
 * BaseType_t xTimerChangePeriodFromISR( TimerHandle_t xTimer,
 *										 TickType_t xNewPeriod,
 *										 BaseType_t *pxHigherPriorityTaskWoken );
 *
 * A version of xTimerChangePeriod() that can be called from an interrupt
 * service routine.
 *
 * @param xTimer The handle of the timer that is having its period changed.
 *
 * @param xNewPeriod The new period for xTimer. Timer periods are specified in
 * tick periods, so the constant portTICK_PERIOD_MS can be used to convert a time
 * that has been specified in milliseconds.  For example, if the timer must
 * expire after 100 ticks, then xNewPeriod should be set to 100.  Alternatively,
 * if the timer must expire after 500ms, then xNewPeriod can be set to
 * ( 500 / portTICK_PERIOD_MS ) provided configTICK_RATE_HZ is less than
 * or equal to 1000.
 *
 * @param pxHigherPriorityTaskWoken The timer service/daemon task spends most
 * of its time in the Blocked state, waiting for messages to arrive on the timer
 * command queue.  Calling xTimerChangePeriodFromISR() writes a message to the
 * timer command queue, so has the potential to transition the timer service/
 * daemon task out of the Blocked state.  If calling xTimerChangePeriodFromISR()
 * causes the timer service/daemon task to leave the Blocked state, and the
 * timer service/daemon task has a priority equal to or greater than the
 * currently executing task (the task that was interrupted), then
 * *pxHigherPriorityTaskWoken will get set to pdTRUE internally within the
 * xTimerChangePeriodFromISR() function.  If xTimerChangePeriodFromISR() sets
 * this value to pdTRUE then a context switch should be performed before the
 * interrupt exits.
 *
 * @return pdFAIL will be returned if the command to change the timers period
 * could not be sent to the timer command queue.  pdPASS will be returned if the
 * command was successfully sent to the timer command queue.  When the command
 * is actually processed will depend on the priority of the timer service/daemon
 * task relative to other tasks in the system.  The timer service/daemon task
 * priority is set by the configTIMER_TASK_PRIORITY configuration constant.
 *
 * Example usage:
 * @verbatim
 * // This scenario assumes xTimer has already been created and started.  When
 * // an interrupt occurs, the period of xTimer should be changed to 500ms.
 *
 * // The interrupt service routine that changes the period of xTimer.
 * void vAnExampleInterruptServiceRoutine( void )
 * {
 * BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *
 *     // The interrupt has occurred - change the period of xTimer to 500ms.
 *     // xHigherPriorityTaskWoken was set to pdFALSE where it was defined
 *     // (within this function).  As this is an interrupt service routine, only
 *     // FreeRTOS API functions that end in "FromISR" can be used.
 *     if( xTimerChangePeriodFromISR( xTimer, &xHigherPriorityTaskWoken ) != pdPASS )
 *     {
 *         // The command to change the timers period was not executed
 *         // successfully.  Take appropriate action here.
 *     }
 *
 *     // If xHigherPriorityTaskWoken equals pdTRUE, then a context switch
 *     // should be performed.  The syntax required to perform a context switch
 *     // from inside an ISR varies from port to port, and from compiler to
 *     // compiler.  Inspect the demos for the port you are using to find the
 *     // actual syntax required.
 *     if( xHigherPriorityTaskWoken != pdFALSE )
 *     {
 *         // Call the interrupt safe yield function here (actual function
 *         // depends on the FreeRTOS port being used).
 *     }
 * }
 * @endverbatim
 */
/**
 * @brief 修改定时器周期（中断安全版本）
 * 
 * 该宏用于在中断服务例程中动态调整定时器周期，
 * 是xTimerChangePeriod()的中断安全实现。
 * 
 * @param xTimer 目标定时器句柄
 * @param xNewPeriod 新周期（时钟节拍数，必须＞0）
 * @param pxHigherPriorityTaskWoken 上下文切换标记指针
 * @return BaseType_t 操作结果：
 *         - pdPASS：命令成功入队
 *         - pdFAIL：队列已满或无效参数
 * 
 * @note 实时调整示例：
 * @code
 * void vRPM_ISR() {
 *     static TickType_t xNewPeriod = pdMS_TO_TICKS(100);
 *     BaseType_t xHPW = pdFALSE;
 *     
 *     // 根据转速动态调整采样周期
 *     xNewPeriod = calculate_new_period(...);
 *     if(xTimerChangePeriodFromISR(xRpmSampler, xNewPeriod, &xHPW)) {
 *         portYIELD_FROM_ISR(xHPW);
 *     } else {
 *         xSystemErrors |= PERIOD_CHANGE_FAIL;
 *     }
 * }
 * @endcode
 * 
 * @warning 关键特性：
 * 1. 新周期在下个守护任务周期处理中生效
 * 2. 修改运行中定时器会重新计算剩余时间
 * 3. 单次定时器周期修改后仍保持单次触发特性
 */
#define xTimerChangePeriodFromISR( xTimer, xNewPeriod, pxHigherPriorityTaskWoken ) \
    xTimerGenericCommand( ( xTimer ), tmrCOMMAND_CHANGE_PERIOD_FROM_ISR, ( xNewPeriod ), ( pxHigherPriorityTaskWoken ), 0U )

/**
 * BaseType_t xTimerResetFromISR( 	TimerHandle_t xTimer,
 *									BaseType_t *pxHigherPriorityTaskWoken );
 *
 * A version of xTimerReset() that can be called from an interrupt service
 * routine.
 *
 * @param xTimer The handle of the timer that is to be started, reset, or
 * restarted.
 *
 * @param pxHigherPriorityTaskWoken The timer service/daemon task spends most
 * of its time in the Blocked state, waiting for messages to arrive on the timer
 * command queue.  Calling xTimerResetFromISR() writes a message to the timer
 * command queue, so has the potential to transition the timer service/daemon
 * task out of the Blocked state.  If calling xTimerResetFromISR() causes the
 * timer service/daemon task to leave the Blocked state, and the timer service/
 * daemon task has a priority equal to or greater than the currently executing
 * task (the task that was interrupted), then *pxHigherPriorityTaskWoken will
 * get set to pdTRUE internally within the xTimerResetFromISR() function.  If
 * xTimerResetFromISR() sets this value to pdTRUE then a context switch should
 * be performed before the interrupt exits.
 *
 * @return pdFAIL will be returned if the reset command could not be sent to
 * the timer command queue.  pdPASS will be returned if the command was
 * successfully sent to the timer command queue.  When the command is actually
 * processed will depend on the priority of the timer service/daemon task
 * relative to other tasks in the system, although the timers expiry time is
 * relative to when xTimerResetFromISR() is actually called.  The timer service/daemon
 * task priority is set by the configTIMER_TASK_PRIORITY configuration constant.
 *
 * Example usage:
 * @verbatim
 * // This scenario assumes xBacklightTimer has already been created.  When a
 * // key is pressed, an LCD back-light is switched on.  If 5 seconds pass
 * // without a key being pressed, then the LCD back-light is switched off.  In
 * // this case, the timer is a one-shot timer, and unlike the example given for
 * // the xTimerReset() function, the key press event handler is an interrupt
 * // service routine.
 *
 * // The callback function assigned to the one-shot timer.  In this case the
 * // parameter is not used.
 * void vBacklightTimerCallback( TimerHandle_t pxTimer )
 * {
 *     // The timer expired, therefore 5 seconds must have passed since a key
 *     // was pressed.  Switch off the LCD back-light.
 *     vSetBacklightState( BACKLIGHT_OFF );
 * }
 *
 * // The key press interrupt service routine.
 * void vKeyPressEventInterruptHandler( void )
 * {
 * BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *
 *     // Ensure the LCD back-light is on, then reset the timer that is
 *     // responsible for turning the back-light off after 5 seconds of
 *     // key inactivity.  This is an interrupt service routine so can only
 *     // call FreeRTOS API functions that end in "FromISR".
 *     vSetBacklightState( BACKLIGHT_ON );
 *
 *     // xTimerStartFromISR() or xTimerResetFromISR() could be called here
 *     // as both cause the timer to re-calculate its expiry time.
 *     // xHigherPriorityTaskWoken was initialised to pdFALSE when it was
 *     // declared (in this function).
 *     if( xTimerResetFromISR( xBacklightTimer, &xHigherPriorityTaskWoken ) != pdPASS )
 *     {
 *         // The reset command was not executed successfully.  Take appropriate
 *         // action here.
 *     }
 *
 *     // Perform the rest of the key processing here.
 *
 *     // If xHigherPriorityTaskWoken equals pdTRUE, then a context switch
 *     // should be performed.  The syntax required to perform a context switch
 *     // from inside an ISR varies from port to port, and from compiler to
 *     // compiler.  Inspect the demos for the port you are using to find the
 *     // actual syntax required.
 *     if( xHigherPriorityTaskWoken != pdFALSE )
 *     {
 *         // Call the interrupt safe yield function here (actual function
 *         // depends on the FreeRTOS port being used).
 *     }
 * }
 * @endverbatim
 */
/**
 * @brief 重置定时器（中断安全版本）
 * 
 * 该宏用于在中断服务例程中重置定时器计数器，
 * 是xTimerReset()的中断安全实现，具有非阻塞特性。
 * 
 * @param xTimer 目标定时器句柄
 * @param pxHigherPriorityTaskWoken 上下文切换标记指针：
 *        - 输出参数，可能被设为pdTRUE
 *        - 退出ISR前需调用portYIELD_FROM_ISR()
 * @return BaseType_t 操作结果：
 *         - pdPASS：命令成功入队
 *         - pdFAIL：队列已满或无效参数
 * 
 * @note 典型应用场景：
 * @code
 * void vCanBus_ISR() {
 *     BaseType_t xHPW = pdFALSE;
 *     
 *     // 收到有效报文后重置超时检测
 *     if(xTimerResetFromISR(xCanTimeoutTimer, &xHPW) == pdPASS) {
 *         portYIELD_FROM_ISR(xHPW);
 *     } else {
 *         xCanStatus |= CAN_TIMER_RESET_FAIL;
 *     }
 * }
 * @endcode
 * 
 * @warning 关键特性：
 * 1. 对停止的定时器执行将启动定时器
 * 2. 实际重置时间基准为命令入队时刻
 * 3. 多次重置不会累积效果
 */
#define xTimerResetFromISR( xTimer, pxHigherPriorityTaskWoken ) \
    xTimerGenericCommand( ( xTimer ), tmrCOMMAND_RESET_FROM_ISR, ( xTaskGetTickCountFromISR() ), ( pxHigherPriorityTaskWoken ), 0U )


/**
 * BaseType_t xTimerPendFunctionCallFromISR( PendedFunction_t xFunctionToPend,
 *                                          void *pvParameter1,
 *                                          uint32_t ulParameter2,
 *                                          BaseType_t *pxHigherPriorityTaskWoken );
 *
 *
 * Used from application interrupt service routines to defer the execution of a
 * function to the RTOS daemon task (the timer service task, hence this function
 * is implemented in timers.c and is prefixed with 'Timer').
 *
 * Ideally an interrupt service routine (ISR) is kept as short as possible, but
 * sometimes an ISR either has a lot of processing to do, or needs to perform
 * processing that is not deterministic.  In these cases
 * xTimerPendFunctionCallFromISR() can be used to defer processing of a function
 * to the RTOS daemon task.
 *
 * A mechanism is provided that allows the interrupt to return directly to the
 * task that will subsequently execute the pended callback function.  This
 * allows the callback function to execute contiguously in time with the
 * interrupt - just as if the callback had executed in the interrupt itself.
 *
 * @param xFunctionToPend The function to execute from the timer service/
 * daemon task.  The function must conform to the PendedFunction_t
 * prototype.
 *
 * @param pvParameter1 The value of the callback function's first parameter.
 * The parameter has a void * type to allow it to be used to pass any type.
 * For example, unsigned longs can be cast to a void *, or the void * can be
 * used to point to a structure.
 *
 * @param ulParameter2 The value of the callback function's second parameter.
 *
 * @param pxHigherPriorityTaskWoken As mentioned above, calling this function
 * will result in a message being sent to the timer daemon task.  If the
 * priority of the timer daemon task (which is set using
 * configTIMER_TASK_PRIORITY in FreeRTOSConfig.h) is higher than the priority of
 * the currently running task (the task the interrupt interrupted) then
 * *pxHigherPriorityTaskWoken will be set to pdTRUE within
 * xTimerPendFunctionCallFromISR(), indicating that a context switch should be
 * requested before the interrupt exits.  For that reason
 * *pxHigherPriorityTaskWoken must be initialised to pdFALSE.  See the
 * example code below.
 *
 * @return pdPASS is returned if the message was successfully sent to the
 * timer daemon task, otherwise pdFALSE is returned.
 *
 * Example usage:
 * @verbatim
 *
 *	// The callback function that will execute in the context of the daemon task.
 *  // Note callback functions must all use this same prototype.
 *  void vProcessInterface( void *pvParameter1, uint32_t ulParameter2 )
 *	{
 *		BaseType_t xInterfaceToService;
 *
 *		// The interface that requires servicing is passed in the second
 *      // parameter.  The first parameter is not used in this case.
 *		xInterfaceToService = ( BaseType_t ) ulParameter2;
 *
 *		// ...Perform the processing here...
 *	}
 *
 *	// An ISR that receives data packets from multiple interfaces
 *  void vAnISR( void )
 *	{
 *		BaseType_t xInterfaceToService, xHigherPriorityTaskWoken;
 *
 *		// Query the hardware to determine which interface needs processing.
 *		xInterfaceToService = prvCheckInterfaces();
 *
 *      // The actual processing is to be deferred to a task.  Request the
 *      // vProcessInterface() callback function is executed, passing in the
 *		// number of the interface that needs processing.  The interface to
 *		// service is passed in the second parameter.  The first parameter is
 *		// not used in this case.
 *		xHigherPriorityTaskWoken = pdFALSE;
 *		xTimerPendFunctionCallFromISR( vProcessInterface, NULL, ( uint32_t ) xInterfaceToService, &xHigherPriorityTaskWoken );
 *
 *		// If xHigherPriorityTaskWoken is now set to pdTRUE then a context
 *		// switch should be requested.  The macro used is port specific and will
 *		// be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
 *		// the documentation page for the port being used.
 *		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
 *
 *	}
 * @endverbatim
 */
/**
 * @brief 延迟函数执行到守护任务（中断安全版本）
 * 
 * 该函数用于在中断上下文中将函数调用提交到RTOS守护任务队列，
 * 适用于需要延后处理复杂逻辑或访问任务安全资源的场景。
 * 
 * @param xFunctionToPend 待执行函数（符合PendedFunction_t类型）
 * @param pvParameter1 第一个参数（指针类型，可传递任意数据结构）
 * @param ulParameter2 第二个参数（32位整型，用于传递标志位或数值）
 * @param pxHigherPriorityTaskWoken 上下文切换标记指针
 * @return BaseType_t 提交结果：
 *         - pdPASS：成功入队
 *         - pdFAIL：队列已满
 * 
 * @note 典型中断使用场景：
 * @code
 * void vSensorISR() {
 *     SensorData_t *pxData = pvPortMalloc(sizeof(SensorData_t));
 *     BaseType_t xHPW = pdFALSE;
 *     
 *     // 采集传感器数据
 *     pxData->value = read_sensor();
 *     
 *     // 提交数据处理到守护任务
 *     if(xTimerPendFunctionCallFromISR(vProcessSensorData, pxData, 0, &xHPW)) {
 *         portYIELD_FROM_ISR(xHPW);
 *     } else {
 *         vPortFree(pxData); // 立即释放内存
 *     }
 * }
 * @endcode
 */
BaseType_t xTimerPendFunctionCallFromISR( PendedFunction_t xFunctionToPend,
                                         void *pvParameter1,
                                         uint32_t ulParameter2,
                                         BaseType_t *pxHigherPriorityTaskWoken ) PRIVILEGED_FUNCTION;

/**
 * @brief 延迟函数执行到守护任务（任务上下文版本）
 * 
 * 该函数允许在任务上下文中将函数调用提交到守护任务队列，
 * 用于需要与定时器服务任务同步执行的场景。
 * 
 * @param xFunctionToPend 待执行函数
 * @param pvParameter1 第一个参数（指针类型）
 * @param ulParameter2 第二个参数（整型）
 * @param xTicksToWait 队列满时最大等待时间
 * @return BaseType_t 提交结果
 * 
 * @warning 内存管理规范：
 * - 若参数指向动态内存，需在回调函数中释放
 * - 禁止传递栈变量指针（除非能保证生命周期）
 * 
 * @code
 * void vNetworkPacketHandler(void *pvBuf, uint32_t ulSize) {
 *     PacketWrapper_t *pxWrapper = pvPortMalloc(sizeof(PacketWrapper_t));
 *     pxWrapper->pData = pvBuf;
 *     pxWrapper->ulSize = ulSize;
 *     
 *     if(xTimerPendFunctionCall(vDecodePacket, pxWrapper, 0, pdMS_TO_TICKS(100)) != pdPASS) {
 *         vPortFree(pxWrapper); // 提交失败立即清理
 *     }
 * }
 * @endcode
 */
BaseType_t xTimerPendFunctionCall( PendedFunction_t xFunctionToPend,
                                  void *pvParameter1,
                                  uint32_t ulParameter2,
                                  TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;
/**
 * @brief 获取定时器名称
 * 
 * 返回创建定时器时指定的名称字符串，用于调试和状态追踪。
 * 
 * @param xTimer 目标定时器句柄
 * @return const char* 定时器名称（与xTimerCreate的pcTimerName一致）
 * 
 * @warning 返回的指针在定时器删除后失效
 * 
 * @code
 * // 定时器状态监控示例
 * void vMonitorTimers() {
 *     const char *pcName = pcTimerGetName(xWatchdogTimer);
 *     printf("[%s] Period:%d Expiry:%d", 
 *            pcName, 
 *            xTimerGetPeriod(xWatchdogTimer),
 *            xTimerGetExpiryTime(xWatchdogTimer));
 * }
 * @endcode
 */
const char * pcTimerGetName( TimerHandle_t xTimer ) PRIVILEGED_FUNCTION;

/**
 * @brief 获取定时器周期
 * 
 * 返回定时器的当前周期配置值，单位为系统节拍数。
 * 
 * @param xTimer 目标定时器句柄
 * @return TickType_t 当前周期值（不会随xTimerChangePeriod动态更新）
 */
TickType_t xTimerGetPeriod( TimerHandle_t xTimer ) PRIVILEGED_FUNCTION;

/**
 * @brief 获取定时器到期时间
 * 
 * 返回定时器下一次触发的绝对节拍数。
 * 需结合xTaskGetTickCount()计算剩余时间。
 * 
 * @param xTimer 目标定时器句柄
 * @return TickType_t 到期绝对节拍数：
 *         - 若返回值 < 当前节拍数，表示计时器溢出
 * 
 * @note 剩余时间计算：
 * @code
 * TickType_t xRemaining = xTimerGetExpiryTime(xTimer) - xTaskGetTickCount();
 * @endcode
 */
TickType_t xTimerGetExpiryTime( TimerHandle_t xTimer ) PRIVILEGED_FUNCTION;

/*--------------------- 内核内部API（禁止应用层调用） ---------------------*/
/**
 * @brief 创建定时器守护任务（内核私有）
 * 
 * 由调度器启动时自动调用，初始化定时器服务任务。
 * 
 * @return BaseType_t 创建结果：
 *         - pdPASS：任务创建成功
 *         - pdFAIL：内存不足或任务已存在
 */
BaseType_t xTimerCreateTimerTask( void ) PRIVILEGED_FUNCTION;

/**
 * @brief 定时器通用命令处理（内核私有）
 * 
 * 所有定时器API的底层实现，处理命令队列的发送逻辑。
 * 
 * @param xTimer 目标定时器句柄
 * @param xCommandID 命令标识（tmrCOMMAND_*系列宏）
 * @param xOptionalValue 命令相关参数
 * @param pxHigherPriorityTaskWoken 上下文切换标记
 * @param xTicksToWait 队列满时等待时间
 * @return BaseType_t 命令发送结果
 */
BaseType_t xTimerGenericCommand( TimerHandle_t xTimer,
                                const BaseType_t xCommandID,
                                const TickType_t xOptionalValue,
                                BaseType_t * const pxHigherPriorityTaskWoken,
                                const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

#ifdef __cplusplus
}
#endif
#endif /* TIMERS_H */




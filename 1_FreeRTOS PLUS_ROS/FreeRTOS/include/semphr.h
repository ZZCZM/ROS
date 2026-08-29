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

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h" must appear in source files before "include semphr.h"
#endif

#include "queue.h"
/**
 * @file semphr.h
 * @brief FreeRTOS 信号量类型和基础宏定义
 * 
 * 该头文件定义了信号量句柄类型及信号量操作相关的基础宏：
 * - 二进制信号量队列参数
 * - 信号量操作超时时间设置
 * - 信号量队列项长度定义
 * 这些定义为信号量操作提供基础配置参数，用于FreeRTOS信号量机制的底层实现
 */

#ifndef INC_FREERTOS_H          // 条件编译检查：确保FreeRTOS.h已被包含
    #error "include FreeRTOS.h" must appear in source files before "include semphr.h"
#endif

#include "queue.h"              // 包含队列操作头文件（信号量基于队列实现）

// 将队列句柄类型重定义为信号量句柄类型
// 说明FreeRTOS中信号量是基于队列机制实现的
typedef QueueHandle_t SemaphoreHandle_t;

// 二进制信号量队列长度定义
// 固定为1，因为二进制信号量只需要保存一个状态（可用/不可用）
// (uint8_t)类型转换确保使用最小内存空间
#define semBINARY_SEMAPHORE_QUEUE_LENGTH    ( ( uint8_t ) 1U )

// 信号量队列项长度定义
// 设置为0表示队列项不需要存储实际数据（仅用于状态同步）
// 节省内存空间，符合信号量轻量级特性
#define semSEMAPHORE_QUEUE_ITEM_LENGTH      ( ( uint8_t ) 0U )

// 信号量释放操作的阻塞时间定义
// 设置为0 ticks表示非阻塞操作（立即返回）
// 用于xSemaphoreGive()等函数中，确保信号量操作的高效性
#define semGIVE_BLOCK_TIME                  ( ( TickType_t ) 0U )



/**
 * semphr. h
 * <pre>vSemaphoreCreateBinary( SemaphoreHandle_t xSemaphore )</pre>
 *
 * In many usage scenarios it is faster and more memory efficient to use a
 * direct to task notification in place of a binary semaphore!
 * http://www.freertos.org/RTOS-task-notifications.html
 *
 * This old vSemaphoreCreateBinary() macro is now deprecated in favour of the
 * xSemaphoreCreateBinary() function.  Note that binary semaphores created using
 * the vSemaphoreCreateBinary() macro are created in a state such that the
 * first call to 'take' the semaphore would pass, whereas binary semaphores
 * created using xSemaphoreCreateBinary() are created in a state such that the
 * the semaphore must first be 'given' before it can be 'taken'.
 *
 * <i>Macro</i> that implements a semaphore by using the existing queue mechanism.
 * The queue length is 1 as this is a binary semaphore.  The data size is 0
 * as we don't want to actually store any data - we just want to know if the
 * queue is empty or full.
 *
 * This type of semaphore can be used for pure synchronisation between tasks or
 * between an interrupt and a task.  The semaphore need not be given back once
 * obtained, so one task/interrupt can continuously 'give' the semaphore while
 * another continuously 'takes' the semaphore.  For this reason this type of
 * semaphore does not use a priority inheritance mechanism.  For an alternative
 * that does use priority inheritance see xSemaphoreCreateMutex().
 *
 * @param xSemaphore Handle to the created semaphore.  Should be of type SemaphoreHandle_t.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore = NULL;

 void vATask( void * pvParameters )
 {
    // Semaphore cannot be used before a call to vSemaphoreCreateBinary ().
    // This is a macro so pass the variable in directly.
    vSemaphoreCreateBinary( xSemaphore );

    if( xSemaphore != NULL )
    {
        // The semaphore was created successfully.
        // The semaphore can now be used.
    }
 }
 </pre>
 * \defgroup vSemaphoreCreateBinary vSemaphoreCreateBinary
 * \ingroup Semaphores
 */
/**
 * @def vSemaphoreCreateBinary
 * @brief 动态创建二进制信号量宏
 * @ingroup Semaphores
 * 
 * @param xSemaphore 输出参数，用于接收创建的信号量句柄
 * 
 * 该宏实现以下功能：
 * 1. 通过队列机制创建二进制信号量（队列长度为1，队列项长度为0）
 * 2. 自动初始化信号量为可用状态（创建后立即执行Give操作）
 * 3. 仅在启用动态内存分配时有效（依赖configSUPPORT_DYNAMIC_ALLOCATION配置）
 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )  // 动态内存分配使能检查
    #define vSemaphoreCreateBinary( xSemaphore )                                                                \
        {                                                                                                       \
            /* 调用队列创建函数生成二进制信号量 */                                                                \
            /* 参数1：(UBaseType_t)1 强制类型转换确保队列长度正确 */                                              \
            /* 参数2：使用预定义的空队列项长度（semSEMAPHORE_QUEUE_ITEM_LENGTH=0）*/                            \
            /* 参数3：queueQUEUE_TYPE_BINARY_SEMAPHORE 指定创建二进制信号量类型 */                               \
            ( xSemaphore ) = xQueueGenericCreate( ( UBaseType_t ) 1, semSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_BINARY_SEMAPHORE ); \
                                                                                                                \
            if( ( xSemaphore ) != NULL ) /* 信号量创建成功检查 */                                                \
            {                                                                                                   \
                /* 立即释放信号量，使其初始状态为可用 */                                                         \
                /* (void)强制转换忽略返回值，因初始化操作必定成功 */                                              \
                ( void ) xSemaphoreGive( ( xSemaphore ) );                                                      \
            }                                                                                                   \
        }
#endif  // 结束动态内存分配条件编译



/**
 * semphr. h
 * <pre>SemaphoreHandle_t xSemaphoreCreateBinary( void )</pre>
 *
 * Creates a new binary semaphore instance, and returns a handle by which the
 * new semaphore can be referenced.
 *
 * In many usage scenarios it is faster and more memory efficient to use a
 * direct to task notification in place of a binary semaphore!
 * http://www.freertos.org/RTOS-task-notifications.html
 *
 * Internally, within the FreeRTOS implementation, binary semaphores use a block
 * of memory, in which the semaphore structure is stored.  If a binary semaphore
 * is created using xSemaphoreCreateBinary() then the required memory is
 * automatically dynamically allocated inside the xSemaphoreCreateBinary()
 * function.  (see http://www.freertos.org/a00111.html).  If a binary semaphore
 * is created using xSemaphoreCreateBinaryStatic() then the application writer
 * must provide the memory.  xSemaphoreCreateBinaryStatic() therefore allows a
 * binary semaphore to be created without using any dynamic memory allocation.
 *
 * The old vSemaphoreCreateBinary() macro is now deprecated in favour of this
 * xSemaphoreCreateBinary() function.  Note that binary semaphores created using
 * the vSemaphoreCreateBinary() macro are created in a state such that the
 * first call to 'take' the semaphore would pass, whereas binary semaphores
 * created using xSemaphoreCreateBinary() are created in a state such that the
 * the semaphore must first be 'given' before it can be 'taken'.
 *
 * This type of semaphore can be used for pure synchronisation between tasks or
 * between an interrupt and a task.  The semaphore need not be given back once
 * obtained, so one task/interrupt can continuously 'give' the semaphore while
 * another continuously 'takes' the semaphore.  For this reason this type of
 * semaphore does not use a priority inheritance mechanism.  For an alternative
 * that does use priority inheritance see xSemaphoreCreateMutex().
 *
 * @return Handle to the created semaphore, or NULL if the memory required to
 * hold the semaphore's data structures could not be allocated.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore = NULL;

 void vATask( void * pvParameters )
 {
    // Semaphore cannot be used before a call to xSemaphoreCreateBinary().
    // This is a macro so pass the variable in directly.
    xSemaphore = xSemaphoreCreateBinary();

    if( xSemaphore != NULL )
    {
        // The semaphore was created successfully.
        // The semaphore can now be used.
    }
 }
 </pre>
 * \defgroup xSemaphoreCreateBinary xSemaphoreCreateBinary
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreCreateBinary
 * @brief 创建二进制信号量（动态内存版本）
 * @ingroup Semaphores
 * 
 * @return SemaphoreHandle_t 返回创建的信号量句柄，创建失败返回NULL
 * 
 * 该宏实现以下核心功能：
 * 1. 通过队列机制动态分配二进制信号量
 * 2. 使用预定义的队列参数（长度1/项长度0）
 * 3. 直接返回信号量句柄供后续操作使用
 * 
 * 与vSemaphoreCreateBinary的区别：
 * - 不自动执行Give操作，初始状态为不可用
 * - 采用函数式设计返回句柄，而非通过参数输出
 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )  // 动态内存分配配置检查
    #define xSemaphoreCreateBinary() \
        /* 核心创建函数调用 */ \
        /* 参数1：(UBaseType_t)1 确保队列长度为1，符合二进制信号量特性 */ \
        /* 参数2：semSEMAPHORE_QUEUE_ITEM_LENGTH=0 不存储实际数据 */ \
        /* 参数3：queueQUEUE_TYPE_BINARY_SEMAPHORE 指定信号量类型 */ \
        xQueueGenericCreate( ( UBaseType_t ) 1, semSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_BINARY_SEMAPHORE )
#endif  // 结束动态内存分配条件编译


/**
 * semphr. h
 * <pre>SemaphoreHandle_t xSemaphoreCreateBinaryStatic( StaticSemaphore_t *pxSemaphoreBuffer )</pre>
 *
 * Creates a new binary semaphore instance, and returns a handle by which the
 * new semaphore can be referenced.
 *
 * NOTE: In many usage scenarios it is faster and more memory efficient to use a
 * direct to task notification in place of a binary semaphore!
 * http://www.freertos.org/RTOS-task-notifications.html
 *
 * Internally, within the FreeRTOS implementation, binary semaphores use a block
 * of memory, in which the semaphore structure is stored.  If a binary semaphore
 * is created using xSemaphoreCreateBinary() then the required memory is
 * automatically dynamically allocated inside the xSemaphoreCreateBinary()
 * function.  (see http://www.freertos.org/a00111.html).  If a binary semaphore
 * is created using xSemaphoreCreateBinaryStatic() then the application writer
 * must provide the memory.  xSemaphoreCreateBinaryStatic() therefore allows a
 * binary semaphore to be created without using any dynamic memory allocation.
 *
 * This type of semaphore can be used for pure synchronisation between tasks or
 * between an interrupt and a task.  The semaphore need not be given back once
 * obtained, so one task/interrupt can continuously 'give' the semaphore while
 * another continuously 'takes' the semaphore.  For this reason this type of
 * semaphore does not use a priority inheritance mechanism.  For an alternative
 * that does use priority inheritance see xSemaphoreCreateMutex().
 *
 * @param pxSemaphoreBuffer Must point to a variable of type StaticSemaphore_t,
 * which will then be used to hold the semaphore's data structure, removing the
 * need for the memory to be allocated dynamically.
 *
 * @return If the semaphore is created then a handle to the created semaphore is
 * returned.  If pxSemaphoreBuffer is NULL then NULL is returned.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore = NULL;
 StaticSemaphore_t xSemaphoreBuffer;

 void vATask( void * pvParameters )
 {
    // Semaphore cannot be used before a call to xSemaphoreCreateBinary().
    // The semaphore's data structures will be placed in the xSemaphoreBuffer
    // variable, the address of which is passed into the function.  The
    // function's parameter is not NULL, so the function will not attempt any
    // dynamic memory allocation, and therefore the function will not return
    // return NULL.
    xSemaphore = xSemaphoreCreateBinary( &xSemaphoreBuffer );

    // Rest of task code goes here.
 }
 </pre>
 * \defgroup xSemaphoreCreateBinaryStatic xSemaphoreCreateBinaryStatic
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreCreateBinaryStatic
 * @brief 创建二进制信号量（静态内存版本）
 * @ingroup Semaphores
 * 
 * @param pxStaticSemaphore 指向预分配的StaticSemaphore_t结构体的指针
 * @return SemaphoreHandle_t 返回创建的信号量句柄，创建失败返回NULL
 * 
 * 该宏实现以下核心功能：
 * 1. 使用预分配的静态内存创建二进制信号量
 * 2. 避免动态内存分配，适合确定性内存管理场景
 * 3. 参数配置与动态版本保持一致（长度1/项长度0）
 */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )  // 静态内存分配使能检查
    #define xSemaphoreCreateBinaryStatic( pxStaticSemaphore ) \
        /* 静态队列创建函数调用 */ \
        /* 参数分解：                                                                       */ \
        /* (UBaseType_t)1        -> 队列长度固定为1                                        */ \
        /* semSEMAPHORE_QUEUE_ITEM_LENGTH=0 -> 空数据项                                    */ \
        /* NULL                  -> 不需要队列存储区域（已包含在静态结构体中）              */ \
        /* pxStaticSemaphore     -> 用户提供的静态内存缓冲区                                */ \
        /* queueQUEUE_TYPE_BINARY_SEMAPHORE -> 指定二进制信号量类型                        */ \
        xQueueGenericCreateStatic( ( UBaseType_t ) 1, semSEMAPHORE_QUEUE_ITEM_LENGTH, NULL, pxStaticSemaphore, queueQUEUE_TYPE_BINARY_SEMAPHORE )
#endif /* configSUPPORT_STATIC_ALLOCATION */  // 结束静态分配条件编译

/**
 * semphr. h
 * <pre>xSemaphoreTake(
 *                   SemaphoreHandle_t xSemaphore,
 *                   TickType_t xBlockTime
 *               )</pre>
 *
 * <i>Macro</i> to obtain a semaphore.  The semaphore must have previously been
 * created with a call to xSemaphoreCreateBinary(), xSemaphoreCreateMutex() or
 * xSemaphoreCreateCounting().
 *
 * @param xSemaphore A handle to the semaphore being taken - obtained when
 * the semaphore was created.
 *
 * @param xBlockTime The time in ticks to wait for the semaphore to become
 * available.  The macro portTICK_PERIOD_MS can be used to convert this to a
 * real time.  A block time of zero can be used to poll the semaphore.  A block
 * time of portMAX_DELAY can be used to block indefinitely (provided
 * INCLUDE_vTaskSuspend is set to 1 in FreeRTOSConfig.h).
 *
 * @return pdTRUE if the semaphore was obtained.  pdFALSE
 * if xBlockTime expired without the semaphore becoming available.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore = NULL;

 // A task that creates a semaphore.
 void vATask( void * pvParameters )
 {
    // Create the semaphore to guard a shared resource.
    xSemaphore = xSemaphoreCreateBinary();
 }

 // A task that uses the semaphore.
 void vAnotherTask( void * pvParameters )
 {
    // ... Do other things.

    if( xSemaphore != NULL )
    {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
        {
            // We were able to obtain the semaphore and can now access the
            // shared resource.

            // ...

            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive( xSemaphore );
        }
        else
        {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
        }
    }
 }
 </pre>
 * \defgroup xSemaphoreTake xSemaphoreTake
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreTake
 * @brief 获取信号量（阻塞式）
 * @ingroup Semaphores
 * 
 * @param xSemaphore 要获取的信号量句柄（必须已创建）
 * @param xBlockTime 最大阻塞时间（单位：时钟节拍）
 * @return pdTRUE获取成功，pdFALSE超时或失败
 * 
 * 该宏实现以下核心机制：
 * 1. 通过队列接收机制实现信号量获取
 * 2. 不实际接收数据（NULL缓冲区）
 * 3. 支持可配置的阻塞超时机制
 * 
 * 工作流程：
 * - 信号量计数>0时立即返回成功（计数减1）
 * - 信号量计数=0时根据xBlockTime阻塞等待
 */
#define xSemaphoreTake( xSemaphore, xBlockTime ) \
    /* 将信号量句柄强制转换为队列句柄类型 */ \
    /* 参数分解：                                                                  */ \
    /* (QueueHandle_t)(xSemaphore) -> 类型转换保证队列操作合法性                  */ \
    /* NULL              -> 不需要数据缓冲区（信号量只操作计数）                   */ \
    /* xBlockTime        -> 用户指定的最大等待时间                                */ \
    /* pdFALSE          -> 非队列末尾接收模式                                     */ \
    xQueueGenericReceive( ( QueueHandle_t ) ( xSemaphore ), NULL, ( xBlockTime ), pdFALSE )

/**
 * semphr. h
 * xSemaphoreTakeRecursive(
 *                          SemaphoreHandle_t xMutex,
 *                          TickType_t xBlockTime
 *                        )
 *
 * <i>Macro</i> to recursively obtain, or 'take', a mutex type semaphore.
 * The mutex must have previously been created using a call to
 * xSemaphoreCreateRecursiveMutex();
 *
 * configUSE_RECURSIVE_MUTEXES must be set to 1 in FreeRTOSConfig.h for this
 * macro to be available.
 *
 * This macro must not be used on mutexes created using xSemaphoreCreateMutex().
 *
 * A mutex used recursively can be 'taken' repeatedly by the owner. The mutex
 * doesn't become available again until the owner has called
 * xSemaphoreGiveRecursive() for each successful 'take' request.  For example,
 * if a task successfully 'takes' the same mutex 5 times then the mutex will
 * not be available to any other task until it has also  'given' the mutex back
 * exactly five times.
 *
 * @param xMutex A handle to the mutex being obtained.  This is the
 * handle returned by xSemaphoreCreateRecursiveMutex();
 *
 * @param xBlockTime The time in ticks to wait for the semaphore to become
 * available.  The macro portTICK_PERIOD_MS can be used to convert this to a
 * real time.  A block time of zero can be used to poll the semaphore.  If
 * the task already owns the semaphore then xSemaphoreTakeRecursive() will
 * return immediately no matter what the value of xBlockTime.
 *
 * @return pdTRUE if the semaphore was obtained.  pdFALSE if xBlockTime
 * expired without the semaphore becoming available.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xMutex = NULL;

 // A task that creates a mutex.
 void vATask( void * pvParameters )
 {
    // Create the mutex to guard a shared resource.
    xMutex = xSemaphoreCreateRecursiveMutex();
 }

 // A task that uses the mutex.
 void vAnotherTask( void * pvParameters )
 {
    // ... Do other things.

    if( xMutex != NULL )
    {
        // See if we can obtain the mutex.  If the mutex is not available
        // wait 10 ticks to see if it becomes free.
        if( xSemaphoreTakeRecursive( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
        {
            // We were able to obtain the mutex and can now access the
            // shared resource.

            // ...
            // For some reason due to the nature of the code further calls to
			// xSemaphoreTakeRecursive() are made on the same mutex.  In real
			// code these would not be just sequential calls as this would make
			// no sense.  Instead the calls are likely to be buried inside
			// a more complex call structure.
            xSemaphoreTakeRecursive( xMutex, ( TickType_t ) 10 );
            xSemaphoreTakeRecursive( xMutex, ( TickType_t ) 10 );

            // The mutex has now been 'taken' three times, so will not be
			// available to another task until it has also been given back
			// three times.  Again it is unlikely that real code would have
			// these calls sequentially, but instead buried in a more complex
			// call structure.  This is just for illustrative purposes.
            xSemaphoreGiveRecursive( xMutex );
			xSemaphoreGiveRecursive( xMutex );
			xSemaphoreGiveRecursive( xMutex );

			// Now the mutex can be taken by other tasks.
        }
        else
        {
            // We could not obtain the mutex and can therefore not access
            // the shared resource safely.
        }
    }
 }
 </pre>
 * \defgroup xSemaphoreTakeRecursive xSemaphoreTakeRecursive
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreTakeRecursive
 * @brief 递归获取互斥量（阻塞式）
 * @ingroup Semaphores
 * 
 * @param xMutex 递归互斥量句柄（必须由xSemaphoreCreateRecursiveMutex创建）
 * @param xBlockTime 最大阻塞时间（单位：时钟节拍）
 * @return pdTRUE获取成功，pdFAIL失败（句柄无效或超过最大递归深度）
 * 
 * 该宏实现以下核心机制：
 * 1. 允许同一任务多次获取互斥量而不死锁
 * 2. 跟踪当前任务的递归获取次数
 * 3. 仅当递归计数归零时实际释放互斥量
 */
#if( configUSE_RECURSIVE_MUTEXES == 1 )  // 递归互斥量功能使能检查
    #define xSemaphoreTakeRecursive( xMutex, xBlockTime ) \
        /* 调用队列专用递归获取函数 */ \
        /* 参数分解：                                                                 */ \
        /* xMutex       -> 必须是递归互斥量类型（通过xSemaphoreCreateRecursiveMutex创建） */ \
        /* xBlockTime   -> 阻塞时间遵循标准信号量规则 */ \
        xQueueTakeMutexRecursive( ( xMutex ), ( xBlockTime ) )
#endif  // 结束递归互斥量条件编译


/**
 * semphr. h
 * <pre>xSemaphoreGive( SemaphoreHandle_t xSemaphore )</pre>
 *
 * <i>Macro</i> to release a semaphore.  The semaphore must have previously been
 * created with a call to xSemaphoreCreateBinary(), xSemaphoreCreateMutex() or
 * xSemaphoreCreateCounting(). and obtained using sSemaphoreTake().
 *
 * This macro must not be used from an ISR.  See xSemaphoreGiveFromISR () for
 * an alternative which can be used from an ISR.
 *
 * This macro must also not be used on semaphores created using
 * xSemaphoreCreateRecursiveMutex().
 *
 * @param xSemaphore A handle to the semaphore being released.  This is the
 * handle returned when the semaphore was created.
 *
 * @return pdTRUE if the semaphore was released.  pdFALSE if an error occurred.
 * Semaphores are implemented using queues.  An error can occur if there is
 * no space on the queue to post a message - indicating that the
 * semaphore was not first obtained correctly.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore = NULL;

 void vATask( void * pvParameters )
 {
    // Create the semaphore to guard a shared resource.
    xSemaphore = vSemaphoreCreateBinary();

    if( xSemaphore != NULL )
    {
        if( xSemaphoreGive( xSemaphore ) != pdTRUE )
        {
            // We would expect this call to fail because we cannot give
            // a semaphore without first "taking" it!
        }

        // Obtain the semaphore - don't block if the semaphore is not
        // immediately available.
        if( xSemaphoreTake( xSemaphore, ( TickType_t ) 0 ) )
        {
            // We now have the semaphore and can access the shared resource.

            // ...

            // We have finished accessing the shared resource so can free the
            // semaphore.
            if( xSemaphoreGive( xSemaphore ) != pdTRUE )
            {
                // We would not expect this call to fail because we must have
                // obtained the semaphore to get here.
            }
        }
    }
 }
 </pre>
 * \defgroup xSemaphoreGive xSemaphoreGive
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreGive
 * @brief 释放信号量（非阻塞式）
 * @ingroup Semaphores
 * 
 * @param xSemaphore 要释放的信号量句柄
 * @return pdTRUE释放成功，pdFAIL失败（无效句柄或计数溢出）
 * 
 * 该宏实现以下核心机制：
 * 1. 通过队列发送机制实现信号量计数递增
 * 2. 采用零阻塞时间保证操作原子性
 * 3. 支持二进制/计数信号量的通用释放操作
 */
#define xSemaphoreGive( xSemaphore ) \
    /* 将信号量转换为队列句柄类型 */ \
    /* 参数分解：                                                                  */ \
    /* (QueueHandle_t)强制转换 -> 保持类型系统一致性                              */ \
    /* NULL          -> 无实际数据发送（仅操作信号量计数）                        */ \
    /* semGIVE_BLOCK_TIME=0 -> 非阻塞操作（立即返回结果）                         */ \
    /* queueSEND_TO_BACK    -> 按FIFO顺序唤醒等待任务                             */ \
    xQueueGenericSend( ( QueueHandle_t ) ( xSemaphore ), NULL, semGIVE_BLOCK_TIME, queueSEND_TO_BACK )

/**
 * semphr. h
 * <pre>xSemaphoreGiveRecursive( SemaphoreHandle_t xMutex )</pre>
 *
 * <i>Macro</i> to recursively release, or 'give', a mutex type semaphore.
 * The mutex must have previously been created using a call to
 * xSemaphoreCreateRecursiveMutex();
 *
 * configUSE_RECURSIVE_MUTEXES must be set to 1 in FreeRTOSConfig.h for this
 * macro to be available.
 *
 * This macro must not be used on mutexes created using xSemaphoreCreateMutex().
 *
 * A mutex used recursively can be 'taken' repeatedly by the owner. The mutex
 * doesn't become available again until the owner has called
 * xSemaphoreGiveRecursive() for each successful 'take' request.  For example,
 * if a task successfully 'takes' the same mutex 5 times then the mutex will
 * not be available to any other task until it has also  'given' the mutex back
 * exactly five times.
 *
 * @param xMutex A handle to the mutex being released, or 'given'.  This is the
 * handle returned by xSemaphoreCreateMutex();
 *
 * @return pdTRUE if the semaphore was given.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xMutex = NULL;

 // A task that creates a mutex.
 void vATask( void * pvParameters )
 {
    // Create the mutex to guard a shared resource.
    xMutex = xSemaphoreCreateRecursiveMutex();
 }

 // A task that uses the mutex.
 void vAnotherTask( void * pvParameters )
 {
    // ... Do other things.

    if( xMutex != NULL )
    {
        // See if we can obtain the mutex.  If the mutex is not available
        // wait 10 ticks to see if it becomes free.
        if( xSemaphoreTakeRecursive( xMutex, ( TickType_t ) 10 ) == pdTRUE )
        {
            // We were able to obtain the mutex and can now access the
            // shared resource.

            // ...
            // For some reason due to the nature of the code further calls to
			// xSemaphoreTakeRecursive() are made on the same mutex.  In real
			// code these would not be just sequential calls as this would make
			// no sense.  Instead the calls are likely to be buried inside
			// a more complex call structure.
            xSemaphoreTakeRecursive( xMutex, ( TickType_t ) 10 );
            xSemaphoreTakeRecursive( xMutex, ( TickType_t ) 10 );

            // The mutex has now been 'taken' three times, so will not be
			// available to another task until it has also been given back
			// three times.  Again it is unlikely that real code would have
			// these calls sequentially, it would be more likely that the calls
			// to xSemaphoreGiveRecursive() would be called as a call stack
			// unwound.  This is just for demonstrative purposes.
            xSemaphoreGiveRecursive( xMutex );
			xSemaphoreGiveRecursive( xMutex );
			xSemaphoreGiveRecursive( xMutex );

			// Now the mutex can be taken by other tasks.
        }
        else
        {
            // We could not obtain the mutex and can therefore not access
            // the shared resource safely.
        }
    }
 }
 </pre>
 * \defgroup xSemaphoreGiveRecursive xSemaphoreGiveRecursive
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreGiveRecursive
 * @brief 递归释放互斥量
 * @ingroup Semaphores
 * 
 * @param xMutex 递归互斥量句柄（必须已通过TakeRecursive获取）
 * @return pdTRUE释放成功，pdFAIL失败（未持有或无效句柄）
 * 
 * 该宏实现以下核心机制：
 * 1. 递减当前任务的递归持有计数
 * 2. 仅当计数归零时实际释放互斥量
 * 3. 严格校验调用任务的持有权
 */
#if( configUSE_RECURSIVE_MUTEXES == 1 )  // 递归互斥量功能使能检查
    #define xSemaphoreGiveRecursive( xMutex ) \
        /* 调用队列专用递归释放函数 */ \
        /* 参数约束：                                                                       */ \
        /* xMutex必须由当前任务通过xSemaphoreTakeRecursive获取                               */ \
        /* 每个Give必须对应一个Take                                                         */ \
        xQueueGiveMutexRecursive( ( xMutex ) )
#endif  // 结束递归互斥量条件编译


/**
 * semphr. h
 * <pre>
 xSemaphoreGiveFromISR(
                          SemaphoreHandle_t xSemaphore,
                          BaseType_t *pxHigherPriorityTaskWoken
                      )</pre>
 *
 * <i>Macro</i> to  release a semaphore.  The semaphore must have previously been
 * created with a call to xSemaphoreCreateBinary() or xSemaphoreCreateCounting().
 *
 * Mutex type semaphores (those created using a call to xSemaphoreCreateMutex())
 * must not be used with this macro.
 *
 * This macro can be used from an ISR.
 *
 * @param xSemaphore A handle to the semaphore being released.  This is the
 * handle returned when the semaphore was created.
 *
 * @param pxHigherPriorityTaskWoken xSemaphoreGiveFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if giving the semaphore caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xSemaphoreGiveFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 *
 * @return pdTRUE if the semaphore was successfully given, otherwise errQUEUE_FULL.
 *
 * Example usage:
 <pre>
 \#define LONG_TIME 0xffff
 \#define TICKS_TO_WAIT	10
 SemaphoreHandle_t xSemaphore = NULL;

 // Repetitive task.
 void vATask( void * pvParameters )
 {
    for( ;; )
    {
        // We want this task to run every 10 ticks of a timer.  The semaphore
        // was created before this task was started.

        // Block waiting for the semaphore to become available.
        if( xSemaphoreTake( xSemaphore, LONG_TIME ) == pdTRUE )
        {
            // It is time to execute.

            // ...

            // We have finished our task.  Return to the top of the loop where
            // we will block on the semaphore until it is time to execute
            // again.  Note when using the semaphore for synchronisation with an
			// ISR in this manner there is no need to 'give' the semaphore back.
        }
    }
 }

 // Timer ISR
 void vTimerISR( void * pvParameters )
 {
 static uint8_t ucLocalTickCount = 0;
 static BaseType_t xHigherPriorityTaskWoken;

    // A timer tick has occurred.

    // ... Do other time functions.

    // Is it time for vATask () to run?
	xHigherPriorityTaskWoken = pdFALSE;
    ucLocalTickCount++;
    if( ucLocalTickCount >= TICKS_TO_WAIT )
    {
        // Unblock the task by releasing the semaphore.
        xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );

        // Reset the count so we release the semaphore again in 10 ticks time.
        ucLocalTickCount = 0;
    }

    if( xHigherPriorityTaskWoken != pdFALSE )
    {
        // We can force a context switch here.  Context switching from an
        // ISR uses port specific syntax.  Check the demo task for your port
        // to find the syntax required.
    }
 }
 </pre>
 * \defgroup xSemaphoreGiveFromISR xSemaphoreGiveFromISR
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreGiveFromISR
 * @brief 在中断服务例程中释放信号量
 * @ingroup Semaphores
 * 
 * @param xSemaphore 要释放的信号量句柄
 * @param pxHigherPriorityTaskWoken 用于返回是否需要上下文切换的标记指针
 * @return pdTRUE释放成功，pdFAIL失败（无效句柄或信号量已满）
 * 
 * 该宏实现以下核心机制：
 * 1. 专为中断上下文设计的无阻塞信号量释放
 * 2. 安全的原子操作保证中断与任务的数据一致性
 * 3. 支持唤醒等待任务并触发优先级继承
 */
#define xSemaphoreGiveFromISR( xSemaphore, pxHigherPriorityTaskWoken ) \
    /* 将信号量转换为队列句柄类型 */ \
    /* 参数分解：                                                                          */ \
    /* (QueueHandle_t)强制转换 -> 保持类型系统一致性                                        */ \
    /* pxHigherPriorityTaskWoken -> 输出参数记录高优先级任务唤醒状态                        */ \
    xQueueGiveFromISR( ( QueueHandle_t ) ( xSemaphore ), ( pxHigherPriorityTaskWoken ) )

/**
 * semphr. h
 * <pre>
 xSemaphoreTakeFromISR(
                          SemaphoreHandle_t xSemaphore,
                          BaseType_t *pxHigherPriorityTaskWoken
                      )</pre>
 *
 * <i>Macro</i> to  take a semaphore from an ISR.  The semaphore must have
 * previously been created with a call to xSemaphoreCreateBinary() or
 * xSemaphoreCreateCounting().
 *
 * Mutex type semaphores (those created using a call to xSemaphoreCreateMutex())
 * must not be used with this macro.
 *
 * This macro can be used from an ISR, however taking a semaphore from an ISR
 * is not a common operation.  It is likely to only be useful when taking a
 * counting semaphore when an interrupt is obtaining an object from a resource
 * pool (when the semaphore count indicates the number of resources available).
 *
 * @param xSemaphore A handle to the semaphore being taken.  This is the
 * handle returned when the semaphore was created.
 *
 * @param pxHigherPriorityTaskWoken xSemaphoreTakeFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if taking the semaphore caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xSemaphoreTakeFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 *
 * @return pdTRUE if the semaphore was successfully taken, otherwise
 * pdFALSE
 */
#define xSemaphoreTakeFromISR( xSemaphore, pxHigherPriorityTaskWoken )	xQueueReceiveFromISR( ( QueueHandle_t ) ( xSemaphore ), NULL, ( pxHigherPriorityTaskWoken ) )

/**
 * semphr. h
 * <pre>SemaphoreHandle_t xSemaphoreCreateMutex( void )</pre>
 *
 * Creates a new mutex type semaphore instance, and returns a handle by which
 * the new mutex can be referenced.
 *
 * Internally, within the FreeRTOS implementation, mutex semaphores use a block
 * of memory, in which the mutex structure is stored.  If a mutex is created
 * using xSemaphoreCreateMutex() then the required memory is automatically
 * dynamically allocated inside the xSemaphoreCreateMutex() function.  (see
 * http://www.freertos.org/a00111.html).  If a mutex is created using
 * xSemaphoreCreateMutexStatic() then the application writer must provided the
 * memory.  xSemaphoreCreateMutexStatic() therefore allows a mutex to be created
 * without using any dynamic memory allocation.
 *
 * Mutexes created using this function can be accessed using the xSemaphoreTake()
 * and xSemaphoreGive() macros.  The xSemaphoreTakeRecursive() and
 * xSemaphoreGiveRecursive() macros must not be used.
 *
 * This type of semaphore uses a priority inheritance mechanism so a task
 * 'taking' a semaphore MUST ALWAYS 'give' the semaphore back once the
 * semaphore it is no longer required.
 *
 * Mutex type semaphores cannot be used from within interrupt service routines.
 *
 * See xSemaphoreCreateBinary() for an alternative implementation that can be
 * used for pure synchronisation (where one task or interrupt always 'gives' the
 * semaphore and another always 'takes' the semaphore) and from within interrupt
 * service routines.
 *
 * @return If the mutex was successfully created then a handle to the created
 * semaphore is returned.  If there was not enough heap to allocate the mutex
 * data structures then NULL is returned.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore;

 void vATask( void * pvParameters )
 {
    // Semaphore cannot be used before a call to xSemaphoreCreateMutex().
    // This is a macro so pass the variable in directly.
    xSemaphore = xSemaphoreCreateMutex();

    if( xSemaphore != NULL )
    {
        // The semaphore was created successfully.
        // The semaphore can now be used.
    }
 }
 </pre>
 * \defgroup xSemaphoreCreateMutex xSemaphoreCreateMutex
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreCreateMutex
 * @brief 动态创建互斥量（基于队列实现）
 * @ingroup Semaphores
 * 
 * @return SemaphoreHandle_t 成功返回互斥量句柄，失败返回NULL
 * 
 * 该宏实现以下核心机制：
 * 1. 通过队列机制动态分配互斥量内存
 * 2. 自动初始化互斥量为可用状态
 * 3. 支持优先级继承防止优先级反转
 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )  // 动态内存分配使能检查
    #define xSemaphoreCreateMutex() \
        /* 调用队列互斥量创建函数 */ \
        /* 参数分解：                                                       */ \
        /* queueQUEUE_TYPE_MUTEX -> 指定创建互斥量类型（含优先级继承机制）   */ \
        xQueueCreateMutex( queueQUEUE_TYPE_MUTEX )
#endif  // 结束动态分配条件编译

/**
 * semphr. h
 * <pre>SemaphoreHandle_t xSemaphoreCreateMutexStatic( StaticSemaphore_t *pxMutexBuffer )</pre>
 *
 * Creates a new mutex type semaphore instance, and returns a handle by which
 * the new mutex can be referenced.
 *
 * Internally, within the FreeRTOS implementation, mutex semaphores use a block
 * of memory, in which the mutex structure is stored.  If a mutex is created
 * using xSemaphoreCreateMutex() then the required memory is automatically
 * dynamically allocated inside the xSemaphoreCreateMutex() function.  (see
 * http://www.freertos.org/a00111.html).  If a mutex is created using
 * xSemaphoreCreateMutexStatic() then the application writer must provided the
 * memory.  xSemaphoreCreateMutexStatic() therefore allows a mutex to be created
 * without using any dynamic memory allocation.
 *
 * Mutexes created using this function can be accessed using the xSemaphoreTake()
 * and xSemaphoreGive() macros.  The xSemaphoreTakeRecursive() and
 * xSemaphoreGiveRecursive() macros must not be used.
 *
 * This type of semaphore uses a priority inheritance mechanism so a task
 * 'taking' a semaphore MUST ALWAYS 'give' the semaphore back once the
 * semaphore it is no longer required.
 *
 * Mutex type semaphores cannot be used from within interrupt service routines.
 *
 * See xSemaphoreCreateBinary() for an alternative implementation that can be
 * used for pure synchronisation (where one task or interrupt always 'gives' the
 * semaphore and another always 'takes' the semaphore) and from within interrupt
 * service routines.
 *
 * @param pxMutexBuffer Must point to a variable of type StaticSemaphore_t,
 * which will be used to hold the mutex's data structure, removing the need for
 * the memory to be allocated dynamically.
 *
 * @return If the mutex was successfully created then a handle to the created
 * mutex is returned.  If pxMutexBuffer was NULL then NULL is returned.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore;
 StaticSemaphore_t xMutexBuffer;

 void vATask( void * pvParameters )
 {
    // A mutex cannot be used before it has been created.  xMutexBuffer is
    // into xSemaphoreCreateMutexStatic() so no dynamic memory allocation is
    // attempted.
    xSemaphore = xSemaphoreCreateMutexStatic( &xMutexBuffer );

    // As no dynamic memory allocation was performed, xSemaphore cannot be NULL,
    // so there is no need to check it.
 }
 </pre>
 * \defgroup xSemaphoreCreateMutexStatic xSemaphoreCreateMutexStatic
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreCreateMutexStatic
 * @brief 静态创建互斥量（基于预分配内存）
 * @ingroup Semaphores
 * 
 * @param pxMutexBuffer 指向StaticSemaphore_t类型静态内存缓冲区的指针
 * @return SemaphoreHandle_t 成功返回互斥量句柄，失败返回NULL
 * 
 * 该宏实现以下核心机制：
 * 1. 使用用户提供的静态内存初始化互斥量
 * 2. 避免动态内存分配，确保内存确定性
 * 3. 支持优先级继承机制
 */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )  // 静态内存分配使能检查
    #define xSemaphoreCreateMutexStatic( pxMutexBuffer ) \
        /* 调用静态队列互斥量创建函数 */ \
        /* 参数分解：                                                              */ \
        /* queueQUEUE_TYPE_MUTEX -> 指定互斥量类型（含优先级继承）                 */ \
        /* pxMutexBuffer         -> 用户提供的StaticSemaphore_t类型内存块          */ \
        xQueueCreateMutexStatic( queueQUEUE_TYPE_MUTEX, ( pxMutexBuffer ) )
#endif /* configSUPPORT_STATIC_ALLOCATION */  // 结束静态分配条件编译


/**
 * semphr. h
 * <pre>SemaphoreHandle_t xSemaphoreCreateRecursiveMutex( void )</pre>
 *
 * Creates a new recursive mutex type semaphore instance, and returns a handle
 * by which the new recursive mutex can be referenced.
 *
 * Internally, within the FreeRTOS implementation, recursive mutexs use a block
 * of memory, in which the mutex structure is stored.  If a recursive mutex is
 * created using xSemaphoreCreateRecursiveMutex() then the required memory is
 * automatically dynamically allocated inside the
 * xSemaphoreCreateRecursiveMutex() function.  (see
 * http://www.freertos.org/a00111.html).  If a recursive mutex is created using
 * xSemaphoreCreateRecursiveMutexStatic() then the application writer must
 * provide the memory that will get used by the mutex.
 * xSemaphoreCreateRecursiveMutexStatic() therefore allows a recursive mutex to
 * be created without using any dynamic memory allocation.
 *
 * Mutexes created using this macro can be accessed using the
 * xSemaphoreTakeRecursive() and xSemaphoreGiveRecursive() macros.  The
 * xSemaphoreTake() and xSemaphoreGive() macros must not be used.
 *
 * A mutex used recursively can be 'taken' repeatedly by the owner. The mutex
 * doesn't become available again until the owner has called
 * xSemaphoreGiveRecursive() for each successful 'take' request.  For example,
 * if a task successfully 'takes' the same mutex 5 times then the mutex will
 * not be available to any other task until it has also  'given' the mutex back
 * exactly five times.
 *
 * This type of semaphore uses a priority inheritance mechanism so a task
 * 'taking' a semaphore MUST ALWAYS 'give' the semaphore back once the
 * semaphore it is no longer required.
 *
 * Mutex type semaphores cannot be used from within interrupt service routines.
 *
 * See xSemaphoreCreateBinary() for an alternative implementation that can be
 * used for pure synchronisation (where one task or interrupt always 'gives' the
 * semaphore and another always 'takes' the semaphore) and from within interrupt
 * service routines.
 *
 * @return xSemaphore Handle to the created mutex semaphore.  Should be of type
 * SemaphoreHandle_t.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore;

 void vATask( void * pvParameters )
 {
    // Semaphore cannot be used before a call to xSemaphoreCreateMutex().
    // This is a macro so pass the variable in directly.
    xSemaphore = xSemaphoreCreateRecursiveMutex();

    if( xSemaphore != NULL )
    {
        // The semaphore was created successfully.
        // The semaphore can now be used.
    }
 }
 </pre>
 * \defgroup xSemaphoreCreateRecursiveMutex xSemaphoreCreateRecursiveMutex
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreCreateRecursiveMutex
 * @brief 动态创建递归互斥量（支持嵌套获取）
 * @ingroup Semaphores
 * 
 * @return SemaphoreHandle_t 成功返回互斥量句柄，失败返回NULL
 * 
 * 该宏实现以下核心机制：
 * 1. 基于队列机制动态分配递归互斥量内存
 * 2. 初始化递归计数器为0
 * 3. 启用优先级继承防止优先级反转
 * 
 * 适用条件：同时启用动态内存分配和递归互斥量功能
 */
#if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configUSE_RECURSIVE_MUTEXES == 1 ) )  // 双重配置检查
    #define xSemaphoreCreateRecursiveMutex() \
        /* 调用队列互斥量创建函数 */ \
        /* 参数分解：                                                                         */ \
        /* queueQUEUE_TYPE_RECURSIVE_MUTEX -> 指定递归互斥量类型（含嵌套计数器）              */ \
        xQueueCreateMutex( queueQUEUE_TYPE_RECURSIVE_MUTEX )
#endif  // 结束双重条件编译

/**
 * semphr. h
 * <pre>SemaphoreHandle_t xSemaphoreCreateRecursiveMutexStatic( StaticSemaphore_t *pxMutexBuffer )</pre>
 *
 * Creates a new recursive mutex type semaphore instance, and returns a handle
 * by which the new recursive mutex can be referenced.
 *
 * Internally, within the FreeRTOS implementation, recursive mutexs use a block
 * of memory, in which the mutex structure is stored.  If a recursive mutex is
 * created using xSemaphoreCreateRecursiveMutex() then the required memory is
 * automatically dynamically allocated inside the
 * xSemaphoreCreateRecursiveMutex() function.  (see
 * http://www.freertos.org/a00111.html).  If a recursive mutex is created using
 * xSemaphoreCreateRecursiveMutexStatic() then the application writer must
 * provide the memory that will get used by the mutex.
 * xSemaphoreCreateRecursiveMutexStatic() therefore allows a recursive mutex to
 * be created without using any dynamic memory allocation.
 *
 * Mutexes created using this macro can be accessed using the
 * xSemaphoreTakeRecursive() and xSemaphoreGiveRecursive() macros.  The
 * xSemaphoreTake() and xSemaphoreGive() macros must not be used.
 *
 * A mutex used recursively can be 'taken' repeatedly by the owner. The mutex
 * doesn't become available again until the owner has called
 * xSemaphoreGiveRecursive() for each successful 'take' request.  For example,
 * if a task successfully 'takes' the same mutex 5 times then the mutex will
 * not be available to any other task until it has also  'given' the mutex back
 * exactly five times.
 *
 * This type of semaphore uses a priority inheritance mechanism so a task
 * 'taking' a semaphore MUST ALWAYS 'give' the semaphore back once the
 * semaphore it is no longer required.
 *
 * Mutex type semaphores cannot be used from within interrupt service routines.
 *
 * See xSemaphoreCreateBinary() for an alternative implementation that can be
 * used for pure synchronisation (where one task or interrupt always 'gives' the
 * semaphore and another always 'takes' the semaphore) and from within interrupt
 * service routines.
 *
 * @param pxMutexBuffer Must point to a variable of type StaticSemaphore_t,
 * which will then be used to hold the recursive mutex's data structure,
 * removing the need for the memory to be allocated dynamically.
 *
 * @return If the recursive mutex was successfully created then a handle to the
 * created recursive mutex is returned.  If pxMutexBuffer was NULL then NULL is
 * returned.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore;
 StaticSemaphore_t xMutexBuffer;

 void vATask( void * pvParameters )
 {
    // A recursive semaphore cannot be used before it is created.  Here a
    // recursive mutex is created using xSemaphoreCreateRecursiveMutexStatic().
    // The address of xMutexBuffer is passed into the function, and will hold
    // the mutexes data structures - so no dynamic memory allocation will be
    // attempted.
    xSemaphore = xSemaphoreCreateRecursiveMutexStatic( &xMutexBuffer );

    // As no dynamic memory allocation was performed, xSemaphore cannot be NULL,
    // so there is no need to check it.
 }
 </pre>
 * \defgroup xSemaphoreCreateRecursiveMutexStatic xSemaphoreCreateRecursiveMutexStatic
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreCreateRecursiveMutexStatic
 * @brief 静态创建递归互斥量（基于预分配内存）
 * @ingroup Semaphores
 * 
 * @param pxStaticSemaphore 指向StaticSemaphore_t类型静态内存缓冲区的指针
 * @return SemaphoreHandle_t 成功返回递归互斥量句柄，失败返回NULL
 * 
 * 该宏实现以下核心机制：
 * 1. 使用用户提供的静态内存初始化递归互斥量
 * 2. 支持嵌套获取/释放操作（递归计数器维护）
 * 3. 启用优先级继承机制防止优先级反转
 * 
 * 适用条件：同时启用静态内存分配和递归互斥量功能
 */
#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_RECURSIVE_MUTEXES == 1 ) )  // 双重配置检查
    #define xSemaphoreCreateRecursiveMutexStatic( pxStaticSemaphore ) \
        /* 调用静态队列互斥量创建函数 */ \
        /* 参数分解：                                                                               */ \
        /* queueQUEUE_TYPE_RECURSIVE_MUTEX -> 指定递归互斥量类型（含嵌套计数器机制）                */ \
        /* pxStaticSemaphore             -> 用户提供的StaticSemaphore_t类型内存块                   */ \
        xQueueCreateMutexStatic( queueQUEUE_TYPE_RECURSIVE_MUTEX, pxStaticSemaphore )
#endif /* configSUPPORT_STATIC_ALLOCATION */  // 结束静态分配条件编译

/**
 * semphr. h
 * <pre>SemaphoreHandle_t xSemaphoreCreateCounting( UBaseType_t uxMaxCount, UBaseType_t uxInitialCount )</pre>
 *
 * Creates a new counting semaphore instance, and returns a handle by which the
 * new counting semaphore can be referenced.
 *
 * In many usage scenarios it is faster and more memory efficient to use a
 * direct to task notification in place of a counting semaphore!
 * http://www.freertos.org/RTOS-task-notifications.html
 *
 * Internally, within the FreeRTOS implementation, counting semaphores use a
 * block of memory, in which the counting semaphore structure is stored.  If a
 * counting semaphore is created using xSemaphoreCreateCounting() then the
 * required memory is automatically dynamically allocated inside the
 * xSemaphoreCreateCounting() function.  (see
 * http://www.freertos.org/a00111.html).  If a counting semaphore is created
 * using xSemaphoreCreateCountingStatic() then the application writer can
 * instead optionally provide the memory that will get used by the counting
 * semaphore.  xSemaphoreCreateCountingStatic() therefore allows a counting
 * semaphore to be created without using any dynamic memory allocation.
 *
 * Counting semaphores are typically used for two things:
 *
 * 1) Counting events.
 *
 *    In this usage scenario an event handler will 'give' a semaphore each time
 *    an event occurs (incrementing the semaphore count value), and a handler
 *    task will 'take' a semaphore each time it processes an event
 *    (decrementing the semaphore count value).  The count value is therefore
 *    the difference between the number of events that have occurred and the
 *    number that have been processed.  In this case it is desirable for the
 *    initial count value to be zero.
 *
 * 2) Resource management.
 *
 *    In this usage scenario the count value indicates the number of resources
 *    available.  To obtain control of a resource a task must first obtain a
 *    semaphore - decrementing the semaphore count value.  When the count value
 *    reaches zero there are no free resources.  When a task finishes with the
 *    resource it 'gives' the semaphore back - incrementing the semaphore count
 *    value.  In this case it is desirable for the initial count value to be
 *    equal to the maximum count value, indicating that all resources are free.
 *
 * @param uxMaxCount The maximum count value that can be reached.  When the
 *        semaphore reaches this value it can no longer be 'given'.
 *
 * @param uxInitialCount The count value assigned to the semaphore when it is
 *        created.
 *
 * @return Handle to the created semaphore.  Null if the semaphore could not be
 *         created.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore;

 void vATask( void * pvParameters )
 {
 SemaphoreHandle_t xSemaphore = NULL;

    // Semaphore cannot be used before a call to xSemaphoreCreateCounting().
    // The max value to which the semaphore can count should be 10, and the
    // initial value assigned to the count should be 0.
    xSemaphore = xSemaphoreCreateCounting( 10, 0 );

    if( xSemaphore != NULL )
    {
        // The semaphore was created successfully.
        // The semaphore can now be used.
    }
 }
 </pre>
 * \defgroup xSemaphoreCreateCounting xSemaphoreCreateCounting
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreCreateCounting
 * @brief 动态创建计数信号量（资源池管理）
 * @ingroup Semaphores
 * 
 * @param uxMaxCount 信号量最大计数值（资源池容量）
 * @param uxInitialCount 信号量初始可用计数（初始资源数量）
 * @return SemaphoreHandle_t 成功返回信号量句柄，失败返回NULL
 * 
 * 该宏实现以下核心机制：
 * 1. 动态分配内存创建计数信号量控制块
 * 2. 初始化信号量计数状态
 * 3. 支持资源池模式的Take/Give操作
 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )  // 动态内存分配使能检查
    #define xSemaphoreCreateCounting( uxMaxCount, uxInitialCount ) \
        /* 调用计数信号量创建函数 */ \
        /* 参数分解：                                                                   */ \
        /* uxMaxCount   -> 设定信号量能达到的最大计数值（防止资源超额分配）             */ \
        /* uxInitialCount -> 初始化时立即可用的资源数量                                 */ \
        xQueueCreateCountingSemaphore( ( uxMaxCount ), ( uxInitialCount ) )
#endif  // 结束动态分配条件编译

/**
 * semphr. h
 * <pre>SemaphoreHandle_t xSemaphoreCreateCountingStatic( UBaseType_t uxMaxCount, UBaseType_t uxInitialCount, StaticSemaphore_t *pxSemaphoreBuffer )</pre>
 *
 * Creates a new counting semaphore instance, and returns a handle by which the
 * new counting semaphore can be referenced.
 *
 * In many usage scenarios it is faster and more memory efficient to use a
 * direct to task notification in place of a counting semaphore!
 * http://www.freertos.org/RTOS-task-notifications.html
 *
 * Internally, within the FreeRTOS implementation, counting semaphores use a
 * block of memory, in which the counting semaphore structure is stored.  If a
 * counting semaphore is created using xSemaphoreCreateCounting() then the
 * required memory is automatically dynamically allocated inside the
 * xSemaphoreCreateCounting() function.  (see
 * http://www.freertos.org/a00111.html).  If a counting semaphore is created
 * using xSemaphoreCreateCountingStatic() then the application writer must
 * provide the memory.  xSemaphoreCreateCountingStatic() therefore allows a
 * counting semaphore to be created without using any dynamic memory allocation.
 *
 * Counting semaphores are typically used for two things:
 *
 * 1) Counting events.
 *
 *    In this usage scenario an event handler will 'give' a semaphore each time
 *    an event occurs (incrementing the semaphore count value), and a handler
 *    task will 'take' a semaphore each time it processes an event
 *    (decrementing the semaphore count value).  The count value is therefore
 *    the difference between the number of events that have occurred and the
 *    number that have been processed.  In this case it is desirable for the
 *    initial count value to be zero.
 *
 * 2) Resource management.
 *
 *    In this usage scenario the count value indicates the number of resources
 *    available.  To obtain control of a resource a task must first obtain a
 *    semaphore - decrementing the semaphore count value.  When the count value
 *    reaches zero there are no free resources.  When a task finishes with the
 *    resource it 'gives' the semaphore back - incrementing the semaphore count
 *    value.  In this case it is desirable for the initial count value to be
 *    equal to the maximum count value, indicating that all resources are free.
 *
 * @param uxMaxCount The maximum count value that can be reached.  When the
 *        semaphore reaches this value it can no longer be 'given'.
 *
 * @param uxInitialCount The count value assigned to the semaphore when it is
 *        created.
 *
 * @param pxSemaphoreBuffer Must point to a variable of type StaticSemaphore_t,
 * which will then be used to hold the semaphore's data structure, removing the
 * need for the memory to be allocated dynamically.
 *
 * @return If the counting semaphore was successfully created then a handle to
 * the created counting semaphore is returned.  If pxSemaphoreBuffer was NULL
 * then NULL is returned.
 *
 * Example usage:
 <pre>
 SemaphoreHandle_t xSemaphore;
 StaticSemaphore_t xSemaphoreBuffer;

 void vATask( void * pvParameters )
 {
 SemaphoreHandle_t xSemaphore = NULL;

    // Counting semaphore cannot be used before they have been created.  Create
    // a counting semaphore using xSemaphoreCreateCountingStatic().  The max
    // value to which the semaphore can count is 10, and the initial value
    // assigned to the count will be 0.  The address of xSemaphoreBuffer is
    // passed in and will be used to hold the semaphore structure, so no dynamic
    // memory allocation will be used.
    xSemaphore = xSemaphoreCreateCounting( 10, 0, &xSemaphoreBuffer );

    // No memory allocation was attempted so xSemaphore cannot be NULL, so there
    // is no need to check its value.
 }
 </pre>
 * \defgroup xSemaphoreCreateCountingStatic xSemaphoreCreateCountingStatic
 * \ingroup Semaphores
 */
/**
 * @def xSemaphoreCreateCountingStatic
 * @brief 静态创建计数信号量（基于预分配内存）
 * @ingroup Semaphores
 * 
 * @param uxMaxCount 信号量最大计数值（资源池容量）
 * @param uxInitialCount 信号量初始可用计数（初始资源数量）
 * @param pxSemaphoreBuffer 指向StaticSemaphore_t类型静态内存缓冲区的指针
 * @return SemaphoreHandle_t 成功返回信号量句柄，失败返回NULL
 * 
 * 该宏实现以下核心机制：
 * 1. 使用用户提供的静态内存初始化计数信号量
 * 2. 避免动态内存分配，确保内存确定性
 * 3. 支持资源池模式的Take/Give操作
 */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )  // 静态内存分配使能检查
    #define xSemaphoreCreateCountingStatic( uxMaxCount, uxInitialCount, pxSemaphoreBuffer ) \
        /* 调用静态计数信号量创建函数 */ \
        /* 参数分解：                                                                               */ \
        /* uxMaxCount        -> 设定信号量最大计数值（必须≥uxInitialCount）                         */ \
        /* uxInitialCount    -> 初始化可用资源数（0 ≤ uxInitialCount ≤ uxMaxCount）                */ \
        /* pxSemaphoreBuffer -> 用户提供的StaticSemaphore_t类型内存块                                */ \
        xQueueCreateCountingSemaphoreStatic( ( uxMaxCount ), ( uxInitialCount ), ( pxSemaphoreBuffer ) )
#endif /* configSUPPORT_STATIC_ALLOCATION */  // 结束静态分配条件编译

/**
 * semphr. h
 * <pre>void vSemaphoreDelete( SemaphoreHandle_t xSemaphore );</pre>
 *
 * Delete a semaphore.  This function must be used with care.  For example,
 * do not delete a mutex type semaphore if the mutex is held by a task.
 *
 * @param xSemaphore A handle to the semaphore to be deleted.
 *
 * \defgroup vSemaphoreDelete vSemaphoreDelete
 * \ingroup Semaphores
 */
/*------------------------ vSemaphoreDelete 宏定义 ------------------------*/
/**
 * @def vSemaphoreDelete
 * @brief 删除信号量并释放相关资源
 * @ingroup Semaphores
 * 
 * @param xSemaphore 要删除的信号量句柄
 * 
 * 关键特性：
 * 1. 只能删除动态创建的信号量（静态创建的使用此宏会导致未定义行为）
 * 2. 执行后信号量句柄立即失效
 * 3. 必须确保删除时没有任务处于阻塞等待状态
 * 
 * 危险操作警告：
 * - 删除正在被任务持有的互斥量会导致资源永久锁死
 * - 删除有任务等待的信号量会导致等待任务永久阻塞
 */
#define vSemaphoreDelete( xSemaphore ) \
    /* 调用队列删除函数 */ \
    /* 类型转换保证队列操作合法性 */ \
    vQueueDelete( ( QueueHandle_t ) ( xSemaphore ) )


/*------------------- xSemaphoreGetMutexHolder 宏定义 --------------------*/
/**
 * @def xSemaphoreGetMutexHolder
 * @brief 获取互斥量当前持有者任务句柄
 * @ingroup Semaphores
 * 
 * @param xSemaphore 互斥量句柄（必须为互斥量类型）
 * @return TaskHandle_t 当前持有任务句柄（未持有返回NULL）
 * 
 * 使用限制：
 * 1. 仅对标准互斥量和递归互斥量有效
 * 2. 返回值为调用时刻的快照，不能保证实时准确性
 * 3. 中断上下文中调用返回NULL
 * 
 * 典型应用场景：
 * - 调试互斥量所有权问题
 * - 实现优先级继承监控
 * - 检测当前任务是否持有锁
 */
#define xSemaphoreGetMutexHolder( xSemaphore ) \
    /* 调用队列互斥量持有者查询函数 */ \
    /* 内部实现通过pxCurrentTCB字段追踪 */ \
    xQueueGetMutexHolder( ( xSemaphore ) )


/*------------------- uxSemaphoreGetCount 宏定义 --------------------*/
/**
 * @def uxSemaphoreGetCount
 * @brief 获取信号量当前可用计数
 * @ingroup Semaphores
 * 
 * @param xSemaphore 信号量句柄
 * @return UBaseType_t 当前可用计数（二进制信号量返回0/1）
 * 
 * 不同信号量类型返回值说明：
 * | 信号量类型       | 返回值含义                   |
 * |----------------|----------------------------|
 * | 二进制信号量     | 0:不可用，1:可用             |
 * | 计数信号量       | 当前可用资源数（0~uxMaxCount）|
 * | 互斥量          | 0:被持有，1:可用（实际不会返回1）|
 * 
 * 注意事项：
 * - 互斥量返回0表示已被锁定（即使被递归持有）
 * - 该操作为非原子操作，返回值可能瞬间过时
 */
#define uxSemaphoreGetCount( xSemaphore ) \
    /* 调用队列消息计数查询函数 */ \
    /* 二进制信号量：队列长度1，项大小0 */ \
    /* 互斥量：队列长度1，项大小0 */ \
    uxQueueMessagesWaiting( ( QueueHandle_t ) ( xSemaphore ) )

#endif /* SEMAPHORE_H */



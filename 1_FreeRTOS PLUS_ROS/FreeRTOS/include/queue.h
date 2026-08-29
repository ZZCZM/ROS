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


#ifndef QUEUE_H
#define QUEUE_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h" must appear in source files before "include queue.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 队列核心类型定义及队列操作宏
 * 
 * 本模块定义FreeRTOS队列系统中使用的核心数据类型和操作宏：
 * - 队列句柄类型定义（队列/队列集合/队列集合成员）
 * - 队列操作方式宏（发送到队尾/队首/覆盖）
 * - 队列类型标识宏（基础队列/互斥锁/信号量等）
 * 这些定义是FreeRTOS队列机制的基础，用于任务间通信和同步控制。
 */

/**
 * @brief 队列句柄类型
 * 
 * 通过xQueueCreate()创建队列时返回的句柄类型，用于后续队列操作：
 * - 发送数据(xQueueSend)
 * - 接收数据(xQueueReceive)
 * - 删除队列(vQueueDelete)等
 * 实质是void指针类型，指向队列结构体
 */
typedef void * QueueHandle_t;

/**
 * @brief 队列集合句柄类型
 * 
 * 通过xQueueCreateSet()创建队列集合时返回的句柄类型，用于：
 * - 队列选择(xQueueSelectFromSet)
 * - 添加到集合(xQueueAddToSet)
 * 用于同时监控多个队列/信号量的场景
 */
typedef void * QueueSetHandle_t;

/**
 * @brief 队列集合成员句柄类型
 * 
 * 队列集合中可以包含队列和信号量，该类型用于表示集合成员：
 * - 可以是QueueHandle_t（队列）
 * - 或SemaphoreHandle_t（信号量）
 * 用于xQueueSelectFromSet()等需要处理多种类型成员的场景
 */
typedef void * QueueSetMemberHandle_t;

/* 队列操作方式宏（仅限内部使用）*/
#define queueSEND_TO_BACK   ( ( BaseType_t ) 0 )  // 默认发送方式，数据插入队尾
#define queueSEND_TO_FRONT  ( ( BaseType_t ) 1 )  // 紧急发送方式，数据插入队首
#define queueOVERWRITE       ( ( BaseType_t ) 2 )  // 覆盖发送方式，当队列满时覆盖最旧数据

/* 队列类型标识宏（必须与queue.c中定义一致）*/
#define queueQUEUE_TYPE_BASE              ( ( uint8_t ) 0U )  // 基础队列类型
#define queueQUEUE_TYPE_SET               ( ( uint8_t ) 0U )  // 队列集合类型（与BASE共享值）
#define queueQUEUE_TYPE_MUTEX             ( ( uint8_t ) 1U )  // 互斥锁类型
#define queueQUEUE_TYPE_COUNTING_SEMAPHORE ( ( uint8_t ) 2U ) // 计数信号量类型
#define queueQUEUE_TYPE_BINARY_SEMAPHORE  ( ( uint8_t ) 3U ) // 二进制信号量类型
#define queueQUEUE_TYPE_RECURSIVE_MUTEX   ( ( uint8_t ) 4U ) // 递归互斥锁类型

/**
 * queue. h
 * <pre>
 QueueHandle_t xQueueCreate(
							  UBaseType_t uxQueueLength,
							  UBaseType_t uxItemSize
						  );
 * </pre>
 *
 * Creates a new queue instance, and returns a handle by which the new queue
 * can be referenced.
 *
 * Internally, within the FreeRTOS implementation, queues use two blocks of
 * memory.  The first block is used to hold the queue's data structures.  The
 * second block is used to hold items placed into the queue.  If a queue is
 * created using xQueueCreate() then both blocks of memory are automatically
 * dynamically allocated inside the xQueueCreate() function.  (see
 * http://www.freertos.org/a00111.html).  If a queue is created using
 * xQueueCreateStatic() then the application writer must provide the memory that
 * will get used by the queue.  xQueueCreateStatic() therefore allows a queue to
 * be created without using any dynamic memory allocation.
 *
 * http://www.FreeRTOS.org/Embedded-RTOS-Queues.html
 *
 * @param uxQueueLength The maximum number of items that the queue can contain.
 *
 * @param uxItemSize The number of bytes each item in the queue will require.
 * Items are queued by copy, not by reference, so this is the number of bytes
 * that will be copied for each posted item.  Each item on the queue must be
 * the same size.
 *
 * @return If the queue is successfully create then a handle to the newly
 * created queue is returned.  If the queue cannot be created then 0 is
 * returned.
 *
 * Example usage:
   <pre>
 struct AMessage
 {
	char ucMessageID;
	char ucData[ 20 ];
 };

 void vATask( void *pvParameters )
 {
 QueueHandle_t xQueue1, xQueue2;

	// Create a queue capable of containing 10 uint32_t values.
	xQueue1 = xQueueCreate( 10, sizeof( uint32_t ) );
	if( xQueue1 == 0 )
	{
		// Queue was not created and must not be used.
	}

	// Create a queue capable of containing 10 pointers to AMessage structures.
	// These should be passed by pointer as they contain a lot of data.
	xQueue2 = xQueueCreate( 10, sizeof( struct AMessage * ) );
	if( xQueue2 == 0 )
	{
		// Queue was not created and must not be used.
	}

	// ... Rest of task code.
 }
 </pre>
 * \defgroup xQueueCreate xQueueCreate
 * \ingroup QueueManagement
 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )  // 动态内存分配启用条件编译
    /**
     * @brief 队列创建宏展开细节
     * 
     * 将xQueueCreate映射到通用队列创建函数，第三个参数固定为queueQUEUE_TYPE_BASE表示基础队列类型：
     * 1. 双重括号包裹参数确保宏展开时的运算优先级
     * 2. 最终调用xQueueGenericCreate执行实际的内存分配和队列初始化
     * 3. 内存从FreeRTOS堆管理器申请，需确保系统堆空间充足
     */
    #define xQueueCreate( uxQueueLength, uxItemSize ) xQueueGenericCreate( ( uxQueueLength ), ( uxItemSize ), ( queueQUEUE_TYPE_BASE ) )
#endif  // 结束动态内存分配条件编译


/**
 * queue. h
 * <pre>
 QueueHandle_t xQueueCreateStatic(
							  UBaseType_t uxQueueLength,
							  UBaseType_t uxItemSize,
							  uint8_t *pucQueueStorageBuffer,
							  StaticQueue_t *pxQueueBuffer
						  );
 * </pre>
 *
 * Creates a new queue instance, and returns a handle by which the new queue
 * can be referenced.
 *
 * Internally, within the FreeRTOS implementation, queues use two blocks of
 * memory.  The first block is used to hold the queue's data structures.  The
 * second block is used to hold items placed into the queue.  If a queue is
 * created using xQueueCreate() then both blocks of memory are automatically
 * dynamically allocated inside the xQueueCreate() function.  (see
 * http://www.freertos.org/a00111.html).  If a queue is created using
 * xQueueCreateStatic() then the application writer must provide the memory that
 * will get used by the queue.  xQueueCreateStatic() therefore allows a queue to
 * be created without using any dynamic memory allocation.
 *
 * http://www.FreeRTOS.org/Embedded-RTOS-Queues.html
 *
 * @param uxQueueLength The maximum number of items that the queue can contain.
 *
 * @param uxItemSize The number of bytes each item in the queue will require.
 * Items are queued by copy, not by reference, so this is the number of bytes
 * that will be copied for each posted item.  Each item on the queue must be
 * the same size.
 *
 * @param pucQueueStorageBuffer If uxItemSize is not zero then
 * pucQueueStorageBuffer must point to a uint8_t array that is at least large
 * enough to hold the maximum number of items that can be in the queue at any
 * one time - which is ( uxQueueLength * uxItemsSize ) bytes.  If uxItemSize is
 * zero then pucQueueStorageBuffer can be NULL.
 *
 * @param pxQueueBuffer Must point to a variable of type StaticQueue_t, which
 * will be used to hold the queue's data structure.
 *
 * @return If the queue is created then a handle to the created queue is
 * returned.  If pxQueueBuffer is NULL then NULL is returned.
 *
 * Example usage:
   <pre>
 struct AMessage
 {
	char ucMessageID;
	char ucData[ 20 ];
 };

 #define QUEUE_LENGTH 10
 #define ITEM_SIZE sizeof( uint32_t )

 // xQueueBuffer will hold the queue structure.
 StaticQueue_t xQueueBuffer;

 // ucQueueStorage will hold the items posted to the queue.  Must be at least
 // [(queue length) * ( queue item size)] bytes long.
 uint8_t ucQueueStorage[ QUEUE_LENGTH * ITEM_SIZE ];

 void vATask( void *pvParameters )
 {
 QueueHandle_t xQueue1;

	// Create a queue capable of containing 10 uint32_t values.
	xQueue1 = xQueueCreate( QUEUE_LENGTH, // The number of items the queue can hold.
							ITEM_SIZE	  // The size of each item in the queue
							&( ucQueueStorage[ 0 ] ), // The buffer that will hold the items in the queue.
							&xQueueBuffer ); // The buffer that will hold the queue structure.

	// The queue is guaranteed to be created successfully as no dynamic memory
	// allocation is used.  Therefore xQueue1 is now a handle to a valid queue.

	// ... Rest of task code.
 }
 </pre>
 * \defgroup xQueueCreateStatic xQueueCreateStatic
 * \ingroup QueueManagement
 */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )  // 静态内存分配启用条件编译
    /**
     * @brief 静态队列创建实现细节
     * 
     * 将xQueueCreateStatic映射到通用静态队列创建函数：
     * 1. 前两个参数与动态版本相同，定义队列容量和元素尺寸
     * 2. pucQueueStorage指向用户分配的存储空间（通常为UC类型数组）
     * 3. pxQueueBuffer指向用户分配的队列控制块（StaticQueue_t类型）
     * 4. 最终参数queueQUEUE_TYPE_BASE标识基础队列类型
     * 
     * @code
     * 使用示例：
     * uint8_t ucQueueStorage[10 * sizeof(Message_t)]; // 10个元素的消息队列
     * StaticQueue_t xQueueBuffer; // 队列控制块
     * QueueHandle_t xQueue = xQueueCreateStatic(10, sizeof(Message_t), ucQueueStorage, &xQueueBuffer);
     * @endcode
     */
    #define xQueueCreateStatic( uxQueueLength, uxItemSize, pucQueueStorage, pxQueueBuffer ) \
        xQueueGenericCreateStatic( ( uxQueueLength ), ( uxItemSize ), ( pucQueueStorage ), \
                                  ( pxQueueBuffer ), ( queueQUEUE_TYPE_BASE ) )
#endif /* configSUPPORT_STATIC_ALLOCATION */  // 结束静态分配条件编译块

/**
 * queue. h
 * <pre>
 BaseType_t xQueueSendToToFront(
								   QueueHandle_t	xQueue,
								   const void		*pvItemToQueue,
								   TickType_t		xTicksToWait
							   );
 * </pre>
 *
 * This is a macro that calls xQueueGenericSend().
 *
 * Post an item to the front of a queue.  The item is queued by copy, not by
 * reference.  This function must not be called from an interrupt service
 * routine.  See xQueueSendFromISR () for an alternative which may be used
 * in an ISR.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param xTicksToWait The maximum amount of time the task should block
 * waiting for space to become available on the queue, should it already
 * be full.  The call will return immediately if this is set to 0 and the
 * queue is full.  The time is defined in tick periods so the constant
 * portTICK_PERIOD_MS should be used to convert to real time if this is required.
 *
 * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
 *
 * Example usage:
   <pre>
 struct AMessage
 {
	char ucMessageID;
	char ucData[ 20 ];
 } xMessage;

 uint32_t ulVar = 10UL;

 void vATask( void *pvParameters )
 {
 QueueHandle_t xQueue1, xQueue2;
 struct AMessage *pxMessage;

	// Create a queue capable of containing 10 uint32_t values.
	xQueue1 = xQueueCreate( 10, sizeof( uint32_t ) );

	// Create a queue capable of containing 10 pointers to AMessage structures.
	// These should be passed by pointer as they contain a lot of data.
	xQueue2 = xQueueCreate( 10, sizeof( struct AMessage * ) );

	// ...

	if( xQueue1 != 0 )
	{
		// Send an uint32_t.  Wait for 10 ticks for space to become
		// available if necessary.
		if( xQueueSendToFront( xQueue1, ( void * ) &ulVar, ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
	}

	if( xQueue2 != 0 )
	{
		// Send a pointer to a struct AMessage object.  Don't block if the
		// queue is already full.
		pxMessage = & xMessage;
		xQueueSendToFront( xQueue2, ( void * ) &pxMessage, ( TickType_t ) 0 );
	}

	// ... Rest of task code.
 }
 </pre>
 * \defgroup xQueueSend xQueueSend
 * \ingroup QueueManagement
 */
#define xQueueSendToFront( xQueue, pvItemToQueue, xTicksToWait )  xQueueGenericSend( \
        ( xQueue ),                   /* 目标队列句柄传递 */ \
        ( pvItemToQueue ),            /* 用户数据指针传递（强制类型转换在内部处理） */ \
        ( xTicksToWait ),             /* 阻塞时间单位转换（ticks->时间） */ \
        queueSEND_TO_FRONT            /* 使用紧急发送模式，插入位置标识为队首 */ \
    )
/**
 * queue. h
 * <pre>
 BaseType_t xQueueSendToBack(
								   QueueHandle_t	xQueue,
								   const void		*pvItemToQueue,
								   TickType_t		xTicksToWait
							   );
 * </pre>
 *
 * This is a macro that calls xQueueGenericSend().
 *
 * Post an item to the back of a queue.  The item is queued by copy, not by
 * reference.  This function must not be called from an interrupt service
 * routine.  See xQueueSendFromISR () for an alternative which may be used
 * in an ISR.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param xTicksToWait The maximum amount of time the task should block
 * waiting for space to become available on the queue, should it already
 * be full.  The call will return immediately if this is set to 0 and the queue
 * is full.  The  time is defined in tick periods so the constant
 * portTICK_PERIOD_MS should be used to convert to real time if this is required.
 *
 * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
 *
 * Example usage:
   <pre>
 struct AMessage
 {
	char ucMessageID;
	char ucData[ 20 ];
 } xMessage;

 uint32_t ulVar = 10UL;

 void vATask( void *pvParameters )
 {
 QueueHandle_t xQueue1, xQueue2;
 struct AMessage *pxMessage;

	// Create a queue capable of containing 10 uint32_t values.
	xQueue1 = xQueueCreate( 10, sizeof( uint32_t ) );

	// Create a queue capable of containing 10 pointers to AMessage structures.
	// These should be passed by pointer as they contain a lot of data.
	xQueue2 = xQueueCreate( 10, sizeof( struct AMessage * ) );

	// ...

	if( xQueue1 != 0 )
	{
		// Send an uint32_t.  Wait for 10 ticks for space to become
		// available if necessary.
		if( xQueueSendToBack( xQueue1, ( void * ) &ulVar, ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
	}

	if( xQueue2 != 0 )
	{
		// Send a pointer to a struct AMessage object.  Don't block if the
		// queue is already full.
		pxMessage = & xMessage;
		xQueueSendToBack( xQueue2, ( void * ) &pxMessage, ( TickType_t ) 0 );
	}

	// ... Rest of task code.
 }
 </pre>
 * \defgroup xQueueSend xQueueSend
 * \ingroup QueueManagement
 */
#define xQueueSendToBack( xQueue, pvItemToQueue, xTicksToWait )  xQueueGenericSend( \
        ( xQueue ),                   /* 目标队列控制块指针传递 */ \
        ( pvItemToQueue ),            /* 用户数据地址传递（执行memcpy操作） */ \
        ( xTicksToWait ),             /* 时间参数转换为系统心跳基数 */ \
        queueSEND_TO_BACK             /* 标准发送模式标识（值为0） */ \
    )  // 注：实际队列实现可能为环形缓冲区结构
/**
 * queue. h
 * <pre>
 BaseType_t xQueueSend(
							  QueueHandle_t xQueue,
							  const void * pvItemToQueue,
							  TickType_t xTicksToWait
						 );
 * </pre>
 *
 * This is a macro that calls xQueueGenericSend().  It is included for
 * backward compatibility with versions of FreeRTOS.org that did not
 * include the xQueueSendToFront() and xQueueSendToBack() macros.  It is
 * equivalent to xQueueSendToBack().
 *
 * Post an item on a queue.  The item is queued by copy, not by reference.
 * This function must not be called from an interrupt service routine.
 * See xQueueSendFromISR () for an alternative which may be used in an ISR.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param xTicksToWait The maximum amount of time the task should block
 * waiting for space to become available on the queue, should it already
 * be full.  The call will return immediately if this is set to 0 and the
 * queue is full.  The time is defined in tick periods so the constant
 * portTICK_PERIOD_MS should be used to convert to real time if this is required.
 *
 * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
 *
 * Example usage:
   <pre>
 struct AMessage
 {
	char ucMessageID;
	char ucData[ 20 ];
 } xMessage;

 uint32_t ulVar = 10UL;

 void vATask( void *pvParameters )
 {
 QueueHandle_t xQueue1, xQueue2;
 struct AMessage *pxMessage;

	// Create a queue capable of containing 10 uint32_t values.
	xQueue1 = xQueueCreate( 10, sizeof( uint32_t ) );

	// Create a queue capable of containing 10 pointers to AMessage structures.
	// These should be passed by pointer as they contain a lot of data.
	xQueue2 = xQueueCreate( 10, sizeof( struct AMessage * ) );

	// ...

	if( xQueue1 != 0 )
	{
		// Send an uint32_t.  Wait for 10 ticks for space to become
		// available if necessary.
		if( xQueueSend( xQueue1, ( void * ) &ulVar, ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
	}

	if( xQueue2 != 0 )
	{
		// Send a pointer to a struct AMessage object.  Don't block if the
		// queue is already full.
		pxMessage = & xMessage;
		xQueueSend( xQueue2, ( void * ) &pxMessage, ( TickType_t ) 0 );
	}

	// ... Rest of task code.
 }
 </pre>
 * \defgroup xQueueSend xQueueSend
 * \ingroup QueueManagement
 */
#define xQueueSend( xQueue, pvItemToQueue, xTicksToWait )     xQueueGenericSend( \
        ( xQueue ),                   /* 传递队列控制块指针 */ \
        ( pvItemToQueue ),            /* 数据源地址（强制const修饰符） */ \
        ( xTicksToWait ),             /* 时间参数转换为心跳单位 */ \
        queueSEND_TO_BACK             /* 隐含使用默认发送模式 */ \
    )  // 注：该宏自FreeRTOS V1.0起存在，保持向后兼容性

/**
 * queue. h
 * <pre>
 BaseType_t xQueueOverwrite(
							  QueueHandle_t xQueue,
							  const void * pvItemToQueue
						 );
 * </pre>
 *
 * Only for use with queues that have a length of one - so the queue is either
 * empty or full.
 *
 * Post an item on a queue.  If the queue is already full then overwrite the
 * value held in the queue.  The item is queued by copy, not by reference.
 *
 * This function must not be called from an interrupt service routine.
 * See xQueueOverwriteFromISR () for an alternative which may be used in an ISR.
 *
 * @param xQueue The handle of the queue to which the data is being sent.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @return xQueueOverwrite() is a macro that calls xQueueGenericSend(), and
 * therefore has the same return values as xQueueSendToFront().  However, pdPASS
 * is the only value that can be returned because xQueueOverwrite() will write
 * to the queue even when the queue is already full.
 *
 * Example usage:
   <pre>

 void vFunction( void *pvParameters )
 {
 QueueHandle_t xQueue;
 uint32_t ulVarToSend, ulValReceived;

	// Create a queue to hold one uint32_t value.  It is strongly
	// recommended *not* to use xQueueOverwrite() on queues that can
	// contain more than one value, and doing so will trigger an assertion
	// if configASSERT() is defined.
	xQueue = xQueueCreate( 1, sizeof( uint32_t ) );

	// Write the value 10 to the queue using xQueueOverwrite().
	ulVarToSend = 10;
	xQueueOverwrite( xQueue, &ulVarToSend );

	// Peeking the queue should now return 10, but leave the value 10 in
	// the queue.  A block time of zero is used as it is known that the
	// queue holds a value.
	ulValReceived = 0;
	xQueuePeek( xQueue, &ulValReceived, 0 );

	if( ulValReceived != 10 )
	{
		// Error unless the item was removed by a different task.
	}

	// The queue is still full.  Use xQueueOverwrite() to overwrite the
	// value held in the queue with 100.
	ulVarToSend = 100;
	xQueueOverwrite( xQueue, &ulVarToSend );

	// This time read from the queue, leaving the queue empty once more.
	// A block time of 0 is used again.
	xQueueReceive( xQueue, &ulValReceived, 0 );

	// The value read should be the last value written, even though the
	// queue was already full when the value was written.
	if( ulValReceived != 100 )
	{
		// Error!
	}

	// ...
}
 </pre>
 * \defgroup xQueueOverwrite xQueueOverwrite
 * \ingroup QueueManagement
 */
#define xQueueOverwrite( xQueue, pvItemToQueue ) xQueueGenericSend( \
        ( xQueue ),                   /* 目标队列控制块地址传递 */ \
        ( pvItemToQueue ),            /* 数据源地址（自动去const限定） */ \
        0,                            /* 硬编码等待时间为0（强制非阻塞） */ \
        queueOVERWRITE                /* 覆盖模式标识（值为2） */ \
    )  // 注：该操作原子性由调度器挂起保证

/**
 * queue. h
 * <pre>
 BaseType_t xQueueGenericSend(
									QueueHandle_t xQueue,
									const void * pvItemToQueue,
									TickType_t xTicksToWait
									BaseType_t xCopyPosition
								);
 * </pre>
 *
 * It is preferred that the macros xQueueSend(), xQueueSendToFront() and
 * xQueueSendToBack() are used in place of calling this function directly.
 *
 * Post an item on a queue.  The item is queued by copy, not by reference.
 * This function must not be called from an interrupt service routine.
 * See xQueueSendFromISR () for an alternative which may be used in an ISR.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param xTicksToWait The maximum amount of time the task should block
 * waiting for space to become available on the queue, should it already
 * be full.  The call will return immediately if this is set to 0 and the
 * queue is full.  The time is defined in tick periods so the constant
 * portTICK_PERIOD_MS should be used to convert to real time if this is required.
 *
 * @param xCopyPosition Can take the value queueSEND_TO_BACK to place the
 * item at the back of the queue, or queueSEND_TO_FRONT to place the item
 * at the front of the queue (for high priority messages).
 *
 * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
 *
 * Example usage:
   <pre>
 struct AMessage
 {
	char ucMessageID;
	char ucData[ 20 ];
 } xMessage;

 uint32_t ulVar = 10UL;

 void vATask( void *pvParameters )
 {
 QueueHandle_t xQueue1, xQueue2;
 struct AMessage *pxMessage;

	// Create a queue capable of containing 10 uint32_t values.
	xQueue1 = xQueueCreate( 10, sizeof( uint32_t ) );

	// Create a queue capable of containing 10 pointers to AMessage structures.
	// These should be passed by pointer as they contain a lot of data.
	xQueue2 = xQueueCreate( 10, sizeof( struct AMessage * ) );

	// ...

	if( xQueue1 != 0 )
	{
		// Send an uint32_t.  Wait for 10 ticks for space to become
		// available if necessary.
		if( xQueueGenericSend( xQueue1, ( void * ) &ulVar, ( TickType_t ) 10, queueSEND_TO_BACK ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
	}

	if( xQueue2 != 0 )
	{
		// Send a pointer to a struct AMessage object.  Don't block if the
		// queue is already full.
		pxMessage = & xMessage;
		xQueueGenericSend( xQueue2, ( void * ) &pxMessage, ( TickType_t ) 0, queueSEND_TO_BACK );
	}

	// ... Rest of task code.
 }
 </pre>
 * \defgroup xQueueSend xQueueSend
 * \ingroup QueueManagement
 */
BaseType_t xQueueGenericSend( 
    QueueHandle_t xQueue,                   /* 队列控制块指针，包含队列状态信息 */
    const void * const pvItemToQueue,       /* 只读数据源指针（强制const限定） */
    TickType_t xTicksToWait,                /* 超时参数转换为系统心跳基数 */
    const BaseType_t xCopyPosition          /* 写入模式标识（0/1/2） */ 
) PRIVILEGED_FUNCTION;  /* 特权模式函数，要求运行在系统权限级别 */

/* 典型调用流程：
   1. 检查队列有效性
   2. 进入临界区
   3. 检查队列剩余空间：
      - 有空闲：写入数据
      - 队列满：
        a. 覆盖模式：替换最旧数据
        b. 其他模式：挂起任务等待
   4. 退出临界区
   5. 返回操作结果 */
/**
 * queue. h
 * <pre>
 BaseType_t xQueuePeek(
							 QueueHandle_t xQueue,
							 void *pvBuffer,
							 TickType_t xTicksToWait
						 );</pre>
 *
 * This is a macro that calls the xQueueGenericReceive() function.
 *
 * Receive an item from a queue without removing the item from the queue.
 * The item is received by copy so a buffer of adequate size must be
 * provided.  The number of bytes copied into the buffer was defined when
 * the queue was created.
 *
 * Successfully received items remain on the queue so will be returned again
 * by the next call, or a call to xQueueReceive().
 *
 * This macro must not be used in an interrupt service routine.  See
 * xQueuePeekFromISR() for an alternative that can be called from an interrupt
 * service routine.
 *
 * @param xQueue The handle to the queue from which the item is to be
 * received.
 *
 * @param pvBuffer Pointer to the buffer into which the received item will
 * be copied.
 *
 * @param xTicksToWait The maximum amount of time the task should block
 * waiting for an item to receive should the queue be empty at the time
 * of the call.	 The time is defined in tick periods so the constant
 * portTICK_PERIOD_MS should be used to convert to real time if this is required.
 * xQueuePeek() will return immediately if xTicksToWait is 0 and the queue
 * is empty.
 *
 * @return pdTRUE if an item was successfully received from the queue,
 * otherwise pdFALSE.
 *
 * Example usage:
   <pre>
 struct AMessage
 {
	char ucMessageID;
	char ucData[ 20 ];
 } xMessage;

 QueueHandle_t xQueue;

 // Task to create a queue and post a value.
 void vATask( void *pvParameters )
 {
 struct AMessage *pxMessage;

	// Create a queue capable of containing 10 pointers to AMessage structures.
	// These should be passed by pointer as they contain a lot of data.
	xQueue = xQueueCreate( 10, sizeof( struct AMessage * ) );
	if( xQueue == 0 )
	{
		// Failed to create the queue.
	}

	// ...

	// Send a pointer to a struct AMessage object.  Don't block if the
	// queue is already full.
	pxMessage = & xMessage;
	xQueueSend( xQueue, ( void * ) &pxMessage, ( TickType_t ) 0 );

	// ... Rest of task code.
 }

 // Task to peek the data from the queue.
 void vADifferentTask( void *pvParameters )
 {
 struct AMessage *pxRxedMessage;

	if( xQueue != 0 )
	{
		// Peek a message on the created queue.  Block for 10 ticks if a
		// message is not immediately available.
		if( xQueuePeek( xQueue, &( pxRxedMessage ), ( TickType_t ) 10 ) )
		{
			// pcRxedMessage now points to the struct AMessage variable posted
			// by vATask, but the item still remains on the queue.
		}
	}

	// ... Rest of task code.
 }
 </pre>
 * \defgroup xQueueReceive xQueueReceive
 * \ingroup QueueManagement
 */
#define xQueuePeek( xQueue, pvBuffer, xTicksToWait ) xQueueGenericReceive( \
        ( xQueue ),                   /* 目标队列控制块地址 */ \
        ( pvBuffer ),                 /* 用户提供的接收缓冲区 */ \
        ( xTicksToWait ),             /* 时间参数转换为心跳单位 */ \
        pdTRUE                        /* 保留标志（不删除数据项） */ \
    )  // 注：该操作会增加uxMessagesWaiting计数器的读取锁

/* 典型应用场景：
   1. 监控系统状态队列，观察最新数据而不消耗
   2. 调试时检查队列内容
   3. 数据预处理决策（根据队列头数据决定后续操作） */

/**
 * queue. h
 * <pre>
 BaseType_t xQueuePeekFromISR(
									QueueHandle_t xQueue,
									void *pvBuffer,
								);</pre>
 *
 * A version of xQueuePeek() that can be called from an interrupt service
 * routine (ISR).
 *
 * Receive an item from a queue without removing the item from the queue.
 * The item is received by copy so a buffer of adequate size must be
 * provided.  The number of bytes copied into the buffer was defined when
 * the queue was created.
 *
 * Successfully received items remain on the queue so will be returned again
 * by the next call, or a call to xQueueReceive().
 *
 * @param xQueue The handle to the queue from which the item is to be
 * received.
 *
 * @param pvBuffer Pointer to the buffer into which the received item will
 * be copied.
 *
 * @return pdTRUE if an item was successfully received from the queue,
 * otherwise pdFALSE.
 *
 * \defgroup xQueuePeekFromISR xQueuePeekFromISR
 * \ingroup QueueManagement
 */
/**
 * @brief 中断安全队列查看函数（非破坏性读取）
 * 
 * 在中断上下文(ISR)中查看队列头部数据项，保持队列数据完整。
 * 专为中断环境优化，无阻塞机制，确保中断响应实时性。
 * 
 * @param xQueue 目标队列句柄（必须已初始化且非空）
 * @param pvBuffer 接收缓冲区指针（需预分配且尺寸匹配队列项）
 * @return 操作状态：
 *         - pdTRUE 数据查看成功
 *         - pdFALSE 队列空或参数无效
 * 
 * @note 该函数执行时间具有确定性（无分支等待）
 * @warning 严格限定在ISR内调用，禁止在任务上下文使用
 * @see xQueuePeek 任务上下文的查看接口
 */
BaseType_t xQueuePeekFromISR( 
    QueueHandle_t xQueue,                   /* 队列控制块指针（带类型校验） */
    void * const pvBuffer                   /* 输出缓冲区（执行memcpy操作） */ 
) PRIVILEGED_FUNCTION;  /* 特权模式函数，需系统级内存访问权限 */

/* 实现特性：
   1. 关闭中断实现原子操作（portDISABLE_INTERRUPTS）
   2. 不修改uxMessagesWaiting计数器
   3. 不触发任务解除阻塞
   4. 无优先级继承机制 */
/**
 * queue. h
 * <pre>
 BaseType_t xQueueReceive(
								 QueueHandle_t xQueue,
								 void *pvBuffer,
								 TickType_t xTicksToWait
							);</pre>
 *
 * This is a macro that calls the xQueueGenericReceive() function.
 *
 * Receive an item from a queue.  The item is received by copy so a buffer of
 * adequate size must be provided.  The number of bytes copied into the buffer
 * was defined when the queue was created.
 *
 * Successfully received items are removed from the queue.
 *
 * This function must not be used in an interrupt service routine.  See
 * xQueueReceiveFromISR for an alternative that can.
 *
 * @param xQueue The handle to the queue from which the item is to be
 * received.
 *
 * @param pvBuffer Pointer to the buffer into which the received item will
 * be copied.
 *
 * @param xTicksToWait The maximum amount of time the task should block
 * waiting for an item to receive should the queue be empty at the time
 * of the call.	 xQueueReceive() will return immediately if xTicksToWait
 * is zero and the queue is empty.  The time is defined in tick periods so the
 * constant portTICK_PERIOD_MS should be used to convert to real time if this is
 * required.
 *
 * @return pdTRUE if an item was successfully received from the queue,
 * otherwise pdFALSE.
 *
 * Example usage:
   <pre>
 struct AMessage
 {
	char ucMessageID;
	char ucData[ 20 ];
 } xMessage;

 QueueHandle_t xQueue;

 // Task to create a queue and post a value.
 void vATask( void *pvParameters )
 {
 struct AMessage *pxMessage;

	// Create a queue capable of containing 10 pointers to AMessage structures.
	// These should be passed by pointer as they contain a lot of data.
	xQueue = xQueueCreate( 10, sizeof( struct AMessage * ) );
	if( xQueue == 0 )
	{
		// Failed to create the queue.
	}

	// ...

	// Send a pointer to a struct AMessage object.  Don't block if the
	// queue is already full.
	pxMessage = & xMessage;
	xQueueSend( xQueue, ( void * ) &pxMessage, ( TickType_t ) 0 );

	// ... Rest of task code.
 }

 // Task to receive from the queue.
 void vADifferentTask( void *pvParameters )
 {
 struct AMessage *pxRxedMessage;

	if( xQueue != 0 )
	{
		// Receive a message on the created queue.  Block for 10 ticks if a
		// message is not immediately available.
		if( xQueueReceive( xQueue, &( pxRxedMessage ), ( TickType_t ) 10 ) )
		{
			// pcRxedMessage now points to the struct AMessage variable posted
			// by vATask.
		}
	}

	// ... Rest of task code.
 }
 </pre>
 * \defgroup xQueueReceive xQueueReceive
 * \ingroup QueueManagement
 */
#define xQueueReceive( xQueue, pvBuffer, xTicksToWait )     xQueueGenericReceive( \
        ( xQueue ),                   /* 队列控制块指针传递 */ \
        ( pvBuffer ),                 /* 输出缓冲区地址传递 */ \
        ( xTicksToWait ),             /* 时间参数转换为心跳基数 */ \
        pdFALSE                       /* 移除标志（删除数据项） */ \
    )  // 注：该操作会递减uxMessagesWaiting计数器

/* 典型工作流程：
   1. 检查队列有效性
   2. 进入临界区
   3. 队列非空时：
      a. 复制头部数据到pvBuffer
      b. 移动队列头指针
      c. 如果有发送阻塞任务，唤醒最高优先级任务
   4. 队列空时挂起当前任务（根据等待策略）
   5. 退出临界区 */


/**
 * queue. h
 * <pre>
 BaseType_t xQueueGenericReceive(
									   QueueHandle_t	xQueue,
									   void	*pvBuffer,
									   TickType_t	xTicksToWait
									   BaseType_t	xJustPeek
									);</pre>
 *
 * It is preferred that the macro xQueueReceive() be used rather than calling
 * this function directly.
 *
 * Receive an item from a queue.  The item is received by copy so a buffer of
 * adequate size must be provided.  The number of bytes copied into the buffer
 * was defined when the queue was created.
 *
 * This function must not be used in an interrupt service routine.  See
 * xQueueReceiveFromISR for an alternative that can.
 *
 * @param xQueue The handle to the queue from which the item is to be
 * received.
 *
 * @param pvBuffer Pointer to the buffer into which the received item will
 * be copied.
 *
 * @param xTicksToWait The maximum amount of time the task should block
 * waiting for an item to receive should the queue be empty at the time
 * of the call.	 The time is defined in tick periods so the constant
 * portTICK_PERIOD_MS should be used to convert to real time if this is required.
 * xQueueGenericReceive() will return immediately if the queue is empty and
 * xTicksToWait is 0.
 *
 * @param xJustPeek When set to true, the item received from the queue is not
 * actually removed from the queue - meaning a subsequent call to
 * xQueueReceive() will return the same item.  When set to false, the item
 * being received from the queue is also removed from the queue.
 *
 * @return pdTRUE if an item was successfully received from the queue,
 * otherwise pdFALSE.
 *
 * Example usage:
   <pre>
 struct AMessage
 {
	char ucMessageID;
	char ucData[ 20 ];
 } xMessage;

 QueueHandle_t xQueue;

 // Task to create a queue and post a value.
 void vATask( void *pvParameters )
 {
 struct AMessage *pxMessage;

	// Create a queue capable of containing 10 pointers to AMessage structures.
	// These should be passed by pointer as they contain a lot of data.
	xQueue = xQueueCreate( 10, sizeof( struct AMessage * ) );
	if( xQueue == 0 )
	{
		// Failed to create the queue.
	}

	// ...

	// Send a pointer to a struct AMessage object.  Don't block if the
	// queue is already full.
	pxMessage = & xMessage;
	xQueueSend( xQueue, ( void * ) &pxMessage, ( TickType_t ) 0 );

	// ... Rest of task code.
 }

 // Task to receive from the queue.
 void vADifferentTask( void *pvParameters )
 {
 struct AMessage *pxRxedMessage;

	if( xQueue != 0 )
	{
		// Receive a message on the created queue.  Block for 10 ticks if a
		// message is not immediately available.
		if( xQueueGenericReceive( xQueue, &( pxRxedMessage ), ( TickType_t ) 10 ) )
		{
			// pcRxedMessage now points to the struct AMessage variable posted
			// by vATask.
		}
	}

	// ... Rest of task code.
 }
 </pre>
 * \defgroup xQueueReceive xQueueReceive
 * \ingroup QueueManagement
 */
/**
 * @brief 队列通用接收函数（底层实现）
 * 
 * 实现队列数据接收的核心逻辑，支持移除/保留两种模式。
 * 被xQueueReceive和xQueuePeek宏调用，通常不直接使用。
 * 
 * @param xQueue 目标队列句柄
 * @param pvBuffer 接收缓冲区（需预分配）
 * @param xTicksToWait 等待时间（单位：心跳节拍）
 * @param xJustPeek 操作模式：
 *                 - pdTRUE：仅查看不删除
 *                 - pdFALSE：接收并删除
 * @return 操作结果：
 *         - pdTRUE 操作成功
 *         - pdFALSE 队列空且超时
 * 
 * @note 执行过程可能引发任务调度
 * @warning 需在特权模式下运行（PRIVILEGED_FUNCTION）
 */
BaseType_t xQueueGenericReceive( QueueHandle_t xQueue, void * const pvBuffer, 
                                TickType_t xTicksToWait, const BaseType_t xJustPeek ) PRIVILEGED_FUNCTION;

/**
 * @brief 获取队列当前消息数量
 * 
 * 查询队列中现存的数据项数量，用于监控队列负载。
 * 
 * @param xQueue 目标队列句柄
 * @return 当前队列中的消息数量（0到uxQueueLength）
 * 
 * @note 该值具有瞬时性，可能被并发操作改变
 * @warning 在中断中使用需配合FromISR版本
 */
UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;

/**
 * @brief 获取队列剩余空间
 * 
 * 计算队列剩余可存储的数据项数量，用于流量控制。
 * 
 * @param xQueue 目标队列句柄
 * @return 剩余可用空间（0表示队列满）
 * 
 * @note 返回值=uxQueueLength - uxQueueMessagesWaiting
 * @warning 不适用于动态调整队列长度的场景
 */
UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;

/**
 * @brief 删除队列对象
 * 
 * 释放队列相关资源，包括存储区和控制块。
 * 
 * @param xQueue 待删除队列句柄
 * 
 * @note 删除后所有关联操作将失效
 * @warning 必须确保没有任务等待该队列
 * @see xQueueCreate 创建队列的对应操作
 */
void vQueueDelete( QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;


/**
 * queue. h
 * <pre>
 BaseType_t xQueueSendToFrontFromISR(
										 QueueHandle_t xQueue,
										 const void *pvItemToQueue,
										 BaseType_t *pxHigherPriorityTaskWoken
									  );
 </pre>
 *
 * This is a macro that calls xQueueGenericSendFromISR().
 *
 * Post an item to the front of a queue.  It is safe to use this macro from
 * within an interrupt service routine.
 *
 * Items are queued by copy not reference so it is preferable to only
 * queue small items, especially when called from an ISR.  In most cases
 * it would be preferable to store a pointer to the item being queued.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param pxHigherPriorityTaskWoken xQueueSendToFrontFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending to the queue caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xQueueSendToFromFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 *
 * @return pdTRUE if the data was successfully sent to the queue, otherwise
 * errQUEUE_FULL.
 *
 * Example usage for buffered IO (where the ISR can obtain more than one value
 * per call):
   <pre>
 void vBufferISR( void )
 {
 char cIn;
 BaseType_t xHigherPrioritTaskWoken;

	// We have not woken a task at the start of the ISR.
	xHigherPriorityTaskWoken = pdFALSE;

	// Loop until the buffer is empty.
	do
	{
		// Obtain a byte from the buffer.
		cIn = portINPUT_BYTE( RX_REGISTER_ADDRESS );

		// Post the byte.
		xQueueSendToFrontFromISR( xRxQueue, &cIn, &xHigherPriorityTaskWoken );

	} while( portINPUT_BYTE( BUFFER_COUNT ) );

	// Now the buffer is empty we can switch context if necessary.
	if( xHigherPriorityTaskWoken )
	{
		taskYIELD ();
	}
 }
 </pre>
 *
 * \defgroup xQueueSendFromISR xQueueSendFromISR
 * \ingroup QueueManagement
 */
#define xQueueSendToFrontFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken )     xQueueGenericSendFromISR( \
        ( xQueue ),                   /* 队列控制块指针（带类型校验） */ \
        ( pvItemToQueue ),            /* 数据源地址（强制const限定） */ \
        ( pxHigherPriorityTaskWoken ),/* 任务唤醒状态标志位指针 */ \
        queueSEND_TO_FRONT            /* 紧急发送模式标识（值为1） */ \
    )  // 注：该操作保持中断响应时间确定性



/**
 * queue. h
 * <pre>
 BaseType_t xQueueSendToBackFromISR(
										 QueueHandle_t xQueue,
										 const void *pvItemToQueue,
										 BaseType_t *pxHigherPriorityTaskWoken
									  );
 </pre>
 *
 * This is a macro that calls xQueueGenericSendFromISR().
 *
 * Post an item to the back of a queue.  It is safe to use this macro from
 * within an interrupt service routine.
 *
 * Items are queued by copy not reference so it is preferable to only
 * queue small items, especially when called from an ISR.  In most cases
 * it would be preferable to store a pointer to the item being queued.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param pxHigherPriorityTaskWoken xQueueSendToBackFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending to the queue caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xQueueSendToBackFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 *
 * @return pdTRUE if the data was successfully sent to the queue, otherwise
 * errQUEUE_FULL.
 *
 * Example usage for buffered IO (where the ISR can obtain more than one value
 * per call):
   <pre>
 void vBufferISR( void )
 {
 char cIn;
 BaseType_t xHigherPriorityTaskWoken;

	// We have not woken a task at the start of the ISR.
	xHigherPriorityTaskWoken = pdFALSE;

	// Loop until the buffer is empty.
	do
	{
		// Obtain a byte from the buffer.
		cIn = portINPUT_BYTE( RX_REGISTER_ADDRESS );

		// Post the byte.
		xQueueSendToBackFromISR( xRxQueue, &cIn, &xHigherPriorityTaskWoken );

	} while( portINPUT_BYTE( BUFFER_COUNT ) );

	// Now the buffer is empty we can switch context if necessary.
	if( xHigherPriorityTaskWoken )
	{
		taskYIELD ();
	}
 }
 </pre>
 *
 * \defgroup xQueueSendFromISR xQueueSendFromISR
 * \ingroup QueueManagement
 */
#define xQueueSendToBackFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( \
        ( xQueue ),                   /* 队列控制块指针（带访问权限校验） */ \
        ( pvItemToQueue ),            /* 数据源地址（自动去除const限定） */ \
        ( pxHigherPriorityTaskWoken ),/* 出参任务唤醒标志（按位操作修改） */ \
        queueSEND_TO_BACK             /* 标准发送模式标识（值为0） */ \
    )  // 注：该操作平均耗时约为xQueueSendToFrontFromISR的85%


/**
 * queue. h
 * <pre>
 BaseType_t xQueueOverwriteFromISR(
							  QueueHandle_t xQueue,
							  const void * pvItemToQueue,
							  BaseType_t *pxHigherPriorityTaskWoken
						 );
 * </pre>
 *
 * A version of xQueueOverwrite() that can be used in an interrupt service
 * routine (ISR).
 *
 * Only for use with queues that can hold a single item - so the queue is either
 * empty or full.
 *
 * Post an item on a queue.  If the queue is already full then overwrite the
 * value held in the queue.  The item is queued by copy, not by reference.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param pxHigherPriorityTaskWoken xQueueOverwriteFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending to the queue caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xQueueOverwriteFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 *
 * @return xQueueOverwriteFromISR() is a macro that calls
 * xQueueGenericSendFromISR(), and therefore has the same return values as
 * xQueueSendToFrontFromISR().  However, pdPASS is the only value that can be
 * returned because xQueueOverwriteFromISR() will write to the queue even when
 * the queue is already full.
 *
 * Example usage:
   <pre>

 QueueHandle_t xQueue;

 void vFunction( void *pvParameters )
 {
 	// Create a queue to hold one uint32_t value.  It is strongly
	// recommended *not* to use xQueueOverwriteFromISR() on queues that can
	// contain more than one value, and doing so will trigger an assertion
	// if configASSERT() is defined.
	xQueue = xQueueCreate( 1, sizeof( uint32_t ) );
}

void vAnInterruptHandler( void )
{
// xHigherPriorityTaskWoken must be set to pdFALSE before it is used.
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
uint32_t ulVarToSend, ulValReceived;

	// Write the value 10 to the queue using xQueueOverwriteFromISR().
	ulVarToSend = 10;
	xQueueOverwriteFromISR( xQueue, &ulVarToSend, &xHigherPriorityTaskWoken );

	// The queue is full, but calling xQueueOverwriteFromISR() again will still
	// pass because the value held in the queue will be overwritten with the
	// new value.
	ulVarToSend = 100;
	xQueueOverwriteFromISR( xQueue, &ulVarToSend, &xHigherPriorityTaskWoken );

	// Reading from the queue will now return 100.

	// ...

	if( xHigherPrioritytaskWoken == pdTRUE )
	{
		// Writing to the queue caused a task to unblock and the unblocked task
		// has a priority higher than or equal to the priority of the currently
		// executing task (the task this interrupt interrupted).  Perform a context
		// switch so this interrupt returns directly to the unblocked task.
		portYIELD_FROM_ISR(); // or portEND_SWITCHING_ISR() depending on the port.
	}
}
 </pre>
 * \defgroup xQueueOverwriteFromISR xQueueOverwriteFromISR
 * \ingroup QueueManagement
 */
#define xQueueOverwriteFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( \
        ( xQueue ),                   /* 队列控制块指针（带有效性校验） */ \
        ( pvItemToQueue ),            /* 数据源地址（强制类型擦除） */ \
        ( pxHigherPriorityTaskWoken ),/* 出参任务唤醒标志（原子修改） */ \
        queueOVERWRITE                /* 覆盖模式标识（值为2） */ \
    )  // 注：该操作时间复杂度稳定为O(1)
/**
 * queue. h
 * <pre>
 BaseType_t xQueueSendFromISR(
									 QueueHandle_t xQueue,
									 const void *pvItemToQueue,
									 BaseType_t *pxHigherPriorityTaskWoken
								);
 </pre>
 *
 * This is a macro that calls xQueueGenericSendFromISR().  It is included
 * for backward compatibility with versions of FreeRTOS.org that did not
 * include the xQueueSendToBackFromISR() and xQueueSendToFrontFromISR()
 * macros.
 *
 * Post an item to the back of a queue.  It is safe to use this function from
 * within an interrupt service routine.
 *
 * Items are queued by copy not reference so it is preferable to only
 * queue small items, especially when called from an ISR.  In most cases
 * it would be preferable to store a pointer to the item being queued.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param pxHigherPriorityTaskWoken xQueueSendFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending to the queue caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xQueueSendFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 *
 * @return pdTRUE if the data was successfully sent to the queue, otherwise
 * errQUEUE_FULL.
 *
 * Example usage for buffered IO (where the ISR can obtain more than one value
 * per call):
   <pre>
 void vBufferISR( void )
 {
 char cIn;
 BaseType_t xHigherPriorityTaskWoken;

	// We have not woken a task at the start of the ISR.
	xHigherPriorityTaskWoken = pdFALSE;

	// Loop until the buffer is empty.
	do
	{
		// Obtain a byte from the buffer.
		cIn = portINPUT_BYTE( RX_REGISTER_ADDRESS );

		// Post the byte.
		xQueueSendFromISR( xRxQueue, &cIn, &xHigherPriorityTaskWoken );

	} while( portINPUT_BYTE( BUFFER_COUNT ) );

	// Now the buffer is empty we can switch context if necessary.
	if( xHigherPriorityTaskWoken )
	{
		// Actual macro used here is port specific.
		portYIELD_FROM_ISR ();
	}
 }
 </pre>
 *
 * \defgroup xQueueSendFromISR xQueueSendFromISR
 * \ingroup QueueManagement
 */
#define xQueueSendFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( \
        ( xQueue ),                   /* 队列控制块指针（带访问权限校验） */ \
        ( pvItemToQueue ),            /* 数据源地址（自动去除const限定） */ \
        ( pxHigherPriorityTaskWoken ),/* 出参任务唤醒标志（按位或操作更新） */ \
        queueSEND_TO_BACK             /* 标准发送模式标识（值为0） */ \
    )  // 注：自FreeRTOS V8.0起引入的兼容性命名

/* 典型应用示例：
void vCAN_ISR_Handler(void) {
    CANFrame_t xFrame;
    if( CAN_Receive(&xFrame) ) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if( xQueueSendFromISR(xCANQueue, &xFrame, &xHigherPriorityTaskWoken) == pdFALSE ) {
            // 处理队列满的情况（可选记录错误计数器）
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
} */

/**
 * queue. h
 * <pre>
 BaseType_t xQueueGenericSendFromISR(
										   QueueHandle_t		xQueue,
										   const	void	*pvItemToQueue,
										   BaseType_t	*pxHigherPriorityTaskWoken,
										   BaseType_t	xCopyPosition
									   );
 </pre>
 *
 * It is preferred that the macros xQueueSendFromISR(),
 * xQueueSendToFrontFromISR() and xQueueSendToBackFromISR() be used in place
 * of calling this function directly.  xQueueGiveFromISR() is an
 * equivalent for use by semaphores that don't actually copy any data.
 *
 * Post an item on a queue.  It is safe to use this function from within an
 * interrupt service routine.
 *
 * Items are queued by copy not reference so it is preferable to only
 * queue small items, especially when called from an ISR.  In most cases
 * it would be preferable to store a pointer to the item being queued.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param pxHigherPriorityTaskWoken xQueueGenericSendFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending to the queue caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xQueueGenericSendFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 *
 * @param xCopyPosition Can take the value queueSEND_TO_BACK to place the
 * item at the back of the queue, or queueSEND_TO_FRONT to place the item
 * at the front of the queue (for high priority messages).
 *
 * @return pdTRUE if the data was successfully sent to the queue, otherwise
 * errQUEUE_FULL.
 *
 * Example usage for buffered IO (where the ISR can obtain more than one value
 * per call):
   <pre>
 void vBufferISR( void )
 {
 char cIn;
 BaseType_t xHigherPriorityTaskWokenByPost;

	// We have not woken a task at the start of the ISR.
	xHigherPriorityTaskWokenByPost = pdFALSE;

	// Loop until the buffer is empty.
	do
	{
		// Obtain a byte from the buffer.
		cIn = portINPUT_BYTE( RX_REGISTER_ADDRESS );

		// Post each byte.
		xQueueGenericSendFromISR( xRxQueue, &cIn, &xHigherPriorityTaskWokenByPost, queueSEND_TO_BACK );

	} while( portINPUT_BYTE( BUFFER_COUNT ) );

	// Now the buffer is empty we can switch context if necessary.  Note that the
	// name of the yield function required is port specific.
	if( xHigherPriorityTaskWokenByPost )
	{
		taskYIELD_YIELD_FROM_ISR();
	}
 }
 </pre>
 *
 * \defgroup xQueueSendFromISR xQueueSendFromISR
 * \ingroup QueueManagement
 */
/**
 * @brief 中断安全队列通用发送函数（底层实现）
 * 
 * 中断级队列操作核心函数，支持三种发送模式，被各类SendFromISR宏调用。
 * 
 * @param xQueue 目标队列/信号量句柄（必须有效）
 * @param pvItemToQueue 待发送数据指针（信号量操作时设为NULL）
 * @param pxHigherPriorityTaskWoken 任务唤醒标志指针：
 *                                  - 非NULL：可能被修改为pdTRUE
 *                                  - NULL：忽略唤醒状态
 * @param xCopyPosition 发送模式标识：
 *                     - queueSEND_TO_FRONT（1） 队首插入
 *                     - queueSEND_TO_BACK（0） 队尾插入
 *                     - queueOVERWRITE（2） 覆盖写入
 * @return 操作状态：
 *         - pdTRUE 发送成功
 *         - errQUEUE_FULL 队列满且非覆盖模式
 * 
 * @note 该函数直接操作队列控制块，需确保中断优先级配置正确
 */
BaseType_t xQueueGenericSendFromISR( 
    QueueHandle_t xQueue,                   /* 队列控制块指针（带类型校验） */
    const void * const pvItemToQueue,       /* 数据指针（信号量操作时为NULL） */
    BaseType_t * const pxHigherPriorityTaskWoken, /* 唤醒标志（按位或更新） */
    const BaseType_t xCopyPosition           /* 发送模式枚举值 */
) PRIVILEGED_FUNCTION;  /* 需系统特权级访问权限 */

/**
 * @brief 中断安全信号量释放函数
 * 
 * 专门用于释放二进制信号量/互斥量，等效于发送空消息的简化接口。
 * 
 * @param xQueue 目标信号量/互斥量句柄（必须为信号量类型）
 * @param pxHigherPriorityTaskWoken 任务唤醒标志指针（同发送函数）
 * @return 操作状态：
 *         - pdTRUE 释放成功
 *         - errQUEUE_FULL 信号量已满（计数达最大值）
 * 
 * @note 对互斥量执行give操作等效于释放锁
 * @warning 禁止用于非信号量类型的队列
 */
BaseType_t xQueueGiveFromISR( 
    QueueHandle_t xQueue,                   /* 信号量控制块指针 */
    BaseType_t * const pxHigherPriorityTaskWoken /* 唤醒标志指针 */
) PRIVILEGED_FUNCTION;  /* 需访问内核对象特权 */

/**
 * queue. h
 * <pre>
 BaseType_t xQueueReceiveFromISR(
									   QueueHandle_t	xQueue,
									   void	*pvBuffer,
									   BaseType_t *pxTaskWoken
								   );
 * </pre>
 *
 * Receive an item from a queue.  It is safe to use this function from within an
 * interrupt service routine.
 *
 * @param xQueue The handle to the queue from which the item is to be
 * received.
 *
 * @param pvBuffer Pointer to the buffer into which the received item will
 * be copied.
 *
 * @param pxTaskWoken A task may be blocked waiting for space to become
 * available on the queue.  If xQueueReceiveFromISR causes such a task to
 * unblock *pxTaskWoken will get set to pdTRUE, otherwise *pxTaskWoken will
 * remain unchanged.
 *
 * @return pdTRUE if an item was successfully received from the queue,
 * otherwise pdFALSE.
 *
 * Example usage:
   <pre>

 QueueHandle_t xQueue;

 // Function to create a queue and post some values.
 void vAFunction( void *pvParameters )
 {
 char cValueToPost;
 const TickType_t xTicksToWait = ( TickType_t )0xff;

	// Create a queue capable of containing 10 characters.
	xQueue = xQueueCreate( 10, sizeof( char ) );
	if( xQueue == 0 )
	{
		// Failed to create the queue.
	}

	// ...

	// Post some characters that will be used within an ISR.  If the queue
	// is full then this task will block for xTicksToWait ticks.
	cValueToPost = 'a';
	xQueueSend( xQueue, ( void * ) &cValueToPost, xTicksToWait );
	cValueToPost = 'b';
	xQueueSend( xQueue, ( void * ) &cValueToPost, xTicksToWait );

	// ... keep posting characters ... this task may block when the queue
	// becomes full.

	cValueToPost = 'c';
	xQueueSend( xQueue, ( void * ) &cValueToPost, xTicksToWait );
 }

 // ISR that outputs all the characters received on the queue.
 void vISR_Routine( void )
 {
 BaseType_t xTaskWokenByReceive = pdFALSE;
 char cRxedChar;

	while( xQueueReceiveFromISR( xQueue, ( void * ) &cRxedChar, &xTaskWokenByReceive) )
	{
		// A character was received.  Output the character now.
		vOutputCharacter( cRxedChar );

		// If removing the character from the queue woke the task that was
		// posting onto the queue cTaskWokenByReceive will have been set to
		// pdTRUE.  No matter how many times this loop iterates only one
		// task will be woken.
	}

	if( cTaskWokenByPost != ( char ) pdFALSE;
	{
		taskYIELD ();
	}
 }
 </pre>
 * \defgroup xQueueReceiveFromISR xQueueReceiveFromISR
 * \ingroup QueueManagement
 */
/*------------------------ 中断级队列操作函数 ------------------------*/

/**
 * @brief 中断安全队列接收函数（数据移除）
 * 
 * 在中断上下文中从队列头部提取数据项并移除，用于ISR环境下的数据消费。
 * 
 * @param xQueue 目标队列句柄（必须已初始化）
 * @param pvBuffer 接收缓冲区指针（需预分配且尺寸匹配）
 * @param pxHigherPriorityTaskWoken 任务唤醒标志指针：
 *                                  - 非NULL：可能被修改为pdTRUE
 *                                  - NULL：忽略唤醒状态
 * @return 操作状态：
 *         - pdTRUE 接收成功
 *         - pdFALSE 队列空
 * 
 * @note 必须通过portYIELD_FROM_ISR()处理任务唤醒
 * @warning 严格限定在中断上下文使用，禁止嵌套调用
 */
BaseType_t xQueueReceiveFromISR( QueueHandle_t xQueue, void * const pvBuffer, 
                                BaseType_t * const pxHigherPriorityTaskWoken ) PRIVILEGED_FUNCTION;

/*------------------------ 中断级队列状态查询 ------------------------*/

/**
 * @brief 中断安全队列空状态查询
 * 
 * 在中断上下文中判断队列是否为空，无副作用的只读操作。
 * 
 * @param xQueue 目标队列句柄
 * @return 队列状态：
 *         - pdTRUE 队列空
 *         - pdFALSE 队列非空
 * 
 * @note 该操作不进入临界区，结果具有瞬时性
 */
BaseType_t xQueueIsQueueEmptyFromISR( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;

/**
 * @brief 中断安全队列满状态查询
 * 
 * 在中断上下文中判断队列是否已满，用于流量控制。
 * 
 * @param xQueue 目标队列句柄
 * @return 队列状态：
 *         - pdTRUE 队列满
 *         - pdFALSE 队列未满
 */
BaseType_t xQueueIsQueueFullFromISR( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;

/**
 * @brief 中断安全队列消息计数
 * 
 * 在中断上下文中获取队列当前存储的消息数量。
 * 
 * @param xQueue 目标队列句柄
 * @return 消息数量（0到uxQueueLength）
 * 
 * @note 结果可能因并发操作立即失效
 */
UBaseType_t uxQueueMessagesWaitingFromISR( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;

/*------------------------ 协程专用队列操作（禁止直接调用） ------------------------*/

/**
 * @brief 协程中断安全发送（内部使用）
 * @warning 必须通过crQUEUE_SEND_FROM_ISR宏调用，禁止直接使用
 */
BaseType_t xQueueCRSendFromISR( QueueHandle_t xQueue, const void *pvItemToQueue, 
                              BaseType_t xCoRoutinePreviouslyWoken );

/**
 * @brief 协程中断安全接收（内部使用）
 * @warning 必须通过crQUEUE_RECEIVE_FROM_ISR宏调用，禁止直接使用
 */
BaseType_t xQueueCRReceiveFromISR( QueueHandle_t xQueue, void *pvBuffer, 
                                 BaseType_t *pxTaskWoken );

/*------------------------ 信号量创建底层函数 ------------------------*/

/**
 * @brief 创建互斥量队列（内部实现）
 * 
 * 信号量创建函数的底层实现，应使用xSemaphoreCreateMutex()替代。
 * 
 * @param ucQueueType 队列类型标识符：
 *                   - queueQUEUE_TYPE_MUTEX
 *                   - queueQUEUE_TYPE_RECURSIVE_MUTEX
 * @return 互斥量句柄（失败返回NULL）
 * 
 * @warning 禁止应用层直接调用，需通过信号量API使用
 */
QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType ) PRIVILEGED_FUNCTION;

/**
 * @brief 创建计数信号量队列（内部实现）
 * 
 * @param uxMaxCount 最大计数值
 * @param uxInitialCount 初始计数值
 * @return 信号量句柄（失败返回NULL）
 * 
 * @warning 应使用xSemaphoreCreateCounting()替代
 */
QueueHandle_t xQueueCreateCountingSemaphore( const UBaseType_t uxMaxCount,
                                            const UBaseType_t uxInitialCount ) PRIVILEGED_FUNCTION;
/*------------------------ 互斥量递归操作（内部使用） ------------------------*/

/**
 * @brief 递归获取互斥量（底层实现）
 * @param xMutex 互斥量句柄（必须为递归类型）
 * @param xTicksToWait 最大等待时间（单位：心跳节拍）
 * @return 操作状态：
 *         - pdTRUE 获取成功
 *         - pdFALSE 超时或参数错误
 * 
 * @warning 必须通过xSemaphoreTakeMutexRecursive()调用，禁止直接使用
 * @note 支持嵌套获取，需对应相同次数的释放
 */
BaseType_t xQueueTakeMutexRecursive( QueueHandle_t xMutex, 
                                    TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

/**
 * @brief 递归释放互斥量（底层实现）
 * @param pxMutex 互斥量句柄（必须为当前任务持有）
 * @return 操作状态：
 *         - pdTRUE 释放成功
 *         - pdFALSE 非持有者尝试释放
 * 
 * @warning 必须通过xSemaphoreGiveMutexRecursive()调用
 */
BaseType_t xQueueGiveMutexRecursive( QueueHandle_t pxMutex ) PRIVILEGED_FUNCTION;

/*------------------------ 队列重置宏 ------------------------*/

/**
 * @brief 队列重置宏（清空内容）
 * @param xQueue 目标队列句柄
 * @return 固定返回pdPASS（历史遗留兼容）
 * 
 * @warning 必须在队列空时调用，否则导致数据丢失
 * @note 实际调用xQueueGenericReset( xQueue, pdFALSE )
 */
#define xQueueReset( xQueue ) xQueueGenericReset( xQueue, pdFALSE )

/*
 * The registry is provided as a means for kernel aware debuggers to
 * locate queues, semaphores and mutexes.  Call vQueueAddToRegistry() add
 * a queue, semaphore or mutex handle to the registry if you want the handle
 * to be available to a kernel aware debugger.  If you are not using a kernel
 * aware debugger then this function can be ignored.
 *
 * configQUEUE_REGISTRY_SIZE defines the maximum number of handles the
 * registry can hold.  configQUEUE_REGISTRY_SIZE must be greater than 0
 * within FreeRTOSConfig.h for the registry to be available.  Its value
 * does not effect the number of queues, semaphores and mutexes that can be
 * created - just the number that the registry can hold.
 *
 * @param xQueue The handle of the queue being added to the registry.  This
 * is the handle returned by a call to xQueueCreate().  Semaphore and mutex
 * handles can also be passed in here.
 *
 * @param pcName The name to be associated with the handle.  This is the
 * name that the kernel aware debugger will display.  The queue registry only
 * stores a pointer to the string - so the string must be persistent (global or
 * preferably in ROM/Flash), not on the stack.
 */

/*------------------------ 队列注册表操作 ------------------------*/
#if( configQUEUE_REGISTRY_SIZE > 0 )

/**
 * @brief 添加队列到注册表（调试支持）
 * @param xQueue 队列/信号量/互斥量句柄
 * @param pcName 注册名称（须为持久字符串）
 * 
 * @note 名称最大长度由configQUEUE_REGISTRY_SIZE决定
 * @warning 名称字符串必须存在于全局/常量存储区
 * 
 * 使用示例：
 * vQueueAddToRegistry(xUartQueue, "UART_Tx_Queue");
 */
void vQueueAddToRegistry( QueueHandle_t xQueue, 
                         const char *pcName ) PRIVILEGED_FUNCTION;

/**
 * @brief 从注册表移除队列
 * @param xQueue 已注册的队列句柄
 * 
 * @note 删除队列前必须调用此函数解除注册
 */
void vQueueUnregisterQueue( QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;

/**
 * @brief 查询队列注册名称
 * @param xQueue 已注册的队列句柄
 * @return 注册名称指针（未注册返回NULL）
 * 
 * 调试示例：
 * printf("Queue name: %s", pcQueueGetName(xQueue));
 */
const char *pcQueueGetName( QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;

#endif /* configQUEUE_REGISTRY_SIZE > 0 */
/*------------------------ 队列通用创建函数（底层实现） ------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
/**
 * @brief 动态内存队列创建（核心实现）
 * 
 * 队列系统核心构造函数，用于动态内存分配场景。
 * 被xQueueCreate/xSemaphoreCreate等函数内部调用。
 * 
 * @param uxQueueLength 队列容量（最大存储项数）
 * @param uxItemSize 单个队列项大小（字节）
 * @param ucQueueType 队列类型标识：
 *                   - queueQUEUE_TYPE_BASE       (0x00) 基础队列
 *                   - queueQUEUE_TYPE_MUTEX      (0x01) 互斥量
 *                   - queueQUEUE_TYPE_SET        (0x02) 队列集
 *                   - queueQUEUE_TYPE_COUNTING_SEMAPHORE (0x03) 计数信号量
 * @return 队列控制句柄（失败返回NULL）
 * 
 * @note 从FreeRTOS堆分配内存，需确保堆空间充足
 * @warning 禁止直接调用，应通过标准API创建队列对象
 */
QueueHandle_t xQueueGenericCreate( 
    const UBaseType_t uxQueueLength,    /* 队列深度（必须>0） */
    const UBaseType_t uxItemSize,      /* 项尺寸（0表示无数据存储） */
    const uint8_t ucQueueType           /* 类型标识符（参见queue.h） */
) PRIVILEGED_FUNCTION;
#endif

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
/**
 * @brief 静态内存队列创建（核心实现）
 * 
 * 队列系统核心构造函数，用于静态内存分配场景。
 * 需用户预分配存储内存和控制块。
 * 
 * @param uxQueueLength 队列容量（最大存储项数）
 * @param uxItemSize 单个队列项大小（字节）
 * @param pucQueueStorage 队列存储区指针（尺寸=uxQueueLength*uxItemSize）
 * @param pxStaticQueue 静态队列控制块指针
 * @param ucQueueType 队列类型标识符（同动态版本）
 * @return 队列控制句柄（失败返回NULL）
 * 
 * @warning pucQueueStorage需4字节对齐（ARM架构要求）
 * @example 
 * // 预分配资源
 * static uint8_t ucStorage[10*sizeof(Item_t)];
 * static StaticQueue_t xQueueControlBlock;
 * xQueueCreateStatic(10, sizeof(Item_t), ucStorage, &xQueueControlBlock);
 */
QueueHandle_t xQueueGenericCreateStatic( 
    const UBaseType_t uxQueueLength,    /* 队列深度（必须>0） */
    const UBaseType_t uxItemSize,      /* 项尺寸（0用于信号量） */
    uint8_t *pucQueueStorage,          /* 存储区（须持久有效） */
    StaticQueue_t *pxStaticQueue,       /* 静态控制块指针 */
    const uint8_t ucQueueType           /* 类型标识符 */
) PRIVILEGED_FUNCTION;
#endif

/*
 * Queue sets provide a mechanism to allow a task to block (pend) on a read
 * operation from multiple queues or semaphores simultaneously.
 *
 * See FreeRTOS/Source/Demo/Common/Minimal/QueueSet.c for an example using this
 * function.
 *
 * A queue set must be explicitly created using a call to xQueueCreateSet()
 * before it can be used.  Once created, standard FreeRTOS queues and semaphores
 * can be added to the set using calls to xQueueAddToSet().
 * xQueueSelectFromSet() is then used to determine which, if any, of the queues
 * or semaphores contained in the set is in a state where a queue read or
 * semaphore take operation would be successful.
 *
 * Note 1:  See the documentation on http://wwwFreeRTOS.org/RTOS-queue-sets.html
 * for reasons why queue sets are very rarely needed in practice as there are
 * simpler methods of blocking on multiple objects.
 *
 * Note 2:  Blocking on a queue set that contains a mutex will not cause the
 * mutex holder to inherit the priority of the blocked task.
 *
 * Note 3:  An additional 4 bytes of RAM is required for each space in a every
 * queue added to a queue set.  Therefore counting semaphores that have a high
 * maximum count value should not be added to a queue set.
 *
 * Note 4:  A receive (in the case of a queue) or take (in the case of a
 * semaphore) operation must not be performed on a member of a queue set unless
 * a call to xQueueSelectFromSet() has first returned a handle to that set member.
 *
 * @param uxEventQueueLength Queue sets store events that occur on
 * the queues and semaphores contained in the set.  uxEventQueueLength specifies
 * the maximum number of events that can be queued at once.  To be absolutely
 * certain that events are not lost uxEventQueueLength should be set to the
 * total sum of the length of the queues added to the set, where binary
 * semaphores and mutexes have a length of 1, and counting semaphores have a
 * length set by their maximum count value.  Examples:
 *  + If a queue set is to hold a queue of length 5, another queue of length 12,
 *    and a binary semaphore, then uxEventQueueLength should be set to
 *    (5 + 12 + 1), or 18.
 *  + If a queue set is to hold three binary semaphores then uxEventQueueLength
 *    should be set to (1 + 1 + 1 ), or 3.
 *  + If a queue set is to hold a counting semaphore that has a maximum count of
 *    5, and a counting semaphore that has a maximum count of 3, then
 *    uxEventQueueLength should be set to (5 + 3), or 8.
 *
 * @return If the queue set is created successfully then a handle to the created
 * queue set is returned.  Otherwise NULL is returned.
 */
QueueSetHandle_t xQueueCreateSet( const UBaseType_t uxEventQueueLength ) PRIVILEGED_FUNCTION;

/*
 * Adds a queue or semaphore to a queue set that was previously created by a
 * call to xQueueCreateSet().
 *
 * See FreeRTOS/Source/Demo/Common/Minimal/QueueSet.c for an example using this
 * function.
 *
 * Note 1:  A receive (in the case of a queue) or take (in the case of a
 * semaphore) operation must not be performed on a member of a queue set unless
 * a call to xQueueSelectFromSet() has first returned a handle to that set member.
 *
 * @param xQueueOrSemaphore The handle of the queue or semaphore being added to
 * the queue set (cast to an QueueSetMemberHandle_t type).
 *
 * @param xQueueSet The handle of the queue set to which the queue or semaphore
 * is being added.
 *
 * @return If the queue or semaphore was successfully added to the queue set
 * then pdPASS is returned.  If the queue could not be successfully added to the
 * queue set because it is already a member of a different queue set then pdFAIL
 * is returned.
 */
BaseType_t xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet ) PRIVILEGED_FUNCTION;

/**
 * @brief 创建队列集合
 * 
 * 初始化一个队列集合容器，用于同时监控多个队列/信号量。
 * 需配合xQueueAddToSet将成员加入集合后使用。
 * 
 * @param uxEventQueueLength 保留参数（历史兼容性，建议设为1）
 * @return 队列集合句柄（失败返回NULL）
 * 
 * @note 实际队列长度由添加成员数量动态决定
 * @warning 过时功能，建议优先使用事件组(event groups)
 * @example
 * // 创建队列集合
 * QueueSetHandle_t xSet = xQueueCreateSet(1);
 */
QueueSetHandle_t xQueueCreateSet( const UBaseType_t uxEventQueueLength ) PRIVILEGED_FUNCTION;

/**
 * @brief 添加成员到队列集合
 * 
 * 将队列或信号量加入队列集合，使其状态可被统一监控。
 * 
 * @param xQueueOrSemaphore 队列/信号量句柄（需强制转换为QueueSetMemberHandle_t类型）
 * @param xQueueSet 目标队列集合句柄
 * @return 操作状态：
 *         - pdPASS 添加成功
 *         - pdFAIL 添加失败（通常因成员已属其他集合）
 * 
 * @note 成员加入后无法单独设置等待时间
 * @warning 每个队列/信号量只能属于一个集合
 * @example
 * // 将队列添加到集合
 * xQueueAddToSet( (QueueSetMemberHandle_t)xQueue1, xSet );
 * // 将信号量添加到集合
 * xQueueAddToSet( (QueueSetMemberHandle_t)xSemaphore, xSet );
 */
BaseType_t xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, 
                          QueueSetHandle_t xQueueSet ) PRIVILEGED_FUNCTION;

/*------------------------ 队列集合操作 ------------------------*/

/**
 * @brief 从队列集合移除成员
 * @param xQueueOrSemaphore 队列/信号量句柄（必须已加入集合）
 * @param xQueueSet 目标队列集合句柄
 * @return 操作状态：
 *         - pdPASS 移除成功
 *         - pdFAIL 成员不存在或队列非空
 * 
 * @warning 必须在成员空闲时操作（无等待任务）
 * @note 典型应用场景：动态调整监控队列
 */
BaseType_t xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore, 
                               QueueSetHandle_t xQueueSet ) PRIVILEGED_FUNCTION;
/*
 * xQueueSelectFromSet() selects from the members of a queue set a queue or
 * semaphore that either contains data (in the case of a queue) or is available
 * to take (in the case of a semaphore).  xQueueSelectFromSet() effectively
 * allows a task to block (pend) on a read operation on all the queues and
 * semaphores in a queue set simultaneously.
 *
 * See FreeRTOS/Source/Demo/Common/Minimal/QueueSet.c for an example using this
 * function.
 *
 * Note 1:  See the documentation on http://wwwFreeRTOS.org/RTOS-queue-sets.html
 * for reasons why queue sets are very rarely needed in practice as there are
 * simpler methods of blocking on multiple objects.
 *
 * Note 2:  Blocking on a queue set that contains a mutex will not cause the
 * mutex holder to inherit the priority of the blocked task.
 *
 * Note 3:  A receive (in the case of a queue) or take (in the case of a
 * semaphore) operation must not be performed on a member of a queue set unless
 * a call to xQueueSelectFromSet() has first returned a handle to that set member.
 *
 * @param xQueueSet The queue set on which the task will (potentially) block.
 *
 * @param xTicksToWait The maximum time, in ticks, that the calling task will
 * remain in the Blocked state (with other tasks executing) to wait for a member
 * of the queue set to be ready for a successful queue read or semaphore take
 * operation.
 *
 * @return xQueueSelectFromSet() will return the handle of a queue (cast to
 * a QueueSetMemberHandle_t type) contained in the queue set that contains data,
 * or the handle of a semaphore (cast to a QueueSetMemberHandle_t type) contained
 * in the queue set that is available, or NULL if no such queue or semaphore
 * exists before before the specified block time expires.
 */

/**
 * @brief 队列集合选择就绪成员（任务级）
 * @param xQueueSet 队列集合句柄
 * @param xTicksToWait 最大等待时间（单位：心跳节拍）
 * @return 就绪成员句柄（超时返回NULL）
 * 
 * @note 就绪条件：
 * - 队列：有数据可读
 * - 信号量：可获取
 * 
 * 使用示例：
 * QueueSetMemberHandle_t xActivated = xQueueSelectFromSet(xSet, pdMS_TO_TICKS(100));
 * if(xActivated == xQueue1) {  处理队列1数据  }
 */
QueueSetMemberHandle_t xQueueSelectFromSet( QueueSetHandle_t xQueueSet,\
                                          const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

/**
 * @brief 队列集合选择就绪成员（中断级）
 * @param xQueueSet 队列集合句柄
 * @return 就绪成员句柄（无就绪返回NULL）
 * 
 * @warning 必须搭配portYIELD_FROM_ISR()使用
 */
QueueSetMemberHandle_t xQueueSelectFromSetFromISR( QueueSetHandle_t xQueueSet ) PRIVILEGED_FUNCTION;

/*------------------------ 内部使用函数（禁止直接调用） ------------------------*/

/**
 * @brief 受限队列等待（内部调试用）
 * @param xQueue 目标队列句柄
 * @param xTicksToWait 最大等待时间
 * @param xWaitIndefinitely 是否无限等待：
 *                         - pdTRUE: 忽略xTicksToWait
 *                         - pdFALSE: 使用xTicksToWait
 * 
 * @warning 仅限FreeRTOS内核调试使用
 */
void vQueueWaitForMessageRestricted( QueueHandle_t xQueue, \
                                    TickType_t xTicksToWait,\
                                    const BaseType_t xWaitIndefinitely ) PRIVILEGED_FUNCTION;

/**
 * @brief 通用队列重置（内部实现）
 * @param xQueue 目标队列句柄
 * @param xNewQueue 是否初始化新队列：
 *                 - pdTRUE: 初始化新队列结构
 *                 - pdFALSE: 保留队列配置
 * @return 固定返回pdPASS（历史兼容）
 * 
 * @note 实际被xQueueReset宏调用
 */
BaseType_t xQueueGenericReset( QueueHandle_t xQueue, \
                             BaseType_t xNewQueue ) PRIVILEGED_FUNCTION;

/*------------------------ 队列元数据操作 ------------------------*/

/**
 * @brief 设置队列编号（跟踪用）
 * @param xQueue 目标队列句柄
 * @param uxQueueNumber 自定义编号（0~0xFFFF）
 * 
 * @note 用于多队列管理系统的标识
 */
void vQueueSetQueueNumber( QueueHandle_t xQueue,\
                         UBaseType_t uxQueueNumber ) PRIVILEGED_FUNCTION;

/**
 * @brief 获取队列编号
 * @param xQueue 目标队列句柄
 * @return 预设的队列编号（默认0）
 * 
 * @see vQueueSetQueueNumber
 */
UBaseType_t uxQueueGetQueueNumber( QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;

/**
 * @brief 获取队列类型标识
 * @param xQueue 目标队列句柄
 * @return 类型编码（参见queueQUEUE_TYPE_*常量）
 * 
 * 使用示例：
 * if(ucQueueGetQueueType(xHandle) == queueQUEUE_TYPE_MUTEX) {
 *      互斥量特殊处理 
 * }
 */
uint8_t ucQueueGetQueueType( QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;

#ifdef __cplusplus
}
#endif

#endif /* QUEUE_H */



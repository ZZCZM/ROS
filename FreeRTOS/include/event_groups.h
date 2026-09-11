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

#ifndef EVENT_GROUPS_H
#define EVENT_GROUPS_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h" must appear in source files before "include event_groups.h"
#endif

/* FreeRTOS includes. */
#include "timers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An event group is a collection of bits to which an application can assign a
 * meaning.  For example, an application may create an event group to convey
 * the status of various CAN bus related events in which bit 0 might mean "A CAN
 * message has been received and is ready for processing", bit 1 might mean "The
 * application has queued a message that is ready for sending onto the CAN
 * network", and bit 2 might mean "It is time to send a SYNC message onto the
 * CAN network" etc.  A task can then test the bit values to see which events
 * are active, and optionally enter the Blocked state to wait for a specified
 * bit or a group of specified bits to be active.  To continue the CAN bus
 * example, a CAN controlling task can enter the Blocked state (and therefore
 * not consume any processing time) until either bit 0, bit 1 or bit 2 are
 * active, at which time the bit that was actually active would inform the task
 * which action it had to take (process a received message, send a message, or
 * send a SYNC).
 *
 * The event groups implementation contains intelligence to avoid race
 * conditions that would otherwise occur were an application to use a simple
 * variable for the same purpose.  This is particularly important with respect
 * to when a bit within an event group is to be cleared, and when bits have to
 * be set and then tested atomically - as is the case where event groups are
 * used to create a synchronisation point between multiple tasks (a
 * 'rendezvous').
 *
 * \defgroup EventGroup
 */
/* 事件组核心类型定义模块 */
/**
 * @功能概述 本代码段定义事件组机制的核心数据类型，包括：
 *          1. 事件组句柄类型
 *          2. 事件位存储类型
 * @设计要点
 *  - 通过不透明指针实现类型封装
 *  - 事件位存储与系统时钟类型绑定
 *  - 支持16/32位灵活配置
 */

/**
 * @brief 事件组句柄类型（不透明指针）
 * @描述
 * - 通过xEventGroupCreate()动态创建返回的句柄
 * - 作为所有事件组操作的入口参数
 * - 实际指向内部管理结构，应用层不直接访问
 * @使用示例
 * EventGroupHandle_t xEventGroup = xEventGroupCreate();
 * xEventGroupSetBits(xEventGroup, 0x01);
 * @注意 禁止直接解引用操作，仅通过API函数访问
 * @ingroup EventGroup
 */
typedef void * EventGroupHandle_t;

/**
 * @brief 事件位存储类型（位掩码容器）
 * @特性
 * - 实际位宽由configUSE_16_BIT_TICKS决定：
 *   - 1: 16位（0-0xFFFF） 
 *   - 0: 32位（0-0xFFFFFFFF）
 * - 直接映射系统时钟类型TickType_t
 * @使用场景
 * - 事件标志设置/清除：xEventGroupSetBits()
 * - 事件等待检查：xEventGroupWaitBits()
 * @配置建议
 * - 16位：资源受限设备，需节省内存
 * - 32位：复杂事件系统，需更多事件标志
 * @警告 跨位宽配置编译时需重新初始化事件组
 * @ingroup EventGroup
 */
typedef TickType_t EventBits_t;



/**
 * \ingroup Tasks
 * \defgroup crQUEUE_RECEIVE_FROM_ISR crQUEUE_RECEIVE_FROM_ISR
 * @brief 中断上下文队列接收宏（协程唤醒）
 * 
 * 功能：在中断服务例程(ISR)中实现协程队列接收，支持协程唤醒
 * 
 * 作用：
 * 1. 中断安全地从队列获取数据
 * 2. 唤醒等待该队列的生产者协程
 * 3. 返回协程唤醒状态用于后续调度决策
 * 
 * @param pxQueue [in] 目标队列句柄
 *        - 要求：必须使用xQueueCreate创建
 * @param pvBuffer [out] 数据接收缓冲区指针
 *        - 必须指向有效内存区域
 * @param pxCoRoutineWoken [in/out] 协程唤醒标志指针
 *        - 输入值：pdFALSE(初始)/pdTRUE(已唤醒)
 *        - 返回值：pdTRUE表示需要触发调度
 * 
 * 实现原理：
 * 直接调用协程专用的中断安全接收API：
 * xQueueCRReceiveFromISR -> 队列操作 + 协程等待列表检查
 * 
 * 典型应用：
 * BaseType_t xYieldRequired = pdFALSE;
 * crQUEUE_RECEIVE_FROM_ISR(pxQueue, &data, &xYieldRequired);
 * portYIELD_FROM_ISR( xYieldRequired );
 * 
 * 设计特点：
 * 1. 无阻塞机制：符合ISR即时响应要求
 * 2. 优先级继承：唤醒最高优先级等待协程
 * 3. 内存安全：使用临界区保护数据访问
 */
#define crQUEUE_RECEIVE_FROM_ISR( pxQueue, pvBuffer, pxCoRoutineWoken ) \
    xQueueCRReceiveFromISR( ( pxQueue ), ( pvBuffer ), ( pxCoRoutineWoken ) )

/**
 * @brief 协程延时列表管理函数（内部API）
 * 
 * 功能：将当前协程移出就绪队列，插入延时/事件队列
 * 
 * 作用：
 * 1. 管理协程的时间阻塞（crDELAY类操作）
 * 2. 处理事件驱动阻塞（crQUEUE_SEND/RECEIVE类操作）
 * 
 * @param xTicksToDelay [in] 延时时间基数
 *        - 单位：系统节拍(configTICK_RATE_HZ)
 *        - 特殊值：portMAX_DELAY表示无限等待
 * @param pxEventList [in] 事件等待队列指针
 *        - NULL：插入延时队列
 *        - 非NULL：插入指定事件等待队列
 * 
 * 实现机制：
 * 1. 计算唤醒时间：xTickCount + xTicksToDelay
 * 2. 更新协程状态为BLOCKED
 * 3. 从就绪列表移除，插入目标队列
 * 
 * 重要约束：
 * 1. 仅供协程内核实现使用
 * 2. 必须在调度器启动后调用
 * 3. 需在临界区内执行
 */
void vCoRoutineAddToDelayedList( TickType_t xTicksToDelay, List_t *pxEventList );

/**
 * @brief 事件队列处理函数（内部API）
 * 
 * 功能：从事件队列中唤醒最高优先级协程
 * 
 * 作用：
 * 1. 扫描事件等待队列
 * 2. 选择最高优先级协程
 * 3. 将协程移回就绪队列
 * 
 * @param pxEventList [in] 事件队列指针
 * @return 唤醒结果
 *        - pdTRUE：需要立即触发调度
 *        - pdFALSE：无优先级高于当前运行的协程
 * 
 * 实现机制：
 * 1. 使用uxPriority字段进行优先级排序
 * 2. 通过pxIndex遍历链表寻找最高优先级
 * 3. 更新调度器优先级位图
 * 
 * 典型应用场景：
 * - 队列数据到达时唤醒接收协程
 * - 信号量给出时唤醒等待协程
 * 
 * 性能优化：
 * 使用TAILQ优先队列结构实现O(1)时间复杂度
 */
BaseType_t xCoRoutineRemoveFromEventList( const List_t *pxEventList );

/* 技术架构说明：
 * 1. 延时队列管理：
 *    - 全局pxDelayedCoRoutineList列表
 *    - 按唤醒时间升序排列
 *    - 系统节拍中断中自动检测超时
 * 
 * 2. 事件队列管理：
 *    - 每个队列对象维护等待列表
 *    - 按协程优先级降序排列
 *    - 数据到达/空间释放时触发唤醒
 * 
 * 3. 状态转换关系：
 *    [就绪态] <-> [延时阻塞态] <-> [事件阻塞态]
 */


/**
 * event_groups.h
 *<pre>
 EventGroupHandle_t xEventGroupCreate( void );
 </pre>
 *
 * Create a new event group.
 *
 * Internally, within the FreeRTOS implementation, event groups use a [small]
 * block of memory, in which the event group's structure is stored.  If an event
 * groups is created using xEventGropuCreate() then the required memory is
 * automatically dynamically allocated inside the xEventGroupCreate() function.
 * (see http://www.freertos.org/a00111.html).  If an event group is created
 * using xEventGropuCreateStatic() then the application writer must instead
 * provide the memory that will get used by the event group.
 * xEventGroupCreateStatic() therefore allows an event group to be created
 * without using any dynamic memory allocation.
 *
 * Although event groups are not related to ticks, for internal implementation
 * reasons the number of bits available for use in an event group is dependent
 * on the configUSE_16_BIT_TICKS setting in FreeRTOSConfig.h.  If
 * configUSE_16_BIT_TICKS is 1 then each event group contains 8 usable bits (bit
 * 0 to bit 7).  If configUSE_16_BIT_TICKS is set to 0 then each event group has
 * 24 usable bits (bit 0 to bit 23).  The EventBits_t type is used to store
 * event bits within an event group.
 *
 * @return If the event group was created then a handle to the event group is
 * returned.  If there was insufficient FreeRTOS heap available to create the
 * event group then NULL is returned.  See http://www.freertos.org/a00111.html
 *
 * Example usage:
   <pre>
	// Declare a variable to hold the created event group.
	EventGroupHandle_t xCreatedEventGroup;

	// Attempt to create the event group.
	xCreatedEventGroup = xEventGroupCreate();

	// Was the event group created successfully?
	if( xCreatedEventGroup == NULL )
	{
		// The event group was not created because there was insufficient
		// FreeRTOS heap available.
	}
	else
	{
		// The event group was created.
	}
   </pre>
 * \defgroup xEventGroupCreate xEventGroupCreate
 * \ingroup EventGroup
 */
/**
 * \ingroup EventGroup
 * \defgroup xEventGroupCreate xEventGroupCreate
 * @brief 动态创建事件组对象
 * 
 * 功能：从FreeRTOS内存堆中分配并初始化新的事件组
 * 
 * 作用：
 * 1. 申请EventGroup_t结构体内存
 * 2. 初始化事件位为全0状态
 * 3. 返回可用于事件管理API的有效句柄
 * 
 * 配置依赖：
 * - configSUPPORT_DYNAMIC_ALLOCATION 必须设置为1
 * - configUSE_EVENT_GROUPS 必须设置为1
 * 
 * @return 事件组句柄
 *        - 非NULL：创建成功，返回有效句柄
 *        - NULL：创建失败（通常因堆内存不足）
 * 
 * 内存管理：
 * 1. 使用pvPortMalloc分配内存
 * 2. 内存大小 = sizeof(EventGroup_t) 
 * 3. 必须配套使用vEventGroupDelete释放资源
 * 
 * 特权说明：
 * PRIVILEGED_FUNCTION宏确保该函数在非特权模式下无法访问，
 * 用于支持MPU（内存保护单元）的安全设计
 * 
 * 典型应用：
 * EventGroupHandle_t xEvent = xEventGroupCreate();
 * if(xEvent != NULL) {
 *     // 使用事件组进行任务同步
 * } else {
 *     // 错误处理
 * }
 * 
 * 安全注意：
 * 1. 禁止在中断服务程序(ISR)中调用
 * 2. 需确保FreeRTOS堆有足够空间
 * 3. 多任务环境中需考虑创建时的线程安全
 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    EventGroupHandle_t xEventGroupCreate( void ) PRIVILEGED_FUNCTION;
#endif

/* 扩展说明：
 * 1. 与静态创建函数对比：
 *    + 优点：无需预先分配内存，简化项目管理
 *    - 缺点：可能产生内存碎片
 * 
 * 2. 初始化细节：
 *    - uxEventBits初始化为0x00000000
 *    - xTasksWaitingForBits列表初始化为空
 * 
 * 3. 错误排查：
 *    若返回NULL，可检查：
 *    a) configSUPPORT_DYNAMIC_ALLOCATION配置
 *    b) configTOTAL_HEAP_SIZE大小
 *    c) 内存碎片情况（使用xPortGetFreeHeapSize等API）
 * 
 * 4. 移植注意事项：
 *    在无动态内存管理的系统中，应使用静态创建方式
 */

/**
 * event_groups.h
 *<pre>
 EventGroupHandle_t xEventGroupCreateStatic( EventGroupHandle_t * pxEventGroupBuffer );
 </pre>
 *
 * Create a new event group.
 *
 * Internally, within the FreeRTOS implementation, event groups use a [small]
 * block of memory, in which the event group's structure is stored.  If an event
 * groups is created using xEventGropuCreate() then the required memory is
 * automatically dynamically allocated inside the xEventGroupCreate() function.
 * (see http://www.freertos.org/a00111.html).  If an event group is created
 * using xEventGropuCreateStatic() then the application writer must instead
 * provide the memory that will get used by the event group.
 * xEventGroupCreateStatic() therefore allows an event group to be created
 * without using any dynamic memory allocation.
 *
 * Although event groups are not related to ticks, for internal implementation
 * reasons the number of bits available for use in an event group is dependent
 * on the configUSE_16_BIT_TICKS setting in FreeRTOSConfig.h.  If
 * configUSE_16_BIT_TICKS is 1 then each event group contains 8 usable bits (bit
 * 0 to bit 7).  If configUSE_16_BIT_TICKS is set to 0 then each event group has
 * 24 usable bits (bit 0 to bit 23).  The EventBits_t type is used to store
 * event bits within an event group.
 *
 * @param pxEventGroupBuffer pxEventGroupBuffer must point to a variable of type
 * StaticEventGroup_t, which will be then be used to hold the event group's data
 * structures, removing the need for the memory to be allocated dynamically.
 *
 * @return If the event group was created then a handle to the event group is
 * returned.  If pxEventGroupBuffer was NULL then NULL is returned.
 *
 * Example usage:
   <pre>
	// StaticEventGroup_t is a publicly accessible structure that has the same
	// size and alignment requirements as the real event group structure.  It is
	// provided as a mechanism for applications to know the size of the event
	// group (which is dependent on the architecture and configuration file
	// settings) without breaking the strict data hiding policy by exposing the
	// real event group internals.  This StaticEventGroup_t variable is passed
	// into the xSemaphoreCreateEventGroupStatic() function and is used to store
	// the event group's data structures
	StaticEventGroup_t xEventGroupBuffer;

	// Create the event group without dynamically allocating any memory.
	xEventGroup = xEventGroupCreateStatic( &xEventGroupBuffer );
   </pre>
 */
/**
 * \ingroup EventGroup
 * \defgroup xEventGroupCreateStatic xEventGroupCreateStatic
 * @brief 静态创建事件组对象
 * 
 * 功能：在用户提供的静态内存缓冲区中初始化事件组
 * 
 * 作用：
 * 1. 避免动态内存分配，提高系统确定性
 * 2. 适用于禁止动态内存管理的嵌入式场景
 * 3. 支持MPU保护的内存区域使用
 * 
 * @param pxEventGroupBuffer [in] 静态内存缓冲区指针
 *        - 类型：StaticEventGroup_t*
 *        - 要求：
 *          a) 必须指向已分配的静态内存区域
 *          b) 内存尺寸必须≥sizeof(StaticEventGroup_t)
 *          c) 内存对齐需满足架构要求
 * 
 * @return 事件组句柄
 *        - 非NULL：初始化成功，返回有效句柄
 *        - NULL：初始化失败（缓冲区无效或对齐错误）
 * 
 * 配置依赖：
 * - configSUPPORT_STATIC_ALLOCATION 必须设置为1
 * - configUSE_EVENT_GROUPS 必须设置为1
 * 
 * 典型应用：
 * // 在全局空间分配缓冲区
 * StaticEventGroup_t xEventGroupBuffer;
 * // 在初始化函数中创建
 * EventGroupHandle_t xEvent = xEventGroupCreateStatic(&xEventGroupBuffer);
 * 
 * 设计优势：
 * 1. 无堆内存碎片风险
 * 2. 精确控制内存位置（可放入特定内存段）
 * 3. 支持编译时内存分析
 * 
 * 重要约束：
 * 1. 缓冲区生命周期必须≥事件组使用周期
 * 2. 禁止重复初始化同一缓冲区
 * 3. 需手动管理内存回收
 */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    EventGroupHandle_t xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer ) PRIVILEGED_FUNCTION;
#endif

/* 扩展说明：
 * 1. 内存分配指南：
 *    - 使用__attribute__((aligned()))确保对齐
 *    - 通过sizeof(StaticEventGroup_t)获取实际需求大小
 * 
 * 2. 与动态创建对比：
 *    + 优点：无malloc调用，适合硬实时系统
 *    - 缺点：增加开发者内存管理责任
 * 
 * 3. 错误排查：
 *    若返回NULL，检查：
 *    a) 缓冲区指针是否为NULL
 *    b) 是否跨内存保护区域（如MPU配置）
 *    c) 是否已初始化过该缓冲区
 * 
 * 4. 特权安全：
 *    PRIVILEGED_FUNCTION宏确保：
 *    - 在非特权模式调用会触发内存保护异常
 *    - 防止用户模式代码篡改关键数据结构
 */


/**
 * event_groups.h
 *<pre>
	EventBits_t xEventGroupWaitBits( 	EventGroupHandle_t xEventGroup,
										const EventBits_t uxBitsToWaitFor,
										const BaseType_t xClearOnExit,
										const BaseType_t xWaitForAllBits,
										const TickType_t xTicksToWait );
 </pre>
 *
 * [Potentially] block to wait for one or more bits to be set within a
 * previously created event group.
 *
 * This function cannot be called from an interrupt.
 *
 * @param xEventGroup The event group in which the bits are being tested.  The
 * event group must have previously been created using a call to
 * xEventGroupCreate().
 *
 * @param uxBitsToWaitFor A bitwise value that indicates the bit or bits to test
 * inside the event group.  For example, to wait for bit 0 and/or bit 2 set
 * uxBitsToWaitFor to 0x05.  To wait for bits 0 and/or bit 1 and/or bit 2 set
 * uxBitsToWaitFor to 0x07.  Etc.
 *
 * @param xClearOnExit If xClearOnExit is set to pdTRUE then any bits within
 * uxBitsToWaitFor that are set within the event group will be cleared before
 * xEventGroupWaitBits() returns if the wait condition was met (if the function
 * returns for a reason other than a timeout).  If xClearOnExit is set to
 * pdFALSE then the bits set in the event group are not altered when the call to
 * xEventGroupWaitBits() returns.
 *
 * @param xWaitForAllBits If xWaitForAllBits is set to pdTRUE then
 * xEventGroupWaitBits() will return when either all the bits in uxBitsToWaitFor
 * are set or the specified block time expires.  If xWaitForAllBits is set to
 * pdFALSE then xEventGroupWaitBits() will return when any one of the bits set
 * in uxBitsToWaitFor is set or the specified block time expires.  The block
 * time is specified by the xTicksToWait parameter.
 *
 * @param xTicksToWait The maximum amount of time (specified in 'ticks') to wait
 * for one/all (depending on the xWaitForAllBits value) of the bits specified by
 * uxBitsToWaitFor to become set.
 *
 * @return The value of the event group at the time either the bits being waited
 * for became set, or the block time expired.  Test the return value to know
 * which bits were set.  If xEventGroupWaitBits() returned because its timeout
 * expired then not all the bits being waited for will be set.  If
 * xEventGroupWaitBits() returned because the bits it was waiting for were set
 * then the returned value is the event group value before any bits were
 * automatically cleared in the case that xClearOnExit parameter was set to
 * pdTRUE.
 *
 * Example usage:
   <pre>
   #define BIT_0	( 1 << 0 )
   #define BIT_4	( 1 << 4 )

   void aFunction( EventGroupHandle_t xEventGroup )
   {
   EventBits_t uxBits;
   const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;

		// Wait a maximum of 100ms for either bit 0 or bit 4 to be set within
		// the event group.  Clear the bits before exiting.
		uxBits = xEventGroupWaitBits(
					xEventGroup,	// The event group being tested.
					BIT_0 | BIT_4,	// The bits within the event group to wait for.
					pdTRUE,			// BIT_0 and BIT_4 should be cleared before returning.
					pdFALSE,		// Don't wait for both bits, either bit will do.
					xTicksToWait );	// Wait a maximum of 100ms for either bit to be set.

		if( ( uxBits & ( BIT_0 | BIT_4 ) ) == ( BIT_0 | BIT_4 ) )
		{
			// xEventGroupWaitBits() returned because both bits were set.
		}
		else if( ( uxBits & BIT_0 ) != 0 )
		{
			// xEventGroupWaitBits() returned because just BIT_0 was set.
		}
		else if( ( uxBits & BIT_4 ) != 0 )
		{
			// xEventGroupWaitBits() returned because just BIT_4 was set.
		}
		else
		{
			// xEventGroupWaitBits() returned because xTicksToWait ticks passed
			// without either BIT_0 or BIT_4 becoming set.
		}
   }
   </pre>
 * \defgroup xEventGroupWaitBits xEventGroupWaitBits
 * \ingroup EventGroup
 */
/**
 * \ingroup EventGroup
 * \defgroup xEventGroupWaitBits xEventGroupWaitBits
 * @brief 事件组等待条件位（带超时与清除机制）
 * 
 * 功能：使任务进入阻塞状态，直到指定事件位组合被设置或超时
 * 
 * 作用：
 * 1. 原子化检查事件位状态
 * 2. 管理任务阻塞与就绪状态迁移
 * 3. 可选清除已匹配的事件位
 * 
 * @param xEventGroup [in] 事件组句柄
 *        - 要求：必须通过xEventGroupCreate创建的有效句柄
 * @param uxBitsToWaitFor [in] 等待位掩码
 *        - 格式：BIT(n)或组合掩码（如BIT0|BIT2）
 *        - 特殊值：0表示等待任意位变化
 * @param xClearOnExit [in] 退出清除标志
 *        - pdTRUE：成功等待后清除对应位
 *        - pdFALSE：保持事件位不变
 * @param xWaitForAllBits [in] 等待模式
 *        - pdTRUE：需所有指定位同时置位（AND逻辑）
 *        - pdFALSE：任一指定位置位即满足（OR逻辑）
 * @param xTicksToWait [in] 最大等待时间
 *        - 单位：系统节拍周期
 *        - 特殊值：portMAX_DELAY（无限等待）
 * 
 * @return 事件位状态
 *        - 非0：满足条件时的事件位组合（可能包含未等待的位）
 *        - 0：仅在uxBitsToWaitFor=0且超时时返回
 * 
 * 状态机流程：
 * [运行] -> 检查事件位 -> [满足条件] -> 清除位(可选) -> 返回
 *             |          -> [不满足] -> 加入等待列表 -> [阻塞] -> [超时/事件触发] -> 返回
 * 
 * 典型应用：
 * // 等待BIT0和BIT2同时置位，成功后清除这两个位
 * EventBits_t uxBits = xEventGroupWaitBits(xGroup, BIT0|BIT2, pdTRUE, pdTRUE, 100);
 * if((uxBits & (BIT0|BIT2)) == (BIT0|BIT2)) {
 *     // 事件处理
 * }
 * 
 * 重要约束：
 * 1. 禁止在中断服务程序(ISR)中调用
 * 2. 同一事件组可被多任务同时等待
 * 3. 清除操作影响后续等待者，需谨慎使用xClearOnExit
 */
EventBits_t xEventGroupWaitBits( 
    EventGroupHandle_t xEventGroup, 
    const EventBits_t uxBitsToWaitFor, 
    const BaseType_t xClearOnExit, 
    const BaseType_t xWaitForAllBits, 
    TickType_t xTicksToWait 
) PRIVILEGED_FUNCTION;

/* 工作模式详解：
 * 1. 立即返回条件：
 *    - (当前Bits & uxBitsToWaitFor) 符合xWaitForAllBits条件
 *    - uxBitsToWaitFor=0且当前Bits≠上次检查值
 * 
 * 2. 清除机制：
 *    - 仅在成功等待时执行（超时不触发清除）
 *    - 清除位 = uxBitsToWaitFor & 当前Bits
 * 
 * 3. 特殊等待场景：
 *    uxBitsToWaitFor=0时，监测任意位变化：
 *    - 首次调用记录当前Bits值
 *    - 当Bits值变化时立即返回
 * 
 * 同步策略：
 * 使用任务优先级继承机制，高优先级任务优先获取事件
 * 
 * 性能注意：
 * 当多任务等待同一事件组时，唤醒操作时间复杂度为O(n)
 */

/**
 * event_groups.h
 *<pre>
	EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear );
 </pre>
 *
 * Clear bits within an event group.  This function cannot be called from an
 * interrupt.
 *
 * @param xEventGroup The event group in which the bits are to be cleared.
 *
 * @param uxBitsToClear A bitwise value that indicates the bit or bits to clear
 * in the event group.  For example, to clear bit 3 only, set uxBitsToClear to
 * 0x08.  To clear bit 3 and bit 0 set uxBitsToClear to 0x09.
 *
 * @return The value of the event group before the specified bits were cleared.
 *
 * Example usage:
   <pre>
   #define BIT_0	( 1 << 0 )
   #define BIT_4	( 1 << 4 )

   void aFunction( EventGroupHandle_t xEventGroup )
   {
   EventBits_t uxBits;

		// Clear bit 0 and bit 4 in xEventGroup.
		uxBits = xEventGroupClearBits(
								xEventGroup,	// The event group being updated.
								BIT_0 | BIT_4 );// The bits being cleared.

		if( ( uxBits & ( BIT_0 | BIT_4 ) ) == ( BIT_0 | BIT_4 ) )
		{
			// Both bit 0 and bit 4 were set before xEventGroupClearBits() was
			// called.  Both will now be clear (not set).
		}
		else if( ( uxBits & BIT_0 ) != 0 )
		{
			// Bit 0 was set before xEventGroupClearBits() was called.  It will
			// now be clear.
		}
		else if( ( uxBits & BIT_4 ) != 0 )
		{
			// Bit 4 was set before xEventGroupClearBits() was called.  It will
			// now be clear.
		}
		else
		{
			// Neither bit 0 nor bit 4 were set in the first place.
		}
   }
   </pre>
 * \defgroup xEventGroupClearBits xEventGroupClearBits
 * \ingroup EventGroup
 */
/**
 * \ingroup EventGroup
 * \defgroup xEventGroupClearBits xEventGroupClearBits
 * @brief 清除事件组指定标志位（任务上下文）
 * 
 * 功能：原子化地清除事件组中指定的一个或多个标志位
 * 
 * 作用：
 * 1. 执行位清除操作：EventBits &= ~uxBitsToClear
 * 2. 返回操作前的事件位快照
 * 3. 触发可能满足条件的等待任务检查
 * 
 * @param xEventGroup [in] 事件组句柄
 *        - 要求：必须通过xEventGroupCreate创建的有效句柄
 * @param uxBitsToClear [in] 清除位掩码
 *        - 格式：BIT(n)或组合掩码（如BIT0|BIT2）
 *        - 注意：未设置的位不会影响当前状态
 * 
 * @return 清除操作前的事件位状态
 *        - 包含所有位（包括未被清除位）的原始状态
 * 
 * 典型应用：
 * // 清除BIT3和BIT5位
 * EventBits_t uxPrevBits = xEventGroupClearBits(xGroup, BIT3|BIT5);
 * if(uxPrevBits & BIT3) {
 *     // BIT3在被清除前处于置位状态
 * }
 * 
 * 设计特性：
 * 1. 原子操作：全程关闭中断保护
 * 2. 非破坏性读取：返回值为操作前完整状态
 * 3. 无任务唤醒：清除操作本身不会唤醒等待任务
 * 
 * 重要约束：
 * 1. 禁止在中断服务程序(ISR)中使用
 * 2. 清除操作不影响已处于等待状态的任务
 * 3. 需与xEventGroupSetBits配合实现完整事件生命周期管理
 */
EventBits_t xEventGroupClearBits( 
    EventGroupHandle_t xEventGroup, 
    const EventBits_t uxBitsToClear 
) PRIVILEGED_FUNCTION;

/* 操作原理说明：
 * 1. 位清除实现：
 *    uxEventBits = uxEventBits & (~uxBitsToClear)
 * 
 * 2. 返回值构成：
 *    返回执行清除操作前瞬间的uxEventBits值
 *    包含所有位状态，即使未被清除的位
 * 
 * 3. 与等待任务交互：
 *    清除操作完成后，调度器会：
 *    a) 重新评估等待列表中的任务条件
 *    b) 仅唤醒新满足条件的任务
 * 
 * 使用技巧：
 * 1. 获取事件位快照：
 *    // 不实际清除任何位，仅获取当前状态
 *    xEventGroupClearBits(xGroup, 0)
 * 
 * 2. 批量清除多个位：
 *    xEventGroupClearBits(xGroup, BIT0|BIT1|BIT2)
 * 
 * 3. 安全清除策略：
 *    do {
 *        uxBits = xEventGroupClearBits(xGroup, MASK);
 *    } while(uxBits & MASK); // 确保清除成功
 */ 

/**
 * event_groups.h
 *<pre>
	BaseType_t xEventGroupClearBitsFromISR( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet );
 </pre>
 *
 * A version of xEventGroupClearBits() that can be called from an interrupt.
 *
 * Setting bits in an event group is not a deterministic operation because there
 * are an unknown number of tasks that may be waiting for the bit or bits being
 * set.  FreeRTOS does not allow nondeterministic operations to be performed
 * while interrupts are disabled, so protects event groups that are accessed
 * from tasks by suspending the scheduler rather than disabling interrupts.  As
 * a result event groups cannot be accessed directly from an interrupt service
 * routine.  Therefore xEventGroupClearBitsFromISR() sends a message to the
 * timer task to have the clear operation performed in the context of the timer
 * task.
 *
 * @param xEventGroup The event group in which the bits are to be cleared.
 *
 * @param uxBitsToClear A bitwise value that indicates the bit or bits to clear.
 * For example, to clear bit 3 only, set uxBitsToClear to 0x08.  To clear bit 3
 * and bit 0 set uxBitsToClear to 0x09.
 *
 * @return If the request to execute the function was posted successfully then
 * pdPASS is returned, otherwise pdFALSE is returned.  pdFALSE will be returned
 * if the timer service queue was full.
 *
 * Example usage:
   <pre>
   #define BIT_0	( 1 << 0 )
   #define BIT_4	( 1 << 4 )

   // An event group which it is assumed has already been created by a call to
   // xEventGroupCreate().
   EventGroupHandle_t xEventGroup;

   void anInterruptHandler( void )
   {
		// Clear bit 0 and bit 4 in xEventGroup.
		xResult = xEventGroupClearBitsFromISR(
							xEventGroup,	 // The event group being updated.
							BIT_0 | BIT_4 ); // The bits being set.

		if( xResult == pdPASS )
		{
			// The message was posted successfully.
		}
  }
   </pre>
 * \defgroup xEventGroupClearBitsFromISR xEventGroupClearBitsFromISR
 * \ingroup EventGroup
 */
/**
 * \ingroup EventGroup
 * \defgroup xEventGroupClearBitsFromISR xEventGroupClearBitsFromISR
 * @brief 中断上下文清除事件标志位（ISR安全版本）
 * 
 * 功能：在中断服务程序中安全清除事件组指定位
 * 
 * 作用：
 * 1. 提供ISR环境下的原子化位清除操作
 * 2. 支持两种实现模式：
 *    - 直接操作（需启用Trace Facility）
 *    - 延迟到任务上下文执行（默认）
 * 
 * @param xEventGroup [in] 事件组句柄
 *        - 要求：必须通过xEventGroupCreate创建的有效句柄
 * @param uxBitsToClear [in] 清除位掩码
 *        - 格式：BIT(n)或组合掩码（如BIT0|BIT2）
 * 
 * @return 操作状态
 *        - 直接模式：pdPASS(成功)/pdFAIL(失败)
 *        - 延迟模式：总是返回pdPASS（通过队列状态判断实际结果）
 * 
 * 配置依赖：
 * - configUSE_TRACE_FACILITY=1：启用直接清除模式
 * - configUSE_TIMERS=1：延迟模式需要定时器服务
 * 
 * 实现差异：
 * [直接模式]
 * 1. 使用临界区保护位操作
 * 2. 立即更新事件位状态
 * 3. 可能唤醒等待任务
 * 
 * [延迟模式]
 * 1. 通过守护任务队列延迟处理
 * 2. 实际清除操作稍后在任务上下文执行
 * 3. 增加1个tick的延迟
 * 
 * 典型应用：
 * // 在串口接收中断中清除数据就绪标志
 * xEventGroupClearBitsFromISR(xGroup, BIT0);
 * 
 * 重要约束：
 * 1. 必须在ISR中调用
 * 2. 延迟模式下需确保：
 *    a) 已创建Timer服务任务
 *    b) 队列未满
 * 3. 清除操作的非即时性需在设计中考虑
 */
#if( configUSE_TRACE_FACILITY == 1 )
    BaseType_t xEventGroupClearBitsFromISR( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet ) PRIVILEGED_FUNCTION;
#else
    #define xEventGroupClearBitsFromISR( xEventGroup, uxBitsToClear ) \
        xTimerPendFunctionCallFromISR( \
            vEventGroupClearBitsCallback,      /* 清除操作回调函数 */ \
            ( void * ) xEventGroup,            /* 事件组句柄透传 */ \
            ( uint32_t ) uxBitsToClear,        /* 位掩码参数 */ \
            NULL )                             /* 不返回pxHigherPriorityTaskWoken */
#endif

/* 技术细节说明：
 * 延迟模式工作流程：
 * 1. ISR中将清除请求加入定时器命令队列
 * 2. 定时器守护任务取出命令
 * 3. 调用vEventGroupClearBitsCallback执行实际清除
 * 
 * 直接模式优势：
 * - 即时性强，适用于实时性要求高的场景
 * - 减少上下文切换开销
 * 
 * 设计考虑：
 * 1. 临界区保护：直接模式使用taskENTER_CRITICAL_FROM_ISR()
 * 2. 内存屏障：确保位操作在多核系统中的可见性
 * 
 * 错误处理：
 * 延迟模式下队列满时：
 * - 返回pdFAIL
 * - 需在ISR中实现重试机制或错误统计
 * 
 * 性能指标：
 * 直接模式：O(1)时间复杂度
 * 延迟模式：包含队列操作+任务切换开销
 */

/**
 * event_groups.h
 *<pre>
	EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet );
 </pre>
 *
 * Set bits within an event group.
 * This function cannot be called from an interrupt.  xEventGroupSetBitsFromISR()
 * is a version that can be called from an interrupt.
 *
 * Setting bits in an event group will automatically unblock tasks that are
 * blocked waiting for the bits.
 *
 * @param xEventGroup The event group in which the bits are to be set.
 *
 * @param uxBitsToSet A bitwise value that indicates the bit or bits to set.
 * For example, to set bit 3 only, set uxBitsToSet to 0x08.  To set bit 3
 * and bit 0 set uxBitsToSet to 0x09.
 *
 * @return The value of the event group at the time the call to
 * xEventGroupSetBits() returns.  There are two reasons why the returned value
 * might have the bits specified by the uxBitsToSet parameter cleared.  First,
 * if setting a bit results in a task that was waiting for the bit leaving the
 * blocked state then it is possible the bit will be cleared automatically
 * (see the xClearBitOnExit parameter of xEventGroupWaitBits()).  Second, any
 * unblocked (or otherwise Ready state) task that has a priority above that of
 * the task that called xEventGroupSetBits() will execute and may change the
 * event group value before the call to xEventGroupSetBits() returns.
 *
 * Example usage:
   <pre>
   #define BIT_0	( 1 << 0 )
   #define BIT_4	( 1 << 4 )

   void aFunction( EventGroupHandle_t xEventGroup )
   {
   EventBits_t uxBits;

		// Set bit 0 and bit 4 in xEventGroup.
		uxBits = xEventGroupSetBits(
							xEventGroup,	// The event group being updated.
							BIT_0 | BIT_4 );// The bits being set.

		if( ( uxBits & ( BIT_0 | BIT_4 ) ) == ( BIT_0 | BIT_4 ) )
		{
			// Both bit 0 and bit 4 remained set when the function returned.
		}
		else if( ( uxBits & BIT_0 ) != 0 )
		{
			// Bit 0 remained set when the function returned, but bit 4 was
			// cleared.  It might be that bit 4 was cleared automatically as a
			// task that was waiting for bit 4 was removed from the Blocked
			// state.
		}
		else if( ( uxBits & BIT_4 ) != 0 )
		{
			// Bit 4 remained set when the function returned, but bit 0 was
			// cleared.  It might be that bit 0 was cleared automatically as a
			// task that was waiting for bit 0 was removed from the Blocked
			// state.
		}
		else
		{
			// Neither bit 0 nor bit 4 remained set.  It might be that a task
			// was waiting for both of the bits to be set, and the bits were
			// cleared as the task left the Blocked state.
		}
   }
   </pre>
 * \defgroup xEventGroupSetBits xEventGroupSetBits
 * \ingroup EventGroup
 */
/**
 * \ingroup EventGroup
 * \defgroup xEventGroupSetBits xEventGroupSetBits
 * @brief 设置事件组指定标志位（任务上下文）
 * 
 * 功能：原子化地设置事件组中指定的一个或多个标志位
 * 
 * 作用：
 * 1. 执行位设置操作：EventBits |= uxBitsToSet
 * 2. 返回设置后的完整事件位状态
 * 3. 触发等待任务的条件检查及可能的唤醒
 * 
 * @param xEventGroup [in] 事件组句柄
 *        - 要求：必须通过xEventGroupCreate创建的有效句柄
 * @param uxBitsToSet [in] 设置位掩码
 *        - 格式：BIT(n)或组合掩码（如BIT0|BIT2）
 *        - 注意：已设置的位保持原状
 * 
 * @return 设置操作后的事件位状态
 *        - 包含所有位的最新状态
 * 
 * 典型应用：
 * // 设置BIT1和BIT4位
 * EventBits_t uxCurrentBits = xEventGroupSetBits(xGroup, BIT1|BIT4);
 * if(uxCurrentBits & BIT4) {
 *     // BIT4在设置后处于置位状态
 * }
 * 
 * 设计特性：
 * 1. 原子操作：全程关闭中断保护
 * 2. 触发唤醒：立即唤醒满足条件的等待任务
 * 3. 位或操作：保留原有置位状态，只增加新位
 * 
 * 重要约束：
 * 1. 禁止在中断服务程序(ISR)中使用
 * 2. 可能引发任务优先级反转（需合理设计等待优先级）
 * 3. 高频设置操作可能影响实时性
 */
EventBits_t xEventGroupSetBits(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToSet
) PRIVILEGED_FUNCTION;

/* 操作原理说明：
 * 1. 位设置实现：
 *    uxEventBits = uxEventBits | uxBitsToSet
 * 
 * 2. 唤醒机制：
 *    a) 遍历等待列表中的所有任务
 *    b) 对每个任务检查其等待条件（AND/OR）
 *    c) 满足条件则移出等待列表，加入就绪队列
 * 
 * 3. 返回值构成：
 *    返回设置操作完成后瞬间的uxEventBits值
 *    包含所有位的最新状态
 * 
 * 使用技巧：
 * 1. 组合设置与检查：
 *    // 设置位并检查是否达到预期状态
 *    if((xEventGroupSetBits(xGroup, BIT2) & BIT2) != 0) {
 *        // BIT2成功置位
 *    }
 * 
 * 2. 批量设置多个位：
 *    xEventGroupSetBits(xGroup, BIT3|BIT5|BIT7)
 * 
 * 3. 事件触发策略：
 *    // 设置事件位后立即进行任务调度
 *    xEventGroupSetBits(xGroup, EVENT_MASK);
 *    taskYIELD();
 * 
 * 性能优化：
 * 当多任务等待同一事件组时，使用优先级排序唤醒：
 * 1. 按任务优先级降序处理
 * 2. 高优先级任务优先获得CPU
 */

/**
 * event_groups.h
 *<pre>
	BaseType_t xEventGroupSetBitsFromISR( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken );
 </pre>
 *
 * A version of xEventGroupSetBits() that can be called from an interrupt.
 *
 * Setting bits in an event group is not a deterministic operation because there
 * are an unknown number of tasks that may be waiting for the bit or bits being
 * set.  FreeRTOS does not allow nondeterministic operations to be performed in
 * interrupts or from critical sections.  Therefore xEventGroupSetBitsFromISR()
 * sends a message to the timer task to have the set operation performed in the
 * context of the timer task - where a scheduler lock is used in place of a
 * critical section.
 *
 * @param xEventGroup The event group in which the bits are to be set.
 *
 * @param uxBitsToSet A bitwise value that indicates the bit or bits to set.
 * For example, to set bit 3 only, set uxBitsToSet to 0x08.  To set bit 3
 * and bit 0 set uxBitsToSet to 0x09.
 *
 * @param pxHigherPriorityTaskWoken As mentioned above, calling this function
 * will result in a message being sent to the timer daemon task.  If the
 * priority of the timer daemon task is higher than the priority of the
 * currently running task (the task the interrupt interrupted) then
 * *pxHigherPriorityTaskWoken will be set to pdTRUE by
 * xEventGroupSetBitsFromISR(), indicating that a context switch should be
 * requested before the interrupt exits.  For that reason
 * *pxHigherPriorityTaskWoken must be initialised to pdFALSE.  See the
 * example code below.
 *
 * @return If the request to execute the function was posted successfully then
 * pdPASS is returned, otherwise pdFALSE is returned.  pdFALSE will be returned
 * if the timer service queue was full.
 *
 * Example usage:
   <pre>
   #define BIT_0	( 1 << 0 )
   #define BIT_4	( 1 << 4 )

   // An event group which it is assumed has already been created by a call to
   // xEventGroupCreate().
   EventGroupHandle_t xEventGroup;

   void anInterruptHandler( void )
   {
   BaseType_t xHigherPriorityTaskWoken, xResult;

		// xHigherPriorityTaskWoken must be initialised to pdFALSE.
		xHigherPriorityTaskWoken = pdFALSE;

		// Set bit 0 and bit 4 in xEventGroup.
		xResult = xEventGroupSetBitsFromISR(
							xEventGroup,	// The event group being updated.
							BIT_0 | BIT_4   // The bits being set.
							&xHigherPriorityTaskWoken );

		// Was the message posted successfully?
		if( xResult == pdPASS )
		{
			// If xHigherPriorityTaskWoken is now set to pdTRUE then a context
			// switch should be requested.  The macro used is port specific and
			// will be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() -
			// refer to the documentation page for the port being used.
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		}
  }
   </pre>
 * \defgroup xEventGroupSetBitsFromISR xEventGroupSetBitsFromISR
 * \ingroup EventGroup
 */
/**
 * \ingroup EventGroup
 * \defgroup xEventGroupSetBitsFromISR xEventGroupSetBitsFromISR
 * @brief 中断上下文设置事件标志位（ISR安全版本）
 * 
 * 功能：在中断服务程序中安全设置事件组指定位，并触发任务唤醒
 * 
 * 作用：
 * 1. 提供ISR环境下的原子化位设置操作
 * 2. 支持两种实现模式：
 *    - 直接操作（需启用Trace Facility）
 *    - 延迟到任务上下文执行（默认）
 * 
 * @param xEventGroup [in] 事件组句柄
 *        - 要求：必须通过xEventGroupCreate创建的有效句柄
 * @param uxBitsToSet [in] 设置位掩码
 *        - 格式：BIT(n)或组合掩码（如BIT0|BIT2）
 * @param pxHigherPriorityTaskWoken [out] 高优先级任务唤醒标志指针
 *        - 输出pdTRUE：有更高优先级任务就绪
 *        - 可为NULL：不关注唤醒状态
 * 
 * @return 操作状态
 *        - 直接模式：pdPASS(成功)/pdFAIL(失败)
 *        - 延迟模式：队列操作结果（pdPASS/pdFAIL）
 * 
 * 配置依赖：
 * - configUSE_TRACE_FACILITY=1：启用直接设置模式
 * - configUSE_TIMERS=1：延迟模式需要定时器服务
 * 
 * 实现差异：
 * [直接模式]
 * 1. 使用临界区保护位操作
 * 2. 立即更新事件位并触发任务唤醒
 * 3. 通过pxHigherPriorityTaskWoken返回调度需求
 * 
 * [延迟模式]
 * 1. 通过守护任务队列延迟处理
 * 2. 实际设置操作稍后在任务上下文执行
 * 3. 唤醒标志通过回调函数传递
 * 
 * 典型应用：
 * // 在定时器中断中触发事件
 * BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 * xEventGroupSetBitsFromISR(xGroup, BIT3, &xHigherPriorityTaskWoken);
 * portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
 * 
 * 重要约束：
 * 1. 必须在ISR中调用
 * 2. 延迟模式下需确保：
 *    a) 已创建Timer服务任务
 *    b) 队列未满
 * 3. 直接模式下需要足够的中断栈空间
 */
#if( configUSE_TRACE_FACILITY == 1 )
    BaseType_t xEventGroupSetBitsFromISR( 
        EventGroupHandle_t xEventGroup, 
        const EventBits_t uxBitsToSet, 
        BaseType_t *pxHigherPriorityTaskWoken 
    ) PRIVILEGED_FUNCTION;
#else
    #define xEventGroupSetBitsFromISR( xEventGroup, uxBitsToSet, pxHigherPriorityTaskWoken ) \
        xTimerPendFunctionCallFromISR( \
            vEventGroupSetBitsCallback,       /* 设置操作回调函数 */ \
            ( void * ) xEventGroup,            /* 事件组句柄透传 */ \
            ( uint32_t ) uxBitsToSet,          /* 位掩码参数 */ \
            pxHigherPriorityTaskWoken )        /* 传递唤醒标志指针 */
#endif

/* 技术细节说明：
 * 直接模式工作流程：
 * 1. 进入临界区保护
 * 2. 执行位设置操作：uxEventBits |= uxBitsToSet
 * 3. 扫描等待列表并唤醒符合条件任务
 * 4. 更新pxHigherPriorityTaskWoken状态
 * 
 * 延迟模式时序：
 * ISR上下文 -> 队列命令 -> 守护任务 -> 回调函数 -> 实际设置
 * 
 * 性能对比：
 * 直接模式：
 * + 即时性高，适用于实时响应场景
 * - 增加中断延迟时间
 * 
 * 延迟模式：
 * + 保持中断快速响应
 * - 增加1~数个tick的延迟
 * 
 * 错误处理：
 * 当队列满时返回pdFAIL，建议：
 * 1. 增加定时器队列长度(configTIMER_QUEUE_LENGTH)
 * 2. 在ISR中实现错误计数机制
 * 
 * 内存管理：
 * 直接模式需确保中断栈足够处理位扫描操作
 */

/**
 * event_groups.h
 *<pre>
	EventBits_t xEventGroupSync(	EventGroupHandle_t xEventGroup,
									const EventBits_t uxBitsToSet,
									const EventBits_t uxBitsToWaitFor,
									TickType_t xTicksToWait );
 </pre>
 *
 * Atomically set bits within an event group, then wait for a combination of
 * bits to be set within the same event group.  This functionality is typically
 * used to synchronise multiple tasks, where each task has to wait for the other
 * tasks to reach a synchronisation point before proceeding.
 *
 * This function cannot be used from an interrupt.
 *
 * The function will return before its block time expires if the bits specified
 * by the uxBitsToWait parameter are set, or become set within that time.  In
 * this case all the bits specified by uxBitsToWait will be automatically
 * cleared before the function returns.
 *
 * @param xEventGroup The event group in which the bits are being tested.  The
 * event group must have previously been created using a call to
 * xEventGroupCreate().
 *
 * @param uxBitsToSet The bits to set in the event group before determining
 * if, and possibly waiting for, all the bits specified by the uxBitsToWait
 * parameter are set.
 *
 * @param uxBitsToWaitFor A bitwise value that indicates the bit or bits to test
 * inside the event group.  For example, to wait for bit 0 and bit 2 set
 * uxBitsToWaitFor to 0x05.  To wait for bits 0 and bit 1 and bit 2 set
 * uxBitsToWaitFor to 0x07.  Etc.
 *
 * @param xTicksToWait The maximum amount of time (specified in 'ticks') to wait
 * for all of the bits specified by uxBitsToWaitFor to become set.
 *
 * @return The value of the event group at the time either the bits being waited
 * for became set, or the block time expired.  Test the return value to know
 * which bits were set.  If xEventGroupSync() returned because its timeout
 * expired then not all the bits being waited for will be set.  If
 * xEventGroupSync() returned because all the bits it was waiting for were
 * set then the returned value is the event group value before any bits were
 * automatically cleared.
 *
 * Example usage:
 <pre>
 // Bits used by the three tasks.
 #define TASK_0_BIT		( 1 << 0 )
 #define TASK_1_BIT		( 1 << 1 )
 #define TASK_2_BIT		( 1 << 2 )

 #define ALL_SYNC_BITS ( TASK_0_BIT | TASK_1_BIT | TASK_2_BIT )

 // Use an event group to synchronise three tasks.  It is assumed this event
 // group has already been created elsewhere.
 EventGroupHandle_t xEventBits;

 void vTask0( void *pvParameters )
 {
 EventBits_t uxReturn;
 TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;

	 for( ;; )
	 {
		// Perform task functionality here.

		// Set bit 0 in the event flag to note this task has reached the
		// sync point.  The other two tasks will set the other two bits defined
		// by ALL_SYNC_BITS.  All three tasks have reached the synchronisation
		// point when all the ALL_SYNC_BITS are set.  Wait a maximum of 100ms
		// for this to happen.
		uxReturn = xEventGroupSync( xEventBits, TASK_0_BIT, ALL_SYNC_BITS, xTicksToWait );

		if( ( uxReturn & ALL_SYNC_BITS ) == ALL_SYNC_BITS )
		{
			// All three tasks reached the synchronisation point before the call
			// to xEventGroupSync() timed out.
		}
	}
 }

 void vTask1( void *pvParameters )
 {
	 for( ;; )
	 {
		// Perform task functionality here.

		// Set bit 1 in the event flag to note this task has reached the
		// synchronisation point.  The other two tasks will set the other two
		// bits defined by ALL_SYNC_BITS.  All three tasks have reached the
		// synchronisation point when all the ALL_SYNC_BITS are set.  Wait
		// indefinitely for this to happen.
		xEventGroupSync( xEventBits, TASK_1_BIT, ALL_SYNC_BITS, portMAX_DELAY );

		// xEventGroupSync() was called with an indefinite block time, so
		// this task will only reach here if the syncrhonisation was made by all
		// three tasks, so there is no need to test the return value.
	 }
 }

 void vTask2( void *pvParameters )
 {
	 for( ;; )
	 {
		// Perform task functionality here.

		// Set bit 2 in the event flag to note this task has reached the
		// synchronisation point.  The other two tasks will set the other two
		// bits defined by ALL_SYNC_BITS.  All three tasks have reached the
		// synchronisation point when all the ALL_SYNC_BITS are set.  Wait
		// indefinitely for this to happen.
		xEventGroupSync( xEventBits, TASK_2_BIT, ALL_SYNC_BITS, portMAX_DELAY );

		// xEventGroupSync() was called with an indefinite block time, so
		// this task will only reach here if the syncrhonisation was made by all
		// three tasks, so there is no need to test the return value.
	}
 }

 </pre>
 * \defgroup xEventGroupSync xEventGroupSync
 * \ingroup EventGroup
 */
/**
 * \ingroup EventGroup
 * \defgroup xEventGroupSync xEventGroupSync
 * @brief 事件组同步操作（原子化设置+等待）
 * 
 * 功能：原子化地设置指定位并等待目标位组合，实现多任务同步
 * 
 * 作用：
 * 1. 设置指定事件位（uxBitsToSet）
 * 2. 阻塞等待目标位组合（uxBitsToWaitFor）
 * 3. 在满足条件或超时后清除等待位（可选）
 * 
 * @param xEventGroup [in] 事件组句柄
 *        - 要求：必须通过xEventGroupCreate创建的有效句柄
 * @param uxBitsToSet [in] 设置位掩码
 *        - 格式：BIT(n)或组合掩码
 * @param uxBitsToWaitFor [in] 等待位掩码
 *        - 格式：BIT(n)或组合掩码，不可为0
 * @param xTicksToWait [in] 最大等待时间
 *        - 单位：系统节拍周期
 *        - 特殊值：portMAX_DELAY（无限等待）
 * 
 * @return 退出时的实际事件位状态
 *        - 满足条件：返回当前事件位（可能包含额外置位）
 *        - 超时：返回超时瞬间的事件位状态
 * 
 * 典型应用：
 * // 任务A设置BIT0后等待BIT1|BIT2
 * EventBits_t uxBits = xEventGroupSync(xGroup, BIT0, BIT1|BIT2, 100);
 * if((uxBits & (BIT1|BIT2)) == (BIT1|BIT2)) {
 *     // 同步成功
 * }
 * 
 * 设计特性：
 * 1. 原子操作：设置与等待之间不会被中断
 * 2. 自动清除：成功等待后可选清除等待位
 * 3. 高效同步：替代传统先设置后等待的分步操作
 */
EventBits_t xEventGroupSync(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToSet,
    const EventBits_t uxBitsToWaitFor,
    TickType_t xTicksToWait
) PRIVILEGED_FUNCTION;

/**
 * \ingroup EventGroup
 * \defgroup xEventGroupGetBits xEventGroupGetBits
 * @brief 获取事件组当前位状态（非中断安全）
 * 
 * 功能：安全读取事件组的当前所有标志位状态
 * 
 * 实现原理：
 * 通过清除0位掩码的巧妙方式获取当前状态：
 * xEventGroupClearBits(xEventGroup, 0) 实际上不修改任何位
 * 
 * @param xEventGroup [in] 事件组句柄
 * @return 调用瞬间的事件位完整状态
 * 
 * 注意：
 * 1. 禁止在中断服务程序中使用
 * 2. 实际为宏定义，无函数调用开销
 * 
 * 典型应用：
 * EventBits_t currentBits = xEventGroupGetBits(xGroup);
 */
#define xEventGroupGetBits( xEventGroup ) xEventGroupClearBits( xEventGroup, 0 )

/**
 * \ingroup EventGroup
 * \defgroup xEventGroupGetBitsFromISR xEventGroupGetBitsFromISR
 * @brief 获取事件组当前位状态（中断安全版本）
 * 
 * 功能：在ISR中安全读取事件组的所有标志位状态
 * 
 * 实现机制：
 * 1. 使用临界区保护（taskENTER_CRITICAL_FROM_ISR）
 * 2. 直接读取事件组结构体的uxEventBits字段
 * 
 * @param xEventGroup [in] 事件组句柄
 * @return 调用瞬间的事件位快照
 * 
 * 重要约束：
 * 1. 必须在中断上下文调用
 * 2. 读取的是瞬时状态，后续可能立即变化
 * 
 * 典型应用：
 * // 在ISR中检查事件位
 * EventBits_t bits = xEventGroupGetBitsFromISR(xGroup);
 * if(bits & BIT3) {
 *     // 处理BIT3置位情况
 * }
 */
EventBits_t xEventGroupGetBitsFromISR( EventGroupHandle_t xEventGroup ) PRIVILEGED_FUNCTION;

/* 设计扩展说明：
 * 1. 同步操作优化：
 *    xEventGroupSync等效于：
 *    xEventGroupSetBits();
 *    xEventGroupWaitBits();
 *    但具有原子性优势，避免竞态条件
 * 
 * 2. 状态获取对比：
 *               | 任务上下文 | 中断上下文
 * ——————————————|———————————|———————————
 * 安全读取      | GetBits   | GetBitsFromISR
 * 性能影响      | 临界区保护 | 临界区保护
 * 
 * 3. 位状态瞬态性：
 *    由于事件组的异步特性，获取的位状态可能
 *    在读取后立即改变，建议：
 *    - 必要时结合互斥锁使用
 *    - 对关键状态采用确认机制
 */

/**
 * event_groups.h
 *<pre>
	void xEventGroupDelete( EventGroupHandle_t xEventGroup );
 </pre>
 *
 * Delete an event group that was previously created by a call to
 * xEventGroupCreate().  Tasks that are blocked on the event group will be
 * unblocked and obtain 0 as the event group's value.
 *
 * @param xEventGroup The event group being deleted.
 */
/**
 * \ingroup EventGroup
 * @brief 删除事件组对象
 * 
 * 功能：永久移除事件组并释放相关资源
 * 
 * 作用：
 * 1. 释放事件组控制块内存（动态创建时）
 * 2. 唤醒所有等待该事件组的任务（返回0作为事件位）
 * 3. 使事件组句柄失效
 * 
 * @param xEventGroup [in] 待删除事件组句柄
 * 
 * 重要约束：
 * 1. 必须确保所有任务已停止使用该事件组
 * 2. 禁止删除已被删除的句柄
 * 3. 静态创建的事件组需手动回收内存
 * 
 * 典型应用：
 * // 动态创建后删除
 * EventGroupHandle_t xGroup = xEventGroupCreate();
 * // ...使用过程...
 * vEventGroupDelete(xGroup);
 * xGroup = NULL; // 防止野指针
 */
void vEventGroupDelete( EventGroupHandle_t xEventGroup ) PRIVILEGED_FUNCTION;

/* 内部回调函数（应用层禁止直接调用） */
/**
 * @brief 事件组设置位回调（定时器守护任务专用）
 * 
 * 功能：执行来自中断上下文的延迟位设置操作
 * 
 * 作用：
 * 1. 将xEventGroupSetBitsFromISR的请求转换到任务上下文
 * 2. 实际调用xEventGroupSetBits进行位操作
 * 
 * @param pvEventGroup [in] 事件组句柄（void*类型转换）
 * @param ulBitsToSet [in] 位设置掩码
 */
void vEventGroupSetBitsCallback( void *pvEventGroup, const uint32_t ulBitsToSet ) PRIVILEGED_FUNCTION;

/**
 * @brief 事件组清除位回调（定时器守护任务专用）
 * 
 * 功能：执行来自中断上下文的延迟位清除操作
 * 
 * 作用：
 * 1. 将xEventGroupClearBitsFromISR的请求转换到任务上下文
 * 2. 实际调用xEventGroupClearBits进行位操作
 * 
 * @param pvEventGroup [in] 事件组句柄（void*类型转换）
 * @param ulBitsToClear [in] 位清除掩码
 */
void vEventGroupClearBitsCallback( void *pvEventGroup, const uint32_t ulBitsToClear ) PRIVILEGED_FUNCTION;

#if (configUSE_TRACE_FACILITY == 1)
/**
 * \ingroup EventGroup
 * @brief 获取事件组唯一标识号
 * 
 * 功能：为调试和追踪提供事件组识别标识
 * 
 * 作用：
 * 1. 配合Trace工具可视化事件组状态
 * 2. 在系统日志中标识特定事件组实例
 * 
 * @param xEventGroup [in] 事件组句柄
 * @return 事件组唯一编号
 *        - 从1开始递增分配
 *        - 0表示无效句柄
 * 
 * 配置依赖：
 * - configUSE_TRACE_FACILITY 必须设置为1
 * 
 * 调试应用：
 * // 在调试中断中查看事件组信息
 * UBaseType_t uxNum = uxEventGroupGetNumber(xGroup);
 * printf("EventGroup #%u state: 0x%X", uxNum, xEventGroupGetBits(xGroup));
 */
UBaseType_t uxEventGroupGetNumber( void* xEventGroup ) PRIVILEGED_FUNCTION;
#endif

/* 系统集成说明：
 * 1. 内存管理：
 *    + 动态创建的事件组在删除时自动释放内存
 *    + 静态创建的事件组需手动管理生命周期
 * 
 * 2. 回调函数关联：
 *    + vEventGroupSetBitsCallback ← xEventGroupSetBitsFromISR
 *    + vEventGroupClearBitsCallback ← xEventGroupClearBitsFromISR
 * 
 * 3. 追踪机制：
 *    + uxEventGroupGetNumber返回的编号与FreeRTOS+Trace事件关联
 *    + 可通过trcKernelPortGetEventGroupNumber()获取相同编号
 * 
 * 安全警告：
 * 1. 删除事件组不会自动重置引用该组的任务
 * 2. 在任务仍处于xEventGroupWaitBits状态时删除组会导致未定义行为
 */

#ifdef __cplusplus
}
#endif

#endif /* EVENT_GROUPS_H */



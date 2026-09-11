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

#ifndef MPU_WRAPPERS_H
#define MPU_WRAPPERS_H

/* This file redefines API functions to be called through a wrapper macro, but
only for ports that are using the MPU. */
#ifdef portUSING_MPU_WRAPPERS

	/* MPU_WRAPPERS_INCLUDED_FROM_API_FILE will be defined when this file is
	included from queue.c or task.c to prevent it from having an effect within
	those files. */
	#ifndef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

		/*
		 * Map standard (non MPU) API functions to equivalents that start
		 * "MPU_".  This will cause the application code to call the MPU_
		 * version, which wraps the non-MPU version with privilege promoting
		 * then demoting code, so the kernel code always runs will full
		 * privileges.
		 */

		/* Map standard tasks.h API functions to the MPU equivalents. */
		#define xTaskCreate								MPU_xTaskCreate
		#define xTaskCreateStatic						MPU_xTaskCreateStatic
		#define xTaskCreateRestricted					MPU_xTaskCreateRestricted
		#define vTaskAllocateMPURegions					MPU_vTaskAllocateMPURegions
		#define vTaskDelete								MPU_vTaskDelete
		#define vTaskDelay								MPU_vTaskDelay
		#define vTaskDelayUntil							MPU_vTaskDelayUntil
		#define xTaskAbortDelay							MPU_xTaskAbortDelay
		#define uxTaskPriorityGet						MPU_uxTaskPriorityGet
		#define eTaskGetState							MPU_eTaskGetState
		#define vTaskGetInfo							MPU_vTaskGetInfo
		#define vTaskPrioritySet						MPU_vTaskPrioritySet
		#define vTaskSuspend							MPU_vTaskSuspend
		#define vTaskResume								MPU_vTaskResume
		#define vTaskSuspendAll							MPU_vTaskSuspendAll
		#define xTaskResumeAll							MPU_xTaskResumeAll
		#define xTaskGetTickCount						MPU_xTaskGetTickCount
		#define uxTaskGetNumberOfTasks					MPU_uxTaskGetNumberOfTasks
		#define pcTaskGetName							MPU_pcTaskGetName
		#define xTaskGetHandle							MPU_xTaskGetHandle
		#define uxTaskGetStackHighWaterMark				MPU_uxTaskGetStackHighWaterMark
		#define vTaskSetApplicationTaskTag				MPU_vTaskSetApplicationTaskTag
		#define xTaskGetApplicationTaskTag				MPU_xTaskGetApplicationTaskTag
		#define vTaskSetThreadLocalStoragePointer		MPU_vTaskSetThreadLocalStoragePointer
		#define pvTaskGetThreadLocalStoragePointer		MPU_pvTaskGetThreadLocalStoragePointer
		#define xTaskCallApplicationTaskHook			MPU_xTaskCallApplicationTaskHook
		#define xTaskGetIdleTaskHandle					MPU_xTaskGetIdleTaskHandle
		#define uxTaskGetSystemState					MPU_uxTaskGetSystemState
		#define vTaskList								MPU_vTaskList
		#define vTaskGetRunTimeStats					MPU_vTaskGetRunTimeStats
		#define xTaskGenericNotify						MPU_xTaskGenericNotify
		#define xTaskNotifyWait							MPU_xTaskNotifyWait
		#define ulTaskNotifyTake						MPU_ulTaskNotifyTake
		#define xTaskNotifyStateClear					MPU_xTaskNotifyStateClear

		#define xTaskGetCurrentTaskHandle				MPU_xTaskGetCurrentTaskHandle
		#define vTaskSetTimeOutState					MPU_vTaskSetTimeOutState
		#define xTaskCheckForTimeOut					MPU_xTaskCheckForTimeOut
		#define xTaskGetSchedulerState					MPU_xTaskGetSchedulerState

		/* Map standard queue.h API functions to the MPU equivalents. */
		#define xQueueGenericSend						MPU_xQueueGenericSend
		#define xQueueGenericReceive					MPU_xQueueGenericReceive
		#define uxQueueMessagesWaiting					MPU_uxQueueMessagesWaiting
		#define uxQueueSpacesAvailable					MPU_uxQueueSpacesAvailable
		#define vQueueDelete							MPU_vQueueDelete
		#define xQueueCreateMutex						MPU_xQueueCreateMutex
		#define xQueueCreateMutexStatic					MPU_xQueueCreateMutexStatic
		#define xQueueCreateCountingSemaphore			MPU_xQueueCreateCountingSemaphore
		#define xQueueCreateCountingSemaphoreStatic		MPU_xQueueCreateCountingSemaphoreStatic
		#define xQueueGetMutexHolder					MPU_xQueueGetMutexHolder
		#define xQueueTakeMutexRecursive				MPU_xQueueTakeMutexRecursive
		#define xQueueGiveMutexRecursive				MPU_xQueueGiveMutexRecursive
		#define xQueueGenericCreate						MPU_xQueueGenericCreate
		#define xQueueGenericCreateStatic				MPU_xQueueGenericCreateStatic
		#define xQueueCreateSet							MPU_xQueueCreateSet
		#define xQueueAddToSet							MPU_xQueueAddToSet
		#define xQueueRemoveFromSet						MPU_xQueueRemoveFromSet
		#define xQueueSelectFromSet						MPU_xQueueSelectFromSet
		#define xQueueGenericReset						MPU_xQueueGenericReset

		#if( configQUEUE_REGISTRY_SIZE > 0 )
			#define vQueueAddToRegistry						MPU_vQueueAddToRegistry
			#define vQueueUnregisterQueue					MPU_vQueueUnregisterQueue
			#define pcQueueGetName							MPU_pcQueueGetName
		#endif

		/* Map standard timer.h API functions to the MPU equivalents. */
		#define xTimerCreate							MPU_xTimerCreate
		#define xTimerCreateStatic						MPU_xTimerCreateStatic
		#define pvTimerGetTimerID						MPU_pvTimerGetTimerID
		#define vTimerSetTimerID						MPU_vTimerSetTimerID
		#define xTimerIsTimerActive						MPU_xTimerIsTimerActive
		#define xTimerGetTimerDaemonTaskHandle			MPU_xTimerGetTimerDaemonTaskHandle
		#define xTimerPendFunctionCall					MPU_xTimerPendFunctionCall
		#define pcTimerGetName							MPU_pcTimerGetName
		#define xTimerGetPeriod							MPU_xTimerGetPeriod
		#define xTimerGetExpiryTime						MPU_xTimerGetExpiryTime
		#define xTimerGenericCommand					MPU_xTimerGenericCommand

		/* Map standard event_group.h API functions to the MPU equivalents. */
		#define xEventGroupCreate						MPU_xEventGroupCreate
		#define xEventGroupCreateStatic					MPU_xEventGroupCreateStatic
		#define xEventGroupWaitBits						MPU_xEventGroupWaitBits
		#define xEventGroupClearBits					MPU_xEventGroupClearBits
		#define xEventGroupSetBits						MPU_xEventGroupSetBits
		#define xEventGroupSync							MPU_xEventGroupSync
		#define vEventGroupDelete						MPU_vEventGroupDelete

		/* Remove the privileged function macro. */
		#define PRIVILEGED_FUNCTION

	#else /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

		/* Ensure API functions go in the privileged execution section. */
		#define PRIVILEGED_FUNCTION __attribute__((section("privileged_functions")))
		#define PRIVILEGED_DATA __attribute__((section("privileged_data")))

	#endif /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

#else /* portUSING_MPU_WRAPPERS */
	/* 当未启用MPU封装时的默认宏定义 */
	#define PRIVILEGED_FUNCTION    /* 空定义：当MPU未启用时，函数不需要特权属性修饰 */
	#define PRIVILEGED_DATA        /* 空定义：当MPU未启用时，数据不需要特殊内存段标记 */
	#define portUSING_MPU_WRAPPERS 0 /* MPU封装开关：0表示禁用MPU内存保护封装机制 */

	/*-------------------------- 详细说明 --------------------------*/
	/*
	* 此代码段属于MPU配置的条件编译分支，当portUSING_MPU_WRAPPERS被定义为0时：
	* 
	* PRIVILEGED_FUNCTION - 用于修饰需要特权权限访问的函数
	*   启用MPU时：可能扩展为__attribute__((section("privileged_functions")))
	*   当前状态：无修饰（MPU保护未激活）
	* 
	* PRIVILEGED_DATA - 用于标记需要特权权限访问的全局变量
	*   启用MPU时：可能扩展为__attribute__((section("privileged_data")))
	*   当前状态：无修饰（数据无需特殊保护）
	* 
	* portUSING_MPU_WRAPPERS - 系统级MPU功能开关
	*   0 = 禁用MPU封装层，所有API直接访问内存
	*   1 = 启用MPU封装，增加权限校验和保护措施
	* 
	* 此配置影响：
	* 1. 所有FreeRTOS API函数不会进行MPU权限校验
	* 2. 内存访问不会触发MPU错误异常
	* 3. 任务不需要定义MPU内存区域
	* 4. 系统运行在单一特权模式
	*/

#endif /* portUSING_MPU_WRAPPERS */


#endif /* MPU_WRAPPERS_H */


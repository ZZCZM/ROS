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

/*
 * When the MPU is used the standard (non MPU) API functions are mapped to
 * equivalents that start "MPU_", the prototypes for which are defined in this
 * header files.  This will cause the application code to call the MPU_ version
 * which wraps the non-MPU version with privilege promoting then demoting code,
 * so the kernel code always runs will full privileges.
 */


#ifndef MPU_PROTOTYPES_H
#define MPU_PROTOTYPES_H
/* MPU versions of tasks.h API function. */

/**
 * @brief 创建MPU保护的任务（动态内存版本）
 * @param pxTaskCode 任务函数指针，指向任务入口函数
 * @param pcName 任务名称字符串（用于调试）
 * @param usStackDepth 任务堆栈深度（以字为单位）
 * @param pvParameters 传递给任务的参数指针
 * @param uxPriority 任务优先级（0为最低优先级）
 * @param pxCreatedTask 输出参数，接收创建的任务句柄
 * @return 创建状态：pdPASS表示成功，pdFAIL表示失败
 */
BaseType_t MPU_xTaskCreate( 
    TaskFunction_t pxTaskCode,        // 任务函数指针（函数原型：void vTaskFunction(void *pvParameters)）
    const char * const pcName,        // 任务可读名称（最大长度由configMAX_TASK_NAME_LEN定义）
    const uint16_t usStackDepth,      // 堆栈大小（单位取决于架构，通常为uint32_t的字大小）
    void * const pvParameters,        // 任务参数指针（可通过pvTaskGetThreadLocalStorage访问）
    UBaseType_t uxPriority,           // 优先级编号（0最低，configMAX_PRIORITIES-1最高）
    TaskHandle_t * const pxCreatedTask // 输出参数，接收新创建任务的句柄
);

/**
 * @brief 创建MPU保护的任务（静态内存版本）
 * @param pxTaskCode 任务函数指针
 * @param pcName 任务名称字符串
 * @param ulStackDepth 任务堆栈深度（以字为单位）
 * @param pvParameters 任务参数指针
 * @param uxPriority 任务优先级
 * @param puxStackBuffer 预分配的堆栈内存缓冲区指针
 * @param pxTaskBuffer 预分配的任务控制块内存指针
 * @return 成功时返回任务句柄，失败返回NULL
 */
TaskHandle_t MPU_xTaskCreateStatic(
    TaskFunction_t pxTaskCode,        // 任务入口函数指针
    const char * const pcName,        // 任务描述性名称（允许用于调试）
    const uint32_t ulStackDepth,      // 堆栈大小（必须足够大以防止溢出）
    void * const pvParameters,        // 传递给任务函数的参数
    UBaseType_t uxPriority,           // 任务优先级（数值越大优先级越高）
    StackType_t * const puxStackBuffer, // 静态分配的堆栈存储区（大小需 >= ulStackDepth）
    StaticTask_t * const pxTaskBuffer  // 静态分配的任务控制块内存
);

/**
 * @brief 创建受MPU保护的受限任务
 * @param pxTaskDefinition 任务参数结构体指针
 * @param pxCreatedTask 输出参数，接收任务句柄
 * @return 创建状态：pdPASS成功，pdFAIL失败
 */
BaseType_t MPU_xTaskCreateRestricted( 
    const TaskParameters_t * const pxTaskDefinition, // 包含内存区域、堆栈、优先级的结构体
    TaskHandle_t *pxCreatedTask        // 输出参数，接收新任务句柄
);

/**
 * @brief 为MPU任务分配内存区域
 * @param xTask 目标任务的句柄
 * @param pxRegions 内存区域配置数组指针
 */
void MPU_vTaskAllocateMPURegions(
    TaskHandle_t xTask,               // 要配置的任务句柄（NULL表示当前任务）
    const MemoryRegion_t * const pxRegions // 内存区域配置数组（必须包含有效MPU配置）
);

/**
 * @brief 删除MPU保护的任务
 * @param xTaskToDelete 要删除的任务句柄
 */
void MPU_vTaskDelete(
    TaskHandle_t xTaskToDelete        // 要删除的任务句柄（传递NULL删除当前任务）
);

/**
 * @brief 延迟任务执行（MPU版本）
 * @param xTicksToDelay 要延迟的时钟节拍数
 */
void MPU_vTaskDelay(
    const TickType_t xTicksToDelay    // 延迟时长（以系统时钟节拍为单位）
);

/**
 * @brief 精确周期延迟（MPU版本）
 * @param pxPreviousWakeTime 指向上次唤醒时间的指针
 * @param xTimeIncrement 固定周期时间（节拍数）
 */
void MPU_vTaskDelayUntil(
    TickType_t * const pxPreviousWakeTime, // 需要保持的变量地址（用于维护唤醒时间基准）
    const TickType_t xTimeIncrement  // 期望的固定执行周期（节拍数）
);

/**
 * @brief 中止任务的延迟状态
 * @param xTask 目标任务的句柄
 * @return 操作结果：pdPASS成功，pdFAIL失败
 */
BaseType_t MPU_xTaskAbortDelay(
    TaskHandle_t xTask               // 要取消延迟状态的任务句柄
);

/**
 * @brief 获取任务优先级（MPU版本）
 * @param xTask 目标任务的句柄
 * @return 当前优先级数值
 */
UBaseType_t MPU_uxTaskPriorityGet(
    TaskHandle_t xTask               // 要查询的任务句柄（NULL表示当前任务）
);

/**
 * @brief 获取任务状态（MPU版本）
 * @param xTask 目标任务的句柄
 * @return 任务状态枚举值
 */
eTaskState MPU_eTaskGetState(
    TaskHandle_t xTask               // 要查询的任务句柄
);

/**
 * @brief 获取任务信息（MPU版本）
 * @param xTask 目标任务的句柄
 * @param pxTaskStatus 输出参数，接收任务状态信息
 * @param xGetFreeStackSpace 是否计算剩余堆栈空间
 * @param eState 任务状态过滤参数
 */
void MPU_vTaskGetInfo(
    TaskHandle_t xTask,              // 要查询的任务句柄（NULL表示当前任务）
    TaskStatus_t *pxTaskStatus,      // 输出参数，接收任务状态结构体
    BaseType_t xGetFreeStackSpace,   // 是否计算剩余堆栈标志（pdTRUE/pdFALSE）
    eTaskState eState                // 状态过滤参数（eInvalid/eRunning等）
);

/**
 * @brief 设置任务优先级（MPU版本）
 * @param xTask 目标任务的句柄
 * @param uxNewPriority 新的优先级数值
 */
void MPU_vTaskPrioritySet(
    TaskHandle_t xTask,              // 要设置的任务句柄（NULL表示当前任务）
    UBaseType_t uxNewPriority        // 新的优先级数值（必须有效）
);

/**
 * @brief 挂起指定任务（MPU版本）
 * @param xTaskToSuspend 要挂起的任务句柄
 */
void MPU_vTaskSuspend(
    TaskHandle_t xTaskToSuspend      // 要挂起的任务句柄（不能为NULL）
);

/**
 * @brief 恢复挂起的任务（MPU版本）
 * @param xTaskToResume 要恢复的任务句柄
 */
void MPU_vTaskResume(
    TaskHandle_t xTaskToResume        // 要恢复执行的任务句柄
);

/**
 * @brief 启动MPU保护的调度器
 */
void MPU_vTaskStartScheduler( void ); // 无参数，启动内核调度

/**
 * @brief 挂起所有任务（MPU版本）
 */
void MPU_vTaskSuspendAll( void );     // 暂停调度器，保持中断响应

/**
 * @brief 恢复所有任务调度（MPU版本）
 * @return 恢复前的挂起计数（如果嵌套挂起时需要）
 */
BaseType_t MPU_xTaskResumeAll( void ); // 恢复调度，返回恢复前的挂起嵌套计数

/**
 * @brief 获取系统时钟计数（MPU版本）
 * @return 当前系统节拍计数
 */
TickType_t MPU_xTaskGetTickCount( void ); // 返回自启动以来的时钟节拍数

/**
 * @brief 获取当前任务总数（MPU版本）
 * @return 系统中存在的任务总数
 */
UBaseType_t MPU_uxTaskGetNumberOfTasks( void ); // 包含所有状态（就绪、阻塞、挂起）的任务计数

/**
 * @brief 获取任务名称（MPU版本）
 * @param xTaskToQuery 目标任务的句柄
 * @return 任务名称字符串指针
 */
char * MPU_pcTaskGetName(
    TaskHandle_t xTaskToQuery         // 要查询的任务句柄（NULL表示当前任务）
);
/**
 * @brief 通过任务名称获取任务句柄（MPU版本）
 * @param pcNameToQuery 要查询的任务名称字符串
 * @return 对应的任务句柄（未找到返回NULL）
 */
TaskHandle_t MPU_xTaskGetHandle( 
    const char *pcNameToQuery        // 精确匹配的任务名称（大小写敏感）
);

/**
 * @brief 获取任务堆栈高水位线（MPU版本）
 * @param xTask 目标任务的句柄
 * @return 堆栈最小剩余空间（单位取决于架构）
 */
UBaseType_t MPU_uxTaskGetStackHighWaterMark(
    TaskHandle_t xTask               // 要检测的任务句柄（NULL表示当前任务）
);

/**
 * @brief 设置应用任务标签（MPU版本）
 * @param xTask 目标任务的句柄
 * @param pxHookFunction 要设置的标签函数指针
 */
void MPU_vTaskSetApplicationTaskTag(
    TaskHandle_t xTask,              // 要设置的任务句柄（NULL表示当前任务）
    TaskHookFunction_t pxHookFunction // 应用定义的标签函数（可用于调试跟踪）
);

/**
 * @brief 获取应用任务标签（MPU版本）
 * @param xTask 目标任务的句柄
 * @return 当前设置的标签函数指针
 */
TaskHookFunction_t MPU_xTaskGetApplicationTaskTag(
    TaskHandle_t xTask               // 要查询的任务句柄（NULL表示当前任务）
);

/**
 * @brief 设置线程本地存储指针（MPU版本）
 * @param xTaskToSet 目标任务的句柄
 * @param xIndex 存储索引位置（0~configNUM_THREAD_LOCAL_STORAGE_POINTERS-1）
 * @param pvValue 要存储的指针值
 */
void MPU_vTaskSetThreadLocalStoragePointer(
    TaskHandle_t xTaskToSet,         // 要设置的任务句柄（NULL表示当前任务）
    BaseType_t xIndex,               // 存储槽索引（从0开始）
    void *pvValue                    // 要存储的任意类型指针
);

/**
 * @brief 获取线程本地存储指针（MPU版本）
 * @param xTaskToQuery 目标任务的句柄
 * @param xIndex 存储索引位置
 * @return 存储的指针值
 */
void * MPU_pvTaskGetThreadLocalStoragePointer(
    TaskHandle_t xTaskToQuery,       // 要查询的任务句柄（NULL表示当前任务）
    BaseType_t xIndex                // 存储槽索引（必须有效）
);

/**
 * @brief 调用应用任务钩子函数（MPU版本）
 * @param xTask 目标任务的句柄
 * @param pvParameter 传递给钩子函数的参数
 * @return 钩子函数的返回值
 */
BaseType_t MPU_xTaskCallApplicationTaskHook(
    TaskHandle_t xTask,              // 要调用钩子的任务句柄
    void *pvParameter                // 传递给任务钩子的参数
);

/**
 * @brief 获取空闲任务句柄（MPU版本）
 * @return 空闲任务的任务句柄
 */
TaskHandle_t MPU_xTaskGetIdleTaskHandle( void ); // 始终返回系统空闲任务句柄

/**
 * @brief 获取系统任务状态快照（MPU版本）
 * @param pxTaskStatusArray 任务状态数组指针
 * @param uxArraySize 数组元素数量
 * @param pulTotalRunTime 总运行时间统计（需要configGENERATE_RUN_TIME_STATS启用）
 * @return 实际填充的任务数量
 */
UBaseType_t MPU_uxTaskGetSystemState(
    TaskStatus_t * const pxTaskStatusArray, // 输出缓冲区（需足够存储所有任务）
    const UBaseType_t uxArraySize,         // 数组容量（建议用uxTaskGetNumberOfTasks获取数量）
    uint32_t * const pulTotalRunTime       // 输出总运行时间（单位由portCONFIGURE_TIMER_FOR_RUN_TIME_STATS定义）
);

/**
 * @brief 生成任务状态列表（MPU版本）
 * @param pcWriteBuffer 输出缓冲区（需足够大，建议最小40字节/任务）
 */
void MPU_vTaskList(
    char * pcWriteBuffer             // 输出缓冲区（生成人类可读的任务列表）
);

/**
 * @brief 生成任务运行时间统计（MPU版本）
 * @param pcWriteBuffer 输出缓冲区（需足够大，建议最小40字节/任务）
 */
void MPU_vTaskGetRunTimeStats(
    char *pcWriteBuffer              // 输出缓冲区（生成带运行时间百分比的数据）
);

/**
 * @brief 通用任务通知发送（MPU版本）
 * @param xTaskToNotify 目标任务的句柄
 * @param ulValue 通知值
 * @param eAction 通知操作类型
 * @param pulPreviousNotificationValue 输出先前通知值
 * @return 发送结果：pdPASS成功，pdFAIL失败
 */
BaseType_t MPU_xTaskGenericNotify(
    TaskHandle_t xTaskToNotify,      // 要通知的任务句柄（不能为NULL）
    uint32_t ulValue,                // 通知数据（最大32位）
    eNotifyAction eAction,           // 操作类型（eSetValueWith/WithoutOverwrite等）
    uint32_t *pulPreviousNotificationValue // 输出参数，接收前一个通知值（可NULL）
);

/**
 * @brief 等待任务通知（MPU版本）
 * @param ulBitsToClearOnEntry 进入时清除的位掩码
 * @param ulBitsToClearOnExit 退出时清除的位掩码
 * @param pulNotificationValue 输出接收到的通知值
 * @param xTicksToWait 最大等待时间（节拍数）
 * @return 等待结果：pdTRUE收到通知，pdFALSE超时
 */
BaseType_t MPU_xTaskNotifyWait(
    uint32_t ulBitsToClearOnEntry,   // 进入前清除的通知位（按位操作）
    uint32_t ulBitsToClearOnExit,    // 退出前清除的通知位（按位操作）
    uint32_t *pulNotificationValue,  // 输出参数，接收通知值
    TickType_t xTicksToWait          // 最大阻塞时间（portMAX_DELAY表示无限等待）
);

/**
 * @brief 获取任务通知值（MPU版本）
 * @param xClearCountOnExit 退出时是否清零计数器
 * @param xTicksToWait 最大等待时间
 * @return 接收到的通知计数值
 */
uint32_t MPU_ulTaskNotifyTake(
    BaseType_t xClearCountOnExit,    // 退出时清零标志（pdTRUE清0，pdFALSE减1）
    TickType_t xTicksToWait          // 最大阻塞时间（节拍数）
);

/**
 * @brief 清除任务通知状态（MPU版本）
 * @param xTask 目标任务的句柄
 * @return 操作结果：pdPASS成功，pdFAIL失败
 */
BaseType_t MPU_xTaskNotifyStateClear(
    TaskHandle_t xTask               // 要清除状态的任务句柄（NULL表示当前任务）
);

/**
 * @brief 递增系统节拍计数器（MPU版本）
 * @return 是否需要上下文切换
 */
BaseType_t MPU_xTaskIncrementTick( void ); // 通常在时钟中断中调用，返回pdTRUE表示需要调度

/**
 * @brief 获取当前任务句柄（MPU版本）
 * @return 当前运行任务的任务句柄
 */
TaskHandle_t MPU_xTaskGetCurrentTaskHandle( void ); // 在中断中调用返回NULL

/**
 * @brief 设置超时状态（MPU版本）
 * @param pxTimeOut 超时状态结构指针
 */
void MPU_vTaskSetTimeOutState(
    TimeOut_t * const pxTimeOut      // 要初始化的超时结构体（记录当前时间）
);

/**
 * @brief 检查超时状态（MPU版本）
 * @param pxTimeOut 超时状态结构指针
 * @param pxTicksToWait 剩余等待时间指针
 * @return 是否超时：pdTRUE超时，pdFALSE未超时
 */
BaseType_t MPU_xTaskCheckForTimeOut(
    TimeOut_t * const pxTimeOut,     // 之前记录的超时结构体
    TickType_t * const pxTicksToWait // 输入输出参数，更新剩余等待时间
);

/**
 * @brief 通知可能遗漏的任务切换（MPU版本）
 */
void MPU_vTaskMissedYield( void );   // 在中断服务程序中请求上下文切换

/**
 * @brief 获取调度器状态（MPU版本）
 * @return 调度状态：taskSCHEDULER_RUNNING等
 */
BaseType_t MPU_xTaskGetSchedulerState( void ); // 返回值需用eTaskState枚举判断
/* MPU versions of queue.h API function. */

/**
 * @brief 通用队列发送（MPU版本）
 * @param xQueue 队列句柄
 * @param pvItemToQueue 要发送的数据指针
 * @param xTicksToWait 最大阻塞时间
 * @param xCopyPosition 发送位置（queueSEND_TO_BACK等）
 * @return 操作结果：pdPASS成功，errQUEUE_FULL等错误码
 */
BaseType_t MPU_xQueueGenericSend( 
    QueueHandle_t xQueue,            // 目标队列句柄（必须已创建）
    const void * const pvItemToQueue, // 发送数据指针（需符合队列项大小）
    TickType_t xTicksToWait,         // 最大等待时间（portMAX_DELAY表示无限等待）
    const BaseType_t xCopyPosition   // 发送位置：queueSEND_TO_BACK/FRONT/OVERWRITE
);

/**
 * @brief 通用队列接收（MPU版本）
 * @param xQueue 队列句柄
 * @param pvBuffer 接收缓冲区指针
 * @param xTicksToWait 最大阻塞时间
 * @param xJustPeek 是否仅查看不取出
 * @return 操作结果：pdPASS成功，errQUEUE_EMPTY等错误码
 */
BaseType_t MPU_xQueueGenericReceive(
    QueueHandle_t xQueue,            // 目标队列句柄
    void * const pvBuffer,           // 接收缓冲区（需足够存储队列项）
    TickType_t xTicksToWait,         // 阻塞时间（0表示不阻塞）
    const BaseType_t xJustPeek       // 是否保留数据：pdTRUE仅查看，pdFALSE取出
);

/**
 * @brief 获取队列当前消息数（MPU版本）
 * @param xQueue 队列句柄
 * @return 队列中当前消息数量
 */
UBaseType_t MPU_uxQueueMessagesWaiting(
    const QueueHandle_t xQueue       // 要查询的队列句柄（必须有效）
);

/**
 * @brief 获取队列剩余空间（MPU版本）
 * @param xQueue 队列句柄
 * @return 队列剩余可存储项数量
 */
UBaseType_t MPU_uxQueueSpacesAvailable(
    const QueueHandle_t xQueue       // 要查询的队列句柄
);

/**
 * @brief 删除队列（MPU版本）
 * @param xQueue 要删除的队列句柄
 */
void MPU_vQueueDelete(
    QueueHandle_t xQueue             // 要销毁的队列句柄（删除后不可再使用）
);

/**
 * @brief 创建互斥量（MPU版本）
 * @param ucQueueType 队列类型标识
 * @return 新创建的互斥量句柄
 */
QueueHandle_t MPU_xQueueCreateMutex(
    const uint8_t ucQueueType        // 必须为queueQUEUE_TYPE_MUTEX
);

/**
 * @brief 静态创建互斥量（MPU版本）
 * @param ucQueueType 队列类型标识
 * @param pxStaticQueue 静态分配的内存结构
 * @return 互斥量句柄
 */
QueueHandle_t MPU_xQueueCreateMutexStatic(
    const uint8_t ucQueueType,       // 必须为queueQUEUE_TYPE_MUTEX
    StaticQueue_t *pxStaticQueue     // 预分配的队列控制块内存
);

/**
 * @brief 创建计数信号量（MPU版本）
 * @param uxMaxCount 最大计数值
 * @param uxInitialCount 初始计数值
 * @return 新创建的信号量句柄
 */
QueueHandle_t MPU_xQueueCreateCountingSemaphore(
    const UBaseType_t uxMaxCount,    // 信号量最大计数值（必须>0）
    const UBaseType_t uxInitialCount // 初始计数值（必须≤uxMaxCount）
);

/**
 * @brief 静态创建计数信号量（MPU版本）
 * @param uxMaxCount 最大计数值
 * @param uxInitialCount 初始计数值
 * @param pxStaticQueue 静态分配的内存结构
 * @return 信号量句柄
 */
QueueHandle_t MPU_xQueueCreateCountingSemaphoreStatic(
    const UBaseType_t uxMaxCount,    // 信号量最大计数值
    const UBaseType_t uxInitialCount, // 初始计数值
    StaticQueue_t *pxStaticQueue     // 预分配的静态队列结构
);

/**
 * @brief 获取互斥量持有者（MPU版本）
 * @param xSemaphore 互斥量句柄
 * @return 当前持有任务句柄（未被持有时返回NULL）
 */
void* MPU_xQueueGetMutexHolder(
    QueueHandle_t xSemaphore         // 互斥量句柄（必须为Mutex类型）
);

/**
 * @brief 递归获取互斥量（MPU版本）
 * @param xMutex 互斥量句柄
 * @param xTicksToWait 最大等待时间
 * @return 获取结果：pdPASS成功，pdFAIL超时
 */
BaseType_t MPU_xQueueTakeMutexRecursive(
    QueueHandle_t xMutex,            // 递归互斥量句柄
    TickType_t xTicksToWait          // 阻塞时间（仅第一次获取时生效）
);

/**
 * @brief 递归释放互斥量（MPU版本）
 * @param pxMutex 互斥量句柄
 * @return 释放结果：pdPASS成功，pdFAIL错误
 */
BaseType_t MPU_xQueueGiveMutexRecursive(
    QueueHandle_t pxMutex            // 递归互斥量句柄
);

/**
 * @brief 注册队列到内核（MPU版本）
 * @param xQueue 队列句柄
 * @param pcName 注册名称
 */
void MPU_vQueueAddToRegistry(
    QueueHandle_t xQueue,            // 要注册的队列句柄
    const char *pcName               // 注册名称（用于调试查看）
);

/**
 * @brief 取消队列注册（MPU版本）
 * @param xQueue 队列句柄
 */
void MPU_vQueueUnregisterQueue(
    QueueHandle_t xQueue             // 要取消注册的队列句柄
);

/**
 * @brief 获取队列注册名称（MPU版本）
 * @param xQueue 队列句柄
 * @return 注册名称字符串指针
 */
const char * MPU_pcQueueGetName(
    QueueHandle_t xQueue             // 已注册的队列句柄
);

/**
 * @brief 通用队列创建（MPU版本）
 * @param uxQueueLength 队列长度
 * @param uxItemSize 队列项大小（字节）
 * @param ucQueueType 队列类型标识
 * @return 新队列句柄
 */
QueueHandle_t MPU_xQueueGenericCreate(
    const UBaseType_t uxQueueLength, // 队列最大容量
    const UBaseType_t uxItemSize,    // 每个队列项的大小（0表示无存储）
    const uint8_t ucQueueType        // 队列类型：queueQUEUE_TYPE_BASE等
);

/**
 * @brief 静态创建通用队列（MPU版本）
 * @param uxQueueLength 队列长度
 * @param uxItemSize 队列项大小
 * @param pucQueueStorage 静态存储区指针
 * @param pxStaticQueue 静态控制块指针
 * @param ucQueueType 队列类型标识
 * @return 新队列句柄
 */
QueueHandle_t MPU_xQueueGenericCreateStatic(
    const UBaseType_t uxQueueLength, // 队列容量
    const UBaseType_t uxItemSize,    // 队列项字节大小
    uint8_t *pucQueueStorage,        // 预分配的存储区（大小=uxQueueLength*uxItemSize）
    StaticQueue_t *pxStaticQueue,    // 预分配的队列控制块
    const uint8_t ucQueueType        // 队列类型标识
);

/**
 * @brief 创建队列集合（MPU版本）
 * @param uxEventQueueLength 集合容量
 * @return 新队列集合句柄
 */
QueueSetHandle_t MPU_xQueueCreateSet(
    const UBaseType_t uxEventQueueLength // 最大同时等待队列数
);

/**
 * @brief 添加队列到集合（MPU版本）
 * @param xQueueOrSemaphore 队列/信号量句柄
 * @param xQueueSet 目标集合句柄
 * @return 添加结果：pdPASS成功，pdFAIL失败
 */
BaseType_t MPU_xQueueAddToSet(
    QueueSetMemberHandle_t xQueueOrSemaphore, // 要添加的队列/信号量
    QueueSetHandle_t xQueueSet      // 目标集合句柄
);

/**
 * @brief 从集合移除队列（MPU版本）
 * @param xQueueOrSemaphore 队列/信号量句柄
 * @param xQueueSet 目标集合句柄
 * @return 移除结果：pdPASS成功，pdFAIL失败
 */
BaseType_t MPU_xQueueRemoveFromSet(
    QueueSetMemberHandle_t xQueueOrSemaphore, // 要移除的队列/信号量
    QueueSetHandle_t xQueueSet      // 目标集合句柄
);

/**
 * @brief 从集合中选择就绪队列（MPU版本）
 * @param xQueueSet 队列集合句柄
 * @param xTicksToWait 最大等待时间
 * @return 就绪的队列句柄（超时返回NULL）
 */
QueueSetMemberHandle_t MPU_xQueueSelectFromSet(
    QueueSetHandle_t xQueueSet,      // 要监听的集合句柄
    const TickType_t xTicksToWait    // 阻塞时间（portMAX_DELAY表示无限等待）
);

/**
 * @brief 重置队列状态（MPU版本）
 * @param xQueue 队列句柄
 * @param xNewQueue 是否作为新队列重置
 * @return 重置结果：pdPASS成功，pdFAIL失败
 */
BaseType_t MPU_xQueueGenericReset(
    QueueHandle_t xQueue,            // 要重置的队列句柄
    BaseType_t xNewQueue             // pdTRUE清空所有数据，pdFALSE仅重置状态
);

/**
 * @brief 设置队列编号（MPU版本）
 * @param xQueue 队列句柄
 * @param uxQueueNumber 自定义队列编号
 */
void MPU_vQueueSetQueueNumber(
    QueueHandle_t xQueue,            // 目标队列句柄
    UBaseType_t uxQueueNumber        // 用户自定义标识编号
);

/**
 * @brief 获取队列编号（MPU版本）
 * @param xQueue 队列句柄
 * @return 预设的队列编号
 */
UBaseType_t MPU_uxQueueGetQueueNumber(
    QueueHandle_t xQueue             // 要查询的队列句柄
);

/**
 * @brief 获取队列类型（MPU版本）
 * @param xQueue 队列句柄
 * @return 类型标识：queueQUEUE_TYPE_*系列常量
 */
uint8_t MPU_ucQueueGetQueueType(
    QueueHandle_t xQueue             // 要查询的队列句柄
);
/* MPU versions of timers.h API function. */

/**
 * @brief 创建定时器（MPU版本）
 * @param pcTimerName 定时器名称
 * @param xTimerPeriodInTicks 定时周期（节拍数）
 * @param uxAutoReload 是否自动重载
 * @param pvTimerID 定时器标识指针
 * @param pxCallbackFunction 回调函数
 * @return 定时器句柄
 */
TimerHandle_t MPU_xTimerCreate(
    const char * const pcTimerName,  // 定时器描述名称（用于调试）
    const TickType_t xTimerPeriodInTicks, // 周期时间（单位与系统节拍一致）
    const UBaseType_t uxAutoReload,  // 自动重载：pdTRUE周期，pdFALSE单次
    void * const pvTimerID,          // 用户自定义标识指针（可多个定时器共享）
    TimerCallbackFunction_t pxCallbackFunction // 回调函数原型：void func(TimerHandle_t)
);

/**
 * @brief 静态创建定时器（MPU版本）
 * @param pxTimerBuffer 静态分配的内存结构
 * @return 定时器句柄
 */
TimerHandle_t MPU_xTimerCreateStatic(
    const char * const pcTimerName, 
    const TickType_t xTimerPeriodInTicks,
    const UBaseType_t uxAutoReload,
    void * const pvTimerID,
    TimerCallbackFunction_t pxCallbackFunction,
    StaticTimer_t *pxTimerBuffer     // 预分配的静态定时器结构
);

/**
 * @brief 获取定时器标识（MPU版本）
 * @param xTimer 定时器句柄
 * @return 用户自定义标识指针
 */
void * MPU_pvTimerGetTimerID(
    const TimerHandle_t xTimer       // 要查询的定时器句柄
);

/**
 * @brief 设置定时器标识（MPU版本）
 * @param pvNewID 新标识指针
 */
void MPU_vTimerSetTimerID(
    TimerHandle_t xTimer,            // 目标定时器句柄
    void *pvNewID                   // 新的用户标识指针
);

/**
 * @brief 检查定时器是否激活（MPU版本）
 * @return 激活状态：pdTRUE运行中，pdFALSE未启动
 */
BaseType_t MPU_xTimerIsTimerActive(
    TimerHandle_t xTimer             // 要检测的定时器句柄
);

/**
 * @brief 获取定时器守护任务句柄（MPU版本）
 * @return 守护任务句柄（用于优先级调整）
 */
TaskHandle_t MPU_xTimerGetTimerDaemonTaskHandle( void ); // 守护任务负责执行定时回调

/**
 * @brief 提交延迟函数调用（MPU版本）
 * @param xFunctionToPend 待执行函数
 * @param pvParameter1 参数1
 * @param ulParameter2 参数2
 * @param xTicksToWait 队列满时最大等待时间
 * @return 提交结果：pdPASS成功，pdFAIL超时
 */
BaseType_t MPU_xTimerPendFunctionCall(
    PendedFunction_t xFunctionToPend, // 函数原型：void func(void*,uint32_t)
    void *pvParameter1,             // 通用参数指针
    uint32_t ulParameter2,          // 附加参数（32位）
    TickType_t xTicksToWait         // 队列满时的阻塞时间
);

/**
 * @brief 获取定时器名称（MPU版本）
 * @return 定时器名称字符串指针
 */
const char * MPU_pcTimerGetName(
    TimerHandle_t xTimer            // 要查询的定时器句柄
);

/**
 * @brief 获取定时器周期（MPU版本）
 * @return 当前设置的周期值（节拍数）
 */
TickType_t MPU_xTimerGetPeriod(
    TimerHandle_t xTimer            // 目标定时器句柄
);

/**
 * @brief 获取下次到期时间（MPU版本）
 * @return 下次触发时刻的节拍计数
 */
TickType_t MPU_xTimerGetExpiryTime(
    TimerHandle_t xTimer            // 目标定时器句柄
);

/**
 * @brief 创建定时器守护任务（MPU版本）
 * @return 创建结果：pdPASS成功，pdFAIL失败
 */
BaseType_t MPU_xTimerCreateTimerTask( void ); // 必须在调度器启动前调用

/**
 * @brief 定时器通用控制命令（MPU版本）
 * @param xCommandID 命令类型（tmrCOMMAND_*系列）
 * @param xOptionalValue 命令参数
 * @param pxHigherPriorityTaskWoken 是否引发任务切换
 * @return 命令执行结果
 */
BaseType_t MPU_xTimerGenericCommand(
    TimerHandle_t xTimer,            // 目标定时器句柄
    const BaseType_t xCommandID,     // 命令：启动(tmrCOMMAND_START)/停止/重置等
    const TickType_t xOptionalValue, // 命令参数（如新周期值）
    BaseType_t * const pxHigherPriorityTaskWoken, // 输出是否引发任务切换（可NULL）
    const TickType_t xTicksToWait    // 命令发送超时时间
);

/* MPU versions of event_group.h API function. */

/**
 * @brief 创建事件组（MPU版本）
 * @return 新事件组句柄
 */
EventGroupHandle_t MPU_xEventGroupCreate( void ); // 返回8/24/30位事件组（取决于配置）

/**
 * @brief 静态创建事件组（MPU版本）
 * @param pxEventGroupBuffer 静态内存结构
 * @return 事件组句柄
 */
EventGroupHandle_t MPU_xEventGroupCreateStatic(
    StaticEventGroup_t *pxEventGroupBuffer // 预分配的事件组控制块
);

/**
 * @brief 等待事件位（MPU版本）
 * @param uxBitsToWaitFor 等待的位掩码
 * @param xClearOnExit 是否清除匹配位
 * @param xWaitForAllBits 是否需所有位同时成立
 * @return 满足的事件位（可能包含未等待的位）
 */
EventBits_t MPU_xEventGroupWaitBits(
    EventGroupHandle_t xEventGroup,  // 目标事件组句柄
    const EventBits_t uxBitsToWaitFor, // 等待的位掩码（按位或组合）
    const BaseType_t xClearOnExit,   // pdTRUE清除匹配位，pdFALSE保持
    const BaseType_t xWaitForAllBits, // pdTRUE需所有位，pdFALSE任一位置位
    TickType_t xTicksToWait          // 最大等待时间
);

/**
 * @brief 清除事件位（MPU版本）
 * @param uxBitsToClear 要清除的位掩码
 * @return 清除后的事件位值
 */
EventBits_t MPU_xEventGroupClearBits(
    EventGroupHandle_t xEventGroup, 
    const EventBits_t uxBitsToClear  // 按位清除的掩码（如0x01清除bit0）
);

/**
 * @brief 设置事件位（MPU版本）
 * @param uxBitsToSet 要设置的位掩码
 * @return 设置后的事件位值
 */
EventBits_t MPU_xEventGroupSetBits(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToSet    // 按位设置的掩码（可多bit或组合）
);

/**
 * @brief 同步事件位（MPU版本）
 * @param uxBitsToSet 要设置的位掩码
 * @param uxBitsToWaitFor 等待的位掩码
 * @return 触发时的事件位值
 */
EventBits_t MPU_xEventGroupSync(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToSet,   // 先设置这些位
    const EventBits_t uxBitsToWaitFor, // 然后等待这些位
    TickType_t xTicksToWait          // 同步超时时间
);

/**
 * @brief 删除事件组（MPU版本）
 */
void MPU_vEventGroupDelete(
    EventGroupHandle_t xEventGroup   // 要销毁的事件组句柄
);

/**
 * @brief 获取事件组编号（MPU版本）
 * @return 事件组唯一编号（用于调试追踪）
 */
UBaseType_t MPU_uxEventGroupGetNumber(
    void* xEventGroup                // 目标事件组句柄
);

#endif /* MPU_PROTOTYPES_H */


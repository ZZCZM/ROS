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


#ifndef INC_TASK_H
#define INC_TASK_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h must appear in source files before include task.h"
#endif

#include "list.h"
#include "semphr.h"
#ifdef __cplusplus
extern "C" {
#endif
/*-----------------------------------------------------------
 * FreeRTOS 核心类型系统定义
 *----------------------------------------------------------*/

/*----------------------- 内核版本信息宏 -----------------------*/
#define tskKERNEL_VERSION_NUMBER "V9.0.0"  // 内核版本字符串（主版本.次版本.修订号）
#define tskKERNEL_VERSION_MAJOR 9         // 主版本号（重大架构变更）
#define tskKERNEL_VERSION_MINOR 0         // 次版本号（功能新增）
#define tskKERNEL_VERSION_BUILD 0         // 构建版本号（问题修复）

/*----------------------- 任务句柄类型定义 ---------------------*/
/**
 * @typedef TaskHandle_t
 * @brief 任务控制块指针的抽象类型
 * 
 * 使用场景：
 * - xTaskCreate()的参数返回
 * - 任务间通信API的参数传递
 * - 任务状态查询
 * 
 * 示例：
 * TaskHandle_t xHandle = NULL;
 * xTaskCreate(vTaskFunction, "Task", configMINIMAL_STACK_SIZE, NULL, 1, &xHandle);
 */
typedef void * TaskHandle_t;

/*--------------------- 任务钩子函数原型定义 --------------------*/
/**
 * @typedef TaskHookFunction_t
 * @brief 任务钩子函数原型（用于trace或监控）
 * 
 * 函数规范：
 * - 在每个任务的时间片结束时调用
 * - 返回pdTRUE可让内核跳过当前任务
 * 
 * 应用场景：
 * 实现自定义的任务执行时间统计
 */
typedef BaseType_t (*TaskHookFunction_t)( void * );

/*--------------------- 任务状态枚举定义 -----------------------*/
/**
 * @enum eTaskState
 * @brief 描述任务生命周期的状态集合
 * 
 * 状态转换示意图：
 * eReady → eRunning → eBlocked → eReady
 *           ↑          ↓
 *           └── eSuspended
 */
typedef enum
{
    eRunning = 0,   // 正在运行（仅查询自身状态时可能返回）
    eReady,         // 就绪状态（等待调度）
    eBlocked,       // 阻塞状态（等待信号量/队列/事件等）
    eSuspended,    // 挂起状态（通过vTaskSuspend()）
    eDeleted,       // 已删除但未清理TCB
    eInvalid        // 无效状态（查询不存在的任务时返回）
} eTaskState;

/*--------------------- 任务通知动作枚举定义 --------------------*/
/**
 * @enum eNotifyAction
 * @brief 任务通知机制的操作类型
 * 
 * 与API关联：
 * - xTaskNotify() / xTaskNotifyWait()
 * 
 * 位操作说明：
 * 当使用eSetBits时，32位通知值按位或操作
 */
typedef enum
{
    eNoAction = 0,              // 仅触发通知，不修改通知值
    eSetBits,                   // 按位设置通知值（原子或操作）
    eIncrement,                 // 递增通知值（原子加1）
    eSetValueWithOverwrite,     // 强制覆盖通知值
    eSetValueWithoutOverwrite   // 仅当通知未读时更新值
} eNotifyAction;

/*--------------------- 超时跟踪结构体定义 --------------------*/
/**
 * @struct xTIME_OUT
 * @brief 内部阻塞超时跟踪结构体
 * 
 * 成员说明：
 * xOverflowCount - 系统时钟溢出次数计数器
 * xTimeOnEntering - 进入阻塞时刻的系统节拍值
 * 
 * 典型应用：
 * 跟踪vTaskDelayUntil()等函数的超时逻辑
 */
typedef struct xTIME_OUT
{
    BaseType_t xOverflowCount;  // 系统时钟溢出次数
    TickType_t xTimeOnEntering; // 进入阻塞状态的起始时间戳
} TimeOut_t;

/*--------------------- MPU内存区域描述结构体 -------------------*/
/**
 * @struct xMEMORY_REGION
 * @brief MPU保护内存区域描述符
 * 
 * 使用场景：
 * 创建MPU保护任务时通过xTaskCreateRestricted()配置
 * 
 * 成员说明：
 * pvBaseAddress - 内存区域起始地址（需对齐到MPU要求）
 * ulLengthInBytes - 区域长度（必须是2的幂次）
 * ulParameters - MPU属性位（如特权访问、可缓存等）
 */
typedef struct xMEMORY_REGION
{
    void *pvBaseAddress;        // 内存区域基地址
    uint32_t ulLengthInBytes;   // 区域长度（字节）
    uint32_t ulParameters;      // MPU属性配置位
} MemoryRegion_t;
/*-----------------------------------------------------------
 * 扩展类型定义部分
 *----------------------------------------------------------*/

/*------------------ MPU保护任务创建参数结构体 ------------------*/
/**
 * @struct xTASK_PARAMETERS
 * @brief 创建MPU保护任务所需的完整参数集合
 * 
 * 使用场景：
 * 通过xTaskCreateRestricted()创建具有内存保护的任务
 * 
 * 成员说明：
 * pvTaskCode    - 任务函数入口地址
 * pcName        - 任务名称字符串（最大长度由configMAX_TASK_NAME_LEN定义）
 * usStackDepth  - 堆栈深度（以StackType_t为单位）
 * pvParameters  - 传递给任务函数的参数指针
 * uxPriority    - 初始优先级（0~configMAX_PRIORITIES-1）
 * puxStackBuffer- 用户提供的堆栈内存指针（静态分配时使用）
 * xRegions      - MPU内存区域配置数组（portNUM_CONFIGURABLE_REGIONS定义区域数）
 */
typedef struct xTASK_PARAMETERS
{
    TaskFunction_t pvTaskCode;         // 任务函数指针
    const char * const pcName;         // 任务名称（用于调试）
    uint16_t usStackDepth;             // 堆栈深度（单位：StackType_t）
    void *pvParameters;                // 任务参数传递指针
    UBaseType_t uxPriority;            // 初始优先级
    StackType_t *puxStackBuffer;       // 堆栈缓冲区指针（静态分配）
    MemoryRegion_t xRegions[ portNUM_CONFIGURABLE_REGIONS ]; // MPU内存区域配置
} TaskParameters_t;

/*--------------------- 任务状态信息结构体 --------------------*/
/**
 * @struct xTASK_STATUS
 * @brief 任务状态信息快照（用于系统状态监控）
 * 
 * 典型应用：
 * 通过uxTaskGetSystemState()获取所有任务的状态信息
 * 
 * 生命周期警告：
 * pcTaskName可能指向已删除任务的无效内存
 */
typedef struct xTASK_STATUS
{
    TaskHandle_t xHandle;              // 任务句柄
    const char *pcTaskName;            // 任务名称（可能已失效）
    UBaseType_t xTaskNumber;           // 任务唯一ID（调试用途）
    eTaskState eCurrentState;          // 当前状态（eRunning/eReady等）
    UBaseType_t uxCurrentPriority;     // 当前优先级（含继承优先级）
    UBaseType_t uxBasePriority;        // 基础优先级（互斥量释放后恢复的优先级）
    uint32_t ulRunTimeCounter;         // 累计运行时间（需启用configGENERATE_RUN_TIME_STATS）
    StackType_t *pxStackBase;          // 堆栈起始地址（低地址端）
    uint16_t usStackHighWaterMark;     // 堆栈历史最小剩余空间（字节）
} TaskStatus_t;

/*--------------------- 低功耗模式状态枚举 --------------------*/
/**
 * @enum eSleepModeStatus
 * @brief 进入低功耗模式前的系统状态判断结果
 * 
 * 与API关联：
 * eTaskConfirmSleepModeStatus()
 */
typedef enum
{
    eAbortSleep = 0,       // 存在就绪任务，禁止进入睡眠
    eStandardSleep,         // 可进入定时器唤醒的睡眠模式
    eNoTasksWaitingTimeout  // 无任务等待，可进入深度睡眠（仅外部中断可唤醒）
} eSleepModeStatus;

/*--------------------- 自定义资源管理扩展部分 -----------------*/
/* 注：以下为自定义扩展内容，非FreeRTOS标准API */

/**
 * @struct xRESOURCE_CB
 * @brief 资源控制块（前向声明）
 * （推测用于分阶段资源管理）
 */
typedef struct xRESOURCE_CB ResourceCB_t;

/* 分阶段优先级设置函数 */
void vTaskInitStagedPriorities(TaskHandle_t xTask, 
                              UBaseType_t uxStages, 
                              UBaseType_t *puxPriorities);

/* 分阶段互斥量操作函数 */
BaseType_t xSemaphoreGiveStagedMutex(SemaphoreHandle_t xMutex);
BaseType_t xSemaphoreTakeStagedMutex(SemaphoreHandle_t xMutex, 
                                    TickType_t xBlockTime,
                                    UBaseType_t uxResourceStage);

/* 阶段切换函数 */
BaseType_t xTaskSetCurrentStage(TaskHandle_t xTask, 
                               UBaseType_t uxNewStage);

/* 资源控制块转换函数（内部使用） */
ResourceCB_t *prvConvertToResourceCB(SemaphoreHandle_t xSemaphore,
                                    UBaseType_t uxStage);

/* 从信号量获取资源控制块（内部使用） */
static ResourceCB_t *prvGetResourceCBFromSemaphore(SemaphoreHandle_t xSemaphore);
/*-----------------------------------------------------------
 * 调度控制相关宏定义
 *----------------------------------------------------------*/

/*------------------- 空闲任务优先级定义 -------------------*/
/**
 * @def tskIDLE_PRIORITY
 * @brief 空闲任务固定优先级（不可修改）
 * @ingroup TaskUtils
 * 
 * 特殊说明：
 * - 始终为系统最低优先级
 * - 用于优先级继承时的基准值
 */
#define tskIDLE_PRIORITY            ( ( UBaseType_t ) 0U )

/*----------------------- 强制任务切换宏 ---------------------*/
/**
 * @def taskYIELD
 * @brief 强制触发上下文切换
 * @ingroup SchedulerControl
 * 
 * 典型应用场景：
 * - 协作式调度中主动让出CPU
 * - 在ISR中唤醒高优先级任务时
 * 
 * 硬件依赖：
 * 实际实现为portYIELD()，可能触发PendSV异常（Cortex-M）
 */
#define taskYIELD()                 portYIELD()

/*----------------------- 临界区管理宏 -----------------------*/
/**
 * @def taskENTER_CRITICAL
 * @brief 进入临界区（禁用中断/提升优先级）
 * @ingroup SchedulerControl
 * 
 * 嵌套规则：
 * 必须与taskEXIT_CRITICAL()严格匹配
 * 
 * 示例：
 * taskENTER_CRITICAL();
 * // 操作共享资源
 * taskEXIT_CRITICAL();
 */
#define taskENTER_CRITICAL()        portENTER_CRITICAL()

/**
 * @def taskENTER_CRITICAL_FROM_ISR
 * @brief 在中断中进入临界区（保存当前中断状态）
 * @return 进入临界区前的中断状态（需传递给EXIT宏）
 */
#define taskENTER_CRITICAL_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()

/**
 * @def taskEXIT_CRITICAL
 * @brief 退出临界区（恢复中断/优先级）
 */
#define taskEXIT_CRITICAL()         portEXIT_CRITICAL()

/**
 * @def taskEXIT_CRITICAL_FROM_ISR
 * @brief 在中断中退出临界区
 * @param x 进入临界区时保存的中断状态
 */
#define taskEXIT_CRITICAL_FROM_ISR( x ) portCLEAR_INTERRUPT_MASK_FROM_ISR( x )

/*----------------------- 中断控制宏 -----------------------*/
/**
 * @def taskDISABLE_INTERRUPTS
 * @brief 全局禁用可屏蔽中断
 * @warning 慎用，可能破坏系统实时性
 */
#define taskDISABLE_INTERRUPTS()    portDISABLE_INTERRUPTS()

/**
 * @def taskENABLE_INTERRUPTS
 * @brief 全局启用可屏蔽中断
 */
#define taskENABLE_INTERRUPTS()     portENABLE_INTERRUPTS()

/*--------------------- 调度器状态常量 ---------------------*/
/**
 * @def taskSCHEDULER_SUSPENDED
 * @brief 调度器挂起状态（vTaskSuspendAll()调用后）
 * 
 * 状态转换图：
 * NOT_STARTED → RUNNING ? SUSPENDED
 */
#define taskSCHEDULER_SUSPENDED     ( ( BaseType_t ) 0 )  /* 优化断言检查 */

/**
 * @def taskSCHEDULER_NOT_STARTED
 * @brief 调度器未启动状态（vTaskStartScheduler()调用前）
 */
#define taskSCHEDULER_NOT_STARTED   ( ( BaseType_t ) 1 )

/**
 * @def taskSCHEDULER_RUNNING
 * @brief 调度器正常运行状态
 */
#define taskSCHEDULER_RUNNING       ( ( BaseType_t ) 2 )

/*-----------------------------------------------------------
 * TASK CREATION API
 *----------------------------------------------------------*/

/**
 * task. h
 *<pre>
 BaseType_t xTaskCreate(
							  TaskFunction_t pvTaskCode,
							  const char * const pcName,
							  uint16_t usStackDepth,
							  void *pvParameters,
							  UBaseType_t uxPriority,
							  TaskHandle_t *pvCreatedTask
						  );</pre>
 *
 * Create a new task and add it to the list of tasks that are ready to run.
 *
 * Internally, within the FreeRTOS implementation, tasks use two blocks of
 * memory.  The first block is used to hold the task's data structures.  The
 * second block is used by the task as its stack.  If a task is created using
 * xTaskCreate() then both blocks of memory are automatically dynamically
 * allocated inside the xTaskCreate() function.  (see
 * http://www.freertos.org/a00111.html).  If a task is created using
 * xTaskCreateStatic() then the application writer must provide the required
 * memory.  xTaskCreateStatic() therefore allows a task to be created without
 * using any dynamic memory allocation.
 *
 * See xTaskCreateStatic() for a version that does not use any dynamic memory
 * allocation.
 *
 * xTaskCreate() can only be used to create a task that has unrestricted
 * access to the entire microcontroller memory map.  Systems that include MPU
 * support can alternatively create an MPU constrained task using
 * xTaskCreateRestricted().
 *
 * @param pvTaskCode Pointer to the task entry function.  Tasks
 * must be implemented to never return (i.e. continuous loop).
 *
 * @param pcName A descriptive name for the task.  This is mainly used to
 * facilitate debugging.  Max length defined by configMAX_TASK_NAME_LEN - default
 * is 16.
 *
 * @param usStackDepth The size of the task stack specified as the number of
 * variables the stack can hold - not the number of bytes.  For example, if
 * the stack is 16 bits wide and usStackDepth is defined as 100, 200 bytes
 * will be allocated for stack storage.
 *
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 *
 * @param uxPriority The priority at which the task should run.  Systems that
 * include MPU support can optionally create tasks in a privileged (system)
 * mode by setting bit portPRIVILEGE_BIT of the priority parameter.  For
 * example, to create a privileged task at priority 2 the uxPriority parameter
 * should be set to ( 2 | portPRIVILEGE_BIT ).
 *
 * @param pvCreatedTask Used to pass back a handle by which the created task
 * can be referenced.
 *
 * @return pdPASS if the task was successfully created and added to a ready
 * list, otherwise an error code defined in the file projdefs.h
 *
 * Example usage:
   <pre>
 // Task to be created.
 void vTaskCode( void * pvParameters )
 {
	 for( ;; )
	 {
		 // Task code goes here.
	 }
 }

 // Function that creates a task.
 void vOtherFunction( void )
 {
 static uint8_t ucParameterToPass;
 TaskHandle_t xHandle = NULL;

	 // Create the task, storing the handle.  Note that the passed parameter ucParameterToPass
	 // must exist for the lifetime of the task, so in this case is declared static.  If it was just an
	 // an automatic stack variable it might no longer exist, or at least have been corrupted, by the time
	 // the new task attempts to access it.
	 xTaskCreate( vTaskCode, "NAME", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle );
     configASSERT( xHandle );

	 // Use the handle to delete the task.
     if( xHandle != NULL )
     {
	     vTaskDelete( xHandle );
     }
 }
   </pre>
 * \defgroup xTaskCreate xTaskCreate
 * \ingroup Tasks
 */
/**
 * @fn BaseType_t xTaskCreate(TaskFunction_t, const char*, uint16_t, void*, UBaseType_t, TaskHandle_t*)
 * @brief 动态创建新任务（使用FreeRTOS内存管理）
 * @ingroup Tasks
 * 
 * @param pxTaskCode 任务函数入口指针（必须永不返回）
 * @param pcName 任务描述性名称（用于调试，最长configMAX_TASK_NAME_LEN-1字符）
 * @param usStackDepth 任务堆栈深度（以StackType_t字为单位，实际字节数=字大小*深度）
 * @param pvParameters 传递给任务函数的参数指针（可NULL）
 * @param uxPriority 任务优先级（0=最低，configMAX_PRIORITIES-1=最高）
 * @param pxCreatedTask 输出参数，接收新任务句柄（可NULL）
 * @return pdPASS 创建成功，pdFAIL 内存不足或参数无效
 * 
 * 内存行为：
 * - 从FreeRTOS堆中分配TCB和堆栈空间
 * - 任务删除时自动释放内存（需调用vTaskDelete）
 * 
 * 典型用法：
 * TaskHandle_t xHandle;
 * if(xTaskCreate(vTaskFunc, "Task1", 128, NULL, 2, &xHandle) == pdPASS) {
 *    // 任务创建成功
 * }
 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                       const char * const pcName,
                       const uint16_t usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pxCreatedTask ) PRIVILEGED_FUNCTION;
#endif

/**
 * task. h
 *<pre>
 TaskHandle_t xTaskCreateStatic( TaskFunction_t pvTaskCode,
								 const char * const pcName,
								 uint32_t ulStackDepth,
								 void *pvParameters,
								 UBaseType_t uxPriority,
								 StackType_t *pxStackBuffer,
								 StaticTask_t *pxTaskBuffer );</pre>
 *
 * Create a new task and add it to the list of tasks that are ready to run.
 *
 * Internally, within the FreeRTOS implementation, tasks use two blocks of
 * memory.  The first block is used to hold the task's data structures.  The
 * second block is used by the task as its stack.  If a task is created using
 * xTaskCreate() then both blocks of memory are automatically dynamically
 * allocated inside the xTaskCreate() function.  (see
 * http://www.freertos.org/a00111.html).  If a task is created using
 * xTaskCreateStatic() then the application writer must provide the required
 * memory.  xTaskCreateStatic() therefore allows a task to be created without
 * using any dynamic memory allocation.
 *
 * @param pvTaskCode Pointer to the task entry function.  Tasks
 * must be implemented to never return (i.e. continuous loop).
 *
 * @param pcName A descriptive name for the task.  This is mainly used to
 * facilitate debugging.  The maximum length of the string is defined by
 * configMAX_TASK_NAME_LEN in FreeRTOSConfig.h.
 *
 * @param ulStackDepth The size of the task stack specified as the number of
 * variables the stack can hold - not the number of bytes.  For example, if
 * the stack is 32-bits wide and ulStackDepth is defined as 100 then 400 bytes
 * will be allocated for stack storage.
 *
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 *
 * @param uxPriority The priority at which the task will run.
 *
 * @param pxStackBuffer Must point to a StackType_t array that has at least
 * ulStackDepth indexes - the array will then be used as the task's stack,
 * removing the need for the stack to be allocated dynamically.
 *
 * @param pxTaskBuffer Must point to a variable of type StaticTask_t, which will
 * then be used to hold the task's data structures, removing the need for the
 * memory to be allocated dynamically.
 *
 * @return If neither pxStackBuffer or pxTaskBuffer are NULL, then the task will
 * be created and pdPASS is returned.  If either pxStackBuffer or pxTaskBuffer
 * are NULL then the task will not be created and
 * errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY is returned.
 *
 * Example usage:
   <pre>

    // Dimensions the buffer that the task being created will use as its stack.
    // NOTE:  This is the number of words the stack will hold, not the number of
    // bytes.  For example, if each stack item is 32-bits, and this is set to 100,
    // then 400 bytes (100 * 32-bits) will be allocated.
    #define STACK_SIZE 200

    // Structure that will hold the TCB of the task being created.
    StaticTask_t xTaskBuffer;

    // Buffer that the task being created will use as its stack.  Note this is
    // an array of StackType_t variables.  The size of StackType_t is dependent on
    // the RTOS port.
    StackType_t xStack[ STACK_SIZE ];

    // Function that implements the task being created.
    void vTaskCode( void * pvParameters )
    {
        // The parameter value is expected to be 1 as 1 is passed in the
        // pvParameters value in the call to xTaskCreateStatic().
        configASSERT( ( uint32_t ) pvParameters == 1UL );

        for( ;; )
        {
            // Task code goes here.
        }
    }

    // Function that creates a task.
    void vOtherFunction( void )
    {
        TaskHandle_t xHandle = NULL;

        // Create the task without using any dynamic memory allocation.
        xHandle = xTaskCreateStatic(
                      vTaskCode,       // Function that implements the task.
                      "NAME",          // Text name for the task.
                      STACK_SIZE,      // Stack size in words, not bytes.
                      ( void * ) 1,    // Parameter passed into the task.
                      tskIDLE_PRIORITY,// Priority at which the task is created.
                      xStack,          // Array to use as the task's stack.
                      &xTaskBuffer );  // Variable to hold the task's data structure.

        // puxStackBuffer and pxTaskBuffer were not NULL, so the task will have
        // been created, and xHandle will be the task's handle.  Use the handle
        // to suspend the task.
        vTaskSuspend( xHandle );
    }
   </pre>
 * \defgroup xTaskCreateStatic xTaskCreateStatic
 * \ingroup Tasks
 */
/**
 * @fn TaskHandle_t xTaskCreateStatic(TaskFunction_t, const char*, uint32_t, void*, UBaseType_t, StackType_t*, StaticTask_t*)
 * @brief 静态创建新任务（用户提供内存缓冲区）
 * @ingroup Tasks
 * 
 * @param pxTaskCode 任务函数入口指针（必须永不返回）
 * @param pcName 任务描述性名称（最长configMAX_TASK_NAME_LEN-1字符）
 * @param ulStackDepth 堆栈深度（以StackType_t字为单位）
 * @param pvParameters 传递给任务函数的参数指针
 * @param uxPriority 任务优先级（0~configMAX_PRIORITIES-1）
 * @param puxStackBuffer 堆栈缓冲区指针（需对齐到portBYTE_ALIGNMENT）
 * @param pxTaskBuffer TCB缓冲区指针（需为StaticTask_t类型）
 * @return 成功返回任务句柄，失败返回NULL
 * 
 * 内存管理要求：
 * 1. 堆栈缓冲区大小 = ulStackDepth * sizeof(StackType_t)
 * 2. TCB缓冲区大小 = sizeof(StaticTask_t)
 * 3. 缓冲区需在任务生命周期内保持有效
 * 
 * 示例：
 * StaticTask_t xTaskBuffer;
 * StackType_t xStack[ 128 ];
 * TaskHandle_t xHandle = xTaskCreateStatic(vTask, "StatTask", 128, NULL, 1, xStack, &xTaskBuffer);
 */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                               const char * const pcName,
                               const uint32_t ulStackDepth,
                               void * const pvParameters,
                               UBaseType_t uxPriority,
                               StackType_t * const puxStackBuffer,
                               StaticTask_t * const pxTaskBuffer ) PRIVILEGED_FUNCTION;
#endif /* configSUPPORT_STATIC_ALLOCATION */

/**
 * task. h
 *<pre>
 BaseType_t xTaskCreateRestricted( TaskParameters_t *pxTaskDefinition, TaskHandle_t *pxCreatedTask );</pre>
 *
 * xTaskCreateRestricted() should only be used in systems that include an MPU
 * implementation.
 *
 * Create a new task and add it to the list of tasks that are ready to run.
 * The function parameters define the memory regions and associated access
 * permissions allocated to the task.
 *
 * @param pxTaskDefinition Pointer to a structure that contains a member
 * for each of the normal xTaskCreate() parameters (see the xTaskCreate() API
 * documentation) plus an optional stack buffer and the memory region
 * definitions.
 *
 * @param pxCreatedTask Used to pass back a handle by which the created task
 * can be referenced.
 *
 * @return pdPASS if the task was successfully created and added to a ready
 * list, otherwise an error code defined in the file projdefs.h
 *
 * Example usage:
   <pre>
// Create an TaskParameters_t structure that defines the task to be created.
static const TaskParameters_t xCheckTaskParameters =
{
	vATask,		// pvTaskCode - the function that implements the task.
	"ATask",	// pcName - just a text name for the task to assist debugging.
	100,		// usStackDepth	- the stack size DEFINED IN WORDS.
	NULL,		// pvParameters - passed into the task function as the function parameters.
	( 1UL | portPRIVILEGE_BIT ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
	cStackBuffer,// puxStackBuffer - the buffer to be used as the task stack.

	// xRegions - Allocate up to three separate memory regions for access by
	// the task, with appropriate access permissions.  Different processors have
	// different memory alignment requirements - refer to the FreeRTOS documentation
	// for full information.
	{
		// Base address					Length	Parameters
        { cReadWriteArray,				32,		portMPU_REGION_READ_WRITE },
        { cReadOnlyArray,				32,		portMPU_REGION_READ_ONLY },
        { cPrivilegedOnlyAccessArray,	128,	portMPU_REGION_PRIVILEGED_READ_WRITE }
	}
};

int main( void )
{
TaskHandle_t xHandle;

	// Create a task from the const structure defined above.  The task handle
	// is requested (the second parameter is not NULL) but in this case just for
	// demonstration purposes as its not actually used.
	xTaskCreateRestricted( &xRegTest1Parameters, &xHandle );

	// Start the scheduler.
	vTaskStartScheduler();

	// Will only get here if there was insufficient memory to create the idle
	// and/or timer task.
	for( ;; );
}
   </pre>
 * \defgroup xTaskCreateRestricted xTaskCreateRestricted
 * \ingroup Tasks
 */
/**
 * @fn BaseType_t xTaskCreateRestricted(const TaskParameters_t*, TaskHandle_t*)
 * @brief 创建MPU内存保护任务（需硬件支持）
 * @ingroup Tasks
 * 
 * @param pxTaskDefinition 任务参数结构体指针（包含MPU配置）
 * @param pxCreatedTask 输出参数，接收新任务句柄
 * @return pdPASS 创建成功，pdFAIL 配置错误或资源不足
 * 
 * 关键特性：
 * 1. 基于MPU硬件实现内存访问隔离
 * 2. 支持特权/非特权执行模式
 * 3. 防止堆栈溢出破坏系统关键区域
 * 
 * 使用流程：
 * 1. 配置TaskParameters_t结构体（含MPU区域）
 * 2. 调用本函数创建受保护任务
 * 3. 校验返回值及输出句柄
 * 
 * 配置要求：
 * - MPU区域必须满足对齐要求（地址和长度）
 * - 至少配置一个可执行代码区域
 */
#if( portUSING_MPU_WRAPPERS == 1 )
BaseType_t xTaskCreateRestricted( 
    const TaskParameters_t * const pxTaskDefinition, /* MPU任务配置结构体 */
    TaskHandle_t *pxCreatedTask                     /* 输出任务句柄 */
) PRIVILEGED_FUNCTION;
#endif

/**
 * task. h
 *<pre>
 void vTaskAllocateMPURegions( TaskHandle_t xTask, const MemoryRegion_t * const pxRegions );</pre>
 *
 * Memory regions are assigned to a restricted task when the task is created by
 * a call to xTaskCreateRestricted().  These regions can be redefined using
 * vTaskAllocateMPURegions().
 *
 * @param xTask The handle of the task being updated.
 *
 * @param xRegions A pointer to an MemoryRegion_t structure that contains the
 * new memory region definitions.
 *
 * Example usage:
   <pre>
// Define an array of MemoryRegion_t structures that configures an MPU region
// allowing read/write access for 1024 bytes starting at the beginning of the
// ucOneKByte array.  The other two of the maximum 3 definable regions are
// unused so set to zero.
static const MemoryRegion_t xAltRegions[ portNUM_CONFIGURABLE_REGIONS ] =
{
	// Base address		Length		Parameters
	{ ucOneKByte,		1024,		portMPU_REGION_READ_WRITE },
	{ 0,				0,			0 },
	{ 0,				0,			0 }
};

void vATask( void *pvParameters )
{
	// This task was created such that it has access to certain regions of
	// memory as defined by the MPU configuration.  At some point it is
	// desired that these MPU regions are replaced with that defined in the
	// xAltRegions const struct above.  Use a call to vTaskAllocateMPURegions()
	// for this purpose.  NULL is used as the task handle to indicate that this
	// function should modify the MPU regions of the calling task.
	vTaskAllocateMPURegions( NULL, xAltRegions );

	// Now the task can continue its function, but from this point on can only
	// access its stack and the ucOneKByte array (unless any other statically
	// defined or shared regions have been declared elsewhere).
}
   </pre>
 * \defgroup xTaskCreateRestricted xTaskCreateRestricted
 * \ingroup Tasks
 */
/**
 * @fn void vTaskAllocateMPURegions(TaskHandle_t, const MemoryRegion_t*)
 * @brief 动态重配任务的MPU内存区域（需特权模式）
 * @ingroup Tasks
 * 
 * @param xTask 目标任务的句柄（NULL表示当前任务）
 * @param pxRegions 新MPU配置数组指针（需包含portNUM_CONFIGURABLE_REGIONS个区域）
 * 
 * 典型应用场景：
 * 1. 动态加载可执行模块到安全内存区域
 * 2. 运行时调整任务的数据访问权限
 * 3. 实现内存隔离的热更新机制
 * 
 * 配置要求：
 * - 新配置必须覆盖任务当前访问的所有内存区域
 * - 数组必须包含portNUM_CONFIGURABLE_REGIONS个有效区域
 * - 区域参数必须符合MPU对齐规则
 * 
 * 调用限制：
 * 必须在特权模式下执行（如从特权任务或系统调用）
 */
void vTaskAllocateMPURegions( 
    TaskHandle_t xTask,                   /* 目标任务句柄 */
    const MemoryRegion_t * const pxRegions /* 新MPU区域配置数组 */
) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>void vTaskDelete( TaskHandle_t xTask );</pre>
 *
 * INCLUDE_vTaskDelete must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Remove a task from the RTOS real time kernel's management.  The task being
 * deleted will be removed from all ready, blocked, suspended and event lists.
 *
 * NOTE:  The idle task is responsible for freeing the kernel allocated
 * memory from tasks that have been deleted.  It is therefore important that
 * the idle task is not starved of microcontroller processing time if your
 * application makes any calls to vTaskDelete ().  Memory allocated by the
 * task code is not automatically freed, and should be freed before the task
 * is deleted.
 *
 * See the demo application file death.c for sample code that utilises
 * vTaskDelete ().
 *
 * @param xTask The handle of the task to be deleted.  Passing NULL will
 * cause the calling task to be deleted.
 *
 * Example usage:
   <pre>
 void vOtherFunction( void )
 {
 TaskHandle_t xHandle;

	 // Create the task, storing the handle.
	 xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );

	 // Use the handle to delete the task.
	 vTaskDelete( xHandle );
 }
   </pre>
 * \defgroup vTaskDelete vTaskDelete
 * \ingroup Tasks
 */
/**
 * @fn void vTaskDelete(TaskHandle_t)
 * @brief 删除指定任务并释放资源
 * @ingroup Tasks
 * 
 * @param xTaskToDelete 待删除任务句柄（NULL表示删除当前任务）
 * 
 * 关键行为：
 * 1. 立即将任务移出所有调度列表
 * 2. 回收动态分配的任务内存（TCB+堆栈）
 * 3. 任务删除后句柄失效变为野指针
 * 
 * 删除流程：
 * 任务标记为eDeleted → 空闲任务清理资源 → 内存返还堆
 * 
 * 使用示例：
 * // 删除其他任务
 * vTaskDelete(xHandle);
 * 
 * // 任务自我删除
 * void vSelfDeletingTask(void* pvParam) {
 *     // 任务逻辑...
 *     vTaskDelete(NULL); // 删除自身
 * }
 */
void vTaskDelete( TaskHandle_t xTaskToDelete ) PRIVILEGED_FUNCTION;

/*-----------------------------------------------------------
 * TASK CONTROL API
 *----------------------------------------------------------*/

/**
 * task. h
 * <pre>void vTaskDelay( const TickType_t xTicksToDelay );</pre>
 *
 * Delay a task for a given number of ticks.  The actual time that the
 * task remains blocked depends on the tick rate.  The constant
 * portTICK_PERIOD_MS can be used to calculate real time from the tick
 * rate - with the resolution of one tick period.
 *
 * INCLUDE_vTaskDelay must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 *
 * vTaskDelay() specifies a time at which the task wishes to unblock relative to
 * the time at which vTaskDelay() is called.  For example, specifying a block
 * period of 100 ticks will cause the task to unblock 100 ticks after
 * vTaskDelay() is called.  vTaskDelay() does not therefore provide a good method
 * of controlling the frequency of a periodic task as the path taken through the
 * code, as well as other task and interrupt activity, will effect the frequency
 * at which vTaskDelay() gets called and therefore the time at which the task
 * next executes.  See vTaskDelayUntil() for an alternative API function designed
 * to facilitate fixed frequency execution.  It does this by specifying an
 * absolute time (rather than a relative time) at which the calling task should
 * unblock.
 *
 * @param xTicksToDelay The amount of time, in tick periods, that
 * the calling task should block.
 *
 * Example usage:

 void vTaskFunction( void * pvParameters )
 {
 // Block for 500ms.
 const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

	 for( ;; )
	 {
		 // Simply toggle the LED every 500ms, blocking between each toggle.
		 vToggleLED();
		 vTaskDelay( xDelay );
	 }
 }

 * \defgroup vTaskDelay vTaskDelay
 * \ingroup TaskCtrl
 */
/**
 * @fn void vTaskDelay(TickType_t)
 * @brief 将当前任务延迟指定时钟节拍数（相对延时）
 * @ingroup TaskCtrl
 * 
 * @param xTicksToDelay 延迟的时钟节拍数（单位由configTICK_RATE_HZ定义）
 * 
 * 工作机制：
 * 1. 将任务移出就绪列表，加入延迟列表
 * 2. 更新系统节拍计数器
 * 3. 触发上下文切换
 * 
 * 时间计算示例：
 * 当configTICK_RATE_HZ=1000时：
 * vTaskDelay(1000) → 延迟1秒（1000个1ms节拍）
 * 
 * 使用场景：
 * - 周期性任务执行间隔控制
 * - CPU资源主动释放
 * 
 * 注意：
 * 不可在中断中调用，需使用xTaskResumeFromISR系列函数
 */
void vTaskDelay( const TickType_t xTicksToDelay ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>void vTaskDelayUntil( TickType_t *pxPreviousWakeTime, const TickType_t xTimeIncrement );</pre>
 *
 * INCLUDE_vTaskDelayUntil must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Delay a task until a specified time.  This function can be used by periodic
 * tasks to ensure a constant execution frequency.
 *
 * This function differs from vTaskDelay () in one important aspect:  vTaskDelay () will
 * cause a task to block for the specified number of ticks from the time vTaskDelay () is
 * called.  It is therefore difficult to use vTaskDelay () by itself to generate a fixed
 * execution frequency as the time between a task starting to execute and that task
 * calling vTaskDelay () may not be fixed [the task may take a different path though the
 * code between calls, or may get interrupted or preempted a different number of times
 * each time it executes].
 *
 * Whereas vTaskDelay () specifies a wake time relative to the time at which the function
 * is called, vTaskDelayUntil () specifies the absolute (exact) time at which it wishes to
 * unblock.
 *
 * The constant portTICK_PERIOD_MS can be used to calculate real time from the tick
 * rate - with the resolution of one tick period.
 *
 * @param pxPreviousWakeTime Pointer to a variable that holds the time at which the
 * task was last unblocked.  The variable must be initialised with the current time
 * prior to its first use (see the example below).  Following this the variable is
 * automatically updated within vTaskDelayUntil ().
 *
 * @param xTimeIncrement The cycle time period.  The task will be unblocked at
 * time *pxPreviousWakeTime + xTimeIncrement.  Calling vTaskDelayUntil with the
 * same xTimeIncrement parameter value will cause the task to execute with
 * a fixed interface period.
 *
 * Example usage:
   <pre>
 // Perform an action every 10 ticks.
 void vTaskFunction( void * pvParameters )
 {
 TickType_t xLastWakeTime;
 const TickType_t xFrequency = 10;

	 // Initialise the xLastWakeTime variable with the current time.
	 xLastWakeTime = xTaskGetTickCount ();
	 for( ;; )
	 {
		 // Wait for the next cycle.
		 vTaskDelayUntil( &xLastWakeTime, xFrequency );

		 // Perform action here.
	 }
 }
   </pre>
 * \defgroup vTaskDelayUntil vTaskDelayUntil
 * \ingroup TaskCtrl
 */
/*-----------------------------------------------------------
 * 任务控制API
 *----------------------------------------------------------*/

/**
 * @fn void vTaskDelayUntil(TickType_t*, TickType_t)
 * @brief 固定频率周期任务调度（绝对延时）
 * @ingroup TaskCtrl
 * 
 * @param pxPreviousWakeTime 指针，记录上次唤醒时间（需初始化为当前tick）
 * @param xTimeIncrement 周期时间间隔（单位：时钟节拍）
 * 
 * 工作机制：
 * 1. 计算下次唤醒时间 = *pxPreviousWakeTime + xTimeIncrement
 * 2. 阻塞至指定绝对时间点
 * 3. 自动更新*pxPreviousWakeTime为实际唤醒时间
 * 
 * 示例：
 * TickType_t xLastWakeTime = xTaskGetTickCount();
 * while(1) {
 *     // 每100个tick执行一次
 *     vTaskDelayUntil(&xLastWakeTime, 100);
 *     // 周期任务代码
 * }
 */
void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime,
                     const TickType_t xTimeIncrement ) PRIVILEGED_FUNCTION;

/**
 * @fn BaseType_t xTaskAbortDelay(TaskHandle_t)
 * @brief 强制终止任务的阻塞状态
 * @ingroup TaskCtrl
 * 
 * @param xTask 目标任务的句柄
 * @return pdPASS-成功终止阻塞 / pdFAIL-任务未阻塞
 * 
 * 支持中断的阻塞类型：
 * - vTaskDelay() / vTaskDelayUntil()
 * - xQueueReceive() / xQueueSend()
 * - ulTaskNotifyTake() / xEventGroupWaitBits()
 * 
 * 典型应用：
 * 1. 紧急停止某个长期等待外设的任务
 * 2. 高优先级事件需要立即唤醒处理
 */
BaseType_t xTaskAbortDelay( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;


/**
 * task. h
 * <pre>UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask );</pre>
 *
 * INCLUDE_uxTaskPriorityGet must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Obtain the priority of any task.
 *
 * @param xTask Handle of the task to be queried.  Passing a NULL
 * handle results in the priority of the calling task being returned.
 *
 * @return The priority of xTask.
 *
 * Example usage:
   <pre>
 void vAFunction( void )
 {
 TaskHandle_t xHandle;

	 // Create a task, storing the handle.
	 xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );

	 // ...

	 // Use the handle to obtain the priority of the created task.
	 // It was created with tskIDLE_PRIORITY, but may have changed
	 // it itself.
	 if( uxTaskPriorityGet( xHandle ) != tskIDLE_PRIORITY )
	 {
		 // The task has changed it's priority.
	 }

	 // ...

	 // Is our priority higher than the created task?
	 if( uxTaskPriorityGet( xHandle ) < uxTaskPriorityGet( NULL ) )
	 {
		 // Our priority (obtained using NULL handle) is higher.
	 }
 }
   </pre>
 * \defgroup uxTaskPriorityGet uxTaskPriorityGet
 * \ingroup TaskCtrl
 */
/*-----------------------------------------------------------
 * 任务控制API
 *----------------------------------------------------------*/

/**
 * @fn UBaseType_t uxTaskPriorityGet(TaskHandle_t)
 * @brief 获取指定任务的当前优先级
 * @ingroup TaskCtrl
 * 
 * @param xTask 任务句柄（NULL表示当前任务）
 * @return 任务的优先级数值（0 ~ configMAX_PRIORITIES-1）
 * 
 * 工作机制：
 * - 返回任务的实际优先级（可能包含优先级继承）
 * - 实际优先级可能高于基础优先级，如果任务持有互斥量
 * 
 * 示例：
 * UBaseType_t uxPriority = uxTaskPriorityGet(xTaskHandle); // 获取指定任务优先级
 * UBaseType_t uxCurrentPriority = uxTaskPriorityGet(NULL); // 获取当前任务优先级
 */
UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/**
 * @fn UBaseType_t uxTaskPriorityGetFromISR(TaskHandle_t)
 * @brief 从ISR中获取指定任务的当前优先级
 * @ingroup TaskCtrl
 * 
 * @param xTask 任务句柄（NULL表示当前任务）
 * @return 任务的优先级数值（0 ~ configMAX_PRIORITIES-1）
 * 
 * 工作机制：
 * - 中断安全版本的uxTaskPriorityGet
 * - 返回任务的实际优先级（可能包含优先级继承）
 * - 实际优先级可能高于基础优先级，如果任务持有互斥量
 * 
 * 注意：
 * - 不能在任务中使用此函数
 * - 适用于需要在中断中查询任务优先级的情况
 * 
 * 示例：
 * UBaseType_t uxPriority = uxTaskPriorityGetFromISR(xTaskHandle); // 从ISR中获取指定任务优先级
 * UBaseType_t uxCurrentPriority = uxTaskPriorityGetFromISR(NULL); // 从ISR中获取当前任务优先级
 */
UBaseType_t uxTaskPriorityGetFromISR( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/**
 * @fn eTaskState eTaskGetState(TaskHandle_t)
 * @brief 获取任务的当前状态
 * @ingroup TaskCtrl
 * 
 * @param xTask 任务句柄（NULL表示当前任务）
 * @return 任务的状态（eTaskState枚举类型）
 * 
 * 工作机制：
 * - 返回任务在调用时刻的状态
 * - 状态可能包括eRunning、eReady、eBlocked、eSuspended、eDeleted等
 * - 状态可能在函数调用后立即改变，因此返回值为快照
 * 
 * 注意：
 * - INCLUDE_eTaskGetState需在FreeRTOSConfig.h中定义为1
 * - 使用时需确保配置正确，否则函数不可用
 * - 返回的状态可能不准确，需结合其他机制使用
 * 
 * 示例：
 * eTaskState eState = eTaskGetState(xTaskHandle); // 获取指定任务状态
 * eTaskState eCurrentState = eTaskGetState(NULL); // 获取当前任务状态
 */
eTaskState eTaskGetState( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;


/**
 * task. h
 * <pre>void vTaskGetInfo( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState );</pre>
 *
 * configUSE_TRACE_FACILITY must be defined as 1 for this function to be
 * available.  See the configuration section for more information.
 *
 * Populates a TaskStatus_t structure with information about a task.
 *
 * @param xTask Handle of the task being queried.  If xTask is NULL then
 * information will be returned about the calling task.
 *
 * @param pxTaskStatus A pointer to the TaskStatus_t structure that will be
 * filled with information about the task referenced by the handle passed using
 * the xTask parameter.
 *
 * @xGetFreeStackSpace The TaskStatus_t structure contains a member to report
 * the stack high water mark of the task being queried.  Calculating the stack
 * high water mark takes a relatively long time, and can make the system
 * temporarily unresponsive - so the xGetFreeStackSpace parameter is provided to
 * allow the high water mark checking to be skipped.  The high watermark value
 * will only be written to the TaskStatus_t structure if xGetFreeStackSpace is
 * not set to pdFALSE;
 *
 * @param eState The TaskStatus_t structure contains a member to report the
 * state of the task being queried.  Obtaining the task state is not as fast as
 * a simple assignment - so the eState parameter is provided to allow the state
 * information to be omitted from the TaskStatus_t structure.  To obtain state
 * information then set eState to eInvalid - otherwise the value passed in
 * eState will be reported as the task state in the TaskStatus_t structure.
 *
 * Example usage:
   <pre>
 void vAFunction( void )
 {
 TaskHandle_t xHandle;
 TaskStatus_t xTaskDetails;

    // Obtain the handle of a task from its name.
    xHandle = xTaskGetHandle( "Task_Name" );

    // Check the handle is not NULL.
    configASSERT( xHandle );

    // Use the handle to obtain further information about the task.
    vTaskGetInfo( xHandle,
                  &xTaskDetails,
                  pdTRUE, // Include the high water mark in xTaskDetails.
                  eInvalid ); // Include the task state in xTaskDetails.
 }
   </pre>
 * \defgroup vTaskGetInfo vTaskGetInfo
 * \ingroup TaskCtrl
 */
/*-----------------------------------------------------------
 * 任务控制API
 *----------------------------------------------------------*/

/**
 * @fn void vTaskGetInfo(TaskHandle_t, TaskStatus_t*, BaseType_t, eTaskState)
 * @brief 获取任务的详细信息
 * @ingroup TaskCtrl
 * 
 * @param xTask 任务句柄（NULL表示当前任务）
 * @param pxTaskStatus 指向TaskStatus_t结构体的指针，用于存储任务信息
 * @param xGetFreeStackSpace 是否获取剩余堆栈空间信息（pdTRUE或pdFALSE）
 * @param eState 指定查询的任务状态（或使用eInvalid），通常为eInvalid
 * 
 * 工作机制：
 * - 填充TaskStatus_t结构体以包含任务的详细信息
 * - 可以获取任务的优先级、状态、堆栈使用情况等
 * - 如果xGetFreeStackSpace为pdTRUE，还会计算剩余堆栈空间
 * - eState参数通常设置为eInvalid，表示不指定特定状态
 * 
 * 示例：
 * TaskStatus_t xTaskInfo;
 * vTaskGetInfo(xTaskHandle, &xTaskInfo, pdTRUE, eInvalid); // 获取指定任务的详细信息，包括剩余堆栈空间
 * vTaskGetInfo(NULL, &xTaskInfo, pdFALSE, eInvalid); // 获取当前任务的详细信息，不包括剩余堆栈空间
 */
void vTaskGetInfo( TaskHandle_t xTask, 
                  TaskStatus_t *pxTaskStatus, 
                  BaseType_t xGetFreeStackSpace, 
                  eTaskState eState ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority );</pre>
 *
 * INCLUDE_vTaskPrioritySet must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Set the priority of any task.
 *
 * A context switch will occur before the function returns if the priority
 * being set is higher than the currently executing task.
 *
 * @param xTask Handle to the task for which the priority is being set.
 * Passing a NULL handle results in the priority of the calling task being set.
 *
 * @param uxNewPriority The priority to which the task will be set.
 *
 * Example usage:
   <pre>
 void vAFunction( void )
 {
 TaskHandle_t xHandle;

	 // Create a task, storing the handle.
	 xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );

	 // ...

	 // Use the handle to raise the priority of the created task.
	 vTaskPrioritySet( xHandle, tskIDLE_PRIORITY + 1 );

	 // ...

	 // Use a NULL handle to raise our priority to the same value.
	 vTaskPrioritySet( NULL, tskIDLE_PRIORITY + 1 );
 }
   </pre>
 * \defgroup vTaskPrioritySet vTaskPrioritySet
 * \ingroup TaskCtrl
 */
/*-----------------------------------------------------------
 * 任务控制API
 *----------------------------------------------------------*/

/**
 * @fn void vTaskPrioritySet(TaskHandle_t, UBaseType_t)
 * @brief 设置指定任务的优先级
 * @ingroup TaskCtrl
 * 
 * @param xTask 任务句柄（NULL表示当前任务）
 * @param uxNewPriority 新的优先级数值（0 ~ configMAX_PRIORITIES-1）
 * 
 * 工作机制：
 * - 将指定任务的优先级设置为新的优先级
 * - 如果任务被挂起，优先级调整将在任务恢复时生效
 * - 优先级调整可能影响任务的调度顺序
 * 
 * 示例：
 * vTaskPrioritySet(xTaskHandle, 5); // 将指定任务的优先级设置为5
 * vTaskPrioritySet(NULL, 3); // 将当前任务的优先级设置为3
 */
void vTaskPrioritySet( TaskHandle_t xTask, 
                       UBaseType_t uxNewPriority ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>void vTaskSuspend( TaskHandle_t xTaskToSuspend );</pre>
 *
 * INCLUDE_vTaskSuspend must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Suspend any task.  When suspended a task will never get any microcontroller
 * processing time, no matter what its priority.
 *
 * Calls to vTaskSuspend are not accumulative -
 * i.e. calling vTaskSuspend () twice on the same task still only requires one
 * call to vTaskResume () to ready the suspended task.
 *
 * @param xTaskToSuspend Handle to the task being suspended.  Passing a NULL
 * handle will cause the calling task to be suspended.
 *
 * Example usage:
   <pre>
 void vAFunction( void )
 {
 TaskHandle_t xHandle;

	 // Create a task, storing the handle.
	 xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );

	 // ...

	 // Use the handle to suspend the created task.
	 vTaskSuspend( xHandle );

	 // ...

	 // The created task will not run during this period, unless
	 // another task calls vTaskResume( xHandle ).

	 //...


	 // Suspend ourselves.
	 vTaskSuspend( NULL );

	 // We cannot get here unless another task calls vTaskResume
	 // with our handle as the parameter.
 }
   </pre>
 * \defgroup vTaskSuspend vTaskSuspend
 * \ingroup TaskCtrl
 */
/*-----------------------------------------------------------
 * 任务控制API
 *----------------------------------------------------------*/

/**
 * @fn void vTaskSuspend(TaskHandle_t)
 * @brief 挂起指定任务
 * @ingroup TaskCtrl
 * 
 * @param xTaskToSuspend 任务句柄（NULL表示当前任务）
 * 
 * 工作机制：
 * - 将指定任务从就绪列表中移除
 * - 任务状态变为eSuspended
 * - 任务不再参与调度，直到被恢复
 * 
 * 示例：
 * vTaskSuspend(xTaskHandle); // 挂起指定任务
 * vTaskSuspend(NULL); // 挂起当前任务
 */
void vTaskSuspend( TaskHandle_t xTaskToSuspend ) PRIVILEGED_FUNCTION;


/**
 * task. h
 * <pre>void vTaskResume( TaskHandle_t xTaskToResume );</pre>
 *
 * INCLUDE_vTaskSuspend must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Resumes a suspended task.
 *
 * A task that has been suspended by one or more calls to vTaskSuspend ()
 * will be made available for running again by a single call to
 * vTaskResume ().
 *
 * @param xTaskToResume Handle to the task being readied.
 *
 * Example usage:
   <pre>
 void vAFunction( void )
 {
 TaskHandle_t xHandle;

	 // Create a task, storing the handle.
	 xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );

	 // ...

	 // Use the handle to suspend the created task.
	 vTaskSuspend( xHandle );

	 // ...

	 // The created task will not run during this period, unless
	 // another task calls vTaskResume( xHandle ).

	 //...


	 // Resume the suspended task ourselves.
	 vTaskResume( xHandle );

	 // The created task will once again get microcontroller processing
	 // time in accordance with its priority within the system.
 }
   </pre>
 * \defgroup vTaskResume vTaskResume
 * \ingroup TaskCtrl
 */
/*-----------------------------------------------------------
 * 任务控制API
 *----------------------------------------------------------*/

/**
 * @fn void vTaskResume(TaskHandle_t)
 * @brief 恢复指定任务的运行
 * @ingroup TaskCtrl
 * 
 * @param xTaskToResume 任务句柄（NULL表示当前任务）
 * 
 * 工作机制：
 * - 将指定任务从挂起状态（eSuspended）恢复到就绪状态（eReady）
 * - 如果任务因阻塞时间过长而被挂起，恢复后重新加入就绪列表
 * 
 * 示例：
 * vTaskResume(xTaskHandle); // 恢复指定任务
 * vTaskResume(NULL); // 恢复当前任务
 * 
 * 注意事项：
 * - 必须在特权模式下调用
 * - 恢复的任务将重新参与调度
 * - 需要确保任务句柄有效
 */
void vTaskResume( TaskHandle_t xTaskToResume ) PRIVILEGED_FUNCTION;

/**
 * @fn BaseType_t xTaskResumeFromISR(TaskHandle_t)
 * @brief 在中断服务例程中恢复指定任务的运行
 * @ingroup TaskCtrl
 * 
 * @param xTaskToResume 任务句柄（必须有效任务句柄）
 * @return pdTRUE 如果恢复任务导致上下文切换，否则pdFALSE
 * 
 * 工作机制：
 * - 中断安全版本的vTaskResume
 * - 将指定任务从挂起状态（eSuspended）恢复到就绪状态（eReady）
 * - 返回值用于指示是否需要上下文切换
 * 
 * 典型应用：
 * - 在ISR中恢复被挂起的任务
 * - 快速响应中断事件
 * 
 * 注意事项：
 * - 必须在特权模式下调用
 * - 不能使用在任务中，仅用于ISR
 * - 恢复的任务将重新参与调度
 * - 需要确保任务句柄有效
 * 
 * 警告：
 * - 不应使用此函数在任务未挂起前同步任务与中断
 * - 可能导致中断事件丢失，建议使用信号量进行同步
 */
BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume ) PRIVILEGED_FUNCTION;


/*-----------------------------------------------------------
 * SCHEDULER CONTROL
 *----------------------------------------------------------*/

/**
 * task. h
 * <pre>void vTaskStartScheduler( void );</pre>
 *
 * Starts the real time kernel tick processing.  After calling the kernel
 * has control over which tasks are executed and when.
 *
 * See the demo application file main.c for an example of creating
 * tasks and starting the kernel.
 *
 * Example usage:
   <pre>
 void vAFunction( void )
 {
	 // Create at least one task before starting the kernel.
	 xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

	 // Start the real time kernel with preemption.
	 vTaskStartScheduler ();

	 // Will not get here unless a task calls vTaskEndScheduler ()
 }
   </pre>
 *
 * \defgroup vTaskStartScheduler vTaskStartScheduler
 * \ingroup SchedulerControl
 */
/**
 * @brief 启动RTOS内核调度器
 * 
 * 本函数初始化FreeRTOS内核核心组件，创建系统任务（空闲任务、Timer Service Task等），
 * 并启动任务调度机制。调用后控制权永久转移给RTOS，通常不会返回（除非调用vTaskEndScheduler）
 * 
 * @note 关键执行流程
 * 1. 初始化内核组件（队列、信号量、调度器等）
 * 2. 创建空闲任务（IDLE Task）：
 *    - 优先级为tskIDLE_PRIORITY（最低优先级）
 *    - 使用用户定义的内存分配方案（heap_x.c）
 * 3. 若启用软件定时器（configUSE_TIMERS == 1）：
 *    - 创建Timer Service Task
 *    - 优先级由configTIMER_TASK_PRIORITY定义
 * 4. 启动系统节拍定时器（SysTick）
 * 5. 开始任务调度（选择最高优先级就绪任务执行）
 * 
 * @warning 重要限制
 * - 必须第一个RTOS API调用且仅调用一次
 * - 禁止在中断上下文或已有调度器运行时调用
 * - 若使用静态内存分配（configSUPPORT_STATIC_ALLOCATION），需预先提供内存块
 * - 启用MPU时需确保内存区域访问权限正确配置
 * 
 * @code
 * // 典型启动序列
 * void main( void ) {
 *     HAL_Init();                              // 硬件层初始化
 *     SystemClock_Config();                    // 配置系统时钟
 *     xTaskCreate( vTask1, "T1", 128, NULL, 2, NULL );  // 创建应用任务
 *     xTaskCreate( vTask2, "T2", 128, NULL, 1, NULL );
 *     vTaskStartScheduler();                   // 永不返回（除非终止调度）
 *     while(1);                                // 理论上不可达代码
 * }
 * @endcode
 */
void vTaskStartScheduler( void ) PRIVILEGED_FUNCTION;

 
/*-----------------------------------------------------------
 * 任务控制API
 *----------------------------------------------------------*/

/**
 * @fn void vTaskResume(TaskHandle_t)
 * @brief 恢复指定任务的运行
 * @ingroup TaskCtrl
 * 
 * @param xTaskToResume 任务句柄（NULL表示当前任务）
 * 
 * 工作机制：
 * - 将指定任务从挂起状态（eSuspended）恢复到就绪状态（eReady）
 * - 如果任务因阻塞时间过长而被挂起，恢复后重新加入就绪列表
 * 
 * 示例：
 * vTaskResume(xTaskHandle); // 恢复指定任务
 * vTaskResume(NULL); // 恢复当前任务
 * 
 * 注意事项：
 * - 必须在特权模式下调用
 * - 恢复的任务将重新参与调度
 * - 需要确保任务句柄有效
 */
void vTaskResume( TaskHandle_t xTaskToResume ) PRIVILEGED_FUNCTION;

/**
 * @fn BaseType_t xTaskResumeFromISR(TaskHandle_t)
 * @brief 在中断服务例程中恢复指定任务的运行
 * @ingroup TaskCtrl
 * 
 * @param xTaskToResume 任务句柄（必须有效任务句柄）
 * @return pdTRUE 如果恢复任务导致上下文切换，否则pdFALSE
 * 
 * 工作机制：
 * - 中断安全版本的vTaskResume
 * - 将指定任务从挂起状态（eSuspended）恢复到就绪状态（eReady）
 * - 返回值用于指示是否需要上下文切换
 * 
 * 典型应用：
 * - 在ISR中恢复被挂起的任务
 * - 快速响应中断事件
 * 
 * 注意事项：
 * - 必须在特权模式下调用
 * - 不能使用在任务中，仅用于ISR
 * - 恢复的任务将重新参与调度
 * - 需要确保任务句柄有效
 * 
 * 警告：
 * - 不应使用此函数在任务未挂起前同步任务与中断
 * - 可能导致中断事件丢失，建议使用信号量进行同步
 */
BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>void vTaskSuspendAll( void );</pre>
 *
 * Suspends the scheduler without disabling interrupts.  Context switches will
 * not occur while the scheduler is suspended.
 *
 * After calling vTaskSuspendAll () the calling task will continue to execute
 * without risk of being swapped out until a call to xTaskResumeAll () has been
 * made.
 *
 * API functions that have the potential to cause a context switch (for example,
 * vTaskDelayUntil(), xQueueSend(), etc.) must not be called while the scheduler
 * is suspended.
 *
 * Example usage:
   <pre>
 void vTask1( void * pvParameters )
 {
	 for( ;; )
	 {
		 // Task code goes here.

		 // ...

		 // At some point the task wants to perform a long operation during
		 // which it does not want to get swapped out.  It cannot use
		 // taskENTER_CRITICAL ()/taskEXIT_CRITICAL () as the length of the
		 // operation may cause interrupts to be missed - including the
		 // ticks.

		 // Prevent the real time kernel swapping out the task.
		 vTaskSuspendAll ();

		 // Perform the operation here.  There is no need to use critical
		 // sections as we have all the microcontroller processing time.
		 // During this time interrupts will still operate and the kernel
		 // tick count will be maintained.

		 // ...

		 // The operation is complete.  Restart the kernel.
		 xTaskResumeAll ();
	 }
 }
   </pre>
 * \defgroup vTaskSuspendAll vTaskSuspendAll
 * \ingroup SchedulerControl
 */
/*-----------------------------------------------------------
 * 调度器控制API
 *----------------------------------------------------------*/

/**
 * @fn void vTaskSuspendAll(void)
 * @brief 暂停调度器活动
 * @ingroup SchedulerControl
 * 
 * 工作机制：
 * - 暂停实时操作系统调度器的时钟节拍（tick）
 * - 阻止任务切换和任务创建/删除等操作
 * - 适用于需要临时停止调度器的操作
 * 
 * 示例：
 * void vCriticalOperation( void )
 * {
 *     // 暂停调度器
 *     vTaskSuspendAll();
 *     
 *     // 执行临界区操作
 *     // ...
 *     
 *     // 恢复调度器
 *     xTaskResumeAll();
 * }
 */
void vTaskSuspendAll( void ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>BaseType_t xTaskResumeAll( void );</pre>
 *
 * Resumes scheduler activity after it was suspended by a call to
 * vTaskSuspendAll().
 *
 * xTaskResumeAll() only resumes the scheduler.  It does not unsuspend tasks
 * that were previously suspended by a call to vTaskSuspend().
 *
 * @return If resuming the scheduler caused a context switch then pdTRUE is
 *		  returned, otherwise pdFALSE is returned.
 *
 * Example usage:
   <pre>
 void vTask1( void * pvParameters )
 {
	 for( ;; )
	 {
		 // Task code goes here.

		 // ...

		 // At some point the task wants to perform a long operation during
		 // which it does not want to get swapped out.  It cannot use
		 // taskENTER_CRITICAL ()/taskEXIT_CRITICAL () as the length of the
		 // operation may cause interrupts to be missed - including the
		 // ticks.

		 // Prevent the real time kernel swapping out the task.
		 vTaskSuspendAll ();

		 // Perform the operation here.  There is no need to use critical
		 // sections as we have all the microcontroller processing time.
		 // During this time interrupts will still operate and the real
		 // time kernel tick count will be maintained.

		 // ...

		 // The operation is complete.  Restart the kernel.  We want to force
		 // a context switch - but there is no point if resuming the scheduler
		 // caused a context switch already.
		 if( !xTaskResumeAll () )
		 {
			  taskYIELD ();
		 }
	 }
 }
   </pre>
 * \defgroup xTaskResumeAll xTaskResumeAll
 * \ingroup SchedulerControl
 */
/*-----------------------------------------------------------
 * 调度器控制API
 *----------------------------------------------------------*/

/**
 * @fn BaseType_t xTaskResumeAll(void)
 * @brief 恢复调度器活动
 * @ingroup SchedulerControl
 * 
 * @return pdTRUE 如果恢复调度器导致上下文切换，否则pdFALSE
 * 
 * 工作机制：
 * - 恢复实时操作系统调度器的时钟节拍（tick）
 * - 允许任务切换和任务创建/删除等操作
 * - 适用于需要临时停止调度器的操作
 * 
 * 示例：
 * void vCriticalOperation( void )
 * {
 *     // 暂停调度器
 *     vTaskSuspendAll();
 *     
 *     // 执行临界区操作
 *     // ...
 *     
 *     // 恢复调度器
 *     if( xTaskResumeAll() == pdTRUE )
 *     {
 *         taskYIELD(); // 强制任务切换
 *     }
 * }
 */
BaseType_t xTaskResumeAll( void ) PRIVILEGED_FUNCTION;

/*-----------------------------------------------------------
 * TASK UTILITIES
 *----------------------------------------------------------*/

/**
 * task. h
 * <pre>TickType_t xTaskGetTickCount( void );</pre>
 *
 * @return 自vTaskStartScheduler调用以来的时钟节拍数
 *
 * 工作机制：
 * - 返回当前的系统时钟节拍计数值
 * - 适用于需要时间戳或延时控制的场景
 * 
 * 示例：
 * TickType_t xTickCount = xTaskGetTickCount();
 * printf("Current Tick Count: %u\n", xTickCount);
 * 
 * \defgroup xTaskGetTickCount xTaskGetTickCount
 * \ingroup TaskUtils
 */
TickType_t xTaskGetTickCount( void ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>TickType_t xTaskGetTickCountFromISR( void );</pre>
 *
 * @return 自vTaskStartScheduler调用以来的时钟节拍数
 *
 * 工作机制：
 * - 中断安全版本的xTaskGetTickCount
 * - 返回当前的系统时钟节拍计数值
 * - 适用于在中断服务例程中获取时钟节拍
 * 
 * 注意事项：
 * - 不能在任务中使用此函数
 * - 适用于中断上下文
 * 
 * 示例：
 * void vAnISR(void) {
 *     TickType_t xTickCount = xTaskGetTickCountFromISR();
 *     printf("Tick Count in ISR: %u\n", xTickCount);
 * }
 * 
 * \defgroup xTaskGetTickCountFromISR xTaskGetTickCountFromISR
 * \ingroup TaskUtils
 */
TickType_t xTaskGetTickCountFromISR( void ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>UBaseType_t uxTaskGetNumberOfTasks( void );</pre>
 *
 * @return 当前实时操作系统正在管理的任务数量
 * - 包括所有就绪、阻塞和挂起的任务
 * - 如果任务被删除但尚未由空闲任务释放，也会被计入
 * 
 * 工作机制：
 * - 统计当前系统中所有任务的数量
 * - 适用于需要监控系统任务数量的场景
 * 
 * 示例：
 * UBaseType_t uxNumTasks = uxTaskGetNumberOfTasks();
 * printf("Number of Tasks: %u\n", uxNumTasks);
 * 
 * \defgroup uxTaskGetNumberOfTasks uxTaskGetNumberOfTasks
 * \ingroup TaskUtils
 */
UBaseType_t uxTaskGetNumberOfTasks( void ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>char *pcTaskGetName( TaskHandle_t xTaskToQuery );</pre>
 *
 * @return 指定任务的人类可读名称
 * - 如果xTaskToQuery为NULL，则返回当前任务的名称
 * 
 * 工作机制：
 * - 返回任务的名称字符串
 * - 适用于调试和监控任务
 * 
 * 示例：
 * char *pcName = pcTaskGetName(xTaskHandle);
 * printf("Task Name: %s\n", pcName);
 * 
 * \defgroup pcTaskGetName pcTaskGetName
 * \ingroup TaskUtils
 */
char *pcTaskGetName( TaskHandle_t xTaskToQuery ) PRIVILEGED_FUNCTION; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

/**
 * task. h
 * <pre>TaskHandle_t xTaskGetHandle( const char *pcNameToQuery );</pre>
 *
 * 注意事项：
 * - 此函数执行时间较长，应谨慎使用
 * - INCLUDE_xTaskGetHandle需在FreeRTOSConfig.h中定义为1
 * 
 * @param pcNameToQuery 要查询的任务名称
 * @return 指定名称的任务句柄（如果未找到则返回NULL）
 * 
 * 工作机制：
 * - 根据任务名称查找并返回任务句柄
 * - 适用于需要通过名称获取任务句柄的场景
 * 
 * 示例：
 * TaskHandle_t xHandle = xTaskGetHandle("Example Task");
 * if(xHandle != NULL) {
 *     printf("Task Handle Found: %p\n", xHandle);
 * }
 * 
 * \defgroup xTaskGetHandle xTaskGetHandle
 * \ingroup TaskUtils
 */
TaskHandle_t xTaskGetHandle( const char *pcNameToQuery ) PRIVILEGED_FUNCTION; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

/**
 * task.h
 * <PRE>UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask );</PRE>
 *
 * INCLUDE_uxTaskGetStackHighWaterMark must be set to 1 in FreeRTOSConfig.h for
 * this function to be available.
 *
 * Returns the high water mark of the stack associated with xTask.  That is,
 * the minimum free stack space there has been (in words, so on a 32 bit machine
 * a value of 1 means 4 bytes) since the task started.  The smaller the returned
 * number the closer the task has come to overflowing its stack.
 *
 * @param xTask Handle of the task associated with the stack to be checked.
 * Set xTask to NULL to check the stack of the calling task.
 *
 * @return The smallest amount of free stack space there has been (in words, so
 * actual spaces on the stack rather than bytes) since the task referenced by
 * xTask was created.
 */
/*-----------------------------------------------------------
 * TASK UTILITIES
 *----------------------------------------------------------*/

/**
 * @fn UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t)
 * @brief 获取任务自创建以来的堆栈高水位线（最小剩余空间）
 * @ingroup TaskUtils
 * 
 * @param xTask 任务句柄（NULL表示当前任务）
 * @return 自任务创建以来的最小剩余堆栈空间（以字为单位）
 * 
 * 工作机制：
 * - 返回任务自创建以来从未使用的最小堆栈空间量
 * - 适用于检测堆栈溢出风险
 * 
 * 示例：
 * UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(xTaskHandle);
 * printf("High Water Mark: %u\n", uxHighWaterMark);
 */
UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/* When using trace macros it is sometimes necessary to include task.h before
FreeRTOS.h.  When this is done TaskHookFunction_t will not yet have been defined,
so the following two prototypes will cause a compilation error.  This can be
fixed by simply guarding against the inclusion of these two prototypes unless
they are explicitly required by the configUSE_APPLICATION_TASK_TAG configuration
constant. */
#ifdef configUSE_APPLICATION_TASK_TAG
    #if configUSE_APPLICATION_TASK_TAG == 1
        /**
         * @fn void vTaskSetApplicationTaskTag(TaskHandle_t, TaskHookFunction_t)
         * @brief 设置任务的应用程序钩子函数
         * @ingroup TaskCtrl
         * 
         * @param xTask 任务句柄（NULL表示当前任务）
         * @param pxHookFunction 钩子函数指针
         * 
         * 工作机制：
         * - 设置指定任务的应用程序钩子函数
         * - 钩子函数在任务切换时被调用
         * 
         * 示例：
         * void vHookFunction(void *pvParam) {
         *     // 钩子函数实现
         * }
         * vTaskSetApplicationTaskTag(xTaskHandle, vHookFunction); // 设置钩子函数
         * vTaskSetApplicationTaskTag(NULL, vHookFunction); // 设置当前任务的钩子函数
         */
        void vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxHookFunction ) PRIVILEGED_FUNCTION;

        /**
         * @fn TaskHookFunction_t xTaskGetApplicationTaskTag(TaskHandle_t)
         * @brief 获取任务的应用程序钩子函数
         * @ingroup TaskCtrl
         * 
         * @param xTask 任务句柄（NULL表示当前任务）
         * @return 任务的钩子函数指针
         * 
         * 工作机制：
         * - 返回指定任务的应用程序钩子函数
         * - 如果任务未设置钩子函数，返回NULL
         * 
         * 示例：
         * TaskHookFunction_t pxHook = xTaskGetApplicationTaskTag(xTaskHandle);
         * if(pxHook != NULL) {
         *     printf("Hook Function: %p\n", pxHook);
         * }
         */
        TaskHookFunction_t xTaskGetApplicationTaskTag( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;
    #endif /* configUSE_APPLICATION_TASK_TAG == 1 */
#endif /* ifdef configUSE_APPLICATION_TASK_TAG */

#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )

    /* 每个任务包含一个由configNUM_THREAD_LOCAL_STORAGE_POINTERS设置大小的指针数组。
       内核本身不使用这些指针，因此应用程序可以自由使用这些指针。
       以下两个函数用于设置和查询指针。 */
    /**
     * @fn void vTaskSetThreadLocalStoragePointer(TaskHandle_t, BaseType_t, void*)
     * @brief 设置任务的线程本地存储指针
     * @ingroup TaskUtils
     * 
     * @param xTaskToSet 任务句柄（NULL表示当前任务）
     * @param xIndex 线程本地存储数组索引（0 ~ configNUM_THREAD_LOCAL_STORAGE_POINTERS-1）
     * @param pvValue 要设置的指针值
     * 
     * 工作机制：
     * - 设置指定任务的线程本地存储指针
     * - 适用于存储任务特定的数据
     * 
     * 示例：
     * void *pData = malloc(sizeof(MyData_t));
     * vTaskSetThreadLocalStoragePointer(xTaskHandle, 0, pData); // 设置指针
     * vTaskSetThreadLocalStoragePointer(NULL, 0, pData); // 设置当前任务的指针
     */
    void vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue ) PRIVILEGED_FUNCTION;

    /**
     * @fn void* pvTaskGetThreadLocalStoragePointer(TaskHandle_t, BaseType_t)
     * @brief 获取任务的线程本地存储指针
     * @ingroup TaskUtils
     * 
     * @param xTaskToQuery 任务句柄（NULL表示当前任务）
     * @param xIndex 线程本地存储数组索引（0 ~ configNUM_THREAD_LOCAL_STORAGE_POINTERS-1）
     * @return 指定索引的指针值
     * 
     * 工作机制：
     * - 返回指定任务的线程本地存储指针
     * - 适用于访问任务特定的数据
     * 
     * 示例：
     * void *pData = pvTaskGetThreadLocalStoragePointer(xTaskHandle, 0);
     * if(pData != NULL) {
     *     // 使用pData
     * }
     */
    void *pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex ) PRIVILEGED_FUNCTION;

#endif

/**
 * @fn BaseType_t xTaskCallApplicationTaskHook(TaskHandle_t, void*)
 * @brief 调用任务的应用程序钩子函数
 * @ingroup TaskUtils
 * 
 * @param xTask 任务句柄（NULL表示当前任务）
 * @param pvParameter 传递给钩子函数的参数
 * @return 钩子函数返回的值
 * 
 * 工作机制：
 * - 调用指定任务的应用程序钩子函数
 * - 如果xTask为NULL，则调用当前任务的钩子函数
 * 
 * 示例：
 * BaseType_t xResult = xTaskCallApplicationTaskHook(xTaskHandle, pvParam);
 * if(xResult == pdTRUE) {
 *     // 钩子函数执行成功
 * }
 * BaseType_t xCurrentResult = xTaskCallApplicationTaskHook(NULL, pvParam);
 * if(xCurrentResult == pdTRUE) {
 *     // 当前任务的钩子函数执行成功
 * }
 */
BaseType_t xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter ) PRIVILEGED_FUNCTION;

/**
 * @fn TaskHandle_t xTaskGetIdleTaskHandle(void)
 * @brief 获取空闲任务的任务句柄
 * @ingroup TaskUtils
 * 
 * @param 无
 * @return 空闲任务的任务句柄
 * 
 * 工作机制：
 * - 返回空闲任务的任务句柄
 * - 仅在调度器启动后可用
 * 
 * 示例：
 * TaskHandle_t xIdleTaskHandle = xTaskGetIdleTaskHandle();
 * if(xIdleTaskHandle != NULL) {
 *     printf("Idle Task Handle: %p\n", xIdleTaskHandle);
 * }
 * 
 * 注意事项：
 * - INCLUDE_xTaskGetIdleTaskHandle需在FreeRTOSConfig.h中定义为1
 * - 不应在调度器启动前调用此函数
 */
TaskHandle_t xTaskGetIdleTaskHandle( void ) PRIVILEGED_FUNCTION;


/**
 * configUSE_TRACE_FACILITY must be defined as 1 in FreeRTOSConfig.h for
 * uxTaskGetSystemState() to be available.
 *
 * uxTaskGetSystemState() populates an TaskStatus_t structure for each task in
 * the system.  TaskStatus_t structures contain, among other things, members
 * for the task handle, task name, task priority, task state, and total amount
 * of run time consumed by the task.  See the TaskStatus_t structure
 * definition in this file for the full member list.
 *
 * NOTE:  This function is intended for debugging use only as its use results in
 * the scheduler remaining suspended for an extended period.
 *
 * @param pxTaskStatusArray A pointer to an array of TaskStatus_t structures.
 * The array must contain at least one TaskStatus_t structure for each task
 * that is under the control of the RTOS.  The number of tasks under the control
 * of the RTOS can be determined using the uxTaskGetNumberOfTasks() API function.
 *
 * @param uxArraySize The size of the array pointed to by the pxTaskStatusArray
 * parameter.  The size is specified as the number of indexes in the array, or
 * the number of TaskStatus_t structures contained in the array, not by the
 * number of bytes in the array.
 *
 * @param pulTotalRunTime If configGENERATE_RUN_TIME_STATS is set to 1 in
 * FreeRTOSConfig.h then *pulTotalRunTime is set by uxTaskGetSystemState() to the
 * total run time (as defined by the run time stats clock, see
 * http://www.freertos.org/rtos-run-time-stats.html) since the target booted.
 * pulTotalRunTime can be set to NULL to omit the total run time information.
 *
 * @return The number of TaskStatus_t structures that were populated by
 * uxTaskGetSystemState().  This should equal the number returned by the
 * uxTaskGetNumberOfTasks() API function, but will be zero if the value passed
 * in the uxArraySize parameter was too small.
 *
 * Example usage:
   <pre>
    // This example demonstrates how a human readable table of run time stats
	// information is generated from raw data provided by uxTaskGetSystemState().
	// The human readable table is written to pcWriteBuffer
	void vTaskGetRunTimeStats( char *pcWriteBuffer )
	{
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize, x;
	uint32_t ulTotalRunTime, ulStatsAsPercentage;

		// Make sure the write buffer does not contain a string.
		*pcWriteBuffer = 0x00;

		// Take a snapshot of the number of tasks in case it changes while this
		// function is executing.
		uxArraySize = uxTaskGetNumberOfTasks();

		// Allocate a TaskStatus_t structure for each task.  An array could be
		// allocated statically at compile time.
		pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

		if( pxTaskStatusArray != NULL )
		{
			// Generate raw status information about each task.
			uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );

			// For percentage calculations.
			ulTotalRunTime /= 100UL;

			// Avoid divide by zero errors.
			if( ulTotalRunTime > 0 )
			{
				// For each populated position in the pxTaskStatusArray array,
				// format the raw data as human readable ASCII data
				for( x = 0; x < uxArraySize; x++ )
				{
					// What percentage of the total run time has the task used?
					// This will always be rounded down to the nearest integer.
					// ulTotalRunTimeDiv100 has already been divided by 100.
					ulStatsAsPercentage = pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;

					if( ulStatsAsPercentage > 0UL )
					{
						sprintf( pcWriteBuffer, "%s\t\t%lu\t\t%lu%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter, ulStatsAsPercentage );
					}
					else
					{
						// If the percentage is zero here then the task has
						// consumed less than 1% of the total run time.
						sprintf( pcWriteBuffer, "%s\t\t%lu\t\t<1%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter );
					}

					pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );
				}
			}

			// The array is no longer needed, free the memory it consumes.
			vPortFree( pxTaskStatusArray );
		}
	}
	</pre>
 */
/*-----------------------------------------------------------
 * TASK UTILITIES
 *----------------------------------------------------------*/

/**
 * @fn UBaseType_t uxTaskGetSystemState(TaskStatus_t*, const UBaseType_t, uint32_t*)
 * @brief 获取系统中所有任务的状态信息
 * @ingroup TaskUtils
 * 
 * @param pxTaskStatusArray 指向TaskStatus_t结构体数组的指针，用于存储任务信息
 * @param uxArraySize pxTaskStatusArray数组的大小（以TaskStatus_t为单位）
 * @param pulTotalRunTime 指向uint32_t的指针，用于存储系统总运行时间（可选）
 * @return 实际填充的任务数量
 * 
 * 工作机制：
 * - 填充pxTaskStatusArray数组以包含所有任务的状态信息
 * - 包括任务优先级、状态、堆栈使用情况等
 * - 如果pulTotalRunTime不为NULL，则返回系统总运行时间
 * - 适用于系统状态监控和调试
 * 
 * 示例：
 * TaskStatus_t xTaskStatusArray[10];
 * uint32_t ulTotalRunTime;
 * UBaseType_t uxNumTasks = uxTaskGetSystemState(xTaskStatusArray, 10, &ulTotalRunTime);
 * printf("Number of Tasks: %u\n", uxNumTasks);
 * printf("Total Run Time: %lu\n", ulTotalRunTime);
 * 
 * 注意事项：
 * - 需要确保pxTaskStatusArray数组足够大以容纳所有任务信息
 * - 配置configUSE_TRACE_FACILITY必须在FreeRTOSConfig.h中定义为1
 * - pulTotalRunTime仅在启用configGENERATE_RUN_TIME_STATS时有效
 */
UBaseType_t uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray, 
                                  const UBaseType_t uxArraySize, 
                                  uint32_t * const pulTotalRunTime ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <pre>void vTaskList( char *pcWriteBuffer );</pre>
 *
 * configUSE_TRACE_FACILITY 和 configUSE_STATS_FORMATTING_FUNCTIONS 必须
 * 在FreeRTOSConfig.h中定义为1，才能使用此函数。
 * 
 * @param pcWriteBuffer 指向字符缓冲区的指针，用于存储任务列表信息
 * 
 * 工作机制：
 * - 生成一个包含所有任务状态的文本列表
 * - 输出到提供的字符缓冲区
 * - 适用于系统状态监控和调试
 * 
 * 示例：
 * char cBuffer[1024];
 * vTaskList(cBuffer);
 * printf("%s\n", cBuffer);
 * 
 * 注意事项：
 * - 需要确保pcWriteBuffer缓冲区足够大以容纳所有任务信息
 * - 配置configUSE_TRACE_FACILITY必须在FreeRTOSConfig.h中定义为1
 * - 配置configUSE_STATS_FORMATTING_FUNCTIONS必须在FreeRTOSConfig.h中定义为1
 */
void vTaskList( char *pcWriteBuffer ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <PRE>void vTaskGetRunTimeStats( char *pcWriteBuffer );</PRE>
 *
 * configGENERATE_RUN_TIME_STATS and configUSE_STATS_FORMATTING_FUNCTIONS
 * must both be defined as 1 for this function to be available.  The application
 * must also then provide definitions for
 * portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() and portGET_RUN_TIME_COUNTER_VALUE()
 * to configure a peripheral timer/counter and return the timers current count
 * value respectively.  The counter should be at least 10 times the frequency of
 * the tick count.
 *
 * NOTE 1: This function will disable interrupts for its duration.  It is
 * not intended for normal application runtime use but as a debug aid.
 *
 * Setting configGENERATE_RUN_TIME_STATS to 1 will result in a total
 * accumulated execution time being stored for each task.  The resolution
 * of the accumulated time value depends on the frequency of the timer
 * configured by the portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() macro.
 * Calling vTaskGetRunTimeStats() writes the total execution time of each
 * task into a buffer, both as an absolute count value and as a percentage
 * of the total system execution time.
 *
 * NOTE 2:
 *
 * This function is provided for convenience only, and is used by many of the
 * demo applications.  Do not consider it to be part of the scheduler.
 *
 * vTaskGetRunTimeStats() calls uxTaskGetSystemState(), then formats part of the
 * uxTaskGetSystemState() output into a human readable table that displays the
 * amount of time each task has spent in the Running state in both absolute and
 * percentage terms.
 *
 * vTaskGetRunTimeStats() has a dependency on the sprintf() C library function
 * that might bloat the code size, use a lot of stack, and provide different
 * results on different platforms.  An alternative, tiny, third party, and
 * limited functionality implementation of sprintf() is provided in many of the
 * FreeRTOS/Demo sub-directories in a file called printf-stdarg.c (note
 * printf-stdarg.c does not provide a full snprintf() implementation!).
 *
 * It is recommended that production systems call uxTaskGetSystemState() directly
 * to get access to raw stats data, rather than indirectly through a call to
 * vTaskGetRunTimeStats().
 *
 * @param pcWriteBuffer A buffer into which the execution times will be
 * written, in ASCII form.  This buffer is assumed to be large enough to
 * contain the generated report.  Approximately 40 bytes per task should
 * be sufficient.
 *
 * \defgroup vTaskGetRunTimeStats vTaskGetRunTimeStats
 * \ingroup TaskUtils
 */
/**
 * @fn void vTaskGetRunTimeStats(char*)
 * @brief 获取系统中所有任务的运行时间统计信息
 * @ingroup TaskUtils
 * 
 * @param pcWriteBuffer 指向字符缓冲区的指针，用于存储运行时间统计信息
 * 
 * 工作机制：
 * - 生成一个包含所有任务运行时间统计的文本列表
 * - 输出到提供的字符缓冲区
 * - 适用于性能分析和任务运行时间监控
 * 
 * 示例：
 * char cBuffer[1024];
 * vTaskGetRunTimeStats(cBuffer);
 * printf("%s\n", cBuffer);
 * 
 * 注意事项：
 * - 需要确保pcWriteBuffer缓冲区足够大以容纳所有任务信息
 * - 配置configGENERATE_RUN_TIME_STATS必须在FreeRTOSConfig.h中定义为1
 * - 此函数仅在调度器启动后有效
 */
void vTaskGetRunTimeStats( char *pcWriteBuffer ) PRIVILEGED_FUNCTION; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

/**
 * task. h
 * <PRE>BaseType_t xTaskNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction );</PRE>
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this
 * function to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * A notification sent to a task will remain pending until it is cleared by the
 * task calling xTaskNotifyWait() or ulTaskNotifyTake().  If the task was
 * already in the Blocked state to wait for a notification when the notification
 * arrives then the task will automatically be removed from the Blocked state
 * (unblocked) and the notification cleared.
 *
 * A task can use xTaskNotifyWait() to [optionally] block to wait for a
 * notification to be pending, or ulTaskNotifyTake() to [optionally] block
 * to wait for its notification value to have a non-zero value.  The task does
 * not consume any CPU time while it is in the Blocked state.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for details.
 *
 * @param xTaskToNotify The handle of the task being notified.  The handle to a
 * task can be returned from the xTaskCreate() API function used to create the
 * task, and the handle of the currently running task can be obtained by calling
 * xTaskGetCurrentTaskHandle().
 *
 * @param ulValue Data that can be sent with the notification.  How the data is
 * used depends on the value of the eAction parameter.
 *
 * @param eAction Specifies how the notification updates the task's notification
 * value, if at all.  Valid values for eAction are as follows:
 *
 * eSetBits -
 * The task's notification value is bitwise ORed with ulValue.  xTaskNofify()
 * always returns pdPASS in this case.
 *
 * eIncrement -
 * The task's notification value is incremented.  ulValue is not used and
 * xTaskNotify() always returns pdPASS in this case.
 *
 * eSetValueWithOverwrite -
 * The task's notification value is set to the value of ulValue, even if the
 * task being notified had not yet processed the previous notification (the
 * task already had a notification pending).  xTaskNotify() always returns
 * pdPASS in this case.
 *
 * eSetValueWithoutOverwrite -
 * If the task being notified did not already have a notification pending then
 * the task's notification value is set to ulValue and xTaskNotify() will
 * return pdPASS.  If the task being notified already had a notification
 * pending then no action is performed and pdFAIL is returned.
 *
 * eNoAction -
 * The task receives a notification without its notification value being
 * updated.  ulValue is not used and xTaskNotify() always returns pdPASS in
 * this case.
 *
 *  pulPreviousNotificationValue -
 *  Can be used to pass out the subject task's notification value before any
 *  bits are modified by the notify function.
 *
 * @return Dependent on the value of eAction.  See the description of the
 * eAction parameter.
 *
 * \defgroup xTaskNotify xTaskNotify
 * \ingroup TaskNotifications
 */
/**
 * @brief 任务通知机制核心函数（带返回值查询）
 * 
 * 本函数是FreeRTOS任务通知系统的底层实现，提供完整的通知操作控制。
 * 通过组合不同参数可以实现：设置/清除位、数值覆盖/递增、无操作查询等不同功能
 * 
 * @param xTaskToNotify 目标任务句柄（接收通知的任务）
 * @param ulValue 通知值，具体含义取决于eAction参数：
 *                - 当动作为eSetBits时：位掩码值
 *                - 当动作为eIncrement时：递增值（通常设为1）
 *                - 当动作为eSetValueWithOverwrite/NoOverwrite时：设置的具体数值
 * @param eAction 通知动作类型，详见eNotifyAction枚举定义：
 *                - eNoAction：仅查询当前值（不修改）
 *                - eSetBits：按位或操作
 *                - eIncrement：递增操作
 *                - eSetValueWithOverwrite：强制覆盖
 *                - eSetValueWithoutOverwrite：有条件覆盖（仅当任务尚未处理前一个通知时）
 * @param pulPreviousNotificationValue 用于返回前一个通知值的指针（可NULL）
 * @return BaseType_t 操作结果：
 *         - pdPASS：操作成功（对于覆盖模式，表示实际完成写入）
 *         - pdFAIL：操作失败（通常发生在eSetValueWithoutOverwrite时目标任务已有未处理通知）
 */
BaseType_t xTaskGenericNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue ) PRIVILEGED_FUNCTION;

/**
 * @brief 简化版任务通知宏（不带返回值查询）
 * 
 * 该宏在底层调用xTaskGenericNotify时省略了pulPreviousNotificationValue参数，
 * 适用于不需要获取前一个通知值的场景，提高代码简洁性
 * 
 * @note 与xTaskNotifyAndQuery的区别在于不返回之前的通知状态
 */
#define xTaskNotify( xTaskToNotify, ulValue, eAction ) xTaskGenericNotify( ( xTaskToNotify ), ( ulValue ), ( eAction ), NULL )

/**
 * @brief 完整功能任务通知宏（带返回值查询）
 * 
 * 该宏在底层调用xTaskGenericNotify时保留了pulPreviousNotificationValue参数，
 * 允许调用者获取通知前的任务状态，适用于需要状态跟踪的复杂场景
 * 
 * @note 参数命名与底层函数保持一致性（pulPreviousNotifyValue对应pulPreviousNotificationValue）
 */
#define xTaskNotifyAndQuery( xTaskToNotify, ulValue, eAction, pulPreviousNotifyValue ) xTaskGenericNotify( ( xTaskToNotify ), ( ulValue ), ( eAction ), ( pulPreviousNotifyValue ) )

/**
 * task. h
 * <PRE>BaseType_t xTaskNotifyFromISR( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken );</PRE>
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this
 * function to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * A version of xTaskNotify() that can be used from an interrupt service routine
 * (ISR).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * A notification sent to a task will remain pending until it is cleared by the
 * task calling xTaskNotifyWait() or ulTaskNotifyTake().  If the task was
 * already in the Blocked state to wait for a notification when the notification
 * arrives then the task will automatically be removed from the Blocked state
 * (unblocked) and the notification cleared.
 *
 * A task can use xTaskNotifyWait() to [optionally] block to wait for a
 * notification to be pending, or ulTaskNotifyTake() to [optionally] block
 * to wait for its notification value to have a non-zero value.  The task does
 * not consume any CPU time while it is in the Blocked state.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for details.
 *
 * @param xTaskToNotify The handle of the task being notified.  The handle to a
 * task can be returned from the xTaskCreate() API function used to create the
 * task, and the handle of the currently running task can be obtained by calling
 * xTaskGetCurrentTaskHandle().
 *
 * @param ulValue Data that can be sent with the notification.  How the data is
 * used depends on the value of the eAction parameter.
 *
 * @param eAction Specifies how the notification updates the task's notification
 * value, if at all.  Valid values for eAction are as follows:
 *
 * eSetBits -
 * The task's notification value is bitwise ORed with ulValue.  xTaskNofify()
 * always returns pdPASS in this case.
 *
 * eIncrement -
 * The task's notification value is incremented.  ulValue is not used and
 * xTaskNotify() always returns pdPASS in this case.
 *
 * eSetValueWithOverwrite -
 * The task's notification value is set to the value of ulValue, even if the
 * task being notified had not yet processed the previous notification (the
 * task already had a notification pending).  xTaskNotify() always returns
 * pdPASS in this case.
 *
 * eSetValueWithoutOverwrite -
 * If the task being notified did not already have a notification pending then
 * the task's notification value is set to ulValue and xTaskNotify() will
 * return pdPASS.  If the task being notified already had a notification
 * pending then no action is performed and pdFAIL is returned.
 *
 * eNoAction -
 * The task receives a notification without its notification value being
 * updated.  ulValue is not used and xTaskNotify() always returns pdPASS in
 * this case.
 *
 * @param pxHigherPriorityTaskWoken  xTaskNotifyFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending the notification caused the
 * task to which the notification was sent to leave the Blocked state, and the
 * unblocked task has a priority higher than the currently running task.  If
 * xTaskNotifyFromISR() sets this value to pdTRUE then a context switch should
 * be requested before the interrupt is exited.  How a context switch is
 * requested from an ISR is dependent on the port - see the documentation page
 * for the port in use.
 *
 * @return Dependent on the value of eAction.  See the description of the
 * eAction parameter.
 *
 * \defgroup xTaskNotify xTaskNotify
 * \ingroup TaskNotifications
 */
/**
 * @brief 中断安全版任务通知核心函数（带返回值查询）
 * 
 * 本函数是FreeRTOS任务通知系统的中断服务例程(ISR)专用版本，
 * 提供在中断上下文中进行任务通知操作的能力，支持中断延迟处理机制
 * 
 * @param xTaskToNotify 目标任务句柄（接收通知的任务）
 * @param ulValue 通知值，具体语义取决于eAction参数：
 *               - eSetBits：位掩码值
 *               - eIncrement：递增值（通常为1）
 *               - eSetValue...：设置的具体数值
 * @param eAction 通知动作类型，详见eNotifyAction枚举
 * @param pulPreviousNotificationValue 用于返回前一个通知值的指针（可NULL）
 * @param pxHigherPriorityTaskWoken 用于延迟上下文切换的标记指针：
 *               - 当设为非NULL时，函数可能将其设为pdTRUE
 *               - 退出ISR后需根据该值调用portYIELD_FROM_ISR()
 * @return BaseType_t 操作结果：
 *         - pdPASS：操作成功
 *         - pdFAIL：操作失败（仅当eSetValueWithoutOverwrite且目标已有未处理通知时）
 */
BaseType_t xTaskGenericNotifyFromISR( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue, BaseType_t *pxHigherPriorityTaskWoken ) PRIVILEGED_FUNCTION;

/**
 * @brief 简化版中断安全任务通知宏（不带返回值查询）
 * 
 * 该宏在底层调用xTaskGenericNotifyFromISR时省略了pulPreviousNotificationValue，
 * 适用于ISR中不需要获取前一个通知值的场景
 * 
 * @note 必须确保pxHigherPriorityTaskWoken参数有效，并在ISR退出时处理可能的上下文切换
 */
#define xTaskNotifyFromISR( xTaskToNotify, ulValue, eAction, pxHigherPriorityTaskWoken ) \
    xTaskGenericNotifyFromISR( ( xTaskToNotify ), ( ulValue ), ( eAction ), NULL, ( pxHigherPriorityTaskWoken ) )

/**
 * @brief 完整功能中断安全任务通知宏（带返回值查询）
 * 
 * 该宏保留了pulPreviousNotificationValue参数，允许在ISR中查询通知前的任务状态，
 * 适用于需要精确状态跟踪的中断处理场景
 * 
 * @warning 在ISR中访问返回的pulPreviousNotificationValue时需确保原子操作
 */
#define xTaskNotifyAndQueryFromISR( xTaskToNotify, ulValue, eAction, pulPreviousNotificationValue, pxHigherPriorityTaskWoken ) \
    xTaskGenericNotifyFromISR( ( xTaskToNotify ), ( ulValue ), ( eAction ), ( pulPreviousNotificationValue ), ( pxHigherPriorityTaskWoken ) )

/**
 * task. h
 * <PRE>BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait );</pre>
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this
 * function to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * A notification sent to a task will remain pending until it is cleared by the
 * task calling xTaskNotifyWait() or ulTaskNotifyTake().  If the task was
 * already in the Blocked state to wait for a notification when the notification
 * arrives then the task will automatically be removed from the Blocked state
 * (unblocked) and the notification cleared.
 *
 * A task can use xTaskNotifyWait() to [optionally] block to wait for a
 * notification to be pending, or ulTaskNotifyTake() to [optionally] block
 * to wait for its notification value to have a non-zero value.  The task does
 * not consume any CPU time while it is in the Blocked state.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for details.
 *
 * @param ulBitsToClearOnEntry Bits that are set in ulBitsToClearOnEntry value
 * will be cleared in the calling task's notification value before the task
 * checks to see if any notifications are pending, and optionally blocks if no
 * notifications are pending.  Setting ulBitsToClearOnEntry to ULONG_MAX (if
 * limits.h is included) or 0xffffffffUL (if limits.h is not included) will have
 * the effect of resetting the task's notification value to 0.  Setting
 * ulBitsToClearOnEntry to 0 will leave the task's notification value unchanged.
 *
 * @param ulBitsToClearOnExit If a notification is pending or received before
 * the calling task exits the xTaskNotifyWait() function then the task's
 * notification value (see the xTaskNotify() API function) is passed out using
 * the pulNotificationValue parameter.  Then any bits that are set in
 * ulBitsToClearOnExit will be cleared in the task's notification value (note
 * *pulNotificationValue is set before any bits are cleared).  Setting
 * ulBitsToClearOnExit to ULONG_MAX (if limits.h is included) or 0xffffffffUL
 * (if limits.h is not included) will have the effect of resetting the task's
 * notification value to 0 before the function exits.  Setting
 * ulBitsToClearOnExit to 0 will leave the task's notification value unchanged
 * when the function exits (in which case the value passed out in
 * pulNotificationValue will match the task's notification value).
 *
 * @param pulNotificationValue Used to pass the task's notification value out
 * of the function.  Note the value passed out will not be effected by the
 * clearing of any bits caused by ulBitsToClearOnExit being non-zero.
 *
 * @param xTicksToWait The maximum amount of time that the task should wait in
 * the Blocked state for a notification to be received, should a notification
 * not already be pending when xTaskNotifyWait() was called.  The task
 * will not consume any processing time while it is in the Blocked state.  This
 * is specified in kernel ticks, the macro pdMS_TO_TICSK( value_in_ms ) can be
 * used to convert a time specified in milliseconds to a time specified in
 * ticks.
 *
 * @return If a notification was received (including notifications that were
 * already pending when xTaskNotifyWait was called) then pdPASS is
 * returned.  Otherwise pdFAIL is returned.
 *
 * \defgroup xTaskNotifyWait xTaskNotifyWait
 * \ingroup TaskNotifications
 */
/**
 * @brief 任务通知等待函数（带位清除控制）
 * 
 * 本函数实现任务级通知等待机制，允许任务阻塞自身直到收到通知或超时发生。
 * 支持原子化的位清除操作，适用于实现轻量级事件等待和同步场景。
 * 
 * @param ulBitsToClearOnEntry 进入等待前清除的位掩码：
 *        - 位掩码中设置为1的位将在检查通知前被清除
 *        - 例如：0xFFFFFFFF表示清除所有位
 *        - 设置为0则不进行任何清除
 * @param ulBitsToClearOnExit 退出等待前清除的位掩码：
 *        - 仅在成功获取通知后执行清除
 *        - 清除操作在返回给调用者之前完成
 * @param pulNotificationValue 通知值存储指针（可NULL）：
 *        - 当非NULL时，存储接收到的通知值
 *        - 存储的值是清除ulBitsToClearOnExit之前的原始值
 * @param xTicksToWait 最大等待时间（单位：时钟节拍）：
 *        - 可使用portMAX_DELAY实现无限等待
 *        - 设置为0则立即返回（非阻塞模式）
 * @return BaseType_t 操作结果：
 *         - pdPASS：成功获取通知
 *         - pdFAIL：等待超时或参数错误
 *
 * @note 使用限制：
 * - 必须从任务上下文中调用（禁止在ISR中使用）
 * - 需要配合xTaskNotify/xTaskNotifyGive系列函数使用
 * - 清除位操作仅影响通知值的位域部分
 */
BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, 
                           uint32_t ulBitsToClearOnExit,
                           uint32_t *pulNotificationValue,
                           TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <PRE>BaseType_t xTaskNotifyGive( TaskHandle_t xTaskToNotify );</PRE>
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this macro
 * to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * xTaskNotifyGive() is a helper macro intended for use when task notifications
 * are used as light weight and faster binary or counting semaphore equivalents.
 * Actual FreeRTOS semaphores are given using the xSemaphoreGive() API function,
 * the equivalent action that instead uses a task notification is
 * xTaskNotifyGive().
 *
 * When task notifications are being used as a binary or counting semaphore
 * equivalent then the task being notified should wait for the notification
 * using the ulTaskNotificationTake() API function rather than the
 * xTaskNotifyWait() API function.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for more details.
 *
 * @param xTaskToNotify The handle of the task being notified.  The handle to a
 * task can be returned from the xTaskCreate() API function used to create the
 * task, and the handle of the currently running task can be obtained by calling
 * xTaskGetCurrentTaskHandle().
 *
 * @return xTaskNotifyGive() is a macro that calls xTaskNotify() with the
 * eAction parameter set to eIncrement - so pdPASS is always returned.
 *
 * \defgroup xTaskNotifyGive xTaskNotifyGive
 * \ingroup TaskNotifications
 */
/**
 * @brief 任务通知计数递送宏（简化版信号量）
 * 
 * 本宏通过任务通知机制实现轻量级计数操作，等效于二进制信号量的give操作。
 * 每次调用将使目标任务的通知值+1，且总是返回pdPASS。
 * 
 * @param xTaskToNotify 目标任务句柄：
 *        - 必须为有效任务句柄（通过xTaskCreate/xTaskGetHandle获取）
 *        - 传递NULL将导致内存访问异常
 * 
 * @return BaseType_t 固定返回pdPASS（保持API兼容性）
 * 
 * @note 典型使用场景：
 * - 生产者任务递送事件计数
 * - 替代二值信号量/计数信号量
 * - 轻量级任务同步（1:1模型）
 * 
 * @code
 * // 发送端
 * xTaskNotifyGive(xConsumerTask);  // 等效信号量give
 * 
 * // 接收端
 * ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // 等效信号量take
 * @endcode
 * 
 * @warning 使用限制：
 * 1. 通知值溢出未定义（32位计数器最大支持4294967295次递送）
 * 2. 不能用于中断上下文（ISR中应使用vTaskNotifyGiveFromISR）
 * 3. 不支持优先级继承（传统信号量优势场景）
 */
#define xTaskNotifyGive( xTaskToNotify ) \
    xTaskGenericNotify( ( xTaskToNotify ), ( 0 ), eIncrement, NULL )

/**
 * task. h
 * <PRE>void vTaskNotifyGiveFromISR( TaskHandle_t xTaskHandle, BaseType_t *pxHigherPriorityTaskWoken );
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this macro
 * to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * A version of xTaskNotifyGive() that can be called from an interrupt service
 * routine (ISR).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * vTaskNotifyGiveFromISR() is intended for use when task notifications are
 * used as light weight and faster binary or counting semaphore equivalents.
 * Actual FreeRTOS semaphores are given from an ISR using the
 * xSemaphoreGiveFromISR() API function, the equivalent action that instead uses
 * a task notification is vTaskNotifyGiveFromISR().
 *
 * When task notifications are being used as a binary or counting semaphore
 * equivalent then the task being notified should wait for the notification
 * using the ulTaskNotificationTake() API function rather than the
 * xTaskNotifyWait() API function.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for more details.
 *
 * @param xTaskToNotify The handle of the task being notified.  The handle to a
 * task can be returned from the xTaskCreate() API function used to create the
 * task, and the handle of the currently running task can be obtained by calling
 * xTaskGetCurrentTaskHandle().
 *
 * @param pxHigherPriorityTaskWoken  vTaskNotifyGiveFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending the notification caused the
 * task to which the notification was sent to leave the Blocked state, and the
 * unblocked task has a priority higher than the currently running task.  If
 * vTaskNotifyGiveFromISR() sets this value to pdTRUE then a context switch
 * should be requested before the interrupt is exited.  How a context switch is
 * requested from an ISR is dependent on the port - see the documentation page
 * for the port in use.
 *
 * \defgroup xTaskNotifyWait xTaskNotifyWait
 * \ingroup TaskNotifications
 */
/**
 * @brief 中断安全版任务通知递送函数
 * 
 * 本函数用于在中断服务例程(ISR)中递送任务通知，等效于中断版的xTaskNotifyGive()。
 * 通过递增目标任务的32位通知值实现轻量级信号量操作，支持延迟上下文切换机制。
 * 
 * @param xTaskToNotify 目标任务句柄：
 *        - 必须为已创建的有效任务句柄
 *        - 传递NULL将导致内存访问异常
 * @param pxHigherPriorityTaskWoken 上下文切换标记指针：
 *        - 输出参数，可能被设为pdTRUE
 *        - ISR退出时必须检查并调用portYIELD_FROM_ISR()
 * 
 * @note 典型使用场景：
 * - ISR向任务递送事件计数
 * - 替代中断安全版信号量give操作
 * 
 * @code
 * // 中断服务例程示例
 * void vUART_ISR(void) {
 *     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *     
 *     // 清除中断标志
 *     UART_CLEAR_IT();
 *     
 *     // 递送通知给数据处理任务
 *     vTaskNotifyGiveFromISR(xDataTask, &xHigherPriorityTaskWoken);
 *     
 *     // 必要时触发上下文切换
 *     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
 * }
 * @endcode
 * 
 * @warning 关键限制：
 * 1. 必须与ulTaskNotifyTake()配对使用
 * 2. 通知值溢出不会触发错误（32位循环计数）
 * 3. 禁止在任务上下文中调用
 */
void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, 
                            BaseType_t *pxHigherPriorityTaskWoken ) PRIVILEGED_FUNCTION;

/**
 * task. h
 * <PRE>uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait );</pre>
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this
 * function to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * ulTaskNotifyTake() is intended for use when a task notification is used as a
 * faster and lighter weight binary or counting semaphore alternative.  Actual
 * FreeRTOS semaphores are taken using the xSemaphoreTake() API function, the
 * equivalent action that instead uses a task notification is
 * ulTaskNotifyTake().
 *
 * When a task is using its notification value as a binary or counting semaphore
 * other tasks should send notifications to it using the xTaskNotifyGive()
 * macro, or xTaskNotify() function with the eAction parameter set to
 * eIncrement.
 *
 * ulTaskNotifyTake() can either clear the task's notification value to
 * zero on exit, in which case the notification value acts like a binary
 * semaphore, or decrement the task's notification value on exit, in which case
 * the notification value acts like a counting semaphore.
 *
 * A task can use ulTaskNotifyTake() to [optionally] block to wait for a
 * the task's notification value to be non-zero.  The task does not consume any
 * CPU time while it is in the Blocked state.
 *
 * Where as xTaskNotifyWait() will return when a notification is pending,
 * ulTaskNotifyTake() will return when the task's notification value is
 * not zero.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for details.
 *
 * @param xClearCountOnExit if xClearCountOnExit is pdFALSE then the task's
 * notification value is decremented when the function exits.  In this way the
 * notification value acts like a counting semaphore.  If xClearCountOnExit is
 * not pdFALSE then the task's notification value is cleared to zero when the
 * function exits.  In this way the notification value acts like a binary
 * semaphore.
 *
 * @param xTicksToWait The maximum amount of time that the task should wait in
 * the Blocked state for the task's notification value to be greater than zero,
 * should the count not already be greater than zero when
 * ulTaskNotifyTake() was called.  The task will not consume any processing
 * time while it is in the Blocked state.  This is specified in kernel ticks,
 * the macro pdMS_TO_TICSK( value_in_ms ) can be used to convert a time
 * specified in milliseconds to a time specified in ticks.
 *
 * @return The task's notification count before it is either cleared to zero or
 * decremented (see the xClearCountOnExit parameter).
 *
 * \defgroup ulTaskNotifyTake ulTaskNotifyTake
 * \ingroup TaskNotifications
 */
/**
 * @brief 任务通知值获取函数（带状态清除控制）
 * 
 * 本函数实现任务通知值的原子化获取，支持清零计数器和超时等待机制，
 * 是任务通知系统中接收端的核心操作。
 * 
 * @param xClearCountOnExit 退出时清零控制：
 *        - pdTRUE: 返回前将通知值重置为0
 *        - pdFALSE: 保持通知值不变（仅递减获取值）
 * @param xTicksToWait 最大等待时间（单位：时钟节拍）：
 *        - portMAX_DELAY表示无限等待
 *        - 0表示非阻塞模式
 * @return uint32_t 实际获取的通知值：
 *         - ≥1：成功获取通知
 *         - 0：等待超时或错误
 * 
 * @note 典型使用模式：
 * @code
 * // 等待通知并清零计数器（类似二值信号量）
 * if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY) > 0) {
 *     // 处理事件
 * }
 * 
 * // 处理累积计数（类似计数信号量）
 * uint32_t ulCount = ulTaskNotifyTake(pdFALSE, 0);
 * if(ulCount > 0) {
 *     process_items(ulCount);
 * }
 * @endcode
 */
uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

/**
 * @brief 任务通知状态清除函数
 * 
 * 本函数用于强制清除任务的通知等待状态，将任务状态从"已通知"改为"未等待通知"，
 * 但不修改任务的实际通知值。适用于取消任务通知等待的场景。
 * 
 * @param xTask 目标任务句柄：
 *        - 指定其他任务时需确保访问权限
 *        - NULL表示清除调用任务自身状态
 * @return BaseType_t 状态清除结果：
 *         - pdTRUE：状态成功变更
 *         - pdFALSE：当前状态无需变更
 * 
 * @warning 使用场景：
 * - 任务因超时/错误需要主动退出通知等待
 * - 系统需要强制终止某个任务的通知等待
 * 
 * @code
 * // 任务自身取消等待
 * xTaskNotifyStateClear(NULL);
 * 
 * // 管理器任务强制清除其他任务状态
 * if(xTaskNotifyStateClear(xTargetTask)) {
 *     log("Task%d通知状态已重置", xTargetTask);
 * }
 * @endcode
 */
BaseType_t xTaskNotifyStateClear( TaskHandle_t xTask );


/*-----------------------------------------------------------
 * SCHEDULER INTERNALS AVAILABLE FOR PORTING PURPOSES
 *----------------------------------------------------------*/

/*
 * THIS FUNCTION MUST NOT BE USED FROM APPLICATION CODE.  IT IS ONLY
 * INTENDED FOR USE WHEN IMPLEMENTING A PORT OF THE SCHEDULER AND IS
 * AN INTERFACE WHICH IS FOR THE EXCLUSIVE USE OF THE SCHEDULER.
 *
 * Called from the real time kernel tick (either preemptive or cooperative),
 * this increments the tick count and checks if any tasks that are blocked
 * for a finite period required removing from a blocked list and placing on
 * a ready list.  If a non-zero value is returned then a context switch is
 * required because either:
 *   + A task was removed from a blocked list because its timeout had expired,
 *     or
 *   + Time slicing is in use and there is a task of equal priority to the
 *     currently running task.
 */
/**
 * @brief 系统节拍计数器递增函数（调度器核心）
 * 
 * 本函数由SysTick中断调用，用于推进内核时钟和管理时间敏感任务状态。
 * 
 * @return BaseType_t 上下文切换需求标记：
 *         - pdTRUE: 需要立即进行上下文切换
 *         - pdFALSE: 可维持当前任务运行
 * 
 * @warning 严格禁止应用层调用
 * @note 核心职责：
 * 1. 更新系统节拍计数器xTickCount
 * 2. 处理延迟任务唤醒
 * 3. 检测时间片轮转需求
 * 4. 管理阻塞任务超时
 */
BaseType_t xTaskIncrementTick( void ) PRIVILEGED_FUNCTION;

/**
 * @brief 有序事件列表任务挂接函数（优先级排序）
 * 
 * 将当前任务从就绪列表移除，并按任务优先级插入指定事件列表，
 * 同时挂接到延迟列表实现超时机制。
 * 
 * @param pxEventList 事件列表指针（需已初始化）
 * @param xTicksToWait 最大阻塞时间（单位：时钟节拍）
 * 
 * @warning 调用前提：
 * - 必须在临界段内调用（中断禁用）
 * - 禁止在应用代码中直接调用
 * 
 * @code
 * // 调度器内部典型调用（信号量Pend操作）
 * vTaskPlaceOnEventList(&xSemaphore->xTasksWaitingToReceive, xBlockTime);
 * @endcode
 */
void vTaskPlaceOnEventList( List_t * const pxEventList,
                           const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

/**
 * @brief 无序事件列表任务挂接函数（自定义排序）
 * 
 * 将当前任务按指定xItemValue值插入事件列表尾部，
 * 用于实现先进先出(FIFO)或自定义排序机制。
 * 
 * @param pxEventList 事件列表指针
 * @param xItemValue 列表项排序值（非优先级）
 * @param xTicksToWait 最大阻塞时间
 * 
 * @note 与有序版本的区别：
 * | 特性         | 有序版本               | 无序版本               |
 * |--------------|------------------------|------------------------|
 * | 排序依据     | 任务优先级            | xItemValue指定值       |
 * | 插入位置     | 按优先级排序          | 列表尾部               |
 * | 典型应用场景 | 信号量/互斥量         | 消息队列/FIFO事件      |
 */
void vTaskPlaceOnUnorderedEventList( List_t * pxEventList,
                                    const TickType_t xItemValue,
                                    const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

/**
 * @brief 受限事件列表任务挂接函数（禁止无限等待）
 * 
 * 特殊版本的事件列表挂接函数，强制要求指定最大等待时间，
 * 用于实现不允许永久阻塞的同步机制。
 * 
 * @param pxEventList 事件列表指针
 * @param xTicksToWait 最大阻塞时间（必须>0）
 * @param xWaitIndefinitely 保留参数（必须设pdFALSE）
 * 
 * @warning 严格限制：
 * - xTicksToWait必须设置为具体数值（不能使用portMAX_DELAY）
 * - xWaitIndefinitely参数仅用于兼容性，必须传pdFALSE
 * 
 * @code
 * // 调度器内部超时控制调用
 * vTaskPlaceOnEventListRestricted(xEventList, xMaxBlockTime, pdFALSE);
 * @endcode
 */
void vTaskPlaceOnEventListRestricted( List_t * const pxEventList,
                                     TickType_t xTicksToWait,
                                     const BaseType_t xWaitIndefinitely ) PRIVILEGED_FUNCTION;

/*
 * THIS FUNCTION MUST NOT BE USED FROM APPLICATION CODE.  IT IS AN
 * INTERFACE WHICH IS FOR THE EXCLUSIVE USE OF THE SCHEDULER.
 *
 * THIS FUNCTION MUST BE CALLED WITH INTERRUPTS DISABLED.
 *
 * Removes a task from both the specified event list and the list of blocked
 * tasks, and places it on a ready queue.
 *
 * xTaskRemoveFromEventList()/xTaskRemoveFromUnorderedEventList() will be called
 * if either an event occurs to unblock a task, or the block timeout period
 * expires.
 *
 * xTaskRemoveFromEventList() is used when the event list is in task priority
 * order.  It removes the list item from the head of the event list as that will
 * have the highest priority owning task of all the tasks on the event list.
 * xTaskRemoveFromUnorderedEventList() is used when the event list is not
 * ordered and the event list items hold something other than the owning tasks
 * priority.  In this case the event list item value is updated to the value
 * passed in the xItemValue parameter.
 *
 * @return pdTRUE if the task being removed has a higher priority than the task
 * making the call, otherwise pdFALSE.
 */
/**
 * @brief 事件列表任务唤醒函数（有序列表）
 * 
 * 从按优先级排序的事件列表中唤醒最高优先级任务，
 * 通常在有事件发生时由内核对象（如信号量）调用。
 * 
 * @param pxEventList 事件列表指针
 * @return BaseType_t 唤醒的任务优先级是否高于当前任务：
 *         - pdTRUE：需要触发上下文切换
 *         - pdFALSE：可维持当前任务运行
 */
BaseType_t xTaskRemoveFromEventList( const List_t * const pxEventList ) PRIVILEGED_FUNCTION;

/**
 * @brief 事件列表任务唤醒函数（无序列表）
 * 
 * 从自定义排序的事件列表中唤醒指定任务，
 * 通过直接操作列表项实现高效唤醒。
 * 
 * @param pxEventListItem 要唤醒的任务列表项
 * @param xItemValue 新的事件项值
 * @return BaseType_t 同xTaskRemoveFromEventList
 */
BaseType_t xTaskRemoveFromUnorderedEventList( ListItem_t * pxEventListItem, 
                                             const TickType_t xItemValue ) PRIVILEGED_FUNCTION;

/**
 * @brief 上下文切换核心函数
 * 
 * 选择最高优先级就绪任务进行切换，
 * 包含栈指针切换和任务状态维护。
 * 
 * @warning 严格禁止应用层调用
 * @note 调用时需保证在临界区内
 */
void vTaskSwitchContext( void ) PRIVILEGED_FUNCTION;

/**
 * @brief 事件位组专用值重置函数
 * 
 * 获取并重置当前任务的事件项值，
 * 用于事件位组的等待逻辑实现。
 * 
 * @return TickType_t 原始事件项值
 */
TickType_t uxTaskResetEventItemValue( void ) PRIVILEGED_FUNCTION;

/**
 * @brief 获取当前任务句柄
 * 
 * @return TaskHandle_t 调用任务的TCB指针
 */
TaskHandle_t xTaskGetCurrentTaskHandle( void ) PRIVILEGED_FUNCTION;

/**
 * @brief 超时状态记录函数
 * 
 * 记录当前系统时间和剩余超时值，
 * 用于实现可重入的超时检测机制。
 * 
 * @param pxTimeOut 超时状态结构体指针
 */
void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut ) PRIVILEGED_FUNCTION;

/**
 * @brief 超时状态检测函数
 * 
 * @param pxTimeOut 已记录的超时状态
 * @param pxTicksToWait 剩余等待时间指针（动态更新）
 * @return BaseType_t 是否超时：
 *         - pdTRUE：超时发生
 *         - pdFALSE：仍在等待期内
 */
BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, 
                                TickType_t * const pxTicksToWait ) PRIVILEGED_FUNCTION;

/**
 * @brief 上下文切换遗漏标记
 * 
 * 当队列操作可能导致错过必要切换时，
 * 由内核调用以修正调度状态。
 */
void vTaskMissedYield( void ) PRIVILEGED_FUNCTION;

/**
 * @brief 调度器状态查询
 * 
 * @return BaseType_t 状态枚举：
 *         - taskSCHEDULER_RUNNING    正常运行
 *         - taskSCHEDULER_SUSPENDED  已挂起
 *         - taskSCHEDULER_NOT_STARTED 未启动
 */
BaseType_t xTaskGetSchedulerState( void ) PRIVILEGED_FUNCTION;

/**
 * @brief 优先级继承实施函数
 * 
 * 当高优先级任务请求被占用的互斥量时，
 * 临时提升持有者任务优先级。
 * 
 * @param pxMutexHolder 互斥量持有任务句柄
 */
void vTaskPriorityInherit( TaskHandle_t const pxMutexHolder ) PRIVILEGED_FUNCTION;

/**
 * @brief 优先级继承解除函数
 * 
 * 互斥量释放时恢复持有者的原始优先级，
 * 需处理嵌套调用场景。
 * 
 * @param pxMutexHolder 互斥量持有任务句柄
 * @return BaseType_t 是否实际修改了优先级
 */
BaseType_t xTaskPriorityDisinherit( TaskHandle_t const pxMutexHolder ) PRIVILEGED_FUNCTION;

/**
 * @brief 任务编号获取
 * 
 * @param xTask 目标任务句柄
 * @return UBaseType_t 唯一任务编号（调试用）
 */
UBaseType_t uxTaskGetTaskNumber( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/**
 * @brief 任务编号设置
 * 
 * @param xTask 目标任务句柄
 * @param uxHandle 自定义编号值
 */
void vTaskSetTaskNumber( TaskHandle_t xTask, const UBaseType_t uxHandle ) PRIVILEGED_FUNCTION;

/**
 * @brief 节拍计数器补偿函数
 * 
 * 在低功耗休眠后补偿跳过的节拍数，
 * 仅在启用configUSE_TICKLESS_IDLE时有效。
 * 
 * @param xTicksToJump 需要补偿的节拍数
 */
void vTaskStepTick( const TickType_t xTicksToJump ) PRIVILEGED_FUNCTION;

/*
 * Only avilable when configUSE_TICKLESS_IDLE is set to 1.
 * Provided for use within portSUPPRESS_TICKS_AND_SLEEP() to allow the port
 * specific sleep function to determine if it is ok to proceed with the sleep,
 * and if it is ok to proceed, if it is ok to sleep indefinitely.
 *
 * This function is necessary because portSUPPRESS_TICKS_AND_SLEEP() is only
 * called with the scheduler suspended, not from within a critical section.  It
 * is therefore possible for an interrupt to request a context switch between
 * portSUPPRESS_TICKS_AND_SLEEP() and the low power mode actually being
 * entered.  eTaskConfirmSleepModeStatus() should be called from a short
 * critical section between the timer being stopped and the sleep mode being
 * entered to ensure it is ok to proceed into the sleep mode.
 */
/**
 * @brief 低功耗模式进入确认函数
 * 
 * 在Tickless空闲模式中验证系统是否可安全进入睡眠，
 * 需在关闭系统节拍定时器后、进入低功耗前调用。
 * 
 * @return eSleepModeStatus 睡眠模式状态：
 *         - eNoTasksWaitingTimeout: 无待处理事件，可进入深度睡眠
 *         - eAbortSleep: 有高优先级任务就绪，禁止进入睡眠
 *         - eStandardSleep: 需维持节拍定时器运行（浅度睡眠）
 * 
 * @warning 调用前提：
 * - 必须在临界区内调用（中断禁用）
 * - 仅当configUSE_TICKLESS_IDLE=1时有效
 * 
 * @code
 * // Tickless模式典型调用序列
 * portSUPPRESS_TICKS_AND_SLEEP(xExpectedIdleTime) {
 *     eSleepModeStatus eSleepStatus = eTaskConfirmSleepModeStatus();
 *     if(eSleepStatus == eNoTasksWaitingTimeout) {
 *         __WFI();  // 进入深度睡眠
 *     }
 * }
 * @endcode
 */
eSleepModeStatus eTaskConfirmSleepModeStatus( void ) PRIVILEGED_FUNCTION;

/**
 * @brief 互斥量持有计数递增函数
 * 
 * 内部用于管理互斥量的嵌套获取场景，
 * 递增持有计数并返回当前持有者句柄。
 * 
 * @return void* 互斥量当前持有任务的TCB指针
 * 
 * @note 关键作用：
 * 1. 跟踪互斥量嵌套获取次数
 * 2. 支持优先级继承机制的正确执行
 * 3. 防止重复优先级提升
 * 
 * @warning 严格禁止应用层直接调用，
 * 仅由xQueueTakeMutexRecursive等内部函数使用
 */
void *pvTaskIncrementMutexHeldCount( void ) PRIVILEGED_FUNCTION;

#ifdef __cplusplus
}
#endif
#endif /* INC_TASK_H */




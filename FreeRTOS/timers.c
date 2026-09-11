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

/* Standard includes. */
#include <stdlib.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* 这个文件片段定义了FreeRTOS软件定时器的数据结构和相关配置检查。
   软件定时器允许在FreeRTOS应用程序中创建和管理定时器，这些定时器可以用于定期执行特定的回调函数。 */

#if ( INCLUDE_xTimerPendFunctionCall == 1 ) && ( configUSE_TIMERS == 0 )
	#error configUSE_TIMERS必须设置为1以使xTimerPendFunctionCall()函数可用。
#endif
/* 检查配置文件FreeRTOSConfig.h中的配置项，如果INCLUDE_xTimerPendFunctionCall被启用（设置为1），而configUSE_TIMERS未启用（设置为0），则会触发编译错误，提示用户必须启用configUSE_TIMERS以使用xTimerPendFunctionCall函数。 */

/* Lint e961和e750被抑制作为MISRA异常，因为MPU端口需要MPU_WRAPPERS_INCLUDED_FROM_API_FILE在头文件中被定义，但在此文件中不需要，以生成正确的特权与非特权链接和放置。 */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE /*lint !e961 !e750. */
/* 取消定义MPU_WRAPPERS_INCLUDED_FROM_API_FILE，以确保在编译时不会错误地应用MPU相关的包装器。 */

/* 如果应用程序未配置为包含软件定时器功能，则整个源文件将被跳过。这个#if在文件底部以#endif关闭。如果要包含软件定时器功能，请确保在FreeRTOSConfig.h中将configUSE_TIMERS设置为1。 */
#if ( configUSE_TIMERS == 1 )
/* 检查是否启用了软件定时器功能（configUSE_TIMERS设置为1），如果启用，则定义定时器相关的数据结构和功能。 */

/* 杂项定义 */
#define tmrNO_DELAY		( TickType_t ) 0U
/* 定义一个常量tmrNO_DELAY，表示定时器没有延迟，即立即开始计时。 */

/* 定义定时器本身的结构 */
typedef struct tmrTimerControl
{
	const char				*pcTimerName;		/*<< 文本名称。此名称不被内核使用，仅用于调试。 */ /*lint !e971 未限定的字符类型仅用于字符串和单个字符。 */
	/* 定义定时器名称的指针，用于调试信息。 */
	ListItem_t				xTimerListItem;		/*<< 标准链表项，用于所有内核功能的事件管理。 */
	/* 定义定时器的链表项，用于在链表中存储和管理定时器。 */
	TickType_t				xTimerPeriodInTicks;/*<< 定时器的过期频率和时间（以操作系统滴答为单位）。 */
	/* 定义定时器的周期，即定时器每隔多少个滴答会过期。 */
	UBaseType_t				uxAutoReload;		/*<< 设置为pdTRUE如果定时器过期后应自动重新加载并继续计时。设置为pdFALSE如果定时器是一次性的。 */
	/* 定义定时器是否自动重载，自动重载的定时器会在每次过期后重新开始计时。 */
	void 					*pvTimerID;			/*<< 一个标识符用于识别定时器。在多个定时器使用相同回调函数时，此ID可以帮助区分不同的定时器。 */
	/* 定义定时器的ID，用于在回调函数中识别具体的定时器实例。 */
	TimerCallbackFunction_t	pxCallbackFunction;	/*<< 当定时器过期时将被调用的函数。 */
	/* 定义定时器过期时调用的回调函数指针。 */
	#if( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t			uxTimerNumber;		/*<< 由跟踪工具如FreeRTOS+Trace分配的ID */
	#endif
	/* 如果启用了跟踪功能（configUSE_TRACE_FACILITY设置为1），则定义一个用于跟踪的定时器编号。 */

	#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		uint8_t 			ucStaticallyAllocated; /*<< 设置为pdTRUE如果定时器是静态分配的，因此在删除定时器时不会尝试释放内存。 */
	#endif
	/* 如果同时支持静态和动态内存分配（configSUPPORT_STATIC_ALLOCATION和configSUPPORT_DYNAMIC_ALLOCATION都设置为1），则定义一个标志位，用于指示定时器是静态分配的。 */
} xTIMER;

/* 为了保持与旧版本的兼容性，旧的xTIMER名称被保留，并新定义了一个Timer_t类型作为其别名。这样可以使得使用旧版本调试工具的应用程序仍然可以正常工作。 */
typedef xTIMER Timer_t;

/* 定义可以发送到定时器队列的消息结构。有两种消息类型：操作软件定时器的消息，请求执行非定时器相关回调的消息。每种消息类型都定义在不同的结构中，分别是tmrTimerParameters和tmrCallbackParameters。 */
typedef struct tmrTimerParameters
{
	TickType_t			xMessageValue;		/*<< 由某些命令使用的可选值，例如更改定时器的周期。 */
	/* 定义一个额外的消息值，用于特定的操作命令，如更改定时器周期。 */
	Timer_t *			pxTimer;			/*<< 将被应用命令的定时器。 */
	/* 定义一个定时器指针，指向将被操作的定时器实例。 */
} TimerParameter_t;

typedef struct tmrCallbackParameters
{
	PendedFunction_t	pxCallbackFunction;	/* << 将被执行的回调函数。 */
	/* 定义一个回调函数指针，指向待执行的回调函数。 */
	void *pvParameter1;						/* << 作为回调函数第一个参数使用的值。 */
	/* 定义回调函数的第一个参数。 */
	uint32_t ulParameter2;					/* << 作为回调函数第二个参数使用的值。 */
	/* 定义回调函数的第二个参数。 */
} CallbackParameters_t;

/* 定义包含两种不同类型消息的结构，以及一个标识符用于确定哪种消息类型有效。 */
typedef struct tmrTimerQueueMessage
{
	BaseType_t			xMessageID;			/*<< 发送到定时器服务任务的命令。 */
	/* 定义消息ID，用于标识具体的命令类型。 */
	union
	{
		TimerParameter_t xTimerParameters;

		/* 如果不需要使用xCallbackParameters，则不要包含它，因为这将使结构（以及因此的定时器队列）变大。 */
		#if ( INCLUDE_xTimerPendFunctionCall == 1 )
			CallbackParameters_t xCallbackParameters;
		#endif /* INCLUDE_xTimerPendFunctionCall */
	} u;
	/* 定义一个联合体，用于存储不同类型的消息参数。根据xMessageID的不同，联合体中存储的消息参数也不同。 */
} DaemonTaskMessage_t;

/* lint -e956 手动分析和检查已用于确定哪些静态变量必须被声明为volatile。 */
/* 声明静态变量为volatile，以确保编译器不会对这些变量进行优化，从而保证定时器服务任务能够正确地访问这些变量。 */

/* 存储活动定时器的列表。定时器按到期时间顺序引用，最近的到期时间位于列表的前面。只有定时器服务任务被允许访问这些列表。 */
PRIVILEGED_DATA static List_t xActiveTimerList1;
/* 定义一个列表结构xActiveTimerList1，用于存储活动定时器，按到期时间排序。 */
PRIVILEGED_DATA static List_t xActiveTimerList2;
/* 定义另一个列表结构xActiveTimerList2，用于存储活动定时器，按到期时间排序。 */
PRIVILEGED_DATA static List_t *pxCurrentTimerList;
/* 定义一个指向当前活动定时器列表的指针pxCurrentTimerList。 */
PRIVILEGED_DATA static List_t *pxOverflowTimerList;
/* 定义一个指向溢出定时器列表的指针pxOverflowTimerList。 */

/* 用于向定时器服务任务发送命令的队列。 */
PRIVILEGED_DATA static QueueHandle_t xTimerQueue = NULL;
/* 定义一个队列句柄xTimerQueue，用于存储发送给定时器服务任务的消息。 */
PRIVILEGED_DATA static TaskHandle_t xTimerTaskHandle = NULL;
/* 定义一个任务句柄xTimerTaskHandle，用于指向定时器服务任务。 */

/*lint +e956 */

/*-----------------------------------------------------------*/
/* 本函数用于创建并启动FreeRTOS的定时器服务任务，该任务负责处理所有软件定时器的调度和管理。
   函数执行分为以下步骤：
   1. 初始化定时器基础设施（链表、队列）
   2. 根据编译配置选择静态或动态内存分配方式
   3. 创建定时器守护任务
   4. 返回任务创建状态 */

#if( configSUPPORT_STATIC_ALLOCATION == 1 )

    /* 当启用静态内存分配时：
       应用程序必须实现vApplicationGetTimerTaskMemory回调，用于获取静态分配的：
       - TCB（任务控制块）缓冲区指针
       - 任务堆栈缓冲区指针
       - 堆栈大小参数 */
    extern void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, 
                                                StackType_t **ppxTimerTaskStackBuffer, 
                                                uint32_t *pulTimerTaskStackSize );
    /* 函数参数说明：
       ppxTimerTaskTCBBuffer:  输出参数，接收静态分配的TCB内存指针
       ppxTimerTaskStackBuffer: 输出参数，接收静态分配的堆栈内存指针
       pulTimerTaskStackSize:  输出参数，接收堆栈大小数值 */

#endif

/*-----------------------------------------------------------*/
/* 定时器服务核心函数声明（均运行在特权模式） */

/* 初始化检查函数：确保定时器链表和命令队列有效 */
static void prvCheckForValidListAndQueue( void ) PRIVILEGED_FUNCTION;
/* 功能：验证定时器服务任务依赖的基础设施：
   - 初始化xActiveTimerList1/xActiveTimerList2定时器链表
   - 创建xTimerQueue消息队列 */

/* 定时器守护任务主函数 */
static void prvTimerTask( void *pvParameters ) PRIVILEGED_FUNCTION;
/* 功能：定时器服务的核心循环：
   - 处理队列中的定时器命令
   - 监控定时器到期事件
   - 执行定时器回调函数
   参数：pvParameters 保留参数（通常为NULL） */

/* 命令处理函数 */
static void prvProcessReceivedCommands( void ) PRIVILEGED_FUNCTION;
/* 功能：从xTimerQueue队列中提取并执行命令：
   - 处理定时器创建/启动/停止等操作
   - 执行非定时器类的延迟回调 */

/* 定时器列表管理函数 */
static BaseType_t prvInsertTimerInActiveList( Timer_t * const pxTimer, 
                                             const TickType_t xNextExpiryTime,
                                             const TickType_t xTimeNow,
                                             const TickType_t xCommandTime ) PRIVILEGED_FUNCTION;
/* 功能：将定时器插入活动列表：
   - 处理定时器计数器溢出情况
   - 根据xNextExpiryTime选择插入xActiveTimerList1或xActiveTimerList2
   - 返回操作状态（pdPASS/pdFAIL） */

/* 到期定时器处理 */
static void prvProcessExpiredTimer( const TickType_t xNextExpireTime,
                                   const TickType_t xTimeNow ) PRIVILEGED_FUNCTION;
/* 功能：处理到期的定时器：
   - 如果是周期定时器则重装载定时器
   - 执行定时器回调函数
   - 更新定时器链表 */

/* 定时器列表切换 */
static void prvSwitchTimerLists( void ) PRIVILEGED_FUNCTION;
/* 功能：处理系统滴答计数器溢出时的列表切换：
   - 交换pxCurrentTimerList和pxOverflowTimerList指针
   - 确保当前列表空时切换列表 */

/* 滴答计数采样 */
static TickType_t prvSampleTimeNow( BaseType_t * const pxTimerListsWereSwitched ) PRIVILEGED_FUNCTION;
/* 功能：获取当前时间戳并检测溢出：
   - 采样xTaskGetTickCount()
   - 通过前后两次采样的差值检测溢出
   - 设置pxTimerListsWereSwitched标志 */

/* 获取下一个到期时间 */
static TickType_t prvGetNextExpireTime( BaseType_t * const pxListWasEmpty ) PRIVILEGED_FUNCTION;
/* 功能：从当前活动列表中获取最早到期时间：
   - 当列表非空时返回第一个节点的到期时间
   - 当列表为空时返回0并设置pxListWasEmpty标志 */

/* 定时器阻塞管理 */
static void prvProcessTimerOrBlockTask( const TickType_t xNextExpireTime,
                                       BaseType_t xListWasEmpty ) PRIVILEGED_FUNCTION;
/* 功能：决策任务阻塞行为：
   - 当有定时器即将到期时阻塞到最近到期时间
   - 当列表为空时无限阻塞等待命令 */

/* 定时器初始化函数 */
static void prvInitialiseNewTimer( const char * const pcTimerName,
                                  const TickType_t xTimerPeriodInTicks,
                                  const UBaseType_t uxAutoReload,
                                  void * const pvTimerID,
                                  TimerCallbackFunction_t pxCallbackFunction,
                                  Timer_t *pxNewTimer ) PRIVILEGED_FUNCTION;
/* 功能：完全初始化Timer_t结构：
   - 设置定时器名称
   - 配置定时周期和重载模式
   - 绑定回调函数
   - 初始化链表项 */

/*-----------------------------------------------------------*/
/* 定时器服务任务创建函数（全局可见）*/
BaseType_t xTimerCreateTimerTask( void )
{
    BaseType_t xReturn = pdFAIL;  // 初始化返回值为失败状态

    /* 当调度器启动时调用（configUSE_TIMERS=1时）：
       验证并初始化定时器服务所需的基础设施 */
    prvCheckForValidListAndQueue();  // 初始化活动列表和命令队列

    /* 检查队列是否成功创建 */
    if( xTimerQueue != NULL )
    {
        #if( configSUPPORT_STATIC_ALLOCATION == 1 )  // 静态分配模式
        {
            StaticTask_t *pxTimerTaskTCBBuffer = NULL;     // TCB缓冲区指针
            StackType_t *pxTimerTaskStackBuffer = NULL;    // 堆栈缓冲区指针
            uint32_t ulTimerTaskStackSize;                 // 堆栈尺寸变量

            /* 获取用户提供的静态内存 */
            vApplicationGetTimerTaskMemory( &pxTimerTaskTCBBuffer,
                                          &pxTimerTaskStackBuffer,
                                          &ulTimerTaskStackSize );

            /* 创建静态定时器服务任务 */
            xTimerTaskHandle = xTaskCreateStatic(
                prvTimerTask,                          // 任务入口函数
                "Tmr Svc",                             // 任务名称（最大长度由configMAX_TASK_NAME_LEN定义）
                ulTimerTaskStackSize,                  // 用户定义的堆栈大小
                NULL,                                  // 参数指针（未使用）
                ( ( UBaseType_t ) configTIMER_TASK_PRIORITY ) | portPRIVILEGE_BIT,  // 优先级和特权标记
                pxTimerTaskStackBuffer,                // 静态分配的堆栈内存
                pxTimerTaskTCBBuffer );                // 静态分配的TCB内存

            /* 校验任务句柄有效性 */
            if( xTimerTaskHandle != NULL )
            {
                xReturn = pdPASS;  // 任务创建成功
            }
        }
        #else  // 动态分配模式
        {
            /* 创建动态定时器服务任务 */
            xReturn = xTaskCreate(
                prvTimerTask,                          // 任务入口函数
                "Tmr Svc",                             // 任务名称
                configTIMER_TASK_STACK_DEPTH,         // 使用默认堆栈深度
                NULL,                                  // 参数指针
                ( ( UBaseType_t ) configTIMER_TASK_PRIORITY ) | portPRIVILEGE_BIT,  // 特权优先级
                &xTimerTaskHandle );                   // 输出任务句柄
        }
        #endif /* configSUPPORT_STATIC_ALLOCATION */
    }
    else
    {
        /* 代码覆盖测试标记（当队列创建失败时执行空分支） */
        mtCOVERAGE_TEST_MARKER();  // 用于覆盖测试工具识别代码分支
    }

    /* 调试断言（仅当configASSERT被定义时生效） */
    configASSERT( xReturn );  // 验证返回值有效性（应为pdPASS或pdFAIL）

    return xReturn;  // 返回任务创建状态
}
/*-----------------------------------------------------------*/
/* 定时器创建功能实现区 */

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )  // 动态内存分配模式开关

    /* 动态创建定时器对象函数
       参数说明：
       pcTimerName:         定时器名称（调试用）
       xTimerPeriodInTicks: 定时周期（系统滴答数）
       uxAutoReload:        自动重载模式 (pdTRUE=周期模式, pdFALSE=单次模式)
       pvTimerID:           用户自定义标识符
       pxCallbackFunction:  到期回调函数指针
       返回值：TimerHandle_t 定时器句柄 */
    TimerHandle_t xTimerCreate( const char * const pcTimerName,
                                const TickType_t xTimerPeriodInTicks,
                                const UBaseType_t uxAutoReload,
                                void * const pvTimerID,
                                TimerCallbackFunction_t pxCallbackFunction )
    {
    Timer_t *pxNewTimer;  // 新建定时器结构指针

        /* 从系统堆分配定时器结构内存 */
        pxNewTimer = ( Timer_t * ) pvPortMalloc( sizeof( Timer_t ) );

        if( pxNewTimer != NULL )  // 内存分配成功检查
        {
            /* 调用统一初始化函数 */
            prvInitialiseNewTimer( pcTimerName, xTimerPeriodInTicks, uxAutoReload, 
                                  pvTimerID, pxCallbackFunction, pxNewTimer );

            #if( configSUPPORT_STATIC_ALLOCATION == 1 )  // 混合分配模式支持
            {
                /* 设置内存分配标识：动态分配标志（用于后续删除时判断是否释放内存） */
                pxNewTimer->ucStaticallyAllocated = pdFALSE;
            }
            #endif /* configSUPPORT_STATIC_ALLOCATION */
        }

        return pxNewTimer;  // 返回新定时器对象（可能为NULL）
    }

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )  // 静态内存分配模式开关

    /* 静态创建定时器对象函数
       新增参数：
       pxTimerBuffer:       静态内存缓冲区指针
       其他参数同动态创建函数 */
    TimerHandle_t xTimerCreateStatic( const char * const pcTimerName,
                                      const TickType_t xTimerPeriodInTicks,
                                      const UBaseType_t uxAutoReload,
                                      void * const pvTimerID,
                                      TimerCallbackFunction_t pxCallbackFunction,
                                      StaticTimer_t *pxTimerBuffer )
    {
    Timer_t *pxNewTimer;

        #if( configASSERT_DEFINED == 1 )  // 调试断言检查
        {
            /* 验证静态缓冲区与Timer_t结构体尺寸一致性（防止内存溢出） */
            volatile size_t xSize = sizeof( StaticTimer_t );
            configASSERT( xSize == sizeof( Timer_t ) );  // 必须触发尺寸匹配断言
        }
        #endif /* configASSERT_DEFINED */

        /* 强制校验缓冲区有效性 */
        configASSERT( pxTimerBuffer );  // 禁止传入NULL指针
        
        /* 类型转换静态内存缓冲区为定时器对象 */
        pxNewTimer = ( Timer_t * ) pxTimerBuffer;  // 通过内存布局转换

        if( pxNewTimer != NULL )
        {
            /* 调用统一初始化函数 */
            prvInitialiseNewTimer( pcTimerName, xTimerPeriodInTicks, uxAutoReload,
                                  pvTimerID, pxCallbackFunction, pxNewTimer );

            #if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )  // 混合分配模式支持
            {
                /* 设置内存分配标识：静态分配标志（防止误释放静态内存） */
                pxNewTimer->ucStaticallyAllocated = pdTRUE;
            }
            #endif /* configSUPPORT_DYNAMIC_ALLOCATION */
        }

        return pxNewTimer;  // 返回已初始化对象
    }

#endif /* configSUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/

/* 定时器结构统一初始化函数（被创建函数调用）
   关键操作：
   - 参数合法性校验
   - 核心数据结构初始化
   - 基础设施有效性验证 */
static void prvInitialiseNewTimer( const char * const pcTimerName,
                                  const TickType_t xTimerPeriodInTicks,
                                  const UBaseType_t uxAutoReload,
                                  void * const pvTimerID,
                                  TimerCallbackFunction_t pxCallbackFunction,
                                  Timer_t *pxNewTimer )
{
    /* 参数校验：定时周期必须大于0（避免除零错误） */
    configASSERT( ( xTimerPeriodInTicks > 0 ) );

    if( pxNewTimer != NULL )  // 空指针防护
    {
        /* 确保定时器基础设施就绪：
           - 初始化活动定时器链表(xActiveTimerList1/xActiveTimerList2)
           - 创建定时器命令队列(xTimerQueue) */
        prvCheckForValidListAndQueue();

        /* 定时器结构体成员初始化 */
        pxNewTimer->pcTimerName = pcTimerName;             // 设置调试名称
        pxNewTimer->xTimerPeriodInTicks = xTimerPeriodInTicks; // 存储定时周期
        pxNewTimer->uxAutoReload = uxAutoReload;           // 自动重载配置
        pxNewTimer->pvTimerID = pvTimerID;                 // 用户自定义标识
        pxNewTimer->pxCallbackFunction = pxCallbackFunction; // 绑定回调函数

        /* 初始化链表项（用于加入定时器链表） */
        vListInitialiseItem( &( pxNewTimer->xTimerListItem ) );

        /* 触发跟踪宏（用于调试工具捕获定时器创建事件） */
        traceTIMER_CREATE( pxNewTimer );
    }
}
/*-----------------------------------------------------------*/
/* 定时器控制命令统一处理接口
   功能：向定时器守护任务发送控制命令（启动/停止/重置等）
   参数：
   xTimer: 目标定时器句柄
   xCommandID: 命令标识符（tmrCOMMAND_xxx系列）
   xOptionalValue: 命令附加参数（如新周期值）
   pxHigherPriorityTaskWoken: ISR上下文唤醒标志指针
   xTicksToWait: 队列满时的最大等待时间
   返回值：命令发送状态（pdPASS成功/pdFAIL失败） */
BaseType_t xTimerGenericCommand( TimerHandle_t xTimer,
                                const BaseType_t xCommandID,
                                const TickType_t xOptionalValue,
                                BaseType_t * const pxHigherPriorityTaskWoken,
                                const TickType_t xTicksToWait )
{
BaseType_t xReturn = pdFAIL;           // 默认返回值
DaemonTaskMessage_t xMessage;          // 命令消息结构体

    configASSERT( xTimer );  // 严格校验定时器句柄有效性

    /* 通过定时器命令队列发送操作指令 */
    if( xTimerQueue != NULL )  // 队列创建验证
    {
        /* 构建标准命令消息结构 */
        xMessage.xMessageID = xCommandID;                        // 命令标识符
        xMessage.u.xTimerParameters.xMessageValue = xOptionalValue; // 附加参数值
        xMessage.u.xTimerParameters.pxTimer = ( Timer_t * ) xTimer; // 转换句柄为结构指针

        /* 命令发送模式选择 */
        if( xCommandID < tmrFIRST_FROM_ISR_COMMAND )  // 普通任务上下文命令
        {
            if( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )  // 调度器已启动
            {
                /* 标准队列发送（带超时机制） */
                xReturn = xQueueSendToBack( xTimerQueue, &xMessage, xTicksToWait );
            }
            else  // 调度器未启动的特殊处理
            {
                /* 无阻塞立即发送（启动阶段专用） */
                xReturn = xQueueSendToBack( xTimerQueue, &xMessage, tmrNO_DELAY );
            }
        }
        else  // 中断上下文命令（tmrCOMMAND_xxx_FROM_ISR系列）
        {
            /* 中断安全队列操作 */
            xReturn = xQueueSendToBackFromISR( xTimerQueue, &xMessage, pxHigherPriorityTaskWoken );
        }

        /* 触发跟踪事件（记录命令发送细节） */
        traceTIMER_COMMAND_SEND( xTimer, xCommandID, xOptionalValue, xReturn );
    }
    else  // 队列未初始化分支
    {
        mtCOVERAGE_TEST_MARKER();  // 覆盖率测试标记
    }

    return xReturn;  // 返回命令投递状态
}
/*-----------------------------------------------------------*/

/* 获取定时器守护任务句柄
   注意：必须在调度器启动后调用（否则触发断言） */
TaskHandle_t xTimerGetTimerDaemonTaskHandle( void )
{
    /* 严格校验定时器任务已创建 */
    configASSERT( ( xTimerTaskHandle != NULL ) );  // DEBUG模式下的空指针保护
    return xTimerTaskHandle;  // 返回守护任务TCB指针
}
/*-----------------------------------------------------------*/

/* 获取定时器周期值（单位：系统滴答） */
TickType_t xTimerGetPeriod( TimerHandle_t xTimer )
{
Timer_t *pxTimer = ( Timer_t * ) xTimer;  // 类型转换句柄

    configASSERT( xTimer );  // 空指针保护
    return pxTimer->xTimerPeriodInTicks;  // 返回预存的周期值
}
/*-----------------------------------------------------------*/

/* 获取定时器下一次到期时刻（绝对滴答值） */
TickType_t xTimerGetExpiryTime( TimerHandle_t xTimer )
{
Timer_t * pxTimer = ( Timer_t * ) xTimer;  // 类型转换句柄
TickType_t xReturn;  // 返回值

    configASSERT( xTimer );  // 空指针保护
    /* 从定时器链表项中提取存储的到期时间 */
    xReturn = listGET_LIST_ITEM_VALUE( &( pxTimer->xTimerListItem ) );
    return xReturn;  // 返回计算的绝对时间戳
}
/*-----------------------------------------------------------*/

/* 获取定时器名称（调试用途） */
const char * pcTimerGetName( TimerHandle_t xTimer )
{
Timer_t *pxTimer = ( Timer_t * ) xTimer;  // 类型转换句柄

    configASSERT( xTimer );  // 空指针保护
    return pxTimer->pcTimerName;  // 返回存储的名称字符串
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 定时器到期处理函数
   功能：处理已到达过期时间的定时器，执行回调并处理重载逻辑
   参数：
   xNextExpireTime: 当前处理的到期时间阈值（绝对滴答值）
   xTimeNow: 当前系统时间（绝对滴答值）*/
static void prvProcessExpiredTimer( const TickType_t xNextExpireTime,
                                   const TickType_t xTimeNow )
{
BaseType_t xResult;  // 操作结果临时变量
Timer_t * const pxTimer = ( Timer_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxCurrentTimerList ); // 获取当前列表首个定时器

    /* 从活动定时器列表移除此定时器（列表非空已通过前期检查） */
    ( void ) uxListRemove( &( pxTimer->xTimerListItem ) );  // 移除链表项
    traceTIMER_EXPIRED( pxTimer );  // 触发跟踪事件

    /* 处理自动重载型定时器 */
    if( pxTimer->uxAutoReload == ( UBaseType_t ) pdTRUE )  // 自动重载模式检查
    {
        /* 计算下次触发时间并尝试重新插入活动列表
           xNextExpireTime已包含当前处理周期的偏移 */
        if( prvInsertTimerInActiveList( pxTimer, 
                                      ( xNextExpireTime + pxTimer->xTimerPeriodInTicks ), // 新到期时间
                                      xTimeNow, 
                                      xNextExpireTime ) != pdFALSE )
        {
            /* 插入失败的特殊处理（定时器在插入前已过期）：
               强制通过命令通道重启定时器 */
            xResult = xTimerGenericCommand( pxTimer, 
                                           tmrCOMMAND_START_DONT_TRACE,  // 无跟踪启动命令
                                           xNextExpireTime,  // 起始时间
                                           NULL, 
                                           tmrNO_DELAY );  // 无延时发送
            configASSERT( xResult );  // 必须发送成功
            ( void ) xResult;  // 消除编译器警告
        }
        else  // 正常插入情况
        {
            mtCOVERAGE_TEST_MARKER();  // 覆盖测试标记
        }
    }
    else  // 单次定时器不处理重载
    {
        mtCOVERAGE_TEST_MARKER();  // 覆盖测试标记
    }

    /* 执行定时器回调函数（传入定时器句柄作为参数） */
    pxTimer->pxCallbackFunction( ( TimerHandle_t ) pxTimer );  // 同步执行回调
}
/*-----------------------------------------------------------*/

/* 定时器守护任务主函数
   功能：作为定时器服务的核心循环，负责：
   - 监控定时器到期事件
   - 处理定时器命令队列
   - 任务调度管理 */
static void prvTimerTask( void *pvParameters )
{
TickType_t xNextExpireTime;  // 下次到期时间缓存
BaseType_t xListWasEmpty;    // 列表状态标识

    /* 消除编译器未使用参数警告 */
    ( void ) pvParameters;

    #if( configUSE_DAEMON_TASK_STARTUP_HOOK == 1 )  // 启动钩子开关
    {
        /* 声明外部启动钩子函数（由用户实现） */
        extern void vApplicationDaemonTaskStartupHook( void );

        /* 执行用户自定义初始化（此时调度器已启动） 
           典型用途：
           - 硬件外设初始化
           - 系统状态监控初始化 */
        vApplicationDaemonTaskStartupHook();
    }
    #endif /* configUSE_DAEMON_TASK_STARTUP_HOOK */

    /* 主任务循环 */
    for( ;; )
    {
        /* 获取下一个到期时间并检测列表状态 */
        xNextExpireTime = prvGetNextExpireTime( &xListWasEmpty );  // 更新最近到期时间

        /* 决策执行流程：
           - 有定时器到期：处理到期事件
           - 无定时器：阻塞任务直到事件发生 */
        prvProcessTimerOrBlockTask( xNextExpireTime, xListWasEmpty );

        /* 处理所有已接收的命令（队列清空操作）
           包括：
           - 新定时器启动
           - 运行中定时器停止/重置
           - 回调函数延迟执行请求 */
        prvProcessReceivedCommands();  // 同步处理命令
    }
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 定时器处理与任务阻塞决策函数
   功能：根据定时器状态决定处理到期事件或进入阻塞
   参数：
   xNextExpireTime: 预计算的最近到期时间
   xListWasEmpty: 当前定时器列表是否为空 */
static void prvProcessTimerOrBlockTask( const TickType_t xNextExpireTime,
                                       BaseType_t xListWasEmpty )
{
TickType_t xTimeNow;          // 当前系统滴答值
BaseType_t xTimerListsWereSwitched; // 定时器列表切换标志

    /* 进入临界区（挂起调度器） */
    vTaskSuspendAll();
    {
        /* 安全获取当前时间并检测列表切换事件 */
        xTimeNow = prvSampleTimeNow( &xTimerListsWereSwitched );

        /* 仅在列表未切换时处理 */
        if( xTimerListsWereSwitched == pdFALSE )
        {
            /* 非空列表且定时器到期 */
            if( ( xListWasEmpty == pdFALSE ) && ( xNextExpireTime <= xTimeNow ) )
            {
                ( void ) xTaskResumeAll();  // 退出临界区
                prvProcessExpiredTimer( xNextExpireTime, xTimeNow ); // 处理到期
            }
            else  // 未到期或列表空
            {
                /* 当列表空时检测溢出列表状态 */
                if( xListWasEmpty != pdFALSE )
                {
                    /* 检查溢出列表是否也为空 */
                    xListWasEmpty = listLIST_IS_EMPTY( pxOverflowTimerList );
                }

                /* 受限等待队列消息（带超时机制） */
                vQueueWaitForMessageRestricted( 
                    xTimerQueue,                    // 目标队列
                    ( xNextExpireTime - xTimeNow ), // 最大阻塞时长（相对时间）
                    xListWasEmpty                   // 列表空状态
                );

                /* 恢复调度器并检测是否触发上下文切换 */
                if( xTaskResumeAll() == pdFALSE )  // 返回pdFALSE表示需要切换
                {
                    /* 主动让出CPU（但不进入阻塞状态）
                       当有更高优先级任务就绪时切换 */
                    portYIELD_WITHIN_API();  // 触发任务调度
                }
                else  // 无需切换的情况
                {
                    mtCOVERAGE_TEST_MARKER();  // 覆盖率标记
                }
            }
        }
        else  // 时间列表已切换的情况
        {
            ( void ) xTaskResumeAll();  // 简单恢复调度器
        }
    }
}
/*-----------------------------------------------------------*/

/* 获取最近到期时间函数
   功能：从当前活动列表获取最近到期时间，同时返回列表空状态 */
static TickType_t prvGetNextExpireTime( BaseType_t * const pxListWasEmpty )
{
TickType_t xNextExpireTime;

    /* 检测当前活动列表是否为空 */
    *pxListWasEmpty = listLIST_IS_EMPTY( pxCurrentTimerList );

    /* 当列表非空时获取链表头节点的到期时间值 */
    if( *pxListWasEmpty == pdFALSE )
    {
        xNextExpireTime = listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxCurrentTimerList );
    }
    else  // 列表空时特殊处理
    {
        /* 设置为0将在滴答计数器溢出时解除阻塞 */
        xNextExpireTime = ( TickType_t ) 0U; 
    }

    return xNextExpireTime;  // 返回绝对到期时间戳
}
/*-----------------------------------------------------------*/

/* 时间采样与列表切换检测函数
   功能：安全获取当前时间，检测系统滴答溢出事件 */
static TickType_t prvSampleTimeNow( BaseType_t * const pxTimerListsWereSwitched )
{
TickType_t xTimeNow;
PRIVILEGED_DATA static TickType_t xLastTime = ( TickType_t ) 0U; // 静态保存上次时间

    /* 获取当前系统滴答值 */
    xTimeNow = xTaskGetTickCount();

    /* 检测滴答计数器溢出（当前时间小于上次记录时间） */
    if( xTimeNow < xLastTime )
    {
        prvSwitchTimerLists();              // 执行定时器列表切换
        *pxTimerListsWereSwitched = pdTRUE; // 设置切换标志
    }
    else  // 无溢出情况
    {
        *pxTimerListsWereSwitched = pdFALSE;
    }

    /* 更新静态时间戳 */
    xLastTime = xTimeNow;

    return xTimeNow;  // 返回当前有效时间
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 定时器列表插入函数
   功能：智能插入定时器到活动列表，返回是否需立即处理
   参数：
   pxTimer: 目标定时器
   xNextExpireTime: 新计算的到期时间
   xTimeNow: 当前系统时间
   xCommandTime: 命令接收时间 */
static BaseType_t prvInsertTimerInActiveList( Timer_t * const pxTimer,
                                             const TickType_t xNextExpireTime,
                                             const TickType_t xTimeNow,
                                             const TickType_t xCommandTime )
{
BaseType_t xProcessTimerNow = pdFALSE;  // 立即处理标志

    /* 初始化定时器链表项 */
    listSET_LIST_ITEM_VALUE( &( pxTimer->xTimerListItem ), xNextExpireTime ); // 设置到期时间
    listSET_LIST_ITEM_OWNER( &( pxTimer->xTimerListItem ), pxTimer );        // 绑定定时器实例

    /* 判断到期时间与当前时间关系 */
    if( xNextExpireTime <= xTimeNow )  // 已到期情况
    {
        /* 检测命令处理延迟是否超过定时周期 */
        if( ( ( TickType_t )( xTimeNow - xCommandTime ) ) >= pxTimer->xTimerPeriodInTicks )
        {
            /* 命令处理严重超时：立即触发回调 */
            xProcessTimerNow = pdTRUE;
        }
        else  // 正常到期流程
        {
            /* 插入溢出列表（应对滴答计数器即将溢出） */
            vListInsert( pxOverflowTimerList, &( pxTimer->xTimerListItem ) );
        }
    }
    else  // 未到期情况
    {
        /* 处理滴答计数器溢出场景 */
        if( ( xTimeNow < xCommandTime ) && ( xNextExpireTime >= xCommandTime ) )
        {
            /* 命令发出后发生溢出，但定时器应已到期 */
            xProcessTimerNow = pdTRUE;
        }
        else  // 正常未到期插入
        {
            /* 插入当前活动列表 */
            vListInsert( pxCurrentTimerList, &( pxTimer->xTimerListItem ) );
        }
    }

    return xProcessTimerNow;  // 返回处理决策
}
/*-----------------------------------------------------------*/

/* 定时器命令处理函数
   功能：处理从队列接收的所有定时器命令
   处理流程：
   1. 循环取出队列消息直至队列空
   2. 区分回调命令与定时器操作命令
   3. 执行定时器启停/周期修改/删除等操作 */
static void prvProcessReceivedCommands( void )
{
DaemonTaskMessage_t xMessage;          // 命令消息结构体
Timer_t *pxTimer;                     // 当前操作定时器指针
BaseType_t xTimerListsWereSwitched;  // 列表切换标志
TickType_t xTimeNow;                // 当前时间缓存
BaseType_t xResult;                // 操作结果

    /* 循环处理所有队列命令（无阻塞模式） */
    while( xQueueReceive( xTimerQueue, &xMessage, tmrNO_DELAY ) != pdFAIL )
    {
        #if ( INCLUDE_xTimerPendFunctionCall == 1 )  // 异步回调支持开关
        {
            /* 处理延迟回调命令（命令ID为负数） */
            if( xMessage.xMessageID < ( BaseType_t ) 0 )
            {
                const CallbackParameters_t * const pxCallback = &( xMessage.u.xCallbackParameters );

                configASSERT( pxCallback );  // 确保回调结构有效

                /* 执行延迟回调（参数1: void指针，参数2: 32位数据） */
                pxCallback->pxCallbackFunction( pxCallback->pvParameter1, pxCallback->ulParameter2 );
            }
            else  // 正常定时器命令处理分支
            {
                mtCOVERAGE_TEST_MARKER();  // 覆盖测试标记
            }
        }
        #endif /* INCLUDE_xTimerPendFunctionCall */

        /* 处理正数命令ID的定时器操作 */
        if( xMessage.xMessageID >= ( BaseType_t ) 0 )
        {
            pxTimer = xMessage.u.xTimerParameters.pxTimer;  // 获取目标定时器

            /* 安全移除可能存在的链表项 */
            if( listIS_CONTAINED_WITHIN( NULL, &( pxTimer->xTimerListItem ) ) == pdFALSE )
            {
                ( void ) uxListRemove( &( pxTimer->xTimerListItem ) ); // 从当前列表移除
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 覆盖测试标记
            }

            traceTIMER_COMMAND_RECEIVED( pxTimer, xMessage.xMessageID, xMessage.u.xTimerParameters.xMessageValue ); // 调试追踪

            /* 安全获取当前系统时间 */
            xTimeNow = prvSampleTimeNow( &xTimerListsWereSwitched );

            /* 命令分发处理 */
            switch( xMessage.xMessageID )
            {
                case tmrCOMMAND_START :
                case tmrCOMMAND_START_FROM_ISR :
                case tmrCOMMAND_RESET :
                case tmrCOMMAND_RESET_FROM_ISR :
                case tmrCOMMAND_START_DONT_TRACE :  // 启动类命令
                {
                    /* 计算新到期时间并尝试插入列表 */
                    TickType_t xNewExpiry = xMessage.u.xTimerParameters.xMessageValue + pxTimer->xTimerPeriodInTicks;
                    if( prvInsertTimerInActiveList( pxTimer, xNewExpiry, xTimeNow, xMessage.u.xTimerParameters.xMessageValue ) != pdFALSE )
                    {
                        /* 定时器应立即触发 */
                        pxTimer->pxCallbackFunction( ( TimerHandle_t ) pxTimer );  // 执行回调
                        traceTIMER_EXPIRED( pxTimer );  // 追踪事件

                        if( pxTimer->uxAutoReload == pdTRUE )  // 自动重载处理
                        {
                            /* 重启定时器（无调试追踪） */
                            xResult = xTimerGenericCommand( pxTimer, tmrCOMMAND_START_DONT_TRACE, 
                                                            xNewExpiry, NULL, tmrNO_DELAY );
                            configASSERT( xResult );  // 必须成功
                            ( void ) xResult;
                        }
                        else  // 单次定时器无需处理
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }
                    }
                    break;
                }

                case tmrCOMMAND_STOP :
                case tmrCOMMAND_STOP_FROM_ISR :  // 停止命令（已提前移除列表项）
                    /* 无需额外操作 */
                    break;

                case tmrCOMMAND_CHANGE_PERIOD :
                case tmrCOMMAND_CHANGE_PERIOD_FROM_ISR :  // 修改周期命令
                {
                    /* 更新定时周期 */
                    pxTimer->xTimerPeriodInTicks = xMessage.u.xTimerParameters.xMessageValue;
                    configASSERT( ( pxTimer->xTimerPeriodInTicks > 0 ) );  // 周期必须合法

                    /* 基于当前时间重新计算到期时间 */
                    ( void ) prvInsertTimerInActiveList( pxTimer, 
                                                        ( xTimeNow + pxTimer->xTimerPeriodInTicks ), 
                                                        xTimeNow, 
                                                        xTimeNow );
                    break;
                }

                case tmrCOMMAND_DELETE :  // 删除定时器命令
                    #if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 0 ) )
                    {
                        /* 动态分配内存直接释放 */
                        vPortFree( pxTimer ); 
                    }
                    #elif( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
                    {
                        /* 混合模式需检测分配类型 */
                        if( pxTimer->ucStaticallyAllocated == pdFALSE )
                        {
                            vPortFree( pxTimer );  // 仅释放动态内存
                        }
                        else  // 静态内存不需处理
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }
                    }
                    #endif
                    break;

                default :
                    /* 异常命令处理 */
                    break;
            }//end switch
        }//end if命令类型判断
    }//end while队列处理循环
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 定时器列表切换函数
  功能：处理系统滴答溢出事件，切换主/溢出定时器列表 */
static void prvSwitchTimerLists( void )
{
TickType_t xNextExpireTime, xReloadTime; // 当前到期时间和重载时间

List_t *pxTemp;                         // 临时列表指针
Timer_t *pxTimer;                       // 当前处理的定时器指针
BaseType_t xResult;                     // 操作结果


   /* 在滴答计数器溢出后，必须切换列表
      确保所有当前列表中的定时器都已处理 */
   while( listLIST_IS_EMPTY( pxCurrentTimerList ) == pdFALSE ) 
   {
       /* 获取当前列表中最早到期时间 */
       xNextExpireTime = listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxCurrentTimerList );

       /* 移出链表首项对应的定时器 */
       pxTimer = ( Timer_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxCurrentTimerList );
       ( void ) uxListRemove( &( pxTimer->xTimerListItem ) ); // 从链表移除
       traceTIMER_EXPIRED( pxTimer ); // 追踪过期事件

       /* 立即执行回调函数（在列表切换前处理）
          注：不能在此处直接重启定时器，必须通过命令队列 */
       pxTimer->pxCallbackFunction( ( TimerHandle_t ) pxTimer );

       /* 处理自动重载型定时器 */
       if( pxTimer->uxAutoReload == ( UBaseType_t ) pdTRUE )
       {
           /* 计算重载后的新到期时间 */
           xReloadTime = ( xNextExpireTime + pxTimer->xTimerPeriodInTicks );

           /* 时间未再次溢出时直接插入当前列表 */
           if( xReloadTime > xNextExpireTime )
           {
               /* 更新链表项参数并重新插入 */
               listSET_LIST_ITEM_VALUE( &( pxTimer->xTimerListItem ), xReloadTime );
               listSET_LIST_ITEM_OWNER( &( pxTimer->xTimerListItem ), pxTimer );
               vListInsert( pxCurrentTimerList, &( pxTimer->xTimerListItem ) );
           }
           else  // 时间再次溢出，必须延迟处理
           {
               /* 通过命令队列重启定时器（确保列表切换后的正确性） */
               xResult = xTimerGenericCommand( pxTimer, 
                                             tmrCOMMAND_START_DONT_TRACE, 
                                             xNextExpireTime, 
                                             NULL, 
                                             tmrNO_DELAY );
               configASSERT( xResult );  // 必须发送成功
               ( void ) xResult;
           }
       }
       else  // 单次定时器无需处理
       {
           mtCOVERAGE_TEST_MARKER(); // 覆盖测试标记
       }
   }//end while循环处理所有定时器


   /* 交换主列表与溢出列表的指针 */
   pxTemp = pxCurrentTimerList;
   pxCurrentTimerList = pxOverflowTimerList;
   pxOverflowTimerList = pxTemp;
}
/*-----------------------------------------------------------*/

/* 定时器基础设施校验函数
  功能：确保定时器服务所需的队列和链表已初始化 */
static void prvCheckForValidListAndQueue( void )
{
   /* 进入临界区保护初始化过程 */
   taskENTER_CRITICAL();
   {
       /* 首次调用时初始化基础结构 */
       if( xTimerQueue == NULL )
       {
           /* 初始化两个定时器活动链表 */
           vListInitialise( &xActiveTimerList1 ); // 链表1
           vListInitialise( &xActiveTimerList2 ); // 链表2
           pxCurrentTimerList = &xActiveTimerList1; // 初始当前列表

           pxOverflowTimerList = &xActiveTimerList2; // 初始溢出列表

           #if( configSUPPORT_STATIC_ALLOCATION == 1 )
           {
               /* 静态分配队列内存（用于不支持动态分配的环境） */
               static StaticQueue_t xStaticTimerQueue; // 静态队列控制块
               static uint8_t ucStaticTimerQueueStorage[ configTIMER_QUEUE_LENGTH * sizeof( DaemonTaskMessage_t ) ]; // 存储区


               /* 创建静态队列 */
               xTimerQueue = xQueueCreateStatic(
                   ( UBaseType_t ) configTIMER_QUEUE_LENGTH,  // 队列长度
                   sizeof( DaemonTaskMessage_t ),            // 消息大小
                   &ucStaticTimerQueueStorage[ 0 ],         // 存储区指针
                   &xStaticTimerQueue                      // 队列控制块指针
               );
           }
           #else  // 动态分配模式
           {
               /* 动态创建定时器命令队列 */
               xTimerQueue = xQueueCreate( 
                   ( UBaseType_t ) configTIMER_QUEUE_LENGTH, 
                   sizeof( DaemonTaskMessage_t ) 
               );
           }
           #endif /* configSUPPORT_STATIC_ALLOCATION */

           #if ( configQUEUE_REGISTRY_SIZE > 0 ) // 队列注册支持
           {
               if( xTimerQueue != NULL )
               {
                   /* 将队列注册为"TmrQ"便于调试工具识别 */
                   vQueueAddToRegistry( xTimerQueue, "TmrQ" );
               }
               else
               {
                   mtCOVERAGE_TEST_MARKER(); // 空队列不注册
               }
           }
           #endif /* configQUEUE_REGISTRY_SIZE */
       }
       else  // 已初始化的情况
       {
           mtCOVERAGE_TEST_MARKER(); // 覆盖测试标记
       }
   }
   taskEXIT_CRITICAL(); // 退出临界区
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 定时器状态检查函数
   功能：判断指定定时器是否处于活动状态（存在于任一活动列表） */
BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer )
{
BaseType_t xTimerIsInActiveList;  // 活动状态标识
Timer_t *pxTimer = ( Timer_t * ) xTimer;  // 类型转换定时器结构

    configASSERT( xTimer );  // 空指针校验

    /* 进入临界区保护链表状态检查 */
    taskENTER_CRITICAL();
    {
        /* 反向利用NULL列表检查机制：
           若链表项不属于NULL列表，说明存在于活动/溢出列表 */
        xTimerIsInActiveList = ( BaseType_t ) !( listIS_CONTAINED_WITHIN( NULL, &( pxTimer->xTimerListItem ) ) );
    }
    taskEXIT_CRITICAL();

    return xTimerIsInActiveList;  // 返回状态：pdTRUE活动中，pdFALSE未激活
} /*lint !e818 忽略const修饰符不匹配警告 */
/*-----------------------------------------------------------*/

/* 获取定时器用户标识函数
   功能：安全获取用户关联的定时器标识符 */
void *pvTimerGetTimerID( const TimerHandle_t xTimer )
{
Timer_t * const pxTimer = ( Timer_t * ) xTimer;  // 定时器结构指针
void *pvReturn;                                // 返回值

    configASSERT( xTimer );  // 参数有效性验证

    taskENTER_CRITICAL();    // 进入临界区
    {
        pvReturn = pxTimer->pvTimerID;  // 获取用户自定义ID
    }
    taskEXIT_CRITICAL();     // 退出临界区

    return pvReturn;  // 返回存储的指针
}
/*-----------------------------------------------------------*/

/* 设置定时器用户标识函数
   功能：原子操作更新用户自定义标识 */
void vTimerSetTimerID( TimerHandle_t xTimer, void *pvNewID )
{
Timer_t * const pxTimer = ( Timer_t * ) xTimer;  // 定时器结构指针

    configASSERT( xTimer );  // 参数有效性验证

    taskENTER_CRITICAL();    // 进入临界区
    {
        pxTimer->pvTimerID = pvNewID;  // 更新用户标识
    }
    taskEXIT_CRITICAL();     // 退出临界区
}
/*-----------------------------------------------------------*/

#if( INCLUDE_xTimerPendFunctionCall == 1 )  // 异步回调支持开关

    /* 中断上下文延迟回调请求函数
       功能：从中断安全发送回调请求到定时器服务任务
       参数：
       xFunctionToPend: 待执行函数指针
       pvParameter1: 参数1（void指针）
       ulParameter2: 参数2（32位数据）
       pxHigherPriorityTaskWoken: 任务唤醒标志指针 */
    BaseType_t xTimerPendFunctionCallFromISR( PendedFunction_t xFunctionToPend,
                                            void *pvParameter1,
                                            uint32_t ulParameter2,
                                            BaseType_t *pxHigherPriorityTaskWoken )
    {
    DaemonTaskMessage_t xMessage;  // 命令消息结构
    BaseType_t xReturn;           // 队列操作结果

        /* 构建回调参数消息体 */
        xMessage.xMessageID = tmrCOMMAND_EXECUTE_CALLBACK_FROM_ISR;      // 命令ID
        xMessage.u.xCallbackParameters.pxCallbackFunction = xFunctionToPend; // 函数指针
        xMessage.u.xCallbackParameters.pvParameter1 = pvParameter1;        // 参数1
        xMessage.u.xCallbackParameters.ulParameter2 = ulParameter2;          // 参数2

        /* 中断安全队列操作 */
        xReturn = xQueueSendFromISR( xTimerQueue,          // 目标队列
                                   &xMessage,             // 消息体
                                   pxHigherPriorityTaskWoken ); // 唤醒标志

        tracePEND_FUNC_CALL_FROM_ISR( xFunctionToPend, pvParameter1, ulParameter2, xReturn ); // 追踪事件

        return xReturn;  // 返回队列发送结果
    }

#endif /* INCLUDE_xTimerPendFunctionCall */
/*-----------------------------------------------------------*/

#if( INCLUDE_xTimerPendFunctionCall == 1 )

    /* 任务上下文延迟回调请求函数
       功能：从普通任务发送回调请求到定时器服务任务
       注意事项：必须在定时器队列初始化后调用（调度器启动后） */
    BaseType_t xTimerPendFunctionCall( PendedFunction_t xFunctionToPend,
                                      void *pvParameter1,
                                      uint32_t ulParameter2,
                                      TickType_t xTicksToWait )
    {
    DaemonTaskMessage_t xMessage;  // 命令消息结构
    BaseType_t xReturn;           // 队列操作结果

        configASSERT( xTimerQueue );  // 验证队列已存在

        /* 构建回调参数消息体 */
        xMessage.xMessageID = tmrCOMMAND_EXECUTE_CALLBACK;              // 命令ID
        xMessage.u.xCallbackParameters.pxCallbackFunction = xFunctionToPend; // 函数指针
        xMessage.u.xCallbackParameters.pvParameter1 = pvParameter1;          // 参数1
        xMessage.u.xCallbackParameters.ulParameter2 = ulParameter2;        // 参数2

        /* 标准队列发送（可阻塞） */
        xReturn = xQueueSendToBack( xTimerQueue,       // 目标队列
                                   &xMessage,         // 消息体
                                   xTicksToWait );  // 最大等待时间

        tracePEND_FUNC_CALL( xFunctionToPend, pvParameter1, ulParameter2, xReturn ); // 追踪事件

        return xReturn;  // 返回队列发送结果
    }

#endif /* INCLUDE_xTimerPendFunctionCall */

/*-----------------------------------------------------------*/

/* This entire source file will be skipped if the application is not configured
to include software timer functionality.  If you want to include software timer
functionality then ensure configUSE_TIMERS is set to 1 in FreeRTOSConfig.h. */
#endif /* configUSE_TIMERS == 1 */




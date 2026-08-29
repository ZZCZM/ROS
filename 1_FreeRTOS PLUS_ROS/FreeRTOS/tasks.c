/**
  ******************************************************************************
  * @file    tasks.c
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Source Code File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "StackMacros.h"
#include "semphr.h"
//#include "led.h"
/* Lint e961 and e750 are suppressed as a MISRA exception justified because the
MPU ports require MPU_WRAPPERS_INCLUDED_FROM_API_FILE to be defined for the
header files above, but not in this file, in order to generate the correct
privileged Vs unprivileged linkage and placement. */
/*-----------------------------------------------------------*/

/* 取消MPU_WRAPPERS_INCLUDED_FROM_API_FILE的定义 */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE /*lint !e961 !e750. */
/* Lint e961和e750被抑制，因为MISRA异常允许MPU端口要求MPU_WRAPPERS_INCLUDED_FROM_API_FILE
   在某些头文件中定义，但在本文件中不定义，以生成正确的特权与非特权链接和放置。 */
/*lint !e961 !e750 MISRA异常：MPU端口要求的定义 */

/* 设置configUSE_STATS_FORMATTING_FUNCTIONS为2以包含统计格式化函数但不包含stdio.h */
#if ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 )
	/* 在文件底部有两个可选函数，可以将uxTaskGetSystemState()函数生成的原始数据格式化为人类可读的文本。
	注意：格式化函数仅用于方便，不被视为内核的一部分。 */
	#include <stdio.h>
#endif /* configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) */

/* 根据配置选择是否进行任务调度 */
#if( configUSE_PREEMPTION == 0 )
	/* 如果使用协作式调度器，则不应仅因为唤醒了更高优先级任务而进行任务切换。 */
	#define taskYIELD_IF_USING_PREEMPTION()
#else
	#define taskYIELD_IF_USING_PREEMPTION() portYIELD_WITHIN_API()
#endif

/* 任务通知状态的宏定义 */
/* Values that can be assigned to the ucNotifyState member of the TCB. */
#define taskNOT_WAITING_NOTIFICATION	( ( uint8_t ) 0 )  // 任务不等待通知
#define taskWAITING_NOTIFICATION		( ( uint8_t ) 1 )  // 任务等待通知
#define taskNOTIFICATION_RECEIVED		( ( uint8_t ) 2 )  // 任务已接收通知

/* 任务堆栈初始化时填充的值 */
/* The value used to fill the stack of a task when the task is created. This is used purely for checking the high water mark for tasks. */
#define tskSTACK_FILL_BYTE	( 0xa5U )  // 堆栈填充字节，用于检查高水位标记

/* 根据配置定义任务堆栈和TCB的分配方式 */
/* Sometimes the FreeRTOSConfig.h settings only allow a task to be created using dynamically allocated RAM, in which case when any task is deleted it is known that both the task's stack and TCB need to be freed. */
/* Sometimes the FreeRTOSConfig.h settings only allow a task to be created using statically allocated RAM, in which case when any task is deleted it is known that neither the task's stack or TCB should be freed. */
/* Sometimes the FreeRTOSConfig.h settings allow a task to be created using either statically or dynamically allocated RAM, in which case a member of the TCB is used to record whether the stack and/or TCB were allocated statically or dynamically, so when a task is deleted the RAM that was allocated dynamically is freed again and no attempt is made to free the RAM that was allocated statically. */
#define tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE ( ( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) ) || ( portUSING_MPU_WRAPPERS == 1 ) )
#define tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB 		( ( uint8_t ) 0 )  // 动态分配堆栈和TCB
#define tskSTATICALLY_ALLOCATED_STACK_ONLY 			( ( uint8_t ) 1 )  // 静态分配堆栈，动态分配TCB
#define tskSTATICALLY_ALLOCATED_STACK_AND_TCB		( ( uint8_t ) 2 )  // 静态分配堆栈和TCB

/* 任务状态宏定义 */
/* Macros used by vListTask to indicate which state a task is in. */
#define tskBLOCKED_CHAR		( 'B' )  // 阻塞状态
#define tskREADY_CHAR		( 'R' )  // 就绪状态
#define tskDELETED_CHAR		( 'D' )  // 已删除状态
#define tskSUSPENDED_CHAR	( 'S' )  // 挂起状态

/* 根据调试器要求定义静态变量 */
/* Some kernel aware debuggers require the data the debugger needs access to be global, rather than file scope. */
#ifdef portREMOVE_STATIC_QUALIFIER
	#define static
#endif

/*----------------------------------------------------------------------------*/
/* FreeRTOS任务调度核心宏定义：
   实现不同架构下的优先级任务选择策略，包含：
   1. 通用优先级管理（软件实现）
   2. 硬件优化优先级管理（依赖特定CPU指令）
----------------------------------------------------------------------------*/

#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 )
/* 通用任务选择策略：适用于所有CPU架构，但效率较低 */

    /* 记录当前最高就绪任务优先级宏：
       当任务进入就绪态时更新全局最高优先级记录 */
    #define taskRECORD_READY_PRIORITY( uxPriority )                  \
    {                                                               \
        if( ( uxPriority ) > uxTopReadyPriority )                   \
        {                                                           \
            uxTopReadyPriority = ( uxPriority ); /* 更新全局变量 */  \
        }                                                           \
    } 

    /* 选择最高优先级任务宏：
       通过线性搜索找到最高优先级的非空就绪列表 */
    #define taskSELECT_HIGHEST_PRIORITY_TASK()                       \
    {                                                               \
        UBaseType_t uxTopPriority = uxTopReadyPriority;             \
        /* 从最高优先级向下搜索非空列表 */                          \
        while( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxTopPriority ] ) ) ) \
        {                                                           \
            configASSERT( uxTopPriority ); /* 优先级不为0的保障 */  \
            --uxTopPriority;                                        \
        }                                                           \
        /* 获取下一个就绪任务并更新全局优先级 */                    \
        listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) ); \
        uxTopReadyPriority = uxTopPriority; /* 记录当前最高优先级 */ \
    }

    /* 空定义：通用版本不需要这些操作 */
    #define taskRESET_READY_PRIORITY( uxPriority )
    #define portRESET_READY_PRIORITY( uxPriority, uxTopReadyPriority )

#else
/* 硬件优化任务选择策略：使用特定CPU指令加速查找 */

    /* 通过硬件指令记录最高优先级（如ARM的CLZ指令） */
    #define taskRECORD_READY_PRIORITY( uxPriority ) \
        portRECORD_READY_PRIORITY( uxPriority, uxTopReadyPriority )

    /* 硬件加速选择最高优先级任务 */
    #define taskSELECT_HIGHEST_PRIORITY_TASK()                       \
    {                                                               \
        UBaseType_t uxTopPriority;                                  \
        portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority ); \
        configASSERT( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ uxTopPriority ] ) ) > 0 ); \
        listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) ); \
    }

    /* 重置优先级状态宏：
       当就绪列表变空时更新硬件优先级状态 */
    #define taskRESET_READY_PRIORITY( uxPriority )                  \
    {                                                               \
        if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ ( uxPriority ) ] ) ) == 0 ) \
        {                                                           \
            portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ) ); \
        }                                                           \
    }

#endif /* 结束条件编译 */

/*----------------------------------------------------------------------------*/
/* FreeRTOS任务调度核心功能实现：
   包含延迟列表管理、就绪列表操作和TCB获取等关键操作 */

/*-----------------------------------------------------------*/
/* 延迟任务列表切换宏：
   当系统tick计数器溢出时交换延迟任务列表指针 */
#define taskSWITCH_DELAYED_LISTS()                                  \
{                                                                   \
    List_t *pxTemp;                                                 \
                                                                    \
    /* 断言确保当前延迟列表为空（安全切换条件） */                \
    configASSERT( ( listLIST_IS_EMPTY( pxDelayedTaskList ) ) );     \
                                                                    \
    pxTemp = pxDelayedTaskList;             /* 保存当前列表指针 */   \
    pxDelayedTaskList = pxOverflowDelayedTaskList; /* 切换主列表 */  \
    pxOverflowDelayedTaskList = pxTemp;     /* 旧列表转为溢出列表 */  \
    xNumOfOverflows++;              /* 溢出计数器递增（用于统计） */ \
    prvResetNextTaskUnblockTime();  /* 重置最近任务唤醒时间 */       \
}

/*-----------------------------------------------------------*/
/* 任务就绪化宏：
   将任务添加到对应优先级的就绪列表，并更新调度状态 */
#define prvAddTaskToReadyList( pxTCB )                              \
    traceMOVED_TASK_TO_READY_STATE( pxTCB );  /* 追踪调试钩子 */     \
    taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority ); /* 记录优先级 */ \
    vListInsertEnd( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), /* 插入就绪列表末尾 */ \
                    &( ( pxTCB )->xStateListItem ) );               \
    tracePOST_MOVED_TASK_TO_READY_STATE( pxTCB ) /* 后置追踪钩子 */

/*-----------------------------------------------------------*/
/* TCB获取宏：
   安全处理NULL句柄，返回当前任务控制块或指定句柄 */
// 空句柄返回当前任务TCB   
// 非空返回指定TCB    
#define prvGetTCBFromHandle( pxHandle ) ( ( ( pxHandle ) == NULL ) ? ( TCB_t * ) pxCurrentTCB : ( TCB_t * ) ( pxHandle ) )
/*-----------------------------------------------------------*/
/* 事件列表项标记宏：
   标识事件列表项的值被临时借用，防止优先级更新干扰 */
#if( configUSE_16_BIT_TICKS == 1 )
    #define taskEVENT_LIST_ITEM_VALUE_IN_USE  0x8000U      // 16位系统标记位
#else
    #define taskEVENT_LIST_ITEM_VALUE_IN_USE 0x80000000UL // 32位系统标记位
#endif

/*
 * Task control block.  A task control block (TCB) is allocated for each task,
 * and stores task state information, including a pointer to the task's context
 * (the task's run time environment, including register values)
 */
// 1. 修改任务控制块结构
/*
 * 任务控制块（Task Control Block, TCB）结构定义
 * 功能：
 *   1. 存储任务的核心运行时信息，包括堆栈指针、状态列表项、优先级等
 *   2. 管理任务的上下文切换和状态转换
 *   3. 新增阶段继承机制相关成员，用于实现动态优先级调整和资源管理
 * 作用：
 *   - 作为任务的核心数据结构，系统通过TCB管理所有任务的执行状态
 *   - 维护任务的堆栈信息、优先级状态和资源持有情况
 *   - 支持优先级继承和阶段式优先级调整机制
 */
typedef struct tskTaskControlBlock
{
    /* 堆栈顶部指针（必须是结构体第一个成员）
     * 指向任务堆栈中最后压入的元素位置
     * 在架构相关的端口代码中用于上下文切换 */
    volatile StackType_t *pxTopOfStack;

    #if ( portUSING_MPU_WRAPPERS == 1 )
        /* MPU（内存保护单元）设置
         * 包含任务的内存访问权限配置
         * 必须是结构体第二个成员 */
        xMPU_SETTINGS xMPUSettings;
    #endif

    /* 状态列表项 - 用于将任务连接到状态列表（就绪、阻塞、挂起）
     * 包含指向相邻列表项的指针和所属列表容器 */
    ListItem_t xStateListItem;

    /* 事件列表项 - 用于将任务连接到事件等待列表
     * 当任务等待事件（如信号量、队列）时使用 */
    ListItem_t xEventListItem;

    /* 任务基础优先级（0=最低优先级）
     * 创建任务时指定的优先级
     * 可能被继承机制临时修改 */
    UBaseType_t uxPriority;

    /* 堆栈起始指针
     * 指向任务堆栈内存区域的起始地址
     * 用于堆栈溢出检测和内存回收 */
    StackType_t *pxStack;

    /* 任务名称（用于调试）
     * 最大长度由configMAX_TASK_NAME_LEN定义
     * 格式：以NULL结尾的字符串 */
    char pcTaskName[ configMAX_TASK_NAME_LEN ];

    /* 阶段继承机制相关成员 */
    UBaseType_t uxInheritedPriority;  // 当前生效的继承优先级（可能高于基础优先级）
    UBaseType_t uxCurrentStage;       // 当前执行阶段索引（从0开始计数）
    UBaseType_t uxTotalStages;        // 总阶段数量（决定优先级变化次数）
    UBaseType_t *puxStagePriorities;  // 各阶段目标优先级的数组指针
    List_t xResourcesList;            // 任务当前持有的资源（互斥量等）列表
    uint8_t ucInheritanceActive;      // 继承状态标志（1=正在继承优先级）

    #if ( portSTACK_GROWTH > 0 )
        /* 堆栈结束指针（堆栈向上增长架构专用）
         * 用于检测堆栈溢出
         * 在堆栈向下增长的架构中不需要 */
        StackType_t *pxEndOfStack;
    #endif

    #if ( portCRITICAL_NESTING_IN_TCB == 1 )
        /* 关键区嵌套深度计数器
         * 记录进入/退出关键区的次数
         * 当计数器为0时才允许任务切换 */
        UBaseType_t uxCriticalNesting;
    #endif

    #if ( configUSE_TRACE_FACILITY == 1 )
        /* 跟踪调试用标识号
         * uxTCBNumber：TCB创建序列号（用于检测任务重建）
         * uxTaskNumber：第三方跟踪工具使用的自定义编号 */
        UBaseType_t uxTCBNumber;
        UBaseType_t uxTaskNumber;
    #endif

    #if ( configUSE_MUTEXES == 1 )
        /* 互斥量相关成员
         * uxBasePriority：基础优先级（用于优先级继承）
         * uxMutexesHeld：当前持有的互斥量数量 */
        UBaseType_t uxBasePriority;
        UBaseType_t uxMutexesHeld;
    #endif

    #if ( configUSE_APPLICATION_TASK_TAG == 1 )
        /* 任务标签函数指针
         * 允许应用层关联自定义回调函数
         * 常用于运行时分析工具 */
        TaskHookFunction_t pxTaskTag;
    #endif

    #if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
        /* 线程本地存储指针数组
         * 允许任务存储私有数据
         * 索引由应用层定义使用 */
        void *pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
    #endif

    #if ( configGENERATE_RUN_TIME_STATS == 1 )
        /* 运行时间计数器
         * 记录任务处于运行状态的总时间
         * 单位由portCONFIGURE_TIMER_FOR_RUN_TIME_STATS定义 */
        uint32_t ulRunTimeCounter;
    #endif

    #if ( configUSE_NEWLIB_REENTRANT == 1 )
        /* Newlib可重入结构体
         * 为任务提供独立的C库运行时环境
         * 注意：需要自行实现系统级锁机制 */
        struct _reent xNewLib_reent;
    #endif

    #if ( configUSE_TASK_NOTIFICATIONS == 1 )
        /* 任务通知相关成员
         * ulNotifiedValue：通知携带的32位值
         * ucNotifyState：通知状态标志
         * 状态可能为：
         * taskNOT_WAITING_NOTIFICATION/
         * taskWAITING_NOTIFICATION/
         * taskNOTIFICATION_RECEIVED */
        volatile uint32_t ulNotifiedValue;
        volatile uint8_t ucNotifyState;
    #endif

    #if ( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
        /* 内存分配方式标志
         * pdTRUE表示静态分配（不可释放）
         * pdFALSE表示动态分配（需要回收） */
        uint8_t ucStaticallyAllocated;
    #endif

    #if ( INCLUDE_xTaskAbortDelay == 1 )
        /* 延迟中止标志
         * 当任务通过vTaskDelayUntil()进入阻塞时
         * 可能被其他任务/中断中止延迟 */
        uint8_t ucDelayAborted;
    #endif

} tskTCB;
// 2. 资源管理系统核心数据结构定义
/*
 * 资源管理系统核心数据结构定义
 * 架构组成：
 *   1. 资源控制块(ResourceCB_t) - 管理单个资源实例
 *   2. 资源映射结构(ResourceMapping_t) - 维护信号量与资源的映射关系
 * 设计目标：
 *   - 实现优先级感知的资源管理
 *   - 支持多阶段任务调度
 *   - 保证线程安全的资源访问
 */

/*----------------------------------------------------------------------------*/
/* 资源控制块结构定义（Resource Control Block）*/
/*----------------------------------------------------------------------------*/

/* 保留原始TCB类型定义（向后兼容性设计）
 * 目的：支持旧版本调试工具识别任务控制块
 * 技术细节：
 * - 原始tskTCB结构体通过typedef重命名为TCB_t
 * - 保证二进制兼容性 */
typedef tskTCB TCB_t;

/*
 * 资源控制块（ResourceCB_t）
 * 功能：管理系统内单个资源实例的完整生命周期
 */
typedef struct xRESOURCE_CB
{
    /* 资源等待队列（双向链表）
     * 数据结构：
     * - 使用FreeRTOS标准List_t结构
     * - 存储TCB指针而非任务句柄
     * 操作特性：
     * - 任务按优先级降序排列（最高优先级在头部）
     * - 支持超时机制（通过vTaskDelayUntil实现） */
    List_t  xTasksWaitingForResource;

    /* 当前资源持有者
     * 安全机制：
     * - 使用指针直接访问TCB，减少查表开销
     * - NULL状态表示资源空闲
     * 关联关系：
     * - 与TCB中的xResourcesList形成双向链接
     * - 通过pxCurrentHolder->xResourcesList可遍历所有持有资源 */
    TCB_t *pxCurrentHolder;

    /* 资源绑定阶段标识
     * 取值范围：
     * - 0 ~ (configMAX_STAGES-1)
     * 阶段规则：
     * - 任务只能访问相同或更低阶段的资源
     * - 跨阶段访问触发优先级继承 */
    UBaseType_t uxResourceStage;

    /* 资源链表项（嵌入到持有者TCB）
     * 内存布局：
     * - 包含pxNext/pxPrevious指针
     * - 存储资源块指针作为所有者
     * 操作特性：
     * - 通过listINSERT_END实现快速插入
     * - 任务切换时自动维护链接关系 */
    ListItem_t xResourceListItem;
} ResourceCB_t;

/*----------------------------------------------------------------------------*/
/* 资源映射管理系统 */
/*----------------------------------------------------------------------------*/

/*
 * 资源映射节点（ResourceMapping_t）
 * 功能：建立信号量与资源控制块的映射关系
 * 设计特点：
 * - 单向链表实现动态扩展
 * - O(1)插入/O(n)查找复杂度
 */
typedef struct xRESOURCE_MAPPING
{
    /* 信号量标识符
     * 生命周期：
     * - 与资源控制块解耦
     * - 需手动维护映射关系
     * 特殊处理：
     * - Binary Semaphore用于互斥访问
     * - Counting Semaphore用于共享资源 */
    SemaphoreHandle_t xSemaphore;

    /* 资源控制块指针
     * 内存管理：
     * - 由资源创建函数动态分配
     * - 需在信号量删除时同步释放
     * 访问保护：
     * - 通过xResourceMappingMutex互斥量保护 */
    ResourceCB_t *pxResourceCB;

    /* 链表指针
     * 数据结构：
     * - 头插法维护链表（新节点插入头部）
     * - 遍历方向从头至尾
     * 性能优化：
     * - 使用volatile修饰保证内存可见性 */
    struct xRESOURCE_MAPPING *pxNext;
} ResourceMapping_t;

/*----------------------------------------------------------------------------*/
/* 全局资源映射管理变量 */
/*----------------------------------------------------------------------------*/

/* 资源映射链表头指针
 * 初始化：
 * - 系统启动时初始化为NULL
 * 线程安全：
 * - 操作需获取xResourceMappingMutex */
static ResourceMapping_t *pxResourceMappingList = NULL;

/* 资源映射操作互斥量
 * 创建时机：
 * - 首次资源创建时动态初始化
 * 锁机制：
 * - 采用优先级继承协议
 * - 最大阻塞时间portMAX_DELAY */
static SemaphoreHandle_t xResourceMappingMutex = NULL;

//系统中允许的最大阶段数（configMAX_STAGES）等于最大优先级数（configMAX_PRIORITIES）
#define configMAX_STAGES configMAX_PRIORITIES 
/*----------------------------------------------------------------------------*/
/* FreeRTOS内核核心数据结构定义：
   包含任务调度相关的全局变量、列表管理和系统状态跟踪 */

/* lint静态检查抑制：经过人工分析确认需要volatile修饰的静态变量 */
/*lint -e956 */

/* 当前运行任务控制块指针（volatile确保多核/中断环境可见性） */
PRIVILEGED_DATA TCB_t * volatile pxCurrentTCB = NULL;

/*------------------------ 任务列表管理 ------------------------*/
PRIVILEGED_DATA static List_t pxReadyTasksLists[ configMAX_PRIORITIES ]; /* 优先级就绪任务列表数组，索引0为最低优先级 */
PRIVILEGED_DATA static List_t xDelayedTaskList1;        /* 主延迟任务列表（当前tick周期） */
PRIVILEGED_DATA static List_t xDelayedTaskList2;        /* 溢出延迟任务列表（tick溢出时使用） */
PRIVILEGED_DATA static List_t * volatile pxDelayedTaskList = &xDelayedTaskList1; /* 当前活动延迟列表指针 */
PRIVILEGED_DATA static List_t * volatile pxOverflowDelayedTaskList = &xDelayedTaskList2; /* 溢出延迟列表指针 */
PRIVILEGED_DATA static List_t xPendingReadyList;         /* 调度器挂起期间就绪的任务暂存列表 */

/* 任务删除支持（条件编译） */
#if( INCLUDE_vTaskDelete == 1 )
    PRIVILEGED_DATA static List_t xTasksWaitingTermination; /* 已删除但未清理的任务列表 */
    PRIVILEGED_DATA static volatile UBaseType_t uxDeletedTasksWaitingCleanUp = 0; /* 待清理任务计数器 */
#endif

/* 任务挂起支持（条件编译） */
#if ( INCLUDE_vTaskSuspend == 1 )
    PRIVILEGED_DATA static List_t xSuspendedTaskList;    /* 被挂起任务列表 */
#endif

/*----------------------- 系统状态跟踪变量 ----------------------*/
PRIVILEGED_DATA static volatile UBaseType_t uxCurrentNumberOfTasks = 0;  /* 当前系统任务总数 */
PRIVILEGED_DATA static volatile TickType_t xTickCount = 0;               /* 系统节拍计数器 */
PRIVILEGED_DATA static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY; /* 当前最高就绪优先级 */
PRIVILEGED_DATA static volatile BaseType_t xSchedulerRunning = pdFALSE;   /* 调度器运行状态标志 */
PRIVILEGED_DATA static volatile UBaseType_t uxPendedTicks = 0;          /* 挂起的tick事件计数 */
PRIVILEGED_DATA static volatile BaseType_t xYieldPending = pdFALSE;      /* 待处理的任务切换请求 */
PRIVILEGED_DATA static volatile BaseType_t xNumOfOverflows = 0;          /* tick计数器溢出次数统计 */
PRIVILEGED_DATA static UBaseType_t uxTaskNumber = 0;                    /* 任务创建序列号生成器 */
PRIVILEGED_DATA static volatile TickType_t xNextTaskUnblockTime = 0;     /* 下一个任务唤醒时间点 */
PRIVILEGED_DATA static TaskHandle_t xIdleTaskHandle = NULL;              /* 空闲任务句柄 */

/* 调度器挂起状态管理 */
PRIVILEGED_DATA static volatile UBaseType_t uxSchedulerSuspended = pdFALSE; /* 调度器挂起深度计数器 */

/* 运行时统计支持（条件编译） */
#if ( configGENERATE_RUN_TIME_STATS == 1 )
    PRIVILEGED_DATA static uint32_t ulTaskSwitchedInTime = 0UL;  /* 上次任务切换时间戳 */
    PRIVILEGED_DATA static uint32_t ulTotalRunTime = 0UL;        /* 总运行时间统计 */
#endif

/* 恢复lint检查 */
/*lint +e956 */

/*----------------------- 回调函数原型声明 ----------------------*/
#if( configCHECK_FOR_STACK_OVERFLOW > 0 )
    extern void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName); /* 栈溢出钩子函数 */
#endif

#if( configUSE_TICK_HOOK > 0 )
    extern void vApplicationTickHook(void);  /* 系统节拍钩子函数 */
#endif 

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    extern void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
                                            StackType_t **ppxIdleTaskStackBuffer, 
                                            uint32_t *pulIdleTaskStackSize); /* 静态分配空闲任务内存 */
#endif
/*----------------------------------------------------------------------------*/
/* FreeRTOS内核私有函数声明：
   这些函数供内核内部使用，实现任务调度和管理的核心逻辑 */

/* 任务挂起状态检测函数（条件编译） */
#if ( INCLUDE_vTaskSuspend == 1 )
    /* 功能：检测指定任务是否处于挂起状态
       参数：xTask - 待检测任务句柄
       返回：pdTRUE(挂起)/pdFALSE(非挂起) */
    static BaseType_t prvTaskIsTaskSuspended( const TaskHandle_t xTask ) PRIVILEGED_FUNCTION;
#endif

/* 内核列表初始化函数 */
/* 功能：初始化所有任务管理链表（就绪、延迟、挂起等）
   调用时机：创建第一个用户任务时自动调用 */
static void prvInitialiseTaskLists( void ) PRIVILEGED_FUNCTION;

/* 空闲任务函数原型声明 */
/* 功能：系统空闲任务主函数
   职责：1. 处理已删除任务的内存回收
         2. 执行低功耗模式（若启用）
         3. 调用用户定义的空闲钩子函数 */
static portTASK_FUNCTION_PROTO( prvIdleTask, pvParameters );

/* 任务控制块删除函数（条件编译） */
#if ( INCLUDE_vTaskDelete == 1 )
    /* 功能：彻底释放TCB及关联资源
       参数：pxTCB - 待删除任务控制块指针
       注意：会释放任务栈内存 */
    static void prvDeleteTCB( TCB_t *pxTCB ) PRIVILEGED_FUNCTION;
#endif

/* 终止任务清理函数 */
/* 功能：由空闲任务周期调用，清理已标记删除的任务
   机制：遍历xTasksWaitingTermination列表 */
static void prvCheckTasksWaitingTermination( void ) PRIVILEGED_FUNCTION;

/* 当前任务加入延迟列表函数 */
/* 功能：将运行中任务转移到延迟列表
   参数：
   - xTicksToWait: 需要阻塞的tick数
   - xCanBlockIndefinitely: 是否允许无限阻塞
   注意：处理tick溢出情况 */
static void prvAddCurrentTaskToDelayedList( TickType_t xTicksToWait, 
                                          const BaseType_t xCanBlockIndefinitely ) PRIVILEGED_FUNCTION;

/* 任务状态信息收集函数（调试用） */
#if ( configUSE_TRACE_FACILITY == 1 )
    /* 功能：填充指定列表中的任务状态信息到数组
       参数：
       - pxTaskStatusArray: 输出数组
       - pxList: 扫描的任务列表（如就绪列表）
       - eState: 任务状态标识
       返回：填充的任务数量 */
    static UBaseType_t prvListTasksWithinSingleList( TaskStatus_t *pxTaskStatusArray,
                                                   List_t *pxList, 
                                                   eTaskState eState ) PRIVILEGED_FUNCTION;
#endif

/* 任务名搜索函数（条件编译） */
#if ( INCLUDE_xTaskGetHandle == 1 )
    /* 功能：在指定列表中按任务名查找TCB
       参数：
       - pxList: 搜索的目标列表
       - pcNameToQuery: 要查找的任务名
       返回：匹配的TCB指针或NULL */
    static TCB_t *prvSearchForNameWithinSingleList( List_t *pxList,
                                                  const char pcNameToQuery[] ) PRIVILEGED_FUNCTION;
#endif

/* 栈高水位线检测函数（调试/统计用） */
#if ( ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) )
    /* 功能：计算任务栈剩余空间
       参数：pucStackByte - 栈起始地址
       返回：未使用的栈空间（单位：字） */
    static uint16_t prvTaskCheckFreeStackSpace( const uint8_t * pucStackByte ) PRIVILEGED_FUNCTION;
#endif

/* Tickless空闲模式支持函数 */
#if ( configUSE_TICKLESS_IDLE != 0 )
    /* 功能：预测下一个任务唤醒前的空闲时间
       返回：可休眠的tick数
       应用：用于低功耗模式决策 */
    static TickType_t prvGetExpectedIdleTime( void ) PRIVILEGED_FUNCTION;
#endif

/* 最近任务唤醒时间更新函数 */
/* 功能：重新计算xNextTaskUnblockTime值
   调用场景：延迟列表变更后 */
static void prvResetNextTaskUnblockTime( void );

/* 任务名格式化函数（调试输出） */
#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )
    /* 功能：将任务名填充到缓冲区并右对齐
       参数：
       - pcBuffer: 输出缓冲区
       - pcTaskName: 原始任务名
       返回：下一个可写位置指针 */
    static char *prvWriteNameToBuffer( char *pcBuffer, const char *pcTaskName ) PRIVILEGED_FUNCTION;
#endif

/*----------------------------------------------------------------------------*/
/* FreeRTOS任务创建核心实现：
   包含静态任务创建API和任务初始化内部函数 */

/* 新任务初始化函数声明：
   负责配置任务控制块(TCB)和栈空间的底层初始化 */
static void prvInitialiseNewTask(
    TaskFunction_t pxTaskCode,        // 任务函数指针
    const char * const pcName,       // 任务名称（调试用）
    const uint32_t ulStackDepth,     // 栈深度（以字为单位）
    void * const pvParameters,       // 任务参数指针
    UBaseType_t uxPriority,          // 任务优先级
    TaskHandle_t * const pxCreatedTask, // 输出参数：创建的任务句柄
    TCB_t *pxNewTCB,                 // 预分配的TCB内存指针
    const MemoryRegion_t * const xRegions // MPU内存区域配置（未使用）
) PRIVILEGED_FUNCTION;

/* 新任务就绪化函数声明：
   将初始化后的任务加入调度系统 */
static void prvAddNewTaskToReadyList( TCB_t *pxNewTCB ) PRIVILEGED_FUNCTION;

/*-----------------------------------------------------------*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )

/* 静态任务创建API函数：
   允许用户手动分配任务栈和TCB内存 */
TaskHandle_t xTaskCreateStatic(
    TaskFunction_t pxTaskCode,       // 任务函数入口
    const char * const pcName,       // 任务名称（最长configMAX_TASK_NAME_LEN）
    const uint32_t ulStackDepth,     // 栈深度（以字为单位）
    void * const pvParameters,       // 任务初始化参数
    UBaseType_t uxPriority,          // 任务优先级（0~configMAX_PRIORITIES-1）
    StackType_t * const puxStackBuffer, // 用户提供的栈内存指针
    StaticTask_t * const pxTaskBuffer // 用户提供的TCB内存指针
)
{
    TCB_t *pxNewTCB;                 // 新任务控制块指针
    TaskHandle_t xReturn;            // 函数返回值（任务句柄）

    /* 参数合法性检查（生产环境可关闭） */
    configASSERT( puxStackBuffer != NULL ); // 栈缓冲区必须非空
    configASSERT( pxTaskBuffer != NULL );    // TCB缓冲区必须非空

    if( ( pxTaskBuffer != NULL ) && ( puxStackBuffer != NULL ) )
    {
        /* 将用户提供的静态内存转换为TCB结构 */
        pxNewTCB = ( TCB_t * ) pxTaskBuffer; // 类型转换（需保证内存对齐）
        pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer; // 关联栈空间

        /* 标记静态分配属性（供删除时检查） */
        #if( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
        {
            pxNewTCB->ucStaticallyAllocated = tskSTATICALLY_ALLOCATED_STACK_AND_TCB; // 0x01
        }
        #endif /* configSUPPORT_DYNAMIC_ALLOCATION */

        /* 执行底层初始化 */
        prvInitialiseNewTask( pxTaskCode,   // 任务函数
                            pcName,        // 任务名
                            ulStackDepth,  // 栈深度 
                            pvParameters, // 参数
                            uxPriority,   // 优先级
                            &xReturn,      // 返回句柄
                            pxNewTCB,      // TCB指针
                            NULL );        // MPU区域（未使用）

        /* 将任务加入就绪队列 */
        prvAddNewTaskToReadyList( pxNewTCB );
    }
    else
    {
        /* 参数错误处理 */
        xReturn = NULL; // 返回空句柄表示失败
    }

    return xReturn; // 返回新任务句柄
}

#endif /* configSUPPORT_STATIC_ALLOCATION */

/*-----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* MPU保护任务创建函数：
   用于创建具有内存保护配置的任务，适用于安全关键型系统 */

#if( portUSING_MPU_WRAPPERS == 1 )

BaseType_t xTaskCreateRestricted( 
    const TaskParameters_t * const pxTaskDefinition,  // MPU任务参数结构体指针
    TaskHandle_t *pxCreatedTask                       // 输出参数：创建的任务句柄
)
{
    TCB_t *pxNewTCB;                                  // 新任务控制块指针
    BaseType_t xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY; // 返回值初始化

    /* 参数合法性断言检查（调试模式生效） */
    configASSERT( pxTaskDefinition->puxStackBuffer ); // 必须提供静态栈缓冲区

    if( pxTaskDefinition->puxStackBuffer != NULL )
    {
        /* 动态分配TCB内存（TCB结构体需要特权访问权限） */
        pxNewTCB = ( TCB_t * ) pvPortMalloc( sizeof( TCB_t ) );

        if( pxNewTCB != NULL )
        {
            /* 配置任务栈指针 */
            pxNewTCB->pxStack = pxTaskDefinition->puxStackBuffer;

            /* 混合内存分配标记：
               tskSTATICALLY_ALLOCATED_STACK_ONLY = 0x02 
               表示栈静态分配，TCB动态分配 */
            pxNewTCB->ucStaticallyAllocated = tskSTATICALLY_ALLOCATED_STACK_ONLY;

            /* 执行深层初始化（含MPU配置） */
            prvInitialiseNewTask(
                pxTaskDefinition->pvTaskCode,      // 任务函数入口
                pxTaskDefinition->pcName,          // 任务名称
                ( uint32_t ) pxTaskDefinition->usStackDepth, // 栈深度转换
                pxTaskDefinition->pvParameters,    // 任务参数
                pxTaskDefinition->uxPriority,       // 任务优先级
                pxCreatedTask,                      // 返回句柄
                pxNewTCB,                          // TCB指针
                pxTaskDefinition->xRegions          // MPU内存区域配置数组
            );

            /* 将任务加入就绪队列 */
            prvAddNewTaskToReadyList( pxNewTCB );
            xReturn = pdPASS; // 返回创建成功状态
        }
    }

    return xReturn; // 返回状态：pdPASS或errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY
}

#endif /* portUSING_MPU_WRAPPERS */

/*-----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* FreeRTOS动态任务创建函数：
   从堆中分配TCB和栈内存创建新任务，支持不同栈增长方向 */

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

BaseType_t xTaskCreate(
    TaskFunction_t pxTaskCode,        // 任务函数指针
    const char * const pcName,       // 任务名称（最大长度configMAX_TASK_NAME_LEN）
    const uint16_t usStackDepth,     // 栈深度（以字为单位）
    void * const pvParameters,       // 任务参数指针
    UBaseType_t uxPriority,          // 任务优先级（0 ~ configMAX_PRIORITIES-1）
    TaskHandle_t * const pxCreatedTask // 输出参数：创建的任务句柄
)
{
    TCB_t *pxNewTCB = NULL;          // 新任务控制块指针
    BaseType_t xReturn;              // 函数返回值

    /* 根据栈增长方向决定内存分配顺序 */
    #if( portSTACK_GROWTH > 0 ) /* 栈向上增长*/
    {
        /* 先分配TCB再分配栈，防止栈覆盖TCB */
        pxNewTCB = ( TCB_t * ) pvPortMalloc( sizeof( TCB_t ) ); // 分配TCB内存
        
        if( pxNewTCB != NULL )
        {
            /* 分配任务栈空间（字节数 = 字数 * 字大小） */
            pxNewTCB->pxStack = ( StackType_t * ) pvPortMalloc( ( ( size_t ) usStackDepth ) * sizeof( StackType_t ) );
            
            if( pxNewTCB->pxStack == NULL ) // 栈分配失败处理
            {
                vPortFree( pxNewTCB ); // 释放已分配的TCB
                pxNewTCB = NULL;       // 重置指针防止野指针
            }
        }
    }
    #else /* 栈向下增长（如ARM Cortex-M） */
    {
        StackType_t *pxStack;
        
        /* 先分配栈再分配TCB */
        pxStack = ( StackType_t * ) pvPortMalloc( ( ( size_t ) usStackDepth ) * sizeof( StackType_t ) );
        
        if( pxStack != NULL )
        {
            pxNewTCB = ( TCB_t * ) pvPortMalloc( sizeof( TCB_t ) ); // 分配TCB
            if( pxNewTCB != NULL )
            {
                pxNewTCB->pxStack = pxStack; // 关联栈空间
            }
            else // TCB分配失败处理
            {
                vPortFree( pxStack ); // 释放已分配的栈空间
            }
        }
    }
    #endif /* portSTACK_GROWTH */

    if( pxNewTCB != NULL )
    {
        #if( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
        {
            /* 标记完全动态分配（后续删除时释放内存） */
            pxNewTCB->ucStaticallyAllocated = tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB; // 0x00
        }
        #endif

        /* 执行深层初始化（配置栈、任务名、优先级等） */
        prvInitialiseNewTask(
            pxTaskCode,             // 任务入口函数
            pcName,                // 任务名称
            ( uint32_t ) usStackDepth, // 转换类型保证兼容性
            pvParameters,          // 任务参数
            uxPriority,            // 优先级
            pxCreatedTask,         // 输出句柄
            pxNewTCB,              // TCB指针
            NULL                   // MPU区域（未使用）
        );
        
        /* 将任务加入就绪队列 */
        prvAddNewTaskToReadyList( pxNewTCB );
        xReturn = pdPASS; // 返回成功状态
    }
    else
    {
        xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY; // 内存不足错误
    }

    return xReturn;
}

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
// 函数名称: prvInitialiseNewTask
// 功能描述: 
//   初始化新任务控制块（TCB）的核心函数，完成以下关键操作：
//   1. 配置任务的MPU权限模式（当启用MPU时）
//   2. 堆栈内存初始化（根据调试需求填充标记字节）
//   3. 计算堆栈顶部地址（适应不同硬件架构的堆栈增长方向）
//   4. 任务名称存储（带安全终止符处理）
//   5. 任务优先级验证和标准化处理
//   6. 初始化任务状态链表和事件链表
//   7. 配置任务扩展功能（互斥量、运行时统计、TLS等）
//   8. 初始化任务上下文堆栈结构
//   9. 返回任务句柄
// 参数说明:
//   pxTaskCode    - 函数指针，指向任务入口函数
//   pcName        - 最长configMAX_TASK_NAME_LEN-1字符的任务名称
//   ulStackDepth  - 堆栈深度（以StackType_t为单位的元素数量）
//   pvParameters  - 传递给任务函数的参数指针
//   uxPriority    - 带可选特权位的任务优先级
//   pxCreatedTask - 输出参数，返回新创建的任务句柄
//   pxNewTCB      - 要初始化的任务控制块指针
//   xRegions      - MPU内存区域配置（当启用MPU时有效）
// 注意事项:
//   - 包含对MISRA规则的特定例外处理（通过lint注释标注）
//   - 采用条件编译适配不同硬件架构和系统配置
/*-----------------------------------------------------------*/
static void prvInitialiseNewTask( TaskFunction_t pxTaskCode,
    const char * const pcName,
    const uint32_t ulStackDepth,
    void * const pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t * const pxCreatedTask,
    TCB_t *pxNewTCB,
    const MemoryRegion_t * const xRegions ) /*lint !e971 允许未限定字符类型用于字符串*/
{
    StackType_t *pxTopOfStack;  // 堆栈顶部指针（根据不同架构可能指向高地址或低地址）
    UBaseType_t x;              // 通用循环计数器/临时变量

    /* MPU权限模式处理（条件编译块开始） */
    #if( portUSING_MPU_WRAPPERS == 1 )
        /* 特权模式判定：
           uxPriority的portPRIVILEGE_BIT位决定任务运行模式 
           特权位清除后的值为真实优先级 */
        BaseType_t xRunPrivileged;
        if( ( uxPriority & portPRIVILEGE_BIT ) != 0U )  // 检测特权位
        {
            xRunPrivileged = pdTRUE;   // 标记为特权模式
        }
        else
        {
            xRunPrivileged = pdFALSE;  // 标记为非特权模式
        }
        uxPriority &= ~portPRIVILEGE_BIT;  // 清除优先级中的特权标志位
    #endif /* portUSING_MPU_WRAPPERS */

    /* 堆栈调试初始化（条件编译块） */
    #if( ( configCHECK_FOR_STACK_OVERFLOW > 1 ) || ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) )
    {
        /* 堆栈填充策略：
           使用tskSTACK_FILL_BYTE（通常为0xa5）填充整个堆栈空间
           目的：检测堆栈溢出、可视化堆栈使用情况 */ 
        ( void ) memset( pxNewTCB->pxStack, ( int ) tskSTACK_FILL_BYTE, 
            ( size_t ) ulStackDepth * sizeof( StackType_t ) ); // 注意字节数计算
    }
    #endif /* 堆栈调试相关配置 */

    /* 堆栈拓扑结构初始化 */
    #if( portSTACK_GROWTH < 0 )  // 向下增长堆栈（如ARM架构）
    {
        /* 计算物理堆栈顶部：
           pxStack指向分配内存起始地址
           堆栈顶部 = 起始地址 + (元素数量-1) 
           对齐处理保证地址符合硬件要求 */
        pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 ); 
        pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) 
            & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) ); /*lint !e923 允许指针-整型转换*/

        /* 堆栈对齐验证：
           使用configASSERT确保对齐正确，避免硬件异常 */
        configASSERT( ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack 
            & ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) == 0UL ) );
    }
    #else /* 向上增长堆栈 */
    {
        pxTopOfStack = pxNewTCB->pxStack;  // 堆栈顶部即分配内存起始地址

        /* 堆栈起始地址对齐验证 */
        configASSERT( ( ( ( portPOINTER_SIZE_TYPE ) pxNewTCB->pxStack 
            & ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) == 0UL ) );

        /* 记录堆栈结束地址（用于堆栈检查）：
           pxEndOfStack = 起始地址 + (元素数量-1) */
        pxNewTCB->pxEndOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 );
    }
    #endif /* 堆栈增长方向处理 */

    /* 任务名称存储策略 */
    for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
    {
        pxNewTCB->pcTaskName[ x ] = pcName[ x ];  // 逐字符复制
        
        /* 安全终止策略：
           检测源字符串终止符，避免越界访问 */
        if( pcName[ x ] == 0x00 )
        {
            break;  // 遇到终止符立即停止复制
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖标记（空分支）
        }
    }

    /* 强制添加终止符：
       当源字符串长度 >= configMAX_TASK_NAME_LEN时保证安全性 */
    pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1 ] = '\0';

    /* 优先级标准化处理 */
    if( uxPriority >= ( UBaseType_t ) configMAX_PRIORITIES )  // 优先级校验
    {
        uxPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;  // 限制到最大有效优先级
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();  // 测试覆盖标记
    }

    /* 基础优先级设置 */
    pxNewTCB->uxPriority = uxPriority;  // 存储验证后的优先级
    #if ( configUSE_MUTEXES == 1 )       // 互斥量支持配置
    {
        pxNewTCB->uxBasePriority = uxPriority;  // 初始化基本优先级（用于优先级继承）
        pxNewTCB->uxMutexesHeld = 0;            // 初始化持有互斥量计数器
    }
    #endif

    /* 链表系统初始化 */
    vListInitialiseItem( &( pxNewTCB->xStateListItem ) );   // 状态链表项初始化
    vListInitialiseItem( &( pxNewTCB->xEventListItem ) );   // 事件链表项初始化

    /* 链表所有权绑定：
       通过链表项反向引用TCB结构 */
    listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );

    /* 事件列表优先级设置：
       值越大优先级越低，configMAX_PRIORITIES - uxPriority实现优先级逆序 */
    listSET_LIST_ITEM_VALUE( &( pxNewTCB->xEventListItem ), 
        ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) uxPriority ); /*lint !e961 允许类型转换*/
    listSET_LIST_ITEM_OWNER( &( pxNewTCB->xEventListItem ), pxNewTCB );

    /* 临界区嵌套计数器初始化 */
    #if ( portCRITICAL_NESTING_IN_TCB == 1 )
    {
        pxNewTCB->uxCriticalNesting = ( UBaseType_t ) 0U;  // 初始化为0层嵌套
    }
    #endif

    /* 任务标签初始化 */
    #if ( configUSE_APPLICATION_TASK_TAG == 1 )
    {
        pxNewTCB->pxTaskTag = NULL;  // 初始化为空指针
    }
    #endif

    /* 运行时统计初始化 */
    #if ( configGENERATE_RUN_TIME_STATS == 1 )
    {
        pxNewTCB->ulRunTimeCounter = 0UL;  // 清零运行时间计数器
    }
    #endif

    /* MPU配置存储 */
    #if ( portUSING_MPU_WRAPPERS == 1 )
    {
        /* 存储MPU设置到TCB：
           xRegions参数传递内存区域配置
           pxStack和ulStackDepth确定任务堆栈范围 */
        vPortStoreTaskMPUSettings( &( pxNewTCB->xMPUSettings ), xRegions, 
            pxNewTCB->pxStack, ulStackDepth );
    }
    #else  /* 非MPU配置处理 */
    {
        ( void ) xRegions;  // 避免未使用参数警告
    }
    #endif

    /* 线程本地存储（TLS）初始化 */
    #if( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
    {
        for( x = 0; x < ( UBaseType_t ) configNUM_THREAD_LOCAL_STORAGE_POINTERS; x++ )
        {
            pxNewTCB->pvThreadLocalStoragePointers[ x ] = NULL;  // 全部初始化为NULL
        }
    }
    #endif

    /* 任务通知系统初始化 */
    #if ( configUSE_TASK_NOTIFICATIONS == 1 )
    {
        pxNewTCB->ulNotifiedValue = 0;                 // 通知值清零
        pxNewTCB->ucNotifyState = taskNOT_WAITING_NOTIFICATION;  // 初始为非等待状态
    }
    #endif

    /* Newlib可重入结构初始化 */
    #if ( configUSE_NEWLIB_REENTRANT == 1 )
    {
        _REENT_INIT_PTR( ( &( pxNewTCB->xNewLib_reent ) ) );  // 初始化Newlib重入结构
    }
    #endif

    /* 延时中断标志初始化 */
    #if( INCLUDE_xTaskAbortDelay == 1 )
    {
        pxNewTCB->ucDelayAborted = pdFALSE;  // 初始化为未中断状态
    }
    #endif

    /* 硬件上下文堆栈初始化（核心操作） */
    #if( portUSING_MPU_WRAPPERS == 1 )
    {
        /* 带MPU参数的堆栈初始化：
           xRunPrivileged参数决定任务执行模式 */
        pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, 
            pxTaskCode, pvParameters, xRunPrivileged );
    }
    #else  /* 标准堆栈初始化 */
    {
        pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, 
            pxTaskCode, pvParameters );  // 不含MPU参数版本
    }
    #endif

    /* 任务句柄回传处理 */
    if( ( void * ) pxCreatedTask != NULL )  // 检查是否需求返回句柄
    {
        /* 匿名转换返回句柄：
           实际返回的是TCB指针，但通过TaskHandle_t类型抽象 */
        *pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();  // 测试覆盖标记（空分支）
    }
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
// 函数名称: prvAddNewTaskToReadyList
// 功能描述:
//   将新创建的任务加入就绪列表的核心操作函数，主要完成：
//   1. 临界区保护下的任务列表更新
//   2. 当前运行任务的动态切换决策
//   3. 任务追踪编号分配（TRACE模式）
//   4. 就绪列表的实际添加操作
//   5. 调度器状态感知的任务切换触发
// 参数说明:
//   pxNewTCB - 新创建的任务控制块指针
// 核心逻辑:
//   - 在临界区保护下完成所有状态变更操作
//   - 根据调度器运行状态智能切换当前任务
//   - 同时处理首次任务创建的特殊初始化
/*-----------------------------------------------------------*/
static void prvAddNewTaskToReadyList( TCB_t *pxNewTCB )
{
	/* 临界区入口（关中断）：
	   保护任务列表操作免受中断干扰 */
	taskENTER_CRITICAL();  // 进入临界区，保护共享资源
	{
		/* 全局任务计数更新：
		   每次创建新任务时增加系统任务总数 */
		uxCurrentNumberOfTasks++;  // 原子操作递增任务计数器

		/* 当前任务空状态处理 */
		if( pxCurrentTCB == NULL )  // 系统无运行中任务（初始状态）
		{
			/* 设置新任务为当前运行任务：
			   适用于系统初始化阶段或全部任务挂起的情况 */
			pxCurrentTCB = pxNewTCB;  // 直接接管新任务控制块

			/* 首次任务创建处理：
			   当系统任务数从0->1时执行基础初始化 */
			if( uxCurrentNumberOfTasks == ( UBaseType_t ) 1 )  // 检测首个任务
			{
				/* 核心数据结构初始化：
				   初始化就绪/阻塞/挂起等任务列表
				   注：该初始化仅执行一次 */
				prvInitialiseTaskLists();  // 创建基础任务链表结构
			}
			else  // 非首个任务但当前无活动任务（其他任务被挂起）
			{
				mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（空分支）
			}
		}
		else  // 系统中已有运行任务
		{
			/* 调度器未启动时的处理：
			   在vTaskStartScheduler调用前的任务创建阶段 */
			if( xSchedulerRunning == pdFALSE )  // 调度器尚未启动
			{
				/* 动态当前任务选择：
				   选择最高优先级任务作为当前任务 */
				if( pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority ) // 新任务优先级更高
				{
					pxCurrentTCB = pxNewTCB;  // 更新当前任务为新任务
				}
				else  // 新任务优先级低于当前任务
				{
					mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
				}
			}
			else  // 调度器已正常运行
			{
				mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
			}
		}

		/* 任务序列号分配：
		   每个任务获得唯一递增ID，用于追踪调试 */
		uxTaskNumber++;  // 全局任务序列号递增

		#if ( configUSE_TRACE_FACILITY == 1 )  // 追踪功能启用时
		{
			/* 追踪标识注入：
			   将递增的序列号存入TCB，供调试工具使用 */
			pxNewTCB->uxTCBNumber = uxTaskNumber;  // 设置任务追踪编号
		}
		#endif /* 追踪功能配置 */

		/* 调试钩子函数调用：
		   通知追踪系统新任务创建事件 */
		traceTASK_CREATE( pxNewTCB );  // 追踪系统回调接口

		/* 就绪列表实际添加操作：
		   根据任务优先级将TCB插入对应就绪列表 */
		prvAddTaskToReadyList( pxNewTCB );  // 核心链表操作函数

		/* 硬件相关初始化：
		   架构特定的TCB补充设置（如协处理器状态等） */
		portSETUP_TCB( pxNewTCB );  // 端口级任务控制块配置
	}
	taskEXIT_CRITICAL();  // 退出临界区（恢复中断）

	/* 调度器运行状态处理：
	   当任务创建发生在调度器启动后的情况 */
	if( xSchedulerRunning != pdFALSE )  // 调度器已运行
	{
		/* 抢占式调度检查：
		   新任务优先级高于当前运行任务时触发任务切换 */
		if( pxCurrentTCB->uxPriority < pxNewTCB->uxPriority ) // 优先级比较
		{
			/* 主动让出CPU：
			   若使能抢占调度，立即触发上下文切换 */
			taskYIELD_IF_USING_PREEMPTION();  // 架构相关的任务切换指令
		}
		else  // 新任务优先级不高于当前任务
		{
			mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
		}
	}
	else  // 调度器尚未启动
	{
		mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
	}
}

/*-----------------------------------------------------------*/
#if ( INCLUDE_vTaskDelete == 1 )  // 任务删除功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: vTaskDelete
// 功能描述:
//   删除指定任务的系统级操作函数，完成：
//   1. 任务资源回收链式操作
//   2. 动态任务列表维护
//   3. 自删除任务的特殊处理
//   4. 调度器协同工作
// 参数说明:
//   xTaskToDelete - 要删除的任务句柄（NULL表示删除自身）
// 异常处理:
//   - 自删除任务需要立即触发上下文切换
//   - 确保关键数据结构完整性
/*-----------------------------------------------------------*/
	void vTaskDelete( TaskHandle_t xTaskToDelete )
	{
	TCB_t *pxTCB;  // 任务控制块指针

		taskENTER_CRITICAL();  // 进入临界区（保护任务列表操作）
		{
			/* 任务句柄转换：
			   NULL参数时自动获取当前任务TCB */
			pxTCB = prvGetTCBFromHandle( xTaskToDelete );  // 句柄转TCB核心方法

			/* 从就绪列表移除：
			   操作结果判断是否最后一个该优先级任务 */
			if( uxListRemove( &( pxTCB->xStateListItem ) ) == ( UBaseType_t ) 0 ) 
			{
				/* 优先级位图更新：
				   当该优先级无就绪任务时清除对应bit */
				taskRESET_READY_PRIORITY( pxTCB->uxPriority );  // 更新优先级位图
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（非空分支）
			}

			/* 事件等待状态检测：
			   检查任务是否在事件等待列表 */
			if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
			{
				/* 强制移出事件列表：
				   确保任务不处于任何事件等待状态 */
				( void ) uxListRemove( &( pxTCB->xEventListItem ) );  // 无返回值检查
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
			}

			/* 任务序列号递增策略：
			   辅助调试器检测任务列表变更需求 */
			uxTaskNumber++;  // 使所有调试器知道需要刷新任务列表

			/* 自删除判断：
			   检测是否为当前运行任务自删除 */
			if( pxTCB == pxCurrentTCB )
			{
				/* 延迟删除机制：
				   将TCB插入终止列表由空闲任务回收 */
				vListInsertEnd( &xTasksWaitingTermination, &( pxTCB->xStateListItem ) );

				/* 清理计数器递增：
				   通知空闲任务有资源待回收 */
				++uxDeletedTasksWaitingCleanUp;  // 原子操作递增

				/* 预删除钩子函数：
				   执行平台特定清理操作（如Windows仿真器线程回收） */
				portPRE_TASK_DELETE_HOOK( pxTCB, &xYieldPending );  // 注意：某些端口此宏不返回
			}
			else
			{
				/* 直接删除流程：
				   递减当前任务计数器并立即释放资源 */
				--uxCurrentNumberOfTasks;  // 原子操作递减
				prvDeleteTCB( pxTCB );     // 释放TCB和堆栈内存

				/* 时间管理修正：
				   重新计算下一个任务唤醒时间 */
				prvResetNextTaskUnblockTime();  // 防止已删任务影响唤醒队列
			}

			/* 追踪系统通知：
			   记录任务删除事件 */
			traceTASK_DELETE( pxTCB );  // 追踪系统回调接口
		}
		taskEXIT_CRITICAL();  // 退出临界区

		/* 调度器协同处理：
		   仅在调度器运行时需要特殊处理 */
		if( xSchedulerRunning != pdFALSE )  // 防止在调度器未启动时操作
		{
			if( pxTCB == pxCurrentTCB )  // 确认是否自删除情景
			{
				/* 内核状态验证：
				   确保删除操作不在挂起的调度器中执行 */
				configASSERT( uxSchedulerSuspended == 0 );  // 若触发断言则系统状态错误

				/* 强制上下文切换：
				   立即让出CPU控制权 */
				portYIELD_WITHIN_API();  // 产生PendSV中断触发任务切换
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
			}
		}
	}

#endif /* INCLUDE_vTaskDelete 功能编译结束 */

/*-----------------------------------------------------------*/
#if ( INCLUDE_vTaskDelayUntil == 1 )  // 周期延迟功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: vTaskDelayUntil
// 功能描述:
//   实现固定周期的精准任务延迟，确保任务以严格周期执行
// 关键特性:
//   - 自动补偿任务执行时间波动
//   - 处理tick计数器溢出场景
//   - 维持稳定的时间间隔
// 参数说明:
//   pxPreviousWakeTime - 指向上次唤醒时间的指针（需持久化存储）
//   xTimeIncrement      - 期望的任务执行周期（单位：tick）
// 特殊说明:
//   必须与vTaskDelay区分使用，适用于周期性定时任务
/*-----------------------------------------------------------*/
	void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement )
	{
	TickType_t xTimeToWake;            // 计算得出的下次唤醒时间点
	BaseType_t xAlreadyYielded,        // 标记调度恢复是否已触发切换
              xShouldDelay = pdFALSE; // 延迟必要性标志

		/* 前置校验（开发阶段捕获错误） */
		configASSERT( pxPreviousWakeTime );               // 确保时间指针有效
		configASSERT( ( xTimeIncrement > 0U ) );          // 周期必须大于0
		configASSERT( uxSchedulerSuspended == 0 );        // 禁止在调度挂起时调用

		/* 调度挂起（保护时间计算原子性） */
		vTaskSuspendAll();  // 挂起任务调度
		{
			/* 时间基准固化：
			   获取当前tick快照防止后续计算中值变化 */
			const TickType_t xConstTickCount = xTickCount;  // 固化当前时间基准

			/* 绝对时间计算：
			   基于上次唤醒时间+周期得到理论唤醒点 */
			xTimeToWake = *pxPreviousWakeTime + xTimeIncrement;  // 绝对时间点计算

			/* 时间溢出场景处理：
			   检测到tick计数器发生溢出时的特殊处理 */
			if( xConstTickCount < *pxPreviousWakeTime )  // 当前tick小于上次时间（发生溢出）
			{
				/* 溢出后有效延迟条件：
				   唤醒时间必须同时满足：
				   1. 已溢出（xTimeToWake < *pxPreviousWakeTime）
				   2. 大于当前tick（xTimeToWake > xConstTickCount） */
				if( ( xTimeToWake < *pxPreviousWakeTime ) && ( xTimeToWake > xConstTickCount ) )
				{
					xShouldDelay = pdTRUE;  // 标记需要延迟
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（非触发分支）
				}
			}
			else  // 常规场景（无tick溢出）
			{
				/* 常规有效延迟条件：
				   唤醒时间溢出 或 大于当前tick */
				if( ( xTimeToWake < *pxPreviousWakeTime ) || ( xTimeToWake > xConstTickCount ) )
				{
					xShouldDelay = pdTRUE;  // 标记需要延迟
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
				}
			}

			/* 持久化存储更新：
			   记录下次唤醒时间点（为下个周期准备） */
			*pxPreviousWakeTime = xTimeToWake;  // 更新外部存储的时间基准

			/* 延迟执行决策 */
			if( xShouldDelay != pdFALSE )  // 需要执行延迟
			{
				traceTASK_DELAY_UNTIL( xTimeToWake );  // 追踪系统记录事件

				/* 转换为相对延迟时间：
				   将绝对时间转换为需要等待的tick数 */
				prvAddCurrentTaskToDelayedList( 
                    xTimeToWake - xConstTickCount,  // 计算相对等待时间
                    pdFALSE );                      // 非无限等待模式
			}
			else  // 无需延迟（唤醒时间已过）
			{
				mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
			}
		}
		/* 调度恢复及切换决策：
		   获取恢复调度时是否已触发切换 */
		xAlreadyYielded = xTaskResumeAll();  // 恢复调度并获取切换状态

		/* 二次切换检查：
		   确保延迟操作后正确让出CPU */
		if( xAlreadyYielded == pdFALSE )  // 若调度恢复未触发切换
		{
			portYIELD_WITHIN_API();  // 主动触发上下文切换
		}
		else  // 已由调度恢复过程处理切换
		{
			mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
		}
	}

#endif /* INCLUDE_vTaskDelayUntil 周期延迟功能结束 */

/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelay == 1 )  // 相对延迟功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: vTaskDelay
// 功能描述:
//   实现相对时间的任务延迟，使任务进入阻塞状态指定tick数
// 典型应用:
//   - 非周期性的简单延时
//   - 等待外部事件时的超时机制
// 参数说明:
//   xTicksToDelay - 要延迟的系统tick数
// 注意:
//   传入0将强制触发一次任务切换
/*-----------------------------------------------------------*/
	void vTaskDelay( const TickType_t xTicksToDelay )
	{
	BaseType_t xAlreadyYielded = pdFALSE;  // 调度恢复触发标记

		/* 延迟有效性检查 */
		if( xTicksToDelay > ( TickType_t ) 0U )  // 有效延迟请求
		{
			configASSERT( uxSchedulerSuspended == 0 );  // 禁止在调度挂起时调用
			vTaskSuspendAll();  // 挂起任务调度
			{
				traceTASK_DELAY();  // 追踪系统记录事件

				/* 将当前任务加入延迟列表：
				   第二个参数pdFALSE表示非无限等待 */
				prvAddCurrentTaskToDelayedList( xTicksToDelay, pdFALSE );
			}
			xAlreadyYielded = xTaskResumeAll();  // 恢复调度并获取切换状态
		}
		else  // 零延迟请求（特殊处理）
		{
			mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
		}

		/* 强制上下文切换（如果需要） */
		if( xAlreadyYielded == pdFALSE )  // 未触发切换时
		{
			portYIELD_WITHIN_API();  // 架构相关的切换指令
		}
		else  // 已由调度恢复处理
		{
			mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
		}
	}

#endif /* INCLUDE_vTaskDelay 相对延迟功能结束 */

/*-----------------------------------------------------------*/
#if( ( INCLUDE_eTaskGetState == 1 ) || ( configUSE_TRACE_FACILITY == 1 ) )  // 任务状态查询功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: eTaskGetState
// 功能描述:
//   获取指定任务的状态信息，支持以下状态判断：
//   - 运行中（eRunning）
//   - 阻塞（eBlocked）
//   - 挂起（eSuspended）
//   - 已删除（eDeleted）
//   - 就绪（eReady）
// 参数说明:
//   xTask - 要查询的任务句柄（NULL表示查询自身）
// 返回值:
//   eTaskState枚举类型，反映任务当前精确状态
// 特殊说明:
//   需配合INCLUDE_vTaskSuspend/Delete等配置选项使用
/*-----------------------------------------------------------*/
	eTaskState eTaskGetState( TaskHandle_t xTask )
	{
	eTaskState eReturn;            // 状态返回值
	List_t *pxStateList;           // 任务所在状态列表指针
	const TCB_t * const pxTCB = ( TCB_t * ) xTask;  // 转换为TCB结构

		configASSERT( pxTCB );  // 确保传入的TCB指针有效

		if( pxTCB == pxCurrentTCB )  // 查询当前运行任务状态
		{
			/* 当前任务自查询：
			   无论列表状态如何，直接返回运行状态 */
			eReturn = eRunning;  // 最高优先级状态标识
		}
		else  // 查询其他任务状态
		{
			taskENTER_CRITICAL();  // 进入临界区（保护列表访问）
			{
				/* 获取任务所在主状态列表：
				   通过状态列表项反查容器列表 */
				pxStateList = ( List_t * ) listLIST_ITEM_CONTAINER( &( pxTCB->xStateListItem ) );
			}
			taskEXIT_CRITICAL();   // 退出临界区

			/* 状态列表类型判断 */
			if( ( pxStateList == pxDelayedTaskList ) || ( pxStateList == pxOverflowDelayedTaskList ) )
			{
				/* 阻塞状态判定：
				   位于延时列表（常规/溢出） */
				eReturn = eBlocked;  // 包含有限时间阻塞状态
			}

			#if ( INCLUDE_vTaskSuspend == 1 )  // 挂起功能启用时
				else if( pxStateList == &xSuspendedTaskList )  // 位于挂起列表
				{
					/* 挂起状态细分：
					   检查是否同时等待事件 */
					if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL )
					{
						eReturn = eSuspended;  // 纯挂起状态
					}
					else
					{
						eReturn = eBlocked;    // 无限期等待事件（实质阻塞）
					}
				}
			#endif

			#if ( INCLUDE_vTaskDelete == 1 )  // 删除功能启用时
				else if( ( pxStateList == &xTasksWaitingTermination ) || ( pxStateList == NULL ) )
				{
					/* 已删除状态判定：
					   位于终止列表 或 未在任何列表（异常状态） */
					eReturn = eDeleted;  // 等待资源回收状态
				}
			#endif

			else /* 其他情况默认为就绪状态 */
			{
				/* 就绪状态判定：
				   位于就绪列表或转换中的pendingReady状态 */
				eReturn = eReady;  // 包含等待调度的就绪状态
			}
		}

		return eReturn;  // 返回最终判定状态
	} /*lint !e818 允许xTask非常量指针（因TaskHandle_t类型定义限制） */

#endif /* INCLUDE_eTaskGetState 状态查询功能结束 */

/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskPriorityGet == 1 )  // 优先级查询功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: uxTaskPriorityGet
// 功能描述:
//   获取指定任务的当前优先级（实时优先级，考虑优先级继承）
// 参数说明:
//   xTask - 要查询的任务句柄（NULL表示查询自身）
// 返回值:
//   当前有效优先级数值（0 ~ configMAX_PRIORITIES-1）
// 注意:
//   在临界区内操作确保优先级数值的原子性
/*-----------------------------------------------------------*/
	UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask )
	{
	TCB_t *pxTCB;         // 任务控制块指针
	UBaseType_t uxReturn; // 优先级返回值

		taskENTER_CRITICAL();  // 进入临界区（保护TCB访问）
		{
			/* 任务句柄转换：
			   NULL参数时自动获取当前任务TCB */
			pxTCB = prvGetTCBFromHandle( xTask );  // 统一句柄转换方法
			
			/* 获取实时优先级：
			   考虑互斥量导致的优先级继承 */
			uxReturn = pxTCB->uxPriority;  // 直接读取TCB存储的优先级
		}
		taskEXIT_CRITICAL();   // 退出临界区

		return uxReturn;  // 返回优先级数值
	}

#endif /* INCLUDE_uxTaskPriorityGet 优先级查询功能结束 */

/*-----------------------------------------------------------*/
#if ( INCLUDE_uxTaskPriorityGet == 1 )  // 中断安全优先级查询功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: uxTaskPriorityGetFromISR
// 功能描述:
//   从中断上下文中安全获取任务优先级（实时优先级）
// 设计要点:
//   - 使用中断屏蔽而非任务临界区保护
//   - 验证中断优先级合法性
//   - 保证中断响应实时性
// 参数说明:
//   xTask - 要查询的任务句柄（NULL表示当前中断上下文任务）
// 注意:
//   必须使用FromISR结尾的API，且中断优先级需符合系统最大调用优先级
/*-----------------------------------------------------------*/
	UBaseType_t uxTaskPriorityGetFromISR( TaskHandle_t xTask )
	{
	TCB_t *pxTCB;
	UBaseType_t uxReturn, uxSavedInterruptState;

		/* 中断合法性断言：
		   验证当前中断优先级是否允许调用API 
		   防止在不可屏蔽中断中误操作 */
		portASSERT_IF_INTERRUPT_PRIORITY_INVALID();  // 触发断言若优先级超标

		/* 中断屏蔽操作：
		   等效关中断，但保留优先级信息用于恢复 */
		uxSavedInterruptState = portSET_INTERRUPT_MASK_FROM_ISR();  // 架构特定的中断屏蔽
		{
			/* 安全句柄转换：
			   处理NULL参数为当前任务TCB */
			pxTCB = prvGetTCBFromHandle( xTask );  // 与任务上下文相同转换逻辑
			
			/* 实时优先级读取：
			   直接获取可能被继承机制修改的优先级 */
			uxReturn = pxTCB->uxPriority;  // 原子操作读取
		}
		/* 中断状态恢复：
		   恢复原始中断屏蔽状态 */
		portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptState );  // 恢复中断优先级

		return uxReturn;  // 返回中断安全环境获取的优先级
	}

#endif /* INCLUDE_uxTaskPriorityGet */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskPrioritySet == 1 )  // 优先级设置功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: vTaskPrioritySet
// 功能描述:
//   动态修改任务优先级，处理以下场景：
//   - 优先级继承机制（互斥量场景）
//   - 就绪列表重排序
//   - 抢占式调度触发
// 参数说明:
//   xTask         - 目标任务句柄（NULL表示当前任务）
//   uxNewPriority - 新优先级（0 ~ configMAX_PRIORITIES-1）
// 核心操作:
//   1. 优先级有效性校验
//   2. 临界区保护下的TCB修改
//   3. 就绪列表动态调整
//   4. 抢占决策
/*-----------------------------------------------------------*/
	void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority )
	{
	TCB_t *pxTCB;
	UBaseType_t uxCurrentBasePriority, uxPriorityUsedOnEntry;
	BaseType_t xYieldRequired = pdFALSE;  // 上下文切换标记

		/* 新优先级有效性断言 */
		configASSERT( ( uxNewPriority < configMAX_PRIORITIES ) );  // 开发阶段捕获错误

		/* 新优先级标准化处理 */
		if( uxNewPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
		{
			uxNewPriority = ( UBaseType_t ) configMAX_PRIORITIES - 1U;  // 强制限制到合法范围
		}

		taskENTER_CRITICAL();  // 进入任务级临界区
		{
			/* 获取目标TCB：
			   处理NULL参数为当前任务 */
			pxTCB = prvGetTCBFromHandle( xTask );  // 统一句柄转换

			traceTASK_PRIORITY_SET( pxTCB, uxNewPriority );  // 追踪系统回调

			/* 获取当前基准优先级：
			   区分是否启用互斥量优先级继承 */
			#if ( configUSE_MUTEXES == 1 )
				uxCurrentBasePriority = pxTCB->uxBasePriority;  // 获取原始优先级
			#else
				uxCurrentBasePriority = pxTCB->uxPriority;      // 非互斥量场景直接取当前优先级
			#endif

			/* 优先级变更决策 */
			if( uxCurrentBasePriority != uxNewPriority )  // 过滤无变化情况
			{
				/* 优先级提升场景处理 */
				if( uxNewPriority > uxCurrentBasePriority )
				{
					/* 修改其他任务优先级：
					   当新优先级高于当前运行任务时需触发切换 */
					if( pxTCB != pxCurrentTCB )
					{
						if( uxNewPriority >= pxCurrentTCB->uxPriority )
						{
							xYieldRequired = pdTRUE;  // 标记需要上下文切换
						}
					}
				}
				/* 当前任务降级处理 */
				else if( pxTCB == pxCurrentTCB )
				{
					xYieldRequired = pdTRUE;  // 当前任务降级可能让出CPU
				}

				/* 记录原始优先级：
				   用于后续列表操作 */
				uxPriorityUsedOnEntry = pxTCB->uxPriority;

				/* 更新TCB优先级字段 */
				#if ( configUSE_MUTEXES == 1 )
				{
					/* 互斥量优先级继承处理：
					   仅当未处于继承状态时修改当前优先级 */
					if( pxTCB->uxBasePriority == pxTCB->uxPriority )
					{
						pxTCB->uxPriority = uxNewPriority;  // 更新实时优先级
					}
					pxTCB->uxBasePriority = uxNewPriority;  // 总是更新基准优先级
				}
				#else
				{
					pxTCB->uxPriority = uxNewPriority;      // 非互斥量场景直接更新
				}
				#endif

				/* 事件列表项更新：
				   只有当事件项未使用时更新其排序值 */
				if( ( listGET_LIST_ITEM_VALUE( &pxTCB->xEventListItem ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == 0UL )
				{
					/* 计算新的事件项值：
					   值越大优先级越低，用于事件列表排序 */
					listSET_LIST_ITEM_VALUE( &pxTCB->xEventListItem, 
                        ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) uxNewPriority );
				}

				/* 就绪列表重排操作 */
				if( listIS_CONTAINED_WITHIN( &pxReadyTasksLists[ uxPriorityUsedOnEntry ], &pxTCB->xStateListItem ) )
				{
					/* 从旧优先级列表移除 */
					if( uxListRemove( &pxTCB->xStateListItem ) == 0 )
					{
						/* 更新就绪优先级位图：
						   当该优先级无任务时清除对应bit */
						portRESET_READY_PRIORITY( uxPriorityUsedOnEntry, uxTopReadyPriority );
					}
					/* 插入新优先级就绪列表 */
					prvAddTaskToReadyList( pxTCB );  // 自动更新uxTopReadyPriority
				}

				/* 触发上下文切换 */
				if( xYieldRequired )
				{
					taskYIELD_IF_USING_PREEMPTION();  // 触发PendSV异常
				}
			}
		}
		taskEXIT_CRITICAL();  // 退出临界区
	}

#endif /* INCLUDE_vTaskPrioritySet */

/*-----------------------------------------------------------*/
#if ( INCLUDE_vTaskSuspend == 1 )  // 任务挂起功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: vTaskSuspend
// 功能描述:
//   将指定任务置入挂起状态，使其脱离调度系统
// 核心操作:
//   1. 从所有活动列表移除任务
//   2. 加入挂起列表
//   3. 处理当前任务自挂起场景
// 参数说明:
//   xTaskToSuspend - 要挂起的任务句柄（NULL表示挂起自身）
// 状态转换:
//   Ready/Blocked → Suspended
/*-----------------------------------------------------------*/
	void vTaskSuspend( TaskHandle_t xTaskToSuspend )
	{
	TCB_t *pxTCB;

		taskENTER_CRITICAL();  // 进入临界区（保护列表操作）
		{
			/* 任务句柄转换：
			   NULL参数时自动获取当前任务TCB */
			pxTCB = prvGetTCBFromHandle( xTaskToSuspend );

			traceTASK_SUSPEND( pxTCB );  // 追踪系统回调

			/* 从活动列表移除：
			   包括就绪列表或阻塞列表 */
			if( uxListRemove( &( pxTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
			{
				/* 更新优先级位图：
				   当该优先级无其他任务时清除对应bit */
				taskRESET_READY_PRIORITY( pxTCB->uxPriority );
			}

			/* 事件等待状态处理：
			   如果任务正在等待事件（如队列、信号量） */
			if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
			{
				( void ) uxListRemove( &( pxTCB->xEventListItem ) );  // 强制移出事件列表
			}

			/* 加入全局挂起列表：
			   使用xSuspendedTaskList管理所有挂起任务 */
			vListInsertEnd( &xSuspendedTaskList, &( pxTCB->xStateListItem ) );
		}
		taskEXIT_CRITICAL();  // 退出临界区

		/* 调度器运行状态处理 */
		if( xSchedulerRunning != pdFALSE )
		{
			/* 更新最近唤醒时间：
			   防止挂起任务影响阻塞任务唤醒计时 */
			taskENTER_CRITICAL();
			{
				prvResetNextTaskUnblockTime();  // 重新扫描最近唤醒时间
			}
			taskEXIT_CRITICAL();
		}

		/* 自挂起场景处理 */
		if( pxTCB == pxCurrentTCB )  // 当前任务自挂起
		{
			if( xSchedulerRunning != pdFALSE )  // 调度器已启动
			{
				/* 内核状态验证：
				   确保不在调度器挂起状态中自挂起 */
				configASSERT( uxSchedulerSuspended == 0 );
				/* 强制上下文切换：
				   立即让出CPU控制权 */
				portYIELD_WITHIN_API();  // 触发PendSV异常
			}
			else  // 调度器未启动
			{
				/* 特殊场景处理：
				   当所有任务都被挂起时重置当前任务指针 */
				if( listCURRENT_LIST_LENGTH( &xSuspendedTaskList ) == uxCurrentNumberOfTasks )
				{
					pxCurrentTCB = NULL;  // 清空当前任务指针
				}
				else
				{
					/* 手动触发任务切换：
					   选择新的当前任务 */
					vTaskSwitchContext();  // 遍历就绪列表找最高优先级任务
				}
			}
		}
	}

#endif /* INCLUDE_vTaskSuspend */

/*-----------------------------------------------------------*/
#if ( INCLUDE_vTaskSuspend == 1 )  // 挂起功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: prvTaskIsTaskSuspended
// 功能描述:
//   内部校验函数，确认任务是否处于有效挂起状态
// 参数说明:
//   xTask - 要校验的任务句柄
// 返回值:
//   pdTRUE: 任务处于纯挂起状态
//   pdFALSE: 任务处于其他状态或无效挂起
// 注意:
//   必须在临界区内调用，访问xPendingReadyList需保护   

/*-----------------------------------------------------------*/
	static BaseType_t prvTaskIsTaskSuspended( const TaskHandle_t xTask )
	{
	BaseType_t xReturn = pdFALSE;
	const TCB_t * const pxTCB = ( TCB_t * ) xTask;

		/* 校验逻辑约束：
		   不能检查当前运行任务的状态 */
		configASSERT( xTask );  // 禁止NULL参数

		/* 主状态列表检查：
		   确认任务在挂起列表 */
		if( listIS_CONTAINED_WITHIN( &xSuspendedTaskList, &( pxTCB->xStateListItem ) ) != pdFALSE )
		{
			/* 二次校验：
			   确认任务未被中断服务程序恢复 */
			if( listIS_CONTAINED_WITHIN( &xPendingReadyList, &( pxTCB->xEventListItem ) ) == pdFALSE )
			{
				/* 最终状态确认：
				   检查事件项是否游离（非等待事件状态） */
				if( listIS_CONTAINED_WITHIN( NULL, &( pxTCB->xEventListItem ) ) != pdFALSE )
				{
					xReturn = pdTRUE;  // 确认纯挂起状态
			//	（只有1. 在禁闭室；2. 没在“待恢复”过渡室；3. 没在等任何事件，三重条件都满足，函数才返回 pdTRUE，承认它是真正的纯粹挂起状态。）
				}
			}
		}

		return xReturn;
	}

#endif /* INCLUDE_vTaskSuspend */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )  // 任务恢复功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: vTaskResume
// 功能描述:
//   唤醒处于挂起状态的任务，使其重新参与调度
// 核心操作:
//   1. 从挂起列表移除
//   2. 重新加入就绪列表
//   3. 触发优先级调度
// 参数说明:
//   xTaskToResume - 要恢复的任务句柄（禁止NULL和当前任务）
// 状态转换:
//   Suspended → Ready
/*-----------------------------------------------------------*/
	void vTaskResume( TaskHandle_t xTaskToResume )
	{
	TCB_t * const pxTCB = ( TCB_t * ) xTaskToResume;

		/* 参数有效性验证：
		   禁止恢复当前任务和NULL指针 */
		configASSERT( xTaskToResume );
		if( ( pxTCB != NULL ) && ( pxTCB != pxCurrentTCB ) )
		{
			taskENTER_CRITICAL();  // 进入临界区
			{
				if( prvTaskIsTaskSuspended( pxTCB ) != pdFALSE )  // 校验有效挂起
				{
					traceTASK_RESUME( pxTCB );  // 追踪系统回调

					/* 从挂起列表移除：
					   使用状态列表项进行操作 */
					( void ) uxListRemove(  &( pxTCB->xStateListItem ) );

					/* 加入就绪列表：
					   自动更新优先级位图 */
					prvAddTaskToReadyList( pxTCB );  // 核心列表操作

					/* 抢占决策：
					   恢复的任务优先级不低于当前任务 */
					if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )
					{
						/* 触发上下文切换：
						   可能不会立即切换但确保列表正确 */
						taskYIELD_IF_USING_PREEMPTION();  // 架构相关的切换指令
					}
				}
			}
			taskEXIT_CRITICAL();  // 退出临界区
		}
	}

#endif /* INCLUDE_vTaskSuspend */


/*-----------------------------------------------------------*/
#if ( ( INCLUDE_xTaskResumeFromISR == 1 ) && ( INCLUDE_vTaskSuspend == 1 ) )  // 中断安全恢复功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: xTaskResumeFromISR
// 功能描述:
//   从中断上下文中安全恢复被挂起的任务，返回是否需要上下文切换
// 设计要点:
//   - 满足中断服务程序对API的特殊要求
//   - 处理调度器挂起时的延迟恢复
//   - 返回调度请求标志供后续处理
// 参数说明:
//   xTaskToResume - 要恢复的任务句柄（禁止NULL）
// 返回值:
//   pdTRUE: 需要上下文切换
//   pdFALSE: 无需立即切换
// 注意:
//   必须在中断优先级合法范围内调用（通过portASSERT验证）
/*-----------------------------------------------------------*/
	BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume )
	{
	BaseType_t xYieldRequired = pdFALSE;       // 切换请求标记
	TCB_t * const pxTCB = ( TCB_t * ) xTaskToResume;  // 类型转换
	UBaseType_t uxSavedInterruptStatus;       // 中断状态保存

		configASSERT( xTaskToResume );  // 确保参数有效性

		/* 中断合法性断言：
		   验证当前中断优先级是否允许调用API 
		   该宏展开后可能包含BASEPRI寄存器操作 */
		portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

		/* 中断屏蔽操作：
		   保存当前中断状态并提升屏蔽等级 */
		uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
		{
			if( prvTaskIsTaskSuspended( pxTCB ) != pdFALSE )  // 验证有效挂起状态
			{
				traceTASK_RESUME_FROM_ISR( pxTCB );  // 追踪系统回调

				/* 调度器状态判断：
				   检查是否处于调度挂起状态 */
				if( uxSchedulerSuspended == pdFALSE )  // 调度器正常运行
				{
					/* 抢占决策：
					   比较被恢复任务与当前任务优先级 */
					if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )
					{
						xYieldRequired = pdTRUE;  // 标记需要上下文切换
					}

					/* 立即恢复操作：
					   从挂起列表移除并加入就绪列表 */
					( void ) uxListRemove( &( pxTCB->xStateListItem ) );
					prvAddTaskToReadyList( pxTCB );  // 更新就绪列表和优先级位图
				}
				else  // 调度器被挂起
				{
					/* 延迟恢复机制：
					   将任务加入待处理就绪列表 
					   在xTaskResumeAll()中批量处理 */
					vListInsertEnd( &xPendingReadyList, &( pxTCB->xEventListItem ) );
				}
			}
		}
		/* 中断状态恢复：
		   恢复原始中断屏蔽等级 */
		portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

		return xYieldRequired;  // 返回切换请求标志
	}

#endif /* 中断安全恢复功能结束 */

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
// 函数名称: vTaskStartScheduler
// 功能描述:
//   启动RTOS内核调度器，完成以下核心操作：
//   1. 创建空闲任务（最低优先级）
//   2. 可选创建定时器服务任务
//   3. 初始化系统节拍定时器
//   4. 启动任务调度
// 注意事项:
//   - 通常不会返回，除非调用xTaskEndScheduler()
//   - 必须在硬件初始化完成后调用
//   - 会接管CPU控制权，后续代码仅在异常情况下执行
/*-----------------------------------------------------------*/
void vTaskStartScheduler( void )
{
BaseType_t xReturn;  // 操作结果状态

	/* 创建空闲任务（系统最低优先级任务） */
	#if( configSUPPORT_STATIC_ALLOCATION == 1 )  // 静态内存分配模式
	{
		StaticTask_t *pxIdleTaskTCBBuffer = NULL;   // 静态分配的TCB内存指针
		StackType_t *pxIdleTaskStackBuffer = NULL;  // 静态分配的堆栈内存指针
		uint32_t ulIdleTaskStackSize;               // 堆栈大小

		/* 获取用户提供的空闲任务内存区域 */
		vApplicationGetIdleTaskMemory( &pxIdleTaskTCBBuffer, 
                                      &pxIdleTaskStackBuffer, 
                                      &ulIdleTaskStackSize );

		/* 创建静态分配的空闲任务 */
		xIdleTaskHandle = xTaskCreateStatic(	
            prvIdleTask,                        // 任务函数
            "IDLE",                             // 任务名称
            ulIdleTaskStackSize,                // 堆栈深度
            ( void * ) NULL,                    // 任务参数
            ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), // 优先级+特权位
            pxIdleTaskStackBuffer,              // 堆栈内存指针
            pxIdleTaskTCBBuffer                 // TCB内存指针
        ); 

		/* 验证空闲任务创建结果 */
		if( xIdleTaskHandle != NULL )
		{
			xReturn = pdPASS;  // 创建成功
		}
		else
		{
			xReturn = pdFAIL;  // 内存分配失败
		}
	}
	#else  // 动态内存分配模式
	{
		/* 使用动态内存分配创建空闲任务 */
		xReturn = xTaskCreate(	
            prvIdleTask,                    // 任务函数
            "IDLE",                         // 任务名称
            configMINIMAL_STACK_SIZE,       // 默认最小堆栈
            ( void * ) NULL,                // 任务参数
            ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), // 优先级+特权位
            &xIdleTaskHandle                // 返回任务句柄
        );
	}
	#endif /* configSUPPORT_STATIC_ALLOCATION */

	/* 软件定时器任务创建（条件编译） */
	#if ( configUSE_TIMERS == 1 )
	{
		if( xReturn == pdPASS )  // 空闲任务创建成功时继续
		{
			xReturn = xTimerCreateTimerTask();  // 创建定时器服务任务
		}
	}
	#endif /* configUSE_TIMERS */

	/* 调度器启动准备 */
	if( xReturn == pdPASS )  // 所有前置任务创建成功
	{
		/* 关闭全局中断：
		   确保在初始化完成前不发生上下文切换 */
		portDISABLE_INTERRUPTS();

		#if ( configUSE_NEWLIB_REENTRANT == 1 )  // Newlib可重入支持
		{
			/* 切换Newlib的全局_impure_ptr指针：
			   指向当前任务的reent结构 */
			_impure_ptr = &( pxCurrentTCB->xNewLib_reent );
		}
		#endif 

		/* 初始化调度器核心变量 */
		xNextTaskUnblockTime = portMAX_DELAY;  // 初始化最近唤醒时间为最大值
		xSchedulerRunning = pdTRUE;           // 标记调度器为运行状态
		xTickCount = ( TickType_t ) 0U;       // 系统节拍计数器清零

		/* 运行时统计定时器配置：
		   需要用户实现portCONFIGURE_TIMER_FOR_RUN_TIME_STATS宏 */
		portCONFIGURE_TIMER_FOR_RUN_TIME_STATS();

		/* 启动硬件相关调度器组件：
		   - 初始化系统节拍定时器
		   - 执行架构特定的启动代码
		   - 该函数通常不会返回 */
		if( xPortStartScheduler() != pdFALSE )  // 启动成功（理论上不会进入）
		{
			/* 此处代码不应被执行，调度器运行后不会返回 */
		}
	}
	else  // 任务创建失败处理
	{
		/* 内存分配失败断言：
		   通常由于堆空间不足导致 */
		configASSERT( xReturn != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY );
	}

	/* 消除编译器警告（当INCLUDE_xTaskGetIdleTaskHandle=0时） */
	( void ) xIdleTaskHandle;  // 显式使用避免未使用变量警告
}
/*-----------------------------------------------------------*/
// 函数名称: vTaskEndScheduler
// 功能描述:
//   完全停止RTOS调度器，执行以下操作：
//   1. 关闭全局中断
//   2. 标记调度器为非运行状态
//   3. 调用硬件相关的清理函数
// 使用场景:
//   - 系统需要永久停止多任务调度
//   - 从RTOS模式切换回裸机模式
// 注意事项:
//   该函数不会自动恢复中断状态，需根据具体应用场景处理
/*-----------------------------------------------------------*/
void vTaskEndScheduler( void )
{
    /* 禁用全局中断：
       确保在清理过程中不会发生上下文切换 */
    portDISABLE_INTERRUPTS();
    
    /* 更新调度器状态标志：
       通知系统调度器已停止运行 */
    xSchedulerRunning = pdFALSE;
    
    /* 硬件相关清理操作：
       - 停止系统节拍定时器
       - 恢复原始中断向量表
       - 架构特定的清理工作 */
    vPortEndScheduler();
}

/*-----------------------------------------------------------*/
// 函数名称: vTaskSuspendAll
// 功能描述:
//   挂起任务调度器，暂停所有任务调度（可嵌套调用）
// 核心机制:
//   通过递增挂起计数器实现，需与xTaskResumeAll配对使用
// 注意事项:
//   - 不会关闭中断，仅暂停任务切换
//   - 允许在中断服务程序中调用
/*-----------------------------------------------------------*/
void vTaskSuspendAll( void )
{
    /* 原子操作递增挂起计数器：
       无需临界区保护，因为uxSchedulerSuspended是BaseType_t类型
       在32位架构上单条机器指令完成操作 */
    ++uxSchedulerSuspended;  // 支持多级挂起嵌套
}

/*-----------------------------------------------------------*/
#if ( configUSE_TICKLESS_IDLE != 0 )  // Tickless低功耗模式编译开关

// 函数名称: prvGetExpectedIdleTime
// 功能描述:
//   计算系统可进入低功耗模式的预期空闲时间
// 返回值:
//   可休眠的tick数（0表示无法进入低功耗）
// 算法逻辑:
//   1. 检测高优先级任务就绪状态
//   2. 检查同优先级任务存在情况
//   3. 计算下一个任务唤醒时间间隔
/*-----------------------------------------------------------*/
static TickType_t prvGetExpectedIdleTime( void )
{
TickType_t xReturn;
UBaseType_t uxHigherPriorityReadyTasks = pdFALSE;

    /* 检测是否存在高于空闲优先级的就绪任务 */
    #if( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 )  // 通用任务选择方式
    {
        if( uxTopReadyPriority > tskIDLE_PRIORITY )  // 就绪位图有更高优先级任务
        {
            uxHigherPriorityReadyTasks = pdTRUE;
        }
    }
    #else  // 端口优化任务选择（位图方式）
    {
        const UBaseType_t uxLeastSignificantBit = ( UBaseType_t ) 0x01;
        
        /* 当uxTopReadyPriority位图超过最低有效位时，
           表示存在高于空闲优先级的就绪任务 */
        if( uxTopReadyPriority > uxLeastSignificantBit )
        {
            uxHigherPriorityReadyTasks = pdTRUE;
        }
    }
    #endif

    /* 决策树计算可休眠时间 */
    if( pxCurrentTCB->uxPriority > tskIDLE_PRIORITY )  // 当前任务优先级高于空闲任务
    {
        xReturn = 0;  // 立即唤醒处理高优先级任务
    }
    else if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ tskIDLE_PRIORITY ] ) ) > 1 )  // 存在多个空闲优先级任务
    {
        xReturn = 0;  // 需进行时间片轮转，无法休眠
    }
    else if( uxHigherPriorityReadyTasks != pdFALSE )  // 存在更高优先级就绪任务（协作式调度场景）
    {
        xReturn = 0;  // 需立即处理任务
    }
    else  // 满足低功耗条件
    {
        /* 计算下一个任务唤醒时间间隔：
           最近解除阻塞时间 - 当前tick计数 */
        xReturn = xNextTaskUnblockTime - xTickCount;
    }

    return xReturn;  // 返回可休眠的tick数
}

#endif /* configUSE_TICKLESS_IDLE */

/*----------------------------------------------------------*/
/*-----------------------------------------------------------*/
// 函数名称: xTaskResumeAll
// 功能描述:
//   恢复被挂起的调度器，处理挂起期间积累的任务和时钟节拍
// 返回值:
//   pdTRUE: 已触发上下文切换
//   pdFALSE: 未触发切换
// 注意事项:
//   必须与vTaskSuspendAll()成对使用，支持嵌套调用
/*-----------------------------------------------------------*/
BaseType_t xTaskResumeAll( void )
{
TCB_t *pxTCB = NULL;                   // 临时任务控制块指针
BaseType_t xAlreadyYielded = pdFALSE;  // 是否已触发切换标志

    /* 校验调度器挂起状态有效性 */
    configASSERT( uxSchedulerSuspended );  // 确保之前调用过vTaskSuspendAll

    /* 临界区入口（关中断） */
    taskENTER_CRITICAL();
    {
        /* 递减调度器挂起计数器 */
        --uxSchedulerSuspended;

        /* 当所有挂起层级解除后执行恢复操作 */
        if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
        {
            /* 系统任务数有效性检查 */
            if( uxCurrentNumberOfTasks > ( UBaseType_t ) 0U )
            {
                /* 处理待处理就绪列表（由中断中恢复的任务） */
                while( listLIST_IS_EMPTY( &xPendingReadyList ) == pdFALSE )
                {
                    /* 获取列表首任务TCB */
                    pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( &xPendingReadyList );

                    /* 从待处理列表移除 */
                    ( void ) uxListRemove( &( pxTCB->xEventListItem ) );   // 事件列表项
                    ( void ) uxListRemove( &( pxTCB->xStateListItem ) );    // 状态列表项

                    /* 加入就绪列表 */
                    prvAddTaskToReadyList( pxTCB );  // 自动更新优先级位图

                    /* 抢占决策：如果恢复的任务优先级高于当前任务 */
                    if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )
                    {
                        xYieldPending = pdTRUE;  // 标记需要上下文切换
                    }
                }

                /* 更新下一个任务解除阻塞时间（低功耗模式关键） */
                if( pxTCB != NULL )
                {
                    prvResetNextTaskUnblockTime();  // 重新扫描最近唤醒时间
                }

                /* 处理挂起期间累积的tick中断 */
                UBaseType_t uxPendedCounts = uxPendedTicks;  // 获取非易失副本
                if( uxPendedCounts > 0U )
                {
                    do 
                    {
                        /* 模拟tick中断处理 */
                        if( xTaskIncrementTick() != pdFALSE )  // 返回是否需要切换
                        {
                            xYieldPending = pdTRUE;
                        }
                        --uxPendedCounts;
                    } while( uxPendedCounts > 0U );

                    uxPendedTicks = 0;  // 重置累积tick计数器
                }

                /* 触发上下文切换（如果需要） */
                if( xYieldPending != pdFALSE )
                {
                    #if( configUSE_PREEMPTION != 0 )
                    {
                        xAlreadyYielded = pdTRUE;  // 标记已触发切换
                    }
                    #endif
                    taskYIELD_IF_USING_PREEMPTION();  // 架构相关的切换指令
                }
            }
        }
    }
    taskEXIT_CRITICAL();  // 退出临界区

    return xAlreadyYielded;  // 返回切换状态
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
// 函数名称: xTaskGetTickCount
// 功能描述: 
//   获取当前系统节拍计数器的值（从启动开始的tick数）
// 注意事项:
//   - 在16位架构上需要临界区保护
//   - 保证原子性读取32位变量
/*-----------------------------------------------------------*/
TickType_t xTaskGetTickCount( void )
{
TickType_t xTicks;

    /* 临界区保护（针对16位处理器）：
       确保在读取32位变量时的原子性 */
    portTICK_TYPE_ENTER_CRITICAL();  // 架构相关的关中断实现
    {
        xTicks = xTickCount;  // 获取当前节拍计数器值
    }
    portTICK_TYPE_EXIT_CRITICAL();   // 恢复中断状态

    return xTicks;
}

/*-----------------------------------------------------------*/
// 函数名称: xTaskGetTickCountFromISR
// 功能描述: 
//   从中断上下文安全获取系统节拍计数器值
// 设计要点:
//   - 验证中断优先级合法性
//   - 使用中断屏蔽保护代替任务临界区
/*-----------------------------------------------------------*/
TickType_t xTaskGetTickCountFromISR( void )
{
TickType_t xReturn;
UBaseType_t uxSavedInterruptStatus;

    /* 中断合法性断言：
       确保不在高优先级中断中调用API */
    portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

    /* 中断屏蔽操作：
       保存当前中断状态并提升屏蔽等级 */
    uxSavedInterruptStatus = portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR();
    {
        xReturn = xTickCount;  // 原子操作读取节拍值
    }
    portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    return xReturn;
}

/*-----------------------------------------------------------*/
// 函数名称: uxTaskGetNumberOfTasks
// 功能描述: 
//   返回当前系统中存在的任务总数（包括挂起和删除状态）
// 注意事项:
//   - 由于变量类型为原子类型，无需临界区保护
/*-----------------------------------------------------------*/
UBaseType_t uxTaskGetNumberOfTasks( void )
{
    return uxCurrentNumberOfTasks;  // 直接返回全局计数器
}

/*-----------------------------------------------------------*/
// 函数名称: pcTaskGetName
// 功能描述: 
//   获取指定任务的名称字符串指针
// 参数说明:
//   xTaskToQuery - 要查询的任务句柄（NULL表示当前任务）
// 返回值:
//   指向任务名称字符数组的指针（configMAX_TASK_NAME_LEN长度）
/*-----------------------------------------------------------*/
char *pcTaskGetName( TaskHandle_t xTaskToQuery )
{
TCB_t *pxTCB;

    /* 句柄转换：
       NULL参数自动转换为当前任务TCB */
    pxTCB = prvGetTCBFromHandle( xTaskToQuery );
    configASSERT( pxTCB );  // 确保TCB有效
    return &( pxTCB->pcTaskName[ 0 ] );  // 返回名称数组首地址
}

#if ( INCLUDE_xTaskGetHandle == 1 )  // 任务句柄查询功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: prvSearchForNameWithinSingleList
// 功能描述: 
//   在单个任务列表中按名称搜索任务（内部函数）
// 参数说明:
//   pxList        - 要搜索的任务列表（就绪/阻塞/挂起列表）
//   pcNameToQuery - 要匹配的任务名称（支持通配符）
// 返回值:
//   匹配任务的TCB指针，未找到返回NULL
// 算法特点:
//   - 环形遍历列表直到回到起点
//   - 逐字符对比名称字符串
/*-----------------------------------------------------------*/
static TCB_t *prvSearchForNameWithinSingleList( List_t *pxList, const char pcNameToQuery[] )
{
TCB_t *pxNextTCB, *pxFirstTCB, *pxReturn = NULL;
UBaseType_t x;
char cNextChar;

    /* 列表非空检查 */
    if( listCURRENT_LIST_LENGTH( pxList ) > 0 )
    {
        /* 获取列表入口点（环形遍历） */
        listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );

        do {
            listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );

            /* 逐字符比对任务名称 */
            for( x = 0; x < configMAX_TASK_NAME_LEN; x++ )
            {
                cNextChar = pxNextTCB->pcTaskName[ x ];

                /* 字符不匹配时跳出循环 */
                if( cNextChar != pcNameToQuery[ x ] )
                {
                    break;
                }
                /* 同时遇到终止符则匹配成功 */
                else if( cNextChar == '\0' )
                {
                    pxReturn = pxNextTCB;
                    break;
                }
            }

            if( pxReturn != NULL )  // 找到匹配项后立即终止搜索
            {
                break;
            }

        } while( pxNextTCB != pxFirstTCB );  // 环形遍历检测
    }

    return pxReturn;
}

#endif /* INCLUDE_xTaskGetHandle */

/*-----------------------------------------------------------*/
#if ( INCLUDE_xTaskGetHandle == 1 )  // 任务句柄查询功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: xTaskGetHandle
// 功能描述: 通过任务名称查询对应的任务句柄
// 参数说明:
//   pcNameToQuery - 要查询的任务名称（需小于configMAX_TASK_NAME_LEN）
// 返回值:
//   匹配任务的句柄，未找到返回NULL
// 注意事项:
//   需挂起调度器保证搜索过程的原子性
/*-----------------------------------------------------------*/
	TaskHandle_t xTaskGetHandle( const char *pcNameToQuery )
	{
	UBaseType_t uxQueue = configMAX_PRIORITIES;  // 从最高优先级开始搜索
	TCB_t* pxTCB = NULL;

		/* 名称长度校验（开发阶段捕获错误） */
		configASSERT( strlen( pcNameToQuery ) < configMAX_TASK_NAME_LEN );

		vTaskSuspendAll();  // 挂起调度器保证列表遍历原子性
		{
			/* 遍历就绪列表（按优先级从高到低） */
			do {
				uxQueue--;  // 优先级索引递减
				pxTCB = prvSearchForNameWithinSingleList( 
                    ( List_t * ) &( pxReadyTasksLists[ uxQueue ] ),  // 当前优先级就绪列表
                    pcNameToQuery );
				if( pxTCB != NULL ) break;  // 找到匹配立即终止
			} while( uxQueue > tskIDLE_PRIORITY );  // 遍历到空闲优先级为止

			/* 搜索延迟任务列表（常规） */
			if( pxTCB == NULL )
			{
				pxTCB = prvSearchForNameWithinSingleList( ( List_t * ) pxDelayedTaskList, pcNameToQuery );
			}

			/* 搜索延迟任务列表（溢出） */
			if( pxTCB == NULL )
			{
				pxTCB = prvSearchForNameWithinSingleList( ( List_t * ) pxOverflowDelayedTaskList, pcNameToQuery );
			}

			#if ( INCLUDE_vTaskSuspend == 1 )  // 挂起功能启用时
			{
				/* 搜索挂起任务列表 */
				if( pxTCB == NULL )
				{
					pxTCB = prvSearchForNameWithinSingleList( &xSuspendedTaskList, pcNameToQuery );
				}
			}
			#endif

			#if( INCLUDE_vTaskDelete == 1 )  // 删除功能启用时
			{
				/* 搜索待终止任务列表 */
				if( pxTCB == NULL )
				{
					pxTCB = prvSearchForNameWithinSingleList( &xTasksWaitingTermination, pcNameToQuery );
				}
			}
			#endif
		}
		( void ) xTaskResumeAll();  // 恢复调度器

		return ( TaskHandle_t ) pxTCB;  // 返回匹配的TCB指针
	}

#endif /* INCLUDE_xTaskGetHandle */

#if ( configUSE_TRACE_FACILITY == 1 )  // 追踪功能编译开关

/*-----------------------------------------------------------*/
// 函数名称: uxTaskGetSystemState
// 功能描述: 获取系统所有任务的状态快照
// 参数说明:
//   pxTaskStatusArray - 状态数组指针（输出参数）
//   uxArraySize       - 数组容量
//   pulTotalRunTime   - 总运行时间指针（运行时统计用）
// 返回值:
//   实际填充的任务状态数量
// 核心流程:
//   1. 挂起调度器保持数据一致性
//   2. 分层收集不同状态的任务信息
//   3. 处理运行时统计信息（如果启用）
/*-----------------------------------------------------------*/
	UBaseType_t uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray,
                                     const UBaseType_t uxArraySize,
                                     uint32_t * const pulTotalRunTime )
	{
	UBaseType_t uxTask = 0, uxQueue = configMAX_PRIORITIES;

		vTaskSuspendAll();  // 挂起调度器
		{
			/* 检查数组容量是否足够 */
			if( uxArraySize >= uxCurrentNumberOfTasks )
			{
				/* 收集就绪状态任务 */
				do {
					uxQueue--;  // 从最高优先级开始
					uxTask += prvListTasksWithinSingleList( 
                        &pxTaskStatusArray[ uxTask ],        // 当前写入位置
                        &pxReadyTasksLists[ uxQueue ],       // 当前优先级就绪列表
                        eReady );                            // 任务状态标记
				} while( uxQueue > tskIDLE_PRIORITY );

				/* 收集阻塞状态任务（常规延迟列表） */
				uxTask += prvListTasksWithinSingleList( &pxTaskStatusArray[ uxTask ], 
                                                      ( List_t * ) pxDelayedTaskList, 
                                                      eBlocked );
				/* 收集阻塞状态任务（溢出延迟列表） */
				uxTask += prvListTasksWithinSingleList( &pxTaskStatusArray[ uxTask ],
                                                      ( List_t * ) pxOverflowDelayedTaskList,
                                                      eBlocked );

				#if( INCLUDE_vTaskDelete == 1 )
				{
					/* 收集待删除任务 */
					uxTask += prvListTasksWithinSingleList( &pxTaskStatusArray[ uxTask ],
                                                          &xTasksWaitingTermination,
                                                          eDeleted );
				}
				#endif

				#if ( INCLUDE_vTaskSuspend == 1 )
				{
					/* 收集挂起任务 */
					uxTask += prvListTasksWithinSingleList( &pxTaskStatusArray[ uxTask ],
                                                          &xSuspendedTaskList,
                                                          eSuspended );
				}
				#endif

				/* 处理运行时统计信息 */
				#if ( configGENERATE_RUN_TIME_STATS == 1 )
				{
					if( pulTotalRunTime != NULL )
					{
						/* 获取总运行时间（架构特定实现） */
						#ifdef portALT_GET_RUN_TIME_COUNTER_VALUE
							portALT_GET_RUN_TIME_COUNTER_VALUE( *pulTotalRunTime );
						#else
							*pulTotalRunTime = portGET_RUN_TIME_COUNTER_VALUE();
						#endif
					}
				}
				#else
				{
					if( pulTotalRunTime != NULL )
					{
						*pulTotalRunTime = 0;  // 未启用运行时统计时清零
					}
				}
				#endif
			}
		}
		( void ) xTaskResumeAll();  // 恢复调度器

		return uxTask;  // 返回收集的任务状态数量
	}

#endif /* configUSE_TRACE_FACILITY */

/*----------------------------------------------------------*/
/*----------------------------------------------------------*/
/**
* @brief 获取空闲任务句柄
* @details 该函数用于获取FreeRTOS空闲任务的TCB（任务控制块）句柄
* @return TaskHandle_t 类型，指向空闲任务控制块的指针
* @note 需要在FreeRTOSConfig.h中启用INCLUDE_xTaskGetIdleTaskHandle配置
*/
#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )

    TaskHandle_t xTaskGetIdleTaskHandle( void )
    {
        /* 断言检查确保调度器已启动，xIdleTaskHandle已被初始化 */
        configASSERT( ( xIdleTaskHandle != NULL ) );
        return xIdleTaskHandle;  // 返回静态全局变量xIdleTaskHandle的值
    }

#endif /* INCLUDE_xTaskGetIdleTaskHandle */
/*----------------------------------------------------------*/

/**
* @brief 调整系统节拍计数器
* @param xTicksToJump 需要增加的节拍数
* @details 用于低功耗模式恢复后补偿跳过的系统节拍
* @note 需要配置configUSE_TICKLESS_IDLE为非0值启用
*/
#if ( configUSE_TICKLESS_IDLE != 0 )

    void vTaskStepTick( const TickType_t xTicksToJump )
    {
        /* 验证节拍增量不会导致总节拍数超过下一个任务解除阻塞时间 */
        configASSERT( ( xTickCount + xTicksToJump ) <= xNextTaskUnblockTime );
        xTickCount += xTicksToJump;        // 更新全局节拍计数器
        traceINCREASE_TICK_COUNT( xTicksToJump );  // 调用跟踪宏记录节拍变化
    }

#endif /* configUSE_TICKLESS_IDLE */
/*----------------------------------------------------------*/

/**
* @brief 强制终止任务的延迟状态
* @param xTask 要操作的任务句柄
* @return BaseType_t 操作结果，pdTRUE表示成功，pdFAIL表示失败
* @details 该函数将任务从阻塞状态中移除并使其进入就绪状态
* @note 需要配置INCLUDE_xTaskAbortDelay为1来启用该功能
*/
#if ( INCLUDE_xTaskAbortDelay == 1 )

    BaseType_t xTaskAbortDelay( TaskHandle_t xTask )
    {
        TCB_t *pxTCB = ( TCB_t * ) xTask;  // 将句柄转换为任务控制块指针
        BaseType_t xReturn = pdFALSE;      // 默认返回值设为失败

        configASSERT( pxTCB );  // 验证任务句柄有效性

        vTaskSuspendAll();  // 挂起调度器防止任务状态变化
        {
            /* 检查任务当前是否处于阻塞状态 */
            if( eTaskGetState( xTask ) == eBlocked )
            {
                /* 从状态列表中移除任务 */
                ( void ) uxListRemove( &( pxTCB->xStateListItem ) );

                /* 临界区保护事件列表操作 */
                taskENTER_CRITICAL();
                {
                    /* 检查任务是否在事件等待列表中 */
                    if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
                    {
                        ( void ) uxListRemove( &( pxTCB->xEventListItem ) );  // 从事件列表移除
                        pxTCB->ucDelayAborted = pdTRUE;  // 设置延迟终止标志
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();  // 代码覆盖率测试标记
                    }
                }
                taskEXIT_CRITICAL();

                /* 将任务添加到就绪列表 */
                prvAddTaskToReadyList( pxTCB );

                /* 处理抢占相关逻辑 */
                #if (  configUSE_PREEMPTION == 1 )
                {
                    /* 比较任务优先级决定是否触发上下文切换 */
                    if( pxTCB->uxPriority > pxCurrentTCB->uxPriority )
                    {
                        xYieldPending = pdTRUE;  // 设置挂起切换标志
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();  // 代码覆盖率测试标记
                    }
                }
                #endif /* configUSE_PREEMPTION */
                
                xReturn = pdPASS;  // 操作成功设置返回值
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 代码覆盖率测试标记
            }
        }
        xTaskResumeAll();  // 恢复调度器

        return xReturn;  // 返回操作结果
    }

#endif /* INCLUDE_xTaskAbortDelay */

/*----------------------------------------------------------*/

/**
* @brief 系统节拍计数器递增函数
* @return BaseType_t 是否需要上下文切换 (pdTRUE需要切换，pdFALSE不需要)
* @details 该函数是FreeRTOS时钟节拍中断的核心处理函数，主要完成：
*          1. 递增系统节拍计数器
*          2. 检查并处理延迟任务解除阻塞
*          3. 处理时间片调度
*          4. 执行用户定义的tick钩子函数
* @note 该函数由移植层在每次tick中断时调用
*/
BaseType_t xTaskIncrementTick( void )
{
TCB_t * pxTCB;                      // 临时任务控制块指针
TickType_t xItemValue;              // 列表项值（任务解除阻塞时间）
BaseType_t xSwitchRequired = pdFALSE; // 上下文切换标志，初始化为不需要

    /* 由移植层在每个tick中断调用 */
    traceTASK_INCREMENT_TICK( xTickCount );  // 调用跟踪宏记录节拍递增

    /* 判断调度器是否未挂起 */
    if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
    {
        /* 使用常量局部变量优化，避免多次访问全局变量 */
        const TickType_t xConstTickCount = xTickCount + 1;  // 临时保存递增后的节拍值

        /* 更新全局节拍计数器 */
        xTickCount = xConstTickCount;

        /* 处理节拍计数器溢出（32位系统约49天溢出一次） */
        if( xConstTickCount == ( TickType_t ) 0U )
        {
            taskSWITCH_DELAYED_LISTS();  // 交换延迟任务列表和溢出延迟任务列表
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();    // 代码覆盖率测试标记（非生产代码）
        }

        /* 检查是否达到下一个任务解除阻塞时间 */
        if( xConstTickCount >= xNextTaskUnblockTime )
        {
            /* 无限循环处理所有到期的阻塞任务 */
            for( ;; )
            {
                /* 检查延迟任务列表是否为空 */
                if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
                {
                    /* 列表为空时设置最大可能的解除阻塞时间 */
                    xNextTaskUnblockTime = portMAX_DELAY;  // 0xFFFFFFFFUL
                    break;  // 退出循环
                }
                else
                {
                    /* 获取延迟列表首任务的控制块 */
                    pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
                    xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ) );

                    /* 检查任务是否到期 */
                    if( xConstTickCount < xItemValue )
                    {
                        /* 更新下一个解除阻塞时间并退出循环 */
                        xNextTaskUnblockTime = xItemValue;
                        break;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* 将任务从阻塞状态移除 */
                    ( void ) uxListRemove( &( pxTCB->xStateListItem ) );

                    /* 检查是否在事件列表中等待 */
                    if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
                    {
                        ( void ) uxListRemove( &( pxTCB->xEventListItem ) );
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* 将任务加入就绪列表 */
                    prvAddTaskToReadyList( pxTCB );

                    /* 处理抢占逻辑 */
                    #if (  configUSE_PREEMPTION == 1 )
                    {
                        /* 比较任务优先级决定是否需要切换 */
                        if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )
                        {
                            xSwitchRequired = pdTRUE;  // 标记需要上下文切换
                        }
                        else
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }
                    }
                    #endif /* configUSE_PREEMPTION */
                }
            }
        }

        /* 时间片调度处理（需同时启用抢占和时间片） */
        #if ( ( configUSE_PREEMPTION == 1 ) && ( configUSE_TIME_SLICING == 1 ) )
        {
            /* 检查当前优先级下是否有多个就绪任务 */
            if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ pxCurrentTCB->uxPriority ] ) ) > ( UBaseType_t ) 1 )
            {
                xSwitchRequired = pdTRUE;  // 触发时间片切换
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        #endif

        /* 执行用户定义的tick钩子函数 */
        #if ( configUSE_TICK_HOOK == 1 )
        {
            /* 在调度器未锁定时执行 */
            if( uxPendedTicks == ( UBaseType_t ) 0U )
            {
                vApplicationTickHook();  // 调用用户定义的钩子函数
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        #endif
    }
    else
    {
        /* 调度器挂起时累积待处理节拍 */
        ++uxPendedTicks;

        /* 即使调度器锁定也执行tick钩子 */
        #if ( configUSE_TICK_HOOK == 1 )
        {
            vApplicationTickHook();
        }
        #endif
    }

    /* 处理延迟的上下文切换请求 */
    #if ( configUSE_PREEMPTION == 1 )
    {
        if( xYieldPending != pdFALSE )
        {
            xSwitchRequired = pdTRUE;  // 处理挂起的切换请求
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
    #endif

    return xSwitchRequired;  // 返回是否需要进行上下文切换
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/**
* @brief 设置应用程序任务标签函数
* @param xTask 目标任务句柄（NULL表示当前任务）
* @param pxHookFunction 要设置的任务标签函数指针
* @details 该函数允许为指定任务附加自定义的回调函数
* @note 需要启用configUSE_APPLICATION_TASK_TAG配置
*/
#if ( configUSE_APPLICATION_TASK_TAG == 1 )

    void vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxHookFunction )
    {
    TCB_t *xTCB;  // 任务控制块指针

        /* 处理空任务句柄的特殊情况 */
        if( xTask == NULL )
        {
            /* 获取当前运行任务的控制块 */
            xTCB = ( TCB_t * ) pxCurrentTCB;
        }
        else
        {
            /* 转换用户传入的任务句柄 */
            xTCB = ( TCB_t * ) xTask;
        }

        /* 进入临界区保护TCB访问 */
        taskENTER_CRITICAL();
            /* 将回调函数存入任务控制块 */
            xTCB->pxTaskTag = pxHookFunction;
        taskEXIT_CRITICAL();
    }

#endif /* configUSE_APPLICATION_TASK_TAG */
/*-----------------------------------------------------------*/

/**
* @brief 获取应用程序任务标签函数
* @param xTask 目标任务句柄（NULL表示当前任务）
* @return TaskHookFunction_t 当前设置的任务标签函数指针
* @details 该函数用于检索已附加到任务的自定义回调函数
*/
#if ( configUSE_APPLICATION_TASK_TAG == 1 )

    TaskHookFunction_t xTaskGetApplicationTaskTag( TaskHandle_t xTask )
    {
    TCB_t *xTCB;                // 任务控制块指针
    TaskHookFunction_t xReturn; // 返回值存储

        /* 处理空任务句柄的特殊情况 */
        if( xTask == NULL )
        {
            xTCB = ( TCB_t * ) pxCurrentTCB;
        }
        else
        {
            xTCB = ( TCB_t * ) xTask;
        }

        /* 进入临界区保护TCB访问 */
        taskENTER_CRITICAL();
        {
            /* 从TCB中提取存储的回调函数 */
            xReturn = xTCB->pxTaskTag;
        }
        taskEXIT_CRITICAL();

        return xReturn;  // 返回获取到的函数指针
    }

#endif /* configUSE_APPLICATION_TASK_TAG */
/*-----------------------------------------------------------*/

/**
* @brief 调用应用程序任务标签函数
* @param xTask 目标任务句柄（NULL表示当前任务）
* @param pvParameter 传递给回调函数的参数
* @return BaseType_t 执行结果（pdPASS成功，pdFAIL失败）
* @details 该函数执行已附加到任务的自定义回调函数
*/
#if ( configUSE_APPLICATION_TASK_TAG == 1 )

    BaseType_t xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter )
    {
    TCB_t *xTCB;          // 任务控制块指针
    BaseType_t xReturn;   // 返回值存储

        /* 处理空任务句柄的特殊情况 */
        if( xTask == NULL )
        {
            xTCB = ( TCB_t * ) pxCurrentTCB;
        }
        else
        {
            xTCB = ( TCB_t * ) xTask;
        }

        /* 检查是否存在有效的回调函数 */
        if( xTCB->pxTaskTag != NULL )
        {
            /* 执行回调函数并获取返回值 */
            xReturn = xTCB->pxTaskTag( pvParameter );
        }
        else
        {
            /* 无回调函数时返回失败状态 */
            xReturn = pdFAIL;
        }

        return xReturn;  // 返回执行结果
    }

#endif /* configUSE_APPLICATION_TASK_TAG */

/*-----------------------------------------------------------*/
/**
* @brief 任务上下文切换函数
* @details 该函数是FreeRTOS任务切换的核心实现，主要完成：
*          1. 处理调度器挂起状态
*          2. 更新任务运行时间统计
*          3. 检查栈溢出
*          4. 选择最高优先级就绪任务
* @note 该函数由调度器在需要切换任务时调用
*/
void vTaskSwitchContext( void )
{
    /* 检查调度器是否被挂起 */
    if( uxSchedulerSuspended != ( UBaseType_t ) pdFALSE )
    {
        /* 调度器挂起时设置延迟切换标志 */
        xYieldPending = pdTRUE;
    }
    else
    {
        xYieldPending = pdFALSE;  // 清除切换标志
        traceTASK_SWITCHED_OUT();  // 跟踪任务切换出事件

        /* 运行时间统计处理 */
        #if ( configGENERATE_RUN_TIME_STATS == 1 )
        {
            /* 获取当前运行时间计数器值 */
            #ifdef portALT_GET_RUN_TIME_COUNTER_VALUE
                portALT_GET_RUN_TIME_COUNTER_VALUE( ulTotalRunTime );
            #else
                ulTotalRunTime = portGET_RUN_TIME_COUNTER_VALUE();
            #endif

            /* 计算当前任务运行时间并累加 */
            if( ulTotalRunTime > ulTaskSwitchedInTime )
            {
                pxCurrentTCB->ulRunTimeCounter += ( ulTotalRunTime - ulTaskSwitchedInTime );
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 处理计数器溢出情况
            }
            ulTaskSwitchedInTime = ulTotalRunTime;  // 更新任务启动时间
        }
        #endif /* configGENERATE_RUN_TIME_STATS */

        /* 栈溢出检查 */
        taskCHECK_FOR_STACK_OVERFLOW();

        /* 选择最高优先级任务（平台相关实现） */
        taskSELECT_HIGHEST_PRIORITY_TASK();
        traceTASK_SWITCHED_IN();  // 跟踪任务切换入事件

        /* Newlib重入指针处理 */
        #if ( configUSE_NEWLIB_REENTRANT == 1 )
        {
            _impure_ptr = &( pxCurrentTCB->xNewLib_reent );  // 更新Newlib重入结构指针
        }
        #endif /* configUSE_NEWLIB_REENTRANT */
    }
}

/*-----------------------------------------------------------*/

/**
* @brief 将任务放入有序事件列表
* @param pxEventList 目标事件列表指针
* @param xTicksToWait 最大等待时间（单位：节拍）
* @details 该函数将当前任务加入事件等待队列，并按优先级排序
* @warning 必须在中断禁用或调度器挂起状态下调用
*/
void vTaskPlaceOnEventList( List_t * const pxEventList, const TickType_t xTicksToWait )
{
    configASSERT( pxEventList );  // 验证事件列表有效性

    /* 将当前任务的事件列表项按优先级插入事件列表 */
    vListInsert( pxEventList, &( pxCurrentTCB->xEventListItem ) );

    /* 将任务添加到延迟列表并处理阻塞状态 */
    prvAddCurrentTaskToDelayedList( xTicksToWait, pdTRUE );
}

/*-----------------------------------------------------------*/

/**
* @brief 将任务放入无序事件列表
* @param pxEventList 目标事件列表指针
* @param xItemValue 事件项特征值
* @param xTicksToWait 最大等待时间（单位：节拍）
* @details 该函数用于事件组实现，将任务事件项插入列表末尾
* @note 必须在调度器挂起状态下调用
*/
void vTaskPlaceOnUnorderedEventList( List_t * pxEventList, const TickType_t xItemValue, const TickType_t xTicksToWait )
{
    configASSERT( pxEventList );  // 验证事件列表有效性
    configASSERT( uxSchedulerSuspended != 0 );  // 确保调度器已挂起

    /* 设置事件项特征值（标记为正在使用） */
    listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xEventListItem ), xItemValue | taskEVENT_LIST_ITEM_VALUE_IN_USE );

    /* 将事件项插入列表末尾（无序插入） */
    vListInsertEnd( pxEventList, &( pxCurrentTCB->xEventListItem ) );

    /* 将任务添加到延迟列表并处理阻塞状态 */
    prvAddCurrentTaskToDelayedList( xTicksToWait, pdTRUE );
}

/*-----------------------------------------------------------*/
/**
* @brief 受限版本的事件列表放置函数
* @param pxEventList 目标事件列表指针
* @param xTicksToWait 最大等待时间（单位：节拍）
* @param xWaitIndefinitely 是否无限期等待标志
* @details 该函数是定时器服务专用内部API，用于将任务放入事件列表
* @warning 必须在调度器挂起状态下调用，非公开API
*/
#if( configUSE_TIMERS == 1 )

    void vTaskPlaceOnEventListRestricted( List_t * const pxEventList, TickType_t xTicksToWait, const BaseType_t xWaitIndefinitely )
    {
        configASSERT( pxEventList );  // 验证事件列表有效性

        /* 使用快速末端插入方式（假设当前是唯一等待任务） */
        vListInsertEnd( pxEventList, &( pxCurrentTCB->xEventListItem ) );

        /* 处理无限期等待的特殊情况 */
        if( xWaitIndefinitely != pdFALSE )
        {
            xTicksToWait = portMAX_DELAY;  // 设置为最大延迟值(0xFFFFFFFF)
        }

        traceTASK_DELAY_UNTIL( ( xTickCount + xTicksToWait ) );  // 跟踪延迟时间
        prvAddCurrentTaskToDelayedList( xTicksToWait, xWaitIndefinitely );  // 添加到延迟列表
    }

#endif /* configUSE_TIMERS */
/*-----------------------------------------------------------*/

/**
* @brief 从事件列表移除任务
* @param pxEventList 源事件列表指针
* @return BaseType_t 是否需要上下文切换
* @details 该函数从事件列表中唤醒最高优先级任务，通常用于事件触发时
* @warning 必须在临界区内调用，可由中断服务程序调用
*/
BaseType_t xTaskRemoveFromEventList( const List_t * const pxEventList )
{
TCB_t *pxUnblockedTCB;  // 被唤醒任务控制块指针
BaseType_t xReturn;      // 返回值存储

    /* 获取事件列表首项对应的任务控制块 */
    pxUnblockedTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxEventList );
    configASSERT( pxUnblockedTCB );  // 验证控制块有效性
    
    /* 从事件列表中移除该任务项 */
    ( void ) uxListRemove( &( pxUnblockedTCB->xEventListItem ) );

    /* 根据调度器状态处理任务状态 */
    if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
    {
        /* 正常状态：从延迟列表移除并加入就绪列表 */
        ( void ) uxListRemove( &( pxUnblockedTCB->xStateListItem ) );
        prvAddTaskToReadyList( pxUnblockedTCB );
    }
    else
    {
        /* 调度器挂起时暂存到待处理就绪列表 */
        vListInsertEnd( &( xPendingReadyList ), &( pxUnblockedTCB->xEventListItem ) );
    }

    /* 优先级比较决定是否需要切换 */
    if( pxUnblockedTCB->uxPriority > pxCurrentTCB->uxPriority )
    {
        xReturn = pdTRUE;          // 需要上下文切换
        xYieldPending = pdTRUE;    // 设置挂起切换标志
    }
    else
    {
        xReturn = pdFALSE;         // 不需要切换
    }

    /* Tickless空闲模式特殊处理 */
    #if( configUSE_TICKLESS_IDLE != 0 )
    {
        /* 重置下一个任务解除阻塞时间以优化休眠 */
        prvResetNextTaskUnblockTime();
    }
    #endif

    return xReturn;  // 返回切换需求标志
}

/*-----------------------------------------------------------*/
/**
* @brief 从无序事件列表移除任务
* @param pxEventListItem 事件列表项指针
* @param xItemValue 事件项特征值
* @return BaseType_t 是否需要上下文切换
* @details 该函数用于事件组实现，处理任务从无序事件列表唤醒
* @warning 必须在调度器挂起状态下调用
*/
BaseType_t xTaskRemoveFromUnorderedEventList( ListItem_t * pxEventListItem, const TickType_t xItemValue )
{
TCB_t *pxUnblockedTCB;  // 被唤醒任务控制块指针
BaseType_t xReturn;      // 返回值存储

    configASSERT( uxSchedulerSuspended != pdFALSE );  // 验证调度器状态

    /* 更新事件项特征值（标记为正在使用） */
    listSET_LIST_ITEM_VALUE( pxEventListItem, xItemValue | taskEVENT_LIST_ITEM_VALUE_IN_USE );

    /* 获取任务控制块并验证有效性 */
    pxUnblockedTCB = ( TCB_t * ) listGET_LIST_ITEM_OWNER( pxEventListItem );
    configASSERT( pxUnblockedTCB );

    /* 从事件列表和状态列表移除任务项 */
    ( void ) uxListRemove( pxEventListItem );
    ( void ) uxListRemove( &( pxUnblockedTCB->xStateListItem ) );

    /* 将任务加入就绪列表 */
    prvAddTaskToReadyList( pxUnblockedTCB );

    /* 优先级比较决定切换需求 */
    if( pxUnblockedTCB->uxPriority > pxCurrentTCB->uxPriority )
    {
        xReturn = pdTRUE;        // 需要上下文切换
        xYieldPending = pdTRUE;  // 设置挂起标志
    }
    else
    {
        xReturn = pdFALSE;       // 无需切换
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

/**
* @brief 初始化超时状态结构
* @param pxTimeOut 超时状态结构指针
* @details 记录当前系统节拍计数和溢出次数
* @note 该函数应在任务进入阻塞状态前调用
*/
void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut )
{
    configASSERT( pxTimeOut );  // 验证结构指针有效性
    /* 记录当前溢出次数和节拍值 */
    pxTimeOut->xOverflowCount = xNumOfOverflows;  // 全局溢出计数器
    pxTimeOut->xTimeOnEntering = xTickCount;       // 当前节拍计数器
}

/*-----------------------------------------------------------*/

/**
* @brief 检查任务超时状态
* @param pxTimeOut 超时状态结构指针
* @param pxTicksToWait 剩余等待时间指针
* @return BaseType_t 是否发生超时
* @details 该函数处理以下情况：
*          1. 延迟被主动中止
*          2. 无限期等待配置
*          3. 节拍计数器溢出处理
*          4. 剩余时间调整
*/
BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait )
{
BaseType_t xReturn;

    configASSERT( pxTimeOut );     // 验证结构指针
    configASSERT( pxTicksToWait ); // 验证时间指针

    taskENTER_CRITICAL();  // 进入临界区保护
    {
        const TickType_t xConstTickCount = xTickCount;  // 获取当前节拍快照

        /* 处理延迟中止情况 */
        #if( INCLUDE_xTaskAbortDelay == 1 )
            if( pxCurrentTCB->ucDelayAborted != pdFALSE )
            {
                pxCurrentTCB->ucDelayAborted = pdFALSE;  // 清除中止标志
                xReturn = pdTRUE;                         // 返回超时状态
            }
            else
        #endif

        /* 处理无限期等待配置 */
        #if ( INCLUDE_vTaskSuspend == 1 )
            if( *pxTicksToWait == portMAX_DELAY )
            {
                xReturn = pdFALSE;  // 无限期等待永不超时
            }
            else
        #endif

        /* 节拍溢出检测 */
        if( ( xNumOfOverflows != pxTimeOut->xOverflowCount ) && 
            ( xConstTickCount >= pxTimeOut->xTimeOnEntering ) )
        {
            /* 节拍计数器完全循环后超过原时间 */
            xReturn = pdTRUE;
        }
        /* 剩余时间计算 */
        else if( ( ( TickType_t )( xConstTickCount - pxTimeOut->xTimeOnEntering ) ) < *pxTicksToWait )
        {
            /* 调整剩余等待时间 */
            *pxTicksToWait -= ( xConstTickCount - pxTimeOut->xTimeOnEntering );
            vTaskSetTimeOutState( pxTimeOut );  // 重置超时基准
            xReturn = pdFALSE;
        }
        else
        {
            /* 正常超时情况 */
            xReturn = pdTRUE;
        }
    }
    taskEXIT_CRITICAL();  // 退出临界区

    return xReturn;
}

/*-----------------------------------------------------------*/
/**
* @brief 标记未处理的上下文切换请求
* @details 该函数用于在调度器无法立即切换时设置延迟切换标志
* @note 通常在需要强制切换但调度器无法立即响应时调用
*/
void vTaskMissedYield( void )
{
    xYieldPending = pdTRUE;  // 设置全局切换挂起标志
}

/*-----------------------------------------------------------*/

/**
* @brief 获取任务编号
* @param xTask 目标任务句柄（NULL表示当前任务）
* @return UBaseType_t 任务唯一编号
* @details 用于调试追踪系统，需启用configUSE_TRACE_FACILITY
*/
#if ( configUSE_TRACE_FACILITY == 1 )

    UBaseType_t uxTaskGetTaskNumber( TaskHandle_t xTask )
    {
    UBaseType_t uxReturn;  // 返回值存储
    TCB_t *pxTCB;          // 任务控制块指针

        /* 处理空指针情况 */
        if( xTask != NULL )
        {
            pxTCB = ( TCB_t * ) xTask;          // 转换任务句柄
            uxReturn = pxTCB->uxTaskNumber;     // 获取任务编号字段
        }
        else
        {
            uxReturn = 0U;  // 返回默认无效编号
        }

        return uxReturn;
    }

#endif /* configUSE_TRACE_FACILITY */
/*-----------------------------------------------------------*/

/**
* @brief 设置任务编号
* @param xTask 目标任务句柄（NULL表示当前任务）
* @param uxHandle 要设置的任务编号
* @details 用于调试追踪系统，需启用configUSE_TRACE_FACILITY
*/
#if ( configUSE_TRACE_FACILITY == 1 )

    void vTaskSetTaskNumber( TaskHandle_t xTask, const UBaseType_t uxHandle )
    {
    TCB_t *pxTCB;  // 任务控制块指针

        /* 验证任务句柄有效性 */
        if( xTask != NULL )
        {
            pxTCB = ( TCB_t * ) xTask;         // 转换任务句柄
            pxTCB->uxTaskNumber = uxHandle;    // 更新任务编号字段
        }
        /* 空指针时忽略操作（安全处理） */
    }

#endif /* configUSE_TRACE_FACILITY */
/**
* @brief RTOS空闲任务函数
* @param pvParameters 任务参数（未使用）
* @details 该函数是FreeRTOS自动创建的空闲任务，主要功能包括：
*          1. 清理已终止任务的资源
*          2. 处理低功耗模式
*          3. 调用用户空闲钩子函数
*          4. 协调不同调度模式下的任务切换
* @note 该任务始终运行在最低优先级（tskIDLE_PRIORITY）
*/
static portTASK_FUNCTION( prvIdleTask, pvParameters )
{
    ( void ) pvParameters;  // 显式忽略未使用参数（消除编译器警告）

    /* 无限循环处理后台任务 */
    for( ;; )
    {
        /* 清理已终止任务的TCB和堆栈 */
        prvCheckTasksWaitingTermination();

        /* 协作式调度处理（非抢占模式） */
        #if ( configUSE_PREEMPTION == 0 )
        {
            /* 在非抢占模式下强制切换任务 */
            taskYIELD();  // 主动让出CPU使用权
        }
        #endif /* configUSE_PREEMPTION */

        /* 优先级时间片处理（需同时启用抢占和空闲让步） */
        #if ( ( configUSE_PREEMPTION == 1 ) && ( configIDLE_SHOULD_YIELD == 1 ) )
        {
            /* 检查是否有同优先级任务就绪 */
            if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ tskIDLE_PRIORITY ] ) ) > ( UBaseType_t ) 1 )
            {
                taskYIELD();  // 让出CPU给同优先级任务
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 覆盖率测试标记
            }
        }
        #endif

        /* 用户空闲钩子处理 */
        #if ( configUSE_IDLE_HOOK == 1 )
        {
            extern void vApplicationIdleHook( void );
            /* 执行用户定义的低功耗/后台操作（严禁阻塞操作） */
            vApplicationIdleHook();  // 用户自定义函数
        }
        #endif /* configUSE_IDLE_HOOK */

        /* Tickless低功耗模式处理 */
        #if ( configUSE_TICKLESS_IDLE != 0 )
        {
            TickType_t xExpectedIdleTime;  // 预计空闲时间

            /* 初步估算空闲时间（调度器未挂起） */
            xExpectedIdleTime = prvGetExpectedIdleTime();

            /* 检查是否满足进入低功耗条件 */
            if( xExpectedIdleTime >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP )
            {
                vTaskSuspendAll();  // 挂起调度器
                {
                    /* 精确计算空闲时间（临界区内） */
                    xExpectedIdleTime = prvGetExpectedIdleTime();

                    if( xExpectedIdleTime >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP )
                    {
                        traceLOW_POWER_IDLE_BEGIN();  // 跟踪低功耗开始
                        portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime );  // 进入低功耗状态
                        traceLOW_POWER_IDLE_END();    // 跟踪低功耗结束
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();  // 不满足条件分支
                    }
                }
                ( void ) xTaskResumeAll();  // 恢复调度器
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 不满足条件分支
            }
        }
        #endif /* configUSE_TICKLESS_IDLE */
    }
}

/*-----------------------------------------------------------*/
/**
* @brief 确认低功耗睡眠模式状态
* @return eSleepModeStatus 睡眠模式状态枚举值
* @details 该函数用于Tickless空闲模式，判断系统是否满足进入低功耗条件
* @note 需启用configUSE_TICKLESS_IDLE配置
*/
#if( configUSE_TICKLESS_IDLE != 0 )

    eSleepModeStatus eTaskConfirmSleepModeStatus( void )
    {
        const UBaseType_t uxNonApplicationTasks = 1;  // 非应用任务计数（包含空闲任务）
        eSleepModeStatus eReturn = eStandardSleep;     // 默认返回标准睡眠模式

        /* 检查待处理就绪列表 */
        if( listCURRENT_LIST_LENGTH( &xPendingReadyList ) != 0 )
        {
            eReturn = eAbortSleep;  // 存在待处理任务，终止睡眠
        }
        else if( xYieldPending != pdFALSE )
        {
            eReturn = eAbortSleep;  // 存在挂起切换请求，终止睡眠
        }
        else
        {
            /* 检查所有任务是否处于挂起状态 */
            if( listCURRENT_LIST_LENGTH( &xSuspendedTaskList ) == ( uxCurrentNumberOfTasks - uxNonApplicationTasks ) )
            {
                eReturn = eNoTasksWaitingTimeout;  // 所有任务无限期阻塞，深度睡眠
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 覆盖率测试标记
            }
        }

        return eReturn;
    }

#endif /* configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/

/**
* @brief 设置线程本地存储指针
* @param xTaskToSet 目标任务句柄
* @param xIndex 存储指针索引（0 ~ configNUM_THREAD_LOCAL_STORAGE_POINTERS-1）
* @param pvValue 要存储的指针值
* @details 为任务提供私有数据存储空间，需配置configNUM_THREAD_LOCAL_STORAGE_POINTERS
*/
#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )

    void vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue )
    {
    TCB_t *pxTCB;

        /* 验证索引有效性 */
        if( xIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS )
        {
            pxTCB = prvGetTCBFromHandle( xTaskToSet );  // 获取任务控制块
            pxTCB->pvThreadLocalStoragePointers[ xIndex ] = pvValue; // 存储指针
        }
        /* 无效索引时忽略操作（安全处理） */
    }

#endif /* configNUM_THREAD_LOCAL_STORAGE_POINTERS */
/*-----------------------------------------------------------*/

/**
* @brief 获取线程本地存储指针
* @param xTaskToQuery 目标任务句柄
* @param xIndex 存储指针索引
* @return void* 存储的指针值
* @details 检索任务私有数据，空指针表示无效索引或未设置值
*/
#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )

    void *pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex )
    {
    void *pvReturn = NULL;
    TCB_t *pxTCB;

        /* 验证索引有效性 */
        if( xIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS )
        {
            pxTCB = prvGetTCBFromHandle( xTaskToQuery );  // 获取任务控制块
            pvReturn = pxTCB->pvThreadLocalStoragePointers[ xIndex ]; // 检索指针
        }

        return pvReturn;  // 返回指针或NULL
    }

#endif /* configNUM_THREAD_LOCAL_STORAGE_POINTERS */

/*-----------------------------------------------------------*/
/**
* @brief 配置任务的MPU内存区域
* @param xTaskToModify 目标任务句柄（NULL表示当前任务）
* @param xRegions MPU内存区域配置数组
* @details 该函数用于设置任务的内存保护单元(MPU)区域
* @note 需启用portUSING_MPU_WRAPPERS配置
*/
#if ( portUSING_MPU_WRAPPERS == 1 )

    void vTaskAllocateMPURegions( TaskHandle_t xTaskToModify, const MemoryRegion_t * const xRegions )
    {
    TCB_t *pxTCB;

        /* 获取任务控制块（处理空指针情况） */
        pxTCB = prvGetTCBFromHandle( xTaskToModify );
        
        /* 存储MPU配置到任务控制块 */
        vPortStoreTaskMPUSettings( &( pxTCB->xMPUSettings ), xRegions, NULL, 0 );
    }

#endif /* portUSING_MPU_WRAPPERS */
/*-----------------------------------------------------------*/

/**
* @brief 初始化系统任务列表
* @details 该函数在调度器启动前初始化所有核心数据结构：
*          1. 各优先级就绪列表
*          2. 延迟任务列表
*          3. 挂起/终止任务列表
*/
static void prvInitialiseTaskLists( void )
{
UBaseType_t uxPriority;

    /* 初始化所有优先级的就绪列表 */
    for( uxPriority = 0U; uxPriority < configMAX_PRIORITIES; uxPriority++ )
    {
        vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );  // 初始化优先级就绪列表
    }

    /* 初始化延迟任务双缓冲列表 */
    vListInitialise( &xDelayedTaskList1 );  // 延迟列表1
    vListInitialise( &xDelayedTaskList2 );  // 延迟列表2
    
    /* 初始化待处理就绪列表 */
    vListInitialise( &xPendingReadyList );

    /* 任务删除功能相关初始化 */
    #if ( INCLUDE_vTaskDelete == 1 )
    {
        vListInitialise( &xTasksWaitingTermination );  // 终止任务回收列表
    }
    #endif

    /* 任务挂起功能相关初始化 */
    #if ( INCLUDE_vTaskSuspend == 1 )
    {
        vListInitialise( &xSuspendedTaskList );  // 挂起任务列表
    }
    #endif

    /* 初始化延迟列表指针（双缓冲机制） */
    pxDelayedTaskList = &xDelayedTaskList1;          // 主延迟列表
    pxOverflowDelayedTaskList = &xDelayedTaskList2; // 溢出延迟列表
}

/*-----------------------------------------------------------*/

/**
* @brief 检查并清理已终止任务
* @details 该函数由空闲任务循环调用，用于：
*          1. 从终止列表移除任务项
*          2. 递减任务计数器
*          3. 释放TCB和堆栈内存
* @warning 必须在调度器启动后调用
*/
static void prvCheckTasksWaitingTermination( void )
{
    #if ( INCLUDE_vTaskDelete == 1 )
    {
        BaseType_t xListIsEmpty;

        /* 循环处理所有待清理任务 */
        while( uxDeletedTasksWaitingCleanUp > 0U )
        {
            /* 挂起调度器确保列表访问安全 */
            vTaskSuspendAll();
            {
                xListIsEmpty = listLIST_IS_EMPTY( &xTasksWaitingTermination );  // 检查列表状态
            }
            ( void ) xTaskResumeAll();

            if( xListIsEmpty == pdFALSE )
            {
                TCB_t *pxTCB;

                /* 进入临界区操作任务控制块 */
                taskENTER_CRITICAL();
                {
                    pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( &xTasksWaitingTermination );  // 获取首个任务TCB
                    ( void ) uxListRemove( &( pxTCB->xStateListItem ) );  // 从状态列表移除
                    
                    /* 更新系统任务计数器 */
                    --uxCurrentNumberOfTasks;           // 当前任务总数减1
                    --uxDeletedTasksWaitingCleanUp;     // 待清理任务数减1
                }
                taskEXIT_CRITICAL();

                /* 释放任务控制块资源 */
                prvDeleteTCB( pxTCB );  // 执行内存释放操作
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 空列表分支覆盖率标记
            }
        }
    }
    #endif /* INCLUDE_vTaskDelete */
}

/*-----------------------------------------------------------*/
/**
* @brief 获取任务状态信息
* @param xTask 目标任务句柄（NULL表示当前任务）
* @param pxTaskStatus 任务状态结构体指针
* @param xGetFreeStackSpace 是否计算剩余栈空间
* @param eState 强制指定任务状态（eInvalid表示自动检测）
* @details 该函数用于调试追踪系统，填充任务状态信息结构体
* @note 需启用configUSE_TRACE_FACILITY配置
*/
#if( configUSE_TRACE_FACILITY == 1 )

    void vTaskGetInfo( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, 
                      BaseType_t xGetFreeStackSpace, eTaskState eState )
    {
    TCB_t *pxTCB;

        /* 获取任务控制块（处理空指针情况） */
        pxTCB = prvGetTCBFromHandle( xTask );

        /* 填充基础信息 */
        pxTaskStatus->xHandle = ( TaskHandle_t ) pxTCB;           // 任务句柄
        pxTaskStatus->pcTaskName = ( const char * ) &( pxTCB->pcTaskName [ 0 ] ); // 任务名称字符串指针
        pxTaskStatus->uxCurrentPriority = pxTCB->uxPriority;     // 当前优先级
        pxTaskStatus->pxStackBase = pxTCB->pxStack;              // 栈基地址
        pxTaskStatus->xTaskNumber = pxTCB->uxTCBNumber;          // 任务唯一编号

        /* 处理挂起状态与阻塞状态的差异 */
        #if ( INCLUDE_vTaskSuspend == 1 )
        {
            if( pxTaskStatus->eCurrentState == eSuspended )
            {
                /* 临界区检查实际状态 */
                vTaskSuspendAll();
                {
                    /* 通过事件列表判断真实状态 */
                    if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
                    {
                        pxTaskStatus->eCurrentState = eBlocked;  // 更正为阻塞状态
                    }
                }
                xTaskResumeAll();
            }
        }
        #endif /* INCLUDE_vTaskSuspend */

        /* 处理基本优先级（互斥量优先级继承相关） */
        #if ( configUSE_MUTEXES == 1 )
        {
            pxTaskStatus->uxBasePriority = pxTCB->uxBasePriority; // 原始优先级
        }
        #else
        {
            pxTaskStatus->uxBasePriority = 0;  // 无优先级继承时设为0
        }
        #endif

        /* 处理运行时间统计 */
        #if ( configGENERATE_RUN_TIME_STATS == 1 )
        {
            pxTaskStatus->ulRunTimeCounter = pxTCB->ulRunTimeCounter; // 任务累计运行时间
        }
        #else
        {
            pxTaskStatus->ulRunTimeCounter = 0;  // 未启用统计时设为0
        }
        #endif

        /* 确定任务状态 */
        if( eState != eInvalid )
        {
            pxTaskStatus->eCurrentState = eState;  // 使用强制指定的状态
        }
        else
        {
            pxTaskStatus->eCurrentState = eTaskGetState( xTask ); // 实际查询任务状态
        }

        /* 计算栈空间高水位线 */
        if( xGetFreeStackSpace != pdFALSE )
        {
            /* 根据栈增长方向选择计算起点 */
            #if ( portSTACK_GROWTH > 0 )  // 栈向上增长（如ARM Cortex-M）
            {
                pxTaskStatus->usStackHighWaterMark = prvTaskCheckFreeStackSpace( 
                    ( uint8_t * ) pxTCB->pxEndOfStack );  // 从栈尾开始计算
            }
            #else  // 栈向下增长（如x86）
            {
                pxTaskStatus->usStackHighWaterMark = prvTaskCheckFreeStackSpace( 
                    ( uint8_t * ) pxTCB->pxStack );  // 从栈基开始计算
            }
            #endif
        }
        else
        {
            pxTaskStatus->usStackHighWaterMark = 0;  // 不计算时设为0
        }
    }

#endif /* configUSE_TRACE_FACILITY */

/*-----------------------------------------------------------*/
/**
* @brief 遍历单个任务列表并填充状态数组
* @param pxTaskStatusArray 任务状态数组指针
* @param pxList 要遍历的任务列表
* @param eState 任务状态标识
* @return UBaseType_t 填充的任务数量
* @details 该函数用于调试追踪系统，遍历指定列表的所有任务
*/
#if ( configUSE_TRACE_FACILITY == 1 )

    static UBaseType_t prvListTasksWithinSingleList( TaskStatus_t *pxTaskStatusArray, List_t *pxList, eTaskState eState )
    {
    volatile TCB_t *pxNextTCB, *pxFirstTCB; // 使用volatile防止编译器优化
    UBaseType_t uxTask = 0;                 // 已处理任务计数器

        if( listCURRENT_LIST_LENGTH( pxList ) > 0 )
        {
            /* 获取列表首项建立遍历起点 */
            listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );

            do {
                /* 循环获取下一个任务控制块 */
                listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
                
                /* 填充当前任务状态信息 */
                vTaskGetInfo( ( TaskHandle_t ) pxNextTCB, 
                            &( pxTaskStatusArray[ uxTask ] ), 
                            pdTRUE,       // 强制获取栈信息
                            eState );     // 指定任务状态
                uxTask++;                // 递增计数器
            } while( pxNextTCB != pxFirstTCB ); // 循环直到回到起点
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();    // 空列表分支覆盖率标记
        }

        return uxTask;  // 返回填充的任务数量
    }

#endif /* configUSE_TRACE_FACILITY */
/*-----------------------------------------------------------*/

/**
* @brief 计算栈剩余空间（高水位线）
* @param pucStackByte 栈起始地址
* @return uint16_t 剩余栈空间（以字为单位）
* @details 通过检测填充字节(tskSTACK_FILL_BYTE)计算实际使用量
*/
#if ( ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) )

    static uint16_t prvTaskCheckFreeStackSpace( const uint8_t * pucStackByte )
    {
    uint32_t ulCount = 0U;

        /* 遍历栈空间直到发现非填充字节 */
        while( *pucStackByte == ( uint8_t ) tskSTACK_FILL_BYTE )
        {
            pucStackByte -= portSTACK_GROWTH;  // 根据栈增长方向调整指针
            ulCount++;                        // 统计填充字节数
        }

        /* 转换为栈单元数量（考虑架构字长） */
        ulCount /= ( uint32_t ) sizeof( StackType_t ); 

        return ( uint16_t ) ulCount;  // 返回剩余空间（字单位）
    }

#endif /* 相关配置检查 */
/*-----------------------------------------------------------*/

/**
* @brief 获取任务栈高水位线
* @param xTask 目标任务句柄
* @return UBaseType_t 栈剩余空间（字单位）
* @details 高水位线表示任务运行历史中栈的最小剩余空间
*/
#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )

    UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask )
    {
    TCB_t *pxTCB;
    uint8_t *pucEndOfStack;
    UBaseType_t uxReturn;

        /* 获取任务控制块 */
        pxTCB = prvGetTCBFromHandle( xTask );

        /* 根据栈增长方向确定检测起点 */
        #if portSTACK_GROWTH < 0  // 栈向下增长（如ARM Cortex-M）
        {
            pucEndOfStack = ( uint8_t * ) pxTCB->pxStack;  // 栈基地址
        }
        #else                     // 栈向上增长（如RISC-V）
        {
            pucEndOfStack = ( uint8_t * ) pxTCB->pxEndOfStack; // 栈顶地址
        }
        #endif

        /* 调用底层检测函数 */
        uxReturn = ( UBaseType_t ) prvTaskCheckFreeStackSpace( pucEndOfStack );

        return uxReturn;
    }

#endif /* INCLUDE_uxTaskGetStackHighWaterMark */
/*-----------------------------------------------------------*/

/**
* @brief 安全删除任务控制块
* @param pxTCB 要删除的任务控制块指针
* @details 处理不同内存分配策略，释放任务相关资源
* @warning 必须在调度器挂起状态下调用
*/
#if ( INCLUDE_vTaskDelete == 1 )

    static void prvDeleteTCB( TCB_t *pxTCB )
    {
        /* 平台特定的TCB清理操作（如TriCore架构需求） */
        portCLEAN_UP_TCB( pxTCB );  

        /* 处理Newlib重入结构回收 */
        #if ( configUSE_NEWLIB_REENTRANT == 1 )
        {
            _reclaim_reent( &( pxTCB->xNewLib_reent ) );  // 回收Newlib资源
        }
        #endif

        /* 动态内存分配处理 */
        #if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && \
              ( configSUPPORT_STATIC_ALLOCATION == 0 ) && \
              ( portUSING_MPU_WRAPPERS == 0 ) )
        {
            /* 完全动态分配：释放栈和TCB */
            vPortFree( pxTCB->pxStack );  // 释放任务栈
            vPortFree( pxTCB );           // 释放TCB结构体
        }
        #elif( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE == 1 )
        {
            /* 混合分配策略处理 */
            switch( pxTCB->ucStaticallyAllocated )
            {
                case tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB:
                    vPortFree( pxTCB->pxStack );  // 释放动态分配的栈
                    vPortFree( pxTCB );          // 释放动态分配的TCB
                    break;

                case tskSTATICALLY_ALLOCATED_STACK_ONLY:
                    vPortFree( pxTCB );          // 仅释放动态分配的TCB
                    break;

                default:  // tskSTATICALLY_ALLOCATED_STACK_AND_TCB
                    /* 全静态分配无需释放 */
                    configASSERT( pxTCB->ucStaticallyAllocated == 
                                tskSTATICALLY_ALLOCATED_STACK_AND_TCB );
                    mtCOVERAGE_TEST_MARKER();  // 覆盖率标记
                    break;
            }
        }
        #endif /* 内存分配策略检查 */
    }

#endif /* INCLUDE_vTaskDelete */

/*-----------------------------------------------------------*/
/**
* @brief 重置下一个任务解除阻塞时间
* @details 该函数用于优化Tickless空闲模式，更新下一个待解除阻塞任务的时间戳
*/
static void prvResetNextTaskUnblockTime( void )
{
TCB_t *pxTCB;

    /* 检查当前延迟任务列表是否为空 */
    if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
    {
        /* 列表为空时设置最大可能值 */
        xNextTaskUnblockTime = portMAX_DELAY; // 0xFFFFFFFF
    }
    else
    {
        /* 获取延迟列表首任务的解除阻塞时间 */
        pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
        xNextTaskUnblockTime = listGET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ) );
    }
}

/*-----------------------------------------------------------*/

/**
* @brief 获取当前运行任务句柄
* @return TaskHandle_t 当前任务控制块指针
* @note 该函数可用于互斥量优先级继承等场景
*/
#if ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) )

    TaskHandle_t xTaskGetCurrentTaskHandle( void )
    {
        /* 直接返回全局当前任务指针（无需临界区保护） */
        return pxCurrentTCB; // 返回类型自动转换为TaskHandle_t
    }

#endif /* 相关配置检查 */
/*-----------------------------------------------------------*/

/**
* @brief 获取调度器运行状态
* @return BaseType_t 返回状态枚举值：
*         taskSCHEDULER_NOT_STARTED  未启动
*         taskSCHEDULER_RUNNING      运行中
*         taskSCHEDULER_SUSPENDED    挂起
*/
#if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )

    BaseType_t xTaskGetSchedulerState( void )
    {
    BaseType_t xReturn;

        if( xSchedulerRunning == pdFALSE )
        {
            xReturn = taskSCHEDULER_NOT_STARTED; // 调度器未启动
        }
        else
        {
            /* 根据挂起标志判断状态 */
            xReturn = ( uxSchedulerSuspended == pdFALSE ) ? 
                    taskSCHEDULER_RUNNING :      // 正常运行
                    taskSCHEDULER_SUSPENDED;    // 调度器挂起
        }

        return xReturn;
    }

#endif /* 相关配置检查 */
/*-----------------------------------------------------------*/

/**
* @brief 任务优先级继承实现
* @param pxMutexHolder 互斥量持有者任务句柄
* @details 当高优先级任务请求被占用的互斥量时，临时提升持有者优先级
*/
#if ( configUSE_MUTEXES == 1 )

    void vTaskPriorityInherit( TaskHandle_t const pxMutexHolder )
    {
    TCB_t * const pxTCB = ( TCB_t * ) pxMutexHolder;

        if( pxMutexHolder != NULL )
        {
            /* 仅当持有者优先级低于当前任务时处理 */
            if( pxTCB->uxPriority < pxCurrentTCB->uxPriority )
            {
                /* 更新事件列表项优先级（如果未被占用） */
                if( ( listGET_LIST_ITEM_VALUE( &pxTCB->xEventListItem ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == 0UL )
                {
                    /* 计算反转优先级值（数值越小优先级越高） */
                    listSET_LIST_ITEM_VALUE( &pxTCB->xEventListItem, 
                        configMAX_PRIORITIES - pxCurrentTCB->uxPriority );
                }

                /* 处理就绪状态的任务移动 */
                if( listIS_CONTAINED_WITHIN( &pxReadyTasksLists[ pxTCB->uxPriority ], &pxTCB->xStateListItem ) )
                {
                    /* 从原优先级列表移除 */
                    if( uxListRemove( &pxTCB->xStateListItem ) == 0 )
                    {
                        taskRESET_READY_PRIORITY( pxTCB->uxPriority ); // 清除就绪位
                    }
                    
                    /* 更新优先级并加入新列表 */
                    pxTCB->uxPriority = pxCurrentTCB->uxPriority;
                    prvAddTaskToReadyList( pxTCB );
                }
                else
                {
                    /* 非就绪状态直接更新优先级 */
                    pxTCB->uxPriority = pxCurrentTCB->uxPriority;
                }

                traceTASK_PRIORITY_INHERIT( pxTCB, pxTCB->uxPriority ); // 跟踪调试
            }
        }
    }

#endif /* configUSE_MUTEXES */

/*-----------------------------------------------------------*/
/**
* @brief 任务优先级恢复函数
* @param pxMutexHolder 释放互斥量的任务句柄
* @return BaseType_t 是否需要上下文切换
* @details 当互斥量被释放时恢复任务原始优先级，解决优先级继承问题
* @note 需启用configUSE_MUTEXES配置
*/
#if ( configUSE_MUTEXES == 1 )

    BaseType_t xTaskPriorityDisinherit( TaskHandle_t const pxMutexHolder )
    {
    TCB_t * const pxTCB = ( TCB_t * ) pxMutexHolder; // 转换任务控制块指针
    BaseType_t xReturn = pdFALSE;                    // 默认返回不需要切换

        if( pxMutexHolder != NULL )
        {
            /* 验证当前任务确实是互斥量持有者 */
            configASSERT( pxTCB == pxCurrentTCB );
            /* 确保互斥量持有计数有效 */
            configASSERT( pxTCB->uxMutexesHeld > 0 );

            /* 递减互斥量持有计数 */
            pxTCB->uxMutexesHeld--;

            /* 检查是否需要恢复优先级 */
            if( pxTCB->uxPriority != pxTCB->uxBasePriority )
            {
                /* 仅当没有其他互斥量持有时恢复优先级 */
                if( pxTCB->uxMutexesHeld == 0 )
                {
                    /* 从就绪列表移除任务 */
                    if( uxListRemove( &pxTCB->xStateListItem ) == 0 )
                    {
                        taskRESET_READY_PRIORITY( pxTCB->uxPriority ); // 清除就绪位图
                    }

                    /* 跟踪调试并恢复基础优先级 */
                    traceTASK_PRIORITY_DISINHERIT( pxTCB, pxTCB->uxBasePriority );
                    pxTCB->uxPriority = pxTCB->uxBasePriority; // 恢复原始优先级

                    /* 更新事件列表项值（数值越小优先级越高） */
                    listSET_LIST_ITEM_VALUE( &pxTCB->xEventListItem, 
                        configMAX_PRIORITIES - pxTCB->uxPriority );

                    /* 重新加入就绪列表 */
                    prvAddTaskToReadyList( pxTCB );

                    /* 返回需要上下文切换标志（处理多互斥量嵌套场景） */
                    xReturn = pdTRUE;
                }
            }
        }

        return xReturn; // 返回切换需求标志
    }

#endif /* configUSE_MUTEXES */

/*-----------------------------------------------------------*/
/**
* @brief 进入临界区（嵌套计数存储在TCB）
* @details 禁用中断并增加当前任务的临界区嵌套计数
* @note 需启用portCRITICAL_NESTING_IN_TCB配置
*/
#if ( portCRITICAL_NESTING_IN_TCB == 1 )

    void vTaskEnterCritical( void )
    {
        portDISABLE_INTERRUPTS();  // 关闭中断

        if( xSchedulerRunning != pdFALSE )  // 确保调度器已启动
        {
            pxCurrentTCB->uxCriticalNesting++;  // 递增嵌套计数器

            /* 首次进入时检查是否在中断中调用 */
            if( pxCurrentTCB->uxCriticalNesting == 1 )
            {
                portASSERT_IF_IN_ISR();  // 非中断安全版本禁止在ISR调用
            }
        }
    }

#endif /* portCRITICAL_NESTING_IN_TCB */
/*-----------------------------------------------------------*/

/**
* @brief 退出临界区（嵌套计数存储在TCB）
* @details 减少临界区嵌套计数，当计数为零时重新启用中断
*/
#if ( portCRITICAL_NESTING_IN_TCB == 1 )

    void vTaskExitCritical( void )
    {
        if( xSchedulerRunning != pdFALSE )  // 调度器运行时处理
        {
            if( pxCurrentTCB->uxCriticalNesting > 0U )  // 验证有效嵌套计数
            {
                pxCurrentTCB->uxCriticalNesting--;  // 递减嵌套计数

                if( pxCurrentTCB->uxCriticalNesting == 0U )
                {
                    portENABLE_INTERRUPTS();  // 恢复中断使能
                }
            }
        }
    }

#endif /* portCRITICAL_NESTING_IN_TCB */
/*-----------------------------------------------------------*/

/**
* @brief 格式化任务名称到缓冲区
* @param pcBuffer 目标缓冲区指针
* @param pcTaskName 原始任务名称
* @return char* 格式化后的字符串结尾指针
* @details 将任务名称填充至固定长度(configMAX_TASK_NAME_LEN)便于列对齐
*/
#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )

    static char *prvWriteNameToBuffer( char *pcBuffer, const char *pcTaskName )
    {
    size_t x;

        /* 拷贝原始任务名称 */
        strcpy( pcBuffer, pcTaskName );  // 包括终止符

        /* 填充空格保证固定列宽 */
        for( x = strlen( pcBuffer ); x < ( size_t ) ( configMAX_TASK_NAME_LEN - 1 ); x++ )
        {
            pcBuffer[ x ] = ' ';  // 填充空格对齐
        }

        pcBuffer[ x ] = 0x00;  // 添加终止符

        return &( pcBuffer[ x ] );  // 返回字符串结尾指针
    }

#endif /* 跟踪和统计配置检查 */

/*-----------------------------------------------------------*/
/**
* @brief 生成任务状态信息表格字符串
* @param pcWriteBuffer 输出缓冲区指针（需预分配足够空间）
* @details 该函数生成包含以下列的可读表格：
*          任务名称 | 状态 | 当前优先级 | 栈高水位线 | 任务编号
* @warning 该函数依赖sprintf函数，可能导致代码膨胀
* @note 需同时启用configUSE_TRACE_FACILITY和configUSE_STATS_FORMATTING_FUNCTIONS
*/
#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )

void vTaskList( char * pcWriteBuffer )
{
TaskStatus_t *pxTaskStatusArray;    // 任务状态数组指针
volatile UBaseType_t uxArraySize, x; // 数组尺寸和循环变量
char cStatus;                      // 状态字符标识

    /* 初始化缓冲区（安全处理） */
    *pcWriteBuffer = 0x00;  // 设置空终止符防止垃圾数据

    /* 获取当前任务数量快照（防止处理过程中数量变化） */
    uxArraySize = uxCurrentNumberOfTasks;

    /* 动态分配任务状态数组内存 */
    pxTaskStatusArray = pvPortMalloc( uxCurrentNumberOfTasks * sizeof( TaskStatus_t ) );

    if( pxTaskStatusArray != NULL )
    {
        /* 获取系统任务状态数据 */
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );

        /* 遍历所有任务生成表格行 */
        for( x = 0; x < uxArraySize; x++ )
        {
            /* 转换任务状态为可读字符 */
            switch( pxTaskStatusArray[ x ].eCurrentState )
            {
                case eReady:     cStatus = tskREADY_CHAR;     // 'R'
                                 break;

                case eBlocked:   cStatus = tskBLOCKED_CHAR;   // 'B'
                                 break;

                case eSuspended: cStatus = tskSUSPENDED_CHAR; // 'S' 
                                 break;

                case eDeleted:   cStatus = tskDELETED_CHAR;  // 'D'
                                 break;

                default:         cStatus = 0x00;  // 无效状态保护
                                 break;
            }

            /* 格式化任务名称（固定列宽对齐） */
            pcWriteBuffer = prvWriteNameToBuffer( pcWriteBuffer, 
                                                 pxTaskStatusArray[ x ].pcTaskName );

            /* 生成表格行数据（状态|优先级|栈|任务号） */
            sprintf( pcWriteBuffer, 
                   "\t%c\t%u\t%u\t%u\r\n",  // 制表符分隔格式
                   cStatus,  // 状态字符
                   ( unsigned int ) pxTaskStatusArray[ x ].uxCurrentPriority, // 当前优先级
                   ( unsigned int ) pxTaskStatusArray[ x ].usStackHighWaterMark, // 栈高水位
                   ( unsigned int ) pxTaskStatusArray[ x ].xTaskNumber ); // 任务编号

            /* 移动缓冲区指针到新行末尾 */
            pcWriteBuffer += strlen( pcWriteBuffer );
        }

        /* 释放任务状态数组内存 */
        vPortFree( pxTaskStatusArray );
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();  // 内存分配失败处理标记
    }
}

#endif /* 跟踪和统计配置检查 */

/*----------------------------------------------------------*/
/**
* @brief 生成任务运行时间统计信息
* @param pcWriteBuffer 输出缓冲区指针（需预分配足够空间）
* @details 该函数生成包含以下列的可读表格：
*          任务名称 | 运行时间(ticks) | 运行时间占比
* @warning 需同时启用configGENERATE_RUN_TIME_STATS和configUSE_STATS_FORMATTING_FUNCTIONS
* @note 依赖sprintf函数，可能造成代码膨胀，建议生产系统直接使用uxTaskGetSystemState
*/
#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )

void vTaskGetRunTimeStats( char *pcWriteBuffer )
{
TaskStatus_t *pxTaskStatusArray;     // 任务状态数组指针
volatile UBaseType_t uxArraySize, x;  // 数组尺寸和循环变量
uint32_t ulTotalTime, ulStatsAsPercentage; // 总运行时间和百分比

    /* 编译时检查必须启用跟踪设施 */
    #if( configUSE_TRACE_FACILITY != 1 )
    {
        #error 必须定义configUSE_TRACE_FACILITY=1以使用本函数
    }
    #endif

    /* 初始化缓冲区（防止残留数据） */
    *pcWriteBuffer = 0x00;

    /* 获取当前任务数量快照（防止处理期间变化） */
    uxArraySize = uxCurrentNumberOfTasks;

    /* 动态分配任务状态数组内存 */
    pxTaskStatusArray = pvPortMalloc( uxCurrentNumberOfTasks * sizeof( TaskStatus_t ) );

    if( pxTaskStatusArray != NULL )
    {
        /* 获取系统任务状态数据及总运行时间 */
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalTime );

        /* 将总时间转换为百分比基数（预除以100） */
        ulTotalTime /= 100UL;

        /* 处理有效总时间（避免除零错误） */
        if( ulTotalTime > 0 )
        {
            /* 遍历所有任务生成统计信息 */
            for( x = 0; x < uxArraySize; x++ )
            {
                /* 计算运行时间百分比（向下取整） */
                ulStatsAsPercentage = pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalTime;

                /* 格式化任务名称（固定列宽对齐） */
                pcWriteBuffer = prvWriteNameToBuffer( pcWriteBuffer, pxTaskStatusArray[ x ].pcTaskName );

                /* 根据百分比值选择输出格式 */
                if( ulStatsAsPercentage > 0UL )
                {
                    /* 完整百分比输出 */
                    #ifdef portLU_PRINTF_SPECIFIER_REQUIRED  // 需要长整型格式符
                    {
                        sprintf( pcWriteBuffer, "\t%lu\t\t%lu%%\r\n", 
                               pxTaskStatusArray[ x ].ulRunTimeCounter, // 原始运行时间
                               ulStatsAsPercentage );                 // 百分比
                    }
                    #else  // 使用普通整型格式符
                    {
                        sprintf( pcWriteBuffer, "\t%u\t\t%u%%\r\n",
                               ( unsigned int ) pxTaskStatusArray[ x ].ulRunTimeCounter,
                               ( unsigned int ) ulStatsAsPercentage );
                    }
                    #endif
                }
                else
                {
                    /* 小于1%的特殊处理 */
                    #ifdef portLU_PRINTF_SPECIFIER_REQUIRED
                    {
                        sprintf( pcWriteBuffer, "\t%lu\t\t<1%%\r\n", 
                               pxTaskStatusArray[ x ].ulRunTimeCounter );
                    }
                    #else
                    {
                        sprintf( pcWriteBuffer, "\t%u\t\t<1%%\r\n",
                               ( unsigned int ) pxTaskStatusArray[ x ].ulRunTimeCounter );
                    }
                    #endif
                }

                /* 移动缓冲区指针到新行末尾 */
                pcWriteBuffer += strlen( pcWriteBuffer );
            }
        }

        /* 释放动态分配的内存 */
        vPortFree( pxTaskStatusArray );
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();  // 内存分配失败处理标记
    }
}

#endif /* 运行统计配置检查 */

/*-----------------------------------------------------------*/
/**
* @brief 重置任务事件列表项值并返回原始值
* @return TickType_t 原始事件列表项值
* @details 用于任务从事件列表移除时恢复默认优先级配置
*/
TickType_t uxTaskResetEventItemValue( void )
{
TickType_t uxReturn;

    /* 获取当前事件列表项值 */
    uxReturn = listGET_LIST_ITEM_VALUE( &( pxCurrentTCB->xEventListItem ) );

    /* 重置为默认优先级相关值（数值越小优先级越高） */
    listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xEventListItem ),
        ( ( TickType_t ) configMAX_PRIORITIES - 
          ( TickType_t ) pxCurrentTCB->uxPriority ) ); 

    return uxReturn;
}

/*-----------------------------------------------------------*/

/**
* @brief 递增当前任务的互斥量持有计数
* @return void* 当前任务控制块指针
* @details 用于互斥量获取时跟踪持有数量，支持优先级继承
*/
#if ( configUSE_MUTEXES == 1 )

    void *pvTaskIncrementMutexHeldCount( void )
    {
        /* 处理任务创建前的调用情况（安全保护） */
        if( pxCurrentTCB != NULL )
        {
            pxCurrentTCB->uxMutexesHeld++;  // 原子操作递增计数器
        }

        return pxCurrentTCB;  // 返回TCB指针用于后续操作
    }

#endif /* configUSE_MUTEXES */
/*-----------------------------------------------------------*/

/**
* @brief 等待并获取任务通知值
* @param xClearCountOnExit 退出时清除方式：pdTRUE清零/pdFALSE减一
* @param xTicksToWait 最大等待时间（单位：节拍）
* @return uint32_t 获取到的通知值
* @details 该函数实现类似二值信号量的等待机制
*/
#if( configUSE_TASK_NOTIFICATIONS == 1 )

    uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
    {
    uint32_t ulReturn;

        taskENTER_CRITICAL();
        {
            /* 检查当前是否有可用通知 */
            if( pxCurrentTCB->ulNotifiedValue == 0UL )
            {
                /* 标记任务进入通知等待状态 */
                pxCurrentTCB->ucNotifyState = taskWAITING_NOTIFICATION;

                /* 处理非零超时的情况 */
                if( xTicksToWait > 0 )
                {
                    /* 将任务加入延迟列表 */
                    prvAddCurrentTaskToDelayedList( xTicksToWait, pdTRUE );
                    traceTASK_NOTIFY_TAKE_BLOCK();  // 调试跟踪

                    /* 在临界区内主动触发任务切换 */
                    portYIELD_WITHIN_API();  // 架构相关的切换实现
                }
            }
        }
        taskEXIT_CRITICAL();

        taskENTER_CRITICAL();
        {
            traceTASK_NOTIFY_TAKE();  // 调试跟踪
            ulReturn = pxCurrentTCB->ulNotifiedValue;

            /* 处理获取到的通知值 */
            if( ulReturn != 0UL )
            {
                /* 根据参数清除通知计数 */
                if( xClearCountOnExit != pdFALSE )
                {
                    pxCurrentTCB->ulNotifiedValue = 0UL;  // 清零模式
                }
                else
                {
                    pxCurrentTCB->ulNotifiedValue = ulReturn - 1;  // 减一模式
                }
            }

            /* 重置任务通知状态 */
            pxCurrentTCB->ucNotifyState = taskNOT_WAITING_NOTIFICATION;
        }
        taskEXIT_CRITICAL();

        return ulReturn;
    }

#endif /* configUSE_TASK_NOTIFICATIONS */

/*-----------------------------------------------------------*/
#if( configUSE_TASK_NOTIFICATIONS == 1 )

/**
 * @brief 任务通知等待函数
 * @功能 允许任务等待来自其他任务或中断的通知，可设置超时时间
 * @param ulBitsToClearOnEntry 进入前要清除的通知位掩码（按位取反后与通知值做与运算）
 * @param ulBitsToClearOnExit 退出前要清除的通知位掩码（在返回前应用）
 * @param pulNotificationValue 输出参数，用于返回接收到的通知值
 * @param xTicksToWait 最大等待时间（单位：时钟节拍），使用portMAX_DELAY表示无限等待
 * @return pdTRUE表示收到通知，pdFALSE表示超时或通知未到达
 */
BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, 
                           uint32_t ulBitsToClearOnExit,
                           uint32_t *pulNotificationValue,
                           TickType_t xTicksToWait )
{
BaseType_t xReturn;

    // 进入临界区保护共享资源（任务控制块TCB）
    taskENTER_CRITICAL();
    {
        /* 仅在通知未挂起时阻塞任务 */
        if( pxCurrentTCB->ucNotifyState != taskNOTIFICATION_RECEIVED )
        {
            /* 清除指定通知位（按位取反后与操作）
               可能在等待期间被其他任务或中断设置新值 */
            pxCurrentTCB->ulNotifiedValue &= ~ulBitsToClearOnEntry;

            /* 设置任务状态为"等待通知"状态
               taskWAITING_NOTIFICATION = 1（见task.h定义） */
            pxCurrentTCB->ucNotifyState = taskWAITING_NOTIFICATION;

            if( xTicksToWait > ( TickType_t ) 0 )
            {
                /* 将当前任务添加到延迟列表
                   第二个参数pdTRUE表示使用相对时间 */
                prvAddCurrentTaskToDelayedList( xTicksToWait, pdTRUE );
                // 调试追踪：任务进入阻塞状态等待通知
                traceTASK_NOTIFY_WAIT_BLOCK();

                /* 主动触发任务切换（可能立即切换或退出临界区后切换）
                   这行代码会强制进行上下文切换 */
                portYIELD_WITHIN_API();
            }
            else
            {
                /* 用于测试覆盖率，无实际功能
                   当xTicksToWait=0时会执行此处 */
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            /* 当通知已存在时执行此处（测试覆盖率标记） */
            mtCOVERAGE_TEST_MARKER();
        }
    }
    // 退出临界区（恢复中断状态）
    taskEXIT_CRITICAL();

    // 再次进入临界区处理通知结果
    taskENTER_CRITICAL();
    {
        // 调试追踪：任务结束等待通知状态
        traceTASK_NOTIFY_WAIT();

        if( pulNotificationValue != NULL )
        {
            /* 输出当前通知值（可能被其他任务修改过）
               无论是否收到新通知都会返回当前值 */
            *pulNotificationValue = pxCurrentTCB->ulNotifiedValue;
        }

        /* 判断通知接收状态：
           taskWAITING_NOTIFICATION = 超时未收到
           其他状态 = 已收到通知 */
        if( pxCurrentTCB->ucNotifyState == taskWAITING_NOTIFICATION )
        {
            /* 未收到通知（超时情况） */
            xReturn = pdFALSE;
        }
        else
        {
            /* 成功收到通知：
               1. 清除退出前指定的通知位
               2. 返回成功状态 */
            pxCurrentTCB->ulNotifiedValue &= ~ulBitsToClearOnExit;
            xReturn = pdTRUE;
        }

        /* 重置任务通知状态为"未等待通知"
           taskNOT_WAITING_NOTIFICATION = 0 */
        pxCurrentTCB->ucNotifyState = taskNOT_WAITING_NOTIFICATION;
    }
    taskEXIT_CRITICAL();

    return xReturn;
}

#endif /* configUSE_TASK_NOTIFICATIONS */

/*-----------------------------------------------------------*/
#if( configUSE_TASK_NOTIFICATIONS == 1 )

/**
 * @brief 通用任务通知发送函数
 * @功能 向指定任务发送通知并更新其通知值，支持多种通知操作方式
 * @param xTaskToNotify 目标任务句柄（接收通知的任务）
 * @param ulValue 通知数值（具体含义根据eAction变化）
 * @param eAction 通知操作类型（位设置/增量/覆盖等）
 * @param pulPreviousNotificationValue 输出参数，返回发送前的通知值
 * @return pdPASS操作成功，pdFAIL仅在eSetValueWithoutOverwrite且通知已存在时返回失败
 */
BaseType_t xTaskGenericNotify( TaskHandle_t xTaskToNotify, 
                              uint32_t ulValue, 
                              eNotifyAction eAction,
                              uint32_t *pulPreviousNotificationValue )
{
TCB_t * pxTCB;          // 目标任务控制块指针
BaseType_t xReturn = pdPASS;  // 默认返回成功
uint8_t ucOriginalNotifyState; // 原始通知状态缓存

    // 断言确保目标任务句柄有效
    configASSERT( xTaskToNotify );
    // 将任务句柄转换为TCB结构指针
    pxTCB = ( TCB_t * ) xTaskToNotify;

    // 进入临界区保护共享资源
    taskENTER_CRITICAL();
    {
        // 如果要求返回先前值，则保存当前通知值
        if( pulPreviousNotificationValue != NULL )
        {
            *pulPreviousNotificationValue = pxTCB->ulNotifiedValue;
        }

        // 保存原始通知状态（用于后续状态判断）
        ucOriginalNotifyState = pxTCB->ucNotifyState;
        // 强制设置任务状态为"已接收通知"
        pxTCB->ucNotifyState = taskNOTIFICATION_RECEIVED;

        // 根据通知操作类型处理通知值
        switch( eAction )
        {
            case eSetBits :  // 按位设置模式
                pxTCB->ulNotifiedValue |= ulValue;
                break;

            case eIncrement : // 数值递增模式
                ( pxTCB->ulNotifiedValue )++;
                break;

            case eSetValueWithOverwrite : // 强制覆盖模式
                pxTCB->ulNotifiedValue = ulValue;
                break;

            case eSetValueWithoutOverwrite : // 非覆盖模式
                /* 仅在任务未处于已接收状态时更新值 */
                if( ucOriginalNotifyState != taskNOTIFICATION_RECEIVED )
                {
                    pxTCB->ulNotifiedValue = ulValue;
                }
                else
                {
                    /* 当通知已存在时返回失败（不覆盖已有通知） */
                    xReturn = pdFAIL;
                }
                break;

            case eNoAction: // 仅发送通知不更新值
                /* 仅改变任务状态，不修改通知值 */
                break;
        }

        // 调试追踪：任务通知事件发生
        traceTASK_NOTIFY();

        /* 如果目标任务正处于"等待通知"的阻塞状态 */
        if( ucOriginalNotifyState == taskWAITING_NOTIFICATION )
        {
            // 从等待列表中移除任务
            ( void ) uxListRemove( &( pxTCB->xStateListItem ) );
            // 将任务添加到就绪列表
            prvAddTaskToReadyList( pxTCB );

            /* 验证事件列表项状态（不应存在于事件列表） */
            configASSERT( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL );

            #if( configUSE_TICKLESS_IDLE != 0 )
            {
                /* 在Tickless模式下重置下一个任务解除阻塞时间
                   确保系统能及时进入低功耗状态 */
                prvResetNextTaskUnblockTime();
            }
            #endif

            /* 优先级判断：如果目标任务优先级高于当前任务 */
            if( pxTCB->uxPriority > pxCurrentTCB->uxPriority )
            {
                /* 触发任务切换（抢占当前任务） */
                taskYIELD_IF_USING_PREEMPTION();
            }
            else
            {
                // 测试覆盖率标记（无实际功能）
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            // 测试覆盖率标记（非阻塞状态时的执行路径）
            mtCOVERAGE_TEST_MARKER();
        }
    }
    // 退出临界区（恢复中断状态）
    taskEXIT_CRITICAL();

    return xReturn;
}

#endif /* configUSE_TASK_NOTIFICATIONS */

/*-----------------------------------------------------------*/
#if( configUSE_TASK_NOTIFICATIONS == 1 )

/**
 * @brief 中断服务例程(ISR)中的通用任务通知函数
 * @功能 在中断上下文中向指定任务发送通知，支持多种通知操作方式
 * @param xTaskToNotify 目标任务句柄（接收通知的任务TCB）
 * @param ulValue 通知数值（具体含义根据eAction变化）
 * @param eAction 通知操作类型（位设置/增量/覆盖等）
 * @param pulPreviousNotificationValue 输出参数，返回发送前的通知值
 * @param pxHigherPriorityTaskWoken 输出参数，用于指示是否需要上下文切换
 * @return pdPASS操作成功，pdFAIL仅在eSetValueWithoutOverwrite且通知已存在时返回失败
 */
BaseType_t xTaskGenericNotifyFromISR( TaskHandle_t xTaskToNotify,
                                     uint32_t ulValue,
                                     eNotifyAction eAction,
                                     uint32_t *pulPreviousNotificationValue,
                                     BaseType_t *pxHigherPriorityTaskWoken )
{
TCB_t * pxTCB;                // 目标任务控制块指针
uint8_t ucOriginalNotifyState; // 原始通知状态缓存
BaseType_t xReturn = pdPASS;  // 默认返回成功
UBaseType_t uxSavedInterruptStatus; // 中断状态保存变量

    // 断言确保目标任务句柄有效
    configASSERT( xTaskToNotify );

    /* 验证中断优先级有效性（必须在允许调用API的中断优先级等级）
       该宏会检查当前中断优先级是否低于configMAX_SYSCALL_INTERRUPT_PRIORITY */
    portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

    // 将任务句柄转换为TCB结构指针
    pxTCB = ( TCB_t * ) xTaskToNotify;

    // 保存当前中断状态并禁用中断（进入临界区）
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        // 如果要求返回先前值，则保存当前通知值
        if( pulPreviousNotificationValue != NULL )
        {
            *pulPreviousNotificationValue = pxTCB->ulNotifiedValue;
        }

        // 保存原始通知状态（用于后续状态判断）
        ucOriginalNotifyState = pxTCB->ucNotifyState;
        // 强制设置任务状态为"已接收通知"
        pxTCB->ucNotifyState = taskNOTIFICATION_RECEIVED;

        // 根据通知操作类型处理通知值（与任务级版本逻辑相同）
        switch( eAction )
        {
            case eSetBits :  // 按位设置模式
                pxTCB->ulNotifiedValue |= ulValue;
                break;

            case eIncrement : // 数值递增模式
                ( pxTCB->ulNotifiedValue )++;
                break;

            case eSetValueWithOverwrite : // 强制覆盖模式
                pxTCB->ulNotifiedValue = ulValue;
                break;

            case eSetValueWithoutOverwrite : // 非覆盖模式
                /* 仅在任务未处于已接收状态时更新值 */
                if( ucOriginalNotifyState != taskNOTIFICATION_RECEIVED )
                {
                    pxTCB->ulNotifiedValue = ulValue;
                }
                else
                {
                    /* 当通知已存在时返回失败（不覆盖已有通知） */
                    xReturn = pdFAIL;
                }
                break;

            case eNoAction: // 仅发送通知不更新值
                /* 仅改变任务状态，不修改通知值 */
                break;
        }

        // 调试追踪：ISR中的任务通知事件
        traceTASK_NOTIFY_FROM_ISR();

        /* 如果目标任务正处于"等待通知"的阻塞状态 */
        if( ucOriginalNotifyState == taskWAITING_NOTIFICATION )
        {
            /* 验证事件列表项状态（不应存在于事件列表） */
            configASSERT( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL );

            /* 判断调度器是否挂起 */
            if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
            {
                // 常规情况：从阻塞列表移除并加入就绪列表
                ( void ) uxListRemove( &( pxTCB->xStateListItem ) );
                prvAddTaskToReadyList( pxTCB );
            }
            else
            {
                /* 调度器挂起时：将任务加入待处理就绪列表
                   xPendingReadyList会在调度器恢复时处理 */
                vListInsertEnd( &( xPendingReadyList ), &( pxTCB->xEventListItem ) );
            }

            /* 检查目标任务优先级是否高于当前任务 */
            if( pxTCB->uxPriority > pxCurrentTCB->uxPriority )
            {
                /* 通过参数通知调用者需要上下文切换 */
                if( pxHigherPriorityTaskWoken != NULL )
                {
                    *pxHigherPriorityTaskWoken = pdTRUE;
                }
                else
                {
                    /* 如果用户未提供参数，设置全局切换标志
                       在后续的portYIELD_FROM_ISR()中处理 */
                    xYieldPending = pdTRUE;
                }
            }
            else
            {
                // 测试覆盖率标记（无优先级提升时的路径）
                mtCOVERAGE_TEST_MARKER();
            }
        }
    }
    // 恢复原始中断状态（退出临界区）
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    return xReturn;
}

#endif /* configUSE_TASK_NOTIFICATIONS */

/*-----------------------------------------------------------*/
#if( configUSE_TASK_NOTIFICATIONS == 1 )

    /**
     * @brief 在中断服务例程(ISR)中直接给指定任务发送通知
     * @param xTaskToNotify 要通知的任务句柄
     * @param pxHigherPriorityTaskWoken 用于返回是否需要上下文切换的标记指针
     * 
     * 功能说明：
     * 1. 该函数用于ISR环境，通过递增任务的通知值来实现类似计数信号量的操作
     * 2. 如果目标任务正在阻塞等待通知，则会解除阻塞并将其加入就绪队列
     * 3. 当被通知任务优先级高于当前任务时，会设置上下文切换标志
     * 4. 必须确保在有效中断优先级中调用（通过portASSERT验证）
     * 5. 使用前需确保configUSE_TASK_NOTIFICATIONS配置为1
     */
    void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken )
    {
        TCB_t * pxTCB;                // 任务控制块指针
        uint8_t ucOriginalNotifyState; // 保存原始通知状态
        UBaseType_t uxSavedInterruptStatus; // 保存中断屏蔽状态

        /* 参数有效性验证：确保传入的任务句柄非空 */
        configASSERT( xTaskToNotify );

        /* 中断优先级验证：
           确保当前中断优先级不高于系统允许的最大API调用优先级
           防止在不可屏蔽中断中调用RTOS API */
        portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

        /* 将任务句柄转换为任务控制块结构 */
        pxTCB = ( TCB_t * ) xTaskToNotify;

        /* 保存当前中断屏蔽状态并禁用中断（进入临界区）*/
        uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
        {
            /* 保存任务原有通知状态，并更新为已接收状态 */
            ucOriginalNotifyState = pxTCB->ucNotifyState;
            pxTCB->ucNotifyState = taskNOTIFICATION_RECEIVED;

            /* 递增任务的通知值（模拟信号量give操作）*/
            ( pxTCB->ulNotifiedValue )++;

            /* 跟踪宏：用于调试/分析通知事件 */
            traceTASK_NOTIFY_GIVE_FROM_ISR();

            /* 检查任务是否处于等待通知的阻塞状态 */
            if( ucOriginalNotifyState == taskWAITING_NOTIFICATION )
            {
                /* 安全性验证：确保任务不在任何事件列表中 */
                configASSERT( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL );

                /* 根据调度器状态处理任务状态 */
                if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
                {
                    /* 调度器正常运行：将任务从阻塞列表移除并加入就绪列表 */
                    ( void ) uxListRemove( &( pxTCB->xStateListItem ) );
                    prvAddTaskToReadyList( pxTCB );
                }
                else
                {
                    /* 调度器挂起时：将任务添加到待处理就绪列表（延迟处理）*/
                    vListInsertEnd( &( xPendingReadyList ), &( pxTCB->xEventListItem ) );
                }

                /* 优先级检查：判断被通知任务是否高于当前任务 */
                if( pxTCB->uxPriority > pxCurrentTCB->uxPriority )
                {
                    /* 需要触发上下文切换 */
                    if( pxHigherPriorityTaskWoken != NULL )
                    {
                        *pxHigherPriorityTaskWoken = pdTRUE; // 设置外部标记
                    }
                    else
                    {
                        /* 未使用参数时设置全局挂起标记（兼容旧版本用法）*/
                        xYieldPending = pdTRUE;
                    }
                }
                else
                {
                    /* 覆盖测试标记：表示该分支在测试中不需要执行 */
                    mtCOVERAGE_TEST_MARKER();
                }
            }
        }
        /* 恢复中断屏蔽状态（退出临界区）*/
        portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
    }

#endif /* configUSE_TASK_NOTIFICATIONS */

/*-----------------------------------------------------------*/
#if( configUSE_TASK_NOTIFICATIONS == 1 )

    /**
     * @brief 清除指定任务的通知状态
     * @param xTask 要操作的任务句柄（传入NULL表示当前任务）
     * @return pdPASS-状态清除成功/pdFAIL-无待处理通知
     * 
     * 功能说明：
     * 1. 将任务的通知状态从taskNOTIFICATION_RECEIVED重置为taskNOT_WAITING_NOTIFICATION
     * 2. 通过临界区保护保证状态修改的原子性
     * 3. 当任务没有未处理的通知时返回失败状态
     * 4. 必须与configUSE_TASK_NOTIFICATIONS=1配合使用
     */
    BaseType_t xTaskNotifyStateClear( TaskHandle_t xTask )
    {
        TCB_t *pxTCB;        // 任务控制块指针
        BaseType_t xReturn;  // 操作结果返回值

        /* 任务句柄转换逻辑：
           当传入NULL时，通过prvGetTCBFromHandle获取当前任务控制块
           该宏实际调用xTaskGetCurrentTaskHandle()获取当前任务句柄 */
        pxTCB = prvGetTCBFromHandle( xTask );

        /* 进入临界区（禁用中断）确保状态操作的原子性 */
        taskENTER_CRITICAL();
        {
            /* 检查当前通知状态是否为"已接收"状态 */
            if( pxTCB->ucNotifyState == taskNOTIFICATION_RECEIVED )
            {
                /* 重置通知状态为"非等待通知"状态 */
                pxTCB->ucNotifyState = taskNOT_WAITING_NOTIFICATION;
                xReturn = pdPASS;  // 设置成功返回值
            }
            else
            {
                /* 当前状态无需改变时返回失败 */
                xReturn = pdFAIL;  // 设置失败返回值
            }
        }
        taskEXIT_CRITICAL();  // 退出临界区（恢复中断）

        return xReturn;  // 返回操作结果
    }

#endif /* configUSE_TASK_NOTIFICATIONS */

/*-----------------------------------------------------------*/

/**
 * @brief 将当前任务添加到延时或挂起任务列表
 * @param xTicksToWait 等待的时钟节拍数
 * @param xCanBlockIndefinitely 是否允许无限期阻塞标志
 * 
 * 功能说明：
 * 1. 管理当前任务从就绪态到阻塞态的转换
 * 2. 处理无限阻塞情况下的挂起列表管理
 * 3. 自动处理系统节拍计数器溢出情况
 * 4. 维护下一个任务唤醒时间的优化检测点
 */
static void prvAddCurrentTaskToDelayedList( TickType_t xTicksToWait, const BaseType_t xCanBlockIndefinitely )
{
    TickType_t xTimeToWake;                     // 任务预期唤醒时间
    const TickType_t xConstTickCount = xTickCount; // 获取当前系统节拍快照

    /* 任务中止延迟功能支持 */
    #if( INCLUDE_xTaskAbortDelay == 1 )
    {
        /* 准备进入延迟状态前重置中止标志
           确保能从阻塞状态正确检测到中止请求 */
        pxCurrentTCB->ucDelayAborted = pdFALSE;
    }
    #endif

    /* 从就绪列表移除当前任务（状态列表项复用机制）*/
    if( uxListRemove( &( pxCurrentTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
    {
        /* 当任务从非空就绪列表移除时，重置就绪优先级位图 */
        portRESET_READY_PRIORITY( pxCurrentTCB->uxPriority, uxTopReadyPriority );
    }
    else
    {
        /* 测试覆盖率标记：表示该分支在测试中不需要执行 */
        mtCOVERAGE_TEST_MARKER();
    }

    /* 任务挂起功能支持 */
    #if ( INCLUDE_vTaskSuspend == 1 )
    {
        /* 处理无限期阻塞情况 */
        if( ( xTicksToWait == portMAX_DELAY ) && ( xCanBlockIndefinitely != pdFALSE ) )
        {
            /* 将任务添加到挂起任务列表（不受时钟事件唤醒）*/
            vListInsertEnd( &xSuspendedTaskList, &( pxCurrentTCB->xStateListItem ) );
        }
        else  /* 有限时间阻塞处理 */
        {
            /* 计算绝对唤醒时间（考虑节拍计数器溢出）*/
            xTimeToWake = xConstTickCount + xTicksToWait;

            /* 设置列表项排序依据值 */
            listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xStateListItem ), xTimeToWake );

            /* 处理节拍计数器溢出情况 */
            if( xTimeToWake < xConstTickCount )  // 发生溢出
            {
                /* 插入溢出延迟列表（系统节拍高位不同）*/
                vListInsert( pxOverflowDelayedTaskList, &( pxCurrentTCB->xStateListItem ) );
            }
            else  // 正常延迟处理
            {
                /* 插入常规延迟列表（按唤醒时间排序）*/
                vListInsert( pxDelayedTaskList, &( pxCurrentTCB->xStateListItem ) );

                /* 更新最近唤醒时间优化点 */
                if( xTimeToWake < xNextTaskUnblockTime )
                {
                    xNextTaskUnblockTime = xTimeToWake;  // 刷新最近唤醒时间
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
                }
            }
        }
    }
    #else /* 无任务挂起功能版本 */
    {
        /* 计算绝对唤醒时间（强制有限阻塞）*/
        xTimeToWake = xConstTickCount + xTicksToWait;
        listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xStateListItem ), xTimeToWake );

        /* 溢出处理 */
        if( xTimeToWake < xConstTickCount )
        {
            vListInsert( pxOverflowDelayedTaskList, &( pxCurrentTCB->xStateListItem ) );
        }
        else
        {
            vListInsert( pxDelayedTaskList, &( pxCurrentTCB->xStateListItem ) );
            
            /* 更新最近唤醒时间 */
            if( xTimeToWake < xNextTaskUnblockTime )
            {
                xNextTaskUnblockTime = xTimeToWake;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }

        /* 消除未使用参数警告（兼容不同配置）*/
        ( void ) xCanBlockIndefinitely;
    }
    #endif /* INCLUDE_vTaskSuspend */
}


// 3.初始化资源控制块
/*
 * 资源控制块初始化函数
 * 功能：
 *   1. 初始化资源控制块的核心数据结构
 *   2. 建立资源与任务调度系统的关联关系
 *   3. 准备资源管理所需的基础设施
 * 参数：
 *   pxResource - 待初始化的资源控制块指针
 *   uxStage    - 资源的初始阶段标识
 * 注意：
 *   - 必须在资源创建时调用（在任务开始调度前或临界区内）
 *   - 每个资源控制块只需初始化一次
 */
void vInitializeResourceCB(ResourceCB_t *pxResource, UBaseType_t uxStage)
{
    /* 初始化资源等待队列（List_t结构）
     * 操作内容：
     * 1. 设置列表头节点的前后指针指向自身
     * 2. 重置列表项计数器uxNumberOfItems为0
     * 3. 初始化列表结束标记pxIndex
     * 效果：创建空等待队列 */
    vListInitialise(&(pxResource->xTasksWaitingForResource));

    /* 初始化资源列表项（ListItem_t结构）
     * 操作内容：
     * 1. 设置列表项排序值xItemValue为portMAX_DELAY
     * 2. 重置pvOwner和pvContainer为NULL
     * 3. 初始化相邻指针指向自身
     * 效果：创建独立的列表项，等待插入资源链表 */
    vListInitialiseItem(&(pxResource->xResourceListItem));

    /* 设置资源初始持有状态
     * 安全措施：
     * - 确保资源创建时处于未持有状态
     * - 防止未初始化指针导致非法访问
     * - 与TCB的xResourcesList形成空关联 */
    pxResource->pxCurrentHolder = NULL;

    /* 配置资源阶段属性
     * 参数说明：
     * uxStage通常来自：
     * - 系统初始化配置
     * - 阶段管理器的分配
     * - 资源的创建参数
     * 后续影响：
     * 决定哪些阶段的任务可以访问本资源 */
    pxResource->uxResourceStage = uxStage;

    /* 建立资源块与列表项的所有权关系
     * 实现原理：
     * 将资源控制块指针存入列表项的pvOwner
     * 作用：
     * - 当资源被加入任务资源列表时，可通过列表项反向索引到资源块
     * - 支持快速资源定位（如从xResourceListItem找到pxResource） */
    listSET_LIST_ITEM_OWNER(&(pxResource->xResourceListItem), pxResource);
}

//4.初始化任务的阶段优先级
/*
 * 任务阶段优先级初始化函数
 * 功能：
 *   1. 建立任务的多阶段优先级体系
 *   2. 初始化阶段继承机制相关数据结构
 *   3. 准备任务资源管理基础设施
 * 参数：
 *   xTask         - 目标任务句柄（NULL表示当前任务）
 *   uxStages      - 总阶段数量（必须>0）
 *   puxPriorities - 阶段优先级数组指针（需预先配置）
 * 注意：
 *   - 必须在任务开始调度前调用
 *   - 每个任务只能初始化一次阶段优先级
 *   - 内存分配使用FreeRTOS的pvPortMalloc/vPortFree
 */
void vTaskInitStagedPriorities(TaskHandle_t xTask, UBaseType_t uxStages, UBaseType_t *puxPriorities)
{
    TCB_t *pxTCB;
    
    /* 动态任务句柄处理
     * 处理逻辑：
     * - 允许对任意任务或当前任务进行配置
     * - xTaskGetCurrentTaskHandle()在调度器运行后有效
     * 安全机制：
     * 避免直接暴露TCB结构，通过句柄进行类型转换 */
    if (xTask == NULL)
        pxTCB = (TCB_t *)xTaskGetCurrentTaskHandle();
    else
        pxTCB = (TCB_t *)xTask;
    
    /* 三层参数校验
     * 1. pxTCB有效性：防止非法任务控制块
     * 2. puxPriorities非空：确保优先级数组存在
     * 3. uxStages非零：避免创建0阶段的任务
     * 设计考虑：
     * 返回而不断言，增强系统容错能力 */
    if (pxTCB == NULL || puxPriorities == NULL || uxStages == 0)
    {
        return;
    }
    
    /* 进入临界区（保护TCB结构）
     * 必要性：
     * - 防止在初始化过程中发生任务切换
     * - 保证内存操作的原子性
     * - 确保阶段配置的完整性 */
    taskENTER_CRITICAL();
    {
        /* 内存安全处理
         * 重用检测机制：
         * - 允许重复初始化（先释放旧内存）
         * - 防止内存泄漏
         * 内存管理：
         * vPortFree遵循FreeRTOS内存释放规范 */
        if (pxTCB->puxStagePriorities != NULL)
        {
            vPortFree(pxTCB->puxStagePriorities);
            pxTCB->puxStagePriorities = NULL;
        }
        
        /* 阶段内存分配
         * 分配策略：
         * - 按阶段数量动态分配
         * - 每个元素存储UBaseType_t类型的优先级
         * 失败处理：
         * 通过后续NULL检查实现优雅降级 */
        pxTCB->puxStagePriorities = (UBaseType_t *)pvPortMalloc(uxStages * sizeof(UBaseType_t));
        
        if (pxTCB->puxStagePriorities != NULL)
        {
            /* 深拷贝优先级配置
             * 采用值拷贝而非指针引用：
             * - 保证阶段配置的独立性
             * - 允许外部数组被回收
             * - 避免竞态条件 */
            for (UBaseType_t i = 0; i < uxStages; i++)
            {
                pxTCB->puxStagePriorities[i] = puxPriorities[i];
            }
            
            /* 阶段系统初始化
             * uxTotalStages - 记录最大阶段索引（从1开始计数）
             * uxCurrentStage - 初始化阶段指针（从0开始）
             * 设计规范：
             * 阶段编号强制从0开始，确保数组访问安全 */
            pxTCB->uxTotalStages = uxStages;
            pxTCB->uxCurrentStage = 0;
            
            /* 优先级继承初始化
             * uxInheritedPriority - 初始化为基础优先级
             * ucInheritanceActive - 初始未激活继承
             * 状态同步：
             * 实际优先级=MAX(uxPriority, uxInheritedPriority) */
            pxTCB->uxInheritedPriority = pxTCB->uxPriority;
            pxTCB->ucInheritanceActive = pdFALSE;
            
            /* 资源管理系统初始化
             * 创建空资源链表：
             * - 初始化链表头节点
             * - 准备资源项插入点
             * 作用：
             * 记录任务当前持有的所有资源 */
            vListInitialise(&(pxTCB->xResourcesList));
        }
        else
        {
            /* 内存分配失败处理
             * 安全重置策略：
             * - 清空阶段计数器
             * - 重置继承状态
             * - 保持基础优先级不变
             * 系统影响：
             * 任务将退化为无阶段机制运行 */
            pxTCB->uxTotalStages = 0;
            pxTCB->uxCurrentStage = 0;
            pxTCB->uxInheritedPriority = pxTCB->uxPriority;
            pxTCB->ucInheritanceActive = pdFALSE;
        }
    }
    /* 退出临界区
     * 恢复中断状态：
     * - 若进入前中断已禁用，保持禁用状态
     * - 否则重新启用中断 */
    taskEXIT_CRITICAL();
}

//5.设置任务当前阶段
/*
 * 任务阶段切换函数
 * 功能：
 *   1. 动态切换任务的执行阶段
 *   2. 根据阶段配置调整任务优先级
 *   3. 触发必要的任务调度
 * 参数：
 *   xTask      - 目标任务句柄（NULL表示当前任务）
 *   uxNewStage - 目标阶段索引
 * 返回值：
 *   pdTRUE  - 阶段切换成功
 *   pdFALSE - 参数无效或操作失败
 * 注意：
 *   - 必须在调度器运行后调用
 *   - 可能引发立即任务调度
 */
BaseType_t xTaskSetCurrentStage(TaskHandle_t xTask, UBaseType_t uxNewStage)
{
    TCB_t *pxTCB;
    BaseType_t xReturn = pdFALSE;
    
    /* 动态任务上下文处理
     * 设计特点：
     * - 支持对任意任务或当前任务操作
     * - 通过句柄转换获取TCB指针
     * 安全机制：
     * 防止通过无效句柄访问内存 */
    if (xTask == NULL)
        pxTCB = (TCB_t *)xTaskGetCurrentTaskHandle();
    else
        pxTCB = (TCB_t *)xTask;
    
    /* 基础TCB有效性验证
     * 防御性编程措施：
     * 1. 过滤空指针访问
     * 2. 确保操作合法的任务控制块 */
    if (pxTCB == NULL)
    {
        return pdFALSE;
    }
    
    /* 进入临界区（保护任务状态）
     * 保护范围：
     * - 阶段配置数据
     * - 就绪列表操作
     * - 优先级修改 */
    taskENTER_CRITICAL();
    {
        /* 四重安全校验
         * 1. 阶段数组已初始化
         * 2. 新阶段在有效范围内
         * 3. 总阶段数合法
         * 4. 优先级配置存在 */
        if (pxTCB->puxStagePriorities != NULL && 
            uxNewStage < pxTCB->uxTotalStages && 
            pxTCB->uxTotalStages > 0)
        {
            /* 阶段优先级边界检查
             * 安全机制：
             * - 防止数组越界访问
             * - 校验优先级数值合法性 */
            UBaseType_t uxStagePriority = pxTCB->puxStagePriorities[uxNewStage];
            if (uxStagePriority >= configMAX_PRIORITIES)
            {
                taskEXIT_CRITICAL();
                return pdFALSE;
            }
            
            /* 更新阶段指针
             * 原子操作保证：
             * - 在临界区内完成状态切换
             * - 避免中间状态暴露 */
            pxTCB->uxCurrentStage = uxNewStage;
            
            /* 优先级更新条件判断
             * 继承机制优先级：
             * - 当继承激活时，实际优先级由继承机制控制
             * - 否则使用阶段配置优先级 */
            if (pxTCB->ucInheritanceActive == pdFALSE)
            {
                UBaseType_t uxOldPriority = pxTCB->uxPriority;
                if (uxOldPriority >= configMAX_PRIORITIES)
                {
                    taskEXIT_CRITICAL();
                    return pdFALSE;
                }
                
                /* 优先级变更检测 */
                if (uxOldPriority != uxStagePriority)
                {
                    BaseType_t xTaskInReadyList = pdFALSE;
                    
                    /* 就绪列表存在性检查
                     * 使用listIS_CONTAINED_WITHIN宏：
                     * 验证任务是否在对应优先级的就绪列表中 */
                    if (listIS_CONTAINED_WITHIN(&pxReadyTasksLists[uxOldPriority], 
                                             &(pxTCB->xStateListItem)) != pdFALSE)
                    {
                        xTaskInReadyList = pdTRUE;
                        /* 安全移除列表项
                         * uxListRemove返回值说明：
                         * 0表示列表变为空 */
                        if (uxListRemove(&(pxTCB->xStateListItem)) == 0U)
                        {
                            /* 更新调度器状态
                             * 清除对应优先级的就绪位：
                             * - 优化调度器查找效率
                             * - 保持状态一致性 */
                            taskRESET_READY_PRIORITY(uxOldPriority);
                        }
                    }
                    
                    /* 更新优先级体系
                     * 修改字段：
                     * - uxPriority：当前有效优先级
                     * - uxBasePriority：基础优先级（用于继承机制） */
                    pxTCB->uxPriority = uxStagePriority;
                    #if (configUSE_MUTEXES == 1)
                        pxTCB->uxBasePriority = uxStagePriority;
                    #endif
                    
                    /* 就绪状态恢复
                     * 条件：
                     * - 任务原本在就绪列表
                     * - 需要保持任务可调度状态 */
                    if (xTaskInReadyList == pdTRUE)
                    {
                        /* 内部函数调用：
                         * 1. 设置列表项价值为当前优先级
                         * 2. 插入到新优先级就绪列表
                         * 3. 更新调度器就绪位图 */
                        prvAddTaskToReadyList(pxTCB);
                    }
                }
            }
            
            xReturn = pdTRUE;
        }
    }
    taskEXIT_CRITICAL();
    
    /* 触发潜在调度
     * 策略：
     * - 当优先级变更影响调度顺序时
     * - 立即让出CPU（若在任务上下文中）
     * 注意：
     * portYIELD_WITHIN_API()在中断中无效 */
    if (xReturn == pdTRUE)
    {
        portYIELD_WITHIN_API();
    }
    
    return xReturn;
}


//6.更新任务优先级（内部函数）
/*
 * 任务优先级更新函数（内部）
 * 功能：
 *   1. 根据任务持有资源重新计算有效优先级
 *   2. 实现动态优先级继承机制
 *   3. 维护任务调度队列的准确性
 * 触发时机：
 *   - 任务释放资源时
 *   - 资源阶段变更时
 * 参数：
 *   pxTCB - 目标任务控制块
 * 注意：
 *   - 必须在临界区内调用
 *   - 可能引发立即任务调度
 */
static void prvUpdateTaskPriority(TCB_t *pxTCB)
{
    /* 初始化最高优先级为基准优先级
     * 作用：作为后续比较的初始值 */
    UBaseType_t uxHighestPriority = pxTCB->uxBasePriority;
    BaseType_t xPriorityChanged = pdFALSE;

    /* 资源持有状态检测
     * 原理：通过资源列表长度判断是否有资源持有
     * 优化：避免无资源时的多余计算 */
    if (listCURRENT_LIST_LENGTH(&(pxTCB->xResourcesList)) > 0)
    {
        ListItem_t *pxResourceListItem;
        ResourceCB_t *pxResource;
        /* 获取列表遍历终止标记
         * 作用：防止遍历越界 */
        const ListItem_t *pxResourceListEnd = listGET_END_MARKER(&(pxTCB->xResourcesList));

        /* 获取资源链表头节点
         * 注意：listGET_HEAD_ENTRY不修改列表结构 */
        pxResourceListItem = listGET_HEAD_ENTRY(&(pxTCB->xResourcesList));

        /* 资源遍历循环
         * 策略：前向遍历所有资源项 */
        while (pxResourceListItem != pxResourceListEnd)
        {
            /* 获取资源控制块
             * 安全机制：通过列表项所有者指针转换 */
            pxResource = (ResourceCB_t *)listGET_LIST_ITEM_OWNER(pxResourceListItem);
            
            /* 获取资源关联阶段索引 */
            UBaseType_t uxCurrentStageIndex = pxResource->uxResourceStage;

            /* 阶段有效性验证
             * 双重检查：
             * 1. 阶段索引在有效范围内
             * 2. 优先级数组已初始化 */
            if (uxCurrentStageIndex < pxTCB->uxTotalStages && pxTCB->puxStagePriorities != NULL)
            {
                /* 获取阶段配置优先级
                 * 内存安全：已通过索引校验 */
                UBaseType_t uxStagePriority = pxTCB->puxStagePriorities[uxCurrentStageIndex];
                
                /* 更新最高优先级
                 * 逻辑：取当前最高与阶段优先级的较大者 */
                if (uxStagePriority > uxHighestPriority)
                {
                    uxHighestPriority = uxStagePriority;
                }
            }

            /* 移动至下一资源项
             * 实现：通过列表节点指针遍历 */
            pxResourceListItem = listGET_NEXT(pxResourceListItem);
        }
    }

    /* 优先级变更检测
     * 比较：计算后的最高优先级与当前优先级 */
    if (uxHighestPriority != pxTCB->uxPriority)
    {
        /* 保存旧优先级用于队列操作 */
        UBaseType_t uxPreviousPriority = pxTCB->uxPriority;

        /* 更新任务优先级体系
         * 字段说明：
         * - uxPriority：当前生效优先级
         * - ucInheritanceActive：继承状态标记 */
        pxTCB->uxPriority = uxHighestPriority;
        pxTCB->ucInheritanceActive = (uxHighestPriority > pxTCB->uxBasePriority) ? pdTRUE : pdFALSE;

        /* 就绪状态处理
         * 状态检测：仅处理就绪状态任务 */
        if (eTaskGetState(pxTCB) == eReady)
        {
            /* 验证列表归属
             * 作用：确认任务确实在旧优先级队列 */
            if (listIS_CONTAINED_WITHIN(&pxReadyTasksLists[uxPreviousPriority], 
                                     &(pxTCB->xStateListItem)) != pdFALSE)
            {
                /* 从旧队列移除
                 * 副作用：可能更新就绪位图 */
                uxListRemove(&(pxTCB->xStateListItem));

                /* 插入新优先级队列
                 * 内部操作：
                 * 1. 设置列表项值
                 * 2. 更新就绪位图
                 * 3. 维护列表排序 */
                prvAddTaskToReadyList(pxTCB);
                xPriorityChanged = pdTRUE;
            }
        }
    }

    /* 触发潜在调度
     * 条件：
     * - 配置为可抢占调度
     * - 优先级发生实际变更 */
    if (xPriorityChanged == pdTRUE)
    {
        /* 调度器决策
         * 宏展开：
         * 若使用抢占式调度，执行portYIELD() */
        taskYIELD_IF_USING_PREEMPTION();
    }
}

//7.释放带阶段的互斥量
/*
 * 阶段互斥量释放函数
 * 功能：
 *   1. 释放带阶段属性的互斥量资源
 *   2. 解除任务与资源的关联关系
 *   3. 更新任务的优先级继承状态
 *   4. 唤醒等待该资源的最高优先级任务
 * 参数：
 *   xMutex - 阶段互斥量句柄
 * 返回值：
 *   pdTRUE  - 释放成功
 *   pdFALSE - 当前任务不持有该互斥量或参数错误
 * 注意：
 *   - 必须由持有互斥量的任务调用
 *   - 可能触发立即任务调度
 */
BaseType_t xSemaphoreGiveStagedMutex(SemaphoreHandle_t xMutex)
{
    /* 获取资源控制块
     * 实现细节：
     * - 通过自定义映射表关联信号量与资源块
     * - 返回NULL表示标准互斥量 */
    ResourceCB_t *pxResource = prvGetResourceCBFromSemaphore(xMutex);
    
    /* 获取当前任务控制块
     * 安全机制：
     * - 在调度器运行后始终有效
     * - 强制类型转换保持结构对齐 */
    TCB_t *pxCurrentTCB = (TCB_t *)xTaskGetCurrentTaskHandle();
    BaseType_t xReturn;

    /* 处理标准互斥量情况
     * 兼容性设计：
     * 当资源块不存在时退化为标准释放操作 */
    if (pxResource == NULL)
    {
        return xSemaphoreGive(xMutex);
    }

    /* 进入临界区（保护资源状态）
     * 保护范围：
     * - 资源持有者指针
     * - 任务资源列表
     * - 等待队列操作 */
    taskENTER_CRITICAL();
    {
        /* 验证资源所有权
         * 安全机制：
         * 防止任务释放未持有的互斥量 */
        if (pxResource->pxCurrentHolder == pxCurrentTCB)
        {
            /* 解除资源关联
             * 操作步骤：
             * 1. 从任务资源列表移除资源项
             * 2. 清空资源持有者指针 */
            uxListRemove(&(pxResource->xResourceListItem));
            pxResource->pxCurrentHolder = NULL;

            /* 更新任务优先级
             * 作用：
             * - 重新计算不含该资源的最高优先级
             * - 可能降低任务优先级 */
            prvUpdateTaskPriority(pxCurrentTCB);

            /* 处理等待队列
             * 策略：
             * - 唤醒最高优先级等待任务
             * - 仅移除列表项，实际唤醒由信号量操作完成 */
            if (listLIST_IS_EMPTY(&(pxResource->xTasksWaitingForResource)) == pdFALSE)
            {
                ListItem_t *pxWaitingTaskItem = listGET_HEAD_ENTRY(&(pxResource->xTasksWaitingForResource));
                uxListRemove(pxWaitingTaskItem);
            }

            taskEXIT_CRITICAL();
            
            /* 执行底层信号量释放
             * 注意：
             * 在临界区外调用标准API，避免嵌套问题 */
            xReturn = xSemaphoreGive(xMutex);
        }
        else
        {
            /* 错误处理路径
             * 当前任务不持有该互斥量 */
            taskEXIT_CRITICAL();
            xReturn = pdFALSE;
        }
    }

    /* 触发潜在调度
     * 条件：
     * - 优先级发生变更
     * - 有更高优先级任务就绪 */
    if (xReturn == pdTRUE)
    {
        portYIELD_WITHIN_API();
    }

    return xReturn;
}

//8. 初始化资源映射系统（应在系统启动时调用）
/*
 * 资源映射系统初始化函数
 * 功能：
 *   1. 创建资源映射管理所需的互斥量
 *   2. 确保资源映射操作的线程安全性
 * 调用时机：
 *   - 必须在调度器启动前调用
 *   - 建议在系统初始化阶段调用
 * 注意：
 *   - 重复调用不会重复初始化
 *   - 必须成功初始化后才能进行资源创建操作
 */
void vInitializeResourceMapping(void)
{
    /* 互斥量存在性检查
     * 安全机制：
     * - 避免重复创建互斥量
     * - 保证系统全局唯一性 */
    if (xResourceMappingMutex == NULL)
    {
        /* 互斥量创建
         * 特性：
         * - 优先级继承协议
         * - 内存分配：使用FreeRTOS堆管理器
         * - 安全等级：内核可管理对象
         * 作用：
         * - 保护资源映射链表(pxResourceMappingList)
         * - 序列化资源创建/删除操作 */
        xResourceMappingMutex = xSemaphoreCreateMutex();
        
        /* 调试断言（生产环境可选）
         * 验证互斥量创建成功 */
        configASSERT(xResourceMappingMutex != NULL);
    }
}

//9. 添加资源映射
/*
 * 资源映射添加函数（内部）
 * 功能：
 *   1. 建立信号量与资源控制块的映射关系
 *   2. 维护全局资源映射链表的完整性
 * 参数：
 *   xSemaphore   - 已创建的信号量句柄
 *   pxResourceCB - 关联的资源控制块指针
 * 返回值：
 *   pdTRUE  - 添加成功
 *   pdFAIL   - 参数无效或内存不足
 * 注意：
 *   - 必须在线程安全环境下调用
 *   - 需先初始化资源映射系统(vInitializeResourceMapping)
 */
static BaseType_t prvAddResourceMapping(SemaphoreHandle_t xSemaphore, ResourceCB_t *pxResourceCB)
{
    ResourceMapping_t *pxNewMapping;
    
    /* 参数有效性验证
     * 防御性编程：
     * 1. 防止空指针访问
     * 2. 避免创建无效映射 */
    if (xSemaphore == NULL || pxResourceCB == NULL)
    {
        return pdFAIL;
    }
    
    /* 动态内存分配
     * 内存来源：FreeRTOS堆
     * 分配策略：精确尺寸分配
     * 失败处理：立即返回错误 */
    pxNewMapping = (ResourceMapping_t *)pvPortMalloc(sizeof(ResourceMapping_t));
    if (pxNewMapping == NULL)
    {
        return pdFAIL;
    }
    
    /* 映射节点初始化
     * 字段说明：
     * xSemaphore：作为查找键(key)
     * pxResourceCB：映射值(value)
     * pxNext：链表维护指针 */
    pxNewMapping->xSemaphore = xSemaphore;
    pxNewMapping->pxResourceCB = pxResourceCB;
    pxNewMapping->pxNext = NULL;
    
    /* 临界区保护（链表操作）
     * 阻塞策略：永久阻塞直到获取锁
     * 锁类型：互斥锁（优先级继承） */
    if (xResourceMappingMutex != NULL)
    {
        xSemaphoreTake(xResourceMappingMutex, portMAX_DELAY);
    }
    
    /* 链表头插法操作
     * 操作步骤：
     * 1. 新节点指向原链表头
     * 2. 更新全局链表头指针
     * 时间复杂度：O(1) */
    pxNewMapping->pxNext = pxResourceMappingList;
    pxResourceMappingList = pxNewMapping;
    
    /* 释放互斥锁
     * 注意：必须与Take成对调用
     * 即使后续操作失败也必须释放 */
    if (xResourceMappingMutex != NULL)
    {
        xSemaphoreGive(xResourceMappingMutex);
    }
    
    return pdTRUE;
}


//10. 移除资源映射
/*
 * 资源映射移除函数（内部）
 * 功能：
 *   1. 解除信号量与资源控制块的关联
 *   2. 清理资源管理系统的内存占用
 * 参数：
 *   xSemaphore - 要移除的互斥量/信号量句柄
 * 返回值：
 *   pdTRUE  - 成功找到并移除映射
 *   pdFAIL   - 参数无效或未找到映射
 * 注意：
 *   - 必须与prvAddResourceMapping成对使用
 *   - 会同步释放资源控制块内存
 */
static BaseType_t prvRemoveResourceMapping(SemaphoreHandle_t xSemaphore)
{
    ResourceMapping_t *pxCurrent = pxResourceMappingList;
    ResourceMapping_t *pxPrevious = NULL;
    BaseType_t xFound = pdFALSE;
    
    /* 参数有效性校验
     * 防御机制：
     * 过滤空指针导致的无效操作 */
    if (xSemaphore == NULL)
    {
        return pdFAIL;
    }
    
    /* 进入临界区（保护链表遍历）
     * 阻塞策略：永久等待直到获取锁
     * 锁类型：互斥量（带优先级继承） */
    if (xResourceMappingMutex != NULL)
    {
        xSemaphoreTake(xResourceMappingMutex, portMAX_DELAY);
    }
    
    /* 链表遍历算法
     * 策略：顺序查找（时间复杂度O(n)）
     * 优化：头节点特殊处理 */
    while (pxCurrent != NULL)
    {
        if (pxCurrent->xSemaphore == xSemaphore)
        {
            /* 链表节点移除操作
             * 场景处理：
             * 1. 头节点移除：更新全局链表头指针
             * 2. 中间/尾节点：更新前驱节点指针 */
            if (pxPrevious == NULL)
            {
                pxResourceMappingList = pxCurrent->pxNext;
            }
            else
            {
                pxPrevious->pxNext = pxCurrent->pxNext;
            }
            
            /* 资源控制块内存回收
             * 安全机制：
             * - 检查指针有效性
             * - 使用与分配时匹配的释放函数 */
            if (pxCurrent->pxResourceCB != NULL)
            {
                vPortFree(pxCurrent->pxResourceCB);
            }
            
            /* 映射节点内存回收
             * 注意：必须在资源块释放之后执行 */
            vPortFree(pxCurrent);
            xFound = pdTRUE;
            break;
        }
        
        /* 双指针遍历法
         * 作用：
         * - pxPrevious跟踪前驱节点
         * - pxCurrent遍历当前节点 */
        pxPrevious = pxCurrent;
        pxCurrent = pxCurrent->pxNext;
    }
    
    /* 退出临界区
     * 重要性：必须确保在所有代码路径中释放锁 */
    if (xResourceMappingMutex != NULL)
    {
        xSemaphoreGive(xResourceMappingMutex);
    }
    
    return xFound;
}

//11. 获取与信号量关联的资源控制块
/*
 * 资源控制块查询函数（内部）
 * 功能：
 *   1. 通过信号量句柄查找关联的资源控制块
 *   2. 保证资源管理系统的线程安全性
 * 参数：
 *   xSemaphore - 已注册的互斥量/信号量句柄
 * 返回值：
 *   ResourceCB_t* - 找到的资源控制块指针
 *   NULL          - 未找到对应资源块或参数无效
 * 注意：
 *   - 必须在资源映射系统初始化后调用
 *   - 返回的指针需在临界区内使用
 */
static ResourceCB_t *prvGetResourceCBFromSemaphore(SemaphoreHandle_t xSemaphore)
{
    ResourceMapping_t *pxCurrent = pxResourceMappingList;
    ResourceCB_t *pxResourceCB = NULL;
    
    /* 参数有效性校验
     * 防御机制：
     * 防止无效信号量导致的空指针访问 */
    if (xSemaphore == NULL)
    {
        return NULL;
    }
    
    /* 进入临界区（保护链表遍历）
     * 阻塞策略：永久等待直到获取锁
     * 优先级继承：确保高优先级任务能及时获取锁 */
    if (xResourceMappingMutex != NULL)
    {
        xSemaphoreTake(xResourceMappingMutex, portMAX_DELAY);
    }
    
    /* 线性搜索算法
     * 时间复杂度：O(n)
     * 优化空间：可改用哈希表提升查找效率 */
    while (pxCurrent != NULL)
    {
        /* 信号量句柄比对
         * 注意：直接比较句柄值而非内容
         * 前提：信号量句柄具有唯一性 */
        if (pxCurrent->xSemaphore == xSemaphore)
        {
            /* 获取资源控制块指针
             * 生命周期保证：
             * 映射存在期间资源块有效 */
            pxResourceCB = pxCurrent->pxResourceCB;
            break;
        }
        pxCurrent = pxCurrent->pxNext;
    }
    
    /* 退出临界区
     * 重要性：必须确保锁释放避免死锁 */
    if (xResourceMappingMutex != NULL)
    {
        xSemaphoreGive(xResourceMappingMutex);
    }
    
    return pxResourceCB;
}


//12. 将已有信号量/互斥量转换为带阶段的资源
/*
 * 信号量到阶段资源转换函数（内部）
 * 功能：
 *   1. 将普通信号量升级为阶段感知资源
 *   2. 维护资源控制块与信号量的生命周期绑定
 * 参数：
 *   xSemaphore - 需要转换的信号量句柄
 *   uxStage    - 资源所属阶段标识
 * 返回值：
 *   ResourceCB_t* - 成功返回资源控制块指针
 *   NULL          - 转换失败（参数无效/内存不足/阶段冲突）
 * 注意：
 *   - 必须通过xCleanupResourceCB进行资源清理
 *   - 禁止重复转换同一信号量到不同阶段
 */
ResourceCB_t *prvConvertToResourceCB(SemaphoreHandle_t xSemaphore, UBaseType_t uxStage)
{
    ResourceCB_t *pxResource = NULL;
    
    /* 前置参数校验
     * 过滤条件：
     * - 无效信号量句柄
     * - 非法阶段值（需小于configMAX_STAGES） */
    if (xSemaphore == NULL || uxStage >= configMAX_STAGES)
    {
        return NULL;
    }
    
    /* 查询现有资源映射
     * 目的：避免重复创建相同信号的资源控制块
     * 线程安全：在prvGetResourceCBFromSemaphore内部实现 */
    pxResource = prvGetResourceCBFromSemaphore(xSemaphore);
    if (pxResource != NULL)
    {
        /* 阶段一致性校验
         * 设计约束：
         * - 同一信号量不能绑定到多个阶段
         * - 现有阶段必须与参数完全匹配 */
        if (pxResource->uxResourceStage == uxStage)
        {
            return pxResource;
        }
        else
        {
            /* 阶段冲突处理
             * 可选策略：
             * 1. 返回错误（当前实现）
             * 2. 强制更新阶段（需重新初始化等待队列）
             * 安全选择：保守策略避免状态不一致 */
            return NULL;
        }
    }
    
    /* 创建新的资源控制块
     * 内存分配：使用RTOS堆管理器
     * 失败处理：直接返回NULL */
    pxResource = (ResourceCB_t *)pvPortMalloc(sizeof(ResourceCB_t));
    if (pxResource != NULL)
    {
        /* 资源控制块初始化
         * 初始化内容：
         * - 等待队列初始化
         * - 阶段标识设置
         * - 持有者指针置空 */
        vInitializeResourceCB(pxResource, uxStage);
        
        /* 注册资源映射
         * 关键操作：将信号量与资源块绑定
         * 失败回滚：释放已分配内存 */
        if (prvAddResourceMapping(xSemaphore, pxResource) != pdTRUE)
        {
            /* 内存回收
             * 安全机制：
             * - 逆向操作分配过程
             * - 防止内存泄漏 */
            vPortFree(pxResource);
            pxResource = NULL;
        }
    }
    
    return pxResource;
}


//13.清理资源控制块（当信号量被删除时调用）
/*
 * 资源控制块清理函数
 * 功能：
 *   1. 解除信号量与资源控制块的关联
 *   2. 清理资源管理系统中的相关内存
 * 参数：
 *   xSemaphore - 需要清理的互斥量/信号量句柄
 * 返回值：
 *   pdTRUE  - 资源清理成功
 *   pdFAIL   - 参数无效或未找到映射
 * 注意：
 *   - 必须与prvConvertToResourceCB配对使用
 *   - 应在删除信号量前调用本函数
 */
BaseType_t xCleanupResourceCB(SemaphoreHandle_t xSemaphore)
{
    /* 调用内部映射移除函数
     * 封装目的：
     * 1. 提供统一的资源清理接口
     * 2. 隐藏内部实现细节
     * 操作包含：
     * - 链表节点移除
     * - 资源控制块内存释放
     * - 映射节点内存释放 */
    return prvRemoveResourceMapping(xSemaphore);
}

//14. 获取当前任务的阶段（辅助函数）
/*
 * 任务阶段获取函数
 * 功能：
 *   1. 查询任务的当前执行阶段
 *   2. 支持跨任务阶段状态监控
 * 参数：
 *   xTask - 目标任务句柄（NULL表示当前任务）
 * 返回值：
 *   0 ~ (configMAX_STAGES-1) - 当前阶段索引
 *   0                        - 默认值（任务无效或未初始化阶段）
 * 注意：
 *   - 对于未初始化阶段的任务返回0
 *   - 在中断安全临界区内获取阶段值
 */
UBaseType_t xTaskGetCurrentStage(TaskHandle_t xTask)
{
    TCB_t *pxTCB;
    UBaseType_t uxCurrentStage = 0;
    
    /* 动态任务上下文处理
     * 设计特点：
     * - 支持查询任意任务或当前任务
     * - 通过句柄转换获取TCB指针
     * 安全机制：
     * 防止通过无效句柄访问内存 */
    if (xTask == NULL)
        pxTCB = (TCB_t *)xTaskGetCurrentTaskHandle();
    else
        pxTCB = (TCB_t *)xTask;
    
    if (pxTCB != NULL)
    {
        /* 进入临界区（保护阶段读取）
         * 必要性：
         * - 防止任务切换导致阶段值变化
         * - 保证读取操作的原子性 */
        taskENTER_CRITICAL();
        {
            /* 直接访问TCB阶段字段
             * 内存布局：
             * uxCurrentStage在TCB结构中的偏移固定
             * 有效性保证：
             * 未被阶段系统初始化的任务默认返回0 */
            uxCurrentStage = pxTCB->uxCurrentStage;
        }
        taskEXIT_CRITICAL();
    }
    
    return uxCurrentStage;
}

/*------------------------------------------------------------------*/
/* 15.阶段感知型互斥量获取函数 - 增强版 (xSemaphoreTakeStagedMutex) 
 * 核心功能：
 *   1. 实现基于任务阶段的细粒度资源访问控制，确保阶段匹配才能获取资源
 *   2. 自动维护任务-资源绑定关系，形成资源依赖链
 *   3. 支持优先级继承的嵌套式阶段锁管理
 *   4. 提供原子化的资源列表管理，防止并发修改
 *   5. 实现双重锁获取机制（立即获取+带超时获取）
 * 
 * 工作机制：
 *   a) 阶段校验 → 资源转换 → 立即获取锁 → 资源绑定
 *   b) 若首次获取失败 → 触发优先级继承 → 带超时重试
 * 
 * 参数说明：
 *   xMutex        - 互斥量句柄（必须为有效的递归互斥量）
 *   xBlockTime    - 最大等待时间（单位tick，portMAX_DELAY表示无限等待）
 *   uxResourceStage - 资源要求的阶段标识（必须与当前任务阶段严格匹配）
 * 
 * 返回值：
 *   pdTRUE  - 成功获取锁并完成资源绑定
 *   pdFAIL  - 参数无效/阶段不匹配/资源初始化失败/获取超时
 * 
 * 调用约束：
 *   - 必须在任务上下文调用（禁止中断服务程序中使用）
 *   - 必须与xSemaphoreGiveStagedMutex配对使用
 *   - 同一任务对同一互斥量需按FILO顺序操作
 *   - 资源阶段需通过任务控制块(TCB)的stage字段验证
 * 
 * 设计要点：
 *   - 使用临界区保护资源列表操作
 *   - 通过资源控制块(ResourceCB)跟踪持有关系
 *   - 采用listIS_CONTAINED_WITHIN实现防重复插入
 *   - 优先级继承仅在首次获取失败时触发
 */
/*------------------------------------------------------------------*/
BaseType_t xSemaphoreTakeStagedMutex(SemaphoreHandle_t xMutex, TickType_t xBlockTime, UBaseType_t uxResourceStage)
{
    /* 返回值初始化（默认失败状态） */
    BaseType_t xReturn;

    /* 获取当前任务控制块指针（需强制类型转换） */
    TCB_t *pxCurrentTCB = (TCB_t *)xTaskGetCurrentTaskHandle();

    /* 资源控制块指针声明（用于管理资源持有关系） */
    ResourceCB_t *pxResource;

    /*--------------------------------------------------------------
     * 前置校验（防御性编程）
     * - 检查基础参数有效性，防止系统崩溃
     * - 空指针检查作为第一道防线
     --------------------------------------------------------------*/
    if (xMutex == NULL || pxCurrentTCB == NULL) 
    {
        /* 立即返回失败，避免后续非法内存访问 */
        return pdFAIL; 
    }

    /*--------------------------------------------------------------
     * 阶段兼容性验证
     * - 获取任务当前阶段（通过NULL参数获取调用者自身阶段）
     * - 实现阶段隔离策略的核心检查点
     --------------------------------------------------------------*/
    UBaseType_t uxCurrentStage = xTaskGetCurrentStage(NULL);
    if (uxCurrentStage != uxResourceStage) 
    {
        /* 阶段不匹配时直接拒绝，保证阶段隔离性 */
        return pdFAIL; 
    }

    /*--------------------------------------------------------------
     * 资源控制块转换/创建
     * - 将互斥量句柄转换为资源控制块
     * - 处理阶段冲突和内存分配问题
     --------------------------------------------------------------*/
    pxResource = prvConvertToResourceCB(xMutex, uxResourceStage);
    if (pxResource == NULL) 
    {
        /* 资源初始化失败处理（可能内存不足或阶段配置冲突） */
        return pdFAIL; 
    }

    /*--------------------------------------------------------------
     * 首次尝试立即获取互斥量（非阻塞模式）
     * - 使用0超时立即返回获取状态
     * - 成功获取时跳过后续等待逻辑
     --------------------------------------------------------------*/
    xReturn = xSemaphoreTake(xMutex,0);

    /*--------------------------------------------------------------
     * 锁获取成功处理分支
     * - 需在临界区内操作共享资源
     --------------------------------------------------------------*/
    if (xReturn == pdTRUE) 
    {
        /* 进入临界区（禁用中断）保护资源列表 */
        taskENTER_CRITICAL();
        {   
            /*------------------------------------------------------
             * 更新资源持有者
             * - 建立任务与资源的所属关系
             * - 为后续优先级继承提供依据
             ------------------------------------------------------*/
            pxResource->pxCurrentHolder = pxCurrentTCB;

            /*------------------------------------------------------
             * 原子化资源列表更新
             * 防重复插入机制：
             * 1. listIS_CONTAINED_WITHIN检查节点是否存在
             * 2. vListInsertEnd的幂等性设计保证最终一致性
             ------------------------------------------------------*/
            if (listIS_CONTAINED_WITHIN(&pxCurrentTCB->xResourcesList,
                                      &pxResource->xResourceListItem) == pdFALSE) 
            {
                /* 将资源项插入任务资源列表末端 */
                vListInsertEnd(&pxCurrentTCB->xResourcesList,
                              &pxResource->xResourceListItem);
            }
        }
        /* 退出临界区（恢复中断） */
        taskEXIT_CRITICAL();        
    }
    /*--------------------------------------------------------------
     * 首次获取失败处理分支
     * - 触发优先级继承机制
     * - 执行带超时的二次获取
     --------------------------------------------------------------*/
    else
    {
        /* 进入临界区检查资源持有者状态 */
        taskENTER_CRITICAL();
        {
            /* 检查资源当前持有者是否存在 */
            if (pxResource->pxCurrentHolder != NULL)
            {
                /* 比较当前任务与持有者的优先级 */
                if (uxTaskPriorityGet(pxCurrentTCB) > uxTaskPriorityGet(pxResource->pxCurrentHolder))
                {
                    /*--------------------------------------------------
                     * 触发优先级继承
                     * - 提升持有者优先级至当前任务优先级
                     * - 防止优先级反转的关键机制
                     --------------------------------------------------*/
                    vTaskPriorityInherit(pxResource->pxCurrentHolder);
                }
            }
        }
        taskEXIT_CRITICAL();

        /*--------------------------------------------------------------
         * 二次尝试获取互斥量（带超时阻塞）
         * - 使用用户指定的阻塞时间
         * - 可能挂起当前任务等待资源释放
         --------------------------------------------------------------*/
        xReturn = xSemaphoreTake(xMutex, xBlockTime);
				    /*--------------------------------------------------------------
			 * 锁获取成功处理分支
			 * - 需在临界区内操作共享资源
			 --------------------------------------------------------------*/
			if (xReturn == pdTRUE) 
			{
					/* 进入临界区（禁用中断）保护资源列表 */
					taskENTER_CRITICAL();
					{   
							/*------------------------------------------------------
							 * 更新资源持有者
							 * - 建立任务与资源的所属关系
							 * - 为后续优先级继承提供依据
							 ------------------------------------------------------*/
							pxResource->pxCurrentHolder = pxCurrentTCB;

							/*------------------------------------------------------
							 * 原子化资源列表更新
							 * 防重复插入机制：
							 * 1. listIS_CONTAINED_WITHIN检查节点是否存在
							 * 2. vListInsertEnd的幂等性设计保证最终一致性
							 ------------------------------------------------------*/
							if (listIS_CONTAINED_WITHIN(&pxCurrentTCB->xResourcesList,
																				&pxResource->xResourceListItem) == pdFALSE) 
							{
									/* 将资源项插入任务资源列表末端 */
									vListInsertEnd(&pxCurrentTCB->xResourcesList,
																&pxResource->xResourceListItem);
							}
					}
					/* 退出临界区（恢复中断） */
					taskEXIT_CRITICAL();        
			}
			else //获取失败，则加入超时列表
			{
				vListInsertEnd(&(pxResource->xTasksWaitingForResource), &(pxCurrentTCB->xEventListItem));
			}
			
    }

    /* 返回最终获取状态（成功/失败） */
    return xReturn;
}


#ifdef FREERTOS_MODULE_TEST
	#include "tasks_test_access_functions.h"
#endif


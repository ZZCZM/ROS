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

#include <stdlib.h>
#include <string.h>

/*-------------------------------------------------------------------------------------*/
/* FreeRTOS队列核心数据结构及配套宏定义模块                                             */
/* 本模块主要完成以下功能：                                                             */
/* 1.定义队列和互斥量共用的核心数据结构QueueDefinition                                */
/* 2.处理MPU内存保护单元在API层的封装控制                                             */
/* 3.实现队列/信号量/互斥量的底层类型标志管理                                          */
/* 4.定义队列操作相关的关键控制参数和状态标识                                          */
/* 5.提供不同调度模式下的任务切换控制宏                                                */
/*-------------------------------------------------------------------------------------*/

/* 通过定义该宏防止task.h重复包裹MPU相关API函数定义
   此宏仅应在应用文件包含task.h时生效，此处临时定义用于隔离API层的MPU封装 */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 包含FreeRTOS核心组件 ------------------------------------------------------------*/
#include "FreeRTOS.h"   // 核心数据类型和配置项定义
#include "task.h"       // 任务调度相关API原型
#include "queue.h"      // 队列操作API原型声明

/* 协程支持的条件编译 --------------------------------------------------------------*/
#if ( configUSE_CO_ROUTINES == 1 )
    #include "croutine.h" // 协程控制块和相关调度逻辑
#endif

/* 解除MPU封装宏定义(此处形成完整代码块)----------------------------------------------
   lint错误抑制说明：
   - e961: 不信任的宏定义（因MISRA通常不允许宏重定义，此处特殊处理）
   - e750: 局部宏未使用（该宏的全局作用需要在此处解除定义）
   架构要求：MPU端口需要通过头文件定义该宏，但本体文件需要取消定义以保持原始API接口 */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE /*lint !e961 !e750. */

/* 队列锁定状态常量定义 -------------------------------------------------------------*/
#define queueUNLOCKED                ( ( int8_t ) -1 )  // 队列未锁定状态标识值
#define queueLOCKED_UNMODIFIED       ( ( int8_t ) 0 )   // 队列锁定但内容未变化状态

/* 结构体成员双重用途重命名宏 -------------------------------------------------------
   当Queue_t用作基础队列时：
   - pcHead: 队列存储区起始地址指针
   - pcTail: 队列存储区结束地址指针
   当Queue_t用作互斥量时：
   - pcHead置NULL表示当前结构用作互斥量
   - pcTail此时指向持有互斥量的任务控制块 */
#define pxMutexHolder                pcTail    // 互斥量持有者指针标识别名
#define uxQueueType                  pcHead    // 队列类型标志指针别名
#define queueQUEUE_IS_MUTEX          NULL      // 标识当前结构作为互斥量的特殊指针值

/* 信号量专用参数定义 ---------------------------------------------------------------*/
#define queueSEMAPHORE_QUEUE_ITEM_LENGTH ( ( UBaseType_t ) 0 ) // 信号量项长度设为0（不存储实际数据）
#define queueMUTEX_GIVE_BLOCK_TIME       ( ( TickType_t ) 0U ) // 互斥量释放操作的立即执行超时设置

/* 调度器模式相关操作宏 -------------------------------------------------------------*/
#if( configUSE_PREEMPTION == 0 )
    /* 协作式调度模式：禁止自动触发任务切换（开发者需手动管理） */
    #define queueYIELD_IF_USING_PREEMPTION() 
#else
    /* 抢占式调度模式：立即触发调度器重新评估任务优先级 */
    #define queueYIELD_IF_USING_PREEMPTION() portYIELD_WITHIN_API() // 调用体系相关的上下文切换指令
#endif

/* 队列核心数据结构定义 -------------------------------------------------------------
   遵循以下设计原则：
   1.数据拷贝方式入队（非引用传递），保证数据隔离性
   2.队列存储区环形缓冲区管理机制
   3.支持互斥量/信号量的特殊处理模式
   （详见官方设计文档：http://www.freertos.org/Embedded-RTOS-Queues.html） */
typedef struct QueueDefinition
{
    /* 存储区指针三重奏 -----------------------------------------------------------*/
    int8_t *pcHead;         /*< 队列存储区物理起始地址（固定指针，初始化后不变）*/
    int8_t *pcTail;         /*< 队列存储区终止标识地址（包含一个防护字节空间）*/
    int8_t *pcWriteTo;      /*< 下一可写入位置指针（循环缓冲区移动指针）*/

    /* 联合体实现双重用途字段 -----------------------------------------------------
       注意：此处使用联合体属于MISRA标准的例外情况，主要为了优化内存使用效率 */
    union {
        int8_t *pcReadFrom;            /*< 队列模式下最后读取位置指针 */
        UBaseType_t uxRecursiveCallCount; /*< 互斥量模式下递归获取计数器 */
    } u; // 匿名联合体字段（通过宏选择使用方式）

    /* 任务等待列表 ---------------------------------------------------------------*/
    List_t xTasksWaitingToSend;   /*< 发送阻塞任务列表（优先级排序，等待队列空间）*/
    List_t xTasksWaitingToReceive;/*< 接收阻塞任务列表（优先级排序，等待队列数据）*/

    /* 队列状态参数 ---------------------------------------------------------------*/
    volatile UBaseType_t uxMessagesWaiting; /*< 当前队列中的有效消息数量（原子访问）*/
    UBaseType_t uxLength;          /*< 队列总容量（可容纳的最大消息数）*/
    UBaseType_t uxItemSize;        /*< 单条消息的字节大小（包含对齐填充的物理尺寸）*/

    /* 队列锁定控制 ---------------------------------------------------------------*/
    volatile int8_t cRxLock;       /*< 接收锁定计数器（操作时递增，解锁后重置为-1）*/
    volatile int8_t cTxLock;       /*< 发送锁定计数器（操作时递增，解锁后重置为-1）*/

    /* 内存分配模式标志（同时支持静态和动态分配时启用）-----------------------------*/
    #if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
        uint8_t ucStaticallyAllocated; /*< 静态分配标识（防止错误释放内存）*/
    #endif

    /* 队列集支持字段（需要启用队列集功能时存在）-----------------------------------*/
    #if ( configUSE_QUEUE_SETS == 1 )
        struct QueueDefinition *pxQueueSetContainer; /*< 所属队列集的容器指针 */
    #endif

    /* 追踪调试信息（需要启用追踪功能时存在）---------------------------------------*/
    #if ( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t uxQueueNumber;  /*< 调试器可见的队列唯一编号 */
        uint8_t ucQueueType;       /*< 队列类型标识符（普通队列/互斥量/信号量等）*/
    #endif

} xQUEUE; // 历史类型名（向下兼容保留）

/*-----------------------------------------------------------------------------------*/
/* FreeRTOS队列辅助功能模块                                                           */
/* 本模块包含以下核心功能：                                                          */
/* 1.队列注册表实现 - 为内核调试工具提供队列定位支持                                  */
/* 2.队列基础操作函数原型声明（锁定机制、数据拷贝、状态检查）                        */
/* 3.队列集(Queue Set)相关功能支持                                                  */
/* 4.队列结构初始化标准化流程                                                        */
/*-----------------------------------------------------------------------------------*/

/* 历史类型名兼容处理（供旧版调试器识别）---------------------------------------------*/
typedef xQUEUE Queue_t;

/* 队列注册表实现（需配置configQUEUE_REGISTRY_SIZE >0启用）---------------------------*/
#if ( configQUEUE_REGISTRY_SIZE > 0 )

    /* 队列注册项数据结构（用于关联队列与可读名称）-------------------------------------*/
    typedef struct QUEUE_REGISTRY_ITEM
    {
        const char *pcQueueName; /*< 队列名称字符串指针（符合MISRA字符串处理规则）*/
        QueueHandle_t xHandle;   /*< 关联队列的句柄（用于快速访问队列控制块）*/
    } xQueueRegistryItem;  // 历史类型名

    /* 新老类型名兼容处理（保持向下兼容性）-------------------------------------------*/
    typedef xQueueRegistryItem QueueRegistryItem_t;

    /* 全局队列注册表数组（特权数据段存储）--------------------------------------------
       说明：
       - 数组元素pcQueueName为NULL时表示该槽位可用
       - 注册表尺寸通过configQUEUE_REGISTRY_SIZE配置 */
    PRIVILEGED_DATA QueueRegistryItem_t xQueueRegistry[ configQUEUE_REGISTRY_SIZE ];

#endif /* configQUEUE_REGISTRY_SIZE */


/* 队列核心操作函数原型 ------------------------------------------------------------*/

/**
 * @brief 解除队列锁定状态
 * @param pxQueue 目标队列指针
 * 功能：
 * - 解除通过prvLockQueue设置的队列锁
 * - 检查锁定期间的操作计数并处理可能需要的任务唤醒
 * - ISR安全设计：锁定期间允许ISR操作队列，但禁止修改事件列表
 */
static void prvUnlockQueue( Queue_t * const pxQueue ) PRIVILEGED_FUNCTION;

/**
 * @brief 队列空状态检测
 * @param pxQueue 目标队列指针
 * @return pdTRUE表示队列无数据，pdFALSE表示有数据
 * 注意：
 * - 使用临界区保护检测过程
 * - 原子化读取uxMessagesWaiting字段
 */
static BaseType_t prvIsQueueEmpty( const Queue_t *pxQueue ) PRIVILEGED_FUNCTION;

/**
 * @brief 队列满状态检测
 * @param pxQueue 目标队列指针
 * @return pdTRUE表示队列已满，pdFALSE表示有空位
 * 注意：
 * - 使用临界区保护检测过程
 * - 比较uxMessagesWaiting与uxLength判断
 */
static BaseType_t prvIsQueueFull( const Queue_t *pxQueue ) PRIVILEGED_FUNCTION;

/**
 * @brief 数据写入队列
 * @param pxQueue 目标队列指针
 * @param pvItemToQueue 待写入数据指针
 * @param xPosition 写入位置选择：
 *                   - queueSEND_TO_BACK 队尾写入（标准入队）
 *                   - queueSEND_TO_FRONT 队首写入（紧急消息）
 * @return 操作结果（依赖具体实现）
 * 功能：
 * - 执行内存拷贝操作
 * - 处理队列指针移动（环形缓冲区管理）
 */
static BaseType_t prvCopyDataToQueue( Queue_t * const pxQueue, 
                                    const void *pvItemToQueue,
                                    const BaseType_t xPosition ) PRIVILEGED_FUNCTION;

/**
 * @brief 从队列提取数据
 * @param pxQueue 目标队列指针
 * @param pvBuffer 接收缓冲区指针
 * 功能：
 * - 执行内存拷贝操作
 * - 更新读取位置指针（pcReadFrom）
 * - 处理环形缓冲区回绕逻辑
 */
static void prvCopyDataFromQueue( Queue_t * const pxQueue, 
                                void * const pvBuffer ) PRIVILEGED_FUNCTION;

/* 队列集相关功能（需配置configUSE_QUEUE_SETS ==1启用）-------------------------------*/
#if ( configUSE_QUEUE_SETS == 1 )

    /**
     * @brief 队列集通知处理
     * @param pxQueue 源队列指针
     * @param xCopyPosition 数据位置标识
     * @return 是否需要触发任务切换
     * 功能：
     * - 当队列属于某个队列集时，向集合发送通知
     * - 处理跨队列集的事件传播
     */
    static BaseType_t prvNotifyQueueSetContainer( const Queue_t * const pxQueue,
                                                const BaseType_t xCopyPosition ) PRIVILEGED_FUNCTION;
#endif

/**
 * @brief 新型队列结构初始化
 * @param uxQueueLength 队列容量（最大元素数）
 * @param uxItemSize 单个元素字节大小
 * @param pucQueueStorage 存储区指针（静态分配时提供）
 * @param ucQueueType 队列类型标识：
 *                    - queueQUEUE_TYPE_BASE 基础队列
 *                    - queueQUEUE_TYPE_MUTEX 互斥量
 *                    - queueQUEUE_TYPE_COUNTING_SEMAPHORE 计数信号量等
 * @param pxNewQueue 目标队列结构指针
 * 功能：
 * - 统一初始化队列结构各字段
 * - 支持静态/动态两种内存分配模式
 * - 配置队列类型特定参数（如互斥量递归计数器初始化）
 */
static void prvInitialiseNewQueue( const UBaseType_t uxQueueLength,
                                 const UBaseType_t uxItemSize,
                                 uint8_t *pucQueueStorage,
                                 const uint8_t ucQueueType,
                                 Queue_t *pxNewQueue ) PRIVILEGED_FUNCTION;

/*-----------------------------------------------------------------------------------*/
/* FreeRTOS队列控制功能模块                                                           */
/* 本模块实现以下核心功能：                                                           */
/* 1.互斥量队列的初始化接口                                                          */
/* 2.队列锁定/解锁的原子操作实现                                                     */
/* 3.通用队列重置方法，支持新队列初始化和已有队列状态重置                            */
/*-----------------------------------------------------------------------------------*/

/* 互斥量初始化函数声明（需配置configUSE_MUTEXES ==1启用）---------------------------*/
#if( configUSE_MUTEXES == 1 )
    /**
     * @brief 互斥量专用初始化器
     * @param pxNewQueue 基础队列对象指针
     * 功能：
     * - 将普通队列转换为互斥量类型
     * - 初始化互斥量特有参数（如递归计数器）
     * - 配置优先级继承相关属性
     */
    static void prvInitialiseMutex( Queue_t *pxNewQueue ) PRIVILEGED_FUNCTION;
#endif

/* 队列锁定原子操作宏 --------------------------------------------------------------*/
// 进入临界区（关闭中断）
// 检查接收锁状态
// 锁定接收操作计数器
// 检查发送锁状态
// 锁定发送操作计数器
// 退出临界区（恢复中断）
#define prvLockQueue( pxQueue )								\
	taskENTER_CRITICAL();									\
	{														\
		if( ( pxQueue )->cRxLock == queueUNLOCKED )			\
		{													\
			( pxQueue )->cRxLock = queueLOCKED_UNMODIFIED;	\
		}													\
		if( ( pxQueue )->cTxLock == queueUNLOCKED )			\
		{													\
			( pxQueue )->cTxLock = queueLOCKED_UNMODIFIED;	\
		}													\
	}														\
	taskEXIT_CRITICAL()                                     

/* 队列通用重置函数实现 ------------------------------------------------------------*/
BaseType_t xQueueGenericReset( QueueHandle_t xQueue, BaseType_t xNewQueue )
{
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;  // 类型转换为队列控制块指针

    configASSERT( pxQueue );  // 参数有效性验证（调试模式）

    taskENTER_CRITICAL();  // 进入临界区（保护队列结构）
    {
        /* 存储区指针重置 ---------------------------------------------------------*/
        pxQueue->pcTail = pxQueue->pcHead + ( pxQueue->uxLength * pxQueue->uxItemSize );  // 计算并设置尾指针
        pxQueue->uxMessagesWaiting = ( UBaseType_t ) 0U;  // 清空队列消息计数器
        pxQueue->pcWriteTo = pxQueue->pcHead;  // 写指针回归起始位置
        /* 读指针初始化到最后一个元素的位置（环形缓冲区设计）------------------------*/
        pxQueue->u.pcReadFrom = pxQueue->pcHead + ( ( pxQueue->uxLength - 1 ) * pxQueue->uxItemSize );
        pxQueue->cRxLock = queueUNLOCKED;  // 解除接收锁
        pxQueue->cTxLock = queueUNLOCKED;  // 解除发送锁

        /* 队列复用重置处理 vs 新队列初始化 -----------------------------------------*/
        if( xNewQueue == pdFALSE )  // 现有队列重置模式
        {
            /* 任务阻塞列表处理策略：
               - 读阻塞任务：保持阻塞（队列依然为空）
               - 写阻塞任务：唤醒首个任务（队列现在可写）*/
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
            {
                /* 从写等待列表移除任务并加入就绪列表 */
                if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
                {
                    queueYIELD_IF_USING_PREEMPTION();  // 触发调度器（如果需要）
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（空分支）
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（空列表分支）
            }
        }
        else  // 全新队列初始化模式
        {
            /* 任务等待列表初始化（双向链表基础设置）*/
            vListInitialise( &( pxQueue->xTasksWaitingToSend ) );    // 发送等待列表
            vListInitialise( &( pxQueue->xTasksWaitingToReceive ) ); // 接收等待列表
        }
    }
    taskEXIT_CRITICAL();  // 退出临界区

    /* 返回值保持历史兼容性（实际操作始终成功）*/
    return pdPASS;  // 统一返回成功状态
}
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS队列创建功能模块                                                           */
/* 本模块实现以下核心功能：                                                           */
/* 1.静态内存分配方式队列创建（xQueueGenericCreateStatic）                           */
/* 2.动态内存分配方式队列创建（xQueueGenericCreate）                                 */
/* 3.队列存储区的内存布局管理                                                        */
/* 4.队列类型标识的通用化处理                                                        */
/*-----------------------------------------------------------------------------------*/

/* 静态队列创建函数（需配置configSUPPORT_STATIC_ALLOCATION==1）----------------------*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )

QueueHandle_t xQueueGenericCreateStatic( 
    const UBaseType_t uxQueueLength,     /* 队列最大容量（元素个数）*/
    const UBaseType_t uxItemSize,        /* 单个元素字节大小（0表示无数据队列）*/
    uint8_t *pucQueueStorage,            /* 预分配的存储区指针（需与item_size匹配）*/
    StaticQueue_t *pxStaticQueue,       /* 静态队列控制块指针（需预分配内存）*/
    const uint8_t ucQueueType )          /* 队列类型标识符 */
{
    Queue_t *pxNewQueue;  // 新队列对象指针

    /* 参数有效性验证 -----------------------------------------------------------*/
    configASSERT( uxQueueLength > 0 );  // 队列容量必须大于0
    configASSERT( pxStaticQueue != NULL );  // 静态控制块必须提供
    /* 存储区指针验证（有数据队列需提供存储区，无数据队列不得提供）*/
    configASSERT( !( pucQueueStorage && (uxItemSize == 0) ) );  
    configASSERT( !( !pucQueueStorage && (uxItemSize != 0) ) );

    #if( configASSERT_DEFINED == 1 )
    {
        /* 结构体尺寸校验（确保StaticQueue_t与Queue_t内存布局一致）*/
        volatile size_t xSize = sizeof( StaticQueue_t );
        configASSERT( xSize == sizeof( Queue_t ) );  // 防御错误类型传递
    }
    #endif

    /* 类型转换（依赖结构体尺寸一致的校验）-----------------------------------------
       lint抑制说明：740警告是因非常规类型转换，此处通过断言保证结构兼容性 */
    pxNewQueue = ( Queue_t * ) pxStaticQueue; /*lint !e740 */

    if( pxNewQueue != NULL )
    {
        #if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
        {
            /* 静态分配标识设置（供后续删除操作判断内存释放方式）*/
            pxNewQueue->ucStaticallyAllocated = pdTRUE;  
        }
        #endif

        /* 调用通用初始化器完成队列结构配置 */
        prvInitialiseNewQueue( uxQueueLength, uxItemSize, pucQueueStorage, 
                             ucQueueType, pxNewQueue );
    }

    return pxNewQueue;  // 返回队列句柄
}

#endif /* 静态分配支持结束 */
/*-----------------------------------------------------------*/

/* 动态队列创建函数（需配置configSUPPORT_DYNAMIC_ALLOCATION==1）----------------------*/
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

QueueHandle_t xQueueGenericCreate( 
    const UBaseType_t uxQueueLength,     /* 队列最大容量（元素个数）*/
    const UBaseType_t uxItemSize,        /* 单个元素字节大小（0表示无数据队列）*/
    const uint8_t ucQueueType )          /* 队列类型标识符 */
{
    Queue_t *pxNewQueue;       // 新队列对象指针
    size_t xQueueSizeInBytes;  // 存储区需求字节数
    uint8_t *pucQueueStorage;  // 计算出的存储区起始地址

    configASSERT( uxQueueLength > 0 );  // 队列容量必须大于0

    /* 存储区尺寸计算（无数据队列分配0字节）----------------------------------------*/
    if( uxItemSize == 0 )
    {
        xQueueSizeInBytes = 0;  // 信号量等类型无需数据存储
    }
    else
    {
        /* 总字节数 = 容量 × 单元素尺寸（考虑lint误报的MISRA异常）*/
        xQueueSizeInBytes = ( size_t )( uxQueueLength * uxItemSize ); /*lint !e961 */
    }

    /* 动态内存分配（控制块+存储区连续空间）----------------------------------------*/
    pxNewQueue = ( Queue_t * ) pvPortMalloc( sizeof(Queue_t) + xQueueSizeInBytes );

    if( pxNewQueue != NULL )
    {
        /* 存储区定位（紧接控制块之后）--------------------------------------------*/
        pucQueueStorage = ( ( uint8_t * ) pxNewQueue ) + sizeof( Queue_t );

        #if( configSUPPORT_STATIC_ALLOCATION == 1 )
        {
            /* 动态分配标识设置（与静态分配区分内存释放方式）*/
            pxNewQueue->ucStaticallyAllocated = pdFALSE;  
        }
        #endif

        /* 调用通用初始化器完成队列结构配置 */
        prvInitialiseNewQueue( uxQueueLength, uxItemSize, pucQueueStorage,
                             ucQueueType, pxNewQueue );
    }

    return pxNewQueue;  // 返回队列句柄
}

#endif /* 动态分配支持结束 */
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS队列初始化核心模块                                                         */
/* 本模块实现以下核心功能：                                                           */
/* 1.新型队列结构通用初始化逻辑                                                      */
/* 2.互斥量队列的专用初始化流程                                                      */
/* 3.动态创建互斥量队列的封装接口                                                    */
/*-----------------------------------------------------------------------------------*/

/**
 * @brief 新型队列结构通用初始化器（内部核心函数）
 * @param uxQueueLength 队列容量
 * @param uxItemSize 单个元素字节数
 * @param pucQueueStorage 存储区指针（可NULL）
 * @param ucQueueType 队列类型标识
 * @param pxNewQueue 目标队列结构指针
 * 功能说明：
 * - 完成队列元数据的基础配置
 * - 处理无存储队列的特殊情况（互斥量/信号量）
 * - 调用重置函数完成环形缓冲区设置
 */
static void prvInitialiseNewQueue( const UBaseType_t uxQueueLength,
                                 const UBaseType_t uxItemSize,
                                 uint8_t *pucQueueStorage,
                                 const uint8_t ucQueueType,
                                 Queue_t *pxNewQueue )
{
    /* 抑制编译器警告（当追踪功能关闭时ucQueueType可能未使用）*/
    ( void ) ucQueueType;

    /* 存储区特殊处理 ---------------------------------------------------------*/
    if( uxItemSize == 0 )
    {
        /* 无数据队列处理（如互斥量/信号量）：
           pcHead不能置NULL（因为NULL用于标识互斥量），
           所以指向自身地址作为占位符 */
        pxNewQueue->pcHead = ( int8_t * ) pxNewQueue;
    }
    else
    {
        /* 标准队列存储区配置 */
        pxNewQueue->pcHead = ( int8_t * ) pucQueueStorage;
    }

    /* 基础参数注入 ---------------------------------------------------------*/
    pxNewQueue->uxLength = uxQueueLength;   // 设置队列容量
    pxNewQueue->uxItemSize = uxItemSize;    // 设置单个元素尺寸

    /* 调用通用重置函数初始化队列结构 */
    ( void ) xQueueGenericReset( pxNewQueue, pdTRUE );  // 第二参数pdTRUE表示新队列

    /* 调试追踪支持（需配置configUSE_TRACE_FACILITY ==1）----------------------*/
    #if ( configUSE_TRACE_FACILITY == 1 )
    {
        pxNewQueue->ucQueueType = ucQueueType;  // 注入队列类型标识
    }
    #endif

    /* 队列集支持初始化（需配置configUSE_QUEUE_SETS ==1）----------------------*/
    #if( configUSE_QUEUE_SETS == 1 )
    {
        pxNewQueue->pxQueueSetContainer = NULL;  // 初始化队列集关联指针
    }
    #endif

    traceQUEUE_CREATE( pxNewQueue );  // 追踪队列创建事件
}

/* 互斥量初始化模块（需configUSE_MUTEXES ==1）--------------------------------------*/
#if( configUSE_MUTEXES == 1 )

    /**
     * @brief 互斥量队列特殊配置器
     * @param pxNewQueue 基础队列对象指针
     * 功能说明：
     * - 添加互斥量专用参数（持有者追踪、递归计数器）
     * - 配置优先继承机制所需的基础参数
     * - 发送初始信号量值
     */
    static void prvInitialiseMutex( Queue_t *pxNewQueue )
    {
        if( pxNewQueue != NULL )
        {
            /* 互斥量参数覆盖配置 ---------------------------------------------*/
            pxNewQueue->pxMutexHolder = NULL;          // 初始无持有者
            pxNewQueue->uxQueueType = queueQUEUE_IS_MUTEX;  // 类型标识

            /* 递归互斥量计数器初始化 */
            pxNewQueue->u.uxRecursiveCallCount = 0;   // 递归调用计数器归零

            traceCREATE_MUTEX( pxNewQueue );  // 追踪互斥量创建事件

            /* 初始化队列状态（相当于释放互斥量）*/
            ( void ) xQueueGenericSend( pxNewQueue, 
                                      NULL,          // 无数据载荷
                                      ( TickType_t ) 0U,  // 无等待
                                      queueSEND_TO_BACK );
        }
        else
        {
            traceCREATE_MUTEX_FAILED();  // 追踪创建失败事件
        }
    }

#endif /* 互斥量支持结束 */

/* 动态互斥量创建接口（需同时启用互斥量和动态分配）-----------------------------------*/
#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )

    QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType )
    {
        Queue_t *pxNewQueue;
        /* 互斥量队列参数配置：
           uxMutexLength = 1（互斥量二进制信号）
           uxMutexSize = 0（无需数据存储）*/
        const UBaseType_t uxMutexLength = 1, uxMutexSize = 0;

        /* 创建基础队列结构 */
        pxNewQueue = ( Queue_t * ) xQueueGenericCreate( uxMutexLength,
                                                      uxMutexSize,
                                                      ucQueueType );
        /* 应用互斥量专用配置 */
        prvInitialiseMutex( pxNewQueue );

        return pxNewQueue;  // 返回互斥量句柄
    }

#endif /* 动态互斥量支持结束 */
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS互斥量高级功能模块                                                         */
/* 本模块实现以下核心功能：                                                           */
/* 1.静态内存分配方式互斥量创建（xQueueCreateMutexStatic）                           */
/* 2.互斥量持有者查询接口（xQueueGetMutexHolder）                                    */
/*-----------------------------------------------------------------------------------*/

/* 静态互斥量创建接口（需同时启用互斥量和静态分配）-----------------------------------*/
#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )

QueueHandle_t xQueueCreateMutexStatic( 
    const uint8_t ucQueueType,        /* 队列类型标识（调试追踪用）*/
    StaticQueue_t *pxStaticQueue )    /* 预分配的静态队列控制块指针 */
{
    Queue_t *pxNewQueue;
    /* 互斥量固定参数：
       uxMutexLength = 1（二进制信号量特性）
       uxMutexSize = 0（无数据存储区）*/
    const UBaseType_t uxMutexLength = 1, uxMutexSize = 0;

    /* 抑制编译器警告（当追踪功能关闭时ucQueueType未使用）*/
    ( void ) ucQueueType;

    /* 调用静态队列创建器（存储区设为NULL）*/
    pxNewQueue = ( Queue_t * ) xQueueGenericCreateStatic( uxMutexLength,
                                                        uxMutexSize,
                                                        NULL,        /* 无数据存储区 */
                                                        pxStaticQueue,
                                                        ucQueueType );
    /* 应用互斥量专用配置 */
    prvInitialiseMutex( pxNewQueue );

    return pxNewQueue;  // 返回静态分配的互斥量句柄
}

#endif /* 静态互斥量支持结束 */

/* 互斥量持有者查询接口（需同时启用互斥量和INCLUDE_xSemaphoreGetMutexHolder）------*/
#if ( ( configUSE_MUTEXES == 1 ) && ( INCLUDE_xSemaphoreGetMutexHolder == 1 ) )

void* xQueueGetMutexHolder( QueueHandle_t xSemaphore )
{
    void *pxReturn;

    /* 注意事项：
       - 本函数由xSemaphoreGetMutexHolder()调用，不应直接使用
       - 返回的持有者指针可能随着上下文切换立即失效
       - 最佳用途：检测当前任务是否为持有者 */
    
    taskENTER_CRITICAL();  // 进入临界区（保证原子读取）
    {
        /* 类型安全验证（确保参数确实是互斥量）*/
        if( ( ( Queue_t * ) xSemaphore )->uxQueueType == queueQUEUE_IS_MUTEX )
        {
            pxReturn = ( void * ) ( ( Queue_t * ) xSemaphore )->pxMutexHolder; // 当前持有者
        }
        else
        {
            pxReturn = NULL;  // 非互斥量返回空指针
        }
    }
    taskEXIT_CRITICAL();  // 退出临界区

    /* lint抑制说明：818警告因typedef类型特性产生的误报 */
    return pxReturn; /*lint !e818 */
}

#endif /* 互斥量持有者查询支持结束 */

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS递归互斥量操作模块                                                         */
/* 本模块实现以下核心功能：                                                           */
/* 1.递归互斥量的层次化释放机制（xQueueGiveMutexRecursive）                          */
/* 2.递归互斥量的嵌套获取机制（xQueueTakeMutexRecursive）                            */
/*-----------------------------------------------------------------------------------*/

/* 递归互斥量释放函数（需configUSE_RECURSIVE_MUTEXES==1）---------------------------*/
#if ( configUSE_RECURSIVE_MUTEXES == 1 )

BaseType_t xQueueGiveMutexRecursive( QueueHandle_t xMutex )
{
    BaseType_t xReturn;
    Queue_t * const pxMutex = ( Queue_t * ) xMutex;  // 类型转换控制块指针

    configASSERT( pxMutex );  // 参数有效性断言

    /* 持有者身份验证说明：
       - 只有持有者线程会调用该函数，非竞态情况下直接访问pxMutexHolder是安全的
       - 通过任务句柄比较验证当前任务是否持有者 */
    if( pxMutex->pxMutexHolder == ( void * ) xTaskGetCurrentTaskHandle() ) /*lint !e961 */
    {
        traceGIVE_MUTEX_RECURSIVE( pxMutex );  // 追踪递归释放事件

        /* 递归计数器递减（无下溢保护，因持有者存在时计数器>=1）*/
        pxMutex->u.uxRecursiveCallCount--;

        /* 递归层级归零检查 -----------------------------------------------------*/
        if( pxMutex->u.uxRecursiveCallCount == 0 )
        {
            /* 实际释放互斥量（通过标准队列发送机制）*/
            ( void ) xQueueGenericSend( pxMutex, 
                                      NULL,  // 无数据载荷
                                      queueMUTEX_GIVE_BLOCK_TIME,  // 立即返回
                                      queueSEND_TO_BACK );
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未归零分支）
        }

        xReturn = pdPASS;  // 操作成功
    }
    else
    {
        /* 非持有者尝试释放互斥量（非法操作）*/
        xReturn = pdFAIL;  // 操作失败
        traceGIVE_MUTEX_RECURSIVE_FAILED( pxMutex );  // 追踪失败事件
    }

    return xReturn;
}

#endif /* 递归互斥量释放支持结束 */

/* 递归互斥量获取函数（需configUSE_RECURSIVE_MUTEXES==1）---------------------------*/
#if ( configUSE_RECURSIVE_MUTEXES == 1 )

BaseType_t xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xTicksToWait )
{
    BaseType_t xReturn;
    Queue_t * const pxMutex = ( Queue_t * ) xMutex;  // 类型转换控制块指针

    configASSERT( pxMutex );  // 参数有效性断言

    traceTAKE_MUTEX_RECURSIVE( pxMutex );  // 追踪递归获取事件

    /* 持有者身份验证（同释放函数的线程安全逻辑）*/
    if( pxMutex->pxMutexHolder == ( void * ) xTaskGetCurrentTaskHandle() ) /*lint !e961 */
    {
        /* 当前任务已持有：递增递归层级 */
        pxMutex->u.uxRecursiveCallCount++;
        xReturn = pdPASS;  // 直接返回成功
    }
    else
    {
        /* 首次获取或跨任务获取：通过标准接收机制获取 */
        xReturn = xQueueGenericReceive( pxMutex, 
                                      NULL,  // 无数据接收
                                      xTicksToWait,  // 用户指定阻塞时间
                                      pdFALSE );  // 非内核调用模式

        /* 获取成功后的处理 -----------------------------------------------------*/
        if( xReturn != pdFAIL )
        {
            /* 设置持有者并初始化递归计数器 */
            pxMutex->u.uxRecursiveCallCount = 1;  // 这里应改为直接递增？原代码有++操作？
        }
        else
        {
            traceTAKE_MUTEX_RECURSIVE_FAILED( pxMutex );  // 追踪失败事件
        }
    }

    return xReturn;
}

#endif /* 递归互斥量获取支持结束 */

/*-----------------------------------------------------------------------------------*/
/* FreeRTOS计数信号量创建模块                                                         */
/* 本模块实现以下核心功能：                                                           */
/* 1.静态分配方式计数信号量创建（xQueueCreateCountingSemaphoreStatic）              */
/* 2.动态分配方式计数信号量创建（xQueueCreateCountingSemaphore）                    */
/*-----------------------------------------------------------------------------------*/

/* 静态计数信号量创建接口（需同时启用计数信号量和静态分配）---------------------------*/
#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )

QueueHandle_t xQueueCreateCountingSemaphoreStatic( 
    const UBaseType_t uxMaxCount,        /* 信号量最大计数值（>0）*/
    const UBaseType_t uxInitialCount,     /* 信号量初始计数值（≤uxMaxCount）*/
    StaticQueue_t *pxStaticQueue )        /* 预分配的静态队列控制块指针 */
{
    QueueHandle_t xHandle;  // 返回的信号量句柄

    /* 参数合法性校验（调试模式生效）*/
    configASSERT( uxMaxCount != 0 );
    configASSERT( uxInitialCount <= uxMaxCount );

    /* 调用静态队列创建函数（关键参数说明）：
       uxQueueLength = uxMaxCount - 信号量容量设为最大计数值
       uxItemSize = 0 - 信号量无需数据存储区
       pucQueueStorage = NULL - 无需存储区指针 */
    xHandle = xQueueGenericCreateStatic( uxMaxCount,
                                        queueSEMAPHORE_QUEUE_ITEM_LENGTH,
                                        NULL,
                                        pxStaticQueue,
                                        queueQUEUE_TYPE_COUNTING_SEMAPHORE );

    if( xHandle != NULL )
    {
        /* 通过uxMessagesWaiting字段承载信号量计数值 */
        ( ( Queue_t * ) xHandle )->uxMessagesWaiting = uxInitialCount;
        traceCREATE_COUNTING_SEMAPHORE();  // 追踪创建成功事件
    }
    else
    {
        traceCREATE_COUNTING_SEMAPHORE_FAILED();  // 追踪创建失败事件
    }

    return xHandle;  // 返回初始化后的信号量句柄
}

#endif /* 静态计数信号量支持结束 */

/* 动态计数信号量创建接口（需同时启用计数信号量和动态分配）---------------------------*/
#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )

QueueHandle_t xQueueCreateCountingSemaphore( 
    const UBaseType_t uxMaxCount,        /* 信号量最大计数值（>0）*/
    const UBaseType_t uxInitialCount )    /* 信号量初始计数值（≤uxMaxCount）*/
{
    QueueHandle_t xHandle;  // 返回的信号量句柄

    /* 参数合法性校验（调试模式生效）*/
    configASSERT( uxMaxCount != 0 );
    configASSERT( uxInitialCount <= uxMaxCount );

    /* 调用动态队列创建函数（关键参数说明）：
       uxQueueLength = uxMaxCount - 信号量容量设为最大计数值
       uxItemSize = 0 - 信号量无需数据存储区 */
    xHandle = xQueueGenericCreate( uxMaxCount,
                                 queueSEMAPHORE_QUEUE_ITEM_LENGTH,
                                 queueQUEUE_TYPE_COUNTING_SEMAPHORE );

    if( xHandle != NULL )
    {
        /* 通过uxMessagesWaiting字段承载信号量计数值 */
        ( ( Queue_t * ) xHandle )->uxMessagesWaiting = uxInitialCount;
        traceCREATE_COUNTING_SEMAPHORE();  // 追踪创建成功事件
    }
    else
    {
        traceCREATE_COUNTING_SEMAPHORE_FAILED();  // 追踪创建失败事件
    }

    return xHandle;  // 返回初始化后的信号量句柄
}

#endif /* 动态计数信号量支持结束 */
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS通用队列发送方法 - xQueueGenericSend                                        */
/* 本函数实现以下核心功能：                                                           */
/* 1.支持多种发送模式：队尾、队首、覆盖写入（queueOVERWRITE）                        */
/* 2.实现队列满时的任务阻塞机制与超时管理                                             */
/* 3.处理队列集(Queue Set)关联通知                                                   */
/* 4.优先级继承与任务唤醒的智能处理                                                   */
/*-----------------------------------------------------------------------------------*/

BaseType_t xQueueGenericSend( 
    QueueHandle_t xQueue,              /* 目标队列句柄 */
    const void * const pvItemToQueue, /* 待发送数据指针 */
    TickType_t xTicksToWait,          /* 阻塞超时时间（tick数）*/
    const BaseType_t xCopyPosition )  /* 发送位置标识 */
{
    BaseType_t xEntryTimeSet = pdFALSE, xYieldRequired;  // 超时状态标记与切换标记
    TimeOut_t xTimeOut;               // 超时控制结构体
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;  // 类型转换队列控制块

    /* 参数有效性验证 ---------------------------------------------------------*/
    configASSERT( pxQueue );  // 确保队列句柄有效
    configASSERT( !( ( pvItemToQueue == NULL ) && ( pxQueue->uxItemSize != 0 ) ) );  // 非空检查
    configASSERT( !( ( xCopyPosition == queueOVERWRITE ) && ( pxQueue->uxLength != 1 ) ) );  // 覆盖模式校验

    /* 调度器状态验证（避免挂起状态下误操作）------------------------------------*/
    #if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )
    {
        configASSERT( !( ( xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED ) 
                        && ( xTicksToWait != 0 ) ) );  // 非运行状态禁止阻塞
    }
    #endif

    /* 主处理循环（可能多次尝试）------------------------------------------------*/
    for( ;; )
    {
        // 进入临界区（保护队列状态的一致性）
        taskENTER_CRITICAL();
        {
            /* 队列空间检查逻辑 -------------------------------------------------*/
            if( ( pxQueue->uxMessagesWaiting < pxQueue->uxLength ) 
               || ( xCopyPosition == queueOVERWRITE ) )  // 覆盖模式不限容量
            {
                traceQUEUE_SEND( pxQueue );  // 追踪队列发送事件

                /* 数据写入队列（返回是否需要触发任务切换）*/
                xYieldRequired = prvCopyDataToQueue( pxQueue, 
                                                   pvItemToQueue, 
                                                   xCopyPosition );

                /* 队列集处理分支 -------------------------------------------------*/
                #if ( configUSE_QUEUE_SETS == 1 )
                {
                    if( pxQueue->pxQueueSetContainer != NULL )
                    {
                        /* 触发队列集通知并处理任务切换 */
                        if( prvNotifyQueueSetContainer( pxQueue, 
                                                      xCopyPosition ) != pdFALSE )
                        {
                            queueYIELD_IF_USING_PREEMPTION();  // 优先级驱动切换
                        }
                        else
                        {
                            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
                        }
                    }
                    else  // 非队列集关联队列处理
                    {
                        /* 唤醒接收阻塞任务处理 */
                        if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) 
                                                                     == pdFALSE )
                        {
                            if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) 
                                                                     != pdFALSE )
                            {
                                queueYIELD_IF_USING_PREEMPTION();  // 立刻切换
                            }
                        }
                        else if( xYieldRequired != pdFALSE )  // 互斥量优先级继承场景
                        {
                            queueYIELD_IF_USING_PREEMPTION();  // 特殊场景切换
                        }
                    }
                }
                #else  /* 未启用队列集的分支 */
                {
                    /* 标准接收任务唤醒逻辑（同上）*/
                    if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) 
                                                                     == pdFALSE )
                    {
                        if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) 
                                                                     != pdFALSE )
                        {
                            queueYIELD_IF_USING_PREEMPTION();
                        }
                    }
                    else if( xYieldRequired != pdFALSE )
                    {
                        queueYIELD_IF_USING_PREEMPTION();
                    }
                }
                #endif /* configUSE_QUEUE_SETS */

                taskEXIT_CRITICAL();
                return pdPASS;  // 成功发送返回
            }
            else  // 队列满处理分支
            {
                /* 非阻塞模式直接返回错误 */
                if( xTicksToWait == 0 )
                {
                    taskEXIT_CRITICAL();
                    traceQUEUE_SEND_FAILED( pxQueue );  // 追踪失败
                    return errQUEUE_FULL;
                }
                else if( xEntryTimeSet == pdFALSE )  // 初次阻塞初始化超时结构
                {
                    vTaskSetTimeOutState( &xTimeOut );  // 记录初始时间戳
                    xEntryTimeSet = pdTRUE;
                }
            }
        } // 结束临界区保护
        taskEXIT_CRITICAL();

        /* 阻塞管理阶段（释放队列锁后执行）----------------------------------------*/
        vTaskSuspendAll();   // 挂起调度器
        prvLockQueue( pxQueue );  // 锁定队列操作

        /* 超时检测（更新剩余等待时间）--------------------------------------------*/
        if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
        {
            if( prvIsQueueFull( pxQueue ) != pdFALSE )  // 再次确认队列状态
            {
                traceBLOCKING_ON_QUEUE_SEND( pxQueue );  // 追踪阻塞事件
                /* 将任务加入等待发送列表 */
                vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToSend ), 
                                      xTicksToWait );
                prvUnlockQueue( pxQueue );  // 解锁队列允许其他操作

                /* 恢复调度并处理潜在切换 */
                if( xTaskResumeAll() == pdFALSE )
                {
                    portYIELD_WITHIN_API();  // 触发上下文切换
                }
            }
            else  // 队列突然有空位（其他任务释放资源）
            {
                prvUnlockQueue( pxQueue );
                ( void ) xTaskResumeAll();  // 无切换恢复
            }
        }
        else  // 等待超时处理
        {
            prvUnlockQueue( pxQueue );
            ( void ) xTaskResumeAll();
            traceQUEUE_SEND_FAILED( pxQueue );
            return errQUEUE_FULL;
        }
    } // end for( ;; )
}
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS中断级队列发送方法 - xQueueGenericSendFromISR                               */
/* 本函数实现以下核心功能：                                                           */
/* 1.支持从中断上下文发送数据到队列，实现非阻塞操作                                  */
/* 2.处理队列集(Queue Set)关联通知机制                                                */
/* 3.提供高效的任务唤醒指示（通过pxHigherPriorityTaskWoken）                        */
/* 4.适应不同中断优先级架构的安全访问控制                                            */
/*-----------------------------------------------------------------------------------*/

BaseType_t xQueueGenericSendFromISR( 
    QueueHandle_t xQueue,                  /* 目标队列句柄 */
    const void * const pvItemToQueue,      /* 待发送数据指针 */
    BaseType_t * const pxHigherPriorityTaskWoken, /* 高优先级任务唤醒标志输出 */
    const BaseType_t xCopyPosition )       /* 发送位置标识 */
{
    BaseType_t xReturn;                    /* 函数返回状态 */
    UBaseType_t uxSavedInterruptStatus;    /* 中断状态保存 */
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;  /* 类型转换队列控制块 */

    /* 参数有效性验证 */
    configASSERT( pxQueue );  // 确保队列句柄有效
    configASSERT( !( ( pvItemToQueue == NULL ) && ( pxQueue->uxItemSize != 0 ) ) );  // 非空检查
    configASSERT( !( ( xCopyPosition == queueOVERWRITE ) && ( pxQueue->uxLength != 1 ) ) );  // 覆盖模式校验

    /* 中断优先级验证 ---------------------------------------------------------
       该宏确保当前中断优先级不高于系统最大API调用优先级，保证内核数据结构安全 */
    portASSERT_IF_INTERRUPT_PRIORITY_INVALID(); 

    /* 保存并屏蔽中断（进入临界区）--------------------------------------------
       注意：使用专用中断屏蔽函数而非常规taskENTER_CRITICAL */
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        if( ( pxQueue->uxMessagesWaiting < pxQueue->uxLength ) 
           || ( xCopyPosition == queueOVERWRITE ) )
        {
            const int8_t cTxLock = pxQueue->cTxLock;  // 获取当前发送锁状态

            traceQUEUE_SEND_FROM_ISR( pxQueue );  // 追踪ISR发送事件

            /* 执行数据拷贝（不会引发优先级继承，因为信号量使用单独的Give方法）*/
            ( void ) prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );

            /* 队列未锁定时的处理流程 -----------------------------------------*/
            if( cTxLock == queueUNLOCKED )  // 当前发送未被锁定
            {
                #if ( configUSE_QUEUE_SETS == 1 )
                {
                    if( pxQueue->pxQueueSetContainer != NULL )  // 属于队列集
                    {
                        if( prvNotifyQueueSetContainer( pxQueue, xCopyPosition ) != pdFALSE )
                        {
                            /* 队列集有更高优先级任务被唤醒 */
                            if( pxHigherPriorityTaskWoken != NULL )
                            {
                                *pxHigherPriorityTaskWoken = pdTRUE;  // 设置切换标志
                            }
                        }
                    }
                    else  // 标准队列处理
                    {
                        if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                        {
                            /* 唤醒接收阻塞任务（可能位于挂起状态）*/
                            if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) 
                                                                      != pdFALSE )
                            {
                                if( pxHigherPriorityTaskWoken != NULL )
                                {
                                    *pxHigherPriorityTaskWoken = pdTRUE;  // 标记需要上下文切换
                                }
                            }
                        }
                    }
                }
                #else /* 未启用队列集的分支 */
                {
                    if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                    {
                        if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) 
                                                                  != pdFALSE )
                        {
                            if( pxHigherPriorityTaskWoken != NULL )
                            {
                                *pxHigherPriorityTaskWoken = pdTRUE;  // 设置切换标志
                            }
                        }
                    }
                }
                #endif /* configUSE_QUEUE_SETS */
            }
            else  // 队列处于发送锁定状态
            {
                /* 递增发送锁定计数器（解锁时会处理事件列表）*/
                pxQueue->cTxLock = ( int8_t ) ( cTxLock + 1 );
            }

            xReturn = pdPASS;  // 发送成功
        }
        else  // 队列已满且不允许覆盖
        {
            traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue );  // 追踪失败事件
            xReturn = errQUEUE_FULL;  // 返回队列满错误
        }
    }
    /* 恢复中断状态（退出临界区）----------------------------------------------*/
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    return xReturn;  // 返回操作结果
}

/*-----------------------------------------------------------------------------------*/
/* FreeRTOS中断级信号量释放方法 - xQueueGiveFromISR                                   */
/* 本函数实现以下核心功能：                                                           */
/* 1.专用于计数信号量的ISR释放操作（item size=0）                                    */
/* 2.严格禁止互斥量的中断释放操作（优先级继承无效）                                   */
/* 3.集成队列集通知机制与高效的任务唤醒标记                                           */
/*-----------------------------------------------------------------------------------*/

BaseType_t xQueueGiveFromISR( 
    QueueHandle_t xQueue,                 /* 目标信号量句柄 */
    BaseType_t * const pxHigherPriorityTaskWoken ) /* 高优先级任务唤醒标志输出 */
{
    BaseType_t xReturn;                    /* 函数返回状态 */
    UBaseType_t uxSavedInterruptStatus;    /* 中断状态保存 */
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;  /* 类型转换信号量控制块 */

    /* 参数有效性验证 ---------------------------------------------------------*/
    configASSERT( pxQueue );  // 确保信号量句柄有效
    configASSERT( pxQueue->uxItemSize == 0 );  // 强制仅用于无数据队列（信号量）
    configASSERT( !( ( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX ) 
                   && ( pxQueue->pxMutexHolder != NULL ) ) );  // 互斥量禁用检查

    /* 中断优先级验证 ---------------------------------------------------------
       确保当前中断优先级不高于系统最大API调用优先级，防止破坏内核数据结构 */
    portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

    /* 保存并屏蔽中断（进入临界区）--------------------------------------------
       使用体系结构专用中断屏蔽函数保证原子操作 */
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        const UBaseType_t uxMessagesWaiting = pxQueue->uxMessagesWaiting;  // 获取当前计数值

        /* 信号量资源检查（等效队列非满检查）*/
        if( uxMessagesWaiting < pxQueue->uxLength )  // 未达最大许可值
        {
            const int8_t cTxLock = pxQueue->cTxLock;  // 获取当前发送锁状态

            traceQUEUE_SEND_FROM_ISR( pxQueue );  // 追踪信号量释放事件

            /* 直接递增信号量计数器（无需数据拷贝操作）*/
            pxQueue->uxMessagesWaiting = uxMessagesWaiting + 1;

            /* 队列未锁定处理流程（唤醒等待任务）*/
            if( cTxLock == queueUNLOCKED )  
            {
                #if ( configUSE_QUEUE_SETS == 1 )
                {
                    if( pxQueue->pxQueueSetContainer != NULL )  // 属于队列集
                    {
                        if( prvNotifyQueueSetContainer( pxQueue, queueSEND_TO_BACK ) != pdFALSE )
                        {
                            /* 队列集有更高优先级任务被唤醒 */
                            if( pxHigherPriorityTaskWoken != NULL )
                            {
                                *pxHigherPriorityTaskWoken = pdTRUE;  // 设置切换标志
                            }
                        }
                    }
                    else  // 标准信号量处理
                    {
                        /* 唤醒等待接收信号量的首个任务 */
                        if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                        {
                            if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) 
                                                                  != pdFALSE )
                            {
                                if( pxHigherPriorityTaskWoken != NULL )
                                {
                                    *pxHigherPriorityTaskWoken = pdTRUE;  // 标记需要上下文切换
                                }
                            }
                        }
                    }
                }
                #else /* 未启用队列集分支 */
                {
                    if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                    {
                        /* 标准任务唤醒逻辑 */
                        if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) 
                                                              != pdFALSE )
                        {
                            if( pxHigherPriorityTaskWoken != NULL )
                            {
                                *pxHigherPriorityTaskWoken = pdTRUE;  // 设置切换标志
                            }
                        }
                    }
                }
                #endif /* configUSE_QUEUE_SETS */
            }
            else  // 队列处于发送锁定状态
            {
                /* 递增发送锁定计数器（实际唤醒操作延迟到解锁时执行）*/
                pxQueue->cTxLock = ( int8_t ) ( cTxLock + 1 );
            }

            xReturn = pdPASS;  // 信号量释放成功
        }
        else  // 信号量计数已满（达到uxLength）
        {
            traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue );  // 追踪失败事件
            xReturn = errQUEUE_FULL;  // 返回资源耗尽错误
        }
    }
    /* 恢复中断状态（退出临界区）---------------------------------------------*/
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    return xReturn;  // 返回操作结果
}
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS通用队列接收方法 - xQueueGenericReceive                                     */
/* 本函数实现以下核心功能：                                                           */
/* 1.支持标准队列数据接收（移除项目）及查看模式（peek）                               */
/* 2.处理互斥量接收时的优先级继承逻辑                                                 */
/* 3.实现阻塞等待与超时管理机制                                                       */
/* 4.智能唤醒发送阻塞任务                                                             */
/*-----------------------------------------------------------------------------------*/

BaseType_t xQueueGenericReceive( QueueHandle_t xQueue,         /* 目标队列句柄 */
                               void * const pvBuffer,        /* 接收数据缓冲区 */
                               TickType_t xTicksToWait,       /* 阻塞超时时间 */
                               const BaseType_t xJustPeeking )/* 接收模式标记 */
{
    BaseType_t xEntryTimeSet = pdFALSE;      /* 超时状态初始化标记 */
    TimeOut_t xTimeOut;                     /* 超时控制结构体 */
    int8_t *pcOriginalReadPosition;        /* 查看模式读指针备份 */
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;  /* 类型转换队列控制块 */

    /* 参数有效性验证 */
    configASSERT( pxQueue );  // 确保队列句柄有效
    configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != 0 ) ) );  // 缓冲区验证
    #if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )
    {
        configASSERT( !( ( xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED ) 
                       && ( xTicksToWait != 0 ) ) );  // 挂起状态阻塞检查
    }
    #endif

    /* 主处理循环（可能包含多次尝试）*/
    for( ;; )
    {
        /* 进入临界区保护队列状态 ---------------------------------------------------*/
        taskENTER_CRITICAL();
        {
            const UBaseType_t uxMessagesWaiting = pxQueue->uxMessagesWaiting;  // 当前队列项数

            if( uxMessagesWaiting > 0 )  // 队列非空
            {
                /* 查看模式读指针备份（用于恢复操作） */
                pcOriginalReadPosition = pxQueue->u.pcReadFrom;

                /* 数据拷贝操作（环形缓冲区处理） */
                prvCopyDataFromQueue( pxQueue, pvBuffer );

                if( xJustPeeking == pdFALSE )  // 标准接收模式
                {
                    traceQUEUE_RECEIVE( pxQueue );  // 追踪接收事件

                    /* 更新队列计数器 */
                    pxQueue->uxMessagesWaiting = uxMessagesWaiting - 1;

                    #if ( configUSE_MUTEXES == 1 )
                    {
                        if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )  // 互斥量处理
                        {
                            /* 更新互斥量持有者信息并增加持有计数 */
                            pxQueue->pxMutexHolder = ( int8_t * ) pvTaskIncrementMutexHeldCount(); 
                        }
                    }
                    #endif

                    /* 唤醒发送阻塞任务 -------------------------------------------------*/
                    if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
                    {
                        if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
                        {
                            queueYIELD_IF_USING_PREEMPTION();  // 触发任务切换
                        }
                    }
                }
                else  // 查看模式（peek）
                {
                    traceQUEUE_PEEK( pxQueue );  // 追踪查看事件

                    /* 恢复读指针位置（数据保留在队列中） */
                    pxQueue->u.pcReadFrom = pcOriginalReadPosition;

                    /* 尝试唤醒其他接收任务（可能多个任务在等待查看） */
                    if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                    {
                        if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                        {
                            queueYIELD_IF_USING_PREEMPTION();  // 触发任务切换
                        }
                    }
                }

                taskEXIT_CRITICAL();
                return pdPASS;  // 成功接收返回
            }
            else  // 队列空处理分支
            {
                if( xTicksToWait == 0 )  // 非阻塞模式立即返回
                {
                    taskEXIT_CRITICAL();
                    traceQUEUE_RECEIVE_FAILED( pxQueue );  // 追踪失败事件
                    return errQUEUE_EMPTY;  // 队列空错误
                }
                else if( xEntryTimeSet == pdFALSE )  // 首次阻塞初始化超时
                {
                    vTaskSetTimeOutState( &xTimeOut );  // 记录初始时间戳
                    xEntryTimeSet = pdTRUE;
                }
            }
        }
        taskEXIT_CRITICAL();

        /* 阻塞管理阶段 -----------------------------------------------------------*/
        vTaskSuspendAll();          // 挂起任务调度器
        prvLockQueue( pxQueue );    // 锁定队列操作

        /* 超时检测（动态计算剩余等待时间） */
        if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
        {
            if( prvIsQueueEmpty( pxQueue ) != pdFALSE )  // 队列依然为空
            {
                traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue );  // 追踪阻塞事件

                #if ( configUSE_MUTEXES == 1 )
                {
                    if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )  // 互斥量优先级继承处理
                    {
                        taskENTER_CRITICAL();
                        {
                            vTaskPriorityInherit( ( void * ) pxQueue->pxMutexHolder );  // 优先级继承
                        }
                        taskEXIT_CRITICAL();
                    }
                }
                #endif

                /* 加入接收等待列表 */
                vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToReceive ), xTicksToWait );
                prvUnlockQueue( pxQueue );  // 解锁队列

                /* 恢复调度并处理潜在切换 */
                if( xTaskResumeAll() == pdFALSE )
                {
                    portYIELD_WITHIN_API();  // 触发上下文切换
                }
            }
            else  // 队列状态在临界区外发生变化
            {
                prvUnlockQueue( pxQueue );
                ( void ) xTaskResumeAll();  // 无任务切换恢复
            }
        }
        else  // 超时处理
        {
            prvUnlockQueue( pxQueue );
            ( void ) xTaskResumeAll();

            if( prvIsQueueEmpty( pxQueue ) != pdFALSE )  // 最终队列仍为空
            {
                traceQUEUE_RECEIVE_FAILED( pxQueue );
                return errQUEUE_EMPTY;
            }
        }
    } // end for( ;; )
}
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS中断级队列接收方法 - xQueueReceiveFromISR                                  */
/* 本函数实现以下核心功能：                                                           */
/* 1.从中断上下文安全接收队列数据（非阻塞模式）                                       */
/* 2.处理队列锁定状态下的接收计数管理                                                 */
/* 3.智能唤醒发送阻塞任务并提供上下文切换建议                                          */
/*-----------------------------------------------------------------------------------*/

BaseType_t xQueueReceiveFromISR( 
    QueueHandle_t xQueue,         /* 目标队列句柄 */
    void * const pvBuffer,        /* 接收数据缓冲区 */
    BaseType_t * const pxHigherPriorityTaskWoken ) /* 上下文切换建议标志 */
{
    BaseType_t xReturn;                    /* 操作结果 */
    UBaseType_t uxSavedInterruptStatus;   /* 中断状态保存 */
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;  /* 类型转换队列控制块 */

    /* 参数有效性验证 */
    configASSERT( pxQueue );  // 确保队列句柄有效
    configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != 0 ) ) );  // 缓冲区验证

    /* 中断优先级校验 ----------------------------------------------------------
       验证当前中断优先级未超过系统最大API调用优先级，确保内核数据结构安全 */
    portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

    /* 保存并屏蔽中断（进入临界区）---------------------------------------------
       使用体系专用中断屏蔽函数保证原子操作 */
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        const UBaseType_t uxMessagesWaiting = pxQueue->uxMessagesWaiting;  // 当前队列项数

        if( uxMessagesWaiting > 0 )  // 队列有数据可接收
        {
            const int8_t cRxLock = pxQueue->cRxLock;  // 获取当前接收锁状态

            traceQUEUE_RECEIVE_FROM_ISR( pxQueue );  // 追踪ISR接收事件

            /* 执行数据拷贝 */
            prvCopyDataFromQueue( pxQueue, pvBuffer );  
            pxQueue->uxMessagesWaiting = uxMessagesWaiting - 1;  // 更新计数器

            /* 接收锁状态处理分支 */
            if( cRxLock == queueUNLOCKED )  // 队列未锁定
            {
                if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
                {
                    /* 唤醒发送阻塞任务 */
                    if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
                    {
                        /* 高优先级任务被唤醒，设置切换建议标志 */
                        if( pxHigherPriorityTaskWoken != NULL )
                        {
                            *pxHigherPriorityTaskWoken = pdTRUE;
                        }
                    }
                }
            }
            else  // 队列处于接收锁定状态
            {
                /* 递增接收锁定计数器（解锁时处理唤醒操作）*/
                pxQueue->cRxLock = ( int8_t ) ( cRxLock + 1 );
            }

            xReturn = pdPASS;  // 接收成功
        }
        else  // 队列空
        {
            xReturn = pdFAIL;  // 接收失败
            traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue );  // 追踪失败事件
        }
    }
    /* 恢复中断状态（退出临界区）-----------------------------------------------*/
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    return xReturn;  // 返回操作结果
}
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS队列中断级查看及状态查询模块                                                */
/* 包含以下功能：                                                                     */
/* 1.xQueuePeekFromISR - 中断上下文查看队列首部数据                                   */
/* 2.uxQueueMessagesWaiting - 获取队列当前消息数（任务级安全）                        */
/* 3.uxQueueSpacesAvailable - 获取队列剩余容量（任务级安全）                          */
/* 4.uxQueueMessagesWaitingFromISR - 获取队列消息数（中断级版本）                      */
/*-----------------------------------------------------------------------------------*/

/* 中断级队列查看函数 ------------------------------------------------------------*/
BaseType_t xQueuePeekFromISR( QueueHandle_t xQueue, void * const pvBuffer )
{
    BaseType_t xReturn;                    /* 操作结果 */
    UBaseType_t uxSavedInterruptStatus;   /* 中断状态保存 */
    int8_t *pcOriginalReadPosition;       /* 读指针备份 */
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;  /* 类型转换队列控制块 */

    /* 参数三重校验 */
    configASSERT( pxQueue );  // 队列句柄有效性检查
    configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != 0 ) ) );  // 缓冲区合法性
    configASSERT( pxQueue->uxItemSize != 0 );  // 禁用信号量查看（信号量无数据）

    /* 中断优先级验证（保护内核数据结构完整性）*/
    portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

    /* 进入临界区（体系专用中断屏蔽）*/
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        if( pxQueue->uxMessagesWaiting > 0 )  // 队列非空检测
        {
            traceQUEUE_PEEK_FROM_ISR( pxQueue );  // 追踪查看事件

            /* 读指针处理三步曲 */
            pcOriginalReadPosition = pxQueue->u.pcReadFrom;  // 备份当前读位置
            prvCopyDataFromQueue( pxQueue, pvBuffer );      // 执行数据拷贝
            pxQueue->u.pcReadFrom = pcOriginalReadPosition; // 恢复读指针（保持队列不变）

            xReturn = pdPASS;  // 操作成功
        }
        else  // 队列空处理
        {
            xReturn = pdFAIL;  // 操作失败
            traceQUEUE_PEEK_FROM_ISR_FAILED( pxQueue );  // 追踪失败事件
        }
    }
    /* 退出临界区（恢复中断环境）*/
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    return xReturn;  // 返回操作状态
}

/* 任务级队列消息数查询 ----------------------------------------------------------*/
UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue )
{
    UBaseType_t uxReturn;   /* 返回消息数量 */
    
    configASSERT( xQueue );  // 队列句柄有效性断言

    /* 进入临界区保护（防止任务切换干扰）*/
    taskENTER_CRITICAL();
    {
        // 直接访问队列消息计数器（原子操作保障）
        uxReturn = ( ( Queue_t * ) xQueue )->uxMessagesWaiting;  
    }
    taskEXIT_CRITICAL();

    return uxReturn;  // 返回当前消息数
} /*lint !e818 类型限定符规则例外（因队列句柄为typedef）*/

/* 任务级队列剩余容量计算 ----------------------------------------------------------*/
UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue )
{
    UBaseType_t uxReturn;    /* 返回剩余容量 */
    Queue_t *pxQueue = ( Queue_t * ) xQueue;  // 类型转换队列控制块

    configASSERT( pxQueue );  // 队列句柄有效性验证

    /* 进入临界区保护计算过程 */
    taskENTER_CRITICAL();
    {
        // 剩余容量 = 总长度 - 当前消息数
        uxReturn = pxQueue->uxLength - pxQueue->uxMessagesWaiting;
    }
    taskEXIT_CRITICAL();

    return uxReturn;  // 返回可用空间数
} /*lint !e818 类型限定符规则例外 */

/* 中断级队列消息数查询 ----------------------------------------------------------*/
UBaseType_t uxQueueMessagesWaitingFromISR( const QueueHandle_t xQueue )
{
    UBaseType_t uxReturn;  /* 返回消息数量 */
    
    configASSERT( xQueue );  // 队列句柄有效性检查

    /* 直接原子访问（无需临界区保护，ISR自身不可抢占）*/
    uxReturn = ( ( Queue_t * ) xQueue )->uxMessagesWaiting;

    return uxReturn;  // 返回即时消息数
} /*lint !e818 类型限定符规则例外 */
/*-----------------------------------------------------------------------------------*/
/* FreeRTOS队列生命周期管理及调试追踪模块                                              */
/* 本模块包含以下核心功能：                                                           */
/* 1.队列删除及资源回收(vQueueDelete)                                                */
/* 2.调试追踪功能支持(队列编号、类型管理)                                              */
/*-----------------------------------------------------------------------------------*/

/* 队列删除函数 ----------------------------------------------------------------*/
void vQueueDelete( QueueHandle_t xQueue )
{
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;  // 类型转换队列控制块

    configASSERT( pxQueue );           // 确保队列句柄有效性
    traceQUEUE_DELETE( pxQueue );       // 追踪队列删除事件

    /* 队列注册表处理（需配置configQUEUE_REGISTRY_SIZE >0）*/
    #if ( configQUEUE_REGISTRY_SIZE > 0 )
    {
        vQueueUnregisterQueue( pxQueue );  // 从注册表中注销队列
    }
    #endif

    /* 动态内存回收处理（根据编译选项） */
    #if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 0 ) )
    {
        /* 仅动态分配模式：直接释放内存 */
        vPortFree( pxQueue );  // 调用端口内存释放函数
    }
    #elif( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
    {
        /* 混合分配模式：检查分配标识 */
        if( pxQueue->ucStaticallyAllocated == pdFALSE )  // 动态分配队列
        {
            vPortFree( pxQueue );
        }
        else  // 静态分配队列不释放
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记
        }
    }
    #else /* 仅静态分配模式 */
    {
        /* 静默处理未使用参数（避免编译器警告）*/
        ( void ) pxQueue;  // 无实际内存回收操作
    }
    #endif /* 动态分配支持检查结束 */
}

/* 追踪功能支持函数（需configUSE_TRACE_FACILITY ==1）---------------------------*/
#if ( configUSE_TRACE_FACILITY == 1 )

    /* 获取队列唯一编号 -------------------------------------------------------*/
    UBaseType_t uxQueueGetQueueNumber( QueueHandle_t xQueue )
    {
        return ( ( Queue_t * ) xQueue )->uxQueueNumber;  // 返回调试器可见的队列编号
    }

    /* 设置队列追踪编号 -------------------------------------------------------*/
    void vQueueSetQueueNumber( QueueHandle_t xQueue, UBaseType_t uxQueueNumber )
    {
        ( ( Queue_t * ) xQueue )->uxQueueNumber = uxQueueNumber;  // 注入自定义编号
    }

    /* 获取队列类型标识 -------------------------------------------------------*/
    uint8_t ucQueueGetQueueType( QueueHandle_t xQueue )
    {
        return ( ( Queue_t * ) xQueue )->ucQueueType;  // 返回队列类型枚举值
    }

#endif /* 追踪功能支持结束 */

/*-----------------------------------------------------------------------------------*/
/* 队列数据写入核心函数 - prvCopyDataToQueue                                          */
/* 本函数实现以下核心功能：                                                           */
/* 1.将数据写入队列的不同位置（队尾/队首/覆盖）                                      */
/* 2.处理互斥量优先级继承解除逻辑                                                     */
/* 3.维护环形缓冲区的指针状态                                                         */
/*-----------------------------------------------------------------------------------*/

static BaseType_t prvCopyDataToQueue( 
    Queue_t * const pxQueue,        /* 目标队列控制块 */
    const void *pvItemToQueue,      /* 待写入数据指针 */
    const BaseType_t xPosition )    /* 写入位置标识符 */
{
    BaseType_t xReturn = pdFALSE;    /* 优先级继承状态返回 */
    UBaseType_t uxMessagesWaiting;  /* 当前消息计数器缓存 */

    /* 注意：本函数在临界区中调用（需确保原子操作）*/

    uxMessagesWaiting = pxQueue->uxMessagesWaiting;  // 获取当前消息数

    /* 分支1：处理无数据项队列（信号量/互斥量）----------------------------------*/
    if( pxQueue->uxItemSize == 0 ) 
    {
        #if ( configUSE_MUTEXES == 1 )
        {
            if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )  // 互斥量处理
            {
                /* 解除优先级继承（返回是否降级）*/
                xReturn = xTaskPriorityDisinherit( ( void * ) pxQueue->pxMutexHolder );
                pxQueue->pxMutexHolder = NULL;  // 清除互斥量持有者
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（非互斥量分支）
            }
        }
        #endif /* 互斥量支持结束 */
    }
    /* 分支2：队尾写入模式（标准入队）------------------------------------------*/
    else if( xPosition == queueSEND_TO_BACK )
    {
        /* 执行数据拷贝（将数据写入写指针位置）*/
        ( void ) memcpy( ( void * ) pxQueue->pcWriteTo, pvItemToQueue, ( size_t ) pxQueue->uxItemSize ); /*lint !e961 !e418 MISRA exception as the casts are only redundant for some ports, plus previous logic ensures a null pointer can only be passed to memcpy() if the copy size is 0. */
        /* 更新写指针位置（指向下一个可用位置）*/
        pxQueue->pcWriteTo += pxQueue->uxItemSize;
        /* 环形缓冲区处理（写指针达到尾部时回绕到头部）*/
        if( pxQueue->pcWriteTo >= pxQueue->pcTail ) /*lint !e946 MISRA exception justified as comparison of pointers is the cleanest solution. */
        {
            pxQueue->pcWriteTo = pxQueue->pcHead;  // 回绕到队列头部
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未回绕分支）
        }
    }
    /* 分支3：队首写入或覆盖模式-----------------------------------------------------*/
    else
    {
        /* 执行数据拷贝（将数据写入读指针前一个位置）*/
        ( void ) memcpy( ( void * ) pxQueue->u.pcReadFrom, pvItemToQueue, ( size_t ) pxQueue->uxItemSize ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
        /* 更新读指针位置（指向下一个可用位置）*/
        pxQueue->u.pcReadFrom -= pxQueue->uxItemSize;
        /* 环形缓冲区处理（读指针小于头部时回绕到尾部）*/
        if( pxQueue->u.pcReadFrom < pxQueue->pcHead ) /*lint !e946 MISRA exception justified as comparison of pointers is the cleanest solution. */
        {
            pxQueue->u.pcReadFrom = ( pxQueue->pcTail - pxQueue->uxItemSize );  // 回绕到队列尾部
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未回绕分支）
        }

        /* 分支3-1：覆盖模式处理----------------------------------------------------*/
        if( xPosition == queueOVERWRITE )
        {
            if( uxMessagesWaiting > 0 )
            {
                /* 覆盖写入时不增加消息计数，但需减少计数以保持一致性*/
                --uxMessagesWaiting;  // 减少消息计数器
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（空队列分支）
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（非覆盖分支）
        }
    }

    /* 更新队列消息计数器（增加一个新消息）----------------------------------------*/
    pxQueue->uxMessagesWaiting = uxMessagesWaiting + 1;

    return xReturn;  // 返回优先级继承状态（仅互斥量相关）
}
/*-----------------------------------------------------------------------------------*/
/* 队列数据读取与队列解锁核心函数 - prvCopyDataFromQueue 和 prvUnlockQueue         */
/* 本函数实现以下核心功能：                                                           */
/* 1.prvCopyDataFromQueue - 从队列读取数据而不移除数据项（仅用于查看模式）          */
/* 2.prvUnlockQueue - 解锁队列，处理在锁定期间发生的发送和接收操作                    */
/*-----------------------------------------------------------------------------------*/

/* 队列数据读取函数（查看模式）----------------------------------------------------*/
static void prvCopyDataFromQueue( 
    Queue_t * const pxQueue,        /* 目标队列控制块 */
    void * const pvBuffer )         /* 接收数据缓冲区 */
{
    if( pxQueue->uxItemSize != 0 )  // 如果队列包含实际数据项
    {
        /* 更新读指针位置（指向下一个数据项）*/
        pxQueue->u.pcReadFrom += pxQueue->uxItemSize;
        /* 环形缓冲区处理（读指针达到尾部时回绕到头部）*/
        if( pxQueue->u.pcReadFrom >= pxQueue->pcTail ) /*lint !e946 MISRA exception justified as use of the relational operator is the cleanest solutions. */
        {
            pxQueue->u.pcReadFrom = pxQueue->pcHead;  // 回绕到队列头部
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未回绕分支）
        }
        /* 执行数据拷贝（从读指针位置拷贝数据到缓冲区）*/
        ( void ) memcpy( 
            ( void * ) pvBuffer, 
            ( void * ) pxQueue->u.pcReadFrom, 
            ( size_t ) pxQueue->uxItemSize ); /*lint !e961 !e418 MISRA exception as the casts are only redundant for some ports.  Also previous logic ensures a null pointer can only be passed to memcpy() when the count is 0. */
        /* 注意：memcpy的指针转换和大小转换，确保数据正确拷贝到缓冲区*/
    }
}

/* 队列解锁函数 --------------------------------------------------------------------*/
static void prvUnlockQueue( 
    Queue_t * const pxQueue )       /* 目标队列控制块 */
{
    /* 注意：本函数必须在调度器挂起状态下调用（确保原子操作）*/

    /* unlockQueue操作处理锁定期间的数据项变化（发送/接收）*/
    taskENTER_CRITICAL();
    {
        int8_t cTxLock = pxQueue->cTxLock;  // 获取当前发送锁状态

        /* 处理发送锁定计数器大于初始值的情况（发送操作在锁定期间发生）*/
        while( cTxLock > queueLOCKED_UNMODIFIED )
        {
            #if ( configUSE_QUEUE_SETS == 1 )  // 如果启用队列集支持
            {
                if( pxQueue->pxQueueSetContainer != NULL )  // 队列属于队列集
                {
                    /* 通知队列集，可能有高优先级任务被唤醒需要调度*/
                    if( prvNotifyQueueSetContainer( pxQueue, queueSEND_TO_BACK ) != pdFALSE )
                    {
                        /* 队列集有更高优先级任务被唤醒，标记需要上下文切换（在调度器恢复时处理）*/
                        vTaskMissedYield();
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（队列集未影响）
                    }
                }
                else  // 标准队列处理
                {
                    /* 检查是否有任务在等待接收数据*/
                    if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                    {
                        /* 从事件列表中移除等待任务，标记需要上下文切换（在调度器恢复时处理）*/
                        if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                        {
                            vTaskMissedYield();
                        }
                        else
                        {
                            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（任务未被唤醒）
                        }
                    }
                    else
                    {
                        break;  // 无任务等待，退出循环
                    }
                }
            }
            #else  // 未启用队列集分支
            {
                /* 检查是否有任务在等待接收数据*/
                if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                {
                    /* 从事件列表中移除等待任务，标记需要上下文切换（在调度器恢复时处理）*/
                    if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                    {
                        vTaskMissedYield();
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（任务未被唤醒）
                    }
                }
                else
                {
                    break;  // 无任务等待，退出循环
                }
            }
            #endif /* configUSE_QUEUE_SETS */

            --cTxLock;  // 递减发送锁定计数器
        }

        pxQueue->cTxLock = queueUNLOCKED;  // 完全解锁队列
    }
    taskEXIT_CRITICAL();

    /* 对接收锁进行相同的处理 */
    taskENTER_CRITICAL();
    {
        int8_t cRxLock = pxQueue->cRxLock;  // 获取当前接收锁状态

        /* 处理接收锁定计数器大于初始值的情况（接收操作在锁定期间发生）*/
        while( cRxLock > queueLOCKED_UNMODIFIED )
        {
            /* 检查是否有任务在等待发送数据*/
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
            {
                /* 从事件列表中移除等待任务，标记需要上下文切换（在调度器恢复时处理）*/
                if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
                {
                    vTaskMissedYield();
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（任务未被唤醒）
                }

                --cRxLock;  // 递减接收锁定计数器
            }
            else
            {
                break;  // 无任务等待，退出循环
            }
        }

        pxQueue->cRxLock = queueUNLOCKED;  // 完全解锁队列
    }
    taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

/* 检查队列是否为空 - prvIsQueueEmpty                                                  */
static BaseType_t prvIsQueueEmpty( const Queue_t *pxQueue )
{
    BaseType_t xReturn;

    /* 进入临界区（保护队列状态一致性）*/
    taskENTER_CRITICAL();
    {
        /* 判断队列中的消息数量是否为0 */
        if( pxQueue->uxMessagesWaiting == ( UBaseType_t )  0 )
        {
            xReturn = pdTRUE;  // 队列为空返回pdTRUE
        }
        else
        {
            xReturn = pdFALSE;  // 队列非空返回pdFALSE
        }
    }
    taskEXIT_CRITICAL();

    return xReturn;  // 返回队列是否为空的状态
}
/*-----------------------------------------------------------*/

/* 检查队列是否为空（ISR版本） - xQueueIsQueueEmptyFromISR                            */
BaseType_t xQueueIsQueueEmptyFromISR( const QueueHandle_t xQueue )
{
    BaseType_t xReturn;

    configASSERT( xQueue );  // 确保队列句柄有效

    /* 中断安全验证（确保中断优先级允许访问API）*/
    if( ( ( Queue_t * ) xQueue )->uxMessagesWaiting == ( UBaseType_t ) 0 )
    {
        xReturn = pdTRUE;  // 队列为空返回pdTRUE
    }
    else
    {
        xReturn = pdFALSE;  // 队列非空返回pdFALSE
    }

    return xReturn;  // 返回队列是否为空的状态（不需要进入临界区，ISR中调用）
} /*lint !e818 xQueue could not be pointer to const because it is a typedef. */
/*-----------------------------------------------------------*/

/* 检查队列是否已满 - prvIsQueueFull                                                    */
static BaseType_t prvIsQueueFull( const Queue_t *pxQueue )
{
    BaseType_t xReturn;

    /* 进入临界区（保护队列状态一致性）*/
    taskENTER_CRITICAL();
    {
        /* 判断队列中的消息数量是否达到队列长度 */
        if( pxQueue->uxMessagesWaiting == pxQueue->uxLength )
        {
            xReturn = pdTRUE;  // 队列已满返回pdTRUE
        }
        else
        {
            xReturn = pdFALSE;  // 队列未满返回pdFALSE
        }
    }
    taskEXIT_CRITICAL();

    return xReturn;  // 返回队列是否已满的状态
}
/*-----------------------------------------------------------*/

/* 检查队列是否已满（ISR版本） - xQueueIsQueueFullFromISR                                */
BaseType_t xQueueIsQueueFullFromISR( const QueueHandle_t xQueue )
{
    BaseType_t xReturn;

    configASSERT( xQueue );  // 确保队列句柄有效

    /* 中断安全验证（确保中断优先级允许访问API）*/
    if( ( ( Queue_t * ) xQueue )->uxMessagesWaiting == ( ( Queue_t * ) xQueue )->uxLength )
    {
        xReturn = pdTRUE;  // 队列已满返回pdTRUE
    }
    else
    {
        xReturn = pdFALSE;  // 队列未满返回pdFALSE
    }

    return xReturn;  // 返回队列是否已满的状态（不需要进入临界区，ISR中调用）
} /*lint !e818 xQueue could not be pointer to const because it is a typedef. */
/*-----------------------------------------------------------*/

#if ( configUSE_CO_ROUTINES == 1 )

/* 协程发送数据到队列 - xQueueCRSend                                                   */
BaseType_t xQueueCRSend( QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait )
{
    BaseType_t xReturn;
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;

    /* 如果队列已满，我们可能需要阻塞。临界区保护防止中断在检查和阻塞期间移除队列中的数据 */
    portDISABLE_INTERRUPTS();
    {
        /* 检查队列是否已满 */
        if( prvIsQueueFull( pxQueue ) != pdFALSE )
        {
            /* 队列已满 - 我们是要阻塞还是直接离开而不发送？ */
            if( xTicksToWait > ( TickType_t ) 0 )
            {
                /* 由于这是从协程调用的，我们不能直接阻塞，而是返回需要阻塞的指示 */
                vCoRoutineAddToDelayedList( xTicksToWait, &( pxQueue->xTasksWaitingToSend ) );
                portENABLE_INTERRUPTS();
                return errQUEUE_BLOCKED;  // 需要阻塞返回errQUEUE_BLOCKED
            }
            else
            {
                portENABLE_INTERRUPTS();
                return errQUEUE_FULL;  // 不阻塞返回errQUEUE_FULL
            }
        }
    }
    portENABLE_INTERRUPTS();

    portDISABLE_INTERRUPTS();
    {
        /* 检查队列是否有空位 */
        if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
        {
            /* 队列有空位，将数据写入队列 */
            prvCopyDataToQueue( pxQueue, pvItemToQueue, queueSEND_TO_BACK );
            xReturn = pdPASS;

            /* 检查是否有协程等待接收数据 */
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
            {
                /* 在此情况下，可以立即将协程放置在就绪列表中，因为我们在临界区中。
                   但是为了与中断中的处理方式一致，使用相同的挂起就绪列表机制 */
                if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                {
                    /* 等待的协程优先级高于当前协程，记录可能需要调度 */
                    xReturn = errQUEUE_YIELD;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未唤醒协程）
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（无协程等待）
            }
        }
        else
        {
            xReturn = errQUEUE_FULL;  // 队列已满返回errQUEUE_FULL
        }
    }
    portENABLE_INTERRUPTS();

    return xReturn;  // 返回操作结果
}

#endif /* configUSE_CO_ROUTINES */
/*-----------------------------------------------------------*/

/* 检查队列是否为空并从中接收数据（协程版本） - xQueueCRReceive */
#if ( configUSE_CO_ROUTINES == 1 )

BaseType_t xQueueCRReceive( QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait )
{
    BaseType_t xReturn;
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;

    /* 如果队列已空，我们可能需要阻塞。临界区保护防止中断在检查和阻塞期间添加数据到队列 */
    portDISABLE_INTERRUPTS();
    {
        if( pxQueue->uxMessagesWaiting == ( UBaseType_t ) 0 )
        {
            /* 队列为空，我们是要阻塞还是直接离开而不接收？ */
            if( xTicksToWait > ( TickType_t ) 0 )
            {
                /* 由于这是从协程调用的，我们不能直接阻塞，而是返回需要阻塞的指示 */
                vCoRoutineAddToDelayedList( xTicksToWait, &( pxQueue->xTasksWaitingToReceive ) );
                portENABLE_INTERRUPTS();
                return errQUEUE_BLOCKED;  // 需要阻塞返回errQUEUE_BLOCKED
            }
            else
            {
                portENABLE_INTERRUPTS();
                return errQUEUE_FULL;  // 不阻塞返回errQUEUE_FULL（此处为错误注释，应为errQUEUE_EMPTY）
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（队列非空）
        }
    }
    portENABLE_INTERRUPTS();

    portDISABLE_INTERRUPTS();
    {
        if( pxQueue->uxMessagesWaiting > ( UBaseType_t ) 0 )
        {
            /* 数据从队列可用 */
            pxQueue->u.pcReadFrom += pxQueue->uxItemSize;
            if( pxQueue->u.pcReadFrom >= pxQueue->pcTail )
            {
                pxQueue->u.pcReadFrom = pxQueue->pcHead;  // 回绕到队列头部
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未回绕）
            }
            --( pxQueue->uxMessagesWaiting );  // 减少消息计数器
            ( void ) memcpy( 
                ( void * ) pvBuffer, 
                ( void * ) pxQueue->u.pcReadFrom, 
                ( unsigned ) pxQueue->uxItemSize );  // 执行数据拷贝

            xReturn = pdPASS;

            /* 检查是否有协程等待发送数据 */
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
            {
                /* 在此情况下，可以立即将协程放置在就绪列表中，因为我们在临界区中。
                   但是为了与中断中的处理方式一致，使用相同的挂起就绪列表机制 */
                if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
                {
                    xReturn = errQUEUE_YIELD;  // 需要上下文切换
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未唤醒协程）
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（无协程等待）
            }
        }
        else
        {
            xReturn = pdFAIL;  // 队列空返回pdFAIL
        }
    }
    portENABLE_INTERRUPTS();

    return xReturn;  // 返回操作结果
}

#endif /* configUSE_CO_ROUTINES */
/*-----------------------------------------------------------*/

/* 从ISR中发送数据到队列（协程版本） - xQueueCRSendFromISR */
#if ( configUSE_CO_ROUTINES == 1 )

BaseType_t xQueueCRSendFromISR( QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t xCoRoutinePreviouslyWoken )
{
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;

    /* 由于ISR中不能阻塞，如果队列已满，则直接退出 */
    if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
    {
        /* 执行数据拷贝（将数据写入队列队尾）*/
        prvCopyDataToQueue( pxQueue, pvItemToQueue, queueSEND_TO_BACK );

        /* 我们只想在每个ISR中唤醒一个协程，因此检查是否已经唤醒了协程 */
        if( xCoRoutinePreviouslyWoken == pdFALSE )
        {
            /* 检查是否有协程等待接收数据 */
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
            {
                /* 从事件列表中移除等待协程 */
                if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                {
                    return pdTRUE;  // 返回pdTRUE表示需要上下文切换
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未唤醒协程）
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（无协程等待）
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（已唤醒协程）
        }
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（队列已满）
    }

    return xCoRoutinePreviouslyWoken;  // 返回操作结果
}

#endif /* configUSE_CO_ROUTINES */

/*-----------------------------------------------------------*/

/* 从中断服务例程接收数据（协程版本） - xQueueCRReceiveFromISR */
#if ( configUSE_CO_ROUTINES == 1 )

BaseType_t xQueueCRReceiveFromISR( QueueHandle_t xQueue, void *pvBuffer, BaseType_t *pxCoRoutineWoken )
{
    BaseType_t xReturn;
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;

    /* 由于ISR中不能阻塞，检查队列是否有数据 */
    if( pxQueue->uxMessagesWaiting > ( UBaseType_t ) 0 )
    {
        /* 执行数据拷贝（从读指针位置拷贝数据到缓冲区） */
        pxQueue->u.pcReadFrom += pxQueue->uxItemSize;
        if( pxQueue->u.pcReadFrom >= pxQueue->pcTail )
        {
            pxQueue->u.pcReadFrom = pxQueue->pcHead;  // 回绕到队列头部
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未回绕）
        }
        --( pxQueue->uxMessagesWaiting );  // 减少消息计数器
        ( void ) memcpy( 
            ( void * ) pvBuffer, 
            ( void * ) pxQueue->u.pcReadFrom, 
            ( unsigned ) pxQueue->uxItemSize );  // 执行数据拷贝

        /* 检查是否已唤醒协程 */
        if( ( *pxCoRoutineWoken ) == pdFALSE )
        {
            /* 检查是否有协程等待发送数据 */
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
            {
                /* 从事件列表中移除等待协程 */
                if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
                {
                    *pxCoRoutineWoken = pdTRUE;  // 标记需要上下文切换
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未唤醒协程）
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（无协程等待）
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（已唤醒协程）
        }

        xReturn = pdPASS;  // 返回接收成功
    }
    else
    {
        xReturn = pdFAIL;  // 队列空返回pdFAIL
    }

    return xReturn;  // 返回操作结果
}

#endif /* configUSE_CO_ROUTINES */
/*-----------------------------------------------------------*/

/* 将队列添加到注册表中（任务级版本） - vQueueAddToRegistry */
#if ( configQUEUE_REGISTRY_SIZE > 0 )

void vQueueAddToRegistry( QueueHandle_t xQueue, const char *pcQueueName ) /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
{
    UBaseType_t ux;

    /* 检查注册表中是否有空闲槽位 */
    for( ux = 0U; ux < configQUEUE_REGISTRY_SIZE; ux++ )
    {
        if( xQueueRegistry[ ux ].pcQueueName == NULL )  // 找到空闲槽位
        {
            /* 存储队列信息（名称和句柄） */
            xQueueRegistry[ ux ].pcQueueName = pcQueueName;
            xQueueRegistry[ ux ].xHandle = xQueue;

            traceQUEUE_REGISTRY_ADD( xQueue, pcQueueName );  // 追踪添加事件
            break;
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未找到空闲槽位）
        }
    }
}

#endif /* configQUEUE_REGISTRY_SIZE */
/*-----------------------------------------------------------*/

/* 从注册表中获取队列名称（任务级版本） - pcQueueGetName */
#if ( configQUEUE_REGISTRY_SIZE > 0 )

const char *pcQueueGetName( QueueHandle_t xQueue ) /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
{
    UBaseType_t ux;
    const char *pcReturn = NULL; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

    /* 遍历注册表查找目标队列 */
    for( ux = 0U; ux < configQUEUE_REGISTRY_SIZE; ux++ )
    {
        if( xQueueRegistry[ ux ].xHandle == xQueue )  // 找到目标队列
        {
            pcReturn = xQueueRegistry[ ux ].pcQueueName;  // 获取队列名称
            break;
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未找到目标队列）
        }
    }

    return pcReturn;  // 返回队列名称
}

#endif /* configQUEUE_REGISTRY_SIZE */

/*-----------------------------------------------------------*/

/* 从注册表中注销队列（任务级版本） - vQueueUnregisterQueue */
#if ( configQUEUE_REGISTRY_SIZE > 0 )

void vQueueUnregisterQueue( QueueHandle_t xQueue )
{
    UBaseType_t ux;

    /* 检查注册表中是否存在要注销的队列 */
    for( ux = 0U; ux < configQUEUE_REGISTRY_SIZE; ux++ )
    {
        if( xQueueRegistry[ ux ].xHandle == xQueue )  // 找到目标队列
        {
            /* 将名称设置为NULL表示该槽位为空闲 */
            xQueueRegistry[ ux ].pcQueueName = NULL;

            /* 将句柄设置为NULL，防止重复注册 */
            xQueueRegistry[ ux ].xHandle = ( QueueHandle_t ) 0;
            break;
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未找到目标队列）
        }
    }

} /*lint !e818 xQueue could not be pointer to const because it is a typedef. */

#endif /* configQUEUE_REGISTRY_SIZE */
/*-----------------------------------------------------------*/

/* 使任务在队列为空时等待（定时器支持版本） - vQueueWaitForMessageRestricted */
#if ( configUSE_TIMERS == 1 )

void vQueueWaitForMessageRestricted( QueueHandle_t xQueue, TickType_t xTicksToWait, const BaseType_t xWaitIndefinitely )
{
    Queue_t * const pxQueue = ( Queue_t * ) xQueue;

    /* 本函数不应由应用程序代码调用，因此名称中包含'Restricted'。
       它不是公共API的一部分，而是用于内核代码，具有特殊的调用要求。
       它可以在调度器锁定时调用，但不应在临界区中调用。
       这可能会导致vListInsert()在一个最多只有一个项目的列表中被调用，因此该列表将是快速的。
       但是即使如此，也不应在临界区中调用，因为当队列解锁时会立即执行上下文切换。 */

    /* 如果队列为空，将任务放置在阻塞列表中 */
    prvLockQueue( pxQueue );
    if( pxQueue->uxMessagesWaiting == 0U )
    {
        /* 队列为空，阻塞指定时间 */
        vTaskPlaceOnEventListRestricted( &( pxQueue->xTasksWaitingToReceive ), xTicksToWait, xWaitIndefinitely );
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（队列非空）
    }
    prvUnlockQueue( pxQueue );
}

#endif /* configUSE_TIMERS */
/*-----------------------------------------------------------*/

/* 创建队列集（动态分配版本） - xQueueCreateSet */
#if( ( configUSE_QUEUE_SETS == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )

QueueSetHandle_t xQueueCreateSet( const UBaseType_t uxEventQueueLength )
{
    QueueSetHandle_t pxQueue;

    /* 创建固定长度的队列，每个项目保存一个Queue_t *指针 */
    pxQueue = xQueueGenericCreate( uxEventQueueLength, sizeof( Queue_t * ), queueQUEUE_TYPE_SET );

    return pxQueue;  // 返回队列集句柄
}

#endif /* configUSE_QUEUE_SETS */

/*-----------------------------------------------------------*/

/* 将队列或信号量添加到队列集 - xQueueAddToSet */
#if ( configUSE_QUEUE_SETS == 1 )

BaseType_t xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet )
{
    BaseType_t xReturn;

    /* 进入临界区（保护队列操作的一致性） */
    taskENTER_CRITICAL();
    {
        /* 检查队列或信号量是否已经属于某个队列集 */
        if( ( ( Queue_t * ) xQueueOrSemaphore )->pxQueueSetContainer != NULL )
        {
            /* 队列或信号量已经属于一个队列集，无法添加到另一个队列集 */
            xReturn = pdFAIL;
        }
        else if( ( ( Queue_t * ) xQueueOrSemaphore )->uxMessagesWaiting != 0 )
        {
            /* 队列或信号量中有未处理的数据，无法添加到队列集 */
            xReturn = pdFAIL;
        }
        else
        {
            /* 将队列或信号量添加到队列集 */
            ( ( Queue_t * ) xQueueOrSemaphore )->pxQueueSetContainer = xQueueSet;
            xReturn = pdPASS;
        }
    }
    taskEXIT_CRITICAL();

    return xReturn;  // 返回操作结果
}

#endif /* configUSE_QUEUE_SETS */
/*-----------------------------------------------------------*/

/* 从队列集中移除队列或信号量 - xQueueRemoveFromSet */
#if ( configUSE_QUEUE_SETS == 1 )

BaseType_t xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet )
{
    BaseType_t xReturn;
    Queue_t * const pxQueueOrSemaphore = ( Queue_t * ) xQueueOrSemaphore;

    /* 检查队列或信号量是否属于指定的队列集 */
    if( pxQueueOrSemaphore->pxQueueSetContainer != xQueueSet )
    {
        /* 队列或信号量不属于该队列集 */
        xReturn = pdFAIL;
    }
    else if( pxQueueOrSemaphore->uxMessagesWaiting != 0 )
    {
        /* 队列或信号量中有未处理的数据，无法移除 */
        xReturn = pdFAIL;
    }
    else
    {
        /* 进入临界区（保护队列操作的一致性） */
        taskENTER_CRITICAL();
        {
            /* 将队列或信号量从队列集中移除 */
            pxQueueOrSemaphore->pxQueueSetContainer = NULL;
        }
        taskEXIT_CRITICAL();
        xReturn = pdPASS;
    }

    return xReturn;  // 返回操作结果
} /*lint !e818 xQueueSet could not be declared as pointing to const because it is a typedef. */

#endif /* configUSE_QUEUE_SETS */
/*-----------------------------------------------------------*/

/* 从队列集中选择队列或信号量（阻塞版本） - xQueueSelectFromSet */
#if ( configUSE_QUEUE_SETS == 1 )

QueueSetMemberHandle_t xQueueSelectFromSet( QueueHandle_t xQueueSet, TickType_t const xTicksToWait )
{
    QueueSetMemberHandle_t xReturn = NULL;

    /* 从队列集中接收数据，返回队列或信号量句柄 */
    ( void ) xQueueGenericReceive( ( QueueHandle_t ) xQueueSet, &xReturn, xTicksToWait, pdFALSE ); /*lint !e961 Casting from one typedef to another is not redundant. */
    return xReturn;  // 返回选择的队列或信号量句柄
}

#endif /* configUSE_QUEUE_SETS */
/*-----------------------------------------------------------*/

/* 从队列集中选择队列或信号量（ISR版本） - xQueueSelectFromSetFromISR */
#if ( configUSE_QUEUE_SETS == 1 )

QueueSetMemberHandle_t xQueueSelectFromSetFromISR( QueueHandle_t xQueueSet )
{
    QueueSetMemberHandle_t xReturn = NULL;

    /* 从队列集中接收数据（ISR版本），返回队列或信号量句柄 */
    ( void ) xQueueReceiveFromISR( ( QueueHandle_t ) xQueueSet, &xReturn, NULL ); /*lint !e961 Casting from one typedef to another is not redundant. */
    return xReturn;  // 返回选择的队列或信号量句柄
}

#endif /* configUSE_QUEUE_SETS */

/*-----------------------------------------------------------*/

/* 通知队列集容器 - prvNotifyQueueSetContainer */
#if ( configUSE_QUEUE_SETS == 1 )

static BaseType_t prvNotifyQueueSetContainer( const Queue_t * const pxQueue, const BaseType_t xCopyPosition )
{
    Queue_t *pxQueueSetContainer = pxQueue->pxQueueSetContainer;  // 获取队列集容器指针
    BaseType_t xReturn = pdFALSE;                                // 初始化返回值

    /* 注意：本函数必须在临界区中调用（确保原子操作） */
    configASSERT( pxQueueSetContainer );  // 确保队列集容器有效
    configASSERT( pxQueueSetContainer->uxMessagesWaiting < pxQueueSetContainer->uxLength );  // 确保队列集未满

    if( pxQueueSetContainer->uxMessagesWaiting < pxQueueSetContainer->uxLength )
    {
        const int8_t cTxLock = pxQueueSetContainer->cTxLock;  // 获取当前发送锁状态

        traceQUEUE_SEND( pxQueueSetContainer );  // 追踪发送事件

        /* 将队列句柄作为数据写入队列集容器 */
        xReturn = prvCopyDataToQueue( pxQueueSetContainer, &pxQueue, xCopyPosition );

        if( cTxLock == queueUNLOCKED )
        {
            /* 检查是否有协程在等待接收数据 */
            if( listLIST_IS_EMPTY( &( pxQueueSetContainer->xTasksWaitingToReceive ) ) == pdFALSE )
            {
                /* 从事件列表中移除等待协程 */
                if( xTaskRemoveFromEventList( &( pxQueueSetContainer->xTasksWaitingToReceive ) ) != pdFALSE )
                {
                    xReturn = pdTRUE;  // 标记需要上下文切换
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（未唤醒协程）
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（无协程等待）
            }
        }
        else
        {
            /* 队列集容器处于发送锁定状态，增加锁定计数器 */
            pxQueueSetContainer->cTxLock = ( int8_t ) ( cTxLock + 1 );
        }
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();  // 测试覆盖率标记（队列集已满）
    }

    return xReturn;  // 返回操作结果
}

#endif /* configUSE_QUEUE_SETS */















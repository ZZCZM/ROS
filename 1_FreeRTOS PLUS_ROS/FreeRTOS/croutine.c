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

#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
/* 移除此文件如果未启用协程功能 */
#if( configUSE_CO_ROUTINES != 0 )  // 协程功能配置开关

/*
 * 针对需要全局可见数据的调试器兼容性处理
 * 通过移除static修饰符使静态变量全局可见
 */
#ifdef portREMOVE_STATIC_QUALIFIER
    #define static  // 替换static为空宏定义
#endif

/*------------------------ 协程调度数据结构 ------------------------*/
static List_t pxReadyCoRoutineLists[ configMAX_CO_ROUTINE_PRIORITIES ];  /* 优先级就绪队列数组：
                                                                            索引表示优先级(0~max)
                                                                            元素存储对应优先级的就绪协程 */

static List_t xDelayedCoRoutineList1;        /* 延迟协程列表1：存储当前周期未超时的延迟协程 */
static List_t xDelayedCoRoutineList2;        /* 延迟协程列表2：存储溢出周期的延迟协程 */
static List_t * pxDelayedCoRoutineList = &xDelayedCoRoutineList1;          /* 当前使用的延迟列表指针 */
static List_t * pxOverflowDelayedCoRoutineList = &xDelayedCoRoutineList2; /* 溢出延迟列表指针（用于时间溢出处理）*/

static List_t xPendingReadyCoRoutineList;    /* 中断上下文中暂存的外部事件就绪协程列表：
                                                用于不能直接访问就绪队列的中断处理场景 */

/*----------------------- 协程调度核心变量 -------------------------*/
CRCB_t * pxCurrentCoRoutine = NULL;          /* 当前正在运行的协程控制块指针 */

static UBaseType_t uxTopCoRoutineReadyPriority = 0;  /* 当前最高就绪优先级：
                                                        用于快速查找最高优先级协程 */

static TickType_t xCoRoutineTickCount = 0;   /* 协程调度器时钟计数器 */
static TickType_t xLastTickCount = 0;         /* 上次调度时的系统时钟 */
static TickType_t xPassedTicks = 0;           /* 两次调度间经过的时钟节拍数 */

/* 协程初始状态标识 */
#define corINITIAL_STATE ( 0 )  // 用于协程状态机初始化

/*--------------------- 核心调度操作宏 --------------------------*/
/*
 * 功能：将协程控制块插入就绪队列
 * 特点：
 *   - 按优先级插入队列末端
 *   - 维护最高优先级跟踪
 *   - 禁止在中断中使用（直接操作调度器数据结构）
 */
#define prvAddCoRoutineToReadyQueue( pxCRCB ) \
{ \
    /* 更新最高优先级跟踪 */ \
    if( pxCRCB->uxPriority > uxTopCoRoutineReadyPriority ) \
    { \
        uxTopCoRoutineReadyPriority = pxCRCB->uxPriority; /* 优先级倒置保护 */ \
    } \
    /* 插入对应优先级的就绪列表末端 */ \
    vListInsertEnd( ( List_t * ) &( pxReadyCoRoutineLists[ pxCRCB->uxPriority ] ), \
                    &( pxCRCB->xGenericListItem ) ); \
}




/*
 * Utility to ready all the lists used by the scheduler.  This is called
 * automatically upon the creation of the first co-routine.
 */
static void prvInitialiseCoRoutineLists( void );

/*
 * Co-routines that are readied by an interrupt cannot be placed directly into
 * the ready lists (there is no mutual exclusion).  Instead they are placed in
 * in the pending ready list in order that they can later be moved to the ready
 * list by the co-routine scheduler.
 */
static void prvCheckPendingReadyList( void );

/*
 * Macro that looks at the list of co-routines that are currently delayed to
 * see if any require waking.
 *
 * Co-routines are stored in the queue in the order of their wake time -
 * meaning once one co-routine has been found whose timer has not expired
 * we need not look any further down the list.
 */
static void prvCheckDelayedList( void );

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 函数功能：创建新协程并初始化到就绪队列                     */
/* 参数：                                                    */
/*   pxCoRoutineCode - 协程入口函数指针                      */
/*   uxPriority      - 协程优先级(0~configMAX_CO_ROUTINE_PRIORITIES-1) */
/*   uxIndex         - 协程标识索引                          */
/* 返回值：                                                  */
/*   pdPASS(成功)/errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY(失败) */
/*-----------------------------------------------------------*/
BaseType_t xCoRoutineCreate( crCOROUTINE_CODE pxCoRoutineCode, 
                            UBaseType_t uxPriority, 
                            UBaseType_t uxIndex )
{
BaseType_t xReturn;
CRCB_t *pxCoRoutine;

    /* 分配协程控制块内存（CRCB包含协程状态信息） */
    pxCoRoutine = ( CRCB_t * ) pvPortMalloc( sizeof( CRCB_t ) );
    
    if( pxCoRoutine ) // 内存分配成功
    {
        /* 首次协程创建时的系统初始化 */
        if( pxCurrentCoRoutine == NULL ) // 检查是否是第一个协程
        {
            pxCurrentCoRoutine = pxCoRoutine;  // 设置当前运行协程
            prvInitialiseCoRoutineLists();    // 初始化调度器队列结构
        }

        /* 优先级合法性检查（防越界保护） */
        if( uxPriority >= configMAX_CO_ROUTINE_PRIORITIES )
        {
            uxPriority = configMAX_CO_ROUTINE_PRIORITIES - 1; // 降级到最高可用优先级
        }

        /* 初始化协程控制块参数 */
        pxCoRoutine->uxState = corINITIAL_STATE;    // 初始状态（状态机起点）
        pxCoRoutine->uxPriority = uxPriority;      // 设置优先级
        pxCoRoutine->uxIndex = uxIndex;            // 标识索引（用于区分同优先级协程）
        pxCoRoutine->pxCoRoutineFunction = pxCoRoutineCode; // 关联入口函数

        /* 初始化链表项（用于就绪队列和事件队列） */
        vListInitialiseItem( &( pxCoRoutine->xGenericListItem ) );  // 通用列表项（用于调度）
        vListInitialiseItem( &( pxCoRoutine->xEventListItem ) );    // 事件列表项（用于事件等待）

        /* 建立链表项到协程控制块的反向指针 */
        listSET_LIST_ITEM_OWNER( &( pxCoRoutine->xGenericListItem ), pxCoRoutine ); // 通用项所有者
        listSET_LIST_ITEM_OWNER( &( pxCoRoutine->xEventListItem ), pxCoRoutine );   // 事件项所有者

        /* 配置事件列表项的排序值（值越大优先级越高） 
           公式：configMAX_CO_ROUTINE_PRIORITIES - uxPriority
           例如：最大优先级5，当前优先级2 → 排序值3 */
        listSET_LIST_ITEM_VALUE( &( pxCoRoutine->xEventListItem ), 
                               ( ( TickType_t ) configMAX_CO_ROUTINE_PRIORITIES - 
                               ( TickType_t ) uxPriority ) );

        /* 将新协程加入就绪队列 */
        prvAddCoRoutineToReadyQueue( pxCoRoutine ); // 宏操作：更新优先级队列

        xReturn = pdPASS; // 返回成功标志
    }
    else // 内存分配失败
    {
        xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY; // 返回内存不足错误
    }

    return xReturn;
}

/*-----------------------------------------------------------*/
/* 函数功能：将当前协程加入延迟队列                           */
/* 参数：                                                    */
/*   xTicksToDelay - 需要延迟的时钟节拍数                    */
/*   pxEventList   - 可选的事件等待列表（如信号量队列）        */
/* 注意：操作事件列表时必须关闭中断                           */
/*-----------------------------------------------------------*/
void vCoRoutineAddToDelayedList( TickType_t xTicksToDelay, 
                               List_t *pxEventList )
{
TickType_t xTimeToWake;

    /* 计算绝对唤醒时间（允许溢出）*/
    xTimeToWake = xCoRoutineTickCount + xTicksToDelay; // 可能超过TickType_t最大值

    /* 从就绪队列移除（同一个列表项不能存在多个列表） */
    ( void ) uxListRemove( ( ListItem_t * ) &( pxCurrentCoRoutine->xGenericListItem ) );

    /* 设置通用列表项的排序值为唤醒时间 */
    listSET_LIST_ITEM_VALUE( &( pxCurrentCoRoutine->xGenericListItem ), xTimeToWake );

    /* 处理时间溢出情况 */
    if( xTimeToWake < xCoRoutineTickCount ) // 发生时间回绕
    {
        /* 加入溢出延迟列表（处理大周期延迟） */
        vListInsert( ( List_t * ) pxOverflowDelayedCoRoutineList, 
                   ( ListItem_t * ) &( pxCurrentCoRoutine->xGenericListItem ) );
    }
    else // 正常延迟情况
    {
        /* 加入常规延迟列表 */
        vListInsert( ( List_t * ) pxDelayedCoRoutineList, 
                   ( ListItem_t * ) &( pxCurrentCoRoutine->xGenericListItem ) );
    }

    /* 可选：加入事件等待列表（如信号量队列） */
    if( pxEventList ) 
    {
        /* 将事件列表项插入指定事件队列 */
        vListInsert( pxEventList,                   // 目标事件列表（如信号量等待队列）
                   &( pxCurrentCoRoutine->xEventListItem ) ); // 事件列表项
    }
}
/*-----------------------------------------------------------*/
/* 函数功能：处理中断环境中就绪的待转移协程                    */
/* 机制：                                                    */
/* - 中断服务程序(ISR)不能直接操作就绪队列                    */
/* - 通过中转队列xPendingReadyCoRoutineList进行缓冲         */
/*-----------------------------------------------------------*/
static void prvCheckPendingReadyList( void )
{
    /* 循环处理所有待转移协程 */
    while( listLIST_IS_EMPTY( &xPendingReadyCoRoutineList ) == pdFALSE )
    {
        CRCB_t *pxUnblockedCRCB;  // 当前处理的协程控制块指针

        /* 临界段操作（防止ISR修改列表） */
        portDISABLE_INTERRUPTS();
        {
            /* 获取待处理队列首项所属协程 */
            pxUnblockedCRCB = ( CRCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( (&xPendingReadyCoRoutineList) );
            /* 将协程从待处理队列移除 */
            ( void ) uxListRemove( &( pxUnblockedCRCB->xEventListItem ) );
        }
        portENABLE_INTERRUPTS();

        /* 确保协程不在旧队列中 */
        ( void ) uxListRemove( &( pxUnblockedCRCB->xGenericListItem ) );
        /* 加入优先级就绪队列 */
        prvAddCoRoutineToReadyQueue( pxUnblockedCRCB );
    }
}

/*-----------------------------------------------------------*/
/* 函数功能：检查延迟列表中的协程超时情况                     */
/* 执行时机：每次调度器tick中断时调用                       */
/* 核心逻辑：                                                */
/* 1. 更新经过的tick数                                       */
/* 2. 处理时间溢出（32位系统约49.7天溢出一次）              */
/* 3. 遍历延迟列表唤醒到期协程                               */
/*-----------------------------------------------------------*/
static void prvCheckDelayedList( void )
{
    CRCB_t *pxCRCB;  // 当前处理的协程控制块指针

    /* 计算自上次调度以来的时间增量 */
    xPassedTicks = xTaskGetTickCount() - xLastTickCount;
    
    /* 逐tick处理（处理多tick累积的情况） */
    while( xPassedTicks )
    {
        xCoRoutineTickCount++;  // 协程调度器时钟+1
        xPassedTicks--;         // 剩余需要处理的tick数-1

        /* 时间溢出处理（0xFFFFFFFF → 0x00000000时） */
        if( xCoRoutineTickCount == 0 )
        {
            List_t * pxTemp;  // 临时指针用于交换列表

            /* 交换延迟列表角色：
               - pxDelayedCoRoutineList处理新周期延迟
               - pxOverflowDelayedCoRoutineList处理溢出的延迟 */
            pxTemp = pxDelayedCoRoutineList;
            pxDelayedCoRoutineList = pxOverflowDelayedCoRoutineList;
            pxOverflowDelayedCoRoutineList = pxTemp;
        }

        /* 遍历当前延迟列表处理到期协程 */
        while( listLIST_IS_EMPTY( pxDelayedCoRoutineList ) == pdFALSE )
        {
            /* 获取延迟队列首项所属协程 */
            pxCRCB = ( CRCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedCoRoutineList );

            /* 检查是否到期（列表按唤醒时间升序排列） */
            if( xCoRoutineTickCount < listGET_LIST_ITEM_VALUE( &( pxCRCB->xGenericListItem ) ) )
            {
                break;  // 当前协程未到期，后续协程更不会到期，结束检查
            }

            /* 临界段操作（防止事件并发修改） */
            portDISABLE_INTERRUPTS();
            {
                /* 从延迟队列移除 */
                ( void ) uxListRemove( &( pxCRCB->xGenericListItem ) );

                /* 如果协程同时在等待事件（如信号量） */
                if( pxCRCB->xEventListItem.pvContainer )
                {
                    /* 从事件队列移除（事件可能已经发生） */
                    ( void ) uxListRemove( &( pxCRCB->xEventListItem ) );
                }
            }
            portENABLE_INTERRUPTS();

            /* 将协程重新加入就绪队列 */
            prvAddCoRoutineToReadyQueue( pxCRCB );
        }
    }

    /* 更新最后处理时间点 */
    xLastTickCount = xCoRoutineTickCount;
}
/*-----------------------------------------------------------*/
/* 函数功能：协程调度器核心                                    */
/* 执行流程：                                                */
/* 1. 处理中断待转移协程                                      */
/* 2. 处理延迟到期的协程                                      */
/* 3. 选择最高优先级协程执行                                 */
/*-----------------------------------------------------------*/
void vCoRoutineSchedule( void )
{
    /* 步骤1：处理来自ISR的待就绪协程 */
    prvCheckPendingReadyList();

    /* 步骤2：检查延迟列表中的超时协程 */
    prvCheckDelayedList();

    /* 步骤3：查找最高非空优先级队列 */
    while( listLIST_IS_EMPTY( &( pxReadyCoRoutineLists[ uxTopCoRoutineReadyPriority ] ) ) )
    {
        if( uxTopCoRoutineReadyPriority == 0 )
        {
            /* 无就绪协程可执行，直接返回 */
            return; 
        }
        --uxTopCoRoutineReadyPriority;  // 降级检查更低优先级队列
    }

    /* 步骤4：从就绪队列取出下一个协程（轮转调度同优先级协程） */
    listGET_OWNER_OF_NEXT_ENTRY( pxCurrentCoRoutine, 
                               &( pxReadyCoRoutineLists[ uxTopCoRoutineReadyPriority ] ) );

    /* 步骤5：执行协程入口函数（协程运行至主动挂起） */
    ( pxCurrentCoRoutine->pxCoRoutineFunction )( pxCurrentCoRoutine, 
                                               pxCurrentCoRoutine->uxIndex );

    return;  // 协程主动挂起（crDELAY/crWAIT）后返回
}

/*-----------------------------------------------------------*/
/* 函数功能：初始化协程调度数据结构                            */
/* 注意：系统第一个协程创建时自动调用                          */
/*-----------------------------------------------------------*/
static void prvInitialiseCoRoutineLists( void )
{
    UBaseType_t uxPriority;

    /* 初始化各优先级的就绪队列（数组） */
    for( uxPriority = 0; uxPriority < configMAX_CO_ROUTINE_PRIORITIES; uxPriority++ )
    {
        vListInitialise( ( List_t * ) &( pxReadyCoRoutineLists[ uxPriority ] ) );
    }

    /* 初始化延迟队列和待处理就绪队列 */
    vListInitialise( ( List_t * ) &xDelayedCoRoutineList1 );  // 当前延迟列表
    vListInitialise( ( List_t * ) &xDelayedCoRoutineList2 );  // 溢出延迟列表
    vListInitialise( ( List_t * ) &xPendingReadyCoRoutineList ); // 中断待处理队列

    /* 初始指针分配（启动时没有溢出情况） */
    pxDelayedCoRoutineList = &xDelayedCoRoutineList1;         // 指向常规延迟列表
    pxOverflowDelayedCoRoutineList = &xDelayedCoRoutineList2; // 指向溢出延迟列表
}

/*-----------------------------------------------------------*/
/* 函数功能：从事件列表释放协程（ISR环境调用）                  */
/* 参数：pxEventList - 事件等待列表（如信号量等待队列）         */
/* 返回：pdTRUE(需要触发调度)/pdFALSE(无需调度)                */
/* 注意：必须在中断服务程序(ISR)中调用                         */
/*-----------------------------------------------------------*/
BaseType_t xCoRoutineRemoveFromEventList( const List_t *pxEventList )
{
    CRCB_t *pxUnblockedCRCB;  // 被解除阻塞的协程控制块
    BaseType_t xReturn;

    /* 从事件队列头部取出协程（队列已确保非空） */
    pxUnblockedCRCB = ( CRCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxEventList );
    
    /* 从事件列表移除 */
    ( void ) uxListRemove( &( pxUnblockedCRCB->xEventListItem ) );
    
    /* 加入待处理就绪队列（等待调度器处理） */
    vListInsertEnd( ( List_t * ) &xPendingReadyCoRoutineList, 
                  &( pxUnblockedCRCB->xEventListItem ) );

    /* 判断是否需要立即调度：
       如果被唤醒协程优先级 ≥ 当前运行协程 */
    if( pxUnblockedCRCB->uxPriority >= pxCurrentCoRoutine->uxPriority )
    {
        xReturn = pdTRUE;  // 需要触发任务切换
    }
    else
    {
        xReturn = pdFALSE; // 无需立即切换
    }

    return xReturn;
}


#endif /* configUSE_CO_ROUTINES == 0 */


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
/* 宏定义：防止MPU包装器重复定义 */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* FreeRTOS核心头文件引入 */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"

/* 撤销MPU包装器定义（符合MISRA规范） */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 事件控制位定义（根据时钟类型分16/32位） */
#if configUSE_16_BIT_TICKS == 1
    #define eventCLEAR_EVENTS_ON_EXIT_BIT  0x0100U    // 退出时清除事件位标志
    #define eventUNBLOCKED_DUE_TO_BIT_SET  0x0200U    // 因事件位触发解除阻塞标志
    #define eventWAIT_FOR_ALL_BITS        0x0400U    // 所有位匹配等待模式标志
    #define eventEVENT_BITS_CONTROL_BYTES 0xff00U     // 控制位掩码（保留高字节）
#else
    #define eventCLEAR_EVENTS_ON_EXIT_BIT  0x01000000UL  // 32位版相同含义
    #define eventUNBLOCKED_DUE_TO_BIT_SET  0x02000000UL
    #define eventWAIT_FOR_ALL_BITS        0x04000000UL
    #define eventEVENT_BITS_CONTROL_BYTES 0xff000000UL
#endif

/* 事件组结构体定义 */
typedef struct xEventGroupDefinition
{
    EventBits_t uxEventBits;         // 当前有效的事件位（用户可用位）
    List_t xTasksWaitingForBits;    // 等待事件位的任务链表

    #if( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t uxEventGroupNumber; // 追踪标识号（用于可视化调试）
    #endif

    #if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
        uint8_t ucStaticallyAllocated; // 静态分配标识（防止错误释放内存）
    #endif
} EventGroup_t;

/*-----------------------------------------------------------*/
/* 事件等待条件检测函数（内部使用） */
static BaseType_t prvTestWaitCondition( 
    const EventBits_t uxCurrentEventBits,   // 当前事件位
    const EventBits_t uxBitsToWaitFor,      // 需要等待的事件位
    const BaseType_t xWaitForAllBits        // 等待模式（pdTRUE=全部位）
) PRIVILEGED_FUNCTION;
/*-----------------------------------------------------------*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )  // 启用静态分配时编译

/* 函数功能：创建静态分配的事件组对象 */
EventGroupHandle_t xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer )
{
    EventGroup_t *pxEventBits;  // 事件组结构指针
    
    /* 参数检查：必须提供静态分配的内存块 */
    configASSERT( pxEventGroupBuffer );  // 触发断言失败如果参数为NULL

    /* 将用户提供的静态内存转换为事件组结构 */
    pxEventBits = ( EventGroup_t * ) pxEventGroupBuffer; 
    /* lint提示抑制：保证StaticEventGroup_t与EventGroup_t内存布局一致 */
    /*lint !e740 */

    if( pxEventBits != NULL )
    {
        /* 初始化事件位为0（无任何事件触发） */
        pxEventBits->uxEventBits = 0;  
        
        /* 初始化事件等待任务列表 */
        vListInitialise( &( pxEventBits->xTasksWaitingForBits ) );

        /* 当同时启用动态分配时，设置静态分配标识 */
        #if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
        {
            /* 标识该事件组为静态分配，防止误调用vEventGroupDelete */
            pxEventBits->ucStaticallyAllocated = pdTRUE;  
        }
        #endif /* configSUPPORT_DYNAMIC_ALLOCATION */

        /* 追踪事件：记录事件组创建成功 */
        traceEVENT_GROUP_CREATE( pxEventBits );
    }
    else
    {
        /* 追踪事件：记录内存转换失败（实际不会执行，因前面有断言） */
        traceEVENT_GROUP_CREATE_FAILED();
    }

    /* 返回事件组句柄（实质是指针转换） */
    return ( EventGroupHandle_t ) pxEventBits;
}

#endif /* configSUPPORT_STATIC_ALLOCATION */

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )  // 启用动态分配时编译

/* 函数功能：动态创建事件组对象 */
EventGroupHandle_t xEventGroupCreate( void )
{
    EventGroup_t *pxEventBits;  // 事件组结构指针

    /* 动态分配事件组内存 */
    pxEventBits = ( EventGroup_t * ) pvPortMalloc( sizeof( EventGroup_t ) );

    if( pxEventBits != NULL )  // 内存分配成功
    {
        /* 初始化事件位（所有位清零） */
        pxEventBits->uxEventBits = 0;
        
        /* 初始化事件等待任务链表 */
        vListInitialise( &( pxEventBits->xTasksWaitingForBits ) );

        /* 当同时支持静态分配时的标识处理 */
        #if( configSUPPORT_STATIC_ALLOCATION == 1 )
        {
            /* 标记为动态分配（防止与静态分配混淆） */
            pxEventBits->ucStaticallyAllocated = pdFALSE;  
        }
        #endif /* configSUPPORT_STATIC_ALLOCATION */

        /* 记录追踪事件（可视化调试） */
        traceEVENT_GROUP_CREATE( pxEventBits );
    }
    else  // 内存分配失败
    {
        /* 记录创建失败的追踪信息 */
        traceEVENT_GROUP_CREATE_FAILED();
    }

    /* 返回事件组操作句柄（类型转换） */
    return ( EventGroupHandle_t ) pxEventBits;
}

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */


/*-----------------------------------------------------------*/
/* 函数功能：同步设置事件位并等待指定条件                      */
/* 参数：                                                    */
/*   xEventGroup   - 事件组句柄                             */
/*   uxBitsToSet   - 需要设置的事件位                         */
/*   uxBitsToWaitFor - 需要等待的事件位组合                   */
/*   xTicksToWait  - 超时时间（单位：系统节拍）               */
/* 返回：                                                    */
/*   成功时返回满足条件的事件位，超时返回当前事件位            */
/* 注意：该函数自动清除uxBitsToWaitFor对应的事件位            */
/*-----------------------------------------------------------*/
EventBits_t xEventGroupSync( EventGroupHandle_t xEventGroup, 
                           const EventBits_t uxBitsToSet,
                           const EventBits_t uxBitsToWaitFor, 
                           TickType_t xTicksToWait )
{
    EventBits_t uxOriginalBitValue, uxReturn;
    EventGroup_t *pxEventBits = ( EventGroup_t * ) xEventGroup; // 转换为事件组结构体
    BaseType_t xAlreadyYielded;
    BaseType_t xTimeoutOccurred = pdFALSE;

    /* 参数检查：确保等待的是用户位（非控制位）且不为0 */
    configASSERT( ( uxBitsToWaitFor & eventEVENT_BITS_CONTROL_BYTES ) == 0 );
    configASSERT( uxBitsToWaitFor != 0 );

    /* 检查调度器状态：挂起时不允许阻塞 */
    #if ( INCLUDE_xTaskGetSchedulerState == 1 || configUSE_TIMERS == 1 )
        configASSERT( !( ( xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED ) && ( xTicksToWait != 0 ) ) );
    #endif

    vTaskSuspendAll(); // 进入临界区（挂起任务调度）
    {
        /* 获取当前事件位并设置新位 */
        uxOriginalBitValue = pxEventBits->uxEventBits;
        ( void ) xEventGroupSetBits( xEventGroup, uxBitsToSet );

        /* 检查是否立即满足同步条件 */
        if( ( ( uxOriginalBitValue | uxBitsToSet ) & uxBitsToWaitFor ) == uxBitsToWaitFor )
        {
            /* 条件已满足：返回当前事件位并清除等待位 */
            uxReturn = ( uxOriginalBitValue | uxBitsToSet );
            pxEventBits->uxEventBits &= ~uxBitsToWaitFor; // 同步后自动清除事件位
            xTicksToWait = 0; // 不需要阻塞
        }
        else
        {
            if( xTicksToWait != 0 )
            {
                traceEVENT_GROUP_SYNC_BLOCK( xEventGroup, uxBitsToSet, uxBitsToWaitFor );

                /* 将任务加入等待队列（带特殊控制位） */
                vTaskPlaceOnUnorderedEventList(
                    &( pxEventBits->xTasksWaitingForBits ),
                    ( uxBitsToWaitFor | eventCLEAR_EVENTS_ON_EXIT_BIT | eventWAIT_FOR_ALL_BITS ),
                    xTicksToWait
                );
                uxReturn = 0; // 防止编译器警告
            }
            else
            {
                /* 超时时间为0：直接返回当前事件位 */
                uxReturn = pxEventBits->uxEventBits;
            }
        }
    }
    xAlreadyYielded = xTaskResumeAll(); // 退出临界区并决定是否触发上下文切换

    /* 处理阻塞后的状态 */
    if( xTicksToWait != 0 )
    {
        /* 如果调度器未自动切换，手动触发 */
        if( xAlreadyYielded == pdFALSE )
        {
            portYIELD_WITHIN_API();
        }

        /* 获取任务解除阻塞的原因 */
        uxReturn = uxTaskResetEventItemValue(); // 返回触发的事件位（含控制标志）

        /* 判断是否超时 */
        if( ( uxReturn & eventUNBLOCKED_DUE_TO_BIT_SET ) == 0 )
        {
            /* 超时处理：检查最终事件位并决定是否清除 */
            taskENTER_CRITICAL();
            {
                uxReturn = pxEventBits->uxEventBits;
                /* 超时后仍可能满足条件（其他任务设置） */
                if( ( uxReturn & uxBitsToWaitFor ) == uxBitsToWaitFor )
                {
                    pxEventBits->uxEventBits &= ~uxBitsToWaitFor; // 清除事件位
                }
            }
            taskEXIT_CRITICAL();
            xTimeoutOccurred = pdTRUE; // 标记超时发生
        }

        /* 去除控制位，返回纯粹的事件位 */
        uxReturn &= ~eventEVENT_BITS_CONTROL_BYTES;
    }

    traceEVENT_GROUP_SYNC_END( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred );
    return uxReturn;
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 函数功能：等待事件组中的指定条件                            */
/* 参数：                                                    */
/*   xEventGroup     - 事件组句柄                           */
/*   uxBitsToWaitFor - 等待的事件位掩码                      */
/*   xClearOnExit    - pdTRUE: 退出时清除等待位              */
/*   xWaitForAllBits - pdTRUE: 需所有位都置位, pdFALSE:任一位置位 */
/*   xTicksToWait    - 最大等待时间（单位：Tick）             */
/* 返回：                                                    */
/*   满足条件的事件位（超时返回当前值）                       */
/*-----------------------------------------------------------*/
EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToWaitFor,
                                const BaseType_t xClearOnExit,
                                const BaseType_t xWaitForAllBits,
                                TickType_t xTicksToWait )
{
    EventGroup_t *pxEventBits = ( EventGroup_t * ) xEventGroup; // 类型转换
    EventBits_t uxReturn, uxControlBits = 0;                    // 返回值和控制标志
    BaseType_t xWaitConditionMet, xAlreadyYielded;             // 条件检测及调度状态
    BaseType_t xTimeoutOccurred = pdFALSE;                     // 超时标记

    /*-------- 参数有效性检查 --------*/
    configASSERT( xEventGroup ); // 确保事件组句柄有效
    configASSERT( ( uxBitsToWaitFor & eventEVENT_BITS_CONTROL_BYTES ) == 0 ); // 禁止使用内核控制位
    configASSERT( uxBitsToWaitFor != 0 ); // 至少需要等待一个事件位

    /* 防止在调度器挂起状态下进行阻塞等待 */
    #if ( INCLUDE_xTaskGetSchedulerState == 1 || configUSE_TIMERS == 1 )
        configASSERT( !( ( xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED ) && ( xTicksToWait != 0 ) ) );
    #endif

    vTaskSuspendAll(); // 进入临界区（挂起所有任务调度）
    {
        const EventBits_t uxCurrentEventBits = pxEventBits->uxEventBits; // 原子获取当前事件位

        /*-------- 步骤1：检测是否已满足等待条件 --------*/
        xWaitConditionMet = prvTestWaitCondition( 
            uxCurrentEventBits,  // 当前事件位
            uxBitsToWaitFor,     // 需要等待的位
            xWaitForAllBits      // 等待模式
        );

        if( xWaitConditionMet != pdFALSE ) // 条件立即满足
        {
            uxReturn = uxCurrentEventBits; // 返回当前事件位
            xTicksToWait = 0;             // 无需阻塞等待

            /* 根据标志清除事件位 */
            if( xClearOnExit != pdFALSE )
            {
                pxEventBits->uxEventBits &= ~uxBitsToWaitFor; // 原子清除操作
            }
        }
        else if( xTicksToWait == 0 )        // 条件不满足且不阻塞
        {
            uxReturn = uxCurrentEventBits; // 直接返回当前值
        }
        else                                // 需要进入阻塞等待
        {
            /*-------- 步骤2：配置控制标志 --------*/
            if( xClearOnExit != pdFALSE )
            {
                uxControlBits |= eventCLEAR_EVENTS_ON_EXIT_BIT; // 退出时清除事件位
            }
            if( xWaitForAllBits != pdFALSE )
            {
                uxControlBits |= eventWAIT_FOR_ALL_BITS;        // 需要所有位置位
            }

            /*-------- 步骤3：将任务加入等待列表 --------*/
            vTaskPlaceOnUnorderedEventList(
                &( pxEventBits->xTasksWaitingForBits ), // 事件组的等待队列
                ( uxBitsToWaitFor | uxControlBits ),    // 组合用户位与控制位
                xTicksToWait                            // 超时时间
            );
            uxReturn = 0; // 初始值（后续会被覆盖）
            
            traceEVENT_GROUP_WAIT_BITS_BLOCK( xEventGroup, uxBitsToWaitFor ); // 追踪调试
        }
    }
    xAlreadyYielded = xTaskResumeAll(); // 退出临界区，返回是否需要主动切换上下文

    /*-------- 步骤4：处理阻塞后的状态 --------*/
    if( xTicksToWait != 0 )
    {
        /* 触发一次上下文切换（如果需要） */
        if( xAlreadyYielded == pdFALSE )
        {
            portYIELD_WITHIN_API(); // 主动让出CPU
        }

        /* 获取解除阻塞的原因及事件位 */
        uxReturn = uxTaskResetEventItemValue(); // 获取任务事件项值（含控制位）

        if( ( uxReturn & eventUNBLOCKED_DUE_TO_BIT_SET ) == 0 ) // 因超时解除阻塞
        {
            taskENTER_CRITICAL(); // 进入临界段（禁用中断）
            {
                uxReturn = pxEventBits->uxEventBits; // 获取最新事件位

                /* 即便超时也要检查是否条件已满足 */
                if( prvTestWaitCondition( uxReturn, uxBitsToWaitFor, xWaitForAllBits ) )
                {
                    /* 满足条件则按配置清除事件位 */
                    if( xClearOnExit != pdFALSE )
                    {
                        pxEventBits->uxEventBits &= ~uxBitsToWaitFor;
                    }
                }
            }
            taskEXIT_CRITICAL();
            xTimeoutOccurred = pdTRUE; // 标记超时发生
        }

        /* 去除内核控制位，返回纯粹的事件位 */
        uxReturn &= ~eventEVENT_BITS_CONTROL_BYTES;
    }

    traceEVENT_GROUP_WAIT_BITS_END( xEventGroup, uxBitsToWaitFor, xTimeoutOccurred );
    return uxReturn;
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 函数功能：清除事件组中的指定事件位                          */
/* 参数：                                                   */
/*   xEventGroup   - 事件组句柄                             */
/*   uxBitsToClear - 需清除的事件位掩码                      */
/* 返回：清除前的原始事件位值                                 */
/*-----------------------------------------------------------*/
EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, 
                                 const EventBits_t uxBitsToClear )
{
    EventGroup_t *pxEventBits = ( EventGroup_t * ) xEventGroup;
    EventBits_t uxReturn;

    /* 参数检查：确保事件组有效，且不操作内核保留位 */
    configASSERT( xEventGroup );
    configASSERT( ( uxBitsToClear & eventEVENT_BITS_CONTROL_BYTES ) == 0 );

    taskENTER_CRITICAL(); // 进入临界区（关中断）
    {
        traceEVENT_GROUP_CLEAR_BITS( xEventGroup, uxBitsToClear );

        /* 记录清除前的原始事件位状态 */
        uxReturn = pxEventBits->uxEventBits;

        /* 执行位清除操作 */
        pxEventBits->uxEventBits &= ~uxBitsToClear;
    }
    taskEXIT_CRITICAL();  // 退出临界区（开中断）

    return uxReturn;
}

/*-----------------------------------------------------------*/
/* 条件编译：仅在启用追踪功能、定时器及延迟函数调用时包含以下代码 */
#if ( ( configUSE_TRACE_FACILITY == 1 ) && \
      ( INCLUDE_xTimerPendFunctionCall == 1 ) && \
      ( configUSE_TIMERS == 1 ) )

/* 函数功能：从中断服务程序(ISR)清除事件位                     */
BaseType_t xEventGroupClearBitsFromISR( EventGroupHandle_t xEventGroup, 
                                       const EventBits_t uxBitsToClear )
{
    BaseType_t xReturn;

    traceEVENT_GROUP_CLEAR_BITS_FROM_ISR( xEventGroup, uxBitsToClear );
    
    /* 通过定时器守护任务延迟执行实际清除操作 */
    xReturn = xTimerPendFunctionCallFromISR(
        vEventGroupClearBitsCallback,   // 回调函数
        ( void * ) xEventGroup,        // 事件组句柄作为参数
        ( uint32_t ) uxBitsToClear,    // 清除位掩码
        NULL                           // 无返回状态指针
    );

    return xReturn;
}

#endif /* 条件编译结束 */
/*-----------------------------------------------------------*/

/* 函数功能：在ISR中安全获取事件组当前事件位值                 */
EventBits_t xEventGroupGetBitsFromISR( EventGroupHandle_t xEventGroup )
{
    UBaseType_t uxSavedInterruptStatus;
    EventGroup_t *pxEventBits = ( EventGroup_t * ) xEventGroup;
    EventBits_t uxReturn;

    /* 禁用中断以保证读操作的原子性 */
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        uxReturn = pxEventBits->uxEventBits; // 读取当前事件位
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus ); // 恢复中断

    return uxReturn;
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 函数功能：设置事件组中的指定位并触发相关任务唤醒            */
/* 参数：                                                   */
/*   xEventGroup - 要操作的事件组句柄                       */
/*   uxBitsToSet - 需要设置的二进制位掩码                   */
/* 返回：                                                    */
/*   设置完成后事件组的新状态值（包含最新的事件位）            */
/* 说明：                                                    */
/*   该函数会自动处理等待列表中符合触发条件的任务              */
/*   任务根据其等待条件（任意位/全位）被唤醒                   */
/*-----------------------------------------------------------*/
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )
{
    ListItem_t *pxListItem, *pxNext;           // 用于遍历等待列表的链表指针
    ListItem_t const *pxListEnd;               // 等待列表结束标记
    List_t *pxList;                             // 等待任务列表指针
    EventBits_t uxBitsToClear = 0;             // 需要清除的事件位（累计值）
    EventBits_t uxBitsWaitedFor, uxControlBits; // 任务等待的事件位和控制位
    EventGroup_t *pxEventBits = ( EventGroup_t * ) xEventGroup; // 类型转换
    BaseType_t xMatchFound = pdFALSE;          // 标记是否找到符合触发条件的任务

    /* 参数校验：确保事件组合法且不修改内核保留位 */
    configASSERT( xEventGroup );               // 事件组句柄不能为空
    configASSERT( ( uxBitsToSet & eventEVENT_BITS_CONTROL_BYTES ) == 0 ); // 不能设置控制位

    pxList = &( pxEventBits->xTasksWaitingForBits ); // 获取等待任务链表
    pxListEnd = listGET_END_MARKER( pxList );   // 获取链表尾部标记（遍历终止条件）

    vTaskSuspendAll(); // 挂起调度器，进入临界区（原子操作）
    {
        traceEVENT_GROUP_SET_BITS( xEventGroup, uxBitsToSet ); // 调试追踪

        /* STEP 1: 设置新的事件位 */
        pxEventBits->uxEventBits |= uxBitsToSet; // 使用位或操作设置目标位

        /* STEP 2: 遍历等待任务链表处理符合条件的任务 */
        pxListItem = listGET_HEAD_ENTRY( pxList ); // 获取链表第一个节点
        while( pxListItem != pxListEnd )        // 遍历所有等待任务
        {
            pxNext = listGET_NEXT( pxListItem ); // 保存下一节点指针（因为当前节点可能被移除）
            uxBitsWaitedFor = listGET_LIST_ITEM_VALUE( pxListItem ); // 获取该任务等待的特征位
            
            xMatchFound = pdFALSE;             // 重置匹配标记

            /* 分离控制位和实际等待位 */
            uxControlBits = uxBitsWaitedFor & eventEVENT_BITS_CONTROL_BYTES; // 提取高8位控制信息
            uxBitsWaitedFor &= ~eventEVENT_BITS_CONTROL_BYTES; // 清除控制位得到原始等待位

            /* 情况1：任务等待任意一个事件位被设置 */
            if( ( uxControlBits & eventWAIT_FOR_ALL_BITS ) == ( EventBits_t ) 0 )
            {
                if( ( uxBitsWaitedFor & pxEventBits->uxEventBits ) != ( EventBits_t ) 0 )
                {
                    xMatchFound = pdTRUE;      // 检测到任一等待位被设置
                }
            }
            /* 情况2：任务需要所有等待位都被设置 */
            else if( ( uxBitsWaitedFor & pxEventBits->uxEventBits ) == uxBitsWaitedFor )
            {
                xMatchFound = pdTRUE;          // 所有等待位都已设置
            }

            /* 如果任务触发条件满足 */
            if( xMatchFound != pdFALSE )
            {
                /* 检查是否需要自动清除事件位 */
                if( ( uxControlBits & eventCLEAR_EVENTS_ON_EXIT_BIT ) != ( EventBits_t ) 0 )
                {
                    uxBitsToClear |= uxBitsWaitedFor; // 累加需要清除的事件位
                }

                /* STEP 3: 将任务从等待列表移至就绪列表 */
                ( void ) xTaskRemoveFromUnorderedEventList(
                    pxListItem,                     // 当前列表项
                    pxEventBits->uxEventBits | eventUNBLOCKED_DUE_TO_BIT_SET // 触发原因
                );
            }

            /* 移动到下一个节点（使用预先保存的pxNext指针，因为原链表结构可能已改变） */
            pxListItem = pxNext;
        }

        /* STEP 4: 批量清除需要自动清除的事件位 */
        pxEventBits->uxEventBits &= ~uxBitsToClear;
    }
    ( void ) xTaskResumeAll(); // 恢复调度器，退出临界区

    /* 返回更新后的事件组值 */
    return pxEventBits->uxEventBits;
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 函数功能：删除事件组并唤醒所有等待该事件组的任务              */
/* 参数：                                                   */
/*   xEventGroup - 要删除的事件组句柄                       */
/* 返回：无                                                   */
/* 说明：                                                   */
/*   该函数会唤醒所有等待该事件组的任务，并按配置释放动态分配的内存 */
/*-----------------------------------------------------------*/
void vEventGroupDelete( EventGroupHandle_t xEventGroup )
{
    EventGroup_t *pxEventBits = ( EventGroup_t * ) xEventGroup; // 类型转换
    const List_t *pxTasksWaitingForBits = &( pxEventBits->xTasksWaitingForBits ); // 获取等待任务链表

    vTaskSuspendAll(); // 挂起调度器，进入临界区
    {
        traceEVENT_GROUP_DELETE( xEventGroup ); // 追踪调试

        /* 遍历等待任务链表，唤醒所有任务 */
        while( listCURRENT_LIST_LENGTH( pxTasksWaitingForBits ) > ( UBaseType_t ) 0 )
        {
            /*--------- 步骤1：检查链表头是否有效 ---------*/
            configASSERT( pxTasksWaitingForBits->xListEnd.pxNext != ( ListItem_t * ) &( pxTasksWaitingForBits->xListEnd ) );
            /* 说明：
               - 确保链表头节点不是链表结束标记，防止误操作
               - listCURRENT_LIST_LENGTH 返回链表长度
               - pxTasksWaitingForBits->xListEnd.pxNext 应该指向链表中的有效节点 */

            /*--------- 步骤2：唤醒任务 ---------*/
            ( void ) xTaskRemoveFromUnorderedEventList(
                pxTasksWaitingForBits->xListEnd.pxNext, // 获取链表第一个节点
                eventUNBLOCKED_DUE_TO_BIT_SET           // 触发原因
            );
            /* 说明：
               - xTaskRemoveFromUnorderedEventList 从事件列表移除任务并将其加入就绪列表
               - eventUNBLOCKED_DUE_TO_BIT_SET 标识任务被事件触发唤醒
               - 传入链表头节点的下一个节点（有效任务节点） */
        }

        /*--------- 步骤3：释放内存（根据配置） ---------*/
        #if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 0 ) )
        {
            /* 配置仅支持动态分配时：
               - 直接释放事件组内存 */
            vPortFree( pxEventBits );
        }
        #elif( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
        {
            /* 配置同时支持动态和静态分配时：
               - 检查事件组是否动态分配，如果是则释放内存 */
            if( pxEventBits->ucStaticallyAllocated == ( uint8_t ) pdFALSE )
            {
                vPortFree( pxEventBits );
            }
            else
            {
                mtCOVERAGE_TEST_MARKER(); // 测试覆盖率占位符
            }
        }
        #endif /* configSUPPORT_DYNAMIC_ALLOCATION */
    }
    ( void ) xTaskResumeAll(); // 恢复调度器，退出临界区
}

/*-----------------------------------------------------------*/

/* For internal use only - execute a 'set bits' command that was pended from
an interrupt. */
void vEventGroupSetBitsCallback( void *pvEventGroup, const uint32_t ulBitsToSet )
{
    ( void ) xEventGroupSetBits( pvEventGroup, ( EventBits_t ) ulBitsToSet ); // 调用xEventGroupSetBits设置事件位
    /* 说明：
       - 从中断中调用，处理延迟的事件位设置请求
       - pvEventGroup 是事件组句柄
       - ulBitsToSet 是要设置的事件位掩码 */
}

/*-----------------------------------------------------------*/

/* For internal use only - execute a 'clear bits' command that was pended from
an interrupt. */
void vEventGroupClearBitsCallback( void *pvEventGroup, const uint32_t ulBitsToClear )
{
    ( void ) xEventGroupClearBits( pvEventGroup, ( EventBits_t ) ulBitsToClear ); // 调用xEventGroupClearBits清除事件位
    /* 说明：
       - 从中断中调用，处理延迟的事件位清除请求
       - pvEventGroup 是事件组句柄
       - ulBitsToClear 是要清除的事件位掩码 */
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* 函数功能：测试当前事件位是否满足等待条件                    */
/* 参数：                                                   */
/*   uxCurrentEventBits - 当前事件组中的事件位                  */
/*   uxBitsToWaitFor - 任务等待的事件位掩码                   */
/*   xWaitForAllBits - 是否需要所有等待位都置位 (pdTRUE/pdFALSE)  */
/* 返回：                                                    */
/*   pdTRUE - 满足等待条件; pdFALSE - 不满足等待条件          */
/*-----------------------------------------------------------*/
static BaseType_t prvTestWaitCondition( const EventBits_t uxCurrentEventBits, 
                                        const EventBits_t uxBitsToWaitFor, 
                                        const BaseType_t xWaitForAllBits )
{
    BaseType_t xWaitConditionMet = pdFALSE; // 初始化等待条件未满足

    if( xWaitForAllBits == pdFALSE ) // 等待任意一个位被置位
    {
        /* 检查是否有至少一个等待位被设置 */
        if( ( uxCurrentEventBits & uxBitsToWaitFor ) != ( EventBits_t ) 0 )
        {
            xWaitConditionMet = pdTRUE; // 至少一个位被设置，满足条件
        }
        else
        {
            mtCOVERAGE_TEST_MARKER(); // 用于测试覆盖率的占位符
        }
    }
    else // 等待所有等待位都被置位
    {
        /* 检查所有等待位是否都被设置 */
        if( ( uxCurrentEventBits & uxBitsToWaitFor ) == uxBitsToWaitFor )
        {
            xWaitConditionMet = pdTRUE; // 所有位都被设置，满足条件
        }
        else
        {
            mtCOVERAGE_TEST_MARKER(); // 用于测试覆盖率的占位符
        }
    }

    return xWaitConditionMet; // 返回是否满足等待条件
}

/*-----------------------------------------------------------*/
/* 条件编译：仅在启用追踪功能、定时器及延迟函数调用时包含以下代码 */
#if ( ( configUSE_TRACE_FACILITY == 1 ) && \
      ( INCLUDE_xTimerPendFunctionCall == 1 ) && \
      ( configUSE_TIMERS == 1 ) )

/* 函数功能：从中断服务程序(ISR)设置事件位                     */
/* 参数：                                                   */
/*   xEventGroup - 要操作的事件组句柄                       */
/*   uxBitsToSet - 需要设置的事件位掩码                     */
/*   pxHigherPriorityTaskWoken - 指向布尔值，指示是否需要调度   */
/* 返回：                                                    */
/*   pdPASS - 成功设置位; pdFAIL - 设置失败                   */
/* 说明：                                                   */
/*   该函数通过延迟函数调用，确保在ISR中安全地设置事件位         */
/*-----------------------------------------------------------*/
BaseType_t xEventGroupSetBitsFromISR( EventGroupHandle_t xEventGroup, 
                                      const EventBits_t uxBitsToSet, 
                                      BaseType_t *pxHigherPriorityTaskWoken )
{
    BaseType_t xReturn;

    traceEVENT_GROUP_SET_BITS_FROM_ISR( xEventGroup, uxBitsToSet ); // 追踪调试

    /* 向定时器守护任务发送延迟函数调用请求 */
    xReturn = xTimerPendFunctionCallFromISR(
        vEventGroupSetBitsCallback,   // 回调函数：实际设置事件位
        ( void * ) xEventGroup,        // 参数1：事件组句柄
        ( uint32_t ) uxBitsToSet,    // 参数2：要设置的事件位掩码
        pxHigherPriorityTaskWoken     // 参数3：指示是否需要调度
    );

    return xReturn; // 返回操作结果
}

#endif /* 条件编译结束 */
/*-----------------------------------------------------------*/

#if (configUSE_TRACE_FACILITY == 1) // 仅在启用追踪功能时包含以下代码

/* 函数功能：获取事件组的追踪编号                              */
/* 参数：                                                   */
/*   xEventGroup - 事件组句柄                                 */
/* 返回：                                                    */
/*   事件组的追踪编号（用于调试追踪）                         */
/* 说明：                                                   */
/*   该函数用于获取事件组的唯一追踪编号，便于调试和可视化工具使用 */
/*-----------------------------------------------------------*/
UBaseType_t uxEventGroupGetNumber( void* xEventGroup )
{
    UBaseType_t xReturn;
    EventGroup_t *pxEventBits = ( EventGroup_t * ) xEventGroup; // 类型转换

    if( xEventGroup == NULL ) // 检查事件组句柄是否为空
    {
        xReturn = 0; // 句柄无效，返回0
    }
    else
    {
        xReturn = pxEventBits->uxEventGroupNumber; // 返回事件组的追踪编号
    }

    return xReturn; // 返回结果
}

#endif /* configUSE_TRACE_FACILITY */



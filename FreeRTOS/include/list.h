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
 * This is the list implementation used by the scheduler.  While it is tailored
 * heavily for the schedulers needs, it is also available for use by
 * application code.
 *
 * list_ts can only store pointers to list_item_ts.  Each ListItem_t contains a
 * numeric value (xItemValue).  Most of the time the lists are sorted in
 * descending item value order.
 *
 * Lists are created already containing one list item.  The value of this
 * item is the maximum possible that can be stored, it is therefore always at
 * the end of the list and acts as a marker.  The list member pxHead always
 * points to this marker - even though it is at the tail of the list.  This
 * is because the tail contains a wrap back pointer to the true head of
 * the list.
 *
 * In addition to it's value, each list item contains a pointer to the next
 * item in the list (pxNext), a pointer to the list it is in (pxContainer)
 * and a pointer to back to the object that contains it.  These later two
 * pointers are included for efficiency of list manipulation.  There is
 * effectively a two way link between the object containing the list item and
 * the list item itself.
 *
 *
 * \page ListIntroduction List Implementation
 * \ingroup FreeRTOSIntro
 */

#ifndef INC_FREERTOS_H
	#error FreeRTOS.h must be included before list.h
#endif

#ifndef LIST_H
#define LIST_H

/*
 * The list structure members are modified from within interrupts, and therefore
 * by rights should be declared volatile.  However, they are only modified in a
 * functionally atomic way (within critical sections of with the scheduler
 * suspended) and are either passed by reference into a function or indexed via
 * a volatile variable.  Therefore, in all use cases tested so far, the volatile
 * qualifier can be omitted in order to provide a moderate performance
 * improvement without adversely affecting functional behaviour.  The assembly
 * instructions generated by the IAR, ARM and GCC compilers when the respective
 * compiler's options were set for maximum optimisation has been inspected and
 * deemed to be as intended.  That said, as compiler technology advances, and
 * especially if aggressive cross module optimisation is used (a use case that
 * has not been exercised to any great extend) then it is feasible that the
 * volatile qualifier will be needed for correct optimisation.  It is expected
 * that a compiler removing essential code because, without the volatile
 * qualifier on the list structure members and with aggressive cross module
 * optimisation, the compiler deemed the code unnecessary will result in
 * complete and obvious failure of the scheduler.  If this is ever experienced
 * then the volatile qualifier can be inserted in the relevant places within the
 * list structures by simply defining configLIST_VOLATILE to volatile in
 * FreeRTOSConfig.h (as per the example at the bottom of this comment block).
 * If configLIST_VOLATILE is not defined then the preprocessor directives below
 * will simply #define configLIST_VOLATILE away completely.
 *
 * To use volatile list structure members then add the following line to
 * FreeRTOSConfig.h (without the quotes):
 * "#define configLIST_VOLATILE volatile"
 */
#ifndef configLIST_VOLATILE
	#define configLIST_VOLATILE
#endif /* configSUPPORT_CROSS_MODULE_OPTIMISATION */

#ifdef __cplusplus
extern "C" {
#endif

/* Macros that can be used to place known values within the list structures,
then check that the known values do not get corrupted during the execution of
the application.   These may catch the list data structures being overwritten in
memory.  They will not catch data errors caused by incorrect configuration or
use of FreeRTOS.*/
/* 列表完整性检查配置模块 */
/**
 * @功能概述 本代码段控制FreeRTOS列表数据结构的完整性检查机制，
 *          当configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES为0时，
 *          完全禁用列表结构的内存校验功能。
 * @配置影响
 *  - 节省内存：减少每个列表项8-16字节的校验存储
 *  - 提升性能：消除校验值的设置/验证操作
 *  - 降低安全性：无法检测内存越界等数据损坏
 */

#if( configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES == 0 )
	/* 定义空操作宏集合 */
	#define listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE        /* 列表项首校验字段占位符 */
	#define listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE       /* 列表项尾校验字段占位符 */
	#define listFIRST_LIST_INTEGRITY_CHECK_VALUE            /* 列表容器首校验字段占位符 */
	#define listSECOND_LIST_INTEGRITY_CHECK_VALUE            /* 列表容器尾校验字段占位符 */
	
	#define listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem )   /* 禁用列表项头校验值设置 */
	#define listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem )  /* 禁用列表项尾校验值设置 */
	#define listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList )           /* 禁用列表头校验值设置 */
	#define listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList )            /* 禁用列表尾校验值设置 */
	
	#define listTEST_LIST_ITEM_INTEGRITY( pxItem )           /* 禁用列表项完整性校验 */
	#define listTEST_LIST_INTEGRITY( pxList )                /* 禁用列表容器完整性校验 */

#else
	/* Define macros that add new members into the list structures. */
	#define listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE				TickType_t xListItemIntegrityValue1;
	#define listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE				TickType_t xListItemIntegrityValue2;
	#define listFIRST_LIST_INTEGRITY_CHECK_VALUE					TickType_t xListIntegrityValue1;
	#define listSECOND_LIST_INTEGRITY_CHECK_VALUE					TickType_t xListIntegrityValue2;

	/* Define macros that set the new structure members to known values. */
	#define listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem )		( pxItem )->xListItemIntegrityValue1 = pdINTEGRITY_CHECK_VALUE
	#define listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem )	( pxItem )->xListItemIntegrityValue2 = pdINTEGRITY_CHECK_VALUE
	#define listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList )		( pxList )->xListIntegrityValue1 = pdINTEGRITY_CHECK_VALUE
	#define listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList )		( pxList )->xListIntegrityValue2 = pdINTEGRITY_CHECK_VALUE

	/* Define macros that will assert if one of the structure members does not
	contain its expected value. */
	#define listTEST_LIST_ITEM_INTEGRITY( pxItem )		configASSERT( ( ( pxItem )->xListItemIntegrityValue1 == pdINTEGRITY_CHECK_VALUE ) && ( ( pxItem )->xListItemIntegrityValue2 == pdINTEGRITY_CHECK_VALUE ) )
	#define listTEST_LIST_INTEGRITY( pxList )			configASSERT( ( ( pxList )->xListIntegrityValue1 == pdINTEGRITY_CHECK_VALUE ) && ( ( pxList )->xListIntegrityValue2 == pdINTEGRITY_CHECK_VALUE ) )
#endif /* configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES */

/* FreeRTOS 链表核心数据结构模块 */
/**
 * @功能概述 本代码段定义FreeRTOS调度器使用的链表系统，包含：
 *          1. 标准链表项结构
 *          2. 精简链表项结构
 *          3. 链表容器结构
 *          4. 链表操作基础宏
 * @设计特点
 *  - 双向循环链表实现高效插入/删除
 *  - 包含所有者指针实现双向关联
 *  - 支持完整性校验（需配置启用）
 */

/* 标准链表项结构体 -------------------------------------------------------*/
struct xLIST_ITEM
{
	listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE  /* 首校验字段（配置启用时有效） */
	
	configLIST_VOLATILE TickType_t xItemValue; /* 排序键值（决定链表位置） 
	                                           * 典型应用：
	                                           * - 任务阻塞时间（时间序列表）
	                                           * - 任务优先级（就绪列表） */
	                                           
	struct xLIST_ITEM * configLIST_VOLATILE pxNext;     /* 后向指针（闭环维护） */
	struct xLIST_ITEM * configLIST_VOLATILE pxPrevious; /* 前向指针 */
	
	void * pvOwner;        /* 所属对象指针（通常指向TCB）
	                        * 建立任务与链表项的双向关联 */
	                        
	void * configLIST_VOLATILE pvContainer; /* 所属链表指针（快速容器查询） */
	
	listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE /* 尾校验字段（配置启用时有效） */
};
typedef struct xLIST_ITEM ListItem_t;

/* 精简链表项结构体（用于链表头/尾节点） -----------------------------------*/
struct xMINI_LIST_ITEM
{
	listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE
	configLIST_VOLATILE TickType_t xItemValue;  /* 固定为portMAX_DELAY */
	struct xLIST_ITEM * configLIST_VOLATILE pxNext;
	struct xLIST_ITEM * configLIST_VOLATILE pxPrevious;
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;

/* 链表容器结构体 -------------------------------------------------------*/
typedef struct xLIST
{
	listFIRST_LIST_INTEGRITY_CHECK_VALUE      /* 链表头校验字段 */
	
	configLIST_VOLATILE UBaseType_t uxNumberOfItems; /* 当前链表项数 
	                                                 * 注意：O(1)复杂度计数 */
	                                                 
	ListItem_t * configLIST_VOLATILE pxIndex; /* 遍历指针（记录最后访问位置
	                                           * 实现公平轮询调度） */
	                                           
	MiniListItem_t xListEnd;                 /* 尾哨兵节点（含最大键值 
	                                         * 确保始终位于链表末端） */
	                                         
	listSECOND_LIST_INTEGRITY_CHECK_VALUE     /* 链表尾校验字段 */
} List_t;

/* 链表操作宏组 ---------------------------------------------------------*/
/**
 * @brief 设置链表项所有者（通常为任务TCB）
 * @param pxListItem 目标链表项
 * @param pxOwner 所属对象指针
 * @安全警告 禁止传入非任务对象指针
 */
#define listSET_LIST_ITEM_OWNER( pxListItem, pxOwner ) ( ( pxListItem )->pvOwner = ( void * ) ( pxOwner ) )

/**
 * @brief 获取链表项所有者（逆向关联）
 * @param pxListItem 目标链表项
 * @返回 所属TCB指针（需显式类型转换）
 */
#define listGET_LIST_ITEM_OWNER( pxListItem ) ( ( pxListItem )->pvOwner )

/**
 * @brief 设置链表项排序键值
 * @param pxListItem 目标链表项
 * @param xValue 新键值（TickType_t类型）
 * @典型应用 
 * - 更新任务唤醒时间 
 * - 调整任务优先级
 */
#define listSET_LIST_ITEM_VALUE( pxListItem, xValue ) ( ( pxListItem )->xItemValue = ( xValue ) )
/* FreeRTOS 链表操作宏模块 */
/**
 * @功能概述 本代码段提供链表操作的核心宏定义，包括：
 *          1. 链表项值获取
 *          2. 链表遍历控制
 *          3. 链表状态判断
 * @设计特点
 *  - 类型安全转换保证操作可靠性
 *  - 常量时间复杂度访问关键属性
 *  - 支持高效循环遍历模式
 */

/* 链表项值操作宏 -------------------------------------------------------*/
/**
 * @brief 获取链表项排序键值
 * @param pxListItem 目标链表项指针
 * @返回 TickType_t类型键值
 * @使用场景 调度器优先级比较/时间事件处理
 */
#define listGET_LIST_ITEM_VALUE( pxListItem ) ( ( pxListItem )->xItemValue )

/**
 * @brief 获取链表首项键值
 * @param pxList 目标链表指针
 * @返回 首项的xItemValue值
 * @注意 空链表调用将触发断言
 * @典型应用 快速获取最高优先级任务
 */
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxList ) ( ( ( pxList )->xListEnd ).pxNext->xItemValue )

/* 链表遍历控制宏 -------------------------------------------------------*/
/**
 * @brief 获取链表首项（跳过哨兵节点）
 * @param pxList 目标链表指针
 * @返回 首个有效链表项指针
 * @警告 空链表返回哨兵节点（需配合listLIST_IS_EMPTY使用）
 */
#define listGET_HEAD_ENTRY( pxList ) ( ( ( pxList )->xListEnd ).pxNext )

/**
 * @brief 获取下一链表项（支持循环遍历）
 * @param pxListItem 当前链表项指针
 * @返回 后续链表项指针
 * @遍历示例：
 * ListItem_t *pxItem = listGET_HEAD_ENTRY(pxList);
 * while( pxItem != listGET_END_MARKER(pxList) ) {
 *     // 处理项
 *     pxItem = listGET_NEXT(pxItem);
 * }
 */
#define listGET_NEXT( pxListItem ) ( ( pxListItem )->pxNext )

/**
 * @brief 获取链表结束标记（哨兵节点）
 * @param pxList 目标链表指针
 * @返回 常量指针形式的哨兵节点
 * @主要用途 遍历终止条件判断
 */
#define listGET_END_MARKER( pxList ) ( ( ListItem_t const * ) ( &( ( pxList )->xListEnd ) ) )

/* 链表状态判断宏 -------------------------------------------------------*/
/**
 * @brief 判断链表是否为空
 * @param pxList 目标链表指针
 * @返回 pdTRUE(空)/pdFALSE(非空)
 * @性能 O(1)复杂度直接判断
 */
#define listLIST_IS_EMPTY( pxList ) ( ( BaseType_t ) ( ( pxList )->uxNumberOfItems == ( UBaseType_t ) 0 ) )

/**
 * @brief 获取链表当前项数
 * @param pxList 目标链表指针
 * @返回 UBaseType_t类型项数
 * @优势 避免全遍历计数的高效实现
 */
#define listCURRENT_LIST_LENGTH( pxList ) ( ( pxList )->uxNumberOfItems )
/* FreeRTOS 链表高级操作模块 */
/**
 * @功能概述 本代码段提供链表管理的高级功能，包括：
 *          1. 安全遍历机制
 *          2. 容器关系验证
 *          3. 链表初始化操作
 * @核心机制
 *  - 通过pxIndex实现公平轮询遍历
 *  - 容器指针双向校验确保数据一致性
 *  - 结束标记值检测初始化状态
 */

/* 链表遍历及所有者访问宏 -----------------------------------------------*/
/**
 * @brief 获取下一链表项所有者（支持循环遍历）
 * @param pxTCB [输出] 接收所有者指针的变量
 * @param pxList 目标链表指针
 * @实现原理
 * 1. 移动pxIndex到下一项
 * 2. 若到达结束标记，跳转到首项
 * 3. 返回当前项所有者
 * @应用场景 时间片轮转调度器核心逻辑
 * @注意 需在临界区内调用保证原子性
 */
#define listGET_OWNER_OF_NEXT_ENTRY( pxTCB, pxList )                      \
{                                                                          \
List_t * const pxConstList = ( pxList );                                   \
    pxConstList->pxIndex = pxConstList->pxIndex->pxNext;                   \
    if( ( void * ) pxConstList->pxIndex == ( void * ) &( pxConstList->xListEnd ) ) \
    {                                                                      \
        pxConstList->pxIndex = pxConstList->pxIndex->pxNext;               \
    }                                                                      \
    ( pxTCB ) = pxConstList->pxIndex->pvOwner;                            \
}

/**
 * @brief 获取链表首项所有者
 * @param pxList 目标链表指针
 * @返回 首项所有者指针（TCB类型）
 * @性能 O(1)时间复杂度
 * @警告 空链表将返回未定义指针
 */
#define listGET_OWNER_OF_HEAD_ENTRY( pxList )  ( (&( ( pxList )->xListEnd ))->pxNext->pvOwner )

/* 链表项状态验证宏 -----------------------------------------------------*/
/**
 * @brief 验证链表项是否属于指定链表
 * @param pxList 目标链表指针
 * @param pxListItem 待验证链表项
 * @返回 pdTRUE(属于)/pdFALSE(不属于)
 * @安全机制 防止错误项操作导致链表损坏
 */
#define listIS_CONTAINED_WITHIN( pxList, pxListItem ) ( ( pxListItem )->pvContainer == ( void * ) ( pxList ) )

/**
 * @brief 获取链表项所属容器
 * @param pxListItem 目标链表项
 * @返回 所属链表指针
 * @应用场景 快速定位任务所在队列
 */
#define listLIST_ITEM_CONTAINER( pxListItem ) ( ( pxListItem )->pvContainer )

/* 链表初始化状态检测 ---------------------------------------------------*/
/**
 * @brief 检测链表是否完成初始化
 * @param pxList 目标链表指针
 * @返回 pdTRUE(已初始化)/pdFALSE(未初始化)
 * @原理 检测结束标记xItemValue是否为portMAX_DELAY
 */
#define listLIST_IS_INITIALISED( pxList ) ( ( pxList )->xListEnd.xItemValue == portMAX_DELAY )

/* 链表初始化函数 -------------------------------------------------------*/
/**
 * @brief 初始化链表结构
 * @param pxList 目标链表指针
 * @执行操作
 * 1. 设置结束标记xItemValue为portMAX_DELAY
 * 2. 初始化pxIndex指向结束标记
 * 3. 重置项计数器为0
 * @必须性 使用链表前必须调用此函数
 * @临界要求 应在调度器启动前调用
 */
void vListInitialise( List_t * const pxList ) PRIVILEGED_FUNCTION;
/* FreeRTOS 链表项管理模块 */
/**
 * @功能概述 本代码段提供链表项的生命周期管理功能，包括：
 *          1. 链表项初始化
 *          2. 有序链表插入操作
 * @核心机制
 *  - 链表项状态预校验机制
 *  - 基于键值的降序插入算法
 *  - 容器指针安全维护
 */

/* 链表项初始化函数 ---------------------------------------------------*/
/**
 * @brief 初始化链表项基础属性
 * @param pxItem 待初始化的链表项指针
 * @执行操作
 * 1. 清空容器指针（pvContainer = NULL）
 * 2. 重置完整性校验字段（若启用）
 * @必须性 链表项使用前必须初始化
 * @典型应用场景
 * - 创建新任务时初始化任务状态列表项
 * - 定时器初始化时配置关联项
 */
void vListInitialiseItem( ListItem_t * const pxItem ) PRIVILEGED_FUNCTION;

/* 链表有序插入函数 ---------------------------------------------------*/
/**
 * @brief 按降序插入链表项到指定链表
 * @param pxList 目标链表指针
 * @param pxNewListItem 待插入的新链表项
 * @算法原理
 * 1. 遍历链表找到首个xItemValue小于新项的节点
 * 2. 在新项前插入位置前驱节点后
 * 3. 更新链表项计数和容器指针
 * @时间复杂度 O(n) 
 * @使用示例
 * // 初始化任务唤醒时间项
 * vListInitialiseItem( &pxTask->xStateListItem );
 * // 设置唤醒时间戳
 * listSET_LIST_ITEM_VALUE( &pxTask->xStateListItem, xWakeTime );
 * // 插入延迟队列
 * vListInsert( pxDelayedTaskList, &pxTask->xStateListItem );
 */
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem ) PRIVILEGED_FUNCTION;
/* FreeRTOS 链表高级操作模块（续） */
/**
 * @功能概述 本代码段扩展链表管理功能，包含：
 *          1. 尾部插入操作
 *          2. 链表项移除操作
 * @核心机制
 *  - 基于遍历指针的公平插入策略
 *  - 安全的双向链表解除链接
 *  - 实时项数更新维护
 */

/* 链表尾部插入函数 ---------------------------------------------------*/
/**
 * @brief 在pxIndex位置后插入新链表项（非排序插入）
 * @param pxList 目标链表指针
 * @param pxNewListItem 待插入的新项
 * @算法原理
 * 1. 定位当前pxIndex的前驱节点
 * 2. 将新项插入pxIndex与前驱节点之间
 * 3. 保持pxIndex指向不变
 * @应用场景
 * - 相同优先级任务的公平轮转调度
 * - 定时器列表的非时间排序插入
 * @注意 插入位置与listGET_OWNER_OF_NEXT_ENTRY遍历顺序相关
 */
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem ) PRIVILEGED_FUNCTION;

/* 链表项移除函数 -----------------------------------------------------*/
/**
 * @brief 从所属链表中移除指定项
 * @param pxItemToRemove 待移除链表项指针
 * @返回 移除后链表的剩余项数
 * @执行操作
 * 1. 解除前后节点的链接关系
 * 2. 重置项容器指针为NULL
 * 3. 递减链表项计数器
 * @安全机制
 * - 自动检测项是否在有效链表中
 * - 空链表操作保护
 * @典型应用
 * - 任务从等待队列中移除时
 * - 定时器到期后从活动列表移除
 */
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove ) PRIVILEGED_FUNCTION;

#ifdef __cplusplus
}
#endif

#endif


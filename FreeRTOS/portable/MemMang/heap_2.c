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
 * A sample implementation of pvPortMalloc() and vPortFree() that permits
 * allocated blocks to be freed, but does not combine adjacent free blocks
 * into a single larger block (and so will fragment memory).  See heap_4.c for
 * an equivalent that does combine adjacent blocks into single larger blocks.
 *
 * See heap_1.c, heap_3.c and heap_4.c for alternative implementations, and the
 * memory management pages of http://www.FreeRTOS.org for more information.
 */
#include <stdlib.h>
/*-----------------------------------------------------------*/
/**
 * @file heap_2.c
 * @brief FreeRTOS内存管理方案2的初始化配置
 * 
 * 方案特性：
 * 1. 最佳适应算法（按空闲块大小排序）
 * 2. 支持动态分配与释放
 * 3. 可能产生内存碎片
 * 4. 适用于中等复杂度的动态内存需求
 */

/* 防止任务头文件重定义MPU包装器 */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 包含内核核心头文件 */
#include "FreeRTOS.h"   // 内核配置和数据类型
#include "task.h"       // 任务管理相关声明

/* 取消MPU包装器限制 */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 动态分配支持检查 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
    #error 本实现需要configSUPPORT_DYNAMIC_ALLOCATION=1
#endif

/* 调整后的堆空间大小（补偿地址对齐损失） */
#define configADJUSTED_HEAP_SIZE ( configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT )

/* 前置声明堆初始化函数 */
static void prvHeapInit( void );

/*-----------------------------------------------------------*/
/* 堆存储区定义 */
#if( configAPPLICATION_ALLOCATED_HEAP == 1 )
    /* 外部声明模式：允许应用自定义堆位置 */
    extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  // 用户定义存储位置
#else
    /* 静态分配模式：系统自动分配 */
    static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  // 默认堆数组
#endif

/*-----------------------------------------------------------*/
/* 内存块链表节点结构 */
typedef struct A_BLOCK_LINK {
    struct A_BLOCK_LINK *pxNextFreeBlock;  /* 指向下一个空闲块 */
    size_t xBlockSize;                     /* 当前空闲块总大小（含块头） */
} BlockLink_t;

/* 计算对齐后的块头结构大小 */
static const uint16_t heapSTRUCT_SIZE = 
    ( ( sizeof( BlockLink_t ) + ( portBYTE_ALIGNMENT - 1 ) ) /* 原始大小+最大补偿值 */
    & ~portBYTE_ALIGNMENT_MASK );  /* 位清除操作实现对齐 */

/* 定义最小内存块尺寸（防止碎片化） */
#define heapMINIMUM_BLOCK_SIZE ( ( size_t ) ( heapSTRUCT_SIZE * 2 ) )

/*-----------------------------------------------------------*/
/* 链表管理节点 */
static BlockLink_t xStart;  // 链表头节点（哨兵节点）
static BlockLink_t xEnd;    // 链表尾节点（边界标记）

/* 剩余可用内存统计 */
static size_t xFreeBytesRemaining = configADJUSTED_HEAP_SIZE;

/*-----------------------------------------------------------*/
/**
 * @brief 将空闲块插入有序链表的宏
 * @param pxBlockToInsert 待插入的空闲块指针
 * 
 * 排序规则：
 * 按内存块大小升序排列（小块在前，大块在后）
 * 实现最佳适应算法关键逻辑
 */
#define prvInsertBlockIntoFreeList( pxBlockToInsert )                      \
{                                                                          \
    BlockLink_t *pxIterator;                                               \
    size_t xBlockSize = pxBlockToInsert->xBlockSize;                       \
                                                                           \
    /* 遍历寻找首个比当前块大的节点 */                                     \
    for( pxIterator = &xStart;                                             \
         pxIterator->pxNextFreeBlock->xBlockSize < xBlockSize;             \
         pxIterator = pxIterator->pxNextFreeBlock )                        \
    { /* 空循环体 */ }                                                     \
                                                                           \
    /* 插入到合适位置 */                                                   \
    pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;        \
    pxIterator->pxNextFreeBlock = pxBlockToInsert;                        \
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/**
 * @brief 基于最佳适应算法的内存分配函数
 * @param xWantedSize 请求分配的内存字节数
 * @return void* 成功返回分配地址，失败返回NULL
 * 
 * 算法特点：
 * 1. 按空闲块大小升序排列链表
 * 2. 执行块分割优化内存利用率
 * 3. 不支持相邻空闲块合并
 * 4. 线程安全的分配操作
 */
void *pvPortMalloc( size_t xWantedSize )
{
BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
static BaseType_t xHeapHasBeenInitialised = pdFALSE; // 堆初始化标志
void *pvReturn = NULL;

    /* 挂起调度器保证原子操作 */
    vTaskSuspendAll();
    {
        /* 首次调用时的堆初始化 */
        if( xHeapHasBeenInitialised == pdFALSE )
        {
            prvHeapInit();                 // 初始化空闲链表结构
            xHeapHasBeenInitialised = pdTRUE; // 设置初始化完成标志
        }

        /* 计算实际需要的内存大小（包含管理头） */
        if( xWantedSize > 0 )
        {
            xWantedSize += heapSTRUCT_SIZE; // 增加块头大小

            /* 内存对齐处理 */
            if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0 )
            {
                /* 计算对齐补偿值（示例：xWantedSize=30需要补2达到32对齐） */
                xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
            }
        }

        /* 有效性检查：请求大小在合理范围内 */
        if( ( xWantedSize > 0 ) && ( xWantedSize < configADJUSTED_HEAP_SIZE ) )
        {
            /* 遍历空闲链表（从最小块开始） */
            pxPreviousBlock = &xStart;     // 链表头哨兵节点
            pxBlock = xStart.pxNextFreeBlock;
            while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock != NULL ) )
            {
                pxPreviousBlock = pxBlock; // 保存前驱节点
                pxBlock = pxBlock->pxNextFreeBlock; // 移动到下一个节点
            }

            /* 找到合适块且不是尾节点 */
            if( pxBlock != &xEnd )
            {
                /* 计算用户可用地址（跳过块头） */
                pvReturn = ( void * )( ( ( uint8_t * ) pxPreviousBlock->pxNextFreeBlock ) + heapSTRUCT_SIZE );

                /* 从空闲链表移除该块 */
                pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

                /* 块分割处理（当剩余空间足够时） */
                if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
                {
                    /* 计算新块位置（当前块地址 + 分配大小） */
                    pxNewBlockLink = ( void * )( ( ( uint8_t * ) pxBlock ) + xWantedSize );
                    
                    /* 设置新块属性 */
                    pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize; // 新块大小
                    pxBlock->xBlockSize = xWantedSize;           // 当前块设为分配大小

                    /* 将新块插入空闲链表 */
                    prvInsertBlockIntoFreeList( pxNewBlockLink ); // 按大小排序插入
                }

                /* 更新可用内存统计 */
                xFreeBytesRemaining -= pxBlock->xBlockSize; 
            }
        }

        /* 调试追踪（如果启用） */
        traceMALLOC( pvReturn, xWantedSize );
    }
    /* 恢复任务调度 */
    ( void ) xTaskResumeAll();

    /* 分配失败处理钩子 */
    #if( configUSE_MALLOC_FAILED_HOOK == 1 )
    {
        if( pvReturn == NULL )
        {
            extern void vApplicationMallocFailedHook( void );
            vApplicationMallocFailedHook(); // 触发用户定义回调
        }
    }
    #endif

    return pvReturn;
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/**
 * @brief 释放动态分配的内存块
 * @param pv 需要释放的内存指针
 * 
 * 实现特点：
 * 1. 将内存块重新插入空闲链表
 * 2. 更新可用内存统计
 * 3. 不执行相邻块合并操作
 * 4. 线程安全的释放操作
 */
void vPortFree( void *pv )
{
uint8_t *puc = ( uint8_t * ) pv;  // 将通用指针转换为字节指针
BlockLink_t *pxLink;              // 内存块管理头指针

    /* 空指针安全检查 */
    if( pv != NULL )
    {
        /* 定位内存块管理头（实际地址 = 用户地址 - 头结构大小） */
        puc -= heapSTRUCT_SIZE;  // 回退heapSTRUCT_SIZE个字节找到块头

        /* 类型转换抑制编译器对齐警告 */
        pxLink = ( void * ) puc;  // 将字节指针转换为块头结构指针

        /* 挂起调度器保证操作原子性 */
        vTaskSuspendAll();
        {
            /* 将块重新链接到空闲链表 */
            prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );  // 按大小排序插入

            /* 更新可用内存统计 */
            xFreeBytesRemaining += pxLink->xBlockSize;  // 增加可用内存计数

            /* 内存释放追踪（如果启用） */
            traceFREE( pv, pxLink->xBlockSize );  // 记录释放的内存信息
        }
        /* 恢复任务调度 */
        ( void ) xTaskResumeAll();
    }
}

/*-----------------------------------------------------------*/
/**
 * @brief 获取当前堆剩余可用内存量
 * @return size_t 可用字节数
 * 
 * 注意：
 * 返回值包含因内存碎片无法使用的空间
 */
size_t xPortGetFreeHeapSize( void )
{
    return xFreeBytesRemaining;  // 直接返回全局剩余内存统计值
}

/*-----------------------------------------------------------*/
/**
 * @brief 堆初始化兼容性函数（空实现）
 * 
 * 说明：
 * 此函数仅用于保持API兼容性
 * 实际初始化工作由prvHeapInit()完成
 */
void vPortInitialiseBlocks( void )
{
    /* 空实现用于消除链接器警告 */
}

/*-----------------------------------------------------------*/
/**
 * @brief 堆内存管理系统初始化
 * 
 * 初始化内容：
 * 1. 对齐堆起始地址
 * 2. 建立空闲链表结构
 * 3. 创建初始完整空闲块
 */
static void prvHeapInit( void )
{
BlockLink_t *pxFirstFreeBlock;    // 初始空闲块指针
uint8_t *pucAlignedHeap;         // 对齐后的堆起始地址

    /* 堆起始地址对齐处理 */
    pucAlignedHeap = ( uint8_t * ) ( 
        ( ( portPOINTER_SIZE_TYPE ) &ucHeap[ portBYTE_ALIGNMENT ] )  // 从ucHeap的对齐位置开始
        & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) )  // 地址对齐操作
    );

    /* 初始化空闲链表头节点 */
    xStart.pxNextFreeBlock = ( void * ) pucAlignedHeap;  // 指向第一个空闲块
    xStart.xBlockSize = ( size_t ) 0;                    // 头节点自身不占用空间

    /* 设置空闲链表尾节点属性 */
    xEnd.xBlockSize = configADJUSTED_HEAP_SIZE;  // 记录整个堆空间大小
    xEnd.pxNextFreeBlock = NULL;                 // 尾节点指向空

    /* 创建初始空闲块（占据整个可用堆空间） */
    pxFirstFreeBlock = ( void * ) pucAlignedHeap;          // 初始块起始地址
    pxFirstFreeBlock->xBlockSize = configADJUSTED_HEAP_SIZE; // 设置块大小为整个堆空间
    pxFirstFreeBlock->pxNextFreeBlock = &xEnd;             // 连接尾节点形成闭环
}

/*-----------------------------------------------------------*/

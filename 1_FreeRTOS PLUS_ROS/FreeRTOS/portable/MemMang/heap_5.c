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
 * A sample implementation of pvPortMalloc() that allows the heap to be defined
 * across multiple non-contigous blocks and combines (coalescences) adjacent
 * memory blocks as they are freed.
 *
 * See heap_1.c, heap_2.c, heap_3.c and heap_4.c for alternative
 * implementations, and the memory management pages of http://www.FreeRTOS.org
 * for more information.
 *
 * Usage notes:
 *
 * vPortDefineHeapRegions() ***must*** be called before pvPortMalloc().
 * pvPortMalloc() will be called if any task objects (tasks, queues, event
 * groups, etc.) are created, therefore vPortDefineHeapRegions() ***must*** be
 * called before any other objects are defined.
 *
 * vPortDefineHeapRegions() takes a single parameter.  The parameter is an array
 * of HeapRegion_t structures.  HeapRegion_t is defined in portable.h as
 *
 * typedef struct HeapRegion
 * {
 *	uint8_t *pucStartAddress; << Start address of a block of memory that will be part of the heap.
 *	size_t xSizeInBytes;	  << Size of the block of memory.
 * } HeapRegion_t;
 *
 * The array is terminated using a NULL zero sized region definition, and the
 * memory regions defined in the array ***must*** appear in address order from
 * low address to high address.  So the following is a valid example of how
 * to use the function.
 *
 * HeapRegion_t xHeapRegions[] =
 * {
 * 	{ ( uint8_t * ) 0x80000000UL, 0x10000 }, << Defines a block of 0x10000 bytes starting at address 0x80000000
 * 	{ ( uint8_t * ) 0x90000000UL, 0xa0000 }, << Defines a block of 0xa0000 bytes starting at address of 0x90000000
 * 	{ NULL, 0 }                << Terminates the array.
 * };
 *
 * vPortDefineHeapRegions( xHeapRegions ); << Pass the array into vPortDefineHeapRegions().
 *
 * Note 0x80000000 is the lower address so appears in the array first.
 *
 */
#include <stdlib.h>
/*-----------------------------------------------------------*/
/**
 * @file heap_5.c
 * @brief FreeRTOS内存管理方案5的初始化配置
 * 
 * 核心特性：
 * 1. 支持多个非连续内存区域
 * 2. 自动合并相邻空闲块
 * 3. 基于地址排序的空闲链表
 * 4. 可自定义内存区域布局
 */

/* 防止MPU包装器被重复定义 */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 包含内核核心头文件 */
#include "FreeRTOS.h"   // 内核配置及数据类型
#include "task.h"       // 任务管理相关API

/* 取消MPU包装器定义限制 */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* 动态分配支持检查（本实现依赖动态分配） */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
    #error 本文件需要configSUPPORT_DYNAMIC_ALLOCATION=1
#endif

/*-----------------------------------------------------------*/
/* 内存块最小尺寸限制（防止内存碎片） */
#define heapMINIMUM_BLOCK_SIZE    ( ( size_t ) ( xHeapStructSize << 1 ) )

/* 系统字节位数定义（用于位操作计算） */
#define heapBITS_PER_BYTE         ( ( size_t ) 8 )

/*-----------------------------------------------------------*/
/**
 * @brief 内存块链表节点结构
 * 
 * 管理特性：
 * - 按内存地址升序排列
 * - 支持相邻块自动合并
 */
typedef struct A_BLOCK_LINK {
    struct A_BLOCK_LINK *pxNextFreeBlock; /* 指向下一个空闲块（地址更高的块） */
    size_t xBlockSize;                    /* 当前块总大小（包含块头） */
} BlockLink_t;

/*-----------------------------------------------------------*/
/* 声明内存块插入函数原型 */
static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert );

/*-----------------------------------------------------------*/
/* 对齐后的块头结构尺寸计算 */
static const size_t xHeapStructSize = 
    ( sizeof( BlockLink_t ) + ( portBYTE_ALIGNMENT - 1 ) ) /* 原始尺寸+对齐补偿 */
    & ~( ( size_t ) portBYTE_ALIGNMENT_MASK ); /* 位掩码操作实现对齐 */

/* 链表管理节点 */
static BlockLink_t xStart;        // 链表头哨兵节点（固定位置）
static BlockLink_t *pxEnd = NULL; // 链表尾指针（动态计算）

/* 内存状态跟踪变量 */
static size_t xFreeBytesRemaining = 0U;            // 当前可用字节数
static size_t xMinimumEverFreeBytesRemaining = 0U; // 历史最小可用字节数

/* 块分配状态标记位 */
static size_t xBlockAllocatedBit = 0; /* 最高位作为分配标志位 */

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/**
 * @brief 基于多内存区域的动态内存分配
 * @param xWantedSize 请求分配字节数
 * @return void* 成功返回分配地址，失败返回NULL
 * 
 * 核心功能：
 * 1. 支持多非连续内存区域管理
 * 2. 首次适应算法搜索空闲块
 * 3. 自动内存块分割与合并
 * 4. 线程安全的内存操作
 */
void *pvPortMalloc( size_t xWantedSize )
{
BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
void *pvReturn = NULL;

    /* 堆初始化验证（必须调用vPortDefineHeapRegions初始化） */
    configASSERT( pxEnd );  // 确保已初始化内存区域

    /* 挂起所有任务保证原子操作 */
    vTaskSuspendAll();
    {
        /* 检查请求大小未占用分配标记位（最高位） */
        if( ( xWantedSize & xBlockAllocatedBit ) == 0 )
        {
            /* 计算实际需要的内存尺寸（包含管理头） */
            if( xWantedSize > 0 )
            {
                xWantedSize += xHeapStructSize;  // 增加块头大小

                /* 内存地址对齐处理 */
                if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0x00 )
                {
                    /* 计算对齐补偿值（示例：30字节对齐到32字节） */
                    xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
                    configASSERT( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) == 0 );
                }
            }

            /* 有效性检查：请求大小在可用内存范围内 */
            if( ( xWantedSize > 0 ) && ( xWantedSize <= xFreeBytesRemaining ) )
            {
                /* 首次适应算法遍历空闲链表 */
                pxPreviousBlock = &xStart;       // 链表头哨兵节点
                pxBlock = xStart.pxNextFreeBlock; // 第一个空闲块
                while( ( pxBlock->xBlockSize < xWantedSize ) && 
                       ( pxBlock->pxNextFreeBlock != NULL ) )
                {
                    pxPreviousBlock = pxBlock;
                    pxBlock = pxBlock->pxNextFreeBlock; // 线性搜索
                }

                /* 找到合适块且不是尾节点 */
                if( pxBlock != pxEnd )
                {
                    /* 计算用户可用地址（跳过块头） */
                    pvReturn = ( void * )( ( ( uint8_t * )pxPreviousBlock->pxNextFreeBlock ) + xHeapStructSize );

                    /* 从空闲链表移除该块 */
                    pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

                    /* 块分割处理（当剩余空间足够时） */
                    if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
                    {
                        /* 计算新块位置（当前块地址 + 分配大小） */
                        pxNewBlockLink = ( void * )( ( ( uint8_t * )pxBlock ) + xWantedSize );
                        
                        /* 设置新块属性 */
                        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
                        pxBlock->xBlockSize = xWantedSize;  // 当前块设为分配大小

                        /* 将剩余空间插入空闲链表 */
                        prvInsertBlockIntoFreeList( pxNewBlockLink ); // 自动合并相邻块
                    }

                    /* 更新内存统计信息 */
                    xFreeBytesRemaining -= pxBlock->xBlockSize;
                    if( xFreeBytesRemaining < xMinimumEverFreeBytesRemaining )
                    {
                        xMinimumEverFreeBytesRemaining = xFreeBytesRemaining; // 记录历史最小值
                    }

                    /* 标记为已分配块 */
                    pxBlock->xBlockSize |= xBlockAllocatedBit;  // 设置最高位标记
                    pxBlock->pxNextFreeBlock = NULL;            // 清除链表指针
                }
            }
        }

        /* 调试追踪信息（如果启用） */
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
 * 功能流程：
 * 1. 定位内存块管理头
 * 2. 验证块分配状态
 * 3. 清除分配标记位
 * 4. 将块重新插入空闲链表（自动合并相邻块）
 * 5. 支持多内存区域合并
 */
void vPortFree( void *pv )
{
uint8_t *puc = ( uint8_t * ) pv;  // 转换为字节指针操作
BlockLink_t *pxLink;               // 内存块管理头指针

    /* 空指针安全检查 */
    if( pv != NULL )
    {
        /* 回退指针定位块头（用户地址 - 头结构大小） */
        puc -= xHeapStructSize;    // xHeapStructSize为对齐后的块头大小

        /* 类型转换抑制编译器警告 */
        pxLink = ( void * ) puc;   // 获得实际块头地址

        /* 调试断言验证块状态 */
        configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 ); // 必须为已分配状态
        configASSERT( pxLink->pxNextFreeBlock == NULL );                  // 必须为孤立块

        /* 正式状态检查 */
        if( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 )
        {
            /* 确认未被重复释放 */
            if( pxLink->pxNextFreeBlock == NULL )
            {
                /* 清除最高位分配标记 */
                pxLink->xBlockSize &= ~xBlockAllocatedBit;  // 使用位清除操作

                /* 挂起调度器保证原子操作 */
                vTaskSuspendAll();
                {
                    /* 更新可用内存统计 */
                    xFreeBytesRemaining += pxLink->xBlockSize;  // 恢复内存计数
                    traceFREE( pv, pxLink->xBlockSize );         // 追踪调试信息

                    /* 执行智能合并插入 */
                    prvInsertBlockIntoFreeList( ( BlockLink_t * ) pxLink ); // 跨区域合并相邻块
                }
                /* 恢复任务调度 */
                ( void ) xTaskResumeAll();
            }
        }
    }
}

/*-----------------------------------------------------------*/
/**
 * @brief 获取当前可用堆内存量
 * @return size_t 可用字节数
 * 
 * 注意：
 * - 返回值包含碎片空间
 * - 不反映实际可分配最大块大小
 */
size_t xPortGetFreeHeapSize( void )
{
    return xFreeBytesRemaining;  // 返回实时统计的剩余内存值
}

/*-----------------------------------------------------------*/
/**
 * @brief 获取历史最小可用堆内存量
 * @return size_t 最小剩余字节数
 * 
 * 用途：
 * 1. 检测内存泄漏
 * 2. 评估内存配置合理性
 * 3. 优化内存区域划分
 */
size_t xPortGetMinimumEverFreeHeapSize( void )
{
    return xMinimumEverFreeBytesRemaining;  // 返回历史最低水位值
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/**
 * @brief 将空闲块插入链表并执行相邻块合并
 * @param pxBlockToInsert 待插入的空闲块指针
 * 
 * 核心功能：
 * 1. 按内存地址升序维护空闲链表
 * 2. 前向合并（与前一空闲块合并）
 * 3. 后向合并（与后一空闲块合并）
 * 4. 支持跨内存区域合并
 */
static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert )
{
BlockLink_t *pxIterator;  // 链表遍历指针
uint8_t *puc;             // 用于地址计算的字节指针

    /* 步骤1：找到合适插入位置（地址升序排列） */
    for( pxIterator = &xStart; 
         pxIterator->pxNextFreeBlock < pxBlockToInsert; 
         pxIterator = pxIterator->pxNextFreeBlock )
    {
        /* 空循环：遍历直到找到首个高地址块 */
    }

    /* 步骤2：前向合并检查（与前一空闲块合并） */
    puc = ( uint8_t * ) pxIterator;
    if( ( puc + pxIterator->xBlockSize ) == ( uint8_t * ) pxBlockToInsert )
    {
        /* 合并到前一区块 */
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;  // 合并块大小
        pxBlockToInsert = pxIterator;                          // 更新操作目标块
    }

    /* 步骤3：后向合并检查（与后一空闲块合并） */
    puc = ( uint8_t * ) pxBlockToInsert;
    if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t * ) pxIterator->pxNextFreeBlock )
    {
        if( pxIterator->pxNextFreeBlock != pxEnd )
        {
            /* 合并到后一区块 */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize; // 大小累加
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock; // 跳过被合并块
        }
        else
        {
            /* 处理尾节点特殊情况 */
            pxBlockToInsert->pxNextFreeBlock = pxEnd;  // 直接连接尾节点
        }
    }
    else
    {
        /* 无后向合并时直接链接后续块 */
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    /* 步骤4：更新链表连接（避免自环指针） */
    if( pxIterator != pxBlockToInsert )
    {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;  // 完成链表插入
    }
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/**
 * @brief 初始化多区域堆内存管理系统
 * @param pxHeapRegions HeapRegion_t结构体数组，定义多个内存区域
 * 
 * 功能说明：
 * 1. 支持多个非连续内存区域的初始化
 * 2. 自动处理每个区域的内存对齐
 * 3. 建立跨区域的空闲链表连接
 * 4. 计算总可用堆内存容量
 * 5. 设置内存管理核心参数
 * 
 * 注意事项：
 * - 必须在使用堆分配前调用且仅调用一次
 * - 内存区域必须按地址升序排列
 */
void vPortDefineHeapRegions( const HeapRegion_t * const pxHeapRegions )
{
BlockLink_t *pxFirstFreeBlockInRegion = NULL, *pxPreviousFreeBlock; // 区域首块指针和前区域尾指针
size_t xAlignedHeap;              // 对齐后的区域起始地址
size_t xTotalRegionSize, xTotalHeapSize = 0; // 区域有效尺寸和堆总大小
BaseType_t xDefinedRegions = 0;    // 已处理区域计数器
size_t xAddress;                  // 地址计算中间变量
const HeapRegion_t *pxHeapRegion;  // 当前处理区域指针

    /* 初始化安全检查（确保只调用一次） */
    configASSERT( pxEnd == NULL );  // pxEnd必须在未初始化状态

    /* 获取首个内存区域配置 */
    pxHeapRegion = &( pxHeapRegions[ xDefinedRegions ] );

    /* 遍历区域配置数组（直到遇到尺寸为0的终止项） */
    while( pxHeapRegion->xSizeInBytes > 0 )
    {
        /* 获取当前区域原始尺寸 */
        xTotalRegionSize = pxHeapRegion->xSizeInBytes;

        /* 处理区域起始地址对齐 */
        xAddress = ( size_t ) pxHeapRegion->pucStartAddress;  // 原始地址
        if( ( xAddress & portBYTE_ALIGNMENT_MASK ) != 0 )
        {
            /* 计算对齐补偿（示例：地址0x1007对齐到0x1000） */
            xAddress += ( portBYTE_ALIGNMENT - 1 );
            xAddress &= ~portBYTE_ALIGNMENT_MASK;

            /* 调整区域有效尺寸（扣除对齐损失的空间） */
            xTotalRegionSize -= xAddress - ( size_t ) pxHeapRegion->pucStartAddress;
        }

        xAlignedHeap = xAddress;  // 对齐后的起始地址

        /* 初始化首个区域 */
        if( xDefinedRegions == 0 )
        {
            /* 设置全局链表头节点 */
            xStart.pxNextFreeBlock = ( BlockLink_t * ) xAlignedHeap; // 指向首个区域
            xStart.xBlockSize = ( size_t ) 0;                       // 头节点自身无空间
        }
        else
        {
            /* 后续区域安全检查 */
            configASSERT( pxEnd != NULL );  // 确保前序区域已处理
            /* 验证区域地址升序排列 */
            configASSERT( xAddress > ( size_t ) pxEnd ); // 新区域地址必须高于前一区域
        }

        /* 保存前区域尾节点指针（用于跨区域链接） */
        pxPreviousFreeBlock = pxEnd;

        /* 设置当前区域尾节点 */
        xAddress = xAlignedHeap + xTotalRegionSize;  // 计算区域结束地址
        xAddress -= xHeapStructSize;                // 为尾节点保留空间
        xAddress &= ~portBYTE_ALIGNMENT_MASK;        // 尾节点地址对齐
        pxEnd = ( BlockLink_t * ) xAddress;          // 定位尾节点
        pxEnd->xBlockSize = 0;                       // 尾节点尺寸标记
        pxEnd->pxNextFreeBlock = NULL;               // 终止链表

        /* 创建当前区域初始空闲块 */
        pxFirstFreeBlockInRegion = ( BlockLink_t * ) xAlignedHeap;   // 块起始地址
        pxFirstFreeBlockInRegion->xBlockSize = xAddress - ( size_t ) pxFirstFreeBlockInRegion; // 计算可用空间
        pxFirstFreeBlockInRegion->pxNextFreeBlock = pxEnd;           // 连接尾节点

        /* 跨区域链表连接（当前区域与前区域链接） */
        if( pxPreviousFreeBlock != NULL )
        {
            pxPreviousFreeBlock->pxNextFreeBlock = pxFirstFreeBlockInRegion; // 形成跨区域链表
        }

        /* 累计总堆空间 */
        xTotalHeapSize += pxFirstFreeBlockInRegion->xBlockSize;

        /* 移动到下一个区域配置 */
        xDefinedRegions++;
        pxHeapRegion = &( pxHeapRegions[ xDefinedRegions ] ); // 获取下一个区域配置
    }

    /* 初始化内存统计参数 */
    xMinimumEverFreeBytesRemaining = xTotalHeapSize;  // 初始总空间即最小剩余
    xFreeBytesRemaining = xTotalHeapSize;             // 当前剩余空间等于总量

    /* 有效性验证（至少有一个有效区域） */
    configASSERT( xTotalHeapSize );

    /* 设置块分配标记位（使用size_t的最高位） */
    xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );
}



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
#include "FreeRTOS.h"
#include "list.h"

/*----------------------------------------------------------- 
 * 函数名称：vListInitialise
 * 功能描述：
 *   初始化FreeRTOS链表结构，构建空链表基础框架。主要完成：
 *   - 设置链表索引指针指向链表尾标记
 *   - 初始化链表尾节点的排序值（保证最大排序优先级）
 *   - 创建链表节点的双向环形指针结构
 *   - 重置链表节点计数器
 *   - 执行链表数据结构完整性校验（根据配置）
 * 参数：
 *   pxList [IN] 链表控制块指针，通过该参数操作链表结构体
 * 返回值：
 *   无
 * 详细说明：
 *   1. 初始化后的链表构成环形结构，xListEnd节点同时作为头尾节点
 *   2. 链表索引(pxIndex)初始指向尾节点，用于后续遍历起点
 *   3. 节点计数器(uxNumberOfItems)归零标志空链表状态
 *   4. 通过指针自引用实现空链表的闭环结构检测
 *   5. 使用portMAX_DELAY保证尾节点永远处于链表末端
 *-----------------------------------------------------------*/
void vListInitialise( List_t * const pxList )
{
    /* 核心指针初始化 ------------------------------------------------------*/
    /* 将链表索引指针指向链表尾节点（xListEnd）
     * 此处进行指针类型转换：
     * - 将List_t结构体的xListEnd成员地址转换为ListItem_t类型指针
     * - 此操作在FreeRTOS数据结构设计中合法，用于节省内存空间 */
    pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 已通过静态检查验证类型转换安全性 */

    /* 尾节点初始化 --------------------------------------------------------*/
    /* 设置尾节点的排序值为最大可能值（portMAX_DELAY通常定义为0xFFFFFFFF）
     * 确保该节点始终作为链表排序的基准点，维护在链表末端的位置 */
    pxList->xListEnd.xItemValue = portMAX_DELAY; 

    /* 构建环形指针结构 ----------------------------------------------------*/
    /* 前向指针初始化：
     * - 将尾节点的pxNext指针指向自身地址
     * - 形成初始闭环结构的关键步骤 */
    pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 相同类型转换验证 */

    /* 后向指针初始化：
     * - 将尾节点的pxPrevious指针同样指向自身
     * - 完成双向链表的自环结构，这是空链表的特征表现 */
    pxList->xListEnd.pxPrevious = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 同上类型转换检查 */

    /* 计数器清零 ----------------------------------------------------------*/
    /* 初始化链表节点数量计数器为0
     * - 使用UBaseType_t类型保证与系统位宽一致
     * - 显式转换为0值确保初始状态明确 */
    pxList->uxNumberOfItems = ( UBaseType_t ) 0U;

    /* 数据完整性校验 ------------------------------------------------------*/
    /* 条件编译的完整性检查：
     * - 当configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES设为1时生效
     * - 在链表头尾写入特定校验字节（通常为0x5A5A或0x5555）
     * - 用于运行时检测内存越界等数据损坏情况 */
    listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList ); // 设置头校验标记（如0x5A5A）
    listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList ); // 设置尾校验标记（如0x5555）
}

/*-----------------------------------------------------------*/

/*----------------------------------------------------------- 
 * 函数名称：vListInitialiseItem
 * 功能描述：
 *   初始化链表项数据结构，准备加入链表前的准备工作。主要完成：
 *   - 清除链表项的容器归属标记
 *   - 设置链表项数据完整性校验标记
 * 参数：
 *   pxItem [IN] 链表项控制块指针，需要初始化的链表项
 * 返回值：
 *   无
 * 详细说明：
 *   1. 通过置空容器指针(pvContainer)确保新链表项处于游离状态
 *   2. 在链表项首尾设置校验标记用于内存越界检测（需配置启用）
 *   3. 初始化后链表项需通过插入操作才能加入链表结构
 *-----------------------------------------------------------*/
void vListInitialiseItem( ListItem_t * const pxItem )
{
    /* 容器状态初始化 ------------------------------------------------------*/
    /* 重置容器归属指针：
     * - pvContainer存储链表项所属的链表控制块地址
     * - 设为NULL表示当前未加入任何链表结构
     * - 这是判断链表项是否在链中的关键标识 */
    pxItem->pvContainer = NULL;

    /* 数据完整性防护 ------------------------------------------------------*/
    /* 条件编译的完整性校验初始化：
     * - 当configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES启用时生效
     * - 在链表项首部写入校验值（通常为0x5A5A）
     * - 在链表项尾部写入校验值（通常为0x5555）
     * - 用于检测内存溢出等意外修改 */
    listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem ); // 首校验标记（32位）
    listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem ); // 尾校验标记（32位）
}

/*----------------------------------------------------------- 
 * 函数名称：vListInsertEnd
 * 功能描述：
 *   将指定链表项插入到链表的逻辑末端，维护链表的环形结构。主要完成：
 *   - 在pxIndex指向位置（当前遍历位置）后插入新项
 *   - 保持链表项的双向指针完整性
 *   - 更新链表项计数
 * 参数：
 *   pxList [IN] 目标链表控制块指针
 *   pxNewListItem [IN] 待插入的新链表项指针
 * 返回值：
 *   无
 * 详细说明：
 *   1. 插入操作不改变链表项排序值(xItemValue)的排列顺序
 *   2. 实际插入位置是当前遍历指针(pxIndex)的前驱位置
 *   3. 插入操作保持环形链表的双向指针正确性
 *   4. 包含数据完整性校验（需配置启用）
 *-----------------------------------------------------------*/
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem )
{
    /* 获取遍历定位指针 ----------------------------------------------------*/
    ListItem_t * const pxIndex = pxList->pxIndex; // 获取当前遍历位置指针
                                                   // 初始指向xListEnd节点

    /* 数据完整性验证 ------------------------------------------------------*/
    /* 运行时校验（仅在开启断言时生效）：
     * 1. 链表控制块校验：检查头尾校验值是否匹配
     * 2. 链表项校验：检查新项的完整性标记是否有效 */
    listTEST_LIST_INTEGRITY( pxList );       // 触发链表结构校验宏
    listTEST_LIST_ITEM_INTEGRITY( pxNewListItem ); // 触发链表项校验宏

    /* 指针链路构建 --------------------------------------------------------*/
    /* 前向指针设置：
     * - 新项的pxNext指向当前遍历位置（构成环形结构的关键）
     * - 形成 [新项]->[当前项] 的链接 */
    pxNewListItem->pxNext = pxIndex;

    /* 后向指针设置：
     * - 新项的pxPrevious指向当前项的前驱节点
     * - 形成 [前驱节点]<- [新项] 的链接 */
    pxNewListItem->pxPrevious = pxIndex->pxPrevious;

    /* 覆盖测试桩 ----------------------------------------------------------*/
    /* 为测试工具提供的空操作宏：
     * - 用于统计代码覆盖率时消除未执行路径警告
     * - 无实际功能影响 */
    mtCOVERAGE_TEST_DELAY(); // 覆盖率测试占位符

    /* 链表结构更新 --------------------------------------------------------*/
    /* 更新前驱节点指针：
     * - 原前驱节点的pxNext指向新插入项
     * - 完成 [前驱节点]->[新项] 的链接 */
    pxIndex->pxPrevious->pxNext = pxNewListItem;

    /* 更新当前项指针：
     * - 当前项的pxPrevious指向新插入项
     * - 完成 [新项]<- [当前项] 的链接 */
    pxIndex->pxPrevious = pxNewListItem;

    /* 归属关系标记 --------------------------------------------------------*/
    /* 设置链表项的容器指针：
     * - 将链表控制块地址存入pvContainer
     * - 用于快速判断链表项所属链表
     * - 类型转换为void*避免类型警告 */
    pxNewListItem->pvContainer = ( void * ) pxList;

    /* 链表计数递增 --------------------------------------------------------*/
    /* 更新链表项计数器：
     * - 使用无符号基础类型(UBaseType_t)保证原子操作
     * - 直接递增操作，无需锁保护（由调用方保证上下文安全） */
    ( pxList->uxNumberOfItems )++;
}
/*----------------------------------------------------------- 
 * 函数名称：vListInsert
 * 功能描述：
 *   将链表项按排序值(xItemValue)有序插入链表，维护链表的有序性。主要完成：
 *   - 定位符合排序要求的插入位置
 *   - 处理最大值边界情况(portMAX_DELAY)
 *   - 维护双向链表的指针完整性
 *   - 更新链表项计数及归属标记
 * 参数：
 *   pxList [IN] 目标链表控制块指针
 *   pxNewListItem [IN] 待插入的新链表项指针
 * 返回值：
 *   无
 * 详细说明：
 *   1. 当插入值等于portMAX_DELAY时特殊处理，保持链表尾节点始终为最大值
 *   2. 常规插入时使用线性搜索定位首个大于插入值的节点位置
 *   3. 插入操作保持链表项按升序排列
 *   4. 包含运行时数据完整性校验（需配置启用）
 *   5. 处理内存越界等常见错误场景（详见代码警告注释）
 *-----------------------------------------------------------*/
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem )
{
    ListItem_t *pxIterator;
    /* 获取插入项的排序键值 ------------------------------------------------*/
    const TickType_t xValueOfInsertion = pxNewListItem->xItemValue; // 提取新项的排序基准值
                                                                     // 类型与调度器时钟基准一致

    /* 数据完整性验证 ------------------------------------------------------*/
    /* 运行时安全检查（需启用configASSERT）：
     * 1. 校验链表控制块的完整性标记
     * 2. 校验新链表项的完整性标记 */
    listTEST_LIST_INTEGRITY( pxList );       // 触发链表结构校验宏
    listTEST_LIST_ITEM_INTEGRITY( pxNewListItem ); // 触发链表项校验宏

    /* 边界条件处理 --------------------------------------------------------*/
    if( xValueOfInsertion == portMAX_DELAY )
    {
        /* 最大值特殊处理逻辑：
         * - 直接定位到尾节点的前驱位置（倒数第二位置）
         * - 确保xListEnd始终作为最大值节点存在
         * - 防止遍历陷入死循环 */
        pxIterator = pxList->xListEnd.pxPrevious; // 获取尾节点前驱指针
    }
    else
    {
        /* 常见错误防护说明 --------------------------------------------------
         * 此处的NOTE列举了常见崩溃原因，包括：
         * 1. 栈溢出（需检查任务栈分配）
         * 2. Cortex-M中断优先级配置错误
         * 3. 在临界区/调度挂起状态调用API
         * 4. 未初始化队列/信号量提前使用
         * 详细解决方案参考FreeRTOS官方文档 */

        /* 遍历定位插入点 ----------------------------------------------------
         * 从链表头节点(xListEnd)开始遍历：
         * - 寻找第一个大于插入值的节点位置
         * - 最终插入在最后一个小于等于插入值的节点之后 */
        for( pxIterator = ( ListItem_t * ) &( pxList->xListEnd );   // 起始点为头节点地址
             pxIterator->pxNext->xItemValue <= xValueOfInsertion;  // 比较后继节点值
             pxIterator = pxIterator->pxNext )                     // 迭代指针移动
        {
            /* 空循环体：仅用于遍历定位，实际操作为指针迭代 */
            /*lint !e826 !e740 忽略类型转换警告，已验证数据结构设计合理性 */
        }
    }

    /* 四步指针更新法插入新项 ----------------------------------------------*/
    /* 步骤1：建立新项前向链接 */
    pxNewListItem->pxNext = pxIterator->pxNext; // 新项next指向当前项的后继节点
    /* 步骤2：更新后继节点的逆向链接 */
    pxNewListItem->pxNext->pxPrevious = pxNewListItem; // 后继节点的previous回指新项
    /* 步骤3：建立新项逆向链接 */
    pxNewListItem->pxPrevious = pxIterator;     // 新项previous指向当前项
    /* 步骤4：更新当前项的前向链接 */
    pxIterator->pxNext = pxNewListItem;          // 当前项next指向新项

    /* 归属关系标记 --------------------------------------------------------*/
    /* 设置链表项的容器指针：
     * - 记录所属链表控制块地址
     * - 支持快速移除操作时的链表定位 */
    pxNewListItem->pvContainer = ( void * ) pxList; // 类型转换消除编译警告

    /* 链表计数更新 --------------------------------------------------------*/
    /* 原子级递增操作：
     * - uxNumberOfItems使用基础无符号类型保证原子性
     * - 在调度器运行环境下操作安全 */
    ( pxList->uxNumberOfItems )++;
}

/*-----------------------------------------------------------*/
/*----------------------------------------------------------- 
 * 函数名称：uxListRemove
 * 功能描述：
 *   安全地从链表中移除指定链表项，维护数据结构完整性。主要完成：
 *   - 解除目标项与相邻节点的链接关系
 *   - 更新链表遍历索引指针（若受影响）
 *   - 清除链表项归属标记
 *   - 维护链表项计数器
 * 参数：
 *   pxItemToRemove [IN] 需要移除的链表项指针
 * 返回值：
 *   当前链表剩余项数（UBaseType_t类型）
 * 详细说明：
 *   1. 操作后保持链表双向指针的连续性
 *   2. 自动处理遍历指针(pxIndex)的特殊情况
 *   3. 返回更新后的链表项数便于状态监控
 *   4. 包含防御性指针操作确保系统稳定性
 *-----------------------------------------------------------*/
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
    /* 获取链表归属信息 ----------------------------------------------------*/
    /* 通过链表项的容器指针逆向获取所属链表：
     * - pvContainer存储链表控制块地址（由插入操作设置）
     * - 类型转换为List_t*需保证类型安全 */
    List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;

    /* 双向链表解链操作 ----------------------------------------------------*/
    /* 前向解链：
     * - 将后驱节点的previous指针指向当前项的前驱节点
     * - 跳过当前项实现解链 */
    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;

    /* 后向解链：
     * - 将前驱节点的next指针指向当前项的后驱节点
     * - 完成双向解链 */
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

    /* 覆盖率测试桩 --------------------------------------------------------*/
    /* 测试工具使用的空操作宏：
     * - 消除未执行路径警告
     * - 对实际功能无影响 */
    mtCOVERAGE_TEST_DELAY(); // 路径覆盖率测试点

    /* 遍历指针安全维护 ----------------------------------------------------*/
    if( pxList->pxIndex == pxItemToRemove )
    {
        /* 索引指针特殊处理：
         * - 当遍历指针指向被移除项时
         * - 将索引回退到前驱节点
         * - 防止后续遍历操作失效 */
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }
    else
    {
        /* 非关键路径标记：
         * - 用于覆盖率测试统计
         * - 无实际操作 */
        mtCOVERAGE_TEST_MARKER(); // 分支覆盖率标记
    }

    /* 状态清零操作 --------------------------------------------------------*/
    /* 重置容器指针：
     * - 清除链表项与链表的归属关系
     * - 作为"未链接"状态标识 */
    pxItemToRemove->pvContainer = NULL;

    /* 计数器更新 ----------------------------------------------------------*/
    /* 原子级递减操作：
     * - 使用无符号基础类型保证操作原子性
     * - 在调度器管理下无需额外锁保护 */
    ( pxList->uxNumberOfItems )--;

    /* 操作结果反馈 --------------------------------------------------------*/
    return pxList->uxNumberOfItems; // 返回当前链表项数
}

/*-----------------------------------------------------------*/


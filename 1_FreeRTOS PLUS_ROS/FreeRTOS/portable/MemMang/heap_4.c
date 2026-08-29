/**
  ******************************************************************************
  * @file    heap_4.c
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

/* 文件功能说明：
   1. 实现FreeRTOS的动态内存管理算法（DSA）版本4
   2. 支持内存块延迟合并机制，通过时间窗口和阈值控制合并频率
   3. 维护详细的堆内存统计信息，包括碎片分析和操作计数
   4. 提供内存分配安全检查，防止碎片化和小块内存浪费
-----------------------------------------------------------------------------*/

#include <stdlib.h>  // 包含标准库的内存管理函数声明

/* 定义MPU保护相关宏：
   防止FreeRTOS API被MPU包装器重复定义，确保在包含task.h时使用原始API实现 */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"  // FreeRTOS核心头文件
#include "task.h"      // 任务管理相关函数和类型定义
#include "system.h"    // 系统特定功能（如内存布局配置）
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE  // 取消宏定义防止后续包含污染

/* 编译时安全检查：必须启用动态内存分配 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
    #error 本文件要求启用动态内存分配，configSUPPORT_DYNAMIC_ALLOCATION必须设为1
#endif

/* 内存碎片防护配置：
   确保每个分配块至少包含两个堆结构体的空间，防止产生无法利用的微小碎片 */
#define heapMINIMUM_BLOCK_SIZE (xHeapStructSize << 1)  // 使用位运算快速计算2倍结构体大小

/* 系统字节位宽假设：
   基于8位字节系统进行内存计算，确保跨平台一致性 */
#define heapBITS_PER_BYTE      ( ( size_t ) 8 )

/* 延迟合并队列参数：
   设置合并队列最大长度，防止异常情况下队列无限增长 */
#define heapMAX_PENDING_BLOCKS  (16)      // 环形队列设计，最大保存16个待合并块

/* 合并时间窗口控制：
   500ms合并时间窗口平衡实时性和效率，避免频繁合并操作消耗CPU */
#define heapCOALESCE_DELAY_MS   (500UL)   // 使用无符号长整型确保跨平台兼容性

/* 批量合并触发阈值：
   当待合并块达到14个时立即触发合并（即使未到时间窗口），防止队列溢出 */
#define heapPENDING_THRESHOLD   (14)      // 保留2个位置作为安全缓冲

/* 堆内存分配方式选择：
   根据配置选择使用外部定义或内部静态分配的堆空间 */
#if( configAPPLICATION_ALLOCATED_HEAP == 1 )
    /* 外部引用声明：
       允许应用程序将堆放置在特定内存区域（如高速RAM或受保护区域） */
    extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  // 需在链接脚本中确保正确对齐
#else
    /* 静态内存分配：
       默认在BSS段分配堆空间，由编译器自动处理对齐 */
    static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  // static限定作用域在本文件
#endif

/* 堆统计信息结构体（增强版）：
   在标准统计基础上增加延迟合并相关指标 */
typedef struct {
    // 基础容量指标
    size_t xAvailableHeapSpaceInBytes;      // 实时可用字节数（包含可合并块）
    size_t xSizeOfLargestFreeBlockInBytes;  // 最大连续可用块（决定最大可分配值）
    size_t xSizeOfSmallestFreeBlockInBytes;// 反映内存碎片程度（值越小碎片越多）
    size_t xNumberOfFreeBlocks;             // 空闲块总数（包含待合并块）
    size_t xMinimumEverFreeBytesRemaining;  // 历史最低水位线（用于检测内存泄漏）

    // 操作统计
    size_t xNumberOfSuccessfulAllocations;  // 分配成功计数器（包含malloc和pvPortMalloc）
    size_t xNumberOfSuccessfulFrees;        // 释放成功计数器（包含free和vPortFree）

    /* 延迟合并系统指标 */
    size_t xNumberOfPendingBlocks;          // 当前待合并块数量（0表示无延迟块）
    size_t xTotalSizeOfPendingBlocks;        // 待合并块总大小（潜在可用空间）
    size_t xMaxPendingBlocks;                // 历史最大待合并数（用于调整队列尺寸）
    BaseType_t xDeferredCoalescingEnabled;   // 延迟合并功能开关（可运行时动态调整）
    TickType_t xLastCoalescingTime;          // 上次合并操作时间戳（单位：系统节拍）
} HeapStats_t;

/* 内存块链表节点（经典实现）：
   采用单链表结构管理空闲内存块 */
typedef struct A_BLOCK_LINK {
    struct A_BLOCK_LINK *pxNextFreeBlock;   // 后向指针（形成空闲链表）
    size_t xBlockSize;                      // 包含头部的总块大小（字节对齐值）
} BlockLink_t;

/* 延迟合并队列元素：
   记录待合并块的信息和入队时间 */
typedef struct PENDING_BLOCK {
    BlockLink_t *pxBlock;    // 指向待合并内存块的指针
    TickType_t xTimeStamp;   // 入队时间（用于判断是否超出合并时间窗口）
} PendingBlock_t;


/*----------------------------------------------------------------------------*/
/* 内存管理核心函数原型：
   实现内存块的链表管理、延迟合并机制和堆初始化逻辑 */
/*----------------------------------------------------------------------------*/

/* 内存块插入空闲链表函数：
   将释放的内存块智能插入空闲链表，自动与相邻块合并防止碎片 */
static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert );

/* 延迟合并队列插入函数：
   将待合并块加入队列，等待后续批量处理 */
static void prvInsertBlockIntoPendingList( BlockLink_t *pxBlockToInsert );

/* 待合并块处理函数：
   根据系统状态决定是否执行合并操作（支持强制合并模式）*/
static void prvProcessPendingBlocks( BaseType_t xForceCoalescing );

/* 堆初始化函数：
   首次内存分配时自动执行，建立内存管理基础结构 */
static void prvHeapInit( void );

/* 低功耗状态检测函数：
   返回系统是否处于低功耗模式，影响合并策略 */
static BaseType_t prvIsSystemInLowPowerMode( void );

/*----------------------------------------------------------------------------*/

/* 堆结构体对齐计算：
   确保内存块头部结构满足平台字节对齐要求（关键性能优化） */
static const size_t xHeapStructSize = 
    ( sizeof( BlockLink_t ) + ( ( size_t )( portBYTE_ALIGNMENT - 1 ) ) )  // 原始大小+最大偏移
    & ~( ( size_t ) portBYTE_ALIGNMENT_MASK );  // 位掩码清除低位实现向下对齐

/* 空闲链表管理节点：
   xStart作为链表哨兵节点，pxEnd指向链表末尾（动态更新） */
static BlockLink_t xStart;        // 链表起始标记（不包含实际内存块）
static BlockLink_t *pxEnd = NULL; // 链表结束指针（初始化为空）

/* 堆状态跟踪变量 */
static volatile size_t xFreeBytesRemaining = configTOTAL_HEAP_SIZE;// 实时可用内存量（动态变化）
static volatile size_t xMinimumEverFreeBytesRemaining = configTOTAL_HEAP_SIZE;    // 历史最低水位线（用于泄漏检测）
static volatile size_t xNumberOfSuccessfulAllocations = 0U;    // 成功分配计数器（调试用）
static volatile size_t xNumberOfSuccessfulFrees = 0U;          // 成功释放计数器（调试用）

/* 内存块状态标记位：
   使用size_t的最高位作为分配状态标志（优化存储空间） */
static size_t xBlockAllocatedBit = 0;  // 初始化时由prvHeapInit()设置具体值

/* 延迟合并系统控制变量 */
static PendingBlock_t xPendingBlocks[ heapMAX_PENDING_BLOCKS ]; // 环形缓冲队列
static volatile size_t xPendingBlockCount = 0;          // 当前队列有效元素计数
static BaseType_t xDeferredCoalescingEnabled = pdTRUE; // 功能开关（默认启用）
static TickType_t xLastCoalescingTime = 0;     // 最近合并操作的时间戳（tick单位）

/* 系统低功耗状态标志（外部变量）：
   当系统准备进入待机模式时，应暂停内存合并操作 */
extern uint8_t isReadyForStandby;  // 预期取值范围：0=正常模式，1=准备进入低功耗

/* 演示模式配置参数 */
#define DEMO_DELAY_SHORT    pdMS_TO_TICKS(100)  // 短延迟（用于快速响应场景）
#define DEMO_DELAY_MEDIUM   pdMS_TO_TICKS(500)  // 中等延迟（平衡模式）
#define DEMO_DELAY_LONG     pdMS_TO_TICKS(1000) // 长延迟（节能模式）

/* 演示控制全局变量 */
static volatile BaseType_t xDemoRunning = pdFALSE; // 演示模式运行标志（原子访问）
static UBaseType_t uxScenarioCounter = 1;        // 测试场景计数器（多模式切换）

/*----------------------------------------------------------------------------*/
/* 动态内存分配主函数：
   实现延迟合并感知的内存分配算法，支持块分割和智能合并策略 */
/*----------------------------------------------------------------------------*/
/**
 * @brief 动态内存分配核心函数 - 支持延迟合并策略的智能分配器
 * 
 * 本函数实现以下核心功能：
 * 1. 延迟合并预处理：根据阈值/低功耗状态/定期触发待处理块合并
 * 2. 首次运行初始化堆结构
 * 3. 内存对齐处理（默认8字节对齐）
 * 4. 首次适应算法搜索空闲块
 * 5. 内存块分割与空闲链表维护
 * 6. 分配标记设置与统计信息更新
 * 7. 内存分配失败钩子支持
 * 
 * @param xWantedSize 请求分配的内存大小（字节数）
 * @return void* 成功返回内存地址，失败返回NULL
 * 
 * @note 特性：
 * - 线程安全：通过挂起调度器保证原子操作
 * - 延迟初始化：首次调用时构建堆结构
 * - 防御性编程：包含对齐验证和断言保护
 * - 扩展统计：维护分配成功/失败计数器
 */
void *pvPortMalloc( size_t xWantedSize )
{
    // 定义遍历指针和返回指针
    BlockLink_t *pxBlock,          // 当前遍历块指针
               *pxPreviousBlock,  // 前驱块指针（用于链表操作）
               *pxNewBlockLink;   // 新分割块指针
    void *pvReturn = NULL;        // 返回值初始化为NULL
	
		static uint8_t numberOfRecursive = 0; //记录单次递归次数
    // 挂起所有任务保证内存操作原子性
    vTaskSuspendAll();
    {
        /*-------------------- 延迟合并预处理阶段 --------------------*/
        if( xPendingBlockCount > 0 ) 
        {
            BaseType_t xShouldProcess = pdFALSE;  // 合并触发标志
            
            /* 触发合并条件判断（三选一） */
            if( xPendingBlockCount >= heapPENDING_THRESHOLD ) // 阈值保护
            {
                xShouldProcess = pdTRUE;  // 高频释放触发批量处理
            }
            else if( prvIsSystemInLowPowerMode() == pdTRUE ) // 低功耗优化窗口
            {
                xShouldProcess = pdTRUE;  // 利用空闲时段处理
            }
            else if( ( xTaskGetTickCount() - xLastCoalescingTime ) > 
                    pdMS_TO_TICKS( heapCOALESCE_DELAY_MS ) ) // 定期维护
            {
                xShouldProcess = pdTRUE;  // 防止长期未合并产生碎片
            }
            
            /* 执行待处理块合并 */
            if( xShouldProcess == pdTRUE )
            {
                prvProcessPendingBlocks( pdFALSE ); // 参数pdFALSE表示非强制模式
            }
        }

        /*-------------------- 堆结构延迟初始化 --------------------*/
        if( pxEnd == NULL )  // 首次分配时初始化堆
        {
            prvHeapInit();  // 建立空闲链表和结束标记
        }
        else  // 已有初始化跳过
        {
            mtCOVERAGE_TEST_MARKER();  // 覆盖率测试标记（生产代码中为空宏）
        }

        /*-------------------- 分配参数预处理 --------------------*/
        /* 检查最高位是否被占用（防御非法参数） */
        if( ( xWantedSize & xBlockAllocatedBit ) == 0 ) 
        {
            /* 有效大小处理流程 */
            if( xWantedSize > 0 ) 
            {
                // 增加堆结构头大小（每个块包含BlockLink_t头）
                xWantedSize += xHeapStructSize; 

                /* 内存地址对齐处理（默认8字节对齐） */
                if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0x00 )
                {
                    // 计算需要补齐的字节数（位运算优化）
                    xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
                    // 断言验证对齐结果正确
                    configASSERT( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) == 0 ); 
                }
                else  // 已对齐情况
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else  // 请求0字节的特殊情况
            {
                mtCOVERAGE_TEST_MARKER();
            }

            /* 可用性检查（请求大小合法且剩余空间足够） */
            if( ( xWantedSize > 0 ) && ( xWantedSize <= xFreeBytesRemaining ) )
            {
                /*-------------------- 首次适应算法算法搜索 --------------------*/
                pxPreviousBlock = &xStart;  // 从哨兵节点开始
                pxBlock = xStart.pxNextFreeBlock;  // 首个空闲块
                // 遍历寻找第一个足够大的空闲块
                while( ( pxBlock->xBlockSize < xWantedSize ) && 
                      ( pxBlock->pxNextFreeBlock != NULL ) )
                {
                    pxPreviousBlock = pxBlock;
                    pxBlock = pxBlock->pxNextFreeBlock;
                }

                /* 找到合适块时的处理 */
                if( pxBlock != pxEnd )  // 排除结束标记
                {
                    // 计算返回地址（跳过块头部结构）
                    pvReturn = ( void * )( ( ( uint8_t * )pxPreviousBlock->pxNextFreeBlock ) + xHeapStructSize );

                    /*-------------------- 块分割处理 --------------------*/
                    /* 检查剩余空间是否足够创建新块 */
                    if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
                    {
												/* 从空闲链表移除当前块 */
												pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;
                        // 计算新块起始地址（类型转换避免对齐警告）
                        pxNewBlockLink = ( void * )( ( ( uint8_t * )pxBlock ) + xWantedSize );
                        // 验证新块地址对齐正确
                        configASSERT( ( ( ( size_t )pxNewBlockLink ) & portBYTE_ALIGNMENT_MASK ) == 0 );

                        /* 调整块尺寸 */
                        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;  // 新块大小
                        pxBlock->xBlockSize = xWantedSize;  // 当前块设为分配大小
                        
                        // 更新空闲内存统计（减去新块大小）
                        xFreeBytesRemaining -= pxNewBlockLink->xBlockSize;
                        // 修正释放计数器（因插入新块导致）
                        xNumberOfSuccessfulFrees--;
                        // 将新块插入空闲链表
                        prvInsertBlockIntoFreeList( pxNewBlockLink );
                    }
                    else  // 剩余空间不足最小块要求
                    {
                        /* 尝试合并碎片后重试 */
                        if( xPendingBlockCount > 0 ) 
                        {
                            prvProcessPendingBlocks( pdTRUE );  // 强制模式合并
													
														/* 从空闲链表移除当前块 */
														pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;	
													
                            /* 再次检查剩余空间 */
                            if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
                            {
                                // 同上分割处理流程
                                pxNewBlockLink = ( void * )( ( ( uint8_t * )pxBlock ) + xWantedSize );
                                configASSERT( ( ( ( size_t )pxNewBlockLink ) & portBYTE_ALIGNMENT_MASK ) == 0 );

                                pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
                                pxBlock->xBlockSize = xWantedSize;

                                xFreeBytesRemaining -= pxNewBlockLink->xBlockSize;
                                xNumberOfSuccessfulFrees--;
                                prvInsertBlockIntoFreeList( pxNewBlockLink );
                            }
                            else  // 合并后仍不满足
                            {
                                mtCOVERAGE_TEST_MARKER();
                            }
                        }
                        else  // 无待处理块可合并
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }
                    }

                    /* 更新全局内存统计 */
                    xFreeBytesRemaining -= pxBlock->xBlockSize;  // 扣除已分配块
                    // 更新历史最低水位
                    if( xFreeBytesRemaining < xMinimumEverFreeBytesRemaining )
                    {
                        xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
                    }

                    /* 标记为已分配状态 */
                    pxBlock->xBlockSize |= xBlockAllocatedBit;  // 设置最高位标记
                    pxBlock->pxNextFreeBlock = NULL;  // 断开链表连接
                }
            }
						else
						{
								/*-------------------- 空间不足补救机制 --------------------*/
								/* 优先尝试强制处理挂起块，释放更多内存 */
								if( xPendingBlockCount > 0 )
								{
										prvProcessPendingBlocks( pdTRUE );  // 强制合并所有挂起块

										/* 合并后再尝试满足请求 */
										if( ( xWantedSize > 0 ) && ( xWantedSize <= xFreeBytesRemaining ) && numberOfRecursive <= 10)
										{
												/* 可选：递归调用自身重新分配（需防止死循环） */
												numberOfRecursive ++;
												pvReturn = pvPortMalloc( xWantedSize - xHeapStructSize );//减去上述流程中增加的xHeapStructSize大小，才是原来的xWantedSize
												
										}
										else
										{
												/* 记录重试失败，可能为内存碎片或请求过大 */
												traceMALLOC( NULL, xWantedSize );
										}
								}
								else
								{
										/* 可选：记录失败日志或告警 */
										traceMALLOC( NULL, xWantedSize );
								}
						}
        }

        /* 调试追踪接口 */
        traceMALLOC( pvReturn, xWantedSize );  // 调用系统追踪钩子
        // 更新成功分配计数器，清零当次记录的递归次数
        if( pvReturn != NULL )
        {
            xNumberOfSuccessfulAllocations++; 
						numberOfRecursive = 0;
        }
    }
    ( void )xTaskResumeAll();  // 恢复任务调度（void转换消除编译器警告）

    /* 分配失败处理（用户钩子支持） */
    #if( configUSE_MALLOC_FAILED_HOOK == 1 )
    {
        if( pvReturn == NULL )
        {
            extern void vApplicationMallocFailedHook( void );
            vApplicationMallocFailedHook();  // 触发用户定义处理函数
        }
    }
    #endif

    /* 最终地址对齐验证 */
    configASSERT( ( ( ( size_t )pvReturn ) & ( size_t )portBYTE_ALIGNMENT_MASK ) == 0 );
    return pvReturn;  // 返回最终分配结果
}

/*----------------------------------------------------------------------------*/
/* 内存释放核心函数：
   实现安全的内存释放机制，支持延迟合并策略和防错检测 */
/*----------------------------------------------------------------------------*/
/**
 * @brief 动态内存释放核心函数 - 支持延迟合并的安全释放器
 * 
 * 本函数实现以下核心功能：
 * 1. 空指针安全过滤
 * 2. 内存块头定位与状态验证
 * 3. 双重释放检测与防护
 * 4. 延迟合并策略决策
 * 5. 空闲链表维护与统计更新
 * 
 * @param pv 需要释放的内存块地址（必须由pvPortMalloc分配）
 * 
 * @note 安全特性：
 * - 空指针安全处理
 * - 双重释放断言保护
 * - 链表完整性验证
 * - 原子操作保护（挂起调度器）
 */
void vPortFree( void *pv )
{
    // 将通用指针转为字节指针便于地址计算（兼容不同对齐方式）
    uint8_t *puc = ( uint8_t * ) pv;
    BlockLink_t *pxLink;  // 块结构指针

    /* 空指针安全过滤（防御性编程） */
    if( pv != NULL ) 
    {
        /* 定位块头部地址（关键逆向指针运算） */
        puc -= xHeapStructSize;        // 回退块头结构大小（sizeof(BlockLink_t)）
        pxLink = ( void * ) puc;       // 转换为块结构指针

        /* 块分配状态合法性验证 */
        if( ( pxLink->xBlockSize & xBlockAllocatedBit ) == 0 )
        {
            /* 双重释放防护：
               检测到分配位未设置，说明该块已被释放 */
            configASSERT(0);  // 调试模式下触发断言（生产环境可配置为日志）
            return;          // 安全退出防止统计信息污染
        }

        /* 链表指针有效性验证（防御内存越界） */
        if( pxLink->pxNextFreeBlock != NULL && 
            pxLink->pxNextFreeBlock != ( BlockLink_t * )0xFFFFFFFF )
        {
            /* 检测到异常指针值（可能因内存踩踏导致） */
            configASSERT(0);  // 触发调试断言
            return;          // 防止链表结构破坏
        }

        /* 清除分配标记（恢复原始块大小） */
        pxLink->xBlockSize &= ~xBlockAllocatedBit;  // 位操作清除最高位

        /* 挂起调度器保证原子操作 */
        vTaskSuspendAll();
        {
            // 调用系统追踪钩子（记录释放操作）
            traceFREE( pv, pxLink->xBlockSize );  

            /* 延迟合并决策逻辑 */
            if( ( xDeferredCoalescingEnabled == pdTRUE ) &&      // 功能开关检查
                ( prvIsSystemInLowPowerMode() == pdFALSE ) &&    // 非低功耗模式
                ( xPendingBlockCount < heapMAX_PENDING_BLOCKS ) ) // 队列容量检查
            {
                /* 阈值触发处理（队列达到处理阈值） */
                if( xPendingBlockCount >= heapPENDING_THRESHOLD )
                {
                    prvProcessPendingBlocks( pdTRUE ); // 强制模式处理积压块
                }
                
                /* 加入延迟处理队列 */
                prvInsertBlockIntoPendingList( pxLink );  // 插入待合并列表
            }
            else  // 直接合并模式
            {
                /* 立即合并到空闲链表 */
                prvInsertBlockIntoFreeList( pxLink );  // 传统合并方式
            }

            // 注释保留：原统计代码位置
            // xNumberOfSuccessfulFrees++; 
        }
        ( void ) xTaskResumeAll(); // 恢复任务调度（void转换消除编译器警告）
    }
}


/* 获取当前可用堆空间（实时值） */
size_t xPortGetFreeHeapSize( void )
{
    return xFreeBytesRemaining; // 用于内存监控和预警
}

/* 获取历史最低可用堆空间（泄漏检测） */
size_t xPortGetMinimumEverFreeHeapSize( void )
{
    return xMinimumEverFreeBytesRemaining; // 用于分析内存泄漏
}

/* 兼容性空函数：
   保留旧版本API，防止链接错误 */
void vPortInitialiseBlocks( void )
{
    /* 实际初始化在首次分配时通过prvHeapInit完成
       此函数仅为兼容旧版本设计 */
}


/*----------------------------------------------------------------------------*/
/**
 * @brief 强制处理待合并内存块的外部接口函数
 * 
 * 本函数提供紧急内存整理能力，用于立即执行待合并内存块的处理流程。
 * 主要功能：
 * 1. 通过挂起调度器进入临界区保证线程安全
 * 2. 检查待合并块计数器有效性
 * 3. 调用底层处理函数执行强制合并操作
 * 4. 恢复调度器运行环境
 * 适用场景：系统出现内存碎片紧急情况时主动触发内存整理
 */
/*----------------------------------------------------------------------------*/
void vPortProcessPendingBlocks( void )
{
    /* 挂起所有任务调度器：禁止任务切换，确保内存操作原子性 */
    vTaskSuspendAll(); // 进入临界区
    
    /* 显式代码块：限定临界区操作范围，为后续可能的扩展保持结构清晰 */
    {
        /* 有效性验证：仅在存在待合并块时执行操作 */
        if( xPendingBlockCount > 0 ) // 安全检查（防止空操作）
        {
            /* 调用底层处理函数：参数pdTRUE表示强制立即执行合并操作
               跳过常规的延迟合并策略 */
            prvProcessPendingBlocks( pdTRUE ); // 强制模式处理
        }
    }
    
    /* 恢复任务调度器：解除调度锁定，恢复系统正常运行
       (void)强制转换用于显式忽略返回值，强调此处不处理恢复结果 */
    ( void ) xTaskResumeAll(); // 退出临界区
}
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* 延迟合并开关函数：
   动态控制延迟合并功能的启用状态 */
/*----------------------------------------------------------------------------*/
/**
 * @brief 延迟合并功能开关控制函数
 * 
 * 本函数用于动态管理延迟合并策略的启用状态，主要功能包括：
 * 1. 原子性地更新延迟合并功能开关状态
 * 2. 禁用功能时自动清理待处理块
 * 3. 保证系统状态转换的线程安全性
 * 
 * @param xEnable 控制参数：
 *               - pdTRUE: 启用延迟合并
 *               - pdFALSE: 立即合并所有待处理块并禁用延迟功能
 * 
 * @note 安全特性：
 * - 全程挂起调度器保证状态切换原子性
 * - 禁用时自动清理防止内存泄漏
 * - 支持运行时动态配置策略
 */
void vPortSetDeferredCoalescing( BaseType_t xEnable )
{
    // 挂起所有任务保证配置操作的原子性
    vTaskSuspendAll(); 
    {
        /* 更新全局功能开关状态 */
        xDeferredCoalescingEnabled = xEnable; // 写入新状态（pdTRUE/pdFALSE）
        
        /* 功能禁用时的清理操作 */
        if( ( xEnable == pdFALSE ) &&         // 检测到禁用指令
            ( xPendingBlockCount > 0 ) )      // 存在待处理块时
        {
            /* 立即强制合并所有待处理块 */
            prvProcessPendingBlocks( pdTRUE ); // 参数pdTRUE表示强制合并模式
        }
    }
    // 恢复任务调度（void转换消除编译器警告）
    ( void ) xTaskResumeAll();
}

/*----------------------------------------------------------------------------*/
/* 获取待合并块数量：
   用于监控延迟合并队列状态 */
/*----------------------------------------------------------------------------*/
size_t xPortGetPendingBlockCount( void )
{
    return xPendingBlockCount; // 返回当前队列元素计数
}

/*----------------------------------------------------------------------------*/
/* 堆初始化核心函数：
   首次内存分配时执行，建立内存管理基础结构 */
/*----------------------------------------------------------------------------*/
/**
 * @brief 堆内存初始化函数 - 内存管理系统引导程序
 * 
 * 本函数在首次内存分配时执行，主要完成：
 * 1. 待处理队列初始化
 * 2. 堆空间地址对齐处理
 * 3. 空闲链表结构构建（头节点+尾节点）
 * 4. 初始大空闲块创建
 * 5. 统计信息初始化
 * 6. 分配标记位计算
 * 
 * @note 关键流程：
 * - 延迟初始化设计（首次分配时构建）
 * - 自动地址对齐补偿
 * - 防御性空间预留（防止尾节点越界）
 */
static void prvHeapInit( void )
{
    BlockLink_t *pxFirstFreeBlock;  // 初始空闲块指针
    uint8_t *pucAlignedHeap;       // 对齐后的堆起始地址
    size_t uxAddress;              // 地址计算临时变量
    size_t xTotalHeapSize = configTOTAL_HEAP_SIZE; // 获取配置的原始堆大小

    /*-------------------- 待处理队列初始化 --------------------*/
    for( size_t i = 0; i < heapMAX_PENDING_BLOCKS; i++ )
    {
        xPendingBlocks[i].pxBlock = NULL;    // 清空块指针（防御野指针）
        xPendingBlocks[i].xTimeStamp = 0;    // 重置时间戳为初始状态
    }
    xPendingBlockCount = 0;                  // 队列计数器归零
    xLastCoalescingTime = xTaskGetTickCount(); // 记录初始化时刻为首次合并时间

    /*-------------------- 堆地址对齐处理 --------------------*/
    uxAddress = ( size_t ) ucHeap; // 获取原始堆起始地址（configAPPLICATION_ALLOCATED_HEAP决定）
    if( ( uxAddress & portBYTE_ALIGNMENT_MASK ) != 0 ) // 检测未对齐情况
    {
        // 计算对齐补偿（示例：0x1000 3 -> +3 → 0x1003 & ~0x3 = 0x1000）
        uxAddress += ( portBYTE_ALIGNMENT - 1 ); 
        // 清除低位实现向下对齐（位运算优化）
        uxAddress &= ~( ( size_t ) portBYTE_ALIGNMENT_MASK );
        // 调整有效堆大小（扣除补偿空间）
        xTotalHeapSize -= uxAddress - ( size_t ) ucHeap;
    }
    pucAlignedHeap = ( uint8_t * ) uxAddress; // 转换为对齐后的字节指针

    /*-------------------- 空闲链表结构初始化 --------------------*/
    /* 头节点设置（哨兵节点） */
    xStart.pxNextFreeBlock = ( void * ) pucAlignedHeap; // 指向首个有效块
    xStart.xBlockSize = ( size_t ) 0;                 // 头节点自身不占用空间

    /* 尾节点计算与设置 */
    uxAddress = ( ( size_t ) pucAlignedHeap ) + xTotalHeapSize; // 原始末端地址
    uxAddress -= xHeapStructSize;             // 预留尾节点结构空间（防越界）
    uxAddress &= ~( ( size_t ) portBYTE_ALIGNMENT_MASK ); // 尾节点地址对齐
    pxEnd = ( void * ) uxAddress;             // 转换为块结构指针
    pxEnd->xBlockSize = 0;                    // 标记为结束节点
    pxEnd->pxNextFreeBlock = NULL;            // 链表终止标记

    /*-------------------- 初始空闲块创建 --------------------*/
    pxFirstFreeBlock = ( void * ) pucAlignedHeap; // 初始块起始地址
    // 计算可用空间（尾节点地址 - 起始地址）
    pxFirstFreeBlock->xBlockSize = uxAddress - ( size_t ) pxFirstFreeBlock; 
    pxFirstFreeBlock->pxNextFreeBlock = pxEnd; // 连接尾节点形成闭环

    /*-------------------- 统计信息初始化 --------------------*/
    xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize; // 初始即为最大值
    xFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;  // 当前可用空间
    // 调试用打印语句（示例：printf("当前可用堆大小: %u 字节\n", (unsigned)xFreeBytesRemaining*sizeof(size_t));）

    /* 分配标记位计算（平台适配） */
    xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );
    // 示例：32位系统 → 1<<31 → 0x80000000
}



/*----------------------------------------------------------------------------*/
/**
 * @brief 空闲链表插入函数 - 智能合并内存块
 * 
 * 本函数实现以下核心功能：
 * 1. 将释放的内存块插入空闲链表
 * 2. 前向/后向合并相邻空闲块
 * 3. 维护空闲内存统计信息
 * 4. 优化内存碎片
 * 
 * @param pxBlockToInsert 待插入的空闲块指针（必须已清除分配标记）
 * 
 * @note 算法特性：
 * - 前向合并：与物理地址连续的前驱块合并
 * - 后向合并：与物理地址连续的后继块合并
 * - 统计更新：实时跟踪可用空间变化
 */
static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert )
{
    BlockLink_t *pxIterator; // 链表遍历指针（当前节点）
    uint8_t *puc;            // 地址计算临时指针（字节级操作）
    
    /* 更新全局统计信息 */
    xFreeBytesRemaining += pxBlockToInsert->xBlockSize; // 增加可用空间
    xNumberOfSuccessfulFrees++; // 递增释放成功计数器

    /* 遍历寻找插入位置（升序地址排列） */
    for( pxIterator = &xStart;  // 从哨兵节点开始
         pxIterator->pxNextFreeBlock < pxBlockToInsert; // 比较地址大小
         pxIterator = pxIterator->pxNextFreeBlock ) // 移动至下一个节点
    {
        /* 空循环体：仅通过条件判断推进迭代器 */
    }

    /*-------------------- 前向合并检查 --------------------*/
    puc = ( uint8_t * ) pxIterator; // 当前节点地址
    /* 检查当前节点末尾是否与待插入块起始地址连续 */
    if( ( puc + pxIterator->xBlockSize ) == ( uint8_t * ) pxBlockToInsert )
    {
        /* 合并到前驱块：扩大前驱块尺寸 */
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
        pxBlockToInsert = pxIterator; // 将插入块指向前驱块（已合并）
    }

    /*-------------------- 后向合并检查 --------------------*/
    puc = ( uint8_t * ) pxBlockToInsert; // 待插入块地址
    /* 检查待插入块末尾是否与后继块起始地址连续 */
    if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t * ) pxIterator->pxNextFreeBlock )
    {   
        /* 处理后继块非尾节点的情况 */
        if( pxIterator->pxNextFreeBlock != pxEnd )
        {
            /* 合并后继块：扩大当前块尺寸 */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
            /* 跳过被合并的后继节点 */
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock; 
        }
        else  // 处理后继为尾节点的边界情况
        {
            pxBlockToInsert->pxNextFreeBlock = pxEnd;  // 直接连接尾节点
        }
    }
    else  // 无后向合并可能
    {
        /* 正常连接后继节点 */
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock; 
    }

    /* 更新前驱节点链接（排除前向合并后的自引用情况） */
    if( pxIterator != pxBlockToInsert )
    {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;  // 建立前驱到当前块的链接
    }
}

/*----------------------------------------------------------------------------*/
/* 延迟合并队列插入函数：
   管理待合并块队列，防止频繁合并 */
/*----------------------------------------------------------------------------*/
/**
 * @brief 延迟合并队列插入函数 - 待处理块生命周期管理
 * 
 * 本函数实现以下核心功能：
 * 1. 管理固定大小的待合并块队列
 * 2. 线性探测寻找可用槽位
 * 3. 记录块释放时间戳用于超时判断
 * 4. 队列溢出时的安全处理
 * 
 * @param pxBlockToInsert 待延迟合并的内存块指针
 * 
 * @note 队列管理策略：
 * - 队列未满：存储块信息并标记待处理状态
 * - 队列已满：立即执行传统合并
 * - 特殊标记：0xFFFFFFFF标识待合并块防止误操作
 */
static void prvInsertBlockIntoPendingList( BlockLink_t *pxBlockToInsert )
{
    /* 队列容量检查 */
    if( xPendingBlockCount < heapMAX_PENDING_BLOCKS ) 
    {
        /* 线性探测寻找空槽（时间复杂度O(n)） */
        for( size_t i = 0; i < heapMAX_PENDING_BLOCKS; i++ )
        {
            if( xPendingBlocks[i].pxBlock == NULL ) 
            {
                /* 记录块元数据 */
                xPendingBlocks[i].pxBlock = pxBlockToInsert;      // 存储块地址
                xPendingBlocks[i].xTimeStamp = xTaskGetTickCount();// 记录释放时刻(tick)
                xPendingBlockCount++;                             // 更新队列计数器

                /* 设置特殊标记：
                   将next指针设为0xFFFFFFFF标识待合并状态，
                   防止该块被错误地重新分配 */
                pxBlockToInsert->pxNextFreeBlock = ( BlockLink_t * )0xFFFFFFFF;
                break;  // 找到空槽后终止探测
            }
        }
    }
    else  // 队列已满处理策略
    {
        /* 安全回退：立即执行传统合并 */
        prvInsertBlockIntoFreeList( pxBlockToInsert ); 
    }
}


/**
 * @brief 待处理块处理函数 - 延迟合并策略执行器
 * 
 * 本函数根据以下条件处理延迟合并队列：
 * 1. 强制合并标志触发
 * 2. 块滞留时间超过阈值
 * 3. 系统进入低功耗模式
 * 4. 队列达到处理阈值
 * 
 * @param xForceCoalescing 强制合并标志：
 *                        - pdTRUE: 忽略条件立即合并
 *                        - pdFALSE: 按策略条件处理
 * 
 * @note 处理流程：
 * - 遍历队列检查处理条件
 * - 清除待处理标记
 * - 执行实际合并操作
 * - 更新队列状态与统计信息
 */
static void prvProcessPendingBlocks( BaseType_t xForceCoalescing )
{
    if( xPendingBlockCount == 0 ) return; // 空队列快速返回（优化性能）

    TickType_t xCurrentTime = xTaskGetTickCount(); // 获取当前系统时钟
    BaseType_t xAnyProcessed = pdFALSE; // 操作成功标志

    /* 遍历整个待处理队列 */
    for( size_t i = 0; i < heapMAX_PENDING_BLOCKS; i++ )
    {
        if( xPendingBlocks[i].pxBlock != NULL ) // 有效槽位检查
        {
            BaseType_t xShouldProcess = xForceCoalescing; // 初始化处理标志

            /* 非强制模式时检查条件 */
            if( !xShouldProcess )
            {
                /* 条件1：超过延迟时间（默认500ms） */
                if( ( xCurrentTime - xPendingBlocks[i].xTimeStamp ) >= 
                     pdMS_TO_TICKS( heapCOALESCE_DELAY_MS ) )
                {
                    xShouldProcess = pdTRUE; // 超时触发合并
                }
                /* 条件2：低功耗模式准备 */
                else if( prvIsSystemInLowPowerMode() == pdTRUE )
                {
                    xShouldProcess = pdTRUE; // 趁低功耗窗口合并
                }
                /* 条件3：队列达到处理阈值 */
                else if( xPendingBlockCount >= heapPENDING_THRESHOLD)
                {
                    xShouldProcess = pdTRUE; // 防止队列溢出
                }
            }

            /* 执行合并操作 */
            if( xShouldProcess )
            {
                /* 清除待处理标记（恢复NULL指针状态） */
                xPendingBlocks[i].pxBlock->pxNextFreeBlock = NULL;
                /* 实际合并到空闲链表 */
                prvInsertBlockIntoFreeList( xPendingBlocks[i].pxBlock );

                /* 清空队列项 */
                xPendingBlocks[i].pxBlock = NULL;    // 释放槽位
                xPendingBlocks[i].xTimeStamp = 0;    // 重置时间戳
                xPendingBlockCount--;                // 更新计数器
                xAnyProcessed = pdTRUE;             // 标记有操作
            }
        }
    }

    /* 更新最后合并时间（如有操作） */
    if( xAnyProcessed )
    {
        xLastCoalescingTime = xCurrentTime; 
    }
}

/*----------------------------------------------------------------------------*/
/* 低功耗模式检测函数：
   判断系统是否处于适合延迟合并的低功耗状态 */
/*----------------------------------------------------------------------------*/
static BaseType_t prvIsSystemInLowPowerMode( void )
{
    /* 基础检测：外部低功耗标志 */
    if( isReadyForStandby == pdTRUE ) // 检查全局变量
    {
        return pdTRUE; // 系统准备进入待机模式
    }
    
    /* 项目提升点2：扩展检测预留位（示例逻辑）：
       可添加更多检测条件，例如：
       - CPU利用率低于阈值
       - 所有任务处于阻塞状态
       - 外设进入低功耗模式 */
    
    return pdFALSE; // 默认返回正常模式
}

/**
 * @brief 堆内存状态统计函数
 * 
 * 本函数用于获取堆内存管理的详细统计信息，包括：
 * 1. 空闲内存块分布特征（最大/最小空闲块）
 * 2. 内存分配/释放历史记录
 * 3. 延迟合并机制相关状态
 * 4. 堆空间使用水位监测
 * 
 * @param pxHeapStats 输出参数，用于接收统计数据的结构体指针
 * 
 * @note 安全特性：
 * - 使用调度器挂起保护链表遍历操作
 * - 通过临界区保护全局变量访问
 * - 返回数据中的大小单位均为字节
 */
/*----------------------------------------------------------------------------*/
/* 堆状态统计函数：
   获取完整的堆内存使用情况报告，可自行打印查看 */
/*----------------------------------------------------------------------------*/
void vPortGetHeapStats( HeapStats_t *pxHeapStats )
{
    BlockLink_t *pxBlock;  // 空闲链表遍历指针
    size_t xBlocks = 0,    // 空闲块计数器
           xMaxSize = 0,   // 最大空闲块大小追踪
           xMinSize = portMAX_DELAY; // 最小空闲块初始化（portMAX_DELAY作为极大值便于比较） 
    size_t xPendingTotalSize = 0; // 待处理块总大小统计

    /* 挂起调度器确保链表遍历原子性 */
    vTaskSuspendAll();
    {
        /* 获取空闲链表首节点 */
        pxBlock = xStart.pxNextFreeBlock; // 从空闲链表头节点开始遍历
        
        /* 堆初始化状态检查 */
        if( pxBlock != NULL )
        {
            /* 遍历空闲链表直到结束标记pxEnd */
            while( pxBlock != pxEnd )
            {
                xBlocks++; // 递增空闲块计数
                
                /* 更新最大空闲块尺寸 */
                if( pxBlock->xBlockSize > xMaxSize )
                {
                    xMaxSize = pxBlock->xBlockSize; // 记录新的最大值
                }
                
                /* 更新最小空闲块尺寸 */
                if( pxBlock->xBlockSize < xMinSize )
                {
                    xMinSize = pxBlock->xBlockSize; // 记录新的最小值
                }
                
                /* 移动至下一个空闲块 */
                pxBlock = pxBlock->pxNextFreeBlock; // 通过next指针遍历链表
            }
        }

        /* 计算所有待处理块的总大小 */
        for( size_t i = 0; i < heapMAX_PENDING_BLOCKS; i++ ) // 遍历待处理块数组
        {
            if( xPendingBlocks[i].pxBlock != NULL ) // 跳过空槽位
            {
                xPendingTotalSize += xPendingBlocks[i].pxBlock->xBlockSize; // 累加有效块大小
            }
        }
    }
    ( void ) xTaskResumeAll(); // 恢复调度（强制转换避免编译器警告）

    /* 进入临界区保护全局统计变量 */
    taskENTER_CRITICAL();
    {
        /* 填充基础统计信息 */
        pxHeapStats->xSizeOfLargestFreeBlockInBytes = xMaxSize;     // 最大连续空闲块（单位：size_t个数）
        pxHeapStats->xSizeOfSmallestFreeBlockInBytes = xMinSize;   // 最小空闲块（反映内存碎片程度）
        pxHeapStats->xNumberOfFreeBlocks = xBlocks;               // 当前空闲块总数
        pxHeapStats->xAvailableHeapSpaceInBytes = xFreeBytesRemaining*sizeof(size_t); // 转换为字节单位
        pxHeapStats->xNumberOfSuccessfulAllocations = xNumberOfSuccessfulAllocations; // 成功分配次数
        pxHeapStats->xNumberOfSuccessfulFrees = xNumberOfSuccessfulFrees;            // 成功释放次数
        pxHeapStats->xMinimumEverFreeBytesRemaining = xMinimumEverFreeBytesRemaining*sizeof(size_t); // 历史最低水位

        /* 填充延迟合并扩展信息 */
        pxHeapStats->xNumberOfPendingBlocks = xPendingBlockCount;    // 当前待处理块数量
        pxHeapStats->xTotalSizeOfPendingBlocks = xPendingTotalSize*sizeof(size_t);   // 待处理块总字节数
        pxHeapStats->xDeferredCoalescingEnabled = xDeferredCoalescingEnabled; // 延迟合并功能开关状态
        pxHeapStats->xLastCoalescingTime = xLastCoalescingTime;      // 上次合并操作时间戳（tick）
        pxHeapStats->xMaxPendingBlocks = heapPENDING_THRESHOLD;      // 系统配置的待处理块阈值
    }
    taskEXIT_CRITICAL(); // 退出临界区
}

/**
 * @brief 格式化分隔线打印函数
 * 
 * 本函数用于在控制台输出带标题的格式化分隔线，主要作用包括：
 * 1. 提升多阶段调试信息的视觉区分度
 * 2. 结构化输出日志的章节划分
 * 3. 突出显示关键操作阶段的开始标记
 *
 * @param title 需要突出显示的标题文本（UTF-8编码）
 * 
 * @note 输出格式规范：
 * =================================================
 *  标题文本居中显示
 * =================================================
 */
/*----------------------------------------------------------------------------*/
/* 格式化打印辅助函数：
   增强调试信息可读性 */
/*----------------------------------------------------------------------------*/
void prvPrintSeparator( const char *title )
{
    printf("\n");  // 输出空行实现节段间隔
    printf("=================================================\n");  // 上分隔线（50字符宽度）
    printf(" %s\n", title);          // 带前导空格居中打印标题文本
    printf("=================================================\n");  // 下分隔线（保持对称）
}


/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/**
 * @brief 打印FreeRTOS内存管理状态信息
 * 
 * 本函数用于在指定阶段输出当前内存堆的状态信息，包括待处理内存块数量和可用堆空间大小。
 * 通常用于调试内存分配/释放过程中的状态变化，帮助开发者监控内存使用情况。
 * 
 * @param stage 当前调用阶段的标识字符串(如："Before alloc"、"After free")，
 *              用于在输出中区分不同场景的调用点。
 * 
 * @note 依赖于FreeRTOS内存管理API：
 *       - xPortGetPendingBlockCount() 获取待处理内存块数量
 *       - xPortGetFreeHeapSize() 获取当前可用堆大小（以size_t为单位）
 * 输出示例：[Before alloc] 待处理块数量: 5, 可用堆: 10240 字节
 */
/* 辅助函数：打印内存状态 */
void prvPrintMemoryStatus( const char *stage )
{
    /* 格式化输出内存状态信息：
       %s - 替换为传入的阶段标识字符串
       %u - 替换为待处理内存块数量（无符号整型）
       %u - 替换为可用堆字节数（无符号整型） */
    printf("[%s] 待处理块数量: %u, 可用堆: %u 字节\n", 
           stage,  // 当前操作阶段描述（由调用者传入）
           (unsigned)xPortGetPendingBlockCount(),  // 调用FreeRTOS API获取待分配内存块计数
           (unsigned)xPortGetFreeHeapSize()*sizeof(size_t));  // 计算实际可用堆字节数：
                                                             // 1. 获取可用堆单元数量
                                                             // 2. 乘以size_t类型大小（通常4字节）
                                                             // 3. 转换为无符号整型输出
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/**
 * @brief 场景1演示任务 - 推迟内存合并机制演示
 * 
 * 本任务通过分阶段的内存分配/释放操作，演示FreeRTOS heap_4内存管理策略中的
 * 推迟合并机制。主要展示以下特性：
 * 1. 内存释放后不会立即合并，而是进入待处理列表
 * 2. 超过heapCOALESCE_DELAY_MS时间阈值的块会被自动合并
 * 3. 支持手动触发待处理块的合并操作
 * 
 * @param pvParameters 任务参数（本例中未使用）
 * 
 * @note 操作流程：
 * 1. 顺序分配三个不同大小的内存块(A/B/C)
 * 2. 分阶段释放内存块并跟踪状态变化
 * 3. 演示自动超时处理和手动强制处理两种合并方式
 */
/* 场景1：正常运行状态演示任务 */
void prvScenario1Task( void *pvParameters )
{
    // 定义三个内存块指针，分别对应不同大小的分配
    void *ptrA, *ptrB, *ptrC;

    // 打印场景分隔线及说明信息
    prvPrintSeparator("场景1：正常运行状态\n");
    
    // 输出场景说明
    printf("此场景演示正常运行时的推迟合并策略\n");
    printf("内存块按时间顺序释放，模拟时间推移\n");
    printf("超过%ums的块会被自动处理\n", (unsigned)heapCOALESCE_DELAY_MS);
    

    /******************** 步骤1：分配内存块 ********************/
    printf("步骤1：分配内存块\n");
    // 分配不同大小的内存块（128B/256B/64B）
    ptrA = pvPortMalloc( 128 );  // 分配块A（128字节）
    ptrB = pvPortMalloc( 256 );  // 分配块B（256字节）
    ptrC = pvPortMalloc( 64 );   // 分配块C（64字节）
    
    // 验证分配结果
    if( ptrA && ptrB && ptrC )
    {
        printf("分配: 块A(128B), 块B(256B), 块C(64B) - 成功\n");
    }
    else
    {
        // 分配失败处理
        printf("内存分配失败！\n");
        goto cleanup_scenario1;  // 跳转到清理环节
    }
    // 打印分配后的内存状态
    prvPrintMemoryStatus("分配后");
    
    
    /******************** 步骤2：立即释放块A ********************/
    printf("步骤2：时间0ms - 释放块A\n");
    vPortFree(ptrA);     // 释放内存块A
    ptrA = NULL;         // 置空指针避免悬垂指针
    printf("块A已释放 → 加入待处理列表\n");
    prvPrintMemoryStatus("释放A后");
    
    
    /******************** 步骤3：延迟100ms释放块B ********************/
    vTaskDelay( DEMO_DELAY_SHORT );  // 延迟100ms（假设DEMO_DELAY_SHORT=100ms）
    printf("步骤3：时间100ms - 释放块B\n");
    vPortFree( ptrB );   // 释放内存块B
    ptrB = NULL;         // 置空指针
    printf("块B已释放 → 加入待处理列表\n");
    prvPrintMemoryStatus("释放B后");
    
    
    /******************** 步骤4：再延迟200ms释放块C ********************/
    vTaskDelay( pdMS_TO_TICKS(200) );  // 将200ms转换为系统节拍数
    printf("步骤4：时间300ms - 释放块C\n");
    vPortFree( ptrC );   // 释放内存块C
    ptrC = NULL;         // 置空指针
    printf("块C已释放 → 加入待处理列表\n");
    prvPrintMemoryStatus("释放C后");
    
    
    /******************** 步骤5：等待超时自动处理 ********************/
    printf("步骤5：等待超时检查...\n");
    // 等待（合并延迟阈值 + 100ms）的时长
    vTaskDelay( pdMS_TO_TICKS( heapCOALESCE_DELAY_MS + 100 ) );
    printf("等待%ums后，某些块可能已被自动处理\n", (unsigned)(heapCOALESCE_DELAY_MS + 100));
    
    // 项目提升点1：如何实现超时触发处理？
    // （提示：可利用系统滴答定时器维护时间戳，定期检查超时块）


    /******************** 手动强制处理剩余块 ********************/
    printf("手动触发处理剩余待处理块...\n");
    vPortProcessPendingBlocks();  // 调用待处理块处理函数
    prvPrintMemoryStatus("处理完成后");
    
    printf("场景1完成：演示了基于时间阈值的推迟合并机制\n");
    
    
/******************** 清理环节 ********************/
cleanup_scenario1:
    /* 安全释放可能残留的指针 */
    if( ptrA ) vPortFree( ptrA );  // 防御性编程：释放未清理的指针
    if( ptrB ) vPortFree( ptrB );
    if( ptrC ) vPortFree( ptrC );
    
    /* 更新场景计数器并删除任务 */
    uxScenarioCounter++;        // 场景计数器递增
    vTaskDelete( NULL );        // 删除当前任务（参数NULL表示当前任务）
}


/*-----------------------------------------------------------*/
/**
 * @brief 场景2演示任务 - 高频释放保护机制
 * 
 * 本任务通过高频率的内存释放操作，验证heap_4内存管理策略中的批量处理保护机制。
 * 主要演示以下特性：
 * 1. 待处理块数量达到heapPENDING_THRESHOLD阈值时触发自动批量处理
 * 2. 超过heapMAX_PENDING_BLOCKS最大容量的处理机制
 * 3. 高频操作下的内存管理稳定性
 * 
 * @param pvParameters 任务参数（本例中未使用）
 * 
 * @note 操作流程：
 * 1. 分配超过最大待处理数量+2的内存块
 * 2. 分阶段释放内存块观察阈值触发
 * 3. 演示自动批量处理和手动清理过程
 */
/* 场景2：高频释放状态演示任务 */
void prvScenario2Task( void *pvParameters )
{
    // 创建指针数组（比最大待处理数量多2个用于边界测试）
    void *ptrs[heapMAX_PENDING_BLOCKS + 2];  /* 略大于最大待处理数量 */
    int i;                                  // 循环计数器
    int allocatedCount = 0;                // 成功分配块计数器
    
    // 打印场景分隔线及说明
    prvPrintSeparator("场景2：高频释放状态\n");
    
    // 输出场景说明
    printf("此场景演示高频内存释放时的保护机制\n");
    printf("连续释放大量内存块\n");
    printf("达到阈值(%d个)时触发批量处理\n", heapPENDING_THRESHOLD);
    

    /******************** 初始化阶段 ********************/
    // 初始化指针数组（防御性编程）
    for( i = 0; i < (heapMAX_PENDING_BLOCKS + 2); i++ )
    {
        ptrs[i] = NULL;  // 显式置空避免野指针
    }
    

    /******************** 步骤1：批量分配内存块 ********************/
    printf("步骤1：尝试分配%d个内存块\n", heapMAX_PENDING_BLOCKS + 2);
    // 分配不同大小的内存块（64B起始，每块递增8B）
    for( i = 0; i < (heapMAX_PENDING_BLOCKS + 2); i++ )
    {
        ptrs[i] = pvPortMalloc( 64 + i * 8 );  // 分配逐渐增大的内存块
        if( ptrs[i] != NULL )
        {
            allocatedCount++;  // 成功计数
        }
        else
        {
            // 分配失败处理（通常由于堆空间不足）
            printf("内存分配失败于块%d\n", i);
            break;  // 中断分配循环
        }
    }
    printf("成功分配%d个内存块\n", allocatedCount);
    prvPrintMemoryStatus("分配后");
    

    /******************** 步骤2：阈值内高频释放 ********************/
    // 计算释放数量（不超过阈值或分配数-1）
    int releaseCount = (allocatedCount > heapPENDING_THRESHOLD) ? 
                      heapPENDING_THRESHOLD :  // 超过阈值时按阈值释放
                      allocatedCount - 1;      // 保留最后一个用于触发测试
    
    printf("步骤2：高频释放前%d个内存块\n", releaseCount);
    
    // 循环释放指定数量的内存块
    for( i = 0; i < releaseCount; i++ )
    {
        if( ptrs[i] != NULL )
        {
            vPortFree( ptrs[i] );  // 释放内存块
            ptrs[i] = NULL;       // 立即置空指针
            
            // 输出带字母标识的块状态（A,B,C...）
            printf("释放块%c → 待处理列表(当前%u个块)\n", 
                   'A' + i,  // 将索引转换为字母（0=A,1=B...）
                   (unsigned)xPortGetPendingBlockCount());  // 获取当前待处理数
            
            /* 模拟高频操作：延迟20ms（约相当于50Hz操作频率） */
            vTaskDelay( pdMS_TO_TICKS(20) );  // 将毫秒转换为系统节拍
        }
    }
    
    // 打印当前待处理状态
    printf("当前待处理块数量: %u\n", (unsigned)xPortGetPendingBlockCount());
    prvPrintMemoryStatus("释放多个块前");
    

    /******************** 步骤3：触发阈值保护机制 ********************/
    if( releaseCount < allocatedCount && ptrs[releaseCount] != NULL )
    {
        printf("步骤3：释放第%d个块 - 触发阈值保护\n", releaseCount + 1);
        
        vPortFree( ptrs[releaseCount] );  // 释放触发块
        ptrs[releaseCount] = NULL;        // 置空指针
        
        // 打印触发后的状态变化
        printf("释放后待处理块数量: %u\n", (unsigned)xPortGetPendingBlockCount());
        prvPrintMemoryStatus("触发批量处理后");
    }
    

    /******************** 步骤4：清理残余内存块 ********************/
    printf("步骤4：清理剩余已分配的块\n");
    // 释放releaseCount+1之后的所有成功分配块
    for( i = releaseCount + 1; i < allocatedCount; i++ )
    {
        if( ptrs[i] != NULL )
        {
            vPortFree( ptrs[i] );  // 安全释放
            ptrs[i] = NULL;       // 置空指针
        }
    }
    
    /* 强制处理剩余待处理块（确保堆状态重置） */
    printf("强制处理所有剩余待处理块...\n");
    vPortProcessPendingBlocks();  // 调用待处理块处理函数
    prvPrintMemoryStatus("全部释放后");
    
    printf("场景2完成：演示了高频释放时的批量处理保护机制\n");
    

    /******************** 任务收尾 ********************/
    /* 更新场景计数器并删除任务 */
    uxScenarioCounter++;    // 场景完成计数器递增
    vTaskDelete( NULL );   // 删除当前任务（传入NULL表示当前任务）
}

/*-----------------------------------------------------------*/
/**
 * @brief 场景3演示任务 - 低功耗模式内存优化
 * 
 * 本任务模拟设备进入低功耗模式前的内存优化过程，验证以下机制：
 * 1. 系统低功耗准备状态标志(isReadyForStandby)对内存管理的影响
 * 2. 待处理内存块的强制合并策略
 * 3. 内存碎片整理对低功耗模式的适配
 * 
 * @param pvParameters 任务参数（本例中未使用）
 * 
 * @note 操作流程：
 * 1. 分配多个不同大小内存块
 * 2. 部分释放创建待处理块但不触发自动处理
 * 3. 设置低功耗标志触发优化机制
 * 4. 演示低功耗准备状态下的自动内存整理
 */
/* 场景3：低功耗准备状态演示任务 */
void prvScenario3Task( void *pvParameters )
{
    // 定义8个内存块指针数组（典型低功耗场景测试规模）
    void *ptrs[8];         
    int i;                // 循环计数器
    int allocatedCount = 0; // 成功分配计数器
    
    // 打印场景分隔线及说明
    prvPrintSeparator("场景3：低功耗准备状态\n");
    
    // 输出场景说明
    printf("此场景演示系统进入低功耗前的内存优化\n");
    printf("模拟低功耗准备状态\n");
    printf("强制处理所有待合并块以减少碎片\n");
    

    /******************** 初始化阶段 ********************/
    /* 初始化指针数组（防御性编程） */
    for( i = 0; i < 8; i++ )
    {
        ptrs[i] = NULL;  // 显式初始化避免野指针
    }
    

    /******************** 步骤1：预分配测试块 ********************/
    printf("步骤1：分配8个内存块用于测试\n");
    // 分配逐渐增大的内存块（80B起始，每块增加24B）
    for( i = 0; i < 8; i++ )
    {
        ptrs[i] = pvPortMalloc( 80 + i * 24 );  // 计算块大小
        if( ptrs[i] != NULL )
        {
            allocatedCount++;  // 成功计数
        }
        else
        {
            // 分配失败处理（通常堆空间不足）
            printf("内存分配失败于块%d\n", i);
            break;  // 中断分配循环
        }
    }
    printf("成功分配%d个内存块\n", allocatedCount);
    prvPrintMemoryStatus("分配后");
    

    /******************** 步骤2：创建待处理块 ********************/
    // 计算释放数量（至少保留2个块用于后续测试）
    int releaseCount = (allocatedCount > 6) ? 6 : allocatedCount - 2;
    printf("步骤2：释放前%d个内存块\n", releaseCount);
    
    // 释放指定数量的内存块
    for( i = 0; i < releaseCount; i++ )
    {
        if( ptrs[i] != NULL )
        {
            vPortFree( ptrs[i] );  // 释放内存块
            ptrs[i] = NULL;       // 立即置空指针
            printf("释放块%c\n", 'A' + i);  // 字母标识块(A,B,C...)
            
            /* 模拟实时系统操作间隔 */
            vTaskDelay( pdMS_TO_TICKS(30) );  // 30ms延迟（约33Hz操作频率）
        }
    }
    
    // 打印当前待处理状态
    printf("当前待处理列表包含%u个块\n", (unsigned)xPortGetPendingBlockCount());
    prvPrintMemoryStatus("释放多个块后");
    

    /******************** 步骤3：激活低功耗模式 ********************/
    printf("步骤3：系统准备进入低功耗模式\n");
    printf("设置 isReadyForStandby = TRUE\n");
    isReadyForStandby = pdTRUE;  // 设置全局低功耗准备标志
    
    // 说明低功耗检测机制
    printf("系统检测到低功耗准备状态\n");
    printf("→ prvIsSystemInLowPowerMode() 返回 TRUE\n");
    printf("→ 下次内存操作时将强制处理所有待合并块...\n");
    

    /******************** 步骤4：触发优化机制 ********************/
    printf("步骤4：触发内存分配以激活低功耗处理\n");
    // 分配测试块（实际触发低功耗处理逻辑）
    void *testPtr = pvPortMalloc( 32 );  // 小内存块分配
    if( testPtr != NULL )
    {
        // 分配成功说明优化机制已执行
        printf("分配测试块成功，低功耗优化已触发\n");
        vPortFree( testPtr );  // 立即释放测试块
    }
    prvPrintMemoryStatus("低功耗优化后");
    

    /******************** 步骤5：最终清理 ********************/
    printf("步骤5：手动确保完全清理\n");
    vPortProcessPendingBlocks();  // 强制处理剩余块
    printf("强制处理完成\n");
    prvPrintMemoryStatus("完全清理后");
    
    /* 清理残余分配块（防御性编程） */
    printf("清理剩余测试块...\n");
    for( i = releaseCount; i < allocatedCount; i++ )
    {
        if( ptrs[i] != NULL )
        {
            vPortFree( ptrs[i] );  // 安全释放
            ptrs[i] = NULL;       // 置空指针
        }
    }
    prvPrintMemoryStatus("清理后");
    
    printf("场景3完成：演示了低功耗前的内存状态优化\n");
    

    /******************** 任务收尾 ********************/
    /* 更新场景计数器并删除任务 */
    uxScenarioCounter++;    // 场景完成计数器递增
    vTaskDelete( NULL );   // 删除当前任务（传入NULL表示当前任务）
}


/*-----------------------------------------------------------*/

/* Task handle */
TaskHandle_t MainControlTask_Handler;// 存储prvInnovation_Point_2_Test_Task任务的句柄

/**
 * @brief 主控制任务 - 内存管理演示调度器
 * 
 * 本任务作为演示程序的总控制器，负责：
 * 1. 系统初始化及参数展示
 * 2. 依次创建并监控三个测试场景任务
 * 3. 协调场景执行顺序及状态跟踪
 * 4. 演示结束后的资源清理
 * 
 * @param pvParameters 任务参数（本例中未使用）
 * 
 * @note 执行流程：
 * 1. 打印系统配置参数
 * 2. 启用推迟合并功能
 * 3. 顺序执行场景1-3
 * 4. 每个场景完成后自动推进
 */
/* 主控制任务 */
void prvInnovation_Point_2_Test_Task( void *pvParameters )
{
    // 打印程序标题和分隔线
    prvPrintSeparator("FreeRTOS 内存推迟合并策略演示程序\n");
    
    // 输出程序目标和配置信息
    printf("程序目标:\n");
    printf("演示修改版heap_4.c的推迟合并策略\n");
    printf("包含三个关键场景的完整测试\n");
    
    printf("系统配置:\n");
    printf("当前可用堆大小: %u 字节\n", (unsigned)xFreeBytesRemaining*sizeof(size_t)); // 实际可用字节数
    printf("最大待处理块: %d 个\n", heapMAX_PENDING_BLOCKS);      // 待处理列表容量
    printf("合并延迟阈值: %u ms\n", (unsigned)heapCOALESCE_DELAY_MS); // 自动合并延迟时间
    printf("批量处理阈值: %d 个\n", heapPENDING_THRESHOLD);       // 触发批量处理的块数
    
    // 等待确保输出可见
    vTaskDelay( DEMO_DELAY_SHORT );  
    
    /* 启用推迟合并功能 */
    vPortSetDeferredCoalescing( pdTRUE );  // 调用API启用特性
    printf("推迟合并功能: 已启用\n");
    
    /* 初始化统计计数器 */
    xNumberOfSuccessfulAllocations = 0U;  // 成功分配计数器清零
    xNumberOfSuccessfulFrees = 0U;       // 成功释放计数器清零
    //xPendingBlockCount =0U;             // （注释保留）待处理块计数器
    xDemoRunning = pdTRUE;                // 设置演示运行标志


    /******************** 场景1：基础推迟合并演示 ********************/
    printf("启动场景1...\n");
    // 创建场景1任务（优先级1，3倍最小栈）
    if( xTaskCreate( prvScenario1Task,       // 任务函数指针
                    "Scenario1",            // 任务名称
                    configMINIMAL_STACK_SIZE * 3, // 堆栈大小（通常>=128*3=384字）
                    NULL,                   // 无传入参数
                    tskIDLE_PRIORITY + 1,   // 优先级高于空闲任务
                    NULL ) == pdPASS )      // 不返回任务句柄
    {
        // 等待场景计数器更新（1→2）
        while( uxScenarioCounter < 2 ) 
        {
            vTaskDelay( DEMO_DELAY_SHORT ); // 短延迟轮询（100ms）
        }
        vTaskDelay( DEMO_DELAY_LONG );      // 场景间长延迟（500ms）
    }
    else
    {
        printf("场景1任务创建失败\n");  // 错误处理
    }
    

    /******************** 场景2：高频释放保护演示 ********************/
    printf("启动场景2...\n");
    // 创建场景2任务（更高优先级3，同栈配置）
    if( xTaskCreate( prvScenario2Task, 
                    "Scenario2",
                    configMINIMAL_STACK_SIZE * 3, 
                    NULL, 
                    tskIDLE_PRIORITY + 3,  // 提高优先级确保及时响应
                    NULL ) == pdPASS )
    {
        // 等待计数器更新（2→3）
        while( uxScenarioCounter < 3 )
        {
            vTaskDelay( DEMO_DELAY_SHORT );
        }
        vTaskDelay( DEMO_DELAY_LONG );
    }
    else
    {
        printf("场景2任务创建失败\n");
    }
    

    /******************** 场景3：低功耗优化演示 ********************/
    printf("启动场景3...\n");
    // 创建场景3任务（同场景2优先级）
    if( xTaskCreate( prvScenario3Task,
                    "Scenario3",
                    configMINIMAL_STACK_SIZE * 3,
                    NULL,
                    tskIDLE_PRIORITY + 3, 
                    NULL ) == pdPASS )
    {
        // 等待计数器更新（3→4）
        while( uxScenarioCounter < 4 )
        {
            vTaskDelay( DEMO_DELAY_SHORT );
        }
        vTaskDelay( DEMO_DELAY_LONG );
    }
    else
    {
        printf("场景3任务创建失败\n");
    }
    

    /******************** 演示结束处理 ********************/
    xDemoRunning = pdFALSE;  // 清除演示运行标志
    
    // 删除主控制任务自身（通过存储的句柄）
    vTaskDelete( MainControlTask_Handler ); 
}



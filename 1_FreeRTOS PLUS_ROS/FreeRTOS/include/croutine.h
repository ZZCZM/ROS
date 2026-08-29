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

#ifndef CO_ROUTINE_H
#define CO_ROUTINE_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h must appear in source files before include croutine.h"
#endif

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * 协程控制块及相关类型定义
 * 功能：定义FreeRTOS协程实现所需的核心数据结构及类型
 * 作用：
 *   1. 声明协程控制块(CRCB_t)结构，用于维护协程运行状态
 *   2. 定义协程函数原型及协程句柄类型
 *   3. 提供协程调度所需的链表项和优先级管理字段
 * 注意：该结构必须与任务控制块(TCB_t)保持uxPriority字段前的内存布局一致
 */

/* 协程句柄类型定义（不透明指针）
 * 作用：对外隐藏协程控制块的具体实现细节，仅通过句柄进行操作
 * 注意：实际结构体定义必须包含在头文件中以满足宏实现的依赖关系 */
typedef void * CoRoutineHandle_t;

/* 协程函数原型定义
 * 参数说明：
 *   CoRoutineHandle_t - 当前协程的句柄
 *   UBaseType_t - 初始化参数/状态标志
 * 功能：定义所有协程函数必须遵守的函数签名规范 */
typedef void (*crCOROUTINE_CODE)( CoRoutineHandle_t, UBaseType_t );

/* 协程控制块结构体
 * 作用：存储协程运行时的所有状态信息，用于调度器管理 */
typedef struct corCoRoutineControlBlock
{
    /* 协程入口函数指针
     * 说明：指向协程实际执行代码的函数地址
     * 调用方式：由调度器周期性调用 */
    crCOROUTINE_CODE  pxCoRoutineFunction;

    /* 通用链表项（双向链表节点）
     * 作用：用于将协程插入就绪队列或阻塞队列
     * 包含：指针域、时间排序值等调度信息 */
    ListItem_t        xGenericListItem;

    /* 事件链表项（专用链表节点）
     * 作用：用于事件驱动型协程的等待队列管理
     * 特点：通常按优先级排序，用于快速唤醒 */
    ListItem_t        xEventListItem;

    /* 协程优先级（0最低，configMAX_CO_ROUTINE_PRIORITIES-1最高）
     * 作用：决定协程在就绪队列中的调度顺序
     * 注意：必须与TCB_t结构中的优先级字段位置对齐 */
    UBaseType_t       uxPriority;

    /* 协程实例索引
     * 作用：当多个协程共用相同函数时，用于区分不同实例
     * 典型应用：协程池实现 */
    UBaseType_t       uxIndex;

    /* 协程状态标志
     * 内部使用：记录协程当前状态（运行/挂起/等待等）
     * 状态值定义：通常包含CR_READY, CR_RUNNING, CR_BLOCKED等 */
    uint16_t          uxState;
} CRCB_t; 

/* 结构体设计说明：
 * 1. 与TCB_t结构保持uxPriority前的内存布局一致，便于调度器统一处理
 * 2. 包含两个独立链表项实现多队列管理（通用队列+事件专用队列）
 * 3. 使用16位状态字段优化内存占用 */

/**
 * croutine. h
 *<pre>
 BaseType_t xCoRoutineCreate(
                                 crCOROUTINE_CODE pxCoRoutineCode,
                                 UBaseType_t uxPriority,
                                 UBaseType_t uxIndex
                               );</pre>
 *
 * Create a new co-routine and add it to the list of co-routines that are
 * ready to run.
 *
 * @param pxCoRoutineCode Pointer to the co-routine function.  Co-routine
 * functions require special syntax - see the co-routine section of the WEB
 * documentation for more information.
 *
 * @param uxPriority The priority with respect to other co-routines at which
 *  the co-routine will run.
 *
 * @param uxIndex Used to distinguish between different co-routines that
 * execute the same function.  See the example below and the co-routine section
 * of the WEB documentation for further information.
 *
 * @return pdPASS if the co-routine was successfully created and added to a ready
 * list, otherwise an error code defined with ProjDefs.h.
 *
 * Example usage:
   <pre>
 // Co-routine to be created.
 void vFlashCoRoutine( CoRoutineHandle_t xHandle, UBaseType_t uxIndex )
 {
 // Variables in co-routines must be declared static if they must maintain value across a blocking call.
 // This may not be necessary for const variables.
 static const char cLedToFlash[ 2 ] = { 5, 6 };
 static const TickType_t uxFlashRates[ 2 ] = { 200, 400 };

     // Must start every co-routine with a call to crSTART();
     crSTART( xHandle );

     for( ;; )
     {
         // This co-routine just delays for a fixed period, then toggles
         // an LED.  Two co-routines are created using this function, so
         // the uxIndex parameter is used to tell the co-routine which
         // LED to flash and how int32_t to delay.  This assumes xQueue has
         // already been created.
         vParTestToggleLED( cLedToFlash[ uxIndex ] );
         crDELAY( xHandle, uxFlashRates[ uxIndex ] );
     }

     // Must end every co-routine with a call to crEND();
     crEND();
 }

 // Function that creates two co-routines.
 void vOtherFunction( void )
 {
 uint8_t ucParameterToPass;
 TaskHandle_t xHandle;

     // Create two co-routines at priority 0.  The first is given index 0
     // so (from the code above) toggles LED 5 every 200 ticks.  The second
     // is given index 1 so toggles LED 6 every 400 ticks.
     for( uxIndex = 0; uxIndex < 2; uxIndex++ )
     {
         xCoRoutineCreate( vFlashCoRoutine, 0, uxIndex );
     }
 }
   </pre>
 * \defgroup xCoRoutineCreate xCoRoutineCreate
 * \ingroup Tasks
 */
/**
 * \ingroup Tasks
 * \defgroup xCoRoutineCreate xCoRoutineCreate
 * @brief 协程创建函数
 * 
 * 功能：在FreeRTOS系统中创建新的协程实例
 * 
 * 作用：
 * 1. 分配并初始化协程控制块(CRCB_t)
 * 2. 将新协程插入就绪队列
 * 3. 初始化协程栈空间（如果使用栈协程）
 * 
 * @param pxCoRoutineCode [in] 协程函数指针
 *        - 类型：crCOROUTINE_CODE（符合void (*)(CoRoutineHandle_t, UBaseType_t)签名）
 *        - 要求：必须持久存在的函数地址
 *        
 * @param uxPriority [in] 协程优先级
 *        - 范围：0（最低）~ (configMAX_CO_ROUTINE_PRIORITIES - 1)
 *        - 注意：不同协程可设相同优先级
 *        
 * @param uxIndex [in] 协程实例索引
 *        - 用途：当多个协程共用相同函数时，通过索引区分实例
 *        - 典型值：从0开始的序列号
 *        
 * @return 创建状态
 *        - pdPASS(1)：创建成功
 *        - pdFAIL(0)：创建失败（通常因内存不足）
 *        
 * 特殊说明：
 * 1. 协程创建后不会立即执行，需通过vCoRoutineSchedule调度
 * 2. 协程栈分配策略取决于具体FreeRTOS移植版本
 * 3. 非线程安全，应在调度器启动前或临界区内调用
 */
BaseType_t xCoRoutineCreate( 
    crCOROUTINE_CODE pxCoRoutineCode,
    UBaseType_t uxPriority,
    UBaseType_t uxIndex 
);


/**
 * croutine. h
 *<pre>
 void vCoRoutineSchedule( void );</pre>
 *
 * Run a co-routine.
 *
 * vCoRoutineSchedule() executes the highest priority co-routine that is able
 * to run.  The co-routine will execute until it either blocks, yields or is
 * preempted by a task.  Co-routines execute cooperatively so one
 * co-routine cannot be preempted by another, but can be preempted by a task.
 *
 * If an application comprises of both tasks and co-routines then
 * vCoRoutineSchedule should be called from the idle task (in an idle task
 * hook).
 *
 * Example usage:
   <pre>
 // This idle task hook will schedule a co-routine each time it is called.
 // The rest of the idle task will execute between co-routine calls.
 void vApplicationIdleHook( void )
 {
	vCoRoutineSchedule();
 }

 // Alternatively, if you do not require any other part of the idle task to
 // execute, the idle task hook can call vCoRoutineScheduler() within an
 // infinite loop.
 void vApplicationIdleHook( void )
 {
    for( ;; )
    {
        vCoRoutineSchedule();
    }
 }
 </pre>
 * \defgroup vCoRoutineSchedule vCoRoutineSchedule
 * \ingroup Tasks
 */
/**
 * \ingroup Tasks
 * \defgroup vCoRoutineSchedule vCoRoutineSchedule
 * @brief 协程调度器核心函数
 * 
 * 功能：执行协程调度算法，管理协程的上下文切换
 * 
 * 作用：
 * 1. 遍历就绪协程队列，选择最高优先级协程
 * 2. 执行协程上下文切换（保存/恢复协程栈）
 * 3. 处理协程状态迁移（就绪->运行->阻塞->删除）
 * 
 * 工作流程：
 * 1. 检查就绪队列中的协程优先级
 * 2. 通过优先级位图算法快速定位最高优先级协程
 * 3. 执行协程函数直至遇到crDELAY或crQUEUE_SEND等阻塞操作
 * 4. 维护时间片计数（如果启用时间片轮转）
 * 
 * 调用要求：
 * - 必须在全局中断使能状态下调用
 * - 通常由空闲任务或专用调度任务循环调用
 * - 禁止在中断服务例程(ISR)中调用
 * 
 * 特殊说明：
 * 1. 采用协作式调度策略，协程必须主动让出CPU
 * 2. 实际上下文切换通过setjmp/longjmp实现（依赖具体移植）
 * 3. 与任务调度器互斥运行，同一时刻只能激活一种调度模式
 * 
 * 配置依赖：
 * - configUSE_CO_ROUTINES 必须设置为1
 * - configMAX_CO_ROUTINE_PRIORITIES 决定优先级数量
 */
void vCoRoutineSchedule( void );


/**
 * croutine. h
 * <pre>
 crSTART( CoRoutineHandle_t xHandle );</pre>
 *
 * This macro MUST always be called at the start of a co-routine function.
 *
 * Example usage:
   <pre>
 // Co-routine to be created.
 void vACoRoutine( CoRoutineHandle_t xHandle, UBaseType_t uxIndex )
 {
 // Variables in co-routines must be declared static if they must maintain value across a blocking call.
 static int32_t ulAVariable;

     // Must start every co-routine with a call to crSTART();
     crSTART( xHandle );

     for( ;; )
     {
          // Co-routine functionality goes here.
     }

     // Must end every co-routine with a call to crEND();
     crEND();
 }</pre>
 * \defgroup crSTART crSTART
 * \ingroup Tasks
 */
/**
 * \ingroup Tasks
 * \defgroup crSTART crSTART
 * @brief 协程入口宏（状态机初始化）
 * 
 * 功能：实现协程控制流的上下文保存与恢复机制
 * 
 * 作用：
 * 1. 创建协程状态机框架
 * 2. 根据uxState值跳转到上次挂起的位置
 * 3. 实现协程的初始启动和恢复执行
 * 
 * 实现原理：
 * 使用switch-case结构配合uxState状态变量：
 * - 首次调用时uxState=0，执行case 0后的代码
 * - 协程挂起时uxState被设置为非零值，恢复时跳转到对应case
 * 
 * @param pxCRCB [in] 协程控制块指针
 *        - 类型：CoRoutineHandle_t（实际转换为CRCB_t*）
 *        - 要求：必须指向有效已初始化的CRCB结构
 * 
 * 技术细节：
 * 1. 基于Duff's device原理实现轻量级上下文切换
 * 2. uxState字段作为程序位置计数器(Program Counter)
 * 3. 与crEND宏形成代码块作用域
 * 
 * 使用约束：
 * 1. 必须作为协程函数的第一个语句
 * 2. 必须与crEND宏配对使用
 * 3. 禁止在普通任务函数中使用
 * 
 * 典型用法：
 * void vCoroutine(CoRoutineHandle_t xHandle, UBaseType_t uxIndex) {
 *     crSTART(xHandle);
 *     for(;;) {
 *         // 协程主体代码
 *         crDELAY(xHandle, 100);
 *     }
 *     crEND();
 * }
 */
#define crSTART( pxCRCB ) switch( ( ( CRCB_t * )( pxCRCB ) )->uxState ) { case 0:
/* 代码展开说明：
 * 1. (pxCRCB) 强制转型为CRCB_t指针，访问状态字段
 * 2. switch结构创建跳转表，case 0对应初始状态
 * 3. 后续代码将通过修改uxState值实现位置记忆 */

/**
 * croutine. h
 * <pre>
 crEND();</pre>
 *
 * This macro MUST always be called at the end of a co-routine function.
 *
 * Example usage:
   <pre>
 // Co-routine to be created.
 void vACoRoutine( CoRoutineHandle_t xHandle, UBaseType_t uxIndex )
 {
 // Variables in co-routines must be declared static if they must maintain value across a blocking call.
 static int32_t ulAVariable;

     // Must start every co-routine with a call to crSTART();
     crSTART( xHandle );

     for( ;; )
     {
          // Co-routine functionality goes here.
     }

     // Must end every co-routine with a call to crEND();
     crEND();
 }</pre>
 * \defgroup crSTART crSTART
 * \ingroup Tasks
 */
#define crEND() }

/*
 * These macros are intended for internal use by the co-routine implementation
 * only.  The macros should not be used directly by application writers.
 */
/**
 * @brief 协程状态设置宏（内部实现级）
 * 
 * 功能：实现协程挂起点的状态标记与上下文保存
 * 作用：为协程中的每个挂起点生成唯一状态标识，用于恢复执行定位
 *
 * 注意：这些宏仅供协程内核实现使用，禁止在应用层直接调用
 */

/* 双状态点生成策略：
 * 使用__LINE__*2 和 __LINE__*2+1 实现同一代码行的两个独立状态点
 * 优点：
 * 1. 避免宏展开时的行号冲突
 * 2. 支持单行代码内的多个挂起点
 */
#define crSET_STATE0( xHandle ) \
    ( ( CRCB_t * )( xHandle ) )->uxState = (__LINE__ * 2); /* 设置偶数状态值 */ \
    return; /* 退出协程函数，控制权返回调度器 */ \
    case (__LINE__ * 2): /* 创建恢复点标签 */

#define crSET_STATE1( xHandle ) \
    ( ( CRCB_t * )( xHandle ) )->uxState = ((__LINE__ * 2)+1); /* 设置奇数状态值 */ \
    return; \
    case ((__LINE__ * 2)+1):

/* 技术细节说明：
 * 1. __LINE__宏展开：在预处理阶段替换为当前行号
 * 2. 状态值算法：通过乘2操作确保相邻行状态值间隔≥2
 * 3. return机制：协程函数在此退出，等待下次调度时从case标签恢复
 * 
 * 典型应用场景：
 * 当协程需要在同一行代码中实现多个等待条件时：
 * crDELAY(xHandle,10); crQUEUE_RECEIVE(...); 
 * 通过crSET_STATE0和crSET_STATE1区分不同操作的状态
 * 
 * 重要约束：
 * 1. 必须与crSTART/crEND宏配合使用
 * 2. 宏参数xHandle必须为有效协程句柄
 * 3. 禁止修改生成的uxState值
 * 
 * 调试注意：
 * 修改源代码行号将导致状态标识变化，可能引发协程状态混乱
 */

/**
 * croutine. h
 *<pre>
 crDELAY( CoRoutineHandle_t xHandle, TickType_t xTicksToDelay );</pre>
 *
 * Delay a co-routine for a fixed period of time.
 *
 * crDELAY can only be called from the co-routine function itself - not
 * from within a function called by the co-routine function.  This is because
 * co-routines do not maintain their own stack.
 *
 * @param xHandle The handle of the co-routine to delay.  This is the xHandle
 * parameter of the co-routine function.
 *
 * @param xTickToDelay The number of ticks that the co-routine should delay
 * for.  The actual amount of time this equates to is defined by
 * configTICK_RATE_HZ (set in FreeRTOSConfig.h).  The constant portTICK_PERIOD_MS
 * can be used to convert ticks to milliseconds.
 *
 * Example usage:
   <pre>
 // Co-routine to be created.
 void vACoRoutine( CoRoutineHandle_t xHandle, UBaseType_t uxIndex )
 {
 // Variables in co-routines must be declared static if they must maintain value across a blocking call.
 // This may not be necessary for const variables.
 // We are to delay for 200ms.
 static const xTickType xDelayTime = 200 / portTICK_PERIOD_MS;

     // Must start every co-routine with a call to crSTART();
     crSTART( xHandle );

     for( ;; )
     {
        // Delay for 200ms.
        crDELAY( xHandle, xDelayTime );

        // Do something here.
     }

     // Must end every co-routine with a call to crEND();
     crEND();
 }</pre>
 * \defgroup crDELAY crDELAY
 * \ingroup Tasks
 */
/**
 * \ingroup Tasks
 * \defgroup crDELAY crDELAY
 * @brief 协程延时宏（协作式延时）
 * 
 * 功能：实现协程的定时阻塞，主动让出CPU控制权
 * 
 * 作用：
 * 1. 将当前协程插入延时队列
 * 2. 更新协程状态为阻塞态
 * 3. 触发协程调度器切换
 * 
 * @param xHandle [in] 协程控制块句柄
 *        - 要求：必须来自crSTART宏的当前协程句柄
 *        
 * @param xTicksToDelay [in] 延时时间基数
 *        - 单位：系统节拍周期(configTICK_RATE_HZ)
 *        - 特殊值：
 *          pdMS_TO_TICKS(ms) 可将毫秒转换为节拍数
 *          0表示立即让出CPU但不进入延时队列
 * 
 * 实现流程：
 * if(延时时间>0):
 *   1. 调用vCoRoutineAddToDelayedList加入延时队列
 *   2. 通过crSET_STATE0保存当前执行位置
 * else:
 *   直接触发协程切换
 * 
 * 典型应用：
 * void vCoroutine(CoRoutineHandle_t xHandle, UBaseType_t uxIndex) {
 *     crSTART(xHandle);
 *     for(;;) {
 *         // 执行操作
 *         crDELAY(xHandle, pdMS_TO_TICKS(100)); // 每100ms执行一次
 *     }
 *     crEND();
 * }
 * 
 * 重要约束：
 * 1. 必须在协程函数内部调用
 * 2. 延时精度受系统节拍影响（最小1个tick）
 * 3. 禁止在临界区或中断服务程序中使用
 * 
 * 设计说明：
 * 与任务延时vTaskDelay的区别：
 * 1. 采用协作式而非抢占式调度
 * 2. 延时期间协程栈会被保留
 * 3. 不需要维护任务TCB的额外开销
 */
#define crDELAY( xHandle, xTicksToDelay )              \
    if( ( xTicksToDelay ) > 0 )                        \
    {                                                   \
        /* 将协程插入延时队列，参数说明：               \
         * xTicksToDelay：延时基准时间                \
         * NULL：不绑定事件对象 */                    \
        vCoRoutineAddToDelayedList( ( xTicksToDelay ), NULL ); \
    }                                                   \
    /* 设置协程状态并跳转回调度器：                   \
     * 1. 记录当前代码位置到uxState                  \
     * 2. 通过return退出当前协程函数 */              \
    crSET_STATE0( ( xHandle ) );

/* 技术细节：
 * 1. 延时精度：实际延时时间为 xTicksToDelay * tick周期 + 调度延迟
 * 2. 使用NULL参数：表示纯时间等待，与事件等待队列区分
 * 3. 宏展开后的代码流：
 *    - 首次执行：执行case 0后的代码，调用crDELAY
 *    - 恢复执行：根据uxState跳转到对应的case标签
 */

/**
 * <pre>
 crQUEUE_SEND(
                  CoRoutineHandle_t xHandle,
                  QueueHandle_t pxQueue,
                  void *pvItemToQueue,
                  TickType_t xTicksToWait,
                  BaseType_t *pxResult
             )</pre>
 *
 * The macro's crQUEUE_SEND() and crQUEUE_RECEIVE() are the co-routine
 * equivalent to the xQueueSend() and xQueueReceive() functions used by tasks.
 *
 * crQUEUE_SEND and crQUEUE_RECEIVE can only be used from a co-routine whereas
 * xQueueSend() and xQueueReceive() can only be used from tasks.
 *
 * crQUEUE_SEND can only be called from the co-routine function itself - not
 * from within a function called by the co-routine function.  This is because
 * co-routines do not maintain their own stack.
 *
 * See the co-routine section of the WEB documentation for information on
 * passing data between tasks and co-routines and between ISR's and
 * co-routines.
 *
 * @param xHandle The handle of the calling co-routine.  This is the xHandle
 * parameter of the co-routine function.
 *
 * @param pxQueue The handle of the queue on which the data will be posted.
 * The handle is obtained as the return value when the queue is created using
 * the xQueueCreate() API function.
 *
 * @param pvItemToQueue A pointer to the data being posted onto the queue.
 * The number of bytes of each queued item is specified when the queue is
 * created.  This number of bytes is copied from pvItemToQueue into the queue
 * itself.
 *
 * @param xTickToDelay The number of ticks that the co-routine should block
 * to wait for space to become available on the queue, should space not be
 * available immediately. The actual amount of time this equates to is defined
 * by configTICK_RATE_HZ (set in FreeRTOSConfig.h).  The constant
 * portTICK_PERIOD_MS can be used to convert ticks to milliseconds (see example
 * below).
 *
 * @param pxResult The variable pointed to by pxResult will be set to pdPASS if
 * data was successfully posted onto the queue, otherwise it will be set to an
 * error defined within ProjDefs.h.
 *
 * Example usage:
   <pre>
 // Co-routine function that blocks for a fixed period then posts a number onto
 // a queue.
 static void prvCoRoutineFlashTask( CoRoutineHandle_t xHandle, UBaseType_t uxIndex )
 {
 // Variables in co-routines must be declared static if they must maintain value across a blocking call.
 static BaseType_t xNumberToPost = 0;
 static BaseType_t xResult;

    // Co-routines must begin with a call to crSTART().
    crSTART( xHandle );

    for( ;; )
    {
        // This assumes the queue has already been created.
        crQUEUE_SEND( xHandle, xCoRoutineQueue, &xNumberToPost, NO_DELAY, &xResult );

        if( xResult != pdPASS )
        {
            // The message was not posted!
        }

        // Increment the number to be posted onto the queue.
        xNumberToPost++;

        // Delay for 100 ticks.
        crDELAY( xHandle, 100 );
    }

    // Co-routines must end with a call to crEND().
    crEND();
 }</pre>
 * \defgroup crQUEUE_SEND crQUEUE_SEND
 * \ingroup Tasks
 */
/**
 * \ingroup Tasks
 * \defgroup crQUEUE_SEND crQUEUE_SEND
 * @brief 协程队列发送宏（带阻塞机制）
 * 
 * 功能：实现协程安全的队列发送操作，支持超时等待
 * 
 * 作用：
 * 1. 尝试向队列发送数据
 * 2. 队列满时阻塞协程直至超时或空间可用
 * 3. 处理队列操作中的调度器触发请求
 * 
 * @param xHandle [in] 协程控制块句柄
 *        - 要求：必须来自crSTART宏的当前协程句柄
 * @param pxQueue [in] 目标队列句柄
 *        - 类型：QueueHandle_t
 * @param pvItemToQueue [in] 待发送数据指针
 *        - 允许NULL（用于信号通知）
 * @param xTicksToWait [in] 最大等待时间
 *        - 单位：系统节拍(configTICK_RATE_HZ)
 *        - 特殊值：portMAX_DELAY表示无限等待
 * @param pxResult [out] 操作结果指针
 *        - 接收值：pdPASS(成功)/errQUEUE_FULL(失败)/errQUEUE_YIELD(触发调度)
 * 
 * 实现流程：
 * 1. 首次尝试发送（允许等待）
 * 2. 若队列阻塞(errQUEUE_BLOCKED):
 *    - 保存当前状态(crSET_STATE0)
 *    - 立即重试发送（不等待）
 * 3. 若触发调度请求(errQUEUE_YIELD):
 *    - 保存状态(crSET_STATE1)
 *    - 强制返回成功状态
 * 
 * 典型应用：
 * crQUEUE_SEND(xHandle, pxQueue, &data, pdMS_TO_TICKS(100), &xResult);
 * if(xResult == pdPASS) {  发送成功处理  }
 * 
 * 设计特点：
 * 1. 双重状态点设计（crSET_STATE0/1）支持不同等待场景
 * 2. 与vCoRoutineAddToQueueWaitList配合实现事件驱动
 * 3. 自动处理队列满->阻塞->就绪的状态转换
 * 
 * 重要约束：
 * 1. 必须在协程函数内部调用
 * 2. 队列必须使用xQueueCreate创建
 * 3. 禁止在中断服务程序中使用
 * 
 * 与任务版队列发送的差异：
 * 1. 支持协程特有的状态保存机制
 * 2. 自动处理调度器触发请求
 * 3. 使用协程专用队列API（xQueueCRSend）
 */
#define crQUEUE_SEND( xHandle, pxQueue, pvItemToQueue, xTicksToWait, pxResult )    \
{                                                                                   \
    /* 首次尝试队列发送（允许等待） */                                              \
    *( pxResult ) = xQueueCRSend( ( pxQueue ), ( pvItemToQueue ), ( xTicksToWait ) ); \
    if( *( pxResult ) == errQUEUE_BLOCKED )                                         \
    {                                                                               \
        /* 进入阻塞态并保存执行位置 */                                              \
        crSET_STATE0( ( xHandle ) );                                                \
        /* 立即重试发送（不等待） */                                                \
        *pxResult = xQueueCRSend( ( pxQueue ), ( pvItemToQueue ), 0 );              \
    }                                                                               \
    if( *pxResult == errQUEUE_YIELD )                                               \
    {                                                                               \
        /* 处理调度请求并保存新状态点 */                                            \
        crSET_STATE1( ( xHandle ) );                                                \
        *pxResult = pdPASS;                                                         \
    }                                                                               \
}

/* 状态转换说明：
 * [运行态] -> crQUEUE_SEND -> [阻塞态] -(队列有空间)-> [就绪态] -> [运行态]
 *                         \_(超时/失败)-> [就绪态] -> [运行态]
 * 
 * 技术细节：
 * 1. xQueueCRSend：协程专用队列发送API，返回特殊错误码
 * 2. errQUEUE_BLOCKED：表示协程已被加入队列等待列表
 * 3. errQUEUE_YIELD：表示发送过程中触发了调度请求
 * 
 * 调试注意：
 * 当使用portMAX_DELAY时，需确保队列消费者协程能及时处理数据
 */


/**
 * croutine. h
 * <pre>
  crQUEUE_RECEIVE(
                     CoRoutineHandle_t xHandle,
                     QueueHandle_t pxQueue,
                     void *pvBuffer,
                     TickType_t xTicksToWait,
                     BaseType_t *pxResult
                 )</pre>
 *
 * The macro's crQUEUE_SEND() and crQUEUE_RECEIVE() are the co-routine
 * equivalent to the xQueueSend() and xQueueReceive() functions used by tasks.
 *
 * crQUEUE_SEND and crQUEUE_RECEIVE can only be used from a co-routine whereas
 * xQueueSend() and xQueueReceive() can only be used from tasks.
 *
 * crQUEUE_RECEIVE can only be called from the co-routine function itself - not
 * from within a function called by the co-routine function.  This is because
 * co-routines do not maintain their own stack.
 *
 * See the co-routine section of the WEB documentation for information on
 * passing data between tasks and co-routines and between ISR's and
 * co-routines.
 *
 * @param xHandle The handle of the calling co-routine.  This is the xHandle
 * parameter of the co-routine function.
 *
 * @param pxQueue The handle of the queue from which the data will be received.
 * The handle is obtained as the return value when the queue is created using
 * the xQueueCreate() API function.
 *
 * @param pvBuffer The buffer into which the received item is to be copied.
 * The number of bytes of each queued item is specified when the queue is
 * created.  This number of bytes is copied into pvBuffer.
 *
 * @param xTickToDelay The number of ticks that the co-routine should block
 * to wait for data to become available from the queue, should data not be
 * available immediately. The actual amount of time this equates to is defined
 * by configTICK_RATE_HZ (set in FreeRTOSConfig.h).  The constant
 * portTICK_PERIOD_MS can be used to convert ticks to milliseconds (see the
 * crQUEUE_SEND example).
 *
 * @param pxResult The variable pointed to by pxResult will be set to pdPASS if
 * data was successfully retrieved from the queue, otherwise it will be set to
 * an error code as defined within ProjDefs.h.
 *
 * Example usage:
 <pre>
 // A co-routine receives the number of an LED to flash from a queue.  It
 // blocks on the queue until the number is received.
 static void prvCoRoutineFlashWorkTask( CoRoutineHandle_t xHandle, UBaseType_t uxIndex )
 {
 // Variables in co-routines must be declared static if they must maintain value across a blocking call.
 static BaseType_t xResult;
 static UBaseType_t uxLEDToFlash;

    // All co-routines must start with a call to crSTART().
    crSTART( xHandle );

    for( ;; )
    {
        // Wait for data to become available on the queue.
        crQUEUE_RECEIVE( xHandle, xCoRoutineQueue, &uxLEDToFlash, portMAX_DELAY, &xResult );

        if( xResult == pdPASS )
        {
            // We received the LED to flash - flash it!
            vParTestToggleLED( uxLEDToFlash );
        }
    }

    crEND();
 }</pre>
 * \defgroup crQUEUE_RECEIVE crQUEUE_RECEIVE
 * \ingroup Tasks
 */
/**
 * \ingroup Tasks
 * \defgroup crQUEUE_RECEIVE crQUEUE_RECEIVE
 * @brief 协程队列接收宏（带阻塞机制）
 * 
 * 功能：实现协程安全的队列接收操作，支持超时等待
 * 
 * 作用：
 * 1. 尝试从队列获取数据
 * 2. 队列空时阻塞协程直至超时或数据到达
 * 3. 处理队列操作中的调度器触发请求
 * 
 * @param xHandle [in] 协程控制块句柄
 *        - 要求：必须来自crSTART宏的当前协程句柄
 * @param pxQueue [in] 目标队列句柄
 *        - 类型：QueueHandle_t
 * @param pvBuffer [out] 数据接收缓冲区指针
 *        - 必须指向足够容纳队列项的内存
 * @param xTicksToWait [in] 最大等待时间
 *        - 单位：系统节拍(configTICK_RATE_HZ)
 *        - 特殊值：portMAX_DELAY表示无限等待
 * @param pxResult [out] 操作结果指针
 *        - 接收值：pdPASS(成功)/errQUEUE_EMPTY(失败)/errQUEUE_YIELD(触发调度)
 * 
 * 实现流程：
 * 1. 首次尝试接收（允许等待）
 * 2. 若队列阻塞(errQUEUE_BLOCKED):
 *    - 保存当前状态(crSET_STATE0)
 *    - 立即重试接收（不等待）
 * 3. 若触发调度请求(errQUEUE_YIELD):
 *    - 保存状态(crSET_STATE1)
 *    - 强制返回成功状态
 * 
 * 典型应用：
 * crQUEUE_RECEIVE(xHandle, pxQueue, &data, pdMS_TO_TICKS(100), &xResult);
 * if(xResult == pdPASS) { /数据处理逻辑 }
 * 
 * 设计特点：
 * 1. 双重状态点设计支持接收失败后的快速重试
 * 2. 与vCoRoutineAddToQueueWaitList配合实现事件驱动
 * 3. 自动处理队列空->阻塞->就绪的状态转换
 * 
 * 重要约束：
 * 1. 缓冲区大小必须匹配队列项尺寸
 * 2. 队列必须使用xQueueCreate创建
 * 3. 禁止在中断服务程序中使用
 * 
 * 与任务版队列接收的差异：
 * 1. 使用协程专用接收API（xQueueCRReceive）
 * 2. 自动维护协程阻塞链表
 * 3. 支持协作式调度触发
 */
#define crQUEUE_RECEIVE( xHandle, pxQueue, pvBuffer, xTicksToWait, pxResult )      \
{                                                                                   \
    /* 首次尝试队列接收（允许等待） */                                              \
    *( pxResult ) = xQueueCRReceive( ( pxQueue ), ( pvBuffer ), ( xTicksToWait ) ); \
    if( *( pxResult ) == errQUEUE_BLOCKED )                                         \
    {                                                                               \
        /* 进入阻塞态并保存执行位置 */                                              \
        crSET_STATE0( ( xHandle ) );                                                \
        /* 立即重试接收（不等待） */                                                \
        *( pxResult ) = xQueueCRReceive( ( pxQueue ), ( pvBuffer ), 0 );            \
    }                                                                               \
    if( *( pxResult ) == errQUEUE_YIELD )                                           \
    {                                                                               \
        /* 处理调度请求并保存新状态点 */                                            \
        crSET_STATE1( ( xHandle ) );                                                \
        *( pxResult ) = pdPASS;                                                     \
    }                                                                               \
}

/* 状态转换说明：
 * [运行态] -> crQUEUE_RECEIVE -> [阻塞态] -(数据到达)-> [就绪态] -> [运行态]
 *                          \_(超时/失败)-> [就绪态] -> [运行态]
 * 
 * 技术细节：
 * 1. xQueueCRReceive：协程专用接收API，返回扩展错误码
 * 2. 数据拷贝机制：与任务队列相同的memcpy方式
 * 3. 零等待重试：避免重复加入等待列表
 * 
 * 调试技巧：
 * 当接收持续失败时，检查生产者协程的发送频率和队列容量
 */

/**
 * croutine. h
 * <pre>
  crQUEUE_SEND_FROM_ISR(
                            QueueHandle_t pxQueue,
                            void *pvItemToQueue,
                            BaseType_t xCoRoutinePreviouslyWoken
                       )</pre>
 *
 * The macro's crQUEUE_SEND_FROM_ISR() and crQUEUE_RECEIVE_FROM_ISR() are the
 * co-routine equivalent to the xQueueSendFromISR() and xQueueReceiveFromISR()
 * functions used by tasks.
 *
 * crQUEUE_SEND_FROM_ISR() and crQUEUE_RECEIVE_FROM_ISR() can only be used to
 * pass data between a co-routine and and ISR, whereas xQueueSendFromISR() and
 * xQueueReceiveFromISR() can only be used to pass data between a task and and
 * ISR.
 *
 * crQUEUE_SEND_FROM_ISR can only be called from an ISR to send data to a queue
 * that is being used from within a co-routine.
 *
 * See the co-routine section of the WEB documentation for information on
 * passing data between tasks and co-routines and between ISR's and
 * co-routines.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param xCoRoutinePreviouslyWoken This is included so an ISR can post onto
 * the same queue multiple times from a single interrupt.  The first call
 * should always pass in pdFALSE.  Subsequent calls should pass in
 * the value returned from the previous call.
 *
 * @return pdTRUE if a co-routine was woken by posting onto the queue.  This is
 * used by the ISR to determine if a context switch may be required following
 * the ISR.
 *
 * Example usage:
 <pre>
 // A co-routine that blocks on a queue waiting for characters to be received.
 static void vReceivingCoRoutine( CoRoutineHandle_t xHandle, UBaseType_t uxIndex )
 {
 char cRxedChar;
 BaseType_t xResult;

     // All co-routines must start with a call to crSTART().
     crSTART( xHandle );

     for( ;; )
     {
         // Wait for data to become available on the queue.  This assumes the
         // queue xCommsRxQueue has already been created!
         crQUEUE_RECEIVE( xHandle, xCommsRxQueue, &uxLEDToFlash, portMAX_DELAY, &xResult );

         // Was a character received?
         if( xResult == pdPASS )
         {
             // Process the character here.
         }
     }

     // All co-routines must end with a call to crEND().
     crEND();
 }

 // An ISR that uses a queue to send characters received on a serial port to
 // a co-routine.
 void vUART_ISR( void )
 {
 char cRxedChar;
 BaseType_t xCRWokenByPost = pdFALSE;

     // We loop around reading characters until there are none left in the UART.
     while( UART_RX_REG_NOT_EMPTY() )
     {
         // Obtain the character from the UART.
         cRxedChar = UART_RX_REG;

         // Post the character onto a queue.  xCRWokenByPost will be pdFALSE
         // the first time around the loop.  If the post causes a co-routine
         // to be woken (unblocked) then xCRWokenByPost will be set to pdTRUE.
         // In this manner we can ensure that if more than one co-routine is
         // blocked on the queue only one is woken by this ISR no matter how
         // many characters are posted to the queue.
         xCRWokenByPost = crQUEUE_SEND_FROM_ISR( xCommsRxQueue, &cRxedChar, xCRWokenByPost );
     }
 }</pre>
 * \defgroup crQUEUE_SEND_FROM_ISR crQUEUE_SEND_FROM_ISR
 * \ingroup Tasks
 */
/**
 * \ingroup Tasks
 * \defgroup crQUEUE_SEND_FROM_ISR crQUEUE_SEND_FROM_ISR
 * @brief 中断上下文队列发送宏（协程唤醒）
 * 
 * 功能：在中断服务例程(ISR)中实现协程队列发送，支持协程唤醒
 * 
 * 作用：
 * 1. 中断安全地发送数据到队列
 * 2. 唤醒等待该队列的阻塞协程
 * 3. 返回协程唤醒状态用于后续调度决策
 * 
 * @param pxQueue [in] 目标队列句柄
 *        - 要求：必须使用xQueueCreate创建
 * @param pvItemToQueue [in] 待发送数据指针
 *        - 允许NULL（用于信号通知）
 * @param xCoRoutinePreviouslyWoken [in] 先前唤醒状态
 *        - 输入值：pdFALSE(初始)/pdTRUE(已唤醒)
 *        - 返回值：pdTRUE表示有协程需要调度
 * 
 * 实现原理：
 * 直接调用协程专用的中断安全发送API：
 * xQueueCRSendFromISR -> 队列操作 + 协程等待列表检查
 * 
 * 典型应用：
 * BaseType_t xYieldRequired = pdFALSE;
 * crQUEUE_SEND_FROM_ISR(pxQueue, &data, xYieldRequired);
 * portYIELD_FROM_ISR( xYieldRequired );
 * 
 * 设计特点：
 * 1. 无阻塞机制：ISR中禁止等待
 * 2. 轻量级唤醒检查：通过优先级位图快速定位等待协程
 * 3. 线程安全：全程关闭中断保护
 * 
 * 重要约束：
 * 1. 必须在中段服务程序(ISR)内调用
 * 2. 必须使用以"FromISR"结尾的API配套使用
 * 3. 禁止在普通协程函数中使用
 * 
 * 与任务版中断发送的区别：
 * 1. 专门处理协程等待列表
 * 2. 返回的唤醒标志针对协程调度器
 * 3. 使用协程专用的事件列表项
 */
#define crQUEUE_SEND_FROM_ISR( pxQueue, pvItemToQueue, xCoRoutinePreviouslyWoken ) \
    xQueueCRSendFromISR( ( pxQueue ), ( pvItemToQueue ), ( xCoRoutinePreviouslyWoken ) )

/* 技术细节说明：
 * 1. xQueueCRSendFromISR内部流程：
 *    a) 尝试直接发送数据到队列
 *    b) 如果失败，检查协程等待列表
 *    c) 唤醒最高优先级的等待协程
 *    d) 更新xCoRoutinePreviouslyWoken状态
 * 
 * 2. 唤醒机制：
 *    - 将协程从队列等待列表移到就绪列表
 *    - 设置协程状态为CR_READY
 *    - 更新调度器优先级位图
 * 
 * 3. 性能优化：
 *    通过uxPriority字段快速定位最高优先级协程
 *    避免遍历整个等待链表
 */


/**
 * croutine. h
 * <pre>
  crQUEUE_SEND_FROM_ISR(
                            QueueHandle_t pxQueue,
                            void *pvBuffer,
                            BaseType_t * pxCoRoutineWoken
                       )</pre>
 *
 * The macro's crQUEUE_SEND_FROM_ISR() and crQUEUE_RECEIVE_FROM_ISR() are the
 * co-routine equivalent to the xQueueSendFromISR() and xQueueReceiveFromISR()
 * functions used by tasks.
 *
 * crQUEUE_SEND_FROM_ISR() and crQUEUE_RECEIVE_FROM_ISR() can only be used to
 * pass data between a co-routine and and ISR, whereas xQueueSendFromISR() and
 * xQueueReceiveFromISR() can only be used to pass data between a task and and
 * ISR.
 *
 * crQUEUE_RECEIVE_FROM_ISR can only be called from an ISR to receive data
 * from a queue that is being used from within a co-routine (a co-routine
 * posted to the queue).
 *
 * See the co-routine section of the WEB documentation for information on
 * passing data between tasks and co-routines and between ISR's and
 * co-routines.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvBuffer A pointer to a buffer into which the received item will be
 * placed.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from the queue into
 * pvBuffer.
 *
 * @param pxCoRoutineWoken A co-routine may be blocked waiting for space to become
 * available on the queue.  If crQUEUE_RECEIVE_FROM_ISR causes such a
 * co-routine to unblock *pxCoRoutineWoken will get set to pdTRUE, otherwise
 * *pxCoRoutineWoken will remain unchanged.
 *
 * @return pdTRUE an item was successfully received from the queue, otherwise
 * pdFALSE.
 *
 * Example usage:
 <pre>
 // A co-routine that posts a character to a queue then blocks for a fixed
 // period.  The character is incremented each time.
 static void vSendingCoRoutine( CoRoutineHandle_t xHandle, UBaseType_t uxIndex )
 {
 // cChar holds its value while this co-routine is blocked and must therefore
 // be declared static.
 static char cCharToTx = 'a';
 BaseType_t xResult;

     // All co-routines must start with a call to crSTART().
     crSTART( xHandle );

     for( ;; )
     {
         // Send the next character to the queue.
         crQUEUE_SEND( xHandle, xCoRoutineQueue, &cCharToTx, NO_DELAY, &xResult );

         if( xResult == pdPASS )
         {
             // The character was successfully posted to the queue.
         }
		 else
		 {
			// Could not post the character to the queue.
		 }

         // Enable the UART Tx interrupt to cause an interrupt in this
		 // hypothetical UART.  The interrupt will obtain the character
		 // from the queue and send it.
		 ENABLE_RX_INTERRUPT();

		 // Increment to the next character then block for a fixed period.
		 // cCharToTx will maintain its value across the delay as it is
		 // declared static.
		 cCharToTx++;
		 if( cCharToTx > 'x' )
		 {
			cCharToTx = 'a';
		 }
		 crDELAY( 100 );
     }

     // All co-routines must end with a call to crEND().
     crEND();
 }

 // An ISR that uses a queue to receive characters to send on a UART.
 void vUART_ISR( void )
 {
 char cCharToTx;
 BaseType_t xCRWokenByPost = pdFALSE;

     while( UART_TX_REG_EMPTY() )
     {
         // Are there any characters in the queue waiting to be sent?
		 // xCRWokenByPost will automatically be set to pdTRUE if a co-routine
		 // is woken by the post - ensuring that only a single co-routine is
		 // woken no matter how many times we go around this loop.
         if( crQUEUE_RECEIVE_FROM_ISR( pxQueue, &cCharToTx, &xCRWokenByPost ) )
		 {
			 SEND_CHARACTER( cCharToTx );
		 }
     }
 }</pre>
 * \defgroup crQUEUE_RECEIVE_FROM_ISR crQUEUE_RECEIVE_FROM_ISR
 * \ingroup Tasks
 */
/**
 * \ingroup Tasks
 * \defgroup crQUEUE_RECEIVE_FROM_ISR crQUEUE_RECEIVE_FROM_ISR
 * @brief 中断上下文队列接收宏（协程唤醒）
 * 
 * 功能：在中断服务例程(ISR)中实现协程队列接收，支持协程唤醒
 * 
 * 作用：
 * 1. 中断安全地从队列获取数据
 * 2. 唤醒等待该队列的生产者协程
 * 3. 返回协程唤醒状态用于后续调度决策
 * 
 * @param pxQueue [in] 目标队列句柄
 *        - 要求：必须使用xQueueCreate创建
 * @param pvBuffer [out] 数据接收缓冲区指针
 *        - 必须指向有效内存区域
 * @param pxCoRoutineWoken [in/out] 协程唤醒标志指针
 *        - 输入值：pdFALSE(初始)/pdTRUE(已唤醒)
 *        - 返回值：pdTRUE表示需要触发调度
 * 
 * 实现原理：
 * 直接调用协程专用的中断安全接收API：
 * xQueueCRReceiveFromISR -> 队列操作 + 协程等待列表检查
 * 
 * 典型应用：
 * BaseType_t xYieldRequired = pdFALSE;
 * crQUEUE_RECEIVE_FROM_ISR(pxQueue, &data, &xYieldRequired);
 * portYIELD_FROM_ISR( xYieldRequired );
 * 
 * 设计特点：
 * 1. 无阻塞机制：符合ISR即时响应要求
 * 2. 优先级继承：唤醒最高优先级等待协程
 * 3. 内存安全：使用临界区保护数据访问
 */
#define crQUEUE_RECEIVE_FROM_ISR( pxQueue, pvBuffer, pxCoRoutineWoken ) \
    xQueueCRReceiveFromISR( ( pxQueue ), ( pvBuffer ), ( pxCoRoutineWoken ) )

/**
 * @brief 协程延时列表管理函数（内部API）
 * 
 * 功能：将当前协程移出就绪队列，插入延时/事件队列
 * 
 * 作用：
 * 1. 管理协程的时间阻塞（crDELAY类操作）
 * 2. 处理事件驱动阻塞（crQUEUE_SEND/RECEIVE类操作）
 * 
 * @param xTicksToDelay [in] 延时时间基数
 *        - 单位：系统节拍(configTICK_RATE_HZ)
 *        - 特殊值：portMAX_DELAY表示无限等待
 * @param pxEventList [in] 事件等待队列指针
 *        - NULL：插入延时队列
 *        - 非NULL：插入指定事件等待队列
 * 
 * 实现机制：
 * 1. 计算唤醒时间：xTickCount + xTicksToDelay
 * 2. 更新协程状态为BLOCKED
 * 3. 从就绪列表移除，插入目标队列
 * 
 * 重要约束：
 * 1. 仅供协程内核实现使用
 * 2. 必须在调度器启动后调用
 * 3. 需在临界区内执行
 */
void vCoRoutineAddToDelayedList( TickType_t xTicksToDelay, List_t *pxEventList );

/**
 * @brief 事件队列处理函数（内部API）
 * 
 * 功能：从事件队列中唤醒最高优先级协程
 * 
 * 作用：
 * 1. 扫描事件等待队列
 * 2. 选择最高优先级协程
 * 3. 将协程移回就绪队列
 * 
 * @param pxEventList [in] 事件队列指针
 * @return 唤醒结果
 *        - pdTRUE：需要立即触发调度
 *        - pdFALSE：无优先级高于当前运行的协程
 * 
 * 实现机制：
 * 1. 使用uxPriority字段进行优先级排序
 * 2. 通过pxIndex遍历链表寻找最高优先级
 * 3. 更新调度器优先级位图
 * 
 * 典型应用场景：
 * - 队列数据到达时唤醒接收协程
 * - 信号量给出时唤醒等待协程
 * 
 * 性能优化：
 * 使用TAILQ优先队列结构实现O(1)时间复杂度
 */
BaseType_t xCoRoutineRemoveFromEventList( const List_t *pxEventList );

/* 技术架构说明：
 * 1. 延时队列管理：
 *    - 全局pxDelayedCoRoutineList列表
 *    - 按唤醒时间升序排列
 *    - 系统节拍中断中自动检测超时
 * 
 * 2. 事件队列管理：
 *    - 每个队列对象维护等待列表
 *    - 按协程优先级降序排列
 *    - 数据到达/空间释放时触发唤醒
 * 
 * 3. 状态转换关系：
 *    [就绪态] <-> [延时阻塞态] <-> [事件阻塞态]
 */

#ifdef __cplusplus
}
#endif

#endif /* CO_ROUTINE_H */

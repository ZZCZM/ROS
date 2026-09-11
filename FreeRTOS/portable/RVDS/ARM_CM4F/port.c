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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM4F port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
/* 硬件依赖性预处理检查 */
#ifndef __TARGET_FPU_VFP
    #error 本端口要求启用硬件浮点支持，请检查项目配置中FPU设置
#endif

/* 系统调用中断优先级有效性检查 */
#if configMAX_SYSCALL_INTERRUPT_PRIORITY == 0
    #error configMAX_SYSCALL_INTERRUPT_PRIORITY不能设为0，参考：http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html
#endif

/* SysTick时钟源配置 */
#ifndef configSYSTICK_CLOCK_HZ
    #define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ // 默认使用CPU主频
    #define portNVIC_SYSTICK_CLK_BIT (1UL << 2UL)    // SysTick控制寄存器第2位（时钟源选择）
#else
    #define portNVIC_SYSTICK_CLK_BIT (0)              // 保留原始时钟配置
#endif

/* Keil工具链弱函数覆盖配置 */
#ifndef configOVERRIDE_DEFAULT_TICK_CONFIGURATION
    #define configOVERRIDE_DEFAULT_TICK_CONFIGURATION 0 // 默认不覆盖SysTick配置
#endif

/*---------------------- 寄存器地址定义 ------------------------*/
#define portNVIC_SYSTICK_CTRL_REG        (*(volatile uint32_t*)0xE000E010) // SysTick控制寄存器
#define portNVIC_SYSTICK_LOAD_REG        (*(volatile uint32_t*)0xE000E014) // 重载值寄存器
#define portNVIC_SYSTICK_CURRENT_VALUE_REG (*(volatile uint32_t*)0xE000E018) // 当前值寄存器
#define portNVIC_SYSPRI2_REG             (*(volatile uint32_t*)0xE000ED20) // 系统优先级寄存器2

/*---------------------- 寄存器位定义 ---------------------------*/
#define portNVIC_SYSTICK_INT_BIT         (1UL << 1UL)   // SysTick中断使能位
#define portNVIC_SYSTICK_ENABLE_BIT      (1UL << 0UL)   // SysTick计数器使能位
#define portNVIC_SYSTICK_COUNT_FLAG_BIT  (1UL << 16UL)  // 计数完成标志位
#define portNVIC_PENDSVCLEAR_BIT         (1UL << 27UL)  // PendSV清除挂起位
#define portNVIC_PEND_SYSTICK_CLEAR_BIT  (1UL << 25UL)  // SysTick清除挂起位

/* Cortex-M7芯片识别 */
#define portCPUID                        (*(volatile uint32_t*)0xE000ED00)  // CPU ID寄存器
#define portCORTEX_M7_r0p1_ID             0x410FC271UL   // M7 r0p1芯片ID
#define portCORTEX_M7_r0p0_ID             0x410FC270UL   // M7 r0p0芯片ID

/* 内核中断优先级配置 */
#define portNVIC_PENDSV_PRI              (((uint32_t)configKERNEL_INTERRUPT_PRIORITY) << 16UL) // PendSV优先级位置
#define portNVIC_SYSTICK_PRI             (((uint32_t)configKERNEL_INTERRUPT_PRIORITY) << 24UL) // SysTick优先级位置

/* 中断优先级校验常量 */
#define portFIRST_USER_INTERRUPT_NUMBER  16            // 第一个用户可配置中断号
#define portNVIC_IP_REGISTERS_OFFSET_16  0xE000E3F0    // 中断16-255的优先级寄存器偏移
#define portAIRCR_REG                    (*(volatile uint32_t*)0xE000ED0C) // 应用中断控制寄存器
#define portMAX_8_BIT_VALUE               (uint8_t)0xFF // 8位最大值
#define portTOP_BIT_OF_BYTE              (uint8_t)0x80 // 字节最高位
#define portMAX_PRIGROUP_BITS            (uint8_t)7     // 优先级分组最大位数
#define portPRIORITY_GROUP_MASK          0x07UL << 8UL // 优先级组掩码
#define portPRIGROUP_SHIFT               8UL           // 优先级组偏移量

/* 中断控制状态寄存器掩码 */
#define portVECTACTIVE_MASK              0xFFUL       // 当前激活中断号掩码

/* 浮点单元控制相关 */
#define portFPCCR                  ((volatile uint32_t*)0xE000EF34) // 浮点上下文控制寄存器地址
#define portASPEN_AND_LSPEN_BITS   (0x3UL << 30UL)                 // 自动状态保存使能位

/* 初始上下文堆栈配置 */
#define portINITIAL_XPSR           0x01000000  // 初始程序状态寄存器值（Thumb模式）
#define portINITIAL_EXEC_RETURN    0xFFFFFFFD  // 异常返回使用PSP堆栈指针

/* SysTick相关限制 */
#define portMAX_24_BIT_NUMBER      0xFFFFFFUL   // 24位计数器的最大值

/* 低功耗模式校准参数 */
#define portMISSED_COUNTS_FACTOR   45UL         // Tickless模式下补偿系数

/* 任务入口地址对齐 */
#define portSTART_ADDRESS_MASK     (StackType_t)0xFFFFFFFEUL // PC地址必须对齐

/* 临界区嵌套计数器（调试用特殊初始化值） */
static UBaseType_t uxCriticalNesting = 0xAAAAAAAA; // 初始化为易识别模式

/*-----------------------------------------------------------*/
/* 设置定时器以生成节拍中断。该文件中的实现为弱定义，允许应用程序编写者更改用于生成节拍中断的定时器。 */
void vPortSetupTimerInterrupt( void );

/* 异常处理函数 */
void xPortPendSVHandler( void );    /* PendSV异常处理函数 */
void xPortSysTickHandler( void );   /* SysTick异常处理函数 */
void vPortSVCHandler( void );       /* SVC异常处理函数 */

/* 启动第一个任务单独作为一个函数，以便独立测试。 */
static void prvStartFirstTask( void );

/* 在portasm.s中定义的函数，用于启用VFP（浮点单元）。 */
static void prvEnableVFP( void );

/* 用于捕获试图从其实现函数返回的任务。 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/* 每个SysTick计数器递增所代表的tick周期数 */
#if configUSE_TICKLESS_IDLE == 1
    static uint32_t ulTimerCountsForOneTick = 0; // 单个tick周期的SysTick计数值
#endif /* configUSE_TICKLESS_IDLE */

/* 最大可抑制的tick周期数受限于SysTick定时器的24位分辨率 */
#if configUSE_TICKLESS_IDLE == 1
    static uint32_t xMaximumPossibleSuppressedTicks = 0; // 最大可休眠的tick数
#endif /* configUSE_TICKLESS_IDLE */

/* 补偿CPU在SysTick停止期间运行的时钟周期（仅用于低功耗模式） */
#if configUSE_TICKLESS_IDLE == 1
    static uint32_t ulStoppedTimerCompensation = 0; // 停止计时器时的补偿计数值
#endif /* configUSE_TICKLESS_IDLE */

/* 用于portASSERT_IF_INTERRUPT_PRIORITY_INVALID()宏，确保FreeRTOS API函数不会从优先级高于configMAX_SYSCALL_INTERRUPT_PRIORITY的中断中调用。 */
#if ( configASSERT_DEFINED == 1 )
    static uint8_t ucMaxSysCallPriority = 0; // 最大系统调用优先级
    static uint32_t ulMaxPRIGROUPValue = 0;  // 最大优先级分组值
    static const volatile uint8_t * const pcInterruptPriorityRegisters = ( uint8_t * ) portNVIC_IP_REGISTERS_OFFSET_16; // 中断优先级寄存器地址
#endif /* configASSERT_DEFINED */

/*-----------------------------------------------------------*/
/* 堆栈初始化函数
   功能：构建符合Cortex-M架构的任务初始堆栈结构
   参数：
   pxTopOfStack: 堆栈顶指针（需8字节对齐）
   pxCode:       任务函数入口指针
   pvParameters: 任务参数指针 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, 
                                   TaskFunction_t pxCode, 
                                   void *pvParameters )
{
    /* 模拟中断发生时的自动堆栈帧布局：
       Cortex-M硬件在异常进入时自动保存以下寄存器：
       xPSR, PC, LR, R12, R3, R2, R1, R0 */

    /* 调整栈顶指针：
       - 由于M4在中断入栈前会预调整SP，此处预先减1用于补偿 */
    pxTopOfStack--;  // 对齐到8字节边界
    
    /* 初始化程序状态寄存器xPSR（含Thumb状态位） */
    *pxTopOfStack = portINITIAL_XPSR; /* bit24置1表示Thumb模式 */
    pxTopOfStack--;

    /* 任务入口地址（清除地址bit0确保ARM模式兼容） */
    *pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;
    pxTopOfStack--;

    /* 链接寄存器初始为错误处理函数（阻止任务直接return） */
	*pxTopOfStack = ( StackType_t ) prvTaskExitError;

    /* 跳过R12,R3-R1寄存器初始化（优化空间） */
    pxTopOfStack -= 5;  
    
    /* 设置R0寄存器为任务参数指针 */
    *pxTopOfStack = ( StackType_t ) pvParameters;
    pxTopOfStack--;

    /* EXEC_RETURN值控制异常返回行为（使用PSP堆栈指针） */
    *pxTopOfStack = portINITIAL_EXEC_RETURN; /* 0xFFFFFFFD */
    // pxTopOfStack--;

    /* 预留通用寄存器R11-R4的存储空间 */
    pxTopOfStack -= 8; 

    return pxTopOfStack; // 返回调整后的栈顶指针
}

/*-----------------------------------------------------------*/
/* 任务退出错误处理函数
   触发场景：当任务函数试图通过return语句退出时调用 */
static void prvTaskExitError( void )
{
    /* 强制触发断言（仅在configASSERT开启时有效） 
       通过检查临界区嵌套计数值是否被修改（初始为0xAAAAAAAA） */
    configASSERT( uxCriticalNesting == ~0UL );
    
    /* 禁用所有可屏蔽中断 */
    portDISABLE_INTERRUPTS();
    
    /* 进入死循环以冻结系统（便于调试） */
    for( ;; )
    {
        __nop(); // 可添加断点用于调试器捕获
    }
}
/*-----------------------------------------------------------*/
/* SVC中断服务函数（上下文切换核心）*/
__asm void vPortSVCHandler( void )
{
    PRESERVE8   /* 保证8字节栈对齐 */

    /* 获取当前任务控制块指针 */
    ldr r3, =pxCurrentTCB   // 加载pxCurrentTCB地址到r3
    ldr r1, [r3]            // 获取当前TCB指针
    ldr r0, [r1]            // 获取TCB栈顶指针到r0

    /* 恢复现场寄存器组 */
    ldmia r0!, {r4-r11, r14} // 加载R4-R11及LR（异常返回地址）
    msr psp, r0              // 更新进程栈指针PSP
    isb                      // 指令同步屏障

    /* 允许所有优先级中断 */
    mov r0, #0
    msr basepri, r0          // 清除BASEPRI寄存器
    
    bx r14                   // 使用LR地址返回至任务入口
}
/*-----------------------------------------------------------*/

/* 首个任务启动代码 */
__asm void prvStartFirstTask( void )
{
    PRESERVE8

    /* 通过向量表偏移量获取初始堆栈地址 */
    ldr r0, =0xE000ED08      // 加载VTOR寄存器地址
    ldr r0, [r0]            // 获取向量表地址
    ldr r0, [r0]            // 获取初始MSP值

    msr msp, r0             // 复位主栈指针MSP
    
    /* 全局中断使能 */
    cpsie i                 // 允许处理器中断
    cpsie f                 // 允许FPU异常
    dsb                     // 数据同步屏障
    isb                     // 指令同步屏障

    /* 触发SVC异常进入任务环境 */
    svc 0                   // 生成SVC系统调用
    nop                     // 对齐占位
    nop                     // 对齐占位
}
/*-----------------------------------------------------------*/

/* 硬件浮点单元使能函数 */
__asm void prvEnableVFP( void )
{
    PRESERVE8

    /* CPACR寄存器地址（协处理器访问控制） */
    ldr.w r0, =0xE000ED88
    
    /* 设置CP10/CP11完全访问权限 */
    ldr r1, [r0]           // 读取当前寄存器值
    orr r1, r1, #(0xF << 20) // 设置bits23-20（CP10和CP11特权模式）
    str r1, [r0]           // 写回寄存器
    
    bx r14                  // 返回调用位置
    nop                     // 对齐占位
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* 调度器启动函数 */
BaseType_t xPortStartScheduler( void )
{
    /* 核验优先级配置合法性 */
    configASSERT( configMAX_SYSCALL_INTERRUPT_PRIORITY ); // 确保系统调用中断优先级不为0
    configASSERT( portCPUID != portCORTEX_M7_r0p1_ID );    // 确保不是Cortex-M7 r0p1版本
    configASSERT( portCPUID != portCORTEX_M7_r0p0_ID );    // 确保不是Cortex-M7 r0p0版本

    #if( configASSERT_DEFINED == 1 )
    {
        volatile uint32_t ulOriginalPriority;
        volatile uint8_t * const pucFirstUserPriorityReg = (uint8_t*)(portNVIC_IP_REGISTERS_OFFSET_16 + portFIRST_USER_INTERRUPT_NUMBER);
        
        /* 验证可用的中断优先级位数 */
        ulOriginalPriority = *pucFirstUserPriorityReg;     // 保存原始优先级
        *pucFirstUserPriorityReg = 0xFF;                    // 写入最大可能值
        uint8_t ucMaxPriorityValue = *pucFirstUserPriorityReg; // 读取实际有效位
        
        /* 核验内核优先级有效性 */
        configASSERT( ucMaxPriorityValue == ( configKERNEL_INTERRUPT_PRIORITY & ucMaxPriorityValue ) );

        /* 计算合法优先级组值 */
        ulMaxPRIGROUPValue = portMAX_PRIGROUP_BITS;
        while( (ucMaxPriorityValue & 0x80) == 0x80 ) {
            ulMaxPRIGROUPValue--;
            ucMaxPriorityValue <<= 1;
        }
        ulMaxPRIGROUPValue <<= portPRIGROUP_SHIFT; // 对齐到AIRCR寄存器位置
        ulMaxPRIGROUPValue &= portPRIORITY_GROUP_MASK; // 应用优先级组掩码
        /* 恢复原始优先级 */
        *pucFirstUserPriorityReg = ulOriginalPriority;
    }
    #endif

    /* 配置SysTick和PendSV为最低优先级 */
    portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI | portNVIC_SYSTICK_PRI;

    /* 初始化系统心跳定时器 */
    vPortSetupTimerInterrupt();

    /* 初始化临界区计数器 */
    uxCriticalNesting = 0;

    /* 启用硬件浮点支持 */
    prvEnableVFP();
    *(portFPCCR) |= portASPEN_AND_LSPEN_BITS;  // 启用惰性堆栈保存

    /* 切换到第一个任务 */
    prvStartFirstTask();

    /* 正常情况不应到达此处 */
    return 0; 
}

/*-----------------------------------------------------------*/
/* 调度器终止函数（当前端口未实现） */
void vPortEndScheduler( void )
{
    /* 此端口设计为无限运行，调用此函数将强制触发断言 */
    configASSERT( uxCriticalNesting == 1000UL ); // 伪造不成立条件
}
/*-----------------------------------------------------------*/

/* 进入临界区函数（非中断安全版） */
void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();  // 关闭可屏蔽中断
    uxCriticalNesting++;       // 递增嵌套计数器

    /* 检查是否在中断上下文中错误调用（仅在首次进入时检测） */
    if( uxCriticalNesting == 1 )
    {
        /* 通过检查VECTACTIVE字段确认是否处于中断中 */
        configASSERT( ( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK ) == 0 );
    }
}
/*-----------------------------------------------------------*/

/* 退出临界区函数 */
void vPortExitCritical( void )
{
    configASSERT( uxCriticalNesting );  // 防止过多次退出
    uxCriticalNesting--;

    /* 当嵌套归零时恢复中断 */
    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
}
/*-----------------------------------------------------------*/

/* PendSV中断处理（上下文切换核心） */
__asm void xPortPendSVHandler( void )
{
    /* 引入全局符号 */
    extern uxCriticalNesting;     // 临界区嵌套计数器
    extern pxCurrentTCB;          // 当前任务控制块指针
    extern vTaskSwitchContext;     // 任务切换函数

    PRESERVE8                      // 强制8字节栈对齐

    /* ---- 第一阶段：保存当前任务上下文 ---- */
    mrs r0, psp                   // R0 = 进程栈指针
    isb                           // 指令流同步屏障
    
    /* 获取当前TCB地址 */
    ldr r3, =pxCurrentTCB         // R3 = &pxCurrentTCB
    ldr r2, [r3]                 // R2 = pxCurrentTCB
    
    /* 检查FPU使用标志（LR bit4=0表示使用FPU） */
    tst r14, #0x10               // 测试EXC_RETURN的bit4
    it eq                        // 条件执行：浮点上下文存在时
    vstmdbeq r0!, {s16-s31}      // 保存FPU高16位寄存器(S16-S31)
    
    /* 保存核心寄存器到任务堆栈 */
    stmdb r0!, {r4-r11, r14}     // 保存R4-R11和LR
    
    /* 更新TCB中的栈顶指针 */
    str r0, [r2]                 // [pxCurrentTCB->pxTopOfStack] = R0

    /* ---- 第二阶段：任务切换保护 ---- */
    stmdb sp!, {r3}              // 保存R3到主堆栈
    mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
    msr basepri, r0              // 屏蔽低优先级中断
    dsb                          // 数据存储屏障
    isb                          // 指令流屏障
    
    /* 执行任务切换决策 */
    bl vTaskSwitchContext         // 选择下一个就绪任务
    
    /* 恢复中断屏蔽状态 */
    mov r0, #0
    msr basepri, r0
    ldmia sp!, {r3}              // 恢复R3（当前TCB指针）

    /* ---- 第三阶段：恢复新任务上下文 ---- */
    ldr r1, [r3]                 // R1 = 新TCB指针
    ldr r0, [r1]                 // R0 = 新任务栈顶指针
    
    /* 恢复核心寄存器 */
    ldmia r0!, {r4-r11, r14}     // 弹出R4-R11和LR
    
    /* 条件恢复FPU高寄存器 */
    tst r14, #0x10               // 检查是否需恢复FPU
    it eq
    vldmiaeq r0!, {s16-s31}      // 加载FPU高寄存器
    
    /* 切换进程堆栈指针 */
    msr psp, r0                  // 更新PSP为新任务栈顶
    isb                          // 确保指令顺序

    /* XMC4000系列芯片的特定补丁 */
    #ifdef WORKAROUND_PMU_CM001
        #if WORKAROUND_PMU_CM001 == 1
            push { r14 }         // 通过PC跳转清除流水线
            pop { pc }
            nop
        #endif
    #endif

    bx r14                       // 异常返回，跳转至新任务
}
/*-----------------------------------------------------------*/
/* SysTick中断服务函数（系统心跳核心） */
void xPortSysTickHandler( void )
{
    /* 提升中断优先级至configMAX_SYSCALL_INTERRUPT_PRIORITY */
    vPortRaiseBASEPRI(); // 等价于portSET_INTERRUPT_MASK_FROM_ISR()
    
    {
        /* 更新系统心跳计数并检测任务切换需求 */
        if( xTaskIncrementTick() != pdFALSE ) // 返回pdTRUE需切换任务
        {
            /* 挂起PendSV中断触发上下文切换 */
            portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
        }
    }
    
    /* 恢复原始中断优先级 */
    vPortClearBASEPRIFromISR(); 
}
/*-----------------------------------------------------------*/

#if configUSE_TICKLESS_IDLE == 1  // Tickless低功耗模式支持

/* Tickless模式休眠控制函数（__weak允许用户覆盖） */
__weak void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements, ulSysTickCTRL;
    TickType_t xModifiableIdleTime;

    /* 校核最大可休眠节拍数（防止24位计数器溢出） */
    if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
    {
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
    }

    /* 暂停SysTick计时 */
    portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

    /* 计算重载值（补偿停止期间流逝的时间） */
    ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE_REG + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );
    if( ulReloadValue > ulStoppedTimerCompensation )
    {
        ulReloadValue -= ulStoppedTimerCompensation; // 应用补偿值
    }

    /* 禁止中断（不使用taskENTER_CRITICAL避免屏蔽必要中断） */
    __disable_irq();
    __dsb( portSY_FULL_READ_WRITE ); // 数据同步屏障
    __isb( portSY_FULL_READ_WRITE ); // 指令同步屏障

    if( eTaskConfirmSleepModeStatus() == eAbortSleep ) // 存在待处理事件
    {
        /* 恢复当前周期剩余计数 */
        portNVIC_SYSTICK_LOAD_REG = portNVIC_SYSTICK_CURRENT_VALUE_REG;
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT; // 重启SysTick
        portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL; // 重置标准周期
        __enable_irq();
    }
    else // 允许进入低功耗
    {
        /* 配置新重载值并清空当前计数器 */
        portNVIC_SYSTICK_LOAD_REG = ulReloadValue;
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

        /* 执行预休眠处理（如外设断电）*/
        xModifiableIdleTime = xExpectedIdleTime;
        configPRE_SLEEP_PROCESSING( xModifiableIdleTime );
        
        /* 当处理宏未执行WFI时进入休眠 */
        if( xModifiableIdleTime > 0 )
        {
            __dsb( portSY_FULL_READ_WRITE );
            __wfi(); // 等待中断唤醒
            __isb( portSY_FULL_READ_WRITE );
        }
        configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

        /* 停止SysTick并保存控制状态 */
        ulSysTickCTRL = portNVIC_SYSTICK_CTRL_REG;
        portNVIC_SYSTICK_CTRL_REG = ( ulSysTickCTRL & ~portNVIC_SYSTICK_ENABLE_BIT );
        __enable_irq();

        if( ( ulSysTickCTRL & portNVIC_SYSTICK_COUNT_FLAG_BIT ) != 0 ) // 完整节拍
        {
            uint32_t ulCalculatedLoadValue;
            
            /* 计算剩余计数值（防止下溢） */
            ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL ) - ( ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE_REG );
            if( ( ulCalculatedLoadValue < ulStoppedTimerCompensation ) || ( ulCalculatedLoadValue > ulTimerCountsForOneTick ) )
            {
                ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL );
            }
            
            portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;
            ulCompleteTickPeriods = xExpectedIdleTime - 1UL; // 完整周期计数
        }
        else // 中途被其他中断唤醒
        {
            /* 计算实际发生的递减次数 */
            ulCompletedSysTickDecrements = ( xExpectedIdleTime * ulTimerCountsForOneTick ) - portNVIC_SYSTICK_CURRENT_VALUE_REG;
            
            /* 计算完整节拍数 */
            ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;
            
            /* 重新加载剩余计数 */
            portNVIC_SYSTICK_LOAD_REG = ( ( ulCompleteTickPeriods + 1 ) * ulTimerCountsForOneTick ) - ulCompletedSysTickDecrements;
        }

        /* 原子操作恢复SysTick */
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
        portENTER_CRITICAL();
        {
            portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT; // 重启
            vTaskStepTick( ulCompleteTickPeriods ); // 补偿丢失的节拍
            portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL; // 恢复标准周期
        }
        portEXIT_CRITICAL();
    }
}

#endif /* configUSE_TICKLESS_IDLE */


/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/* SysTick定时器初始化函数（默认实现，可被用户覆盖） */
#if configOVERRIDE_DEFAULT_TICK_CONFIGURATION == 0

    void vPortSetupTimerInterrupt( void )
    {
        /* Tickless模式相关参数计算 */
        #if configUSE_TICKLESS_IDLE == 1
        {
            /* 计算每个tick周期的定时器计数值 */
            ulTimerCountsForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ );
            
            /* 计算允许的最大休眠节拍（防止24位计数器溢出） */
            xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
            
            /* 补偿计时器停止期间的误差（经验系数校准） */
            ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / ( configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ );
        }
        #endif /* configUSE_TICKLESS_IDLE */

        /* 配置SysTick重载值： 
           计数值 = (时钟频率 / tick频率) - 1 
           示例：168MHz/1kHz=168000 -> 167999 */
        portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
        
        /* 启用SysTick定时器：
           portNVIC_SYSTICK_CLK_BIT-时钟源选择
           portNVIC_SYSTICK_INT_BIT-启用中断
           portNVIC_SYSTICK_ENABLE_BIT-启动计数器 */
        portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | 
                                    portNVIC_SYSTICK_INT_BIT | 
                                    portNVIC_SYSTICK_ENABLE_BIT );
    }

#endif /* 默认配置开关 */
/*-----------------------------------------------------------*/

/* 获取当前中断状态寄存器值（汇编实现） */
__asm uint32_t vPortGetIPSR( void )
{
    PRESERVE8          /* 保证8字节栈对齐 */

    mrs r0, ipsr      /* 将ipsr寄存器的值移动到r0 */
    bx r14            /* 通过链接寄存器返回 */
}
/*-----------------------------------------------------------*/

#if( configASSERT_DEFINED == 1 )  // 断言检测功能开启

    /* 中断优先级验证函数 */
    void vPortValidateInterruptPriority( void )
    {
    uint32_t ulCurrentInterrupt;  // 当前中断号
    uint8_t ucCurrentPriority;    // 当前中断优先级

        /* 获取当前执行的中断号 */
        ulCurrentInterrupt = vPortGetIPSR();

        /* 仅校验用户可配置的中断（中断号>=16） */
        if( ulCurrentInterrupt >= portFIRST_USER_INTERRUPT_NUMBER )
        {
            /* 从NVIC优先级寄存器获取优先级（寄存器映射表查询） */
            ucCurrentPriority = pcInterruptPriorityRegisters[ ulCurrentInterrupt ];

            /* 关键断言：确保中断优先级≥允许的最高系统调用优先级 
               数值越大表示优先级越低 */
            configASSERT( ucCurrentPriority >= ucMaxSysCallPriority );
        }

        /* 校验优先级分组配置（必须全为抢占优先级） 
           portAIRCR_REG寄存器的8-10位存储优先级分组设置 */
        configASSERT( ( portAIRCR_REG & portPRIORITY_GROUP_MASK ) <= ulMaxPRIGROUPValue );
    }

#endif /* 断言检测开关 */



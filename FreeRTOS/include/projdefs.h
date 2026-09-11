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

#ifndef PROJDEFS_H
#define PROJDEFS_H
/*--------------------- 任务函数原型定义 ---------------------*/
/**
 * @brief 任务函数标准原型
 * @param pvParameters 任务参数指针
 * 
 * 所有用户任务函数必须符合此原型，例如：
 * void vTaskFunction(void *pvParameters) { ... }
 */
typedef void (*TaskFunction_t)( void * );

/*--------------------- 时间转换宏 ---------------------*/
/**
 * @brief 毫秒转系统节拍数
 * @param xTimeInMs 毫秒时间值
 * @warning 当configTICK_RATE_HZ非1000约数时存在精度损失
 * 
 * 示例：configTICK_RATE_HZ=1000 → pdMS_TO_TICKS(500) = 500 ticks
 *       configTICK_RATE_HZ=100  → pdMS_TO_TICKS(50) = 5 ticks
 */
#ifndef pdMS_TO_TICKS
    #define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif

/*--------------------- 基础状态常量 ---------------------*/
#define pdFALSE         ( ( BaseType_t ) 0 )   ///< 逻辑假值
#define pdTRUE          ( ( BaseType_t ) 1 )   ///< 逻辑真值

#define pdPASS          ( pdTRUE )             ///< 操作成功返回值
#define pdFAIL          ( pdFALSE )            ///< 操作失败返回值
#define errQUEUE_EMPTY  ( ( BaseType_t ) 0 )   ///< 队列空状态码（xQueueReceive）
#define errQUEUE_FULL   ( ( BaseType_t ) 0 )   ///< 队列满状态码（xQueueSend）

/*--------------------- 扩展错误码定义 ---------------------*/
#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY   ( -1 ) ///< 内存分配失败（pvPortMalloc）
#define errQUEUE_BLOCKED                        ( -4 ) ///< 队列操作被阻塞（API调用超时）
#define errQUEUE_YIELD                          ( -5 ) ///< 队列操作导致任务切换（xQueueSendFromISR）

/*--------------------- 数据完整性校验 ---------------------*/
#ifndef configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES
    #define configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES 0 ///< 启用校验：1=开启列表结构校验
#endif

#if( configUSE_16_BIT_TICKS == 1 )
    #define pdINTEGRITY_CHECK_VALUE 0x5a5a          ///< 16位校验魔数（ASCII "ZZ"）
#else
    #define pdINTEGRITY_CHECK_VALUE 0x5a5a5a5aUL    ///< 32位校验魔数（ASCII "ZZZZ"）
#endif

/*------------------ FreeRTOS+组件错误码体系 ------------------*/
/* 注：以下错误码用于FreeRTOS+扩展组件（如FAT/TCP/IP栈） */

#define pdFREERTOS_ERRNO_NONE			0	/* 无错误（成功状态） */
#define	pdFREERTOS_ERRNO_ENOENT			2	/* 路径不存在（文件/目录未找到） */
#define	pdFREERTOS_ERRNO_EINTR			4	/* 系统调用被信号中断 */
#define	pdFREERTOS_ERRNO_EIO			5	/* 物理I/O错误（存储设备故障） */
#define	pdFREERTOS_ERRNO_ENXIO			6	/* 设备地址无效（驱动不存在） */
#define	pdFREERTOS_ERRNO_EBADF			9	/* 错误文件描述符（未打开/已关闭） */
#define	pdFREERTOS_ERRNO_EAGAIN			11	/* 资源暂时不可用（非阻塞模式） */
#define	pdFREERTOS_ERRNO_EWOULDBLOCK	11	/* 操作将阻塞（同EAGAIN） */
#define	pdFREERTOS_ERRNO_ENOMEM			12	/* 内存不足（动态分配失败） */
#define	pdFREERTOS_ERRNO_EACCES			13	/* 权限不足（写保护/无访问权） */
#define	pdFREERTOS_ERRNO_EFAULT			14	/* 非法地址（用户空间指针错误） */
#define	pdFREERTOS_ERRNO_EBUSY			16	/* 设备/资源忙（无法卸载） */
#define	pdFREERTOS_ERRNO_EEXIST			17	/* 文件已存在（创建冲突） */
#define	pdFREERTOS_ERRNO_EXDEV			18	/* 跨设备链接（rename跨分区） */
#define	pdFREERTOS_ERRNO_ENODEV			19	/* 设备不存在（驱动未加载） */
#define	pdFREERTOS_ERRNO_ENOTDIR		20	/* 非目录对象（路径中含文件） */
#define	pdFREERTOS_ERRNO_EISDIR			21	/* 目录操作冲突（误用文件API） */
#define	pdFREERTOS_ERRNO_EINVAL			22	/* 无效参数（API参数校验失败） */
#define	pdFREERTOS_ERRNO_ENOSPC			28	/* 设备空间不足（磁盘满） */
#define	pdFREERTOS_ERRNO_ESPIPE			29	/* 非法定位（管道文件seek） */
#define	pdFREERTOS_ERRNO_EROFS			30	/* 只读文件系统（写操作被拒） */
#define	pdFREERTOS_ERRNO_EUNATCH		42	/* 协议驱动未连接（CAN总线等） */
#define	pdFREERTOS_ERRNO_EBADE			50	/* 无效交换（IPC通信异常） */
#define	pdFREERTOS_ERRNO_EFTYPE			79	/* 文件类型不匹配（固件格式错误） */
#define	pdFREERTOS_ERRNO_ENMFILE		89	/* 打开文件数超限（FAT表满） */
#define	pdFREERTOS_ERRNO_ENOTEMPTY		90	/* 目录非空（删除失败） */
#define	pdFREERTOS_ERRNO_ENAMETOOLONG 	91	/* 路径超长（超过_MAX_LFN） */
#define	pdFREERTOS_ERRNO_EOPNOTSUPP		95	/* 操作不支持（socket类型不符） */
#define	pdFREERTOS_ERRNO_ENOBUFS		105	/* 缓冲区不足（网络堆栈耗尽） */
#define	pdFREERTOS_ERRNO_ENOPROTOOPT	109	/* 协议选项无效（TCP参数错误） */
#define	pdFREERTOS_ERRNO_EADDRINUSE		112	/* 端口/地址被占用（socket绑定失败） */
#define	pdFREERTOS_ERRNO_ETIMEDOUT		116	/* 连接超时（TCP握手失败） */
#define	pdFREERTOS_ERRNO_EINPROGRESS	119	/* 操作进行中（非阻塞模式返回） */
#define	pdFREERTOS_ERRNO_EALREADY		120	/* 已建立连接（重复connect调用） */
#define	pdFREERTOS_ERRNO_EADDRNOTAVAIL 	125	/* 地址不可用（IP配置错误） */
#define	pdFREERTOS_ERRNO_EISCONN		127	/* 已连接状态（重复操作） */
#define	pdFREERTOS_ERRNO_ENOTCONN		128	/* 未建立连接（sendto未指定目标） */
#define	pdFREERTOS_ERRNO_ENOMEDIUM		135	/* 存储介质未插入（SD卡拔出） */
#define	pdFREERTOS_ERRNO_EILSEQ			138	/* 非法字符序列（UTF-8解码失败） */
#define	pdFREERTOS_ERRNO_ECANCELED		140	/* 操作被取消（异步任务终止） */

/*--------------------- 系统字节序定义 ---------------------*/ 
#define pdFREERTOS_LITTLE_ENDIAN	0   ///< 小端模式（x86/ARM常见）
#define pdFREERTOS_BIG_ENDIAN		1   ///< 大端模式（网络协议/PPC架构）


#endif /* PROJDEFS_H */




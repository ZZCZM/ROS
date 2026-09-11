/**
  ******************************************************************************
  * @file    stmflash.c
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
#include "stmflash.h"
#include "delay.h"
#include "stmflash.h"  
#define FLASH_SAVE_ADDR  0X0800E000 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
/* STM32内部Flash操作底层驱动模块 */

/*------------------------------------
 * Flash控制寄存器操作系列函数
 * 注意：所有写/擦除操作前必须先解锁
 *----------------------------------*/

/**
  * @brief  Flash解锁函数
  * @note   必须先解锁才能执行编程或擦除操作
  *         解锁序列为依次写入两个特定密钥
  */
void STMFLASH_Unlock(void)
{
  FLASH->KEYR = FLASH_KEY1;  // 写入第一个解锁密钥（0x45670123）
  FLASH->KEYR = FLASH_KEY2;  // 写入第二个解锁密钥（0xCDEF89AB）
}

/**
  * @brief  Flash上锁函数
  * @note   完成编程操作后必须立即上锁
  *         通过设置CR寄存器的LOCK位实现
  */
void STMFLASH_Lock(void)
{
  FLASH->CR |= 1 << 7;  // 设置LOCK位(bit7)为1
}

/*------------------------------------
 * Flash状态管理系列函数
 *----------------------------------*/

/**
  * @brief  获取Flash操作状态
  * @retval 状态码:
  *         0:操作完成  1:忙状态 
  *         2:编程错误 3:写保护错误
  */
u8 STMFLASH_GetStatus(void)
{	
	u32 res;		
	res = FLASH->SR;  // 读取状态寄存器
	if(res & (1 << 0)) return 1;     // 检查BSY位(bit0)
	else if(res & (1 << 2)) return 2;// 检查PGERR位(bit2)
	else if(res & (1 << 4)) return 3;// 检查WRPRTERR位(bit4)
	return 0;                        // 无异常状态
}
/**
  * @brief  等待Flash操作完成
  * @param  time: 最大等待时间（单位：微秒）
  * @retval 状态码（0-成功，非0-错误码，0xFF-超时）
  * @note   状态机流转：
  *         1. 持续轮询FLASH_SR寄存器状态
  *         2. 检测到FLASH_BUSY标志清除时退出
  *         3. 超时保护防止死锁
  */
u8 STMFLASH_WaitDone(u16 time)
{
    u8 res;
    do {
        res = STMFLASH_GetStatus(); // 获取当前状态（0-空闲，1-忙，2-操作错误）
        if(res != 1) break;         // 非忙状态立即退出循环
        
        /* 精确时序控制 */
        delay_us(1);                // 使用硬件级精确延时（避免使用软件循环延时）
        time--;                     // 递减超时计数器
    } while(time);                  // 检测超时或状态变化
    
    if(time == 0) res = 0xFF;       // 超时标志（0xFF表示超过最大等待时间）
    return res;
}

/*------------------------------------
 * Flash存储单元操作函数
 *----------------------------------*/

/**
  * @brief  擦除指定Flash页（4KB）
  * @param  paddr: 页起始地址（必须4KB对齐）
  * @retval 执行状态（0-成功，其他值参考STMFLASH_GetStatus定义）
  * @note   擦除流程：
  *         1. 检查并等待Flash控制器就绪
  *         2. 设置页擦除模式
  *         3. 写入目标地址
  *         4. 触发擦除操作
  *         5. 等待擦除完成
  *         6. 清理操作标志
  */
u8 STMFLASH_ErasePage(u32 paddr)
{
    u8 res = 0;
    /* 前置状态检查 */
    res = STMFLASH_WaitDone(0X5FFF);  // 等待最大20ms（0x5FFF*1us≈28ms）
    if(res == 0) {                    // 确认控制器处于就绪状态
        /* 配置页擦除模式 */
        FLASH->CR |= 1 << 1;         // 设置CR寄存器的PER位（Page Erase）
        
        /* 设置擦除地址 */
        FLASH->AR = paddr;            // 写入要擦除的页起始地址（必须4KB对齐）
        
        /* 启动擦除操作 */
        FLASH->CR |= 1 << 6;          // 置位STRT位启动擦除
        
        /* 等待擦除完成 */
        res = STMFLASH_WaitDone(0X5FFF); // 再次等待操作完成
        
        /* 后置清理 */
        if(res != 1) {                // 操作异常时恢复状态
            FLASH->CR &= ~(1 << 1);   // 清除PER位防止误操作
            FLASH->SR = 0xFFFFFFFF;   // 清除所有状态标志（可选）
        }
    }
    return res;
}

/**
  * @brief  写入16位数据到指定地址
  * @param  faddr: 目标地址（必须2字节对齐）
  * @param  dat: 要写入的16位数据
  * @retval 写入状态
  * @note   地址必须位于已擦除的区域
  */
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat)
{
	u8 res;	   	    
	res = STMFLASH_WaitDone(0XFF);  // 等待空闲
	if(res == 0) {
		FLASH->CR |= 1 << 0;        // 设置PG位(bit0)启用编程
		*(vu16*)faddr = dat;        // 执行数据写入
		res = STMFLASH_WaitDone(0XFF);
		if(res != 1) {
			FLASH->CR &= ~(1 << 0); // 清除PG位
		}
	} 
	return res;
}

/**
  * @brief  读取指定地址的16位数据
  * @param  faddr: 读取地址
  * @retval 读取到的16位数据
  * @note   无需解锁即可执行读取操作
  */
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr;  // 直接访问Flash地址空间
}
#if STM32_FLASH_WREN  // Flash写操作使能宏定义开关
/* 
函数功能：无校验快速写入多个半字数据到Flash
参数说明：
- WriteAddr: 写入起始地址（必须为合法Flash地址）
- pBuffer:   待写入数据缓冲区的16位指针
- NumToWrite:需要写入的半字（16位）数量
注意事项：
1. 不进行地址对齐检查和Flash状态校验
2. 要求调用者确保目标地址已擦除且地址合法
3. 地址自动按半字(2字节)递增
*/
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
    u16 i;
    for(i = 0; i < NumToWrite; i++)  // 遍历所有待写入数据
    {
        // 调用底层半字写入函数
        STMFLASH_WriteHalfWord(WriteAddr, pBuffer[i]); 
        WriteAddr += 2;  // 地址指针递增2字节（半字地址步进）
    }
}

/* 根据芯片型号定义扇区大小 */
#if STM32_FLASH_SIZE < 256       // 小容量芯片处理
    #define STM_SECTOR_SIZE 1024 // 小容量芯片扇区为1KB
#else                            // 大容量芯片处理
    #define STM_SECTOR_SIZE 2048 // 大容量芯片扇区为2KB
#endif

u16 STMFLASH_BUF[STM_SECTOR_SIZE/2]; // 定义扇区缓冲区（按半字计算大小）

/*
函数功能：带擦除校验的安全写入函数
参数说明：
- WriteAddr: 写入起始地址（必须2字节对齐且合法）
- pBuffer:   待写入数据缓冲区的16位指针
- NumToWrite:需要写入的半字（16位）数量
算法流程：
1. 地址合法性验证 → 2. 解锁Flash → 3. 计算扇区位置 → 
4. 循环写入（需要时擦除）→ 5. 重新上锁
*/
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
    u32 secpos;    // 目标地址所在扇区编号
    u16 secoff;    // 在扇区内的半字偏移量
    u16 secremain; // 当前扇区剩余可写半字数
    u16 i;
    u32 offaddr;   // 相对于Flash基址的偏移地址

    /* 地址有效性检查 */
    if(WriteAddr < STM32_FLASH_BASE || 
       WriteAddr >= (STM32_FLASH_BASE + 1024*STM32_FLASH_SIZE))
        return;  // 非法地址直接返回

    STMFLASH_Unlock();  // 解除Flash写保护
    
    offaddr = WriteAddr - STM32_FLASH_BASE;  // 计算实际偏移地址
    secpos = offaddr / STM_SECTOR_SIZE;      // 计算所属扇区索引
    secoff = (offaddr % STM_SECTOR_SIZE)/2;  // 计算扇区内半字偏移
    secremain = STM_SECTOR_SIZE/2 - secoff;  // 计算扇区剩余容量

    /* 调整实际写入数量 */
    secremain = (NumToWrite <= secremain) ? NumToWrite : secremain;

    while(1)  // 多扇区写入循环
    {
        /* 读取整个扇区到缓冲区 */
        STMFLASH_Read(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE, 
                     STMFLASH_BUF, 
                     STM_SECTOR_SIZE/2);

        /* 校验缓冲区是否需要擦除 */
        for(i = 0; i < secremain; i++)
        {
            if(STMFLASH_BUF[secoff+i] != 0XFFFF) 
                break;  // 发现非空位置需要擦除
        }

        if(i < secremain)  // 需要执行擦除操作
        {
            /* 擦除当前扇区 */
            STMFLASH_ErasePage(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE);
            
            /* 更新缓冲区数据 */
            for(i = 0; i < secremain; i++)
                STMFLASH_BUF[secoff+i] = pBuffer[i];  // 合并新旧数据
            
            /* 写入整个扇区 */
            STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE,
                                  STMFLASH_BUF, 
                                  STM_SECTOR_SIZE/2);
        }
        else  // 无需擦除直接写入
        {
            STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);
        }

        /* 判断写入是否完成 */
        if(NumToWrite == secremain) 
            break;  // 全部数据写入完成
        
        /* 准备下一个扇区写入 */
        secpos++;      // 切换到下一个扇区
        secoff = 0;    // 新扇区偏移归零
        pBuffer += secremain;         // 调整数据指针
        WriteAddr += secremain * 2;   // 地址指针前进（字节地址）
        NumToWrite -= secremain;      // 更新剩余写入数量
        
        /* 计算下次写入量 */
        secremain = (NumToWrite > (STM_SECTOR_SIZE/2)) ? 
                   (STM_SECTOR_SIZE/2) : NumToWrite;
    }
    
    STMFLASH_Lock();  // 重新上锁Flash写保护
}
#endif
/*
函数功能：从Flash存储器连续读取多个半字数据
参数说明：
- ReadAddr:  读取起始地址（建议2字节对齐）
- pBuffer:   数据存储缓冲区的16位指针
- NumToRead: 需要读取的半字数量
注意事项：
1. 地址自动按半字(2字节)递增
2. 不进行地址合法性检查，调用者需确保地址有效
3. 读取操作不会影响Flash内容
*/
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)
{
    u16 i;
    for(i = 0; i < NumToRead; i++)
    {
        /* 读取单个半字到缓冲区 */
        pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);  // 调用底层读取函数
        
        /* 地址指针递增2字节 */
        ReadAddr += 2;  // Flash按半字(16位)编址，每次读取后前进2字节地址
    }
}

/*
函数功能：Flash写入测试函数（单半字写入）
参数说明：
- WriteAddr: 写入目标地址（必须合法且已擦除）
- WriteData: 要写入的16位数据
设计特点：
1. 封装STMFLASH_Write用于快速测试单数据写入
2. 实际执行的是半字(16位)写入操作
3. 适用于参数调试和单元测试
*/
void Test_Write(u32 WriteAddr, u16 WriteData)
{
    /* 调用通用写入函数执行单数据写入 */
    STMFLASH_Write(WriteAddr, &WriteData, 1);  // 末参数1表示写入1个半字
}

 /**************************************************************************
函数功能：从Flash读取PID参数数据，用于系统初始化时加载保存的控制参数
入口参数：无
返回  值：无
注意事项：
1. 首次上电时Flash内容为全FF，需初始化默认参数
2. 使用STM32内部Flash的特定扇区存储关键运行参数
3. 参数存储格式：u16数组按顺序存储各控制参数
**************************************************************************/
void Flash_Read(void)
{
    /* 从预定义Flash地址读取10个u16数据到参数数组
       FLASH_SAVE_ADDR: 参数存储起始地址（需4字节对齐）
       PID_Parameter:   参数缓存数组指针
       10:            读取10个u16型数据 */
    STMFLASH_Read(FLASH_SAVE_ADDR, (u16*)PID_Parameter, 10);

    /* 检测Flash初始状态：判断前7个参数是否为未编程状态（全FF）
       - STM32擦除后Flash内容为全FF
       - 65535对应u16型的0xFFFF */
    if(PID_Parameter[0]==65535 && PID_Parameter[1]==65535 && 
       PID_Parameter[2]==65535 && PID_Parameter[3]==65535 &&
       PID_Parameter[4]==65535 && PID_Parameter[5]==65535 && 
       PID_Parameter[6]==65535)
    {
        /* 初始化默认参数：检测到首次使用时设置默认值
           RC遥控速度基准值 */
        RC_Velocity = 30;    // 单位：cm/s
        
        /* PID控制参数：速度环比例系数 */
        Velocity_KP = 12;    // 比例系数需根据系统调试确定
        
        /* PID控制参数：速度环积分系数 */ 
        Velocity_KI = 12;    // 积分系数防止超调
    }
    else
    {        
        /* 加载存储参数：从Flash读取有效参数
           [0] 速度控制基准值（单位：cm/s） */
        RC_Velocity = PID_Parameter[0];
        
        /* [1] 速度环比例系数，影响系统响应速度 */
        Velocity_KP = PID_Parameter[1];
        
        /* [2] 速度环积分系数，消除稳态误差 */
        Velocity_KI = PID_Parameter[2];
        
        /* 注：数组索引3-6保留为未来扩展参数使用 */
    }
}

/**************************************************************************
函数功能：将当前控制参数写入Flash持久化存储
入口参数：无
返回  值：无
注意事项：
1. 写入前必须确保Flash目标扇区已擦除
2. 使用u16数组存储参数以适应Flash编程粒度
3. FLASH_SAVE_ADDR需4字节对齐且位于有效扇区
4. 建议在系统停机前调用，避免运行时频繁写Flash
**************************************************************************/
void Flash_Write(void)
{
    /* 参数打包：将运行参数存入临时数组
       [0] 遥控基准速度（单位：厘米/秒） */
    Flash_Parameter[0] = RC_Velocity;  
    
    /* [1] 速度环比例系数，影响系统动态响应 */
    Flash_Parameter[1] = Velocity_KP;   
    
    /* [2] 速度环积分系数，消除稳态误差 */
    Flash_Parameter[2] = Velocity_KI;   
    
    /* 保留索引3-9为未来参数扩展预留空间 */

    /* Flash编程操作：
       FLASH_SAVE_ADDR：参数存储起始地址（需4KB对齐）
       Flash_Parameter：数据源指针（u16类型强制转换适配编程函数）
       10：写入10个u16数据（含预留空间）
       注意：此操作会耗时数百微秒，需关闭中断 */
    STMFLASH_Write(FLASH_SAVE_ADDR, (u16*)Flash_Parameter, 10);    
}    









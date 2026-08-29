/**
  ******************************************************************************
  * @file    F4_flash.c
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
#include "F4_flash.h"

extern void Delay(__IO uint32_t nCount);//延迟函数
/* 全局变量定义 -----------------------------------------------------------*/
volatile Flash_Parameter flash_parameter = {
    FLASH_SAVE_ADDR, // uint32_t flash_address_mark; // 位置标记
    0x0001,        // uint16_t Moveit_Angle1_init; // 初始位置1
    0x0010,        // uint16_t Moveit_Angle2_init; // 初始位置2
    0x0100,        // uint16_t Moveit_Angle3_init; // 初始位置3
    0x1000         // uint16_t Moveit_Angle4_init; // 初始位置4
};

float Position1 = SERVO_INIT, Position2 = SERVO_INIT, Position3 = SERVO_INIT, Position4 = SERVO_INIT; // 电机当前位置
float Velocity1 = 0, Velocity2 = 0, Velocity3 = 0, Velocity4 = 0; // 电机速度
float Position_KP = 2, Position_KI = 0, Position_KD = 1; // 位置控制PID参数
short Moveit_Angle1_init = 0, Moveit_Angle2_init = 0, Moveit_Angle3_init = 0, Moveit_Angle4_init = 0; // 舵机初始位置微调值
int Servo_init_angle_adjust = 0; // 零点调整标志位

/* 函数定义 --------------------------------------------------------------*/

/**************************************************************************
函数功能：读取指定地址的半字（16位数据）
入口参数：
  faddr - 读取地址
返回  值：对应数据（16位）
说明：从指定地址读取一个16位数据（半字）
**************************************************************************/
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr; // 读取指定地址的半字
}

/* 函数定义 --------------------------------------------------------------*/

/**************************************************************************
函数功能：获取某个地址所在的Flash扇区
入口参数：
  addr - Flash地址
返回  值：0~11，即addr所在的扇区
说明：根据地址确定所在的Flash扇区，STM32F4的Flash有12个扇区，每个扇区大小不同
**************************************************************************/
uint16_t STMFLASH_GetFlashSector(u32 addr)
{
    if (addr < ADDR_FLASH_SECTOR_1) return FLASH_Sector_0; // 扇区0
    else if (addr < ADDR_FLASH_SECTOR_2) return FLASH_Sector_1; // 扇区1
    else if (addr < ADDR_FLASH_SECTOR_3) return FLASH_Sector_2; // 扇区2
    else if (addr < ADDR_FLASH_SECTOR_4) return FLASH_Sector_3; // 扇区3
    else if (addr < ADDR_FLASH_SECTOR_5) return FLASH_Sector_4; // 扇区4
    else if (addr < ADDR_FLASH_SECTOR_6) return FLASH_Sector_5; // 扇区5
    else if (addr < ADDR_FLASH_SECTOR_7) return FLASH_Sector_6; // 扇区6
    else if (addr < ADDR_FLASH_SECTOR_8) return FLASH_Sector_7; // 扇区7
    else if (addr < ADDR_FLASH_SECTOR_9) return FLASH_Sector_8; // 扇区8
    else if (addr < ADDR_FLASH_SECTOR_10) return FLASH_Sector_9; // 扇区9
    else if (addr < ADDR_FLASH_SECTOR_11) return FLASH_Sector_10; // 扇区10
    return FLASH_Sector_11; // 扇区11
}

/**************************************************************************
函数功能：从指定地址开始写入指定长度的数据
入口参数：
  WriteAddr - 起始地址（必须为2的倍数）
  pBuffer - 数据指针
  NumToWrite - 半字（16位）数
返回  值：无
说明：将数据写入Flash，注意写入非0xFFFF的地址会先擦除整个扇区，导致数据丢失
      建议在写入前确保扇区已擦除，从扇区起始地址逐步写入
      可以写入OTP区域（0X1FFF7800~0X1FFF7A0F）
**************************************************************************/
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u32 NumToWrite)	
{ 
    FLASH_Status status = FLASH_COMPLETE;
    u32 addrx = 0;
    u32 endaddr = 0;	

    if (WriteAddr < STM32_FLASH_BASE || WriteAddr % 2) return; // 非法地址

    FLASH_Unlock(); // 解锁Flash以允许写入

    FLASH_DataCacheCmd(DISABLE); // 写入期间禁止数据缓存

    addrx = WriteAddr; // 写入的起始地址
    endaddr = WriteAddr + NumToWrite * 2; // 写入的结束地址

    if (addrx < 0X1FFF0000) // 只有主存储区才需要执行擦除操作
    {
        while (addrx < endaddr) // 扫描地址范围，找到非0xFFFF的地方
        {
            if (STMFLASH_ReadHalfWord(addrx) != 0XFFFF) // 如果有非0XFFFF的地方，需要擦除扇区
            {   
                status = FLASH_EraseSector(STMFLASH_GetFlashSector(addrx), VoltageRange_3); // 2.7~3.6V之间
                if (status != FLASH_COMPLETE) break; // 发生错误，停止写入
            }
            else
                addrx += 2;
        } 
    }

    if (status == FLASH_COMPLETE)
    {
        while (WriteAddr < endaddr) // 写入数据
        {
            if (FLASH_ProgramHalfWord(WriteAddr, *pBuffer) != FLASH_COMPLETE) // 写入半字
            { 
                break; // 写入异常，停止写入
            }
            WriteAddr += 2;
            pBuffer++;
        } 
    }

    FLASH_DataCacheCmd(ENABLE); // 写入结束，开启数据缓存

    FLASH_Lock(); // 上锁Flash以防止误操作
}

/**************************************************************************
函数功能：从指定地址开始读出指定长度的数据
入口参数：
  ReadAddr - 起始地址
  pBuffer - 数据指针
  NumToRead - 半字（16位）数
返回  值：无
说明：从指定地址读取指定长度的16位数据，并存储到缓冲区
**************************************************************************/
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u32 NumToRead)   	
{
    u32 i;
    for (i = 0; i < NumToRead; i++)
    {
        pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr); // 读取2个字节
        ReadAddr += 2; // 偏移2个字节
    }
}

/**************************************************************************
函数功能：从指定地址读取32位数据
入口参数：
  addr - 起始地址（必须为4的倍数）
  ptr - 数据指针
  nword - 32位数据个数
返回  值：无
说明：从指定地址读取指定数量的32位数据，并存储到缓冲区
**************************************************************************/
void Read_ST_Flash32(uint32_t addr, uint32_t *ptr, uint16_t nword)
{
    for (uint16_t i = 0; i < nword; i++)
    {
        ptr[i] = *(__IO uint32_t *)addr; // 读取32位数据
        addr += 4; // 偏移4个字节
    }
}
/* 注意地址的选取尽量选在FLASH的末尾，防止被用户代码覆盖 */

/**
 * 函数功能：带地址标记的Flash写入操作
 * 作用：在STM32 FLASH的指定地址写入数据，实现循环存储管理
 * 参数：
 *   address - 写入起始地址（需双字对齐）
 *   ptr     - 数据缓冲区指针（按uint8_t类型处理）
 *   nbyte   - 要写入的字节数
 * 返回值：
 *   1 - 写入失败（参数错误或操作失败）
 *   2 - 写入成功（注意返回值与常规定义不同）
 */
uint8_t Write_ST_Flash_address_mark(uint32_t address, uint8_t *ptr, uint32_t nbyte)
{
    uint32_t sector = 0;      // 存储当前操作扇区号
    uint32_t i = 0;          // 循环计数器
    uint32_t data32;         // 临时存储32位数据
	
    // 字节对齐处理：STM32 F4系列按字(32位)写入，必须4字节对齐
    if (nbyte % 4 != 0)
    {
        // 计算补齐后的字节数（向上取整到最近的4倍数）
        nbyte = nbyte / 4 * 4 + 4;
    }
    
    // 安全保护：单次写入不能超过FLASH扇区大小
    if (nbyte > FLASH_SECTOR_SIZE) return 1;

    // Flash操作准备
    FLASH_Unlock();  // 解除Flash写保护
    
    // 清除所有状态标志，防止之前操作的状态影响
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    // 获取当前地址对应的FLASH扇区
    sector=STMFLASH_GetFlashSector(address);
    
    /* 扇区空间检查策略：
     * 1. 剩余空间不足3倍(数据大小+地址保留空间)
     * 2. 当前地址不在预设操作扇区
     * 满足任一条件则擦除扇区 */
    if((FLASH_SAVE_ADDR+FLASH_SECTOR_SIZE-address)<(unsigned int)(3 * (nbyte + Reserved_Address_Number))||sector!=FLASH_Sector)
    {    
				 // 执行扇区擦除操作
				 FLASH_EraseSector(FLASH_Sector, VoltageRange_3);           
         FLASH_Unlock(); // 第二次解锁（准备写入）
         // 重置指针标识和操作地址到扇区起始
        *(uint32_t *)(ptr) = FLASH_SAVE_ADDR; // 更新指针到起始地址
        address = FLASH_SAVE_ADDR;            // 重置当前写入地址
    }
    else
    {
        // 正常更新指针标识到当前地址
        *(uint32_t *)(ptr) = (uint32_t)address; 
    }
    
    // 数据写入阶段：按32位字进行循环写入
    for (i = 0; i < nbyte; i += 4)  // 每次递增4字节
    {
        data32 = *(uint32_t *)(ptr + i); // 从缓冲区获取32位数据
        // 执行Flash编程操作
        if (FLASH_ProgramWord(address + i, data32) != FLASH_COMPLETE)
        {
            FLASH_Lock(); // 写入失败后锁定Flash
            return 1;
        }
    }
    FLASH_Lock();  // 写入完成后锁定Flash
    return 2;      // 注意：返回码与常规定义不符（建议检查业务逻辑）
}
/**
 * 函数功能：查找最后一次有效写入地址
 * 作用：在FLASH存储区域中循环查找最后有效数据位置
 * 参数：
 *   begin_adress - 搜索起始地址
 *   nbyte        - 单次写入数据块的大小
 * 返回值：
 *   找到的有效地址标记
 */
uint32_t Search_DS2745_Flash_address_mark(uint32_t begin_adress, uint32_t nbyte)  
{
    // 初始化地址标记为FLASH保存区域的起始地址
    uint32_t address_mark = FLASH_SAVE_ADDR; 
    
    // 字节对齐处理（与写入函数保持一致）
    // STM32 F4系列Flash操作要求4字节对齐
    if (nbyte % 4 != 0)
    {
        // 计算方式：取整后补足4字节对齐
        // 例如：nbyte=5 → 5/4=1 → 1*4=4 → 4+4=8
        nbyte = nbyte /4 * 4 + 4;
    }
    
    /* 循环搜索策略说明：
     * 遍历范围：从起始地址到扇区末尾-1的地址空间
     * 步进单位：数据块大小 + 地址保留空间
     * 设计目的：为每个数据块保留地址标记空间 */
    for (int i =0; i <FLASH_SAVE_ADDR+FLASH_SECTOR_SIZE-begin_adress-1; 
         i += nbyte + Reserved_Address_Number)
    {
        // 读取当前块的地址标记（从begin_address+i位置读取4字节）
        Read_ST_Flash32(begin_adress + i, &address_mark, sizeof(address_mark));
        
        // 情况1：首次烧写处理（全FF状态）
        if (address_mark == 0XFFFFFFFF && i ==0) 
        {
            // 初始化地址标记到起始位置
            address_mark = begin_adress; 
            
            // Flash操作准备
            FLASH_Unlock(); // 第一次解锁（准备擦除）
            
            // 擦除整个扇区（确保起始状态干净）
            // VoltageRange_3 表示操作电压范围3（具体根据芯片手册定义）
            FLASH_EraseSector(FLASH_Sector, VoltageRange_3); 
            
            FLASH_Unlock(); // 第二次解锁（准备写入）
            
            // 写入初始地址标记
            Write_ST_Flash_address_mark(address_mark, 
                                      (uint8_t *)&flash_parameter, 
                                      nbyte);
            break; // 退出循环
        }
        // 情况2：后续发现未使用区域（FF状态）
        else if (address_mark == 0XFFFFFFFF) 
        {
            /* 地址计算说明：
             * 当前地址i位置是未使用区域
             * 有效地址应该回退到：当前地址 - 数据块大小 - 地址保留空间
             * 这样得到的地址是前一个有效块的结束位置 */
            address_mark = begin_adress + i - nbyte - Reserved_Address_Number;
            break;
        }
        // 情况3：地址异常处理（超出合法范围）
        else if (address_mark < begin_adress || 
                address_mark > (FLASH_SAVE_ADDR+FLASH_SECTOR_SIZE - 1)) 
        {
            // 重置到预设的安全起始地址
            address_mark = FLASH_SAVE_ADDR; 
            
            // Flash操作流程
            FLASH_Unlock();
            FLASH_EraseSector(FLASH_Sector, VoltageRange_3); // 全扇区擦除
            FLASH_Unlock();
            
            // 写入初始标记
            Write_ST_Flash_address_mark(address_mark, 
                                      (uint8_t *)&flash_parameter, 
                                      nbyte);
            break;
        }
        // 情况4：存储空间耗尽处理（到达扇区末尾）
        // 判断条件：当前地址+数据块+保留空间 >= 扇区末尾地址
        if ((i + nbyte + Reserved_Address_Number) >= 
            (FLASH_SAVE_ADDR+FLASH_SECTOR_SIZE - 1)) 
        {
            // 重置地址到扇区起始
            address_mark=FLASH_SAVE_ADDR;
            
            // 执行全扇区擦除
            FLASH_Unlock();
            FLASH_EraseSector(FLASH_Sector, VoltageRange_3);
            FLASH_Unlock();
            
            // 重新写入初始标记
            Write_ST_Flash_address_mark(address_mark, 
                                      (uint8_t *)&flash_parameter, 
                                      nbyte);
            break;
        }
    }
    // 返回最终找到的有效地址
    return address_mark; 
}


    
/**************************************************************************
函数功能：从Flash读取机械臂关节参数数据
@details 本函数完成以下操作：
    1. 查找最后有效存储地址
    2. 从Flash读取参数结构体
    3. 检测首次启动标志（全0xFFFF）
    4. 初始化或加载关节参数
    5. LCD显示调试信息
@note 重要说明：
    - 使用Flash参数结构体Flash_Parameter存储关节角度
    - 65535（0xFFFF）表示初始未编程状态
    - 需配合Search_DS2745_Flash_address_mark地址查找函数使用
    - LCD显示坐标系统：Y轴以550为基准，每行间隔50像素
**************************************************************************/
void Flash_Read(void)
{
    // LCD显示缓存（50字符容量足够显示32位数值）
    char pStr[50]; 
    
//    /*------------------------------------------------
//    历史调试代码块（已禁用）：
//    - 用于强制擦除指定扇区（FLASH_Sector需定义）
//    - 测试Flash擦写耐久性（压力测试循环）
//    - 预设初始关节角度参数（开发阶段使用）
//    -------------------------------------------------*/
    //历史调试代码（已注释）：
    // 用于强制初始化Flash的测试代码
    FLASH_Unlock();
    FLASH_EraseSector(FLASH_Sector, VoltageRange_3);
    FLASH_Unlock();
    // 测试用初始值设置
    Moveit_Angle1_init=29155;
    Moveit_Angle2_init=27010;
    Moveit_Angle3_init=27010;
    Moveit_Angle4_init=24898;
    // 压力测试循环（已注释）
    for(int i=0;i<10000;i++) { 
        Flash_Write(flash_parameter.flash_address_mark,sizeof(Flash_Parameter));
			  if(i%1000 == 0)
				{
					Delay(0x100);//暂停处,看Flash对应地址结果
				}
    }

    /* 【核心操作1：获取有效存储地址】 
    - FLASH_SAVE_ADDR: 参数存储起始地址（需根据实际Flash布局定义）
    - sizeof(Flash_Parameter): 确保搜索步长与结构体大小一致 */
    flash_parameter.flash_address_mark = Search_DS2745_Flash_address_mark(
                                        FLASH_SAVE_ADDR, 
                                        sizeof(Flash_Parameter));

    /* 【核心操作2：Flash数据读取】
    - Read_ST_Flash32: 32位对齐读取函数（需确保实现）
    - 参数说明：
      (1) flash_address_mark: 有效参数存储地址
      (2) &flash_parameter: 结构体指针强制转换为uint32_t*（需内存对齐）
      (3) sizeof(Flash_Parameter): 按4字节倍数读取 */
    Read_ST_Flash32(flash_parameter.flash_address_mark, 
                   (uint32_t *)&flash_parameter, 
                   sizeof(Flash_Parameter));

    /* 【首次运行检测】 
    - 判断标准：所有关节参数均为0xFFFF（Flash擦除后状态）
    - 初始化策略：将关节角度归零（根据实际需求可修改） */
    if(flash_parameter.Flash_Moveit_Angle1_init==65535 &&
       flash_parameter.Flash_Moveit_Angle2_init==65535 &&
       flash_parameter.Flash_Moveit_Angle3_init==65535 &&
       flash_parameter.Flash_Moveit_Angle4_init==65535) 
    {
        // 关节角度初始化（开发阶段可设置非零默认值）
        Moveit_Angle1_init=0; // 关节1初始角度
        Moveit_Angle2_init=0; // 关节2初始角度
        Moveit_Angle3_init=0; // 关节3初始角度
        Moveit_Angle4_init=0; // 关节4初始角度
        
        // LCD显示当前存储地址（调试用，正式版可删除）
        sprintf(pStr,"Addr:%08X",flash_parameter.flash_address_mark);
        ILI9806G_DispString_EN_CH(200,550,pStr); // X=200,Y=550基准线
    }
    else // 正常读取模式
    {        
        /* 【参数加载】从结构体到工作变量
        - Moveit_AngleX_init: 全局关节角度变量（用于运动控制）
        - Flash_前缀参数：Flash存储的持久化版本 */
        Moveit_Angle1_init=flash_parameter.Flash_Moveit_Angle1_init; 
        Moveit_Angle2_init=flash_parameter.Flash_Moveit_Angle2_init; 
        Moveit_Angle3_init=flash_parameter.Flash_Moveit_Angle3_init; 
        Moveit_Angle4_init=flash_parameter.Flash_Moveit_Angle4_init; 
        
        /* 【调试信息显示系统】
        - 显示层级：地址标记 -> 各关节参数
        - 坐标布局：Y轴以50像素为间隔向下排列 */
        // 地址显示（十六进制格式）
        sprintf(pStr,"Addr:%08X",flash_parameter.flash_address_mark);
        ILI9806G_DispString_EN_CH(200,550,pStr); 
        
        // 关节1参数显示（十进制）
        sprintf(pStr,"J1:%05d",(int)Moveit_Angle1_init);
        ILI9806G_DispString_EN_CH(200,600,pStr); // Y=600
        
        // 关节2参数显示
        sprintf(pStr,"J2:%05d",(int)Moveit_Angle2_init);
        ILI9806G_DispString_EN_CH(200,650,pStr); // Y=650
        
        // 关节3参数显示
        sprintf(pStr,"J3:%05d",(int)Moveit_Angle3_init);
        ILI9806G_DispString_EN_CH(200,700,pStr); // Y=700
        
        // 关节4参数显示
        sprintf(pStr,"J4:%05d",(int)Moveit_Angle4_init);
        ILI9806G_DispString_EN_CH(200,750,pStr); // Y=750
    }
}    


/**************************************************************************
函数功能：向Flash写入指定数据
入口参数：无
返回  值：无
**************************************************************************/
void Flash_Write(uint32_t begin_adress, uint32_t nbyte)
{
    // 参数格式转换与存储（关键操作）
    flash_parameter.Flash_Moveit_Angle1_init=(u16)Moveit_Angle1_init; // 浮点转定点存储
    flash_parameter.Flash_Moveit_Angle2_init=(u16)Moveit_Angle2_init; 
    flash_parameter.Flash_Moveit_Angle3_init=(u16)Moveit_Angle3_init; 
    flash_parameter.Flash_Moveit_Angle4_init=(u16)Moveit_Angle4_init; 
    
    // Flash写入操作（核心功能）
    Write_ST_Flash_address_mark(begin_adress,
                               (uint8_t *)&flash_parameter,
                               sizeof(Flash_Parameter));   
    
    // 地址指针更新（为下次写入准备）
	flash_parameter.flash_address_mark = (begin_adress + nbyte + Reserved_Address_Number-ADDR_FLASH_SECTOR_5)<FLASH_SECTOR_SIZE ? begin_adress + nbyte + Reserved_Address_Number:ADDR_FLASH_SECTOR_5;
}




















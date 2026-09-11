/**
  ******************************************************************************
  * @file    bsp_spi_flash.c
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
#include "bsp_spi_flash.h"
/**************************************************************************
全局超时计数器：
- 作用：为SPI操作提供超时保护机制
- 特性：volatile修饰确保中断/主程序可见性
- 初始值：SPIT_LONG_TIMEOUT（建议值5000≈500ms@1MHz时钟）
- 访问范围：本文件内有效（static限定）
**************************************************************************/
static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

/**************************************************************************
超时回调函数原型声明：
- 功能：统一处理SPI通信超时事件
- 参数：errorCode标识超时类型（0:发送超时 1:接收超时...）
- 返回值：固定返回0简化错误处理流程
- 设计说明：
  1. static限定本文件内访问
  2. 实际实现在文件后部（约行545）
  3. 错误码规范参考后续实现注释
**************************************************************************/
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

/**************************************************************************
函数功能：SPI Flash控制器及GPIO初始化配置
入口参数：
    无
返回  值：
    无
说明：
    本函数完成SPI外设和GPIO引脚的初始化配置，包括时钟使能、引脚复用配置、
    GPIO模式设置、SPI工作参数配置等，建立与FLASH芯片的通信基础
**************************************************************************/
void SPI_FLASH_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;  // SPI初始化结构体
    GPIO_InitTypeDef GPIO_InitStructure; // GPIO初始化结构体
  
    /* 使能 FLASH_SPI 及GPIO 时钟 */
    /* 同时使能SCK、MISO、MOSI、CS引脚所在GPIO组的时钟 */
    RCC_AHB1PeriphClockCmd(FLASH_SPI_SCK_GPIO_CLK | FLASH_SPI_MISO_GPIO_CLK 
                         | FLASH_SPI_MOSI_GPIO_CLK | FLASH_CS_GPIO_CLK, ENABLE);

    /* 使能SPI外设时钟 */
    FLASH_SPI_CLK_INIT(FLASH_SPI_CLK, ENABLE);
 
    // 配置引脚复用功能，将GPIO映射到SPI外设
    GPIO_PinAFConfig(FLASH_SPI_SCK_GPIO_PORT,  FLASH_SPI_SCK_PINSOURCE,  FLASH_SPI_SCK_AF);  // SCK引脚复用
    GPIO_PinAFConfig(FLASH_SPI_MISO_GPIO_PORT, FLASH_SPI_MISO_PINSOURCE, FLASH_SPI_MISO_AF); // MISO引脚复用
    GPIO_PinAFConfig(FLASH_SPI_MOSI_GPIO_PORT, FLASH_SPI_MOSI_PINSOURCE, FLASH_SPI_MOSI_AF); // MOSI引脚复用
  
    /* SCK引脚配置 */
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;    // 选择SCK引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    // 最大输出速度50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      // 推挽输出类型
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    // 无上拉下拉电阻
    GPIO_Init(FLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure); // 应用配置
  
    /* MISO引脚配置（复用参数继承之前的配置） */
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;  
    GPIO_Init(FLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
    /* MOSI引脚配置 */
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
    GPIO_Init(FLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

    /* CS片选引脚配置 */
    GPIO_InitStructure.GPIO_Pin = FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       // 普通输出模式
    GPIO_Init(FLASH_CS_GPIO_PORT, &GPIO_InitStructure); 

    /* 初始化CS引脚为高电平（无效状态） */
    SPI_FLASH_CS_HIGH();

    /* SPI工作参数配置 */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 全双工模式
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;        // 主机模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;    // 8位数据帧
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;         // 时钟极性：空闲时高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        // 时钟相位：第二个边沿采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;           // 软件控制NSS信号
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; // 预分频系数2
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  // 高位先传
    SPI_InitStructure.SPI_CRCPolynomial = 7;             // CRC多项式（实际未使用CRC）
    SPI_Init(FLASH_SPI, &SPI_InitStructure);             // 应用SPI配置

    /* 使能SPI外设 */
    SPI_Cmd(FLASH_SPI, ENABLE);
}

/**************************************************************************
函数功能：擦除指定SPI Flash扇区
入口参数：
    SectorAddr：要擦除的扇区地址（32位地址）
返回  值：
    无
说明：
    1. 执行扇区擦除前需要先使能写操作
    2. 使用W25X_SectorErase命令（4KB扇区擦除）
    3. 地址需要分解为3个字节发送
    4. 擦除操作需要等待完成（典型时间400ms）
**************************************************************************/
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
    /* 发送写使能命令（解锁FLASH写操作） */
    SPI_FLASH_WriteEnable();
    
    /* 等待上次操作完成（防止冲突） */
    SPI_FLASH_WaitForWriteEnd();

    /* 开始擦除操作 */
    SPI_FLASH_CS_LOW();                     // 拉低CS选中芯片
    
    SPI_FLASH_SendByte(W25X_SectorErase);   // 发送扇区擦除指令0x20
    
    /* 分解32位地址为3个字节（符合SPI Flash地址格式） */
    SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16); // 地址高位字节
    SPI_FLASH_SendByte((SectorAddr & 0x00FF00) >> 8);  // 地址中位字节
    SPI_FLASH_SendByte(SectorAddr & 0x0000FF);         // 地址低位字节
    
    SPI_FLASH_CS_HIGH();                    // 释放CS结束传输
    
    /* 等待擦除操作完成（查询状态寄存器） */
    SPI_FLASH_WaitForWriteEnd();
}

/**************************************************************************
函数功能：执行整片擦除操作（擦除整个FLASH芯片）
入口参数：
    无
返回  值：
    无
说明：
    1. 需要先发送写使能命令解锁擦除操作
    2. 使用W25X_ChipErase指令（通常对应0xC7或0x60指令）
    3. 整片擦除时间较长（典型值需要几十秒）
    4. 擦除后所有数据变为0xFF
**************************************************************************/
void SPI_FLASH_BulkErase(void)
{
    /* 发送写使能命令（0x06），解除写保护 */
    SPI_FLASH_WriteEnable();

    /* 开始整片擦除流程 */
    SPI_FLASH_CS_LOW();                // 拉低CS片选信号开始通信
    SPI_FLASH_SendByte(W25X_ChipErase);// 发送整片擦除指令（通常为0xC7）
    SPI_FLASH_CS_HIGH();               // 释放CS结束指令传输

    /* 等待擦除操作完成（通过轮询状态寄存器） */
    SPI_FLASH_WaitForWriteEnd();       // 典型等待时间40-200秒
}

/**************************************************************************
函数功能：按页写入数据到FLASH（页大小为256字节）
入口参数：
    pBuffer：待写入数据缓冲区的指针
    WriteAddr：写入起始地址（32位地址）
    NumByteToWrite：写入字节数（必须≤256）
返回  值：
    无
说明：
    1. 写入前必须确保目标区域已擦除（擦除后为0xFF）
    2. 单次写入不能跨页，超限部分会被截断
    3. 使用页编程指令（通常0x02）
**************************************************************************/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    /* 发送写使能命令 */
    SPI_FLASH_WriteEnable();

    /* 开始页编程操作 */
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_PageProgram); // 发送页编程指令0x02
    
    /* 分解32位地址为3个字节 */
    SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16); // 地址高位
    SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);    // 地址中位
    SPI_FLASH_SendByte(WriteAddr & 0xFF);            // 地址低位

    /* 处理超限写入（FLASH页大小为256字节） */
    if(NumByteToWrite > SPI_FLASH_PerWritePageSize) {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;  // 强制截断为最大页大小
        FLASH_ERROR("SPI_FLASH_PageWrite too large!"); // 记录错误日志
    }

    /* 逐字节写入数据 */
    while (NumByteToWrite--) {
        SPI_FLASH_SendByte(*pBuffer); // 发送数据字节
        pBuffer++;                    // 指针递增
    }

    SPI_FLASH_CS_HIGH();             // 结束传输
    SPI_FLASH_WaitForWriteEnd();     // 等待写入完成（典型时间1-5ms）
}

/**************************************************************************
函数功能：大数据量缓冲写入（自动处理跨页写入）
入口参数：
    pBuffer：数据缓冲区指针
    WriteAddr：起始写入地址
    NumByteToWrite：总写入字节数
返回  值：
    无
说明：
    1. 自动处理地址对齐和跨页写入
    2. 分三种情况处理：地址对齐、小数据量、大数据量
    3. 最大支持65535字节写入
**************************************************************************/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    /* 计算地址偏移量（相对于页起始地址） */
    Addr = WriteAddr % SPI_FLASH_PageSize; // 获取页内偏移量（0-255）
    count = SPI_FLASH_PageSize - Addr;     // 当前页剩余可写空间

    /* 计算完整页数和剩余字节数 */
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;  // 完整页数
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize; // 剩余字节

    /* 情况1：地址刚好页对齐 */
    if (Addr == 0) {
        if (NumOfPage == 0) { // 小数据量（不超过一页）
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        } else { // 大数据量
            // 先写入完整页
            while (NumOfPage--) {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize; // 地址递增
                pBuffer += SPI_FLASH_PageSize;    // 缓冲区指针递增
            }
            // 写入剩余字节
            if(NumOfSingle != 0) {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    } 
    /* 情况2：地址未页对齐 */
    else { 
        if (NumOfPage == 0) { // 总数据量不超过一页
            if (NumOfSingle > count) { // 需要拆分为两次写入
                temp = NumOfSingle - count;
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, count); // 写满当前页
                WriteAddr += count;
                pBuffer += count;
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp); // 写入剩余数据
            } else { // 单次写入即可
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
            }
        } else { // 大数据量跨页写入
            NumByteToWrite -= count; // 扣除首部不完整部分
            NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            // 写入首部不完整页
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
            WriteAddr += count;
            pBuffer += count;

            // 写入完整页
            while (NumOfPage--) {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr += SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            // 写入尾部剩余数据
            if (NumOfSingle != 0) {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

/**************************************************************************
函数功能：从FLASH读取数据到缓冲区
入口参数：
    pBuffer：数据存储缓冲区指针
    ReadAddr：读取起始地址
    NumByteToRead：读取字节数
返回  值：
    无
说明：
    1. 使用连续读指令（通常0x03）
    2. 支持跨页读取
    3. 读取后指针自动递增
**************************************************************************/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    SPI_FLASH_CS_LOW(); // 开始通信
    
    // 发送读指令0x03
    SPI_FLASH_SendByte(W25X_ReadData);
    
    // 发送24位地址
    SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16); // 地址高位
    SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);    // 地址中位
    SPI_FLASH_SendByte(ReadAddr & 0xFF);            // 地址低位

    // 连续读取数据
    while (NumByteToRead--) {
        *pBuffer = SPI_FLASH_SendByte(Dummy_Byte); // 发送哑元数据获取响应
        pBuffer++; // 缓冲区指针递增
    }

    SPI_FLASH_CS_HIGH(); // 结束读取
}

/**************************************************************************
函数功能：读取FLASH芯片的JEDEC标准ID
入口参数：
    无
返回  值：
    24位组合ID（厂商ID + 容量ID）
说明：
    1. 使用JEDEC ID指令（通常0x9F）
    2. ID格式：厂商ID(1字节)+器件类型(1字节)+容量代码(1字节)
    3. 例如：Winbond W25Q64的ID为0xEF4017
**************************************************************************/
u32 SPI_FLASH_ReadID(void)
{
    u32 Temp = 0;
    
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_JedecDeviceID); // 发送JEDEC ID指令0x9F
    
    // 读取3字节ID
    Temp  = SPI_FLASH_SendByte(Dummy_Byte) << 16; // 厂商ID（如0xEF）
    Temp |= SPI_FLASH_SendByte(Dummy_Byte) << 8;  // 器件类型（如0x40）
    Temp |= SPI_FLASH_SendByte(Dummy_Byte);       // 容量代码（如0x17）
    
    SPI_FLASH_CS_HIGH();
    
    return Temp; // 返回组合后的24位ID
}

/**************************************************************************
函数功能：读取FLASH器件的唯一设备ID（单个字节）
入口参数：
    无
返回  值：
    8位设备ID（与JEDEC ID不同）
说明：
    1. 使用RDID指令（通常为0xAB或0x9F）
    2. 需要发送3个哑元字节满足时序要求
    3. 返回的ID用于识别具体存储容量版本
**************************************************************************/
u32 SPI_FLASH_ReadDeviceID(void)
{
    u32 Temp = 0;

    /* 启动SPI通信 */
    SPI_FLASH_CS_LOW();

    /* 发送读设备ID指令 */
    SPI_FLASH_SendByte(W25X_DeviceID);  // 0xAB或0x90指令
    
    /* 发送3个哑元时钟以获取ID */
    SPI_FLASH_SendByte(Dummy_Byte);  // 维持时钟时序
    SPI_FLASH_SendByte(Dummy_Byte);  // 生成足够时钟周期
    SPI_FLASH_SendByte(Dummy_Byte);  // 制造商要求格式
    
    /* 读取有效ID字节 */
    Temp = SPI_FLASH_SendByte(Dummy_Byte);

    /* 结束通信 */
    SPI_FLASH_CS_HIGH();

    return Temp;  // 如W25Q64返回0x17
}

/**************************************************************************
函数功能：初始化连续读取序列（保持CS低电平状态）
入口参数：
    ReadAddr：起始读取地址
返回  值：
    无
说明：
    1. 启动后持续保持片选，支持无限连续读取
    2. 必须与ReadByte配合使用完成读取周期
    3. 适合大数据块连续读取（提高传输效率）
**************************************************************************/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
    /* 激活片选信号 */
    SPI_FLASH_CS_LOW();
    
    /* 发送连续读指令0x03 */
    SPI_FLASH_SendByte(W25X_ReadData);
    
    /* 发送24位地址（高位优先） */
    SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16); // 地址字节1
    SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);    // 地址字节2
    SPI_FLASH_SendByte(ReadAddr & 0xFF);             // 地址字节3
    
    /* 保持CS低电平以继续数据传输 */
}

/**************************************************************************
函数功能：从SPI总线读取单个字节（需在连续读序列中调用）
入口参数：
    无
返回  值：
    接收到的数据字节
说明：
    1. 必须在StartReadSequence后调用
    2. 每次读取自动递增地址指针
    3. 全双工通信时需发送哑元数据获取响应
**************************************************************************/
u8 SPI_FLASH_ReadByte(void)
{
    return (SPI_FLASH_SendByte(Dummy_Byte));  // 发送0xFF获取返回数据
}

/**************************************************************************
函数功能：SPI全双工数据收发核心函数
入口参数：
    byte：待发送的字节数据
返回  值：
    接收到的字节数据
说明：
    1. 含双缓冲超时检测机制
    2. 超时时间由SPIT_FLAG_TIMEOUT定义
    3. 错误码：0-TXE超时 1-RXNE超时
**************************************************************************/
u8 SPI_FLASH_SendByte(u8 byte)
{
    SPITimeout = SPIT_FLAG_TIMEOUT;  // 重载超时计数器

    /* 等待发送缓冲区就绪（TXE标志） */
    while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET) {
        if((SPITimeout--) == 0) 
            return SPI_TIMEOUT_UserCallback(0);  // 发送超时错误
    }

    /* 写入待发送数据 */
    SPI_I2S_SendData(FLASH_SPI, byte);

    SPITimeout = SPIT_FLAG_TIMEOUT;  // 重置超时计数器

    /* 等待接收缓冲区就绪（RXNE标志） */
    while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET) {
        if((SPITimeout--) == 0) 
            return SPI_TIMEOUT_UserCallback(1);  // 接收超时错误
    }

    /* 读取接收数据 */
    return SPI_I2S_ReceiveData(FLASH_SPI);  // 返回读取值
}

/**************************************************************************
函数功能：16位数据收发（用于扩展传输模式）
入口参数：
    HalfWord：待发送的16位数据
返回  值：
    接收到的16位数据
说明：
    1. 主要用于Dual/Quad SPI模式
    2. 超时错误码：2-TXE超时 3-RXNE超时
**************************************************************************/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
    SPITimeout = SPIT_FLAG_TIMEOUT;

    /* 等待发送缓冲区空 */
    while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET) {
        if((SPITimeout--) == 0) 
            return SPI_TIMEOUT_UserCallback(2);
    }

    /* 发送16位数据 */
    SPI_I2S_SendData(FLASH_SPI, HalfWord);

    SPITimeout = SPIT_FLAG_TIMEOUT;

    /* 等待接收完成 */
    while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET) {
        if((SPITimeout--) == 0) 
            return SPI_TIMEOUT_UserCallback(3);
    }

    return SPI_I2S_ReceiveData(FLASH_SPI);  // 返回接收数据
}

/**************************************************************************
函数功能：使能FLASH写操作（发送0x06指令）
入口参数：
    无
返回  值：
    无
说明：
    1. 擦除/编程前必须执行
    2. 写使能锁存直至状态寄存器变更
    3. 有效时间：直到WEL位被清除
**************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_WriteEnable);  // 0x06指令
    SPI_FLASH_CS_HIGH();
}

/**************************************************************************
函数功能：等待编程/擦除操作完成
入口参数：
    无
返回  值：
    无
说明：
    1. 轮询状态寄存器BUSY位（WIP_Flag）
    2. 超时保护防止死锁
    3. 典型等待时间：页写入1-5ms，扇区擦除100-400ms
**************************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
    u8 FLASH_Status = 0;
    
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_ReadStatusReg);  // 0x05指令

    SPITimeout = SPIT_FLAG_TIMEOUT;  // 典型值5000（约500ms）
    
    do {
        FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);  // 读取状态
        
        if((SPITimeout--) == 0) {
            SPI_TIMEOUT_UserCallback(4);  // 等待超时错误
            break;
        }
    } while ((FLASH_Status & WIP_Flag) == SET);  // 检查BUSY位

    SPI_FLASH_CS_HIGH();
}

/**************************************************************************
函数功能：进入深度掉电模式（功耗<1μA）
入口参数：
    无
返回  值：
    无
说明：
    1. 发送0xB9指令
    2. 进入后除WAKEUP外不响应指令
    3. 恢复时间tRES1（通常3μs~20ms）
**************************************************************************/
void SPI_Flash_PowerDown(void)   
{ 
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_PowerDown);  // 0xB9指令
    SPI_FLASH_CS_HIGH();
}   

/**************************************************************************
函数功能：从掉电模式唤醒
入口参数：
    无
返回  值：
    无
说明：
    1. 发送0xAB指令
    2. 需要等待tRES1恢复时间
    3. 唤醒后需重新初始化时序
**************************************************************************/
void SPI_Flash_WAKEUP(void)   
{
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_ReleasePowerDown);  // 0xAB指令
    SPI_FLASH_CS_HIGH();                       
}   

/**************************************************************************
函数功能：超时统一回调处理
入口参数：
    errorCode：错误类型代码
返回  值：
    默认返回0
说明：
    错误代码对应表：
    0 - 发送缓冲区等待超时
    1 - 接收缓冲区等待超时
    2 - 16位发送超时
    3 - 16位接收超时
    4 - Flash忙状态超时
**************************************************************************/
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
    FLASH_ERROR("SPI操作超时 [代码%d]", errorCode);
    /* 建议扩展处理：
    1. 复位SPI外设
    2. 重新初始化Flash
    3. 记录错误日志 */
    return 0;
}

   
/*********************************************END OF FILE**********************/

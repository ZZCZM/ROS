/**
  ******************************************************************************
  * @file    I2C.c
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
#include "I2C.h"
/**************************************************************************
函数功能：I2C总线GPIO初始化（用于MPU6050通信）
@details 本函数完成以下配置：
    1. 使能GPIOB时钟（SCL/SDA所在端口）
    2. 配置PB6(SCL)和PB7(SDA)为推挽输出模式
    3. 设置GPIO高速模式（100MHz）
    4. 启用内部上拉电阻
    5. 初始化总线为高电平（I2C空闲状态）
@note 重要说明：
    - 采用软件模拟I2C时序（GPIO电平手动控制）
    - 总线空闲时SCL和SDA必须保持高电平
    - 推挽输出+上拉实现准双向IO特性
    - 适用于标准模式（100kHz）和快速模式（400kHz）
**************************************************************************/
void I2C_GPIOInit(void)
{
    /* GPIO配置结构体声明 */
    GPIO_InitTypeDef  GPIO_InitStructure;  // STM32 GPIO初始化结构体

    /* 【时钟使能阶段】 */
    // 使能GPIOB时钟（SCL_PIN和SDA_PIN所在的端口）
    // SCL_GPIO_CLK 应定义为 RCC_AHB1Periph_GPIOB
    RCC_AHB1PeriphClockCmd(SCL_GPIO_CLK, ENABLE);

    /* 【GPIO参数配置】 */
    // 同时配置SCL和SDA引脚（PB8和PB9）
    GPIO_InitStructure.GPIO_Pin = SCL_PIN | SDA_PIN;  // 使用位或操作组合引脚
    
    // 配置为通用输出模式（软件完全控制电平变化）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      
    
    /* 推挽输出配置说明：
       - 输出0时：强下拉（直接接地）
       - 输出1时：弱上拉（通过内部上拉电阻）*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     
    
    /* 高速模式选择：
       - 100MHz驱动能力：确保快速上升沿
       - 实际通信速率由软件延时决定 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    
    /* 上拉电阻启用：
       - 补偿总线电容，确保快速上升沿
       - 当引脚输出高电平时呈现约40KΩ上拉 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       
    
    // 应用配置到GPIOB端口（SCL_PORT应定义为GPIOB）
    GPIO_Init(SCL_PORT, &GPIO_InitStructure);           

    /* 【总线初始化】 */
    // 设置SCL和SDA为高电平（总线空闲状态）
    // IIC_SCL/IIC_SDA应为引脚宏定义，例如：
    // #define IIC_SCL  GPIOB->BSRRH = GPIO_Pin_6
    // #define IIC_SDA  GPIOB->BSRRH = GPIO_Pin_7
    IIC_SCL = 1;  // 等同于 GPIO_SetBits(GPIOB, GPIO_Pin_6)
    IIC_SDA = 1;  // 等同于 GPIO_SetBits(GPIOB, GPIO_Pin_7)
}
/**
 * 函数功能：生成I2C起始条件信号
 * 协议时序：
 *   ┌───┐     ┌───┐
 * SCL   ───┘   └───┘
 * SDA   ─────────┐
 *                └───
 *                ↑ 起始条件
 * 时序要求：
 * 1. tSU;STA（起始条件建立时间）≥4.7us（标准模式）
 * 2. tHD;STA（起始条件保持时间）≥4us
 * 错误检测：
 * - 总线占用检测（SDA未释放时中止操作）
 * - 起始信号有效性验证
 */
void I2C_Start(void)
{
    /* 阶段0：总线初始化 */
    SDA_OUT();       // 配置SDA为推挽输出（例：GPIOB_CRL寄存器配置）
    IIC_SDA = 1;     // 释放SDA线（高电平表示总线空闲）

    /* 总线状态检测（防止多主冲突） */
    if (!READ_SDA)   // 检测到SDA被拉低（总线被占用）
        return;      // 中止起始信号生成（实际工程应增加重试机制）

    /* 阶段1：建立起始条件 */
    IIC_SCL = 1;     // 时钟线置高（准备生成起始条件）
    delay_us(1);     // 保持tHIGH时钟高电平时间（≥4us）
    
    /* 阶段2：生成起始条件（SCL高时SDA↓） */
    IIC_SDA = 0;     // 产生下降沿（起始条件核心时序）
    if (READ_SDA)    // 验证SDA是否成功拉低（检测总线竞争）
        return;      // 如果SDA仍为高，说明总线控制失败
    
    /* 阶段3：保持起始条件 */
    delay_us(1);     // 保持tHD;STA时间（≥4us）
    IIC_SCL = 0;     // 时钟线拉低，进入数据传输阶段

    /* 此时总线状态：
     - SCL低电平（允许数据变化）
     - SDA低电平（保持起始状态）
     后续可开始发送地址字节 */
}

/**
 * 函数功能：生成I2C停止条件信号
 * 协议时序：
 *   ┌───┐     ┌───┐
 * SCL   ───┘   └───┘
 * SDA   ──┐         ┌───
 *         └───┬─────┘
 *             ↑ 停止条件
 * 时序要求：
 * 1. SCL高电平期间SDA产生上升沿
 * 2. tSU:STO（停止条件建立时间）≥4us（标准模式）
 * 作用效果：
 * - 终止当前通信会话
 * - 释放总线控制权
 */
void I2C_Stop(void)
{
    /* 配置SDA线为输出模式（主设备控制） */
    SDA_OUT();  // 宏展开：GPIOx->CRL &= 0X0FFFFFFF; GPIOx->CRL|=0X30000000;

    /* 阶段1：准备停止条件 */
    IIC_SCL = 0;  // 确保时钟线为低，允许改变数据线
    IIC_SDA = 0;  // 预置数据线低电平（为上升沿做准备）
    delay_us(1);  // 保持tVD;DAT数据有效时间（≥0us）

    /* 阶段2：生成停止条件时序 */
    IIC_SCL = 1;  // 时钟线先置高（满足停止条件时序要求）
    delay_us(1);  // 保持tSU;STO停止条件建立时间（标准模式≥4us）
    
    /* 阶段3：产生停止条件（SCL高电平期间SDA↑）*/
    IIC_SDA = 1;  // 在SCL高电平期间产生上升沿→停止条件
    delay_us(1);  // 保持tBUF总线空闲时间（≥4.7us）

    /* 总线状态：
    此时SCL和SDA均为高电平，进入总线空闲状态
    其他主设备可检测到总线空闲后发起通信 */
}

/**
 * 函数功能：I2C总线应答等待检测
 * 返回值：
 *   - true(1): 成功接收到从设备应答
 *   - false(0): 应答超时或总线错误
 * 协议时序：
 *   1. SDA切换为输入模式
 *   2. 主设备释放SDA线（置高）
 *   3. 产生时钟脉冲
 *   4. 检测SDA线电平状态
 * 注意事项：
 *   - 超时后会主动发送停止信号复位总线
 *   - 典型超时时间：50*(1us+1us)=100us（实际包含循环开销）
 */
bool I2C_WaiteForAck(void)
{
    u8 ucErrTime = 0;  // 超时计数器（8位无符号，范围0-255）

    /* 配置SDA线为输入模式 */
    SDA_IN();      // 调用硬件层GPIO模式配置（通常设置为浮空输入）

    /* 主设备释放SDA线（高电平）*/
    IIC_SDA = 1;   // 设置输出寄存器为高，结合输入模式实现总线释放

    /* 建立时序-保持t_VD;DAT数据有效时间 */
    delay_us(1);   // 满足I2C协议t_SU;DAT ≥ 250ns的要求

    /* 产生时钟脉冲（SCL上升沿）*/
    IIC_SCL = 1;   // 时钟线置高，从设备应在此时钟高电平期间读取SDA
    delay_us(1);   // 保持时钟高电平时间满足t_HIGH ≥ 4us（标准模式）

    /* 检测应答窗口（SCL高电平期间）*/
    while (READ_SDA)  // 持续检测SDA线电平状态（0表示收到ACK）
    {
        ucErrTime++;
        if (ucErrTime > 50) // 超时阈值（约50us+）
        {
            /* 总线异常处理 */
            I2C_Stop();  // 发送停止条件复位总线：SCL↑→SDA↑
            return false; // 返回错误码（可能原因：从设备无响应/总线冲突）
        }
        delay_us(1);  // 检测间隔周期（降低CPU占用率）
    }

    /* 完成应答检测 */
    IIC_SCL = 0;     // 时钟线置低，准备后续操作（保持总线有效状态）
    return true;     // 成功标志（检测到SDA被从设备拉低）
}

/**************************************************************************
Function: IIC response
Input   : none
Output  : none
函数功能：IIC应答
入口参数：无
返回  值：无
**************************************************************************/
void I2C_Ack(void)
{
    IIC_SCL = 0; // 设置SCL为低电平
    SDA_OUT(); // SDA线输出
    IIC_SDA = 0; // 设置SDA为低电平（应答信号）
    delay_us(1); // 延时1微秒
    IIC_SCL = 1; // 设置SCL为高电平
    delay_us(1); // 延时1微秒
    IIC_SCL = 0; // 设置SCL为低电平
}

/**************************************************************************
Function: IIC don't reply
Input   : none
Output  : none
函数功能：IIC不应答
入口参数：无
返回  值：无
**************************************************************************/
void I2C_NAck(void)
{
    IIC_SCL = 0; // 设置SCL为低电平
    SDA_OUT(); // SDA线输出
    IIC_SDA = 1; // 设置SDA为高电平（不应答信号）
    delay_us(1); // 延时1微秒
    IIC_SCL = 1; // 设置SCL为高电平
    delay_us(1); // 延时1微秒
    IIC_SCL = 0; // 设置SCL为低电平
}
/**************************************************************************
函数功能：通过I2C总线向指定设备的指定寄存器写入一个位
入口参数：
    DevAddr：目标设备的I2C地址
    RegAddr：目标寄存器地址
    BitNum：要写入的位号（从0开始）
    Data：要写入的位值（0或1）
返回  值：
    成功写入时返回true
**************************************************************************/
bool I2C_WriteOneBit(uint8_t DevAddr, uint8_t RegAddr, uint8_t BitNum, uint8_t Data)
{
    uint8_t Dat;

    // 读取指定设备和寄存器的当前值
    Dat = I2C_ReadOneByte(DevAddr, RegAddr);

    // 根据Data的值修改指定的位
    // 如果Data不为0，则将Dat的BitNum位置1，否则将BitNum位置0
    Dat = (Data != 0) ? (Dat | (1 << BitNum)) : (Dat & ~(1 << BitNum));

    // 将修改后的值写回指定设备和寄存器
    I2C_WriteOneByte(DevAddr, RegAddr, Dat);

    return true;
}

/**************************************************************************
函数功能：通过I2C总线向指定设备的指定寄存器写入多个连续的位
入口参数：
    DevAddr：目标设备的I2C地址
    RegAddr：目标寄存器地址
    BitStart：起始位号（从0开始）
    Length：要写入的位数
    Data：要写入的位值（在BitStart位置开始的Length位）
返回  值：
    成功写入时返回true
**************************************************************************/
bool I2C_WriteBits(uint8_t DevAddr, uint8_t RegAddr, uint8_t BitStart, uint8_t Length, uint8_t Data)
{
    uint8_t Dat, Mask;

    // 读取指定设备和寄存器的当前值
    Dat = I2C_ReadOneByte(DevAddr, RegAddr);

    // 创建掩码，保留不需要修改的位
    // 该掩码的BitStart位之前和BitStart+Length位之后的位被设置为1，其余位为0
    Mask = (0xFF << (BitStart + 1)) | 0xFF >> ((8 - BitStart) + Length - 1);

    // 将Data左移，使其对齐到BitStart位置
    Data <<= (8 - Length);

    // 将Data右移，使其对齐到BitStart位置
    // 这一步是为了将Data的高Length位对齐到BitStart位开始的位置
    Data >>= (7 - BitStart);

    // 应用掩码，保留不需要修改的位，并应用修改后的位
    Dat &= Mask; // 使用掩码保留不需要修改的位
    Dat |= Data; // 将修改后的位应用到Dat中

    // 将修改后的值写回指定设备和寄存器
    I2C_WriteOneByte(DevAddr, RegAddr, Dat);

    return true;
}

/**************************************************************************
函数功能：通过I2C总线发送一个字节的数据
入口参数：
    Data：要发送的字节数据
返回  值：
    无
**************************************************************************/
void I2C_WriteByte(uint8_t Data)
{
    u8 t;   
    SDA_OUT(); // 设置SDA为输出模式
    IIC_SCL = 0; // 拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {              
        IIC_SDA = (Data & 0x80) >> 7; // 发送当前字节的最高位
        Data <<= 1; // 左移Data，以便发送下一个位
	    delay_us(1); // 延时1微秒
	    IIC_SCL = 1; // 设置时钟为高电平，通知从设备读取数据
	    delay_us(1); 
	    IIC_SCL = 0; // 设置时钟为低电平，准备发送下一个位
	    delay_us(1);
    }	 
}

/**************************************************************************
函数功能：通过I2C总线向指定设备的指定寄存器写入一个字节的数据
入口参数：
    DevAddr：目标设备的I2C地址
    RegAddr：目标寄存器地址
    Data：要写入的字节数据
返回  值：
    成功写入时返回1
**************************************************************************/
u8 I2C_WriteOneByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t Data)
{
    I2C_Start(); // 发送I2C起始信号
    I2C_WriteByte(DevAddr | I2C_Direction_Transmitter); // 发送设备地址和写入方向位
    I2C_WaiteForAck(); // 等待从设备的ACK响应
    I2C_WriteByte(RegAddr); // 发送寄存器地址
    I2C_WaiteForAck(); // 等待从设备的ACK响应
    I2C_WriteByte(Data); // 发送数据字节
    I2C_WaiteForAck(); // 等待从设备的ACK响应
    I2C_Stop(); // 发送I2C停止信号

    return 1; // 返回1表示成功写入
}

/**************************************************************************
函数功能：通过I2C总线向指定设备的指定寄存器写入多个字节的数据
入口参数：
    DevAddr：目标设备的I2C地址
    RegAddr：目标寄存器地址
    Num：要写入的字节数
    *pBuff：指向要写入的数据缓冲区的指针
返回  值：
    成功写入时返回true，否则返回false
**************************************************************************/
bool I2C_WriteBuff(uint8_t DevAddr, uint8_t RegAddr, uint8_t Num, uint8_t *pBuff)
{
    uint8_t i;

    // 检查参数的有效性
    if(0 == Num || NULL == pBuff)
    {
        return false; // 如果要写入的字节数为0或缓冲区指针为空，则返回false
    }
	
    I2C_Start(); // 发送I2C起始信号
    I2C_WriteByte(DevAddr | I2C_Direction_Transmitter); // 发送设备地址和写入方向位
    I2C_WaiteForAck(); // 等待从设备的ACK响应
    I2C_WriteByte(RegAddr); // 发送寄存器地址
    I2C_WaiteForAck(); // 等待从设备的ACK响应
	
    // 循环写入多个字节的数据
    for(i = 0; i < Num; i ++)
    {
        I2C_WriteByte(*(pBuff + i)); // 发送当前字节的数据
        I2C_WaiteForAck(); // 等待从设备的ACK响应
    }
    I2C_Stop(); // 发送I2C停止信号

    return true; // 返回true表示成功写入
}

/**************************************************************************
函数功能：通过I2C总线读取一个字节的数据
入口参数：
    Ack：是否发送ACK（1发送ACK，0发送NACK）
返回  值：
    读取到的字节数据
**************************************************************************/
uint8_t I2C_ReadByte(uint8_t Ack)
{
    uint8_t i, RecDat = 0;

    SDA_IN(); // 设置SDA为输入模式
    for(i = 0; i < 8; i ++)
    {
        IIC_SCL = 0; // 拉低时钟
        delay_us(1); // 延时1微秒
        IIC_SCL = 1; // 设置时钟为高电平，通知从设备发送数据
        RecDat <<= 1; // 左移接收的数据以接收新的位
        if(READ_SDA) // 读取SDA上的数据
            RecDat |= 0x01; // 如果SDA为高电平，则将接收数据的最低位置1
        else
            RecDat &= ~0x01; // 如果SDA为低电平，则将接收数据的最低位置0
        delay_us(1); // 延时1微秒
    }
    if(I2C_ACK == Ack)
        I2C_Ack(); // 发送ACK
    else
        I2C_NAck(); // 发送NACK

    return RecDat; // 返回读取到的字节数据
}

/**************************************************************************
函数功能：通过I2C总线从指定设备的指定寄存器读取一个字节的数据
入口参数：
    DevAddr：目标设备的I2C地址
    RegAddr：目标寄存器地址
返回  值：
    读取到的字节数据
**************************************************************************/
uint8_t I2C_ReadOneByte(uint8_t DevAddr, uint8_t RegAddr)
{
    uint8_t TempVal = 0;
	
    I2C_Start(); // 发送I2C起始信号
    I2C_WriteByte(DevAddr | I2C_Direction_Transmitter); // 发送设备地址和写入方向位
    I2C_WaiteForAck(); // 等待从设备的ACK响应
    I2C_WriteByte(RegAddr); // 发送寄存器地址
    I2C_WaiteForAck(); // 等待从设备的ACK响应
    I2C_Start(); // 再次发送I2C起始信号
    I2C_WriteByte(DevAddr | I2C_Direction_Receiver); // 发送设备地址和读取方向位
    I2C_WaiteForAck(); // 等待从设备的ACK响应
    TempVal = I2C_ReadByte(I2C_NACK); // 读取一个字节的数据，并发送NACK
    I2C_Stop(); // 发送I2C停止信号
	
    return TempVal; // 返回读取到的字节数据
}

/**************************************************************************
函数功能：通过I2C总线从指定设备的指定寄存器读取多个字节的数据
入口参数：
    DevAddr：目标设备的I2C地址
    RegAddr：目标寄存器地址
    Num：要读取的字节数
    *pBuff：指向存储读取数据的缓冲区的指针
返回  值：
    成功读取时返回true，否则返回false
**************************************************************************/
bool I2C_ReadBuff(uint8_t DevAddr, uint8_t RegAddr, uint8_t Num, uint8_t *pBuff)
{
    uint8_t i;

    // 检查参数的有效性
    if(0 == Num || NULL == pBuff)
    {
        return false; // 如果要读取的字节数为0或缓冲区指针为空，则返回false
    }
	
    I2C_Start(); // 发送I2C起始信号
    I2C_WriteByte(DevAddr | I2C_Direction_Transmitter); // 发送设备地址和写入方向位
    I2C_WaiteForAck(); // 等待从设备的ACK响应
    I2C_WriteByte(RegAddr); // 发送寄存器地址
    I2C_WaiteForAck(); // 等待从设备的ACK响应
    I2C_Start(); // 再次发送I2C起始信号
    I2C_WriteByte(DevAddr | I2C_Direction_Receiver); // 发送设备地址和读取方向位
    I2C_WaiteForAck(); // 等待从设备的ACK响应

    // 循环读取多个字节的数据
    for(i = 0; i < Num; i ++)
    {
        if((Num - 1) == i)
        {
            // 如果是最后一个字节，则发送NACK
            *(pBuff + i) = I2C_ReadByte(I2C_NACK);
        }
        else
        {
            // 如果不是最后一个字节，则发送ACK
            *(pBuff + i) = I2C_ReadByte(I2C_ACK);
        }
    }

    I2C_Stop(); // 发送I2C停止信号
	
    return true; // 返回true表示成功读取
}


///**************************************************************************
//Function: IIC continuous reading data
//Input   : dev：Target device IIC address；reg:Register address；
//					length：Number of bytes；*data:The pointer where the read data will be stored
//Output  : count：Number of bytes read out-1
//函数功能：IIC连续读数据
//入口参数：dev：目标设备IIC地址；reg:寄存器地址；length：字节数；
//					*data:读出的数据将要存放的指针
//返回  值：count：读出来的字节数量-1
//**************************************************************************/ 
//u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data){
//    u8 count = 0;
//	
//	IIC_Start();
//	IIC_Send_Byte(dev);	   //发送写命令
//	IIC_Wait_Ack();
//	IIC_Send_Byte(reg);   //发送地址
//  IIC_Wait_Ack();	  
//	IIC_Start();
//	IIC_Send_Byte(dev+1);  //进入接收模式	
//	IIC_Wait_Ack();
//	
//    for(count=0;count<length;count++){
//		 
//		 if(count!=length-1)   data[count]=IIC_Read_Byte(1);  //带ACK的读数据
//		 else                  data[count]=IIC_Read_Byte(0);  //最后一个字节NACK
//	}
//    IIC_Stop();//产生一个停止条件
//    return count;
//}




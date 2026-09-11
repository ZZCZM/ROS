/**
  ******************************************************************************
  * @file    pstwo.c
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
#include "pstwo.h" // 包含 PS2 手柄的头文件

// 定义延迟时间宏，使用 delay_us 函数延迟 5 微秒
#define DELAY_TIME  delay_us(5); 
// 声明一个外部变量 flash_parameter，用于存储 Flash 参数
extern Flash_Parameter flash_parameter ;

// 按键值读取，零时存储
// 定义一个变量 Handkey 用于存储手柄按键值
u16 Handkey;	
// 开始命令。请求数据
// 定义一个数组 Comd，包含两个字节，初始化为 0x01 和 0x42，用于发送开始命令
u8 Comd[2]={0x01,0x42};	
// 数据存储数组
// 定义一个数组 Data，包含九个字节，初始化为 0，用于存储 PS2 数据
u8 Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
// 定义一个整型变量 Beep_Control_Flag，初始值为 0，用于控制蜂鸣器
int Beep_Control_Flag = 0;
// 定义一个数组 MASK，包含 16 个元素，每个元素代表一个 PS2 手柄按键的值
u16 MASK[]={
    PSB_SELECT,  // Select 按键值
    PSB_L3,      // L3 按键值
    PSB_R3 ,      // R3 按键值
    PSB_START,     // Start 按键值
    PSB_PAD_UP,    // 方向键 上 按键值
    PSB_PAD_RIGHT, // 方向键 右 按键值
    PSB_PAD_DOWN,  // 方向键 下 按键值
    PSB_PAD_LEFT,  // 方向键 左 按键值
    PSB_L2,        // L2 按键值
    PSB_R2,        // R2 按键值
    PSB_L1,        // L1 按键值
    PSB_R1 ,       // R1 按键值
    PSB_GREEN,     // 绿色按键值
    PSB_RED,       // 红色按键值
    PSB_BLUE,      // 蓝色按键值
    PSB_PINK       // 粉色按键值
	}; // 按键值与按键名
/**************************************************************************
函数功能：PS2手柄任务
入口参数：无
返回  值：无
这个函数是一个任务循环，以 100Hz 的频率处理 PS2 手柄的数据。
**************************************************************************/	
void pstwo_task(void *pvParameters)
{
    u32 lastWakeTime = getSysTickCnt(); // 获取系统滴答计数值，作为任务的初始唤醒时间
    while(1) // 无限循环
    {	
			// 此任务以 100Hz 的频率运行
			// 使用 vTaskDelayUntil 函数实现任务的周期性运行
			vTaskDelayUntil(&lastWakeTime, F2T(RATE_100_HZ));
			// 读取 PS2 的数据
			// 调用 PS2_Read 函数读取 PS2 手柄的数据
      PS2_Read(); 
			// 检测是否进入舵机初始位置微调模式
			// 调用 Servo_init_angle_adjust_mode_check 函数检查是否进入舵机初始位置微调模式
			Servo_init_angle_adjust_mode_check();
    }
}  
/**************************************************************************
函数功能：PS2手柄初始化
入口参数：无
返回  值：无
这个函数初始化 PS2 手柄相关的 GPIO 引脚。
**************************************************************************/	
void PS2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; // 定义 GPIO 初始化结构体变量
	
	// 使能端口时钟
	// 使能 GPIOE 端口的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;			// 配置 GPIOE 端口的 Pin15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		// 设置为普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 设置最大速度为 100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;		// 设置为下拉模式
	GPIO_Init(GPIOE, &GPIO_InitStructure); // 初始化 GPIOE 端口的 Pin15

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_12;	// 配置 GPIOE 端口的 Pin8, Pin10 和 Pin12
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;          // 设置为输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;         // 设置为推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;      // 设置最大速度为 50MHz
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;          // 设置为上拉模式
	GPIO_Init(GPIOE, &GPIO_InitStructure); // 初始化 GPIOE 端口的 Pin8, Pin10 和 Pin12
}
/**************************************************************************
函数功能：读取PS2手柄的控制量
入口参数：无
返回  值：无
这个函数读取 PS2 手柄的按键值和遥感数据。
**************************************************************************/	
void PS2_Read(void)
{
	static int Strat; // 定义一个静态变量 Strat，用于记录 Start 按键的状态

	// 读取按键键值
  // 调用 PS2_DataKey 函数获取 PS2 手柄的按键值，并赋值给 PS2_KEY
	PS2_KEY=PS2_DataKey(); 
	// 读取左边遥感 X 轴方向的模拟量
  // 调用 PS2_AnologData 函数获取 PS2 手柄左边遥感 X 轴方向的模拟量，并赋值给 PS2_LX
	PS2_LX=PS2_AnologData(PSS_LX); 
	// 读取左边遥感 Y 轴方向的模拟量
  // 调用 PS2_AnologData 函数获取 PS2 手柄左边遥感 Y 轴方向的模拟量，并赋值给 PS2_LY
	PS2_LY=PS2_AnologData(PSS_LY);
	// 读取右边遥感 X 轴方向的模拟量
  // 调用 PS2_AnologData 函数获取 PS2 手柄右边遥感 X 轴方向的模拟量，并赋值给 PS2_RX
	PS2_RX=PS2_AnologData(PSS_RX);
	// 读取右边遥感 Y 轴方向的模拟量
  // 调用 PS2_AnologData 函数获取 PS2 手柄右边遥感 Y 轴方向的模拟量，并赋值给 PS2_RY  
	PS2_RY=PS2_AnologData(PSS_RY);  

	if(PS2_KEY==4&&PS2_ON_Flag==0) 
		// 手柄上的 Start 按键被按下
		// 检测是否 Start 按键被按下并且 PS2_ON_Flag 为 0（表示 PS2 模式未启用），如果是，则设置 Strat 为 1
		Strat=1; 
	
	if(Strat&&(PS2_LY<118)&&PS2_ON_Flag==0&&Deviation_Count>=CONTROL_DELAY)
		// Start 按键被按下后，需要推下右边前进杆，才可以正式 PS2 控制小车
		// 检测 Strat 是否为 1（Start 按键按下），PS2_LY 是否小于 118（右边遥感 Y 轴方向的模拟量小于阈值），PS2_ON_Flag 是否为 0（PS2 模式未启用），并且 Deviation_Count 是否大于等于 CONTROL_DELAY
		// 如果以上条件都满足，则将 PS2_ON_Flag 设置为 1，表示进入 PS2 控制模式，并将其他控制模式标志位（Remote_ON_Flag, APP_ON_Flag, CAN_ON_Flag, Usart1_ON_Flag, Usart5_ON_Flag）设置为 0
		PS2_ON_Flag=1,Remote_ON_Flag=0,APP_ON_Flag=0,CAN_ON_Flag=0,Usart1_ON_Flag=0,Usart5_ON_Flag=0;  
}
/**************************************************************************
函数功能：向手柄发送命令
入口参数：CMD - 要发送的命令字节
返回  值：无
这个函数向 PS2 手柄发送命令字节。
**************************************************************************/	
void PS2_Cmd(u8 CMD)
{
	volatile u16 ref=0x01; // 定义一个 volatile 变量 ref，初始值为 0x01，用于逐位发送命令
	Data[1] = 0; // 初始化 Data 数组的第二个元素为 0
	for(ref=0x01;ref<0x0100;ref<<=1) // 使用 for 循环从最低位到最高位逐位发送命令
	{
		if(ref&CMD) // 检测当前位是否为 1
		{
			DO_H;     // 输出一位控制位，为高电平
		}
		else DO_L; // 如果当前位为 0，则输出低电平

		CLK_H;      // 时钟拉高，开始发送当前位
		DELAY_TIME; // 延迟一段时间
		CLK_L;      // 时钟拉低，结束发送当前位
		DELAY_TIME; // 延迟一段时间
		CLK_H;      // 时钟拉高，等待接收确认
		if(DI) // 检测 DI 引脚的电平状态
			Data[1] = ref|Data[1]; // 如果 DI 引脚为高电平，表示接收确认，将当前位标记到 Data[1] 中
	}
	delay_us(16); // 延迟 16 微秒
}
/**************************************************************************
函数功能：检测手柄是否为红灯模式
入口参数：无
返回  值：0表示红灯模式，其他值表示其他模式
说明：通过发送命令读取手柄状态数据，判断当前是否为红灯模式（0x73）
**************************************************************************/	
u8 PS2_RedLight(void)
{
	CS_L;  // 拉低片选信号，开始与PS2手柄通信
	PS2_Cmd(Comd[0]);  // 发送开始命令0x01
	PS2_Cmd(Comd[1]);  // 发送请求数据命令0x42
	CS_H;  // 拉高片选信号，结束通信
	if( Data[1] == 0X73)   return 0 ;  // 判断数据缓冲区第二位是否为红灯模式标识
	else return 1;  // 非红灯模式返回1
}

/**************************************************************************
函数功能：读取手柄原始数据
入口参数：无
返回  值：无
说明：通过SPI通信协议完整读取手柄的9字节数据，存储到Data数组
**************************************************************************/	
void PS2_ReadData(void)
{
	volatile u8 byte=0;    // 数据字节计数器
	volatile u16 ref=0x01; // 位操作掩码
	CS_L;  // 启动通信
	PS2_Cmd(Comd[0]);  // 发送开始命令0x01
	PS2_Cmd(Comd[1]);  // 发送请求数据命令0x42
	
	// 接收后续7字节数据（从Data[2]到Data[8]）
	for(byte=2;byte<9;byte++) 
	{
		// 逐位接收1字节数据（8位）
		for(ref=0x01;ref<0x100;ref<<=1)
		{
			CLK_H;  // 拉高时钟
			DELAY_TIME;  // 保持时钟高电平5us
			CLK_L;  // 拉低时钟
			DELAY_TIME;  // 保持时钟低电平5us
			CLK_H;  // 准备读取数据位
			if(DI)  // 检测数据线电平
				Data[byte] = ref|Data[byte];  // 若为高电平，设置对应位
		}
		delay_us(16);  // 字节间延迟16us
	}
	CS_H;  // 结束通信
}

/**************************************************************************
函数功能：解析按键状态
入口参数：无
返回  值：0-无按键按下，1-16对应按键索引
说明：组合按键数据并遍历检测16个按键状态，返回首个被按下的按键编号
**************************************************************************/	
u8 PS2_DataKey()
{
	u8 index;  // 按键索引计数器
	PS2_ClearData();  // 清空数据缓冲区
	PS2_ReadData();   // 读取最新手柄数据

	// 组合按键状态（Data[3]为低8位，Data[4]为高8位）
	Handkey=(Data[4]<<8)|Data[3]; 
	
	// 遍历16个按键掩码
	for(index=0;index<16;index++)
	{	    
		if((Handkey&(1<<(MASK[index]-1)))==0)  // 检测对应掩码位是否为0（按下状态）
		return index+1;  // 返回按键编号（1-16）
	}
	return 0;  // 无按键按下返回0
}

/**************************************************************************
函数功能：获取摇杆模拟量
入口参数：button - 摇杆类型(PSS_LX/PSS_LY/PSS_RX/PSS_RY)
返回  值：0-255的模拟量数值
说明：直接从数据缓冲区获取指定摇杆的ADC采样值
**************************************************************************/
u8 PS2_AnologData(u8 button)
{
	return Data[button];  // 返回对应摇杆的数据存储位置值
}

/**************************************************************************
函数功能：清空数据缓冲区
入口参数：无
返回  值：无
说明：将9字节数据缓冲区全部清零，准备接收新数据
**************************************************************************/
void PS2_ClearData()
{
	u8 a;  // 数组索引
	for(a=0;a<9;a++)
		Data[a]=0x00;  // 逐字节清零
}

/**************************************************************************
函数功能：控制手柄震动电机
入口参数：motor1 - 右侧小电机(0关/非0开)
         motor2 - 左侧大电机(0x40-0xFF强度值)
返回  值：无
说明：发送8字节控制指令，其中motor2的值越大震动强度越高
******************************************************/
void PS2_Vibration(u8 motor1, u8 motor2)
{
	CS_L;  // 启动通信
	delay_us(16);  // 延时等待稳定
	PS2_Cmd(0x01);  // 发送开始命令
	PS2_Cmd(0x42);  // 发送振动控制指令
	PS2_Cmd(0X00);  // 保留位1
	PS2_Cmd(motor1);  // 右侧小电机控制
	PS2_Cmd(motor2);  // 左侧大电机强度
	PS2_Cmd(0X00);  // 保留位2
	PS2_Cmd(0X00);  // 保留位3
	PS2_Cmd(0X00);  // 保留位4
	PS2_Cmd(0X00);  // 保留位5
	CS_H;  // 结束通信
	delay_us(16);  // 延时确保指令完成
}
/**************************************************************************
函数功能：短轮询操作
入口参数：无
返回  值：无
说明：发送基础指令保持手柄连接状态，维持通信链路
**************************************************************************/
void PS2_ShortPoll(void)
{
	CS_L;  // 拉低片选信号开始通信
	delay_us(16);  // 等待信号稳定
	PS2_Cmd(0x01);  // 发送开始命令
	PS2_Cmd(0x42);  // 请求数据命令
	PS2_Cmd(0X00);  // 保留位1
	PS2_Cmd(0x00);  // 保留位2
	PS2_Cmd(0x00);  // 保留位3
	CS_H;  // 结束通信
	delay_us(16);	// 确保指令完成
}

/**************************************************************************
函数功能：进入配置模式
入口参数：无
返回  值：无
说明：发送9字节配置指令开启手柄的深度配置模式
**************************************************************************/
void PS2_EnterConfing(void)
{
	CS_L;  // 启动配置通信
	delay_us(16);
	PS2_Cmd(0x01);  // 命令头
	PS2_Cmd(0x43);  // 进入配置模式指令
	PS2_Cmd(0X00);  // 配置选项1
	PS2_Cmd(0x01);  // 0x01表示开启配置
	PS2_Cmd(0x00);  // 配置选项2
	PS2_Cmd(0X00);  // 保留位1
	PS2_Cmd(0X00);  // 保留位2
	PS2_Cmd(0X00);  // 保留位3
	PS2_Cmd(0X00);  // 保留位4
	CS_H;  // 结束配置通信
	delay_us(16);
}

/**************************************************************************
函数功能：启用模拟模式
入口参数：无
返回  值：无
说明：配置手柄摇杆返回0-255模拟量（默认返回0/1数字量），并锁定模式设置
**************************************************************************/
void PS2_TurnOnAnalogMode(void)
{
	CS_L;  // 启动模式设置通信
	PS2_Cmd(0x01);  // 命令头
	PS2_Cmd(0x44);  // 模式设置指令
	PS2_Cmd(0X00);  // 设置类型
	PS2_Cmd(0x01);  // 0x01启用模拟模式（0x00为数字模式）
	PS2_Cmd(0x03);  // 模式锁定参数：0x03禁止通过MODE键切换
					 // 0xEE允许通过MODE键切换模式
	PS2_Cmd(0X00);  // 保留位1
	PS2_Cmd(0X00);  // 保留位2
	PS2_Cmd(0X00);  // 保留位3
	PS2_Cmd(0X00);  // 保留位4
	CS_H;  // 结束通信
	delay_us(16);
}

/**************************************************************************
函数功能：启用震动模式
入口参数：无
返回  值：无
说明：激活手柄内置震动电机功能，需配合PS2_Vibration()使用
**************************************************************************/
void PS2_VibrationMode(void)
{
	CS_L;  
	delay_us(16);
	PS2_Cmd(0x01);  
	PS2_Cmd(0x4D);  // 震动功能控制指令
	PS2_Cmd(0X00);  // 电机控制位
	PS2_Cmd(0x00);  // 保留位
	PS2_Cmd(0X01);  // 0x01启用震动功能
	CS_H;
	delay_us(16);	
}

/**************************************************************************
函数功能：保存配置并退出
入口参数：无
返回  值：无
说明：发送5个0x5A确认字节将配置参数写入手柄非易失存储器
**************************************************************************/
void PS2_ExitConfing(void)
{
	CS_L;  
	delay_us(16);
	PS2_Cmd(0x01);  
	PS2_Cmd(0x43);  // 退出配置指令
	PS2_Cmd(0X00);  // 退出模式参数
	PS2_Cmd(0x00);  // 保留位1
	PS2_Cmd(0x5A);  // 确认字节1
	PS2_Cmd(0x5A);  // 确认字节2
	PS2_Cmd(0x5A);  // 确认字节3
	PS2_Cmd(0x5A);  // 确认字节4
	PS2_Cmd(0x5A);  // 确认字节5
	CS_H;  // 结束通信后配置生效
	delay_us(16);
}

/**************************************************************************
函数功能：完整配置初始化流程
入口参数：无
返回  值：无
说明：标准初始化序列，包含3次握手->配置模式->模式设置->保存配置
**************************************************************************/
void PS2_SetInit(void)
{
	PS2_ShortPoll();  // 三次握手确保连接稳定
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();	 // 进入配置模式
	PS2_TurnOnAnalogMode(); // 设置模拟模式
	//PS2_VibrationMode();	// 可选震动功能
	PS2_ExitConfing();  // 保存配置退出
}

/**************************************************************************
函数功能：综合数据采集
入口参数：无
返回  值：无
说明：当PS2控制标志有效时，采集全部摇杆和按键数据
**************************************************************************/
void PS2_Receive (void)
{
	if(PS2_ON_Flag)  // PS2控制模式激活时
	{
		// 读取四个摇杆的模拟量
		PS2_LX=PS2_AnologData(PSS_LX);
		PS2_LY=PS2_AnologData(PSS_LY);
		PS2_RX=PS2_AnologData(PSS_RX);
		PS2_RY=PS2_AnologData(PSS_RY);
	}
	// 无论是否激活都更新按键状态
	PS2_KEY=PS2_DataKey();
}

/**************************************************************************
函数功能：舵机微调模式进入检测
入口参数：无
返回  值：无
说明：通过特定摇杆组合（左右摇杆外推/内推）触发调试模式
**************************************************************************/
void Servo_init_angle_adjust_mode_check(void)
{
	static int check_count1=0,check_count2=0;  // 持续时间计数器
	
	// 检测条件1：左摇杆左偏（<100）且右摇杆右偏（>200）
	if(PS2_LX<100 && PS2_RX>200)   
	{
		check_count1++;  // 符合条件时计数器累加
		Move_X=0,Move_Y=0,Move_Z=0;  // 冻结底盘运动
	}
	else check_count1=0;  // 条件不满足重置计数器
	
	// 检测条件2：左摇杆右偏（>200）且右摇杆左偏（<100）
	if(PS2_LX>200 && PS2_RX<100)   
	{
		check_count2++;  // 符合条件时计数器累加
		Move_X=0,Move_Y=0,Move_Z=0;  // 冻结底盘运动
	}
	else check_count2=0;

	// 进入调试模式判断（持续5秒）
	if(check_count1>500)  // 100Hz下500次=5秒
	{
		Beep_Control_Flag = 1;  // 触发蜂鸣器提示
		Servo_init_angle_adjust=1;  // 进入调试模式
		check_count1=0;  // 重置计数器
	}
	
	// 退出调试模式判断（持续5秒）
	if(check_count2>500 && Servo_init_angle_adjust==1)
	{
		Beep_Control_Flag = 1;  // 蜂鸣器提示
		Servo_init_angle_adjust=0;  // 退出调试模式
		Flash_Write(flash_parameter.flash_address_mark,sizeof(Flash_Parameter));  // 写入Flash保存参数
		check_count2=0;
	}
}

/**************************************************************************
函数功能：舵机微调执行
入口参数：无
返回  值：无
说明：通过方向键调整四个舵机的初始角度，步长1度
**************************************************************************/
void Servo_init_angle_adjust_function(void)
{
	int step=1;  // 调整步长
	static int once=0;  // 初始化标记
	
	// 首次进入时重置角度
	if (once == 0)
	{
		Moveit_Angle1=0,Moveit_Angle2=0,Moveit_Angle3=0,Moveit_Angle4=0;
		once=1;
	}
	
	// 按键与舵机对应关系：
	if(PS2_KEY==5)     Moveit_Angle1_init+=step;      // 方向右键增
	else if(PS2_KEY==7) Moveit_Angle1_init-=step;    // 方向左键减
	
	else if(PS2_KEY==6) Moveit_Angle2_init-=step;   // 方向下键减    
	else if(PS2_KEY==8) Moveit_Angle2_init+=step;    // 方向上键增
	
	else if(PS2_KEY==13) Moveit_Angle3_init+=step;   // △键增
	else if(PS2_KEY==15) Moveit_Angle3_init-=step;	 // ×键减
	
	else if(PS2_KEY==14) Moveit_Angle4_init-=step;  // □键减    
	else if(PS2_KEY==16) Moveit_Angle4_init+=step;   // ○键增
}



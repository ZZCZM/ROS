/**
  ******************************************************************************
  * @file    timer.c
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
#include "timer.h"

/* 全局变量说明 -----------------------------------------------------------*/
// 通道1输入捕获状态寄存器（高2位：0-无捕获，1-上升沿捕获，2-下降沿捕获；低6位：捕获溢出次数）
u8 TIM8CH1_CAPTURE_STA = 0;	
u16 TIM8CH1_CAPTURE_UPVAL;  // 上升沿捕获值
u16 TIM8CH1_CAPTURE_DOWNVAL;// 下降沿捕获值

// 通道2输入捕获状态寄存器（位定义同通道1）
u8 TIM8CH2_CAPTURE_STA = 0;		
u16 TIM8CH2_CAPTURE_UPVAL;
u16 TIM8CH2_CAPTURE_DOWNVAL;

// 通道3输入捕获状态寄存器（位定义同通道1）
u8 TIM8CH3_CAPTURE_STA = 0;		
u16 TIM8CH3_CAPTURE_UPVAL;
u16 TIM8CH3_CAPTURE_DOWNVAL;

// 通道4输入捕获状态寄存器（位定义同通道1）
u8 TIM8CH4_CAPTURE_STA = 0;			
u16 TIM8CH4_CAPTURE_UPVAL;
u16 TIM8CH4_CAPTURE_DOWNVAL;

// 定时器时间戳缓存（用于计算脉冲宽度）
u32 TIM8_T1;
u32 TIM8_T2;
u32 TIM8_T3;
u32 TIM8_T4;

// 航模遥控信号当前值（1500为中位，范围通常1000-2000us）
int Remoter_Ch1=1500,Remoter_Ch2=1500,Remoter_Ch3=1500,Remoter_Ch4=1500;
// 航模遥控信号上次采样值（用于信号滤波处理）
int L_Remoter_Ch1=1500,L_Remoter_Ch2=1500,L_Remoter_Ch3=1500,L_Remoter_Ch4=1500;  

/**************************************************************************
函数功能：TIM8输入捕获初始化
入口参数：arr - 自动重装载值，决定计数周期
         psc - 预分频系数，72MHz主频下(psc+1)分频
返 回 值：无
说明：配置TIM8的4个通道为PWM输入模式，用于测量航模遥控器PWM信号脉宽
       定时器时钟 = 72MHz / (psc+1)
       最大计数周期 = (arr+1) * 时钟周期
**************************************************************************/ 
void TIM8_Cap_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	// 使能GPIOC时钟（假设STEER_PORT为GPIOC）
	RCC_AHB1PeriphClockCmd(STEER_GPIO_CLK, ENABLE); 	
	// 使能TIM8时钟（APB2总线）
	RCC_APB2PeriphClockCmd(STEER_TIM_CLK,ENABLE);  	
		
	// 配置PC6/7/8/9为复用功能（TIM8通道1-4）
	GPIO_InitStructure.GPIO_Pin = STEER1_PIN | STEER2_PIN | STEER3_PIN | STEER4_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz速率
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      // 推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;     // 下拉电阻（抗干扰）
	GPIO_Init(STEER_PORT,&GPIO_InitStructure); 

	// 引脚复用映射（将GPIO连接到TIM8通道）
	GPIO_PinAFConfig(STEER1_PORT,STEER1_PIN_Source,STEER_AF);  // PC6 -> TIM8_CH1
	GPIO_PinAFConfig(STEER2_PORT,STEER2_PIN_Source,STEER_AF);  // PC7 -> TIM8_CH2
	GPIO_PinAFConfig(STEER3_PORT,STEER3_PIN_Source,STEER_AF);  // PC8 -> TIM8_CH3
	GPIO_PinAFConfig(STEER4_PORT,STEER4_PIN_Source,STEER_AF);  // PC9 -> TIM8_CH4

	/****** 定时器基础配置 ******/
	TIM_TimeBaseStructure.TIM_Period = arr;        // 自动重装载值（决定计数周期）
	TIM_TimeBaseStructure.TIM_Prescaler = psc;     // 预分频系数（72MHz/(psc+1)）
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频（与输入捕获无关）
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
	TIM_TimeBaseInit(STEER_ID, &TIM_TimeBaseStructure); 

	/****** 通道1输入捕获配置 ******/
	TIM_ICInitStructure.TIM_Channel = STEER1_Channel;    // 选择通道1（TIM_Channel_1）
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 直连输入
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;    // 输入分频（不分频）
	TIM_ICInitStructure.TIM_ICFilter = 0x0F;           // 滤波器（15个时钟周期滤波）
	TIM_ICInit(STEER_ID, &TIM_ICInitStructure);

	/****** 通道2输入捕获配置 ******/
	TIM_ICInitStructure.TIM_Channel = STEER2_Channel;    // 通道2（TIM_Channel_2）
	TIM_ICInitStructure.TIM_ICFilter = 0x00;           // 关闭滤波器（通道2未使用滤波）
	TIM_ICInit(STEER_ID, &TIM_ICInitStructure);

	/****** 通道3输入捕获配置 ******/
	TIM_ICInitStructure.TIM_Channel = STEER3_Channel;    // 通道3（TIM_Channel_3）
	TIM_ICInit(STEER_ID, &TIM_ICInitStructure);

	/****** 通道4输入捕获配置 ******/
	TIM_ICInitStructure.TIM_Channel = STEER4_Channel;    // 通道4（TIM_Channel_4）
	TIM_ICInit(STEER_ID, &TIM_ICInitStructure);

	/****** 中断配置 ******/
	NVIC_InitStructure.NVIC_IRQChannel = STEER_IRQn;     // TIM8中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;    // 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        // 使能中断
	NVIC_Init(&NVIC_InitStructure);    

	// 使能通道1-4的捕获中断
	TIM_ITConfig(STEER_ID, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4, ENABLE);   
	
	// 高级定时器必须使能PWM输出（即使未使用PWM功能）
	TIM_CtrlPWMOutputs(STEER_ID,ENABLE); 	
	
	// 启动定时器
	TIM_Cmd(STEER_ID, ENABLE); 		
}

/**************************************************************************
函数功能：TIM8输入捕获中断服务函数
入口参数：无
返 回 值：无
说明：处理4个通道的PWM信号捕获，测量1000-2000us脉宽信号
       采用边沿触发+定时器溢出的组合测量方式，支持最大20ms周期测量
**************************************************************************/
void TIM8_CC_IRQHandler(void)
{
	// 遥控模式激活条件：通道2值>1600（油门杆前推）且未激活状态
	if(Remoter_Ch2>1600 && Remote_ON_Flag==0 && Deviation_Count>=CONTROL_DELAY)
	{
		// 设置遥控模式标志位，清除其他控制模式
		Remote_ON_Flag=1;   // 启用航模遥控模式
		APP_ON_Flag=0;      // 禁用APP控制
		PS2_ON_Flag=0;      // 禁用PS2手柄控制
		CAN_ON_Flag=0;      // 禁用CAN控制
		Usart1_ON_Flag=0;   // 禁用串口1控制
		Usart5_ON_Flag=0;   // 禁用串口5控制
	}

	/******************** 通道1处理 ********************/
	if ((TIM8CH1_CAPTURE_STA & 0X80) == 0) // 最高位为0表示捕获未完成
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET) // 检查通道1捕获中断
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC1); // 清除中断标志
			if (TIM8CH1_CAPTURE_STA & 0X40) // 第6位为1表示已捕获上升沿
			{
				/* 下降沿捕获处理 */
				TIM8CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM8); // 获取下降沿时刻计数值
				// 处理计数器溢出（定时器周期10000）
				if (TIM8CH1_CAPTURE_DOWNVAL < TIM8CH1_CAPTURE_UPVAL)
					TIM8_T1 = 9999;  // 发生溢出时补偿周期值
				else
					TIM8_T1 = 0;
				// 计算脉宽 = 下降沿值 - 上升沿值 + 溢出补偿
				Remoter_Ch1 = TIM8CH1_CAPTURE_DOWNVAL - TIM8CH1_CAPTURE_UPVAL + TIM8_T1;
				// 限幅滤波（防止突变超过500us）
				if(abs(Remoter_Ch1-L_Remoter_Ch1)>500) 
					Remoter_Ch1=L_Remoter_Ch1;
				L_Remoter_Ch1=Remoter_Ch1; // 更新上次采样值
				
				TIM8CH1_CAPTURE_STA = 0; // 状态清零
				TIM_OC1PolarityConfig(TIM8, TIM_ICPolarity_Rising); // 重置为上升沿捕获
			}
			else 
			{
				/* 上升沿捕获处理 */
				TIM8CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM8); // 记录上升沿时刻
				TIM8CH1_CAPTURE_STA |= 0X40; // 设置上升沿捕获标志
				TIM_OC1PolarityConfig(TIM8, TIM_ICPolarity_Falling); // 切换为下降沿捕获
			}
		}
	}
	/******************** 通道2处理 ********************/
	if ((TIM8CH2_CAPTURE_STA & 0X80) == 0) // 检查完成标志位（bit7未置位）
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET) // 确认是通道2捕获中断
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC2); // 清除中断标志
			
			if (TIM8CH2_CAPTURE_STA & 0X40) // 检测是否已捕获上升沿（bit6判断）
			{
				/****** 下降沿处理 ******/
				TIM8CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM8); // 获取下降沿时刻计数器值
				
				/* 溢出补偿计算（假设定时器为16位，ARR=9999）*/
				if (TIM8CH2_CAPTURE_DOWNVAL < TIM8CH2_CAPTURE_UPVAL) 
					TIM8_T2 = 9999; // 发生溢出时补偿周期最大值
				else 
					TIM8_T2 = 0;    // 无溢出情况
				
				// 计算实际脉宽（单位：定时器计数周期）
				Remoter_Ch2 = TIM8CH2_CAPTURE_DOWNVAL - TIM8CH2_CAPTURE_UPVAL + TIM8_T2;
				
				/* 软件滤波（抑制>500个计数值的突变） */
				if(abs(Remoter_Ch2 - L_Remoter_Ch2) > 500)
					Remoter_Ch2 = L_Remoter_Ch2; // 保留前次值
				L_Remoter_Ch2 = Remoter_Ch2;      // 更新历史值
				
				/* 状态机复位 */
				TIM8CH2_CAPTURE_STA = 0;               // 清除状态标志
				TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Rising); // 切回上升沿捕获
			}
			else 
			{
				/****** 上升沿处理 ******/
				TIM8CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM8); // 记录上升沿时刻计数器值
				TIM8CH2_CAPTURE_STA |= 0X40;          // 置位上升沿捕获标志（bit6）
				TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Falling); // 切换为下降沿捕获
			}
		}
	}
	/* TIM8通道3输入捕获中断处理 ---------------------------------------------------
	* 功能：测量PWM信号高电平脉宽（通道3）
	* 状态机与通道2相同，硬件特性：
	*     - 使用TIM8通道3（具体引脚参考数据手册）
	*     - 适用于频率范围50Hz~500Hz的PWM信号
	*/
	if ((TIM8CH3_CAPTURE_STA & 0X80) == 0) // 检查通道3完成标志位（bit7）
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC3) != RESET) 
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC3); // 清除通道3中断标志
			
			if (TIM8CH3_CAPTURE_STA & 0X40) // 检查上升沿捕获标记（bit6）
			{
				/* 下降沿处理阶段 */
				TIM8CH3_CAPTURE_DOWNVAL = TIM_GetCapture3(TIM8); // 获取下降沿捕获值
				
				// 溢出补偿计算（假设ARR=9999）
				TIM8_T3 = (TIM8CH3_CAPTURE_DOWNVAL < TIM8CH3_CAPTURE_UPVAL) ? 9999 : 0;
				
				// 计算实际脉宽：下降沿值 - 上升沿值 + 溢出补偿
				Remoter_Ch3 = TIM8CH3_CAPTURE_DOWNVAL - TIM8CH3_CAPTURE_UPVAL + TIM8_T3;
				
				/* 滑动窗口滤波：抑制突发干扰 */
				if(abs(Remoter_Ch3 - L_Remoter_Ch3) > 500) // 500个计数≈0.5ms@1MHz
					Remoter_Ch3 = L_Remoter_Ch3; // 超过阈值保持前值
				L_Remoter_Ch3 = Remoter_Ch3;     // 更新历史值
				
				/* 状态复位 */
				TIM8CH3_CAPTURE_STA = 0;               // 清除所有状态位
				TIM_OC3PolarityConfig(TIM8, TIM_ICPolarity_Rising); // 重置为上升沿触发
			}
			else 
			{
				/* 上升沿处理阶段 */
				TIM8CH3_CAPTURE_UPVAL = TIM_GetCapture3(TIM8); // 记录上升沿时间戳
				TIM8CH3_CAPTURE_STA |= 0X40;          // 置位上升沿捕获标志
				TIM_OC3PolarityConfig(TIM8, TIM_ICPolarity_Falling); // 切换下降沿检测
			}
		}
	}

	/* TIM8通道4输入捕获中断处理 ---------------------------------------------------
	* 功能：测量PWM信号高电平脉宽（通道4）
	* 特殊说明：TIM8通道4无互补输出功能，适合单向信号检测
	*/
	if ((TIM8CH4_CAPTURE_STA & 0X80) == 0) // 检查通道4完成标志
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC4) != RESET) 
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC4); // 清除通道4中断标志
			
			if (TIM8CH4_CAPTURE_STA & 0X40) 
			{
				/* 下降沿处理 */
				TIM8CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM8);
				TIM8_T4 = (TIM8CH4_CAPTURE_DOWNVAL < TIM8CH4_CAPTURE_UPVAL) ? 9999 : 0;
				Remoter_Ch4 = TIM8CH4_CAPTURE_DOWNVAL - TIM8CH4_CAPTURE_UPVAL + TIM8_T4;
				
				// 带死区的滤波处理
				if(abs(Remoter_Ch4 - L_Remoter_Ch4) > 500)
					Remoter_Ch4 = L_Remoter_Ch4;
				L_Remoter_Ch4 = Remoter_Ch4;
				
				TIM8CH4_CAPTURE_STA = 0;
				TIM_OC4PolarityConfig(TIM8, TIM_ICPolarity_Rising); // 注：TIM8_CH4极性配置特殊
			}
			else 
			{
				/* 上升沿捕获 */
				TIM8CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM8);
				TIM8CH4_CAPTURE_STA |= 0X40;
				TIM_OC4PolarityConfig(TIM8, TIM_ICPolarity_Falling); 
			}
		}
	}

}
/**************************************************************************
函数功能：TIM8更新中断服务函数
入口参数：无
返 回 值：无
说明：处理TIM8周期更新中断（此处仅清除中断标志，未执行实际功能）
**************************************************************************/
void TIM8_UP_TIM13_IRQHandler(void) 
{ 
	TIM8->SR &= ~(1<<0);  // 清除UIF更新中断标志位（第0位）
}

/**************************************************************************
函数功能：TIM8舵机PWM输出初始化
入口参数：arr - 自动重装载值（决定PWM周期）
         psc - 预分频系数（168MHz/(psc+1)）
返 回 值：无
说明：配置TIM8的4个通道为PWM输出模式，输出50Hz舵机控制信号
       PWM分辨率 = 定时器时钟/(arr+1)
       典型配置：arr=9999, psc=167 → 10ms周期（168MHz/(168*(10000))=100Hz）
**************************************************************************/
void TIM8_SERVO_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;           // GPIO配置结构体
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // 定时器基础配置
	TIM_OCInitTypeDef  TIM_OCInitStructure;        // PWM输出配置

	// 使能GPIOC时钟（假设STEER_PORT为GPIOC）
	RCC_AHB1PeriphClockCmd(STEER_GPIO_CLK, ENABLE); 	
	// 使能TIM8时钟（APB2总线）
	RCC_APB2PeriphClockCmd(STEER_TIM_CLK, ENABLE);  	

	// 配置PC6/7/8/9为复用功能（TIM8通道1-4）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用模式
	GPIO_InitStructure.GPIO_Pin = STEER1_PIN | STEER2_PIN | STEER3_PIN | STEER4_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz速率
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉电阻（确保默认高电平）
	GPIO_Init(STEER_PORT, &GPIO_InitStructure);

	// 引脚复用映射（将GPIO连接到TIM8通道）
	GPIO_PinAFConfig(STEER1_PORT, STEER1_PIN_Source, STEER_AF); // PC6 -> TIM8_CH1
	GPIO_PinAFConfig(STEER2_PORT, STEER2_PIN_Source, STEER_AF); // PC7 -> TIM8_CH2
	GPIO_PinAFConfig(STEER3_PORT, STEER3_PIN_Source, STEER_AF); // PC8 -> TIM8_CH3
	GPIO_PinAFConfig(STEER4_PORT, STEER4_PIN_Source, STEER_AF); // PC9 -> TIM8_CH4

	/****** 定时器基础配置 ******/
	TIM_TimeBaseStructure.TIM_Period = arr;        // 自动重装载值（周期=arr+1）
	TIM_TimeBaseStructure.TIM_Prescaler = psc;      // 预分频系数（分频后时钟=72MHz/(psc+1)）
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分割（与PWM无关）
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
	TIM_TimeBaseInit(STEER_ID, &TIM_TimeBaseStructure); 

	/****** PWM输出配置 ******/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   // PWM模式1（CNT<CCR时输出有效电平）
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
	TIM_OCInitStructure.TIM_Pulse = 0;                  // 初始占空比为0
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 有效电平为高
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset; // 空闲状态低电平

	// 初始化四个通道（CH1-CH4）
	TIM_OC1Init(STEER_ID, &TIM_OCInitStructure); 
	TIM_OC2Init(STEER_ID, &TIM_OCInitStructure); 
	TIM_OC3Init(STEER_ID, &TIM_OCInitStructure); 
	TIM_OC4Init(STEER_ID, &TIM_OCInitStructure); 

	// 使能通道预装载（保证占空比更新无抖动）
	TIM_OC1PreloadConfig(STEER_ID, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(STEER_ID, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(STEER_ID, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(STEER_ID, TIM_OCPreload_Enable);

	// 高级定时器必须启用主输出
	TIM_CtrlPWMOutputs(STEER_ID, ENABLE); 	
	// 启动定时器
	TIM_Cmd(STEER_ID, ENABLE); 		 

	/****** 舵机初始位置设置 ******/
	Servo_PWM1 = SERVO_INIT + Moveit_Angle1_init; // 通道1初始位置（SERVO_INIT通常为1500）
	Servo_PWM2 = SERVO_INIT + Moveit_Angle2_init; // 通道2初始位置
	Servo_PWM3 = SERVO_INIT + Moveit_Angle3_init; // 通道3初始位置
	Servo_PWM4 = SERVO_INIT + Moveit_Angle4_init; // 通道4初始位置
}

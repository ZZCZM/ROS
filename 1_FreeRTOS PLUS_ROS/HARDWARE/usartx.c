/**
  ******************************************************************************
  * @file    usartx.c
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
#include "usartx.h"
#include "stm32f4xx_dma.h"
#include "event_groups.h"
#include "stdio.h"
// extern uint8_t isReadyForStandby;
// extern uint16_t Hour;
// extern uint16_t Minute;
// extern uint16_t Second;

// SEND_DATA Send_Data;
// RECEIVE_DATA Receive_Data;
// SemaphoreHandle_t Rx_data_semaphore;  // 用于传输完成的信号量句柄
// EventGroupHandle_t xEventGroup;  // 事件标志组句柄
// extern int Time_count;
// void init_event_group() {
//     // 创建一个事件标志组
//     xEventGroup = xEventGroupCreate();
//     if (xEventGroup == NULL) {
//         // 事件组创建失败，可能需要处理错误
// //        printf("Failed to create event group\n");
//     }
// }

// ///重定向c库函数printf到USART3
// int fputc(int ch, FILE *f)
// {
// 		/* 发送一个字节数据到USART3 */
// 		USART_SendData(DEBUG_USART, (uint8_t) ch);
		
// 		/* 等待发送完毕 */
// 		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);		
	
// 		return (ch);
// }

// ///重定向c库函数scanf到USART3
// int fgetc(FILE *f)
// {
// 		/* 等待串口1输入数据 */
// 		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);

// 		return (int)USART_ReceiveData(DEBUG_USART);
// }
// void initiate_next_transmission(void) {
	
// 	   USART_DMA_Config();
// 	   USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);
// //		 DMA_ITConfig(DEBUG_USART_DMA_STREAM, DMA_IT_TC, ENABLE);
// //    DMA_Cmd(DEBUG_USART_DMA_STREAM, DISABLE);
// //    while (DMA_GetCmdStatus(DEBUG_USART_DMA_STREAM) != DISABLE);

// //    DEBUG_USART_DMA_STREAM->M0AR = (uint32_t)&Send_Data;
// //    DEBUG_USART_DMA_STREAM->NDTR = SEND_DATA_SIZE;

// //    DMA_Cmd(DEBUG_USART_DMA_STREAM, ENABLE);
// //		USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);
// }
// int Received_data_processing(void)
// {
//   static u8 Count_car,Count_moveit,Count_time;
// 	static u8 rxbuf_moveit[16];
// 	static u8 rxbuf_time[16];
// 	u8 Usart_Receive;
// 	int check=0,error=1,i;

// 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //Check if data is received //判断是否接收到数据
// 	{
// 		Usart_Receive = USART_ReceiveData(USART3);//Read the data //读取数据
// 		if(Time_count<CONTROL_DELAY)
// 			// Data is not processed until 10 seconds after startup
// 		  //开机10秒前不处理数据
// 		  return 0;	
		
// 		//Fill the array with serial data
// 		//串口数据填入数组
//     Receive_Data.buffer[Count_car]=Usart_Receive;
// 		rxbuf_moveit[Count_moveit]=Usart_Receive;
// 		rxbuf_time[Count_time]=Usart_Receive;
		
// 		// Ensure that the first data in the array is FRAME_HEADER
// 		//确保数组第一个数据为FRAME_HEADER
// 		if(Usart_Receive == FRAME_HEADER_CAR||Count_car>0) Count_car++; else Count_car=0; //接收控制底盘的数据帧
// 		if(Usart_Receive == FRAME_HEADER_MOVEIT||Count_moveit>0) Count_moveit++; else Count_moveit=0; //接收控制机械臂的数据帧
// 		if(Usart_Receive == FRAME_HEADER_TIME||Count_time>0) Count_time++; else Count_time=0; //接收控制休眠时间的数据帧
		
// 		if (Count_time == 11) //Verify the length of the packet //验证数据包的长度
// 		{   
// 				Count_time=0; //Prepare for the serial port data to be refill into the array //为串口数据重新填入数组做准备
// 				if(rxbuf_time[10] == FRAME_TAIL_TIME) //Verify the frame tail of the packet //验证数据包的帧尾
// 				{
// 					for(i=0; i<9; i++)
// 					{
// 						check=rxbuf_time[i]^check; //异或，用于检测数据是否出错
// 					}
// 					if(check==rxbuf_time[9]) error=0; //检验成功
					
// 					if(error==0&&rxbuf_time[2]==1)	 //数据校验位计算
// 				  {	
	
// 						//进入临界区修改成休眠模式，并设置下次唤醒时间
// 						taskENTER_CRITICAL();
// 						isReadyForStandby=rxbuf_moveit[2];
// 						Hour=(rxbuf_time[3]<<8)+(rxbuf_time[4]);
// 						Minute=(rxbuf_time[5]<<8)+(rxbuf_time[6]);
// 						Second=(rxbuf_time[7]<<8)+(rxbuf_time[8]);
// 						taskEXIT_CRITICAL();				
// 				  }
// 			}
// 		}		
		
// 		if (Count_car == 11) //Verify the length of the packet //验证数据包的长度
// 		{   
// 				Count_car=0; //Prepare for the serial port data to be refill into the array //为串口数据重新填入数组做准备
// 				if(Receive_Data.buffer[10] == FRAME_TAIL_CAR) //Verify the frame tail of the packet //验证数据包的帧尾
// 				{
// 					//Data exclusionary or bit check calculation, mode 0 is sent data check
// 					//数据异或位校验计算，模式0是发送数据校验
// 					if(Receive_Data.buffer[9] ==Check_Sum(9,0))	 
// 				  {		
// 						//All modes flag position 0, USART3 control mode
//             //所有模式标志位置0，为Usart3控制模式						
// 						PS2_ON_Flag=0;
// 						Remote_ON_Flag=0;
// 						APP_ON_Flag=0;
// 						CAN_ON_Flag=0;
// 						Usart1_ON_Flag=0;
// 						Usart5_ON_Flag=1;
// 						command_lost_count=0; 
// 						//Calculate the target speed of three axis from serial data, unit m/s
// 						//从串口数据求三轴目标速度， 单位m/s
// 						Move_X=XYZ_Target_Speed_transition(Receive_Data.buffer[3],Receive_Data.buffer[4]);
// 						Move_Y=XYZ_Target_Speed_transition(Receive_Data.buffer[5],Receive_Data.buffer[6]);
// 						Move_Z=XYZ_Target_Speed_transition(Receive_Data.buffer[7],Receive_Data.buffer[8]);
// 				  }
// 			}
// 		}
// 		  if (Count_moveit == 11)	//验证数据包的长度
// 		{   
// 				Count_moveit=0;//重新开始接收
// 				if(rxbuf_moveit[10] == FRAME_TAIL_MOVEIT) //验证数据包的尾部校验信息
// 				{
					 
// 					for(i=0; i<9; i++)
// 					{
// 						check=rxbuf_moveit[i]^check; //异或，用于检测数据是否出错
// 					}
// 					if(check==rxbuf_moveit[9]) error=0; //检验成功
					
// 					if(error==0)	 //数据校验位计算
// 				  {		
// 						Moveit_Angle1=(short)((rxbuf_moveit[1]<<8)+(rxbuf_moveit[2])); //求X轴速度 分高8位和低8位 单位mm/s
// 						Moveit_Angle2=(short)((rxbuf_moveit[3]<<8)+(rxbuf_moveit[4])); //求X轴速度 分高8位和低8位 单位mm/s
// 						Moveit_Angle3=(short)((rxbuf_moveit[5]<<8)+(rxbuf_moveit[6])); //求Z轴速度 分高8位和低8位 单位mm/s
// 						Moveit_Angle4=(short)((rxbuf_moveit[7]<<8)+(rxbuf_moveit[8])); //求Z轴速度 分高8位和低8位 单位mm/s					
						
// 						Moveit_Angle1=-Moveit_Angle1*0.001f; //单位(弧度)
// 						Moveit_Angle2= Moveit_Angle2*0.001f; 
// 						Moveit_Angle3= Moveit_Angle3*0.001f; 
// 						Moveit_Angle4=-Moveit_Angle4*0.001f;
	
// 					}
// 			  }
// 		 }
// 	} 
//   return 0;	
// }
// void Rx_data_task(void *pvParameters)//接收,DMA+中断接收
// {
// 	 Rx_data_semaphore = xSemaphoreCreateBinary();
// 	 xSemaphoreGive(Rx_data_semaphore); // 启动时就给一次信号量
//    while(1)
//     {	
	
// 		 // 等待数据发送完成的信号
// 			if (xSemaphoreTake(Rx_data_semaphore, portMAX_DELAY) == pdTRUE) {
// 					// 处理所接收的数据
// 					Received_data_processing();
// 			}
// 			//考虑是否要延迟
// 		}
// }
// /**************************************************************************
// Function: Usartx3, Usartx1,Usartx5 and CAN send data task 
// Input   : none
// Output  : none
// 函数功能：串口3、串口1、串口5、CAN发送数据任务
// 入口参数：无
// 返回  值：无
// **************************************************************************/
// void TX_data_task(void *pvParameters)//定时发送,DMA发送
// {
// 	 u32 lastWakeTime = getSysTickCnt();
// //	 tx_data_semaphore = xSemaphoreCreateBinary();
// //	 xSemaphoreGive(tx_data_semaphore); // 启动时就给一次信号量
// //	 init_event_group();
// //	 DMA_ITConfig(DEBUG_USART_DMA_RX_STREAM, DMA_IT_TC, ENABLE);
//    while(1)
//     {	
// 			//The task is run at 20hz
// 			//此任务以20Hz的频率运行
// 			vTaskDelayUntil(&lastWakeTime, F2T(RATE_20_HZ));
// 			data_transition();
// 		  initiate_next_transmission();
			
// //			// 等待 DMA 传输完成的事件标志
// //			EventBits_t uxBits = xEventGroupWaitBits(xEventGroup, DMA_TRANSFER_COMPLETE, pdTRUE, pdFALSE, portMAX_DELAY);

// 			// 如果 DMA 传输完成的标志已经被设置，执行数据处理
// //			if ((uxBits & DMA_TRANSFER_COMPLETE) != 0) {
// 					// 处理数据，执行下一步任务
// //					data_transition();
// //					initiate_next_transmission();
// //			}
// //			
// //		 // 等待数据发送完成的信号
// //			if (xSemaphoreTake(tx_data_semaphore, portMAX_DELAY) == pdTRUE) {
// //					// 处理或启动下一个数据发送
// //					data_transition();
// //				  initiate_next_transmission();
// //			}


// 			/* USART3 向 DMA发出TX请求 */
// //			USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);
// //			USART1_SEND();     //Serial port 1 sends data //串口1发送数据
// //			USART3_SEND();     //Serial port 3 (ROS) sends data  //串口3(ROS)发送数据
// //			USART5_SEND();		 //Serial port 5 (ROS) sends data //串口5(ROS)发送数据
// 			CAN_SEND();        //CAN send data //CAN发送数据	
// 		}
// }
// /**************************************************************************
// Function: The data sent by the serial port is assigned
// Input   : none
// Output  : none
// 函数功能：串口发送的数据进行赋值
// 入口参数：无
// 返回  值：无
// **************************************************************************/
// void data_transition(void)
// {
// 	Send_Data.Sensor_Str.Frame_Header = FRAME_HEADER_CAR; //Frame_header //帧头
// 	Send_Data.Sensor_Str.Frame_Tail = FRAME_TAIL_CAR;     //Frame_tail //帧尾
	
// 	//According to different vehicle types, different kinematics algorithms were selected to carry out the forward kinematics solution, 
// 	//and the three-axis velocity was obtained from each wheel velocity
// 	//根据不同车型选择不同运动学算法进行运动学正解，从各车轮速度求出三轴速度
// 	switch(Car_Mode)
// 	{	
// 		case Mec_Car:      
// 			Send_Data.Sensor_Str.X_speed = ((MOTOR_A.Encoder+MOTOR_B.Encoder+MOTOR_C.Encoder+MOTOR_D.Encoder)/4)*1000;
// 	    Send_Data.Sensor_Str.Y_speed = ((MOTOR_A.Encoder-MOTOR_B.Encoder+MOTOR_C.Encoder-MOTOR_D.Encoder)/4)*1000; 
// 	    Send_Data.Sensor_Str.Z_speed = ((-MOTOR_A.Encoder-MOTOR_B.Encoder+MOTOR_C.Encoder+MOTOR_D.Encoder)/4/(Axle_spacing+Wheel_spacing))*1000;         
// 		  break; 
		
// 		case FourWheel_Car:
//       Send_Data.Sensor_Str.X_speed = ((MOTOR_A.Encoder+MOTOR_B.Encoder+MOTOR_C.Encoder+MOTOR_D.Encoder)/4)*1000; 
// 	    Send_Data.Sensor_Str.Y_speed = 0;
// 	    Send_Data.Sensor_Str.Z_speed = ((-MOTOR_B.Encoder-MOTOR_A.Encoder+MOTOR_C.Encoder+MOTOR_D.Encoder)/2/(Axle_spacing+Wheel_spacing))*1000;
// 		 break; 
		
// 		case Tank_Car:   
// 			Send_Data.Sensor_Str.X_speed = ((MOTOR_A.Encoder+MOTOR_B.Encoder)/2)*1000; 
// 			Send_Data.Sensor_Str.Y_speed = 0;
// 			Send_Data.Sensor_Str.Z_speed = ((MOTOR_B.Encoder-MOTOR_A.Encoder)/(Wheel_spacing)*1000);
// 			break; 
// 	}
	
// 	//The acceleration of the triaxial acceleration //加速度计三轴加速度
// 	Send_Data.Sensor_Str.Accelerometer.X_data= accel[1]; //The accelerometer Y-axis is converted to the ros coordinate X axis //加速度计Y轴转换到ROS坐标X轴
// 	Send_Data.Sensor_Str.Accelerometer.Y_data=-accel[0]; //The accelerometer X-axis is converted to the ros coordinate y axis //加速度计X轴转换到ROS坐标Y轴
// 	Send_Data.Sensor_Str.Accelerometer.Z_data= accel[2]; //The accelerometer Z-axis is converted to the ros coordinate Z axis //加速度计Z轴转换到ROS坐标Z轴
	
// 	//The Angle velocity of the triaxial velocity //角速度计三轴角速度
// 	Send_Data.Sensor_Str.Gyroscope.X_data= gyro[1]; //The Y-axis is converted to the ros coordinate X axis //角速度计Y轴转换到ROS坐标X轴
// 	Send_Data.Sensor_Str.Gyroscope.Y_data=-gyro[0]; //The X-axis is converted to the ros coordinate y axis //角速度计X轴转换到ROS坐标Y轴
// 	if(Flag_Stop==0) 
// 		//If the motor control bit makes energy state, the z-axis velocity is sent normall
// 	  //如果电机控制位使能状态，那么正常发送Z轴角速度
// 		Send_Data.Sensor_Str.Gyroscope.Z_data=gyro[2];  
// 	else  
// 		//If the robot is static (motor control dislocation), the z-axis is 0
//     //如果机器人是静止的（电机控制位失能），那么发送的Z轴角速度为0		
// 		Send_Data.Sensor_Str.Gyroscope.Z_data=0;        
	
// 	//Battery voltage (this is a thousand times larger floating point number, which will be reduced by a thousand times as well as receiving the data).
// 	//电池电压(这里将浮点数放大一千倍传输，相应的在接收端在接收到数据后也会缩小一千倍)
// 	Send_Data.Sensor_Str.Power_Voltage = Voltage*1000; 
	
// 	Send_Data.buffer[0]=Send_Data.Sensor_Str.Frame_Header; //Frame_heade //帧头
//   Send_Data.buffer[1]=Flag_Stop; //Car software loss marker //小车软件失能标志位
	
// 	//The three-axis speed of / / car is split into two eight digit Numbers
// 	//小车三轴速度,各轴都拆分为两个8位数据再发送
// 	Send_Data.buffer[2]=Send_Data.Sensor_Str.X_speed >>8; 
// 	Send_Data.buffer[3]=Send_Data.Sensor_Str.X_speed ;    
// 	Send_Data.buffer[4]=Send_Data.Sensor_Str.Y_speed>>8;  
// 	Send_Data.buffer[5]=Send_Data.Sensor_Str.Y_speed;     
// 	Send_Data.buffer[6]=Send_Data.Sensor_Str.Z_speed >>8; 
// 	Send_Data.buffer[7]=Send_Data.Sensor_Str.Z_speed ;    
	
// 	//The acceleration of the triaxial axis of / / imu accelerometer is divided into two eight digit reams
// 	//IMU加速度计三轴加速度,各轴都拆分为两个8位数据再发送
// 	Send_Data.buffer[8]=Send_Data.Sensor_Str.Accelerometer.X_data>>8; 
// 	Send_Data.buffer[9]=Send_Data.Sensor_Str.Accelerometer.X_data;   
// 	Send_Data.buffer[10]=Send_Data.Sensor_Str.Accelerometer.Y_data>>8;
// 	Send_Data.buffer[11]=Send_Data.Sensor_Str.Accelerometer.Y_data;
// 	Send_Data.buffer[12]=Send_Data.Sensor_Str.Accelerometer.Z_data>>8;
// 	Send_Data.buffer[13]=Send_Data.Sensor_Str.Accelerometer.Z_data;
	
// 	//The axis of the triaxial velocity of the / /imu is divided into two eight digits
// 	//IMU角速度计三轴角速度,各轴都拆分为两个8位数据再发送
// 	Send_Data.buffer[14]=Send_Data.Sensor_Str.Gyroscope.X_data>>8;
// 	Send_Data.buffer[15]=Send_Data.Sensor_Str.Gyroscope.X_data;
// 	Send_Data.buffer[16]=Send_Data.Sensor_Str.Gyroscope.Y_data>>8;
// 	Send_Data.buffer[17]=Send_Data.Sensor_Str.Gyroscope.Y_data;
// 	Send_Data.buffer[18]=Send_Data.Sensor_Str.Gyroscope.Z_data>>8;
// 	Send_Data.buffer[19]=Send_Data.Sensor_Str.Gyroscope.Z_data;
	
// 	//Battery voltage, split into two 8 digit Numbers
// 	//电池电压,拆分为两个8位数据发送
// 	Send_Data.buffer[20]=Send_Data.Sensor_Str.Power_Voltage >>8; 
// 	Send_Data.buffer[21]=Send_Data.Sensor_Str.Power_Voltage; 

//   //Data check digit calculation, Pattern 1 is a data check
//   //数据校验位计算，模式1是发送数据校验
// 	Send_Data.buffer[22]=Check_Sum(22,1); 
	
// 	Send_Data.buffer[23]=Send_Data.Sensor_Str.Frame_Tail; //Frame_tail //帧尾
// }
// /**************************************************************************
// Function: Serial port 1 sends data
// Input   : none
// Output  : none
// 函数功能：串口1发送数据
// 入口参数：无
// 返回  值：无
// **************************************************************************/
// void USART1_SEND(void)
// {
//   unsigned char i = 0;	
	
// 	for(i=0; i<24; i++)
// 	{
// 		usart1_send(Send_Data.buffer[i]);
// 	}	 
// }
// /**************************************************************************
// Function: Serial port 3 sends data
// Input   : none
// Output  : none
// 函数功能：串口3发送数据
// 入口参数：无
// 返回  值：无
// **************************************************************************/
// void USART3_SEND(void)
// {
//   unsigned char i = 0;	
// 	for(i=0; i<24; i++)
// 	{
// 		usart3_send(Send_Data.buffer[i]);
// 	}	 
// }
// /**************************************************************************
// Function: Serial port 5 sends data
// Input   : none
// Output  : none
// 函数功能：串口5发送数据
// 入口参数：无
// 返回  值：无
// **************************************************************************/
// void USART5_SEND(void)
// {
//   unsigned char i = 0;	
// 	for(i=0; i<24; i++)
// 	{
// 		usart5_send(Send_Data.buffer[i]);
// 	}	 
// }
// /**************************************************************************
// Function: CAN sends data
// Input   : none
// Output  : none
// 函数功能：CAN发送数据
// 入口参数：无
// 返 回 值：无
// **************************************************************************/
// void CAN_SEND(void) 
// {
// 	u8 CAN_SENT[8],i;
	
// 	for(i=0;i<8;i++)
// 	{
// 	  CAN_SENT[i]=Send_Data.buffer[i];
// 	}
// 	CAN_Send_Num(0x101,CAN_SENT);
	
// 	for(i=0;i<8;i++)
// 	{
// 	  CAN_SENT[i]=Send_Data.buffer[i+8];
// 	}
// 	CAN_Send_Num(0x102,CAN_SENT);
	
// 	for(i=0;i<8;i++)
// 	{
// 	  CAN_SENT[i]=Send_Data.buffer[i+16];
// 	}
// 	CAN_Send_Num(0x103,CAN_SENT);
// }
// /**************************************************************************
// Function: Serial port 1 initialization
// Input   : none
// Output  : none
// 函数功能：串口1初始化
// 入口参数：无
// 返 回 值：无
// **************************************************************************/
// void uart1_init(u32 bound)
// {  	 
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 	USART_InitTypeDef USART_InitStructure;
// 	NVIC_InitTypeDef NVIC_InitStructure;
	
// 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	 //Enable the gpio clock //使能GPIO时钟
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //Enable the Usart clock //使能USART时钟

// 	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);	
// 	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10 ,GPIO_AF_USART1);	 
	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
// 	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;            //输出模式
// 	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;          //推挽输出
// 	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;       //高速50MHZ
// 	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;            //上拉
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);  		          //初始化
	
//   //UsartNVIC configuration //UsartNVIC配置
// 	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
// 	//Preempt priority //抢占优先级
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
// 	//Subpriority //子优先级
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
// 	//Enable the IRQ channel //IRQ通道使能
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
//   //Initialize the VIC register with the specified parameters 
// 	//根据指定的参数初始化VIC寄存器	
// 	NVIC_Init(&NVIC_InitStructure);	
	
//   //USART Initialization Settings 初始化设置
// 	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
// 	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
// 	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止位
// 	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
// 	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
// 	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
// 	USART_Init(USART1, &USART_InitStructure); //Initialize serial port 1 //初始化串口1
	
// 	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
// 	USART_Cmd(USART1, ENABLE);                     //Enable serial port 1 //使能串口1
// }
// /**************************************************************************
// Function: Serial port 2 initialization
// Input   : none
// Output  : none
// 函数功能：串口2初始化
// 入口参数：无
// 返回  值：无
// **************************************************************************/
// void uart2_init(u32 bound)
// {  	 
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 	USART_InitTypeDef USART_InitStructure;
// 	NVIC_InitTypeDef NVIC_InitStructure;

// 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //Enable the Usart clock //使能USART时钟
	
// 	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);	
// 	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6 ,GPIO_AF_USART2);	 
	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
// 	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;            //输出模式
// 	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;          //推挽输出
// 	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;       //高速50MHZ
// 	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;            //上拉
// 	GPIO_Init(GPIOD, &GPIO_InitStructure);  		          //初始化
	
// 	//UsartNVIC configuration //UsartNVIC配置
// 	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
// 	//Preempt priority //抢占优先级
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
// 	//Subpriority //子优先级
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
//   //Enable the IRQ channel //IRQ通道使能	
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//   //Initialize the VIC register with the specified parameters 
// 	//根据指定的参数初始化VIC寄存器		
// 	NVIC_Init(&NVIC_InitStructure);	
	
// 	//USART Initialization Settings 初始化设置
// 	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
// 	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
// 	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止
// 	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
// 	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
// 	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
// 	USART_Init(USART2, &USART_InitStructure);      //Initialize serial port 2 //初始化串口2
	
// 	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
// 	USART_Cmd(USART2, ENABLE);                     //Enable serial port 2 //使能串口2 
// }
// /**************************************************************************
// Function: Serial port 3 initialization
// Input   : none
// Output  : none
// 函数功能：串口3初始化
// 入口参数：无
// 返回  值：无
// **************************************************************************/
// void uart3_init(u32 bound)
// {  	 
//   GPIO_InitTypeDef GPIO_InitStructure;
//   USART_InitTypeDef USART_InitStructure;
		
//   RCC_AHB1PeriphClockCmd( DEBUG_USART_RX_GPIO_CLK|DEBUG_USART_TX_GPIO_CLK, ENABLE);

//   /* Enable UART clock */
//   RCC_APB1PeriphClockCmd(DEBUG_USART_CLK, ENABLE);
  
//   /* Connect PXx to USARTx_Tx*/
//   GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);

//   /* Connect PXx to USARTx_Rx*/
//   GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF);

//   /* Configure USART Tx as alternate function  */
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

//   GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

//   /* Configure USART Rx as alternate function  */
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//   GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
//   GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
			
//   /* USART mode config */
//   USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
//   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//   USART_InitStructure.USART_StopBits = USART_StopBits_1;
//   USART_InitStructure.USART_Parity = USART_Parity_No ;
//   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//   USART_Init(DEBUG_USART, &USART_InitStructure); 
//   USART_Cmd(DEBUG_USART, ENABLE);
// }
/* 全局变量说明 -----------------------------------------------------------*/
extern uint8_t isReadyForStandby;  // 来自其他文件的待机准备标志（可能用于低功耗控制）
extern uint16_t Hour, Minute, Second; // 来自RTC模块的时间变量（时/分/秒）

SEND_DATA Send_Data;      // 发送数据结构体（需查看定义了解具体字段）
RECEIVE_DATA Receive_Data; // 接收数据结构体（需查看定义了解具体字段）
SemaphoreHandle_t Rx_data_semaphore; // FreeRTOS信号量，用于DMA接收完成通知
EventGroupHandle_t xEventGroup;     // FreeRTOS事件标志组，用于任务同步
extern int Time_count;    // 来自其他文件的时间计数器

/**************************************************************************
函数功能：创建FreeRTOS事件标志组
入口参数：无
返 回 值：无
说明：用于多任务间的事件通知（如数据接收完成、错误状态等）
**************************************************************************/
void init_event_group() {
    xEventGroup = xEventGroupCreate();  // 创建事件标志组
    if (xEventGroup == NULL) {          // 创建失败处理
        // 实际项目中建议添加错误处理（如系统复位或LED报警）
    }
}

/**************************************************************************
函数功能：重定向printf输出到USART3
入口参数：ch - 要发送的字符, f - 文件指针（未使用）
返 回 值：发送的字符
说明：实现标准库printf的串口输出，采用阻塞式发送
**************************************************************************/
int fputc(int ch, FILE *f) {
    USART_SendData(DEBUG_USART, (uint8_t) ch);  // 发送单字节数据
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET); // 等待发送完成
    return ch;  // 返回发送的字符
}

/**************************************************************************
函数功能：重定向scanf输入到USART3
入口参数：f - 文件指针（未使用）
返 回 值：接收到的字符
说明：实现标准库scanf的串口输入，采用阻塞式接收
**************************************************************************/
int fgetc(FILE *f) {
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET); // 等待接收完成
    return (int)USART_ReceiveData(DEBUG_USART); // 返回接收到的字符
}

/**************************************************************************
函数功能：启动DMA串口传输
入口参数：无
返 回 值：无
说明：配置并启动USART3的DMA发送（需配合USART_DMA_Config函数使用）
**************************************************************************/
void initiate_next_transmission(void) {
    USART_DMA_Config(); // 假设该函数已正确配置DMA（流、通道、方向等）
    USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE); // 使能USART的DMA发送请求
    
    /* 以下为被注释的原始DMA控制代码 */
    // DMA_ITConfig(DEBUG_USART_DMA_STREAM, DMA_IT_TC, ENABLE); // 使能传输完成中断
    // DMA_Cmd(DEBUG_USART_DMA_STREAM, DISABLE); // 禁用DMA
    // while (DMA_GetCmdStatus(...)); // 等待禁用完成
    // DEBUG_USART_DMA_STREAM->M0AR = (uint32_t)&Send_Data; // 设置内存地址
    // DEBUG_USART_DMA_STREAM->NDTR = SEND_DATA_SIZE; // 设置传输数据量
    // DMA_Cmd(DEBUG_USART_DMA_STREAM, ENABLE); // 重新使能DMA
}
/**************************************************************************
函数功能：串口数据解析处理
入口参数：无
返 回 值：0（固定返回0，实际状态通过全局变量传递）
说明：处理USART3接收的三种数据帧（底盘控制/机械臂控制/休眠时间设置）
      数据帧格式：[帧头]+[数据区]+[校验]+[帧尾]
      采用异或校验，临界区保护关键数据修改
**************************************************************************/
//进阶点:这里数据处理有自行可提高的环节：想一想如何处理粘包、错位帧的情况？
int Received_data_processing(void)
{
    int check=0, error=1, i=0;   // 校验计算变量

		// 系统启动后延迟处理（等待系统稳定）
		if(Time_count < CONTROL_DELAY) return 0;  
	
		/****** 休眠时间帧处理（11字节） ******/
		if(Receive_Data.buffer[0] == FRAME_HEADER_TIME && Receive_Data.buffer[10] == FRAME_TAIL_TIME) // 验证帧头0X6A+帧尾0X6B
		{
			  //校验变量清0
			  check = 0;
			  error = 1;
				// 异或校验（数据字节0-8）
				for(i=0; i<9; i++) check ^= Receive_Data.buffer[i];
				
				if(check == Receive_Data.buffer[9]) error = 0; // 校验位比对

				if(error==0 && Receive_Data.buffer[2]==1) // 校验成功且模式有效
				{
						taskENTER_CRITICAL(); // 进入临界区（防止任务切换）
						isReadyForStandby = Receive_Data.buffer[2]; // 待机模式标志
						Hour = (Receive_Data.buffer[3]<<8) + Receive_Data.buffer[4];   // 小时（高位在前）
						Minute = (Receive_Data.buffer[5]<<8) + Receive_Data.buffer[6]; // 分钟
						Second = (Receive_Data.buffer[7]<<8) + Receive_Data.buffer[8]; // 秒钟
						taskEXIT_CRITICAL();  // 退出临界区
				}
		}
		/****** 底盘控制帧处理（11字节） ******/
		else if(Receive_Data.buffer[0] == FRAME_HEADER_CAR && Receive_Data.buffer[10] == FRAME_TAIL_CAR) // 验证帧头0X7B+帧尾0X7D
		{
			  //校验变量清0
			  check = 0;
			  error = 1;
				// 校验计算（模式0为接收校验）
				if(Receive_Data.buffer[9] == Check_Sum(9,0)) 
				{
						// 禁用其他控制方式
						PS2_ON_Flag=0; Remote_ON_Flag=0; APP_ON_Flag=0; 
						CAN_ON_Flag=0; Usart1_ON_Flag=0; Usart5_ON_Flag=0;
						command_lost_count=0; // 丢失指令计数器清零
						
						// 解析三轴速度（单位转换为m/s）
						Move_X = XYZ_Target_Speed_transition(Receive_Data.buffer[3], Receive_Data.buffer[4]);
						Move_Y = XYZ_Target_Speed_transition(Receive_Data.buffer[5], Receive_Data.buffer[6]);
						Move_Z = XYZ_Target_Speed_transition(Receive_Data.buffer[7], Receive_Data.buffer[8]);
				}
		}
		/****** 机械臂控制帧处理（11字节） ******/
		else if(Receive_Data.buffer[0] == FRAME_HEADER_MOVEIT && Receive_Data.buffer[10] == FRAME_TAIL_MOVEIT)  // 验证帧头0XAA+帧尾0XBB
		{   
			  //校验变量清0
			  check = 0;
			  error = 1;
				// 异或校验（数据字节0-8）
				for(i=0; i<9; i++) check ^= Receive_Data.buffer[i];
				
				if(check == Receive_Data.buffer[9]) error = 0;

				if(error == 0) 
				{
						// 解析四个关节角度（原始单位0.001弧度）
						Moveit_Angle1 = (short)((Receive_Data.buffer[1]<<8) + Receive_Data.buffer[2]) * -0.001f;
						Moveit_Angle2 = (short)((Receive_Data.buffer[3]<<8) + Receive_Data.buffer[4]) * 0.001f;
						Moveit_Angle3 = (short)((Receive_Data.buffer[5]<<8) + Receive_Data.buffer[6]) * 0.001f;
						Moveit_Angle4 = (short)((Receive_Data.buffer[7]<<8) + Receive_Data.buffer[8]) * -0.001f;
				}
		}
 
    return 0; // 固定返回0
}
/**************************************************************************
函数功能：串口数据接收任务（DMA+中断模式）
入口参数：pvParameters - FreeRTOS任务参数（未使用）
返 回 值：无
说明：通过信号量同步方式处理USART3接收数据，20Hz轮询
**************************************************************************/
void Rx_data_task(void *pvParameters)
{
    // 创建二进制信号量（用于接收完成通知）
    Rx_data_semaphore = xSemaphoreCreateBinary();
    // 初始释放信号量（确保首次进入等待前可立即处理）
    xSemaphoreGive(Rx_data_semaphore); 
    
    while(1) {
        // 无限等待信号量（portMAX_DELAY表示永久阻塞）
        if (xSemaphoreTake(Rx_data_semaphore, portMAX_DELAY) == pdTRUE) {
            // 信号量获取成功，处理接收数据
            Received_data_processing(); // 调用数据解析函数
        }
        // 注：实际接收在中断中释放信号量
    }
}

/**************************************************************************
函数功能：多通道数据发送任务（USART1/3/5 + CAN）
入口参数：pvParameters - FreeRTOS任务参数（未使用）
返 回 值：无
说明：以20Hz频率周期发送数据，使用DMA提升效率
**************************************************************************/
void TX_data_task(void *pvParameters)
{
    u32 lastWakeTime = getSysTickCnt(); // 获取系统节拍基准时间
    
    while(1) {
        // 固定频率运行（20Hz，即50ms周期）
        vTaskDelayUntil(&lastWakeTime, F2T(RATE_20_HZ));
        
        // 数据预处理（填充Send_Data结构体）
        data_transition(); 
        
        // 启动USART的DMA传输
        initiate_next_transmission(); 
        
        // CAN总线数据发送（独立于串口发送）
        CAN_SEND(); 

        /* 以下为被注释的备选同步方案 */
        // 方案1：事件组等待DMA完成标志
        // EventBits_t uxBits = xEventGroupWaitBits(xEventGroup, 
        //                        DMA_TRANSFER_COMPLETE, 
        //                        pdTRUE, pdFALSE, portMAX_DELAY);
        // if (uxBits & DMA_TRANSFER_COMPLETE) {
        //    data_transition();
        //    initiate_next_transmission();
        // }
        
        // 方案2：信号量同步发送
        // if (xSemaphoreTake(tx_data_semaphore, portMAX_DELAY) == pdTRUE) {
        //    data_transition();
        //    initiate_next_transmission();
        // }
        
        // 方案3：直接控制USART DMA请求（当前使用）
        // USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);
        // USART1_SEND(); // 串口1发送（已注释）
        // USART3_SEND(); // 串口3发送（已注释） 
        // USART5_SEND(); // 串口5发送（已注释）
    }
}
/**************************************************************************
函数功能：发送数据协议封装
入口参数：无
返 回 值：无
说明：将底盘运动数据、IMU信息、电源状态封装为串口通信协议
       协议帧结构：[头][状态][速度x3][加速度x3][角速度x3][电压][校验][尾]
**************************************************************************/
void data_transition(void)
{
    /*———————协议帧头设置———————*/
    Send_Data.Sensor_Str.Frame_Header = FRAME_HEADER_CAR; // 0X7B
    Send_Data.Sensor_Str.Frame_Tail = FRAME_TAIL_CAR;    // 0X7D

    /*———————运动学正解计算———————*/
    switch(Car_Mode) {
        case Mec_Car: // 全向轮模式
            Send_Data.Sensor_Str.X_speed = ((MOTOR_A.Encoder + MOTOR_B.Encoder + 
                                           MOTOR_C.Encoder + MOTOR_D.Encoder)/4)*1000; // X轴合成速度(mm/s)
            Send_Data.Sensor_Str.Y_speed = ((MOTOR_A.Encoder - MOTOR_B.Encoder + 
                                           MOTOR_C.Encoder - MOTOR_D.Encoder)/4)*1000; // Y轴合成速度
            Send_Data.Sensor_Str.Z_speed = ((-MOTOR_A.Encoder - MOTOR_B.Encoder + 
                                           MOTOR_C.Encoder + MOTOR_D.Encoder)/4/(Axle_spacing + Wheel_spacing))*1000; // 旋转角速度(0.001rad/s)
            break;

        case FourWheel_Car: // 四轮差速模式
            Send_Data.Sensor_Str.X_speed = ((MOTOR_A.Encoder + MOTOR_B.Encoder + 
                                           MOTOR_C.Encoder + MOTOR_D.Encoder)/4)*1000; 
            Send_Data.Sensor_Str.Y_speed = 0; // 无侧向移动能力
            Send_Data.Sensor_Str.Z_speed = ((-MOTOR_B.Encoder - MOTOR_A.Encoder + 
                                           MOTOR_C.Encoder + MOTOR_D.Encoder)/2/(Axle_spacing + Wheel_spacing))*1000;
            break;

        case Tank_Car: // 双轮差速模式
            Send_Data.Sensor_Str.X_speed = ((MOTOR_A.Encoder + MOTOR_B.Encoder)/2)*1000;
            Send_Data.Sensor_Str.Y_speed = 0;
            Send_Data.Sensor_Str.Z_speed = ((MOTOR_B.Encoder - MOTOR_A.Encoder)/Wheel_spacing)*1000; 
            break;
    }

    /*———————IMU坐标系转换———————*/
    // 加速度计数据转换（原始传感器坐标系→ROS坐标系）
    Send_Data.Sensor_Str.Accelerometer.X_data = accel[1];  // 传感器Y→ROS X
    Send_Data.Sensor_Str.Accelerometer.Y_data = -accel[0]; // 传感器X→ROS Y（取反）
    Send_Data.Sensor_Str.Accelerometer.Z_data = accel[2];  // 传感器Z→ROS Z

    // 陀螺仪数据转换
    Send_Data.Sensor_Str.Gyroscope.X_data = gyro[1];   // 传感器Y→ROS X
    Send_Data.Sensor_Str.Gyroscope.Y_data = -gyro[0]; // 传感器X→ROS Y（取反）
    Send_Data.Sensor_Str.Gyroscope.Z_data = (Flag_Stop ? 0 : gyro[2]); // 静止时Z轴归零

    /*———————电源数据转换———————*/
    Send_Data.Sensor_Str.Power_Voltage = Voltage * 1000; // 浮点电压放大千倍传输

    /*———————协议数据打包———————*/
    // 帧头与状态位
    Send_Data.buffer[0] = Send_Data.Sensor_Str.Frame_Header; // 0
    Send_Data.buffer[1] = Flag_Stop; // 1-使能状态位

    // 三轴速度拆分（16bit→2x8bit）
    Send_Data.buffer[2] = Send_Data.Sensor_Str.X_speed >> 8;  // 2-X高8位
    Send_Data.buffer[3] = Send_Data.Sensor_Str.X_speed;       // 3-X低8位
    Send_Data.buffer[4] = Send_Data.Sensor_Str.Y_speed >> 8;  // 4-Y高8位
    Send_Data.buffer[5] = Send_Data.Sensor_Str.Y_speed;       // 5-Y低8位
    Send_Data.buffer[6] = Send_Data.Sensor_Str.Z_speed >> 8;  // 6-Z高8位
    Send_Data.buffer[7] = Send_Data.Sensor_Str.Z_speed;       // 7-Z低8位

    // 加速度数据拆分
    Send_Data.buffer[8]  = Send_Data.Sensor_Str.Accelerometer.X_data >> 8; // 8
    Send_Data.buffer[9]  = Send_Data.Sensor_Str.Accelerometer.X_data;     // 9 
		Send_Data.buffer[10]=Send_Data.Sensor_Str.Accelerometer.Y_data>>8;
		Send_Data.buffer[11]=Send_Data.Sensor_Str.Accelerometer.Y_data;
		Send_Data.buffer[12]=Send_Data.Sensor_Str.Accelerometer.Z_data>>8;
		Send_Data.buffer[13]=Send_Data.Sensor_Str.Accelerometer.Z_data;
		
		//The axis of the triaxial velocity of the / /imu is divided into two eight digits
		//IMU角速度计三轴角速度,各轴都拆分为两个8位数据再发送
		Send_Data.buffer[14]=Send_Data.Sensor_Str.Gyroscope.X_data>>8;
		Send_Data.buffer[15]=Send_Data.Sensor_Str.Gyroscope.X_data;
		Send_Data.buffer[16]=Send_Data.Sensor_Str.Gyroscope.Y_data>>8;
		Send_Data.buffer[17]=Send_Data.Sensor_Str.Gyroscope.Y_data;
		Send_Data.buffer[18]=Send_Data.Sensor_Str.Gyroscope.Z_data>>8;
		Send_Data.buffer[19]=Send_Data.Sensor_Str.Gyroscope.Z_data;
		
		//Battery voltage, split into two 8 digit Numbers
		//电池电压,拆分为两个8位数据发送
		Send_Data.buffer[20]=Send_Data.Sensor_Str.Power_Voltage >>8; 
		Send_Data.buffer[21]=Send_Data.Sensor_Str.Power_Voltage; 

    // 校验位计算（模式1校验算法）
    Send_Data.buffer[22] = Check_Sum(22,1); // 22-校验位
    
    // 帧尾
    Send_Data.buffer[23] = Send_Data.Sensor_Str.Frame_Tail; // 23
}
/**************************************************************************
函数功能：USART1数据发送
入口参数：无
返 回 值：无
说明：通过USART1逐字节发送24字节数据包，用于调试或辅助通信
**************************************************************************/
void USART1_SEND(void)
{
    unsigned char i = 0;	
    // 遍历发送缓冲区（0-23共24字节）
    for(i=0; i<24; i++) {
        usart1_send(Send_Data.buffer[i]); // 调用底层字节发送函数
    }	 
}

/**************************************************************************
函数功能：USART3数据发送
入口参数：无
返 回 值：无
说明：通过USART3逐字节发送24字节数据包，通常用于ROS通信
**************************************************************************/
void USART3_SEND(void)
{
    unsigned char i = 0;	
    for(i=0; i<24; i++) {
        usart3_send(Send_Data.buffer[i]); // 调用底层字节发送函数
    }	 
}

/**************************************************************************
函数功能：USART5数据发送
入口参数：无
返 回 值：无
说明：通过USART5逐字节发送24字节数据包，用于扩展通信
**************************************************************************/
void USART5_SEND(void)
{
    unsigned char i = 0;	
    for(i=0; i<24; i++) {
        usart5_send(Send_Data.buffer[i]); // 调用底层字节发送函数
    }	 
}

/**************************************************************************
函数功能：CAN总线数据发送
入口参数：无
返 回 值：无
说明：将24字节数据分3帧通过CAN发送，ID分别为0x101/0x102/0x103
      每帧8字节，符合CAN 2.0B标准数据帧格式
**************************************************************************/
void CAN_SEND(void) 
{
    u8 CAN_SENT[8], i; // CAN数据暂存区
    
    // 第一帧：字节0-7，ID 0x101
    for(i=0; i<8; i++) {
        CAN_SENT[i] = Send_Data.buffer[i];
    }
    CAN_Send_Num(0x101, CAN_SENT); // 发送标准ID数据帧
    
    // 第二帧：字节8-15，ID 0x102
    for(i=0; i<8; i++) {
        CAN_SENT[i] = Send_Data.buffer[i+8];
    }
    CAN_Send_Num(0x102, CAN_SENT);
    
    // 第三帧：字节16-23，ID 0x103
    for(i=0; i<8; i++) {
        CAN_SENT[i] = Send_Data.buffer[i+16];
    }
    CAN_Send_Num(0x103, CAN_SENT);
}
/**************************************************************************
函数功能：USART1初始化配置
入口参数：bound - 波特率（如115200）
返 回 值：无
说明：配置PA9(TX)/PA10(RX)为USART1功能，启用接收中断
**************************************************************************/
void uart1_init(u32 bound)
{  	 
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 时钟使能 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);   // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  // 使能USART1时钟

    /* GPIO复用配置 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);  // PA9复用为USART1_TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);  // PA10复用为USART1_RX

    /* GPIO参数设置 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;     // 引脚选择
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;          // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       // 推挽输出（TX需要）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    // 高速模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         // 上拉增强抗干扰
    GPIO_Init(GPIOA, &GPIO_InitStructure);               // 应用配置

    /* 中断优先级配置 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;    // USART1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级1级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   // 子优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        // 使能中断
    NVIC_Init(&NVIC_InitStructure);                        // 写入NVIC寄存器

    /* USART参数配置 */
    USART_InitStructure.USART_BaudRate = bound;            // 设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      // 1位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;         // 无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(USART1, &USART_InitStructure);              // 应用配置

    /* 使能中断与串口 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // 使能接收中断（RXNE：接收寄存器非空）
    USART_Cmd(USART1, ENABLE);                      // 启动USART1
}

/**************************************************************************
函数功能：USART2初始化配置
入口参数：bound - 波特率（如115200）
返 回 值：无
说明：配置PD5(TX)/PD6(RX)为USART2功能，启用接收中断
**************************************************************************/
void uart2_init(u32 bound)
{  	 
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 时钟使能 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);   // 使能GPIOD时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  // 使能USART2时钟

    /* GPIO复用配置 */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);  // PD5复用为USART2_TX
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);  // PD6复用为USART2_RX

    /* GPIO参数设置 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;     // 引脚选择
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;          // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    // 高速模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         // 上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);               // 应用配置

    /* 中断优先级配置 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;    // USART2中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级1级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   // 子优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       // 使能中断
    NVIC_Init(&NVIC_InitStructure);                        // 写入NVIC寄存器

    /* USART参数配置（同USART1）*/
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);              // 应用配置到USART2

    /* 使能中断与串口 */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  // 使能接收中断
    USART_Cmd(USART2, ENABLE);                      // 启动USART2
}
/**************************************************************************
函数功能：USART3初始化配置
入口参数：bound - 波特率（如115200）
返 回 值：无
说明：配置USART3的GPIO和通信参数，未启用中断
**************************************************************************/
void uart3_init(u32 bound)
{  	 
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* 时钟使能（通过预定义宏选择具体引脚）*/
    RCC_AHB1PeriphClockCmd( DEBUG_USART_RX_GPIO_CLK | DEBUG_USART_TX_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(DEBUG_USART_CLK, ENABLE);  // USART3属于APB1外设

    /* GPIO复用配置*/
    GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT, DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);
    GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_SOURCE, DEBUG_USART_TX_AF);

    /* TX引脚配置（推挽输出）*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      // 上拉增强信号
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN;  // PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 高速模式
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* RX引脚配置（复用输入）*/
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;  // 如PC11
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* USART参数配置（8N1模式）*/
    USART_InitStructure.USART_BaudRate = bound;            // 设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      // 1停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;         // 无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无流控
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // 收发模式
    USART_Init(DEBUG_USART, &USART_InitStructure);        // 应用配置到USART3

    USART_Cmd(DEBUG_USART, ENABLE);  // 使能USART3
}

/**************************************************************************
函数功能：USART3 DMA配置（发送/发送）
入口参数：无
返 回 值：无
说明：配置DMA1 Stream3用于发送，DMA1 Stream1用于循环接收
**************************************************************************/
void USART_DMA_Config(void)
{

    DMA_InitTypeDef DMA_InitStructure;

    /*--------------------- DMA发送配置 ---------------------*/
    RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_TX_CLK, ENABLE); // 使能DMA1时钟
    
    DMA_DeInit(DEBUG_USART_DMA_TX_STREAM); // 复位DMA1 Stream3
    while (DMA_GetCmdStatus(DEBUG_USART_DMA_TX_STREAM) != DISABLE); // 等待复位完成

  	/*usart3 tx对应dma1，数据流3*/	
		DMA_InitStructure.DMA_Channel = DEBUG_USART_DMA_TX_CHANNEL;  
		/*设置DMA源：串口数据寄存器地址*/
		DMA_InitStructure.DMA_PeripheralBaseAddr = DEBUG_USART_DR_BASE;	 
		/*内存地址(要传输的变量的指针)*/
		DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&Send_Data;
		/*方向：从内存到外设*/		
		DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;	
		/*传输大小DMA_BufferSize=SENDBUFF_SIZE*/	
		DMA_InitStructure.DMA_BufferSize = SEND_BUFFER_SIZE;
		/*外设地址不增*/	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
		/*内存地址自增*/
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
		/*外设数据单位*/	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		/*内存数据单位 8bit*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
		/*DMA模式：一次循环*/
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		/*DMA模式：不断循环*/
		//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 
		/*优先级：中*/	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
		/*禁用FIFO*/
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
		/*存储器突发传输 16个节拍*/
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
		/*外设突发传输 1个节拍*/
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
		/*配置DMA1的数据流3*/		   
		DMA_Init(DEBUG_USART_DMA_TX_STREAM, &DMA_InitStructure);
		
		/*使能DMA*/
		DMA_Cmd(DEBUG_USART_DMA_TX_STREAM, ENABLE);
		
		/* 等待DMA数据流有效*/
		while(DMA_GetCmdStatus(DEBUG_USART_DMA_TX_STREAM) != ENABLE)
		{
		}
    /*--------------------- DMA接收配置 ---------------------*/
    RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_RX_CLK, ENABLE); // 使能DMA1时钟
    
    DMA_DeInit(DEBUG_USART_DMA_RX_STREAM); // 复位DMA1 Stream1
    while (DMA_GetCmdStatus(DEBUG_USART_DMA_RX_STREAM) != DISABLE);

    /* 接收流参数配置 */
    DMA_InitStructure.DMA_Channel = DEBUG_USART_DMA_RX_CHANNEL;  // 通道4
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR; // 外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&Receive_Data;  // 接收缓冲区
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;          // 外设->内存
    DMA_InitStructure.DMA_BufferSize = RX_BUFFER_SIZE;                // 接收缓冲区长度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                  // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;               // 高优先级
    DMA_Init(DEBUG_USART_DMA_RX_STREAM, &DMA_InitStructure);         // 应用配置
    
    DMA_Cmd(DEBUG_USART_DMA_RX_STREAM, ENABLE); // 使能接收DMA

    /* 接收中断配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_DMA_RX_STREAM_IRQn; // DMA1 Stream1中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    // 最高抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           // 最高子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    DMA_ITConfig(DEBUG_USART_DMA_RX_STREAM, DMA_IT_TC, ENABLE); // 使能传输完成中断
}

/**************************************************************************
Function: Serial port 5 initialization
Input   : none
Output  : none
函数功能：串口5初始化
入口参数：无
返回  值：无
**************************************************************************/
void uart5_init(u32 bound)
{  	 
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//PC12 TX
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
		//PD2 RX
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); //Enable the Usart clock //使能USART时钟

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2 ,GPIO_AF_UART5);	 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;            //输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;          //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;       //高速50MHZ
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;            //上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);  		          //初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;            //输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;          //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;       //高速50MHZ
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;            //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);  		          //初始化
	
  //UsartNVIC configuration //UsartNVIC配置
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	//Preempt priority //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
	//Preempt priority //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	//Enable the IRQ channel //IRQ通道使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  //Initialize the VIC register with the specified parameters 
	//根据指定的参数初始化VIC寄存器		
	NVIC_Init(&NVIC_InitStructure);
	
  //USART Initialization Settings 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
  USART_Init(UART5, &USART_InitStructure);      //Initialize serial port 5 //初始化串口5
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
  USART_Cmd(UART5, ENABLE);                     //Enable serial port 5 //使能串口5
}

// DMA1流1中断处理程序（接收）
void DMA1_Stream1_IRQHandler(void) {	

     // 检查 DMA 传输完成标志
    if (DMA_GetITStatus(DEBUG_USART_DMA_RX_STREAM, DMA_IT_TCIF1)) {
			  // 清除中断标志
        DMA_ClearITPendingBit(DEBUG_USART_DMA_RX_STREAM, DMA_IT_TCIF1);
			 
        // 创建一个BaseType_t类型的变量，指示是否需要任务切换
        BaseType_t xHigherPriorityTaskWoken = pdTRUE;

        // 通知 FreeRTOS 任务数据传输已完成，并检查是否需要进行任务切换
        if (xSemaphoreGiveFromISR(Rx_data_semaphore, &xHigherPriorityTaskWoken) != pdTRUE) {
            // 如果信号量释放失败，打印调试信息或进行其他处理
            // 例如，添加一个错误标志或者重试机制
        }

        // 如果需要任务切换，触发上下文切换
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);			
    }
}

/**************************************************************************
函数功能：串口1接收中断
入口参数：无
返回  值：无
说明：处理通过USART1接收到的数据，进行校验并更新目标速度
**************************************************************************/
int USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 检查是否接收到数据
    {
        u8 Usart_Receive;
        static u8 Count_car, Count_moveit;
        static u8 rxbuf_car[11], rxbuf_moveit[16];
        int check = 0, error = 1, i;

        Usart_Receive = USART_ReceiveData(USART1); // 读取数据
        if (Time_count < CONTROL_DELAY)
            // 开机10秒前不处理数据
            return 0;

        // 读取到完整数据，开始校验，校验成功则赋值XYZ目标速度
        rxbuf_car[Count_car] = Usart_Receive;
        rxbuf_moveit[Count_moveit] = Usart_Receive;

        // 确保数组第一个数据为FRAME_HEADER
        if (Usart_Receive == FRAME_HEADER_CAR || Count_car > 0)
            Count_car++;
        else
            Count_car = 0;

        if (Usart_Receive == FRAME_HEADER_MOVEIT || Count_moveit > 0)
            Count_moveit++;
        else
            Count_moveit = 0;

        if (Count_car == 11) // 验证数据包的长度
        {
            Count_car = 0; // 重新开始接收
            if (rxbuf_car[10] == FRAME_TAIL_CAR) // 验证数据包的帧尾
            {
                for (i = 0; i < 9; i++)
                {
                    // 异或位校验，用于检测数据是否出错
                    check = rxbuf_car[i] ^ check;
                }
                if (check == rxbuf_car[9])
                    // 异或位校验成功
                    error = 0;

                if (error == 0)
                {
                    if (Usart1_ON_Flag == 0)
                    {
                        // 串口1控制标志位置1，其他标志位置0
                        Usart1_ON_Flag = 1;
                        Usart5_ON_Flag = 0;
                        APP_ON_Flag = 0;
                        PS2_ON_Flag = 0;
                        Remote_ON_Flag = 0;
                        CAN_ON_Flag = 0;
                    }

                    command_lost_count = 0;
                    // 从串口数据求三轴目标速度，分高8位和低8位，单位mm/s
                    Move_X = (short)((rxbuf_car[3] << 8) + (rxbuf_car[4]));
                    Move_Y = (short)((rxbuf_car[5] << 8) + (rxbuf_car[6]));
                    Move_Z = (short)((rxbuf_car[7] << 8) + (rxbuf_car[8]));

                    // 单位转换, mm/s->m/s
                    Move_X = Move_X / 1000.0f;
                    Move_Y = Move_Y / 1000.0f;
                    Move_Z = Move_Z / 1000.0f;
                }
            }
        }

        if (Count_moveit == 11) // 验证数据包的长度
        {
            Count_moveit = 0; // 重新开始接收
            if (rxbuf_moveit[10] == FRAME_TAIL_MOVEIT) // 验证数据包的帧尾
            {
                for (i = 0; i < 9; i++)
                {
                    // 异或位校验，用于检测数据是否出错
                    check = rxbuf_moveit[i] ^ check;
                }
                if (check == rxbuf_moveit[9])
                    error = 0; // 校验成功

                if (error == 0)
                {
                    // 从串口数据求四轴目标角度，分高8位和低8位，单位mm/s
                    Moveit_Angle1 = (short)((rxbuf_moveit[1] << 8) + (rxbuf_moveit[2]));
                    Moveit_Angle2 = (short)((rxbuf_moveit[3] << 8) + (rxbuf_moveit[4]));
                    Moveit_Angle3 = (short)((rxbuf_moveit[5] << 8) + (rxbuf_moveit[6]));
                    Moveit_Angle4 = (short)((rxbuf_moveit[7] << 8) + (rxbuf_moveit[8]));

                    // 单位转换, mm/s->弧度（假设单位为mm/s，转换为弧度）
                    Moveit_Angle1 = -Moveit_Angle1 * 0.001f;
                    Moveit_Angle2 = Moveit_Angle2 * 0.001f;
                    Moveit_Angle3 = Moveit_Angle3 * 0.001f;
                    Moveit_Angle4 = -Moveit_Angle4 * 0.001f;
                }
            }
        }
    }
    return 0;
}

/**************************************************************************
函数功能：串口2接收中断
入口参数：无
返回  值：无
说明：处理通过USART2接收到的数据，进行控制模式切换和速度调整
**************************************************************************/
int USART2_IRQHandler(void)
{
    int Usart_Receive;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 检查是否接收到数据
    {
        static u8 Flag_PID, i, j, Receive[50], Last_Usart_Receive;
        static float Data;

        Usart_Receive = USART2->DR; // 读取数据
        if (Deviation_Count < CONTROL_DELAY)
            // 开机10秒前不处理数据
            return 0;

        if (Usart_Receive == 0x41 && Last_Usart_Receive == 0x41 && APP_ON_Flag == 0)
        {
            // 开机10秒之后，按下APP的前进键进入APP控制模式
            // APP控制标志位置1，其他标志位置0
            PS2_ON_Flag = 0;
            Remote_ON_Flag = 0;
            APP_ON_Flag = 1;
            CAN_ON_Flag = 0;
            Usart1_ON_Flag = 0;
            Usart5_ON_Flag = 0;
        }
        Last_Usart_Receive = Usart_Receive;

        if (Usart_Receive == 0x4B)
            // 进入APP转向控制界面
            Turn_Flag = 1;
        else if (Usart_Receive == 0x49 || Usart_Receive == 0x4A)
            // 进入APP方向控制界面
            Turn_Flag = 0;

        if (Turn_Flag == 0)
        {
            // APP摇杆控制界面命令
            if (Usart_Receive >= 0x41 && Usart_Receive <= 0x48)
            {
                Flag_Direction = Usart_Receive - 0x40;
            }
            else if (Usart_Receive <= 8)
            {
                Flag_Direction = Usart_Receive;
            }
            else
                Flag_Direction = 0;
        }
        else if (Turn_Flag == 1)
        {
            // APP转向控制界面命令
            if (Usart_Receive == 0x43)
                Flag_Left = 0, Flag_Right = 1; // 右自转
            else if (Usart_Receive == 0x47)
                Flag_Left = 1, Flag_Right = 0; // 左自转
            else
                Flag_Left = 0, Flag_Right = 0, Flag_Direction = 0;

            if (Usart_Receive == 0x41)
                car_A_steer_flag = 1; // 小车控制
            else if (Usart_Receive == 0x45)
                car_A_steer_flag = 2; // 舵机控制
            else
                Flag_Direction = 0;
        }

        if (Usart_Receive == 0x58)
            RC_Velocity = RC_Velocity + 100; // 加速按键，+100mm/s
        if (Usart_Receive == 0x59)
            RC_Velocity = RC_Velocity - 100; // 减速按键，-100mm/s

        if (RC_Velocity < 50)
            RC_Velocity = 50;
        else if (RC_Velocity > 1200)
            RC_Velocity = 1200;

        // 以下是与APP调试界面通讯
        if (Usart_Receive == 0x7B)
            Flag_PID = 1; // APP参数指令起始位
        if (Usart_Receive == 0x7D)
            Flag_PID = 2; // APP参数指令停止位

        if (Flag_PID == 1) // 采集数据
        {
            Receive[i] = Usart_Receive;
            i++;
        }
        if (Flag_PID == 2) // 分析数据
        {
            if (Receive[3] == 0x50)
                PID_Send = 1;
            else if (Receive[1] != 0x23)
            {
                for (j = i; j >= 4; j--)
                {
                    Data += (Receive[j - 1] - 48) * pow(10, i - j);
                }
                switch (Receive[1])
                {
                    case 0x30: Moveit_Angle1 = Data * 0.01f - 1.57f; break; // *0.01是因为参数在发送前放大了100倍，这里需要还原
                    case 0x31: Moveit_Angle2 = Data * 0.01f - 1.57f; break; // -1.57是因为参数在发送前加了1.57，这里需要还原
                    case 0x32: Moveit_Angle3 = Data * 0.01f - 1.57f; break;
                    case 0x33: Moveit_Angle4 = Data * 0.01f - 1.57f; break;
                    case 0x36: RC_Velocity = Data; break;
                    case 0x37: Velocity_KP = Data; break;
                    case 0x38: Velocity_KI = Data; break;
                }
            }
            // 相关标志位清零
            Flag_PID = 0;
            i = 0;
            j = 0;
            Data = 0;
            memset(Receive, 0, sizeof(u8) * 50); // 数组清零
        }

        if (RC_Velocity < 0)
            RC_Velocity = 0;
    }
    return 0;
}

/////**************************************************************************
//Function: Serial port 3 receives interrupted
//Input   : none
//Output  : none
//函数功能：串口3接收中断
//入口参数：无
//返回  值：无
//**************************************************************************/
//int USART3_IRQHandler(void)
//{	
//	static u8 Count_car,Count_moveit;
//	static u8 rxbuf_moveit[16];
//	u8 Usart_Receive;
//	int check=0,error=1,i;

//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //Check if data is received //判断是否接收到数据
//	{
//		Usart_Receive = USART_ReceiveData(USART3);//Read the data //读取数据
//		if(Time_count<CONTROL_DELAY)
//			// Data is not processed until 10 seconds after startup
//		  //开机10秒前不处理数据
//		  return 0;	
//		
//		//Fill the array with serial data
//		//串口数据填入数组
//    Receive_Data.buffer[Count_car]=Usart_Receive;
//		rxbuf_moveit[Count_moveit]=Usart_Receive;
//		
//		
//		// Ensure that the first data in the array is FRAME_HEADER
//		//确保数组第一个数据为FRAME_HEADER
//		if(Usart_Receive == FRAME_HEADER_CAR||Count_car>0) Count_car++; else Count_car=0; //接收控制底盘的数据帧
//		if(Usart_Receive == FRAME_HEADER_MOVEIT||Count_moveit>0) Count_moveit++; else Count_moveit=0; //接收控制机械臂的数据帧
//		if (Count_car == 11) //Verify the length of the packet //验证数据包的长度
//		{   
//				Count_car=0; //Prepare for the serial port data to be refill into the array //为串口数据重新填入数组做准备
//				if(Receive_Data.buffer[10] == FRAME_TAIL_CAR) //Verify the frame tail of the packet //验证数据包的帧尾
//				{
//					//Data exclusionary or bit check calculation, mode 0 is sent data check
//					//数据异或位校验计算，模式0是发送数据校验
//					if(Receive_Data.buffer[9] ==Check_Sum(9,0))	 
//				  {		
//						//All modes flag position 0, USART3 control mode
//            //所有模式标志位置0，为Usart3控制模式						
//						PS2_ON_Flag=0;
//						Remote_ON_Flag=0;
//						APP_ON_Flag=0;
//						CAN_ON_Flag=0;
//						Usart1_ON_Flag=0;
//						Usart5_ON_Flag=0;
//						command_lost_count=0; 
//						//Calculate the target speed of three axis from serial data, unit m/s
//						//从串口数据求三轴目标速度， 单位m/s
//						Move_X=XYZ_Target_Speed_transition(Receive_Data.buffer[3],Receive_Data.buffer[4]);
//						Move_Y=XYZ_Target_Speed_transition(Receive_Data.buffer[5],Receive_Data.buffer[6]);
//						Move_Z=XYZ_Target_Speed_transition(Receive_Data.buffer[7],Receive_Data.buffer[8]);
//				  }
//			}
//		}
//		  if (Count_moveit == 11)	//验证数据包的长度
//		{   
//				Count_moveit=0;//重新开始接收
//				if(rxbuf_moveit[10] == FRAME_TAIL_MOVEIT) //验证数据包的尾部校验信息
//				{
//					 
//					for(i=0; i<9; i++)
//					{
//						check=rxbuf_moveit[i]^check; //异或，用于检测数据是否出错
//					}
//					if(check==rxbuf_moveit[9]) error=0; //检验成功
//					
//					if(error==0)	 //数据校验位计算
//				  {		
//						Moveit_Angle1=(short)((rxbuf_moveit[1]<<8)+(rxbuf_moveit[2])); //求X轴速度 分高8位和低8位 单位mm/s
//						Moveit_Angle2=(short)((rxbuf_moveit[3]<<8)+(rxbuf_moveit[4])); //求X轴速度 分高8位和低8位 单位mm/s
//						Moveit_Angle3=(short)((rxbuf_moveit[5]<<8)+(rxbuf_moveit[6])); //求Z轴速度 分高8位和低8位 单位mm/s
//						Moveit_Angle4=(short)((rxbuf_moveit[7]<<8)+(rxbuf_moveit[8])); //求Z轴速度 分高8位和低8位 单位mm/s					
//						
//						Moveit_Angle1=-Moveit_Angle1*0.001f; //单位(弧度)
//						Moveit_Angle2= Moveit_Angle2*0.001f; 
//						Moveit_Angle3= Moveit_Angle3*0.001f; 
//						Moveit_Angle4=-Moveit_Angle4*0.001f;
//	
//					}
//			  }
//		 }
//	} 
//  return 0;	
//}
/**************************************************************************
函数功能：串口5接收中断
入口参数：无
返回  值：无
说明：处理通过串口5接收到的数据，进行校验并更新目标速度
**************************************************************************/
int UART5_IRQHandler(void)
{	
    static u8 Count_car, Count_moveit;
    static u8 rxbuf_moveit[16];
    u8 Usart_Receive;
    int check = 0, error = 1, i;

    if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) // 检查是否接收到数据
    {
        Usart_Receive = USART_ReceiveData(UART5); // 读取数据
        if (Time_count < CONTROL_DELAY)
            // 开机10秒前不处理数据
            return 0;

        // 串口数据填入数组
        Receive_Data.buffer[Count_car] = Usart_Receive;
        rxbuf_moveit[Count_moveit] = Usart_Receive;

        // 确保数组第一个数据为FRAME_HEADER
        if (Usart_Receive == FRAME_HEADER_CAR || Count_car > 0)
            Count_car++;
        else
            Count_car = 0; // 接收控制底盘的数据帧

        if (Usart_Receive == FRAME_HEADER_MOVEIT || Count_moveit > 0)
            Count_moveit++;
        else
            Count_moveit = 0; // 接收控制机械臂的数据帧

        if (Count_car == 11) // 验证数据包的长度
        {
            Count_car = 0; // 重新开始接收
            if (Receive_Data.buffer[10] == FRAME_TAIL_CAR) // 验证数据包的帧尾
            {
                // 数据异或位校验计算，模式0是接收数据校验
                if (Receive_Data.buffer[9] == Check_Sum(9, 0))
                {
                    // 所有模式标志位置0，为USART5控制模式
                    PS2_ON_Flag = 0;
                    Remote_ON_Flag = 0;
                    APP_ON_Flag = 0;
                    CAN_ON_Flag = 0;
                    Usart1_ON_Flag = 0;
                    Usart5_ON_Flag = 1;
                    command_lost_count = 0;

                    // 从串口数据求三轴目标速度，单位m/s
                    Move_X = XYZ_Target_Speed_transition(Receive_Data.buffer[3], Receive_Data.buffer[4]);
                    Move_Y = XYZ_Target_Speed_transition(Receive_Data.buffer[5], Receive_Data.buffer[6]);
                    Move_Z = XYZ_Target_Speed_transition(Receive_Data.buffer[7], Receive_Data.buffer[8]);
                }
            }
        }

        if (Count_moveit == 11) // 验证数据包的长度
        {   
            Count_moveit = 0; // 重新开始接收
            if (rxbuf_moveit[10] == FRAME_TAIL_MOVEIT) // 验证数据包的尾部校验信息
            {
                // 异或位校验，用于检测数据是否出错
                for (i = 0; i < 9; i++)
                {
                    check = rxbuf_moveit[i] ^ check;
                }
                if (check == rxbuf_moveit[9])
                    error = 0; // 检验成功

                if (error == 0) // 数据校验位计算
                {		
                    // 求X轴速度 分高8位和低8位 单位mm/s
                    Moveit_Angle1 = (short)((rxbuf_moveit[1] << 8) + (rxbuf_moveit[2]));
                    // 求X轴速度 分高8位和低8位 单位mm/s
                    Moveit_Angle2 = (short)((rxbuf_moveit[3] << 8) + (rxbuf_moveit[4]));
                    // 求Z轴速度 分高8位和低8位 单位mm/s
                    Moveit_Angle3 = (short)((rxbuf_moveit[5] << 8) + (rxbuf_moveit[6]));
                    // 求Z轴速度 分高8位和低8位 单位mm/s
                    Moveit_Angle4 = (short)((rxbuf_moveit[7] << 8) + (rxbuf_moveit[8]));					

                    // 单位转换, mm/s -> 弧度
                    Moveit_Angle1 = -Moveit_Angle1 * 0.001f;
                    Moveit_Angle2 = Moveit_Angle2 * 0.001f;
                    Moveit_Angle3 = Moveit_Angle3 * 0.001f;
                    Moveit_Angle4 = -Moveit_Angle4 * 0.001f;
                }
            }
        }
    } 
    return 0;
}

/**************************************************************************
函数功能：将上位机发过来的目标前进速度Vx、目标角速度Vz，转换为阿克曼小车的右前轮转角
入口参数：
  Vx - 目标前进速度，单位：m/s
  Vz - 目标角速度，单位：rad/s
返回  值：阿克曼小车的右前轮转角，单位：rad
说明：根据阿克曼转向原理计算右前轮转角，确保转弯半径不小于最小转弯半径
      以避免小车摩擦力增大，影响控制效果
**************************************************************************/
float Vz_to_Akm_Angle(float Vx, float Vz)
{
    float R, AngleR, Min_Turn_Radius;
    //float AngleL;  // 左轮转角
    
    // 阿克曼小车需要设置最小转弯半径
    // 如果目标速度要求的转弯半径小于最小转弯半径，
    // 会导致小车运动摩擦力大大提高，严重影响控制效果
    Min_Turn_Radius = MINI_AKM_MIN_TURN_RADIUS;
    
    if (Vz != 0 && Vx != 0)
    {
        // 如果目标速度要求的转弯半径小于最小转弯半径
        if (float_abs(Vx / Vz) <= Min_Turn_Radius)
        {
            // 降低目标角速度，配合前进速度，提高转弯半径到最小转弯半径
            if (Vz > 0)
                Vz = float_abs(Vx) / Min_Turn_Radius;
            else
                Vz = -float_abs(Vx) / Min_Turn_Radius;
        }
        
        R = Vx / Vz;  // 计算转弯半径
        // AngleL = atan(Axle_spacing / (R - 0.5 * Wheel_spacing)); // 计算左轮转角
        AngleR = atan(Axle_spacing / (R + 0.5f * Wheel_spacing)); // 计算右轮转角
    }
    else
    {
        AngleR = 0; // 如果速度或角速度为0，转角为0
    }
    
    return AngleR;
}

/**************************************************************************
函数功能：将上位机发过来的高8位和低8位数据整合成一个short型数据后，再做单位还原换算
入口参数：
  High - 高8位数据
  Low - 低8位数据
返回  值：机器人X/Y/Z轴的目标速度，单位：m/s
说明：将高8位和低8位数据组合成一个16位的short型数据，然后进行单位转换
      例如，将mm/s转换为m/s
**************************************************************************/
float XYZ_Target_Speed_transition(u8 High, u8 Low)
{
    // 数据转换的中间变量
    short transition;
    
    // 将高8位和低8位整合成一个16位的short型数据
    transition = ((High << 8) + Low);
    
    // 单位转换，从mm/s转换为m/s
    return transition / 1000.0f;
}

/**************************************************************************
函数功能：串口1发送数据
入口参数：
  data - 要发送的数据，8位
返回  值：无
说明：将一个字节的数据通过USART1发送出去，并等待发送完成
**************************************************************************/
void usart1_send(u8 data)
{
    USART1->DR = data;  // 写入数据寄存器
    while ((USART1->SR & USART_SR_TXE) == 0);  // 等待发送数据寄存器为空（TXE标志）
}

/**************************************************************************
函数功能：串口2发送数据
入口参数：
  data - 要发送的数据，8位
返回  值：无
说明：将一个字节的数据通过USART2发送出去，并等待发送完成
**************************************************************************/
void usart2_send(u8 data)
{
    USART2->DR = data;  // 写入数据寄存器
    while ((USART2->SR & USART_SR_TXE) == 0);  // 等待发送数据寄存器为空（TXE标志）
}

/**************************************************************************
函数功能：串口3发送数据
入口参数：
  data - 要发送的数据，8位
返回  值：无
说明：将一个字节的数据通过USART3发送出去，并等待发送完成
**************************************************************************/
void usart3_send(u8 data)
{
    USART3->DR = data;  // 写入数据寄存器
    while ((USART3->SR & USART_SR_TXE) == 0);  // 等待发送数据寄存器为空（TXE标志）
}

/**************************************************************************
函数功能：串口5发送数据
入口参数：
  data - 要发送的数据，8位
返回  值：无
说明：将一个字节的数据通过UART5发送出去，并等待发送完成
      注意：UART5可能是在不同的头文件或配置中定义的
**************************************************************************/
void usart5_send(u8 data)
{
    UART5->DR = data;  // 写入数据寄存器
    while ((UART5->SR & USART_SR_TXE) == 0);  // 等待发送数据寄存器为空（TXE标志）
}

/**************************************************************************
函数功能：计算要发送/接收的数据校验结果
入口参数：
  Count_Number - 校验的前几位数
  Mode - 0-对接收数据进行校验，1-对发送数据进行校验
返回  值：校验结果
说明：根据指定的数据长度和模式，计算校验和
      使用异或运算进行校验，适用于简单的错误检测
**************************************************************************/
u8 Check_Sum(unsigned char Count_Number, unsigned char Mode)
{
    unsigned char check_sum = 0, k;
    
    // 对要发送的数据进行校验
    if (Mode == 1)
    {
        for (k = 0; k < Count_Number; k++)
        {
            check_sum = check_sum ^ Send_Data.buffer[k];  // 异或运算计算校验和
        }
    }
    
    // 对接收到的数据进行校验
    if (Mode == 0)
    {
        for (k = 0; k < Count_Number; k++)
        {
            check_sum = check_sum ^ Receive_Data.buffer[k];  // 异或运算计算校验和
        }
    }
    
    return check_sum;
}







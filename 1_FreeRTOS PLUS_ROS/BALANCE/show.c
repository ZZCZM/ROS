/**
  ******************************************************************************
  * @file    show.c
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
#include "show.h"
/* 电压显示全局变量（单位：0.1V）*/
int Voltage_Show; 

/* 通用计数器变量 */
unsigned char i;          // 用于循环计数等场景
unsigned char Send_Count;  // 数据发送频率控制计数器

/* 外部变量声明（实际定义在其他源文件）*/
extern SEND_DATA Send_Data;          // 数据发送结构体（包含蓝牙/串口数据）
extern int MPU9250ErrorCount;         // MPU9250传感器错误累计次数
extern int EncoderA_Count, EncoderB_Count, EncoderC_Count, EncoderD_Count;  // 四路编码器原始脉冲计数 
extern int MPU9250SensorCountA, MPU9250SensorCountB, MPU9250SensorCountC, MPU9250SensorCountD; // 四轴传感器数据缓存
extern int Time_count;                // 系统运行时间计数器（单位：10ms）

/**************************************************************************
【函数名称】Delay
【功能概述】简单阻塞式延时函数
            - 通过空循环消耗CPU时间实现延时
            - 适用于短时间非精确延时场景
【输入参数】nCount：延时循环次数（与实际时间相关，需校准）
【注意事项】1. 延时精度受主频影响大
          2. 在实时操作系统中慎用会阻塞任务调度
**************************************************************************/
void Delay(__IO uint32_t nCount)
{
    /* volatile防止编译器优化空循环 */
    for ( ; nCount != 0; nCount-- ) {
        // 空指令集保证循环不被优化
        __ASM volatile ("nop"); 
    }
}

///**************************************************************************
//Function: Read the battery voltage, buzzer alarm, start the self-test, send data to APP, OLED display task
//Input   : none
//Output  : none
//函数功能：读取电池电压、蜂鸣器报警、开启自检、向APP发送数据、OLED显示屏显示任务
//入口参数：无
//返回  值：无
//**************************************************************************/
//int Buzzer_count=25;
//void show_task(void *pvParameters)
//{
//	//其中0、3、5、6 模式适合从左至右显示文字，
//  //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
//  //其中 6 模式为大部分液晶例程的默认显示方向  
//	ILI9806G_GramScan (6);
//	u32 lastWakeTime = getSysTickCnt();
//	while(1)
//	{
//		int i=0;
//		static int LowVoltage_1=0, LowVoltage_2=0;
//		static int Servo_adjust_timecount;
//		//This task runs at 5Hz
//		//此任务以5Hz的频率运行
//		vTaskDelayUntil(&lastWakeTime, F2T(RATE_5_HZ));	
//		
//		//The buzzer will beep briefly when the machine is switched on		
//		//开机时蜂鸣器短暂蜂鸣，开机提醒
//		if(Time_count<50)Buzzer=1; 
//		else if(Time_count>=51 && Time_count<100)
//		{
//			Buzzer=0;
//			
//		}
//		if(LowVoltage_1==1 || LowVoltage_2==1)Buzzer_count=0;
//		if(Buzzer_count<5)Buzzer_count++;
//		 
//		if(Buzzer_count<5)Buzzer=1; //The buzzer is buzzing //蜂鸣器蜂鸣
//		else if(Buzzer_count==5)Buzzer=0;
//		
//		if(Beep_Control_Flag)
//		{
//			Buzzer = 1;
//			Servo_adjust_timecount++;
//			if(Servo_adjust_timecount == 10)
//			{
//				Beep_Control_Flag = 0;
//				Buzzer = 0;
//				Servo_adjust_timecount = 0;
//				ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
//			}
//		}
//		 
//		//Read the battery voltage //读取电池电压
//		for(i=0;i<10;i++)
//		{
//			Voltage_All+=Get_battery_volt(); 
//		}
//		Voltage=Voltage_All/10;
//		Voltage_All=0;
//		
//		if(LowVoltage_1==1)LowVoltage_1++; //Make sure the buzzer only rings for 0.5 seconds //确保蜂鸣器只响0.5秒
//		if(LowVoltage_2==1)LowVoltage_2++; //Make sure the buzzer only rings for 0.5 seconds //确保蜂鸣器只响0.5秒
//		if(Voltage>=12.6f)Voltage=12.6f;
//		else if(10<=Voltage && Voltage<10.5f && LowVoltage_1<2)LowVoltage_1++; //10.5V, first buzzer when low battery //10.5V，低电量时蜂鸣器第一次报警
//		else if(Voltage<10 && LowVoltage_1<2)LowVoltage_2++; //10V, when the car is not allowed to control, the buzzer will alarm the second time //10V，小车禁止控制时蜂鸣器第二次报警
//					
//    //oled_show(); //Tasks are displayed on the screen //显示屏显示任务
////		LCD_Test();
//		lcd_show();
//	}
//}  

/**************************************************************************
Function: Read the battery voltage, buzzer alarm, start the self-test, send data to APP, OLED display task
Input   : none
Output  : none
函数功能：读取电池电压、蜂鸣器报警、开启自检、向APP发送数据、OLED显示屏显示任务
入口参数：无
返回  值：无
**************************************************************************/

// 定义任务的不同阶段
#define STAGE_INIT          0  // 初始化阶段
#define STAGE_VOLTAGE_READ  1  // 电压读取阶段
#define STAGE_ALARM         2  // 报警检测阶段
#define STAGE_DISPLAY       3  // 显示阶段

// 信号量/互斥量声明
SemaphoreHandle_t Display_Resource_Mutex;    // 显示资源互斥量
SemaphoreHandle_t Voltage_Resource_Mutex;    // 电压资源互斥量
SemaphoreHandle_t Buzzer_Resource_Mutex;     // 蜂鸣器资源互斥量

int Buzzer_count = 25;//蜂鸣器计数

/**************************************************************************
【函数名称】Show_task
【功能概述】综合状态显示与告警管理任务
            - 实现多阶段优先级任务调度（电压采集/报警处理/显示刷新）
            - 管理LCD显示、电池电压监控、蜂鸣器告警等关键功能
            - 采用信号量机制保证资源互斥访问
            - 支持分级低电压报警和系统状态可视化
【任务特性】
            - 严格5Hz周期执行（200ms间隔）
            - 四级优先级阶段式调度（报警>电压>显示>初始化）
            - 支持开机自检提示音和紧急报警
**************************************************************************/
void Show_task(void *pvParameters)
{
    // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
    // 第一阶段：资源初始化
    // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
    // 创建互斥信号量（防止资源冲突）
    SemaphoreHandle_t Display_Semaphore = xSemaphoreCreateMutex();   // 显示资源互斥量（保护LCD操作）
    SemaphoreHandle_t Voltage_Semaphore = xSemaphoreCreateMutex();   // 电压资源互斥量（保护电压采集）
    SemaphoreHandle_t Buzzer_Semaphore = xSemaphoreCreateMutex();    // 蜂鸣器资源互斥量（保护蜂鸣器控制）
    
    // 转换信号量为阶段资源控制块（自定义结构）
    ResourceCB_t *pxDisplayResource = prvConvertToResourceCB(Display_Semaphore, STAGE_DISPLAY);    // 显示阶段资源块
    ResourceCB_t *pxVoltageResource = prvConvertToResourceCB(Voltage_Semaphore, STAGE_VOLTAGE_READ); // 电压阶段资源块
    ResourceCB_t *pxBuzzerResource = prvConvertToResourceCB(Buzzer_Semaphore, STAGE_ALARM);       // 报警阶段资源块
    
    // 配置四阶段优先级（数值越大优先级越高）
    UBaseType_t uxStages = 4;  // 阶段总数
    UBaseType_t uxPriorities[4] = {  // 优先级数组（索引对应阶段）
        2,  // STAGE_INIT: 初始化阶段（低优先级，仅执行硬件初始化）
        4,  // STAGE_VOLTAGE_READ: 电压读取（较高优先级，保证电源监控实时性）
        5,  // STAGE_ALARM: 报警处理（最高优先级，即时响应系统异常）
        3   // STAGE_DISPLAY: 显示刷新（中等优先级，允许适度延迟）
    };
    
    // 初始化阶段优先级配置
    vTaskInitStagedPriorities(NULL, uxStages, uxPriorities);  // 参数1为任务句柄，NULL表示当前任务
    
    // 进入初始化阶段（设置当前阶段标识）
    xTaskSetCurrentStage(NULL, STAGE_INIT);
    
    // 初始化LCD显示模式（GRAM扫描方向）
    ILI9806G_GramScan(6);  // 参数6对应横屏模式，RGB565颜色格式
    
    // 局部变量初始化
    u32 lastWakeTime = getSysTickCnt();   // 获取系统节拍基准值（用于精确周期控制）
    int i = 0;                            // 通用循环计数器
    static int LowVoltage_1 = 0;           // 一级低压报警标志（10.0V-10.5V）
    static int LowVoltage_2 = 0;           // 二级低压报警标志（<10.0V，触发系统停控）
    static int Servo_adjust_timecount = 0; // 舵机调节模式计时器（单位：任务周期）
    
    // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
    // 任务主循环
    // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
    while(1)
    {
        // 严格周期控制（5Hz，200ms间隔）
        vTaskDelayUntil(&lastWakeTime, F2T(RATE_5_HZ));  // F2T将频率转换为节拍数
        
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        // 第二阶段：电压采集（STAGE_VOLTAGE_READ）
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        xTaskSetCurrentStage(NULL, STAGE_VOLTAGE_READ);  // 切换阶段标识
        
        // 尝试获取电压资源（10ms超时保护）
        if (xSemaphoreTakeStagedMutex(Voltage_Semaphore, pdMS_TO_TICKS(10), STAGE_VOLTAGE_READ) == pdTRUE)
        {
            /* 电池电压采集（10次滑动平均滤波） */
            Voltage_All = 0;  // 清零累加器
            for(i = 0; i < 10; i++)  // 采样10次
            {
                Voltage_All += Get_battery_volt();  // 读取电池电压
            }
            Voltage = Voltage_All / 10;  // 计算平均值
            
            /* 电压上限保护（铅酸电池满电电压12.6V）*/
            if(Voltage >= 12.6f) Voltage = 12.6f;  // 防止显示溢出
            
            xSemaphoreGiveStagedMutex(Voltage_Semaphore);  // 释放信号量
        }
        
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        // 第三阶段：报警处理（STAGE_ALARM）
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        xTaskSetCurrentStage(NULL, STAGE_ALARM);  // 切换阶段标识
        
        // 尝试获取蜂鸣器资源（10ms超时保护）
        if (xSemaphoreTakeStagedMutex(Buzzer_Semaphore, pdMS_TO_TICKS(10), STAGE_ALARM) == pdTRUE)
        {
            /* 开机提示音序列（前0.5秒鸣响，后0.5秒静音） */
            if(Time_count < 50)         // Time_count单位：10ms，50=500ms
                Buzzer = 1;             // 蜂鸣器使能（0.5秒提示音）
            else if(Time_count < 100)   // 50-100对应0.5-1秒
                Buzzer = 0;             // 蜂鸣器禁用（0.5秒静音）
                
            /* 分级低电压检测（铅酸电池保护） */
            if(10 <= Voltage && Voltage < 10.5f && LowVoltage_1 < 2)  // 一级低压（10.0V-10.5V）
                LowVoltage_1++;  // 触发计数（防止误报）
            else if(Voltage < 10 && LowVoltage_2 < 2)  // 二级低压（<10.0V，严重欠压）
                LowVoltage_2++;  // 触发计数
            else
						{
							LowVoltage_1 = 0; // 标志清除,允许再次触发
							LowVoltage_2 = 0; // 标志清除,允许再次触发
						}
						
            /* 报警触发控制（首次触发重置计数器） */
            if(LowVoltage_1 == 1 || LowVoltage_2 == 1) 
                Buzzer_count = 0;  // 重置蜂鸣脉冲计数器
            
            /* 蜂鸣器脉冲控制（产生1秒鸣响） */
            if(Buzzer_count < 5)       // 5次×200ms=1秒
            {
                Buzzer_count++;       // 计数递增
                Buzzer = 1;           // 持续鸣响
            }
            else if(Buzzer_count == 5)
                Buzzer = 0;           // 停止鸣响
            
            /* 舵机调试模式提示音（强制鸣响2秒） */
            if(Beep_Control_Flag)      // 外部调试标志
            {
                Buzzer = 1;           // 强制使能蜂鸣器
                Servo_adjust_timecount++;  // 递增计时器
                if(Servo_adjust_timecount == 10)  // 10周期×200ms=2秒
                {
                    Beep_Control_Flag = 0;    // 清除调试标志
                    Buzzer = 0;               // 关闭蜂鸣器
                    Servo_adjust_timecount = 0;  // 重置计时器
                }
            }
            
            /* 报警标志自锁（防止重复触发） */
            if(LowVoltage_1 == 1) LowVoltage_1++;  // 从1→2，避免重复计数
            if(LowVoltage_2 == 1) LowVoltage_2++;
                
            xSemaphoreGiveStagedMutex(Buzzer_Semaphore);  // 释放信号量
        }
        
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        // 第四阶段：显示刷新（STAGE_DISPLAY）
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        xTaskSetCurrentStage(NULL, STAGE_DISPLAY);  // 切换阶段标识
        
        // 尝试获取显示资源（20ms超时保护）
        if (xSemaphoreTakeStagedMutex(Display_Semaphore, pdMS_TO_TICKS(20), STAGE_DISPLAY) == pdTRUE)
        {
            /* 舵机调试模式清屏处理 */
            if(Beep_Control_Flag && Servo_adjust_timecount == 10) 
            {
                ILI9806G_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);  // 全屏清黑（参数：x,y,width,height）
            }
            
            /* 执行LCD显示更新（电压/状态/错误码等） */
            lcd_show();  // 调用显示函数
            
            xSemaphoreGiveStagedMutex(Display_Semaphore);  // 释放信号量
        }
        
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        // 返回初始化阶段（准备下次循环）
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        xTaskSetCurrentStage(NULL, STAGE_INIT);  // 重置阶段标识
    }
}
/**************************************************************************
【函数名称】lcd_show
【功能概述】LCD显示屏显示任务
            - 根据车型和工作模式实时更新显示内容
            - 支持正常控制模式和舵机微调模式的显示
【入口参数】none
【返回值】none
【显示内容】
            - 正常模式：车型标识、电压、电机速度、陀螺仪数据
            - 微调模式：四个舵机的角度偏移值
【注意事项】
            - 电压显示格式化处理（整数部分和小数部分分离）
            - 电机速度和陀螺仪数据符号处理
            - 舵机角度偏移值符号处理
**************************************************************************/
void lcd_show(void)
{  
    static int count=0;     // 显示刷新计数器（未使用）
    char pStr[50];          // 字符串格式化缓冲区
    int Car_Mode_Show;      // 当前显示车型标识
    
    // 计算电位器分档阈值（Divisor_Mode用于Car_Mode_Show计算）
    Divisor_Mode=2048/CAR_NUMBER+2;
    Car_Mode_Show=(int)((Get_adc_Average(Potentiometer,10))/Divisor_Mode); 
    if(Car_Mode_Show>2) Car_Mode_Show=2;  // 车型编号限幅
    
    // 电压转换为整数处理（12.6V→1260）
    Voltage_Show=Voltage*100; 
    count++;  // 刷新计数（未使用）

    // 采集电位器档位信息，实时显示小车开机时要适配的小车型号
    // Collect the tap information of the potentiometer, 
    // and display the car model to be fitted when the car starts up in real time
    if(Servo_init_angle_adjust == 0)  // 正常控制，不进入微调模式
    {
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        // 第1行显示：车型标识+传感器数据
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示屏第1行显示内容
        switch(Car_Mode_Show)
        {
            case Mec_Car:       ILI9806G_DispString_EN_CH(0,0,"Mec "); break;  // 麦克纳姆轮
            case FourWheel_Car: ILI9806G_DispString_EN_CH(0,0,"4WD "); break;  // 四驱车
            case Tank_Car:      ILI9806G_DispString_EN_CH(0,0,"Tank"); break;   // 履带车
        }
        
        // 麦轮/全向轮车显示Z轴角速度，履带车显示陀螺仪零点
        if(Car_Mode==Mec_Car)
        {
            // 显示"GZ"标识
            ILI9806G_DispString_EN_CH(100,0,"GZ");
            // 处理角速度符号及数值
            if( gyro[2]<0) {
                ILI9806G_DispString_EN_CH(200,0,"-");  // 显示负号
                sprintf(pStr,"%d ",-gyro[2]);  // 负值转正
                ILI9806G_DispString_EN_CH(215,0,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(200,0,"+");  // 显示正号
                sprintf(pStr,"%d ",gyro[2]);
                ILI9806G_DispString_EN_CH(215,0,pStr);
            }
        }
        else if(Car_Mode==FourWheel_Car || Car_Mode==Tank_Car)
        {
            // 显示零点偏移
            ILI9806G_DispString_EN_CH(100,0,"BIAS");
            // 处理零点偏移符号
            if( Deviation_gyro[2]<0) {
                ILI9806G_DispString_EN_CH(200,0,"-");  // 显示负号
                sprintf(pStr,"%d ",-gyro[2]);
                ILI9806G_DispString_EN_CH(215,0,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(200,0,"+");  // 显示正号
                sprintf(pStr,"%d ",gyro[2]);
                ILI9806G_DispString_EN_CH(215,0,pStr);
            }
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        // 第2行显示：电机A目标速度与实际速度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示屏第2行显示内容
        if(Car_Mode==Mec_Car || Car_Mode==FourWheel_Car)
        {
            // 显示电机A标识
            ILI9806G_DispString_EN_CH(0,100,"A");
            // 目标速度显示
            if( MOTOR_A.Target<0) {
                ILI9806G_DispString_EN_CH(100,100,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_A.Target*1000));  // 放大1000倍显示小数
                ILI9806G_DispString_EN_CH(115,100,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(100,100,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_A.Target*1000));
                ILI9806G_DispString_EN_CH(115,100,pStr);
            }
            
            // 实际速度显示
            if( MOTOR_A.Encoder<0) {
                ILI9806G_DispString_EN_CH(200,100,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_A.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,100,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(200,100,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_A.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,100,pStr);
            }
        }
        else if(Car_Mode==Tank_Car)
        {
            // 显示陀螺仪Z轴角速度
            ILI9806G_DispString_EN_CH(00,100,"GYRO_Z:");
            if( gyro[2]<0) {
                ILI9806G_DispString_EN_CH(300,100,"-");  // 显示负号
                sprintf(pStr,"%d ",-gyro[2]);
                ILI9806G_DispString_EN_CH(315,100,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(300,100,"+");  // 显示正号
                sprintf(pStr,"%d ",gyro[2]);
                ILI9806G_DispString_EN_CH(315,100,pStr);  // 显示数值
            }
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        // 第3-4行显示：电机B/C或左右电机参数
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        if(Car_Mode==Mec_Car || Car_Mode==FourWheel_Car)
        {
            // 显示电机B标识（第3行）
            ILI9806G_DispString_EN_CH(0,200,"B"); 
            if( MOTOR_B.Target<0) {
                ILI9806G_DispString_EN_CH(100,200,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_B.Target*1000));  // 放大1000倍显示小数
                ILI9806G_DispString_EN_CH(115,200,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(100,200,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_B.Target*1000));
                ILI9806G_DispString_EN_CH(115,200,pStr);
            }
            
            if( MOTOR_B.Encoder<0) {
                ILI9806G_DispString_EN_CH(200,200,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_B.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,200,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(200,200,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_B.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,200,pStr);
            }

            // 显示电机C标识（第4行）
            ILI9806G_DispString_EN_CH(0,300,"C"); 
            if( MOTOR_C.Target<0) {
                ILI9806G_DispString_EN_CH(100,300,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_C.Target*1000));
                ILI9806G_DispString_EN_CH(115,300,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(100,300,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_C.Target*1000));
                ILI9806G_DispString_EN_CH(115,300,pStr);
            }
                
            if( MOTOR_C.Encoder<0) {
                ILI9806G_DispString_EN_CH(200,300,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_C.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,300,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(200,300,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_C.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,300,pStr);
            }
        }
        else if(Car_Mode==Tank_Car)
        {
            // 显示左电机A标识（第3行）
            ILI9806G_DispString_EN_CH(0,200,"L:"); 
            if( MOTOR_A.Target<0) {
                ILI9806G_DispString_EN_CH(100,200,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_A.Target*1000));
                ILI9806G_DispString_EN_CH(115,200,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(100,200,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_A.Target*1000));
                ILI9806G_DispString_EN_CH(115,200,pStr);
            }
            if( MOTOR_A.Encoder<0) {
                ILI9806G_DispString_EN_CH(200,200,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_A.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,200,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(200,200,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_A.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,200,pStr);
            }
            
            // 显示右电机B标识（第4行）
            ILI9806G_DispString_EN_CH(0,300,"R:"); 
            if( MOTOR_B.Target<0) {
                ILI9806G_DispString_EN_CH(100,300,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_B.Target*1000));
                ILI9806G_DispString_EN_CH(115,300,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(100,300,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_B.Target*1000));
                ILI9806G_DispString_EN_CH(115,300,pStr);
            }
            
            if( MOTOR_B.Encoder<0) {
                ILI9806G_DispString_EN_CH(200,300,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_B.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,300,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(200,300,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_B.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,300,pStr);
            }
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        // 第5行显示：电机D目标速度或PWM值
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        if(Car_Mode==Mec_Car || Car_Mode==FourWheel_Car)
        {
            // 显示电机D标识
            ILI9806G_DispString_EN_CH(0,400,"D");
            if( MOTOR_D.Target<0) {
                ILI9806G_DispString_EN_CH(100,400,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_D.Target*1000));
                ILI9806G_DispString_EN_CH(115,400,pStr);
            } else {
                ILI9806G_DispString_EN_CH(100,400,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_D.Target*1000));
                ILI9806G_DispString_EN_CH(115,400,pStr);
            }
            
            if( MOTOR_D.Encoder<0) {
                ILI9806G_DispString_EN_CH(200,400,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_D.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,400,pStr);
            } else {
                ILI9806G_DispString_EN_CH(200,400,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_D.Encoder*1000));
                ILI9806G_DispString_EN_CH(215,400,pStr);
            }
        } 
        else if(Car_Mode==Tank_Car)
        {
            // 显示左电机A PWM值（第3行）
            ILI9806G_DispString_EN_CH(0,400,"MA"); 
            if( MOTOR_A.Motor_Pwm<0) {
                ILI9806G_DispString_EN_CH(100,400,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_A.Motor_Pwm*1000));  // 放大1000倍显示小数
                ILI9806G_DispString_EN_CH(115,400,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(100,400,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_A.Motor_Pwm*1000));
                ILI9806G_DispString_EN_CH(115,400,pStr);
            }
            
            // 显示右电机B PWM值（第4行）
            ILI9806G_DispString_EN_CH(200,400,"MB"); 
            if(MOTOR_B.Motor_Pwm<0) {
                ILI9806G_DispString_EN_CH(300,400,"-");  // 显示负号
                sprintf(pStr,"%d ",(int)(-MOTOR_B.Motor_Pwm*1000));
                ILI9806G_DispString_EN_CH(315,400,pStr);  // 显示数值
            } else {
                ILI9806G_DispString_EN_CH(300,400,"+");  // 显示正号
                sprintf(pStr,"%d ",(int)(MOTOR_B.Motor_Pwm*1000));
                ILI9806G_DispString_EN_CH(315,400,pStr);
            }
        }
        
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第6行显示：控制模式
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示当前控制模式
        if(PS2_ON_Flag==1)         ILI9806G_DispString_EN_CH(0,500,"PS2  ");
        else if (APP_ON_Flag==1)   ILI9806G_DispString_EN_CH(0,500,"APP  ");
        else if (Remote_ON_Flag==1)ILI9806G_DispString_EN_CH(0,500,"R-C  ");
        else if (CAN_ON_Flag==1)   ILI9806G_DispString_EN_CH(0,500,"CAN  ");
        else if (Usart1_ON_Flag==1)ILI9806G_DispString_EN_CH(0,500,"UART1");
        else if (Usart5_ON_Flag==1)ILI9806G_DispString_EN_CH(0,500,"UART5");
        else                       ILI9806G_DispString_EN_CH(0,500,"ROS  ");
        
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第6行显示：控制是否允许
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示当前小车是否允许控制
        if(Flag_Stop==0)           ILI9806G_DispString_EN_CH(100,500,"O N");  
        else                       ILI9806G_DispString_EN_CH(100,500,"OFF"); 
        
        // 显示电压整数部分（第6行）
        sprintf(pStr,"%d ",(int)Voltage_Show/100);
        ILI9806G_DispString_EN_CH(200,500,pStr);
        
        // 显示电压小数点（第6行）
        ILI9806G_DispString_EN_CH(230,500,".");
        
        // 显示电压小数部分（第6行）
        sprintf(pStr,"%d ",(int)Voltage_Show%100);
        ILI9806G_DispString_EN_CH(245,500,pStr);
        ILI9806G_DispString_EN_CH(275,500,"V");
        
        // 处理电压小数部分为0的情况（第6行）
        if(Voltage_Show%100==0) {
            sprintf(pStr,"%d ",(int)00);
            ILI9806G_DispString_EN_CH(245,500,pStr);
        }	
    }
    else  // 微调模式显示四个舵机的偏移值
    {
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第1行显示：舵机Angle1初始角度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示舵机Angle1初始角度
        ILI9806G_DispString_EN_CH(0,0,"Angle1_init:");
        if( Moveit_Angle1_init<0)	
        {
            ILI9806G_DispString_EN_CH(200,0,"-");  // 显示负号
            sprintf(pStr,"%d ",(int)-Moveit_Angle1_init);  // 负值转正
            ILI9806G_DispString_EN_CH(215,0,pStr);  // 显示数值
        }
        else
        {
            ILI9806G_DispString_EN_CH(200,0,"+");  // 显示正号
            sprintf(pStr,"%d ",(int)Moveit_Angle1_init);
            ILI9806G_DispString_EN_CH(215,0,pStr);
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第2行显示：舵机Angle2初始角度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示舵机Angle2初始角度
        ILI9806G_DispString_EN_CH(0,100,"Angle2_init:");
        if( Moveit_Angle2_init<0)	
        {
            ILI9806G_DispString_EN_CH(200,100,"-");  // 显示负号
            sprintf(pStr,"%d ",(int)-Moveit_Angle2_init);  // 负值转正
            ILI9806G_DispString_EN_CH(215,100,pStr);  // 显示数值
        }
        else
        {
            ILI9806G_DispString_EN_CH(200,100,"+");  // 显示正号
            sprintf(pStr,"%d ",(int)Moveit_Angle2_init);
            ILI9806G_DispString_EN_CH(215,100,pStr);
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第3行显示：舵机Angle3初始角度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示舵机Angle3初始角度
        ILI9806G_DispString_EN_CH(0,200,"Angle3_init:");
        if( Moveit_Angle3_init<0)	
        {
            ILI9806G_DispString_EN_CH(200,200,"-");  // 显示负号
            sprintf(pStr,"%d ",(int)-Moveit_Angle3_init);  // 负值转正
            ILI9806G_DispString_EN_CH(215,200,pStr);  // 显示数值
        }
        else
        {
            ILI9806G_DispString_EN_CH(200,200,"+");  // 显示正号
            sprintf(pStr,"%d ",(int)Moveit_Angle3_init);
            ILI9806G_DispString_EN_CH(215,200,pStr);
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第4行显示：舵机Angle4初始角度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示舵机Angle4初始角度
        ILI9806G_DispString_EN_CH(0,300,"Angle4_init:");
        if( Moveit_Angle4_init<0)	
        {
            ILI9806G_DispString_EN_CH(200,300,"-");  // 显示负号
            sprintf(pStr,"%d ",(int)-Moveit_Angle4_init);  // 负值转正
            ILI9806G_DispString_EN_CH(215,300,pStr);  // 显示数值
        }
        else
        {
            ILI9806G_DispString_EN_CH(200,300,"+");  // 显示正号
            sprintf(pStr,"%d ",(int)Moveit_Angle4_init);
            ILI9806G_DispString_EN_CH(215,300,pStr);
        }
    }
    // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
    //  刷新显示内容
    // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
    //=============刷新=======================//
    // ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
} 
/**************************************************************************
【函数名称】oled_show
【功能概述】OLED显示屏显示任务
            - 根据车型和工作模式实时更新显示内容
            - 支持正常模式与舵机微调模式界面切换
            - 显示各类传感器数据、电机速度和控制模式
【入口参数】none
【返回值】none
【显示模式】
            - 正常模式：显示电机速度、陀螺仪数据、电压等
            - 微调模式：显示四个舵机的初始角度偏移值
【关键函数】
            - OLED_ShowString：在指定位置显示字符串
            - OLED_ShowNumber：在指定位置显示数字
            - OLED_Refresh_Gram：刷新OLED显示内容
【注意】
            - 电压显示格式化处理（整数部分和小数部分分离）
            - 电机速度和陀螺仪数据符号处理
            - 舵机角度偏移值符号处理
**************************************************************************/
void oled_show(void)
{  
    static int count=0;     // 显示刷新计数器（未使用）
    int Car_Mode_Show;      // 当前显示车型标识

    // 计算电位器分档阈值（Divisor_Mode用于Car_Mode_Show计算）
    Divisor_Mode=2048/CAR_NUMBER+2;
    Car_Mode_Show=(int)((Get_adc_Average(Potentiometer,10))/Divisor_Mode); 
    if(Car_Mode_Show>2) Car_Mode_Show=2;  // 车型编号限幅

    Voltage_Show=Voltage*100;  // 电压转换为整数处理（12.6V→1260）
    count++;  // 刷新计数（未使用）

    // 采集电位器档位信息，实时显示小车开机时要适配的小车型号
    // Collect the tap information of the potentiometer, 
    // and display the car model to be fitted when the car starts up in real time
    if(Servo_init_angle_adjust == 0)  // 正常控制，不进入微调模式
    {
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第1行显示：车型标识+传感器数据
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        switch(Car_Mode_Show)  // 根据电位器显示车型
        {
            case Mec_Car:       OLED_ShowString(0,0,"Mec "); break;   // 麦克纳姆轮
            case FourWheel_Car: OLED_ShowString(0,0,"4WD "); break;   // 四驱车
            case Tank_Car:      OLED_ShowString(0,0,"Tank"); break;   // 履带车
        }
        
        // 麦克纳姆轮/四驱车显示Z轴角速度，履带车显示陀螺仪零点
        if(Car_Mode==Mec_Car) {
            OLED_ShowString(55,0,"GZ");  // 显示"GZ"标识
            // 处理角速度符号及数值
            if( gyro[2]<0) {
                OLED_ShowString(80,0,"-");
                OLED_ShowNumber(90,0,-gyro[2],5,12);  // 显示数值
            } else {
                OLED_ShowString(80,0,"+");
                OLED_ShowNumber(90,0,gyro[2],5,12);
            }
        }
        else if(Car_Mode==FourWheel_Car || Car_Mode==Tank_Car) {
            OLED_ShowString(55,0,"BIAS");  // 显示零点偏移
            // 处理零点偏移符号
            if( Deviation_gyro[2]<0) {
                OLED_ShowString(90,0,"-");
                OLED_ShowNumber(100,0,-Deviation_gyro[2],3,12);  // 显示数值
            } else {
                OLED_ShowString(90,0,"+");
                OLED_ShowNumber(100,0,Deviation_gyro[2],3,12);
            }
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第2行显示：电机A目标速度与实际速度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        if(Car_Mode==Mec_Car || Car_Mode==FourWheel_Car) {
            OLED_ShowString(0,10,"A");  // 显示电机A标识
            // 目标速度显示
            if( MOTOR_A.Target<0) {
                OLED_ShowString(15,10,"-");
                OLED_ShowNumber(20,10,-MOTOR_A.Target*1000,5,12);  // 放大1000倍显示小数
            } else {
                OLED_ShowString(15,10,"+");
                OLED_ShowNumber(20,10,MOTOR_A.Target*1000,5,12);
            }
            
            // 实际速度显示
            if( MOTOR_A.Encoder<0) {
                OLED_ShowString(60,10,"-");
                OLED_ShowNumber(75,10,-MOTOR_A.Encoder*1000,5,12);
            } else {
                OLED_ShowString(60,10,"+");
                OLED_ShowNumber(75,10,MOTOR_A.Encoder*1000,5,12);
            }

        }
        else if(Car_Mode==Tank_Car) {
            OLED_ShowString(00,10,"GYRO_Z:");  // 显示陀螺仪Z轴
            if( gyro[2]<0) {
                OLED_ShowString(60,10,"-");
                OLED_ShowNumber(75,10,-gyro[2],5,12);
            } else {
                OLED_ShowString(60,10,"+");
                OLED_ShowNumber(75,10,gyro[2],5,12);
            }
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第3-4行显示：电机B/C或左右电机参数
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        if(Car_Mode==Mec_Car || Car_Mode==FourWheel_Car) {
            // 显示电机B标识（第3行）
            OLED_ShowString(0,20,"B"); 
            if( MOTOR_B.Target<0) {
                OLED_ShowString(15,20,"-");
                OLED_ShowNumber(20,20,-MOTOR_B.Target*1000,5,12);
            } else {
                OLED_ShowString(15,20,"+");
                OLED_ShowNumber(20,20,MOTOR_B.Target*1000,5,12);
            }
            
            if( MOTOR_B.Encoder<0) {
                OLED_ShowString(60,20,"-");
                OLED_ShowNumber(75,20,-MOTOR_B.Encoder*1000,5,12);
            } else {
                OLED_ShowString(60,20,"+");
                OLED_ShowNumber(75,20,MOTOR_B.Encoder*1000,5,12);
            }

            // 显示电机C标识（第4行）
            OLED_ShowString(0,30,"C"); 
            if( MOTOR_C.Target<0) {
                OLED_ShowString(15,30,"-");
                OLED_ShowNumber(20,30,-MOTOR_C.Target*1000,5,12);
            } else {
                OLED_ShowString(15,30,"+");
                OLED_ShowNumber(20,30,MOTOR_C.Target*1000,5,12);
            }
                
            if( MOTOR_C.Encoder<0) {
                OLED_ShowString(60,30,"-");
                OLED_ShowNumber(75,30,-MOTOR_C.Encoder*1000,5,12);
            } else {
                OLED_ShowString(60,30,"+");
                OLED_ShowNumber(75,30,MOTOR_C.Encoder*1000,5,12);
            }
        }
        else if(Car_Mode==Tank_Car) {
            // 显示左电机A标识（第3行）
            OLED_ShowString(0,20,"L:"); 
            if( MOTOR_A.Target<0) {
                OLED_ShowString(15,20,"-");
                OLED_ShowNumber(20,20,-MOTOR_A.Target*1000,5,12);
            } else {
                OLED_ShowString(15,20,"+");
                OLED_ShowNumber(20,20,MOTOR_A.Target*1000,5,12);
            }
            
            if( MOTOR_A.Encoder<0) {
                OLED_ShowString(60,20,"-");
                OLED_ShowNumber(75,20,-MOTOR_A.Encoder*1000,5,12);
            } else {
                OLED_ShowString(60,20,"+");
                OLED_ShowNumber(75,20,MOTOR_A.Encoder*1000,5,12);
            }

            // 显示右电机B标识（第4行）
            OLED_ShowString(0,30,"R:"); 
            if( MOTOR_B.Target<0) {
                OLED_ShowString(15,30,"-");
                OLED_ShowNumber(20,30,-MOTOR_B.Target*1000,5,12);
            } else {
                OLED_ShowString(15,30,"+");
                OLED_ShowNumber(20,30,MOTOR_B.Target*1000,5,12);
            }
            
            if( MOTOR_B.Encoder<0) {
                OLED_ShowString(60,30,"-");
                OLED_ShowNumber(75,30,-MOTOR_B.Encoder*1000,5,12);
            } else {
                OLED_ShowString(60,30,"+");
                OLED_ShowNumber(75,30,MOTOR_B.Encoder*1000,5,12);
            }
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第5行显示：电机D或PWM参数
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        if(Car_Mode==Mec_Car || Car_Mode==FourWheel_Car) {
            OLED_ShowString(0,40,"D");  // 显示电机D标识
            if( MOTOR_D.Target<0) {
                OLED_ShowString(15,40,"-");
                OLED_ShowNumber(20,40,- MOTOR_D.Target*1000,5,12);
            } else {
                OLED_ShowString(15,40,"+");
                OLED_ShowNumber(20,40,MOTOR_D.Target*1000,5,12);
            }
            
            if( MOTOR_D.Encoder<0) {
                OLED_ShowString(60,40,"-");
                OLED_ShowNumber(75,40,-MOTOR_D.Encoder*1000,5,12);
            } else {
                OLED_ShowString(60,40,"+");
                OLED_ShowNumber(75,40,MOTOR_D.Encoder*1000,5,12);
            }
        } 
        else if(Car_Mode==Tank_Car) {
            // 显示左电机A PWM值（第3行）
            OLED_ShowString(0,40,"MA"); 
            if( MOTOR_A.Motor_Pwm<0) {
                OLED_ShowString(20,40,"-");
                OLED_ShowNumber(30,40,-MOTOR_A.Motor_Pwm,4,12);
            } else {
                OLED_ShowString(20,40,"+");
                OLED_ShowNumber(30,40,MOTOR_A.Motor_Pwm,4,12);
            }
            
            // 显示右电机B PWM值（第4行）
            OLED_ShowString(60,40,"MB"); 
            if(MOTOR_B.Motor_Pwm<0) {
                OLED_ShowString(80,40,"-");
                OLED_ShowNumber(90,40,-MOTOR_B.Motor_Pwm,4,12);
            } else {
                OLED_ShowString(80,40,"+");
                OLED_ShowNumber(90,40,MOTOR_B.Motor_Pwm,4,12);
            }
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第6行显示：控制模式与控制允许状态
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示当前控制模式
        if(PS2_ON_Flag==1)         OLED_ShowString(0,50,"PS2  ");
        else if (APP_ON_Flag==1)   OLED_ShowString(0,50,"APP  ");
        else if (Remote_ON_Flag==1)OLED_ShowString(0,50,"R-C  ");
        else if (CAN_ON_Flag==1)   OLED_ShowString(0,50,"CAN  ");
        else if (Usart1_ON_Flag==1)OLED_ShowString(0,50,"UART1");
        else if (Usart5_ON_Flag==1)OLED_ShowString(0,50,"UART5");
        else                       OLED_ShowString(0,50,"ROS  ");
        
        // 显示当前小车是否允许控制
        if(Flag_Stop==0)           OLED_ShowString(45,50,"O N");  
        else                       OLED_ShowString(45,50,"OFF"); 
        
        // 显示电压整数部分（第6行）
        OLED_ShowNumber(75,50,Voltage_Show/100,2,12);
        
        // 显示电压小数点（第6行）
        OLED_ShowString(88,50,".");
        
        // 显示电压小数部分（第6行）
        OLED_ShowNumber(98,50,Voltage_Show%100,2,12);
        OLED_ShowString(110,50,"V");
        
        // 处理电压小数部分小于10的情况（补零显示）
        if(Voltage_Show%100<10) {
            OLED_ShowNumber(92,50,0,2,12);
        }	
    }
    else  // 微调模式显示四个舵机的偏移值 
    {
        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第1行显示：舵机Angle1初始角度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示舵机Angle1初始角度
        OLED_ShowString(0,0,"Angle1_init:");
        if( Moveit_Angle1_init<0)	
        {
            OLED_ShowString(100,0,"-");  // 显示负号
            OLED_ShowNumber(110,0,-Moveit_Angle1_init,3,12);  // 显示数值
        }
        else
        {
            OLED_ShowString(100,0,"+");  // 显示正号
            OLED_ShowNumber(110,0,Moveit_Angle1_init,3,12);
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第2行显示：舵机Angle2初始角度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示舵机Angle2初始角度
        OLED_ShowString(0,10,"Angle2_init:");
        if( Moveit_Angle2_init<0)	
        {
            OLED_ShowString(100,10,"-");  // 显示负号
            OLED_ShowNumber(110,10,-Moveit_Angle2_init,3,12);  // 显示数值
        }
        else
        {
            OLED_ShowString(100,10,"+");  // 显示正号
            OLED_ShowNumber(110,10,Moveit_Angle2_init,3,12);
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第3行显示：舵机Angle3初始角度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示舵机Angle3初始角度
        OLED_ShowString(0,20,"Angle3_init:");
        if( Moveit_Angle3_init<0)	
        {
            OLED_ShowString(100,20,"-");  // 显示负号
            OLED_ShowNumber(110,20,-Moveit_Angle3_init,3,12);  // 显示数值
        }
        else
        {
            OLED_ShowString(100,20,"+");  // 显示正号
            OLED_ShowNumber(110,20,Moveit_Angle3_init,3,12);
        }

        // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
        //  第4行显示：舵机Angle4初始角度
        // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
        // 显示舵机Angle4初始角度
        OLED_ShowString(0,30,"Angle4_init:");
        if( Moveit_Angle4_init<0)	
        {
            OLED_ShowString(100,30,"-");  // 显示负号
            OLED_ShowNumber(110,30,-Moveit_Angle4_init,3,12);  // 显示数值
        }
        else
        {
            OLED_ShowString(100,30,"+");  // 显示正号
            OLED_ShowNumber(110,30,Moveit_Angle4_init,3,12);
        }
    }
    // ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁
    //  刷新显示内容
    // ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
    //=============刷新=======================//
    OLED_Refresh_Gram();  // 刷新OLED显示内容
}
/**************************************************************************
【函数名称】APP_Show
【功能概述】向APP发送数据的核心函数
            - 根据不同条件发送不同的数据格式
            - 支持调试界面、首页和波形界面的数据发送
【输入参数】none
【输出参数】none
【关键变量】flag_show: 用于交替发送不同数据
            Left_Figure: 左电机速度（单位0.01m/s）
            Right_Figure: 右电机速度（单位0.01m/s）
            Voltage_Show: 电池电压（百分比形式）
【关键函数】printf: 格式化输出数据到终端或串口
            Get_adc_Average: 获取电位器平均ADC值
【注意】
            - 电压显示格式化处理（转换为百分比形式）
            - 电机速度取绝对值并转换为0.01m/s
            - 数据格式根据flag_show和PID_Send交替发送
**************************************************************************/
void APP_Show(void)
{    
    static u8 flag_show = 0;  // 用于交替发送不同数据的标志
    int Left_Figure, Right_Figure, Voltage_Show;
    
    // 电池电压处理成百分比形式
    // The battery voltage is processed as a percentage
    // 对电池电压处理成百分比形式（假设满电电压12.6V，最小电压10V）
    Voltage_Show = (Voltage * 1000 - 10000) / 27;
    if(Voltage_Show > 100) Voltage_Show = 100;  // 限幅处理，确保不超过100%
    
    // 车轮速度单位转换为0.01m/s，方便在APP显示
    // Wheel speed unit is converted to 0.01m/s for easy display in APP
    // 车轮速度单位转换为0.01m/s，方便在APP显示
    Left_Figure = MOTOR_A.Encoder * 100;  // 左电机速度转换
    if(Left_Figure < 0) Left_Figure = -Left_Figure;  // 取绝对值
    
    Right_Figure = MOTOR_B.Encoder * 100;  // 右电机速度转换
    if(Right_Figure < 0) Right_Figure = -Right_Figure;  // 取绝对值
    
    // 用于交替打印APP数据和显示波形
    // Used to alternately print APP data and display waveform
    // 用于交替打印APP数据和显示波形
    flag_show = !flag_show;  // 切换标志
    
    if(PID_Send == 1) 
    {
        // 发送参数到APP，APP在调试界面显示
        // Send parameters to the APP, the APP is displayed in the debug screen
        // 发送参数到APP，APP在调试界面显示
        // 数据格式：{C<Angle1>:<Angle2>:<Angle3>:<Angle4>:<RC_Velocity>:<Velocity_KP>:<Velocity_KI>}$
        printf("{C%d:%d:%d:%d:%d:%d:%d}$",
            (int)((Moveit_Angle1 + 1.57f) * 100),  // Moveit_Angle1+1.57f确保非负，*100便于整数显示
            (int)((Moveit_Angle2 + 1.57f) * 100),
            (int)((Moveit_Angle3 + 1.57f) * 100),
            (int)((Moveit_Angle4 + 1.57f) * 100),
            (int)RC_Velocity,                     // 当前速度
            (int)Velocity_KP,                     // KP参数
            (int)Velocity_KI);                    // KI参数
        PID_Send = 0;  // 清除发送标志
    }	
    else if(flag_show == 0) 
    {
        // 发送参数到APP，APP在首页显示
        // Send parameters to the APP and the APP will be displayed on the front page
        // 发送参数到APP，APP在首页显示
        // 数据格式：{A<Left_Figure>:<Right_Figure>:<Voltage_Show>:<Gyro_Z>}$
        printf("{A%d:%d:%d:%d}$", 
            (u8)Left_Figure,      // 左电机速度（单位0.01m/s）
            (u8)Right_Figure,     // 右电机速度（单位0.01m/s）
            Voltage_Show,         // 电池电压百分比
            (int)gyro[2]);         // 陀螺仪Z轴数据
    }
    else
    {
        // 发送参数到APP，APP在波形界面显示
        // Send parameters to the APP, the APP is displayed in the waveform interface
        // 发送参数到APP，APP在波形界面显示
        // 数据格式：{B<Gyro_X>:<Gyro_Y>:<Gyro_Z>}$
        printf("{B%d:%d:%d}$", 
            (int)gyro[0],  // 陀螺仪X轴数据
            (int)gyro[1],  // 陀螺仪Y轴数据
            (int)gyro[2]);  // 陀螺仪Z轴数据
    }
}

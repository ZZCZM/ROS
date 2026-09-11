/**
  ******************************************************************************
  * @file    balance.c
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
#include "balance.h"

/**
 * @brief 初始化系统计时变量和标志位
 * @note 初始化时间计数器、机器人模式检测标志位和命令丢失计数器
 */
volatile int Time_count = 0; // Time variable // 计时变量

// Robot mode is wrong to detect flag bits
// 机器人模式是否出错检测标志位
int robot_mode_check_flag = 0; 

// Command lost count variable
// 命令丢失计数变量
u8 command_lost_count = 0;

// Test number variable
// 测试编号变量
short test_num;

// Encoder raw data structure
// 编码器原始数据结构  
Encoder OriginalEncoder; 

/**************************************************************************
Function: The inverse kinematics solution is used to calculate the target speed of each wheel according to the target speed of three axes
Input   : X and Y, Z axis direction of the target movement speed
Output  : none
函数功能：运动学逆解，根据三轴目标速度计算各车轮目标转速
入口参数：X和Y、Z轴方向的目标运动速度
返回  值：无
**************************************************************************/
void Drive_Motor(float Vx, float Vy, float Vz)
{
    float amplitude = 3.5; // Wheel target speed limit // 车轮目标速度限幅
    
    // Speed smoothing is enabled when moving the omnidirectional trolley
    // 全向移动小车才开启速度平滑处理
    if(Car_Mode == Mec_Car)
    {
        Smooth_control(Vx, Vy, Vz); // Smoothing the input speed // 对输入速度进行平滑处理
  
        // Get the smoothed data 
        // 获取平滑处理后的数据			
        Vx = smooth_control.VX;     
        Vy = smooth_control.VY;
        Vz = smooth_control.VZ;
    }
    
    // Mecanum wheel car
    // 麦克纳姆轮小车
    if (Car_Mode == Mec_Car) 
    {
        // Inverse kinematics // 运动学逆解
        MOTOR_A.Target = +Vy + Vx - Vz * (Axle_spacing + Wheel_spacing);
        MOTOR_B.Target = -Vy + Vx - Vz * (Axle_spacing + Wheel_spacing);
        MOTOR_C.Target = +Vy + Vx + Vz * (Axle_spacing + Wheel_spacing);
        MOTOR_D.Target = -Vy + Vx + Vz * (Axle_spacing + Wheel_spacing);
        
        // Wheel (motor) target speed limit // 车轮(电机)目标速度限幅
        MOTOR_A.Target = target_limit_float(MOTOR_A.Target, -amplitude, amplitude); 
        MOTOR_B.Target = target_limit_float(MOTOR_B.Target, -amplitude, amplitude); 
        MOTOR_C.Target = target_limit_float(MOTOR_C.Target, -amplitude, amplitude); 
        MOTOR_D.Target = target_limit_float(MOTOR_D.Target, -amplitude, amplitude); 
    } 
    
    // FourWheel car
    // 四驱车
    else if (Car_Mode == FourWheel_Car) 
    {	
        // Inverse kinematics // 运动学逆解
        MOTOR_A.Target  = Vx - Vz * (Wheel_spacing + Axle_spacing) / 2.0f; // 计算出左轮的目标速度
        MOTOR_B.Target  = Vx - Vz * (Wheel_spacing + Axle_spacing) / 2.0f; // 计算出左轮的目标速度
        MOTOR_C.Target  = Vx + Vz * (Wheel_spacing + Axle_spacing) / 2.0f; // 计算出右轮的目标速度
        MOTOR_D.Target  = Vx + Vz * (Wheel_spacing + Axle_spacing) / 2.0f; // 计算出右轮的目标速度
                    
        // Wheel (motor) target speed limit // 车轮(电机)目标速度限幅
        MOTOR_A.Target = target_limit_float(MOTOR_A.Target, -amplitude, amplitude); 
        MOTOR_B.Target = target_limit_float(MOTOR_B.Target, -amplitude, amplitude); 
        MOTOR_C.Target = target_limit_float(MOTOR_C.Target, -amplitude, amplitude); 
        MOTOR_D.Target = target_limit_float(MOTOR_D.Target, -amplitude, amplitude); 	
    }
    
    // Tank Car
    // 履带车
    else if (Car_Mode == Tank_Car) 
    {
        // Inverse kinematics // 运动学逆解
        MOTOR_A.Target  = Vx - Vz * (Wheel_spacing) / 2.0f;    // 计算出左轮的目标速度
        MOTOR_B.Target  = Vx + Vz * (Wheel_spacing) / 2.0f;    // 计算出右轮的目标速度
			
        // Wheel (motor) target speed limit // 车轮(电机)目标速度限幅
        MOTOR_A.Target = target_limit_float(MOTOR_A.Target, -amplitude, amplitude); 
        MOTOR_B.Target = target_limit_float(MOTOR_B.Target, -amplitude, amplitude); 
        MOTOR_C.Target = 0; // Out of use // 没有使用到
        MOTOR_D.Target = 0; // Out of use // 没有使用到
    }
}
/**************************************************************************
【函数名称】Balance_task
【功能概述】FreeRTOS核心运动控制任务，实现机器人多模式运动控制
            - 100Hz周期执行运动控制逻辑
            - 集成多种遥控设备输入处理
            - 电机闭环速度控制
            - 安全保护机制（电压检测/急停）
【输入参数】void *pvParameters (FreeRTOS任务标准参数，本例未使用)
【输出参数】无
【全局变量】Time_count, Check, *ON_Flag系列, Car_Mode等
【调用函数】Get_Velocity_Form_Encoder(), PS2_control(), Drive_Motor()等
**************************************************************************/
void Balance_task(void *pvParameters)
{ 
    u32 lastWakeTime = getSysTickCnt();  // 获取系统节拍计数作为基准时间

    while(1)  // FreeRTOS任务标准无限循环
    {	
        // 任务周期控制：100Hz（10ms间隔）精确周期执行
        vTaskDelayUntil(&lastWakeTime, F2T(RATE_100_HZ)); 

        // 系统运行时间累计（30秒后停止计数）
        if(Time_count<3000)Time_count++;  // 3000*10ms=30秒

        // 获取编码器数据并转换单位（转换为m/s或rad/s等国际单位）
        Get_Velocity_Form_Encoder();   

        // 舵机初始位置微调模式使能检测
        if(Servo_init_angle_adjust == 1) 
            Servo_init_angle_adjust_function(); 

        // 系统自检模式判断（Check=0表示正常模式）
        if(Check==0) 
        {
            // 多模式遥控选择（优先级顺序：APP > PS2 > 直接控制）
            if (APP_ON_Flag)            // APP遥控模式
                Get_RC();               // 处理蓝牙APP控制指令
            else if (PS2_ON_Flag)       // PS2手柄模式
                PS2_control();          // 处理PS2无线手柄输入
            else                        // 直接控制模式（CAN/串口）
                Drive_Motor(Move_X, Move_Y, Move_Z);  // 驱动电机执行目标速度

            Key();  // 按键检测（用于陀螺仪零点校准等操作）
            
            Drive_Robot_Arm();  // 机械臂伺服控制（需配合舵机模块）

            // 系统安全状态检测（电压正常+使能开关ON+无软件故障）
            if(Turn_Off(Voltage)==0) 
            { 			
                // 增量式PI速度闭环计算（四个电机独立控制）
                MOTOR_A.Motor_Pwm=Incremental_PI_A(MOTOR_A.Encoder, MOTOR_A.Target);
                MOTOR_B.Motor_Pwm=Incremental_PI_B(MOTOR_B.Encoder, MOTOR_B.Target);
                MOTOR_C.Motor_Pwm=Incremental_PI_C(MOTOR_C.Encoder, MOTOR_C.Target);
                MOTOR_D.Motor_Pwm=Incremental_PI_D(MOTOR_D.Encoder, MOTOR_D.Target);
                
                Limit_Pwm(16700);  // PWM输出限幅（16800为满幅值，保留100单位裕量）

                // 多车型PWM极性适配（不同车型的电机安装方向不同）
                switch(Car_Mode)
                {
                    case Mec_Car:       // 麦克纳姆轮车型（全向移动）
                        Set_Pwm(-MOTOR_A.Motor_Pwm, MOTOR_B.Motor_Pwm, 
                                MOTOR_C.Motor_Pwm, -MOTOR_D.Motor_Pwm,
                                Position1,Position2,Position3,Position4); 
                        break;
                    case FourWheel_Car: // 四驱车型（差速转向）
                        Set_Pwm(-MOTOR_A.Motor_Pwm, MOTOR_B.Motor_Pwm,
                                MOTOR_C.Motor_Pwm, -MOTOR_D.Motor_Pwm,
                                Position1,Position2,Position3,Position4);
                        break;
                    case Tank_Car:      // 履带车型（双电机差速）
                        Set_Pwm(-MOTOR_A.Motor_Pwm, -MOTOR_B.Motor_Pwm,
                               -MOTOR_C.Motor_Pwm, -MOTOR_D.Motor_Pwm,
                                Position1,Position2,Position3,Position4);
                        break;
                }
             }
             else  // 系统保护状态（急停或故障）
                Set_Pwm(0,0,0,0,0,0,0,0);  // 全PWM输出清零
         }	
    }  
}

/**************************************************************************
【函数名称】Set_Pwm
【功能概述】电机PWM输出及方向控制函数
            - 将计算后的PWM值写入硬件寄存器
            - 处理电机正反转逻辑
            - 支持多路舵机控制
【输入参数】motor_a~d: 四个电机的PWM值（有符号数）
           servo1~4: 四个舵机的PWM占空比
【输出参数】无
【寄存器操作】PWMA1/PWMA2等：电机驱动寄存器
             Servo_PWM1~4：舵机控制寄存器
【注意事项】PWM满幅值为16800，实际使用16799避免溢出
**************************************************************************/
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d,
             int servo1,int servo2,int servo3,int servo4)
{
    /* 电机A控制（双极性H桥控制）*/
    if(motor_a<0)  // 反转模式
        PWMA1=16799,        // 正极PWM全开
        PWMA2=16799+motor_a;// 负极PWM按比例输出（motor_a为负值）
    else           // 正转模式
        PWMA2=16799,        // 负极PWM全开 
        PWMA1=16799-motor_a;// 正极PWM按比例输出

    /* 电机B控制（逻辑同上）*/  
    if(motor_b<0)
        PWMB1=16799,
        PWMB2=16799+motor_b;
    else 
        PWMB2=16799,
        PWMB1=16799-motor_b;

    /* 电机C控制（逻辑同上）*/
    if(motor_c<0)
        PWMC1=16799,
        PWMC2=16799+motor_c;
    else 
        PWMC2=16799,
        PWMC1=16799-motor_c;

    /* 电机D控制（逻辑同上）*/
    if(motor_d<0)
        PWMD1=16799,
        PWMD2=16799+motor_d;
    else 
        PWMD2=16799,
        PWMD1=16799-motor_d;

    /* 舵机PWM输出控制（固定周期，可变占空比）*/
    Servo_PWM1 = servo1;  // 舵机1位置控制
    Servo_PWM2 = servo2;  // 舵机2位置控制
    Servo_PWM3 = servo3;  // 舵机3位置控制 
    Servo_PWM4 = servo4;  // 舵机4位置控制
}

/**************************************************************************
【函数名称】Limit_Pwm
【功能概述】电机PWM输出限幅控制
            - 对四个电机的PWM输出进行统一幅值限制
            - 防止过大的PWM值损坏电机驱动器
            - 维持闭环控制的稳定性
【输入参数】amplitude: 允许的最大PWM绝对值（典型值16700）
【输出参数】无
【全局变量】MOTOR_A~D结构体的Motor_Pwm成员
【调用函数】target_limit_float()
**************************************************************************/
void Limit_Pwm(int amplitude)
{	
    // 对四个电机PWM输出进行对称限幅（-amplitude ≤ PWM ≤ amplitude）
    MOTOR_A.Motor_Pwm = target_limit_float(MOTOR_A.Motor_Pwm, -amplitude, amplitude);
    MOTOR_B.Motor_Pwm = target_limit_float(MOTOR_B.Motor_Pwm, -amplitude, amplitude);
    MOTOR_C.Motor_Pwm = target_limit_float(MOTOR_C.Motor_Pwm, -amplitude, amplitude);
    MOTOR_D.Motor_Pwm = target_limit_float(MOTOR_D.Motor_Pwm, -amplitude, amplitude);
}    

/**************************************************************************
【函数名称】target_limit_float
【功能概述】通用浮点数限幅函数
            - 将输入值限制在[low, high]区间
            - 用于信号调理和参数保护
【输入参数】insert: 待限幅的浮点数值
           low: 下限值
           high: 上限值
【返回参数】经过限幅后的浮点数值
【算法说明】三段式限幅逻辑（低于下限取low，高于上限取high）
**************************************************************************/
float target_limit_float(float insert, float low, float high)
{
    if (insert < low)
        return low;    // 低于下限时返回下限值
    else if (insert > high)
        return high;   // 超过上限时返回上限值
    else
        return insert; // 正常范围内保持原值
}

/**************************************************************************
【函数名称】target_limit_int
【功能概述】通用整型数限幅函数
            - 将输入值限制在[low, high]区间
            - 用于整型参数的保护
【输入参数】insert: 待限幅的整型数值
           low: 下限值
           high: 上限值
【返回参数】经过限幅后的整型数值
【算法说明】与浮点版本逻辑一致，处理整型数据
**************************************************************************/
int target_limit_int(int insert, int low, int high)
{
    if (insert < low)
        return low;    // 低于下限时返回下限值
    else if (insert > high)
        return high;   // 超过上限时返回上限值
    else
        return insert; // 正常范围内保持原值
}

/**************************************************************************
【函数名称】Turn_Off
【功能概述】系统安全检测及急停处理
            - 检测电池电压是否低于安全阈值（10单位）
            - 检查急停标志位Flag_Stop
            - 异常状态下切断所有电机输出
【输入参数】voltage: 当前电池电压值（需换算为实际单位）
【返回参数】u8: 系统使能状态（1=禁用，0=正常）
【寄存器操作】直接清零所有PWM输出寄存器
【注意事项】电压单位需与系统配置一致，典型为1V单位时10对应10V
**************************************************************************/
u8 Turn_Off(int voltage)
{
    u8 temp;
    // 异常条件：电压过低或急停标志置位
    if(voltage < 10 || Flag_Stop == 1) // 电压单位为1V，10对应10V
    {                                                 
        temp = 1;       // 设置禁用标志
        // 立即关闭所有电机驱动（H桥两路PWM都置零）
        PWMA1 = 0; PWMA2 = 0;  // 电机A停止
        PWMB1 = 0; PWMB2 = 0;  // 电机B停止
        PWMC1 = 0; PWMC2 = 0;  // 电机C停止 
        PWMD1 = 0; PWMD2 = 0;  // 电机D停止
    }
    else
        temp = 0;  // 系统正常
    return temp;   // 返回系统状态			
}

/**************************************************************************
【函数名称】myabs
【功能概述】有符号32位整数取绝对值
            - 处理正负数值的绝对值转换
            - 返回无符号32位整型
【输入参数】a: 待处理的有符号长整型
【返回参数】u32: 绝对值计算结果
【注意事项】当输入为-2147483648时可能溢出（未处理极限情况）
**************************************************************************/
u32 myabs(long int a)
{ 		   
    u32 temp;
    if(a < 0)  
        temp = -a;  // 负值取反
    else 
        temp = a;   // 正值保持
    return temp;    // 返回无符号结果
}

/**************************************************************************
【函数名称】Incremental_PI_A
【功能概述】电机A增量式PI速度控制器
            - 实现离散化的增量式PI控制算法
            - 动态调整KP/KI参数的非线性控制
            - 带输出限幅保护
【输入参数】Encoder: 编码器反馈速度值（单位需与Target一致）
           Target: 目标速度值
【返回参数】int: PWM控制量（范围±16700）
【控制公式】ΔPWM = KP*(e(k)-e(k-1)) + KI*e(k)
            KP/KI = 218 + 179*(V/Vmax) + 21*(V/Vmax)^2 (Vmax=3.5)
【注意事项】使用静态变量保持上次控制状态
**************************************************************************/
int Incremental_PI_A(float Encoder, float Target)
{ 	
    // 静态变量保持控制状态（跨周期保持）
    static float Bias, Pwm, Last_bias; // Bias:当前偏差, Last_bias:上次偏差, Pwm:累积PWM输出
    
    // 计算速度偏差（目标值 - 实际值）
    Bias = Target - Encoder;  // 单位需保持一致（如rad/s）
    
    // 动态参数计算（基于当前速度的非线性调整）
    // 公式分解：218基础值 + 179*(速度/3.5) + 21*(速度/3.5)^2
    // 3.5为速度最大量程（如3.5 rad/s），实现参数随速度变化自适应
    Velocity_KI = 218.0f + 179.0f * (Encoder/3.5f) + 21.0f * (Encoder*Encoder)/(3.5f*3.5f);
    Velocity_KP = 218.0f + 179.0f * (Encoder/3.5f) + 21.0f * (Encoder*Encoder)/(3.5f*3.5f);

    // 增量式PI计算（离散化公式）
    // ΔPWM = KP*(当前偏差-上次偏差) + KI*当前偏差
    Pwm += Velocity_KP * (Bias - Last_bias) + Velocity_KI * Bias;
    
    // PWM输出硬限幅（防止驱动器过载）
    if(Pwm > 16700)  Pwm = 16700;   // 正向饱和限制（对应满幅的99.4%）
    if(Pwm < -16700) Pwm = -16700;  // 反向饱和限制
    
    // 保存当前偏差供下次计算使用
    Last_bias = Bias;  // 更新历史偏差值
    
    return (int)Pwm;    // 返回整型PWM值（浮点转整型隐式转换）
}

/**************************************************************************
【函数名称】Incremental_PI_B
【功能概述】电机B增量式PI速度控制器（实现同A）
【硬件说明】B电机独立控制，参数与A电机相同但独立存储
**************************************************************************/
int Incremental_PI_B(float Encoder, float Target)
{  
    // 控制逻辑与A电机完全相同，但使用独立静态变量
    static float Bias, Pwm, Last_bias;
    Bias = Target - Encoder; 
    Velocity_KI = 218.0f + 179.0f*(Encoder/3.5f) + 21.0f*(Encoder*Encoder)/(3.5f*3.5f);
    Velocity_KP = 218.0f + 179.0f*(Encoder/3.5f) + 21.0f*(Encoder*Encoder)/(3.5f*3.5f);
    Pwm += Velocity_KP*(Bias - Last_bias) + Velocity_KI*Bias;  
    if(Pwm>16700) Pwm=16700;
    if(Pwm<-16700) Pwm=-16700;
    Last_bias = Bias;
    return (int)Pwm;  // 返回值范围：-16700 ~ +16700
}

/**************************************************************************
【函数名称】Incremental_PI_C
【功能概述】电机C增量式PI速度控制器（实现同A）
【安装注意】需确保电机转向与程序极性匹配
**************************************************************************/
int Incremental_PI_C(float Encoder, float Target)
{  
    // 独立控制通道，变量作用域隔离
    static float Bias, Pwm, Last_bias;
    Bias = Target - Encoder;
    Velocity_KI = 218.0f + 179.0f*(Encoder/3.5f) + 21.0f*(Encoder*Encoder)/(3.5f*3.5f);
    Velocity_KP = 218.0f + 179.0f*(Encoder/3.5f) + 21.0f*(Encoder*Encoder)/(3.5f*3.5f);
    Pwm += Velocity_KP*(Bias - Last_bias) + Velocity_KI*Bias; 
    if(Pwm>16700) Pwm=16700;
    if(Pwm<-16700) Pwm=-16700;
    Last_bias = Bias;
    return (int)Pwm;  // 实际输出需结合Set_Pwm()的极性处理
}

/**************************************************************************
【函数名称】Incremental_PI_D
【功能概述】电机D增量式PI速度控制器（实现同A）
【调试建议】建议单独测试各电机响应曲线
**************************************************************************/
int Incremental_PI_D(float Encoder, float Target)
{  
    // 每个电机独立控制确保运动解耦
    static float Bias, Pwm, Last_bias;
    Bias = Target - Encoder;
    Velocity_KI = 218.0f + 179.0f*(Encoder/3.5f) + 21.0f*(Encoder*Encoder)/(3.5f*3.5f);
    Velocity_KP = 218.0f + 179.0f*(Encoder/3.5f) + 21.0f*(Encoder*Encoder)/(3.5f*3.5f);
    Pwm += Velocity_KP*(Bias - Last_bias) + Velocity_KI*Bias;  
    if(Pwm>16700) Pwm=16700;
    if(Pwm<-16700) Pwm=-16700;
    Last_bias = Bias;
    return (int)Pwm;  // PWM输出最终由Set_Pwm()处理方向
}

/**************************************************************************
【函数名称】Position_PID1
【功能概述】位置式PID控制器（通道1）
            - 实现标准位置式PID算法
            - 含积分分离和微分先行处理
            - 适用于位置/角度等需要累积误差的控制场景
【输入参数】Encoder: 反馈测量值
           Target: 目标设定值
【返回参数】float: 控制量输出（需后续处理）
【算法公式】Pwm = (KP*e + KI*Σe + KD*Δe)/100
【注意事项】积分项未做抗饱和处理，长时间误差需外部清零
**************************************************************************/
float Position_PID1(float Encoder, float Target)
{ 	
    // 静态变量保持控制状态（跨周期有效）
    static float Bias,          // 当前偏差值 e(k)
                 Pwm,           // 输出控制量
                 Integral_bias,// 积分项 Σe(k)
                 Last_Bias;     // 上次偏差 e(k-1)
    
    // 计算当前偏差（目标值 - 实际值）
    Bias = Target - Encoder;        // 单位需与系统物理量一致
    
    // 积分项累加（可能产生积分饱和）
    Integral_bias += Bias;          // Σe(k) = Σe(k-1) + e(k)
    
    // PID复合计算（各系数预先除以100进行标幺处理）
    Pwm = (Position_KP * Bias +            // 比例项
           Position_KI * Integral_bias +  // 积分项
           Position_KD * (Bias - Last_Bias)) // 微分项（Δe = e(k)-e(k-1)）
           / 100.0f;                       // 系数缩放（假设KP/KI/KD以百分比形式存储）
    
    // 保存历史偏差
    Last_Bias = Bias;                // 当前偏差转为上次偏差
    
    return Pwm;  // 输出控制量（需后续限幅处理）
}

/**************************************************************************
【函数名称】Position_PID2
【功能概述】位置式PID控制器（通道2）
            - 结构功能与Position_PID1完全相同
            - 独立控制通道，变量作用域隔离
【硬件对应】建议对应第二路执行机构控制
**************************************************************************/
float Position_PID2(float Encoder, float Target)
{ 	
    // 独立控制通道变量组
    static float Bias, Pwm, Integral_bias, Last_Bias;
    Bias = Target - Encoder;         // 偏差计算
    Integral_bias += Bias;           // 积分累积
    Pwm = (Position_KP * Bias + 
           Position_KI * Integral_bias +
           Position_KD * (Bias - Last_Bias))
           / 100.0f;                // 保持相同的系数缩放
    Last_Bias = Bias;
    return Pwm;  // 输出到第二路执行器
}

/**************************************************************************
【函数名称】Position_PID3
【功能概述】位置式PID控制器（通道3）
            - 结构功能与Position_PID1完全相同
            - 适用于多自由度系统中的第三轴控制
**************************************************************************/
float Position_PID3(float Encoder, float Target)
{ 	
    static float Bias, Pwm, Integral_bias, Last_Bias;
    Bias = Target - Encoder;         // 目标-实际偏差
    Integral_bias += Bias;           // 积分项持续累积
    Pwm = (Position_KP * Bias + 
           Position_KI * Integral_bias +
           Position_KD * (Bias - Last_Bias))
           / 100.0f;                // 统一缩放比例
    Last_Bias = Bias;
    return Pwm;  // 第三通道输出
}

/**************************************************************************
【函数名称】Position_PID4
【功能概述】位置式PID控制器（通道4）
            - 结构功能与Position_PID1完全相同
            - 建议用于备用控制通道或辅助执行器
**************************************************************************/
float Position_PID4(float Encoder, float Target)
{ 	
    static float Bias, Pwm, Integral_bias, Last_Bias;
    Bias = Target - Encoder;         // 偏差获取
    Integral_bias += Bias;           // 积分项累加
    Pwm = (Position_KP * Bias + 
           Position_KI * Integral_bias +
           Position_KD * (Bias - Last_Bias))
           / 100.0f;                // 保持系数一致性
    Last_Bias = Bias;
    return Pwm;  // 第四控制量输出
}

/**************************************************************************
【函数名称】Position_PID5
【功能概述】位置式PID控制器（通道5）
            - 结构功能与Position_PID1完全相同
            - 用于多路控制系统中的第五轴
**************************************************************************/
float Position_PID5(float Encoder, float Target)
{ 	
    static float Bias, Pwm, Integral_bias, Last_Bias;
    Bias = Target - Encoder;         // 实时偏差计算
    Integral_bias += Bias;           // 积分器工作
    Pwm = (Position_KP * Bias + 
           Position_KI * Integral_bias +
           Position_KD * (Bias - Last_Bias))
           / 100.0f;                // 参数标准化处理
    Last_Bias = Bias;
    return Pwm;  // 第五通道输出
}

/**************************************************************************
【函数名称】Position_PID6
【功能概述】位置式PID控制器（通道6）
            - 结构功能与Position_PID1完全相同
            - 系统最大控制通道预留
**************************************************************************/
float Position_PID6(float Encoder, float Target)
{ 	
    static float Bias, Pwm, Integral_bias, Last_Bias;
    Bias = Target - Encoder;         // 最终偏差计算
    Integral_bias += Bias;           // 积分项更新
    Pwm = (Position_KP * Bias + 
           Position_KI * Integral_bias +
           Position_KD * (Bias - Last_Bias))
           / 100.0f;                // 保持算法一致性
    Last_Bias = Bias;
    return Pwm;  // 第六控制通道输出
}
/**************************************************************************
【函数名称】Get_RC
【功能概述】APP遥控指令处理核心函数
            - 解析串口接收的APP控制指令
            - 支持多车型运动控制（全向轮/四驱车/履带车）
            - 集成底盘运动与机械臂控制模式切换
【输入参数】无（通过全局变量获取遥控数据）
【输出参数】无（修改全局运动控制变量）
【控制模式】car_A_steer_flag: 
            1=底盘运动模式，2=机械臂控制模式
【关键变量】Flag_Direction: 方向指令(1-8)
           RC_Velocity: 速度基准值(单位mm/s)
**************************************************************************/
void Get_RC(void)
{
    u8 Flag_Move = 1;            // 运动有效标志（1=方向控制有效）
    float step = 0.003;          // 机械臂关节调整步长（约0.17度）
    
    // 底盘运动控制模式
    if(car_A_steer_flag == 1) 
    {
        // 全向轮车型运动处理（麦克纳姆轮）
        if(Car_Mode == Mec_Car) 
        {
            // 八方向运动控制（笛卡尔坐标系）
            switch(Flag_Direction) 
            { 
                case 1:  // 正前
                    Move_X = RC_Velocity; Move_Y = 0;        break;
                case 2:  // 右前斜向
                    Move_X = RC_Velocity; Move_Y = -RC_Velocity; break;
                case 3:  // 正右横移
                    Move_X = 0;         Move_Y = -RC_Velocity; break; 
                case 4:  // 右后斜向
                    Move_X = -RC_Velocity; Move_Y = -RC_Velocity; break;
                case 5:  // 正后
                    Move_X = -RC_Velocity; Move_Y = 0;        break;
                case 6:  // 左后斜向
                    Move_X = -RC_Velocity; Move_Y = RC_Velocity;  break;
                case 7:  // 正左横移
                    Move_X = 0;         Move_Y = RC_Velocity;  break;
                case 8:  // 左前斜向
                    Move_X = RC_Velocity;  Move_Y = RC_Velocity;  break;
                default: // 停止
                    Move_X = 0; Move_Y = 0; Flag_Move = 0; break;
            }

            // 无方向指令时的自转处理
            if(Flag_Move == 0) 
            {    
                // 左右自转控制（角速度与线速度联动）
                if(Flag_Left == 1)       // 左自转
                    Move_Z = PI/2 * (RC_Velocity/500);  // 最大π/2 rad/s（约90度/秒）
                else if(Flag_Right == 1) // 右自转
                    Move_Z = -PI/2 * (RC_Velocity/500);
                else                     // 停止自转
                    Move_Z = 0;
            }
        }
        // 非全向轮车型（四驱/履带）
        else 
        {
            // 八方向运动控制（极坐标系）
            switch(Flag_Direction) 
            {
                case 1:  // 正前直行
                    Move_X = +RC_Velocity; Move_Z = 0;    break;
                case 2:  // 右前转向
                    Move_X = +RC_Velocity; Move_Z = -PI/2; break;
                case 3:  // 原地右转
                    Move_X = 0;         Move_Z = -PI/2; break;  
                case 4:  // 右后转向
                    Move_X = -RC_Velocity; Move_Z = -PI/2;  break;   
                case 5:  // 正后倒退
                    Move_X = -RC_Velocity; Move_Z = 0;    break; 
                case 6:  // 左后转向
                    Move_X = -RC_Velocity; Move_Z = +PI/2;  break; 
                case 7:  // 原地左转
                    Move_X = 0;         Move_Z = +PI/2;  break;
                case 8:  // 左前转向
                    Move_X = +RC_Velocity; Move_Z = +PI/2;  break;
                default: // 停止
                    Move_X = 0; Move_Z = 0;       break;
            }

            // 独立转向控制（优先级高于方向指令）
            if(Flag_Left == 1)      // 持续左转
                Move_Z = PI/2;      // 固定转向角速度
            else if(Flag_Right == 1)// 持续右转
                Move_Z = -PI/2;
        }

        // 坦克/四驱车型差速补偿
        if(Car_Mode==Tank_Car || Car_Mode==FourWheel_Car)
        {
            if(Move_X < 0) 
                Move_Z = -Move_Z;  // 倒车时差速反向
            Move_Z = Move_Z * RC_Velocity/500; // 角速度与线速度比例控制
        }
    }    
    // 机械臂控制模式
    else if (car_A_steer_flag == 2)
    {
        // 机械臂四关节控制（八个方向对应四个关节的正反转）
        switch(Flag_Direction) 
        {
            case 1: Moveit_Angle1 += step; break; // 关节1正转
            case 2: Moveit_Angle2 += step; break; // 关节2正转
            case 3: Moveit_Angle3 += step; break; // 关节3正转
            case 4: Moveit_Angle4 += step; break; // 关节4正转
            case 5: Moveit_Angle1 -= step; break; // 关节1反转
            case 6: Moveit_Angle2 -= step; break; // 关节2反转
            case 7: Moveit_Angle3 -= step; break; // 关节3反转
            case 8: Moveit_Angle4 -= step; break; // 关节4反转
        }

        // 关节角度限幅（±1.57rad≈±90度）
        Moveit_Angle1 = target_limit_float(Moveit_Angle1, -1.57, 1.57);
        Moveit_Angle2 = target_limit_float(Moveit_Angle2, -1.57, 1.57);
        Moveit_Angle3 = target_limit_float(Moveit_Angle3, -1.57, 1.57);
        Moveit_Angle4 = target_limit_float(Moveit_Angle4, -1.57, 1.57);
    }

    // 单位系统转换（底盘控制量）
    Move_X = Move_X/1000;   // mm/s -> m/s（线速度）
    Move_Y = Move_Y/1000;   // mm/s -> m/s（横向速度）
    Move_Z = Move_Z;        // rad/s 保持原单位

    // 执行运动学解算并驱动电机
    Drive_Motor(Move_X, Move_Y, Move_Z);
}
/**************************************************************************
【函数名称】PS2_control
【功能概述】PS2手柄控制命令处理核心函数
            - 解析PS2手柄模拟摇杆及按键输入
            - 支持多车型运动控制
            - 集成机械臂关节控制
【输入参数】无（通过全局变量PS2_LX/PS2_LY等获取数据）
【输出参数】无（修改全局运动控制变量）
【关键特性】死区处理/单位转换/安全锁定机制
**************************************************************************/
void PS2_control(void)
{
    int LX, LY, RY;                // 摇杆原始值临时存储
    int Threshold = 20;            // 摇杆死区阈值（±20）
    float step1 = 0.005;           // 机械臂关节步进量（约0.28度）

    // PS2摇杆数据预处理（坐标系转换）
    LY = -(PS2_LX - 128);          // 左摇杆X轴 -> 运动Y轴（坐标系镜像）
    LX = -(PS2_LY - 128);          // 左摇杆Y轴 -> 运动X轴（坐标系镜像）
    RY = -(PS2_RX - 128);          // 右摇杆X轴 -> 旋转Z轴

    // 摇杆死区处理（消除零漂）
    if(LX > -Threshold && LX < Threshold) LX = 0;
    if(LY > -Threshold && LY < Threshold) LY = 0;
    if(RY > -Threshold && RY < Threshold) RY = 0;

    // 基础运动量计算（比例缩放）
    Move_X = LX * RC_Velocity / 128;   // X轴线速度（mm/s）
    Move_Y = LY * RC_Velocity / 128;   // Y轴线速度（全向轮专用）
    Move_Z = RY * (PI/2) / 128;        // Z轴角速度（rad/s）

    // 车型相关特殊处理
    if(Car_Mode == Mec_Car) {
        // 全向轮车型：角速度与线速度联动
        Move_Z = Move_Z * RC_Velocity / 500;
    } else if(Car_Mode==Tank_Car || Car_Mode==FourWheel_Car) {
        // 履带/四驱车型：倒车时差速补偿
        if(Move_X < 0) Move_Z = -Move_Z;
        Move_Z = Move_Z * RC_Velocity / 500;
    }

    // 单位系统转换
    Move_X /= 1000;  // mm/s -> m/s
    Move_Y /= 1000;  // mm/s -> m/s
    Move_Z = Move_Z; // rad/s保持原单位

    // 安全锁定机制（特定摇杆组合急停）
    if((PS2_LX<100 && PS2_RX>200) || (PS2_LX>200 && PS2_RX<100)) {
        Move_X=0; Move_Y=0; Move_Z=0;  // 防止异常摇杆组合导致失控
    }

    // 机械臂关节控制（非初始化模式时生效）
    if(Servo_init_angle_adjust == 0) {
        switch(PS2_KEY) {              // PS2按键编码解析
            case 5:  Moveit_Angle1 -= step1; break;  // △按钮：关节1-
            case 6:  Moveit_Angle2 += step1; break;  // ○按钮：关节2+
            case 13: Moveit_Angle3 += step1; break;  // L1按钮：关节3+
            case 14: Moveit_Angle4 += step1; break;  // R1按钮：关节4+
            case 7:  Moveit_Angle1 += step1; break;  // □按钮：关节1+
            case 8:  Moveit_Angle2 -= step1; break;  // ×按钮：关节2-
            case 15: Moveit_Angle3 -= step1; break;  // L2按钮：关节3-
            case 16: Moveit_Angle4 -= step1; break;  // R2按钮：关节4-
            case 11: RC_Velocity += 5; break;       // ↑方向键：加速
            case 9:  RC_Velocity -= 5; break;       // ↓方向键：减速
            default: break;
        }
    }

    // 机械臂关节角度限幅（±1.57rad≈±90度）
    Moveit_Angle1 = target_limit_float(Moveit_Angle1, -1.57, 1.57);
    Moveit_Angle2 = target_limit_float(Moveit_Angle2, -1.57, 1.57);
    Moveit_Angle3 = target_limit_float(Moveit_Angle3, -1.57, 1.57);
    Moveit_Angle4 = target_limit_float(Moveit_Angle4, -1.57, 1.57);

    // 执行运动学解算
    Drive_Motor(Move_X, Move_Y, Move_Z);
}

/**************************************************************************
【函数名称】Remote_Control
【功能概述】航模遥控器控制命令处理核心函数
            - 解析PWM型航模遥控器信号
            - 支持标准航模通道布局
            - 集成启动安全延时机制
【输入参数】无（通过全局变量Remoter_Ch1-4获取数据）
【关键特性】通道滤波/油门联动/新手保护
**************************************************************************/
void Remote_Control(void)
{
    static u8 thrice = 100;         // 启动保护计数器（约1秒）
    int Threshold = 100;            // 摇杆死区阈值（±100）
    int LX, LY, RY, RX;            // 通道原始值临时存储
    int Remote_RCvelocity;         // 动态速度基准值

    // 航模通道限幅（1000-2000μs标准PWM范围）
    Remoter_Ch1 = target_limit_int(Remoter_Ch1, 1000, 2000);  // 通道1：右摇杆X
    Remoter_Ch2 = target_limit_int(Remoter_Ch2, 1000, 2000);  // 通道2：左摇杆Y
    Remoter_Ch3 = target_limit_int(Remoter_Ch3, 1000, 2000);  // 通道3：右摇杆Y
    Remoter_Ch4 = target_limit_int(Remoter_Ch4, 1000, 2000);  // 通道4：左摇杆X

    // 通道数据预处理（1500为中心零点）
    LX = Remoter_Ch2 - 1500;       // 左摇杆Y -> 前后运动
    LY = Remoter_Ch4 - 1500;       // 左摇杆X -> 左右移动/转向
    RX = Remoter_Ch3 - 1500;       // 右摇杆Y -> 油门调整
    RY = Remoter_Ch1 - 1500;       // 右摇杆X -> 自转控制

    // 死区处理（消除中立点漂移）
    if(abs(LX)<Threshold) LX=0;
    if(abs(LY)<Threshold) LY=0;
    if(abs(RX)<Threshold) RX=0;
    if(abs(RY)<Threshold) RY=0;

    // 动态速度调整（右摇杆Y轴作为油门）
    Remote_RCvelocity = RC_Velocity + RX;  // 基础速度±油门调整
    Remote_RCvelocity = (Remote_RCvelocity < 0) ? 0 : Remote_RCvelocity;

    // 运动量计算
    Move_X = LX * Remote_RCvelocity / 500;    // 前后运动分量
    Move_Y = -LY * Remote_RCvelocity / 500;   // 横向运动分量（全向轮）
    Move_Z = -RY * (PI/2) / 500;             // 自转角速度（rad/s）

    // 车型相关处理
    if(Car_Mode == Mec_Car) {
        Move_Z *= Remote_RCvelocity / 500;    // 全向轮角速度联动
    } else if(Car_Mode==Tank_Car || Car_Mode==FourWheel_Car) {
        if(Move_X < 0) Move_Z = -Move_Z;     // 倒车差速补偿
        Move_Z *= Remote_RCvelocity / 500;    // 角速度比例控制
    }

    // 单位转换（与PS2控制保持一致）
    Move_X /= 1000;  // mm/s -> m/s
    Move_Y /= 1000;
    Move_Z = Move_Z;

    // 新手保护机制（上电后1秒内锁定）
    if(thrice > 0) {
        Move_X=0; Move_Z=0;  // 禁止运动
        thrice--;            // 计数器递减（100*10ms=1s）
    }

    // 执行运动控制
    Drive_Motor(Move_X, Move_Y, Move_Z);
}
/**************************************************************************
【函数名称】Key
【功能概述】用户按键处理及陀螺仪校准
            - 检测双击事件更新陀螺仪零点
            - 保存当前传感器原始数据作为偏差基准
【输入参数】无
【输出参数】无
【关键变量】click_N_Double_MPU6050: 双击检测函数
           Deviation_gyro/accel: 校准基准值存储数组
**************************************************************************/
void Key(void)
{	
    u8 tmp;
    // 检测50ms时间窗口内的单击/双击（返回1-单击，2-双击）
    tmp = click_N_Double_MPU6050(50); 
    
    // 双击触发校准：保存当前陀螺仪和加速度计原始值作为基准
    if(tmp == 2) {
        memcpy(Deviation_gyro, Original_gyro, sizeof(gyro));   // 复制三轴陀螺仪数据
        memcpy(Deviation_accel, Original_accel, sizeof(accel)); // 复制三轴加速度计数据
    }
}

/**************************************************************************
【函数名称】Get_Velocity_Form_Encoder
【功能概述】编码器数据采集及速度换算
            - 读取四路编码器原始脉冲值
            - 根据车型调整极性方向
            - 转换为国际单位制线速度（m/s）
【关键参数】Encoder_precision: 编码器每转脉冲数
           Wheel_perimeter: 轮周长（米）
           CONTROL_FREQUENCY: 控制频率（Hz）
【换算公式】速度(m/s) = (脉冲数*周长)/(精度*控制周期)
**************************************************************************/
void Get_Velocity_Form_Encoder(void)
{
    // 原始脉冲临时变量
    float Encoder_A_pr, Encoder_B_pr, Encoder_C_pr, Encoder_D_pr; 
    
    // 读取编码器原始值（硬件接口2-5对应电机A-D）
    OriginalEncoder.A = -Read_Encoder(2);  // 电机A（极性反转）
    OriginalEncoder.B = -Read_Encoder(3);  // 电机B
    OriginalEncoder.C = -Read_Encoder(4);  // 电机C
    OriginalEncoder.D = -Read_Encoder(5);  // 电机D

    // 根据车型适配编码器极性
    switch(Car_Mode) {
        case Mec_Car:       // 麦克纳姆轮车型
            Encoder_A_pr = OriginalEncoder.A; 
            Encoder_B_pr = OriginalEncoder.B;
            Encoder_C_pr = -OriginalEncoder.C;  // C电机极性反转
            Encoder_D_pr = -OriginalEncoder.D;  // D电机极性反转
            break;
        case FourWheel_Car: // 四驱车型（同麦克纳姆轮）
            Encoder_A_pr = OriginalEncoder.A;
            Encoder_B_pr = OriginalEncoder.B;
            Encoder_C_pr = -OriginalEncoder.C;
            Encoder_D_pr = -OriginalEncoder.D;
            break;
        case Tank_Car:      // 履带车型
            Encoder_A_pr = OriginalEncoder.A;
            Encoder_B_pr = -OriginalEncoder.B;  // B电机极性反转
            Encoder_C_pr = OriginalEncoder.C;
            Encoder_D_pr = OriginalEncoder.D;
            break;
    }
    
    // 速度换算（考虑控制频率的实时性）
    MOTOR_A.Encoder = Encoder_A_pr * CONTROL_FREQUENCY * Wheel_perimeter / Encoder_precision;
    MOTOR_B.Encoder = Encoder_B_pr * CONTROL_FREQUENCY * Wheel_perimeter / Encoder_precision;
    MOTOR_C.Encoder = Encoder_C_pr * CONTROL_FREQUENCY * Wheel_perimeter / Encoder_precision;
    MOTOR_D.Encoder = Encoder_D_pr * CONTROL_FREQUENCY * Wheel_perimeter / Encoder_precision;
}

/**************************************************************************
【函数名称】Smooth_control
【功能概述】运动速度渐进控制
            - 实现速度的线性渐变和惯性衰减
            - 防止控制量突变导致机械冲击
【控制策略】非零目标：步进式逼近 (±0.01m/s?)
           零目标：指数衰减 (0.9衰减系数)
【限幅方式】动态限幅（不超过目标绝对值）
**************************************************************************/
void Smooth_control(float vx, float vy, float vz)
{
    float step = 0.01;  // 速度渐变步长（m/s?）
    
    // X轴处理
    if(vx > 0)       smooth_control.VX += step;
    else if(vx < 0)  smooth_control.VX -= step;
    else if(vx == 0) smooth_control.VX *= 0.9f;  // 惯性衰减
    
    // Y轴处理
    if(vy > 0)       smooth_control.VY += step;
    else if(vy < 0)  smooth_control.VY -= step;
    else if(vy == 0) smooth_control.VY *= 0.9f;
    
    // Z轴处理
    if(vz > 0)       smooth_control.VZ += step;
    else if(vz < 0)  smooth_control.VZ -= step;
    else if(vz == 0) smooth_control.VZ *= 0.9f;
    
    // 动态限幅（保持目标速度范围内）
    smooth_control.VX = target_limit_float(smooth_control.VX, -float_abs(vx), float_abs(vx));
    smooth_control.VY = target_limit_float(smooth_control.VY, -float_abs(vy), float_abs(vy));
    smooth_control.VZ = target_limit_float(smooth_control.VZ, -float_abs(vz), float_abs(vz));
}

/**************************************************************************
【函数名称】float_abs
【功能概述】浮点数绝对值计算
            - 替代标准库函数实现
            - 避免链接库依赖
【输入参数】float: 待处理浮点数
【返回参数】输入值的绝对值
**************************************************************************/
float float_abs(float insert)
{
    return (insert >= 0) ? insert : -insert;  // 三元条件运算
}

/**************************************************************************
【函数名称】robot_mode_check（已停用）
【历史功能】电机异常运转保护
            - 检测超过2500的PWM异常输出
            - 连续6次异常触发急停保护
【当前状态】已通过其他安全机制替代，保留代码供参考
**************************************************************************/
void robot_mode_check(void)
{
    static u8 error = 0;  // 错误计数（静态保持）
    
    // 检测四路PWM异常输出
    if(abs(MOTOR_A.Motor_Pwm)>2500 || 
       abs(MOTOR_B.Motor_Pwm)>2500 ||
       abs(MOTOR_C.Motor_Pwm)>2500 ||
       abs(MOTOR_D.Motor_Pwm)>2500) {
        error++;  // 错误计数累加
    }
    
    // 连续6次异常触发保护
    if(error > 6) {
        Flag_Stop = 1;              // 急停标志置位
        robot_mode_check_flag = 1;  // 模式检查标志
    }  
}
/**************************************************************************
【函数名称】SERVO_PWM_VALUE
【功能概述】机械臂关节角度-PWM转换核心函数
            - 将弧度值转换为舵机控制PWM信号
            - 支持舵机中位点校准和行程比例调节
【输入参数】angle: 目标关节角度（弧度制）
【返回参数】int: 舵机控制PWM值
【换算公式】PWM = 中位值 - (角度×比例系数×1000)
**************************************************************************/
int SERVO_PWM_VALUE(float angle)
{
    int K = 1000;           // 比例放大系数（将浮点弧度转换为整型PWM）
    float Ratio = 0.6369;   // 弧度-PWM比例系数（≈2000PWM/3.14rad）
    int pwm_value;
    
    // 核心转换公式：PWM = 中位值 - 角度×比例系数 
    // SERVO_INIT为舵机中位PWM（通常1500）
    pwm_value = (SERVO_INIT - angle * K * Ratio); 
    
    return pwm_value;  // 返回计算后的PWM信号值
}

/**************************************************************************
【函数名称】moveit_angle_limit
【功能概述】机械臂关节角度安全限幅
            - 防止关节超限造成机械损伤
            - 各关节独立设置运动范围
【限制范围】关节1-3：±1.57rad（±90度）
           关节4：±0.7rad（±40度）
**************************************************************************/
void moveit_angle_limit(void)
{ 
    // 使用通用限幅函数约束关节角度
    Moveit_Angle1 = target_limit_float(Moveit_Angle1, -1.57, 1.57);  // 关节1
    Moveit_Angle2 = target_limit_float(Moveit_Angle2, -1.57, 1.57);  // 关节2
    Moveit_Angle3 = target_limit_float(Moveit_Angle3, -1.57, 1.57);  // 关节3
    Moveit_Angle4 = target_limit_float(Moveit_Angle4, -0.7, 0.7);    // 关节4（较小行程）
}

/**************************************************************************
【函数名称】moveit_pwm_limit
【功能概述】舵机PWM输出安全限幅
            - 防止PWM超范围损坏舵机
            - 适配不同型号舵机参数
【限制范围】关节1-3：400-2600μs（标准舵机脉宽）
           关节4：900-2100μs（特殊舵机或安全范围）
**************************************************************************/
void moveit_pwm_limit(void)
{ 
    // 关节1-3使用标准舵机范围
    Moveit_PWM1 = target_limit_int(Moveit_PWM1, 400, 2600);  // PWM脉宽下限400μs，上限2600μs
    Moveit_PWM2 = target_limit_int(Moveit_PWM2, 400, 2600);
    Moveit_PWM3 = target_limit_int(Moveit_PWM3, 400, 2600);
    
    // 关节4特殊限制（可能用于夹爪等精密控制）
    Moveit_PWM4 = target_limit_int(Moveit_PWM4, 900, 2100);  // 缩小控制范围确保安全
}

/**************************************************************************
【函数名称】Drive_Robot_Arm
【功能概述】机械臂运动控制主函数
            - 实现从角度到PWM的完整转换流程
            - 集成位置式PID闭环控制
            - 速度积分生成位置指令
【控制流程】角度限幅→PWM转换→PWM限幅→PID计算→位置更新
**************************************************************************/
void Drive_Robot_Arm(void)
{
    // 步骤1：关节角度安全限幅
    moveit_angle_limit();		 
    
    // 步骤2：角度转PWM并叠加初始偏移（校准机械误差）
    Moveit_PWM1 = SERVO_PWM_VALUE(Moveit_Angle1) + Moveit_Angle1_init;  // 关节1
    Moveit_PWM2 = SERVO_PWM_VALUE(Moveit_Angle2) + Moveit_Angle2_init;  // 关节2
    Moveit_PWM3 = SERVO_PWM_VALUE(Moveit_Angle3) + Moveit_Angle3_init;  // 关节3
    Moveit_PWM4 = SERVO_PWM_VALUE(Moveit_Angle4) + Moveit_Angle4_init;  // 关节4
    
    // 步骤3：PWM输出限幅
    moveit_pwm_limit();
    
    // 步骤4：位置式PID计算控制量（目标PWM vs 当前位置）
    Velocity1 = Position_PID1(Position1, Moveit_PWM1);  // 关节1速度指令
    Velocity2 = Position_PID2(Position2, Moveit_PWM2);  // 关节2
    Velocity3 = Position_PID3(Position3, Moveit_PWM3);  // 关节3
    Velocity4 = Position_PID4(Position4, Moveit_PWM4);  // 关节4
    
    // 步骤5：速度积分得到位置（模拟位置闭环）
    Position1 += Velocity1;  // 关节1位置更新		   
    Position2 += Velocity2;  // 关节2
    Position3 += Velocity3;  // 关节3
    Position4 += Velocity4;  // 关节4
}


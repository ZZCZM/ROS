/**
  ******************************************************************************
  * @file    MPU6050.c
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
#include "MPU6050.h"
#include "I2C.h"
#include "usart.h"
/*********************** MPU6050传感器配置宏 **************************/
/* 数据输出控制标志位 */
#define PRINT_ACCEL     (0x01)  // 0b00000001 启用加速度计数据打印
#define PRINT_GYRO      (0x02)  // 0b00000010 启用陀螺仪数据打印
#define PRINT_QUAT      (0x04)  // 0b00000100 启用四元数数据打印

/* 传感器启用标志 */
#define ACCEL_ON        (0x01)  // 加速度计使能位（对应寄存器配置）
#define GYRO_ON         (0x02)  // 陀螺仪使能位（对应寄存器配置）

/* 运动检测状态 */
#define MOTION          (0)     // 运动状态标识
#define NO_MOTION       (1)     // 静止状态标识

/* 默认采样率配置 */
#define DEFAULT_MPU_HZ  (200)   // 默认采样频率200Hz

/* Flash存储相关 */
#define FLASH_SIZE      (512)   // 分配用于存储校准数据的Flash容量（字节）
#define FLASH_MEM_START ((void*)0x1800)  // Flash校准数据起始地址（STM32特定地址）

/* 四元数计算常量 */
#define q30  1073741824.0f      // 2^30，用于Q30定点数格式转换

/*********************** 传感器原始数据存储 **************************/
short gyro[3];          // 陀螺仪三轴原始数据（X,Y,Z） 单位：dps
short accel[3];         // 加速度计三轴原始数据（X,Y,Z）单位：mg
unsigned char sensors;  // 传感器状态标志位（位0:ACCEL_ON, 位1:GYRO_ON）

/*********************** 校准相关变量 *******************************/
int Deviation_Count;    // 零点校准采样计数器

/* 陀螺仪校准数据 */
int Deviation_sum_gyro[3];  // 陀螺仪三轴误差累加器（用于计算平均值）
short Deviation_gyro[3];    // 陀螺仪三轴静态误差补偿值（最终校准值）
short Original_gyro[3];      // 校准前的原始零点偏移量备份

/* 加速度校准数据 */ 
int Deviation_sum_accel[3]; // 加速度计三轴误差累加器
short Deviation_accel[3];    // 加速度计三轴静态误差补偿值
short Original_accel[3];      // 校准前原始偏移量备份

/*********************** 姿态解算变量 *******************************/
float q0=1.0f, q1=0.0f, q2=0.0f, q3=0.0f;  // 四元数（q0为标量部分）

/* 外部系统状态标志 */
extern uint8_t isReadyForStandby;  // 来自其他模块的低功耗就绪标志


/**
 * @brief MPU6050传感器数据采集与校准任务
 * @param pvParameters FreeRTOS任务参数（未使用）
 * @note 功能说明：
 * 1. 以100Hz频率周期性执行
 * 2. 执行传感器零点校准（前CONTROL_DELAY个周期）
 * 3. 实时获取陀螺仪/加速度计原始数据
 * 校准策略：
 * - 使用CONTROL_DELAY-20至CONTROL_DELAY周期的20组数据计算静态偏移
 * - 校准完成后持续输出补偿后的传感器数据
 */
void MPU6050_task(void *pvParameters)
{
    u32 lastWakeTime = getSysTickCnt(); // 获取系统节拍计数器初始值
    
    while(1)
    {    
        /*----- 任务周期控制 -----*/
        // FreeRTOS精确周期延迟（100Hz对应10ms周期）
        vTaskDelayUntil(&lastWakeTime, F2T(RATE_100_HZ)); 
        
        /*----- 零点校准阶段 -----*/
        if(Deviation_Count < CONTROL_DELAY) 
        {     
            /* 最后20个采样周期进行数据采集 */
            if(Deviation_Count >= (CONTROL_DELAY - 20))
            {
                // 累加陀螺仪三轴原始数据
                Deviation_sum_gyro[0] += (int)gyro[0]; // X轴
                Deviation_sum_gyro[1] += (int)gyro[1]; // Y轴
                Deviation_sum_gyro[2] += (int)gyro[2]; // Z轴
                
                // 累加加速度计三轴原始数据
                Deviation_sum_accel[0] += (int)accel[0]; // X轴
                Deviation_sum_accel[1] += (int)accel[1]; // Y轴 
                Deviation_sum_accel[2] += (int)accel[2]; // Z轴
            }
            
            Deviation_Count++; // 校准计数器递增
            
            /* 校准完成时计算最终偏移量 */
            if(Deviation_Count >= CONTROL_DELAY)
            {
                // 计算陀螺仪三轴平均偏移（Q15定点数）
                Deviation_gyro[0] = (short int)(Deviation_sum_gyro[0] / 20); // X轴
                Deviation_gyro[1] = (short int)(Deviation_sum_gyro[1] / 20); // Y轴
                Deviation_gyro[2] = (short int)(Deviation_sum_gyro[2] / 20); // Z轴
                
                // 计算加速度计三轴平均偏移
                Deviation_accel[0] = (short int)(Deviation_sum_accel[0] / 20); // X轴
                Deviation_accel[1] = (short int)(Deviation_sum_accel[1] / 20); // Y轴
                Deviation_accel[2] = (short int)(Deviation_sum_accel[2] / 20); // Z轴
            }
        }
        
        /*----- 常规数据采集 -----*/
        MPU_Get_Gyroscope();    // 读取陀螺仪数据到gyro数组（已补偿偏移）
        MPU_Get_Accelscope();   // 读取加速度计原始数据到accel数组
    }
}

/************************** 全局数据区 ***************************/
uint8_t buffer[14];            // I2C通信缓冲区（14字节对应传感器数据长度）

int16_t MPU6050_FIFO[6][11];   // 传感器数据FIFO队列（6轴×11组历史数据）

/* 陀螺仪零点偏移补偿值 */
int16_t Gx_offset = 0;         // X轴补偿值（单位：LSB）
int16_t Gy_offset = 0;         // Y轴补偿值
int16_t Gz_offset = 0;         // Z轴补偿值



/**************************** 传感器数据处理函数 **************************/
/**
 * @brief 更新传感器数据到FIFO队列并进行滑动平均滤波
 * @param ax X轴加速度原始值
 * @param ay Y轴加速度原始值
 * @param az Z轴加速度原始值
 * @param gx X轴陀螺仪原始值
 * @param gy Y轴陀螺仪原始值
 * @param gz Z轴陀螺仪原始值
 * @note FIFO队列结构：
 *   - 维度0-2: 加速度X/Y/Z轴历史数据（10组）
 *   - 维度3-5: 陀螺仪X/Y/Z轴历史数据（10组）
 *   - 每维第10个元素存储滤波后的平均值
 */
void MPU6050_newValues(int16_t ax, int16_t ay, int16_t az, 
                      int16_t gx, int16_t gy, int16_t gz)
{
    unsigned char i;
    int32_t sum = 0;

    /*----- FIFO队列更新 -----*/
    // 所有维度向前移位（丢弃最旧数据）
    for(i = 1; i < 10; i++) {  
        MPU6050_FIFO[0][i-1] = MPU6050_FIFO[0][i];  // 加速度X轴
        MPU6050_FIFO[1][i-1] = MPU6050_FIFO[1][i];  // 加速度Y轴
        MPU6050_FIFO[2][i-1] = MPU6050_FIFO[2][i];  // 加速度Z轴
        MPU6050_FIFO[3][i-1] = MPU6050_FIFO[3][i];  // 陀螺仪X轴
        MPU6050_FIFO[4][i-1] = MPU6050_FIFO[4][i];  // 陀螺仪Y轴
        MPU6050_FIFO[5][i-1] = MPU6050_FIFO[5][i];  // 陀螺仪Z轴
    }

    // 插入最新数据到队列末尾
    MPU6050_FIFO[0][9] = ax;  // 加速度X轴新数据
    MPU6050_FIFO[1][9] = ay;  // 加速度Y轴
    MPU6050_FIFO[2][9] = az;  // 加速度Z轴
    MPU6050_FIFO[3][9] = gx;  // 陀螺仪X轴
    MPU6050_FIFO[4][9] = gy;  // 陀螺仪Y轴
    MPU6050_FIFO[5][9] = gz;  // 陀螺仪Z轴

    /*----- 滑动平均计算 -----*/
    // 加速度X轴滤波（维度0）
    sum = 0;
    for(i = 0; i < 10; i++) {
        sum += MPU6050_FIFO[0][i];  // 累加最近10组数据
    }
    MPU6050_FIFO[0][10] = sum / 10;  // 存储平均值到第10位置

    // 加速度Y轴滤波（维度1）
    sum = 0;
    for(i = 0; i < 10; i++) {
        sum += MPU6050_FIFO[1][i];
    }
    MPU6050_FIFO[1][10] = sum / 10;

    // 加速度Z轴滤波（维度2）
    sum = 0;
    for(i = 0; i < 10; i++) {
        sum += MPU6050_FIFO[2][i];
    }
    MPU6050_FIFO[2][10] = sum / 10;

    // 陀螺仪X轴滤波（维度3）
    sum = 0;
    for(i = 0; i < 10; i++) {
        sum += MPU6050_FIFO[3][i];
    }
    MPU6050_FIFO[3][10] = sum / 10;

    // 陀螺仪Y轴滤波（维度4）
    sum = 0;
    for(i = 0; i < 10; i++) {
        sum += MPU6050_FIFO[4][i];
    }
    MPU6050_FIFO[4][10] = sum / 10;

    // 陀螺仪Z轴滤波（维度5）
    sum = 0;
    for(i = 0; i < 10; i++) {
        sum += MPU6050_FIFO[5][i];
    }
    MPU6050_FIFO[5][10] = sum / 10;
}

/**************************** 传感器配置函数 **************************/
/**
 * @brief 设置MPU6050时钟源
 * @param source 时钟源编号：
 *   0-内部RC振荡器  1-X轴陀螺参考PLL 
 *   2-Y轴陀螺参考PLL 3-Z轴陀螺参考PLL 
 *   4-外部32.768kHz参考 5-外部19.2MHz参考
 *   7-停止时钟并复位时序发生器
 * @note 影响功耗和精度，通常选择陀螺参考PLL(1-3)
 */
void MPU6050_setClockSource(uint8_t source) {
    // 写PWR_MGMT_1寄存器CLKSEL位域
    I2C_WriteBits(devAddr, 
                 MPU6050_RA_PWR_MGMT_1,
                 MPU6050_PWR1_CLKSEL_BIT,
                 MPU6050_PWR1_CLKSEL_LENGTH,
                 source);
}

/**
 * @brief 设置陀螺仪满量程范围
 * @param range 量程选项：
 *   0-±250dps  1-±500dps 
 *   2-±1000dps 3-±2000dps
 * @note 值越大噪声越高但动态范围越大
 */
void MPU6050_setFullScaleGyroRange(uint8_t range) {
    // 写GYRO_CONFIG寄存器FS_SEL位域
    I2C_WriteBits(devAddr,
                 MPU6050_RA_GYRO_CONFIG,
                 MPU6050_GCONFIG_FS_SEL_BIT,
                 MPU6050_GCONFIG_FS_SEL_LENGTH,
                 range);
}
/**************************** 加速度计量程配置 **************************/
/**
 * @brief 设置加速度计满量程范围
 * @param range 量程选项：
 *   0-±2g  1-±4g 
 *   2-±8g 3-±16g
 * @note 量程越大，分辨率越低但动态范围越大
 */
void MPU6050_setFullScaleAccelRange(uint8_t range) {
    // 写ACCEL_CONFIG寄存器AFS_SEL位域（bit4-3）
    I2C_WriteBits(devAddr, 
                 MPU6050_RA_ACCEL_CONFIG,
                 MPU6050_ACONFIG_AFS_SEL_BIT,  // 起始位=4
                 MPU6050_ACONFIG_AFS_SEL_LENGTH,// 位长度=2
                 range);
}

/**************************** 电源管理模式配置 **************************/
/**
 * @brief 设置睡眠模式使能状态
 * @param enabled 1:进入睡眠模式 0:正常工作模式
 * @note 睡眠模式下功耗降低至5μA，但所有传感器停止工作
 */
void MPU6050_setSleepEnabled(uint8_t enabled) {
    // 写PWR_MGMT_1寄存器SLEEP位（bit6）
    I2C_WriteOneBit(devAddr,
                  MPU6050_RA_PWR_MGMT_1,
                  MPU6050_PWR1_SLEEP_BIT,
                  enabled);
}

/**************************** 设备识别与检测 **************************/
/**
 * @brief 读取设备身份标识寄存器
 * @retval 0x68（MPU6050固定返回值）
 * @note WHO_AM_I寄存器地址：0x75，复位值0x68
 */
uint8_t MPU6050_getDeviceID(void) {
    // 直接读取WHO_AM_I寄存器值
    return I2C_ReadOneByte(devAddr, MPU6050_RA_WHO_AM_I);
}

/**
 * @brief 检测MPU6050物理连接状态
 * @retval 1:设备正常连接 0:设备未响应
 * @note 通过验证WHO_AM_I寄存器值判断连接性
 */
uint8_t MPU6050_testConnection(void) {
    return (MPU6050_getDeviceID() == 0x68) ? 1 : 0;  // 0x68=104（十进制）
}

/**************************** I2C扩展接口配置 **************************/
/**
 * @brief 启用/禁用I2C主控制器模式
 * @param enabled 1:启用主模式 0:禁用
 * @note 主模式下MPU6050可控制外部I2C设备（如磁力计）
 */
void MPU6050_setI2CMasterModeEnabled(uint8_t enabled) {
    // 写USER_CTRL寄存器I2C_MST_EN位（bit5）
    I2C_WriteOneBit(devAddr,
                   MPU6050_RA_USER_CTRL,
                   MPU6050_USERCTRL_I2C_MST_EN_BIT,
                   enabled);
}

/**
 * @brief 启用I2C旁路模式
 * @param enabled 1:启用 0:禁用
 * @note 旁路模式下主机可直接访问MPU6050的AUX_I2C接口
 */
void MPU6050_setI2CBypassEnabled(uint8_t enabled) {
    // 写INT_PIN_CFG寄存器I2C_BYPASS_EN位（bit1）
    I2C_WriteOneBit(devAddr,
                   MPU6050_RA_INT_PIN_CFG,
                   MPU6050_INTCFG_I2C_BYPASS_EN_BIT,
                   enabled);
}
/**************************** MPU6050初始化函数 **************************/
/**
 * @brief 初始化MPU6050传感器进入工作状态
 * @return 0-成功 1-设备ID校验失败
 * @note 执行流程：
 * 1. 硬件复位 → 2. 基础配置 → 3. 采样率设置 → 4. 设备验证
 */
u8 MPU6050_initialize(void) 
{
    u8 res;
    /*----- 硬件复位 -----*/
    I2C_WriteOneByte(devAddr, MPU6050_RA_PWR_MGMT_1, 0X80); // 发送复位命令（bit7=1）
    delay_ms(200);  // 等待复位完成（数据手册要求至少100ms）
    
    /*----- 基础功能配置 -----*/
    I2C_WriteOneByte(devAddr, MPU6050_RA_PWR_MGMT_1, 0X00); // 退出睡眠模式
    
    // 传感器量程配置
    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_500);   // 陀螺仪±500dps
    MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);    // 加速度计±2g
    
    /*----- 接口配置 -----*/
    I2C_WriteOneByte(devAddr, MPU6050_RA_INT_ENABLE, 0X00);  // 禁用所有中断
    I2C_WriteOneByte(devAddr, MPU6050_RA_USER_CTRL, 0X00);   // 禁用I2C主模式
    I2C_WriteOneByte(devAddr, MPU6050_RA_FIFO_EN, 0X00);     // 关闭FIFO
    
    /*----- 旁路模式配置 -----*/
    I2C_WriteOneByte(devAddr, MPU6050_RA_INT_PIN_CFG, 0X80); // 启用旁路模式（bit7=1）
    
    /*----- 设备验证 -----*/
    res = I2C_ReadOneByte(devAddr, MPU6050_RA_WHO_AM_I);    // 读取设备ID
    if(res == MPU6050_DEFAULT_ADDRESS) {                    // 验证ID是否为0x68
        // 时钟与电源配置
        I2C_WriteOneByte(devAddr, MPU6050_RA_PWR_MGMT_1, 0X01); // 使用X轴陀螺PLL作为时钟源
        I2C_WriteOneByte(devAddr, MPU6050_RA_PWR_MGMT_2, 0X00); // 启用所有传感器
        MPU6050_Set_Rate(50);                                 // 设置50Hz采样率
        return 0;
    } else {
        return 1; // ID校验失败
    }
}

/**************************** 温度传感器接口 **************************/
/**
 * @brief 读取芯片内部温度传感器数值
 * @return 温度值（单位：0.1℃）
 * @note 计算公式：
 * 温度(℃) = (TEMP_OUT / 340) + 36.53
 */
int Read_Temperature(void)
{
    float Temp;
    // 读取温度寄存器高低字节（16位有符号数）
    Temp = (I2C_ReadOneByte(devAddr, MPU6050_RA_TEMP_OUT_H) << 8) + 
            I2C_ReadOneByte(devAddr, MPU6050_RA_TEMP_OUT_L);
    
    /* 数据格式转换 */
    if(Temp > 32768) Temp -= 65536; // 处理负数
    
    /* 温度转换公式 */
    Temp = (36.53f + Temp / 340) * 10; // 放大10倍存储（保留1位小数）
    
    return (int)Temp; // 返回整型值如365表示36.5℃
}

/**************************** 滤波器配置函数 **************************/
/**
 * @brief 设置数字低通滤波器截止频率
 * @param lpf 截止频率（Hz）有效值：5~188
 * @return 0-成功 其他-I2C错误码
 * @note 实际设置值会匹配最近的预设档位
 */
unsigned char MPU6050_Set_LPF(u16 lpf)
{
    u8 data = 0;
    /* 频率-配置值映射 */
    if(lpf >= 188)       data = 1;   // 对应188Hz
    else if(lpf >= 98)  data = 2;   // 98Hz
    else if(lpf >= 42)  data = 3;   // 42Hz
    else if(lpf >= 20)  data = 4;   // 20Hz
    else if(lpf >= 10)  data = 5;   // 10Hz
    else                data = 6;   // 5Hz
    
    // 写入CONFIG寄存器（地址0x1A）
    return I2C_WriteOneByte(devAddr, MPU6050_RA_CONFIG, data); 
}

/**
 * @brief 设置采样率并自动配置低通滤波器
 * @param rate 采样率（4-1000Hz）
 * @return 0-成功 其他-I2C错误码
 * @note 实际采样率 = 1kHz / (1 + SMPLRT_DIV)
 */
unsigned char MPU6050_Set_Rate(u16 rate)
{
    u8 data;
    /* 参数边界检查 */
    rate = (rate > 1000) ? 1000 : ((rate < 4) ? 4 : rate);
    
    /* 计算分频系数 */
    data = 1000 / rate - 1; // SMPLRT_DIV寄存器值
    
    /* 配置采样率 */
    I2C_WriteOneByte(devAddr, MPU6050_RA_SMPLRT_DIV, data); 
    
    /* 自动设置低通滤波器为采样率的一半 */
    return MPU6050_Set_LPF(rate / 2); 
}
/**************************** 陀螺仪数据获取 **************************/
/**
 * @brief 获取并校准陀螺仪三轴数据
 * @note 功能说明：
 * - 实现陀螺仪原始数据采集
 * - 处理设备启动阶段的初始化过程
 * - 执行零点漂移校准
 * @note 执行流程：
 * 1. 读取原始数据 → 2. 启动阶段处理 → 3. 零点校准
 * @note 全局变量影响：
 * - gyro[]: 存储最终校准后的陀螺仪数据
 * - Original_gyro[]: 保存原始测量值用于调试
 * - Deviation_gyro[]: 存储校准偏移量
 * - Deviation_Count: 系统运行时间计数器
 * - Flag_Stop: 系统运动控制使能标志
 * - Led_Count: LED闪烁频率控制
 * @note 依赖关系：
 * - 需配合I2C_ReadOneByte()函数进行寄存器读取
 * - 需预先设置好devAddr设备地址
 * - 依赖CONTROL_DELAY常量定义初始化时长
 */
void MPU_Get_Gyroscope(void)
{
    /*----- 读取原始数据 -----*/
    // 通过组合高8位和低8位数据构建16位有符号整型
    // X轴数据采集（寄存器地址MPU6050_RA_GYRO_XOUT_H为陀螺仪X轴高字节寄存器）
    gyro[0] = (I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_XOUT_H) << 8) | 
               I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_XOUT_L);  // 低字节数据直接拼接
    
    // Y轴数据采集（使用相同方式获取Y轴测量值）
    gyro[1] = (I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_YOUT_H) << 8) | 
               I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_YOUT_L);  // 寄存器地址递增获取各轴数据
    
    // Z轴数据采集（陀螺仪Z轴数据寄存器操作）
    gyro[2] = (I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_ZOUT_H) << 8) | 
               I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_ZOUT_L);  // 完成三轴数据采集

    /*----- 启动初期处理（前10秒）-----*/
    // 系统初始化阶段处理（假设CONTROL_DELAY对应10秒计数）
    if(Deviation_Count < CONTROL_DELAY) {
        // 设置LED快速闪烁模式（Led_Fast_Count应为快速闪烁的时间参数）
        Led_Count = Led_Fast_Count;   // 通过LED状态指示系统初始化中
        
        // 锁定系统控制（防止在未完成校准时执行运动控制）
        Flag_Stop = 1;                // 设置停止标志，停止PID计算或电机驱动
    } 
    /*----- 正常操作阶段 -----*/
    else {
        // 当计数器正好达到阈值时的单次操作（保证只执行一次模式切换）
        if(Deviation_Count == CONTROL_DELAY) {
            Flag_Stop = 0;            // 解除控制锁定，允许系统进行运动控制
            Led_Count = Led_Init_Count; // 恢复正常LED指示频率（如1Hz呼吸灯）
            Deviation_Count++;        // 递增计数器避免重复进入此条件
        }
        
        /*----- 零点校准处理 -----*/
        // 原始数据备份（用于后期调试或校准验证）
        Original_gyro[0] = gyro[0];  // 保存X轴原始测量值
        Original_gyro[1] = gyro[1];  // 保存Y轴原始测量值
        Original_gyro[2] = gyro[2];  // 保存Z轴原始测量值
        
        // 应用校准补偿（Deviation_gyro[]应包含预先计算好的零点偏移量）
        gyro[0] = Original_gyro[0] - Deviation_gyro[0]; // X轴偏移补偿（单位：LSB）
        gyro[1] = Original_gyro[1] - Deviation_gyro[1]; // Y轴偏移补偿（消除静止状态零点误差）
        gyro[2] = Original_gyro[2] - Deviation_gyro[2]; // Z轴偏移补偿（基于启动阶段的采样均值）
    }
}

/**
 * @brief 获取校准后的加速度计三轴数据
 * @note 执行流程：
 * 1. 读取原始数据 → 2. 启动阶段处理 → 3. 零点校准
 * 校准策略：前10秒为校准阶段，后续数据应用补偿值
 */
void MPU_Get_Accelscope(void)
{
    /*----- 读取原始加速度数据 -----*/
    // X轴加速度（16位有符号数，范围±32768对应配置量程）
    accel[0] = (I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_XOUT_H) << 8) | 
                I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_XOUT_L);
    
    // Y轴加速度（寄存器地址0x3D-0x3E）
    accel[1] = (I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_YOUT_H) << 8) | 
                I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_YOUT_L);
    
    // Z轴加速度（寄存器地址0x3F-0x40）
    accel[2] = (I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_ZOUT_H) << 8) | 
                I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_ZOUT_L);

    /*----- 启动初期处理（前10秒校准阶段）-----*/
    if(Deviation_Count < CONTROL_DELAY) {
        // 此阶段不进行校准应用（仅数据采集）
    }
    /*----- 正常操作阶段 -----*/
    else {
        /*----- 原始数据保存（用于用户手动校准）-----*/
        Original_accel[0] = accel[0];  // 保存X轴原始数据
        Original_accel[1] = accel[1];  // Y轴
        Original_accel[2] = accel[2];   // Z轴
        
        /*----- 零点漂移补偿 -----*/
        accel[0] = Original_accel[0] - Deviation_accel[0];  // X轴补偿
        accel[1] = Original_accel[1] - Deviation_accel[1];  // Y轴
        // Z轴特殊处理：补偿值+16384（1g对应的LSB值，假设量程为±2g）
        accel[2] = Original_accel[2] - Deviation_accel[2] + 16384; 
    }
}


//------------------End of File----------------------------

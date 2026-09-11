/**
  ******************************************************************************
  * @file    stm32f4xx_can.h
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   This file contains all the functions prototypes for the CAN firmware 
  *          library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_CAN_H
#define __STM32F4xx_CAN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup CAN
  * @{
  */
/*----------------------------- 导出类型定义 -----------------------------*/

/* CAN外设有效性检查宏 */
  // 验证CAN外设实例
#define IS_CAN_ALL_PERIPH(PERIPH) (((PERIPH) == CAN1) || \
                                   ((PERIPH) == CAN2))

/* CAN总线标识符类型重定义 */
#define CAN_ID CAN2  // 用于兼容旧版本代码的宏定义

/**
  * @brief  CAN初始化结构体定义
  * @note   用于配置CAN总线通信的核心参数
  */
typedef struct
{
    /* 时钟预分频器配置（决定时间量子的长度）
       - 范围：1-1024
       - 实际波特率 = APB1时钟 / (Prescaler * (BS1 + BS2 + 1)) */
    uint16_t CAN_Prescaler;   

    /* CAN工作模式选择
       - 可选：正常模式/静默模式/回环模式等
       - 详见@ref CAN_operating_mode 枚举定义 */
    uint8_t CAN_Mode;         

    /* 同步跳转宽度配置（SJW）
       - 范围：1-4个时间量子
       - 决定总线同步时允许的最大时间量调整幅度
       - 详见@ref CAN_synchronisation_jump_width 枚举 */
    uint8_t CAN_SJW;          

    /* 时间段1配置（BS1）
       - 范围：1-16个时间量子
       - 包含传播段和相位缓冲段1
       - 详见@ref CAN_time_quantum_in_bit_segment_1 枚举 */
    uint8_t CAN_BS1;          

    /* 时间段2配置（BS2）
       - 范围：1-8个时间量子
       - 包含相位缓冲段2
       - 详见@ref CAN_time_quantum_in_bit_segment_2 枚举 */
    uint8_t CAN_BS2;          

    /* 时间触发通信模式使能
       - ENABLE: 启用时间戳功能
       - DISABLE: 关闭时间戳（默认） */
    FunctionalState CAN_TTCM; 

    /* 自动离线管理使能
       - ENABLE: 总线错误超限后自动恢复
       - DISABLE: 需要软件干预恢复 */
    FunctionalState CAN_ABOM;  

    /* 自动唤醒模式使能
       - ENABLE: 检测到总线活动自动唤醒
       - DISABLE: 需要硬件唤醒信号 */
    FunctionalState CAN_AWUM;  

    /* 非自动重传模式
       - ENABLE: 发送失败不自动重传
       - DISABLE: 自动重传（默认） */
    FunctionalState CAN_NART;  

    /* 接收FIFO锁定模式
       - ENABLE: FIFO满后丢弃新报文
       - DISABLE: FIFO满后覆盖旧报文 */
    FunctionalState CAN_RFLM; 

    /* 发送FIFO优先级配置
       - ENABLE: 按报文ID优先级排序
       - DISABLE: 按提交顺序发送 */
    FunctionalState CAN_TXFP;  
} CAN_InitTypeDef;

/**
  * @brief  CAN过滤器初始化结构体定义
  * @note   用于配置报文过滤规则，最多支持28个过滤器（F4系列）
  */
typedef struct
{
    /* 过滤器标识符高16位
       - 32位模式：组成完整ID的高16位
       - 16位模式：第一个过滤ID的高16位 */
    uint16_t CAN_FilterIdHigh;     

    /* 过滤器标识符低16位
       - 32位模式：组成完整ID的低16位
       - 16位模式：第一个过滤ID的低16位 */
    uint16_t CAN_FilterIdLow;      

    /* 过滤器掩码高16位
       - 标识符掩码模式：掩码值高位
       - 标识符列表模式：第二个ID的高位 */
    uint16_t CAN_FilterMaskIdHigh; 

    /* 过滤器掩码低16位
       - 标识符掩码模式：掩码值低位
       - 标识符列表模式：第二个ID的低位 */
    uint16_t CAN_FilterMaskIdLow;   

    /* 过滤器关联的FIFO
       - 0: 分配到FIFO0
       - 1: 分配到FIFO1
       - 详见@ref CAN_filter_FIFO 枚举 */
    uint16_t CAN_FilterFIFOAssignment; 

    /* 过滤器编号
       - 范围：0-27（根据具体型号）
       - 决定过滤器的存储位置 */
    uint8_t CAN_FilterNumber;      

    /* 过滤器工作模式
       - 0: 标识符掩码模式
       - 1: 标识符列表模式
       - 详见@ref CAN_filter_mode 枚举 */
    uint8_t CAN_FilterMode;        

    /* 过滤器位宽模式
       - 0: 16位短过滤器（2个ID）
       - 1: 32位长过滤器（1个ID+掩码）
       - 详见@ref CAN_filter_scale 枚举 */
    uint8_t CAN_FilterScale;       

    /* 过滤器激活状态
       - ENABLE: 启用该过滤器
       - DISABLE: 禁用该过滤器 */
    FunctionalState CAN_FilterActivation; 
} CAN_FilterInitTypeDef;
/**
  * @brief  CAN发送报文结构体定义
  * @note   用于组织待发送的CAN数据帧，包含标识符、帧类型及数据载荷
  */
typedef struct
{
  /* 标准标识符（11位）
     - 范围：0x000-0x7FF
     - 标准帧与扩展帧二选一 */
  uint32_t StdId;  

  /* 扩展标识符（29位）
     - 范围：0x00000000-0x1FFFFFFF
     - 当IDE位设为EXTENDED_ID时生效 */
  uint32_t ExtId;  

  /* 标识符类型选择
     - 0: 标准标识符（使用StdId）
     - 1: 扩展标识符（使用ExtId）
     - 详见@ref CAN_identifier_type 枚举 */
  uint8_t IDE;     

  /* 远程传输请求标志
     - 0: 数据帧（携带数据）
     - 1: 远程帧（不携带数据，请求数据）
     - 详见@ref CAN_remote_transmission_request 枚举 */
  uint8_t RTR;     

  /* 数据长度码（实际数据字节数）
     - 范围：0-8
     - 0表示空数据帧，最大支持8字节 */
  uint8_t DLC;     

  /* 数据载荷数组
     - 每个元素范围：0x00-0xFF
     - 实际有效长度由DLC决定 */
  uint8_t Data[8]; 
} CanTxMsg;

/**
  * @brief  CAN接收报文结构体定义
  * @note   用于存储接收到的CAN数据帧及其元信息
  */
typedef struct
{
  uint32_t StdId;  // 同发送结构体标准ID定义
  uint32_t ExtId;  // 同发送结构体扩展ID定义
  uint8_t IDE;     // 同发送结构体ID类型定义
  uint8_t RTR;     // 同发送结构体RTR定义
  uint8_t DLC;     // 同发送结构体DLC定义
  uint8_t Data[8]; // 同发送结构体数据载荷定义

  /* 过滤器匹配索引号
     - 范围：0x00-0xFF
     - 指示报文通过的过滤器编号
     - 用于多过滤器配置时识别过滤源 */
  uint8_t FMI;     
} CanRxMsg;

/*---------------------- 导出常量定义 ----------------------*/

/**
  * @defgroup CAN_InitStatus CAN初始化状态码
  * @brief   标识CAN外设初始化结果
  */
#define CAN_InitStatus_Failed              ((uint8_t)0x00) /*!< 初始化失败（通常因总线参数错误） */
#define CAN_InitStatus_Success             ((uint8_t)0x01) /*!< 初始化成功 */

/* 历史兼容宏定义（不推荐在新代码中使用） */
#define CANINITFAILED    CAN_InitStatus_Failed  // 旧版本失败状态宏
#define CANINITOK        CAN_InitStatus_Success // 旧版本成功状态宏

/**
  * @}
  */

/*------------------------- CAN工作模式配置组 -------------------------*/

/**
  * @defgroup CAN_operating_mode 
  * @brief CAN总线通信模式配置选项
  * @note 定义CAN控制器的基础工作模式，影响总线收发行为
  */
#define CAN_Mode_Normal             ((uint8_t)0x00)  /*!< 正常模式：常规总线通信 */
#define CAN_Mode_LoopBack           ((uint8_t)0x01)  /*!< 环回模式：内部自发自收，用于硬件自检 */
#define CAN_Mode_Silent             ((uint8_t)0x02)  /*!< 静默模式：只监听总线，不发送数据（监控模式） */
#define CAN_Mode_Silent_LoopBack    ((uint8_t)0x03)  /*!< 静默环回模式：组合静默与环回特性，用于分析错误帧 */

/* 工作模式校验宏 */
          // 正常通信模式
                   // 环回测试模式
                             // 总线监听模式
                             // 静默诊断模式
#define IS_CAN_MODE(MODE) ( \
    ((MODE) == CAN_Mode_Normal) || \
    ((MODE) == CAN_Mode_LoopBack)|| \
    ((MODE) == CAN_Mode_Silent) || \
    ((MODE) == CAN_Mode_Silent_LoopBack)      \
)

/*----------------------- CAN操作状态模式组 -----------------------*/
/**
  * @defgroup CAN_operating_mode 
  * @brief CAN控制器运行状态模式
  * @note 反映CAN控制器的当前操作状态，通过寄存器设置切换
  */  
#define CAN_OperatingMode_Initialization  ((uint8_t)0x00) /*!< 初始化模式：配置参数时自动进入 */
#define CAN_OperatingMode_Normal          ((uint8_t)0x01) /*!< 正常运行模式：完成初始化后进入 */
#define CAN_OperatingMode_Sleep           ((uint8_t)0x02) /*!< 睡眠模式：低功耗状态，可通过总线活动唤醒 */

/* 操作模式校验宏 */
  // 初始化状态
            // 正常工作状态
                          // 低功耗睡眠状态
#define IS_CAN_OPERATING_MODE(MODE) ( \
    ((MODE) == CAN_OperatingMode_Initialization) ||\
    ((MODE) == CAN_OperatingMode_Normal)|| \
    ((MODE) == CAN_OperatingMode_Sleep) \
)

/*--------------------- 模式切换状态码组 ---------------------*/
/**
  * @defgroup CAN_operating_mode_status
  * @brief 模式切换操作结果状态码
  */
#define CAN_ModeStatus_Failed    ((uint8_t)0x00)    /*!< 模式切换失败（常见于总线参数冲突） */
#define CAN_ModeStatus_Success   ((uint8_t)!CAN_ModeStatus_Failed) /*!< 模式切换成功 */

/*------------------- 同步跳转宽度配置组 -------------------*/
/**
  * @defgroup CAN_synchronisation_jump_width 
  * @brief 同步跳转宽度(SJW)配置选项
  * @note 定义位定时调整的最大步长，影响总线时钟同步能力
  */
#define CAN_SJW_1tq    ((uint8_t)0x00)  /*!< 最大调整1个时间量子 */
#define CAN_SJW_2tq    ((uint8_t)0x01)  /*!< 最大调整2个时间量子 */
#define CAN_SJW_3tq    ((uint8_t)0x02)  /*!< 最大调整3个时间量子 */
#define CAN_SJW_4tq    ((uint8_t)0x03)  /*!< 最大调整4个时间量子（推荐高速通信） */

/* SJW参数校验宏 */
    // 1tq调整能力
         // 2tq调整能力
             // 3tq调整能力
                   // 4tq调整能力
#define IS_CAN_SJW(SJW) ( \
    ((SJW) == CAN_SJW_1tq) || \
    ((SJW) == CAN_SJW_2tq)|| \
    ((SJW) == CAN_SJW_3tq) || \
    ((SJW) == CAN_SJW_4tq)   \
)
/**
  * @}
  */

/**
  * @defgroup CAN_time_quantum_in_bit_segment_1 
  * @brief CAN时间段1(BS1)时间量子配置组
  * @note 定义传播段和相位缓冲段1的总时间量子数，范围1-16tq
  *       总位时间 = 1(Sync_Seg) + BS1 + BS2
  */

/* 时间段1配置选项（直接影响抗干扰能力和最大波特率） */
#define CAN_BS1_1tq    ((uint8_t)0x00)  /*!< 1时间量子（传播段+缓冲段1总时长） */
#define CAN_BS1_2tq    ((uint8_t)0x01)  /*!< 2时间量子（适用于短距离高速通信） */
#define CAN_BS1_3tq    ((uint8_t)0x02)  /*!< 3时间量子 */
#define CAN_BS1_4tq    ((uint8_t)0x03)  /*!< 4时间量子（典型工业现场配置） */
#define CAN_BS1_5tq    ((uint8_t)0x04)  /*!< 5时间量子 */
#define CAN_BS1_6tq    ((uint8_t)0x05)  /*!< 6时间量子 */
#define CAN_BS1_7tq    ((uint8_t)0x06)  /*!< 7时间量子 */
#define CAN_BS1_8tq    ((uint8_t)0x07)  /*!< 8时间量子（长线通信推荐） */
#define CAN_BS1_9tq    ((uint8_t)0x08)  /*!< 9时间量子 */
#define CAN_BS1_10tq   ((uint8_t)0x09)  /*!< 10时间量子 */
#define CAN_BS1_11tq   ((uint8_t)0x0A)  /*!< 11时间量子 */
#define CAN_BS1_12tq   ((uint8_t)0x0B)  /*!< 12时间量子 */
#define CAN_BS1_13tq   ((uint8_t)0x0C)  /*!< 13时间量子 */
#define CAN_BS1_14tq   ((uint8_t)0x0D)  /*!< 14时间量子 */
#define CAN_BS1_15tq   ((uint8_t)0x0E)  /*!< 15时间量子 */
#define CAN_BS1_16tq   ((uint8_t)0x0F)  /*!< 16时间量子（最长配置，抗干扰最强） */

/* BS1参数校验宏（确保配置值在硬件支持范围内） */
#define IS_CAN_BS1(BS1) ((BS1) <= CAN_BS1_16tq)  // 验证BS1参数有效性（0x00-0x0F）
/*------------------------- 时间段2(BS2)配置组 -------------------------*/
/**
  * @defgroup CAN_time_quantum_in_bit_segment_2 
  * @brief CAN时间段2(BS2)时间量子配置选项
  * @note 定义相位缓冲段2的时间量子数（1-8tq），影响采样点位置
  */
#define CAN_BS2_1tq    ((uint8_t)0x00)  /*!< 1时间量子（最短缓冲段） */
#define CAN_BS2_2tq    ((uint8_t)0x01)  /*!< 2时间量子（推荐高速通信） */
#define CAN_BS2_3tq    ((uint8_t)0x02)  /*!< 3时间量子 */
#define CAN_BS2_4tq    ((uint8_t)0x03)  /*!< 4时间量子（典型配置） */
#define CAN_BS2_5tq    ((uint8_t)0x04)  /*!< 5时间量子 */
#define CAN_BS2_6tq    ((uint8_t)0x05)  /*!< 6时间量子 */
#define CAN_BS2_7tq    ((uint8_t)0x06)  /*!< 7时间量子 */
#define CAN_BS2_8tq    ((uint8_t)0x07)  /*!< 8时间量子（最长缓冲段） */

/* BS2参数校验宏 */
#define IS_CAN_BS2(BS2) ((BS2) <= CAN_BS2_8tq)  // 验证BS2参数有效性（0x00-0x07）

/*----------------------- 预分频器校验组 -----------------------*/
/**
  * @defgroup CAN_clock_prescaler 
  * @brief 预分频器范围校验宏
  * @note 确保分频系数在1-1024之间，影响时间量子精度
  */
#define IS_CAN_PRESCALER(PRESCALER) (((PRESCALER) >= 1) && ((PRESCALER) <= 1024))

/*----------------------- 过滤器配置组 -----------------------*/
/**
  * @defgroup CAN_filter_number 
  * @brief 过滤器编号校验宏
  * @note STM32F4系列支持28个过滤器（0-27）
  */
#define IS_CAN_FILTER_NUMBER(NUMBER) ((NUMBER) <= 27)  // 验证过滤器编号有效性

/**
  * @defgroup CAN_filter_mode 
  * @brief 过滤器工作模式选项
  * @note 配置过滤器匹配规则
  */
#define CAN_FilterMode_IdMask       ((uint8_t)0x00)  /*!< 标识符掩码模式（范围过滤） */
#define CAN_FilterMode_IdList       ((uint8_t)0x01)  /*!< 标识符列表模式（精确匹配） */
   // 掩码模式校验
        // 列表模式校验
#define IS_CAN_FILTER_MODE(MODE) ( \
    ((MODE) == CAN_FilterMode_IdMask) || \
    ((MODE) == CAN_FilterMode_IdList)   \
)

/**
  * @defgroup CAN_filter_scale 
  * @brief 过滤器位宽模式选项
  * @note 决定单个过滤器寄存器存储的ID数量
  */
#define CAN_FilterScale_16bit    ((uint8_t)0x00) /*!< 16位模式：1个过滤器存2个ID */
#define CAN_FilterScale_32bit    ((uint8_t)0x01) /*!< 32位模式：1个过滤器存1个ID+掩码 */
      // 16位模式校验
                 // 32位模式校验
#define IS_CAN_FILTER_SCALE(SCALE) ( \
    ((SCALE) == CAN_FilterScale_16bit) || \
    ((SCALE) == CAN_FilterScale_32bit) \
)

/**
  * @defgroup CAN_filter_FIFO
  * @brief 过滤器-FIFO关联配置
  * @note 指定匹配的报文存入FIFO0或FIFO1
  */
#define CAN_Filter_FIFO0    ((uint8_t)0x00)  /*!< 报文存入接收FIFO0 */
#define CAN_Filter_FIFO1    ((uint8_t)0x01)  /*!< 报文存入接收FIFO1 */
         // FIFO0校验
                   // FIFO1校验
#define IS_CAN_FILTER_FIFO(FIFO) ( \
    ((FIFO) == CAN_Filter_FIFO0) || \
    ((FIFO) == CAN_Filter_FIFO1)    \
)

/*----------------------- 从CAN过滤器组 -----------------------*/
/**
  * @defgroup CAN_Start_bank_filter_for_slave_CAN 
  * @brief 从CAN过滤器组起始编号校验
  * @note 双CAN实例时分配过滤器组范围
  */
#define IS_CAN_BANKNUMBER(BANKNUMBER) (((BANKNUMBER) >= 1) && ((BANKNUMBER) <= 27))

/*----------------------- 发送配置组 -----------------------*/
/**
  * @defgroup CAN_Tx 
  * @brief 发送相关参数校验
  */
#define IS_CAN_TRANSMITMAILBOX(MAILBOX) ((MAILBOX) <= 0x02)  /*!< 发送邮箱编号校验（0-2） */
#define IS_CAN_STDID(STDID)   ((STDID) <= 0x7FF)            /*!< 标准ID范围校验（11位） */
#define IS_CAN_EXTID(EXTID)   ((EXTID) <= 0x1FFFFFFF)        /*!< 扩展ID范围校验（29位） */
#define IS_CAN_DLC(DLC)       ((DLC) <= 0x08)                /*!< 数据长度码校验（0-8） */

/*----------------------- 标识符类型组 -----------------------*/
/**
  * @defgroup CAN_identifier_type 
  * @brief 帧标识符类型选项
  */
#define CAN_Id_Standard    ((uint32_t)0x00000000)  /*!< 标准帧（11位ID） */
#define CAN_Id_Extended    ((uint32_t)0x00000004)  /*!< 扩展帧（29位ID） */
             // 标准帧校验
                             // 扩展帧校验
#define IS_CAN_IDTYPE(IDTYPE) ( \
    ((IDTYPE) == CAN_Id_Standard) || \
    ((IDTYPE) == CAN_Id_Extended)   \
)

/*----------------------- 远程帧配置组 -----------------------*/
/**
  * @defgroup CAN_remote_transmission_request 
  * @brief 远程传输请求类型
  */
#define CAN_RTR_Data      ((uint32_t)0x00000000)  /*!< 数据帧（携带有效数据） */
#define CAN_RTR_Remote    ((uint32_t)0x00000002)  /*!< 远程帧（请求数据发送） */
                   // 数据帧校验
                                      // 远程帧校验
#define IS_CAN_RTR(RTR) ( \
    ((RTR) == CAN_RTR_Data) || \
    ((RTR) == CAN_RTR_Remote)    \
)
/*------------------------- CAN发送状态码 -------------------------*/

/**
  * @defgroup CAN_transmit_constants 
  * @brief CAN报文发送结果状态码
  * @note 用于CAN_TransmitStatus()函数返回值，反映报文发送状态
  */
#define CAN_TxStatus_Failed         ((uint8_t)0x00)/*!< 发送失败（仲裁丢失或错误超限） */
#define CAN_TxStatus_Ok             ((uint8_t)0x01) /*!< 发送成功（邮箱数据已传送到总线） */
#define CAN_TxStatus_Pending        ((uint8_t)0x02) /*!< 发送挂起（邮箱数据等待传输） */
#define CAN_TxStatus_NoMailBox      ((uint8_t)0x04) /*!< 无可用邮箱（三个发送邮箱均被占用） */

/* 历史兼容定义（不推荐新代码使用） */	
#define CANTXFAILED                  CAN_TxStatus_Failed  // 旧版本发送失败宏
#define CANTXOK                      CAN_TxStatus_Ok      // 旧版本发送成功宏
#define CANTXPENDING                 CAN_TxStatus_Pending // 旧版本发送挂起宏
#define CAN_NO_MB                    CAN_TxStatus_NoMailBox // 旧版本邮箱满宏

/*------------------------- 接收FIFO选择 -------------------------*/

/**
  * @defgroup CAN_receive_FIFO_number_constants 
  * @brief 接收FIFO选择配置
  * @note 用于指定从哪个FIFO读取接收到的报文
  */
#define CAN_FIFO0                 ((uint8_t)0x00) /*!< 使用FIFO0接收（默认优先级较高） */
#define CAN_FIFO1                 ((uint8_t)0x01) /*!< 使用FIFO1接收 */
#define IS_CAN_FIFO(FIFO) (((FIFO) == CAN_FIFO0) || ((FIFO) == CAN_FIFO1))  // FIFO选择校验

/*------------------------- 睡眠模式状态码 -------------------------*/

/**
  * @defgroup CAN_sleep_constants 
  * @brief 睡眠模式操作结果
  */
#define CAN_Sleep_Failed     ((uint8_t)0x00) /*!< 进入睡眠模式失败（总线活动未停止） */
#define CAN_Sleep_Ok         ((uint8_t)0x01) /*!< 成功进入睡眠模式（低功耗状态） */

/* 历史兼容定义 */	
#define CANSLEEPFAILED   CAN_Sleep_Failed  // 旧版本睡眠失败宏
#define CANSLEEPOK       CAN_Sleep_Ok      // 旧版本睡眠成功宏

/*------------------------- 唤醒模式状态码 -------------------------*/

/**
  * @defgroup CAN_wake_up_constants 
  * @brief 唤醒模式操作结果
  */
#define CAN_WakeUp_Failed        ((uint8_t)0x00) /*!< 唤醒失败（总线无活动） */
#define CAN_WakeUp_Ok            ((uint8_t)0x01) /*!< 成功退出睡眠模式 */

/* 历史兼容定义 */
#define CANWAKEUPFAILED   CAN_WakeUp_Failed // 旧版本唤醒失败宏     
#define CANWAKEUPOK       CAN_WakeUp_Ok     // 旧版本唤醒成功宏        

/*------------------------- 错误类型编码 -------------------------*/

/**
  * @defgroup CAN_Error_Code_constants
  * @brief CAN总线错误类型编码
  * @note 通过CAN_GetLastErrorCode()获取，反映最后检测到的错误类型
  */                                                         
#define CAN_ErrorCode_NoErr           ((uint8_t)0x00) /*!< 无错误 */ 
#define	CAN_ErrorCode_StuffErr        ((uint8_t)0x10) /*!< 位填充错误（连续6个相同电平） */ 
#define	CAN_ErrorCode_FormErr         ((uint8_t)0x20) /*!< 格式错误（固定位电平不符） */ 
#define	CAN_ErrorCode_ACKErr          ((uint8_t)0x30) /*!< 应答错误（发送节点未收到ACK） */ 
#define	CAN_ErrorCode_BitRecessiveErr ((uint8_t)0x40) /*!< 隐性位错误（节点发送显性但监测到隐性） */ 
#define	CAN_ErrorCode_BitDominantErr  ((uint8_t)0x50) /*!< 显性位错误（节点发送隐性但监测到显性） */ 
#define	CAN_ErrorCode_CRCErr          ((uint8_t)0x60) /*!< CRC校验错误（报文CRC与计算值不符） */ 
#define	CAN_ErrorCode_SoftwareSetErr  ((uint8_t)0x70) /*!< 软件错误（尝试在保留位写入1） */ 

/*------------------------- 硬件标志位定义 -------------------------*/

/**
  * @defgroup CAN_flags 
  * @brief CAN状态标志位
  * @note 标志位地址高4位说明：
  *       0x3XXXXXXX: 可通过CAN_ClearFlag()清除
  *       0x1XXXXXXX: 只读标志，不可清除
  */

/* 发送相关标志 -------------------------------------------------*/
#define CAN_FLAG_RQCP0   ((uint32_t)0x38000001) /*!< 邮箱0传输完成标志（1:完成/失败） */
#define CAN_FLAG_RQCP1   ((uint32_t)0x38000100) /*!< 邮箱1传输完成标志 */ 
#define CAN_FLAG_RQCP2   ((uint32_t)0x38010000) /*!< 邮箱2传输完成标志 */

/* 接收FIFO状态标志 ---------------------------------------------*/
#define CAN_FLAG_FMP0    ((uint32_t)0x12000003) /*!< FIFO0待读报文计数（值范围0-3） */
#define CAN_FLAG_FF0     ((uint32_t)0x32000008) /*!< FIFO0满标志（1:已满，3个报文未读） */
#define CAN_FLAG_FOV0    ((uint32_t)0x32000010) /*!< FIFO0溢出标志（新报文覆盖旧数据） */
#define CAN_FLAG_FMP1    ((uint32_t)0x14000003) /*!< FIFO1待读报文计数 */
#define CAN_FLAG_FF1     ((uint32_t)0x34000008) /*!< FIFO1满标志 */
#define CAN_FLAG_FOV1    ((uint32_t)0x34000010) /*!< FIFO1溢出标志 */
/*------------------------- 操作模式标志 -------------------------*/

/**
  * @defgroup CAN_operating_mode_flags 
  * @brief CAN操作模式状态标志
  * @note 可通过CAN_GetFlagStatus()获取标志状态，并使用CAN_ClearFlag()清除可清除标志
  */

#define CAN_FLAG_WKU               ((uint32_t)0x31000008) /*!< 唤醒标志（1:CAN控制器已退出睡眠模式） */
#define CAN_FLAG_SLAK              ((uint32_t)0x31000012) /*!< 睡眠确认标志（1:CAN控制器已进入睡眠模式） */
/* @note 当SLAK中断被禁用（SLKIE=0）时，无法通过CAN_ClearFlag清除SLAK标志。 
         在这种情况下，需要通过轮询检查SLAKI来确认是否进入睡眠模式。 
         否则，直接读取SLAK标志即可。*/

/*------------------------- 错误标志 -----------------------------*/

/**
  * @defgroup CAN_error_flags 
  * @brief CAN错误状态标志
  * @note 可通过CAN_GetFlagStatus()获取标志状态，并使用CAN_ClearFlag()清除可清除标志
  */
#define CAN_FLAG_EWG               ((uint32_t)0x10F00001) /*!< 错误警告标志（1:检测到8次错误，需关注） */
#define CAN_FLAG_EPV               ((uint32_t)0x10F00002) /*!< 错误被动标志（1:检测到13次错误，总线活动受限） */
#define CAN_FLAG_BOF               ((uint32_t)0x10F00004) /*!< 总线关闭标志（1:检测到16次错误，总线关闭） */
#define CAN_FLAG_LEC               ((uint32_t)0x30F00070) /*!< 最后错误代码标志（包含具体的错误类型，可清除） */

/* 标志位校验 ---------------------------------------------------*/
#define IS_CAN_GET_FLAG(FLAG) (((FLAG) == CAN_FLAG_LEC)  || ((FLAG) == CAN_FLAG_BOF)   || \
                               ((FLAG) == CAN_FLAG_EPV)  || ((FLAG) == CAN_FLAG_EWG)   || \
                               ((FLAG) == CAN_FLAG_WKU)  || ((FLAG) == CAN_FLAG_FOV0)  || \
                               ((FLAG) == CAN_FLAG_FF0)  || ((FLAG) == CAN_FLAG_FMP0)  || \
                               ((FLAG) == CAN_FLAG_FOV1) || ((FLAG) == CAN_FLAG_FF1)   || \
                               ((FLAG) == CAN_FLAG_FMP1) || ((FLAG) == CAN_FLAG_RQCP2) || \
                               ((FLAG) == CAN_FLAG_RQCP1)  || ((FLAG) == CAN_FLAG_RQCP0) || \
                               ((FLAG) == CAN_FLAG_SLAK )) /* 可获取标志校验 */
#define IS_CAN_CLEAR_FLAG(FLAG)(((FLAG) == CAN_FLAG_LEC) || ((FLAG) == CAN_FLAG_RQCP2) || \
                                ((FLAG) == CAN_FLAG_RQCP1)  || ((FLAG) == CAN_FLAG_RQCP0) || \
                                ((FLAG) == CAN_FLAG_FF0)  || ((FLAG) == CAN_FLAG_FOV0) ||\
                                ((FLAG) == CAN_FLAG_FF1) || ((FLAG) == CAN_FLAG_FOV1) || \
                                ((FLAG) == CAN_FLAG_WKU) || ((FLAG) == CAN_FLAG_SLAK)) /* 可清除标志校验 */

/*------------------------- CAN中断 ------------------------------*/

/**
  * @defgroup CAN_interrupts 
  * @brief CAN中断配置
  * @note 可通过CAN_ITConfig()配置中断使能状态
  */

#define CAN_IT_TME                  ((uint32_t)0x00000001) /*!< 发送邮箱空中断（1:任意邮箱已清空）*/

/* 接收中断 -----------------------------------------------------*/
#define CAN_IT_FMP0                 ((uint32_t)0x00000002) /*!< FIFO 0消息待读中断（1:FIFO0中有待读报文）*/
#define CAN_IT_FF0                  ((uint32_t)0x00000004) /*!< FIFO 0满中断（1:FIFO0已满3个报文）*/
#define CAN_IT_FOV0                 ((uint32_t)0x00000008) /*!< FIFO 0溢出中断（1:FIFO0溢出，新报文覆盖旧数据）*/
#define CAN_IT_FMP1                 ((uint32_t)0x00000010) /*!< FIFO 1消息待读中断（1:FIFO1中有待读报文）*/
#define CAN_IT_FF1                  ((uint32_t)0x00000020) /*!< FIFO 1满中断（1:FIFO1已满3个报文）*/
#define CAN_IT_FOV1                 ((uint32_t)0x00000040) /*!< FIFO 1溢出中断（1:FIFO1溢出，新报文覆盖旧数据）*/

/* 操作模式中断 -------------------------------------------------*/
#define CAN_IT_WKU                  ((uint32_t)0x00010000) /*!< 唤醒中断（1:CAN控制器已退出睡眠模式）*/
#define CAN_IT_SLK                  ((uint32_t)0x00020000) /*!< 睡眠确认中断（1:CAN控制器已进入睡眠模式）*/

/* 错误中断 -----------------------------------------------------*/
#define CAN_IT_EWG                  ((uint32_t)0x00000100) /*!< 错误警告中断（1:检测到8次错误，需关注）*/
#define CAN_IT_EPV                  ((uint32_t)0x00000200) /*!< 错误被动中断（1:检测到13次错误，总线活动受限）*/
#define CAN_IT_BOF                  ((uint32_t)0x00000400) /*!< 总线关闭中断（1:检测到16次错误，总线关闭）*/
#define CAN_IT_LEC                  ((uint32_t)0x00000800) /*!< 最后错误代码中断（1:检测到错误，可清除）*/
#define CAN_IT_ERR                  ((uint32_t)0x00008000) /*!< 错误中断（1:发生任意错误）*/

/**
  * @}
  */

/**
  * @defgroup CAN_interrupts 
  * @brief CAN中断配置
  * @note 可通过CAN_ITConfig()配置中断使能状态
  */

/* Flags named as Interrupts : kept only for FW compatibility */
/* 虽然这些标志作为中断使用，但为了向后兼容，它们仍然被定义为标志 */
#define CAN_IT_RQCP0   CAN_IT_TME /*!< 发送邮箱0空中断（兼容旧版本，实际使用CAN_IT_TME）*/
#define CAN_IT_RQCP1   CAN_IT_TME /*!< 发送邮箱1空中断（兼容旧版本，实际使用CAN_IT_TME）*/
#define CAN_IT_RQCP2   CAN_IT_TME /*!< 发送邮箱2空中断（兼容旧版本，实际使用CAN_IT_TME）*/

/* 中断校验 ---------------------------------------------------*/
#define IS_CAN_IT(IT)        (((IT) == CAN_IT_TME) || ((IT) == CAN_IT_FMP0)  ||\
                             ((IT) == CAN_IT_FF0)  || ((IT) == CAN_IT_FOV0)  ||\
                             ((IT) == CAN_IT_FMP1) || ((IT) == CAN_IT_FF1)   ||\
                             ((IT) == CAN_IT_FOV1) || ((IT) == CAN_IT_EWG)   ||\
                             ((IT) == CAN_IT_EPV)  || ((IT) == CAN_IT_BOF)   ||\
                             ((IT) == CAN_IT_LEC)  || ((IT) == CAN_IT_ERR)   ||\
                             ((IT) == CAN_IT_WKU)  || ((IT) == CAN_IT_SLK)) /*!< 中断校验宏（用于判断是否为有效中断）*/

/* 可清除中断校验 ---------------------------------------------*/
#define IS_CAN_CLEAR_IT(IT) (((IT) == CAN_IT_TME) || ((IT) == CAN_IT_FF0)    ||\
                             ((IT) == CAN_IT_FOV0)|| ((IT) == CAN_IT_FF1)    ||\
                             ((IT) == CAN_IT_FOV1)|| ((IT) == CAN_IT_EWG)    ||\
                             ((IT) == CAN_IT_EPV) || ((IT) == CAN_IT_BOF)    ||\
                             ((IT) == CAN_IT_LEC) || ((IT) == CAN_IT_ERR)    ||\
                             ((IT) == CAN_IT_WKU) || ((IT) == CAN_IT_SLK)) /*!< 可清除中断校验宏（用于判断是否为可清除中断）*/

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief 将CAN控制器配置为默认的复位状态
  * @param CANx 指向CAN1或CAN2的指针
  * @note 此函数会重置CAN控制器的所有寄存器为默认值
  */
void CAN_DeInit(CAN_TypeDef* CANx);

/**
  * @brief 初始化CAN控制器
  * @param CANx 指向CAN1或CAN2的指针
  * @param CAN_InitStruct 指向CAN初始化结构体的指针
  * @return 初始化结果，0表示成功，非0表示失败
  * @note 此函数用于配置CAN控制器的工作模式、时钟、波特率等参数
  */
uint8_t CAN_Init(CAN_TypeDef* CANx, CAN_InitTypeDef* CAN_InitStruct);

/**
  * @brief 初始化CAN过滤器
  * @param CAN_FilterInitStruct 指向CAN过滤器初始化结构体的指针
  * @note 此函数用于配置CAN过滤器，以决定哪些报文会被接收并放入FIFO中
  */
void CAN_FilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct);

/**
  * @brief 将CAN初始化结构体成员设置为默认值
  * @param CAN_InitStruct 指向CAN初始化结构体的指针
  * @note 此函数用于初始化CAN_InitStruct结构体，使其成员为默认配置
  */
void CAN_StructInit(CAN_InitTypeDef* CAN_InitStruct);

/**
  * @brief 设置CAN从模式开始的存储库号
  * @param CAN_BankNumber CAN存储库号（0-27）
  * @note 此函数用于设置CAN控制器在从模式下开始使用的存储库号
  */
void CAN_SlaveStartBank(uint8_t CAN_BankNumber);

/**
  * @brief 使能或失能CAN调试冻结
  * @param CANx 指向CAN1或CAN2的指针
  * @param NewState 使能状态（ENABLE或DISABLE）
  * @note 此函数用于在调试时冻结CAN控制器，防止其状态变化
  */
void CAN_DBGFreeze(CAN_TypeDef* CANx, FunctionalState NewState);

/**
  * @brief 使能或失能CAN时间触发传输模式
  * @param CANx 指向CAN1或CAN2的指针
  * @param NewState 使能状态（ENABLE或DISABLE）
  * @note 此函数用于配置CAN控制器的时间触发传输模式
  */
void CAN_TTComModeCmd(CAN_TypeDef* CANx, FunctionalState NewState);

/* CAN Frames Transmission functions ******************************************/
/**
  * @brief 发送CAN报文
  * @param CANx 指向CAN1或CAN2的指针
  * @param TxMessage 指向CAN发送报文结构体的指针
  * @return 发送结果，0表示成功，非0表示失败
  * @note 此函数用于将CAN报文放入指定的邮箱进行发送
  */
uint8_t CAN_Transmit(CAN_TypeDef* CANx, CanTxMsg* TxMessage);

/**
  * @brief 获取指定邮箱的发送状态
  * @param CANx 指向CAN1或CAN2的指针
  * @param TransmitMailbox 发送邮箱号（0-2）
  * @return 发送状态（CAN_TxStatus_Failed, CAN_TxStatus_Ok, CAN_TxStatus_Pending, CAN_TxStatus_NoMailBox）
  * @note 此函数用于查询指定邮箱的发送状态，确认报文是否成功发送
  */
uint8_t CAN_TransmitStatus(CAN_TypeDef* CANx, uint8_t TransmitMailbox);

/**
  * @brief 取消指定邮箱的报文发送
  * @param CANx 指向CAN1或CAN2的指针
  * @param Mailbox 发送邮箱号（0-2）
  * @note 此函数用于取消指定邮箱中的报文发送请求
  */
void CAN_CancelTransmit(CAN_TypeDef* CANx, uint8_t Mailbox);

/* CAN Frames Reception functions *********************************************/
/**
  * @brief 接收CAN报文
  * @param CANx 指向CAN1或CAN2的指针
  * @param FIFONumber 接收FIFO号（0或1）
  * @param RxMessage 指向CAN接收报文结构体的指针
  * @note 此函数用于从指定的FIFO中读取接收到的报文
  */
void CAN_Receive(CAN_TypeDef* CANx, uint8_t FIFONumber, CanRxMsg* RxMessage);

/**
  * @brief 释放指定FIFO中的报文
  * @param CANx 指向CAN1或CAN2的指针
  * @param FIFONumber 接收FIFO号（0或1）
  * @note 此函数用于释放FIFO中的报文，使其可以接收新的报文
  */
void CAN_FIFORelease(CAN_TypeDef* CANx, uint8_t FIFONumber);

/**
  * @brief 检查指定FIFO中是否有待处理的报文
  * @param CANx 指向CAN1或CAN2的指针
  * @param FIFONumber 接收FIFO号（0或1）
  * @return 待处理报文数量（0-3）
  * @note 此函数用于检查指定FIFO中是否有待处理的报文
  */
uint8_t CAN_MessagePending(CAN_TypeDef* CANx, uint8_t FIFONumber);
/*------------------------- 操作模式函数组 -------------------------*/

/**
  * @brief 请求CAN控制器的操作模式
  * @param CANx 指向CAN1或CAN2的指针
  * @param CAN_OperatingMode 操作模式（CAN_OperatingMode_Initialization, CAN_OperatingMode_Normal, CAN_OperatingMode_Sleep）
  * @return 操作结果状态码（CAN_ModeStatus_Failed 或 CAN_ModeStatus_Success）
  * @note 此函数用于请求CAN控制器进入指定的操作模式
  */
uint8_t CAN_OperatingModeRequest(CAN_TypeDef* CANx, uint8_t CAN_OperatingMode);

/**
  * @brief 使CAN控制器进入睡眠模式
  * @param CANx 指向CAN1或CAN2的指针
  * @return 睡眠模式请求结果状态码（CAN_ModeStatus_Failed 或 CAN_ModeStatus_Success）
  * @note 此函数用于请求CAN控制器进入睡眠模式以降低功耗
  */
uint8_t CAN_Sleep(CAN_TypeDef* CANx);

/**
  * @brief 使CAN控制器从睡眠模式唤醒
  * @param CANx 指向CAN1或CAN2的指针
  * @return 唤醒模式请求结果状态码（CAN_ModeStatus_Failed 或 CAN_ModeStatus_Success）
  * @note 此函数用于请求CAN控制器从睡眠模式唤醒
  */
uint8_t CAN_WakeUp(CAN_TypeDef* CANx);

/*------------------------- CAN总线错误管理函数组 -------------------------*/

/**
  * @brief 获取CAN控制器的最后一个错误代码
  * @param CANx 指向CAN1或CAN2的指针
  * @return 错误代码（CAN_ErrorCode_NoErr, CAN_ErrorCode_StuffErr, CAN_ErrorCode_FormErr, CAN_ErrorCode_ACKErr, CAN_ErrorCode_BitRecessiveErr, CAN_ErrorCode_BitDominantErr, CAN_ErrorCode_CRCErr, CAN_ErrorCode_SoftwareSetErr）
  * @note 此函数用于获取CAN控制器在总线通信中检测到的最后一个错误类型
  */
uint8_t CAN_GetLastErrorCode(CAN_TypeDef* CANx);

/**
  * @brief 获取CAN控制器的接收错误计数器
  * @param CANx 指向CAN1或CAN2的指针
  * @return 接收错误计数器值（0-127）
  * @note 此函数用于获取CAN控制器的接收错误计数器，超过127表示进入错误被动状态
  */
uint8_t CAN_GetReceiveErrorCounter(CAN_TypeDef* CANx);

/**
  * @brief 获取CAN控制器的传输错误计数器（最低有效字节）
  * @param CANx 指向CAN1或CAN2的指针
  * @return 传输错误计数器值（最低有效字节，0-63）
  * @note 此函数用于获取CAN控制器的传输错误计数器，超过63表示进入错误被动状态
  */
uint8_t CAN_GetLSBTransmitErrorCounter(CAN_TypeDef* CANx);

/*------------------------- 中断与标志管理函数组 -------------------------*/

/**
  * @brief 配置CAN中断使能状态
  * @param CANx 指向CAN1或CAN2的指针
  * @param CAN_IT 中断类型（CAN_IT_TME, CAN_IT_FMP0, CAN_IT_FF0, CAN_IT_FOV0, CAN_IT_FMP1, CAN_IT_FF1, CAN_IT_FOV1, CAN_IT_EWG, CAN_IT_EPV, CAN_IT_BOF, CAN_IT_LEC, CAN_IT_ERR, CAN_IT_WKU, CAN_IT_SLK）
  * @param NewState 使能状态（ENABLE或DISABLE）
  * @note 此函数用于配置CAN控制器的中断使能状态，并通过NVIC进行中断管理
  */
void CAN_ITConfig(CAN_TypeDef* CANx, uint32_t CAN_IT, FunctionalState NewState);

/**
  * @brief 获取CAN控制器的状态标志
  * @param CANx 指向CAN1或CAN2的指针
  * @param CAN_FLAG 状态标志（CAN_FLAG_EWG, CAN_FLAG_EPV, CAN_FLAG_BOF, CAN_FLAG_LEC, CAN_FLAG_WKU, CAN_FLAG_SLAK, CAN_FLAG_FF0, CAN_FLAG_FOV0, CAN_FLAG_FMP0, CAN_FLAG_FF1, CAN_FLAG_FOV1, CAN_FLAG_FMP1）
  * @return 标志状态（SET 或 RESET）
  * @note 此函数用于查询CAN控制器的状态标志
  */
FlagStatus CAN_GetFlagStatus(CAN_TypeDef* CANx, uint32_t CAN_FLAG);

/**
  * @brief 清除CAN控制器的状态标志
  * @param CANx 指向CAN1或CAN2的指针
  * @param CAN_FLAG 待清除的状态标志（CAN_FLAG_EWG, CAN_FLAG_EPV, CAN_FLAG_BOF, CAN_FLAG_LEC, CAN_FLAG_WKU, CAN_FLAG_SLAK, CAN_FLAG_FF0, CAN_FLAG_FOV0, CAN_FLAG_FMP0, CAN_FLAG_FF1, CAN_FLAG_FOV1, CAN_FLAG_FMP1）
  * @note 此函数用于清除CAN控制器的状态标志
  */
void CAN_ClearFlag(CAN_TypeDef* CANx, uint32_t CAN_FLAG);

/**
  * @brief 获取CAN控制器的中断状态
  * @param CANx 指向CAN1或CAN2的指针
  * @param CAN_IT 中断类型（CAN_IT_TME, CAN_IT_FMP0, CAN_IT_FF0, CAN_IT_FOV0, CAN_IT_FMP1, CAN_IT_FF1, CAN_IT_FOV1, CAN_IT_EWG, CAN_IT_EPV, CAN_IT_BOF, CAN_IT_LEC, CAN_IT_ERR, CAN_IT_WKU, CAN_IT_SLK）
  * @return 中断状态（SET 或 RESET）
  * @note 此函数用于查询CAN控制器的中断状态
  */
ITStatus CAN_GetITStatus(CAN_TypeDef* CANx, uint32_t CAN_IT);

/**
  * @brief 清除CAN控制器的中断挂起标志
  * @param CANx 指向CAN1或CAN2的指针
  * @param CAN_IT 待清除的中断类型（CAN_IT_TME, CAN_IT_FMP0, CAN_IT_FF0, CAN_IT_FOV0, CAN_IT_FMP1, CAN_IT_FF1, CAN_IT_FOV1, CAN_IT_EWG, CAN_IT_EPV, CAN_IT_BOF, CAN_IT_LEC, CAN_IT_ERR, CAN_IT_WKU, CAN_IT_SLK）
  * @note 此函数用于清除CAN控制器的中断挂起标志
  */
void CAN_ClearITPendingBit(CAN_TypeDef* CANx, uint32_t CAN_IT);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_CAN_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

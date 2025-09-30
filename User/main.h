/**
 ******************************************************************************
 * @file    main.h
 * @author  MCU Application Team
 * @brief   Header for main.c file.
 *          This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by Puya under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "py32f0xx_hal.h"
#include "py32f003xx_Start_Kit.h"
#include "arm_math.h"
#include <stdio.h>
#include <stdlib.h>

/* Private includes ----------------------------------------------------------*/
#include "Drive_uart.h"
#include "Drive_adc.h"
#include "Drive_BackLed.h"
#include "Drive_Button.h"
#include "Drive_Buz.h"
#include "Drive_clock.h"
#include "Drive_DesplayLcd.h"
#include "Drive_Encoder.h"
#include "Drive_MosSwitch.h"
#include "Drive_uart.h"
#include "Drive_Iwdg.h"
#include "Drive_flash.h"

#include "APP_displayLcd.h"
#include "APP_TimeSlice.h"
#include "APP_pid.h"
#include "APP_math.h"
#include "APP_HCI.h"
#include "APP_Coder.h"
#include "APP_BuzMusic.h"
#include "APP_Sleep.h"

/* Private defines -----------------------------------------------------------*/
#define DEBUG_TAG "OSS-T210"

#define USE_BUZ_ACTIVE 0
#define USE_BUZ_PASSIVE 1
#define USE_BUZ_TYPE USE_BUZ_ACTIVE // 0:有源蜂鸣器 1:无源蜂鸣器（可发出音调）

#define USE_DISPLAY_TYPE_HG1621 0 // HG1612/HG1621,屏幕驱动方式
#define USE_DISPLAY_TYPE_AIP650 1 // AIP650,屏幕驱动方式
#define USE_DISPLAY_TYPE USE_DISPLAY_TYPE_AIP650

#define FLASH_USER_START_ADDR1 0x0800F000
#define FLASH_USER_START_ADDR2 0x0800F800
#define FLASH_USER_START_ADDR3 0x0800FF00

#define ADC_CHANNEL_NUM 8 // 通道数

#define PCB_NTC_NUM 0               // ADC序号
#define SOLDERING_ELECTRICITY_NUM 1 // ADC序号
#define SOLDERING_TEMP210_NUM 2     // ADC序号
#define SOLDERING_TEMP245_NUM 3     // ADC序号
#define SOLDERING_TID_NUM 4         // ADC序号
#define SLEEP_NUM 7                 // ADC序号

#define BUZ_ON_TIME 2                // 单位：25ms(蜂鸣器)
#define FLASH_CHECK_VLUEb 0xAAAAAAAA // 校验值
// #define FLASH_CHECK_VLUEb 0xACACACAC // 校验值
#define FIRST_SOLDERING_TEMP 300 // 初始化温度
#define NEW_TEMP_SAVE_TIME 3     // 温度调节后保存时间
#define PCB_PROTECT_TEMP 75.0f   // PCB保护温度

// 积分限幅
#define T210_MAX_PID_I 2250 // 60W
#define T245_MAX_PID_I 3000 // 110W ±10W
#define T115_MAX_PID_I 750  // 30W

// 功率限幅
#define T210_MAX_PID_O 2250             // 60W ±10W
#define T245_MAX_PID_O 5000             // 120W ±10W
#define T115_MAX_PID_O 1400             // 30W ±10W
#define T12_MAX_PID_O 10000             // 70W ±5W
#define T245_POWER_COMPENSATION_STEP 40 // T245功率补偿步进值PWM/W
#define T245_POWER_COMPENSATION_MAX 50  // T245功率补偿最大值

// 显示稳定滤波最小值
#define T115_PID_MIX_CHANGE_PRIOD 100
#define T210_PID_MIX_CHANGE_PRIOD 200
#define T245_PID_MIX_CHANGE_PRIOD 700
#define T12_PID_MIX_CHANGE_PRIOD 1900

// 显示稳定滤波突变值
#define T115_PID_MAX_CHANGE_PRIOD 150
#define T210_PID_MAX_CHANGE_PRIOD 200
#define T245_PID_MAX_CHANGE_PRIOD 1000
#define T12_PID_MAX_CHANGE_PRIOD 2000

#define MAX_STRONG_TEMP 450 // 强温最大值
#define MAX_TAR_TEMP 480    // 目标温度最大值
#define MIN_TAR_TEMP 105    // 目标温度最小值（实际值=MIN_TAR_TEMP-5）
#define N_TAR_TEMP_STEP 5   // 目标温度调节步进值

#define CALIBRATION_TEMP_MAX 50  // 校准温度最大值
#define CALIBRATION_TEMP_MIN -50 // 校准温度最小值

#define SLEEP_DELAY_TIME_MIN 0   // 休眠延时最小值
#define SLEEP_DELAY_TIME_MAX 120 // 休眠延时最大值
#define SLEEP_DEEP_TEMP_RANGE 55 // 深度休眠温度阈值

#define PROTECT_TEMP_MIN 100 // 待机保护温度最小值
#define PROTECT_TEMP_MAX 300 // 待机保护温度最大值

#define STANDBY_DELAY_TIME_MIN 0   // 待机延时最小值
#define STANDBY_DELAY_TIME_MAX 120 // 待机延时最大值

#define STRONG_WARM_TIME_MIN 60  // 加强温度时间最小值
#define STRONG_WARM_TIME_MAX 180 // 加强温度时间最大值

#define SOLDERING_PID_I_CMD 15          // 积分引入温度阈值
#define SOLDERING_PID_I_CLOSE 15        // 积分引出温度阈值
#define SOLDERING_TEMP_DISPLAY_REDUCE 5 // 温度显示降低阈值

#define T115_SOLDERING_MAX_PID 20 // 最大功率输出温度阈值
#define T210_SOLDERING_MAX_PID 20 // 最大功率输出温度阈值
#define T245_SOLDERING_MAX_PID 25 // 最大功率输出温度阈值

#define SOLDERING_ELECTRICITY_THRESHOLD 250 // 短路电压阈值
#define SOLDERING_ELECTRICITY_OPEN 20      // 开路电压阈值
#define SOLDERING_TEMP_OPEN 600             // 电烙铁温度错误

#define SOLDERING_TID_T245 0x800     // ADC判断阈值
#define SOLDERING_TID_T210 0x380     // ADC判断阈值
#define SOLDERING_TID_T115_MIX 0x380 // ADC判断阈值
#define SOLDERING_TID_T115_MAX 0x600 // ADC判断阈值

#define SOLDERING_MODEL_T210 210 // 烙铁头型号
#define SOLDERING_MODEL_T245 245 // 烙铁头型号
#define SOLDERING_MODEL_T115 115 // 烙铁头型号

#define PREINSTALL_TMEP300 1 // 预设
#define PREINSTALL_TMEP350 2 // 预设
#define PREINSTALL_TMEP400 3 // 预设

#define OPTION_NUM 8 // 选项个数
#define SMG_P01 1    // 设置发热芯待机保护温度，可调节范围100-300摄氏度，默认200摄氏度。
#define SMG_P02 2    // 待机模式时间，可调节范围0-120秒钟， OFF：关闭待机模式。
#define SMG_P03 3    // 休眠模式时间，可调节范围0-120分钟， OFF：关闭休眠模式。
#define SMG_P04 4    // 一键增强温度保持时间，可调节范围60-180秒钟。
#define SMG_P05 5    // 声音状态，可调节打开/关闭机器声音。
#define SMG_P06 6    // 发热芯温度校准，可调节校准参数-50到50摄氏度。
#define SMG_P07 7    // (预设温度开关 ON：使用预设温度 OFF：关闭预设温度)。
#define SMG_P08 8    // (睡眠状态屏下幕变暗，ON：开启该功能 OFF：关闭该功能)。
#define SMG_P09 9    // (切换功率显示 ON：显示功率 OFF：显示电烙铁温度)。

#define SMG_OFF 0
#define SMG_ON 1

#define CODERING_RIGHT 1
#define CODERING_LEFT 0

#define BUTTON_LONG_PRESS_TIME 60 // 3S

#define ERROR_E0 0           // 烧坏&开路&短路
#define ERROR_E1 1           // NTC温度过高
#define ERROR_E2 2           // 短路
#define ERROR_E3 3           // 手柄未接
#define ERROR_SYSTEM_INIT 15 // MCU外设损坏
#define DRIVE_SLEEP 16       // 睡眠

#define TIME_TO_CLOSE_BACKLIGHT 60 // 熄屏时间 单位：秒
/* Exported variables prototypes ---------------------------------------------*/
typedef struct
{
    uint32_t EncoderIsRun;      // 编码器运行状态
    uint32_t TurnleftOrRight;   // 旋转方向
    uint32_t ButtonIsTrigeer;   // 短按事件
    uint32_t ButtonIsLongPress; // 长按事件
    uint32_t OneKeyStrongTemp;  // 一键增强温度选择
} TYPEDEF_ENCODER_S;

typedef struct
{
    float32_t pid_pCoef;
    float32_t pid_iCoef;
    float32_t pid_dCoef;
    float32_t pid_pItem;    // 比例项
    float32_t pid_iItem;    // 积分项
    float32_t pid_iItemCmd; // 积分使能
    float32_t pid_dItem;    // 微分项
    float32_t pid_integration_max;
    float32_t pid_lastDif;      // 上一个误差
    float32_t pid_out;          // PWM输出
    float32_t pid_outMax;       // 最大PWM输出
    uint32_t outCmd;            // 输出使能
    uint32_t outPriod;          // 滤波参数
    uint32_t outPriod_max;      // 滤波参数
    uint32_t diffTempOutMaxPWM; // 温差输出最大PWM
    uint32_t pid_iItemJoinTemp; // 积分引入温度
    uint32_t pid_iItemQuitTemp; // 积分引出温度
} TYPEDEF_PID_parameter_S;

typedef struct
{
    uint32_t checkVlue;            // 校验值
    uint32_t TarTemp;              // 目标温度（设置后3秒掉电保存，保护flash寿命）(预设模式不保存)
    int32_t calibration_temp;      // 校准温度值（修改后立即保存）
    uint32_t BuzOnOff;             // 蜂鸣器使能（修改后立即保存）
    uint32_t PreinstallTempOnOff;  // 预设温度使能（修改后立即保存）
    uint32_t PreinstallTempNum;    // 预设温度编号（修改后立即保存）
    uint32_t ProtectTemp;          // 待机保护温度（修改后立即保存）
    uint32_t StandbyTime;          // 待机模式时间（修改后立即保存）单位s
    uint32_t SleepDelayTime;       // 休眠时间（修改后立即保存）单位min
    uint32_t KeepStrongTempTime;   // 一键增强温度保持时间（修改后立即保存）单位s
    uint32_t DisplayPowerOnOff;    // 功率显示开关（修改后立即保存）
    uint32_t BackgroundLightOnoff; // 背光开关（修改后立即保存）
    uint32_t SaveNum;              // 保存次数

} TYPEDEF_FLASHSAVE_S;

typedef struct
{
    uint32_t SetingPage;       // 设置页面
    int32_t PNumber;           // P码
    uint32_t SetingSel;        // 进入选择项
    uint32_t OptionChange;     // 选项选择
    uint32_t CommonModeChange; // 普通模式选择
} TYPEDEF_SETINNG_PAGE_S;

typedef enum
{
    SOLDERING_STATE_OK = 0,         // 正常状态
    SOLDERING_STATE_INIT_ERROR,     // MCU外设损坏
    SOLDERING_STATE_SHORTCIR_ERROR, // 短路状态
    SOLDERING_STATE_OPEN_ERROR,     // 开路状态
    SOLDERING_STATE_STANDBY,        // 待机状态
    SOLDERING_STATE_SLEEP,          // 休眠状态
    SOLDERING_STATE_SLEEP_DEEP,     // 深度睡眠状态
    SOLDERING_STATE_NTC_ERROR,      // NTC错误状态
    SOLDERING_STATE_PULL_OUT_ERROR  // 拔出错误状态
} TYPEDEF_SOLDERING_STATE_S;

typedef struct
{
    volatile float32_t CurTemp;    // 当前温度
    volatile uint32_t Old_TarTemp; // 历史目标温度

    uint32_t adc_value[ADC_CHANNEL_NUM];            // adc采样值
    float32_t adc_conversionValue[ADC_CHANNEL_NUM]; // 处理后的值（PCB温度、电流mA、T210温度）
    float32_t data_filter[ADC_CHANNEL_NUM];         // 低通滤波后的值（PCB温度、电流mA、T210温度）(实时)
    float32_t data_filter_prev[ADC_CHANNEL_NUM];    // 低通滤波前的值（PCB温度、电流mA、T210温度）(历史)

    volatile TYPEDEF_FLASHSAVE_S flashSave_s; // 掉电保存参数
    TYPEDEF_FLASHSAVE_S *P_flashSave_s1;      // 指向掉电保存参数地址
    TYPEDEF_FLASHSAVE_S *P_flashSave_s2;      // 指向掉电保存参数地址
    TYPEDEF_FLASHSAVE_S *P_flashSave_s3;      // 指向掉电保存参数地址
    TYPEDEF_PID_parameter_S pid_s;            // 位置式pid参数
    TYPEDEF_ENCODER_S encoder_s;              // 旋转编码器
    volatile TYPEDEF_SETINNG_PAGE_S Seting;   // 设置页面

    TYPEDEF_SOLDERING_STATE_S SolderingState; // 发热芯状态
    float32_t r0;                             // 阻值
    float32_t Electricity;                    // 负载电流A
    float32_t Power;                          // 功率W
    float32_t PowerStatic;                    // 功率静态值W
    uint32_t Sleep_PowerFilter;               // 休眠功率滤波值

    uint32_t Buz_ccr;             // 蜂鸣器PWM占空比
    uint8_t BuzFlag;              // 蜂鸣器启动状态
    uint8_t BuzmusicNum;          // 蜂鸣器音乐编号
    uint16_t BuzMusicFreq;        // 蜂鸣器音乐频率
    uint8_t PwmIsInitComplete;    // PWM初始化完成
    uint8_t SleepFlag;            // 休眠标志
    uint8_t OneState_TempOk;      // 到达温度标志
    uint8_t SolderingModelNumber; // 发热芯型号
    uint16_t adc_filter_value;    // adc滤波值
    uint16_t sleep_cnt;           // 休眠计数

} TYPEDEF_ALL_STATUS_S;

extern TYPEDEF_ALL_STATUS_S AllStatus_S;
extern __IO uint32_t uwTick;
extern IWDG_HandleTypeDef IwdgHandle;
extern ADC_HandleTypeDef AdcHandle;
/* Exported functions prototypes ---------------------------------------------*/
void APP_ErrorHandler(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/

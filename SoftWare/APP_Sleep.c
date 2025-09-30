#include "main.h"

// 休眠检测相关宏定义
#define SLEEP_ADC_STABLE_RANGE 0x400 // ADC稳定范围
#define SLEEP_ADC_TASK_PERIOD_MS 250 // 任务调用周期（ms）

// 获取ADC值
static uint32_t APP_Sleep_GetAdcValue(void)
{
    /* 采样率: 4Hz (任务周期250ms)
       目标截止频率: 0.5Hz
    */
    uint32_t raw = Drive_ADCConvert(SLEEP_NUM);
    AllStatus_S.adc_value[SLEEP_NUM] = raw;

    static uint8_t initialized = 0;
    static float filtered = 0.0f;

    const float dt = 0.25f; // 1 / 4Hz
    const float fc = 0.5f;  // 截止频率
    const float RC = 1.0f / (2.0f * 3.1415926f * fc);
    const float alpha = dt / (RC + dt); 

    if (!initialized)
    {
        filtered = (float)raw;
        initialized = 1;
    }
    else
    {
        filtered += alpha * ((float)raw - filtered);
    }

    return (uint32_t)(filtered + 0.5f);
}

static uint32_t APP_Sleep_PowerCheck(void)
{
    float tar = (float)AllStatus_S.flashSave_s.TarTemp;
    static float minT = 100.0f;
    static float maxT = 450.0f;
    static float minP = 8.0f;
    static float maxP = 20.0f;

    if (tar <= minT)
        return minP;
    if (tar >= maxT)
        return maxP;

    return minP + (tar - minT) * (maxP - minP) / (maxT - minT);
}

static float32_t APP_Sleep_TempCheck(void)
{
    float tar = (float)AllStatus_S.flashSave_s.TarTemp;
    static float minT = 100.0f;
    static float maxT = 450.0f;
    static float minRange = 2.0f;
    static float maxRange = 4.0f;

    if (tar <= minT)
        return minRange;
    if (tar >= maxT)
        return maxRange;

    return minRange + (tar - minT) * (maxRange - minRange) / (maxT - minT);
}

// 图标闪烁相关：任务调用周期为 SLEEP_ADC_TASK_PERIOD_MS (250ms)
// 设置图标翻转（Toggle）周期（ms），应为 SLEEP_ADC_TASK_PERIOD_MS 的整数倍
#ifndef SLEEP_ICON_BLINK_TOGGLE_PERIOD_MS
#define SLEEP_ICON_BLINK_TOGGLE_PERIOD_MS 500u // 每500ms翻转一次(=2次任务调用)
#endif

#if (SLEEP_ICON_BLINK_TOGGLE_PERIOD_MS < SLEEP_ADC_TASK_PERIOD_MS)
#error "SLEEP_ICON_BLINK_TOGGLE_PERIOD_MS must >= SLEEP_ADC_TASK_PERIOD_MS"
#endif

static void APP_SleepIconBlink(void)
{
    static uint16_t elapsed_ms = 0;
    static uint8_t icon_on = 0;

    if (AllStatus_S.SolderingState == SOLDERING_STATE_STANDBY)
    {
        elapsed_ms += SLEEP_ADC_TASK_PERIOD_MS;
        if (elapsed_ms >= SLEEP_ICON_BLINK_TOGGLE_PERIOD_MS)
        {
            elapsed_ms = 0;
            icon_on = !icon_on;
            Lcd_icon_onOff(icon_soldering, icon_on);
        }
    }
}

// 休眠控制任务，每250ms调用一次
void APP_Sleep_Control_Task(void)
{
    // 状态转换逻辑：
    // 1. ADC变化超过阈值 -> 退出待机/休眠/深度睡眠，恢复正常
    //    或者功率超过阈值 -> 退出待机/休眠/深度睡眠，恢复正常
    //    或者温度偏差超过阈值 -> 退出待机/休眠/深度睡眠，恢复正常
    //    （以上三项任意一项满足即可）
    // 2. 一键增强温度激活 -> 清空计时
    // 3. ADC在 StandbyTime 秒内保持稳定 -> 进入待机
    // 4. 待机时间超过 SleepDelayTime 分钟 -> 进入休眠
    // 5. 休眠状态下当前温度低于 SLEEP_DEEP_TEMP_RANGE -> 进入深度休眠
    static uint32_t last_adc_value = 0;     // 上次ADC值
    static uint32_t stable_time_ms = 0;     // 用于判定进入待机
    static uint8_t initialized = 0;         // 是否已初始化
    static uint32_t standby_elapsed_ms = 0; // 已处于待机后的累计时间
    uint32_t cur_adc_value = APP_Sleep_GetAdcValue();
    // Lcd_SMG_DisplaySel(cur_adc_value, 1, uintHex);

    if (AllStatus_S.SolderingState == SOLDERING_STATE_PULL_OUT_ERROR || AllStatus_S.SolderingState == SOLDERING_STATE_SHORTCIR_ERROR)
    {
        stable_time_ms = 0;
        standby_elapsed_ms = 0;
        return;
    }

    if (AllStatus_S.SolderingState == SOLDERING_STATE_OK)
    {
        AllStatus_S.Sleep_PowerFilter = APP_Sleep_PowerFilter();
        if (AllStatus_S.Sleep_PowerFilter > APP_Sleep_PowerCheck())
        {
            stable_time_ms = 0;
            standby_elapsed_ms = 0;
        }
    }

    if (AllStatus_S.SolderingState == SOLDERING_STATE_OK)
    {
        float temp_diff = (float32_t)AllStatus_S.flashSave_s.TarTemp - AllStatus_S.data_filter_prev[SOLDERING_TEMP210_NUM];

        if (temp_diff > APP_Sleep_TempCheck()) // 温度变小才生效
        {
            stable_time_ms = 0;
            standby_elapsed_ms = 0;
        }
    }

    if (AllStatus_S.encoder_s.OneKeyStrongTemp && AllStatus_S.SolderingState == SOLDERING_STATE_OK)
    {
        stable_time_ms = 0;
        standby_elapsed_ms = 0;
    }

    // 初始化
    if (!initialized)
    {
        last_adc_value = cur_adc_value;
        initialized = 1;
    }

    uint32_t diff = (cur_adc_value > last_adc_value) ? (cur_adc_value - last_adc_value)
                                                     : (last_adc_value - cur_adc_value);

    uint32_t standby_threshold_ms = (uint32_t)AllStatus_S.flashSave_s.StandbyTime * 1000UL;     // 秒 -> ms
    uint32_t sleep_delay_ms = (uint32_t)AllStatus_S.flashSave_s.SleepDelayTime * 60UL * 1000UL; // 分钟 -> ms

    // 1. 大幅变化: 退出非正常状态
    if (diff > SLEEP_ADC_STABLE_RANGE)
    {
        stable_time_ms = 0;
        standby_elapsed_ms = 0;
        if (AllStatus_S.SolderingState == SOLDERING_STATE_STANDBY ||
            AllStatus_S.SolderingState == SOLDERING_STATE_SLEEP ||
            AllStatus_S.SolderingState == SOLDERING_STATE_SLEEP_DEEP)
        {
            AllStatus_S.SolderingState = SOLDERING_STATE_OK;
            AllStatus_S.flashSave_s.TarTemp = AllStatus_S.Old_TarTemp;
            AllStatus_S.OneState_TempOk = 0;
            Lcd_icon_onOff(icon_soldering, 0);
            Drive_Buz_OnOff(BUZ_20MS, BUZ_FREQ_CHANGE_OFF, USE_BUZ_TYPE);
            Drive_DisplayLcd_Init();
        }
        last_adc_value = cur_adc_value;
        return;
    }

    // ADC稳定
    last_adc_value = cur_adc_value;

    switch (AllStatus_S.SolderingState)
    {
    case SOLDERING_STATE_OK:
    case SOLDERING_STATE_STANDBY:
        // 2. 累计稳定时间 -> 进入待机
        if (AllStatus_S.SolderingState != SOLDERING_STATE_STANDBY)
        {
            if (standby_threshold_ms > 0 && stable_time_ms < standby_threshold_ms)
            {
                stable_time_ms += SLEEP_ADC_TASK_PERIOD_MS;
                if (stable_time_ms > standby_threshold_ms)
                    stable_time_ms = standby_threshold_ms;
            }
            if (standby_threshold_ms > 0 && stable_time_ms >= standby_threshold_ms)
            {
                AllStatus_S.SolderingState = SOLDERING_STATE_STANDBY;
                if (AllStatus_S.Old_TarTemp > AllStatus_S.flashSave_s.ProtectTemp)
                    AllStatus_S.OneState_TempOk = 0;
                standby_elapsed_ms = 0;
                Drive_Buz_OnOff(BUZ_20MS, BUZ_FREQ_CHANGE_OFF, USE_BUZ_TYPE);
            }
        }
        if (standby_threshold_ms > 0)
            APP_SleepIconBlink(); // 待机闪烁图标
        if (standby_threshold_ms == 0 || AllStatus_S.SolderingState == SOLDERING_STATE_STANDBY)
        {
            // 3. 待机累计时间 -> 进入休眠状态
            if (sleep_delay_ms > 0 && standby_elapsed_ms < sleep_delay_ms)
            {
                standby_elapsed_ms += SLEEP_ADC_TASK_PERIOD_MS;
                if (standby_elapsed_ms > sleep_delay_ms)
                    standby_elapsed_ms = sleep_delay_ms;
            }
            if (sleep_delay_ms > 0 && standby_elapsed_ms >= sleep_delay_ms)
            {
                AllStatus_S.SolderingState = SOLDERING_STATE_SLEEP;
                Lcd_icon_onOff(icon_soldering, 1);
                Drive_Buz_OnOff(BUZ_20MS, BUZ_FREQ_CHANGE_OFF, USE_BUZ_TYPE);
            }
        }
        break;

    case SOLDERING_STATE_SLEEP:
        // 4. 低温 -> 深度休眠
        if (AllStatus_S.CurTemp < SLEEP_DEEP_TEMP_RANGE)
        {
            AllStatus_S.SolderingState = SOLDERING_STATE_SLEEP_DEEP;
        }
        break;

    case SOLDERING_STATE_SLEEP_DEEP:
        APP_SleepBackLight_Task();
        break;
    }
}

#define PWM_FILTER_ORDER 3                        // 滤波器阶数
#define PWM_FILTER_DEFAULT_RC_MS 500.0f           // RC时间常数（ms）
#define PWM_FILTER_DT_MS SLEEP_ADC_TASK_PERIOD_MS // 采样周期（ms）
/*
- 截止频率约 0.32Hz
- 截止点总衰减约 -9dB
*/
uint32_t APP_Sleep_PowerFilter(void)
{
    static float stage[PWM_FILTER_ORDER];
    static uint8_t initialized = 0;

    float input = AllStatus_S.Power;

    float rc_ms = PWM_FILTER_DEFAULT_RC_MS;
    if (rc_ms <= 0.0f)
        rc_ms = PWM_FILTER_DEFAULT_RC_MS;

    float dt = (float)PWM_FILTER_DT_MS;

    // alpha = dt / (RC + dt)
    float alpha = dt / (rc_ms + dt);
    if (alpha < 0.0f)
        alpha = 0.0f;
    else if (alpha > 1.0f)
        alpha = 1.0f;

    if (!initialized)
    {
        for (int i = 0; i < PWM_FILTER_ORDER; ++i)
            stage[i] = input;
        initialized = 1;
    }

    // 一阶RC的离散形式 x += alpha * (u - x)
    stage[0] += alpha * (input - stage[0]);
    for (int i = 1; i < PWM_FILTER_ORDER; ++i)
        stage[i] += alpha * (stage[i - 1] - stage[i]);

    float out = stage[PWM_FILTER_ORDER - 1];
    if (out < 0.0f)
        return 0u;
    return (uint32_t)(out + 0.5f);
}

/* #define SLEEP_BACKLIGHT_FLASH_PERIOD_MS 10 // 背光闪烁周期（ms）
#define SLEEP_BACKLIGHT_ON_TIME_MS 3       // 10ms周期内点亮时间（ms）

void APP_SleepBackLight_Task(void)
{
    static uint16_t ms_counter = 0;

    if (AllStatus_S.SolderingState == SOLDERING_STATE_SLEEP_DEEP &&
        AllStatus_S.flashSave_s.BackgroundLightOnoff)
    {
        ms_counter++;
        if (ms_counter >= SLEEP_BACKLIGHT_FLASH_PERIOD_MS)
            ms_counter = 0;
        if (ms_counter < SLEEP_BACKLIGHT_ON_TIME_MS)
            Drive_BackLed_OnOff(1); // 点亮背光
        else
            Drive_BackLed_OnOff(0); // 熄灭背光
    }
    else
    {
        ms_counter = 0;
        Drive_BackLed_OnOff(1); // 常亮背光
    }
} */

void APP_SleepBackLight_Task(void)
{
    if (AllStatus_S.SolderingState == SOLDERING_STATE_SLEEP_DEEP &&
        AllStatus_S.flashSave_s.BackgroundLightOnoff)
    {
        Drive_DisplayLcd_SetBrightnessLow();
    }
}

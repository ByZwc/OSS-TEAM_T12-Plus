#include "main.h"

// 进入设置模式
void app_joinSeting_Lcd(void)
{
#if USE_DISPLAY_TYPE == USE_DISPLAY_TYPE_HG1621
    Lcd_icon_onOff(icon_400Temp, 0);
    Lcd_icon_onOff(icon_350Temp, 0);
    Lcd_icon_onOff(icon_300Temp, 0);
    Lcd_SMG_DisplaySel(AllStatus_S.Seting.PNumber, 1, DispPNum);
    switch (AllStatus_S.Seting.PNumber)
    {
    case SMG_P01:
        Lcd_icon_onOff(icon_temp, 0); // 熄灭℃图标
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.BuzOnOff, 1, DispOnOff);
        break;
    case SMG_P02:
        Lcd_icon_onOff(icon_temp, 0); // 熄灭℃图标
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.PreinstallTempOnOff, 1, DispOnOff);
        break;
    case SMG_P03:
        Lcd_icon_onOff(icon_temp, 1); // 点亮℃图标
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.calibration_temp, 1, intVar);
        break;
    case SMG_P04:
        Lcd_icon_onOff(icon_temp, 0); // 熄灭℃图标
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.BackgroundLightOnoff, 1, DispOnOff);
        break;
    case SMG_P05:
        Lcd_icon_onOff(icon_temp, 0); // 熄灭℃图标
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.SleepDelayTime, 1, uintVar);
        break;
    case SMG_P06:
        Lcd_icon_onOff(icon_temp, 0); // 熄灭℃图标
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.DisplayPowerOnOff, 1, DispOnOff);
        break;
    }
#endif
}

// 退出设置模式
void app_exitSeting_Lcd(void)
{
#if USE_DISPLAY_TYPE == USE_DISPLAY_TYPE_HG1621
    Lcd_icon_onOff(icon_400Temp, 1);
    Lcd_icon_onOff(icon_350Temp, 1);
    Lcd_icon_onOff(icon_300Temp, 1);
#endif
    if (AllStatus_S.flashSave_s.PreinstallTempOnOff)
    {
        switch (AllStatus_S.flashSave_s.PreinstallTempNum)
        {
        case PREINSTALL_TMEP300:
            AllStatus_S.flashSave_s.TarTemp = 300;
            break;
        case PREINSTALL_TMEP350:
            AllStatus_S.flashSave_s.TarTemp = 350;
            break;
        case PREINSTALL_TMEP400:
            AllStatus_S.flashSave_s.TarTemp = 400;
            break;
        }
    }
    AllStatus_S.Seting.PNumber = 1;
    Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.TarTemp, 1, uintVar); // 恢复温度显示
    if (!AllStatus_S.flashSave_s.DisplayPowerOnOff)
        Lcd_icon_onOff(icon_temp, 1); // 点亮℃图标
}

// 编码器快速调温相关宏定义
#define ENCODER_FASTSET_WINDOW_MS 1000   // 统计窗口：1s
#define ENCODER_FASTSET_RESET_GAP_MS 800 // 间隔>800ms清零
#define ENCODER_FASTSET_THRESH_PER_SEC 3 // 每秒阈值：<=3为慢速
#define ENCODER_FASTSET_STEP_BASE 5      // 基础步进：5
#define ENCODER_FASTSET_MAX_LEVEL 4      // 加速等级数（对应步进：10/20/30/40）
#define ENCODER_FASTSET_STEP_MAX (ENCODER_FASTSET_STEP_BASE * ENCODER_FASTSET_MAX_LEVEL * 2)

uint8_t app_Encoder_FastSetTemp(void)
{
    static uint32_t last_tick = 0;   // 上次触发时间
    static uint32_t win_start = 0;   // 当前1s窗口起点
    static uint16_t win_count = 0;   // 当前窗口内触发次数
    static uint8_t fast_sec_cnt = 0; // 已完成的“快速秒”（>3次/秒）的连续计数

    uint32_t now = uwTick;

    if (last_tick == 0)
    {
        last_tick = now;
        win_start = now;
        win_count = 0;
        fast_sec_cnt = 0;
    }

    // 间隔大于复位阈值则清除记录
    if ((uint32_t)(now - last_tick) > ENCODER_FASTSET_RESET_GAP_MS)
    {
        win_start = now;
        win_count = 0;
        fast_sec_cnt = 0;
    }

    // 跨过1秒窗口，结算上一窗口
    if ((uint32_t)(now - win_start) >= ENCODER_FASTSET_WINDOW_MS)
    {
        if (win_count > ENCODER_FASTSET_THRESH_PER_SEC)
        {
            if (fast_sec_cnt < 255)
                fast_sec_cnt++; // 记录连续“快速秒”
        }
        else
        {
            fast_sec_cnt = 0; // <=3次/秒：清除记录
        }
        win_count = 0;
        win_start = now; // 以当前触发作为新窗口起点
    }

    // 计入当前窗口
    win_count++;
    last_tick = now;

    // 计算步进：
    // 无加速：5；从第二个“快速秒”开始：// 10,20,30,40
    uint8_t step = ENCODER_FASTSET_STEP_BASE;
    if (fast_sec_cnt > 0)
    {
        uint8_t lvl = fast_sec_cnt; // 1,2,3,4,...
        if (lvl > ENCODER_FASTSET_MAX_LEVEL)
            lvl = ENCODER_FASTSET_MAX_LEVEL;
        step = ENCODER_FASTSET_STEP_BASE * lvl * 2; // 10,20,30,40
    }
    if (step > ENCODER_FASTSET_STEP_MAX)
        step = ENCODER_FASTSET_STEP_MAX;

    return step;
}

// 普通模式或预设温度模式
void app_LcdCommonMode(uint8_t addOrSub)
{
    if (AllStatus_S.flashSave_s.PreinstallTempOnOff)
    {
        if (addOrSub)
        {
            if (AllStatus_S.flashSave_s.PreinstallTempNum < 3)
                AllStatus_S.flashSave_s.PreinstallTempNum++;
        }
        else
        {
            if (AllStatus_S.flashSave_s.PreinstallTempNum > 1)
                AllStatus_S.flashSave_s.PreinstallTempNum--;
        }
#if USE_DISPLAY_TYPE == USE_DISPLAY_TYPE_HG1621
        Lcd_icon_onOff(icon_400Temp, 1);
        Lcd_icon_onOff(icon_350Temp, 1);
        Lcd_icon_onOff(icon_300Temp, 1);
#endif
        AllStatus_S.flashSave_s.TarTemp = 300 + (AllStatus_S.flashSave_s.PreinstallTempNum - 1) * 50;
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.TarTemp, 1, uintVar); // 恢复温度显示
    }
    else
    {
        if (addOrSub)
        {
            if (AllStatus_S.flashSave_s.TarTemp < MAX_TAR_TEMP)
                AllStatus_S.flashSave_s.TarTemp += app_Encoder_FastSetTemp();
            if (AllStatus_S.flashSave_s.TarTemp > MAX_TAR_TEMP)
                AllStatus_S.flashSave_s.TarTemp = MAX_TAR_TEMP;
        }
        else
        {
            if (AllStatus_S.flashSave_s.TarTemp >= MIN_TAR_TEMP)
                AllStatus_S.flashSave_s.TarTemp -= app_Encoder_FastSetTemp();
            if (AllStatus_S.flashSave_s.TarTemp < MIN_TAR_TEMP - 5)
                AllStatus_S.flashSave_s.TarTemp = MIN_TAR_TEMP - 5;
        }
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.TarTemp, 1, uintVar); // 恢复温度显示
    }
}

// 设置模式，编码器左右转
void app_EncoderSetData_LcdSettingPage(uint8_t addOrSub)
{
    switch (AllStatus_S.Seting.PNumber)
    {
    case SMG_P01:
        if (addOrSub)
        {
            if (AllStatus_S.flashSave_s.ProtectTemp < PROTECT_TEMP_MAX)
                AllStatus_S.flashSave_s.ProtectTemp += app_Encoder_FastSetTemp();
            if (AllStatus_S.flashSave_s.ProtectTemp > PROTECT_TEMP_MAX)
                AllStatus_S.flashSave_s.ProtectTemp = PROTECT_TEMP_MAX;
        }
        else
        {
            if (AllStatus_S.flashSave_s.ProtectTemp > PROTECT_TEMP_MIN)
                AllStatus_S.flashSave_s.ProtectTemp -= app_Encoder_FastSetTemp();
            if (AllStatus_S.flashSave_s.ProtectTemp < PROTECT_TEMP_MIN)
                AllStatus_S.flashSave_s.ProtectTemp = PROTECT_TEMP_MIN;
        }
        break;
    case SMG_P02:
        if (addOrSub)
        {
            if (AllStatus_S.flashSave_s.StandbyTime < STANDBY_DELAY_TIME_MAX)
                AllStatus_S.flashSave_s.StandbyTime += app_Encoder_FastSetTemp();
            if (AllStatus_S.flashSave_s.StandbyTime > STANDBY_DELAY_TIME_MAX)
                AllStatus_S.flashSave_s.StandbyTime = STANDBY_DELAY_TIME_MAX;
        }
        else
        {
            if (AllStatus_S.flashSave_s.StandbyTime > STANDBY_DELAY_TIME_MIN)
                AllStatus_S.flashSave_s.StandbyTime -= app_Encoder_FastSetTemp();
            if (AllStatus_S.flashSave_s.StandbyTime > STANDBY_DELAY_TIME_MAX)
                AllStatus_S.flashSave_s.StandbyTime = STANDBY_DELAY_TIME_MIN;
        }
        break;
    case SMG_P03:
        if (addOrSub)
        {
            if (AllStatus_S.flashSave_s.SleepDelayTime < SLEEP_DELAY_TIME_MAX)
                AllStatus_S.flashSave_s.SleepDelayTime += app_Encoder_FastSetTemp() / 5;
            if (AllStatus_S.flashSave_s.SleepDelayTime > SLEEP_DELAY_TIME_MAX)
                AllStatus_S.flashSave_s.SleepDelayTime = SLEEP_DELAY_TIME_MAX;
        }
        else
        {
            if (AllStatus_S.flashSave_s.SleepDelayTime > SLEEP_DELAY_TIME_MIN)
                AllStatus_S.flashSave_s.SleepDelayTime -= app_Encoder_FastSetTemp() / 5;
            if (AllStatus_S.flashSave_s.SleepDelayTime > SLEEP_DELAY_TIME_MAX)
                AllStatus_S.flashSave_s.SleepDelayTime = SLEEP_DELAY_TIME_MIN;
        }
        break;
    case SMG_P04:
        if (addOrSub)
        {
            if (AllStatus_S.flashSave_s.KeepStrongTempTime < STRONG_WARM_TIME_MAX)
                AllStatus_S.flashSave_s.KeepStrongTempTime += app_Encoder_FastSetTemp();
            if (AllStatus_S.flashSave_s.KeepStrongTempTime > STRONG_WARM_TIME_MAX)
                AllStatus_S.flashSave_s.KeepStrongTempTime = STRONG_WARM_TIME_MAX;
        }
        else
        {
            if (AllStatus_S.flashSave_s.KeepStrongTempTime > STRONG_WARM_TIME_MIN)
                AllStatus_S.flashSave_s.KeepStrongTempTime -= app_Encoder_FastSetTemp();
            if (AllStatus_S.flashSave_s.KeepStrongTempTime < STRONG_WARM_TIME_MIN)
                AllStatus_S.flashSave_s.KeepStrongTempTime = STRONG_WARM_TIME_MIN;
        }
        break;
    case SMG_P05:
        if (AllStatus_S.flashSave_s.BuzOnOff)
        {
            AllStatus_S.flashSave_s.BuzOnOff = 0;
            Lcd_icon_onOff(icon_buzz, 0);
        }
        else
        {
            AllStatus_S.flashSave_s.BuzOnOff = 1;
            Lcd_icon_onOff(icon_buzz, 1);
        }
        break;
    case SMG_P06:
        if (addOrSub)
        {
            if (AllStatus_S.flashSave_s.calibration_temp < CALIBRATION_TEMP_MAX)
                AllStatus_S.flashSave_s.calibration_temp++;
        }
        else
        {
            if (AllStatus_S.flashSave_s.calibration_temp > (CALIBRATION_TEMP_MIN))
                AllStatus_S.flashSave_s.calibration_temp--;
        }
        break;
    case SMG_P07:
        if (AllStatus_S.flashSave_s.PreinstallTempOnOff)
            AllStatus_S.flashSave_s.PreinstallTempOnOff = 0;
        else
            AllStatus_S.flashSave_s.PreinstallTempOnOff = 1;
        break;
    case SMG_P08:
        if (AllStatus_S.flashSave_s.BackgroundLightOnoff)
            AllStatus_S.flashSave_s.BackgroundLightOnoff = 0;
        else
            AllStatus_S.flashSave_s.BackgroundLightOnoff = 1;
        break;
    case SMG_P09:
        if (AllStatus_S.flashSave_s.DisplayPowerOnOff)
            AllStatus_S.flashSave_s.DisplayPowerOnOff = 0;
        else
            AllStatus_S.flashSave_s.DisplayPowerOnOff = 1;
        break;
    }
}

/*
 * @brief 显示当前参数编号
 * @param addOrSub: 1-加，0-减
 * @return
 */
void app_Lcd_DisplayPNumber_SettingPage(uint8_t addOrSub)
{
    if (addOrSub)
    {
        AllStatus_S.Seting.PNumber++;
        if (AllStatus_S.Seting.PNumber > OPTION_NUM)
            AllStatus_S.Seting.PNumber = 1;
    }
    else
    {
        AllStatus_S.Seting.PNumber--;
        if (AllStatus_S.Seting.PNumber < 1)
            AllStatus_S.Seting.PNumber = OPTION_NUM;
    }

    // Lcd_SMG_DisplaySel(AllStatus_S.Seting.PNumber, 1, DispPNum);
    switch (AllStatus_S.Seting.PNumber)
    {
    case SMG_P01:
        Lcd_icon_onOff(icon_temp, 1);
        // Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.BuzOnOff, 1, DispOnOff);
        break;
    case SMG_P02:
        Lcd_icon_onOff(icon_temp, 0);
        // Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.PreinstallTempOnOff, 1, DispOnOff);
        break;
    case SMG_P03:
        Lcd_icon_onOff(icon_temp, 0);
        // Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.calibration_temp, 1, intVar);
        break;
    case SMG_P04:
        Lcd_icon_onOff(icon_temp, 1);
        // Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.BackgroundLightOnoff, 1, DispOnOff);
        break;
    case SMG_P05:
        Lcd_icon_onOff(icon_temp, 0);
        // Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.SleepDelayTime, 1, uintVar);
        break;
    case SMG_P06:
        Lcd_icon_onOff(icon_temp, 1);
        // Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.DisplayPowerOnOff, 1, DispOnOff);
        break;
    case SMG_P07:
        Lcd_icon_onOff(icon_temp, 0);
        break;
    case SMG_P08:
        Lcd_icon_onOff(icon_temp, 0);
        break;
    case SMG_P09:
        Lcd_icon_onOff(icon_temp, 0);
        break;
    }
}

// 宏定义：点亮时长（ms）、最快闪烁次数（Hz）、最慢闪烁次数（Hz）
#define SOLDER_ICON_ON_TIME_MS 100          // 点亮时长
#define SOLDER_ICON_BLINK_FAST_HZ 5         // 最快闪烁次数
#define SOLDER_ICON_BLINK_SLOW_HZ 0.25f     // 最慢闪烁次数
#define SOLDER_ICON_BLINK_PERIOD 100        // 调用周期（50ms）
void app_Lcd_SolderingIcon_Blink_Task(void) // 函数调用周期（50ms）
{
    static uint16_t blink_cnt = 0;
    static uint8_t icon_state = 0; // 0: off, 1: on

    if (AllStatus_S.pid_s.pid_out == 0 || AllStatus_S.pid_s.outCmd == 0)
    {
        blink_cnt = 0;
        icon_state = 0;
        Lcd_icon_onOff(icon_cup, 0); // 熄灭
        return;
    }

    // 计算占空比
    float duty = (float)AllStatus_S.pid_s.pid_out / (float)AllStatus_S.pid_s.pid_integration_max;
    if (duty > 1.0f)
        duty = 1.0f;
    if (duty < 0.0f)
        duty = 0.0f;

    // 计算周期（ms）
    float period_fast = 1000.0f / SOLDER_ICON_BLINK_FAST_HZ;
    float period_slow = 1000.0f / SOLDER_ICON_BLINK_SLOW_HZ;
    float period_ms = period_slow - (period_slow - period_fast) * duty;
    if (period_ms < period_fast)
        period_ms = period_fast;
    if (period_ms > period_slow)
        period_ms = period_slow;

    uint16_t on_cnt = SOLDER_ICON_ON_TIME_MS / SOLDER_ICON_BLINK_PERIOD;
    uint16_t off_cnt = (uint16_t)((period_ms - SOLDER_ICON_ON_TIME_MS) / SOLDER_ICON_BLINK_PERIOD);

    if (icon_state == 0) // off
    {
        blink_cnt++;
        if (blink_cnt >= off_cnt)
        {
            blink_cnt = 0;
            icon_state = 1;
            Lcd_icon_onOff(icon_cup, 1); // 点亮
        }
        else
        {
            Lcd_icon_onOff(icon_cup, 0); // 熄灭
        }
    }
    else // on
    {
        blink_cnt++;
        if (blink_cnt >= on_cnt)
        {
            blink_cnt = 0;
            icon_state = 0;
            Lcd_icon_onOff(icon_cup, 0); // 熄灭
        }
        else
        {
            Lcd_icon_onOff(icon_cup, 1); // 保持点亮
        }
    }
}

void APP_SolderingOpenStateCheck_Task(void) // 函数调用周期（500ms）
{
    if (AllStatus_S.SolderingState == SOLDERING_STATE_OK || AllStatus_S.SolderingState == SOLDERING_STATE_SHORTCIR_ERROR)
    {
        if (AllStatus_S.CurTemp >= SOLDERING_TEMP_OPEN)
        {
            AllStatus_S.SolderingState = SOLDERING_STATE_PULL_OUT_ERROR;
            Drive_Buz_OnOff(BUZ_1S, BUZ_FREQ_CHANGE_OFF, USE_BUZ_TYPE);
        }
    }
    else
    {
        if (AllStatus_S.SolderingState == SOLDERING_STATE_PULL_OUT_ERROR)
            APP_shortCircuitProtection();
    }
}

/* static void app_LcdBlink(uint8_t onOff)
{
    switch (AllStatus_S.flashSave_s.PreinstallTempNum)
    {
    case PREINSTALL_TMEP300:
        Lcd_icon_onOff(icon_300Temp, onOff);
        break;
    case PREINSTALL_TMEP350:
        Lcd_icon_onOff(icon_350Temp, onOff);
        break;
    case PREINSTALL_TMEP400:
        Lcd_icon_onOff(icon_400Temp, onOff);
        break;
    }
}

void app_IconBlink_Task(void)
{
    static uint8_t onOff = 0;
    onOff = !onOff;
    if ((!AllStatus_S.Seting.SetingPage) && (!(AllStatus_S.SolderingState == SOLDERING_STATE_STANDBY)) && (!(AllStatus_S.SolderingState == SOLDERING_STATE_STANDBY_DEEP)))
    {
        if (AllStatus_S.flashSave_s.PreinstallTempOnOff)
        {
            app_LcdBlink(onOff);
        }
        else
        {
            app_LcdBlink(1);
        }
    }
} */

static void app_SelBlink(uint8_t onOff)
{
    if (onOff == 0)
    {
        // 闪烁显示参数序号
        Lcd_SMG_DisplaySel(AllStatus_S.Seting.PNumber, 1, DispPNum);
        return;
    }

    switch (AllStatus_S.Seting.PNumber)
    {
    case SMG_P01: // 保护温度
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.ProtectTemp, 1, uintVar);
        break;
    case SMG_P02: // 待机模式时间（0=OFF）
        if (AllStatus_S.flashSave_s.StandbyTime > 0)
            Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.StandbyTime, 1, uintVar);
        else
            Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.StandbyTime, 1, DispOnOff);
        break;
    case SMG_P03: // 休眠模式时间（0=OFF）
        if (AllStatus_S.flashSave_s.SleepDelayTime > 0)
            Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.SleepDelayTime, 1, uintVar);
        else
            Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.SleepDelayTime, 1, DispOnOff);
        break;
    case SMG_P04: // 保温时间
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.KeepStrongTempTime, 1, uintVar);
        break;
    case SMG_P05: // 蜂鸣器
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.BuzOnOff, 1, DispOnOff);
        break;
    case SMG_P06: // 校准温度
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.calibration_temp, 1, intVar);
        break;
    case SMG_P07: // 预设温度功能开关
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.PreinstallTempOnOff, 1, DispOnOff);
        break;
    case SMG_P08: // 背光
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.BackgroundLightOnoff, 1, DispOnOff);
        break;
    case SMG_P09: // 功率/温度显示切换
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.DisplayPowerOnOff, 1, DispOnOff);
        break;
    default:
        Lcd_SMG_DisplaySel(AllStatus_S.Seting.PNumber, 1, DispPNum);
        break;
    }
}

#define SELBLINK_CALL_INTERVAL_MS 100 // 调用周期（100ms）
#define SELBLINK_PERIOD_SEC 2
#define SELBLINK_PERIOD_MS (SELBLINK_PERIOD_SEC * 1000)         // 闪烁周期
#define SELBLINK_OFF0_MS 1000                                   // onOff=0 初始保持时间
#define SELBLINK_ON1_MS (SELBLINK_PERIOD_MS - SELBLINK_OFF0_MS) // onOff=1 保持时间
#if (SELBLINK_OFF0_MS + SELBLINK_ON1_MS) > SELBLINK_PERIOD_MS
#error "SELBLINK_OFF0_MS + SELBLINK_ON1_MS 不能大于 SELBLINK_PERIOD_MS"
#endif
#define SELBLINK_NOCHANGE_TIMEOUT_MS (SELBLINK_PERIOD_MS) // 2秒内无变化才开始闪烁
#define SELBLINK_OPTIONCHANGE_FORCE_ON 1                  // OptionChange变化时强制显示参数
#define SELBLINK_PNUMBER_CHANGE_SHOW_INDEX 1              // PNumber变化时立即显示序号(onOff=0)
#define PNUMBER_CHANGE_HOLD_MS 1000                       // 参数编号变化后保持显示序号的时间（ms）
void app_SelBlink_Task(void)
{
    static uint32_t elapsed_ms = 0;             // 闪烁计时（进入闪烁周期后）
    static uint16_t last_pnumber = 0xFFFF;      // 上次参数编号
    static uint32_t last_optionchange = 0;      // 上次 OptionChange 值
    static uint32_t optionchange_stable_ms = 0; // OptionChange 未变化累计时间(ms)
    static uint8_t pnum_hold_active = 0;        // 1=处于参数编号变化后的保持阶段
    static uint32_t pnum_hold_elapsed = 0;      // 已保持时间

    if (!AllStatus_S.Seting.SetingPage)
    {
        elapsed_ms = 0;
        last_pnumber = 0xFFFF;
        optionchange_stable_ms = 0;
        last_optionchange = AllStatus_S.Seting.OptionChange;
        pnum_hold_active = 0;
        pnum_hold_elapsed = 0;
        return;
    }

    // 参数编号变化：进入“保持显示序号1秒”阶段
    if (last_pnumber != AllStatus_S.Seting.PNumber)
    {
        last_pnumber = AllStatus_S.Seting.PNumber;
        elapsed_ms = 0; // 闪烁周期计时复位（真正开始闪烁要等保持阶段结束）
        optionchange_stable_ms = 0;
        last_optionchange = AllStatus_S.Seting.OptionChange;
        pnum_hold_active = 1;
        pnum_hold_elapsed = 0;
        app_SelBlink(0); // 立即显示序号
        return;
    }

    // 仍在“参数编号变化后保持阶段”
    if (pnum_hold_active)
    {
        app_SelBlink(0); // 始终显示序号
        pnum_hold_elapsed += SELBLINK_CALL_INTERVAL_MS;
        if (pnum_hold_elapsed >= PNUMBER_CHANGE_HOLD_MS)
        {
            // 保持结束，进入正常闪烁逻辑，先清零相关计时
            pnum_hold_active = 0;
            pnum_hold_elapsed = 0;
            elapsed_ms = SELBLINK_OFF0_MS / 2;
            optionchange_stable_ms = 0;
        }
        return;
    }

    // OptionChange 有变化：强制显示当前参数值(onOff=1)，并重新计时（此时已不在保持阶段）
    if (last_optionchange != AllStatus_S.Seting.OptionChange)
    {
        last_optionchange = AllStatus_S.Seting.OptionChange;
        optionchange_stable_ms = 0;
        elapsed_ms = 0;  // 重新进入前置显示阶段（随后按 2 秒无操作再闪烁）
        app_SelBlink(1); // 显示参数内容
        return;
    }

    // 累计 OptionChange 未变化时间
    optionchange_stable_ms += SELBLINK_CALL_INTERVAL_MS;

    // 2秒内有过变化（或尚未满2秒稳定）：持续显示参数内容，不闪烁
    if (optionchange_stable_ms < SELBLINK_NOCHANGE_TIMEOUT_MS)
    {
        app_SelBlink(1);
        return;
    }

    // 进入闪烁周期
    if (elapsed_ms < SELBLINK_OFF0_MS)
    {
        app_SelBlink(0);
    }
    else if (elapsed_ms < (SELBLINK_OFF0_MS + SELBLINK_ON1_MS))
    {
        app_SelBlink(1);
    }
    else
    {
        app_SelBlink(0);
    }

    elapsed_ms += SELBLINK_CALL_INTERVAL_MS;
    if (elapsed_ms >= SELBLINK_PERIOD_MS)
        elapsed_ms = 0;
}

// 编码器调节目标温度显示相关宏
#define CODERING_CHANGE_DISPLAY_CALL_INTERVAL_MS 100 // 函数调用周期(ms)
#define CODERING_CHANGE_DISPLAY_HOLD_MS 2000         // 设定温度保持显示时长(ms)

void app_CoderingChangeTarTemp_display(void)
{
    // 函数调用周期：CODERING_CHANGE_DISPLAY_CALL_INTERVAL_MS
    // CommonModeChange 变化后显示设定温度保持 CODERING_CHANGE_DISPLAY_HOLD_MS 毫秒，否则显示当前实时温度
    static uint32_t last_common_change = 0;
    static uint16_t hold_ms = 0;    // 已保持时间
    static uint8_t hold_active = 0; // 1=保持显示设定温度

    if (last_common_change != AllStatus_S.Seting.CommonModeChange)
    {
        last_common_change = AllStatus_S.Seting.CommonModeChange;
        hold_ms = 0;
        hold_active = 1;
    }

    if (hold_active)
    {
        Lcd_SMG_DisplaySel(AllStatus_S.flashSave_s.TarTemp, 1, uintVar);
        hold_ms += CODERING_CHANGE_DISPLAY_CALL_INTERVAL_MS;
        if (hold_ms >= CODERING_CHANGE_DISPLAY_HOLD_MS)
        {
            hold_active = 0;
        }
    }
    else
    {
        Lcd_SMG_DisplaySel((uint16_t)AllStatus_S.data_filter_prev[SOLDERING_TEMP210_NUM], 1, uintVar);
    }
}

void app_SolderingTempDisplay(void)
{
    static uint32_t last_display_temp = 0.0f; // 睡眠防抖动

    static uint32_t display_temp_cnt = 0;
    static uint32_t display_temp_interval = 0;
    static uint32_t last_display_temp_tick = 0;

    static uint32_t last_commonmodechange = 0;
    static uint8_t fast_refresh_active = 0;
    static uint32_t fast_refresh_start_tick = 0;
    static float32_t diff = 0.0f;
    const uint32_t FAST_REFRESH_HOLD_MS = CODERING_CHANGE_DISPLAY_HOLD_MS;

    last_display_temp_tick++;

    if (last_commonmodechange != AllStatus_S.Seting.CommonModeChange)
    {
        last_commonmodechange = AllStatus_S.Seting.CommonModeChange;
        fast_refresh_active = 1;
        fast_refresh_start_tick = uwTick;
    }

    if (fast_refresh_active)
    {
        display_temp_interval = 1;
        if ((uint32_t)(uwTick - fast_refresh_start_tick) >= FAST_REFRESH_HOLD_MS)
            fast_refresh_active = 0;
    }
    else
    {
        if (AllStatus_S.OneState_TempOk &&
            ((uint32_t)AllStatus_S.pid_s.pid_out < AllStatus_S.pid_s.outPriod_max))
            display_temp_interval = 10;
        else
            display_temp_interval = 1;
    }

    if (last_display_temp_tick - display_temp_cnt >= display_temp_interval)
    {
        display_temp_cnt += display_temp_interval;
        if (!AllStatus_S.Seting.SetingPage)
        {
            switch (AllStatus_S.SolderingState)
            {
            case SOLDERING_STATE_SHORTCIR_ERROR: // 短路
                Lcd_SMG_DisplaySel(ERROR_E0, 1, DispErrorNum);
                AllStatus_S.OneState_TempOk = 0;
                break;
            case SOLDERING_STATE_PULL_OUT_ERROR: // 拔出手柄
                Lcd_SMG_DisplaySel(ERROR_E3, 1, DispErrorNum);
                AllStatus_S.OneState_TempOk = 0;
                break;
            case SOLDERING_STATE_STANDBY: // 进入待机
                if (AllStatus_S.flashSave_s.StandbyTime)
                {
                    if (AllStatus_S.Old_TarTemp > AllStatus_S.flashSave_s.ProtectTemp)
                        AllStatus_S.flashSave_s.TarTemp = AllStatus_S.flashSave_s.ProtectTemp;
                    diff = fabsf(AllStatus_S.data_filter_prev[SOLDERING_TEMP210_NUM] - (float32_t)AllStatus_S.flashSave_s.TarTemp);
                    Lcd_SMG_DisplaySel((uint16_t)AllStatus_S.data_filter_prev[SOLDERING_TEMP210_NUM], 1, uintVar);
                    if (diff < 1.0f) // 首次到达温度蜂鸣器响应
                    {
                        if (!AllStatus_S.OneState_TempOk)
                            AllStatus_S.OneState_TempOk = 1;
                    }
                }
                break;
            case SOLDERING_STATE_SLEEP: // 进入休眠
                if ((uint32_t)AllStatus_S.data_filter_prev[SOLDERING_TEMP210_NUM] < last_display_temp)
                {
                    Lcd_SMG_DisplaySel((uint16_t)AllStatus_S.data_filter_prev[SOLDERING_TEMP210_NUM], 1, uintVar);
                    last_display_temp = AllStatus_S.data_filter_prev[SOLDERING_TEMP210_NUM];
                }
                AllStatus_S.OneState_TempOk = 0;
                break;
            case SOLDERING_STATE_SLEEP_DEEP: // 进入深睡眠
                Lcd_SMG_DisplaySel(DRIVE_SLEEP, 1, DispErrorNum);
                AllStatus_S.OneState_TempOk = 0;
                break;
            case SOLDERING_STATE_OK: // 正常状态
                diff = fabsf(AllStatus_S.data_filter_prev[SOLDERING_TEMP210_NUM] - (float32_t)AllStatus_S.flashSave_s.TarTemp);
                // Lcd_SMG_DisplaySel(AllStatus_S.adc_filter_value, 1, uintVar);    //温度原始ADC值，带FIR滤波
                // Lcd_SMG_DisplaySel((uint16_t)AllStatus_S.data_filter[SOLDERING_ELECTRICITY_NUM], 1, uintVar);    //估计值
                // Lcd_SMG_DisplaySel((uint16_t)AllStatus_S.data_filter_prev[SOLDERING_ELECTRICITY_NUM], 1, uintVar);   // 实时值
                // Lcd_SMG_DisplaySel((uint16_t)AllStatus_S.Power, 1, uintVar); // 功率值(互补滤波)
                // Lcd_SMG_DisplaySel(AllStatus_S.adc_value[SLEEP_NUM], 1, uintVar); // 睡眠ADC值
                // Lcd_SMG_DisplaySel((uint16_t)AllStatus_S.Power, 1, uintVar);

                app_CoderingChangeTarTemp_display(); // 编码器调节目标温度显示

                if (diff < 1.0f) // 首次到达温度蜂鸣器响应
                {
                    if (!AllStatus_S.OneState_TempOk)
                        Drive_Buz_OnOff(BUZ_20MS, BUZ_FREQ_CHANGE_OFF, USE_BUZ_TYPE);
                    AllStatus_S.OneState_TempOk = 1;
                }
                last_display_temp = MAX_TAR_TEMP;
                break;
            }
        }
    }
}

void APP_SleepCloseBackLight_Task(void)
{
    if (AllStatus_S.SolderingState == SOLDERING_STATE_SLEEP_DEEP)
    {
        if (AllStatus_S.sleep_cnt < 61)
            AllStatus_S.sleep_cnt++;

        if (AllStatus_S.sleep_cnt == TIME_TO_CLOSE_BACKLIGHT)
        {
            if (AllStatus_S.flashSave_s.BackgroundLightOnoff)
            {
                Drive_BackLed_OnOff(0); // 关闭背光
                Drive_Buz_OnOff(BUZ_1S, BUZ_FREQ_CHANGE_OFF, USE_BUZ_TYPE);
            }
        }
    }
    else
    {
        AllStatus_S.sleep_cnt = 0;
    }
}

static void app_OneKeyStrongTemp_Blink(void)
{
    static uint8_t onOff = 0;
    onOff = !onOff;
    Lcd_icon_onOff(icon_temp, onOff);
}

void APP_OneKeyStrongTemp_Task(void)
{
    static uint8_t active = 0;   // 1=已进入强温提升阶段
    static uint16_t elapsed = 0; // 已维持的秒数（函数1s调用一次）

    if (AllStatus_S.encoder_s.OneKeyStrongTemp)
    {
        if (!active)
        {
            // 若当前目标温度已超过最大强温限制，则退出强温模式
            if (AllStatus_S.flashSave_s.TarTemp > MAX_STRONG_TEMP)
            {
                AllStatus_S.encoder_s.OneKeyStrongTemp = 0;
                active = 0;
                elapsed = 0;
                return;
            }
            AllStatus_S.flashSave_s.TarTemp += 50;
            AllStatus_S.OneState_TempOk = 0;
            active = 1;
            elapsed = 0;
        }
        else
        {
            elapsed++;
            app_OneKeyStrongTemp_Blink(); // 图标闪烁
            if (elapsed >= AllStatus_S.flashSave_s.KeepStrongTempTime)
            {
                if (AllStatus_S.flashSave_s.TarTemp >= 50)
                    AllStatus_S.flashSave_s.TarTemp -= 50;
                AllStatus_S.encoder_s.OneKeyStrongTemp = 0;
                AllStatus_S.OneState_TempOk = 0;
                active = 0;
                elapsed = 0;
                Lcd_icon_onOff(icon_temp, 1); // 退出强温提升，常亮
            }
        }
    }
    else
    {
        if (active)
        {
            if (AllStatus_S.flashSave_s.TarTemp >= 50)
                AllStatus_S.flashSave_s.TarTemp -= 50;
            active = 0;
            elapsed = 0;
        }
    }
}

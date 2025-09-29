#include "main.h"

const uint8_t smg_hexArray[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
#if USE_DISPLAY_TYPE == USE_DISPLAY_TYPE_HG1621
uint8_t Lcd_smgArray_Up[3];
uint8_t Lcd_smgArray_Dowm[3];

static void Lcd_icon_400Temp_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[6] |= 0x01;
    else
        displayMemory[6] &= (~0x01);
}

static void Lcd_icon_350Temp_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[6] |= 0x02;
    else
        displayMemory[6] &= (~0x02);
}

static void Lcd_icon_300Temp_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[6] |= 0x04;
    else
        displayMemory[6] &= (~0x04);
}

static void Lcd_icon_SET_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[6] |= 0x08;
    else
        displayMemory[6] &= (~0x08);
}

static void Lcd_icon_soldering_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[7] |= 0x08;
    else
        displayMemory[7] &= (~0x08);
}

static void Lcd_icon_cup_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[9] |= 0x08;
    else
        displayMemory[9] &= (~0x08);
}

static void Lcd_icon_temp_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[11] |= 0x08;
    else
        displayMemory[11] &= (~0x08);
}

void Lcd_icon_onOff(LCD_ICON_TYPE lcdIcon, uint8_t onOff)
{
    switch (lcdIcon)
    {
    case icon_400Temp:
        Lcd_icon_400Temp_onOff(onOff);
        break;
    case icon_350Temp:
        Lcd_icon_350Temp_onOff(onOff);
        break;
    case icon_300Temp:
        Lcd_icon_300Temp_onOff(onOff);
        break;
    case icon_SET:
        Lcd_icon_SET_onOff(onOff);
        break;
    case icon_soldering:
        Lcd_icon_soldering_onOff(onOff);
        break;
    case icon_cup:
        Lcd_icon_cup_onOff(onOff);
        break;
    case icon_temp:
        Lcd_icon_temp_onOff(onOff);
        break;
    }
}

static void Lcd_RawArrayTurnForVideoMemory_Dowm3(void)
{

    displayMemory[8] = 0;
    displayMemory[10] = 0;
    displayMemory[12] = 0;
    displayMemory[7] &= 0x08;
    displayMemory[9] &= 0x08;
    displayMemory[11] &= 0x08;

    // dowm1
    displayMemory[8] |= (Lcd_smgArray_Dowm[0] & 0x01) << 3; // a
    displayMemory[8] |= (Lcd_smgArray_Dowm[0] & 0x02) << 1; // b
    displayMemory[8] |= (Lcd_smgArray_Dowm[0] & 0x04) >> 1; // c
    displayMemory[8] |= (Lcd_smgArray_Dowm[0] & 0x08) >> 3; // d

    displayMemory[7] |= (Lcd_smgArray_Dowm[0] & 0x10) >> 4; // e
    displayMemory[7] |= (Lcd_smgArray_Dowm[0] & 0x20) >> 3; // f
    displayMemory[7] |= (Lcd_smgArray_Dowm[0] & 0x40) >> 5; // g

    // dowm2
    displayMemory[10] |= (Lcd_smgArray_Dowm[1] & 0x01) << 3; // a
    displayMemory[10] |= (Lcd_smgArray_Dowm[1] & 0x02) << 1; // b
    displayMemory[10] |= (Lcd_smgArray_Dowm[1] & 0x04) >> 1; // c
    displayMemory[10] |= (Lcd_smgArray_Dowm[1] & 0x08) >> 3; // d

    displayMemory[9] |= (Lcd_smgArray_Dowm[1] & 0x10) >> 4; // e
    displayMemory[9] |= (Lcd_smgArray_Dowm[1] & 0x20) >> 3; // f
    displayMemory[9] |= (Lcd_smgArray_Dowm[1] & 0x40) >> 5; // g

    // dowm3
    displayMemory[12] |= (Lcd_smgArray_Dowm[2] & 0x01) << 3; // a
    displayMemory[12] |= (Lcd_smgArray_Dowm[2] & 0x02) << 1; // b
    displayMemory[12] |= (Lcd_smgArray_Dowm[2] & 0x04) >> 1; // c
    displayMemory[12] |= (Lcd_smgArray_Dowm[2] & 0x08) >> 3; // d

    displayMemory[11] |= (Lcd_smgArray_Dowm[2] & 0x10) >> 4; // e
    displayMemory[11] |= (Lcd_smgArray_Dowm[2] & 0x20) >> 3; // f
    displayMemory[11] |= (Lcd_smgArray_Dowm[2] & 0x40) >> 5; // g
}

static void Lcd_RawArrayTurnForVideoMemory_Up3(void)
{
    displayMemory[4] = 0;
    displayMemory[2] = 0;
    displayMemory[0] = 0;
    displayMemory[1] = 0;
    displayMemory[3] = 0;
    displayMemory[5] = 0;

    // up1
    displayMemory[4] = (Lcd_smgArray_Up[0] & 0x0f); // abcd

    displayMemory[5] |= (Lcd_smgArray_Up[0] & 0x10) >> 2; // e
    displayMemory[5] |= (Lcd_smgArray_Up[0] & 0x20) >> 5; // f
    displayMemory[5] |= (Lcd_smgArray_Up[0] & 0x40) >> 5; // g

    // up2
    displayMemory[2] = (Lcd_smgArray_Up[1] & 0x0f); // abcd

    displayMemory[3] |= (Lcd_smgArray_Up[1] & 0x10) >> 2; // e
    displayMemory[3] |= (Lcd_smgArray_Up[1] & 0x20) >> 5; // f
    displayMemory[3] |= (Lcd_smgArray_Up[1] & 0x40) >> 5; // g

    // up3
    displayMemory[0] = (Lcd_smgArray_Up[2] & 0x0f); // abcd

    displayMemory[1] |= (Lcd_smgArray_Up[2] & 0x10) >> 2; // e
    displayMemory[1] |= (Lcd_smgArray_Up[2] & 0x20) >> 5; // f
    displayMemory[1] |= (Lcd_smgArray_Up[2] & 0x40) >> 5; // g
}

static void Lcd_smgDowm3_Close(void)
{
    Lcd_smgArray_Dowm[0] = 0;
    Lcd_smgArray_Dowm[1] = 0;
    Lcd_smgArray_Dowm[2] = 0;
}

static void Lcd_SetNum_D3(uint16_t num)
{
    uint8_t ge, shi, bai;
    if (num > 999)
        num = 999;

    ge = num % 10;
    shi = num / 10 % 10;
    bai = num / 100 % 10;
    Lcd_smgArray_Dowm[1] = 0;
    Lcd_smgArray_Dowm[0] = 0;

    Lcd_smgArray_Dowm[2] = smg_hexArray[ge];
    if (bai || shi) // 有百位或十位时，十位显示
        Lcd_smgArray_Dowm[1] = smg_hexArray[shi];
    if (bai)
        Lcd_smgArray_Dowm[0] = smg_hexArray[bai];
}

void Lcd_smgDowm3_SetNum(uint16_t num, uint8_t OnOff)
{
    if (OnOff)
    {
        Lcd_SetNum_D3(num);
    }
    else
    {
        Lcd_smgDowm3_Close();
    }

    Lcd_RawArrayTurnForVideoMemory_Dowm3();
}

void Lcd_smgDowm3_SetHex(uint16_t num)
{
    uint8_t ge, shi, bai;
    ge = num & 0x0f;
    shi = (num >> 4) & 0x0f;
    bai = (num >> 8) & 0x0f;

    Lcd_smgArray_Dowm[2] = smg_hexArray[ge];
    Lcd_smgArray_Dowm[1] = smg_hexArray[shi];
    Lcd_smgArray_Dowm[0] = smg_hexArray[bai];

    Lcd_RawArrayTurnForVideoMemory_Dowm3();
}

static void Lcd_DisplayOnOff_D3(uint8_t OnOff_Icon)
{
    if (!OnOff_Icon)
    {
        Lcd_smgArray_Dowm[0] = smg_hexArray[0];
        Lcd_smgArray_Dowm[1] = smg_hexArray[0x0f];
        Lcd_smgArray_Dowm[2] = smg_hexArray[0x0f];
    }
    else
    {
        Lcd_smgArray_Dowm[0] = 0;
        Lcd_smgArray_Dowm[1] = smg_hexArray[0];
        Lcd_smgArray_Dowm[2] = 0x37; // 大n
    }
}

void Lcd_smgDowm3_DisplayOnOff(uint8_t OnOff_Icon, uint8_t OnOff)
{
    if (OnOff)
    {
        Lcd_DisplayOnOff_D3(OnOff_Icon);
    }
    else
    {
        Lcd_smgDowm3_Close();
    }

    Lcd_RawArrayTurnForVideoMemory_Dowm3();
}

static void Lcd_SetNimus_D3(int16_t Nimus)
{
    uint8_t ge, shi;
    if (Nimus >= 0)
    {
        Lcd_smgDowm3_SetNum(Nimus, 1);
    }
    else
    {
        Nimus = abs(Nimus);
        if (Nimus > 99)
            Nimus = 99;
        ge = Nimus % 10;
        shi = Nimus / 10 % 10;
        Lcd_smgArray_Dowm[0] = 0x40;
        Lcd_smgArray_Dowm[1] = smg_hexArray[shi];
        Lcd_smgArray_Dowm[2] = smg_hexArray[ge];
    }
}

void Lcd_smgDowm3_SetNimus(int16_t Nimus, uint8_t OnOff)
{
    if (OnOff)
    {
        if (Nimus == 0)
        {
            Lcd_smgArray_Dowm[0] = smg_hexArray[0];
            Lcd_smgArray_Dowm[1] = smg_hexArray[0];
            Lcd_smgArray_Dowm[2] = smg_hexArray[0];
        }
        else
        {
            Lcd_SetNimus_D3(Nimus);
        }
    }
    else
    {
        Lcd_smgDowm3_Close();
    }

    Lcd_RawArrayTurnForVideoMemory_Dowm3();
}

void Lcd_smgDowm3_SetErrorNum(int16_t ErrorNum, uint8_t OnOff)
{
    if (OnOff)
    {
        switch (ErrorNum)
        {
        case ERROR_E0:
            Lcd_smgArray_Dowm[0] = 0;
            Lcd_smgArray_Dowm[1] = smg_hexArray[0x0E];
            Lcd_smgArray_Dowm[2] = smg_hexArray[ERROR_E0];
            break;
        case ERROR_E1:
            Lcd_smgArray_Dowm[0] = 0;
            Lcd_smgArray_Dowm[1] = smg_hexArray[0x0E];
            Lcd_smgArray_Dowm[2] = smg_hexArray[ERROR_E1];
            break;
        case ERROR_E2:
            Lcd_smgArray_Dowm[0] = 0;
            Lcd_smgArray_Dowm[1] = smg_hexArray[0x0E];
            Lcd_smgArray_Dowm[2] = smg_hexArray[ERROR_E2];
            break;
        case ERROR_E3: // S-E
            /* Lcd_smgArray_Dowm[0] = 0;
            Lcd_smgArray_Dowm[1] = smg_hexArray[0x0E];
            Lcd_smgArray_Dowm[2] = smg_hexArray[ERROR_E3]; */
            Lcd_smgArray_Dowm[0] = 0x6D;
            Lcd_smgArray_Dowm[1] = 0x40;
            Lcd_smgArray_Dowm[2] = smg_hexArray[0x0E];
            break;
        case ERROR_SYSTEM_INIT:
            Lcd_smgArray_Dowm[0] = 0;
            Lcd_smgArray_Dowm[1] = smg_hexArray[0x0E];
            Lcd_smgArray_Dowm[2] = smg_hexArray[ERROR_SYSTEM_INIT];
            break;
        case DRIVE_SLEEP:
            Lcd_smgArray_Dowm[0] = 0x40;
            Lcd_smgArray_Dowm[1] = 0x40;
            Lcd_smgArray_Dowm[2] = 0x40;
            break;
        }
    }
    else
    {
        Lcd_smgDowm3_Close();
    }

    Lcd_RawArrayTurnForVideoMemory_Dowm3();
}

void Lcd_smgUp3_SetNum(uint16_t num)
{
    uint8_t ge, shi, bai;
    ge = num % 10;
    shi = num / 10 % 10;
    bai = num / 100 % 10;

    Lcd_smgArray_Up[2] = smg_hexArray[ge];
    Lcd_smgArray_Up[1] = smg_hexArray[shi];
    Lcd_smgArray_Up[0] = smg_hexArray[bai];

    Lcd_RawArrayTurnForVideoMemory_Up3();
}

void Lcd_smgUp3_SetPNum(uint16_t Number)
{
    if (Number > 9)
        Number = 9;
    Lcd_smgArray_Up[2] = smg_hexArray[Number];
    Lcd_smgArray_Up[1] = smg_hexArray[0];
    Lcd_smgArray_Up[0] = 0x73;
    Lcd_RawArrayTurnForVideoMemory_Up3();
}

void Drive_Lcd_AllIcon_init(void)
{
    uint8_t i;
    for (i = 0; i < ALL_SEG_NUM; i++)
        displayMemory[i] = 0xff;

    if (AllStatus_S.flashSave_s.PreinstallTempOnOff)
    {
        switch (AllStatus_S.flashSave_s.PreinstallTempNum)
        {
        case PREINSTALL_TMEP300:
            AllStatus_S.flashSave_s.TarTemp = 300;
            Lcd_smgUp3_SetNum(AllStatus_S.flashSave_s.TarTemp); // 恢复温度显示
            break;
        case PREINSTALL_TMEP350:
            AllStatus_S.flashSave_s.TarTemp = 350;
            Lcd_smgUp3_SetNum(AllStatus_S.flashSave_s.TarTemp); // 恢复温度显示
            break;
        case PREINSTALL_TMEP400:
            AllStatus_S.flashSave_s.TarTemp = 400;
            Lcd_smgUp3_SetNum(AllStatus_S.flashSave_s.TarTemp); // 恢复温度显示
            break;
        }
    }
    else
    {
        Lcd_smgUp3_SetNum(AllStatus_S.flashSave_s.TarTemp); // 恢复温度显示
    }
    Lcd_smgDowm3_SetNum(0, 1);
    AllStatus_S.Seting.PNumber = 1;
    Lcd_icon_onOff(icon_soldering, 0);
    if (AllStatus_S.flashSave_s.DisplayPowerOnOff)
        Lcd_icon_onOff(icon_temp, 0); // 熄灭℃图标
    Drive_DisplayLcd_sendData_Task();
}
#endif

#if USE_DISPLAY_TYPE == USE_DISPLAY_TYPE_AIP650
static void Lcd_icon_soldering_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[2] |= 0x80;
    else
        displayMemory[2] &= (~0x80);
}

static void Lcd_icon_temp_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[0] |= 0x80;
    else
        displayMemory[0] &= (~0x80);
}

static void Lcd_icon_buzz_onOff(uint8_t onOff)
{
    if (onOff)
        displayMemory[1] |= 0x80;
    else
        displayMemory[1] &= (~0x80);
}

void Lcd_icon_onOff(LCD_ICON_TYPE lcdIcon, uint8_t onOff)
{
    switch (lcdIcon)
    {
    case icon_soldering:
        Lcd_icon_soldering_onOff(onOff);
        break;
    case icon_temp:
        Lcd_icon_temp_onOff(onOff);
        break;
    case icon_buzz:
        Lcd_icon_buzz_onOff(onOff);
        break;
    }
}

void Lcd_SMG_DisplaySel(int32_t temp, uint8_t OnOff, LCD_SMG_DISPLAY_TYPE lcdSmgDisplayType)
{
    switch (lcdSmgDisplayType)
    {
    case uintVar:
        Lcd_smgDowm3_SetNum(temp, OnOff);
        break;
    case intVar:
        Lcd_smgDowm3_SetNimus(temp, OnOff);
        break;
    case uintHex:
        Lcd_smgDowm3_SetHex(temp);
        break;
    case DispOnOff:
        Lcd_smgDowm3_DisplayOnOff(temp, OnOff);
        break;
    case DispPNum:
        Lcd_smgUp3_SetPNum(temp);
        break;
    case DispErrorNum:
        Lcd_smgDowm3_SetErrorNum(temp, OnOff);
        break;
    }
}

static void Lcd_smgDowm3_Close(void)
{
    displayMemory[0] &= 0x80;
    displayMemory[1] &= 0x80;
    displayMemory[2] &= 0x80;
}

static void Lcd_SetNum_D3(uint16_t num)
{
    uint8_t ge, shi, bai;
    if (num > 999)
        num = 999;

    ge = num % 10;
    shi = num / 10 % 10;
    bai = num / 100 % 10;
    Lcd_smgDowm3_Close();

    displayMemory[2] |= smg_hexArray[ge];
    if (bai || shi) // 有百位或十位时，十位显示
        displayMemory[1] |= smg_hexArray[shi];
    if (bai)
        displayMemory[0] |= smg_hexArray[bai];
}

void Lcd_smgDowm3_SetNum(uint16_t num, uint8_t OnOff)
{
    if (OnOff)
    {
        Lcd_SetNum_D3(num);
    }
    else
    {
        Lcd_smgDowm3_Close();
    }
}

void Lcd_smgDowm3_SetHex(uint16_t num)
{
    uint8_t ge, shi, bai;
    ge = num & 0x0f;
    shi = (num >> 4) & 0x0f;
    bai = (num >> 8) & 0x0f;
    Lcd_smgDowm3_Close();
    displayMemory[2] |= smg_hexArray[ge];
    displayMemory[1] |= smg_hexArray[shi];
    displayMemory[0] |= smg_hexArray[bai];
}

static void Lcd_DisplayOnOff_D3(uint8_t OnOff_Icon)
{
    Lcd_smgDowm3_Close();
    if (!OnOff_Icon)
    {
        displayMemory[0] |= smg_hexArray[0];
        displayMemory[1] |= smg_hexArray[0x0f];
        displayMemory[2] |= smg_hexArray[0x0f];
    }
    else
    {
        displayMemory[0] |= 0;
        displayMemory[1] |= smg_hexArray[0];
        displayMemory[2] |= 0x37; // 大n
    }
}

void Lcd_smgDowm3_DisplayOnOff(uint8_t OnOff_Icon, uint8_t OnOff)
{
    if (OnOff)
    {
        Lcd_DisplayOnOff_D3(OnOff_Icon);
    }
    else
    {
        Lcd_smgDowm3_Close();
    }
}

static void Lcd_SetNimus_D3(int16_t Nimus)
{
    uint8_t ge, shi;
    if (Nimus >= 0)
    {
        Lcd_smgDowm3_SetNum(Nimus, 1);
    }
    else
    {
        Nimus = abs(Nimus);
        if (Nimus > 99)
            Nimus = 99;
        ge = Nimus % 10;
        shi = Nimus / 10 % 10;
        Lcd_smgDowm3_Close();
        displayMemory[0] |= 0x40;
        displayMemory[1] |= smg_hexArray[shi];
        displayMemory[2] |= smg_hexArray[ge];
    }
}

void Lcd_smgDowm3_SetNimus(int16_t Nimus, uint8_t OnOff)
{
    if (OnOff)
    {
        if (Nimus == 0)
        {
            Lcd_smgDowm3_Close();
            displayMemory[0] |= smg_hexArray[0];
            displayMemory[1] |= smg_hexArray[0];
            displayMemory[2] |= smg_hexArray[0];
        }
        else
        {
            Lcd_SetNimus_D3(Nimus);
        }
    }
    else
    {
        Lcd_smgDowm3_Close();
    }
}

void Lcd_smgUp3_SetNum(uint16_t num)
{
    uint8_t ge, shi, bai;
    ge = num % 10;
    shi = num / 10 % 10;
    bai = num / 100 % 10;
    Lcd_smgDowm3_Close();
    displayMemory[2] |= smg_hexArray[ge];
    displayMemory[1] |= smg_hexArray[shi];
    displayMemory[0] |= smg_hexArray[bai];
}

void Lcd_smgUp3_SetPNum(uint16_t Number)
{
    Lcd_smgDowm3_Close();
    if (Number > 9)
        Number = 9;
    displayMemory[2] |= smg_hexArray[Number];
    displayMemory[1] |= smg_hexArray[0];
    displayMemory[0] |= 0x73;
}

void Drive_Lcd_AllIcon_init(void)
{
    uint8_t i;
    for (i = 0; i < ALL_SEG_NUM; i++)
        displayMemory[i] = 0xff;

    if (AllStatus_S.flashSave_s.PreinstallTempOnOff)
    {
        switch (AllStatus_S.flashSave_s.PreinstallTempNum)
        {
        case PREINSTALL_TMEP300:
            AllStatus_S.flashSave_s.TarTemp = 300;
            Lcd_smgUp3_SetNum(AllStatus_S.flashSave_s.TarTemp); // 恢复温度显示
            break;
        case PREINSTALL_TMEP350:
            AllStatus_S.flashSave_s.TarTemp = 350;
            Lcd_smgUp3_SetNum(AllStatus_S.flashSave_s.TarTemp); // 恢复温度显示
            break;
        case PREINSTALL_TMEP400:
            AllStatus_S.flashSave_s.TarTemp = 400;
            Lcd_smgUp3_SetNum(AllStatus_S.flashSave_s.TarTemp); // 恢复温度显示
            break;
        }
    }
    else
    {
        Lcd_smgUp3_SetNum(AllStatus_S.flashSave_s.TarTemp); // 恢复温度显示
    }
    Lcd_smgDowm3_SetNum(0, 1);
    AllStatus_S.Seting.PNumber = 1;
    if (AllStatus_S.flashSave_s.DisplayPowerOnOff)
        Lcd_icon_onOff(icon_temp, 0); // 熄灭℃图标

    if (AllStatus_S.flashSave_s.BuzOnOff)
        Lcd_icon_onOff(icon_buzz, 1);
    else
        Lcd_icon_onOff(icon_buzz, 0);
    Lcd_icon_onOff(icon_soldering, 0);
    Drive_DisplayLcd_sendData_Task();
}
void Lcd_smgDowm3_SetErrorNum(int16_t ErrorNum, uint8_t OnOff) // 显示错误码
{
    if (OnOff)
    {
        Lcd_smgDowm3_Close();
        switch (ErrorNum)
        {
        case ERROR_E0:
            displayMemory[0] |= 0;
            displayMemory[1] |= smg_hexArray[0x0E];
            displayMemory[2] |= smg_hexArray[ERROR_E0];
            break;
        case ERROR_E1:
            displayMemory[0] |= 0;
            displayMemory[1] |= smg_hexArray[0x0E];
            displayMemory[2] |= smg_hexArray[ERROR_E1];
            break;
        case ERROR_E2:
            displayMemory[0] |= 0;
            displayMemory[1] |= smg_hexArray[0x0E];
            displayMemory[2] |= smg_hexArray[ERROR_E2];
            break;
        case ERROR_E3: // S-E
            /* displayMemory[0] |= 0;
            displayMemory[1] |= smg_hexArray[0x0E];
            displayMemory[2] |= smg_hexArray[ERROR_E3]; */
            displayMemory[0] |= 0x6D;
            displayMemory[1] |= 0x40;
            displayMemory[2] |= smg_hexArray[0x0E];
            break;
        case ERROR_SYSTEM_INIT:
            displayMemory[0] |= 0;
            displayMemory[1] |= smg_hexArray[0x0E];
            displayMemory[2] |= smg_hexArray[ERROR_SYSTEM_INIT];
            break;
        case DRIVE_SLEEP:
            displayMemory[0] |= 0x40;
            displayMemory[1] |= 0x40;
            displayMemory[2] |= 0x40;
            break;
        }
    }
    else
    {
        Lcd_smgDowm3_Close();
    }
}

void APP_Lcd_PowerSetPoint_Task(void)
{
    float32_t Pwmvalue = AllStatus_S.pid_s.pid_out;
    if (Pwmvalue <= 0.0f)
    {
        displayMemory[3] = 0x00;
        return;
    }
    if (Pwmvalue >= (float32_t)MAX_PWM_PRIOD)
    {
        displayMemory[3] = 0xFF; /* 全亮(8个) */
        return;
    }

    float32_t ratio = Pwmvalue / (float32_t)MAX_PWM_PRIOD;
    uint8_t ledCount = (uint8_t)(ratio * 8.0f + 0.8f);
    if (ledCount > 8)
        ledCount = 8;

    if (ledCount == 0)
        displayMemory[3] = 0x00;
    else
        displayMemory[3] = (uint8_t)((1U << ledCount) - 1U);
}

void APP_Lcd_Test(void)
{
    for (int num = 0; num <= 999; num++)
    {
        Lcd_smgDowm3_SetNum(num, 1);
        HAL_Delay(50);                    // 可根据实际需要调整延时
        Drive_DisplayLcd_sendData_Task(); // LCD数据发送任务
    }
}

#endif

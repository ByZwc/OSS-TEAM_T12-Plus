#ifndef __APP_DISPLAYLCD_H__
#define __APP_DISPLAYLCD_H__

typedef enum
{
    icon_400Temp = 0,
    icon_350Temp,
    icon_300Temp,
    icon_SET,
    icon_soldering,
    icon_cup,   
    icon_temp,
    icon_buzz,

} LCD_ICON_TYPE;

typedef enum
{
    uintVar = 0,  // 整数
    intVar,       // 小数
    uintHex,      // 16进制
    DispOnOff,    // 开关
    DispPNum,     // P码
    DispErrorNum, // 错误码

} LCD_SMG_DISPLAY_TYPE;

void Lcd_icon_onOff(LCD_ICON_TYPE lcdIcon, uint8_t onOff);
void Lcd_smgDowm3_SetNum(uint16_t num, uint8_t OnOff);             // 显示0-999整数
void Lcd_smgDowm3_SetHex(uint16_t num);                            // 显示16进制
void Lcd_smgDowm3_DisplayOnOff(uint8_t OnOff_Icon, uint8_t OnOff); // 显示ON/OFF
void Lcd_smgDowm3_SetNimus(int16_t Nimus, uint8_t OnOff);          // 显示负数-50~50
void Lcd_smgDowm3_SetErrorNum(int16_t ErrorNum, uint8_t OnOff);    // 显示错误码

void Lcd_smgUp3_SetNum(uint16_t num);     // 显示0-999整数
void Lcd_smgUp3_SetPNum(uint16_t Number); // 显示P码
void Drive_Lcd_AllIcon_init(void);
void APP_Lcd_Test(void);
void Lcd_SMG_DisplaySel(int32_t temp, uint8_t OnOff, LCD_SMG_DISPLAY_TYPE lcdSmgDisplayType);
void APP_Lcd_PowerSetPoint_Task(void);
#endif

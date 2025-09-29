#ifndef __DRIVE_DESPLAYLCD_H__
#define __DRIVE_DESPLAYLCD_H__

/******************************************************************************* */
#define HG1612_CS_PORT GPIOA
#define HG1612_CS_PIN GPIO_PIN_3
#define HG1612_CS_UP() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET)
#define HG1612_CS_DOWM() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET)

#define HG1612_CLK_PORT GPIOF
#define HG1612_CLK_PIN GPIO_PIN_1
#define HG1612_CLK_UP() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET)
#define HG1612_CLK_DOWM() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET)

#define HG1612_DIO_PORT GPIOF
#define HG1612_DIO_PIN GPIO_PIN_0
#define HG1612_DIO_UP() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET)
#define HG1612_DIO_DOWM() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET)

#define ALL_SEG_NUM 4
#define SEG9ADDR 0x149 // 0b1 0100 1001

#define START_VIDEO 0x807 // 0b1000 0000 0111  打开显示
#define ENABLE_SYS 0x802  // 0b1000 0000 0010  打开系统时钟
#define SET_COMS 0x852    // 0b1000 0101 0010  4COMS,表示 1/3 偏置
#define CLOSE_VIDEO 0x804 // 0b1000 0000 0100  关闭显示
#define CLOSE_SYS 0x800   // 0b1000 0000 0000  关闭时钟

/******************************************************************************* */
#define AIP650E_DIO_PORT GPIOF
#define AIP650E_DIO_PIN GPIO_PIN_1
#define AIP650E_DIO_UP() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET)
#define AIP650E_DIO_DOWM() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET)

#define AIP650E_CLK_PORT GPIOF
#define AIP650E_CLK_PIN GPIO_PIN_0
#define AIP650E_CLK_UP() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET)
#define AIP650E_CLK_DOWM() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET)

#define AIP650E_WRITE_VIDEO 0x48 // 系统指令
#define AIP650E_SET_ADDR1 0x68
#define AIP650E_SET_ADDR2 0x6A
#define AIP650E_SET_ADDR3 0x6C
#define AIP650E_SET_ADDR4 0x6E
#define AIP650E_SET_BRIGHTNESS_MAX 0x01 // 最大亮度
#define AIP650E_SET_BRIGHTNESS_LOW 0x11 // 最小亮度
/******************************************************************************* */

extern uint8_t displayMemory[ALL_SEG_NUM];
extern void Drive_DisplayLcd_Gpio_Init(void);
extern void Drive_DisplayLcd_Init(void);
void Drive_DisplayLcd_SetBrightnessLow(void);
extern void Drive_DisplayLcd_sendData_Task(void);

void Drive_Lcd_Test(void);
void Drive_Lcd_Tid_Test(void);
void Drive_Lcd_ShortCircuit_Test(void);
void Drive_Lcd_SolderingTemp_Test(void);
void Drive_Lcd_Sleep_Test(void);
#endif

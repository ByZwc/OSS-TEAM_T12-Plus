#include "main.h"

uint8_t displayMemory[ALL_SEG_NUM] = {0};
#if USE_DISPLAY_TYPE == USE_DISPLAY_TYPE_HG1621
void Drive_DisplayLcd_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitStruct.Pin = HG1612_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(HG1612_CS_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = HG1612_DIO_PIN | HG1612_CLK_PIN;
    HAL_GPIO_Init(HG1612_DIO_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(HG1612_DIO_PORT, HG1612_DIO_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(HG1612_CLK_PORT, HG1612_CLK_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(HG1612_CS_PORT, HG1612_CS_PIN, GPIO_PIN_SET);
}

static void Drive_DisplayLcd_delay(void)
{
    uint8_t i;
    for (i = 0; i < 10; i++)
    {
        __NOP();
    }
}

static void DisplayLcd_CsColse(void)
{
    HG1612_CS_UP();
    Drive_DisplayLcd_delay();
    HG1612_CLK_UP();
    HG1612_DIO_UP();
    Drive_DisplayLcd_delay();
}

static void Drive_DisplayLcd_SendCmd(uint32_t LcdCmd) // 设置命令
{
    uint8_t i;
    uint32_t temp;

    HG1612_CS_DOWM();
    Drive_DisplayLcd_delay();
    temp = LcdCmd;
    for (i = 0; i < 12; i++)
    {
        HG1612_CLK_DOWM();
        if (temp & 0x800)
            HG1612_DIO_UP();
        else
            HG1612_DIO_DOWM();
        Drive_DisplayLcd_delay();
        HG1612_CLK_UP();
        Drive_DisplayLcd_delay();
        temp = temp << 1;
    }
    DisplayLcd_CsColse();
}

void Drive_DisplayLcd_Init(void)
{
    Drive_DisplayLcd_SendCmd(ENABLE_SYS);  // 打开系统时钟
    Drive_DisplayLcd_SendCmd(SET_COMS);    // 4COMS,表示 1/3 偏置
    Drive_DisplayLcd_SendCmd(START_VIDEO); // 打开显示
}

static void Drive_DisplayLcd_sendAddr(uint32_t LcdCmd) // 发送地址
{
    uint8_t i;
    uint32_t temp;

    temp = LcdCmd;
    for (i = 0; i < 9; i++)
    {
        HG1612_CLK_DOWM();
        if (temp & 0x100)
            HG1612_DIO_UP();
        else
            HG1612_DIO_DOWM();
        Drive_DisplayLcd_delay();
        HG1612_CLK_UP();
        Drive_DisplayLcd_delay();
        temp = temp << 1;
    }
}

static void Drive_DisplayLcd_sendMessage(void) // 数据
{
    uint8_t i, j;
    uint8_t temp;

    for (j = 0; j < ALL_SEG_NUM; j++)
    {
        temp = displayMemory[j];
        for (i = 0; i < 4; i++)
        {
            HG1612_CLK_DOWM();
            if (temp & 0x01)
                HG1612_DIO_UP();
            else
                HG1612_DIO_DOWM();
            Drive_DisplayLcd_delay();
            HG1612_CLK_UP();
            Drive_DisplayLcd_delay();
            temp = temp >> 1;
        }
    }
}

void Drive_DisplayLcd_sendData_Task(void)
{
    // uint32_t primask_bit;

    HG1612_CS_DOWM();
    Drive_DisplayLcd_delay();

    /* Enter critical section */
    // primask_bit = __get_PRIMASK();
    //__disable_irq();

    Drive_DisplayLcd_sendAddr(SEG9ADDR);

    Drive_DisplayLcd_sendMessage();

    DisplayLcd_CsColse();

    /* Exit critical section: restore previous priority mask */
    //__set_PRIMASK(primask_bit);
}
#endif

#if USE_DISPLAY_TYPE == USE_DISPLAY_TYPE_AIP650
void Drive_DisplayLcd_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitStruct.Pin = AIP650E_CLK_PIN | AIP650E_DIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(AIP650E_CLK_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(AIP650E_CLK_PORT, AIP650E_CLK_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AIP650E_DIO_PORT, AIP650E_DIO_PIN, GPIO_PIN_SET);
}

static void AIP650E_DIO_SetInOut(uint32_t mode)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOF_CLK_ENABLE();
    if (mode == 0)
    {
        GPIO_InitStruct.Pin = AIP650E_DIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(AIP650E_DIO_PORT, &GPIO_InitStruct);
    }
    else
    {
        GPIO_InitStruct.Pin = AIP650E_DIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(AIP650E_DIO_PORT, &GPIO_InitStruct);
    }
}

static void AIP650E_nop(void)
{
    uint8_t i;
    for (i = 0; i < 10; i++)
    {
        __NOP();
    }
}

static void AIP650E_sendStart(void)
{
    AIP650E_DIO_DOWM();
    AIP650E_nop();
    AIP650E_CLK_DOWM();
    AIP650E_nop();
}

static void AIP650E_sendStop(void)
{
    AIP650E_DIO_DOWM();
    AIP650E_nop();
    AIP650E_CLK_UP();
    AIP650E_nop();
    AIP650E_DIO_UP();
    AIP650E_nop();
}

static void AIP650E_RecAck(void)
{
    AIP650E_DIO_SetInOut(1);
    AIP650E_nop();
    AIP650E_CLK_UP();
    AIP650E_nop();
    AIP650E_CLK_DOWM();
    AIP650E_nop();
    AIP650E_DIO_SetInOut(0);
}

static void AIP650E_sendData(uint8_t data_t)
{
    static uint8_t i;

    for (i = 0; i < 8; i++)
    {
        if (data_t & 0X80)
            AIP650E_DIO_UP();
        else
            AIP650E_DIO_DOWM();
        AIP650E_nop();
        AIP650E_CLK_UP();
        AIP650E_nop();
        AIP650E_CLK_DOWM();
        data_t = (data_t << 1);
    }
    AIP650E_RecAck();
}

void Drive_DisplayLcd_Init(void)
{
    uint32_t primask_bit;
    /* Enter critical section */
    primask_bit = __get_PRIMASK();
    __disable_irq();

    AIP650E_sendStart();
    AIP650E_sendData(AIP650E_WRITE_VIDEO);
    AIP650E_sendData(AIP650E_SET_BRIGHTNESS_MAX);
    AIP650E_sendStop();

    /* Exit critical section: restore previous priority mask */
    __set_PRIMASK(primask_bit);
}

void Drive_DisplayLcd_SetBrightnessLow(void)
{
    uint32_t primask_bit;
    /* Enter critical section */
    primask_bit = __get_PRIMASK();
    __disable_irq();

    AIP650E_sendStart();
    AIP650E_sendData(AIP650E_WRITE_VIDEO);
    AIP650E_sendData(AIP650E_SET_BRIGHTNESS_LOW);
    AIP650E_sendStop();

    /* Exit critical section: restore previous priority mask */
    __set_PRIMASK(primask_bit);
}

void Drive_DisplayLcd_sendData_Task(void)
{
    uint32_t primask_bit;
    /* Enter critical section */
    primask_bit = __get_PRIMASK();
    __disable_irq();

    /* AIP650E_sendStart();
    AIP650E_sendData(AIP650E_WRITE_VIDEO);
    AIP650E_sendData(AIP650E_SET_BRIGHTNESS_MAX);
    AIP650E_sendStop(); */

    AIP650E_sendStart();
    AIP650E_sendData(AIP650E_SET_ADDR1);
    AIP650E_sendData(displayMemory[0]);
    AIP650E_sendStop();

    AIP650E_sendStart();
    AIP650E_sendData(AIP650E_SET_ADDR2);
    AIP650E_sendData(displayMemory[1]);
    AIP650E_sendStop();

    AIP650E_sendStart();
    AIP650E_sendData(AIP650E_SET_ADDR3);
    AIP650E_sendData(displayMemory[2]);
    AIP650E_sendStop();

    AIP650E_sendStart();
    AIP650E_sendData(AIP650E_SET_ADDR4);
    AIP650E_sendData(displayMemory[3]);
    AIP650E_sendStop();

    /* Exit critical section: restore previous priority mask */
    __set_PRIMASK(primask_bit);
}

void Drive_Lcd_Test(void)
{
    for (int i = 0; i < 4; i++)
    {
        displayMemory[i] = 0xFF;
    }
    Drive_DisplayLcd_sendData_Task(); // LCD数据发送任务
    HAL_Delay(500);
    for (int i = 0; i < 4; i++)
    {
        displayMemory[i] = 0x00;
    }
    Drive_DisplayLcd_sendData_Task(); // LCD数据发送任务
    HAL_Delay(500);
}

void Drive_Lcd_Tid_Test(void)
{
    AllStatus_S.adc_value[SOLDERING_TID_NUM] = Drive_ADCConvert(SOLDERING_TID_NUM);
    Lcd_SMG_DisplaySel(AllStatus_S.adc_value[SOLDERING_TID_NUM], 1, uintHex);
    Drive_DisplayLcd_sendData_Task();
    HAL_Delay(500);
}

void Drive_Lcd_ShortCircuit_Test(void)
{
    HAL_GPIO_WritePin(MOSSWITCH_GPIOB_PORT, MOSSWITCH_GPIOB_PIN, GPIO_PIN_SET);
    HAL_Delay(30);
    AllStatus_S.adc_value[SOLDERING_ELECTRICITY_NUM] = Drive_ADCConvert(SOLDERING_ELECTRICITY_NUM);
    HAL_GPIO_WritePin(MOSSWITCH_GPIOB_PORT, MOSSWITCH_GPIOB_PIN, GPIO_PIN_RESET);
    Lcd_SMG_DisplaySel(AllStatus_S.adc_value[SOLDERING_ELECTRICITY_NUM], 1, uintHex);
    Drive_DisplayLcd_sendData_Task();
    HAL_Delay(200);
}

#endif

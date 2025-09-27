#include "main.h"

TIM_HandleTypeDef TimHandle;
TIM_OC_InitTypeDef sConfig;

// Macro definitions for GPIO pins and timer parameters

void Drive_MosSwitch_OFF(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable the GPIO_LED Clock */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = MOSSWITCH_GPIOB_PIN;
    GPIO_InitStruct.Mode = MOSSWITCH_GPIO_MODE_OUT;
    GPIO_InitStruct.Pull = MOSSWITCH_GPIO_PULL;
    GPIO_InitStruct.Speed = MOSSWITCH_GPIO_SPEED;

    HAL_GPIO_Init(MOSSWITCH_GPIOB_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(MOSSWITCH_GPIOB_PORT, MOSSWITCH_GPIOB_PIN, GPIO_PIN_RESET);
}

/**
 * @brief  Initialize TIM3
 * @param  None
 * @retval None
 */
static void Drive_TIM3_INIT(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* Select TIM3 */
    TimHandle.Instance = MOSSWITCH_TIM_INSTANCE;

    /* Auto-reload value */
    TimHandle.Init.Period = MOSSWITCH_TIM_PERIOD;

    /* Prescaler value */
    TimHandle.Init.Prescaler = MOSSWITCH_TIM_PRESCALER;

    /* Clock not divided */
    TimHandle.Init.ClockDivision = MOSSWITCH_TIM_CLOCKDIV;

    /* Up-counting mode*/
    TimHandle.Init.CounterMode = MOSSWITCH_TIM_COUNTERMODE;

    /* No repetition */
    TimHandle.Init.RepetitionCounter = MOSSWITCH_TIM_REPETITION;

    /* Auto-reload register not buffered */
    TimHandle.Init.AutoReloadPreload = MOSSWITCH_TIM_AUTORELOAD;

    /* Initialize clock settings */
    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
        AllStatus_S.SolderingState = SOLDERING_STATE_INIT_ERROR;
        APP_ErrorHandler();
    }
}
/**
 * @brief  TIM3 PWM Configuration
 * @param  None
 * @retval None
 */
static void Drive_TIM3_PWM(void)
{
    /*Output configured for PWM1 mode */
    sConfig.OCMode = MOSSWITCH_PWM_OCMODE;

    /*OC channel output active high */
    sConfig.OCPolarity = MOSSWITCH_PWM_OCPOLARITY;

    /*Disable the output compare fast mode */
    sConfig.OCFastMode = MOSSWITCH_PWM_OCFASTMODE;

    /*OCN channel output active high */
    sConfig.OCNPolarity = MOSSWITCH_PWM_OCNPOLARITY;

    /*Idle state OC1N output low level */
    sConfig.OCNIdleState = MOSSWITCH_PWM_OCNIDLE;

    /*Idle state OC1 output low level*/
    sConfig.OCIdleState = MOSSWITCH_PWM_OCIDLE;

    /*Set CC2 pulse value to 0 */
    sConfig.Pulse = MOSSWITCH_PWM_PULSE;

    /* Configure Channel 2 for PWM */
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, MOSSWITCH_TIM_CHANNEL) != HAL_OK)
    {
        AllStatus_S.SolderingState = SOLDERING_STATE_INIT_ERROR;
        APP_ErrorHandler();
    }
}

static void Drive_MosPWMoutMode(void)
{
    Drive_TIM3_INIT();

    Drive_TIM3_PWM();

    /* Start PWM output on Channel 2 */
    if (HAL_TIM_PWM_Start(&TimHandle, MOSSWITCH_TIM_CHANNEL) != HAL_OK)
    {
        AllStatus_S.SolderingState = SOLDERING_STATE_INIT_ERROR;
        APP_ErrorHandler();
    }
}

void Drive_MosSwitch210_PWMOut(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = MOSSWITCH_GPIOA_PIN;
    GPIO_InitStruct.Mode = MOSSWITCH_GPIO_MODE_AF;
    GPIO_InitStruct.Pull = MOSSWITCH_GPIO_PULL;
    GPIO_InitStruct.Speed = MOSSWITCH_GPIO_SPEED;
    GPIO_InitStruct.Alternate = MOSSWITCH_GPIO_AF_TIM3;

    HAL_GPIO_Init(MOSSWITCH_GPIOA_PORT, &GPIO_InitStruct);
    if (!AllStatus_S.PwmIsInitComplete)
    {
        Drive_MosPWMoutMode();
        AllStatus_S.PwmIsInitComplete = 1;
    }
}

void Drive_MosSwitch245_PWMOut(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = MOSSWITCH_GPIOB_PIN;
    GPIO_InitStruct.Mode = MOSSWITCH_GPIO_MODE_AF;
    GPIO_InitStruct.Pull = MOSSWITCH_GPIO_PULL;
    GPIO_InitStruct.Speed = MOSSWITCH_GPIO_SPEED;
    GPIO_InitStruct.Alternate = MOSSWITCH_GPIO_AF_TIM3;

    HAL_GPIO_Init(MOSSWITCH_GPIOB_PORT, &GPIO_InitStruct);
    if (!AllStatus_S.PwmIsInitComplete)
    {
        Drive_MosPWMoutMode();
        AllStatus_S.PwmIsInitComplete = 1;
    }
}

void Drive_MosSwitch_SetDuty(uint16_t duty)
{
    TIM3->CCR2 = duty;
}

#include "BTIM1.h"

/*
TODO
pwm beep,按键beep,睡眠前beep
唤醒5306通过gpio 15秒调用一次
超时60秒休眠
*/


void Btim1_Init(void)
{
    BTIM_TimeBaseInitTypeDef BTIM_TimeBaseInitStruct;
    __RCC_BTIM_CLK_ENABLE();

    /* NVIC Configuration */
    NVIC_EnableIRQ(BTIM1_IRQn);

    BTIM_TimeBaseInitStruct.BTIM_Mode      = BTIM_Mode_TIMER;
    BTIM_TimeBaseInitStruct.BTIM_Period    = 6000 - 1;      // 1ms采集1次
    BTIM_TimeBaseInitStruct.BTIM_Prescaler = BTIM_PRS_DIV1; // 6MHZ
    BTIM_TimeBaseInitStruct.BTIM_OPMode    = BTIM_OPMode_Repetitive;

    BTIM_TimeBaseInit(CW_BTIM1, &BTIM_TimeBaseInitStruct);
    BTIM_ITConfig(CW_BTIM1, BTIM_IT_OV, ENABLE);
    BTIM_Cmd(CW_BTIM1, ENABLE);
}

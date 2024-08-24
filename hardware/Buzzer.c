#include "Buzzer.h"

void Buzzer_Init(void)
{
	__RCC_GPIOC_CLK_ENABLE();       // ʹ��GPIOʱ��
	__RCC_GTIM4_CLK_ENABLE();       // ʹ��ͨ�ö�ʱ��1ʱ��
	GPIO_InitTypeDef GPIO_InitStruct;  // GPIO��ʼ���ṹ��
	GPIO_InitStruct.Pins = GPIO_PIN_14;           // GPIO����
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // �������
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;     // ����ٶȸ�
	
	PC14_AFx_GTIM4CH4();       // ʹ�ø��ù��ܣ�����Ϊͨ�ö�ʱ��4ͨ��4
	
	GPIO_Init(CW_GPIOC, &GPIO_InitStruct);    // ��ʼ��
	
	GTIM_InitTypeDef GTIM_InitStruct;  // ͨ�ö�ʱ����ʼ���ṹ��
	
	GTIM_InitStruct.Mode = GTIM_MODE_TIME;               // ��ʱ��ģʽ
	GTIM_InitStruct.OneShotMode = GTIM_COUNT_CONTINUE;   // ��������ģʽ
	GTIM_InitStruct.Prescaler = GTIM_PRESCALER_DIV64;    // DCLK = PCLK / 4 = 6MHz/4 = 1.5MHz
	GTIM_InitStruct.ReloadValue = 100;                  // ��װ��ֵ����
	GTIM_InitStruct.ToggleOutState = DISABLE;            // �����ת����
	
	GTIM_TimeBaseInit(CW_GTIM4, &GTIM_InitStruct);        // ��ʼ��
	
	GTIM_OCInit(CW_GTIM4, GTIM_CHANNEL4, GTIM_OC_OUTPUT_PWM_HIGH); // ��������Ƚ�ͨ��4ΪPWMģʽ
	GTIM_Cmd(CW_GTIM4, DISABLE);
	GTIM_SetCompare4(CW_GTIM4, 50);
}

void buzzer_beep()
{
	GTIM_Cmd(CW_GTIM4, ENABLE);
}

void buzzer_idle()
{
	GTIM_Cmd(CW_GTIM4, DISABLE);
}

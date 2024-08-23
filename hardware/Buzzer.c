#include "Buzzer.h"

void Buzzer_Init(void)
{
	__RCC_GPIOC_CLK_ENABLE();       // 使能GPIO时钟
	__RCC_GTIM4_CLK_ENABLE();       // 使能通用定时器1时钟
	GPIO_InitTypeDef GPIO_InitStruct;  // GPIO初始化结构体
	GPIO_InitStruct.Pins = GPIO_PIN_14;           // GPIO引脚
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;     // 输出速度高
	
	PC14_AFx_GTIM4CH4();       // 使用复用功能，复用为通用定时器1通道3
	
	GPIO_Init(CW_GPIOC, &GPIO_InitStruct);    // 初始化
	
		GTIM_InitTypeDef GTIM_InitStruct;  // 通用定时器初始化结构体
	
	GTIM_InitStruct.Mode = GTIM_MODE_TIME;               // 定时器模式
	GTIM_InitStruct.OneShotMode = GTIM_COUNT_CONTINUE;   // 连续计数模式
	GTIM_InitStruct.Prescaler = GTIM_PRESCALER_DIV64;    // DCLK = PCLK / 64 = 64MHz/64 = 1MHz
	GTIM_InitStruct.ReloadValue = 100;                  // 重装载值设置
	GTIM_InitStruct.ToggleOutState = DISABLE;            // 输出翻转功能
	
	GTIM_TimeBaseInit(CW_GTIM4, &GTIM_InitStruct);        // 初始化
	
	GTIM_OCInit(CW_GTIM4, GTIM_CHANNEL4, GTIM_OC_OUTPUT_PWM_HIGH); // 配置输出比较通道3为PWM模式
	
	GTIM_Cmd(CW_GTIM4, ENABLE);
}

void buzzer_beep()
{
	//GTIM_SetCompare4(CW_GTIM4, 50);
	
}

void buzzer_idle()
{
	//GTIM_SetCompare4(CW_GTIM4, 5);
}
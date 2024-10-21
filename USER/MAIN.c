
#include "main.h" 
#include "Seg_Dis.h"
#include "BTIM1.h"
#include "ADC.h"
#include "flash.h"
#include "UART3.h"
#include "PWR.h"
#include "calibration.h"
#include "measurement.h"
#include "Buzzer.h"

extern uint8_t Seg_Reg[6];


volatile uint8_t gKeyStatus;  /* set to 1 after User Button interrupt  */

unsigned int timecount=0;
unsigned int sleepcount=0;
unsigned int beepcount=0;

unsigned int beep_stat = 0;
unsigned int sleep_beep_stat = 0;

unsigned int enter_sleep = 0;

//定义模式
// mode 0: TEST.VO,tu-
// mode 1: TEST.CU,tc-
// mode 2: CAL.U05,cU5.
// mode 3: CAL.U15,cUF.
// mode 2: CAL.I05,c0.5
// mode 5: CAL.I15,c1.5

Select_Mode currentMode = TEST_MODE_VO;//initial mode

unsigned char BrushFlag=0;  

void RCC_Configuration(void)
{
  /* 0. HSI使能并校准 */
  RCC_HSI_Enable(RCC_HSIOSC_DIV6);

  /* 1. 设置HCLK和PCLK的分频系数　*/
  RCC_HCLKPRS_Config(RCC_HCLK_DIV1);
  RCC_PCLKPRS_Config(RCC_PCLK_DIV1);
  
  /* 2. 使能PLL，通过PLL倍频到48MHz */
  RCC_PLL_Enable(RCC_PLLSOURCE_HSI, 8000000, 6);     // HSI 默认输出频率8MHz
 // RCC_PLL_OUT();  //PC13脚输出PLL时钟
  
  ///< 当使用的时钟源HCLK大于24M,小于等于48MHz：设置FLASH 读等待周期为2 cycle
  ///< 当使用的时钟源HCLK大于48MHz：设置FLASH 读等待周期为3 cycle
  __RCC_FLASH_CLK_ENABLE();
  FLASH_SetLatency(FLASH_Latency_2);
    
  /* 3. 时钟切换到PLL */
  RCC_SysClk_Switch(RCC_SYSCLKSRC_PLL);
  RCC_SystemCoreClockUpdate(48000000);
	RCC_PCLKPRS_Config(RCC_PCLK_DIV8); //配置HCLK 到 PCLK的分频系数  6MHz
}



void KEYGPIO_Init(void)
{
	__RCC_GPIOA_CLK_ENABLE();//打开GPIOA的时钟
	GPIO_InitTypeDef GPIO_InitStruct;
		
	GPIO_InitStruct.Pins = GPIO_PIN_8|GPIO_PIN_9; //K1 K2
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
  GPIO_InitStruct.IT = GPIO_IT_NONE;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

  	GPIO_InitStruct.Pins = GPIO_PIN_10;//K3
	GPIO_InitStruct.IT = GPIO_IT_FALLING;
	GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
	NVIC_SetPriority(GPIOA_IRQn, 0x03);
    //清除中断标志并使能NVIC
    GPIOA_INTFLAG_CLR(GPIOx_ICR_PIN10_Msk);
    NVIC_EnableIRQ(GPIOA_IRQn);
	

}

void LED_Init(void)
{
	__RCC_GPIOC_CLK_ENABLE();//打开GPIOC的时钟
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pins = GPIO_PIN_13; //LED
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.IT = GPIO_IT_NONE;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(CW_GPIOC, &GPIO_InitStruct);
}

void DisplayBuff(void)
{
	DisplayMode(currentMode);
	uint8_t mode = (uint8_t)currentMode;
	switch(mode)
	{
		case TEST_MODE_VO:
			DisplaySETV(V_Buffer);
			break;
		case TEST_MODE_CU:
			if(I_Buffer>400)I_Buffer=400;
			DisplayI(I_Buffer);
			break;
		case CALI_MODE_U05:
		case CALI_MODE_U15:
			DisplaySETV(V_Buffer);
			break;
		case CALI_MODE_I0P5:
		case CALI_MODE_I1P5:
			DisplayI(I_Buffer);
			break;
	}
}

int main()
{	
	RCC_Configuration();   //系统时钟64M
	KEYGPIO_Init();
	LED_Init();
	GPIO_WritePin(CW_GPIOC,GPIO_PIN_13,GPIO_Pin_RESET); 
	Seg_Init();
	Btim1_Init();
	ADC_init();
	UART3_Init();
	Buzzer_Init();
	read_vol_cur_calibration();
	ComputeK();
	while(1)
	{
		gKeyStatus = 0;
		if(BrushFlag==1)
		{
			DisplayBuff();
			BrushFlag=0;
		}
			
		if(timecount>= 300)     //300ms改变一次数码管显示值//	
		{
			timecount=0;
			Volt_Cal();
			BrushFlag=1;
		}
		
		if(gKeyStatus==1)
		{
			enter_sleep = 1;
		}
		
		if(sleepcount >= 60000)//1分钟睡眠
		{
			if(beepcount<1000){
				  sleep_beep_stat = 1;
					buzzer_beep();
			}else{
				  sleep_beep_stat = 0;
					buzzer_idle();
					beepcount=0;
				  processDeepSleep();
			    sleepcount=0;
			}
		}
		
		if(beep_stat)
		{
			if(beepcount<100){
			    buzzer_beep();
			}else{
				  buzzer_idle();
				  beepcount=0;
				  beep_stat = 0;
				  if(enter_sleep == 1){
					    processDeepSleep();
				      enter_sleep = 0;
					}
			}			
		}
	}
}

void BTIM1_IRQHandler(void)
{
  	static uint32_t keytime=0,keytime2=0,keytime3=0,ledcount=0;
	
  	/* USER CODE BEGIN */
  	if (BTIM_GetITStatus(CW_BTIM1, BTIM_IT_OV))
  	{
			BTIM_ClearITPendingBit(CW_BTIM1, BTIM_IT_OV);
			Get_ADC_Value();
				
			ledcount++;  //LED闪
			if(ledcount>=1000)
			{PC13_TOG();ledcount=0;}

			timecount++;
			sleepcount++;
			
			if((beep_stat == 1) | (sleep_beep_stat == 1))
			    beepcount++;
			
			Dis_Refresh();//数码管扫描显示
			
			
			if(GPIO_ReadPin(CW_GPIOA,GPIO_PIN_8)==GPIO_Pin_RESET)//K1切换模式
			{
				
				keytime++;
				sleepcount = 0;//有按键，重新计时
				if(keytime>=100 )
				{
					beep_stat = 1;
					keytime=0;  //切换模式
					currentMode++;
					if(currentMode>=MODE_END)currentMode=0;
					BrushFlag=1; //更新数码管
				}			 
			}else{
				keytime=0;
			}
					
			if(GPIO_ReadPin(CW_GPIOA,GPIO_PIN_9)==GPIO_Pin_RESET && (currentMode>1))//K2存储校准
			{
				beep_stat = 1;
				keytime2++;
				sleepcount = 0;//有按键，重新计时
				if(keytime2>=100 )
				{
					beep_stat = 1;
					keytime2=0;  //切换模式
				
					if(currentMode==2)
					{
						X05=Mean_Value_Filter(Volt_Buffer,ADC_SAMPLE_SIZE);
						save_calibration();ComputeK();Volt_Cal();BrushFlag=1;currentMode=0;
					}
					if(currentMode==3)
					{
						X15=Mean_Value_Filter(Volt_Buffer,ADC_SAMPLE_SIZE);
						save_calibration();ComputeK();Volt_Cal();BrushFlag=1;currentMode=0;
					}
					if(currentMode==4)
					{
						IX05=Mean_Value_Filter(Curr_Buffer,ADC_SAMPLE_SIZE);
						save_calibration();ComputeK();Volt_Cal();BrushFlag=1;currentMode=0;
					}
					if(currentMode==5)
					{
						IX15=Mean_Value_Filter(Curr_Buffer,ADC_SAMPLE_SIZE);
						save_calibration();ComputeK();Volt_Cal();BrushFlag=1;currentMode=0;
					}						 					 
				}			 
			}else{
				keytime2=0;
			}
					
			if(GPIO_ReadPin(CW_GPIOA,GPIO_PIN_10)==GPIO_Pin_RESET)//K3退出模式
			{
					keytime3++;
					if(keytime3>=100 )
					{
						beep_stat = 1;
						keytime3=0;  //切换模式				
					}
			}
			else keytime3=0;			 
  	}
  /* USER CODE END */
}

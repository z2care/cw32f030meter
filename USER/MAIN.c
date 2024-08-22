
#include "main.h" 
#include "Seg_Dis.h"
#include "BTIM1.h"
#include "ADC.h"
#include "flash.h"

#ifdef __GNUC__
    /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
    set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
 * @brief Retargets the C library printf function to the USART.
 *
 */
PUTCHAR_PROTOTYPE
{
    USART_SendData_8bit(CW_UART3, (uint8_t)ch);

    while (USART_GetFlagStatus(CW_UART3, USART_FLAG_TXE) == RESET);

    return ch;
}

extern uint16_t Volt_Buffer[ADC_SAMPLE_SIZE];
extern uint16_t Curr_Buffer[ADC_SAMPLE_SIZE];
extern uint8_t Seg_Reg[6];
uint16_t V_Buffer,I_Buffer;

volatile uint8_t gKeyStatus;  /* set to 1 after User Button interrupt  */

unsigned int timecount=0;
unsigned int sleepcount=0;
uint8_t enterDeep = 0;

//5V与15V 校准
unsigned int X05=0;
unsigned int X15=0;

unsigned int Y15=15;
unsigned int Y05=5;
float K; //斜率


//0.5A与1.5A 校准
unsigned int IX05=0;
unsigned int IX15=0;

unsigned int IY15=150;
unsigned int IY05=50;
float KI; //斜率

//定义模式
// mode 0: TEST.VO,tu-
// mode 1: TEST.CU,tc-
// mode 2: CAL.U05,cU5.
// mode 3: CAL.U15,cUF.
// mode 2: CAL.I05,c0.5
// mode 5: CAL.I15,c1.5
typedef enum
{
    TEST_MODE_VO = 0,
	TEST_MODE_CU,
	CALI_MODE_U05,
	CALI_MODE_U15,
	CALI_MODE_I0P5,
	CALI_MODE_I1P5,
	MODE_END
} Select_Mode;

uint8_t currentMode = TEST_MODE_VO;//initial mode

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

void UART3_Init(void)
{
	__RCC_GPIOA_CLK_ENABLE();//打开GPIOA的时钟
	__RCC_UART3_CLK_ENABLE();//打开UART3的时钟

	GPIO_InitTypeDef GPIO_InitStructure;
    //UART TX RX 复用
	PA06_AFx_UART1TXD();
	PA07_AFx_UART1RXD();
    GPIO_InitStructure.Pins = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_Init(CW_GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;//DEBUG_USART_BaudRate;
    USART_InitStructure.USART_Over = USART_Over_16;
    USART_InitStructure.USART_Source = USART_Source_PCLK;
    USART_InitStructure.USART_UclkFreq = 8000000;//DEBUG_USART_UclkFreq;
    USART_InitStructure.USART_StartBit = USART_StartBit_FE;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(CW_UART3, &USART_InitStructure);

}

void KEYGPIO_Init(void)
{
	__RCC_GPIOA_CLK_ENABLE();//打开GPIOA的时钟
	__RCC_GPIOC_CLK_ENABLE();//打开GPIOC的时钟
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
	
	GPIO_InitStruct.Pins = GPIO_PIN_13; //LED
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.IT = GPIO_IT_NONE;
  GPIO_Init(CW_GPIOC, &GPIO_InitStruct);
}

extern void DisplaySETV(uint32_t value);
void DisplayBuff(void)
{
	DisplayMode(currentMode);
	switch(currentMode)
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

void ComputeK(void)
{
	K=(Y15-Y05);
	K=K/(X15-X05);
	
	KI=(IY15-IY05);
	KI=KI/(IX15-IX05);
}

void save_calibration(void)
{
	 uint16_t da[5];
	  da[0]=0xaa;
	  da[1]=X05;
	  da[2]=X15;
	  da[3]=IX05;
	  da[4]=IX15;
    flash_erase();
    flash_write(0,da,5);
}
/**
 * @brief
 *
 */
void read_vol_cur_calibration(void)
{
    uint16_t da[5];
    flash_read(0,da, 5);
	  if(da[0]!=0xaa)//还没校准过时，计算理论值，并存储
		{
			X15=15.0/23/1.5*4096;
			X05=5.0/23/1.5*4096;
			IX05=0.5*0.1/1.5*4096;
			IX15=1.5*0.1/1.5*4096;
      save_calibration();
		}
		else 
		{
			X05=da[1];
			X15=da[2];
			IX05=da[3];
			IX15=da[4];
		}
		
}

void Volt_Cal(void);

int main()
{	
	RCC_Configuration();   //系统时钟64M
	KEYGPIO_Init();
	GPIO_WritePin(CW_GPIOC,GPIO_PIN_13,GPIO_Pin_RESET); 
	Seg_Init();
	Btim1_Init();
	ADC_init(); 
		
	read_vol_cur_calibration();
	ComputeK();
	while(1)
	{ 
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
	}
}


uint32_t Mean_Value_Filter(uint16_t *value, uint32_t size)     //均值滤波
{
    uint32_t sum = 0;
    uint16_t max = 0;
    uint16_t min = 0xffff;
    int      i;

    for(i = 0; i < size; i++)
    {
        sum += value[i];
        if(value[i] > max)
        {
            max = value[i];
        }
        if(value[i] < min)
        {
            min = value[i];
        }
    }
    sum -= max + min;
    sum  = sum / (size - 2);
		//if(sum>1)sum+=4; 后期校准
    return sum;
}

void Volt_Cal(void)
{ 	
	float t,KT1;
	
	V_Buffer = Mean_Value_Filter(Volt_Buffer,ADC_SAMPLE_SIZE);//使用均值滤波
	I_Buffer = Mean_Value_Filter(Curr_Buffer,ADC_SAMPLE_SIZE); //使用均值滤波
	
	if(V_Buffer>=X05)
	{
		t=V_Buffer-X05;
	  V_Buffer=(K*t+Y05)*1000;}
	else
  { 
		KT1=5000;
		KT1=KT1/X05;
		V_Buffer=KT1*V_Buffer;
	}
	// 四舍五入
    if(V_Buffer % 10 >= 5)
    {
        V_Buffer = V_Buffer / 10 + 1;  //10mV为单位
    }
    else
    {
        V_Buffer = V_Buffer / 10;
    }
	printf("%d\r\n",V_Buffer);
		
		
	if(I_Buffer>=IX05)
	{
		 t=I_Buffer-IX05;
	   I_Buffer=(KI*t+IY05)*10;
	}
	else
	{
		KT1=500;
		KT1=KT1/IX05;
		I_Buffer=KT1*I_Buffer;
	}
	
		if(I_Buffer % 10 >= 5)
    {
        I_Buffer = I_Buffer / 10 + 1;
    }
    else
    {
        I_Buffer = I_Buffer / 10;
    }
	//	I_Buffer=I_Buffer * ADC_REF_VALUE >> 12;
		 /**
        mv =I_Buffer * ADC_REF_VALUE >> 12,
        R = 100mr,
        10ma = mv/R/10=mv/0.1/10 = mv
     */	
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

		if(sleepcount >= 60000)//1分钟睡眠
		{
			enterDeep=1;
			processDeepSleep();
		}

		Dis_Refresh();//数码管扫描显示
		
		
		if(GPIO_ReadPin(CW_GPIOA,GPIO_PIN_8)==GPIO_Pin_RESET)//K1切换模式
		{
			keytime++;
			sleepcount = 0;//有按键，重新计时
			if(keytime>=100 )
			{
				keytime=0;  //切换模式
				currentMode++;
				if(currentMode>=MODE_END)currentMode=0;
				BrushFlag=1; //更新数码管
			}			 
		}
		else keytime=0;
				
		if(GPIO_ReadPin(CW_GPIOA,GPIO_PIN_9)==GPIO_Pin_RESET&&Mode!=0)//K2存储校准
		{
			keytime2++;
			sleepcount = 0;//有按键，重新计时
			if(keytime2>=100 )
			{
				keytime2=0;  //切换模式
			
				if(Mode==2)
				{
					X05=Mean_Value_Filter(Volt_Buffer,ADC_SAMPLE_SIZE);
					save_calibration();ComputeK();Volt_Cal();BrushFlag=1;Mode=0;
				}
				if(Mode==3)
				{
					X15=Mean_Value_Filter(Volt_Buffer,ADC_SAMPLE_SIZE);
					save_calibration();ComputeK();Volt_Cal();BrushFlag=1;Mode=0;
				}
				if(Mode==4)
				{
					IX05=Mean_Value_Filter(Curr_Buffer,ADC_SAMPLE_SIZE);
					save_calibration();ComputeK();Volt_Cal();BrushFlag=1;Mode=0;
				}
				if(Mode==5)
				{
					IX15=Mean_Value_Filter(Curr_Buffer,ADC_SAMPLE_SIZE);
					save_calibration();ComputeK();Volt_Cal();BrushFlag=1;Mode=0;
				}						 					 
			}			 
		}
		else keytime2=0; 
				
		if(GPIO_ReadPin(CW_GPIOA,GPIO_PIN_10)==GPIO_Pin_RESET)//K3退出模式
		{
				keytime3++;
				if(keytime3>=100 )
				{
					keytime3=0;  //切换模式
					if(enterDeep){
						enterDeep=0;//唤醒回来，设置清醒状态
						sleepcount = 0;//唤醒回来，重新计时
					}else{
						enterDeep=1;
						processDeepSleep();
					}					
				}
		}
		else keytime3=0;			 
  	}
  /* USER CODE END */
}

void processDeepSleep(void)
{
    uint8_t res = 0u;
    GPIO_InitTypeDef GPIO_InitStructure;
    PWR_InitTypeDef PWR_InitStructure;
    /* Configure all GPIO as analog to reduce current consumption on non used IOs */
    /* Warning : Reconfiguring all GPIO will close the connexion with the debugger */
    /* Enable GPIOs clock */
    //打开GPIO时钟
    REGBITS_SET(CW_SYSCTRL->AHBEN, SYSCTRL_AHBEN_GPIOA_Msk | SYSCTRL_AHBEN_GPIOB_Msk | \
                SYSCTRL_AHBEN_GPIOC_Msk | SYSCTRL_AHBEN_GPIOF_Msk);
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.IT = GPIO_IT_NONE;
    GPIO_InitStructure.Pins = GPIO_PIN_All;
    GPIO_Init(CW_GPIOA, &GPIO_InitStructure);
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);
    GPIO_Init(CW_GPIOC, &GPIO_InitStructure);
    GPIO_Init(CW_GPIOF, &GPIO_InitStructure);
    //关闭GPIO时钟
    REGBITS_CLR(CW_SYSCTRL->AHBEN, SYSCTRL_AHBEN_GPIOA_Msk | SYSCTRL_AHBEN_GPIOB_Msk | \
                SYSCTRL_AHBEN_GPIOC_Msk | SYSCTRL_AHBEN_GPIOF_Msk);
    /* Configure User Button */
    KEYGPIO_Init();
    /* Configures system clock after wake-up from DeepSleep: enable HSI and PLL with HSI as source*/
    // 唤醒后自动使用内部高速时钟（HSI）
    RCC_WAKEUPCLK_Config(RCC_SYSCTRL_WAKEUPCLKEN);
    /* Enter Stop Mode */
    PWR_InitStructure.PWR_Sevonpend = PWR_Sevonpend_Disable;
    PWR_InitStructure.PWR_SleepDeep = PWR_SleepDeep_Enable;
    PWR_InitStructure.PWR_SleepOnExit = PWR_SleepOnExit_Disable;
    PWR_Config(&PWR_InitStructure);
    //打开FLASH时钟
    REGBITS_SET(CW_SYSCTRL->AHBEN, SYSCTRL_AHBEN_FLASH_Msk);
    REGBITS_SET(CW_FLASH->CR1, FLASH_CR1_STANDBY_Msk); //打开FLASH低功耗使能控制
    //关闭FLASH时钟
    REGBITS_CLR(CW_SYSCTRL->AHBEN, SYSCTRL_AHBEN_FLASH_Msk);
    //进入深度休眠模式前应当将HCLK的时钟频率修改为不大于4MHz
    RCC_HSI_Enable(RCC_HSIOSC_DIV12);  //配置系统时钟为HSI 4M

    PWR_GotoLpmMode();

    /* Configures system clock after wake-up from DeepSleep: enable HSI and PLL with HSI as source*/
    RCC_Configuration();
    /* Initialize LED on the board */
    //resume rcc gpio adc
		RCC_Configuration();   //系统时钟64M
	KEYGPIO_Init();
	GPIO_WritePin(CW_GPIOC,GPIO_PIN_13,GPIO_Pin_RESET); 
	Seg_Init();
	Btim1_Init();
	ADC_init(); 
		
	read_vol_cur_calibration();
	ComputeK();
}

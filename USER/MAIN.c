
#include "main.h" 
#include "Seg_Dis.h"
#include "BTIM1.h"
#include "ADC.h"
#include "flash.h"

extern uint16_t Volt_Buffer[ADC_SAMPLE_SIZE];
extern uint16_t Curr_Buffer[ADC_SAMPLE_SIZE];
extern uint8_t Seg_Reg[6];
uint16_t V_Buffer,I_Buffer;

unsigned int spp_start=0;
uint32_t ble_time=0;

unsigned int timecount=0;
unsigned int sleepcount=0;

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
// mode 2: CAL.I05,ci5.
// mode 3: CAL.I15,ciF.
// mode 4: CAL.U05,cu5.
// mode 5: CAL.U15,cuF.
typedef enum
{
    TEST_MODE_VO = 0,
	TEST_MODE_CU,
	CALI_MODE_U05,
	CALI_MODE_U15,
	CALI_MODE_I05,
	CALI_MODE_I15,
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


void KEYGPIO_Init(void)
{
	__RCC_GPIOA_CLK_ENABLE();//打开GPIOB的时钟
	__RCC_GPIOC_CLK_ENABLE();//打开GPIOC的时钟
	GPIO_InitTypeDef GPIO_InitStruct;
		
	GPIO_InitStruct.Pins = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10; //K1 K2 K3
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
  GPIO_InitStruct.IT = GPIO_IT_NONE;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
	
	
	GPIO_InitStruct.Pins = GPIO_PIN_13; //LED
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Init(CW_GPIOC, &GPIO_InitStruct);
}

extern void DisplaySETV(uint32_t value);
void DisplayBuff(void)
{
	DisplayMode(currentMode);
	switch(currentMode)
	{
		case TEST_MODE_VO:
			Display(V_Buffer);
			break;
		case TEST_MODE_CU:
			if(I_Buffer>400)I_Buffer=400;
			DisplayI(I_Buffer);
			break;
		case CALI_MODE_U05:
		case CALI_MODE_U15:
			DisplaySETV(V_Buffer);
			break;
		case CALI_MODE_I05:
		case CALI_MODE_I15:
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
// pwm beep
// 5306唤醒
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
		if(sleepcount>=60000)//1分钟后休眠
		{// 定时休眠
			_WFI();
			sleepcount=0;
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
		if(Mode == 5)
			Dis_Refresh();//数码管扫描显示
		
		
	  	if(GPIO_ReadPin(CW_GPIOA,GPIO_PIN_8)==GPIO_Pin_RESET)//K1切换模式
        {
			keytime++;
			sleepcount = 0;//有按键，重新计时
			if(keytime>=100 )
			{
				keytime=0;  //切换模式
				Mode++;
				if(Mode>=5)Mode=0;
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
			
				if(Mode==1)
				{
					X05=Mean_Value_Filter(Volt_Buffer,ADC_SAMPLE_SIZE);
					save_calibration();ComputeK();Volt_Cal();BrushFlag=1;Mode=0;
				}
				if(Mode==2)
				{
					X15=Mean_Value_Filter(Volt_Buffer,ADC_SAMPLE_SIZE);
					save_calibration();ComputeK();Volt_Cal();BrushFlag=1;Mode=0;
				}
				if(Mode==3)
				{
					IX05=Mean_Value_Filter(Curr_Buffer,ADC_SAMPLE_SIZE);
					save_calibration();ComputeK();Volt_Cal();BrushFlag=1;Mode=0;
				}
				if(Mode==4)
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
				sleepcount = 0;//有按键，重新计时
				if(keytime3>=100 )
				{
					keytime3=0;  //切换模式
					BrushFlag=1; //更新数码管
				}			 
		}
		else keytime3=0;			 
  }
  /* USER CODE END */
}

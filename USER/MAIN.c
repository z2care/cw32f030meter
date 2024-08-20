
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

//5V��15V У׼
unsigned int X05=0;
unsigned int X15=0;

unsigned int Y15=15;
unsigned int Y05=5;
float K; //б��


//0.5A��1.5A У׼
unsigned int IX05=0;
unsigned int IX15=0;

unsigned int IY15=150;
unsigned int IY05=50;
float KI; //б��

//����ģʽ
unsigned char Mode=0;
//mode0 :��ѹ����������ʾģʽ
//mode1 :��ѹ5VУ׼
//mode2 :��ѹ15VУ׼
//mode3 :����0.5AУ׼
//mode4 :����1.5AУ׼

unsigned char BrushFlag=0;  

void RCC_Configuration(void)
{
  /* 0. HSIʹ�ܲ�У׼ */
  RCC_HSI_Enable(RCC_HSIOSC_DIV6);

  /* 1. ����HCLK��PCLK�ķ�Ƶϵ����*/
  RCC_HCLKPRS_Config(RCC_HCLK_DIV1);
  RCC_PCLKPRS_Config(RCC_PCLK_DIV1);
  
  /* 2. ʹ��PLL��ͨ��PLL��Ƶ��64MHz */
  RCC_PLL_Enable(RCC_PLLSOURCE_HSI, 8000000, 6);     // HSI Ĭ�����Ƶ��8MHz
 // RCC_PLL_OUT();  //PC13�����PLLʱ��
  
  ///< ��ʹ�õ�ʱ��ԴHCLK����24M,С�ڵ���48MHz������FLASH ���ȴ�����Ϊ2 cycle
  ///< ��ʹ�õ�ʱ��ԴHCLK����48MHz������FLASH ���ȴ�����Ϊ3 cycle
  __RCC_FLASH_CLK_ENABLE();
  FLASH_SetLatency(FLASH_Latency_3);   
    
  /* 3. ʱ���л���PLL */
  RCC_SysClk_Switch(RCC_SYSCLKSRC_PLL);
  RCC_SystemCoreClockUpdate(48000000);	
	RCC_PCLKPRS_Config(RCC_PCLK_DIV8); //����HCLK �� PCLK�ķ�Ƶϵ��  6MHz
}


void KEYGPIO_Init(void)
{
	__RCC_GPIOB_CLK_ENABLE();//��GPIOB��ʱ��	
	__RCC_GPIOC_CLK_ENABLE();//��GPIOC��ʱ��	
	GPIO_InitTypeDef GPIO_InitStruct; 
		
	GPIO_InitStruct.Pins = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14; // 
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
  GPIO_InitStruct.IT = GPIO_IT_NONE;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
	
	
	GPIO_InitStruct.Pins = GPIO_PIN_13; // 
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
  GPIO_Init(CW_GPIOC, &GPIO_InitStruct);
}

extern void DisplaySETV(uint32_t value);
void DisplayBuff(void)
{
  if(Mode==0)
  {
		 Display(V_Buffer);
		
		 if(I_Buffer>400)I_Buffer=400;
		 DisplayI(I_Buffer);
	}		
	else if(Mode==1) //S.05.
	{
		Seg_Reg[0] =5+10;
		Seg_Reg[1] =0;
		Seg_Reg[2]=5+10;
		DisplaySETV(V_Buffer);
	}
	else if(Mode==2) //S.15.
	{
		Seg_Reg[0] =5+10;
		Seg_Reg[1] =1;
		Seg_Reg[2]=5+10;
		DisplaySETV(V_Buffer);
	}
	else if(Mode==3) //A.0.5
	{
		Seg_Reg[0] =20;
		Seg_Reg[1] =0+10;
		Seg_Reg[2]=5;
		DisplayI(I_Buffer);
	}
	else if(Mode==4) //A.1.5
	{
		Seg_Reg[0] =20;
		Seg_Reg[1] =1+10;
		Seg_Reg[2]=5;
		DisplayI(I_Buffer);
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
	  if(da[0]!=0xaa)//��ûУ׼��ʱ����������ֵ�����洢
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
	RCC_Configuration();   //ϵͳʱ��64M
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
			
			if(timecount>= 300)     //300ms�ı�һ���������ʾֵ//	
			{
				timecount=0;
				Volt_Cal();
				BrushFlag=1;
			}			
	}
}


uint32_t Mean_Value_Filter(uint16_t *value, uint32_t size)     //��ֵ�˲�
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
		//if(sum>1)sum+=4; ����У׼
    return sum;
}

void Volt_Cal(void)
{ 	
	float t,KT1;
	
	V_Buffer = Mean_Value_Filter(Volt_Buffer,ADC_SAMPLE_SIZE);//ʹ�þ�ֵ�˲�
	I_Buffer = Mean_Value_Filter(Curr_Buffer,ADC_SAMPLE_SIZE); //ʹ�þ�ֵ�˲�
	
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
	// ��������
    if(V_Buffer % 10 >= 5)
    {
        V_Buffer = V_Buffer / 10 + 1;  //10mVΪ��λ
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
		
		ledcount++;  //LED��
		if(ledcount>=1000)
		{PC13_TOG();ledcount=0;}
		
		
		timecount++;
    Dis_Refresh();//�����ɨ����ʾ
		
		 	if(GPIO_ReadPin(CW_GPIOB,GPIO_PIN_12)==GPIO_Pin_RESET)
       {
				 keytime++;
				 if(keytime>=100 )
				 {
					  keytime=0;  //�л�ģʽ
						Mode++;
						if(Mode>=5)Mode=0;
						BrushFlag=1; //���������
				 }			 
			 }
			else keytime=0;
			 
			if(GPIO_ReadPin(CW_GPIOB,GPIO_PIN_13)==GPIO_Pin_RESET&&Mode!=0)
       {
				 keytime2++;
				 if(keytime2>=100 )
				 {
					  keytime2=0;  //�л�ģʽ
					
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
			 
			 if(GPIO_ReadPin(CW_GPIOB,GPIO_PIN_13)==GPIO_Pin_RESET)
       {
				 keytime3++;
				 if(keytime3>=100 )
				 {
					  keytime3=0;  //�л�ģʽ
						Mode=0; 
						BrushFlag=1; //���������
				 }			 
			 }
			else keytime3=0;			 
  }
  /* USER CODE END */
}







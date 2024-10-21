#include "measurement.h"
#include "ADC.h"
#include "calibration.h"
#include "UART3.h"

uint16_t V_Buffer = 0;
uint16_t I_Buffer = 0;
uint32_t timeline = 0;

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
    timeline+=10;
	  printf("%d,%d\r\n",timeline,V_Buffer);
		
		
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

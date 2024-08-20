#include "ADC.h"

uint16_t Volt_Buffer[ADC_SAMPLE_SIZE];
uint16_t TD_Buffer[ADC_SAMPLE_SIZE];
uint16_t JZ_Buffer[ADC_SAMPLE_SIZE];
uint16_t Curr_Buffer[ADC_SAMPLE_SIZE];

	/*
	��ѹ ADCIN11->PB10->CH11
	ͨ�� ADCIN9 ->PB01->CH9
	��׼ ADCIN8 ->PB11->CH12
	���� ADCIN12->PA00->CH0
	*/
void ADC_init(void)
{
    ADC_InitTypeDef     ADC_InitStructure;         //ADC���ýṹ��
    ADC_SerialChTypeDef ADC_SerialChStructure;     //ADC����ͨ���ṹ��
	
    __RCC_GPIOA_CLK_ENABLE(); //��ADC��Ӧ����ʱ��
    __RCC_GPIOB_CLK_ENABLE(); //��ADC��Ӧ����ʱ��
    __RCC_ADC_CLK_ENABLE();   // ��ADCʱ��
	
    
    PB10_ANALOG_ENABLE();                   //ʹ��ģ������
    PB01_ANALOG_ENABLE();                   //ʹ��ģ������
    PB11_ANALOG_ENABLE();                   //ʹ��ģ������
    PA00_ANALOG_ENABLE();                   //ʹ��ģ������
	
    ADC_StructInit(&ADC_InitStructure);      // ADCĬ��ֵ��ʼ��
    ADC_InitStructure.ADC_ClkDiv     = ADC_Clk_Div128; //ADC����ʱ������ PCLK/4 = 6/4 = 1.5Mhz
	
/*�źŵ�ѹ�ϵ�ʱ�����Խ��Ͳο���ѹ����߷ֱ��ʡ� �ı�ο���ѹ��ͬ�������Ʊ�ʾ�ĵ�ѹֵ�ͻ᲻һ����
  ���Ķ����ƣ�ȫ1����ʾ�ľ�����Ĳο���ѹ���ڼ���ʵ�ʵ�ѹʱ������Ҫ���ο���ѹ���ǽ�ȥ��*/
    ADC_InitStructure.ADC_VrefSel    = ADC_Vref_BGR1p5;     //�ο���ѹ����Ϊ1.5V
    ADC_InitStructure.ADC_SampleTime = ADC_SampTime10Clk;   //���ڵ�ѹ�ź�Ϊ�����źţ�ADC����ʱ��Ϊʮ��ADC����������ȷ��׼ȷ

   	ADC_SerialChStructure.ADC_Sqr0Chmux  = ADC_SqrCh11;      //����ADC���У�PB01��ADC�ĵ�9ͨ��
    ADC_SerialChStructure.ADC_Sqr1Chmux  = ADC_SqrCh9;
    ADC_SerialChStructure.ADC_Sqr2Chmux  = ADC_SqrCh12;      //����ADC���У�PB01��ADC�ĵ�9ͨ��
    ADC_SerialChStructure.ADC_Sqr3Chmux  = ADC_SqrCh0;
    ADC_SerialChStructure.ADC_SqrEns     = ADC_SqrEns03;
    ADC_SerialChStructure.ADC_InitStruct = ADC_InitStructure; //ADC��ʼ��
		
    ADC_SerialChContinuousModeCfg(&ADC_SerialChStructure);   //ADC��������ת��ģʽ����
    ADC_ClearITPendingAll();           //���ADC�����ж�״̬
    ADC_Enable();                      // ADCʹ��
    ADC_SoftwareStartConvCmd(ENABLE);  //ADCת��������������
}

void Get_ADC_Value(void)
{
	static uint8_t cnt;
  ADC_GetSqr0Result(&Volt_Buffer[cnt]);
	//ADC_GetSqr1Result(&Volt_Buffer[cnt]);//ͨ���ݲ�����
	ADC_GetSqr2Result(&JZ_Buffer[cnt]);
  ADC_GetSqr3Result(&Curr_Buffer[cnt]);	
	
	cnt++;
	if(cnt >= ADC_SAMPLE_SIZE)
    {
        cnt = 0;				
    }
}
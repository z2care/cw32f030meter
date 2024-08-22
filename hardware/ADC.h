#ifndef _ADC_H_
#define _ADC_H_

#define ADC_SAMPLE_SIZE (100)
#define ADC_REF_VALUE (1500)
#define R2            (200) //单位：KΩ//用已有器件200k
#define R1            (10)

#include "stdint.h"
#include "cw32f030_gpio.h"
#include "cw32f030_adc.h"
#include "cw32f030_rcc.h"

extern uint16_t Volt_Buffer[ADC_SAMPLE_SIZE];
//uint16_t TD_Buffer[ADC_SAMPLE_SIZE];
//uint16_t JZ_Buffer[ADC_SAMPLE_SIZE];
extern uint16_t Curr_Buffer[ADC_SAMPLE_SIZE];

void ADC_init(void);
void Get_ADC_Value(void);
	
#endif

#ifndef __SEG_DIS_H
#define __SEG_DIS_H

#include "cw32f030_gpio.h"
#include "cw32f030_rcc.h"

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

void Seg_Init(void);
void Seg_Dis(uint8_t Pos,uint8_t Num);
void Close_Com(void);
void Display(uint32_t value);
void DisplayI(uint32_t value);
void DisplaySETV(uint32_t value);
void Dis_Refresh(void);
void DisplayMode(Select_Mode value);
#endif

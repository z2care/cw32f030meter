#ifndef __MESURE_H
#define __MESURE_H
#include "base_types.h"

extern uint16_t V_Buffer;
extern uint16_t I_Buffer;

uint32_t Mean_Value_Filter(uint16_t *value, uint32_t size);
void Volt_Cal(void);
//
#endif

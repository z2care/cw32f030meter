#ifndef _BUZZER_H_
#define _BUZZER_H_

#include <stdint.h>
#include "cw32f030_rcc.h"
#include "cw32f030_gpio.h"
#include "cw32f030_gtim.h"

void Buzzer_Init(void);
void buzzer_beep();
void buzzer_idle();

#endif

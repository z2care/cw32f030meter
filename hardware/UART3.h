#ifndef __UART3_H
#define __UART3_H

#include "cw32f030_gpio.h"
#include "cw32f030_rcc.h"
#include "cw32f030_uart.h"

#ifdef __GNUC__
    /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
    set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */



void UART3_Init(void);

#endif

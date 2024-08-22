#include "UART3.h"

void UART3_Init(void)
{
		__RCC_GPIOA_CLK_ENABLE();//打开GPIOA的时钟
		__RCC_UART3_CLK_ENABLE();//打开UART3的时钟

		GPIO_InitTypeDef GPIO_InitStructure;
		//UART TX RX 复用
		PB06_AFx_UART3TXD();
		PB07_AFx_UART3RXD();
    GPIO_InitStructure.Pins = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);

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

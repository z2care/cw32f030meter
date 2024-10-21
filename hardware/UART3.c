#include "UART3.h"

void UART3_Init(void)
{
	//����RCC
	RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_GPIOB, ENABLE);  	// ʹ��GPIOʱ��
	RCC_APBPeriphClk_Enable1(RCC_APB1_PERIPH_UART3, ENABLE);	// ʹ�ܴ���ʱ��
	
	// ����GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	
    //UART TX RX ����
    PB06_AFx_UART3TXD();
    PB07_AFx_UART3RXD();
	
    GPIO_InitStructure.Pins = GPIO_PIN_6;						// ����
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;				// �������
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;					// ����ٶȸ�
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);					// ��ʼ��GPIO
	
    GPIO_InitStructure.Pins = GPIO_PIN_7;						// ����			
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;			// ��������
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);					// ��ʼ��GPIO
	
	
	// ����UART
	USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;				// ������
    USART_InitStructure.USART_Over = USART_Over_16;				// ����USART�Ĺ������ʡ�
    USART_InitStructure.USART_Source = USART_Source_PCLK;		// ����ʱ��Դ
    USART_InitStructure.USART_UclkFreq = 6000000;  			//����USARTʱ��Ƶ��(����Ƶһ�¼���)
    USART_InitStructure.USART_StartBit = USART_StartBit_FE;	 	//RXD�½��ؿ�ʼ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;		// ֹͣλ1
    USART_InitStructure.USART_Parity = USART_Parity_No ;		// ��ʹ��У��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��ʹ������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// �շ�ģʽ
    USART_Init(CW_UART3, &USART_InitStructure);						// ��ʼ������1
	
	//���ȼ��������ȼ�����
    NVIC_SetPriority(UART3_IRQn, 0);
    //UARTx�ж�ʹ��
    NVIC_EnableIRQ(UART3_IRQn);
	
	//ʹ��UARTx RC�ж�
    USART_ITConfig(CW_UART3, USART_IT_RC, ENABLE);

}

#if !defined(__MICROLIB)
//��ʹ��΢��Ļ�����Ҫ�������ĺ���
#if (__ARMCLIB_VERSION <= 6000000)
//�����������AC5  �Ͷ�����������ṹ��
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
#endif

/******************************************************************
 * �� �� �� �ƣ�usart_send_data
 * �� �� ˵ ��������һ���ֽڵ�����
 * �� �� �� �Σ�ucch��һ���ֽڵ�����
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void usart_send_data(uint8_t ucch)
{
	// ����һ���ֽ�
    USART_SendData_8bit(CW_UART3, (uint8_t)ucch);
	
	// �ȴ��������
	while( RESET == USART_GetFlagStatus(CW_UART3, USART_FLAG_TXE) ){}
}

/******************************************************************
 * �� �� �� �ƣ�usart_send_String
 * �� �� ˵ ���������ַ�������
 * �� �� �� �Σ�ucstr��һ��ָ�� uint8_t ���͵�ָ�룬��ʾ�ַ�������ʼ��ַ��
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void usart_send_String(uint8_t *ucstr)
{   
      while(ucstr && *ucstr)  // ��ַΪ�ջ���ֵΪ������   
      {     
        usart_send_data(*ucstr++);    
      }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
	// ����һ���ֽ�
    USART_SendData_8bit(CW_UART3, (uint8_t)ch);
	
	// �ȴ��������
	while( RESET == USART_GetFlagStatus(CW_UART3, USART_FLAG_TXE) ){}
	
    return ch;
}

void UART3_IRQHandler(void)
{
    uint8_t TxRxBuffer;
	
    if (USART_GetITStatus(CW_UART3, USART_IT_RC) != RESET)
    {
		// ����һ���ֽ�
        TxRxBuffer = USART_ReceiveData_8bit(CW_UART3);
		
        USART_SendData_8bit(CW_UART3, TxRxBuffer); // �����ݷ���������֤
		
		// �����־λ
        USART_ClearITPendingBit(CW_UART3, USART_IT_RC);
    }
}
#include "UART3.h"

void UART3_Init(void)
{
	//配置RCC
	RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_GPIOB, ENABLE);  	// 使能GPIO时钟
	RCC_APBPeriphClk_Enable1(RCC_APB1_PERIPH_UART3, ENABLE);	// 使能串口时钟
	
	// 配置GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	
    //UART TX RX 复用
    PB06_AFx_UART3TXD();
    PB07_AFx_UART3RXD();
	
    GPIO_InitStructure.Pins = GPIO_PIN_6;						// 引脚
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;				// 推挽输出
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;					// 输出速度高
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);					// 初始化GPIO
	
    GPIO_InitStructure.Pins = GPIO_PIN_7;						// 引脚			
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;			// 上拉输入
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);					// 初始化GPIO
	
	
	// 配置UART
	USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;				// 波特率
    USART_InitStructure.USART_Over = USART_Over_16;				// 配置USART的过采样率。
    USART_InitStructure.USART_Source = USART_Source_PCLK;		// 设置时钟源
    USART_InitStructure.USART_UclkFreq = 6000000;  			//设置USART时钟频率(和主频一致即可)
    USART_InitStructure.USART_StartBit = USART_StartBit_FE;	 	//RXD下降沿开始
    USART_InitStructure.USART_StopBits = USART_StopBits_1;		// 停止位1
    USART_InitStructure.USART_Parity = USART_Parity_No ;		// 不使用校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 不使用流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// 收发模式
    USART_Init(CW_UART3, &USART_InitStructure);						// 初始化串口1
	
	//优先级，无优先级分组
    NVIC_SetPriority(UART3_IRQn, 0);
    //UARTx中断使能
    NVIC_EnableIRQ(UART3_IRQn);
	
	//使能UARTx RC中断
    USART_ITConfig(CW_UART3, USART_IT_RC, ENABLE);

}

#if !defined(__MICROLIB)
//不使用微库的话就需要添加下面的函数
#if (__ARMCLIB_VERSION <= 6000000)
//如果编译器是AC5  就定义下面这个结构体
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
#endif

/******************************************************************
 * 函 数 名 称：usart_send_data
 * 函 数 说 明：发送一个字节的数据
 * 函 数 形 参：ucch：一个字节的数据
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void usart_send_data(uint8_t ucch)
{
	// 发送一个字节
    USART_SendData_8bit(CW_UART3, (uint8_t)ucch);
	
	// 等待发送完成
	while( RESET == USART_GetFlagStatus(CW_UART3, USART_FLAG_TXE) ){}
}

/******************************************************************
 * 函 数 名 称：usart_send_String
 * 函 数 说 明：发送字符串数据
 * 函 数 形 参：ucstr：一个指向 uint8_t 类型的指针，表示字符串的起始地址。
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void usart_send_String(uint8_t *ucstr)
{   
      while(ucstr && *ucstr)  // 地址为空或者值为空跳出   
      {     
        usart_send_data(*ucstr++);    
      }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
	// 发送一个字节
    USART_SendData_8bit(CW_UART3, (uint8_t)ch);
	
	// 等待发送完成
	while( RESET == USART_GetFlagStatus(CW_UART3, USART_FLAG_TXE) ){}
	
    return ch;
}

void UART3_IRQHandler(void)
{
    uint8_t TxRxBuffer;
	
    if (USART_GetITStatus(CW_UART3, USART_IT_RC) != RESET)
    {
		// 接收一个字节
        TxRxBuffer = USART_ReceiveData_8bit(CW_UART3);
		
        USART_SendData_8bit(CW_UART3, TxRxBuffer); // 将数据发回用于验证
		
		// 清除标志位
        USART_ClearITPendingBit(CW_UART3, USART_IT_RC);
    }
}
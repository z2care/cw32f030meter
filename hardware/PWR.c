#include "PWR.h"
#include "main.h"
#include "Seg_Dis.h"
#include "BTIM1.h"
#include "ADC.h"
#include "calibration.h"

void processDeepSleep(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    PWR_InitTypeDef PWR_InitStructure;
    /* Configure all GPIO as analog to reduce current consumption on non used IOs */
    /* Warning : Reconfiguring all GPIO will close the connexion with the debugger */
    /* Enable GPIOs clock */
    //��GPIOʱ��
    REGBITS_SET(CW_SYSCTRL->AHBEN, SYSCTRL_AHBEN_GPIOA_Msk | SYSCTRL_AHBEN_GPIOB_Msk | \
                SYSCTRL_AHBEN_GPIOC_Msk | SYSCTRL_AHBEN_GPIOF_Msk);
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.IT = GPIO_IT_NONE;
    GPIO_InitStructure.Pins = GPIO_PIN_All;
    GPIO_Init(CW_GPIOA, &GPIO_InitStructure);
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);
    GPIO_Init(CW_GPIOC, &GPIO_InitStructure);
    GPIO_Init(CW_GPIOF, &GPIO_InitStructure);
    //�ر�GPIOʱ��
    REGBITS_CLR(CW_SYSCTRL->AHBEN, SYSCTRL_AHBEN_GPIOA_Msk | SYSCTRL_AHBEN_GPIOB_Msk | \
                SYSCTRL_AHBEN_GPIOC_Msk | SYSCTRL_AHBEN_GPIOF_Msk);

    /* Configure User Button */
    KEYGPIO_Init();

    /* Configures system clock after wake-up from DeepSleep: enable HSI and PLL with HSI as source*/
    // ���Ѻ��Զ�ʹ���ڲ�����ʱ�ӣ�HSI��
    RCC_WAKEUPCLK_Config(RCC_SYSCTRL_WAKEUPCLKEN);

    /* Enter Stop Mode */
    PWR_InitStructure.PWR_Sevonpend = PWR_Sevonpend_Disable;
    PWR_InitStructure.PWR_SleepDeep = PWR_SleepDeep_Enable;
    PWR_InitStructure.PWR_SleepOnExit = PWR_SleepOnExit_Disable;
    PWR_Config(&PWR_InitStructure);

    //��FLASHʱ��
    REGBITS_SET(CW_SYSCTRL->AHBEN, SYSCTRL_AHBEN_FLASH_Msk);
    REGBITS_SET(CW_FLASH->CR1, FLASH_CR1_STANDBY_Msk); //��FLASH�͹���ʹ�ܿ���
    //�ر�FLASHʱ��
    REGBITS_CLR(CW_SYSCTRL->AHBEN, SYSCTRL_AHBEN_FLASH_Msk);

    //�����������ģʽǰӦ����HCLK��ʱ��Ƶ���޸�Ϊ������4MHz
    RCC_HSI_Enable(RCC_HSIOSC_DIV12);  //����ϵͳʱ��ΪHSI 4M

    PWR_GotoLpmMode();

    /* Configures system clock after wake-up from DeepSleep: enable HSI and PLL with HSI as source*/
    RCC_Configuration();
    /* Initialize LED on the board */
    //resume rcc gpio adc
		KEYGPIO_Init();
		LED_Init();
		GPIO_WritePin(CW_GPIOC,GPIO_PIN_13,GPIO_Pin_RESET); 
		Seg_Init();
		Btim1_Init();
		ADC_init(); 
		
		read_vol_cur_calibration();
		ComputeK();
}

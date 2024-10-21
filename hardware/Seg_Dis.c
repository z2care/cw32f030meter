#include "Seg_Dis.h"

/*
 0x3f   0x06   0x5b  0x4f  0x66  0x6d  0x7d  0x07  0x7f  0x6f 
  0      1      2     3     4     5     6     7     8     9 
 0xbf   0x86   0xdb  0xcf  0xe6  0xed  0xfd  0x87  0xff  0xef           
  0.     1.     2.    3.    4.    5.    6.    7.    8.    9.
 0f7    0xF1   0x58  0x39  0x1C  0x3E  0xF8  0x40
  A      F.     c     C     u     U     t     -  */
 
             
uint8_t Seg_Table[28] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,
	                       0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef,
						             0xF7, 0xF1, 0x58, 0x39, 0x1C, 0x3E, 0xF8, 0x40};// 0xF7:A.

uint8_t Seg_Reg[6] = {1,2,3,4,5,6};



void Seg_Init(void)
{
	__RCC_GPIOA_CLK_ENABLE();
	__RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStruct; 
		
	GPIO_InitStruct.Pins = GPIO_PIN_3 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15; 
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.IT = GPIO_IT_NONE;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pins = GPIO_PIN_0 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
}

/*
power the pin for the display
Pos - which seg to be power
Num - which char to be shown
*/
void Seg_Dis(uint8_t Pos,uint8_t Num)
{
	int i;
	uint8_t Dis_Value;
	Dis_Value = Seg_Table[Num];

	for(i = 0; i < 8; i++)
	{
      switch(i)
        {
          case 0:
              GPIO_WritePin(CW_GPIOA,GPIO_PIN_3,(GPIO_PinState)((Dis_Value >> i) & 0x01));    //PA02,A
              break;
          case 1:
              GPIO_WritePin(CW_GPIOA,GPIO_PIN_1,(GPIO_PinState)((Dis_Value >> i) & 0x01));    //PA00,B
              break;
          case 2:
              GPIO_WritePin(CW_GPIOA,GPIO_PIN_5,(GPIO_PinState)((Dis_Value >> i) & 0x01));    //PA04,C
              break;
          case 3:
              GPIO_WritePin(CW_GPIOA,GPIO_PIN_7,(GPIO_PinState)((Dis_Value >> i) & 0x01));    //PA06,D
              break;
          case 4:
              GPIO_WritePin(CW_GPIOB,GPIO_PIN_0,(GPIO_PinState)((Dis_Value >> i) & 0x01));    //PA07,E
              break;
          case 5:
              GPIO_WritePin(CW_GPIOA,GPIO_PIN_2,(GPIO_PinState)((Dis_Value >> i) & 0x01));    //PA01,F
              break;
          case 6:
              GPIO_WritePin(CW_GPIOA,GPIO_PIN_4,(GPIO_PinState)((Dis_Value >> i) & 0x01));    //PA03,G
              break;
          case 7:
              GPIO_WritePin(CW_GPIOA,GPIO_PIN_6,(GPIO_PinState)((Dis_Value >> i) & 0x01));    //PA05,DP
              break;
          default:
              break;
				}
	}
	
	switch(Pos)
	{
	  case 0:
      GPIO_WritePin(CW_GPIOA,GPIO_PIN_11,GPIO_Pin_RESET);  //PA8,COM1
      break;
    case 1:
		  GPIO_WritePin(CW_GPIOA,GPIO_PIN_12,GPIO_Pin_RESET);  //PA9,COM2
      break;
    case 2:
		  GPIO_WritePin(CW_GPIOA,GPIO_PIN_15,GPIO_Pin_RESET);  //PA10,COM3
      break;
		case 3:
		  GPIO_WritePin(CW_GPIOB,GPIO_PIN_3,GPIO_Pin_RESET);  //PA11,COM4
      break;
    case 4:
		  GPIO_WritePin(CW_GPIOB,GPIO_PIN_4,GPIO_Pin_RESET);  //PA12,COM5
      break;
    case 5:
		  GPIO_WritePin(CW_GPIOB,GPIO_PIN_5,GPIO_Pin_RESET);  //PA15,COM6
      break;
		default:
      break;
	}
}

/**
 * @brief close the com
 * 
 */
void Close_Com(void)
{
  GPIO_WritePin(CW_GPIOB,GPIO_PIN_3,GPIO_Pin_SET);
	GPIO_WritePin(CW_GPIOB,GPIO_PIN_4,GPIO_Pin_SET);
	GPIO_WritePin(CW_GPIOB,GPIO_PIN_5,GPIO_Pin_SET);
  GPIO_WritePin(CW_GPIOA,GPIO_PIN_11,GPIO_Pin_SET);
	GPIO_WritePin(CW_GPIOA,GPIO_PIN_12,GPIO_Pin_SET);
	GPIO_WritePin(CW_GPIOA,GPIO_PIN_15,GPIO_Pin_SET);
}

//display the mode in pre 3 segs
void DisplayMode(Select_Mode value)
{
	uint8_t mode = (uint8_t)value;
	switch(mode)
	{
		case TEST_MODE_VO://tu-
            Seg_Reg[0] = 26;
            Seg_Reg[1] = 24;
            Seg_Reg[2] = 27;
            break;
		case TEST_MODE_CU://tc-
            Seg_Reg[0] = 26;
            Seg_Reg[1] = 22;
            Seg_Reg[2] = 27;
            break;
		case CALI_MODE_U05://CU5.
            Seg_Reg[0] = 23;
            Seg_Reg[1] = 25;
            Seg_Reg[2] = 15;
            break;
		case CALI_MODE_U15://CUF.
            Seg_Reg[0] = 23;
            Seg_Reg[1] = 25;
            Seg_Reg[2] = 21;
            break;
		case CALI_MODE_I0P5://C0.5
            Seg_Reg[0] = 23;
            Seg_Reg[1] = 10;
            Seg_Reg[2] = 5;
            break;
		case CALI_MODE_I1P5://C1.5
            Seg_Reg[0] = 23;
            Seg_Reg[1] = 11;
            Seg_Reg[2] = 5;
            break;
		default:
			break;
	}

}
//show vol in after 3 segs
void DisplaySETV(uint32_t value)
{
	uint8_t Thousands;
  uint8_t Hundreds;
  uint8_t Tens;
  uint8_t Units;
	
	Thousands = value / 1000;
	 if(Thousands > 0)
    {
       Units     = value % 10;
       value     = Units > 5 ? (value + 10) : value;
       Thousands = value / 1000 % 10;
       Hundreds  = value / 100 % 10;
       Tens      = value / 10 % 10;
			
       Seg_Reg[3] = Thousands;
       Seg_Reg[4] = Hundreds + 10;
       Seg_Reg[5] = Tens;
		}
		
	 else
	   {
	     Units     = value % 10;
	     Tens      = value / 10 % 10;
       Hundreds  = value / 100 % 10;

	     Seg_Reg[3] = Hundreds + 10;
	     Seg_Reg[4] = Tens;
	     Seg_Reg[5] = Units;
	   }
}
//display vol in pre 3 segs
void Display(uint32_t value)
{
	uint8_t Thousands;
  uint8_t Hundreds;
  uint8_t Tens;
  uint8_t Units; // ¸öÎ»Êý
	
	Thousands = value / 1000;
	 if(Thousands > 0)
    {
       Units     = value % 10;
       value     = Units > 5 ? (value + 10) : value;
       Thousands = value / 1000 % 10;
       Hundreds  = value / 100 % 10;
       Tens      = value / 10 % 10;
			
       // ÏÔÊ¾xx.x·ü
       Seg_Reg[0] = Thousands;
       Seg_Reg[1] = Hundreds + 10;
       Seg_Reg[2] = Tens;
		}
		
	 else
	   {
	     Units     = value % 10;
	     Tens      = value / 10 % 10;
       Hundreds  = value / 100 % 10;

	     Seg_Reg[0] = Hundreds + 10;
	     Seg_Reg[1] = Tens;
	     Seg_Reg[2] = Units;
	   }
}
//display I in after 3 segs
void DisplayI(uint32_t value)
{
	     Seg_Reg[3] = value/100 + 10;
       Seg_Reg[4] = value%100/10; 
       Seg_Reg[5] = value%10;	
}

/**
 * @brief refresh every 6 segs one by one
 *
 */
void Dis_Refresh()
{
  	static uint8_t num = 0;

	Close_Com();
	Seg_Dis(num,Seg_Reg[num]);
	num++;
	if(num > 6)
	{
		num = 0;
	}
}


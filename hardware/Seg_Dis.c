#include "Seg_Dis.h"

/*  ��������ܱ����
 0x3f   0x06   0x5b  0x4f  0x66  0x6d  0x7d  0x07  0x7f  0x6f 
  0      1      2     3     4     5     6     7     8     9 
 0xbf   0x86   0xdb  0xcf  0xe6  0xed  0xfd  0x87  0xff  0xef           
  0.     1.     2.    3.    4.    5.    6.    7.    8.    9.          */
 
             
uint8_t Seg_Table[21] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,
	0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef,0xF7};// 0xF7:A.

/*
0   t:defg:78
1   E:adefg:79
2   S=5:acdfg:6D
3   U:bcdef:3E
4   o:cdeg:5C
5   C:adef:39
6   u:cde:1C
7   t.:defgDP:F8
8   -:g:40
9   A:abcefg:77
10  L.:defDP:B8
11  I:ef:30
12  0:abcdef:3F
13  1:bc:06
14  2:abdeg:5b
*/
uint8_t Seg_Table2[15] = {0x78, 0x79, 0x6D, 0x3E, 0x5C, 0x39, 0x1C, 0xF8, 0x40, 0x77,
                          0xB8, 0x30, 0x3F, 0x06, 0x5B};


uint8_t Seg_Reg[6] = {1,2,3,4,5,6};



void Seg_Init(void)
{
	__RCC_GPIOA_CLK_ENABLE();//��GPIOA��ʱ��
	__RCC_GPIOB_CLK_ENABLE();//��GPIOB��ʱ��
	
	GPIO_InitTypeDef GPIO_InitStruct; 
		
	GPIO_InitStruct.Pins = GPIO_PIN_3 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15; 
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.IT = GPIO_IT_NONE;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pins = GPIO_PIN_0 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
}

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

void Seg_Dis2(uint8_t Pos,uint8_t Num)
{
	int i;
	uint8_t Dis_Value;
	Dis_Value = Seg_Table2[Num];
	
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
 * @brief �ر����й�����
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

/*
mode 0: TEST.VO
mode 1: TEST.CU
mode 2: TEST.-O
mode 3: CAL.I05
mode 4: CAL.I15
mode 5: CAL.2U5
mode 6: CAL.U05
mode 7: CAL.U15
*/
/*
0   t:defg:78
1   E:adefg:79
2   S=5:acdfg:6D
3   U:bcdef:3E
4   o:cdeg:5C
5   C:adef:39
6   u:cde:1C
7   t.:DPdefg:F8
8   -:g:40
9   A:abcefg:77
10  L.:def:B8
11  I:ef:30
12  0:abcdef:3F
13  1:bc:06
14  2:abdeg:5b
*/
void DisplayMode(uint32_t value)
{
  switch(value)
	{
		case 0://tESt.Uo
			Seg_Reg[0] = 0;
			Seg_Reg[1] = 1;
			Seg_Reg[2] = 2;
			Seg_Reg[3] = 7;
			Seg_Reg[4] = 3;
			Seg_Reg[5] = 4;
			break;
		case 1://tESt.Cu
			Seg_Reg[0] = 0;
			Seg_Reg[1] = 1;
			Seg_Reg[2] = 2;
			Seg_Reg[3] = 7;
			Seg_Reg[4] = 5;
			Seg_Reg[5] = 6;
			break;
		case 2://TEST.-O
			Seg_Reg[0] = 0;
			Seg_Reg[1] = 1;
			Seg_Reg[2] = 2;
			Seg_Reg[3] = 7;
			Seg_Reg[4] = 8;
			Seg_Reg[5] = 12;
			break;
		case 3://CAL.I05
			Seg_Reg[0] = 5;
			Seg_Reg[1] = 9;
			Seg_Reg[2] = 10;
			Seg_Reg[3] = 11;
			Seg_Reg[4] = 12;
			Seg_Reg[5] = 2;
			break;
		case 4://CAL.I15
			Seg_Reg[0] = 5;
			Seg_Reg[1] = 9;
			Seg_Reg[2] = 10;
			Seg_Reg[3] = 11;
			Seg_Reg[4] = 13;
			Seg_Reg[5] = 2;
			break;
		case 5://CAL.2U5
			Seg_Reg[0] = 5;
			Seg_Reg[1] = 9;
			Seg_Reg[2] = 10;
			Seg_Reg[3] = 14;
			Seg_Reg[4] = 3;
			Seg_Reg[5] = 2;
			break;
		case 6://CAL.U05
			Seg_Reg[0] = 5;
			Seg_Reg[1] = 9;
			Seg_Reg[2] = 10;
			Seg_Reg[3] = 3;
			Seg_Reg[4] = 12;
			Seg_Reg[5] = 2;
			break;
		case 7://CAL.U15
			Seg_Reg[0] = 5;
			Seg_Reg[1] = 9;
			Seg_Reg[2] = 10;
			Seg_Reg[3] = 3;
			Seg_Reg[4] = 13;
			Seg_Reg[5] = 2;
			break;
		default:
			break;
	}
	
}

void DisplaySETV(uint32_t value)
{
	uint8_t Thousands;
  uint8_t Hundreds;
  uint8_t Tens;
  uint8_t Units; // ��λ��
	
	Thousands = value / 1000;
	 if(Thousands > 0)
    {
       Units     = value % 10;
       value     = Units > 5 ? (value + 10) : value; // ���ݺ�һλ��������
       Thousands = value / 1000 % 10;
       Hundreds  = value / 100 % 10;
       Tens      = value / 10 % 10;
			
       // ��ʾxx.x��
       Seg_Reg[3] = Thousands;
       Seg_Reg[4] = Hundreds + 10; // ��dp��ʾ
       Seg_Reg[5] = Tens;
		}
		
	 else
	   {
	     Units     = value % 10;
	     Tens      = value / 10 % 10;
       Hundreds  = value / 100 % 10;
	     
			 // ��ʾx.xx��
	     Seg_Reg[3] = Hundreds + 10;              // ��dp��ʾ
	     Seg_Reg[4] = Tens;
	     Seg_Reg[5] = Units;
	   }
}

void Display(uint32_t value)
{
	uint8_t Thousands;
  uint8_t Hundreds;
  uint8_t Tens;
  uint8_t Units; // ��λ��
	
	Thousands = value / 1000;
	 if(Thousands > 0)
    {
       Units     = value % 10;
       value     = Units > 5 ? (value + 10) : value; // ���ݺ�һλ��������
       Thousands = value / 1000 % 10;
       Hundreds  = value / 100 % 10;
       Tens      = value / 10 % 10;
			
       // ��ʾxx.x��
       Seg_Reg[0] = Thousands;
       Seg_Reg[1] = Hundreds + 10; // ��dp��ʾ
       Seg_Reg[2] = Tens;
		}
		
	 else
	   {
	     Units     = value % 10;
	     Tens      = value / 10 % 10;
       Hundreds  = value / 100 % 10;
	     
			 // ��ʾx.xx��
	     Seg_Reg[0] = Hundreds + 10;              // ��dp��ʾ
	     Seg_Reg[1] = Tens;
	     Seg_Reg[2] = Units;
	   }
}

void DisplayI(uint32_t value)
{
	     Seg_Reg[3] = value/100 + 10;// ��dp��ʾ
       Seg_Reg[4] = value%100/10; 
       Seg_Reg[5] = value%10;	
}

/**
 * @brief �����ɨ����ʾ����,��ʱ�������Ե���
 *
 */
void Dis_Refresh(void)
{
  static uint8_t num = 0;

	Close_Com();//�ȹرչ�����,��ֹ��Ӱ
	Seg_Dis(num,Seg_Reg[num]);
	num++;
  if(num > 6)
  {
    num = 0;
  }
}

void Dis_Refresh2(void)
{
  static uint8_t num2 = 0;

	Close_Com();//�ȹرչ�����,��ֹ��Ӱ
	Seg_Dis2(num2,Seg_Reg[num2]);
	num2++;
  if(num2 > 6)
  {
    num2 = 0;
  }
}

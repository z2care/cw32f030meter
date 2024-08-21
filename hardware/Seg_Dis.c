#include "Seg_Dis.h"

/*  共阴数码管编码表：
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
	__RCC_GPIOA_CLK_ENABLE();//打开GPIOA的时钟
	__RCC_GPIOB_CLK_ENABLE();//打开GPIOB的时钟
	
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
另一边显示数码管数据
Pos第几位
Num表中第几个数
inTab在第几个表，默认表1
*/
void Seg_Dis(uint8_t Pos,uint8_t Num, uint8_t inTab=1)
{
	int i;
	uint8_t Dis_Value;
	if(inTab == 1)
	Dis_Value = Seg_Table[Num];
	else
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
 * @brief 关闭所有公共端
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
mode 0: TEST.VO,tu-
mode 1: TEST.CU,tc-
mode 2: TEST.-O,tb-
mode 3: CAL.I05,cI5.
mode 4: CAL.I15,cIF.
mode 5: CAL.2U5,cUb.
mode 6: CAL.U05,cU5.
mode 7: CAL.U15,cUF.
*/
/*-bcituIU5F 5. b. F.
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
//一遍配置好MODE的数码管数据，按照表2
/*
    TEST_MODE_VO = 0,
	TEST_MODE_CU,
	TEST_MODE_IO,//unused
	CALI_MODE_2U5,
	CALI_MODE_U05,
	CALI_MODE_U15,
	CALI_MODE_I05,
	CALI_MODE_I15,
	*/
void DisplayMode(Select_Mode value)
{
	switch(value)
	{
		case TEST_MODE_VO:
		break;
		case TEST_MODE_CU:
		break;
		case TEST_MODE_IO:
		break;
		case CALI_MODE_2U5:
		break;
		case CALI_MODE_U05:
		break;
		case CALI_MODE_U15:
		break;
		case CALI_MODE_I05:
		break;
		case CALI_MODE_I15:
		break;
	}

}
//一边配置好6位数码管数据，按照表1
void DisplaySETV(uint32_t value)
{
	uint8_t Thousands;
  uint8_t Hundreds;
  uint8_t Tens;
  uint8_t Units; // 个位数
	
	Thousands = value / 1000;
	 if(Thousands > 0)
    {
       Units     = value % 10;
       value     = Units > 5 ? (value + 10) : value; // 根据后一位四舍五入
       Thousands = value / 1000 % 10;
       Hundreds  = value / 100 % 10;
       Tens      = value / 10 % 10;
			
       // 显示xx.x伏
       Seg_Reg[3] = Thousands;
       Seg_Reg[4] = Hundreds + 10; // 加dp显示
       Seg_Reg[5] = Tens;
		}
		
	 else
	   {
	     Units     = value % 10;
	     Tens      = value / 10 % 10;
       Hundreds  = value / 100 % 10;
	     
			 // 显示x.xx伏
	     Seg_Reg[3] = Hundreds + 10;              // 加dp显示
	     Seg_Reg[4] = Tens;
	     Seg_Reg[5] = Units;
	   }
}
//一边配置好6位数码管数据，按照表1
void Display(uint32_t value)
{
	uint8_t Thousands;
  uint8_t Hundreds;
  uint8_t Tens;
  uint8_t Units; // 个位数
	
	Thousands = value / 1000;
	 if(Thousands > 0)
    {
       Units     = value % 10;
       value     = Units > 5 ? (value + 10) : value; // 根据后一位四舍五入
       Thousands = value / 1000 % 10;
       Hundreds  = value / 100 % 10;
       Tens      = value / 10 % 10;
			
       // 显示xx.x伏
       Seg_Reg[0] = Thousands;
       Seg_Reg[1] = Hundreds + 10; // 加dp显示
       Seg_Reg[2] = Tens;
		}
		
	 else
	   {
	     Units     = value % 10;
	     Tens      = value / 10 % 10;
       Hundreds  = value / 100 % 10;
	     
			 // 显示x.xx伏
	     Seg_Reg[0] = Hundreds + 10;              // 加dp显示
	     Seg_Reg[1] = Tens;
	     Seg_Reg[2] = Units;
	   }
}
//一边配置好6位数码管数据，按照表1
void DisplayI(uint32_t value)
{
	     Seg_Reg[3] = value/100 + 10;// 加dp显示
       Seg_Reg[4] = value%100/10; 
       Seg_Reg[5] = value%10;	
}

/**
 * @brief 数码管扫描显示函数,定时器周期性调用
 *触发显示的位置，指定table，默认1
 */
void Dis_Refresh(uint8_t intab=1)
{
  static uint8_t num = 0;

	Close_Com();//先关闭公共端,防止重影
	if(intab==1)
	Seg_Dis(num,Seg_Reg[num]);
	else
	Seg_Dis(num2,Seg_Reg[num2],2);//使用table2
	num++;
  if(num > 6)
  {
    num = 0;
  }
}
}

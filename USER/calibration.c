#include "calibration.h"
#include "base_types.h"
#include "flash.h"


//5V��15V У׼
unsigned int X05=0;
unsigned int X15=0;

unsigned int Y15=15;
unsigned int Y05=5;
float K; //б��

//0.5A��1.5A У׼
unsigned int IX05=0;
unsigned int IX15=0;

unsigned int IY15=150;
unsigned int IY05=50;
float KI; //б��

void save_calibration(void)
{
		uint16_t da[5];
	  da[0]=0xaa;
	  da[1]=X05;
	  da[2]=X15;
	  da[3]=IX05;
	  da[4]=IX15;
    flash_erase();
    flash_write(0,da,5);
}
/**
 * @brief
 *
 */
void read_vol_cur_calibration(void)
{
    uint16_t da[5];
    flash_read(0,da, 5);
	  if(da[0]!=0xaa)//��ûУ׼��ʱ����������ֵ�����洢
		{
			X15=15.0/23/1.5*4096;
			X05=5.0/23/1.5*4096;
			IX05=0.5*0.1/1.5*4096;
			IX15=1.5*0.1/1.5*4096;
      save_calibration();
		}
		else 
		{
			X05=da[1];
			X15=da[2];
			IX05=da[3];
			IX15=da[4];
		}
		
}

void ComputeK(void)
{
	K=(Y15-Y05);
	K=K/(X15-X05);
	
	KI=(IY15-IY05);
	KI=KI/(IX15-IX05);
}

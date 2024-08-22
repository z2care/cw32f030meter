#ifndef __CALI_H
#define __CALI_H

//5V��15V У׼
extern unsigned int X05;
extern unsigned int X15;

extern unsigned int Y15;
extern unsigned int Y05;
extern float K; //б��

//0.5A��1.5A У׼
extern unsigned int IX05;
extern unsigned int IX15;

extern unsigned int IY15;
extern unsigned int IY05;
extern float KI; //б��

void save_calibration(void);
void read_vol_cur_calibration(void);
void ComputeK(void);
void Volt_Cal(void);

#endif

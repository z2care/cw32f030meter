#ifndef __CALI_H
#define __CALI_H

//5V与15V 校准
extern unsigned int X05;
extern unsigned int X15;

extern unsigned int Y15;
extern unsigned int Y05;
extern float K; //斜率

//0.5A与1.5A 校准
extern unsigned int IX05;
extern unsigned int IX15;

extern unsigned int IY15;
extern unsigned int IY05;
extern float KI; //斜率

void save_calibration(void);
void read_vol_cur_calibration(void);
void ComputeK(void);
void Volt_Cal(void);

#endif

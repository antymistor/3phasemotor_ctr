#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

extern u8 Dir_of_Stepmptor;


void initEXTI(void);
void initPWM(void);
void setspeed(u8 dir,u16 speed);
void  Adc_Init(void);
u16 Get_Adc(void) ;
u16 Get_Adc_Average(void);
void init_stepmotor(void);
void set_stepmotor(u8 sw);
#endif

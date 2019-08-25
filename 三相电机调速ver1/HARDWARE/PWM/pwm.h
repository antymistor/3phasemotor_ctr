#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

void initPWM(void);
void setspeed(u16 speed);
void  Adc_Init(void);
u16 Get_Adc(void) ;
u16 Get_Adc_Average(void);
#endif

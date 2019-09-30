#ifndef __BAMOTOR_H
#define __BAMOTOR_H
#include "sys.h"
#include "config.h"

#if DAMODE
void initSPI(void);
void setspispeed(u16  data);
#else
void initPWM(void);
#endif
void setspeed(u8 dir,u16 speed);
void initDAC(void);
#endif

#ifndef __CONFIG_H
#define __CONFIG_H
#include "sys.h"
// Pin MAP:
// EXTI:PA3 PA4
// DAMODE=1,spi mode ,PA5=SCK PA6=MISO PA7=MOSI PA8=CSN
// DAMODE=0,pwm mode ,PA6=PWM
// SWICTH,PB10,PB11
// LIMOTOR, PA0,PA2
// ADC:PA1
extern u8 Dir_of_Stepmptor;
#define DAMODE 1  //ģ���ѹ�������ʽѡ��   1:spi   2:pwm
#define LMMODE 0  //˿��ֱ�ߵ���Ŀ��Ʒ�ʽ   1:stepmotor  2:BLmotor
#define IDt 'A'   //����������� A:�����˶�   B:�����˶�
#define IDr a
#endif
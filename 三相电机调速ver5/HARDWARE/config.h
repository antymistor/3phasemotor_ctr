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
#define DAMODE 1  //模拟电压的输出方式选择   1:spi   2:pwm
#define LMMODE 0  //丝杆直线电机的控制方式   1:stepmotor  2:BLmotor
#define IDt 'A'   //定义卷扬机编号 A:竖向运动   B:横向运动
#define IDr a
#endif
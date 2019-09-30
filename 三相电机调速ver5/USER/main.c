#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "bamotor.h"
#include "limotor.h"
#include "exti.h"
#include "adc.h"
#include "config.h"


 int main(void)
 {	
	
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);
	initDAC();
	Adc_Init();
	init_limotor();
	initEXTI();	 
	 while(1)
	{
 		;
	} 
}


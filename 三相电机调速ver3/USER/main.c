
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "pwm.h"
//ALIENTEK Mini STM32�����巶������8
//PWM���ʵ��   
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾

 int main(void)
 {	
	
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);
	initDAC();
	Adc_Init();
	 init_stepmotor();
	initEXTI();



	 while(1)
	{
 		
	} 
}


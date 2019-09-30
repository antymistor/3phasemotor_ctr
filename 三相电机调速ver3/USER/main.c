
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "pwm.h"
//ALIENTEK Mini STM32开发板范例代码8
//PWM输出实验   
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

 int main(void)
 {	
	
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);
	initDAC();
	Adc_Init();
	 init_stepmotor();
	initEXTI();



	 while(1)
	{
 		
	} 
}


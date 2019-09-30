#include "limotor.h"
#include "config.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
u8 Dir_of_Stepmptor=0;

void initEXTI(void)
{
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;

 	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	  GPIO_Init(GPIOA, &GPIO_InitStructure);
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//?????
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 

	  EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);	 
	
	
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure);  	  
		
		
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;	
		NVIC_Init(&NVIC_InitStructure);  	
}

void EXTI3_IRQHandler(void)
{
	delay_ms(1);
	if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3))
	{
  Dir_of_Stepmptor=1;
	}
  EXTI_ClearITPendingBit(EXTI_Line3); 
}

void EXTI4_IRQHandler(void)
{
	delay_ms(1);
	if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4))
	{
  Dir_of_Stepmptor=0;
	}

  EXTI_ClearITPendingBit(EXTI_Line4); 
}

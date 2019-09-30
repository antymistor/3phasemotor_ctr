#include "limotor.h"
#include "config.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"

#include "delay.h"

// Pin MAP:
// EXTI:PA3 PA4
// DAMODE=1,spi mode ,PA5=SCK PA6=MISO PA7=MOSI PA8=CSN
// DAMODE=0,pwm mode ,PA6=PWM
// SWICTH,PB10,PB11
// STEPMOTOR, PA0,PA2
// ADC:PA1

void init_limotor(void)
{
#if LMMMODE
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	TIM_OCInitTypeDef              TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	
	TIM_DeInit(TIM2);
	TIM_TimeBaseStructure.TIM_Period=999;                
  TIM_TimeBaseStructure.TIM_Prescaler=359;     
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM2, ENABLE);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;                              
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;    
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;    
  TIM_OCInitStructure.TIM_Pulse=500;
  
  TIM_OC1Init(TIM2,&TIM_OCInitStructure);  
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
  
	TIM_Cmd(TIM2,DISABLE);
	TIM_CtrlPWMOutputs(TIM2,ENABLE);
	TIM_SetCompare1(TIM2,500);
	
#else
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	
#endif
}


void set_limotor(u8 sw)
{
#if LMMODE
	if(!sw)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
		TIM_Cmd(TIM2,ENABLE);
	}
	else if(sw==1)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		TIM_Cmd(TIM2,ENABLE);
	}
	else
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
		TIM_Cmd(TIM2,DISABLE);
	}
#else
	if(!sw)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
		GPIO_SetBits(GPIOA,GPIO_Pin_0);
	}
	else if(sw==1)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		GPIO_SetBits(GPIOA,GPIO_Pin_0);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	}
#endif
}

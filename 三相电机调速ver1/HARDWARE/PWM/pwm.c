#include "pwm.h"

void initPWM(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	TIM_OCInitTypeDef              TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	TIM_DeInit(TIM3);
	TIM_TimeBaseStructure.TIM_Period=999;                
    TIM_TimeBaseStructure.TIM_Prescaler=35;     
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;                              
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;    
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;    
    TIM_OCInitStructure.TIM_Pulse=0;
  
  TIM_OC1Init(TIM3,&TIM_OCInitStructure);  
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  
	TIM_Cmd(TIM3,ENABLE);
	TIM_CtrlPWMOutputs(TIM3,ENABLE);
	TIM_SetCompare1(TIM3,500);
}




void setspeed(u16 speed)
{
	TIM_SetCompare1(TIM3,speed);
}


void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1); 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
	ADC_Init(ADC1, &ADC_InitStructure);	 

  
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);	 
	while(ADC_GetResetCalibrationStatus(ADC1));	
	ADC_StartCalibration(ADC1);	 
	while(ADC_GetCalibrationStatus(ADC1));	


}				  

u16 Get_Adc()   
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );		    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
	return ADC_GetConversionValue(ADC1);	
}





//带有滤波算法
u16 Get_Adc_Average()
{
	u32 temp_val=0;
	u16 t_val=0;
	u8 times=5;
	u8 times_max=50;  //设置一次测量的采样上线为50，超过则跳出
	
	while(times!=0&&times_max!=0)
	{
		times_max--;
		t_val=Get_Adc();
		if(times!=5)
		{
			if(t_val>(u16)(1.2f*temp_val/(5-times))||t_val<(u16)(0.8f*temp_val/(5-times)) )
			{times=6;temp_val=0;}
		}
		temp_val=temp_val+t_val;
		times--;
		times_max--;
	}
	return (u16)temp_val/(5-times);
}


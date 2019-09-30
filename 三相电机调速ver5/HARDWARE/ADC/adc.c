#include "adc.h"
#include "config.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"

#include "delay.h"

// Pin MAP:
// EXTI:PA3 PA4
// DAMODE=1,spi mode ,PA5=SCK PA6=MISO PA7=MOSI PA8=CSN
// DAMODE=0,pwm mode ,PA6=PWM
// SWICTH,PB10,PB11
// LIMOTOR, PA0,PA2
// ADC:PA1


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



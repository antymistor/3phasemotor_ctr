#include "pwm.h"
#include "stm32f10x_exti.h"
#include "delay.h"
u8 Dir_of_Stepmptor=0;



// Pin MAP:
// EXTI:PA3 PA4
// DAMODE=1,spi mode ,PA5=SCK PA6=MISO PA7=MOSI PA8=CSN
// DAMODE=0,pwm mode ,PA6=PWM
// SWICTH,PB10,PB11
// STEPMOTOR, PA0,PA2
// ADC:PA1
#define  CS_H()  GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define  CS_L()  GPIO_ResetBits(GPIOA,GPIO_Pin_8)


#define DAMODE 1  //模拟电压的输出方式选择   1:spi   2:pwm
#define LMMODE 0  //丝杆直线电机的控制方式   1:stepmotor  2:BLmotor
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

void initPWM(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	TIM_OCInitTypeDef              TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
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
	TIM_SetCompare1(TIM3,1);
}

void init_stepmotor(void)
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



void set_stepmotor(u8 sw)
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

void setspeed(u8 dir,u16 speed)
{
	if(dir==0)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_10);
		GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	}
	else if(dir==1)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_11);
		GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	}
	else
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_11);
		GPIO_SetBits(GPIOB,GPIO_Pin_10);
	}
#if DAMODE
	  setspispeed(speed);
#else
	  TIM_SetCompare1(TIM3,speed);
#endif
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






 void initSPI(void)
{         
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* Configure SPI1 pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//配置片选引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //上拉输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_8); 
	
	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;           
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;              
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;       
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;        
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;        
	SPI_InitStructure.SPI_CRCPolynomial = 7;           
	SPI_Init(SPI1, &SPI_InitStructure);             

	SPI_Cmd(SPI1, ENABLE);             
}  

void setspispeed(u16  data)
{
	u16 da_value =  data<<2;
	CS_L();
	delay_us(10);
	SPI_I2S_SendData(SPI1, (da_value>>8)&0XFF); 
	while (SPI_I2S_GetFlagStatus(
	SPI1, SPI_I2S_FLAG_TXE) == RESET);//等待发送结束 
	SPI_I2S_SendData(SPI1, da_value&0XFF); 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//等待发送结束 
	delay_us(10);
	CS_H();
	delay_us(2);
}

void initDAC(void)
{
#if DAMODE
	  initSPI();
#else
	  initPWM(); 
#endif
	setspeed(2,1);
}


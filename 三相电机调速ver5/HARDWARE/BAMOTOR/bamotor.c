#include "bamotor.h"
#include "config.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "delay.h"

// Pin MAP:
// EXTI:PA3 PA4
// DAMODE=1,spi mode ,PA5=SCK PA6=MISO PA7=MOSI PA8=CSN
// DAMODE=0,pwm mode ,PA6=PWM
// SWICTH,PB10,PB11
// STEPMOTOR, PA0,PA2
// ADC:PA1
#define  CS_H()  GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define  CS_L()  GPIO_ResetBits(GPIOA,GPIO_Pin_8)

#if DAMODE
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

#else
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
#endif




void initDAC(void)
{
#if DAMODE
	  initSPI();
#else
	  initPWM(); 
#endif
	setspeed(2,1);
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



#include "sys.h"
#include "delay.h"
#include "usart.h"	  
#include "pwm.h"

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数         

#define IDt 'A'
#define IDr a
u8 cnum=0;
u8 dectr[5];
u16 speed=0;
u16 power=500;
u16 out_speed=0;
int StSpe=700;
//u8 ID='A';
u16 time2run=100;

struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
 

void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

void USART1_IRQHandler(void)                	
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
		{
		Res =USART_ReceiveData(USART1);	
		if ( Res==IDt)
		{
			cnum=1;
			dectr[0]=Res;
		}
		else if(cnum>0)
		{
			if ( ( (Res>='0')&& (Res<='9') )  )
			{dectr[cnum]=Res;
			cnum++;
			}
			else
			cnum=0;
		}
		
		if(cnum==5)
		{
			cnum=0;
			if(dectr[1]=='0'||(dectr[1]=='1'||dectr[1]=='2'))
			{
				speed=(u16)((dectr[2]-'0')*100+(dectr[3]-'0')*10+(dectr[4]-'0'));
				setspeed(dectr[1]-'0',speed);
				//set_stepmotor(dectr[1]-'0');
				if(dectr[1]=='2')
				{
				set_stepmotor(2);
				}
				else
				{
					set_stepmotor(Dir_of_Stepmptor);
				}
				
				
			}
			else if(dectr[1]=='3'||dectr[1]=='4')
			{
				time2run=(u16)((dectr[3]-'0')*10+(dectr[4]-'0'));
				StSpe=(dectr[2]-'0')*100;
				while(StSpe>0)
				{
					setspeed(dectr[1]-'3',StSpe);
					delay_ms(time2run);
					StSpe=StSpe-20;
				}
				setspeed(2,1);
			}
			else if(dectr[1]=='4')
			{
				out_speed=Get_Adc_Average();
				printf("a%d\r\n",out_speed);
			}
		}

		}
} 


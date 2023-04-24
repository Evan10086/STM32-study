#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;



struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);		
	
	USART_SendData(USART1,ch);
	
	return(ch);
}


void delay_ms(uint32_t n)
{
	while(n--)
	{
	
		SysTick->CTRL = 0; 	// 关闭系统定时器
		SysTick->LOAD = 21000-1; //
		SysTick->VAL = 0; // 清空当前计数值寄存器且会清零COUNTFLAG标志位
		SysTick->CTRL = 1; // 使能系统定时器，它的时钟源为参考时钟=内核时钟（168MHz）/8
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
		
	}
	
	SysTick->CTRL = 0; // 关闭系统定时器

}

void delay_us(uint32_t n)
{

	SysTick->CTRL = 0; 	// 关闭系统定时器
	SysTick->LOAD = 168*n-1; // 1us*n
	SysTick->VAL = 0; // 清空当前计数值寄存器且会清零COUNTFLAG标志位
	SysTick->CTRL = 5; // 使能系统定时器，它的时钟源为内核时钟（168MHz）
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set

	SysTick->CTRL = 0; // 关闭系统定时器

}



void usart1_init(uint32_t baud)
{

	/* 打开串口1硬件时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* 打开端口A的硬件时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	
	/* 将GPIO引脚配置为复用功能模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//复用功能模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	/* 将GPIO引脚连接到串口1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	/* 配置串口1相关的参数：波特率、数据位 */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	/* 使能串口1中断相关：接收数据触发、中断优先级 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	/* 使能串口1工作 */
	USART_Cmd(USART1, ENABLE);

}


void usart1_send_str(char *str)
{

	char *p = str;
	
	while(*p)
	{
		
		/* 检测上一次是否发送完毕 */
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);		
		
		USART_SendData(USART1,*p);
		
		p++;
		
	}

}


void usart3_init(uint32_t baud)
{

	/* 打开串口1硬件时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	/* 打开端口B的硬件时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	/* 将GPIO引脚配置为复用功能模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//复用功能模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;//第10 11号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	/* 将GPIO引脚连接到串口3 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	/* 配置串口3相关的参数：波特率、数据位 */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	/* 使能串口3中断相关：接收数据触发、中断优先级 */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	/* 使能串口3工作 */
	USART_Cmd(USART3, ENABLE);

}


void usart3_send_str(char *str)
{

	char *p = str;
	
	while(*p)
	{
		
		/* 检测上一次是否发送完毕 */
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);		
		
		USART_SendData(USART3,*p);
		
		p++;
		
	}

}

void tim14_init(void)
{
	/* 打开TIM14的硬件时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

	/* 配置TIM14：输出频率、脉宽 */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/100-1; //定时频率等价于输出频率
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; //预分频值8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频为1，但是F407不支持，可以不用配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM的工作模式为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//允许输出脉冲
	TIM_OCInitStructure.TIM_Pulse = 100;//比较值，用于控制输出的脉宽
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//通道的有效状态为高电平
	
	/* 配置TIM14的通道1 */
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	
	/* 使能TIM14工作 */
	TIM_Cmd(TIM14, ENABLE);

}
void dht11_init(void)
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);


	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//第9号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	
	/* 根据时序图，PG9引脚初始状态需要输出为高电平*/
	PGout(9)=1;
}

int32_t dht11_read(uint8_t *buf)
{
	uint32_t t=0;
	uint8_t *p=buf;
	
	uint8_t d=0;
	uint8_t check_sum=0;
	
	int32_t i,j;
	
	/* 配置PG9引脚为开漏输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//第9号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOG,&GPIO_InitStructure);
		
	
	PGout(9)=0;
	
	/* 延时18ms */
	delay_ms(18);

	PGout(9)=1;
	
	/* 延时30us */
	delay_us(30);
	
	
	/* 配置PG9引脚为输入模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//输入模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//第9号引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOG,&GPIO_InitStructure);	

	/* 等待dht11输出响应信号的低电平，添加超时处理，防止程序卡死 */	
	t=0;
	while(PGin(9))
	{
		t++;
		
		delay_us(1);
		
		
		if(t>=4000)
			return -1;
	}
	
	/* 测量低电平的持续时间，添加超时处理，防止程序卡死 */
	t=0;
	while(PGin(9)==0)
	{
		delay_us(1); //延时1us
		
		t++;
		
		if(t>=100)
			return -2;
	}
	
	/* 测量高电平的持续时间，添加超时处理，防止程序卡死 */
	t=0;
	while(PGin(9))
	{
		delay_us(1); //延时1us
		
		t++;
		
		if(t>=100)
			return -3;
	}	
	
	
	/* 连续接收5个字节：湿度整数+湿度小数+温度整数+温度小数+校验和 */
	for(j=0; j<5; j++)
	{
		d=0;
		
		for(i=7; i>=0; i--)//MSB：bit7 -> bit0
		{
			
			/* 等待50us的低电平持续完毕 */
			t=0;
			while(PGin(9)==0)
			{
				delay_us(1); //延时1us
				
				t++;
				
				if(t>=100)
					return -4;
			}		

			/* 延时40us */
			delay_us(40);
			
			/* 判断当前引脚的电平是高电平/低电平 */
			
			if(PGin(9))
			{
				/* 将d变量对应的bit位置1 */
				d|=1<<i;
				
				/* 将剩下的高电平等待完毕 */
				t=0;
				while(PGin(9))
				{
					delay_us(1); //延时1us
					
					t++;
					
					if(t>=100)
						return -5;
				}					
			
			}
		}
		
		p[j]=d;
	}
	
	delay_us(50);
	
	/* 接收完5个字节后，就校验数据 */
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(p[4]!=check_sum)
		return -6;

	
	return 0;
}
volatile uint8_t  g_usart1_buf[64]={0};
volatile uint32_t g_usart1_cnt=0;
volatile uint32_t g_usart1_event=0;

int main(void)
{
	char *p;
	uint32_t i;
	uint8_t buf[5];
	int32_t rt;
	uint32_t duty;
	
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* 配置GPIOF的9号引脚为复用功能模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//复用功能模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	/* 将PF9引脚连接到TIM14 */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	
	PFout(9)=1;
	

	tim14_init();
	
	dht11_init();
	
	
	/* 串口1波特率为115200bps (注意:保证PLL配置正确，否则收发数据乱码)*/
	usart1_init(115200);
	
	
	/* 串口3波特率为9600bps (注意:保证PLL配置正确，否则收发数据乱码)*/
	usart3_init(9600);

	delay_ms(100);
	
	printf("This is ble ex3 test\r\n");
	
#if 0	
	/* 发送AT测试指令 */
	usart3_send_str("AT\r\n");
	
	delay_ms(100);
	
	/* 发送AT指令查询硬件地址 */
	usart3_send_str("AT+LADDR\r\n");
	
	delay_ms(100);
	
	/* 发送AT指令修改模块名字 */
	usart3_send_str("AT+NAMETWen\r\n");	
	
	delay_ms(100);
	
	/* 发送AT指令复位模块 */
	usart3_send_str("AT+RESET\r\n");	

	delay_ms(100);
	
	/* 发送AT指令查询模块名字 */
	usart3_send_str("AT+NAME\r\n");

#endif
	
	while(1)
	{
		if(g_usart1_event)
		{
			/* 添加字符串解析代码 */
#if 0			
			rt = sscanf((char *)g_usart1_buf,"duty=%d#",&duty);
			
			TIM_SetCompare1(TIM14,i);
				
			printf("duty set %d ok\r\n",i);
#else			
			//“duty=50#”
			if(strstr((const char *)g_usart1_buf,"duty"))
			{
				
				p=strtok((char *)g_usart1_buf,"=");
				
				if(p==NULL)
					goto end;
				
				printf("p=%s\r\n",p);//duty
			
				p=strtok(NULL,"=");
				
				if(p==NULL)
					goto end;
				
				printf("p=%s\r\n",p);//50#	

				i=atoi(p);//atoi("50#") -> 50
				
				TIM_SetCompare1(TIM14,i);
				
				printf("duty set %d ok\r\n",i);
			}
#endif		
			if(strstr((const char *)g_usart1_buf,"temp?#"))
			{
				rt = dht11_read(buf);
				
				
				if(rt < 0)
				{
					printf("dht11 read error %d\r\n",rt);
				}
			
				else
				{
					printf("%d.%d\r\n",buf[2],buf[3]);
#if 0					
					char tmp[64];
					
					sprintf(tmp,"%d.%d\r\n",buf[2],buf[3]);
					
					usart3_send_str(tmp);
#endif					
				}

			}
			
			if(strstr((const char *)g_usart1_buf,"humi?#"))
			{
				rt = dht11_read(buf);
				
				
				if(rt < 0)
				{
					printf("dht11 read error %d\r\n",rt);
				}
			
				else
				{
					printf("%d.%d\r\n",buf[0],buf[1]);
				}

			}			
			
end:		
			g_usart1_event=0;
			g_usart1_cnt=0;
			memset((void *)g_usart1_buf,0,sizeof(g_usart1_buf));
			
		}
			
	
	}
	
	//return 0;

}

void USART1_IRQHandler(void)
{
	uint8_t d;
	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
	
		d = USART_ReceiveData(USART1);
		
		g_usart1_buf[g_usart1_cnt]=d;
		
		g_usart1_cnt++;
		
		if(d == '#' || g_usart1_cnt>= sizeof(g_usart1_buf))
		{
		
			g_usart1_event=1;
		
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}


void USART3_IRQHandler(void)
{
	uint8_t d;
	
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
	
		d = USART_ReceiveData(USART3);
		
		if(d == '0')PFout(9)=0;
		if(d == 'a')PFout(9)=1;

		
		USART_SendData(USART1,d);
		USART_SendData(USART3,d);
		
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

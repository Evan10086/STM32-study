#include "stm32f4xx.h"
#include <stdio.h>
#include "sys.h"

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;




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





int main(void)
{
	uint8_t buf[5];
	
	int32_t rt;
	
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* 配置GPIOF的9号引脚为输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	dht11_init();
	
	/* 串口1波特率为115200bps (注意:保证PLL配置正确，否则收发数据乱码)*/
	usart1_init(115200);
	
	delay_ms(100);
	
	printf("This is dht11 test\r\n");
	
	
	while(1)
	{
		rt = dht11_read(buf);
		
		
		if(rt < 0)
		{
			printf("dht11 read error %d\r\n",rt);
		}
	
		else
		{
			printf("T:%d.%d H:%d.%d\r\n",buf[2],buf[3],buf[0],buf[1]);
		}
		
		
		delay_ms(6000);
			
	
	}
	
	//return 0;

}

void USART1_IRQHandler(void)
{
	uint8_t d;
	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
	
		d = USART_ReceiveData(USART1);
		
		USART_SendData(USART1,d);
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}




}

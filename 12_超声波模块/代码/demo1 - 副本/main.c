#include "stm32f4xx.h"
#include <stdio.h>


#define PFout(n) (*(uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))
#define PBout(n) (*(uint32_t *)(0x42000000+(GPIOB_BASE+0x14-0x40000000)*32+n*4))
#define PEin(n)  (*(uint32_t *)(0x42000000+(GPIOE_BASE+0x10-0x40000000)*32+n*4))

#define PFout(n) (*(uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))
#define PEout(n) (*(uint32_t *)(0x42000000+(GPIOE_BASE+0x14-0x40000000)*32+n*4))

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

void sr04_init(void)
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//第6号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//输入模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//第6号引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	/* 根据时序图，PB6引脚是连接到TRIG，初始状态需要输出为低电平*/
	PBout(6)=0;
}

int32_t sr04_get_distance(void)
{
	uint32_t t=0;
	int32_t d;
	
	/* 触发信号引脚输出高电平 */
	PBout(6)=1;
	
	/* 延时10us */
	delay_us(10);

	/* 触发信号引脚输出低电平 */	
	PBout(6)=0;	

	/* 等待回响信号引脚输出高电平，添加超时处理，防止程序卡死 */	
	t=0;
	while(PEin(6)==0)
	{
		t++;
		
		delay_us(1);
		
		
		if(t>=100*1000)
			return -1;
	}
	
	/* 测量高电平的持续时间，添加超时处理，防止程序卡死 */
	t=0;
	while(PEin(6))
	{
		delay_us(9); //延时9us，等价于声音传播3mm
		
		t++;
		
		if(t>=100*1000)
			return -2;
	
	}
	
	d = 3*t/2;
	
	
	return d;


}




int main(void)
{
	
	int32_t distance;
	
	

	
	
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	
	/* 配置GPIOF的9号引脚为输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* 配置GPIOF的9号引脚为输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	
	/* 配置GPIOF的8号引脚为输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;//第8号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	
	
	
	
//	PFout(9)=1;
//	PFout(10)=1;
//	PEout(13)=1;
//	PEout(14)=1;
	
	
	
	sr04_init();
	
	/* 串口1波特率为115200bps (注意:保证PLL配置正确，否则收发数据乱码)*/
	usart1_init(115200);
	
	delay_ms(100);
	
	printf("This is sr04 test\r\n");
	
	
	while(1)
	{
		distance = sr04_get_distance();
		
		
		if(distance < 0)
		{
			printf("sr04 get distance error %d\r\n",distance);
		}
		else if(distance>=20 && distance<=4000)
		{
			printf("sr04 get distance %d mm\r\n",distance);
			
			
			if(distance>=200){
				PFout(9)=1;
				PFout(10)=1;
				PEout(13)=1;
				PEout(14)=1;
			}
			
			if(distance<=200 && distance>=150){
				PFout(9)=0;
				PFout(10)=1;
				PEout(13)=1;
				PEout(14)=1;
				
			}
			
			if(distance<=149 && distance>=100){
				PFout(9)=0;
				PFout(10)=0;
				PEout(13)=1;
				PEout(14)=1;
			}
			
			if(distance<=99 && distance>=50){
				PFout(9)=0;
				PFout(10)=0;
				PFout(13)=0;
				PEout(14)=1;
				
			}
			
			
			if(distance<=49){
				PFout(9)=0;
				PFout(10)=0;
				PEout(13)=0;
				PEout(14)=0;
				PFout(8)=0;
				
			}
		
		
		}
		else
		{
			printf("sr04 get distance inval(%d)\r\n",distance);
		}
		
		
		
		
		
		delay_ms(100);
			
	
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



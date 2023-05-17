#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef   		NVIC_InitStructure;
static USART_InitTypeDef   		USART_InitStructure;


//使用到IDR寄存器
#define PAin(n)		(*(volatile uint32_t *)(0x42000000 + (GPIOA_BASE + 0x10 - 0x40000000)*32 + n*4))
#define PEin(n)		(*(volatile uint32_t *)(0x42000000 + (GPIOE_BASE + 0x10 - 0x40000000)*32 + n*4))
#define PGin(n)		(*(volatile uint32_t *)(0x42000000 + (GPIOG_BASE + 0x10 - 0x40000000)*32 + n*4))

//使用到ODR寄存器
#define PEout(n)	(*(volatile uint32_t *)(0x42000000 + (GPIOE_BASE + 0x14 - 0x40000000)*32 + n*4))
#define PFout(n)	(*(volatile uint32_t *)(0x42000000 + (GPIOF_BASE + 0x14 - 0x40000000)*32 + n*4))
#define PGout(n)	(*(volatile uint32_t *)(0x42000000 + (GPIOG_BASE + 0x14 - 0x40000000)*32 + n*4))	

#pragma import(__use_no_semihosting_swi)

struct __FILE { int32_t handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int32_t fputc(int32_t ch, FILE *f) 
{
	USART_SendData(USART1,ch);
		
	//等待数据发送成功
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);


	return ch;
}

void _sys_exit(int32_t return_code) {

}

void delay_us(uint32_t n)
{
	SysTick->CTRL = 0; // Disable SysTick
	SysTick->LOAD = n*168-1; // 计数值 
	SysTick->VAL  = 0; // Clear current value as well as count flag
	SysTick->CTRL = 5; // Enable SysTick timer with processor clock(168MHz)
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; // Disable SysTick	
}



void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; // Disable SysTick
		SysTick->LOAD = 168000-1; // 计数值 Count from n to 0 (168000-1+1=168000 cycles)
		SysTick->VAL = 0; // Clear current value as well as count flag
		SysTick->CTRL = 5; // Enable SysTick timer with processor clock(168MHz)
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
		SysTick->CTRL = 0; // Disable SysTick	
	
	}
}


void usart1_init(uint32_t baud)
{
	//使能端口A硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//使能串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	
	//配置PA9、PA10为复用功能引脚
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//将PA9、PA10连接到USART1的硬件
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	
	//配置USART1的相关参数：波特率、数据位、校验位
	USART_InitStructure.USART_BaudRate = baud;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//允许串口发送和接收数据
	USART_Init(USART1, &USART_InitStructure);
	
	
	//使能串口接收到数据触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//使能串口1工作
	USART_Cmd(USART1,ENABLE);
}

void usart3_init(uint32_t baud)
{
	//使能端口B硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	//使能串口3硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	
	//配置PB10、PB11为复用功能引脚
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//将PB10、PB11连接到USART3的硬件
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	
	//配置USART1的相关参数：波特率、数据位、校验位
	USART_InitStructure.USART_BaudRate = baud;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//允许串口发送和接收数据
	USART_Init(USART3, &USART_InitStructure);
	
	
	//使能串口接收到数据触发中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//使能串口3工作
	USART_Cmd(USART3,ENABLE);
}

void usart3_send_str(char *str)
{

	char *p = str;
	
	while(*p!='\0')
	{
		USART_SendData(USART3,*p);
		
		p++;
		
		
		//等待数据发送成功
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
	
	}
}



int32_t getNi(int32_t e, int32_t n)//求逆
{
	int32_t d;
	
	for (d = 0; d < n; d++) 
	{
		if (e * d % n == 1)
			break;
			
	}
	
	return d;
}
int32_t Gcd(int32_t a, int32_t b)//求最大公因数
{
	if (a % b == 0)
		return b;
	else
		return Gcd(b, a % b);
}

int32_t getrand(int32_t p,int32_t q)
{
	//产生随机数e
	int32_t m=(p-1)*(q-1);
	int32_t e,c;
	int32_t i=0;
	
	while (1) 
	{
		srand(i);
		
		i++;
		
		e = rand() % m;
		
		c = Gcd(e, m);
		
		if (c == 1)
			break;
	}
	
	return e;
}
	

void flash_rsa_encryption(int32_t e,int32_t n,uint32_t *buf,uint32_t len)
{
	int32_t i,j;
	int32_t flag = 1;
	uint32_t *p = buf;
	
	/* 解锁FLASH的写保护，就允许对FLASH写入数据 */
	FLASH_Unlock();	

	/* 擦除扇区4，电压设置为等级3，每个单元时间内擦除4个字节 */
	FLASH_EraseSector(FLASH_Sector_4,VoltageRange_3);
	
	for (i = 0; i < len; i++) 
	{
		for (j = 0; j < e; j++) 
		{
			flag = flag * p[i] % n;
		}
		
		/* 向FLASH写入密文*/
		FLASH_ProgramWord(0x08010000+i*4,(uint32_t)flag);
		
		flag = 1;
		
		p++;
	}

	/* 添加写保护 */
	FLASH_Lock(); 

}

void flash_rsa_decryption(int32_t d, int32_t n,uint32_t *buf,uint32_t len)
{
	int32_t i,j;
	uint32_t rd;
	int32_t flag = 1;
	uint32_t *p = buf;
	
#if 1	
	printf("\r\n(*) ciphertext:\r\n");
	
	for(i=0; i<len; i++)
	{
		/* 从FLASH读取密文 */
		rd=*(volatile uint32_t *)(0x08010000+i*4);
		
		if(i%8==0)
			printf("\r\n");
		
		printf("0x%08X ",rd);
		

	}
	
#endif	
	
	printf("\r\n(*) decryption:\r\n");	
	
	for(i=0; i<len; i++)
	{
		/* 从FLASH读取密文 */
		rd=*(volatile uint32_t *)(0x08010000+i*4);	
		
		for (j = 0; j < d; j++) 
		{
			flag = flag * rd % n;
			
		}
		
		*p = (uint32_t)flag;
		
		flag=1;
		
		p++;		
	}

}

int32_t main(void)
{
	uint32_t i;
	uint32_t buf[64];
	int32_t p=13,q=17,n;
	int32_t m,e,d;
	
	//串口1初始化波特率为115200bps
	usart1_init(115200);
	
	
	delay_ms(500);
	
	printf("This is flash test\r\n");
	
	n=p*q;
	
	m = (p - 1) * (q - 1);
	
	printf("n=%d\r\n",n);

	e = getrand(p, q);
	printf("Public Key e=%d\r\n", e);
	
	d= getNi(e, m);
	printf("Private Key d=%d\r\n",d);
	
	printf("1.Plaintext:1~64,encryption\r\n");
	
	for(i=0; i<64; i++)
		buf[i]=i+1;	
	
	/* 加密写入，密钥为0x2022*/
	flash_rsa_encryption(e,n,buf,64);
	
	printf("2.ciphertext decryption:\r\n");

	for(i=0; i<64; i++)
		buf[i]=0;	
	
	/* 解密读取，密钥为0x2022*/
	flash_rsa_decryption(d,n,buf,64);
	
	for(i=0; i<64; i++)
	{
		if(i%8==0)
			printf("\r\n");		
		
		printf("0x%08X ",buf[i]);
	}	
	
	while(1)
	{
		

	}

}

void USART1_IRQHandler(void)
{
	uint8_t d=0;
	
	//检测是否接收到数据
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		d=USART_ReceiveData(USART1);
		
		//清空标志位，可以响应新的中断请求
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}



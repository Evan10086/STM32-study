#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;
SPI_InitTypeDef  SPI_InitStructure;



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

void spi_flash_init(void)
{

	/* 打开SPI1硬件时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	/* 打开端口B的硬件时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	/* 将GPIO引脚配置为复用功能模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//复用功能模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;//第3 4 5号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	/* 将GPIO引脚连接到串口1 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);


	/* 配置GPIOB的14号引脚为输出功能模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;//第14号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	/* 看时序图，片选引脚初始电平为高电平 */
	PBout(14)=1;
	
	
	/* SPI的配置 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工通信
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//主机角色
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//数据大小为8bit
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //CPOL=1
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//CHPA=1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//片选引脚通过软件的代码控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//SPI波特率=84MHz/16=5.25MHz
	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//最高有效位优先传输
	//SPI_InitStructure.SPI_CRCPolynomial = 7;//如果有两个M4开发板使用SPI接口可以增加CRC的校验
	SPI_Init(SPI1, &SPI_InitStructure);
	
	/*!< Enable the sFLASH_SPI  */
	SPI_Cmd(SPI1, ENABLE);
	
	
}


uint8_t SPI1_SendByte(uint8_t byte)
{
	/*等待上一次数据发送完毕 */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	/* SPI1发送字节 */
	SPI_I2S_SendData(SPI1, byte);

	/*等待字节接收完毕 */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	/*返回接收到的数据 */
	return SPI_I2S_ReceiveData(SPI1);
}


void spi_flash_read_id(uint8_t *mid,uint8_t *did)
{
	/* 片选引脚为低电平，开始通信 */
	PBout(14)=0;
	
	/* 发送0x90指令，读取ID的指令 */
	SPI1_SendByte(0x90);
	
	/* 发送24bit的地址 */
	SPI1_SendByte(0x00);
	SPI1_SendByte(0x00);	
	SPI1_SendByte(0x00);

	/* 读取厂商ID，传递任意参数，一般为0xFF */
	*mid=SPI1_SendByte(0x00);


	/* 读取设备ID，传递任意参数，一般为0xFF */
	*did=SPI1_SendByte(0x00);
	
	/* 片选引脚为高电平，结束通信 */
	PBout(14)=1;
}


void spi_flash_read_data(uint32_t addr,uint8_t *buf,uint32_t len)
{
	uint8_t *p = buf;
	
	/* 片选引脚为低电平，开始通信 */
	PBout(14)=0;
	
	/* 发送0x03指令 */
	SPI1_SendByte(0x03);
	
	/* 发送24bit的地址,addr=0x123456 */
	SPI1_SendByte((addr>>16)&0xFF);//0x12
	SPI1_SendByte((addr>>8)&0xFF);//0x34	
	SPI1_SendByte(addr&0xFF);//0x56

	while(len--)
	{
		/* 读取数据，传递任意参数，一般为0xFF */
		*p=SPI1_SendByte(0xFF);

		p++;
	}

	/* 片选引脚为高电平，结束通信 */
	PBout(14)=1;
}

void spi_flash_write_disable(void)
{
	/* 片选引脚为低电平，开始通信 */
	PBout(14)=0;
	
	
	SPI1_SendByte(0x04);	
	
	/* 片选引脚为高电平，结束通信 */
	PBout(14)=1;
}

void spi_flash_write_enable(void)
{
	/* 片选引脚为低电平，开始通信 */
	PBout(14)=0;
	
	
	SPI1_SendByte(0x06);	
	
	/* 片选引脚为高电平，结束通信 */
	PBout(14)=1;
}

uint8_t spi_flash_read_status1(void)
{
	uint8_t sta;
	
	/* 片选引脚为低电平，开始通信 */
	PBout(14)=0;
	
	
	SPI1_SendByte(0x05);

	sta=SPI1_SendByte(0xFF);
	
	
	/* 片选引脚为高电平，结束通信 */
	PBout(14)=1;
	
	return sta;

}

void spi_flash_erase_sector(uint32_t sector_address)
{
	uint8_t sta;
	
	/* 解除写保护 */
	spi_flash_write_enable();
	
	delay_us(1);
	
	/* 片选引脚为低电平，开始通信 */
	PBout(14)=0;	
	
	/* 发送扇区擦除指令 */
	SPI1_SendByte(0x20);	

	/* 发送24bit的扇区地址 */
	SPI1_SendByte((sector_address>>16)&0xFF);
	SPI1_SendByte((sector_address>>8)&0xFF);
	SPI1_SendByte(sector_address&0xFF);
	
	/* 片选引脚为高电平，结束通信 */
	PBout(14)=1;

	delay_us(1);
		
	while(1)
	{
		/* 读取状态寄存器的BUSY位是否由1变为0，若变为0则擦除完毕 */
		sta = spi_flash_read_status1();	
		
		if((sta & 0x01)==0)
			break;
		
		delay_ms(100);
	
	}
	
	/* 添加写保护 */
	spi_flash_write_disable();
}



int main(void)
{

	uint32_t i;
	
	uint8_t buf[64];
	
	uint8_t mid,did;
	
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
	
	
	/* 串口1波特率为115200bps (注意:保证PLL配置正确，否则收发数据乱码)*/
	usart1_init(115200);
	
	spi_flash_init();
	
	printf("This is spi flash test\r\n");
	

	spi_flash_read_id(&mid,&did);
	
	printf("mid=%02X did=%02X\r\n",mid,did);
	
	

	
	spi_flash_read_data(0,buf,sizeof(buf));
	
	printf("spi flash read data from addr 0:");
	
	for(i=0; i<64; i++)
	{
		printf("%02X ",buf[i]);
	
	}
	
	printf("\r\n");	
	
	printf("spi flash erase sector 0\r\n");
	
	/* 擦除扇区0 */
	spi_flash_erase_sector(0);
	
	
	spi_flash_read_data(0,buf,sizeof(buf));
	
	printf("spi flash read data from addr 0:");
	
	for(i=0; i<64; i++)
	{
		printf("%02X ",buf[i]);
	
	}	
	printf("\r\n");
	
	while(1)
	{


		
	
	}
	
	//return 0;

}

void USART1_IRQHandler(void)
{
	uint8_t d;
	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
	
		d = USART_ReceiveData(USART1);
		

		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}



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
	
		SysTick->CTRL = 0; 	// �ر�ϵͳ��ʱ��
		SysTick->LOAD = 21000-1; //
		SysTick->VAL = 0; // ��յ�ǰ����ֵ�Ĵ����һ�����COUNTFLAG��־λ
		SysTick->CTRL = 1; // ʹ��ϵͳ��ʱ��������ʱ��ԴΪ�ο�ʱ��=�ں�ʱ�ӣ�168MHz��/8
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
		
	}
	
	SysTick->CTRL = 0; // �ر�ϵͳ��ʱ��

}

void delay_us(uint32_t n)
{

	SysTick->CTRL = 0; 	// �ر�ϵͳ��ʱ��
	SysTick->LOAD = 168*n-1; // 1us*n
	SysTick->VAL = 0; // ��յ�ǰ����ֵ�Ĵ����һ�����COUNTFLAG��־λ
	SysTick->CTRL = 5; // ʹ��ϵͳ��ʱ��������ʱ��ԴΪ�ں�ʱ�ӣ�168MHz��
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set

	SysTick->CTRL = 0; // �ر�ϵͳ��ʱ��

}



void usart1_init(uint32_t baud)
{

	/* �򿪴���1Ӳ��ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* �򿪶˿�A��Ӳ��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	
	/* ��GPIO��������Ϊ���ù���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	/* ��GPIO�������ӵ�����1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	/* ���ô���1��صĲ����������ʡ�����λ */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	/* ʹ�ܴ���1�ж���أ��������ݴ������ж����ȼ� */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	/* ʹ�ܴ���1���� */
	USART_Cmd(USART1, ENABLE);

}


void usart1_send_str(char *str)
{

	char *p = str;
	
	while(*p)
	{
		
		/* �����һ���Ƿ������ */
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);		
		
		USART_SendData(USART1,*p);
		
		p++;
		
	}

}
#define W25Q128_SCLK   PBout(3)
#define W25Q128_MOSI   PBout(5)
#define W25Q128_SS     PBout(14)
#define W25Q128_MISO   PBin(4)

void spi_flash_init(void)
{

	/* �򿪶˿�B��Ӳ��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	/* ��GPIO��������Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_14|GPIO_Pin_5;//��3 14 5������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	


	/* ����GPIOB��4������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;//��4������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	/* ��ʱ��ͼ��Ƭѡ���ų�ʼ��ƽΪ�ߵ�ƽ */
	W25Q128_SS=1;
	
	/* ��ʱ��ͼ��SCLK���ų�ʼ��ƽΪ�ߵ�ƽ��������ģʽ3 */	
	W25Q128_SCLK=1;
	
	/* ��ʱ��ͼ����MOSI���Ÿߵ�ƽ��͵�ƽ���� */
	W25Q128_MOSI=1;
	
}


uint8_t SPI1_SendByte(uint8_t byte)
{
	int32_t i;
	
	uint8_t d=0;
	
	for(i=7; i>=0; i--)
	{
		if(byte & (1<<i))
			W25Q128_MOSI=1;
		else
			W25Q128_MOSI=0;
	
		W25Q128_SCLK=0;	
		
		delay_us(1);
		
		W25Q128_SCLK=1;		

		delay_us(1);	

		if(W25Q128_MISO)
			d|=1<<i;
	}
	
	return d;
}


void spi_flash_read_id(uint8_t *mid,uint8_t *did)
{
	/* Ƭѡ����Ϊ�͵�ƽ����ʼͨ�� */
	PBout(14)=0;
	
	/* ����0x90ָ���ȡID��ָ�� */
	SPI1_SendByte(0x90);
	
	/* ����24bit�ĵ�ַ */
	SPI1_SendByte(0x00);
	SPI1_SendByte(0x00);	
	SPI1_SendByte(0x00);

	/* ��ȡ����ID���������������һ��Ϊ0xFF */
	*mid=SPI1_SendByte(0x00);


	/* ��ȡ�豸ID���������������һ��Ϊ0xFF */
	*did=SPI1_SendByte(0x00);
	
	/* Ƭѡ����Ϊ�ߵ�ƽ������ͨ�� */
	PBout(14)=1;
}


void spi_flash_read_data(uint32_t addr,uint8_t *buf,uint32_t len)
{
	uint8_t *p = buf;
	
	/* Ƭѡ����Ϊ�͵�ƽ����ʼͨ�� */
	PBout(14)=0;
	
	/* ����0x03ָ�� */
	SPI1_SendByte(0x03);
	
	/* ����24bit�ĵ�ַ,addr=0x123456 */
	SPI1_SendByte((addr>>16)&0xFF);//0x12
	SPI1_SendByte((addr>>8)&0xFF);//0x34	
	SPI1_SendByte(addr&0xFF);//0x56

	while(len--)
	{
		/* ��ȡ���ݣ��������������һ��Ϊ0xFF */
		*p=SPI1_SendByte(0xFF);

		p++;
	}

	/* Ƭѡ����Ϊ�ߵ�ƽ������ͨ�� */
	PBout(14)=1;
}

void spi_flash_write_disable(void)
{
	/* Ƭѡ����Ϊ�͵�ƽ����ʼͨ�� */
	PBout(14)=0;
	
	
	SPI1_SendByte(0x04);	
	
	/* Ƭѡ����Ϊ�ߵ�ƽ������ͨ�� */
	PBout(14)=1;
}

void spi_flash_write_enable(void)
{
	/* Ƭѡ����Ϊ�͵�ƽ����ʼͨ�� */
	PBout(14)=0;
	
	
	SPI1_SendByte(0x06);	
	
	/* Ƭѡ����Ϊ�ߵ�ƽ������ͨ�� */
	PBout(14)=1;
}

uint8_t spi_flash_read_status1(void)
{
	uint8_t sta;
	
	/* Ƭѡ����Ϊ�͵�ƽ����ʼͨ�� */
	PBout(14)=0;
	
	
	SPI1_SendByte(0x05);

	sta=SPI1_SendByte(0xFF);
	
	
	/* Ƭѡ����Ϊ�ߵ�ƽ������ͨ�� */
	PBout(14)=1;
	
	return sta;

}

void spi_flash_erase_sector(uint32_t sector_address)
{
	uint8_t sta;
	
	/* ���д���� */
	spi_flash_write_enable();
	
	delay_us(1);
	
	/* Ƭѡ����Ϊ�͵�ƽ����ʼͨ�� */
	PBout(14)=0;	
	
	/* ������������ָ�� */
	SPI1_SendByte(0x20);	

	/* ����24bit��������ַ */
	SPI1_SendByte((sector_address>>16)&0xFF);
	SPI1_SendByte((sector_address>>8)&0xFF);
	SPI1_SendByte(sector_address&0xFF);
	
	/* Ƭѡ����Ϊ�ߵ�ƽ������ͨ�� */
	PBout(14)=1;

	delay_us(1);
		
	while(1)
	{
		/* ��ȡ״̬�Ĵ�����BUSYλ�Ƿ���1��Ϊ0������Ϊ0�������� */
		sta = spi_flash_read_status1();	
		
		if((sta & 0x01)==0)
			break;
		
		delay_ms(100);
	
	}
	
	/* ���д���� */
	spi_flash_write_disable();
}



void spi_flash_write_page(uint32_t addr,uint8_t *buf,uint32_t len)
{
	uint8_t sta;
	uint8_t *p=buf;
	
	/* ���д���� */
	spi_flash_write_enable();
	
	delay_us(1);
	
	/* Ƭѡ����Ϊ�͵�ƽ����ʼͨ�� */
	PBout(14)=0;	
	
	/* ����ҳ���ָ�� */
	SPI1_SendByte(0x02);	

	/* ����24bit��Ҫд�����ݵ���ʼ��ַ */
	SPI1_SendByte((addr>>16)&0xFF);
	SPI1_SendByte((addr>>8)&0xFF);
	SPI1_SendByte(addr&0xFF);
	
	/* ����д������ */
	while(len--)
	{
		SPI1_SendByte(*p);
		
		p++;
	
	
	}
	
	/* Ƭѡ����Ϊ�ߵ�ƽ������ͨ�� */
	PBout(14)=1;

	delay_us(1);
		
	while(1)
	{
		/* ��ȡ״̬�Ĵ�����BUSYλ�Ƿ���1��Ϊ0������Ϊ0�������� */
		sta = spi_flash_read_status1();	
		
		if((sta & 0x01)==0)
			break;
		
		delay_ms(100);
	
	}
	
	/* ���д���� */
	spi_flash_write_disable();
}



int main(void)
{

	uint32_t i;
	
	uint8_t buf[64];
	
	uint8_t mid,did;
	
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* ����GPIOF��9������Ϊ���ù���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	/* ��PF9�������ӵ�TIM14 */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	
	PFout(9)=1;
	
	
	/* ����1������Ϊ115200bps (ע��:��֤PLL������ȷ�������շ���������)*/
	usart1_init(115200);
	
	spi_flash_init();
	
	printf("This is spi flash test\r\n");
	
	printf("1<<7=0x%02X\r\n",1<<7);
	printf("1<<6=0x%02X\r\n",1<<6);
	printf("1<<5=0x%02X\r\n",1<<5);
	printf("1<<4=0x%02X\r\n",1<<4);
	printf("1<<3=0x%02X\r\n",1<<3);
	printf("1<<2=0x%02X\r\n",1<<2);
	printf("1<<1=0x%02X\r\n",1<<1);
	printf("1<<0=0x%02X\r\n",1<<0);	

	spi_flash_read_id(&mid,&did);
	
	printf("mid=%02X did=%02X\r\n",mid,did);
	
	
	printf("spi flash erase sector 0\r\n");
	
	/* ��������0 */
	spi_flash_erase_sector(0);
	
	
	memset(buf,0x88,sizeof (buf));
	
	/* ��0��ַд��64�ֽ����� */
	spi_flash_write_page(0,buf,sizeof(buf));
	
	
	memset(buf,0,sizeof (buf));

	/* ��0��ַ��ȡ64�ֽ����� */	
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



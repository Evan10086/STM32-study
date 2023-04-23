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

void dht11_init(void)
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);


	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//��9������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��©�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	
	/* ����ʱ��ͼ��PG9���ų�ʼ״̬��Ҫ���Ϊ�ߵ�ƽ*/
	PGout(9)=1;
}

int32_t dht11_read(uint8_t *buf)
{
	uint32_t t=0;
	uint8_t *p=buf;
	
	uint8_t d=0;
	uint8_t check_sum=0;
	
	int32_t i,j;
	
	/* ����PG9����Ϊ��©���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//��9������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��©�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOG,&GPIO_InitStructure);
		
	
	PGout(9)=0;
	
	/* ��ʱ18ms */
	delay_ms(18);

	PGout(9)=1;
	
	/* ��ʱ30us */
	delay_us(30);
	
	
	/* ����PG9����Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//��9������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOG,&GPIO_InitStructure);	

	/* �ȴ�dht11�����Ӧ�źŵĵ͵�ƽ����ӳ�ʱ������ֹ������ */	
	t=0;
	while(PGin(9))
	{
		t++;
		
		delay_us(1);
		
		
		if(t>=4000)
			return -1;
	}
	
	/* �����͵�ƽ�ĳ���ʱ�䣬��ӳ�ʱ������ֹ������ */
	t=0;
	while(PGin(9)==0)
	{
		delay_us(1); //��ʱ1us
		
		t++;
		
		if(t>=100)
			return -2;
	}
	
	/* �����ߵ�ƽ�ĳ���ʱ�䣬��ӳ�ʱ������ֹ������ */
	t=0;
	while(PGin(9))
	{
		delay_us(1); //��ʱ1us
		
		t++;
		
		if(t>=100)
			return -3;
	}	
	
	
	/* ��������5���ֽڣ�ʪ������+ʪ��С��+�¶�����+�¶�С��+У��� */
	for(j=0; j<5; j++)
	{
		d=0;
		
		for(i=7; i>=0; i--)//MSB��bit7 -> bit0
		{
			
			/* �ȴ�50us�ĵ͵�ƽ������� */
			t=0;
			while(PGin(9)==0)
			{
				delay_us(1); //��ʱ1us
				
				t++;
				
				if(t>=100)
					return -4;
			}		

			/* ��ʱ40us */
			delay_us(40);
			
			/* �жϵ�ǰ���ŵĵ�ƽ�Ǹߵ�ƽ/�͵�ƽ */
			
			if(PGin(9))
			{
				/* ��d������Ӧ��bitλ��1 */
				d|=1<<i;
				
				/* ��ʣ�µĸߵ�ƽ�ȴ���� */
				t=0;
				while(PGin(9))
				{
					delay_us(1); //��ʱ1us
					
					t++;
					
					if(t>=100)
						return -5;
				}					
			
			}
		}
		
		p[j]=d;
	}
	
	delay_us(50);
	
	/* ������5���ֽں󣬾�У������ */
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(p[4]!=check_sum)
		return -6;

	
	return 0;
}





int main(void)
{
	uint8_t buf[5];
	
	int32_t rt;
	
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* ����GPIOF��9������Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	dht11_init();
	
	/* ����1������Ϊ115200bps (ע��:��֤PLL������ȷ�������շ���������)*/
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

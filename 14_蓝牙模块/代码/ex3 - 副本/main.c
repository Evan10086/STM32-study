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


void usart3_init(uint32_t baud)
{

	/* �򿪴���1Ӳ��ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	/* �򿪶˿�B��Ӳ��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	/* ��GPIO��������Ϊ���ù���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;//��10 11������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	/* ��GPIO�������ӵ�����3 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	/* ���ô���3��صĲ����������ʡ�����λ */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	/* ʹ�ܴ���3�ж���أ��������ݴ������ж����ȼ� */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	/* ʹ�ܴ���3���� */
	USART_Cmd(USART3, ENABLE);

}


void usart3_send_str(char *str)
{

	char *p = str;
	
	while(*p)
	{
		
		/* �����һ���Ƿ������ */
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);		
		
		USART_SendData(USART3,*p);
		
		p++;
		
	}

}

void tim14_init(void)
{
	/* ��TIM14��Ӳ��ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

	/* ����TIM14�����Ƶ�ʡ����� */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/100-1; //��ʱƵ�ʵȼ������Ƶ��
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; //Ԥ��Ƶֵ8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�ƵΪ1������F407��֧�֣����Բ�������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM�Ĺ���ģʽΪPWMģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�����������
	TIM_OCInitStructure.TIM_Pulse = 100;//�Ƚ�ֵ�����ڿ������������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//ͨ������Ч״̬Ϊ�ߵ�ƽ
	
	/* ����TIM14��ͨ��1 */
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	
	/* ʹ��TIM14���� */
	TIM_Cmd(TIM14, ENABLE);

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
	

	tim14_init();
	
	dht11_init();
	
	
	/* ����1������Ϊ115200bps (ע��:��֤PLL������ȷ�������շ���������)*/
	usart1_init(115200);
	
	
	/* ����3������Ϊ9600bps (ע��:��֤PLL������ȷ�������շ���������)*/
	usart3_init(9600);

	delay_ms(100);
	
	printf("This is ble ex3 test\r\n");
	
#if 0	
	/* ����AT����ָ�� */
	usart3_send_str("AT\r\n");
	
	delay_ms(100);
	
	/* ����ATָ���ѯӲ����ַ */
	usart3_send_str("AT+LADDR\r\n");
	
	delay_ms(100);
	
	/* ����ATָ���޸�ģ������ */
	usart3_send_str("AT+NAMETWen\r\n");	
	
	delay_ms(100);
	
	/* ����ATָ�λģ�� */
	usart3_send_str("AT+RESET\r\n");	

	delay_ms(100);
	
	/* ����ATָ���ѯģ������ */
	usart3_send_str("AT+NAME\r\n");

#endif
	
	while(1)
	{
		if(g_usart1_event)
		{
			/* ����ַ����������� */
#if 0			
			rt = sscanf((char *)g_usart1_buf,"duty=%d#",&duty);
			
			TIM_SetCompare1(TIM14,i);
				
			printf("duty set %d ok\r\n",i);
#else			
			//��duty=50#��
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

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

void sr04_init(void)
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//��6������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//��6������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	/* ����ʱ��ͼ��PB6���������ӵ�TRIG����ʼ״̬��Ҫ���Ϊ�͵�ƽ*/
	PBout(6)=0;
}

int32_t sr04_get_distance(void)
{
	uint32_t t=0;
	int32_t d;
	
	/* �����ź���������ߵ�ƽ */
	PBout(6)=1;
	
	/* ��ʱ10us */
	delay_us(10);

	/* �����ź���������͵�ƽ */	
	PBout(6)=0;	

	/* �ȴ������ź���������ߵ�ƽ����ӳ�ʱ������ֹ������ */	
	t=0;
	while(PEin(6)==0)
	{
		t++;
		
		delay_us(1);
		
		
		if(t>=100*1000)
			return -1;
	}
	
	/* �����ߵ�ƽ�ĳ���ʱ�䣬��ӳ�ʱ������ֹ������ */
	t=0;
	while(PEin(6))
	{
		delay_us(9); //��ʱ9us���ȼ�����������3mm
		
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
	
	

	
	
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	
	/* ����GPIOF��9������Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* ����GPIOF��9������Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	
	/* ����GPIOF��8������Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;//��8������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	
	
	
	
//	PFout(9)=1;
//	PFout(10)=1;
//	PEout(13)=1;
//	PEout(14)=1;
	
	
	
	sr04_init();
	
	/* ����1������Ϊ115200bps (ע��:��֤PLL������ȷ�������շ���������)*/
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



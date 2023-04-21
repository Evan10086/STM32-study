#include "stm32f4xx.h"

#define PAin(n)	     *((volatile uint32_t *)(0x42000000+(GPIOA_BASE+0x10-0x40000000)*32+n*4))
#define PEin(n)	     *((volatile uint32_t *)(0x42000000+(GPIOE_BASE+0x10-0x40000000)*32+n*4))	
#define PEout(n)	 *((volatile uint32_t *)(0x42000000+(GPIOE_BASE+0x14-0x40000000)*32+n*4))
#define PFout(n)	 *((volatile uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))


static GPIO_InitTypeDef GPIO_InitStructure;
static EXTI_InitTypeDef EXTI_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;

static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;


void tim3_init(void)
{
	//ʹ��tim3��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	//����tim3�ķ�Ƶֵ������ֵ
	//tim3Ӳ��ʱ��=84MHz/8400=10000Hz�����ǽ���10000�μ���������1��ʱ��ĵ���

	TIM_TimeBaseStructure.TIM_Period = 10000/20-1;	//����ֵ500,0 -> 499����50����ʱ��ĵ���
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//Ԥ��Ƶֵ8400
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//ʱ�ӷ�Ƶ����ǰ��û�еģ�����Ҫ��������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	//����tim3���ж�
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void exti_init(void)
{
	//�򿪶˿�A��Ӳ��ʱ�ӣ����ǶԶ˿�A����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//�򿪶˿�Ӳ��ʱ�ӣ����ǶԵ�ǰӲ������
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//�˿ڵĵ�2������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;//����ٶ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ���ڲ�����������
	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	
	
	
	
	//PA0���ӵ�EXTI0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
		
	
	//�����ⲿ�ж�0
	EXTI_InitStructure.EXTI_Line = EXTI_Line0|EXTI_Line2|EXTI_Line3|EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�жϴ���
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //��ⰴ���İ���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��
	EXTI_Init(&EXTI_InitStructure);
	
	//NVIC����EXTI0���������ж�����������������ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;		//EXTI0���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ���0x0F
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;//��Ӧ���ӣ����ȼ���0x0F
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//����������ж����󣬴򿪸�ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;	//ָ����EXTI2���ж�����ͨ������д�����жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ� 0x1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//��Ӧ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ��EXTI2�ж�����ͨ��
	NVIC_Init(&NVIC_InitStructure);		
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;	//ָ����EXTI3���ж�����ͨ������д�����жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//��ռ���ȼ� 0x1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//��Ӧ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ��EXTI3�ж�����ͨ��
	NVIC_Init(&NVIC_InitStructure);		
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;	//ָ����EXTI4���ж�����ͨ������д�����жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x04;//��ռ���ȼ� 0x1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//��Ӧ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ��EXTI4�ж�����ͨ��
	NVIC_Init(&NVIC_InitStructure);		
	
	
}

int main(void)
{
	//�򿪶˿�Ӳ��ʱ�ӣ����ǶԵ�ǰӲ������
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	
	exti_init();
	tim3_init();	
	
	
	//���ö˿�ָ�����ŵĹ���
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//�˿ڵĵ�9 10������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;//�������ģʽ��ʹ���ڲ���P-mos&N-mos��
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;//����ٶ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ���ڲ�����������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	//���ö˿�ָ�����ŵĹ���
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//�˿ڵĵ�9 10������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;//�������ģʽ��ʹ���ڲ���P-mos&N-mos��
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;//����ٶ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ���ڲ�����������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	PFout(9)=1;
	PFout(10)=1;
	PEout(13)=1;
	PEout(14)=1;
	
	
	


	
	while(1)
	{

		
		
	}
}


void EXTI0_IRQHandler(void)
{
	//���EXTI0�Ƿ����ж�����
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		TIM_Cmd(TIM3, ENABLE);
		
		//��ձ�־λ������CPU����ǰ�ж������Ѿ�������ϣ�˼���⣺�������ձ�־λ�������ʲô����
		EXTI_ClearITPendingBit(EXTI_Line0);
	
	}
}


void EXTI2_IRQHandler(void)
{
	//���EXTI0�Ƿ����ж�����
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		TIM_Cmd(TIM3, ENABLE);
		
		//��ձ�־λ������CPU����ǰ�ж������Ѿ�������ϣ�˼���⣺�������ձ�־λ�������ʲô����
		EXTI_ClearITPendingBit(EXTI_Line2);
	
	}
}

void EXTI3_IRQHandler(void)
{
	//���EXTI3�Ƿ����ж�����
	if(EXTI_GetITStatus(EXTI_Line3) == SET)
	{
		TIM_Cmd(TIM3, ENABLE);
		
		//��ձ�־λ������CPU����ǰ�ж������Ѿ�������ϣ�˼���⣺�������ձ�־λ�������ʲô����
		EXTI_ClearITPendingBit(EXTI_Line3);
	
	}
}

void EXTI4_IRQHandler(void)
{
	//���EXTI0�Ƿ����ж�����
	if(EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		TIM_Cmd(TIM3, ENABLE);
		
		//��ձ�־λ������CPU����ǰ�ж������Ѿ�������ϣ�˼���⣺�������ձ�־λ�������ʲô����
		EXTI_ClearITPendingBit(EXTI_Line4);
	
	}
}

void TIM3_IRQHandler(void)
{

	//����־λ
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		if(PAin(0)==0)
			PFout(9)^=1;
		
		if(PEin(2)==0)
			PFout(10)^=1;	
		if(PEin(3)==0)
			PEout(13)^=1;
		if(PEin(4)==0)
			PEout(14)^=1;
		
		
		TIM_Cmd(TIM3, DISABLE);
		//��ձ�־λ
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}



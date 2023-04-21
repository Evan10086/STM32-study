#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

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

#define PFout(n) (*(uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))
#define PEout(n) (*(uint32_t *)(0x42000000+(GPIOE_BASE+0x14-0x40000000)*32+n*4))


void tim2_init(void)
{
	/* ��TIM2��Ӳ��ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* ����TIM2����ʱʱ�䡢�ж� */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/5-1; //������ʱʱ��
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; //Ԥ��Ƶֵ8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�ƵΪ1������F407��֧�֣����Բ�������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* ʹ��ʱ������ж� */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* ʹ��TIM2���� */
	TIM_Cmd(TIM2, ENABLE);

}

void tim3_init(void)
{
	/* ��TIM3��Ӳ��ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* ����TIM3����ʱʱ�䡢�ж� */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/2-1; //������ʱʱ��
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; //Ԥ��Ƶֵ8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�ƵΪ1������F407��֧�֣����Բ�������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* ʹ��ʱ������ж� */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* ʹ��TIM3���� */
	TIM_Cmd(TIM3, ENABLE);

}

void tim8_init(void)
{
	/* ��TIM8��Ӳ��ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	/* ����TIM8����ʱʱ�䡢�ж� */
	
	TIM_TimeBaseStructure.TIM_Period = 10000*4-1; //������ʱʱ��
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1; //Ԥ��Ƶֵ8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�ƵΪ1������F407��֧�֣����Բ�������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	/* ʹ��ʱ������ж� */
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* ʹ��TIM8���� */
	TIM_Cmd(TIM8, ENABLE);

}

void tim1_init(void)
{
	/* ��TIM1��Ӳ��ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* ����TIM1����ʱʱ�䡢�ж� */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/10-1; //������ʱʱ��
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1; //Ԥ��Ƶֵ8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�ƵΪ1������F407��֧�֣����Բ�������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	/* ʹ��ʱ������ж� */
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* ʹ��TIM1���� */
	TIM_Cmd(TIM1, ENABLE);

}



int main(void)
{
	//uint32_t *pf9 = (uint32_t *)0x424282A4;
	
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	/* ����GPIOF��9������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* ����GPIOF��9������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//��13 14������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	tim1_init();
	tim3_init();
	tim2_init();
	tim8_init();
	
	

	
	while(1)
	{

	}
	
	//return 0;

}

void TIM2_IRQHandler(void)
{
	/* �жϱ�־λ */
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{
		PFout(10)^=1;
		
		/* ��ձ�־λ */
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
	}

}

void TIM3_IRQHandler(void)
{
	/* �жϱ�־λ */
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		PEout(13)^=1;
		
		/* ��ձ�־λ */
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
	}

}

void TIM8_UP_TIM13_IRQHandler(void)
{
	/* �жϱ�־λ */
	if(TIM_GetITStatus(TIM8,TIM_IT_Update) == SET)
	{
		PEout(14)^=1;
		
		/* ��ձ�־λ */
		TIM_ClearITPendingBit(TIM8,TIM_IT_Update);
	
	}

}

void TIM1_UP_TIM10_IRQHandler(void)
{
	/* �жϱ�־λ */
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		PFout(9)^=1;
		
		/* ��ձ�־λ */
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	
	}

}




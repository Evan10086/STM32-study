#include "stm32f4xx.h"

void pwm_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  		 TIM_OCInitStructure;	
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
	
	//ʹ��TIM14��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	//�򿪶˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	
	//����GPIOF�ĵ�9��
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	//��ʼ��
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//PF9���ӵ�TIM14
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);	
	
	//����TIM14�ļ���ֵ��������������Ƶ��
	TIM_TimeBaseStructure.TIM_Period = 10000/10-1;	//������������ɼ���ֵ�������������Ƶ��Ϊ10Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;	//Ԥ��Ƶֵ=8400-1+1=8400��Ƶ����һ��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//ʱ�ӷ�Ƶ��Ҳ��֮Ϊ�ڶ��η�Ƶ������F407��֧��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ�������0 -> TIM_Period
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	/* ���ͨ��1������PWM1ģʽ */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			 //ֻҪ TIMx_CNT������ֵ�� < TIMx_CCR1���Ƚ�ֵ����ͨ�� 1 ��Ϊ��Ч״̬������Ϊ��Ч״̬��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�����������
	TIM_OCInitStructure.TIM_Pulse = 10000/10;	//�Ƚ�ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //ͨ������Ч״̬Ϊ�ߵ�ƽ
	
	//TIM14��ͨ��1����
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	

	//ʹ��TIM14����
	TIM_Cmd(TIM14, ENABLE);
}

void pwm_led(uint32_t freq,uint32_t duty)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//����TIM14�ļ���ֵ��������������Ƶ��
	TIM_TimeBaseStructure.TIM_Period = 10000/freq-1;	//������������ɼ���ֵ�������������Ƶ��Ϊ10Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;	//Ԥ��Ƶֵ=8400-1+1=8400��Ƶ����һ��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//ʱ�ӷ�Ƶ��Ҳ��֮Ϊ�ڶ��η�Ƶ������F407��֧��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ�������0 -> TIM_Period
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	
	//����TIM14ͨ��1�ıȽ�ֵ
	TIM_SetCompare1(TIM14,(TIM_TimeBaseStructure.TIM_Period+1)*duty/100);

}

#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

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

void tim1_init(void)
{
	/* ��TIM1��Ӳ��ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* ����TIM4�����Ƶ�ʡ����� */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/100-1; //��ʱƵ�ʵȼ������Ƶ��
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1; //Ԥ��Ƶֵ8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�ƵΪ1������F407��֧�֣����Բ�������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM�Ĺ���ģʽΪPWMģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�����������
	TIM_OCInitStructure.TIM_Pulse = 0;//�Ƚ�ֵ�����ڿ������������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//ͨ������Ч״̬Ϊ�ߵ�ƽ
	
	/* ����TIM1��ͨ��3 4*/
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	
	/* ʹ��TIM1���� */
	TIM_Cmd(TIM1, ENABLE);
	
	TIM_CtrlPWMOutputs(TIM1 , ENABLE);

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
	TIM_OCInitStructure.TIM_Pulse = 0;//�Ƚ�ֵ�����ڿ������������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//ͨ������Ч״̬Ϊ�ߵ�ƽ
	
	/* ����TIM14��ͨ��1 */
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	
	/* ʹ��TIM14���� */
	TIM_Cmd(TIM14, ENABLE);

}



int main(void)
{
	uint32_t comp = 0;
	
	/* ʹ�ܶ˿�E��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	/* ʹ�ܶ˿�E��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* ����GPIOF��9������Ϊ���ù���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//��9������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* ����GPIOF��13��14������Ϊ���ù���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//��13 14������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	

	
	
	/* ��PE13��14�������ӵ�TIM1 */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);
	
	/* ��PF9�������ӵ�TIM1 */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	
	
	
	tim1_init();
	tim14_init();
	
	
	while(1)
	{
		//1s���������
		while(comp<100){
			TIM_SetCompare1(TIM14,comp);
			TIM_SetCompare3(TIM1,comp);
			TIM_SetCompare4(TIM1,comp);
			comp++;
			delay_ms(50);
		}
		
		//1s���������
		while(comp>0){
			TIM_SetCompare1(TIM14,comp);
			TIM_SetCompare3(TIM1,comp);
			TIM_SetCompare4(TIM1,comp);
			comp--;
			delay_ms(50);
		}
		
		delay_ms(200);

	}
	

}



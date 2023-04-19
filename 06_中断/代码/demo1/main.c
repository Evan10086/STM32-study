#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void delay(void)
{
	uint32_t i=0x2000000;//unsigned int
	
	while(i--);


}

#define PAin(n)  (*(volatile uint32_t *)(0x42000000+(GPIOA_BASE+0x10-0x40000000)*32+n*4))
#define PFout(n) (*(volatile uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))


int main(void)
{
	//uint32_t *pf9 = (uint32_t *)0x424282A4;
	
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* ʹ��ϵͳ����Ӳ��ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	
	/* ����GPIOF��9������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//��9������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* ����GPIOA��0������Ϊ����ʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//��0������
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	
	/* ��PA0���ӵ�EXTI0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	
	/* ��ʼ��EXTI0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//ָ��EXTI0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش��������ڼ�ⰴ���İ��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��EXTI0
	EXTI_Init(&EXTI_InitStructure);
	
	/* ����NVIC�е�EXTI0�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//EXTI0���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;//��Ӧ���ӣ����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//��EXTI0������ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
	
	while(1)
	{
		PFout(9)=PAin(0);
	
	}
	
	//return 0;

}

void EXTI0_IRQHandler(void)
{
	/* ����־λ */
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{

		/* ����û����� */
		PFout(9)=0;
		
		delay();
		
		PFout(9)=1;
		
		delay();		

		/* ��ձ�־λ������CPU��ǰ�ж���������ϣ�������Ӧ��EXTI0���ж�����
		˼���⣺������ձ�־λ�������ʲô����
		*/
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}


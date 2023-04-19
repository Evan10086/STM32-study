#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void delay(void)
{
	uint32_t i=0xA000000;//unsigned int
	
	while(i--);


}

#define PAin(n)  (*(volatile uint32_t *)(0x42000000+(GPIOA_BASE+0x10-0x40000000)*32+n*4))
#define PEin(n)  (*(volatile uint32_t *)(0x42000000+(GPIOE_BASE+0x10-0x40000000)*32+n*4))
	
#define PEout(n) (*(volatile uint32_t *)(0x42000000+(GPIOE_BASE+0x14-0x40000000)*32+n*4))
#define PFout(n) (*(volatile uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))


int main(void)
{
	//uint32_t *pf9 = (uint32_t *)0x424282A4;
	
	/* ֧��4����ռ���ȼ���4����Ӧ���ȼ� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* ʹ��ϵͳ����Ӳ��ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	
	/* ����GPIOF��9 10������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//��13 14������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	
	/* ����GPIOA��0������Ϊ����ʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//��0������
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	/* ����GPIOE��2 3 ������Ϊ����ʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//����ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;//��2 3������
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);		
	
	
	/* ��PA0���ӵ�EXTI0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	
	/* ��PE2���ӵ�EXTI2 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource2);	
	
	/* ��PE3���ӵ�EXTI3 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);		
	
	/* ��ʼ��EXTI0 EXTI2 EXTI3*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line0|EXTI_Line2|EXTI_Line3;	//ָ��EXTI0 EXTI2 EXTI3
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش��������ڼ�ⰴ���İ��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��EXTI0
	EXTI_Init(&EXTI_InitStructure);
	
	/* ����NVIC�е�EXTI0�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//EXTI0���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//��Ӧ���ӣ����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//��EXTI0������ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
	/* ����NVIC�е�EXTI2�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//EXTI2���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//��Ӧ���ӣ����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//��EXTI0������ͨ��
	NVIC_Init(&NVIC_InitStructure);	
	
	/* ����NVIC�е�EXTI3�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//EXTI3���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//��Ӧ���ӣ����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//��EXTI0������ͨ��
	NVIC_Init(&NVIC_InitStructure);		
	
	PFout(9)=1;
	PFout(10)=1;
	PEout(13)=1;
	PEout(14)=1;	
	while(1)
	{

	
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


void EXTI2_IRQHandler(void)
{
	/* ����־λ */
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{

		/* ����û����� */
		PFout(10)=0;
		
		delay();
		
		PFout(10)=1;
		
		delay();		

		/* ��ձ�־λ������CPU��ǰ�ж���������ϣ�������Ӧ��EXTI0���ж�����
		˼���⣺������ձ�־λ�������ʲô����
		*/
		EXTI_ClearITPendingBit(EXTI_Line2);
	}	
}


void EXTI3_IRQHandler(void)
{
	/* ����־λ */
	if(EXTI_GetITStatus(EXTI_Line3) == SET)
	{

		/* ����û����� */
		PEout(13)=0;
		
		delay();
		
		PEout(13)=1;
		
		delay();		

		/* ��ձ�־λ������CPU��ǰ�ж���������ϣ�������Ӧ��EXTI0���ж�����
		˼���⣺������ձ�־λ�������ʲô����
		*/
		EXTI_ClearITPendingBit(EXTI_Line3);
	}	
}

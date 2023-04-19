#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	uint32_t i=0x200000;//unsigned int
	
	while(i--);


}


int main(void)
{

	/* ʹ�ܶ˿�E F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�E F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* ����GPIOF��9 10������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* ����GPIOE��13 14������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//��13 14������
	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	
	
	while(1)
	{
		GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10);
		GPIO_SetBits(GPIOE,GPIO_Pin_13|GPIO_Pin_14);
		
		delay();
		
		
		/* ������ */
		GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
		
		/* ��ʱһ��*/
		delay();
		

		/* ������ */
		GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_RESET);		
		
		/* ��ʱһ��*/		
		delay();
		
		/* ������ */
		GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_RESET);		
		
		/* ��ʱһ��*/		
		delay();		
		
		/* ������ */
		GPIO_WriteBit(GPIOE,GPIO_Pin_14,Bit_RESET);		
		
		/* ��ʱһ��*/		
		delay();		
	}
	
	//return 0;

}

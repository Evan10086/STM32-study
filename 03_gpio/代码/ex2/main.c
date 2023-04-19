#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	uint32_t i=0x200000;//unsigned int
	
	while(i--);


}


int main(void)
{

	/* ʹ�ܶ˿�A E F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�A E F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOA|
							RCC_AHB1Periph_GPIOE|
							RCC_AHB1Periph_GPIOF,ENABLE);
	
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
	
	
	/* ����GPIOA��0������Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//��0������
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/* ����GPIOE��2 3 4 ������Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//��2 3 4������
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);


	GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10);
	GPIO_SetBits(GPIOE,GPIO_Pin_13|GPIO_Pin_14);
	
	while(1)
	{

		
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==Bit_RESET)
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
		else
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET);	
		
		
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==Bit_RESET)
			GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_RESET);
		else
			GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_SET);
		
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)==Bit_RESET)
			GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_RESET);
		else
			GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_SET);	
		
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==Bit_RESET)
			GPIO_WriteBit(GPIOE,GPIO_Pin_14,Bit_RESET);
		else
			GPIO_WriteBit(GPIOE,GPIO_Pin_14,Bit_SET);			
		
	}
	
	//return 0;

}

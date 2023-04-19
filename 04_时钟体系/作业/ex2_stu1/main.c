#include "stm32f4xx.h"


GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	uint32_t i=0x200000;//unsigned int
	
	while(i--);

}


int main(void)
{
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	/* ����GPIOF��9��10������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//��9������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* ����GPIOF��13��14������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//��13��14������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	/* ����GPIOA��0������Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//��9������
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/* ����GPIOA��2��3��4������Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//��9������
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10);		//�ߵ�ƽϨ�𣬵͵�ƽ���
	GPIO_SetBits(GPIOE,GPIO_Pin_13|GPIO_Pin_14);
	
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == Bit_RESET)
		{
			//PLL
			RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
			RCC->CFGR |= RCC_CFGR_SW_PLL;
		
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
			
			delay();
			
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET);
			
			delay();
		}else if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == Bit_RESET)
		{
			//HSI
			RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
			RCC->CFGR |= RCC_CFGR_SW_HSI;
			
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
			
			delay();
			
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET);
			
			delay();
		}else if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == Bit_RESET)
		{
			//HSE
			RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
			RCC->CFGR |= RCC_CFGR_SW_HSE;
			
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
			
			delay();
			
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET);
			
			delay();
		}
		
		/*
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == Bit_RESET)
			
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
		else
			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET);
		
		
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == Bit_RESET)
			
			GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_RESET);
		else
			GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_SET);

		
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == Bit_RESET)
			
			GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_RESET);
		else
			GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_SET);
		
		
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == Bit_RESET)
			
			GPIO_WriteBit(GPIOE,GPIO_Pin_14,Bit_RESET);
		else
			GPIO_WriteBit(GPIOE,GPIO_Pin_14,Bit_SET);
		*/
	}
	
	//return 0;

}

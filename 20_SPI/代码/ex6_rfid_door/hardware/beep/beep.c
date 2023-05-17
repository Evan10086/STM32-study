#include "stm32f4xx.h"


void beep_init(void)
{    	 
	GPIO_InitTypeDef  	GPIO_InitStructure;	 
	
	//ʹ��GPIOFʱ��
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);			

	//GPIOF8��ʼ������ 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;		//BEEP��ӦIO��
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    	//��ͨ���ģʽ��
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//�������������LED��Ҫ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //����
	GPIO_Init(GPIOF, &GPIO_InitStructure);							//��ʼ��GPIOF�������õ�����д��Ĵ���		

	GPIO_WriteBit(GPIOF,GPIO_Pin_8,Bit_RESET);	
}

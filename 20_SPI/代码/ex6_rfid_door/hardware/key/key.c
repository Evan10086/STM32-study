#include "stm32f4xx.h"


void key_init(void)
{    	 
	GPIO_InitTypeDef  	GPIO_InitStructure;	 
	
	//ʹ��GPIOE��GPIOFʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE, ENABLE);			

	//GPIOA0��ʼ������ 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;			    				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    
	GPIO_Init(GPIOA, &GPIO_InitStructure);											


	//GPIOE2~GPIOE4��ʼ������ 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3|GPIO_Pin_4;		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;					//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;					//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);							//��ʼ��GPIOE�������õ�����д��Ĵ���	
}

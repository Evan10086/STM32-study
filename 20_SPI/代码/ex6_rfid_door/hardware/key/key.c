#include "stm32f4xx.h"


void key_init(void)
{    	 
	GPIO_InitTypeDef  	GPIO_InitStructure;	 
	
	//使能GPIOE，GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE, ENABLE);			

	//GPIOA0初始化设置 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;			    				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    
	GPIO_Init(GPIOA, &GPIO_InitStructure);											


	//GPIOE2~GPIOE4初始化设置 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3|GPIO_Pin_4;		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;					//普通输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;					//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);							//初始化GPIOE，把配置的数据写入寄存器	
}

#include "stm32f4xx.h"


GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	uint32_t i=0x200000;//unsigned int
	
	while(i--);

}


int main(void)
{
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	/* 配置GPIOF的9、10号引脚为推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//第9号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* 配置GPIOF的13、14号引脚为推挽输出模式 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//第13、14号引脚
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	/* 配置GPIOA的0号引脚为输入模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//第9号引脚
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/* 配置GPIOA的2、3、4号引脚为输入模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//第9号引脚
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10);		//高电平熄灭，低电平点灯
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

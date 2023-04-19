#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	uint32_t i=0x200000;//unsigned int
	
	while(i--);


}


int main(void)
{

	/* 使能端口E F的硬件时钟，说白就是端口E F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* 配置GPIOF的9 10号引脚为推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* 配置GPIOE的13 14号引脚为推挽输出模式 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//第13 14号引脚
	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	
	
	while(1)
	{
		GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10);
		GPIO_SetBits(GPIOE,GPIO_Pin_13|GPIO_Pin_14);
		
		delay();
		
		
		/* 点亮灯 */
		GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
		
		/* 延时一会*/
		delay();
		

		/* 点亮灯 */
		GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_RESET);		
		
		/* 延时一会*/		
		delay();
		
		/* 点亮灯 */
		GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_RESET);		
		
		/* 延时一会*/		
		delay();		
		
		/* 点亮灯 */
		GPIO_WriteBit(GPIOE,GPIO_Pin_14,Bit_RESET);		
		
		/* 延时一会*/		
		delay();		
	}
	
	//return 0;

}

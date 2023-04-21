#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;

void delay_ms(uint32_t n)
{
	while(n--)
	{
	
		SysTick->CTRL = 0; 	// 关闭系统定时器
		SysTick->LOAD = 168000-1; // Count from 167999 to 0 (168000 cycles)
		SysTick->VAL = 0; // 清空当前计数值寄存器且会清零COUNTFLAG标志位
		SysTick->CTRL = 5; // 使能系统定时器，它的时钟源为内核时钟（168MHz）
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
		
	}
	
	SysTick->CTRL = 0; // 关闭系统定时器

}

void delay_us(uint32_t n)
{
   
        SysTick->CTRL = 0;                      // 关闭系统定时器
        SysTick->LOAD = 168000/1000-1;          // Count from 167 to 0 (168 cycles)
        SysTick->VAL = 0;                       // 清空当前计数值寄存器且会清零COUNTFLAG标志位
        SysTick->CTRL = 5;                      // 使能系统定时器，它的时钟源为内核时钟（168MHz）
        while ((SysTick->CTRL & 0x10000)==0);   // 等待count flag被置位
    
}


#define PFout(n) (*(uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))




int main(void)
{
	//uint32_t *pf9 = (uint32_t *)0x424282A4;
	
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* 配置GPIOF的9号引脚为推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	

	
	while(1)
	{
		PFout(9)^=1;
		delay_us(100000);
	}
	
	//return 0;

}


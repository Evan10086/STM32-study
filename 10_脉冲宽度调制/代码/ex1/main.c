#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

void delay_ms(uint32_t n)
{
	while(n--)
	{
	
		SysTick->CTRL = 0; 	// 关闭系统定时器
		SysTick->LOAD = 21000-1; //
		SysTick->VAL = 0; // 清空当前计数值寄存器且会清零COUNTFLAG标志位
		SysTick->CTRL = 1; // 使能系统定时器，它的时钟源为参考时钟=内核时钟（168MHz）/8
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
		
	}
	
	SysTick->CTRL = 0; // 关闭系统定时器

}

void delay_us(uint32_t n)
{

	SysTick->CTRL = 0; 	// 关闭系统定时器
	SysTick->LOAD = 168*n-1; // 1us*n
	SysTick->VAL = 0; // 清空当前计数值寄存器且会清零COUNTFLAG标志位
	SysTick->CTRL = 5; // 使能系统定时器，它的时钟源为内核时钟（168MHz）
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set

	SysTick->CTRL = 0; // 关闭系统定时器

}

#define PFout(n) (*(uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))

void tim14_init(void)
{
	/* 打开TIM14的硬件时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

	/* 配置TIM14：输出频率、脉宽 */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/100-1; //定时频率等价于输出频率
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; //预分频值8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频为1，但是F407不支持，可以不用配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM的工作模式为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//允许输出脉冲
	TIM_OCInitStructure.TIM_Pulse = 100;//比较值，用于控制输出的脉宽
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//通道的有效状态为高电平
	
	/* 配置TIM14的通道1 */
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	
	/* 使能TIM14工作 */
	TIM_Cmd(TIM14, ENABLE);

}




int main(void)
{
	int32_t i;
	
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* 配置GPIOF的9号引脚为复用功能模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//复用功能模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	/* 将PF9引脚连接到TIM14 */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	
	tim14_init();

	
	while(1)
	{
		/* 渐灭 */
		for(i=0; i<=100; i++)
		{
		
			TIM_SetCompare1(TIM14,i);
			delay_ms(20);
		
		}
		
		/* 渐亮 */
		for(i=100; i>=0; i--)
		{
		
			TIM_SetCompare1(TIM14,i);
			delay_ms(20);
		
		}		
	}
	
	//return 0;

}



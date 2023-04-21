#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

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
#define PEout(n) (*(uint32_t *)(0x42000000+(GPIOE_BASE+0x14-0x40000000)*32+n*4))


void tim2_init(void)
{
	/* 打开TIM2的硬件时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* 配置TIM2：定时时间、中断 */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/5-1; //决定定时时间
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; //预分频值8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频为1，但是F407不支持，可以不用配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* 使能时间更新中断 */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 使能TIM2工作 */
	TIM_Cmd(TIM2, ENABLE);

}

void tim3_init(void)
{
	/* 打开TIM3的硬件时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* 配置TIM3：定时时间、中断 */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/2-1; //决定定时时间
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; //预分频值8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频为1，但是F407不支持，可以不用配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* 使能时间更新中断 */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 使能TIM3工作 */
	TIM_Cmd(TIM3, ENABLE);

}

void tim8_init(void)
{
	/* 打开TIM8的硬件时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	/* 配置TIM8：定时时间、中断 */
	
	TIM_TimeBaseStructure.TIM_Period = 10000*4-1; //决定定时时间
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1; //预分频值8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频为1，但是F407不支持，可以不用配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	/* 使能时间更新中断 */
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 使能TIM8工作 */
	TIM_Cmd(TIM8, ENABLE);

}

void tim1_init(void)
{
	/* 打开TIM1的硬件时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* 配置TIM1：定时时间、中断 */
	
	TIM_TimeBaseStructure.TIM_Period = 10000/10-1; //决定定时时间
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1; //预分频值8400
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频为1，但是F407不支持，可以不用配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	/* 使能时间更新中断 */
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 使能TIM1工作 */
	TIM_Cmd(TIM1, ENABLE);

}



int main(void)
{
	//uint32_t *pf9 = (uint32_t *)0x424282A4;
	
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	/* 配置GPIOF的9号引脚为推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* 配置GPIOF的9号引脚为推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//第13 14号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	tim1_init();
	tim3_init();
	tim2_init();
	tim8_init();
	
	

	
	while(1)
	{

	}
	
	//return 0;

}

void TIM2_IRQHandler(void)
{
	/* 判断标志位 */
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{
		PFout(10)^=1;
		
		/* 清空标志位 */
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
	}

}

void TIM3_IRQHandler(void)
{
	/* 判断标志位 */
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		PEout(13)^=1;
		
		/* 清空标志位 */
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
	}

}

void TIM8_UP_TIM13_IRQHandler(void)
{
	/* 判断标志位 */
	if(TIM_GetITStatus(TIM8,TIM_IT_Update) == SET)
	{
		PEout(14)^=1;
		
		/* 清空标志位 */
		TIM_ClearITPendingBit(TIM8,TIM_IT_Update);
	
	}

}

void TIM1_UP_TIM10_IRQHandler(void)
{
	/* 判断标志位 */
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		PFout(9)^=1;
		
		/* 清空标志位 */
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	
	}

}




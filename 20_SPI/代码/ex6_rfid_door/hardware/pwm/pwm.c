#include "stm32f4xx.h"

void pwm_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  		 TIM_OCInitStructure;	
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
	
	//使能TIM14的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	//打开端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	
	//配置GPIOF的第9根
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	//初始化
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//PF9连接到TIM14
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);	
	
	//配置TIM14的计数值，决定脉冲的输出频率
	TIM_TimeBaseStructure.TIM_Period = 10000/10-1;	//输出脉冲周期由计数值决定，输出脉冲频率为10Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;	//预分频值=8400-1+1=8400分频，第一分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//时钟分频，也称之为第二次分频，但是F407不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数，从0 -> TIM_Period
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	/* 输出通道1工作在PWM1模式 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			 //只要 TIMx_CNT（计数值） < TIMx_CCR1（比较值），通道 1 便为有效状态，否则为无效状态。
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//允许输出脉冲
	TIM_OCInitStructure.TIM_Pulse = 10000/10;	//比较值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //通道的有效状态为高电平
	
	//TIM14的通道1配置
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	

	//使能TIM14工作
	TIM_Cmd(TIM14, ENABLE);
}

void pwm_led(uint32_t freq,uint32_t duty)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//配置TIM14的计数值，决定脉冲的输出频率
	TIM_TimeBaseStructure.TIM_Period = 10000/freq-1;	//输出脉冲周期由计数值决定，输出脉冲频率为10Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;	//预分频值=8400-1+1=8400分频，第一分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//时钟分频，也称之为第二次分频，但是F407不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数，从0 -> TIM_Period
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	
	//设置TIM14通道1的比较值
	TIM_SetCompare1(TIM14,(TIM_TimeBaseStructure.TIM_Period+1)*duty/100);

}

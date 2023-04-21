#include "stm32f4xx.h"

#define PAin(n)	     *((volatile uint32_t *)(0x42000000+(GPIOA_BASE+0x10-0x40000000)*32+n*4))
#define PEin(n)	     *((volatile uint32_t *)(0x42000000+(GPIOE_BASE+0x10-0x40000000)*32+n*4))	
#define PEout(n)	 *((volatile uint32_t *)(0x42000000+(GPIOE_BASE+0x14-0x40000000)*32+n*4))
#define PFout(n)	 *((volatile uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))


static GPIO_InitTypeDef GPIO_InitStructure;
static EXTI_InitTypeDef EXTI_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;

static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;


void tim3_init(void)
{
	//使能tim3的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	//配置tim3的分频值、计数值
	//tim3硬件时钟=84MHz/8400=10000Hz，就是进行10000次计数，就是1秒时间的到达

	TIM_TimeBaseStructure.TIM_Period = 10000/20-1;	//计数值500,0 -> 499就是50毫秒时间的到达
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//预分频值8400
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//时钟分频，当前是没有的，不需要进行配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	//配置tim3的中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void exti_init(void)
{
	//打开端口A的硬件时钟，就是对端口A供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//打开端口硬件时钟，就是对当前硬件供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//端口的第2根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;//最高速度
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能内部上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	
	
	
	
	//PA0连接到EXTI0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
		
	
	//配置外部中断0
	EXTI_InitStructure.EXTI_Line = EXTI_Line0|EXTI_Line2|EXTI_Line3|EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //检测按键的按下
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能
	EXTI_Init(&EXTI_InitStructure);
	
	//NVIC管理EXTI0，接纳其中断请求和配置它的优先级
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;		//EXTI0的中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级：0x0F
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;//响应（子）优先级：0x0F
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//允许接纳其中断请求，打开该通道
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;	//指定是EXTI2的中断请求通道，填写的是中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级 0x1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//响应优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能EXTI2中断请求通道
	NVIC_Init(&NVIC_InitStructure);		
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;	//指定是EXTI3的中断请求通道，填写的是中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级 0x1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//响应优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能EXTI3中断请求通道
	NVIC_Init(&NVIC_InitStructure);		
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;	//指定是EXTI4的中断请求通道，填写的是中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x04;//抢占优先级 0x1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//响应优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能EXTI4中断请求通道
	NVIC_Init(&NVIC_InitStructure);		
	
	
}

int main(void)
{
	//打开端口硬件时钟，就是对当前硬件供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	
	exti_init();
	tim3_init();	
	
	
	//配置端口指定引脚的功能
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//端口的第9 10根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;//推挽输出模式，使能内部的P-mos&N-mos管
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;//最高速度
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能内部上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	//配置端口指定引脚的功能
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;//端口的第9 10根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;//推挽输出模式，使能内部的P-mos&N-mos管
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;//最高速度
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能内部上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	PFout(9)=1;
	PFout(10)=1;
	PEout(13)=1;
	PEout(14)=1;
	
	
	


	
	while(1)
	{

		
		
	}
}


void EXTI0_IRQHandler(void)
{
	//检测EXTI0是否有中断请求
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		TIM_Cmd(TIM3, ENABLE);
		
		//清空标志位，告诉CPU，当前中断请求已经处理完毕！思考题：如果不清空标志位，会出现什么现象？
		EXTI_ClearITPendingBit(EXTI_Line0);
	
	}
}


void EXTI2_IRQHandler(void)
{
	//检测EXTI0是否有中断请求
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		TIM_Cmd(TIM3, ENABLE);
		
		//清空标志位，告诉CPU，当前中断请求已经处理完毕！思考题：如果不清空标志位，会出现什么现象？
		EXTI_ClearITPendingBit(EXTI_Line2);
	
	}
}

void EXTI3_IRQHandler(void)
{
	//检测EXTI3是否有中断请求
	if(EXTI_GetITStatus(EXTI_Line3) == SET)
	{
		TIM_Cmd(TIM3, ENABLE);
		
		//清空标志位，告诉CPU，当前中断请求已经处理完毕！思考题：如果不清空标志位，会出现什么现象？
		EXTI_ClearITPendingBit(EXTI_Line3);
	
	}
}

void EXTI4_IRQHandler(void)
{
	//检测EXTI0是否有中断请求
	if(EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		TIM_Cmd(TIM3, ENABLE);
		
		//清空标志位，告诉CPU，当前中断请求已经处理完毕！思考题：如果不清空标志位，会出现什么现象？
		EXTI_ClearITPendingBit(EXTI_Line4);
	
	}
}

void TIM3_IRQHandler(void)
{

	//检测标志位
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		if(PAin(0)==0)
			PFout(9)^=1;
		
		if(PEin(2)==0)
			PFout(10)^=1;	
		if(PEin(3)==0)
			PEout(13)^=1;
		if(PEin(4)==0)
			PEout(14)^=1;
		
		
		TIM_Cmd(TIM3, DISABLE);
		//清空标志位
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}



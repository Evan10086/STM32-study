#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void delay(void)
{
	uint32_t i=0x2000000;//unsigned int
	
	while(i--);


}

#define PAin(n)  (*(volatile uint32_t *)(0x42000000+(GPIOA_BASE+0x10-0x40000000)*32+n*4))
#define PFout(n) (*(volatile uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))


int main(void)
{
	//uint32_t *pf9 = (uint32_t *)0x424282A4;
	
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* 使能系统配置硬件时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	
	/* 配置GPIOF的9号引脚为推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//第9号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* 配置GPIOA的0号引脚为输入式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//输入模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//第0号引脚
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	
	/* 将PA0连接到EXTI0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	
	/* 初始化EXTI0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//指定EXTI0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发，用于检测按键的按下 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能EXTI0
	EXTI_Init(&EXTI_InitStructure);
	
	/* 设置NVIC中的EXTI0中断 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//EXTI0的中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;//响应（子）优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//打开EXTI0的请求通道
	NVIC_Init(&NVIC_InitStructure);
	
	
	while(1)
	{
		PFout(9)=PAin(0);
	
	}
	
	//return 0;

}

void EXTI0_IRQHandler(void)
{
	/* 检测标志位 */
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{

		/* 添加用户代码 */
		PFout(9)=0;
		
		delay();
		
		PFout(9)=1;
		
		delay();		

		/* 清空标志位，告诉CPU当前中断请求已完毕，可以响应新EXTI0的中断请求
		思考题：若不清空标志位，会出现什么现象？
		*/
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}


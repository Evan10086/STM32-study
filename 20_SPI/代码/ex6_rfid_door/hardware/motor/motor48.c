#include "stm32f4xx.h"
#include "sys.h"


#define MOTOR_IN1		PCout(9)
#define MOTOR_IN2		PBout(6)
#define MOTOR_IN3		PEout(6)
#define MOTOR_IN4		PAout(8)


void motor_init(void)
{
	
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	/* 使能对应的GPIOA GPIOB GPIOC GPIOE 时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;					//第8根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽模式，增加驱动电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					//第6根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽模式，增加驱动电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					//第6根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽模式，增加驱动电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					//第9根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽模式，增加驱动电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	MOTOR_IN1=MOTOR_IN2=MOTOR_IN3=MOTOR_IN4=1;



	//为系统定时器选择时钟源为168MHz/8=21MHz
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE);
	/* 配置PF9 为PWM输出模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//设置为复用功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽模式，增加驱动电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	

	//配置中断优先级的分组
	//配置支持4个抢占优先级，支持4个响应优先级
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


//}

//双四拍驱动方式正转表360度 AD-DC-CB-BA 
void motor_corotation_double_pos(void)
{
	uint32_t i=0,j=0;
	
	for(i=0; i<64;i++)
	{
		for(j=0;j<8;j++)
		{
			MOTOR_IN4=0;
			MOTOR_IN3=1;
			MOTOR_IN2=1;
			MOTOR_IN1=0;
			delay_ms(2);
			
			MOTOR_IN4=0;
			MOTOR_IN3=0;
			MOTOR_IN2=1;
			MOTOR_IN1=1;
			delay_ms(2);
			
			MOTOR_IN4=1;
			MOTOR_IN3=0;
			MOTOR_IN2=0;
			MOTOR_IN1=1;
			delay_ms(2);
			
			MOTOR_IN4=1;
			MOTOR_IN3=1;
			MOTOR_IN2=0;
			MOTOR_IN1=0;
			delay_ms(2);	
			
					
		}
	}
}

//双四拍驱动方式反转表360度 AB-BC-CD-DA
void motor_corotation_double_rev(void)
{
	uint32_t i=0,j=0;
	
	for(i=0; i<64;i++)
	{
		for(j=0;j<8;j++)
		{
			MOTOR_IN4=1;
			MOTOR_IN3=1;
			MOTOR_IN2=0;
			MOTOR_IN1=0;
			delay_ms(2);
			
			MOTOR_IN4=1;
			MOTOR_IN3=0;
			MOTOR_IN2=0;
			MOTOR_IN1=1;
			delay_ms(2);
			
			MOTOR_IN4=0;
			MOTOR_IN3=0;
			MOTOR_IN2=1;
			MOTOR_IN1=1;
			delay_ms(2);
			
			MOTOR_IN4=0;
			MOTOR_IN3=1;
			MOTOR_IN2=1;
			MOTOR_IN1=0;
			delay_ms(2);	
			
					
		}
	}
}



	



#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;



struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);		
	
	USART_SendData(USART1,ch);
	
	return(ch);
}


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



void usart1_init(uint32_t baud)
{

	/* 打开串口1硬件时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* 打开端口A的硬件时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	
	/* 将GPIO引脚配置为复用功能模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//复用功能模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	/* 将GPIO引脚连接到串口1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	/* 配置串口1相关的参数：波特率、数据位 */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	/* 使能串口1中断相关：接收数据触发、中断优先级 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	/* 使能串口1工作 */
	USART_Cmd(USART1, ENABLE);

}


void usart1_send_str(char *str)
{

	char *p = str;
	
	while(*p)
	{
		
		/* 检测上一次是否发送完毕 */
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);		
		
		USART_SendData(USART1,*p);
		
		p++;
		
	}

}

RTC_TimeTypeDef  RTC_TimeStructure;
RTC_InitTypeDef  RTC_InitStructure;
RTC_DateTypeDef RTC_DateStructure;
EXTI_InitTypeDef  EXTI_InitStructure;

void rtc_init(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	/* 打开备份电路的供电 */
	PWR_BackupAccessCmd(ENABLE);

#if 0
	/* 使能LSE*/
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* 检查该LSE是否有效*/  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	/* 选择LSE作为RTC的硬件时钟源*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	

#else  //若LSE无法工作，可用内部LSI
	/* 使能LSI*/
	RCC_LSICmd(ENABLE);
	
	/* 检查该LSI是否有效*/  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* 选择LSI作为RTC的硬件时钟源*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	

#endif
	
	/* 使能RTC的硬件时钟 */
	RCC_RTCCLKCmd(ENABLE);
	
	/* 等待RTC所有寄存器就绪 */
	RTC_WaitForSynchro();
	


#if 0 //LSE
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
	                = 32.768KHz/(127+1)/(255+1)
					= 1HZ
	*/

	/* Configure the RTC data register and RTC prescaler，配置RTC数据寄存器与RTC的分频值 */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//异步分频系数
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;				//同步分频系数
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;	//24小时格式
	RTC_Init(&RTC_InitStructure);
	
#else //LSI

	/* ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
	                = 32000Hz/(127+1)/(249+1)
					= 1HZ
	*/
	/* Configure the RTC data register and RTC prescaler，配置RTC数据寄存器与RTC的分频值 */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//异步分频系数
	RTC_InitStructure.RTC_SynchPrediv = 0xF9;				//同步分频系数
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;	//24小时格式
	RTC_Init(&RTC_InitStructure);

#endif
	
	/* Set the date: Monday April 24th 2023 */
	RTC_DateStructure.RTC_Year = 0x23;
	RTC_DateStructure.RTC_Month = RTC_Month_April;
	RTC_DateStructure.RTC_Date = 0x24;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	/* Set the time to 16h 17mn 30s AM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	RTC_TimeStructure.RTC_Hours   = 0x16;
	RTC_TimeStructure.RTC_Minutes = 0x17;
	RTC_TimeStructure.RTC_Seconds = 0x30; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
	
	
	//关闭唤醒功能
	RTC_WakeUpCmd(DISABLE);
	
	//为唤醒功能选择RTC配置好的时钟源
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//设置唤醒计数值为自动重载，写入值默认是0
	RTC_SetWakeUpCounter(1-1);
	
	//清除RTC唤醒中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//使能RTC唤醒中断
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//使能唤醒功能
	RTC_WakeUpCmd(ENABLE);

	/* Configure EXTI Line22，配置外部中断控制线22 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;			//当前使用外部中断控制线22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//上升沿触发中断 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			//使能外部中断控制线22
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;		//允许RTC唤醒中断触发
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//抢占优先级为0x3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//响应优先级为0x3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//使能
	NVIC_Init(&NVIC_InitStructure);

}


volatile uint32_t g_rtc_wakeup_event=0;

int main(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* 配置GPIOF的9号引脚为输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	/* 串口1波特率为115200bps (注意:保证PLL配置正确，否则收发数据乱码)*/
	usart1_init(115200);

	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x6666)
	{
		rtc_init();
		
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x6666);
	}
	else/* 假如读取到备份寄存器0的值为0x6666，则进行正常RTC初始化，不需要额外修改日期和时间 */
	{
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		
		/* 打开备份电路的供电 */
		PWR_BackupAccessCmd(ENABLE);

	#if 0
		/* 使能LSE*/
		RCC_LSEConfig(RCC_LSE_ON);
		
		/* 检查该LSE是否有效*/  
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

		/* 选择LSE作为RTC的硬件时钟源*/
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		

	#else  //若LSE无法工作，可用内部LSI
		/* 使能LSI*/
		RCC_LSICmd(ENABLE);
		
		/* 检查该LSI是否有效*/  
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

		/* 选择LSI作为RTC的硬件时钟源*/
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		

	#endif
		
		/* 使能RTC的硬件时钟 */
		RCC_RTCCLKCmd(ENABLE);
		
		/* 等待RTC所有寄存器就绪 */
		RTC_WaitForSynchro();
		


	#if 0 //LSE
		/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
						= 32.768KHz/(127+1)/(255+1)
						= 1HZ
		*/

		/* Configure the RTC data register and RTC prescaler，配置RTC数据寄存器与RTC的分频值 */
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//异步分频系数
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;				//同步分频系数
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;	//24小时格式
		RTC_Init(&RTC_InitStructure);
		
	#else //LSI

		/* ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
						= 32000Hz/(127+1)/(249+1)
						= 1HZ
		*/
		/* Configure the RTC data register and RTC prescaler，配置RTC数据寄存器与RTC的分频值 */
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//异步分频系数
		RTC_InitStructure.RTC_SynchPrediv = 0xF9;				//同步分频系数
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;	//24小时格式
		RTC_Init(&RTC_InitStructure);

	#endif
		
		
		//关闭唤醒功能
		RTC_WakeUpCmd(DISABLE);
		
		//为唤醒功能选择RTC配置好的时钟源
		RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
		
		//设置唤醒计数值为自动重载，写入值默认是0
		RTC_SetWakeUpCounter(1-1);
		
		//清除RTC唤醒中断标志
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		//使能RTC唤醒中断
		RTC_ITConfig(RTC_IT_WUT, ENABLE);

		//使能唤醒功能
		RTC_WakeUpCmd(ENABLE);

		/* Configure EXTI Line22，配置外部中断控制线22 */
		EXTI_InitStructure.EXTI_Line = EXTI_Line22;			//当前使用外部中断控制线22
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//上升沿触发中断 
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;			//使能外部中断控制线22
		EXTI_Init(&EXTI_InitStructure);
		
		NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;		//允许RTC唤醒中断触发
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//抢占优先级为0x3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//响应优先级为0x3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//使能
		NVIC_Init(&NVIC_InitStructure);	
	}
	
	
	printf("This is rtc test\r\n");
	
	while(1)
	{
		if(g_rtc_wakeup_event)
		{
			/* 获取时间 */
			RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);
			printf("Time %02x:%02x:%02x\r\n",	RTC_TimeStructure.RTC_Hours,
												RTC_TimeStructure.RTC_Minutes,
												RTC_TimeStructure.RTC_Seconds);
			/* 获取日期 */	
			RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
			printf("Date 20%02x/%02x/%02x Week:%x\r\n",	RTC_DateStructure.RTC_Year,
														RTC_DateStructure.RTC_Month,
														RTC_DateStructure.RTC_Date,
														RTC_DateStructure.RTC_WeekDay);
			g_rtc_wakeup_event=0;
		}

		
	
	}
	
	//return 0;

}

void RTC_WKUP_IRQHandler(void)
{
	
	/* 检测是否有唤醒中断 */
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		PFout(9)^=1;
		
		g_rtc_wakeup_event=1;

		/* 清空RTC唤醒中断标志位、EXTI22的标志位*/
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line22);
	} 




}

void USART1_IRQHandler(void)
{
	uint8_t d;
	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
	
		d = USART_ReceiveData(USART1);
		

		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}



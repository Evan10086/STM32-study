#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"

static GPIO_InitTypeDef GPIO_InitStructure;//LED灯信息结构体
static NVIC_InitTypeDef NVIC_InitStructure;	//中断优先级信息结构体
static USART_InitTypeDef USART_InitStructure;	//串口信息结构体
static RTC_TimeTypeDef  RTC_TimeStructure;		//RTC信息结构体
static RTC_InitTypeDef  RTC_InitStructure;
static RTC_DateTypeDef RTC_DateStructure;
static RTC_AlarmTypeDef RTC_AlarmStructure; //闹钟信息结构体
static EXTI_InitTypeDef  EXTI_InitStructure;	//中断信息结构体

static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	//定时器信息结构体
static TIM_OCInitTypeDef  TIM_OCInitStructure;	//定时器脉冲信息结构体


static volatile uint8_t aShowTime[50] = {0};

static volatile uint8_t  g_usart1_buf[64]={0};
static volatile uint32_t g_usart1_cnt=0;
static volatile uint32_t g_usart1_event=0;

static volatile uint8_t  g_usart3_buf[64]={0};
static volatile uint32_t g_usart3_cnt=0;
static volatile uint32_t g_usart3_event=0;

/*闹钟*/



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


void usart3_init(uint32_t baud)
{

	/* 打开串口1硬件时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	/* 打开端口B的硬件时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	/* 将GPIO引脚配置为复用功能模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//复用功能模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;//第10 11号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	/* 将GPIO引脚连接到串口3 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	/* 配置串口3相关的参数：波特率、数据位 */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	/* 使能串口3中断相关：接收数据触发、中断优先级 */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	/* 使能串口3工作 */
	USART_Cmd(USART3, ENABLE);
}


void usart3_send_str(char *str)
{
	char *p = str;
	while(*p)
	{
		/* 检测上一次是否发送完毕 */
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);		
		USART_SendData(USART3,*p);
		p++;
	}
}




/* 设置日期 */
void set_rtc_time(void)
{
#if 1
	/* Set the date: Monday April 24th 2023 */
	RTC_DateStructure.RTC_Year = 0x23;
	RTC_DateStructure.RTC_Month = RTC_Month_April;
	RTC_DateStructure.RTC_Date = 0x24;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	/* Set the time to 16h 17mn 30s AM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	RTC_TimeStructure.RTC_Hours   = 0x19;
	RTC_TimeStructure.RTC_Minutes = 0x55;
	RTC_TimeStructure.RTC_Seconds = 0x30; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
	
	
#else
	(void)0;
	
#endif
}

/* 闹钟 */
void set_rtc_clock(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
#if 1

	// 闹钟A已经使能
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	/* 使能报警中断 Enable RTC Alarm A Interrupt */
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
		
	/* 设置闹钟为 xx h:xx min: xx s 05h:20min:30s */
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = hour;	//设置闹钟为 xx 小时
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = minutes;	//设置闹钟为 xx 分钟
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = seconds;	//设置闹钟为 xx 秒
//	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
//	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	
	/* 配置RTC报警寄存器 Configure the RTC Alarm A register */
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
	
	/* RTC报警中断配置 */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* 启用RTC报警中断 */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 使能报警中断 Enable RTC Alarm A Interrupt */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	
	/* 使能报警 Enable the alarm */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	
	RTC_ClearFlag(RTC_FLAG_ALRAF);

#else
	(void)0;
	
#endif
}
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
void RTC_TimeShow(void)
{
	/* Get the current Time */
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	/* Display time Format : hh:mm:ss */
	sprintf((char*)aShowTime,"%0.2d:%0.2d:%0.2d",RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
	/* 获取日期 */	
	RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
	printf("Date 20%02x/%02x/%02x Week:%x\r\n",	RTC_DateStructure.RTC_Year,
												RTC_DateStructure.RTC_Month,
												RTC_DateStructure.RTC_Date,
												RTC_DateStructure.RTC_WeekDay);
	printf("Time: : %s ",aShowTime);
}

/**
  * @brief  Display the current Alarm.
  * @param  None
  * @retval None
  */
void RTC_AlarmShow(void)
{
	uint8_t showalarm[50] = {0};
	/* Get the current Alarm */
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	sprintf((char*)showalarm,"%0.2d:%0.2d:%0.2d", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours,
												  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes,
												  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
	printf("clock : %s ",showalarm);
}







volatile uint32_t g_rtc_wakeup_event=0;
volatile uint32_t g_rtc_clock_event=0;

int main(void)
{
	char *p;
//	char *p_temp;
//	uint8_t year, month, day, week, hour, minute, second;
	/* 一定要为32位类型变量，否则取出字符串数据内容会丢失数据 */
	
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* 使能端口F的硬件时钟，说白就是端口F进行供电，默认情况下，硬件时钟是被关闭的，咱们要开启 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	/* 配置GPIOF的9号引脚为输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_8;//第8  9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* 配置GPIOA的0号引脚为输出模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//第9 10号引脚
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出类型
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//引脚的工作速度，速度越高，响应越快，但是功耗也高。
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//不使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(8)=0;
	PFout(9)=1;
	PFout(10)=1;
	
	
	
	/* 将PA0连接到EXTI0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	
	/* 初始化EXTI0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发，用于检测按键的按下 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能EXTI0
	EXTI_Init(&EXTI_InitStructure);
	
	/* 设置NVIC中的EXTI0中断 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//EXTI0的中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;//响应（子）优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//打开EXTI0的请求通道
	NVIC_Init(&NVIC_InitStructure);
	
	
	
	
	
	
	
	/* 串口1波特率为115200bps (注意:保证PLL配置正确，否则收发数据乱码)*/
	usart1_init(115200);
	
	/* 串口3波特率为9600bps (注意:保证PLL配置正确，否则收发数据乱码)*/
	usart3_init(9600);
	delay_ms(100);
	
	printf("This is ble init finish\r\n");
	
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x6666)
	{
		RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
		
		rtc_init();		//rtc时钟初始化
		set_rtc_time();//设置时间
		set_rtc_clock(0x20, 0x16, 0x30);//配置闹钟
		
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x6666);
	}
	else/* 假如读取到备份寄存器0的值为0x6666，则进行正常RTC初始化，不需要额外修改日期和时间 */
	{
		
		/* 清空RTC唤醒中断标志位、EXTI17的标志位*/
		RTC_ClearFlag(RTC_FLAG_ALRAF);
		EXTI_ClearITPendingBit(EXTI_Line17);

		// 闹钟A未使能
		rtc_init(); //rtc时钟初始化
		
		/* 配置闹钟 */
		
		set_rtc_clock(0x20, 0x16, 0x30);
	}
	
	
	
	
	printf("This is rtc test\r\n");
	
	while(1)
	{
		if(g_rtc_wakeup_event)
		{
			/* 显示日期 */
			RTC_TimeShow();
			/* 显示闹钟时间 */
			RTC_AlarmShow();
			g_rtc_wakeup_event=0;
			
		}
		if(g_rtc_clock_event)	//闹钟响了,按key0关掉才行
		{
			
			g_rtc_clock_event = 0;
			
			PFout(8) = !PFout(8);
			PFout(10) = !PFout(10);  
			delay_ms(500);           
				
		}
		
		if(g_usart1_event || g_usart3_event){
			if(g_usart1_event == 1)
			{
				//DATE SET-2021-2-23-2#
				//TIME SET-10-20-30#
				if(strstr((const char *)g_usart1_buf,"DATE SET"))
				{
					p=strtok((char *)g_usart1_buf,"-");
					
					printf("p=%s\r\n",p);//DATE SET

					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//2021 年
					RTC_DateStructure.RTC_Year = atoi(p)-2000;
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//2 月
					RTC_DateStructure.RTC_Month = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//23 日
					RTC_DateStructure.RTC_Date = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//2# 周
					RTC_DateStructure.RTC_WeekDay = atoi(p); 
					
					RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
					
					printf("DATA set ok\r\n");
				}
				
				if(strstr((const char *)g_usart1_buf,"TIME SET"))
				{
					p=strtok((char *)g_usart1_buf,"-");
					
					printf("p=%s\r\n",p);//DATE SET

					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//时
					RTC_TimeStructure.RTC_Hours = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//分
					RTC_TimeStructure.RTC_Minutes = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//秒
					RTC_TimeStructure.RTC_Seconds = atoi(p);
					
					RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
					
					//usart3_send_str("TIME set ok\r\n");
					printf("TIME set ok\r\n");
				}
				
				if(strstr((const char *)g_usart1_buf,"ALARM SET"))
				{
					p=strtok((char *)g_usart1_buf,"-");
					
					printf("p=%s\r\n",p);//DATE SET

					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//时
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//分
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//秒
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = atoi(p);
					
					RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
					
					RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
					
					/* 让RTC的闹钟A工作*/
					RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
				
					
					//usart3_send_str("TIME set ok\r\n");
					printf("Clcck set ok\r\n");
				}
				
				

				
				
				
				
				
			}
			if(g_usart1_event == 1)
			{
				//同上
			}
			
end:		
			g_usart1_event=0;
			g_usart1_cnt=0;
			g_usart3_event=0;
			g_usart3_cnt=0;
			memset((void *)g_usart1_buf,0,sizeof(g_usart1_buf));
			memset((void *)g_usart3_buf,0,sizeof(g_usart3_buf));
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
		g_usart1_buf[g_usart1_cnt]=d;
		
		g_usart1_cnt++;
		if(d == '#' || g_usart1_cnt>= sizeof(g_usart1_buf))
		{
			g_usart1_event=1;
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void USART3_IRQHandler(void)
{
	uint8_t d;
	
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
	
		d = USART_ReceiveData(USART3);
		
		g_usart3_buf[g_usart3_cnt]=d;
		
		g_usart3_cnt++;
		if(d == '#' || g_usart3_cnt>= sizeof(g_usart3_buf))
		{
			g_usart3_event=1;
		}
		
		
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}
/* 闹钟中断 */
void RTC_Alarm_IRQHandler(void)
{
	/* 检测是否有闹钟唤醒中断 */
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{
		
		
		g_rtc_clock_event = 1;
		
		/* 清空RTC唤醒中断标志位、EXTI17的标志位*/
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	} 
}

/*按键关闭闹钟*/
void EXTI0_IRQHandler(void)
{
	/* 检测标志位 */
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
	
		PFout(10)=1;
		PFout(8)=0;
		

		/* 清空标志位，告诉CPU当前中断请求已完毕，可以响应新EXTI0的中断请求
		思考题：若不清空标志位，会出现什么现象？
		*/
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
	

}

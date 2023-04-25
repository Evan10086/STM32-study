#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"

static GPIO_InitTypeDef GPIO_InitStructure;//LED����Ϣ�ṹ��
static NVIC_InitTypeDef NVIC_InitStructure;	//�ж����ȼ���Ϣ�ṹ��
static USART_InitTypeDef USART_InitStructure;	//������Ϣ�ṹ��
static RTC_TimeTypeDef  RTC_TimeStructure;		//RTC��Ϣ�ṹ��
static RTC_InitTypeDef  RTC_InitStructure;
static RTC_DateTypeDef RTC_DateStructure;
static RTC_AlarmTypeDef RTC_AlarmStructure; //������Ϣ�ṹ��
static EXTI_InitTypeDef  EXTI_InitStructure;	//�ж���Ϣ�ṹ��

static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	//��ʱ����Ϣ�ṹ��
static TIM_OCInitTypeDef  TIM_OCInitStructure;	//��ʱ��������Ϣ�ṹ��


static volatile uint8_t aShowTime[50] = {0};

static volatile uint8_t  g_usart1_buf[64]={0};
static volatile uint32_t g_usart1_cnt=0;
static volatile uint32_t g_usart1_event=0;

static volatile uint8_t  g_usart3_buf[64]={0};
static volatile uint32_t g_usart3_cnt=0;
static volatile uint32_t g_usart3_event=0;

/*����*/



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
	
		SysTick->CTRL = 0; 	// �ر�ϵͳ��ʱ��
		SysTick->LOAD = 21000-1; //
		SysTick->VAL = 0; // ��յ�ǰ����ֵ�Ĵ����һ�����COUNTFLAG��־λ
		SysTick->CTRL = 1; // ʹ��ϵͳ��ʱ��������ʱ��ԴΪ�ο�ʱ��=�ں�ʱ�ӣ�168MHz��/8
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
		
	}
	
	SysTick->CTRL = 0; // �ر�ϵͳ��ʱ��

}

void delay_us(uint32_t n)
{

	SysTick->CTRL = 0; 	// �ر�ϵͳ��ʱ��
	SysTick->LOAD = 168*n-1; // 1us*n
	SysTick->VAL = 0; // ��յ�ǰ����ֵ�Ĵ����һ�����COUNTFLAG��־λ
	SysTick->CTRL = 5; // ʹ��ϵͳ��ʱ��������ʱ��ԴΪ�ں�ʱ�ӣ�168MHz��
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set

	SysTick->CTRL = 0; // �ر�ϵͳ��ʱ��

}



void usart1_init(uint32_t baud)
{

	/* �򿪴���1Ӳ��ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* �򿪶˿�A��Ӳ��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	
	/* ��GPIO��������Ϊ���ù���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	/* ��GPIO�������ӵ�����1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	/* ���ô���1��صĲ����������ʡ�����λ */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	/* ʹ�ܴ���1�ж���أ��������ݴ������ж����ȼ� */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	/* ʹ�ܴ���1���� */
	USART_Cmd(USART1, ENABLE);

}


void usart1_send_str(char *str)
{

	char *p = str;
	
	while(*p)
	{
		
		/* �����һ���Ƿ������ */
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);		
		
		USART_SendData(USART1,*p);
		
		p++;
		
	}

}


void usart3_init(uint32_t baud)
{

	/* �򿪴���1Ӳ��ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	/* �򿪶˿�B��Ӳ��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	/* ��GPIO��������Ϊ���ù���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;//��10 11������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	/* ��GPIO�������ӵ�����3 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	/* ���ô���3��صĲ����������ʡ�����λ */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	/* ʹ�ܴ���3�ж���أ��������ݴ������ж����ȼ� */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	/* ʹ�ܴ���3���� */
	USART_Cmd(USART3, ENABLE);
}


void usart3_send_str(char *str)
{
	char *p = str;
	while(*p)
	{
		/* �����һ���Ƿ������ */
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);		
		USART_SendData(USART3,*p);
		p++;
	}
}




/* �������� */
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

/* ���� */
void set_rtc_clock(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
#if 1

	// ����A�Ѿ�ʹ��
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	/* ʹ�ܱ����ж� Enable RTC Alarm A Interrupt */
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
		
	/* ��������Ϊ xx h:xx min: xx s 05h:20min:30s */
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = hour;	//��������Ϊ xx Сʱ
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = minutes;	//��������Ϊ xx ����
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = seconds;	//��������Ϊ xx ��
//	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
//	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	
	/* ����RTC�����Ĵ��� Configure the RTC Alarm A register */
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
	
	/* RTC�����ж����� */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* ����RTC�����ж� */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* ʹ�ܱ����ж� Enable RTC Alarm A Interrupt */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	
	/* ʹ�ܱ��� Enable the alarm */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	
	RTC_ClearFlag(RTC_FLAG_ALRAF);

#else
	(void)0;
	
#endif
}
void rtc_init(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	/* �򿪱��ݵ�·�Ĺ��� */
	PWR_BackupAccessCmd(ENABLE);

#if 0
	/* ʹ��LSE*/
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* ����LSE�Ƿ���Ч*/  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	/* ѡ��LSE��ΪRTC��Ӳ��ʱ��Դ*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	

#else  //��LSE�޷������������ڲ�LSI
	/* ʹ��LSI*/
	RCC_LSICmd(ENABLE);
	
	/* ����LSI�Ƿ���Ч*/  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* ѡ��LSI��ΪRTC��Ӳ��ʱ��Դ*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	

#endif
	
	/* ʹ��RTC��Ӳ��ʱ�� */
	RCC_RTCCLKCmd(ENABLE);
	
	/* �ȴ�RTC���мĴ������� */
	RTC_WaitForSynchro();
	


#if 0 //LSE
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
	                = 32.768KHz/(127+1)/(255+1)
					= 1HZ
	*/

	/* Configure the RTC data register and RTC prescaler������RTC���ݼĴ�����RTC�ķ�Ƶֵ */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//�첽��Ƶϵ��
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;				//ͬ����Ƶϵ��
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;	//24Сʱ��ʽ
	RTC_Init(&RTC_InitStructure);
	
#else //LSI

	/* ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
	                = 32000Hz/(127+1)/(249+1)
					= 1HZ
	*/
	/* Configure the RTC data register and RTC prescaler������RTC���ݼĴ�����RTC�ķ�Ƶֵ */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//�첽��Ƶϵ��
	RTC_InitStructure.RTC_SynchPrediv = 0xF9;				//ͬ����Ƶϵ��
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;	//24Сʱ��ʽ
	RTC_Init(&RTC_InitStructure);

#endif

	
	//�رջ��ѹ���
	RTC_WakeUpCmd(DISABLE);
	
	//Ϊ���ѹ���ѡ��RTC���úõ�ʱ��Դ
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//���û��Ѽ���ֵΪ�Զ����أ�д��ֵĬ����0
	RTC_SetWakeUpCounter(1-1);
	
	//���RTC�����жϱ�־
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//ʹ��RTC�����ж�
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//ʹ�ܻ��ѹ���
	RTC_WakeUpCmd(ENABLE);

	/* Configure EXTI Line22�������ⲿ�жϿ�����22 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;			//��ǰʹ���ⲿ�жϿ�����22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//�����ش����ж� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			//ʹ���ⲿ�жϿ�����22
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;		//����RTC�����жϴ���
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//��ռ���ȼ�Ϊ0x3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//��Ӧ���ȼ�Ϊ0x3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//ʹ��
	NVIC_Init(&NVIC_InitStructure);

}
void RTC_TimeShow(void)
{
	/* Get the current Time */
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	/* Display time Format : hh:mm:ss */
	sprintf((char*)aShowTime,"%0.2d:%0.2d:%0.2d",RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
	/* ��ȡ���� */	
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
	/* һ��ҪΪ32λ���ͱ���������ȡ���ַ����������ݻᶪʧ���� */
	
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	/* ����GPIOF��9������Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_8;//��8  9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	/* ����GPIOA��0������Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(8)=0;
	PFout(9)=1;
	PFout(10)=1;
	
	
	
	/* ��PA0���ӵ�EXTI0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	
	/* ��ʼ��EXTI0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش��������ڼ�ⰴ���İ��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��EXTI0
	EXTI_Init(&EXTI_InitStructure);
	
	/* ����NVIC�е�EXTI0�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//EXTI0���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;//��Ӧ���ӣ����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//��EXTI0������ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
	
	
	
	
	
	
	/* ����1������Ϊ115200bps (ע��:��֤PLL������ȷ�������շ���������)*/
	usart1_init(115200);
	
	/* ����3������Ϊ9600bps (ע��:��֤PLL������ȷ�������շ���������)*/
	usart3_init(9600);
	delay_ms(100);
	
	printf("This is ble init finish\r\n");
	
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x6666)
	{
		RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
		
		rtc_init();		//rtcʱ�ӳ�ʼ��
		set_rtc_time();//����ʱ��
		set_rtc_clock(0x20, 0x16, 0x30);//��������
		
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x6666);
	}
	else/* �����ȡ�����ݼĴ���0��ֵΪ0x6666�����������RTC��ʼ��������Ҫ�����޸����ں�ʱ�� */
	{
		
		/* ���RTC�����жϱ�־λ��EXTI17�ı�־λ*/
		RTC_ClearFlag(RTC_FLAG_ALRAF);
		EXTI_ClearITPendingBit(EXTI_Line17);

		// ����Aδʹ��
		rtc_init(); //rtcʱ�ӳ�ʼ��
		
		/* �������� */
		
		set_rtc_clock(0x20, 0x16, 0x30);
	}
	
	
	
	
	printf("This is rtc test\r\n");
	
	while(1)
	{
		if(g_rtc_wakeup_event)
		{
			/* ��ʾ���� */
			RTC_TimeShow();
			/* ��ʾ����ʱ�� */
			RTC_AlarmShow();
			g_rtc_wakeup_event=0;
			
		}
		if(g_rtc_clock_event)	//��������,��key0�ص�����
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
					printf("p=%s\r\n",p);//2021 ��
					RTC_DateStructure.RTC_Year = atoi(p)-2000;
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//2 ��
					RTC_DateStructure.RTC_Month = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//23 ��
					RTC_DateStructure.RTC_Date = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//2# ��
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
					printf("p=%s\r\n",p);//ʱ
					RTC_TimeStructure.RTC_Hours = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//��
					RTC_TimeStructure.RTC_Minutes = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//��
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
					printf("p=%s\r\n",p);//ʱ
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//��
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = atoi(p);
					
					p=strtok(NULL,"-");
					if(p==NULL)
						goto end;
					printf("p=%s\r\n",p);//��
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = atoi(p);
					
					RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
					
					RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
					
					/* ��RTC������A����*/
					RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
				
					
					//usart3_send_str("TIME set ok\r\n");
					printf("Clcck set ok\r\n");
				}
				
				

				
				
				
				
				
			}
			if(g_usart1_event == 1)
			{
				//ͬ��
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
	
	/* ����Ƿ��л����ж� */
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		PFout(9)^=1;
		
		g_rtc_wakeup_event=1;

		/* ���RTC�����жϱ�־λ��EXTI22�ı�־λ*/
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
/* �����ж� */
void RTC_Alarm_IRQHandler(void)
{
	/* ����Ƿ������ӻ����ж� */
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{
		
		
		g_rtc_clock_event = 1;
		
		/* ���RTC�����жϱ�־λ��EXTI17�ı�־λ*/
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	} 
}

/*�����ر�����*/
void EXTI0_IRQHandler(void)
{
	/* ����־λ */
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
	
		PFout(10)=1;
		PFout(8)=0;
		

		/* ��ձ�־λ������CPU��ǰ�ж���������ϣ�������Ӧ��EXTI0���ж�����
		˼���⣺������ձ�־λ�������ʲô����
		*/
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
	

}

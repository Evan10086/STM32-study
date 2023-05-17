#include "stm32f4xx.h"


void rtc_init(void)
{
	RTC_InitTypeDef  RTC_InitStructure;		
	RTC_DateTypeDef  RTC_DateStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;		
	
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);


	/* Enable the LSE OSC��ʹ���ⲿ����ʱ��32.868KHz */
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* Wait till LSE is ready������ⲿ��ʱ���Ƿ���Ч */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	/* Select the RTC Clock Source,LSE��ΪRTC��ʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);


	/* Enable the RTC Clock ��ʹ��RTCӲ��ʱ��*/
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC APB registers synchronisation���ȴ�RTC��ؼĴ������� */
	RTC_WaitForSynchro();
	
	/* Configure the RTC data register and RTC prescaler */
	/* ck_spre(1Hz) = RTCCLK(LSE) /(RTC_AsynchPrediv + 1)*(RTC_SynchPrediv + 1)*/
	RTC_InitStructure.RTC_AsynchPrediv = 127;
	RTC_InitStructure.RTC_SynchPrediv = 255;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;//24Сʱ��ʽ
	RTC_Init(&RTC_InitStructure);


	/* Set the date: Thursday October 15th 2020 */
	RTC_DateStructure.RTC_Year = 0x20;
	RTC_DateStructure.RTC_Month = RTC_Month_October;
	RTC_DateStructure.RTC_Date = 0x15;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Thursday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	/* Set the time to 10h 45mn 30s AM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x10;
	RTC_TimeStructure.RTC_Minutes = 0x45;
	RTC_TimeStructure.RTC_Seconds = 0x30; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
  
	//�رջ��ѹ���
	RTC_WakeUpCmd(DISABLE);
	
	//Ϊ���ѹ���ѡ��ʱ��Դ1Hz
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//���ý���1�μ���������1���ʱ��
	RTC_SetWakeUpCounter(0);
}

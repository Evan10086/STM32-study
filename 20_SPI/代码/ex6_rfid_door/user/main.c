#include "stm32f4xx.h"
#include "stdio.h"
#include "sys.h"
#include "usart.h"
#include "w25qxx.h"
#include "led.h"
#include "key.h"
#include "mfrc522.h"
#include "delay.h"
#include "string.h"
#include "rtc.h"
#include "beep.h"
#include "pwm.h"
#include "motor48.h"



#define STA_CARD_REQUEST		0
#define STA_CARD_FOUND			1
#define STA_CARD_VALID_FOUND	2
#define STA_CARD_INVALID_FOUND	3
#define STA_CARD_VALID_ADD		4


#define MAX_CARDS				100


static int32_t  spi_flash_card_match(uint8_t *card_id);
static int32_t  spi_flash_card_add(uint8_t *card_id);
static int32_t  spi_flash_card_times(uint8_t *card_id);
static uint32_t spi_flash_card_total(void);
static int32_t  spi_flash_card_list_all(void);
static int32_t  spi_flash_card_remove_all(void);



int main(void)
{ 
	uint32_t t=0;
	int32_t rt=0;
	
	uint32_t sta=STA_CARD_REQUEST;
	uint8_t  card_type[2];
	uint8_t  card_id[5]={0};
	uint8_t  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
	uint8_t  card_writebuf[16]={0};
	uint8_t  card_readbuf[18]={0};	
	
	
	RTC_DateTypeDef  RTC_DateStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;	
	
	/* 设置中断优先级分组4 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* led初始化 */
	//led_init();
	
	/* pwm初始化 */
	pwm_init();
	
	/* 串口1,波特率115200bps,开启接收中断 */
	usart1_init(115200);
	
	/* w25qxx初始化 */
	w25qxx_init();
	
	/* 按键初始化 */
	key_init();

	/* 蜂鸣器初始化 */	
	beep_init();
	
	/* rtc初始化 */
	rtc_init();
	

	motor_init();
	
	
	delay_ms(500);
	
	
	printf("This is smart card door lock test by Teacher.Wen\r\n");
	
	/* 检查spi flash 是否有存储有效卡 */	
	if(0 == spi_flash_card_total())
	{
		printf("[NOTICE] Please add valid card in spi flash\r\n");
	}
	
	/* 亮度暗表示空闲状态 */
	pwm_led(100,95);
	
	while(1)
	{
		delay_ms(1);
		
		if(t++ > 1000)
			t=0;
		
		/* 检测状态是否请求卡 */
		if(sta == STA_CARD_REQUEST)
		{
			/* 每500ms寻卡一次 */
			if(t==500)
			{
				MFRC522_Initializtion();
	
				if(MI_OK == MFRC522_Request(0x52, card_type))
				{
					/* 状态变更为找到卡 */
					sta = STA_CARD_FOUND;
					
					printf("[NOTICE] Card Request success,card type is %02X %02X\r\n",card_type[0],card_type[1]);
					
					continue;
				
				}
			}
		}
		
		/* 检测状态是否找到卡 */
		if(sta == STA_CARD_FOUND)
		{
			if(MI_OK == MFRC522_Anticoll(card_id))
			{
				/* 打开找到的卡 */
				printf("[NOTICE] Card %02X%02X%02X%02X found\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);
				
				/* 先查询spi flash有没有存在该卡 */
				rt = spi_flash_card_match(card_id);
				
				if(rt < 0)
					/* 识别到无效卡 */
					sta = STA_CARD_INVALID_FOUND;
				else
					/* 识别到有效卡 */
					sta = STA_CARD_VALID_FOUND;
					
			
			}
		}
	
		if(sta == STA_CARD_VALID_FOUND)
		{
			/* 点亮LED1 */
			pwm_led(100,0);	
			
			/* 更新该卡在spi flash中识别的次数 */
			spi_flash_card_times(card_id);
			
			MFRC522_SelectTag(card_id);
			
		
			
			/* 验卡 */
			if(MI_OK!=MFRC522_Auth(0x60, 4, card_key0Abuf, card_id))
			{
				printf("[ERROR] Card %02X%02X%02X%02X Auth fail\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);
				
				/* 状态变更为寻卡状态 */
				sta = STA_CARD_REQUEST;		
				
				continue;
			}				
			
			/* 读卡 */
			if(MI_OK!=MFRC522_Read(4, card_readbuf))
			{
				printf("[ERROR] Card %02X%02X%02X%02X read data block 4 fail\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);				
				
				/* 状态变更为寻卡状态 */
				sta = STA_CARD_REQUEST;		
				
				continue;				
			
			}

			if(card_readbuf[0]=='@')
			{
				/* 打印出卡上一次存储RTC的日期时间 */
				printf("[NOTICE] Card %02X%02X%02X%02X %04X/%02X/%02X Week %x %02X:%02X:%02X last\r\n",
				card_id[0],card_id[1],card_id[2],card_id[3],
				card_readbuf[1]+0x2000,card_readbuf[2],card_readbuf[3],card_readbuf[4],
				card_readbuf[5],card_readbuf[6],card_readbuf[7]);
			}
			else
			{
				printf("[NOTICE] Card %02X%02X%02X%02X data block 4 is not recorded\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);
			}
			

			
			/* RTC_GetTime，获取时间 */
			RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);
			
			/* RTC_GetDate，获取日期 */
			RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
			
			/* 设置要向卡写入的数据格式 */
			card_writebuf[0]='@';
			card_writebuf[1]=RTC_DateStructure.RTC_Year;
			card_writebuf[2]=RTC_DateStructure.RTC_Month;
			card_writebuf[3]=RTC_DateStructure.RTC_Date;
			card_writebuf[4]=RTC_DateStructure.RTC_WeekDay;
			card_writebuf[5]=RTC_TimeStructure.RTC_Hours;
			card_writebuf[6]=RTC_TimeStructure.RTC_Minutes;
			card_writebuf[7]=RTC_TimeStructure.RTC_Seconds;	
			
			/* 写卡 */
			if(MI_OK == MFRC522_Write(4, card_writebuf))
			{
				/* printf打印出日期和时间 */		
				printf("Card %02X%02X%02X%02X  20%02X/%X/%X Week %X %02X:%02X:%02X now\r\n",
					card_id[0],card_id[1],card_id[2],card_id[3],
					RTC_DateStructure.RTC_Year,
					RTC_DateStructure.RTC_Month,
					RTC_DateStructure.RTC_Date,
					RTC_DateStructure.RTC_WeekDay,
					RTC_TimeStructure.RTC_Hours,
					RTC_TimeStructure.RTC_Minutes,
					RTC_TimeStructure.RTC_Seconds);			
			
				/* 蜂鸣器嘀一声示意 */
				PFout(8)=1;
				delay_ms(50);
				PFout(8)=0;
				
				/*开门*/
				motor_corotation_double_rev();
			}
			else
			{
				/* 蜂鸣器嘀两声示意 */
				PFout(8)=1;delay_ms(50);				
				PFout(8)=0;delay_ms(50);		
				PFout(8)=1;delay_ms(50);				
				PFout(8)=0;delay_ms(50);				
			}

			/* 一直检测卡是否已经离开 */		
			while(1)
			{
				
				if(MI_OK != MFRC522_Read(4,card_readbuf))
					break;
	
				delay_ms(100);
			}
			
			printf("[NOTICE ] Card has left\r\n");
			
			/* 亮度暗表示空闲状态 */
			pwm_led(100,95);			
			
			/* 状态变更为寻卡状态 */
			sta = STA_CARD_REQUEST;			
		}

		if(sta == STA_CARD_INVALID_FOUND)
		{
			/* 点亮LED1 */
			pwm_led(100,0);		

			/* 蜂鸣器响 */
			PFout(8)=1;	
			
			
			
			printf("Card %02X%02X%02X%02X is invalid\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);

			/* 一直检测卡是否已经离开 */		
			while(1)
			{
				
				if(MI_OK != MFRC522_Anticoll(card_id))
					break;
					
				delay_ms(100);
			}
			
			printf("[NOTICE ] Card has left\r\n");
			
			/* 亮度暗表示空闲状态 */
			pwm_led(100,95);		
			
			/* 蜂鸣器关闭 */
			PFout(8)=0;				
			
			/* 状态变更为寻卡状态 */
			sta = STA_CARD_REQUEST;	
		}		
		
		if(PAin(0) == 0)
		{
			sta = STA_CARD_VALID_ADD;
			
			t=0;
			
			/* 进入慢速闪烁状态 */
			pwm_led(1,50);	

			
			while(1)
			{
				t++;
				
				delay_ms(1);
				
				if((t % 500) ==0)
				{
					/* 卡初始化 */
					MFRC522_Initializtion();
			
					if(MI_OK == MFRC522_Request(0x52, card_type))
					{
						if(MI_OK == MFRC522_Anticoll(card_id))
						{
							
							/* 先查询spi flash有没有存在该卡 */
							rt = spi_flash_card_match(card_id);
							
							/* spi flash没有存储该卡 */
							if(rt < 0)
							{
								/* 添加卡 */
								rt = spi_flash_card_add(card_id);
								
								if(rt == 0)
									printf("[NOTICE] Spi flash add card %02X%02X%02X%02X success\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);
								else
									printf("[ERROR] Spi flash add card %02X%02X%02X%02X fail\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);
							
							}
							/* spi flash 已存储该卡 */
							else
							{
								printf("[NOTICE] Spi flash card %02X%02X%02X%02X alread exist\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);
							}

						}
					}
				}
				
				
				if(t >= 1000)
					t=0;

				/* 退出添加卡操作 */
				if(PEin(2) == 0)
					break;
			
			}
			
			t=0;
			
			/* 亮度暗表示空闲状态 */
			pwm_led(100,95);		
		
			/* 状态变更为寻卡状态 */
			sta = STA_CARD_REQUEST;
		}
		
		/* 列出spi flash存储的卡id */
		if(PEin(3) == 0)
		{
			delay_ms(20);
			
			if(PEin(3) == 0)
			{
				/* 列出spi flash存储的有效卡记录 */
				spi_flash_card_list_all();
				
				while(PEin(3)==0);
			}
			
		}
		
		/* 清空spi flash存储的卡id */
		if(PEin(4) == 0)
		{
			delay_ms(20);
			
			if(PEin(4) == 0)
			{			
				
				/* 清空spi flash存储的有效卡记录 */
				spi_flash_card_remove_all();
				
				printf("[NOTICE] All cards stored in spi flash have been removed\r\n");
				
				while(PEin(4)==0);
				
			}
		}		
		
	}
} 


int32_t spi_flash_card_match(uint8_t *card_id)
{
	uint8_t  card_id_format[7]={0};	
	uint32_t i;
	
	for(i=0;i<(MAX_CARDS*(sizeof card_id_format));i+=(sizeof card_id_format))
	{
		w25qxx_read_data(i,card_id_format,sizeof card_id_format);
		
		if(card_id_format[0]!='C')
			return -1;
		
		if(0 == memcmp(card_id,&card_id_format[1],4))
		{
			/* 返回该卡在spi flash的存储地址 */
			return i;
		}
	}
	
	return -1;
}


int32_t spi_flash_card_times(uint8_t *card_id)
{
	uint8_t  card_id_format[7]={0};	
	int32_t i;	
	
	i = spi_flash_card_match(card_id);
	
	if(i < 0)
		return -1;
	
	w25qxx_read_data(i,card_id_format,sizeof card_id_format);
	
	/* 该卡识别的次数加1 */
	if(card_id_format[5] < 255)
		card_id_format[5]+=1;

	/* 更新该卡的识别次数 */
	w25qxx_write_data_ex(i+5,&card_id_format[5],1);
	
	return 0;
}

uint32_t spi_flash_card_total(void)
{
	uint8_t  card_id_format[7]={0};	
	uint32_t i;
	uint32_t total=0;	
	
	for(i=0;i<(MAX_CARDS*(sizeof card_id_format));i+=(sizeof card_id_format))
	{
		w25qxx_read_data(i,card_id_format,sizeof card_id_format);
		
		/* 若没有检测到'C'，即没有存储到卡 */
		if(card_id_format[0]!='C')
			break;
		
		total++;
	}
	
	return total;
}


int32_t spi_flash_card_list_all(void)
{
	uint8_t  card_id_format[7]={0};	
	uint32_t i;
	uint32_t total=0;	
	
	printf("\r\n[NOTICE] valid card list:\r\n");
	
	for(i=0;i<(MAX_CARDS*(sizeof card_id_format));i+=(sizeof card_id_format))
	{
		w25qxx_read_data(i,card_id_format,sizeof card_id_format);
		
		/* 若没有检测到'C'，即没有存储到卡 */
		if(card_id_format[0]!='C')
			break;
		
		total++;
		
		printf("[%03d] card %02X%02X%02X%02X %03d\r\n",
				total,
				card_id_format[1],
				card_id_format[2],
				card_id_format[3],
				card_id_format[4],
				card_id_format[5]);		
	}
	
	//printf("card total is %d:\r\n",total);
	
	return total;
}

int32_t spi_flash_card_remove_all(void)
{
	/* 擦除扇区0  */
	w25qxx_erase_sector(0);
	
	return 0;
}

int32_t spi_flash_card_add(uint8_t *card_id)
{
	uint8_t  card_id_format[7]={0};	
	uint32_t i;
	uint32_t total=0;	
	
	for(i=0;i<(MAX_CARDS*(sizeof card_id_format));i+=(sizeof card_id_format))
	{
		w25qxx_read_data(i,card_id_format,sizeof card_id_format);
		
		/* 若没有检测到'C'，即没有存储到卡 */
		if(card_id_format[0]!='C')
			break;
		
		total++;
	}
	
	/* 检测是否存储已满 */
	if(total >= MAX_CARDS)
		return -1;
	
	card_id_format[0]='C';
	card_id_format[1]=card_id[0];
	card_id_format[2]=card_id[1];
	card_id_format[3]=card_id[2];
	card_id_format[4]=card_id[3];
	card_id_format[5]=0;
	card_id_format[6]='#';
	
	/* 向spi flash存储有效卡 */
	w25qxx_write_data_ex(i,card_id_format,sizeof card_id_format);
	
	return 0;
}


void USART1_IRQHandler(void)                				//串口1中断服务程序
{
	uint8_t d;
	

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  	//接收中断
	{
		d = USART_ReceiveData(USART1);
				 		
	} 
} 





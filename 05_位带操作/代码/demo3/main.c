#include "stm32f4xx.h"

GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	volatile uint32_t i=0x2000000;//unsigned int
	
	while(i--);


}


#define PFout(n) (*(uint32_t *)(0x42000000+(GPIOF_BASE+0x14-0x40000000)*32+n*4))

static uint32_t g=0;


int main(void)
{
	//uint32_t *pf9 = (uint32_t *)0x424282A4;
	
	
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* ����GPIOF��9������Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	while(1)
	{
		/* ������ */
		//GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
		//*pf9=0;
		PFout(9)=0;
		
		g++;
		
		
		/* ��ʱһ��*/
		delay();
		
		/* Ϩ��� */
		//GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET);
		//*pf9=1;
		PFout(9)=1;		
		
		/* ��ʱһ��*/		
		delay();
		
		
		/* ������ */
		PFout(10)=0;
		
		g++;
		
		/* ��ʱһ��*/
		delay();
		
		/* Ϩ��� */

		PFout(10)=1;		
		
		/* ��ʱһ��*/		
		delay();		
	
	}
	
	//return 0;

}

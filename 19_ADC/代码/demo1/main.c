#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;
ADC_InitTypeDef       ADC_InitStructure;
ADC_CommonInitTypeDef ADC_CommonInitStructure;



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

void adc_init(void)
{
	/* ��Ӳ��ʱ��:GPIO��ADC	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* ����PA5����ΪANģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;//ģ���ź�ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;//��5������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	/* ����ADC��صĲ��� */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����1��ADC����
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//ADCӲ��ʱ��=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//��ʹ��DMA
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//�����������ʱ������5*1/42MHz�������ԽС������Խ�Ǳ���
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC1 Init */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12bit�ķֱ��ʣ��õ�����ֵ��Χ0~4095
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��Ե���ͨ����ҪдDISABLE�����ɨ����ͨ����������ΪENABLE
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//������������ENABLE�����ι���������DISABLE��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//����Ҫ�ⲿ����������ADC�Ĺ���
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�����12bit�������Ҷ���洢
	ADC_InitStructure.ADC_NbrOfConversion = 1;//ɨ����ٸ�ͨ��������ָ���˵���ͨ������Ҫд1�����ʹ���˶��ͨ��������Ҫ��д���������
	ADC_Init(ADC1, &ADC_InitStructure);
	
	
	/* ����ADC1��ͨ��5����ת�������ڹ���ͨ�������к�Ϊ1������ʱ��Ϊ3��ADCʱ�����ڣ�3*1/42MHz��*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);

	
	/* ʹ��ADC���� */
	ADC_Cmd(ADC1, ENABLE);


}


int main(void)
{

	uint32_t d;
	
	uint16_t adc_val;
	uint16_t adc_vol;
	
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ�˵�׾��Ƕ˿�F���й��磬Ĭ������£�Ӳ��ʱ���Ǳ��رյģ�����Ҫ���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	/* ����GPIOF��9������Ϊ���ù���ģʽ */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;//���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//��9 10������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//���ŵĹ����ٶȣ��ٶ�Խ�ߣ���ӦԽ�죬���ǹ���Ҳ�ߡ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;//��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	/* ��PF9�������ӵ�TIM14 */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	
	PFout(9)=1;
	
	
	/* ����1������Ϊ115200bps (ע��:��֤PLL������ȷ�������շ���������)*/
	usart1_init(115200);
	
	
	adc_init();
	
	/* �������ADC1 */
	ADC_SoftwareStartConv(ADC1);


	printf("This is adc test\r\n");
	


	while(1)
	{
		/* ��ȡADC1��ת�����ֵ */
		adc_val=ADC_GetConversionValue(ADC1);
		
		adc_vol = adc_val * 3300 /4095;
		
		printf("adc_val = %d\r\n",adc_val);
		printf("adc_vol = %dmv\r\n",adc_vol);	
		
		delay_ms(1000);
		
	
	}
	
	//return 0;

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



#include "stm32f4xx.h"
#include "sys.h"


#define MOTOR_IN1		PCout(9)
#define MOTOR_IN2		PBout(6)
#define MOTOR_IN3		PEout(6)
#define MOTOR_IN4		PAout(8)


void motor_init(void)
{
	
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	/* ʹ�ܶ�Ӧ��GPIOA GPIOB GPIOC GPIOE ʱ��*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;					//��8������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					//��6������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					//��6������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					//��9������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	MOTOR_IN1=MOTOR_IN2=MOTOR_IN3=MOTOR_IN4=1;



	//Ϊϵͳ��ʱ��ѡ��ʱ��ԴΪ168MHz/8=21MHz
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE);
	/* ����PF9 ΪPWM���ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//����Ϊ���ù���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	

	//�����ж����ȼ��ķ���
	//����֧��4����ռ���ȼ���֧��4����Ӧ���ȼ�
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


//}

//˫����������ʽ��ת��360�� AD-DC-CB-BA 
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

//˫����������ʽ��ת��360�� AB-BC-CD-DA
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



	



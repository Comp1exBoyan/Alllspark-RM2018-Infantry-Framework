#include "led.h"
/*
 * ��������TIM12_Configuration
 * ����  : ������PWM��ʼ��
 * IO���ã�PF14(��ɫ) PE7(��ɫ) 
 * ˵  �����͵�ƽ����  �ߵ�ƽϨ��
 * ����  ����
 * ���  ����
 */ 
void LED_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	/*				��GPIOE�Ķ˿�ʱ��			*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

	/*				����ģʽ����			*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶ�
	GPIO_Init(GPIOF,&GPIO_InitStructure);							//��ʼ������
	red_off();
		/*				����ģʽ����			*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶ�
	GPIO_Init(GPIOE,&GPIO_InitStructure);							//��ʼ������
	green_off();
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    
    GPIO_InitTypeDef	LASER;
    LASER.GPIO_Pin = GPIO_Pin_13;	
    LASER.GPIO_Mode = GPIO_Mode_OUT;
    LASER.GPIO_OType = GPIO_OType_PP;
    LASER.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOG, &LASER);
    GPIO_SetBits(GPIOG,GPIO_Pin_13);//cyq
		
}


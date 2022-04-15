#include "init.h"
#include "can1.h"
#include "usart3.h"
#include "JudgeSystem.h"

//#include "FreeCarsInt.h"
/*-------------------------------------------------------------------------------------------------------------------
������  ��ctrl_Init����
�������� :
����    ��2016.12.30
����    ���쿣��ģ�
-----------------------------------------------------------------------------------------------------------------------*/
u8 ctrl_Init(void)
{
	int yuntai_int_flag,dipan_int_flag,status;;
	/*--------------------һ���ʼ��--------------------*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ���

	KEY_Configuration();							//������ʼ��  ��ʱδʹ�õ�
	LED_Init();										//LED ��ʼ�� red_on() red_off() green_on() green_off()
	USART1_Configuration();							//sbus��ʼ��
	
	USART2_Config(115200);
	USART3_Configuration(115200);
	/*--------------------���̳�ʼ��-------------------*/
	dipan_int_flag=dipan_int();			
	delay_ms(50);
	/*----------------------������ʼ��-----------------*/ 
	WeaponInit();
	delay_ms(50);
	/*----------------------��̨��ʼ��-----------------*/ 
	yuntai_int_flag=yuntai_int();
	delay_ms(50);
	/*----------------------IMU��ʼ��------------------*/ 
	IMU_int();
	delay_ms(50);
	
	AutoCorrect_YunTai(2);  // 2Ϊ�Զ�У׼  3Ϊ�ֶ�У׼

	//	/*----------------------������ʼ��-----------------*/ 								
	IWDG_Init(4,1000); 								    //���Ź���ʼ�������Ƶ��Ϊ64,����ֵΪ100,���ʱ��Ϊ1s	  ���Ź���ר�γ����ܷɣ�
	delay_ms(50);

	/*----------------------��ʼ�����-----------------*/
	green_on();

	if(dipan_int_flag&&yuntai_int_flag)  status=1;
	else status=0;
	
	return (status);
}

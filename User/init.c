#include "init.h"
#include "can1.h"
#include "usart3.h"
#include "JudgeSystem.h"

//#include "FreeCarsInt.h"
/*-------------------------------------------------------------------------------------------------------------------
函数名  ：ctrl_Init（）
函数变量 :
日期    ：2016.12.30
作者    ：杨博炜（改）
-----------------------------------------------------------------------------------------------------------------------*/
u8 ctrl_Init(void)
{
	int yuntai_int_flag,dipan_int_flag,status;;
	/*--------------------一般初始化--------------------*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组

	KEY_Configuration();							//按键初始化  暂时未使用到
	LED_Init();										//LED 初始化 red_on() red_off() green_on() green_off()
	USART1_Configuration();							//sbus初始化
	
	USART2_Config(115200);
	USART3_Configuration(115200);
	/*--------------------底盘初始化-------------------*/
	dipan_int_flag=dipan_int();			
	delay_ms(50);
	/*----------------------武器初始化-----------------*/ 
	WeaponInit();
	delay_ms(50);
	/*----------------------云台初始化-----------------*/ 
	yuntai_int_flag=yuntai_int();
	delay_ms(50);
	/*----------------------IMU初始化------------------*/ 
	IMU_int();
	delay_ms(50);
	
	AutoCorrect_YunTai(2);  // 2为自动校准  3为手动校准

	//	/*----------------------其他初始化-----------------*/ 								
	IWDG_Init(4,1000); 								    //看门狗初始化，与分频数为64,重载值为100,溢出时间为1s	  看门狗（专治程序跑飞）
	delay_ms(50);

	/*----------------------初始化完成-----------------*/
	green_on();

	if(dipan_int_flag&&yuntai_int_flag)  status=1;
	else status=0;
	
	return (status);
}

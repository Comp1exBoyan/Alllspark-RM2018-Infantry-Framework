#include "led.h"
/*
 * 函数名：TIM12_Configuration
 * 描述  : 蜂鸣器PWM初始化
 * IO配置：PF14(红色) PE7(绿色) 
 * 说  明：低电平点亮  高电平熄灭
 * 输入  ：无
 * 输出  ：无
 */ 
void LED_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	/*				打开GPIOE的端口时钟			*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

	/*				引脚模式配置			*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推完输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度
	GPIO_Init(GPIOF,&GPIO_InitStructure);							//初始化引脚
	red_off();
		/*				引脚模式配置			*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//设置为输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推完输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度
	GPIO_Init(GPIOE,&GPIO_InitStructure);							//初始化引脚
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


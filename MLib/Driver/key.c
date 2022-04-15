#include "key.h"
#include "main.h"
/*
 *函数名：KEY_Configuration
 *初始化按键
 *IO :PD10
 * 输入  ：prer rlr
 * 输出  ：无
 */ 
void KEY_Configuration(void)
{
    GPIO_InitTypeDef  gpio;
    EXTI_InitTypeDef  exti;
    NVIC_InitTypeDef  nvic;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
   
    gpio.GPIO_Pin = GPIO_Pin_10;   
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOD, &gpio);
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD,EXTI_PinSource10);
    
    exti.EXTI_Line = EXTI_Line10;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti);
    
    nvic.NVIC_IRQChannel = EXTI15_10_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}
int flag=0;
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line10))
    {
			//编写控制程序
      EXTI_ClearITPendingBit(EXTI_Line10);
			flag=KEY_Scan(0);
    }
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
static u8 key_up=1,KEY0_case=0;//按键按松开标志
u8 KEY_Scan(u8 mode)
{	 
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&KEY0==0)
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return (KEY0_case=!KEY0_case);
	}else if(KEY0==1)
	{
	delay_ms(10);//去抖动 
	key_up=1; 	
	}  
	return KEY0_case;
}

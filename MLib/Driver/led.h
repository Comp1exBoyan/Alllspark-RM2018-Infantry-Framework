#ifndef	__LED_H_
#define __LED_H_

#include "stm32f4xx.h"


//º¯ÊýÉùÃ÷
void LED_Init(void);
#define  red_on()          GPIO_ResetBits(GPIOE, GPIO_Pin_7)
#define  red_off()         GPIO_SetBits(GPIOE, GPIO_Pin_7)
#define  green_on()        GPIO_ResetBits(GPIOF, GPIO_Pin_14)
#define  green_off()       GPIO_SetBits(GPIOF, GPIO_Pin_14)
#define  LaserOn()         GPIO_SetBits(GPIOG,GPIO_Pin_13);
#define  LaserOff()        GPIO_ResetBits(GPIOG,GPIO_Pin_13);//cyqGPIO_ResetBits


#endif

#ifndef __KEY_H__
#define __KEY_H__
#include "main.h"
void KEY_Configuration(void);
#define KEY0 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10) //PD4
u8 KEY_Scan(u8 mode);
extern int flag;
#endif

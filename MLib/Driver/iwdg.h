#ifndef _IWDG_H
#define _IWDG_H
#include "main.h"

void IWDG_Init(u8 prer,u16 rlr);//IWDG��ʼ��
void IWDG_Feed(void);  //ι������
void IWDG_judge(void);
#endif

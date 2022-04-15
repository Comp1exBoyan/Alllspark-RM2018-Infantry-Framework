/***************************************************************************************************************
���ƣ�can2.c

ʵ�ֹ��ܣ��Ե��̺���̨�������̲���
�޸����ڣ�20161126
���ߣ�	���Ͻ��Ŷ�
�޸ĸ�ʽ�� �޸���+�޸���Ŀ+�޸�����


***************************************************************************************************************/

#ifndef __CAN2_H__
#define __CAN2_H__

#include "yuntaiint.h"
#include "Weapon.h"
#include "can1.h"

void CAN2_Configuration(void);
void Yuntai_Cmd_ESC(int16_t output1,int16_t output2,int16_t output3,int16_t output4);
void Yuntai_Yaw_Cmd_ESC(int16_t yaw, int16_t dia_wheel);
void TIM4_Configuration(void);
void TIM4_Start(void);

float CAN2_EncoderProcess(float *filter,float filter_in ,float filter_out,float filter_parameter);


extern short Yaw_Angle_Value;  
extern short Yaw_Speed_Value;  
extern short Pitch_Angle_Value;  
extern short Pitch_Speed_Value;  
extern int Can2_receive_date_flag;
extern int16_t Weapon_Real_Speed_Value[2];
#endif 

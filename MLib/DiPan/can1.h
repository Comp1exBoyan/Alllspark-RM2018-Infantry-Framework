#ifndef __CAN1_H__
#define __CAN1_H__

#include "dipanint.h"
#include "Weapon.h"
#include "stm32f4xx.h"

void CAN1_Configuration(void);
void Underpan_Cmd_ESC(int16_t output1,int16_t output2,int16_t output3,int16_t output4);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dial_wheel_ctrl(int16_t output5);
void CAN1_RX0_IRQHandler(void);

extern float Gun_Real_Angle_Value ;//CAN1接收的波轮电机数据
extern float Gun_Real_Speed_Value ;//CAN1接收的波轮电机数据
extern float Gun_Real_Angle_Value_0_360;
extern float Gun_Real_Angle_Value_Gearbox;
extern float Gun_Target_Angle_Value;
extern float Real_Angle_Value_Flt[5][2];

extern short Real_Angle_Value[5];  //底盘3510电机的数据 及波轮电机数据
extern short Real_Speed_Value[5];  //底盘3510电机的数据 及波轮电机数据
extern long  Real_Position_Value[5];//底盘3510电机的数据 及波轮电机数据
extern int Can1_receive_date_flag;
float Can1EncoderProcess(float *filter,float filter_in ,float filter_out,float filter_parameter);
/*临时用于云台控制*/
extern float  yaw_angle;
extern float  pitch_angle;
extern float  pitch_speed;
extern float  yaw_speed;
/*-------------------*/
#endif 

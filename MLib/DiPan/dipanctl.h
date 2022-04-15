#ifndef __DIPAN_CTRL_H__
#define __DIPAN_CTRL_H__

#include "dipanint.h"
#include "mpu9250.h"
#include "can1.h"

void Angle_Dipan_Ctrl(float Expect_Yaw_turn_data,float Current_T,float Standard_T) ;
void Speed_Dipan_Ctrl(void *Parameters);
void Break_Ctl(float date1,float date2);
void DMPowerLimit(void);
void DiPan_Follow_YunTai(void);
void DFVRGypoPidInit(void);

extern float  wheel[4];
extern float  Virtual_DiPan_Angle;
extern float  TurnPidOutput;
extern double angle_expect_wheel[4];
extern float  BaiWeiDelay; 

#define  Position_control_Enable  0  //位置控制模式选择 1-位置控制 0-速度控制
#define  Follow_Gyroscope_control_Enable 0 //陀螺仪控制模式 1-陀螺仪控制模式 0-遥控开环控制模式
	
#endif 

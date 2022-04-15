#ifndef __YUNTAI_STANDARDCTRL_H__
#define __YUNTAI_STANDARDCTRL_H__

#include "mpu6500.h"
#include "can2.h"
#include "Pid_Regulator.h"

#define Reset_manually 1 //手动复位
#define Max_Angle 50

#define YT_Correction_Angle       30
#define YT_Start                  0x00 //初始状态
#define YT_Left_limit             0x01 //到达左极限
#define YT_Right_limit            0x02 //到达右极限
#define YT_Correction_inital_yaw  0x03 //到达右极限
#define YT_Left_calibration_mode  0x04 //左校准模式
#define YT_Right_calibration_mode 0x05 //左校准模式
#define YT_Correction_End         0x05 //校准完成

extern float inital_yaw;
extern float Former_Ctrl_Yaw,Former_Ctrl_Pitch;


void Yuntai_Former_Ctrl(float Current_T,float Standard_T);
void Task1_Yuntai_Ctl(float Current_T,float Standard_T);
void Task2_Yuntai_Ctl(float Current_T,float Standard_T);
int Automatic_Calibration(int mode,float Current_T,float Standard_T);

#endif 

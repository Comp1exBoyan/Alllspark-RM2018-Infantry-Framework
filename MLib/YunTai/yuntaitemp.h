#ifndef __YUNTAI_STANDARDCTRL_H__
#define __YUNTAI_STANDARDCTRL_H__

#include "mpu6500.h"
#include "can2.h"
#include "Pid_Regulator.h"

#define Reset_manually 1 //�ֶ���λ
#define Max_Angle 50

#define YT_Correction_Angle       30
#define YT_Start                  0x00 //��ʼ״̬
#define YT_Left_limit             0x01 //��������
#define YT_Right_limit            0x02 //�����Ҽ���
#define YT_Correction_inital_yaw  0x03 //�����Ҽ���
#define YT_Left_calibration_mode  0x04 //��У׼ģʽ
#define YT_Right_calibration_mode 0x05 //��У׼ģʽ
#define YT_Correction_End         0x05 //У׼���

extern float inital_yaw;
extern float Former_Ctrl_Yaw,Former_Ctrl_Pitch;


void Yuntai_Former_Ctrl(float Current_T,float Standard_T);
void Task1_Yuntai_Ctl(float Current_T,float Standard_T);
void Task2_Yuntai_Ctl(float Current_T,float Standard_T);
int Automatic_Calibration(int mode,float Current_T,float Standard_T);

#endif 

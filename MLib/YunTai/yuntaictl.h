#ifndef __YUNTAI_STANDARDCTRL_H__
#define __YUNTAI_STANDARDCTRL_H__

#include "mpu6500.h"
#include "can2.h"
#include "Pid_Regulator.h"


#define YTFollwModel  0
#define YTAuxiModel   1
#define YTBuffModel   2
#define YTBaiWeiModel 3

extern short YunTaiControlModel;
extern float Yaw_TarAngle_Value;
extern const float Yaw_Center_Angle;
extern float mpu_9250_init_angle;

extern float Yaw_Speed_PID_Output;
extern float Pit_Speed_PID_Output;

void AutoCorrect_YunTai(u8 model);
void YawBackToCenter(void);
void PitBackToCenter(void);
void YunTaiControl(float timetick);
void YunTai_Yaw_FollowDIPA_Control(float timetick);
void YunTai_Auxi_Control(float timetick);
void YunTai_Buff_Control_Move(void);
void YunTai_Buff_Control_Fixed(void);
void PitchPIDCaculate(float Pit_TarAngle);
void YawPIDCaculate(float Yaw_TarAngle);
short CorrectOneDirection(short Axis,short IntAngleValue);

#endif 

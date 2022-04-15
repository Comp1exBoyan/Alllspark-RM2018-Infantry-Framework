#ifndef _PID_REGULATOR_H_
#define _PID_REGULATOR_H_
#include "stm32f4xx.h"

#define PID_COMPONENT_NOLIMIT 60000          //��ʾPID�и�������û������
#define PID_OUTPUT_MAX_LIMIT  5500       //PID�������������

typedef struct PID_Regulator_t
{
	float tar;
	float cur;
	float err[2];
	float kp;
	float ki;
	float kd;
	float componentKp;
	float componentKi;
	float componentKd;
	float componentKpMax;
	float componentKiMax;
	float componentKdMax;
	float output;
	float outputMax;
}PID_Regulator;

void PID_Clear(PID_Regulator *pid);
void CalcPID(PID_Regulator *pid);
void PID_Calc(PID_Regulator *pid,float timetick);
void PID_SlicCalc(PID_Regulator *pid,float timetick);
#endif


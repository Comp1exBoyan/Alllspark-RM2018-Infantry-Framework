#ifndef __TASK_POWER_LIMIT_H__
#define __TASK_POWER_LIMIT_H__


#include "main.h"

typedef struct _under_PID
	{             
		float current[4];          
	} under_PID;

extern 	u16	T1;								//在decode中也有用到			
extern  under_PID pid;

float  myabs(float x);
void power_limit(void);
void underpan_stable_start(void);
void pwr_limit_proportion_compute(void);
void pwr_limit_proportion_distribution(void);
void pid_current_output_limit(void);
void speed_value_is_zero(void);

#endif 

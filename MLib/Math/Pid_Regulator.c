#include "Pid_Regulator.h"
#include "mymath.h"

void CalcPID(PID_Regulator *pid)
{
		pid->err[0] = pid->err[1];
    pid->err[1] = pid->tar-pid->cur;
    
    pid->componentKp  = pid->kp*pid->err[1];
    pid->componentKi += pid->ki*pid->err[1];
    pid->componentKd  = pid->kd*(pid->err[1]-pid->err[0]);
    
    pid->componentKp = LIMIT(pid->componentKp,-pid->componentKpMax,pid->componentKpMax);
    pid->componentKi = LIMIT(pid->componentKi,-pid->componentKiMax,pid->componentKiMax);
    pid->componentKd = LIMIT(pid->componentKd,-pid->componentKdMax,pid->componentKdMax);
    
    pid->output = pid->componentKp+pid->componentKi+pid->componentKd;
    pid->output = LIMIT(pid->output,-pid->outputMax,pid->outputMax);
}
void PID_Calc(PID_Regulator *pid,float timetick)
{
    pid->err[0] = pid->err[1];
    pid->err[1] = pid->tar-pid->cur;
    
    pid->componentKp  = pid->kp*pid->err[1];
		if(my_abs(pid->err[1])>0.4)
				pid->componentKi += pid->err[1]*timetick;
    pid->componentKd  = pid->kd*(pid->err[1]-pid->err[0])*timetick;
    
    pid->componentKp = LIMIT(pid->componentKp,-pid->componentKpMax,pid->componentKpMax);
    pid->componentKi = LIMIT(pid->componentKi,-pid->componentKiMax,pid->componentKiMax);
    pid->componentKd = LIMIT(pid->componentKd,-pid->componentKdMax,pid->componentKdMax);
    
    pid->output = pid->componentKp + pid->ki * pid->componentKi + pid->componentKd;
    pid->output = LIMIT(pid->output, -pid->outputMax, pid->outputMax);
}
void PidCalcNoI(PID_Regulator *pid,float timetick){
		pid->err[0] = pid->err[1];
    pid->err[1] = pid->tar-pid->cur;
    
    pid->componentKp  = pid->kp*pid->err[1];
    pid->componentKd  = pid->kd*(pid->err[1]-pid->err[0])*timetick;
    
    pid->componentKp = LIMIT(pid->componentKp,-pid->componentKpMax,pid->componentKpMax);
    pid->componentKd = LIMIT(pid->componentKd,-pid->componentKdMax,pid->componentKdMax);
    
    pid->output = pid->componentKp + pid->componentKd;
    pid->output = LIMIT(pid->output, -pid->outputMax, pid->outputMax);
}
void PID_Clear(PID_Regulator *pid)
{
    pid->err[0] = 0;
    pid->err[1] = 0;
    pid->componentKp = 0;
    pid->componentKi = 0;
    pid->componentKd = 0;
    pid->output = 0;
}
    
void PID_SlicCalc(PID_Regulator *pid,float timetick)
{
    PID_Calc(pid,timetick);
    //if(pid->output>0 && pid->output<500)  pid->output=pid->output+200;
    //if(pid->output<0 && pid->output>-500) pid->output=pid->output-200;
   
    if(pid->output>200  && pid->output< 800)  pid->output=pid->output+700;
    if(pid->output<-200 && pid->output>-600)  pid->output=pid->output-1000;
}

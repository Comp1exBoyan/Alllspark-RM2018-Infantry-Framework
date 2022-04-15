#include "motor.h"

void RefreshCurrValue(mMotor* motor,float angle,int speed)
{
	float tempD = angle - motor->AbsAngle;
	
	if(tempD > 340)
			tempD = tempD - 360;
	if(tempD < -340)
			tempD = tempD + 360;
	
	motor->AbsAngle = angle;
	motor->CurrSpeed = speed;
	motor->CurrAngle = motor->CurrAngle + tempD;
}

void RefreshTarValue(mMotor* motor,float angle,int speed)
{
	motor->TarAngle = angle;
	motor->TarSpeed = speed;
}	

void RefreshTarAngle(mMotor* motor,float angle)
{
	motor->TarAngle = angle;
}

void RefreshTarSpeed(mMotor* motor,float speed)
{
	motor->TarSpeed = speed;
}

float CalcOutput(mMotor* motor)
{
	if(motor->ControlModel == MotorDRingControl)
	{
		motor->AnglePid.cur = motor->CurrAngle;
		motor->AnglePid.tar = motor->TarAngle;
		
		CalcPID(&(motor->AnglePid));
		
		motor->SpeedPid.cur = motor->CurrSpeed;
		motor->SpeedPid.tar = motor->AnglePid.output;
		CalcPID(&(motor->SpeedPid));
	}
	else
	{
		motor->SpeedPid.cur = motor->CurrSpeed;
		motor->SpeedPid.tar = motor->TarSpeed;
		CalcPID(&(motor->SpeedPid));
	}	
	
	return motor->SpeedPid.output;
}

#include "Pid_Regulator.h"

#define MotorSpeedControl 0
#define MotorDRingControl 1

typedef struct mMotor_t
{
	u8    ControlModel;
	float AbsAngle;
	float CurrAngle;
	short CurrSpeed;
	float TarAngle;
	short TarSpeed;
	PID_Regulator AnglePid;
	PID_Regulator SpeedPid;
}mMotor;


void RefreshCurrValue(mMotor* motor,float angle,int speed);

void RefreshTarValue(mMotor* motor,float angle,int speed);

void RefreshTarAngle(mMotor* motor,float angle);

void RefreshTarSpeed(mMotor* motor,float speed);

float CalcOutput(mMotor* motor);

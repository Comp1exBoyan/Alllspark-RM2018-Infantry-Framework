#ifndef __MPU9250_H
#define __MPU9250_H
#include "imuint.h"

void USART6_Config(u32 bound);
void CopeSerial2Data(int ucData);
float MPU9250_EncoderProcess(float *filter,float filter_in ,float filter_out,float filter_parameter);
u8 MPU9250_calibration(u8 number);

struct SAcc
{
	double a[3];
	double Temperature;
};
struct SGyro
{
	double w[3];
	double Temperature;
};
struct SAngle
{
	double Angle[3];
	double Temperature;
};
struct SMag
{
	double h[3];
	double Temperature;
};
extern float  mpu_9250_yaw_angle,mpu_9250_pitch_angle,mpu_9250_roll_angle;
extern float  mpu_9250_yaw_acc,mpu_9250_pitch_acc,mpu_9250_roll_acc;
extern float  mpu_9250_yaw_speed,mpu_9250_pitch_speed,mpu_9250_roll_speed;
extern float  mid_mpu_9250_yaw_angle;
extern short  mpu9250_flag;
#define EN_USART6_RX					1

#endif

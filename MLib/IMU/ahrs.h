#ifndef __AHRS_H
#define __AHRS_H

#include "main.h"

#define SAMPLE_HALF_T 0.0005f     //采样周期的一半，单位：s
#define FILTER_LENGTH 3					//滑动滤波窗口长度

#define GYRO_G 	0.0610351f				//角速度变成度/S   此参数对应陀螺2000度每秒  Gyro_G=1/16.375=0.0610687
#define GYRO_GR	0.0010653f				//角速度变成弧度/S	此参数对应陀螺2000度每秒

#define IMU_KP 1.5f     					//比例
#define IMU_KI 0.0005f 						//积分

void Prepare_Data(void);
void IMUupdate(float half_T,float gx, float gy, float gz, float ax, float ay, float az,float *rol,float *pit,float *yaw);

#endif



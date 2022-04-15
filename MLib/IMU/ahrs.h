#ifndef __AHRS_H
#define __AHRS_H

#include "main.h"

#define SAMPLE_HALF_T 0.0005f     //�������ڵ�һ�룬��λ��s
#define FILTER_LENGTH 3					//�����˲����ڳ���

#define GYRO_G 	0.0610351f				//���ٶȱ�ɶ�/S   �˲�����Ӧ����2000��ÿ��  Gyro_G=1/16.375=0.0610687
#define GYRO_GR	0.0010653f				//���ٶȱ�ɻ���/S	�˲�����Ӧ����2000��ÿ��

#define IMU_KP 1.5f     					//����
#define IMU_KI 0.0005f 						//����

void Prepare_Data(void);
void IMUupdate(float half_T,float gx, float gy, float gz, float ax, float ay, float az,float *rol,float *pit,float *yaw);

#endif



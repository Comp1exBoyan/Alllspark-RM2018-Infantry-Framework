#ifndef _IMU_H_
#define	_IMU_H_

#include "stm32f4xx.h"
#include "mymath.h"
#include "math.h"
#include "mpu6500.h"
#include "filter.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_iwdg.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "stm32f4xx_i2c.h"
typedef struct 
{
  float x;
	float y;
	float z;
}trans_xyz_f_t;
typedef struct 
{
	trans_xyz_f_t err;
	trans_xyz_f_t err_tmp;
	trans_xyz_f_t err_lpf;
	trans_xyz_f_t err_Int;
	trans_xyz_f_t g;
	
}ref_t;
typedef struct{
	int32_t raw_value;   									//���������������ԭʼֵ
	int32_t last_raw_value;								//��һ�εı�����ԭʼֵ
	int32_t diff;													//���α�����֮��Ĳ�ֵ
	int32_t round_cnt;										//Ȧ��
	float ecd_angle;											//�Ƕ�
}YAWEncoder;
extern trans_xyz_f_t reference_v;
extern volatile YAWEncoder C_YAW;

void IMUupdate(float half_T,float gx, float gy, float gz, float ax, float ay, float az,float *rol,float *pit,float *yaw);
extern float Roll,Pitch,Yaw,Continuous_YAW; 

void simple_3d_trans(trans_xyz_f_t *ref, trans_xyz_f_t *in, trans_xyz_f_t *out);//С��Χ����ȷ
float YawEncoderProcess(volatile YAWEncoder *v,float date_in);

#endif


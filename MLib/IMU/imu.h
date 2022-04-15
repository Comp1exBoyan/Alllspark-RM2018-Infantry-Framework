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
	int32_t raw_value;   									//编码器不经处理的原始值
	int32_t last_raw_value;								//上一次的编码器原始值
	int32_t diff;													//两次编码器之间的差值
	int32_t round_cnt;										//圈数
	float ecd_angle;											//角度
}YAWEncoder;
extern trans_xyz_f_t reference_v;
extern volatile YAWEncoder C_YAW;

void IMUupdate(float half_T,float gx, float gy, float gz, float ax, float ay, float az,float *rol,float *pit,float *yaw);
extern float Roll,Pitch,Yaw,Continuous_YAW; 

void simple_3d_trans(trans_xyz_f_t *ref, trans_xyz_f_t *in, trans_xyz_f_t *out);//小范围内正确
float YawEncoderProcess(volatile YAWEncoder *v,float date_in);

#endif


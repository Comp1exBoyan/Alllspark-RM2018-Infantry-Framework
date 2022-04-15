#ifndef __FILTER_H__
#define __FILTER_H__

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_iwdg.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "stm32f4xx_i2c.h"
typedef struct{

	float data_in[4];					//�������
	float data_out[4];				//�������
	float den;
	float a;                  //x[n]��ϵ��
	float b;                  //y[n-1]��ϵ��
	float c;                  //y[n-2]��ϵ��
}filter_date;


float first_order_filter(float filter_in ,float filter_out, u16 filter_parameter);
float first_order_filter_dail(float *filter,float filter_in ,float filter_out, float filter_parameter);
float GetMedianNum(float * bArray, int iFilterLen, float mean_in);
float my_deathzoom(float x,float zoom);

float glide_filter(float ADNum,u8 count_number);

void Calculate_FilteringCoefficient(float Time, float Cut_Off);
float ACC_IIR_Filter( float date_in,float date_out);
int isNaN(float f);
typedef struct{
	float raw_date1;   									//δ�˲���ԭʼ����
	float raw_date2;   									//�����˲�������
	float last_raw_date;								//��һ���˲��������
	float T;
	float K1;
}filtering_;
float Complementary_filtering(volatile filtering_ *p);
#endif

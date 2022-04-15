#include "filter.h"
/*************************************************************************
                          一阶滤波滤尖峰噪声
描述：用于云台测试的 can总线接收数据的一阶滤波滤尖峰噪声
*************************************************************************/
float filter_d=0;
float filter[2];
float first_order_filter(float filter_in ,float filter_out, u16 filter_parameter)
{
	filter[0] = filter[1];
	filter[1] = filter_in;
	filter_d = filter[1]-filter[0];
	if(filter_d > filter_parameter || filter_d < -filter_parameter)	filter_d=0;
	filter_out = filter_d + filter_in ; //一阶滤波滤尖峰噪声
	return filter_out;
}
float filter_d_dail=0;
float first_order_filter_dail(float *filter,float filter_in ,float filter_out, float filter_parameter)
{
	filter[0] = filter[1];
	filter[1] = filter_in;
	filter_d_dail = filter[1]-filter[0];
	if(filter_d_dail > filter_parameter || filter_d_dail < -filter_parameter)	
		filter_d_dail=0;
	filter_out = filter_d_dail + filter_out ; //一阶滤波滤尖峰噪声
	return filter_out;
}
/*************************************************************************
                          二阶低通滤波
描述：用于云台测试的 can总线接收数据的一阶滤波滤尖峰噪声
*************************************************************************/
void Second_order_filter_Coefficient(volatile filter_date *filter_parameter,float fs,float fo)
{
 	 //fo = 50;                                //截止频率 实际值为2*fo
	 //fs = 1000;                               //采样频率(Hz)
	 float fs_square,fo_square;
	 float pi2=9.8696f;
	 float pi=3.14f;
	 fs_square=fs*fs;
	 fo_square=fo*fo;
	
	 filter_parameter->den = fs_square+6*pi*fo*fs+4*pi2*fo_square;
	 filter_parameter->a = 4*pi2*fo_square/filter_parameter->den;                 //x[n]的系数
	 filter_parameter->b = (2*fs_square+6*pi*fo*fs)/filter_parameter->den;        //y[n-1]的系数
	 filter_parameter->c = -fs_square/filter_parameter->den;                      //y[n-2]的系数
}
float Second_order_filter(volatile filter_date *fp ,float filter_in)
{
	float filter_out;
  filter_out=fp->a*filter_in+fp->b*fp->data_in[1]+fp->c*fp->data_in[2];

  fp->data_in[2]=fp->data_in[1];
  fp->data_in[1]=filter_in;
  return filter_out;
}
/*
 * 函数名：GetMedianNum
 * 描述  ：中值滤波 
 * 输入  ：iFilterLen 滤波数据总数  mean_in 待滤波参数
 * 输出  ：bArray 输出
 */ 
float GetMedianNum(float * bArray, int iFilterLen ,float mean_in) 
{  
	int i,j,ni,k; 
	float bTemp; 
	float temp[6]={0};
//数据左移
	for (ni = 0; ni < 6-1; ni ++)
	{
		bArray[ni] = bArray[ni+1];
	}
		bArray[5] = mean_in;
	for(k=0;k<6;k++)temp[k] = bArray[k];
	
//排序
  for (j = 0; j < iFilterLen - 1; j ++)
    {  
        for (i = 0; i < iFilterLen - j - 1; i ++)  
        {  
            if (temp[i] > temp[i + 1]) //将大的数据放在右边
            {  
                bTemp = temp[i];  
                temp[i] = temp[i + 1];  
                temp[i + 1] = bTemp;  
            }  
        }  
    }  
       
    if ((iFilterLen & 1) > 0)  
    {  
        bTemp = temp[(iFilterLen + 1) / 2];  
    }  
    else  
    {  
        bTemp = (temp[iFilterLen / 2] + temp[iFilterLen / 2 + 1]) / 2;  
    }  
  
    return bTemp;  
}  

/*
 * 函数名：glide_filter
 * 描述  ：滑动平均滤波
 * 输入  ：value_buf[] 滤波数据总数  mean_in 待滤波参数
 * 输出  ：bArray 输出
 */ 
float glide_filter(float ADNum,u8 count_number) 
{
  float sum=0;
	int mi,count;
	float value_buf[count_number];
	for (mi = 0; mi < count_number; mi ++)
		{
			value_buf[mi] = value_buf[mi+1];
		}
	value_buf[count_number-1] = ADNum;
  for(count=0;count<count_number;count++)
		sum+=value_buf[count];
  return (sum/count_number);
}
/******************************************************************************
函数原型：	void Calculate_FilteringCoefficient(float Time, float Cut_Off)
功    能：	iir低通滤波参数计算
*******************************************************************************/ 
static float ACC_IIR_FACTOR=0.5;
void Calculate_FilteringCoefficient(float Time, float Cut_Off)
{
	ACC_IIR_FACTOR = Time /( Time + 1/(2.0f*3.14f*Cut_Off) );
}
/******************************************************************************
函数原型：	void ACC_IIR_Filter(struct _acc *Acc_in,struct _acc *Acc_out)
功    能：	iir低通滤波
*******************************************************************************/ 
float ACC_IIR_Filter( float date_in,float date_out)
{
	date_out = date_in + ACC_IIR_FACTOR*(date_in - date_out); 
	return(date_out);									//返回状态值
}
/*
 * 函数名：isNaN
 * 描述  ：取反
 * 输入  ：f 待取反的参数
 * 输出  ：f 取反后的数据
 */ 
int isNaN(float f)
{
	
	return f!=f;
}
/**************************************************************************
函数功能：一阶互补滤波
入口参数：滤波数据指针
返回  值：无
**************************************************************************/
float Complementary_filtering(volatile filtering_ *p)
{
  p->last_raw_date = p->K1 * p->raw_date1+ (1- p->K1) * (p->last_raw_date + p->raw_date2 * p->T);
	return(p->last_raw_date);
}
/**************************************************************************
函数功能：简易卡尔曼滤波
入口参数：加速度、角速度
返回  值：无
**************************************************************************/
float angle; 	
float R_angle=0.5;// 测量噪声的协方差 既测量偏差              
char  C_0 = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] ={0,0,0,0};
float PP[2][2] = { { 1, 0 },{ 0, 1 } };
float Q_angle=0.001;// 过程噪声的协方差
float Q_gyro=0.003;//0.003 过程噪声的协方差 过程噪声的协方差为一个一行两列矩阵
float Sample_Kalman_Filter(float Accel,float Gyro,float dt)		
{
	angle+=(Gyro - Q_bias) * dt; //先验估计
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分
	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
	PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - angle;	//zk-先验估计
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //后验估计误差协方差
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	angle	+= K_0 * Angle_err;	 //后验估计
	Q_bias	+= K_1 * Angle_err;	 //后验估计
	return(angle);
}

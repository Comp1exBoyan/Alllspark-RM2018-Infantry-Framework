#include "filter.h"
/*************************************************************************
                          һ���˲��˼������
������������̨���Ե� can���߽������ݵ�һ���˲��˼������
*************************************************************************/
float filter_d=0;
float filter[2];
float first_order_filter(float filter_in ,float filter_out, u16 filter_parameter)
{
	filter[0] = filter[1];
	filter[1] = filter_in;
	filter_d = filter[1]-filter[0];
	if(filter_d > filter_parameter || filter_d < -filter_parameter)	filter_d=0;
	filter_out = filter_d + filter_in ; //һ���˲��˼������
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
	filter_out = filter_d_dail + filter_out ; //һ���˲��˼������
	return filter_out;
}
/*************************************************************************
                          ���׵�ͨ�˲�
������������̨���Ե� can���߽������ݵ�һ���˲��˼������
*************************************************************************/
void Second_order_filter_Coefficient(volatile filter_date *filter_parameter,float fs,float fo)
{
 	 //fo = 50;                                //��ֹƵ�� ʵ��ֵΪ2*fo
	 //fs = 1000;                               //����Ƶ��(Hz)
	 float fs_square,fo_square;
	 float pi2=9.8696f;
	 float pi=3.14f;
	 fs_square=fs*fs;
	 fo_square=fo*fo;
	
	 filter_parameter->den = fs_square+6*pi*fo*fs+4*pi2*fo_square;
	 filter_parameter->a = 4*pi2*fo_square/filter_parameter->den;                 //x[n]��ϵ��
	 filter_parameter->b = (2*fs_square+6*pi*fo*fs)/filter_parameter->den;        //y[n-1]��ϵ��
	 filter_parameter->c = -fs_square/filter_parameter->den;                      //y[n-2]��ϵ��
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
 * ��������GetMedianNum
 * ����  ����ֵ�˲� 
 * ����  ��iFilterLen �˲���������  mean_in ���˲�����
 * ���  ��bArray ���
 */ 
float GetMedianNum(float * bArray, int iFilterLen ,float mean_in) 
{  
	int i,j,ni,k; 
	float bTemp; 
	float temp[6]={0};
//��������
	for (ni = 0; ni < 6-1; ni ++)
	{
		bArray[ni] = bArray[ni+1];
	}
		bArray[5] = mean_in;
	for(k=0;k<6;k++)temp[k] = bArray[k];
	
//����
  for (j = 0; j < iFilterLen - 1; j ++)
    {  
        for (i = 0; i < iFilterLen - j - 1; i ++)  
        {  
            if (temp[i] > temp[i + 1]) //��������ݷ����ұ�
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
 * ��������glide_filter
 * ����  ������ƽ���˲�
 * ����  ��value_buf[] �˲���������  mean_in ���˲�����
 * ���  ��bArray ���
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
����ԭ�ͣ�	void Calculate_FilteringCoefficient(float Time, float Cut_Off)
��    �ܣ�	iir��ͨ�˲���������
*******************************************************************************/ 
static float ACC_IIR_FACTOR=0.5;
void Calculate_FilteringCoefficient(float Time, float Cut_Off)
{
	ACC_IIR_FACTOR = Time /( Time + 1/(2.0f*3.14f*Cut_Off) );
}
/******************************************************************************
����ԭ�ͣ�	void ACC_IIR_Filter(struct _acc *Acc_in,struct _acc *Acc_out)
��    �ܣ�	iir��ͨ�˲�
*******************************************************************************/ 
float ACC_IIR_Filter( float date_in,float date_out)
{
	date_out = date_in + ACC_IIR_FACTOR*(date_in - date_out); 
	return(date_out);									//����״ֵ̬
}
/*
 * ��������isNaN
 * ����  ��ȡ��
 * ����  ��f ��ȡ���Ĳ���
 * ���  ��f ȡ���������
 */ 
int isNaN(float f)
{
	
	return f!=f;
}
/**************************************************************************
�������ܣ�һ�׻����˲�
��ڲ������˲�����ָ��
����  ֵ����
**************************************************************************/
float Complementary_filtering(volatile filtering_ *p)
{
  p->last_raw_date = p->K1 * p->raw_date1+ (1- p->K1) * (p->last_raw_date + p->raw_date2 * p->T);
	return(p->last_raw_date);
}
/**************************************************************************
�������ܣ����׿������˲�
��ڲ��������ٶȡ����ٶ�
����  ֵ����
**************************************************************************/
float angle; 	
float R_angle=0.5;// ����������Э���� �Ȳ���ƫ��              
char  C_0 = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] ={0,0,0,0};
float PP[2][2] = { { 1, 0 },{ 0, 1 } };
float Q_angle=0.001;// ����������Э����
float Q_gyro=0.003;//0.003 ����������Э���� ����������Э����Ϊһ��һ�����о���
float Sample_Kalman_Filter(float Accel,float Gyro,float dt)		
{
	angle+=(Gyro - Q_bias) * dt; //�������
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-����������Э�����΢��
	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
	PP[0][1] += Pdot[1] * dt;   // =����������Э����
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - angle;	//zk-�������
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //����������Э����
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	angle	+= K_0 * Angle_err;	 //�������
	Q_bias	+= K_1 * Angle_err;	 //�������
	return(angle);
}

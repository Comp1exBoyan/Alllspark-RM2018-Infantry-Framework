#include "mpu6500.h"
#include "delay.h"
#include "mymath.h"
/*-------------------------------------
* 函数名：MPU6500_Date_Offset
* 描述  ：MPU6500数据校准
* 输入  ：校准次数
* 输出  ：无
-----------------------------------------*/ 
MPU6500_STRUCT mpu6500;
u16 acc_sum_cnt = 0,gyro_sum_cnt = 0;
s32 sum_temp[7]={0,0,0,0,0,0,0};
sensor_setup_t sensor_setup;
void MPU6500_Date_Offset(void)
{
if(mpu6500.Acc_CALIBRATE == 1)
	{
						if(my_sqrt(my_pow(mpu6500.Acc_I16.x)+my_pow(mpu6500.Acc_I16.y)+my_pow(mpu6500.Acc_I16.z)) < 2500)
						{
							sensor_setup.Offset.mpu_flag = 1;
						}
						else if(my_sqrt(my_pow(mpu6500.Acc_I16.x)+my_pow(mpu6500.Acc_I16.y)+my_pow(mpu6500.Acc_I16.z)) > 2600)
						{
							sensor_setup.Offset.mpu_flag = 0;
						}
						
        acc_sum_cnt++;
		sum_temp[A_X] += mpu6500.Acc_I16.x;
		sum_temp[A_Y] += mpu6500.Acc_I16.y;
		sum_temp[A_Z] += mpu6500.Acc_I16.z - 65536/16;   // +-8G
		sum_temp[TEM] += mpu6500.Tempreature;

    if( acc_sum_cnt >= OFFSET_AV_NUM )
		{
			mpu6500.Acc_Offset.x = sum_temp[A_X]/OFFSET_AV_NUM;
			mpu6500.Acc_Offset.y = sum_temp[A_Y]/OFFSET_AV_NUM;
			mpu6500.Acc_Offset.z = sum_temp[A_Z]/OFFSET_AV_NUM;
			mpu6500.Acc_Temprea_Offset = sum_temp[TEM]/OFFSET_AV_NUM;
			if(mpu6500.Acc_CALIBRATE == 1)
			acc_sum_cnt =0;
			mpu6500.Acc_CALIBRATE = 0;
			sum_temp[A_X] = sum_temp[A_Y] = sum_temp[A_Z] = sum_temp[TEM] = 0;
		}	
	}
if(mpu6500.Gyro_CALIBRATE == 1)
	{
		gyro_sum_cnt++;
		sum_temp[G_X] += mpu6500.Gyro_I16.x;
		sum_temp[G_Y] += mpu6500.Gyro_I16.y;
		sum_temp[G_Z] += mpu6500.Gyro_I16.z;
		sum_temp[TEM] += mpu6500.Tempreature;

    if( gyro_sum_cnt >= OFFSET_AV_NUM )
		{
			mpu6500.Gyro_Offset.x = (float)sum_temp[G_X]/OFFSET_AV_NUM;
			mpu6500.Gyro_Offset.y = (float)sum_temp[G_Y]/OFFSET_AV_NUM;
			mpu6500.Gyro_Offset.z = (float)sum_temp[G_Z]/OFFSET_AV_NUM;
			mpu6500.Gyro_Temprea_Offset = sum_temp[TEM]/OFFSET_AV_NUM;
			gyro_sum_cnt =0;
			if(mpu6500.Gyro_CALIBRATE == 1)
			mpu6500.Gyro_CALIBRATE = 0;
			sum_temp[G_X] = sum_temp[G_Y] = sum_temp[G_Z] = sum_temp[TEM] = 0;
		}
	}
}
void Transform(float itx,float ity,float itz,float *it_x,float *it_y,float *it_z)
{
	*it_x = itx;
	*it_y = ity;
	*it_z = itz;
}
/*
 * 函数名：MPU6500_ReadValue
 * 描述  ：读取MPU6500原始数据
 * 输入  ：无
 * 输出  ：无
 */ 
float mpu6500_tmp[ITEMS];
float mpu_fil_tmp[ITEMS];
uint8_t filter_cnt = 0,filter_cnt_old = 0;
s16 FILT_BUF[ITEMS][(FILTER_NUM + 1)];
float test_ang =0,test_ang_old=0,test_ang_d,test_fli_a,test_i;
void MPU6500_ReadValue(void)
{
	uint8_t i;
  uint8_t mpu_buff[14];
	float Gyro_tmp[3];
	s32 FILT_TMP[ITEMS] = {0,0,0,0,0,0,0};
	
	MPU6500_CS(0); 				//使能SPI传输

	SPI5_Read_Write_Byte(MPU6500_ACCEL_XOUT_H|0x80); //从加速度计的寄存器开始进行读取陀螺仪和加速度计的值//发送读命令+寄存器号
	
	for(i	=	0;i	<	14;i++)														//一共读取14字节的数据
	{
	mpu_buff[i]	=	SPI5_Read_Write_Byte(0xff);	//输入0xff,因为slave不识别
	}	

	mpu6500.Acc_I16.x = (((int16_t)(mpu_buff[0])<<8) | mpu_buff[1]);
    mpu6500.Acc_I16.y = (((int16_t)(mpu_buff[2])<<8) | mpu_buff[3]);
    mpu6500.Acc_I16.z = (((int16_t)(mpu_buff[4])<<8) | mpu_buff[5]);
	
    mpu6500.Gyro_I16.x = (((int16_t)(mpu_buff[8])<<8)  | mpu_buff[9]) ;
    mpu6500.Gyro_I16.y = (((int16_t)(mpu_buff[10])<<8) | mpu_buff[11]);
    mpu6500.Gyro_I16.z = (((int16_t)(mpu_buff[12])<<8) | mpu_buff[13]);
	
	MPU6500_CS(1);  	    //禁止SPI传输
	
	MPU6500_Date_Offset(); //校准
	
	Gyro_tmp[0] = mpu6500.Gyro_I16.x ;//
    Gyro_tmp[1] = mpu6500.Gyro_I16.y ;//
	Gyro_tmp[2] = mpu6500.Gyro_I16.z ;//
//======================================================================
	if( ++filter_cnt > FILTER_NUM )	
	{
		filter_cnt = 0;
		filter_cnt_old = 1;
	}
	else
	{
		filter_cnt_old = (filter_cnt == FILTER_NUM)? 0 : (filter_cnt + 1);
	}
/* 得到校准后的数据 */
	if(sensor_setup.Offset.mpu_flag == 0)
	{
		mpu6500_tmp[A_X] = (mpu6500.Acc_I16.x - mpu6500.Acc_Offset.x) ;
		mpu6500_tmp[A_Y] = (mpu6500.Acc_I16.y - mpu6500.Acc_Offset.y) ;
		mpu6500_tmp[A_Z] = (mpu6500.Acc_I16.z - mpu6500.Acc_Offset.z) ;
	}
	else
	{
		mpu6500_tmp[A_X] = 2*(mpu6500.Acc_I16.x - mpu6500.Acc_Offset.x) ;
		mpu6500_tmp[A_Y] = 2*(mpu6500.Acc_I16.y - mpu6500.Acc_Offset.y) ;
		mpu6500_tmp[A_Z] = 2*(mpu6500.Acc_I16.z - mpu6500.Acc_Offset.z - 2048) ;
	}
	
	mpu6500_tmp[G_X] = Gyro_tmp[0] - mpu6500.Gyro_Offset.x ;//
	mpu6500_tmp[G_Y] = Gyro_tmp[1] - mpu6500.Gyro_Offset.y ;//
	mpu6500_tmp[G_Z] = Gyro_tmp[2] - mpu6500.Gyro_Offset.z ;//
	
	/* 滑动滤波 */
	FILT_BUF[A_X][filter_cnt] = mpu6500_tmp[A_X];
	FILT_BUF[A_Y][filter_cnt] = mpu6500_tmp[A_Y];
	FILT_BUF[A_Z][filter_cnt] = mpu6500_tmp[A_Z];
	FILT_BUF[G_X][filter_cnt] = mpu6500_tmp[G_X]; 
	FILT_BUF[G_Y][filter_cnt] = mpu6500_tmp[G_Y];
	FILT_BUF[G_Z][filter_cnt] = mpu6500_tmp[G_Z];

	for(i=0;i<FILTER_NUM;i++)
	{
		FILT_TMP[A_X] += FILT_BUF[A_X][i];
		FILT_TMP[A_Y] += FILT_BUF[A_Y][i];
		FILT_TMP[A_Z] += FILT_BUF[A_Z][i];
		FILT_TMP[G_X] += FILT_BUF[G_X][i];
		FILT_TMP[G_Y] += FILT_BUF[G_Y][i];
		FILT_TMP[G_Z] += FILT_BUF[G_Z][i];
	}


	mpu_fil_tmp[A_X] = (float)( FILT_TMP[A_X] )/(float)FILTER_NUM;
	mpu_fil_tmp[A_Y] = (float)( FILT_TMP[A_Y] )/(float)FILTER_NUM;
	mpu_fil_tmp[A_Z] = (float)( FILT_TMP[A_Z] )/(float)FILTER_NUM;


	mpu_fil_tmp[G_X] = (float)( FILT_TMP[G_X] )/(float)FILTER_NUM;
	mpu_fil_tmp[G_Y] = (float)( FILT_TMP[G_Y] )/(float)FILTER_NUM;
	mpu_fil_tmp[G_Z] = (float)( FILT_TMP[G_Z] )/(float)FILTER_NUM;
	
	
	/*坐标转换*/
	Transform(mpu_fil_tmp[A_X],mpu_fil_tmp[A_Y],mpu_fil_tmp[A_Z],&mpu6500.Acc.x,&mpu6500.Acc.y,&mpu6500.Acc.z);
	Transform(mpu_fil_tmp[G_X],mpu_fil_tmp[G_Y],mpu_fil_tmp[G_Z],&mpu6500.Gyro.x,&mpu6500.Gyro.y,&mpu6500.Gyro.z);

	mpu6500.Gyro_deg.x = mpu6500.Gyro.x *TO_ANGLE;
	mpu6500.Gyro_deg.y = mpu6500.Gyro.y *TO_ANGLE;
	mpu6500.Gyro_deg.z = mpu6500.Gyro.z *TO_ANGLE;
	
}

/*
 * 函数名：MPU6500_ReadValue
 * 描述  ：读取MPU6500原始数据
 * 输入  ：无
 * 输出  ：无
 */ 
uint8_t MPU_id = 0;
uint8_t MPU6500_Init(void)
{
	uint8_t index = 0;
	SPI5_Init();
  uint8_t MPU6500_Init_Data[10][2] = 
  {
    {MPU6500_PWR_MGMT_1,    0x80},      // Reset Device
    {MPU6500_PWR_MGMT_1,    0x03},      // Clock Source - Gyro-Z
    {MPU6500_PWR_MGMT_2,    0x00},      // Enable Acc & Gyro
    {MPU6500_CONFIG,        0x02},      // LPF 98Hz
    {MPU6500_GYRO_CONFIG,   0x18},      // +-2000dps
    {MPU6500_ACCEL_CONFIG,  0x10},      // +-8G
    {MPU6500_ACCEL_CONFIG_2,0x02},      // enable LowPassFilter  Set Acc LPF
    {MPU6500_USER_CTRL,     0x20},      // Enable AUX
  };
  
  delay_ms(100);
  MPU_id = MPU6500_Read_Reg(MPU6500_WHO_AM_I);  //read id of device,check if MPU6500 or not
  
  for(index = 0; index < 10; index++)
  {
    MPU6500_Write_Reg(MPU6500_Init_Data[index][0], MPU6500_Init_Data[index][1]);
    delay_ms(1);
  }

  return 0;
}

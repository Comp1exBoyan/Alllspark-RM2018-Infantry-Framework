#include "imuint.h"
#include "dipanctl.h"

/*-------------------------------------
* 函数名：IMU初始化函数
* 描述  ：对MPU6500 9250 IST8310进行初始化
* 输入  ：无
* 输出  ：无
-----------------------------------------*/ 
u8 IMU_int(void)
{
	uint8_t status;
	//mpu9250初始化
	USART6_Config(115200);							//用于mpu9250数据传输
	while( !MPU9250_calibration(20) ); //电机位置初始化后才能进行MPU9250校准
	//
    while(!mpu9250_flag){;}
	mpu6500.Acc_CALIBRATE = 1;
	mpu6500.Gyro_CALIBRATE = 1;
	
	MPU6500_Init();
	
	Mag_CALIBRATED=1;
	IST8310_Init();
    DFVRGypoPidInit();
    status=1;
	return(status);											//返回状态值
}

#include "imuint.h"
#include "dipanctl.h"

/*-------------------------------------
* ��������IMU��ʼ������
* ����  ����MPU6500 9250 IST8310���г�ʼ��
* ����  ����
* ���  ����
-----------------------------------------*/ 
u8 IMU_int(void)
{
	uint8_t status;
	//mpu9250��ʼ��
	USART6_Config(115200);							//����mpu9250���ݴ���
	while( !MPU9250_calibration(20) ); //���λ�ó�ʼ������ܽ���MPU9250У׼
	//
    while(!mpu9250_flag){;}
	mpu6500.Acc_CALIBRATE = 1;
	mpu6500.Gyro_CALIBRATE = 1;
	
	MPU6500_Init();
	
	Mag_CALIBRATED=1;
	IST8310_Init();
    DFVRGypoPidInit();
    status=1;
	return(status);											//����״ֵ̬
}

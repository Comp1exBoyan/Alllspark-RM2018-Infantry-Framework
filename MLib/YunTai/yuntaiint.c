#include "yuntaiint.h"
/*-------------------------------------
* ��������IMU��ʼ������
* ����  ����MPU6500 9250 IST8310���г�ʼ��
* ����  ����
* ���  ����
-----------------------------------------*/ 
short YawCenterValue=0;
short PitCenterValue=0;
float FollowYawAngle=0.0f;
u8      YawInitFlag=0;
u8 yuntai_int(void)
{

    YawInitFlag=0;
    //TIM4_Configuration();
		CAN2_Configuration();							//CAN1��ʼ�� ������̨����
    while(!Can2_receive_date_flag){;}
    
        
    YawInitFlag=1;
        
	return(YawInitFlag);											//����״ֵ̬
}

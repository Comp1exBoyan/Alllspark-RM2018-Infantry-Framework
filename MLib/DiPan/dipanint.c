#include "dipanint.h"
/*-------------------------------------
* ��������IMU��ʼ������
* ����  ����MPU6500 9250 IST8310���г�ʼ��
* ����  ����
* ���  ����
-----------------------------------------*/ 
u8 dipan_int(void)
{
	uint8_t status;
	
	CAN1_Configuration();							//CAN1��ʼ�� ���ڵ��̼����ֵ���Ŀ���
	while(!Can1_receive_date_flag){;}
    status=1;
	return(status);											//����״ֵ̬
}

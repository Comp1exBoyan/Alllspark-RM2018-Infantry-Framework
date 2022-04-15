#include "dipanint.h"
/*-------------------------------------
* 函数名：IMU初始化函数
* 描述  ：对MPU6500 9250 IST8310进行初始化
* 输入  ：无
* 输出  ：无
-----------------------------------------*/ 
u8 dipan_int(void)
{
	uint8_t status;
	
	CAN1_Configuration();							//CAN1初始化 用于底盘及波轮电机的控制
	while(!Can1_receive_date_flag){;}
    status=1;
	return(status);											//返回状态值
}

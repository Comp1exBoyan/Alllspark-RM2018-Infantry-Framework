#include "yuntaiint.h"
/*-------------------------------------
* 函数名：IMU初始化函数
* 描述  ：对MPU6500 9250 IST8310进行初始化
* 输入  ：无
* 输出  ：无
-----------------------------------------*/ 
short YawCenterValue=0;
short PitCenterValue=0;
float FollowYawAngle=0.0f;
u8      YawInitFlag=0;
u8 yuntai_int(void)
{

    YawInitFlag=0;
    //TIM4_Configuration();
		CAN2_Configuration();							//CAN1初始化 用于云台控制
    while(!Can2_receive_date_flag){;}
    
        
    YawInitFlag=1;
        
	return(YawInitFlag);											//返回状态值
}

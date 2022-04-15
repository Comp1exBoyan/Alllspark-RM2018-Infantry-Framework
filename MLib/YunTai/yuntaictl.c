#include "SBUS.h"
#include "mpu6500.h"
#include "yuntaiint.h"
#include "yuntaictl.h"
#include "Weapon.h"
#include "delay.h"
#include "usart3.h"
#include "JudgeSystemN.h"

#define YAW_CORRECT  0
#define PIC_CORRECT  1
#define AUTO_CORRECT 2
#define HAND_CORRECT 3

const float Yaw_Center_Angle = 150.0f;
float mpu_9250_init_angle = 0;
float yaw_speed_filter[6]={0};

short YunTaiControlModel = YTFollwModel;

short BuffRebackDelay=0;
float YawLimitOffset=61.5f;
short PitDoLimitOffset=16;
short PitUpLimitOffset=25;

float YawLfLimit=0;
float YawRiLimit=0;
float PitUpLimit=0;
float PitDoLimit=0;

float Pit_AddAngle_Value;
float Yaw_TarAngle_Value;
float Pit_TarAngle_Value;

float Pit_TarAngle_YKValue=0;
float BuffYawCenterValue = 0;

float Yaw_Speed_PID_Output = 0;
float Pit_Speed_PID_Output = 0;

short ArriveFlag = 0;
float tempAuxiYawTarValue = 0.0f;
float tempAuxiPitTarValue = 0.0f;

//----------------------------------------------

PID_Regulator YTYawAnglePID = YUNTAI_YAW_ANGLE_PID_DEFAULT;
PID_Regulator YTYawSpeedPID = YUNTAI_YAW_SPEED_PID_DEFAULT;

PID_Regulator YTFollowDPAngelPID = YUNTAI_YAW_ANGLE_PID_FOLLOWDP;
PID_Regulator YTFollowDPSpeedPID = YUNTAI_YAW_SPEED_PID_FOLLOWDP;

PID_Regulator YTBaiWeiAnglePID = YUNTAI_YAW_ANGLE_PID_BAIWEI;

PID_Regulator YTPitAnglePID = YUNTAI_PITCH_ANGLE_PID_DEFAULT;
PID_Regulator YTPitSpeedPID = YUNTAI_PITCH_SPEED_PID_DEFAULT;

void YunTaiControl(float timetick)
{
		/*c或v高电平触发buff*/
    if(UartBuffFlag == 1)
    {
        MCLSwitch = 1;              //buff模式开摩擦轮
        FireModel = OneClickModel;  //buff模式点发模式
        
				YunTai_Buff_Control_Fixed();
    }
    
		PitchPIDCaculate(2*radio_pitch_control_data);
		YawPIDCaculate(radio_turn_control_data);
}


void PitchPIDCaculate(float Pit_TarAngle)
{
			static char div = 15;
			div++;
			/*	pitch控制数据获得	*/
			Pit_TarAngle_Value = Pit_TarAngle;
			/*	pitch限位	*/
			Pit_TarAngle_Value=Pit_TarAngle_Value> 40? 40:Pit_TarAngle_Value;
			Pit_TarAngle_Value=Pit_TarAngle_Value<-40?-40:Pit_TarAngle_Value;
			if(div==14)
			{
				if(UartAuxiFlag)
				{
					static float min_out = 0;
					if(my_abs(YTPitAnglePID.err[1])<1.5f && my_abs(YTPitAnglePID.err[1])>0.1f)
					{	min_out = 102.8*my_abs(YTPitAnglePID.err[1])+85.7;
						YTPitAnglePID.kp = min_out/my_abs(YTPitAnglePID.err[1]);}//-20 * my_abs(YTPitAnglePID.err[1]) + 200;
					else if(my_abs(YTPitAnglePID.err[1])<0.1f)
					{YTPitAnglePID.kp = 180;}
				}
				if(my_abs(YTPitAnglePID.err[1])>1.5f)
					{YTPitAnglePID.kp = 100;}
				/*	pitch 位置环 PID 运算	*/
				YTPitAnglePID.tar=Pit_TarAngle_Value;
				YTPitAnglePID.cur=mpu_9250_pitch_angle;
				PID_Calc(&YTPitAnglePID,1);
				div = 0;
			}
			if(my_abs(YTPitAnglePID.err[1]<0.3)) YTPitSpeedPID.componentKi = 0;
			/*	pitch 速度环 PID 运算	*/
			YTPitSpeedPID.tar=YTPitAnglePID.output;
			YTPitSpeedPID.cur=mpu_9250_pitch_speed;
			PID_Calc(&YTPitSpeedPID,1);
			
			Pit_Speed_PID_Output = YTPitSpeedPID.output;
}

void YawPIDCaculate(float Yaw_TarAngle)
{
			// 云台控制
			static char div = 9;
			div++;
			Yaw_TarAngle_Value = Yaw_TarAngle;
			if(div==10)
			{
					/*	yaw 	位置环 PID计算	*/
					YTFollowDPAngelPID.tar = Yaw_TarAngle_Value;
					YTFollowDPAngelPID.cur = mid_mpu_9250_yaw_angle - mpu_9250_init_angle;   
					PID_Calc(&YTFollowDPAngelPID,1);
					div = 0;
			}
			/*	yaw 	速度环 PID计算	*/

			YTFollowDPSpeedPID.tar = YTFollowDPAngelPID.output;
			YTFollowDPSpeedPID.cur = mpu_9250_yaw_speed; 
			PID_Calc(&YTFollowDPSpeedPID,1);
			
			Yaw_Speed_PID_Output = -LIMIT(YTFollowDPSpeedPID.output,-5000,5000);
			//Yaw_Speed_PID_Output = GetMedianNum(yaw_speed_filter, 6,YTFollowDPSpeedPID.output);//中值滤波
			//Yaw_Speed_PID_Output = glide_filter(YTFollowDPSpeedPID.output,6);
}

u8 FireNum = 0;

void YunTai_Buff_Control_Move(void)		// 摄像头移动
{
    static float DelayBuffSeconds = 0.0f;
    static u8  	 shoot_one = 0;

		// 第一步 转动云台到目标位置，并发弹
    if(RecieveBuffflag == 1 && ArriveFlag == 0)		
    {    
				if(my_abs(YTFollowDPSpeedPID.err[1]) < 0.5f && my_abs(YTPitAnglePID.err[1]) < 0.5f )
				{
					ArriveFlag = 1;
					RecieveBuffflag = 0;
					BLForwardTurn();
					DelayBuffSeconds = secTime;							// 记录波轮开始转动的时间
					shoot_one = 0;
				}
    }
		// 第二步 判断是否已经发弹，若是则云台回到初始位置
    if(ArriveFlag == 1)
    {
				if(have_shoot == 1 || shoot_one == 1)			
				{		
						have_shoot = 0;
						shoot_one = 1;
						radio_turn_control_data -= Buff_yaw_store;
						radio_pitch_control_data -= Buff_pitch_store;
						Buff_yaw_store = 0;
						Buff_pitch_store = 0;
					
						if(my_abs(YTFollowDPSpeedPID.err[1]) < 0.5f && my_abs(YTPitAnglePID.err[1]) < 0.5f )
						{
							ArriveFlag = 0;
							SendToPc(SENDPOS);
							DelayBuffSeconds = secTime;
						}
				}
				else if((secTime - DelayBuffSeconds)>0.4f) // 若0.4s内未发弹则继续转动波轮
				{
						BLForwardTurn();
						DelayBuffSeconds = secTime;						// 记录波轮开始转动的时间
				}
    }     
}

void YunTai_Buff_Control_Fixed(void)		//摄像头不动
{
		static float DelayBuffSeconds = 0.0f;

		// 第一步 转动云台到目标位置，并发弹
    if(RecieveBuffflag == 1 && ArriveFlag == 0)		
    {
				radio_turn_control_data = buff_init_angle_yaw + Buff_yaw;
				radio_pitch_control_data = buff_init_angle_pit + Buff_pitch;

				if(my_abs(YTFollowDPSpeedPID.err[1]) < 0.5f && my_abs(YTPitAnglePID.err[1]) < 0.5f )
				{
						ArriveFlag = 1;
						RecieveBuffflag = 0;
						BLForwardTurn();
						DelayBuffSeconds = secTime;							// 记录波轮开始转动的时间
				}
    }
		// 第二步 判断是否已经发弹，若是则回到第一步
    if(ArriveFlag == 1)
    {
				if(have_shoot == 1)			
				{		
						have_shoot = 0;
						ArriveFlag = 0;
				}
				else if((secTime - DelayBuffSeconds)>0.4f)// 若0.4s内未发弹则继续转动波轮
				{
						BLForwardTurn();
						DelayBuffSeconds = secTime;						// 记录波轮开始转动的时间
				}
    }    
}

//云台YAW PITCH轴校准中心值
void AutoCorrect_YunTai(u8 model)
{  
    if(model == AUTO_CORRECT)
    {
        YawLfLimit=Yaw_Center_Angle+YawLimitOffset;
        YawRiLimit=Yaw_Center_Angle-YawLimitOffset;
        YawCenterValue=Yaw_Center_Angle;
        YawBackToCenter();
    }
    else if(model == HAND_CORRECT)
    {
        delay_ms(5);
        YawCenterValue = mid_mpu_9250_yaw_angle;
        YawLfLimit = YawCenterValue+YawLimitOffset;
        YawRiLimit = YawCenterValue-YawLimitOffset;
        
        delay_ms(5);
        PitCenterValue = 0;
        PitDoLimit = PitCenterValue-PitDoLimitOffset;
        PitUpLimit = PitCenterValue+PitUpLimitOffset;
    }
    Yaw_TarAngle_Value = YawCenterValue;
    Pit_TarAngle_Value = PitCenterValue;
    FollowYawAngle = YawCenterValue;
}
short CorrectOneDirection(short Axis,short IntAngleValue)
{
    //IntAngleValue为进行校准时电机的初始角度值
    short LastDiffValue=0;   // 上一时刻电机角度同初始角度的差值
    short CurrDiffValue=1;   // 当前时刻电机角度同初始角度的差值
    short DelayCheckCnt=0;
    int   UmoveCheckCnt=0;
    short RetAngleValue=0;
    short IsInCorrect  =0;
    while(DelayCheckCnt<400 && UmoveCheckCnt<800)
    {
        //YAW轴校准
        if(Axis == YAW_CORRECT)
        {
            Yuntai_Cmd_ESC(0,2000+UmoveCheckCnt,0,0); //使YAW向左旋转直到达到限位
            delay_ms(2);
            if(my_abs(Yaw_Speed_Value)>300)                  IsInCorrect =1;   //检测YAW轴在移动时 认为已经在校准
            if(CurrDiffValue==LastDiffValue)                 UmoveCheckCnt++;  //YAW轴未移动，可能是由于阻力较大 
            if(CurrDiffValue==LastDiffValue &&  IsInCorrect) DelayCheckCnt++;  //设置计数
            
            LastDiffValue=CurrDiffValue;
            CurrDiffValue=Yaw_Angle_Value-IntAngleValue;
            RetAngleValue=mid_mpu_9250_yaw_angle;
        }
        //PITCH轴校准
        else if(Axis == PIC_CORRECT)
        {
            return 0;           
        }
    }
    return RetAngleValue;
}
void YawBackToCenter(void)
{
        //获取当前陀螺仪YAW角度值，不断逼近中心角度值
        while(abs(Yaw_Angle_Value-YawCenterValue)>0.5 )
        {
            YTYawAnglePID.cur=Yaw_Angle_Value;
            YTYawAnglePID.tar=YawCenterValue;
            
            CalcPID(&YTYawAnglePID);
						float tempYawOutput = LIMIT(YTYawAnglePID.output,-5000,5000);
            Yuntai_Yaw_Cmd_ESC(-tempYawOutput,0);
            delay_ms(5);
        }
				mpu_9250_init_angle = mid_mpu_9250_yaw_angle;
        delay_ms(10);
}
void PitBackToCenter(void)
{
        //陀螺仪Pitch角度作为输入 目标值为水平0            
        short   UnMoveCnt=0;
        while(my_abs(mpu_9250_pitch_angle-0)>2 || my_abs(Pitch_Speed_Value)>200)
        {
						
            YTPitAnglePID.cur=mpu_9250_pitch_angle;
            YTPitAnglePID.tar=0;
            
            PID_Calc(&YTPitAnglePID,1);
						PitchPIDCaculate(0);
            Yuntai_Cmd_ESC(YTPitAnglePID.output,0,0,0);
            delay_ms(3);
            
            if(my_abs(Pitch_Speed_Value)<200)   UnMoveCnt++;
            if(UnMoveCnt ==400) break;
        }
        delay_ms(10);
}

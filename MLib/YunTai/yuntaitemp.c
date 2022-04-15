#include "SBUS.h"
#include "mpu6500.h"
#include "yuntaiint.h"
#include "yuntaictl.h"

//----------------------------------------------
PID_Regulator YTYawAnglePID = YUNTAI_YAW_ANGLE_PID_DEFAULT;
PID_Regulator YTYawSpeedPID = YUNTAI_YAW_SPEED_PID_DEFAULT;

PID_Regulator YTPitchAnglePID = YUNTAI_PITCH_ANGLE_PID_DEFAULT;
PID_Regulator YTPitchSpeedPID = YUNTAI_PITCH_SPEED_PID_DEFAULT;

static int Yuntai_init=0;

float Former_Ctrl_Yaw,Former_Ctrl_Pitch;
void Yuntai_Former_Ctrl(float Current_T,float Standard_T)
{
    static float last_mpu_9250_yaw_angle,last_yaw_err;
	static int Yuntai_Former_int_flag=0,fixed_point_flag=0;
	float Err_Weight;
	
	//期望角度初始化
	if(radio_yaw==0&&Yuntai_Former_int_flag==0&&Yuntai_init)
	{
        Former_Ctrl_Yaw += 100 *3.14 *Current_T*( mid_mpu_9250_yaw_angle - Former_Ctrl_Yaw );
        if(abs( mid_mpu_9250_yaw_angle - Former_Ctrl_Yaw )<0.01f) Yuntai_Former_int_flag=1;
	}
	if(Yuntai_init==0)
	{
        Former_Ctrl_Yaw += 100 *3.14 *Current_T*( mid_mpu_9250_yaw_angle - Former_Ctrl_Yaw );
	}
	//目标角度 软件限位，防止目标角度转的太快，云台无法跟上
	Err_Weight=Yuntai_init*LIMIT(abs(Yaw_Angle_Value-inital_yaw),0,Max_Angle)/Max_Angle;
	Former_Ctrl_Yaw=Former_Ctrl_Yaw + ((0.35f+0.65f*(1-Err_Weight)) *radio_yaw )*Current_T;
	if(radio_yaw==0&&fixed_point_flag==0) 
	{
		Former_Ctrl_Yaw=mid_mpu_9250_yaw_angle;
		fixed_point_flag=1;
	}
	if(radio_yaw>100||radio_yaw<-100)	fixed_point_flag=0;
	//越界保护
	last_yaw_err=Former_Ctrl_Yaw-last_mpu_9250_yaw_angle;
	if(abs(last_mpu_9250_yaw_angle-mid_mpu_9250_yaw_angle)>10000) Former_Ctrl_Yaw=last_yaw_err;
	last_mpu_9250_yaw_angle=mid_mpu_9250_yaw_angle;
	Former_Ctrl_Pitch =LIMIT(radio_pitch*0.045f,-50,50);
} 
float Task1_output_yaw;		  //yaw电流输出 经过pid计算之后
float Task1_output_pitch;		//pitch 电流输出 经过pid计算之后
float inital_yaw=0;
void Task1_Yuntai_Ctl(float Current_T,float Standard_T)
{
	static float Task1_Expect_Yaw_Angle,Compensate_Angle[2],Compensate_Angle_Value;
	if(Yuntai_init==0)				//云台位置初始化（回正）
	{
		if(Reset_manually) //手动复位
		{
			inital_yaw   =Yaw_Angle_Value;
			Yuntai_init=1;
		}
		else//自动复位
		{
             =Automatic_Calibration(YT_Right_calibration_mode,Current_T,Standard_T);
		}
	}
	else										//云台位置初始化完成
	{
		Compensate_Angle[0]=Compensate_Angle[1];
	    Compensate_Angle[1]=Former_Ctrl_Yaw-(mid_mpu_9250_yaw_angle-(Yaw_Angle_Value-inital_yaw));
		Compensate_Angle_Value=12.0f*(Compensate_Angle[1]-Compensate_Angle[0]);
		Task1_Expect_Yaw_Angle=(Yaw_Angle_Value-inital_yaw)
		                       +1.0f*(Former_Ctrl_Yaw - mid_mpu_9250_yaw_angle) 
		                       +Compensate_Angle_Value;
		//角度限位  软件保护
		if(Task1_Expect_Yaw_Angle>Max_Angle)  Task1_Expect_Yaw_Angle =Max_Angle-inital_yaw;
		if(Task1_Expect_Yaw_Angle<-Max_Angle) Task1_Expect_Yaw_Angle=-Max_Angle-inital_yaw;
        
        YTYawAnglePID.tar = Task1_Expect_Yaw_Angle;
        YTYawAnglePID.cur = Yaw_Angle_Value-inital_yaw;
        
        YTPitchAnglePID.tar = Former_Ctrl_Pitch;
        YTPitchAnglePID.cur = mpu_9250_pitch_angle;
        
        PID_Calc(&YTYawAnglePID,Current_T/Standard_T);
        PID_Calc(&YTPitchAnglePID,Current_T/Standard_T);
		//yaw数据
//		inital_yaw_position_pid_set(Task1_Expect_Yaw_Angle , (Yaw_Angle_Value-inital_yaw));
//	    Task1_output_yaw = Driver_PID(&yaw_position_pid,Current_T,Standard_T,0);
//		//pitch数据	
//		inital_pitch_position_pid_set(Former_Ctrl_Pitch ,mpu_9250_pitch_angle);
//		Task1_output_pitch = Driver_PID(&pitch_position_pid,Current_T,Standard_T,0);
	}
}
void Task2_Yuntai_Ctl(float Current_T,float Standard_T)
{
	
	float output_protection=0.0f;
	/*------------------------------------角速度环控制量计算----------------------------------*/
	//yaw数据
    YTYawSpeedPID.tar = YTYawAnglePID.output;
    YTYawSpeedPID.cur = Yaw_Speed_Value;
    
    //pitch数据
    YTPitchSpeedPID.tar = YTPitchAnglePID.output;
    YTPitchSpeedPID.cur = Pitch_Speed_Value;
    
    PID_Calc(&YTYawSpeedPID,Current_T/Standard_T);
    PID_Calc(&YTPitchSpeedPID,Current_T/Standard_T);
    
	/*---------------------------------------走直PID控制计算-----------------------------------*/
	if(Yuntai_init)Yuntai_Cmd_ESC(output_protection*YTPitchSpeedPID.output,
		                               output_protection*YTYawSpeedPID.output,output_protection*MC_PID_Output1,output_protection*MC_PID_Output2);
}
int Automatic_Calibration(int mode,float Current_T,float Standard_T)
{
	static u8 YT_Int_Flag=YT_Start,calibration_mode=YT_Right_calibration_mode;
	static float calibration_angle;
	calibration_mode=mode;
	switch(YT_Int_Flag)
	{
		case YT_Start:   //启动过程
		{
			 if(calibration_mode==YT_Right_calibration_mode)
			 {
				 calibration_angle=calibration_angle+0.1f;
				 Yuntai_Cmd_ESC(0,3000,0,0);
			 }
			 else
			 {
				 calibration_angle=calibration_angle-0.1f;
				 Yuntai_Cmd_ESC(0,-3000,0,0);
			 }
			 if(abs(calibration_angle-(Yaw_Angle_Value-inital_yaw))>=200)
			 {
				inital_yaw   =Yaw_Angle_Value;
				PID_Reset(&YTYawAnglePID);
				YT_Int_Flag=YT_Right_limit;//到达右极限
			 }		
		}break;
		case YT_Right_limit:   //归位 矫正inital_yaw
		{
             YTYawAnglePID.tar = inital_yaw-YT_Correction_Angle;
             YTYawAnglePID.cur = Yaw_Angle_Value-inital_yaw;          
             PID_Calc(&YTYawAnglePID,Current_T/Standard_T);
            
			 Yuntai_Cmd_ESC(0,Task1_output_yaw,0,0);
			 if(abs(2*inital_yaw-YT_Correction_Angle-Yaw_Angle_Value)<0.1f)
			 {
			 inital_yaw   =Yaw_Angle_Value;
			 PID_Reset(&YTYawAnglePID);
			 YT_Int_Flag=YT_Correction_inital_yaw;
			 }
		}break;
		case YT_Left_limit:   //归位 矫正inital_yaw
		{
             YTYawAnglePID.tar = inital_yaw+YT_Correction_Angle;
             YTYawAnglePID.cur = Yaw_Angle_Value-inital_yaw;          
             PID_Calc(&YTYawAnglePID,Current_T/Standard_T);
			 
			 Yuntai_Cmd_ESC(0,Task1_output_yaw,0,0);
			 if(abs(2*inital_yaw-YT_Correction_Angle-Yaw_Angle_Value)<0.1f)
			 {
			 inital_yaw   =Yaw_Angle_Value;
			 PID_Reset(&YTYawAnglePID);
			 YT_Int_Flag=YT_Correction_inital_yaw;
			 }
		}break;
		case YT_Correction_inital_yaw:   //
		{
			 YT_Int_Flag=YT_Correction_End;
		}
		}
		if(YT_Int_Flag==YT_Correction_inital_yaw)return 1;
		else return 0;
}

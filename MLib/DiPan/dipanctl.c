#include "mymath.h"
#include "dipanint.h"
#include "dipanctl.h"
#include "yuntaiint.h"
#include "JudgeSystemN.h"
#include "usart3.h"
/*===================================================================================
������  �����̿��ƺ���
������������
����    ���ص��̵�� 
�����ĸ��˵����
ǰ���������ӳɡ�����֡� ���֣�������������ӳɡ� �ڰ���" ����
˳ʱ��Ϊ��������
��ǰ�������Ϊ��0
��ǰ�������Ϊ��1
�Һ��������Ϊ��2
����������Ϊ��3
����    ��/
����    ��������
===================================================================================*/

/*--------------------------����4�ֿ���PID����-------------------------------*/

//radio_ahead_back_data  1000<-ǰ��--0--����-->-1000
//radio_left_right_data  1000<-����--0--����-->-1000
//radio_turn_data        1000<-��ת--0--��ת-->-1000
//-------------------------------------------�⻷����--------------------------------------------------------

//���̸�����̨����PID��ʼ��
PID_Regulator DFYAnglePID = DIPAN_VIRTUAL_ANGLE_PID_DEFAULT;
PID_Regulator DFVRGypoPID = DIPAN_VIRTUAL_GYPO_PID_DEFAULT;
//�����ٶȻ�����PID��ʼ��
PID_Regulator DM1SpeedPID = DIPAN_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator DM2SpeedPID = DIPAN_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator DM3SpeedPID = DIPAN_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator DM4SpeedPID = DIPAN_MOTOR_SPEED_PID_DEFAULT;

double angle_expect_wheel[4]={0.0};
double las_expect_wheel[4]={0.0};
float Yaw_turn_data;
float yaw_filter_positon_mean[6]={0};
float ExpectLimit = 8000;
//-------------------------------------------�ٶȿ���-------------------------------------------------
float wheel[4]={0.0};
float DP_Virtual_Yaw=0.0f;
float DMPidOutPut[4] = {0};
float FinalDMPidOut[4] = {0};
float TurnPidOutput;
float PidOutputSum=0;
float buffMoveDelay = 0.0f;
float BaiWeiDelay = 0.0f;
int	cov_flag = 0;
/*-------------------filter------------------*/
float FinalDMPidOutFliter[4][6] = {0}; 

void DFVRGypoPidInit(void)
{
    DFVRGypoPID.tar = mid_mpu_9250_yaw_angle;
}

void Speed_Dipan_Ctrl(void *Parameters)
{  	
	
	portTickType CurrentControlTick = 0;                //��ǰϵͳʱ��
	
	for(;;)
    {
        CurrentControlTick = xTaskGetTickCount();
			
				IWDG_Feed();
				secTime += 0.005f;

				if(YunTaiControlModel == YTFollwModel)
				{
					if(cov_flag == 0)
					 BaiWeiDelay += 0.01f;
				  if(cov_flag == 1)
					 BaiWeiDelay -= 0.01f;
				  if(BaiWeiDelay >= 8)
					  cov_flag = 1;
				  if(BaiWeiDelay <= -8)
					  cov_flag = 0;
								if(decode_the_key.key_z)
						{
							//if(attack_direction) FollowYawAngle = YawCenterValue + 60;
							//else FollowYawAngle = YawCenterValue - 60;
							if(BaiWeiDelay>0) FollowYawAngle = YawCenterValue + 60;
							else FollowYawAngle = YawCenterValue - 60;
						}
						else FollowYawAngle = YawCenterValue;
						DFYAnglePID.tar = FollowYawAngle;
						DFYAnglePID.cur = Yaw_Angle_Value;
						PID_Calc(&DFYAnglePID,1);

						TurnPidOutput = -DFYAnglePID.output+radio_yaw;
				}

			angle_expect_wheel[0] =(+radio_ahead_back_data - radio_left_right_data - TurnPidOutput)*10.0f;
			angle_expect_wheel[1] =(-radio_ahead_back_data - radio_left_right_data - TurnPidOutput)*10.0f;
			angle_expect_wheel[2] =(+radio_ahead_back_data + radio_left_right_data - TurnPidOutput)*10.0f;
			angle_expect_wheel[3] =(-radio_ahead_back_data + radio_left_right_data - TurnPidOutput)*10.0f;
				
			angle_expect_wheel[0] = LIMIT(angle_expect_wheel[0],-ExpectLimit,ExpectLimit);
			angle_expect_wheel[1] = LIMIT(angle_expect_wheel[1],-ExpectLimit,ExpectLimit);
			angle_expect_wheel[2] = LIMIT(angle_expect_wheel[2],-ExpectLimit,ExpectLimit);
			angle_expect_wheel[3] = LIMIT(angle_expect_wheel[3],-ExpectLimit,ExpectLimit);
			
			DM1SpeedPID.tar = angle_expect_wheel[0];
			DM2SpeedPID.tar = angle_expect_wheel[1];
			DM3SpeedPID.tar = angle_expect_wheel[2];
			DM4SpeedPID.tar = angle_expect_wheel[3];
		 
		
			/*------------------------------------����PID������----------------------------------------------------*/
			float  Rate_T;
				Rate_T=1; //Current_T/Standard_T;  
			DM1SpeedPID.cur=Real_Speed_Value[0];
			DM2SpeedPID.cur=Real_Speed_Value[1];
			DM3SpeedPID.cur=Real_Speed_Value[2];
			DM4SpeedPID.cur=Real_Speed_Value[3];
			
			PID_Calc(&DM1SpeedPID,Rate_T);
			PID_Calc(&DM2SpeedPID,Rate_T);
			PID_Calc(&DM3SpeedPID,Rate_T);
			PID_Calc(&DM4SpeedPID,Rate_T);
		
			
			DMPidOutPut[0] = DM1SpeedPID.output;
			DMPidOutPut[1] = DM2SpeedPID.output;
			DMPidOutPut[2] = DM3SpeedPID.output;
			DMPidOutPut[3] = DM4SpeedPID.output;
			
			
			DMPowerLimit();
			//ͨ��CAN1���߷��Ϳ������� ����ֵ-32768 ~ 32767
			for(int i = 0;i<3;i++)
					FinalDMPidOut[i] = GetMedianNum(FinalDMPidOutFliter[i], 6 ,FinalDMPidOut[i]) ;
			
			Underpan_Cmd_ESC(FinalDMPidOut[0],FinalDMPidOut[1],
											 FinalDMPidOut[2],FinalDMPidOut[3]);
										 
										 
			 vTaskDelayUntil(&CurrentControlTick, 5 / portTICK_RATE_MS);
    }
}
/*
�������ƣ�Break_Ctl
˵    �����ƶ�����
���������date1----radio_ahead_back_data
           date2----radio_left_right_data
*/
void Break_Ctl(float date1,float date2)
{
	static int Break_flag=0;
	
	if(date1 ==0 && date2 ==0 && Break_flag==0)
	 {
		//�������ӻ������㣬�������ӵĽǶ�ֵ��ֵ�������Ƕȣ��Ѵﵽ����ɲ����Ч��
		PID_Clear(&DM1SpeedPID);
		PID_Clear(&DM2SpeedPID);
		PID_Clear(&DM3SpeedPID);
		PID_Clear(&DM4SpeedPID);
         
		DM1SpeedPID.tar=Real_Speed_Value[0];
		DM2SpeedPID.tar=Real_Speed_Value[1];
		DM3SpeedPID.tar=Real_Speed_Value[2];
		DM4SpeedPID.tar=Real_Speed_Value[3];
         
		Break_flag=1;
	}
	if(radio_ahead_back_data||radio_left_right_data)
	{
		Break_flag=0;
	}
}

float  PwrLimSingleWheel[4] = {0};
float  tempProp[4] = {0};
float  SelfCalcPower = 0;

float PwrReal[4] = {0};
void DMPowerLimit(void)
{
    float PwrTempSum = 0;
    short PwrUpLimit = 80; 
    short DMOutLimit = 14000;

    static float PwrTemp[4] = {0};
    
    u8 DMWheel_Order = 0;
    for (DMWheel_Order = 0;DMWheel_Order<4;DMWheel_Order++)
    {
        PwrReal[DMWheel_Order] = my_abs(angle_expect_wheel[DMWheel_Order]*DMPidOutPut[DMWheel_Order]);
        PwrTempSum += PwrReal[DMWheel_Order];
    }
    
    if(PwrTempSum !=0 )
    {
        for (DMWheel_Order = 0;DMWheel_Order<4;DMWheel_Order++)
        {
            PwrLimSingleWheel[DMWheel_Order] = PwrReal[DMWheel_Order]*PwrUpLimit/PwrTempSum;
        }
    }
    else
    {
        FinalDMPidOut[0] = DMPidOutPut[0];
        FinalDMPidOut[1] = DMPidOutPut[1];
        FinalDMPidOut[2] = DMPidOutPut[2];
        FinalDMPidOut[3] = DMPidOutPut[3];
        return;
    }
        
    
    PwrTemp[0] = 9549.0f*PwrLimSingleWheel[0]/0.3f*10;
    PwrTemp[1] = 9549.0f*PwrLimSingleWheel[1]/0.3f*10;
    PwrTemp[2] = 9549.0f*PwrLimSingleWheel[2]/0.3f*10;
    PwrTemp[3] = 9549.0f*PwrLimSingleWheel[3]/0.3f*10;
    
    for (DMWheel_Order = 0;DMWheel_Order<4;DMWheel_Order++)
    {
       if(Real_Speed_Value[DMWheel_Order]!=0)
       {
           if(DMPidOutPut[DMWheel_Order]*Real_Speed_Value[DMWheel_Order] >  PwrTemp[DMWheel_Order])
               DMPidOutPut[DMWheel_Order] =  PwrTemp[DMWheel_Order]/Real_Speed_Value[DMWheel_Order];
           
           if(DMPidOutPut[DMWheel_Order]*Real_Speed_Value[DMWheel_Order] < -PwrTemp[DMWheel_Order])
               DMPidOutPut[DMWheel_Order] = -PwrTemp[DMWheel_Order]/Real_Speed_Value[DMWheel_Order];          
       }
       
       FinalDMPidOut[DMWheel_Order] = DMPidOutPut[DMWheel_Order] > DMOutLimit?  DMOutLimit:DMPidOutPut[DMWheel_Order];
       FinalDMPidOut[DMWheel_Order] = DMPidOutPut[DMWheel_Order] <-DMOutLimit? -DMOutLimit:DMPidOutPut[DMWheel_Order];      
    }
}

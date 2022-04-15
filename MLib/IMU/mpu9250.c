#include "mpu9250.h"

/*-------------------------------ÓÃÓÚMPU9250-----------------------------------*/
/*
 * º¯ÊýÃû£ºUSART6_Config
 * ÃèÊö  : ÓÃÓÚMPU9250½ÓÊÕÊý¾Ý
 * IOÅäÖÃ£ºPG14 PG9
 * ÊäÈë  £ºÎÞ
 * Êä³ö  £ºÎÞ
 */ 
void USART6_Config(u32 bound)
{
	GPIO_InitTypeDef	GPIO_InitStrucutre;
	USART_InitTypeDef	USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	/*			enable Colck port				*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);

	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);
	
	/*			config GPIO pin				*/
	GPIO_InitStrucutre.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_9;		
	GPIO_InitStrucutre.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStrucutre.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStrucutre.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStrucutre.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG,&GPIO_InitStrucutre);
	
	/*			config USART6				*/
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART6,&USART_InitStructure);
	
	/*		enable USART6			*/
	USART_Cmd(USART6,ENABLE);
	/*			clear transimission complete flag		*/
	USART_ClearFlag(USART6,USART_FLAG_TC);
	
#if	EN_USART6_RX
		/*			open Rx interrupt			*/
		USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);
		/*			config NVIC control				*/
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_Init(&NVIC_InitStructure);
#endif
	
}
/*
 * º¯ÊýÃû£ºUSART6_IRQHandler
 * ÃèÊö  : ÓÃÓÚMPU9250½ÓÊÕÊý¾Ý£¬ÖÐ¶Ï´¦Àíº¯Êý µ÷ÓÃCopeSerial2Data((int)USART6->DR);//´¦ÀíÊý¾Ý
 * ÊäÈë  £ºÎÞ
 * Êä³ö  £ºÎÞ
 */ 
void USART6_IRQHandler(void)
{
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
  {
		CopeSerial2Data((int)USART6->DR);//´¦ÀíÊý¾Ý
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
  }
	 if (USART_GetFlagStatus(USART6, USART_FLAG_PE) != RESET)  
   {  
     USART_ReceiveData(USART6);  
     USART_ClearFlag(USART6, USART_FLAG_PE);  
   }  
      
   if (USART_GetFlagStatus(USART6, USART_FLAG_ORE) != RESET)  
   {  
     USART_ReceiveData(USART6);  
     USART_ClearFlag(USART6, USART_FLAG_ORE);  
   }  
      
    if (USART_GetFlagStatus(USART6, USART_FLAG_FE) != RESET)  
   {  
      USART_ReceiveData(USART6);  
      USART_ClearFlag(USART6, USART_FLAG_FE);  
   }   
}

/*
 * º¯ÊýÃû£ºCopeSerial2Data
 * ÃèÊö  : ÓÃÓÚMPU9250½ÓÊÕÊý¾Ý½âÂë
 * ÊäÈë  £ºUSART6½ÓÊÕµ½µÄÊý¾Ý£¬ÔÚUSART6ÖÐ¶Ïº¯ÊýÖÐµ÷ÓÃ
 * Êä³ö  £ºÎÞ
 */ 
/*--------------------------------------------------------------------------------------------*/
u16 mpu9250_data[10]={0};
u16 ch;
int cn=0;
short  mpu9250_flag=0;
float  mid_mpu_9250_yaw_angle;
float  mpu_9250_yaw_angle,mpu_9250_pitch_angle,mpu_9250_roll_angle;
float  mpu_9250_yaw_acc,mpu_9250_pitch_acc,mpu_9250_roll_acc;
float  mpu_9250_yaw_speed,mpu_9250_pitch_speed,mpu_9250_roll_speed;
struct SAcc 		stcAcc;
struct SGyro 		stcGyro;
struct SAngle 	    stcAngle;
struct SMag 		stcMag;
/*----------------------MPU9250»úÐµ³õÊ¼ÖµÉè¶¨³õÊ¼Ö------------------------------------------------µ*/
float mpu_9250_yaw_first=0;
float mpu_9250_pitch_first=0;

float mpu_9250_yaw_zero=0;// ÍÓÂÝÒÇÉÏµç½Ç¶È »úÐµ½Ç¶È °²×°ºóÐèÒª±ê¶¨
float mpu_9250_pitch_zero=0;
/*--------------------------------------------------------------------------------------------*/
float MPU9250_Gun_Real_Angle_Value_Flt1[2] = {0};
void CopeSerial2Data(int ucData)
{
	static float mpu_9250_yaw_Position_Value;
	static int ucRxBuffer[250];
	static int ucRxCnt = 0;	
	ucRxBuffer[ucRxCnt]=ucData;
	ucRxCnt++;
	if (ucRxBuffer[0]!=0x55) //Êý¾ÝÍ·²»¶Ô£¬ÔòÖØÐÂ¿ªÊ¼Ñ°ÕÒ0x55Êý¾ÝÍ·
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<11) {return;}//Êý¾Ý²»Âú11¸ö£¬Ôò·µ»Ø
	else
	{
		switch(ucRxBuffer[1])
		{
			case 0x51:
						stcAcc.a[0]=((ucRxBuffer[3]<<8) | ucRxBuffer[2]);//32768.0*16*9.8;
						stcAcc.a[1]=((ucRxBuffer[5]<<8) | ucRxBuffer[4]);//32768.0*16*9.8;
						stcAcc.a[2]=((ucRxBuffer[7]<<8) | ucRxBuffer[6]);//32768.0*16*9.8;
						stcAcc.Temperature=((ucRxBuffer[9]<<8) | ucRxBuffer[8])/100;
						if(stcAcc.a[0]>0x7fff) stcAcc.a[0]-=0xffff;
						if(stcAcc.a[1]>0x7fff) stcAcc.a[1]-=0xffff;
						if(stcAcc.a[2]>0x7fff) stcAcc.a[2]-=0xffff;
						mpu_9250_roll_acc = stcAcc.a[0]/32768*16*9.8;
						mpu_9250_pitch_acc = stcAcc.a[1]/32768*16*9.8;
						mpu_9250_yaw_acc = stcAcc.a[2]/32768*16*9.8;
						break;
			case 0x52:
						stcGyro.w[0]=((ucRxBuffer[3]<<8) | ucRxBuffer[2]);//32768.0*16*9.8;
						stcGyro.w[1]=((ucRxBuffer[5]<<8) | ucRxBuffer[4]);//32768.0*16*9.8;
						stcGyro.w[2]=((ucRxBuffer[7]<<8) | ucRxBuffer[6]);//32768.0*16*9.8;
						stcGyro.Temperature=((ucRxBuffer[9]<<8) | ucRxBuffer[8])/100;
						if(stcGyro.w[0]>0x7fff) stcGyro.w[0]-=0xffff;
						if(stcGyro.w[1]>0x7fff) stcGyro.w[1]-=0xffff;
						if(stcGyro.w[2]>0x7fff) stcGyro.w[2]-=0xffff;
						mpu_9250_roll_speed = stcGyro.w[0]/32768.0*2000;
						mpu_9250_pitch_speed = stcGyro.w[1]/32768.0*2000;
						mpu_9250_yaw_speed = stcGyro.w[2]/32768.0*2000;
						break;
			case 0x53:
						stcAngle.Angle[0]=((ucRxBuffer[3]<<8) | ucRxBuffer[2]);//32768.0*16*9.8;
						stcAngle.Angle[1]=((ucRxBuffer[5]<<8) | ucRxBuffer[4]);//32768.0*16*9.8;
						stcAngle.Angle[2]=((ucRxBuffer[7]<<8) | ucRxBuffer[6]);//32768.0*16*9.8;
						stcAngle.Temperature=((ucRxBuffer[9]<<8) | ucRxBuffer[8])/100;
						if(stcAngle.Angle[0]>0x7fff) stcAngle.Angle[0]-=0xffff;
						if(stcAngle.Angle[1]>0x7fff) stcAngle.Angle[1]-=0xffff;
						if(stcAngle.Angle[2]>0x7fff) stcAngle.Angle[2]-=0xffff;
						mpu_9250_roll_angle = stcAngle.Angle[0]/32768.0*180;
						mpu_9250_pitch_angle = stcAngle.Angle[1]/32768.0*180;//mpu_9250²âµÃµÄÊý¾Ý  µ¥Î»½Ç¶È
						mpu_9250_yaw_angle = stcAngle.Angle[2]/32768.0*180;
			
						break;
			case 0x54:
						stcMag.h[0]=((ucRxBuffer[3]<<8) | ucRxBuffer[2]);
						stcMag.h[1]=((ucRxBuffer[5]<<8) | ucRxBuffer[4]);
						stcMag.h[2]=((ucRxBuffer[7]<<8) | ucRxBuffer[6]);
						if(stcMag.h[0]>0x7fff) stcMag.h[0]-=0xffff;
						if(stcMag.h[1]>0x7fff) stcMag.h[1]-=0xffff;
						if(stcMag.h[2]>0x7fff) stcMag.h[2]-=0xffff;
						stcMag.Temperature=((ucRxBuffer[9]<<8) | ucRxBuffer[8])/100;
						break;
		}
        mpu9250_flag=1;
		for(ucRxCnt=0;ucRxCnt<11;ucRxCnt++)
		{
			ucRxBuffer[ucRxCnt]=0;
		}
		ucRxCnt=0;
		
		//½«YAWÊý¾ÝÁ¬Ðø»¯
		mid_mpu_9250_yaw_angle=mpu_9250_yaw_angle;
		mpu_9250_yaw_Position_Value = MPU9250_EncoderProcess(MPU9250_Gun_Real_Angle_Value_Flt1,
																		mid_mpu_9250_yaw_angle,mpu_9250_yaw_Position_Value,300);	
		mid_mpu_9250_yaw_angle=mpu_9250_yaw_Position_Value;
		if(mid_mpu_9250_yaw_angle>30000||mid_mpu_9250_yaw_angle<-30000) //·ÀÖ¹Ô½½ç
		{		
			mid_mpu_9250_yaw_angle=0;
			mpu_9250_yaw_Position_Value=0;
		}
	}
}

/*
 * º¯ÊýÃû£ºMPU9250_calibration
 * ÃèÊö  : ÓÃÓÚMPU9250Ð£×¼
 * ÊäÈë  £ºMPU9250_Pitch_turn
 * Êä³ö  £ºÎÞ
 */ 
u8 MPU9250_calibration(u8 number)
{
	uint8_t status=0;
	
	float value_buf[2][number],sum[2];
	int mi=0,count;
	for (mi = 0; mi < number; mi ++)
		{
		value_buf[0][mi] = value_buf[0][mi+1];
		value_buf[1][mi] = value_buf[1][mi+1];
		}
	 value_buf[0][9] = mpu_9250_pitch_angle;
	 value_buf[1][9] = mpu_9250_yaw_angle;
  
	for(count=0;count<number;count++)
		{
	  sum[0]+=value_buf[0][count];
		sum[1]+=value_buf[0][count];
		}
		
	  mpu_9250_pitch_zero=sum[0]/number;
    mpu_9250_yaw_zero=sum[1]/number;
		
    status=1;
	return(status);									//·µ»Ø×´Ì¬Öµ
}
float MPU9250_EncoderProcess(float *filter,float filter_in ,float filter_out,float filter_parameter)
{
	float filter_d_dail = 0;
	
	filter[0] = filter[1];
	filter[1] = filter_in;
	filter_d_dail = filter[1]-filter[0];
	
	//Ë³Ê±Õë
	if(filter_d_dail > filter_parameter)  filter_d_dail=(filter[1]-360) - filter[0];
	//ÄæÊ±Õë 
	if(filter_d_dail < -filter_parameter) filter_d_dail=(filter[1]+360) - filter[0];

	filter_out = filter_d_dail + filter_out ; //Ò»½×ÂË²¨ÂË¼â·åÔëÉù
	return filter_out;
}

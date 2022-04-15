/***************************************************************************************************************
名称：can2.c

实现功能：对底盘和云台的鼠标键盘操作
修改日期：20161126
作者：	：上届团队
修改格式： 修改者+修改项目+修改日期


***************************************************************************************************************/
#include "can2.h"
#include "key.h"

/*----CAN2_TX-----PB13----*/
/*----CAN2_RX-----PB12----*/
void CAN2_Configuration(void)
{
    CAN_InitTypeDef        can;
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       gpio;
    NVIC_InitTypeDef       nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2); 

    gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 ;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOB, &gpio);

    nvic.NVIC_IRQChannel = CAN2_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    CAN_DeInit(CAN2);
    CAN_StructInit(&can);

    can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = DISABLE;    
    can.CAN_AWUM = DISABLE;    
    can.CAN_NART = DISABLE;    
    can.CAN_RFLM = DISABLE;    
    can.CAN_TXFP = ENABLE;     
    can.CAN_Mode = CAN_Mode_Normal; 
    can.CAN_SJW  = CAN_SJW_1tq;
    can.CAN_BS1 = CAN_BS1_9tq;
    can.CAN_BS2 = CAN_BS2_4tq;
    can.CAN_Prescaler = 3;   //CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN2, &can);
    
    can_filter.CAN_FilterNumber=14;
    can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
    can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
    can_filter.CAN_FilterIdHigh=0x0000;
    can_filter.CAN_FilterIdLow=0x0000;
    can_filter.CAN_FilterMaskIdHigh=0x0000;
    can_filter.CAN_FilterMaskIdLow=0x0000;
    can_filter.CAN_FilterFIFOAssignment=0;//the message which pass the filter save in fifo0
    can_filter.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&can_filter);
    
    CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);
}


void Yuntai_Cmd_ESC(int16_t output1,int16_t output2,int16_t output3,int16_t output4)
{
    CanTxMsg tx_message;
    
    tx_message.StdId = 0x200;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = ((output1>>8)&0xff);
    tx_message.Data[1] = output1&0xff;
    tx_message.Data[2] = ((output2>>8)&0xff);
    tx_message.Data[3] = output2&0xff;
    tx_message.Data[4] = ((output3>>8)&0xff);
    tx_message.Data[5] = output3&0xff;
    tx_message.Data[6] = ((output4>>8)&0xff);
    tx_message.Data[7] = output4&0xff;
    
    CAN_Transmit(CAN2,&tx_message);
}

void Yuntai_Yaw_Cmd_ESC(int16_t yaw, int16_t dia_wheel)
{
    CanTxMsg tx_message;
    
    tx_message.StdId = 0x1FF;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = (unsigned char)(yaw >> 8);
    tx_message.Data[1] = (unsigned char)yaw;
    tx_message.Data[2] = (unsigned char)(dia_wheel >> 8);
    tx_message.Data[3] = (unsigned char)dia_wheel;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    
    CAN_Transmit(CAN2,&tx_message);
}
/*************************************************************************
                          CAN2_RX0_IRQHandler
描述：用于云台测试的 can总线接受函数
*************************************************************************/
float center_pitch=83.5f;//增大数值 使云台抬头 反之低头
float center_yaw=240.5f;//增大数值 使云台左转 反之右转
short Yaw_Angle_Value;  //CAN2接收的 DATE1 DATE2 的数据
short Yaw_Speed_Value;  //CAN2接收的 DATE3 DATE4 的数据
short Pitch_Angle_Value;  //CAN2接收的 DATE1 DATE2 的数据
short Pitch_Speed_Value;  //CAN2接收的 DATE3 DATE4 的数据

short Can2_Real_Angle_Value[4] = {0};  //CAN1接收的 DATE1 DATE2 的数据
short Can2_Real_Speed_Value[4] = {0};  //CAN1接收的 DATE3 DATE4 的数据
long  Can2_Real_Position_Value[4] = {0};//CAN1接收的 DATE5 DATE6 DATE7 DATE8 的数据

float CAN2_Real_Angle_Value_Flt0[2] = {0};
float CAN2_Real_Angle_Value_Flt1[2] = {0};
float CAN2_Real_Angle_Value_Flt2[2] = {0};
float CAN2_Real_Angle_Value_Flt3[2] = {0};

int16_t  Weapon_Real_Speed_Value[2]={0};

int Can2_receive_date_flag=0;
void  CAN2_RX0_IRQHandler(void)
{
    CanRxMsg rx_message;
    if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET) 
    {
      CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
      CAN_Receive(CAN2, CAN_FIFO0, &rx_message); 
			red_on();//绿色的指示灯

			//yaw数据
			if(rx_message.StdId == 0x205) 
			{
				Can2_Real_Angle_Value[1]    = (rx_message.Data[0]<<8)|(rx_message.Data[1]);
				Can2_Real_Angle_Value[1]    =	Can2_Real_Angle_Value[1]*360.0f/0x1fff;
				Can2_Real_Speed_Value[1]    = (rx_message.Data[2]<<8)|(rx_message.Data[3]);
				Can2_Real_Position_Value[1] = CAN2_EncoderProcess(CAN2_Real_Angle_Value_Flt1,
																			Can2_Real_Angle_Value[1],Can2_Real_Position_Value[1],300);
				Can2_Real_Angle_Value[1]    = Can2_Real_Position_Value[1];
				Yaw_Angle_Value							=	Can2_Real_Angle_Value[1];
				Yaw_Speed_Value							= Can2_Real_Speed_Value[1];
			}
			
			else if(rx_message.StdId == 0x206)//波轮电机
			{
				Real_Angle_Value[4] = (rx_message.Data[0]<<8)|(rx_message.Data[1]);
				Real_Angle_Value[4] = Real_Angle_Value[4]*360.0f/0x1fff;
				Real_Position_Value[4] = Can1EncoderProcess(Real_Angle_Value_Flt[4],Real_Angle_Value[4],Real_Position_Value[4] ,300);
				Real_Angle_Value[4] = Real_Position_Value[4] / 96.0f;
				Real_Speed_Value[4]  = (rx_message.Data[2]<<8)|(rx_message.Data[3]);
				
				Gun_Real_Angle_Value = Real_Angle_Value[4];
				Gun_Real_Speed_Value = Real_Speed_Value[4];
			}
	 
					//武器摩擦轮1数据  左边
			else if(rx_message.StdId == 0x203) 
			{          
				Weapon_Real_Speed_Value[0]    = (rx_message.Data[2]<<8)|(rx_message.Data[3]);
			}
					
			//武器摩擦轮2数据   右边
			else if(rx_message.StdId == 0x204) 
			{          
				Weapon_Real_Speed_Value[1]    = (rx_message.Data[2]<<8)|(rx_message.Data[3]);
			}  
			//pitch数据	
			else if(rx_message.StdId == 0x201) 
			{
				Can2_Real_Angle_Value[0]    = (rx_message.Data[0]<<8)|(rx_message.Data[1]);
				Can2_Real_Angle_Value[0]    = Can2_Real_Angle_Value[0]*360.0f/0x1fff;
				Can2_Real_Speed_Value[0]    = (rx_message.Data[2]<<8)|(rx_message.Data[3]);
				Can2_Real_Position_Value[0] = CAN2_EncoderProcess(CAN2_Real_Angle_Value_Flt0,
																			Can2_Real_Angle_Value[0],Can2_Real_Position_Value[0],300);
				Can2_Real_Angle_Value[0]    = Can2_Real_Position_Value[0];
				Pitch_Angle_Value						= Can2_Real_Angle_Value[0]/19;
				Pitch_Speed_Value						=	Can2_Real_Speed_Value[0];
			}
			
		Can2_receive_date_flag=1;
   }
}

float CAN2_EncoderProcess(float *filter,float filter_in ,float filter_out,float filter_parameter)
{
	float filter_d_dail;
	
	filter[0] = filter[1];
	filter[1] = filter_in;
	filter_d_dail = filter[1]-filter[0];
	//if(filter_d_dail > filter_parameter || filter_d_dail < -filter_parameter)	filter_d_dail=0;
	//逆时针
	if(filter_d_dail > filter_parameter) filter_d_dail=(filter[1]-360)-filter[0];
	//顺时针 
	if(filter_d_dail < -filter_parameter) filter_d_dail=(360-filter[0])+filter[1];

	filter_out = filter_d_dail + filter_out ; //一阶滤波滤尖峰噪声
	return filter_out;
}


void TIM4_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  tim;
    NVIC_InitTypeDef         nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
    nvic.NVIC_IRQChannel = TIM4_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
	
    tim.TIM_Prescaler = 42-1;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_Period = 400-1;
    TIM_TimeBaseInit(TIM4,&tim);
}

void TIM4_Start(void)
{
    TIM_Cmd(TIM4, ENABLE);	 
    TIM_ITConfig(TIM4, TIM_IT_Update,ENABLE);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);	
}


void TIM4_IRQHandler(void)  //云台发送中断	
{
  if (TIM_GetITStatus(TIM4,TIM_IT_Update)!= RESET) 
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);

  }
}

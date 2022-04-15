/************************************************************************************************************************
函数名：can1.c
功能：can1的通信
作者：孙晓军  杨师兄
简化代码 删除了无用的数据缓存空间
************************************************************************************************************************/
#include "can1.h"
/*************************************************************************
                          CAN1_Configuration
描述：初始化CAN1配置为1M波特率
引脚配置：CAN1_TX(PD1) CAN1_RX(PD0)
*************************************************************************/
void CAN1_Configuration(void)
{
    CAN_InitTypeDef        can;
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       gpio;
    NVIC_InitTypeDef       nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);

    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOD, &gpio);
    
    nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    nvic.NVIC_IRQChannel = CAN1_TX_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    CAN_DeInit(CAN1);
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
    CAN_Init(CAN1, &can);

	can_filter.CAN_FilterNumber = 0;
	can_filter.CAN_FilterMode = CAN_FilterMode_IdMask;
	can_filter.CAN_FilterScale = CAN_FilterScale_32bit;
	can_filter.CAN_FilterIdHigh = 0x0000;
	can_filter.CAN_FilterIdLow = 0x0000;
	can_filter.CAN_FilterMaskIdHigh = 0x0000;
	can_filter.CAN_FilterMaskIdLow = 0x0000;
	can_filter.CAN_FilterFIFOAssignment = 0;
	can_filter.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&can_filter);
    
    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
    CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);
}

/*************************************************************************
                          CAN1_TX_IRQHandler
描述：CAN1的发送中断函数
*************************************************************************/
void CAN1_TX_IRQHandler(void) //CAN TX
{
    if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET) 
	{
	     CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
    }
}
//can1	数据接受
/*---------------17mm武器 波轮电机--------------------------------------------*/
float Gun_Real_Angle_Value = 0; //CAN1接收的波轮电机数据
float Gun_Real_Speed_Value = 0; //CAN1接收的波轮电机数据
float Gun_Real_Angle_Value_Flt[2] = {0};
float Gun_Real_Angle_Value_0_360 = 0;
float Gun_Real_Angle_Value_Gearbox = 0;
float Gun_Target_Angle_Value=0;
/*----------------底盘3510电机------------------------------------------------*/
short Real_Angle_Value[5] = {0};  //CAN1接收的 DATE1 DATE2 的数据
short Real_Speed_Value[5] = {0};  //CAN1接收的 DATE3 DATE4 的数据
long Real_Position_Value[5] = {0};//CAN1接收的 DATE5 DATE6 DATE7 DATE8 的数据
/*----------------拨轮电机-------------------------------------------------*/
/*----------------------------------------------------------------------------*/
float Real_Angle_Value_Flt[5][2] = {0};
int Can1_receive_date_flag=0;
void CAN1_RX0_IRQHandler(void)
{
    
    CanRxMsg rx_message;
    u8 n=0;
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
        CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
        
				if(rx_message.StdId == 0x201)
				{
					Real_Angle_Value[0] = (rx_message.Data[0]<<8)|(rx_message.Data[1]);
					Real_Angle_Value[0] = Real_Angle_Value[0]*360.0f/0x1fff;
					Real_Speed_Value[0] = (rx_message.Data[2]<<8)|(rx_message.Data[3]);
					Real_Position_Value[0] = Can1EncoderProcess(Real_Angle_Value_Flt[0],Real_Angle_Value[0],Real_Position_Value[0],300);
					Real_Angle_Value[0]=Real_Position_Value[0]*0.053f;
				}
				else if(rx_message.StdId == 0x202)
				{
					Real_Angle_Value[1] = (rx_message.Data[0]<<8)|(rx_message.Data[1]);
					Real_Angle_Value[1] = Real_Angle_Value[1]*360.0f/0x1fff;
					Real_Speed_Value[1] = (rx_message.Data[2]<<8)|(rx_message.Data[3]);
					Real_Position_Value[1] = Can1EncoderProcess(Real_Angle_Value_Flt[1],Real_Angle_Value[1], Real_Position_Value[1],300);
					Real_Angle_Value[1]=Real_Position_Value[1]*0.053f;
				}
				else if(rx_message.StdId == 0x203)
				{
					Real_Angle_Value[2] = (rx_message.Data[0]<<8)|(rx_message.Data[1]);
					Real_Angle_Value[2] = Real_Angle_Value[2]*360.0f/0x1fff;
					Real_Speed_Value[2] = (rx_message.Data[2]<<8)|(rx_message.Data[3]);
					Real_Position_Value[2] = Can1EncoderProcess(Real_Angle_Value_Flt[2],Real_Angle_Value[2], Real_Position_Value[2],300);
					Real_Angle_Value[2]=Real_Position_Value[2]*0.053f;
				}
				else if(rx_message.StdId == 0x204)
				{
					Real_Angle_Value[3] = (rx_message.Data[0]<<8)|(rx_message.Data[1]);
					Real_Angle_Value[3] = Real_Angle_Value[3]*360.0f/0x1fff;
					Real_Speed_Value[3] = (rx_message.Data[2]<<8)|(rx_message.Data[3]);
					Real_Position_Value[3] = Can1EncoderProcess(Real_Angle_Value_Flt[3],Real_Angle_Value[3], Real_Position_Value[3],300);
					Real_Angle_Value[3]=Real_Position_Value[3]*0.053f;
				}

//----------------------------以上是底盘电机的四个轮子的数据-----------------------------------------------------------------------------	         
				for(n=0;n<5;n++)
				{
				 if((Real_Angle_Value[n]>30000) || (Real_Angle_Value[n]<-30000))
				 {					
					 Real_Angle_Value[n]=0;
					 Real_Position_Value[n]=0;
					 angle_expect_wheel[n]=0;
					 Real_Angle_Value_Flt[n][0]=0;
					 Real_Angle_Value_Flt[n][1]=0;
				 }
				}
				
				Can1_receive_date_flag=1;
//----------------------------临时使用CAN1来进行云台控制-----------------------------------------------------------				
}
}
/********************************************************************************
给电调发送控制指令
电机的报文地址是： 0x200 
*********************************************************************************/
void Underpan_Cmd_ESC(int16_t output1,int16_t output2,int16_t output3,int16_t output4)
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
    
    CAN_Transmit(CAN1,&tx_message);
}


float Can1EncoderProcess(float *filter,float filter_in ,float filter_out,float filter_parameter)
{ 
	float filter_d_dail;
	
	filter[0] = filter[1];
	filter[1] = filter_in;
	filter_d_dail = filter[1]-filter[0];
	
	//逆时针
	if(filter_d_dail > filter_parameter) filter_d_dail=(filter[1]-360)-filter[0];
	//顺时针 
	if(filter_d_dail < -filter_parameter) filter_d_dail=(360-filter[0])+filter[1];

	filter_out = filter_d_dail + filter_out ; //一阶滤波滤尖峰噪声
	return filter_out;
}

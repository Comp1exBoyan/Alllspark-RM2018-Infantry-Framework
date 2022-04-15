#include "usart3.h"
#include "SBUS.h"


/*-----USART3_RX(PD9)-----*/
/*-----USART3_TX(PD8-----*/


u8  UartAuxiFlag=1,UartBuffFlag=0;
int rev[8]={0};
void USART3_Configuration(u32 bound)
{
	GPIO_InitTypeDef	GPIO_InitStrucutre;
	USART_InitTypeDef	USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	/*			enable Colck port				*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);
	
	/*			config GPIO pin				*/
	GPIO_InitStrucutre.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;		
	GPIO_InitStrucutre.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStrucutre.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStrucutre.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStrucutre.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStrucutre);
	
	/*			config USART3				*/
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStructure);
	
	/*		enable USART3			*/
	USART_Cmd(USART3,ENABLE);
	/*			clear transimission complete flag		*/
	USART_ClearFlag(USART3,USART_FLAG_TC);
	
#if	EN_USART3_RX
		/*			open Rx interrupt			*/
		USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
		/*			config NVIC control				*/
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_Init(&NVIC_InitStructure);
#endif
}
/****************************步兵辅助瞄准********************
作者：唐义丰
功能：读取minipc数据
**********************************************************/
u8 RecieveBuffflag = 0;
u8 RecieveAuxiliaryflag = 0;

float Buff_yaw = 0, Buff_pitch = 0;
float Buff_yaw_store = 0, Buff_pitch_store = 0;
float Auxi_yaw = 0, Auxi_pitch = 0,Auxi_Distance=0;
float TempbuffYaw = 0,TempbuffPit = 0; 
void USART3_IRQHandler(void)
{ 
	u16 sum,judge = 0;
	u8 RX_flag = 0;
	u16 Test = 0;
	static u8 mark = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
        
		if(UartAuxiFlag ==1 )
		{
			RX_flag = USART_ReceiveData(USART3);
			switch(mark)
			{
				case 0: if(RX_flag == 0xFF) mark++;break;
				case 1: if(RX_flag == 0x00) mark++;break;
				case 2: rev[0] = USART_ReceiveData(USART3);mark++;break;
				case 3: rev[1] = USART_ReceiveData(USART3);mark++;break;
				case 4: rev[2] = USART_ReceiveData(USART3);mark++;break;
				case 5: rev[3] = USART_ReceiveData(USART3);mark++;break;
				case 6: rev[4] = USART_ReceiveData(USART3);mark++;break;
				case 7: rev[5] = USART_ReceiveData(USART3);mark++;break;
                case 8: rev[6] = USART_ReceiveData(USART3);mark++;break;
                case 9: rev[7] = USART_ReceiveData(USART3);mark=0;   
     
				sum = (rev[1]<<8|rev[0])+(rev[3]<<8|rev[2])+(rev[5]<<8|rev[4])+255;
				judge = rev[7]<<8|rev[6];
				if(sum == judge)
				{
					if(rev[1]&0x80)//如果是负数，求原码
					{
						Test = (rev[1]<<8)|rev[0];
						Test--;
						Test = ~Test;
						rev[0] = Test&0x00FF;
						rev[1] = (Test>>8)&0x00FF;
						Test = 0;
						Auxi_yaw = -((rev[1]<<8)|rev[0])/100.0;
					}
					else Auxi_yaw = ((rev[1]<<8)|rev[0])/100.0;
					if(rev[3]&0x80)
					{
						Test = (rev[3]<<8)|rev[2];
						Test--;
						Test = ~Test;
						rev[2] = Test&0x00FF;
						rev[3] = (Test>>8)&0x00FF;
						Test = 0;
						Auxi_pitch = -((rev[3]<<8)|rev[2])/100.0;
					}
					else Auxi_pitch = ((rev[3]<<8)|rev[2])/100.0;
                    if(rev[5]&0x80)
					{
						Test = (rev[5]<<8)|rev[4];
						Test--;
						Test = ~Test;
						rev[4] = Test&0x00FF;
						rev[5] = (Test>>8)&0x00FF;
						Test = 0;
						Auxi_Distance = -((rev[5]<<8)|rev[4])/100.0;
					}
					else Auxi_Distance = ((rev[5]<<8)|rev[4])/100.0;
					RecieveAuxiliaryflag = 1;
					break;
				}
				else 
        break;
        }
      }
		
			else if(UartBuffFlag == 1 )
			{
					RX_flag = USART_ReceiveData(USART3);
					switch(mark)
					{
							case 0: if(RX_flag == 0xFF) mark++;break;
							case 1: if(RX_flag == 0x00) mark++;break;
							case 2: rev[0] = USART_ReceiveData(USART3);mark++;break;
							case 3: rev[1] = USART_ReceiveData(USART3);mark++;break;
							case 4: rev[2] = USART_ReceiveData(USART3);mark++;break;
							case 5: rev[3] = USART_ReceiveData(USART3);mark++;break;
							case 6: rev[4] = USART_ReceiveData(USART3);mark++;break;
							case 7: rev[5] = USART_ReceiveData(USART3);mark=0;
							sum = (rev[1]<<8|rev[0])+(rev[3]<<8|rev[2])+255;
							judge = rev[5]<<8|rev[4];
							if(sum == judge)
							{
								if(rev[1]&0x80)//如果是负数，求原码
								{
									Test = (rev[1]<<8)|rev[0];
									Test--;
									Test = ~Test;
									rev[0] = Test&0x00FF;
									rev[1] = (Test>>8)&0x00FF;
									Test = 0;
									TempbuffYaw = -((rev[1]<<8)|rev[0])/100.0;
															
								}
								else TempbuffYaw = ((rev[1]<<8)|rev[0])/100.0;
													
								if(rev[3]&0x80)
								{
									Test = (rev[3]<<8)|rev[2];
									Test--;
									Test = ~Test;
									rev[2] = Test&0x00FF;
									rev[3] = (Test>>8)&0x00FF;
									Test = 0;
									TempbuffPit = - ((rev[3]<<8)|rev[2])/100.0;
								}
								else TempbuffPit = ((rev[3]<<8)|rev[2])/100.0;
													
								if(TempbuffPit == Buff_pitch && TempbuffYaw == Buff_yaw)
											return;
								else
								{
										Buff_pitch = TempbuffPit;
										Buff_yaw = TempbuffYaw;
										Buff_pitch_store = TempbuffPit;
										Buff_yaw_store = TempbuffYaw;
								}
								RecieveBuffflag = 1;
								break;
							}
							else 
							break;
					 }
        }
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);					//clear IT pending flag
	}
				 
} 

void USART3_SendChar(unsigned char b)
{
    static int u3 = 0;
    
    while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
			USART_SendData(USART3,b);
    
    u3++;
}


void SendToPc(u8 model)
{
		switch (model)
		{
			case EXIT: 		USART3_SendChar(0x30);break; //0x30 代表终止程序
			case AXUXARM: USART3_SendChar(0x41);break; //0x41 代表红色装甲板
			case BLUEARM: USART3_SendChar(0x42);break; //0x42 代表蓝色装甲板
			case SMLBUFF: USART3_SendChar(0x31);break; //0x31 代表小buff
			case BIGBUFF: USART3_SendChar(0x32);break; //0x32 代表大buff
			case SENDPOS: USART3_SendChar(0x33);break; //0x33 请求buff数据
		}
}


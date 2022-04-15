#include "SBUS.h"
#include "delay.h"
#include "Weapon.h"
#include "can2.h"
#include "usart3.h"
#include "main.h"
/***************************************************************************************************************
���ƣ�������

ʵ�ֹ��ܣ��Ե��̺���̨�������̲���
���Ŷ��壺USART1_RX��PB7��
�����ʣ�100000
DMA:DMA2_Stream2_IRQn
˵�������ջ�ÿ 7ms  ���յ�һ������

��Ҫ���ز�����
   radio_3 -��̨����ת����Ӧ��������ƶ�
   radio_2 -��̨����ת����Ӧ��������ƶ�
   mouse_state -���״̬
   radio_ahead_back_data
   radio_left_right_data 
   radio_turn_data

�޸����ڣ�20161126
���ߣ�	��warpgate_hzk
�޸ĸ�ʽ�� �޸���+�޸���Ŀ+�޸�����
�޸��ߣ�  warpgate_hzk	����һ�죨2016��������������̨���ֳ�����ȡ�������Եİ汾	20161126

�޸����ڣ�20171012
�޸��ߣ�  ������	����һ�죨2017�������̳����� �����ڽ��ճ��� ���� ң�ؿ���ģʽ  ���̿���ģʽ������һ�� 

***************************************************************************************************************/
/*
 * ��������USART1_Configuration
 * ����  ��SUBS���ջ�����
 * ˵��  ��GPIO_Pin_7(RX) USART_BaudRate = 100000;
 * ����  ����
 * ���  ����
 */ 
u8 attack_direction = 0;
u8    SpeedModel   = 0;
short KeyBordSpeed = 800;

float radio_yaw=0;//yaw
float radio_pitch=0;//pitch
float radio_ahead_back_data;
float radio_left_right_data;
float radio_turn_data;
float radio_turn_control_data;
float radio_pitch_control_data;
float buff_init_angle_yaw;
float buff_init_angle_pit;

float yaw_limite = 0;
float yaw_limite_flt[6] = {0};

usetoctl Uppertoctl;

volatile unsigned char sbus_rx_buffer[25];
void USART1_Configuration(void)
{   
	USART_InitTypeDef usart1;
	GPIO_InitTypeDef  gpio;
	NVIC_InitTypeDef  nvic;
	DMA_InitTypeDef   dma;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);

	gpio.GPIO_Pin = GPIO_Pin_7 ;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&gpio);

	USART_DeInit(USART1);
	usart1.USART_BaudRate = 100000;   							//SBUS 100K baudrate
	usart1.USART_WordLength = USART_WordLength_8b;
	usart1.USART_StopBits = USART_StopBits_1;
	usart1.USART_Parity = USART_Parity_Even;
	usart1.USART_Mode = USART_Mode_Rx;
	usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1,&usart1);

	USART_Cmd(USART1,ENABLE);
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	nvic.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	DMA_DeInit(DMA2_Stream2);
	dma.DMA_Channel= DMA_Channel_4;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	dma.DMA_Memory0BaseAddr = (uint32_t)sbus_rx_buffer;
	dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma.DMA_BufferSize = 18;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_VeryHigh;
	dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	dma.DMA_MemoryBurst = DMA_Mode_Normal;
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream2,&dma);

	DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA2_Stream2,ENABLE);
}

/*
 * ��������DMA2_Stream2_IRQHandler
 * ����  ��SUBS���ջ��Ӵ��� �жϴ�����

 * ����data_decoder();  //���ݽ���

 * ����  ����
 * ���  ����
 */ 
void DMA2_Stream2_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2))
    {
				DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
				DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
				data_decoder();  //���ݽ���
	  }
}
/*
 * ��������data_decoder
 * ����  ��SUBS���ջ��������ݽ���
 * ��DMA2_Stream2_IRQHandler�����б�����
 * ����  ����
 * ���  ����
 */ 
struct RC_Ctl_t RC_Ctl;
struct RC_Ctl_t Laster_RC_Ctl;
decode_keyboard decode_the_key;
decode_keyboard decode_the_key_last;
int ON_OFF=1,ON_OFF_CURRENT=3,ON_OFF_BEFORE=3,sbus_num0;

u8 separate_flag = 0;//��̨���̷�����Ʊ�־λ
u8 auxiliary_flag=0;//shift�ߵ�ƽ����������׼
u8 Left45Degree_flag=0;
u8 Righ45Degree_flag=0;
u8 buff_flag = 0;
u8 BlueOrRedChoose = 0;

float Buff_yaw_last = 0, Buff_pitch_last = 0;

void data_decoder(void)
{
	
	ON_OFF_CURRENT = ((sbus_rx_buffer[5] >> 4)& 0x0003);//ң���Ҳ࿪��switch_right
	if(ON_OFF_CURRENT==3 && ON_OFF_BEFORE==1)
	{
		ON_OFF=!ON_OFF;
	}
	ON_OFF_BEFORE=ON_OFF_CURRENT;
	
	if(ON_OFF==1)//����ң�ؽ���
	{
		decode_the_key_last  = decode_the_key;
		Laster_RC_Ctl=RC_Ctl;
		sbus_num0=sbus_num0+1;
		/*----------------------------����ң����-------------------------------*/
		RC_Ctl.rc.ch0 = (sbus_rx_buffer[0]| (sbus_rx_buffer[1] << 8)) & 0x07ff;
		RC_Ctl.rc.ch1 = ((sbus_rx_buffer[1] >> 3) | (sbus_rx_buffer[2] << 5)) & 0x07ff;
		RC_Ctl.rc.ch2 = ((sbus_rx_buffer[2] >> 6) | (sbus_rx_buffer[3] << 2) | (sbus_rx_buffer[4] << 10)) & 0x07ff;
		RC_Ctl.rc.ch3 = ((sbus_rx_buffer[4] >> 1) | (sbus_rx_buffer[5] << 7)) & 0x07ff;
		RC_Ctl.rc.switch_left = ((sbus_rx_buffer[5] >> 4)& 0x000C) >> 2;//radio_switch_left Switch left (1->2->3)
		RC_Ctl.rc.switch_right = ((sbus_rx_buffer[5] >> 4)& 0x0003);//radio_switch_right
		/*---------------------------�������---------------------------------*/	
			
		/*----------------------------�������-------------------------------*/
		RC_Ctl.mouse.x=sbus_rx_buffer[6]|(sbus_rx_buffer[7]<<8);//���x��
		RC_Ctl.mouse.y=sbus_rx_buffer[8]|(sbus_rx_buffer[9]<<8);//���y��
		RC_Ctl.mouse.z=sbus_rx_buffer[10]|(sbus_rx_buffer[11]<<8);//���z��
		RC_Ctl.mouse.press_l = sbus_rx_buffer[12]; //������
		RC_Ctl.mouse.press_r = sbus_rx_buffer[13]; //����Ҽ�
		/*---------------------------�������---------------------------------*/	
			
		/*----------------------------������̰�-------------------------------*/
		//���̵�ֵ�������������ܳ�����ʮ���룬��ɼ�֡�����ݲ��ɿ������������ش��������˲�
		// w:1   a:4  s:2  d:8  q:40  e:80   shift:10   ctrl:20 
		RC_Ctl.key.v=sbus_rx_buffer[14]|(sbus_rx_buffer[15]<<8);
		decode_the_key.key_w     =  RC_Ctl.key.v & 0x0001;
		decode_the_key.key_s     = (RC_Ctl.key.v & 0x0002)>>1;
		decode_the_key.key_a     = (RC_Ctl.key.v & 0x0004)>>2;
		decode_the_key.key_d     = (RC_Ctl.key.v & 0x0008)>>3;
		decode_the_key.key_shift = (RC_Ctl.key.v & 0x0010)>>4;
		decode_the_key.key_ctrl  = (RC_Ctl.key.v & 0x0020)>>5;
		decode_the_key.key_q     = (RC_Ctl.key.v & 0x0040)>>6;
		decode_the_key.key_e     = (RC_Ctl.key.v & 0x0080)>>7;
    decode_the_key.key_r     = (RC_Ctl.key.v & 0x0100)>>8;
    decode_the_key.key_f     = (RC_Ctl.key.v & 0x0200)>>9;
    decode_the_key.key_g     = (RC_Ctl.key.v & 0x0400)>>10;
    decode_the_key.key_z     = (RC_Ctl.key.v & 0x0800)>>11;
    decode_the_key.key_x     = (RC_Ctl.key.v & 0x1000)>>12;
    decode_the_key.key_c     = (RC_Ctl.key.v & 0x2000)>>13;
    decode_the_key.key_v     = (RC_Ctl.key.v & 0x4000)>>14;
    decode_the_key.key_b     = (RC_Ctl.key.v & 0x8000)>>15;
		/*---------------------------�������---------------------------------*/	
	
		/*-------------------���switch_left switch_right û�н������ݣ�˵�������ܷ�------------------*/
		if((RC_Ctl.rc.switch_left != 1)&&(RC_Ctl.rc.switch_left != 2)&&(RC_Ctl.rc.switch_left != 3))
		{
				delay_ms(1);
				//reset CPU
				__set_FAULTMASK(1);
				NVIC_SystemReset();
		}
		if((RC_Ctl.rc.switch_right != 1)&&(RC_Ctl.rc.switch_right != 2)&&(RC_Ctl.rc.switch_right != 3))
		{
				delay_ms(1);
				//reset CPU
				__set_FAULTMASK(1);
				NVIC_SystemReset();
		}
		/*-----------------------------��ҪӦ�ÿ��Ʋ������-------------------------*/
	  //compute_key_time_of_underpan();//���̰���ʱ����㺯��

		if(decode_the_key.key_x == 1 && decode_the_key_last.key_x == 0) 
		{
				if(MCLSwitch==0  && BulletLid==0)
				{
						MCLSwitch = 1;
						FireModel = OneClickModel;
				}
				else if(MCLSwitch==1)
				{
						MCLSwitch = 0;
				}
		}
		
		if((decode_the_key.key_b ) && (decode_the_key_last.key_b == 0))
    {
        if(FireModel == OneClickModel)        FireModel = SlowContModel;
        else
        {
            FireModel = OneClickModel;
            GunInitAngelValue = Gun_Real_Angle_Value;
        }
        //��Ħ����δ����ʱ�ſ��Դ򿪵��ָ�
        if(MCLSwitch == 0)
        {
            if(BulletLid == 0)              
                BulletLid = 1;
            else           
                BulletLid = 0;         
        }
    }
		
		
		static float ResetCheckSconds = 0.0f;
		if(decode_the_key.key_ctrl == 1)
		{
				ResetCheckSconds += 0.001f;
		}
		else
		{
				ResetCheckSconds = 0;
		}

		if(decode_the_key.key_shift==1 && decode_the_key.key_shift==0)
		{
				if(KeyBordSpeed == 500)
						KeyBordSpeed = 630;
				else
						KeyBordSpeed = 500;
		}
		
		if((decode_the_key.key_z==1)&&(decode_the_key_last.key_z==0)) attack_direction = !attack_direction;
		
		UartAuxiFlag = decode_the_key.key_g;																											//�ߵ�ƽ����Сװ�װ�
		if(decode_the_key.key_g && decode_the_key_last.key_g == 0)    {SendToPc(4);}							//�Ӿ�����װ�װ�ģʽ
		else if(decode_the_key.key_g == 0 && decode_the_key_last.key_g == 1)    {SendToPc(0);}		//�Ӿ��˳�װ�װ�ģʽ
		
		UartAuxiFlag = decode_the_key.key_f;																											//�ߵ�ƽ������װ�װ�
		if(decode_the_key.key_f && decode_the_key_last.key_f == 0)    {SendToPc(1);}							//�Ӿ�����װ�װ�ģʽ
		else if(decode_the_key.key_f == 0 && decode_the_key_last.key_f == 1)    {SendToPc(0);}		//�Ӿ��˳�װ�װ�ģʽ
		
		UartBuffFlag = decode_the_key.key_c;																											//�ߵ�ƽ����Сbuffģʽ
		if(decode_the_key.key_c && decode_the_key_last.key_c == 0)    														//�Ӿ�����Сbuffģʽ
			{
				SendToPc(2);
				radio_pitch_control_data = 0;
				buff_init_angle_yaw = radio_turn_control_data;
				buff_init_angle_pit = radio_pitch_control_data;
			}
		else if(decode_the_key.key_c == 0 && decode_the_key_last.key_c == 1)    {SendToPc(0);}		//�Ӿ��˳�Сbuffģʽ
		
		UartBuffFlag = decode_the_key.key_v;																											//�ߵ�ƽ�����buffģʽ
		if(decode_the_key.key_v && decode_the_key_last.key_v == 0)    														//�Ӿ������buffģʽ
			{
				SendToPc(3);
				radio_pitch_control_data = 0;
				buff_init_angle_yaw = radio_turn_control_data;
				buff_init_angle_pit = radio_pitch_control_data;
			}
		else if(decode_the_key.key_v == 0 && decode_the_key_last.key_v == 1)    {SendToPc(0);}		//�Ӿ��˳���buffģʽ
		
		static float ReBootDelay = 0.0f;
		if(decode_the_key.key_r==1)
				ReBootDelay += 0.1f;
		else
				ReBootDelay = 0.0f;
		
		if(ReBootDelay >= 3)
		{
				delay_ms(1);
				//reset CPU
				__set_FAULTMASK(1);
				NVIC_SystemReset();
		}
	}
	if(all_init)
			Upper_part();
}

/*
 * ��������Upper_part
 * ����  ��SUBS���ջ����յ��Ľ���������-->ս��ʵ��ǰ�����ҵȿ������ݺϳ�
 * ����  ����
 * ���  ����
 */ 

void Upper_part(void)
{   
		yaokong_Ctrl();
	
		keyboard_Ctrl();
	
    radio_yaw   = Uppertoctl.radio_yaw_remote*radio_yaw_sensitivity 	//ң��
                + Uppertoctl.radio_turn_data_keyboard									//����
                + Uppertoctl.radio_yaw_mouse;													//���

	
    radio_pitch = -(Uppertoctl.radio_pitch_remote*radio_pitch_sensitivity
								+ Uppertoctl.radio_pitch_mouse);

    radio_ahead_back_data = Uppertoctl.radio_ahead_back_data_remote*remote_sensitivity_x 
                          + Uppertoctl.radio_ahead_back_data_keyboard;
    radio_ahead_back_data = radio_ahead_back_data>KeyBordSpeed?KeyBordSpeed:radio_ahead_back_data;
    radio_ahead_back_data = radio_ahead_back_data<-KeyBordSpeed?-KeyBordSpeed:radio_ahead_back_data;
    
    radio_left_right_data = Uppertoctl.radio_left_right_data_remote*remote_sensitivity_y 
                          + Uppertoctl.radio_left_right_data_keyboard;
    radio_left_right_data = radio_left_right_data>KeyBordSpeed?KeyBordSpeed:radio_left_right_data;
    radio_left_right_data = radio_left_right_data<-KeyBordSpeed?-KeyBordSpeed:radio_left_right_data;
    
		radio_pitch_control_data += radio_pitch/240 ;
		if(RecieveAuxiliaryflag)
		{radio_pitch_control_data -= Auxi_pitch/7;}

		radio_pitch_control_data = (radio_pitch_control_data> 35)? 35:radio_pitch_control_data;
    radio_pitch_control_data = (radio_pitch_control_data<-35)?-35:radio_pitch_control_data;
		
    radio_turn_control_data += radio_yaw / 120;
		if(RecieveAuxiliaryflag)
		{radio_turn_control_data += Auxi_yaw/7;RecieveAuxiliaryflag = 0;}
		
		yaw_limite = mid_mpu_9250_yaw_angle - mpu_9250_init_angle - (Yaw_Angle_Value - Yaw_Center_Angle);
		yaw_limite = GetMedianNum(yaw_limite_flt,6,yaw_limite);
		radio_turn_control_data = radio_turn_control_data > yaw_limite + 65 ?  yaw_limite + 65 : radio_turn_control_data;
		radio_turn_control_data = radio_turn_control_data < yaw_limite - 65 ?  yaw_limite - 65 : radio_turn_control_data;
}
int switch_left_position;
int switch_right_position;
//0x41 red  0x42 blue

void yaokong_Ctrl(void)
{ 
	switch_left_position=RC_Ctl.rc.switch_left;  
	switch_right_position=RC_Ctl.rc.switch_right; 
    
	if(switch_right_position == 3 && BlueOrRedChoose == FightRed)
    {
        BlueOrRedChoose = FightBlue;
        USART3_SendChar(0x41);
    }
    if(switch_right_position == 2 && BlueOrRedChoose == FightBlue)
    {
        BlueOrRedChoose = FightRed;
        USART3_SendChar(0x42);
    }
    
	Uppertoctl.radio_ahead_back_data_remote = my_deathzoom( ( RC_Ctl.rc.ch1 - 1024 ),25);//ǰ���ƶ�����Ӧ����ws
	Uppertoctl.radio_left_right_data_remote = my_deathzoom((RC_Ctl.rc.ch0 - 1024),25);//���Һ��ƣ���Ӧ����ad
	Uppertoctl.radio_yaw_remote =my_deathzoom(-(RC_Ctl.rc.ch2 - 1024),25);//��̨����ת����Ӧ��������ƶ�	
	Uppertoctl.radio_pitch_remote =my_deathzoom((RC_Ctl.rc.ch3 - 1024),25);//��̨����ת����Ӧ��������ƶ�
}


short ahead_back_temp = 0;
short left_right_temp = 0;
short turn_temp = 0;
short mouse_pitch_mouse = 0;


void keyboard_Ctrl(void)
{
/*---------------��̨���������������Ƿ��ģ�------------------*/
		//������������ ��ֵ��ʾ���ƶ�  ��ֵ������ƶ�
		Uppertoctl.radio_yaw_mouse    =(-RC_Ctl.mouse.x * mouse_sensitivity_x);//��̨����ת����Ӧ��������ƶ� 
    Uppertoctl.radio_pitch_mouse  =(-RC_Ctl.mouse.y * mouse_sensitivity_y);//��̨����ת����Ӧ��������ƶ�
/*-------------------------���̿���-------------------------*/	
		// ǰ���˶���ͬʱ��ws������Ϊ����
			
		if((decode_the_key.key_w + decode_the_key.key_s) ==1)  //δͬʱ����ws
		{
					
				if(decode_the_key.key_w) 
				{
						ahead_back_temp+=10;
						if(ahead_back_temp>KeyBordSpeed)  ahead_back_temp = KeyBordSpeed;
				}
				else if(decode_the_key.key_s) 
				{
						ahead_back_temp-=10;
						if(ahead_back_temp<-KeyBordSpeed)  ahead_back_temp = -KeyBordSpeed;
				}
		 }
			
		else 
    {
        if(ahead_back_temp>0) 
				{
						//ahead_back_temp -=20;
						//if(ahead_back_temp<0) 
					ahead_back_temp = 0;
				}
				if(ahead_back_temp<0) 
				{
						//ahead_back_temp +=20;
						//if(ahead_back_temp>0) 
					ahead_back_temp = 0;
				}
    }
		Uppertoctl.radio_ahead_back_data_keyboard = ahead_back_temp;  
	
		// �����˶�,ͬʱ��ad������Ϊ����
			
		if((decode_the_key.key_a + decode_the_key.key_d)==1) //δͬʱ����ad
		{
				if(decode_the_key.key_d) 
				{
						left_right_temp+=10;
						if(left_right_temp>0.7f*KeyBordSpeed)  left_right_temp =  0.7f*KeyBordSpeed;
				}
				else if(decode_the_key.key_a) 
				{
						left_right_temp-=10;
						if(left_right_temp<-0.7f*KeyBordSpeed)  left_right_temp = -0.7f*KeyBordSpeed;
				}
		 }
			
		else 
    {
        if(left_right_temp>0) 
		{
				//left_right_temp -=20;
				//if(left_right_temp<0) 
			left_right_temp = 0;
		}
        if(left_right_temp<0) 
		{
				//left_right_temp +=20;
				//if(left_right_temp>0) 
			left_right_temp = 0;
		}
        
    }
		Uppertoctl.radio_left_right_data_keyboard = left_right_temp; 
  
		if(Uppertoctl.radio_yaw_mouse>350)	turn_temp +=350;
		else if(Uppertoctl.radio_yaw_mouse<-350) turn_temp -=350;
		else turn_temp += Uppertoctl.radio_yaw_mouse;
		
		//������ת��ͬʱ��qe������Ϊ����
		if((decode_the_key.key_q + decode_the_key.key_e) ==1)  //δͬʱ����qe
		{
				if(decode_the_key.key_q) 
				{
						if(decode_the_key.key_s)
								turn_temp -=10;
						else
								turn_temp +=10;
				}
				else if(decode_the_key.key_e) 
				{
						if(decode_the_key.key_s)
								turn_temp +=10;
						else
								turn_temp -=10;
				}
		}
		else 
    {
				if(turn_temp>0) 
				{
						turn_temp -=20;
						if(turn_temp<0) turn_temp = 0;
				}
				if(turn_temp<0) 
				{
						turn_temp +=20;
						if(turn_temp>0) turn_temp = 0;
				}
        
    }
    
		if(turn_temp>0.8f*KeyBordSpeed)   turn_temp =  0.8f*KeyBordSpeed;
		if(turn_temp<-0.8f*KeyBordSpeed)  turn_temp = -0.8f*KeyBordSpeed;
			
		Uppertoctl.radio_turn_data_keyboard =  turn_temp;   
 }


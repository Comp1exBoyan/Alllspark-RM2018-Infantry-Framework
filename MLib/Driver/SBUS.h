#ifndef __SBUS_H__
#define __SBUS_H__

#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#include "filter.h"

#define T_USART1  1      
#define ZK_USART2  0


/*------------------------��Ҫ�õ��Ŀ���ȫ�ֲ���--------------------------------*/
extern float radio_yaw; //��̨����ת����Ӧ��������ƶ�
extern float radio_pitch; //��̨����ת����Ӧ��������ƶ�
extern float radio_ahead_back_data;
extern float radio_left_right_data;
extern float radio_turn_data;
extern float radio_turn_control_data;
extern float radio_pitch_control_data;
extern short mouse_pitch_mouse;

extern float buff_init_angle_yaw;
extern float buff_init_angle_pit;

extern int s1_position;
extern int s2_position;
extern u16 keyTimeOfUnderpan;  //�������µ�ʱ��

extern u8 separate_flag ;//��̨���̷�����Ʊ�־λ
extern u8 auxiliary_flag;//shift�ߵ�ƽ����������׼
extern u8 buff_flag;
extern u8 Left45Degree_flag;// ��45�ȵ��̸���
extern u8 Righ45Degree_flag;// ��45�ȵ��̸���
extern u8 attack_direction;	// ���̷���
/*---------------------------���ݴ����õ��Ĳ���---------------------------------------------*/
/*������*/
#define remote_sensitivity_x  	1.25f        //ң���������ȣ���ֵԽ��Խ����
#define remote_sensitivity_y  	1.25f        //ң���������ȣ���ֵԽ��Խ����
#define radio_yaw_sensitivity 	1.54f        //ң���������ȣ���ֵԽ��Խ����
#define radio_pitch_sensitivity 1.54f      	 //ң���������ȣ���ֵԽ��Խ����
#define turn_sensitivity				0.10f				 //	 ��ת�����ȣ���ֵԽ��Խ����

#define mouse_sensitivity_x   5	       //��������ȣ���ֵԽ��Խ����
#define mouse_sensitivity_y   34       //��������ȣ���ֵԽ��Խ����

#define Remote_Ctrl           1320     //ң����ͨ����ֵ 1648-364 
#define Remote_Ctrl_mouse     12000    //���ͨ����

#define FightRed    0
#define FightBlue   1
/*-----------------------------------------------------------------------------------------*/
typedef struct _usetoctl
	{
   float radio_yaw_remote;
   float radio_yaw_mouse;//yaw

   float radio_pitch_remote;
   float radio_pitch_mouse;//pitch
		
   int mouse_state_keyboard;
   int mouse_state_remote;

   float radio_turn_data_follow;//����

   float radio_ahead_back_data_keyboard;//���̿���
   float radio_left_right_data_keyboard;
   float radio_turn_data_keyboard;
   float radio_turn_data_mouse;
   float turn_flag_keyboard;

   float radio_ahead_back_data_remote;//ң�ؿ���
   float radio_left_right_data_remote;
   float radio_turn_data_remote; 
	 
   float go_straight_leftright_flag_keyboard;//������������
   float go_straight_aheadback_flag_keyboard;//����ǰ������
   float go_straight_aheadback_flag_remote;//ң��ǰ����ֱ
   float go_straight_leftright_flag_remote;//ң��������ֱ	 
	} usetoctl;

struct RC_Ctl_t
{
	struct
	{
		uint16_t ch0;
		uint16_t ch1;
		uint16_t ch2;
		uint16_t ch3;
		uint8_t switch_left;
		uint8_t switch_right;
	}rc;
	
	struct
	{
		int16_t x;
		int16_t y;
		int16_t z;
		uint8_t press_l;
		uint8_t press_r;
	}mouse;
	
	struct
	{
		uint16_t v;
	}key;
};

typedef struct _decode_keyboard
	{
		u8 key_none;
		u8 key_w;
		u8 key_a;
		u8 key_s;
		u8 key_d;
		u8 key_q;
		u8 key_e;
		u8 key_ctrl;
		u8 key_shift; 
		u8 key_r;
		u8 key_f;
		u8 key_g;
		u8 key_z;
		u8 key_x;
		u8 key_c;
		u8 key_v;
		u8 key_b;
	} decode_keyboard;
	
extern usetoctl Uppertoctl;
extern struct RC_Ctl_t RC_Ctl;
extern struct RC_Ctl_t Laster_RC_Ctl;
extern decode_keyboard decode_the_key;
extern decode_keyboard decode_the_key_last;
extern volatile unsigned char sbus_rx_buffer[25];
extern int switch_left_position;
extern int switch_right_position;	
	
void USART2_Configuration(void);
void USART1_Configuration(void);
void data_decoder(void);
void Upper_part(void);
void yaokong_Ctrl(void);
void keyboard_Ctrl(void);
//void compute_key_time_of_underpan(void);

#endif

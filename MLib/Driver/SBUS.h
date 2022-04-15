#ifndef __SBUS_H__
#define __SBUS_H__

#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#include "filter.h"

#define T_USART1  1      
#define ZK_USART2  0


/*------------------------主要用到的控制全局参数--------------------------------*/
extern float radio_yaw; //云台左右转，对应鼠标左右移动
extern float radio_pitch; //云台俯仰转，对应鼠标上下移动
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
extern u16 keyTimeOfUnderpan;  //按键按下的时间

extern u8 separate_flag ;//云台底盘分离控制标志位
extern u8 auxiliary_flag;//shift高电平触发辅助瞄准
extern u8 buff_flag;
extern u8 Left45Degree_flag;// 左45度底盘跟随
extern u8 Righ45Degree_flag;// 右45度底盘跟随
extern u8 attack_direction;	// 底盘方向
/*---------------------------数据处理用到的参数---------------------------------------------*/
/*灵敏度*/
#define remote_sensitivity_x  	1.25f        //遥控器灵敏度，数值越高越灵敏
#define remote_sensitivity_y  	1.25f        //遥控器灵敏度，数值越高越灵敏
#define radio_yaw_sensitivity 	1.54f        //遥控器灵敏度，数值越高越灵敏
#define radio_pitch_sensitivity 1.54f      	 //遥控器灵敏度，数值越高越灵敏
#define turn_sensitivity				0.10f				 //	 旋转灵敏度，数值越高越灵敏

#define mouse_sensitivity_x   5	       //鼠标灵敏度，数值越高越灵敏
#define mouse_sensitivity_y   34       //鼠标灵敏度，数值越高越灵敏

#define Remote_Ctrl           1320     //遥控器通道差值 1648-364 
#define Remote_Ctrl_mouse     12000    //鼠标通道差

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

   float radio_turn_data_follow;//跟随

   float radio_ahead_back_data_keyboard;//键盘控制
   float radio_left_right_data_keyboard;
   float radio_turn_data_keyboard;
   float radio_turn_data_mouse;
   float turn_flag_keyboard;

   float radio_ahead_back_data_remote;//遥控控制
   float radio_left_right_data_remote;
   float radio_turn_data_remote; 
	 
   float go_straight_leftright_flag_keyboard;//键盘左右走至
   float go_straight_aheadback_flag_keyboard;//键盘前后走至
   float go_straight_aheadback_flag_remote;//遥控前后走直
   float go_straight_leftright_flag_remote;//遥控左右走直	 
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

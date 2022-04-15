#ifndef __USART3_H__
#define __USART3_H__

#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <main.h>

#define EN_USART3_RX		  1
#define EN_USART2_RX          1

#define EXIT    0    // 0x30
#define AXUXARM 1    // 0x41
#define SMLBUFF 2    // 0x31 
#define BIGBUFF 3    // 0x32
#define BLUEARM 4    // 0x42
#define SENDPOS 5		 // 0x33
 
extern u8    UartAuxiFlag;
extern u8    UartBuffFlag;
extern u8    RecieveAuxiliaryflag;
extern u8    RecieveBuffflag;
extern float Auxi_yaw, Auxi_pitch;
extern float Buff_yaw, Buff_pitch;
extern float Buff_yaw_store, Buff_pitch_store;

void USART3_Configuration(u32 bound);
void USART3_SendChar(unsigned char b);
void SendToPc(u8 model);

#endif

#ifndef __YUNTAIINT_H__
#define __YUNTAIINT_H__

#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stm32f4xx_spi.h>

#include "filter.h"
#include "SBUS.h"
#include "can1.h"
#include "can2.h"
#include "imuint.h"
#include "Pid_Regulator.h"

//µ•∂¿Ω«∂» ± P 20   D120
//66   166
#define YUNTAI_YAW_ANGLE_PID_DEFAULT \
{\
    0,\
	0,\
    {0,0},\
    80,\
    0,\
    40,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    PID_OUTPUT_MAX_LIMIT,\
}\

//14.3f   60

#define YUNTAI_YAW_SPEED_PID_DEFAULT \
{\
    0,\
	0,\
    {0,0},\
    4,\
    0,\
    5,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    PID_OUTPUT_MAX_LIMIT,\
}\

#define YUNTAI_YAW_ANGLE_PID_BAIWEI \
{\
    0,\
	0,\
    {0,0},\
    77,\
    0,\
    300,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    PID_OUTPUT_MAX_LIMIT,\
}\

#define YUNTAI_YAW_ANGLE_PID_FOLLOWDP \
{\
    0,\
		0,\
    {0,0},\
    40,\
    0.0f,\
    400,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    PID_OUTPUT_MAX_LIMIT,\
}\

#define YUNTAI_YAW_SPEED_PID_FOLLOWDP \
{\
    0,\
		0,\
    {0,0},\
    4.0f,\
    0,\
    2,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    PID_OUTPUT_MAX_LIMIT,\
}\

#define YUNTAI_PITCH_ANGLE_PID_DEFAULT \
{\
    0,\
		0,\
    {0,0},\
    110,\
    1,\
    1000,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    PID_OUTPUT_MAX_LIMIT,\
}\

#define YUNTAI_PITCH_SPEED_PID_DEFAULT \
{\
    0,\
		0,\
    {0,0},\
    2.0f,\
    0.004f,\
    30.0f,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    200000,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    PID_OUTPUT_MAX_LIMIT,\
}\

extern short YawCenterValue;
extern short PitCenterValue;
extern float FollowYawAngle;
extern u8      YawInitFlag;
u8 yuntai_int(void);

#endif 

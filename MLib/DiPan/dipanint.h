#ifndef __DIPANINT_H__
#define __DIPANINT_H__
#include "dipanint.h"
#include "dipanctl.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stm32f4xx_spi.h>

#include "filter.h"
#include "SBUS.h"
#include "mymath.h"
#include "Pid_Regulator.h"
#include "can1.h"
#include "dipanctl.h"

#define DIPAN_MOTOR_ANGLE_PID_DEFAULT \
{\
    0,\
	0,\
    {0,0},\
    20,\
    0,\
    1,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    PID_OUTPUT_MAX_LIMIT,\
}\

#define DIPAN_VIRTUAL_ANGLE_PID_DEFAULT \
{\
    0,\
	0,\
    {0,0},\
    5,\
    0,\
    10,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    2000,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    1000,\
}\

#define DIPAN_VIRTUAL_GYPO_PID_DEFAULT \
{\
    0,\
	0,\
    {0,0},\
    12.0f,\
    0,\
    30,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    9000,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    1000,\
}\

#define DIPAN_MOTOR_SPEED_PID_DEFAULT \
{\
    0,\
		0,\
    {0,0},\
    20,\
    0.0,\
    10,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    9000,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    PID_OUTPUT_MAX_LIMIT,\
}\

u8 dipan_int(void);

#endif 

#ifndef __INIT_H__
#define __INIT_H__
//---------Other-------------
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_iwdg.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "stm32f4xx_i2c.h"
//---------Driver-------------
#include "led.h"
#include "iwdg.h"
#include "key.h"
#include "delay.h"
//---------Task------------
#include "filter.h"
#include "mymath.h"
//---------dipan------------
#include "dipanint.h"
//---------yuntai----------
#include "yuntaiint.h"
#include "yuntaictl.h"
//---------IMU-------------
#include "imuint.h"
//---------SBUS------------
#include "SBUS.h"
//---------Weapon----------
#include "Weapon.h"
////---------FreeCars---------
//#include "FreeCarsInt.h"
#include "FreeCars.h"
u8 ctrl_Init(void);

#endif 


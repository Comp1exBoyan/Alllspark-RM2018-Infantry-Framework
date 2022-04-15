#ifndef __WEAPON__H
#define __WEAPON__H

#include "stm32f4xx.h"

#define WEAPON_MC_SPEED_PID_DEFAULT \
{\
    0,\
	0,\
    {0,0},\
    8,\
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

#define WEAPON_BL_SPEED_PID_DEFAULT \
{\
    0,\
	0,\
    {0,0},\
    30,\
    0,\
    80,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    8000,\
}

#define WEAPON_BL_ANGLE_PID_DEFAULT \
{\
    0,\
	0,\
    {0,0},\
    166,\
    0,\
    166,\
    0,\
    0,\
    0,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    PID_COMPONENT_NOLIMIT,\
    0,\
    8000,\
}
#define OneClickModel   0
#define SlowContModel   1
#define YKBackZDModel   2

#define OneClickOneFire 22.5f
#define OneClickTwoFire OneClickOneFire*2

#define Speed10MPerSecond 2500
#define Speed15MPerSecond 5000
#define Speed20MPerSecond 6200
#define Speed25MPerSecond 7200

#define Freq2CPerSecond   1500   
#define Freq5CPerSecond   1400 
#define Freq8CPerSecond   2000

extern s8      checkIsFire;
extern u8      FireModel;
extern u8      MCLSwitch;
extern u8      BulletNum;
extern u8      BulletLid;
extern int16_t MC_PID_Output1;
extern int16_t MC_PID_Output2;
extern int16_t BLN_Speed_PID_Output;
extern int16_t BLN_Angle_PID_Output;
extern float SelfCalcShootHeat;
extern float GunInitAngelValue;

void WeaponInit(void);
void WeaponControl(float timetick);
void WeaponSpeedControl(float timetick);
void WeaponClickControl(float timetick);
void BLForwardTurn(void);
void BLPosRunCheck(void);
void BLNegRunCheck(void);
void BLReverseTurn(void);

#endif

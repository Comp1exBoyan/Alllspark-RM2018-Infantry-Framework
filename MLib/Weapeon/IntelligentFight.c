#include "IntelligentFight.h"

float Intelligent_ShootSped = 0.0f;
float Intelligent_ShootFreq = 0.0f;

void IntelligentControl(u8 distance,u8 RobotLevel)
{
    //根据距离装甲板的距离调整射速，射频
    u8 x = distance;
    if( RobotLevel == RobotLevelOne)
    {
        Intelligent_ShootSped = -1.794f*x*x*x+10.81f*x*x-10.2f*x+12.67f;
        Intelligent_ShootFreq =  1.414f*x*x*x-5.851f*x*x+3.71f*x+5.704f;
    }
    
    if( RobotLevel == RobotLevelTwo)
    {
        Intelligent_ShootSped = -0.216f*x*x*x+3.536f*x*x-1.680f*x+11.87f;
        Intelligent_ShootFreq =  0.960f*x*x*x-3.681f*x*x-0.258f*x+10.07f;
    }
    
    if( RobotLevel == RobotLevelOne)
    {
        Intelligent_ShootSped = -0.334f*x*x*x-1.191f*x*x+7.516f*x+10.3f;
        Intelligent_ShootFreq =  1.487f*x*x*x-6.739f*x*x+5.155f*x+8.82f;
    }
       
}
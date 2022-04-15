#include "Pid_Regulator.h"
#include "IntelligentFight.h"
#include "JudgeSystemN.h"
#include "Weapon.h"
#include "SBUS.h"
#include "can1.h"
#include "can2.h"
#include "mymath.h"
#include "led.h"

//MC1 表示面向方向为发弹方向的左边摩擦轮 MC2为右边摩擦轮
PID_Regulator WeaMC1SpeedPid = WEAPON_MC_SPEED_PID_DEFAULT;
PID_Regulator WeaMC2SpeedPid = WEAPON_MC_SPEED_PID_DEFAULT;
PID_Regulator WeaBLNSpeedPid = WEAPON_BL_SPEED_PID_DEFAULT;
PID_Regulator WeaBLNAnglePid = WEAPON_BL_ANGLE_PID_DEFAULT;


u8    YKBullLik=0;
u8    FireModel=0;
u8    MCLSwitch=0;
u8    RbkSwitch=0;
u8    FirSwitch=0;
u8    LasSwitch=3;
u8    BulletNum=0;
u8    BulletLid=0;
s8    checkIsFire = 0;

float BAddAngle=OneClickOneFire;
float LianxuAngle=60;

short BLState=-1;
short BLIsRun=-1;

short ReverseCount  = 0;
short MCTargetSpeed = 4000;
float GunInitAngelValue=0;

float FiTargetSpeed = 3000;
float BLTargetAngle = 0;
float FiTargetAngle = 0;

int16_t MC_PID_Output1;
int16_t MC_PID_Output2;
int16_t BLN_Speed_PID_Output;
int16_t BLN_Angle_PID_Output;

short MCLSpeedSUM=0;

short ShootFreChoose[3] = {0,Freq2CPerSecond,Freq8CPerSecond};
short MCLSpeedChoose[3] = {Speed20MPerSecond,Speed15MPerSecond,Speed20MPerSecond};

short HeatUplimit[3] = {90,180,360};
short HeatCooling[3] = {18,36,72};

float SelfCalcShootHeat = 0.0f;
float oneclickcount=0;

void WeaponInit(void)
{
    
    GPIO_InitTypeDef          gpio;
    TIM_TimeBaseInitTypeDef   tim;
    TIM_OCInitTypeDef         oc;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	/*time8 CH1 CH2 用作PWM输出控制舵机*/
    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOI,&gpio);
    
    GPIO_PinAFConfig(GPIOI,GPIO_PinSource5, GPIO_AF_TIM8);
    GPIO_PinAFConfig(GPIOI,GPIO_PinSource6, GPIO_AF_TIM8);  
    
    tim.TIM_Prescaler = 168-1;//1M
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = 20000;   //20ms
    tim.TIM_ClockDivision = TIM_CKD_DIV1;//0
    TIM_TimeBaseInit(TIM8,&tim);
    
    oc.TIM_OCMode = TIM_OCMode_PWM2;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OutputNState = TIM_OutputState_Disable;
    oc.TIM_Pulse = 0;
    oc.TIM_OCPolarity = TIM_OCPolarity_Low;
    oc.TIM_OCNPolarity = TIM_OCPolarity_High;
    oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
    oc.TIM_OCNIdleState = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM8,&oc);
    TIM_OC2Init(TIM8,&oc);
    
    TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM8,TIM_OCPreload_Enable);
		
		TIM_ARRPreloadConfig(TIM8,ENABLE);
    TIM_CtrlPWMOutputs(TIM8,ENABLE);
    TIM_Cmd(TIM8,ENABLE);
    
    //武器Pid初始化，控制模式初始化，初始角度初始化
    WeaMC1SpeedPid.tar = MCTargetSpeed;
    WeaMC2SpeedPid.tar =-MCTargetSpeed;
    FireModel = OneClickModel;
    GunInitAngelValue  = Gun_Real_Angle_Value;
}

void WeaponControl(float timetick)
{   
    static float FirModelChooseDelay = 0.0f;
		
    if(switch_left_position == 1)
    {
        YKBullLik = 1;
    }
    else
    {
        YKBullLik = 0;
    }
    
    //弹舱盖PWM控制，不能使用阶跃值，力气不够
    static short pwmValue = 1250;
    if(BulletLid==1 || YKBullLik==1)
    {
        pwmValue ++;
        pwmValue = pwmValue>2500?2500:pwmValue;
        TIM_SetCompare1(TIM8,pwmValue);
        
        //弹舱盖打开时激光灯闪烁
        static u32 LidDecond = 0;
        LidDecond++;
        if(LidDecond == 200)
            LaserOn();
        if(LidDecond == 400)
        {
            LaserOff();
            LidDecond = 0;
        }
    }
    else
    {
         pwmValue --;
         pwmValue = pwmValue<1250?1250:pwmValue;
         TIM_SetCompare1(TIM8,pwmValue);
    }
    
    //遥控退子弹模式
    if(switch_left_position == 2 && switch_right_position == 2)
    {
        MCLSwitch = 1;
        FireModel = YKBackZDModel; // 遥控直接退子弹
    }
    else if(FireModel == YKBackZDModel)
    {
        MCLSwitch = 0;
        FireModel = OneClickModel;
    }
    
    FiTargetSpeed = ShootFreChoose[FireModel];
    MCTargetSpeed = MCLSpeedChoose[FireModel];
    
    //检测摩擦轮是否已经正常运转
    MCLSpeedSUM = my_abs(Weapon_Real_Speed_Value[0])+my_abs(Weapon_Real_Speed_Value[1]);
    
    if(MCLSwitch == 1 && MCLSpeedSUM>500)
    {       
        if(checkIsFire==0 && MCLSpeedSUM>2*MCTargetSpeed-350)
        {
            checkIsFire = 1;
            LaserOn();
        }         
    }
    else 
    {
        if(BulletLid == 0)
            LaserOff();
        checkIsFire = 0;
    }
    
    //利用摩擦轮的速降检测点发模式子弹发射
    if(checkIsFire>1 && MCLSpeedSUM<2*MCTargetSpeed-250)
    {
        SelfCalcShootHeat += 20;
        checkIsFire = -1;
        BulletNum ++;
    }
    static float checkContinueFire = 0.0f; 
    if( FireModel == SlowContModel && checkIsFire==1 && MCLSpeedSUM<2*MCTargetSpeed-350)
    {
        if(secTime-checkContinueFire>0.2f && RC_Ctl.mouse.press_l==1)
        {
            BulletNum++;
            SelfCalcShootHeat += 13;
            checkContinueFire = secTime;
        }
    }
    WeaMC1SpeedPid.tar =  MCTargetSpeed;
    WeaMC2SpeedPid.tar = -MCTargetSpeed;
    WeaMC1SpeedPid.cur = Weapon_Real_Speed_Value[0];
    WeaMC2SpeedPid.cur = Weapon_Real_Speed_Value[1];
    
    PID_Calc(&WeaMC1SpeedPid,timetick);
    PID_Calc(&WeaMC2SpeedPid,timetick);
    
    //MC1 逆时针转  MC2顺时针转
    MC_PID_Output1=MCLSwitch==1?WeaMC1SpeedPid.output:0;
    MC_PID_Output2=MCLSwitch==1?WeaMC2SpeedPid.output:0;
    
    if(JudgeDataFlag == 1)
    {
        SelfCalcShootHeat = ShooterHeat;
        JudgeDataFlag = 0;
    }
    
    WeaponClickControl(timetick);
    WeaponSpeedControl(timetick);

}

void WeaponSpeedControl(float timetick)
{
    //if(HeatUplimit[RobotLevel-1]-25 < SelfCalcShootHeat )    {BLN_Speed_PID_Output = 0;return;}
    if(my_abs(Weapon_Real_Speed_Value[0])<100)        {BLN_Speed_PID_Output = 0;return;}
    if(my_abs(Weapon_Real_Speed_Value[1])<100)        {BLN_Speed_PID_Output = 0;return;}
    if(FireModel == OneClickModel && BLN_Angle_PID_Output == 0)  {BLN_Speed_PID_Output = 0;return;}
    
    WeaBLNSpeedPid.cur = Gun_Real_Speed_Value;
   
    if(FireModel == OneClickModel)
    {
        if(RC_Ctl.mouse.press_l==1&&Laster_RC_Ctl.mouse.press_l==0)
				{
					oneclickcount=0;
				}
				
          WeaBLNSpeedPid.tar =  3000;
					        oneclickcount++;
					  //oneclickcount++;
				
        
        
        PidCalcNoI(&WeaBLNSpeedPid,timetick);
        BLN_Speed_PID_Output = WeaBLNSpeedPid.output;
				
        if( oneclickcount>50)
				{
					BLN_Speed_PID_Output=0;
					
				}
    }
    else if(FireModel == SlowContModel) 
    {   
        if(RC_Ctl.mouse.press_l==1)
            WeaBLNSpeedPid.tar =  FiTargetSpeed;
        else if(RC_Ctl.mouse.press_r==1)
            WeaBLNSpeedPid.tar = -FiTargetSpeed;
        
        PID_Calc(&WeaBLNSpeedPid,timetick);
        BLN_Speed_PID_Output = WeaBLNSpeedPid.output;
        
        if(RC_Ctl.mouse.press_l + RC_Ctl.mouse.press_r ==0)    BLN_Speed_PID_Output = 0;
    }   
    else if(FireModel == YKBackZDModel)
    {
        WeaBLNSpeedPid.tar =  FiTargetSpeed;
        PID_Calc(&WeaBLNSpeedPid,timetick);
        BLN_Speed_PID_Output =WeaBLNSpeedPid.output;
    }
    
}
void WeaponClickControl(float timetick)
{
    if(FireModel != OneClickModel ) {BLN_Angle_PID_Output = 0;return;}
    if( SelfCalcShootHeat > HeatUplimit[RobotLevel-1] - 30 ) {BLN_Angle_PID_Output=0;return;}
    if(my_abs(Weapon_Real_Speed_Value[0])<100) {BLN_Angle_PID_Output = 0;return;}
    if(my_abs(Weapon_Real_Speed_Value[1])<100) {BLN_Angle_PID_Output = 0;return;}
    
    LasSwitch = FirSwitch+RbkSwitch; 
    FirSwitch = RC_Ctl.mouse.press_l;  // 鼠标左键按下时发射子弹
    RbkSwitch = RC_Ctl.mouse.press_r;  // 鼠标右键按下时回转
    
    // 仅当上一时刻发射子弹开关关闭 现在开关打开时子弹发射
    if(FirSwitch==1 && LasSwitch==0 && (BLIsRun<=0 || BLIsRun==3))    BLIsRun=1;
    if(RbkSwitch==1 && LasSwitch==0 && (BLIsRun<=0 || BLIsRun==2))    BLReverseTurn();
    if(BLIsRun==0)
    {
        BLTargetAngle=FiTargetAngle;
        BLIsRun=my_abs(Gun_Real_Speed_Value)<500?-1:0; 
    }   
    if(BLIsRun==1)     BLForwardTurn();
    if(BLIsRun==2)     BLPosRunCheck();
    if(BLIsRun==3)     BLNegRunCheck();
      
    WeaBLNAnglePid.cur = Gun_Real_Angle_Value;
    WeaBLNAnglePid.tar = (BLIsRun<=0)?Gun_Real_Angle_Value:BLTargetAngle; 
    
    PID_Calc(&WeaBLNAnglePid,timetick);
      
    BLN_Angle_PID_Output=WeaBLNAnglePid.output;
    
}

void BLForwardTurn()
{
    BLIsRun = 2;
    ReverseCount  = 0;   
    BLTargetAngle = GunInitAngelValue + BAddAngle;
    FiTargetAngle = GunInitAngelValue + BAddAngle;
    GunInitAngelValue += BAddAngle;
}
void BLPosRunCheck(void)
{
    if(Gun_Real_Speed_Value<10)                                   
    {
        ReverseCount++;
        if(ReverseCount >50 && ReverseCount%2==0)
            BLTargetAngle+=1;
    }
    if(ReverseCount==200)
    {
        ReverseCount=0;
        BLReverseTurn();
    }
    if(FiTargetAngle-Gun_Real_Angle_Value<3 )
    {
        BLIsRun=0;
        ReverseCount=0;
        checkIsFire ++;
    }
}
void BLNegRunCheck(void)
{
    if(my_abs(Gun_Real_Speed_Value)<10)
    {
        ReverseCount++;
        BLTargetAngle-=1;
    }
    if(Gun_Real_Angle_Value - FiTargetAngle<3 )
    {
        BLIsRun=0;
        ReverseCount=0;
    }
    if(ReverseCount == 100)
    {
        ReverseCount = 0;
        BLIsRun = 0;
    }
}
void BLReverseTurn(void)
{
    BLIsRun=3;
    ReverseCount = 0;
    BLTargetAngle = GunInitAngelValue - BAddAngle;
    FiTargetAngle = GunInitAngelValue - BAddAngle;
    GunInitAngelValue -= BAddAngle;
}

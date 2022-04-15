#include "main.h"
#include "OSinclude.h"
//功率环参数配置	开始
//版本1.2version  
//作者：warpgate
//功能：动态的功率分配
//功率分配参考
//修改时间20170417
//版本修改说明：
//	添加了稳步启动和稳步制动的代码
//	将power_limit（）函数分开书写
//0418 键盘按键改为上升沿和下降沿触发 上升沿完成启动过程 下降沿完成制动过程 两个过程可以互相抢断
//		电流 与keyTimeOfUnderpan之间的函数关系 改动为一次函数 电流上线值沿直线变化
//0429改进功率函数 
//0502修改了pid。c中的134行和power_limit。c
//0510以炖为蓝本修改了 小鸡的功率限制的参数
//0711重新优化了功率函数
//		单轮功率上限的比例是依据各个轮子电流的比例
//		不再是简单的电流限幅，电流限幅上限值由给定电流和实际转速共同决定
//		注：假设认为电气时间常数很小，即近似认为给定电流与实际电流几乎同步变换，
//				而机械时间常数比较大，所以速度参数只能使用实际的速度
//0723修改了格式
//----------------------------------------
//P(W)practical 		|			P(W)definition
//----------------------------------------
//75								|80
//----------------------------------------
//69								|75
//----------------------------------------
//63								|70
//----------------------------------------
//56								|65
//----------------------------------------

//-----------------------------------------------------------------
//-----------------------------------------------------------------
/*
			底盘限制功率（w） 稳定运行功0率上限  给定78。0w对应实际功率浮动上限会稳定在80w
	左右 实际对应关系待测！
			75w下步兵经过测试没有超功率，但是不是自大值
*/
//功率环参数配置	开始
const float POWER_UPLIMIT 					=	75.0;						
const float POWER_T_I								= 0.0202;							//转矩常数（N*m/A）		每安培产生0.0202牛*米的扭矩
const float DECAY 									= 1.0;								//衰减因子
const u16 PIDPWRMAX 								= 14000;							//pid.c 中的pidPanMax 应该保持一致		待修改		
const u16 CURRENT_OUTPUTLM					=	7000;							//整个功率限制最后一个环节电流限制的上限		热身赛4000


float pwrUpLimit							=	0;
u16 pidPwrMax									=	0;								//随着速度增大逐渐变化的pid.current 输出上限
//功率环参数配置	结束

/*------------------------功率限制函数定义 开始--------------------------
功能：底盘总功率限制
参数：
返回：
----------------------------------------------------------------------*/
//求绝对值
float  myabs(float x)
{
	return x<0?	-x:x;
}

/*
功率限制函数
此函数调用了其余的几个子函数，
若要去除部分功能 只需在此函数中注释该部分即可，而部分变量也应该做相应的改动
*/
	u8 wheel_number=0;
	float temp_p[4]={0};
	float temp_sum=0;
	u16 speedValueSum=0;
	float powerLimitSingleWheel[4]={0};	
	float	ABSrealSpeed[4]={0};
	/*
		在Task_Decode.c中被调用
		功率限制函数
	*/
	float pidcurrent[4]={0};
	float pwr_Overflow;                                     //裁判系统读出可超功率剩余量
	float pwr_Uplimit;                                      //浮动的功率上限
void power_limit(void)
{	
  //pwr_limit_parameter_set();                               
pidcurrent[0] =	ChassisParam.LF.NeedCurrent ;
pidcurrent[1]	= ChassisParam.RF.NeedCurrent;
pidcurrent[2]	= ChassisParam.RB.NeedCurrent ;
pidcurrent[3]	= ChassisParam.LB.NeedCurrent ;
	
	pwr_Uplimit=70;
	//pwr_Uplimit
	
//策略一
//	pwr_Overflow = tttGameInfo.remainPower;
//	if(pwr_Overflow>10)		pwr_Uplimit= POWER_UPLIMIT-0.2f*(60-pwr_Overflow);    //可超功率剩余量不能超过60
//	if(pwr_Overflow<=10)		pwr_Uplimit=40;                                      //如果效果不好，可以调整POWER_UPLIMIT，0.2和阈值0

//策略二	
//	if(pwr_Overflow==60)  pwr_Uplimit= POWER_UPLIMIT;
//	else if(pwr_Overflow>0&&pwr_Overflow<60)		pwr_Uplimit= POWER_UPLIMIT-0.1*(60-pwr_Overflow)-5;
//	else if(pwr_Overflow<=0)	  pwr_Uplimit=60;                               //可调节系数0.1和常数-5	
	
	pwr_limit_proportion_compute();														//计算功率分配的权重
	
	pwr_limit_proportion_distribution();											//分配功率
	
	pid_current_output_limit();																//最终的	总的输出限幅
	
}


/*
参数名：pwr_limit_parameter_set
获取底盘的实际转速、目标电流值
*/

/*
	函数名：pwr_limit_proportion_compute
	参数：	none
	作者：	warpgate
	说明：
	计算每个轮的功率上限权重
	比例依据：每个轮电流之间的比例
*/

void 	pwr_limit_proportion_compute(void)
{
	temp_sum=0;
	float pwr_real[4] = {0};
	for(wheel_number=0;wheel_number<4;wheel_number++)	
		{
		 pwr_real[wheel_number]=myabs(wheel[wheel_number]*pidcurrent[wheel_number]);
		 temp_sum +=pwr_real[wheel_number];
		}				
	if(temp_sum==0)	return;																											//剔除分母为0的情况
	for(wheel_number=0;wheel_number<4;wheel_number++)		
		powerLimitSingleWheel[wheel_number]	=	pwr_real[wheel_number]*pwr_Uplimit/temp_sum;
}
/*
	函数名：pwr_limit_proportion_distribution
	参数：	none
	作者：	warpgate
	说明：
	将pwr_limit_proportion_compute计算出的功率进行分配处理，
	即对pid电流输出进行限幅
*/
float pwr_temp[4]={0};
void pwr_limit_proportion_distribution(void)
{
	/*
		（pidcurrent[0]*Real_Speed_Value[0])	<	9549.0f*powerLimitSingleWheel[0]/POWER_T_I 
			I * T * N < P * 60 /2/3.14
		时候什么都不做
		9549.2965855137201461330258023509
	
		ChassisParam.LF.RealSpeed 是实际速度
	*/
	pwr_temp[0]		=		9549.0f	*	powerLimitSingleWheel[0]	/	POWER_T_I;				//计算中间值
	pwr_temp[1]		=		9549.0f	*	powerLimitSingleWheel[1]	/	POWER_T_I;
	pwr_temp[2]		=		9549.0f	*	powerLimitSingleWheel[2]	/	POWER_T_I;
	pwr_temp[3]		=		9549.0f	*	powerLimitSingleWheel[3]	/	POWER_T_I;
	
	//LF
	if(Real_Speed_Value[0]!=0)
		{
			if((ChassisParam.LF.NeedCurrent*Real_Speed_Value[0])	>		pwr_temp[0])	
																ChassisParam.LF.NeedCurrent		=		pwr_temp[0]	
																															/		Real_Speed_Value[0];
			if((ChassisParam.LF.NeedCurrent*Real_Speed_Value[0])	<		-pwr_temp[0])
																ChassisParam.LF.NeedCurrent		=		-pwr_temp[0]	
																															/		Real_Speed_Value[0];
		}

	//RF
	if(Real_Speed_Value[1]!=0)
	{
			if((ChassisParam.RF.NeedCurrent*Real_Speed_Value[1])	>		pwr_temp[1])
																ChassisParam.RF.NeedCurrent		=		pwr_temp[1]
																															/		Real_Speed_Value[1];
			if((ChassisParam.RF.NeedCurrent*Real_Speed_Value[1])	<		-pwr_temp[1])
																ChassisParam.RF.NeedCurrent		=		-pwr_temp[1]
																															/		Real_Speed_Value[1];
	}
	
	//RB
	if(Real_Speed_Value[2]!=0)
	{
		if((ChassisParam.RB.NeedCurrent*Real_Speed_Value[2])	>		pwr_temp[2])
																ChassisParam.RB.NeedCurrent		=		pwr_temp[2]	
																															/		Real_Speed_Value[2];
		if((ChassisParam.RB.NeedCurrent*Real_Speed_Value[2])	<		-pwr_temp[2])
																ChassisParam.RB.NeedCurrent		=		-pwr_temp[2]	
																															/		Real_Speed_Value[2];
	}
	
	//LB
	if(Real_Speed_Value[3]!=0)
	{
		if((ChassisParam.LB.NeedCurrent*Real_Speed_Value[3])	>		pwr_temp[3])
																ChassisParam.LB.NeedCurrent		=		pwr_temp[3]	
																															/		Real_Speed_Value[3];
		if((ChassisParam.LB.NeedCurrent*Real_Speed_Value[3])	<		-pwr_temp[3])
																ChassisParam.LB.NeedCurrent		=		-pwr_temp[3]	
																															/		Real_Speed_Value[3];
	}
}


/*
最后一step，电流限幅函数! 
*/

void pid_current_output_limit(void)
	{
		ChassisParam.LF.LimitCurrent	=		(ChassisParam.LF.NeedCurrent	>	CURRENT_OUTPUTLM		?		CURRENT_OUTPUTLM	:	ChassisParam.LF.NeedCurrent);
		ChassisParam.RF.LimitCurrent	=		(ChassisParam.RF.NeedCurrent	>	CURRENT_OUTPUTLM		?		CURRENT_OUTPUTLM	:	ChassisParam.RF.NeedCurrent);
		ChassisParam.RB.LimitCurrent	=		(ChassisParam.RB.NeedCurrent	>	CURRENT_OUTPUTLM		?		CURRENT_OUTPUTLM	:	ChassisParam.RB.NeedCurrent);
		ChassisParam.LB.LimitCurrent	=		(ChassisParam.LB.NeedCurrent	>	CURRENT_OUTPUTLM		?		CURRENT_OUTPUTLM	:	ChassisParam.LB.NeedCurrent);
		
		ChassisParam.LF.LimitCurrent	=		(ChassisParam.LF.NeedCurrent	<	-CURRENT_OUTPUTLM		?		-CURRENT_OUTPUTLM	:	ChassisParam.LF.NeedCurrent);
		ChassisParam.RF.LimitCurrent	=		(ChassisParam.RF.NeedCurrent	<	-CURRENT_OUTPUTLM		?		-CURRENT_OUTPUTLM	:	ChassisParam.RF.NeedCurrent);
		ChassisParam.RB.LimitCurrent	=		(ChassisParam.RB.NeedCurrent	<	-CURRENT_OUTPUTLM		?		-CURRENT_OUTPUTLM	:	ChassisParam.RB.NeedCurrent);
		ChassisParam.LB.LimitCurrent	=		(ChassisParam.LB.NeedCurrent	<	-CURRENT_OUTPUTLM		?		-CURRENT_OUTPUTLM	:	ChassisParam.LB.NeedCurrent);
	}

/*------------------------功率限制函数定义 结束--------------------------*/

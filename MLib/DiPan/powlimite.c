#include "main.h"
#include "OSinclude.h"
//���ʻ���������	��ʼ
//�汾1.2version  
//���ߣ�warpgate
//���ܣ���̬�Ĺ��ʷ���
//���ʷ���ο�
//�޸�ʱ��20170417
//�汾�޸�˵����
//	������Ȳ��������Ȳ��ƶ��Ĵ���
//	��power_limit���������ֿ���д
//0418 ���̰�����Ϊ�����غ��½��ش��� ����������������� �½�������ƶ����� �������̿��Ի�������
//		���� ��keyTimeOfUnderpan֮��ĺ�����ϵ �Ķ�Ϊһ�κ��� ��������ֵ��ֱ�߱仯
//0429�Ľ����ʺ��� 
//0502�޸���pid��c�е�134�к�power_limit��c
//0510����Ϊ�����޸��� С���Ĺ������ƵĲ���
//0711�����Ż��˹��ʺ���
//		���ֹ������޵ı��������ݸ������ӵ����ı���
//		�����Ǽ򵥵ĵ����޷��������޷�����ֵ�ɸ���������ʵ��ת�ٹ�ͬ����
//		ע��������Ϊ����ʱ�䳣����С����������Ϊ����������ʵ�ʵ�������ͬ���任��
//				����еʱ�䳣���Ƚϴ������ٶȲ���ֻ��ʹ��ʵ�ʵ��ٶ�
//0723�޸��˸�ʽ
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
			�������ƹ��ʣ�w�� �ȶ����й�0������  ����78��0w��Ӧʵ�ʹ��ʸ������޻��ȶ���80w
	���� ʵ�ʶ�Ӧ��ϵ���⣡
			75w�²�����������û�г����ʣ����ǲ����Դ�ֵ
*/
//���ʻ���������	��ʼ
const float POWER_UPLIMIT 					=	75.0;						
const float POWER_T_I								= 0.0202;							//ת�س�����N*m/A��		ÿ�������0.0202ţ*�׵�Ť��
const float DECAY 									= 1.0;								//˥������
const u16 PIDPWRMAX 								= 14000;							//pid.c �е�pidPanMax Ӧ�ñ���һ��		���޸�		
const u16 CURRENT_OUTPUTLM					=	7000;							//���������������һ�����ڵ������Ƶ�����		������4000


float pwrUpLimit							=	0;
u16 pidPwrMax									=	0;								//�����ٶ������𽥱仯��pid.current �������
//���ʻ���������	����

/*------------------------�������ƺ������� ��ʼ--------------------------
���ܣ������ܹ�������
������
���أ�
----------------------------------------------------------------------*/
//�����ֵ
float  myabs(float x)
{
	return x<0?	-x:x;
}

/*
�������ƺ���
�˺�������������ļ����Ӻ�����
��Ҫȥ�����ֹ��� ֻ���ڴ˺�����ע�͸ò��ּ��ɣ������ֱ���ҲӦ������Ӧ�ĸĶ�
*/
	u8 wheel_number=0;
	float temp_p[4]={0};
	float temp_sum=0;
	u16 speedValueSum=0;
	float powerLimitSingleWheel[4]={0};	
	float	ABSrealSpeed[4]={0};
	/*
		��Task_Decode.c�б�����
		�������ƺ���
	*/
	float pidcurrent[4]={0};
	float pwr_Overflow;                                     //����ϵͳ�����ɳ�����ʣ����
	float pwr_Uplimit;                                      //�����Ĺ�������
void power_limit(void)
{	
  //pwr_limit_parameter_set();                               
pidcurrent[0] =	ChassisParam.LF.NeedCurrent ;
pidcurrent[1]	= ChassisParam.RF.NeedCurrent;
pidcurrent[2]	= ChassisParam.RB.NeedCurrent ;
pidcurrent[3]	= ChassisParam.LB.NeedCurrent ;
	
	pwr_Uplimit=70;
	//pwr_Uplimit
	
//����һ
//	pwr_Overflow = tttGameInfo.remainPower;
//	if(pwr_Overflow>10)		pwr_Uplimit= POWER_UPLIMIT-0.2f*(60-pwr_Overflow);    //�ɳ�����ʣ�������ܳ���60
//	if(pwr_Overflow<=10)		pwr_Uplimit=40;                                      //���Ч�����ã����Ե���POWER_UPLIMIT��0.2����ֵ0

//���Զ�	
//	if(pwr_Overflow==60)  pwr_Uplimit= POWER_UPLIMIT;
//	else if(pwr_Overflow>0&&pwr_Overflow<60)		pwr_Uplimit= POWER_UPLIMIT-0.1*(60-pwr_Overflow)-5;
//	else if(pwr_Overflow<=0)	  pwr_Uplimit=60;                               //�ɵ���ϵ��0.1�ͳ���-5	
	
	pwr_limit_proportion_compute();														//���㹦�ʷ����Ȩ��
	
	pwr_limit_proportion_distribution();											//���书��
	
	pid_current_output_limit();																//���յ�	�ܵ�����޷�
	
}


/*
��������pwr_limit_parameter_set
��ȡ���̵�ʵ��ת�١�Ŀ�����ֵ
*/

/*
	��������pwr_limit_proportion_compute
	������	none
	���ߣ�	warpgate
	˵����
	����ÿ���ֵĹ�������Ȩ��
	�������ݣ�ÿ���ֵ���֮��ı���
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
	if(temp_sum==0)	return;																											//�޳���ĸΪ0�����
	for(wheel_number=0;wheel_number<4;wheel_number++)		
		powerLimitSingleWheel[wheel_number]	=	pwr_real[wheel_number]*pwr_Uplimit/temp_sum;
}
/*
	��������pwr_limit_proportion_distribution
	������	none
	���ߣ�	warpgate
	˵����
	��pwr_limit_proportion_compute������Ĺ��ʽ��з��䴦��
	����pid������������޷�
*/
float pwr_temp[4]={0};
void pwr_limit_proportion_distribution(void)
{
	/*
		��pidcurrent[0]*Real_Speed_Value[0])	<	9549.0f*powerLimitSingleWheel[0]/POWER_T_I 
			I * T * N < P * 60 /2/3.14
		ʱ��ʲô������
		9549.2965855137201461330258023509
	
		ChassisParam.LF.RealSpeed ��ʵ���ٶ�
	*/
	pwr_temp[0]		=		9549.0f	*	powerLimitSingleWheel[0]	/	POWER_T_I;				//�����м�ֵ
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
���һstep�������޷�����! 
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

/*------------------------�������ƺ������� ����--------------------------*/

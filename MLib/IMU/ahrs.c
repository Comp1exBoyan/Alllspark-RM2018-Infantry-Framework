/*********BlackHole1 For Study**************
 *	�ļ���					:			   ahrs.c
 *	����					:   �������ݴ����ļ�
 *	�汾					:	BlackHole1_For_Study_V1
 *	����					:	 	��INF�������		
 *	������̳				:		 www.infcre.com
 *	QQȺ					:   	  168590475
 *  Դ��״̬				:         ��Դ
 *			For infinite creativity
 *********(C) COPYRIGHT 2015 INF ************/

#include 	"ahrs.h"
#include 	"stdio.h"
#include  "math.h"    
#include "mpu6500.h"

/*
 * ��������Acc_Flitter
 * ����  �����ٶȼ�Ԥ�˲�
 *                     ������С���˹���/�����˲�
 *			1������洢ȫ�����ݣ�ȡ��һ��۲����ݱ���Թ���һ�β���
 *				�Ҷ��ܹ���һ��������������ɣ�������С��ռ�ô洢�ռ�С
 *			2������һ����ʵʱ��������
 * ����  ����
 * ���  ����
 */ 
void	Acc_Flitter()
{
	
}

/*
 * ��������Prepare_Data
 * ����  ����ȡMPU6500ԭʼ���ݣ����ٶȼ�Ԥ�˲�
 * ����  ��Now_Angle����ǰ��̬��Exp_Angle��������̬
 * ���  ����
 */ 
void Prepare_Data(void)
{
	MPU6500_ReadValue(); //��ȡMPU6500ԭʼֵ
	Acc_Flitter();		 	 //���ٶȼ�Ԥ�˲�
}

/*********BlackHole1 For Study**************
 *	文件名					:			   ahrs.c
 *	描述					:   航姿数据处理文件
 *	版本					:	BlackHole1_For_Study_V1
 *	作者					:	 	【INF】无穷创新		
 *	技术论坛				:		 www.infcre.com
 *	QQ群					:   	  168590475
 *  源码状态				:         开源
 *			For infinite creativity
 *********(C) COPYRIGHT 2015 INF ************/

#include 	"ahrs.h"
#include 	"stdio.h"
#include  "math.h"    
#include "mpu6500.h"

/*
 * 函数名：Acc_Flitter
 * 描述  ：加速度计预滤波
 *                     递推最小二乘估计/滑动滤波
 *			1：无需存储全部数据，取得一组观测数据便可以估计一次参数
 *				且都能够在一个采样周期内完成，计算量小，占用存储空间小
 *			2：具有一定的实时处理能力
 * 输入  ：无
 * 输出  ：无
 */ 
void	Acc_Flitter()
{
	
}

/*
 * 函数名：Prepare_Data
 * 描述  ：读取MPU6500原始数据；加速度计预滤波
 * 输入  ：Now_Angle：当前姿态；Exp_Angle：期望姿态
 * 输出  ：无
 */ 
void Prepare_Data(void)
{
	MPU6500_ReadValue(); //读取MPU6500原始值
	Acc_Flitter();		 	 //加速度计预滤波
}

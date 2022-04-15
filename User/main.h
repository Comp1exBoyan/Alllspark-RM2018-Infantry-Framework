/***************************************************************************************************************
名称：main.h

实现功能：对底盘和云台的鼠标键盘操作
修改日期：20161126
作者：	：上届团队
修改格式： 修改者+修改项目+修改日期
修改者： 	warpgate_hzk	将上一届（2016）比赛程序中云台部分程序提取用来测试的版本	20161126
			warpgate_hzk	除去用于云台测试的部分 删去其余所有多余的部分 PITCH yaw 现与实际安装一致
							pitch俯仰	
							yaw	平转	20161127
			ybw				将can发送pid计算后角度的函数改进为内部时钟中断，发送时间为固定值400us	20161127
			warpgate_hzk	将所有的常量修改为大写  增加了param_Init()函数				20161127

***************************************************************************************************************/

#ifndef __MAIN_H__
#define __MAIN_H__

#include "scheduler.h"
#include "init.h"
#include "OSinclude.h"

#define TRUE	1
#define FLASE 0
int isNaN(float f);
extern u8 all_init;	

#endif 

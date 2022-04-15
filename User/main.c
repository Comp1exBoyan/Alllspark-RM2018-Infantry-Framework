/***************************************************************************************************************
名称：main.c
实现功能：对底盘和云台的鼠标键盘操作
修改格式： 修改者+修改项目+修改日期
修改者：唐义丰
修改日期：20170222
修改内容：整理程序架构，添加 custom 文件夹，新增fun.c,init.c,PID.c文件

20180621修改者：唐义丰 整理程序框架

***************************************************************************************************************/

#include "main.h"
#include "OSinclude.h"


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

u8 all_init = 0;
//开始任务任务函数


int main(void)
    {
			all_init=ctrl_Init();
			while(!all_init);
			xTaskCreate((TaskFunction_t )start_task,            //任务函数
									(const char*    )"start_task",          //任务名称
									(uint16_t       )START_STK_SIZE,        //任务堆栈大小
									(void*          )NULL,                  //传递给任务函数的参数
									(UBaseType_t    )START_TASK_PRIO,       //任务优先级
									(TaskHandle_t*  )&StartTask_Handler);   //任务句柄
									
			vTaskStartScheduler();          //开启任务调度

}

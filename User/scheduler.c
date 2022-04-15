
#include "scheduler.h"
#include "Weapon.h"
#include "JudgeSystemN.h"
#include "usart3.h"
#include "OSinclude.h"

/*底盘控制5ms*/
#define Task_Control_PRIO		6
#define Task_Control_STK_SIZE 		512
TaskHandle_t Task_Control_Handler;
void Speed_Dipan_Ctrl(void *Parameters);

/*传回数据*/
#define Task_SendCosume_PRIO	5
#define Task_SendCosume_STK_SIZE 		128
TaskHandle_t Task_SendCosume_Handler;
void SendCosume(void *Parameters);

/*云台控制函数*/
#define Task_Yuntai_Ctrl_PRIO		5
#define Task_Yuntai_Ctrl_STK_SIZE 		512  
TaskHandle_t Task_Yuntai_Ctrl_Handler;

/*武器控制函数*/
#define Task_Weapon_Ctrl_PRIO		5
#define Task_Weapon_Ctrl_STK_SIZE 		512  
TaskHandle_t Task_Weapon_Ctrl_Handler;

/*Can发送函数*/
#define Task_SendCan_PRIO   6
#define Task_SendCan_STK_SIZE    512
TaskHandle_t Task_SendCan_Handler;

/**/
#define Task_CalcShoot_PRIO   5
#define Task_CalcShoot_STK_SIZE    512
TaskHandle_t Task_CalcShoot_Handler;

/*--------------------------获取系统准确运行时间---------------------------------------*/
float secTime = 0.0f;

volatile uint32_t sysTickUptime = 0;
volatile float Cycle_T[20][3];
enum
{
	NOW = 0,
	OLD,
	NEW,
};

void TaskCanSend(void *Parameters)
{
		portTickType CurrentControlTick = 0;                //当前系统时间

		while(1)
		{
//				//Yuntai_Cmd_ESC(Pit_Speed_PID_Output,0,0,0);
				Yuntai_Yaw_Cmd_ESC(Yaw_Speed_PID_Output, BLN_Speed_PID_Output);
				Yuntai_Cmd_ESC(Pit_Speed_PID_Output,0,MC_PID_Output1,MC_PID_Output2);
			
				vTaskDelayUntil(&CurrentControlTick, 5 / portTICK_RATE_MS);
		}
}

void TaskYuntaiControl(void *Parameters)
{
		portTickType CurrentControlTick = 0;                //当前系统时间

		while(1)
		{
				YunTaiControl(0.1f);
				vTaskDelayUntil(&CurrentControlTick, 5 / portTICK_RATE_MS);
		}
}

void TaskWeaponControl(void *Parameters)
{
		portTickType CurrentControlTick = 0;                //当前系统时间

		while(1)
		{
				WeaponControl(1);	 
				vTaskDelayUntil(&CurrentControlTick, 6 / portTICK_RATE_MS);
		}
}

void TaskCalcShoot(void *Parameters)
{
		portTickType CurrentControlTick = 0;
		while(1)
		{
				SelfCalcShootHeat -= 0.305f;
				if(SelfCalcShootHeat<0) SelfCalcShootHeat = 0;
				
				vTaskDelayUntil(&CurrentControlTick, 50 / portTICK_RATE_MS);
		}
}

void TaskSendCosume(void *Parameters)
{   
		portTickType CurrentControlTick = 0;
		while(1)
		{
				u8 SendLed = 0;
				SendLed = UartAuxiFlag==1?(SendLed | 1):SendLed;
				SendLed = RecieveAuxiliaryflag==1?(SendLed | 2):SendLed;
				SendLed = UartBuffFlag==1?(SendLed | 4):SendLed;
				SendLed = RecieveAuxiliaryflag==1?(SendLed | 8):SendLed;
				SendLed = FireModel==OneClickModel?(SendLed | 16):(SendLed | 32);
				SendCosumeData(BulletNum,SelfCalcShootHeat,0,SendLed);
				
				vTaskDelayUntil(&CurrentControlTick, 1000 / portTICK_RATE_MS);
		}
}


void start_task(void *pvParameters)
{
		taskENTER_CRITICAL();           //进入临界区

		//Queue_CANSend = xQueueCreate(64, sizeof(CanSend_Type));
		//底盘控制任务
    xTaskCreate((TaskFunction_t )Speed_Dipan_Ctrl,
                (const char*    )"Speed_Dipan_Ctrl",
                (uint16_t       )Task_Control_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Task_Control_PRIO,
                (TaskHandle_t*  )&Task_Control_Handler);
								
		//云台控制任务
		xTaskCreate((TaskFunction_t )TaskYuntaiControl,
                (const char*    )"Task_Yuntai_Ctrl",
                (uint16_t       )Task_Yuntai_Ctrl_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Task_Yuntai_Ctrl_PRIO,
                (TaskHandle_t*  )&Task_Yuntai_Ctrl_Handler);
								
		//武器控制任务
		xTaskCreate((TaskFunction_t )TaskWeaponControl,
                (const char*    )"Task_Weapon_Ctrl",
                (uint16_t       )Task_Weapon_Ctrl_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Task_Weapon_Ctrl_PRIO,
                (TaskHandle_t*  )&Task_Weapon_Ctrl_Handler);
								
		//can发送任务
		xTaskCreate((TaskFunction_t )TaskCanSend,
                (const char*    )"TaskCanSend",
                (uint16_t       )Task_SendCan_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Task_SendCan_PRIO,
                (TaskHandle_t*  )&Task_SendCan_Handler);

		//数据传回
		xTaskCreate((TaskFunction_t )TaskSendCosume,
                (const char*    )"SendCosume",
                (uint16_t       )Task_SendCosume_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )Task_SendCosume_PRIO,
                (TaskHandle_t*  )&Task_SendCosume_Handler);

		//
		xTaskCreate((TaskFunction_t )TaskCalcShoot,
								(const char*    )"CalcShoot",
								(uint16_t       )Task_CalcShoot_STK_SIZE,
								(void*          )NULL,
								(UBaseType_t    )Task_CalcShoot_PRIO,
								(TaskHandle_t*  )&Task_CalcShoot_Handler);
								
								
		vTaskDelete(NULL);              //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
	
}


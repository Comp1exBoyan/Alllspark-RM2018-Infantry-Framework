/***************************************************************************************************************
���ƣ�main.c
ʵ�ֹ��ܣ��Ե��̺���̨�������̲���
�޸ĸ�ʽ�� �޸���+�޸���Ŀ+�޸�����
�޸��ߣ������
�޸����ڣ�20170222
�޸����ݣ��������ܹ������ custom �ļ��У�����fun.c,init.c,PID.c�ļ�

20180621�޸��ߣ������ ���������

***************************************************************************************************************/

#include "main.h"
#include "OSinclude.h"


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

u8 all_init = 0;
//��ʼ����������


int main(void)
    {
			all_init=ctrl_Init();
			while(!all_init);
			xTaskCreate((TaskFunction_t )start_task,            //������
									(const char*    )"start_task",          //��������
									(uint16_t       )START_STK_SIZE,        //�����ջ��С
									(void*          )NULL,                  //���ݸ��������Ĳ���
									(UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
									(TaskHandle_t*  )&StartTask_Handler);   //������
									
			vTaskStartScheduler();          //�����������

}

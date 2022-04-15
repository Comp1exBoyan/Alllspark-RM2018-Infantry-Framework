#ifndef __FREECARS_H__
#define __FREECARS_H__

#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#define FreeCarsUARTPort UART0
#define FreeCarsIRQ      IRQ_UART0
#define FreeCarsBR       115200       //������

//FreeCars��λ�� ����������ʾ�� ����ͨ������������λ�����øı�
#define UartDataNum      17	    

//���²�Ҫ�޸�
#define UartRxBufferLen  100
#define UartRxDataLen    41           //FreeCars��λ�����͸�������MCU���գ���Ҫ��
#define UartRxCmdLen     7	      //FreeCars��λ�������������ݳ��ȣ���Ҫ��

#define UartCmdNum  SerialPortRx.Buffer[SerialPortRx.Stack-3]//�����
#define UartCmdData SerialPortRx.Buffer[SerialPortRx.Stack-2]//��������
//�������ݵĳ���ֻҪ�����鳤��Ϊ26=22+3+1������Ϊ���뷢���ַ���ȡ��ͳһ
//ȡ���ݵĳ������ַ����ĳ�����ȣ������ڷ��������ǻ�෢����һЩ
//��Чλ������Ӱ�첻���

       		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��

void push(uint8_t chanel,uint16_t data);
void USART3_SendChar(unsigned char b);



#endif

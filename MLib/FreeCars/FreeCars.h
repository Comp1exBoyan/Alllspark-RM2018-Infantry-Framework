#ifndef __FREECARS_H__
#define __FREECARS_H__

#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#define FreeCarsUARTPort UART0
#define FreeCarsIRQ      IRQ_UART0
#define FreeCarsBR       115200       //波特率

//FreeCars上位机 串口数据显示框 接收通道数，按照上位机设置改变
#define UartDataNum      17	    

//以下不要修改
#define UartRxBufferLen  100
#define UartRxDataLen    41           //FreeCars上位机发送浮点数据MCU接收，不要改
#define UartRxCmdLen     7	      //FreeCars上位机接收命令数据长度，不要改

#define UartCmdNum  SerialPortRx.Buffer[SerialPortRx.Stack-3]//命令号
#define UartCmdData SerialPortRx.Buffer[SerialPortRx.Stack-2]//命令数据
//尽管数据的长度只要求数组长度为26=22+3+1；但是为了与发送字符串取得统一
//取数据的长度与字符串的长度相等，这样在发送数据是会多发送了一些
//无效位，但是影响不大的

       		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义

void push(uint8_t chanel,uint16_t data);
void USART3_SendChar(unsigned char b);



#endif

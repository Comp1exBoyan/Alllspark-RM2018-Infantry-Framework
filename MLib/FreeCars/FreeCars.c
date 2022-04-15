#include "FreeCars.h"
#include "mpu9250.h"
#include "can2.h"
uint8_t uSendBuf[UartDataNum*2]={0};
uint8_t FreeCarsDataNum=UartDataNum*2;

double UartData[9] = {0};

void push(uint8_t chanel,uint16_t data)
{
    uSendBuf[chanel*2]=data/256;
    uSendBuf[chanel*2+1]=data%256;
}

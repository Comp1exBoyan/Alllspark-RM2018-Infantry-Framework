#ifndef __MPU6500_SPI_H
#define __MPU6500_SPI_H

#include "imuint.h"

#define MPU6500_CS(X)			(X==0)?GPIO_ResetBits(GPIOF,GPIO_Pin_6):GPIO_SetBits(GPIOF,GPIO_Pin_6) //MPU6500片选信号

extern void SPI5_Init(void);

extern u8 MPU6500_Write_Reg(uint8_t reg,uint8_t value);
extern u8 MPU6500_Read_Reg(uint8_t reg);
extern u8 SPI5_Read_Write_Byte(uint8_t TxData);//SPI总线读写一个字节


#endif



#ifndef __IST8310_H__
#define __IST8310_H__
#include "imuint.h"
//IST8310 REG address

#define IST8310_ADDRESS           0x0E
#define IST8310_DEVICE_ID_A       0x10

//refer to IST8310 datasheet for more informations
#define IST8310_WHO_AM_I          0x00
#define IST8310_R_CONFA           0x0A
#define IST8310_R_CONFB           0x0B
#define IST8310_R_MODE            0x02

#define IST8310_R_XL              0x03
#define IST8310_R_XM              0x04
#define IST8310_R_YL              0x05
#define IST8310_R_YM              0x06
#define IST8310_R_ZL              0x07
#define IST8310_R_ZM              0x08

#define IST8310_AVGCNTL           0x41
#define IST8310_PDCNTL            0x42

#define IST8310_ODR_MODE          0x01

#define CALIBRATING_MAG_CYCLES              2000  //校准时间20s

typedef struct 
{
  s16 x;
	s16 y;
	s16 z;

}Mag_xyz_s16_t;
typedef struct 
{
  float x;
	float y;
	float z;
}Mag_xyz_f_t;
typedef struct 
{
	Mag_xyz_s16_t Mag_Adc;			//采样值
	Mag_xyz_f_t   Mag_Offset;		//偏移值
	Mag_xyz_f_t 	Mag_Gain;			//比例缩放
  Mag_xyz_f_t 	Mag_Val;			//纠正后的值
}IST8310_t;
extern IST8310_t IST8310;
extern u8 Mag_CALIBRATED;
static void IST_Reg_Write_By_MPU(uint8_t addr, uint8_t data);
static uint8_t IST_Reg_Read_By_MPU(uint8_t addr);
static void MPU_Auto_Read_IST_config(uint8_t device_address, uint8_t reg_base_addr, uint8_t data_num);
uint8_t IST8310_Init(void);
void IST8310_ReadValue(void);
void ANO_IST8310_CalOffset_Mag(void);

#endif

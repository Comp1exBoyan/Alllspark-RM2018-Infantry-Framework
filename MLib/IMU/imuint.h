#ifndef __IMUINT_H__
#define __IMUINT_H__

#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stm32f4xx_spi.h>

#include "mpu9250.h"
#include "filter.h"
#include "can2.h"
#include "mpu6500_spi.h"
#include "mpu6500.h"
#include 	"ahrs.h"
#include "mymath.h"
#include "imu.h"
#include "IST8310.h"

u8 IMU_int(void);

#endif 

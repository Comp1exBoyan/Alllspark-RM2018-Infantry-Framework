#include "IST8310.h"
IST8310_t IST8310 = { {0,0,0},{-1,-1,-1},{1,0.8538,0.9389},{0,0,0} };
//Write IST8310 register through MPU6500
static void IST_Reg_Write_By_MPU(uint8_t addr, uint8_t data)
{
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_CTRL, 0x00);
  delay_ms(2);
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_REG, addr);
  delay_ms(2);
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_DO, data);
  delay_ms(2);
  
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_CTRL, 0x080 | 0x01);
  delay_ms(10);
}

//Write IST8310 register through MPU6500
static uint8_t IST_Reg_Read_By_MPU(uint8_t addr)
{
  uint8_t data;
  
  MPU6500_Write_Reg(MPU6500_I2C_SLV4_REG, addr);
  delay_ms(10);
  MPU6500_Write_Reg(MPU6500_I2C_SLV4_CTRL, 0x80);
  delay_ms(10);
  data = MPU6500_Read_Reg(MPU6500_I2C_SLV4_DI);
  MPU6500_Write_Reg(MPU6500_I2C_SLV4_CTRL, 0x00);
  delay_ms(10);
  return data;
}

//Initialize the MPU6500 I2C Slave0 for I2C reading
static void MPU_Auto_Read_IST_config(uint8_t device_address, uint8_t reg_base_addr, uint8_t data_num)
{
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_ADDR, device_address);
  delay_ms(2);
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_REG, IST8310_R_CONFA);
  delay_ms(2);
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_DO, IST8310_ODR_MODE);
  delay_ms(2);
  
  MPU6500_Write_Reg(MPU6500_I2C_SLV0_ADDR, 0x80 | device_address);
  delay_ms(2);
  MPU6500_Write_Reg(MPU6500_I2C_SLV0_REG, reg_base_addr);
  delay_ms(2);
  
  MPU6500_Write_Reg(MPU6500_I2C_SLV4_CTRL, 0x03);
  delay_ms(2);
  
  MPU6500_Write_Reg(MPU6500_I2C_MST_DELAY_CTRL, 0x01 | 0x02);
  delay_ms(2);
  
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_CTRL, 0x80 | 0x01);
  delay_ms(6);
  
  MPU6500_Write_Reg(MPU6500_I2C_SLV0_CTRL, 0x80 | data_num);
  delay_ms(7);
}

//Initialize the IST8310
uint8_t IST8310_Init(void)
{
  MPU6500_Write_Reg(MPU6500_USER_CTRL, 0x30);
  delay_ms(10);
  MPU6500_Write_Reg(MPU6500_I2C_MST_CTRL, 0x0d);
  delay_ms(10);
  
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_ADDR, IST8310_ADDRESS);
  delay_ms(10);
  MPU6500_Write_Reg(MPU6500_I2C_SLV4_ADDR, 0x80 | IST8310_ADDRESS);
  delay_ms(10);
  
  IST_Reg_Write_By_MPU(IST8310_R_CONFB, 0x01);
  if(IST8310_DEVICE_ID_A != IST_Reg_Read_By_MPU(IST8310_WHO_AM_I))
    return 1; //error
  delay_ms(10);
  
  IST_Reg_Write_By_MPU(IST8310_R_CONFA, 0x00);
  if(IST_Reg_Read_By_MPU(IST8310_R_CONFA) != 0x00)
    return 2;
  delay_ms(10);
  
  IST_Reg_Write_By_MPU(IST8310_R_CONFB, 0x00);
  if(IST_Reg_Read_By_MPU(IST8310_R_CONFB) != 0x00)
    return 3;
  delay_ms(10);
  
  IST_Reg_Write_By_MPU(IST8310_AVGCNTL, 0x24);
  if(IST_Reg_Read_By_MPU(IST8310_AVGCNTL) != 0x24)
    return 4;
  delay_ms(10);
  
  IST_Reg_Write_By_MPU(IST8310_PDCNTL, 0xc0);
  if(IST_Reg_Read_By_MPU(IST8310_PDCNTL) != 0xc0)
    return 5;
  delay_ms(10);
  
  MPU6500_Write_Reg(MPU6500_I2C_SLV1_CTRL, 0x00);
  delay_ms(10);
  MPU6500_Write_Reg(MPU6500_I2C_SLV4_CTRL, 0x00);
  delay_ms(10);
  
  MPU_Auto_Read_IST_config(IST8310_ADDRESS, IST8310_R_XL, 0x06);
  delay_ms(100);
  return 0;
}
void IST8310_ReadValue(void)
{
	int16_t mag_temp[3];
	u8 IST8310_buffer[6]; 
	
	MPU6500_CS(0); 				//使能SPI传输

	SPI5_Read_Write_Byte(IST8310_R_XL|0x80); //从加速度计的寄存器开始进行读取陀螺仪和加速度计的值//发送读命令+寄存器号
	
	for(uint8_t i	=	0;i	<	6;i++)														//一共读取14字节的数据
	{
	IST8310_buffer[i]	=	SPI5_Read_Write_Byte(0xff);	//输入0xff,因为slave不识别
	}	

	mag_temp[0] = (((int16_t)(IST8310_buffer[0])<<8) | IST8310_buffer[1]);
    mag_temp[1] = (((int16_t)(IST8310_buffer[2])<<8) | IST8310_buffer[3]);
    mag_temp[2] = (((int16_t)(IST8310_buffer[4])<<8) | IST8310_buffer[5]);
	
	MPU6500_CS(1);  	    //禁止SPI传输
	
	IST8310.Mag_Adc.x = mag_temp[0];
	IST8310.Mag_Adc.y = mag_temp[1];
	IST8310.Mag_Adc.z = mag_temp[2];
	
	IST8310.Mag_Val.x = (IST8310.Mag_Adc.x - IST8310.Mag_Offset.x) ;
	IST8310.Mag_Val.y = (IST8310.Mag_Adc.y - IST8310.Mag_Offset.y) ;
	IST8310.Mag_Val.z = (IST8310.Mag_Adc.z - IST8310.Mag_Offset.z) ;
		//磁力计中点矫正
	ANO_IST8310_CalOffset_Mag();
	
}
/*磁力计中点纠正*/
u8 Mag_CALIBRATED = 0;
void ANO_IST8310_CalOffset_Mag(void)
{
	static xyz_f_t	MagMAX = { -100 , -100 , -100 }, MagMIN = { 100 , 100 , 100 }, MagSum;
	static uint16_t cnt_m=0;
	
	if(Mag_CALIBRATED)
	{	
		
		if(ABS(IST8310.Mag_Adc.x)<400&&ABS(IST8310.Mag_Adc.y)<400&&ABS(IST8310.Mag_Adc.z)<400)
		{
			MagMAX.x = MAX(IST8310.Mag_Adc.x, MagMAX.x);
			MagMAX.y = MAX(IST8310.Mag_Adc.y, MagMAX.y);
			MagMAX.z = MAX(IST8310.Mag_Adc.z, MagMAX.z);
			
			MagMIN.x = MIN(IST8310.Mag_Adc.x, MagMIN.x);
			MagMIN.y = MIN(IST8310.Mag_Adc.y, MagMIN.y);
			MagMIN.z = MIN(IST8310.Mag_Adc.z, MagMIN.z);		
			
			if(cnt_m == CALIBRATING_MAG_CYCLES)
			{
				IST8310.Mag_Offset.x = (int16_t)((MagMAX.x + MagMIN.x) * 0.5f);
				IST8310.Mag_Offset.y = (int16_t)((MagMAX.y + MagMIN.y) * 0.5f);
				IST8310.Mag_Offset.z = (int16_t)((MagMAX.z + MagMIN.z) * 0.5f);
	
				MagSum.x = MagMAX.x - MagMIN.x;
				MagSum.y = MagMAX.y - MagMIN.y;
				MagSum.z = MagMAX.z - MagMIN.z;
				
				IST8310.Mag_Gain.y = MagSum.x / MagSum.y;
				IST8310.Mag_Gain.z = MagSum.x / MagSum.z;
			
				cnt_m = 0;
				Mag_CALIBRATED = 0;
			}
		}
		cnt_m++;
		
	}
	else
	{

	}
}

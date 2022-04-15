/*********BlackHole1 For Study**************
 *	�ļ���					:			mpu6500_spi.c
 *	����					:  mpu6500��SPI�˿ڳ�ʼ��
 *	�汾					:	BlackHole1_For_Study_V1
 *	����					:	 	��INF�������		
 *	������̳				:		 www.infcre.com
 *	QQȺ					:   	  168590475
 *  Դ��״̬				:         ��Դ
 *			For infinite creativity
 *********(C) COPYRIGHT 2015 INF ************/

#include "mpu6500_spi.h"
/*
 * ��������SPI5_Init
 * ����  ��SPI5��ʼ��
 * ����  ����
 * ���  ����
 */ 
void SPI5_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5,ENABLE);//ʹ��SPI_2ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9, GPIO_AF_SPI5); //GPIOF9����
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8, GPIO_AF_SPI5); //GPIOF8����
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource7, GPIO_AF_SPI5); //GPIOF7����
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7;           //GPIOH10         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOF,&GPIO_InitStructure);              //��ʼ��PA0
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //GPIOH10         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOF,&GPIO_InitStructure);              //��ʼ��PA0
	
	
	MPU6500_CS(1); 			 																											  //MPU6500Ƭѡȡ��
	GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7);  									//PB13/14/15����
	
	SPI_Cmd(SPI5,DISABLE);  																										//SPI5ʧ��
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  				//SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;																//SPI����
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;														//���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;																	//ʱ�����յ�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;																//���ݲ����ڵ�1��ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;																		//NSS�ź����������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;				//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16��72M/8=9M��
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;													//���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 10;																		//CRCֵ����Ķ���ʽ
	SPI_Init(SPI5, &SPI_InitStructure); 																			  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	SPI_Cmd(SPI5,ENABLE);																												//ʹ��SPI5
	SPI5_Read_Write_Byte(0xff);																									//��������	 
}


/*
 * ��������MPU6500_Write_Reg
 * ����  ��SPIд��Ĵ���
 * ����  ��reg:ָ���ļĴ�����ַ��value��д���ֵ
 * ���  ��status������״ֵ̬
 */ 
u8 MPU6500_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;
	MPU6500_CS(0);  										//ʹ��SPI����
	status = SPI5_Read_Write_Byte(reg&0x7f); //����д����+�Ĵ�����
	SPI5_Read_Write_Byte(value);				//д��Ĵ���ֵ
	MPU6500_CS(1);  										//��ֹMPU9500
	return(status);											//����״ֵ̬
}
/*
 * ��������MPU6500_Read_Reg
 * ����  ��SPI��ȡ�Ĵ���
 * ����  ��reg:ָ���ļĴ�����ַ
 * ���  ��reg_val��reg�Ĵ�����ַ��Ӧ��ֵ
 */ 
u8 MPU6500_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;
	MPU6500_CS(0);  										//ʹ��SPI����
	SPI5_Read_Write_Byte(reg | 0x80); 	//���Ͷ�����+�Ĵ�����
	reg_val = SPI5_Read_Write_Byte(0xff); //��ȡ�Ĵ���ֵ
	MPU6500_CS(1);  									  //��ֹMPU6500
	return(reg_val);
}

/*
 * ��������SPI5_Read_Write_Byte
 * ����  ����дһ���ֽ�
 * ����  ��TxData:Ҫд����ֽ�
 * ���  ����ȡ�����ֽ�
 */ 
u8 SPI5_Read_Write_Byte(uint8_t TxData)
{		
	u8 retry = 0;				 	
	while (SPI_I2S_GetFlagStatus(SPI5, SPI_I2S_FLAG_TXE) == RESET) 	//���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry > 250)	return 0;
		}			  
	SPI_I2S_SendData(SPI5, TxData); 																//ͨ������SPIx����һ������
	retry = 0;

	while (SPI_I2S_GetFlagStatus(SPI5, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry > 250) 
		return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI5); 															//����ͨ��SPIx������յ�����					    
}


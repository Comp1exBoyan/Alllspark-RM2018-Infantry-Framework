#ifndef __JUDGE_H
#define __JUDGE_H

#include "main.h"
#include "stm32f4xx.h"

#define JudgeBufferLength       150

extern u8 shoot_freq_flag;

extern uint8_t JudgeDataBuffer[JudgeBufferLength];
extern struct ttGameInfo tttGameInfo;
extern struct ttRealShootData tttRealShootData,tttLastShootData;

//��ʽת��������
typedef union
{
    uint8_t U[4];
    float F;
    int I;
}FormatTrans;

/***********************************    ��    ����ϵͳ���ݰ��ṹ��   ��  ***********************************/

//0-3 bits
#define Strike_0   0x00 //0��װ���棨ǰ��
#define Strike_1   0x01 //1��װ���棨��
#define Strike_2   0x02 //2��װ���棨��
#define Strike_3   0x03 //3��װ���棨�ң�
#define Strike_4   0x04 //4��װ���棨��1��
#define Strike_5   0x05 //5��װ���棨��2��
//4-7 bits
#define BC_Strike          0x00 //װ���˺����ܵ������� BloodChanged    BC
#define BC_ShootSpeed      0x01 //�ӵ����ٿ�Ѫ
#define BC_ShootFreq       0x02 //�ӵ���Ƶ��Ѫ
#define BC_PowerLimit      0x03 //���ʳ���
#define BC_ModuleOffline   0x04 //ģ�����߿�Ѫ
#define BC_CommonFoul      0x06 //��ͨ�����Ѫ
#define BC_Tarmac          0x0a //ͣ��ƺ��Ѫ
#define BC_EngineerOuto    0x0b //���̻������Զ���Ѫ


typedef __packed struct
{
	uint8_t SOF;          //������ʼ�ֽڣ��̶�Ϊ0xA5          
	uint16_t DataLength;  //���ݳ���
	uint8_t Seq;          //�����
	uint8_t CRC8;         //֡ͷCRCУ��
}tFrameHeader;//֡ͷ

typedef enum 
{
	GameInfo0 = 0x0001,     //����������Ϣ    ����Ƶ�� 50 Hz
	RealBloodChangedData0 = 0x0002,  //ʵʱѪ���仯����
	RealShootData0 = 0x0003,             //ʵʱ�������
	SelfDefinedData0 =0x0005, //ѧ���Զ�������
	Wrong = 0x1301 //ö����Ч��ֻ��Ϊ��ʹ��ö�ٴ�СΪ2�ֽ�
}tCmdID; 
typedef __packed struct
{
	uint8_t flag;
	float x;
	float y;
	float z;
	float compass; //��λ
}tLocData; //λ����Ϣ 

typedef __packed struct
{
	uint32_t remainTime;       //����ʣ��ʱ�䣨�ӵ���ʱ�����ӿ�ʼ���㣬��λ s��
	uint16_t remainLifeValue; //������ʣ��Ѫ��
	float realChassisOutV;    //ʵʱ���������ѹ����λV��
	float realChassisOutA;    //ʵʱ���������������λ A��
	tLocData locData;         //LOC ״̬���� tLocData �ṹ�嶨��
	float remainPower;        //ʣ����������ֵ 60J��
}tGameInfo; //����������Ϣ��0x0001��

typedef __packed struct
{
	uint8_t weakId:4;
	uint8_t way:4;
	uint16_t value;
}tRealBloodChangedData;//ʵʱѪ���仯���ݣ�0x0002��

typedef __packed struct
{
	float realBulletShootSpeed; //�ӵ�ʵʱ���٣�m/s��
	float realBulletShootFreq;  //�ӵ�ʵʱ��Ƶ���� /s��
	float realGolfShootSpeed;   //�߶���ʵʱ���٣��� /s��
	float realGolfShootFreq;    //�߶���ʵʱ��Ƶ ( �� /s Ӣ�ۻ����� )
}tRealShootData;    //ʵʱ�����Ϣ (0��0003)

__packed struct ttGameInfo
{
	uint32_t remainTime;       //����ʣ��ʱ�䣨�ӵ���ʱ�����ӿ�ʼ���㣬��λ s��
	uint16_t remainLifeValue; //������ʣ��Ѫ��
	float realChassisOutV;    //ʵʱ���������ѹ����λV��
	float realChassisOutA;    //ʵʱ���������������λ A��
	tLocData locData;         //LOC ״̬���� tLocData �ṹ�嶨��
	float remainPower;        //ʣ����������ֵ 60J��
}; //����������Ϣ��0x0001��

__packed struct ttRealBloodChangedData
{
	uint8_t weakId:4;
	uint8_t way:4;
	uint16_t value;
};//ʵʱѪ���仯���ݣ�0x0002��

__packed struct ttRealShootData
{
	float realBulletShootSpeed; //�ӵ�ʵʱ���٣�m/s��
	float realBulletShootFreq;  //�ӵ�ʵʱ��Ƶ���� /s��
	float realGolfShootSpeed;   //�߶���ʵʱ���٣��� /s��
	float realGolfShootFreq;    //�߶���ʵʱ��Ƶ ( �� /s Ӣ�ۻ����� )
};    //ʵʱ�����Ϣ (0��0003)

typedef __packed struct  
{  
     uint8_t RobotColor :2;  
     uint8_t RedBaseSta :2;  
     uint8_t BlueBaseSta:2;  
     uint8_t IslandLanding :2;  
  
     uint8_t RedAirPortSta :4;  
     uint8_t BlueAirPortSta:4;  
  
     uint8_t No1PillarSta:4;  
     uint8_t No2PillarSta:4;  
     uint8_t No3PillarSta:4;  
     uint8_t No4PillarSta:4;  
     uint8_t No5PillarSta:4;  
     uint8_t No6PillarSta:4;  
  
     uint8_t RedBulletBoxSta :4;  
     uint8_t BlueBulletBoxSta:4;  
     uint16_t RedBulletAmount;  
     uint16_t BlueBulletAmount;  

     uint8_t No0BigRuneSta :4;  
     uint8_t No1BigRuneSta :4;  
  
     uint8_t AddDefendPrecent;  
}tStudentPropInfo;

typedef __packed struct
{
	float data1;
	float data2;
	float data3;
}tSelfDefineInfo;   //ѧ���ϴ��Զ������� (0��0005)   ����Ƶ����� 200Hz.

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	__packed union 
	{
		tGameInfo       GameInfo0;    //����������Ϣ ��0x0001��
		tRealBloodChangedData   RealBloodChangedData0; //ʵʱѪ���仯���ݣ�0x0002��
		tRealShootData   RealShootData0; //ʵʱ�����Ϣ (0��0003)
		tSelfDefineInfo SelfDefineInfo0; //�Զ������� ��0x0005��	
	}Data;
	uint16_t        CRC16;         //֮ǰ��������CRCУ��   ע������ݺ�֮ǰ�����ݿ��ܲ����������Բ�Ҫֱ��ʹ�ã�����Ҫֱ��ʹ�ã������ڴ˸�ֵ
}tFrame;  //����֡


unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);
void USART2_Config(u32 bound);
void Judge_Data(uint8_t ucData);
void Send_FrameData(tCmdID cmdid, uint8_t * pchMessage,uint8_t dwLength); 
void USART_SendBuff(uint8_t * txmessage, uint8_t i);
void sendcustomdata(float tcustomdata1, float tcustomdata2, float tcustomdata3);

#endif

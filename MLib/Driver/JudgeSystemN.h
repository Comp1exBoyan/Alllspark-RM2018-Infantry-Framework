#ifndef JUDGE_SYSTEM_H
#define JUDGE_SYSTEM_H

#include "main.h"
#include "stm32f4xx.h"

#define CID_GameRobotStat  0x0001
#define CID_RobotHurt      0x0002
#define CID_ShootData      0x0003
#define CID_PowerHeatData  0x0004
#define CID_RfidDetect     0x0005
#define CID_GameResult     0x0006
#define CID_GetBuff        0x0007
#define CID_GameRobotPos   0x0008
#define CID_ShowData       0x0100
//��ʽת��������
#define SHOWDATA_LEN       22


/***********************************    ��    ����ϵͳ���ݰ��ṹ��   ��  ***********************************/
__packed struct tFrameHeader_t
{
	uint8_t SOF;          //������ʼ�ֽڣ��̶�Ϊ0xA5          
	uint16_t DataLength;  //���ݳ���
	uint8_t Seq;          //�����
	uint8_t CRC8;         //֡ͷCRCУ��
};                       //֡ͷ

//0x0001  ����������״̬
__packed struct exGameRobotState_t
{
    uint16_t stageRemianTime;    //��ǰ�׶�ʣ��ʱ��
    uint8_t  gameProgress;       //��ǰ�����׶�
    uint8_t  robotLevel;         //�����˵�ǰ�ȼ�
    uint16_t remainHP;           //�����˵�ǰѪ��
    uint16_t maxHP;              //��������Ѫ��
};
//0x00002 �˺�����
__packed struct extRobotHurt_t
{
    uint8_t armorType :4;        // ��ʾװ�װ�ID
    uint8_t hurtType  :4;        //Ѫ���仯����
};

//0x00003 ʵʱ�����Ϣ
__packed struct extShootData_t
{
    uint8_t bulletType;          //��������
    uint8_t bulletFreq;          //������Ƶ
    float   bulletSpeed;         //��������
};

//0x0004 ʵʱ������������
__packed struct extPowerHeatData_t
{
    float chassisVolt;           //���������ѹ
    float chassisCurrent;        //�����������
    float chassisPower;          //�����������
    float chassisPowerBuffer;    //���̹��ʻ���
    uint16_t shooterHeat0;       //17mmǹ������
    uint16_t shooterHeat1;       //42mmǹ������
};

//0x0005 ���ؽ�������
__packed struct extRfidDetect_t
{
    uint8_t cardType;             //�����ͣ�0-5��Ӧ  �����ӳɡ������ӳɡ��췽��Ѫ��������Ѫ���췽��������������������
    uint8_t cardIdx;              //��������
};

//0x0006 ����ʤ������
__packed struct extGameResult_t
{
    uint8_t winner;               //�������  0ƽ��   1�췽   2����
};

//0x0007 Buff��ȡ����
__packed struct extGetBuff_t
{
    uint8_t buffType;             //Buff���� �����ӳɣ������ӳɣ���ô���������
    uint8_t buffAddition;         //�ӳɰٷֱ�
};

//0x0008 ������λ�ó�����Ϣ
__packed struct extGameRobotPos_t
{
    float x;                      //λ��x����ֵ
    float y;                      //λ��y����ֵ
    float z;                      //λ��z����ֵ
    float yaw;                    //ǹ�ڳ���Ƕ�ֵ
};
__packed struct exShowData_t
{     
    float data1;
    float data2;
    float data3;
    uint8_t mask;   
};

__packed struct showDataFrame_t
{
    struct tFrameHeader_t FrameHeader;
    uint16_t CMD_ID;
    struct exShowData_t   ShowData;
    uint16_t CRC_16;
};

extern u8    JudgeDataFlag;
extern u8		 have_shoot;

extern u8    RobotLevel;
extern u16   ShooterHeat;
extern float ShooteSpeed;
extern float PowerOutput;
extern float PowerBuffLeft;
extern float ExPowerOut;

void SendCosumeData(float data1,float data2,float data3,u8 mask);
void USART_SendBuff(uint8_t * txmessage,uint8_t length);
void USART2_SendChar(unsigned char c);
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);
void USART2_Config(u32 bound);
void Handle_JudgeData(void);
void Clear_FrameFlag(u8 length);
    
#endif


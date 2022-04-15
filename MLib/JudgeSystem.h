#ifndef __JUDGE_H
#define __JUDGE_H

#include "main.h"
#include "stm32f4xx.h"

#define JudgeBufferLength       150

extern u8 shoot_freq_flag;

extern uint8_t JudgeDataBuffer[JudgeBufferLength];
extern struct ttGameInfo tttGameInfo;
extern struct ttRealShootData tttRealShootData,tttLastShootData;

//格式转换联合体
typedef union
{
    uint8_t U[4];
    float F;
    int I;
}FormatTrans;

/***********************************    ↓    裁判系统数据包结构体   ↓  ***********************************/

//0-3 bits
#define Strike_0   0x00 //0号装甲面（前）
#define Strike_1   0x01 //1号装甲面（左）
#define Strike_2   0x02 //2号装甲面（后）
#define Strike_3   0x03 //3号装甲面（右）
#define Strike_4   0x04 //4号装甲面（上1）
#define Strike_5   0x05 //5号装甲面（上2）
//4-7 bits
#define BC_Strike          0x00 //装甲伤害（受到攻击） BloodChanged    BC
#define BC_ShootSpeed      0x01 //子弹超速扣血
#define BC_ShootFreq       0x02 //子弹超频扣血
#define BC_PowerLimit      0x03 //功率超限
#define BC_ModuleOffline   0x04 //模块离线扣血
#define BC_CommonFoul      0x06 //普通犯规扣血
#define BC_Tarmac          0x0a //停机坪加血
#define BC_EngineerOuto    0x0b //工程机器人自动回血


typedef __packed struct
{
	uint8_t SOF;          //数据起始字节，固定为0xA5          
	uint16_t DataLength;  //数据长度
	uint8_t Seq;          //包序号
	uint8_t CRC8;         //帧头CRC校验
}tFrameHeader;//帧头

typedef enum 
{
	GameInfo0 = 0x0001,     //比赛进程信息    发送频率 50 Hz
	RealBloodChangedData0 = 0x0002,  //实时血量变化数据
	RealShootData0 = 0x0003,             //实时射击数据
	SelfDefinedData0 =0x0005, //学生自定义数据
	Wrong = 0x1301 //枚举无效，只是为了使该枚举大小为2字节
}tCmdID; 
typedef __packed struct
{
	uint8_t flag;
	float x;
	float y;
	float z;
	float compass; //方位
}tLocData; //位置信息 

typedef __packed struct
{
	uint32_t remainTime;       //比赛剩余时间（从倒计时三分钟开始计算，单位 s）
	uint16_t remainLifeValue; //机器人剩余血量
	float realChassisOutV;    //实时底盘输出电压（单位V）
	float realChassisOutA;    //实时底盘输出电流（单位 A）
	tLocData locData;         //LOC 状态，见 tLocData 结构体定义
	float remainPower;        //剩余能量（满值 60J）
}tGameInfo; //比赛进程信息（0x0001）

typedef __packed struct
{
	uint8_t weakId:4;
	uint8_t way:4;
	uint16_t value;
}tRealBloodChangedData;//实时血量变化数据（0x0002）

typedef __packed struct
{
	float realBulletShootSpeed; //子弹实时射速（m/s）
	float realBulletShootFreq;  //子弹实时射频（发 /s）
	float realGolfShootSpeed;   //高尔夫实时射速（发 /s）
	float realGolfShootFreq;    //高尔夫实时射频 ( 发 /s 英雄机器人 )
}tRealShootData;    //实时射击信息 (0×0003)

__packed struct ttGameInfo
{
	uint32_t remainTime;       //比赛剩余时间（从倒计时三分钟开始计算，单位 s）
	uint16_t remainLifeValue; //机器人剩余血量
	float realChassisOutV;    //实时底盘输出电压（单位V）
	float realChassisOutA;    //实时底盘输出电流（单位 A）
	tLocData locData;         //LOC 状态，见 tLocData 结构体定义
	float remainPower;        //剩余能量（满值 60J）
}; //比赛进程信息（0x0001）

__packed struct ttRealBloodChangedData
{
	uint8_t weakId:4;
	uint8_t way:4;
	uint16_t value;
};//实时血量变化数据（0x0002）

__packed struct ttRealShootData
{
	float realBulletShootSpeed; //子弹实时射速（m/s）
	float realBulletShootFreq;  //子弹实时射频（发 /s）
	float realGolfShootSpeed;   //高尔夫实时射速（发 /s）
	float realGolfShootFreq;    //高尔夫实时射频 ( 发 /s 英雄机器人 )
};    //实时射击信息 (0×0003)

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
}tSelfDefineInfo;   //学生上传自定义数据 (0×0005)   发送频率最大 200Hz.

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	__packed union 
	{
		tGameInfo       GameInfo0;    //比赛进程信息 （0x0001）
		tRealBloodChangedData   RealBloodChangedData0; //实时血量变化数据（0x0002）
		tRealShootData   RealShootData0; //实时射击信息 (0×0003)
		tSelfDefineInfo SelfDefineInfo0; //自定义数据 （0x0005）	
	}Data;
	uint16_t        CRC16;         //之前所有数据CRC校验   注意此数据和之前的数据可能不连续，所以不要直接使用，若需要直接使用，必须在此赋值
}tFrame;  //数据帧


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

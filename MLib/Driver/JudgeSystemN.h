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
//格式转换联合体
#define SHOWDATA_LEN       22


/***********************************    ↓    裁判系统数据包结构体   ↓  ***********************************/
__packed struct tFrameHeader_t
{
	uint8_t SOF;          //数据起始字节，固定为0xA5          
	uint16_t DataLength;  //数据长度
	uint8_t Seq;          //包序号
	uint8_t CRC8;         //帧头CRC校验
};                       //帧头

//0x0001  比赛机器人状态
__packed struct exGameRobotState_t
{
    uint16_t stageRemianTime;    //当前阶段剩余时间
    uint8_t  gameProgress;       //当前比赛阶段
    uint8_t  robotLevel;         //机器人当前等级
    uint16_t remainHP;           //机器人当前血量
    uint16_t maxHP;              //机器人满血量
};
//0x00002 伤害数据
__packed struct extRobotHurt_t
{
    uint8_t armorType :4;        // 表示装甲板ID
    uint8_t hurtType  :4;        //血量变化类型
};

//0x00003 实时射击信息
__packed struct extShootData_t
{
    uint8_t bulletType;          //弹丸类型
    uint8_t bulletFreq;          //弹丸射频
    float   bulletSpeed;         //弹丸射速
};

//0x0004 实时功率热量数据
__packed struct extPowerHeatData_t
{
    float chassisVolt;           //底盘输出电压
    float chassisCurrent;        //底盘输出电流
    float chassisPower;          //底盘输出功率
    float chassisPowerBuffer;    //底盘功率缓冲
    uint16_t shooterHeat0;       //17mm枪口热量
    uint16_t shooterHeat1;       //42mm枪口热量
};

//0x0005 场地交互数据
__packed struct extRfidDetect_t
{
    uint8_t cardType;             //卡类型（0-5对应  攻击加成、防御加成、红方加血、蓝方加血、红方大能量、蓝方大能量）
    uint8_t cardIdx;              //卡索引号
};

//0x0006 比赛胜负数据
__packed struct extGameResult_t
{
    uint8_t winner;               //比赛结果  0平局   1红方   2蓝方
};

//0x0007 Buff获取数据
__packed struct extGetBuff_t
{
    uint8_t buffType;             //Buff类型 攻击加成，防御加成，获得大能量机关
    uint8_t buffAddition;         //加成百分比
};

//0x0008 机器人位置朝向信息
__packed struct extGameRobotPos_t
{
    float x;                      //位置x坐标值
    float y;                      //位置y坐标值
    float z;                      //位置z坐标值
    float yaw;                    //枪口朝向角度值
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


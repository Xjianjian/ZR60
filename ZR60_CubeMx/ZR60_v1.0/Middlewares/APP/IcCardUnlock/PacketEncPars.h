#ifndef __READCARD_H
#define	__READCARD_H



#include "stm32f4xx.h"
#include <stdio.h>
#include "./wdata/wdata.h"
#include "Include.h"


#define OK  			0x00//正确获取卡号

#define QUIT  0x1e           // 上一次发送命令时被打断
#define COMM_ERR  0xff       // 串行通信错误
#define NO_TAG_ERR  0x01     // 在有效期内没有卡
#define CRC_ERR  2           // 从卡中接收到了错误的CRC校验和
#define NOT_AUTH_ERR  0x0a   // 卡没有验证
#define PARITY_ERR  5        // 从卡中接收到了错误的效验位
#define BIT_COUNT_ERR  0x0b  // 从卡中接收到了错误数量的位



 #define On_Card  0x00
        /**
         * 加载key
         */
#define Load_Key  0x01

        /**
         * 激活卡片并获取卡片号
         */
#define Get_CardSnr  0x02

        /**
         * 设置为主动检查卡片模式 IDLE模式
         */
#define SetDetectCard_ALL  0x03

        /**
         * 扇区0x14 存储了小区ID信息communityId
         */
#define Block_0x14  0x14

        /**
         * 扇区0x15 存储了楼栋编号信息buildNum1
         */
#define Block_0x15  0x15

        /**
         * 扇区0x16 村粗了楼栋编号信息buildNum2
         */
#define Block_0x16  0x16

/**
 * 扇区0x18 楼栋id
 */
#define Block_0x18  0x18

/**
 * 扇区0x19 门id
 */
#define Block_0x19  0x19

/**
 * 扇区0x20 下标
 */
#define Block_0x1A  0x1a

        /**
         * 数据块10, 旧IC卡存储用户ID块
         */
#define Block_0x10  0x10

        /**
         * 数据块11, 旧IC卡存储手机号码的块
         */
#define Block_0x11 0x11

#define ReadCard_VrfyCardNumber(x)  BListMng_u_ContrastBList(x)
#define ReadCard_CardSetSt   GetZR60Ctrl_u_CardSetSt

#define ReadCard_DEVICE_TYPE  machine_type

typedef struct 
{		
	char CardID[4U];//卡号
	char lng;//卡号长
}CardNumInfo; /*卡号信息结构体*/ 



typedef struct 
{		
	union
	{
		unsigned char Block[59];/*接收数据数*/
		struct
		{
			unsigned char Seqnr;/* 接收帧包号*/
			unsigned char St;/* 状态*/
			unsigned char DtLng;/* 接收有效数据段长度表示*/
			unsigned char ValidDt[56];/* 接收的有效数据段：按最大长度定义存储空间*/
		}DtSrt;
	}Dt;
}RebackInfo; /*报文接收结构体*/ 


extern uint8_t volatile Sereadcard_SetKeyflag;
extern uint8_t volatile open_door;
extern uint8_t volatile setcard ;
extern uint8_t volatile isSectedCard_flag;
extern struct comm_info sminfo;
extern struct comm_info sminfo1;
extern RebackInfo  rebackinfo;
extern CardNumInfo SeReadcard_h_CardID;//当前开门使用的ic卡卡号


extern void handleMsg(RebackInfo rebackInfo);
extern void Setreadcard_AutoSearchCard(void);
extern void Setreadcard_Key(void);

#endif

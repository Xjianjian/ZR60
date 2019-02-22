#ifndef __READCARD_H
#define	__READCARD_H



#include "stm32f4xx.h"
#include <stdio.h>
#include "./wdata/wdata.h"
#include "Include.h"


#define OK  			0x00//��ȷ��ȡ����

#define QUIT  0x1e           // ��һ�η�������ʱ�����
#define COMM_ERR  0xff       // ����ͨ�Ŵ���
#define NO_TAG_ERR  0x01     // ����Ч����û�п�
#define CRC_ERR  2           // �ӿ��н��յ��˴����CRCУ���
#define NOT_AUTH_ERR  0x0a   // ��û����֤
#define PARITY_ERR  5        // �ӿ��н��յ��˴����Ч��λ
#define BIT_COUNT_ERR  0x0b  // �ӿ��н��յ��˴���������λ



 #define On_Card  0x00
        /**
         * ����key
         */
#define Load_Key  0x01

        /**
         * ���Ƭ����ȡ��Ƭ��
         */
#define Get_CardSnr  0x02

        /**
         * ����Ϊ������鿨Ƭģʽ IDLEģʽ
         */
#define SetDetectCard_ALL  0x03

        /**
         * ����0x14 �洢��С��ID��ϢcommunityId
         */
#define Block_0x14  0x14

        /**
         * ����0x15 �洢��¥�������ϢbuildNum1
         */
#define Block_0x15  0x15

        /**
         * ����0x16 �����¥�������ϢbuildNum2
         */
#define Block_0x16  0x16

/**
 * ����0x18 ¥��id
 */
#define Block_0x18  0x18

/**
 * ����0x19 ��id
 */
#define Block_0x19  0x19

/**
 * ����0x20 �±�
 */
#define Block_0x1A  0x1a

        /**
         * ���ݿ�10, ��IC���洢�û�ID��
         */
#define Block_0x10  0x10

        /**
         * ���ݿ�11, ��IC���洢�ֻ�����Ŀ�
         */
#define Block_0x11 0x11

#define ReadCard_VrfyCardNumber(x)  BListMng_u_ContrastBList(x)
#define ReadCard_CardSetSt   GetZR60Ctrl_u_CardSetSt

#define ReadCard_DEVICE_TYPE  machine_type

typedef struct 
{		
	char CardID[4U];//����
	char lng;//���ų�
}CardNumInfo; /*������Ϣ�ṹ��*/ 



typedef struct 
{		
	union
	{
		unsigned char Block[59];/*����������*/
		struct
		{
			unsigned char Seqnr;/* ����֡����*/
			unsigned char St;/* ״̬*/
			unsigned char DtLng;/* ������Ч���ݶγ��ȱ�ʾ*/
			unsigned char ValidDt[56];/* ���յ���Ч���ݶΣ�����󳤶ȶ���洢�ռ�*/
		}DtSrt;
	}Dt;
}RebackInfo; /*���Ľ��սṹ��*/ 


extern uint8_t volatile Sereadcard_SetKeyflag;
extern uint8_t volatile open_door;
extern uint8_t volatile setcard ;
extern uint8_t volatile isSectedCard_flag;
extern struct comm_info sminfo;
extern struct comm_info sminfo1;
extern RebackInfo  rebackinfo;
extern CardNumInfo SeReadcard_h_CardID;//��ǰ����ʹ�õ�ic������


extern void handleMsg(RebackInfo rebackInfo);
extern void Setreadcard_AutoSearchCard(void);
extern void Setreadcard_Key(void);

#endif

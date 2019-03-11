/******************************************************
�ļ�����	UnlockLogCache

������		

Data			  Vasion			author
2017/7/22		   V1.0			    liujian
*******************************************************/
#ifndef		UNLOCKLOGCACHE_H
#define		UNLOCKLOGCACHE_H
/*******************************************************
description�� include the header file
*******************************************************/
#include "Include.h"

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/


/**********�곣������*********/
#define UnlockLogCache_QUEUE_LNG  100U/*���ݶ��г�*/
//#define UnlockLogCache_TIME_LNG  4U/*ʱ�����ݳ�*/
#define UnlockLogCache_ID_LNG  32U/*������Ϣ���ݳ�*/
//#define UnlockLogCache_EVENT_LNG  4U/*�¼����ݳ�*/
/***********�꺯��***********/

/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/*****struct definitions*****/
/*
0��ʾδ֪��1��ʾ�ſ�������2��ʾ���п�����
3��ʾ���뿪����4��ʾ�ֻ�NFC������5��ʾ������wifi������
6��ʾ����������������7��ʾԶ���뿪����8��ʾ΢�ſ�����
9��ʾ��ʱ���뿪����10��ʾ����ʶ������11��ʾ��ά�뿪����
12��ʾ���տ�����13���֤������14����,15���ڿ���,16��������
*/
/*
ˢ������id ; ���п��������š�����Ա���ֻ��� ; 
���뿪�������� ; NFC�������ݶ� ; ������wifi������ţ������� ; 
�����������ֻ��� ; Զ���뿪�����ֻ��š����Ļ�id ; ΢�ſ�����open_id ; 
��ʱ���뿪����δ֪ ; ����ʶ������{"user_id":"0001_1024_666_18183037622","face_token":"c48a71f1fff887617cd4528b531cb4d9"} ; 
��ά�뿪��: ¥����_�����/δ֪ ; ���տ�����¥����_�����/�ֻ��� ; ���֤���������֤����
*/
#define UNLOCKLOGCACHE_UNKNOW   	0U
#define UNLOCKLOGCACHE_CARD     	1U
#define UNLOCKLOGCACHE_CALL   		2U
#define UNLOCKLOGCACHE_PASSWORD   	3U
#define UNLOCKLOGCACHE_NFC     		4U
#define UNLOCKLOGCACHE_WIFI         5U
#define UNLOCKLOGCACHE_BLE         	6U
#define UNLOCKLOGCACHE_REMOTE         7U
#define UNLOCKLOGCACHE_WECHAT         	8U
#define UNLOCKLOGCACHE_TEMPPSWORD     	9U
#define UNLOCKLOGCACHE_FACE         	10U
#define UNLOCKLOGCACHE_TWODIME         	11U
#define UNLOCKLOGCACHE_SNAP         	12U
#define UNLOCKLOGCACHE_IDCARD         	13U
#define UNLOCKLOGCACHE_DOORSWITCH       14U
#define UNLOCKLOGCACHE_FIREALARM        15U
#define UNLOCKLOGCACHE_SOUNDWAVE        16U


typedef struct
{
	uint8  e_u_NonEmptyFlg;	/*���ݷǿձ�־*/
	uint64_t  e_u_Time;    /*ʱ��*/
	char  e_u_ID[UnlockLogCache_ID_LNG];/*������Ϣ*/
	uint8  e_u_Event;/*������ʽ*/
}UnlockLogCacheStruct;/*�����ź����ݶ��нṹ��*/


/******enum definitions******/

/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/

/*******************************************************
description�� function External declaration
*******************************************************/
extern void InitUnlockLogCache_Parameter(void);
extern void WrUnlockLogCache_Queue(uint64_t* Time,char* Id,uint8 Event);
extern uint8 GetUnlockLogCache_u_LogAvild(void);
extern void GetUnlockLogCache_doorLog(UnlockLogCacheStruct* UnlockLog);
extern void ClrUnlockLogCache_Queue(void);

#endif

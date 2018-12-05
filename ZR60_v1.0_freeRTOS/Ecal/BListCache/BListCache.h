/******************************************************
�ļ�����	BListCache

������		

Data			  Vasion			author
2017/7/22		   V1.0			    liujian
*******************************************************/
#ifndef		BLISTCACHE_H
#define		BLISTCACHE_H
/*******************************************************
description�� include the header file
*******************************************************/
#include	"Include.h"

/*******************************************************
description�� macro definitions
*******************************************************/
/**********�꿪�ض���*********/


/**********�곣������*********/
#define BLISTCACHE_QUEUE_LNG  100U/*���ݶ��г�,���г���Ϊһ����ȡ��������������2������ֹ��ʱ��������ȡ2�κ�����ʱ����������*/
#define BLISTCACHE_DATA_LNG   4U/*���ݳ�*/
/***********�꺯��***********/

/*******************************************************
description�� struct definitions
*******************************************************/

/*******************************************************
description�� typedef definitions
*******************************************************/
/*****struct definitions*****/
typedef struct
{
	uint8  e_u_NonEmptyFlg;	/*���ݷǿձ�־*/
	uint8  e_u_Data[BLISTCACHE_DATA_LNG];/*����*/
}BListCacheStruct;/*�����ź����ݶ��нṹ��*/


/******enum definitions******/

/******union definitions*****/

/*******************************************************
description�� variable External declaration
*******************************************************/

/*******************************************************
description�� function External declaration
*******************************************************/
extern void InitBListCache_Parameter(void);
extern void WrBListCache_BListQueue(const uint8* LeBListCache_u_data);
extern uint8 GetBListCache_u_BListValidity(void);
extern uint32 GetBListCache_dw_BlistData(void);
extern void ClrBListCache_Queue(void);

#endif

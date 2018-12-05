#ifndef __LHL_EDITTEXT_H_
#define __LHL_EDITTEXT_H_
#include "LHLGUI.h"

typedef struct EditText_Data
{
	struct EditText_Data *Next;
	
	GUI_HMEM datahmem;
	
	GUI_HMEM Nudehmem;
	short Datalen;
}EditText_DataTypeDef;

typedef struct
{
	void (*pTouchPro)(void *Data,GUI_POINT nowpoint,GUI_POINT add);//滑动触摸响应函数
	
	short x0;//起始坐标；
	short y0;//起始坐标；
	short xlen;//长;
	short ylen;//宽;
	short fx;
	short fy;
	
	short TmpDataLen;
	GUI_HMEM TmpDataHmem;
	int MemSize_Total;
	int MemSize_Used;
	int DeleteLineNum;
	
	EditText_DataTypeDef *pDataHead;//数据的链表头。
	short ShowLine;
	short TotalLine;
}LHL_EditTextTypeDef;

void EditText_Create(LHL_EditTextTypeDef *NewStruct,short x0,short y0,short xlen,short ylen);
void EditText_AddData(LHL_EditTextTypeDef *NewStruct,char *NewData,short datalen);
void EditText_ShowText(LHL_EditTextTypeDef *NewStruct,short numtoshow);

void EditText_TouchMovePro(void *Data,GUI_POINT nowpoint,GUI_POINT add);
void EditText_DeleteNude(LHL_EditTextTypeDef *NewStruct,int Index);
void EditText_ClearData(LHL_EditTextTypeDef *NewStruct);
void EditText_SetBufferSize(LHL_EditTextTypeDef *NewStruct,int size);
void EditText_DeleteLastData(LHL_EditTextTypeDef *NewStruct);

#endif

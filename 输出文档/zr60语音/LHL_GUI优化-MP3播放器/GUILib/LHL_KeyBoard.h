#ifndef __LHL_KEYBOARD_H_
#define __LHL_KEYBOARD_H_
#include "LHLGUI.h"
void KeyBoardCreate(void);
typedef struct
{
	short x0;
	short y0;
	short xlen;
	short ylen;
	LHL_NormalButtonTypeDef Key_TenNum[10];
	LHL_NormalButtonTypeDef Key_Enter;
	LHL_NormalButtonTypeDef Key_Back;
	LHL_NormalButtonTypeDef Key_Symbol;
	LHL_NormalButtonTypeDef Key_SymbolGrup[4];
	LHL_NormalButtonTypeDef Key_Shift;
	LHL_NormalButtonTypeDef Key_NumToABC;
	LHL_NormalButtonTypeDef Key_OK;
	
	LHL_NormalButtonTypeDef Key_DataEnter[5];
	char Key_DataEnterData[5];
	
	LHL_EditTextTypeDef EditText;
}KeyBoardTypeDef;
#endif

#ifndef __LHL_BUTTON_H_
#define __LHL_BUTTON_H_typedef
//#define BTNISFLOW				(char)1
//#define  BTNNOTFLOW			(char)0
#include "Bmp.h"
#include "LHL_GUICore.h"
void LHL_BottonCreate(LHLButtonTypeDef *Button,
											short x0,short y0,
											short xlen,short ylen,
											char IsShow,
											char *NotPressPic,
											char *PressPic,
											short Msg);
short BTN_ButtonMessageCheck(short xPos,short yPos,unsigned char Pressed);
void BTN_ButtonUpdate(LHLButtonTypeDef *Button,unsigned char State);
void BTN_HideBotton(LHLButtonTypeDef *Button,unsigned char State);
void BTN_SetCallBack(LHLButtonTypeDef *Button,
											void (*pf)(void *Handle,MsgType Messagesend));
void LHL_BottonChangPos(LHLButtonTypeDef *Button,
											short x0,short y0,
											short xlen,short ylen);
void LHL_BottonDel(LHLButtonTypeDef *Button);
											void LHL_ButtonpTouchUserHandleSet(LHLButtonTypeDef *Button,
												void (*pTouchUserHandle)(void *Data,GUI_POINT nowpoint,GUI_POINT add));
#endif

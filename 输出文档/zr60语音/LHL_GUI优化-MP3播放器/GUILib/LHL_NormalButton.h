#ifndef __LHL_NORMALBUTTON_H_
#define	__LHL_NORMALBUTTON_H_
#include "LHL_GUICore.h"
void NMBTN_NormalButtonCreate(LHL_NormalButtonTypeDef *NewButton,
														short x0,short y0,
														short xlen,short ylen,
														char IsShow,
														short Msg);
short NMBTN_ButtonMessageCheck(short xPos,short yPos,
														unsigned char Pressed);
void NMBTN_BottonStateRun(LHL_NormalButtonTypeDef *Button,
													short xPos,short yPos,
													unsigned char Pressed);
void NMBTN_ButtonUpdate(LHL_NormalButtonTypeDef *Button,
												unsigned char State);
void NMBTN_SetText(LHL_NormalButtonTypeDef *Button,char *pCh);
void NMBTN_SetTextColor(LHL_NormalButtonTypeDef *Button,
													unsigned short Color);
void NMBTN_SetPos(LHL_NormalButtonTypeDef *Button,
									unsigned short xPos,unsigned short yPos);
void NMBTN_SetSize(LHL_NormalButtonTypeDef *Button,
									unsigned short xLen,unsigned short yLen);
void NMBTN_SetCallBack(LHL_NormalButtonTypeDef *Button,
												void (*pf)(void *Handle,MsgType Messagesend));
void NMBTN_DelButton(LHL_NormalButtonTypeDef *Button);
void NMBTN_ButtonDisable(LHL_NormalButtonTypeDef *Button,unsigned char NewIsShow);
void NMBTN_SetInCallBack(LHL_NormalButtonTypeDef *Button,char NewState);
void NMBTN_SetColor(LHL_NormalButtonTypeDef *Button,unsigned short Color);
void NMBTN_SetPressedColor(LHL_NormalButtonTypeDef *Button,unsigned short PressedColor);
void NMBTN_SetStyle(LHL_NormalButtonTypeDef *Button,short Style);
void NMBTN_SetTextOffset(LHL_NormalButtonTypeDef *Button,short xOff,short yOff);
#endif

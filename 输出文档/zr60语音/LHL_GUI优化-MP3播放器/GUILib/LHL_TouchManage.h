#ifndef __LHL_TOUCHMANAGE_H_
#define __LHL_TOUCHMANAGE_H_
#include "LHLGUI.h"
short LHL_TouchProcessHandler(short xpos,short ypos,unsigned char Pressed);
void LHL_TouchGetPoint(GUI_POINT *point,unsigned char *ifispressed);


void LHL_TouchMoveAddMessage(void *Data);
void LHL_TouchMoveDeleteMesasge(void *Data);

void LHL_TouchMoveHandler(GUI_POINT nowpoint,GUI_POINT add);
void LHL_TouchGetMoveState(unsigned char *ifismove);
#endif

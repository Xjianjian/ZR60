#ifndef __LHL_DATAGRAPH_H_
#define __LHL_DATAGRAPH_H_
#include "LHLGUI.h"
void LHL_DataGraphCreate(LHL_DataGraphTypeDef *NewStruct,
													short x0,short y0,
													short xlen,short ylen);
void LHL_DataGraphDrawBack(LHL_DataGraphTypeDef *NewStruct);
void LHL_DataGraphDrawLine(LHL_DataGraphTypeDef *NewStruct);
void LHL_DataGraphClearLine(LHL_DataGraphTypeDef *NewStruct);
void LHL_DataGraphTouchProcess(void *Data,GUI_POINT nowpoint,GUI_POINT add);
void LHL_DataGraphMessageCheck(GUI_POINT nowpoint,GUI_POINT add);
void LHL_DataGraphDrawGraphLine(unsigned short x_b, unsigned short y_b, 
											unsigned short x_e, unsigned short y_e,
											unsigned short color);


void LHL_DataGraph_AddData(LHL_DataGraphTypeDef *NewStruct,short NewData);
void LHL_DataGraph_Updata(LHL_DataGraphTypeDef *NewStruct);
short LHL_DataGraphGetfx(LHL_DataGraphTypeDef *NewStruct);
void LHL_DataGraphSetfx(LHL_DataGraphTypeDef *NewStruct,int mfx);
void LHL_DataGraphSetCallBack(LHL_DataGraphTypeDef *NewStruct,void (*ReadData)(short *Buffer,short Datapos,short DataToRead),
															void (*WriteData)(short *Buffer,short DataToWrite));
															
void LHL_DataGraphDel(LHL_DataGraphTypeDef *NewStruct);
#endif

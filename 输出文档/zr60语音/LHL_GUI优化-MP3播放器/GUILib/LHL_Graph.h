#ifndef __LHL_GRAPH_H_
#define __LHL_GRAPH_H_
#include "LHL_GUICore.h"
void LHL_GraphCreate(LHL_GraphTypeDef *NewTypeDef,short x0,short y0
										,short xlen,short ylen,char IsShow);
void LHL_GraphReDrawAll(LHL_GraphTypeDef *NewTypeDef);
void LHL_GraphSetHLine(LHL_GraphTypeDef *NewTypeDef,unsigned char Num,unsigned short PosOfZero);
void LHL_GraphChangeLineColor(LHL_GraphTypeDef *NewTypeDef,unsigned short NewColor);
void LHL_GraphDrawLine(unsigned short x_b, unsigned short y_b, 
											unsigned short x_e, unsigned short y_e,
											unsigned short color);
void LHL_GraphDel(LHL_GraphTypeDef *NewTypeDef);
/*****************»æÍ¼ºËÐÄº¯Êý******************/
void LHL_GraphAddData(LHL_GraphTypeDef *NewTypeDef,short NewData);
void LHL_GraphClearLine(LHL_GraphTypeDef *NewTypeDef);
void LHL_GraphUpData(LHL_GraphTypeDef *NewTypeDef);
/************************************************/
void LHL_GraphPosationChange(LHL_GraphTypeDef *NewTypeDef,short x0,short y0,short xlen,short ylen);
#endif

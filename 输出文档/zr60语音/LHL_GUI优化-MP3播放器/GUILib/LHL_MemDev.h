#ifndef __LHL_MEMDEV_H_
#define __LHL_MEMDEV_H_
#define			GUI_MEMDEV_4BIT				4
#define			GUI_MEMDEV_8BIT				8
#define     GUI_MEMDEV_16BIT			16

#include "LHL_GUICore.h"

GUI_HMEM GUI_MemDevInit(MemDevHandle *handle,short Width,short Height,short Bit);
void GUI_MemDevSetColorPalettle(MemDevHandle *handle,const unsigned short *pColorPalettle);
void GUI_MemDevDrawPoint(MemDevHandle *handle,short x,short y,short color);
void GUI_MemDevDrawHLine(MemDevHandle *handle,short x1,short y,short x2,short color);
void GUI_MemDevDrawHLineWithData(MemDevHandle *handle,short x1,short y,short x2,unsigned short *color);
void GUI_MemDevCut(MemDevHandle *handle,short xb,short yb,
										short xe,short ye);
										
void  GUI_MemDevShow(MemDevHandle *handle,short x,short y);
void  GUI_MemDevDestroy(MemDevHandle *handle);

void GUICore_SetMemDev(MemDevHandle *handle);
void GUI_MemDevCopy(MemDevHandle *des,const MemDevHandle *src,short xpos,short ypos);
#endif

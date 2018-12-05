#ifndef __LHL_GUICORE_H_
#define	__LHL_GUICORE_H_
#include "Bmp.h"
#include "LoopList.h"
#include "LHL_GUICfg.h"
#include "LHL_GUIAlloc.h"
#define BTNINIT					(char)0	
#define BTNISPRESSED		(char)1
#define BTNNOTPRESSED		(char)2
#define 	TOUCHMANEGER_PRESSED			1
#define 	TOUCHMANEGER_NPRESSED			0
#define RGB(r,g,b)			((((r)>>3)<<11)|(((g)>>2)<<5)|(b)>>3)
#define GETR_FROM_RGB16(RGB565)   ((unsigned char)((  ((unsigned short int )RGB565)  >>11)<< 3))            //返回8 位 R    
#define GETG_FROM_RGB16(RGB565)   ((unsigned char)((  ((unsigned short int )(RGB565  & 0x7ff)) >>5)<< 2))   //返回8 位 G    
#define GETB_FROM_RGB16(RGB565)   ((unsigned char)((  ((unsigned short int )(RGB565  & 0x1f))<<3)))         //返回8 位 B    

#define MsgType					char	
enum{
	GUI_TRUE,
	GUI_FAULT,
	BTN_MOVEIN,
	BTN_MOVEOUT,
	BTN_MESSAGESEND,
	BTN_REDRAW_NOTPRESSED,
	BTN_REDRAW_PRESSED,
	
	BTN_STYLE_NORMAL,
	BTN_STYLE_OWNDRAW,
};
#define FONT_TYPE				unsigned char
#define TRANSPARENT			0x01
#define FILLWITHSPACE		0x00
#define FILLWITHZERO		0x01

/* LCD color */
#define GUI_RED	            0XF800
#define GUI_GREEN           0X07E0
#define GUI_BLUE            0X001F  
#define GUI_BRED            0XF81F
#define GUI_GRED            0XFFE0
#define GUI_GBLUE           0X07FF
#define GUI_BLACK           0
#define GUI_WHITE           0XFFFF  
#define GUI_YELLOW          0XFFF0

/* Define minimum and maximum coordinates in x and y */
#define GUI_XMIN -4095
#define GUI_XMAX  4095
#define GUI_YMIN -4095
#define GUI_YMAX  4095
typedef struct
{
	short x,y;
}GUI_POINT;
typedef struct
{
	short x0,y0,x1,y1;
}LCD_RECT;

typedef LCD_RECT        GUI_RECT;

typedef struct
{
	GUI_HMEM hmem;
	unsigned short bit;
	short width;
	short height;
	const unsigned short *ColorPalettle;
	GUI_POINT cut_Begin;
	GUI_POINT cut_End;
}MemDevHandle;

typedef struct
{
	void (*pTouchPro)(void *Data,GUI_POINT nowpoint,GUI_POINT add);
	void (*pTouchUserHandle)(void *Data,GUI_POINT nowpoint,GUI_POINT add);
	short x0;//起始坐标；
	short y0;//起始坐标；
	short xlen;//长;
	short ylen;//宽；
	char IsShow;//是否显示；
	BITMAPINFO NotPressPic;//未按下时的图标地址；
	BITMAPINFO PressPic;//按下时的图标地址;
	MsgType State;
	char Change;
	short Msg;
	void (*Callback)(void *Handle,MsgType Messagesend);
}LHLButtonTypeDef;

typedef struct
{
	char *pText;
	short FontxOff;
	short FontyOff;
	short x0;//起始坐标；
	short y0;//起始坐标；
	short xlen;//长;
	short ylen;//宽；
	char IsShow;//是否显示；
	char State;//当前状态
	char Change;//状态是否已经发生改变
	short Msg;//按键按下去时候产生的消息
	void (*Callback)(void *Handle,MsgType Messagesend);
	char IsInCallBack;
	unsigned short FontColor;
	unsigned short Color;
	unsigned short PressedColor;
	
/*****************************************/
	GUI_HMEM	Strhandle;//控件字符的句柄
	short Style;
}LHL_NormalButtonTypeDef;

typedef struct pButtonMsg
{
	void *Data;
	struct pButtonMsg *Next;
	GUI_HMEM	Memhandle;//控件的句柄
}pButtonMsg;
typedef struct
{
	unsigned short x;
	unsigned char  y;
}Point;

#define GRAPH_HIGH				399
typedef struct
{ 
	short x0;//起始坐标；
	short y0;//起始坐标；
	short xlen;//长;
	short ylen;//宽；
	char IsShow;//是否显示；
	unsigned short BKColor;//背景颜色
	unsigned short LineColor;//网格颜色
	unsigned short GraphColor;//曲线颜色
	unsigned char HLineNum;//水平网格数量
	unsigned char VLineNum;//垂直网格数量
	unsigned short HLineZeroPos;//水平0点位置
	short DataList[GRAPH_HIGH];//像素点绘图数据
	short MaxData;
}LHL_GraphTypeDef;

typedef struct
{
	void (*pTouchPro)(void *Data,GUI_POINT nowpoint,GUI_POINT add);
	short x0;//起始坐标；
	short y0;//起始坐标；
	short xlen;//长;
	short ylen;//宽；
	
	unsigned short BKColor;//背景颜色
	unsigned short LineColor;//网格颜色
	unsigned short GraphColor;//曲线颜色
	unsigned char HLineNum;//水平网格数量
	unsigned char VLineNum;//垂直网格数量
	
	short fx;//焦点坐标x
	short fy;//焦点坐标y
	
	GUI_HMEM DataInBufferhmem;//数据输入缓冲池
	GUI_HMEM ShowBufferhmem;//数据显示缓冲
	int ShowDataBufferLen;//缓冲池大小
	int ShowDataPosInBuffer;//显示缓冲区内数据显示位置
	int ShowDataWritePos;//显示缓冲区内数据写入位置
	int DataInBufferNum;//缓冲区内数据量
	
	void (*ReadData)(short *Buffer,short Datapos,short DataToRead);//数据读取回调函数
	void (*WriteData)(short *Buffer,short DataToWrite);//数据存储回调函数
}LHL_DataGraphTypeDef;

#include "LHL_GUIFillPolygon.h"
#include "LHL_MemDev.h"

void GUICore_Delay(unsigned int count);
void GUICore_DrawPoint(const unsigned short xPos,const unsigned short yPos,const unsigned short color);
unsigned char  GUICore_SetPointSize(unsigned char NewState);
void GUICore_DrawHLine(const short xPos1,const short yPos,const short xPos2,
											const unsigned short color);
void GUICore_DrawHLineWithData(const short xPos1,const short yPos,const short xPos2,
											unsigned short *ch);
void GUICore_DrawVLine(const short xPos,const short yPos1,const short yPos2,
											const unsigned short color);
void GUICore_DrawLine(unsigned short x_b, unsigned short y_b, 
											unsigned short x_e, unsigned short y_e,
											unsigned short color);
void GUICore_DrawCircle(unsigned short Xpos, unsigned short Ypos, 
												unsigned short Radius,unsigned short color);
void GUICore_circle(unsigned short color,unsigned short x_b,unsigned short y_b,
										unsigned short x_e,unsigned short y_e,unsigned short m);											
void GUICore_DrawRect(unsigned short x1, unsigned short y1,unsigned short x2
									,unsigned short y2 ,unsigned short color);
void GUICore_FillRect(unsigned short x1, unsigned short y1,unsigned short x2
									,unsigned short y2 ,unsigned short color);
void GUI_RotatePolygon(GUI_POINT* pDest, const GUI_POINT* pSrc, int NumPoints, float Angle);
void GUI_ChangeSize(GUI_POINT* pDest,GUI_POINT* pSrc,int NumPoints,float Size);
#endif

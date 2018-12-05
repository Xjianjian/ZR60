#include "includes.h"
#include "MGUI.h"
/*********************************************/
#ifdef			F407_FREERTOS
#else
#define		PI				3.141592654
#endif
/*************************私有成员变量*************************/
float MGUI_Speed=7*PI/6-PI/12;
short MGUI_Mile=0;
short MGUI_Temprature=0;
float MGUI_Energy=0.0f;
unsigned char MGUI_LockState=1;

enum
{
	MSG_PageDeleteUser,
	MSG_PageAddUser,
	MSG_PageNormal,
	MSG_PressFinger,
	MSG_FingerHansRecord,
	MSG_Retry,
	MSG_RegisterSuccess,
	MSG_RegisterFaild,
	MSG_DeleteSuccess,
	MSG_DeleteFaild,
};

unsigned char MGUI_FingerMsg=MSG_FingerHansRecord;
short MGUI_FingerID=1234;
unsigned char	MGUI_Page=MSG_PageNormal;
/*************************************************************/
/******************Exports*******************/
/**
  * @brief  设置表盘指针位置
  * @param  NewSpeed：速度，范围是0-21；
  * @retval None
  */
void MGUI_SetSpeed(float NewSpeed)
{
	if(NewSpeed<0||NewSpeed>21)
		return;
	MGUI_Speed=((21-NewSpeed)*7*PI)/(6*21)-PI/12;
}
/**
  * @brief  设置里程表
  * @param  NewMile：当前里程
  * @retval None
  */
void MGUI_SetMile(float NewMile)
{
	if(NewMile<0||NewMile>9999)
		return;
	MGUI_Mile=NewMile*10;
}
/**
  * @brief  设置温度
  * @param  NewTemprature：当前温度，范围 0到100
  * @retval None
  */
void MGUI_SetTemprature(float NewTemprature)
{
	if(NewTemprature<0||NewTemprature>99)
		return;
	MGUI_Temprature=NewTemprature*10;
}
/**
  * @brief  设置电量
  * @param  NewEnergy：当前电量。范围0到60
  * @retval None
  */
void MGUI_SetEnergy(float NewEnergy)
{
	if(NewEnergy<0||NewEnergy>60)
		return;
	MGUI_Energy=NewEnergy;
}
/**
  * @brief  设置解锁状态
  * @param  NewState：1代表锁住，0代表解开
  * @retval None
  */
void MGUI_SetLockState(unsigned char NewState)
{
	MGUI_LockState=NewState;
}
/**
  * @brief  设置需要显示的ID
  * @param  NewID：需要显示的ID
  * @retval None
  */
void MGUI_SetFingerID(int NewID)
{
	MGUI_FingerID=NewID;
}
/**
  * @brief  发送GUI消息
  * @param  Msg：要发送的消息
  * @retval None
	* @verbatim 在相应任务完成时需要调用此函数以通知GUI发生像一个的改变.一下是消息种类：
	* MSG_PageDeleteUser,通知GUI切换到指纹删除界面
	* MSG_PageAddUser,通知GUI切换到指纹添加界面
	* MSG_PageNormal,通知GUI切换到常规显示界面
	* MSG_PressFinger,通知GUI正在等在指纹按下
	* MSG_FingerHansRecord,通知GUI当前注册手指已经被注册
	* MSG_Retry,通知GUI显示重新尝试信息
	* MSG_RegisterSuccess,通知GUI指纹注册成功
	* MSG_RegisterFaild,通知GUI指纹注册失败
	* MSG_DeleteSuccess,通知GUI指纹删除成功
	* MSG_DeleteFaild,通知GUI指纹删除失败
  */
void MGUI_SendMsg(short NewMsg)
{
	if(NewMsg==MSG_PageDeleteUser||NewMsg==MSG_PageAddUser||NewMsg==MSG_PageNormal)
	{
		MGUI_Page=NewMsg;
	}
	else
	{
		MGUI_FingerMsg=NewMsg;
	}
}











/******************************************/
/******************************************/
/*****************************指针Style********************************/
static const GUI_POINT _aArrow[] = {
  {  0, 0},
  {-10, -40},
  {0, -180},
  {10, -40},
	{0,-40}
};
/***************************指针旋转所用的Buffer***********************/
GUI_POINT _aNew[10]={0};

GUI_HMEM mhandle;
MemDevHandle memhandle;
/******************************调色板数据******************************/
const unsigned short colorpalettle[]=
{
	RGB(255,255,255),
	RGB(5,90,144),
	RGB(196,64,42),
	RGB(0,0,255),
	RGB(255,0,0),
	RGB(0,255,0),
	RGB(255,255,0),
	RGB(0,0,0)
};
/****************************调色板索引数据****************************/
enum
{
	CP_WHITE=0,
	CP_GRAY,
	CP_UN,
	CP_BLUE,
	CP_RED,
	CP_GREEN,
	CP_YELLOW,
	CP_BLACK
};
void GetCutPos(GUI_POINT *PointTable,short number,GUI_POINT *Point1,GUI_POINT *Point2)
{
	short i;
	Point1->x=PointTable->x;
	Point1->y=PointTable->y;
	Point2->x=PointTable->x;
	Point2->y=(PointTable++)->y;
	for(i=1;i<number;i++)
	{
		if(PointTable->x<Point1->x)
			Point1->x=PointTable->x;
		if(PointTable->x>Point2->x)
			Point2->x=PointTable->x;
		if(PointTable->y<Point1->y)
			Point1->y=PointTable->y;
		if(PointTable->y>Point2->y)
			Point2->y=PointTable->y;
		PointTable++;
	}
}
GUI_HMEM SpeedPalehmem;
GUI_POINT *SpeedPale=NULL,*SpeedPale2=NULL;
unsigned short PointCount=0;

float Last_MGUI_Speed=7*PI/6-PI/12;
short Last_MGUI_Mile=0;
short Last_MGUI_Temprature=0;
float Last_MGUI_Energy=0.0f;
unsigned char Last_MGUI_LockState=1;
unsigned char RedrawAll=1;
void MGUI_Init(void)
{
	SpeedPalehmem=GUI_ALLOC_AllocNoInit(sizeof(GUI_POINT)*260);
	SpeedPale=(GUI_POINT *)GUI_ALLOC_h2p(SpeedPalehmem);
	SpeedPale2=SpeedPale;
	{
		float tAngle=-PI/12;
		for(;tAngle<PI/12;tAngle+=0.03f)
		{
			SpeedPale2->y=150*sinf(-tAngle);
			SpeedPale2->x=150*cosf(tAngle);
			PointCount++;
			SpeedPale2++;
		}
		tAngle-=0.02f;
		for(;tAngle>=-PI/12;tAngle-=0.03f)
		{
			SpeedPale2->y=125*sinf(-tAngle);
			SpeedPale2->x=125*cosf(tAngle);
			PointCount++;//PointCount最后等于36.
			SpeedPale2++;
		}
		{
			short i;
			for(i=1;i<7;i++)
			GUI_RotatePolygon(SpeedPale+i*36,SpeedPale,36,i*PI/6);
		}
	}
	GUI_RotatePolygon(_aNew,_aArrow,5,Last_MGUI_Speed-PI/2);//旋转指针
	GUI_MemDevInit(&memhandle,320,240,GUI_MEMDEV_4BIT);//初始化内存设备
	GUI_MemDevSetColorPalettle(&memhandle,colorpalettle);//选择自定义调色板
	GUICore_SetMemDev(&memhandle);//选择内存设备为当前绘图设备
	GUI_RotatePolygon(_aNew,_aArrow,5,MGUI_Speed-PI/2);//旋转指针
	MGUI_ProcessHandler();
}
const char mCN_1[]={17,22,23,1,0};//“请按手指”
const char mCN_2[]={1,2,3,4,0};//"指纹已在" 
const char mCN_3[]={5,6,7,8,0};//"管理员库"
const char mCN_4[]={15,16,0};//"失败"
const char mCN_5[]={17,18,19,0};//"请重试"
const char mCN_6[]={9,10,11,12,9,10,0};//"注册成功注册"
const char mCN_7[]={9,10,15,16,0};//"注册失败"
const char mCN_8[]={20,21,11,12,0};//"删除成功"
const char mCN_9[]={20,21,15,16,0};//"删除失败"
const	char mCN_10[]={13,14,0};//“号是”
const char mCN_11[]={20,21,1,2,0};//"删除指纹"
const char mCN_12[]={24,25,1,2,0};//“添加指纹”
const char mCN_13[]={9,10,13,14,0};//“注册号是”
short Last_MGUI_Page=-1;
short Last_MGUI_FingerMsg=-1;
void MGUI_FingerManager(void)
{
	if(Last_MGUI_Page==MGUI_Page&&Last_MGUI_FingerMsg==MGUI_FingerMsg)
	{
		if(MGUI_Page==MSG_PageDeleteUser)
		{
		}
		else if(MGUI_Page==MSG_PageAddUser)
		{
		}
		return ;
	}
	else
	{
		Last_MGUI_Page=MGUI_Page;
		Last_MGUI_FingerMsg=MGUI_FingerMsg;
	}
	GUI_MemDevCut(&memhandle,0,0,memhandle.width-1,memhandle.height-1);
	GUICore_FillRect(0,0,319,239,7);//背景填充
	ImageShow(gImage_USERManager,0,-4,CP_BLACK,CP_WHITE);
	if(MGUI_Page==MSG_PageDeleteUser)
	{
		MyString_PutStringAt((char *)mCN_11,FONT_CH_64x64,105,10,CP_BLUE,CP_WHITE,0);	
	}
	else if(MGUI_Page==MSG_PageAddUser)
	{
		MyString_PutStringAt((char *)mCN_12,FONT_CH_64x64,105,10,CP_BLUE,CP_WHITE,0);	
	}
	switch(MGUI_FingerMsg)
	{
	case MSG_PressFinger:
		MyString_PutStringAt((char *)mCN_1,FONT_CH_64x64,70,102,CP_WHITE,1,1);
		break;
	case MSG_FingerHansRecord:
	{
		char ptmp[6];
		sprintf(ptmp,"%d",(short)MGUI_FingerID);
		MyString_PutStringAt((char *)mCN_2,FONT_CH_64x64,70,80,CP_WHITE,1,1);
		MyString_PutStringAt((char *)mCN_3,FONT_CH_64x64,70,80+48,CP_WHITE,1,1);
		MyString_PutStringAt((char *)mCN_13,FONT_CH_64x64,0,80+96,CP_WHITE,1,1);
		MyString_PutStringAt(ptmp,FONT_BLACK,210,80+96+16,0,1,1);
		break;
	}
	case MSG_Retry:
		MyString_PutStringAt((char *)mCN_4,FONT_CH_64x64,106,100,CP_WHITE,1,1);
		MyString_PutStringAt((char *)mCN_5,FONT_CH_64x64,83,100+48,CP_WHITE,1,1);
		break;
	case MSG_RegisterSuccess:
	{
		char ptmp[6];
		sprintf(ptmp,"%d",(short)MGUI_FingerID);
		MyString_PutStringAt((char *)mCN_6,FONT_CH_64x64,0,80,CP_WHITE,1,1);
		MyString_PutStringAt((char *)mCN_10,FONT_CH_64x64,0,80+48,CP_WHITE,1,1);
		MyString_PutStringAt(ptmp,FONT_BLACK,100,80+48+16,0,1,1);
		break;
	}
	case MSG_RegisterFaild:
		ImageShow(gImage_Error,110,64,CP_BLACK,CP_GREEN);
		MyString_PutStringAt((char *)mCN_7,FONT_CH_64x64,72,80+100,CP_WHITE,1,1);
		break;
	case MSG_DeleteSuccess:
		ImageShow(gImage_Right,110,64,CP_BLACK,CP_GREEN);
		MyString_PutStringAt((char *)mCN_8,FONT_CH_64x64,72,80+100,CP_WHITE,1,1);
		break;
	case MSG_DeleteFaild:
		ImageShow(gImage_Error,110,64,CP_BLACK,CP_GREEN);
		MyString_PutStringAt((char *)mCN_9,FONT_CH_64x64,72,80+100,CP_WHITE,1,1);
		break;
	}
	GUI_MemDevShow(&memhandle,0,0);//将绘制好的图像贴到屏幕上
}

void MGUI_Process(void)
{
	if(RedrawAll)
	{
		GUI_MemDevCut(&memhandle,0,0,memhandle.width-1,memhandle.height-1);
		GUICore_FillRect(0,0,319,239,7);//背景填充
	}
/************************画温度计**********************/
{
	if(Last_MGUI_Temprature!=MGUI_Temprature||RedrawAll)
	{
		if(!RedrawAll)
		GUI_MemDevCut(&memhandle,0,0,150,150);
		DisplayDigit(Last_MGUI_Temprature/10,20,10,0.7,CP_BLACK,2);
		DisplayDigit(Last_MGUI_Temprature%10,20+70,30,0.4,CP_BLACK,1);
		ImageShow(gImage_temprature,2,2,CP_GREEN,CP_BLACK);
		DisplayDigit(MGUI_Temprature/10,20,10,0.7,CP_GREEN,2);
		DisplayDigit(MGUI_Temprature%10,20+70,30,0.4,CP_RED,1);
		Last_MGUI_Temprature=MGUI_Temprature;
		if(!RedrawAll)
		GUI_MemDevShow(&memhandle,0,0);//将绘制好的图像贴到屏幕上

	}
}
/***********************画电池*************************/
#define BATTERY_POS_X			265
#define	BATTERY_POS_Y			2
{
	if(Last_MGUI_Energy!=MGUI_Energy||RedrawAll)
	{
		static short BatteryColor=CP_GREEN;
		if(MGUI_Energy>30)
			BatteryColor=CP_GREEN;
		else if(MGUI_Energy>10)
			BatteryColor=CP_YELLOW;
		else
			BatteryColor=CP_RED;
		/***********************清除残影***********************/
		if(!RedrawAll)
		GUI_MemDevCut(&memhandle,BATTERY_POS_X,BATTERY_POS_Y,BATTERY_POS_X+40,BATTERY_POS_Y+72);
		GUICore_FillRect(BATTERY_POS_X,BATTERY_POS_Y+10,BATTERY_POS_X+40,BATTERY_POS_Y+72,CP_BLACK);
		GUICore_DrawRect(BATTERY_POS_X,BATTERY_POS_Y+10,BATTERY_POS_X+40,BATTERY_POS_Y+72,BatteryColor);
		GUICore_FillRect(BATTERY_POS_X+8,BATTERY_POS_Y,BATTERY_POS_X+32,BATTERY_POS_Y+10,BatteryColor);
		GUICore_FillRect(BATTERY_POS_X+2,BATTERY_POS_Y-MGUI_Energy+72,BATTERY_POS_X+38,BATTERY_POS_Y+70,BatteryColor);
		Last_MGUI_Energy=MGUI_Energy;
		if(!RedrawAll)
		GUI_MemDevShow(&memhandle,0,0);
	}
}
/***********************画表盘*************************/
	{
		if(Last_MGUI_Speed!=MGUI_Speed||RedrawAll||Last_MGUI_Mile!=MGUI_Mile)
		{
			short i=(Last_MGUI_Speed+PI/12)/(PI/6);
			short color;
			GUI_POINT mp1,mp2;
			short mcenterminx,mcenterminy;
			short mcentermaxx,mcentermaxy;
			memcpy(&_aNew[5],_aNew,sizeof(GUI_POINT)*5);
			GUI_RotatePolygon(_aNew,_aArrow,5,MGUI_Speed-PI/2);//旋转指针
			if(_aNew[4].x<_aNew[9].x)
			{
				mcenterminx=_aNew[4].x;
				mcentermaxx=_aNew[9].x;
			}
			else
			{
				mcenterminx=_aNew[9].x;
				mcentermaxx=_aNew[4].x;
			}
			if(_aNew[4].y<_aNew[9].y)
			{
				mcenterminy=_aNew[4].y;
				mcentermaxy=_aNew[9].y;
			}
			else
			{
				mcenterminy=_aNew[9].y;
				mcentermaxy=_aNew[4].y;
			}
			if(!RedrawAll)
			{
				if(Last_MGUI_Mile!=MGUI_Mile)
				{
					GUI_MemDevCut(&memhandle,102,145,219,194);
					DisplayDigit(Last_MGUI_Mile%10,207,163,0.3,CP_BLACK,1);
					DisplayDigit(Last_MGUI_Mile/10,107,150,0.5,CP_BLACK,4);
					DisplayDigit(MGUI_Mile%10,207,163,0.3,CP_RED,1);
					DisplayDigit(MGUI_Mile/10,107,150,0.5,6,4);
					GUI_MemDevShow(&memhandle,0,0);
				}
				GetCutPos(_aNew,10,&mp1,&mp2);
				GUI_MemDevCut(&memhandle,160-mcentermaxx+mp1.x,200-mcentermaxy+mp1.y,160-mcenterminx+mp2.x,200-mcenterminy+mp2.y);
			}
			Last_MGUI_Mile=MGUI_Mile;
			/***********************清除残影***********************/
			GUI_FillPolygon(_aNew+5, 4, 160-_aNew[9].x, 200-_aNew[9].y,CP_BLACK);//画指针
		/******************************************************/
			DisplayDigit(MGUI_Mile%10,207,163,0.3,CP_RED,1);
			DisplayDigit(MGUI_Mile/10,107,150,0.5,6,4);
			
			MyString_PutStringAt("0",FONT_BLACK,40,206,0,1,1);
			MyString_PutStringAt("3",FONT_BLACK,48,158,0,1,1);
			MyString_PutStringAt("6",FONT_BLACK,70,110,0,1,1);
			MyString_PutStringAt("9",FONT_BLACK,123,84,0,1,1);
			MyString_PutStringAt("12",FONT_BLACK,170,84,0,1,1);
			MyString_PutStringAt("15",FONT_BLACK,220,110,0,1,1);
			MyString_PutStringAt("18",FONT_BLACK,246,161,0,1,1);
			MyString_PutStringAt("21",FONT_BLACK,248,207,0,1,1);
			MyString_PutStringAt("KM/H",FONT_BLACK,126,117,4,1,1);
			if(i<7)
			{
				if(i==0)color=4;
				else if(i<3)color=6;
				else color=5;
				GUI_FillPolygon(SpeedPale+i*36, PointCount, 160, 200,color);//画仪表盘
			}
			if(RedrawAll)
			{
				for(i=0;i<7;i++)
				{
					if(i==0)color=4;
					else if(i<3)color=6;
					else color=5;
					GUI_FillPolygon(SpeedPale+i*36, PointCount, 160, 200,color);//画仪表盘
				}
			}
			GUI_FillPolygon(_aNew, 4, 160-_aNew[4].x, 200-_aNew[4].y,0);//画指针
			GUICore_circle(7,160-8,200-8,160+8,200+8,2);
			Last_MGUI_Speed=MGUI_Speed;
			GUI_MemDevShow(&memhandle,0,0);//将绘制好的图像贴到屏幕上
		}
	}
/***********************画解锁状态*********************/
{
	if(Last_MGUI_LockState!=MGUI_LockState||RedrawAll)
	{
		GUI_MemDevCut(&memhandle,193,0,193+70,70);
		if(!MGUI_LockState)
			ImageShow(gImage_Welcom,193,0,CP_GREEN,CP_BLACK);
		else
			ImageShow(gImage_Locker,193,0,CP_RED,CP_BLACK);
		Last_MGUI_LockState=MGUI_LockState;
		GUI_MemDevShow(&memhandle,0,0);//将绘制好的图像贴到屏幕上
	}
}
	RedrawAll=0;
#ifdef  F407_FREERTOS
/**********************F407触摸屏处理************************/
//	{
//		unsigned short x,y;
//		ADS7843_Rd_Addata(&x,&y);
//		if(x>0&&x<320&&y>0&&y<239)
//		{
//			GUI_MemDevCut(&memhandle,0,0,120,100);
//			LHL_FontShowNumDec(2,2,x,CP_BLACK,CP_WHITE,0,3);
//			LHL_FontShowNumDec(2,20,y,CP_BLACK,CP_WHITE,0,3);
//			GUI_MemDevShow(&memhandle,0,0);//将绘制好的图像贴到屏幕上
//		}
//	}
#endif
}
void MGUI_ProcessHandler(void)
{
	switch(MGUI_Page)
	{
		case MSG_PageNormal:
			MGUI_Process();
			Last_MGUI_Page=MGUI_Page;
		break;
		case MSG_PageAddUser:
			MGUI_FingerManager();
			RedrawAll=1;
		break;
		case MSG_PageDeleteUser:
			MGUI_FingerManager();
			RedrawAll=1;
		break;
		default:
		break;
	}
}

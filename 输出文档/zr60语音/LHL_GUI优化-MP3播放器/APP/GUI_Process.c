#include "includes.h"
#include "LHLGUI.h"
#include "arm_math.h"
#include "LHL_MP3_Decoder.h"

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

void CreatGUITask(void)
{
		xTaskCreate( GUI_Process_TaskBody, ( signed portCHAR * ) "GUI_Pro", StackSize_GUIPROCESS, NULL, 1, NULL );	
}

void fft_compute(short *pdata);

void _cb_mButton(void *Handle,MsgType msg);
void _cb_TouchProcess(void *Data,GUI_POINT nowpoint,GUI_POINT add);
void _cb_TouchMessage(void *Handle,MsgType msg);
#define TEST_LENGTH_SAMPLES 2048

float fftbuf[TEST_LENGTH_SAMPLES];
float testOutput[TEST_LENGTH_SAMPLES/2];
/* ------------------------------------------------------------------ 
* Global variables for FFT Bin Example 
* ------------------------------------------------------------------- */ 
uint32_t fftSize = 1024; 
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1; 
uint32_t testIndex = 0;
short *pfftdata=NULL;
unsigned char ypointg[320]={0};
void fft_setdata(short *pdata)
{
	pfftdata=pdata;
}
void fft_compute(short *pdata)
{
	arm_cfft_radix4_instance_f32 S; 
	float32_t maxValue; 
	
	short i,j=TEST_LENGTH_SAMPLES/2;
	for(i=0;i<j;i++)
	{
		fftbuf[2*i]=pdata[2*i];
		fftbuf[2*i+1]=0;
		testOutput[i]=0;
	}

	/* Initialize the CFFT/CIFFT module */  
	arm_cfft_radix4_init_f32(&S, fftSize,  
	  								ifftFlag, doBitReverse); 
	 
	/* Process the data through the CFFT/CIFFT module */ 
	arm_cfft_radix4_f32(&S, fftbuf); 
	/* Process the data through the Complex Magnitude Module for  
	calculating the magnitude at each bin */ 
	arm_cmplx_mag_f32(fftbuf, testOutput,  
	  				fftSize);  
	 
	/* Calculates maxValue and returns corresponding BIN value */ 
	arm_max_f32(testOutput, fftSize, &maxValue, &testIndex); 
	
	for(i=50;i<370;i++)
	{
		float mtmp=0;
		short mj = 0;
		for(mj = 0;mj < 20 ;mj ++)
		{
			mtmp+=testOutput[i];
		}
		mtmp/=80000;
		
		if(mtmp>200)
		{
			mtmp=200;
		}
		
		if(mtmp>ypointg[i-50])
		{
			ypointg[i-50] = mtmp;
		}
		else
		{
			if(ypointg[i-50] >= 3)
			{
				ypointg[i-50]-=3;
			}
			else
			{
				ypointg[i-50] = 0;
			}
		}
		GUICore_DrawVLine((i-50),110-mtmp,110+mtmp,CP_BLUE);

		GUICore_DrawPoint((i-50),110-ypointg[i-50],CP_RED);
		GUICore_DrawPoint((i-50),110+ypointg[i-50],CP_RED);

	}
}

FIL writefile;

FRESULT scan_files (char* path)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
	int i;
	char *fn;

#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif
	
	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
		i = strlen(path);
		
		for (;;)
		{
			 res = f_readdir(&dir, &fno);
			 if (res != FR_OK || fno.fname[0] == 0)
				 break;
			 if (fno.fname[0] == '.')
				 continue;
#if _USE_LFN
			fn = *fno.lfname ? fno.lfname : fno.fname;
			#else
			fn = fno.fname;
			#endif
			if(fno.fattrib & AM_DIR)
			{
				sprintf(&path[i], "/%s", fn);
				res = scan_files(path);
				
				if (res != FR_OK) break;
				path[i] = 0;
			}
			else
			{
				char mchbuf[100];
				short iname=strlen(fn);
				unsigned int er;
				if(strcmp(&fn[iname-4],".MP3") == 0 || strcmp(&fn[iname-4],".mp3")==0)
				{
					sprintf(mchbuf,"%s/%s\r\n", path, fn);
//					LHLFont_DebugString(mchbuf);
					f_write(&writefile,mchbuf,strlen(mchbuf),&er);
				}
			}
		}
	}
	return res;
}

pMP3Player_TypeDef mpmp3Player[2];

void FindMusicWithIndex(int Index,char *namebuf)
{
	FIL list_music_read;
	GUI_HMEM filenamebufmem = GUI_ALLOC_AllocNoInit(512);
	char *filenamebuf = (char *)GUI_ALLOC_h2p(filenamebufmem);
	unsigned int readlen;
	
	short i;
	short nowindex = 0;
	short readflag = 0;
	
	if(Index == 0)
	{
		readflag = 1;
	}
	
	if(f_open( &list_music_read,"0:list_music.txt",FA_READ) != 0)
	{
		GUI_ALLOC_Free(filenamebufmem);
		return;
	}
	
	while(1)
	{
		f_read(&list_music_read,filenamebuf,512,&readlen);
		if(readlen == 0)
		{
			f_close(&list_music_read);
			GUI_ALLOC_Free(filenamebufmem);
			break;
		}
		for(i=0; i<readlen; i++)
		{
			switch(readflag)
			{
				case 0:
					if(filenamebuf[i] == '\n')
					{
						nowindex++;
						if(nowindex == Index)
						{
							readflag = 1;
						}
					}
					break;
				case 1:
					if(filenamebuf[i] != '\r' && filenamebuf[i] != '\n')
					{
						*namebuf++ = filenamebuf[i];
					}
					else
					{
						readflag = 2;
						*namebuf = '\0';
					}
					break;
				default:
					break;
			}
			
			if(readflag == 2)
			{
				f_close(&list_music_read);
				GUI_ALLOC_Free(filenamebufmem);
			}
		}
	}
}
char filenamebuf[255]="0:";
short nowplayindex=1;

LHL_NormalButtonTypeDef nextsong;
MemDevHandle memhandle,litmemhandle;
LHLButtonTypeDef mButton_ad;
LHLButtonTypeDef pause_button;

short ypressedpos = 0;
int button_focus_y = 0;
int index_select = 1;

short waveIsPressed_delayTime = 0;
extern volatile unsigned char fatfs_lock;

void GUI_Process_TaskBody(void *p)
{
	short x,y,m;
	
	BSP_Init();
	
	LHL_BottonCreate(&mButton_ad,0,0,320,200,1,NULL,NULL,0);
	LHL_ButtonpTouchUserHandleSet(&mButton_ad,&_cb_TouchProcess);
	BTN_SetCallBack(&mButton_ad,&_cb_TouchMessage);
	
	LHL_MP3_Decoer_Init();
	
	if(f_open( &writefile,"0:list_music.txt", FA_CREATE_ALWAYS  | FA_WRITE) != 0)
	{
	}
	
	scan_files(filenamebuf);

	f_close(&writefile);

	GUI_MemDevInit(&litmemhandle,80,20,GUI_MEMDEV_4BIT);
	GUI_MemDevSetColorPalettle(&litmemhandle,colorpalettle);
	GUICore_SetMemDev(&litmemhandle);
	GUICore_FillRect(0,0,80,20,CP_BLACK);
	LHL_FontPutStringAt(0,0,"Hello,I'm Li Hailong",CP_YELLOW,CP_BLACK,0);
	GUICore_SetMemDev(NULL);
	while(1)
	{
		static short nx=0,ny=0,xnd=0,ynd=0;
		if(xnd)
		{
			nx--;
			if(nx <=0)
			{
				xnd = 0;
			}
		}
		else
		{
			nx++;
			if(nx >=300)
			{
				xnd = 1;
			}
		}
		if(ynd)
		{
			ny--;
			if(ny <=0)
			{
				ynd = 0;
			}
		}
		else
		{
			ny++;
			if(ny >=190)
			{
				ynd = 1;
			}
		}
		ADS7843_Rd_Addata((u16 *)&x,(u16 *)&y);
 		if((short)x<0)
 		{
 			m=TOUCHMANEGER_NPRESSED;
 		}
 		else
 			m=TOUCHMANEGER_PRESSED;
 		LHL_TouchProcessHandler(x,y,m);

//		vTaskDelay(500);
		
		if(mpmp3Player[1] == NULL)
		{
			short i;
			FindMusicWithIndex(nowplayindex++,filenamebuf);
			
			GUI_MemDevDestroy(&litmemhandle);
			
			i = strlen(filenamebuf)-1;
			while(filenamebuf[i]!='/')
			{
				i--;
			}
			i++;
			GUI_MemDevInit(&litmemhandle,8*strlen(filenamebuf+i),16,GUI_MEMDEV_4BIT);
			GUI_MemDevSetColorPalettle(&litmemhandle,colorpalettle);
			GUICore_SetMemDev(&litmemhandle);
			LHL_FontPutStringAt(0,0,filenamebuf+i,CP_YELLOW,CP_BLACK,0);
			GUICore_SetMemDev(NULL);
			
			LHL_MP3_DecoderCreate(&mpmp3Player[1],filenamebuf);
		}
		
		GUI_MemDevInit(&memhandle,320,200,GUI_MEMDEV_4BIT);
		GUI_MemDevSetColorPalettle(&memhandle,colorpalettle);
		GUICore_SetMemDev(&memhandle);

		fft_compute(pfftdata);
		GUI_MemDevCopy(&memhandle,&litmemhandle,nx,ny);	

		if(waveIsPressed_delayTime)
		{
			short tmp_yadd = 0;
			short iadd = 0;
			short i;
			waveIsPressed_delayTime--;
			GUICore_FillRect(30,10,260,190,CP_GREEN);
			for(tmp_yadd = 10-button_focus_y%22;tmp_yadd<=200-button_focus_y%22;tmp_yadd+=22)
			{
				if(tmp_yadd < 10 || tmp_yadd>170)
				{
					continue;
				}
				while(fatfs_lock)
				{
					vTaskDelay(1);
				}
				fatfs_lock = 1;
				FindMusicWithIndex(nowplayindex+(iadd++)+button_focus_y/22,filenamebuf);
				i = strlen(filenamebuf)-1;
				while(filenamebuf[i]!='/')
				{
					i--;
				}
				i++;
				
				if(ypressedpos<tmp_yadd+22 && ypressedpos>tmp_yadd)
				{
					GUICore_FillRect(30,tmp_yadd,260,tmp_yadd+22,CP_GRAY);
					index_select = nowplayindex+(iadd-1)+button_focus_y/22;
				}
				LHL_FontPutStringAt(30,tmp_yadd+3,filenamebuf+i,CP_BLACK,CP_GREEN,1);
				fatfs_lock = 0;
				vTaskDelay(2);
			}
		}

		GUI_MemDevShow(&memhandle,0,0);
		GUICore_SetMemDev(NULL);				LHL_FontShowNumDec(0,220,GUI_ALLOC_GetNumFreeBytes(),RGB(0,255,0),RGB(0,0,0),0,6);
		GUI_MemDevDestroy(&memhandle);
	}
}

void _cb_mButton(void *Handle,MsgType msg)
{
	short i;
	NMBTN_SetInCallBack(Handle,1);
	switch(msg)
	{
		case BTN_MOVEIN:
			break;
		case BTN_MOVEOUT:
			break;
		case BTN_MESSAGESEND:
			if(mpmp3Player[1] != NULL)
			{
				LHL_MP3_DecoderRelease(&mpmp3Player[1]);
			}
			FindMusicWithIndex(nowplayindex++,filenamebuf);
			
			GUI_MemDevDestroy(&litmemhandle);
			
			i = strlen(filenamebuf)-1;
			while(filenamebuf[i]!='/')
			{
				i--;
			}
			i++;
			GUI_MemDevInit(&litmemhandle,8*strlen(filenamebuf+i),16,GUI_MEMDEV_4BIT);
			GUI_MemDevSetColorPalettle(&litmemhandle,colorpalettle);
			GUICore_SetMemDev(&litmemhandle);
			LHL_FontPutStringAt(0,0,filenamebuf+i,CP_YELLOW,CP_BLACK,0);
			GUICore_SetMemDev(NULL);
			
			LHL_MP3_DecoderCreate(&mpmp3Player[1],filenamebuf);
			break;
		case BTN_REDRAW_NOTPRESSED:
//			NMBTN_SetText(Handle,"没按下");
//			NMBTN_ButtonUpdate(Handle,BTNNOTPRESSED);
			break;
		case BTN_REDRAW_PRESSED:
//			NMBTN_SetText(Handle,"已按下");
//			NMBTN_ButtonUpdate(Handle,BTNISPRESSED);
			break;
		default:
			break;
	}
	NMBTN_SetInCallBack(Handle,0);
}
void _cb_TouchMessage(void *Handle,MsgType msg)
{
	NMBTN_SetInCallBack(Handle,1);
	switch(msg)
	{
		case BTN_MOVEIN:
			break;
		case BTN_MOVEOUT:
			break;
		case BTN_MESSAGESEND:
		{
			unsigned char i;
			if(waveIsPressed_delayTime > 18)
			{
				break;
			}
			LHL_TouchGetMoveState(&i);
			if(i != GUI_FAULT)
			{
				break;
			}
			
			if(mpmp3Player[1] != NULL)
			{
				LHL_MP3_DecoderRelease(&mpmp3Player[1]);
			}
			FindMusicWithIndex(index_select,filenamebuf);
			
			GUI_MemDevDestroy(&litmemhandle);
			
			i = strlen(filenamebuf)-1;
			while(filenamebuf[i]!='/')
			{
				i--;
			}
			i++;
			GUI_MemDevInit(&litmemhandle,8*strlen(filenamebuf+i),16,GUI_MEMDEV_4BIT);
			GUI_MemDevSetColorPalettle(&litmemhandle,colorpalettle);
			GUICore_SetMemDev(&litmemhandle);
			LHL_FontPutStringAt(0,0,filenamebuf+i,CP_YELLOW,CP_BLACK,0);
			GUICore_SetMemDev(NULL);
			
			LHL_MP3_DecoderCreate(&mpmp3Player[1],filenamebuf);
		}
			break;
		case BTN_REDRAW_NOTPRESSED:
			break;
		case BTN_REDRAW_PRESSED:
		{
				GUI_POINT mgui_point;
				unsigned char ispress;
				LHL_TouchGetPoint(&mgui_point,&ispress);
				ypressedpos = mgui_point.y;
				waveIsPressed_delayTime = 20;
		}
			break;
		default:
			break;
	}
	NMBTN_SetInCallBack(Handle,0);
}
void _cb_TouchProcess(void *Data,GUI_POINT nowpoint,GUI_POINT add)
{
	waveIsPressed_delayTime = 20;
	ypressedpos = nowpoint.y;
	button_focus_y-=add.y;
	if(button_focus_y<0)
	{
		button_focus_y = 0;
	}
}

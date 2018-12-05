#include  "audioplayer.h"
#include  "iis.h"
#include  "mp3play.h"
#include  "lcd12864.h"
#include  "delay.h"
#include  "znfat.h"
#include  "malloc.h"
#include  "text.h"
#include  "wm8978.h"
#include  "exfuns.h"
#include  "usart.h"
#include  "wavcodec.h"
#include  "string.h"

//音乐播放控制器
__audiodev audiodev;	
uint8_t FileIndexTBL[FileIndexMaxNum];
//uint8_t *FileNameTBL[FileIndexMaxNum];
//开始音频播放
void audio_start(void)
{
	audiodev.status=3<<0;//开始播放+非暂停
	I2S_Play_Start();
} 
//关闭音频播放
void audio_stop(void)
{
	audiodev.status=0;
	I2S_Play_Stop();
}  
//显示曲目索引
//index:当前索引
//total:总文件数
void audio_index_show(u16 index,u16 total)
{
	//显示当前曲目的索引,及总曲目数
	printf("曲目索引/总曲目\r\n");
    printf("%d/%d\r\n",index,total);				  	  
}
//显示播放时间,比特率 信息  
//totsec;音频文件总时间长度
//cursec:当前播放时间
//bitrate:比特率(位速)
void audio_msg_show(u32 totsec,u32 cursec,u32 bitrate)
{
    static u16 playtime=0XFFFF;//播放时间标记	      
	if(playtime!=cursec)					//需要更新显示时间
	{
		playtime=cursec;
		//显示播放时间		
//		Show_Str(0,24,"位率→     Kbps",12);
		//当前时间
        LCD_ShowChar(60,0,0x30+playtime/60,12,1);
        LCD_ShowChar(84,0,0x30+(playtime%60)/10,12,1);
		LCD_ShowChar(96,0,0x30+(playtime%60)%10,12,1);
		//播放时间
		LCD_ShowChar(60,12,0x30+totsec/60,12,1);
        LCD_ShowChar(84,12,0x30+(totsec%60)/10,12,1);
		LCD_ShowChar(96,12,0x30+(totsec%60)%10,12,1);
//        //显示位率
//	    LCD_ShowChar(36,24,0x30+(bitrate/1000)/10000,12,1);
//        LCD_ShowChar(48,24,0x30+((bitrate/1000)/1000)%10,12,1);
//		LCD_ShowChar(60,24,0x30+((bitrate/1000)/100)%10,12,1);
//        LCD_ShowChar(84,24,0x30+((bitrate/1000)%100)/10,12,1);
//		LCD_ShowChar(96,24,0x30+((bitrate/1000)%100)%10,12,1);		
        LCD_Refresh_GRAM();				    	   	    		    	

	} 		 
}
//播放某个音频文件
u8 audio_play_song(u8* fname)
{
    uint8_t res;
    res=f_typetell(fname);
    switch(res)
    {
		case T_WAV:
			 res=wav_play_song(fname);
		break;
		//其他待定
		case T_MP3:
			 res=mp3_play_song(fname);
		break;
		default:
			 printf("不能播放此种格式文件->%s\r\n",fname);
		     res=1;
		break;
	}
    return res;	
}
//播放音乐
void audio_play(u8 filetype)
{
  	static uint8_t srcbuf[200];
    static uint8_t dstbuf[200];
	static uint8_t pname[200];
	uint8_t *WavIndexTBL;
	uint8_t  res=0,i=0;
	uint8_t f_type;
    uint8_t file_index=0;
	FIL FileInfo;
	WM8978_ADDA_Cfg(1,0);//开启DAC
	WM8978_Input_Cfg(0,0,0);//关闭输入通道
	WM8978_Output_Cfg(1,0);//开启DAC输出
	WavIndexTBL=(uint8_t *)mymalloc(SRAMIN,FileIndexMaxNum);
    while(!res)//打开文件成功
	{   
		 res=znFAT_Open_File(&FileInfo,"/MUSIC/*",file_index,1); 	
		 if(!res)
		 {
			 if(FileInfo.have_lfn)//有长名
			 {   
				 hexstring2string(FileInfo,srcbuf);
				 //需要将长名UNI转换为GBK，然后比较
				 unigbk_exchange(srcbuf,dstbuf,0);
				 printf("dstbuf:%s\r\n",dstbuf);
				 f_type=f_typetell(dstbuf);
				 if(f_type==filetype)//当前文件为所查找文件
				 {
					 WavIndexTBL[i++]=file_index;
					 strcpy((char*)pname,"/MUSIC/");//复制路径(目录)
					 strcat((char*)pname,(const char*)dstbuf);//将文件名接在后面
					 printf("带路径的文件名:%s\r\n",pname);
                     LCD_Clear_DGRAM();
					 Show_Str(0,0,pname,12);
					 LCD_Refresh_GRAM();
					 KeyVal=audio_play_song(pname);
                     switch(KeyVal)
					 {
						 case 2://下一曲
                               break;
                         case 3:if(file_index>2)file_index-=2;
						        else file_index=WavIndexTBL[sizeof(WavIndexTBL)];//上一曲
                               break;
                         default:
                               break;							  
					 }
				 }

			 }else//没有长名
			 {
				f_type=f_typetell((uint8_t*)(FileInfo.File_Name));
				if(f_type==filetype)//当前文件为所查找文件
				{
					 WavIndexTBL[i++]=file_index;
					 strcpy((char*)pname,"/MUSIC/");//复制路径(目录)
					 strcat((char*)pname,(const char*)dstbuf);//将文件名接在后面
					 printf("带路径的文件名:%s\r\n",pname);
                     LCD_Clear_DGRAM();
					 Show_Str(0,0,pname,12);
					 LCD_Refresh_GRAM();
				     KeyVal=audio_play_song(pname);
					 switch(KeyVal)
					 {
						 case 2://下一曲
                               break;
                         case 3:if(file_index>2)file_index-=2;
						        else file_index=WavIndexTBL[sizeof(WavIndexTBL)];//上一曲
                               break;
                         default:
                               break;							  
					 }

				}
			 }
			 file_index++;
		 }
		 if(res!=0)
		 {
		    res=0;file_index=0;
		 }
	}
	 
}

//得到path路径下,filetype文件的总个数
//path:路径	 
//返回值:总有效文件数
//注意：慎用struct FileInfo *类型变量，这个问题导致了一堆错误的产生
u8 audio_get_tnum(u8 *path)
{
	uint8_t  res=0,i=0;
    uint8_t file_index=0;
	FIL FileInfo;
    while(!res)//打开文件成功
	{   
		 if(i>255)
		 {
		   i=255;break;
		 }
		 res=znFAT_Open_File(&FileInfo,(char*)path,file_index++,1);
		 if(!res)
		 {
		    
           i++;	
		 }
		 
	}
	return i;
}

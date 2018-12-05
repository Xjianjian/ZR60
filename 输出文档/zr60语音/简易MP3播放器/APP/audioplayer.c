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

//���ֲ��ſ�����
__audiodev audiodev;	
uint8_t FileIndexTBL[FileIndexMaxNum];
//uint8_t *FileNameTBL[FileIndexMaxNum];
//��ʼ��Ƶ����
void audio_start(void)
{
	audiodev.status=3<<0;//��ʼ����+����ͣ
	I2S_Play_Start();
} 
//�ر���Ƶ����
void audio_stop(void)
{
	audiodev.status=0;
	I2S_Play_Stop();
}  
//��ʾ��Ŀ����
//index:��ǰ����
//total:���ļ���
void audio_index_show(u16 index,u16 total)
{
	//��ʾ��ǰ��Ŀ������,������Ŀ��
	printf("��Ŀ����/����Ŀ\r\n");
    printf("%d/%d\r\n",index,total);				  	  
}
//��ʾ����ʱ��,������ ��Ϣ  
//totsec;��Ƶ�ļ���ʱ�䳤��
//cursec:��ǰ����ʱ��
//bitrate:������(λ��)
void audio_msg_show(u32 totsec,u32 cursec,u32 bitrate)
{
    static u16 playtime=0XFFFF;//����ʱ����	      
	if(playtime!=cursec)					//��Ҫ������ʾʱ��
	{
		playtime=cursec;
		//��ʾ����ʱ��		
//		Show_Str(0,24,"λ�ʡ�     Kbps",12);
		//��ǰʱ��
        LCD_ShowChar(60,0,0x30+playtime/60,12,1);
        LCD_ShowChar(84,0,0x30+(playtime%60)/10,12,1);
		LCD_ShowChar(96,0,0x30+(playtime%60)%10,12,1);
		//����ʱ��
		LCD_ShowChar(60,12,0x30+totsec/60,12,1);
        LCD_ShowChar(84,12,0x30+(totsec%60)/10,12,1);
		LCD_ShowChar(96,12,0x30+(totsec%60)%10,12,1);
//        //��ʾλ��
//	    LCD_ShowChar(36,24,0x30+(bitrate/1000)/10000,12,1);
//        LCD_ShowChar(48,24,0x30+((bitrate/1000)/1000)%10,12,1);
//		LCD_ShowChar(60,24,0x30+((bitrate/1000)/100)%10,12,1);
//        LCD_ShowChar(84,24,0x30+((bitrate/1000)%100)/10,12,1);
//		LCD_ShowChar(96,24,0x30+((bitrate/1000)%100)%10,12,1);		
        LCD_Refresh_GRAM();				    	   	    		    	

	} 		 
}
//����ĳ����Ƶ�ļ�
u8 audio_play_song(u8* fname)
{
    uint8_t res;
    res=f_typetell(fname);
    switch(res)
    {
		case T_WAV:
			 res=wav_play_song(fname);
		break;
		//��������
		case T_MP3:
			 res=mp3_play_song(fname);
		break;
		default:
			 printf("���ܲ��Ŵ��ָ�ʽ�ļ�->%s\r\n",fname);
		     res=1;
		break;
	}
    return res;	
}
//��������
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
	WM8978_ADDA_Cfg(1,0);//����DAC
	WM8978_Input_Cfg(0,0,0);//�ر�����ͨ��
	WM8978_Output_Cfg(1,0);//����DAC���
	WavIndexTBL=(uint8_t *)mymalloc(SRAMIN,FileIndexMaxNum);
    while(!res)//���ļ��ɹ�
	{   
		 res=znFAT_Open_File(&FileInfo,"/MUSIC/*",file_index,1); 	
		 if(!res)
		 {
			 if(FileInfo.have_lfn)//�г���
			 {   
				 hexstring2string(FileInfo,srcbuf);
				 //��Ҫ������UNIת��ΪGBK��Ȼ��Ƚ�
				 unigbk_exchange(srcbuf,dstbuf,0);
				 printf("dstbuf:%s\r\n",dstbuf);
				 f_type=f_typetell(dstbuf);
				 if(f_type==filetype)//��ǰ�ļ�Ϊ�������ļ�
				 {
					 WavIndexTBL[i++]=file_index;
					 strcpy((char*)pname,"/MUSIC/");//����·��(Ŀ¼)
					 strcat((char*)pname,(const char*)dstbuf);//���ļ������ں���
					 printf("��·�����ļ���:%s\r\n",pname);
                     LCD_Clear_DGRAM();
					 Show_Str(0,0,pname,12);
					 LCD_Refresh_GRAM();
					 KeyVal=audio_play_song(pname);
                     switch(KeyVal)
					 {
						 case 2://��һ��
                               break;
                         case 3:if(file_index>2)file_index-=2;
						        else file_index=WavIndexTBL[sizeof(WavIndexTBL)];//��һ��
                               break;
                         default:
                               break;							  
					 }
				 }

			 }else//û�г���
			 {
				f_type=f_typetell((uint8_t*)(FileInfo.File_Name));
				if(f_type==filetype)//��ǰ�ļ�Ϊ�������ļ�
				{
					 WavIndexTBL[i++]=file_index;
					 strcpy((char*)pname,"/MUSIC/");//����·��(Ŀ¼)
					 strcat((char*)pname,(const char*)dstbuf);//���ļ������ں���
					 printf("��·�����ļ���:%s\r\n",pname);
                     LCD_Clear_DGRAM();
					 Show_Str(0,0,pname,12);
					 LCD_Refresh_GRAM();
				     KeyVal=audio_play_song(pname);
					 switch(KeyVal)
					 {
						 case 2://��һ��
                               break;
                         case 3:if(file_index>2)file_index-=2;
						        else file_index=WavIndexTBL[sizeof(WavIndexTBL)];//��һ��
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

//�õ�path·����,filetype�ļ����ܸ���
//path:·��	 
//����ֵ:����Ч�ļ���
//ע�⣺����struct FileInfo *���ͱ�����������⵼����һ�Ѵ���Ĳ���
u8 audio_get_tnum(u8 *path)
{
	uint8_t  res=0,i=0;
    uint8_t file_index=0;
	FIL FileInfo;
    while(!res)//���ļ��ɹ�
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

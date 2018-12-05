#include "mp3play.h"
#include "audioplayer.h"
#include "sys.h"
#include "dma.h"
#include "delay.h"
#include "malloc.h"
#include "usart.h"
//#include "ff.h"
#include "znfat.h"
#include "string.h"
#include "iis.h"
#include "wm8978.h"
//#include "key.h" 
#include "led.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//��������ֲ��helix MP3�����
//ALIENTEK STM32F407������
//MP3 �������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/29
//�汾��V1.0
//********************************************************************************
//V1.0 ˵��
//1,֧��16λ������/������MP3�Ľ���
//2,֧��CBR/VBR��ʽMP3����
//3,֧��ID3V1��ID3V2��ǩ����
//4,֧�����б�����(MP3�����320Kbps)����
////////////////////////////////////////////////////////////////////////////////// 	
 
__mp3ctrl  *mp3ctrl;	//mp3���ƽṹ�� 
vu8 mp3transferend=0;	//i2s������ɱ�־
vu8 mp3witchbuf=0;		//i2sbufxָʾ��־




//����ID3V1 
//buf:�������ݻ�����(��С�̶���128�ֽ�)
//pctrl:MP3������
//����ֵ:0,��ȡ����
//    ����,��ȡʧ��
u8 mp3_id3v1_decode(u8* buf,__mp3ctrl *pctrl)
{
	ID3V1_Tag *id3v1tag;
	id3v1tag=(ID3V1_Tag*)buf;
	if (strncmp("TAG",(char*)id3v1tag->id,3)==0)//��MP3 ID3V1 TAG
	{
		if(id3v1tag->title[0])strncpy((char*)pctrl->title,(char*)id3v1tag->title,30);
		if(id3v1tag->artist[0])strncpy((char*)pctrl->artist,(char*)id3v1tag->artist,30); 
	    //�˴����������������Ϣ
		
	}else return 1;
	return 0;
}
//����ID3V2 
//buf:�������ݻ�����
//size:���ݴ�С
//pctrl:MP3������
//����ֵ:0,��ȡ����
//    ����,��ȡʧ��
u8 mp3_id3v2_decode(u8* buf,u32 size,__mp3ctrl *pctrl)
{
	ID3V2_TagHead *taghead;
	ID3V23_FrameHead *framehead; 
	u32 t;
	u32 tagsize;	//tag��С
	u32 frame_size;	//֡��С 
	taghead=(ID3V2_TagHead*)buf; 
	if(strncmp("ID3",(const char*)taghead->id,3)==0)//����ID3?
	{
		tagsize=((u32)taghead->size[0]<<21)|((u32)taghead->size[1]<<14)|((u16)taghead->size[2]<<7)|taghead->size[3];//�õ�tag ��С
		pctrl->datastart=tagsize;		//�õ�mp3���ݿ�ʼ��ƫ����
		if(tagsize>size)tagsize=size;	//tagsize��������bufsize��ʱ��,ֻ��������size��С������
		if(taghead->mversion<3)
		{
			printf("not supported mversion!\r\n");
			return 1;
		}
		t=10;
		while(t<tagsize)
		{
			framehead=(ID3V23_FrameHead*)(buf+t);
			frame_size=((u32)framehead->size[0]<<24)|((u32)framehead->size[1]<<16)|((u32)framehead->size[2]<<8)|framehead->size[3];//�õ�֡��С
 			if (strncmp("TT2",(char*)framehead->id,3)==0||strncmp("TIT2",(char*)framehead->id,4)==0)//�ҵ���������֡,��֧��unicode��ʽ!!
			{
				strncpy((char*)pctrl->title,(char*)(buf+t+sizeof(ID3V23_FrameHead)+1),AUDIO_MIN(frame_size-1,MP3_TITSIZE_MAX-1));
			}
 			if (strncmp("TP1",(char*)framehead->id,3)==0||strncmp("TPE1",(char*)framehead->id,4)==0)//�ҵ�����������֡
			{
				strncpy((char*)pctrl->artist,(char*)(buf+t+sizeof(ID3V23_FrameHead)+1),AUDIO_MIN(frame_size-1,MP3_ARTSIZE_MAX-1));
			}
			t+=frame_size+sizeof(ID3V23_FrameHead);
		} 
	}else pctrl->datastart=0;//������ID3,mp3�����Ǵ�0��ʼ
	return 0;
} 

//��ȡMP3������Ϣ
//pname:MP3�ļ�·��
//pctrl:MP3������Ϣ�ṹ�� 
//����ֵ:0,�ɹ�
//    ����,ʧ��
u8 mp3_get_info(u8 *pname,__mp3ctrl* pctrl)
{
    HMP3Decoder decoder;
    MP3FrameInfo frame_info;
	MP3_FrameXing* fxing;
	MP3_FrameVBRI* fvbri;
	FIL *fmp3;
	u8  *buf;
//	u32 br;
	u8 res;
	int offset=0;
	u32 p;
	short samples_per_frame;	//һ֡�Ĳ�������
	u32 totframes;				//��֡��
	
	fmp3=mymalloc(SRAMIN,sizeof(FIL)); 
	buf=mymalloc(SRAMIN,5*1024);		//����5K�ڴ� 
	if(buf)//�ڴ�����ɹ�
	{ 		
		res=znFAT_Open_File(fmp3,(char*)pname,0,1);
		res=znFAT_ReadData(fmp3,fmp3->File_CurOffset,5*1024,buf);
		if(res==0)//��ȡ�ļ��ɹ�,��ʼ����ID3V2/ID3V1�Լ���ȡMP3��Ϣ
		{  
			mp3_id3v2_decode(buf,fmp3->File_CurOffset,pctrl);	//����ID3V2����	
			znFAT_ReadData(fmp3,fmp3->File_Size-128,128,buf);//��ȡ����128���ֽ�
			mp3_id3v1_decode(buf,pctrl);	//����ID3V1����  
			decoder=MP3InitDecoder(); 		//MP3���������ڴ�
			
         	//ƫ�Ƶ����ݿ�ʼ�ĵط�
            //��ȡ5K�ֽ�mp3����
			znFAT_ReadData(fmp3,pctrl->datastart,5*1024,buf);
 			offset=MP3FindSyncWord(buf,fmp3->File_CurOffset);	//����֡ͬ����Ϣ
			if(offset>=0&&MP3GetNextFrameInfo(decoder,&frame_info,&buf[offset])==0)//�ҵ�֡ͬ����Ϣ��,����һ����Ϣ��ȡ����	
			{ 
				p=offset+4+32;
				fvbri=(MP3_FrameVBRI*)(buf+p);
				if(strncmp("VBRI",(char*)fvbri->id,4)==0)//����VBRI֡(VBR��ʽ)
				{
					if (frame_info.version==MPEG1)samples_per_frame=1152;//MPEG1,layer3ÿ֡����������1152
					else samples_per_frame=576;//MPEG2/MPEG2.5,layer3ÿ֡����������576 
 					totframes=((u32)fvbri->frames[0]<<24)|((u32)fvbri->frames[1]<<16)|((u16)fvbri->frames[2]<<8)|fvbri->frames[3];//�õ���֡��
					pctrl->totsec=totframes*samples_per_frame/frame_info.samprate;//�õ��ļ��ܳ���
				}else	//����VBRI֡,�����ǲ���Xing֡(VBR��ʽ)
				{  
					if (frame_info.version==MPEG1)	//MPEG1 
					{
						p=frame_info.nChans==2?32:17;
						samples_per_frame = 1152;	//MPEG1,layer3ÿ֡����������1152
					}else
					{
						p=frame_info.nChans==2?17:9;
						samples_per_frame=576;		//MPEG2/MPEG2.5,layer3ÿ֡����������576
					}
					p+=offset+4;
					fxing=(MP3_FrameXing*)(buf+p);
					if(strncmp("Xing",(char*)fxing->id,4)==0||strncmp("Info",(char*)fxing->id,4)==0)//��Xng֡
					{
						if(fxing->flags[3]&0X01)//������frame�ֶ�
						{
							totframes=((u32)fxing->frames[0]<<24)|((u32)fxing->frames[1]<<16)|((u16)fxing->frames[2]<<8)|fxing->frames[3];//�õ���֡��
							pctrl->totsec=totframes*samples_per_frame/frame_info.samprate;//�õ��ļ��ܳ���
						}else	//��������frames�ֶ�
						{
							pctrl->totsec=fmp3->File_Size/(frame_info.bitrate/8);
						} 
					}else 		//CBR��ʽ,ֱ�Ӽ����ܲ���ʱ��
					{
						pctrl->totsec=fmp3->File_Size/(frame_info.bitrate/8);
					}
				} 
				pctrl->bitrate=frame_info.bitrate;			//�õ���ǰ֡������
				mp3ctrl->samplerate=frame_info.samprate; 	//�õ�������. 
				if(frame_info.nChans==2)mp3ctrl->outsamples=frame_info.outputSamps; //���PCM��������С 
				else mp3ctrl->outsamples=frame_info.outputSamps*2; //���PCM��������С,���ڵ�����MP3,ֱ��*2,����Ϊ˫�������
			}else res=0XFE;//δ�ҵ�ͬ��֡	
			MP3FreeDecoder(decoder);//�ͷ��ڴ�		
		} 
		znFAT_Close_File(fmp3);
	}else res=0XFF;
	myfree(SRAMIN,&fmp3);
	myfree(SRAMIN,buf);	
	return res;	
}  
//�õ���ǰ����ʱ��
//fx:�ļ�ָ��
//mp3x:mp3���ſ�����
void mp3_get_curtime(FIL*fx,__mp3ctrl *mp3x)
{
	u32 fpos=0;  	 
	if(fx->File_CurOffset>mp3x->datastart)fpos=fx->File_CurOffset-mp3x->datastart;	//�õ���ǰ�ļ����ŵ��ĵط� 
	mp3x->cursec=fpos*mp3x->totsec/(fx->File_Size-mp3x->datastart);	//��ǰ���ŵ��ڶ�������?	
}
//mp3�ļ�������˺���
//pos:��Ҫ��λ�����ļ�λ��
//����ֵ:��ǰ�ļ�λ��(����λ��Ľ��)
u32 mp3_file_seek(u32 pos)
{
	if(pos>audiodev.file->File_Size)
	{
		pos=audiodev.file->File_Size;
	}
	znFAT_Seek(audiodev.file,pos);
	return audiodev.file->File_CurOffset;
}

//����һ��MP3����
//fname:MP3�ļ�·��.
//����ֵ:0,�����������
//[b7]:0,����״̬;1,����״̬
//[b6:0]:b7=0ʱ,��ʾ������ 
//       b7=1ʱ,��ʾ�д���(���ﲻ�ж��������,0X80~0XFF,�����Ǵ���)
u8 mp3_play_song(u8* fname)
{ 
	HMP3Decoder mp3decoder;
    MP3FrameInfo mp3frameinfo;
    //����buf��С
    uint8_t *buffer;
	u8 res;
	u8 t=0; 
	u8* readptr;	//MP3�����ָ��
	int offset=0;	//ƫ����
	int outofdata=0;//�������ݷ�Χ
	int bytesleft=0;//buffer��ʣ�����Ч����
	u32 br=0; 
	int err=0;  
    mp3ctrl=mymalloc(SRAMIN,sizeof(__mp3ctrl)); 	
	audiodev.file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	audiodev.tbuf=mymalloc(SRAMIN,2304*2);
	buffer=(uint8_t*)mymalloc(SRAMIN,MP3_FILE_BUF_SZ);
	audiodev.i2sbuf1=mymalloc(SRAMIN,2304*2);
	audiodev.i2sbuf2=mymalloc(SRAMIN,2304*2);
	audiodev.file_seek=mp3_file_seek;
	//�ж��ڴ������Ƿ�ɹ�
	if(!mp3ctrl||!audiodev.file||!audiodev.tbuf||!buffer||!audiodev.i2sbuf1||!audiodev.i2sbuf2)//�ڴ�����ʧ��
	{
		printf("�ڴ��������\r\n"); 
		myfree(SRAMIN,audiodev.file);
		myfree(SRAMIN,audiodev.tbuf);		
		myfree(SRAMIN,buffer);
		myfree(SRAMIN,mp3ctrl);
		myfree(SRAMIN,audiodev.i2sbuf1);
		myfree(SRAMIN,audiodev.i2sbuf2);
		return AP_ERR;	//����
	}else
	{
	    printf("�ڴ�����ɹ�\r\n");
	}
	memset(audiodev.i2sbuf1,0,2304*2);	//�������� 
	memset(audiodev.i2sbuf2,0,2304*2);	//�������� 
	memset(mp3ctrl,0,sizeof(__mp3ctrl));//�������� 
    res=mp3_get_info(fname,mp3ctrl);  
	if(res==0)
	{ 
		printf("     title:%s\r\n",mp3ctrl->title); 
		printf("    artist:%s\r\n",mp3ctrl->artist); 
		printf("   bitrate:%dbps\r\n",mp3ctrl->bitrate);	
		printf("samplerate:%d\r\n", mp3ctrl->samplerate);	
		printf("  totalsec:%d\r\n",mp3ctrl->totsec); 
		WM8978_I2S_Cfg(2,0);	//�����ֱ�׼,16λ���ݳ���
	    I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16bextended);	//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ��չ֡����
	    I2S2_SamplingRate_Set(mp3ctrl->samplerate);		//���ò����� 
	    DMA1_Stream4_Channel0_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,mp3ctrl->outsamples);//����TX DMA
	    I2S_TX_CallBack=mp3_i2s_dma_tx_callback;		//�ص�����ָ��mp3_i2s_dma_tx_callback
	}else 
	{
	    printf("����MP3�ļ���Ϣ����\r\n");
		printf("�������%d\r\n",res);	
		myfree(SRAMIN,audiodev.file);
		myfree(SRAMIN,audiodev.tbuf);		
		myfree(SRAMIN,buffer);
		myfree(SRAMIN,mp3ctrl);
		myfree(SRAMIN,audiodev.i2sbuf1);
		myfree(SRAMIN,audiodev.i2sbuf2);
		return AP_ERR;//����
	}
		res=znFAT_Open_File(audiodev.file,(char*)fname,0,1);
	    audio_stop();//�ر���Ƶ���
	    mp3decoder=MP3InitDecoder();//MP3���������ڴ�
	   if((!res)&&(mp3decoder!=0))//MP3���������ڴ�ɹ�����ʼ����
	   { 
		
		audio_start();								//��ʼ���� 
		printf("****************************************");
		printf("\r\n��ʼ����\r\n");
		//����MP3����ѭ��
		while(res==0)
		{
			readptr=buffer;	//MP3��ָ��ָ��buffer
			offset=0;		//ƫ����Ϊ0
//			outofdata=0;	//��������
			bytesleft=0;	
            //�����ݿ�ʼ��λ��һ�ζ�ȡMP3_FILE_BUF_SZ�ֽ�
			br=znFAT_ReadData(audiodev.file,audiodev.file->File_CurOffset,MP3_FILE_BUF_SZ,buffer);
			//����������Ϊ0�����������
			if(br==0)		
			{
				res=AP_OK;	//�������
				printf("�����������");
				printf("**************************************");
				break;
			}
            if(outofdata>10)
           {
		      outofdata=0;
			  myfree(SRAMIN,audiodev.file);
		      myfree(SRAMIN,audiodev.tbuf);		
		      myfree(SRAMIN,buffer);
		      myfree(SRAMIN,mp3ctrl);
		      myfree(SRAMIN,audiodev.i2sbuf1);
		      myfree(SRAMIN,audiodev.i2sbuf2);
		      break;
           }//֡���������ֱ࣬���л���һ��			
			bytesleft+=br;	//buffer�����ж�����ЧMP3����?
			err=0;	
            //����֡����ѭ��			
			while(1)//û�г��������쳣(���ɷ��ҵ�֡ͬ���ַ�)
			{
				
				offset=MP3FindSyncWord(readptr,bytesleft);//��readptrλ��,��ʼ����ͬ���ַ�
				if(offset<0)	//û���ҵ�ͬ���ַ�,����֡����ѭ��
				{ 
//					outofdata=1;//û�ҵ�֡ͬ���ַ�
					break;//��Ҫ����ѭ��������
				}else
				{
						readptr+=offset;		//MP3��ָ��ƫ�Ƶ�ͬ���ַ���.
						bytesleft-=offset;		//buffer�������Ч���ݸ���,�����ȥƫ����
						err=MP3Decode(mp3decoder,&readptr,&bytesleft,(short*)audiodev.tbuf,0);//����һ֡MP3����
						if(err!=0)
						{
							printf("decode error:%d\r\n",err);
							outofdata++;
							break;
						}
						MP3GetLastFrameInfo(mp3decoder,&mp3frameinfo);	//�õ��ոս����MP3֡��Ϣ
						if(mp3ctrl->bitrate!=mp3frameinfo.bitrate)		//��������
						{
							mp3ctrl->bitrate=mp3frameinfo.bitrate; 
						}
							mp3_fill_buffer((u16*)audiodev.tbuf,mp3frameinfo.outputSamps,mp3frameinfo.nChans);//���pcm����
			
						if(bytesleft<MAINBUF_SIZE*2)//����������С��2��MAINBUF_SIZE��ʱ��,���벹���µ����ݽ���.
						{ 
							memmove(buffer,readptr,bytesleft);//�ƶ�readptr��ָ������ݵ�buffer����,��������СΪ:bytesleft
							//�������µ�����
							znFAT_ReadData(audiodev.file,audiodev.file->File_CurOffset,MP3_FILE_BUF_SZ-bytesleft,buffer+bytesleft);
							if(br<MP3_FILE_BUF_SZ-bytesleft)
							{
								memset(buffer+bytesleft+br,0,MP3_FILE_BUF_SZ-bytesleft-br); 
							}
							bytesleft=MP3_FILE_BUF_SZ;  
							readptr=buffer; 
						}			
                            delay_ms(5);						
							mp3_get_curtime(audiodev.file,mp3ctrl); 
							audiodev.totsec=mp3ctrl->totsec;	//��������
							audiodev.cursec=mp3ctrl->cursec; 
						    if(audiodev.totsec==audiodev.cursec)break;
						t++;
						if(t==20)
						{
						  t=0;
						  LED0=!LED0;
						  printf("Totsec:%d\r\n",audiodev.totsec);
						  printf("Cursec:%d\r\n",audiodev.cursec);
						  
						}
				}	
                				
			}  
		}
		audio_stop();//�ر���Ƶ���
	}else 
	{
	    printf("�ļ���ʧ��\r\n");
		res=AP_ERR;//����
	}
	znFAT_Close_File(audiodev.file);
	MP3FreeDecoder(mp3decoder);		//�ͷ��ڴ�	
	myfree(SRAMIN,audiodev.file);
	myfree(SRAMIN,audiodev.tbuf);		
	myfree(SRAMIN,buffer);
	myfree(SRAMIN,mp3ctrl);
	myfree(SRAMIN,audiodev.i2sbuf1);
	myfree(SRAMIN,audiodev.i2sbuf2);
	return res;
}
//MP3 DMA���ͻص�����
void mp3_i2s_dma_tx_callback(void) 
{    
	u16 i;
	if(DMA1_Stream4->CR&(1<<19))
	{
		mp3witchbuf=0;
		if((audiodev.status&0X01)==0)//��ͣ��,���0
		{
			for(i=0;i<2304*2;i++)audiodev.i2sbuf1[i]=0;
		}
	}else 
	{
		mp3witchbuf=1;
		if((audiodev.status&0X01)==0)//��ͣ��,���0
		{
			for(i=0;i<2304*2;i++)audiodev.i2sbuf2[i]=0;
		}
	} 
	mp3transferend=1;
} 
//���PCM���ݵ�DAC
//buf:PCM�����׵�ַ
//size:pcm������(16λΪ��λ)
//nch:������(1,������,2������)
void mp3_fill_buffer(u16* buf,u16 size,u8 nch)
{
	u16 i; 
	u16 *p;
	while(mp3transferend==0)//�ȴ��������
	{
//		delay_ms(1000/OS_TICKS_PER_SEC);
		delay_ms(1000/200u);
	};
	mp3transferend=0;
	if(mp3witchbuf==0)
	{
		p=(u16*)audiodev.i2sbuf1;
	}else 
	{
		p=(u16*)audiodev.i2sbuf2;
	}
	if(nch==2)for(i=0;i<size;i++)p[i]=buf[i];
	else	//������
	{
		for(i=0;i<size;i++)
		{
			p[2*i]=buf[i];
			p[2*i+1]=buf[i];
		}
	}
} 


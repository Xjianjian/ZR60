#include  "wavcodec.h"
#include  "audioplayer.h"
#include  "malloc.h"
#include  "keyboard.h"
#include  "iis.h"
#include  "led.h"
#include  "dma.h"
#include  "delay.h"
#include  "wm8978.h"
#include  "znfat.h"
#include  "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//WAV �������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved				
//********************************************************************************
//V1.0 ˵��
//1,֧��16λ/24λWAV�ļ�����
//2,��߿���֧�ֵ�192K/24bit��WAV��ʽ. 
////////////////////////////////////////////////////////////////////////////////// 	
 
__wavctrl wavctrl;		//WAV���ƽṹ��
vu8 wavtransferend=0;	//i2s������ɱ�־
vu8 wavwitchbuf=0;		//i2sbufxָʾ��־

//WAV������ʼ��
//fname:�ļ�·��+�ļ���
//wavx:wav ��Ϣ��Žṹ��ָ��
//����ֵ:0,�ɹ�;1,���ļ�ʧ��;2,��WAV�ļ�;3,DATA����δ�ҵ�.
u8 wav_decode_init(u8* fname,__wavctrl* wavx)
{
     FIL *ftemp;
	 uint8_t *buf,res;
	 ChunkRIFF *riff;
	 ChunkFMT *fmt;
	 ChunkFACT *fact;
	 ChunkDATA *data;
	 ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	 buf=mymalloc(SRAMIN,512);
     if(ftemp&&buf)//�ж��ڴ��Ƿ�����ɹ�
	 {
	     res=znFAT_Open_File(ftemp,(char *)fname,0,1);
		 if(!res)//���ļ��ɹ�
		 {
			 znFAT_ReadData(ftemp,ftemp->File_CurOffset,512,buf);
			 riff=(ChunkRIFF *)buf;//�õ�RIFF��
			 if(Bytes2Value(riff->Format,4)==0X45564157)//�ж��Ƿ���WAV�ļ�
			 {
			    fmt=(ChunkFMT *)(buf+12);	//��ȡFMT�� 
				fact=(ChunkFACT *)(buf+12+8+fmt->ChunkSize);//��ȡFACT��
				if(Bytes2Value(fact->ChunkID,4)==0X74636166||Bytes2Value(fact->ChunkID,4)==0X5453494C)wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;//����fact/LIST���ʱ��(δ����)
				else wavx->datastart=12+8+fmt->ChunkSize;   //DATA Chunk��ʼ��λ��
				data=(ChunkDATA *)(buf+wavx->datastart);	//��ȡDATA��
				if(Bytes2Value(data->ChunkID,4)==0X61746164)//�����ɹ�!
				{
					wavx->audioformat=fmt->AudioFormat;		//��Ƶ��ʽ
					wavx->nchannels=fmt->NumOfChannels;		//ͨ����
					wavx->samplerate=fmt->SampleRate;		//������
					wavx->bitrate=fmt->ByteRate*8;			//�õ�λ��
					wavx->blockalign=fmt->BlockAlign;		//�����
					wavx->bps=fmt->BitsPerSample;			//λ��,16/24/32λ
					
					wavx->datasize=data->ChunkSize;			//���ݿ��С
					wavx->datastart=wavx->datastart+8;		//������������ʼ�ĵط�. 
					                                        //��������
					
				}else res=3;//data����δ�ҵ�.
			 }else res=2;//��wav�ļ�
			 
		 }else res=1;//���ļ�����
	 }
	 znFAT_Close_File(ftemp);
	 myfree(SRAMIN,ftemp);
	 myfree(SRAMIN,buf);
	 return res;
}
//���buf
//buf:������
//size:���������
//bits:λ��(16/24)
//����ֵ:���������ݸ���
u32 wav_buffill(u8 *buf,u16 size,u8 bits)
{
    uint16_t readlen=0;
	uint32_t havereadlen=0;//ʵ�ʶ������ֽ���
	u8 *p_tbuf;
	u16 i=0;
	if(bits==24)
	{
	   readlen=(size/4)*3;//�����4���ֽڣ���ײ��3���ֽ�
	   havereadlen=znFAT_ReadData(audiodev.file,audiodev.file->File_CurOffset,readlen,audiodev.tbuf);
	   p_tbuf=audiodev.tbuf;
		//��SD���ж�ȡ����������С�˴洢
	   for(i=0;i<size;)
	   {
		    buf[i++]=p_tbuf[1];
			buf[i]=p_tbuf[2];
			i+=2;
			buf[i++]=p_tbuf[0];
			p_tbuf+=3;
		}
	   havereadlen=(havereadlen*4)/3;	
	}else
	{
		//16bit��Ƶ,ֱ�Ӷ�ȡ���� �����4���ֽڣ���ֱ�Ӷ�4���ֽ�
	   	havereadlen=znFAT_ReadData(audiodev.file,audiodev.file->File_CurOffset,size,buf);
	    if(havereadlen<size)
		{
		   for(i=havereadlen;i<size-havereadlen;i++)
		    buf[i]=0;
		}
	}
	return havereadlen;
}
//WAV����ʱ,I2S DMA����ص�����
void wav_i2s_dma_tx_callback(void) 
{   
	u16 i;                      //�жϵ�ǰĿ��洢��Ϊ�洢��1���Ǵ洢��0
	if(DMA1_Stream4->CR&(1<<19))//��ǰĿ��洢��Ϊ�洢��1
	{
		wavwitchbuf=0;          //���洢��0
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				audiodev.i2sbuf1[i]=0;//���0
			}
		}
	}else                       //��ǰĿ��洢��Ϊ�洢��0
	{
		wavwitchbuf=1;          //���洢��1
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				audiodev.i2sbuf2[i]=0;//���0
			}
		}
	}
	wavtransferend=1;//I2S������ɱ�־
}
//�õ���ǰ����ʱ��
//fx:�ļ�ָ��
//wavx:wav���ſ�����
void wav_get_curtime(FIL*fx,__wavctrl *wavx)
{
	long long fpos;
    wavx->totsec=wavx->datasize/(wavx->bitrate/8);//�����ܳ��ȣ���λ:S
	fpos=fx->File_CurOffset-wavx->datastart;//�õ���ǰ�ļ����ŵ��ĵط�
	wavx->cursec=fpos*wavx->totsec/wavx->datasize;//��ǰ���ŵ��ڶ�����
}
//����ĳ��WAV�ļ�
//fname:wav�ļ�·��.
//����ֵ:
//KEY0_PRES:��һ��
//KEY1_PRES:��һ��
//����:����
u8 wav_play_song(u8* fname)
{
   u8 res =0;
   u8 key=0;
   u8 t=0;
   u32  fillnum=0;
   audiodev.file=(FIL *)mymalloc(SRAMIN,sizeof(FIL));
   audiodev.i2sbuf1=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
   audiodev.i2sbuf2=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
   audiodev.tbuf=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
   //�ж��ڴ������Ƿ�ɹ�
   if(audiodev.file&&audiodev.i2sbuf1&&audiodev.i2sbuf2&&audiodev.tbuf)
   {
      res=wav_decode_init(fname,&wavctrl);
	  if(!res)//�����ļ��ɹ�
	  {
	    if(wavctrl.bps==16)
		{
		   WM8978_I2S_Cfg(2,0);
		   I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16bextended);
			
		}else if(wavctrl.bps==24)
        {
		    WM8978_I2S_Cfg(2,2);
            I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_24b);			
		 }
        I2S2_SamplingRate_Set(wavctrl.samplerate);
        DMA1_Stream4_Channel0_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE/2);
        I2S_TX_CallBack=wav_i2s_dma_tx_callback;
        audio_stop();
        res=znFAT_Open_File(audiodev.file,(char*)fname,0,1);
		if(!res)
		{
			 //��λ�����ݿ�ʼλ��
		     znFAT_Seek(audiodev.file,wavctrl.datastart);
			 fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
			 fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
			 audio_start();
			 while(res==0)
			 {
				 //�ȴ�wactransferend��־��Ϊ1
				 //��wav�������
			     while(wavtransferend==0);
				 wavtransferend=0;
				 //�ж��Ƿ񲥷Ž���
				 //�Ƚ���亯���ķ���ֵ��ʵ�εõ�����
				 if(fillnum!=WAV_I2S_TX_DMA_BUFSIZE)
				 {
				     res=1;
					 break;
				 }
				 if(wavwitchbuf)fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
			     else fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
				 while(1)
				 {
//				    key=KEYBoard_LRScan();
					if(KeyVal==6)//��ͣ
					{
					   if(audiodev.status&0x01)audiodev.status&=~(1<<0);
					   else audiodev.status|=0x01;
					   
					}
					if(KeyVal==2||KeyVal==3)//�л���Ŀ
					{
					   res=KeyVal;
					   break;
					}
					//�õ���ʱ��͵�ǰ����ʱ��
					wav_get_curtime(audiodev.file,&wavctrl);
//					audio_msg_show(wavctrl.totsec,wavctrl.cursec,wavctrl.bitrate);
					t++;
					if(t==20)
					{
					  t=0;
					  LED0=!LED0;
					}
					if((audiodev.status&0x01)==0)delay_ms(10);
					else break;
				 }
			 
			 }audio_stop();
			 
		}else res=0xFF;
      		 
	  }else res=0xFF; 
   }else res=0xFF;
   myfree(SRAMIN,audiodev.tbuf);
   myfree(SRAMIN,audiodev.i2sbuf1);
   myfree(SRAMIN,audiodev.i2sbuf2);
   myfree(SRAMIN,audiodev.file);   
   return res;	
}


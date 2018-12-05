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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//WAV 解码代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved				
//********************************************************************************
//V1.0 说明
//1,支持16位/24位WAV文件播放
//2,最高可以支持到192K/24bit的WAV格式. 
////////////////////////////////////////////////////////////////////////////////// 	
 
__wavctrl wavctrl;		//WAV控制结构体
vu8 wavtransferend=0;	//i2s传输完成标志
vu8 wavwitchbuf=0;		//i2sbufx指示标志

//WAV解析初始化
//fname:文件路径+文件名
//wavx:wav 信息存放结构体指针
//返回值:0,成功;1,打开文件失败;2,非WAV文件;3,DATA区域未找到.
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
     if(ftemp&&buf)//判断内存是否申请成功
	 {
	     res=znFAT_Open_File(ftemp,(char *)fname,0,1);
		 if(!res)//打开文件成功
		 {
			 znFAT_ReadData(ftemp,ftemp->File_CurOffset,512,buf);
			 riff=(ChunkRIFF *)buf;//得到RIFF块
			 if(Bytes2Value(riff->Format,4)==0X45564157)//判断是否是WAV文件
			 {
			    fmt=(ChunkFMT *)(buf+12);	//获取FMT块 
				fact=(ChunkFACT *)(buf+12+8+fmt->ChunkSize);//读取FACT块
				if(Bytes2Value(fact->ChunkID,4)==0X74636166||Bytes2Value(fact->ChunkID,4)==0X5453494C)wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;//具有fact/LIST块的时候(未测试)
				else wavx->datastart=12+8+fmt->ChunkSize;   //DATA Chunk开始的位置
				data=(ChunkDATA *)(buf+wavx->datastart);	//读取DATA块
				if(Bytes2Value(data->ChunkID,4)==0X61746164)//解析成功!
				{
					wavx->audioformat=fmt->AudioFormat;		//音频格式
					wavx->nchannels=fmt->NumOfChannels;		//通道数
					wavx->samplerate=fmt->SampleRate;		//采样率
					wavx->bitrate=fmt->ByteRate*8;			//得到位速
					wavx->blockalign=fmt->BlockAlign;		//块对齐
					wavx->bps=fmt->BitsPerSample;			//位数,16/24/32位
					
					wavx->datasize=data->ChunkSize;			//数据块大小
					wavx->datastart=wavx->datastart+8;		//数据流真正开始的地方. 
					                                        //纯数据流
					
				}else res=3;//data区域未找到.
			 }else res=2;//非wav文件
			 
		 }else res=1;//打开文件错误
	 }
	 znFAT_Close_File(ftemp);
	 myfree(SRAMIN,ftemp);
	 myfree(SRAMIN,buf);
	 return res;
}
//填充buf
//buf:数据区
//size:填充数据量
//bits:位数(16/24)
//返回值:读到的数据个数
u32 wav_buffill(u8 *buf,u16 size,u8 bits)
{
    uint16_t readlen=0;
	uint32_t havereadlen=0;//实际读到的字节数
	u8 *p_tbuf;
	u16 i=0;
	if(bits==24)
	{
	   readlen=(size/4)*3;//欲填充4个字节，则底层读3个字节
	   havereadlen=znFAT_ReadData(audiodev.file,audiodev.file->File_CurOffset,readlen,audiodev.tbuf);
	   p_tbuf=audiodev.tbuf;
		//从SD卡中读取到的数据是小端存储
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
		//16bit音频,直接读取数据 欲填充4个字节，则直接读4个字节
	   	havereadlen=znFAT_ReadData(audiodev.file,audiodev.file->File_CurOffset,size,buf);
	    if(havereadlen<size)
		{
		   for(i=havereadlen;i<size-havereadlen;i++)
		    buf[i]=0;
		}
	}
	return havereadlen;
}
//WAV播放时,I2S DMA传输回调函数
void wav_i2s_dma_tx_callback(void) 
{   
	u16 i;                      //判断当前目标存储器为存储器1还是存储器0
	if(DMA1_Stream4->CR&(1<<19))//当前目标存储器为存储器1
	{
		wavwitchbuf=0;          //填充存储器0
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				audiodev.i2sbuf1[i]=0;//填充0
			}
		}
	}else                       //当前目标存储器为存储器0
	{
		wavwitchbuf=1;          //填充存储器1
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				audiodev.i2sbuf2[i]=0;//填充0
			}
		}
	}
	wavtransferend=1;//I2S传输完成标志
}
//得到当前播放时间
//fx:文件指针
//wavx:wav播放控制器
void wav_get_curtime(FIL*fx,__wavctrl *wavx)
{
	long long fpos;
    wavx->totsec=wavx->datasize/(wavx->bitrate/8);//歌曲总长度，单位:S
	fpos=fx->File_CurOffset-wavx->datastart;//得到当前文件播放到的地方
	wavx->cursec=fpos*wavx->totsec/wavx->datasize;//当前播放到第多少秒
}
//播放某个WAV文件
//fname:wav文件路径.
//返回值:
//KEY0_PRES:下一曲
//KEY1_PRES:上一曲
//其他:错误
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
   //判断内存申请是否成功
   if(audiodev.file&&audiodev.i2sbuf1&&audiodev.i2sbuf2&&audiodev.tbuf)
   {
      res=wav_decode_init(fname,&wavctrl);
	  if(!res)//解析文件成功
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
			 //定位到数据开始位置
		     znFAT_Seek(audiodev.file,wavctrl.datastart);
			 fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
			 fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
			 audio_start();
			 while(res==0)
			 {
				 //等待wactransferend标志变为1
				 //即wav传输完成
			     while(wavtransferend==0);
				 wavtransferend=0;
				 //判断是否播放结束
				 //比较填充函数的返回值与实参得到结论
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
					if(KeyVal==6)//暂停
					{
					   if(audiodev.status&0x01)audiodev.status&=~(1<<0);
					   else audiodev.status|=0x01;
					   
					}
					if(KeyVal==2||KeyVal==3)//切换曲目
					{
					   res=KeyVal;
					   break;
					}
					//得到总时间和当前播放时间
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


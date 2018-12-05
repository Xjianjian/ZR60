#include   "text.h"
#include   "fontupd.h"
#include   "lcd12864.h"
#include   "usart.h"
#include   "znfat.h" 
//2015年4月17日修改

//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,GBK码
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
//size:字体大小
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{   
  	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
	{   		    
	    for(i=0;i<csize;i++)*mat++=0x00;//填充满格
	    return; //结束访问
	}          
	if(ql<0x7f)ql-=0x40;//注意!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*csize;	//得到字库中的字节偏移量  		  
//	printf("foffset:%ld",foffset);
	switch(size)
	{	
		case 12:znFAT_ReadData(&fileinfo_f12,foffset,csize,mat);
             break;		
        case 16:znFAT_ReadData(&fileinfo_f16,foffset,csize,mat);
             break;	
        case 24:znFAT_ReadData(&fileinfo_f24,foffset,csize,mat);
              break;					
	}     									   
    
}

//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//size:字体大小   
void Show_Font(unsigned char x,unsigned char y,u8 *font,u8 size)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[72];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	if(size!=12&&size!=16&&size!=24)return;	//不支持的size
	Get_HzMat(font,dzk,size);	//得到相应大小的点阵数据 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//得到点阵数据   		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
			LCD_DrawPoint(x,y,1);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  
//	LCD_Refresh_GRAM();
}
//在指定位置开始显示一个字符串	    
//(x,y):起始坐标
//str  :字符串
//size :字体大小   	   		   
void Show_Str(unsigned char x,unsigned char y,u8*str,u8 size)
{					
	uint8_t i=0;
	uint8_t isHZ=0;
	if(x>127||y>63)return ;
    while(str[i]!='\0')//判断数据是否结束
	{
		if(!isHZ)
		{
		    if(str[i]>0x80)isHZ=1;//中文
            else                  //字符
		    {
			  if(x>127-11)
			  {
				   x=0;
				  if(size!=24)
				   y+=16;
				  else				  
				   y+=24;
			  }
			  if(size!=24)
			  {
				  if(y>63-11)
				  {	  
				    y=0;
				    LCD_Clear_DGRAM();
				  }
			  }else
			  {
				  if(y>63-23)
				  {
				    y=0;
				    LCD_Clear_DGRAM();
				  }
			  }
			  LCD_ShowChar(x,y,*(str+i),size,1);
			  x+=size;
			  i++;
			}			
			
		}else//中文
       { 
		      isHZ=0;
			  if(x>127-11)
			  {
				   x=0;
				  if(size!=24)
				   y+=16;
				  else				  
				   y+=24;
			  }
			  if(size!=24)
			  {
				  if(y>63-11)
				  {
				    y=0;
				    LCD_Clear_DGRAM();
				  }
			  }else
			  {
				  if(y>63-23)
				  {
					y=0;
				    LCD_Clear_DGRAM();
				  }
			  }
			  Show_Font(x,y,str+i,size);
			  x+=size;
			  i+=2;   
	   }
	}
	
}  
//src:要转换的字符
//dir:0:uni2oem 1:oem2uni
//oem此处即指gbk
//2分法查表，前提是数据必须有序，要么从高到低；要么从低到高。
//所以在gbkuni文件中存在2个表即2个数组
unsigned short unigbk(unsigned short src,unsigned char dir)
{
    unsigned short res;
	uint32_t high,low,temp;
	uint16_t n;
	uint16_t t[2];
	uint32_t gbk2uni_offset=0;//相对文件gbkuni开始的偏移量
    if(src<0x80)res=src; //ASCII码直接不用转换
    else
	{
	     if(dir)//dir=1表示oem转uni
		 {
		    gbk2uni_offset=fileinfo_gbkuni.File_Size/2;//oem转uni表放在gbkuni文件后半截 
		 }else//dir=0表示uni转oem
		 {
		    gbk2uni_offset=0;  
		 }
		 high=fileinfo_gbkuni.File_Size/2;
		 high=high/4-1;//此处说明如下 因为uni和gbk都是2字节表示
		               //所以需要用数组的总长度/2得到一共多少项
		               //再除以2得到项数一半
		 low=0;
		 for(n=16;n;n--)
		 {
		     temp=low+(high-low)/2;
			 znFAT_ReadData(&fileinfo_gbkuni,gbk2uni_offset+temp*4,4,(unsigned char*)t);
		     if(src==t[0])break;
			 if(src>t[0])low=temp;
			 else        high=temp;
		 }
		 res=n?t[1]:0;
		 
	}	
     return res;	
}
//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 text_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//其它返回字符'0'
//返回值:字符
u8 text_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}
void  hexstring2string(struct FileInfo FileInfo,u8* dstbuf)
{
	uint8_t i=0;uint16_t j=0;
	while(FileInfo.longname[i]!=0)
	{
      dstbuf[j]=text_hex2chr((FileInfo.longname[i]>>12)&0x0F);
	  dstbuf[j+1]=text_hex2chr((FileInfo.longname[i]>>8)&0x0F);
	  dstbuf[j+2]=text_hex2chr((FileInfo.longname[i]>>4)&0x0F);
	  dstbuf[j+3]=text_hex2chr((FileInfo.longname[i]>>0)&0x0F);
	  i++;j+=4;
	  }	
	dstbuf[j]=0;
}
//src:输入字符串
//dst:输出字符串(unigbk时为gbk内码，gbk2uni时为uni字符串)
//mode:0,uni2gbk
//     1,gbk2uni
void  unigbk_exchange(u8* src,u8* dst,u8 mode)
{
   u16 temp;
   u8  buf[2];
   if(mode)//gbk2uni
   {
       while(*src!='\0')
	   {
	      if(*src<0x81)//并不是汉字，因为汉字GBK码最小为0x8140
	      {
		      temp=(u16)unigbk((unsigned short)*src,1);
			  src++;
		   }else//为汉字 占用2个字节
		  {
		     buf[1]=*src++;
			 buf[0]=*src++;
			 //将某个汉字转换为了uni码
			 temp=(u16)unigbk((unsigned short)*(u16*)buf,1);
		  }
		  *dst++=text_hex2chr((temp>>12)&0x0F);
		  *dst++=text_hex2chr((temp>>8)&0x0F);
	      *dst++=text_hex2chr((temp>>4)&0x0F);
		  *dst++=text_hex2chr((temp)&0x0F);
	   }
   }
   else//uni2gbk
   {
      while(*src!='\0')
	  {
	      buf[1]=text_chr2hex(*src++)*16;
		  buf[1]+=text_chr2hex(*src++);
		  buf[0]=text_chr2hex(*src++)*16;
		  buf[0]+=text_chr2hex(*src++);
		  temp=(u16)unigbk((unsigned short)*(u16*)buf,0);
	      if(temp<0x80)
          {
		     *dst=temp;dst++;
		   }else//需要交换一下高低字节，得到正确汉字
		  {
		    *(u16*)dst=swap16(temp);
			dst+=2;
		  }
	  
	  }
   }

  *dst=0;
}
	















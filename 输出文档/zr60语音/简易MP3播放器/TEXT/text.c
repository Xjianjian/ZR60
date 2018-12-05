#include   "text.h"
#include   "fontupd.h"
#include   "lcd12864.h"
#include   "usart.h"
#include   "znfat.h" 
//2015��4��17���޸�

//code �ַ�ָ�뿪ʼ
//���ֿ��в��ҳ���ģ
//code �ַ����Ŀ�ʼ��ַ,GBK��
//mat  ���ݴ�ŵ�ַ (size/8+((size%8)?1:0))*(size) bytes��С	
//size:�����С
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{   
  	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�� ���ú���
	{   		    
	    for(i=0;i<csize;i++)*mat++=0x00;//�������
	    return; //��������
	}          
	if(ql<0x7f)ql-=0x40;//ע��!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*csize;	//�õ��ֿ��е��ֽ�ƫ����  		  
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

//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//size:�����С   
void Show_Font(unsigned char x,unsigned char y,u8 *font,u8 size)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[72];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	if(size!=12&&size!=16&&size!=24)return;	//��֧�ֵ�size
	Get_HzMat(font,dzk,size);	//�õ���Ӧ��С�ĵ������� 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//�õ���������   		
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
//��ָ��λ�ÿ�ʼ��ʾһ���ַ���	    
//(x,y):��ʼ����
//str  :�ַ���
//size :�����С   	   		   
void Show_Str(unsigned char x,unsigned char y,u8*str,u8 size)
{					
	uint8_t i=0;
	uint8_t isHZ=0;
	if(x>127||y>63)return ;
    while(str[i]!='\0')//�ж������Ƿ����
	{
		if(!isHZ)
		{
		    if(str[i]>0x80)isHZ=1;//����
            else                  //�ַ�
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
			
		}else//����
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
//src:Ҫת�����ַ�
//dir:0:uni2oem 1:oem2uni
//oem�˴���ָgbk
//2�ַ����ǰ�������ݱ�������Ҫô�Ӹߵ��ͣ�Ҫô�ӵ͵��ߡ�
//������gbkuni�ļ��д���2����2������
unsigned short unigbk(unsigned short src,unsigned char dir)
{
    unsigned short res;
	uint32_t high,low,temp;
	uint16_t n;
	uint16_t t[2];
	uint32_t gbk2uni_offset=0;//����ļ�gbkuni��ʼ��ƫ����
    if(src<0x80)res=src; //ASCII��ֱ�Ӳ���ת��
    else
	{
	     if(dir)//dir=1��ʾoemתuni
		 {
		    gbk2uni_offset=fileinfo_gbkuni.File_Size/2;//oemתuni�����gbkuni�ļ����� 
		 }else//dir=0��ʾuniתoem
		 {
		    gbk2uni_offset=0;  
		 }
		 high=fileinfo_gbkuni.File_Size/2;
		 high=high/4-1;//�˴�˵������ ��Ϊuni��gbk����2�ֽڱ�ʾ
		               //������Ҫ��������ܳ���/2�õ�һ��������
		               //�ٳ���2�õ�����һ��
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
//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 text_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//��1��16��������ת��Ϊ�ַ�
//hex:16��������,0~15;
//���������ַ�'0'
//����ֵ:�ַ�
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
//src:�����ַ���
//dst:����ַ���(unigbkʱΪgbk���룬gbk2uniʱΪuni�ַ���)
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
	      if(*src<0x81)//�����Ǻ��֣���Ϊ����GBK����СΪ0x8140
	      {
		      temp=(u16)unigbk((unsigned short)*src,1);
			  src++;
		   }else//Ϊ���� ռ��2���ֽ�
		  {
		     buf[1]=*src++;
			 buf[0]=*src++;
			 //��ĳ������ת��Ϊ��uni��
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
		   }else//��Ҫ����һ�¸ߵ��ֽڣ��õ���ȷ����
		  {
		    *(u16*)dst=swap16(temp);
			dst+=2;
		  }
	  
	  }
   }

  *dst=0;
}
	















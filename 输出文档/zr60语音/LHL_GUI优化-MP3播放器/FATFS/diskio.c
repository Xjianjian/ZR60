/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include "sdio_sd.h"		/* Example: MMC/SDC contorl */

/* Definitions of physical drive number for each media */
//#define ATA		0
//#define MMC		1
//#define USB		2

extern SD_CardInfo SDCardInfo;
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
		SD_Error Status;
	
	switch (pdrv) 
 {
	case 0 :	  
    Status = SD_Init();
    if(Status == SD_OK)
	  return 0;
    else 
	  return STA_NOINIT;
	
	case 1 :	  
		return STA_NOINIT;
		  
	case 2 :
		return STA_NOINIT;
  }
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
    switch (pdrv) 
	{
	  case 0 :		
	  /* translate the reslut code here	*/
	    return 0;
	  case 1 :
	  /* translate the reslut code here	*/
	    return 0;
	  case 2 :
	  /* translate the reslut code here	*/
	    return 0;
	  default:
        break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
SD_Error Status;
  if( !count )
  {    
    return RES_PARERR;  /* count不能等于0，否则返回参数错误 */
  }
  switch (pdrv)
  {
    case 0:
    if(count==1)            /* 1个sector的读操作 */      
    {       
	  Status =  SD_ReadBlock( buff ,sector<< 9 , SDCardInfo.CardBlockSize );
    }                                                
    else                    /* 多个sector的读操作 */     
    {   
      Status = SD_ReadMultiBlocks( buff ,sector<< 9 ,SDCardInfo.CardBlockSize,count);
    }                                                
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
    if(Status == SD_OK)
    {
      return RES_OK;
    }
    else
    {
      return RES_ERROR;
    }   
	case 1:	
	  break;

    case 2:	
	  break;

    default:
      break;
  } 
  return RES_ERROR;
	//return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
SD_Error Status;
  if( !count )
  {    
    return RES_PARERR;  /* count不能等于0，否则返回参数错误 */
  }
  switch (pdrv)
  {
    case 0:
    if(count==1)            /* 1个sector的写操作 */      
    {   
       Status = SD_WriteBlock( (uint8_t *)(&buff[0]) ,sector << 9 , SDCardInfo.CardBlockSize); 
    }                                                
    else                    /* 多个sector的写操作 */    
    {   
       Status = SD_WriteMultiBlocks( (uint8_t *)(&buff[0]) ,sector << 9 ,SDCardInfo.CardBlockSize,count);	  
    } 
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);	                                               
    if(Status == SD_OK)
    {
       return RES_OK;
    }
    else
    {
       return RES_ERROR;
    }
    case 2:
	   break;
    default :
       break;
  }
 return RES_ERROR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  if (pdrv)
    {    
        return RES_PARERR;  /* 仅支持单磁盘操作，否则返回参数错误 */
    }
	switch (cmd) 
	{
	  case CTRL_SYNC :	      
		return RES_OK;
	  case GET_SECTOR_COUNT : 
	    *(DWORD*)buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
	    return RES_OK;
	  case GET_BLOCK_SIZE :
	    *(WORD*)buff = SDCardInfo.CardBlockSize;
	    return RES_OK;	
	  case CTRL_POWER :
		break;
	  case CTRL_LOCK :
		break;
	  case CTRL_EJECT :
		break;
      /* MMC/SDC command */
	  case MMC_GET_TYPE :
		break;
	  case MMC_GET_CSD :
		break;
	  case MMC_GET_CID :
		break;
	  case MMC_GET_OCR :
		break;
	  case MMC_GET_SDSTAT :
		break;	
	}
	return RES_PARERR;   
}
#endif

DWORD get_fattime (void)
{
   
    return 0;
}

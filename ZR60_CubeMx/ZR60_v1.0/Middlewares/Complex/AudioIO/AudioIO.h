#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <inttypes.h>
#include "Include.h"

/* RECBUFFER_SIZE����������������С��ʵ��ÿ��������ռ��RAM�ռ��ֽ���Ϊ��RECBUFFER_SIZE*2
 * ����ѡ��I2S_AudioFreq_16k������ʹ��RECBUFFER_SIZEΪ1024*4���Ϳɱ�֤¼��������Ч��������
 * ���ѡ���������߲�������Ҫ�ʵ�������RECBUFFER_SIZE��С�����統ѡ����I2S_AudioFreq_44k
 * ������RECBUFFER_SIZEΪ1024*8
 * ���⣬SD������Ķ�д�ٶ�Ҳ��һ��������Ӱ��¼��Ч��
 */
#define RECBUFFER_SIZE  	1024

/*STM32���ݴ���IIS��DMA����*/
#define AUDIO_PLAY_START    I2S_Play_Start();//����DMA TX
#define AUDIO_PLAY_STOP    	I2S_Play_Stop();//�ر�DMA TX

/* ��ƵоƬ�ӿ����� */
#define AUDIO_PLAY_POWEROFF    wm8978_PowerDown();//�ر�power 
//����оƬ��������,����Ϊ���������,�����ո�ʽ,16bit����
#define AUDIO_DRIVERCHIP_PLAYCONFIG		wm8978_CfgAudioPath(0x10,0x04);\
										wm8978_CfgAudioIF((uint16_t)0x0000,16);\
										wm8978_SetOUT2Volume(50);

/* ��������������Ƶ�ļ����� */
typedef enum
{
	AudioIO_WelBack = 0,	/* ��ӭ�ؼ�  */
	AudioIO_FireAlarm,		/* ��������  */
	AudioIO_CardInvalid,	/* ��Ƭ��Ч  */
	AudioIO_DoorOpen,		/* �����ѿ�  */
	AudioIO_ConfCancel,		/* �����豸ȡ�� */
	AudioIO_keyVoice,		/* ������   */
	AudioIO_ConfSuccess,	/*  �����豸�ɹ�  */
	AudioIO_ConfDevice,		/*  ˢ�������豸  */
	AudioIO_BonVoyage,		/* һ·˳�� */
	AudioIO_ErrKey,			/*  �������  */
	AudioIO_SoftUpdata,		/*  ���ڽ����������  */
	AudioIO_SoftUpdataFinish,		/*  ��������ɹ�  */
	AudioIO_SoftUpdataFail,		/*  �������ʧ��  */
	AudioIO_DeInitFailLostId,		/*  �豸��ʼ��ʧ�ܣ�δ����id  */
	AudioIO_DeInitFailExistId,		/*  �豸��ʼ��ʧ�ܣ��ѱ��󶨵���id  */
	AudioIO_DeInitFinish,		/*  �豸��ʼ���ɹ�  */
	AudioIO_DeInitFail,		/*  �豸��ʼ��ʧ��  */
	AudioIO_Num,
	AudioIO_Empty = 0xff	/* ���ļ� */
}Audio_PlayFile;

typedef union
{
	uint16_t  wBrdcast[RECBUFFER_SIZE];			/* �������ݻ���������ŷ��͸�WM8978����Ƶ����*/
	uint8_t   cBrdcast[RECBUFFER_SIZE*2];			/* �������ݻ���������ŷ��͸�WM8978����Ƶ����*/
}Recorder_Brdcast;	


typedef struct
{
	uint8_t ucInput;			/* ����Դ��0:MIC, 1:������ */
	uint8_t ucFmtIdx;			/* ��Ƶ��ʽ����׼��λ��������Ƶ�� */
	uint8_t ucVolume;			/* ��ǰ�������� */
	uint8_t ucGain;			  /* ��ǰ���� */	
	uint8_t ucStatus;			/* ¼����״̬��0��ʾ������1��ʾ¼���У�2��ʾ������ */	
}REC_TYPE;	

/* WAV�ļ�ͷ��ʽ */
typedef __packed struct
{ 
	uint32_t	riff;							/* = "RIFF"	0x46464952*/
	uint32_t	size_8;						/* ���¸���ַ��ʼ���ļ�β�����ֽ���	*/
	uint32_t	wave;							/* = "WAVE" 0x45564157*/
	
	uint32_t	fmt;							/* = "fmt " 0x20746d66*/
	uint32_t	fmtSize;					/* ��һ���ṹ��Ĵ�С(һ��Ϊ16) */
	uint16_t	wFormatTag;				/* ���뷽ʽ,һ��Ϊ1	*/
	uint16_t	wChannels;				/* ͨ������������Ϊ1��������Ϊ2 */
	uint32_t	dwSamplesPerSec;	/* ������ */
	uint32_t	dwAvgBytesPerSec;	/* ÿ���ֽ���(= ������ �� ÿ���������ֽ���) */
	uint16_t	wBlockAlign;			/* ÿ���������ֽ���(=����������/8*ͨ����) */
	uint16_t	wBitsPerSample;		/* ����������(ÿ��������Ҫ��bit��) */
																			  
	uint32_t	data;							/* = "data" 0x61746164*/
	uint32_t	datasize;					/* �����ݳ��� */
} WavHead;	

typedef struct
{
	uint32_t read_addr;			/* ��Ƶ������ʼ��ַ */
	uint32_t  BytesSize;			/* ��Ƶ�����ֽڳ��� */
}wavFile_Struct;


/* API�ӿ� */
extern void InitAudioIO_playconfig(void);
extern void TskAudioIO_MainFunction(void);
extern void SetAudioIO_PlayFile(uint8_t Le_u_File,uint8_t Le_u_PlayPriority);
extern uint8_t GetAudioIO_u_PlaySt(void);
#endif  /* __RECORD_H__   */


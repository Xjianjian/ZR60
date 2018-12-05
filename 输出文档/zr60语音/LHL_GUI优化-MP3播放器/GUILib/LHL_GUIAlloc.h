#ifndef __LHL_GUIALLOC_H_
#define	__LHL_GUIALLOC_H_

#define I8    signed char
#define U8  unsigned char     /* unsigned 8  bits. */
#define I16   signed short    /*   signed 16 bits. */
#define U16 unsigned short    /* unsigned 16 bits. */
#define I32   signed long   /*   signed 32 bits. */
#define U32 unsigned long   /* unsigned 32 bits. */
#define I16P I16              /*   signed 16 bits OR MORE ! */
#define U16P U16              /* unsigned 16 bits OR MORE ! */

/* Types used for memory allocation */
#if GUI_ALLOC_SIZE <32767
  #define GUI_ALLOC_DATATYPE   I16
  #define GUI_ALLOC_DATATYPE_U U16
#else
  #define GUI_ALLOC_DATATYPE   I32
  #define GUI_ALLOC_DATATYPE_U U32
#endif

#ifndef     GUI_HMEM
  #if GUI_MAXBLOCKS < 32768
    #define     GUI_HMEM        I16P
  #else
    #define     GUI_HMEM        I32
  #endif
#endif

void GUI_ALLOC_Init(void);

GUI_ALLOC_DATATYPE GUI_ALLOC_GetMaxSize(void);

GUI_HMEM GUI_ALLOC_AllocNoInit(GUI_ALLOC_DATATYPE Size);

void GUI_ALLOC_Free(GUI_HMEM hMem);

void GUI_ALLOC_FreePtr(GUI_HMEM *ph);

void GUI_ALLOC_FreePtrArray(GUI_HMEM * pArray, int NumElems);

GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumFreeBlocks(void);

GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumUsedBlocks(void);

GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumFreeBytes(void);

GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumUsedBytes(void);

GUI_ALLOC_DATATYPE GUI_ALLOC_GetSize(GUI_HMEM  hMem);

void* GUI_ALLOC_h2p(GUI_HMEM  hMem);

void GUI_ALLOC_Lock(void);

void GUI_ALLOC_Unlock(void);

#endif

#ifndef HM_DMA_H_
#define HM_DMA_H_

typedef volatile USIGN32 IO_REG32;

// DMA API
typedef enum {DMA_Chan_1, DMA_Chan_2, DMA_Chan_3, DMA_Chan_4, DMA_Chan_5, DMA_Chan_6, DMA_Chan_7,   // channels in DMA1
              DMA_Chan_8, DMA_Chan_9, DMA_Chan_10, DMA_Chan_11, DMA_Chan_12} DMA_Chan;              // channels in DMA2

FUNCTION PUBLIC VOID     DMA_ConfigUART(const volatile void *pBuffer, USIGN32 BufferSize, USIGN32 devadr, DMA_Chan chn);
FUNCTION PUBLIC VOID     DMA_SetUART(DMA_Chan chntx, DMA_Chan chnrx);
FUNCTION PUBLIC USIGN32  DMA_getcount(DMA_Chan chan);
FUNCTION PUBLIC VOID     DMA_Clear_int(DMA_Chan chn);


#endif // HM_DMA_H_

// end of source

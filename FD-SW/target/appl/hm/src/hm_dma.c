/**
Copyright 2011 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file stm_dma.c

    \brief Driver for the STM32F10xx DMA hardware - used by I2C and
    u(s)arts.

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/


  #include "keywords.h"   /* keyword defines */
  #define  MODULE_ID      (COMP_HM + MOD_HM)

INCLUDES

  #include "base.h"
  #include "hm_dma.h"


//lint ++flb

#define DMA1x                    HARDWARE(dma_t *, DMA1_BASE)
#define DMA2x                    HARDWARE(dma_t *, DMA2_BASE)

typedef struct
{
    IO_REG32(CCR);
    IO_REG32(CNDTR);
    IO_REG32(CPAR);
    IO_REG32(CMAR);
    u32     pad;
} DMA_Channel_TypeDefEP;

typedef struct
{
    IO_REG32(ISR);
    IO_REG32(IFCR);
} DMA_TypeDefEP;

typedef struct dma_t
{
    DMA_TypeDefEP base;
    DMA_Channel_TypeDefEP chan[];
} dma_t;

//lint ++flb
#if 0
STATIC const DMA_Channel_TypeDefEP *ch_addr[] =
{
    [DMA_Chan_1]  = &DMA1x->chan[0],
    [DMA_Chan_2]  = &DMA1x->chan[1],
    [DMA_Chan_3]  = &DMA1x->chan[2],
    [DMA_Chan_4]  = &DMA1x->chan[3],
    [DMA_Chan_5]  = &DMA1x->chan[4],
    [DMA_Chan_6]  = &DMA1x->chan[5],
    [DMA_Chan_7]  = &DMA1x->chan[6],
    [DMA_Chan_8]  = &DMA2x->chan[0],
    [DMA_Chan_9]  = &DMA2x->chan[1],
    [DMA_Chan_10] = &DMA2x->chan[2],
    [DMA_Chan_11] = &DMA2x->chan[3],
    [DMA_Chan_12] = &DMA2x->chan[4]
};
#endif

/*Bits 27, 23, 19, 15,
11, 7, 3*/
#define TEIF    (1u << 3)  /* Channel x transfer error flag (x = 1 ..7)
This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the
DMA_IFCR register.
0: No transfer error (TE) on channel x
1: A transfer error (TE) occurred on channel x
Bits 26, 22, 18, 14,
10, 6, 2*/
#define HTIF    (1u << 2)   /*x: Channel x half transfer flag (x = 1 ..7)
This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the
DMA_IFCR register.
0: No half transfer (HT) event on channel x
1: A half transfer (HT) event occurred on channel x
Bits 25, 21, 17, 13,
9, 5, 1*/
#define TCIF   (1u << 1)    /*x: Channel x transfer complete flag (x = 1 ..7)
This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the
DMA_IFCR register.
0: No transfer complete (TC) event on channel x
1: A transfer complete (TC) event occurred on channel x
Bits 24, 20, 16, 12,
8, 4, 0 */
#define GIF     (1u << 0)  /*  : Channel x global interrupt flag (x = 1 ..7)
This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the
DMA_IFCR register.
0: No TE, HT or TC event on channel x
1: A TE, HT or TC event occurred on channel x */

//lint --flb




//Bits 31:15 Reserved, must be kept at reset value.
//Bit 14 MEM2MEM: Memory to memory mode
//This bit is set and cleared by software.
#define MEM2MEM (1u << 14)
//0: Memory to memory mode disabled
//1: Memory to memory mode enabled
//Bits 13:12 PL[1:0]: Channel priority level
//These bits are set and cleared by software.
#define PR_LOW      (0u << 12)
#define PR_MED      (1u << 12)
#define PR_HIGH     (2u << 12)
#define PR_VHIGH    (3u << 12)
//00: Low
//01: Medium
//10: High
//11: Very high
//Bits 11:10 MSIZE[1:0]: Memory size
//These bits are set and cleared by software.
#define MS_8    0u
#define MS_16   1u
#define MS_32   2u
#define M_SFT   10
//00: 8-bits
//01: 16-bits
//10: 32-bits
//11: Reserved
//Bits 9:8 PSIZE[1:0]: Peripheral size
//These bits are set and cleared by software.
#define P_SFT   8
//00: 8-bits
//01: 16-bits
//10: 32-bits
//11: Reserved
//Bit 7 MINC: Memory increment mode
//This bit is set and cleared by software.
#define MINC (1u << 7)
//0: Memory increment mode disabled
//1: Memory increment mode enabled
//Bit 6 PINC: Peripheral increment mode
//This bit is set and cleared by software.
#define PINC (1u << 6)
//0: Peripheral increment mode disabled
//1: Peripheral increment mode enabled
//Bit 5 CIRC: Circular mode
//This bit is set and cleared by software.
#define CIRC (1u << 5)
//0: Circular mode disabled
//1: Circular mode enabled
//where x = channel number)
//Address offset: 0x0C + 0d20 × (channel number – 1)
//Reset value: 0x0000 0000
//Bit 4 DIR: Data transfer direction
//This bit is set and cleared by software.
#define DIR (1u << 4)
//0: Read from peripheral
//1: Read from memory
//Bit 3 TEIE: Transfer error interrupt enable
//This bit is set and cleared by software.
#define TEIE (1u << 3)
//0: TE interrupt disabled
//1: TE interrupt enabled
//Bit 2 HTIE: Half transfer interrupt enable
//This bit is set and cleared by software.
#define HTIE (1u << 2)
//0: HT interrupt disabled
//1: HT interrupt enabled
//Bit 1 TCIE: Transfer complete interrupt enable
//This bit is set and cleared by software.
#define TCIE (1u << 1)
//0: TC interrupt disabled
//1: TC interrupt enabled
//Bit 0 EN: Channel enable
//This bit is set and cleared by software.
#define EN (1u << 0)
//0: Channel disabled
//1: Channel enabled

//lint --flb

#define DMA_UART_ATTR (MINC | PR_MED | (MS_8 << M_SFT) | (MS_8 << P_SFT))

#if 0
FUNCTION PUBLIC USIGN32 DMA_getcount
  (
    IN  DMA_Chan chan
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is called to fetch the current count from the specified DMA channel
  is initialized,


PARAMETERS:

  chan          channel number of the channel

RETURN_VALUES:

  None

------------------------------------------------------------------------------*/
/** \brief Get the remaining count in the circular DMA FIFO used for UART RX

    \param chan - enumerator for the channel
    \return - the index of the next character to be stored in the FIFO
*/
//u32 DMA_getcount(DMA_Chan chan)
{
LOCAL_VARIABLES

FUNCTION_BODY

    DMA_Channel_TypeDefEP *ch = &DMA1x->chan[chan];

    return ch->CNDTR;
}
#endif

FUNCTION PUBLIC VOID DMA_SetUART
  (
    IN DMA_Chan chntx, 
    IN DMA_Chan chnrx
  )

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is called to set up the transfer attributes prior to a DMA tarnsfer. A cheenl pair 
  is initialized,


PARAMETERS:

  chntx         channle number of the transmit channel
  chnrx         channle number of the receive channel

RETURN_VALUES:

  None

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

    DMA_Channel_TypeDefEP *ch;

FUNCTION_BODY

    // UART TX DMA Channel configuration
    ch = &DMA1x->chan[chntx];
    ch->CCR = DMA_UART_ATTR | TCIE | DIR;               // set direction for outbound

    // UART RX DMA Channel configuration
    ch      = &DMA1x->chan[chnrx];
    ch->CCR = DMA_UART_ATTR | TCIE;                     // set circular for inbound
}

FUNCTION PUBLIC VOID DMA_ConfigUART
  (
     IN const volatile void *pBuffer, 
     IN             USIGN32 BufferSize, 
     IN             USIGN32 devadr, 
     IN            DMA_Chan chn
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is called to set up the DMA controller for packet send or receive via USART.
  AFter configuration It enables the DMA. 


PARAMETERS:

  pBuffer       Pointer to the transfer buffer
  BufferSize    The length of the DMA operation
  devadr        the pripharal address of the Data Register in the USART
  chn           The DMA channel number - see hm_dma.h

RETURN_VALUES:

  None

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

    DMA_Channel_TypeDefEP *ch = &DMA1x->chan[chn];

FUNCTION_BODY

    /* Initialize the DMA with the new parameters */
    ch->CCR  &= ~EN;                        // disable the channel

    DMA_Clear_int(chn);

    ch->CMAR  = HARDWARE(u32, pBuffer);     // set the RAM address
    ch->CNDTR = (u32)BufferSize;            // set the transfer length
    ch->CPAR  = devadr;                     // set the UART data reg address
    ch->CCR  |= EN;                         // go
}

FUNCTION PUBLIC VOID DMA_Clear_int
  (
    IN DMA_Chan chn
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is called to clear any pending interrupts on a specific DMA channel
  AFter configuration It enables the DMA. 


PARAMETERS:

  chn           The DMA channel number - see hm_dma.h

RETURN_VALUES:

  None

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    if (chn >= DMA_Chan_8)
    {
        u32 x = (chn - DMA_Chan_8) * 4;
        DMA2x->base.IFCR = (0xfu << x);          // clear flags in DMA 2
    }
    else
    {
        u32 x = (chn) * 4;
        DMA1x->base.IFCR = (0xfu << x);          // clear flag in DMA 1
    }
}

// end of source

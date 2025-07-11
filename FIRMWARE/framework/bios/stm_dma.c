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
#include "mnwrap.h"
#include "stmcommon.h"

//lint ++flb
typedef enum {DISABLE, ENABLE} FunctionalState;

#define DMA1                    HARDWARE(DMA_TypeDef *, DMA1_BASE)
#define DMA2                    HARDWARE(DMA_TypeDef *, DMA2_BASE)
#define DMA2_BASE               (AHBPERIPH_BASE + 0x0400u)
#define DMA1_BASE               (AHBPERIPH_BASE + 0x0000u)

#define DMA1x                    HARDWARE(dma_t *, DMA1_BASE)
#define DMA2x                    HARDWARE(dma_t *, DMA2_BASE)

typedef struct
{
    IO_REG32(CCR);
    IO_REG32(CNDTR);
    IO_REG32(CPAR);
    IO_REG32(CMAR);
    u32     pad;
} DMA_Channel_TypeDef;

typedef struct
{
    IO_REG32(ISR);
    IO_REG32(IFCR);
} DMA_TypeDef;

typedef struct dma_t
{
    DMA_TypeDef base;
    DMA_Channel_TypeDef chan[];
} dma_t;

//lint ++flb
DMA_Channel_TypeDef *ch_addr[] =
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

/** \brief Get the remaining count in the circular DMA FIFO used for UART RX

    \param chan - enumerator for the channel
    \return - the index of the next character to be stored in the FIFO
*/
u32 DMA_getcount(DMA_Chan chan)
{
    DMA_Channel_TypeDef *ch = &DMA1x->chan[chan];

    return ch->CNDTR;
}

/** \brief Set the DMA configuration word for UART TX and RX

    \param chntx - enumerator for the transmit channel
    \param chnrx - enumerator for the receive channel
*/
void DMA_SetUART(DMA_Chan chntx, DMA_Chan chnrx)
{
    DMA_Channel_TypeDef *ch;

    // UART TX DMA Channel configuration
    ch = &DMA1x->chan[chntx];
    ch->CCR = DMA_UART_ATTR | DIR;          // set direction for outbound

    // UART RX DMA Channel configuration
    ch      = &DMA1x->chan[chnrx];
    ch->CCR = DMA_UART_ATTR | CIRC;         // set circular for inbound
}

/** \brief Initializes DMA channel used by the UART read/write routines.

    \param pBuffer - pointer to data buffer in RAM
    \param BufferSize - length of pBuffer in bytes
    \param devadr - address of the data register for the device (&uart[x]->DR)
    \param chn - enumerator for the desired DMA channel
*/
void DMA_ConfigUART(const volatile void *pBuffer, u32 BufferSize, u32 devadr, DMA_Chan chn)
{
    DMA_Channel_TypeDef *ch = &DMA1x->chan[chn];

    /* Initialize the DMA with the new parameters */
    ch->CCR  &= ~EN;                        // disable the channel
//    DMA1x->base.IFCR = (1u << 13);        // clear TC
    ch->CMAR  = HARDWARE(u32, pBuffer);     // set the RAM address
    ch->CNDTR = (u32)BufferSize;            // set the transfer length
    ch->CPAR  = devadr;                     // set the UART data reg address
    ch->CCR  |= EN;                         // go
}

// end of source

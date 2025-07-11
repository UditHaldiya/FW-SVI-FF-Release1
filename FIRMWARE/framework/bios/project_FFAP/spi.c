/**
Copyright 2011s by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file spi.c
    \brief Basic SPI functions, table-based

     CPU: LPC2114

    OWNER: EP                                                        //
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Justin Shriver $
*/

/* $History: spi.c $
 *
 * *****************  Version 6  *****************
 * User: Justin Shriver Date: 12/15/11   Time: 5:22p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- Put back the BIOS defis.
 *
 * *****************  Version 5  *****************
 * User: Justin Shriver Date: 12/15/11   Time: 4:03p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP --
 * - Fixed bug (shift by 8)
 * - Corrected GPIO usage
 * - Added channels
 *
 ** *****************  Version 4  *****************
 * User: Justin Shriver Date: 12/08/11   Time: 6:34p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- LINT - Removed unused headers.
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 12/07/11   Time: 7:44p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- TFS:7893 -- corrected headers.
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 12/07/11   Time: 6:24p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- TFS:7893
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 12/06/11   Time: 6:08p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- BIOS modularization
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 11/30/11   Time: 3:52p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * -- AP -- LINTY corrections.
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 3:54p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * BIOS modulariozation
 *
 *
*/

#include "mnwrap.h"
#include "stmcommon.h"
#include "spi.h"

#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000u)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800u)
//#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00u)

#define SPI1                HARDWARE(SPIx_TypeDef *, SPI1_BASE)
#define SPI2                HARDWARE(SPIx_TypeDef *, SPI2_BASE)
//lint ++flb
typedef struct
{
    IO_REG16(CR1);
    IO_REG16(CR2);
    IO_REG16(SR);
    IO_REG16(DR);
    IO_REG16(CRCPR);
    IO_REG16(RXCRCR);
    IO_REG16(TXCRCR);
    IO_REG16(I2SCFGR);
    IO_REG16(I2SPR);
} SPIx_TypeDef;


//  SPI_CR1
//Bit 15 BIDIMODE: Bidirectional data mode enable
#define BIDIMODE (1u << 15)
//0: 2-line unidirectional data mode selected
//1: 1-line bidirectional data mode selected
//Note: Not used in I2S mode
//Bit 14 BIDIOE: Output enable in bidirectional mode
//This bit combined with the BIDImode bit selects the direction of transfer in bidirectional mode
#define BIDIOE (1u << 14)
//0: Output disabled (receive-only mode)
//1: Output enabled (transmit-only mode)
//Note: In master mode, the MOSI pin is used and in slave mode, the MISO pin is used.
//Not used in I2S mode
//Bit 13 CRCEN: Hardware CRC calculation enable
#define CRCEN1 (1u << 13)
//0: CRC calculation disabled
//1: CRC calculation enabled
//Note: This bit should be written only when SPI is disabled (SPE = ‘0’) for correct operation
//Not used in I2S mode
//Bit 12 CRCNEXT: CRC transfer next
#define CRCNEXT (1u << 12)
//0: Data phase (no CRC phase)
//1: Next transfer is CRC (CRC phase)
//Note: When the SPI is configured in full duplex or transmitter only modes, CRCNEXT must be
//written as soon as the last data is written to the SPI_DR register.
//When the SPI is configured in receiver only mode, CRCNEXT must be set after the
//second last data reception.
//This bit should be kept cleared when the transfers are managed by DMA.
//Not used in I2S mode
//Bit 11 DFF: Data frame format
#define DFF16 (1u << 11)
//0: 8-bit data frame format is selected for transmission/reception
//1: 16-bit data frame format is selected for transmission/reception
//Note: This bit should be written only when SPI is disabled (SPE = ‘0’) for correct operation
//Not used in I2S mode
//Bit 10 RXONLY: Receive only
//This bit combined with the BIDImode bit selects the direction of transfer in 2-line
//unidirectional mode. This bit is also useful in a multislave system in which this particular
//slave is not accessed, the output from the accessed slave is not corrupted.
#define RXONLY (1u << 10)
//0: Full duplex (Transmit and receive)
//1: Output disabled (Receive-only mode)
//Note: Not used in I2S mode
//Bit 9 SSM: Software slave management
//When the SSM bit is set, the NSS pin input is replaced with the value from the SSI bit.
#define SSM (1u << 9)
//0: Software slave management disabled
//1: Software slave management enabled
//Note: Not used in I2S mode
//Bit 8 SSI: Internal slave select
#define SSI (1u << 8)
//This bit has an effect only when the SSM bit is set. The value of this bit is forced onto the
//NSS pin and the IO value of the NSS pin is ignored.
//Note: Not used in I2S mode
//Bit 7 LSBFIRST: Frame format
#define LSBFIRST (1u << 7)
//0: MSB transmitted first
//1: LSB transmitted first
//Note: This bit should not be changed when communication is ongoing.
//Not used in I2S mode
//Bit 6 SPE: SPI enable
#define SPE (1u << 6)
//0: Peripheral disabled
//1: Peripheral enabled
//Note: 1- Not used in I2S mode.
//Note: 2- When disabling the SPI, follow the procedure described in Section 25.3.8: Disabling the
//SPI.
//Bits 5:3 BR[2:0]: Baud rate control
#define SPI_DIV2   (0u << 3)
#define SPI_DIV4   (1u << 3)
#define SPI_DIV8   (2u << 3)
#define SPI_DIV16  (3u << 3)
#define SPI_DIV32  (4u << 3)
#define SPI_DIV64  (5u << 3)
#define SPI_DIV128 (6u << 3)
#define SPI_DIV256 (7u << 3)
//000: fPCLK/2 100: fPCLK/32
//001: fPCLK/4 101: fPCLK/64
//010: fPCLK/8 110: fPCLK/128
//011: fPCLK/16 111: fPCLK/256
//Note: These bits should not be changed when communication is ongoing.
//Not used in I2S mode
//Bit 2 MSTR: Master selection
#define MSTR (1u << 2)
//0: Slave configuration
//1: Master configuration
//Note: This bit should not be changed when communication is ongoing.
//Not used in I2S mode
//Bit1 CPOL: Clock polarity
#define CPOL (1u << 1)
//0: CK to 0 when idle
//1: CK to 1 when idle
//Note: This bit should not be changed when communication is ongoing.
//Not used in I2S mode
//Serial peripheral interface (SPI) RM0008
//714/1093 Doc ID 13902 Rev 13
//25.5.2 SPI control register 2 (SPI_CR2)
//Address offset: 0x04
//Reset value: 0x0000
//Bit 0 CPHA: Clock phase
#define CPHA (1u << 0)
//0: The first clock transition is the first data capture edge
//1: The second clock transition is the first data capture edge
//Note: This bit should not be changed when communication is ongoing.
//Note: Not used in I2S mode
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//TXEIE RXNEIE ERRIE Res. Res. SSOE TXDMAEN RXDMAEN

// SPI_CR2
//Bits 15:8 Reserved, must be kept at reset value.
//Bit 7 TXEIE: Tx buffer empty interrupt enable
#define TXEIE (1u << 7)
//0: TXE interrupt masked
//1: TXE interrupt not masked. Used to generate an interrupt request when the TXE flag is set.
//Bit 6 RXNEIE: RX buffer not empty interrupt enable
#define RXNEIE (1u << 6)
//0: RXNE interrupt masked
//1: RXNE interrupt not masked. Used to generate an interrupt request when the RXNE flag is
//set.
//Bit 5 ERRIE: Error interrupt enable
//This bit controls the generation of an interrupt when an error condition occurs (CRCERR,
//OVR, MODF in SPI mode and UDR, OVR in I2S mode).
#define ERRIE (1u << 5)
//0: Error interrupt is masked
//1: Error interrupt is enabled
//Bits 4:3 Reserved, must be kept at reset value.
//Bit 2 SSOE: SS output enable
#define SSOE (1u << 2)
//0: SS output is disabled in master mode and the cell can work in multimaster configuration
//1: SS output is enabled in master mode and when the cell is enabled. The cell cannot work
//in a multimaster environment.
//Note: Not used in I2S mode
//Bit 1 TXDMAEN: Tx buffer DMA enable
//When this bit is set, the DMA request is made whenever the TXE flag is set.
#define TXDMAEN (1u << 1)
//0: Tx buffer DMA disabled
//1: Tx buffer DMA enabled
//Bit 0 RXDMAEN: Rx buffer DMA enable
//When this bit is set, the DMA request is made whenever the RXNE flag is set.
#define RXDMAEN (1u << 9)
//0: Rx buffer DMA disabled
//1: Rx buffer DMA enabled

// SPI_SR
//Bits 15:8 Reserved, must be kept at reset value.
//Bit 7 BSY: Busy flag
#define BSY (1u << 7)
//0: SPI (or I2S)not busy
//1: SPI (or I2S)is busy in communication or Tx buffer is not empty
//This flag is set and cleared by hardware.
//Note: BSY flag must be used with caution: refer to Section 25.3.7: Status flags and
//Section 25.3.8: Disabling the SPI.
//Bit 6 OVR: Overrun flag
#define OVR (1u << 6)
//0: No overrun occurred
//1: Overrun occurred
//This flag is set by hardware and reset by a software sequence. Refer to Section 25.4.7 on
//page 710 for the software sequence.
//Bit 5 MODF: Mode fault
#define MODF (1u << 5)
//0: No mode fault occurred
//1: Mode fault occurred
//This flag is set by hardware and reset by a software sequence. Refer to Section 25.3.10 on
//page 691 for the software sequence.
//Note: Not used in I2S mode
//Bit 4 CRCERR: CRC error flag
#define CRCERR (1u << 4)
//0: CRC value received matches the SPI_RXCRCR value
//1: CRC value received does not match the SPI_RXCRCR value
//This flag is set by hardware and cleared by software writing 0.
//Note: Not used in I2S mode
//Bit 3 UDR: Underrun flag
#define UDR (1u << 3)
//0: No underrun occurred
//1: Underrun occurred
//This flag is set by hardware and reset by a software sequence. Refer to Section 25.4.7 on
//page 710 for the software sequence.
//Note: Not used in SPI mode
//Bit 2 CHSIDE: Channel side
#define CHSIDE (1u << 2)
//0: Channel Left has to be transmitted or has been received
//1: Channel Right has to be transmitted or has been received
//Note: Not used for the SPI mode. No meaning in PCM mode
//Bit 1 TXE: Transmit buffer empty
#define TXE (1u << 2)
//0: Tx buffer not empty
//1: Tx buffer empty
//Bit 0 RXNE: Receive buffer not empty
#define RXNE (1u << 0)
//0: Rx buffer empty
//1: Rx buffer not empty

// (SPI_I2SCFGR
//Bits 15:12 Reserved, must be kept at reset value.
//Bit 11 I2SMOD: I2S mode selection
#define I2SMOD (1u << 11)
//0: SPI mode is selected
//1: I2S mode is selected
//Note: This bit should be configured when the SPI or I2S is disabled
//Bit 10 I2SE: I2S Enable
#define I2SE (1u << 10)
//0: I2S peripheral is disabled
//1: I2S peripheral is enabled
//Note: Not used in SPI mode
//Bit 9:8 I2SCFG: I2S configuration mode
#define I2SCFG_SLT (0u << 8)
#define I2SCFG_SLR (1u << 8)
#define I2SCFG_MST (2u << 8)
#define I2SCFG_MSR (3u << 8)
//00: Slave - transmit
//01: Slave - receive
//10: Master - transmit
//11: Master - receive
//Note: This bit should be configured when the I2S is disabled.
//Not used for the SPI mode
//Bit 7 PCMSYNC: PCM frame synchronization
#define PCMSYNC (1u << 7)
//0: Short frame synchronization
//1: Long frame synchronization
//Note: This bit has a meaning only if I2SSTD = 11 (PCM standard is used)
//Not used for the SPI mode
//Bit 6 Reserved: forced at 0 by hardware
//Bit 5:4 I2SSTD: I2S standard selection
#define I2SSTD (0u << 4)
#define I2MSB  (1u << 4)
#define I2LSB  (2u << 4)
#define I2PCM  (3u << 4)
//00: I2S Phillips standard.
//01: MSB justified standard (left justified)
//10: LSB justified standard (right justified)
//11: PCM standard
//For more details on I2S standards, refer to Section 25.4.2 on page 694. Not used in SPI mode.
//Note: For correct operation, these bits should be configured when the I2S is disabled.
//Bit 3 CKPOL: Steady state clock polarity
#define CKPOL (1u << 3)
//0: I2S clock steady state is low level
//1: I2S clock steady state is high level
//Note: For correct operation, this bit should be configured when the I2S is disabled.
//Not used in SPI mode
//Serial peripheral interface (SPI) RM0008
//719/1093 Doc ID 13902 Rev 13
//25.5.9 SPI_I2S prescaler register (SPI_I2SPR)
//Address offset: 0x20
//Reset value: 0000 0010 (0x0002)
//Bit 2:1 DATLEN: Data length to be transferred
#define DATLEN_16 (0u << 1)
#define DATLEN_24 (1u << 1)
#define DATLEN_32 (2u << 1)
#define DATLEN_xx (3u << 1)
//00: 16-bit data length
//01: 24-bit data length
//10: 32-bit data length
//11: Not allowed
//Note: For correct operation, these bits should be configured when the I2S is disabled.
//Not used in SPI mode.
//Bit 0 CHLEN: Channel length (number of bits per audio channel)
#define CHLEN (1u << 0)
//0: 16-bit wide
//1: 32-bit wide
//The bit write operation has a meaning only if DATLEN = 00 otherwise the channel length is fixed to
//32-bit by hardware whatever the value filled in. Not used in SPI mode.
//Note: F

// SPI_I2SPR
//Bits 15:10 Reserved, must be kept at reset value.
//Bit 9 MCKOE: Master clock output enable
#define MCKOE (1u << 9)
//0: Master clock output is disabled
//1: Master clock output is enabled
//Note: This bit should be configured when the I2S is disabled. It is used only when the I2S is in master
//mode.
//Not used in SPI mode.
//Bit 8 ODD: Odd factor for the prescaler
#define ODD (1u << 8)
//0: real divider value is = I2SDIV *2
//1: real divider value is = (I2SDIV * 2)+1
//Refer to Section 25.4.3 on page 701. Not used in SPI mode.
//Note: This bit should be configured when the I2S is disabled. It is used only when the I2S is in master
//mode.
#define I2SDIV_MSK 0xff
//Bit 7:0 I2SDIV: I2S Linear prescaler
//I2SDIV [7:0] = 0 or I2SDIV [7:0] = 1 are forbidden values.
//Refer to Section 25.4.3 on page 701. Not used in SPI mode.
//Note: These bits should be configured when the I2S is disabled. It is used only when the I2S is in
//master mode.
//

#define SPI_ERROR_MASK      (OVR | MODF | CRCERR | UDR)
#define BITS_PER_BYTE       (8u)

// ------------------------------------------------------------------------------
// This function is exposed to and used ONLY in GPIO_key.c

void spi_SetEnableDisableDev2(bool_t disable);

// ------------------------------------------------------------------------------
// Initial CR1 values

#define SPI1_CR1_CONFIG         (DFF16 | SPI_DIV16             | MSTR | CPOL | CPHA)
#define SPI2_CR1_CONFIG         (        SPI_DIV16 | SSM | SSI | MSTR | CPOL | CPHA)

static u16 SPI2_CR1_val;

//lint --flb

// For compatibiliry of the interfaces:
// Channel 0 - mapped to SPI2 - 8
// Channel 1 - mapped to SPI1 - 16
// ------------------------------------------------------------------------------
MN_INLINE void setspix(SPIx_TypeDef *SPIx, u32 CR1_val, u16 CR2_val)
{
    SPIx->CR1     = 0u;
    SPIx->CR2     = CR2_val;
//   SPIx->SR      = ;    // BSY  RXNE
//   SPIx->DR      = ;
    SPIx->CRCPR   = 0u;
    SPIx->RXCRCR  = 0u;
    SPIx->TXCRCR  = 0u;
    SPIx->I2SCFGR = 0u;
    SPIx->I2SPR   = 0u;

    SPIx->CR1     = (u16)CR1_val;
    SPIx->CR1     = (u16)CR1_val | SPE;
}

/** \brief Set Enable/disable the SPI2 controller onto the GPIO pins

*/

void spi_SetEnableDisableDev2(bool_t disable)
{
    if (disable)
    {
        SPI2_CR1_val = SPI2_CR1_CONFIG;
    }
    else
    {
        SPI2_CR1_val = SPI2_CR1_CONFIG | SPE;
    }
}

// ------------------------------------------------------------------------------
void spi_Initialize(void)
{
    setspix(SPI2, SPI2_CR1_CONFIG, 0u);
    setspix(SPI1, SPI1_CR1_CONFIG, SSOE);
}

// ------------------------------------------------------------------------------
/** \brief Transfer 1 8-bit word via SPI Channel 0
  \return u32 the MSB as a 32-bit value
*/
u32 spi_Read8bit_Ch0(void)
{
    u32 RetVar;

    if (SPI2_CR1_val != SPI2->CR1)
    {
        SPI2->CR1 =  SPI2_CR1_val;
    }
    if ((SPI2_CR1_val & SPE) == SPE)
    {
        SPI2->DR = 0u;
        while ((SPI2->SR & (u16)RXNE) != (u16)RXNE)
        {
            ; // wait for transfer
        }
        RetVar = (u32)SPI2->DR;     // Get the data
    }
    else
    {
        RetVar = 0u;
    }

    return RetVar;
}

/** \brief Transfer 1 16-bit word via SPI Channel 0
  \return u32 the MSB as a 32-bit value
*/
u32 spi_Read16bit_Ch0(void)
{
    u32     RetVar;
    u32     TempVar;

    TempVar = spi_Read8bit_Ch0();
    RetVar  = TempVar << BITS_PER_BYTE;

    TempVar = spi_Read8bit_Ch0();
    RetVar |= TempVar;

    return RetVar;
}

/** \brief Initiate Data Read via SPI Channel 1
*/
void spi_StartRead_Ch1(void)
{
    SPI1->DR = 0u;      /* Start reading MSBs of A/D via SPI1 (87us until result is available) */
}

/** \brief Complete previously started Read of 1 8-bit word via SPI Channel 1
  \return u32 the MSB as a 32-bit value
*/
u32 spi_CompleteRead16bit_Ch1(void)
{
    u32 RetVar;

    while ((SPI1->SR & (u16)RXNE) != (u16)RXNE)
    {
        // wait for transfer
    }
    RetVar = (u32)SPI1->DR;
    return RetVar;
}

/** \brief Transfer 1 16-bit word via SPI Channel 1
  \return u32 the MSB as a 32-bit value
*/
u32 spi_Read16bit_Ch1(void)
{
    u32     RetVar;

    spi_StartRead_Ch1();
    RetVar = spi_CompleteRead16bit_Ch1();
    return RetVar;
}

// ------------------------------------------------------------------------------
/**
    \brief WRite 8-bit Data via SPI Channel 0 - Enable/Disable the controller, if needed
    \param - u8 Data
*/
void    spi_Write8bit_Ch0(u8 Data)
{
    if (SPI2_CR1_val != SPI2->CR1)
    {
        SPI2->CR1 =  SPI2_CR1_val;
    }
    if ((SPI2_CR1_val & SPE) == SPE)
    {
        SPI2->DR = Data;
        while ((SPI2->SR & (u16)RXNE) != (u16)RXNE)
        {
            ; // wait for transfer
        }
        (void)SPI2->DR;
    }
}

/**
    \brief WRite 16-bit Data via SPI Channel 0
    \param - u16 Data
*/
void    spi_Write16bit_Ch0(u16 Data)
{
    spi_Write8bit_Ch0((u8)(Data >> BITS_PER_BYTE));
    spi_Write8bit_Ch0((u8)Data);
}

/**
    \brief WRite 16-bit Data via SPI Channel 1
    \param - u16 Data
*/
void    spi_Write16bit_Ch1(u16 Data)
{
    SPI1->DR = Data;
    while ((SPI1->SR & (u16)RXNE) != (u16)RXNE)
    {
        ; // wait for transfer
    }
    (void)SPI1->DR;
}

/**
    \brief Test the SPI Status register Channel 0
    \return - true - No errors, OK to proceed; false - error
*/
bool_t  spi_GetSPIStatus_Ch0(void)
{
    bool_t  retval = true;

    if (0u != (SPI2->SR & SPI_ERROR_MASK))            /* no error or status flag should be set at this point */
    {
        retval = false;
    }
    return retval;
}

/**
    \brief Test the SPI Status register Channel 1
    \return - true - No errors, OK to proceed; false - error
*/
bool_t  spi_GetSPIStatus_Ch1(void)
{
    bool_t  retval = true;

    if (0u != (SPI1->SR & SPI_ERROR_MASK))            /* no error or status flag should be set at this point */
    {
        retval = false;
    }
    return retval;
}

// ------------------------------------------------------------------------------

void spi_Sio1Select(void)
{
    ; // Nothing - Compatibility API
}

/**
    \brief Set SPI #1 and GPIO back to idle after position retransmit or A/D input

    \param - none
*/
void spi_Sio1Deselect(void)
{
    ; // Nothing - Compatibility API
}

/** \brief Set SPI mode for the LCD port.  It is normally GPIO to conserve power.
*/
void spi_Sio0Select(void)
{
    ; // Nothing - Compatibility API
}

/** \brief Reset LCD port to GPIO mode to conserve power
*/
void spi_Sio0Deselect(void)
{
    ; // Nothing - Compatibility API
}

//--------------------------------------------------------------

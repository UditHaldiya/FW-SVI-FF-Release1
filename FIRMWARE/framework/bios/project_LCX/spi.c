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
 * User: Justin Shriver Date: 12/15/11   Time: 3:50p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * -- AP -- ADded cghannel-dependant operations.
 *
 * *****************  Version 5  *****************
 * User: Justin Shriver Date: 12/12/11   Time: 5:43p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * -- AP -- Fixed teh bug - shift 8 times.
 *
 * *****************  Version 4  *****************
 * User: Justin Shriver Date: 12/07/11   Time: 6:21p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * -- AP -- COrrected the syntax.
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 12/06/11   Time: 6:34p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * -- AP -- ADjustments for commonalities.
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
#include "oswrap.h"
#include "lpc2114io.h"
#include "bios_def.h"

#include "oswrap.h"
#include "bios.h"
#include "spi.h"

// ------------------------------------------------------------------------------
#define SIO0_CONTROL_INIT   (CPOL | MSTR)
#define SIO0_CLOCK_DIVIDER  ((u32)8)         /* select maximum SPI0 frequency 1.84e6/(2*8) = 115 kbaud */

#define SIO1_CONTROL_INIT   (CPOL | MSTR)
#define SIO1_FREQ_PRESCALER ((u32)8)    /* division of 8 is minimum prescaler value of SIO1 */

#define BITS_PER_BYTE       (8u)

// ------------------------------------------------------------------------------
void spi_Initialize(void)
{
    spi_Sio1Select(); /* Program SSEL from GPIO to SIO usage */
    SPI1->SPCCR = SIO1_FREQ_PRESCALER;  /* select maximum SPI1 frequency 1.84e6/(2*8) = 115 kbaud */
    spi_Sio1Deselect(); /* Program SSEL from SIO to GPIO usage. Set SSEL to 0 to save power */

    spi_Sio0Select(); /* Program SSEL from GPIO to SIO usage */
    SPI0->SPCCR = SIO0_CLOCK_DIVIDER;
    spi_Sio0Deselect(); /* Program SSEL from GPIO to SIO usage */
}

// ------------------------------------------------------------------------------
/** \brief Transfer 1 8-bit word via SPI Channel 0
  \return u32 the MSB as a 32-bit value
*/
u32 spi_Read8bit_Ch0(void)
{
    u32 RetVar;

    SPI0->SPDR = 0; /* Start reading MSBs of A/D via SPI1 (87us until result is available) */
    while (SPIF != (SPI0->SPSR & SPIF)) /* wait for end of transmission */
    {
        ;
    }
    RetVar = (u32)SPI0->SPDR;
    return RetVar;
}

/** \brief Initiate Data Read via SPI Channel 1
*/
void spi_StartRead_Ch1(void)
{
    SPI1->SPDR = 0; /* Start reading MSBs of A/D via SPI1 (87us until result is available) */
}

/** \brief Complete previously started Read of 1 8-bit word via SPI Channel 1
  \return u32 the MSB as a 32-bit value
*/
u32 spi_CompleteRead8bit_Ch1(void)
{
    u32 RetVar;

    while (SPIF != (SPI1->SPSR & SPIF)) /* wait for end of transmission */
    {
        ;
    }
    RetVar = (u32)SPI1->SPDR;
    return RetVar;
}

/** \brief Transfer 1 8-bit word via SPI Channel 1
  \return u32 the MSB as a 32-bit value
*/
u32 spi_Read8bit_Ch1(void)
{
    u32 RetVar;

    SPI1->SPDR = 0; /* Start reading MSBs of A/D via SPI1 (87us until result is available) */

    RetVar = spi_CompleteRead8bit_Ch1();
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

/** \brief Transfer 1 16-bit word via SPI Channel 1
  \return u32 the MSB as a 32-bit value
*/
u32 spi_Read16bit_Ch1(void)
{
    u32     RetVar;
    u32     TempVar;

    TempVar = spi_Read8bit_Ch1();
    RetVar  = TempVar << BITS_PER_BYTE;

    TempVar = spi_Read8bit_Ch1();
    RetVar |= TempVar;

    return RetVar;
}

/** \brief Complete previously started Read of 1 16-bit word via SPI Channel 1
  \return u32 the MSB as a 32-bit value
*/
u32 spi_CompleteRead16bit_Ch1(void)
{
    u32     RetVar;
    u32     TempVar;

    TempVar = spi_CompleteRead8bit_Ch1();
    RetVar  = TempVar << BITS_PER_BYTE;

    TempVar = spi_Read8bit_Ch1();
    RetVar |= TempVar;

    return RetVar;
}

// ------------------------------------------------------------------------------
/**
    \brief WRite 8-bit Data via SPI Channel 0
    \param - u8 Data
*/
void    spi_Write8bit_Ch0(u8 Data)
{
    SPI0->SPDR = Data;                          /* send command to LCD controller */
    while (SPIF != (SPI0->SPSR & SPIF))         /* wait for end of transmission */
    {
        ;
    }
}

/**
    \brief WRite 8-bit Data via SPI Channel 1
    \param - u8 Data
*/
void    spi_Write8bit_Ch1(u8 Data)
{
    SPI1->SPDR = Data;                          /* send command to LCD controller */
    while (SPIF != (SPI1->SPSR & SPIF))         /* wait for end of transmission */
    {
        ;
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
    spi_Write8bit_Ch1((u8)(Data >> BITS_PER_BYTE));
    spi_Write8bit_Ch1((u8)Data);
}

/**
    \brief Test the SPI Status register Channel 0
    \return - true - No errors, OK to proceed; false - error
*/
bool_t  spi_GetSPIStatus_Ch0(void)
{
    bool_t  retval = true;

    if (0u != SPI0->SPSR)            /* no error or status flag should be set at this point */
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

    if (0u != SPI1->SPSR)            /* no error or status flag should be set at this point */
    {
        retval = false;
    }
    return retval;
}

// ------------------------------------------------------------------------------

void spi_Sio1Select(void)
{
#if 0
    GPIO0->IOSET = GPIO0_SSEL1;         /* Set SSEL pin to High */
    PINSEL1      = PINSEL1_SSEL_SIO;    /* Select SSEL pin as SIO */
#endif
    (void)SPI1->SPSR;                   /* clear pending error */
    SPI1->SPCR = SIO1_CONTROL_INIT;     /* Init Sio1 control register  with Master SIO mode */
}

/**
    \brief Set SPI #1 and GPIO back to idle after position retransmit or A/D input

    \param - none
*/
void spi_Sio1Deselect(void)
{
#if 0
    PINSEL1      = PINSEL1_SSEL_GPIO;
    GPIO0->IOCLR = GPIO0_SSEL1;
#endif
}

/** \brief Set SPI mode for the LCD port.  It is normally GPIO to conserve power.
*/
void spi_Sio0Select(void)
{
#if 0
    GPIO0->IOSET = GPIO0_SSEL0;         /* Set SSEL pin to High */
    PINSEL0      = PINSEL0_SSEL_SIO;    /* Select SSEL pin as SIO */
#endif
    (void)SPI0->SPSR;                   /* clear pending error */
    SPI0->SPCR   = SIO0_CONTROL_INIT;   /* Init Sio1 control register  with Master SIO mode */
}

/** \brief Reset LCD port to GPIO mode to conserve power
*/
void spi_Sio0Deselect(void)
{
#if 0
    PINSEL0      = PINSEL0_SSEL_GPIO;
    GPIO0->IOCLR = GPIO0_SSEL0;
#endif
}

//--------------------------------------------------------------

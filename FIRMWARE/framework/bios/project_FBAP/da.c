/*
Copyright 2004-2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file da.h
    \brief Position retransmit AO channel driver

    CPU: Philips LPC21x4 (ARM)

    OWNER: AP

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP/da.c $
    $Date: 1/12/12 12:27p $
    $Revision: 19 $
    $Author: Arkkhasin $

    \ingroup D_to_A
*/

#include "mnwrap.h"
#include "oswrap.h"
#include "lpc2114io.h"
#include "bios_def.h"
#include "inituc.h"
#include "da.h"
#include "spi.h"

/*******************************************************************************
*** internal types
*******************************************************************************/

/* D13..D15 of DAC data word are command bits */
/* Load input register and DAC register, exit shutdown */
#define DAC_COMMAND_LOAD 0x4000

#define DA_LIMIT_MAX  0x00000FFF  /* limit D/A value to maximum value of D/A (12bit) */
//#define DA_LIMIT_MIN  0x0 /* limit D/A value to minimum value */

//----------------------------------------------------------------------------------------------------------------------
/** \brief Select External D/A converter
    \param  - None.
    \return - Nothing
*/
static void    da_SelectExtD2A(void)
{
    GPIO1->IOCLR = GPIO1_CS_DA;         /* Enable D/A CS line (low active) */
}

/** \brief De-Select External D/A converter
    \param  - None.
    \return - Nothing
*/
static void    da_DeSelectExtD2A(void)
{
    GPIO1->IOSET = GPIO1_CS_DA;           /* disable D/A CS line */
}

/**
    \brief Initialize SPI #1 and the position retransmitter DAC

    \param none
*/
void bios_InitDa(void)
{
    ;
    // spi_Sio1Select(); /* Program SSEL from GPIO to SIO usage */
    // SPI1->SPCCR = SIO1_FREQ_PRESCALER;  /* select maximum SPI1 frequency 1.84e6/(2*8) = 115 kbaud */
    // spi_Sio1Deselect(); /* Program SSEL from SIO to GPIO usage. Set SSEL to 0 to save power */
}

static u16 lastDA; //self-correcting

u16 bios_GetLastDa(void)
{
    return lastDA;
}

/**
    \brief Output the specified value to the position retransmitter

    \param[in] u16 value - value to be sent 0..4095
*/
void bios_WriteDa(u16 value)
{
    value = MIN(value, DA_LIMIT_MAX);
    lastDA = value;
    /* Shift data word to D1..D12, or command word */
    //lint -save -e734  loss of precision is OK
    value = ((value << 1u) | (u16)DAC_COMMAND_LOAD);
    //lint -restore
    /* Disable interrupts during usage of SPI1 ***/
    MN_ENTER_CRITICAL();
        spi_Sio1Select();                   /* Program SSEL from GPIO to SIO usage */

        da_SelectExtD2A();                  /* Enable D/A CS line (low active) */

        if (!spi_GetSPIStatus_Ch1())        /* no error or status flag should be set at this point */
        {
            //Dead code Errorhandler(BIOS_SPI1_ERROR);
        }

        // Send the data
        spi_Write16bit_Ch1(value);          // SPI driver will use either 8- or 16-bit operations

        // Done with data
        da_DeSelectExtD2A();                /* disable D/A CS line */
        spi_Sio1Deselect();                 /* Program SSEL from SIO to GPIO usage. Set SSEL to 0 to save power */

    MN_EXIT_CRITICAL(); //OS_EXIT_CRITICAL();
}

/* End of the source */

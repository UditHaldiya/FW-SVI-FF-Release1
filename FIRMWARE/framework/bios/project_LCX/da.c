/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file da.c
    \brief BIOS routines for position retransmit

    CPU: Any (with standard C compiler)

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/

#include "mnwrap.h"
// #include "oswrap.h"
#include "lpc2114io.h"
//#include "bios_def.h"
#include "da.h"

//-----------------------------------------------------------------------
// Below are hacks for the LCX UI.
//
// AP : Need to revisit and remove them!!
//
// Non-stub functions:

/** \brief Set SPI #1 and GPIO for use in position retransmit or A/D input

*/
#define SIO1_CONTROL_INIT (CPOL | MSTR)
void Sio1Select(void)
{
    //GPIO0->IOSET = GPIO0_SSEL1;         /* Set SSEL pin to High */
    //PINSEL1      = PINSEL1_SSEL_SIO;    /* Select SSEL pin as SIO */
    (void)SPI1->SPSR;                   /* clear pending error */
    SPI1->SPCR   = SIO1_CONTROL_INIT;   /* Init Sio1 control register  with Master SIO mode */
}

/**
    \brief Set SPI #1 and GPIO back to idle after position retransmit or A/D input

    \param - none
*/
void Sio1Deselect(void)
{
    //PINSEL1      = PINSEL1_SSEL_GPIO;
    //GPIO0->IOCLR = GPIO0_SSEL1;
}

#define SIO1_FREQ_PRESCALER 8 /* division of 8 is minimum prescaler value of SIO1 */

//void bios_HandleDigitalInput(void) {}
void bios_InitDa(void)
{
    Sio1Select(); /* Program SSEL from GPIO to SIO usage */
    SPI1->SPCCR = SIO1_FREQ_PRESCALER;  /* select maximum SPI1 frequency 1.84e6/(2*8) = 115 kbaud */
    Sio1Deselect(); /* Program SSEL from SIO to GPIO usage. Set SSEL to 0 to save power */
}

// End of non-stub functions

/* End of the source */

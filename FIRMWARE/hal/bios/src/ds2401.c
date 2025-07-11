/**
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ds2401.c
    \brief BIOS routine to read DS2401 serial number

    See also ds2401_a.s79

    CPU: Philips LPC21x4

    OWNER: EP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src/ds2401.c $
    $Date: 6/15/10 2:35p $
    $Revision: 2 $
    $Author: Anatoly Podpaly $

    \ingroup ds2401
*/

/*
    This routine uses a single GPIO port bit to read the DS2401 serial number.  Bit and reset timing
    are accomplished using Tiomer1 which is assumed to be in free-run mode. Timing should work correctly 
    even if the crystal is changed.
*/
#include "mnwrap.h"
#include "lpc2114io.h"
#include "timebase.h"
#include "mnassert.h"
#include "oswrap.h"
#include "ds2401.h"

#define DO_IT_IN_ASM  1

#if VPRATE == VPBDIV1
#define IO_TICKS 8.0                // cpu cycles per 1 IO cycle
#elif VPRATE == VPBDIV2 
#define IO_TICKS 10.0               // cpu cycles per 1 IO cycle
#elif VPRATE == VPBDIV4 
#define IO_TICKS 16.0               // cpu cycles per 1 IO cycle
#else
#error Cannot work with this VPRATE
#endif

#define ONE_IO              (IO_TICKS / (cdouble_t)XTALFREQ)
CONST_ASSERT((s32)(ONE_IO * 1e6) > 0);  // make sure start bit is long enough

#if !DO_IT_IN_ASM
 #define F2I(x)              ((u32)((x) + .5))
 #define TIMER_TICK          ((cdouble_t)VPRATE / (cdouble_t)XTALFREQ)
 #define ONE_BIT_TIME        (60e-6)
 #define HALF_RES_TIME       (480e-6)

 #define DELAY_CST(NO_IO, NO_US) F2I((NO_US - (NO_IO * ONE_IO)) / TIMER_TICK)

 #define OW_PIN      19u             // should be 16 for production units
 #define PINMASK     (1u << OW_PIN)
 #define MSB_POS     31u

 #define READROM     0x33u           // command to DS2401
 #define BYTE_BITS   8u
 #define U8_BITS  (sizeof(u8)  * BYTE_BITS)          // # of bits in a u8
 #define U32_BITS (sizeof(u32) * BYTE_BITS)          // # of bits in a u32
#endif // DO_IT_IN_ASM

// CRC polynomial = x8 + x5 + x4 + 1 
#define POLY_BIT(x) (0x80u >> (x))
#define POLY_8 (POLY_BIT(0) | POLY_BIT(4) | POLY_BIT(5)) 
#define ROM_WIDTH   64
#define CRC_WIDTH   8
#define SER_WIDTH   (ROM_WIDTH - CRC_WIDTH) 

/*  \brief compute the 8-bit CRC of the 56 LSB of the passed word 

    \param wd - 64 bit word 

    \return u8_least the computed CRC
*/
static u8_least crc_8(u64 wd)
{
    u16_least crc = 0x0; 
    u8_least  loopCt;

    for (loopCt = 0; loopCt < SER_WIDTH; loopCt++)
    {
        if ((((u32)wd ^ crc) & 1u) != 0u)
        {
            crc ^= (POLY_8 << 1);
        }
        crc >>= 1;
        wd  >>= 1;
    }
    return (u8_least)crc;
}

#if DO_IT_IN_ASM
/*  \brief Subrooutine to reset, command and read the DS2401.  See also 
    ds2401_a.s79

    \param port - base address of GPIO0
    \param tmr - address of T1->TC (Timer1 Count Register)
    \return 
*/
extern u64 OW_ReadSequence(gpio_t *port, const volatile u32 *tmr);
#else

/*  \brief Set the 1-Wire port to the specified state and delay for 
            approximately 480 microseconds 

    \param port   - pointer to GPIO0
    \param portHL - value to store in the port direction register
    \param pTmr   - pointer to the Timer1 count register
*/
static void OW_HalfReset(gpio_t *port, u32 portHL, const volatile u32 *pTmr)
{
    u32 startTime; 

    port->IODIR = portHL;
    for (startTime = *pTmr; (*pTmr - startTime) < DELAY_CST(3, HALF_RES_TIME);)
    {
    }
}

/*  \brief Write the command byte to the 1-Wire device - LSB first

    \param port    - pointer to GPIO0
    \param portH   - value to store in the port direction register for a '1'
    \param portL   - value to store in the port direction register for a '0'
    \param byteVal - the 8-bit value to be written
    \param pTmr    - pointer to the Timer1 count register
*/
static void OW_WriteByte(gpio_t *port, u32 portH, u32 portL, u8_least byteVal, const volatile u32 *pTmr)
{
    u32 startTime, bitCount;

    for (bitCount = 0; bitCount < U8_BITS; bitCount++)
    {
        port->IODIR = portL;                              // drive low
        if ((byteVal & 1u) != 0u)                           
        {
            port->IODIR = portH;                          // release early for '1'
        }

        // delay for rest of bit time
        for (startTime = *pTmr; (*pTmr - startTime) < DELAY_CST(4, ONE_BIT_TIME);)
        {
        }
        port->IODIR = portH;                          // release for recovery
        byteVal   >>= 1;
    }
}

/*  \brief Read a 32-bit word from the 1-Wire device

    \param port  - pointer to GPIO0
    \param portH - value to store in the port direction register for a '1'
    \param portL - value to store in the port direction register for a '0'
    \param pTmr  - pointer to the Timer1 count register

    \return u64  - the 32-bit value read from the device
*/
static u64 OW_ReadWord(gpio_t *port, u32 portH, u32 portL, const volatile u32 *pTmr)
{
    u32 startTime, bitCount, tmp, val = 0;

    for (bitCount = 0; bitCount < U32_BITS; bitCount++)
    {
        port->IODIR = portL;                                  // drive low
        port->IODIR = portH;                                  // release
        tmp   = port->IOPIN << (MSB_POS - OW_PIN); // sample & align to bit pos 31
        val   = (val >> 1) | (tmp & (1u << MSB_POS)) ;  // merge new bit 

        // delay for rest of bit time
        for (startTime = *pTmr; (*pTmr - startTime) < DELAY_CST(5, ONE_BIT_TIME);)
        {
        }
    }
    return (u64)val;
}


/*  \brief Read a 32-bit word from the 1-Wire device

    \param port  - pointer to GPIO0
    \param pTmr  - pointer to the Timer1 count register
    \return u64  - the 32-bit value read from the device
*/
static u64 OW_ReadSequence(gpio_t *port, const volatile u32 *tmr)
{
    u32 varL = port->IODIR |  PINMASK;                  // value to set port as output
    u32 varH = varL  & ~PINMASK;                        // value to set port as input
    u64 val;                                            // accumulator for serial #

    OW_HalfReset(port, varL, tmr);                      // drive low for 480 us   
    OW_HalfReset(port, varH, tmr);                      // release for 480 us (we don't check for presence)
    OW_WriteByte(port, varH, varL, READROM, tmr);       // send READROM command
    val  = OW_ReadWord(port, varH, varL, tmr);
    val |= OW_ReadWord(port, varH, varL, tmr) << U32_BITS;
    return val;
}

#endif // DO_IT_IN_ASM

/* \brief Read the DS2401 serial number.  If the CRC is OK store the serial at
        *serial.  The CRC is stripped.

    \return bool_t - true if CRC is OK; otherwise false
*/
bool_t DS2401Read(u64 *serial)
{ 
    u64 val;
    bool_t retval;

    MN_DBG_ASSERT(!oswrap_IsOSRunning());
    MN_DBG_ASSERT(VPBDIV == VPRATE);

    val = OW_ReadSequence(GPIO0, &T1->TC); 

    retval = (val != (u64)0) &&             // all 0's would pass CRC a stuck   
                                            // bus or missing resistor could do this
            (crc_8(val) == (val >> SER_WIDTH));
    if (retval)
    {
        *serial = val & ~((u64)0xff << SER_WIDTH);
    }
    return retval;
}

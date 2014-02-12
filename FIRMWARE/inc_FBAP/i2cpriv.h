/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file fram.c
    \brief Device driver for non-volatile FRAM device

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP/i2cpriv.h $
    $Date: 1/16/12 6:16p $
    $Revision: 1 $
    $Author: Anatoly Podpaly $

    \ingroup I2C
*/

/* $History: i2cpriv.h $
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 1/16/12    Time: 6:16p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * Moved from Generic BIOS layer
 * 
 * *****************  Version 12  *****************
 * User: Anatoly Podpaly Date: 8/20/10    Time: 4:22p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Cleaning and preparation for Module packaging.
 * 
 * *****************  Version 11  *****************
 * User: Anatoly Podpaly Date: 1/13/10    Time: 10:49a
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Replaced predefined number of retries in I2C start with calculated
 * number, based on system clock.
 * 
 * *****************  Version 10  *****************
 * User: Anatoly Podpaly Date: 1/12/10    Time: 6:16p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Reverted T/O to number of itrerations for detection of I2C Bus Stuck
 * condition.
 * 
 * *****************  Version 9  *****************
 * User: Anatoly Podpaly Date: 12/18/09   Time: 11:36a
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Corrected Typo.
 * 
 * *****************  Version 8  *****************
 * User: Anatoly Podpaly Date: 12/18/09   Time: 11:27a
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Code review comments and suggestions.
 * 
 * *****************  Version 7  *****************
 * User: Anatoly Podpaly Date: 12/18/09   Time: 10:22a
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Corrected comment.
 * 
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 12/18/09   Time: 10:16a
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Added and removed comments.
 * 
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 12/16/09   Time: 4:17p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Bug #2267: moved include of Timer and timebase headers into i2cpriv.h
 * 
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 12/10/09   Time: 11:02a
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Bug 2214: Corrected STOP condition.
 * 
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 12/09/09   Time: 8:06p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Bug 2214: Replaced the arbitrary T/O counter, that was dependant on CPU
 * execution with T/O based on Real Tme counter. 
 * - Removed an unneccssary parameter in teh function I2C_Start - the
 * status AA18, as it is the part of I2C protocol and is internal to the
 * function.
 * - Added comments, file history.
 * 
*/


#ifndef I2CPRIV_H_
#define I2CPRIV_H_

#include "timebase.h"
#include "timer.h"

#if 0
typedef u32 i2creg;
#define I2CONSET    *(i2creg*)0xE001C000
#define I2STAT      *(i2creg*)0xE001C004
#define I2DAT       *(i2creg*)0xE001C008
#define I2ADR       *(i2creg*)0xE001C00C
#define I2SCLH      *(i2creg*)0xE001C010
#define I2SCLL      *(i2creg*)0xE001C014
#define I2CONCLR    *(i2creg*)0xE001C018
#else
#if 0
static  i2c_t       *i2c;
#define I2CONSET    i2c->I2CONSET 
#define I2STAT      i2c->I2STAT 
#define I2DAT       i2c->I2DAT 
#define I2ADR       i2c->I2ADR 
#define I2SCLH      i2c->I2SCLH 
#define I2SCLL      i2c->I2SCLL 
#define I2CONCLR    i2c->I2CONCLR 
#else
#define I2CONSET    I2C->I2CONSET 
#define I2STAT      I2C->I2STAT 
#define I2DAT       I2C->I2DAT 
#define I2ADR       I2C->I2ADR 
#define I2SCLH      I2C->I2SCLH 
#define I2SCLL      I2C->I2SCLL 
#define I2CONCLR    I2C->I2CONCLR 
#endif
#endif

#define IWRITE      0u      //!< I2C transfer direction
#define IREAD       1u      //!< I2C transfer direction

#define SCL 2U              //!< GPIO pin number for PINSEL
#define SDA 3U              //!< GPIO pin number for PINSEL
#define CLOCK_COUNT 9u      //!< number of clocks per byte
#define I2C_RATE    4u      //!< peripheral clock divide by for I2C bus

// Number Of Reries
// Number of retries = SystemClock in MHz * 10; at least 8.
#define ONE_MILLION_CYCLES                  (1000000u)
#define COMPUTED_RETRIES                    ((PCLKFREQ * 10u)  / ONE_MILLION_CYCLES)
#define MINIMUM_RETRIES                     (8)
#define NUMBER_OF_RETRIES_TO_START_I2C      MAX(COMPUTED_RETRIES,MINIMUM_RETRIES)

//#define F0 0
#define F1 1U               //!< PINSEL I2C function
//#define F2 2
#define F3 3U               //!< PINSEL MASK for GPIO function

#define PS(x,y) ((y) << ((x) << 1)) //!< PINSEL macro
#define PB(x) (1U << (x))           //!< port bit macro

/** \brief Put a slave address or a data byte to the I2C device.  Pass expected status to

    \param[in] byte - the data or command byte to put
    \return u8 - the status after sending the byte to the device
*/
extern u8 I2C_PutByte(u8 byte);

/** \brief Get (read) a data byte from the I2C device

    \param[in] status - the expected status
    \return the requested data byte
*/
extern u8 I2C_GetByte(void);

/** \brief Wait for the device to complete an operation and return the controller status

    \return u8 - the status after the current operation completes
*/
MN_INLINE u8 I2C_WaitStatus(void)
{
    u8 result;

    // Note: After the SI flag has been cleared, the statis I2STAT becomes AAF8 and then, upon
    // completion of a state machine transaction, becomes something else.
    // The better way would have been to wait for SI flag to be set (transition completed) and
    // then analyze the status, but this code works. The functionality has been confirmed :
    // JTAG ICE debug and the chip manual.
    // The current implementation does not utilize the SI flag due to
    // slow execution speed of the particular platform.

    do
    {
        result = I2STAT;
    } while (result == AAF8);
    return result;
}



/** \brief Send a 'start' followed by the slave address with command (R/W)

    \param slaveaddress - the command byte to the device (already OR-ed with the Read/Write command)
    \return error code  - ERR_OK if command byte accepted and status OK, otherwise an error code
*/

MN_INLINE ErrorCode_t I2C_Start(u8 slaveaddr)
{
    ErrorCode_t ret;
    u32         NumberOfRetries;
    
    I2C_BUS_ENABLE(slaveaddr);          // if unit > 0 turn on the extended I2C bus on the DLT
    I2CONSET = STA;                     // request the bus

    NumberOfRetries = 0;                // Start from 0
    ret = ERR_I2C_BUS_STUCK;

    do {
        if (I2STAT == AA08)
        {
            I2CONCLR = STA;                             // turn off start
            ret = (I2C_PutByte(slaveaddr) == AA18)       // Status AA18 - Slave Address successfully sent   
                ? ERR_OK : ERR_I2C_NO_DEVICE;            // send slave address
            break;
        }            

        NumberOfRetries++;

    } while (NumberOfRetries < NUMBER_OF_RETRIES_TO_START_I2C);
    
    return ret;
}

/** \brief Send a 'restart' followed by the slave address and command (R/W)

  \param[in] slaveaddress - the command byte to the device
*/
MN_INLINE void I2C_Restart(u8 slaveaddress)
{
    I2CONSET = STA;                // request 'restart'
    I2CONCLR = SI;                 // free controller state machine
    (void)I2C_WaitStatus();             // wait for restart complete
    I2CONCLR = STA;                // turn off start
    (void)I2C_PutByte(slaveaddress);    // send slave address
}

/** \brief Stop the I2C controller
*/
MN_INLINE void I2C_Stop(void)
{
    u8 result;

    I2CONSET = STO;                // set stop
    I2CONCLR = SI;                 // free state machine

    do {                            // Wait for STOP condition to take effect
        result = I2STAT;
    } while (result != AAF8);

    // Once the status IS 0xF8 - we done. No SI will be generated

    I2C_BUS_DISABLE();             // turn off the extended I2C bus on the DLT
}

/** \brief Set the ACK bit signaling stream transfer
*/
MN_INLINE void I2C_SetAck(void)
{
    I2CONSET = AA;                 // set ack bit
}

/** \brief Reset the ACK bit signaling the last byte transferred
*/
MN_INLINE void I2C_ResetAck(void)
{
    I2CONCLR = AA;                 // clear ack bit
}

/** \brief Take the I2C port pins out of GPIO mode and put them in I2C mode
*/
MN_INLINE void I2C_Enable(void)
{
    PINSEL0 |= PS(SCL, F1) | PS(SDA, F1);
}

/** \brief stop the I2C controller and go idle
*/
MN_INLINE void I2C_Disable(void)
{
    I2C_Stop();
}


#endif	//I2CPRIV_H_

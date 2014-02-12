/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup FRAM  Non-volatile FRAM device
\brief Device driver for non-volatile FRAM device - includes I2C driver
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_FRAM_Module.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_BIOS.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file i2c.c
    \brief Generic device driver for I2C devices

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX/i2c.c $
    $Date: 1/16/12 6:15p $
    $Revision: 1 $
    $Author: Anatoly Podpaly $

    \ingroup I2C
*/

/* $History: i2c.c $
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 1/16/12    Time: 6:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * Moved from Generic BIOS layer
 * 
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 8/20/10    Time: 4:22p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Cleaning and preparation for Module packaging.
 * 
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 12/16/09   Time: 4:17p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Bug #2267: moved include of Timer and timebase headers into i2cpriv.h
 * 
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 12/09/09   Time: 7:54p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Added history and two includes for i2cpriv.h.
 * 
*/


#include "mnwrap.h" //for intrinsic functions used by the OS port
#include BIOS_DEF_H_
#include "errcodes.h"
#include "lpc2114io.h"
#include "i2cpriv.h"
#include "i2c.h"

/** \brief Initialize the I2C controller.

*/
void i2c_Initialize(void)
{
    I2CONCLR = I2EN | SI;                  // disable I2C and dump old status

    // the following two lines are redundant unless the default clocking is changed
    //  elsewhere
    I2SCLH = I2C_RATE;                     // check SCL at VPBDIV 1
    I2SCLL = I2C_RATE;                     //

    I2CONSET = I2EN;                       // enable
}

/** \brief Put a slave address or a data byte to the I2C device.  Pass expected status to

    \param[in] byte - the data or command byte to put
    \return u8 - the status after sending the byte to the device
*/
u8 I2C_PutByte(u8 byte)
{
    I2DAT    = byte;               // store the byte of data
    I2CONCLR = SI;                 // free controller state machine
    return I2C_WaitStatus();
}

/** \brief Get (read) a data byte from the I2C device

    \param[in] status - the expected status
    \return the requested data byte
*/
u8 I2C_GetByte(void)
{
    I2CONCLR = SI;                 // free controller state machine
    (void)I2C_WaitStatus();             // wait for data
    return (u8)I2DAT;              // get the data
}


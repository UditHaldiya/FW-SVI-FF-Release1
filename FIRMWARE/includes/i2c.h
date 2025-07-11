/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file i2c.h
    \brief Header for i2c controller

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/i2c.h $
    $Date: 1/17/12 5:47p $
    $Revision: 1 $
    $Author: Anatoly Podpaly $

    \ingroup I2C
*/

/* $History: i2c.h $
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 1/17/12    Time: 5:47p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Added - I2C moved from generic area of the Framework to project -
 * specific.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 12/16/09   Time: 3:43p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Bug #2264: added history.
 *
*/


#ifndef _I2C_H
#define _I2C_H
extern ErrorCode_t  i2c_Initialize(void);

#endif // _I2C_H

/* This line marks the end of the source */

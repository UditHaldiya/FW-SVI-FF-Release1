/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file refvoltage.h
    \brief Public interfaces of the reference voltage handling

    CPU: Any

    $Revision: 4 $
    OWNER: AP
    $Archive: /MNCB/Dev/FIRMWARE/includes/refvoltage.h $
    $Date: 7/20/12 12:56p $
    $Revision: 1 $
    $Author: Ericj $

*/
/* $History: refvoltage.h $
 *
 * *****************  Version 1  *****************
 * User: EricJ Date: 7/20/12    Time: 5:15p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * reference voltage handling Headers.
 *
*/
#ifndef REFVOLTAGE_H_
#define REFVOLTAGE_H_

typedef struct RefVoltage_t
{
    u32 HiLimit;
    u32 LoLimit;
    u16 CheckWord;
} RefVoltage_t;


/** Public functions
 *
*/
ErrorCode_t refvoltage_SetData(const RefVoltage_t* pRefVoltage);
const RefVoltage_t* refvoltage_GetData(RefVoltage_t *dst);

UNTYPEACC(refvoltage_SetData, refvoltage_GetData);


#endif // REFVOLTAGE_H_
/* This line marks the end of the source */

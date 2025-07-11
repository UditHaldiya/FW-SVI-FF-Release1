/**
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ds2401.h
    \brief header for BIOS routines to read the DS2401 1-Wire Serial Nunber device

    CPU: NXP LPC2114 LPC2124

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc/ds2401.h $
    $Date: 6/15/10 2:35p $
    $Revision: 1 $
    $Author: Anatoly Podpaly $
*/
#ifndef DS2401_H_
#define DS2401_H_

extern bool_t DS2401Read(u64 *serial);

#endif // DS2401_H_

/* End of the source */

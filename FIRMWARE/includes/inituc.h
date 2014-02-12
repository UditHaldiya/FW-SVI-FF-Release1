/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file inituc.h
    \brief header BIOS routines for initializing the microprocessor

    CPU: Philips LPC2114

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/

/*
    $History: inituc.h $
 *
 * *****************  Version 14  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 6:56p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/hal/bios/inc
 * -- AP -- LINT change - div is reserved
 *
 *

*/


#ifndef INITUC_H_
#define INITUC_H_
#include "sysproj.h"

extern void bios_InitTimer1(void);

#endif //INITUC_H_

/* End of the source */

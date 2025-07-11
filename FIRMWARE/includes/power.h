/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file power.h
    \brief Contains functions for checking power lecewls for safe operation

    CPU: Any (with standard C compiler)

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/
#ifndef CALIB_H_
#define CALIB_H_

#define power_OKtoWriteFRAM power_IsOK

bool_t  power_IsOK(void);
void    power_CheckClearAssert(void);
#endif

/* end of the source */

/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file power.c
    \brief Power handling module

     CPU: Any

    OWNER: EP
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/

#include "mnwrap.h"
// #include "adtype.h"
// #include "ad.h"
#include "mnassert.h"
#include "power.h"


/**
  \brief  This function resets the assert record if the power is currently
  below the operating threshold.
  \param[in] none
  \return - none
*/
void power_CheckClearAssert(void)
{
    if (!power_IsOK())
    {
       mn_ClearAssert();
    }
}

/* end of the source */


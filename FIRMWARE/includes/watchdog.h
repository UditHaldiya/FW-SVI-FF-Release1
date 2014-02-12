/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file watchdog.h
    \brief Header for internal watchdog device driver (and in-place resets)

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/FIRMWARE/includes/watchdog.h $
    $Date: 8/07/07 7:14p $
    $Revision: 7 $
    $Author: Arkkhasin $

    \ingroup Watchdog
*/
#ifndef  WATCHDOG_H_
#define  WATCHDOG_H_

#include "config.h"
extern void     wdog_HardwareReset(void);
extern void     wdog_WatchdogTrigger(void);
extern void     wdog_WaitExternalWatchdog(void);

#if USE_INTERNAL_WATCHDOG == OPTION_ON
 extern void    wdog_WatchdogInit(void);
#endif

#endif // WATCHDOG_H_

/* This line marks the end of the source */

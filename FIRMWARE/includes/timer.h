/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file timer.h
    \brief Header for timer tick device driver (5 millisecond)

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/timer.h $
    $Date: 8/18/10 6:27p $
    $Revision: 2 $
    $Author: Anatoly Podpaly $

    \ingroup Timer
*/
#ifndef TIMER_H_
#define TIMER_H_

extern u32      bios_GetTimer0Ticker      (void);
extern u64      bios_GetTimer0TickerLong  (void);
extern void     bios_WaitTicks            (u32 ticks);
extern void     timer_IntegrityCheck      (void);

/** \brief An init-time profiler of a function.
Only valid before init HW is programmed differently and before
RTOS starts
*/
extern u32 timer_EstimateFunctionLength(void (*func)(void));


MN_DECLARE_API_FUNC(bios_ProcessTick) // called by bsp_a.s79

#ifndef OLD_OSPORT
extern void     bios_ProcessTick          (void);
#endif

#endif //TIMER_H_

/* This line marks the end of the source */

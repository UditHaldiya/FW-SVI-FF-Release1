/*
Copyright 2005-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file timesrv.c
    \brief Misc. timer-related services

    CPU: Philips/NXP LPC21x4

    OWNER: AK
*/
#include "mnwrap.h"
#include "timer.h"
#include "lpc2114io.h"

//doc in header
u32 timer_EstimateFunctionLength(void (*func)(void))
{
    u32 teststart = T1->TC;
    func();
    return T1->TC - teststart;
}
/* This line marks the end of the source */

/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file fullthrottle.h
    \brief Header for fullthrottle.c -- Full Open/Close controller

    CPU:

    OWNER: SK

    $Archive: $
    $Date:  $
    $Revision:  $
    $Author:  $

    \ingroup
*/

/* $History:  $
 *
 *
*/


#ifndef FULLTHROTTLE_H_
#define FULLTHROTTLE_H_

extern bool_t full_GetFullThrottleEnabled(void);
extern bool_t full_BypassControl(void);
extern void full_FullThrottle(void);

#endif // FULLTHROTTLE_H_

/* This line marks the end of the source */

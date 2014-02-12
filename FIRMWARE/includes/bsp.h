/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file bsp.h
    \brief Header file for Board Support Package, processor init, VIC and Timer setup

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/FIRMWARE/includes/bsp.h $
    $Date: 7/25/08 7:40p $
    $Revision: 27 $
    $Author: Arkkhasin $

    \ingroup BSP
*/
#ifndef BSP_H_
#define BSP_H_

#include "sysproj.h"
//lint -esym(526,mychecksum)  mychecksum is in bsp_a.s79
extern const u16      mychecksum;
extern void     bsp_StartTicks(void);

#endif // BSP_H_

/* This line marks the end of the source */

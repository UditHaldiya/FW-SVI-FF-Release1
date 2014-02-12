/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file da.h
    \brief Header for position retransmit driver

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/FIRMWARE/includes/da.h $
    $Date: 9/04/08 1:33a $
    $Revision: 10 $
    $Author: Arkkhasin $

    \ingroup D_to_A
*/
#ifndef _DA_H
#define _DA_H

extern void bios_InitDa(void);
extern void bios_WriteDa(const u16 value);
extern u16 bios_GetLastDa(void);

#endif

/* This line marks the end of the source */

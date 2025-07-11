/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mncbdefs_ffap.h
    \brief Project-specific companion to mncbdefs.h

    The first entries are for noise band.

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_FFAP/mncbdefs_ffap.h $
    $Date: 04/01/13 1:50p $
    $Revision: 1 $
    $Author: EricJiang $

    \ingroup proj
*/

#ifndef MNCBDEFS_FFAP_H_
#define MNCBDEFS_FFAP_H_

#include "mncbdefs_proj.h"

// Those defines can be reused by firmware download module
#define APP_HW_REV  MNCB_HW_REV
#define APP_SW_REV  SW_REV
#define APP_TXSEPC_REV TXSPEC_REV
#define APP_FW_TYPE FIRMWARE_TYPE

#endif //MNCBDEFS_FFAP_H_
/* This line marks the end of the source */

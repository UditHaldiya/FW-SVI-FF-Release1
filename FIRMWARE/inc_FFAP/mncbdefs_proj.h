/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mncbdefs_proj.h
    \brief Project-specific companion to mncbdefs.h

    CPU: Any

    \ingroup proj
*/
#ifndef MNCBDEFS_PROJ_H_
#define MNCBDEFS_PROJ_H_

#include "asciicodes.h"

#define MNCB_MANUFACTURER_DEVICE_CODE   0x03 //0xfe //202U //! HART device id (AP)

#define MNCB_MAGIC_NUMBER ((ASCII_F<<8)|ASCII_F) //!Device type FRAM id

#define MNCB_HW_REV 1U
#define SW_REV 1U //TBD
#define TXSPEC_REV 1U

#define FIRMWARE_TYPE 3 //FF AP

#define NVRAM_VERSION 8 //Manually maintained. Absolutely must be kept current

//lint -emacro((755),MNCB_VERSION_NUMBER_) "not referenced" - Used by NVMEMDUMP build target
//lint -emacro({755},MNCB_VERSION_NUMBER_) "not referenced" - Used by NVMEMDUMP build target
//lint -esym(755,MNCB_VERSION_NUMBER_) "not referenced" - Used by NVMEMDUMP build target
#define MNCB_VERSION_NUMBER_ NVRAM_VERSION

#endif //MNCBDEFS_PROJ_H_
/* This line marks the end of the source */

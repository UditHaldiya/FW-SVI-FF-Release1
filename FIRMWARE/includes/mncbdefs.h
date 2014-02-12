/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file mncbdefs.h
    \brief Global defines for hardware behavior

	The first entries are for noise band.

    CPU: Any

    OWNER: EP
    \n RCS: $Id: $
     $DateTime: 19-AUG-2004$ $Change: $ $Author: Arkkhasin $
*/
/* This is optional and is pruned at sole discretion of the maintainer
   HISTORY:
*/

#ifndef MNCBDEFS_H_
#define MNCBDEFS_H_

#include "mncbdefs_proj.h"

#define MNCB_MANUFACTURER_ID            101U

#define MNCB_HARDWARE_REVISION            (MNCB_HW_REV<<3)

#define MNCB_VERSION_NUMBER ((u32)NVRAM_VERSION) //from nvramdef.h
#define MNCB_NVRAM_SIGNATURE ((u32)((MNCB_MAGIC_NUMBER<<16)|MNCB_VERSION_NUMBER)) //do not change

#define MNCB_DEFAULT_DATE_STRING "IDEBLD"  //! a dummy signature; in command-line build, will be replaced by post-link processing
typedef struct VerInfo_t
{
    u8 signature;
    u8 sw_revision;
    u8 trans_spec_revision;
    u8 hw_revision;
    u8 fw_type;
    u8 date_str[sizeof(MNCB_DEFAULT_DATE_STRING) - 1U];
    u8 num_nvm_volumes;
    u8 nvram_version[0+1]; //MAX_VOLID+1
    u8 ManufacturerDeviceCode;
} VerInfo_t;
extern const VerInfo_t VerString[]; //array because a device may now have multiple personalities
extern const s16 VerNumber[];

#endif //MNCBDEFS_H_

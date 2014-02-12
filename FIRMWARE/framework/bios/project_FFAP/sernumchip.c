/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file sernumchip.c
    \brief BIOS routines for Serial Number chip (DS2401 type).

    CPU: Any (with standard C compiler)

    OWNER: AP
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX/sernumchip.c $
    $Date: 11/04/11 4:00p $
    $Revision: 16 $
    $Author: Arkkhasin $
*/

//--------------------------------------------------------------------------------------------

#include "mnwrap.h"
#include "oswrap.h"
#include "mnassert.h"

#include "sernumchip.h"

//--------------------------------------------------------------------------------------------
// The code is using internal Unique ID register in the microcontroller
// The value in register is 96 bit, only ALL bits are utilized.
// 95..64 - EHI_32      -- All 32 bits are available
// 63..32 - HI_32       -- All 32 bits are available
// 31..16 - Reserved_16 -- This 16-bit field is reserved and not used
// 15..0  - LO_16       -- All 16-bit are used
//
// The resulting 64-bit serial number is assembled as shown
//   79..64 63..32 15..0
//--------------------------------------------------------------------------------------------

#define     SERIALNUM_UNIQUE_ID_BASE        (0x1FFFF7E8u)

#define     UNIQUE_ID_LO_16                 HARDWARE(u16 *, SERIALNUM_UNIQUE_ID_BASE)
// #define     UNIQUE_ID_RESERVED_16           HARDWARE(u16 *, SERIALNUM_UNIQUE_ID_BASE + 0x02u)
#define     UNIQUE_ID_RESERVED_HI_32        HARDWARE(u32 *, SERIALNUM_UNIQUE_ID_BASE + 0x04u)
#define     UNIQUE_ID_RESERVED_EHI_32       HARDWARE(u32 *, SERIALNUM_UNIQUE_ID_BASE + 0x08u)

#define     LO_U16_WORD_MASK                (0xFFFFu)

//--------------------------------------------------------------------------------------------

/** \brief Onboard Serialization code
    This function reads the Serial Number

    The function performs the following:
    - Reads the Serial Number from the Internal register in the microcontroller
    - The function returns SERIALNUMBER_OK and 6 bytes of the Serial Number in 64-bit value.
    If the data are NOT valid, the function returns FALSE and 0 as a Serial Number in 64-bit value.

  \param[in] u64    *ptrSerialNumber - pointer to the 64-bit Serial Numbercontainer
  \return SerialNumberError_t --      SERIALNUMBER_OK

*/

static u64  LocalSerialNumber;

SerialNumberError_t bios_ReadSerialization(u64 *ptrSerialNumber)
{
    u64     ReadRegL;
    u64     ReadRegM;
    u64     ReadRegH;

    // Check and Clear the receiver
    MN_ASSERT(ptrSerialNumber != NULL);

    LocalSerialNumber = (u64)0;                         // Clear the variable - buffer

    ReadRegL   = (u64)*UNIQUE_ID_LO_16;
    ReadRegM   = (u64)*UNIQUE_ID_RESERVED_HI_32;
    ReadRegM <<= 16u;
    ReadRegH   = ((u64)*UNIQUE_ID_RESERVED_EHI_32 & LO_U16_WORD_MASK);
    ReadRegH <<= 48u;

    LocalSerialNumber = ReadRegL | ReadRegM | ReadRegH;

    *ptrSerialNumber = LocalSerialNumber;

    // This function does not use the following Error codes
    // that are declared in Generic Serial Number support code
    UNUSED_OK(SERIALNUMBER_NODEVICE);                   //!< SN chip not present
    UNUSED_OK(SERIALNUMBER_WRONGDEVICE);                //!< SN Chip is incorrect (not the specified family)
    UNUSED_OK(SERIALNUMBER_BADCRC);                     //!< SN Data CRC bad

    return SERIALNUMBER_OK;
}

/* End of the source */
//--------------------------------------------------------------------------------------------

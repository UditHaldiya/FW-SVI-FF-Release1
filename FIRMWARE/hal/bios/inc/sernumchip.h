/**
Copyright 2004-2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file sernumchip.h
    \brief BIOS routines and definitions for Serial Number chip (DS2401 type) driver.

    CPU: Any (with standard C compiler)

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc/sernumchip.h $
    $Date: 10/22/10 1:04p $
    $Revision: 3 $
    $Author: Anatoly Podpaly $
*/

/* $History: sernumchip.h $
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 10/22/10   Time: 1:04p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Fixed headers.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 10/13/10   Time: 2:16p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug 4281 - corrected the Header file.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 6/15/10    Time: 2:36p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Onboard Serialization Support: header file: API and error codes.
   *
   */

#ifndef SERNUM_H_
#define SERNUM_H_

/*******************************************************************************
*** data type definitions
*******************************************************************************/

//-----------------------------------
//!< SN Driver Error codes

typedef enum
{
    SERIALNUMBER_OK,                    //!< OK
    SERIALNUMBER_NODEVICE,              //!< SN chip not present
    SERIALNUMBER_WRONGDEVICE,           //!< SN Chip is incorrect (not the specified family)
    SERIALNUMBER_BADCRC                 //!< SN Data CRC bad

} SerialNumberError_t;

/** \brief SN chip API
    This function reads the Serial Number from the 1-wire device.

  \param[out] u64    *ptrSerialNumber - pointer to the 64-bit Serial Number container,
                                        the SN will be placed into that container

  \return SerialNumberError_t       - Error code (See above)
*/

extern  SerialNumberError_t bios_ReadSerialization(u64 *ptrSerialNumber);

#endif  // SERNUM_H_
/* This line marks the end of the source */

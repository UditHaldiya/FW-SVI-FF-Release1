/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file snchippriv.h
    \brief BIOS routines for Serial Number chip (DS2401 type). 

    CPU: Any (with standard C compiler)

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX/snchippriv.h $
    $Date: 10/25/10 6:53p $
    $Revision: 1 $
    $Author: Anatoly Podpaly $
*/

/* (Optional) $History: snchippriv.h $
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 10/25/10   Time: 6:53p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * New Private Header file for the Serial NUmber chip
   *
   */

#ifndef SNCHIPPRIV_H_
#define SNCHIPPRIV_H_

//--------------------------------------------------------------------------------------------
// DS2401 definitions...

#define SERIALNUM_FAMILY_INDEX      (0u)
#define SERIALNUM_FAMILY_SIZE       (1u)
// #define SERIALNUM_SN_INDEX          (1u)
#define SERIALNUM_SN_SIZE           (6u)
#define SERIALNUM_CRC_INDEX         (7u)
#define SERIALNUM_CRC_SIZE          (1u)
#define SERIALNUM_DATA_TOTAL_SIZE   (SERIALNUM_SN_SIZE + SERIALNUM_CRC_SIZE + SERIALNUM_FAMILY_SIZE)
#define SERIALNUM_FAMILY_CODE       (0x01)
#define SERIALNUM_READ_ROM_CMD      (0x33)

#define BYTE_BITS                   (8u)
#define U8_BITS                     (sizeof(u8) * BYTE_BITS)          

//--------------------------------------------------------------------------------------------
// OW protocol
//
// Write 1  -           Drive Bus LOW, Delay A, release Bus, Delay B
// Write 0  -           Drive Bus LOW, Delay C, release Bus, Delay D
// Read     -           Drive Bus LOW, Delay A, release Bus, Delay E, sample Bus, Delay F
// Reset    - Delay G,  Drive Bus LOW, Delay H, release Bus, Delay I, sample Bus 
// 
// Presense bit - Sampled at the end of Reset: 0 - device present, 1 - No device.
//
// Delay Values (uS)
// Delay Value      Standard    Overdrive
//     A               6            1.0
//     B              64            7.5
//     C              60            7.5
//     D              10            2.5
//     E               9            1.0
//     F              55            7.0
//     G               0            2.5
//     H             480           70.0
//     I              70            8.5
//     J             410           40.0
// As we are executing on the chip with incredibly low clock
// precise timing becomes difficult and requires a bit of scary codng tactics
//
// Below are the timings in uS for 1-wire protocol

#define     DELAY_VALUE_A           (6u)
#define     DELAY_VALUE_B          (64u)
#define     DELAY_VALUE_C          (60u)
#define     DELAY_VALUE_D          (10u)
#define     DELAY_VALUE_E           (9u)
#define     DELAY_VALUE_F          (55u)
#define     DELAY_VALUE_G           (0u)
#define     DELAY_VALUE_H         (480u)
#define     DELAY_VALUE_I          (70u)
#define     DELAY_VALUE_J         (410u)

// These are simplified alternative timing values for use in LOW speed system
// #define     DELAY_VALUE_A          (10u)
// #define     DELAY_VALUE_B          (70u)
// #define     DELAY_VALUE_C          (60u)
// #define     DELAY_VALUE_D          (10u)
// #define     DELAY_VALUE_E          (10u)
// #define     DELAY_VALUE_F          (70u)
// #define     DELAY_VALUE_G           (0u)
// #define     DELAY_VALUE_H         (480u)
// #define     DELAY_VALUE_I          (70u)
// #define     DELAY_VALUE_J         (480u)

#endif      // SNCHIPPRIV_H_
 
/* End of the source */
//--------------------------------------------------------------------------------------------

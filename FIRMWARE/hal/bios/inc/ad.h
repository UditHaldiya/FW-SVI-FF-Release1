/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ad.h
    \brief header for BIOS A/Droutines

    CPU: NXP LPC2114 LPC2124

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/hal/bios/inc/ad.h $
    $Date: 11/20/11 11:47p $
    $Revision: 11 $
    $Author: Arkkhasin $
*/

/* $History: ad.h $
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 11/20/11   Time: 11:47p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/hal/bios/inc
 * TFS:8255 - A/D modularization
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 3:54p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/hal/bios/inc
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 8  *****************
 * User: Anatoly Podpaly Date: 7/07/11    Time: 12:28p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Reverted to the revision 6. Rolled resolution for the TFS:6817 back.
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 11/09/10   Time: 9:47a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug 4596 - removed redundantly declared symbol 'bios_MeasureAd(void)'
 * of type 'void (void)' previously declared at line 4.
 *
 * Corrected headers.
 *
 *
*/

#ifndef AD_H_
#define AD_H_

#include "adtype.h"

//AK:TODO: make them strong types
typedef u16 ADRaw_t; //! The type of MNCB A/D converter output
//FUTURE (?) typedef u16_least ADRaw_fast_t; //! The fast type version for ADRaw_t

typedef s32 ADRaw32_t; //! The type for 32-bit A/D reads
//lint -esym(756, ADRaw32_t) a project may not need/have a 32-bit converter

extern void bios_WriteMeasureSequence(const measure_sequence_t *m_arr);
extern const AdDataRaw_t *bios_GetAdRawData(void);

extern void bios_InitAd(void);
extern bool_t bios_MeasureAd(void);

extern void (* const ad_UpdateFunc[])(ADRaw_t advalue); //required table

#endif //AD_H_

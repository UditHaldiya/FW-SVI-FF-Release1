/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup HARTapp
\brief HART application layer component

\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_Hart Module.doc"> Design document </a><br>
Unit Test Credit claim is found <a href="../../../Doc/TestDocs/UnitTests.doc"> here </a><br>
\endhtmlonly
*/
/**
    \file hartfunc.c
    \brief The functions used by each HART command: the HART Functions Module

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hartcmd220_221_mncb.c $
    $Date: 12/07/11 12:59p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hartcmd220_221_mncb.c $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 12:59p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 2:14p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 3/30/11    Time: 1:39p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Reverted to old API for old fixed-length commands
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 3:36p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Added inputs pressure feature PILOT_ONLY
 * Pressure handling is split into individual channels
 * hartcmd220_221_mncb.c is now project-specific
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/04/08    Time: 1:24a
 * Created in $/MNCB/Dev/FIRMWARE/interface/hart
 * (Preliminary) implementations of HART commands 220 and 221
*/
#include "mnwrap.h"
#include "projectdef.h"
#include "ad.h"
#include "tempcomp.h"

#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#include "insignal.h"
#endif
#if FEATURE_PVINPUT_SENSOR == FEATURE_PVINPUT_SENSOR_AVAILABLE
#include "inpv.h"
#endif

#include "position.h"
#include "posint.h"
#include "posext.h"
#include "tempr.h"
#include "tempr_mncb.h"
#include "ipcurr.h"

#if (FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED)
#include "pressures.h"
#include "prestab.h"
#endif

#include "hartfunc.h"
#include "hart.h"
#include "hartdef.h"
#include "bufutils.h"

#include "adtbl.h"

static tcorr_t dummy_TempComp(s32 value)
{
    return (tcorr_t)value;
}

typedef struct RawRead_t
{
    u8 offset;
    tcorr_t (*tempcomp)(s32 adraw);
} RawRead_t;

static const RawRead_t RawReadTable[] =
{
#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
    [AD_AI_SP] = {MN_OFFSETOF(RawValues_t, ADsp), sig_TempComp},
#else
    [AD_AI_SP] = {MN_OFFSETOF(RawValues_t, ADsp), dummy_TempComp},
#endif
#if FEATURE_PVINPUT_SENSOR == FEATURE_PVINPUT_SENSOR_AVAILABLE
    [AD_AI_PV] = {MN_OFFSETOF(RawValues_t, ADpv), inpv_TempComp},
#else
    [AD_AI_PV] = {MN_OFFSETOF(RawValues_t, ADpv), dummy_TempComp},
#endif
    [AD_TEMP_INT] = {MN_OFFSETOF(RawValues_t, ADtempr), tempr_TempComp},
#if FEATURE_PRESSURE_SENSOR_1 == FEATURE_PRESSURE_SENSOR_1_SUPPORTED
    [AD_PRESS_1] = {MN_OFFSETOF(RawValues_t, ADpres1), pres1_TempComp},
#else
    [AD_PRESS_1] = {MN_OFFSETOF(RawValues_t, ADpres1), dummy_TempComp},
#endif

#if FEATURE_PRESSURE_SENSOR_ATM == FEATURE_PRESSURE_SENSOR_ATM_SUPPORTED
    [AD_PRESS_ATMOS] = {MN_OFFSETOF(RawValues_t, ADpresAtm), presatm_TempComp},
#else
    [AD_PRESS_ATMOS] = {MN_OFFSETOF(RawValues_t, ADpresAtm), dummy_TempComp},
#endif
#if FEATURE_PRESSURE_SENSOR_3 == FEATURE_PRESSURE_SENSOR_3_SUPPORTED
    [AD_PRESS_3] = {MN_OFFSETOF(RawValues_t, ADpres3), pres3_TempComp},
#else
    [AD_PRESS_3] = {MN_OFFSETOF(RawValues_t, ADpres3), dummy_TempComp},
#endif
#if FEATURE_PRESSURE_SENSOR_2 == FEATURE_PRESSURE_SENSOR_2_SUPPORTED
    [AD_PRESS_2] = {MN_OFFSETOF(RawValues_t, ADpres2), pres2_TempComp},
#else
    [AD_PRESS_2] = {MN_OFFSETOF(RawValues_t, ADpres2), dummy_TempComp},
#endif
#if FEATURE_REMOTE_POSITION_SENSOR == FEATURE_REMOTE_POSITION_SENSOR_AVAILABLE
    [AD_POS_REM] = {MN_OFFSETOF(RawValues_t, ADposrem), posext_TempComp},
#else
    [AD_POS_REM] = {MN_OFFSETOF(RawValues_t, ADposrem), dummy_TempComp},
#endif
    [AD_POS_INT] = {MN_OFFSETOF(RawValues_t, ADposint), posint_TempComp},
#if FEATURE_PRESSURE_SENSOR_PILOT == FEATURE_PRESSURE_SENSOR_PILOT_SUPPORTED
    [AD_PRESS_PILOT] = {MN_OFFSETOF(RawValues_t, ADpresPilot), prespilot_TempComp},
#else
    [AD_PRESS_PILOT] = {MN_OFFSETOF(RawValues_t, ADpresPilot), dummy_TempComp},
#endif
    [AD_IP_CUR] = {MN_OFFSETOF(RawValues_t, ADIP), ipcurr_TempComp},
    [AD_DIAG] = {MN_OFFSETOF(RawValues_t, ADrefV), dummy_TempComp}, //ref voltage is not temp-corrected
};
CONST_ASSERT(NELEM(RawReadTable) == (sizeof(RawValues_t)/sizeof(u16)));
CONST_ASSERT(NELEM(RawReadTable) == (sizeof(TemperatureCorrectedValues_t)/sizeof(u16)));

CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADsp) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCsp));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADpv) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCpv));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADtempr) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCtempr));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADpres1) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCpres1));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADpresAtm) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCpresAtm));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADpres3) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCpres3));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADpres2) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCpres2));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADposrem) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCposrem));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADposint) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCposint));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADpresPilot) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCpresPilot));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADIP) == MN_OFFSETOF(TemperatureCorrectedValues_t, TCIP));
CONST_ASSERT(MN_OFFSETOF(RawValues_t, ADrefV) == MN_OFFSETOF(TemperatureCorrectedValues_t, ADrefV));


/**
\brief Returns raw A/D values with no correction

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadRawValues(const u8 *src, u8 *dst)
{
    //   {220,sizeof(struct ReadADCRawData_resp)-sizeof(u8),sizeof(struct ReadADCRawData_req)-sizeof(u8),0},
    //const AdDataRaw_t *tmpptr;
    u8_least i;

    UNUSED_OK(src);
    Rsp_ReadRawValues_t *d1 = (void *)dst;
    u8 *d = d1->RawValues[0];

    const AdDataRaw_t * tmpptr = bios_GetAdRawData(); /* read data from bios */

    /* copy all data to hart */
    for(i=0; i<NELEM(RawReadTable); ++i)
    {
        util_PutU16(&d[RawReadTable[i].offset], (u16)tmpptr->AdValue[i]);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/**
\brief Returns temperature corrected A/D values.
But only those which are currently sampled; the unsampled =SHRT_MIN

Notes:  Even though temperature corrections are done by other routines
        this routine will re-do them to make sure all channels are
        compensated for the response
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadTCorrectedValues(const u8 *src, u8 *dst)
{
    u8_least i;
    UNUSED_OK(src);
    Rsp_ReadTCorrectedValues_t *d1 = (void *)dst;
    u8 *d = d1->TemperatureCorrectedValues[0];

    const AdDataRaw_t *tmpptr = bios_GetAdRawData(); /* read data from bios */

    MSeq_t seqn = adtbl_GetActiveSequence();
    const u8 *seq = adtbl_seqTbl[seqn];

    /* copy all data to hart */
    for(i=0; i<NELEM(RawReadTable); ++i)
    {
        s16 val = SHRT_MIN; //a dummy for things not actually sampled
        //Is channel i sampled?
        for(const u8 *p=seq; *p < (END_SEQ_TWO+END_SEQ_ONE); p++)
        {
            if(*p == i)
            {
                //All MNCB tempcomp'ed ranges fit s16
                val = (s16)RawReadTable[i].tempcomp(tmpptr->AdValue[i]);
                break;
            }
        }
        util_PutS16(&d[RawReadTable[i].offset], val);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */

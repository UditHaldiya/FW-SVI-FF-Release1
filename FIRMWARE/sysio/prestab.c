/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file prestab.c
    \brief Project-specific pressures tempcomp

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/prestab.c $
    $Date: 12/07/11 12:59p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: prestab.c $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 12:59p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/14/11   Time: 11:52p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
*/
#include "projectdef.h"
#include "mnwrap.h"
#include "faultpublic.h"
#include "prestab.h"
#include "pressures.h"

#if FEATURE_PRESSURE_INPUTS != FEATURE_PRESSURE_INPUTS_INCLUDED
#error "This file requires pressure inputs"
#endif

#if FEATURE_PRESSURE_SENSOR_ATM == FEATURE_PRESSURE_SENSOR_ATM_SUPPORTED
#define PRES_ATM_MIN (-5.0) //psi
#define PRES_ATM_MAX (5.0) //psi
#define AD_MAX_VALUE (65534)
static s8_least pilot_error_count = 0;

TEMPCOMP_C_ENTRY(presatm) =
{
    .AllowedValue =
    {
        [Xlow] = STD_FROM_PSI(PRES_ATM_MIN),
        [Xhi] = STD_FROM_PSI(PRES_ATM_MAX),
    },
    .fcode = FAULT_PRESSURE5,
    .ErrLimit = ERRLIMIT_DEFAULT,
};
#endif

#if FEATURE_PRESSURE_SENSOR_1 == FEATURE_PRESSURE_SENSOR_1_SUPPORTED
TEMPCOMP_C_ENTRY(pres1) =
{
    .AllowedValue =
    {
        [Xlow] = STD_FROM_PSI(PRESSURE_MIN_PSI),
        [Xhi] = STD_FROM_PSI(PRESSURE_MAX_PSI),
    },
    .fcode = FAULT_PRESSURE1,
    .ErrLimit = ERRLIMIT_DEFAULT,
};
#endif

#if FEATURE_PRESSURE_SENSOR_2 == FEATURE_PRESSURE_SENSOR_2_SUPPORTED
TEMPCOMP_C_ENTRY(pres2) =
{
    .AllowedValue =
    {
        [Xlow] = STD_FROM_PSI(PRESSURE_MIN_PSI),
        [Xhi] = STD_FROM_PSI(PRESSURE_MAX_PSI),
    },
    .fcode = FAULT_PRESSURE2,
    .ErrLimit = ERRLIMIT_DEFAULT,
};
#endif

#if FEATURE_PRESSURE_SENSOR_3 == FEATURE_PRESSURE_SENSOR_3_SUPPORTED
TEMPCOMP_C_ENTRY(pres3) =
{
    .AllowedValue =
    {
        [Xlow] = STD_FROM_PSI(PRESSURE_MIN_PSI),
        [Xhi] = STD_FROM_PSI(PRESSURE_MAX_PSI),
    },
    .fcode = FAULT_PRESSURE3,
    .ErrLimit = ERRLIMIT_DEFAULT,
};
#endif

#if FEATURE_PRESSURE_SENSOR_PILOT == FEATURE_PRESSURE_SENSOR_PILOT_SUPPORTED
TEMPCOMP_C_ENTRY(prespilot) =
{
    .AllowedValue =
    {
        [Xlow] = STD_FROM_PSI(PRESSURE_MIN_PSI),
        [Xhi] = STD_FROM_PSI(100.0),
    },
    .fcode = FAULT_PRESSURE4,
    .ErrLimit = ERRLIMIT_DEFAULT,
};
#endif

/** \brief A simple wrapper of gory details of pressures tempcomp
\param adchannel - a valid pressure channel
\param adval - sensor read
\return tempcomp'ed pressure
*/
pres_least_t pres_CorrectForTemperature(s8 adchannel, ADRaw_t adval)
{
    pres_least_t pres;
    switch(adchannel)
    {
#if FEATURE_PRESSURE_SENSOR_ATM == FEATURE_PRESSURE_SENSOR_ATM_SUPPORTED
        case AD_PRESS_ATMOS:
            pres = presatm_TempComp(adval);
            //No correction on failure: let it rail for now; like in AP/LCX
            break;
#endif
#if FEATURE_PRESSURE_SENSOR_1 == FEATURE_PRESSURE_SENSOR_1_SUPPORTED
        case AD_PRESS_1:
            pres = pres1_TempComp(adval);
            if(error_IsFault(FAULT_PRESSURE1))
            {
                pres = PRESSURE_INVALID;
            }
            break;
#endif
#if FEATURE_PRESSURE_SENSOR_2 == FEATURE_PRESSURE_SENSOR_2_SUPPORTED
        case AD_PRESS_2:
            pres = pres2_TempComp(adval);
            if(error_IsFault(FAULT_PRESSURE2))
            {
                pres = PRESSURE_INVALID;
            }
            break;
#endif
#if FEATURE_PRESSURE_SENSOR_3 == FEATURE_PRESSURE_SENSOR_3_SUPPORTED
        case AD_PRESS_3:
            pres = pres3_TempComp(adval);
            if(error_IsFault(FAULT_PRESSURE1))
            {
                pres = PRESSURE_INVALID;
            }
            break;
#endif
#if FEATURE_PRESSURE_SENSOR_PILOT == FEATURE_PRESSURE_SENSOR_PILOT_SUPPORTED
        case AD_PRESS_PILOT:
            
            //because of the scaling of this channel and the variability of
            //  the components, the check must be made on the AD value instead
            //  of the tempcomp value.  If the AD is maxed out, generate the fault.
            pres = prespilot_TempComp(adval);
            if(adval >= AD_MAX_VALUE)
            {
                if(++pilot_error_count >= prespilot_TemprCorrConf.ErrLimit)
                {
                    error_SetFault(FAULT_PRESSURE4);
                }
            }
            else
            {
                pilot_error_count = 0;
            }

            if(error_IsFault(FAULT_PRESSURE4))
            {
                pres = PRESSURE_INVALID;
            }
            break;
        default:
            pres = PRESSURE_INVALID;
            break;
    }
#endif
    return pres;
}
/* This line marks the end of the source */

/*
Copyright 2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup ADTable  A/D sequence table module
\brief A/D sensor sequence table module
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_ADTable Module.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_adtable_reset.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file adtbl.c
    \brief A/D sensor sequence table

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/project_FFAP/adtbl.c $
    $Date: 1/30/12 3:01p $
    $Revision: 6 $
    $Author: Arkkhasin $

    \ingroup ADTable
*/
/* $History: adtbl.c $
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 3:01p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio/project_FFAP
 * Removed signal setpoint and loop signal data FBO TFS:8782
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 6:47p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio/project_SVI2AP
 * TFS:8255
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:21p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8313 Lint
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/20/11   Time: 11:27p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8255 - A/D channel sequence tables (preliminary)
*/
#include "projectdef.h"
#include "mnwrap.h"
#include "adtype.h"
#include "adtbl.h"
#include "adtblgen.h"

#include "tempr.h"
#include "tempr_mncb.h"
#include "ipcurr.h"
#include "posint.h"
#include "posext.h"
#include "oswrap.h"

#if FEATURE_PVINPUT_SENSOR == FEATURE_PVINPUT_SENSOR_AVAILABLE
#include "inpv.h"
#endif

// Normal sequence for Internal Position 8 slow channels
static const u8 measureSeqPosInt[] =
{
    ADTABLE(FAST_CHANNELS(AD_IP_CUR, AD_POS_INT),
            AD_PRESS_PILOT,
            AD_PRESS_1,
            AD_PRESS_2,
            AD_PRESS_3,
            AD_PRESS_ATMOS,
			AD_AI_PV,
            AD_TEMP_INT,
            AD_DIAG)
};

// Startup sequence - read every sensor once

// Startup helper sequence - read temperature sensor once
static const u8 measureSeqTempr[] =
{
    ADTABLE(FAST_CHANNELS(AD_TEMPR_INIT, AD_TEMP_INT),
            NO_SLOW_CHANNELS
                )
};

static const u8 measureSeqPosRem[] =
{
    ADTABLE(FAST_CHANNELS(AD_IP_CUR, AD_POS_REM),
            AD_PRESS_PILOT,
            AD_PRESS_1,
            AD_PRESS_2,
            AD_PRESS_3,
            AD_PRESS_ATMOS,
			AD_AI_PV,
            AD_TEMP_INT,
            AD_DIAG)
};

// Startup sequences - read every active sensor once
static const u8 measureSeqPosInt_Init[] =
{
    ADTABLE(FAST_CHANNELS(
            AD_IP_CUR,
            AD_POS_INT,
            AD_PRESS_PILOT,
            AD_PRESS_1,
            AD_PRESS_2,
            AD_PRESS_3,
            AD_PRESS_ATMOS,
			AD_AI_PV,
            AD_TEMP_INT,
            AD_DIAG
            ),
            NO_SLOW_CHANNELS
                )
};
static const u8 measureSeqPosExt_Init[] =
{
    ADTABLE(FAST_CHANNELS(
            AD_IP_CUR,
            AD_POS_REM,
            AD_PRESS_PILOT,
            AD_PRESS_1,
            AD_PRESS_2,
            AD_PRESS_3,
            AD_PRESS_ATMOS,
			AD_AI_PV,
            AD_TEMP_INT,
            AD_DIAG
            ),
            NO_SLOW_CHANNELS
                )
};


// Calibrate sequence - read sensors at norm freq - Hall and remote pos both at short cycle
static const u8 measureSeqCalib[] =
{
    // N.B. The short sequence is 5 sensors long (as opposed to four)
    // when the calib sequence is selected.
    // The overall length is 8 slow channels with 4 fast
    ADTABLE(FAST_CHANNELS(AD_IP_CUR, AD_POS_REM, AD_POS_INT),
            AD_PRESS_PILOT,
            AD_PRESS_1,
            AD_PRESS_2,
            AD_PRESS_3,
            AD_PRESS_ATMOS,
			AD_AI_PV,
            AD_TEMP_INT,
            AD_DIAG)
};

const u8 * const adtbl_seqTbl[AD_SEQ_LIMIT] = /*lint -e{785} OK to have uninitialized entries */
{
    [AD_SEQ_POS_INT]     = measureSeqPosInt,
    [AD_SEQ_POS_REM]     = measureSeqPosRem,
    [AD_SEQ_CALIB]       = measureSeqCalib,
    [AD_SEQ_POS_INT_INIT]         = measureSeqPosInt_Init,
    [AD_SEQ_POS_REM_INIT]         = measureSeqPosExt_Init,
    [AD_SEQ_TEMPR] = measureSeqTempr,
};


void (* const ad_UpdateFunc[])(ADRaw_t advalue) =  /*lint -e{785} OK to have uninitialized entries */
{
    //----------- inputs only ------------
    [AD_TEMP_INT] = tempr_Update,
    [AD_TEMPR_INIT] = tempr_UpdateInit,
    [AD_AI_PV] = inpv_Update,
    [AD_POS_INT] = posint_Update,
    [AD_IP_CUR] = ipcurr_Update,
    [AD_POS_REM] = posext_Update,
    [AD_DIAG] = NULL,
};

//================= Configuration part ==============
static SensorType_t SensorType;
static const SensorType_t SensorType_default =
{
    .IsRemotePositionSensor = false,
    .CheckWord = 0, //don't care
};

const SensorType_t *ad_GetSensorType(SensorType_t *dst)
{
    return STRUCT_TESTGET(&SensorType, dst);
}

ErrorCode_t ad_SetSensorType(const SensorType_t *src)
{
    //Note: Runtime behavior hasn't changed yet
    ErrorCode_t err = ERR_OK;

    if(src == NULL)
    {
        src = &SensorType_default;
    }

    if(oswrap_IsOSRunning())
    {
        Struct_Test(SensorType_t, &SensorType);
        //Let's do housekeeping
        if(!equiv(SensorType.IsRemotePositionSensor, src->IsRemotePositionSensor))
        {
            MSeq_t ADseq;
            if(src->IsRemotePositionSensor)
            {
                ADseq = AD_SEQ_POS_REM;
            }
            else
            {
                ADseq = AD_SEQ_POS_INT;
            }

            adtbl_SetActiveSequence(ADseq);
            CONST_ASSERT(sizeof(measureSeqPosInt)==sizeof(measureSeqPosRem));
            /* The tables are identical except for pos sensor, so there is no upset of slow channels.
            No action required w.r.t. slow channels and cycle task watchdog
            */

            Struct_Copy(SensorType_t, &SensorType, src);

            error_SetFault(FAULT_FIND_STOPS_FAILED); //need new stops
            //pos_InitFilter(); //actually, too early; fix it time permitting

            err = ram2nvramAtomic(NVRAMID_SensorType);
        }
    }
    else
    {
        Struct_Copy(SensorType_t, &SensorType, src);
    }

    return err;
}

/* This line marks the end of the source */

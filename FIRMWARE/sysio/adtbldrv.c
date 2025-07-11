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
    \file adtbldrv.c
    \brief A/D sensor sequence runtime configuration and sequencing
    CPU: Philips LPC21x4 (ARM)

    OWNER: AP

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/adtbldrv.c $
    $Date: 12/07/11 6:39p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup ADTable
*/
/* $History: adtbldrv.c $
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 6:39p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8255 AP sequencing fix
*/
#include "mnwrap.h"
#include "mnassert.h"
#include "adtbl.h"
#include "timebase.h"
#include "process.h"
#include "crc.h"
#include "tempr.h"

typedef struct adtblinfo_t
{
    u16_least cycleTaskTime;     // interval, in milliseconds, of Cycle task
    MSeq_t activeSeq;         // table index of active sequence
    u16 CheckWord;
} adtblinfo_t;

static adtblinfo_t adt;

static MSeq_t adtbl_FindRuntimeTable(void)
{
    MSeq_t sequence;
    if(adtbl_seqTbl[AD_SEQ_POS_INT] == NULL)
    {
        MN_DBG_ASSERT((adtbl_seqTbl[AD_SEQ_POS_INT_INIT] == NULL)
                      && (adtbl_seqTbl[AD_SEQ_POS_REM_INIT] != NULL)
                      && (adtbl_seqTbl[AD_SEQ_POS_REM] != NULL)
                          );
        sequence = AD_SEQ_POS_REM;
    }
    else if(adtbl_seqTbl[AD_SEQ_POS_REM] == NULL)
    {
        MN_DBG_ASSERT((adtbl_seqTbl[AD_SEQ_POS_REM_INIT] == NULL)
                      && (adtbl_seqTbl[AD_SEQ_POS_INT_INIT] != NULL)
                      && (adtbl_seqTbl[AD_SEQ_POS_INT] != NULL)
                          );
        sequence = AD_SEQ_POS_INT;
    }
    else
    {
        MN_DBG_ASSERT((adtbl_seqTbl[AD_SEQ_POS_REM_INIT] != NULL)
                      && (adtbl_seqTbl[AD_SEQ_POS_INT_INIT] != NULL)
                          );
        //check option if both internal and remote position sensors are available
        if(ad_GetSensorType(NULL)->IsRemotePositionSensor)
        {
            sequence = AD_SEQ_POS_REM;
        }
        else
        {
            sequence = AD_SEQ_POS_INT;
        }
    }
    return sequence;
}


/** \brief Sets the A/D sampling sequence.
    Calls the BIOS to record the array with the A/D routines.

  \param[in] seqSel - table number of sequence table to be used
*/
void adtbl_SetActiveSequence(MSeq_t seqSel)
{
    const u8 *seq;

    if (seqSel >= AD_SEQ_DEFAULT)
    {
        seqSel = adtbl_FindRuntimeTable();
    }

    seq = adtbl_seqTbl[seqSel];
    MN_DBG_ASSERT(seq != NULL); //The project is misconfigured if the sequence is NULL
    if (seq != NULL)            //lint !e774  table may contain NULL
    {
        bios_WriteMeasureSequence(seq+1); //Skip over the number of A/D runs
        MN_ENTER_CRITICAL();
            adt.activeSeq     = seqSel;
            adt.cycleTaskTime = seq[0] * (TICK_MS * CTRL_TASK_DIVIDER);
            STRUCT_CLOSE(adtblinfo_t, &adt);
        MN_EXIT_CRITICAL();
    }
    // changing the table will disrupt the Cycle task.  Refresh the watchdog for it.
    process_SetWatchdogFlag(WD_CYCLE_TASK_RUN);
}

/** \brief Get the current long cyle time

  \return the time in milliseconds;
*/
u16_least adtbl_GetADCycleInterval(void)
{
    Struct_Test(adtblinfo_t, &adt);      // RAM data test
    return adt.cycleTaskTime;
}

MN_DECLARE_API_FUNC(adtbl_GetActiveSequence)
/** \brief Get the currently active A/D sequence table

  \return table reference
*/
MSeq_t adtbl_GetActiveSequence(void)
{
    Struct_Test(adtblinfo_t, &adt);      // RAM data test
    return adt.activeSeq;
}


/** \brief Performs fast load of the A/D samples during startup
    This code is for MNCB platform
*/
void adtbl_StartAD(void)
{
    //First, measure temperature only
    adtbl_SetActiveSequence(AD_SEQ_TEMPR); //will average the temperature
    (void)bios_MeasureAd();     // don't care about return!

    // Second, select table with all sensors at the fast (Control) frequency
    MSeq_t sequence = adtbl_FindRuntimeTable();
    if(sequence == AD_SEQ_POS_REM)
    {
        sequence = AD_SEQ_POS_REM_INIT;
    }
    else
    {
        sequence = AD_SEQ_POS_INT_INIT;
    }
    adtbl_SetActiveSequence(sequence);

    // measure all A/D channels - once
    (void)bios_MeasureAd();     // don't care about return!

#if 1
    // if the power has been off for a while, the powered circuits, pos_int,
    //   pos_rem and pressure(s) may not have been stable. So we do make a second
    //    pass through the startup table
    (void)bios_MeasureAd();     // don't care about return!
#endif

    // select the runtime A/D sequence table
    adtbl_SetActiveSequence(AD_SEQ_DEFAULT);
#ifndef NDEBUG
    //A temporary fix for posint/posext mix-up
    (void)bios_MeasureAd();     // don't care about return!
#endif
}

/* This line marks the end of the source */

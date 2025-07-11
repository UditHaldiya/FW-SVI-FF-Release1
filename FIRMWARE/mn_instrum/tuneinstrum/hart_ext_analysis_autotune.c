/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_ext_analysis.c
    \brief HART glue layer for control extended analysis command
           ("X-ray view" into control process)

    CPU: Any

    OWNER: SK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum/hart_ext_analysis_autotune.c $
    $Date: 5/19/11 12:20p $
    $Revision: 4 $
    $Author: Justin Shriver $

    \ingroup HARTapp
*/

/* $History: hart_ext_analysis_autotune.c $
 * 
 * *****************  Version 4  *****************
 * User: Justin Shriver Date: 5/19/11    Time: 12:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum
 * TFS:6337 -- Put back in functionality lost when updating
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 5/18/11    Time: 10:18a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum
 * TFS:6337
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 5/16/11    Time: 6:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum
 * TFS:6337
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 5/16/11    Time: 3:29p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum
 * TFS:6337
 *
 * *****************  Version 15  *****************
 * User: Justin Shriver Date: 5/16/11    Time: 9:19a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6305 LINT
 *
 * *****************  Version 14  *****************
 * User: Justin Shriver Date: 5/13/11    Time: 6:13p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6305 LINT
 *
 * *****************  Version 13  *****************
 * User: Justin Shriver Date: 5/13/11    Time: 5:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5384 Instrument Autotune Temporarily
 *
 * *****************  Version 12  *****************
 * User: Sergey Kruss Date: 3/18/11    Time: 2:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5899 Changed cmd-242 major version to 0xB9
 *
 * *****************  Version 11  *****************
 * User: Sergey Kruss Date: 3/02/11    Time: 12:18p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5746 - CalcFreq is an erroneous term: it must be period of adding
 * integral.
 *
 * *****************  Version 10  *****************
 * User: Justin Shriver Date: 1/13/11    Time: 12:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5384 Add debug capabilities to autotune
 *
 * *****************  Version 9  *****************
 * User: Sergey Kruss Date: 11/19/10   Time: 12:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:4761--Final checkin for command 242,2
 *
 * *****************  Version 8  *****************
 * User: Sergey Kruss Date: 11/17/10   Time: 4:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:4761--Added command 242,2 for observing position propagation
 * through all transformations from A/D converter.
 *
 * *****************  Version 7  *****************
 * User: Sergey Kruss Date: 10/01/10   Time: 11:52a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:4226. Changed Cotnrol Output related variables to less misleading.
 *
 * *****************  Version 6  *****************
 * User: Sergey Kruss Date: 9/24/10    Time: 10:09a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:4180
 * Fixed cmd-242,1 parameters types.
 *
 * *****************  Version 5  *****************
 * User: Sergey Kruss Date: 8/31/10    Time: 12:44p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * TFS:4016
 * -Added non-normalized value of PWM
 * -Changed the version to Alpha 9
 *
 * *****************  Version 4  *****************
 * User: Sergey Kruss Date: 8/07/10    Time: 3:17p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * TFS:3520, TFS:3596 -- Separate terms P and D and add Boost term
 *
 * *****************  Version 3  *****************
 * User: Sergey Kruss Date: 7/28/10    Time: 12:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * TFS-3637: Command 242 is modified so that subcommand 0 returns version.
 * Workhorse is moved to subcommand 1.
 *
 * *****************  Version 2  *****************
 * User: Sergey Kruss Date: 7/16/10    Time: 5:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * Fixed for Lint problems
 *
 * *****************  Version 1  *****************
 * User: Sergey Kruss Date: 6/25/10    Time: 1:51p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * Modified HART command 242, implemented it with XML and moved from
 * hartfunc.c
 *

*/

#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "ext_analysis.h"
#include "tuneinstrumpriv.h"



ExtAnalysAutoTune_t extDataAutoTune;


/**
    \brief Extended Analysis Data Collector
           Note:
           This command is allowed in all modes
*/
//this is a command shell -- does nothing


s8_least hartcmd_ExtendedAnalysisAutoTune(const u8 *src, u8 *dst)
{


    Req_ExtendedAnalysisAutoTune_t *s = (void *)src;
    Rsp_ExtendedAnalysisAutoTune_t *d = (void *)dst;

    u8 i = util_GetU8(s->eaat_req_step[0]);
    if (i>=MAX_STEPS)
    {

        i=MAX_STEPS;
    }



    util_PutU16(d->eaat_step[0], i);
    util_PutU16(d->eaat_initBias[0], extDataAutoTune.InitBias);
    util_PutU16(d->eaat_gainInit[0], extDataAutoTune.gainInit[0]);
    util_PutU16(d->eaat_gainInit[1], extDataAutoTune.gainInit[1]);
    util_PutU16(d->eaat_dTime[0], extDataAutoTune.dTime);
    util_PutU16(d->eaat_BiasLow[0], extDataAutoTune.BiasLow);
    util_PutU16(d->eaat_BiasHigh[0], extDataAutoTune.BiasHigh);
    util_PutU16(d->eaat_openLoopTests[0], extDataAutoTune.openLoopTests);
    util_PutU16(d->eaat_RampLoops[0], extDataAutoTune.RampLoops);
    util_PutU16(d->eaat_closedLoops[0], extDataAutoTune.closedLoops);
    util_PutU32(d->eaat_Time[i], extDataAutoTune.Times[i]);
    s16 last_step = i-1;
    last_step = MAX(last_step,0);
    util_PutU16(d->eaat_TimeLastStep[i], extDataAutoTune.Times[i]-extDataAutoTune.Times[last_step]);
    util_PutU16(d->eaat_TimeElapsed[i], extDataAutoTune.Times[i]-extDataAutoTune.Times[0]);
    util_PutU16(d->eaat_DeltaTime[i], extDataAutoTune.DeltaTime[i]);
    util_PutS16(d->eaat_Delta[i], extDataAutoTune.Delta[i]);
    util_PutS16(d->eaat_Inc[i], extDataAutoTune.Inc[i]);
    util_PutU16(d->eaat_gainP[i], extDataAutoTune.gainP[i]);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;

}




/* This line marks the end of the source */

s8_least hartcmd_ExtendedAnalysisAutoTuneTimes(const u8 *src, u8 *dst)
{

    UNUSED_OK(src);
    Rsp_ExtendedAnalysisAutoTuneTimes_t *d = (void *)dst;

    u8 i;
    for (i=0; i<MAX_STEPS; i++)
    {
        util_PutU32(d->eaat_Times[i], extDataAutoTune.Times[i]);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_ExtendedAnalysisAutoTuneDeltaTimes(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ExtendedAnalysisAutoTuneDeltaTimes_t *d = (void *)dst;

    u8 i;
    util_PutU16(d->eaat_DeltaTimes[0], 0);
    for (i=1; i<MAX_STEPS; i++)
    {
        util_PutU16(d->eaat_DeltaTimes[i], extDataAutoTune.Times[i]-extDataAutoTune.Times[0]);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_ExtendedAnalysisAutoTuneDeltas(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ExtendedAnalysisAutoTuneDeltas_t *d = (void *)dst;

    u8 i;
    for (i=0; i<MAX_STEPS; i++)
    {
        util_PutU16(d->eaat_Deltas[i], (u16)extDataAutoTune.Delta[i]);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_ExtendedAnalysisAutoTuneIncs(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ExtendedAnalysisAutoTuneIncs_t *d = (void *)dst;

    u8 i;
    for (i=0; i<MAX_STEPS; i++)
    {
        util_PutU16(d->eaat_Incs[i], (u16)extDataAutoTune.Inc[i]);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_ExtendedAnalysisAutoTunegainPs(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ExtendedAnalysisAutoTunegainPs_t *d = (void *)dst;

    u8 i;
    for (i=0; i<MAX_STEPS; i++)
    {
        util_PutU16(d->eaat_gainPs[i], extDataAutoTune.gainP[i]);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


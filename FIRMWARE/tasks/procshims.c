/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file procshims.c
    \brief Necessary wrappers for legacy or normal mode processes

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/tasks/procshims.c $
    $Date: 11/29/11 3:05p $
    $Revision: 13 $
    $Author: Arkkhasin $

    \ingroup Process
*/
/* $History: procshims.c $
 *
 * *****************  Version 13  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 3:05p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks
 * TFS:8313 Lint
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:41a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * comp_SaveAllComp_SHIM adapted
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 7:36p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 1/18/11    Time: 11:56a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:5411,5412,5413 Wrapper tidied up
 *
 * *****************  Version 8  *****************
 * User: Sergey Kruss Date: 1/05/11    Time: 2:39p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:5241 - osadj_RunOpenStopAdjustment() should return type void
 * TFS:4293 - Initial SP in OS Adj process must be set to current pos,
 * rather than to Norm Mode SP.
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 12/03/10   Time: 2:56a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:4946 - eliminated test_TestProcess_SHIM in favor of a correct
 * prototype of test_TestProcess
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 11/17/10   Time: 2:13p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:4760 - removed shim for the bumpless transfer process
 *
 * *****************  Version 5  *****************
 * User: Sergey Kruss Date: 10/26/10   Time: 2:17p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:4288 - OS Adj is called as a "normal" process via proc table.
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 10/14/10   Time: 6:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:4282,4283 Use mode_SetOOSModeEx() for UI process wrappers.
 * Open stop adjustment is not fixed; it will be fixed automatically with
 * fix of TFS:4288
 *
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 8/31/10    Time: 4:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * CHanged name - uihw_test.h.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 8/16/10    Time: 7:44p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Bug 3733 - Normal OOS is OK to run the process.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/14/10    Time: 7:33p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Necessary wrappers for legacy or normal mode processes for LCX (will be
 * conditionally compiled in the near future)
*/
#include "mnwrap.h"
#include "process.h"
#include "procpriv.h"
#include "procshims.h"

#include "control.h"
#include "nvram.h"
#include "selftune.h"
#include "diagnostics.h"
#include "hartfunc.h"
#include "tempcomp.h"

#include "ctllimits.h"

//for bad wrappers
#include "devicemode.h"
#include "oswrap.h"

//------- Good shims - wrappers of bare processes -----------------
//------- Backward compatibility shims - wrappers of old processes ------------------
#ifdef OLD_NVRAM
procresult_t nvram_PageTest_SHIM(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    nvram_PageTest();
    return PROCRESULT_OK;
}
#endif

procresult_t tune_Run_Selftune_SHIM(s16 *procdetails)
{
    return control_ProcWithNoLimits(tune_Run_Selftune, procdetails);
}

procresult_t diag_Run_StepTest_SHIM(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    diag_Run_StepTest();
    return PROCRESULT_OK;
}

#ifdef OLD_NVRAM
procresult_t hart_procRamCommitNoCal_SHIM(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    hart_procRamCommit(false);
    return PROCRESULT_OK;
}

procresult_t hart_procRamCommitWithCal_SHIM(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    hart_procRamCommit(true);
    return PROCRESULT_OK;
}

procresult_t comp_SaveAllComp_SHIM(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    comp_SaveAllComp(); //NOTE: old OLD_NVRAM  thus needs old tempcomp
    return PROCRESULT_OK;
}

#else
//Real brutal
static bool_t select_Vol0_NonTempcomp(u8 volid, u8 flags)
{
    return(volid==0) && ((flags & NVMFLAG_CALIB1) == 0);
}
static bool_t select_Vol0_Tempcomp(u8 volid, u8 flags)
{
    return(volid==0) && ((flags & NVMFLAG_CALIB1) != 0);
}
static bool_t select_Vol0_All(u8 volid, u8 flags)
{
    UNUSED_OK(flags);
    return(volid==0);
}

procresult_t hart_procRamCommitNoCal_SHIM(s16 *procdetails)
{
    (void)ram_CommitRange(select_Vol0_NonTempcomp);
    UNUSED_OK(procdetails);
    return PROCRESULT_OK;
}

procresult_t hart_procRamCommitWithCal_SHIM(s16 *procdetails)
{
    (void)ram_CommitRange(select_Vol0_All);
    UNUSED_OK(procdetails);
    return PROCRESULT_OK;
}

procresult_t comp_SaveAllComp_SHIM(s16 *procdetails)
{
    (void)ram_CommitRange(select_Vol0_Tempcomp);
    UNUSED_OK(procdetails);
    return PROCRESULT_OK;
}

#endif //OLD_NVRAM


/* This line marks the end of the source */

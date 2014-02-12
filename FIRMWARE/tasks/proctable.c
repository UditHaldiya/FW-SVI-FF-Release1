/*
Copyright 2005-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file proctable.c
    \brief Project-specific tables defining the actions of process.c.

    CPU: Any

    OWNER: AK

    \ingroup Process
*/
#include "projectdef.h"
#include "mnwrap.h"
#include "oswrap.h"
#include "errcodes.h"
#include "devicemode.h"
#include "nvram.h"
#include "procshims.h"
#include "adtbl.h"
#include "key.h"
#include "selftest.h"
#include "diagnostics.h"

#if FEATURE_LOGFILES == FEATURE_LOGFILES_USED
#include "diagrw.h"
#include "facdefaults.h"
#endif
#include "clonenvram.h"

#include "cycle.h"
#if FEATURE_DIAG_POSITIONER_DIAG == FEATURE_DIAG_POSITIONER_DIAG_INCLUDED
//#   include "diagnostics.h"
#endif //FEATURE_DIAG_POSITIONER_DIAG == FEATURE_DIAG_POSITIONER_DIAG_INCLUDED
#include "process.h"
#include "faultpublic.h"
#include "procpriv.h"
#if FEATURE_PARTIAL_STROKE_TEST == FEATURE_PARTIAL_STROKE_TEST_SUPPORTED
#   include "pst.h"
#endif //FEATURE_PARTIAL_STROKE_TEST
#if FEATURE_ESD_CONTINUOUS_DIAGS == FEATURE_ESD_CONTINUOUS_DIAGS_SUPPORTED
#   include "contdiag.h"
#endif
#if FEATURE_EVENT_MONITOR_SHUTDOWN == FEATURE_EVENT_MONITOR_SHUTDOWN_SUPPORTED
#   include "esdtripmon.h"
#endif

#include "uistartnodes.h"
#if FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
//backport #include "posstats.h"
#include "control.h"
//backport #include "stuckmon.h"
//backport #include "supplymon.h"
//backport #include "diagnostics2.h"
#include "selftune.h"
#endif //FEATURE_POSITION_CONTROL_INCLUDED
#include "hartfunc.h" //for HC_WRITE_COMMAND and such
//backport #include "doswitch.h"
//backport #include "posmon.h"
#if FEATURE_EVENT_MONITOR_SHUTDOWN == FEATURE_EVENT_MONITOR_SHUTDOWN_SUPPORTED
#include "unlatchmon.h"
#endif //FEATURE_EVENT_MONITOR_SHUTDOWN
#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
//backport
#include "insignal.h"
#endif
//backport
#include "position.h"
#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
#include "tmp100.h"
#include "diagDLT.h"
#include "dltfilterinterface.h"
#include "ao.h"
#include "aoramptest.h"
#include "adtbl_dlt.h"
#endif// FEATURE_HARDWARE_PLATFORM
#if FEATURE_AO_RETRANSMIT == FEATURE_AO_RETRANSMIT_AVAILABLE
#include "posretran.h"
#endif

#if FEATURE_BUMPLESS_XFER == FEATURE_BUMPLESS_XFER_ENABLED
#include "bumpless.h"
#endif

#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
#include "uihw_test.h"
#include "proc4uisimple.h"
#endif

#if FEATURE_DIGITAL_SETPOINT == FEATURE_DIGITAL_SETPOINT_SUPPORTED
#include "digitalsp.h"
#endif

#include "timer.h"
#include "poscharact.h"
#include "doswitch.h"
#include "bufferhandler.h"

// trick to use hartfunc.h in AP build
CONST_ASSERT (HC_WRITE_COMMAND != 0u);

/* \brief An empty function to make an empty process non-NULL
*/
static procresult_t DummyProc(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    return PROCRESULT_OK;
}

/** Array of definitions of "long processes" indexed by project-independent
"process ids".
If an entry is not explicitly iniitialized, it happens to have a NULL function pointer,
and no function will be run at a request with an uninitialized entry.
*/
const proctable_t proctable[] =
{
    {
        .id = PROC_NONE,
#ifdef OLD_NVRAM
        .procf = nvram_PageTest_SHIM,
#else
        .procf = nvram_PageTest,
#endif
        .flags = {.throughflags=0xFFU, .initflags = 0U}, //everything whatsoever is allowed
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#if FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
    {
        .id = PROC_FIND_STOPS,
        .procf = poscal_RunFindStops,
        .flags = {.throughflags=0, .initflags = PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_CALSTOPS,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE, //Code common to manual and auto find stops handles faults
                .UIplace = UINODEID_RUNSTOPS_FAILED,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE, //Code common to manual and auto find stops handles faults
                .UIplace = UINODEID_RUNSTOPS_FAILED,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
    {
        .id = PROC_FINDSTOPS_NORMAL,
        .procf = cal_RunFindStops_Normal,
        .flags = {.throughflags=0, .initflags = PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_CALSTOPS,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE, //Shims fake return code; not usable for faults FAULT_FIND_STOPS_FAILED,
                .UIplace = UINODEID_RUNSTOPS_FAILED,
                .termflags = PROCTERMFLAG_CTLMODE, //good only if canceled before started doing things
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE, //Shims fake return code; not usable for faults FAULT_FIND_STOPS_FAILED,
                .UIplace = UINODEID_RUNSTOPS_FAILED,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
        }
    },
#if FEATURE_UI_OPSTOP_ADJ == FEATURE_UI_OPSTOP_ADJ_PROCESS
    {   //TFS:4288 + corrected the cut-n-paste error, fields from Find Stops
        .id = PROC_OPENSTOPADJ_NORMAL,
        .procf = osadj_RunOpenStopAdjustment_Normal,
        .flags = {.throughflags=0, .initflags = PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_OPENSTOPADJ_OK,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_OPENSTOPADJ_FAILED,
                .termflags = PROCTERMFLAG_CTLMODE, //good only if canceled before started doing things
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_OPENSTOPADJ_FAILED,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
        }
    },
#endif //FEATURE_UI_OPSTOP_ADJ == FEATURE_UI_OPSTOP_ADJ_PROCESS
#endif //FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
#endif //FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
#if FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
    {
        .id = PROC_AUTOTUNE,
        .procf =  tune_Run_Selftune_SHIM,
        .flags = {.throughflags=0, .initflags = PROCINIT_CLAIMDIAGBUFFER|PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_TUNE_OK,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_AUTOTUNE_FAILED,
                .UIplace = UINODEID_TUNE_FAILED,
                .termflags = PROCTERMFLAG_CTLMODE, //good only if canceled before started doing things
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_AUTOTUNE_FAILED,
                .UIplace = UINODEID_TUNE_FAILED,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
        }
    },
#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
    {
        .id = PROC_AUTOTUNE_NORMAL,
        .procf =  tune_Run_Selftune_Normal,
        .flags = {.throughflags=0, .initflags = PROCINIT_CLAIMDIAGBUFFER|PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_TUNE_OK,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_AUTOTUNE_FAILED,
                .UIplace = -1, //atune takes care of diverse displays
                .termflags = PROCTERMFLAG_CTLMODE, //good only if canceled before started doing things
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_AUTOTUNE_FAILED,
                .UIplace = UINODEID_TUNE_FAILED,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
        }
    },
#endif //FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
#endif //FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
    {
        .id = PROC_AUTOTUNE,
        .procf = sf_AutoTuneProc,
        .flags = {.throughflags=0, .initflags = PROCINIT_CLAIMDIAGBUFFER},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_TUNE_OK,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_AUTOTUNE_FAILED,
                .UIplace = UINODEID_TUNE_CANCELED,
                .termflags = PROCTERMFLAG_CTLMODE, //good only if canceled before started doing things
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_AUTOTUNE_FAILED,
                .UIplace = UINODEID_TUNE_FAILED,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
        }
    },
    {
        .id = PROC_AOSWEEP,
        .procf = aosweep_Proc,
        .flags = {.throughflags=0, .initflags = PROCINIT_CLAIMDIAGBUFFER|PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_CTLMODE, //good only if canceled before started doing things
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
        }
    },
#endif



#if FEATURE_DIAG_POSITIONER_DIAG == FEATURE_DIAG_POSITIONER_DIAG_INCLUDED
    {
        .id = PROC_EXT_ACT_SIGNATURE,
        .procf = diag_Run_ExtActuatorSignature,
        .flags = {.throughflags=0, .initflags = PROCINIT_CLAIMDIAGBUFFER|PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_SETUPNODE,
                .termflags = PROCTERMFLAG_CTLMODE, //managed within the process
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE, //Shims fake return code; not usable for faults FAULT_EXT_DIAGNOSTICS_FAILED,
                .UIplace = -1, //no UI change if cannot start
                .termflags = PROCTERMFLAG_CTLMODE, //good only if canceled before started doing things
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_EXT_DIAGNOSTICS_FAILED,
                .UIplace = UINODEID_SETUPNODE,
                .termflags = PROCTERMFLAG_CTLMODE, //managed within the process
            },
        }
    },
    {
        .id = PROC_DIAG_RAMP_TEST,
        .procf = diag_Run_RampTest,
        .flags = {.throughflags=0, .initflags = PROCINIT_CLAIMDIAGBUFFER|PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_SETUPNODE,
                .termflags = PROCTERMFLAG_NOCTLMODE, //managed within the process
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE, //Shims fake return code; not usable for faults FAULT_EXT_DIAGNOSTICS_FAILED,
                .UIplace = -1, //no UI change if cannot start
                .termflags = PROCTERMFLAG_NOCTLMODE, //good only if canceled before started doing things
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_EXT_DIAGNOSTICS_FAILED,
                .UIplace = UINODEID_SETUPNODE,
                .termflags = PROCTERMFLAG_NOCTLMODE, //managed within the process
            },
        }
    },  // ****NOTE:  this index is out of order on purpose to be inside conditional compile
#endif //FEATURE_DIAG_POSITIONER_DIAG
#if FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
    { //by design: restores control mode only if fails
        .id = PROC_STEP_TEST,
        .procf = diag_Run_StepTest_SHIM,
        .flags = {.throughflags=0U,  .initflags = PROCINIT_CLAIMCTLMODE},
        .action =
        { //rather stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_SETUPNODE,
                .termflags = PROCTERMFLAG_NOCTLMODE, //leave the valve positioned at the end position
                                                     //to allow successive step tests
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE, //Shims fake return code; not usable for faults FAULT_EXT_DIAGNOSTICS_FAILED,
                .UIplace = UINODEID_SETUPNODE,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
        }
    },
#if FEATURE_BUMPLESS_XFER == FEATURE_BUMPLESS_XFER_ENABLED
    {
        .id = PROC_BUMPLESS,
        .procf = mode_RunBumpless,
        .flags = {.throughflags=0U,  .initflags = PROCINIT_CLAIMCTLMODE},
        .action =
        { //rather stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = UINODEID_BUMPLESS_XFER_FAILED,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#endif //FEATURE_BUMPLESS_XFER
#endif //FEATURE_POSITION_CONTROL

#if 1 //for now def OLD_NVRAM
    {
        .id = PROC_HART_SUB132_INIT_NOCAL,
        .procf = hart_procRamCommitNoCal_SHIM,
        .flags = {.throughflags=0U, .initflags = 0U},
        .action =
        { //absolutely stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
    {
        .id = PROC_HART_SUB132_INIT_WITHCAL,
        .procf = hart_procRamCommitWithCal_SHIM,
        .flags = {.throughflags=0U, .initflags = 0U},
        .action =
        { //absolutely stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
    {
        .id = PROC_HART_235_SAVECAL,
        .procf = comp_SaveAllComp_SHIM,
        .flags = {.throughflags=0U, .initflags = 0U},
        .action =
        { //absolutely stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#else //new nvram
    {
        .id = PROC_HART_RAM_COMMIT,
        .procf = hart_procRamCommit,
        .flags = {.throughflags=0U, .initflags = 0U},
        .action =
        { //absolutely stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#endif //OLD_NVRAM

#if FEATURE_ESD_CONTINUOUS_DIAGS == FEATURE_ESD_CONTINUOUS_DIAGS_SUPPORTED
    { //by design: changes but doesn't restore control mode even if failed or canceled
        .id = PROC_ESD_CONT_DIAGS,
        .procf = cdiag_DiagCycle,
        .flags = {.throughflags=0U, .initflags = PROCINIT_CLAIMDIAGBUFFER|PROCINIT_CLAIMCTLMODE},
        .action =
        { //absolutely stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] = //dummy: cannot be canceled
            {
                .event = FAULT_NONE, //Shims fake return code; not usable for faults FAULT_PNEUMATIC_TEST_CANCELED,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE, //Shims fake return code; not usable for faults FAULT_PNEUMATIC_TRAIN,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
    {
        .id = PROC_IP_STEP_CAL,
        .procf = cdiag_IPCalibrate,
        .flags = {.throughflags=0U, .initflags = PROCINIT_CLAIMDIAGBUFFER},
        .action =
        { //doesn't change control mode, UI or faults
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] = //dummy: cannot be canceled
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#endif //FEATURE_ESD_CONTINUOUS_DIAGS
#if FEATURE_TERMINAL_BOARD_TEST == FEATURE_TERMINAL_BOARD_TEST_SUPPORTED
    {
        .id = PROC_ESD_TB_DIAG,
        .procf = cdiag_TerminalBoardTest,
        .flags = {.throughflags=0U, .initflags = PROCINIT_CLAIMDIAGBUFFER},
        .action =
        { //absolutely stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] = //dummy: cannot be canceled
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_TERMINAL_BOARD,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#endif //FEATURE_TERMINAL_BOARD_TEST
#if FEATURE_PARTIAL_STROKE_TEST == FEATURE_PARTIAL_STROKE_TEST_SUPPORTED
    {
        .id = PROC_DIAG_PARTIAL_STROKE_TEST,
        .procf = diag_PartialStrokeTest,
        .flags = {.throughflags=0U, .initflags = PROCINIT_CLAIMDIAGBUFFER|PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_PST_CANCELED,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_PST_FAILED,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_CTLMODE,
            },
        }
    },
    {//by design: leave the valve where it is for the next diagnostic, even if failed
        .id = PROC_PROOF_TEST,
        .procf = diag_ProofTest,
        .flags = {.throughflags=0U, .initflags = PROCINIT_CLAIMDIAGBUFFER|PROCINIT_CLAIMCTLMODE},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE, //Shims fake return code; not usable for faults FAULT_EXT_DIAGNOSTICS_FAILED,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE, //Shims fake return code; not usable for faults FAULT_EXT_DIAGNOSTICS_FAILED,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#endif //FEATURE_PARTIAL_STROKE_TEST
#if FEATURE_EVENT_MONITOR_SHUTDOWN == FEATURE_EVENT_MONITOR_SHUTDOWN_SUPPORTED
    {
        .id = PROC_SHUTDOWN_MONITOR,
        .procf = sdmon_Proc,
        .flags = {.throughflags=HC_WRITE_COMMAND, .initflags = 0U},
        .action =
        { //absolutely stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] = //dummy: cannot be canceled
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] = //dummy: cannot be fail
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#endif //FEATURE_EVENT_MONITOR_SHUTDOWN
#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
    {
        .id = PROC_TEST_PROCESS,
        .procf = test_TestProcess,
        .flags = {.throughflags=0U, .initflags = 0U},
        .action =
        { //absolutely stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#endif //FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
    {
        .id = PROC_CANCEL_PROC,
        .procf = DummyProc, //we need a non-NULL placeholder for a process
        .flags = {.throughflags=HC_WRITE_COMMAND, .initflags = 0U},
        .action =
        { //absolutely stealth
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] = //dummy: cannot be canceled
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] = //dummy: cannot fail
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
    //------------------ new diagnostics ------------------
#if FEATURE_LOGFILES == FEATURE_LOGFILES_USED
    {
        .id = PROC_READ_BUFFER,
        .procf = diag_ReadBuffer,
        .flags = {.throughflags=0U, .initflags = 0},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
    {
        .id = PROC_WRITE_BUFFER,
        .procf = diag_WriteBuffer,
        .flags = {.throughflags=0U, .initflags = 0},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
    {
        .id = PROC_FILE_NVMEM,
        .procf = nvmem_SaveAsFactoryDefaults,
        .flags = {.throughflags=0U, .initflags = 0},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NV_BK_RESTORE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
    {
        .id = PROC_RESTORE_FILED_NVMEM,
        .procf = nvmem_RestoreFactoryDefaults,
        .flags = {.throughflags=0U, .initflags = 0},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NV_BK_RESTORE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
#endif
    {
        .id = PROC_CLONE_NVMEM,
        .procf = nvmem_Clone,
        .flags = {.throughflags=0U, .initflags = 0},
        .action =
        {
            [PROCRESULT_OK] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_CANCELED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
            [PROCRESULT_FAILED] =
            {
                .event = FAULT_NONE,
                .UIplace = -1,
                .termflags = PROCTERMFLAG_NOCTLMODE,
            },
        }
    },
};

const u8 proctable_size = NELEM(proctable); //!< exported size of proctable

/** Array of function pointers (to mop-up - deferred action - functions).
process.c services one function at a time in cyclical order.
*/
MopupFunc_t * const mopuplist[] =
{
    error_Mopup, //deferred fault storage save (NVRAM save)
    mode_Mopup,

#if FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
    //posmon_ContinuousDiagnostics,
    control_ContinuousDiagnostics, //old
    control_CheckSaveBias,
#endif //FEATURE_POSITION_CONTROL
    cycle_Mopup,
#if FEATURE_PARTIAL_STROKE_TEST == FEATURE_PARTIAL_STROKE_TEST_SUPPORTED
    pst_PSTScheduler,
#endif //FEATURE_PARTIAL_STROKE_TEST
#if FEATURE_ESD_CONTINUOUS_DIAGS == FEATURE_ESD_CONTINUOUS_DIAGS_SUPPORTED
    cdiag_Scheduler,
#endif //FEATURE_ESD_CONTINUOUS_DIAGS
    selftest_FlashCrcMon,
#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
    tmp100_ReadTemp,
    ProcessTask_UpdateTimeDiagnostics, //time statistics-diagnostics for DLT
    ProcessTask_TestDisplacerHeight, //test if (URV-LRV) > DispHeight
#endif
#if FEATURE_DIGITAL_SETPOINT == FEATURE_DIGITAL_SETPOINT_SUPPORTED
    digsp_Mopup,
#endif
    timer_IntegrityCheck,
    error_TestData,
    //Periodic_Measurement_Table_Test,
};

const u8 mopuplist_size = NELEM(mopuplist); //!< exported size of mopuplist

#ifndef OLD_CYCLETASK
static const taskchore_t cycle_inittable[] =
{
#if FEATURE_BUMPLESS_XFER == FEATURE_BUMPLESS_XFER_ENABLED
    //bumpless_Correct,
#endif
    adtbl_StartAD,
#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
    //done inside initjob_Temperature,
#endif
    periodicjob_Temperature, //need to do it right away

#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
    sig_InitFilter,
#endif

    pos_InitFilter,
    pos_InitFilterRaw,
    mode_GuardControlMode, //in case someone wants to set mode-affecting/affected faults, may need position measurement so it's not the first
#if FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
    //control_FixBias,                  // control needs temperature from Cycle
#endif //FEATURE_POSITION_CONTROL

//Wrong conditional
#if 0 //FEATURE_LOGFILES == FEATURE_LOGFILES_USED
    dsampler_Init,
#endif

#if FEATURE_EVENT_MONITOR_SHUTDOWN == FEATURE_EVENT_MONITOR_SHUTDOWN_SUPPORTED
    softesd_Init,
    sdmon_Init, //must be **AFTER** dsampler_Init
    unlatchmon_Init,
#endif //FEATURE_EVENT_MONITOR_SHUTDOWN
#if FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED
    //stuck_MonInit,
    //supplymon_Init,
#endif //FEATURE_POSITION_CONTROL
    mode_Mopup, //in case we wake up in shutdown
    //posmon_Init,
    NULL
};

static const taskchore_t cycle_runtable[] =
{
    periodicjob_Temperature,
#if FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
    periodicjob_RefVoltage,
#endif
#if FEATURE_PRESSURE_INPUTS != FEATURE_PRESSURE_INPUTS_EXCLUDED
    pres_PressureComp,
#endif
#if FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED
    //supply_Monitor,
#endif //FEATURE_POSITION_CONTROL
#if FEATURE_AO == FEATURE_AO_SUPPORTED
    periodicjob_PositionRetransmit,
#endif
    //posmon_Monitor,
    dosw_OutputSwitches, //Replace with periodicjob_DOSwitches, time permitting
#if FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED
    //stuck_Monitor,
#endif //FEATURE_POSITION_CONTROL
#if FEATURE_EVENT_MONITOR_SHUTDOWN == FEATURE_EVENT_MONITOR_SHUTDOWN_SUPPORTED
    sdmon_Monitor,
#endif //FEATURE_EVENT_MONITOR_SHUTDOWN

#if FEATURE_EVENT_MONITOR_SHUTDOWN == FEATURE_EVENT_MONITOR_SHUTDOWN_SUPPORTED
    unlatchmon_Run,
#endif //FEATURE_EVENT_MONITOR_SHUTDOWN
    key_Mopup,
    poscharact_Mopup,
    buffer_SampleAllData,
    NULL
};

const taskjob_t taskjob_cycle =
{
    cycle_inittable,
    cycle_runtable,
    WD_CYCLE_TASK_RUN
};
#endif //OLD_CYCLETASK

/* This line marks the end of the source */

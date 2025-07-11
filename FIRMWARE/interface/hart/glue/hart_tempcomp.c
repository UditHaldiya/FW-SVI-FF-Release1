/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_tempcomp.c
    \brief Temperature compensation HART interface

    CPU: Any

    OWNER: LS
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_tempcomp.c $
    $Date: 12/07/11 12:58p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_tempcomp.c $
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 12:58p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:13p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8313 Lint
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 3:30p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 *
 * *****************  Version 83  *****************
 * User: Arkkhasin    Date: 3/30/11    Time: 1:39p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Reverted to old API for old fixed-length commands
 *
 * *****************  Version 81  *****************
 * User: Arkkhasin    Date: 9/15/10    Time: 2:07p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Partial dismantling for modularization
 *
 * *****************  Version 80  *****************
 * User: Arkkhasin    Date: 9/13/10    Time: 12:31p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Extracted hart_corefac.c from hartfunc_fac.c,79 (TFS:4067)
 *
 * *****************  Version 79  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 4:12p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * hartcmd_DirectAOControl moved from hartfunc_fac.c to hart_ao.c
 * Added inputs AO feature
 *
 * *****************  Version 78  *****************
 * User: Arkkhasin    Date: 11/17/09   Time: 6:09p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Light linting
 *
 * *****************  Version 77  *****************
 * User: Arkkhasin    Date: 7/31/09    Time: 6:32p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Brought back ver. 78 from DLT 1.1.1 branch
 *
 * *****************  Version 78  *****************
 * User: Arkkhasin    Date: 7/14/09    Time: 6:56p
 * Updated in $/MNCB/Dev/DLT_Release_1.1.1/FIRMWARE/interface/hart
 * Command 129/126 to read expected NVMEM signature
 *
 * *****************  Version 76  *****************
 * User: Arkkhasin    Date: 6/04/09    Time: 1:33a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * CONST_ASSERTs for Lint
 *
 * *****************  Version 75  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 2:20p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Header refactoring (for DLT lint)
 *
 * *****************  Version 74  *****************
 * User: Arkkhasin    Date: 3/30/09    Time: 2:33p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Removed unused configure.h
 *
 * *****************  Version 73  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 3:13a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Removed unused charact.h
 *
 * *****************  Version 72  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 1:13a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Removed unused control.h
 *
 * *****************  Version 71  *****************
 * User: Arkkhasin    Date: 3/28/09    Time: 4:02p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Removed wrapper header utility.h
 *
 * *****************  Version 70  *****************
 * User: Arkkhasin    Date: 2/26/09    Time: 6:10p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Repaired and put back command 128
 *
 * *****************  Version 69  *****************
 * User: Arkkhasin    Date: 2/16/09    Time: 5:31p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Resurrected old Larry's "smooth filters" settings (but away from
 * hartfunc_fac.c)
 *
 * *****************  Version 68  *****************
 * User: Arkkhasin    Date: 2/13/09    Time: 1:00a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Removed 129/{135,136} and 130/136
 *
 * *****************  Version 67  *****************
 * User: Arkkhasin    Date: 2/11/09    Time: 3:18p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Factory Options Read/Write implemented as commands {129,130}/248
 * compatible with MNCB
 *
 * *****************  Version 66  *****************
 * User: Arkkhasin    Date: 2/09/09    Time: 12:03p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * hart_procRamCommit() no longer causes automatic reset
 *
 * *****************  Version 65  *****************
 * User: Anatoly Podpaly Date: 2/04/09    Time: 4:09p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * LINT:
 * - ramdatamap.h is included only when it is needed.
 * - function hartcmd_ReadKeyStates:
 *  -- properly ignore (void) the return value when calling
 * functionbios_ReadPushButtonStatus.
 *  -- remove not needed casts.
 *  -- remove complains about bitwise operations.
 *
 *
 * *****************  Version 64  *****************
 * User: Arkkhasin    Date: 1/15/09    Time: 12:23a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Lint (partial)
 * MNCB compile restored (but crippled in functionality)
 *
 * *****************  Version 63  *****************
 * User: Arkkhasin    Date: 1/11/09    Time: 7:47p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * hart_IsFactoryCmdEnabled went to hartapputils.c
 *
 * *****************  Version 62  *****************
 * User: Arkkhasin    Date: 1/09/09    Time: 11:49p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * DeviceId ==> DeviceID to match the xml.
 * Thanks, Ken
 *
 * *****************  Version 61  *****************
 * User: Arkkhasin    Date: 1/09/09    Time: 1:30a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * First shot at UI control over HART in factory mode (primarily for
 * autoamted testing), and the first cut of supporting xmltostdout
 *
 * *****************  Version 60  *****************
 * User: Derek Li     Date: 12/22/08   Time: 7:36p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * add hard commands function support for hart commands 130-136, 129-135,
 * 129-136
 *
 * *****************  Version 59  *****************
 * User: Derek Li     Date: 12/18/08   Time: 7:03p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * hart commands 128, and 248 for mncb platform (new style with xmls
 * supprot)
 *
 * *****************  Version 58  *****************
 * User: Justin Shriver Date: 12/18/08   Time: 6:19p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Making buildable
 *
 * *****************  Version 57  *****************
 * User: Arkkhasin    Date: 12/16/08   Time: 7:58p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * faultdef.{inc,h} go by the natural name, not symbolic anymore
 *
 * *****************  Version 56  *****************
 * User: Sergey Kruss Date: 12/05/08   Time: 12:33p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Added time statistics-diagnostics
 *
 * *****************  Version 54  *****************
 * User: Arkkhasin    Date: 11/21/08   Time: 12:51p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Renamed HART variables.
 *
 * *****************  Version 53  *****************
 * User: Arkkhasin    Date: 11/17/08   Time: 6:51p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Fixed bug 49
 *
 * *****************  Version 52  *****************
 * User: Arkkhasin    Date: 11/17/08   Time: 12:06a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * MNCB-style PV is tempcomped only in MNCB (not in DLT where it will soon
 * be gone).
 *
 * *****************  Version 51  *****************
 * User: Arkkhasin    Date: 10/17/08   Time: 2:37a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Removed commands 131, 133 from DLT
 * Added commands 36, 37 to DLT
 * err2hart made public for HART
 *
 * *****************  Version 50  *****************
 * User: Arkkhasin    Date: 10/13/08   Time: 12:20a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Lint (types and headers)
 * Added signed versions of util_Get..., util_Put...
 * Some service functions made FEATURE-conditional
 *
 * *****************  Version 49  *****************
 * User: Arkkhasin    Date: 10/01/08   Time: 5:22p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Project-specific factory operations extracted into separate source
 * files.
 *
 * *****************  Version 48  *****************
 * User: Arkkhasin    Date: 9/22/08    Time: 4:54p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Added direct-control of (both) ao
 *
 * *****************  Version 47  *****************
 * User: Arkkhasin    Date: 9/19/08    Time: 7:47p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Fixed size mismatch of util_{Ge,Pu}t_Uxx in 32-bit tempcomp. Oops.
 *
 * *****************  Version 46  *****************
 * User: Arkkhasin    Date: 9/19/08    Time: 6:40p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Added R/W linearization table to 129/130 (sub 42)
 * 1st shot at DLT tempcomp support via HART
 * AP/ESD builds remain broken for now
 *
 * *****************  Version 45  *****************
 * User: Arkkhasin    Date: 9/11/08    Time: 6:49p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Refactored (partially) commands 129 and 130 to take advantage of
 * automatic import of command definitions (which have changed, too)
 *
 * *****************  Version 44  *****************
 * User: Arkkhasin    Date: 8/06/08    Time: 11:31p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * hartdef.u (and .h) replaces hartdef_common.u and hartdef_project.u
 * (resp., .h)
 *
 * *****************  Version 43  *****************
 * User: Arkkhasin    Date: 7/28/08    Time: 1:53p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Light linting
 *
 * *****************  Version 42  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 10:12p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * MNCB builds repaired
 *
 * *****************  Version 41  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 9:24p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Adapted to tempr interface changes
 *
 * *****************  Version 40  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 1:30a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Changes to accommodate the new position input management
 *
 * *****************  Version 39  *****************
 * User: Arkkhasin    Date: 5/29/08    Time: 10:15a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Implemented subcommand 127 (R/W board id) for commands 129/130 R/W
 * memory.  Address field represents the volume; only 0 (and 1 for DLT)
 * are valid values.
 * Changed subcommand 130 (R/W FRAM): Address field's 3 MSB represent the
 * volume; only 00 (and 01 for DLT) are valid patterns.
 *
 * *****************  Version 38  *****************
 * User: Arkkhasin    Date: 5/28/08    Time: 3:53p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Implemented NVMEM initialization over HART (for any volume)
 *
 * *****************  Version 37  *****************
 * User: Arkkhasin    Date: 5/26/08    Time: 10:13p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Removed obsolete header
 *
 * *****************  Version 36  *****************
 * User: Arkkhasin    Date: 5/23/08    Time: 10:15a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * hart_FactoryCommand_133 now takes an optional NVRAM volume id to clear
 * (if omitted, vol 0 assumed)
 *
 * *****************  Version 35  *****************
 * User: Arkkhasin    Date: 5/19/08    Time: 4:01p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * New interface for ram_Commit() - now takes a selector function as a
 * parameter
 *
 * *****************  Version 34  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 4:59p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Adapted to new tempcomp interface
 *
 * *****************  Version 33  *****************
 * User: Ernieprice   Date: 5/12/08    Time: 9:48a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Updated files for 12400 BIOS - added include of resource.h
 *
 * *****************  Version 32  *****************
 * User: Arkkhasin    Date: 4/12/08    Time: 1:43a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Limited refactoring required by Lint: some types changed, errors either
 * propagate up or explicitly ignored; a new HART error code 43 added to
 * indicate failed NVMEM write
 *
 * *****************  Version 31  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 1:14p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Adapted to new API without changing the format of HART data
 * Obscure and weak ram_Commit replaced with clear ram_CommitRange with a
 * different API
 *
 * *****************  Version 29  *****************
 * User: Arkkhasin    Date: 3/05/08    Time: 6:24p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * Use mode_{Set,Get}Mode() API
 *
 * *****************  Version 28  *****************
 * User: Arkkhasin    Date: 2/24/08    Time: 8:52p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * Removed no longer used header(s)
 *
 * *****************  Version 27  *****************
 * User: Arkkhasin    Date: 2/15/08    Time: 6:04p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * Updated for new mode, position and temperature API
 *
 * *****************  Version 26  *****************
 * User: Arkkhasin    Date: 2/01/08    Time: 9:03p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * hart_FactoryCommand_138_SetFilterCoef() is retrofitted for new filter
 * design - spread over the data sources being filtered
 *
 * *****************  Version 25  *****************
 * User: Arkkhasin    Date: 1/03/08    Time: 6:38p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * A massive but interim check-in:
 * - Initialization is handled with a common routine
 * - ATO/ATC moved to PositionStop to improve modularity
*/

// #include "projectdef.h"
//#include "config.h"
#include "mnwrap.h"
//#include "mnassert.h"
//#include "mncbtypes.h"
#include "nvram.h"
//#include "devicemode.h"
//#include "faultpublic.h"
//#include "process.h"
#include "hartfunc.h"
//#include "hartpriv.h"
#include "hartcmd.h"
#include "tempcomp.h"
//#include "smoothing.h"
//#include "watchdog.h"
#include "hartdef.h"
//#include "position.h"
//#include "insignal.h"
#include "tempr.h"
//#include "ao.h"
//#include "key.h"

#include "hart_fac.h"


//#include "hartdef.h"
//#include "hart_fac.h"

//#include "uipublic.h"
//#include "lcd.h"

#include "bufutils.h"

#if 0
static u8 ReqVolid;
static u8 ReqCommitFlip;
static u8 ReqCommitSuppress;
/**
\brief writes RAM to FRAM with or without rewriting the calibration data

used by the factory only - writes RAM to FRAM with or without rewriting the calibration data
This automatically sets the fram version number correctly
*/
s8_least hartcmd_CommitRAMtoNVMEM(const u8 *src, u8 *dst)
{
    const Req_CommitRAMtoNVMEM_t *s = (const void *)src;
    UNUSED_OK(dst);
    ReqVolid = util_GetU8(s->NVMEMVolumeId[0]);
    ReqCommitFlip = util_GetU8(s->RAMCommitFlip[0]);
    ReqCommitSuppress = util_GetU8(s->RAMCommitSuppress[0]);
    return hart_SetProcessCommand(PROC_HART_RAM_COMMIT);
}


static bool_t select_VolX_byMask(u8 volid, u8 flags)
{
    return (volid==ReqVolid) && (((flags ^ ReqCommitFlip) & ReqCommitSuppress) == 0);
}


/** \brief Resets MNCB after ensuring the FRAM is initialized
    from RAM (excluding temperature calibrations) and that the HART response went out
    IMPORTANT: This can only be called from the context of the
    process task!!!
*/
procresult_t hart_procRamCommit(s16 *procdetails)
{
    //Nothing we can do if writes fail
    (void)ram_CommitRange(select_VolX_byMask);
    //no longer, per Sean's request: m_bResetRequested = true;
    UNUSED_OK(procdetails);
    return PROCRESULT_OK;
}
#endif


/**
\brief clears the fram version number - this will cause all defaults to be loaded on reset

used by the factory only - clears the fram version number - this will cause all defaults to be loaded on reset
*/
s8_least hartcmd_ClearNVMEMSignature(const u8 *src, u8 *dst)
{
    const Req_ClearNVMEMSignature_t *s = (const void *)src;
    u8 volid = util_GetU8(s->NVMEMVolumeId[0]);

    ErrorCode_t err = nvram_SetSignature(0U, volid);
    if(err == ERR_INVALID_PARAMETER)
    {
        return HART_INVALID_DATA;
    }
    //Ignore all other errors
    return hart_Command_42_Reset(src, dst);
}

/**
\brief reset the low and high temperatures stored in fram back to current temp
*/
s8_least hartcmd_ClearBoardTemperature(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_ClearBoardTemperature_t *s = (const void *)src;
    u8 volid = util_GetU8(s->TemprSensorId[0]);
    tempr_FactoryResetStats(volid);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


#if 0
/**
\brief Writes units and range values of level and reports actually accepted values
Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadKeyStates(const u8 *src, u8 *dst)
{
    // u8 pressed = 0;

    UNUSED_OK(src);
    Rsp_ReadKeyStates_t *d = (void *)dst;

    u8 status =
#if KBD_OPTION_EDGE_UP == OPTION_OFF
        PUSHBUTTON_DOWN_EDGE;
#endif
#if KBD_OPTION_EDGE_UP == OPTION_ON
        PUSHBUTTON_RELEASE_EDGE;
#endif
    (void) bios_ReadPushButtonStatus(&status);

    d->KeyStar  [0][sizeof(KeyStarEnum_t)  - 1] =  (status & 1u);
    d->KeyMinus [0][sizeof(KeyMinusEnum_t) - 1] = ((status & 2u)>>1u);
    d->KeyPlus  [0][sizeof(KeyPlusEnum_t)  - 1] = ((status & 4u)>>2u);

    return (s8_least)HART_NO_COMMAND_SPECIFIC_ERRORS;
} // ----- end of hartcmd_ReadKeyStates() -----

#endif


s8_least hartcmd_ReadTempCompPoints(const u8 *src, u8 *dst)
{
    const Req_ReadTempCompPoints_t *s = (const void *)src;
    Rsp_ReadTempCompPoints_t *d = (void *)dst;
    u8 BoardNumber = util_GetU8(s->BoardNumber[0]);
    if(BoardNumber >= TEMPR_NUM_SENSORS)
    {
        return HART_INVALID_SELECTION;
    }
    //Board number is already in the output buffer - by the HART framework
    CONST_ASSERT(MN_OFFSETOF(Req_ReadTempCompPoints_t, BoardNumber) == MN_OFFSETOF(Rsp_ReadTempCompPoints_t, BoardNumber));

    const TempComp_t *tab = TempComp[BoardNumber];
    const TemperatureCalibrationPoints_t *pPts = nvmem_GetItemById(NULL, tab->ptsId);
    util_PutU8(d->NumberofPoints[0], pPts->ActualNumberTempPoints);
    CONST_ASSERT(NELEM(d->TempPoint) == NELEM(pPts->TemperatureAD));
    u8_least i;
    for(i=0; i<NELEM(pPts->TemperatureAD); i++)
    {
        util_PutU16(d->TempPoint[i], pPts->TemperatureAD[i]);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_WriteTempCompPoints(const u8 *src, u8 *dst)
{
    const Req_WriteTempCompPoints_t *s = (const void *)src;
    UNUSED_OK(dst);
    u8 BoardNumber = util_GetU8(s->BoardNumber[0]);
    if(BoardNumber >= TEMPR_NUM_SENSORS)
    {
        return HART_INVALID_SELECTION;
    }
    const TempComp_t *tab = TempComp[BoardNumber];
    TemperatureCalibrationPoints_t Pts;

    Pts.ActualNumberTempPoints = util_GetU8(s->NumberofPoints[0]);
    CONST_ASSERT(NELEM(s->TempPoint) == NELEM(Pts.TemperatureAD));
    u8_least i;
    for(i=0; i<NELEM(Pts.TemperatureAD); i++)
    {
        Pts.TemperatureAD[i] = util_GetU16(s->TempPoint[i]);
    }
    return (nvmem_SetItemById(&Pts, tab->ptsId)==ERR_OK)?HART_NO_COMMAND_SPECIFIC_ERRORS:HART_INVALID_DATA;
}

s8_least hartcmd_WriteTempCompRow(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}
s8_least hartcmd_ReadTempCompRow(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/** \brief Generic 16-bit tempcomp HART helper (write)
\param s - a pointer to a piece of HART buffer with 16-bit tempcomp row
\param setf - a pointer to the function to set the tempcomp data
\return HART error code or HART_NO_COMMAND_SPECIFIC_ERRORS
*/
s8_least SetTempComp16(const TCRow16_t *s, ErrorCode_t (*setf)(const TemperatureCalibrationRow_t *arg))
{
    TemperatureCalibrationRow_t trow;
    //Populate the data structure to pass to setf by parsing the HART buffer
    trow.ChannelShift = util_GetU8(s->ChannelShift[0]);
    for(u8_least i=0; i<NELEM(trow.TemperatureCalData); i++)
    {
        const TCOffsetSpan16_t *p = (const void *)s->TCOffsetSpan16[i];
        trow.TemperatureCalData[i].offset = (s16)util_GetU16(p->TCOffset16[0]);
        trow.TemperatureCalData[i].span = (s16)util_GetU16(p->TCSpan16[0]);
    }
    ErrorCode_t err = setf(&trow);
    return err2hart(err);
}
/** \brief Generic 16-bit tempcomp HART helper (read)
\param trow - a pointer to the data structure to format for HART
\param[out] d - a pointer to a piece of HART buffer with 16-bit tempcomp row
\return HART error code or HART_NO_COMMAND_SPECIFIC_ERRORS
*/
s8_least GetTempComp16(const TemperatureCalibrationRow_t *trow, TCRow16_t *d)
{
    util_PutU8(d->ChannelShift[0], trow->ChannelShift);
    for(u8_least i=0; i<NELEM(trow->TemperatureCalData); i++)
    {
        TCOffsetSpan16_t *p = (void *)d->TCOffsetSpan16[i];
        util_PutU16(p->TCOffset16[0], (u16)trow->TemperatureCalData[i].offset);
        util_PutU16(p->TCSpan16[0], (u16)trow->TemperatureCalData[i].span);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}
/** \brief Generic 32-bit tempcomp HART helper (write)
\param s - a pointer to a piece of HART buffer with 32-bit tempcomp row
\param setf - a pointer to the function to set the tempcomp data
\return HART error code or HART_NO_COMMAND_SPECIFIC_ERRORS
*/
s8_least SetTempComp32(const TCRow32_t *s, ErrorCode_t (*setf)(const TemperatureCalibrationRow32_t *arg))
{
    TemperatureCalibrationRow32_t trow;
    trow.ChannelShift = util_GetU8(s->ChannelShift[0]);
    for(u8_least i=0; i<NELEM(trow.TemperatureCalData); i++)
    {
        const TCOffsetSpan32_t *p = (const void *)s->TCOffsetSpan32[i];
        trow.TemperatureCalData[i].offset = (s32)util_GetU32(p->TCOffset32[0]);
        trow.TemperatureCalData[i].span = (s32)util_GetU32(p->TCSpan32[0]);
    }
    ErrorCode_t err = setf(&trow);
    return err2hart(err);
}
/** \brief Generic 32-bit tempcomp HART helper (read)
\param trow - a pointer to the data structure to format for HART
\param[out] d - a pointer to a piece of HART buffer with 32-bit tempcomp row
\return HART error code or HART_NO_COMMAND_SPECIFIC_ERRORS
*/
s8_least GetTempComp32(const TemperatureCalibrationRow32_t *trow, TCRow32_t *d)
{
    util_PutU8(d->ChannelShift[0], trow->ChannelShift);
    for(u8_least i=0; i<NELEM(trow->TemperatureCalData); i++)
    {
        TCOffsetSpan32_t *p = (void *)d->TCOffsetSpan32[i];
        util_PutU32(p->TCOffset32[0], (u32)trow->TemperatureCalData[i].offset);
        util_PutU32(p->TCSpan32[0], (u32)trow->TemperatureCalData[i].span);
    }
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */


/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file oldapicollection.c
    \brief HART glue layer for useful factory stuff but using old API.

    When/as API gets upgraded, pieces of this file can be modularized

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/oldapicollection.c $
    $Date: 11/29/11 2:15p $
    $Revision: 5 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: oldapicollection.c $
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:15p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8313 Lint
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 5:49p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * Preliminary check-in for
 * TFS:8051 new tempcomp (removed redundancies)
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 5:10p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/15/10    Time: 12:27a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * Some operations refactored to xml (TFS:4067)
*/

#include "mnwrap.h"
#include "hartdef.h"
#include "hartfunc.h"
#include "dimensions.h"
#include "bufutils.h"
#include "hartpriv.h"
#include "cycle.h"
//#include "nvram.h"
#include "process.h"



#if 1 //def OLD_NVRAM
s8_least hartcmd_CommitRAMtoNVMEM(const u8 *src, u8 *dst) //used to be hart_FactoryCommand_132_SetVersion
{
    bool_t result;
    u8 flag;
    s8_least ret;
    const Req_CommitRAMtoNVMEM_t *s = (const void *)src;
    UNUSED_OK(dst);
    if(util_GetU8(s->NVMEMVolumeId[0]) > NVMEM_MAX_VOLID)
    {
        return HART_INVALID_DATA;
    }

    flag = util_GetU8(s->RAMCommitSuppress[0]);
    u8 flip = util_GetU8(s->RAMCommitFlip[0]);
    ProcId_t id;
    if(flag == 0)
    {
        if(flip != 0)
        {
            id = PROC_NONE;
        }
        else
        {
            id = PROC_HART_SUB132_INIT_WITHCAL;
        }
    }
    else
    {
        if(flip != 0)
        {
            id = PROC_HART_235_SAVECAL;
        }
        else
        {
            id = PROC_HART_SUB132_INIT_NOCAL;
        }
    }
    result = hart_SetProcessCommand(id);
    if(result)
    {
        ret = HART_BUSY;
    }
    else
    {
        ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    }
    UNUSED_OK(dst);
    return ret;
}

#else //ndef OLD_NVRAM

static u8 ReqVolid;
static u8 ReqCommitFlip;
static u8 ReqCommitSuppress;

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

#endif //OLD_NVRAM
/* This line marks the end of the source */

/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
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
    \file hart_logfile.c
    \brief Glue functions to support log file system

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/hart/glue/hart_logfile.c $
    $Date: 3/29/09 4:12p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_logfile.c $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 4:12p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart/glue
 * A few items moved from configure.h to pressures.h and errlimits.h
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/27/09    Time: 6:19p
 * Created in $/MNCB/Dev/FIRMWARE/interface/hart/glue
 * Logfile HART support extracted in a separate file; log file inclusion
 * is controlled by a new FEATURE_LOGFILE conditional
*/
#include "mnwrap.h"
#include "hartcmd.h"
#include "hartfunc.h"
#include "bufutils.h"
#include "hart.h"
#include "hartpriv.h"
#include "hartdef.h"
#include "logfile.h"
#include "resource.h"

/**
\brief Reads data from the a log file - used for pst et al
*/
s8_least hartcmd_ReadLogFile(const u8 *src, u8 *dst)
{
    const Req_ReadLogFile_t *s1 = (const void *)src;
    const FileRequest_t *s = (const void *)s1->FileRequest[0];
    Rsp_ReadLogFile_t *d = (void *)dst;
    UNUSED_OK(d->FileRequest); //rely on HART framework to fill

    u8 ID, Version, FullID;
    u16 Length = NELEM(d->FileData);
    u16 FilePosition;
    s8_least Ret = 0;
    ErrorCode_t Err = ERR_OK;

    //read the request data
    ID = util_GetU8(s->FileId[0]);
    Version = util_GetU8(s->FileVersion[0]);

    FullID = LOGF_FULL_ID(ID,Version);

    FilePosition = util_GetU16(s->FilePosition[0]);

    MN_FRAM_ACQUIRE();
        Err = logf_Retrieve(d->FileData[0], &Length, FullID, FilePosition);
    MN_FRAM_RELEASE();

    //mnhartcmd fills in some of the results
    if(Err != ERR_OK)
    {
       Length = (u16)-((s8_least)Err);
       Ret = HART_INVALID_DATA;
    }

    util_PutU8(d->FileReadLength[0], (u8)Length);

    return Ret;
}

/**
\brief Resets log file system - (factory)
*/
s8_least hartcmd_ResetLogFileSystem(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    s8_least Ret;
    /* outside the mutex sandwich since logf_ResetFS() acquires the mutex
    */
    ErrorCode_t err = logf_ResetFS();
    if(err!=ERR_OK)
    {
        Ret = TRANSMITTER_SPECIFIC_COMMAND_ERROR; //operation failed or incomplete
    }
    else
    {
        Ret = HART_OK;
    }
    return Ret;
}

/* This line marks the end of the source */

/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup HARTapp
\brief HART application layer component

    \file hart_dbuftest.c.c
    \brief The HART wiring functions to support diagnostic buffer sampling tests

    CPU: Any

    OWNER: AK
    \ingroup HARTapp
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufferhandler.h"
#include "bufutils.h"

/** \brief A function to stuff data in a buffer - for testing
*/
s8_least hartcmd_WriteDataToDiagBuffer(const u8 *src, u8 *dst)
{
    const Req_WriteDataToDiagBuffer_t *s = (const void *)src;
    UNUSED_OK(dst);
    u8 bufid = util_GetU8(s->BufferId[0]);
    s16 val = util_GetS16(s->BufferFill[0]);
    s8_least ret;
    if(buffer_AddXDiagnosticData1(bufid, val))
    {
        //error
        ret = TRANSMITTER_SPECIFIC_COMMAND_ERROR;
    }
    else
    {
        //no error
        ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    }
    return ret;
}

s8_least hartcmd_ReadDiagBufferSamplingInfo(const u8 *src, u8 *dst)
{
    const Req_ReadDiagBufferSamplingInfo_t *s = (const void *)src;
    Rsp_ReadDiagBufferSamplingInfo_t *d = (void *)dst;
    u8 bufid = util_GetU8(s->BufferId[0]);
    UNUSED_OK(d->BufferId); //rely on HART framework to fill
    const SamplerInfo_t *info = buffer_GetSamplerInfo(bufid);
    util_PutU16(d->SamplingInterval[0], info->prune_scale);
    util_PutU8(d->NumberOfInitialPoints[0], info->init_points);
    util_PutU16(d->NumberOfSampledPoints[0], info->num_points);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

static s16 testdata[2];
static void mysample(diag_t data[2])
{
    data[0] = testdata[0];
    data[1] = testdata[1];
}

s8_least hartcmd_WriteSampleToDiagBuffer(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_WriteSampleToDiagBuffer_t *s = (const void *)src;
    testdata[0] = util_GetS16(s->BufferFill2[0]);
    testdata[1] = util_GetS16(s->BufferFill2[1]);
    u8 bufid = util_GetU8(s->BufferId[0]);
    buffer_SampleData(bufid);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_StartSamplingIntoDiagBuffer(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_StartSamplingIntoDiagBuffer_t *s = (const void *)src;
    u8 bufid = util_GetU8(s->BufferId[0]);
    dsample_t init_stuff[20]; //20 is just for kicks; any number wil do for testing but watch the stack
    u8 init_points = util_GetU8(s->Reservedsamples[0]);

    ErrorCode_t err;
    if(init_points >= NELEM(init_stuff))
    {
        err = ERR_CAL_VALUE;
    }
    else
    {
        err = ERR_OK;
        const diag_t* p = buffer_GetXDiagnosticBuffer(bufid);
        u16 max_points = util_GetU16(s->MaxNumberOfPointsToKeep[0]);
        u8_least i;
        dsample_t *ps;
        if(init_points > 0)
        {
            for(i=0; i<init_points; i++)
            {
                init_stuff[i].data1 = *p++;
                init_stuff[i].data2 = *p++;
            }
            ps = init_stuff;
        }
        else
        {
            ps = NULL;
        }

        //The sampling must come from the same context as start command
        //(a little awkward API but oh well...)
        taskid_t context;
        if(oswrap_IsContext(TASKID_HART))
        {
            context=TASKID_HART;
        }
        else if(oswrap_IsContext(TASKID_ISPCOMM))
        {
            context=TASKID_ISPCOMM;
        }
        else if(oswrap_IsContext(TASKID_IPCCOMM))
        {
            context=TASKID_IPCCOMM;
        }
        else
        {
            context = TASKID_HART; //to keep lint quiet (but lint is wrong nonetheless)
            err = ERR_CAL_INVALID_PROCEDURE_CALL;
        }
        if(err == ERR_OK)
        {
            err = buffer_StartSampling(bufid,
                                  context,
                                  mysample,
                                  max_points,
                                  init_points,
                                  ps);
        }
    }
    return err2hart(err);
}

s8_least hartcmd_StopSamplingIntoDiagBuffer(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_StopSamplingIntoDiagBuffer_t *s = (const void *)src;
    u8 bufid = util_GetU8(s->BufferId[0]);
    buffer_StopSampling(bufid);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}
/* This line marks the end of the source */

/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_nvmclone.c
    \brief HART glue layer for debug support for NVMEM cloning

    CPU: Any

    OWNER: AK

    \ingroup HARTapp
*/
#include "mnwrap.h"
#include "clonenvram.h"
#include "hartdef.h"
#include "hartcmd.h"
#include "bufutils.h"
#include "hartfunc.h"

s8_least hartcmd_CompleteCloningNvmemToOppositeBank(const u8 *src, u8 *dst)
{
    const Req_CompleteCloningNvmemToOppositeBank_t *s = (const void *)src;
    UNUSED_OK(s);
    Rsp_CompleteCloningNvmemToOppositeBank_t *d = (void *)dst;
    ErrorCode_t err = nvmem_CloneSync();
    util_PutU8(d->CloningCompletionCode[0], (u8)err);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_CloneNvmemToOppositeBank(const u8 *src, u8 *dst)
{
    const Req_CloneNvmemToOppositeBank_t *s = (const void *)src;
    Rsp_CloneNvmemToOppositeBank_t *d = (void *)dst;
    //Rely on HART framework to fill the fields
    UNUSED_OK(d);
    UNUSED_OK(s);
    ErrorCode_t err = process_SetProcessCommand(PROC_CLONE_NVMEM);
    return err2hart(err);
}

/* This line marks the end of the source */

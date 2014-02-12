/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_diagrw.c
    \brief HART glue layer for valve diagnostic signature read/write

    CPU: Any

    OWNER: AK

    \ingroup HARTapp
*/
#include "mnwrap.h"
#include "facdefaults.h"
#include "hartdef.h"
#include "hartcmd.h"
#include "bufutils.h"
#include "hartfunc.h"

s8_least hartcmd_SaveNvmemAsFactoryDefault(const u8 *src, u8 *dst)
{
    const Req_SaveNvmemAsFactoryDefault_t *s = (const void *)src;
    UNUSED_OK(dst);
    UNUSED_OK(s);
    ErrorCode_t err = nvmem_LaunchFactorySave();
    return err2hart(err);
}

s8_least hartcmd_RestoreFactoryDefaultNvmem(const u8 *src, u8 *dst)
{
    const Req_RestoreFactoryDefaultNvmem_t *s = (const void *)src;
    Rsp_RestoreFactoryDefaultNvmem_t *d = (void *)dst;
    //Rely on HART framework to fill the fields
    UNUSED_OK(d->NvmemVolumeId);
    UNUSED_OK(d->NvmemDefaultsMaskFlags);
    UNUSED_OK(d->NvmemDefaultsMatchFlags);
    UNUSED_OK(d->NvmemDefaultsTestOnly);

    u8 volid = util_GetU8(s->NvmemVolumeId[0]);
    u8 mask = util_GetU8(s->NvmemDefaultsMaskFlags[0]);
    u8 match = util_GetU8(s->NvmemDefaultsMatchFlags[0]);
    u8 test_only = util_GetU8(s->NvmemDefaultsTestOnly[0]);
    ErrorCode_t err = nvmem_LaunchFactoryRestore(volid, mask, match, test_only);
    return err2hart(err);
}

/* This line marks the end of the source */

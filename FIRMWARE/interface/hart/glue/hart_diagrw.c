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
#include "diagrw.h"
#include "hartdef.h"
#include "hartcmd.h"
#include "bufutils.h"
#include "hartfunc.h"

s8_least hartcmd_SaveDiagnosticSignature(const u8 *src, u8 *dst)
{
    const Req_SaveDiagnosticSignature_t *s = (const void *)src;
    UNUSED_OK(dst);
    u8 SignatureType = util_GetU8(s->SignatureType[0]);
    u8 SignatureAssignment = util_GetU8(s->SignatureAssignment[0]);
    ErrorCode_t err = diag_LaunchSignatureWrite(SignatureType, SignatureAssignment);
    return err2hart(err);
}

s8_least hartcmd_ReadDiagnosticSignatureIntoDiagnosticBuffer(const u8 *src, u8 *dst)
{
    const Req_ReadDiagnosticSignatureIntoDiagnosticBuffer_t *s = (const void *)src;
    UNUSED_OK(dst);
    u8 SignatureType = util_GetU8(s->SignatureType[0]);
    u8 SignatureAssignment = util_GetU8(s->SignatureAssignment[0]);
    u8 FileVersion = util_GetU8(s->FileVersion[0]);
    u8 BufferId = util_GetU8(s->BufferId[0]);
    ErrorCode_t err = diag_LaunchSignatureRead(SignatureType, SignatureAssignment, FileVersion, BufferId);
    return err2hart(err);
}

/* This line marks the end of the source */

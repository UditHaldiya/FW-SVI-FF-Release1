/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mnhart2ff.h
    \brief API collection of MN plugins to Softing FF stack

    CPU: Any

    OWNER: AK
*/

#ifndef MNHART2FF_H_
#define MNHART2FF_H_

#include <fbif.h>
#ifndef MISRA_RECAST
//MISRA 2 requires re-cast of shifts and complements of sub-integer types
#define MISRA_RECAST(type, expr) ((type)(expr))
#endif
#include "bitutils.h"

typedef u16 fferr_t; //FF error code type

extern fferr_t err_Hart2FF(u8 harterr);
//A wrapper
extern fferr_t mn_HART_acyc_cmd(u8 cmd, u8 *send_buff, u8 send_length, u8 *receive_buff);

//Softing static (=LOCAL) cum API
//extern fferr_t appl_send_acyc_HART_cmd(u8 command);

MN_INLINE float32 mn_pull_float(const void *buf)
{
    float32 f;
    memcpy(&f, buf, sizeof(float32));
    return f;
}

MN_INLINE u32 mn_pull_u32(const void *buf)
{
    u32 f;
    memcpy(&f, buf, sizeof(u32));
    return f;
}

MN_INLINE u32 mn_pull_u16(const void *buf)
{
    u16 f;
    memcpy(&f, buf, sizeof(u16));
    return f;
}

//lint -emacro(506, mn_IsAppFault) Constant value Boolean [MISRA 2004 Rules 13.7 and 14.1] OK depending on caller
#define mn_IsAppFault(ptb, fcode) ((fcode==0)?false:(( ((const u8 *)&(ptb)->complete_status) [BYTENUM((fcode)-1U)] & (1U<<BYTEBITNUM((fcode)-1U))) != 0))

// Set of functions: detect and signal the HART Response TimeOut on FF processor.
extern void     mn_HART_ReceivedResponse(void);
extern void     mn_HART_VerifyResponseTO(void);
extern bool_t   mn_HART_ResponseTimeOut(void);

#endif //MNHART2FF_H_
/* This line marks the end of the source */

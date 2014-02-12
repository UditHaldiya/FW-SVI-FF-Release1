/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_checkprocess.c
    \brief File contains implementation of the CHECK_PROCESS FF parameter

    CPU: Any

    OWNER:
    $Date:  $
    $Revision: 1 $
    $Author: $

*/
#include <softing_base.h>
#include <ptb.h>

#include "mn_checkprocess.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"

//-----------------------------------------------------------------------------------------

/* \brief Routine called by Read_handler_PTB() to populate pPTB with the parameter
          CHECK_PROCESS
          Called when Read Handler is called with the index REL_IDX_PTB_CHECK_PROCESS.
    param in:
        p_block_instance:   pointer to the FF block
        p_read:             pointer to parameters for reading (not used)
        from:               pointer to the HART send buffer
    param out:
        to:                 rcv_buf:            pointer to the HART receive buffer
    return:
        error code: OK if succeeds, ERROR if fails
*/
fferr_t chkpro_CheckProcess(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to)
{
    UNUSED_OK(p_read);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    fferr_t fferr = mn_HART_acyc_cmd(251, from, 0, to);
    if(fferr == E_OK)
    {
        Rsp_CheckProcess_t *d = (void *)((u8 *)to + 2);
        p_PTB->check_process.component_0 = util_GetU8(d->ProcessId[0]);
        p_PTB->check_process.component_1 = util_GetU8(d->PercentComplete[0]); //FF param is u32, but HART is u8
    }
    return fferr;
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly



/* This line marks the end of the source */

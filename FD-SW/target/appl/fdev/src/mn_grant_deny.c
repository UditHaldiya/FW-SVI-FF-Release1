/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_grant_deny.c
    \brief implement GRANT_DENY parameter in RB block

    CPU: Any

    OWNER: EJ
    $Date: 11/01/12 11:19a $
    $Revision: 1 $
    $Author: Stanley Fu $

*/
/* $History: mn_grant_deny.c $
 *
 * *****************  Version 1  *****************
 * User: Stanley Fu    Date: 11/1/12    Time: 10:38p
 * Init version
*/

#include <softing_base.h>
#include <resb.h>
#include "mnhart2ff.h"
#include "mn_grant_deny.h"

/* \brief This function keep GRANT_DENY consistent
    param in:
        p_PTB: pointer to the objects of transducer block
        p_write: pointer to write structure
    return:
        FF error
*/
fferr_t WriteGrantDeny(const T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA* p_write)
{
    fferr_t fferr = E_OK;
    u8 and_result;

    if(1 == p_write->subindex)
    {
        T_FBIF_RESB* p_RES = p_block_instance->p_block_desc->p_block;
        u8 grant = *(p_write->source);
        and_result = grant & p_RES->grant_deny.denied;
        if(0 != and_result)
        {
            fferr = E_ERROR;
        }
    }
    else if(2 == p_write->subindex)
    {
        T_FBIF_RESB* p_RES = p_block_instance->p_block_desc->p_block;
        u8 deny = *(p_write->source);
        and_result = deny & p_RES->grant_deny.grant;
        if(0 != and_result)
        {
            fferr = E_ERROR;
        }
    }
    else
    {
        ACCESS_PERM* p_grant_deny = (ACCESS_PERM*)(void*)p_write->source;
        and_result = p_grant_deny->grant & p_grant_deny->denied;
        if(0 != and_result)
        {
            fferr = E_ERROR;
        }
    }

    return fferr;
}


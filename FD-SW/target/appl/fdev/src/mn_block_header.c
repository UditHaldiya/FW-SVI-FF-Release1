/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_actuator_3.c
    \brief check the BLOCK_HEADER when it is written.

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include <ptb.h>

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "crc.h"

//Glue headers last
#include "mnhart2ff.h"
#include "mn_block_header.h"

/* \brief write BLOCK_HEADER parameter
    param in:
        p_block_instance: pointer to function block
        p_write: point to the write structure
    return:
        FF error
*/
fferr_t WriteBlockHeader(const T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA *p_write)
{
    fferr_t fferr = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    STRINGV* p_block_tag = NULL;

    if(p_PTB->mode_blk.actual != MODE_OS && p_PTB->mode_blk.actual != MODE_LO)
    {
        switch (p_write->subindex)
        {
        case 0:
            {
                F_BLOCK* p_block_header = (F_BLOCK*)(void*)p_write->source;
                p_block_tag = p_block_header->block_tag;
                break;
            }
        case 1:
            {
                p_block_tag = (STRINGV*)(void*)p_write->source;
                break;
            }
        default:
            {
            }
        }
        for(u16 i=0; p_block_tag != NULL && i<NELEM(p_PTB->blk_data.block_tag); i++)
        {
            if(p_block_tag[i] != p_PTB->blk_data.block_tag[i])
            {
                fferr = E_ERROR;
                break;
            }
        }
    }
    return fferr;
}


/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mndoswff.c
    \brief Handling of control limits interface

    CPU: Any

    OWNER: AK
*/


//Softing headers first
#include <softing_base.h>
#include <ptb.h>

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

#include "mnassert.h"
#include "mn_errorbits.h"

//----------------------------------------------------------------
// CONST ASSERT to verify that we fit in the parameter

#define BITS_IN_U32                 (32u)

CONST_ASSERT((u32)MN_ERR_CONDITION_MAX < BITS_IN_U32);

//----------------------------------------------------------------
// Local Storage area
// Initialize the bitmap to 0
static  u32     LocalErrorBitMap = 0u;

//---------------------- runtime part ----------------------------
static u32 GetClearBitMask(void)
{
    mn_ErrorConditionCode_t i;
    u32                     ErrorBitMaspClearMask;

    ErrorBitMaspClearMask = 0u;

    for (i = ERRB_ERROR_CONDITION_0;
         i < MN_ERR_CONDITION_MAX;
         i++)
    {
        ErrorBitMaspClearMask |= 1u << (u32)i;
    }

    ErrorBitMaspClearMask = ~ErrorBitMaspClearMask;
    return ErrorBitMaspClearMask;
}

// In this function we get the Final Value ***, both Value and status, and Channel
void mn_ErrorBits_Set(mn_ErrorConditionCode_t   ErrCondition,
                      bool_t                    SetErrorState)
{
    u32     SetBitMap;

    // Make sure the Caller ID is correct!
    MN_ASSERT(ErrCondition < MN_ERR_CONDITION_MAX);

    MN_ENTER_CRITICAL();
        SetBitMap = 1u << (u32)ErrCondition;

        if (SetErrorState)
        {
            LocalErrorBitMap |= SetBitMap;
        }
        else
        {
            LocalErrorBitMap &= ~SetBitMap;
        }
    MN_EXIT_CRITICAL();
}

// In this function we get the Final Value ***, both Value and status, and Channel
void mn_ErrorBits_Process(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    T_FBIF_PTB *       p_PTB;
    u32                 ClearBitMask;
    u32                 SetBitMask;
    u32                 TempVal;

    p_PTB = p_block_instance->p_block_desc->p_block;

    ClearBitMask = GetClearBitMask();
    SetBitMask   = LocalErrorBitMap;

    MN_ENTER_CRITICAL();
        TempVal      = p_PTB->block_err_desc_1;
        TempVal     &= ClearBitMask;
        TempVal     |= SetBitMask;

        p_PTB->block_err_desc_1 = TempVal;
    MN_EXIT_CRITICAL();
}

/* This line marks the end of the source */

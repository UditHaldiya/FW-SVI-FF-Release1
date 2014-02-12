/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mnctllimff.h
    \brief API of MN implementations of position control limits

    CPU: Any

    OWNER: AK
*/
#ifndef MN_ERRORBITS_H_
#define MN_ERRORBITS_H_

// Definitions for the WRONG CONFIGURATION PARAMETER

typedef enum
{
    ERRB_ERROR_CONDITION_0              = 0u,
    // Do not modify or move this enum
    //----------------------------------------------------------------

    ERRB_MODE_SWITCH,
    ERRB_HART_RESPONSE_TIMEOUT,
    SP_SOURCE_ERRORBIT,                 // For SK
    ERRB_ERROR_FIND_STOPS,

    // Insert additional Error conditionas above the line
    //----------------------------------------------------------------
    MN_ERR_CONDITION_MAX

} mn_ErrorConditionCode_t;

#define    ERRB_MOD_RB_MODE     (ERRB_ERROR_CONDITION_0)

extern void mn_ErrorBits_Set(mn_ErrorConditionCode_t ErrCondition, bool_t SetErrorState);\
extern void mn_ErrorBits_Process(const T_FBIF_BLOCK_INSTANCE *p_block_instance);

#endif // MN_ERRORBITS_H_
/* This line marks the end of the source */

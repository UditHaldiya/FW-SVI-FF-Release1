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
#ifndef MN_FSTATE_H_
#define MN_FSTATE_H_

#include "mnhart2ff.h"
#include "ptb.h"

// Definitions for the FSTATE
#define FULL_MAX_SETPOINT                       (175.0f)
#define FULL_MIN_SETPOINT                       (-5.0f)

#define FSTATE_CONFIGURATION_LOCALCFG           (2u)
#define FSTATE_CONFIGURATION_COPYCFG            (1u)

// The Action values:
// 0..3 - defined and used
// 4..7 - Reserved
// 8..255 - Mfg specific
#define FSTATE_ACTION_HOLD_LAST_VALUE           (0u)
#define FSTATE_ACTION_FAIL_CLOSED               (1u)
#define FSTATE_ACTION_FAIL_OPEN                 (2u)
#define FSTATE_ACTION_FSTATE_VALUE              (3u)
#define FSTATE_ACTION_RESERVED_1                (4u)
#define FSTATE_ACTION_RESERVED_2                (5u)
#define FSTATE_ACTION_RESERVED_3                (6u)
#define FSTATE_ACTION_RESERVED_4                (7u)
#define FSTATE_ACTION_MFG_SPECIFIC_START        (8u)
#define FSTATE_ACTION_MAX                       (255u)

extern  fferr_t     ff_fstate_VerifyWriteConf(const T_FBIF_BLOCK_INSTANCE *p_block_instance, const T_FBIF_WRITE_DATA *p_write);
extern  fferr_t     ff_fstate_PostProcessWriteConf(const T_FBIF_BLOCK_INSTANCE *p_block_instance, const T_FBIF_WRITE_DATA *p_write);
extern  void        ff_fstate_Execute_fromFB(const T_FBIF_BLOCK_INSTANCE *p_block_instance, u16 Channel);
extern  void        ff_fstate_StoreLastSP(const FLOAT_S *sp);
extern  bool_t      ff_fstate_ForceFstate_SP(const T_FBIF_PTB *p_PTB, FLOAT_S *dst);
extern  void        ff_fstate_Execute(const T_FBIF_BLOCK_INSTANCE *p_block_instance);
extern  void        ff_fstate_Background(const T_FBIF_BLOCK_INSTANCE *p_block_instance);

#endif //MN_FSTATE_H_
/* This line marks the end of the source */

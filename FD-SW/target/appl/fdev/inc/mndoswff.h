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
#ifndef MNDOSWFF_H_
#define MNDOSWFF_H_

#include "mnhart2ff.h"
#include "doswitch.h"

enum
{
    FSWITCH_DISABLE = 0,
    FSWITCH_DO_BLOCK = 1,
    FSWITCH_FAULT_STATE = 2,
    FSWITCH_NOT_IN_NORMAL = 3,
    FSWITCH_MAINTENANCE_REQ = 4,
    FSWITCH_WARNING_DATA = 5,
    FSWITCH_AIR_SUPPLY_ALERT = 6,
    FSWITCH_TRAVEL_DEVIATION_ALERT = 7,
    FSWITCH_POSITION_HI_ALERT = 8,
    FSWITCH_POSITION_LO_ALERT = 9,
    FSWITCH_ALWAYSON = 10,
    FSWITCH_ALWAYSOFF = 11,
    FSWITCH_RESET = 12,
    FSWITCH_CUTOFF = 13,
};

extern fferr_t ffdosw_ReadConfiguration(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to);
extern fferr_t ffdosw_WriteConfiguration(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);

extern void ffdosw_Runtime(const T_FBIF_BLOCK_INSTANCE *p_block_instance, u8 swvalue[DO_NUM_CHANNELS]);

extern void ffdosw_SetValue(u8 switchnum, const DISCRETE_S *val);
extern void ffdosw_TbBackgroundPatrol(void);

extern void tb_DOsw1(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val);
extern void tb_DOsw2(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val);

#endif //MNDOSWFF_H_
/* This line marks the end of the source */

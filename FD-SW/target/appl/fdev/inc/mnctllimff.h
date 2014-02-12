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
#ifndef MNCTLLIMFF_H_
#define MNCTLLIMFF_H_

#include "mnhart2ff.h"

extern fferr_t ffctllim_WriteCutoffHi
(
    T_FBIF_BLOCK_INSTANCE *p_block_instance,
    const T_FBIF_WRITE_DATA *p_write,
    void *from,
    void *to
);
extern fferr_t ffctllim_WriteCutoffLo
(
    T_FBIF_BLOCK_INSTANCE *p_block_instance,
    const T_FBIF_WRITE_DATA *p_write,
    void *from,
    void *to
);
extern fferr_t ffctllim_WriteControlLimits
(
    T_FBIF_BLOCK_INSTANCE *p_block_instance,
    const T_FBIF_WRITE_DATA *p_write,
    void *from,
    void *to
);
extern fferr_t ffctllim_ReadControlLimits
(
    T_FBIF_BLOCK_INSTANCE *p_block_instance,
    T_FBIF_READ_DATA *p_read,
    void *from,
    void *to
);

#endif //MNCTLLIMFF_H_
/* This line marks the end of the source */

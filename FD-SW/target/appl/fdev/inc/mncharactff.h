/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mncharactff.h
    \brief API of MN implementations of position characterization

    CPU: Any

    OWNER: AK
*/
#ifndef MNCHARACTFF_H_
#define MNCHARACTFF_H_

#include "mnhart2ff.h"

extern fferr_t ffcharact_WriteSelection(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);
extern fferr_t ffcharact_ReadSelection(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to);
extern fferr_t ffcharact_WriteCustomCharacterization(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);
extern fferr_t ffcharact_ReadCharacterizationCurve(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to);
extern fferr_t ffcharact_InitCustomCharacterization(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);


#endif //MNCHARACTFF_H_
/* This line marks the end of the source */

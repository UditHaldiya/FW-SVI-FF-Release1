/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_checkprocess.c
    \brief Include file for implementation of the CHECK_PROCESS FF parameter
    CPU: Any

    OWNER: SK
*/
#ifndef  _MN_CHECKPROCESS_H_
#define  _MN_CHECKPROCESS_H_

#include "mnhart2ff.h"

extern fferr_t chkpro_CheckProcess(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to);

#endif //_MN_CHECKPROCESS_H_

/* This line marks the end of the source */

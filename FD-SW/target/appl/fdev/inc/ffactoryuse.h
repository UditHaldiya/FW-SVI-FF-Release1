/*
Copyright 2013 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ffactoryuse.h
    \brief API for factory_use_{1,2} support in FFP

    HART pass-through and FFP intercepts
*/

#ifndef FFACTORYUSE_H_
#define FFACTORYUSE_H_

#include "mnhart2ff.h"

extern fferr_t ffact_WriteFactoryUse1(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t ffact_ReadFactoryUse1or2(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);

#endif //FFACTORYUSE_H_
/* This line marks the end of the source */

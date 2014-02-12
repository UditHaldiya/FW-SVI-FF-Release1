/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file clonenvram.h
    \brief API for Copy NVMEM to the opposite bank

    CPU: Any

    OWNER: AK

    \ingroup NVRAM
*/

#ifndef CLONENVRAM_H_
#define CLONENVRAM_H_
#include "errcodes.h"
#include "process.h"
extern ErrorCode_t nvmem_CloneSync(void);
extern procresult_t nvmem_Clone(s16 *procdetails);

#endif //CLONENVRAM_H_
/* This line marks the end of the source */

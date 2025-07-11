/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file diagrw.h
    \brief API for valve diagnostic signature read/write

    CPU: Any

    OWNER: AK

    \ingroup Diagnostics
*/

#ifndef FACDEFAULTS_H_
#define FACDEFAULTS_H_
#include "errcodes.h"
#include "process.h"
extern ErrorCode_t nvmem_LaunchFactorySave(void);
extern ErrorCode_t nvmem_LaunchFactoryRestore(u8 volid, u8 mask_flags, u8 match_flags, u8 test_only);
extern procresult_t nvmem_SaveAsFactoryDefaults(s16 *procdetails);
extern procresult_t nvmem_RestoreFactoryDefaults(s16 *procdetails);

#endif //FACDEFAULTS_H_
/* This line marks the end of the source */

/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file procshims.h
    \brief Necessary wrappers for legacy or normal mode processes

    CPU: Any

    OWNER: AK

    \ingroup Process
*/
#ifndef PROCSHIMS_H_
#define PROCSHIMS_H_
#include "procpriv.h"
#ifdef OLD_NVRAM
extern procresult_t nvram_PageTest_SHIM(s16 *procdetails);
#endif
extern procresult_t tune_Run_Selftune_SHIM(s16 *procdetails);
extern procresult_t diag_Run_StepTest_SHIM(s16 *procdetails);
extern procresult_t hart_procRamCommitNoCal_SHIM(s16 *procdetails);
extern procresult_t hart_procRamCommitWithCal_SHIM(s16 *procdetails);
extern procresult_t comp_SaveAllComp_SHIM(s16 *procdetails);

#endif //PROCSHIMS_H_

/* This line marks the end of the source */

/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file proc4uisimple.c
    \brief Necessary wrappers for normal mode processes

    CPU: Any

    OWNER: AK

    \ingroup Process
*/

#ifndef PROC4UISIMPLE_H_
#define PROC4UISIMPLE_H_
#include "process.h" //for procresult_t
//Good shims
extern procresult_t cal_RunFindStops_Normal(s16 *procdetails);
extern procresult_t tune_Run_Selftune_Normal(s16 *procdetails);
extern procresult_t osadj_RunOpenStopAdjustment_Normal(s16 *procdetails);     //TFS:4288
extern procresult_t osadj_RunOpenStopAdjustment_SHIM(s16 *procdetails);       //TFS:4288

#endif //PROC4UISIMPLE_H_
/* This line marks the end of the source */

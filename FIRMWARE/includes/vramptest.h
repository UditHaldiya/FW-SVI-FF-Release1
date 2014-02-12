/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file vsteptest.h
    \brief API of Generic engine for (virtual) step test

    CPU: Any

    OWNER: AK, LS

    \ingroup Diagnostics
*/
#ifndef VRAMPTEST_H_
#define VRAMPTEST_H_

#include "bufferhandler.h"
#include "process.h"
#include "position.h"
#include "taskids.h"

extern procresult_t diag_Prepare_VRampTest(bool_t bNoSetup, pos_t StartPosition, pos_t EndPosition);
extern procresult_t diag_Perform_VRampTest(pos_t sp_speed,
                                           pos_t StartPosition,
                                           pos_t EndPosition
                                             );

/* From AP 3.2.1 REQ 25-13 cause test to wait if error excessive */
//#define FINAL_STEP_ERR_THRSH INT_PERCENT_OF_RANGE(3.0)


#endif //VRAMPTEST_H_
/* This line marks the end of the source */

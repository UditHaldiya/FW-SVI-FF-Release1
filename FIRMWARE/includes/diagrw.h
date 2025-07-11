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

#ifndef DIAGRW_H_
#define DIAGRW_H_
#include "errcodes.h"
#include "process.h"
extern ErrorCode_t diag_LaunchSignatureWrite(u8 SignatureType, u8 SignatureAssignment);
extern ErrorCode_t diag_LaunchSignatureRead(u8 SignatureType, u8 SignatureAssignment, u8 FileVersion, u8 BufferId);
extern procresult_t diag_WriteBuffer(s16 *procdetails);
extern procresult_t diag_ReadBuffer(s16 *procdetails);

//Signature types
#define DIAGRW_EXT_SIGNATURE 0

//Signature destinations
#define DIAGRW_CURRENT 0
#define DIAGRW_BASELINE 1
#define DIAGRW_USER 2




#endif //DIAGRW_H_
/* This line marks the end of the source */

/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hartpriv.h
    \brief Private HART stuff

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/hartpriv.h $
    $Date: 1/23/12 2:20p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup HART
*/
/* $History: hartpriv.h $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/23/12    Time: 2:20p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * TFS:8762 Units mapping
*/
#ifndef HARTPRIV_H_
#define HARTPRIV_H_

#include "bufutils.h"
#include "unitsids.h"
#include "process.h"
#include "hart.h"
#include "oswrap.h"

extern const UnitsId_t HartUnitsTab[];
extern const u8 HartUnitsTab_size;
MN_DECLARE_API_FUNC(hc_WriteFixedAO)
extern s8_least hc_WriteFixedAO(const fpbuf_t in, fpbuf_t out, UnitsId_t uid, u8 aochannel);

extern bool_t hart_SetProcessCommand(ProcId_t procId);
#if HART_NUM_CHANNELS > 1
extern void command(u8 hart_channel, const HartData_t *hdata);
extern void hart_DoResetRequest(taskid_t taskid);
#else
//Those functions are static in the source file
#endif

//Move more HART-private stuff here

#endif //HARTPRIV_H_
/* This line marks the end of the source */

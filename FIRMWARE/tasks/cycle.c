/*
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file cycle.c
    \brief Contains the cycle task

    CPU: Any (with standard C compiler)

    OWNER: AK
*/


#include "mnwrap.h"
#include "cycle.h"
#include "refvoltage.h"

#define REF_ERR_MAX     5
static u8   refVoltageFailCount;

void periodicjob_RefVoltage(void)
{
    RefVoltage_t refVol;
    u32          rawData;

    (void)refvoltage_GetData(&refVol);
    rawData = (u32)bios_GetAdRawData()->AdValue[AD_DIAG];
    if (  (rawData > refVol.HiLimit)
        ||(rawData < refVol.LoLimit) )
    {
        refVoltageFailCount++;
        if (refVoltageFailCount >= REF_ERR_MAX)
        {
            error_SetFault(FAULT_REF_VOLTAGE);
        }
    }
    else
    {
        refVoltageFailCount = 0;
    }
}

/** \brief The main body of the Cycle Task

  \param[in] arg - a handle to the Cycle Task semaphore
*/
void cycle_CycleTask(void *arg)
{
    UNUSED_OK(arg);
    oswrap_task(TASKID_CYCLE, &taskjob_cycle);
}


/* end of the source */

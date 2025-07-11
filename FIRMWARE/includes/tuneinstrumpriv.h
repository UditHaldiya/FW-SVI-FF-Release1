/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file tuneinstrumpriv.h
    \brief Private instrumentation handling to hide ifdef logic

    CPU: Any

    OWNER: DZ
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Justin Shriver $
*/
#ifndef TUNEINSTRUMPRIV_H_
#define TUNEINSTRUMPRIV_H_

//This file hides this ugliness so people are less likely to find it
#ifdef OPTIONAL_TUNE_DIAG_FLAG
  #define OPTIONAL_TUNE_DIAG 1
#else
  #define OPTIONAL_TUNE_DIAG 0
#endif

#define MAX_STEPS 20
#define NUM_DIR 2

#define PID_PHASE_START 0
#define PID_PHASE_OPEN_START 1
#define PID_PHASE_OPEN_END 2
#define PID_PHASE_RAMP_FCN_START 3
#define PID_PHASE_RAMP_FCN_END 4
#define PID_PHASE_TUNE_CLOSED_START 5
#define PID_PHASE_TUNE_CLOSED_END 6
#define PID_PHASE_END 7

typedef struct ExtAnalysAutoTune_t
{
    /* eaat_req_step part of HART command*/
    u16 step;
    u16 InitBias;
    u16 gainInit[NUM_DIR];
    tick_t dTime;
    u16 BiasLow;
    u16 BiasHigh;
    /*gainRuns not currently coded*/
    u16 gainRuns;
    /*curItr not currently coded*/
    u16 curItr;
    u8 openLoopTests;
    u8 RampLoops;
    u8 closedLoops;
    tick_t Times[MAX_STEPS];
    /*eaat_TimeLastStep, eaat_TimeElapased  created from Times*/
    tick_t DeltaTime[MAX_STEPS];
    s16 Delta[MAX_STEPS];
    s16 Inc[MAX_STEPS];
    u16 gainP[MAX_STEPS];
}ExtAnalysAutoTune_t;

extern ExtAnalysAutoTune_t extDataAutoTune;








#endif //TUNEINSTRUMPRIV_H_
/* This line marks the end of the source */

/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file control.h
    \brief Public interfaces of the Control Module

    CPU: Any

    $Revision: 73 $
    OWNER: JS
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/control.h $
    $Date: 1/06/12 4:36p $
    $Revision: 73 $
    $Author: Arkkhasin $

    \ingroup poscontrol
*/
/* $History: control.h $
 *
 * *****************  Version 73  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 4:36p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * include "errcodes.h" FBO TFS:8577
 *
 * *****************  Version 72  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 1:59p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8313 Lint
 *
 * *****************  Version 71  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 4:30p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 69  *****************
 * User: Justin Shriver Date: 5/13/11    Time: 5:44p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4923 Autotune Performance, New Interface
 *
 * *****************  Version 68  *****************
 * User: Anatoly Podpaly Date: 2/17/11    Time: 3:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5607 -- moved Low Power handling code to SYSIO layer.
 *
 * *****************  Version 67  *****************
 * User: Arkkhasin    Date: 12/03/10   Time: 2:29a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4922 - ctltask.c moved and compiles conditionally
 *
 * *****************  Version 66  *****************
 * User: Arkkhasin    Date: 10/22/10   Time: 2:51a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4206 fix - Step 2: (a, also TFS:2514) checksums in rate limits
 * stuff which moved to ctllimits.c (b) removed /some/ compiled-out code
 * (c, FOB TFS:2616) rate limits can be reset to any setpoint
 *
 *
 * *****************  Version 65  *****************
 * User: Arkkhasin    Date: 10/21/10   Time: 5:25p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4206 fix - Step 1: Removed unused (and confusing) mode
 * CONTROL_MANUAL_SIG.
 * Also, standard tomstone header.
*/
#ifndef CONTROL_H_
#define CONTROL_H_

/** Public functions
*/
#include "errcodes.h"
#include "posctlmode.h"
#include "crc.h" //for CHECKFIELD

typedef struct BiasData_t
{
    u16 BiasSaved;
    CHECKFIELD;
} BiasData_t;

typedef struct BiasExt_t
{
    u16 uiBiasShift;
    s16 nBiasAddAirLoss;
    s16 nBiasAdd;
    s16 nBiasTempCoef;
    u16 uiMaxHysteresis;
    CHECKFIELD;
} BiasExt_t;

typedef struct PosErr_t
{
    s16 err;
    s16 err1;
    s16 err2;
    s16 err3;
    s16 err4;
    s16 err5;
    s16 err6;
    s16 err7;
    s16 err8;
    s16 err_abs;
} PosErr_t;

typedef struct ContinuousDiagnostics_t
{
    u32 TotalTravelCntr;
    u32 TimeClosedCntr;
    u32 TimeOpenCntr;
    u32 TimeNearCntr;
    u32 CyclesCntr;
    CHECKFIELD;
} ContinuousDiagnostics_t;


void control_CheckSaveBias(void);
void control_ContinuousDiagnostics(void);
/* void control_SetPositionErrorFlag(void); */

void control_GetControlMode(ctlmode_t* pn1ControlMode, s32* pn4Setpoint);
void control_GetSetpointPosition(s32* pn4Setpoint , s32* pn4Position);
u16 control_GetBias(void);

s16 control_GetPosition(void);

//TFS:8334  Removed obsolete function declarations (not referenced)
u8 control_GetBiasChangeFlag(void);
void control_ResetBiasChangeFlag(void);
const PosErr_t* control_GetPosErr(void);
extern void control_Control(void); //the voodoo is all there

extern ErrorCode_t posmon_SetContinuousDiagnostics(const ContinuousDiagnostics_t *src);
extern const ContinuousDiagnostics_t* posmon_GetContinuousDiagnostics(ContinuousDiagnostics_t *dst);
UNTYPEACC(posmon_SetContinuousDiagnostics, posmon_GetContinuousDiagnostics);
#define control_GetContinuousDiagnostics() (posmon_GetContinuousDiagnostics(NULL)) //deprecated
#define control_ClearContinuousDiagnostics() ((void)posmon_SetContinuousDiagnostics(NULL)) //deprecated

extern ErrorCode_t control_SetBiasExt(const BiasExt_t* pBiasExt);
extern const BiasExt_t *control_GetFillBiasExt(BiasExt_t *dst);
UNTYPEACC(control_SetBiasExt, control_GetFillBiasExt);
#define control_GetBiasExt() (control_GetFillBiasExt(NULL)) //deprecated

extern const BiasData_t *control_GetNVMEMBiasData(BiasData_t* dst);
extern ErrorCode_t control_SetNVMEMBiasData(const BiasData_t* pBiasData);
UNTYPEACC(control_SetNVMEMBiasData, control_GetNVMEMBiasData);

extern void control_RunOnce(void);

#ifdef OLD_NVRAM
void control_SaveContinuousDiagnostics(void);
extern void  control_InitControlData(InitType_t Type);
void  control_SaveControlDiagData(void);
#endif

#define PT1_PCT_16          ((s16)(((float32)STANDARD_RANGE * 0.001F) + 0.5F))
#define PT15_PCT_24         ((s16)(((float32)STANDARD_RANGE * 0.0015F) + 0.5F))
#define PT2_PCT_32          ((s16)(((float32)STANDARD_RANGE * 0.002F) + 0.5F))
#define PT3_PCT_49          ((s16)(((float32)STANDARD_RANGE * 0.003F) + 0.5F))
#define HALF_PCT_82         ((s16)(((float32)STANDARD_RANGE * 0.005F) + 0.5F))
#define PT6_PCT_98          ((s16)(((float32)STANDARD_RANGE * 0.006F) + 0.5F))
#define ONE_PCT_164         ((s16)(((float32)STANDARD_RANGE * 0.01F) + 0.5F))                 //
#define TWO_PCT_328         ((s16)(((float32)STANDARD_RANGE * 0.02F) + 0.5F))
#define THREE_PCT_491       ((s16)(((float32)STANDARD_RANGE * 0.03F) + 0.5F))
#define FOUR_PT7_PCT_770    ((s16)(((float32)STANDARD_RANGE * 0.047F) + 0.5F))
#define FIVE_PCT_819        ((s16)(((float32)STANDARD_RANGE * 0.05F) + 0.5F))
#define FIVE_PT3_PCT_868    ((s16)(((float32)STANDARD_RANGE * 0.053F) + 0.5F))
#define TWENTY_PCT_3277     ((s16)(((float32)STANDARD_RANGE * 0.2F) + 0.5F))
//#define THIRTY_PCT_4915     ((s16)(((float32)STANDARD_RANGE * 0.3F) + 0.5F))
#define FIFTY_PCT_8192     ((s16)(((float32)STANDARD_RANGE * 0.5F) + 0.5F))

#define COMP_BASE    15
#define COMP_MAX     21

#define OUT_LOW_LIMIT 3000u
#define OUT_HIGH_LIMIT 65000u
#define BIAS_LOW_LIMIT 8000u
#define BIAS_HIGH_LIMIT 45000u
#define BIAS_ALARM_LOW_LIMIT 10000u
#define BIAS_ALARM_HIGH_LIMIT 35000u

/* guarded output  */
extern void control_SetPWM(u16_least PWMValue);

#define MIN_EFFECTIVE_SETPOINT (-FIFTY_PCT_8192)
#define MAX_EFFECTIVE_SETPOINT (MAX_ALLOWED_POSITION - JIGGLE_AMOUNT) //not terribly modular: depends on jiggle test particulars

//Jiggle ("in-control") test parameters
#define JIGGLE_AMOUNT               INT_PERCENT_OF_RANGE(.08F) //! amount to move alter the setpoint


extern u16 control_GetControlOutput(void);

#endif //CONTROL_H_
/* This line marks the end of the source */

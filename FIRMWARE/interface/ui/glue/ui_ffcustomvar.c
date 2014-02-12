/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_ffcustomvar.c
    \brief UI service functions for "foundation field bus" support (MNCB)

     CPU: Any

    OWNER: EJ
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_ffcustomvar.c $
    $Date: 1/09/12 3:14p $
    $Revision: 1 $
    $Author: EricJ $

    \ingroup UI
*/
/* $History: ui_ffcustomvar.c $

 * *****************  Version 1  *****************
 * User: Eric J    Date: 2/07/12    Time: 3:29p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * First version
*/
#include "mnwrap.h"
#include "numutils.h"
#include "ui_config.h"
#include "uidef.h"
#include "uishow.h"
#include "lcd.h"
#include "ui_ffcustomvar.h"
#include "ff_resblockvars.h"
#include "ff_aoblockvars.h"
#include "ff_doblockvars.h"
#include "ff_do2blockvars.h"
#include "ff_pidblockvars.h"
#include "ff_pid2blockvars.h"
#include "ff_tbblockvars.h"
#include "ff_aiblockvars.h"
#include "ff_ai2blockvars.h"
#include "ff_ai3blockvars.h"
#include "ff_isblockvars.h"
#include "bufutils.h"

/**\brief show NULL for string initial value
*/
static void check_initial_string(u8* str, u8 len)
{
	if(0 == str[0])
	{
		p8 null_buf[] = "    NULL";
		util_GetU8Array((u8*)null_buf, len, str);
	}
}

/** \brief called by ui enable list when display check if node is enabled
        by custom 1
*/
const void *ui_CycleMenuGetFfCustom1(void)
{
    const IPC_FFTBParams_t* pVar;

    pVar = GetTbBlockVar();
    return &(pVar->cust_config_0);
}

/** \brief called by ui enable list when display check if node is enabled
        by custom 2
*/
const void *ui_CycleMenuGetFfCustom2(void)
{
    const IPC_FFTBParams_t* pVar;

    pVar = GetTbBlockVar();
    return &(pVar->cust_config_1);
}


/* local invoked, display variable status at line 3
    #define SQ_BAD                       (0 * 0x40)
    #define SQ_UNCERTAIN                 (1 * 0x40)
    #define SQ_GOOD_NC                   (2 * 0x40)
    #define SQ_GOOD_CAS                  (3 * 0x40)
    #define SQ_GOOD                      (2 * 0x40)
*/
static void ui_dispFfVarStatus(u8 status)
{
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1] = BLANK_STR_LCD_1LINE;

    status = status & IPC_QUALITY_MASK;
    switch (status)
    {
    case IPC_QUALITY_BAD:
        mn_memcpy((void*)ln2, FFSTR_VARSTATUS_BAD, NUMBER_OF_DIGITS_PER_LINE);
        break;
    case IPC_QUALITY_UNCERTAIN:
        mn_memcpy((void*)ln2, FFSTR_VARSTATUS_UNCERTAIN, NUMBER_OF_DIGITS_PER_LINE);
        break;
    case IPC_QUALITY_GOODCASCADE:
        mn_memcpy((void*)ln2, FFSTR_VARSTATUS_GOODCAS, NUMBER_OF_DIGITS_PER_LINE);
        break;
    case IPC_QUALITY_GOOD:
        mn_memcpy((void*)ln2, FFSTR_VARSTATUS_GOOD, NUMBER_OF_DIGITS_PER_LINE);
        break;
    default:
        break;
    }
    ui_setDispmode(DISPMODE_FF_INFO, ln2);
}

/** \brief called by ui variable node
        to get the value for display, index 1
*/
u8 ui_ffGetWorkingSP(u8 * str, u8 maxlen)
{
    IPC_FFTBParams_t* pvar;

    pvar = GetTbBlockVar();
    ui_dispFfVarStatus(pvar->working_sp.status);
	util_GetU8Array(pvar->working_sp.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 2
*/
u8 ui_ffGetWorkingPos(u8 * str, u8 maxlen)
{
    IPC_FFTBParams_t* pvar;

    pvar = GetTbBlockVar();
    ui_dispFfVarStatus(pvar->working_pos.status);
	util_GetU8Array(pvar->working_pos.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 3
*/
u8 ui_ffGetPidSp(u8 * str, u8 maxlen)
{
    IPC_FFPIDParams_t* pvar;

    pvar = GetPidBlockVar();
    ui_dispFfVarStatus(pvar->sp.status);
	util_GetU8Array(pvar->sp.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 4
*/
u8 ui_ffGetPidPv(u8 * str, u8 maxlen)
{
    IPC_FFPIDParams_t* pvar;

    pvar = GetPidBlockVar();
    ui_dispFfVarStatus(pvar->pv.status);
	util_GetU8Array(pvar->pv.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 5
*/
u8 ui_ffGetPidOut(u8 * str, u8 maxlen)
{
    IPC_FFPIDParams_t* pvar;

    pvar = GetPidBlockVar();
    ui_dispFfVarStatus(pvar->out.status);
	util_GetU8Array(pvar->out.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 6
*/
u8 ui_ffGetPid2Sp(u8 * str, u8 maxlen)
{
    IPC_FFPID2Params_t* pvar;

    pvar = GetPid2BlockVar();
    ui_dispFfVarStatus(pvar->sp.status);
	util_GetU8Array(pvar->sp.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 7
*/
u8 ui_ffGetPid2Pv(u8 * str, u8 maxlen)
{
    IPC_FFPID2Params_t* pvar;

    pvar = GetPid2BlockVar();
    ui_dispFfVarStatus(pvar->pv.status);
	util_GetU8Array(pvar->pv.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 8
*/
u8 ui_ffGetPid2Out(u8 * str, u8 maxlen)
{
    IPC_FFPID2Params_t* pvar;

    pvar = GetPid2BlockVar();
    ui_dispFfVarStatus(pvar->out.status);
	util_GetU8Array(pvar->out.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 9
*/
u8 ui_ffGetAi1Out(u8 * str, u8 maxlen)
{
    IPC_FFAIParams_t* pvar;

    pvar = GetAiBlockVar();
    ui_dispFfVarStatus(pvar->out.status);
	util_GetU8Array(pvar->out.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 10
*/
u8 ui_ffGetAi2Out(u8 * str, u8 maxlen)
{
    IPC_FFAI2Params_t* pvar;

    pvar = GetAi2BlockVar();
    ui_dispFfVarStatus(pvar->out.status);
	util_GetU8Array(pvar->out.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 11
*/
u8 ui_ffGetAi3Out(u8 * str, u8 maxlen)
{
    IPC_FFAI3Params_t* pvar;

    pvar = GetAi3BlockVar();
    ui_dispFfVarStatus(pvar->out.status);
	util_GetU8Array(pvar->out.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 12
*/
u8 ui_ffGetAoSp(u8 * str, u8 maxlen)
{
    IPC_FFAOParams_t* pvar;

    pvar = GetAoBlockVar();
    ui_dispFfVarStatus(pvar->sp.status);
	util_GetU8Array(pvar->sp.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 13
*/
s16 *ui_ffGetDoSp(void)
{
    IPC_FFDOParams_t* pvar;

    pvar = GetDoBlockVar();
    ui_dispFfVarStatus(pvar->sp_d.status);
    return &(pvar->sp_d.value);
}

/** \brief called by ui variable node
        to get the value for display, index 14
*/
s16 *ui_ffGetDo2Sp(void)
{
    IPC_FFDO2Params_t* pvar;

    pvar = GetDo2BlockVar();
    ui_dispFfVarStatus(pvar->sp_d.status);
    return &(pvar->sp_d.value);
}

/** \brief called by ui variable node
        to get the value for display, index 15
*/
u8 ui_ffGetIsIn1(u8 * str, u8 maxlen)
{
    IPC_FFISParams_t* pvar;

    pvar = GetIsBlockVar();
    ui_dispFfVarStatus(pvar->in1.status);
	util_GetU8Array(pvar->in1.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}
/** \brief called by ui variable node
        to get the value for display, index 16
*/
u8 ui_ffGetIsIn2(u8 * str, u8 maxlen)
{
    IPC_FFISParams_t* pvar;

    pvar = GetIsBlockVar();
    ui_dispFfVarStatus(pvar->in2.status);
	util_GetU8Array(pvar->in2.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 17
*/
u8 ui_ffGetIsIn3(u8 * str, u8 maxlen)
{
    IPC_FFISParams_t* pvar;

    pvar = GetIsBlockVar();
    ui_dispFfVarStatus(pvar->in3.status);
	util_GetU8Array(pvar->in3.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/** \brief called by ui variable node
        to get the value for display, index 18
*/
u8 ui_ffGetIsIn4(u8 * str, u8 maxlen)
{
    IPC_FFISParams_t* pvar;

    pvar = GetIsBlockVar();
    ui_dispFfVarStatus(pvar->in4.status);
	util_GetU8Array(pvar->in4.value, maxlen, str);
	check_initial_string(str, maxlen);
	return maxlen;
}

/* This line marks the end of the source */


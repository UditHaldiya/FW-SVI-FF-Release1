/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_deviation_alert.c
    \brief functions about parameter DEVIATION_ALERT

    CPU: Any

    OWNER:
    $Date: 07/01/13 11:16p $
    $Revision: 1 $
    $Author: victoria huang $

*/
#include <softing_base.h>
#include <ptb.h>

//Glue headers last
#include "mnhart2ff.h"
#include "numutils.h"
#include "mn_deviation_alert.h"

#define MAX_DEVIATION_ALERT_TIME  (3600u)
#define MAX_DEVIATION_DEADBAND    (10u)
#define MIN_DEVIATION_DEADBAND    (9.99999977E-3f)   //0.01 in float32
#define MAX_DEVIATION_AlERT_VALUE (250.0f)
#define MIN_DEVIATION_AlERT_VALUE (9.99999977E-2f)   //0.1 in float32

/* \brief write deviation_alert parameter
    param in:
        p_block_instance: pointer to function block
        p_write:  pointer to write resource data

    return:
        FF error
*/
fferr_t ffda_WriteDeviationAlert
(
    T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA *p_write
)
{
    fferr_t fferr = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    if(0 == p_write->subindex)
    {
        _DEVIATION_ALERT* pAlert = (void*)p_write->source;
        float32 tempcomp1 = ABS(pAlert->component_1 - (float32)p_PTB->deviation_alert.component_1);
        float32 tempcomp2 = ABS(pAlert->component_2 - (float32)p_PTB->deviation_alert.component_2);
        //check if the alert_point is out of range or not
        if (pAlert->component_1 > MAX_DEVIATION_AlERT_VALUE\
            || pAlert->component_1 < MIN_DEVIATION_AlERT_VALUE)
        {
            fferr = E_FB_PARA_LIMIT;
        }
        //check if the dead_band is out of range or not
        if (pAlert->component_2 > MAX_DEVIATION_DEADBAND\
            || pAlert->component_2 < MIN_DEVIATION_DEADBAND)
        {
            fferr = E_FB_PARA_LIMIT;
        }
        if(pAlert->component_3 > MAX_DEVIATION_ALERT_TIME)
        {
            fferr = E_FB_PARA_LIMIT;
        }
        //if only ALERT is changed, manully oprate the ST_REV.
        if ((tempcomp1 < FLOAT_POSITIVE_SMALL_DIFF) && (tempcomp2 < FLOAT_POSITIVE_SMALL_DIFF)\
            &&(pAlert->component_3 == p_PTB->deviation_alert.component_3)\
            &&(pAlert->component_5 == p_PTB->deviation_alert.component_5)\
            &&(pAlert->component_6 == p_PTB->deviation_alert.component_6)\
            &&((bool_t)pAlert->component_4 != (bool_t)p_PTB->deviation_alert.component_4))
        {
            //mannully decrese the ST_REV number.
            IncrementStaticRevision(p_block_instance, -2);
        }
    }
    else if (2 == p_write->subindex)
    {
        //check if the alert_point is out of range or not
        if (mn_pull_float(&p_write->source[0]) > MAX_DEVIATION_AlERT_VALUE ||
            mn_pull_float(&p_write->source[0]) < MIN_DEVIATION_AlERT_VALUE)
        {
            fferr = E_FB_PARA_LIMIT;
        }
    }
    else if (3 == p_write->subindex)
    {
        //check if the dead_band is out of range or not
        if (mn_pull_float(&p_write->source[0]) > MAX_DEVIATION_DEADBAND ||
            mn_pull_float(&p_write->source[0]) < MIN_DEVIATION_DEADBAND)
        {
            fferr = E_FB_PARA_LIMIT;
        }
    }
    else if (4 == p_write->subindex)
    {
        u32* p_tmp = (u32*)(void*)p_write->source;
        if(MAX_DEVIATION_ALERT_TIME < *p_tmp)
        {
            fferr = E_FB_PARA_LIMIT;
        }
    }
    else if (5 == p_write->subindex)
    {
        if(p_write->source[0] != p_PTB->deviation_alert.component_4)
        {
            //mannully decrese the ST_REV number.
            IncrementStaticRevision(p_block_instance, -2);
        }
    }
    else
    {
    }
    return fferr;
}


/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_supply_pressure_alert.c
    \brief functions about parameter SUPPLY_PRESSURE_HI_ALERT,SUPPLY_PRESSURE_LO_ALERT,
    SUPPLY_PRESSURE_LOLO_ALERT

    CPU: Any

    OWNER:
    $Date: 03/29/13 14:39p $
    $Revision: 1 $
    $Author: victoria huang $

*/
#include <softing_base.h>
#include <ptb.h>

//Glue headers last
#include "mnhart2ff.h"
#include "mn_supply_pressure_alert.h"
#include "pressure_range.h"
#include "numutils.h"

/* \brief write supply_pressure_hi_alert, supply_pressure_lo_alert,
    supply_pressure_lolo_alert parameter
    param in:
        p_block_instance: pointer to function block
        p_write:  pointer to write resource data

    return:
        FF error
*/
fferr_t fftbsp_WriteSupplyPressureAlert
(
    T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA *p_write
)
{
    fferr_t fferr = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    u16       uCurrent_unit;
    float32  fmax_dead_band = 0.0F;
    float32  fmax_alert_point = 0.0F;
    float32  tempcomp1;
    float32  tempcomp2;

    uCurrent_unit = p_PTB->pressure_range.units;
    switch (uCurrent_unit)
    {
    case FF_UNITS_PSI:
        fmax_alert_point = PSI_PRESSURE_MAX;
        fmax_dead_band = PSI_PRESSURE_DEADBAND_MAX;
        break;
    case FF_UNITS_BAR:
        fmax_alert_point = BAR_PRESSURE_MAX;
        fmax_dead_band = BAR_PRESSURE_DEADBAND_MAX;
        break;
    case FF_UNITS_KPA:
        fmax_alert_point = KPA_PRESSURE_MAX;
        fmax_dead_band = KPA_PRESSURE_DEADBAND_MAX;
        break;
    default:
        fferr = E_ERROR;
        break;
    }

    if(0 == p_write->subindex)
    {
        _SYSTEM_ALERT* pAlert = (void*)p_write->source;

        //check if the alert_point is out of range or not
        if (pAlert->component_1 > fmax_alert_point || pAlert->component_1 < PRESSURE_MIN)
        {
            fferr = E_FB_PARA_LIMIT;
        }
        //check if the dead_band is out of range or not
        if (pAlert->component_2 > fmax_dead_band || pAlert->component_2 < PRESSURE_MIN)
        {
            fferr = E_FB_PARA_LIMIT;
        }

        switch (p_write->rel_idx)
        {
        case REL_IDX_PTB_SUPPLY_PRESSURE_HI_ALERT:
            tempcomp1 = ABS(pAlert->component_1 - p_PTB->supply_pressure_hi_alert.component_1);
            tempcomp2 = ABS(pAlert->component_2 - p_PTB->supply_pressure_hi_alert.component_2);
            //if only ALERT is changed, manully oprate the ST_REV.
            if ((tempcomp1 < FLOAT_POSITIVE_SMALL_DIFF) && (tempcomp2 < FLOAT_POSITIVE_SMALL_DIFF)\
                && (pAlert->component_4 == p_PTB->supply_pressure_hi_alert.component_4)\
                && (pAlert->component_5 == p_PTB->supply_pressure_hi_alert.component_5)\
                && ((bool_t)pAlert->component_3 != (bool_t)p_PTB->supply_pressure_hi_alert.component_3))
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        case REL_IDX_PTB_SUPPLY_PRESSURE_LO_ALERT:
            tempcomp1 = ABS(pAlert->component_1 - p_PTB->supply_pressure_lo_alert.component_1);
            tempcomp2 = ABS(pAlert->component_2 - p_PTB->supply_pressure_lo_alert.component_2);
            //if only ALERT is changed, manully oprate the ST_REV.
            if ((tempcomp1 < FLOAT_POSITIVE_SMALL_DIFF) && (tempcomp2 < FLOAT_POSITIVE_SMALL_DIFF)\
                &&(pAlert->component_4 == p_PTB->supply_pressure_lo_alert.component_4)\
                &&(pAlert->component_5 == p_PTB->supply_pressure_lo_alert.component_5)\
                &&((bool_t)pAlert->component_3 != (bool_t)p_PTB->supply_pressure_lo_alert.component_3))
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        case REL_IDX_PTB_SUPPLY_PRESSURE_LOLO_ALERT:
            tempcomp1 = ABS(pAlert->component_1 - p_PTB->supply_pressure_lolo_alert.component_1);
            tempcomp2 = ABS(pAlert->component_2 - p_PTB->supply_pressure_lolo_alert.component_2);
            //if only ALERT is changed, manully oprate the ST_REV.
            if ((tempcomp1 < FLOAT_POSITIVE_SMALL_DIFF) && (tempcomp2 < FLOAT_POSITIVE_SMALL_DIFF)\
                &&(pAlert->component_4 == p_PTB->supply_pressure_lolo_alert.component_4)\
                &&(pAlert->component_5 == p_PTB->supply_pressure_lolo_alert.component_5)\
                &&((bool_t)pAlert->component_3 != (bool_t)p_PTB->supply_pressure_lolo_alert.component_3))
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        default:
            break;
        }
    }
    else if (2 == p_write->subindex)
    {
        //check if the alert_point is out of range or not
        if (mn_pull_float(&p_write->source[0]) > fmax_alert_point ||
            mn_pull_float(&p_write->source[0]) < PRESSURE_MIN)
        {
            fferr = E_FB_PARA_LIMIT;
        }
    }
    else if (3 == p_write->subindex)
    {
        //check if the dead_band is out of range or not
        if (mn_pull_float(&p_write->source[0]) > fmax_dead_band ||
            mn_pull_float(&p_write->source[0]) < PRESSURE_MIN)
        {
            fferr = E_FB_PARA_LIMIT;
        }
    }
    else if (4 == p_write->subindex)
    {

        switch (p_write->rel_idx)
        {
        case REL_IDX_PTB_SUPPLY_PRESSURE_HI_ALERT:
            if(p_write->source[0] != p_PTB->supply_pressure_hi_alert.component_3)
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        case REL_IDX_PTB_SUPPLY_PRESSURE_LO_ALERT:
            if(p_write->source[0] != p_PTB->supply_pressure_lo_alert.component_3)
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        case REL_IDX_PTB_SUPPLY_PRESSURE_LOLO_ALERT:
            if(p_write->source[0] != p_PTB->supply_pressure_lolo_alert.component_3)
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        default:
            break;
        }
    }
    else
    {
    }
    return fferr;
}

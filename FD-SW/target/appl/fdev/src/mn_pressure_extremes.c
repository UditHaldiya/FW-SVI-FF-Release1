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
    $Date: 04/11/13 14:39p $
    $Revision: 1 $
    $Author: victoria huang $

*/
#include <softing_base.h>

//Glue headers last
#include "mn_supply_pressure_alert.h"
#include "pressure_range.h"
#include "mn_pressure_extremes.h"
#include "except.h"

#define MODULE_ID      (COMP_FAPP + MOD_APPL_TRN)


/* \brief write supply_pressure_hi_alert, supply_pressure_lo_alert,
    supply_pressure_lolo_alert parameter
    param in:
        p_block_instance: pointer to function block
        p_write:  pointer to write resource data

    return:
        FF error
*/
fferr_t fftbpe_WritePressureExtremes
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA *p_write
)
{
    fferr_t fferr     = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    float32  fmax_pressure = 0.0F;
    float32  fmin_pressure = 0.0F;
    u16      uCurrent_unit;
    u8       i;

    uCurrent_unit = p_PTB->pressure_range.units;
    switch (uCurrent_unit)
    {
    case FF_UNITS_PSI:
        fmax_pressure = PSI_PRESSURE_MAX;
        fmin_pressure = PSI_PRESSURE_MIN;
        break;
    case FF_UNITS_BAR:
        fmax_pressure = BAR_PRESSURE_MAX;
        fmin_pressure = BAR_PRESSURE_MIN;
        break;
    case FF_UNITS_KPA:
        fmax_pressure = KPA_PRESSURE_MAX;
        fmin_pressure = KPA_PRESSURE_MIN;
        break;
    default:
        fferr = E_ERROR;
        break;
    }

    if(0 == p_write->subindex)
    {
        _ASSERT (p_write->length == sizeof(_PRESSURE_EXTREMES));

        for (i = 0; i < sizeof(_PRESSURE_EXTREMES)/sizeof(float32); i++)
        {
            //check if the extrem is out of range or not
            if (mn_pull_float(&p_write->source[i * sizeof(float32)]) > fmax_pressure ||
                mn_pull_float(&p_write->source[i * sizeof(float32)]) < fmin_pressure)
            {
                fferr = E_FB_PARA_LIMIT;
            }
        }
    }
    else //if the p_write->subindex=1~8
    {
        //check if the supply_pressure_max is out of range or not
        if (mn_pull_float(&p_write->source[0]) > fmax_pressure ||
            mn_pull_float(&p_write->source[0]) < fmin_pressure)
        {
            fferr = E_FB_PARA_LIMIT;
        }
    }
    return fferr;
}

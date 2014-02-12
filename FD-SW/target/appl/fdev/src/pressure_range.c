/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file pressure_range.c
    \brief functions about parameter PRESSURE_RANGE

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include <ptb.h>

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "crc.h"

//Glue headers last
#include "mnhart2ff.h"
#include "pressure_range.h"
#include "pressuredef.h"

static float32 fftbpr_ChangUnit(u16 uLastUnit, u16 uCurrentUnit, float32 fValue);

/* \brief read pressure unit
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t ReadPressureRange(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    //hart cmd 170,176
    fferr_t fferr;

    Req_ReadSettings_t* req = snd_buf;

    Rsp_ReadPressureUnits_t* rsp_176 = (void*)((u8*)rcv_buf + HC170_LENGTH + 2);

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    util_PutU8(req->SubCmdNum[0], 176);

    fferr = mn_HART_acyc_cmd(170, snd_buf, HC170_REQ_LENGTH, rcv_buf);

    if(fferr == E_OK)
    {
        u8 ap_units;

        ap_units = util_GetU8(rsp_176->PressureUnits[0]);

        switch(ap_units)
        {
            case H_PSI:
            {
                //if the units changed, store the last pressure unit.
                if (p_PTB->pressure_range.units != FF_UNITS_PSI)
                {
                    fftbpr_PressureUnitUpdate(p_block_instance,
                        p_PTB->pressure_range.units, FF_UNITS_PSI);
                }
                break;
            }
            case H_BAR:
            {
                //if the units changed, store the last pressure unit.
                if (p_PTB->pressure_range.units != FF_UNITS_BAR)
                {
                    fftbpr_PressureUnitUpdate(p_block_instance,
                        p_PTB->pressure_range.units, FF_UNITS_BAR);
                }
                break;
            }
            case H_KPA:
            {
                //if the units changed, store the last pressure unit.
                if (p_PTB->pressure_range.units != FF_UNITS_KPA)
                {
                    fftbpr_PressureUnitUpdate(p_block_instance,
                        p_PTB->pressure_range.units, FF_UNITS_KPA);
                }
                break;
            }
            default:
            {
                fferr = E_ERROR;
            }
        }
    }

    return fferr;
}

/* \brief write pressure unit
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t WritePressureRange(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    //hart cmd 171,176
    fferr_t fferr;

    Req_WriteSettings_t* req = snd_buf;

    Req_WritePressureUnits_t* req_176 = (void*)((u8*)snd_buf + HC171_REQ_LENGTH);

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    u16 ff_units;

    util_PutU8(req->SubCmdNum[0], 176);

    ff_units = p_PTB->pressure_range.units;

    switch(ff_units)
    {
        case FF_UNITS_PSI:
        {
            util_PutU8(req_176->PressureUnits[0], H_PSI);
            break;
        }
        case FF_UNITS_BAR:
        {
            util_PutU8(req_176->PressureUnits[0], H_BAR);
            break;
        }
        case FF_UNITS_KPA:
        {
            util_PutU8(req_176->PressureUnits[0], H_KPA);
            break;
        }
        default:
        {
            fferr = E_ERROR;
            return fferr;
        }
    }

    fferr = mn_HART_acyc_cmd(171, snd_buf, HC171_REQ_LENGTH + MN_OFFSETOF(Req_WritePressureUnits_t, endmark), rcv_buf);

    return fferr;
}

/* \brief change the vlaue from last unit to current unit based on the ratio
    param in:
        uLastUnit   : Last time pressure unit, kpa, psi, bar
        uCurrentUnit: Current pressure unit, kpa, psi, bar
        fValue      : The previous pressure value

    return:
        The current unit corresponding pressure value
*/
static float32 fftbpr_ChangUnit(u16 uLastUnit, u16 uCurrentUnit, float32 fValue)
{
    float32 fResult = 0.0F;
    switch (uLastUnit)
    {
    case FF_UNITS_PSI:
        switch (uCurrentUnit)
        {
        case FF_UNITS_KPA:
            fResult = (float32)(fValue * PRESSURE_FACTOR_PSI2KPA);
            break;
        case FF_UNITS_BAR:
            fResult = (float32)(fValue * PRESSURE_FACTOR_PSI2BAR);
            break;
        case FF_UNITS_PSI:
            fResult = fValue;
            break;
        default:
            break;
        }
        break;
    case FF_UNITS_BAR:
        switch (uCurrentUnit)
        {
        case FF_UNITS_KPA:
            fResult = (float32)(fValue * PRESSURE_FACTOR_BAR2KPA);
            break;
        case FF_UNITS_BAR:
            fResult = fValue;
            break;
        case FF_UNITS_PSI:
            fResult = (float32)(fValue / PRESSURE_FACTOR_PSI2BAR);
            break;
        default:
            break;
        }
        break;
    case FF_UNITS_KPA:
        switch (uCurrentUnit)
        {
        case FF_UNITS_KPA:
            fResult = fValue;
            break;
        case FF_UNITS_BAR:
            fResult = (float32)(fValue * PRESSURE_FACTOR_KPA2BAR);
            break;
        case FF_UNITS_PSI:
            fResult = (float32)(fValue / PRESSURE_FACTOR_PSI2KPA);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return fResult;
}

typedef struct
{
    u8 block_idx;
    u16 value_offset;      //offset from the head of p_block
} UNIT_variable_info_t;

/* a table to hold the information for unit related variables*/
static const UNIT_variable_info_t unit_array[] =
{
//   block_idx     ---------------------value_offset--------------------------
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, pressure_extremes)+MN_OFFSETOF(_PRESSURE_EXTREMES, supply_pressure_max)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, pressure_extremes)+MN_OFFSETOF(_PRESSURE_EXTREMES, supply_pressure_min)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, pressure_extremes)+MN_OFFSETOF(_PRESSURE_EXTREMES, actuator_a_max)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, pressure_extremes)+MN_OFFSETOF(_PRESSURE_EXTREMES, actuator_a_min)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, pressure_extremes)+MN_OFFSETOF(_PRESSURE_EXTREMES, actuator_b_max)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, pressure_extremes)+MN_OFFSETOF(_PRESSURE_EXTREMES, actuator_b_min)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, pressure_extremes)+MN_OFFSETOF(_PRESSURE_EXTREMES, pilot_max)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, pressure_extremes)+MN_OFFSETOF(_PRESSURE_EXTREMES, pilot_min)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, actuator_3)+MN_OFFSETOF(_ACTUATOR_3, supply_prs_max)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, actuator_3)+MN_OFFSETOF(_ACTUATOR_3, prs_control_hi)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, actuator_3)+MN_OFFSETOF(_ACTUATOR_3, prs_control_lo)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, supply_pressure_hi_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_1)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, supply_pressure_hi_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_2)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, supply_pressure_lo_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_1)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, supply_pressure_lo_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_2)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, supply_pressure_lolo_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_1)},
    {ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, supply_pressure_lolo_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_2)}
};


/* \brief check pressure unit, when it changed, update all the related
    parameters.
    param in:
        p_block_instance: pointer to write description block
        uLastUnit       : last pressure unit
        uCurrentUnit    : Current pressure unit
    return:
        no
*/
void fftbpr_PressureUnitUpdate
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    u16 uLastUnit,
    u16 uCurrentUnit
)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    u16 i;
    u8* p_block;
    float32 *fValue;
    p_PTB->pressure_range.units = uCurrentUnit;
    if (uCurrentUnit == FF_UNITS_PSI)
    {
        p_PTB->pressure_range.eu_100 = PSI_MAX;
    }
    else if (uCurrentUnit == FF_UNITS_KPA)
    {
        p_PTB->pressure_range.eu_100 = KPA_MAX;
    }
    else if (uCurrentUnit == FF_UNITS_BAR)
    {
        p_PTB->pressure_range.eu_100 = BAR_MAX;
    }
    else
    {
        //lint
    }
    for (i = 0; i < NELEM(unit_array) ; i++)
    {
        p_block = (u8*)fbs_get_block_inst(unit_array[i].block_idx)->p_block_desc->p_block;
        fValue  = (float32*)(void*)(p_block + unit_array[i].value_offset);
        *fValue  = fftbpr_ChangUnit(uLastUnit, uCurrentUnit, *fValue);
    }

    return;
}

/* \brief check pressure unit parameter when it is written, and store the
    previous pressure at static virable sLastPressureUnit
    param in:
        p_write: pointer to write description block
    return:
        FF error
*/

fferr_t WritePressureRangeCheck
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA* p_write
)
{
    fferr_t fferr = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    u16 uCurUnit;

    switch (p_write->subindex)
    {
        case 0:
        {
            SCALE* p_pressure_range = (void*)p_write->source;
            if(p_pressure_range->dec_pt != PRESSURE_RANGE_DECIMAL
                || (p_pressure_range->units != FF_UNITS_PSI && p_pressure_range->units != FF_UNITS_BAR && p_pressure_range->units != FF_UNITS_KPA)
                )
            {
                fferr = E_FB_PARA_CHECK;
            }
            //if the units changed, store the last pressure unit.
            uCurUnit = p_pressure_range->units;
            if (p_PTB->pressure_range.units != uCurUnit)
            {
                fftbpr_PressureUnitUpdate(p_block_instance,
                    p_PTB->pressure_range.units, uCurUnit);
            }
            break;
        }
        case 3:
        {
            u16* p_units_index = (void*)p_write->source;
            if(*p_units_index != FF_UNITS_PSI && *p_units_index != FF_UNITS_BAR && *p_units_index != FF_UNITS_KPA)
            {
                fferr = E_FB_PARA_CHECK;
            }
            //if the units changed, store the last pressure unit.
            uCurUnit = *p_units_index;
            if (p_PTB->pressure_range.units != uCurUnit)
            {
                fftbpr_PressureUnitUpdate(p_block_instance,
                    p_PTB->pressure_range.units, uCurUnit);
            }
            break;
        }
        case 4:
        {
            if(PRESSURE_RANGE_DECIMAL != p_write->source[0])
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        default:
        {
            fferr = E_ERROR;
        }
    }
    return fferr;
}


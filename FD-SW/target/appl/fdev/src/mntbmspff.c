/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mntbspff.c
    \brief Handling of TB mode and setpoint interface

    CPU: Any

    OWNER: AK
*/

#include <math.h>

//Softing headers first
#include <softing_base.h>
#include <ptb.h>
#include <resb.h>
#include <appl_int.h>

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "hartcmd.h"
//#include "faultpublic.h"
#include "ipcdefs.h"

//Glue headers last
#include "mntbmspff.h"
#include "mn_fstate.h"
#include "mn_errorbits.h"

//---------------------------------------------------------------------------------------------
// SP limiters

#define     SP_MIN_CLAMP_VALUE          (-50.0F)
#define     SP_MAX_CLAMP_VALUE          (199.0F)
#define     FF_TRUE                    1u
#define     FF_FALSE                   0u

/** \brief A simple helper to make setpoint from setpoint D
\param spd - setpoint D
\return setpoint
*/
static float32 spd2sp(u8 spd)
{
    float32 sp;
    if (spd == 0)
    {
        sp  = SETPOINT_DOBOOLFB_MIN;
    }
    else
    {
        sp  = SETPOINT_DOBOOLFB_MAX;
    }
    return sp;
}

/** \brief A helper to send manual setpoint via command 155 interface
\param p_PTB - a pointer to TB data
\param mansp - a pointer to setpoint struct
\param from - a pointer to HART send buffer
\param to - a pointer to HART receive buffer
\return an error code
*/
static fferr_t ffmansp_SendManualSetpoint(const T_FBIF_PTB *p_PTB, const FLOAT_S *mansp, void *from, void *to)
{
    fferr_t fferr = E_OK;
    if((p_PTB->mode_blk.target == MODE_MAN) && (p_PTB->mode_blk.actual == MODE_MAN))
    {
        //Allow the setpoint in
//05/20/13: Vlad wants final_value_x *sent* unconditionally but it doesn't work in/confuses APP
        if((mansp->status & SQ_GOOD) != 0)
        {
            //Status is good; send to APP
            Req_WriteSetpoint_t *subcmd = from;
            Req_WriteDigitalSetpoint_t *s = (void *)((u8 *)from + MN_OFFSETOF(Req_WriteSetpoint_t, endmark));
            u8 len = MN_OFFSETOF(Req_WriteSetpoint_t, endmark) + MN_OFFSETOF(Req_WriteDigitalSetpoint_t, endmark);
            util_PutU8(subcmd->SubCmdNum[0], 5); //digital setpoint
            util_PutFloat(s->DigitalSetpoint[0], mansp->value);
            util_PutU8(s->TargetBlockMode[0], IPC_MODE_NO_CHANGE);

            fferr = mn_HART_acyc_cmd(155, from, len, to);
        }
        else
        {
            //Nothing to do; just have it stored locally
        }
    }
    else if(p_PTB->mode_blk.actual == MODE_OS)
    {
        //Nothing to do; just have it stored locally
    }
    else
    {
        fferr = E_FB_WRONG_MODE;
    }
    return fferr;
}

/** \brief Write manual FINAL_VALUE setpoint
\param p_block_instance - a pointer to TB instance
\param p_write - a pointer a write stream
\param from - a pointer to HART send buffer
\param to - a pointer to HART receive buffer
\return an error code
*/
fferr_t ffmansp_WriteManualSetpoint(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to)
{
    fferr_t fferr = E_ERROR;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    FLOAT_S mansp = p_PTB->final_value;
    switch(p_write->subindex)
    {
        case 0: //status+value
            mansp.status = p_write->source[0];
            memcpy(&mansp.value, &p_write->source[1], sizeof(mansp.value));
            break;
        case 1: //status
            mansp.status = p_write->source[0];
            break;
        case 2: //value
            memcpy(&mansp.value, &p_write->source[0], sizeof(mansp.value));
            break;
        default:
            fferr = E_FB_PARA_CHECK;
            break;
    }

    if(p_PTB->setpoint_source == SP_SELECT_AOFB)
    {
        fferr = ffmansp_SendManualSetpoint(p_PTB, &mansp, from, to);
    }
    if(fferr == E_OK)
    {
        osif_disable_all_tasks();
        p_PTB->final_value = mansp;
        if(p_PTB->setpoint_source == SP_SELECT_AOFB)
        {
            /*setpoint, although not directly used, must be populated because it
                (a) can be read independently and
                (b) is shipped to local UI
            */
            p_PTB->setpoint = mansp;
        }
        osif_enable_all_tasks();
    }
    return fferr;
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly


/** \brief Write manual FINAL_VALUE_D setpoint
\param p_block_instance - a pointer to TB instance
\param p_write - a pointer a write stream
\param from - a pointer to HART send buffer
\param to - a pointer to HART receive buffer
\return an error code
*/
fferr_t ffmansp_WriteManualSetpointD(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to)
{
    fferr_t fferr = E_ERROR;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    DISCRETE_S fvd = p_PTB->final_value_d;
    switch(p_write->subindex)
    {
        case 0: //status+value
            fvd.status = p_write->source[0];
            memcpy(&fvd.value, &p_write->source[1], sizeof(fvd.value));
            break;
        case 1: //status
            fvd.status = p_write->source[0];
            break;
        case 2: //value
            memcpy(&fvd.value, &p_write->source[0], sizeof(fvd.value));
            break;
        default:
            fferr = E_FB_PARA_CHECK;
            break;
    }

    FLOAT_S mansp;
    mansp.status = fvd.status;
    mansp.value = spd2sp(fvd.value);

    if(p_PTB->setpoint_source == SP_SELECT_DOBOOL)
    {
        fferr = ffmansp_SendManualSetpoint(p_PTB, &mansp, from, to);
    }
    if(fferr == E_OK)
    {
        osif_disable_all_tasks();
        p_PTB->final_value_d = fvd;
        if(p_PTB->setpoint_source == SP_SELECT_DOBOOL)
        {
            /*setpoint, although not directly used, must be populated because it
                (a) can be read independently and
                (b) is shipped to local UI
            */
            p_PTB->setpoint = mansp;
        }
        osif_enable_all_tasks();
    }
    return fferr;
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly

/** \brief Write manual FINAL_VALUE_DINT setpoint
\param p_block_instance - a pointer to TB instance
\param p_write - a pointer a write stream
\param from - a pointer to HART send buffer
\param to - a pointer to HART receive buffer
\return an error code
*/
fferr_t ffmansp_WriteManualSetpointDINT(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to)
{
    fferr_t fferr = E_ERROR;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    DISCRETE_S fvd = p_PTB->final_value_dint;
    switch(p_write->subindex)
    {
        case 0: //status+value
            fvd.status = p_write->source[0];
            memcpy(&fvd.value, &p_write->source[1], sizeof(fvd.value));
            break;
        case 1: //status
            fvd.status = p_write->source[0];
            break;
        case 2: //value
            memcpy(&fvd.value, &p_write->source[0], sizeof(fvd.value));
            break;
        default:
            fferr = E_FB_PARA_CHECK;
            break;
    }

    FLOAT_S mansp;
    mansp.status = fvd.status;
    mansp.value = (float32)(fvd.value);

    if(p_PTB->setpoint_source == SP_SELECT_DOINT)
    {
        fferr = ffmansp_SendManualSetpoint(p_PTB, &mansp, from, to);
    }
    if(fferr == E_OK)
    {
        osif_disable_all_tasks();
        p_PTB->final_value_dint = fvd;

        if(p_PTB->setpoint_source == SP_SELECT_DOINT)
        {
            /*setpoint, although not directly used, must be populated because it
                (a) can be read independently and
                (b) is shipped to local UI
            */
            p_PTB->setpoint = mansp;
        }
        osif_enable_all_tasks();
    }
    return fferr;
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly


/** \brief Initialize statuses/mode related to setpoint
\param p_PTB - a pointer to TB parameters
*/
void mnipc_InitSpAndMode(T_FBIF_PTB *p_PTB)
{
    p_PTB->mode_blk.actual = MODE_AUTO;
    /* EXPLANATION:
    When we get the actual mode from APP, we'll fix this field.
    Until then, we pretend to be in AUTO with bad status.
    We use this to see if we need to populate setpoints/final values (all sources)
    */

    u8 init_status;
    if( (p_PTB->mode_blk.target & MODE_OS) != 0)
    {
        init_status = SQ_BAD | SUB_OUT_OF_SERVICE;
    }
    else
    {
        init_status = SQ_UNCERTAIN | SUB_INITIAL_VALUE;
    }
    // POS and SP
    p_PTB->final_value.status           = init_status;
    p_PTB->final_value_dint.status      = init_status;
    p_PTB->final_value_d.status         = init_status;
    p_PTB->final_position_value.status  = init_status;
    p_PTB->working_pos.status           = init_status;
    p_PTB->working_sp.status            = init_status;
}

/** \brief A helper to convert position to D and DINT
*/
static void DandDint(const FLOAT_S *src, DISCRETE_S *d, DISCRETE_S *dint)
{
    d->status = src->status;
    if (src->value < SETPOINT_DOBOOLFB_LO_AS_MIN)
    {
        d->value = 0;
    }
    else
    {
        d->value = 1;
    }
    dint->status = src->status;
    if (src->value >= ONE_HUNDRED_PERCENT_FLOAT)
    {   // Limit HI
        dint->value = ONE_HUNDRED_PERCENT_INT;
    }
    else if (src->value <= ZERO_PERCENT_FLOAT)
    {   // Limit LO
        dint->value = ZERO_PERCENT_INT;
    }
    else
    {   // Just convert to INT - cast below is valid because of the range limit from above
        dint->value = (u8)lroundf(src->value);
    }
}

/** \brief Periodically receive actual mode and final_position_value)
A plugin to cmd. 177
\param receive_buff - a pointer to HART receive buffer
\param p_resource_ - a pointer to RB parameters
\param p_PTB - a pointer to TB parameters
*/
void mnipc_ReceiveSpAndMode(const u8 *receive_buff, T_RESOURCE *p_resource_, T_FBIF_PTB *p_PTB)
{
    //Part 1: prepare the data to write
    const Rsp_IPCDynamicParametersExchange_t *rcv = (const void *)(receive_buff+2);
    u8 actual_mode_cmd_177 = util_GetU8(rcv->IPCActualMode[0]);
    T_FBIF_RESB     *p_RTB;
    u8 HartRsp2 = receive_buff[1];
    //if FailState propagate flag is set -- set RB target mode to OOS (propagate FailState)
    if ( (p_PTB->failed_state.propagate_apfs_to_rb == 1) ) //AK: or != 0?
    {
        //check whether the OOS mode is due to AP fault
        if ((HartRsp2 & HART_DEVICE_FAILED) != 0 )
        {
            MN_ASSERT(actual_mode_cmd_177 == MODE_OS);
            p_resource_->mode_blk.target = MODE_OS;
        }
    }

    //characterized params
    FLOAT_S local_position_value =
    {
        .status = util_GetU8(rcv->IPCPOSStatus[0]),
        .value = util_GetFloat(rcv->IPCPOSFloat[0]),
    };

    //test if the faile state condition
    if (0 != (HartRsp2 & HART_DEVICE_FAILED))
    {
        /*the Valve control Failed State condition exists*/
        p_PTB->failed_state.app = FF_TRUE;
    }
    else
    {
        /*no failed state condition is detected in the valve control*/
        p_PTB->failed_state.app = FF_FALSE;
    }
    p_RTB = fbs_get_block_inst((USIGN8)ID_RESB_1)->p_block_desc->p_block;

    _ASSERT(p_RTB);

    /*in resource block, fault_state is variy from 1 to 2*/
    p_PTB->failed_state.ff = p_RTB->fault_state - 1;


    if(actual_mode_cmd_177 != MODE_AUTO)
    {
        if(actual_mode_cmd_177 == MODE_OS)
        {
            local_position_value.status = SQ_BAD | SUB_OUT_OF_SERVICE;
        }
        else
        {
            local_position_value.status |= LIMIT_MASK; //?? If this not so as shipped, it's APP bug
        }
    }

    DISCRETE_S local_position_value_dint;
    DISCRETE_S local_position_value_d;
    DandDint(&local_position_value, &local_position_value_d, &local_position_value_dint);

    //readback params
    FLOAT_S local_final_value;
    DISCRETE_S local_final_value_dint;
    DISCRETE_S local_final_value_d;

    if(p_PTB->readback_select == READBACK_SELECT_WORKINGPOS)
    {
        osif_disable_all_tasks();
        local_final_value = p_PTB->working_pos;
        osif_enable_all_tasks();
        DandDint(&local_final_value, &local_final_value_d, &local_final_value_dint);
    }
    else
    {
        //i.e., p_PTB->readback_select == READBACK_SELECT_ACTUALPOS
        local_final_value = local_position_value;
        local_final_value_d = local_position_value_d;
        local_final_value_dint = local_position_value_dint;
    }

    bool_t modeswitch = false;

    //Part 2: Write the prepared data atomically
    osif_disable_all_tasks();

    p_PTB->final_position_value = local_final_value;
    p_PTB->final_position_value_d = local_final_value_d;
    p_PTB->final_position_value_dint = local_final_value_dint;

    if(p_PTB->mode_blk.actual != actual_mode_cmd_177) //actual mode change!
    {

        // Need to reset SP and final values to ActPos

        //this is for viewing pleasure
        p_PTB->setpoint = local_position_value;

        //this is for bumpless switch to AUTO *before* the controlling block starts sending setpoint
        p_PTB->final_value  = local_position_value;
        p_PTB->final_value_dint = local_position_value_dint;
        p_PTB->final_value_d = local_position_value_d;

        if(actual_mode_cmd_177 != MODE_AUTO)
        {
            //The mode change is NOT to AUTO
            modeswitch = true;
        }
    }

    //-------------------------------------------------------------------------
    // The Actual_position parameter should be updated from the cmd 177 (as Local
    // Position Value) and then the Final Position should be populated either from
    // Actual Position or from Working Position based on READBACK_SELECT parameter.
    // Making this code close to what it supposed to be.

    p_PTB->actual_position = local_position_value;

    // Store the ACTUAL mode in the PTB parameter
    p_PTB->mode_blk.actual = actual_mode_cmd_177;

    osif_enable_all_tasks();

    //Part 3: Miscellany
    mn_ErrorBits_Set(ERRB_MODE_SWITCH, modeswitch);

    (void)Handle_MinMax(&local_position_value,
                        &p_PTB->position_extremes.final_pos_value_max,
                        &p_PTB->position_extremes.final_pos_value_min);
}

extern bool_t rb_HasRun, tb_HasRun;

/** \brief Periodically send target(ed) mode and final_value
A plugin to cmd. 177
\param send_buff - a pointer to HART send buffer
\param p_resource_ - a pointer to RB parameters
\param p_block_instance - a pointer to TB instance
*/
void mnipc_SendSpAndMode(u8 *send_buff, const T_RESOURCE *p_resource_, const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    /* write "target to send" mode value to HART */
    //"target to send" comes either from PTB block or from Resource Block when it is OOS
    u8 send_target_mode_cmd_177;

    osif_disable_all_tasks();

    /* AK: The following condition is a countermeasure against intermittent
    setting of RB actual mode = MODE_OS (by Softing initialization
    */
    //if( (p_resource_->mode_blk.actual == MODE_OS) && (p_resource_->mode_blk.target == MODE_OS) )
    if(rb_HasRun && tb_HasRun)
    {
        if(p_resource_->mode_blk.actual == MODE_OS)
        {
            send_target_mode_cmd_177 = MODE_OS;
        }
        else
        {
            send_target_mode_cmd_177 = p_PTB->mode_blk.target;
        }
    }
    else
    {
        send_target_mode_cmd_177 = IPC_MODE_NO_CHANGE;
    }

    // -----------------------------------------------------------------------------------------------------------
    FLOAT_S sp;
    // Here we take the Setpoint Select parameter and get the SP from an appropriate source.

    // We check if the TB is in the FAILSTATE

    //AK: if *target* mode is not AUTO, don't touch the setpoint
    if(p_PTB->mode_blk.target == MODE_AUTO) //or send_target_mode_cmd_177???
    {
        if (ff_fstate_ForceFstate_SP(p_PTB, &sp))
        {
            // OK, we are in the Fail State! It filled sp as configured
        }
        else
        {   // OK, we are NOT in Fail State -- just work with the SP source as usual
            switch (p_PTB->setpoint_source)
            {
                case SP_SELECT_AOFB :
                    sp = p_PTB->final_value;
                    break;

                case SP_SELECT_DOINT :
                    sp.status = p_PTB->final_value_dint.status;
                    sp.value  = (FLOAT)p_PTB->final_value_dint.value;
                    break;

                case SP_SELECT_DOBOOL :
                    sp.status = p_PTB->final_value_d.status;
                    if (p_PTB->final_value_d.value == 0)
                    {
                        sp.value  = SETPOINT_DOBOOLFB_MIN;
                    }
                    else
                    {
                        sp.value  = SETPOINT_DOBOOLFB_MAX;
                    }
                    break;

                default :           // Error
                    sp.status = (SQ_BAD | SUB_OUT_OF_SERVICE);
                    sp.value = 0.0F; //must be *a* value
                    break;
            }
        }
    }
    else
    {
        //unchanged; do as in the previous version
        sp = p_PTB->setpoint;
    }

    // Clamp the SP -51% to +200%
    // If clamping occurs, then also mark the status as limited.
    if (sp.value >= SP_MAX_CLAMP_VALUE)
    {
        sp.value = SP_MAX_CLAMP_VALUE;
        sp.status |= LIMIT_HIGH;
    }

    if (sp.value <= SP_MIN_CLAMP_VALUE)
    {
        sp.value = SP_MIN_CLAMP_VALUE;
        sp.status |= LIMIT_LOW;
    }

    p_PTB->setpoint = sp;

    osif_enable_all_tasks();

    ff_fstate_StoreLastSP(&sp);

    // Handle extremes of SP
    (void)Handle_MinMax(&sp,
                        &p_PTB->position_extremes.final_value_max,
                        &p_PTB->position_extremes.final_value_min);


    /* write to HART */
    Req_IPCDynamicParametersExchange_t *s = (void *)send_buff;
    util_PutU8(s->IPCTargetMode[0], send_target_mode_cmd_177);
    util_PutU8(s->IPCSPStatus[0], sp.status);
    util_PutFloat(s->IPCSPFloat[0], sp.value);
}


/* This line marks the end of the source */

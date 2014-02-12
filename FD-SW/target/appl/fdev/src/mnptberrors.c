/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mnptberrors.c
    \brief MN part of TB errors of all sorts (mostly, aggregation of work done by others)

    CPU: Any

    OWNER: AK
*/
#include <limits.h>
//Softing headers first
#include <softing_base.h>
#include <appl_int.h>
#include <ptb.h>
#include <resb.h> //required to access the RB

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

//MN FIRMWARE headers second

//Glue headers last
#include "mnfault2alert.h"
#include "mnhart2ff.h"
#include "appl_alert.h"

//From appl_dia.c - need to reconcile
//TODO !!!!!!!!!!!
  #define FD_HART_COM_DIAG_COND     31


#define BLK_ERR_SOFTING_SET (BLK_ERR_POWER_UP | BLK_ERR_LINK_CONFIG_ERR)
#define BLK_ERR_SOFTING_SET1 (BLK_ERR_LOST_STATIC_DATA|BLK_ERR_LOST_NV_DATA)

//Possible values for "alert_action.mapped_to_rb"
#define BLK_ERR_ALERT_MAP_TO_RB 1
//#define BLK_ERR_ALERT_IN_TB     2

static MnAlertsEtc_t AlertsEtcIF =
{
    .alert_map = 0U,
    .block_alert_map = 0U,
    .recom_action = FD_NO_RECOMM_ACT,
    .priority = 0,
};

static const u16 AlarmConfXlate[] =
{
    [0] = BLK_ERR_NO_ERROR,
    [1] = BLK_ERR_DEV_NEEDS_MAINT_SOON,
    [2] = BLK_ERR_DEV_NEEDS_MAINT_NOW,
    [3] = BLK_ERR_DEV_FAULT_STATE_SET, //or BLK_ERR_OUT_OF_SERVICE? With the new design?
};

u16 mnptb_GetTBRecommendedAction(void)
{
    return AlertsEtcIF.recom_action;
}

/** \brief Centralized collector of errors, xd_errors and what not
\param p_block_instance - a pointer to PTB
*/
void mnptb_ProcErrors(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{

    u32 block_alert_map;
    MnAlertsEtc_t AlertsEtc;
    //Pre-initialize AlertsEtc
    AlertsEtc.alert_map = 0U;
    AlertsEtc.block_alert_map = 0U;
    AlertsEtc.recom_action = FD_NO_RECOMM_ACT;
    AlertsEtc.priority = 0;

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    //Begin with block errors
    u16 block_err = mnfaults_Import(p_PTB, &AlertsEtc); //complete it!

    block_alert_map = AlertsEtc.block_alert_map;
    block_alert_map |= Execute_PTB_Alerts(p_block_instance);

    //Now, go over block alerts bitmap and set TB goodies
    //CONST_ASSERT(sizeof(p_PTB->alert_action.mapped_to_rb) == 1);
    CONST_ASSERT(MN_OFFSETOF(_ALERT_ACTION, mapped_to_rb) == 0);
    const u8 *aconf = (const u8 *)&p_PTB->alert_action + sizeof(p_PTB->alert_action.mapped_to_rb);
    u8 *astate = (u8 *)&p_PTB->alert_state;
    u16 *acount = (u16 *)&p_PTB->alert_counters; //lint !e740 cast of struct ptr to u16 ptr: struct is all u16's

    for(u8_least bit=0; bit < sizeof(p_PTB->alert_state); bit++)
    {
        if( (block_alert_map & (1U<<bit)) != 0U)
        {
            u8 c = aconf[bit];
            if(c < NELEM(AlarmConfXlate)) //IOW, ignore invalid configuration silently
            {
                block_err |= AlarmConfXlate[c]; //block errors from configurable alerts only
            }
            if(astate[bit] == 0)
            {
                osif_disable_all_tasks();

                astate[bit] = 1;
                acount[bit] = MIN(acount[bit]+1, UINT16_MAX);

                osif_enable_all_tasks();
            }
        }
        else
        {
            astate[bit] = 0;
        }
    }


    if(p_PTB->mode_blk.actual == MODE_OS)
    {
        block_err |= BLK_ERR_OUT_OF_SERVICE;
    }
    if(p_PTB->mode_blk.actual == MODE_LO)
    {
        block_err |= BLK_ERR_LOCAL_OVERRIDE;
    }
    if ( appl_get_comm_state() != DEV_COMM_OK )
    {
        AlertsEtc.alert_map |= 1U << FD_HART_COM_DIAG_COND;
        block_err |= BLK_ERR_READBACK_CHECK_FAILED;
    }

    //BLK_ERR_SIMULATE_ACTIVE not supported

    //TODO: What about BLK_ERR_LINK_CONFIG_ERR? Softing?

    //TODO: Add a dropping from target mode change request to set BLK_ERR_DEV_FAULT_STATE_SET

    //TODO: Add errors from FFP alerts here
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //---------- Updating block error -------------
    osif_disable_all_tasks();

    u16 old_block_err = p_PTB->block_err;
    u16 softing_block_err = (old_block_err & (BLK_ERR_SOFTING_SET|BLK_ERR_SOFTING_SET1)); //preserve Softing bits
    block_err |= softing_block_err;
    p_PTB->block_err = block_err;

    AlertsEtcIF = AlertsEtc;

    //Update RB error according to value of "p_PTB->alert_action.mapped_to_rb"
    //get RB block instance by ID
    T_FBIF_BLOCK_INSTANCE *pRB_inst = fbs_get_block_inst(ID_RESB_1);
    if (pRB_inst != NULL)
    {
        T_FBIF_RESB *p_RB = pRB_inst->p_block_desc->p_block;
        if (p_RB != NULL)
        {
            //first clear old alerts errors
            p_RB->block_err &= ~(BLK_ERR_DEV_NEEDS_MAINT_SOON | BLK_ERR_DEV_NEEDS_MAINT_NOW | BLK_ERR_DEV_FAULT_STATE_SET | BLK_ERR_LOST_NV_DATA | BLK_ERR_LOST_STATIC_DATA | BLK_ERR_MEMORY_FAILURE);

            if (p_PTB->alert_action.mapped_to_rb == BLK_ERR_ALERT_MAP_TO_RB)
            {
                //set new alerts errors
                p_RB->block_err |= (block_err & (BLK_ERR_DEV_NEEDS_MAINT_SOON | BLK_ERR_DEV_NEEDS_MAINT_NOW | BLK_ERR_DEV_FAULT_STATE_SET | BLK_ERR_LOST_NV_DATA | BLK_ERR_LOST_STATIC_DATA | BLK_ERR_MEMORY_FAILURE));
            }
        }
    }

    osif_enable_all_tasks();

    /* update block alarm */
    u16 new_block_err = block_err & ~old_block_err; //clear out previous block errors
    T_APPL_TB_DATA *p_PTB_data = p_block_instance->p_block_data;

    ffbl_update_block_alarm(p_block_instance->p_block_desc->block_id,
                            p_block_instance->p_block_desc->idx_blk_alm,
                            new_block_err,
                            &p_PTB_data->old_block_alarm_bits);

    //---------- Updating diag alerts -------------
    //This replaces appl_dia_update_field_diagnostics() in appl_dia.c
    osif_disable_all_tasks();

    appl_dia_set_all_fd_conditions(AlertsEtc.alert_map);

    osif_enable_all_tasks();



    /* on to xd errors */
    //TODO: decide whether we want or need xd_errors
    //p_PTB->xd_error_pressure = 0xFFU; /* no error detected */

}

/* This line marks the end of the source */

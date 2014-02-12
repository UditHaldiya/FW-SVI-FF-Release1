/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mnctllimff.c
    \brief Handling of control limits interface

    CPU: Any

    OWNER: AK
*/
#include <limits.h>
//Softing headers first
#include <softing_base.h>
#include <ptb.h>
//#include <appl_int.h>

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)


//MN FIRMWARE headers second

//Glue headers last
#include "mnfault2alert.h"

/** \brief A helper function to translate APP faults into FFP parlance
\param p_PTB - a pointer to TB info
\param[in][out] AlertsEtc - a pointer to other diag info to update (must be pre-initialized by caller)
\return forced block error bitmap (from faults directly; before alerts calculation)
*/
u16 mnfaults_Import(const T_FBIF_PTB *p_PTB, MnAlertsEtc_t *AlertsEtc)
{
    //Prime the loop
    u32 alert_map = AlertsEtc->alert_map; //Softing
    u32 block_alert_map = AlertsEtc->block_alert_map; //Vlad's
    u8 prio = AlertsEtc->priority;
    u16 recom_action = AlertsEtc->recom_action;

    u16 block_err = BLK_ERR_NO_ERROR;
    const u8 *fbyte = (const void *)&p_PTB->complete_status;
    CONST_ASSERT(FAULT_CODE_BYTES_REQUIRED <= sizeof(p_PTB->complete_status));
    u32 i = FAULT_CODE_BYTES_REQUIRED; //counter of fault bytes
    do
    {
        --i;
        if(fbyte[i] != 0) //only then do we go slowly bit by bit
        {
            u8_least fbit;
            //for(fbit = 0; fbit < CHAR_BIT; fbit++)
            for(fbit = 0; (i*CHAR_BIT + fbit) < MAX_FAULT_NUMBER; fbit++)
            {
                if((fbyte[i] & (1U<<fbit)) != 0)
                {
                    //fault bit set: act on it
                    const MnFaultAlarmMap_t *p = &MnFaultAlarmMap[i*CHAR_BIT + fbit];
                    block_err |= p->BlockError; //unconditional block error
                    if(p->diag_alarm_number != 0) //0 is otherwise reserved so we use it as an indicator of no action
                    {
                        u32 alert_map_new = alert_map | (1U << p->diag_alarm_number);
                        if(alert_map_new > alert_map)
                        {
                            //remember new map
                            alert_map = alert_map_new;

                            if(prio > p->priority) //Vlad: or < ?
                            {
                                //Higher prority
                                recom_action = p->recommended_action;
                                prio = p->priority;
                            }
                        }
                    }
                    if(p->BlockAlertX != ALERT_NONE) //0 is otherwise reserved so we use it as an indicator of no action
                    {
                        block_alert_map |= 1U << p->BlockAlertX;
                    }
                }
            }
        }
    } while(i != 0);

    //Write back
    AlertsEtc->alert_map = alert_map;
    AlertsEtc->block_alert_map = block_alert_map;
    AlertsEtc->recom_action = recom_action;
    AlertsEtc->priority = prio;

    return block_err;
}

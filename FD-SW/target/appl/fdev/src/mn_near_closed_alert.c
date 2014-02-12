/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_near_closed_alert.c
    \brief "near_closed_alert parameters of transduser block" handler

    CPU: Any

    OWNER: EJ
    $Date: 11/01/12 11:19a $
    $Revision: 1 $
    $Author: Stanley Fu $

*/
/* $History: mn_near_closed_alert.c $
 *
 * *****************  Version 1  *****************
 * User: Stanley Fu    Date: 11/1/12    Time: 10:38p
 * Init version
*/

#include <softing_base.h>
#include <ptb.h>
#include "numutils.h"
#include "mn_near_closed_alert.h"

#define  POINT_CLOSED_MAX  (100)
#define  POINT_CLOSED_MIN  (-49)
#define  NEAR_CLOSED_MIN   (2000u)


/* \brief This function will be called when near close alert is written
    param in:
        p_block_instance: pointer to the objects of transducer block
        p_write: pointer to write block
    return:
        fferr_t
*/
fferr_t WriteNearClosedAlert(T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA* p_write)
{
    fferr_t fferr = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    switch (p_write->subindex)
    {
        case 0:
        {
            _NEAR_CLOSED_ALERT* p_near_closed_alert = (_NEAR_CLOSED_ALERT*)(void*)p_write->source;
            float32 tempcomp1 = ABS(p_near_closed_alert->position_closed - p_PTB->near_closed_alert.position_closed);
            if (p_near_closed_alert->position_closed > POINT_CLOSED_MAX || p_near_closed_alert->position_closed < POINT_CLOSED_MIN)
            {
                fferr = E_FB_PARA_LIMIT;
                break;
            }
            if (p_near_closed_alert->near_closed > NEAR_CLOSED_MIN)
            {
                fferr = E_FB_PARA_LIMIT;
                break;
            }
            //only the alert change and others keep the same.
            if ((tempcomp1 < FLOAT_POSITIVE_SMALL_DIFF)\
                &&(p_near_closed_alert->near_closed == p_PTB->near_closed_alert.near_closed)\
                &&(p_near_closed_alert->alert_point == p_PTB->near_closed_alert.alert_point)\
                &&(p_near_closed_alert->historic_alert == p_PTB->near_closed_alert.historic_alert)\
                &&(p_near_closed_alert->enable == p_PTB->near_closed_alert.enable)\
                &&((bool_t)p_near_closed_alert->alert != (bool_t)p_PTB->near_closed_alert.alert))
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        }
        case 1:
        {
            if (mn_pull_float(&p_write->source[0]) > POINT_CLOSED_MAX
                || mn_pull_float(&p_write->source[0]) < POINT_CLOSED_MIN)
            {
                fferr = E_FB_PARA_LIMIT;
            }
            break;
        }
        case 3:
        {
            if (mn_pull_u32(&p_write->source[0]) > NEAR_CLOSED_MIN)
            {
                fferr = E_FB_PARA_LIMIT;
            }
            break;
        }
        case 4:
        {
            if(p_write->source[0] != p_PTB->near_closed_alert.alert)
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return fferr;
}





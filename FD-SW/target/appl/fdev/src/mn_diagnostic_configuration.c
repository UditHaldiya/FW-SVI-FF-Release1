/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_diagnostic_configuration.c
    \brief functions about parameter DIAGNOSTIC_CONFIGURATION

    CPU: Any

    OWNER:
    $Date: 08/26/13 13:20p $
    $Revision: 1 $
    $Author: victoria huang $

*/
#include <softing_base.h>
#include <ptb.h>
#include "ctllimits.h"

//Glue headers
#include "mnhart2ff.h"
#include "mn_diagnostic_configuration.h"

#define  MN_MIN_SETPOINT_RATE          (0.5f)
#define  MN_MAX_SETPOINT_RATE          (100.1f)
#define  MN_MIN_SAMPLING_TIME          (2u)
#define  MN_MAX_SAMPLING_TIME          (60u)
#define  MN_DIRECTION_BOTH_WAY         (0u)
#define  MN_DIRECTION_ONE_WAY          (1u)
#define  MN_OPTION_OPEN_LOOP           (0u)
#define  MN_OPTION_CLOSED_LOOP         (1u)


/* \brief check Diagnostic Configuration parameter when it is written,
    if the value is incorrect, return error.
    return
    param in:
        p_write: pointer to write description block
    return:
        FF error
*/

fferr_t ffdc_WriteDiagnostic_Configuration
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA* p_write
)
{
    UNUSED_OK(p_block_instance);
    fferr_t fferr = E_OK;

    //check the parameter
    switch (p_write->subindex)
    {
        case 0:
        {
            _DIAGNOSTIC_CONFIGURATION* p_diagconf = (_DIAGNOSTIC_CONFIGURATION*)(void*)p_write->source;
            if ( (p_diagconf->start_postion > POS_SETPOINT_HIGH) ||
                 (p_diagconf->start_postion < POS_SETPOINT_LOW) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            if ( (p_diagconf->end_position > POS_SETPOINT_HIGH) ||
                 (p_diagconf->end_position < POS_SETPOINT_LOW) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            if ( (p_diagconf->set_point_rate > MN_MAX_SETPOINT_RATE) ||
                 (p_diagconf->set_point_rate < MN_MIN_SETPOINT_RATE) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            if ( (p_diagconf->sampling_time > MN_MAX_SAMPLING_TIME) ||
                 (p_diagconf->sampling_time < MN_MIN_SAMPLING_TIME) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            if ( (p_diagconf->direction != MN_DIRECTION_BOTH_WAY) &&
                 (p_diagconf->direction != MN_DIRECTION_ONE_WAY) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            if ( (p_diagconf->option != MN_OPTION_OPEN_LOOP) &&
                 (p_diagconf->option != MN_OPTION_CLOSED_LOOP) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            break;
        }
        case 1:
        {
            if ( (mn_pull_float(&p_write->source[0]) > POS_SETPOINT_HIGH) ||
                 (mn_pull_float(&p_write->source[0]) < POS_SETPOINT_LOW) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            break;
        }
        case 2:
        {
            if ( (mn_pull_float(&p_write->source[0]) > POS_SETPOINT_HIGH) ||
                 (mn_pull_float(&p_write->source[0]) < POS_SETPOINT_LOW) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            break;
        }
        case 3:
        {
            if ( (mn_pull_float(&p_write->source[0]) > MN_MAX_SETPOINT_RATE) ||
                 (mn_pull_float(&p_write->source[0]) < MN_MIN_SETPOINT_RATE) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            break;
        }
        case 4:
        {
            if ( (p_write->source[0] > MN_MAX_SAMPLING_TIME) ||
                 (p_write->source[0] < MN_MIN_SAMPLING_TIME) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            break;
        }
        case 5:
        {
            if ( (p_write->source[0] != MN_DIRECTION_BOTH_WAY) &&
                 (p_write->source[0] != MN_DIRECTION_ONE_WAY) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            break;
        }
        case 6:
        {
            if ( (p_write->source[0] != MN_OPTION_OPEN_LOOP) &&
                 (p_write->source[0] != MN_OPTION_CLOSED_LOOP) )
            {
                fferr = E_FB_PARA_LIMIT;
                return fferr;
            }
            break;
        }
        default:
            break;
    }

    return fferr;
}





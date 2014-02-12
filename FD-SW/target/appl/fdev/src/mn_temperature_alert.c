/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_temperature_alert.c
    \brief check the parameters of TEMPERATURE_HI_ALERT and TEMPERATURE_HI_ALERT

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include <ptb.h>
//Glue headers last
#include "mnhart2ff.h"
#include "mn_temperature_alert.h"

#define TEMPERATURE_ALERT_POINT_MAX           (85.0f)
#define TEMPERATURE_ALERT_POINT_MIN           (-45.01f)
#define TEMPERATURE_DEADBAND_MAX              (2.0f)

/* \brief check temperature lo and hi alert when it is written
    param in:
        p_write: pointer to write description block
    return:
        FF error
*/
fferr_t WriteTemperatureAlert(const T_FBIF_WRITE_DATA* p_write)
{
    fferr_t fferr = E_OK;

    switch (p_write->subindex)
    {
        case 0:
        {
            _SYSTEM_ALERT* p_temperature_alert = (void*)p_write->source;
            if(
               (p_temperature_alert->component_1 < TEMPERATURE_ALERT_POINT_MIN || p_temperature_alert->component_1 > TEMPERATURE_ALERT_POINT_MAX) ||
               (p_temperature_alert->component_2 < 0.0 || p_temperature_alert->component_2 > TEMPERATURE_DEADBAND_MAX)
               )
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 2:
        {
            float32* p_alert_point = (void*)p_write->source;
            if(*p_alert_point < TEMPERATURE_ALERT_POINT_MIN || *p_alert_point > TEMPERATURE_ALERT_POINT_MAX)
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 3:
        {
            float32* p_deadband = (void*)p_write->source;
            if(*p_deadband < 0.0 || *p_deadband > TEMPERATURE_DEADBAND_MAX)
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        default:
        {
        }
    }
    return fferr;
}



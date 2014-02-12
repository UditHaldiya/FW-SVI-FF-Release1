/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_ip_drive_current_alert.c
    \brief check the parameters of IP_DRIVE_CURRENT_HI_ALERT and IP_DRIVE_CURRENT_LO_ALERT

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
#include "mn_ip_drive_current_alert.h"

#define IP_DRIVE_CURRENT_TIME_MAX           (60.0f)
#define IP_DRIVE_CURRENT_TIME_MIN           (1.0f)
#define IP_DRIVE_CURRENT_ALERT_POINT_MAX    (120.0f)
#define IP_DRIVE_CURRENT_ALERT_POINT_MIN    (-20.0f)
#define IP_DRIVE_CURRENT_DEADBAND_MAX       (20.0f)
#define IP_DRIVE_CURRENT_DEADBAND_MIN       (0.0f)

/* \brief check ip drive current lo and hi alert when it is written
    param in:
        p_write: pointer to write description block
    return:
        FF error
*/
fferr_t WriteIPDriveCurrentAlert(const T_FBIF_WRITE_DATA* p_write)
{
    fferr_t fferr = E_OK;

    switch (p_write->subindex)
    {
        case 0:
        {
            _IP_DRIVE_CURRENT_ALERT* p_ip_drive_current_alert = (void*)p_write->source;
            if((p_ip_drive_current_alert->time < IP_DRIVE_CURRENT_TIME_MIN || p_ip_drive_current_alert->time > IP_DRIVE_CURRENT_TIME_MAX) ||
               (p_ip_drive_current_alert->alert_point < IP_DRIVE_CURRENT_ALERT_POINT_MIN || p_ip_drive_current_alert->alert_point > IP_DRIVE_CURRENT_ALERT_POINT_MAX) ||
               (p_ip_drive_current_alert->deadband < IP_DRIVE_CURRENT_DEADBAND_MIN || p_ip_drive_current_alert->deadband > IP_DRIVE_CURRENT_DEADBAND_MAX)
               )
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 2:
        {
            float32* p_alert_point = (void*)p_write->source;
            if(*p_alert_point < IP_DRIVE_CURRENT_ALERT_POINT_MIN || *p_alert_point > IP_DRIVE_CURRENT_ALERT_POINT_MAX)
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 3:
        {
            float32* p_deadband = (void*)p_write->source;
            if(*p_deadband < IP_DRIVE_CURRENT_DEADBAND_MIN|| *p_deadband > IP_DRIVE_CURRENT_DEADBAND_MAX)
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 4:
        {
            float32* p_time = (void*)p_write->source;
            if(*p_time < IP_DRIVE_CURRENT_TIME_MIN || *p_time > IP_DRIVE_CURRENT_TIME_MAX)
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


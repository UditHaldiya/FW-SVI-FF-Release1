/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_remote_sensor.c
    \brief functions about parameter ACCESSORY

    CPU: Any

    OWNER:
    $Date: 05/23/13 13:20p $
    $Revision: 1 $
    $Author: victoria huang $

*/
#include <softing_base.h>
#include <ptb.h>

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

//MN FIRMWARE headers second
//#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
//#include "crc.h"
#include "hcimport_enum.h"
#include "faultpublic.h"

//Glue headers last
#include "mnhart2ff.h"
#include "mn_remote_sensor.h"
#define  ENABLE_INTERNAL_SENSOR     2
#define  ENABLE_EXTERNAL_SENSOR     3

/* \brief read Accessory parameter
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t ffrps_ReadAccessory
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    void* snd_buf,
    void* rcv_buf
)
{
    //hart cmd 170,248
    fferr_t fferr;

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    Req_ReadSettings_t* req = snd_buf;
    Rsp_ReadSensorType_t* rsp_248 = (void*)((u8*)rcv_buf + HC170_LENGTH + 2);
    util_PutU8(req->SubCmdNum[0], 248);
    fferr = mn_HART_acyc_cmd(170, snd_buf, HC170_REQ_LENGTH, rcv_buf);

    if(fferr == E_OK)
    {
        p_PTB->accessory.remote_sensor = util_GetU8(rsp_248->PositionSensorType[0]);
    }

    return fferr;
}

/* \brief check Accessory parameter when it is written, if the mode is wrong,
    return
    param in:
        p_write: pointer to write description block
    return:
        FF error
*/

fferr_t ffrps_WriteAccessory
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA* p_write,
    void* snd_buf,
    void* rcv_buf
)
{
    fferr_t fferr = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    //hart cmd 171,248
    Req_WriteSettings_t* req = snd_buf;
    Req_WriteSensorType_t* req_248 = (void*)((u8*)snd_buf + HC171_REQ_LENGTH);
    util_PutU8(req->SubCmdNum[0], 248);
    u8 uRemoteSensorType = 0;

    if(2 == p_write->subindex || 0 == p_write->subindex)
    {
        //check the mode;
        if(MODE_OS != p_PTB->mode_blk.target)
        {
            fferr = E_FB_WRONG_MODE;
            return fferr;
        }
        //check the parameter
        switch (p_write->subindex)
        {
            case 0:
            {
                _ACCESSORY* p_accessory = (void*)p_write->source;
                if (p_accessory->remote_sensor != ENABLE_INTERNAL_SENSOR &&
                    p_accessory->remote_sensor != ENABLE_EXTERNAL_SENSOR)
                {
                    fferr = E_FB_PARA_LIMIT;
                    return fferr;
                }
                else if(p_accessory->remote_sensor == ENABLE_INTERNAL_SENSOR)
                {
                    //if already the internal sensor, do nothing.
                    if (p_PTB->accessory.remote_sensor == HCInternalPositionSensor)
                    {
                        //change the write vlaue to 0;
                        p_accessory->remote_sensor = HCInternalPositionSensor;
                        return fferr;
                    }
                    else
                    {
                        uRemoteSensorType = HCInternalPositionSensor;
                        //change the write vlaue to 0;
                        p_accessory->remote_sensor = HCInternalPositionSensor;
                    }
                }
                else//ENABLE_EXTERNAL_SENSOR
                {
                    //if already the external sensor, do nothing.
                    if (p_PTB->accessory.remote_sensor == HCExternalPositionSensor)
                    {
                        //change the write vlaue to 1;
                        p_accessory->remote_sensor = HCExternalPositionSensor;
                        return fferr;
                    }
                    else
                    {
                        uRemoteSensorType = HCExternalPositionSensor;
                        //change the write vlaue to 1;
                        p_accessory->remote_sensor = HCExternalPositionSensor;
                    }
                }
                break;
            }
            case 2:
            {
                if (*((u8*)p_write->source) != ENABLE_INTERNAL_SENSOR &&
                    *((u8*)p_write->source) != ENABLE_EXTERNAL_SENSOR)
                {
                    fferr = E_FB_PARA_LIMIT;
                    return fferr;
                }
                else if(*((u8*)p_write->source) == ENABLE_INTERNAL_SENSOR)
                {
                    //if already the internal sensor, do nothing.
                    if (p_PTB->accessory.remote_sensor == HCInternalPositionSensor)
                    {
                        //change the write vlaue to 0;
                        *((u8*)p_write->source) = HCInternalPositionSensor;
                        return fferr;
                    }
                    else
                    {
                        uRemoteSensorType = HCInternalPositionSensor;
                        //change the write vlaue to 0;
                        *((u8*)p_write->source) = HCInternalPositionSensor;
                    }
                }
                else
                {
                    //if already the external sensor, do nothing.
                    if (p_PTB->accessory.remote_sensor == HCExternalPositionSensor)
                    {
                        //change the write vlaue to 1;
                        *((u8*)p_write->source) = HCExternalPositionSensor;
                        return fferr;
                    }
                    else
                    {
                        uRemoteSensorType = HCExternalPositionSensor;
                        //change the write vlaue to 1;
                        *((u8*)p_write->source) = HCExternalPositionSensor;
                    }
                }
                break;
            }
            default:
                break;
        }
        //send hart command 171,248 to write remote sensor type.
        util_PutU8(req_248->PositionSensorType[0], uRemoteSensorType);

        fferr = mn_HART_acyc_cmd(171, snd_buf, HC171_REQ_LENGTH +
                MN_OFFSETOF(Req_WriteSensorType_t, endmark), rcv_buf);

    }
    return fferr;
}




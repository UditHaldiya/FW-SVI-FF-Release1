/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_facotry_defaults.c
    \brief implement the parameter failed_state read

    CPU: Any

    OWNER: Victoria
    $Date: 04/27/13 14:19p $
*/

//Softing headers first
#include <softing_base.h>
#include <ptb.h>
#include <RTOS.h>

#include "hw_if.h"
#include "hartdef.h"
#include "mnhart2ff.h"
#include "bufutils.h"
#include "mn_factory_defaults.h"
#include "mn_fstate.h"
//#include "uipublic.h"
//#include "uimenued.h"
//#include "hcimport_enum.h"
#include "eep_blk3.h"
#include "eep_if.h"
#include "fbif_dcl.h"
#include "mn_advanced.h"


#define SAVE_AP_DELAY_TIME_MS    1000u
#define RETRY_TIMES              6u

#if 0
#define INITIAL_BUTTON_LEVEL     BUTTON_LOCK_NONE
#define INITIAL_PASSWORD         8888u

/* \brief set the paramter to factor default value(partly)
   \param in:
        snd_buf:  send buffer
        rcv_buf:  receive buffer
   \return:
        FF error
*/
fferr_t ffres_recovery_defaults(void* snd_buf,void* rcv_buf)
{
    fferr_t fferr = E_OK;
    T_FBIF_BLOCK_INSTANCE * p_block_instance = fbs_get_block_inst(ID_PTB_1);
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;

    Req_WriteSettings_t* req = snd_buf;

    Req_ChangeDeviceMode_t* req3 = snd_buf;

    //change the AP mode to Setup to send the hart command.
    if (MODE_OS != p_PTB->mode_blk.target)
    {
        util_PutU8(req3->DeviceMode[0], HSetup);

        fferr = mn_HART_acyc_cmd(135, snd_buf, HC135_REQ_LENGTH, rcv_buf);
    }

    if (E_OK == fferr)
    {
        //hart cmd 171,1
        Req_WriteUIlanguage_t* req_1
            = (void*)((u8*)snd_buf + HC171_REQ_LENGTH);

        util_PutU8(req->SubCmdNum[0], 1);

        util_PutU8(req_1->UILanguage[0], 0);

        fferr = mn_HART_acyc_cmd(171, snd_buf, HC171_REQ_LENGTH
            + MN_OFFSETOF(Req_WriteUIlanguage_t, endmark), rcv_buf);
    }

    if (E_OK == fferr)
    {
        //hart cmd 171,2
        Req_WriteUIAccessControl_t* req_2
            = (void*)((u8*)snd_buf + HC171_REQ_LENGTH);

        UIAccessControl_t* p_access_ctrl
            = (UIAccessControl_t*)(void*)(req_2->UIAccessControl);

        util_PutU8(req->SubCmdNum[0], 2);

        util_PutU8(p_access_ctrl->UILockLevel[0], INITIAL_BUTTON_LEVEL);
        util_PutU8(p_access_ctrl->UIPasswordEnabledFlag[0], FALSE);
        util_PutU16(p_access_ctrl->UIpassword[0], INITIAL_PASSWORD);

        fferr = mn_HART_acyc_cmd(171, snd_buf, HC171_REQ_LENGTH
            + MN_OFFSETOF(Req_WriteUIAccessControl_t, endmark), rcv_buf);
    }

    if (E_OK == fferr)
    {
        //change the AP mode to default value(Normal)
        util_PutU8(req3->DeviceMode[0], HNormal);

        fferr = mn_HART_acyc_cmd(135, snd_buf, HC135_REQ_LENGTH, rcv_buf);
    }
    return fferr;
}
#endif

/* \brief reset the APP by hart command 42
   \param in:
        snd_buf:  send buffer
        rcv_buf:  receive buffer
   \return:
        FF error
*/
fferr_t ffres_restart_APP(void* snd_buf, void* rcv_buf)
{
    fferr_t fferr;

    fferr = mn_HART_acyc_cmd(42, snd_buf, HC42_REQ_LENGTH
        + MN_OFFSETOF(Req_Reset_t, endmark), rcv_buf);

    return fferr;
}

/* \brief save the static/nv type in tb block to NV.
   \param in:
        p_block_instance: pointer to TB function block.
        uStRev          : the orignial TB ST_REV value.
   \return:
        no
*/
static fferr_t ffres_store_tb_NV(T_FBIF_BLOCK_INSTANCE * p_block_instance, u16 uStRev)
{
    fferr_t fferr = E_OK;
    u8 i;
    T_FBIF_WRITE_DATA p_write;
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;

    for (i = 0; i <p_block_instance->p_block_desc->no_of_param; i++)
    {
        // Scanning the parameters -- if the parameter in T_FBIF_PTB_STATIC table, save it to NV.
        if (ptb_param_dir[i].eep_offset != 0xffff)
        {
            p_write.rel_idx = i;
            p_write.subindex      = 0;
            p_write.length        = (USIGN8)ptb_param_dir[i].par_size;
            p_write.source        = (USIGN8 *)(void*)((u8 *)p_PTB + ptb_param_dir[i].par_offset);
            p_write.dest          = (USIGN8 *)(void*)((u8 *)p_PTB + ptb_param_dir[i].par_offset);
            p_write.phase         = POST_WRITE_ACTION;

            // Process the ST_REV separately: increment the current ST_REV and store the resulting NEW ST_REV
            if (REL_IDX_PTB_ST_REV == i)
            {
                uStRev++;
                p_write.source = (USIGN8 *)(void*)&uStRev;
            }
            if (appl_check_hm_state() == HM_RUN )
            {
              p_write.startup_sync = FALSE;
            }
            else
            {
              p_write.startup_sync = TRUE;
            }
            fferr = fbs_write_param(p_block_instance,&p_write);
            if (fferr != E_OK)
            {
                return fferr;
            }
        }

        // Tickle the Watch Dog every 10 cycles.
        if (i % 10 == 0)
        {
            Reset_Watchdog();
        }
    }
    return fferr;
}

/* \brief When the restart parameter choose the "RESTART_FACTORY_DEFAULTS",
    we need to do "RESTART_HIDDEN" at first, if we do not do "hidden", return
    false to prevent the user to do "RESTART_FACTORY_DEFAULTS".
    the value will be 0 from gw file.
   \param in:
        snd_buf:  send buffer
        rcv_buf:  receive buffer
   \return:
        true or false, if the device have done "hidden", return true.
*/
static bool_t ffres_restart_check_hidden_done(void)
{
    T_EEP_RESULT  eep_result;
    MODE         sTestMode;

    //get the mode parameter from blk3;
    eep_result = eep_blk3_get(MN_OFFSETOF(eep_blk3_t, tb_factory_default) +
        MN_OFFSETOF(T_FBIF_PTB, mode_blk), elemsize(T_FBIF_PTB,mode_blk), (void*)&sTestMode);
    if (eep_result != EEP_OK)
    {
        return false;
    }
    //if the target and actual mode are all 0, it means we do not do hidden before.
    if (sTestMode.actual == 0 && sTestMode.target == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/* \brief When the restart parameter choose the "HIDDEN", we need to
    save the TB block parameters to eep blk3. Then we need to send a
    hart command to save the current APP parameter also.
   \param in:
        snd_buf:  send buffer
        rcv_buf:  receive buffer
   \return:
        FF error
*/
fferr_t ffres_restart_hidden(void* snd_buf, void* rcv_buf)
{
    u8      i;
    fferr_t fferr;
    T_EEP_RESULT  eep_result;

    T_FBIF_BLOCK_INSTANCE * p_block_instance = fbs_get_block_inst(ID_PTB_1);
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;

    Req_WriteSettings_t* req = snd_buf;
    //send hart command 155,6 to set the APP mode to Failsafe.
    util_PutU8(req->SubCmdNum[0], 6);
    fferr = mn_HART_acyc_cmd(155, snd_buf, HC155_REQ_LENGTH +
            MN_OFFSETOF(Req_SetFailedState_t, endmark), rcv_buf);
    if (fferr == E_OK)
    {
        //send hart command 190,130 to save current APP parameters.
        util_PutU8(req->SubCmdNum[0], 130);
        fferr = mn_HART_acyc_cmd(190, snd_buf, HC190_REQ_LENGTH +
            MN_OFFSETOF(Req_SaveNvmemAsFactoryDefault_t, endmark), rcv_buf);
    }
    else
    {
        return fferr;
    }
    if (fferr != E_OK)
    {
        return fferr;
    }

    //store the PTB structure to the blk3;
    eep_result = eep_blk3_store(MN_OFFSETOF(eep_blk3_t, tb_factory_default),
        sizeof(T_FBIF_PTB), (void *)p_PTB);
    if (eep_result != EEP_OK)
    {
        return E_ERROR;
    }
    ///transfer the private data from blk2 eep_blk2_private to
    //blk3 eep_blk3_private_factory_default;
    eep_result = eep_blk23_transfer(MN_OFFSETOF(eep_blk3_t, eep_blk3_private_factory_default),\
        MN_OFFSETOF(eep_blk2_t, eep_blk2_private), sizeof(eep_blk2_private_t));
    if (eep_result != EEP_OK)
    {
        return E_ERROR;
    }

    //try four times to send hart command 155,7 to clear the Failsafe,
    //to avoid the 190,130 do not complete save the datas.
    util_PutU8(req->SubCmdNum[0], 7);
    for (i = 0; i < RETRY_TIMES; i++)
    {
        OS_Delay(SAVE_AP_DELAY_TIME_MS);
        //send hart command 155,7 to clear the Failsafe.
        fferr = mn_HART_acyc_cmd(155, snd_buf, HC155_REQ_LENGTH +
            MN_OFFSETOF(Req_ClearFailedState_t, endmark), rcv_buf);
        if (fferr == E_OK)
        {
            break;
        }
    }

    return fferr;
}

/* \brief When the restart parameter choose the "RESTART_FACTORY_DEFAULTS",
    we need to restore the TB block parameters from eep blk3. Then we need
    to send a hart command to restore the APP parameter using the saving datas.
   \param in:
        snd_buf:  send buffer
        rcv_buf:  receive buffer
   \return:
        FF error
*/
fferr_t ffres_restart_factory_defaults(void* snd_buf, void* rcv_buf)
{
    u32      i;
    u16     uStRev;
    fferr_t fferr;
    T_EEP_RESULT  eep_result;

    T_FBIF_BLOCK_INSTANCE * p_block_instance = fbs_get_block_inst(ID_PTB_1);
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;
    Req_WriteSettings_t* req = snd_buf;
    Req_RestoreFactoryDefaultNvmem_t* req_129 = (void*)((u8*)snd_buf + HC190_REQ_LENGTH);

    //check if the mode is OOS both in TB.target and TB.actual.
    if (p_PTB->mode_blk.target != MODE_OS || p_PTB->mode_blk.actual != MODE_OS)
    {
        fferr = E_FB_WRONG_MODE;
        return fferr;
    }
    //check if we have done "hidden" or not.
    if (!ffres_restart_check_hidden_done())
    {
        fferr = E_FB_PARA_LIMIT;
        return fferr;
    }
    //send hart command 155 to set the APP mode to Failsafe.
    util_PutU8(req->SubCmdNum[0], 6);
    fferr = mn_HART_acyc_cmd(155, snd_buf, HC155_REQ_LENGTH +
            MN_OFFSETOF(Req_SetFailedState_t, endmark), rcv_buf);
    if (fferr == E_OK)
    {
        //send a hart command to resotre current APP parameters.hart cmd 190,129
        util_PutU8(req->SubCmdNum[0], 129);
        util_PutU8(req_129->NvmemDefaultsMaskFlags[0], 4);
        util_PutU8(req_129->NvmemDefaultsMatchFlags[0], 4);
        util_PutU8(req_129->NvmemDefaultsTestOnly[0], 0);
        util_PutU8(req_129->NvmemVolumeId[0], 0);

        fferr = mn_HART_acyc_cmd(190, snd_buf, HC190_REQ_LENGTH +
            MN_OFFSETOF(Req_RestoreFactoryDefaultNvmem_t, endmark), rcv_buf);
    }
    if (fferr != E_OK)
    {
        return fferr;
    }
    // Retrieve the current ST_REV value;
    uStRev = p_PTB->st_rev;

    // get the tb structure from blk3;
    eep_result = eep_blk3_get(MN_OFFSETOF(eep_blk3_t, tb_factory_default),
        sizeof(T_FBIF_PTB), p_PTB);
    if (eep_result != EEP_OK)
    {
        return E_ERROR;
    }

    //save the tb block static/nv data to NV
    fferr = ffres_store_tb_NV(p_block_instance, uStRev);
    if (fferr != E_OK)
    {
        return fferr;
    }
    //transfer the private data from blk3 eep_blk3_private_factory_default to
    //blk2 eep_blk2_private;
    eep_result = eep_blk32_transfer(MN_OFFSETOF(eep_blk2_t, eep_blk2_private),
        MN_OFFSETOF(eep_blk3_t, eep_blk3_private_factory_default), sizeof(eep_blk2_private_t));

    if (eep_result != EEP_OK)
    {
        return E_ERROR;
    }

    u32 stamp1 = eep_get_stamp(); //we need to wait until deferred write completes
    //(twice because we may have asked for write in the middle of the cycle

    //try four times to send hart command 155,7 to clear the Failsafe,
    //to avoid the 190,129 do not complete restore the datas.
    i = 0;
    do
    {
        Reset_Watchdog();
        OS_Delay(SAVE_AP_DELAY_TIME_MS);
        fferr = mn_HART_acyc_cmd(251, snd_buf, HC251_REQ_LENGTH +
            MN_OFFSETOF(Req_ClearFailedState_t, endmark), rcv_buf);
        const Rsp_CheckProcess_t *rsp = (void *)((u8 *)rcv_buf + 2);
        if (fferr == E_OK)
        {
            if(util_GetU8(rsp->ProcessId[0])==0)
            {
                break;
            }
        }
        else
        {
            i++;
        }
    } while(i < RETRY_TIMES);


    //First wait for eep cycle to complete
    u32 stamp2;
    do
    {
        Reset_Watchdog();
        OS_Delay(SAVE_AP_DELAY_TIME_MS);
        stamp2 = eep_get_stamp(); //watching for stamp1 change
    } while(stamp2 == stamp1);

    //send hart command 155,7 to clear the Failsafe.
    //We probably won't change the mode though. Just clear faults.
    util_PutU8(req->SubCmdNum[0], 7);
    (void)mn_HART_acyc_cmd(155, snd_buf, HC155_REQ_LENGTH +
        MN_OFFSETOF(Req_ClearFailedState_t, endmark), rcv_buf);

    fferr_t fferr1 = mn_HART_acyc_cmd(42, snd_buf, HC42_REQ_LENGTH
        + MN_OFFSETOF(Req_Reset_t, endmark), rcv_buf);
    if (fferr == E_OK)
    {
        fferr = fferr1;
    }

    //Second wait for eep cycle to complete
    do
    {
        Reset_Watchdog();
        OS_Delay(SAVE_AP_DELAY_TIME_MS);
        stamp1 = eep_get_stamp(); //now watching for stamp2 change
    } while(stamp2 == stamp1);

    // Note: CMD 42 resets only the APP uC, the FF uC still has to be reset
    // Force the Reset on FF uC here.
    if (fferr == E_OK)
    {
        Reset_CPU();
    }

    return fferr;
}
/* \brief When the restart parameter choose the "RESTART_DEFAULTS",
    we need to restore the advanced parameter in TB block from eep blk3, as
    the value will be 0 from gw file.
   \param in:
        snd_buf:  send buffer
        rcv_buf:  receive buffer
   \return:
        FF error
*/
fferr_t ffres_restart_restore_advanced(void)
{
    fferr_t fferr;
    T_EEP_RESULT  eep_result;

    T_FBIF_BLOCK_INSTANCE * p_block_instance = fbs_get_block_inst(ID_PTB_1);
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;
    T_FBIF_WRITE_DATA       p_write;

    //get the advanced parameter from blk3;
    eep_result = eep_blk3_get(MN_OFFSETOF(eep_blk3_t, tb_factory_default) +
        MN_OFFSETOF(T_FBIF_PTB, advanced), elemsize(T_FBIF_PTB,advanced), (void*)p_PTB->advanced);
    if (eep_result != EEP_OK)
    {
        return E_ERROR;
    }

    //save the parameter advanced to NV
    osif_disable_all_tasks();

    p_write.rel_idx       = REL_IDX_PTB_ADVANCED;
    p_write.subindex      = 0;
    p_write.length        = (USIGN8)elemsize(T_FBIF_PTB,advanced);
    p_write.source        = (USIGN8 *)(void*)((u8 *)p_PTB + MN_OFFSETOF(T_FBIF_PTB,advanced) );
    p_write.dest          = (USIGN8 *)(void*)((u8 *)p_PTB + MN_OFFSETOF(T_FBIF_PTB,advanced) );
    p_write.phase         = POST_WRITE_ACTION;
    if (appl_check_hm_state() == HM_RUN )
    {
      p_write.startup_sync = FALSE;
    }
    else
    {
      p_write.startup_sync = TRUE;
    }
    fferr = fbs_write_param(p_block_instance,&p_write);
    //reinitialize the advanced flag;
    ffcheck_AdvancedOnStartUp();
    osif_enable_all_tasks();

    return fferr;
}
/*This is the last line of this file*/

/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ctrl_param_set.c
    \brief functions about parameter ACTIVATE_CONTROL_SET, ACTIVATE_CONTROL_SET, CUSTOM_CONTROL_SET and BACKUP_CONTROL_SET

    CPU: Any

    OWNER: EJ
    $Date: 11/01/12 11:19a $
    $Revision: 1 $
    $Author: Eric Jiang $

*/
#include <softing_base.h>
#include <ptb.h>
//end incantaion

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "crc.h"

//Glue headers last
#include "mnhart2ff.h"
//#undef POS
//#include "selftune.h"
#include "ctrl_param_set.h"

#define MODULE_ID   (COMP_FAPP + MOD_APPL_TRN)

//There is a redefinition problem of macro POS in selftune.h and osif.h, so selftune.h cannot be included here
#define P_HIGH_LIMIT  5000
#define I_HIGH_LIMIT  1000
#define D_HIGH_LIMIT  200
#define PADJ_LOW_LIMIT  -3000
#define PADJ_HIGH_LIMIT  3000
#define BETA_LOW_LIMIT  -9
#define BETA_HIGH_LIMIT  9
#define PCOMP_LOW_LIMIT  2
#define PCOMP_HIGH_LIMIT  20
#define DEADZONE_HIGH_LIMIT  821
#define NONLIN_HIGH_LIMIT       (20)
#define NONLIN_LOW_LIMIT        (0)
//-----------------------------------------------------------------------------------------

/* \brief execution codes for Write_handler_PTB(), when change on REL_IDX_PTB_CUSTOM_CONTROL_SET
    param in:
        p_block_instance:   pointer to the FF block
        p_write:            ponter to the write parameters
        snd_buf:            pointer to memory, which is for send of HART
        rcv_buf:            pointer to memory, which is for receive of hart
    param out:

    return:
        OK if succeeds
        ERROR if fails
*/
static fferr_t ctrl_param_WriteCustomCtrlSet(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    fferr_t                         fferr;
    Req_WriteControlParameters_t    *req = snd_buf;
    ControlParams_t                 *pCtrlParams = (void*)req->ControlParams[0];
    T_FBIF_PTB                      *p_PTB = p_block_instance->p_block_desc->p_block;

    util_PutU8(req->PIDDataSelector[0], 0);
    util_PutS16(pCtrlParams->PIDDataP[0], p_PTB->custom_control_set.component_0);
    util_PutS16(pCtrlParams->PIDDataI[0], p_PTB->custom_control_set.component_1);
    util_PutS16(pCtrlParams->PIDDataD[0], p_PTB->custom_control_set.component_2);
    util_PutS16(pCtrlParams->PIDDataPADJ[0], p_PTB->custom_control_set.component_3);
    util_PutS16(pCtrlParams->PIDDataBETA[0], p_PTB->custom_control_set.component_4);
    util_PutS16(pCtrlParams->PIDDataPOS_COMP[0], p_PTB->custom_control_set.component_5);
    util_PutS16(pCtrlParams->PIDDataDEADZONE[0], p_PTB->custom_control_set.component_6);
    util_PutS16(pCtrlParams->PIDDataNONLIN1[0], p_PTB->custom_control_set.component_7);

    fferr = mn_HART_acyc_cmd(173, snd_buf, HC173_REQ_LENGTH, rcv_buf);

    return fferr;
}

/* async hart command 171, 216 to write slector for control param set */
static fferr_t ctrl_param_WriteActivateCtrlSet(u16 selector, void* snd_buf, void* rcv_buf)
{
    fferr_t fferr;
    Req_WriteSettings_t* req = snd_buf;
    Req_WriteControlParametersSelector_t* req_216 = (void*)((u8*)snd_buf + HC171_REQ_LENGTH);

    util_PutU8(req->SubCmdNum[0], 216);
    util_PutU8(req_216->PIDDataSelector[0], (u8)selector);
    fferr = mn_HART_acyc_cmd(171, snd_buf, HC171_REQ_LENGTH + MN_OFFSETOF(Req_WriteControlParametersSelector_t, endmark), rcv_buf);

    return fferr;
}

/* \brief Since backup control set is changed by itself, not by user
            so the storage is not handled by Write_handler_PTB()
            and we need to do it manually
    param in:
        p_PTB: pointer to the objects of transducer block
    return:
        void
*/
static USIGN16 ctrl_param__WriteBackup(T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    T_FBS_WRITE_PARAM_LOCAL write_loc;
    T_FBIF_PTB *  p_PTB = p_block_instance->p_block_desc->p_block;

    write_loc.rel_idx = REL_IDX_PTB_BACKUP_CONTROL_SET;
    // Store out parameter in non-volatile RAM ----------------------
    write_loc.subindex      = 0;
    write_loc.length        = sizeof(p_PTB->backup_control_set);
    write_loc.source        = (USIGN8 *)&p_PTB->backup_control_set;
    if (appl_check_hm_state() == HM_RUN )
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }

    return fbs_write_param_loc (p_block_instance, &write_loc);
}


/* \brief execution codes for Read_handler_PTB(), when retrive REL_IDX_PTB_ACTIVATE_CONTROL_SET
    param in:
        p_block_instance:   pointer to the FF block
        p_read:             pointer to parameters for reading
        snd_buf:            pointer to memory, which is for send of HART
        rcv_buf:            pointer to memory, which is for receive of hart
    param out:

    return:
        OK if succeeds
        ERROR if fails
*/
fferr_t ctrl_param_ReadActiveCtrlSet(const T_FBIF_BLOCK_INSTANCE* p_block_instance, T_FBIF_READ_DATA * p_read, void* snd_buf, void* rcv_buf)
{
    u8 selector;
    fferr_t fferr;
    Req_ReadSettings_t* req = snd_buf;
    Rsp_ReadControlParametersSelector_t* rsp_216 = (void*)((u8*)rcv_buf + HC170_LENGTH + 2);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    UNUSED_OK(p_read);
    util_PutU8(req->SubCmdNum[0], 216);
    fferr = mn_HART_acyc_cmd(170, snd_buf, HC170_REQ_LENGTH, rcv_buf);

    if(fferr == E_OK)
    {
        selector = util_GetU8(rsp_216->PIDDataSelector[0]);
        p_PTB->active_control_set.component_0 = selector;
    }

    return fferr;
}

/* \brief execution codes for Read_handler_PTB(), when retrive REL_IDX_PTB_ACTIVE_CONTROL_SET, REL_IDX_PTB_CUSTOM_CONTROL_SET
    param in:
        p_block_instance:   pointer to the FF block
        rel_idx:            the index value of the parameter to be read
        snd_buf:            pointer to memory, which is for send of HART
        rcv_buf:            pointer to memory, which is for receive of hart
    param out:

    return:
        OK if succeeds
        ERROR if fails
*/
fferr_t ctrl_param_ReadCtrlSetParam(const T_FBIF_BLOCK_INSTANCE* p_block_instance, USIGN16 rel_idx, void* snd_buf, void* rcv_buf)
{
    fferr_t fferr;
    Req_ReadSettings_t* req = snd_buf;
    Req_ReadControlParametersExt_t* req_217 = (void*)((u8*)snd_buf + MN_OFFSETOF(Req_ReadSettings_t, endmark));
    Rsp_ReadControlParametersExt_t* rsp_217 = (void*)((u8*)rcv_buf + MN_OFFSETOF(Rsp_ReadSettings_t, endmark) + 2);
    ControlParams_t* pCtlSet = (void*)rsp_217->ControlParams[0];
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    _CONTROL_SET* pCtrlSet;

    struct buckets_t
    {
        INT16           selector;
        _CONTROL_SET    controlset;
    } *p = (void*)&p_PTB->active_control_set;
    //Assert my assumptions:
    //1. (No) padding
    CONST_ASSERT(sizeof(*p) == sizeof(p_PTB->active_control_set));
    //2. Alignment
    CONST_ASSERT(sizeof(p->controlset) == sizeof(p_PTB->custom_control_set));
    UNUSED_OK(p->selector);

    util_PutU8(req->SubCmdNum[0], 217);

    switch(rel_idx)
    {
    case REL_IDX_PTB_ACTIVE_CONTROL_SET:
        util_PutU8(req_217->PIDDataSelector[0], (u8)(p_PTB->active_control_set.component_0));
        pCtrlSet = &(p->controlset);
        break;

    case REL_IDX_PTB_CUSTOM_CONTROL_SET:
    default: //lint
        util_PutU8(req_217->PIDDataSelector[0], 0);
        pCtrlSet = &p_PTB->custom_control_set;
        break;
    }
    fferr = mn_HART_acyc_cmd(170, snd_buf, MN_OFFSETOF(Req_ReadSettings_t, endmark) + MN_OFFSETOF(Req_ReadControlParametersExt_t, endmark), rcv_buf);

    if(fferr == E_OK)
    {
        pCtrlSet->component_0 = util_GetS16(pCtlSet->PIDDataP[0]);
        pCtrlSet->component_1 = util_GetS16(pCtlSet->PIDDataI[0]);
        pCtrlSet->component_2 = util_GetS16(pCtlSet->PIDDataD[0]);
        pCtrlSet->component_3 = util_GetS16(pCtlSet->PIDDataPADJ[0]);
        pCtrlSet->component_4 = util_GetS16(pCtlSet->PIDDataBETA[0]);
        pCtrlSet->component_5 = util_GetS16(pCtlSet->PIDDataPOS_COMP[0]);
        pCtrlSet->component_6 = util_GetS16(pCtlSet->PIDDataDEADZONE[0]);
        pCtrlSet->component_7 = util_GetS16(pCtlSet->PIDDataNONLIN1[0]);
    }

    return fferr;
}

/* \brief execution codes for Write_handler_PTB(), when change on REL_IDX_PTB_ACTIVATE_CONTROL_SET
    param in:
        p_block_instance:   pointer to the FF block
        p_write:            ponter to the write parameters
        snd_buf:            pointer to memory, which is for send of HART
        rcv_buf:            pointer to memory, which is for receive of hart
    param out:

    return:
        OK if succeeds
        ERROR if fails
*/
fferr_t ctrl_param_WriteActivate(T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA *p_write, void* snd_buf, void* rcv_buf)
{
    fferr_t     fferr = E_OK;
    T_FBIF_PTB  *p_PTB = p_block_instance->p_block_desc->p_block;

    u8* p_selector = p_write->source;

    switch(*p_selector)
    {
        case ACTIVATE_CUSTOM_CONTROL_SET:
        case ACTIVATE_CONTROL_SET_1:
        case ACTIVATE_CONTROL_SET_2:
        case ACTIVATE_CONTROL_SET_3:
        case ACTIVATE_CONTROL_SET_4:
        case ACTIVATE_CONTROL_SET_5:
        case ACTIVATE_CONTROL_SET_6:
        case ACTIVATE_CONTROL_SET_7:
        {
            if(ACTIVATE_CUSTOM_CONTROL_SET == *p_selector)
            {
                fferr = ctrl_param_WriteCustomCtrlSet(p_block_instance, snd_buf, rcv_buf);
            }
            if(E_OK == fferr)
            {
                fferr = ctrl_param_WriteActivateCtrlSet(*p_selector, snd_buf, rcv_buf);
                if (E_OK == fferr)
                {
                    mn_memcpy((void*)&p_PTB->backup_control_set,
                    &(p_PTB->active_control_set),
                    sizeof(p_PTB->backup_control_set));
                    fferr = ctrl_param__WriteBackup(p_block_instance);
                }
            }
            break;
        }
        case RESTORE_CONTROL_SET:
        {
            fferr = ctrl_param_WriteActivateCtrlSet(p_PTB->backup_control_set.selector, snd_buf, rcv_buf);
            break;
        }
        case MAKE_ACTIVE_CONTROL_AS_CUSTOM_CONTROL:
        {
            struct buckets_t
            {
                INT16           selector;
                _CONTROL_SET    controlset;
            } *p = (void*)&p_PTB->active_control_set;
            //Assert my assumptions:
            //1. (No) padding
            CONST_ASSERT(sizeof(*p) == sizeof(p_PTB->active_control_set));
            //2. Alignment
            CONST_ASSERT(sizeof(p->controlset) == sizeof(p_PTB->custom_control_set) );
            UNUSED_OK(p->selector);

            _CONTROL_SET* pData = (_CONTROL_SET*)&p->controlset;
            mn_memcpy(&(p_PTB->custom_control_set), (void*)pData, sizeof(_CONTROL_SET));
            //fferr = ctrl_param_WriteCustomCtrlSet(p_block_instance, snd_buf, rcv_buf);
            break;
        }
        case DO_NOTHING:
        {
            break;
        }
        default:
        {
            fferr = E_ERROR;
            break;
        }
    }

    *p_selector = DO_NOTHING;
    return fferr;
}


/* \brief Routine called by Read_handler_PTB() to populate pPTB with Autotune parameters.
          Called when Read Handler is called with the index REL_IDX_PTB_AUTOTUNE.
    param in:
        p_block_instance:   pointer to the FF block
        snd_buf:            pointer to a buffer sent to HART
    param out:
        rcv_buf:            pointer to a buffer received from HART
    return:
        error code: OK if succeeds, ERROR if fails
*/
fferr_t ctrl_param_ReadAutotuneParam(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    fferr_t fferr;
    Req_ReadSettings_t* req = snd_buf;

    Rsp_ReadTuningParameters_t* rsp_189 = (void*)((u8*)rcv_buf + MN_OFFSETOF(Rsp_ReadSettings_t, endmark) + 2);
    AutotuneParams_t* pTuneParams = (void*)rsp_189->AutotuneParams[0];

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    _AUTOTUNE* pAutotuneParams = &p_PTB->autotune;

    util_PutU8(req->SubCmdNum[0], 189);
    fferr = mn_HART_acyc_cmd(170, snd_buf, MN_OFFSETOF(Req_ReadTuningParameters_t, endmark) + MN_OFFSETOF(Req_ReadSettings_t, endmark), rcv_buf);

    if(fferr == E_OK)
    {
        pAutotuneParams->component_0 = util_GetFloat(pTuneParams->DefaultSupplyPressure[0]);
        pAutotuneParams->component_1 = (s8)util_GetU8(pTuneParams->TuneAggressiveness[0]);
        pAutotuneParams->component_2 = util_GetU8(pTuneParams->TuneFlags[0]);
        pAutotuneParams->component_3 = util_GetU16(rsp_189->TuneCompletionCode[0]);
    }

    return fferr;
} // ----- end of ctrl_param_ReadAutotuneParam() -----

/** \brief Write Autotune params and start Autotune (combo)
*/
#define SUBCMDLENGTH    1u
fferr_t ctrl_param_Autotune(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    fferr_t fferr;
    u16     TuneCompletion;
    u8      buffLen;

    const T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    const _AUTOTUNE *pAutotuneParams = &p_PTB->autotune;
    AutotuneParams_t* pTuneParams189 = snd_buf;
    Req_WriteSettings_t* req = snd_buf; //pointer to the start of the buffer to put subcommand
    //pointer to the rest of the send buffer
    AutotuneParams_t* pTuneParams171 = (void*)((u8*)snd_buf + SUBCMDLENGTH);

    //AK: This is the original passage from Sergey. Not sure what it is for
    if ( p_PTB->mode_blk.target != MODE_MAN )
    {
        return E_FB_WRONG_MODE;
    }

    TuneCompletion = pAutotuneParams->component_3; //Completion sub-parameter of Autotune

    switch (TuneCompletion)
    {
        case AUTOTUNE_CANCEL:   //cancel process
            buffLen = 0u;
            fferr = mn_HART_acyc_cmd(253, snd_buf, buffLen, rcv_buf);
            break;

        case AUTOTUNE_START:    //start autotune
            util_PutFloat(pTuneParams189->DefaultSupplyPressure[0], pAutotuneParams->component_0);
            util_PutU8(pTuneParams189->TuneAggressiveness[0], (u8)pAutotuneParams->component_1);
            util_PutU8(pTuneParams189->TuneFlags[0], pAutotuneParams->component_2);
            fferr = mn_HART_acyc_cmd(189, snd_buf, sizeof(AutotuneParams_t), rcv_buf);
            break;

        default:  //write parameters only without starting autotune
            util_PutU8(req->SubCmdNum[0], 189); //write subcommand
            //write the rest of the buffer
            util_PutFloat(pTuneParams171->DefaultSupplyPressure[0], pAutotuneParams->component_0);
            util_PutU8(pTuneParams171->TuneAggressiveness[0], (u8)pAutotuneParams->component_1);
            util_PutU8(pTuneParams171->TuneFlags[0], pAutotuneParams->component_2);
            fferr = mn_HART_acyc_cmd(171, snd_buf, sizeof(AutotuneParams_t) + SUBCMDLENGTH, rcv_buf);
            break;
    }
    return fferr;
} // ----- end of ctrl_param_AutotuneParam() -----

/* \brief Routine called by Write_handler_PTB() when users write CUSTOM_CONTROL_SET
    param in:
        p_write:   pointer to the write block
    return:
        error code: OK if succeeds, ERROR if fails
*/
fferr_t ctrl_param_WriteCustom(const T_FBIF_WRITE_DATA *p_write)
{
    fferr_t fferr = E_OK;

    switch (p_write->subindex)
    {
        case 0:
        {
            _CONTROL_SET* p_controlset = (_CONTROL_SET*)(void*)p_write->source;
            if(
                (p_controlset->component_0 > P_HIGH_LIMIT) ||
                (p_controlset->component_1 > I_HIGH_LIMIT) ||
                (p_controlset->component_2 > D_HIGH_LIMIT) ||
                (p_controlset->component_3 < PADJ_LOW_LIMIT)  ||
                (p_controlset->component_3 > PADJ_HIGH_LIMIT) ||
                (p_controlset->component_4 < BETA_LOW_LIMIT)  ||
                (p_controlset->component_4 > BETA_HIGH_LIMIT) ||
                (p_controlset->component_5 < PCOMP_LOW_LIMIT)  ||
                (p_controlset->component_5 > PCOMP_HIGH_LIMIT) ||
                (p_controlset->component_6 > DEADZONE_HIGH_LIMIT) ||
                (p_controlset->component_7 > NONLIN_HIGH_LIMIT) ||
                (p_controlset->component_7 < NONLIN_LOW_LIMIT)
               )
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 1:
        {
            s16* p_P = (s16*)(void*)p_write->source;
            if(*p_P > P_HIGH_LIMIT)
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 2:
        {
            s16* p_I = (s16*)(void*)p_write->source;
            if(*p_I > I_HIGH_LIMIT)
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 3:
        {
            s16* p_D = (s16*)(void*)p_write->source;
            if(*p_D > D_HIGH_LIMIT)
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 4:
        {
            s16* p_Padj = (s16*)(void*)p_write->source;
            if((*p_Padj < PADJ_LOW_LIMIT) || (*p_Padj > PADJ_HIGH_LIMIT))
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 5:
        {
            s16* p_beta = (s16*)(void*)p_write->source;
            if((*p_beta < BETA_LOW_LIMIT) || (*p_beta > BETA_HIGH_LIMIT))
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 6:
        {
            s16* p_posComp = (s16*)(void*)p_write->source;
            if((*p_posComp < PCOMP_LOW_LIMIT) || (*p_posComp > PCOMP_HIGH_LIMIT))
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 7:
        {
            s16* p_deadZone = (s16*)(void*)p_write->source;
            if(*p_deadZone > DEADZONE_HIGH_LIMIT)
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        case 8:
        {
            s16* p_nonLin = (s16*)(void*)p_write->source;
            if((*p_nonLin > NONLIN_HIGH_LIMIT) || (*p_nonLin < NONLIN_LOW_LIMIT))
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        default:
        {
            fferr = E_FB_PARA_CHECK;
            break;
        }
    }

    return fferr;
}

/* This line marks the end of the source */

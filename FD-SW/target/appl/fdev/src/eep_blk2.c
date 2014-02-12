/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file eep_blk2.c
    \brief API for eep block 2 operation

    CPU: Any

    OWNER: EJ
    $Date: 11/01/12 11:19a $
    $Revision: 1 $
    $Author: Stanley Fu $

*/
/* $History: eep_blk2.c $
 *
 * *****************  Version 1  *****************
 * User: Stanley Fu    Date: 11/1/12    Time: 10:38p
 * Init version
*/
#include <softing_base.h>
#define  MODULE_ID      (COMP_FAPP + MOD_APPL_IF)
#include "base.h"
#include "eep_if.h"
#include "mnassert.h"

#include "eep_blk2.h"

typedef void func_get_p_buf_of_eep_blk2_t(void** pp_value, u16* p_len);
/*
    The order of the following functions should be according to eep_blk2_t structure.
    If a new structure need to be saved in eep block2, the 'get' function of the new structure should be appended.

func_get_p_buf_of_eep_blk2_t* const func_get_p_value_array[] =
{
    get_trend_counter_travel_acc,
    get_trend_counter_cycle_counter,
    get_trend_counter_position_err,
    get_Histogram_PosErr,
    get_AlertLogPtr,
    get_Position_Histogram_internal,

};
*/
typedef struct item_info_t
{
    size_t offset;
    func_get_p_buf_of_eep_blk2_t* fptr;
} item_info_t;

static item_info_t const func_get_p_value_array[] =
{
    {.offset = MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, trend_counter_travel_acc),          .fptr = get_trend_counter_travel_acc},
    {.offset = MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, trend_counter_cycle_counter),       .fptr = get_trend_counter_cycle_counter},
    {.offset = MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, trend_counter_position_err),        .fptr = get_trend_counter_position_err},
    {.offset = MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, Histogram_PosErr),                  .fptr = get_Histogram_PosErr},
    {.offset = MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, AlertLog),                          .fptr = get_AlertLogPtr},
    {.offset = MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, Position_Histogram_internal_array), .fptr = get_Position_Histogram_internal},
};

/* \brief store a specific part of eep_blk2_t structure into eep block 2
    \param
        offset: the offset of the value which will be stored in eep_blk2_t structure
        len: the length of the value
        p_value: the pointer to the value which will be stored
    \return
        result
*/
T_EEP_RESULT eep_blk2_store(u16 offset, u16 len, const void* p_value)
{
    u8*  p_dest;
    T_EEP_RESULT        eep_result;

    eep_AcquireMutex();

    eep_result = eep_provide_block (EEP_APPL_BLK2, sizeof(eep_blk2_t), (void**) &p_dest);
    if(eep_result == EEP_OK)
    {
        eep_result = eep_start_write (EEP_APPL_BLK2);
    }
    if(eep_result == EEP_OK)
    {
        osif_disable_all_tasks();
        mn_memcpy(p_dest+offset, p_value, len);
        osif_enable_all_tasks();
        eep_result = eep_write (EEP_APPL_BLK2, offset, len, EEP_WAIT);
    }

    eep_ReleaseMutex();

    return eep_result;
}

/* \brief It will be invoked by appl_config() to store default values that is 0 into eep block 2.
    \param
        void
    \return
        void
*/
void eep_blk2_store_default(void)
{
    void *  p_dest;
    T_EEP_RESULT        eep_result;

    eep_AcquireMutex();

    eep_result = eep_provide_block (EEP_APPL_BLK2, sizeof(eep_blk2_t), (VOID * *) &p_dest);

    if(eep_result == EEP_OK)
    {
        eep_result = eep_start_write (EEP_APPL_BLK2);
    }
    if(eep_result == EEP_OK)
    {
        osif_disable_all_tasks();
        mn_memset(p_dest, 0u, sizeof(eep_blk2_t));
        osif_enable_all_tasks();
        eep_result = eep_write (EEP_APPL_BLK2,0,sizeof(eep_blk2_t),EEP_WAIT);
    }

    eep_ReleaseMutex();

    MN_DBG_ASSERT(eep_result == EEP_OK);
}

/* \brief It will be invoked by appl_config() to apply eep block 2.
    \param
        void
    \return
        u8: error
*/
u8 eep_blk2_apply(void)
{
    T_EEP_BLOCK_DSC     eep_block_dsc;
    T_EEP_RESULT        eep_result;

    /* Apply for data block in the non-volatile memory ---------------------- */
    eep_block_dsc.block_id           = EEP_APPL_BLK2;
    eep_block_dsc.auto_write_length  = 0;
    eep_block_dsc.block_size         = sizeof(eep_blk2_t);

    eep_result = eep_config_change(1,&eep_block_dsc);

    if (eep_result == EEP_OK)
    {
        return E_OK;
    }

    return E_ERROR;
}

/* \brief It will be invoked by appl_config() to check the values in eep block 2 and load them.
    \param
        void
    \return
        u8: error
*/
u8 eep_blk2_check(void)
{
    u8*  p_eep_blk2;
    T_EEP_RESULT        eep_result;
    USIGN8              result;

    eep_AcquireMutex();
    eep_result = eep_provide_block (EEP_APPL_BLK2,sizeof(eep_blk2_t),(VOID * *)&p_eep_blk2);

    if (eep_result != EEP_OK)
    {
         result = E_ERROR;
    }
    else
    {
        result = E_OK;
        /* perform some checks and return E_OK or ~E_OK */
        //load values from NV
        for(u8 i = 0; i < NELEM(func_get_p_value_array); i++)
        {
            void* p_value;
            u16 len;

            func_get_p_value_array[i].fptr(&p_value, &len);

            osif_disable_all_tasks();
            mn_memcpy(p_value, p_eep_blk2+func_get_p_value_array[i].offset, len);
            osif_enable_all_tasks();
        }
    }
    eep_ReleaseMutex();
    return result;
}


/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file eep_blk3.c
    \brief API for eep block 3 operation

    CPU: Any

    OWNER: EJ
    $Date: 10/09/13 13:19 $
    $Revision: 1 $
    $Author: Victoria Huang $

*/

#include <softing_base.h>
#define  MODULE_ID      (COMP_FAPP + MOD_APPL_IF)
#include "base.h"
#include "eep_if.h"
#include "mnassert.h"
#include "eep_blk3.h"

/* \brief store a specific part of eep_blk3_t structure into eep block 3
    \param
        offset: the offset of the value which will be stored in eep_blk3_t structure
        len: the length of the value
        p_value: the pointer to the value which will be stored
    \return
        result
*/
T_EEP_RESULT eep_blk3_store(u16 offset, u16 len, const void* p_value)
{
    u8*  p_dest;
    T_EEP_RESULT        eep_result;

    eep_AcquireMutex();

    eep_result = eep_provide_block (EEP_APPL_BLK3, sizeof(eep_blk3_t), (void**) &p_dest);
    if(eep_result == EEP_OK)
    {
        eep_result = eep_start_write (EEP_APPL_BLK3);
    }
    if(eep_result == EEP_OK)
    {
        osif_disable_all_tasks();
        mn_memcpy(p_dest+offset, p_value, len);
        osif_enable_all_tasks();
        eep_result = eep_write (EEP_APPL_BLK3, offset, len, EEP_WAIT);
    }

    eep_ReleaseMutex();

    return eep_result;
}

/* \brief transfer a specific part from eep block 2 to eep block 3
    \param
        offsetdest: the offset of the value which will be stored in eep_blk3_t structure
        offsetsrc: the offset of the value which will be coped from eep_blk2_t structure
        len: the length of the value
        p_value: the pointer to the value which will be stored
    \return
        result
*/
T_EEP_RESULT eep_blk23_transfer(u16 offsetdest, u16 offsetsrc, u16 len)
{
    u8*  p_source;
    u8*  p_dest = NULL;
    T_EEP_RESULT        eep_result;

    eep_AcquireMutex();

    eep_result = eep_provide_block (EEP_APPL_BLK2, sizeof(eep_blk2_t), (void**) &p_source);
    if(eep_result == EEP_OK)
    {
        eep_result = eep_provide_block (EEP_APPL_BLK3, sizeof(eep_blk3_t), (void**) &p_dest);
    }
    if(eep_result == EEP_OK)
    {
        eep_result = eep_start_write (EEP_APPL_BLK3);
    }
    if(eep_result == EEP_OK)
    {
        MN_DBG_ASSERT(p_dest != NULL);
        osif_disable_all_tasks();
        mn_memcpy(p_dest + offsetdest, p_source + offsetsrc, len);
        osif_enable_all_tasks();
        eep_result = eep_write (EEP_APPL_BLK3, offsetdest, len, EEP_WAIT);
    }

    eep_ReleaseMutex();

    return eep_result;
}


/* \brief transfer a specific part from eep block 3 to eep block 2
    \param
        offsetdest: the offset of the value which will be stored in eep_blk2_t structure
        offsetsrc: the offset of the value which will be coped from eep_blk3_t structure
        len: the length of the value
        p_value: the pointer to the value which will be stored
    \return
        result
*/
T_EEP_RESULT eep_blk32_transfer(u16 offsetdest, u16 offsetsrc, u16 len)
{
    u8*  p_source;
    u8*  p_dest = NULL;
    T_EEP_RESULT        eep_result;

    eep_AcquireMutex();

    eep_result =eep_provide_block (EEP_APPL_BLK3, sizeof(eep_blk3_t), (void**) &p_source);
    if (eep_result == EEP_OK)
    {
        eep_result = eep_provide_block (EEP_APPL_BLK2, sizeof(eep_blk2_t), (void**) &p_dest);
    }

    if(eep_result == EEP_OK)
    {
        eep_result = eep_start_write (EEP_APPL_BLK2);
    }
    if(eep_result == EEP_OK)
    {
        MN_DBG_ASSERT(p_dest != NULL);
        osif_disable_all_tasks();
        mn_memcpy(p_dest + offsetdest, p_source + offsetsrc, len);
        osif_enable_all_tasks();
        eep_result = eep_write (EEP_APPL_BLK2, offsetdest, len, EEP_WAIT);
    }

    eep_ReleaseMutex();

    return eep_result;
}

/* \brief It will be invoked by appl_config() to store default values that is 0
   into eep block 3.
    \param
        void
    \return
        void
*/
void eep_blk3_store_default(void)
{
    void *  p_dest;
    T_EEP_RESULT        eep_result;

    eep_AcquireMutex();

    eep_result = eep_provide_block (EEP_APPL_BLK3, sizeof(eep_blk3_t), (VOID * *) &p_dest);

    if(eep_result == EEP_OK)
    {
        eep_result = eep_start_write (EEP_APPL_BLK3);
    }
    if(eep_result == EEP_OK)
    {
        osif_disable_all_tasks();
        mn_memset(p_dest, 0u, sizeof(eep_blk3_t));
        osif_enable_all_tasks();
        eep_result = eep_write (EEP_APPL_BLK3,0,sizeof(eep_blk3_t),EEP_WAIT);
    }

    eep_ReleaseMutex();

    MN_DBG_ASSERT(eep_result == EEP_OK);
}

/* \brief It will be invoked by appl_config() to apply eep block 3.
    \param
        void
    \return
        u8: error
*/
u8 eep_blk3_apply(void)
{
    T_EEP_BLOCK_DSC     eep_block_dsc;
    T_EEP_RESULT        eep_result;

    // Apply for data block in the non-volatile memory ----------------------
    eep_block_dsc.block_id           = EEP_APPL_BLK3;
    eep_block_dsc.auto_write_length  = 0;
    eep_block_dsc.block_size         = sizeof(eep_blk3_t);

    eep_result = eep_config_change(1, &eep_block_dsc);

    if (eep_result == EEP_OK)
    {
        return E_OK;
    }

    return E_ERROR;
}

/* \brief It will be invoked by appl_config() to check the values in eep block
    3 and load them.
    \param
        void
    \return
        u8: error
*/
u8 eep_blk3_check(void)
{
    u8*  p_eep_blk3;
    T_EEP_RESULT        eep_result;
    USIGN8              result;

    eep_AcquireMutex();
    eep_result = eep_provide_block (EEP_APPL_BLK3,sizeof(eep_blk3_t),(VOID * *)&p_eep_blk3);

    if (eep_result != EEP_OK)
    {
         result = E_ERROR;
    }
    else
    {
        result = E_OK;
    }
    eep_ReleaseMutex();
    return result;
}

/* \brief get a specific part of eep_blk3_t structure to p_value
    \param
        offset: the offset of the value which will be got from eep_blk3_t structure
        len: the length of the value
        p_value: the pointer to the value which will be got
    \return
        result
*/
T_EEP_RESULT eep_blk3_get(u16 offset, u16 len, void* p_value)
{
    u8*  p_eep_blk3;
    T_EEP_RESULT        eep_result;

    eep_result = eep_provide_block (EEP_APPL_BLK3,sizeof(eep_blk3_t),(VOID * *)&p_eep_blk3);

    if (eep_result == EEP_OK)
    {
        osif_disable_all_tasks();
        mn_memcpy(p_value, p_eep_blk3+offset, len);
        osif_enable_all_tasks();
    }

    return eep_result;
}



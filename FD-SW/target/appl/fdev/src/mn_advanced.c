/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_advanced.c
    \brief functions about parameter ADVANCED

    CPU: Any

    OWNER:
    $Date: 07/08/13 15:40p $
    $Revision: 1 $
    $Author: victoria huang $

*/
#include <softing_base.h>
#include <ptb.h>
#include <appl_int.h>
#include <fbif_dcl.h>

//Glue headers last
#include "mn_advanced.h"
#include "mnhart2ff.h"
#include "mn_fstate.h"
#include "bitutils.h"


#define PRAMETER_STANDARD_DEFAULT    {0xffffffffu, 0xffffffffu, 0xffffffffu, \
                                     0xffffffu, 0x7fffffffu, 0x180bfe00u, \
                                     0xfffffff8u, 0xffffffu, 0, 0, 0, 0, 0, 0, 0, 0}
#if 0
#define PRAMETER_ADVANCED_DEFAULT   {0xffffffffu, 0xffffffffu, 0xffffffffu, \
                                     0xffffffu, 0xffffffffu, 0xffffffffu, \
                                     0xffffffffu, 0xffffffu}
#endif

#define CMP_KEY                      0xFEEDBEEFu
#define PARA_ADV_LOCK_FU1_DWORD      0xFFFFEFu
#define PARA_ADV_UNLOCK_FU1_DWORD    0xFFFFFFu
#define PARA_ADV_LOCK_FU1_BYTE       0xEFu
#define PARA_ADV_UNLOCK_FU1_BYTE     0xFFu

#define PARA_ADV_READ_DWORD          4u
#define PARA_ADV_READ_BYTES          16u
#define PARA_ADV_REDWRT_DWORD        8u
#define PARA_ADV_DWORD               9u
#define DEVICE_ID_BYTES              32u
#define PARA_ADV_REDWRT_BYTES        32u
#define PARA_ADV_REWRCHE_BYTES       36u
#define CAL_TOTAL_DEVICE_ID_DWORD    16u
#define CAL_TOTAL_DEVICE_ID_BYTES    64u

#define state_CommitAdvanced         0
#define state_RestoreDefault         1
#define state_WriteOne               2
#define state_CommitBuffer           3
#define state_DoNothing              4

#define BEEN_DISABLED                1
#define TO_BE_DISABLE                0

typedef struct
{
    u16 ptb_index;
    u16 alert_offset;      //offset from the head of p_block
} UNIT_alert_info_t;
static const UNIT_alert_info_t ptb_param_readonly[] = {
    {REL_IDX_PTB_TRAVEL_ACCUMULATION_A_ALERT, MN_OFFSETOF(T_FBIF_PTB, travel_accumulation_a_alert)+MN_OFFSETOF(_TRAVEL_ALERT, enable)},
    {REL_IDX_PTB_TRAVEL_ACCUMULATION_B_ALERT, MN_OFFSETOF(T_FBIF_PTB, travel_accumulation_b_alert)+MN_OFFSETOF(_TRAVEL_ALERT, enable)},
    {REL_IDX_PTB_CYCLE_COUNTER_A_ALERT, MN_OFFSETOF(T_FBIF_PTB, cycle_counter_a_alert)+MN_OFFSETOF(_CYCLE_COUNTER_ALERT, enable)},
    {REL_IDX_PTB_CYCLE_COUNTER_B_ALERT, MN_OFFSETOF(T_FBIF_PTB, cycle_counter_b_alert)+MN_OFFSETOF(_CYCLE_COUNTER_ALERT, enable)},
    {REL_IDX_PTB_NEAR_CLOSED_ALERT,  MN_OFFSETOF(T_FBIF_PTB, near_closed_alert)+MN_OFFSETOF(_NEAR_CLOSED_ALERT, enable)},
    {REL_IDX_PTB_SUPPLY_PRESSURE_HI_ALERT,  MN_OFFSETOF(T_FBIF_PTB, supply_pressure_hi_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_5)},
    {REL_IDX_PTB_SUPPLY_PRESSURE_LO_ALERT,  MN_OFFSETOF(T_FBIF_PTB, supply_pressure_lo_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_5)},
    {REL_IDX_PTB_SUPPLY_PRESSURE_LOLO_ALERT,  MN_OFFSETOF(T_FBIF_PTB, supply_pressure_lolo_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_5)},
    {REL_IDX_PTB_TEMPERATURE_HI_ALERT,  MN_OFFSETOF(T_FBIF_PTB, temperature_hi_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_5)},
    {REL_IDX_PTB_TEMPERATURE_LO_ALERT,  MN_OFFSETOF(T_FBIF_PTB, temperature_lo_alert)+MN_OFFSETOF(_SYSTEM_ALERT, component_5)},
    {REL_IDX_PTB_IP_DRIVE_CURRENT_HI_ALERT,  MN_OFFSETOF(T_FBIF_PTB, ip_drive_current_hi_alert)+MN_OFFSETOF(_IP_DRIVE_CURRENT_ALERT, enable)},
    {REL_IDX_PTB_IP_DRIVE_CURRENT_LO_ALERT,  MN_OFFSETOF(T_FBIF_PTB, ip_drive_current_lo_alert)+MN_OFFSETOF(_IP_DRIVE_CURRENT_ALERT, enable)},
};

static const  u16 pFK_1 = 0x5AA5u;
static const  u16 pFK_2 = 0xA55Au;
static advanced_state advanced_flag = a_false;/*variable used for advanced device flag*/
static u32 sAdvancedBuffer[PARA_ADV_DWORD];

/* \brief calculate the first 32 bytes of the arrays which is used to calculate
    the userkey.
    param in:
        no
    return:
        no
*/
static u16 calcFrst(const s8 *DeviceId, u16 FK)
{
    u16 Frst = FK;
    for (u8 i = 0; i < PARA_ADV_REDWRT_BYTES; i++)
    {
        u16 xDvcId = (u16)((DeviceId[i]) ^ 0xAA);
        u16 DvcId = (u16)(DeviceId[i] & 0xFF);
        if (i % 2 != 0)
        {
            Frst += DvcId;
            DvcId = (u16)(DvcId << 7);
            Frst += DvcId;
            DvcId = (u16)(DvcId << 4);
            Frst += DvcId;
        }
        else
        {
            Frst += xDvcId;
            xDvcId = (u16)(xDvcId << 7);
            Frst += xDvcId;
            xDvcId = (u16)(xDvcId << 4);
            Frst += xDvcId;
        }

    }
    return Frst;
}

/* \brief calculate the last 32 bytes of the arrays which is used to calculate
    the userkey.
    param in:
        no
    return:
        no
*/
static u16 calcScnd(const s8 *DeviceId, u16 FK)
{
    u16 Scnd = FK;
    for (u8 i = PARA_ADV_REDWRT_BYTES; i < CAL_TOTAL_DEVICE_ID_BYTES; i++)
    {
        u16 xDvcId=(u16)((DeviceId[i]) ^ 0xAA);
        u16 DvcId=(u16)(DeviceId[i] & 0xFF);
        if (i % 2 != 0)
        {
            Scnd += DvcId;
            xDvcId = (u16)(xDvcId << 7);
            Scnd += xDvcId;
            DvcId = (u16)(DvcId << 11);
            Scnd += DvcId;
        } else
        {
            Scnd += xDvcId;
            DvcId = (u16)(DvcId << 7);
            Scnd += DvcId;
            xDvcId = (u16)(xDvcId << 11);
            Scnd += xDvcId;
        }
    }
    return Scnd;
}

/* \brief compare the input FFKey is euqal to the UserKey or not.
    param in:
        pDeviceId: advanced parameter read/write table and the device id together
        FFKey    £ºadvanced parameter sub-checksum parmater
    return:
        true or false, if the FFKey is equal to the UserKey which is calculated
        by pDevicedId and algrithom.
*/
static bool_t ffcheck_Advanced(const s8 *pDeviceId, u32 FFKey)
{
    u32  UserKey = 0x0;
    u16 ClcltdKeyFrst, ClcltdKeyScnd;

    ClcltdKeyFrst = calcFrst(pDeviceId, pFK_1);
    ClcltdKeyScnd = calcScnd(pDeviceId, pFK_2);
    UserKey += ClcltdKeyScnd;
    UserKey += ((u32)ClcltdKeyFrst) << 16;
    if (UserKey == FFKey)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* \brief calculate the UserKey by the input arrays.
    param in:
        pDeviceId: advanced parameter read/write table and the device id together
    return:
        UserKey which is calculated by input pDeviceId and algrithom.
*/
static u32 ffcheck_GenerateUkey(const s8 *pDeviceId)
{
    u32  UserKey = 0x0;
    u16 ClcltdKeyFrst, ClcltdKeyScnd;

    ClcltdKeyFrst = calcFrst(pDeviceId, pFK_1);
    ClcltdKeyScnd = calcScnd(pDeviceId, pFK_2);
    UserKey += ClcltdKeyScnd;
    UserKey += ((u32)ClcltdKeyFrst) << 16;

    return UserKey;
}

/* \brief chcek the device is advanced or not by the advanced parameter
    read/write table.
    param in:
        no
    return:
        true or false, if the device is advanced type, return true.
*/
static bool_t ffcheck_AdvancedByBits(void)
{
    T_FBIF_BLOCK_INSTANCE * p_block_instance = fbs_get_block_inst(ID_PTB_1);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    u8 uSumRead = 0;
    u8 uSumWrite = 0;
    u8 i;
    u32 temp;

    //count the numbers of 1 in the Read_enable arrays.
    for (i = 0; i < PARA_ADV_READ_DWORD; i++)
    {
        temp = p_PTB->advanced[i];
        while(temp != 0)
        {
            temp &= temp-1;
            uSumRead ++;
        }
    }
    //count the numbers of 1 in the Write_enable arrays.
    for (i = PARA_ADV_READ_DWORD; i < PARA_ADV_REDWRT_DWORD; i++)
    {
        temp = p_PTB->advanced[i];
        //if the last key, ignore factory_use_1 parameter bit.
        if (i == PARA_ADV_REDWRT_DWORD - 1 && temp == PARA_ADV_LOCK_FU1_DWORD)
        {
            //ignore the factory_use_1 bit.
            temp = PARA_ADV_UNLOCK_FU1_DWORD;
        }
        while(temp != 0)
        {
            temp &= temp-1;
            uSumWrite ++;
        }
    }

    //check the number of the read and write is equal to the number
    //of TB blcok parameter index or not.
    if (uSumRead == uSumWrite &&
        uSumRead == p_block_instance->p_block_desc->no_of_param)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* \brief chcek the device is advanced or not by the advanced parameter
    read/write table.
    param in:
        no
    return:
        the bit 1 numbers in the array.
*/
static u8 ffcheck_CalculateBits(const u8 *pAdvanced)
{
    u8 uSum = 0;
    u8 i, temp;

    //count the numbers of 1 in the write_enable arrays.
    for (i = PARA_ADV_READ_BYTES; i < PARA_ADV_REDWRT_BYTES; i++)
    {
        temp = pAdvanced[i];
        //if the last key, ignore factory_use_1 parameter bit.
        if (i == PARA_ADV_REDWRT_BYTES - 4 && temp == PARA_ADV_LOCK_FU1_BYTE)
        {
            //ignore the factory_use_1 bit.
            temp = PARA_ADV_UNLOCK_FU1_BYTE;
        }
        while(temp != 0)
        {
            temp &= temp - 1;
            uSum ++;
        }
    }

    return uSum;
}

/*\brief clear the advanced buffer
    param in:
        no
    return:
        no
*/
static void ffcheck_ClrBuffer()
{
    u8 i;
    for (i = 0; i < PARA_ADV_DWORD; i++)
    {
        sAdvancedBuffer[i] = 0;
    }
    return;
}


/* \brief set the advanced device type flag advanced_flag.
    param in:
        no
    return:
        no
*/
static void ffcheck_SetAdvancedFlag(void)
{
    osif_disable_all_tasks();
    advanced_flag = a_true;
    osif_enable_all_tasks();
}

/* \brief clear the advanced device type flag advanced_flag.
    param in:
        no
    return:
        no
*/
static void ffcheck_ClrAdvancedFlag(void)
{
    osif_disable_all_tasks();
    advanced_flag = a_false;
    osif_enable_all_tasks();
}


/* \brief disable the alert enalbe when the device is standard type.
    param in:
        p_block_instance: pointer to function block
         p_read         : pointer to a read description block.

    return:
        FF error
*/
static fferr_t ffcheck_AlertEnableChange()
{
    T_FBIF_BLOCK_INSTANCE * p_block_instance = fbs_get_block_inst(ID_PTB_1);
    fferr_t fferr = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    T_FBIF_WRITE_DATA p_write;
    u8 i, temp;

    for(i = 0; i < NELEM(ptb_param_readonly); i ++)
    {
        temp = TO_BE_DISABLE;
        if (0 == *((u8 *)p_PTB + ptb_param_readonly[i].alert_offset) )
        {
            temp = BEEN_DISABLED;
        }
        if (ptb_param_readonly[i].ptb_index != REL_IDX_PTB_IP_DRIVE_CURRENT_HI_ALERT
            && ptb_param_readonly[i].ptb_index != REL_IDX_PTB_IP_DRIVE_CURRENT_LO_ALERT)
        {
            p_write.subindex  = 6;
        }
        else
        {
            p_write.subindex  = 7;
        }
        p_write.rel_idx       = ptb_param_readonly[i].ptb_index;
        p_write.dest          =
            (USIGN8 *)(void*)((u8 *)p_PTB + ptb_param_readonly[i].alert_offset);
        p_write.length        = sizeof(u8);
        p_write.source        = (USIGN8 *)(void *)&temp;
        p_write.phase         = POST_WRITE_ACTION;
        if (TO_BE_DISABLE == temp)
        {
            if (appl_check_hm_state() == HM_RUN )
            {
                p_write.startup_sync = FALSE;
            } else
            {
                p_write.startup_sync = TRUE;
            }
            fferr = fbs_write_param(p_block_instance,&p_write);
            if (fferr != E_OK)
            {
                break;
            }
        }
    }

    return fferr;
}


/* \brief write advanced parameter
    param in:
        p_block_instance: pointer to function block
        p_write:  pointer to write resource data

    return:
        FF error
*/
fferr_t ffcheck_WriteAdvanced
(
    const T_FBIF_BLOCK_INSTANCE * p_block_instance,
    T_FBIF_WRITE_DATA *p_write
)
{
    fferr_t fferr = E_OK;
    u32 uDeviceId[CAL_TOTAL_DEVICE_ID_DWORD] = PRAMETER_STANDARD_DEFAULT;
    STRINGV pd_tag[DEVICE_ID_BYTES], dev_id[DEVICE_ID_BYTES];
    u8 dev_addr, uSum, i, state;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    //got the device id.
    get_sm_config_data(&dev_addr, pd_tag, dev_id);

    switch (p_write->subindex)
    {
    case 0:
        //if we want to set the device to advanced or standard type,
        //check the key.we will change the parameter only when the key
        //is correct.
        state = state_CommitAdvanced;

        if (a_true == ffcheck_GetAdvancedFlag())
        {
            //if we write the strange string, the device will be
            //set back to standard mode, just for testing;
            //we can write the key to parameter first 4 bytes.
            u32 uCmpkey = mn_pull_u32((void *)(&p_write->source[0]));
            if (uCmpkey == CMP_KEY)
            {
                state = state_RestoreDefault;
            }
            else
            {
                fferr = E_FB_DATA_NO_WRITE;
                state = state_DoNothing;
            }
        }
        break;

    case 1:
        //save the write value to the arrays.
        state = state_WriteOne;
        if (a_true == ffcheck_GetAdvancedFlag())
        {
            //if we write the strange string, the device will be
            //set back to standard mode, just for testing;
            //we can write the key to parameter first 4 bytes.
            u32 uCmpkey = mn_pull_u32((void *)(&p_write->source[0]));
            if (uCmpkey == CMP_KEY)
            {
                state = state_RestoreDefault;
            }
            else
            {
                fferr = E_FB_DATA_NO_WRITE;
                state = state_DoNothing;
            }
        }
        break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        if (a_true == ffcheck_GetAdvancedFlag())
        {
            fferr = E_FB_DATA_NO_WRITE;
            state = state_DoNothing;
        }
        else
        {
            state = state_WriteOne;
        }
        break;

    case 9:
        if (a_true == ffcheck_GetAdvancedFlag())
        {
            fferr = E_FB_DATA_NO_WRITE;
            state = state_DoNothing;
        }
        else
        {
            state = state_CommitBuffer;
        }
        break;

    case 10:
    default:
        state = state_DoNothing;
        break;
    }

    switch (state)
    {
    case state_RestoreDefault:
        fferr = ffcheck_AlertEnableChange();
        if ( fferr == E_OK)
        {
            //set the calculate arrays.
            //mn_memcpy((void *)uDeviceId, (void*)uDeviceIdDefault, PARA_ADV_REDWRT_BYTES);
            mn_memcpy(&uDeviceId[PARA_ADV_REDWRT_DWORD], (void *)dev_id, DEVICE_ID_BYTES);
            mn_memcpy((void *)(&p_PTB->advanced[0]),(void *)uDeviceId, PARA_ADV_REDWRT_BYTES);
            p_PTB->advanced[PARA_ADV_REDWRT_DWORD] = ffcheck_GenerateUkey((s8 *)(void*)uDeviceId);
            //change the write source to set the device to standard type.
            p_write->source = (u8*)(void*)p_PTB->advanced;
            p_write->subindex = 0;
            p_write->length = PARA_ADV_REWRCHE_BYTES;
            p_write->dest = (u8 *)(void*)p_PTB->advanced;
            ffcheck_ClrAdvancedFlag();
            ffcheck_ClrBuffer();
        }
        break;

    case state_CommitAdvanced:
        mn_memcpy(uDeviceId, p_write->source, PARA_ADV_REDWRT_BYTES);
        mn_memcpy(&uDeviceId[PARA_ADV_REDWRT_DWORD], (void *)dev_id, DEVICE_ID_BYTES);
        if (ffcheck_Advanced((s8 *)(void*)uDeviceId,
            mn_pull_u32((void *)(&p_write->source[PARA_ADV_REDWRT_BYTES]))))
        {
            uSum = ffcheck_CalculateBits(p_write->source);
            if (uSum == p_block_instance->p_block_desc->no_of_param)
            {
                ffcheck_SetAdvancedFlag();
            }
            else
            {
                fferr = ffcheck_AlertEnableChange();
            }
            ffcheck_ClrBuffer();
            //fferr = E_OK;
        }
        else
        {
            fferr = E_FB_PARA_LIMIT;
        }
        break;
    case state_WriteOne:
        //save the write value to the arrays.
        mn_memcpy(&sAdvancedBuffer[p_write->subindex - 1],
                  p_write->source, PARA_ADV_READ_DWORD);
        fferr = E_FB_PARA_LIMIT;
        break;

    case state_CommitBuffer:
        //save the write value to the arrays.
        mn_memcpy(&sAdvancedBuffer[p_write->subindex - 1],
                  p_write->source, PARA_ADV_READ_DWORD);
        //maybe we just change part of the parameter
        for (i = 0; i < p_write->subindex; i++)
        {
            //if buffer is not initialized, copy it from PTB.
            if (0 == sAdvancedBuffer[i] && p_PTB->advanced[i] != 0)
            {
                sAdvancedBuffer[i] = p_PTB->advanced[i];
            }
        }

        //if we want to set the device to advanced or standard type,
        //check the key.we will change the parameter only when the key
        //is correct.
        mn_memcpy(uDeviceId, sAdvancedBuffer, PARA_ADV_REDWRT_BYTES);
        mn_memcpy(&uDeviceId[PARA_ADV_REDWRT_DWORD], (void *)dev_id, DEVICE_ID_BYTES);
        if (ffcheck_Advanced((s8 *)(void*)uDeviceId, sAdvancedBuffer[PARA_ADV_REDWRT_DWORD]))
        {
            p_write->subindex = 0;
            p_write->length = PARA_ADV_REWRCHE_BYTES;

            mn_memcpy((void *)(&p_PTB->advanced[0]),
                (u8*)(void*)sAdvancedBuffer, PARA_ADV_REWRCHE_BYTES);
            p_write->source = (u8*)(void*)p_PTB->advanced;
            p_write->dest = (u8 *)(void*)p_PTB->advanced;

            uSum = ffcheck_CalculateBits((u8 *)sAdvancedBuffer);
            if (uSum == p_block_instance->p_block_desc->no_of_param)
            {
                ffcheck_SetAdvancedFlag();
                fferr = E_OK;
            }
            else
            {
                ffcheck_ClrAdvancedFlag();
                fferr = ffcheck_AlertEnableChange();
            }
        }
        else
        {
            fferr = E_FB_PARA_LIMIT;
        }
        ffcheck_ClrBuffer();
        break;

    case state_DoNothing:
    default:
        break;
    }

    return fferr;
}


/* \brief get the advanced device type flag advanced_flag.
    param in:
        no
    return:
        no
*/
advanced_state ffcheck_GetAdvancedFlag(void)
{
    return advanced_flag;
}

/* \brief check the device is andvanced or standard type,
    if the device is not initialized, initial it with the
    advanced type during the testing phase, and when deliver,
    initial it with the standard type.
    param in:
        no
    return:
        no
*/
void ffcheck_AdvancedOnStartUp(void)
{
    u32 uDeviceId[CAL_TOTAL_DEVICE_ID_DWORD];
    STRINGV pd_tag[DEVICE_ID_BYTES], dev_id[DEVICE_ID_BYTES];
    u8 dev_addr;

    T_FBIF_BLOCK_INSTANCE * p_block_instance = fbs_get_block_inst(ID_PTB_1);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    //got the device id.
    get_sm_config_data(&dev_addr, pd_tag, dev_id);

    mn_memcpy(uDeviceId, (void *)(p_PTB->advanced), PARA_ADV_REDWRT_BYTES);
    mn_memcpy(&uDeviceId[PARA_ADV_REDWRT_DWORD], (void *)dev_id, DEVICE_ID_BYTES);

    //if check the key valid, and check if the read/write enable bit are all 1,
    // change the device type from standard to advanced.
    if (ffcheck_Advanced((s8 *)(void*)uDeviceId, p_PTB->advanced[8]))
    {
        if (ffcheck_AdvancedByBits())
        {
            ffcheck_SetAdvancedFlag();
        }
        else
        {
            ffcheck_ClrAdvancedFlag();
        }
    }
    else
    {
        ffcheck_ClrAdvancedFlag();
    }
    return;
}

/* \brief check if the device is standard type, disable the advanced parameter
    write function, and will reject the write operation.
    param in:
        p_block_instance: pointer to function block
        p_write:  pointer to write resource data

    return:
        FF error
*/
fferr_t ffcheck_WriteFilter
(
    const T_FBIF_BLOCK_INSTANCE * p_block_instance,
    const T_FBIF_WRITE_DATA *p_write
)
{
    fferr_t fferr = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    bool_t bBit;

    //filter for writeable parameters.
    bBit = util_GetBit(p_write->rel_idx,
       (u8*)(void*)&p_PTB->advanced[PARA_ADV_READ_DWORD], PARA_ADV_READ_BYTES);
    if ((!bBit) && (p_write->rel_idx != REL_IDX_PTB_ADVANCED) )
    {
        fferr = E_FB_WRITE_LOCK;
    }

    //check when the TB.actual mode is LO,reject the parameter write except the
    //MODE_BLK,APP_MODE,Factory_USE1 and Advanced key;
    if (E_OK == fferr)
    {
        if (MODE_LO == p_PTB->mode_blk.actual)
        {
            if (p_write->rel_idx != REL_IDX_PTB_PTB
                &&p_write->rel_idx != REL_IDX_PTB_MODE_BLK
                && p_write->rel_idx != REL_IDX_PTB_APP_MODE
                && p_write->rel_idx != REL_IDX_PTB_ADVANCED
                && p_write->rel_idx != REL_IDX_PTB_FACTORY_USE_1)
            {
                fferr = E_FB_WRONG_MODE;
            }
        }
    }

    return fferr;
}

/* This line marks the end of the source */

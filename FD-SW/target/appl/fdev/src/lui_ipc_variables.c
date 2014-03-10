/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file lui_ipc_variables.c
    \brief "ipc variables" handler

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/

#include <stdlib.h>
#include <string.h>

#include <softing_base.h>
#include <fbif_dcl.h>
#include <appl_int.h>
#include "mnassert.h"
#include "ffdefs.h"
#include "lui_ipc_variables.h"
#include "ipcdefs.h"
#include "ipcvarids.h"
#include "hartdef.h"
#include "bufutils.h"
#include <float.h>
#include <math.h>
#include "asciicodes.h"
#include "fbs_api.h"
#include "fbif_idx.h"
#include "mnhart2ff.h"
#include "pressure_range.h"
#include "pressuredef.h"

#define FLOAT_DISPLAY_PRECISION     2U

#define OFFSET_DISABLE            0xffff
#define BLOCK_IDX_DISABLE         0xff

#define ONE_BYTE_LEN            1
#define TWO_BYTES_LEN           2
#define FOUR_BYTES_LEN          4

#define FF_DEVICE_ADDR_LEN      1u
#define BLOCK_TAG_LEN           FF_TAG_MAX_LEN
#define BLOCK_ACTUAL_MODE_LEN   ONE_BYTE_LEN
#define BLOCK_ERROR_LEN         TWO_BYTES_LEN

#define LONG_VAR_BUF_LEN        IPC_WRITE_ARRAY_DATASIZE

#define CUSTOM_VAR_DISABLED              0
#define CUSTOM_VAR_ENABLED               1
#define CUSTOM_VAR_FLOAT_ENABLED         2
#define CUSTOM_VAR_DEVICE_ID_ENABLED     3
#define CUSTOM_VAR_DEVICE_TAG_ENABLED    4
#define CUSTOM_VAR_DEVICE_ADDR_ENABLED   3


static u8 is_enabled_float_var(IPC_Variable_IDs_t var_id);
static u8 is_enabled_var(IPC_Variable_IDs_t var_id);
static u8 device_param(IPC_Variable_IDs_t var_id);

static u8 float_str[FLOAT_STRING_LEN]; //the first 8 bytes is the formated value. The last byte is the status of this value
static STRINGV pd_tag[32];
static STRINGV dev_id[32];

typedef u8 func_is_enable_var_t(IPC_Variable_IDs_t var_id);

typedef struct
{
    IPC_Variable_IDs_t id;
    u8 len;
    u8 block_idx;
    u16 value_offset;      //offset from the head of p_block
    u16 status_offset;     //offset from the head of p_block. the OFFSET_DISABLE indicate the status is disabled
    func_is_enable_var_t* fp_is_enable_var;
} HART_short_variable_info_t;

typedef struct
{
    HART_short_variable_info_t info;
    u32 value;
    u8 status;
} HART_short_variable_t;

typedef struct
{
    IPC_Variable_IDs_t id;
    u8 len;
    u8 block_idx;
    u16 value_offset;    //offset from the head of p_block
    u16 status_offset;     //offset from the head of p_block. the OFFSET_DISABLE indicate the status is disabled
    func_is_enable_var_t* fp_is_enable_var;
} HART_long_variable_info_t;

typedef struct
{
    HART_long_variable_info_t info;
    void* p_value;
} HART_long_variable_t;

/* a table to hold the information for short variables*/
const HART_short_variable_info_t short_var_info_array[] =
{
//   -----------id-----------      ---------len---------      block_idx     ---------------------value_offset--------------------------                            status_offset                                                function pointer
    {IPC_TB_UI_CUST_CONF_0,        ONE_BYTE_LEN,              ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, ui_custom_configuration)+MN_OFFSETOF(_UI_CUST_CONF, component_0),    OFFSET_DISABLE,                                              NULL},
    {IPC_TB_UI_CUST_CONF_1,        ONE_BYTE_LEN,              ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, ui_custom_configuration)+MN_OFFSETOF(_UI_CUST_CONF, component_1),    OFFSET_DISABLE,                                              NULL},
    {IPC_TB_BLOCK_ACTUAL_MODE,     BLOCK_ACTUAL_MODE_LEN,     ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, mode_blk)+MN_OFFSETOF(MODE, actual),                                 OFFSET_DISABLE,                                              NULL},
    {IPC_TB_BLOCK_ERROR,           BLOCK_ERROR_LEN,           ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, block_err),                                                       OFFSET_DISABLE,                                              NULL},

    {IPC_AO_BLOCK_ACTUAL_MODE,     BLOCK_ACTUAL_MODE_LEN,     ID_AOFB_1,    MN_OFFSETOF(T_FBIF_AOFB, mode_blk)+MN_OFFSETOF(MODE, actual),                                OFFSET_DISABLE,                                              NULL},
    {IPC_AO_BLOCK_ERROR,           BLOCK_ERROR_LEN,           ID_AOFB_1,    MN_OFFSETOF(T_FBIF_AOFB, block_err),                                                      OFFSET_DISABLE,                                              NULL},


    {IPC_DO_BLOCK_ACTUAL_MODE,     BLOCK_ACTUAL_MODE_LEN,     ID_DOFB_1,    MN_OFFSETOF(T_FBIF_DOFB, mode_blk)+MN_OFFSETOF(MODE, actual),                                OFFSET_DISABLE,                                              NULL},
    {IPC_DO_BLOCK_ERROR,           BLOCK_ERROR_LEN,           ID_DOFB_1,    MN_OFFSETOF(T_FBIF_DOFB, block_err),                                                      OFFSET_DISABLE,                                              NULL},
    {IPC_DO_SP_D,                  ONE_BYTE_LEN,              ID_DOFB_1,    MN_OFFSETOF(T_FBIF_DOFB, sp_d)+MN_OFFSETOF(DISCRETE_S, value),                               MN_OFFSETOF(T_FBIF_DOFB, sp_d)+MN_OFFSETOF(DISCRETE_S, status),    is_enabled_var},
    {IPC_DO2_SP_D,                 ONE_BYTE_LEN,              ID_DOFB_2,    MN_OFFSETOF(T_FBIF_DOFB, sp_d)+MN_OFFSETOF(DISCRETE_S, value),                               MN_OFFSETOF(T_FBIF_DOFB, sp_d)+MN_OFFSETOF(DISCRETE_S, status),    is_enabled_var},

    {IPC_PID_BLOCK_ACTUAL_MODE,    BLOCK_ACTUAL_MODE_LEN,     ID_PIDFB_1,   MN_OFFSETOF(T_FBIF_PIDFB, mode_blk)+MN_OFFSETOF(MODE, actual),                               OFFSET_DISABLE,                                              NULL},
    {IPC_PID_BLOCK_ERROR,          BLOCK_ERROR_LEN,           ID_PIDFB_1,   MN_OFFSETOF(T_FBIF_PIDFB, block_err),                                                     OFFSET_DISABLE,                                              NULL},

    {IPC_PID2_BLOCK_ACTUAL_MODE,   BLOCK_ACTUAL_MODE_LEN,     ID_PIDFB_2,   MN_OFFSETOF(T_FBIF_PIDFB, mode_blk)+MN_OFFSETOF(MODE, actual),                               OFFSET_DISABLE,                                              NULL},
    {IPC_PID2_BLOCK_ERROR,         BLOCK_ERROR_LEN,           ID_PIDFB_2,   MN_OFFSETOF(T_FBIF_PIDFB, block_err),                                                     OFFSET_DISABLE,                                              NULL},

    {IPC_RB_BLOCK_ACTUAL_MODE,     BLOCK_ACTUAL_MODE_LEN,     ID_RESB_1,    MN_OFFSETOF(T_FBIF_RESB, mode_blk)+MN_OFFSETOF(MODE, actual),                                OFFSET_DISABLE,                                              NULL},
    {IPC_RB_BLOCK_ERROR,           BLOCK_ERROR_LEN,           ID_RESB_1,    MN_OFFSETOF(T_FBIF_RESB, block_err),                                                      OFFSET_DISABLE,                                              NULL},

    {IPC_DO2_BLOCK_ACTUAL_MODE,    BLOCK_ACTUAL_MODE_LEN,     ID_DOFB_2,    MN_OFFSETOF(T_FBIF_DOFB, mode_blk)+MN_OFFSETOF(MODE, actual),                                OFFSET_DISABLE,                                              NULL},
    {IPC_DO2_BLOCK_ERROR,          BLOCK_ERROR_LEN,           ID_DOFB_2,    MN_OFFSETOF(T_FBIF_DOFB, block_err),                                                      OFFSET_DISABLE,                                              NULL},

    {IPC_DEVICE_ADDRESS,           FF_DEVICE_ADDR_LEN,        BLOCK_IDX_DISABLE,     OFFSET_DISABLE,                                                                  OFFSET_DISABLE,                                              device_param},
};

/* a table to hold the information for array variables*/
const HART_long_variable_info_t long_var_info_array[] =
{
//   -----------id-----      ---------len---------      block_idx     ---------------------value_offset--------------------------                                           status_offset                                                   function pointer
    {IPC_AO_BLOCK_TAG,       BLOCK_TAG_LEN,             ID_AOFB_1,    MN_OFFSETOF(T_FBIF_AOFB, blk_data)+MN_OFFSETOF(F_BLOCK, block_tag),                                   OFFSET_DISABLE,                                                 NULL},
    {IPC_DO_BLOCK_TAG,       BLOCK_TAG_LEN,             ID_DOFB_1,    MN_OFFSETOF(T_FBIF_DOFB, blk_data)+MN_OFFSETOF(F_BLOCK, block_tag),                                   OFFSET_DISABLE,                                                 NULL},
    {IPC_PID_BLOCK_TAG,      BLOCK_TAG_LEN,             ID_PIDFB_1,   MN_OFFSETOF(T_FBIF_PIDFB, blk_data)+MN_OFFSETOF(F_BLOCK, block_tag),                                  OFFSET_DISABLE,                                                 NULL},
    {IPC_RB_BLOCK_TAG,       BLOCK_TAG_LEN,             ID_RESB_1,    MN_OFFSETOF(T_FBIF_RESB, blk_data)+MN_OFFSETOF(F_BLOCK, block_tag),                                   OFFSET_DISABLE,                                                 NULL},
    {IPC_TB_BLOCK_TAG,       BLOCK_TAG_LEN,             ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, blk_data)+MN_OFFSETOF(F_BLOCK, block_tag),                                    OFFSET_DISABLE,                                                 NULL},

    {IPC_PID2_BLOCK_TAG,     BLOCK_TAG_LEN,             ID_PIDFB_2,   MN_OFFSETOF(T_FBIF_PIDFB, blk_data)+MN_OFFSETOF(F_BLOCK, block_tag),                                  OFFSET_DISABLE,                                                 NULL},
    {IPC_DO2_BLOCK_TAG,      BLOCK_TAG_LEN,             ID_DOFB_2,    MN_OFFSETOF(T_FBIF_DOFB, blk_data)+MN_OFFSETOF(F_BLOCK, block_tag),                                   OFFSET_DISABLE,                                                 NULL},

    {IPC_DEVICE_ID,          FF_DEVICE_ID_LEN,         BLOCK_IDX_DISABLE,   OFFSET_DISABLE,                                                                                OFFSET_DISABLE,                                                 device_param},
    {IPC_DEVICE_TAG,         FF_DEVICE_TAG_LEN,          BLOCK_IDX_DISABLE,   OFFSET_DISABLE,                                                                                OFFSET_DISABLE,                                                 device_param},

    {IPC_TB_FINAL_SP,              FLOAT_STRING_LEN,            ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, setpoint)+MN_OFFSETOF(FLOAT_S, value),                               MN_OFFSETOF(T_FBIF_PTB, setpoint)+MN_OFFSETOF(FLOAT_S, status),    is_enabled_float_var},
    {IPC_TB_FINAL_POS,             FLOAT_STRING_LEN,            ID_PTB_1,     MN_OFFSETOF(T_FBIF_PTB, final_position_value)+MN_OFFSETOF(FLOAT_S, value),         MN_OFFSETOF(T_FBIF_PTB, final_position_value)+MN_OFFSETOF(FLOAT_S, status),  is_enabled_float_var},

    {IPC_PID_PV,                   FLOAT_STRING_LEN,            ID_PIDFB_1,   MN_OFFSETOF(T_FBIF_PIDFB, pv)+MN_OFFSETOF(FLOAT_S, value),                                   MN_OFFSETOF(T_FBIF_PIDFB, pv)+MN_OFFSETOF(FLOAT_S, status),        is_enabled_float_var},
    {IPC_PID_SP,                   FLOAT_STRING_LEN,            ID_PIDFB_1,   MN_OFFSETOF(T_FBIF_PIDFB, sp)+MN_OFFSETOF(FLOAT_S, value),                                   MN_OFFSETOF(T_FBIF_PIDFB, sp)+MN_OFFSETOF(FLOAT_S, status),        is_enabled_float_var},
    {IPC_PID_OUT,                  FLOAT_STRING_LEN,            ID_PIDFB_1,   MN_OFFSETOF(T_FBIF_PIDFB, out)+MN_OFFSETOF(FLOAT_S, value),                                  MN_OFFSETOF(T_FBIF_PIDFB, out)+MN_OFFSETOF(FLOAT_S, status),       is_enabled_float_var},
    {IPC_PID2_PV,                  FLOAT_STRING_LEN,            ID_PIDFB_2,   MN_OFFSETOF(T_FBIF_PIDFB, pv)+MN_OFFSETOF(FLOAT_S, value),                                   MN_OFFSETOF(T_FBIF_PIDFB, pv)+MN_OFFSETOF(FLOAT_S, status),        is_enabled_float_var},
    {IPC_PID2_SP,                  FLOAT_STRING_LEN,            ID_PIDFB_2,   MN_OFFSETOF(T_FBIF_PIDFB, sp)+MN_OFFSETOF(FLOAT_S, value),                                   MN_OFFSETOF(T_FBIF_PIDFB, sp)+MN_OFFSETOF(FLOAT_S, status),        is_enabled_float_var},
    {IPC_PID2_OUT,                 FLOAT_STRING_LEN,            ID_PIDFB_2,   MN_OFFSETOF(T_FBIF_PIDFB, out)+MN_OFFSETOF(FLOAT_S, value),                                  MN_OFFSETOF(T_FBIF_PIDFB, out)+MN_OFFSETOF(FLOAT_S, status),       is_enabled_float_var},
    {IPC_AI_OUT,                   FLOAT_STRING_LEN,            ID_AIFB_1,    MN_OFFSETOF(T_FBIF_AIFB, out)+MN_OFFSETOF(FLOAT_S, value),                                   MN_OFFSETOF(T_FBIF_AIFB, out)+MN_OFFSETOF(FLOAT_S, status),        is_enabled_float_var},
    {IPC_AI2_OUT,                  FLOAT_STRING_LEN,            ID_AIFB_2,    MN_OFFSETOF(T_FBIF_AIFB, out)+MN_OFFSETOF(FLOAT_S, value),                                   MN_OFFSETOF(T_FBIF_AIFB, out)+MN_OFFSETOF(FLOAT_S, status),        is_enabled_float_var},
    {IPC_AI3_OUT,                  FLOAT_STRING_LEN,            ID_AIFB_3,    MN_OFFSETOF(T_FBIF_AIFB, out)+MN_OFFSETOF(FLOAT_S, value),                                   MN_OFFSETOF(T_FBIF_AIFB, out)+MN_OFFSETOF(FLOAT_S, status),        is_enabled_float_var},
    {IPC_AO_SP,                    FLOAT_STRING_LEN,            ID_AOFB_1,    MN_OFFSETOF(T_FBIF_AOFB, sp)+MN_OFFSETOF(FLOAT_S, value),                                    MN_OFFSETOF(T_FBIF_AOFB, sp)+MN_OFFSETOF(FLOAT_S, status),         is_enabled_float_var},
    {IPC_IS_IN1,                   FLOAT_STRING_LEN,            ID_ISFB_1,    MN_OFFSETOF(T_FBIF_ISFB, in_1)+MN_OFFSETOF(FLOAT_S, value),                                  MN_OFFSETOF(T_FBIF_ISFB, in_1)+MN_OFFSETOF(FLOAT_S, status),       is_enabled_float_var},
    {IPC_IS_IN2,                   FLOAT_STRING_LEN,            ID_ISFB_1,    MN_OFFSETOF(T_FBIF_ISFB, in_2)+MN_OFFSETOF(FLOAT_S, value),                                  MN_OFFSETOF(T_FBIF_ISFB, in_2)+MN_OFFSETOF(FLOAT_S, status),       is_enabled_float_var},
    {IPC_IS_IN3,                   FLOAT_STRING_LEN,            ID_ISFB_1,    MN_OFFSETOF(T_FBIF_ISFB, in_3)+MN_OFFSETOF(FLOAT_S, value),                                  MN_OFFSETOF(T_FBIF_ISFB, in_3)+MN_OFFSETOF(FLOAT_S, status),       is_enabled_float_var},
    {IPC_IS_IN4,                   FLOAT_STRING_LEN,            ID_ISFB_1,    MN_OFFSETOF(T_FBIF_ISFB, in_4)+MN_OFFSETOF(FLOAT_S, value),                                  MN_OFFSETOF(T_FBIF_ISFB, in_4)+MN_OFFSETOF(FLOAT_S, status),       is_enabled_float_var},
};

//a table to hold custom variables. the order of elements is sensitive
const IPC_Variable_IDs_t custom_var_mapping[] =
{
    IPC_VARIABLE_ID_NOTUSED,
    IPC_TB_FINAL_SP,
    IPC_TB_FINAL_POS,
    IPC_PID_SP,
    IPC_PID_PV,
    IPC_PID_OUT,
    IPC_PID2_SP,
    IPC_PID2_PV,
    IPC_PID2_OUT,
    IPC_AI_OUT,
    IPC_AI2_OUT,
    IPC_AI3_OUT,
    IPC_AO_SP,
    IPC_DO_SP_D,
    IPC_DO2_SP_D,
    IPC_IS_IN1,
    IPC_IS_IN2,
    IPC_IS_IN3,
    IPC_IS_IN4,
};

/** \brief format a float32 to str.(sprintf() cannot be used in this system because it will cause RESET)
    param in:
        s: formated string
        maxlen:  maximum length
        num_fraction: the length of fraction required
        n:  input float value
    return:
        the length of formated string
*/
static u8 ftoa(p8* s, u8 maxlen, u8 num_fraction, float32 n)
{
    if(n > FLT_MAX)
    {
        strcpy(s, "+inf");
        return 4u;
    }
    else if(n < -FLT_MAX)
    {
        strcpy(s, "-inf");
        return 4u;
    }
    else if(n > - FLT_MIN && n < FLT_MIN)
    {
        strcpy(s, "0");
        return 1u;
    }
    else
    {
        s32 digit;
        s32 m;
        s32 m1 = 0;

        s32 t_char;
        u8 char_0 = ASCII_0;
        s8 fraction_count = -1;
        s16 index = 0u;

        s16 s_maxlen = maxlen;
        float32 f32_temp;

        bool_t neg = false;

        if (n<0)
        {
            neg = true;
            n = -n;
            s[index++] = '-';
        }
        // calculate magnitude
        f32_temp = log10f(n);
        m = (s32)f32_temp;

        bool_t useExp = false;
        if((neg == true && (m < -(s_maxlen - 3) || m > (s_maxlen - 2)))||(neg == false && (m < -(s_maxlen - 2) || m > (s_maxlen - 1))))
        {
            useExp = true;
        }

        // set up for scientific notation
        if (useExp)
        {
            if (m < 0)
            {
                m -= 1;
            }
            f32_temp = (float32)m;
            n = n / powf(10.0f, f32_temp);
            m1 = m;
            m = 0;
        }
        if (m < 1)
        {
            m = 0;
        }
        // convert the number
        f32_temp = (float32)s_maxlen;
        f32_temp = - f32_temp;
        float32 precision = powf(10.0f, f32_temp);
        u8 out_loop = 0u;
        while ((n > precision || m >= 0) && out_loop == 0u)
        {
            f32_temp = (float32)m;
            float32 weight = powf(10.0f, f32_temp);

            if (weight > 0)
            {
                if(fraction_count >= 0)
                {
                    fraction_count++;
                    if(num_fraction < fraction_count)
                    {
                        out_loop = 1u; //break;
                    }
                }

                if(out_loop == 0u)
                {
                    digit = (s32)floorf(n / weight);
                    n -= (digit * weight);
                    digit = digit > 9 ? 9: digit;
                    t_char = char_0 + digit;
                    s[index++] = (p8)t_char;

                    if(index >= s_maxlen)
                    {
                        out_loop = 1u; //break;
                    }
                }
            }
            if (m == 0 && n > precision && out_loop == 0u)
            {
                s[index++] = '.';
                fraction_count = 0;

                if(index >= s_maxlen)
                {
                    index--;
                    out_loop = 1u; //break;
                }
            }
            m--;
        }
        if (useExp)
        {
            // convert the exponent
            p8 exponent[5];
            u8 exp_idx = 0;
            u8 exp_len;
            p8 exp_sign = '+';

            u8 fraction_len = (u8)index;

            if(m1 < 0)
            {
                exp_sign = '-';
                m1 = -m1;
            }

            //reversed order
            while(m1 > 0)
            {
                t_char = char_0 + m1 % 10;
                exponent[exp_idx++] = (p8)t_char;
                m1 /= 10;
            }

            exponent[exp_idx++] = exp_sign;
            exponent[exp_idx++] = 'e';

            exp_len = exp_idx;

            if(s_maxlen < (exp_len + fraction_len))
            {
                index = index - (exp_len - (s_maxlen - fraction_len));
            }

            //copy from exponent[] to s[] *reversed order*
            for(u8 i = 0; i < exp_len; i++)
            {
                s[index++] = exponent[--exp_idx];
            }

        }

        s[index] = '\0';

        return (u8)index;
    }
}

/** \brief format a float32 to str with the maxlen
*/
static void format_float_into_array(u8* str, u8 maxlen, float32 value)
{
    p8 buf[20];

    u8 len;

    len = ftoa(buf, maxlen, FLOAT_DISPLAY_PRECISION, value);

    //right alignment
    s16 j = maxlen - 1;
    s16 i = len - 1;
    while(i >= 0)
    {
        str[j--] = (u8)buf[i--];
    }
    while(j >= 0)
    {
        str[j--] = ASCII_SPACE;
    }
}


static u8 is_enabled_var(IPC_Variable_IDs_t var_id)
{

    u8 cus_conf_0 = ((T_FBIF_PTB*)fbs_get_block_inst(ID_PTB_1)->p_block_desc->p_block)->ui_custom_configuration.component_0;
    u8 cus_conf_1 = ((T_FBIF_PTB*)fbs_get_block_inst(ID_PTB_1)->p_block_desc->p_block)->ui_custom_configuration.component_1;

    if(custom_var_mapping[cus_conf_0] == var_id || custom_var_mapping[cus_conf_1] == var_id)
    {
        return CUSTOM_VAR_ENABLED;
    }
    else
    {
        return CUSTOM_VAR_DISABLED;
    }
}

static u8 is_enabled_float_var(IPC_Variable_IDs_t var_id)
{
    u8 ret = is_enabled_var(var_id);

    if(CUSTOM_VAR_ENABLED == ret)
    {
        ret = CUSTOM_VAR_FLOAT_ENABLED;
    }

    return ret;
}

static u8 device_param(IPC_Variable_IDs_t var_id)
{
    switch (var_id)
    {
        case IPC_DEVICE_TAG:
        {
            return CUSTOM_VAR_DEVICE_TAG_ENABLED;
        }
        case IPC_DEVICE_ID:
        {
            return CUSTOM_VAR_DEVICE_ID_ENABLED;
        }
        case IPC_DEVICE_ADDRESS:
        {
            return CUSTOM_VAR_DEVICE_ADDR_ENABLED;
        }
        default:
        {
            return CUSTOM_VAR_DISABLED;
        }
    }
}

/* \brief get a variable from the short varialbe table
    param out:
        pHart_short_var: hold a short variable
        index:  index of the short variable table
    return:
        E_OK if succeeds
        E_ERROR if fails
*/
static u8 get_a_short_var(HART_short_variable_t* pHart_short_var, u8 index)
{
    if(index >= sizeof(short_var_info_array)/sizeof(short_var_info_array[0]))
    {
        return E_ERROR;
    }

    u8* p_block;

    pHart_short_var->info.id = short_var_info_array[index].id;
    pHart_short_var->info.fp_is_enable_var = short_var_info_array[index].fp_is_enable_var;

    if(pHart_short_var->info.fp_is_enable_var != NULL)
    {
        u8 contom_var_status = pHart_short_var->info.fp_is_enable_var(pHart_short_var->info.id);
        switch (contom_var_status)
        {
            case CUSTOM_VAR_DISABLED:
            {
                return E_ERROR;
            }
            case CUSTOM_VAR_DEVICE_ADDR_ENABLED:
            {
                pHart_short_var->info.len = short_var_info_array[index].len;
                u8 dev_addr;
                get_sm_config_data(&dev_addr, pd_tag, dev_id);
                pHart_short_var->value = dev_addr;
                pHart_short_var->status = IPC_QUALITY_GOOD;
                return E_OK;
            }
        default:{}
        }
    }

    pHart_short_var->info.len = short_var_info_array[index].len;

    p_block = (u8*)(fbs_get_block_inst(short_var_info_array[index].block_idx)->p_block_desc->p_block);

    if(pHart_short_var->info.len == ONE_BYTE_LEN)
    {
        pHart_short_var->value = *((u8*)(p_block + short_var_info_array[index].value_offset));
    }
    else if (pHart_short_var->info.len == TWO_BYTES_LEN)
    {
        pHart_short_var->value = *((u16*)(void*)(p_block + short_var_info_array[index].value_offset));
    }
    else if (pHart_short_var->info.len == FOUR_BYTES_LEN)
    {
        pHart_short_var->value = *((u32*)(void*)(p_block + short_var_info_array[index].value_offset));
    }
    else
    {
        //exception
    }

    if(short_var_info_array[index].status_offset == OFFSET_DISABLE)
    {
        pHart_short_var->status = IPC_QUALITY_GOOD;
    }
    else
    {
        pHart_short_var->status = *((u8*)(p_block + short_var_info_array[index].status_offset));
    }

    return E_OK;
}

/* \brief get a variable from the long varialbe table
    param out:
        pHart_long_var: hold a long variable
        index:  index of the long variable table
    return:
        E_OK if succeeds
        E_ERROR if fails
*/
static u8 get_a_long_var(HART_long_variable_t* pHart_long_var, u8 index)
{
    if(index >= sizeof(long_var_info_array)/sizeof(long_var_info_array[0]))
    {
        return E_ERROR;
    }
    u8* p_block;

    pHart_long_var->info.id = long_var_info_array[index].id;
    pHart_long_var->info.fp_is_enable_var = long_var_info_array[index].fp_is_enable_var;
    pHart_long_var->info.len = long_var_info_array[index].len;


    if(pHart_long_var->info.fp_is_enable_var != NULL)
    {
        u8 contom_var_status = pHart_long_var->info.fp_is_enable_var(pHart_long_var->info.id);
        switch (contom_var_status)
        {
            case CUSTOM_VAR_DISABLED:
            {
                return E_ERROR;
            }
            case CUSTOM_VAR_DEVICE_ID_ENABLED:
            {
                pHart_long_var->info.len = long_var_info_array[index].len;
                u8 dev_addr;
                get_sm_config_data(&dev_addr, pd_tag, dev_id);
                pHart_long_var->p_value = dev_id;
                return E_OK;
            }
            case CUSTOM_VAR_DEVICE_TAG_ENABLED:
            {
                pHart_long_var->info.len = long_var_info_array[index].len;
                u8 dev_addr;
                get_sm_config_data(&dev_addr, pd_tag, dev_id);
                pHart_long_var->p_value = pd_tag;
                return E_OK;
            }
            case CUSTOM_VAR_FLOAT_ENABLED:
            {
                p_block = (u8*)fbs_get_block_inst(long_var_info_array[index].block_idx)->p_block_desc->p_block;
                float32 f = *((float32*)(void*)(p_block + long_var_info_array[index].value_offset));
                format_float_into_array(float_str, FLOAT_STRING_LEN - 1, f);
                float_str[FLOAT_STRING_LEN - 1] = *((u8*)(p_block + long_var_info_array[index].status_offset));
                pHart_long_var->p_value = float_str;
                return E_OK;
            }
        default:{}
        }
    }
    else
    {
        p_block = (u8*)fbs_get_block_inst(long_var_info_array[index].block_idx)->p_block_desc->p_block;
        pHart_long_var->p_value = (void*)(p_block + long_var_info_array[index].value_offset);
    }

    return E_OK;
}


/* \brief handler for hart command 186
    param out:
        toHart: cache for hart data
        send_length:  data length
    return:
        void
*/
void cmd_186_handler(u8 * toHart, u8 *send_length)
{
    HART_short_variable_t hart_short_var;
    Req_IPCWriteShortVariables_t* req_buf = (void*)toHart;

    static u8 short_var_index = 0;

    u8 pre_short_var_index = short_var_index;

    while(E_ERROR == get_a_short_var(&hart_short_var, short_var_index))
    {
        short_var_index = (short_var_index+1)%(sizeof(short_var_info_array)/sizeof(short_var_info_array[0]));
        if(short_var_index == pre_short_var_index)
        {
            return;
        }
    }

    util_PutU16(req_buf->IPCVariableID1[0], (u16)hart_short_var.info.id);
    util_PutU8(req_buf->IPCVar1Status[0], hart_short_var.status);
    util_PutU32(req_buf->IPCVar1Buffer[0], hart_short_var.value);

    short_var_index = (short_var_index+1)%(sizeof(short_var_info_array)/sizeof(short_var_info_array[0]));

    while(E_ERROR == get_a_short_var(&hart_short_var, short_var_index))
    {
        short_var_index = (short_var_index+1)%(sizeof(short_var_info_array)/sizeof(short_var_info_array[0]));
    }

    util_PutU16(req_buf->IPCVariableID2[0], (u16)hart_short_var.info.id);
    util_PutU8(req_buf->IPCVar2Status[0], hart_short_var.status);
    util_PutU32(req_buf->IPCVar2Buffer[0], hart_short_var.value);

    short_var_index = (short_var_index+1)%(sizeof(short_var_info_array)/sizeof(short_var_info_array[0]));

    *send_length = HC186_REQ_LENGTH;
}

/* \brief handler for hart command 187
    param out:
        toHart: cache for hart data
        send_length:  data length
    return:
        void
*/
void cmd_187_handler(u8 * toHart, u8 *send_length)
{
    static u8 long_var_index = 0;
    u8 pre_long_var_index = long_var_index;

    static u8 block_num = 0;

    static HART_long_variable_t hart_long_var;
    Req_IPCWriteArray_t* req_buf = (void*)toHart;

    u8 ret_get_a_var = E_ERROR;

    //get new variable
    if (block_num == 0)
    {
        do
        {
            ret_get_a_var = get_a_long_var(&hart_long_var, long_var_index);

            long_var_index = (long_var_index+1)%(sizeof(long_var_info_array)/sizeof(long_var_info_array[0]));

            if(long_var_index == pre_long_var_index)
            {
                return;
            }
        }
        while(E_ERROR == ret_get_a_var);
    }

    util_PutU16(req_buf->IPCVariableID1[0], (u16)hart_long_var.info.id);
    util_PutU8(req_buf->IPCDataBlockNum[0], block_num);
    util_PutU8Array(req_buf->IPCDataBlock[0], LONG_VAR_BUF_LEN, (u8*)hart_long_var.p_value+block_num*LONG_VAR_BUF_LEN);

    block_num = (block_num+1)%(0 != (hart_long_var.info.len%LONG_VAR_BUF_LEN) ? hart_long_var.info.len/LONG_VAR_BUF_LEN+1 : hart_long_var.info.len/LONG_VAR_BUF_LEN);

    *send_length = HC187_REQ_LENGTH;
}

fferr_t WriteUiCustomConfiguration(const T_FBIF_WRITE_DATA* p_write)
{
    fferr_t result = E_OK;

    if(0 == p_write->subindex)
    {
        MN_DBG_ASSERT(sizeof(_UI_CUST_CONF) == p_write->length);
        _UI_CUST_CONF* p_ui_cust_conf = (_UI_CUST_CONF*)(void*)p_write->source;

        if(p_ui_cust_conf->component_0 < 1
           || p_ui_cust_conf->component_0 > (sizeof(custom_var_mapping)/sizeof(IPC_Variable_IDs_t)-1)
           || p_ui_cust_conf->component_1 < 1
           || p_ui_cust_conf->component_1 > (sizeof(custom_var_mapping)/sizeof(IPC_Variable_IDs_t)-1)
           )
        {
            result = E_FB_PARA_CHECK;
        }
    }
    else
    {
        u8* p_ui_cust_conf_one_value = (u8*)(void*)p_write->source;

        if(1 > *p_ui_cust_conf_one_value || (sizeof(custom_var_mapping)/sizeof(IPC_Variable_IDs_t)-1) < *p_ui_cust_conf_one_value)
        {
            result = E_FB_PARA_CHECK;
        }
    }

    return result;
}

/* \brief handler for hart command 170,166, this function will be called by
   IPC, use to recieve from hart, and update the unit and unit related
   parameters if the unit is changed.
   \param in:
        form_hart: deta recieve from hart
   \param out:
        p_block_instance: save the recieve data to pointer
   \return:
        void
*/
static void cmd_170_176_read_handler
(
    USIGN8 * from_hart,
    T_FBIF_BLOCK_INSTANCE * p_block_instance
)
{
    fferr_t fferr;
    u8 ap_units;
    u8 flag = FALSE;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    Rsp_ReadPressureUnits_t* rsp_176 = (void*)((u8*)from_hart + HC170_LENGTH);
    ap_units = util_GetU8(rsp_176->PressureUnits[0]);

    switch(ap_units)
    {
        case H_PSI:
        {
            //if the units changed, update the  pressure unit
            //related parameters.
            if (p_PTB->pressure_range.units != FF_UNITS_PSI)
            {
                fftbpr_PressureUnitUpdate(p_block_instance,
                    p_PTB->pressure_range.units, FF_UNITS_PSI);
                flag = TRUE;
            }
            break;
        }
        case H_BAR:
        {
            //if the units changed, update the  pressure unit
            //related parameters.
            if (p_PTB->pressure_range.units != FF_UNITS_BAR)
            {
                fftbpr_PressureUnitUpdate(p_block_instance,
                    p_PTB->pressure_range.units, FF_UNITS_BAR);
                flag = TRUE;
            }
            break;
        }
        case H_KPA:
        {
            //if the units changed, update the  pressure unit
            //related parameters.
            if (p_PTB->pressure_range.units != FF_UNITS_KPA)
            {
                fftbpr_PressureUnitUpdate(p_block_instance,
                    p_PTB->pressure_range.units, FF_UNITS_KPA);
                flag = TRUE;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    //if the unit is changed
    if (flag != FALSE)
    {
        T_FBS_WRITE_PARAM_LOCAL   p_write_loc;
        p_write_loc.rel_idx  = REL_IDX_PTB_PRESSURE_RANGE;
        p_write_loc.subindex = 3;
        p_write_loc.length   = sizeof(u16);
        p_write_loc.source   = (u8 *)&p_PTB->pressure_range.units;

        if (appl_check_hm_state() == HM_RUN )
        {
            p_write_loc.startup_sync = FALSE;
        }
        else
        {
            p_write_loc.startup_sync = TRUE;
        }

        fferr = fbs_write_param_loc( p_block_instance, &p_write_loc );
        if (E_OK == fferr) {
            return;
            //lint
        }
    }
    return;
}

/*  \brief handler for hart command 170,176, this function will be called by
    IPC, use to send command to hart
    \param in:
        to_hart: deta send to hart
    \param out:
        send_length: data length
    \return:
        void
*/
static void cmd_170_176_write_handler(USIGN8 * toHart, u8 *send_length)
{
    Req_ReadSettings_t* req = (void*)toHart;
    util_PutU8(req->SubCmdNum[0], 176);
    *send_length = HC170_REQ_LENGTH;

    return;
}

/* \brief handler for hart command 170,216 and 167 called in IPC
   \param in:
        form_hart  : deta recieve from hart
        to_hart    : data send to hart
        send_length: data length
        access     : to flag the data send or receive
   \param out:
        p_block_instance: save the recieve data to pointer
   \return:
        void
*/
void cmd_170_handler
(
    T_FBIF_BLOCK_INSTANCE * p_block_instance,
    USIGN8 *from_hart,
    USIGN8 *toHart,
    USIGN8 *send_length,
    USIGN8 access
)
{
    if ( access == RECEIVE )
    {
        cmd_170_176_read_handler(from_hart, p_block_instance);
    }
    else if ( access == SEND )
    {
        cmd_170_176_write_handler(toHart, send_length);
    }
    else
    {
    }
    return;
}



/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mndoswff.c
    \brief Handling of control limits interface

    CPU: Any

    OWNER: AK
*/


//Softing headers first
#include <softing_base.h>
#include <ptb.h>
#include <dofb.h>

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"

//Glue headers last
#include "mndoswff.h"
#include "appl_alert.h"

/** \brief EXCLUSIVELY for DO FB passthrough
*/
static u8 switcharray[DO_NUM_CHANNELS] = {DSS_Inactive, DSS_Inactive};

//#define WATCH_BAD
#ifdef WATCH_BAD //debug hook
__root volatile u32 mycount;
#endif

void ffdosw_SetValue(u8 switchnum, const DISCRETE_S *val)
{
    u8 s = DSS_Inactive;
#ifdef WATCH_BAD //debug hook
    if((val->status & QUALITY_MASK) < SQ_GOOD)
    {
        mycount++;
    }
#endif
    if( ((val->status & QUALITY_MASK) >= SQ_GOOD) && (val->value != 0))
    {
        s = DSS_Active;
    }
    switcharray[switchnum] = s;
}

//returns DO switch requested value
typedef u8 rtfunc_t(const T_FBIF_BLOCK_INSTANCE *p_block_instance);

typedef struct dosw_map_t
{
    u8 APPConfCode;
    rtfunc_t *rtfunc; //!< runtime processing function
} dosw_map_t;

//------------- helpers to populate logical value requested of a DO switch ----
static u8 alwayson(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    UNUSED_OK(p_block_instance);
    return DSS_Active;
}
static u8 alwaysoff(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    UNUSED_OK(p_block_instance);
    return DSS_Inactive;
}
static u8 notnormal(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    if(p_PTB->mode_blk.actual == MODE_AUTO)
    {
        return DSS_Inactive;
    }
    else
    {
        return DSS_Active;
    }
}
static u8 blockerr(const T_FBIF_BLOCK_INSTANCE *p_block_instance, u16 mask)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    if((p_PTB->block_err & mask) == 0)
    {
        return DSS_Inactive;
    }
    else
    {
        return DSS_Active;
    }
}

static u8 maintsoon(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    return blockerr(p_block_instance, BLK_ERR_DEV_NEEDS_MAINT_SOON);
}

static u8 maintnow(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    return blockerr(p_block_instance, BLK_ERR_DEV_NEEDS_MAINT_NOW);
}

static u8 doblock(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    //Done inline
    UNUSED_OK(p_block_instance);
    return DSS_Inactive;
}

// -------------------------------------------------------------------
// Functions to get the states of specific alerts

static u8 GetAlert_AirSuppy(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    u8      retval;

    retval = (Alert_AirSupply(p_PTB)) ? DSS_Active : DSS_Inactive;
    return retval;
}

static u8 GetAlert_TravelDeviation(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    u8      retval;

    retval = (Alert_TravelDeviation(p_PTB)) ? DSS_Active : DSS_Inactive;
    return retval;
}

static u8 GetAlert_PositionHI(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    u8      retval;

    retval = (Alert_PositionHI(p_PTB)) ? DSS_Active : DSS_Inactive;
    return retval;
}

static u8 GetAlert_PositionLO(const T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    u8      retval;

    retval = (Alert_PositionLO(p_PTB)) ? DSS_Active : DSS_Inactive;
    return retval;
}




/* Configurations delegated to APP are OK to have NULL runtime functions
because they are going to be ignored.
The exception is what happens when switch configuration changes from delegated
to FFP-based until the new value arrives. It is supposed to be a short period of
time. Anyone cares?
*/
static const  dosw_map_t dosw_map[] =
{
    [FSWITCH_DISABLE] =
    {
        .APPConfCode = HSWITCH_DISABLE,
        .rtfunc = NULL,
    },
    [FSWITCH_DO_BLOCK] =
    {
        .APPConfCode = HSWITCH_PASSTHROUGH,
        .rtfunc = doblock,
    },
    [FSWITCH_FAULT_STATE] =
    {
        .APPConfCode = HSWITCH_FAIL,
        .rtfunc = NULL,
    },
    [FSWITCH_NOT_IN_NORMAL] =
    {
        .APPConfCode = HSWITCH_PASSTHROUGH,
        .rtfunc = notnormal,
    },
    [FSWITCH_MAINTENANCE_REQ] =
    {
        .APPConfCode = HSWITCH_PASSTHROUGH,
        .rtfunc = maintnow,
    },
    [FSWITCH_WARNING_DATA] =
    {
        .APPConfCode = HSWITCH_PASSTHROUGH,
        .rtfunc = maintsoon,
    },
    [FSWITCH_AIR_SUPPLY_ALERT] =
    {
        .APPConfCode = HSWITCH_PASSTHROUGH,
        .rtfunc = GetAlert_AirSuppy,
    },
    [FSWITCH_TRAVEL_DEVIATION_ALERT] =
    {
        .APPConfCode = HSWITCH_PASSTHROUGH,
        .rtfunc = GetAlert_TravelDeviation,
    },
    [FSWITCH_POSITION_HI_ALERT] =
    {
        .APPConfCode = HSWITCH_PASSTHROUGH,
        .rtfunc = GetAlert_PositionHI,
    },
    [FSWITCH_POSITION_LO_ALERT] =
    {
        .APPConfCode = HSWITCH_PASSTHROUGH,
        .rtfunc = GetAlert_PositionLO,
    },
    [FSWITCH_ALWAYSON] =
    {
        .APPConfCode = HSWITCH_ALWAYSON,
        .rtfunc = alwayson,
    },
    [FSWITCH_ALWAYSOFF] =
    {
        .APPConfCode = HSWITCH_ALWAYSOFF,
        .rtfunc = alwaysoff,
    },
    [FSWITCH_RESET] =
    {
        .APPConfCode = HSWITCH_RESET,
        .rtfunc = NULL,
    },
    [FSWITCH_CUTOFF] =
    {
        .APPConfCode = HSWITCH_TIGHT,
        .rtfunc = NULL,
    },
};

//---------------------- runtime part ----------------------------

/** \brief populates discrete switches to be sent to APP; agnostic to the caller
\param[out] swvalue - switch values to populate
*/
void ffdosw_Runtime(const T_FBIF_BLOCK_INSTANCE *p_block_instance, u8 swvalue[DO_NUM_CHANNELS])
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    u8 sw_function[DO_NUM_CHANNELS] =
    {
        [0] = p_PTB->discrete_switch_1_conf.component_1,
        [1] = p_PTB->discrete_switch_2_conf.component_1,
    };

    rtfunc_t *f;
    for(u8 swid=0; swid<NELEM(sw_function); swid++)
    {
        if(sw_function[swid] >= NELEM(dosw_map))
        {
            f = NULL;
        }
        else
        {
            f = dosw_map[sw_function[swid]].rtfunc;
        }
        if(f == NULL)
        {
            swvalue[swid] = DSS_Inactive; //inactive, maybe DSS_LNA; TBD
        }
        else
        {
            if(f == doblock)
            {
                swvalue[swid] = switcharray[swid];
            }
            else
            {
                swvalue[swid] = f(p_block_instance);
            }
        }
    }
}

//---------------------- configuration part ----------------------------
static void dosw_FillDoSwData(_STRUCT2_U8 *sw, const DOSWConf_t *s)
{
    u8 dofunc;
    u8 ffdofunc;
    if(sw != NULL)
    {
        sw->component_0 = util_GetU8(s->DONormalState[0]);
        dofunc = util_GetU8(s->DOFunction[0]);
        if(dofunc != HSWITCH_PASSTHROUGH)
        {
            for(ffdofunc=0; ffdofunc<NELEM(dosw_map); ffdofunc++)
            {
                if(dosw_map[ffdofunc].APPConfCode == dofunc)
                {
                    break;
                }
            }
            sw->component_1 = ffdofunc;
        }
        else
        {
            //FF side keeps the function value; nothing to do other than error handling
            if(dosw_map[sw->component_1].APPConfCode != HSWITCH_PASSTHROUGH)
            {
                //for now, silently repair to the most benign pass-through thing
                sw->component_1 = FSWITCH_AIR_SUPPLY_ALERT;
                //TODO: set configuration error
            }
        }
    }
}

/** \brief A helper to pre-populate DO switches config from the APP. \n
Also Fills Tx buffer and control limits structure(s) from APP
*/
static fferr_t dosw_Populate(_STRUCT2_U8 *sw1, _STRUCT2_U8 *sw2, u8 *from, u8 *to)
{
    //Ask APP what it has
    fferr_t fferr;

    //Populate the send buffer
    Rsp_ReadOutputSwitchesConfiguration_t *p = (void *)((u8 *)to + 2);
    DOSWConf_t *s = (void *)p->DOSWConf[0];

    u8 len = MN_OFFSETOF(Req_ReadOutputSwitchesConfiguration_t, endmark);
    fferr = mn_HART_acyc_cmd(137, from, len, to);
    if(fferr == E_OK)
    {
        //Copy response to Tx buffer for write-back (in case we will)
        memcpy(from, s, sizeof(p->DOSWConf));
        //Populate TB types
        dosw_FillDoSwData(sw1, &s[0]);
        dosw_FillDoSwData(sw2, &s[1]);
    }

    return fferr;
}

fferr_t ffdosw_ReadConfiguration(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to)
{
    UNUSED_OK(p_read);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    return dosw_Populate(&p_PTB->discrete_switch_1_conf, &p_PTB->discrete_switch_2_conf, from, to);
} //lint !e818 const-ness of pointers here may be misleading


/** \brief Writes DO switches configuration
NOTE: apprently, p_write->source is filled in correct order but without alignment padding
*/
fferr_t ffdosw_WriteConfiguration(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to)
{
    //T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    fferr_t fferr = dosw_Populate(NULL, NULL, from, to);

    if(fferr == E_OK)
    {
        Req_WriteOutputSwitchesConfiguration_t *s1 = (void *)((u8 *)from);
        DOSWConf_t *s = (void *)s1->DOSWConf[0];
        u8 index = (p_write->rel_idx == REL_IDX_PTB_DISCRETE_SWITCH_1_CONF)?0:1;

        //Populate what we are not changing with the data from TB
        switch(p_write->subindex)
        {
            case 0: //everything)
            {
                util_PutU8(s[index].DONormalState[0], *p_write->source);
                util_PutU8(s[index].DOFunction[0], dosw_map[p_write->source[1]].APPConfCode);
                break;
            }
            case 1: //Normal State
                util_PutU8(s[index].DONormalState[0], *p_write->source);
                break;
            case 2: //function
                util_PutU8(s[index].DOFunction[0], dosw_map[*p_write->source].APPConfCode);
                break;
            default:
                fferr = E_FB_PARA_CHECK;
                break;
        }
    }
    if(fferr == E_OK)
    {
        u8 len = MN_OFFSETOF(Req_WriteOutputSwitchesConfiguration_t, endmark);
        fferr = mn_HART_acyc_cmd(134, from, len, to);
    }
    if(fferr == E_OK)
    {
        fferr = fbs_write_param(p_block_instance, p_write);
    }

    return fferr;
}

//-------------- TB hooks ----------------------
#define IGNORE_COUNT_DOSW 1

void tb_DOsw1(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    //T_FBS_WRITE_PARAM_LOCAL   p_write_loc;
    static u8 ignore_count = IGNORE_COUNT_DOSW;

    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    DISCRETE_S local_dosw = *(DISCRETE_S*)(void*)p_process_val;
    CONST_ASSERT(sizeof(local_dosw)<=sizeof(*p_process_val));

    if(ignore_count > 0)
    {
        --ignore_count;
        local_dosw.status = SQ_UNCERTAIN | SUB_INITIAL_VALUE;
    }
    //only in Auto mode the DO request should propagate to PTB
    if ( p_PTB->mode_blk.actual == MODE_AUTO )
    {
        if(p_PTB->discrete_switch_1_conf.component_1 == FSWITCH_DO_BLOCK)
        {
            ffdosw_SetValue(0, &local_dosw);
        }
    }
    *(DISCRETE_S *)(void *)p_process_val = p_PTB->discrete_output_1_state;

} //lint !e818 all helper functions must have the same prototype

void tb_DOsw2(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    //T_FBS_WRITE_PARAM_LOCAL   p_write_loc;
    static u8 ignore_count = IGNORE_COUNT_DOSW;

    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    DISCRETE_S local_dosw = *(DISCRETE_S*)(void*)p_process_val;
    CONST_ASSERT(sizeof(local_dosw)<=sizeof(*p_process_val));

    if(ignore_count > 0)
    {
        --ignore_count;
        local_dosw.status = SQ_UNCERTAIN | SUB_INITIAL_VALUE;
    }
    //only in Auto mode the DO request should propagate to PTB
    if ( p_PTB->mode_blk.actual == MODE_AUTO )
    {
        if(p_PTB->discrete_switch_2_conf.component_1 == FSWITCH_DO_BLOCK)
        {
            ffdosw_SetValue(1, &local_dosw);
        }
    }
    *(DISCRETE_S *)(void *)p_process_val = p_PTB->discrete_output_2_state;
} //lint !e818 all helper functions must have the same prototype

static void ffdosw_BackgroundPlugin(u8 block_id)
{
    //Set back-calculation values

    //Set buffered dosw inactive if DO FB is configured for dosw and is OOS
    const T_FBIF_BLOCK_INSTANCE *p_DOFB_instance = fbs_get_block_inst(block_id);
    T_FBIF_DOFB *p_DOFB = p_DOFB_instance->p_block_desc->p_block;
    const T_FBIF_BLOCK_INSTANCE *pB = fbs_get_block_inst(ID_PTB_1);
    const T_FBIF_PTB *p_PTB = pB->p_block_desc->p_block;
    const DISCRETE_S *p = NULL;
    if(block_id == ID_DOFB_1)
    {
        p = &p_PTB->discrete_output_1_state;
    }
    if(block_id == ID_DOFB_2)
    {
        p = &p_PTB->discrete_output_2_state;
    }
    if(p == NULL)
    {
        return;
    }

    osif_disable_all_tasks();

#if 0
    u8 status;
    if(comm_state == HART_COMM_OK)
    {
        status = p->status;
    }
    else
    {
        status = SQ_BAD | SUB_DEVICE_FAILURE;
    }

    p_DOFB->bkcal_out_d.status = status;
    p_DOFB->bkcal_out_d.value = p->value;
#endif

    if((p_DOFB->mode_blk.actual & MODE_OS) != 0)
    {
#if 1
        if(p_DOFB->channel == CH_DISCRETE_SWITCH_1)
        {
            ffdosw_SetValue(0, &(DISCRETE_S){SQ_BAD, 0});
        }
        if(p_DOFB->channel == CH_DISCRETE_SWITCH_2)
        {
            ffdosw_SetValue(1, &(DISCRETE_S){SQ_BAD, 0});
        }
#else
        switcharray[0] = DSS_Inactive;
        switcharray[1] = DSS_Inactive;
#endif
    }
    osif_enable_all_tasks();

}

/** \brief Monitors BAD state of DO when Execute_PTB is not called
*/
void ffdosw_TbBackgroundPatrol(void)
{
    ffdosw_BackgroundPlugin(ID_DOFB_1);
    ffdosw_BackgroundPlugin(ID_DOFB_2);
}



/* This line marks the end of the source */

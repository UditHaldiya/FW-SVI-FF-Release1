/**
    \file mntbexec.c
    \brief Some application-level TB executiuon stuff coded by MN

*/
#include <softing_base.h>
#include <ptb.h>
// #include <dofb.h>
// #include <ffbl_ao.h>
// #include <ffbl_do.h>
#include "mndoswff.h"
#include "mn_fstate.h"
// #include "appl_int.h"
#include "appl_alert.h"
#define MN_READBACK_SELECT_FINALPOS    0
#define MN_READBACK_SELECT_WORKINGPOS  1

typedef void TbChannelFunc_t(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val);

/* TFS:16709,16710 traced to a Softing bug in AO FB where AO's first GOOD OUT is grabbed
from last written value in MAN. (If target mode is AUTO, AO FB silently starts
in MAN which is a bug in its own right)
A kludgy workaround is to ignore a few first runs of FB application
We may want to revert this when/if Softing provides a fix
*/
#define IGNORE_COUNT 1

static void tb_PositionSp(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    static u8 ignore_count = IGNORE_COUNT;
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    FLOAT_S                   local_final_value = *p_process_val;
    T_FBS_WRITE_PARAM_LOCAL   p_write_loc;

#if 0
    // if PTB is in OOS: FINAL_VALUE status must be OOS regardless of upstream status
    //                   FINAL_VALUE value shall not change
    if ( p_PTB->mode_blk.actual == MODE_OS )
    {
        local_final_value.status = SQ_BAD | SUB_OUT_OF_SERVICE;
    }
#endif

    if(ignore_count > 0)
    {
        --ignore_count;
        local_final_value.status = SQ_UNCERTAIN | SUB_INITIAL_VALUE;
    }

    //write local_final_value to FINAL_VALUE in PTB block
    //if ( (p_PTB->mode_blk.actual == MODE_AUTO) || (p_PTB->mode_blk.actual == MODE_OS) )
    if ( (p_PTB->mode_blk.actual != MODE_MAN) )
    {
        p_write_loc.subindex = 0;
        p_write_loc.length   = sizeof(FLOAT_S);
        p_write_loc.rel_idx = REL_IDX_PTB_FINAL_VALUE;
        p_write_loc.source   = (USIGN8*) (&local_final_value);
        (void)fbs_write_param_loc( p_block_instance, &p_write_loc );
    }

    /* write AOFB readback parameter */
    switch (p_PTB->readback_select)
    {
        case MN_READBACK_SELECT_FINALPOS :        // Spec: 0 - Final Position
            *p_process_val = p_PTB->final_position_value;
            break;
        case MN_READBACK_SELECT_WORKINGPOS :      // Spec: 1 - Working Position
            *p_process_val = p_PTB->working_pos;
            break;
        default :                                 // Error!
            break;
    }
} //lint !e818 all helper functions must have the same prototype


static void tb_IntSp(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    static u8 ignore_count = IGNORE_COUNT;
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    DISCRETE_S                local_final_value_dint = *(DISCRETE_S *)(void *)p_process_val;
    T_FBS_WRITE_PARAM_LOCAL   p_write_loc;

#if 0
    // if PTB is in OOS: FINAL_VALUE_DINT status must be OOS regardless of upstream status
    //                   FINAL_VALUE_DINT value shall not change
    if ( p_PTB->mode_blk.actual == MODE_OS )
    {
        local_final_value_dint.status = SQ_BAD | SUB_OUT_OF_SERVICE;
    }
#endif

    if(ignore_count > 0)
    {
        --ignore_count;
        local_final_value_dint.status = SQ_UNCERTAIN | SUB_INITIAL_VALUE;
    }

#if 0
    // only if PTB is in AUTO mode FINAL_VALUE_DINT should reflect SP_D from upstream
    if ( p_PTB->mode_blk.actual == MODE_AUTO )
    {
        local_final_value_dint.status = ((DISCRETE_S *)(void *)p_process_val)->status;
    }

    //write local_final_value_dint to FINAL_VALUE_DINT in PTB block
    if ( (p_PTB->mode_blk.actual == MODE_AUTO) || (p_PTB->mode_blk.actual == MODE_OS) )
#endif
    if ( (p_PTB->mode_blk.actual != MODE_MAN) )
    {
        p_write_loc.subindex = 0;
        p_write_loc.length   = sizeof(DISCRETE_S);
        p_write_loc.rel_idx = REL_IDX_PTB_FINAL_VALUE_DINT;
        p_write_loc.source   = (USIGN8*) (&local_final_value_dint);
        (void)fbs_write_param_loc( p_block_instance, &p_write_loc );
    }

    /* write DOFBInt readback parameter */
    *(DISCRETE_S *)(void *)p_process_val = p_PTB->final_position_value_dint;
} //lint !e818 all helper functions must have the same prototype


static void tb_BoolSp(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    static u8 ignore_count = IGNORE_COUNT;
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    T_FBS_WRITE_PARAM_LOCAL   p_write_loc;
    DISCRETE_S                local_final_value_d = *(DISCRETE_S *)(void *)p_process_val;

#if 0
    // if PTB is in OOS: FINAL_VALUE_D status must be OOS regardless of upstream status
    //                   FINAL_VALUE_D value shall not change
    if ( p_PTB->mode_blk.actual == MODE_OS )
    {
        local_final_value_d.status = SQ_BAD | SUB_OUT_OF_SERVICE;
    }
#endif

    if(ignore_count > 0)
    {
        --ignore_count;
        local_final_value_d.status = SQ_UNCERTAIN | SUB_INITIAL_VALUE;
    }

#if 0
    // only if PTB is in AUTO mode FINAL_VALUE_DINT should reflect SP_D from upstream
    if ( p_PTB->mode_blk.actual == MODE_AUTO )
    {
        local_final_value_d.status = ((DISCRETE_S *)(void *)p_process_val)->status;
    }

    //write local_final_value_d to FINAL_VALUE_D in PTB block
    if ( (p_PTB->mode_blk.actual == MODE_AUTO) || (p_PTB->mode_blk.actual == MODE_OS) )
#endif
    if ( (p_PTB->mode_blk.actual != MODE_MAN) )
    {
        //cast value from <float> to <u8> so that we can compare integer to integer
        local_final_value_d.value = ((DISCRETE_S *)(void *)p_process_val)->value;
        if ( local_final_value_d.value != 0 )
        {
            local_final_value_d.value = 1;
        }
        p_write_loc.subindex = 0;
        p_write_loc.length   = sizeof(DISCRETE_S);
        p_write_loc.rel_idx = REL_IDX_PTB_FINAL_VALUE_D;
        p_write_loc.source   = (USIGN8*) (&local_final_value_d);
        (void)fbs_write_param_loc( p_block_instance, &p_write_loc );
    }

    /* write DOFBBool readback parameter */
    *(DISCRETE_S *)(void *)p_process_val = p_PTB->final_position_value_d;
} //lint !e818 all helper functions must have the same prototype


//*****  Helper functions for ANALOG INPUT block *****
static void tb_SupplyPressure(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write supply_pressure value and status to AIFB channel */
    *p_process_val = p_PTB->supply_pressure;
} //lint !e818 all helper functions must have the same prototype

static void tb_Act1Pressure(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write actuator_a_pressure value and status to AIFB channel */
    *p_process_val = p_PTB->actuator_a_pressure;
} //lint !e818 all helper functions must have the same prototype

static void tb_Act2Pressure(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write actuator_b_pressure value and status to AIFB channel */
    *p_process_val = p_PTB->actuator_b_pressure;
} //lint !e818 all helper functions must have the same prototype

static void tb_PilotPressure(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write pilot_pressure value and status to AIFB channel */
    *p_process_val = p_PTB->pilot_pressure;
} //lint !e818 all helper functions must have the same prototype

static void tb_IPcurrent(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write ip_current value and status to AIFB channel */
    *p_process_val = p_PTB->ip_drive_current;
} //lint !e818 all helper functions must have the same prototype

static void tb_Temperature(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write temperature value and status to AIFB channel */
    *p_process_val = p_PTB->temperature;
} //lint !e818 all helper functions must have the same prototype

static void tb_FinalValue(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write final_value value and status to AIFB channel */
    *p_process_val = p_PTB->final_value;
} //lint !e818 all helper functions must have the same prototype

static void tb_FinalPosValue(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write final_position_value value and status to AIFB channel */
    *p_process_val = p_PTB->final_position_value;
} //lint !e818 all helper functions must have the same prototype

static void tb_WorkingSP(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write working_sp value and status to AIFB channel */
    *p_process_val = p_PTB->working_sp;
} //lint !e818 all helper functions must have the same prototype

static void tb_WorkingPos(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    /* write working_pos value and status to AIFB channel */
    *p_process_val = p_PTB->working_pos;
} //lint !e818 all helper functions must have the same prototype


//*****  Helper functions for DISCRETE INPUT block *****
static void tb_Switch1(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    DISCRETE_S *p = (DISCRETE_S *)(void *)p_process_val;
    /* write discrete_output_1 value and status to DIFB channel */
    p->status = p_PTB->discrete_output_1_state.status;
    p->value = p_PTB->discrete_output_1_state.value;
} //lint !e818 all helper functions must have the same prototype

static void tb_Switch2(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    DISCRETE_S *p = (DISCRETE_S *)(void *)p_process_val;
    /* write discrete_output_2 value and status to DIFB channel */
    p->status = p_PTB->discrete_output_2_state.status;
    p->value = p_PTB->discrete_output_2_state.value;
} //lint !e818 all helper functions must have the same prototype

static void tb_DiscreteInput(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    DISCRETE_S *p = (DISCRETE_S *)(void *)p_process_val;
    /* write discrete_input value and status to DIFB channel */
    p->status = p_PTB->discrete_input.status;
    p->value = p_PTB->discrete_input.value;
} //lint !e818 all helper functions must have the same prototype

static void tb_MAIchannels(T_FBIF_BLOCK_INSTANCE *p_block_instance, FLOAT_S *p_process_val)
{
    T_FBIF_PTB *p_PTB         = p_block_instance->p_block_desc->p_block;
    FLOAT_S *p = p_process_val;
    // write 8 channels AI values and statuses to MAI block
    p[0] = p_PTB->supply_pressure;
    p[1] = p_PTB->actuator_a_pressure;
    p[2] = p_PTB->actuator_b_pressure;
    p[3] = p_PTB->pilot_pressure;
    p[4] = p_PTB->ip_drive_current;
    p[5] = p_PTB->temperature;
    p[6] = p_PTB->working_sp;
    p[7] = p_PTB->working_pos;
} //lint !e818 all helper functions must have the same prototype


static const TbChannelFunc_t * const TbChannelFunc[] =
{
    [CH_NOT_CONFIGURED] = NULL,
    [CH_POSITION] = tb_PositionSp,
    [CH_POSITION_OPEN_CLOSE] = tb_BoolSp,
    [CH_POSITION_DISCRETE_POSITION] = tb_IntSp,
    [CH_DISCRETE_SWITCH_1] = tb_DOsw1,
    [CH_DISCRETE_SWITCH_2] = tb_DOsw2,
    [CH_FAULT] = NULL,
    [CH_SUPPLY_PRESSURE] = tb_SupplyPressure,
    [CH_ACTUATOR_PRESSURE_1] = tb_Act1Pressure,
    [CH_ACTUATOR_PRESSURE_2] = tb_Act2Pressure,
    [CH_IP_CURRENT] = tb_IPcurrent,
    [CH_PILOT_PRESSURE] = tb_PilotPressure,
    [CH_TEMPERATURE] = tb_Temperature,
    [CH_FINAL_VALUE] = tb_FinalValue,
    [CH_FINAL_POSITION_VALUE] = tb_FinalPosValue,
    [CH_WORKING_SP] = tb_WorkingSP,
    [CH_WORKING_POSITION] = tb_WorkingPos,
    [CH_DISCRETE_INPUT] = tb_DiscreteInput,
    [CH_STATE_OF_SW1] = tb_Switch1,
    [CH_STATE_OF_SW2] = tb_Switch2,
    [CH_MAI_CHANNELS] = tb_MAIchannels,
};

/**
\brief A TB callback called separately with each channel number
It must dispatch prosessing ASAP.
Individual channel functions should be fast, too, and do what their channel dictates

  This function is called by the execution function of a AI function block.
  It has to provide a analog input value.

\param  p_block_instance  Pointer to the transducer block to be executed
\param  p_exec_tb         Pointer to a process data description block.
The relevant components of the process data description block are:

    channel             Channel number

    p_process_value     Pointer to the process value (type FLOAT_S *)

    p_xd_scale          Pointer to the XD_SCALE parameter of the AI block
*/
void Execute_PTB(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_PROCESS_DATA *p_exec_tb)
{
    FLOAT_S *p_process_val = p_exec_tb->p_process_value;

    u16 channel = p_exec_tb->channel;
    if((channel >= NELEM(TbChannelFunc)) || (TbChannelFunc[channel] == NULL))
    {
        //Softing's default action
        p_process_val->value  = 0.0F;
        p_process_val->status = SQ_BAD | SUB_CONFIGURATION_ERROR;
    }
    else
    {
        TbChannelFunc[channel](p_block_instance, p_process_val);

        ff_fstate_Execute_fromFB(p_block_instance, channel);
        ff_AlertSPTimeout_Execute_fromFB(p_block_instance, channel);
    }
} //lint !e818 Softing prototype - can't change (automatically generated)



/* This line marks the end of the source */

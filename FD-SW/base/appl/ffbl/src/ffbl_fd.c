/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2013                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             Softing FF/PA FD 2.42.A                           *
*                                                                            *
* VERSION                  FF - 2.42.A                                       *
*                          PA - 2.42.A                                       *
*                                                                            *
* DATE                     14. Aug. 2013                                     *
*                                                                            *
*****************************************************************************/

/* ===========================================================================

FILE_NAME          ffbl_bas.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKFD)

INCLUDES
  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbif_idx.h"
  #include "fbap.h"
  #include "ffbl_int.h"
  #include "fbs_api.h"
  #include "ffbl_api.h"
  #include "ffbl_res.h"

LOCAL_DEFINES

#define NO_OF_FD_ALARMS   4
#define FD_FAIL           0
#define FD_OFFSPEC        1
#define FD_MAINT          2
#define FD_CHECK          3

#define FFBL_FD_SIM_RECOMMENDED_ACT   1

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

FUNCTION LOCAL VOID ffbl_fd_update_fd_alarm
  (
    IN USIGN16        fd_idx,
    IN USIGN32        field_diagnostic_value
  );

IMPORT_DATA

EXPORT_DATA

#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
NO_INIT T_FD_PARAM *               p_fd;

NO_INIT STATIC USIGN32 *           ffbl_fd_map   [NO_OF_FD_ALARMS];
NO_INIT STATIC USIGN32 *           ffbl_fd_active[NO_OF_FD_ALARMS];
NO_INIT STATIC USIGN32 *           ffbl_fd_mask  [NO_OF_FD_ALARMS];
NO_INIT STATIC ALARM_FD_DIAG *     ffbl_fd_alm   [NO_OF_FD_ALARMS];
NO_INIT STATIC USIGN8 *            ffbl_fd_pri   [NO_OF_FD_ALARMS];

NO_INIT STATIC USIGN32             ffbl_fd_alarm_bits  [NO_OF_FD_ALARMS];
NO_INIT STATIC USIGN16             ffbl_fd_start_idx;
NO_INIT STATIC USIGN16             ffbl_fd_alarm_source[32];

#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

LOCAL_DATA


/****************************************************************************/

FUNCTION PUBLIC VOID ffbl_fd_start
  (
    IN const T_FBIF_BLOCK_DESCR    * p_block_desc
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  Start routine field diagnostics module

PARAMETERS
  p_block_desc  Pointer to the object descriptions of the resource block

RETURN
  none

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16                       i;
  const T_FBIF_PARAM_DESCR *    p_param_desc;
  const T_BLK_OBJ_DESC *        p_obj_desc;

FUNCTION_BODY

  for (i = REL_IDX_RESB_FD_VER; i < p_block_desc->no_of_param; i++)
  {
    p_obj_desc = p_block_desc->list_of_obj_desc[i];

    if (p_obj_desc->r_var_obj_descr.obj_code      == RECORD_OBJECT    &&
        p_obj_desc->r_var_obj_descr.index_of_type == DS_ALARM_FD_DIAG    )
    {
      break;
    }
  }

  if (i >= p_block_desc->no_of_param)
  {
    /* The resource block has got no FD alarm parameter ------------------- */
    p_fd = NULL;

    return;
  }


  /* The resource block has an FD alarm parameter ------------------------- */
  ffbl_fd_start_idx = i - FD_PAR_FAIL_ALM;

  p_param_desc = &p_block_desc->param_dir[ffbl_fd_start_idx];

  _ASSERT(p_param_desc != NULL);

  /* Initialize the pointer to the field diagnostic structure */
  p_fd = (T_FD_PARAM *) ( ((USIGN32) p_block_desc->p_block) + p_param_desc->par_offset);

  ffbl_fd_map[FD_FAIL]        = &p_fd->fd_fail_map;
  ffbl_fd_map[FD_OFFSPEC]     = &p_fd->fd_offspec_map;
  ffbl_fd_map[FD_MAINT]       = &p_fd->fd_maint_map;
  ffbl_fd_map[FD_CHECK]       = &p_fd->fd_check_map;

  ffbl_fd_active[FD_FAIL]     = &p_fd->fd_fail_active;
  ffbl_fd_active[FD_OFFSPEC]  = &p_fd->fd_offspec_active;
  ffbl_fd_active[FD_MAINT]    = &p_fd->fd_maint_active;
  ffbl_fd_active[FD_CHECK]    = &p_fd->fd_check_active;

  ffbl_fd_mask[FD_FAIL]       = &p_fd->fd_fail_mask;
  ffbl_fd_mask[FD_OFFSPEC]    = &p_fd->fd_offspec_mask;
  ffbl_fd_mask[FD_MAINT]      = &p_fd->fd_maint_mask;
  ffbl_fd_mask[FD_CHECK]      = &p_fd->fd_check_mask;

  ffbl_fd_alm[FD_FAIL]        = &p_fd->fd_fail_alm;
  ffbl_fd_alm[FD_OFFSPEC]     = &p_fd->fd_offspec_alm;
  ffbl_fd_alm[FD_MAINT]       = &p_fd->fd_maint_alm;
  ffbl_fd_alm[FD_CHECK]       = &p_fd->fd_check_alm;

  ffbl_fd_pri[FD_FAIL]        = &p_fd->fd_fail_pri;
  ffbl_fd_pri[FD_OFFSPEC]     = &p_fd->fd_offspec_pri;
  ffbl_fd_pri[FD_MAINT]       = &p_fd->fd_maint_pri;
  ffbl_fd_pri[FD_CHECK]       = &p_fd->fd_check_pri;

  memset (ffbl_fd_alarm_bits,0x00,sizeof(ffbl_fd_alarm_bits));
  memset (ffbl_fd_alarm_source,0x00,32*sizeof(USIGN16));

  return;
}






FUNCTION GLOBAL VOID ffbl_load_fdiag_sources
  (
    IN USIGN16 const *   fdiag_sources
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  Field diagnostics alerts have got an element ‘Source Block Index’.
  ‘Source Block Index’ is the OD start index of the block that triggered the
  alert; it is zero if the alert does not relate to a certain block.

  This function allows the application to provide an array of 32 ‘Source Block
  Indexes’ for 32 field diagnostics conditions. The first element of the array
  relates to field diagnostics condition 0; the last element of the array
  relates to field diagnostics condition 31.

  The application must not call this function before fbs_start() has been
  executed.

  The Restart-with-defaults command reset the internal FFBL array of ‘Source
  Block Indexes’ to zero. After Restart-with-defaults the application has to
  re-load an array with 32 ‘Source Block Indexes’.

  The application is allowed to call this function more than one time. The FFBL
  component will always work with a copy of the newest array.

  The call of this function is optional. If the application does not call this
  function the FFBL component will use zero as ‘Source Block Index’ for all
  field diagnostics conditions.

PARAMETERS
  fdiag_sources   Pointer to an array of 32 ‘Source Block Indexes’.
                  The FFBL component works with an internal copy of the ‘Source
                  Block Index’ array. When this function returns the calling
                  function may free the buffer where fdiag_sources points to.
RETURN
  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  memcpy (ffbl_fd_alarm_source, fdiag_sources, 32*sizeof(USIGN16));

  return ;
}







FUNCTION PUBLIC VOID ffbl_fd_reset_simulate (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function resets the Field Diagnostics simulation.

PARAMETERS


RETURN

  none

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (p_fd != NULL)
  {
    p_fd->fd_simulate.enable = SIMULATION_DISABLED;
  }

  return;
}






FUNCTION PUBLIC VOID ffbl_fd_background (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


PARAMETERS


RETURN

  none

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8    i;
  BOOL      simulation_active;
  USIGN32   field_diagnostic_value;
  USIGN32   recomm_act_bitstring;

FUNCTION_BODY

  /* Check if the resource block has got FD parameters -------------------- */
  if (p_fd == NULL)
  {
    return;
  }

  _ASSERT (ffbl_call_fdev_funct.a_of_fd_get_diag_value != NULL);
  _ASSERT (ffbl_call_fdev_funct.a_of_fd_get_recomm_act != NULL);

  /*Fix of TTP947
    FF-912: 2.11 "Field Diagnostic conditions are not evaluated if the target mode of the Resource block is Out-of-Service."*/
  if (p_resource->mode_blk.target != MODE_OS)
  {
    p_fd->fd_simulate.diagnostic_value = ffbl_call_fdev_funct.a_of_fd_get_diag_value();

    if (p_fd->fd_simulate.enable == SIMULATION_ENABLED)
    {
      field_diagnostic_value = p_fd->fd_simulate.diagnostic_simulate_value;

      simulation_active = TRUE;
    }
    else
    {
      field_diagnostic_value = p_fd->fd_simulate.diagnostic_value;
      p_fd->fd_simulate.diagnostic_simulate_value = field_diagnostic_value;

      simulation_active = FALSE;
    }

    /* Update recommended action -------------------------------------------- */
    recomm_act_bitstring  = *ffbl_fd_map[0] | *ffbl_fd_map[1] |
                            *ffbl_fd_map[2] | *ffbl_fd_map[3];

    recomm_act_bitstring &=  field_diagnostic_value;

    p_fd->fd_recommen_act = ffbl_call_fdev_funct.a_of_fd_get_recomm_act
                                (recomm_act_bitstring,simulation_active);
  }
  else /* FF-912: 2.11 "... Therefore the FD_SIMULATE Values have to be 0 if the resource block is OOS".*/
  {
    p_fd->fd_simulate.diagnostic_value = 0;
    p_fd->fd_simulate.diagnostic_simulate_value = 0; 
    field_diagnostic_value = 0; 
  }


  for (i = 0; i < NO_OF_FD_ALARMS; i++)
  {
    ffbl_fd_update_fd_alarm (i, field_diagnostic_value);
  }

  return;
}







FUNCTION LOCAL VOID ffbl_fd_update_fd_alarm
  (
    IN USIGN16        fd_idx,
    IN USIGN32        field_diagnostic_value
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS

RETURN

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  INT16                 i;
  USIGN32               current_alarm_bits;
  USIGN32               alarm_occur_bits;
  USIGN32               alarm_clear_bits;
  T_FBS_ALERT_NOTIFY    alert_desc;
FUNCTION_BODY

  /* Calculate the currently active alarms -------------------------------- */
  *ffbl_fd_active[fd_idx]  = field_diagnostic_value & *ffbl_fd_map[fd_idx];

  /* Calculate the currently active and enabled alarms -------------------- */
  current_alarm_bits = *ffbl_fd_active[fd_idx] & ~(*ffbl_fd_mask[fd_idx]);

  /* Check if the FD alarm is enabled ------------------------------------- */
  if (!(p_resource->alarm_sum.disable & (ALARM_SUM_FD_FAIL >> fd_idx)))
  {
    alarm_occur_bits =  current_alarm_bits & ~ffbl_fd_alarm_bits[fd_idx];
    alarm_clear_bits = ~current_alarm_bits &  ffbl_fd_alarm_bits[fd_idx];

    if ((p_resource->ack_option & (ALARM_SUM_FD_FAIL >> fd_idx))) /* Auto ack. enabled */
    {
      if (ffbl_fd_alm[fd_idx]->unack == ALARM_UNACKED)
      {
        ffbl_fd_alm[fd_idx]->unack = ALARM_ACKED;
      }
    }
  }
  else /* FD alarm is disabled -------------------------------------------- */
  {
    alarm_occur_bits = 0x00000000UL;
    alarm_clear_bits = ffbl_fd_alarm_bits[fd_idx];

    if (ffbl_fd_alm[fd_idx]->state)
    {
      ffbl_fd_alm[fd_idx]->unack = ALARM_ACKED;
    }
  }

  /* Clear all alarms if the resource block is out-of-service ------------- */
  /* Fix of TTP947 - FF-912: "2.11 ... All alarms are cleared when the target mode is Out-of-Service." */
  if ( (p_resource->mode_blk.actual == MODE_OS) || (p_resource->mode_blk.target == MODE_OS) )
  {
    alarm_occur_bits = 0x00000000UL;
    alarm_clear_bits = ffbl_fd_alarm_bits[fd_idx];

    ffbl_fd_alarm_bits[fd_idx] = 0x00000000UL;
  }
  else
  {
    ffbl_fd_alarm_bits[fd_idx] = current_alarm_bits;
  }

  if (!alarm_occur_bits && !alarm_clear_bits)
  {
    return;
  }

  alert_desc.block_id    = 0; /* ID of resource block */
  alert_desc.rel_idx     = ffbl_fd_start_idx + FD_PAR_FAIL_ALM + fd_idx;
  alert_desc.priority    = *(ffbl_fd_pri[fd_idx]);

  /* Encode alarm subcode ------------------------------------------------- */
  alert_desc.data.fld.subcode = 0x00000000UL;

  for (i = 0; i < 32; i++)
  {
    if (current_alarm_bits & (0x80000000UL >> i))
    {
      alert_desc.data.fld.subcode  |= (0x00000001UL << i);
    }
  }

  /* Send alert description with alarm clear */
  if (alarm_clear_bits)
  {
    USIGN32 single_clear_bit;

    alert_desc.action = ALARM_CLEAR;

    for (i = 31; i >= 0; i--)
    {
      single_clear_bit = (alarm_clear_bits & (0x80000000 >> i));

      if (single_clear_bit)
      {
        alert_desc.data.fld.mbit_mask = single_clear_bit;
        alert_desc.data.fld.value     = i;
        alert_desc.data.fld.alarm_src = ffbl_fd_alarm_source[i];

        fbs_alert_notify(&alert_desc);
      }
    }
  }

  if (alarm_occur_bits)
  {
    USIGN32 single_active_bit;

    alert_desc.action = ALARM_ACTIVE;

    for (i = 31; i >= 0; i--)
    {
      single_active_bit = (alarm_occur_bits & (0x80000000 >> i));

      if (single_active_bit)
      {
        alert_desc.data.fld.mbit_mask = single_active_bit;
        alert_desc.data.fld.value     = i;
        alert_desc.data.fld.alarm_src = ffbl_fd_alarm_source[i];

        fbs_alert_notify(&alert_desc);
      }
    }
  }

  return;
}





FUNCTION PUBLIC USIGN16 ffbl_fd_pre_write_check
  (
    IN T_FBIF_WRITE_DATA *        p_write
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8    subindex;
  USIGN8 *  source;
  USIGN16   result;

FUNCTION_BODY

  subindex  = p_write->subindex;
  source    = p_write->source;
  result    = E_OK;

  switch (p_write->rel_idx - ffbl_fd_start_idx)
  {
    case FD_PAR_FAIL_PRI:
    case FD_PAR_OFFSPEC_PRI:
    case FD_PAR_MAINT_PRI:
    case FD_PAR_CHECK_PRI:
    {
      if (*source > WRITE_PRI_MAX)
      {
        result = E_FB_PARA_LIMIT;
      }

      break;
    }

    case FD_PAR_FAIL_ALM:
    {
      result = ffbl_check_unack_flag (&p_fd->fd_fail_alm.unack, source, subindex);

      break;
    }

    case FD_PAR_OFFSPEC_ALM:
    {
      result = ffbl_check_unack_flag (&p_fd->fd_offspec_alm.unack, source, subindex);

      break;
    }

    case FD_PAR_MAINT_ALM:
    {
      result = ffbl_check_unack_flag (&p_fd->fd_maint_alm.unack, source, subindex);

      break;
    }

    case FD_PAR_CHECK_ALM:
    {
      result = ffbl_check_unack_flag (&p_fd->fd_check_alm.unack, source, subindex);

      break;
    }

    case FD_PAR_SIMULATE:
    {
      USIGN8 enable_flag;

      if      (subindex == 0) enable_flag = ((SIMULATE_FD *)source)->enable;
      else if (subindex == 3) enable_flag = *source;
      else                    break;

      if (ffbl_bas_check_sim_param (enable_flag) == E_FB_PARA_CHECK)
        return (E_FB_PARA_CHECK);

      break;
    }

    default:
    {
      ;
    }
  }

  return result;
}






FUNCTION PUBLIC VOID ffbl_fd_update_alarm_sum
  (
    INOUT ALARM_SUMMARY *  p_alarm_sum
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8    i;
  USIGN16   fd_current;
  USIGN16   fd_unrep;
  USIGN16   fd_unack;

FUNCTION_BODY

  fd_current = 0;
  fd_unrep   = 0;
  fd_unack   = 0;

  for (i = 0; i < NO_OF_FD_ALARMS; i++)
  {
    if (ffbl_fd_alarm_bits[i])
    {
      fd_current |= (ALARM_SUM_FD_FAIL >> i);
    }

    if (_ALARM_NOT_REPORTED(ffbl_fd_alm[i]->state))
    {
      fd_unrep |= (ALARM_SUM_FD_FAIL >> i);
    }

    if (_ALARM_NOT_ACKNOWLEDGED(ffbl_fd_alm[i]->unack))
    {
      fd_unack |= (ALARM_SUM_FD_FAIL >> i);
    }
  }

  p_alarm_sum->current  |= fd_current;
  p_alarm_sum->unrep    |= fd_unrep;
  p_alarm_sum->unack    |= fd_unack;

  return;
}

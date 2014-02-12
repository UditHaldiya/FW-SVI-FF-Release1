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
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKBAS)

INCLUDES
  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbap.h"
  #include "ffbl_int.h"
  #include "fbs_api.h"
  #include "ffbl_res.h"


LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL USIGN8  check_target_mode (USIGN8 target,USIGN8 permit);

IMPORT_DATA

EXPORT_DATA
#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT T_FFBL_CALL_FDEV_FUNCT  ffbl_call_fdev_funct;
  NO_INIT T_FBS_ALERT_NOTIFY      ffbl_alert_notify;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

LOCAL_DATA
  static const USIGN16 block_err_table[] =
  {
    BLK_ERR_OTHER,
    BLK_ERR_BLOCK_CONFIG_ERR,
    BLK_ERR_LINK_CONFIG_ERR,
    BLK_ERR_SIMULATE_ACTIVE,
    BLK_ERR_LOCAL_OVERRIDE,
    BLK_ERR_DEV_FAULT_STATE_SET,
    BLK_ERR_DEV_NEEDS_MAINT_SOON,
    BLK_ERR_INPUT_FAILURE,
    BLK_ERR_OUTPUT_FAILURE,
    BLK_ERR_MEMORY_FAILURE,
    BLK_ERR_LOST_STATIC_DATA,
    BLK_ERR_LOST_NV_DATA,
    BLK_ERR_READBACK_CHECK_FAILED,
    BLK_ERR_DEV_NEEDS_MAINT_NOW,
    BLK_ERR_POWER_UP,
    BLK_ERR_OUT_OF_SERVICE
  };






/****************************************************************************/

FUNCTION GLOBAL VOID ffbl_init (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by FB shell in the initialization phase. The
  initialization phase is the very first phase of the device startup phase.

  This function is used to initialize data of the FF block layer (FFBL)

PARAMETERS
  None

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  ffbl_call_fdev_funct.a_of_chan_unit_check = NULL;

  return;
}





FUNCTION PUBLIC USIGN8 ffbl_bas_calc_block_mode
  (
    MODE *    p_mode,
    BSTRING   block_err
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (p_resource->mode_blk.actual == MODE_OS)
  {
    return (MODE_OS);
  }

  if ((p_mode->target & MODE_OS)||(block_err & BLK_ERR_BLOCK_CONFIG_ERR))
  {
    return (MODE_OS);
  }

  if ((p_mode->actual == MODE_OS)||(p_mode->target == MODE_MAN))
  {
    return (MODE_MAN);
  }

  return (MODE_AUTO);
}







FUNCTION PUBLIC USIGN8 ffbl_bas_determine_good_nc_status
  (
    IN ALARM_DISCRETE *   p_block_alm
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8  good_nc_substatus;

FUNCTION_BODY

  if (p_block_alm->unack == ALARM_UNACKED)
  {
    good_nc_substatus = SUB_UNACK_BLOCK_ALARM;
  }
  else if (p_block_alm->current == ALARM_ACTIVE)
  {
    good_nc_substatus = SUB_ACTIVE_BLOCK_ALARM;
  }
  else
  {
    good_nc_substatus = SUB_NON_SPECIFIC;
  }

  return (good_nc_substatus);
}








/****************************************************************************/

FUNCTION PUBLIC VOID ffbl_bas_check_scale_limits
     (
     INOUT FLOAT *  p_acknowledge,
     IN    SCALE *  scale_ptr
     )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Limits the float value to the range of -10% to +110% of the scale range

Input   p_acknowledge      pointer to float value
        scale_ptr       pointer to scale record

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  FLOAT max_val;   /* upper scale-limit of parameter */
  FLOAT min_val;   /* lower scale-limit of parameter */

FUNCTION_BODY

  max_val = scale_ptr->eu_100 + 0.1 * (scale_ptr->eu_100 - scale_ptr->eu_0);
  min_val = scale_ptr->eu_0   - 0.1 * (scale_ptr->eu_100 - scale_ptr->eu_0);

  if (*p_acknowledge < min_val)
    {
      *p_acknowledge = min_val;
    }

  else if (*p_acknowledge > max_val)
    {
      *p_acknowledge = max_val;
    }

  return;
}






FUNCTION GLOBAL USIGN16 ffbl_check_mode
  (
    IN MODE *    mode_ptr,
    IN USIGN8    subidx,
    IN USIGN8 *  source,
    IN USIGN8    valid_modes
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This function may be used to check if a write access to the MODE_BLK para-
  meter is valid.

PARAMETERS:
  mode_ptr          Pointer to the MODE_BLK parameter
  subidx            subindex to be written
  source            Pointer to the new value
  valid_modes       Bitmask of valid modes.

RETURN_VALUES:
  E_OK              The new mode value is valid
  E_FB_PARA_CHECK   The new mode value is not valid
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
MODE *    new_mode;
USIGN8    new_value;

FUNCTION_BODY

  new_value = *((USIGN8 *)source);

  if (subidx == 0)
  {
    new_mode = (MODE *)source;

    /* Check permitted modes ---------------------------------------------- */

    if (new_mode->permit & ~valid_modes)
    {
      return E_FB_PARA_CHECK;
    }

    /* Check normal mode  ------------------------------------------------- */

    if (new_mode->normal & ~new_mode->permit)
    {
      return E_FB_PARA_CHECK;
    }

    switch (new_mode->normal)
    {
      case MODE_OS:
      case MODE_MAN:
      case MODE_AUTO:
      case MODE_AUTO|MODE_CAS:
      case MODE_AUTO|MODE_RCAS:
      case MODE_AUTO|MODE_ROUT:
      {
        break;
      }
      default:
      {
        return E_FB_PARA_CHECK;
      }
    }

    /* Check target modes ------------------------------------------------- */

    return (check_target_mode (new_mode->target,new_mode->permit));
  }

  else if (subidx == 1)
  {
    return (check_target_mode (new_value,mode_ptr->permit));
  }

  else if (subidx == 3)
  {
    if (*source & ~valid_modes)
    {
      return E_FB_PARA_CHECK;
    }
  }

  else if (subidx == 4)
  {
    if (new_value & ~mode_ptr->permit)
    {
      return (E_FB_PARA_CHECK);
    }

    switch (new_value)
    {
      case MODE_OS:
      case MODE_MAN:
      case MODE_AUTO:
      case MODE_AUTO|MODE_CAS:
      case MODE_AUTO|MODE_RCAS:
      case MODE_AUTO|MODE_ROUT:
      {
        break;
      }
      default:
      {
        return E_FB_PARA_CHECK;
      }
    }
  }

  return (E_OK);
}





FUNCTION LOCAL USIGN8 check_target_mode
  (
    IN USIGN8   target,
    IN USIGN8   permit
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if ((target & ~permit)||(target == 0))
  {
    return (E_FB_PARA_CHECK);
  }

  #if 0
  else if ((target & MODE_OS)&&(target != MODE_OS))
  {
    return (E_FB_PARA_CHECK);
  }

  else if ((target & MODE_MAN)&&(target != MODE_MAN))
  {
    return (E_FB_PARA_CHECK);
  }
  #endif /* 0 */

  else if ((target & MODE_MAN)&&(target & MODE_AUTO))
  {
    return (E_FB_PARA_CHECK);
  }

  else if ((target & MODE_RCAS)&&(target & MODE_ROUT))
  {
    return (E_FB_PARA_CHECK);
  }

  else if ((target & (MODE_RCAS|MODE_ROUT|MODE_CAS)) && !(target & MODE_AUTO))
  {
    return (E_FB_PARA_CHECK);
  }

  else
  {
    return (E_OK);
  }
}





FUNCTION PUBLIC USIGN16 ffbl_bas_check_channel
  (
    IN USIGN16                     channel,
    IN const T_FBIF_BLOCK_DESCR *  p_block_desc
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This function may be used to check if a write access to the CHANNEL para-
  meter is valid.

PARAMETERS:
  channel           New value
  no_of_channels    Number of entries in the channel list
  channel_list_ptr  Pointer to the channel list

RETURN_VALUES:
  E_OK              The new channel value is valid
  E_FB_PARA_CHECK   The new channel value is not valid
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16  i;
    u16 ret = E_FB_PARA_CHECK;

FUNCTION_BODY

    for (i = 0; i < p_block_desc->no_of_channels; i++)
    {
        if (p_block_desc->channel_list[i].channel == channel)
        {
            ret = E_OK;
            break;
        }
    }

    return ret;
}






FUNCTION GLOBAL USIGN16 ffbl_check_unack_flag
  (
    IN USIGN8 *       p_alarm_unack,
    IN USIGN8 *       p_acknowledge,
    IN USIGN8         subindex
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called when the Unacknowledged flag of a alert parameter is
  written. It checks if the write access is valid.

PARAMETERS
  p_alarm_unack       Pointer to the alert parameter
  p_acknowledge       Pointer to the location where the acknowledge value
                      is buffered
  subindex            Subindex used in the write indication

RETURN_VALUES
  E_OK                Write access is accepted; alert is acknowledged
  E_FB_ALM_ACK        Alert is already acknowledged
  E_FB_DATA_NO_WRITE  Subindex is not valid
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (subindex < 2)
  {
    if (*p_alarm_unack == ALARM_UNACKED)
    {
      /* Set value of the unacked flag to ALARM_ACKED independently of the
         received value */

      *p_acknowledge = ALARM_ACKED;

      return (E_OK);
    }
    else
    {
      return (E_FB_ALM_ACK);
    }
  }
  else
  {
    return (E_FB_DATA_NO_WRITE);
  }
}







FUNCTION GLOBAL USIGN16 ffbl_check_block_alarm_unack_flag
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance,
    IN USIGN8 *                   p_alarm_unack,
    IN USIGN8 *                   p_acknowledge,
    IN USIGN8                     subindex
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called when the Unacknowledged flag of a block alarm
  parameter is written. It checks if the write access is valid.

  Note: Due to modifications in FF-890 FS 1.8 and ITK 5.2 it is not necessary
  anymore to handle the alarm unack flag of block alarms in a different way as
  the unack flag of other alarms. Therefore it is not necessary anymore to use
  a separate function for the handling of a block alarm’s unack flag

  This function is just available to keep the compatibility with the FFBL
  interface of the FF-FD-SW 2.2x.

PARAMETERS
  p_block_instance    Pointer to the block whose block alarm should be
                      acknowledged
  p_alarm_unack       Pointer to the block alarm parameter
  p_acknowledge       Pointer to the location where the acknowledge value
                      is buffered
  subindex            Subindex used in the write indication

RETURN_VALUES
  E_OK                Write access is accepted; block alarm is acknowledged
  E_FB_ALM_ACK        Block alarm is already acknowledged
  E_FB_DATA_NO_WRITE  Subindex is not valid
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _UNREF_PARAM (p_block_instance);

  return ffbl_check_unack_flag (p_alarm_unack,p_acknowledge,subindex);
}







FUNCTION PUBLIC VOID ffbl_bas_write_grant_deny
  (
    IN USIGN8           subindex,
    IN USIGN8 *         source,
    IN ACCESS_PERM *    p_grant_deny
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8 new_deny;

FUNCTION_BODY

  *source &= 0xFE; /* Delete the undefined bit  */

  if (subindex != 2)
  {
    p_grant_deny->denied |= p_grant_deny->grant; /* Store grant bits */

    new_deny = (p_grant_deny->denied &  ~(*source)); /* Remove all grant bits from deny */

    if (subindex == 0)
    {
      ((ACCESS_PERM *)source)->denied = new_deny;
    }
    else /* subindex == 1 */
    {
      p_grant_deny->denied = new_deny;
    }
  }

  return;
}




/****************************************************************************/

FUNCTION PUBLIC USIGN8 ffbl_bas_check_sim_param
  (
    IN USIGN8     enable_flag
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (enable_flag == SIMULATION_ENABLED)
  {
    if (p_resource->block_err & BLK_ERR_SIMULATE_ACTIVE)
    {
      return (E_OK);
    }
    else /* !(p_resource->block_err & BLK_ERR_SIMULATE_ACTIVE) */
    {
      return (E_FB_PARA_CHECK);
    }
  }

  else if (enable_flag == SIMULATION_DISABLED)
  {
    return (E_OK);
  }

  else
  {
    return (E_FB_PARA_CHECK);
  }
}





FUNCTION VOID ffbl_bas_update_sim_param
  (
    IN USIGN8 *   enable_flag,
    IN USIGN16 *  block_err
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (p_resource->block_err & BLK_ERR_SIMULATE_ACTIVE)
  {
    if (*enable_flag == SIMULATION_ENABLED)
    {
      if (!(*block_err & BLK_ERR_SIMULATE_ACTIVE))
      {
        *block_err |= BLK_ERR_SIMULATE_ACTIVE;

        return;
      }
    }
    else /* *enable_flag == SIMULATION_DISABLED */
    {
      if (*block_err & BLK_ERR_SIMULATE_ACTIVE)
      {
        *block_err &= ~BLK_ERR_SIMULATE_ACTIVE;

        return;
      }
    }
  }
  else /* (!(p_resource->block_err & BLK_ERR_SIMULATE_ACTIVE)) */
  {
    if (*enable_flag == SIMULATION_ENABLED)
    {
      *enable_flag = SIMULATION_DISABLED;

      if (*block_err & BLK_ERR_SIMULATE_ACTIVE)
      {
        *block_err &= ~BLK_ERR_SIMULATE_ACTIVE;

        return;
      }
    }
  }

  return;
}



/****************************************************************************/

FUNCTION PUBLIC FLOAT ffbl_bas_rescale_value
  (
    IN FLOAT    old_value,
    IN SCALE *  old_scale_p,
    IN SCALE *  new_scale_p
  )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

rescales a value from one scale to another scale

Input   old_value     original float value
        old_scale_p   pointer to original scale record
        new_scale_p   pointer to dest scale record

Return  rescaled value

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  FLOAT      new_value;                 /* return value */

FUNCTION_BODY

  if ((old_scale_p->eu_0   != new_scale_p->eu_0)  ||
      (old_scale_p->eu_100 != new_scale_p->eu_100)  )
  {
    /* Intermediate rescale of value in percent */
    new_value = (old_value - old_scale_p->eu_0) / (old_scale_p->eu_100 - old_scale_p->eu_0);

    /* Calculate value based on new scale */
    new_value = new_value * (new_scale_p->eu_100 - new_scale_p->eu_0) + new_scale_p->eu_0;
  }
  else /* old scale and new scale are identical */
  {
    new_value = old_value;
  }

  return(new_value);
}



/****************************************************************************/

FUNCTION GLOBAL VOID ffbl_update_block_alarm
  (
    IN USIGN16            block_id,
    IN USIGN16            blk_alm_idx,
    IN USIGN16            alarm_bits,
    IN USIGN16 *          old_alarm_bits
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to update the block alarm parameter. If there is a change
  in the block error state this function will generate an alert notification.

PARAMETERS
  block_id          Block identfier as generated by GenVFD (defined in fbif_idx.h)
  blk_alm_idx       Relative index of the block alarm parameter within the block.
                    (The relative index of the block alarm parameter is calculated
                    by GenVFD.)
  alarm_bits        Current state of the block error parameter
  old_alarm_bits    Previous state of the block error parameter

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  INT8                  i;
  T_FBS_ALERT_NOTIFY    alert_desc;
  USIGN16               changed_bits;
  USIGN16               cleared_bits;
  USIGN16               activated_bits;

FUNCTION_BODY

    osif_disable_all_tasks();
    USIGN16 loc_old_alarm_bits = *old_alarm_bits;
    changed_bits = loc_old_alarm_bits ^ alarm_bits;
    if (changed_bits)
    {
        *old_alarm_bits = alarm_bits;
    }
    osif_enable_all_tasks();


  if (changed_bits)
  {
    cleared_bits    = changed_bits & loc_old_alarm_bits;
    activated_bits  = changed_bits & alarm_bits;

    alert_desc.block_id          = block_id;
    alert_desc.rel_idx           = blk_alm_idx;
    alert_desc.priority          = ALERT_PRIO_2;

    /* Encode alarm subcode ----------------------------------------------- */
    {
      T_FBIF_BLOCK_INSTANCE *     p_block_instance;
      const T_FBIF_BLOCK_DESCR *  p_block_desc;
      const T_FBIF_PARAM_DESCR *  p_param_desc;
      USIGN8 *                    p_param;
      USIGN16                     block_err;

      p_block_instance  = fbs_get_block_inst (block_id);
      p_block_desc      = p_block_instance->p_block_desc;
      p_param_desc      = &p_block_desc->param_dir[6];
      p_param           = (USIGN8 *)p_block_desc->p_block + p_param_desc->par_offset;
      block_err         = *((USIGN16 *)p_param);

      alert_desc.data.dsc.subcode  = 0;

      for (i = 15;i >= 0;i--)
      {
        if (block_err & block_err_table[i])
        {
          alert_desc.data.dsc.subcode  |= block_err_table[15 - i];
        }
      }
    }

    if (activated_bits & BLK_ERR_OUT_OF_SERVICE)
    {
      /* Clear all active alarms ------------------------------------------ */
      if (cleared_bits)
      {
        alert_desc.action = ALARM_CLEAR;

        for (i = 15;i >= 0;i--)
        {
          if (cleared_bits & block_err_table[i])
          {
            alert_desc.data.dsc.value     = i;
            alert_desc.data.dsc.mbit_mask = block_err_table[i];

            fbs_alert_notify (&alert_desc);
          }
        }
      }

      /* Activate out-of-service alarm------------------------------------- */
      alert_desc.action             = ALARM_ACTIVE;
      alert_desc.data.dsc.value     = 15;
      alert_desc.data.dsc.mbit_mask = block_err_table[15];

      fbs_alert_notify (&alert_desc);
    }

    else if (cleared_bits & BLK_ERR_OUT_OF_SERVICE)
    {
      /* Clear out-of-service alarm --------------------------------------- */
      alert_desc.action             = ALARM_CLEAR;
      alert_desc.data.dsc.value     = 15;
      alert_desc.data.dsc.mbit_mask = block_err_table[15];

      fbs_alert_notify (&alert_desc);

      /* Activate all active alarms --------------------------------------- */
      if (activated_bits)
      {
        alert_desc.action = ALARM_ACTIVE;

        for (i = 15;i >= 0;i--)
        {
          if (activated_bits & block_err_table[i])
          {
            alert_desc.data.dsc.value     = i;
            alert_desc.data.dsc.mbit_mask = block_err_table[i];

            fbs_alert_notify (&alert_desc);
          }
        }
      }
    }

    else /* BLK_ERR_OUT_OF_SERVICE is not changed */
    {
      /* Encode alarm subcode --------------------------------------------- */
      alert_desc.data.dsc.subcode  = 0;

      for (i = 15;i >= 0;i--)
      {
        if (alarm_bits & block_err_table[i])
        {
          alert_desc.data.dsc.subcode  |= block_err_table[15 - i];
        }
      }

      if (cleared_bits)
      {
        alert_desc.action = ALARM_CLEAR;

        for (i = 15;i >= 0;i--)
        {
          if (cleared_bits & block_err_table[i])
          {
            alert_desc.data.dsc.value     = i;
            alert_desc.data.dsc.mbit_mask = block_err_table[i];

            fbs_alert_notify (&alert_desc);
          }
        }
      }

      if (activated_bits)
      {
        alert_desc.action = ALARM_ACTIVE;

        for (i = 15;i >= 0;i--)
        {
          if (activated_bits & block_err_table[i])
          {
            alert_desc.data.dsc.value     = i;
            alert_desc.data.dsc.mbit_mask = block_err_table[i];

            fbs_alert_notify (&alert_desc);
          }
        }
      }
    }
  }

  return;
}






FUNCTION GLOBAL VOID ffbl_bas_update_extended_block_alarm
  (
    IN const T_FBIF_BLOCK_DESCR * p_block_desc,
    INOUT ALARM_DISCRETE *        p_block_alm,
    IN BSTRING                    alm_disable,
    IN BSTRING                    ack_option,
    IN USIGN16                    block_alarm_bits,
    INOUT USIGN16 *               p_old_block_alarm_bits
  )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This utility function handles the alarm generation for blocks with
  have both parameters block_alm and alarm_sum.

PARAMETERS:
  p_block_desc
  blk_alm_action      Set or clear the alarm
  blk_alm_code        Block alram codes are defined in fbap.h (ALM_SC_xxx)
  blk_alm_ptr         Pointer to the BLOCK_ALM parameter
  blk_alm_sum         Pointer to the ALARM_SUM parameter
  block_alarm_bits           content of the BLOCK_ERR parameter
  ack_option          content of the ACK_OPTION parameter

RETURN_VALUES:
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (!(alm_disable & ALARM_SUM_BLOCK))
  {

    ffbl_update_block_alarm
      (
        p_block_desc->block_id,p_block_desc->idx_blk_alm,
        block_alarm_bits,p_old_block_alarm_bits
      );

    if (ack_option & ALARM_SUM_BLOCK) /* Auto ack. enabled */
    {
      if (p_block_alm->unack == ALARM_UNACKED)
      {
        p_block_alm->unack = ALARM_ACKED;
      }
    }
  }
  else /* Block alarms are disabled --------------------------------------- */
  {
    ffbl_update_block_alarm
      (
        p_block_desc->block_id,p_block_desc->idx_blk_alm,
        0x0000,p_old_block_alarm_bits
      );

    if (p_block_alm->state)
    {
      p_block_alm->unack = ALARM_ACKED;
    }
  }

  return;
}







FUNCTION GLOBAL VOID ffbl_disable_update_events
  (
    IN EVENT *  p_update_evt
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  According to the FF specifications no update events are sent if a block is in
  mode out-of-service. As the update events are generated by the function block
  shell and the function block shell has no knowledge about the block state,
  application layer and function block layer have to disable and enable the
  generation of update events by interface functions.

  This function is called when a block goes to Out-of-Service. As after power-on
  a block is usually in Out-of-Service the function is also called by the start
  function of each block.

PARAMETERS
  p_update_evt      Pointer to the update event parameter

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  p_update_evt->standard_type = STORE_UPDATE;

  return;
}






FUNCTION GLOBAL VOID ffbl_enable_update_events
  (
    IN USIGN16  block_id,
    IN USIGN16  update_evt_idx,
    IN EVENT *  update_evt,
    IN USIGN16  static_revision
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  According to the FF specifications no update events are sent if a block is in
  mode out-of-service.  As the update events are generated by the function block
  shell and the function block shell has no knowledge about the block state,
  application layer and function block layer have to disable and enable the
  generation of update events by interface functions.

  This function is called when a block goes out of Out-of-Service. The function
  block shell will send an update event if a static parameter has been changed
  while the block was Out-of-Service.

PARAMETERS
  block_id          Block identfier as generated by GenVFD (defined in fbif_idx.h)
  update_evt_idx    Relative index of the update event parameter within the block.
                    (The relative index of the update event parameter is calculated
                    by GenVFD.)
  update_evt        Pointer to the update event
  static_revision   Current value of the static revision parameter

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_FBS_ALERT_NOTIFY  alert_desc;

FUNCTION_BODY

  if (update_evt->standard_type == SEND_UPDATE)
  {
    update_evt->unack                   = ALARM_UNACKED;

    alert_desc.action                   = UPDATE_EVENT;
    alert_desc.block_id                 = block_id;
    alert_desc.rel_idx                  = update_evt_idx;
    alert_desc.priority                 = ALERT_PRIO_2;
    alert_desc.data.evt.static_revision = static_revision;
    alert_desc.data.evt.relative_index  = update_evt->relative_index;

    fbs_alert_notify (&alert_desc);
  }

  update_evt->standard_type = SEND_UPDATE;
}

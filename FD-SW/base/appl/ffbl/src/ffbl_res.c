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

FILE_NAME          ffbl_res.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
#include "keywords.h"
#define  MODULE_ID      (COMP_FFBL + MOD_FBLKRES)

INCLUDES

  #include "base.h"
  #include "osif.h"
  #include "hw_if.h"
  #include "version.h"
  #include "vfd.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_res.h"
  #include <fbif_dcl.h>
  #include <fbif_idx.h>

  #include "except.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID Update_alarm_sum_RESB (VOID);

IMPORT_DATA

EXPORT_DATA
#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT T_RESOURCE *                      p_resource;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

LOCAL_DATA
#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT STATIC const T_FBIF_BLOCK_DESCR * p_resb_desc;
  NO_INIT STATIC USIGN16                    ffbl_res_write_lock_index;
  NO_INIT STATIC USIGN16                    ffbl_res_old_block_alarm_bits;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */


/****************************************************************************/

FUNCTION PUBLIC VOID Start_RESB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  p_resb_desc   = p_block_instance->p_block_desc;
  p_resource    = p_resb_desc->p_block;

  /* Initialize resource internal data ------------------------------------ */
  ffbl_res_old_block_alarm_bits = 0;
  ffbl_res_write_lock_index     = p_resb_desc->start_index + RES_PAR_WRITE_LOCK;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_resource->update_evt);

#ifdef USE_FIELD_DIAGNOSTICS /* -------------------------------------------- */
  ffbl_fd_start (p_resb_desc);
#endif /* USE_FIELD_DIAGNOSTICS */

  if (p_resource->feature_sel & RES_FEAT_HARD_WRITE_LOCK_SUPP)
  {
    // if (hw_write_protection () )
    if (hw_write_protection () || Remote_WP_Jumper())
    {
      p_resource->write_lock = WRITE_LOCKED;
    }
  }

  return;
}






FUNCTION PUBLIC USIGN16 Read_handler_RESB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBIF_READ_DATA *          p_read
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Return  E_OK    or a neg. return code defined in fbap.h
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _UNREF_PARAM (p_block_instance);
  p_resb_desc   = p_block_instance->p_block_desc;
  T_FBIF_RESB *p_resource_full = (T_FBIF_RESB *)(void *)p_resource;


 /* Check and/or update parameter to be read ----------------------------- */
  switch (p_read->rel_idx)
  {
    case RES_PAR_RS_STATE:
    {
      /* Update resource state -------------------------------------------- */

      if (p_resource->mode_blk.actual == MODE_OS)
      {
        p_resource->rs_state = RS_STATE_STANDBY;
      }
      else
      {
        if (fbs_links_online ())
          p_resource->rs_state = RS_STATE_ON_LINE;
        else
          p_resource->rs_state = RS_STATE_ON_LINE_LNKING;
      }

      break;
    }

    case RES_PAR_NV_CYCLE_T:
    {
      break;
    }

    case RES_PAR_ALARM_SUM:
    {
      Update_alarm_sum_RESB ();

      break;
    }
      case REL_IDX_RESB_SOFTWARE_REV:
      {
          memcpy(p_resource_full->software_rev, c_resb.software_rev, sizeof(p_resource_full->software_rev));
          break;
      }
      case REL_IDX_RESB_SOFTWARE_REV_FF:
      {
          p_resource_full->software_rev_ff = c_resb.software_rev_ff;
          break;
      }
      case REL_IDX_RESB_SOFTWARE_REV_APP:
      {
          p_resource_full->software_rev_app = c_resb.software_rev_app;
          break;
      }
  }

  return E_OK;
}







FUNCTION PUBLIC USIGN16 Write_handler_RESB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance,
    IN T_FBIF_WRITE_DATA *        p_write
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Return  E_OK    or a neg. return code defined in fbap.h
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

  switch (p_write->rel_idx)
  {
    case RES_PAR_ALERT_KEY:
    {
      if (*source == 0) return (E_FB_PARA_LIMIT);

      break;
    }

    case RES_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (&p_resource->mode_blk,
                          subindex, source, MODE_RES_VALID);
      break;
    }

    case RES_PAR_GRANT_DENY:
    {
      ffbl_bas_write_grant_deny (subindex,source,&p_resource->grant_deny);

      break;
    }

    case RES_PAR_RESTART:
    {
      if ((source[0] > RS_RESTART_MAX_VALUE)&&(source[0] != RS_RESTART_HIDDEN)
          && (source[0] != RS_RESTART_TEST_WATCHDOG) )
      {
        #ifdef FBL_BLOCK_INST /* ------------------------------------------ */
        if (source[0] == RS_RESET_BLOCKS) break;
        #endif /* FBL_BLOCK_INST */

        return (E_FB_PARA_LIMIT);
      }

      break;
    }

    case RES_PAR_FEATURE_SEL:
    {
      *((USIGN16 *)source) &= p_resource->features; /* Clear all unsupp bits */

      /* FAILURE --> HW Write Lock Jumper could be in place before according feature_sel bit will be enabled */
      #if 0
      if (*((USIGN16 *)source) & RES_FEAT_HARD_WRITE_LOCK_SUPP)
      {
        /* Feature 'hard write lock' shall be enabled --------------------- */

        // if (hw_write_protection ())
        if (hw_write_protection () || Remote_WP_Jumper())
        {
          /* Do not allow to enable the feature 'hard write lock'
             when the physical hard write lock is enabled ----------------- */

          return (E_FB_WRITE_LOCK);
        }
      }
      #endif
      break;
    }

    case RES_PAR_CYCLE_SEL:
    {
      *((USIGN16 *)source) &= p_resource->cycle_type; /* Clear all unsupp bits */

      break;
    }

    case RES_PAR_SET_FSTATE:
    {
      if (*source == SET_FAULT_STATE_OFF)
      {
        ;  /* do nothing */
      }
      else if ((*source == SET_FAULT_STATE_SET)                  &&
               (p_resource->feature_sel & RES_FEAT_FAULT_STATE_SUPP)   )
      {
        p_resource->fault_state = FAULT_STATE_ACTIVE; /* Set fault state */
        p_resource->block_err |= BLK_ERR_DEV_FAULT_STATE_SET; /* Set BLOCK_ERR bit */
        *source = SET_FAULT_STATE_OFF;             /* Reset parameter */
      }
      else
      {
        return (E_FB_PARA_CHECK);
      }

      break;
    }

    case RES_PAR_CLR_FSTATE:
    {
      if (*source == CLR_FAULT_STATE_OFF)
      {
        ;  /* do nothing */
      }
      else if (*source == CLR_FAULT_STATE_CLEAR)
      {
        p_resource->fault_state = FAULT_STATE_CLEAR; /* Clear fault state */
        p_resource->block_err &= ~BLK_ERR_DEV_FAULT_STATE_SET; /* Clear BLOCK_ERR bit */
        *source = CLR_FAULT_STATE_OFF;            /* Reset parameter */
      }
      else
      {
        return (E_FB_PARA_CHECK);
      }

      break;
    }

    case RES_PAR_LIM_NOTIFY:
    {
      if (*source > p_resource->max_notify) return (E_FB_PARA_LIMIT);

      break;
    }

    case RES_PAR_CONFIRM_TIME:
    {
      if (*((USIGN32 *)source) == 0) break;

      if (*((USIGN32 *)source) < p_resource->min_cycle_t) return (E_FB_PARA_LIMIT);

      break;
    }

    case RES_PAR_WRITE_LOCK:
    {
      if (p_resource->feature_sel & RES_FEAT_HARD_WRITE_LOCK_SUPP)
      {
        /* If hard write lock is enabled, writes to write-lock are not allowed */

        return (E_FB_PARA_CHECK);
      }

      if (!(p_resource->feature_sel & RES_FEAT_SOFT_WRITE_LOCK_SUPP))
      {
        /* If soft write lock is not enabled, writes to write-lock are not allowed */

        return (E_FB_PARA_CHECK);
      }

      if (!((*source == WRITE_LOCKED) || (*source == WRITE_UNLOCKED)))
      {
        return (E_FB_PARA_CHECK);
      }

      break;
    }

    case RES_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_resource->update_evt.unack, source, subindex);

      break;
    }

    case RES_PAR_BLOCK_ALM:
    {
      result = ffbl_check_unack_flag (&p_resource->block_alm.unack, source, subindex);

      break;
    }

    case RES_PAR_WRITE_PRI:
    {
      if (*source > WRITE_PRI_MAX) return (E_FB_PARA_LIMIT);

      break;
    }

    case RES_PAR_WRITE_ALM:
    {
      result = ffbl_check_unack_flag (&p_resource->write_alm.unack,
                                                   source, subindex);
      break;
    }

    default:
    {
     #ifdef USE_FIELD_DIAGNOSTICS /* --------------------------------------- */
     result = ffbl_fd_pre_write_check (p_write);
     #endif /* USE_FIELD_DIAGNOSTICS */
    }

  }


  if (result == E_OK)
  {
    result = fbs_write_param (p_block_instance,p_write);
  }


  if (result != E_OK) return (result);


  if (p_write->rel_idx == RES_PAR_RESTART)
  {
    p_resource->restart = RS_RESTART_RUN;
  }
  #ifdef USE_FIELD_DIAGNOSTICS
  else if (p_write->rel_idx == (FD_PAR_SIMULATE + RES_PAR_ITK_VER + 1))
  {
    ffbl_fd_background ();
  }
  #endif
  else if (p_write->rel_idx == RES_PAR_TEST_RW)
  {
    if (p_resource->test_rw.value7 == 85540)
    {
      memset (p_resource->test_rw.value9,0x20,sizeof(p_resource->test_rw.value9));
      memcpy (p_resource->test_rw.value9,PRJ_LABEL,strlen(PRJ_LABEL));
    }
  }

  return (result);
}







FUNCTION GLOBAL VOID Background_RESB
   (
     T_FBIF_BLOCK_INSTANCE *    p_block_instance
   )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

PARAMETERS:
  p_block_instance  Pointer to the transducer block to be executed

RETURN_VALUES:
  none
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8    mode_action;
  USIGN16   alarm_bits;
  BOOL      enable_update_evt = FALSE;

FUNCTION_BODY

  /* Update simulate state of the resource -------------------------------- */
  if (  (hw_get_simulate_enable_jumper() == TRUE)
      ||(Lui_Simulation() != FALSE) )
  {
    if (!(p_resource->block_err & BLK_ERR_SIMULATE_ACTIVE))
    {
      p_resource->block_err |= BLK_ERR_SIMULATE_ACTIVE;
    }
  }
  else
  {
    if (p_resource->block_err & BLK_ERR_SIMULATE_ACTIVE)
    {
      p_resource->block_err &= ~BLK_ERR_SIMULATE_ACTIVE;

      #ifdef USE_FIELD_DIAGNOSTICS /* -------------------------------------------- */
      ffbl_fd_reset_simulate ();
      #endif /* USE_FIELD_DIAGNOSTICS */
    }
  }

  /* Update write protection of the resource ------------------------------ */
  if (p_resource->feature_sel & RES_FEAT_HARD_WRITE_LOCK_SUPP)
  {
    // if (hw_write_protection ())
    if (hw_write_protection () || Remote_WP_Jumper())
    {
      if (p_resource->write_lock == WRITE_UNLOCKED)
      {
        p_resource->write_lock = WRITE_LOCKED;
      }
    }
    else
    {
      if (p_resource->write_lock == WRITE_LOCKED)
      {
        p_resource->write_lock = WRITE_UNLOCKED;
      }
    }
  }


  /* Update actual mode of the resource block ----------------------------- */
  if (p_resource->mode_blk.target & MODE_OS)
  {
    if (p_resource->mode_blk.actual != MODE_OS)
    {
      p_resource->mode_blk.actual = MODE_OS;

      mode_action = MODE_INTO_OS;
    }
    else
    {
      mode_action = MODE_IN_OS;
    }
  }

  else /* target_mode == MODE_AUTO */
  {
    if (p_resource->mode_blk.actual != MODE_AUTO)
    {
      p_resource->mode_blk.actual = MODE_AUTO;

      mode_action = MODE_OUTOF_OS;
    }
    else
    {
      mode_action = MODE_OK;
    }
  }


  /* Prepare alerts in dependance of the new mode ------------------------_ */
  if (mode_action == MODE_INTO_OS)
  {
    p_resource->block_err |= BLK_ERR_OUT_OF_SERVICE;

    ffbl_disable_update_events (&p_resource->update_evt);
  }

  else if (mode_action == MODE_OUTOF_OS)
  {
    if (p_resource->block_err & BLK_ERR_LOST_STATIC_DATA)
    {
      p_resource->mode_blk.actual = MODE_OS;
    }
    else
    {
      p_resource->block_err &= ~BLK_ERR_OUT_OF_SERVICE;

      enable_update_evt = TRUE;
    }
  }

  if (p_resource->block_err & BLK_ERR_OUT_OF_SERVICE)
  {
    alarm_bits = BLK_ERR_OUT_OF_SERVICE;
  }
  else
  {
    alarm_bits = p_resource->block_err;
  }


//lint -estring(457,ffbl_res_old_block_alarm_bits) Review carefully since it is fragile;
//for now I made the underlying ffbl_update_block_alarm() handle ffbl_res_old_block_alarm_bits atomically
  /* Update resource block alarm ------------------------------------------ */
  ffbl_bas_update_extended_block_alarm
  (
    p_block_instance->p_block_desc, &p_resource->block_alm,
    p_resource->alarm_sum.disable, p_resource->ack_option,
    alarm_bits, &ffbl_res_old_block_alarm_bits
  );


  /* Enable update events ------------------------------------------------- */
  if (enable_update_evt)
  {
    ffbl_enable_update_events (
      p_resb_desc->block_id,
      p_resb_desc->idx_upd_evt,
      &p_resource->update_evt,
      p_resource->st_rev
    );
  }

#ifdef USE_FIELD_DIAGNOSTICS /* -------------------------------------------- */
  /* Fix of TTP#947 */
  ffbl_fd_background ();
#endif /* USE_FIELD_DIAGNOSTICS */

  if (p_resource->write_alm.current == ALARM_CLEAR)
  {
    if (p_resource->write_lock == WRITE_UNLOCKED)
    {
      if (!(p_resource->alarm_sum.disable & ALARM_SUM_DISC))
      {
        if ((p_resource->feature_sel & RES_FEAT_HARD_WRITE_LOCK_SUPP) ||
            (p_resource->feature_sel & RES_FEAT_SOFT_WRITE_LOCK_SUPP)    )
        {
          if (p_resource->write_pri)
          {
            T_FBS_ALERT_NOTIFY  resb_alert_notify;

            resb_alert_notify.action              = ALARM_ACTIVE;
            resb_alert_notify.block_id            = 0; /* block id for resource block is fix */
            resb_alert_notify.rel_idx             = RES_PAR_WRITE_ALM;
            resb_alert_notify.priority            = p_resource->write_pri;
            resb_alert_notify.data.dsc.subcode    = 0;
            resb_alert_notify.data.dsc.value      = WRITE_UNLOCKED;
            resb_alert_notify.data.dsc.mbit_mask  = 0;

            fbs_alert_notify (&resb_alert_notify);

            if (p_resource->ack_option & ALARM_SUM_DISC)      /* Auto ack. enabled */
                 p_resource->write_alm.unack = ALARM_ACKED;
            else p_resource->write_alm.unack = ALARM_UNACKED;
          }
        }
      }
    }
  }

  else if (p_resource->write_alm.current == ALARM_ACTIVE)
  {
    BOOL clear_write_alm = FALSE;

    if (p_resource->write_lock == WRITE_LOCKED)
    {
      clear_write_alm = TRUE;
    }

    if (p_resource->alarm_sum.disable & ALARM_SUM_DISC)
    {
      clear_write_alm = TRUE;
    }

    if (p_resource->write_pri == 0)
    {
      clear_write_alm = TRUE;
    }

    if ((!(p_resource->feature_sel & RES_FEAT_HARD_WRITE_LOCK_SUPP)) &&
        (!(p_resource->feature_sel & RES_FEAT_SOFT_WRITE_LOCK_SUPP))    )
    {
      clear_write_alm = TRUE;
    }

    if (clear_write_alm)
    {
      T_FBS_ALERT_NOTIFY  resb_alert_notify;

      resb_alert_notify.action              = ALARM_CLEAR;
      resb_alert_notify.block_id            = 0; /* block id for resource block is fix */
      resb_alert_notify.rel_idx             = RES_PAR_WRITE_ALM;
      resb_alert_notify.priority            = p_resource->write_pri;
      resb_alert_notify.data.dsc.subcode    = 0;
      resb_alert_notify.data.dsc.mbit_mask  = 0;

      if (p_resource->write_lock == WRITE_LOCKED)
      {
        resb_alert_notify.data.dsc.value      = WRITE_LOCKED;
      }
      else
      {
        resb_alert_notify.data.dsc.value      = 0;
      }


      fbs_alert_notify (&resb_alert_notify);
    }
  }

  return;
}






FUNCTION LOCAL VOID Update_alarm_sum_RESB (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* Update the 'current' bitfield ---------------------------------------- */
  p_resource->alarm_sum.current = 0;

  if (p_resource->block_err)
       p_resource->alarm_sum.current |= ALARM_SUM_BLOCK;

  if (p_resource->write_alm.current == ALARM_ACTIVE)
    p_resource->alarm_sum.current |= ALARM_SUM_DISC;


  /* Update the 'unrep' bitfield ------------------------------------------ */
  p_resource->alarm_sum.unrep = 0;

  if (_ALARM_NOT_REPORTED(p_resource->block_alm.state))
       p_resource->alarm_sum.unrep |= ALARM_SUM_BLOCK;

  if (_ALARM_NOT_REPORTED(p_resource->write_alm.state))
       p_resource->alarm_sum.unrep |= ALARM_SUM_DISC;


  /* Update the 'unack' bitfield ------------------------------------------ */
  p_resource->alarm_sum.unack = 0;

  if (_ALARM_NOT_ACKNOWLEDGED(p_resource->block_alm.unack))
       p_resource->alarm_sum.unack |= ALARM_SUM_BLOCK;

  if (_ALARM_NOT_ACKNOWLEDGED(p_resource->write_alm.unack))
       p_resource->alarm_sum.unack |= ALARM_SUM_DISC;


#ifdef USE_FIELD_DIAGNOSTICS
  ffbl_fd_update_alarm_sum (&p_resource->alarm_sum);
#endif /* USE_FIELD_DIAGNOSTICS */

  p_resource->alarm_sum.current &= ~p_resource->alarm_sum.disable;
  p_resource->alarm_sum.unrep   &= ~p_resource->alarm_sum.disable;
  p_resource->alarm_sum.unack   &= ~p_resource->alarm_sum.disable;

  return;
}





FUNCTION GLOBAL VOID ffbl_provide_resb_data_to_fdc
  (
    OUT USIGN32 *   p_manufac_id,
    OUT USIGN16 *   p_dev_type,
    OUT USIGN32 *   p_min_cycle_t
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by Field Device Communication stack in the device
  startup phase.

  It provides some resource block parameters.

PARAMETERS
  p_manufac_id      Pointer to the parameter MAX_NOTIFY
  p_dev_type        Pointer to the parameter DEV_TYPE
  p_min_cycle_t     Pointer to the parameter MIN_CYCLE_T

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _ASSERT (p_resource != NULL);

  *p_manufac_id   = p_resource->manufac_id;
  *p_dev_type     = p_resource->dev_type;
  *p_min_cycle_t  = p_resource->min_cycle_t;

  return;
}






FUNCTION GLOBAL VOID ffbl_provide_resb_data_to_fbs
  (
    OUT USIGN8 * *   pp_max_notify,
    OUT USIGN8 * *   pp_lim_notify,
    OUT USIGN32 * *  pp_confirm_time,
    OUT USIGN16 * *  pp_feature_sel
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by FB shell in the device startup phase.

  It provides pointers to some resource block parameters.

PARAMETERS
  pp_max_notify     Pointer to pointer to the parameter MAX_NOTIFY
  pp_lim_notify     Pointer to pointer to the parameter LIM_NOTIFY
  pp_confirm_time   Pointer to pointer to the parameter CONFIRM_TIME
  pp_feature_sel    Pointer to pointer to the parameter FEATURE_SEL

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _ASSERT (p_resource != NULL);

  *pp_max_notify   = &p_resource->max_notify;
  *pp_lim_notify   = &p_resource->lim_notify;
  *pp_confirm_time = &p_resource->confirm_time;
  *pp_feature_sel  = &p_resource->feature_sel;

  return;
}






FUNCTION GLOBAL USIGN16 ffbl_check_write_lock
  (
    IN USIGN16 index
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by FB shell when a parameter has to be written.

PARAMETERS
  index             Index of the parameter to be written

RETURN_VALUES
  E_OK              Parameter is writeable
  E_FB_WRITE_LOCK   Parameter is not writeable
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (p_resource->write_lock == WRITE_LOCKED)
  {
    if (p_resource->feature_sel & RES_FEAT_HARD_WRITE_LOCK_SUPP)
    {
      return (E_FB_WRITE_LOCK);
    }

    if (index != ffbl_res_write_lock_index) return (E_FB_WRITE_LOCK);
  }

  return (E_OK);
}





FUNCTION GLOBAL VOID ffbl_generate_RESB_alarm
  (
    IN T_ALARM_ACTION   alarm,
    IN T_RESOURCE_ERROR error
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This function allows to set/clear an error bit in the BLOCK_ERR parameter of
  the resource block. If necessary a block alarm message is sent to the host
  system.

PARAMETERS:
  alarm               Set or clear the alarm
  error               Error code

RETURN_VALUES:
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16 blk_error;

FUNCTION_BODY

  switch (error)
  {
    case res_err_static_data_lost:
      blk_error    = BLK_ERR_LOST_STATIC_DATA;
      break;

    case res_err_dev_needs_maint_soon:
      blk_error    = BLK_ERR_DEV_NEEDS_MAINT_SOON;
      break;

    case res_err_dev_needs_maint_now:
      blk_error    = BLK_ERR_DEV_NEEDS_MAINT_NOW;
      break;

    case res_err_other:
      blk_error    = BLK_ERR_OTHER;
      break;

    default:
      blk_error    = 0;
  }

    /* Set/Clear block error bit -------------------------------------------- */
    osif_disable_all_tasks(); //read/modify/write is a non-atomic operation
    if (alarm == ACTIVATE_BLOCK_ALARM)
    {
        p_resource->block_err |= blk_error;
    }
    else if (alarm == CLEAR_BLOCK_ALARM)
    {
        p_resource->block_err &= ~blk_error;
    }
    osif_enable_all_tasks();

  return;
}








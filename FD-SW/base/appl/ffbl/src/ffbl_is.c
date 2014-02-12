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

FILE_NAME          ffbl_is.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKIS)

INCLUDES
  #include <float.h>

  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbap.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_is.h"
  #include "ffbl_res.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID Check_config_ISFB (T_ISFB *);
  FUNCTION LOCAL USIGN8 calc_input_states (T_ISFB *, USIGN8 *);
  FUNCTION LOCAL BOOL is_op_selected (T_ISFB *, USIGN8 *);
  FUNCTION LOCAL VOID select_input (T_ISFB *, USIGN8 *, USIGN8);
  FUNCTION LOCAL VOID set_good_nc_substatus (T_ISFB *);

IMPORT_DATA

EXPORT_DATA

LOCAL_DATA





FUNCTION GLOBAL VOID Start_ISFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_ISFB *        p_isfb;
  T_ISFB_DATA *   p_isfb_data;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_ISFB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_isfb      = p_block_instance->p_block_desc->p_block;
  p_isfb_data = p_block_instance->p_block_data;

  p_isfb_data->old_block_alarm_bits = 0;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_isfb->update_evt);

  /* Store out param in the internal block-specific data structure -------- */
  p_isfb_data->out    = p_isfb->out;

  /* Set out status to BAD out-of-service --------------------------------- */
  p_isfb->out.status  = SQ_BAD|SUB_OUT_OF_SERVICE;

  /* Check function block configuration ----------------------------------- */
  Check_config_ISFB (p_isfb);

  return;
}





FUNCTION GLOBAL VOID Stop_ISFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_ISFB *                    p_isfb;
  T_ISFB_DATA *               p_isfb_data;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_isfb        = p_block_desc->p_block;
  p_isfb_data   = p_block_instance->p_block_data;

  if (p_isfb->mode_blk.actual == MODE_OS) return;

  p_isfb->mode_blk.actual = MODE_OS;

  /* Set block error ------------------------------------------------------ */
  p_isfb->block_err |= BLK_ERR_OUT_OF_SERVICE;

  /* Store out param in the internal block-specific data structure -------- */
  p_isfb_data->out = p_isfb->out;

  /* Set status of output parameters -------------------------------------- */
  p_isfb->out.status      = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_isfb->selected.status = SQ_BAD|SUB_OUT_OF_SERVICE;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_isfb->update_evt);

  /* Update block alarm ----------------------------------------------- */
  ffbl_update_block_alarm
  (
    p_block_desc->block_id,p_block_desc->idx_blk_alm,
    BLK_ERR_OUT_OF_SERVICE,&p_isfb_data->old_block_alarm_bits
  );

  return;
}





FUNCTION GLOBAL USIGN16 Read_handler_ISFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBIF_READ_DATA *          p_read
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* Check and/or update parameter to be read ----------------------------- */

  _UNREF_PARAM (p_block_instance);
  _UNREF_PARAM (p_read);

  return (E_OK);
}






FUNCTION GLOBAL USIGN16 Write_handler_ISFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBIF_WRITE_DATA *         p_write
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_ISFB *                    p_isfb;
  T_ISFB_DATA *               p_isfb_data;
  USIGN8                      subindex;
  USIGN8 *                    source;
  USIGN8                      result;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_isfb_data   = p_block_instance->p_block_data;
  p_isfb        = p_block_desc->p_block;

  subindex      = p_write->subindex;
  source        = p_write->source;
  result        = E_OK;

  switch (p_write->rel_idx)
  {
    case IS_PAR_ALERT_KEY:
    {
      if (*source == 0) result = E_FB_PARA_CHECK;

      break;
    }

    case IS_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (
                 &p_isfb->mode_blk,
                 subindex,
                 source,
                 MODE_IS_VALID
               );
      break;
    }

    case IS_PAR_OUT:
    {
      if (MODE_LOWER_THAN_MAN (p_isfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      if ((subindex != 0) && (subindex != 2))
      {
        return (E_FB_DATA_NO_WRITE);
      }

      break;
    }

    case IS_PAR_GRANT_DENY:
    {
      ffbl_bas_write_grant_deny (subindex,source,&p_isfb->grant_deny);

      break;
    }

    case IS_PAR_STATUS_OPTS:
    {
      if (MODE_NOT_OS (p_isfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      *((USIGN16 *)source) &= IS_STATUS_OPTS;  /* Delete all undefined bits */

      break;
    }

    case IS_PAR_SELECT_TYPE:
    {
      if ((*source < IS_SEL_TYPE_FIRST_GOOD)||(*source > IS_SEL_TYPE_AVERAGE))
        return (E_FB_PARA_CHECK);

      break;
    }

    case IS_PAR_DISABLE_1:
    case IS_PAR_DISABLE_2:
    case IS_PAR_DISABLE_3:
    case IS_PAR_DISABLE_4:
    {
      USIGN8 *   p_value;

      if      (subindex == 0) p_value = (USIGN8 *)(source + 1);
      else if (subindex == 2) p_value = (USIGN8 *) source;
      else /* (subindex == 1) */
      {
        break;
      }

      if ((*p_value != IS_USE_IN)&&(*p_value != IS_DISABLE_IN))
        return (E_FB_PARA_CHECK);

      break;
    }

    case IS_PAR_MIN_GOOD:
    {
      if (*source > 4)
        return (E_FB_PARA_CHECK);

      break;
    }

    case IS_PAR_OP_SELECT:
    {
      USIGN8 *   p_value;

      if      (subindex == 0) p_value = (USIGN8 *)(source + 1);
      else if (subindex == 2) p_value = (USIGN8 *) source;
      else /* (subindex == 1) */
      {
        break;
      }

      if (*p_value > 4)
        return (E_FB_PARA_CHECK);

      break;
    }

    case IS_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_isfb->update_evt.unack, source, subindex);

      break;
    }

    case IS_PAR_BLOCK_ALM:
    {
      result = ffbl_check_unack_flag (&p_isfb->block_alm.unack, source, subindex);

      break;
    }
  }

  if (result == E_OK)
  {
    result = fbs_write_param (p_block_instance,p_write);
  }

  if (result == E_OK)
  {
    switch (p_write->rel_idx)
    {
      case IS_PAR_MODE_BLK:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        write_loc.rel_idx = 0;

        if (p_isfb->mode_blk.target & MODE_OS)
        {
          p_isfb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;

          write_loc.rel_idx = IS_PAR_OUT;
        }

        else if (p_isfb->mode_blk.target & MODE_MAN)
        {
          write_loc.rel_idx = IS_PAR_OUT;
        }

        if (write_loc.rel_idx)
        {
          /* Store out parameter in non-volatile RAM ---------------------- */
          write_loc.subindex      = 0;
          write_loc.length        = 5;
          write_loc.source        = (USIGN8 *)&p_isfb->out;
          write_loc.startup_sync  = FALSE;

          fbs_write_param_loc (p_block_instance,&write_loc);
        }

        break;
      }

      case IS_PAR_OUT:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        p_isfb->out.status = SQ_GOOD_NC;

        /* Store GOOD out status in non-volatile RAM ---------------------- */
        write_loc.rel_idx       = IS_PAR_OUT;
        write_loc.subindex      = 0;
        write_loc.length        = 5;
        write_loc.source        = (USIGN8 *)&p_isfb->out;
        write_loc.startup_sync  = FALSE;

        fbs_write_param_loc (p_block_instance,&write_loc);

        /* Store out param in the internal block-specific data structure -- */
        p_isfb_data->out = p_isfb->out;

        /* Adapt out status in accordance to the actual mode -------------- */
        if (p_isfb->mode_blk.actual == MODE_OS)
        {
          p_isfb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;
        }
        else
        {
          p_isfb->out.status |= LIMIT_CONST;
        }

        break;
      }

      case IS_PAR_BLK_DATA:
      case IS_PAR_SELECT_TYPE:
      {
        Check_config_ISFB (p_isfb);

        break;
      }
    }
  }

  return (result);
}







FUNCTION GLOBAL USIGN16 Execute_ISFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Return  next_fb_to_execute  next function block to be executed (used if
                            'completion of block exec.' is set in CYCLE_SEL)

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *   p_block_desc;
  T_ISFB *                     p_isfb;
  T_ISFB_DATA *                p_isfb_data;
  USIGN8                       old_mode;
  USIGN8                       actual_mode;
  USIGN8                       no_of_good_inputs;
  USIGN8                       input_status[4];
  BOOL                         enable_update_evt;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_isfb        = p_block_desc->p_block;
  p_isfb_data   = p_block_instance->p_block_data;

  /* Update block mode -------------------------------------------------- */
  old_mode    = p_isfb->mode_blk.actual;
  actual_mode = ffbl_bas_calc_block_mode (&p_isfb->mode_blk,p_isfb->block_err);

  if (actual_mode == MODE_OS)
  {
    if (old_mode != MODE_OS)
    {
      /* Mode OS is entered now ------------------------------------------- */
      Stop_ISFB (p_block_instance);
    }
    else
    {
      /* Update block alarm ----------------------------------------------- */
      ffbl_update_block_alarm
      (
        p_block_desc->block_id,p_block_desc->idx_blk_alm,
        BLK_ERR_OUT_OF_SERVICE,&p_isfb_data->old_block_alarm_bits
      );
    }

    return (p_isfb->blk_data.next_fb_to_execute);
  }

  p_isfb->mode_blk.actual = actual_mode;

  if (old_mode == MODE_OS) /* Mode OS is left now ------------------------- */
  {
    /* Reset block error -------------------------------------------------- */
    p_isfb->block_err &= ~BLK_ERR_OUT_OF_SERVICE;
    enable_update_evt  = TRUE;

    /* Restore out parameter ---------------------------------------------- */
    p_isfb->out = p_isfb_data->out;

    if ((p_isfb->out.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_BAD|SUB_OUT_OF_SERVICE))
    {
      p_isfb->out.status = SQ_BAD;
    }
  }
  else
  {
    enable_update_evt = FALSE;
  }

  if (actual_mode == MODE_MAN)
  {
    if (p_isfb->status_opts & STATUS_OPT_UNCERT_IF_MAN)
    {
      if ((p_isfb->out.status & QUALITY_MASK) & SQ_GOOD_NC)
      {
        p_isfb->out.status = SQ_UNCERTAIN | SUB_NON_SPECIFIC;
      }
    }

    if ((p_isfb->out.status & QUALITY_MASK) != SQ_BAD)
    {
      p_isfb->out.status |= LIMIT_CONST;
    }

    if ((p_isfb->selected.status & QUALITY_MASK) != SQ_BAD)
    {
      p_isfb->selected.status |= LIMIT_CONST;
    }

    goto endlbl;
  }


  /* Mode is AUTO; select input(s) ---------------------------------------- */
  /* ---------------------------------------------------------------------- */
  no_of_good_inputs = calc_input_states (p_isfb,input_status);

  if (! is_op_selected (p_isfb,input_status))
  {
    if (no_of_good_inputs && (no_of_good_inputs >= p_isfb->min_good))
    {
      select_input (p_isfb,input_status,no_of_good_inputs);
    }
    else /* no_of_good_inputs < p_isfb->min_good */
    {
      p_isfb->out.value  = 0.0;
      p_isfb->out.status = SQ_BAD;

      p_isfb->selected.value  = 0;
      p_isfb->selected.status = SQ_GOOD_NC;
    }
  }

  endlbl:

  set_good_nc_substatus (p_isfb);

  /* Update block alarm --------------------------------------------------- */
  ffbl_update_block_alarm
  (
    p_block_desc->block_id,p_block_desc->idx_blk_alm,
    p_isfb->block_err,&p_isfb_data->old_block_alarm_bits
  );

  if (enable_update_evt)
  {
    ffbl_enable_update_events
    (
      p_block_desc->block_id,
      p_block_desc->idx_upd_evt,
      &p_isfb->update_evt,
      p_isfb->st_rev
    );
  }

  return(p_isfb->blk_data.next_fb_to_execute);
}







FUNCTION GLOBAL VOID Background_ISFB
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

FUNCTION_BODY

  _UNREF_PARAM (p_block_instance);

  return;
}







FUNCTION LOCAL VOID Check_config_ISFB
  (
    IN T_ISFB *   p_isfb
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN32   block_err_desc;

FUNCTION_BODY

  block_err_desc = BLK_ERR_NO_ERROR;

  if (p_isfb->blk_data.period_of_execution == 0)
  {
    block_err_desc |= BLK_ERR_EXE_TIME;
  }

  if (!p_isfb->select_type)
  {
    block_err_desc |= BLK_ERR_SELECT_TYPE_UNINIT;
  }

  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_isfb->block_err &= ~BLK_ERR_BLOCK_CONFIG_ERR;
  }
  else
  {
    p_isfb->block_err |= BLK_ERR_BLOCK_CONFIG_ERR;
  }

  p_isfb->block_err_desc_1 = block_err_desc;

  return;
}






FUNCTION LOCAL USIGN8 calc_input_states
  (
    IN  T_ISFB *    p_isfb,
    OUT USIGN8 *    input_status
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  If DISABLE_n is true then don’t process (ignore) the respective input IN_n.

  Process the Use Uncertain as Good status options.
  Discard (ignore) inputs whose status is bad.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  DISCRETE_S *    p_disable;
  FLOAT_S *       p_in;
  USIGN8          i;
  USIGN8          no_of_good_inputs;

FUNCTION_BODY

  p_disable = &p_isfb->disable_1;
  p_in      = &p_isfb->in_1;

  no_of_good_inputs = 0;

  for (i = 0;i < 4;i++)
  {
    input_status[i] = SQ_GOOD_NC;

    if (p_disable->status & SQ_GOOD_NC)
    {
      if (p_disable->value == IS_DISABLE_IN)
      {
        input_status[i] = SQ_BAD;
      }
    }
    else if ((p_disable->status & QUALITY_MASK) == SQ_UNCERTAIN)
    {
      if (p_isfb->status_opts & STATUS_OPT_USE_UNCERT_AS_GOOD)
      {
        if (p_disable->value == IS_DISABLE_IN)
        {
          input_status[i] = SQ_BAD;
        }
      }
    }

    if ((p_in->status & QUALITY_MASK) == SQ_BAD)
    {
      input_status[i] = SQ_BAD;
    }
    else if ((p_in->status & QUALITY_MASK) == SQ_UNCERTAIN)
    {
      if (!(p_isfb->status_opts & STATUS_OPT_USE_UNCERT_AS_GOOD))
      {
        input_status[i] = SQ_BAD;
      }
    }

    if (input_status[i] == SQ_GOOD_NC)
    {
      no_of_good_inputs++;
    }

    p_disable++;
    p_in++;
  }

  return (no_of_good_inputs);
}





FUNCTION LOCAL BOOL is_op_selected
  (
    IN T_ISFB *    p_isfb,
    IN USIGN8 *    input_status
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (p_isfb->op_select.status & SQ_GOOD_NC)
  {
    if ((p_isfb->op_select.value > 0) && (p_isfb->op_select.value < 5))
    {
      if (input_status[p_isfb->op_select.value-1] == SQ_GOOD_NC)
      {
        FLOAT_S *   p_in = &p_isfb->in_1 + p_isfb->op_select.value - 1;

        p_isfb->out.value       = p_in->value;
        p_isfb->out.status      = SQ_GOOD_NC;
        p_isfb->selected.value  = p_isfb->op_select.value;
        p_isfb->selected.status = SQ_GOOD_NC;
      }
      else
      {
        p_isfb->out.value       = 0.0;
        p_isfb->out.status      = SQ_BAD;
        p_isfb->selected.value  = 0;
        p_isfb->selected.status = SQ_GOOD_NC;
      }

      return (TRUE);
    }
  }

  return (FALSE);
}





FUNCTION LOCAL VOID select_input
  (
    IN T_ISFB *    p_isfb,
    IN USIGN8 *    input_status,
    IN USIGN8      no_of_good_inputs
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16     i;
  FLOAT_S *   p_in;

FUNCTION_BODY

  p_isfb->out.value       = 0.0;
  p_isfb->out.status      = SQ_GOOD_NC;
  p_isfb->selected.status = SQ_GOOD_NC;

  p_in = &p_isfb->in_1;

  switch (p_isfb->select_type)
  {
    case IS_SEL_TYPE_FIRST_GOOD:
    {
      for (i = 0; i < 4;i++)
      {
        if (input_status[i] == SQ_GOOD_NC)
        {
          p_isfb->selected.value = i+1;
          p_isfb->out.value      = p_in->value;

          break;
        }

        p_in++;
      }

      break;
    }

    case IS_SEL_TYPE_MINIMUM:
    {
      p_isfb->out.value = FLT_MAX;

      for (i = 0; i < 4;i++)
      {
        if (input_status[i] == SQ_GOOD_NC)
        {
          if (p_in->value < p_isfb->out.value)
          {
            p_isfb->selected.value = i+1;
            p_isfb->out.value      = p_in->value;
          }
        }

        p_in++;
      }

      break;
    }

    case IS_SEL_TYPE_MAXIMUM:
    {
      p_isfb->out.value = -FLT_MAX;

      for (i = 0; i < 4;i++)
      {
        if (input_status[i] == SQ_GOOD_NC)
        {
          if (p_in->value > p_isfb->out.value)
          {
            p_isfb->selected.value = i+1;
            p_isfb->out.value      = p_in->value;
          }
        }

        p_in++;
      }

      break;
    }

    case IS_SEL_TYPE_MIDDLE:
    {
      if (no_of_good_inputs > 2)
      {
        USIGN8 idx_of_discarded;
        FLOAT  val_of_discarded;

        /* Discard the highest value -------------------------------------- */
        val_of_discarded = FLT_MAX;
        idx_of_discarded = 0;

        for (i = 0; i < 4;i++)
        {
          if (input_status[i] == SQ_GOOD_NC)
          {
            if (p_in->value < val_of_discarded)
            {
              idx_of_discarded  = i;
              val_of_discarded  = p_in->value;
            }
          }

          p_in++;
        }

        input_status[idx_of_discarded] = SQ_BAD;
        no_of_good_inputs--;

        p_in = &p_isfb->in_1; /* Reset pointer to first input */

        /* Discard the highest value -------------------------------------- */
        val_of_discarded = -FLT_MAX;

        for (i = 0; i < 4;i++)
        {
          if (input_status[i] == SQ_GOOD_NC)
          {
            if (p_in->value > val_of_discarded)
            {
              idx_of_discarded  = i;
              val_of_discarded  = p_in->value;
            }
          }

          p_in++;
        }

        input_status[idx_of_discarded] = SQ_BAD;
        no_of_good_inputs--;
      }

      p_in = &p_isfb->in_1; /* Reset pointer to first input */

      if (no_of_good_inputs == 2)
      {
        /* Calculate average ---------------------------------------------- */
        for (i = 0; i < 4;i++)
        {
          if (input_status[i] == SQ_GOOD_NC)
          {
            p_isfb->out.value += p_in->value;
          }

          p_in++;
        }

        p_isfb->out.value /= 2;

        p_isfb->selected.value = 0;
      }

      else /* no_of_good_inputs == 1 */
      {
        for (i = 0; i < 4;i++)
        {
          if (input_status[i] == SQ_GOOD_NC)
          {
            p_isfb->out.value = p_in->value;

            break;
          }

          p_in++;
        }

        p_isfb->selected.value = i+1;
      }

      break;
    }

    case IS_SEL_TYPE_AVERAGE:
    {
      p_isfb->out.value = 0;

      for (i = 0; i < 4;i++)
      {
        if (input_status[i] == SQ_GOOD_NC)
        {
          p_isfb->out.value += p_in->value;
        }

        p_in++;
      }

      p_isfb->out.value      /= no_of_good_inputs;
      p_isfb->selected.value  = no_of_good_inputs;

      break;
    }

    default:
    {
      p_isfb->out.value  = 0.0;
      p_isfb->out.status = (SQ_BAD & LIMIT_CONST);

      p_isfb->selected.value  = 0;
      p_isfb->selected.status = (SQ_BAD & LIMIT_CONST);
    }
  }

  return;
}







FUNCTION LOCAL VOID set_good_nc_substatus
  (
    IN T_ISFB *   p_isfb
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
#ifdef NEVER /* because of i3300_b0 */
  USIGN8  good_nc_status;

FUNCTION_BODY

  good_nc_status = ffbl_bas_determine_good_nc_status (&p_isfb->block_alm);

  if (p_isfb->out.status & SQ_GOOD_NC)
  {
    p_isfb->out.status &= ~SUB_STATUS_MASK;
    p_isfb->out.status |= good_nc_status;
  }

  if (p_isfb->selected.status & SQ_GOOD_NC)
  {
    p_isfb->selected.status &= ~SUB_STATUS_MASK;
    p_isfb->selected.status |= good_nc_status;
  }
#else
  p_isfb          = p_isfb;
#endif

  return;
}








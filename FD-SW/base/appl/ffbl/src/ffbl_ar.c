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

FILE_NAME          ffbl_ar.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKAR)

INCLUDES
  #include <math.h>

  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbap.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_ar.h"

LOCAL_DEFINES
  #define EULER   2.718281828459
//#define FLOAT_TYPE FLOAT
  #define FLOAT_TYPE DOUBLE

LOCAL_TYPEDEFS
  typedef struct
  {
    USIGN8    good_inputs;
    USIGN8    uncertain_inputs;
    USIGN8    bad_inputs;
  }
    T_AR_INPUT_STATUS;


FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID Check_config_ARFB (T_ARFB *);
  FUNCTION LOCAL VOID determine_in_states (T_ARFB *, T_AR_INPUT_STATUS *);
  FUNCTION LOCAL VOID determine_in_status (USIGN8, USIGN16, USIGN16, USIGN16, USIGN8, T_AR_INPUT_STATUS *);
  FUNCTION LOCAL VOID calc_pv_status_and_value (T_ARFB *, T_AR_INPUT_STATUS *);
  FUNCTION LOCAL VOID exec_arithm_funct (T_ARFB *, FLOAT_TYPE, FLOAT_TYPE, FLOAT_TYPE, T_AR_INPUT_STATUS *);
  FUNCTION LOCAL USIGN8 determine_pre_out_status (T_AR_INPUT_STATUS * , USIGN8);
  FUNCTION LOCAL VOID update_out_diff (T_ARFB *, T_ARFB_DATA *);
  FUNCTION LOCAL VOID update_out (T_ARFB *, T_ARFB_DATA *);
  FUNCTION LOCAL VOID set_good_nc_substatus (T_ARFB *);

IMPORT_DATA

EXPORT_DATA

LOCAL_DATA
#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT T_AR_INPUT_STATUS  input_states;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */





FUNCTION GLOBAL VOID Start_ARFB
  (
     IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_ARFB *        p_arfb;
  T_ARFB_DATA *   p_arfb_data;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_ARFB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_arfb      = p_block_instance->p_block_desc->p_block;
  p_arfb_data = p_block_instance->p_block_data;

  p_arfb_data->old_block_alarm_bits = 0;
  p_arfb_data->period_of_exec = p_arfb->blk_data.period_of_execution / 32000.0;

  /* Store out param in the internal block-specific data structure -------- */
  p_arfb_data->out    = p_arfb->out;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_arfb->update_evt);

  /* Check function block configuration ----------------------------------- */
  Check_config_ARFB (p_arfb);

  return;
}





FUNCTION GLOBAL VOID Stop_ARFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_ARFB *                    p_arfb;
  T_ARFB_DATA *               p_arfb_data;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_arfb        = p_block_desc->p_block;
  p_arfb_data   = p_block_instance->p_block_data;

  if(p_arfb->mode_blk.actual == MODE_OS) return;

  p_arfb->mode_blk.actual = MODE_OS;

  /* Set block error ------------------------------------------------------ */
  p_arfb->block_err |= BLK_ERR_OUT_OF_SERVICE;

  /* Store out param in the internal block-specific data structure -------- */
  p_arfb_data->out = p_arfb->out;

  /* Set parameter status to Out-of-Service ------------------------------- */
  p_arfb->out.status     = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_arfb->pre_out.status = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_arfb->pv.status      = SQ_BAD|SUB_OUT_OF_SERVICE;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_arfb->update_evt);

  /* Update block alarm --------------------------------------------------- */
  ffbl_update_block_alarm
  (
    p_block_desc->block_id,p_block_desc->idx_blk_alm,
    BLK_ERR_OUT_OF_SERVICE,&p_arfb_data->old_block_alarm_bits
  );

  return;
}





FUNCTION PUBLIC  USIGN16 Read_handler_ARFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBIF_READ_DATA *          p_read
  )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Return  E_OK    or a neg. return code defined in fbs_ds.h

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _UNREF_PARAM (p_block_instance);
  _UNREF_PARAM (p_read);

  return (E_OK);
}





FUNCTION PUBLIC  USIGN16 Write_handler_ARFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBIF_WRITE_DATA *         p_write
  )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Return  E_OK    or a neg. return code defined in fbs_ds.h
----------------------------------------------------------------------------*/
{
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_ARFB *                    p_arfb;
  T_ARFB_DATA *               p_arfb_data;
  USIGN8                      subindex;
  USIGN8 *                    source;
  USIGN8                      result;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_arfb_data   = p_block_instance->p_block_data;
  p_arfb        = p_block_desc->p_block;

  subindex      = p_write->subindex;
  source        = p_write->source;

  result        = E_OK;

  switch (p_write->rel_idx)
  {
    case AR_PAR_ALERT_KEY:
    {
      if (*source == 0) result = E_FB_PARA_CHECK;

      break;
    }

    case AR_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (
                 &p_arfb->mode_blk,
                 subindex,source,
                 MODE_AR_VALID
               );
      break;
    }

    case AR_PAR_OUT:
    {
      if (MODE_LOWER_THAN_MAN (p_arfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      if (subindex == 1)
          return (E_FB_DATA_NO_WRITE);

      break;
    }

    case AR_PAR_PV_SCALE:
    {
      if (MODE_NOT_OS (p_arfb->mode_blk.target)) /* Check because of i3010_55 */
        return (E_FB_WRONG_MODE);

      break;
    }

    case AR_PAR_GRANT_DENY:
    {
      ffbl_bas_write_grant_deny (subindex,source,&p_arfb->grant_deny);

      break;
    }

    case AR_PAR_INPUT_OPTS:
    {
      *((USIGN16 *)source) &= AR_INPUT_OPTS; /* Delete all undefined bits */

      break;
    }

    case AR_PAR_BAL_TIME:
    {
      if (*((FLOAT *)source) < 0.0)
          return (E_FB_PARA_CHECK);

      break;
    }


    case AR_PAR_ARITH_TYPE:
    { /* 3010_55 */
      if ((*source < FLOW_COMP_LINEAR)||(*source > FOURTH_ORDER_POLY_2))
          return (E_FB_PARA_CHECK);

      break;
    }

    case AR_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_arfb->update_evt.unack, source, subindex);

      break;
    }

    case AR_PAR_BLOCK_ALM:
    {
      result = ffbl_check_unack_flag (&p_arfb->block_alm.unack, source, subindex);
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
      case AR_PAR_MODE_BLK:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        write_loc.rel_idx = 0;

        if (p_arfb->mode_blk.target & MODE_OS)
        {
          p_arfb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;

          write_loc.rel_idx = AR_PAR_OUT;
        }

        else if (p_arfb->mode_blk.target & MODE_MAN)
        {
          write_loc.rel_idx = AR_PAR_OUT;
        }

        if (write_loc.rel_idx)
        {
          /* Store out parameter in non-volatile RAM ---------------------- */
          write_loc.subindex      = 0;
          write_loc.length        = 5;
          write_loc.source        = (USIGN8 *)&p_arfb->out;
          write_loc.startup_sync  = FALSE;

          fbs_write_param_loc (p_block_instance,&write_loc);
        }

        break;
      }

      case AR_PAR_OUT:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        p_arfb->out.status = SQ_GOOD_NC;

        /* Store GOOD out status in non-volatile RAM ---------------------- */
        write_loc.rel_idx       = AR_PAR_OUT;
        write_loc.subindex      = 0;
        write_loc.length        = 5;
        write_loc.source        = (USIGN8 *)&p_arfb->out;
        write_loc.startup_sync  = FALSE;

        fbs_write_param_loc (p_block_instance,&write_loc);

        /* Store out param in the internal block-specific data structure -- */
        p_arfb_data->out = p_arfb->out;

        /* Adapt out status in accordance to the actual mode -------------- */
        if (p_arfb->mode_blk.actual == MODE_OS)
        {
          p_arfb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;
        }
        else
        {
          p_arfb->out.status |= LIMIT_CONST;
        }

        break;
      }

      case AR_PAR_BLK_DATA:
      {
        /* Convert period of execution from 1/32 ms to sec ------------------ */
        p_arfb_data->period_of_exec = p_arfb->blk_data.period_of_execution / 32000.0;

        /* Do not break ! */
      }

      case AR_PAR_ARITH_TYPE:
      case AR_PAR_RANGE_HI:
      case AR_PAR_RANGE_LO:
      case AR_PAR_COMP_HI_LIM:
      case AR_PAR_COMP_LO_LIM:
      case AR_PAR_OUT_HI_LIM:
      case AR_PAR_OUT_LO_LIM:
      {
        Check_config_ARFB (p_arfb);

        break;
      }
    }
  }

  return result;
}






FUNCTION PUBLIC USIGN16 Execute_ARFB
  (
     IN T_FBIF_BLOCK_INSTANCE *     p_block_instance
  )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *    p_block_desc;
  T_ARFB *                      p_arfb;
  T_ARFB_DATA *                 p_arfb_data;
  USIGN8                        actual_mode;
  USIGN8                        old_mode;
  FLOAT_TYPE                    t1,t2,t3;
  BOOL                          enable_update_evt;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_arfb        = p_block_desc->p_block;
  p_arfb_data   = p_block_instance->p_block_data;

  /* Update block mode -------------------------------------------------- */
  old_mode    = p_arfb->mode_blk.actual;
  actual_mode = ffbl_bas_calc_block_mode (&p_arfb->mode_blk,p_arfb->block_err);

  if (actual_mode == MODE_OS)
  {
    if (old_mode != MODE_OS)
    {
      /* Mode OS is entered now ------------------------------------------- */
      Stop_ARFB (p_block_instance);
    }
    else
    {
      /* Update block alarm ----------------------------------------------- */
      ffbl_update_block_alarm
      (
        p_block_desc->block_id,p_block_desc->idx_blk_alm,
        BLK_ERR_OUT_OF_SERVICE,&p_arfb_data->old_block_alarm_bits
      );
    }

    set_good_nc_substatus (p_arfb);

    return (p_arfb->blk_data.next_fb_to_execute);
  }

  p_arfb->mode_blk.actual = actual_mode;

  if (old_mode == MODE_OS) /* Mode OS is left now ------------------------- */
  {
    /* Reset block error -------------------------------------------------- */
    p_arfb->block_err &= ~BLK_ERR_OUT_OF_SERVICE;
    enable_update_evt  = TRUE;

    /* Restore out parameter ---------------------------------------------- */
    p_arfb->out = p_arfb_data->out;

    if ((p_arfb->out.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_BAD|SUB_OUT_OF_SERVICE))
    {
      p_arfb->out.status = SQ_BAD;
    }
  }
  else
  {
    enable_update_evt = FALSE;
  }


  /* Determine status of inputs in dependance of INPUT_OPTS --------------- */
  input_states.good_inputs      = 0;
  input_states.uncertain_inputs = 0;
  input_states.bad_inputs       = 0;

  determine_in_states (p_arfb,&input_states);


  /* Calculate status and value of PV ------------------------------------- */
  calc_pv_status_and_value (p_arfb,&input_states);


  /* Calculate term_1, term_2 and term_3 ---------------------------------- */
  t1 = (p_arfb->in_1.value + p_arfb->bias_in_1) * p_arfb->gain_in_1;
  t2 = (p_arfb->in_2.value + p_arfb->bias_in_2) * p_arfb->gain_in_2;
  t3 = (p_arfb->in_3.value + p_arfb->bias_in_3) * p_arfb->gain_in_3;


  /* Execute selected arithmetic function and update PRE_OUT -------------- */
  exec_arithm_funct (p_arfb, t1, t2, t3, &input_states);


  if (p_arfb->pre_out.status != SQ_BAD)
  {
    /* Apply bias, gain to PRE_OUT value  --------------------------------- */
    p_arfb->pre_out.value = p_arfb->pre_out.value * p_arfb->gain + p_arfb->bias;


    /* Apply limits to PRE_OUT value  ------------------------------------- */
    if (p_arfb->pre_out.value > p_arfb->out_hi_lim)
    {
      p_arfb->pre_out.value   = p_arfb->out_hi_lim;
      p_arfb->pre_out.status |= LIMIT_HIGH;
    }
    else if (p_arfb->pre_out.value < p_arfb->out_lo_lim)
    {
      p_arfb->pre_out.value   = p_arfb->out_lo_lim;
      p_arfb->pre_out.status |= LIMIT_LOW;
    }
  }


  if (actual_mode == MODE_AUTO)
  {
    update_out (p_arfb,p_arfb_data);
  }
  else /* actual_mode == MODE_MAN */
  {
    update_out_diff (p_arfb,p_arfb_data);

    if ((p_arfb->out.status & QUALITY_MASK) != SQ_BAD)
    {
      p_arfb->out.status |= LIMIT_CONST;
    }
  }

  set_good_nc_substatus (p_arfb);

  /* Update block alarm --------------------------------------------------- */
  ffbl_update_block_alarm
  (
    p_block_desc->block_id,p_block_desc->idx_blk_alm,
    p_arfb->block_err,&p_arfb_data->old_block_alarm_bits
  );

  if (enable_update_evt)
  {
    ffbl_enable_update_events
    (
      p_block_desc->block_id,
      p_block_desc->idx_upd_evt,
      &p_arfb->update_evt,
      p_arfb->st_rev
    );
  }

  return (p_arfb->blk_data.next_fb_to_execute);
}







FUNCTION GLOBAL VOID Background_ARFB
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







FUNCTION LOCAL VOID determine_in_states
  (
    IN  T_ARFB *              p_arfb,
    OUT T_AR_INPUT_STATUS *   p_status
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  determine_in_status (
     p_arfb->in.status,
     p_arfb->input_opts,
     IN_USE_UNCERTAIN,
     0,
     AR_IN,
     p_status
   );

  determine_in_status (
     p_arfb->in_lo.status,
     p_arfb->input_opts,
     IN_LO_USE_UNCERTAIN,
     0,
     AR_IN_LO,
     p_status
   );

  determine_in_status (
     p_arfb->in_1.status,
     p_arfb->input_opts,
     IN_1_USE_UNCERTAIN,
     IN_1_USE_BAD,
     AR_IN_1,
     p_status
   );

  determine_in_status (
     p_arfb->in_2.status,
     p_arfb->input_opts,
     IN_2_USE_UNCERTAIN,
     IN_2_USE_BAD,
     AR_IN_2,
     p_status
   );

  determine_in_status (
     p_arfb->in_3.status,
     p_arfb->input_opts,
     IN_3_USE_UNCERTAIN,
     IN_3_USE_BAD,
     AR_IN_3,
     p_status
   );

  return;
}






FUNCTION LOCAL VOID determine_in_status
  (
    IN  USIGN8                in_status,
    IN  USIGN16               input_opts,
    IN  USIGN16               uncert_option,
    IN  USIGN16               bad_option,
    IN  USIGN8                input_bit,
    INOUT T_AR_INPUT_STATUS * p_status
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  in_status &= QUALITY_MASK;

  if (in_status & SQ_GOOD)
  {
    p_status->good_inputs |= input_bit;
  }
  else if (in_status == SQ_UNCERTAIN)
  {
    if (input_opts & uncert_option)
    {
      p_status->good_inputs |= input_bit;
    }
    else
    {
      p_status->uncertain_inputs |= input_bit;
    }
  }
  else /* in_status is BAD */
  {
    if (input_opts & bad_option)
    {
      p_status->good_inputs |= input_bit;
    }
    else
    {
      p_status->bad_inputs |= input_bit;
    }
  }

  return;
}





FUNCTION LOCAL VOID calc_pv_status_and_value
  (
    IN  T_ARFB *              p_arfb,
    OUT T_AR_INPUT_STATUS *   p_status
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (p_status->bad_inputs & AR_IN_LO)
  {
    if (p_status->good_inputs & AR_IN)
    {
      p_status->good_inputs |= AR_PV;

      p_arfb->pv.status = SQ_GOOD_NC;
    }
    else if (p_status->uncertain_inputs & AR_IN)
    {
      p_status->uncertain_inputs |= AR_PV;

      p_arfb->pv.status = SQ_UNCERTAIN;
    }
    else
    {
      p_status->bad_inputs |= AR_PV;

      p_arfb->pv.status = SQ_BAD;
    }

    if (p_arfb->pv.status != SQ_BAD)
    {
      if (p_arfb->in.value > p_arfb->range_lo)
      {
        p_arfb->pv.value = p_arfb->in.value;
      }
      else
      {
        p_arfb->pv.value = p_arfb->range_lo;
      }
    }
    else
    {
      p_arfb->pv.value = p_arfb->in.value;
    }
  }

  else if (p_status->bad_inputs & AR_IN)
  {
    if (p_status->good_inputs & AR_IN_LO)
    {
      p_status->good_inputs |= AR_PV;

      p_arfb->pv.status = SQ_GOOD_NC;
    }
    else if (p_status->uncertain_inputs & AR_IN_LO)
    {
      p_status->uncertain_inputs |= AR_PV;

      p_arfb->pv.status = SQ_UNCERTAIN;
    }
    else
    {
      p_status->bad_inputs |= AR_PV;

      p_arfb->pv.status = SQ_BAD;
    }

    if (p_arfb->pv.status != SQ_BAD)
    {
      if (p_arfb->in_lo.value < p_arfb->range_hi)
      {
        p_arfb->pv.value = p_arfb->in_lo.value;
      }
      else
      {
        p_arfb->pv.value = p_arfb->range_hi;
      }
    }
    else
    {
      p_arfb->pv.value = p_arfb->in_lo.value;
    }
  }

  else /* IN and IN_LO are usable are useable */
  {
    /* Calc g(in) value --------------------------------------------------- */
    FLOAT  g;

    if (p_arfb->in.value > p_arfb->range_hi)
    {
      g = 1.0;
    }
    else if (p_arfb->in.value < p_arfb->range_lo)
    {
      g = 0.0;
    }
    else if (p_arfb->range_lo == p_arfb->range_hi)
    {
      g = 0.5;
    }
    else
    {
      g = (p_arfb->in.value - p_arfb->range_lo) / (p_arfb->range_hi - p_arfb->range_lo);
    }


    /* Calculate the PV value --------------------------------------------- */
    p_arfb->pv.value = g * p_arfb->in.value + (1 - g) * p_arfb->in_lo.value;


    /* Set the PV status -------------------------------------------------- */
    if (g >= 0.5)
    {
      p_arfb->pv.status = p_arfb->in.status & QUALITY_MASK;  /* i3300_C2 */
    }
    else
    {
      p_arfb->pv.status = p_arfb->in_lo.status & QUALITY_MASK;  /* i3300_C2 */
    }
  }
}





FUNCTION LOCAL VOID exec_arithm_funct
  (
    IN T_ARFB *             p_arfb,
    IN FLOAT_TYPE           t1,
    IN FLOAT_TYPE           t2,
    IN FLOAT_TYPE           t3,
    IN T_AR_INPUT_STATUS *  p_in_states
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  FLOAT_TYPE  f;

FUNCTION_BODY

  p_arfb->pre_out.value   = 0;
  p_arfb->pre_out.status  = SQ_BAD;

  switch (p_arfb->arith_type)
  {
    case FLOW_COMP_LINEAR:
    {
      if (t2 != 0.0)
      {
        f = t1 / t2;

        if      (f > p_arfb->comp_hi_lim) f = p_arfb->comp_hi_lim;
        else if (f < p_arfb->comp_lo_lim) f = p_arfb->comp_lo_lim;
      }
      else /* t2 == 0.0 */
      {
        if (t1 >= 0.0) f = p_arfb->comp_hi_lim;
        else           f = p_arfb->comp_lo_lim;
      }

      p_arfb->pre_out.value   = (FLOAT)(f * p_arfb->pv.value);
      p_arfb->pre_out.status  = determine_pre_out_status (p_in_states, FLOW_COMP_LINEAR_INPUTS);

      break;
    }


    case FLOW_COMP_SQROOT:
    {
      if ((t2 != 0.0) && (t3 != 0.0))
      {
        f = t1 / t2 / t3;

        if (f >= 0.0) f =  sqrt( f);
        else          f = -sqrt(-f);

        if      (f > p_arfb->comp_hi_lim) f = p_arfb->comp_hi_lim;
        else if (f < p_arfb->comp_lo_lim) f = p_arfb->comp_lo_lim;
      }
      else /* (t2 == 0.0) || (t3 == 0.0) */
      {
        if (t1 >= 0.0) f = p_arfb->comp_hi_lim;
        else           f = p_arfb->comp_lo_lim;
      }

      p_arfb->pre_out.value   = (FLOAT)(f * p_arfb->pv.value);
      p_arfb->pre_out.status  = determine_pre_out_status (p_in_states, FLOW_COMP_SQROOT_INPUTS);

      break;
    }

    case FLOW_COMP_APPROX:
    {
      f = t1 * t2 * t3 * t3;

      if (f >= 0.0) f =  sqrt( f);
      else          f = -sqrt(-f);

      if      (f > p_arfb->comp_hi_lim) f = p_arfb->comp_hi_lim;
      else if (f < p_arfb->comp_lo_lim) f = p_arfb->comp_lo_lim;

      p_arfb->pre_out.value   = (FLOAT)(f * p_arfb->pv.value);
      p_arfb->pre_out.status  = determine_pre_out_status (p_in_states, FLOW_COMP_APPROX_INPUTS);

      break;
    }

    case BTU_FLOW:
    {
      f = t1 - t2;

      if      (f > p_arfb->comp_hi_lim) f = p_arfb->comp_hi_lim;
      else if (f < p_arfb->comp_lo_lim) f = p_arfb->comp_lo_lim;

      p_arfb->pre_out.value   = (FLOAT)(f * p_arfb->pv.value);
      p_arfb->pre_out.status  = determine_pre_out_status (p_in_states, BTU_FLOW_INPUTS);

      break;
    }

    case MULTIPLY_DIVIDE:
    {
      if (t2 != 0.0)
      {
        f = (t1 / t2) + t3;

        if      (f > p_arfb->comp_hi_lim) f = p_arfb->comp_hi_lim;
        else if (f < p_arfb->comp_lo_lim) f = p_arfb->comp_lo_lim;
      }
      else /* t2 == 0.0 */
      {
        if (t1 >= 0.0) f = p_arfb->comp_hi_lim;
        else           f = p_arfb->comp_lo_lim;
      }

      p_arfb->pre_out.value   = (FLOAT)(f * p_arfb->pv.value);
      p_arfb->pre_out.status  = determine_pre_out_status (p_in_states, MULTIPLY_DIVIDE_INPUTS);

      break;
    }

    case AVERAGE:
    {
      USIGN8  ctr = 0;

      p_arfb->pre_out.value   = 0.0;
      p_arfb->pre_out.status  = SQ_GOOD_NC;

      if (!(p_in_states->bad_inputs & AR_PV))
      {
        p_arfb->pre_out.value = p_arfb->pv.value;   ctr++;

        if (p_in_states->uncertain_inputs & AR_PV)
        {
          p_arfb->pre_out.status  = SQ_UNCERTAIN;
        }
      }

      if (!(p_in_states->bad_inputs & AR_IN_1))
      {
        p_arfb->pre_out.value += t1;      ctr++;

        if (p_in_states->uncertain_inputs & AR_IN_1)
        {
          p_arfb->pre_out.status  = SQ_UNCERTAIN;
        }
      }

      if (!(p_in_states->bad_inputs & AR_IN_2))
      {
        p_arfb->pre_out.value += t2;      ctr++;

        if (p_in_states->uncertain_inputs & AR_IN_2)
        {
          p_arfb->pre_out.status  = SQ_UNCERTAIN;
        }
      }

      if (!(p_in_states->bad_inputs & AR_IN_3))
      {
        p_arfb->pre_out.value += t3;      ctr++;

        if (p_in_states->uncertain_inputs & AR_IN_3)
        {
          p_arfb->pre_out.status  = SQ_UNCERTAIN;
        }
      }

      if (ctr)
      {
        p_arfb->pre_out.value /= ctr;
      }
      else
      {
        p_arfb->pre_out.status  = SQ_UNCERTAIN;
      }

      break;
    }

    case SUMMER:
    {
      p_arfb->pre_out.value   = p_arfb->pv.value + t1 + t2 +t3;
      p_arfb->pre_out.status  = determine_pre_out_status (p_in_states, SUMMER_INPUTS);

      break;
    }

    case FOURTH_ORDER_POLY:
    {
      p_arfb->pre_out.value   = p_arfb->pv.value + pow(t1,2.0) + pow(t2,3.0) + pow(t3,4.0);
      p_arfb->pre_out.status  = determine_pre_out_status (p_in_states, FORTH_ORDER_POLYNOM_INPUTS);

      break;
    }

    case SIMPLE_HTG:
    {
      f = p_arfb->pv.value - t1;

      if ((p_arfb->pv.value - t2) != 0.0)
      {
        p_arfb->pre_out.value = f / (p_arfb->pv.value - t2);
      }

      else /* (p_arfb->pv.value - t2) == 0.0 */
      {
        if (f >= 0.0)
        {
          p_arfb->pre_out.value = MAX_FLOAT_POS;
        }
        else
        {
          p_arfb->pre_out.value = -MAX_FLOAT_POS;
        }
      }

      p_arfb->pre_out.status  = determine_pre_out_status (p_in_states, SIMPLE_HTG_INPUTS);

      break;
    }

    case FOURTH_ORDER_POLY_2:
    {
      p_arfb->pre_out.value  = p_arfb->pv.value;
      p_arfb->pre_out.value += p_arfb->gain_in_1 * pow(p_arfb->pv.value,2.0);
      p_arfb->pre_out.value += p_arfb->gain_in_2 * pow(p_arfb->pv.value,3.0);
      p_arfb->pre_out.value += p_arfb->gain_in_3 * pow(p_arfb->pv.value,4.0);

      p_arfb->pre_out.status = determine_pre_out_status (p_in_states, AR_PV);

      break;
    }
  }

  return;
}







FUNCTION LOCAL USIGN8 determine_pre_out_status
  (
    IN T_AR_INPUT_STATUS *  p_in_states,
    IN USIGN8               used_inputs
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (p_in_states->bad_inputs & used_inputs)
  {
    return (SQ_BAD);
  }
  else if (p_in_states->uncertain_inputs & used_inputs)
  {
    return (SQ_UNCERTAIN);
  }
  else
  {
    return (SQ_GOOD_NC);
  }
}



FUNCTION LOCAL VOID update_out_diff
  (
    IN T_ARFB *       p_arfb,
    IN T_ARFB_DATA *  p_arfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  If the mode is Man, an internal value shall be set to the difference
  between OUT and the output of the selected function.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if ((p_arfb->pre_out.status & SQ_GOOD_NC) && (p_arfb->out.status & SQ_GOOD_NC))
  {
    p_arfb_data->out_diff = p_arfb->out.value - p_arfb->pre_out.value;
  }
  else
  {
    p_arfb_data->out_diff = 0.0;
  }

  /* Initialize a counter to meassure the time since Man mode ------------- */
  p_arfb_data->time_since_man = 0.0;

  return;
}




FUNCTION LOCAL VOID update_out
  (
    IN T_ARFB *       p_arfb,
    IN T_ARFB_DATA *  p_arfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  If the mode is Man, an internal value shall be set to the difference
  between OUT and the output of the selected function. When the mode is
  changed to Auto, then this difference value should be added to the
  output of the selected function to calculate the OUT value. The difference
  value used in this calculation shall ramp to zero in a linear manner specified
  by BAL_TIME

  ----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  FLOAT   ramp_factor;

FUNCTION_BODY

  if (p_arfb->bal_time == 0.0f) p_arfb_data->out_diff = 0.0f;

  p_arfb->out = p_arfb->pre_out;

  if ((p_arfb_data->out_diff != 0.0f)&&(p_arfb->pre_out.status != SQ_BAD))
  {
    p_arfb_data->time_since_man += p_arfb_data->period_of_exec;

    if (p_arfb_data->time_since_man <= p_arfb->bal_time)
    {
      /* Apply ramp on out value -------------------------------------------- */
      ramp_factor = p_arfb_data->time_since_man / p_arfb->bal_time;

      p_arfb->out.value = p_arfb->pre_out.value + (1 - ramp_factor) * p_arfb_data->out_diff;
    }
  }

  return;
}






FUNCTION LOCAL VOID set_good_nc_substatus
  (
    IN T_ARFB *   p_arfb
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8  good_nc_status;

FUNCTION_BODY

  good_nc_status = ffbl_bas_determine_good_nc_status (&p_arfb->block_alm);

  if (p_arfb->out.status & SQ_GOOD_NC)
  {
    p_arfb->out.status &= ~SUB_STATUS_MASK;
    p_arfb->out.status |= good_nc_status;
  }

  return;
}







FUNCTION LOCAL VOID Check_config_ARFB
  (
    IN T_ARFB *   p_arfb
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN32   block_err_desc;

FUNCTION_BODY

  block_err_desc = BLK_ERR_NO_ERROR;

  if (p_arfb->blk_data.period_of_execution == 0)
  {
    block_err_desc |= BLK_ERR_EXE_TIME;
  }

  if (!(p_arfb->arith_type))
  {
    block_err_desc |= BLK_ERR_ARITH_TYPE_UNINIT;
  }

  /* Check ranges and limits ---------------------------------------------- */
  if (p_arfb->range_lo > p_arfb->range_hi)
  {
    block_err_desc |= BLK_ERR_RANGE_SET;
  }

  if (p_arfb->comp_lo_lim > p_arfb->comp_hi_lim)
  {
    block_err_desc |= BLK_ERR_COMP_LIM_SET;
  }

  if (p_arfb->out_lo_lim  > p_arfb->out_hi_lim)
  {
    block_err_desc |= BLK_ERR_OUT_LIM_SET;
  }

  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_arfb->block_err &= ~BLK_ERR_BLOCK_CONFIG_ERR;
  }
  else
  {
    p_arfb->block_err |= BLK_ERR_BLOCK_CONFIG_ERR;
  }

  p_arfb->block_err_desc_1 = block_err_desc;

  return;
}


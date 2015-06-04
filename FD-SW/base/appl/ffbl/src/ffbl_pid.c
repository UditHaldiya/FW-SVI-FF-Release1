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
*                     Copyright (C) SOFTING IA GmbH 2012                     *
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
* PROJECT_NAME             Softing FF/PA FD 2.42                             *
*                                                                            *
* VERSION                  FF - 2.42                                         *
*                          PA - 2.42 (beta)                                  *
*                                                                            *
* DATE                     16. April 2012                                    *
*                                                                            *
*****************************************************************************/

/**
    \file ffbl_pid.c.
    \brief It calls the PID algorithm and read/write handler

*/

/* ===========================================================================

FILE_NAME          ffbl_pid.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKPID)

//#define _TIMEMEAS

INCLUDES
  #include <float.h>

  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_res.h"
  #include "ffbl_pai.h"
  #include "ffbl_pid.h"
  #include "..\..\..\..\target\appl\fdev\inc\defs_pid.h"
  #include "..\..\..\..\target\appl\fdev\inc\controller_pid.h"
#ifdef _TIMEMEAS
  #include "..\..\..\..\target\appl\fdev\inc\mcu.h"
#endif

  #include <timemeas.h>

LOCAL_DEFINES

  
LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID Reset_PIDFB (T_PIDFB_DATA *);
  FUNCTION LOCAL VOID Check_config_PIDFB (T_PIDFB *);
  FUNCTION LOCAL VOID Calc_timer_values_PIDFB (T_PIDFB *, T_PIDFB_DATA *);
  FUNCTION LOCAL VOID Update_alarm_sum_PIDFB (T_FBIF_BLOCK_INSTANCE *);
  FUNCTION LOCAL VOID Calc_block_mode_PIDFB (T_PIDFB *, T_PIDFB_DATA *);
  FUNCTION LOCAL VOID Clear_alarms_PIDFB (T_PIDFB *, USIGN16);

IMPORT_DATA
#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT extern T_FBS_ALERT_NOTIFY    ffbl_alert_notify;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

EXPORT_DATA

LOCAL_DATA
#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

//const USIGN8 plusInf[4] = {0x7f,0x80,0x00,0x00}; /* bugreport 17657 */
//const USIGN8 minusInf[4] = {0xff,0x80,0x00,0x00};/* bugreport 17657 */
const USIGN8 plusInf[4] = {0x00,0x00,0x80,0x7f}; /* bugreport 17657 */
const USIGN8 minusInf[4] = {0x00,0x00,0x80,0xff};/* bugreport 17657 */

/****************************************************************************/

FUNCTION GLOBAL VOID Start_PIDFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_PIDFB *        p_pidfb;
  T_PIDFB_DATA *   p_pidfb_data;

  FLOAT   Tmp_factor;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_PIDFB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_pidfb   = p_block_instance->p_block_desc->p_block;
  p_pidfb_data = p_block_instance->p_block_data;

  /* Initialize internal function block data ------------------------------ */
  p_pidfb_data->bypass_changed        = FALSE;
  p_pidfb_data->tracking              = FALSE;
  p_pidfb_data->old_block_alarm_bits  = 0;

  /* Store out param in the internal block-specific data structure -------- */
  p_pidfb_data->out    = p_pidfb->out;

  /* Set out status to BAD out-of-service --------------------------------- */
  p_pidfb->out.status  = SQ_BAD|SUB_OUT_OF_SERVICE;

  Reset_PIDFB (p_pidfb_data);

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_pidfb->update_evt);

  /* Initialize the alert's unit index (New requirement ITK 6, tests i0602_xx) */
  p_pidfb->lo_alm.unit_index    = p_pidfb->pv_scale.units;
  p_pidfb->lo_lo_alm.unit_index = p_pidfb->pv_scale.units;
  p_pidfb->hi_alm.unit_index    = p_pidfb->pv_scale.units;
  p_pidfb->hi_hi_alm.unit_index = p_pidfb->pv_scale.units;
  p_pidfb->dv_lo_alm.unit_index = p_pidfb->pv_scale.units;
  p_pidfb->dv_hi_alm.unit_index = p_pidfb->pv_scale.units;

  /* Check function block configuration ----------------------------------- */
  Check_config_PIDFB (p_pidfb);

  p_pidfb_data->I_factor = (FLOAT)0.0;
  p_pidfb_data->D_factor = (FLOAT)0.0;

  /* Calculate timer values ----------------------------------------------- */
  /* Adaptation of I,D, PV factor to period of execution. Can be called often */
  Calc_timer_values_PIDFB (p_pidfb,p_pidfb_data);

  /* Calculate BKCAL hysteresis-------------------------------------------- */
  p_pidfb_data->out_lim_hys  = p_pidfb->out_scale.eu_100 - p_pidfb->out_scale.eu_0;
  p_pidfb_data->out_lim_hys *= (0.01f * p_pidfb->bkcal_hys);

  /* initialize the PID parameters and coefficients */
  /* working and temporary values from here */
  /* initialize these values to IDEAL algorithm */
  p_pidfb_data->last_I_sum = (FLOAT)0.0;
  p_pidfb_data->last_algorithm = ALGO_IDEAL;
  p_pidfb_data->last_outputvalue = (FLOAT) 0.0;
  p_pidfb_data->last_deviation = (FLOAT)0.0;
  p_pidfb_data->last_tfpid = (FLOAT)0.0;
  p_pidfb_data->last_pv = (FLOAT)0.0;
  p_pidfb_data->I_sum = (FLOAT)0.0;
  p_pidfb_data->bump_flag = CONDNO;
  p_pidfb_data->last_new_target_mode = 0;
  p_pidfb_data->tfpid_factor = (FLOAT)1.0;
  p_pidfb_data->tfpd_factor = (FLOAT)1.0;
  p_pidfb_data->kpvc = (FLOAT)0.0;
  p_pidfb_data->kpvl = (FLOAT)0.0;
  p_pidfb_data->kp_2    = (FLOAT)1.0;
  /* working and temporary values up to here */

  /* calc Gain again bugreport 17449 */
  if (p_pidfb->gain == PID_GAIN_OFF)
  {
      Tmp_factor = 1.0;
  }
  else
  {
      if (p_pidfb->gain > PID_GAIN_MIN)
      {
          Tmp_factor = p_pidfb->gain;
      }
      else
      {
          Tmp_factor = PID_GAIN_MIN;
      }
  }
  osif_disable_all_tasks ();
  p_pidfb_data->P_factor = Tmp_factor;
  osif_enable_all_tasks ();
  p_pidfb_data->kp_1    = p_pidfb_data->P_factor;

  /* calc Reset again bugreport 17449  */
  if ((p_pidfb->reset == PID_RESET_OFF) || ((*((FLOAT *)minusInf)) == p_pidfb->reset) || ((*((FLOAT *)plusInf)) == p_pidfb->reset))
  {
      Tmp_factor = 0.0;
  }
  else
  {
      if (p_pidfb->reset > PID_RESET_MIN)
      {
          Tmp_factor = p_pidfb_data->period_of_exec / p_pidfb->reset;
      }
      else
      {
          Tmp_factor = p_pidfb_data->period_of_exec / PID_RESET_MIN;
      }
  }

  osif_disable_all_tasks ();
  p_pidfb_data->I_factor = Tmp_factor;
  osif_enable_all_tasks ();
  p_pidfb_data->ki    = p_pidfb_data->I_factor;

  /* calc Rate again bugreport 17449 */
  if (p_pidfb_data->period_of_exec > 0.0f)
  {
    Tmp_factor = p_pidfb->rate / p_pidfb_data->period_of_exec;
    osif_disable_all_tasks ();
    p_pidfb_data->D_factor = Tmp_factor;
    osif_enable_all_tasks ();
  }
  p_pidfb_data->kd    = p_pidfb_data->D_factor;

  /* recalculate all values for the algorithm, bugreport 17449 */
  pid_re_calc (p_pidfb,p_pidfb_data);
  
  /* Check function block configuration ----------------------------------- */
  Check_config_PIDFB (p_pidfb);
  /* Calculate timer values ----------------------------------------------- */
  /* Adaptation of I,D, PV factor to period of execution. Can be called often */
  Calc_timer_values_PIDFB (p_pidfb,p_pidfb_data);
  
#ifdef _TIMEMEAS
  init_mcu();
#endif

  return;
}






FUNCTION GLOBAL VOID Stop_PIDFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_PIDFB_DATA *              p_pidfb_data;
  T_PIDFB *                   p_pidfb;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_pidfb_data  = p_block_instance->p_block_data;
  p_pidfb       = p_block_desc->p_block;

  if (p_pidfb->mode_blk.actual == MODE_OS) return;

  p_pidfb->mode_blk.actual = MODE_OS;

  /* Set block error ------------------------------------------------------ */
  p_pidfb->block_err &= ~BLK_ERR_LOCAL_OVERRIDE;
  p_pidfb->block_err |= BLK_ERR_OUT_OF_SERVICE;


  /* Clear all alarms ----------------------------------------------------- */
  Clear_alarms_PIDFB (p_pidfb,p_block_desc->block_id);

  /* Store out param in the internal block-specific data structure -------- */
  p_pidfb_data->out          = p_pidfb->out;

  /* Set status of OUT parameter to SQ_BAD|SUB_OUT_OF_SERVICE ------------- */
  p_pidfb->out.status       = SQ_BAD | SUB_OUT_OF_SERVICE;
  p_pidfb->pv.status        = SQ_BAD | SUB_OUT_OF_SERVICE;
  p_pidfb->bkcal_out.status = SQ_BAD | SUB_OUT_OF_SERVICE;
  p_pidfb->rcas_out.status  = SQ_BAD | SUB_OUT_OF_SERVICE;
  p_pidfb->rout_out.status  = SQ_BAD | SUB_OUT_OF_SERVICE;


  /* Reset internal function block data ----------------------------------- */
  Reset_PIDFB (p_pidfb_data);

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_pidfb->update_evt);

  /* Update block alarm --------------------------------------------------- */
  ffbl_bas_update_extended_block_alarm
  (
    p_block_desc, &p_pidfb->block_alm,
    p_pidfb->alarm_sum.disable, p_pidfb->ack_option,
    BLK_ERR_OUT_OF_SERVICE, &p_pidfb_data->old_block_alarm_bits
  );

  return;
}





FUNCTION GLOBAL USIGN16 Read_handler_PIDFB
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

  /* Check and/or update parameter to be read ----------------------------- */
  if (p_read->rel_idx == PID_PAR_ALARM_SUM)
  {
    Update_alarm_sum_PIDFB (p_block_instance);
  }

  return (E_OK);
}





FUNCTION GLOBAL USIGN16 Write_handler_PIDFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBIF_WRITE_DATA *         p_write
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Input   a_of_od       address of object description of object to be read
        index         index of object to be read
        object_code   type of object
        subindex      to be read

Return  E_OK    or a neg. return code defined in fbap.h

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_PIDFB_DATA *              p_pidfb_data;
  T_PIDFB *                   p_pidfb;
  USIGN8                      subindex;
  USIGN8 *                    source;
  USIGN8                      result;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_pidfb_data  = p_block_instance->p_block_data;
  p_pidfb       = p_block_desc->p_block;

  subindex      = p_write->subindex;
  source        = p_write->source;
  result        = E_OK;

  switch (p_write->rel_idx)
  {
    case PID_PAR_ALERT_KEY:
    {
      if (*source == 0) result = E_FB_PARA_CHECK;

      break;
    }

    case PID_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (
                 &p_pidfb->mode_blk,
                 subindex,
                 source,
                 MODE_PID_VALID
               );
      break;
    }

    case PID_PAR_SP:
    {
      FLOAT *   a_of_value;

      if (MODE_LOWER_THAN_AUTO (p_pidfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      if      (subindex == 0) a_of_value = (FLOAT *)(source + 1);
      else if (subindex == 2) a_of_value = (FLOAT *) source;
      else                    return (E_FB_DATA_NO_WRITE);

      p_pidfb->sp.status = SQ_GOOD_CAS;

      /* Check scale-limits: PV_SCALE +/-10% -------------------------- */
      ffbl_bas_check_scale_limits (a_of_value,&p_pidfb->pv_scale);

      break;
    }

    case PID_PAR_OUT:
     {
      FLOAT *   a_of_value;

      if (MODE_LOWER_THAN_MAN (p_pidfb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      if      (subindex == 0) a_of_value = (FLOAT *)(source + 1);
      else if (subindex == 2) a_of_value = (FLOAT *) source;
      else                    return (E_FB_DATA_NO_WRITE);

      /* Check scale-limits: OUT_SCALE +/-10% ------------------------- */
      ffbl_bas_check_scale_limits (a_of_value,&p_pidfb->out_scale);

      break;
    }

    case PID_PAR_PV_SCALE:
    case PID_PAR_OUT_SCALE:
    {
      if (MODE_NOT_OS (p_pidfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      break;
    }

    case PID_PAR_TRK_SCALE:
    case PID_PAR_FF_SCALE:
    case PID_PAR_FF_GAIN:
    {
      if (MODE_LOWER_THAN_MAN (p_pidfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      break;
    }

    case PID_PAR_GRANT_DENY:
    {
      ffbl_bas_write_grant_deny (subindex,source,&p_pidfb->grant_deny);

      break;
    }

    case PID_PAR_CONTROL_OPTS:
    {
      if (MODE_NOT_OS (p_pidfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      *((USIGN16 *)source) &= PID_CNTRL_OPTS; /* Delete all undefined bits */

      break;
    }

    case PID_PAR_STATUS_OPTS:
    {
      if (MODE_NOT_OS (p_pidfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      *((USIGN16 *)source) &= PID_STATUS_OPTS;  /* Delete all undefined bits */

      break;
    }

    case PID_PAR_RATE:
    {
      FLOAT period_of_exec = p_pidfb->blk_data.period_of_execution / 32000.0f;
      if (*((FLOAT *)source) < 0)
        return (E_FB_PARA_LIMIT);
     
      /* if block is running then reject bad values */
      if (p_pidfb->blk_data.period_of_execution > 0)
      {
        if ((0.0 < (*((FLOAT *)source))) && ((*((FLOAT *)source)) < (period_of_exec * BLK_ERRTIMEERRMIN)))
        {
            //return (E_FB_PARA_LIMIT);
			*((FLOAT *)source) = (period_of_exec * BLK_ERRTIMEERRMIN);
        }
      }
      break;
    }
    case PID_PAR_PV_FTIME:
    case PID_PAR_BAL_TIME:
    {
      if (*((FLOAT *)source) < 0)
        return (E_FB_PARA_LIMIT);

      break;
    }

    case PID_PAR_RESET:/* range check by Mesco */
	{
          if (/* bugreport 17657 */
              ((*((FLOAT *)source)) != (*((FLOAT *)plusInf)))
                &&
              ((*((FLOAT *)source)) != (*((FLOAT *)minusInf)))
              )
          {
	    if ((*((FLOAT *)source) < PID_RESET_OFF) || (*((FLOAT *)source) > PID_RESET_MAX) || ((*((FLOAT *)source) > PID_RESET_OFF) && (*((FLOAT *)source) < PID_RESET_MIN)))
		{
			return (E_FB_PARA_LIMIT);
		}
          }
          /* if block is running then reject bad values */
          if (p_pidfb->blk_data.period_of_execution > 0)
          {
            FLOAT period_of_exec = p_pidfb->blk_data.period_of_execution / 32000.0f;
            if ((0.0 < (*((FLOAT *)source))) && ((*((FLOAT *)source)) < (period_of_exec * BLK_ERRTIMEERRMIN)))
            {
            	*((FLOAT *)source) = (period_of_exec * BLK_ERRTIMEERRMIN);
            }
            
          }

          break;
	}

	
	
    case PID_PAR_BYPASS:
    {
      if (MODE_LOWER_THAN_MAN (p_pidfb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      if (*source == BYPASS_ON)
        if (!(p_pidfb->control_opts & CNTRL_OPT_BYPASS_ENABLE))
          return (E_FB_PARA_CHECK);

      if ((*source == BYPASS_OFF) && (p_pidfb->bypass == BYPASS_ON))
      {
        p_pidfb_data->bypass_changed = TRUE;
      }

      else if ((*source == BYPASS_ON) && (p_pidfb->bypass != BYPASS_ON) )
      {
        p_pidfb_data->bypass_changed = TRUE;
      }

      break;
    }

    case PID_PAR_SP_RATE_DN:
    case PID_PAR_SP_RATE_UP:
    {
      if (*((FLOAT *)source) < 0)
        return (E_FB_PARA_LIMIT);
      break;
    }

    case PID_PAR_SP_HI_LIM:
    case PID_PAR_SP_LO_LIM:
    {
      /* Check scale-limits: PV_SCALE +/-10% -------------------------- */
      ffbl_bas_check_scale_limits ((FLOAT *)source,&p_pidfb->pv_scale);

      break;
    }

    case PID_PAR_HI_HI_LIM:
    case PID_PAR_HI_LIM:
    {
      if (*(USIGN32 *)source == 0x7F800000)
      {
        break; /* +INF is an allowed value */
      }

      if (*(FLOAT *)source > p_pidfb->pv_scale.eu_100)
      {
        /* Map the written value to the upper bound of PV_SCALE ----------- */
        *(FLOAT *)source = p_pidfb->pv_scale.eu_100;
      }
      else if (*(FLOAT *)source < p_pidfb->pv_scale.eu_0)
      {
        /* Do not allow values bellow the lower bound of PV_SCALE --------- */
        return (E_FB_PARA_CHECK);
      }

      break;
    }


    case PID_PAR_LO_LIM:
    case PID_PAR_LO_LO_LIM:
    {
      if (*(USIGN32 *)source == 0xFF800000)
      {
        break; /* -INF is an allowed value */
      }

      if (*(FLOAT *)source < p_pidfb->pv_scale.eu_0)
      {
        /* Map the written value to the lower bound of PV_SCALE ----------- */
        *(FLOAT *)source = p_pidfb->pv_scale.eu_0;
      }
      else if (*(FLOAT *)source > p_pidfb->pv_scale.eu_100)
      {
        /* Do not allow values above the upper bound of PV_SCALE --------- */
        return (E_FB_PARA_CHECK);
      }

      break;
    }

    case PID_PAR_DV_HI_LIM:
    {
      FLOAT pv_span;

      if (*(USIGN32 *)source == 0x7F800000)
      {
        break; /* +INF is an allowed value */
      }

      pv_span = p_pidfb->pv_scale.eu_100 - p_pidfb->pv_scale.eu_0;

      if (*(FLOAT *)source > pv_span)
      {
        /* Map the written value to span of the PV_SCALE ------------------ */
        *(FLOAT *)source = pv_span;
      }
      else if (*(FLOAT *)source < 0)
      {
        /* Do not allow values less than 0 -------------------------------- */
        return (E_FB_PARA_CHECK);
      }

      break;
    }

    case PID_PAR_DV_LO_LIM:
    {
      FLOAT pv_span;

      if (*(USIGN32 *)source == 0xFF800000)
      {
        break; /* -INF is an allowed value */
      }

      pv_span = p_pidfb->pv_scale.eu_0 - p_pidfb->pv_scale.eu_100;

      if (*(FLOAT *)source < pv_span)
      {
        /* Map the written value to span of the PV_SCALE ------------------ */
        *(FLOAT *)source = pv_span;
      }
      else if (*(FLOAT *)source > 0)
      {
        /* Do not allow values greater than 0 ----------------------------- */
        return (E_FB_PARA_CHECK);
      }

      break;
    }

    case PID_PAR_OUT_HI_LIM:
    case PID_PAR_OUT_LO_LIM:
    {
      /* Check scale-limits: OUT_SCALE +/-10% ------------------------- */
      ffbl_bas_check_scale_limits ((FLOAT *)source,&p_pidfb->out_scale);

      break;
    }

    case PID_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_pidfb->update_evt.unack, source, subindex);

      break;
    }

    case PID_PAR_BLOCK_ALM:
    {
      result = ffbl_check_unack_flag (&p_pidfb->block_alm.unack, source, subindex);

      break;
    }

    case PID_PAR_ALARM_HYS:
    case PID_PAR_BKCAL_HYS:
    {
      if (*((FLOAT *)source) < 0.0)
        *((FLOAT *)source) = 0.0;
      else if (*((FLOAT *)source) > 50.0)
        *((FLOAT *)source) = 50.0;

      break;
    }

    case PID_PAR_RCAS_IN:
    case PID_PAR_ROUT_IN:
    {
      if (p_resource->write_lock == WRITE_LOCKED)
      {
        return E_FB_WRITE_LOCK; /* ITK, test case i1105_20 */
      }

      break;
    }

    case PID_PAR_SHED_OPT:
    {
      if ((*source < SHED_NORMAL_NORMAL_RETURN) ||
          (*source > SHED_TO_RETAINED_NO_RETURN)  )
        return (E_FB_PARA_CHECK);

      break;
    }

    case PID_PAR_HI_HI_PRI:
    case PID_PAR_HI_PRI:
    case PID_PAR_LO_PRI:
    case PID_PAR_LO_LO_PRI:
    case PID_PAR_DV_HI_PRI:
    case PID_PAR_DV_LO_PRI:
    {
      if (*source > WRITE_PRI_MAX)
        return (E_FB_PARA_LIMIT);

      break;
    }
        
    /* *************************************** */
    /* begin additional parameters for the PID */

    case PID_PAR_GAIN:                
    {
      if ((*((FLOAT *)source) < PID_GAIN_OFF) || (*((FLOAT *)source) > PID_GAIN_MAX) || ((*((FLOAT *)source) > PID_GAIN_OFF) && (*((FLOAT *)source) < PID_GAIN_MIN)))
        return (E_FB_PARA_LIMIT);

      break;
    }

    case PID_ALGORITHM:
    {
      if (*((USIGN8 *)source) > PID_ALGO_MAX)
        return (E_FB_PARA_LIMIT);
      break;
    }
    
    case PID_TFPD:
    {
      if ((*((FLOAT *)source) < PID_TFPD_OFF) || ((*((FLOAT *)source) > PID_TFPD_OFF) && (*((FLOAT *)source) < PID_TFPD_MIN)))
        return (E_FB_PARA_LIMIT);
      break;
    }
 
    case PID_TFPID:
    {
      if ((*((FLOAT *)source) < PID_TFPID_OFF) || ((*((FLOAT *)source) > PID_TFPID_OFF) && (*((FLOAT *)source) < PID_TFPID_MIN)))
        return (E_FB_PARA_LIMIT);
      break;
    }

    /*  end additional parameters for the PID  */
    /* *************************************** */
    
  }


  if (result == E_OK)
  {
     result = fbs_write_param (p_block_instance,p_write);
  }

  if (result == E_OK)
  {
    BOOL check_config = FALSE;

    switch (p_write->rel_idx)
    {
      case PID_PAR_BLK_DATA:
      {
        Calc_timer_values_PIDFB (p_pidfb,p_pidfb_data);

        check_config = TRUE;

        break;
      }

      case PID_PAR_MODE_BLK:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        check_config                    = TRUE;
        p_pidfb_data->mode_was_written  = TRUE;

        write_loc.rel_idx = 0;

        if (p_pidfb->mode_blk.target & MODE_OS)
        {
          p_pidfb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;

          write_loc.rel_idx = PID_PAR_OUT;
        }

        else if (p_pidfb->mode_blk.target & MODE_MAN)
        {
          write_loc.rel_idx = PID_PAR_OUT;
        }

        if (write_loc.rel_idx)
        {
          /* Store out parameter in non-volatile RAM ---------------------- */
          write_loc.subindex      = 0;
          write_loc.length        = 5;
          write_loc.source        = (USIGN8 *)&p_pidfb->out;
          write_loc.startup_sync  = FALSE;

          fbs_write_param_loc (p_block_instance,&write_loc);
        }

        break;
      }

      case PID_PAR_OUT:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        p_pidfb->out.status = SQ_GOOD_CAS;

        /* Store GOOD out status in non-volatile RAM ---------------------- */
        write_loc.rel_idx       = PID_PAR_OUT;
        write_loc.subindex      = 0;
        write_loc.length        = 5;
        write_loc.source        = (USIGN8 *)&p_pidfb->out;
        write_loc.startup_sync  = FALSE;

        fbs_write_param_loc (p_block_instance,&write_loc);

        /* Store out param in the internal block-specific data structure -- */
        p_pidfb_data->out = p_pidfb->out;

        /* Adapt out status in accordance to the actual mode -------------- */
        if (p_pidfb->mode_blk.actual == MODE_OS)
        {
          p_pidfb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;
        }
        else
        {
          p_pidfb->out.status |= LIMIT_CONST;
        }

        break;
      }

      case PID_PAR_RCAS_IN:
      {
        if ((p_pidfb->mode_blk.target & MODE_RCAS)&&(*source & SQ_GOOD))
        {
          p_pidfb_data->shed.active       = FALSE;
          p_pidfb_data->shed.current_time = 0;
        }

        break;
      }

      case PID_PAR_ROUT_IN:
      {
        if ((p_pidfb->mode_blk.target & MODE_ROUT)&&(*source & SQ_GOOD))
        {
          p_pidfb_data->shed.active       = FALSE;
          p_pidfb_data->shed.current_time = 0;
        }

        break;
      }

      case PID_PAR_RESET:
      {
        FLOAT   I_factor;
        
        /*bugreport 17657 cannot set integral part to infinity */
        if ((p_pidfb->reset == PID_RESET_OFF) || ((*((FLOAT *)minusInf)) == p_pidfb->reset) || ((*((FLOAT *)plusInf)) == p_pidfb->reset))
        {
            I_factor = 0.0;
            p_pidfb_data->last_I_sum = (FLOAT)0.0;
            p_pidfb_data->I_sum = (FLOAT)0.0;
        }
        else
        {
            if (p_pidfb->reset > PID_RESET_MIN)
            {
                I_factor = p_pidfb_data->period_of_exec / p_pidfb->reset;
            }
            else
            {
                I_factor = p_pidfb_data->period_of_exec / PID_RESET_MIN;
            }
        }

        osif_disable_all_tasks ();

        p_pidfb_data->I_factor = I_factor;

        osif_enable_all_tasks ();

        break;
      }

      case PID_PAR_RATE:
      {
        if (p_pidfb_data->period_of_exec > 0.0f)
        {
          FLOAT D_factor = p_pidfb->rate / p_pidfb_data->period_of_exec;

          osif_disable_all_tasks ();

          p_pidfb_data->D_factor = D_factor;

          osif_enable_all_tasks ();
        }

        break;
      }
      
      case PID_PAR_BAL_TIME:
      {
        if (p_pidfb_data->period_of_exec > 0.0f)
        {
          FLOAT old_B_time,new_B_time;

          old_B_time  = p_pidfb_data->B_time;
          new_B_time  = p_pidfb->bal_time / p_pidfb_data->period_of_exec;

          osif_disable_all_tasks ();

          if (old_B_time != p_pidfb_data->bal_timer)
          {
            p_pidfb_data->bal_timer -= (old_B_time - new_B_time);

            if (p_pidfb_data->bal_timer < 0.0f) p_pidfb_data->bal_timer = 0.0f;
          }

          p_pidfb_data->B_time = new_B_time;

          osif_enable_all_tasks ();
        }

        break;
      }

      case PID_PAR_PV_FTIME:
      {
        if (p_pidfb_data->period_of_exec > 0.0f)
        {
          FLOAT pv_factor = p_pidfb_data->period_of_exec /
                           (p_pidfb_data->period_of_exec + p_pidfb->pv_ftime);

          osif_disable_all_tasks ();

          p_pidfb_data->pv_factor = pv_factor;

          osif_enable_all_tasks ();
        }

        break;
      }

      case PID_PAR_OUT_SCALE:
      case PID_PAR_BKCAL_HYS:
      {
        FLOAT out_lim_hys;

        out_lim_hys  = p_pidfb->out_scale.eu_100 - p_pidfb->out_scale.eu_0;
        out_lim_hys *= (0.01f * p_pidfb->bkcal_hys);

        if (out_lim_hys >= 0.0f)
        {
          osif_disable_all_tasks ();

          p_pidfb_data->out_lim_hys  = out_lim_hys;

          osif_enable_all_tasks ();
        }

        if (p_write->rel_idx == PID_PAR_OUT_SCALE)
        {
          check_config = TRUE;
        }

        break;
      }

      case PID_PAR_PV_SCALE:
      {
        /* Update the alert's unit index (New requirement ITK 6, tests i0602_xx) */
        p_pidfb->lo_alm.unit_index    = p_pidfb->pv_scale.units;
        p_pidfb->lo_lo_alm.unit_index = p_pidfb->pv_scale.units;
        p_pidfb->hi_alm.unit_index    = p_pidfb->pv_scale.units;
        p_pidfb->hi_hi_alm.unit_index = p_pidfb->pv_scale.units;
        p_pidfb->dv_lo_alm.unit_index = p_pidfb->pv_scale.units;
        p_pidfb->dv_hi_alm.unit_index = p_pidfb->pv_scale.units;

        /* no break - fall through */
      }

      case PID_PAR_BYPASS:
      case PID_PAR_SHED_OPT:
      case PID_PAR_TRK_SCALE:
      case PID_PAR_FF_SCALE:
      {
        check_config = TRUE;   break;
      }
      
      /* *************************************** */
      /* begin additional parameters for the PID */
    
      case PID_PAR_GAIN:
      {
        FLOAT   P_factor;

        if (p_pidfb->gain == PID_GAIN_OFF)
        {
            P_factor = 1.0;
        }
        else
        {
            if (p_pidfb->gain > PID_GAIN_MIN)
            {
                P_factor = p_pidfb->gain;
            }
            else
            {
                P_factor = PID_GAIN_MIN;
            }
        }

        osif_disable_all_tasks ();

        p_pidfb_data->P_factor = P_factor;

        osif_enable_all_tasks ();

        break;
      }    
    
      case PID_TFPD:
      {
        FLOAT tfpd_factor ;
      
        if (p_pidfb_data->period_of_exec > 0.0f)
        {
            if (p_pidfb->tfpd >= PID_TFPD_MIN)
            {
                tfpd_factor = p_pidfb_data->period_of_exec / p_pidfb->tfpd;
            }
            else
            {
                if(p_pidfb->tfpd == PID_TFPD_OFF)
                {
                    tfpd_factor = PID_TF_FALSE;
                }
                else
                {
                    tfpd_factor = p_pidfb_data->period_of_exec / PID_TFPD_MIN;
                }    
            }        

            osif_disable_all_tasks ();

            p_pidfb_data->tfpd_factor = tfpd_factor;

            osif_enable_all_tasks ();
        }

        break;
      }    
    
      case PID_TFPID:
      {
        FLOAT tfpid_factor ;
      
        if (p_pidfb_data->period_of_exec > 0.0f)
        {
            if (p_pidfb->tfpid >= PID_TFPID_MIN)
            {
                tfpid_factor = p_pidfb_data->period_of_exec / p_pidfb->tfpid;
            }
            else
            {
                if(p_pidfb->tfpid == PID_TFPID_OFF)
                {
                    tfpid_factor = PID_TF_FALSE;
                }
                else
                {
                    tfpid_factor = p_pidfb_data->period_of_exec / PID_TFPID_MIN;
                }    
            }        

            osif_disable_all_tasks ();

            p_pidfb_data->tfpid_factor = tfpid_factor;

            osif_enable_all_tasks ();
        }

        break;
      }    
    
      case PID_ALGORITHM:
      { 
        if (p_pidfb->algorithm > PID_ALGO_MAX)
        {
            p_pidfb->algorithm = PID_ALGO_MAX;
        }
      
        break;
      }
    
      /*  end additional parameters for the PID  */
      /* *************************************** */
    }

    
    /* *************************************************** */
    /*  begin additional function cal for the PID re calc  */
    
    switch (p_write->rel_idx)
    {
      case PID_PAR_RESET:
      case PID_PAR_RATE:
      case PID_PAR_GAIN:
      case PID_TFPD:
      case PID_TFPID:
      case PID_ALGORITHM:
      {
        pid_re_calc (p_pidfb,p_pidfb_data);
        check_config = TRUE;

        break;
      }    
    }

    /*   end additional function cal for the PID re calc   */
    /* *************************************************** */

    
    if (check_config)
    {
      Check_config_PIDFB (p_pidfb);
    }
  }

return result;
}







FUNCTION GLOBAL USIGN16 Execute_PIDFB
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
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_PIDFB *                   p_pidfb;
  T_PIDFB_DATA *              p_pidfb_data;
  T_PIDFB_CONTROL_DATA        pid_control_data;
  USIGN8                      old_mode;
  USIGN8                      actual_mode;
  BOOL                        enable_update_evt;
  FLOAT_S                     raw_sp;
  USIGN8                      in_status_quality;
  USIGN8                      pv_status_quality;
  USIGN8                      old_control_active;
  USIGN8                      bkcal_limit;
//MEASURING_LOCALS

FUNCTION_BODY

#ifdef _TIMEMEAS
    TP301_HIGH();
#endif


//  FB_EXEC_START();
  p_block_desc  = p_block_instance->p_block_desc;
  p_pidfb_data  = p_block_instance->p_block_data;
  p_pidfb       = p_block_desc->p_block;

  enable_update_evt = FALSE;

  /* Update block mode -------------------------------------------------- */
  old_mode = p_pidfb->mode_blk.actual;

  if (p_pidfb->block_err & BLK_ERR_BLOCK_CONFIG_ERR)
  {
    actual_mode = MODE_OS;
  }
  else
  {
    Calc_block_mode_PIDFB (p_pidfb,p_pidfb_data);
    actual_mode = p_pidfb->mode_blk.actual;
  }


  if (actual_mode == MODE_OS)
  {
    if (old_mode != MODE_OS)
    {
      /* Mode OS is entered now ------------------------------------------- */
      p_pidfb->mode_blk.actual = old_mode; /* Actual mode is checked and
                                              set again by the stop routine */
      Stop_PIDFB (p_block_instance);
    }
    else
    {
      /* Update block alarm ----------------------------------------------- */
      ffbl_bas_update_extended_block_alarm
      (
        p_block_desc, &p_pidfb->block_alm,
        p_pidfb->alarm_sum.disable, p_pidfb->ack_option,
        BLK_ERR_OUT_OF_SERVICE, &p_pidfb_data->old_block_alarm_bits
      );
    }

    p_pidfb_data->mode_was_written  = FALSE;

//    FB_EXEC_END();

#ifdef _TIMEMEAS
    TP301_LOW();
#endif
    
    return (p_pidfb->blk_data.next_fb_to_execute);
  }


  if (old_mode == MODE_OS) /* Mode OS is left now ------------------------- */
 {
    /* Reset block error -------------------------------------------------- */
    p_pidfb->block_err &= ~(BLK_ERR_OUT_OF_SERVICE|BLK_ERR_BLOCK_CONFIG_ERR);

    /* Restore out parameter ---------------------------------------------- */
    p_pidfb->out = p_pidfb_data->out;

    if ((p_pidfb->out.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_BAD|SUB_OUT_OF_SERVICE))
    {
      p_pidfb->out.status = SQ_BAD;
    }

    p_pidfb_data->control_active = FALSE;

    /* Note: event handling is done at the end of this function */
    enable_update_evt = TRUE;
  }
  else
  {
    enable_update_evt = FALSE;
  }


  /* Handle PV parameter -------------------------------------------------- */
  /* ---------------------------------------------------------------------- */
  /* Determine quality of IN and PV --------------------------------------- */
  in_status_quality = p_pidfb->in.status & QUALITY_MASK;
  pv_status_quality = p_pidfb->pv.status & QUALITY_MASK;

  if (in_status_quality == SQ_GOOD_CAS) in_status_quality = SQ_GOOD;

  if (p_pidfb->status_opts & STATUS_OPT_USE_UNCERT_AS_GOOD)
  {
    if (in_status_quality == SQ_UNCERTAIN) in_status_quality = SQ_GOOD;
  }
  else /* (!(p_pidfb->status_opts & STATUS_OPT_USE_UNCERT_AS_GOOD)) */
  {
    if (in_status_quality == SQ_UNCERTAIN) in_status_quality = SQ_BAD;
  }


  if ((in_status_quality == SQ_GOOD)&&(pv_status_quality == SQ_GOOD))
  {
    /* Apply PV filter ---------------------------------------------------- */
    p_pidfb->pv.value =     p_pidfb_data->pv_factor  * p_pidfb->in.value +
                       (1 - p_pidfb_data->pv_factor) * p_pidfb->pv.value;
  }
  else
  {
    /* Assign IN value to PV value ---------------------------------------- */
    p_pidfb->pv.value = p_pidfb->in.value;
  }

  /* Assign IN status to PV status ---------------------------------------- */
  p_pidfb->pv.status = in_status_quality;
  pv_status_quality  = in_status_quality;


  if (((p_pidfb->out.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_GOOD_CAS|SUB_IA)) ||
      ((p_pidfb->out.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_GOOD_CAS|SUB_IFS))    )
  {
    p_pidfb->out.status = SQ_GOOD_CAS;
  }


  if (pv_status_quality == SQ_BAD)
  {
    goto END_OF_PV_HANDLING; /* Don't check the PV alarm conditions */
  }

  /* Check the PV alarm conditions ---------------------------------------- */
  alm.block_id  = p_block_desc->block_id;
  alm.value    = p_pidfb->pv.value;
  alm.hyst     = (0.01 * p_pidfb->alarm_hys);
  alm.hyst    *= (p_pidfb->pv_scale.eu_100 - p_pidfb->pv_scale.eu_0);

  /* Check low alarms ----------------------------------------------------- */
  alm.high = FALSE;
  if (p_pidfb->lo_alm.current == ALARM_CLEAR)
  {
    alm.ptr      = &p_pidfb->lo_alm;
    alm.idx      = PID_PAR_LO_ALM;
    alm.pri      = p_pidfb->lo_pri;
    alm.disabled = (BOOL)((p_pidfb->alarm_sum.disable & ALARM_SUM_LO)?(TRUE):(FALSE));
    alm.auto_ack = (BOOL)((p_pidfb->ack_option & ALARM_SUM_LO)?(TRUE):(FALSE));
    alm.limit    = p_pidfb->lo_lim;

     Handle_inact_alarm (); /* This routine works with the data stored in alm! */
  }

  alm.ptr      = &p_pidfb->lo_lo_alm;
  alm.idx      = PID_PAR_LO_LO_ALM;
  alm.pri      = p_pidfb->lo_lo_pri;
  alm.disabled = (BOOL)((p_pidfb->alarm_sum.disable & ALARM_SUM_LO_LO)?(TRUE):(FALSE));
  alm.auto_ack = (BOOL)((p_pidfb->ack_option & ALARM_SUM_LO_LO)?(TRUE):(FALSE));
  alm.limit    = p_pidfb->lo_lo_lim;

  if (p_pidfb->lo_lo_alm.current == ALARM_CLEAR)
  {
     Handle_inact_alarm (); /* This routine works with the data stored in alm! */
  }
  else /* (p_pidfb->lo_lo_alm.current == ALARM_ACTIVE) */
  {
     Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }

  if (p_pidfb->lo_alm.current == ALARM_ACTIVE)
  {
    alm.ptr      = &p_pidfb->lo_alm;
    alm.idx      = PID_PAR_LO_ALM;
    alm.pri      = p_pidfb->lo_pri;
    alm.disabled = (BOOL)((p_pidfb->alarm_sum.disable & ALARM_SUM_LO)?(TRUE):(FALSE));
    alm.auto_ack = (BOOL)((p_pidfb->ack_option & ALARM_SUM_LO)?(TRUE):(FALSE));
    alm.limit    = p_pidfb->lo_lim;

     Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }


  /* Check high alarms ---------------------------------------------------- */
  alm.high = TRUE;
  if (p_pidfb->hi_alm.current == ALARM_CLEAR)
  {
    alm.ptr      = &p_pidfb->hi_alm;
    alm.idx      = PID_PAR_HI_ALM;
    alm.pri      = p_pidfb->hi_pri;
    alm.disabled = (BOOL)((p_pidfb->alarm_sum.disable & ALARM_SUM_HI)?(TRUE):(FALSE));
    alm.auto_ack = (BOOL)((p_pidfb->ack_option & ALARM_SUM_HI)?(TRUE):(FALSE));
    alm.limit    = p_pidfb->hi_lim;

     Handle_inact_alarm (); /* This routine works with the data stored in alm! */
  }

  alm.ptr      = &p_pidfb->hi_hi_alm;
  alm.idx      = PID_PAR_HI_HI_ALM;
  alm.pri      = p_pidfb->hi_hi_pri;
  alm.disabled = (BOOL)((p_pidfb->alarm_sum.disable & ALARM_SUM_HI_HI)?(TRUE):(FALSE));
  alm.auto_ack = (BOOL)((p_pidfb->ack_option & ALARM_SUM_HI_HI)?(TRUE):(FALSE));
  alm.limit    = p_pidfb->hi_hi_lim;

  if (p_pidfb->hi_hi_alm.current == ALARM_CLEAR)
  {
     Handle_inact_alarm (); /* This routine works with the data stored in alm! */
  }
  else /* (p_pidfb->hi_hi_alm.current == ALARM_ACTIVE) */
  {
     Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }

  if (p_pidfb->hi_alm.current == ALARM_ACTIVE)
  {
    alm.ptr      = &p_pidfb->hi_alm;
    alm.idx      = PID_PAR_HI_ALM;
    alm.pri      = p_pidfb->hi_pri;
    alm.disabled = (BOOL)((p_pidfb->alarm_sum.disable & ALARM_SUM_HI)?(TRUE):(FALSE));
    alm.auto_ack = (BOOL)((p_pidfb->ack_option & ALARM_SUM_HI)?(TRUE):(FALSE));
    alm.limit    = p_pidfb->hi_lim;

     Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }


  /* Update status of PV parameter ---------------------------------------- */
  if ((p_pidfb->pv.status & QUALITY_MASK) == SQ_GOOD_NC)
  {
    Update_alarm_sum_PIDFB (p_block_instance);

    p_pidfb->pv.status &= ~SUB_STATUS_MASK;

    p_pidfb->pv.status |= Set_good_nc_substatus
      (p_pidfb->alarm_sum.unack,p_pidfb->alarm_sum.current,
       p_pidfb->lo_pri,p_pidfb->lo_lo_pri,
       p_pidfb->hi_pri,p_pidfb->hi_hi_pri);
  }


  END_OF_PV_HANDLING: /* PV handling is finished -------------------------- */


  /* Handle setpoint parameter -------------------------------------------- */
  /* ---------------------------------------------------------------------- */

  raw_sp.status = SQ_BAD;

  /* Apply control option 'SP Track retained target' ---------------------- */
  if (p_pidfb->control_opts & CNTRL_OPT_TRACK_RETAINED_TARGET)
  {
    if (p_pidfb->mode_blk.target & MODE_RCAS)
    {
      Get_setpoint (&raw_sp,p_pidfb->rcas_in);

      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto END_OF_STORE_RETAINED_SP;
      }
    }

    if (p_pidfb->mode_blk.target & MODE_CAS)
    {
      Get_setpoint (&raw_sp,p_pidfb->cas_in);
    }

    END_OF_STORE_RETAINED_SP: ;
  }


  if (actual_mode == MODE_AUTO)
  {
    if ((old_mode == MODE_LO) ||(old_mode == MODE_IMAN)||
        (old_mode == MODE_MAN)||(old_mode == MODE_ROUT)  )
    {
      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
      {
        goto AFTER_GET_SP_IN_AUTO;
      }
    }

    Get_setpoint (&raw_sp,p_pidfb->sp);

    AFTER_GET_SP_IN_AUTO: ;
  }

  else if (actual_mode == MODE_CAS)
  {
    Get_setpoint (&raw_sp,p_pidfb->cas_in);
  }

  else if (actual_mode == MODE_RCAS)
  {
    Get_setpoint (&raw_sp,p_pidfb->rcas_in);
  }

  else if (actual_mode == MODE_ROUT)
  {
    if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
    {
      goto AFTER_GET_SP_IN_ROUT;
    }

    if (p_pidfb->control_opts & CNTRL_OPT_SP_PV_TRACK_IN_ROUT)
    {
      Get_setpoint (&raw_sp,p_pidfb->pv);

      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto AFTER_GET_SP_IN_ROUT;
      }
    }

    Get_setpoint (&raw_sp,p_pidfb->sp);

    AFTER_GET_SP_IN_ROUT: ;
  }

  else if (actual_mode == MODE_MAN)
  {
    if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
    {
      goto AFTER_GET_SP_IN_MAN;
    }

    if (p_pidfb->control_opts & CNTRL_OPT_SP_PV_TRACK_IN_MAN)
    {
      Get_setpoint (&raw_sp,p_pidfb->pv);

      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto AFTER_GET_SP_IN_MAN;
      }
    }

    Get_setpoint (&raw_sp,p_pidfb->sp);

    AFTER_GET_SP_IN_MAN: ;
  }

  else  /* (actual_mode == MODE_LO)||(actual_mode == MODE_IMAN) */
  {
    if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
    {
      goto AFTER_GET_SP_IN_LO_IMAN;
    }

    if (p_pidfb->control_opts & CNTRL_OPT_SP_PV_TRACK_IN_LO_AND_IMAN)
    {
      Get_setpoint (&raw_sp,p_pidfb->pv);

      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto AFTER_GET_SP_IN_LO_IMAN;
      }
    }

    Get_setpoint (&raw_sp,p_pidfb->sp);

    AFTER_GET_SP_IN_LO_IMAN: ;
  }


  if (p_pidfb_data->bypass_changed)
  {
    if (p_pidfb->bypass == BYPASS_ON)
    {
      /* Bypass has been enabled since last fb execution ----------------- */

      /* Initialize SP to OUT -------------------------------------------- */
      raw_sp.status = p_pidfb->sp.status;
      raw_sp.value  = ffbl_bas_rescale_value (p_pidfb->out.value,
                              &p_pidfb->out_scale,&p_pidfb->pv_scale);
    }
    else
    {
      /* Bypass has been disabled since last fb execution ---------------- */

      /* Initialize SP to PV --------------------------------------------- */
      Get_setpoint (&raw_sp,p_pidfb->pv); /* Control deviation is 0 */
    }

    p_pidfb_data->bypass_changed = FALSE;
  }


  /* Update setpoint parameter  ------------------------------------------ */
  p_pidfb->sp = raw_sp;

  if (raw_sp.status == SQ_BAD)
  {
    goto END_OF_SP_HANDLING;
  }


  /* Check setpoint limits of SP parameter -------------------------------- */
  p_pidfb->sp.status &= ~LIMIT_CONST;  /* Reset limit bits ----------- */

  if (raw_sp.value <= p_pidfb->sp_lo_lim)
  {
    raw_sp.value = p_pidfb->sp.value = p_pidfb->sp_lo_lim;
    p_pidfb->sp.status |= LIMIT_LOW;
  }

  if (raw_sp.value >= p_pidfb->sp_hi_lim)
  {
    raw_sp.value = p_pidfb->sp.value = p_pidfb->sp_hi_lim;
    p_pidfb->sp.status |= LIMIT_HIGH;
  }


  if ((actual_mode == MODE_AUTO)                                    &&
      ((p_pidfb_data->last_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)   )
  {
    /* FF-891, 5.3: For control blocks, rate limiting will apply only in Auto */
    FLOAT lim_up;
    FLOAT lim_dn;

    if (p_pidfb->sp_rate_up > 0.0)
    {
      lim_up = p_pidfb_data->last_sp.value + p_pidfb->sp_rate_up * p_pidfb_data->period_of_exec;

      if (raw_sp.value > lim_up)
      {
        raw_sp.value = lim_up; goto END_OF_RAMP;
      }
    }

    if (p_pidfb->sp_rate_dn > 0.0)
    {
      lim_dn = p_pidfb_data->last_sp.value - p_pidfb->sp_rate_dn * p_pidfb_data->period_of_exec;

      if (raw_sp.value < lim_dn)
      {
        raw_sp.value = lim_dn;
      }
    }
  }


  END_OF_RAMP:
  END_OF_SP_HANDLING:


  /* Store the current setpoint-------------------------------------------- */
  p_pidfb_data->last_sp = raw_sp;


  /* Check whether the PID control algorithm has to be executed ----------- */
  old_control_active            = p_pidfb_data->control_active;
  p_pidfb_data->control_active  = TRUE;

  if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
  {
    /* Rescale the working setpoint ------------------------------------- */
    pid_control_data.working_sp = ffbl_bas_rescale_value (raw_sp.value,
                                     &p_pidfb->pv_scale,&p_pidfb->out_scale);
  }
  else /* SP.status != GOOD */
  {
    p_pidfb_data->control_active = FALSE; /* It is not possible to execute
                                             the PID control algorithm    */

    goto APPLY_PID_CONTROL;              /* Do not check deviation alarms */
  }



  /* Check whether the PID control algorithm has to be executed ----------- */
  if ((p_pidfb->bypass == BYPASS_ON)         ||
      (p_pidfb_data->tracking)               ||
      (p_pidfb->mode_blk.target == MODE_MAN) ||
      (actual_mode              == MODE_ROUT)   )
  {
    /* PID control algorithm hasn't to be executed ------------------------ */
    p_pidfb_data->control_active = FALSE;
  }

  else /* The PID control algorithm should be executed -------------------- */
  {
    if (p_pidfb->mode_blk.target & MODE_ROUT)
    {
      if (pv_status_quality == SQ_BAD)
      {
        p_pidfb_data->control_active = FALSE;

        goto APPLY_PID_CONTROL;                 /* Don't switch to MAN mode */
                                            /* Don't check deviation alarms */
      }

      /* The next section is because of ITK 5.2, test case 403_53 --------- */
      if (p_pidfb_data->mode_was_written)
      {
        p_pidfb_data->control_active = FALSE;

        goto APPLY_PID_CONTROL;            /* Do not check deviation alarms */
      }
    }

    if (pv_status_quality == SQ_BAD)
    {
      /* It is not possible to execute the PID control algorithm ---------- */

      if (p_pidfb->mode_blk.actual > MODE_MAN)
      {
        p_pidfb->mode_blk.actual = actual_mode = MODE_MAN;
      }

      if (p_pidfb->status_opts & STATUS_OPT_TARG_MAN_IF_BAD_IN)
      {
        if (p_pidfb->mode_blk.target != MODE_MAN)
        {
          p_pidfb_data->new_target_mode = MODE_MAN;
        }
      }

      p_pidfb_data->control_active = FALSE;

      goto APPLY_PID_CONTROL;   /* Don't check downstream block   */
                                /* Don't check deviation alarms   */
    }

    /* Check whether the downstream block is available ---------------------- */
    if (p_pidfb_data->downstream & PID_DOWNSTREAM_ACTIVE)
    {
      if (old_control_active == FALSE)
      {
        p_pidfb_data->downstream = PID_DOWNSTREAM_INIT;
      }
    }
    else /* Downstream block is not available */
    {
      p_pidfb_data->control_active = FALSE; /* It is not necessary to execute
                                               the PID control algorithm      */
    }
  }



  if (pv_status_quality == SQ_BAD)
  {
    goto APPLY_PID_CONTROL;               /* Don't check deviation alarms   */
  }

  /* Check deviation alarm conditions ------------------------------------- */
  alm.block_id  = p_block_desc->block_id;
  alm.value     = p_pidfb->pv.value - p_pidfb->sp.value;

  /* Check deviation low alarm -------------------------------------------- */
  alm.ptr      = &p_pidfb->dv_lo_alm;
  alm.idx      = PID_PAR_DV_LO_ALM;
  alm.pri      = p_pidfb->dv_lo_pri;
  alm.disabled = (BOOL)((p_pidfb->alarm_sum.disable & ALARM_SUM_DEV_LO)?(TRUE):(FALSE));
  alm.auto_ack = (BOOL)((p_pidfb->ack_option & ALARM_SUM_DEV_LO)?(TRUE):(FALSE));
  alm.limit    = p_pidfb->dv_lo_lim;

  if (p_pidfb->dv_lo_alm.current == ALARM_CLEAR)
  {
    if ((alm.value <= alm.limit) && (!alm.disabled) && alm.pri)
    {
      ffbl_alert_notify.action            = ALARM_ACTIVE;
      ffbl_alert_notify.block_id          = alm.block_id;
      ffbl_alert_notify.rel_idx           = alm.idx;
      ffbl_alert_notify.priority          = alm.pri;
      ffbl_alert_notify.data.flt.subcode  = 0;
      ffbl_alert_notify.data.flt.value    = p_pidfb->pv.value;

      fbs_alert_notify (&ffbl_alert_notify);

      if (alm.auto_ack || (alm.pri == 1))
           alm.ptr->unack = ALARM_ACKED;
      else alm.ptr->unack = ALARM_UNACKED;
    }
  }
  else /* (p_pidfb->dv_lo_alm.current == ALARM_ACTIVE) */
  {
    if (alm.disabled || (alm.value > (alm.limit + alm.hyst)))
    {
      ffbl_alert_notify.action            = ALARM_CLEAR;
      ffbl_alert_notify.block_id          = alm.block_id;
      ffbl_alert_notify.rel_idx           = alm.idx;
      ffbl_alert_notify.priority          = alm.pri;
      ffbl_alert_notify.data.flt.subcode  = 0;
      ffbl_alert_notify.data.flt.value    = p_pidfb->pv.value;

      fbs_alert_notify (&ffbl_alert_notify);
    }
  }


  /* Check deviation high alarm -------------------------------------------- */
  alm.ptr      = &p_pidfb->dv_hi_alm;
  alm.idx      = PID_PAR_DV_HI_ALM;
  alm.pri      = p_pidfb->dv_hi_pri;
  alm.disabled = (BOOL)((p_pidfb->alarm_sum.disable & ALARM_SUM_DEV_HI)?(TRUE):(FALSE));
  alm.auto_ack = (BOOL)((p_pidfb->ack_option & ALARM_SUM_DEV_HI)?(TRUE):(FALSE));
  alm.limit    = p_pidfb->dv_hi_lim;

  if (p_pidfb->dv_hi_alm.current == ALARM_CLEAR)
  {
    if ((alm.value >= alm.limit) && (!alm.disabled) && alm.pri)
    {
      ffbl_alert_notify.action            = ALARM_ACTIVE;
      ffbl_alert_notify.block_id          = alm.block_id;
      ffbl_alert_notify.rel_idx           = alm.idx;
      ffbl_alert_notify.priority          = alm.pri;
      ffbl_alert_notify.data.flt.subcode  = 0;
      ffbl_alert_notify.data.flt.value    = p_pidfb->pv.value;

      fbs_alert_notify (&ffbl_alert_notify);

      if (alm.auto_ack || (alm.pri == 1))
           alm.ptr->unack = ALARM_ACKED;
      else alm.ptr->unack = ALARM_UNACKED;
    }
  }
  else /* (p_pidfb->dv_hi_alm.current == ALARM_ACTIVE) */
  {
    if (alm.disabled || (alm.value < (alm.limit - alm.hyst)))
    {
      ffbl_alert_notify.action            = ALARM_CLEAR;
      ffbl_alert_notify.block_id          = alm.block_id;
      ffbl_alert_notify.rel_idx           = alm.idx;
      ffbl_alert_notify.priority          = alm.pri;
      ffbl_alert_notify.data.flt.subcode  = 0;
      ffbl_alert_notify.data.flt.value    = p_pidfb->pv.value;

      fbs_alert_notify (&ffbl_alert_notify);
    }
  }



  APPLY_PID_CONTROL:

  /* Calculate feed forward deviation ------------------------------------- */
  p_pidfb_data->last_feed_forward = p_pidfb_data->feed_forward;
  pid_control_data.ff_deviation   = 0.0f;

  if ((p_pidfb->ff_val.status & QUALITY_MASK) == SQ_GOOD_NC)
  {
    p_pidfb_data->feed_forward.value = ffbl_bas_rescale_value
               (p_pidfb->ff_val.value,&p_pidfb->ff_scale,&p_pidfb->out_scale);

    p_pidfb_data->feed_forward.value *= p_pidfb->ff_gain;
    p_pidfb_data->feed_forward.status = SQ_GOOD;

    if (p_pidfb_data->last_feed_forward.status == SQ_GOOD)
    {
      pid_control_data.ff_deviation =
        p_pidfb_data->feed_forward.value - p_pidfb_data->last_feed_forward.value;
    }
  }
  else
  {
    p_pidfb_data->feed_forward.status = SQ_BAD;
  }


  /* Reset OUT limit bits ------------------------------------------------- */
  p_pidfb->out.status &=~LIMIT_MASK;

  if (p_pidfb_data->control_active)
  {
    /* Rescale the pv value ----------------------------------------------- */
    pid_control_data.working_pv = ffbl_bas_rescale_value (p_pidfb->pv.value,
                                     &p_pidfb->pv_scale, &p_pidfb->out_scale);

    p_pidfb->out.value = pid_control (p_pidfb,p_pidfb_data,&pid_control_data);

    if (p_pidfb_data->downstream == PID_DOWNSTREAM_INIT)
    {
      if ((p_pidfb->bkcal_in.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_GOOD_CAS|SUB_IR))
      {
        p_pidfb->out.status = SQ_GOOD_CAS|SUB_IA;
      }
      else
      {
        p_pidfb->out.status = SQ_GOOD_CAS;
      }

      p_pidfb_data->downstream = PID_DOWNSTREAM_ACTIVE;
    }
  }

  else /* The PID conrol algrithm hasn't been executed - update OUT parameter */
  {
    /* remember the mode MESCO */
    p_pidfb_data->last_new_target_mode = p_pidfb->mode_blk.actual;
    
    if (actual_mode == MODE_ROUT)
    {
      p_pidfb->out.status  = p_pidfb->rout_in.status & QUALITY_MASK;
      p_pidfb->out.value   = p_pidfb->rout_in.value;
    }

    //Ugly hack for viewing pleasure; no effect on actual behavior
	if(actual_mode == MODE_IMAN)
	{
        p_pidfb->out  = p_pidfb->bkcal_in;
	}
	
	if ((actual_mode == MODE_MAN)||(actual_mode == MODE_IMAN))
    {
      if (p_pidfb_data->downstream == PID_DOWNSTREAM_INIT)
      {
        p_pidfb->out.value  = p_pidfb->bkcal_in.value;

        if ((p_pidfb->bkcal_in.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_GOOD_CAS|SUB_IR))
        {
          p_pidfb->out.status = SQ_GOOD_CAS|SUB_IA;
        }
        else
        {
          p_pidfb->out.status = SQ_GOOD_CAS;
        }

        p_pidfb_data->downstream = PID_DOWNSTREAM_ACTIVE;
      }

      if ((p_pidfb->out.status & SQ_GOOD) != SQ_GOOD)
      {
        if ((p_pidfb->bkcal_in.status & QUALITY_MASK) != SQ_BAD)
        {
          p_pidfb->out.value  = p_pidfb->bkcal_in.value;
          p_pidfb->out.status = SQ_GOOD_CAS;
        }
      }

      if (actual_mode == MODE_MAN)
      {
        p_pidfb->out.status |= LIMIT_CONST;
      }

      p_pidfb->bkcal_out.status = SQ_GOOD_CAS|SUB_NI;
      p_pidfb->rcas_out.status  = SQ_GOOD_CAS|SUB_NI;
      p_pidfb->rout_out.status  = SQ_GOOD_CAS|SUB_NI;

      if (p_pidfb->mode_blk.target == MODE_MAN)
      {
        if (p_pidfb->control_opts & CNTRL_OPT_NO_OUT_LIM_IN_MAN)
        {
          goto END_OF_OUT_LIM_CHECK;
        }
      }
    }

    else if (actual_mode == MODE_LO)
    {
      /* External tracking is enabled -------------------------------------- */
      if (p_pidfb->trk_val.status & SQ_GOOD)
      {
        p_pidfb_data->working_trk_val.value = ffbl_bas_rescale_value
         (p_pidfb->trk_val.value,&p_pidfb->trk_scale,&p_pidfb->out_scale);
        p_pidfb_data->working_trk_val.status = SQ_GOOD_NC;
      }

      if (p_pidfb_data->working_trk_val.status == SQ_GOOD_NC)
      {
        p_pidfb->out.value  = p_pidfb_data->working_trk_val.value;
        p_pidfb->out.status = SQ_GOOD_CAS|LIMIT_CONST;
      }
      else /* (p_pidfb->trk_val.status is not GOOD) */
      {
        /* Interop. Test 2400_05 -------------------------------------------- */

        if (p_pidfb->mode_blk.target & MODE_ROUT)
        {
          if ( ((p_pidfb->rout_in.status & QUALITY_MASK) == SQ_GOOD_CAS) ||
               ((p_pidfb->rout_in.status & QUALITY_MASK) == SQ_GOOD_NC)
             )
          {
            p_pidfb->out.status  = p_pidfb->rout_in.status & QUALITY_MASK;
            p_pidfb->out.value   = p_pidfb->rout_in.value;
          }
        }
      }
    }

    else /* actual_mode == MODE_AUTO | MODE_CAS | MODE_RCAS */
    {
      /* Apply bypass function ------------------------------------------------ */
      if ((p_pidfb->bypass == BYPASS_ON)&&((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS))
      {
        /* Assign the working setpoint to the out parameter ------------------- */
        p_pidfb->out.value = pid_control_data.working_sp;
      }
      else
      {
        if ((p_pidfb_data->mode_was_written == FALSE)||(!(p_pidfb->mode_blk.target & MODE_ROUT)))
        {
          p_pidfb->out.value  = p_pidfb->bkcal_in.value;
          p_pidfb->out.status = SQ_GOOD_CAS|LIMIT_CONST;
        }
        else
        {
          ;/* Do not update the OUT parameter because of ITK 5.2, test case i0403_53 */
        }
      }

    }
  }


  if (p_pidfb->control_opts & CNTRL_OPT_TRACK_ENABLE)
  {
    if ((p_pidfb->trk_in_d.status & QUALITY_MASK) == SQ_BAD)
    {
      /* Apply status option 'IFS if BAD TRK_IN_D' -------------------------- */
      if (p_pidfb->status_opts & STATUS_OPT_IFS_IF_BAD_TRK_IN)
      {
        p_pidfb->out.status &= ~SUB_STATUS_MASK;
        p_pidfb->out.status |= SUB_IFS;
      }

      /* Apply status option 'Target to Man if BAD TRK_IN_D' ---------------- */
      if (p_pidfb->status_opts & STATUS_OPT_TARG_MAN_IF_BAD_TRK_IN)
      {
        if (!(p_pidfb->mode_blk.target & MODE_MAN))
        {
          if (!p_pidfb_data->new_target_mode)
          {
            p_pidfb_data->new_target_mode = MODE_MAN;
          }
        }
      }
    }
  }

  if (p_pidfb->out.value >= p_pidfb->out_hi_lim)
  {
    p_pidfb->out.value   = p_pidfb->out_hi_lim;
    p_pidfb->out.status |= LIMIT_HIGH;
  }

  else if (p_pidfb->out.value <= p_pidfb->out_lo_lim)
  {
    p_pidfb->out.value   = p_pidfb->out_lo_lim;
    p_pidfb->out.status |= LIMIT_LOW;
  }

  if ((actual_mode >= MODE_AUTO)&&(p_pidfb->out.status & LIMIT_MASK))
  {
    /* Apply BKCAL_HYS ---------------------------------------------------- */
    if ((p_pidfb->out.status & LIMIT_HIGH)  &&
        (p_pidfb->out.value < (p_pidfb->out_hi_lim - p_pidfb_data->out_lim_hys)))
    {
      p_pidfb->out.status &= ~LIMIT_HIGH;
    }
    else if ((p_pidfb->out.status & LIMIT_LOW)  &&
             (p_pidfb->out.value > (p_pidfb->out_lo_lim + p_pidfb_data->out_lim_hys)))
    {
      p_pidfb->out.status &= ~LIMIT_LOW;
    }
  }


  END_OF_OUT_LIM_CHECK:

  /* The following check is required by ITK test case i3300_00 ------------ */
  if (p_pidfb->out_lo_lim >= p_pidfb->out_hi_lim)
  {
    p_pidfb->out.status |= LIMIT_CONST;
  }


  /* Update BKCAL_OUT and RCAS_OUT parameters ------------------------------ */
  if (p_pidfb->control_opts & CNTRL_OPT_USE_PV_FOR_BKCAL_OUT)
  {
    p_pidfb->bkcal_out.value   = p_pidfb->pv.value;
    p_pidfb->rcas_out.value    = p_pidfb->pv.value;

    if (pv_status_quality == SQ_BAD)
    {
      p_pidfb->bkcal_out.status = SQ_BAD;
      p_pidfb->rcas_out.status  = SQ_BAD;
    }
    else
    {
      p_pidfb->bkcal_out.status |= (p_pidfb->pv.status & LIMIT_MASK);
      p_pidfb->rcas_out.status  |= (p_pidfb->pv.status & LIMIT_MASK);
    }
  }
  else /* !(p_pidfb->control_opts & CONTROL_USE_PV_FOR_BKCAL_OUT) */
  {
    p_pidfb->bkcal_out.value = raw_sp.value;
    p_pidfb->rcas_out.value  = raw_sp.value;

    if ((p_pidfb->sp.status & QUALITY_MASK) == SQ_BAD)
    {
      p_pidfb->bkcal_out.status = SQ_BAD;
      p_pidfb->rcas_out.status  = SQ_BAD;
    }
    else
    {
      p_pidfb->bkcal_out.status |= (raw_sp.status & LIMIT_MASK);
      p_pidfb->rcas_out.status  |= (raw_sp.status & LIMIT_MASK);
    }
  }


  /* Update the ROUT_OUT parameter ---------------------------------------- */
  if (actual_mode == MODE_ROUT)
  {
    p_pidfb->rout_out.value   =  p_pidfb->rout_in.value;
  }
  else
  {
    p_pidfb->rout_out.value   =  p_pidfb->out.value;
  }



  /* Apply status option 'IFS if BAD IN' ---------------------------------- */
  if ((p_pidfb->in.status & QUALITY_MASK) == SQ_BAD)
  {
    if ( (p_pidfb->status_opts & STATUS_OPT_IFS_IF_BAD_IN) &&
        ( (p_pidfb->mode_blk.target & MODE_AUTO) &&
         !(p_pidfb->mode_blk.target & MODE_ROUT))             )
    {
      p_pidfb->out.status = SQ_GOOD_CAS|SUB_IFS;
    }
  }


  if ((p_pidfb->cas_in.status & QUALITY_MASK) == SQ_BAD)
  {
    /* Apply status option 'IFS if BAD CAS_IN' ---------------------------- */
    if (  (p_pidfb->status_opts & STATUS_OPT_IFS_IF_BAD_CAS_IN)&&
          (p_pidfb->mode_blk.target & MODE_CAS)                   )
    {
      p_pidfb->out.status = SQ_GOOD_CAS|SUB_IFS;
    }

    /* Apply status option 'Target to Auto if BAD CAS_IN' ----------------- */
    if (  (p_pidfb->status_opts & STATUS_OPT_TARG_NEXT_IF_BAD_CAS_IN)&&
          (p_pidfb->mode_blk.target & MODE_CAS)                   )
    {
      p_pidfb->mode_blk.target = MODE_AUTO;
      p_pidfb_data->mode_was_written = TRUE;
    }
  }


  /* Propagate limit bits in accordance with ITK 5.3 test case i3300_00    */
  bkcal_limit  = (p_pidfb->bkcal_in.status & LIMIT_MASK);
  bkcal_limit |= (p_pidfb->out.status & LIMIT_MASK);

  if (actual_mode == MODE_MAN) bkcal_limit = LIMIT_CONST;

  p_pidfb->rout_out.status &= ~LIMIT_MASK;
  p_pidfb->rout_out.status |= (bkcal_limit & LIMIT_MASK);

  if (p_pidfb->control_opts & CNTRL_OPT_DIRECT_ACTING)
  {
    if      (bkcal_limit == LIMIT_LOW)  bkcal_limit = LIMIT_HIGH;
    else if (bkcal_limit == LIMIT_HIGH) bkcal_limit = LIMIT_LOW;
  }

  bkcal_limit |= (p_pidfb->sp.status & LIMIT_MASK);

  p_pidfb->bkcal_out.status &= ~LIMIT_MASK;
  p_pidfb->bkcal_out.status |= (bkcal_limit & LIMIT_MASK);

  p_pidfb->rcas_out.status &= ~LIMIT_MASK;
  p_pidfb->rcas_out.status |= (bkcal_limit & LIMIT_MASK);


  /* Update block alarm --------------------------------------------------- */
  ffbl_bas_update_extended_block_alarm
    (
      p_block_desc, &p_pidfb->block_alm,
      p_pidfb->alarm_sum.disable, p_pidfb->ack_option,
      p_pidfb->block_err, &p_pidfb_data->old_block_alarm_bits
    );

  if (enable_update_evt)/* Mode OS is left now */
  {
    /* Enable update events ----------------------------------------------- */
    ffbl_enable_update_events (
      p_block_desc->block_id,
      p_block_desc->idx_upd_evt,
      &p_pidfb->update_evt,
      p_pidfb->st_rev
    );
  }

  p_pidfb_data->mode_was_written  = FALSE;

#ifdef _TIMEMEAS
  TP301_LOW();
#endif

  return (p_pidfb->blk_data.next_fb_to_execute);
}







FUNCTION GLOBAL VOID Background_PIDFB
   (
     T_FBIF_BLOCK_INSTANCE *    p_block_instance
   )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

PARAMETERS:
  p_block_instance  Pointer to the function block to be executed

RETURN_VALUES:
  none
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_PIDFB_DATA *              p_pidfb_data;
  T_FBS_WRITE_PARAM_LOCAL     write_loc;

FUNCTION_BODY

  p_pidfb_data  = p_block_instance->p_block_data;

  _ASSERT (p_pidfb_data != NULL);

  if (p_pidfb_data->new_target_mode)
  {
    write_loc.rel_idx       = PID_PAR_MODE_BLK;
    write_loc.subindex      = 1;
    write_loc.length        = 1;
    write_loc.source        = &p_pidfb_data->new_target_mode;
    write_loc.startup_sync  = FALSE;

    fbs_write_param_loc (p_block_instance,&write_loc);

    if ((p_pidfb_data->new_target_mode == MODE_OS) ||
        (p_pidfb_data->new_target_mode == MODE_MAN)  )
    {
      write_loc.rel_idx = PID_PAR_OUT;
    }
    else
    {
      write_loc.rel_idx = 0;
    }

    p_pidfb_data->new_target_mode = 0;

    if (write_loc.rel_idx)
    {
      /* Store out parameter in non-volatile RAM ---------------------- */
      T_PIDFB *  p_pidfb = p_block_instance->p_block_desc->p_block;

      write_loc.subindex      = 0;
      write_loc.length        = 5;
      write_loc.source        = (USIGN8 *)&p_pidfb->out;

      fbs_write_param_loc (p_block_instance,&write_loc);
    }
  }

  return;
}






FUNCTION LOCAL VOID Calc_block_mode_PIDFB
  (
    T_PIDFB *       p_pidfb,
    T_PIDFB_DATA *  p_pidfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  mo.profile             = PID_PROFILE_NUMBER;
  mo.mode_was_written    = p_pidfb_data->mode_was_written;
  mo.period_of_execution = p_pidfb->blk_data.period_of_execution;
  mo.mode                = p_pidfb->mode_blk;
  mo.shed                = p_pidfb_data->shed;
  mo.shed_opt            = p_pidfb->shed_opt;
  mo.control_opts        = p_pidfb->control_opts;
  mo.status_opts         = p_pidfb->status_opts;
  mo.in_status           = p_pidfb->in.status;
  mo.cas_in_status       = p_pidfb->cas_in.status;
  mo.rcas_in_status      = p_pidfb->rcas_in.status;
  mo.rout_in_status      = p_pidfb->rout_in.status;
  mo.cas_out_status      = p_pidfb->bkcal_out.status;
  mo.rcas_out_status     = p_pidfb->rcas_out.status;
  mo.rout_out_status     = p_pidfb->rout_out.status;
  mo.bkcal_in_status     = p_pidfb->bkcal_in.status;
  mo.out_status          = p_pidfb->out.status & ~LIMIT_MASK;
  mo.pid_downstream      = p_pidfb_data->downstream;
  mo.tracking            = p_pidfb_data->tracking;
  mo.trk_in_d            = p_pidfb->trk_in_d;

  if (p_pidfb->bypass == BYPASS_ON) mo.in_status = SQ_GOOD_NC;

  Calc_block_mode ();

  p_pidfb->mode_blk               = mo.mode;
  p_pidfb_data->shed              = mo.shed;
  p_pidfb_data->tracking          = mo.tracking;
  p_pidfb_data->downstream        = mo.pid_downstream;
  p_pidfb->bkcal_out.status       = mo.cas_out_status;
  p_pidfb->rcas_out.status        = mo.rcas_out_status;
  p_pidfb->rout_out.status        = mo.rout_out_status;

  if (!p_pidfb_data->new_target_mode && mo.new_target_mode)
  {
    p_pidfb_data->new_target_mode = mo.new_target_mode;
  }

  return;
}






FUNCTION LOCAL VOID Reset_PIDFB
  (
    T_PIDFB_DATA *   p_pidfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Reset block specific data of a PID block.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  p_pidfb_data->mode_was_written       = FALSE;
  p_pidfb_data->new_target_mode        = 0;
  p_pidfb_data->shed.active            = FALSE;
  p_pidfb_data->shed.current_time      = 0;
  p_pidfb_data->control_active         = FALSE;
  p_pidfb_data->working_trk_val.status = SQ_BAD;
  p_pidfb_data->last_sp.status         = SQ_BAD;
  p_pidfb_data->retained_sp.status     = SQ_BAD;
  p_pidfb_data->feed_forward.status    = SQ_BAD;
  p_pidfb_data->downstream             = PID_DOWNSTREAM_NOT_ACTIVE;

  return;
}






FUNCTION LOCAL VOID Clear_alarms_PIDFB
  (
    T_PIDFB * p_pidfb,
    USIGN16   block_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  alm.block_id = block_id;
  alm.disabled = TRUE;
  alm.value    = p_pidfb->pv.value;

  /* LO-LO-ALM clear */
  if (p_pidfb->lo_lo_alm.current == ALARM_ACTIVE)
  {
    alm.ptr  = &p_pidfb->lo_lo_alm;
    alm.idx  = PID_PAR_LO_LO_ALM;
    alm.pri  = p_pidfb->lo_lo_pri;

    Handle_act_alarm ();
  }


  /* LO-ALM clear */
  if (p_pidfb->lo_alm.current == ALARM_ACTIVE)
  {
    alm.ptr  = &p_pidfb->lo_alm;
    alm.idx  = PID_PAR_LO_ALM;
    alm.pri  = p_pidfb->lo_pri;

    Handle_act_alarm ();
  }


  /* HI-HI-ALM clear */
  if (p_pidfb->hi_hi_alm.current == ALARM_ACTIVE)
  {
    alm.ptr  = &p_pidfb->hi_hi_alm;
    alm.idx  = PID_PAR_HI_HI_ALM;
    alm.pri  = p_pidfb->hi_hi_pri;

    Handle_act_alarm ();
  }


  /* HI-ALM clear */
  if (p_pidfb->hi_alm.current == ALARM_ACTIVE)
  {
    alm.ptr  = &p_pidfb->hi_alm;
    alm.idx  = PID_PAR_HI_ALM;
    alm.pri  = p_pidfb->hi_pri;

    Handle_act_alarm ();
  }


  alm.value    = p_pidfb->pv.value - p_pidfb->sp.value;

  /* DV_HI_ALM clear */
  if (p_pidfb->dv_hi_alm.current == ALARM_ACTIVE)
  {
    alm.ptr  = &p_pidfb->dv_hi_alm;
    alm.idx  = PID_PAR_DV_HI_ALM;
    alm.pri  = p_pidfb->dv_hi_pri;

    Handle_act_alarm ();
  }


  /* DV_LO_ALM clear */
  if (p_pidfb->dv_lo_alm.current == ALARM_ACTIVE)
  {
    alm.ptr  = &p_pidfb->dv_lo_alm;
    alm.idx  = PID_PAR_DV_LO_ALM;
    alm.pri  = p_pidfb->dv_lo_pri;

    Handle_act_alarm ();
  }

  return;
}







FUNCTION LOCAL VOID Update_alarm_sum_PIDFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_PIDFB *    p_pidfb;

FUNCTION_BODY

  p_pidfb  = p_block_instance->p_block_desc->p_block;

  /* Update the 'current' bitfield ---------------------------------------- */
  p_pidfb->alarm_sum.current = 0;

  if (p_pidfb->block_err)
    p_pidfb->alarm_sum.current |= ALARM_SUM_BLOCK;

  if (p_pidfb->lo_alm.current == ALARM_ACTIVE)
       p_pidfb->alarm_sum.current |= ALARM_SUM_LO;

  if (p_pidfb->lo_lo_alm.current == ALARM_ACTIVE)
       p_pidfb->alarm_sum.current |= ALARM_SUM_LO_LO;

  if (p_pidfb->hi_alm.current == ALARM_ACTIVE)
       p_pidfb->alarm_sum.current |= ALARM_SUM_HI;

  if (p_pidfb->hi_hi_alm.current == ALARM_ACTIVE)
       p_pidfb->alarm_sum.current |= ALARM_SUM_HI_HI;

  if (p_pidfb->dv_lo_alm.current == ALARM_ACTIVE)
       p_pidfb->alarm_sum.current |= ALARM_SUM_DEV_LO;

  if (p_pidfb->dv_hi_alm.current == ALARM_ACTIVE)
       p_pidfb->alarm_sum.current |= ALARM_SUM_DEV_HI;

  p_pidfb->alarm_sum.current &= ~p_pidfb->alarm_sum.disable;


  /* Update the 'unrep' bitfield ------------------------------------------ */
  p_pidfb->alarm_sum.unrep = 0;

  if (_ALARM_NOT_REPORTED(p_pidfb->block_alm.state))
       p_pidfb->alarm_sum.unrep |= ALARM_SUM_BLOCK;

  if (_ALARM_NOT_REPORTED(p_pidfb->lo_alm.state))
       p_pidfb->alarm_sum.unrep |= ALARM_SUM_LO;

  if (_ALARM_NOT_REPORTED(p_pidfb->lo_lo_alm.state))
       p_pidfb->alarm_sum.unrep |= ALARM_SUM_LO_LO;

  if (_ALARM_NOT_REPORTED(p_pidfb->hi_alm.state))
       p_pidfb->alarm_sum.unrep |= ALARM_SUM_HI;

  if (_ALARM_NOT_REPORTED(p_pidfb->hi_hi_alm.state))
       p_pidfb->alarm_sum.unrep |= ALARM_SUM_HI_HI;


  if (_ALARM_NOT_REPORTED(p_pidfb->dv_lo_alm.state))
       p_pidfb->alarm_sum.unrep |= ALARM_SUM_DEV_LO;

  if (_ALARM_NOT_REPORTED(p_pidfb->dv_hi_alm.state))
       p_pidfb->alarm_sum.unrep |= ALARM_SUM_DEV_HI;

  p_pidfb->alarm_sum.unrep &= ~p_pidfb->alarm_sum.disable;


  /* Update the 'unack' bitfield ------------------------------------------ */
  p_pidfb->alarm_sum.unack = 0;

  if (_ALARM_NOT_ACKNOWLEDGED(p_pidfb->block_alm.unack))
       p_pidfb->alarm_sum.unack |= ALARM_SUM_BLOCK;

  if (_ALARM_NOT_ACKNOWLEDGED(p_pidfb->lo_alm.unack))
       p_pidfb->alarm_sum.unack |= ALARM_SUM_LO;

  if (_ALARM_NOT_ACKNOWLEDGED(p_pidfb->lo_lo_alm.unack))
       p_pidfb->alarm_sum.unack |= ALARM_SUM_LO_LO;

  if (_ALARM_NOT_ACKNOWLEDGED(p_pidfb->hi_alm.unack))
       p_pidfb->alarm_sum.unack |= ALARM_SUM_HI;

  if (_ALARM_NOT_ACKNOWLEDGED(p_pidfb->hi_hi_alm.unack))
       p_pidfb->alarm_sum.unack |= ALARM_SUM_HI_HI;

  if (_ALARM_NOT_ACKNOWLEDGED(p_pidfb->dv_lo_alm.unack))
       p_pidfb->alarm_sum.unack |= ALARM_SUM_DEV_LO;

  if (_ALARM_NOT_ACKNOWLEDGED(p_pidfb->dv_hi_alm.unack))
       p_pidfb->alarm_sum.unack |= ALARM_SUM_DEV_HI;

  p_pidfb->alarm_sum.unack &= ~p_pidfb->alarm_sum.disable;

  return;
}






FUNCTION LOCAL VOID Check_config_PIDFB
  (
    IN T_PIDFB *      p_pidfb
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN32     block_err_desc;

  FLOAT period_of_exec = p_pidfb->blk_data.period_of_execution / 32000.0f;

FUNCTION_BODY

  block_err_desc = BLK_ERR_NO_ERROR;

  if (p_pidfb->blk_data.period_of_execution == 0)
  {
    block_err_desc |= BLK_ERR_EXE_TIME;
  }

  if ((p_pidfb->mode_blk.target & MODE_RCAS)&&(p_pidfb->shed_opt == 0))
  {
    block_err_desc |= BLK_ERR_SHED_OPT_SET;
  }

  if ((p_pidfb->mode_blk.target & MODE_ROUT)&&(p_pidfb->shed_opt == 0))
  {
    block_err_desc |= BLK_ERR_SHED_OPT_SET;
  }

  if (p_pidfb->bypass == 0)
  {
    block_err_desc |= BLK_ERR_BYPASS_SET;
  }

  /* Make sure that eu_0 is equal or less than eu_100 --------------------- */
  if (p_pidfb->pv_scale.eu_100  <= p_pidfb->pv_scale.eu_0 )
  {
    block_err_desc |= BLK_ERR_PV_SCALE_LIM_SET;
  }
  if (p_pidfb->out_scale.eu_100 <= p_pidfb->out_scale.eu_0)
  {
    block_err_desc |= BLK_ERR_OUT_SCALE_LIM_SET;
  }
  if (p_pidfb->trk_scale.eu_100 <= p_pidfb->trk_scale.eu_0)
  {
    block_err_desc |= BLK_ERR_TRK_SCALE_LIM_SET;
  }
  if (p_pidfb->ff_scale.eu_100  <= p_pidfb->ff_scale.eu_0)
  {
    block_err_desc |= BLK_ERR_FF_SCALE_LIM_SET;
  }

  /* Make sure that lo_lim is equal or less then hi_lim ------------------- */
  if (p_pidfb->sp_hi_lim   < p_pidfb->sp_lo_lim )
  {
    block_err_desc |= BLK_ERR_SP_LIM_SET;
  }
  if (p_pidfb->out_hi_lim  < p_pidfb->out_lo_lim)
  {
    block_err_desc |= BLK_ERR_OUT_LIM_SET;
  }

  /* make sure that RATE and RESET is equal or greater then block execution period */
  if (
          ((0.0 < p_pidfb->rate) && (p_pidfb->rate < (period_of_exec * BLK_ERRTIMEERRMIN)))
          ||
          ((0.0 < p_pidfb->reset) && (p_pidfb->reset < (period_of_exec * BLK_ERRTIMEERRMIN)))
          )
  {
      block_err_desc |= BLK_ERR_TIMECONSTERR;
  }
  
  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_pidfb->block_err &= ~BLK_ERR_BLOCK_CONFIG_ERR;
  }
  else
  {
    p_pidfb->block_err |= BLK_ERR_BLOCK_CONFIG_ERR;
  }

  p_pidfb->block_err_desc_1 = block_err_desc;

  return;
}







FUNCTION LOCAL VOID Calc_timer_values_PIDFB
  (
    IN T_PIDFB *      p_pidfb,
    IN T_PIDFB_DATA * p_pidfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY


  if (p_pidfb->blk_data.period_of_execution != 0)
  {
    FLOAT period_of_exec = p_pidfb->blk_data.period_of_execution / 32000.0f;

    p_pidfb_data->D_factor = p_pidfb->rate / period_of_exec;
    p_pidfb_data->kd    = p_pidfb_data->D_factor;/* ch */

    if ((p_pidfb->reset == PID_RESET_OFF) || ((*((FLOAT *)minusInf)) == p_pidfb->reset) || ((*((FLOAT *)plusInf)) == p_pidfb->reset))/* bugreport 17657 */
    {
           p_pidfb_data->I_factor = 0.0;
        p_pidfb_data->last_I_sum = (FLOAT)0.0;
        p_pidfb_data->I_sum = (FLOAT)0.0;
    }
    else
    {    
        if (p_pidfb->reset > PID_RESET_MIN)
        {
          p_pidfb_data->I_factor = period_of_exec / p_pidfb->reset;
        }
        else
        {
          p_pidfb_data->I_factor = period_of_exec / PID_RESET_MIN;
        }
    }

	p_pidfb_data->ki    = p_pidfb_data->I_factor;/* ch */

    p_pidfb_data->pv_factor = period_of_exec / (period_of_exec + p_pidfb->pv_ftime);

    p_pidfb_data->B_time    = p_pidfb->bal_time / period_of_exec;
    p_pidfb_data->bal_timer = p_pidfb_data->B_time;

    p_pidfb_data->period_of_exec = period_of_exec;
	if (0.0 != p_pidfb_data->tfpid_factor)/* ch */
	{
		p_pidfb->tfpid = p_pidfb_data->period_of_exec / p_pidfb_data->tfpid_factor;
	}
  }
  else
  {
    p_pidfb_data->period_of_exec = 0.0f;
  }

  return;
}

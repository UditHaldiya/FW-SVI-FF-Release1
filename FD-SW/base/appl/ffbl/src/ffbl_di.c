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

FILE_NAME          ffbl_di.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKDI)

INCLUDES

  /*@Dresser-Masoneilan project */
  #include <string.h>
  
  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_di.h"
  #include "ffbl_res.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

  FUNCTION LOCAL VOID Check_config_DIFB (T_DIFB *, T_DIFB_DATA *, const T_FBIF_BLOCK_DESCR *);
  FUNCTION LOCAL VOID Clear_alarms_DIFB (T_DIFB *, USIGN16);
  FUNCTION LOCAL VOID Update_block_alarm_DIFB (const T_FBIF_BLOCK_DESCR *, T_DIFB *, T_DIFB_DATA *);
  FUNCTION LOCAL VOID Update_alarm_sum_DI (T_FBIF_BLOCK_INSTANCE *);
  FUNCTION LOCAL USIGN8 Update_good_nc_substatus_DIFB (T_FBIF_BLOCK_INSTANCE *);

IMPORT_DATA
#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT extern T_FBS_ALERT_NOTIFY    ffbl_alert_notify;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

EXPORT_DATA

LOCAL_DATA


/****************************************************************************/

FUNCTION GLOBAL VOID Start_DIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_DIFB_DATA *               p_difb_data;
  T_DIFB *                    p_difb;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_DIFB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_difb      = p_block_instance->p_block_desc->p_block;
  p_difb_data = p_block_instance->p_block_data;


  if (p_difb->blk_data.period_of_execution && (p_difb->pv_ftime != 0))
  {
    p_difb_data->pv_time_d = (USIGN16)((32000.0f * p_difb->pv_ftime) / p_difb->blk_data.period_of_execution);
  }
  else
  {
    p_difb_data->pv_time_d = 0;
  }

  p_difb_data->pv_time_counter      = 0;
  p_difb_data->old_block_alarm_bits = 0;

  /* Store out param in the internal block-specific data structure -------- */
  p_difb_data->out_d    = p_difb->out_d;

  /* Set out status to BAD out-of-service --------------------------------- */
  p_difb->out_d.status  = SQ_BAD|SUB_OUT_OF_SERVICE;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_difb->update_evt);


  /* Check function block configuration ----------------------------------- */
  Check_config_DIFB (p_difb,p_difb_data,p_block_instance->p_block_desc);

  return;
}




FUNCTION GLOBAL VOID Stop_DIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_DIFB_DATA *               p_difb_data;
  T_DIFB *                    p_difb;

FUNCTION_BODY

  p_block_desc = p_block_instance->p_block_desc;
  p_difb_data  = p_block_instance->p_block_data;
  p_difb       = p_block_desc->p_block;

  if (p_difb->mode_blk.actual == MODE_OS) return;

  p_difb->mode_blk.actual = MODE_OS;

  /* Set block error ------------------------------------------------------ */
  p_difb->block_err &= ~BLK_ERR_INPUT_FAILURE;
  p_difb->block_err |= BLK_ERR_OUT_OF_SERVICE;


  /* Clear all alarms ----------------------------------------------------- */
  Clear_alarms_DIFB (p_difb,p_block_desc->block_id);

  /* Store out param in the internal block-specific data structure -------- */
  p_difb_data->out_d      = p_difb->out_d;

  if (p_difb->block_err & BLK_ERR_BLOCK_CONFIG_ERR)
  {
     /* Set status of the OUT parameter to BAD|SUB_CONFIGURATION_ERROR ---- */
     p_difb->out_d.status = SQ_BAD|SUB_CONFIGURATION_ERROR;
     p_difb->pv_d.status  = SQ_BAD|SUB_CONFIGURATION_ERROR;
  }
  else
  {
     /* Set status of the OUT parameter to BAD|SUB_OUT_OF_SERVICE --------- */
     p_difb->out_d.status = SQ_BAD|SUB_OUT_OF_SERVICE;
     p_difb->pv_d.status  = SQ_BAD|SUB_OUT_OF_SERVICE;
  }

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_difb->update_evt);


  /* Update block alarm --------------------------------------------------- */
  Update_block_alarm_DIFB (p_block_desc, p_difb,
          (T_DIFB_DATA *)p_block_instance->p_block_data);

  return;
}





FUNCTION GLOBAL USIGN16 Read_handler_DIFB
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

  /* Check and/or update parameter to be read ------------------------------- */
  if (p_read->rel_idx == DI_PAR_ALARM_SUM)
  {
    Update_alarm_sum_DI (p_block_instance);
  }

  return (E_OK);
}




FUNCTION GLOBAL USIGN16 Write_handler_DIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBIF_WRITE_DATA *         p_write
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Return  E_OK    or a neg. return code defined in fbap.h
----------------------------------------------------------------------------*/

{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_DIFB_DATA *               p_difb_data;
  T_DIFB *                    p_difb;
  USIGN8                      subindex;
  USIGN8 *                    source;
  USIGN8                      result;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_difb_data   = p_block_instance->p_block_data;
  p_difb        = p_block_desc->p_block;

  subindex      = p_write->subindex;
  source        = p_write->source;
  result        = E_OK;

  switch (p_write->rel_idx)
  {
    case DI_PAR_ALERT_KEY:
    {
      if (*source == 0) result = E_FB_PARA_CHECK;

      break;
    }

    case DI_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (
                 &p_difb->mode_blk,
                 subindex,
                 source,
                 MODE_DI_VALID
               );
      break;
    }

    case DI_PAR_OUT_D:
    {
      if (MODE_LOWER_THAN_MAN (p_difb->mode_blk.target))
         return (E_FB_WRONG_MODE);

      if (subindex == 1) return (E_FB_DATA_NO_WRITE);

      break;
    }

    case DI_PAR_SIMULATE_D:
    {
      USIGN8 enable_flag;

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //if      (subindex == 0) enable_flag = ((SIMULATE_DISCRETE*)source)->enable;
      if      (subindex == 0) enable_flag = source[4];
      else if (subindex == 5) enable_flag = *source;
      else                    break;

      if (ffbl_bas_check_sim_param (enable_flag) == E_FB_PARA_CHECK)
        return (E_FB_PARA_CHECK);

      break;
    }

    case DI_PAR_GRANT_DENY:
    {
      ffbl_bas_write_grant_deny (subindex,source,&p_difb->grant_deny);

      break;
    }

    case DI_PAR_IO_OPTS:
    {
      if (MODE_NOT_OS (p_difb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //*((USIGN16 *)source) &= DI_IO_OPTS; /* Delete all undefined bits */
      WPHB_(source) &= CWHB_(DI_IO_OPTS); /* Delete all undefined bits */
      WPLB_(source) &= CWLB_(DI_IO_OPTS); /* Delete all undefined bits */

      break;
    }

    case DI_PAR_STATUS_OPTS:
    {
      if (MODE_NOT_OS (p_difb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //*((USIGN16 *)source) &= DI_STATUS_OPTS;  /* Delete all undefined bits */
      WPHB_(source) &= CWHB_(DI_STATUS_OPTS);  /* Delete all undefined bits */
      WPLB_(source) &= CWLB_(DI_STATUS_OPTS);  /* Delete all undefined bits */

      break;
    }

    case DI_PAR_CHANNEL:
    {
      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      USIGN16   u16value;

      if (MODE_NOT_OS (p_difb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      memcpy (&u16value, source, sizeof (u16value));
      //if ( ffbl_bas_check_channel (*(USIGN16 *)source, p_block_desc))
      if ( ffbl_bas_check_channel (u16value, p_block_desc))
        return E_FB_PARA_CHECK;

      break;
     }

    case DI_PAR_PV_FTIME:
    {
    if (*((FLOAT *)source)<0) return (E_FB_PARA_LIMIT);

    break;
    }

    case DI_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_difb->update_evt.unack, source, subindex);

      break;
    }

    case DI_PAR_BLOCK_ALM:
    {
      result = ffbl_check_unack_flag (&p_difb->block_alm.unack, source, subindex);

      break;
    }

    case DI_PAR_DISC_PRI:
    {
      if (*source > WRITE_PRI_MAX) return (E_FB_PARA_LIMIT);

      break;
    }

    case DI_PAR_DISC_ALM:
    {
      result = ffbl_check_unack_flag (&p_difb->disc_alm.unack,
                                             source,subindex);
      break;
    }
  }


  if (result == E_OK)
  {
    result = fbs_write_param (p_block_instance,p_write);
  }


  if (result == E_OK)
  {
    BOOL check_config = FALSE;
    BOOL calc_pvt = FALSE;

    switch (p_write->rel_idx)
    {
      case DI_PAR_BLK_DATA:
      {
        check_config  = TRUE;
        calc_pvt      = TRUE;

        break;
      }

      case DI_PAR_MODE_BLK:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        write_loc.rel_idx = 0;

        if (p_difb->mode_blk.target & MODE_OS)
        {
          p_difb->out_d.status = SQ_BAD|SUB_OUT_OF_SERVICE;

          write_loc.rel_idx = DI_PAR_OUT_D;
        }

        else if (p_difb->mode_blk.target & MODE_MAN)
        {
          write_loc.rel_idx = DI_PAR_OUT_D;
        }

        if (write_loc.rel_idx)
        {
          /* Store out parameter in non-volatile RAM ---------------------- */
          write_loc.subindex      = 0;
          write_loc.length        = 2;
          write_loc.source        = (USIGN8 *)&p_difb->out_d;
          write_loc.startup_sync  = FALSE;

          fbs_write_param_loc (p_block_instance,&write_loc);
        }

        break;
      }

      case DI_PAR_OUT_D:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        p_difb->out_d.status = SQ_GOOD_NC;

        /* Store GOOD out status in non-volatile RAM ---------------------- */
        write_loc.rel_idx       = DI_PAR_OUT_D;
        write_loc.subindex      = 0;
        write_loc.length        = 2;
        write_loc.source        = (USIGN8 *)&p_difb->out_d;
        write_loc.startup_sync  = FALSE;

        fbs_write_param_loc (p_block_instance,&write_loc);

        /* Store out param in the internal block-specific data structure -- */
        p_difb_data->out_d = p_difb->out_d;

        /* Adapt out status in accordance to the actual mode -------------- */
        if (p_difb->mode_blk.actual == MODE_OS)
        {
          p_difb->out_d.status = SQ_BAD|SUB_OUT_OF_SERVICE;
        }
        else
        {
          p_difb->out_d.status |= LIMIT_CONST;
        }

        break;
      }

      case DI_PAR_CHANNEL:
      {
        check_config = TRUE;    break;
      }

      case DI_PAR_PV_FTIME:
      {
        calc_pvt = TRUE;        break;
      }
    }

    if (check_config)
    {
      Check_config_DIFB(p_difb,p_difb_data,p_block_desc);
    }

    if (calc_pvt)
    {
      if (p_difb->blk_data.period_of_execution && (p_difb->pv_ftime != 0))
      {
        p_difb_data->pv_time_d = (USIGN16)((32000.0f * p_difb->pv_ftime) / p_difb->blk_data.period_of_execution);
      }
      else
      {
        p_difb_data->pv_time_d = 0;
      }

      p_difb_data->pv_time_counter = 0;
    }

  }

return result;
}







FUNCTION GLOBAL USIGN16 Execute_DIFB
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
  T_DIFB_DATA *               p_difb_data;
  T_DIFB *                    p_difb;
  USIGN8                      old_mode;
  USIGN8                      actual_mode;
  DISCRETE_S                  raw_value;
  BOOL                        enable_update_evt;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_difb        = p_block_instance->p_block_desc->p_block;
  p_difb_data   = p_block_instance->p_block_data;

  /* Update simulate parameter -------------------------------------------- */
  ffbl_bas_update_sim_param (&p_difb->simulate_d.enable,&p_difb->block_err);

  /* Update block mode -------------------------------------------------- */
  old_mode    = p_difb->mode_blk.actual;
  actual_mode = ffbl_bas_calc_block_mode (&p_difb->mode_blk,p_difb->block_err);

  if (actual_mode == MODE_OS)
  {
    if (old_mode != MODE_OS)
    {
      /* Mode OS is entered now ------------------------------------------- */
      Stop_DIFB (p_block_instance);
    }
    else
    {
      /* Update block alarm ----------------------------------------------- */
      Update_block_alarm_DIFB (p_block_desc, p_difb, p_difb_data);
    }

    return(p_difb->blk_data.next_fb_to_execute);
  }

  p_difb->mode_blk.actual = actual_mode;

  if (old_mode == MODE_OS) /* Mode OS is left now ------------------------- */
  {
    /* Reset block error -------------------------------------------------- */
    p_difb->block_err &= ~(BLK_ERR_OUT_OF_SERVICE|BLK_ERR_BLOCK_CONFIG_ERR);

    enable_update_evt = TRUE;

    /* Restore out parameter ---------------------------------------------- */
    p_difb->out_d = p_difb_data->out_d;

    if ((p_difb->out_d.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_BAD|SUB_OUT_OF_SERVICE))
    {
      p_difb->out_d.status = SQ_BAD;
    }
  }
  else
  {
    enable_update_evt = FALSE;
  }


  /* Get raw input value -------------------------------------------------- */
  if (p_difb->simulate_d.enable == SIMULATION_DISABLED)
  {
    /* Execute the transducer block --------------------------------------- */
    p_difb_data->exec_tb.p_process_value = &raw_value;
    p_difb_data->exec_tb.tb_error        = FALSE;

    (*p_difb_data->linked_tb.a_of_execute_tb)(
            p_difb_data->linked_tb.p_tb_instance,
            &p_difb_data->exec_tb
         );

    /* Store FIELD_VAL_D ------------------------------------------------------ */
    p_difb->simulate_d.field_status = raw_value.status;
    p_difb->simulate_d.field_value  = raw_value.value;


    /* Copy field value to simulate parameter owing to interop. test 2705_00 */
    p_difb->simulate_d.status   = raw_value.status;
    p_difb->simulate_d.value    = raw_value.value;


  }
  else /* p_difb->simulate_d.enable == SIMULATION_ENABLED */
  {
    raw_value.status = p_difb->simulate_d.status;
    raw_value.value  = p_difb->simulate_d.value;
  }


  /* Store FIELD_VAL_D ------------------------------------------------------ */
  p_difb->field_val_d.status = raw_value.status;
  p_difb->field_val_d.value = raw_value.value;


  /* Apply invert IO option ------------------------------------------------- */
  if (p_difb->io_opts & IO_OPT_INVERT)
  {
     if (raw_value.value)
          raw_value.value = 0;
     else raw_value.value = 1;
  }


  if (p_difb_data->pv_time_d)
  {
    if (((p_difb->pv_d.status & QUALITY_MASK) == SQ_GOOD_NC) &&
        ((raw_value.status    & QUALITY_MASK) == SQ_GOOD_NC)    )
    {
      if (p_difb->pv_d.value != raw_value.value)
      {
        if (++p_difb_data->pv_time_counter >= p_difb_data->pv_time_d)
        {
          p_difb->pv_d = raw_value;

          p_difb_data->pv_time_counter = 0;
        }
      }
      else /* p_difb->pv_d.value == raw_value.value */
      {
        p_difb_data->pv_time_counter = 0;
      }
    }
    else
    {
      p_difb->pv_d = raw_value;
    }
  }
  else /* (p_difb_data->pv_time_d == 0) */
  {
    p_difb->pv_d = raw_value;
  }


  if ((raw_value.status & QUALITY_MASK) != SQ_BAD)
  {
    /* Input from transducer block is not BAD ------------------------------- */
    if (p_difb->block_err & BLK_ERR_INPUT_FAILURE)
    {
      p_difb->block_err &= ~BLK_ERR_INPUT_FAILURE;
    }
  }
  else
  {
    /* Set input failure bit if input from transducer block is BAD -------- */
    if (!(p_difb->block_err & BLK_ERR_INPUT_FAILURE))
    {
      p_difb->block_err |= BLK_ERR_INPUT_FAILURE;
    }
  }


  /* Determine status and substatus of the out_d parameter -------------------- */
  if ((raw_value.status & QUALITY_MASK) == SQ_GOOD_NC)
  {
    raw_value.status &= ~SUB_STATUS_MASK;
    raw_value.status |= Update_good_nc_substatus_DIFB (p_block_instance);
  }

  if (p_difb->mode_blk.actual == MODE_AUTO)
  {
    p_difb->out_d = p_difb->pv_d;

    if ((p_difb->pv_d.status & QUALITY_MASK) == SQ_BAD)
    {
      if (((p_difb->pv_d.status & SUB_STATUS_MASK) != SUB_DEVICE_FAILURE) &&
          ((p_difb->pv_d.status & SUB_STATUS_MASK) != SUB_SENSOR_FAILURE)    )
      {
        p_difb->out_d.status = SQ_BAD|SUB_NON_SPECIFIC;
      }
    }
  }
  else if (p_difb->mode_blk.actual == MODE_MAN)
  {
    /* Apply status option 'Uncertain if Man mode' ------------------------ */
    if (p_difb->status_opts & STATUS_OPT_UNCERT_IF_MAN)
    {
      if ((p_difb->out_d.status & QUALITY_MASK) & SQ_GOOD_NC)
      {
        p_difb->out_d.status = SQ_UNCERTAIN | SUB_NON_SPECIFIC;
      }
    }

    if ((p_difb->out_d.status & QUALITY_MASK) != SQ_BAD)
    {
      p_difb->out_d.status |= LIMIT_CONST;
    }
  }

  /* Alarm checks ------------------------------------------------------- */
  if (p_difb->disc_alm.current == ALARM_CLEAR)
  {
    if ( (p_difb->out_d.value == p_difb->disc_lim)    &&
        !(p_difb->alarm_sum.disable & ALARM_SUM_DISC) &&
          p_difb->disc_pri                                )
    {
      ffbl_alert_notify.action              = ALARM_ACTIVE;
      ffbl_alert_notify.block_id            = p_block_desc->block_id;
      ffbl_alert_notify.rel_idx             = DI_PAR_DISC_ALM;
      ffbl_alert_notify.priority            = p_difb->disc_pri;
      ffbl_alert_notify.data.dsc.subcode    = 0;
      ffbl_alert_notify.data.dsc.value      = p_difb->out_d.value;
      ffbl_alert_notify.data.dsc.mbit_mask  = 0;

      fbs_alert_notify (&ffbl_alert_notify);

      if ((p_difb->ack_option & ALARM_SUM_DISC) || (p_difb->disc_pri == 1))
           p_difb->disc_alm.unack = ALARM_ACKED;
      else p_difb->disc_alm.unack = ALARM_UNACKED;
    }
  }
  else /* p_difb->disc_alm.current == ALARM_ACTIVE */
  {
    if ((p_difb->out_d.value != p_difb->disc_lim)    ||
        (p_difb->alarm_sum.disable & ALARM_SUM_DISC) ||
        (p_difb->disc_pri == 0)                         )
    {
      ffbl_alert_notify.action              = ALARM_CLEAR;
      ffbl_alert_notify.block_id            = p_block_desc->block_id;
      ffbl_alert_notify.rel_idx             = DI_PAR_DISC_ALM;
      ffbl_alert_notify.priority            = p_difb->disc_pri;
      ffbl_alert_notify.data.dsc.subcode    = 0;
      ffbl_alert_notify.data.dsc.value      = p_difb->out_d.value;
      ffbl_alert_notify.data.dsc.mbit_mask  = 0;

      fbs_alert_notify (&ffbl_alert_notify);
    }
  }

  /* Update block alarm --------------------------------------------------- */
  Update_block_alarm_DIFB (p_block_desc, p_difb, p_difb_data);

  if (enable_update_evt)
  {
    /* Enable update events ----------------------------------------------- */
    ffbl_enable_update_events
    (
      p_block_desc->block_id,
      p_block_desc->idx_upd_evt,
      &p_difb->update_evt,
      p_difb->st_rev
    );
  }

  return(p_difb->blk_data.next_fb_to_execute);
}







FUNCTION GLOBAL VOID Background_DIFB
   (
     IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
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






FUNCTION LOCAL USIGN8 Update_good_nc_substatus_DIFB
   (
     IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
   )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_DIFB *       p_difb;
  USIGN8         substatus;

FUNCTION_BODY

  p_difb    = p_block_instance->p_block_desc->p_block;
  substatus = SUB_NON_SPECIFIC;

  Update_alarm_sum_DI (p_block_instance);

  if (p_difb->alarm_sum.unack)
  {
    if (p_difb->alarm_sum.unack & ALARM_SUM_DISC)
    {
      if (p_difb->disc_pri < 8)
      {
        substatus = SUB_UNACK_ADVISORY_ALARM;
      }
      else
      {
        substatus = SUB_UNACK_CRITICAL_ALARM;

        return substatus;
      }
    }

    if (substatus < SUB_UNACK_ADVISORY_ALARM)
    {
      substatus = SUB_UNACK_BLOCK_ALARM;
    }
  }
  else if (p_difb->alarm_sum.current)
  {
    if (p_difb->alarm_sum.current & ALARM_SUM_DISC)
    {
      if (p_difb->disc_pri < 8)
      {
        substatus = SUB_ACTIVE_ADVISORY_ALARM;
      }
      else
      {
        substatus = SUB_ACTIVE_CRITICAL_ALARM;

        return substatus;
      }
    }

    if (substatus < SUB_UNACK_ADVISORY_ALARM)
    {
      substatus = SUB_ACTIVE_BLOCK_ALARM;
    }
  }

  return substatus;
}







FUNCTION LOCAL VOID Update_block_alarm_DIFB
  (
    IN const T_FBIF_BLOCK_DESCR * p_block_desc,
    INOUT T_DIFB *                p_difb,
    INOUT T_DIFB_DATA *           p_difb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16   alarm_bits;

FUNCTION_BODY

  if (p_difb->block_err & BLK_ERR_OUT_OF_SERVICE)
  {
    alarm_bits = BLK_ERR_OUT_OF_SERVICE;
  }
  else
  {
    alarm_bits = p_difb->block_err;

    if (p_difb->status_opts & STATUS_OPT_PROP_FLT_FORW)
      {
        /* No input failure alarm ----------------------------------------- */
        alarm_bits &= ~BLK_ERR_INPUT_FAILURE;
      }
  }

  ffbl_bas_update_extended_block_alarm
  (
    p_block_desc, &p_difb->block_alm,
    p_difb->alarm_sum.disable, p_difb->ack_option,
    alarm_bits, &p_difb_data->old_block_alarm_bits
  );
}








FUNCTION LOCAL VOID Update_alarm_sum_DI
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_DIFB *    p_difb;

FUNCTION_BODY

  p_difb  = p_block_instance->p_block_desc->p_block;

  /* Update the 'current' bitfield ---------------------------------------- */
  p_difb->alarm_sum.current = 0;

  if (p_difb->block_err)
       p_difb->alarm_sum.current |= ALARM_SUM_BLOCK;

  if (p_difb->disc_alm.current == ALARM_ACTIVE )
       p_difb->alarm_sum.current |= ALARM_SUM_DISC;

  p_difb->alarm_sum.current &= ~p_difb->alarm_sum.disable;


  /* Update the 'unreported' bitfield ------------------------------------- */
  p_difb->alarm_sum.unrep = 0;

  if (_ALARM_NOT_REPORTED(p_difb->block_alm.state))
       p_difb->alarm_sum.unrep |= ALARM_SUM_BLOCK;

  if (_ALARM_NOT_REPORTED(p_difb->disc_alm.state))
       p_difb->alarm_sum.unrep |= ALARM_SUM_DISC;

  p_difb->alarm_sum.current &= ~p_difb->alarm_sum.disable;


  /* Update the 'unack' bitfield ------------------------------------------ */
  p_difb->alarm_sum.unack = 0;

  if (_ALARM_NOT_ACKNOWLEDGED(p_difb->block_alm.unack))
       p_difb->alarm_sum.unack |= ALARM_SUM_BLOCK;

  if (_ALARM_NOT_ACKNOWLEDGED(p_difb->disc_alm.unack))
       p_difb->alarm_sum.unack |= ALARM_SUM_DISC;

  p_difb->alarm_sum.current &= ~p_difb->alarm_sum.disable;

  return;
}






FUNCTION LOCAL VOID Clear_alarms_DIFB
  (
    T_DIFB *  p_difb,
    USIGN16   block_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (p_difb->disc_alm.current == ALARM_ACTIVE)
  {
    ffbl_alert_notify.action              = ALARM_CLEAR;
    ffbl_alert_notify.block_id            = block_id;
    ffbl_alert_notify.rel_idx             = DI_PAR_DISC_ALM;
    ffbl_alert_notify.priority            = p_difb->disc_pri;
    ffbl_alert_notify.data.dsc.subcode    = 0;
    ffbl_alert_notify.data.dsc.value      = p_difb->out_d.value;
    ffbl_alert_notify.data.dsc.mbit_mask  = 0;

    fbs_alert_notify (&ffbl_alert_notify);
  }

  return;
}





FUNCTION LOCAL VOID Check_config_DIFB
  (
    IN T_DIFB *                    p_difb,
    IN T_DIFB_DATA *               p_difb_data,
    IN const T_FBIF_BLOCK_DESCR *  p_block_desc
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Check the parameter with E units if zero.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN32     block_err_desc;

FUNCTION_BODY

  block_err_desc = BLK_ERR_NO_ERROR;

  if (p_difb->blk_data.period_of_execution == 0)
  {
    block_err_desc |= BLK_ERR_EXE_TIME;
  }

  /* Check if the DI block is linked to a transducer block ---------------- */
  if (p_difb->channel == 0)
  {
    block_err_desc |= BLK_ERR_CHANNEL_INIT;
  }

  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_difb_data->exec_tb.channel = fbs_link_transducer_block
                     (p_difb->channel,p_block_desc,&p_difb_data->linked_tb);

    if (p_difb_data->exec_tb.channel != 0)
    {
      p_difb_data->exec_tb.p_xd_scale = NULL;
    }
    else /* No transducer block for this channel */
    {
      block_err_desc |= BLK_ERR_CHANNEL_SETTING;
    }
  }


  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_difb->block_err &= ~BLK_ERR_BLOCK_CONFIG_ERR;
  }
  else
  {
    p_difb->block_err |= BLK_ERR_BLOCK_CONFIG_ERR;
  }

  p_difb->block_err_desc_1 = block_err_desc;

  return;

}




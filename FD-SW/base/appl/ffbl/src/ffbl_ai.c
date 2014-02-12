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

FILE_NAME          ffbl_ai.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKAI)

INCLUDES

  #include <math.h>
  /*@Dresser-Masoneilan project */
  #include <string.h>

  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "fbap.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_ai.h"

//lint -estring(457,working_scale) - this is probably wrong in general but good for what lint had in mind:
/*
Warning 457: Function 'Execute_AIFB(struct _T_FBIF_BLOCK_INSTANCE *)' of type
    'unsigned short (struct _T_FBIF_BLOCK_INSTANCE *)' of thread
    'dummy_FB_exec_task(unsigned char, struct _T_FBIF_BLOCK_INSTANCE *, struct
    _T_FBIF_PROCESS_DATA *)' of type 'void (unsigned char, struct
    _T_FBIF_BLOCK_INSTANCE *, struct _T_FBIF_PROCESS_DATA *)' has an
    unprotected write access to variable 'working_scale' of type 'SCALE' which
    is used by function 'Start_AIFB(struct _T_FBIF_BLOCK_INSTANCE *)' of type
    'void (struct _T_FBIF_BLOCK_INSTANCE *)' of thread 'dummy_appl_task(unsigned
    char, struct _T_FBIF_BLOCK_INSTANCE *)' of type 'void (unsigned char,
    struct _T_FBIF_BLOCK_INSTANCE *)'
Warning 457: Function 'Start_AIFB(struct _T_FBIF_BLOCK_INSTANCE *)' of type
    'void (struct _T_FBIF_BLOCK_INSTANCE *)' of thread 'dummy_appl_task(unsigned
    char, struct _T_FBIF_BLOCK_INSTANCE *)' of type 'void (unsigned char,
    struct _T_FBIF_BLOCK_INSTANCE *)' has an unprotected write access to
    variable 'working_scale' of type 'SCALE' which is used by function
    'Execute_AIFB(struct _T_FBIF_BLOCK_INSTANCE *)' of type 'unsigned short
    (struct _T_FBIF_BLOCK_INSTANCE *)' of thread 'dummy_FB_exec_task(unsigned
    char, struct _T_FBIF_BLOCK_INSTANCE *, struct _T_FBIF_PROCESS_DATA *)' of
    type 'void (unsigned char, struct _T_FBIF_BLOCK_INSTANCE *, struct
    _T_FBIF_PROCESS_DATA *)'
*/

LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID Update_lim_alarms_AIFB (T_AIFB *, USIGN16);
  FUNCTION LOCAL VOID Clear_lim_alarms_AIFB (T_AIFB *, USIGN16);
  FUNCTION LOCAL VOID Update_block_alarm_AIFB (const T_FBIF_BLOCK_DESCR *, T_AIFB *, T_AIFB_DATA *);
  FUNCTION LOCAL VOID Update_alarm_sum_AIFB (T_FBIF_BLOCK_INSTANCE *);
  FUNCTION LOCAL VOID Check_config_AIFB (T_AIFB *, T_AIFB_DATA *, const T_FBIF_BLOCK_DESCR *);

IMPORT_DATA

EXPORT_DATA

LOCAL_DATA
#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT static SCALE working_scale;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */


/****************************************************************************/

FUNCTION GLOBAL VOID Start_AIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_AIFB *            p_aifb;
  T_AIFB_DATA *       p_aifb_data;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_AIFB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_aifb      = p_block_instance->p_block_desc->p_block;
  p_aifb_data = p_block_instance->p_block_data;


  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_aifb->update_evt);

  p_aifb_data->exec_tb.p_xd_scale   = &p_aifb->xd_scale;
  p_aifb_data->old_block_alarm_bits = 0;

  /* Store out param in the internal block-specific data structure -------- */
  p_aifb_data->out    = p_aifb->out;

  /* Set out status to BAD out-of-service --------------------------------- */
  p_aifb->out.status  = SQ_BAD|SUB_OUT_OF_SERVICE;

  /* Initialize static variables ------------------------------------------ */
  working_scale.eu_0   =   0.0;
  working_scale.eu_100 = 100.0;

  /* Check function block configuration ----------------------------------- */
  _ASSERT (ffbl_call_fdev_funct.a_of_chan_unit_check != NULL)

  p_aifb_data->chan_unit_check = ffbl_call_fdev_funct.a_of_chan_unit_check (
                                    p_aifb->channel,
                                    &p_aifb->xd_scale
                                 );

  /* Initialize the alert's unit index (New requirement ITK 6, tests i0602_xx) */
  p_aifb->lo_alm.unit_index    = p_aifb->out_scale.units;
  p_aifb->lo_lo_alm.unit_index = p_aifb->out_scale.units;
  p_aifb->hi_alm.unit_index    = p_aifb->out_scale.units;
  p_aifb->hi_hi_alm.unit_index = p_aifb->out_scale.units;

  Check_config_AIFB (p_aifb,p_aifb_data,p_block_instance->p_block_desc);

  return;
}





FUNCTION GLOBAL VOID Stop_AIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_AIFB_DATA *               p_aifb_data;
  T_AIFB *                    p_aifb;

FUNCTION_BODY

  p_block_desc = p_block_instance->p_block_desc;
  p_aifb_data  = p_block_instance->p_block_data;
  p_aifb       = p_block_desc->p_block;

  if (p_aifb->mode_blk.actual == MODE_OS) return;

  p_aifb->mode_blk.actual = MODE_OS;

  /* Set block error ------------------------------------------------------ */
  p_aifb->block_err &= ~BLK_ERR_INPUT_FAILURE;
  p_aifb->block_err |= BLK_ERR_OUT_OF_SERVICE;


  /* Clear all limit alarms ----------------------------------------------- */
  Clear_lim_alarms_AIFB (p_aifb,p_block_desc->block_id);

  /* Store out param in the internal block-specific data structure -------- */
  p_aifb_data->out          = p_aifb->out;

  p_aifb->field_val.status  = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_aifb->pv.status         = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_aifb->out.status        = SQ_BAD|SUB_OUT_OF_SERVICE;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_aifb->update_evt);

  /* Update block alarm --------------------------------------------------- */
  Update_block_alarm_AIFB (p_block_desc, p_aifb,
          (T_AIFB_DATA *)p_block_instance->p_block_data);

  return;
}





FUNCTION GLOBAL USIGN16 Read_handler_AIFB
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

  /* Check and/or update parameter to be read ------------------------------- */
  if (p_read->rel_idx == AI_PAR_ALARM_SUM)
  {
    Update_alarm_sum_AIFB (p_block_instance);
  }

  return E_OK;
}







FUNCTION GLOBAL USIGN16 Write_handler_AIFB
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
  T_AIFB *                    p_aifb;
  T_AIFB_DATA *               p_aifb_data;
  USIGN8                      subindex;
  USIGN8 *                    source;
  USIGN8                      result;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_aifb_data   = p_block_instance->p_block_data;
  p_aifb        = p_block_desc->p_block;

  subindex      = p_write->subindex;
  source        = p_write->source;
  result        = E_OK;

  switch (p_write->rel_idx)
  {
    case AI_PAR_ALERT_KEY:
    {
      if (*source == 0) result = E_FB_PARA_CHECK;

      break;
    }

    case AI_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (
                 &p_aifb->mode_blk,
                 subindex,
                 source,
                 MODE_AI_VALID
               );

      break;
    }

    case AI_PAR_OUT:
    {
      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //FLOAT *   a_of_value;
      USIGN8 *  a_of_value;
      FLOAT     fvalue;

      if (MODE_LOWER_THAN_MAN (p_aifb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      //if      (subindex == 0) a_of_value = (FLOAT *)(source + 1);
      //else if (subindex == 2) a_of_value = (FLOAT *) source;
      if      (subindex == 0) a_of_value = source + 1;
      else if (subindex == 2) a_of_value = source;
      else /* (subindex == 1) */ return (E_FB_DATA_NO_WRITE);

      /* Check scale-limits: OUT_SCALE +/-10% ------------------------- */

      //ffbl_bas_check_scale_limits (a_of_value,&p_aifb->out_scale);
      memcpy (&fvalue, a_of_value, sizeof (fvalue));
      ffbl_bas_check_scale_limits (&fvalue,&p_aifb->out_scale);

      /* fvalue could have been modified, copy it back to the telegram buffer */
      memcpy (a_of_value, &fvalue, sizeof (fvalue));


      break;
    }

    case AI_PAR_SIMULATE:
    {
      USIGN8 enable_flag;

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //if      (subindex == 0) enable_flag = ((SIMULATE_FLOAT *)source)->enable;
      if      (subindex == 0) enable_flag = source[10];
      else if (subindex == 5) enable_flag = *source;
      else                    break;

      if (ffbl_bas_check_sim_param (enable_flag) == E_FB_PARA_CHECK)
        return (E_FB_PARA_CHECK);

      break;
    }

    case AI_PAR_XD_SCALE:
    {
      SCALE   xd_scale = p_aifb->xd_scale;

      if (MODE_NOT_OS (p_aifb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //if      (subindex == 0) xd_scale       = *((SCALE *)  source);
      //else if (subindex == 3) xd_scale.units = *((USIGN16 *)source);
      /* Struct is Float, Float, USIGN16, INT8 --> no alignment byte between components */
      if      (subindex == 0)
      {
        memcpy(&xd_scale, source, 4+4+2+1);
      }
      else if (subindex == 3)
      {
        WHB_(xd_scale.units) = WPHB_(source);
        WLB_(xd_scale.units) = WPLB_(source);
      }
      else break;

      p_aifb_data->chan_unit_check = ffbl_call_fdev_funct.a_of_chan_unit_check (
                                       p_aifb->channel,
                                       &xd_scale
                                     );

      if (p_aifb_data->chan_unit_check == INVALID_ENGINEERING_UNIT)
        return (E_FB_PARA_CHECK);

      break;
    }

    case AI_PAR_OUT_SCALE:
    {
      if (MODE_NOT_OS (p_aifb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      break;
    }

    case AI_PAR_GRANT_DENY:
    {
      ffbl_bas_write_grant_deny (subindex,source,&p_aifb->grant_deny);

      break;
    }

    case AI_PAR_IO_OPTS:
    {
      if (MODE_NOT_OS (p_aifb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //*((USIGN16 *)source) &= AI_IO_OPTS; /* Delete all undefined bits */
      WPHB_(source) &= CWHB_(AI_IO_OPTS); /* Delete all undefined bits */
      WPLB_(source) &= CWLB_(AI_IO_OPTS); /* Delete all undefined bits */

      break;
    }

    case AI_PAR_STATUS_OPTS:
    {
      if (MODE_NOT_OS (p_aifb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //*((USIGN16 *)source) &= AI_STATUS_OPTS;  /* Delete all undefined bits */
      WPHB_(source) &= CWHB_(AI_STATUS_OPTS);  /* Delete all undefined bits */
      WPLB_(source) &= CWLB_(AI_STATUS_OPTS);  /* Delete all undefined bits */

      break;
    }

    case AI_PAR_CHANNEL:
    {
      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      USIGN16   u16value;

      if (MODE_NOT_OS (p_aifb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      memcpy (&u16value, source, sizeof (u16value));
      //if ( ffbl_bas_check_channel (*(USIGN16 *)source, p_block_desc))
      if ( ffbl_bas_check_channel (u16value, p_block_desc))
        return E_FB_PARA_CHECK;

      //p_aifb_data->chan_unit_check =
          //ffbl_call_fdev_funct.a_of_chan_unit_check
                         //(*(USIGN16 *)source,&p_aifb->xd_scale);
      p_aifb_data->chan_unit_check =
          ffbl_call_fdev_funct.a_of_chan_unit_check
                         (u16value, &p_aifb->xd_scale);
      break;
    }

    case AI_PAR_L_TYPE:
    {
      if (MODE_LOWER_THAN_MAN (p_aifb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      if ( (*source < L_TYPE_DIRECT) ||
           (*source > L_TYPE_IND_SQR_ROOT) )
        return (E_FB_PARA_LIMIT);

      break;
    }

    case AI_PAR_LOW_CUT:
    {
      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //if (*((FLOAT *)source) < 0)
      /* float value < 0.0 -> Check Bit 0x80 in high byte */
      if ((DPWHHB_(source) & 0x80u))
        return (E_FB_PARA_LIMIT);

      break;
    }

    case AI_PAR_PV_FTIME:
    {
      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //if (*((FLOAT *)source) < 0)
      /* float value < 0.0 -> Check Bit 0x80 in high byte */
      if ((DPWHHB_(source) & 0x80u))
        return (E_FB_PARA_LIMIT);

      break;
    }

    case AI_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_aifb->update_evt.unack, source, subindex);

      break;
    }

    case AI_PAR_BLOCK_ALM:
    {
      result = ffbl_check_unack_flag
                (&p_aifb->block_alm.unack, source, subindex);
      break;
    }

    case AI_PAR_ALARM_HYS:
    {
      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //if ( (*((FLOAT *)source) < 0.0) || (*((FLOAT *)source) > 50.0) )
        //return (E_FB_PARA_LIMIT);
      FLOAT     fvalue;

      memcpy (&fvalue, source, sizeof (fvalue));
      if ( (fvalue < 0.0f) || (fvalue > 50.0f) )
        return (E_FB_PARA_LIMIT);

      break;
    }

    case AI_PAR_HI_HI_PRI:
    case AI_PAR_HI_PRI:
    case AI_PAR_LO_PRI:
    case AI_PAR_LO_LO_PRI:
    {
      if (*source > WRITE_PRI_MAX)
        return (E_FB_PARA_LIMIT);

      break;
    }

    case AI_PAR_HI_HI_ALM:
    {
      result = ffbl_check_unack_flag (&p_aifb->hi_hi_alm.unack, source, subindex);

      break;
    }

    case AI_PAR_HI_ALM:
    {
      result = ffbl_check_unack_flag (&p_aifb->hi_alm.unack, source, subindex);

      break;
    }

    case AI_PAR_LO_ALM:
    {
      result = ffbl_check_unack_flag (&p_aifb->lo_alm.unack, source, subindex);

      break;
    }

    case AI_PAR_LO_LO_ALM:
    {
      result = ffbl_check_unack_flag (&p_aifb->lo_lo_alm.unack, source, subindex);

      break;
    }

    /* ----------------------------------------------------------------------
    case AI_PAR_HI_HI_LIM:
    case AI_PAR_HI_LIM:
    case AI_PAR_LO_LIM:
    case AI_PAR_LO_LO_LIM:
    {
      FF-891, rev 1.6, 5.5, states "PV_SCALE and +INF or -INF resp." as
      valid range of the alarm limit parameters.

      Because there is no reason for these restrictions this implementation
      does not apply checks on alarm limit parameters. The decision about the
      alarm-limits is an exclusively matter of the configuring host application.
    }
    ----------------------------------------------------------------------- */
  }


  if (result == E_OK)
  {
    result = fbs_write_param (p_block_instance,p_write);
  }


  if (result == E_OK)
  {
    BOOL check_config;

    switch (p_write->rel_idx)
    {
      case AI_PAR_MODE_BLK:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        write_loc.rel_idx = 0;

        if (p_aifb->mode_blk.target & MODE_OS)
        {
          p_aifb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;

          write_loc.rel_idx = AI_PAR_OUT;
        }

        else if (p_aifb->mode_blk.target & MODE_MAN)
        {
          write_loc.rel_idx = AI_PAR_OUT;
        }

        if (write_loc.rel_idx)
        {
          /* Store out parameter in non-volatile RAM ---------------------- */
          write_loc.subindex      = 0;
          /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
          //write_loc.length        = 5;
          write_loc.length        = sizeof(p_aifb->out);
          write_loc.source        = (USIGN8 *)&p_aifb->out;
          write_loc.startup_sync  = FALSE;

          fbs_write_param_loc (p_block_instance,&write_loc);
        }

        break;
      }

      case AI_PAR_OUT:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        p_aifb->out.status = SQ_GOOD_NC;

        /* Store GOOD out status in non-volatile RAM ---------------------- */
        write_loc.rel_idx       = AI_PAR_OUT;
        write_loc.subindex      = 0;
        /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
        //write_loc.length        = 5;
        write_loc.length        = sizeof(p_aifb->out);
        write_loc.source        = (USIGN8 *)&p_aifb->out;
        write_loc.startup_sync  = FALSE;

        fbs_write_param_loc (p_block_instance,&write_loc);

        /* Store out param in the internal block-specific data structure -- */
        p_aifb_data->out = p_aifb->out;

        /* Adapt out status in accordance to the actual mode -------------- */
        if (p_aifb->mode_blk.actual == MODE_OS)
        {
          p_aifb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;
        }
        else
        {
          p_aifb->out.status |= LIMIT_CONST;
        }

        break;
      }

      case AI_PAR_OUT_SCALE:
      {
        /* Update the alert's unit index (New requirement ITK 6, tests i0602_xx) */
        p_aifb->lo_alm.unit_index    = p_aifb->out_scale.units;
        p_aifb->lo_lo_alm.unit_index = p_aifb->out_scale.units;
        p_aifb->hi_alm.unit_index    = p_aifb->out_scale.units;
        p_aifb->hi_hi_alm.unit_index = p_aifb->out_scale.units;

        /* no break - fall through */
      }

      case AI_PAR_BLK_DATA:
      case AI_PAR_XD_SCALE:
      case AI_PAR_L_TYPE:
      case AI_PAR_CHANNEL:
      case AI_PAR_PV_FTIME:
        check_config = TRUE;
        break;
      default:
        check_config = FALSE;
    }

    if (check_config)
    {
      Check_config_AIFB (p_aifb,p_aifb_data,p_block_instance->p_block_desc);
    }
  }

return result;
}







FUNCTION GLOBAL USIGN16 Execute_AIFB
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
  T_AIFB *                     p_aifb;
  T_AIFB_DATA *                p_aifb_data;
  USIGN8                       old_mode;
  USIGN8                       actual_mode;
  FLOAT_S                      raw_value;
  BOOL                         enable_update_evt;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_aifb_data   = p_block_instance->p_block_data;
  p_aifb        = p_block_desc->p_block;

  /* Update simulate parameter -------------------------------------------- */
  ffbl_bas_update_sim_param (&p_aifb->simulate.enable,&p_aifb->block_err);

  /* Update block mode -------------------------------------------------- */
  old_mode    = p_aifb->mode_blk.actual;
  actual_mode = ffbl_bas_calc_block_mode (&p_aifb->mode_blk,p_aifb->block_err);

  if (actual_mode == MODE_OS)
  {
    if (old_mode != MODE_OS)
    {
      /* Mode OS is entered now ------------------------------------------- */
      Stop_AIFB (p_block_instance);
    }
    else
    {
      /* Update block alarm ----------------------------------------------- */
      Update_block_alarm_AIFB (p_block_desc, p_aifb, p_aifb_data);
    }

    return(p_aifb->blk_data.next_fb_to_execute);
  }

  p_aifb->mode_blk.actual = actual_mode;

  if (old_mode == MODE_OS) /* Mode OS is left now ------------------------- */
  {
    /* Reset block error -------------------------------------------------- */
    p_aifb->block_err &= ~BLK_ERR_OUT_OF_SERVICE;
    enable_update_evt  = TRUE;

    /* Restore out parameter ---------------------------------------------- */
    p_aifb->out = p_aifb_data->out;

    if ((p_aifb->out.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_BAD|SUB_OUT_OF_SERVICE))
    {
      p_aifb->out.status = SQ_BAD;
    }
  }
  else
  {
    enable_update_evt = FALSE;
  }

  /* Get raw input value -------------------------------------------------- */
  if (p_aifb->simulate.enable == SIMULATION_DISABLED)
  {
    /* Execute the transducer block --------------------------------------- */
    p_aifb_data->exec_tb.p_process_value = &raw_value;
    p_aifb_data->exec_tb.tb_error        = FALSE;

    _ASSERT (p_aifb_data->linked_tb.a_of_execute_tb != NULL);

    (*p_aifb_data->linked_tb.a_of_execute_tb)(
            p_aifb_data->linked_tb.p_tb_instance,
            &p_aifb_data->exec_tb
         );

    p_aifb->simulate.field_status = raw_value.status;
    p_aifb->simulate.field_value  = raw_value.value;

    /* Copy field value to simulate parameter owing to interop. test 2705_00 */
    p_aifb->simulate.status       = raw_value.status;
    p_aifb->simulate.value        = raw_value.value;
  }

  else /* p_aifb->simulate.enable == SIMULATION_ENABLED */
  {
    raw_value.status = p_aifb->simulate.status;
    raw_value.value  = p_aifb->simulate.value;
  }


  if ((raw_value.status & QUALITY_MASK) != SQ_BAD)
  {
    /* Input from transducer block is not BAD ------------------------------- */
    if (p_aifb->block_err & BLK_ERR_INPUT_FAILURE)
    {
      p_aifb->block_err &= ~BLK_ERR_INPUT_FAILURE;
    }

    /* Calculate the field value ---------------------------------------------- */
    p_aifb->field_val.value  = ffbl_bas_rescale_value (raw_value.value,
                                        &p_aifb->xd_scale,&working_scale);


    /* Apply linearization to the raw value ----------------------------------- */
    switch (p_aifb->l_type)
    {
      case L_TYPE_DIRECT:
        /* If l_type is Direct, raw_value = channel value */

        break;

      case L_TYPE_INDIRECT:
        raw_value.value = ffbl_bas_rescale_value (p_aifb->field_val.value,
                                     &working_scale,&p_aifb->out_scale);
        break;

      case L_TYPE_IND_SQR_ROOT:
        {
          BOOL negative;

          if (p_aifb->field_val.value < 0.0)
          {
            p_aifb->field_val.value *= -1.0;
            negative = TRUE;
          }
          else
          {
            negative = FALSE;
          }

          raw_value.value = sqrt(0.01 * p_aifb->field_val.value) *
                            (p_aifb->out_scale.eu_100 -
                            p_aifb->out_scale.eu_0) +
                            p_aifb->out_scale.eu_0;

          if (negative)
          {
            raw_value.value *= -1.0;
          }
        }
        break;

      default:
        raw_value.value = 0.0;
    }


    /* Apply LOW_CUT ---------------------------------------------------------- */
    if (p_aifb->io_opts & IO_OPT_LOW_CUTOFF)
    {
      if (p_aifb->field_val.value < p_aifb->low_cut)
      {
        raw_value.value = p_aifb->out_scale.eu_0;
      }
    }


    /* Apply PV filter ------------------------------------------------------ */
    if (((p_aifb->pv.status & QUALITY_MASK) == SQ_GOOD_NC) &&
        ((raw_value.status  & QUALITY_MASK) == SQ_GOOD_NC) &&
         (p_aifb_data->pv_factor != 1.0f)                                   )
    {
      /* PV filter only if old PV an the current raw-value have good status - */
      raw_value.value =     p_aifb_data->pv_factor  * raw_value.value   +
                       (1 - p_aifb_data->pv_factor) * p_aifb->pv.value;
    }


    /* Assign raw-value to PV ----------------------------------------------- */
    p_aifb->pv.value  = raw_value.value;
  }

  else /* raw_value.status & QUALITY_MASK) == SQ_BAD */
  {
    /* Set input failure bit if input from transducer block is BAD -------- */
    if (!(p_aifb->block_err & BLK_ERR_INPUT_FAILURE))
    {
      p_aifb->block_err |= BLK_ERR_INPUT_FAILURE;
    }

    p_aifb->field_val.value  = 0.0f;
    p_aifb->pv.value         = 0.0f;
  }


  /* Propagate status from TB to field value and to process value --------- */
  p_aifb->field_val.status = raw_value.status;
  p_aifb->pv.status        = raw_value.status;


  if (p_aifb->mode_blk.actual == MODE_AUTO)
  {
    p_aifb->out = raw_value; /* Update OUT parameter */


    /* Apply status options on output parameter --------------------------- */
    if (raw_value.status & LIMIT_MASK)
    {
      USIGN8 limit_bits = raw_value.status & LIMIT_MASK;

      /* Apply status option 'Uncertain if Limited' ----------------------- */
      if (p_aifb->status_opts & STATUS_OPT_UNCERT_IF_LIM)
      {
        if ((raw_value.status & QUALITY_MASK) & SQ_GOOD_NC)
        {
          p_aifb->out.status = SQ_UNCERTAIN | SUB_NON_SPECIFIC | limit_bits;
        }
      }

      /* Apply status option 'BAD if Limited' ----------------------------- */
      if (p_aifb->status_opts & STATUS_OPT_BAD_IF_LIM)
      {
        if ((raw_value.status & QUALITY_MASK) != SQ_BAD)
        {
          p_aifb->out.status = SQ_BAD | SUB_NON_SPECIFIC | limit_bits;
        }
      }
    }
  }

  else /* p_aifb->mode_blk.actual == MODE_MAN */
  {
    /* Apply status option 'Uncertain if Man mode' ------------------------ */
    if (p_aifb->status_opts & STATUS_OPT_UNCERT_IF_MAN)
    {
      if ((p_aifb->out.status & QUALITY_MASK) & SQ_GOOD_NC)
      {
        p_aifb->out.status = SQ_UNCERTAIN | SUB_NON_SPECIFIC;
      }
    }

    if ((p_aifb->out.status & QUALITY_MASK) != SQ_BAD)
    {
      p_aifb->out.status |= LIMIT_CONST;
    }
  }


  /* Alarm checks --------------------------------------------------------- */
  if ((p_aifb->out.status & QUALITY_MASK) != SQ_BAD)
  {
    Update_lim_alarms_AIFB (p_aifb,p_block_desc->block_id);
  }

  Update_alarm_sum_AIFB (p_block_instance);


  /* Calculate sub-status of out parameter -------------------------------- */
  if ((p_aifb->out.status & QUALITY_MASK) == SQ_GOOD_NC)
  {
    /* Set Good (Non-cascade) substatus ----------------------------------- */
    p_aifb->out.status &= ~SUB_STATUS_MASK;

    p_aifb->out.status |= Set_good_nc_substatus
      (p_aifb->alarm_sum.unack,p_aifb->alarm_sum.current,
       p_aifb->lo_pri,p_aifb->lo_lo_pri,
       p_aifb->hi_pri,p_aifb->hi_hi_pri);
  }

  /* Update block alarm --------------------------------------------------- */
  Update_block_alarm_AIFB (p_block_desc, p_aifb, p_aifb_data);

  if (enable_update_evt)
  {
    /* Enable update events ----------------------------------------------- */
    ffbl_enable_update_events
    (
      p_block_desc->block_id,
      p_block_desc->idx_upd_evt,
      &p_aifb->update_evt,
      p_aifb->st_rev
    );
  }

  return(p_aifb->blk_data.next_fb_to_execute);
}







FUNCTION GLOBAL VOID Background_AIFB
   (
     T_FBIF_BLOCK_INSTANCE *    p_block_instance
   )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

PARAMETERS:
  p_block_instance  Pointer to the block to be executed

RETURN_VALUES:
  none
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _UNREF_PARAM (p_block_instance);

  return;
}









FUNCTION LOCAL VOID Update_lim_alarms_AIFB
  (
    T_AIFB *    p_aifb,
    USIGN16     block_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* Alarm checks ------------------------------------------------------- */
  alm.block_id = block_id;
  alm.value    = p_aifb->out.value;
  alm.hyst     = (0.01 * p_aifb->alarm_hys);
  alm.hyst    *= (p_aifb->out_scale.eu_100 - p_aifb->out_scale.eu_0);

  /* Check low alarms ----------------------------------------------------- */
  alm.high = FALSE;
  if (p_aifb->lo_alm.current == ALARM_CLEAR)
  {
    alm.ptr      = &p_aifb->lo_alm;
    alm.idx      = AI_PAR_LO_ALM;
    alm.pri      = p_aifb->lo_pri;
    alm.disabled = (BOOL)((p_aifb->alarm_sum.disable & ALARM_SUM_LO)?(TRUE):(FALSE));
    alm.auto_ack = (BOOL)((p_aifb->ack_option & ALARM_SUM_LO)?(TRUE):(FALSE));
    alm.limit    = p_aifb->lo_lim;

    Handle_inact_alarm (); /* This routine works with the data stored in alm! */
  }

  alm.ptr      = &p_aifb->lo_lo_alm;
  alm.idx      = AI_PAR_LO_LO_ALM;
  alm.pri      = p_aifb->lo_lo_pri;
  alm.disabled = (BOOL)((p_aifb->alarm_sum.disable & ALARM_SUM_LO_LO)?(TRUE):(FALSE));
  alm.auto_ack = (BOOL)((p_aifb->ack_option & ALARM_SUM_LO_LO)?(TRUE):(FALSE));
  alm.limit    = p_aifb->lo_lo_lim;

  if (p_aifb->lo_lo_alm.current == ALARM_CLEAR)
  {
    Handle_inact_alarm (); /* This routine works with the data stored in alm! */
  }
  else /* (p_aifb->lo_lo_alm.current == ALARM_ACTIVE) */
  {
    Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }


  if (p_aifb->lo_alm.current == ALARM_ACTIVE)
  {
    alm.ptr      = &p_aifb->lo_alm;
    alm.idx      = AI_PAR_LO_ALM;
    alm.pri      = p_aifb->lo_pri;
    alm.disabled = (BOOL)((p_aifb->alarm_sum.disable & ALARM_SUM_LO)?(TRUE):(FALSE));
    alm.auto_ack = (BOOL)((p_aifb->ack_option & ALARM_SUM_LO)?(TRUE):(FALSE));
    alm.limit    = p_aifb->lo_lim;

    Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }


  /* Check high alarms ---------------------------------------------------- */
  alm.high = TRUE;
  if (p_aifb->hi_alm.current == ALARM_CLEAR)
  {
    alm.ptr      = &p_aifb->hi_alm;
    alm.idx      = AI_PAR_HI_ALM;
    alm.pri      = p_aifb->hi_pri;
    alm.disabled = (BOOL)((p_aifb->alarm_sum.disable & ALARM_SUM_HI)?(TRUE):(FALSE));
    alm.auto_ack = (BOOL)((p_aifb->ack_option & ALARM_SUM_HI)?(TRUE):(FALSE));
    alm.limit    = p_aifb->hi_lim;

    Handle_inact_alarm (); /* This routine works with the data stored in alm! */
  }


  alm.ptr      = &p_aifb->hi_hi_alm;
  alm.idx      = AI_PAR_HI_HI_ALM;
  alm.pri      = p_aifb->hi_hi_pri;
  alm.disabled = (BOOL)((p_aifb->alarm_sum.disable & ALARM_SUM_HI_HI)?(TRUE):(FALSE));
  alm.auto_ack = (BOOL)((p_aifb->ack_option & ALARM_SUM_HI_HI)?(TRUE):(FALSE));
  alm.limit    = p_aifb->hi_hi_lim;

  if (p_aifb->hi_hi_alm.current == ALARM_CLEAR)
  {
    Handle_inact_alarm (); /* This routine works with the data stored in alm! */
  }
  else /* (p_aifb->hi_hi_alm.current == ALARM_ACTIVE) */
  {
    Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }


  if (p_aifb->hi_alm.current == ALARM_ACTIVE)
  {
    alm.ptr      = &p_aifb->hi_alm;
    alm.idx      = AI_PAR_HI_ALM;
    alm.pri      = p_aifb->hi_pri;
    alm.disabled = (BOOL)((p_aifb->alarm_sum.disable & ALARM_SUM_HI)?(TRUE):(FALSE));
    alm.auto_ack = (BOOL)((p_aifb->ack_option & ALARM_SUM_HI)?(TRUE):(FALSE));
    alm.limit    = p_aifb->hi_lim;

    Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }

  return;
}






FUNCTION LOCAL VOID Clear_lim_alarms_AIFB
  (
    T_AIFB *  p_aifb,
    USIGN16   block_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  alm.block_id = block_id;
  alm.value    = p_aifb->out.value;
  alm.disabled = TRUE;

  /* Clear low-low alarm -------------------------------------------------- */

  if (p_aifb->lo_lo_alm.current == ALARM_ACTIVE)
  {
    alm.ptr      = &p_aifb->lo_lo_alm;
    alm.idx      = AI_PAR_LO_LO_ALM;
    alm.pri      = p_aifb->lo_lo_pri;

    Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }

  /* Clear low alarm ------------------------------------------------------ */

  if (p_aifb->lo_alm.current == ALARM_ACTIVE)
  {
    alm.ptr      = &p_aifb->lo_alm;
    alm.idx      = AI_PAR_LO_ALM;
    alm.pri      = p_aifb->lo_pri;

    Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }

  /* Clear high-high alarm ------------------------------------------------ */

  if (p_aifb->hi_hi_alm.current == ALARM_ACTIVE)
  {
    alm.ptr      = &p_aifb->hi_hi_alm;
    alm.idx      = AI_PAR_HI_HI_ALM;
    alm.pri      = p_aifb->hi_hi_pri;

    Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }

  /* Clear high alarm ----------------------------------------------------- */

  if (p_aifb->hi_alm.current == ALARM_ACTIVE)
  {
    alm.ptr      = &p_aifb->hi_alm;
    alm.idx      = AI_PAR_HI_ALM;
    alm.pri      = p_aifb->hi_pri;

    Handle_act_alarm (); /* This routine works with the data stored in alm! */
  }

return;
}







FUNCTION LOCAL VOID Update_block_alarm_AIFB
  (
    IN const T_FBIF_BLOCK_DESCR * p_block_desc,
    INOUT T_AIFB *                p_aifb,
    INOUT T_AIFB_DATA *           p_aifb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16   alarm_bits;

FUNCTION_BODY

  if (p_aifb->block_err & BLK_ERR_OUT_OF_SERVICE)
  {
    alarm_bits = BLK_ERR_OUT_OF_SERVICE;
  }
  else
  {
    alarm_bits = p_aifb->block_err;

    if (p_aifb->status_opts & STATUS_OPT_PROP_FLT_FORW)
    {
      /* No input failure alarm --------------------------------------------- */
      alarm_bits &= ~BLK_ERR_INPUT_FAILURE;
    }
  }


  ffbl_bas_update_extended_block_alarm
  (
    p_block_desc, &p_aifb->block_alm,
    p_aifb->alarm_sum.disable, p_aifb->ack_option,
    alarm_bits, &p_aifb_data->old_block_alarm_bits
  );
}







FUNCTION LOCAL VOID Update_alarm_sum_AIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_AIFB *    p_aifb;

FUNCTION_BODY

  p_aifb  = p_block_instance->p_block_desc->p_block;

  /* Update the 'current' bitfield ---------------------------------------- */
  p_aifb->alarm_sum.current = 0;

  if (p_aifb->block_err)
       p_aifb->alarm_sum.current |= ALARM_SUM_BLOCK;

  if (p_aifb->lo_alm.current == ALARM_ACTIVE)
       p_aifb->alarm_sum.current |= ALARM_SUM_LO;

  if (p_aifb->lo_lo_alm.current == ALARM_ACTIVE)
       p_aifb->alarm_sum.current |= ALARM_SUM_LO_LO;

  if (p_aifb->hi_alm.current == ALARM_ACTIVE)
       p_aifb->alarm_sum.current |= ALARM_SUM_HI;

  if (p_aifb->hi_hi_alm.current == ALARM_ACTIVE)
       p_aifb->alarm_sum.current |= ALARM_SUM_HI_HI;

  p_aifb->alarm_sum.current &= ~p_aifb->alarm_sum.disable;


  /* Update the 'unreported' bitfield ------------------------------------- */
  p_aifb->alarm_sum.unrep = 0;

  if (_ALARM_NOT_REPORTED(p_aifb->block_alm.state))
       p_aifb->alarm_sum.unrep |= ALARM_SUM_BLOCK;

  if (_ALARM_NOT_REPORTED(p_aifb->lo_alm.state))
       p_aifb->alarm_sum.unrep |= ALARM_SUM_LO;

  if (_ALARM_NOT_REPORTED(p_aifb->lo_lo_alm.state))
       p_aifb->alarm_sum.unrep |= ALARM_SUM_LO_LO;

  if (_ALARM_NOT_REPORTED(p_aifb->hi_alm.state))
       p_aifb->alarm_sum.unrep |= ALARM_SUM_HI;

  if (_ALARM_NOT_REPORTED(p_aifb->hi_hi_alm.state))
       p_aifb->alarm_sum.unrep |= ALARM_SUM_HI_HI;

  p_aifb->alarm_sum.unrep &= ~p_aifb->alarm_sum.disable;


  /* Update the 'unack' bitfield ------------------------------------------ */
  p_aifb->alarm_sum.unack = 0;

  if (_ALARM_NOT_ACKNOWLEDGED(p_aifb->block_alm.unack))
       p_aifb->alarm_sum.unack |= ALARM_SUM_BLOCK;

  if (_ALARM_NOT_ACKNOWLEDGED(p_aifb->lo_alm.unack))
       p_aifb->alarm_sum.unack |= ALARM_SUM_LO;

  if (_ALARM_NOT_ACKNOWLEDGED(p_aifb->lo_lo_alm.unack))
       p_aifb->alarm_sum.unack |= ALARM_SUM_LO_LO;

  if (_ALARM_NOT_ACKNOWLEDGED(p_aifb->hi_alm.unack))
       p_aifb->alarm_sum.unack |= ALARM_SUM_HI;

  if (_ALARM_NOT_ACKNOWLEDGED(p_aifb->hi_hi_alm.unack))
       p_aifb->alarm_sum.unack |= ALARM_SUM_HI_HI;

  p_aifb->alarm_sum.unack &= ~p_aifb->alarm_sum.disable;


  return;
}






FUNCTION LOCAL VOID Check_config_AIFB
  (
    IN T_AIFB *                    p_aifb,
    IN T_AIFB_DATA *               p_aifb_data,
    IN const T_FBIF_BLOCK_DESCR *  p_block_desc
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN32   block_err_desc;

FUNCTION_BODY

  block_err_desc = BLK_ERR_NO_ERROR;

  if (p_aifb->blk_data.period_of_execution == 0)
  {
    block_err_desc |= BLK_ERR_EXE_TIME;
  }


  if (p_aifb_data->chan_unit_check != CHANNEL_UNIT_OK)
  {
    block_err_desc |= BLK_ERR_CH_UNIT;
  }


  /* Check if eu_100 is equal or less than eu_0 --------------------------- */
  if (p_aifb->xd_scale.eu_100  <= p_aifb->xd_scale.eu_0 )
  {
    block_err_desc |= BLK_ERR_XD_SCALE_LIM_SET;
  }
  if (p_aifb->out_scale.eu_100 <= p_aifb->out_scale.eu_0)
  {
    block_err_desc |= BLK_ERR_OUT_SCALE_LIM_SET;
  }


  /* Check L_TYPE parameter ----------------------------------------------- */
  if (p_aifb->l_type == 0)
  {
    block_err_desc |= BLK_ERR_L_TYP_UNINIT;
  }
  else if (p_aifb->l_type == L_TYPE_DIRECT)
  {
    /* Check if XD scale and OUT scale are the same ----------------------- */
    if ((p_aifb->xd_scale.units  != p_aifb->out_scale.units) ||
        (p_aifb->xd_scale.eu_100 != p_aifb->out_scale.eu_100)||
        (p_aifb->xd_scale.eu_0   != p_aifb->out_scale.eu_0)    )
    {
      block_err_desc |= BLK_ERR_L_TYP_DIRECT_SET;
    }
  }


  /* Check if the AI block is linked to a transducer block ---------------- */
  if (p_aifb->channel == 0)
  {
    block_err_desc |= BLK_ERR_CHANNEL_INIT;
  }


  /* No config error; update AI data -------------------------------------- */
  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_aifb_data->exec_tb.channel = fbs_link_transducer_block
                        (p_aifb->channel,p_block_desc,&p_aifb_data->linked_tb);

    if (p_aifb_data->exec_tb.channel != 0)
    {
      if (p_aifb->pv_ftime != 0.0f)
      {
        p_aifb_data->pv_factor =
            p_aifb->blk_data.period_of_execution /
           (p_aifb->blk_data.period_of_execution + 32000.0f * p_aifb->pv_ftime);
      }
      else
      {
        p_aifb_data->pv_factor = 1.0f;
      }
    }
    else /* No transducer block for this channel */
    {
      block_err_desc |= BLK_ERR_CHANNEL_SETTING;
    }


    if (p_aifb->pv_ftime != 0.0f)
    {
      p_aifb_data->pv_factor =
          p_aifb->blk_data.period_of_execution /
         (p_aifb->blk_data.period_of_execution + 32000.0f * p_aifb->pv_ftime);
    }
    else
    {
      p_aifb_data->pv_factor = 1.0f;
    }
  }


  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_aifb->block_err &= ~BLK_ERR_BLOCK_CONFIG_ERR;
  }
  else
  {
    p_aifb->block_err |= BLK_ERR_BLOCK_CONFIG_ERR;
  }

  p_aifb->block_err_desc_1 = block_err_desc;


  return;
}

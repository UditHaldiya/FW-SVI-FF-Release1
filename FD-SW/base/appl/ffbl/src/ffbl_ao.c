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

FILE_NAME          ffbl_ao.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
 #include "keywords.h"
 #define  MODULE_ID      (COMP_FFBL + MOD_FBLKAO)

INCLUDES

  /*@Dresser-Masoneilan project */
  #include <string.h>

  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbap.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_ao.h"
  #include "ffbl_res.h"

extern VOID     SetPA_5(VOID);
extern VOID     SetPA_6(VOID);
extern VOID     ResetPA_5(VOID);
extern VOID     ResetPA_6(VOID);


LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID Calc_block_mode_AOFB (T_AOFB *, T_AOFB_DATA *);
  FUNCTION LOCAL VOID Reset_AOFB (T_AOFB_DATA *);
  FUNCTION LOCAL VOID Check_config_AOFB (T_AOFB *, T_AOFB_DATA *, const T_FBIF_BLOCK_DESCR *);
  FUNCTION LOCAL VOID Update_block_alarm_AOFB (const T_FBIF_BLOCK_DESCR *, T_AOFB *,T_AOFB_DATA *);

IMPORT_DATA

EXPORT_DATA

LOCAL_DATA


/****************************************************************************/

FUNCTION GLOBAL VOID Start_AOFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_AOFB_DATA *       p_aofb_data;
  T_AOFB *            p_aofb;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_AOFB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_aofb_data = p_block_instance->p_block_data;
  p_aofb      = p_block_instance->p_block_desc->p_block;

  /* Initialize internal function block data ------------------------------ */
  Reset_AOFB (p_aofb_data);


  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_aofb->update_evt);


  p_aofb_data->exec_tb.p_xd_scale   = &p_aofb->xd_scale;
  p_aofb_data->old_block_alarm_bits = 0;

  /* Store out param in the internal block-specific data structure -------- */
  p_aofb_data->out    = p_aofb->out;

  /* Set out status to BAD out-of-service --------------------------------- */
  p_aofb->out.status  = SQ_BAD|SUB_OUT_OF_SERVICE;

  /* Apply IO option 'Use Fault State value on restart' ------------------- */
  if (p_aofb->io_opts & IO_OPT_FSTATE_TO_VAL_ON_RESTART)
  {
    p_aofb->sp.value  = p_aofb->fstate_val;
    p_aofb->sp.status = SQ_GOOD_CAS|LIMIT_CONST;
  }

  /* Check function block configuration ----------------------------------- */
  p_aofb_data->chan_unit_check = ffbl_call_fdev_funct.a_of_chan_unit_check (
                                   p_aofb->channel,
                                   &p_aofb->xd_scale
                                 );

  Check_config_AOFB (p_aofb,p_aofb_data,p_block_instance->p_block_desc);

  return;
}



FUNCTION GLOBAL VOID Stop_AOFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_AOFB_DATA *               p_aofb_data;
  T_AOFB *                    p_aofb;
  FLOAT_S                     raw_sp;
  FLOAT_S                     raw_out;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_aofb_data   = p_block_instance->p_block_data;
  p_aofb        = p_block_desc->p_block;

  if (p_aofb->mode_blk.actual == MODE_OS) return;

  /* Check simulate parameter --------------------------------------------- */
  ffbl_bas_update_sim_param (&p_aofb->simulate.enable,&p_aofb->block_err);

  if (p_aofb->simulate.enable == SIMULATION_ENABLED) goto AFTER_TB_EXEC;

  if (p_aofb->mode_blk.actual != MODE_MAN)
  {
    /* Get setpoint --------------------------------------------------------- */
    if (p_aofb->io_opts & IO_OPT_FSTATE_TO_VAL)
    {
      raw_sp.value  = p_aofb->fstate_val;
      raw_sp.status = SQ_GOOD_CAS;
    }
    else
    {
      Get_setpoint (&raw_sp,p_aofb->sp);
    }

    if ((raw_sp.status & QUALITY_MASK) != SQ_GOOD_CAS) goto AFTER_TB_EXEC;


    /* Convert setpoint to PV scale ---------------------------------------- */
    raw_out.value = ffbl_bas_rescale_value (raw_sp.value,
                            &p_aofb->pv_scale, &p_aofb->xd_scale);



    /* Apply IO option 'Increase to close' -------------------------------- */
    if (p_aofb->io_opts & IO_OPT_INC_TO_CLOSE)
    {
      FLOAT  scale_100_0 = p_aofb->xd_scale.eu_100 - p_aofb->xd_scale.eu_0;
      FLOAT  temp        = (raw_out.value - p_aofb->xd_scale.eu_0)/scale_100_0;

      raw_out.value = (1 - temp)*scale_100_0 + p_aofb->xd_scale.eu_0;
    }

    p_aofb->out.value = raw_out.value;
  }
  else /* p_aofb->mode_blk.actual == MODE_MAN */
  {
    raw_out.value = p_aofb->out.value;
  }

  raw_out.status = SQ_GOOD_CAS|SUB_IFS;

  p_aofb_data->exec_tb.p_process_value = &raw_out;
  p_aofb_data->exec_tb.tb_error        = FALSE;

  /* Call transducer block ------------------------------------------------ */
  (*p_aofb_data->linked_tb.a_of_execute_tb)(
          p_aofb_data->linked_tb.p_tb_instance,
          &p_aofb_data->exec_tb
       );

  AFTER_TB_EXEC:

  p_aofb->mode_blk.actual = MODE_OS;

  /* Set block error ---------------------------------------------------- */
  p_aofb->block_err &= ~(BLK_ERR_LOCAL_OVERRIDE |
                             BLK_ERR_INPUT_FAILURE  |
                             BLK_ERR_OUTPUT_FAILURE   );
  p_aofb->block_err |= BLK_ERR_OUT_OF_SERVICE;


  /* Store out param in the internal block-specific data structure -------- */
  p_aofb_data->out         = p_aofb->out;

  /* Set status of OUT parameter to SQ_BAD|SUB_OUT_OF_SERVICE ----------- */
  p_aofb->out.status       = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_aofb->pv.status        = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_aofb->bkcal_out.status = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_aofb->rcas_out.status  = SQ_BAD|SUB_OUT_OF_SERVICE;


  /* Apply IO option 'Use Fault State value on restart' ----------------- */
  if (p_aofb->io_opts & IO_OPT_FSTATE_TO_VAL_ON_RESTART)
  {
    p_aofb->sp.value  = p_aofb->fstate_val;
    p_aofb->sp.status = SQ_GOOD_CAS|LIMIT_CONST;
  }


  /* Reset internal function block data ------------------------------- */
  Reset_AOFB (p_aofb_data);


  /* Disable update events ---------------------------------------------- */
  ffbl_disable_update_events (&p_aofb->update_evt);


  /* Update block alarm ----------------------------------------------- */
  Update_block_alarm_AOFB (p_block_desc, p_aofb, p_aofb_data);

  return;
}



FUNCTION GLOBAL USIGN16 Read_handler_AOFB
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
  _UNREF_PARAM (p_read);

  return (E_OK);
}






FUNCTION GLOBAL USIGN16 Write_handler_AOFB
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
  T_AOFB *                    p_aofb;
  T_AOFB_DATA *               p_aofb_data;
  USIGN8                      subindex;
  USIGN8 *                    source;
  USIGN8                      result;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_aofb_data   = p_block_instance->p_block_data;
  p_aofb        = p_block_desc->p_block;

  subindex      = p_write->subindex;
  source        = p_write->source;
  result        = E_OK;

  switch (p_write->rel_idx)
  {
    case AO_PAR_ALERT_KEY:
    {
      if (*source == 0) result = E_FB_PARA_CHECK;

      break;
    }

    case AO_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (
                 &p_aofb->mode_blk,
                 subindex,
                 source,
                 MODE_AO_VALID
               );
      break;
    }

    case AO_PAR_SP:
    {
      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //FLOAT *   a_of_value;
      USIGN8 *  a_of_value;
      FLOAT     fvalue;

      if (MODE_LOWER_THAN_AUTO (p_aofb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      //if      (subindex == 0) a_of_value = (FLOAT *)(source + 1);
      //else if (subindex == 2) a_of_value = (FLOAT *) source;
      if      (subindex == 0) a_of_value = source + 1;
      else if (subindex == 2) a_of_value = source;
      else                    return (E_FB_DATA_NO_WRITE);

      p_aofb->sp.status = SQ_GOOD_CAS;

      /* Check scale-limits: PV_SCALE +/-10% -------------------------- */
      //ffbl_bas_check_scale_limits (a_of_value,&p_aofb->pv_scale);
      memcpy (&fvalue, a_of_value, sizeof (fvalue));
      ffbl_bas_check_scale_limits (&fvalue,&p_aofb->pv_scale);

      /* fvalue could have been modified, copy it back to the telegram buffer */
      memcpy (a_of_value, &fvalue, sizeof (fvalue));

      break;
    }

    case AO_PAR_OUT:
    {
      if (MODE_LOWER_THAN_MAN (p_aofb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      if (subindex == 1) return (E_FB_DATA_NO_WRITE);

      break;
    }

    case AO_PAR_SIMULATE:
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

    case AO_PAR_PV_SCALE:
    {
      if (MODE_NOT_OS (p_aofb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      break;
    }

    case AO_PAR_XD_SCALE:
    {
      SCALE   xd_scale = p_aofb->xd_scale;

      if (MODE_NOT_OS (p_aofb->mode_blk.target))
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

      p_aofb_data->chan_unit_check = ffbl_call_fdev_funct.a_of_chan_unit_check (
                                       p_aofb->channel,
                                       &xd_scale
                                     );

      if (p_aofb_data->chan_unit_check == INVALID_ENGINEERING_UNIT)
        return (E_FB_PARA_CHECK);

      break;
    }

    case AO_PAR_GRANT_DENY:
    {
      ffbl_bas_write_grant_deny (subindex,source,&p_aofb->grant_deny);

      break;
    }

    case AO_PAR_IO_OPTS:
    {
      if (MODE_NOT_OS (p_aofb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //*((USIGN16 *)source) &= AO_IO_OPTS; /* Delete all undefined bits */
      WPHB_(source) &= CWHB_(AO_IO_OPTS); /* Delete all undefined bits */
      WPLB_(source) &= CWLB_(AO_IO_OPTS); /* Delete all undefined bits */

      break;
    }

    case AO_PAR_STATUS_OPTS:
    {
      if (MODE_NOT_OS (p_aofb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //*((USIGN16 *)source) &= AO_STATUS_OPTS;  /* Delete all undefined bits */
      WPHB_(source) &= CWHB_(AO_STATUS_OPTS);  /* Delete all undefined bits */
      WPLB_(source) &= CWLB_(AO_STATUS_OPTS);  /* Delete all undefined bits */

      break;
    }

    case AO_PAR_SP_RATE_DN:
    case AO_PAR_SP_RATE_UP:
    case AO_PAR_FSTATE_TIME:
    {
      if (*((FLOAT *)source) < 0)
        return (E_FB_PARA_LIMIT);

      break;
    }

    case AO_PAR_SP_HI_LIM:
    case AO_PAR_SP_LO_LIM:
    case AO_PAR_FSTATE_VAL:
    {
      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      USIGN8 *  a_of_value;
      FLOAT     fvalue;

      a_of_value = source;

      /* Check scale-limits: PV_SCALE +/-10% ------------------------------ */

      //ffbl_bas_check_scale_limits ((FLOAT *)source,&p_aofb->pv_scale);
      memcpy (&fvalue, a_of_value, sizeof (fvalue));
      ffbl_bas_check_scale_limits (&fvalue,&p_aofb->pv_scale);

      /* fvalue could have been modified, copy it back to the telegram buffer */
      memcpy (a_of_value, &fvalue, sizeof (fvalue));

      break;
    }

    case AO_PAR_CHANNEL:
    {

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      USIGN16   u16value;

      if (MODE_NOT_OS(p_aofb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      memcpy (&u16value, source, sizeof (u16value));
      //if (ffbl_bas_check_channel (*(USIGN16 *)source, p_block_desc))
      if (ffbl_bas_check_channel (u16value, p_block_desc))
        return E_FB_PARA_CHECK;

      //p_aofb_data->chan_unit_check =
      //      ffbl_call_fdev_funct.a_of_chan_unit_check
      //                      (*(USIGN16 *)source, &p_aofb->xd_scale);
      p_aofb_data->chan_unit_check =
            ffbl_call_fdev_funct.a_of_chan_unit_check
                            (u16value, &p_aofb->xd_scale);
      break;
    }

    case AO_PAR_RCAS_IN:
    {
      if (p_resource->write_lock == WRITE_LOCKED)
      {
        return E_FB_WRITE_LOCK; /* ITK, test case i1105_00 */
      }

      break;
    }

    case AO_PAR_SHED_OPT:
    {
      if ((*source < SHED_NORMAL_NORMAL_RETURN) ||
          (*source > SHED_TO_RETAINED_NO_RETURN)  )
        return (E_FB_PARA_CHECK);

      break;
    }

    case AO_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_aofb->update_evt.unack, source, subindex);

      break;
    }

    case AO_PAR_BLOCK_ALM:
    {
      result = ffbl_check_unack_flag (&p_aofb->block_alm.unack, source, subindex);
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

    switch (p_write->rel_idx)
    {
      case AO_PAR_BLK_DATA:
      {
        check_config = TRUE;    break;
      }

      case AO_PAR_MODE_BLK:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        p_aofb_data->mode_was_written = TRUE;
        check_config                  = TRUE;

        write_loc.rel_idx = 0;

        if (p_aofb->mode_blk.target & MODE_OS)
        {
          p_aofb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;

          write_loc.rel_idx = AO_PAR_OUT;
        }

        else if (p_aofb->mode_blk.target & MODE_MAN)
        {
          write_loc.rel_idx = AO_PAR_OUT;
        }

        if (write_loc.rel_idx)
        {
          /* Store out parameter in non-volatile RAM ---------------------- */
          write_loc.subindex      = 0;
          /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
          //write_loc.length        = 5;
          write_loc.length        = sizeof(p_aofb->out);
          write_loc.source        = (USIGN8 *)&p_aofb->out;
          write_loc.startup_sync  = FALSE;

          fbs_write_param_loc (p_block_instance,&write_loc);
        }

        break;
      }

      case AO_PAR_OUT:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        p_aofb->out.status = SQ_GOOD_NC;

        /* Store GOOD out status in non-volatile RAM ---------------------- */
        write_loc.rel_idx       = AO_PAR_OUT;
        write_loc.subindex      = 0;
        /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
        //write_loc.length        = 5;
        write_loc.length        = sizeof(p_aofb->out);
        write_loc.source        = (USIGN8 *)&p_aofb->out;
        write_loc.startup_sync  = FALSE;

        fbs_write_param_loc (p_block_instance,&write_loc);

        /* Store out param in the internal block-specific data structure -- */
        p_aofb_data->out = p_aofb->out;

        /* Adapt out status in accordance to the actual mode -------------- */
        if (p_aofb->mode_blk.actual == MODE_OS)
        {
          p_aofb->out.status = SQ_BAD|SUB_OUT_OF_SERVICE;
        }
        else
        {
          p_aofb->out.status |= LIMIT_CONST;
        }

        break;
      }

      case AO_PAR_RCAS_IN:
      {
        if ((p_aofb->mode_blk.target & MODE_RCAS)&&(*source & SQ_GOOD))
        {
          p_aofb_data->shed.active       = FALSE;
          p_aofb_data->shed.current_time = 0;
        }

        break;
      }

      case AO_PAR_CHANNEL:
      case AO_PAR_XD_SCALE:
      case AO_PAR_PV_SCALE:
      case AO_PAR_SHED_OPT:
      {
        check_config = TRUE;    break;
      }
    }

    if (check_config)
    {
      Check_config_AOFB(p_aofb,p_aofb_data,p_block_desc);
    }
  }

  return result;
}




FUNCTION GLOBAL USIGN16 Execute_AOFB
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
  T_AOFB *                    p_aofb;
  T_AOFB_DATA *               p_aofb_data;
  USIGN8                      old_mode;
  USIGN8                      actual_mode;
  FLOAT_S                     raw_sp;
  FLOAT_S                     raw_out;
  FLOAT_S                     raw_rb;
  BOOL                        enable_update_evt;
  USIGN8                      good_nc_substatus;
  BOOL                        init_cas;
  BOOL                        init_rcas;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_aofb_data   = p_block_instance->p_block_data;
  p_aofb        = p_block_desc->p_block;

  init_cas      = FALSE;
  init_rcas     = FALSE;

SetPA_5();

  /* Update simulate parameter -------------------------------------------- */
  ffbl_bas_update_sim_param (&p_aofb->simulate.enable,&p_aofb->block_err);

  /* Update block mode -------------------------------------------------- */
  old_mode = p_aofb->mode_blk.actual;

  if (p_aofb->block_err & BLK_ERR_BLOCK_CONFIG_ERR)
  {
    actual_mode = MODE_OS;
  }
  else
  {
    Calc_block_mode_AOFB (p_aofb,p_aofb_data);
    actual_mode = p_aofb->mode_blk.actual;
  }


  if (actual_mode == MODE_OS)
  {
    if (old_mode != MODE_OS)
    {
      /* Mode OS is entered now ------------------------------------------- */
      p_aofb->mode_blk.actual = old_mode;  /* Actual mode is checked and
                                              set again by the stop routine */
      Stop_AOFB (p_block_instance);
    }
    else
    {
      /* Update block alarm ----------------------------------------------- */
      Update_block_alarm_AOFB (p_block_desc, p_aofb, p_aofb_data);
    }

ResetPA_5();
    return (p_aofb->blk_data.next_fb_to_execute);
  }

  if (old_mode == MODE_OS) /* Mode OS is left now ------------------------- */
  {
    /* Reset block error -------------------------------------------------- */
    p_aofb->block_err &= ~BLK_ERR_OUT_OF_SERVICE;
    enable_update_evt  = TRUE;

    /* Restore out parameter ---------------------------------------------- */
    p_aofb->out = p_aofb_data->out;

    if ((p_aofb->out.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_BAD|SUB_OUT_OF_SERVICE))
    {
      p_aofb->out.status = SQ_BAD;
    }
  }
  else
  {
    enable_update_evt = FALSE;
  }


  /* Determine GOOD(NC) substatus ----------------------------------------- */
  if (p_aofb->block_alm.current == ALARM_ACTIVE)
  {
    good_nc_substatus = SUB_ACTIVE_BLOCK_ALARM;
  }
  else if (p_aofb->block_alm.unack == ALARM_UNACKED)
  {
    good_nc_substatus = SUB_UNACK_BLOCK_ALARM;
  }
  else
  {
    good_nc_substatus = 0;
  }


  /* Handle setpoint parameter -------------------------------------------- */
  /* ---------------------------------------------------------------------- */

  raw_sp.status = SQ_BAD;

  /* Apply IO option 'SP Track retained target' --------------------------- */
  if (p_aofb->io_opts & IO_OPT_SP_TRACK_RET_TARGET)
  {
    if (p_aofb->mode_blk.target & MODE_RCAS)
    {
      Get_setpoint (&raw_sp,p_aofb->rcas_in);

      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto END_OF_STORE_RETAINED_SP;
      }
    }

    if (p_aofb->mode_blk.target & MODE_CAS)
    {
      Get_setpoint (&raw_sp,p_aofb->cas_in);

      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto END_OF_STORE_RETAINED_SP;
      }
    }

    END_OF_STORE_RETAINED_SP: ;
  }



  if (actual_mode == MODE_CAS)
  {
    if (p_aofb_data->fault.reason == FAULT_STATE_CLEAR)
    {
      Get_setpoint (&raw_sp,p_aofb->cas_in);
    }
    else
    {
      /* A fault condition was detected but FSTATE_TIME isn't expired yet */

      raw_sp = p_aofb->sp;
    }
  }

  else if (actual_mode == MODE_RCAS)
  {
    if (p_aofb_data->fault.reason == FAULT_STATE_CLEAR)
    {
      Get_setpoint (&raw_sp,p_aofb->rcas_in);
    }
    else
    {
      /* A fault condition was detected but FSTATE_TIME isn't expired yet */

      raw_sp = p_aofb->sp;
    }
  }

  else if (actual_mode == MODE_AUTO)
  {
    if ((old_mode == MODE_LO)||(old_mode == MODE_IMAN))
    {
      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
      {
        goto AFTER_GET_SP_IN_AUTO;
      }
    }

    Get_setpoint (&raw_sp,p_aofb->sp);

    AFTER_GET_SP_IN_AUTO: ;
  }


  else if (actual_mode == MODE_MAN)
  {
    if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
    {
      goto AFTER_GET_SP_IN_MAN;
    }

    if (p_aofb->io_opts & IO_OPT_SP_PV_TRACK_IN_MAN)
    {
      Get_setpoint (&raw_sp,p_aofb->pv);

      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto AFTER_GET_SP_IN_MAN;
      }
    }

    Get_setpoint (&raw_sp,p_aofb->sp);

    AFTER_GET_SP_IN_MAN: ;
  }

  else if (actual_mode == MODE_IMAN)
  {
    if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
    {
      goto AFTER_GET_SP_IN_IMAN;
    }

    Get_setpoint (&raw_sp,p_aofb->sp);

    AFTER_GET_SP_IN_IMAN: ;
  }

  else /* (actual_mode == MODE_LO) */
  {
    if (p_aofb_data->fault.active)
    {
      if (p_aofb->io_opts & IO_OPT_FSTATE_TO_VAL)
      {
        raw_sp.value  = p_aofb->fstate_val;
        raw_sp.status = SQ_GOOD_CAS;

        goto AFTER_GET_SP_IN_LO;
      }
    }

    if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
    {
      goto AFTER_GET_SP_IN_LO;
    }

    if (p_aofb->io_opts & IO_OPT_SP_PV_TRACK_IN_LO)
    {
      Get_setpoint (&raw_sp,p_aofb->pv);

      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto AFTER_GET_SP_IN_LO;
      }
    }

    Get_setpoint (&raw_sp,p_aofb->sp);

    AFTER_GET_SP_IN_LO:

    if (p_aofb_data->fault.active)
    {
      if ((raw_sp.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        raw_sp.status = (SQ_GOOD_CAS|SUB_FSA|LIMIT_CONST);
      }
    }
  }


  /* Update setpoint parameter  ------------------------------------------ */
  p_aofb->sp = raw_sp;

  if (raw_sp.status == SQ_BAD)
  {
    goto END_OF_SP_HANDLING;
  }


  /* Check setpoint limits of SP parameter -------------------------------- */
  if ((p_aofb->sp.status & LIMIT_CONST) != LIMIT_CONST)
    p_aofb->sp.status &= ~LIMIT_CONST;  /* Reset limit bits ----------- */

  if (raw_sp.value <= p_aofb->sp_lo_lim)
  {
    raw_sp.value = p_aofb->sp.value = p_aofb->sp_lo_lim;
    p_aofb->sp.status |= LIMIT_LOW;
  }

  if (raw_sp.value >= p_aofb->sp_hi_lim)
  {
    raw_sp.value = p_aofb->sp.value = p_aofb->sp_hi_lim;
    p_aofb->sp.status |= LIMIT_HIGH;
  }


  if ((actual_mode >= MODE_AUTO)&&(p_aofb_data->last_sp.status != SQ_BAD))
  {
    /* Apply ramp function ------------------------------------------------ */
    FLOAT lim_up;
    FLOAT lim_dn;

    if (p_aofb->sp_rate_up > 0.0)
    {
      lim_up = p_aofb_data->last_sp.value + p_aofb->sp_rate_up * p_aofb_data->period_of_exec;

      if (raw_sp.value > lim_up)
      {
        raw_sp.value = lim_up; goto END_OF_RAMP;
      }
    }

    if (p_aofb->sp_rate_dn > 0.0)
    {
      lim_dn = p_aofb_data->last_sp.value - p_aofb->sp_rate_dn * p_aofb_data->period_of_exec;

      if (raw_sp.value < lim_dn)
      {
        raw_sp.value = lim_dn;
      }
    }
  }

  END_OF_RAMP:
  END_OF_SP_HANDLING:


  /* Now the raw SP is the working SP ------------------------------------ */
  p_aofb_data->last_sp = raw_sp;


  /* Update OUT parameter ------------------------------------------------- */
  if (actual_mode == MODE_MAN)
  {
    raw_out = p_aofb->out;

    if ((raw_out.status & QUALITY_MASK) != SQ_BAD) raw_out.status |= LIMIT_CONST;
  }
  else
  {
    /* Convert setpoint value to XD_scale --------------------------------- */
    raw_out.value  = ffbl_bas_rescale_value (raw_sp.value,
                            &p_aofb->pv_scale, &p_aofb->xd_scale);

    /* Propagate SP status to OUT status ---------------------------------- */
    raw_out.status = raw_sp.status & ~LIMIT_MASK;

    /* Map OUT good cascade status to OUT good non-cascade status --------- */
    if ((raw_out.status & QUALITY_MASK) == SQ_GOOD_CAS)
    {
      raw_out.status = (SQ_GOOD_NC|good_nc_substatus);
    }

    /* Apply IO option 'Increase to close' -------------------------------- */
    if (p_aofb->io_opts & IO_OPT_INC_TO_CLOSE)
    {
      USIGN8 limits      = raw_sp.status & LIMIT_MASK;
      FLOAT  scale_100_0 = p_aofb->xd_scale.eu_100 - p_aofb->xd_scale.eu_0;
      FLOAT  temp        = (raw_out.value - p_aofb->xd_scale.eu_0)/scale_100_0;

      raw_out.value = (1 - temp)*scale_100_0 + p_aofb->xd_scale.eu_0;

      if (limits)
      {
        raw_sp.status &= ~LIMIT_MASK;

        if (limits == LIMIT_HIGH) raw_sp.status |= LIMIT_LOW;
        else                      raw_sp.status |= LIMIT_HIGH;
      }
    }
  }


  /* Assign raw out to OUT parameter -------------------------------------- */
  p_aofb->out = raw_out;



  p_aofb->block_err &= ~BLK_ERR_OUTPUT_FAILURE; /* Reset set output failure
                                                       bit; setting may be forced
                                                       by the transducer block
                                                    */


  if (p_aofb->simulate.enable == SIMULATION_DISABLED)
  {
    /* Execute the transducer block --------------------------------------- */
    if ((raw_out.status & QUALITY_MASK) == SQ_GOOD_NC)
    {
      raw_out.status = SQ_GOOD_CAS;
    }

    if (p_aofb->mode_blk.actual == MODE_LO)
    {
      if (p_aofb_data->fault.active)
      {
        if (raw_out.status == SQ_GOOD_CAS)
        {
          raw_out.status = SQ_GOOD_CAS|SUB_FSA;
        }
      }
    }

    else if (p_aofb->mode_blk.target > MODE_AUTO)
    {
      USIGN8  not_invited = 0x00;

      if ((p_aofb->rcas_out.status & ~LIMIT_MASK) == (SQ_GOOD_CAS|SUB_IR))
      {
        init_rcas = TRUE;
      }
      else if ((p_aofb->rcas_out.status & ~LIMIT_MASK) != (SQ_GOOD_CAS|SUB_NON_SPECIFIC))
      {
        not_invited |= 0x01;
      }

      if ((p_aofb->bkcal_out.status & ~LIMIT_MASK) == (SQ_GOOD_CAS|SUB_IR))
      {
        if (init_rcas == FALSE)
        {
          init_cas = TRUE;
        }
      }
      else if ((p_aofb->bkcal_out.status & ~LIMIT_MASK) != (SQ_GOOD_CAS|SUB_NON_SPECIFIC))
      {
        not_invited |= 0x02;
      }

      if (init_rcas || init_cas || (not_invited == 0x03))
      {
        /* Traget mode is not reached ------------------------------------- */
        if (p_aofb_data->old_out.status == SQ_GOOD_CAS)
        {
          raw_out.status = SQ_UNCERTAIN|SUB_LUV;
          raw_out.value  = p_aofb_data->old_out.value;
        }
        else
        {
          if (raw_out.status == SQ_GOOD_CAS)
          {
            raw_out.status = SQ_UNCERTAIN|SUB_INITIAL_VALUE;
          }
        }
      }
    }

    if ((raw_out.status & QUALITY_MASK) == SQ_GOOD_CAS)
    {
      /* Store a usable out-value ----------------------------------------- */
      p_aofb_data->old_out.status = SQ_GOOD_CAS;
      p_aofb_data->old_out.value  = raw_out.value;
    }

    p_aofb_data->exec_tb.p_process_value = &raw_out;
    p_aofb_data->exec_tb.tb_error        = FALSE;

    (*p_aofb_data->linked_tb.a_of_execute_tb)(
            p_aofb_data->linked_tb.p_tb_instance,
            &p_aofb_data->exec_tb
         );

    if (p_resource->feature_sel & FEAT_OUTPUT_READBACK_SUPP)
    {
      raw_rb = raw_out; /* Readback from transducer block */

      if ((raw_out.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        raw_rb.status = SQ_GOOD_NC;
      }
    }
    else /* Readback not supported */
    {
      raw_rb = p_aofb->out; /* Readback from OUT parameter */
    }

    p_aofb->simulate.field_value  = raw_rb.value;
    p_aofb->simulate.field_status = raw_rb.status;
    p_aofb->simulate.value        = raw_rb.value;
    p_aofb->simulate.status       = raw_rb.status;

    p_aofb_data->last_tb_status   = raw_out.status;

    if (p_aofb_data->exec_tb.tb_error) p_aofb->block_err |= BLK_ERR_OUTPUT_FAILURE;
  }

  else /* Simulation is active -------------------------------------------- */
  {
    p_aofb->simulate.field_value  = raw_out.value;
    p_aofb->simulate.field_status = raw_out.status;
    raw_rb.value                  = p_aofb->simulate.value;
    raw_rb.status                 = p_aofb->simulate.status;

    p_aofb_data->last_tb_status   = SQ_GOOD_CAS;
                       /* Prevent block to go to IMAN when simulate enabled */
  }


  /* Assign raw readback to READBACK parameter ---------------------------- */
  p_aofb->readback = raw_rb;


  /* Apply IO option 'Increase to close' ---------------------------------- */
  if (p_aofb->io_opts & IO_OPT_INC_TO_CLOSE)
  {
    FLOAT  scale_100_0 = p_aofb->xd_scale.eu_100 - p_aofb->xd_scale.eu_0;
    FLOAT  temp = (raw_rb.value - p_aofb->xd_scale.eu_0)/scale_100_0;

    raw_rb.value = (1 - temp)*scale_100_0 + p_aofb->xd_scale.eu_0;
  }


  /* Update PV ------------------------------------------------------------ */
  p_aofb->pv.value  = ffbl_bas_rescale_value (raw_rb.value,
                                &p_aofb->xd_scale, &p_aofb->pv_scale);


  if ((raw_rb.status & QUALITY_MASK) == SQ_BAD)
  {
    USIGN8 substatus = raw_rb.status & SUB_STATUS_MASK;

    if ((substatus != SUB_DEVICE_FAILURE)&&(substatus != SUB_SENSOR_FAILURE))
    {
      raw_rb.status = SQ_BAD|SUB_NON_SPECIFIC;
    }
  }
  else
  {
    if ((raw_rb.status & QUALITY_MASK) == SQ_GOOD_NC)
    {
      raw_rb.status &= ~SUB_STATUS_MASK;
      raw_rb.status |= good_nc_substatus;
    }
  }

  p_aofb->pv.status = raw_rb.status;


  /* Check actuator input ------------------------------------------------- */
  if (p_resource->feature_sel & FEAT_OUTPUT_READBACK_SUPP)
  {
    if ((p_aofb->pv.status & QUALITY_MASK) == SQ_BAD)
    {
      if (!(p_aofb->block_err & BLK_ERR_INPUT_FAILURE))
      {
        p_aofb->block_err |= BLK_ERR_INPUT_FAILURE;
      }

      /* Apply status option 'Propagate Fault Forward' -------------------- */
      if (p_aofb->status_opts & STATUS_OPT_PROP_FLT_BACKW)
      {
        p_aofb->bkcal_out.status = p_aofb->pv.status;
        p_aofb->rcas_out.status  = p_aofb->pv.status;
      }
    }
    else /* (p_aofb->pv.status & QUALITY_MASK) != SQ_BAD) */
    {
      if (p_aofb->block_err & BLK_ERR_INPUT_FAILURE)
      {
        p_aofb->block_err &= ~BLK_ERR_INPUT_FAILURE;
      }
    }
  } /* (p_resource->feature_sel & FEAT_OUTPUT_READBACK_SUPP) */



  /* Check fault state conditions ----------------------------------------- */
  if (actual_mode != MODE_LO)
  {
    /* Fault state is not active------------------------------------------- */

    if (p_aofb->block_err & BLK_ERR_LOCAL_OVERRIDE)
    {
      /* Fault state was left during the current block execution ---------- */

      p_aofb->block_err &= ~BLK_ERR_LOCAL_OVERRIDE;
    }
  }
  else /* (actual_mode == MODE_LO) */
  {
    /* Fault state is active ---------------------------------------------- */

    if (!(p_aofb->block_err & BLK_ERR_LOCAL_OVERRIDE))
    {
      /* Fault state was entered during the current block execution ------- */

      p_aofb->block_err |= BLK_ERR_LOCAL_OVERRIDE;
    }

    if (p_aofb->status_opts & STATUS_OPT_PROP_FLT_BACKW)
    {
      p_aofb->bkcal_out.status = SQ_GOOD_CAS|SUB_FSA;
      p_aofb->rcas_out.status  = SQ_GOOD_CAS|SUB_FSA;
    }
    else
    {
      p_aofb->bkcal_out.status = SQ_GOOD_CAS|SUB_NI;
      p_aofb->rcas_out.status  = SQ_GOOD_CAS|SUB_NI;
    }
  } /* (actual_mode == MODE_LO) */



  /* Update BKCAL_OUT and RCAS_OUT ---------------------------------------- */
  p_aofb->bkcal_out.status &= ~LIMIT_MASK;
  p_aofb->rcas_out.status  &= ~LIMIT_MASK;

  if (p_aofb->io_opts & IO_OPT_PV_FOR_BKCAL_OUT)
  {
    p_aofb->bkcal_out.value = p_aofb->pv.value;
    p_aofb->rcas_out.value  = p_aofb->pv.value;

    if ((p_aofb->pv.status & QUALITY_MASK) == SQ_BAD)
    {
      if (!(p_aofb->status_opts & STATUS_OPT_PROP_FLT_BACKW))
      {
        p_aofb->bkcal_out.status = p_aofb->pv.status;
        p_aofb->rcas_out.status  = p_aofb->pv.status;
      }
    }
  }
  else /* (!(p_aofb->io_opts & IO_OPT_PV_FOR_BKCAL_OUT)) */
  {
    p_aofb->bkcal_out.value   = raw_sp.value;
    p_aofb->rcas_out.value    = raw_sp.value;

    if ( init_rcas )
    {
      init_rcas = FALSE;

      if ((p_aofb->sp.status & QUALITY_MASK) == SQ_BAD)
      {
        p_aofb->bkcal_out.status = p_aofb->sp.status;
      }
    }
    else if ( init_cas )
    {
      init_cas = FALSE;

      if ((p_aofb->sp.status & QUALITY_MASK) == SQ_BAD)
      {
        p_aofb->rcas_out.status = p_aofb->sp.status;
      }
    }
    else /* No cascade initialization */
    {
      if ((p_aofb->sp.status & QUALITY_MASK) == SQ_BAD)
      {
        p_aofb->bkcal_out.status = p_aofb->sp.status;
        p_aofb->rcas_out.status  = p_aofb->sp.status;
      }
    }
  }

  if (actual_mode == MODE_IMAN)
  {
    if ((p_aofb->pv.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_GOOD_CAS|SUB_LO))
    {
      if (p_aofb->status_opts & STATUS_OPT_PROP_FLT_BACKW)
      {
        p_aofb->bkcal_out.status = (SQ_GOOD_CAS|SUB_LO);
      }
    }
  }

  /* Last but not least set the limit bits of the back calculation parameters
     in accordance with test i3300_00 ------------------------------------- */
  p_aofb->bkcal_out.status |= (p_aofb->sp.status & LIMIT_MASK);
  p_aofb->rcas_out.status  |= (p_aofb->sp.status & LIMIT_MASK);

  if (p_aofb->io_opts & IO_OPT_PV_FOR_BKCAL_OUT)
  {
    p_aofb->bkcal_out.status |= (p_aofb->pv.status & LIMIT_MASK);
    p_aofb->rcas_out.status  |= (p_aofb->pv.status & LIMIT_MASK);
  }

  /* Update block alarm --------------------------------------------------- */
  Update_block_alarm_AOFB (p_block_desc, p_aofb, p_aofb_data);

  if (enable_update_evt)
  {
    /* Enable update events --------------------------------------------- */
    ffbl_enable_update_events
    (
      p_block_desc->block_id,p_block_desc->idx_upd_evt,
      &p_aofb->update_evt,p_aofb->st_rev
    );
  }

ResetPA_5();

  return (p_aofb->blk_data.next_fb_to_execute);
}







FUNCTION GLOBAL VOID Background_AOFB
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
  T_AOFB_DATA *             p_aofb_data;
  T_FBS_WRITE_PARAM_LOCAL   write_loc;

FUNCTION_BODY

  p_aofb_data = p_block_instance->p_block_data;

  _ASSERT (p_aofb_data != NULL);

  if (p_aofb_data->new_target_mode)
  {
    write_loc.rel_idx       = AO_PAR_MODE_BLK;
    write_loc.subindex      = 1;
    write_loc.length        = 1;
    write_loc.source        = &p_aofb_data->new_target_mode;
    write_loc.startup_sync  = FALSE;

    fbs_write_param_loc (p_block_instance,&write_loc);

    if ((p_aofb_data->new_target_mode == MODE_OS) ||
        (p_aofb_data->new_target_mode == MODE_MAN)  )
    {
      write_loc.rel_idx = AO_PAR_OUT;
    }
    else
    {
      write_loc.rel_idx = 0;
    }

    p_aofb_data->new_target_mode = 0;

    if (write_loc.rel_idx)
    {
      /* Store out parameter in non-volatile RAM ---------------------- */
      T_AOFB *  p_aofb = p_block_instance->p_block_desc->p_block;

      write_loc.subindex      = 0;
      write_loc.length        = 5;
      write_loc.source        = (USIGN8 *)&p_aofb->out;

      fbs_write_param_loc (p_block_instance,&write_loc);
    }
  }

  return;
}







FUNCTION LOCAL VOID Calc_block_mode_AOFB
  (
    IN T_AOFB *         p_aofb,
    IN T_AOFB_DATA *    p_aofb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  mo.profile             = AO_PROFILE_NUMBER;
  mo.period_of_execution = p_aofb->blk_data.period_of_execution;
  mo.channel             = p_aofb->channel;
  mo.shed_opt            = p_aofb->shed_opt;
  mo.io_opts             = p_aofb->io_opts;
  mo.mode                = p_aofb->mode_blk;
  mo.fstate_time         = p_aofb->fstate_time;
  mo.cas_in_status       = p_aofb->cas_in.status;
  mo.rcas_in_status      = p_aofb->rcas_in.status;
  mo.cas_out_status      = p_aofb->bkcal_out.status;
  mo.rcas_out_status     = p_aofb->rcas_out.status;

  mo.mode_was_written    = p_aofb_data->mode_was_written;
  mo.fault               = p_aofb_data->fault;
  mo.shed                = p_aofb_data->shed;
  mo.rout_in_status      = p_aofb_data->last_tb_status; /* Misuse of 'rout_in_status' */

  Calc_block_mode ();

  p_aofb_data->mode_was_written = FALSE;
  p_aofb->mode_blk              = mo.mode;
  p_aofb_data->fault            = mo.fault;
  p_aofb_data->shed             = mo.shed;
  p_aofb->bkcal_out.status      = mo.cas_out_status;
  p_aofb->rcas_out.status       = mo.rcas_out_status;

  if (!p_aofb_data->new_target_mode && mo.new_target_mode)
  {
    p_aofb_data->new_target_mode = mo.new_target_mode;
  }

  return;
}






FUNCTION LOCAL VOID Reset_AOFB
  (
    IN T_AOFB_DATA *   p_aofb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Reset block specific data of a AO block.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  p_aofb_data->mode_was_written   = FALSE;
  p_aofb_data->new_target_mode    = 0;
  p_aofb_data->shed.active        = FALSE;
  p_aofb_data->shed.current_time  = 0;
  p_aofb_data->fault.reason       = FAULT_STATE_CLEAR;
  p_aofb_data->fault.active       = FALSE;
  p_aofb_data->fault.no_of_cycles = 0;
  p_aofb_data->last_tb_status     = SQ_BAD;
  p_aofb_data->old_out.status     = SQ_BAD;

  return;
}





FUNCTION LOCAL VOID Update_block_alarm_AOFB
  (
    IN const T_FBIF_BLOCK_DESCR * p_block_desc,
    INOUT T_AOFB *                p_aofb,
    INOUT T_AOFB_DATA *           p_aofb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16   alarm_bits;

FUNCTION_BODY

  if (p_aofb->block_err & BLK_ERR_OUT_OF_SERVICE)
  {
    alarm_bits = BLK_ERR_OUT_OF_SERVICE;
  }
  else
  {
    alarm_bits = p_aofb->block_err;

    /* Apply status option 'Propagate Fault Forward' ------------------------ */
    if (p_aofb->status_opts & STATUS_OPT_PROP_FLT_BACKW)
    {
      alarm_bits &= ~(BLK_ERR_INPUT_FAILURE|BLK_ERR_LOCAL_OVERRIDE);
    }
  }

  /* Update block alarm --------------------------------------------------- */
  ffbl_update_block_alarm
  (
    p_block_desc->block_id,p_block_desc->idx_blk_alm,
    alarm_bits,&p_aofb_data->old_block_alarm_bits
  );

  return;
}







FUNCTION LOCAL VOID Check_config_AOFB
  (
    IN T_AOFB *                     p_aofb,
    IN T_AOFB_DATA *                p_aofb_data,
    IN const T_FBIF_BLOCK_DESCR *   p_block_desc
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN32     block_err_desc;

FUNCTION_BODY

  block_err_desc = BLK_ERR_NO_ERROR;

  if (p_aofb_data->chan_unit_check != CHANNEL_UNIT_OK)
  {
    block_err_desc |= BLK_ERR_CH_UNIT;
  }


  if ((p_aofb->mode_blk.target & MODE_RCAS)&&(p_aofb->shed_opt == 0))
  {
    block_err_desc |= BLK_ERR_SHED_OPT_SET;
  }


  if (p_aofb->blk_data.period_of_execution == 0)
  {
    block_err_desc |= BLK_ERR_EXE_TIME;
  }

  /* Check if eu_100 is equal or less than eu_0 --------------------------- */
  if (p_aofb->pv_scale.eu_100 <= p_aofb->pv_scale.eu_0)
  {
    block_err_desc |= BLK_ERR_PV_SCALE_LIM_SET;
  }

  if (p_aofb->xd_scale.eu_100 <= p_aofb->xd_scale.eu_0)
  {
    block_err_desc |= BLK_ERR_XD_SCALE_LIM_SET;
  }


  /* Check if the AO block is linked to a transducer block ---------------- */
  if (p_aofb->channel == 0)
  {
    block_err_desc |= BLK_ERR_CHANNEL_INIT;
  }

  /* No config error; update AO data -------------------------------------- */
  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_aofb_data->exec_tb.channel = fbs_link_transducer_block
                    (p_aofb->channel,p_block_desc,&p_aofb_data->linked_tb);

    if (p_aofb_data->exec_tb.channel != 0)
    {
      /* Convert period of execution from 1/32 ms to sec ------------------ */
      p_aofb_data->period_of_exec = p_aofb->blk_data.period_of_execution / 32000.0f;
    }
    else
    {
      block_err_desc |= BLK_ERR_CHANNEL_SETTING;
    }
  }


  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_aofb->block_err &= ~BLK_ERR_BLOCK_CONFIG_ERR;
  }
  else
  {
    p_aofb->block_err |= BLK_ERR_BLOCK_CONFIG_ERR;
  }

  p_aofb->block_err_desc_1 = block_err_desc;

  return;
}

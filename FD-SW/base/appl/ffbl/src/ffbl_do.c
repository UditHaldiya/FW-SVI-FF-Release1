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

FILE_NAME          ffbl_do.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKDO)

INCLUDES

  /*@Dresser-Masoneilan project */
  #include <string.h>
  
  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbs_api.h"
  #include "ffbl_res.h"
  #include "ffbl_int.h"
  #include "ffbl_do.h"
  #include "ffbl_mo.h"
  #include "fbif_idx.h"
  #include "..\..\fdev\inc\mn_errorbits.h"
  #include "fbif_cfg.h"

LOCAL_DEFINES

/*--------------------- OUTPUT VALUE ----------------------*/
#define MN_ONEHUNDRED              100u
#define MN_ZERO                    0u
#define MN_ONE                     1u

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID Reset_DOFB (T_DOFB_DATA *);
  FUNCTION LOCAL VOID Check_config_DOFB (T_DOFB *, T_DOFB_DATA *, const T_FBIF_BLOCK_DESCR *);
  FUNCTION LOCAL VOID Get_setpoint_d (DISCRETE_S *, DISCRETE_S);
  FUNCTION LOCAL VOID Calc_block_mode_DOFB (T_DOFB *, T_DOFB_DATA *);
  FUNCTION LOCAL VOID Update_block_alarm_DOFB (const T_FBIF_BLOCK_DESCR *, T_DOFB *,T_DOFB_DATA *);
  FUNCTION LOCAL u16 mn_ConflictOtherDOChannels(const T_FBIF_BLOCK_DESCR *p_block_desc);

IMPORT_DATA

EXPORT_DATA

LOCAL_DATA


/****************************************************************************/

FUNCTION GLOBAL VOID Start_DOFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_DOFB_DATA *       p_dofb_data;
  T_DOFB *            p_dofb;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_DOFB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_dofb_data   = p_block_instance->p_block_data;
  p_dofb        = p_block_instance->p_block_desc->p_block;


  /* Initialize internal function block data ------------------------------ */
  p_dofb_data->old_block_alarm_bits = 0;

  Reset_DOFB (p_dofb_data);

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_dofb->update_evt);

  /* Store out param in the internal block-specific data structure -------- */
  p_dofb_data->out_d    = p_dofb->out_d;

  /* Set out status to BAD out-of-service --------------------------------- */
  p_dofb->out_d.status  = SQ_BAD|SUB_OUT_OF_SERVICE;


  /* Apply IO option 'Use Fault State value on restart' ------------------- */
  if (p_dofb->io_opts & IO_OPT_FSTATE_TO_VAL_ON_RESTART)
  {
    p_dofb->sp_d.value  = p_dofb->fstate_val_d;
    p_dofb->sp_d.status = SQ_GOOD_CAS|LIMIT_CONST;
  }

  /* Check function block configuration ----------------------------------- */
  Check_config_DOFB (p_dofb,p_dofb_data,p_block_instance->p_block_desc);

  return;
}



FUNCTION GLOBAL VOID Stop_DOFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_DOFB_DATA *               p_dofb_data;
  T_DOFB *                    p_dofb;
  DISCRETE_S                  raw_out_d;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_dofb_data   = p_block_instance->p_block_data;
  p_dofb        = p_block_desc->p_block;

  if (p_dofb->mode_blk.actual == MODE_OS) return;

  /* Check simulate parameter --------------------------------------------- */
  ffbl_bas_update_sim_param (&p_dofb->simulate_d.enable,&p_dofb->block_err);

  if (p_dofb->simulate_d.enable == SIMULATION_ENABLED) goto AFTER_TB_EXEC;

  if (p_dofb->mode_blk.actual != MODE_MAN)
  {
    /* Get setpoint (raw_out_d) --------------------------------------------- */
    if (p_dofb->io_opts & IO_OPT_FSTATE_TO_VAL)
    {
      raw_out_d.value  = p_dofb->fstate_val_d;
      raw_out_d.status = SQ_GOOD_CAS;
    }
    else
    {
      Get_setpoint_d (&raw_out_d,p_dofb->sp_d);
    }

    if ((raw_out_d.status & QUALITY_MASK) != SQ_GOOD_CAS) goto AFTER_TB_EXEC;


    /* Apply IO option 'Invert' ------------------------------------------- */
    if ((p_dofb->io_opts & IO_OPT_INVERT) != MN_ZERO)
    {
        if (p_dofb->channel == CH_POSITION_DISCRETE_POSITION)
        {
            if (raw_out_d.value >= MN_ONEHUNDRED)
            {
                raw_out_d.value = MN_ZERO;
            }
            else
            {
                raw_out_d.value = MN_ONEHUNDRED - raw_out_d.value;
            }
        }
        else
        {
            if (raw_out_d.value)
            {
                raw_out_d.value = MN_ZERO;
            }
            else
            {
                raw_out_d.value = MN_ONE;
            }
        }

    }

    p_dofb->out_d.value = raw_out_d.value;
  }
  else /* p_dofb->mode_blk.actual == MODE_MAN */
  {
    raw_out_d.value = p_dofb->out_d.value;
  }

  raw_out_d.status = SQ_GOOD_CAS|SUB_IFS;

  p_dofb_data->exec_tb.p_process_value = &raw_out_d;

  /* Call transducer block ------------------------------------------------ */
  (*p_dofb_data->linked_tb.a_of_execute_tb)(
          p_dofb_data->linked_tb.p_tb_instance,
          &p_dofb_data->exec_tb
       );

  AFTER_TB_EXEC:

  p_dofb->mode_blk.actual = MODE_OS;

  /* Set block error ---------------------------------------------------- */
  p_dofb->block_err &= ~(BLK_ERR_LOCAL_OVERRIDE |
                             BLK_ERR_INPUT_FAILURE  |
                             BLK_ERR_OUTPUT_FAILURE   );
  p_dofb->block_err |= BLK_ERR_OUT_OF_SERVICE;


  /* Store out param in the internal block-specific data structure -------- */
  p_dofb_data->out_d         = p_dofb->out_d;

  /* Set status of OUT parameter to SQ_BAD|SUB_OUT_OF_SERVICE ----------- */
  p_dofb->out_d.status       = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_dofb->pv_d.status        = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_dofb->bkcal_out_d.status = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_dofb->rcas_out_d.status  = SQ_BAD|SUB_OUT_OF_SERVICE;


  /* Apply IO option 'Use Fault State value on restart' ----------------- */
  if (p_dofb->io_opts & IO_OPT_FSTATE_TO_VAL_ON_RESTART)
  {
    p_dofb->sp_d.value  = p_dofb->fstate_val_d;
    p_dofb->sp_d.status = SQ_GOOD_CAS|LIMIT_CONST;
  }


  /* Reset internal function block data ------------------------------- */
  Reset_DOFB (p_dofb_data);


  /* Disable update events ---------------------------------------------- */
  ffbl_disable_update_events (&p_dofb->update_evt);

  /* Update block alarm --------------------------------------------------- */
  Update_block_alarm_DOFB (p_block_desc, p_dofb, p_dofb_data);

  return;
}



FUNCTION GLOBAL USIGN16 Read_handler_DOFB
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







FUNCTION GLOBAL USIGN16 Write_handler_DOFB
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
  T_DOFB_DATA *               p_dofb_data;
  T_DOFB *                    p_dofb;
  USIGN8                      subindex;
  USIGN8 *                    source;
  USIGN8                      result;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_dofb_data   = p_block_instance->p_block_data;
  p_dofb        = p_block_desc->p_block;

  subindex      = p_write->subindex;
  source        = p_write->source;
  result        = E_OK;

  switch (p_write->rel_idx)
  {
    case DO_PAR_ALERT_KEY:
    {
      if (*source == 0) result = E_FB_PARA_CHECK;

      break;
    }

    case DO_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (
                 &p_dofb->mode_blk,
                 subindex,
                 source,
                 MODE_DO_VALID
              );
      break;
    }

    case DO_PAR_SP_D:
    {
      if (MODE_LOWER_THAN_AUTO (p_dofb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      if (subindex == 1) return (E_FB_DATA_NO_WRITE);

      p_dofb->sp_d.status = SQ_GOOD_CAS;

      break;
    }

    case DO_PAR_OUT_D:
    {
      if (MODE_LOWER_THAN_MAN (p_dofb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      if (subindex == 1) return (E_FB_DATA_NO_WRITE);

      break;
    }

    case DO_PAR_SIMULATE_D:
    {
      USIGN8 enable_flag;

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //if      (subindex == 0) enable_flag = ((SIMULATE_DISCRETE *)source)->enable;
      if      (subindex == 0) enable_flag = source[4];
      else if (subindex == 5) enable_flag = *source;
      else                    break;

      if (ffbl_bas_check_sim_param (enable_flag) == E_FB_PARA_CHECK)
        return (E_FB_PARA_CHECK);

      break;
    }

    case DO_PAR_GRANT_DENY:
    {
      ffbl_bas_write_grant_deny (subindex,source,&p_dofb->grant_deny);

      break;
    }

    case DO_PAR_IO_OPTS:
    {
      if (MODE_NOT_OS (p_dofb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //*((USIGN16 *)source) &= DO_IO_OPTS; /* Delete all undefined bits */
      WPHB_(source) &= CWHB_(DO_IO_OPTS); /* Delete all undefined bits */
      WPLB_(source) &= CWLB_(DO_IO_OPTS); /* Delete all undefined bits */

      break;
    }

    case DO_PAR_STATUS_OPTS:
    {
      if (MODE_NOT_OS (p_dofb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      //*((USIGN16 *)source) &= DO_STATUS_OPTS;  /* Delete all undefined bits */
      WPHB_(source) &= CWHB_(DO_STATUS_OPTS);  /* Delete all undefined bits */
      WPLB_(source) &= CWLB_(DO_STATUS_OPTS);  /* Delete all undefined bits */

      break;
    }

    case DO_PAR_CHANNEL:
    {
      /*@Dresser-Masoneilan project: 32bit Alignment adaptation */
      USIGN16   u16value;
      
      if (MODE_NOT_OS (p_dofb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      memcpy (&u16value, source, sizeof (u16value));
      //if ( ffbl_bas_check_channel (*(USIGN16 *)source, p_block_desc))
      if ( ffbl_bas_check_channel (u16value, p_block_desc))
        return E_FB_PARA_CHECK;

      break;
    }

    case DO_PAR_FSTATE_TIME:
    {
      if (*((FLOAT *)source) < 0)
        return (E_FB_PARA_LIMIT);

      break;
    }

    case DO_PAR_RCAS_IN_D:
    {
      if (p_resource->write_lock == WRITE_LOCKED)
      {
        return E_FB_WRITE_LOCK; /* ITK, test case i1105_05 */
      }

      break;
    }

    case DO_PAR_SHED_OPT:
    {
      if ((*source < SHED_NORMAL_NORMAL_RETURN) ||
          (*source > SHED_TO_RETAINED_NO_RETURN)  )
        return (E_FB_PARA_CHECK);

      break;
    }

    case DO_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_dofb->update_evt.unack, source, subindex);

      break;
    }

    case DO_PAR_BLOCK_ALM:
    {
      result = ffbl_check_unack_flag (&p_dofb->block_alm.unack, source, subindex);
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
      case DO_PAR_BLK_DATA:
      {
        check_config = TRUE;     break;
      }

      case DO_PAR_MODE_BLK:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        p_dofb_data->mode_was_written = TRUE;
        check_config                  = TRUE;

        write_loc.rel_idx = 0;

        if (p_dofb->mode_blk.target & MODE_OS)
        {
          p_dofb->out_d.status = SQ_BAD|SUB_OUT_OF_SERVICE;

          write_loc.rel_idx = DO_PAR_OUT_D;
        }

        else if (p_dofb->mode_blk.target & MODE_MAN)
        {
          write_loc.rel_idx = DO_PAR_OUT_D;
        }

        if (write_loc.rel_idx)
        {
          /* Store out parameter in non-volatile RAM ---------------------- */
          write_loc.subindex      = 0;
          write_loc.length        = 2;
          write_loc.source        = (USIGN8 *)&p_dofb->out_d;
          write_loc.startup_sync  = FALSE;

          fbs_write_param_loc (p_block_instance,&write_loc);
        }

        break;
      }

      case DO_PAR_OUT_D:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        p_dofb->out_d.status = SQ_GOOD_NC;

        /* Store GOOD out status in non-volatile RAM ---------------------- */
        write_loc.rel_idx       = DO_PAR_OUT_D;
        write_loc.subindex      = 0;
        write_loc.length        = 2;
        write_loc.source        = (USIGN8 *)&p_dofb->out_d;
        write_loc.startup_sync  = FALSE;

        fbs_write_param_loc (p_block_instance,&write_loc);

        /* Store out param in the internal block-specific data structure -- */
        p_dofb_data->out_d      = p_dofb->out_d;

        /* Adapt out status in accordance to the actual mode -------------- */
        if (p_dofb->mode_blk.actual == MODE_OS)
        {
          p_dofb->out_d.status  = SQ_BAD|SUB_OUT_OF_SERVICE;
        }
        else
        {
          p_dofb->out_d.status |= LIMIT_CONST;
        }

        break;
      }

      case DO_PAR_RCAS_IN_D:
      {
        if ((p_dofb->mode_blk.target & MODE_RCAS)&&(*source & SQ_GOOD))
        {
          p_dofb_data->shed.active       = FALSE;
          p_dofb_data->shed.current_time = 0;
        }

        break;
      }

      case DO_PAR_CHANNEL:
      case DO_PAR_SHED_OPT:
      {
        check_config = TRUE;     break;
      }
    }

    if (check_config)
    {
      Check_config_DOFB (p_dofb,p_dofb_data,p_block_desc);
    }
  }

  return result;
}







FUNCTION GLOBAL USIGN16 Execute_DOFB
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
  T_DOFB_DATA *               p_dofb_data;
  T_DOFB *                    p_dofb;
  USIGN8                      old_mode;
  USIGN8                      actual_mode;
  DISCRETE_S                  raw_sp_d;
  DISCRETE_S                  raw_out_d;
  DISCRETE_S                  raw_rb_d;
  BOOL                        enable_update_evt;
  USIGN8                      good_nc_substatus;
  BOOL                        init_cas;
  BOOL                        init_rcas;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_dofb_data   = p_block_instance->p_block_data;
  p_dofb        = p_block_desc->p_block;
  init_cas      = FALSE;
  init_rcas     = FALSE;

  /* Update simulate parameter -------------------------------------------- */
  ffbl_bas_update_sim_param (&p_dofb->simulate_d.enable,&p_dofb->block_err);


  /* Update block mode -------------------------------------------------- */
  old_mode = p_dofb->mode_blk.actual;

  if (p_dofb->block_err & BLK_ERR_BLOCK_CONFIG_ERR)
  {
    actual_mode = MODE_OS;
  }
  else
  {
    Calc_block_mode_DOFB (p_dofb,p_dofb_data);
    actual_mode = p_dofb->mode_blk.actual;
  }


  if (actual_mode == MODE_OS)
  {
    if (old_mode != MODE_OS)
    {
      /* Mode OS is entered now ------------------------------------------- */
      p_dofb->mode_blk.actual = old_mode;  /* Actual mode is checked and
                                              set again by the stop routine */
      Stop_DOFB (p_block_instance);
    }
    else
    {
      /* Update block alarm ----------------------------------------------- */
      Update_block_alarm_DOFB (p_block_desc, p_dofb, p_dofb_data);
    }

    return (p_dofb->blk_data.next_fb_to_execute);
  }


  if (old_mode == MODE_OS) /* Mode OS is left now ------------------------- */
  {
      //before switching from OOS check if no conflict with the other DO channel
      mn_ErrorBits_Set(SP_SOURCE_ERRORBIT, FALSE); //clear the error bit

      if ( mn_ConflictOtherDOChannels(p_block_desc) )
      {
          p_dofb->mode_blk.actual = MODE_OS;
          mn_ErrorBits_Set(SP_SOURCE_ERRORBIT, TRUE); //set error bit if needed
          return (p_dofb->blk_data.next_fb_to_execute);
      }

    /* Reset block error -------------------------------------------------- */
    p_dofb->block_err &= ~BLK_ERR_OUT_OF_SERVICE;
    enable_update_evt  = TRUE;

    /* Restore out parameter ---------------------------------------------- */
    p_dofb->out_d= p_dofb_data->out_d;

    if ((p_dofb->out_d.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_BAD|SUB_OUT_OF_SERVICE))
    {
      p_dofb->out_d.status = SQ_BAD;
    }
  }
  else
  {
    enable_update_evt = FALSE;
  }


  /* Determine GOOD(NC) substatus ----------------------------------------- */
  if (p_dofb->block_alm.current == ALARM_ACTIVE)
  {
    good_nc_substatus = SUB_ACTIVE_BLOCK_ALARM;
  }
  else if (p_dofb->block_alm.unack == ALARM_UNACKED)
  {
    good_nc_substatus = SUB_UNACK_BLOCK_ALARM;
  }
  else
  {
    good_nc_substatus = 0;
  }


  /* Handle setpoint parameter -------------------------------------------- */
  /* ---------------------------------------------------------------------- */

  raw_sp_d.status = SQ_BAD;

  /* Apply control option 'SP Track retained target' ---------------------- */
  if (p_dofb->io_opts & IO_OPT_SP_TRACK_RET_TARGET)
  {
    if (p_dofb->mode_blk.target & MODE_RCAS)
    {
      Get_setpoint_d (&raw_sp_d,p_dofb->rcas_in_d);

      if ((raw_sp_d.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto END_OF_STORE_RETAINED_SP;
      }
    }

    if (p_dofb->mode_blk.target & MODE_CAS)
    {
      Get_setpoint_d (&raw_sp_d,p_dofb->cas_in_d);

      if ((raw_sp_d.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto END_OF_STORE_RETAINED_SP;
      }
    }

    END_OF_STORE_RETAINED_SP: ;
  }


  if (actual_mode == MODE_CAS)
  {
    if (p_dofb_data->fault.reason == FAULT_STATE_CLEAR)
    {
      Get_setpoint_d (&raw_sp_d,p_dofb->cas_in_d);
    }
    else
    {
      /* A fault condition was detected but FSTATE_TIME isn't expired yet */

      raw_sp_d = p_dofb->sp_d;
    }
  }

  else if (actual_mode == MODE_RCAS)
  {
    if (p_dofb_data->fault.reason == FAULT_STATE_CLEAR)
    {
      Get_setpoint_d (&raw_sp_d,p_dofb->rcas_in_d);
    }
    else
    {
      /* A fault condition was detected but FSTATE_TIME isn't expired yet */

      raw_sp_d = p_dofb->sp_d;
    }
  }

  else if (actual_mode == MODE_AUTO)
  {
    if ((old_mode == MODE_LO)||(old_mode == MODE_IMAN))
    {
      if ((raw_sp_d.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
      {
        goto AFTER_GET_SP_IN_AUTO;
      }
    }

    Get_setpoint_d (&raw_sp_d,p_dofb->sp_d);

    AFTER_GET_SP_IN_AUTO: ;
  }

  else if (actual_mode == MODE_MAN)
  {
    if (p_dofb->io_opts & IO_OPT_SP_TRACK_RET_TARGET)
    {
      if ((raw_sp_d.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
      {
        goto AFTER_GET_SP_IN_MAN;
      }
    }

    if (p_dofb->io_opts & IO_OPT_SP_PV_TRACK_IN_MAN)
    {
      Get_setpoint_d (&raw_sp_d,p_dofb->pv_d);

      if ((raw_sp_d.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto AFTER_GET_SP_IN_MAN;
      }
    }

    Get_setpoint_d (&raw_sp_d,p_dofb->sp_d);

    AFTER_GET_SP_IN_MAN: ;
  }

  else if (actual_mode == MODE_IMAN)
  {
    if ((raw_sp_d.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
    {
      goto AFTER_GET_SP_IN_IMAN;
    }

    Get_setpoint_d (&raw_sp_d,p_dofb->sp_d);

    AFTER_GET_SP_IN_IMAN: ;
  }

  else /* (actual_mode == MODE_LO) */
  {
    if (p_dofb_data->fault.active)
    {
      if (p_dofb->io_opts & IO_OPT_FSTATE_TO_VAL)
      {
        raw_sp_d.value  = p_dofb->fstate_val_d;
        raw_sp_d.status = SQ_GOOD_CAS;

        goto AFTER_GET_SP_IN_LO;
      }
    }

    if (p_dofb->io_opts & IO_OPT_SP_TRACK_RET_TARGET)
    {
      if ((raw_sp_d.status & QUALITY_MASK) == SQ_GOOD_CAS) /* Retained SP */
      {
        goto AFTER_GET_SP_IN_LO;
      }
    }

    if (p_dofb->io_opts & IO_OPT_SP_PV_TRACK_IN_LO)
    {
      Get_setpoint_d (&raw_sp_d,p_dofb->pv_d);

      if ((raw_sp_d.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        goto AFTER_GET_SP_IN_LO;
      }
    }

    Get_setpoint_d (&raw_sp_d,p_dofb->sp_d);

    AFTER_GET_SP_IN_LO:

    if (p_dofb_data->fault.active)
    {
      if ((raw_sp_d.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        raw_sp_d.status = (SQ_GOOD_CAS|SUB_FSA|LIMIT_CONST);
      }
    }
  }


  /* Update setpoint parameter  ------------------------------------------ */
  p_dofb->sp_d = raw_sp_d;


  /* Update OUT parameter ------------------------------------------------- */
  if (actual_mode == MODE_MAN)
  {
    raw_out_d = p_dofb->out_d;

    if ((raw_out_d.status & QUALITY_MASK) != SQ_BAD) raw_out_d.status |= LIMIT_CONST;
  }
  else
  {
    raw_out_d = raw_sp_d;

    /* Apply IO option 'Invert' ------------------------------------------- */

    if ((p_dofb->io_opts & IO_OPT_INVERT) != MN_ZERO)
    {
        if (p_dofb->channel == CH_POSITION_DISCRETE_POSITION)
        {
            if (raw_out_d.value >= MN_ONEHUNDRED)
            {
                raw_out_d.value = MN_ZERO;
            }
            else
            {
                raw_out_d.value = MN_ONEHUNDRED - raw_out_d.value;
            }
        }
        else
        {
            if (raw_out_d.value)
            {
                raw_out_d.value = MN_ZERO;
            }
            else
            {
                raw_out_d.value = MN_ONE;
            }
        }
    }

    /* Map good cascade status to good non-cascade status ----------------- */
    if ((raw_out_d.status & QUALITY_MASK) == SQ_GOOD_CAS)
    {
      raw_out_d.status &= LIMIT_MASK;
      raw_out_d.status |= (SQ_GOOD_NC|good_nc_substatus);
    }
  }


  /* Assign raw out to OUT parameter -------------------------------------- */
  p_dofb->out_d = raw_out_d;




  p_dofb->block_err &= ~BLK_ERR_OUTPUT_FAILURE; /* Reset set output failure
                                                       bit; setting may be forced
                                                       by the transducer block
                                                    */


  if (p_dofb->simulate_d.enable == SIMULATION_DISABLED)
  {
    /* Execute the transducer block --------------------------------------- */

    if ((raw_out_d.status & QUALITY_MASK) == SQ_GOOD_NC)
    {
      raw_out_d.status = SQ_GOOD_CAS;
    }

    if (p_dofb->mode_blk.actual == MODE_LO)
    {
      if (p_dofb_data->fault.active)
      {
        if (raw_out_d.status == SQ_GOOD_CAS)
        {
          raw_out_d.status = SQ_GOOD_CAS|SUB_FSA;
        }
      }
    }

    else if (p_dofb->mode_blk.target > MODE_AUTO)
    {
      USIGN8  not_invited = 0x00;

      if ((p_dofb->rcas_out_d.status & ~LIMIT_MASK) == (SQ_GOOD_CAS|SUB_IR))
      {
        init_rcas = TRUE;
      }
      else if ((p_dofb->rcas_out_d.status & ~LIMIT_MASK) != (SQ_GOOD_CAS|SUB_NON_SPECIFIC))
      {
        not_invited |= 0x01;
      }

      if ((p_dofb->bkcal_out_d.status & ~LIMIT_MASK) == (SQ_GOOD_CAS|SUB_IR))
      {
        if (init_rcas == FALSE)
        {
          init_cas = TRUE;
        }
      }
      else if ((p_dofb->bkcal_out_d.status & ~LIMIT_MASK) != (SQ_GOOD_CAS|SUB_NON_SPECIFIC))
      {
        not_invited |= 0x02;
      }

      if (init_rcas || init_cas || (not_invited == 0x03))
      {
        /* Traget mode is not reached ------------------------------------- */
        if (p_dofb_data->old_out_d.status == SQ_GOOD_CAS)
        {
          raw_out_d.status = SQ_UNCERTAIN|SUB_LUV;
          raw_out_d.value  = p_dofb_data->old_out_d.value;
        }
        else
        {
          if (raw_out_d.status == SQ_GOOD_CAS)
          {
            raw_out_d.status = SQ_UNCERTAIN|SUB_INITIAL_VALUE;
          }
        }
      }
    }

    if ((raw_out_d.status & QUALITY_MASK) == SQ_GOOD_CAS)
    {
      /* Store a usable out-value ----------------------------------------- */
      p_dofb_data->old_out_d.status = SQ_GOOD_CAS;
      p_dofb_data->old_out_d.value  = raw_out_d.value;
    }

    p_dofb_data->exec_tb.p_process_value = &raw_out_d;
    p_dofb_data->exec_tb.tb_error        = FALSE;

    (*p_dofb_data->linked_tb.a_of_execute_tb)(
            p_dofb_data->linked_tb.p_tb_instance,
            &p_dofb_data->exec_tb
         );

    if (p_resource->feature_sel & FEAT_OUTPUT_READBACK_SUPP)
    {
      /* Readback from transducer block ----------------------------------- */
      raw_rb_d = raw_out_d;

      if ((raw_out_d.status & QUALITY_MASK) == SQ_GOOD_CAS)
      {
        raw_rb_d.status = SQ_GOOD_NC;
      }
    }
    else
    {
      /* Readback from OUT parameter -------------------------------------- */
      raw_rb_d = p_dofb->out_d;
    }

    p_dofb->simulate_d.field_value  = raw_rb_d.value;
    p_dofb->simulate_d.field_status = raw_rb_d.status;
    p_dofb->simulate_d.value        = raw_rb_d.value;
    p_dofb->simulate_d.status       = raw_rb_d.status;

    p_dofb_data->last_tb_status     = raw_out_d.status;

    if (p_dofb_data->exec_tb.tb_error) p_dofb->block_err |= BLK_ERR_OUTPUT_FAILURE;
  }

  else /* Simulation is active -------------------------------------------- */
  {
    p_dofb->simulate_d.field_value  = raw_out_d.value;
    p_dofb->simulate_d.field_status = raw_out_d.status;
    raw_rb_d.value                  = p_dofb->simulate_d.value;
    raw_rb_d.status                 = p_dofb->simulate_d.status;

    p_dofb_data->last_tb_status     = SQ_GOOD_CAS;
                       /* Prevent block to go to IMAN when simulate enabled */
  }


  /* Assign raw readback to READBACK_D parameter -------------------------- */
  p_dofb->readback_d          = raw_rb_d;

  /* Apply IO option 'Invert' --------------------------------------------- */
  if ((p_dofb->io_opts & IO_OPT_INVERT) != MN_ZERO)
  {
        if (p_dofb->channel == CH_POSITION_DISCRETE_POSITION)
        {
            if (raw_rb_d.value >= MN_ONEHUNDRED)
            {
                raw_rb_d.value = MN_ZERO;
            }
            else
            {
                raw_rb_d.value = MN_ONEHUNDRED - raw_rb_d.value;
            }
        }
        else
        {
            if (raw_rb_d.value)
            {
                raw_rb_d.value = MN_ZERO;
            }
            else
            {
                raw_rb_d.value = MN_ONE;
            }
        }
  }


  /* Update PV_D ---------------------------------------------------------- */
  if ((raw_rb_d.status & QUALITY_MASK) == SQ_BAD)
  {
    USIGN8 substatus = raw_rb_d.status & SUB_STATUS_MASK;

    if ((substatus != SUB_DEVICE_FAILURE)&&(substatus != SUB_SENSOR_FAILURE))
    {
      raw_rb_d.status = SQ_BAD|SUB_NON_SPECIFIC;
    }
  }
  else if ((raw_rb_d.status & QUALITY_MASK) == SQ_GOOD_NC)
  {
    raw_rb_d.status &= ~SUB_STATUS_MASK;

    raw_rb_d.status |= good_nc_substatus;
  }

  p_dofb->pv_d = raw_rb_d;


  /* Check actuator input ------------------------------------------------- */
  if (p_resource->feature_sel & FEAT_OUTPUT_READBACK_SUPP)
  {
    if ((p_dofb->pv_d.status & QUALITY_MASK) == SQ_BAD)
    {
      if (!(p_dofb->block_err & BLK_ERR_INPUT_FAILURE))
      {
        p_dofb->block_err |= BLK_ERR_INPUT_FAILURE;
      }

     /* Apply status option 'Propagate Fault Forward' ------------------ */
      if (p_dofb->status_opts & STATUS_OPT_PROP_FLT_BACKW)
      {
        p_dofb->bkcal_out_d.status = p_dofb->pv_d.status;
        p_dofb->rcas_out_d.status  = p_dofb->pv_d.status;
      }
    }
    else /* (p_dofb->pv_d.status & QUALITY_MASK) != SQ_BAD) */
    {
      if (p_dofb->block_err & BLK_ERR_INPUT_FAILURE)
      {
        p_dofb->block_err &= ~BLK_ERR_INPUT_FAILURE;
      }
    }
  } /* (p_resource->feature_sel & FEAT_OUTPUT_READBACK_SUPP) */



  /* Check fault state conditions ----------------------------------------- */
  if (actual_mode != MODE_LO)
  {
    /* Fault state is not active------------------------------------------- */
    if (p_dofb->block_err & BLK_ERR_LOCAL_OVERRIDE)
    {
      /* Fault state was left during the current block execution ---------- */
      p_dofb->block_err &= ~BLK_ERR_LOCAL_OVERRIDE;

    }
  }
  else /* (actual_mode == MODE_LO) */
  {
    /* Fault state is active ---------------------------------------------- */

    if (!(p_dofb->block_err & BLK_ERR_LOCAL_OVERRIDE))
    {
      /* Fault state was entered during the current block execution ------- */

      p_dofb->block_err |= BLK_ERR_LOCAL_OVERRIDE;
    }

    if (p_dofb->status_opts & STATUS_OPT_PROP_FLT_BACKW)
    {
      p_dofb->bkcal_out_d.status = SQ_GOOD_CAS|SUB_FSA;
      p_dofb->rcas_out_d.status  = SQ_GOOD_CAS|SUB_FSA;
    }
    else
    {
      p_dofb->bkcal_out_d.status = SQ_GOOD_CAS|SUB_NI;
      p_dofb->rcas_out_d.status  = SQ_GOOD_CAS|SUB_NI;
    }
  } /* (actual_mode == MODE_LO) */



  /* Update BKCAL_OUT_D and RCAS_OUT_D ------------------------------------ */
  if (p_dofb->io_opts & IO_OPT_PV_FOR_BKCAL_OUT)
  {
    p_dofb->bkcal_out_d.value = p_dofb->pv_d.value;
    p_dofb->rcas_out_d.value  = p_dofb->pv_d.value;

    if ((p_dofb->pv_d.status & QUALITY_MASK) == SQ_BAD)
    {
      if (!(p_dofb->status_opts & STATUS_OPT_PROP_FLT_BACKW))
      {
        p_dofb->bkcal_out_d.status = SQ_BAD;
        p_dofb->rcas_out_d.status  = SQ_BAD;
      }
    }
  }
  else /* (!(p_dofb->io_opts & IO_OPT_PV_FOR_BKCAL_OUT)) */
  {
    p_dofb->bkcal_out_d.value   = raw_sp_d.value;
    p_dofb->rcas_out_d.value    = raw_sp_d.value;

    if ( init_rcas )
    {
      init_rcas = FALSE;

      if ((p_dofb->sp_d.status & QUALITY_MASK) == SQ_BAD)
      {
        p_dofb->bkcal_out_d.status = p_dofb->sp_d.status;
      }
    }
    else if ( init_cas )
    {
      init_cas = FALSE;

      if ((p_dofb->sp_d.status & QUALITY_MASK) == SQ_BAD)
      {
        p_dofb->rcas_out_d.status = p_dofb->sp_d.status;
      }
    }
    else /* No cascade initialization */
    {
      if ((p_dofb->sp_d.status & QUALITY_MASK) == SQ_BAD)
      {
        p_dofb->bkcal_out_d.status = p_dofb->sp_d.status;
        p_dofb->rcas_out_d.status  = p_dofb->sp_d.status;
      }
    }
  }

  if (actual_mode == MODE_IMAN)
  {
    if ((p_dofb->pv_d.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_GOOD_CAS|SUB_LO))
    {
      if (p_dofb->status_opts & STATUS_OPT_PROP_FLT_BACKW)
      {
        p_dofb->bkcal_out_d.status = (SQ_GOOD_CAS|SUB_LO);
      }
    }
  }

  /* Update block alarm --------------------------------------------------- */
  Update_block_alarm_DOFB (p_block_desc, p_dofb, p_dofb_data);

  if (enable_update_evt)
  {
    ffbl_enable_update_events
    (
      p_block_desc->block_id,p_block_desc->idx_upd_evt,
      &p_dofb->update_evt,p_dofb->st_rev
    );
  }

  return (p_dofb->blk_data.next_fb_to_execute);
}







FUNCTION GLOBAL VOID Background_DOFB
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
  T_DOFB_DATA *             p_dofb_data;
  T_FBS_WRITE_PARAM_LOCAL   write_loc;

FUNCTION_BODY

  p_dofb_data  = p_block_instance->p_block_data;

  _ASSERT (p_dofb_data != NULL);

  if (p_dofb_data->new_target_mode)
  {
    write_loc.rel_idx       = DO_PAR_MODE_BLK;
    write_loc.subindex      = 1;
    write_loc.length        = 1;
    write_loc.source        = &p_dofb_data->new_target_mode;
    write_loc.startup_sync  = FALSE;

    fbs_write_param_loc (p_block_instance,&write_loc);

    if ((p_dofb_data->new_target_mode == MODE_OS) ||
        (p_dofb_data->new_target_mode == MODE_MAN)  )
    {
      write_loc.rel_idx = DO_PAR_OUT_D;
    }
    else
    {
      write_loc.rel_idx = 0;
    }

    p_dofb_data->new_target_mode = 0;

    if (write_loc.rel_idx)
    {
      /* Store out parameter in non-volatile RAM ---------------------- */
      T_DOFB *  p_dofb = p_block_instance->p_block_desc->p_block;

      write_loc.subindex      = 0;
      write_loc.length        = 2;
      write_loc.source        = (USIGN8 *)&p_dofb->out_d;

      fbs_write_param_loc (p_block_instance,&write_loc);
    }
  }

  return;
}






FUNCTION LOCAL VOID Calc_block_mode_DOFB
     (
     T_DOFB *         p_dofb,
     T_DOFB_DATA *    p_dofb_data
     )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  mo.profile             = DO_PROFILE_NUMBER;
  mo.period_of_execution = p_dofb->blk_data.period_of_execution;
  mo.channel             = p_dofb->channel;
  mo.mode_was_written    = p_dofb_data->mode_was_written;
  mo.shed_opt            = p_dofb->shed_opt;
  mo.io_opts             = p_dofb->io_opts;
  mo.mode                = p_dofb->mode_blk;
  mo.fstate_time         = p_dofb->fstate_time;
  mo.cas_in_status       = p_dofb->cas_in_d.status;
  mo.rcas_in_status      = p_dofb->rcas_in_d.status;
  mo.cas_out_status      = p_dofb->bkcal_out_d.status;
  mo.rcas_out_status     = p_dofb->rcas_out_d.status;

  mo.shed                = p_dofb_data->shed;
  mo.fault               = p_dofb_data->fault;
  mo.rout_in_status      = p_dofb_data->last_tb_status; /* Misuse mo parameter */

  Calc_block_mode ();

  p_dofb_data->mode_was_written = FALSE;
  p_dofb->mode_blk              = mo.mode;
  p_dofb_data->fault            = mo.fault;
  p_dofb_data->shed             = mo.shed;
  p_dofb->bkcal_out_d.status    = mo.cas_out_status;
  p_dofb->rcas_out_d.status     = mo.rcas_out_status;

  if (!p_dofb_data->new_target_mode && mo.new_target_mode)
  {
    p_dofb_data->new_target_mode = mo.new_target_mode;
  }

  return;
}







FUNCTION LOCAL VOID Update_block_alarm_DOFB
  (
    IN const T_FBIF_BLOCK_DESCR * p_block_desc,
    INOUT T_DOFB *                p_dofb,
    INOUT T_DOFB_DATA *           p_dofb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16   alarm_bits;

FUNCTION_BODY

  if (p_dofb->block_err & BLK_ERR_OUT_OF_SERVICE)
  {
    alarm_bits = BLK_ERR_OUT_OF_SERVICE;
  }
  else
  {
    alarm_bits = p_dofb->block_err;

    /* Apply status option 'Propagate Fault Forward' ------------------------ */
    if (p_dofb->status_opts & STATUS_OPT_PROP_FLT_BACKW)
    {
      alarm_bits &= ~(BLK_ERR_INPUT_FAILURE|BLK_ERR_LOCAL_OVERRIDE);
    }
  }

  ffbl_update_block_alarm
  (
    p_block_desc->block_id,p_block_desc->idx_blk_alm,
    alarm_bits,&p_dofb_data->old_block_alarm_bits
  );

  return;
}






FUNCTION LOCAL VOID Reset_DOFB
  (
    IN T_DOFB_DATA *   p_dofb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Reset block specific data of a DO block.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  p_dofb_data->mode_was_written   = FALSE;
  p_dofb_data->new_target_mode    = 0;
  p_dofb_data->shed.active        = FALSE;
  p_dofb_data->shed.current_time  = 0;
  p_dofb_data->fault.reason       = FAULT_STATE_CLEAR;
  p_dofb_data->fault.active       = FALSE;
  p_dofb_data->fault.no_of_cycles = 0;
  p_dofb_data->last_tb_status     = SQ_BAD;
  p_dofb_data->old_out_d.status   = SQ_BAD;

  return;
}





FUNCTION LOCAL VOID Get_setpoint_d
  (
    OUT DISCRETE_S *  setpoint,
    IN  DISCRETE_S    source
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8  source_status;

FUNCTION_BODY

  source_status = source.status & QUALITY_MASK;

  if ((source_status == SQ_GOOD_CAS)||(source_status == SQ_GOOD_NC))
  {
    setpoint->status = (SQ_GOOD_CAS | (source.status & LIMIT_MASK));
  }

  else if (source_status == SQ_UNCERTAIN)
  {
    if (((source.status & SUB_STATUS_MASK) == SUB_SUBSTITUTE)   ||
        ((source.status & SUB_STATUS_MASK) == SUB_INITIAL_VALUE)  )
    {
      setpoint->status = (SQ_GOOD_CAS  | (source.status & LIMIT_MASK));
    }
    else
    {
      setpoint->status = SQ_BAD;
    }
  }

  else if (source_status == SQ_BAD)
  {
    setpoint->status = SQ_BAD;
  }

  setpoint->value  = source.value;
}





FUNCTION LOCAL VOID Check_config_DOFB
  (
    IN T_DOFB *                     p_dofb,
    IN T_DOFB_DATA *                p_dofb_data,
    IN const T_FBIF_BLOCK_DESCR *   p_block_desc
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN32    block_err_desc;

FUNCTION_BODY

  block_err_desc = BLK_ERR_NO_ERROR;

  if ((p_dofb->mode_blk.target & MODE_RCAS)&&(p_dofb->shed_opt == 0))
  {
    block_err_desc |= BLK_ERR_SHED_OPT_SET;
  }


  if (p_dofb->blk_data.period_of_execution == 0)
  {
    block_err_desc |= BLK_ERR_EXE_TIME;
  }


  /* Check if the DO block is linked to a transducer block ---------------- */
  if (p_dofb->channel == 0)
  {
    block_err_desc |= BLK_ERR_CHANNEL_INIT;
  }

  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_dofb_data->exec_tb.channel = fbs_link_transducer_block
                     (p_dofb->channel,p_block_desc,&p_dofb_data->linked_tb);

    if (p_dofb_data->exec_tb.channel != 0)
    {
      p_dofb_data->exec_tb.p_xd_scale = NULL;
    }
    else /* No transducer block for this channel */
    {
      block_err_desc |= BLK_ERR_CHANNEL_SETTING;
    }
  }


  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_dofb->block_err &= ~BLK_ERR_BLOCK_CONFIG_ERR;
  }
  else
  {
    p_dofb->block_err |= BLK_ERR_BLOCK_CONFIG_ERR;
  }

  p_dofb->block_err_desc_1 = block_err_desc;


  return;
}

static u16 mn_ConflictOtherDOChannels(const T_FBIF_BLOCK_DESCR *p_block_desc)
{
    u16 check;
    u16 thisBlockID = p_block_desc->block_id; //block ID of the calling block
    u16 otherBlockID;
    u16 thisChannel;
    T_DOFB *pThis_block = p_block_desc->p_block;

    //find which channel the block wants to set
    thisChannel = pThis_block->channel;
    //find other block ID
    otherBlockID = (thisBlockID == ID_DOFB_1)? ID_DOFB_2: ID_DOFB_1;


    //get other DO block instance by ID
    const T_FBIF_BLOCK_INSTANCE *pDOFB_other_inst = fbs_get_block_inst(otherBlockID);
    //get pointers to other DO block
    const T_DOFB *pOther_block = pDOFB_other_inst->p_block_desc->p_block;

    //check if the other block has the same channel and not in OOS
    if ( (pOther_block->mode_blk.actual != MODE_OS) && (pOther_block->channel == thisChannel) )
    {
        check = TRUE;
    }
    else
    {
        check = FALSE;
    }

    return check;
}// ----- end of mn_CheckBlockChannels() -----

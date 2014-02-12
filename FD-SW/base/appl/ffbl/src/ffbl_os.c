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

FILE_NAME          ffbl_os.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKOS)

INCLUDES
  #include <math.h>
  #include <string.h>

  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbap.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_os.h"
  #include "ffbl_res.h"

LOCAL_DEFINES

  #define ALERT_KEY_MIN        0x01
  #define ALERT_KEY_MAX        0xFF

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID   Check_config_OSFB      (T_OSFB *);
  FUNCTION LOCAL USIGN8 Calc_block_mode_OSFB   (T_OSFB *, T_OSFB_DATA *);
  FUNCTION LOCAL VOID   init_osfb_data         (T_OSFB_DATA *);
  FUNCTION LOCAL VOID   calc_osfb_data         (T_OSFB *, T_OSFB_DATA *);
  FUNCTION LOCAL VOID   calc_downstream_status (USIGN8, USIGN8 *);
  FUNCTION LOCAL USIGN8 init_cascade           (USIGN8, USIGN8 *, USIGN8, USIGN8);
  FUNCTION LOCAL VOID   calc_setpoint          (T_OSFB *, T_OSFB_DATA *, USIGN8);
  FUNCTION LOCAL VOID   calc_working_setpoints (T_OSFB *, T_OSFB_DATA *);
  FUNCTION LOCAL VOID   calc_out_values        (T_OSFB *, T_OSFB_DATA *);
  FUNCTION LOCAL VOID   update_bkcal_out       (T_OSFB *, T_OSFB_DATA *);

IMPORT_DATA

EXPORT_DATA

LOCAL_DATA





FUNCTION GLOBAL VOID Start_OSFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_OSFB *        p_osfb;
  T_OSFB_DATA *   p_osfb_data;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_OSFB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_osfb        = p_block_instance->p_block_desc->p_block;
  p_osfb_data   = p_block_instance->p_block_data;

  p_osfb_data->old_block_alarm_bits = 0;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_osfb->update_evt);

  /* Store out param in the internal block-specific data structure -------- */
  p_osfb_data->out_1 = p_osfb->out_1;
  p_osfb_data->out_2 = p_osfb->out_2;

  /* Set out status to BAD out-of-service --------------------------------- */
  p_osfb->out_1.status = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_osfb->out_2.status = SQ_BAD|SUB_OUT_OF_SERVICE;

  /* Check function block configuration ----------------------------------- */
  Check_config_OSFB (p_osfb);

  /* Initialize OS data, NOT OUT_1/OUT_2 ---------------------------------- */
  init_osfb_data (p_osfb_data);

  /* Calculate OS data ---------------------------------------------------- */
  if (!(p_osfb->block_err & BLK_ERR_BLOCK_CONFIG_ERR))
  {
    calc_osfb_data (p_osfb,p_osfb_data);
  }

  return;
}





FUNCTION GLOBAL VOID Stop_OSFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_OSFB_DATA *               p_osfb_data;
  T_OSFB *                    p_osfb;

FUNCTION_BODY

  p_block_desc = p_block_instance->p_block_desc;
  p_osfb_data  = p_block_instance->p_block_data;
  p_osfb       = p_block_desc->p_block;

  if (p_osfb->mode_blk.actual == MODE_OS) return;

  p_osfb->mode_blk.actual = MODE_OS;

  init_osfb_data (p_osfb_data);

  /* Set block error ------------------------------------------------------ */
  p_osfb->block_err |= BLK_ERR_OUT_OF_SERVICE;

  /* Store out param in the internal block-specific data structure -------- */
  p_osfb_data->out_1 = p_osfb->out_1;
  p_osfb_data->out_2 = p_osfb->out_2;

  /* Set status of output parameters -------------------------------------- */
  p_osfb->out_1.status      = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_osfb->out_2.status      = SQ_BAD|SUB_OUT_OF_SERVICE;
  p_osfb->bkcal_out.status  = SQ_BAD|SUB_OUT_OF_SERVICE;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_osfb->update_evt);

  /* Update block alarm --------------------------------------------------- */
  ffbl_update_block_alarm
  (
    p_block_desc->block_id,p_block_desc->idx_blk_alm,
    BLK_ERR_OUT_OF_SERVICE,&p_osfb_data->old_block_alarm_bits
  );

  return;
}





FUNCTION GLOBAL USIGN16 Read_handler_OSFB
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
  p_block_instance  = p_block_instance; /* avoid compiler warning */
  p_read            = p_read;           /* avoid compiler warning */

  return (E_OK);
}






FUNCTION GLOBAL USIGN16 Write_handler_OSFB
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
  T_OSFB_DATA *               p_osfb_data;
  T_OSFB *                    p_osfb;
  USIGN8                      subindex;
  USIGN8 *                    source;
  USIGN8                      result;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_osfb_data   = p_block_instance->p_block_data;
  p_osfb        = p_block_desc->p_block;

  subindex      = p_write->subindex;
  source        = p_write->source;

  result        = E_OK;

  switch (p_write->rel_idx)
  {
    case OS_PAR_ALERT_KEY:
    {
      if (*source == 0) result = E_FB_PARA_CHECK;

      break;
    }

    case OS_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (
                 &p_osfb->mode_blk,
                 subindex,
                 source,
                 MODE_OS_VALID
               );
      break;
    }

    case OS_PAR_SP:
    {
      if (MODE_LOWER_THAN_AUTO (p_osfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      if ((subindex != 0) && (subindex != 2))
      {
        return (E_FB_DATA_NO_WRITE);
      }

      p_osfb->sp.status = SQ_GOOD_CAS;

      break;
    }

    case OS_PAR_OUT_1:
    case OS_PAR_OUT_2:
    {
      if (MODE_LOWER_THAN_MAN (p_osfb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      if ((subindex != 0) && (subindex != 2))
      {
        return (E_FB_DATA_NO_WRITE);
      }

      if (p_write->rel_idx == OS_PAR_OUT_1)
      {
        p_osfb->out_1.status = (SQ_GOOD_CAS|LIMIT_CONST);
      }
      else
      {
        p_osfb->out_2.status = (SQ_GOOD_CAS|LIMIT_CONST);
      }

      break;
    }

    case OS_PAR_GRANT_DENY:
    {
      ffbl_bas_write_grant_deny (subindex,source,&p_osfb->grant_deny);

      break;
    }

    case OS_PAR_STATUS_OPTS:
    {
      if (MODE_NOT_OS (p_osfb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      WPHB_(source) &= CWHB_(OS_STATUS_OPTS);  /* Delete all undefined bits */
      WPLB_(source) &= CWLB_(OS_STATUS_OPTS);  /* Delete all undefined bits */

      break;
    }

    case OS_PAR_LOCKVAL:
    {
      if ((*source != OS_LOCKVAL_NO_LOCK)&&(*source != OS_LOCKVAL_LOCK))
          return (E_FB_PARA_CHECK);

      if (p_osfb->lockval != *source)
      {
        p_osfb_data->end_value_reached = FB_FALSE;
      }

      break;
    }

    case OS_PAR_BAL_TIME:
    case OS_PAR_HYSTVAL:
    {
      FLOAT  fval;

      memcpy(&fval, source, sizeof(fval));

      if (fval < 0.0f)
          return (E_FB_PARA_CHECK);

      break;
    }

    case OS_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_osfb->update_evt.unack, source, subindex);

      break;
    }

    case OS_PAR_BLOCK_ALM:
    {
      result = ffbl_check_block_alarm_unack_flag
                 (p_block_instance, &p_osfb->block_alm.unack, source, subindex);
      break;
    }
  }

  if (result == E_OK)
  {
    result = fbs_write_param (p_block_instance,p_write);
  }

  if (result == E_OK)
  {
    BOOL  check_config = FB_FALSE;
    BOOL  calc_data    = FB_FALSE;

    switch (p_write->rel_idx)
    {
      case OS_PAR_MODE_BLK:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        write_loc.rel_idx = 0;

        if (p_osfb->mode_blk.target & MODE_OS)
        {
          p_osfb->out_1.status = SQ_BAD|SUB_OUT_OF_SERVICE;
          p_osfb->out_2.status = SQ_BAD|SUB_OUT_OF_SERVICE;

          write_loc.rel_idx = !0;
        }

        else if (p_osfb->mode_blk.target & MODE_MAN)
        {
          write_loc.rel_idx = !0;
        }

        if (write_loc.rel_idx)
        {
          /* Store out parameter in non-volatile RAM ---------------------- */

          /* OUT_1 first -------------------------------------------------- */

          write_loc.rel_idx       = OS_PAR_OUT_1;
          write_loc.subindex      = 0;
          write_loc.length        = sizeof(p_osfb->out_1);
          write_loc.source        = (USIGN8 *) &p_osfb->out_1;
          write_loc.startup_sync  = FB_FALSE;

          fbs_write_param_loc (p_block_instance,&write_loc);

          /* OUT_2 second ------------------------------------------------- */

          write_loc.rel_idx       = OS_PAR_OUT_2;
          write_loc.subindex      = 0;
          write_loc.length        = sizeof(p_osfb->out_2);
          write_loc.source        = (USIGN8 *) &p_osfb->out_2;
          write_loc.startup_sync  = FB_FALSE;

          fbs_write_param_loc (p_block_instance,&write_loc);
        }

        if (p_osfb->mode_blk.actual == MODE_CAS)
        {
          p_osfb_data->mode_was_written  = FB_TRUE;
        }

        break;
      }

      case OS_PAR_OUT_1:
      case OS_PAR_OUT_2:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        if(p_write->rel_idx == OS_PAR_OUT_1)
        {
          p_osfb->out_1.status = SQ_GOOD_NC;

          write_loc.rel_idx = OS_PAR_OUT_1;
          write_loc.length  = sizeof(p_osfb->out_1);
          write_loc.source  = (USIGN8 *)&p_osfb->out_1;
        }
        else
        {
          p_osfb->out_2.status = SQ_GOOD_NC;

          write_loc.rel_idx = OS_PAR_OUT_2;
          write_loc.length  = sizeof(p_osfb->out_2);
          write_loc.source  = (USIGN8 *)&p_osfb->out_2;
        }

        /* Store GOOD out status in non-volatile RAM ---------------------- */
        write_loc.subindex     = 0;
        write_loc.startup_sync = FB_FALSE;

        fbs_write_param_loc (p_block_instance,&write_loc);

        /* Store out param in the internal block-specific data structure -- */
        if(p_write->rel_idx == OS_PAR_OUT_1)
          p_osfb_data->out_1 = p_osfb->out_1;
        else
          p_osfb_data->out_2 = p_osfb->out_2;

        /* Adapt out status in accordance to the actual mode -------------- */
        if (p_osfb->mode_blk.actual == MODE_OS)
        {
          p_osfb->out_1.status = SQ_BAD|SUB_OUT_OF_SERVICE;
          p_osfb->out_2.status = SQ_BAD|SUB_OUT_OF_SERVICE;
        }
        else
        {
          p_osfb->out_1.status |= LIMIT_CONST;
          p_osfb->out_2.status |= LIMIT_CONST;
        }

        break;
      }

      case OS_PAR_BLK_DATA:
      case OS_PAR_IN_ARRAY:
      case OS_PAR_OUT_ARRAY:
      case OS_PAR_LOCKVAL:
      {
        calc_data    = FB_TRUE;
        check_config = FB_TRUE;
        break;
      }

      case OS_PAR_BAL_TIME:
      case OS_PAR_HYSTVAL:
      {
        calc_data    = FB_TRUE;
        break;
      }

      default:
        break;
    }

    if (check_config)
    {
      Check_config_OSFB (p_osfb);
    }

    if (calc_data && !(p_osfb->block_err & BLK_ERR_BLOCK_CONFIG_ERR))
    {
      calc_osfb_data (p_osfb,p_osfb_data);
    }
  }

  return (result);
}







FUNCTION GLOBAL USIGN16 Execute_OSFB
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
  T_OSFB_DATA *                p_osfb_data;
  T_OSFB *                     p_osfb;
  USIGN8                       old_mode;
  USIGN8                       actual_mode;
  BOOL                         enable_update_evt;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_osfb_data   = p_block_instance->p_block_data;
  p_osfb        = p_block_desc->p_block;

  /* Update block mode -------------------------------------------------- */
  old_mode    = p_osfb->mode_blk.actual;
  actual_mode = Calc_block_mode_OSFB (p_osfb,p_osfb_data);

  if (actual_mode == MODE_OS)
  {
    if (old_mode != MODE_OS)
    {
      /* Mode OS is entered now ------------------------------------------- */
      Stop_OSFB (p_block_instance);
    }
    else
    {
      /* Update block alarm ----------------------------------------------- */
      ffbl_update_block_alarm
      (
        p_block_desc->block_id,p_block_desc->idx_blk_alm,
        BLK_ERR_OUT_OF_SERVICE,&p_osfb_data->old_block_alarm_bits
      );
    }

    return (p_osfb->blk_data.next_fb_to_execute);
  }

  p_osfb->mode_blk.actual = actual_mode;

  if (old_mode == MODE_OS) /* Mode OS is left now ------------------------- */
  {
    /* Reset block error -------------------------------------------------- */
    p_osfb->block_err &= ~BLK_ERR_OUT_OF_SERVICE;
    enable_update_evt  = FB_TRUE;

    /* Restore out parameter ---------------------------------------------- */
    p_osfb->out_1 = p_osfb_data->out_1;
    p_osfb->out_2 = p_osfb_data->out_2;

    if ((p_osfb->out_1.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_BAD|SUB_OUT_OF_SERVICE))
    {
      p_osfb->out_1.status = SQ_BAD;
    }

    if ((p_osfb->out_2.status & (QUALITY_MASK|SUB_STATUS_MASK)) == (SQ_BAD|SUB_OUT_OF_SERVICE))
    {
      p_osfb->out_2.status = SQ_BAD;
    }
  }
  else
  {
    enable_update_evt = FB_FALSE;
  }

  /* Calculate setpoint(s) ------------------------------------------------ */
  if (actual_mode == MODE_CAS)
  {
    p_osfb->sp.value  = p_osfb->cas_in.value;
    p_osfb->sp.status = SQ_GOOD_CAS;
  }

  calc_setpoint (p_osfb,p_osfb_data,old_mode);

  calc_working_setpoints (p_osfb,p_osfb_data);

  /* Handle OUT_1 and OUT_2 ----------------------------------------------- */
  calc_out_values (p_osfb,p_osfb_data);

  if ((p_osfb->cas_in.status & QUALITY_MASK) == SQ_BAD)
  {
    /* Apply status option 'IFS if BAD CAS_IN' ---------------------------- */
    if (  (p_osfb->status_opts & STATUS_OPT_IFS_IF_BAD_CAS_IN)&&
          (p_osfb->mode_blk.target & MODE_CAS)                   )
    {
      p_osfb->out_1.status = SQ_GOOD_CAS|SUB_IFS;
      p_osfb->out_2.status = SQ_GOOD_CAS|SUB_IFS;
    }

    /* Apply status option 'Target to next permitted mode of BAD CAS_IN' -- */
    if (  (p_osfb->status_opts & STATUS_OPT_TARG_NEXT_IF_BAD_CAS_IN)&&
          (p_osfb->mode_blk.target & MODE_CAS)                     )
    {
      p_osfb->mode_blk.target = MODE_AUTO;
    }
  }

  /* Update BKCAL_OUT ----------------------------------------------------- */
  update_bkcal_out (p_osfb,p_osfb_data);


  /* endlbl: */       /* Label endlbl is currently not used. Avoid warning! */


  /* Update block alarm --------------------------------------------------- */
  ffbl_update_block_alarm
  (
    p_block_desc->block_id,p_block_desc->idx_blk_alm,
    p_osfb->block_err,&p_osfb_data->old_block_alarm_bits
  );

  if (enable_update_evt)
  {
    ffbl_enable_update_events
    (
      p_block_desc->block_id,
      p_block_desc->idx_upd_evt,
      &p_osfb->update_evt,
      p_osfb->st_rev
    );
  }

  return(p_osfb->blk_data.next_fb_to_execute);
}






FUNCTION GLOBAL VOID Background_OSFB
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
  T_OSFB                  * p_osfb;
  T_OSFB_DATA             * p_osfb_data;
  T_FBS_WRITE_PARAM_LOCAL   write_loc;

FUNCTION_BODY

  p_osfb = p_block_instance->p_block_desc->p_block;
  p_osfb_data  = p_block_instance->p_block_data;

  _ASSERT (p_osfb != NULL);
  _ASSERT (p_osfb_data != NULL);

  if (p_osfb_data->new_target_mode)
  {
    write_loc.rel_idx       = OS_PAR_MODE_BLK;
    write_loc.subindex      = 1;
    write_loc.length        = sizeof(p_osfb->mode_blk.target);
    write_loc.source        = &p_osfb_data->new_target_mode;
    write_loc.startup_sync  = FB_FALSE;

    fbs_write_param_loc (p_block_instance,&write_loc);

    if ((p_osfb_data->new_target_mode == MODE_OS) ||
        (p_osfb_data->new_target_mode == MODE_MAN)  )
    {
      write_loc.rel_idx = OS_PAR_OUT_1;
    }
    else
    {
      write_loc.rel_idx = 0;
    }

    p_osfb_data->new_target_mode = 0;

    if (write_loc.rel_idx)
    {
      /* Store OUT_1 parameter in non-volatile RAM ---------------------- */
      write_loc.subindex = 0;
      write_loc.length   = sizeof(p_osfb->out_1);
      write_loc.source   = (USIGN8 *)&p_osfb->out_1;

      fbs_write_param_loc (p_block_instance,&write_loc);

      /* Store OUT_2 parameter in non-volatile RAM ---------------------- */
      write_loc.rel_idx  = OS_PAR_OUT_2;
      write_loc.subindex = 0;
      write_loc.length   = sizeof(p_osfb->out_2);
      write_loc.source   = (USIGN8 *)&p_osfb->out_2;

      fbs_write_param_loc (p_block_instance,&write_loc);
    }
  }

  return;
}






FUNCTION LOCAL VOID Check_config_OSFB
  (
    IN T_OSFB * p_osfb
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN32     block_err_desc;

FUNCTION_BODY

  block_err_desc = BLK_ERR_NO_ERROR;

  if (p_osfb->blk_data.period_of_execution == 0)
  {
    block_err_desc |= BLK_ERR_EXE_TIME;
  }

  /* Check IN ARRAY ------------------------------------------------------- */
  if ((p_osfb->in_array[2] <  p_osfb->in_array[0]) ||
      (p_osfb->in_array[1] <= p_osfb->in_array[0]) ||
      (p_osfb->in_array[3] <= p_osfb->in_array[2])    )
  {
    block_err_desc |= BLK_ERR_IN_ARR_INCONSIST;
  }

  if (p_osfb->lockval == 0)
  {
    block_err_desc |= BLK_ERR_LOCKVAL_INIT;
  }

  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_osfb->block_err &= ~BLK_ERR_BLOCK_CONFIG_ERR;
  }
  else
  {
    p_osfb->block_err |= BLK_ERR_BLOCK_CONFIG_ERR;
  }

  p_osfb->block_err_desc_1 = block_err_desc;

  return;
}






FUNCTION LOCAL VOID init_osfb_data
  (
    IN T_OSFB_DATA *  p_osfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  p_osfb_data->end_value_reached = FB_FALSE;
  p_osfb_data->balance_1         = FB_FALSE;
  p_osfb_data->balance_2         = FB_FALSE;
  p_osfb_data->downstream_1      = CASC_NOT_ESTABLISHED;
  p_osfb_data->downstream_2      = CASC_NOT_ESTABLISHED;
  p_osfb_data->new_target_mode   = 0;
  p_osfb_data->mode_was_written  = FB_FALSE;

  return;
}







FUNCTION LOCAL VOID calc_osfb_data
  (
    IN T_OSFB *       p_osfb,
    IN T_OSFB_DATA *  p_osfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  p_osfb_data->m1 = (p_osfb->out_array[1]-p_osfb->out_array[0]) /
                    (p_osfb->in_array [1]-p_osfb->in_array [0])   ;

  if (fabs(p_osfb_data->m1) < 0.000001f) p_osfb_data->m1 = 0.0f;

  p_osfb_data->m2 = (p_osfb->out_array[3]-p_osfb->out_array[2]) /
                    (p_osfb->in_array [3]-p_osfb->in_array [2])   ;

  if (fabs(p_osfb_data->m2) < 0.000001f) p_osfb_data->m2 = 0.0f;

  if (p_osfb->bal_time > 0.0f)
  {
    p_osfb_data->balance_step = p_osfb->blk_data.period_of_execution / (32000 * p_osfb->bal_time);
  }
  else
  {
    p_osfb_data->balance_step = 1.0f;
  }

  p_osfb_data->hyst_limit = p_osfb->in_array[1] - p_osfb->hystval;

  return;
}






FUNCTION LOCAL USIGN8 Calc_block_mode_OSFB
  (
    IN T_OSFB *       p_osfb,
    IN T_OSFB_DATA *  p_osfb_data
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


  if ((p_osfb->mode_blk.target == MODE_OS)          ||
      (p_osfb->block_err & BLK_ERR_BLOCK_CONFIG_ERR)  )
  {
    return (MODE_OS);
  }

  if (p_osfb->mode_blk.actual == MODE_OS)
  {
    return (MODE_MAN);
  }

  calc_downstream_status (p_osfb->bkcal_in_1.status,&p_osfb_data->downstream_1);
  calc_downstream_status (p_osfb->bkcal_in_2.status,&p_osfb_data->downstream_2);


  if (p_osfb_data->mode_was_written)
  {
    /* i0403_A2 / step 7 -------------------------------------------------- */
    p_osfb->bkcal_out.status  = SQ_GOOD_CAS|SUB_NI;

    p_osfb_data->mode_was_written = FB_FALSE;

    return (MODE_AUTO);
  }


  if (p_osfb->mode_blk.target & MODE_CAS)
  {
    if (p_osfb->mode_blk.actual == MODE_CAS)
    {
      if ((p_osfb->cas_in.status & QUALITY_MASK) != SQ_BAD)
      {
        if ((p_osfb_data->downstream_1 == CASC_ESTABLISHED) ||
            (p_osfb_data->downstream_2 == CASC_ESTABLISHED)    )
        {
          p_osfb->bkcal_out.status = SQ_GOOD_CAS;

          return (MODE_CAS);
        }
      }
      else
      {
        p_osfb->bkcal_out.status  = SQ_GOOD_CAS|SUB_NI;

        /* Check if "STATUS_OPT_TARG_NEXT_IF_BAD_CAS_IN" is selected */
        if (p_osfb->status_opts & STATUS_OPT_TARG_NEXT_IF_BAD_CAS_IN)
        {
          if (p_osfb->mode_blk.permit & MODE_AUTO)
          {
            p_osfb->mode_blk.target = MODE_AUTO;
          }
          else if (p_osfb->mode_blk.permit & MODE_MAN)
          {
            p_osfb->mode_blk.target = MODE_MAN;
          }
          else
          {
            p_osfb->mode_blk.target = MODE_OS;
          }

          if(!p_osfb_data->new_target_mode)
          {
            p_osfb_data->new_target_mode = p_osfb->mode_blk.target;
          }

          return(p_osfb->mode_blk.target);
        }
        else
        {
          return (MODE_AUTO);
        }
      }
    }
    else /* p_osfb->mode_blk.actual != MODE_CAS */
    {
      if (init_cascade (p_osfb->cas_in.status,&p_osfb->bkcal_out.status,
                        p_osfb_data->downstream_1,p_osfb_data->downstream_2))
      {
        return (MODE_CAS);
      }
    }
  }

  if (!(p_osfb->mode_blk.target & MODE_CAS))
  {
    p_osfb->bkcal_out.status = SQ_GOOD_CAS|SUB_NI;
  }

  if ((p_osfb_data->downstream_1 < CASC_INIT_WITHOUT_ACK) &&
      (p_osfb_data->downstream_2 < CASC_INIT_WITHOUT_ACK)    )
  {
    return (MODE_IMAN);
  }

  return (MODE_AUTO);
}







FUNCTION LOCAL VOID calc_downstream_status
  (
    IN USIGN8     bkcal_in_status,
    IN USIGN8 *   downstream
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if ((bkcal_in_status & QUALITY_MASK) == SQ_GOOD_CAS)
  {
    if ((bkcal_in_status & SUB_STATUS_MASK) == SUB_IR)
    {
      *downstream = CASC_INIT_WITH_ACK;
    }
    else if ((bkcal_in_status & SUB_STATUS_MASK) == SUB_NON_SPECIFIC)
    {
      if (*downstream != CASC_ESTABLISHED)
      {
        *downstream = CASC_INIT_WITHOUT_ACK;
      }
    }
    else
    {
      *downstream = CASC_NOT_ESTABLISHED;
    }
  }
  else
  {
    *downstream = CASC_NOT_ESTABLISHED;
  }
}






FUNCTION LOCAL USIGN8 init_cascade
  (
    IN    USIGN8     cas_in_status,
    INOUT USIGN8 *   cas_out_status,
    IN    USIGN8     downstream_1,
    IN    USIGN8     downstream_2
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if ((cas_in_status & QUALITY_MASK) == SQ_GOOD_CAS)
  {
    if ((cas_in_status & SUB_STATUS_MASK) == SUB_NON_SPECIFIC)
    {
      if ((downstream_1 > CASC_INIT_WITH_ACK)||(downstream_1 > CASC_INIT_WITH_ACK))
      {
        *cas_out_status = SQ_GOOD_CAS|SUB_IR;

        return (FB_FALSE);
      }
      else if ((downstream_1 & CASC_INIT) && (downstream_2 != CASC_ESTABLISHED))
      {
        *cas_out_status = SQ_GOOD_CAS|SUB_IR;

        return (FB_FALSE);
      }
      else if ((downstream_2 & CASC_INIT) && (downstream_1 == CASC_NOT_ESTABLISHED))
      {
        *cas_out_status = SQ_GOOD_CAS|SUB_IR;

        return (FB_FALSE);
      }
    }
    else if ((cas_in_status & SUB_STATUS_MASK) == SUB_IA)
    {
      if ((*cas_out_status & SUB_STATUS_MASK) == SUB_IR)
      {
        *cas_out_status = SQ_GOOD_CAS;

        return (FB_TRUE);
      }
    }
  }

  else if ((cas_in_status & QUALITY_MASK) == SQ_GOOD_NC)
  {
    if ((downstream_1 != CASC_NOT_ESTABLISHED)||(downstream_2 != CASC_NOT_ESTABLISHED))
    {
      *cas_out_status = SQ_GOOD_CAS;

      return (FB_TRUE);
    }
  }

  else if ((cas_in_status & QUALITY_MASK) == SQ_UNCERTAIN)
  {
    if ((downstream_1 != CASC_NOT_ESTABLISHED)||(downstream_2 != CASC_NOT_ESTABLISHED))
    {
      if (((cas_in_status & SUB_STATUS_MASK) == SUB_SUBSTITUTE)   ||
          ((cas_in_status & SUB_STATUS_MASK) == SUB_INITIAL_VALUE)  )
      {
        *cas_out_status = SQ_GOOD_CAS;

        return (FB_TRUE);
      }
    }
  }

  *cas_out_status = SQ_GOOD_CAS|SUB_NI;

  return (FB_FALSE);
}






FUNCTION LOCAL VOID calc_setpoint
  (
    IN T_OSFB *       p_osfb,
    IN T_OSFB_DATA *  p_osfb_data,
    IN USIGN8         old_mode
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  FLOAT m,y,x1,y1;
  BOOL  init_sp_with_bkcal_in_1;

FUNCTION_BODY

  init_sp_with_bkcal_in_1 = FALSE;

  if (p_osfb_data->downstream_1 & CASC_INIT)
  {
    /* Calculate start setpoint 1 ----------------------------------------- */
    m   = p_osfb_data->m1;
    y   = p_osfb->bkcal_in_1.value;
    y1  = p_osfb->out_array[0];
    x1  = p_osfb->in_array [0];

    if (m != 0.0f)
          p_osfb_data->start_sp_1 = (y - y1)/m + x1;
    else  p_osfb_data->start_sp_1 = x1;

    if ((old_mode == MODE_MAN)||(old_mode == MODE_IMAN))
    {
      if ( (p_osfb->mode_blk.target & MODE_CAS) ||
           (p_osfb->sp.status   != SQ_GOOD_CAS)    )
      {
        /* Initialize SP with BKCAL_IN_1 value ---------------------------- */
        p_osfb->sp.value  = p_osfb_data->start_sp_1;
        p_osfb->sp.status = SQ_GOOD_CAS;

        init_sp_with_bkcal_in_1 = TRUE;
      }
    }

    if (p_osfb_data->balance_step < 1.0f)
    {
      p_osfb_data->balance_1        = FB_TRUE;
      p_osfb_data->balance_factor_1 = 0.0f;
    }
  }

  if (p_osfb_data->downstream_2  & CASC_INIT)
  {
    /* Calculate start setpoint 2 ----------------------------------------- */
    m   = p_osfb_data->m2;
    y   = p_osfb->bkcal_in_2.value;
    y1  = p_osfb->out_array[2];
    x1  = p_osfb->in_array [2];

    if (m != 0.0f)
          p_osfb_data->start_sp_2 = (y - y1)/m + x1;
    else  p_osfb_data->start_sp_2 = x1;

    if ((old_mode == MODE_MAN)||(old_mode == MODE_IMAN))
    {
      if (p_osfb->mode_blk.target & MODE_CAS)
      {
        if (init_sp_with_bkcal_in_1)
        {
          /* Both downstreams request initialization ---------------------- */
          p_osfb->sp.value  = (p_osfb_data->start_sp_1 + p_osfb_data->start_sp_2) / 2.0f;
        }
        else /* init_sp_with_bkcal_in_1 == FALSE */
        {
          /* Initialize SP with BKCAL_IN_2 value -------------------------- */
          p_osfb->sp.value  = p_osfb_data->start_sp_2;
        }

        p_osfb->sp.status = SQ_GOOD_CAS;
      }
      else /* target == MODE_AUTO */
      {
        if (p_osfb->sp.status != SQ_GOOD_CAS)
        {
          /* Initialize SP with BKCAL_IN_2 value ---------------------------- */
          p_osfb->sp.value  = p_osfb_data->start_sp_2;
          p_osfb->sp.status = SQ_GOOD_CAS;
        }
      }
    }

    if (p_osfb_data->balance_step < 1.0f)
    {
      p_osfb_data->balance_2        = FB_TRUE;
      p_osfb_data->balance_factor_2 = 0.0f;
    }
  }

  return;
}






FUNCTION LOCAL VOID calc_working_setpoints
  (
    IN T_OSFB *       p_osfb,
    IN T_OSFB_DATA *  p_osfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  FLOAT working_sp, target_sp;

FUNCTION_BODY

  if (p_osfb_data->balance_1)
  {
    if (p_osfb->sp.value < p_osfb->in_array[0])
    {
      target_sp = p_osfb->in_array[0];
    }
    else if (p_osfb->sp.value > p_osfb->in_array[1])
    {
      target_sp = p_osfb->in_array[1];
    }
    else
    {
      target_sp = p_osfb->sp.value;
    }

    working_sp = (1 - p_osfb_data->balance_factor_1) * p_osfb_data->start_sp_1 +
                    p_osfb_data->balance_factor_1    * target_sp;

    p_osfb_data->balance_factor_1 += p_osfb_data->balance_step;

    if (p_osfb_data->balance_factor_1 >= 1.0f)
    {
      p_osfb_data->balance_1 = FB_FALSE;
    }
  }
  else
  {
    working_sp = p_osfb->sp.value;
  }

  p_osfb_data->working_sp_1 = working_sp;


  if (p_osfb_data->balance_2)
  {
    if (p_osfb->sp.value < p_osfb->in_array[2])
    {
      target_sp = p_osfb->in_array[2];
    }
    else if (p_osfb->sp.value > p_osfb->in_array[3])
    {
      target_sp = p_osfb->in_array[3];
    }
    else
    {
      target_sp = p_osfb->sp.value;
    }

    working_sp = (1 - p_osfb_data->balance_factor_2) * p_osfb_data->start_sp_2 +
                    p_osfb_data->balance_factor_2    * target_sp;

    p_osfb_data->balance_factor_2 += p_osfb_data->balance_step;

    if (p_osfb_data->balance_factor_2 >= 1.0f)
    {
      p_osfb_data->balance_2 = FB_FALSE;
    }
  }
  else
  {
    working_sp = p_osfb->sp.value;
  }

  p_osfb_data->working_sp_2 = working_sp;

  return;
}






FUNCTION LOCAL VOID calc_out_values
  (
    IN T_OSFB *       p_osfb,
    IN T_OSFB_DATA *  p_osfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Calculate output value from input value.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  FLOAT   setpoint,out;

FUNCTION_BODY

  /* Handle OUT_1 --------------------------------------------------------- */
  setpoint = p_osfb_data->working_sp_1;

  if (setpoint < p_osfb->in_array[0]) /* SP < X11 */
  {
    out = p_osfb->out_array[0];
  }
  else if (setpoint <= p_osfb->in_array[1]) /* X11 < SP < X12 */
  {
    out = p_osfb_data->m1 * (setpoint - p_osfb->in_array[0]) + p_osfb->out_array[0];

    if (setpoint >= p_osfb_data->hyst_limit)  /* (X12 - HYST) < SP */
    {
      if (p_osfb_data->end_value_reached)
      {
        out = p_osfb_data->end_value;
      }
    }
    else /* The setpoint has left the area (X12 - HYSTVAL) */
    {
      p_osfb_data->end_value_reached = FB_FALSE;
    }
  }
  else  /* X12 < SP */
  {
    if (p_osfb->lockval== OS_LOCKVAL_LOCK) out = p_osfb->out_array[1];
    else                                   out = p_osfb->out_array[0];

    p_osfb_data->end_value_reached = FB_TRUE;
    p_osfb_data->end_value         = out;
  }

  p_osfb->out_1.value  = out;
  p_osfb->out_1.status = SQ_GOOD_CAS;


  if (p_osfb_data->downstream_1 & CASC_INIT)
  {
    if (p_osfb_data->downstream_1 == CASC_INIT_WITH_ACK)
    {
      p_osfb->out_1.status |= SUB_IA;
    }

    p_osfb_data->downstream_1 = CASC_ESTABLISHED;
  }

  /* Handle OUT_2 --------------------------------------------------------- */
  setpoint = p_osfb_data->working_sp_2;

  if (setpoint < p_osfb->in_array[2]) /* SP < X21 */
  {
    out = p_osfb->out_array[2];
  }

  else if (setpoint <= p_osfb->in_array[3]) /* X21 < SP < X22 */
  {
    out = p_osfb_data->m2 * (setpoint - p_osfb->in_array[2]) + p_osfb->out_array[2];
  }
  else /* X22 < SP */
  {
    out = p_osfb->out_array[3];
  }

  p_osfb->out_2.value  = out;
  p_osfb->out_2.status = SQ_GOOD_CAS;

  if (p_osfb_data->downstream_2 & CASC_INIT)
  {
    if (p_osfb_data->downstream_2 == CASC_INIT_WITH_ACK)
    {
      p_osfb->out_2.status |= SUB_IA;
    }

    p_osfb_data->downstream_2 = CASC_ESTABLISHED;
  }

  return;
}








FUNCTION LOCAL VOID update_bkcal_out
  (
    IN T_OSFB *       p_osfb,
    IN T_OSFB_DATA *  p_osfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  FLOAT  bkcal_low;
  FLOAT  bkcal_high;

FUNCTION_BODY

  p_osfb->bkcal_out.value = p_osfb->sp.value;

  /* Check setpoint limits ------------------------------------------------ */
  if ((p_osfb_data->downstream_1 != CASC_NOT_ESTABLISHED) ||
      (p_osfb_data->downstream_2 != CASC_NOT_ESTABLISHED)    )
  {
    if (p_osfb_data->downstream_1 != CASC_NOT_ESTABLISHED)
    {
      bkcal_low   = p_osfb->in_array[0];
      bkcal_high  = p_osfb->in_array[1];
    }

    if (p_osfb_data->downstream_2 != CASC_NOT_ESTABLISHED)
    {
      if (p_osfb_data->downstream_1 != CASC_NOT_ESTABLISHED)
      {
        if (bkcal_high < p_osfb->in_array[3])
        {
          bkcal_high  = p_osfb->in_array[3];
        }
      }
      else
      {
        bkcal_low   = p_osfb->in_array[2];
        bkcal_high  = p_osfb->in_array[3];
      }
    }

    if (p_osfb->sp.value <= bkcal_low)
    {
      p_osfb->bkcal_out.value   = bkcal_low;
      p_osfb->bkcal_out.status |= LIMIT_LOW;
    }
    else if (p_osfb->sp.value >= bkcal_high)
    {
      p_osfb->bkcal_out.value   = bkcal_high;
      p_osfb->bkcal_out.status |= LIMIT_HIGH;
    }

    /* Propagate BKCAL_IN limit bits -------------------------------------- */
    if (p_osfb_data->downstream_1 != CASC_NOT_ESTABLISHED)
    {
      if (p_osfb->bkcal_in_1.status & LIMIT_HIGH)
      {
        if (p_osfb_data->m1 > 0.0f) /* Gradient is positive */
        {
          p_osfb->bkcal_out.status |= LIMIT_HIGH;
        }
        else
        {
          p_osfb->bkcal_out.status |= LIMIT_LOW;
        }
      }
    }

    if (p_osfb_data->downstream_2 != CASC_NOT_ESTABLISHED)
    {
      if (p_osfb->bkcal_in_2.status & LIMIT_HIGH)
      {
        if (p_osfb_data->m2 > 0.0f) /* Gradient is positive */
        {
          p_osfb->bkcal_out.status |= LIMIT_HIGH;
        }
        else
        {
          p_osfb->bkcal_out.status |= LIMIT_LOW;
        }
      }
    }

    /* Check range limits ------------------------------------------------- */
    if (p_osfb_data->downstream_1 != CASC_NOT_ESTABLISHED)
    {
      if (p_osfb_data->m1 > 0.0f) /* Gradient is positive */
      {
        if (p_osfb->bkcal_in_1.value < p_osfb->out_array[0])
        {
          p_osfb->bkcal_out.status |= LIMIT_LOW;
        }
        else if (p_osfb->bkcal_in_1.value > p_osfb->out_array[1])
        {
          p_osfb->bkcal_out.status |= LIMIT_HIGH;
        }
      }
      else if (p_osfb_data->m1 < 0.0f) /* Gradient is negative */
      {
        if (p_osfb->bkcal_in_1.value > p_osfb->out_array[0])
        {
          p_osfb->bkcal_out.status |= LIMIT_LOW;
        }
        else if (p_osfb->bkcal_in_1.value < p_osfb->out_array[1])
        {
          p_osfb->bkcal_out.status |= LIMIT_HIGH;
        }
      }
      else /* Gradient is zero */
      {
        p_osfb->bkcal_out.status |= LIMIT_CONST;
      }
    }

    if (p_osfb_data->downstream_2 != CASC_NOT_ESTABLISHED)
    {
      if (p_osfb_data->m2 > 0.0f) /* Gradient is positive */
      {
        if (p_osfb->bkcal_in_2.value < p_osfb->out_array[2])
        {
          p_osfb->bkcal_out.status |= LIMIT_LOW;
        }
        else if (p_osfb->bkcal_in_2.value > p_osfb->out_array[3])
        {
          p_osfb->bkcal_out.status |= LIMIT_HIGH;
        }
      }
      else if (p_osfb_data->m2 < 0.0f) /* Gradient is negative */
      {
        if (p_osfb->bkcal_in_2.value > p_osfb->out_array[2])
        {
          p_osfb->bkcal_out.status |= LIMIT_LOW;
        }
        else if (p_osfb->bkcal_in_2.value < p_osfb->out_array[3])
        {
          p_osfb->bkcal_out.status |= LIMIT_HIGH;
        }
      }
      else /* Gradient is zero */
      {
        p_osfb->bkcal_out.status |= LIMIT_CONST;
      }
    }
  }
  else
  {
    p_osfb->bkcal_out.status = SQ_GOOD_CAS + SUB_NI;
  }

  return;
}

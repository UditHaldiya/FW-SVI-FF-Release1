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

Block     :  Multiple Analog Input

FILE_NAME :  ffbl_mai.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKMAI)

INCLUDES

  #include "base.h"
  #include "osif.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbap.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_mai.h"
  #include "ffbl_res.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID Check_config_MAIFB (T_MAIFB *, T_MAIFB_DATA *, const T_FBIF_BLOCK_DESCR *);

EXPORT_DATA


LOCAL_DATA



FUNCTION GLOBAL VOID Start_MAIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  specific initialization of block data

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_MAIFB *            p_maifb;
  T_MAIFB_DATA *       p_maifb_data;
  USIGN8               i;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_MAIFB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_maifb      = p_block_instance->p_block_desc->p_block;
  p_maifb_data = p_block_instance->p_block_data;

  p_maifb_data->old_block_alarm_bits = 0;

  /* Store out params in the internal block-specific data structure
    and set  status to BAD out-of-service --------------------------------- */
  for (i = 0;i < 8;i++)
  {
    p_maifb_data->out[i]   = p_maifb->out[i];
    p_maifb->out[i].status = SQ_BAD|SUB_OUT_OF_SERVICE;
  }

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_maifb->update_evt);


  /* Check function block configuration ----------------------------------- */
  Check_config_MAIFB (p_maifb,p_maifb_data,p_block_instance->p_block_desc);

  return;
}




FUNCTION GLOBAL VOID Stop_MAIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_MAIFB *                   p_maifb;
  T_MAIFB_DATA *              p_maifb_data;
  USIGN8                      i;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_maifb_data  = p_block_instance->p_block_data;
  p_maifb       = p_block_desc->p_block;

  if (p_maifb->mode_blk.actual == MODE_OS) return;

  p_maifb->mode_blk.actual = MODE_OS;

  /* Set block error ------------------------------------------------------ */
  p_maifb->block_err &= ~BLK_ERR_INPUT_FAILURE;
  p_maifb->block_err |= BLK_ERR_OUT_OF_SERVICE;

  for (i = 0;i < 8;i++)
  {
    /* Store out param in the internal block-specific data structure ------ */
    p_maifb_data->out[i] = p_maifb->out[i];

    if (p_maifb->block_err & BLK_ERR_BLOCK_CONFIG_ERR)
    {
      /* Set status of the OUT parameter to BAD|SUB_CONFIGURATION_ERROR ---- */
      p_maifb->out[i].status = SQ_BAD|SUB_CONFIGURATION_ERROR;
    }
    else
    {
      /* Set status of the OUT parameter to BAD|SUB_OUT_OF_SERVICE --------- */
      p_maifb->out[i].status = SQ_BAD|SUB_OUT_OF_SERVICE;
    }
  }

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_maifb->update_evt);

  /* Update block alarm ----------------------------------------------- */
  ffbl_update_block_alarm
    (
      p_block_desc->block_id,p_block_desc->idx_blk_alm,
      BLK_ERR_OUT_OF_SERVICE,&p_maifb_data->old_block_alarm_bits
    );

  return;
}





FUNCTION GLOBAL USIGN16 Read_handler_MAIFB
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

  _UNREF_PARAM (p_block_instance);
  _UNREF_PARAM (p_read);

  return (E_OK);
}






FUNCTION GLOBAL USIGN16 Write_handler_MAIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBIF_WRITE_DATA *         p_write
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  called for write access to block parameters

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_MAIFB *                   p_maifb;
  T_MAIFB_DATA *              p_maifb_data;
  USIGN8                      i,subindex;
  USIGN8 *                    source;
  USIGN16                     result;

FUNCTION_BODY

  p_block_desc  = p_block_instance->p_block_desc;
  p_maifb_data  = p_block_instance->p_block_data;
  p_maifb       = p_block_desc->p_block;
  subindex      = p_write->subindex;
  source        = p_write->source;
  result        = E_OK;

  /* Check parameter access ----------------------------------------------- */
  switch (p_write->rel_idx)
  {
    case MAI_PAR_ALERT_KEY:
    {
      if (*source == 0) result = E_FB_PARA_CHECK;

      break;
    }

    case MAI_PAR_MODE_BLK:
    {
      result = ffbl_check_mode (
                 &p_maifb->mode_blk,
                 subindex,
                 source,
                 MODE_MAI_VALID
                 );

      break;
    }

    case MAI_PAR_CHANNEL:
    {
      if (MODE_NOT_OS (p_maifb->mode_blk.target))
        return (E_FB_WRONG_MODE);

      if ( ffbl_bas_check_channel (*(USIGN16 *)source, p_block_desc))
        return E_FB_PARA_CHECK;

      break;
    }

    case MAI_PAR_OUT_1:
    case MAI_PAR_OUT_2:
    case MAI_PAR_OUT_3:
    case MAI_PAR_OUT_4:
    case MAI_PAR_OUT_5:
    case MAI_PAR_OUT_6:
    case MAI_PAR_OUT_7:
    case MAI_PAR_OUT_8:

    {
      if (MODE_LOWER_THAN_MAN (p_maifb->mode_blk.target))
           return (E_FB_WRONG_MODE);

      if ( (subindex != 0) && (subindex != 2) )
        return (E_FB_DATA_NO_WRITE);

      break;
    }

    case MAI_PAR_UPDATE_EVT:
    {
      result = ffbl_check_unack_flag (&p_maifb->update_evt.unack, source, subindex);

      break;
    }

    case MAI_PAR_BLOCK_ALM:
    {
      result = ffbl_check_unack_flag (&p_maifb->block_alm.unack, source, subindex);

      break;
    }

    default:
    {
      break;
    }
  }  /* switch rel_idx */


  if (result == E_OK)
  {
    result = fbs_write_param (p_block_instance,p_write);
  }


  if (result == E_OK)
  {
    BOOL check_config = FALSE;

    switch (p_write->rel_idx)
    {
      case MAI_PAR_BLK_DATA:
      case MAI_PAR_CHANNEL:
        check_config = TRUE;
        break;

      case MAI_PAR_MODE_BLK:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        write_loc.rel_idx = 0;

        if (p_maifb->mode_blk.target & MODE_OS)
        {
          for (i = 0;i < 8;i++)
          {
            p_maifb->out[i].status = SQ_BAD|SUB_OUT_OF_SERVICE;
          }

          write_loc.rel_idx = MAI_PAR_OUT_1;
        }

        else if (p_maifb->mode_blk.target & MODE_MAN)
        {
          write_loc.rel_idx = MAI_PAR_OUT_1;
        }

        if (write_loc.rel_idx)
        {
          /* Store out parameter in non-volatile RAM ---------------------- */
          write_loc.subindex      = 0;
          write_loc.length        = 5;
          write_loc.startup_sync  = FALSE;

          for (i = 0;i < 8;i++)
          {
            write_loc.rel_idx = MAI_PAR_OUT_1 + i;
            write_loc.source  = (USIGN8 *)&p_maifb->out[i];

            fbs_write_param_loc (p_block_instance,&write_loc);
          }
        }

        break;
      }

      case MAI_PAR_OUT_1:
      case MAI_PAR_OUT_2:
      case MAI_PAR_OUT_3:
      case MAI_PAR_OUT_4:
      case MAI_PAR_OUT_5:
      case MAI_PAR_OUT_6:
      case MAI_PAR_OUT_7:
      case MAI_PAR_OUT_8:
      {
        T_FBS_WRITE_PARAM_LOCAL write_loc;

        i = p_write->rel_idx - MAI_PAR_OUT_1;

        p_maifb->out[i].status = SQ_GOOD_NC;

        /* Store GOOD out status in non-volatile RAM ---------------------- */
        write_loc.rel_idx       = p_write->rel_idx;
        write_loc.subindex      = 0;
        write_loc.length        = 5;
        write_loc.source        = (USIGN8 *)&p_maifb->out[i];
        write_loc.startup_sync  = FALSE;

        fbs_write_param_loc (p_block_instance,&write_loc);

        /* Store out param in the internal block-specific data structure -- */
        p_maifb_data->out[i] = p_maifb->out[i];

        /* Adapt out status in accordance to the actual mode -------------- */
        if (p_maifb->mode_blk.actual == MODE_OS)
        {
          p_maifb->out[i].status = SQ_BAD|SUB_OUT_OF_SERVICE;
        }
        else
        {
          p_maifb->out[i].status |= LIMIT_CONST;
        }

        break;
      }
    }

    if (check_config)
    {
      Check_config_MAIFB(p_maifb,p_maifb_data,p_block_desc);
    }
  }

return result;
}





FUNCTION GLOBAL USIGN16 Execute_MAIFB
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  execute function block algorithm

  Return  next_fb_to_execute  next function block to be executed (used if
                            'completion of block exec.' is set in CYCLE_SEL)

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_MAIFB *                   p_maifb;
  T_MAIFB_DATA *              p_maifb_data;
  USIGN8                      old_mode;
  USIGN8                      actual_mode;
  USIGN8                      i;
  BOOL                        enable_update_evt;

FUNCTION_BODY

  p_block_desc = p_block_instance->p_block_desc;
  p_maifb_data = p_block_instance->p_block_data;
  p_maifb      = p_block_desc->p_block;


  /* Update block mode -------------------------------------------------- */
  old_mode    = p_maifb->mode_blk.actual;
  actual_mode = ffbl_bas_calc_block_mode (&p_maifb->mode_blk,p_maifb->block_err);

  if (actual_mode == MODE_OS)
  {
    if (old_mode != MODE_OS)
    {
      /* Mode OS is entered now ------------------------------------------- */
      Stop_MAIFB (p_block_instance);
    }
    else
    {
      /* Update block alarm ----------------------------------------------- */
      ffbl_update_block_alarm
      (
        p_block_desc->block_id,p_block_desc->idx_blk_alm,
        BLK_ERR_OUT_OF_SERVICE,&p_maifb_data->old_block_alarm_bits
      );
    }

    return(p_maifb->blk_data.next_fb_to_execute);
  }

  p_maifb->mode_blk.actual = actual_mode;

  if (old_mode == MODE_OS) /* Mode OS is left now ------------------------- */
  {
    /* Reset block error -------------------------------------------------- */
    p_maifb->block_err &= ~BLK_ERR_OUT_OF_SERVICE;
    enable_update_evt   = TRUE;

    /* Restore out parameters --------------------------------------------- */
    for (i = 0;i < 8;i++)
    {
      p_maifb->out[i] = p_maifb_data->out[i];
    }
  }
  else
  {
    enable_update_evt = FALSE;
  }


  if (actual_mode == MODE_AUTO)
  {
    p_maifb_data->exec_tb.p_process_value = p_maifb->out;
    p_maifb_data->exec_tb.tb_error        = FALSE;

    (*p_maifb_data->linked_tb.a_of_execute_tb)
          (p_maifb_data->linked_tb.p_tb_instance,&p_maifb_data->exec_tb);
  }
  else /* (actual_mode == MODE_MAN) */
  {
    for (i = 0;i < 8;i++)
    {
      if ((p_maifb->out[i].status & QUALITY_MASK) != SQ_BAD)
      {
        p_maifb->out[i].status |= LIMIT_CONST;
      }
    }
  }

  /* Update block alarm --------------------------------------------------- */
  ffbl_update_block_alarm
  (
    p_block_desc->block_id,p_block_desc->idx_blk_alm,
    p_maifb->block_err,&p_maifb_data->old_block_alarm_bits
  );

  if (enable_update_evt)
  {
    ffbl_enable_update_events
    (
      p_block_desc->block_id,p_block_desc->idx_upd_evt,
      &p_maifb->update_evt,p_maifb->st_rev
    );
  }

  return(p_maifb->blk_data.next_fb_to_execute);
}







FUNCTION GLOBAL VOID Background_MAIFB
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








FUNCTION LOCAL VOID Check_config_MAIFB
  (
    IN T_MAIFB *                   p_maifb,
    IN T_MAIFB_DATA *              p_maifb_data,
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

  if (p_maifb->blk_data.period_of_execution == 0)
  {
    block_err_desc |= BLK_ERR_EXE_TIME;
  }


  /* Check if the MAI block is linked to a transducer block --------------- */
  if (p_maifb->channel == 0)
  {
    block_err_desc |= BLK_ERR_CHANNEL_INIT;
  }
  else
  {
    p_maifb_data->exec_tb.channel = fbs_link_transducer_block
                     (p_maifb->channel,p_block_desc,&p_maifb_data->linked_tb);

    if (p_maifb_data->exec_tb.channel == 0)
    {
      block_err_desc |= BLK_ERR_CHANNEL_SETTING;
    }

    p_maifb_data->exec_tb.p_xd_scale = NULL;
  }

  if (block_err_desc == BLK_ERR_NO_ERROR)
  {
    p_maifb->block_err &= ~BLK_ERR_BLOCK_CONFIG_ERR;
  }
  else
  {
    p_maifb->block_err |= BLK_ERR_BLOCK_CONFIG_ERR;
  }

  p_maifb->block_err_desc_1 = block_err_desc;

  return;
}


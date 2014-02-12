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


/* ===========================================================================

FILE_NAME          appl_res.c



FUNCTIONAL_MODULE_DESCRIPTION
  This module contains examples of routines that implement the application-
  related part of the resource block.

  If the resource block contains standard parameters only then the application-
  related part of the resource block is pretty small. This module shows some
  extra functionality that is executed when Restart-with-defaults is applied.

  If a device uses an enhanced resource block the functions implemented in
  this module may get larger.

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FAPP + MOD_APPL_RES)

INCLUDES
  #include "base.h"
  #include "except.h"
  #include "hw_if.h"
  #include "eep_if.h"
  #include "ffbl_api.h"
  #include "fbif_idx.h"
  #include "appl_int.h"
  #include "resb.h"
  #include "mn_working_times.h"
  #include "mn_grant_deny.h"
  #include "mn_factory_defaults.h"
  #include "hartdef.h"
  #include "mn_fstate.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA

#if defined (HART_MASTER) || defined (MODBUS_MASTER)
  BOOL doRestartUART = FALSE;
#endif

#define REQ_MAX_LEN1       (HC171_REQ_LENGTH + MN_OFFSETOF(Req_WriteUIlanguage_t, endmark))
#define RSP_MAX_LEN1       (HC171_LENGTH + MN_OFFSETOF(Rsp_WriteUIlanguage_t, endmark))
#define REQ_MAX_LEN2       MAX(REQ_MAX_LEN1, HC171_REQ_LENGTH + MN_OFFSETOF(Req_WriteUIAccessControl_t, endmark))
#define RSP_MAX_LEN2       MAX(RSP_MAX_LEN1, HC171_LENGTH + +MN_OFFSETOF(Rsp_WriteUIAccessControl_t, endmark))
#define REQ_MAX_LEN3       MAX(REQ_MAX_LEN2, HC42_REQ_LENGTH + MN_OFFSETOF(Req_Reset_t, endmark))
#define RSP_MAX_LEN3       MAX(RSP_MAX_LEN2, HC42_LENGTH + MN_OFFSETOF(Rsp_Reset_t, endmark))
#define REQ_MAX_LEN4       MAX(REQ_MAX_LEN3, HC135_REQ_LENGTH + MN_OFFSETOF(Req_ChangeDeviceMode_t, endmark))
#define RSP_MAX_LEN4       MAX(RSP_MAX_LEN3, HC135_LENGTH + MN_OFFSETOF(Rsp_ChangeDeviceMode_t, endmark))

#define MAX_RCV_BUF_SIZE   RSP_MAX_LEN4
#define MAX_SND_BUF_SIZE   REQ_MAX_LEN4

static u8 snd_buf[MAX_SND_BUF_SIZE];
static u8 rcv_buf[MAX_RCV_BUF_SIZE];

/****************************************************************************/

FUNCTION GLOBAL USIGN16 Appl_read_handler_RESB
  (
    T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    T_FBIF_READ_DATA *          p_read
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function will be called by read handler of the FB shell if a resource
  block parameter with a 'A' or 'X' GenVFD-flag is read.

  As the function block layer handles read accesses to all standard parameters
  it is assumed that the 'A' and 'X' flags are set just for enhanced resource
  block parameters.

  The 'A' flag can be set for parameters that are temporary inconsistent
  or not accessible. Typically the 'A' flag is assigned to dynamic
  parameter that are updated just when they are read. In this case
  Appl_read_handler_RESB() has to make sure that the parameter is up-to-date
  before it returns. If it is not possible to get a consistent state or value
  Appl_read_handler_RESB() may return with E_FB_TEMP_ERROR. Please note that
  sporadic denials of read accesses are usually accepted by host systems.
  However, iterated negative read responses are indicated as device errors
  at the system-side.

  A parameter with an 'X' flag is completely under the control of the appli-
  cation. It is not stored by the funcion block layer. When it is called
  Appl_read_handler_RESB() has to copy the current parameter value to the read
  response buffer. The read response buffer is referenced by the component
  'dest' of the read descriptor. Please note that access by subindex is
  supported. I.e. in case of record or array parameters  Appl_read_handler_RESB()
  has to check whether the whole parameter or just one component is read.

PARAMETERS
  p_block_instance      Pointer to the resource block instance

  p_read                Pointer to a read descriptor block
                        (For the structure of the read descriptor see fbif.h)

RETURN_VALUES
  E_OK                  Read access is approved and the parameter is up-to-date
  E_FB_TEMP_ERROR       Read access is not possible at the moment
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

#if 1
  _UNREF_PARAM (p_block_instance);
  _UNREF_PARAM (p_read);

#else
  error APP FW Revision si incomplete!

  #define   PLACEHOLDER_REVISION    (0x01000102)

  T_FBIF_RESB *  p_RESB = p_block_instance->p_block_desc->p_block;

  switch(p_read->rel_idx)
  {
      case REL_IDX_RESB_SOFTWARE_REV_APP :
          p_RESB->software_rev_app = PLACEHOLDER_REVISION;
          break;
  }

#endif

  return (E_OK);
}




bool_t rb_HasRun = false;



FUNCTION GLOBAL USIGN16 Appl_write_handler_RESB
  (
    T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    T_FBIF_WRITE_DATA *         p_write
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function will be called by write handler of the FB shell if a resource
  block parameter with a 'B' or 'X' GenVFD-flag has to be written.

  During the write handling this function is called two times. When it is
  called the first time, the component 'phase' of the write descriptor is
  set to PRE_WRITE_CHECK. In this phase the value to be written should be
  validated.

  When Appl_write_handler_RESB() is called the second time, the component
  'phase' of the write descriptor is set to POST_WRITE_ACTION. This phase is
  intended to perform actions in dependance on the new value.

  Please note that the write handler of the FB shell does not send the write
  response before Appl_write_handler_RESB() returns from its POST_WRITE_ACTION.
  If complex actions have to be performed it is a good idea to just send a
  message to an other application task. This additional application task is
  intended to have a lower priority than the FDC task. When this additional
  application task receives the message from a_of_write_hndl_RESB() it can
  update the application in dependance on the new parameter value without
  deferring the sending of the write response via the fieldbus.

  A parameter with an 'X' flag is completly under the control of the
  application. Thus, the storage of the new parameter value is a mandatory
  POST_WRITE_ACTION. If the parameter to be written is static, the appli-
  cation has in addition to increment the static revision counter. The FB
  shell provides the fbs_incr_st_rev() routine to do this.

  If the parameter to be written has got no 'X' flag, the standard write
  handler steps - check of the new value,  storage of the new value,
  incrementation of the static revision counter - are done by the write
  handler of the FB shell.

  In this example a write access to the 'restart' parameter is indicated to
  the application layer. A non-standard reset of the non-volatile memory is
  implemented. The non-standard reset is used during the development phase.
  It may be removed in the product version of a device.

PARAMETERS
  p_block_instance      Pointer to the resource block instance

  p_write               Pointer to a write descriptor block
                        (For the structure of the write descriptor see fbif.h)

RETURN_VALUES
  In the PRE_WRITE_CHECK phase:
    E_OK                   Parameter check was successful
    E_FB_PARA_CHECK        Parameter check was not successful
    E_FB_PARA_LIMIT        Parameter exceeds a limit
    E_FB_WRONG_MODE        Parameter is not writable in the current target mode
    E_FB_DATA_NO_WRITE     The component selected by subindex is read-only
    E_FB_TEMP_ERROR        It is not possible to write the parameter at the moment

  In the POST_WRITE_ACTION phase:
    E_OK                  The application-specific actions are completed
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16   result;

FUNCTION_BODY

  _UNREF_PARAM (p_block_instance);

  result = E_OK;

  if (p_write->phase == PRE_WRITE_CHECK)
  {
    if (p_write->rel_idx == REL_IDX_RESB_RESTART)
    {
      /* The 'restart' parameter should be written -------------------------- */
      if (*p_write->source == RS_RESTART_WITH_DEFAULTS)
      {
        /* After the return of this function all FBAP objects (i.e. block
           parameters, link objects, trend objects are reset to its default
           values. The application may perform device-specific actions before
           the parameters are reset.                                          */

      }
      else if (*p_write->source == RS_RESTART_PROCESSOR)
      {
        /* After the return of this function the resource block restarts the
           processor by calling the Reset_CPU function. The application may
           perform device-specific actions before the processor is reset      */
          SetResetFlag();
          result = ffres_restart_APP(snd_buf, rcv_buf);

      }
      else if (*p_write->source == RS_RESTART_FACTORY_DEFAULTS)
      {
          /*restore the TB blcok in FF and all the APP parameter*/
          result = ffres_restart_factory_defaults(snd_buf, rcv_buf);
      }
      else
      {
      }
    }
    else if(p_write->rel_idx == REL_IDX_RESB_GRANT_DENY)
    {
        result = WriteGrantDeny(p_block_instance, p_write);
    }
  }
  else /* p_write->phase == POST_WRITE_ACTION */
  {
    if (p_write->rel_idx == REL_IDX_RESB_RESTART)
    {
      if (*p_write->source == RS_RESTART_FACTORY_DEFAULTS)
      {
        /* If the non-standard values RS_RESTART_FACTORY_DEFAULTS
           are written a non-standard reset of the NV-RAM is executed.

           The non-standard reset of the NV-RAM area that is controlled by
           the EEP interface works as follows:

           i)   The modified 'Config Revision' is stored in the NV-RAM
           ii)  In the succeeding startup phase the application compares the
                'Config Revision' from the firmware with the 'Config Revision'
                stored in NV-RAM. As it will detect different values, the
                application task assumes inconsistent NV-RAM data and
                subsequently it starts the device with default values.
        */

        /* Store a modified 'config_revision' in NV-RAM ---------------------
        we donot need to use default values here!*/
        //appl_if_modify_config_rev ();

        /* Reset processor -------------------------------------------------- */
        //Reset_CPU ();
      }
      else if (*p_write->source == RS_RESTART_HIDDEN)
      {
          result = ffres_restart_hidden(snd_buf, rcv_buf);
      }

      else if (*p_write->source == RS_RESTART_WITH_DEFAULTS)
      {
        #ifdef USE_FIELD_DIAGNOSTICS /* --------------------------------------- */
        appl_dia_start_field_diagnostics ();
        #endif /* USE_FIELD_DIAGNOSTICS */

        #if defined (HART_MASTER) || defined (MODBUS_MASTER)
        doRestartUART = TRUE;
        rb_HasRun = false;
        #endif
        result = ffres_restart_restore_advanced();
      }
      //used for watch dog test.
      else if (*p_write->source == RS_RESTART_TEST_WATCHDOG)
      {
          while(1)
          {
              ;
          }
      }
      else
      {
      }
    }
  }


  return (result);
}





FUNCTION GLOBAL VOID Appl_background_RESB
  (
    T_FBIF_BLOCK_INSTANCE * p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called periocally by the background task. It can be used to
  perform resource block or FBAP related background actions.


  ....


PARAMETERS
  p_block_instance      Pointer to the resource block instance

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _UNREF_PARAM (p_block_instance);

  #ifdef USE_FIELD_DIAGNOSTICS /* ----------------------------------------- */
  {
    T_FBIF_RESB *  p_RESB = p_block_instance->p_block_desc->p_block;

    _UNREF_PARAM (p_RESB);

    /* Make sure that in case of USE_FIELD_DIAGNOSTICS the multi-bit
       feature flag is set                                                  */

    _ASSERT (p_RESB->features & RES_FEAT_MULTIBIT_ALARM_SUPP);
  }

  appl_dia_update_field_diagnostics ();

  #endif /* USE_FIELD_DIAGNOSTICS */

  rb_HasRun=true;
  return;
}








FUNCTION GLOBAL VOID Appl_get_default_value
  (
    T_FBIF_BLOCK_INSTANCE * p_block_instance,
    T_FBIF_READ_DATA *      p_read
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function will be called if the block parameter are set to their
  default values and if in the GenVFD script a parameter is attributed
  with the Y-flag. The 'Y' flag indicates that the value from the GenVFD
  script should not be used as default value. In this case the application
  has to provide the default value.

PARAMETERS
  p_block_instance      Pointer to the block that conains the 'Y' parameter

  p_read                Pointer to a read descriptor. The read descriptor
                        specifies the parameter that needs a default value
                        (For the structure of the read descriptor see fbif.h)

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _UNREF_PARAM (p_block_instance);
  _UNREF_PARAM (p_read);

  return;

}



#ifdef FBL_BLOCK_INST /* -------------------------------------------------- */

FUNCTION GLOBAL USIGN8 Appl_check_block_state
  (
    IN USIGN16     block_id,
    IN USIGN8      action
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
 This function is called if a block is to be activated or deactivated via
 fieldbus. The application may decide if the action is performed or
 rejected.

 In this example all activation or deactivation requests accepted. In a
 real device the application layer has to check this request carefully.
 Especially it has to check if the request is conform to the CFF file keys
 'NumOfInstantiableBlocks' and 'NumOfPermanentBlocks'.

PARAMETERS
  block_id          Block identifier as generated by GenVFD
  action            BLK_INACTIVE  An active block should be deactivated
                    BLK_ACTIVE    An inactive block should be activated

RETURN_VALUES
  TRUE              The application agrees with the action
  FALSE             The application refuses the action
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _UNREF_PARAM (block_id);
  _UNREF_PARAM (action);

  return (TRUE);
}

#endif /* FBL_BLOCK_INST */






#if defined (HART_MASTER) || defined (MODBUS_MASTER)
FUNCTION GLOBAL BOOL Appl_check_restart_flag (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  return(doRestartUART);

}


FUNCTION GLOBAL VOID Appl_reset_restart_flag (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  doRestartUART = FALSE;

  return;
}
#endif





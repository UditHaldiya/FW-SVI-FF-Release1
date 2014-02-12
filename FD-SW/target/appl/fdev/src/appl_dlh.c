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

FILE_NAME          appl_dlh.c


FUNCTIONAL_MODULE_DESCRIPTION

  This module implements an example of a domain download handler. The download
  handler is the counter part of the download server which is a part of the
  System Management Agent.

  The download server implements the download state machine as specified
  in FF-883. The download handler is responsible for checking and storing
  the downloaded domain. Both, download handler and download server implement
  the software download function of a device.

  This module is intended to be an example for class 1 as well as for class 3
  software download. Its current state implements an example for class 1
  software download only. Future releases of this module will also implement
  an example of class 3 software download.

  This module is intended to be an example for single domain software download
  as well as for multi-domain software download. Its current state implements
  an example for single domain software download only. Future releases of this
  module will also implement an example of multi-domain software download.

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FAPP + MOD_APPL_DLH)

INCLUDES
  #include "base.h"
  #include "hw_if.h"
  #include "except.h"
  #include "osif.h"
  #include "eep_if.h"
  #include "swdl_cfg.h"
  #include "swdl_api.h"
  #include "appl_int.h"

  #include "appl_svi.h"


LOCAL_DEFINES


  #define _INIT_PLATFORM_SPECIFIC_DWNLD_FUNC        init_svi_dwnld()

  #define _FD_SW_IS_AVAILABLE                       appl_fd_sw_available()

  #define _PREPARE_DWNLD_OF_FD_SW                   start_prepare_swdl()

  #define _FINISH_FD_DWNLD_PREPARATION_PHASE        finish_prepare_swdl()

  #define _FD_SW_DOM_READY_FOR_DWNLD

  #define _CHECK_FD_DOMAIN_HDR(p_dwnld_dom_hdr)                     \
                    appl_check_fd_dom_hdr (p_dwnld_dom_hdr)

  #define _STORE_FD_DOMAIN_HDR(p_dwnld_dom_hdr)                     \
                    appl_store_fd_dom_hdr (p_dwnld_dom_hdr)

  #define _CHECK_FD_DOMAIN_SEGM(segment_id,segment_len,p_segment)   \
                    appl_check_fd_dom_segm (segment_id,segment_len,p_segment)

  #define _STORE_FD_DOMAIN_SEGM(segment_id,segment_len,p_segment)   \
                    dwnld_handle_data(segment_id,segment_len,p_segment)

  #define _COMPLETE_FD_DOM_IS_AVAILABLE             complete_fd_dom_is_avail()

  #define _RESET_FD_DOMAIN                          start_prepare_swdl()

  #define _FINISH_FD_DWNLD_ACTIVATION_PHASE         verify_activation()

  #define _ACTIVATE_FD_FW                           activate_fd_sw()


  #define NEW_FD_DOM_MFG_ID         FD_DOM_MFG_ID
  #define NEW_FD_DOM_FAMILY         FD_DOM_FAMILY
  #define NEW_FD_DOM_DEV_TYPE       FD_DOM_DEV_TYPE
  #define NEW_FD_DOM_DEV_REV        FD_DOM_DEV_REV
  #define NEW_FD_DOM_DD_REV         FD_DOM_DD_REV
  #define NEW_FD_DOM_SW_REV         FD_DOM_SW_REV
  #define NEW_FD_DOM_SW_NAME        FD_DOM_SW_NAME


LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION GLOBAL USIGN8 Appl_prepare_download
    (
      IN USIGN8               dom_id
    );

  FUNCTION GLOBAL T_DOMAIN_HDR_API * Appl_cancel_download
    (
      IN USIGN8               dom_id
    );

  FUNCTION GLOBAL USIGN8 Appl_download_header
    (
      IN USIGN8               dom_id,
      IN T_DOMAIN_HDR_API *   p_dwnld_dom_hdr
    );

  FUNCTION GLOBAL USIGN8 Appl_download_segment
    (
      IN USIGN8               dom_id,
      IN USIGN8               segment_id,
      IN USIGN8               segment_len,
      IN USIGN8 *             p_segment
    );

  FUNCTION GLOBAL VOID Appl_activate_software
    (
      IN USIGN8               dom_id
    );

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA
#include "da_fdev.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT STATIC T_DWNLD_PROPERTY_API   appl_dwnld_prop;
  NO_INIT STATIC T_MULTIDOM_DESCR_API   appl_fd_multidom_descr;
  NO_INIT STATIC T_MULTIDOM_DESCR_API   appl_ap_multidom_descr;
  NO_INIT STATIC T_DOMAIN_DESCR_API     appl_fd_dom_descr;
  NO_INIT STATIC T_DOMAIN_DESCR_API     appl_ap_dom_descr;
  NO_INIT        T_DOMAIN_HDR_API       appl_fd_dom_hdr;
  NO_INIT STATIC T_DOMAIN_HDR_API       appl_ap_dom_hdr;
  NO_INIT STATIC T_DWNLD_CFG_API        appl_dwnld_cfg;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

typedef struct domdesc_t
{
    T_DOMAIN_HDR_API dh1;
    T_DOMAIN_DESCR_API dh2;
} domdesc_t;

const domdesc_t domainHdr =
{
     { FD_DOM_MFG_ID,
      FD_DOM_FAMILY,
      FD_DOM_DEV_TYPE,
      FD_DOM_DEV_REV,
      FD_DOM_DD_REV,
      FD_DOM_SW_REV,
      FD_DOM_SW_NAME},
     { FD_DOM_DOM_NAME }
};

/****************************************************************************/

FUNCTION PUBLIC VOID appl_dhl_init (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS
  None

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY


  smdl_call_fdev_funct.a_of_prepare_download  = Appl_prepare_download;
  smdl_call_fdev_funct.a_of_cancel_download   = Appl_cancel_download;

  _INIT_PLATFORM_SPECIFIC_DWNLD_FUNC;

  smdl_call_fdev_funct.a_of_download_header   = Appl_download_header;
  smdl_call_fdev_funct.a_of_download_segment  = Appl_download_segment;
  smdl_call_fdev_funct.a_of_activate_software = Appl_activate_software;

  return;
}



FUNCTION PUBLIC VOID appl_dlh_start (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called in the device startup phases when the APPL task
  handles the SYSSTART event.

  With this function the download handler configures the download server.

  If the device has been power-cycled during a software download, the download
  handler has take the software download into a proper state.

  This function performs the following steps:
  a) Ask the download server about the state of the download procedure
  b) Configure the download server in dependence of the current state
  c) Complete the preparation of a download or the activation of the new
     software - if necessary

PARAMETERS
  None

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_DWNLD_STATE_API     dwnld_state;
  USIGN16               result;

FUNCTION_BODY


  /* Ask the download server about the state of the download procedure ---- */
  result = fdc_get_dwnld_state (&dwnld_state);

  if (result != E_OK)
  {
    _FAPP_FATAL (XCP_FAPP_FDC_ERR,XCP_FAPP_IDENT_LOAD,result);
  }

  /* Initialize the download property structure --------------------------- */

  appl_dwnld_prop.resp_for_prepare                = SEND_WRITE_RESP_FOR_DWNLD_PREPARE; /* Class 1 */
  appl_dwnld_prop.max_time_for_preparation        = MAX_TIME_FOR_DWNLD_PREPARE;
  appl_dwnld_prop.max_time_for_activation         = MAX_TIME_FOR_SW_ACTIVATE;


  /* Initialize the download descriptor ----------------------------------- */
  memcpy (appl_fd_dom_descr.dom_name,               FD_DOM_DOM_NAME, 8);

  if (dwnld_state.fd_dwnld_state == SWDL_DOM_NOT_READY_FOR_DWNLD)
  {
    /* No download in progress -------------------------------------------- */

    /* There are two cases that could take a device into this state.

      a) No download has been started or interrupted - the device will continue
         with its normal operation
      b) The software download procedure was canceled and the device was re-started.

      In case b) there are different requirements on class 1 and class 3
      devices. A class 3 device will not be able to restored the deactivated
      software and the download server will not indicate an active domain
      header. In a class 1 device the deactivated software is still running.
      A class 1 download handler shall provide the domain header of the running
      software to the download server. The download server will indicate it as
      header of the active domain.                                          */

    if (_FD_SW_IS_AVAILABLE)
    {
      /* Provide the header of the active FD domain to the download server  */
      memcpy (appl_fd_dom_hdr.manufacturer_id,        FD_DOM_MFG_ID, 6);
      memcpy (appl_fd_dom_hdr.device_family,          FD_DOM_FAMILY, 4);
      memcpy (appl_fd_dom_hdr.device_type,            FD_DOM_DEV_TYPE, 4);
      appl_fd_dom_hdr.device_revision               = FD_DOM_DEV_REV;
      appl_fd_dom_hdr.dd_revision                   = FD_DOM_DD_REV;
      memcpy (appl_fd_dom_hdr.software_revision,      FD_DOM_SW_REV, 8);
      memcpy (appl_fd_dom_hdr.software_name,          FD_DOM_SW_NAME, 8);

    }
    else /* No field device software available */
    {
      /* Invalidate domain header ----------------------------------------- */
      memcpy (appl_fd_dom_hdr.manufacturer_id,        SWDL_NO_MANUFAC_ID, 6);
    }
  }

  else if (dwnld_state.fd_dwnld_state == SWDL_DOM_COMPLETE)
  {
    /* The device was stopped at a time where the domain had been loaded
       successfully and the device was waiting for the activation command.  */

    /* It depends on the implementation of the download handler whether or
       not the downloaded software is still available for activation. If
       the downloaded software is still available the download handler will
       provide its domain header to the download server. If the downloaded
       software is not available anymore the download handler will invalidate
       the domain header by setting the SWDL_NO_MANUFAC_ID.                 */


    if (_COMPLETE_FD_DOM_IS_AVAILABLE)
    {

      memcpy (appl_fd_dom_hdr.manufacturer_id,      NEW_FD_DOM_MFG_ID, 6);
      memcpy (appl_fd_dom_hdr.device_family,        NEW_FD_DOM_FAMILY, 4);
      memcpy (appl_fd_dom_hdr.device_type,          NEW_FD_DOM_DEV_TYPE, 4);
      appl_fd_dom_hdr.device_revision             = NEW_FD_DOM_DEV_REV;
      appl_fd_dom_hdr.dd_revision                 = NEW_FD_DOM_DD_REV;
      memcpy (appl_fd_dom_hdr.software_revision,    NEW_FD_DOM_SW_REV, 8);
      memcpy (appl_fd_dom_hdr.software_name,        NEW_FD_DOM_SW_NAME, 8);
    }
    else /* The downloaded domain isn't available anymore */
    {

      /* Invalidate domain header ----------------------------------------- */
      memcpy (appl_fd_dom_hdr.manufacturer_id,      SWDL_NO_MANUFAC_ID, 6);
    }
  }

  /* Initialize the download config structure ----------------------------- */
  appl_dwnld_cfg.p_property                       = &appl_dwnld_prop;
  appl_dwnld_cfg.p_fd_multidom_descr              = &appl_fd_multidom_descr;
  appl_dwnld_cfg.p_ap_multidom_descr              = &appl_ap_multidom_descr;
  appl_dwnld_cfg.p_fd_dom_descr                   = &appl_fd_dom_descr;
  appl_dwnld_cfg.p_ap_dom_descr                   = &appl_ap_dom_descr;
  appl_dwnld_cfg.p_fd_dom_hdr                     = &appl_fd_dom_hdr;
  appl_dwnld_cfg.p_ap_dom_hdr                     = &appl_ap_dom_hdr;


  /* Provide the config data to the download server ----------------------- */
  result = fdc_config_dwnld_server (&appl_dwnld_cfg);

  if (result != E_OK)
  {
    _FAPP_FATAL (XCP_FAPP_FDC_ERR,XCP_FAPP_IDENT_LOAD,result);
  }


  /* Continue in the download procedure - if necessary -------------------- */
  if (dwnld_state.fd_dwnld_state == SWDL_DOM_PREPARING)
  {
    /* The device was stopped while it was preparing a software download.
       In case of NO_WRITE_RESP_FOR_DWNLD_PREPARE the reset may be
       triggered by the download handler itself.                            */

    /* Finish the preparation phase. The possible results of the
       preparation phase are SWDL_PREPARE_OK and SWDL_PREPARE_FAIL          */

    if (_FINISH_FD_DWNLD_PREPARATION_PHASE == E_OK)
    {
      fdc_complete_dwnld_action (FD_SW_DOM_ID, SWDL_PREPARE_OK, NULL);
    }
    else /* Download preparation failed */
    {
      memcpy (appl_fd_dom_hdr.manufacturer_id,      FD_DOM_MFG_ID, 6);
      memcpy (appl_fd_dom_hdr.device_family,        FD_DOM_FAMILY, 4);
      memcpy (appl_fd_dom_hdr.device_type,          FD_DOM_DEV_TYPE, 4);
      appl_fd_dom_hdr.device_revision             = FD_DOM_DEV_REV;
      appl_fd_dom_hdr.dd_revision                 = FD_DOM_DD_REV;
      memcpy (appl_fd_dom_hdr.software_revision,    FD_DOM_SW_REV, 8);
      memcpy (appl_fd_dom_hdr.software_name,        FD_DOM_SW_NAME, 8);

      fdc_complete_dwnld_action (FD_SW_DOM_ID, SWDL_PREPARE_FAIL, &appl_fd_dom_hdr);
    }
  }

  else if (dwnld_state.fd_dwnld_state == SWDL_DOM_READY_FOR_DWNLD)
  {
    /* The device was stopped after the preparation has finished and before
       the Download is started.                                            */

    _FD_SW_DOM_READY_FOR_DWNLD;

  }


  else if (dwnld_state.fd_dwnld_state == SWDL_DOM_DOWNLOADING)
  {
    /* The device was stopped during a download process. As the download
       server does not support download recovery, a partially stored domain
       has to be reset.                                                    */

    _RESET_FD_DOMAIN;

    /* (The host will re-start the download procedure by sending a prepare
       command)                                                             */
  }

  else if (dwnld_state.fd_dwnld_state == SWDL_DOM_ACTIVATING)
  {
    /* The device was stopped while it was activating a downloaded software.
       As the device restart is a mandatory step in the activation procedure
       the reset was possibly triggered by the download handler itself      */

    /* Finish the activation phase. The possible results of the
       activation phase are SWDL_ACTIVATE_OK and SWDL_ACTIVATE_FAIL         */

    if (_FINISH_FD_DWNLD_ACTIVATION_PHASE == E_OK)
    {
      memcpy (appl_fd_dom_hdr.manufacturer_id,      FD_DOM_MFG_ID, 6);
      memcpy (appl_fd_dom_hdr.device_family,        FD_DOM_FAMILY, 4);
      memcpy (appl_fd_dom_hdr.device_type,          FD_DOM_DEV_TYPE, 4);
      appl_fd_dom_hdr.device_revision             = FD_DOM_DEV_REV;
      appl_fd_dom_hdr.dd_revision                 = FD_DOM_DD_REV;
      memcpy (appl_fd_dom_hdr.software_revision,    FD_DOM_SW_REV, 8);
      memcpy (appl_fd_dom_hdr.software_name,        FD_DOM_SW_NAME, 8);

      fdc_complete_dwnld_action (FD_SW_DOM_ID,SWDL_ACTIVATE_OK, &appl_fd_dom_hdr);
    }
    else /* The software activation failed */
    {
      fdc_complete_dwnld_action (FD_SW_DOM_ID,SWDL_ACTIVATE_FAIL, NULL);
    }
  }

  return;
}






FUNCTION GLOBAL USIGN8 Appl_prepare_download
  (
    IN USIGN8   dom_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by the download server when a prepare download
  command has been received.

  The download handler has to prepare the software download. For a class 3
  device restarting will be  one step of the preparation; a class 1 device
  is not allowed to restart during the preparation phase.

PARAMETERS
  dom_id    FD_SW_DOM_ID

RETURN_VALUES
  E_OK                  The preparation command is accepted
  E_SWDL_TEMP_DENIED    Software download is not possible at the moment
  E_SWDL_NOT_POSSIBLE   Software download is not possible for this device

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (dom_id == FD_SW_DOM_ID)
  {
    _PREPARE_DWNLD_OF_FD_SW;
  }
  else
  {
    _ASSERT(TRUE); /* MULTIDOMAIN Not Supported */
  }


  if (appl_dwnld_prop.resp_for_prepare == SEND_WRITE_RESP_FOR_DWNLD_PREPARE)
  {
    fdc_complete_dwnld_action (dom_id, SWDL_PREPARE_OK, NULL);
  }
  else /* appl_dwnld_prop.resp_for_prepare == NO_WRITE_RESP_FOR_DWNLD_PREPARE */
  {
    Reset_CPU ();     /* A class 1 device is not allowed to restart
                         in the preparation phase!                   */
  }

  return E_OK;
}







FUNCTION GLOBAL T_DOMAIN_HDR_API * Appl_cancel_download
  (
    IN USIGN8   dom_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by the download server when the download procedure is
  aborted. The abort can be triggered by the host by sending a cancel command;
  the procedure is also aborted when the underlying connection has been closed.

PARAMETERS
  dom_id          FD_SW_DOM_ID

RETURN_VALUES
  If the old software is still executable the download handler shall return
  the domain header of the executable software - otherwise it shall return NULL

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (dom_id == FD_SW_DOM_ID)
  {
    _RESET_FD_DOMAIN;

    /* If the old field device software is still executable return its
       domain header - otherwise return NULL                                */

    memcpy (appl_fd_dom_hdr.manufacturer_id,      FD_DOM_MFG_ID, 6);
    memcpy (appl_fd_dom_hdr.device_family,        FD_DOM_FAMILY, 4);
    memcpy (appl_fd_dom_hdr.device_type,          FD_DOM_DEV_TYPE, 4);
    appl_fd_dom_hdr.device_revision             = FD_DOM_DEV_REV;
    appl_fd_dom_hdr.dd_revision                 = FD_DOM_DD_REV;
    memcpy (appl_fd_dom_hdr.software_revision,    FD_DOM_SW_REV, 8);
    memcpy (appl_fd_dom_hdr.software_name,        FD_DOM_SW_NAME, 8);

    return &appl_fd_dom_hdr;
  }
  else
  {
    _ASSERT(TRUE); /* MULTIDOMAIN Not Supported */

    return NULL;
  }
}




FUNCTION GLOBAL USIGN8 Appl_download_header
  (
    IN USIGN8               dom_id,
    IN T_DOMAIN_HDR_API *   p_dwnld_dom_hdr
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by the download server when the header of the
  downloaded software has been received.

  The download server cuts the software download domain header from the rest
  of the domain. The first segment provided to the download handler will not
  contain the domain header.

PARAMETERS
  dom_id            FD_SW_DOM_ID

  p_dwnld_dom_hdr   Pointer to the header of the domain to be downloaded

RETURN_VALUES
  E_OK            A domain with this header will be accepted
  E_SWDL_OTHER    A domain with this header will not be accepted

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8 result;

FUNCTION_BODY


  if (dom_id == FD_SW_DOM_ID)
  {
    result = _CHECK_FD_DOMAIN_HDR (p_dwnld_dom_hdr);

    if (result == E_OK)
    {
      result = _STORE_FD_DOMAIN_HDR (p_dwnld_dom_hdr);
    }
  }
  else
  {
    _ASSERT(TRUE); /* MULTIDOMAIN Not Supported */

    result = E_SWDL_OTHER;
  }

  return result;
}




FUNCTION GLOBAL USIGN8 Appl_download_segment
  (
    IN USIGN8           dom_id,
    IN USIGN8           segment_id,
    IN USIGN8           segment_len,
    IN USIGN8 *         p_segment
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by the download server when a segment of downloaded
  domain  has been received.

PARAMETERS
  dom_id          FD_SW_DOM_ID

  segment_id      SWDL_LOAD_FIRST_SEGM  The first segment of the domain has
                                        been received. The first segment does
                                        not contain the domain header. The
                                        domain header is indicated by an extra
                                        interface function.

                  SWDL_LOAD_SEGM        The received segment is neither the
                                        first nor the last segment of the
                                        domain.

                  SWDL_LOAD_LAST_SEGM   The last segment has been received.
                                        The download handler has to check
                                        the downloaded domain for consistency.

  segment_len     Length of the segment

  p_segment       Pointer to the segment

RETURN_VALUES
  E_OK                  The segment has been received correctly
  E_SWDL_CHECKSUM_FAIL  The checksum (of the complete domain) is not correct
  E_SWDL_OTHER          The segment or the whole domain is not correct

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8 result;

FUNCTION_BODY

  result = E_OK;

  if (dom_id == FD_SW_DOM_ID)
  {
    result = _CHECK_FD_DOMAIN_SEGM (segment_id,segment_len,p_segment);

    if (result == E_OK)
    {
      result = _STORE_FD_DOMAIN_SEGM (segment_id,segment_len,p_segment);
    }
  }
  else
  {
    _ASSERT(TRUE); /* MULTIDOMAIN Not Supported */

    result = E_SWDL_OTHER;
  }

  return result;
}




FUNCTION GLOBAL VOID Appl_activate_software
  (
    IN USIGN8           dom_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by the download server when the host has issued an
  activation command.

PARAMETERS
  dom_id          FD_SW_DOM_ID

RETURN_VALUES
  There is no 'positive' return value: each successful activation will result
  in a device restart.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  BOOL  activation_ok;

FUNCTION_BODY

  if (dom_id == FD_SW_DOM_ID)
  {
    activation_ok = _ACTIVATE_FD_FW;
  }
  else
  {
    _ASSERT(TRUE); /* MULTIDOMAIN Not Supported */

    activation_ok = FALSE;
  }

  if (activation_ok)
  {
    Reset_CPU ();
  }

  return; /* A return indicates a failed activation ! */
}



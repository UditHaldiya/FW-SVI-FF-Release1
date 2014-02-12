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

FILE_NAME          swdl_api.h



DESCRIPTION
  This file contains macros, definitions and declarations of functions
  that describe the interface between the download handler (application layer)
  and the download server (system management agent).

=========================================================================== */
#ifndef __SWDL_API_H__
#define __SWDL_API_H__


/* ------------------------------------------------------------------------ */
/*    Macros                                                                */
/* ------------------------------------------------------------------------ */

/* Software download domain identifiers ----------------------------------- */
  #define FD_SW_DOM_ID                          0u
  #define AP_SW_DOM_ID                          1u


/* Options for write response after download preparation ------------------ */
  #define NO_WRITE_RESP_FOR_DWNLD_PREPARE       0
  #define SEND_WRITE_RESP_FOR_DWNLD_PREPARE     1


/* Software download states ----------------------------------------------- */
  #define SWDL_DOM_NOT_READY_FOR_DWNLD          1u
  #define SWDL_DOM_PREPARING                    2u
  #define SWDL_DOM_READY_FOR_DWNLD              3u
  #define SWDL_DOM_COMPLETE                     4u
  #define SWDL_DOM_DOWNLOADING                  5u
  #define SWDL_DOM_ACTIVATING                   6u


/* Result of software download preparation and activation ----------------- */
  #define SWDL_PREPARE_OK                       1u
  #define SWDL_PREPARE_FAIL                     2u
  #define SWDL_ACTIVATE_OK                      3u
  #define SWDL_ACTIVATE_FAIL                    4u


/* Software download segment identifiers ---------------------------------- */
  #define SWDL_LOAD_FIRST_SEGM                  1u
  #define SWDL_LOAD_SEGM                        2u
  #define SWDL_LOAD_LAST_SEGM                   3u


/* Software download related error codes ---------------------------------- */
  #define E_SWDL_TEMP_DENIED                    1u
  #define E_SWDL_NOT_POSSIBLE                   2u
  #define E_SWDL_CHECKSUM_FAIL                  6u
  #define E_SWDL_OTHER                         10u
  #define E_SWDL_HW_ERR                        11u
  #define E_SWDL_NO_NVRAM_ACCESS              128u


/* Identifier to invalidate a domain header ------------------------------- */
  #define SWDL_NO_MANUFAC_ID                "      "


/* ------------------------------------------------------------------------ */
/*    Type Definitions                                                      */
/* ------------------------------------------------------------------------ */
  //lint -e659 nothing follows '}' in typedef
  typedef struct
  {
    USIGN8    resp_for_prepare;           /* NO_WRITE_RESP_FOR_DWNLD_PREPARE or
                                             SEND_WRITE_RESP_FOR_DWNLD_PREPARE  */
    USIGN16   max_time_for_preparation;   /* unit: seconds */
    USIGN16   max_time_for_activation;    /* unit: seconds */
  }
    T_DWNLD_PROPERTY_API;


  typedef struct
  {
    STRINGV   multidom_mfg_id [6];        /* As specified in FF-883 */
    STRINGV   multidom_family [4];        /* As specified in FF-883 */
    STRINGV   instance [8];               /* As specified in FF-883 */
    USIGN16   hardware_revision;          /* As specified in FF-883 */
    USIGN16   software_revision;          /* As specified in FF-883 */
  }
    T_MULTIDOM_DESCR_API;


  typedef struct
  {
    STRINGV   dom_name [8];               /* As specified in FF-883 */
  }
    T_DOMAIN_DESCR_API;

  typedef struct
  {
    STRINGV   manufacturer_id [6];        /* As specified in FF-883 */
    STRINGV   device_family [4];          /* As specified in FF-883 */
    STRINGV   device_type [4];            /* As specified in FF-883 */
    USIGN8    device_revision;            /* As specified in FF-883 */
    USIGN8    dd_revision;                /* As specified in FF-883 */
    STRINGV   software_revision [8];      /* As specified in FF-883 */
    STRINGV   software_name [8];          /* As specified in FF-883 */
  }
    T_DOMAIN_HDR_API;


  typedef struct
  {
    T_DWNLD_PROPERTY_API *  p_property;
    T_MULTIDOM_DESCR_API *  p_fd_multidom_descr;
    T_MULTIDOM_DESCR_API *  p_ap_multidom_descr;
    T_DOMAIN_DESCR_API *    p_fd_dom_descr;
    T_DOMAIN_DESCR_API *    p_ap_dom_descr;
    T_DOMAIN_HDR_API *      p_fd_dom_hdr;
    T_DOMAIN_HDR_API *      p_ap_dom_hdr;
  }
    T_DWNLD_CFG_API;
  //lint +e659


  typedef struct
  {
    USIGN8  fd_dwnld_state;       /* Download state of the field device software */
    USIGN8  ap_dwnld_state;       /* Download state of the application software */

  } T_DWNLD_STATE_API;





/* ------------------------------------------------------------------------ */
/*    Interface Functions provided by the Download Handler                  */
/* ------------------------------------------------------------------------ */

/* The following structure contains a set a function pointers. The download
  handler has to implement this set of function. During the device startup
  phase the download  handler has to provide a structure with the function
  pointers to the download server.                                         */

  typedef struct _T_SMDL_CALL_FDEV_FUNCT
  {
    GLOBAL GLOBAL USIGN8 (*a_of_prepare_download)
      (
        IN USIGN8               dom_id
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
                AP_SW_DOM_ID  (for multi-domain download only)

    RETURN_VALUES
      E_OK                  The preparation command is accepted
      E_SWDL_TEMP_DENIED    Software download is not possible at the moment
      E_SWDL_NOT_POSSIBLE   Software download is not possible for this device

    ----------------------------------------------------------------------------*/
    ;


    GLOBAL USIGN8 (*a_of_download_header)
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
                        AP_SW_DOM_ID  (for multi-domain download only)

      p_dwnld_dom_hdr   Pointer to the header of the domain to be downloaded

    RETURN_VALUES
      E_OK            A domain with this header will be accepted
      E_SWDL_OTHER    A domain with this header will not be accepted

    ----------------------------------------------------------------------------*/
    ;


    GLOBAL USIGN8 (*a_of_download_segment)
      (
        IN USIGN8               dom_id,
        IN USIGN8               segment_id,
        IN USIGN8               segment_len,
        IN USIGN8 *             p_segment
      )

    /*----------------------------------------------------------------------------
    FUNCTIONAL_DESCRIPTION
      This function is called by the download server when a segment of downloaded
      domain  has been received.

    PARAMETERS
      dom_id          FD_SW_DOM_ID
                      AP_SW_DOM_ID  (for multi-domain download only)

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
    ;



    GLOBAL T_DOMAIN_HDR_API * (*a_of_cancel_download)
      (
        IN USIGN8               dom_id
      )

    /*----------------------------------------------------------------------------
    FUNCTIONAL_DESCRIPTION
      This function is called by the download server when the download procedure is
      aborted. The abort can be triggered by the host by sending a cancel command;
      the procedure is also aborted when the underlying connection has been closed.

    PARAMETERS
      dom_id          FD_SW_DOM_ID
                      AP_SW_DOM_ID  (for multi-domain download only)

    RETURN_VALUES
      If the old software is still executable the download handler shall return
      the domain header of the executable software - otherwise it shall return NULL

    ----------------------------------------------------------------------------*/
    ;


    GLOBAL VOID (*a_of_activate_software)
      (
        IN USIGN8               dom_id
      )

    /*----------------------------------------------------------------------------
    FUNCTIONAL_DESCRIPTION
      This function is called by the download server when the host has issued an
      activation command.

    PARAMETERS
      dom_id          FD_SW_DOM_ID
                      AP_SW_DOM_ID  (for multi-domain download only)

    RETURN_VALUES
      There is no 'positive' return value: each successful activation will result
      in a device restart.

    ----------------------------------------------------------------------------*/
    ;

  } T_SMDL_CALL_FDEV_FUNCT;


#include "da_fmg.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT extern T_SMDL_CALL_FDEV_FUNCT  smdl_call_fdev_funct;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */




/* ------------------------------------------------------------------------ */
/*    Interface Functions provided by the Download Server                   */
/* ------------------------------------------------------------------------ */

  FUNCTION GLOBAL USIGN16 fdc_get_dwnld_state
    (
      OUT T_DWNLD_STATE_API *   p_dwnld_state
    )

  /*----------------------------------------------------------------------------
    By means of this function the download handler asks the download server for
    the state of the download process.

    Typically this function is called in the device startup phase. In this case
    this function provides the state of the download process when the device was
    power-cycled. However, the download handler is allowed to call this function
    at any time it needs information about the current download state.

  PARAMETERS
    p_dwnld_state     Pointer to structure containing the current download state

    The possible download states are

    SWDL_DOM_NOT_READY_FOR_DWNLD    No download in progress
    SWDL_DOM_PREPARING              Download is in preparation phase
    SWDL_DOM_READY_FOR_DWNLD        Preparation phase has been finished
                                    successfully - the download hasn't been
                                    started yet.
    SWDL_DOM_DOWNLOADING            The host is downloading a domain
    SWDL_DOM_COMPLETE               The download has been finished but the
                                    new software  isn't activated yet.
    SWDL_DOM_ACTIVATING             The download handler is activating the
                                    new software

  RETURN_VALUES
    E_OK                     The download server has access to its non-volatile
                             data base
    E_SWDL_NO_NVRAM_ACCESS   The download server has no access to its non-volatile
                             data base. Fatal error!

  -----------------------------------------------------------------------------*/
  ;


  FUNCTION GLOBAL USIGN16 fdc_config_dwnld_server
    (
      IN  T_DWNLD_CFG_API *     p_dwnld_cfg
    )

  /*----------------------------------------------------------------------------
  FUNCTIONAL_DESCRIPTION
    In the device startup phase the download handler has to provide configuration
    data to the download server.

    The set of configuration data is different for single and multi-domain
    download. The single domain download only requires the download property data,
    the domain name and the header of the active domain.

  PARAMETERS
    p_dwnld_cfg     Pointer to the configuration data

  RETURN_VALUES
    E_OK                     The download server has access to its non-volatile
                             data base
    E_SWDL_NO_NVRAM_ACCESS   The download server has no access to its non-volatile
                             data base. Fatal error!

  -----------------------------------------------------------------------------*/
  ;


  FUNCTION GLOBAL VOID fdc_complete_dwnld_action
    (
      IN USIGN8               dom_id,
      IN USIGN8               complete_action,
      IN T_DOMAIN_HDR_API *   p_dwnld_dom_hdr
    )

  /*----------------------------------------------------------------------------
    If in the device startup phase the download handler detects that it is in
    the download preparation or domain activation phase it has to complete this
    action. After completion the download handler has to report the result to the
    download server by calling this function.

  PARAMETERS
    dom_id              FD_SW_DOM_ID
                        AP_SW_DOM_ID    (for multi-domain download only)

    complete_action     SWDL_PREPARE_OK
                        SWDL_PREPARE_FAIL
                        SWDL_ACTIVATE_OK
                        SWDL_ACTIVATE_FAIL

    p_dwnld_dom_hdr     Pointer to the domain header

    It depends on the result of download preparation or domain activation whether
    or not a domain header is available: after a successful preparation no domain
    header is available; after a successful activation the domain header is
    available. In case of an unsuccessful preparation or activation it depends on
    the implementation of the download handler whether it is able restore the old
    domain. A class 1 download handler can expected to restore the header (as in
    a class 1 device it is a must that a executable software a available) whereas
    for a class 3 download handler it can not be expected to restore the old
    domain.

  RETURN_VALUES
    None

  -----------------------------------------------------------------------------*/
  ;



#endif /* __SWDL_API_H__ */




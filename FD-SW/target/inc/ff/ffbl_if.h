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

FILE_NAME          ffbl_if.h



DESCRIPTION
  This file contains declarations of FFBL functions that are called by the
  FB Shell (FBS) or the Field Device Communication stack (FDC) resp.

=========================================================================== */
#ifndef __FFBL_IF_H__
#define __FFBL_IF_H__

#include "fbif.h"

/* ------------------------------------------------------------------------ */
/*   GLOBAL_DEFINES                                                         */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* EXPORT_TYPEDEFS                                                          */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* FUNCTION_DECLARATIONS                                                    */
/* ------------------------------------------------------------------------ */

FUNCTION GLOBAL VOID ffbl_init (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by FB shell in the initialization phase. The
  initialization phase is the very first phase of the device startup phase.

  This function is used to initialize data of the FF block layer (FFBL)

PARAMETERS
  None

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL VOID ffbl_provide_resb_data_to_fbs
  (
    OUT USIGN8 * *   pp_max_notify,
    OUT USIGN8 * *   pp_lim_notify,
    OUT USIGN32 * *  pp_confirm_time,
    OUT USIGN16 * *  pp_feature_sel
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by FB shell in the device startup phase.

  It provides pointers to some resource block parameters.

PARAMETERS
  pp_max_notify     Pointer to pointer to the parameter MAX_NOTIFY
  pp_lim_notify     Pointer to pointer to the parameter LIM_NOTIFY
  pp_confirm_time   Pointer to pointer to the parameter CONFIRM_TIME
  pp_feature_sel    Pointer to pointer to the parameter FEATURE_SEL

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL VOID ffbl_provide_resb_data_to_fdc
  (
    OUT USIGN32 *   p_manufac_id,
    OUT USIGN16 *   p_dev_type,
    OUT USIGN32 *   p_min_cycle_t
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by Field Device Communication stack in the device
  startup phase.

  It provides some resource block parameters.

PARAMETERS
  p_manufac_id      Pointer to the parameter MAX_NOTIFY
  p_dev_type        Pointer to the parameter DEV_TYPE
  p_min_cycle_t     Pointer to the parameter MIN_CYCLE_T

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL USIGN16 ffbl_check_write_lock
  (
    IN USIGN16 index
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by FB shell when a parameter has to be written.

PARAMETERS
  index             Index of the parameter to be written

RETURN_VALUES
  E_OK              Parameter is writeable
  E_FB_WRITE_LOCK   Parameter is not writeable
----------------------------------------------------------------------------*/
;


#endif /* __FFBL_IF_H__ */




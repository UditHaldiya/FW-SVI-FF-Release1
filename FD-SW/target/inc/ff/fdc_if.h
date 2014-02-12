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

FILE_NAME          fdc_if.h



DESCRIPTION


=========================================================================== */
#ifndef __FDC_IF_H__
#define __FDC_IF_H__


/* ------------------------------------------------------------------------ */
/*   GLOBAL_DEFINES                                                         */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
   EXPORT_TYPEDEFS
/* ------------------------------------------------------------------------ */

  typedef GLOBAL void (* PF_SMT2_TIMEOUT)        (VOID);
  typedef GLOBAL void (* PF_FBS_TIMEOUT)         (VOID);
  typedef GLOBAL void (* PF_UPPER_TESTER_AGENT)  (VOID);


/* ------------------------------------------------------------------------ */
   FUNCTION_DECLARATIONS
/* ------------------------------------------------------------------------ */
FUNCTION GLOBAL VOID fdc_start_smt2
  (
    IN USIGN32          smt2_time,
    IN PF_SMT2_TIMEOUT  pf_smt2_timeout
  );


FUNCTION GLOBAL VOID fdc_stop_smt2 (VOID);


FUNCTION GLOBAL VOID fdc_start_fbs_timer
  (
    IN USIGN32          confirm_time,
    IN PF_SMT2_TIMEOUT  pf_fbs_timeout
  );


FUNCTION GLOBAL VOID fdc_stop_fbs_timer (VOID);


FUNCTION GLOBAL USIGN8 fdc_get_eep_blk_dll_base_cfg (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN8 fdc_get_eep_blk_vcr_cfg (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN8 fdc_get_eep_blk_sm_cfg (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN8 fdc_get_eep_blk_fb_start_cfg (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN8 fdc_get_eep_blk_lm_info_cfg (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN8 fdc_get_eep_blk_lm_data_cfg (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN8 fdc_get_eep_blk_las_cfg (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


----------------------------------------------------------------------------*/
;


#ifdef SW_DOWNLOAD /* ----------------------------------------------------- */
FUNCTION GLOBAL USIGN8 fdc_get_blk_swdl_cfg (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


----------------------------------------------------------------------------*/
;


FUNCTION PUBLIC USIGN16 fdc_get_swdl_cfg
  (
    IN VOID * *   p_nv_swdl
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


----------------------------------------------------------------------------*/
;
#endif /* SW_DOWNLOAD */


EXPORT_DATA

#include "da_fdc.h"
NO_INIT extern USIGN8 fdc_fb_start_timer_id;                  /* export from fdc.c */
NO_INIT extern USIGN32 fdc_bgd_timer;                         /* export from fdc_bgd.c */
NO_INIT extern PF_UPPER_TESTER_AGENT   pf_upper_tester_agent;
#include "da_def.h"

#endif /* __FDC_IF_H__ */




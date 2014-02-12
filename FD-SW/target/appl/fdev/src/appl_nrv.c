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

FILE_NAME          appl_nrv.c



FUNCTIONAL_MODULE_DESCRIPTION
  This module contains functions for a background scan of the NV-RAM.

  The background scan of the NV-RAM is an optional feature. It is enabled
  by the macro ENABLE_NV_RAM_VERIFY.

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FAPP + MOD_APPL_NRV)

INCLUDES
  #include "base.h"
  #include "except.h"
  #include "osif.h"
  #include "eep_if.h"
  #include "fbs_api.h"
  #include "ffbl_api.h"
  #include "appl_int.h"

LOCAL_DEFINES
  #define NV_RAM_VERIFY_CYCLE_TIME  50            /* Cyclic verification time 5s unit [100ms] */

LOCAL_TYPEDEFS

  typedef struct
  {
    USIGN32   fail_cnt[EEP_NBR_OF_BLOCKS];
    USIGN32   ok_cnt[EEP_NBR_OF_BLOCKS];
    USIGN32   chg_cnt[EEP_NBR_OF_BLOCKS];
    USIGN32   read_cnt[2];
    USIGN8    blk_cnt[EEP_NBR_OF_BLOCKS];
  }
    T_EEP_ERR;









USIGN16 Appl_verify_nv_ram (void)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by the background task routine on reception of a
  timer event.

  It performs a background scan of the non-volatile memory and, in case of a
  problem, notifies the communication task to report a resource block error.

  This function is an example only. It may be modified and adapted to
  device-specific requirements.

PARAMETERS
  None

RETURN_VALUES
  Returns the verification cycle time in 100ms unit. The background task
  stores the return value when the function returns for the first time.
  Afterwards, the background task calls this function every n-times * 100ms.

  The maximum value is 65535 (then the timer interval is 65535 * 100ms). A
  return value of 0 requests the background task to stop calling this function
----------------------------------------------------------------------------*/
{
#if (NV_RAM_VERIFY_CYCLE_TIME != 0)
    if(eeptest_GetStats(NULL, NULL, NULL) != 0)
    {
        ffbl_generate_RESB_alarm (ACTIVATE_BLOCK_ALARM, res_err_dev_needs_maint_now);
    }
#endif
    return NV_RAM_VERIFY_CYCLE_TIME;
}

/** \brief a mandatory initializer for Softing framework
*/
void appl_nrv_init(void)
{
    fbs_call_fdev_funct.a_of_verify_nv_ram      = Appl_verify_nv_ram;
}




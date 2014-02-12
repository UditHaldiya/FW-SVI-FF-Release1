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

FILE_NAME          hm_api.h


DESCRIPTION

=========================================================================== */

#ifndef __HM_API_H__
#define __HM_API_H__

GLOBAL_DEFINES

/* --- HART communication state defines --- */

#define HART_COMM_OK                0x00
#define DATA_INIT_FINISHED          0x01       /* only at startup */
#define CONFIG_UPDATE_FINISHED      0x02
#define NO_HART_COMM                0xFF

#define SEND                        0x00
#define RECEIVE                     0xFF

EXPORT_TYPEDEFS

typedef struct _HART_DB
{
  const USIGN8 *    cyc_cmd;                    /* List of high prior (cyclic) commands */
  const USIGN8 *    regular_cmd;                /* List of low prior (regular) commands */
  USIGN8            cyc_cmd_list_length;        /* Length of cyc_cmd list     */
  USIGN8            regular_cmd_list_length;    /* Length of regular_cmd list */
  USIGN16           cycl_min_period;            /* Min period for high prior (cyclic) traffic in msec units  */
  USIGN32           startup_timeout;            /* Max time the HART master is waiting for the
                                                   HART device before declaring it as not available
                                                   - in msec units                              */
  BOOL              keep_regular;               /* Keep regular traffic active after init phase */
} HART_DB;

typedef enum _T_HM_ACT
{
  DEACTIVATE,
  UNCHANGED,
  ACTIVATE
} T_HM_ACT;


FUNCTION_DECLARATIONS

/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Application Functions Called by the HART Master                       */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

  typedef struct
  {
    GLOBAL VOID      (* a_of_handle_HART_cmd)   (USIGN8, USIGN8, USIGN8 *, USIGN8 *, USIGN8 *, USIGN16);
    GLOBAL HART_DB * (* a_of_get_HART_database) (USIGN8, USIGN32);
    GLOBAL VOID      (* a_of_set_HART_status)   (USIGN8);
    GLOBAL VOID      (* a_of_set_dev_id)        (USIGN32);
    #if defined (PROTOCOL_FF)
    GLOBAL BOOL     (* a_of_check_restart_flag) (VOID);
    GLOBAL VOID     (* a_of_reset_restart_flag) (VOID);
    #endif /* PROTOCOL_FF */
  }
    T_HM_CALL_APPL_FUNCT;



/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    HART Master Functions to be called by the Application                  */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

FUNCTION GLOBAL USIGN16 HART_acyc_cmd
  (
    IN  USIGN8      cmd,
    IN  USIGN8 *    send_buff,
    IN  USIGN8      send_length,
    OUT USIGN8 *    receive_buff
  );


FUNCTION GLOBAL VOID HART_set_active
  (
    IN T_HM_ACT     cyc_active,
    IN T_HM_ACT     config_update_active
  );


FUNCTION GLOBAL USIGN32 HART_get_dev_id (VOID);


/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Global Variables                                                      */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

#include "da_hm.h"
  NO_INIT extern T_HM_CALL_APPL_FUNCT   hm_call_appl_funct;
#include "da_def.h"

#endif /* __HM_API_H__ */

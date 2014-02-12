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

FILE_NAME          hm_if.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_HM + MOD_HM_IF)




INCLUDES

  #include "base.h"
  #include "osif.h"
  #include "except.h"

  #include "hw_cfg.h"
  #include "hm_api.h"
  #include "hm_if.h"
  #include "hm_cfg.h"
  #include "hm.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

 LOCAL VOID set_cmd_in_acyc_list (USIGN8 cmd, USIGN8 * send_buff, USIGN8 send_length, USIGN8 * receive_buff);

IMPORT_DATA
#include "da_hm.h"

  NO_INIT extern HART_CMD_WAIT HM_acyc;

EXPORT_DATA

LOCAL_DATA
  NO_INIT STATIC BOOL Atomic_ HART_connected;
#include "da_def.h"


//lint -estring(457,hm_get_acyc_result)
MN_INLINE u8 hm_get_acyc_result(void)
{
    return HM_acyc_result;
}

/****************************************************************************/

FUNCTION GLOBAL USIGN16 HART_acyc_cmd
  (
    IN  USIGN8     cmd,
    IN  USIGN8   * send_buff,
    IN  USIGN8     send_length,
    OUT USIGN8   * receive_buff
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  The function sends an acyclic command to the HART communication. Note,
  that the function will blocks until the command is executed by the HART
  slave.

PARAMETERS

  cmd               Command to execute.

  send_buff         Pointer to the send buffer.

  send_length       Length of the data in the send buffer.

  receive_buff      Pointer to the receive buffer.

RETURN_VALUES

  HART_OK           No error.
  HART_ERROR        Error.
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES



FUNCTION_BODY
  if ( HART_connected == FALSE )
  {
    hm_connect (osif_get_current_task_id ());
    HART_connected = TRUE;
  }

    /* Indicate new requwst to HM-Task */
    /*---------------------------------*/
  set_cmd_in_acyc_list(cmd, send_buff, send_length, receive_buff);
  osif_set_event(HM_TASK_ID, OS_IF_EVENT_REQRES);

    /* Wait until request is completed */
    /*---------------------------------*/
  osif_wait_event(osif_get_current_task_id (), HART_ACY_CMD_END_EVENT);

  return ((USIGN16) hm_get_acyc_result());

} /* FUNCTION HART_acyc_cmd */



FUNCTION GLOBAL VOID hm_init (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  The function intializes the HART interface. The function is called by
  the main-function in osif_mn.c.

PARAMETERS

  None

RETURN_VALUES

  None
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES


FUNCTION_BODY

    /* Create task -------------------------------------------------------- */
    osif_create_task (HM_TASK_ID,
                      HM_TASK_START_PRIO,   /* Run with higher prio during startup phase */
                      task_hm_base_routine,
                      (HART_START_EVENT  | HART_SEND_END_EVENT | HART_RECEIVE_END_EVENT | HART_ERROR_EVENT |
                       OS_IF_EVENT_TIMER | OS_IF_EVENT_REQRES  |OS_IF_EVENT_EEPROM)
                     );

    /* Init HART master using slave adress 0 ------------------------------ */
    MN_ENTER_CRITICAL();
        HART_Init (0);

        HART_connected = FALSE;
    MN_EXIT_CRITICAL();

    return;

} /* FUNCTION hm_init */




FUNCTION LOCAL VOID set_cmd_in_acyc_list
  (
    IN  USIGN8     cmd,
    IN  USIGN8   * send_buff,
    IN  USIGN8     send_length,
    OUT USIGN8   * receive_buff
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  The function is used to fill the HM_acyc - structure. The function is called
  by function HART_acyc_cmd().

PARAMETERS

  cmd               Command to execute.

  send_buff         Pointer to the send buffer.

  send_length       Length of the data in the send buffer.

  receive_buff      Pointer to the receive buffer.

RETURN_VALUES

  None
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    osif_disable_all_tasks();
    HM_acyc.send_buff = send_buff;
    HM_acyc.send_length = send_length;
    HM_acyc.receive_buff = receive_buff;
    HM_acyc.cmd = cmd;
    osif_enable_all_tasks();

} /* FUNCTION set_cmd_in_acyc_list */

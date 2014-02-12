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

FILE_NAME          hm.h


DESCRIPTION       Error codes, status and etc. used by hart interface

=========================================================================== */

#ifndef __HM_H__
#define __HM_H__


GLOBAL_DEFINES

/*--- HART interace error codes ---*/

#define HART_OK                           0
#define HART_ERROR                        1

/*--- HART interace status ---*/
#define HART_STATE_BLOCKING               0xFFFF

#define HART_Check_Done(command)                            \
                  ((command)->state == ASYNC_COMMAND_FAILED \
                          ? HART_ERROR :                    \
                   (command)->state == ASYNC_COMMAND_DONE   \
                          ? HART_OK : HART_STATE_BLOCKING)

EXPORT_TYPEDEFS

typedef struct _HART_CMD_WAIT
{
  USIGN8            cmd;
  USIGN8            result;
  USIGN8 *          send_buff;
  USIGN8            send_length;
  USIGN8 *          receive_buff;

} HART_CMD_WAIT;

extern u8 HM_acyc_result; //move out of HART_CMD_WAIT for thread analysis

enum T_ASYNC_COMMAND_STATE {
  ASYNC_COMMAND_IDLE,
  ASYNC_COMMAND_EXECUTE,
  ASYNC_COMMAND_DONE,
  ASYNC_COMMAND_FAILED
};

typedef struct _T_ASYNC_COMMAND{
  enum T_ASYNC_COMMAND_STATE state;       /* dont touch this field! */
  USIGN8                     cmd;
  USIGN16                    prio;
  USIGN16                    in_len;
  BYTE                       *in_buf;
  USIGN16                    out_len;
  BYTE                       *out_buf;
//  struct _T_ASYNC_COMMAND    *next;       /* dont touch this field! */
} T_ASYNC_COMMAND;

typedef VOID (T_BURST_FUNCTION)(USIGN8 cmd, USIGN16 len, BYTE *buf);

FUNCTION_DECLARATIONS

FUNCTION PUBLIC USIGN16 HART_Init(USIGN8 short_address /*, USIGN8 *device_type, USIGN32 *device_id*/);
FUNCTION PUBLIC USIGN16 HART_send_cmd0 (USIGN8 * buffer, USIGN8 * buf_size);
FUNCTION PUBLIC VOID HART_Exit(VOID);
FUNCTION PUBLIC VOID HART_SetBurstFunction(T_BURST_FUNCTION *burst_function);
FUNCTION PUBLIC VOID HART_InitAsyncCommand(volatile T_ASYNC_COMMAND *command);
FUNCTION PUBLIC USIGN16 HART_Command_non_block(volatile T_ASYNC_COMMAND *command);
FUNCTION PUBLIC VOID hm_connect (USIGN8 task_id);

#endif /* __HM_H__ */


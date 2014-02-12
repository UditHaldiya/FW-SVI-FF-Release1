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

FILE_NAME          osif_cfg.h



FUNCTIONAL_MODULE_DESCRIPTION

Containes type declarations and definitions to be used for configuration
of the OSIF task system

=========================================================================== */
#ifndef __OSIF_CFG__
#define __OSIF_CFG__

#include "keywords.h"   /* keyword defines */

INCLUDES

GLOBAL_DEFINES
/* defines for function return values */
#define MAX_NUM_MSG     200
#define MB_NUM          (NUM_EVENTS * MAX_TASK_NUM)

LOCAL_DEFINES

EXPORT_TYPEDEFS

typedef struct _task_cfg {
    USIGN8          id;                     /* Task buffer id            */
    USIGN8          prio;                   /* Task priority             */
    USIGN8          num_msg;                /* number of messages per event */
    T_EVENT         valid_events;           /* valid events for task     */
    T_EVENT         msg_events;             /* events with assigned msg. */
    int             stacksize;
    void NEAR_D *    stack;
    VOID (FAR_C *   main) (VOID);           /* Task main routine         */
    const char *    name;
} TASK_CFG;

IMPORT_DATA

EXPORT_DATA

#endif /* __OSIF_CFG__ */


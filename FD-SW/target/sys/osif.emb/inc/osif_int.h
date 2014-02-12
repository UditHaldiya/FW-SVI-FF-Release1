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

FILE_NAME          osif_int.h



FUNCTIONAL_MODULE_DESCRIPTION

Module description

=========================================================================== */
#include "keywords.h"   /* keyword defines */

INCLUDES
#include "osif.h"

GLOBAL_DEFINES
#define CHECK_TASK_ID(task_id, location)    \
                {                                                           \
                    if ( task_id == CURRENT_TASK_ID )                       \
                    {                                                       \
                        task_id = osif_get_current_task_id ();              \
                    }                                                       \
                    if ( (task_id > MAX_TASK_ID) && (task_id < VIRTUAL_TASK_ID) )  \
                    {                                                       \
                        _OS_EXCEPTION(ERR_INV_TASK, task_id, location, 0);  \
                    }                                                       \
                }


LOCAL_DEFINES
#ifndef VAR_EXTERN
#define VAR_EXTERN  extern
#endif /* VAR_EXTERN */

EXPORT_TYPEDEFS
typedef VOID (FAR_C * T_TIMER_HOOK) (VOID);

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

EXPORT_DATA

#include "da_osif.h"                             /* DATA SEGMENT DEFINITION */

NO_INIT VAR_EXTERN BOOL Sema_init;
NO_INIT VAR_EXTERN BOOL Tim_init;

#include "da_def.h"                              /* DEFAULT DATA SEGMENT */


#include "cs_osif.h"                             /* CONSTANT SEGMENT DEFINITION */

extern const TASK_CFG Task_cfg[MAX_TASK_NUM];

#include "cs_def.h"                              /* DEFAULT COSTANT SEGMENT */

IMPORT_DATA

LOCAL_DATA


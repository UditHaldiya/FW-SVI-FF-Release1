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

FILE_NAME          osif_sem.c



FUNCTIONAL_MODULE_DESCRIPTION

This modul contains semaphore handling functions.

=========================================================================== */
#include "keywords.h"
#define  MODULE_ID      (COMP_OSIF + MOD_OSIFSEM)

INCLUDES
#include "base.h"

#include "osif.h"
#include "osif_cfg.h"
#include "osif_int.h"

#include "except.h"


FUNCTION_DECLARATIONS


GLOBAL_DEFINES


LOCAL_DEFINES
#define MAX_SEMA_NUM        5
#ifdef _DEBUG
#define SEMA_VALID_VALUE    0xA5
#define CHECK_SEMA_VALID(sema, task_id, location)      \
                {                                                               \
                    if ( sema->valid_flag != SEMA_VALID_VALUE )                 \
                    {                                                           \
                        _OS_EXCEPTION(ERR_SEMA_INV, task_id, location, sema);   \
                    }                                                           \
                }
#else /* _DEBUG */
#define CHECK_SEMA_VALID(sema, task_id, location)
#endif /* _DEBUG */


EXPORT_TYPEDEFS


LOCAL_TYPEDEFS

typedef struct _T_OS_SEMA {
    USIGN8              id;
#ifdef _DEBUG
    USIGN8              valid_flag;
#endif /* _DEBUG */
    OS_RSEMA            rtos_sema;
    struct _T_OS_SEMA * prev;
    struct _T_OS_SEMA * next;
} T_OS_SEMA;


EXPORT_DATA


IMPORT_DATA


LOCAL_DATA

#include "da_osif.h"                              /* DATA SEGMENT DEFINITION */

NO_INIT T_OS_SEMA    Sema_pool[MAX_SEMA_NUM+1];
NO_INIT T_OS_SEMA  * Sema_free;
NO_INIT T_OS_SEMA  * Sema_used;

#include "da_def.h"                               /* DEFAULT DATA SEGMENT */

/*---------------------------------------------------------------------------*/
/* Semaphore initialisation                                                  */
/*---------------------------------------------------------------------------*/

FUNCTION GLOBAL VOID osif_init_sema (VOID)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to initialize the OS Interface.

possible-return-codes:
- none
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
        int i;

FUNCTION_BODY


/* --- init semaphore management ------------------------------------------- */
        memset (Sema_pool, 0, sizeof (Sema_pool));

        for (i=0; i<MAX_SEMA_NUM-1; i++) {
            Sema_pool[i].next = &Sema_pool[i+1];
            Sema_pool[i+1].prev = &Sema_pool[i];
        }
        Sema_pool[0].prev = NULL;
        Sema_pool[i].next = NULL;
        Sema_free = &Sema_pool[i];
        Sema_used = NULL;

        Sema_init = TRUE;

        return;
}


/*---------------------------------------------------------------------------*/
/* Semaphore management                                                      */
/*---------------------------------------------------------------------------*/

FUNCTION GLOBAL USIGN8 osif_create_sema
    (
        IN  USIGN8 task_id
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Creates a resource semaphore. The resource is not blocked after creation.

possible return values:
- pointer to semaphore structure

-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
        T_OS_SEMA * sema;
        USIGN8 sema_idx;

FUNCTION_BODY

    osif_disable_all_tasks ();

    if ( Sema_init == FALSE )
        osif_init_sema ();

    CHECK_TASK_ID(task_id, FKT_CREATE_SEMA);

    if ( Sema_free == NULL )
    {
        _OS_EXCEPTION(ERR_SEMA_OV, task_id, FKT_CREATE_SEMA, 0);
    }

    if ( (Sema_free->next != NULL) || ((Sema_used != NULL) && (Sema_used->next != NULL)) )
    {
        _OS_EXCEPTION(ERR_SEMA_INCONS, task_id, FKT_CREATE_SEMA, 0);
    }

    sema = Sema_free;
    Sema_free = sema->prev;
    if ( Sema_free != NULL )
        Sema_free->next = NULL;

    if ( Sema_used != NULL ) {
        Sema_used->next = sema;
    }
    sema->prev = Sema_used;
    Sema_used = sema;

    sema->id = 0;
#ifdef _DEBUG
    sema->valid_flag = SEMA_VALID_VALUE;
#endif /* _DEBUG */
    OS_CREATERSEMA (&sema->rtos_sema);

    osif_enable_all_tasks ();

    sema_idx = (USIGN8) (sema - Sema_pool);
    return (sema_idx);
}


FUNCTION GLOBAL VOID osif_delete_sema
    (
        IN  USIGN8 task_id,
        IN  USIGN8 sema_id
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Deletes a resource semaphore.

possible return values:
- none
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    T_OS_SEMA * sema;

FUNCTION_BODY

    sema = Sema_pool + sema_id;

    osif_disable_all_tasks ();

    CHECK_TASK_ID(task_id, FKT_DELETE_SEMA);
    CHECK_SEMA_VALID(sema, task_id, FKT_DELETE_SEMA);

#ifdef _DEBUG
    if ( (Sema_used->next != NULL) || ((Sema_free != NULL) && (Sema_free->next != NULL)) )
    {
        _OS_EXCEPTION(ERR_SEMA_INCONS, task_id, FKT_DELETE_SEMA, 0);
    }
#endif /* _DEBUG */

        /* release semaphore, if in use */
        /*------------------------------*/
    while (OS_GetUseCnt (&sema->rtos_sema) != 0) {
        OS_Unuse (&sema->rtos_sema);
    } /* end while */

    if ( sema->next == NULL ) {             /* last element in chain        */

        Sema_used = sema->prev;
        if ( Sema_used != NULL )
            Sema_used->next = NULL;

    }
    else {                                  /* somewhere within chain       */
        if ( sema->prev != NULL ) {
            (sema->prev)->next = sema->next;
        }
        (sema->next)->prev = sema->prev;
    }

    if ( Sema_free != NULL )
        Sema_free->next = (T_OS_SEMA *) sema;
    sema->prev = Sema_free;
    sema->next = NULL;
    Sema_free = (T_OS_SEMA *) sema;

#ifdef _DEBUG
    sema->valid_flag = 0;
#endif /* _DEBUG */

    osif_enable_all_tasks ();

    return;
}


/*---------------------------------------------------------------------------*/
/* Semaphore handling                                                        */
/*---------------------------------------------------------------------------*/

FUNCTION GLOBAL VOID osif_use_sema
    (
        IN  USIGN8 task_id,
        IN  USIGN8 sema_id
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Claimes a resource semaphore. If the resource is already blocked by another task,
the calling task is suspended.

possible return values:
- none
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    T_OS_SEMA * sema;

FUNCTION_BODY

    sema = Sema_pool + sema_id;

    CHECK_TASK_ID(task_id, FKT_USE_SEMA);
    CHECK_SEMA_VALID(sema, task_id, FKT_USE_SEMA);

    OS_Use (&sema->rtos_sema);
    sema->id = task_id;

    return;
}


FUNCTION GLOBAL USIGN8 osif_request_sema
    (
        IN  USIGN8 task_id,
        IN  USIGN8 sema_id
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Requests to claim a resource semaphore. If the semaphore is yet free, the
function claims it. This call is non-blocking, i.e. the task continues execution
in any case.

possible return values:
0:      Resource was not available
1:      Resource was available, is now blocked by calling task
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    T_OS_SEMA * sema;
    USIGN8 result;
FUNCTION_BODY

    sema = Sema_pool + sema_id;

    CHECK_TASK_ID(task_id, FKT_REQ_SEMA);
    CHECK_SEMA_VALID(sema, task_id, FKT_REQ_SEMA);

    result = (USIGN8) OS_Request (&sema->rtos_sema);
    if ( result != 0 )
        sema->id = task_id;

    return (result);
}


FUNCTION GLOBAL VOID osif_release_sema
    (
        IN  USIGN8 task_id,
        IN  USIGN8 sema_id
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Releases a resource semaphore previously used by the calling task.

possible return values:
- none
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    T_OS_SEMA * sema;

FUNCTION_BODY

    sema = Sema_pool + sema_id;

    CHECK_TASK_ID(task_id, FKT_REL_SEMA);
    CHECK_SEMA_VALID(sema, task_id, FKT_REL_SEMA);

    OS_Unuse (&sema->rtos_sema);

    return;
}


FUNCTION GLOBAL USIGN8 osif_check_sema
    (
        IN  USIGN8 sema_id
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Checks the current value of a resource semaphore.

possible return values:
0:      Resource is not available, i.e. blocked by at least one task
1:      Resource is available
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    T_OS_SEMA * sema;

FUNCTION_BODY

    sema = Sema_pool + sema_id;

    CHECK_SEMA_VALID(sema, 0, FKT_CHECK_SEMA);
    return ( (OS_GetUseCnt (&sema->rtos_sema) == 0) ? 1 : 0);
}





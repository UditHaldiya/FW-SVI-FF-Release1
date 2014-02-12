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

FILE_NAME          osif_tsk.c



FUNCTIONAL_MODULE_DESCRIPTION

This modul contains all task managements functions as well as facilities to
exchange events and messages beween tasks.

=========================================================================== */
#include "keywords.h"
#define  MODULE_ID      (COMP_OSIF + MOD_OSIFTSK)

INCLUDES
#include <string.h>
#include <stdlib.h>

#include "base.h"

#define VAR_EXTERN      /* Allocate OSIF global variables                   */

#include "osif.h"
#include "osif_cfg.h"
#include "osif_int.h"

#include "except.h"

#include "oswrap.h"

LOCAL_DEFINES
#ifndef STACK_MEM
#define STACK_MEM           NEAR_D
#endif /* STACK_MEM */


#define TASK_SIZE_MSG       sizeof (void FAR_D *)
#define MBUFFER_SIZE        (TASK_SIZE_MSG * MAX_NUM_MSG)

#define CHECK_TASK_VALID(task_id, location)    \
                {                                                           \
                     if ( Task[task_id].state < CREATED )             \
                     {                                                      \
                         _OS_EXCEPTION(ERR_TASK_NEXIST, task_id, location, 0);   \
                     }                                                      \
                }

#define CHECK_EVENT_VALID(event, task_id, location)    \
                {                                                           \
                     if ( event & ~Task[task_id].valid_events )             \
                     {                                                      \
                         _OS_EXCEPTION(ERR_INV_EVENT, task_id, location, event);   \
                     }                                                      \
                }

#define CHECK_MSGEVT_VALID(event, task_id, location)    \
                {                                                           \
                     if ( event & ~Task[task_id].msg_events )               \
                     {                                                      \
                         _OS_EXCEPTION(ERR_INV_MSGEVT, task_id, location, event);  \
                     }                                                      \
                }

#define CHECK_MSG_PTR_VALID(msg_ptr, task_id, location)    \
                {                                                           \
                     if ( !msg_ptr )                                        \
                     {                                                      \
                         _OS_EXCEPTION(ERR_INV_MSG_PTR, task_id, location, msg_ptr);  \
                     }                                                      \
                }

#define INIT_SIGNATURE 0x5555AAAAU //AK: paranoia but still not bullet-proof

#define TASK_LOOP_SYSSTART_ALWAYS(task_id)                  \
                Task[task_id].main (OS_IF_EVENT_SYSSTART);                  \
                Task[task_id].exec_count = 0;                               \
                Task[task_id].init_signature = INIT_SIGNATURE;              \
                while (TRUE)                                                \
                {                                                           \
                    T_EVENT event;                                          \
                                                                            \
                    event = osif_wait_event (task_id, Task[task_id].await_events);      \
                    Task[task_id].main (event);                             \
                    if(Task[task_id].exec_count < ~0U)                      \
                    {                                                       \
                        Task[task_id].exec_count++;                         \
                    }                                                       \
                    _ASSERT (OS_RegionCnt == 0);                            \
                    _ASSERT (OS_DICnt == 0);                                \
                } /* end while */


    /* Not supported in standard Softing environment */
#define TASK_LOOP_SYSSTART_ON_COLDSTART(task_id)            \
                if ( Coldstart_flag != FALSE )                              \
                {                                                           \
                    Task[task_id].main (OS_IF_EVENT_SYSSTART);              \
                }                                                           \
                while (TRUE)                                                \
                {                                                           \
                    T_EVENT event;                                          \
                                                                            \
                    event = osif_wait_event (task_id, Task[task_id].await_events);      \
                    Task[task_id].main (event);                             \
                    _ASSERT (OS_RegionCnt == 0);                            \
                    _ASSERT (OS_DICnt == 0);                                \
                } /* end while */


LOCAL_TYPEDEFS
typedef enum _T_TASK_STATE {
    UNINITIALIZED   = 0,
    INITIALIZED,
    CREATED,
    SUSPENDED,
    DELETED,
    TSK_IDLE
} T_TASK_STATE;

typedef struct _task_info {
        USIGN8              id;                 /* Task buffer id            */
        USIGN8              prio;               /* Task priority             */
        USIGN8              prio_cfg;            /* Configured task priority  */
        OS_TASK NEAR_D      *tcb;               /* Task control block        */
        VOID (FAR_C         *main) (T_EVENT);   /* Task main routine         */
        int                 stacksize;          /* size of stack for task    */
        void STACK_MEM      *stack;             /* task's stack area         */
        T_EVENT             valid_events;       /* valid events for task     */
        T_EVENT             await_events;       /* expected events for task  */
        T_EVENT             msg_events;         /* events with assigned msg. */
        T_TASK_STATE        state;              /* current task state        */
        OS_MAILBOX NEAR_D   *mb[NUM_EVENTS]; /* Task mailboxes            */
        const char          *name;
        USIGN32 exec_count; //AK: added
        USIGN32 init_signature;  //AK: added
} TASK_INFO;


FUNCTION_DECLARATIONS
FUNCTION LOCAL USIGN8 osif_event2mb (IN T_EVENT event);
#if defined (COMPILER_IAR) && !defined(CPU_TYPE_78K4)
#if __VER__ < 200
#pragma function=C_task
#else
/* 2.11a compiler does not support this feature */
#endif
#endif /* COMPILER_IAR */
#if MAX_TASK_NUM > 0
  FUNCTION LOCAL VOID task_0 (VOID);
#endif
#if MAX_TASK_NUM > 1
  FUNCTION LOCAL VOID task_1 (VOID);
#endif
#if MAX_TASK_NUM > 2
  FUNCTION LOCAL VOID task_2 (VOID);
#endif
#if MAX_TASK_NUM > 3
  FUNCTION LOCAL VOID task_3 (VOID);
#endif
#if MAX_TASK_NUM > 4
  FUNCTION LOCAL VOID task_4 (VOID);
#endif
#if MAX_TASK_NUM > 5
  FUNCTION LOCAL VOID task_5 (VOID);
#endif
#if MAX_TASK_NUM > 6
  FUNCTION LOCAL VOID task_6 (VOID);
#endif
#if MAX_TASK_NUM > 7
  FUNCTION LOCAL VOID task_7 (VOID);
#endif
#if MAX_TASK_NUM > 8
  FUNCTION LOCAL VOID task_8 (VOID);
#endif
#if MAX_TASK_NUM > 9
  FUNCTION LOCAL VOID task_9 (VOID);
#endif
#if MAX_TASK_NUM > 10
  FUNCTION LOCAL VOID task_a (VOID);
#endif
#if MAX_TASK_NUM > 11
  FUNCTION LOCAL VOID task_b (VOID);
#endif
#if MAX_TASK_NUM > 12
  FUNCTION LOCAL VOID task_c (VOID);
#endif
#if MAX_TASK_NUM > 13
  FUNCTION LOCAL VOID task_d (VOID);
#endif
#if MAX_TASK_NUM > 14
  FUNCTION LOCAL VOID task_e (VOID);
#endif
#if MAX_TASK_NUM > 15
  FUNCTION LOCAL VOID task_f (VOID);
#endif
  FUNCTION LOCAL VOID idle_loop (VOID);


#ifdef COMPILER_IAR
#if __VER__ < 200
#pragma function=default
#else
/* 2.11a compiler does not support this feature */
#endif
#endif /* COMPILER_IAR */

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA

#include "cs_osif.h"                         /* CONSTANTS SEGMENT DEFINITION */
const T_TIMER_HOOK Task_array[MAX_TASK_NUM] =
{
#if MAX_TASK_NUM > 0
    task_0
#endif
#if MAX_TASK_NUM > 1
    ,task_1
#endif
#if MAX_TASK_NUM > 2
    ,task_2
#endif
#if MAX_TASK_NUM > 3
    ,task_3
#endif
#if MAX_TASK_NUM > 4
    ,task_4
#endif
#if MAX_TASK_NUM > 5
    ,task_5
#endif
#if MAX_TASK_NUM > 6
    ,task_6
#endif
#if MAX_TASK_NUM > 7
    ,task_7
#endif
#if MAX_TASK_NUM > 8
    ,task_8
#endif
#if MAX_TASK_NUM > 9
    ,task_9
#endif
#if MAX_TASK_NUM > 10
    ,task_a
#endif
#if MAX_TASK_NUM > 11
    ,task_b
#endif
#if MAX_TASK_NUM > 12
    ,task_c
#endif
#if MAX_TASK_NUM > 13
    ,task_d
#endif
#if MAX_TASK_NUM > 14
    ,task_e
#endif
#if MAX_TASK_NUM > 15
    ,task_f
#endif
};
#include "cs_def.h"                             /* DEFAULT CONSTANT SEGMENT */

#include "da_osif.h"                            /* DATA SEGMENT DEFINITION */
NO_INIT char        mbuffer_base[MBUFFER_SIZE];
NO_INIT TASK_INFO   Task[MAX_TASK_NUM];
#include "da_def.h"                             /* DEFAULT DATA SEGMENT */

#include "da_embos.h"                           /* DATA SEGMENT DEFINITION */
NO_INIT OS_TASK     tcb_base[MAX_TASK_NUM];
NO_INIT OS_MAILBOX  mb_base[MB_NUM];
NO_INIT OS_TASK     Idle_tcb;
#include "da_def.h"                             /* DEFAULT DATA SEGMENT */

#include "da_osstk.h"                           /* DATA SEGMENT DEFINITION */
/* recommendation from Segger to use at minimum 256 bytes for Task Stacks */
NO_INIT char        Idle_stack[256];
#include "da_def.h"                             /* DEFAULT DATA SEGMENT */

FUNCTION GLOBAL VOID osif_init (VOID)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to initialize the OS Interface.

possible-return-codes:
- NONE
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

    int      i, k, l;
    USIGN32  mbuf_idx;
    USIGN16  mb_cnt;
    USIGN8   id;

FUNCTION_BODY

    /* clear memory areas */
    /*--------------------*/
    memset (Task, 0, sizeof (Task));
    memset (tcb_base, 0, sizeof (tcb_base));
    memset (mb_base, 0, sizeof (mb_base));
    memset (mbuffer_base, 0, sizeof (mbuffer_base));
    Sema_init = FALSE;
    Tim_init  = FALSE;

/* --- init task management ------------------------------------------------ */
    mbuf_idx = 0;
    mb_cnt = 0;
    for (i=0; i<MAX_TASK_NUM; i++)
    {
        for (k=0; k<MAX_TASK_NUM; k++) {
            id = Task_cfg[k].id;
            CHECK_TASK_ID(id, FKT_INIT);
            if ( (Task_cfg[k].stacksize != 0) && (id == i) )
                break;
        } /* end for k */

        if ( k == MAX_TASK_NUM )
        {
            continue;               /* no according task found      */
        }
            /* Check if task is already defined */
            /*----------------------------------*/
        if ( Task[i].state != UNINITIALIZED)
        {
            _OS_EXCEPTION(ERR_TASKID_AMB, i, FKT_INIT, 0);
        }

            /* Check if OS_IF_EVENT_SYSSTART value (0x80) is */
            /* already defined for other task specific event */
            /*-----------------------------------------------*/

        _ASSERT (!(Task_cfg[k].valid_events & OS_IF_EVENT_SYSSTART))

        Task[i].id           = i;
        Task[i].state        = INITIALIZED;
        Task[i].tcb          = tcb_base + i;
        Task[i].prio_cfg     = Task_cfg[k].prio;
        Task[i].name         = Task_cfg[k].name;
        Task[i].stacksize    = Task_cfg[k].stacksize;
        Task[i].stack        = Task_cfg[k].stack;
        Task[i].valid_events = Task_cfg[k].valid_events | OS_IF_EVENT_SYSSTART;
        Task[i].msg_events   = Task_cfg[k].msg_events;
        Task[i].main         = (VOID (FAR_C *) (T_EVENT)) Task_cfg[k].main;

        for (l=0; l<NUM_EVENTS; l++)
        {
            if ( (Task[i].msg_events & (0x01 << l)) != 0 )
            {
                if ( (Task[i].valid_events & (0x01 << l)) == 0 )
                {
                    _OS_EXCEPTION(ERR_INV_MSGEVT, i, FKT_INIT, 0);
                }

                Task[i].mb[l] = mb_base + (mb_cnt++);
                if ( mb_cnt >= MB_NUM )
                {
                    _OS_EXCEPTION(ERR_INSUFF_MBX, i, FKT_INIT, 0);
                }

                CREATEMB(Task[i].mb[l], TASK_SIZE_MSG, Task_cfg[k].num_msg, mbuffer_base + mbuf_idx);
                mbuf_idx += TASK_SIZE_MSG * Task_cfg[k].num_msg;
                if ( mbuf_idx >= MBUFFER_SIZE )
                {
                    _OS_EXCEPTION(ERR_INSUFF_MBUF, i, FKT_INIT, mbuf_idx);
                }

            }
        } /* end for l */

    } /* end for i */

        /* create IDLE task */
    OS_CreateTask (&Idle_tcb, "Idle_task", 1, idle_loop, Idle_stack, sizeof (Idle_stack), 2);

    return;
}



/*****************************************************************************/
/*** Task management                                                       ***/
/*****************************************************************************/

FUNCTION GLOBAL VOID osif_create_task
    (
        IN USIGN8 task_id,
        IN USIGN8 task_prio,
        IN VOID   (FAR_C *task_main_func) (T_EVENT),
        IN T_EVENT event_mask
    )

/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to create a task. In case of any error the global exception
handler is called. If the task already has been created, only the priority is
changed; the main function pointer is ignored.

IN:  task_id                 task-identifier
     task_prio               task priority
     task_rcv_main_func      task main function pointer
     event_mask              events, the task is expecting on wakeup


possible return value:
- NONE

-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    VOID (FAR_C *task_loop) (VOID);

FUNCTION_BODY

    CHECK_TASK_ID(task_id, FKT_CREATE_TASK);

    if ( Task[task_id].state == UNINITIALIZED )
    {
        _OS_EXCEPTION(ERR_TASK_NEXIST, task_id, FKT_CREATE_TASK, 0);
    }

    CHECK_EVENT_VALID(event_mask, task_id, FKT_CREATE_TASK);

    if ( task_main_func != NULL )
    {
        Task[task_id].main = task_main_func;
        task_loop = Task_array[task_id];
    }
    else
    {
        if ( Task[task_id].main == NULL )
        {
            _OS_EXCEPTION(ERR_INV_MAINFKT, task_id, FKT_CREATE_TASK, 0);
        }
        task_loop = (VOID (FAR_C *) (VOID)) Task[task_id].main;
    }

    if ( task_prio != 0 )
    {
        Task[task_id].prio = task_prio;
    }
    else
    {
        Task[task_id].prio = Task[task_id].prio_cfg;
    }

    if ( (Task[task_id].state == INITIALIZED) || (event_mask != 0) )
    {
        Task[task_id].await_events = event_mask;
    }

    if ( (Task[task_id].state == CREATED) || (Task[task_id].state == SUSPENDED) )
    {
        OS_SetPriority (Task[task_id].tcb, Task[task_id].prio);
    }
    else
    {
        osif_disable_all_tasks ();
        Task[task_id].state = CREATED;
        OS_CreateTask (
                        Task[task_id].tcb,
                        (char *) Task[task_id].name,
                        Task[task_id].prio,
                        task_loop,
                        Task[task_id].stack,
                        Task[task_id].stacksize,
                        2
                      );
        osif_enable_all_tasks ();
    }
    return;
}


u8 tcb2index(const OS_TASK *tcb)
{
    int i;
    for (i=0; i<MAX_TASK_NUM; i++)
    {
        if ( Task[i].tcb == tcb )
        {
            return (i);
        }
    } /* end for i */

    _OS_EXCEPTION(ERR_NO_TASK_ID, 255, FKT_GET_TASK_ID, OS_pCurrentTask);

    return (255);
}


FUNCTION GLOBAL USIGN8 osif_get_current_task_id (VOID)
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Evaluates the ID of the current running task.

possible return value:
- ID of the current running task
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    int i;

FUNCTION_BODY
    for (i=0; i<MAX_TASK_NUM; i++)
    {
        if ( Task[i].tcb == OS_pCurrentTask )
        {
            return (i);
        }
    } /* end for i */

    _OS_EXCEPTION(ERR_NO_TASK_ID, 255, FKT_GET_TASK_ID, OS_pCurrentTask);

    return (255);
}


FUNCTION GLOBAL VOID osif_delete_task
    (
        IN USIGN8 task_id
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to delete a task. In case of any error the global exception
handler is called.

IN:  task_id                 task-identifier

possible return value:
- NONE
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY
    CHECK_TASK_ID(task_id, FKT_DELETE_TASK);

    OS_Terminate (Task[task_id].tcb);

    return;
}


/******************************************************************************/

FUNCTION GLOBAL VOID osif_disable_task
  (
    IN USIGN8   task_id
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Refrains a certain task from being executed by setting its priority to 0.

PARAMETERS:
task_id:    ID of task to be disabled

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY
    CHECK_TASK_ID(task_id, FKT_DISABLE_TASK);
    CHECK_TASK_VALID(task_id, FKT_DISABLE_TASK);

    _ASSERT(task_id != osif_get_current_task_id());

    OS_SetPriority (Task[task_id].tcb, 0);

    return;
} /* FUNCTION disable_tread */


/******************************************************************************/

FUNCTION GLOBAL VOID osif_enable_task
  (
    IN USIGN8   task_id
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Re-enables a previously disables task by restoring its original priority.

PARAMETERS:
task_id:    ID of task to be disabled

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY
    CHECK_TASK_ID(task_id, FKT_ENABLE_TASK);
    CHECK_TASK_VALID(task_id, FKT_ENABLE_TASK);

    OS_SetPriority (Task[task_id].tcb, Task[task_id].prio);

    return;
} /* FUNCTION disable_tread */


/******************************************************************************/

FUNCTION GLOBAL VOID osif_disable_all_tasks (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Refrains the system globally from scheduling tasks. Must be used in pair
    with 'osif_enable_all_tasks ()'.

PARAMETERS:
    none

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY
    OS_EnterRegion ();
    return ;
} /* FUNCTION osif_disable_all_tasks */


/******************************************************************************/

FUNCTION GLOBAL VOID osif_enable_all_tasks (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Re-enables the task scheduling. Must be used in pair with
    'osif_disable_all_tasks ()'.

PARAMETERS:
    none

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY
    OS_LeaveRegion ();
    return ;
} /* FUNCTION osif_enable_all_tasks */



/******************************************************************************/

FUNCTION GLOBAL VOID DISABLE_INTERRUPTS (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Disables all interrupts. Must be used in pair with 'RESTORE_INTERRUPTS()'.

PARAMETERS:
    none

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    OS_SDI();
    return;
} /* FUNCTION DISABLE_INTERRUPTS */


/******************************************************************************/

FUNCTION GLOBAL VOID RESTORE_INTERRUPTS (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Sets interrupt enable state to state before call to 'DISABLE_INTERRUPTS()'.

PARAMETERS:
    none

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    OS_RI();
    return;
} /* FUNCTION RESTORE_INTERRUPTS */


/* ------------------------------------------------------------------------- */
/* memory allocation functions                                               */
/* ------------------------------------------------------------------------- */

/******************************************************************************/

FUNCTION GLOBAL VOID * osif_malloc
  (
    IN USIGN16 mem_size
  )

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This function allocates memory in a thread-safe way.

PARAMETERS:
  mem_size => sizeof requested memory

GLOBALS_AFFECTED:
  none

RETURN_VALUES:
  pointer to the new memory block
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  VOID *mem_block;

FUNCTION_BODY

  osif_disable_all_tasks();
  mem_block = malloc(mem_size);
  osif_enable_all_tasks();

  return mem_block;
}


/******************************************************************************/

FUNCTION GLOBAL VOID osif_free
  (
    IN VOID * mem_block
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This functio frees memory in a thread-safe way.

PARAMETERS:
  mem_block => pointer to allocated memory

GLOBALS_AFFECTED:
  none

RETURN_VALUES:
  none
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  osif_disable_all_tasks();
  free(mem_block);
  osif_enable_all_tasks();
}


/* ------------------------------------------------------------------------- */
/* event control functions                                                   */
/* ------------------------------------------------------------------------- */

FUNCTION GLOBAL T_EVENT osif_wait_event
    (
        IN  USIGN8 task_id,
        IN  T_EVENT event_mask
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to receive an event addressed to the calling task. The
call to 'osif_wait_event' is blocking, i.e. the function does not return, until
an event is received by the task.

IN:     event_mask              -> events which are waited for

possible return value:
- event data
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
        T_EVENT rcv_event;

FUNCTION_BODY


        /* Check task identifier */
        /*-----------------------*/
    CHECK_TASK_ID(task_id, FKT_WAIT_EVENT);
    CHECK_TASK_VALID(task_id, FKT_WAIT_EVENT);

        /* Check event mask */
        /*------------------*/
    CHECK_EVENT_VALID(event_mask, task_id, FKT_WAIT_EVENT);

        /* Receive event from buffer */
        /*---------------------------*/
    rcv_event = OS_WaitEvent (event_mask);

        /* Check for unexpected (= masked) events */
        /*----------------------------------------*/
    if ( (rcv_event & ~event_mask) != 0 )
    {
        OS_SignalEvent ((rcv_event & ~event_mask), Task[task_id].tcb);
        rcv_event &= event_mask;
    }


    return (rcv_event);
}

FUNCTION GLOBAL T_EVENT osif_clear_event
    (
        IN  USIGN8 task_id,
        IN  T_EVENT event_mask
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Clears the event area of a task by the events defined by 'event_mask'.
'task_id' must be the ID of the calling task.

The function returns immediately, i.e. if none of the selected events
are active, nothing happens.

IN:     task_id                 -> ID of task, which events are to be cleared
        event_mask              -> events which are to be cleared

possible return value:
- event data
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
        T_EVENT rcv_event;

FUNCTION_BODY


        /* Check task identifier */
        /*-----------------------*/
    CHECK_TASK_ID(task_id, FKT_WAIT_EVENT);
    CHECK_TASK_VALID(task_id, FKT_WAIT_EVENT);
    _ASSERT(task_id == osif_get_current_task_id ());

        /* Check event mask */
        /*------------------*/
    CHECK_EVENT_VALID(event_mask, task_id, FKT_WAIT_EVENT);

        /* Check for currently active events */
        /*-----------------------------------*/
    osif_disable_all_tasks();
    rcv_event = OS_GetEventsOccured (Task[task_id].tcb);
    rcv_event &= event_mask;

        /* Clear active events by executing WaitEvent */
        /*--------------------------------------------*/
    if ( rcv_event != 0 )
    {
        OS_WaitSingleEvent (rcv_event);
    }


    osif_enable_all_tasks();
    return (rcv_event);
}

FUNCTION GLOBAL VOID  osif_set_event
(
    IN USIGN8 task_id,
    IN USIGN8 event
)
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to send and queue an event message for the task specified
by 'task_id'.

IN:  task_id      ->  task identifer which shall receive the event
IN:  event        ->  event to be signaled

possible return value:
- NONE
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY


        /* Check task identifier */
        /*-----------------------*/
    CHECK_TASK_ID(task_id, FKT_SET_EVENT);
    CHECK_TASK_VALID(task_id, FKT_SET_EVENT);

        /* Check event */
        /*-------------*/
    CHECK_EVENT_VALID(event, task_id, FKT_SET_EVENT);

        /* Send event */
        /*------------*/
    OS_SignalEvent (event, Task[task_id].tcb);

    return;
}


FUNCTION GLOBAL VOID  osif_resume_task
    (
        IN USIGN8 task_id
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to resume a task specified by 'task_id'.

IN:  task_id      ->  task identifer of the task to resume

possible return value:
- NONE
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    CHECK_TASK_ID(task_id, FKT_RESUME_TASK);
    CHECK_TASK_VALID(task_id, FKT_RESUME_TASK);

    osif_set_event (task_id, (T_EVENT) OS_IF_EVENT_RESUME);

    return;
}

/* ------------------------------------------------------------------------- */
/* message control functions                                                 */
/* ------------------------------------------------------------------------- */
FUNCTION GLOBAL T_OS_MSG_HDR *osif_get_msg
    (
        IN USIGN8 task_id,
        IN T_EVENT event
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to get a pointer to the message which has been
indicated by the event OS_IF_EVENT_MESSAGE

IN:  task_id            -> task identifier which want to get the message

possible return value:
- pointer to the received message
- NULL if no message is available
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
        int mb_idx;
        T_OS_MSG_HDR * mail;

FUNCTION_BODY

        /* Check task identifier */
        /*-----------------------*/
    CHECK_TASK_ID(task_id, FKT_GET_MSG);
    CHECK_TASK_VALID(task_id, FKT_GET_MSG);

        /* Check for valid event */
        /*-----------------------*/
    CHECK_MSGEVT_VALID(event, task_id, FKT_GET_MSG);

        /* convert event bit to mailbox index */
        /*------------------------------------*/
    if ( (mb_idx = osif_event2mb (event)) >= NUM_EVENTS )
    {
        _OS_EXCEPTION(ERR_MULT_EVENTS, task_id, FKT_GET_MSG, 0);
    }

        /* Receive mail from mailbox */
        /*---------------------------*/
    if ( OS_GetMailCond (Task[task_id].mb[mb_idx], (VOID *) (&mail)) )
        return (NULL);
    else
        return (mail);

}


FUNCTION GLOBAL VOID osif_send_msg
    (
        IN USIGN8 msg_event,
        IN USIGN8 rcv_task_id,
        IN USIGN8 snd_task_id,
        IN T_OS_MSG_HDR FAR_D *msg_ptr
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to send a MSG-BLOCKS to 'rcv-task-id' via
operating system. The MSG-BLOCK must start with a T_OS_MSG_HDR.

IN:     msg_event               -> event id, the message shall be connected to
IN:     rcv_task_id             -> receive task id
IN:     snd_task_id             -> send task id
IN:     msg_ptr                 -> pointer to USR-MSG

possible return value:
- NONE
-----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN8 mb_idx;

FUNCTION_BODY

        /* Check task identifier */
        /*-----------------------*/
    CHECK_TASK_ID(rcv_task_id, FKT_SND_MSG_RCV);
    CHECK_TASK_VALID(rcv_task_id, FKT_SND_MSG_RCV);
    CHECK_TASK_ID(snd_task_id, FKT_SND_MSG_SND);

    CHECK_MSGEVT_VALID(msg_event, rcv_task_id, FKT_SND_MSG_RCV);
    CHECK_MSG_PTR_VALID(msg_ptr, snd_task_id, FKT_SND_MSG_SND);

        /* convert event bit to mailbox index */
        /*------------------------------------*/
    if ( (mb_idx = osif_event2mb (msg_event)) >= NUM_EVENTS )
    {
        _OS_EXCEPTION(ERR_MULT_EVENTS, snd_task_id, FKT_SND_MSG_SND, 0);
    }

        /* Process message */
        /*-----------------*/
    msg_ptr->message_id  = msg_event;
    msg_ptr->snd_task_id = snd_task_id;

    if ( OS_PutMailCond ( Task[rcv_task_id].mb[mb_idx], (VOID *) &msg_ptr) ) {
        _OS_EXCEPTION(ERR_MSG_NOT_PROC, snd_task_id, FKT_SND_MSG_SND, 0);
    }
    osif_set_event (rcv_task_id, msg_event);

    return;
}


/*########################  Internal functions  ############################*/

FUNCTION LOCAL USIGN8 osif_event2mb
    (
        IN T_EVENT event
    )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Converts a event to the according mailbox index
IN:  event            -> event to be converted

possible return value:
- mailbox index
- 0xFF (255) if more than one events are specified
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
        USIGN8 mb_idx;

FUNCTION_BODY
    switch (event)
    {
        case 0x01:
            mb_idx = 0;
            break;
        case 0x02:
            mb_idx = 1;
            break;
        case 0x04:
            mb_idx = 2;
            break;
        case 0x08:
            mb_idx = 3;
            break;
        case 0x10:
            mb_idx = 4;
            break;
        case 0x20:
            mb_idx = 5;
            break;
        case 0x40:
            mb_idx = 6;
            break;
        case 0x80:
            mb_idx = 7;
            break;
        default:
            mb_idx = 0xff;
    } /* end switch */

    return (mb_idx);
}

#if defined (COMPILER_IAR) && !defined(CPU_TYPE_78K4)
#if __VER__ < 200
#pragma function=C_task
#else
/* 2.11a compiler does not support this feature */
#endif
#endif /* COMPILER_IAR */

/******************************************************************************/

FUNCTION LOCAL VOID task_0
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[0]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY
#if MAX_TASK_NUM > 0
    TASK_LOOP_SYSSTART_ALWAYS(0);
#endif
} /* FUNCTION task_0 */


/******************************************************************************/

#if MAX_TASK_NUM > 1
FUNCTION LOCAL VOID task_1
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[1]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(1);

} /* FUNCTION task_1 */
#endif /* MAX_TASK_NUM > 1 */


/******************************************************************************/

#if MAX_TASK_NUM > 2
FUNCTION LOCAL VOID task_2
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[2]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(2);

} /* FUNCTION task_2 */
#endif /* MAX_TASK_NUM > 2 */


/******************************************************************************/

#if MAX_TASK_NUM > 3
FUNCTION LOCAL VOID task_3
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[3]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(3);

} /* FUNCTION task_3 */
#endif /* MAX_TASK_NUM > 3 */


/******************************************************************************/

#if MAX_TASK_NUM > 4
FUNCTION LOCAL VOID task_4
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[0]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(4);

} /* FUNCTION task_4 */
#endif /* MAX_TASK_NUM > 4 */


/******************************************************************************/

#if MAX_TASK_NUM > 5
FUNCTION LOCAL VOID task_5
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[5]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(5);

} /* FUNCTION task_5 */
#endif /* MAX_TASK_NUM > 5 */


/******************************************************************************/

#if MAX_TASK_NUM > 6
FUNCTION LOCAL VOID task_6
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[6]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(6);

} /* FUNCTION task_6 */
#endif /* MAX_TASK_NUM > 6 */


/******************************************************************************/

#if MAX_TASK_NUM > 7
FUNCTION LOCAL VOID task_7
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[7]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(7);

} /* FUNCTION task_7 */
#endif /* MAX_TASK_NUM > 7 */


/******************************************************************************/

#if MAX_TASK_NUM > 8
FUNCTION LOCAL VOID task_8
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[8]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(8);

} /* FUNCTION task_8 */
#endif /* MAX_TASK_NUM > 8 */


/******************************************************************************/

#if MAX_TASK_NUM > 9
FUNCTION LOCAL VOID task_9
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[9]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(9);

} /* FUNCTION task_9 */
#endif /* MAX_TASK_NUM > 9 */


/******************************************************************************/

#if MAX_TASK_NUM > 10
FUNCTION LOCAL VOID task_a
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[10]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(10);

} /* FUNCTION task_a */
#endif /* MAX_TASK_NUM > 10 */


/******************************************************************************/

#if MAX_TASK_NUM > 11
FUNCTION LOCAL VOID task_b
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[11]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(11);

} /* FUNCTION task_b */
#endif /* MAX_TASK_NUM > 11 */


/******************************************************************************/

#if MAX_TASK_NUM > 12
FUNCTION LOCAL VOID task_c
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[12]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(12);

} /* FUNCTION task_c */
#endif /* MAX_TASK_NUM > 12 */


/******************************************************************************/

#if MAX_TASK_NUM > 13
FUNCTION LOCAL VOID task_d
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[13]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(13);

} /* FUNCTION task_d */
#endif /* MAX_TASK_NUM > 13 */


/******************************************************************************/

#if MAX_TASK_NUM > 14
FUNCTION LOCAL VOID task_e
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[14]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(14);

} /* FUNCTION task_e */
#endif /* MAX_TASK_NUM > 14 */


/******************************************************************************/

#if MAX_TASK_NUM > 15
FUNCTION LOCAL VOID task_f
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Main loop for task[15]

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    TASK_LOOP_SYSSTART_ALWAYS(15);

} /* FUNCTION task_f */
#endif /* MAX_TASK_NUM > 15 */



/******************************************************************************/

FUNCTION LOCAL VOID idle_loop
  (
    VOID
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
Idle task loop

PARAMETERS:
none

GLOBALS_AFFECTED:
none

RETURN_VALUES:
none

------------------------------------------------------------------------------*/
{
#if 0   //this code is for the idle time bachground check
    OSTaskIdleHook();
#else   //this code is for the minimal power consumption
    while (TRUE)
    {
        __WFI();
    }
#endif
} /* FUNCTION idle_loop */

#ifdef COMPILER_IAR
#pragma function=default
#endif /* COMPILER_IAR */

USIGN32 osif_GetNumberOfRuns(USIGN8 task_id)
{
    USIGN32 ret;
    if(Task[task_id].init_signature == INIT_SIGNATURE)
    {
        ret = Task[task_id].exec_count;
    }
    else
    {
        ret = 0;
    }
    return ret;
}
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

FILE_NAME          osif.h



FUNCTIONAL_MODULE_DESCRIPTION

Module description

Contains the interface to OSIF as adopted to the FF-BFD protocol software
and the realtime operating system embOS M16C V3.x

=========================================================================== */

#ifndef __OSIF_H__
#define __OSIF_H__

#define OSIF_EMBOS
_ADAPT_LINT_BEFORE_RTOS_H
#include <RTOS.h>
_RESTORE_LINT_AFTER_RTOS_H

/****************************************************************************/
/* TASK IDENTIFIERS                                                         */
/****************************************************************************/

#define CURRENT_TASK_ID         255
#define MAX_TASK_PRIO           255

/*--------------------------------------------------------------------------*/
/* System tasks                                                             */
/*--------------------------------------------------------------------------*/
/* EEP task: contains the asynchrounous part of the EEP interface --------- */
#define EEP_TASK_ID              0
#define EEP_TASK_START_PRIO     MAX_TASK_PRIO
#define EEP_TASK_PRIO           5 //Used to be 24; need lowest priority with poll mode driver


/*--------------------------------------------------------------------------*/
/* Application task(s)                                                       */
/*--------------------------------------------------------------------------*/
#define APPL_TASK_ID             1
#define APPL_TASK_START_PRIO   100
#if defined(PROTOCOL_FF)
#define APPL_TASK_PRIO           BGD50_TASK_PRIO + 1
#else /* PROTOCOL_PA */
#define APPL_TASK_PRIO           BGD_TASK_PRIO + 1
#endif


/*--------------------------------------------------------------------------*/
/* Function Block execution task(s)                                         */
/*--------------------------------------------------------------------------*/
#define FB_TASK_ID               2
#define FB_TASK_PRIO            50


/*------------------------------------------------------------------------*/
/* Highest prior Watchdog task                                            */
/*------------------------------------------------------------------------*/
#define FDC_WD_TASK_ID        3
#define FDC_WD_TASK_PRIO      MAX_TASK_PRIO - 1


/*--------------------------------------------------------------------------*/
/* Communication layer tasks                                                */
/*--------------------------------------------------------------------------*/
/*@Dresser-Masoneilan project modified: no SWDL Task is used */
//#if (defined (DOWNLOAD_DEVICE) || defined (SW_DOWNLOAD)) && (defined (HART_MASTER) || defined (MODBUS_MASTER))
//#define FIRST_PROT_TASK_ID      FB_TASK_ID + 4
//#elif defined (DOWNLOAD_DEVICE) || defined (SW_DOWNLOAD) || defined (HART_MASTER) || defined (MODBUS_MASTER)
#if defined (DOWNLOAD_DEVICE) || defined (SW_DOWNLOAD) || defined (HART_MASTER) || defined (MODBUS_MASTER)
#define FIRST_PROT_TASK_ID      FB_TASK_ID + 3
#else
#define FIRST_PROT_TASK_ID      FB_TASK_ID + 2
#endif /* DOWNLOAD_DEVICE */

#if defined (PROTOCOL_FF)

  /*------------------------------------------------------------------------*/
  /* Tasks specific to FF-BFD communication layer                           */
  /*------------------------------------------------------------------------*/
  /* FDC task: FAL + FBS -------------------------------------------------- */
  #define FDC_TASK_ID           FIRST_PROT_TASK_ID
  #define FDC_TASK_PRIO         20

  /* SUBSCRIBER task: subscriber part of FAL + FBS ------------------------ */
  #define SBSCR_TASK_ID         FIRST_PROT_TASK_ID + 1
  #define SBSCR_TASK_PRIO       25

  /* Send MVC task: part of FBS ------------------------------------------- */
  #define MVC_TASK_ID           FIRST_PROT_TASK_ID + 2
  #define MVC_TASK_PRIO         20

  /* BGD task: background actions ----------------------------------------- */
  #define BGD50_TASK_ID         FIRST_PROT_TASK_ID + 3
  #define BGD50_TASK_PRIO       12

  /* BGD task: background actions ----------------------------------------- */
  #define BGD_TASK_ID           FIRST_PROT_TASK_ID + 4
  #define BGD_TASK_PRIO         10

#ifdef LINK_MASTER
  /* LAS task: LAS part of the DLL ---------------------------------------- */
  /* --- which is not executed on interrupt level ------------------------- */
  #define LAS_TASK_ID           FIRST_PROT_TASK_ID + 5
  #define LAS_TASK_PRIO         90
  #define MAX_TASK_ID           LAS_TASK_ID
#else
  #define MAX_TASK_ID           BGD_TASK_ID
#endif /* LINK_MASTER */

#elif defined (PROTOCOL_PA) || defined (PROTOCOL_DP)

  /*------------------------------------------------------------------------*/
  /* Tasks specific to PA-Slave communication layer                         */
  /*------------------------------------------------------------------------*/
  /* DP Slave task: DP protocol layer ------------------------------------- */
  #define DPS_TASK_ID           FIRST_PROT_TASK_ID
  #define DPS_TASK_PRIO         60

  /* FBS task: PA related function block shell ---------------------------- */
  #define FBS_TASK_ID           FIRST_PROT_TASK_ID + 1
  #define FBS_TASK_PRIO         20

  /* FBS task: PA related function block shell ---------------------------- */
  #define BGD_TASK_ID           FIRST_PROT_TASK_ID + 2
  #define BGD_TASK_PRIO         10

  /* DPS USER task: for testing purposes; replaces the FBS task ----------- */
  #define DUSR_TASK_ID          FIRST_PROT_TASK_ID + 3
  #define DUSR_TASK_PRIO        30

  #define MAX_TASK_ID           DUSR_TASK_ID

#endif /* Protocol type */


#if defined (HART_MASTER)

/*--------------------------------------------------------------------------*/
/* HART Master task                                                         */
/*--------------------------------------------------------------------------*/
#define HM_TASK_ID              4
#define HM_TASK_START_PRIO      (APPL_TASK_START_PRIO - 1)
#define HM_TASK_PRIO            (BGD_TASK_PRIO - 1)


#elif defined (MODBUS_MASTER)

/*--------------------------------------------------------------------------*/
/* MODBUS Master task                                                       */
/*--------------------------------------------------------------------------*/
#define MODB_TASK_ID            4
#define MODB_TASK_START_PRIO    (APPL_TASK_START_PRIO - 1)
#define MODB_TASK_PRIO          (BGD_TASK_PRIO - 1)

#endif /* HART_MASTER , MODBUS_MASTER */


#define MAX_TASK_NUM            (MAX_TASK_ID + 1)


/* ISR tasks: not real tasks, but needed to have an identifier if --------- */
/* messages are sent by interrupt routines -------------------------------- */
#define VIRTUAL_TASK_ID         250
#define FDL_ISR_ID              VIRTUAL_TASK_ID

/****************************************************************************/
/* EVENT-IDENTIFIERS                                                        */
/****************************************************************************/
typedef USIGN8                  T_EVENT;    /* as required by embOS M16C    */
#define NUM_EVENTS              (8 * sizeof(T_EVENT))

/* SYSTEM-EVENTS ---------------------------------------------------------- */
/* Each event mus be represented as one bit ------------------------------- */
#define OS_IF_EVENT_EEPROM      0x01    /* Event from/to EEPROM task        */
#define OS_IF_EVENT_EEPROG      0x02    /* EEP task internal use only!!     */
#define OS_IF_EVENT_REQRES      0x02    /* Request/reponse msg. event       */
#define OS_IF_EVENT_INDCON      0x04    /* Confirm./indication msg. event   */
#define OS_IF_EVENT_UPDATE_IO   0x08    /* Update DP slave IO data image    */
#define OS_IF_EVENT_RESUME      0x08    /* Resume task event                */
#define OS_IF_EVENT_TIMER       0x10    /* Timer expired                    */
#define OS_IF_EVENT_SYSSTART    0x80    /* System starting                  */

/*@Dresser-Masoneilan project modified: no SWDL Task is used */
//#if defined (DOWNLOAD_DEVICE) || defined (SW_DOWNLOAD)

/* differentiation of events within the DOWNLOAD Task by state of CSDS */
//#define DWLD_ACTIVATE                       0x01
//#define DWLD_PREPARE_FOR_DWNLD              0x01
//#define DWLD_CANCEL_DWNLD                   0x02
//#define DWLD_DOWNLOAD_FAILS                 0x04
//#define DWLD_PREPARATION_COMPLETE           0x04
//#define DWLD_DOWNLOAD_SUCCEEDS              0x20
//#define DWLD_TIMEOUT                        0x20
//#define DWLD_FMS_GENERIC_DOWNLOAD_STARTS    0x40
//#define DWLD_PREPARATION_FAILS              0x40

//#endif /* DOWNLOAD_DEVICE */

#if defined (HART_MASTER)

#define HART_ERROR_EVENT        0x04    /* HART-If generated an error         */
#define HART_SEND_END_EVENT     0x08    /* HART interface sent a message      */
#define HART_RECEIVE_END_EVENT  0x20    /* HART interface received a message  */
#define HART_START_EVENT        0x40    /* HART communication to be started   */
#define HART_ACY_CMD_END_EVENT  0x40    /* HART user waits for a cmd end      */

#elif defined (MODBUS_MASTER)

#define MODB_XMT_END_EVENT      0x01    /* MODB query transmitted             */
#define MODB_RCV_END_EVENT      0x02    /* MODB query completed successf.     */
#define MODB_TIMEOUT_EVENT      0x04    /* MODB query timed out               */
#if defined (PROTOCOL_FF)
#define MODB_USR_EVENT          0x40    /* MODB interaction with user         */
#define MODB_START_EVENT        0x08    /* MODB communication to be started   */
#elif defined (PROTOCOL_PA) || defined (PROTOCOL_DP)
#define MODB_USR_EVENT          0x08    /* MODB interaction with user         */
#define MODB_START_EVENT        0x40    /* MODB communication to be started   */
#endif
#endif /* HART_MASTER , MODBUS_MASTER */

#define OS_IF_EVENT_MASK_ALL    0xFF    /* all possible events */

/****************************************************************************/
/* MESSAGE-IDENTIFIERS                                                      */
/****************************************************************************/

/* --- global message types ----------------------------------------------- */
#define REQ_RES_SERVICE_MSG     OS_IF_EVENT_REQRES
#define IND_CON_SERVICE_MSG     OS_IF_EVENT_INDCON


/* global message header used for inter task communication ---------------- */
typedef struct _T_OS_MSG_HDR
{
    USIGN8    message_id;                                /* Message-Identifier */
    USIGN8    snd_task_id;                             /* send task identifier */
    USIGN8    rvc_task_id;                             /* send task identifier */
    USIGN8    user_reserved;                                /* free for user  */
} T_OS_MSG_HDR;


/****************************************************************************/
/* USER MESSAGE BLOCK                                                       */
/****************************************************************************/
typedef struct _T_USR_MSG_BLOCK
{
  T_OS_MSG_HDR          os_msg_header;          /* standard message header   */
  USIGN8                layer;                  /* layer                     */
  USIGN8                service;                /* service identifier        */
  USIGN8                primitive;              /* service primitive         */
  USIGN8                result;                 /* return result             */
  USIGN16               length;
  USIGN8 *              data;
} T_USR_MSG_BLOCK;



/*****************************************************************************/
/*************   MACROS FOR ACCESSING MESSAGE HEADER     *********************/
/*****************************************************************************/

#define _MSG_GET_MSG_TYPE(buffer)   (((T_OS_MSG_HDR FAR_D*)(buffer))->message_id)
#define _MSG_SET_MSG_TYPE(x,buffer) ((((T_OS_MSG_HDR FAR_D*)(buffer))->message_id) = (x))
#define _MSG_GET_SRC_TASK(buffer)   (((T_OS_MSG_HDR FAR_D*)(buffer))->snd_task_id)
#define _MSG_SET_SRC_TASK(x,buffer) ((((T_OS_MSG_HDR FAR_D*)(buffer))->snd_task_id) = (x))


/* --- service primitives -------------------------------------------------- */

#define REQ                     0      /* request */
#define CON                     1      /* confirmation */
#define IND                     2      /* indication */
#define RES                     3      /* response */
#define RET                     4      /* return */


/* --- service result ------------------------------------------------------ */
#define POS                     0x00             /* result for positive response */
#define NEG                     0x01             /* result for negative response */


/* --- macros to access to USR-MSG-BLOCK ----------------------------------- */

#define _USR_MSG_GET_SND(ptr)           \
            (ptr)->os_msg_header.snd_task_id
#define _USR_MSG_SET_SND(ptr, s)        \
            (ptr)->os_msg_header.snd_task_id = s

#define _USR_MSG_GET_RCV(ptr)           \
            (ptr)->os_msg_header.rcv_task_id
#define _USR_MSG_SET_RCV(ptr, r)        \
            (ptr)->os_msg_header.rcv_task_id = r

#define _USR_MSG_GET_LAYER(ptr)         \
            (ptr)->layer
#define _USR_MSG_SET_LAYER(ptr, l)      \
            (ptr)->layer = l

#define _USR_MSG_GET_SERVICE(ptr)       \
            (ptr)->service
#define _USR_MSG_SET_SERVICE(ptr, s)    \
            (ptr)->service = s

#define _USR_MSG_GET_PRIMITIVE(ptr)     \
            (ptr)->primitive
#define _USR_MSG_SET_PRIMITIVE(ptr, p)  \
            (ptr)->primitive = p

#define _USR_MSG_GET_RESULT(ptr)        \
            (ptr)->result
#define _USR_MSG_SET_RESULT(ptr,r)      \
            (ptr)->result = r

#define _USR_MSG_GET_DATA(ptr)          \
            (ptr)->data
#define _USR_MSG_SET_DATA(ptr, d)       \
            (ptr)->data = d

#define _USR_MSG_GET_DATA_LEN(ptr)      \
            (ptr)->length
#define _USR_MSG_SET_DATA_LEN(ptr, l)   \
            (ptr)->length = l


/****************************************************************************/
/* OSIF INTERFACE FUNCTIONS                                                 */
/****************************************************************************/

typedef struct {

    USIGN32 high;   /* most significant part (units of 2^27 ms (ca. 37,3h))*/
    USIGN32 low;    /* least significant part (units of 2^-5 (=1/32) ms)   */

} T_SYS_TIME;



/****************************************************************************/
/* OSIF INTERFACE FUNCTIONS                                                 */
/****************************************************************************/
#define USE_OSIF
#define OS_COMPATIBLE_TO_OLDVERSION     /* needed by RTOS.H     */
#if 0
#define OS_SDI()                    { OS_IncDI(); }
#define OS_RI()                     { OS_DecRI(); }
#else
//To let Lint know that they lock/unlock threads they must be functions
static inline void OS_SDI(void)                    { OS_IncDI(); }
static inline void OS_RI(void)                     { OS_DecRI(); }
#endif

#ifdef __IAR_SYSTEMS_ICC
#if __VER__ < 200
  #pragma alignment(2)
#else
  #pragma pack(push,2)
#endif
#endif


#ifdef __IAR_SYSTEMS_ICC
#if __VER__ < 200
  #pragma alignment ()   /* for IAR compiler: reset alignment to default */
#else
  #pragma pack(pop)
#endif
#endif


#define ENTER_IRQ()                 OS_EnterNestableInterrupt()
#define LEAVE_IRQ()                 OS_LeaveNestableInterrupt()

#define ENTER_FBC_IRQ()             ENTER_IRQ()
#define LEAVE_FBC_IRQ()             LEAVE_IRQ()

/*
    The following SNYC_ macros are empty in a non-OS environment; they may be
    used to synchronize access to function blocks in a multi-tasking environment
*/

#ifndef SYNC_READ_WRITE_ACCESS_START
#define SYNC_READ_WRITE_ACCESS_START
#endif

#ifndef SYNC_READ_WRITE_ACCESS_END
#define SYNC_READ_WRITE_ACCESS_END
#endif

#ifndef SYNC_ALERT_ACCESS_START
#define SYNC_ALERT_ACCESS_START
#endif

#ifndef SYNC_ALERT_ACCESS_END
#define SYNC_ALERT_ACCESS_END
#endif


/*--------------------------------------------------------------------------*/
/* OSIF task management                                                     */
/*--------------------------------------------------------------------------*/

FUNCTION GLOBAL VOID osif_create_task
    (
        IN USIGN8 task_id,
        IN USIGN8 task_prio,
        IN VOID   (FAR_C *task_main_func) (T_EVENT),
        IN T_EVENT event_mask
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to create a task. In case of any error the global
exception handler is called.

IN:  task_id                 task identifier
     task_prio               task priority
     task_rcv_main_func      task main function pointer
     event_mask              events, the task is expecting on wakeup

possible return value:
- NONE
-----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN8 osif_get_current_task_id (VOID)
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Evaluates the ID of the current running task.

possible return value:
- ID of the current running task
-----------------------------------------------------------------------------*/
;


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
;


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
;
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
;


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
;


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
;

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
;

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
;

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
;

/*--------------------------------------------------------------------------*/
/* OSIF event handling                                                      */
/*--------------------------------------------------------------------------*/

FUNCTION GLOBAL T_EVENT osif_wait_event
    (
        IN  USIGN8 task_id,
        IN T_EVENT event_mask
    )
/*-----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to receive an event addressed to the calling task. The
call to 'osif_wait_event' is blocking, i.e. the function does not return, until
an event is received by the task.

IN:     task_id      ->  ID of task which is going to await the event(s)
IN:     event_mask   ->  events which are waited for

possible return value:
- event data
-----------------------------------------------------------------------------*/
;

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
;

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
;


/*--------------------------------------------------------------------------*/
/* OSIF message handling                                                    */
/*--------------------------------------------------------------------------*/

FUNCTION GLOBAL T_OS_MSG_HDR * osif_get_msg
    (
        IN USIGN8 task_id,
        IN T_EVENT event
    )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to get a pointer to the message which has been
indicated by the event OS_IF_EVENT_MESSAGE

IN:  task_id            -> task identifier which want to get the message

possible return value:
- pointer to the received message
- NULL if no message is available
----------------------------------------------------------------------------*/
;


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
;


/*--------------------------------------------------------------------------*/
/* OSIF semaphore handling                                                  */
/*--------------------------------------------------------------------------*/

FUNCTION GLOBAL VOID osif_init_sema (VOID)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to initialize the OS Interface.

possible-return-codes:
- none
-----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN8 osif_create_sema
    (
        IN  USIGN8 task_id
    )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Creates a resource semaphore. The resource is not blocked after creation.

possible return values:
- ID of newly created semaphore

----------------------------------------------------------------------------*/
;


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
;


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
;


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
;


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
;


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
;


/*--------------------------------------------------------------------------*/
/* OSIF timer handling                                                      */
/*--------------------------------------------------------------------------*/

FUNCTION GLOBAL USIGN8 osif_create_timer
  (
    USIGN8      task_id
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Creates a timer and connects it to task 'task_id', which will receive an
OS_IF_EVENT_TIMER when the timer expires.

Basically, all timers run in one-shot mode, i.e. the timer stops running after
expiration.

possible return values:
  Timer ID the timer is accessible through.

------------------------------------------------------------------------------*/
;


FUNCTION GLOBAL VOID osif_start_timer
  (
    IN USIGN8   timer_id,
    IN USIGN32  timer_value,
    IN BOOL     cyclic
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Starts the timer addressed by ID 'timer_id' with value 'timer_value'.

If 'cyclic' is TRUE, the timer will work in a cyclic manner, i.e. keep
   generating events until the timer is stopped
If 'cyclic' is FALSE, the timer stops automatically after expiration.

possible return values:
  none

------------------------------------------------------------------------------*/
;


FUNCTION GLOBAL VOID osif_stop_timer
  (
    USIGN8      timer_id
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Stops the timer addressed by ID 'timer_id'. As the timer won't expire any more,
the callback function will not be called.

possible return values:
  none

------------------------------------------------------------------------------*/
;


FUNCTION GLOBAL VOID osif_get_system_time
  (
    OUT T_SYS_TIME * sys_time
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  Gets the current system time in units of 1/32 msec.

PARAMETERS:
  system_time   -> Location where to store the time value

GLOBALS_AFFECTED:
  none

RETURN_VALUES:
  none

------------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN32 osif_get_time_in_ms (VOID)

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  The function returns the time in milli seconds (ms) which is
  elapsed after the start of the device.

PARAMETERS:

  None

GLOBALS_AFFECTED:

  None

RETURN_VALUES:

  USIGN32 elapsed time in milli seconds

------------------------------------------------------------------------------*/
;

extern USIGN32 osif_GetNumberOfRuns(USIGN8 task_id);

#endif /* __OSIF_H__ */

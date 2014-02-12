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

FILE_NAME          osif_tim.c



FUNCTIONAL_MODULE_DESCRIPTION

Contains timer functions of OSIF. If called with any invalid parameters, all
functions will generate an ERROR exception.

=========================================================================== */
#include "keywords.h"   /* keyword defines */
#define  MODULE_ID      (COMP_OSIF + MOD_OSIFTIM)

INCLUDES
#include "base.h"
#include "hw_if.h"
#include "hw_cfg.h"
#include "osif.h"
#include "osif_cfg.h"
#include "osif_int.h"

#include "except.h"

GLOBAL_DEFINES

LOCAL_DEFINES
#define  MAX_TIM_ID         10
#define  MAX_TIMER_VALUE    0x8000          /* limited by embOS!        */
#define  OSIF_CURR_TASK     osif_get_current_task_id ()

#define  _OSIF_CHECK_TIMER_ID(timer_id, location)   \
                    if ( timer_id > MAX_TIM_ID )                                \
                    {                                                           \
                        _OS_EXCEPTION(ERR_TIM_INV, OSIF_CURR_TASK, location, 0);    \
                    }

#define  _OSIF_CHECK_TIMER_EXIST(timer_id, location)   \
                    if ( Osif_timer[timer_id].state ==  TIM_UNINITIALIZED)      \
                    {                                                           \
                        _OS_EXCEPTION(ERR_TIM_NEXIST, OSIF_CURR_TASK, location, 0); \
                    }


EXPORT_TYPEDEFS

LOCAL_TYPEDEFS
typedef enum _T_TIMER_STATE {
    TIM_UNINITIALIZED   = 0,
    TIM_CREATED,
    TIM_STARTED         = 0x10,
    TIM_STARTED1        = 0x20
} T_TIMER_STATE;

typedef enum _T_TIMER_SOURCE {
    TIM_UNKNOWN         = 0,
    TIM_OS              = 0x10,
    TIM_HW_T0           = 0x20,
    TIM_HW_T1,
    TIM_HW_T2,
    TIM_HW_T3,
    TIM_HW_T4,
    TIM_HW_T5,
    TIM_HW_T6,
    TIM_HW_T7,
    TIM_HW_TMAX         /* must be last !! */
} T_TIMER_SOURCE;

typedef struct _timer_info {
    OS_TIMER            timer;              /* embOS timer management    */
    USIGN8              id;                 /* Timer id                  */
    USIGN8              task_id;            /* Task to be signaled       */
    USIGN16             start_tim;          /* Timer start value         */
    USIGN32             sw_time;            /* Software cascade          */
    USIGN32             start_sw;           /* Cascade start value       */
    BOOL                cyclic;             /* Cyclic timer mode flag    */
    T_TIMER_STATE       state;              /* current timer state       */
    T_TIMER_SOURCE      source;
} T_TIMER_INFO;


FUNCTION_DECLARATIONS

FUNCTION LOCAL  VOID timer_handler (VOID);
FUNCTION extern VOID cif_get_system_time (T_SYS_TIME * sys_time);

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA

#include "da_osif.h"                              /* DATA SEGMENT DEFINITION */

NO_INIT static T_TIMER_INFO  Osif_timer[MAX_TIM_ID+1];
NO_INIT static USIGN8        Hw_tim_id[TIM_HW_TMAX - TIM_HW_T0];
NO_INIT static int           Timer_idx;

#include "da_def.h"                               /* DEFAULT DATA SEGMENT */

/******************************************************************************/

FUNCTION PUBLIC VOID osif_tim_init (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

possible return values:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    int i;

FUNCTION_BODY

    memset (Osif_timer, 0, sizeof (Osif_timer));
    memset (Hw_tim_id, 0, sizeof (Hw_tim_id));
    for (i=0; i<=MAX_TIM_ID; i++)
    {
        Osif_timer[i].state = TIM_UNINITIALIZED;
    }

    Tim_init = TRUE;
    Timer_idx = 0;

    return;
} /* FUNCTION osif_tim_init */


/******************************************************************************/

FUNCTION GLOBAL VOID hw_timer_handler (USIGN8 hw_timer)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Handles SW cascading of HW timers. Is used as within the timer interrupt handler,
restarts the HW timer, if selected by 'cyclic' operation mode, and sends the event
OS_IF_EVENT_TIMER to the conected task, when expired.


possible return values:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN8 timer_id;

FUNCTION_BODY
    timer_id = Hw_tim_id[hw_timer];

    if ( ((Osif_timer[timer_id].state & TIM_STARTED1) == 0) || (Osif_timer[timer_id].source < TIM_HW_T0) )
    {
        _OS_EXCEPTION(ERR_TIM_ERROR, 0, FKT_HOOK_TIM, timer_id);
    }

    if ( Osif_timer[timer_id].sw_time == 0 )
    {
        hw_tim_stop_timer (hw_timer);

        if ( Osif_timer[timer_id].cyclic == TRUE )
        {
            Osif_timer[timer_id].sw_time = Osif_timer[timer_id].start_sw;
            hw_tim_start_timer (hw_timer, Osif_timer[timer_id].start_tim);

        }
        else
        {
            Osif_timer[timer_id].state = TIM_CREATED;
        }
        osif_set_event (Osif_timer[timer_id].task_id, OS_IF_EVENT_TIMER);
    }
    else
    {
        Osif_timer[timer_id].sw_time--;
    }


    return;
} /* FUNCTION timer_handler */

/******************************************************************************/

FUNCTION LOCAL VOID timer_handler (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Handles SW cascading of embOS timers. Is used as callback function (via 'timer_x'),
retriggers the embOS timer, if needed, and calls the user callback function, when
expired.


possible return values:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN8 timer_id;

FUNCTION_BODY
    timer_id = ((T_TIMER_INFO *) OS_GetpCurrentTimer ())->id;

    if ( (Osif_timer[timer_id].state & (TIM_STARTED | TIM_STARTED1)) == 0 )
    {
        _OS_EXCEPTION(ERR_TIM_ERROR, 0, FKT_HOOK_TIM, timer_id);
    }

    if ( Osif_timer[timer_id].sw_time == 0 )
    {
        if ( Osif_timer[timer_id].cyclic == TRUE )
        {
            if ( Osif_timer[timer_id].start_sw == 0 )
            {
                OS_RetriggerTimer (&Osif_timer[timer_id].timer);
            }
            else
            {
                Osif_timer[timer_id].sw_time = Osif_timer[timer_id].start_sw;
                Osif_timer[timer_id].state   = TIM_STARTED1;
                OS_SetTimerPeriod (&Osif_timer[timer_id].timer, Osif_timer[timer_id].start_tim);
                OS_RetriggerTimer (&Osif_timer[timer_id].timer);
            }
        }
        else
        {
            Osif_timer[timer_id].state = TIM_CREATED;
        }
        osif_set_event (Osif_timer[timer_id].task_id, OS_IF_EVENT_TIMER);
    }
    else if ( Osif_timer[timer_id].state == TIM_STARTED1 )
    {
        OS_SetTimerPeriod (&Osif_timer[timer_id].timer, MAX_TIMER_VALUE - 1);
        OS_RetriggerTimer (&Osif_timer[timer_id].timer);
        Osif_timer[timer_id].sw_time--;
        Osif_timer[timer_id].state = TIM_STARTED;
    }
    else
    {
        OS_RetriggerTimer (&Osif_timer[timer_id].timer);
        Osif_timer[timer_id].sw_time--;
    }

    return;
} /* FUNCTION timer_handler */


/******************************************************************************/

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
{
LOCAL_VARIABLES
    USIGN8 tim_id;

FUNCTION_BODY

    CHECK_TASK_ID(task_id, FKT_CREATE_TIM);

        /* Get next available timer id */
        /*-----------------------------*/
    osif_disable_all_tasks ();
    if ( Tim_init == FALSE )
        osif_tim_init ();
    if ( (tim_id = Timer_idx++) > MAX_TIM_ID )
        _OS_EXCEPTION(ERR_TIM_OV, task_id, FKT_CREATE_TIM, 0);

    Osif_timer[tim_id].state   = TIM_CREATED;
    Osif_timer[tim_id].id      = tim_id;
    Osif_timer[tim_id].task_id = task_id;
    osif_enable_all_tasks ();

    switch (task_id)
    {
        case FB_TASK_ID:
            osif_disable_all_tasks ();
            Osif_timer[tim_id].source = TIM_HW_T0;
            Hw_tim_id[TIM_HW_T0 - TIM_HW_T0] = tim_id;
            osif_enable_all_tasks ();
            break;

        default:
        {
            osif_disable_all_tasks ();
            Osif_timer[tim_id].source = TIM_OS;
            OS_CreateTimer (&(Osif_timer[tim_id].timer), timer_handler, 100);
            osif_enable_all_tasks ();
            break;
        }
    } /* end switch */

    return (tim_id);

} /* FUNCTION osif_create_timer */


/******************************************************************************/

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
{
LOCAL_VARIABLES
    USIGN16 tim_val;
    USIGN32 casc_val;

FUNCTION_BODY
    _OSIF_CHECK_TIMER_ID(timer_id, FKT_START_TIM);

    osif_disable_all_tasks ();
    _OSIF_CHECK_TIMER_EXIST(timer_id, FKT_START_TIM);

    if ( timer_value == 0 )
    {
        osif_set_event (Osif_timer[timer_id].task_id, OS_IF_EVENT_TIMER);
    }
    else
    {
        Osif_timer[timer_id].state = TIM_STARTED1;
        Osif_timer[timer_id].cyclic = cyclic;

            /* Mapped to hardware timer ?? */
            /*-----------------------------*/
        if ( Osif_timer[timer_id].source >= TIM_HW_T0 )
        {
            casc_val = timer_value / MAX_HW_TIME;
            if ( (tim_val = (USIGN16) ((timer_value % MAX_HW_TIME) * HW_TICKS_PER_MS)) == 0 )
            {
                tim_val = HW_TIM_DEF_PERIOD - 1;
                casc_val--;
            }

            Osif_timer[timer_id].start_sw  = casc_val;
            Osif_timer[timer_id].start_tim = tim_val;

            Osif_timer[timer_id].sw_time = casc_val;
            hw_tim_start_timer (Osif_timer[timer_id].source - TIM_HW_T0, tim_val);
        }
        else
        {
            casc_val = timer_value / MAX_TIMER_VALUE;
            if ( (tim_val = (USIGN16) (timer_value % MAX_TIMER_VALUE)) == 0 )
            {
                tim_val = MAX_TIMER_VALUE;
                casc_val--;
            }

            Osif_timer[timer_id].start_sw  = casc_val;
            Osif_timer[timer_id].start_tim = tim_val;

            Osif_timer[timer_id].sw_time = casc_val;
            OS_SetTimerPeriod (&(Osif_timer[timer_id].timer), tim_val);
            OS_RetriggerTimer (&(Osif_timer[timer_id].timer));
        }
    }
    osif_enable_all_tasks ();

    return;
} /* FUNCTION osif_start_timer */


/******************************************************************************/

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
{
LOCAL_VARIABLES

FUNCTION_BODY
    osif_disable_all_tasks ();
    _OSIF_CHECK_TIMER_ID(timer_id, FKT_STOP_TIM);
    _OSIF_CHECK_TIMER_EXIST(timer_id, FKT_STOP_TIM);

    if ( Osif_timer[timer_id].source >= TIM_HW_T0 )
    {

        hw_tim_stop_timer (Osif_timer[timer_id].source - TIM_HW_T0);
    }
    else
    {
        OS_StopTimer (&Osif_timer[timer_id].timer);
    }

    Osif_timer[timer_id].state = TIM_CREATED;
    osif_enable_all_tasks ();

    return;
} /* FUNCTION osif_stop_timer */


/******************************************************************************/

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
{
LOCAL_VARIABLES

FUNCTION_BODY

    cif_get_system_time (sys_time);
    return;
} /* FUNCTION osif_get_system_time */

/******************************************************************************/

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
{
LOCAL_VARIABLES

    USIGN32  time_in_ms;

FUNCTION_BODY

#ifdef OSIF_EMBOS
    time_in_ms = OS_GetTime32();
#else
    #error OS function to calculate the elapsed time in ms is missing!
#endif

    return(time_in_ms);
} /* FUNCTION osif_get_time_in_ms */


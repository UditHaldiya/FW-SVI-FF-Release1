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

FILE_NAME          osif_cfg.c



=========================================================================== */
#include "keywords.h"
#define  MODULE_ID      (COMP_OSIF + MOD_OSIFCFG)

INCLUDES
#include "base.h"

#include "osif.h"           /* OSIF interface declarations      */
#include "osif_cfg.h"       /* configuration data structures    */


LOCAL_DEFINES
#define TASK_STACK(stack_area)  \
                sizeof (stack_area), stack_area

LOCAL_TYPEDEFS


LOCAL_DATA

#include "da_osstk.h"                            /* DATA SEGMENT DEFINITION */

#if defined (PROTOCOL_FF)

/*--------------------------------------------------------------------------*/
/* Tasks specific to FF-BFD (+ LM) communication layer                      */
/*--------------------------------------------------------------------------*/

#ifdef LINK_MASTER

/* LAS task --------------------------------------------------------------- */
#define LAS_EVENTS              OS_IF_EVENT_INDCON
#define LAS_MSGEVT              0
#define LAS_MSGNUM              0
#define LAS_STACK_SIZE          0x100
NO_INIT STATIC char las_stack[LAS_STACK_SIZE];

#endif /* LINK_MASTER */

/* FDC task --------------------------------------------------------------- */
#if defined (MODBUS_MASTER)
#define FDC_EVENTS              OS_IF_EVENT_REQRES + OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM + MODB_USR_EVENT + OS_IF_EVENT_RESUME
#define FDC_MSGEVT              OS_IF_EVENT_EEPROM + MODB_USR_EVENT
#elif defined (HART_MASTER)
#define FDC_EVENTS              OS_IF_EVENT_REQRES + OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM + OS_IF_EVENT_RESUME + HART_ACY_CMD_END_EVENT
#define FDC_MSGEVT              OS_IF_EVENT_EEPROM
#else
#define FDC_EVENTS              OS_IF_EVENT_REQRES + OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM + OS_IF_EVENT_RESUME
#define FDC_MSGEVT              OS_IF_EVENT_EEPROM
#endif
#define FDC_MSGNUM              5
#define FDC_STACK_SIZE          0x300
NO_INIT STATIC char fdc_stack[FDC_STACK_SIZE];

/* SUBSCRIBER task ------------------------------------------------------- */
#define SBSCR_EVENTS            OS_IF_EVENT_INDCON
#define SBSCR_MSGEVT            0
#define SBSCR_MSGNUM            0
#define SBSCR_STACK_SIZE        0x200
NO_INIT STATIC char sbscr_stack[SBSCR_STACK_SIZE];

#ifdef MVC_OBJECTS /* MVC task -------------------------------------------- */
#define MVC_EVENTS              OS_IF_EVENT_REQRES
#define MVC_MSGEVT              0
#define MVC_MSGNUM              0
#define MVC_STACK_SIZE          0x100
NO_INIT STATIC char mvc_stack[MVC_STACK_SIZE];
#endif /* MVC_OBJECTS */


/* BGD50 task: timer triggered background actions ------------------------- */
#define BGD50_EVENTS          OS_IF_EVENT_TIMER + OS_IF_EVENT_EEPROM
#define BGD50_MSGEVT          OS_IF_EVENT_EEPROM
#define BGD50_MSGNUM          2
#define BGD50_STACK_SIZE      0x280
NO_INIT STATIC char bgd50_stack[BGD50_STACK_SIZE];

/* BGD task: background actions ------------------------------------------- */
#define BGD_EVENTS               OS_IF_EVENT_TIMER + OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM
#define BGD_MSGEVT               OS_IF_EVENT_EEPROM
#define BGD_MSGNUM               2
#define BGD_STACK_SIZE           0x800 //4x original size
NO_INIT STATIC char bgd_stack[BGD_STACK_SIZE];

#elif defined (PROTOCOL_PA) || defined (PROTOCOL_DP)

/*--------------------------------------------------------------------------*/
/* Tasks specific to PA-Slave communication layer                           */
/*--------------------------------------------------------------------------*/
/* DPS task:  ------------------------------------------------------------- */
#define DPS_EVENTS          OS_IF_EVENT_REQRES + OS_IF_EVENT_INDCON + OS_IF_EVENT_TIMER + OS_IF_EVENT_UPDATE_IO
#define DPS_MSGEVT          OS_IF_EVENT_REQRES + OS_IF_EVENT_INDCON
#define DPS_MSGNUM          30
#define DPS_STACK_SIZE      0x280
NO_INIT STATIC char dps_stack[DPS_STACK_SIZE];

/* DUSR task:  ------------------------------------------------------------ */
#define DUSR_EVENTS          OS_IF_EVENT_REQRES + OS_IF_EVENT_INDCON + OS_IF_EVENT_TIMER + OS_IF_EVENT_RESUME
#define DUSR_MSGEVT          OS_IF_EVENT_REQRES + OS_IF_EVENT_INDCON
#define DUSR_MSGNUM          10
#define DUSR_STACK_SIZE      0x100
NO_INIT STATIC char dusr_stack[DUSR_STACK_SIZE];

/* FBS task --------------------------------------------------------------- */
#if defined (MODBUS_MASTER)
#define FBS_EVENTS           OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM + OS_IF_EVENT_TIMER + OS_IF_EVENT_RESUME
#define FBS_MSGEVT           OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM + MODB_USR_EVENT
#elif defined (HART_MASTER)
#define FBS_EVENTS           OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM + OS_IF_EVENT_TIMER + OS_IF_EVENT_RESUME + HART_ACY_CMD_END_EVENT
#define FBS_MSGEVT           OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM
#else
#define FBS_EVENTS           OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM + OS_IF_EVENT_TIMER + OS_IF_EVENT_RESUME
#define FBS_MSGEVT           OS_IF_EVENT_INDCON + OS_IF_EVENT_EEPROM
#endif
#define FBS_MSGNUM           10
#define FBS_STACK_SIZE       0x400
NO_INIT STATIC char fbs_stack[FBS_STACK_SIZE];

/* BGD task: background actions ------------------------------------------- */
#define BGD_EVENTS           OS_IF_EVENT_TIMER + OS_IF_EVENT_EEPROM
#define BGD_MSGEVT           OS_IF_EVENT_EEPROM
#define BGD_MSGNUM           2
#define BGD_STACK_SIZE       0x280
NO_INIT STATIC char bgd_stack[BGD_STACK_SIZE];

#else

#error Protocol type (PROTOCOL_FF or PROTOCOL_PA) not defined!

#endif /* Protocol type */

/*--------------------------------------------------------------------------*/
/* Other system tasks                                                       */
/*--------------------------------------------------------------------------*/
/* EEP task: contains the asynchrounous part of the EEP interface --------- */
#define EEP_EVENTS          OS_IF_EVENT_EEPROM + OS_IF_EVENT_EEPROG + OS_IF_EVENT_TIMER
#define EEP_MSGEVT          OS_IF_EVENT_EEPROM
#define EEP_MSGNUM          (15)  //3x the original
#define EEP_STACK_SIZE      (0x600) //3x the original
NO_INIT STATIC char eep_stack[EEP_STACK_SIZE];

/*--------------------------------------------------------------------------*/
/* Function Block execution task(s)                                         */
/*--------------------------------------------------------------------------*/
/* FB tasks: each Function Block runs as a separate task ------------------ */
#define FB_EVENTS           OS_IF_EVENT_TIMER + OS_IF_EVENT_EEPROM
#define FB_MSGEVT           OS_IF_EVENT_EEPROM
#define FB_MSGNUM           5
#define FB_STACK_SIZE       0x300
NO_INIT STATIC char fb1_stack[FB_STACK_SIZE];

/* @Dresser-Masoneilan project: Watchdog Task not required, because external Watchdog is used. */
#if 0
/* WD task ---------------------------------------------------------------- */
#define WD_EVENTS               OS_IF_EVENT_TIMER
#define WD_MSGEVT               0
#define WD_MSGNUM               0
#define WD_STACK_SIZE           0x100
NO_INIT STATIC char wd_stack[WD_STACK_SIZE];
#endif

/*@Dresser-Masoneilan project modified */
//#if defined (FBK2_HW) && defined (DOWNLOAD_DEVICE)
#if defined (SW_DOWNLOAD)
/*--------------------------------------------------------------------------*/
/* Software Download task(s)                                                */
/*--------------------------------------------------------------------------*/
#define SWDL_EVENTS              OS_IF_EVENT_TIMER + OS_IF_EVENT_REQRES + OS_IF_EVENT_RESUME
#define SWDL_MSGEVT              0
#define SWDL_MSGNUM              5
#define SWDL_STACK_SIZE          0x300
#if 0
NO_INIT STATIC char swdl_stack[SWDL_STACK_SIZE];
#endif
#endif /* FBK2_HW && DOWNLOAD_DEVICE */


/*--------------------------------------------------------------------------*/
/* Application task(s)                                                      */
/*--------------------------------------------------------------------------*/
#if defined (HART_MASTER) || defined (MODBUS_MASTER)
#define APPL_EVENTS             OS_IF_EVENT_TIMER  + OS_IF_EVENT_EEPROM + OS_IF_EVENT_INDCON + OS_IF_EVENT_RESUME
#else
#define APPL_EVENTS             OS_IF_EVENT_TIMER + OS_IF_EVENT_EEPROM
#endif
#define APPL_MSGEVT             OS_IF_EVENT_EEPROM
#define APPL_MSGNUM             5
#define APPL_STACK_SIZE         0x300
NO_INIT STATIC char appl_stack[APPL_STACK_SIZE];


#if defined (HART_MASTER)
/*--------------------------------------------------------------------------*/
/* HART Master task(s)                                                      */
/*--------------------------------------------------------------------------*/
#define HM_EVENTS           HART_START_EVENT +                                              \
                            OS_IF_EVENT_EEPROM + OS_IF_EVENT_TIMER + OS_IF_EVENT_REQRES +   \
                            HART_RECEIVE_END_EVENT + HART_SEND_END_EVENT + HART_ERROR_EVENT
#define HM_MSGEVT           OS_IF_EVENT_EEPROM
#define HM_MSGNUM           2
#define HM_STACK_SIZE       0x500
NO_INIT STATIC char hm_stack[HM_STACK_SIZE];

#endif /* HART_MASTER */

#if defined (MODBUS_MASTER)
/*--------------------------------------------------------------------------*/
/* Modbus Master task(s)                                                    */
/*--------------------------------------------------------------------------*/
#define MODB_EVENTS             MODB_USR_EVENT + MODB_RCV_END_EVENT + MODB_XMT_END_EVENT + \
                                MODB_TIMEOUT_EVENT + OS_IF_EVENT_TIMER + MODB_START_EVENT
#define MODB_MSGEVT             OS_IF_EVENT_EEPROM + MODB_USR_EVENT
#define MODB_MSGNUM             5
#define MODB_STACK_SIZE         0x300
NO_INIT static char modb_stack[MODB_STACK_SIZE];

#endif /* MODBUS_MASTER */


#include "da_def.h"                                 /* DEFAULT DATA SEGMENT */

EXPORT_DATA

#include "cs_osif.h"                         /* CONSTANT SEGMENT DEFINITION */
const TASK_CFG Task_cfg[MAX_TASK_NUM] =
{
/*   TASK_ID      , TASK_PRIO      , NUM_MSG     , EVENTS      , MSG_EVTS    , STACKSIZE + STACKAREA  , MAIN_FKT, NAME           */
#if defined (PROTOCOL_FF)
#ifdef LINK_MASTER
    {LAS_TASK_ID  , LAS_TASK_PRIO  , LAS_MSGNUM  , LAS_EVENTS  , LAS_MSGEVT  , TASK_STACK(las_stack)  , NULL    , "LAS Task"     },
#endif /* LINK_MASTER */
    {FDC_TASK_ID  , FDC_TASK_PRIO  , FDC_MSGNUM  , FDC_EVENTS  , FDC_MSGEVT  , TASK_STACK(fdc_stack)  , NULL    , "FDC Task"     },
    {SBSCR_TASK_ID, SBSCR_TASK_PRIO, SBSCR_MSGNUM, SBSCR_EVENTS, SBSCR_MSGEVT, TASK_STACK(sbscr_stack), NULL    , "SBSCR Task"   },
#ifdef MVC_OBJECTS
    {MVC_TASK_ID,   MVC_TASK_PRIO,   MVC_MSGNUM,   MVC_EVENTS,   MVC_MSGEVT,   TASK_STACK(mvc_stack),   NULL    , "MVC Task"     },
#endif /* MVC_OBJECTS */
    {BGD50_TASK_ID, BGD50_TASK_PRIO, BGD50_MSGNUM, BGD50_EVENTS, BGD50_MSGEVT, TASK_STACK(bgd50_stack), NULL    , "BGD50 Task"   },
#elif defined (PROTOCOL_PA) || defined (PROTOCOL_DP)
    {DPS_TASK_ID  , DPS_TASK_PRIO  , DPS_MSGNUM  , DPS_EVENTS  , DPS_MSGEVT  , TASK_STACK(dps_stack)  , NULL    , "DPS Task"     },
    {DUSR_TASK_ID , DUSR_TASK_PRIO , DUSR_MSGNUM , DUSR_EVENTS , DUSR_MSGEVT , TASK_STACK(dusr_stack) , NULL    , "DUSR Task"    },
    {FBS_TASK_ID  , FBS_TASK_PRIO  , FBS_MSGNUM  , FBS_EVENTS  , FBS_MSGEVT  , TASK_STACK(fbs_stack)  , NULL    , "FBS Task"     },
#endif /* Protocol type */
    {BGD_TASK_ID  , BGD_TASK_PRIO  , BGD_MSGNUM  , BGD_EVENTS  , BGD_MSGEVT  , TASK_STACK(bgd_stack)  , NULL    , "BGD Task"     },
    {EEP_TASK_ID  , EEP_TASK_PRIO  , EEP_MSGNUM  , EEP_EVENTS  , EEP_MSGEVT  , TASK_STACK(eep_stack)  , NULL    , "EEP Task"     },
    {FB_TASK_ID   , FB_TASK_PRIO   , FB_MSGNUM   , FB_EVENTS   , FB_MSGEVT   , TASK_STACK(fb1_stack)  , NULL    , "FB Exec Task" },
    /* @Dresser-Masoneilan project: Watchdog Task not required, because external Watchdog is used. */
    //{FDC_WD_TASK_ID , FDC_WD_TASK_PRIO , WD_MSGNUM , WD_EVENTS , WD_MSGEVT   , TASK_STACK(wd_stack)   , NULL    , "WD Task"      },
/*@Dresser-Masoneilan project modified */
#if 0
#if defined (FBK2_HW) && defined (DOWNLOAD_DEVICE)
    {FBK2_DWNLD_HANDLER_TASK_ID , FBK2_DWNLD_HANDLER_TASK_PRIO , SWDL_MSGNUM , SWDL_EVENTS , SWDL_MSGEVT , TASK_STACK(swdl_stack) , NULL  , "SWDL Task"    },
#endif /* DOWNLOAD_DEVICE */
#endif
#if defined (HART_MASTER)
    {HM_TASK_ID   , HM_TASK_PRIO   , HM_MSGNUM   , HM_EVENTS   , HM_MSGEVT   , TASK_STACK(hm_stack)   , NULL    , "HM Task"      },
#endif /* HART_MASTER */
#if defined (MODBUS_MASTER)
    {MODB_TASK_ID , MODB_TASK_PRIO , MODB_MSGNUM , MODB_EVENTS , MODB_MSGEVT , TASK_STACK(modb_stack) , NULL    , "Modbus Task"  },
#endif /* MODBUS_MASTER */
    {APPL_TASK_ID , APPL_TASK_PRIO , APPL_MSGNUM , APPL_EVENTS , APPL_MSGEVT , TASK_STACK(appl_stack) , NULL    , "APPL Task"    }
};
#include "cs_def.h"                           /* DEFAULT CONSTANT SEGMENT */

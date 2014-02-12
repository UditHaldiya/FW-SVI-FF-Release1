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

FILE_NAME          main.c



FUNCTIONAL_MODULE_DESCRIPTION

Contains the 'main' function, if no OS is used

=========================================================================== */
#include "keywords.h"   /* keyword defines */
#define  MODULE_ID      (COMP_OSIF + MOD_OSIFMAIN)

INCLUDES
#include "base.h"
#include "hw_cfg.h"
#include "hw_if.h"
#include "osif_cfg.h"
#include "osif_int.h"

#include "eep_if.h"

#include "except.h"
#include "dbg_out.h"

#if defined (HART_MASTER)
  #include "hm_cfg.h"
  #include "hm_if.h"
#endif /* HART_MASTER */

#if defined (MODBUS_MASTER)
  #include "mm_cfg.h"
  #include "mm_if.h"
#endif /* MODBUS_MASTER */

#include "reset.h"
#include "mnassert.h"
#include "mntrapdef.h"
#if 0
    #include "oswrap.h"
#endif

GLOBAL_DEFINES

LOCAL_DEFINES

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
extern GLOBAL VOID init_interrupts (VOID);

extern GLOBAL VOID osif_init (VOID);
extern GLOBAL VOID fbs_init (VOID);
extern GLOBAL VOID appl_init (VOID);
extern GLOBAL VOID cif_init (VOID);


EXPORT_DATA

#include "da_except.h"                           /* DATA SEGMENT DEFINITION */
NO_INIT T_XCP_DSCR xcp_dscr;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

IMPORT_DATA

LOCAL_DATA


#if defined (COMPILER_IAR)
  #if !defined(CPU_TYPE_78K4)
    #if __VER__ < 200
      #pragma function=C_task
    #else
      /* 2.11a compiler does not support this feature */
    #endif
  #endif
#endif /* COMPILER_IAR */

/******************************************************************************/

FUNCTION GLOBAL main (void)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used as main-function if OSIF is not used

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);
extern unsigned long *__int_vector_table;

    NVIC_SetVectorTable((uint32_t)&__int_vector_table, 0);

    #ifndef PROCESSOR_INIT_IN_STARTUP
    init_processor ();
    #endif

    init_interrupts ();
    DBG_INIT;

        /* First of all initialize OS and according components */
        /*-----------------------------------------------------*/
    OS_InitKern();      /* initialize OS (should be first !)                */
    OS_InitHW();        /* initialize Hardware for OS (see RTOSInit.c)      */
#if 0
    idle_EstimateTime();
#endif
    DBG_OUT_WAIT_("\n\n\n***\n");
    DBG_OUT_WAIT_("osif_init\n");
    osif_init ();       /* Initialize OSIF                                  */
                        /* Must be done before any task is created!!        */

        /* Initialisation phase */
        /*----------------------*/
    DBG_OUT_WAIT_("eep_init\n");
    eep_CreateMutex();
    eep_init ();        /* Initialize EEPROM interface                      */
    reset_OnReset();

    Reset_Watchdog();

    DBG_OUT_WAIT_("cif_init\n");
    cif_init ();        /* Initialize communication layers                  */
    #ifndef DOWNLOAD_DEVICE /* -------------------------------------------- */
    DBG_OUT_WAIT_("fbs_init\n");
    fbs_init ();       /* Initialize function block shell                   */
    #endif /* ! DOWNLOAD_DEVICE */
    DBG_OUT_WAIT_("appl_init\n");
    appl_init ();       /* Initialize application layers                    */

    Reset_Watchdog();

#if 0 //delayed until app init
#if defined (HART_MASTER)
    DBG_OUT_WAIT_("HART_master_init\n");
    hm_init ();        /* Initialize HART master                            */
#endif /* HART_MASTER */

#if defined (MODBUS_MASTER)
    DBG_OUT_WAIT_("Modbus_master_init\n");
    modb_init ();      /* Initialize Modbus master                          */
#endif /* MODBUS_MASTER */
#endif

    DBG_OUT_WAIT_("OS_Start\n");

//    __enable_interrupt ();

    OS_Start();         /* Start multitasking (no return from here)         */
}


#ifdef COMPILER_IAR
  #if __VER__ < 200
    #pragma function=default
  #endif
#endif /* COMPILER_IAR */

/*****************************************************************************/
/*** Exception handling                                                    ***/
/*****************************************************************************/

FUNCTION GLOBAL VOID default_exception_handler
  (
    T_XCP_TYPE          xcp_type,
    USIGN16             task_id,
    USIGN16             module_id,
    T_XCP_DSCR *        xcp_dscr
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

possible return values:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY
    OS_SDI();

    switch (xcp_type) {
    case XCP_FATAL:
        DBG_OUT_("\n\n!! Fatal Error !!\n");
        DBG_FLUSH_NOINT;
        mn_assert((void *)((module_id<<16) | xcp_dscr->xcp_code), -TRAPID_FIQ); //steal TRAPID_FIQ for fatal error
        break;

    case XCP_ASSERTION:
        DBG_OUT_("\n\n** Assertion failed:\n");
        DBG_FLUSH_NOINT;
        mn_assert(xcp_dscr->u.assertion.filename, xcp_dscr->u.assertion.line);
        break;

    case XCP_ERROR:
        DBG_OUT_("\n\n** Error:\n");
        DBG_FLUSH_NOINT;
        break;

    case XCP_WARNING:
        DBG_OUT_NOWAIT_("\n\n** Warning:\n");
        break;
    } /* end switch (xcp_type) */

    DBG_PRINT_( ("   Task-ID = 0x%x, Module-ID = 0x%x\n", task_id, module_id) );


    switch (xcp_dscr->xcp_code) {
    case XCP_ASSERT:
        DBG_PRINT_( ("   Assertion in file %s, line %d\n", xcp_dscr->u.assertion.filename, xcp_dscr->u.assertion.line) );
        break;

    case XCP_DPS:
    case XCP_DUSR:
    case XCP_PFBS:
    case XCP_PFBL:
        DBG_PRINT_( ("   Comp = DPS/DUSR/FBS/FBLK,  reason = 0x%x, detail 1 = 0x%lx, detail 2 = 0x%lx\n", xcp_dscr->u.dps.reason, xcp_dscr->u.dps.detail_1, xcp_dscr->u.dps.detail_2) );
        DBG_PRINT_( ("   file = %s, line %d\n", xcp_dscr->u.dps.filename, xcp_dscr->u.dps.line) );
        break;

    case XCP_FDL:
        DBG_PRINT_( ("   Comp = FDL,  reason = 0x%x, detail = 0x%lx\n", xcp_dscr->u.fdl.reason, xcp_dscr->u.fdl.detail) );
        break;

    case XCP_DLL:
        DBG_PRINT_( ("   Comp = DLL,  reason = 0x%x, detail = 0x%lx\n", xcp_dscr->u.fdl.reason, xcp_dscr->u.fdl.detail) );
        break;

    case XCP_HW:
        DBG_PRINT_( ("   Comp = HW,  reason = 0x%x, detail = 0x%lx\n", xcp_dscr->u.hw.reason, xcp_dscr->u.hw.detail) );
        break;

    case XCP_EEP:
        DBG_PRINT_( ("   Comp = EEP,  reason = 0x%x, block_id = %d, detail = 0x%lx\n", xcp_dscr->u.eep.reason, xcp_dscr->u.eep.block_id, xcp_dscr->u.eep.detail) );
        break;

    case XCP_OS:
        DBG_PRINT_( ("   Comp = OS,  reason = 0x%x, location = 0x%x, detail = 0x%lx\n", xcp_dscr->u.os.reason, xcp_dscr->u.os.location, xcp_dscr->u.os.detail) );
        break;

    case XCP_FAPP:
        DBG_PRINT_( ("   Comp = FF-APPL,  reason = 0x%x, location = 0x%x, detail = 0x%lx\n", xcp_dscr->u.fapp.reason, xcp_dscr->u.fapp.location, xcp_dscr->u.fapp.detail) );
        break;

    default:
        DBG_PRINT_( ("   Code = 0x%x; no more information available\n", xcp_dscr->xcp_code) );
        break;
    } /* end switch (xcp_code) */

    if ( xcp_type != XCP_WARNING ) {
        DBG_FLUSH_NOINT;
    }
    else
    {
        DBG_FLUSH_NOWAIT;
    }

#if defined (PD30) /* || defined (KD30) */
    if ( xcp_type != XCP_WARNING ) {
        //break_instruction ();
    }
#endif /* EMULATOR */

    if ( xcp_type == XCP_FATAL ) {          /* no survival possible -> restart  */
        DBG_OUT_("!!\n\n");
        DBG_FLUSH_NOINT;
        Reset_CPU ();
    }

    OS_RI();

    DBG_OUT_("**\n\n");
    DBG_FLUSH_NOWAIT;

    _UNREF_PARAM (task_id);
    _UNREF_PARAM (module_id);

}



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

FILE_NAME          dbg_out.h



DESCRIPTION        This file contains the global function declarations and
                   the app. access macros of component DBG

=========================================================================== */
#ifndef __DBG_GLB__
#define __DBG_GLB__

#ifdef DEBUG_STRING

#if defined(OSIF_EMBOS) && (defined(OS_LIBMODE_DT) || defined(OS_LIBMODE_DP) || defined(OS_LIBMODE_SP))

#define DBG_INIT

#define DBG_OUT_(string)        \
    SendDebugString (string)

#define DBG_OUT_NOWAIT_(string) \
    SendDebugString (string)

#define DBG_OUT_WAIT_(string)   \
    SendDebugString (string)

#define DBG_FLUSH_NOWAIT

#define DBG_FLUSH_NOINT

#define DBG_FLUSH_WAIT

#define DBG_PRINT_(x)           \
    {                           \
    DebugPrintf x;              \
    }

#define DBG_PRINT_WAIT_(x)      \
    {                           \
    DebugPrintf x;              \
    }

#define DBG_PRINT_NOWAIT_(x)    \
    {                           \
    DebugPrintf x;              \
    }


#else

#define DBG_INIT                \
    dbg_init ()

#define DBG_OUT_(string)        \
    SendDebugString (string)

#define DBG_OUT_NOWAIT_(string) \
    {                           \
    SendDebugString (string);   \
    FlushDebugBuffer ();        \
    }    

#define DBG_OUT_WAIT_(string)   \
    {                           \
    SendDebugString (string);   \
    FlushDebugBuffer ();        \
    AwaitFlushEnd ();           \
    }

#define DBG_FLUSH_NOWAIT        \
    FlushDebugBuffer ()

#define DBG_FLUSH_NOINT         \
    FlushDebugBuffer_NoInt ()

#define DBG_FLUSH_WAIT          \
    {                           \
    FlushDebugBuffer ();        \
    AwaitFlushEnd ();           \
    }

#define DBG_PRINT_(x)           \
    {                           \
    DebugPrintf x;              \
    }

#define DBG_PRINT_WAIT_(x)      \
    {                           \
    DebugPrintf x;              \
    FlushDebugBuffer ();        \
    AwaitFlushEnd ();           \
    }

#define DBG_PRINT_NOWAIT_(x)    \
    {                           \
    DebugPrintf x;              \
    FlushDebugBuffer ();        \
    }

#endif /* OSIF_EMBOS */

/*****************************************************************************/
/* global function declarations from file:    dbg_out.c                      */
/*****************************************************************************/


FUNCTION extern VOID SendDebugString (BYTE *str)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function prints a string to the debug buffer

possible-return-codes:
- NONE
-----------------------------------------------------------------------------*/
;



FUNCTION extern VOID DebugPrintf (char *format, ...)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function prints to the debug buffer like printf

possible-return-codes:
- NONE
-----------------------------------------------------------------------------*/
;



FUNCTION extern VOID FlushDebugBuffer_NoInt(VOID)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function sends the yet unwritten part of the debug buffer to the
serial interface.

CAUTION! The serial interface in debug mode outputs the data using polling,
not interrupts. The call to this function is blocking, until all data is
transferred.

possible-return-codes:
- NONE
-----------------------------------------------------------------------------*/
;



FUNCTION extern VOID FlushDebugBuffer(VOID)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function sends the yet unwritten part of the debug buffer to the
serial interface.

CAUTION! The serial interface in debug mode outputs the data using polling,
not interrupts. The call to this function is blocking, until all data is
transferred.

possible-return-codes:
- NONE
-----------------------------------------------------------------------------*/
;



FUNCTION extern VOID AwaitFlushEnd(VOID)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Waits, until all chars are transmitted to the debug port

possible-return-codes:
- NONE
-----------------------------------------------------------------------------*/
;



FUNCTION extern VOID dbg_init(VOID)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Initialize Debug-Information

possible-return-codes:
- NONE
-----------------------------------------------------------------------------*/
;

#else

#define DBG_INIT
#define DBG_OUT_(string)
#define DBG_OUT_NOWAIT_(string)
#define DBG_OUT_WAIT_(string)
#define DBG_FLUSH_NOWAIT
#define DBG_FLUSH_NOINT
#define DBG_FLUSH_WAIT
#define DBG_PRINT_(x)
#define DBG_PRINT_NOWAIT_(x)
#define DBG_PRINT_WAIT_(x)

#endif /* DEBUG_STRING */


#ifdef DEBUG_TRACE

#ifndef TRACE_VAR
#define TRACE_VAR       extern
#endif /* TRACE_VAR */

TRACE_VAR USIGN16 Trc_mod;

#define TRC_INIT                    \
    trc_init

#define TRC_OUT_(x)                 \
    { trc_string (MODULE_ID, x); }


#define TRC_PRINT_(x)               \
    {                               \
        Trc_mod = MODULE_ID;        \
        trc_printf x;               \
    }

FUNCTION extern GLOBAL VOID trc_put_string (USIGN16 module_id, BYTE *str);
FUNCTION extern GLOBAL VOID trc_printf (char *format, ...);
FUNCTION extern GLOBAL VOID trc_init (USIGN8 * buffer, USIGN16 size, USIGN16 modules, USIGN16 mask);

#else

#define TRC_INIT
#define TRC_OUT_(x)
#define TRC_PRINT_(x)

#endif /* DEBUG_TRACE */



#endif /* __DBG_GLB__ */


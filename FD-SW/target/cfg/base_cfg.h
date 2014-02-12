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

FILE_NAME          base_cfg.h



DESCRIPTION        This file contains the Softing basic data types, CPU type
                   selection, compiler selection and specific defines.

=========================================================================== */
#ifndef __BASE_CFG_H__
#define __BASE_CFG_H__


/* ------------------------------------------------------------------------- */
/* Lint related macros                                                       */
/* ------------------------------------------------------------------------- */

#define _DISABLE_LINT_FOR_SEGM_HDR_FILE                                      \
          /*lint -efile(451,.) */  /* Disable warning 451: Header file is    \
                                      repeatedly included but does not       \
                                      have a standard include guard       */ \
                                                                             \
          /*lint -efile(766,.) */  /* Disable info 766: Header file is not   \
                                      used in this module                 */

#define _ADAPT_LINT_BEFORE_M16C_HDR                                          \
          /*lint -e104 */           /* Disable error 104: Conflicting types */

#define _RESTORE_LINT_AFTER_M16C_HDR                                         \
          /*lint +e104 */           /* Enable error 104: Conflicting types */


#define _ADAPT_LINT_BEFORE_RTOS_H                                            \
          /*lint +fem  */          /* Disable error 140: 'Type appears after \
                                      modifier'. +fem allows early modifiers */

#define _RESTORE_LINT_AFTER_RTOS_H                                           \
          /*lint -fem  */          /* Enable error 140: 'Type appears after  \
                                      modifier'.  */

#ifdef _lint /* The following function is used for PC-lint check only       */
  /*lint -ecall(119,pc_lint_unref_param) */
  void pc_lint_unref_param (void x)
  {
    return;
  }
#endif /* _lint */

/* ------------------------------------------------------------------------- */
/* ---   Compiler selection                                              --- */
/* ------------------------------------------------------------------------- */

#define COMPILER_IAR                                    /* IAR compiler      */

#include "cpu.h"                                        /* processor defines */


/* ------------------------------------------------------------------------- */
/* ---   Enable power-fail IRQ handing                                   --- */
/* ------------------------------------------------------------------------- */

// #define USE_POWERFAIL_IRQ                    /* Power fail interrupt is used */

/* ------------------------------------------------------------------------- */
/* ---   Memory, Code and Const segments for IAR compiler                --- */
/* ---   Used to specify specific segments. Compiler specific!           --- */
/* ------------------------------------------------------------------------- */

#define IAR_CODE_SEGM
#define IAR_DATA_SEGM
#define IAR_CONST_SEGM


/* ------------------------------------------------------------------------- */
/* --- Section to map compiler specific keywords to common keywords.     --- */
/* ------------------------------------------------------------------------- */

#ifdef COMPILER_IAR
#if __VER__ < 200
#define FORCE_REF
#define NO_INIT
#define INTERRUPT               interrupt
#else
#define FORCE_REF               __root
#define NO_INIT                 __no_init
#define INTERRUPT               __interrupt
#endif
#else
#define FORCE_REF
#define NO_INIT
#define INTERRUPT
#endif /* COMPILER_IAR */

#ifdef COMPILER_GNU
#define PACKED  __attribute__ ((packed))
#else
#define PACKED
#endif /* COMPILER_GNU */

#define NEAR_C
/*@Dresser-Masoneilan project modified: No NEAR and FAR distinction with 32bit addressing */
//#define NEAR_D                  near
#define NEAR_D
#define FAR_C
/*@Dresser-Masoneilan project modified: No NEAR and FAR distinction with 32bit addressing */
//#define FAR_D                   far
#define FAR_D


/* ------------------------------------------------------------------------- */
/* --- Define memory specifiers if internal or external RAM is used      --- */
/* ------------------------------------------------------------------------- */

#ifdef IAR_DATA_SEGM
  #define FBIF_DATA_MEM
  #define PBIF_DATA_MEM
  #define FBS_DATA_MEM
  #define FDC_DATA_MEM
  #define MGMT_DATA_MEM
  #define FAL_DATA_MEM
  #define OS_DATA_MEM
  #define DLL_DATA_MEM
  #define FDL_DATA_MEM
#else /* not IAR_DATA_SEGM */
  #define FBIF_DATA_MEM           FAR_D
  #define PBIF_DATA_MEM           FAR_D
  #define FBS_DATA_MEM            FAR_D
  #define FDC_DATA_MEM            FAR_D
  #define MGMT_DATA_MEM           FAR_D
  #define FAL_DATA_MEM            FAR_D
  #define OS_DATA_MEM             NEAR_D
  #define DLL_DATA_MEM            NEAR_D
  #define FDL_DATA_MEM            NEAR_D
#endif /* not IAR_DATA_SEGM */


/* ------------------------------------------------------------------------- */
/* --- Scope of functions -------------------------------------------------- */
/* ------------------------------------------------------------------------- */
#define GLOBAL
#define PUBLIC                  NEAR_C
#define LOCAL                   static NEAR_C
//note: math.h defines HUGE too...
//#define HUGE                    huge


/* ------------------------------------------------------------------------- */
/* --- Scope of variables -------------------------------------------------- */
/* ------------------------------------------------------------------------- */

#ifdef _DEBUG
  #define STATIC
#else
  #define STATIC                static
#endif /* _DEBUG */


/* ------------------------------------------------------------------------- */
/* --- CPU / compiler dependent data types --------------------------------- */
/* ------------------------------------------------------------------------- */

#define FLAG                    bit


/* ------------------------------------------------------------------------- */
/* --- Heap data size                                                    --- */
/* ------------------------------------------------------------------------- */

#define MALLOC_BUFSIZE          0x7FF0  /* buffer size for memory allocation */


/* ------------------------------------------------------------------------- */
/* --- Used to map compiler specific interrupt keywords                  --- */
/* ------------------------------------------------------------------------- */

#define INTR_HANDLER            INTERRUPT VOID


/* ------------------------------------------------------------------------- */
/* --- For command sequence which must not be disturbed by interrupt     --- */
/* ------------------------------------------------------------------------- */

#define atomic_cmd(command)     asm("PUSHC FLG"); \
                                asm("FCLR  I");   \
                                command;          \
                                asm("POPC  FLG")


/* ------------------------------------------------------------------------- */
/* --- Byte and word access to constants and register objects            --- */
/* ------------------------------------------------------------------------- */

#define CWLB_(Id)               ((USIGN8) (Id))
#define CWHB_(Id)               ((USIGN8) ((Id) >> 8))
#define CDWLLB_(Id)             ((USIGN8) (Id))
#define CDWLHB_(Id)             ((USIGN8) ((Id) >> 8))
#define CDWHLB_(Id)             ((USIGN8) ((Id) >> 16))
#define CDWHHB_(Id)             ((USIGN8) ((Id) >> 24))
#define CDWLW_(Id)              ((USIGN16) (Id))
#define CDWHW_(Id)              ((USIGN16) ((Id) >> 16))

/* byte and word access to memory objects */

#if defined(LITTLE_ENDIAN) == defined(BIG_ENDIAN)
#warning either LITTLE_ENDIAN or BIG_ENDIAN must be defined
#endif


#ifdef LITTLE_ENDIAN

#define USE_SWAP

#define WLB_(Id)        (((USIGN8 *) &(Id)) [0])
#define WHB_(Id)        (((USIGN8 *) &(Id)) [1])
#define DWLLB_(Id)      (((USIGN8 *) &(Id)) [0])
#define DWLHB_(Id)      (((USIGN8 *) &(Id)) [1])
#define DWHLB_(Id)      (((USIGN8 *) &(Id)) [2])
#define DWHHB_(Id)      (((USIGN8 *) &(Id)) [3])
#define DWLW_(Id)       (((USIGN16 *) &(Id)) [0])
#define DWHW_(Id)       (((USIGN16 *) &(Id)) [1])
/*@Dresser-Masoneilan project: 32bit alignment related */
#define WPLB_(pId)      (((USIGN8 *) (pId)) [0])
#define WPHB_(pId)      (((USIGN8 *) (pId)) [1])
#define DPWLLB_(pId)    (((USIGN8 *) (pId)) [0])
#define DPWLHB_(pId)    (((USIGN8 *) (pId)) [1])
#define DPWHLB_(pId)    (((USIGN8 *) (pId)) [2])
#define DPWHHB_(pId)    (((USIGN8 *) (pId)) [3])

#endif /* LITTLE_ENDIAN */


#ifdef BIG_ENDIAN

#define WLB_(Id)        (((USIGN8 *) &(Id)) [1])
#define WHB_(Id)        (((USIGN8 *) &(Id)) [0])
#define DWLLB_(Id)      (((USIGN8 *) &(Id)) [3])
#define DWLHB_(Id)      (((USIGN8 *) &(Id)) [2])
#define DWHLB_(Id)      (((USIGN8 *) &(Id)) [1])
#define DWHHB_(Id)      (((USIGN8 *) &(Id)) [0])
#define DWLW_(Id)       (((USIGN16 *) &(Id)) [1])
#define DWHW_(Id)       (((USIGN16 *) &(Id)) [0])
/*@Dresser-Masoneilan project: 32bit alignment related */
#define WPLB_(pId)      (((USIGN8 *) (pId)) [1])
#define WPHB_(pId)      (((USIGN8 *) (pId)) [0])
#define DPWLLB_(pId)    (((USIGN8 *) (pId)) [3])
#define DPWLHB_(pId)    (((USIGN8 *) (pId)) [2])
#define DPWHLB_(pId)    (((USIGN8 *) (pId)) [1])
#define DPWHHB_(pId)    (((USIGN8 *) (pId)) [0])

#endif /* BIG_ENDIAN */


#ifdef COMPILER_IAR
#if __VER__ < 200
#pragma warnings = on
#endif
#endif /* COMPILER_IAR */


#endif /* __BASE_CFG_H__ */

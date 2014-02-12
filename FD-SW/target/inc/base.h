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

FILE_NAME          base.h



DESCRIPTION        This file contains the Softing's basic data types

=========================================================================== */
#ifndef __BASE_H__
#define __BASE_H__

#include "base_cfg.h"

/* --- basic type definitions ---------------------------------------------- */

#define VOID            void

#ifndef NULL
#define NULL            ((VOID *) 0UL)
#endif

typedef unsigned char   BOOL;
typedef unsigned char   BYTE;
typedef unsigned char   USIGN8;
typedef unsigned short  USIGN16;
typedef unsigned long   USIGN32;
typedef signed char     INT8;
typedef signed short    INT16;
typedef signed long     INT32;
typedef char            CHAR;
typedef unsigned char   STRINGV;
typedef unsigned short  BSTRING;
typedef unsigned char   OSTRING;
typedef float           FLOAT;
typedef double          DOUBLE;


/* --- boolean type -------------------------------------------------------- */

#ifdef TRUE
#undef TRUE
#endif
#define TRUE            0xFF
#define FB_TRUE         0xFF

#ifdef FALSE
#undef FALSE
#endif
#define FALSE           0x00
#define FB_FALSE        0x00

#define E_OK            0u        /* no error */
#define E_ERROR         1u        /* error */
#define E_POS_RES       0u
#define E_NEG_RES       1u
#define E_PENDING_RES   0xFFu
#define FLOAT_POSITIVE_SMALL_DIFF         (0.0000001f)

#define CONST_DATA      const
#define ArrElemCnt(Arr) (sizeof (Arr) / sizeof *(Arr))


/* Handle with unreferenced parameters ------------------------------------ */
#if !defined _lint

  #ifdef COMPILER_IAR
    #define _UNREF_PARAM(x) (x) = (x)   /* Avoid IAR compiler warnings */
  #else /* not COMPILER_IAR */
    #define _UNREF_PARAM(x)
  #endif /* not COMPILER_IAR */

#else /* Special macro for PC-lint */

  #define _UNREF_PARAM(x)  pc_lint_unref_param(x)

#endif /* Special macro for PC-lint */


/* ------------------------------------------------------------------------- */
/* macros                                                                    */
/* ------------------------------------------------------------------------- */

/* ---- macro elemsize, to calculate the size of an element of a record ---- */

#ifndef elemsize
#define elemsize(type,member) sizeof (((type *) 0)->member)
#endif

/* --- macro min(a,b), to calculate the minimum of two values  ------------- */

#ifndef min
#define min(a,b)        (((a) < (b)) ? (a) : (b))
#endif

/* --- macro max(a,b), to calculate the maximum of two values  ------------- */

#ifndef max
#define max(a,b)        (((a) > (b)) ? (a) : (b))
#endif

/* --- define offsetof ----------------------------------------------------- */
#if defined (offsetof)
  #undef offsetof
#endif /* defined (offsetof) */

#define offsetof(t,m)   (USIGN16)((USIGN32)&((t*)0)->m)


/* --- define  escape jump ------------------------------------------------- */

#define escape          goto endlbl





#endif /* __BASE_H__ */

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

FILE_NAME          cpu.h



=========================================================================== */
#ifndef __CPU_H__
#define __CPU_H__

/* --- Base values for hardware definition macros -------------------------- */
/* --- !! Do not change !! ------------------------------------------------- */
#if 0
#define DEF_UART0           1
#define DEF_UART1           2
#define DEF_UART2           3

#define DEF_T0              1
#define DEF_T1              2
#define DEF_T2              3
#define DEF_T3              4
#define DEF_T4              5
#define DEF_T5              6
#define DEF_T6              7
#define DEF_T7              8
#define DEF_T8              9
#define DEF_T9              10
#define DEF_T10             11

#define DEF_DMA0            1
#define DEF_DMA1            2
#endif
/*---------------------------------------------------------------------------*/

#ifdef Cortex_M3
  #include "stm32f10x.h"
#else
  #error "CPU type not defined in file Makeconfig.mak!"
#endif

#endif /* __CPU_H__ */

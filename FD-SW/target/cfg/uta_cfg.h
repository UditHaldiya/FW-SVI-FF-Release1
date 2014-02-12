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

FILE_NAME          utacfg.h



DESCRIPTION        This file contains the identification of a field device.
                   The file is included exclusively by the 'utacfg.c' file.

=========================================================================== */


#define UTA_MANUFAC_ID              0x004745  /* GE - Masoneilan */
#define UTA_DEV_TYPE                0x0008
#define UTA_DEV_REV                 1
#define UTA_DD_REV                  1
#ifdef FC_SPC42
#define UTA_SERIAL_NR               "          _SPC42_UTA_V2_42_00001"
#elif defined (FIND1_PLUS)
#define UTA_SERIAL_NR               "          ____F1_UTA_V2_42_00001"
#elif defined (FC_FIND1P_DMA)
#define UTA_SERIAL_NR               "          _F1DMA_UTA_V2_42_00001"
#elif defined (FC_FRONTIER1_PLUS)
#define UTA_SERIAL_NR               "          ___FRT_UTA_V2_42_00001"
#endif
#define UTA_CONFIG_REVISION         1

#define UTA_MIN_CYCLE_T             9600    /* Unit: 1/32 msec */

#define UTA_DEFAULT_PD_TAG          "UpperTesterAgent"
#define UTA_SM_OPERATIONAL          TRUE
#define UTA_DEFAULT_NODE_ADDRESS    20
#define UTA_DEFAULT_DEV_TYPE        DLL_LINKMASTER


#define UTA_ST_MIN                  8
#define UTA_MID_MIN                 6
#define UTA_MRD_MIN                 5



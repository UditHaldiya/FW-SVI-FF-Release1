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

FILE_NAME         swdl_cfg.h



DESCRIPTION

=========================================================================== */
#ifndef __SWDL_CFG_H__
#define __SWDL_CFG_H__

#if (RES_VAL_DEV_TYPE == 0x0002)  /* Dresser SVI Positioner */

  #define FD_MULTIDOM_MFG_ID    "445644"
  #define FD_MULTIDOM_FAMILY    "0002"
  #define FD_MULTIDOM_INSTANCE  "FD-INST "
  #define FD_MULTIDOM_HW_REV    1
  #define FD_MULTIDOM_SW_REV    1

  #define AP_MULTIDOM_MFG_ID    "445644"
  #define AP_MULTIDOM_FAMILY    "0002"
  #define AP_MULTIDOM_INSTANCE  "AP-INST "
  #define AP_MULTIDOM_HW_REV    1
  #define AP_MULTIDOM_SW_REV    1

  #define FD_DOM_MFG_ID         "445644"
  #define FD_DOM_FAMILY         "0002"
  #define FD_DOM_DEV_TYPE       "0002"
  #define FD_DOM_DEV_REV        2
  #define FD_DOM_DD_REV         1
  #define FD_DOM_SW_REV         "2-42    "
  #define FD_DOM_SW_NAME        "FD-SW   "
  #define FD_DOM_DOM_NAME       "FD-DOM  "

  #define AP_DOM_MFG_ID         "445644"
  #define AP_DOM_FAMILY         "0002"
  #define AP_DOM_DEV_TYPE       "0002"
  #define AP_DOM_DEV_REV        2
  #define AP_DOM_DD_REV         1
  #define AP_DOM_SW_REV         "2-42    "
  #define AP_DOM_SW_NAME        "AP-SW   "
  #define AP_DOM_DOM_NAME       "AP-DOM  "

#elif (RES_VAL_DEV_TYPE == 0x0008)  /* GE SVI Positioner */

  #define FD_MULTIDOM_MFG_ID    "004745"
  #define FD_MULTIDOM_FAMILY    "0008"
  #define FD_MULTIDOM_INSTANCE  "FD-INST "
  #define FD_MULTIDOM_HW_REV    1
  #define FD_MULTIDOM_SW_REV    1

  #define AP_MULTIDOM_MFG_ID    "004745"
  #define AP_MULTIDOM_FAMILY    "0008"
  #define AP_MULTIDOM_INSTANCE  "AP-INST "
  #define AP_MULTIDOM_HW_REV    1
  #define AP_MULTIDOM_SW_REV    1

  #define FD_DOM_MFG_ID         "004745"
  #define FD_DOM_FAMILY         "0008"
  #define FD_DOM_DEV_TYPE       "0008"
  #define FD_DOM_DEV_REV        2
  #define FD_DOM_DD_REV         1
  #define FD_DOM_SW_REV         "2-42    "
  #define FD_DOM_SW_NAME        "FD-SW   "
  #define FD_DOM_DOM_NAME       "FD-DOM  "

  #define AP_DOM_MFG_ID         "004745"
  #define AP_DOM_FAMILY         "0008"
  #define AP_DOM_DEV_TYPE       "0008"
  #define AP_DOM_DEV_REV        2
  #define AP_DOM_DD_REV         1
  #define AP_DOM_SW_REV         "2-42    "
  #define AP_DOM_SW_NAME        "AP-SW   "
  #define AP_DOM_DOM_NAME       "AP-DOM  "

#endif /* RES_VAL_DEV_TYPE == 0x00FE */


  #define MAX_TIME_FOR_DWNLD_PREPARE   120    /* seconds */
  #define MAX_TIME_FOR_SW_ACTIVATE     120    /* seconds */

#endif /* __SWDL_CFG_H__ */

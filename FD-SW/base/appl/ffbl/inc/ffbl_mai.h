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
*                     Copyright (C) SOFTING IA GmbH 2013                     *
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
* PROJECT_NAME             Softing FF/PA FD 2.42.A                           *
*                                                                            *
* VERSION                  FF - 2.42.A                                       *
*                          PA - 2.42.A                                       *
*                                                                            *
* DATE                     14. Aug. 2013                                     *
*                                                                            *
*****************************************************************************/


/* ===========================================================================
                            Multiple Analog Input


FILE_NAME          ffbl_mai.h



DESCRIPTION        definition of Multiple Analog Input Function Block and its
                   parameters

=========================================================================== */
#ifndef __FFBL_MAI_H__
#define __FFBL_MAI_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/


#define MAI_PAR_BLK_DATA       0
#define MAI_PAR_ST_REV         1
#define MAI_PAR_TAG_DESC       2
#define MAI_PAR_STRATEGY       3
#define MAI_PAR_ALERT_KEY      4
#define MAI_PAR_MODE_BLK       5
#define MAI_PAR_BLOCK_ERR      6
#define MAI_PAR_CHANNEL        7
#define MAI_PAR_OUT_1          8
#define MAI_PAR_OUT_2          9
#define MAI_PAR_OUT_3          10
#define MAI_PAR_OUT_4          11
#define MAI_PAR_OUT_5          12
#define MAI_PAR_OUT_6          13
#define MAI_PAR_OUT_7          14
#define MAI_PAR_OUT_8          15
#define MAI_PAR_UPDATE_EVT     16
#define MAI_PAR_BLOCK_ALM      17
#define MAI_PAR_BLOCK_ERR_DESC_1     18




typedef struct
{
  F_BLOCK         blk_data;
  USIGN16         st_rev;
  STRINGV         tag_desc[32];
  USIGN16         strategy;
  USIGN8          alert_key;
  MODE            mode_blk;
  USIGN16         block_err;
  USIGN16         channel;
  FLOAT_S         out[8];
  EVENT           update_evt;
  ALARM_DISCRETE  block_alm;
  USIGN32         block_err_desc_1;
}
  T_MAIFB;


typedef struct
{
  T_LINKED_TB               linked_tb;
  T_FBIF_PROCESS_DATA       exec_tb;
  FLOAT_S                   out[8];
  USIGN16                   old_block_alarm_bits;
}
  T_MAIFB_DATA;


#endif /* __FFBL_MAI_H__ */

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

FILE_NAME          ffbl_is.h



DESCRIPTION        definition of Input Selector Function Block and its
                   parameters

=========================================================================== */
#ifndef __FFBL_IS_H__
#define __FFBL_IS_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/


#define IS_PAR_BLK_DATA          0
#define IS_PAR_ST_REV            1
#define IS_PAR_TAG_DESC          2
#define IS_PAR_STRATEGY          3
#define IS_PAR_ALERT_KEY         4
#define IS_PAR_MODE_BLK          5
#define IS_PAR_BLOCK_ERR         6
#define IS_PAR_OUT               7
#define IS_PAR_OUT_RANGE         8
#define IS_PAR_GRANT_DENY        9
#define IS_PAR_STATUS_OPTS      10
#define IS_PAR_IN_1             11
#define IS_PAR_IN_2             12
#define IS_PAR_IN_3             13
#define IS_PAR_IN_4             14
#define IS_PAR_DISABLE_1        15
#define IS_PAR_DISABLE_2        16
#define IS_PAR_DISABLE_3        17
#define IS_PAR_DISABLE_4        18
#define IS_PAR_SELECT_TYPE      19
#define IS_PAR_MIN_GOOD         20
#define IS_PAR_SELECTED         21
#define IS_PAR_OP_SELECT        22
#define IS_PAR_UPDATE_EVT       23
#define IS_PAR_BLOCK_ALM        24
#define IS_PAR_BLOCK_ERR_DESC_1 25


typedef struct
{
  F_BLOCK                   blk_data;
  USIGN16                   st_rev;
  STRINGV                   tag_desc[32];
  USIGN16                   strategy;
  USIGN8                    alert_key;
  MODE                      mode_blk;
  BSTRING                   block_err;
  FLOAT_S                   out;
  SCALE                     out_range;
  ACCESS_PERM               grant_deny;
  BSTRING                   status_opts;
  FLOAT_S                   in_1;
  FLOAT_S                   in_2;
  FLOAT_S                   in_3;
  FLOAT_S                   in_4;
  DISCRETE_S                disable_1;
  DISCRETE_S                disable_2;
  DISCRETE_S                disable_3;
  DISCRETE_S                disable_4;
  USIGN8                    select_type;
  USIGN8                    min_good;
  DISCRETE_S                selected;
  DISCRETE_S                op_select;
  EVENT                     update_evt;
  ALARM_DISCRETE            block_alm;
  USIGN32                   block_err_desc_1;
}
  T_ISFB;


typedef struct
{
  FLOAT_S                   out;
  USIGN16                   old_block_alarm_bits;
}
  T_ISFB_DATA;


#define IS_STATUS_OPTS  STATUS_OPT_USE_UNCERT_AS_GOOD | STATUS_OPT_UNCERT_IF_MAN

#define IS_SEL_TYPE_UNINITIALIZED   0
#define IS_SEL_TYPE_FIRST_GOOD      1
#define IS_SEL_TYPE_MINIMUM         2
#define IS_SEL_TYPE_MAXIMUM         3
#define IS_SEL_TYPE_MIDDLE          4
#define IS_SEL_TYPE_AVERAGE         5

#define IS_USE_IN                   0
#define IS_DISABLE_IN               1


#endif /* __FFBL_IS_H__ */



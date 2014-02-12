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

FILE_NAME          ffbl_ai.h



DESCRIPTION        definition of Analog Input Function Block and its
                   parameters

=========================================================================== */
#ifndef __FFBL_AI_H__
#define __FFBL_AI_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/
#include "keywords.h"   /* keyword defines */

INCLUDES
#include "ffbl_pai.h"


#define AI_PAR_BLK_DATA       0
#define AI_PAR_ST_REV         1
#define AI_PAR_TAG_DESC       2
#define AI_PAR_STRATEGY       3
#define AI_PAR_ALERT_KEY      4
#define AI_PAR_MODE_BLK       5
#define AI_PAR_BLOCK_ERR      6
#define AI_PAR_PV             7
#define AI_PAR_OUT            8
#define AI_PAR_SIMULATE       9
#define AI_PAR_XD_SCALE      10
#define AI_PAR_OUT_SCALE     11
#define AI_PAR_GRANT_DENY    12
#define AI_PAR_IO_OPTS       13
#define AI_PAR_STATUS_OPTS   14
#define AI_PAR_CHANNEL       15
#define AI_PAR_L_TYPE        16
#define AI_PAR_LOW_CUT       17
#define AI_PAR_PV_FTIME      18
#define AI_PAR_FIELD_VAL     19
#define AI_PAR_UPDATE_EVT    20
#define AI_PAR_BLOCK_ALM     21
#define AI_PAR_ALARM_SUM     22
#define AI_PAR_ACK_OPTION    23
#define AI_PAR_ALARM_HYS     24
#define AI_PAR_HI_HI_PRI     25
#define AI_PAR_HI_HI_LIM     26
#define AI_PAR_HI_PRI        27
#define AI_PAR_HI_LIM        28
#define AI_PAR_LO_PRI        29
#define AI_PAR_LO_LIM        30
#define AI_PAR_LO_LO_PRI     31
#define AI_PAR_LO_LO_LIM     32
#define AI_PAR_HI_HI_ALM     33
#define AI_PAR_HI_ALM        34
#define AI_PAR_LO_ALM        35
#define AI_PAR_LO_LO_ALM     36
#define AI_PAR_BLOCK_ERR_DESC_1     37

#define L_TYPE_DIRECT         1
#define L_TYPE_INDIRECT       2
#define L_TYPE_IND_SQR_ROOT   3

typedef struct
{
  F_BLOCK                  blk_data;
  USIGN16                  st_rev;
  OSTRING                  tag_desc[32];
  USIGN16                  strategy;
  USIGN8                   alert_key;
  MODE                     mode_blk;
  BSTRING                  block_err;
  FLOAT_S                  pv;           /* engineering units        */
  FLOAT_S                  out;          /* output eng. units        */
  SIMULATE_FLOAT           simulate;
  SCALE                    xd_scale;
  SCALE                    out_scale;
  ACCESS_PERM              grant_deny;
  BSTRING                  io_opts;
  BSTRING                  status_opts;
  USIGN16                  channel;      /* not 0                    */
  USIGN8                   l_type;       /* external function        */
  FLOAT                    low_cut;
  FLOAT                    pv_ftime;     /* seconds                  */
  FLOAT_S                  field_val;    /* %                        */
  EVENT                    update_evt;
  ALARM_DISCRETE           block_alm;
  ALARM_SUMMARY            alarm_sum;
  BSTRING                  ack_option;
  FLOAT                    alarm_hys;    /* engineering units        */
  USIGN8                   hi_hi_pri;
  FLOAT                    hi_hi_lim;    /* engineering units        */
  USIGN8                   hi_pri;
  FLOAT                    hi_lim;       /* engineering units        */
  USIGN8                   lo_pri;
  FLOAT                    lo_lim;       /* engineering units        */
  USIGN8                   lo_lo_pri;
  FLOAT                    lo_lo_lim;    /* engineering units        */
  ALARM_FLOAT              hi_hi_alm;
  ALARM_FLOAT              hi_alm;
  ALARM_FLOAT              lo_alm;
  ALARM_FLOAT              lo_lo_alm;
  USIGN32                  block_err_desc_1;
}
  T_AIFB;


typedef struct
{
  T_LINKED_TB               linked_tb;
  T_FBIF_PROCESS_DATA       exec_tb;
  T_CHAN_UNIT_CHECK         chan_unit_check;
  FLOAT                     pv_factor;
  FLOAT_S                   out;
  USIGN16                   old_block_alarm_bits;
}
  T_AIFB_DATA;


#define AI_IO_OPTS      IO_OPT_LOW_CUTOFF

#define AI_STATUS_OPTS  STATUS_OPT_PROP_FLT_FORW |\
                        STATUS_OPT_UNCERT_IF_LIM |\
                        STATUS_OPT_BAD_IF_LIM    |\
                        STATUS_OPT_UNCERT_IF_MAN


#endif /* __FFBL_AI_H__ */


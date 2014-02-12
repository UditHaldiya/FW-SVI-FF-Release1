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

FILE_NAME          ffbl_di.h



DESCRIPTION        Definition of Discrete Input Function Block and its
                   parameters

=========================================================================== */
#ifndef __FFBL_DI_H__
#define __FFBL_DI_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/


#define DI_PAR_BLK_DATA       0
#define DI_PAR_ST_REV         1
#define DI_PAR_TAG_DESC       2
#define DI_PAR_STRATEGY       3
#define DI_PAR_ALERT_KEY      4
#define DI_PAR_MODE_BLK       5
#define DI_PAR_BLOCK_ERR      6
#define DI_PAR_PV_D           7
#define DI_PAR_OUT_D          8
#define DI_PAR_SIMULATE_D     9
#define DI_PAR_XD_STATE      10
#define DI_PAR_OUT_STATE     11
#define DI_PAR_GRANT_DENY    12
#define DI_PAR_IO_OPTS       13
#define DI_PAR_STATUS_OPTS   14
#define DI_PAR_CHANNEL       15
#define DI_PAR_PV_FTIME      16
#define DI_PAR_FIELD_VAL_D   17
#define DI_PAR_UPDATE_EVT    18
#define DI_PAR_BLOCK_ALM     19
#define DI_PAR_ALARM_SUM     20
#define DI_PAR_ACK_OPTION    21
#define DI_PAR_DISC_PRI      22
#define DI_PAR_DISC_LIM      23
#define DI_PAR_DISC_ALM      24
#define DI_PAR_BLOCK_ERR_DESC_1     25




typedef struct
{
  F_BLOCK                  blk_data;
  USIGN16                  st_rev;
  OSTRING                  tag_desc[32];
  USIGN16                  strategy;
  USIGN8                   alert_key;
  MODE                     mode_blk;
  BSTRING                  block_err;
  DISCRETE_S               pv_d;
  DISCRETE_S               out_d;
  SIMULATE_DISCRETE        simulate_d;
  USIGN16                  xd_state;
  USIGN16                  out_state;
  ACCESS_PERM              grant_deny;
  BSTRING                  io_opts;
  BSTRING                  status_opts;
  USIGN16                  channel;     /* not 0                    */
  FLOAT                    pv_ftime;    /* seconds                  */
  DISCRETE_S               field_val_d; /* %                        */
  EVENT                    update_evt;
  ALARM_DISCRETE           block_alm;
  ALARM_SUMMARY            alarm_sum;
  BSTRING                  ack_option;
  USIGN8                   disc_pri;
  USIGN8                   disc_lim;
  ALARM_DISCRETE           disc_alm;
  USIGN32                  block_err_desc_1;
}
  T_DIFB;


typedef struct
{
  T_LINKED_TB              linked_tb;
  T_FBIF_PROCESS_DATA      exec_tb;
  USIGN16                  pv_time_d;
  USIGN16                  pv_time_counter;
  DISCRETE_S               out_d;
  USIGN16                  old_block_alarm_bits;
}
  T_DIFB_DATA;



#define DI_IO_OPTS      IO_OPT_INVERT

#define DI_STATUS_OPTS  STATUS_OPT_PROP_FLT_FORW | STATUS_OPT_UNCERT_IF_MAN



#endif /* __FFBL_DI_H__ */




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

FILE_NAME          ffbl_do.h



DESCRIPTION

=========================================================================== */
#ifndef __FFBL_DO_H__
#define __FFBL_DO_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/

#include "ffbl_mo.h"

#define DO_PAR_BLK_DATA         0
#define DO_PAR_ST_REV           1
#define DO_PAR_TAG_DESC         2
#define DO_PAR_STRATEGY         3
#define DO_PAR_ALERT_KEY        4
#define DO_PAR_MODE_BLK         5
#define DO_PAR_BLOCK_ERR        6
#define DO_PAR_PV_D             7
#define DO_PAR_SP_D             8
#define DO_PAR_OUT_D            9
#define DO_PAR_SIMULATE_D      10
#define DO_PAR_PV_STATE        11
#define DO_PAR_XD_STATE        12
#define DO_PAR_GRANT_DENY      13
#define DO_PAR_IO_OPTS         14
#define DO_PAR_STATUS_OPTS     15
#define DO_PAR_READBACK_D      16
#define DO_PAR_CAS_IN_D        17
#define DO_PAR_CHANNEL         18
#define DO_PAR_FSTATE_TIME     19
#define DO_PAR_FSTATE_VAL_D    20
#define DO_PAR_BKCAL_OUT_D     21
#define DO_PAR_RCAS_IN_D       22
#define DO_PAR_SHED_OPT        23
#define DO_PAR_RCAS_OUT_D      24
#define DO_PAR_UPDATE_EVT      25
#define DO_PAR_BLOCK_ALM       26
#define DO_PAR_BLOCK_ERR_DESC_1     27


typedef struct
{
  F_BLOCK             blk_data;
  USIGN16             st_rev;
  STRINGV             tag_desc[32];
  USIGN16             strategy;
  USIGN8              alert_key;
  MODE                mode_blk;
  USIGN16             block_err;
  DISCRETE_S          pv_d;
  DISCRETE_S          sp_d;
  DISCRETE_S          out_d;
  SIMULATE_DISCRETE   simulate_d;
  USIGN16             pv_state;
  USIGN16             xd_state;
  ACCESS_PERM         grant_deny;
  USIGN16             io_opts;
  USIGN16             status_opts;
  DISCRETE_S          readback_d;
  DISCRETE_S          cas_in_d;
  USIGN16             channel;
  FLOAT               fstate_time;
  USIGN8              fstate_val_d;
  DISCRETE_S          bkcal_out_d;
  DISCRETE_S          rcas_in_d;
  USIGN8              shed_opt;
  DISCRETE_S          rcas_out_d;
  EVENT               update_evt;
  ALARM_DISCRETE      block_alm;
  USIGN32             block_err_desc_1;
}
  T_DOFB;


typedef struct
{
  T_LINKED_TB         linked_tb;
  T_FBIF_PROCESS_DATA exec_tb;
  BOOL                mode_was_written;
  USIGN8              new_target_mode;
  T_SHED_DATA         shed;
  T_FAULT_DATA        fault;
  DISCRETE_S          old_out_d;
  DISCRETE_S          out_d;
  USIGN16             old_block_alarm_bits;
  USIGN8              last_tb_status;
}
  T_DOFB_DATA;

#define DO_IO_OPTS      IO_OPT_INVERT                     |\
                        IO_OPT_SP_PV_TRACK_IN_MAN         |\
                        IO_OPT_SP_PV_TRACK_IN_LO          |\
                        IO_OPT_SP_TRACK_RET_TARGET        |\
                        IO_OPT_FSTATE_TO_VAL              |\
                        IO_OPT_FSTATE_TO_VAL_ON_RESTART   |\
                        IO_OPT_TARGET_TO_MAN_IF_FSTATE    |\
                        IO_OPT_PV_FOR_BKCAL_OUT

#define DO_STATUS_OPTS  STATUS_OPT_PROP_FLT_BACKW


#endif /* __FFBL_DO_H__ */


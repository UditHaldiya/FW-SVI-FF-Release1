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

FILE_NAME          ffbl_ao.h



=========================================================================== */
#ifndef __FFBL_AO_H__
#define __FFBL_AO_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/
#include "keywords.h"   /* keyword defines */

INCLUDES
#include "ffbl_mo.h"
#include "ffbl_pao.h"


GLOBAL_DEFINES
#define AO_PAR_BLK_DATA      0
#define AO_PAR_ST_REV        1
#define AO_PAR_TAG_DESC      2
#define AO_PAR_STRATEGY      3
#define AO_PAR_ALERT_KEY     4
#define AO_PAR_MODE_BLK      5
#define AO_PAR_BLOCK_ERR     6
#define AO_PAR_PV            7
#define AO_PAR_SP            8
#define AO_PAR_OUT           9
#define AO_PAR_SIMULATE     10
#define AO_PAR_PV_SCALE     11
#define AO_PAR_XD_SCALE     12
#define AO_PAR_GRANT_DENY   13
#define AO_PAR_IO_OPTS      14
#define AO_PAR_STATUS_OPTS  15
#define AO_PAR_READBACK     16
#define AO_PAR_CAS_IN       17
#define AO_PAR_SP_RATE_DN   18
#define AO_PAR_SP_RATE_UP   19
#define AO_PAR_SP_HI_LIM    20
#define AO_PAR_SP_LO_LIM    21
#define AO_PAR_CHANNEL      22
#define AO_PAR_FSTATE_TIME  23
#define AO_PAR_FSTATE_VAL   24
#define AO_PAR_BKCAL_OUT    25
#define AO_PAR_RCAS_IN      26
#define AO_PAR_SHED_OPT     27
#define AO_PAR_RCAS_OUT     28
#define AO_PAR_UPDATE_EVT   29
#define AO_PAR_BLOCK_ALM    30
#define AO_PAR_BLOCK_ERR_DESC_1     31


LOCAL_DEFINES

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

typedef struct
{
  F_BLOCK               blk_data;
  USIGN16               st_rev;
  STRINGV               tag_desc[32];
  USIGN16               strategy;
  USIGN8                alert_key;
  MODE                  mode_blk;
  USIGN16               block_err;
  FLOAT_S               pv;
  FLOAT_S               sp;
  FLOAT_S               out;
  SIMULATE_FLOAT        simulate;
  SCALE                 pv_scale;
  SCALE                 xd_scale;
  ACCESS_PERM           grant_deny;
  USIGN16               io_opts;
  USIGN16               status_opts;
  FLOAT_S               readback;
  FLOAT_S               cas_in;
  FLOAT                 sp_rate_dn;
  FLOAT                 sp_rate_up;
  FLOAT                 sp_hi_lim;
  FLOAT                 sp_lo_lim;
  USIGN16               channel;
  FLOAT                 fstate_time;
  FLOAT                 fstate_val;
  FLOAT_S               bkcal_out;
  FLOAT_S               rcas_in;
  USIGN8                shed_opt;
  FLOAT_S               rcas_out;
  EVENT                 update_evt;
  ALARM_DISCRETE        block_alm;
  USIGN32               block_err_desc_1;
}
  T_AOFB;



typedef struct
{
  T_LINKED_TB           linked_tb;
  T_FBIF_PROCESS_DATA   exec_tb;
  T_CHAN_UNIT_CHECK     chan_unit_check;
  BOOL                  mode_was_written;
  USIGN8                new_target_mode;
  T_SHED_DATA           shed;
  T_FAULT_DATA          fault;
  FLOAT_S               last_sp;
  FLOAT_S               old_out;
  FLOAT                 period_of_exec;   /* Period of execution in seconds */
  FLOAT_S               out;
  USIGN16               old_block_alarm_bits;
  USIGN8                last_tb_status;
}
  T_AOFB_DATA;

#define AO_IO_OPTS      IO_OPT_SP_PV_TRACK_IN_MAN         |\
                        IO_OPT_SP_PV_TRACK_IN_LO          |\
                        IO_OPT_SP_TRACK_RET_TARGET        |\
                        IO_OPT_INC_TO_CLOSE               |\
                        IO_OPT_FSTATE_TO_VAL              |\
                        IO_OPT_FSTATE_TO_VAL_ON_RESTART   |\
                        IO_OPT_TARGET_TO_MAN_IF_FSTATE    |\
                        IO_OPT_PV_FOR_BKCAL_OUT

#define AO_STATUS_OPTS  STATUS_OPT_PROP_FLT_BACKW


#endif /* __FFBL_AO_H__ */

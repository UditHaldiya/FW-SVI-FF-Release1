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

FILE_NAME          ffbl_res.h



DESCRIPTION        definition of Resource Block and its parameters

=========================================================================== */
#ifndef __FFBL_RES_H__
#define __FFBL_RES_H__

#define RES_PAR_ST_REV           1
#define RES_PAR_TAG_DESC         2
#define RES_PAR_STRATEGY         3
#define RES_PAR_ALERT_KEY        4
#define RES_PAR_MODE_BLK         5
#define RES_PAR_BLOCK_ERR        6
#define RES_PAR_RS_STATE         7
#define RES_PAR_TEST_RW          8
#define RES_PAR_DD_RESOURCE      9
#define RES_PAR_MANUFAC_ID      10
#define RES_PAR_DEV_TYPE        11
#define RES_PAR_DEV_REV         12
#define RES_PAR_DD_REV          13
#define RES_PAR_GRANT_DENY      14
#define RES_PAR_HARD_TYPES      15
#define RES_PAR_RESTART         16
#define RES_PAR_FEATURES        17
#define RES_PAR_FEATURE_SEL     18
#define RES_PAR_CYCLE_TYPE      19
#define RES_PAR_CYCLE_SEL       20
#define RES_PAR_MIN_CYCLE_T     21
#define RES_PAR_MEMORY_SIZE     22
#define RES_PAR_NV_CYCLE_T      23
#define RES_PAR_FREE_SPACE      24
#define RES_PAR_FREE_TIME       25
#define RES_PAR_SHED_RCAS       26
#define RES_PAR_SHED_ROUT       27
#define RES_PAR_FAULT_STATE     28
#define RES_PAR_SET_FSTATE      29
#define RES_PAR_CLR_FSTATE      30
#define RES_PAR_MAX_NOTIFY      31
#define RES_PAR_LIM_NOTIFY      32
#define RES_PAR_CONFIRM_TIME    33
#define RES_PAR_WRITE_LOCK      34
#define RES_PAR_UPDATE_EVT      35
#define RES_PAR_BLOCK_ALM       36
#define RES_PAR_ALARM_SUM       37
#define RES_PAR_ACK_OPTION      38
#define RES_PAR_WRITE_PRI       39
#define RES_PAR_WRITE_ALM       40
#define RES_PAR_ITK_VER         41


/* Resource state machine */
#define RS_STATE_ON_LINE_LNKING  0x03
#define RS_STATE_ON_LINE         0x04
#define RS_STATE_STANDBY         0x05
#define RS_STATE_FAILURE         0x06

#define FAULT_STATE_CLEAR        0x01
#define FAULT_STATE_ACTIVE       0x02

#define SET_FAULT_STATE_OFF      0x01
#define SET_FAULT_STATE_SET      0x02

#define CLR_FAULT_STATE_OFF      0x01
#define CLR_FAULT_STATE_CLEAR    0x02

#define FEAT_UNICODE_STRINGS            0x8000
#define FEAT_REP_SUPP                   0x4000
#define FEAT_FAULT_STATE_SUPP           0x2000
#define FEAT_SOFT_WRITE_LOCK            0x1000
#define FEAT_HARD_WRITE_LOCK            0x0800
#define FEAT_OUTPUT_READBACK_SUPP       0x0400
#define FEAT_DIRECT_WRITE_OUTP_HW_SUPP  0x0200
#define FEAT_CHANGE_BP_IN_AUTO          0x0100

#define WRITE_UNLOCKED                  1
#define WRITE_LOCKED                    2


/* Range check in resource block ------------------------------------------ */

#define RES_CONF_TIME_MIN        16000uL      /*  0.5 sec */


typedef struct _RESOURCE
{
  F_BLOCK            blk_data;
  USIGN16            st_rev;
  OSTRING            tag_desc[32];
  USIGN16            strategy;
  USIGN8             alert_key;
  MODE               mode_blk;
  BSTRING            block_err;
  USIGN8             rs_state;
  TEST               test_rw;
  STRINGV            dd_resource[32];
  USIGN32            manufac_id;
  USIGN16            dev_type;
  USIGN8             dev_rev;
  USIGN8             dd_rev;
  ACCESS_PERM        grant_deny;
  BSTRING            hard_types;
  USIGN8             restart;
  BSTRING            features;
  BSTRING            feature_sel;
  BSTRING            cycle_type;
  BSTRING            cycle_sel;
  USIGN32            min_cycle_t;
  USIGN16            memory_size;
  USIGN32            nv_cycle_t;
  FLOAT              free_space;
  FLOAT              free_time;
  USIGN32            shed_rcas;
  USIGN32            shed_rout;
  USIGN8             fault_state;
  USIGN8             set_fstate;
  USIGN8             clr_fstate;
  USIGN8             max_notify;
  USIGN8             lim_notify;
  USIGN32            confirm_time;
  USIGN8             write_lock;
  EVENT              update_evt;
  ALARM_DISCRETE     block_alm;
  ALARM_SUMMARY      alarm_sum;
  BSTRING            ack_option;
  USIGN8             write_pri;
  ALARM_DISCRETE     write_alm;
  USIGN16            itk_ver;

} T_RESOURCE;


/* Field Diagnostics ------------------------------------------------------ */
/* ------------------------------------------------------------------------ */

#define FD_PAR_VER               0
#define FD_PAR_FAIL_ACTIVE       1
#define FD_PAR_OFFSPEC_ACTIVE    2
#define FD_PAR_MAINT_ACTIVE      3
#define FD_PAR_CHECK_ACTIVE      4
#define FD_PAR_FAIL_MAP          5
#define FD_PAR_OFFSPEC_MAP       6
#define FD_PAR_MAINT_MAP         7
#define FD_PAR_CHECK_MAP         8
#define FD_PAR_FAIL_MASK         9
#define FD_PAR_OFFSPEC_MASK     10
#define FD_PAR_MAINT_MASK       11
#define FD_PAR_CHECK_MASK       12
#define FD_PAR_FAIL_ALM         13
#define FD_PAR_OFFSPEC_ALM      14
#define FD_PAR_MAINT_ALM        15
#define FD_PAR_CHECK_ALM        16
#define FD_PAR_FAIL_PRI         17
#define FD_PAR_OFFSPEC_PRI      18
#define FD_PAR_MAINT_PRI        19
#define FD_PAR_CHECK_PRI        20
#define FD_PAR_SIMULATE         21
#define FD_PAR_RECOMMEN_ACT     22


/* added pack(1) because of address alignment between structures T_FD_PARAM and T_FBIF_RESB */
#pragma pack(1)
typedef struct _T_FD_PARAM
{
  USIGN16            fd_ver;
  USIGN32            fd_fail_active;
  USIGN32            fd_offspec_active;
  USIGN32            fd_maint_active;
  USIGN32            fd_check_active;
  USIGN32            fd_fail_map;
  USIGN32            fd_offspec_map;
  USIGN32            fd_maint_map;
  USIGN32            fd_check_map;
  USIGN32            fd_fail_mask;
  USIGN32            fd_offspec_mask;
  USIGN32            fd_maint_mask;
  USIGN32            fd_check_mask;
  ALARM_FD_DIAG      fd_fail_alm;
  ALARM_FD_DIAG      fd_offspec_alm;
  ALARM_FD_DIAG      fd_maint_alm;
  ALARM_FD_DIAG      fd_check_alm;
  USIGN8             fd_fail_pri;
  USIGN8             fd_offspec_pri;
  USIGN8             fd_maint_pri;
  USIGN8             fd_check_pri;
  SIMULATE_FD        fd_simulate;
  USIGN16            fd_recommen_act;

} T_FD_PARAM;
#pragma pack()

FUNCTION_DECLARATIONS

EXPORT_DATA
#include "da_ffbl.h"
  NO_INIT extern T_RESOURCE *   p_resource;
#include "da_def.h"


#endif /* __FFBL_RES_H__ */

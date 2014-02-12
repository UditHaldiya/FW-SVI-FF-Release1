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

FILE_NAME          ffbl_os.h



DESCRIPTION        Definition of Output Splitter Function Block and its
                   parameters.

=========================================================================== */
#ifndef __FFBL_OS_H__
#define __FFBL_OS_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/


#define OS_PAR_BLK_DATA                         0
#define OS_PAR_ST_REV                           1
#define OS_PAR_TAG_DESC                         2
#define OS_PAR_STRATEGY                         3
#define OS_PAR_ALERT_KEY                        4
#define OS_PAR_MODE_BLK                         5
#define OS_PAR_BLOCK_ERR                        6
#define OS_PAR_SP                               7
#define OS_PAR_OUT_1                            8
#define OS_PAR_OUT_2                            9
#define OS_PAR_OUT_1_RANGE                      10
#define OS_PAR_OUT_2_RANGE                      11
#define OS_PAR_GRANT_DENY                       12
#define OS_PAR_STATUS_OPTS                      13
#define OS_PAR_CAS_IN                           14
#define OS_PAR_BKCAL_OUT                        15
#define OS_PAR_IN_ARRAY                         16
#define OS_PAR_OUT_ARRAY                        17
#define OS_PAR_LOCKVAL                          18
#define OS_PAR_BKCAL_IN_1                       19
#define OS_PAR_BKCAL_IN_2                       20
#define OS_PAR_BAL_TIME                         21
#define OS_PAR_HYSTVAL                          22
#define OS_PAR_UPDATE_EVT                       23
#define OS_PAR_BLOCK_ALM                        24
#define OS_PAR_BLOCK_ERR_DESC_1                 25


typedef struct
{
  F_BLOCK                   blk_data;
  USIGN16                   st_rev;
  STRINGV                   tag_desc[32];
  USIGN16                   strategy;
  USIGN8                    alert_key;
  MODE                      mode_blk;
  USIGN16                   block_err;
  FLOAT_S                   sp;
  FLOAT_S                   out_1;
  FLOAT_S                   out_2;
  SCALE                     out_1_range;
  SCALE                     out_2_range;
  ACCESS_PERM               grant_deny;
  USIGN16                   status_opts;
  FLOAT_S                   cas_in;
  FLOAT_S                   bkcal_out;
  FLOAT                     in_array[4];
  FLOAT                     out_array[4];
  USIGN8                    lockval;
  FLOAT_S                   bkcal_in_1;
  FLOAT_S                   bkcal_in_2;
  FLOAT                     bal_time;
  FLOAT                     hystval;
  EVENT                     update_evt;
  ALARM_DISCRETE            block_alm;
  USIGN32                   block_err_desc_1;
}
  T_OSFB;

typedef struct
{
  FLOAT                     m1;
  FLOAT                     m2;
  FLOAT                     start_sp_1;
  FLOAT                     start_sp_2;
  FLOAT                     working_sp_1;
  FLOAT                     working_sp_2;
  USIGN8                    downstream_1;
  USIGN8                    downstream_2;
  BOOL                      balance_1;
  BOOL                      balance_2;
  BOOL                      end_value_reached;
  FLOAT                     end_value;
  FLOAT                     hyst_limit;
  FLOAT                     balance_factor_1;
  FLOAT                     balance_factor_2;
  FLOAT                     balance_step;
  FLOAT_S                   out_1;
  FLOAT_S                   out_2;
  USIGN16                   old_block_alarm_bits;
  USIGN8                    new_target_mode;
  BOOL                      mode_was_written;
}
  T_OSFB_DATA;

#define MODE_OS_VALID  (MODE_OS | MODE_AUTO | MODE_CAS )

#define OS_STATUS_OPTS  STATUS_OPT_IFS_IF_BAD_CAS_IN       | \
                        STATUS_OPT_TARG_NEXT_IF_BAD_CAS_IN

#define OS_LOCKVAL_NO_LOCK    1
#define OS_LOCKVAL_LOCK       2

#define CASC_NOT_ESTABLISHED        0x10u
#define CASC_INIT                   0x20u
#define CASC_INIT_WITH_ACK          0x21u
#define CASC_INIT_WITHOUT_ACK       0x22u
#define CASC_ESTABLISHED            0x40u


#endif /* __FFBL_OS_H__ */



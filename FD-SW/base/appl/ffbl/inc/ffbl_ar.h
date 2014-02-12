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

FILE_NAME          ffbl_ar.h



DESCRIPTION        definition of Arithmetic Function Block and its
                   parameters

=========================================================================== */
#ifndef __FFBL_AR_H__
#define __FFBL_AR_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/

#define AR_PAR_BLK_DATA          0
#define AR_PAR_ST_REV            1
#define AR_PAR_TAG_DESC          2
#define AR_PAR_STRATEGY          3
#define AR_PAR_ALERT_KEY         4
#define AR_PAR_MODE_BLK          5
#define AR_PAR_BLOCK_ERR         6
#define AR_PAR_PV                7
#define AR_PAR_OUT               8
#define AR_PAR_PRE_OUT           9
#define AR_PAR_PV_SCALE          10
#define AR_PAR_OUT_RANGE         11
#define AR_PAR_GRANT_DENY        12
#define AR_PAR_INPUT_OPTS        13
#define AR_PAR_IN                14
#define AR_PAR_IN_LO             15
#define AR_PAR_IN_1              16
#define AR_PAR_IN_2              17
#define AR_PAR_IN_3              18
#define AR_PAR_RANGE_HI          19
#define AR_PAR_RANGE_LO          20
#define AR_PAR_BIAS_IN_1         21
#define AR_PAR_GAIN_IN_1         22
#define AR_PAR_BIAS_IN_2         23
#define AR_PAR_GAIN_IN_2         24
#define AR_PAR_BIAS_IN_3         25
#define AR_PAR_GAIN_IN_3         26
#define AR_PAR_COMP_HI_LIM       27
#define AR_PAR_COMP_LO_LIM       28
#define AR_PAR_ARITH_TYPE        29
#define AR_PAR_BAL_TIME          30
#define AR_PAR_BIAS              31
#define AR_PAR_GAIN              32
#define AR_PAR_OUT_HI_LIM        33
#define AR_PAR_OUT_LO_LIM        34
#define AR_PAR_UPDATE_EVT        35
#define AR_PAR_BLOCK_ALM         36
#define AR_PAR_BLOCK_ERR_DESC_1  37



typedef struct _ARFB
{
  F_BLOCK           blk_data;
  USIGN16           st_rev;
  STRINGV           tag_desc[32];
  USIGN16           strategy;
  USIGN8            alert_key;
  MODE              mode_blk;
  BSTRING           block_err;
  FLOAT_S           pv;
  FLOAT_S           out;
  FLOAT_S           pre_out;
  SCALE             pv_scale;
  SCALE             out_range;
  ACCESS_PERM       grant_deny;
  BSTRING           input_opts;
  FLOAT_S           in;
  FLOAT_S           in_lo;
  FLOAT_S           in_1;
  FLOAT_S           in_2;
  FLOAT_S           in_3;
  FLOAT             range_hi;
  FLOAT             range_lo;
  FLOAT             bias_in_1;
  FLOAT             gain_in_1;
  FLOAT             bias_in_2;
  FLOAT             gain_in_2;
  FLOAT             bias_in_3;
  FLOAT             gain_in_3;
  FLOAT             comp_hi_lim;
  FLOAT             comp_lo_lim;
  USIGN8            arith_type;
  FLOAT             bal_time;
  FLOAT             bias;
  FLOAT             gain;
  FLOAT             out_hi_lim;
  FLOAT             out_lo_lim;
  EVENT             update_evt;
  ALARM_DISCRETE    block_alm;
  USIGN32           block_err_desc_1;
}
  T_ARFB;


typedef struct
{
  FLOAT             period_of_exec;
  FLOAT             time_since_man;
  FLOAT             out_diff;         /* diffence between OUT and function output         */
  FLOAT_S           out;
  USIGN16           old_block_alarm_bits;
}
  T_ARFB_DATA;



#define FLOW_COMP_LINEAR     1
#define FLOW_COMP_SQROOT     2
#define FLOW_COMP_APPROX     3
#define BTU_FLOW             4
#define MULTIPLY_DIVIDE      5
#define AVERAGE              6
#define SUMMER               7
#define FOURTH_ORDER_POLY    8
#define SIMPLE_HTG           9
#define FOURTH_ORDER_POLY_2 10


/* Definition for INPUT_OPT                     */
#define IN_USE_UNCERTAIN                 0x8000
#define IN_LO_USE_UNCERTAIN              0x4000
#define IN_1_USE_UNCERTAIN               0x2000
#define IN_1_USE_BAD                     0x1000
#define IN_2_USE_UNCERTAIN               0x0800
#define IN_2_USE_BAD                     0x0400
#define IN_3_USE_UNCERTAIN               0x0200
#define IN_3_USE_BAD                     0x0100

#define AR_INPUT_OPTS    (IN_USE_UNCERTAIN    |  \
                          IN_LO_USE_UNCERTAIN |  \
                          IN_1_USE_UNCERTAIN  |  \
                          IN_1_USE_BAD        |  \
                          IN_2_USE_UNCERTAIN  |  \
                          IN_2_USE_BAD        |  \
                          IN_3_USE_UNCERTAIN  |  \
                          IN_3_USE_BAD         )

#define AR_USE_UNCERTAIN (IN_USE_UNCERTAIN    |  \
                          IN_LO_USE_UNCERTAIN |  \
                          IN_1_USE_UNCERTAIN  |  \
                          IN_2_USE_UNCERTAIN  |  \
                          IN_3_USE_UNCERTAIN   )

#define AR_USE_BAD       (IN_1_USE_BAD        |  \
                          IN_2_USE_BAD        |  \
                          IN_3_USE_BAD         )



#define AR_BAD_INPUT                    0x00
#define AR_IN                           0x01
#define AR_IN_LO                        0x02
#define AR_PV                           0x04
#define AR_IN_1                         0x10
#define AR_IN_2                         0x20
#define AR_IN_3                         0x40

#define FLOW_COMP_LINEAR_INPUTS         (AR_PV|AR_IN_1|AR_IN_2)
#define FLOW_COMP_SQROOT_INPUTS         (AR_PV|AR_IN_1|AR_IN_2|AR_IN_3)
#define FLOW_COMP_APPROX_INPUTS         (AR_PV|AR_IN_1|AR_IN_2|AR_IN_3)
#define BTU_FLOW_INPUTS                 (AR_PV|AR_IN_1|AR_IN_2)
#define MULTIPLY_DIVIDE_INPUTS          (AR_PV|AR_IN_1|AR_IN_2|AR_IN_3)
#define SUMMER_INPUTS                   (AR_PV|AR_IN_1|AR_IN_2|AR_IN_3)
#define FORTH_ORDER_POLYNOM_INPUTS      (AR_PV|AR_IN_1|AR_IN_2|AR_IN_3)
#define SIMPLE_HTG_INPUTS               (AR_PV|AR_IN_1|AR_IN_2)


/* Definition for maximal float values                */
#define MAX_FLOAT_NEG       -1.175494351e-38        /* Max negatice value */
#define MAX_FLOAT_POS       3.402823466e+38         /* Max positive value */
#define MIN_FLOAT_POS       1.175494351e-38        /* Max negatice value */

#endif /* __FFBL_AR_H__ */



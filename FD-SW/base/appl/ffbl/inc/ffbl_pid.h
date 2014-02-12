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

FILE_NAME          ffbl_pid.h



DESCRIPTION        Definition of structures and identifiers for a PID block

=========================================================================== */
#ifndef __FFBL_PID_H__
#define __FFBL_PID_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/
#include "keywords.h"   /* keyword defines */

INCLUDES
#include "ffbl_mo.h"
#include "ffbl_pai.h"
#include "ffbl_pao.h"

#define PID_PAR_BLK_DATA      0
#define PID_PAR_ST_REV        1
#define PID_PAR_TAG_DESC      2
#define PID_PAR_STRATEGY      3
#define PID_PAR_ALERT_KEY     4
#define PID_PAR_MODE_BLK      5
#define PID_PAR_BLOCK_ERR     6
#define PID_PAR_PV            7
#define PID_PAR_SP            8
#define PID_PAR_OUT           9
#define PID_PAR_PV_SCALE      10
#define PID_PAR_OUT_SCALE     11
#define PID_PAR_GRANT_DENY    12
#define PID_PAR_CONTROL_OPTS  13
#define PID_PAR_STATUS_OPTS   14
#define PID_PAR_IN            15
#define PID_PAR_PV_FTIME      16
#define PID_PAR_BYPASS        17
#define PID_PAR_CAS_IN        18
#define PID_PAR_SP_RATE_DN    19
#define PID_PAR_SP_RATE_UP    20
#define PID_PAR_SP_HI_LIM     21
#define PID_PAR_SP_LO_LIM     22
#define PID_PAR_GAIN          23
#define PID_PAR_RESET         24
#define PID_PAR_BAL_TIME      25
#define PID_PAR_RATE          26
#define PID_PAR_BKCAL_IN      27
#define PID_PAR_OUT_HI_LIM    28
#define PID_PAR_OUT_LO_LIM    29
#define PID_PAR_BKCAL_HYS     30
#define PID_PAR_BKCAL_OUT     31
#define PID_PAR_RCAS_IN       32
#define PID_PAR_ROUT_IN       33
#define PID_PAR_SHED_OPT      34
#define PID_PAR_RCAS_OUT      35
#define PID_PAR_ROUT_OUT      36
#define PID_PAR_TRK_SCALE     37
#define PID_PAR_TRK_IN_D      38
#define PID_PAR_TRK_VAL       39
#define PID_PAR_FF_VAL        40
#define PID_PAR_FF_SCALE      41
#define PID_PAR_FF_GAIN       42
#define PID_PAR_UPDATE_EVT    43
#define PID_PAR_BLOCK_ALM     44
#define PID_PAR_ALARM_SUM     45
#define PID_PAR_ACK_OPTION    46
#define PID_PAR_ALARM_HYS     47
#define PID_PAR_HI_HI_PRI     48
#define PID_PAR_HI_HI_LIM     49
#define PID_PAR_HI_PRI        50
#define PID_PAR_HI_LIM        51
#define PID_PAR_LO_PRI        52
#define PID_PAR_LO_LIM        53
#define PID_PAR_LO_LO_PRI     54
#define PID_PAR_LO_LO_LIM     55
#define PID_PAR_DV_HI_PRI     56
#define PID_PAR_DV_HI_LIM     57
#define PID_PAR_DV_LO_PRI     58
#define PID_PAR_DV_LO_LIM     59
#define PID_PAR_HI_HI_ALM     60
#define PID_PAR_HI_ALM        61
#define PID_PAR_LO_ALM        62
#define PID_PAR_LO_LO_ALM     63
#define PID_PAR_DV_HI_ALM     64
#define PID_PAR_DV_LO_ALM     65
#define PID_PAR_BLOCK_ERR_DESC_1     66
/* additional parameters for the PID */
#define PID_ALGORITHM         67
#define PID_TFPD              68
#define PID_TFPID             69


/* *************************************** */
/*  begin additional defines for the PID   */
/*! \def PID_RESET_MIN
    Minimum value to avoid division by zero */
	#define   PID_RESET_MIN    1.0

/*! \def PID_RESET_MAX
    Maximum value */
	#define   PID_RESET_MAX    10000.0

/*! \def PID_RESET_OFF
    value to switch off */
	#define   PID_RESET_OFF    0.0

/*! \def PID_GAIN_MIN
    Minimum value to avoid division by zero */
	#define   PID_GAIN_MIN		0.05

/*! \def PID_GAIN_MAX
    Maximum value  */
	#define   PID_GAIN_MAX		20.0

	
/*! \def PID_GAIN_OFF
    value to switch off the GAIN */
	#define   PID_GAIN_OFF		0.0

/*! \def PID_TFPD_MIN
    Minimum value  */
	#define   PID_TFPD_MIN     0.5

/*! \def PID_TFPD_OFF
    Signals off value */
	#define   PID_TFPD_OFF     0.0

/*! \def PID_TFPID_MIN
    Minimal value for filter */
	#define   PID_TFPID_MIN    0.5

/*! \def PID_TFPID_OFF
    Minimal value for filter */
	#define   PID_TFPID_OFF    0.0

#define	PID_TF_FALSE	 1.0
#define   PID_ALGO_MIN	 (0)
#define   PID_ALGO_MAX	 (4)
/*   end additional defines for the PID    */
/* *************************************** */



typedef struct
{
  F_BLOCK        blk_data;
  USIGN16        st_rev;
  STRINGV        tag_desc[32];
  USIGN16        strategy;
  USIGN8         alert_key;
  MODE           mode_blk;
  USIGN16        block_err;
  FLOAT_S        pv;
  FLOAT_S        sp;
  FLOAT_S        out;
  SCALE          pv_scale;
  SCALE          out_scale;
  ACCESS_PERM    grant_deny;
  USIGN16        control_opts;
  USIGN16        status_opts;
  FLOAT_S        in;
  FLOAT          pv_ftime;
  USIGN8         bypass;
  FLOAT_S        cas_in;
  FLOAT          sp_rate_dn;
  FLOAT          sp_rate_up;
  FLOAT          sp_hi_lim;
  FLOAT          sp_lo_lim;
  FLOAT          gain;
  FLOAT          reset;
  FLOAT          bal_time;
  FLOAT          rate;
  FLOAT_S        bkcal_in;
  FLOAT          out_hi_lim;
  FLOAT          out_lo_lim;
  FLOAT          bkcal_hys;
  FLOAT_S        bkcal_out;
  FLOAT_S        rcas_in;
  FLOAT_S        rout_in;
  USIGN8         shed_opt;
  FLOAT_S        rcas_out;
  FLOAT_S        rout_out;
  SCALE          trk_scale;
  DISCRETE_S     trk_in_d;
  FLOAT_S        trk_val;
  FLOAT_S        ff_val;
  SCALE          ff_scale;
  FLOAT          ff_gain;
  EVENT          update_evt;
  ALARM_DISCRETE block_alm;
  ALARM_SUMMARY  alarm_sum;
  BSTRING        ack_option;
  FLOAT          alarm_hys;
  USIGN8         hi_hi_pri;
  FLOAT          hi_hi_lim;
  USIGN8         hi_pri;
  FLOAT          hi_lim;
  USIGN8         lo_pri;
  FLOAT          lo_lim;
  USIGN8         lo_lo_pri;
  FLOAT          lo_lo_lim;
  USIGN8         dv_hi_pri;
  FLOAT          dv_hi_lim;
  USIGN8         dv_lo_pri;
  FLOAT          dv_lo_lim;
  ALARM_FLOAT    hi_hi_alm;
  ALARM_FLOAT    hi_alm;
  ALARM_FLOAT    lo_alm;
  ALARM_FLOAT    lo_lo_alm;
  ALARM_FLOAT    dv_hi_alm;
  ALARM_FLOAT    dv_lo_alm;
  USIGN32        block_err_desc_1;
  /*! \brief additional parameter ALGORITHM */
  USIGN8         algorithm;
  /*! \brief additional parameter filter time */
  FLOAT			 tfpd;
  /*! \brief additional parameter filter time */
  FLOAT			 tfpid;

} T_PIDFB;


typedef struct
{
  FLOAT           working_sp;
  FLOAT           working_pv;
  FLOAT           ff_deviation;
  FLOAT           bkcal_in_value;
}
  T_PIDFB_CONTROL_DATA;



typedef struct
{
  FLOAT           period_of_exec;             /* Period of execution in seconds */
  BOOL            mode_was_written;
  USIGN8          new_target_mode;
  FLOAT_S         out;
  USIGN16         old_block_alarm_bits;
  T_SHED_DATA     shed;
  USIGN8          bypass_changed;
  USIGN8          tracking;
  FLOAT_S         working_trk_val;
  FLOAT_S         retained_sp;
  FLOAT_S         last_sp;
  FLOAT           pv_factor;
  BOOL            control_active;
  USIGN8          downstream;
  FLOAT           I_factor;
  FLOAT           D_factor;
  FLOAT           I_sum;
  FLOAT           FF_sum;
  FLOAT           B_time;
  FLOAT           bal_timer;
  FLOAT_S         feed_forward;
  FLOAT_S         last_feed_forward;
  FLOAT           last_deviation;
  FLOAT           last_control_value;
  FLOAT           out_lim_hys;
  /* additional parameters for the PID */
  /*! \brief parameter ALGORITHM last setting */
  USIGN8         last_algorithm;
  /*! \brief the GAIN */
  FLOAT           P_factor;
  /*! \brief is equal GAIN, dependent from the actual algorithm */
  FLOAT           kp_1;
  /*! \brief */
  /*! \brief is equal GAIN, dependent from the actual algorithm */
  FLOAT           kp_2;
  /*! \brief is equal RESET, dependent from the actual algorithm */
  FLOAT           ki;
  /*! \brief is equal RATE, dependent from the actual algorithm*/
  FLOAT           kd;
  /*! \brief is factor for filtering pre-calculated */
  FLOAT 		  tfpd_factor;
  /*! \brief is factor for filtering pre-calculated */
  FLOAT 		  tfpid_factor;
  /*! \brief last calculated filter value */
  FLOAT 		  last_tfpid;
  /*! \brief is equal GAIN, dependent from the actual algorithm */
  FLOAT           kpvl;
  /*! \brief is equal RATE, dependent from the actual algorithm */
  FLOAT           kpvc;
  /*! \brief is last PV */
  FLOAT           last_pv;
  /*! \brief is last differential value */
  FLOAT           last_pre_kd_value;
  /*! \brief is last output value */
  FLOAT			  last_outputvalue;	/* necessary for observe the controllable region */
  /*! \brief is last integral term */
  FLOAT			  last_I_sum;
  /*! \brief signals conditions for calculations of the integral term because of bumpless transfer */
  USIGN8		  bump_flag;
  /*! \brief is last target block mode */
  USIGN8		  last_new_target_mode;

}
  T_PIDFB_DATA;


#define PID_CNTRL_OPTS  CNTRL_OPT_BYPASS_ENABLE               |\
                        CNTRL_OPT_SP_PV_TRACK_IN_MAN          |\
                        CNTRL_OPT_SP_PV_TRACK_IN_ROUT         |\
                        CNTRL_OPT_SP_PV_TRACK_IN_LO_AND_IMAN  |\
                        CNTRL_OPT_TRACK_RETAINED_TARGET       |\
                        CNTRL_OPT_DIRECT_ACTING               |\
                        CNTRL_OPT_TRACK_IF_BAD_TRK_IN_D       |\
                        CNTRL_OPT_TRACK_ENABLE                |\
                        CNTRL_OPT_TRACK_IN_MANUAL             |\
                        CNTRL_OPT_USE_PV_FOR_BKCAL_OUT        |\
                        CNTRL_OPT_OBEY_SP_LIM_IF_CAS_OR_RCAS  |\
                        CNTRL_OPT_NO_OUT_LIM_IN_MAN

#define PID_STATUS_OPTS STATUS_OPT_IFS_IF_BAD_IN              |\
                        STATUS_OPT_IFS_IF_BAD_CAS_IN          |\
                        STATUS_OPT_USE_UNCERT_AS_GOOD         |\
                        STATUS_OPT_TARG_MAN_IF_BAD_IN         |\
                        STATUS_OPT_TARG_NEXT_IF_BAD_CAS_IN    |\
                        STATUS_OPT_TARG_MAN_IF_BAD_TRK_IN     |\
                        STATUS_OPT_IFS_IF_BAD_TRK_IN


#define PID_DOWNSTREAM_NOT_ACTIVE 0x00
#define PID_DOWNSTREAM_INIT       0x03
#define PID_DOWNSTREAM_ACTIVE     0x01

FUNCTION_DECLARATIONS

/*****************************************************************************/
/* public function declarations from file:   ffbl_krn.c                      */
/*****************************************************************************/

FUNCTION /*TBD DISABLED this for WIN32 TEST --> PUBLIC*/ FLOAT pid_control
  (
    IN T_PIDFB *                p_pidfb,
    IN T_PIDFB_DATA *           p_pidfb_data,
    IN T_PIDFB_CONTROL_DATA *   p_pidfb_control_data
  );
  
  
  /* *************************************** */
  /*  begin additional function for the PID  */
 
 FUNCTION /*TBD DISABLED this for WIN32 TEST --> PUBLIC*/ VOID pid_re_calc
  (
    IN T_PIDFB *                p_pidfb,
    IN T_PIDFB_DATA *           p_pidfb_data
  );
 
 /*   end additional function for the PID   */
 /* *************************************** */  
  
#endif /* __FFBL_PID_H__  */

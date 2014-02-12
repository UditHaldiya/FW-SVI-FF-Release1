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

FILE_NAME          ffbl_int.h



FUNCTIONAL_MODULE_DESCRIPTION

Module description

=========================================================================== */
#ifndef __FFBL_INT__
#define __FFBL_INT__

#include "keywords.h"   /* keyword defines */

INCLUDES
   #include "ffbl_api.h"

GLOBAL_DEFINES

/* Macros und definitions ------------------------------------------------- */
/* ------------------------------------------------------------------------ */

#define AI_PROFILE_NUMBER      0x101
#define AO_PROFILE_NUMBER      0x102
#define DI_PROFILE_NUMBER      0x103
#define DO_PROFILE_NUMBER      0x104
#define PID_PROFILE_NUMBER     0x108
#define RES_PROFILE_NUMBER     0x10B
#define RES_2_PROFILE_NUMBER   0x133

#define MODE_RES_VALID   (MODE_OS | MODE_AUTO)
#define MODE_AI_VALID    (MODE_OS | MODE_MAN | MODE_AUTO)
#define MODE_DI_VALID    (MODE_OS | MODE_MAN | MODE_AUTO)
#define MODE_AO_VALID    (MODE_OS | MODE_MAN | MODE_AUTO | MODE_CAS | MODE_RCAS)
#define MODE_PID_VALID   (MODE_OS | MODE_MAN | MODE_AUTO | MODE_CAS | MODE_RCAS | MODE_ROUT)
#define MODE_DO_VALID    (MODE_OS | MODE_MAN | MODE_AUTO | MODE_CAS | MODE_RCAS)
#define MODE_MAI_VALID   (MODE_OS | MODE_MAN | MODE_AUTO)

#define MODE_IT_VALID    (MODE_OS | MODE_MAN | MODE_AUTO)
#define MODE_AR_VALID    (MODE_OS | MODE_MAN | MODE_AUTO)
#define MODE_SC_VALID    (MODE_OS | MODE_MAN | MODE_AUTO)
#define MODE_IS_VALID    (MODE_OS | MODE_MAN | MODE_AUTO)


#define MODE_NOT_OS(m)           (!(m & MODE_OS))
#define MODE_LOWER_THAN_MAN(m)   (!(m & (MODE_OS|MODE_MAN)))
#define MODE_LOWER_THAN_AUTO(m)  (!((m & (MODE_OS|MODE_MAN))||(m == MODE_AUTO)))

#define MODE_OK          0x00
#define MODE_INTO_OS     0x01
#define MODE_OUTOF_OS    0x02
#define MODE_INTO_MAN    0x03
#define MODE_IN_OS       0x04

#define _ALARM_ACTIVE(alarm)       (  ((alarm) == ALARM_ACTIVE_REP)  \
                                    ||((alarm) == ALARM_ACTIVE_NOTREP) )

#define _ALARM_NOT_REPORTED(alarm) (  ((alarm) == ALARM_CLEAR_NOTREP)  \
                                    ||((alarm) == ALARM_ACTIVE_NOTREP) )

#define _ALARM_NOT_ACKNOWLEDGED(unack) ((unack) == ALARM_UNACKED)


#define ALERT_UNDEF                      0
#define ALERT_LO_LIM                     1
#define ALERT_HI_LIM                     2
#define ALERT_LO_LO_LIM                  3
#define ALERT_HI_HI_LIM                  4
#define ALERT_DV_LO                      5
#define ALERT_DV_HI                      6
#define ALERT_DISC                       7
#define ALERT_BLOCK                      8
#define ALERT_UPDATE                     9
#define ALERT_WRITE                     10
#define ALERT_UPD_LINK                  11
#define ALERT_UPD_TREND                 12

#define AUTO_ACK_DISABLED                0
#define AUTO_ACK_ENABLED                 1

#define WRITE_PRI_MIN                    0
#define WRITE_PRI_MAX                   15

#define SIMULATION_DISABLED              1
#define SIMULATION_ENABLED               2

#define ALL_ALARMS_ENABLED            0x00  /* All alarms are enabled */
#define AUTO_ALARM_ACKNOWLEDGE        0xFF  /* All alarms are auto acknowledged */



/*------------------ Hard Types ---------------------------*/
#define HARDWARE_SCALAR_INPUT                0x8000
#define HARDWARE_SCALAR_OUTPUT               0x4000
#define HARDWARE_DISCRETE_INPUT              0x2000
#define HARDWARE_DISCRETE_OUTPUT             0x1000

/*------------------ Resource features --------------------*/
#define RES_FEAT_REPORTS_SUPP                0x4000
#define RES_FEAT_FAULT_STATE_SUPP            0x2000
#define RES_FEAT_SOFT_WRITE_LOCK_SUPP        0x1000
#define RES_FEAT_HARD_WRITE_LOCK_SUPP        0x0800
#define RES_FEAT_OUT_READBACK_SUPP           0x0400

/*--------------------- IO options ------------------------*/
#define IO_OPT_INVERT                        0x8000
#define IO_OPT_SP_PV_TRACK_IN_MAN            0x4000
#define IO_OPT_RESERVED                      0x2000
#define IO_OPT_SP_PV_TRACK_IN_LO             0x1000
#define IO_OPT_SP_TRACK_RET_TARGET           0x0800
#define IO_OPT_INC_TO_CLOSE                  0x0400
#define IO_OPT_FSTATE_TO_VAL                 0x0200
#define IO_OPT_FSTATE_TO_VAL_ON_RESTART      0x0100
#define IO_OPT_TARGET_TO_MAN_IF_FSTATE       0x0080
#define IO_OPT_PV_FOR_BKCAL_OUT              0x0040
#define IO_OPT_LOW_CUTOFF                    0x0020

/*------------------- Status options ----------------------*/
#define STATUS_OPT_IFS_IF_BAD_IN             0x8000
#define STATUS_OPT_IFS_IF_BAD_CAS_IN         0x4000
#define STATUS_OPT_USE_UNCERT_AS_GOOD        0x2000
#define STATUS_OPT_PROP_FLT_FORW             0x1000
#define STATUS_OPT_PROP_FLT_BACKW            0x0800
#define STATUS_OPT_TARG_MAN_IF_BAD_IN        0x0400
#define STATUS_OPT_UNCERT_IF_LIM             0x0200
#define STATUS_OPT_BAD_IF_LIM                0x0100
#define STATUS_OPT_UNCERT_IF_MAN             0x0080
#define STATUS_OPT_TARG_NEXT_IF_BAD_CAS_IN   0x0040
#define STATUS_OPT_TARG_MAN_IF_BAD_TRK_IN    0x0020
#define STATUS_OPT_IFS_IF_BAD_TRK_IN         0x0010

/*------------------- Control options ---------------------*/
#define CNTRL_OPT_BYPASS_ENABLE              0x8000
#define CNTRL_OPT_SP_PV_TRACK_IN_MAN         0x4000
#define CNTRL_OPT_SP_PV_TRACK_IN_ROUT        0x2000
#define CNTRL_OPT_SP_PV_TRACK_IN_LO_AND_IMAN 0x1000
#define CNTRL_OPT_TRACK_RETAINED_TARGET      0x0800
#define CNTRL_OPT_DIRECT_ACTING              0x0400
#define CNTRL_OPT_TRACK_IF_BAD_TRK_IN_D      0x0200
#define CNTRL_OPT_TRACK_ENABLE               0x0100
#define CNTRL_OPT_TRACK_IN_MANUAL            0x0080
#define CNTRL_OPT_USE_PV_FOR_BKCAL_OUT       0x0040
#define CNTRL_OPT_OBEY_SP_LIM_IF_CAS_OR_RCAS 0x0008
#define CNTRL_OPT_NO_OUT_LIM_IN_MAN          0x0004

/*------------------- Shedding options ---------------------*/
#define SHED_UNDEFINED                       0
#define SHED_NORMAL_NORMAL_RETURN            1
#define SHED_NORMAL_NO_RETURN                2
#define SHED_TO_AUTO_NORMAL_RETURN           3
#define SHED_TO_AUTO_NO_RETURN               4
#define SHED_TO_MAN_NORMAL_RETURN            5
#define SHED_TO_MAN_NO_RETURN                6
#define SHED_TO_RETAINED_NORMAL_RETURN       7
#define SHED_TO_RETAINED_NO_RETURN           8


/*----------------- BYPASS options for PID ----------------*/
#define BYPASS_UNINITIALIZED             0
#define BYPASS_OFF                       1
#define BYPASS_ON                        2

/*-------------------- SIGNUM constants -------------------*/
#define POSITIVE 1
#define NEGATIVE 2

/*--------------- Block Error Description Bits ------------*/
#define BLK_ERR_CHANNEL_INIT            0x40000000
#define BLK_ERR_CHANNEL_SETTING         0x20000000
#define BLK_ERR_CH_UNIT                 0x10000000
#define BLK_ERR_L_TYP_UNINIT            0x08000000
#define BLK_ERR_L_TYP_DIRECT_SET        0x04000000
#define BLK_ERR_XD_SCALE_LIM_SET        0x02000000
#define BLK_ERR_OUT_SCALE_LIM_SET       0x01000000
#define BLK_ERR_PV_SCALE_LIM_SET        0x00800000
#define BLK_ERR_TRK_SCALE_LIM_SET       0x00400000
#define BLK_ERR_FF_SCALE_LIM_SET        0x00200000
#define BLK_ERR_BYPASS_SET              0x00100000
#define BLK_ERR_SHED_OPT_SET            0x00080000
#define BLK_ERR_SP_LIM_SET              0x00040000
#define BLK_ERR_OUT_LIM_SET             0x00020000
/* BLOCK_ERR_DESC_1 For ARFB,ISFB,ITFB */
#define BLK_ERR_ARITH_TYPE_UNINIT       0x00010000
#define BLK_ERR_SELECT_TYPE_UNINIT      0x00008000
#define BLK_ERR_INTEG_TYPE_UNINIT       0x00004000
/* BLOCK_ERR_DESC_1 For ARFB*/
#define BLK_ERR_RANGE_SET               0x00002000
#define BLK_ERR_COMP_LIM_SET            0x00001000
/* BLOCK_ERR_DESC_1 For SCFB*/
#define BLK_ERR_CURVE_X_INCONSIST       0x00000800
#define BLK_ERR_CURVE_X_INMONTONIC      0x00000400
#define BLK_ERR_CURVE_Y_INCONSIST       0x00000200
#define BLK_ERR_SWAP_2_UNINIT           0x00000100
#define BLK_ERR_CURVE_Y_INMONTONIC      0x00000080
/* BLOCK_ERR_DESC_1 for OSFB */
#define BLK_ERR_IN_ARR_INCONSIST        0x00000040
#define BLK_ERR_LOCKVAL_INIT            0x00000020
#define BLK_ERR_EXE_TIME                0x00000001

LOCAL_DEFINES

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

FUNCTION PUBLIC USIGN8 ffbl_bas_calc_block_mode
  (
    MODE *    p_mode,
    BSTRING   block_err
  );


FUNCTION PUBLIC USIGN8 ffbl_bas_determine_good_nc_status
  (
    IN ALARM_DISCRETE *   p_block_alm
  );


FUNCTION PUBLIC USIGN8 ffbl_bas_check_sim_param
  (
    IN USIGN8     enable_flag
  );


FUNCTION VOID ffbl_bas_update_sim_param
  (
    IN USIGN8 *   enable_flag,
    IN USIGN16 *  block_err
  );


FUNCTION PUBLIC VOID ffbl_bas_check_scale_limits
  (
    INOUT FLOAT *           a_of_value,
    IN    SCALE *           scale_ptr
  );


FUNCTION GLOBAL USIGN16 ffbl_bas_check_channel
  (
    IN USIGN16                     channel,
    IN const T_FBIF_BLOCK_DESCR *  p_block_desc
  );


FUNCTION PUBLIC VOID ffbl_bas_write_grant_deny
  (
    IN USIGN8           subindex,
    IN USIGN8 *         source,
    IN ACCESS_PERM *    p_grant_deny
  );


FUNCTION GLOBAL VOID ffbl_bas_update_extended_block_alarm
  (
    IN const T_FBIF_BLOCK_DESCR * p_block_desc,
    INOUT ALARM_DISCRETE *        p_block_alm,
    IN BSTRING                    alm_disable,
    IN BSTRING                    ack_option,
    IN USIGN16                    block_alarm_bits,
    INOUT USIGN16 *               p_old_block_alarm_bits
  );


FUNCTION PUBLIC FLOAT ffbl_bas_rescale_value
  (
    IN FLOAT    old_value,
    IN SCALE *  old_scale_p,
    IN SCALE *  new_scale_p
  );


FUNCTION PUBLIC VOID ffbl_fd_start
  (
    IN const T_FBIF_BLOCK_DESCR    * p_block_desc
  );


FUNCTION PUBLIC VOID ffbl_fd_reset_simulate (VOID);


FUNCTION PUBLIC VOID ffbl_fd_background (VOID);


FUNCTION PUBLIC USIGN16 ffbl_fd_pre_write_check
  (
    IN T_FBIF_WRITE_DATA *        p_write
  );


FUNCTION PUBLIC VOID ffbl_fd_update_alarm_sum
  (
    INOUT ALARM_SUMMARY *  p_alarm_sum
  );


#endif /* __FFBL_INT__ */


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

FILE_NAME          fbap.h


DESCRIPTION
  This file contains macros and definitions derived from FF function block
  specifications FF-890 and FF-891.

  The definition of some of the structures includes hidden attributes. These
  attributes are not shown in the object description, because they are not
  visible for communication. These attributes are used by the function block
  shell and function block layer.

=========================================================================== */
#ifndef __FBAP_H__
#define __FBAP_H__

/* ----------------------------------------------------- */
/* Profile Numbers of FF Function Blocks                 */
/* ----------------------------------------------------- */
#define PROFILE_CB_BLK      0x0000  /* Custom Block */
#define PROFILE_AI_BLK      0x0101  /* Analog Input (AI) */
#define PROFILE_AO_BLK      0x0102  /* Analog Output (AO) */
#define PROFILE_DI_BLK      0x0103  /* Discrete Input (DI) */
#define PROFILE_DO_BLK      0x0104  /* Discrete Output (DO) */
#define PROFILE_BG_BLK      0x0105  /* Bias/Gain (BG) */
#define PROFILE_ML_BLK      0x0106  /* Manual Loader (ML) */
#define PROFILE_PD_BLK      0x0107  /* Proportional-Differential (PD) */
#define PROFILE_PID_BLK     0x0108  /* Proportional-Integral-Differential (PID) */
#define PROFILE_RA_BLK      0x0109  /* Ratio (RA) */
#define PROFILE_CS_BLK      0x010A  /* Control Selector (CS) */
#define PROFILE_RB_BLK      0x010B  /* Resource (RB) */
#define PROFILE_DC_BLK      0x011B  /* Device Control (DC) */
#define PROFILE_OS_BLK      0x011C  /* Output Splitter (OS) */
#define PROFILE_SC_BLK      0x011D  /* Signal Characterizer (SC) */
#define PROFILE_LL_BLK      0x011E  /* Lead Lag (LL) */
#define PROFILE_DT_BLK      0x011F  /* Deadtime (DT) */
#define PROFILE_IT_BLK      0x0120  /* Integrator (IT) */
#define PROFILE_PI_BLK      0x0121  /* Pulse Input (PI) */
#define PROFILE_CAO_BLK     0x0122  /* Complex AO (CAO) */
#define PROFILE_CDO_BLK     0x0123  /* Complex DO (CDO) */
#define PROFILE_SOPID_BLK   0x0124  /* Step Output PID (SOPID) */
#define PROFILE_SPG_BLK     0x0125  /* Setpoint Ramp Generator (SPG) */
#define PROFILE_IS_BLK      0x0126  /* Input Selector (IS) */
#define PROFILE_AR_BLK      0x0127  /* Arithmetic (AR) */
#define PROFILE_CAL_BLK     0x0128  /* Calculate (CAL) */
#define PROFILE_TMR_BLK     0x0129  /* Timer (TMR) */
#define PROFILE_AAL_BLK     0x012A  /* Analog Alarm (AAL) */
#define PROFILE_DAL_BLK     0x012B  /* Discrete Alarm (DAL) */
#define PROFILE_AHI_BLK     0x012C  /* Analog Human Interface (AHI) */
#define PROFILE_DHI_BLK     0x012D  /* Discrete Human Interface (DHI) */
#define PROFILE_MDI_BLK     0x012E  /* Multiple Discrete Input (MDI) */
#define PROFILE_MDO_BLK     0x012F  /* Multiple Discrete Output (MDO) */
#define PROFILE_MAI_BLK     0x0130  /* Multiple Analog Input (MAI) */
#define PROFILE_MAO_BLK     0x0131  /* Multiple Analog Output (MAO) */
#define PROFILE_RB2_BLK     0x0133  /* Resource 2 (RB) */

/* ----------------------------------------------------- */
/* Block Modes                                           */
/* ----------------------------------------------------- */
#define MODE_OS               0x01
#define MODE_IMAN             0x02
#define MODE_LO               0x04
#define MODE_MAN              0x08
#define MODE_AUTO             0x10
#define MODE_CAS              0x20
#define MODE_RCAS             0x40
#define MODE_ROUT             0x80


/* ----------------------------------------------------- */
/* Relative indexes of Standard Block Parameters         */
/* ----------------------------------------------------- */
#define REL_IDX_BLK_HDR          0
#define REL_IDX_ST_REV           1
#define REL_IDX_TAG_DESC         2
#define REL_IDX_STRATEGY         3
#define REL_IDX_ALERT_KEY        4
#define REL_IDX_MODE             5
#define REL_IDX_BLOCK_ERR        6


/* ----------------------------------------------------- */
/* Resource Hard Types                                   */
/* ----------------------------------------------------- */
#define HARDWARE_SCALAR_INPUT            0x8000
#define HARDWARE_SCALAR_OUTPUT           0x4000
#define HARDWARE_DISCRETE_INPUT          0x2000
#define HARDWARE_DISCRETE_OUTPUT         0x1000

/* ----------------------------------------------------- */
/* Resource Features                                     */
/* ----------------------------------------------------- */
#define RES_FEAT_REPORTS_SUPP            0x4000
#define RES_FEAT_FAULT_STATE_SUPP        0x2000
#define RES_FEAT_SOFT_WRITE_LOCK_SUPP    0x1000
#define RES_FEAT_HARD_WRITE_LOCK_SUPP    0x0800
#define RES_FEAT_OUT_READBACK_SUPP       0x0400
#define RES_FEAT_RPT_MVC_SUPP            0x0080
#define RES_FEAT_PUB_SUB_MVC_SUPP        0x0040
#define RES_FEAT_MULTIBIT_ALARM_SUPP     0x0020

/* ----------------------------------------------------- */
/* Resource Restart: Commands and States                 */
/* ----------------------------------------------------- */
#define RS_RESTART_RUN                                1
#define RS_RESTART_RESOURCE                           2
#define RS_RESTART_WITH_DEFAULTS                      3
#define RS_RESTART_PROCESSOR                          4
#define RS_RESTART_FACTORY_DEFAULTS                   5
#define RS_RESET_BLOCKS                              11
#define RS_RESTART_TEST_WATCHDOG                     30
#define RS_RESTART_HIDDEN                            42
#define RS_RESTART_MAX_VALUE       RS_RESTART_FACTORY_DEFAULTS


/* ----------------------------------------------------- */
/* Status Byte of I/O parameters                         */
/* ----------------------------------------------------- */

/* Status Byte Masks */
#define QUALITY_MASK                    0xC0 /* 11000000 */
#define SUB_STATUS_MASK                 0x3C /* 00111100 */
#define LIMIT_MASK                      0x03 /* 00000011 */

/* Quality */
#define SQ_BAD                       (0 * 0x40)
#define SQ_UNCERTAIN                 (1 * 0x40)
#define SQ_GOOD_NC                   (2 * 0x40)
#define SQ_GOOD_CAS                  (3 * 0x40)
#define SQ_GOOD                      (2 * 0x40)

/* Substatus for all Qualities */
#define SUB_NON_SPECIFIC             (0 * 0x04)

/* Substatus for BAD Quality */
#define SUB_CONFIGURATION_ERROR      (1 * 0x04)
#define SUB_NOT_CONNECTED            (2 * 0x04)
#define SUB_DEVICE_FAILURE           (3 * 0x04)
#define SUB_SENSOR_FAILURE           (4 * 0x04)
#define SUB_NO_COMM_WITH_LUV         (5 * 0x04)
#define SUB_NO_COMM_NO_LUV           (6 * 0x04)
#define SUB_OUT_OF_SERVICE           (7 * 0x04)
#define SUB_BAD_TB_IN_MANUAL         (8 * 0x04)

/* Substatus for UNCERTAIN Quality */
#define SUB_LUV                      (1 * 0x04)
#define SUB_SUBSTITUTE               (2 * 0x04)
#define SUB_INITIAL_VALUE            (3 * 0x04)
#define SUB_SENSOR_CONV_NOT_ACCURATE (4 * 0x04)
#define SUB_EU_RANGE_VIOLATION       (5 * 0x04)
#define SUB_SUB_NORMAL               (6 * 0x04)
#define SUB_UNCERT_TB_IN_MANUAL      (7 * 0x04)

/* Substatus for GOOD (Non-cascade) Quality */
#define SUB_ACTIVE_BLOCK_ALARM       (1 * 0x04)
#define SUB_ACTIVE_ADVISORY_ALARM    (2 * 0x04)
#define SUB_ACTIVE_CRITICAL_ALARM    (3 * 0x04)
#define SUB_UNACK_BLOCK_ALARM        (4 * 0x04)
#define SUB_UNACK_ADVISORY_ALARM     (5 * 0x04)
#define SUB_UNACK_CRITICAL_ALARM     (6 * 0x04)

/* Substatus for GOOD (Cascade) Quality */
#define SUB_IA                       (1 * 0x04)
#define SUB_IR                       (2 * 0x04)
#define SUB_NI                       (3 * 0x04)
#define SUB_NS                       (4 * 0x04)
#define SUB_DNS                      (5 * 0x04)
#define SUB_LO                       (6 * 0x04)
#define SUB_FSA                      (7 * 0x04)
#define SUB_IFS                      (8 * 0x04)

/* Limit Bits */
#define NO_LIMIT                         0
#define LIMIT_LOW                        1
#define LIMIT_HIGH                       2
#define LIMIT_CONST                      3



/* ----------------------------------------------------- */
/* States of an Alarm                                    */
/* ----------------------------------------------------- */
#define  ALARM_UNDEF               0
#define  ALARM_CLEAR_REP           1
#define  ALARM_CLEAR_NOTREP        2
#define  ALARM_ACTIVE_REP          3
#define  ALARM_ACTIVE_NOTREP       4

/* ----------------------------------------------------- */
/* States of an Update Event                             */
/* ----------------------------------------------------- */
#define  EVENT_UNDEF               0
#define  EVENT_REP                 1
#define  EVENT_NOTREP              2

/* ----------------------------------------------------- */
/* Unacknowledged State of an Alert                      */
/* ----------------------------------------------------- */
#define  ALARM_ACKED               1
#define  ALARM_UNACKED             2

/* ----------------------------------------------------- */
/* Message type of an Alert                              */
/* ----------------------------------------------------- */
#define  ALERT_MSG_UNDEF           0
#define  ALERT_MSG_EVENT           1
#define  ALERT_MSG_ALARM_CLEAR     2
#define  ALERT_MSG_ALARM_ACTIVE    3
#define  ALERT_MSG_MBIT_CLEAR      4
#define  ALERT_MSG_MBIT_ACTIVE     5

/* ----------------------------------------------------- */
/* Bit Positions in the ALARM_SUM parameter              */
/* ----------------------------------------------------- */
#define  ALARM_SUM_DISC       0x8000
#define  ALARM_SUM_HI_HI      0x4000
#define  ALARM_SUM_HI         0x2000
#define  ALARM_SUM_LO_LO      0x1000
#define  ALARM_SUM_LO         0x0800
#define  ALARM_SUM_DEV_HI     0x0400
#define  ALARM_SUM_DEV_LO     0x0200
#define  ALARM_SUM_BLOCK      0x0100
#define  ALARM_SUM_FD_FAIL    0x0080
#define  ALARM_SUM_FD_OFFSPEC 0x0040
#define  ALARM_SUM_FD_MAINT   0x0020
#define  ALARM_SUM_FD_CHECK   0x0010


/* ----------------------------------------------------- */
/* Default Alert Prio for Block Alarms and Update Events */
/* ----------------------------------------------------- */
#define ALERT_PRIO_2            0x02


/* ----------------------------------------------------- */
/* Block Error Bits                                      */
/* ----------------------------------------------------- */
#define BLK_ERR_OTHER                    0x8000
#define BLK_ERR_BLOCK_CONFIG_ERR         0x4000
#define BLK_ERR_LINK_CONFIG_ERR          0x2000
#define BLK_ERR_SIMULATE_ACTIVE          0x1000
#define BLK_ERR_LOCAL_OVERRIDE           0x0800
#define BLK_ERR_DEV_FAULT_STATE_SET      0x0400
#define BLK_ERR_DEV_NEEDS_MAINT_SOON     0x0200
#define BLK_ERR_INPUT_FAILURE            0x0100
#define BLK_ERR_OUTPUT_FAILURE           0x0080
#define BLK_ERR_MEMORY_FAILURE           0x0040
#define BLK_ERR_LOST_STATIC_DATA         0x0020
#define BLK_ERR_LOST_NV_DATA             0x0010
#define BLK_ERR_READBACK_CHECK_FAILED    0x0008
#define BLK_ERR_DEV_NEEDS_MAINT_NOW      0x0004
#define BLK_ERR_POWER_UP                 0x0002
#define BLK_ERR_OUT_OF_SERVICE           0x0001
#define BLK_ERR_NO_ERROR                 0x0000

/* ----------------------------------------------------- */
/* Block Alarm Codes                                     */
/* ----------------------------------------------------- */
#define ALM_SC_OTHER                        0
#define ALM_SC_BLOCK_CONFIG_ERR             1
#define ALM_SC_LINK_CONFIG_ERR              2
#define ALM_SC_SIMULATE_ACTIVE              3
#define ALM_SC_LOCAL_OVERRIDE               4
#define ALM_SC_DEV_FAIL_SAFE_SET            5
#define ALM_SC_DEV_NEEDS_MAINT_SOON         6
#define ALM_SC_INPUT_FAILURE                7
#define ALM_SC_OUTPUT_FAILURE               8
#define ALM_SC_MEMORY_FAILURE               9
#define ALM_SC_LOST_STATIC_DATA            10
#define ALM_SC_LOST_NV_DATA                11
#define ALM_SC_READBACK_CHECK_FAILED       12
#define ALM_SC_DEV_NEEDS_MAINT_NOW         13
#define ALM_SC_POWER_UP                    14
#define ALM_SC_OUT_OF_SERVICE              15



/* ----------------------------------------------------- */
/* Types of Link Objects                                 */
/* ----------------------------------------------------- */
#define  UNDEF_LINK                0
#define  LOCAL_LINK                1
#define  PUB_LINK                  2
#define  SUB_LINK                  3
#define  ALERT_LINK                6
#define  TREND_LINK                7
#define  MVC_PUB_LINK              8
#define  MVC_SUB_LINK              9
#define  MVC_RPT_LINK             10

/* ----------------------------------------------------- */
/* Types of Multi Variable Container Objects (MVCs)      */
/* ----------------------------------------------------- */
#define  MVC_UNDEF_OBJ         UNDEF_LINK
#define  MVC_PUB_OBJ           MVC_PUB_LINK
#define  MVC_SUB_OBJ           MVC_SUB_LINK
#define  MVC_RPT_OBJ           MVC_RPT_LINK


/* ----------------------------------------------------- */
/* Sample Types of Trend Objects                         */
/* ----------------------------------------------------- */
#define  TREND_INST                1
#define  TREND_AVERAGE             2


/* ----------------------------------------------------- */
/* Actions for Block Instantiation                       */
/* ----------------------------------------------------- */
#define ACT_OBJ_IGNORE             0
#define ACT_OBJ_CREATE             1
#define ACT_OBJ_DELETE             2


/* ----------------------------------------------------- */
/* FF Standard Types (Which are not Covered by C-Types)  */
/* ----------------------------------------------------- */
typedef  struct
{
  USIGN16      msec;
  INT8         minute;
  INT8         hour;
  INT8         day;
  INT8         month;
  INT8         year;
}
  PACKED DATE_S;        /* DS_11 */

typedef  struct
{
  USIGN32      msec;
  USIGN16      day;
}
  PACKED TIME_OF_DAY_S; /* DS_12 */

typedef  struct
{
  USIGN32      msec;
  USIGN16      day;
}
  PACKED TIME_DIFF_S;   /* DS_13 */

typedef  struct
{
  USIGN32      high;
  USIGN32      low;
}
  PACKED TIME_VALUE_S;  /* DS_21 */


/* ----------------------------------------------------- */
/* FF Standard Data Structures                           */
/* ----------------------------------------------------- */

typedef struct
{
  STRINGV      block_tag[32];
  USIGN32      dd_name;
  USIGN32      dd_item;
  USIGN16      dd_revision;
  USIGN16      profile;
  USIGN16      profile_revision;
  USIGN32      execution_time;
  USIGN32      period_of_execution;
  USIGN16      number_of_parameters;
  USIGN16      next_fb_to_execute;
  USIGN16      starting_index_of_views;
  USIGN8       number_of_view_3;
  USIGN8       number_of_view_4;
}
  PACKED F_BLOCK;             /* DS_64 */

typedef struct
{
  USIGN8       status;
  FLOAT        value;
}
  PACKED FLOAT_S;             /* DS_65 */

typedef struct
{
  USIGN8       status;
  USIGN8       value;
}
  PACKED DISCRETE_S;          /* DS_66 */

typedef struct
{
  USIGN8       status;
  BSTRING      value;
}
  PACKED BIT_STRING_S;        /* DS_67 */

typedef struct
{
  FLOAT        eu_100;
  FLOAT        eu_0;
  USIGN16      units;
  INT8         dec_pt;
}
  PACKED SCALE;               /* DS_68 */


typedef struct
{
  USIGN8       target;
  USIGN8       actual;
  USIGN8       permit;
  USIGN8       normal;
}
  PACKED MODE;                /* DS_69 */

typedef struct
{
  USIGN8       grant;
  USIGN8       denied;
}
  PACKED ACCESS_PERM;         /* DS_70 */


/*  The following structures have hidden attributes: DS_71, DS_72, DS_73 */

typedef struct
{
  USIGN8       unack;
  USIGN8       state;
  TIME_VALUE_S time_stamp;
  USIGN16      subcode;
  FLOAT        value;
  USIGN8       current;           /* hidden attribute - current state */
  USIGN8       standard_type;     /* hidden attribute */
  USIGN8       mfgr_type;         /* hidden attribute */
  USIGN16      unit_index;        /* hidden attribute */
}
  PACKED ALARM_FLOAT;         /* DS_71 */

typedef struct
{
  USIGN8       unack;
  USIGN8       state;
  TIME_VALUE_S time_stamp;
  USIGN16      subcode;
  USIGN8       value;
  USIGN8       current;           /* hidden attribute - current state */
  USIGN8       standard_type;     /* hidden attribute */
  USIGN8       mfgr_type;         /* hidden attribute */
  USIGN16      unit_index;        /* hidden attribute */
}
  PACKED ALARM_DISCRETE;      /* DS_72 */

typedef struct
{
  USIGN8       unack;
  USIGN8       state;
  TIME_VALUE_S time_stamp;
  USIGN16      static_revision;
  USIGN16      relative_index;
  USIGN8       standard_type;     /* hidden attribute */
  USIGN8       mfgr_type;         /* hidden attribute */
}
  PACKED EVENT;               /* DS_73 */

typedef struct
{
  BSTRING      current;
  BSTRING      unack;
  BSTRING      unrep;
  BSTRING      disable;
}
  PACKED ALARM_SUMMARY;       /* DS_74 */

typedef struct
{
  USIGN16      block_index;
  USIGN8       alert_key;
  USIGN8       standard_type;
  USIGN8       mfgr_type;
  USIGN8       message_type;
  USIGN8       priority;
  TIME_VALUE_S time_stamp;
  USIGN16      subcode;
  FLOAT        value;
  USIGN16      relative_index;
  USIGN16      unit_index;
}
  PACKED ALERT_FLOAT;         /* DS_75 */

typedef struct
{
  USIGN16      block_index;
  USIGN8       alert_key;
  USIGN8       standard_type;
  USIGN8       mfgr_type;
  USIGN8       message_type;
  USIGN8       priority;
  TIME_VALUE_S time_stamp;
  USIGN16      subcode;
  USIGN8       value;
  USIGN16      relative_index;
  USIGN16      unit_index;
}
  PACKED ALERT_DISCRETE;      /* DS_76 */

typedef struct
{
  USIGN16      block_index;
  USIGN8       alert_key;
  USIGN8       standard_type;
  USIGN8       mfgr_type;
  USIGN8       message_type;
  USIGN8       priority;
  TIME_VALUE_S time_stamp;
  USIGN16      static_revision;
  USIGN16      relative_index;
}
  PACKED ALERT_EVENT;         /* DS_77 */


typedef struct
{
  USIGN8       status;
  FLOAT        value;
  USIGN8       field_status;
  FLOAT        field_value;
  USIGN8       enable;
}
  PACKED SIMULATE_FLOAT;      /* DS_82 */

typedef struct
{
  USIGN8       status;
  USIGN8       value;
  USIGN8       field_status;
  USIGN8       field_value;
  USIGN8       enable;
}
  PACKED SIMULATE_DISCRETE;   /* DS_83 */

typedef struct
{
  USIGN8       status;
  BSTRING      value;
  USIGN8       field_status;
  BSTRING      field_value;
  USIGN8       enable;
}
  PACKED SIMULATE_BIT_STRING; /* DS_84 */

typedef struct
{
  BOOL          value1;
  INT8          value2;
  INT16         value3;
  INT32         value4;
  USIGN8        value5;
  USIGN16       value6;
  USIGN32       value7;
  FLOAT         value8;
  STRINGV       value9[32];
  OSTRING       value10[32];
  DATE_S        value11;
  TIME_OF_DAY_S value12;
  TIME_DIFF_S   value13;
  BSTRING       value14;
  TIME_VALUE_S  value15;
}
  PACKED TEST;                /* DS_85 */

typedef  struct
{
   USIGN8       action;
   USIGN32      function;
   USIGN16      occurence;
}
   PACKED ACTION;             /* DS_86 */

typedef  struct
{
  USIGN8       unack;
  USIGN8       state;
  TIME_VALUE_S time_stamp;
  USIGN32      subcode;
  USIGN8       value;
  USIGN8       current;           /* hidden attribute - current state */
  USIGN8       standard_type;     /* hidden attribute */
  USIGN8       mfgr_type;         /* hidden attribute */
  USIGN16      unit_index;        /* hidden attribute */
}
  PACKED ALARM_FD_DIAG; /* DS_87 */

typedef  struct
{
  USIGN16      block_index;
  USIGN8       alert_key;
  USIGN8       standard_type;
  USIGN8       mfr_type;
  USIGN8       message_type;
  USIGN8       priority;
  TIME_VALUE_S time_stamp;
  USIGN32      sub_code;
  USIGN8       value;
  USIGN16      relative_index;
  USIGN16      source_block_index;
}
   PACKED  ALERT_FD_DIAG;  /* DS_88 */

typedef struct
{
   USIGN32 diagnostic_simulate_value;
   USIGN32 diagnostic_value;
   USIGN8  enable;
}
   PACKED SIMULATE_FD; /* DS_89 */


/* ----------------------------------------------------- */
/* OD indexes of the FF Standard Data Structures         */
/* ----------------------------------------------------- */
#define DS_BLOCK                  64
#define DS_FLOAT_S                65
#define DS_DISC_S                 66
#define DS_BS_S                   67
#define DS_SCALE                  68
#define DS_MODE                   69
#define DS_ACCESS_PERM            70
#define DS_ALARM_FLOAT            71
#define DS_ALARM_DISC             72
#define DS_ALARM_EVENT            73
#define DS_ALARM_SUMMARY          74
#define DS_ALERT_FLOAT            75
#define DS_ALERT_DISC             76
#define DS_ALERT_EVENT            77
#define DS_TREND_FLOAT            78
#define DS_TREND_DISC             79
#define DS_TREND_BS               80
#define DS_FB_LINK                81
#define DS_SIM_FLOAT              82
#define DS_SIM_DISC               83
#define DS_SIM_BS                 84
#define DS_TEST                   85
#define DS_ACTION                 86

#define DS_ALARM_FD_DIAG          87
#define DS_ALERT_FD_DIAG          88
#define DS_SIMULATE_FD            89


/* ----------------------------------------------------- */
/* AP Directory (OD Dirctory Object)                     */
/* ----------------------------------------------------- */
#define MAX_NO_OF_TRANS_BLOCK    32  /* Maximum number of transducer blocks */
#define MAX_NO_OF_FUNCT_BLOCK    32  /* Maximum number of function blocks   */
#define MAX_NO_OF_BLOCKS         32  /* Maximum number of blocks            */
#define MAX_NO_OF_MVCS           32  /* Maximum number of MVCs              */

typedef  struct _BLK_DIR
{
   USIGN16     blk_od_index;     /* block starting OD index          */
   USIGN16     blk_od_count;     /* number of objects in the block   */
}  BLK_DIR;

typedef  struct _FB_AP_DIR
{
   USIGN16     reserved;         /*  1) reserved by the FB Specifications                */
   USIGN16     rev_no;           /*  2) directory revision number                        */
   USIGN16     no_of_dir_obj;    /*  3) total number of directory objects                */
   USIGN16     no_of_dir_entry;  /*  4) total number of directory entries                */
   USIGN16     first_comp_entry; /*  5) index of first composite = 17                    */
   USIGN16     no_of_comp_entry; /*  6) number of composite entries = 3                  */
   USIGN16     action_start;     /*  7) starting index of Action objects                 */
   USIGN16     action_count;     /*  8) number of Action objects                         */
   USIGN16     link_start;       /*  9) starting index of Link objects                   */
   USIGN16     link_count;       /* 10) number of Link objects                           */
   USIGN16     alert_start;      /* 11) starting index of Alert objects                  */
   USIGN16     alert_count;      /* 12) number of Alert objects                          */
   USIGN16     trend_start;      /* 13) starting index of Trend objects                  */
   USIGN16     trend_count;      /* 14) number of Trend objects                          */
   USIGN16     domain_start;     /* 15) starting index of Domain objects                 */
   USIGN16     domain_count;     /* 16) number of Domain objects                         */
   USIGN16     mvc_start;        /* 17) starting index of MVC objects                    */
   USIGN16     mvc_count;        /* 18) number of MVC objects                            */
   USIGN16     mvc_view_start;   /* 19) starting index of MVC view objects               */
   USIGN16     mvc_view_count;   /* 20) number of MVC view objects                       */
   USIGN16     rsc_ap_index;     /* 21) resource block AP directory index = 27           */
   USIGN16     rsc_blk_count;    /* 22) number of resource blocks = 1                    */
   USIGN16     tran_ap_index;    /* 23) transducer block AP directory index = 29         */
   USIGN16     tran_blk_count;   /* 24) number of transducer blocks                      */
   USIGN16     func_ap_index;    /* 25) function block AP directory index                */
   USIGN16     func_blk_count;   /* 26) number of function blocks                        */
   USIGN16     rsc_od_index;     /* 27) resource block starting OD index                 */
   USIGN16     rsc_od_count;     /* 28) number of objects in resource block              */
   BLK_DIR     tblk_dir[MAX_NO_OF_TRANS_BLOCK];
   BLK_DIR     fblk_dir[MAX_NO_OF_FUNCT_BLOCK];
}
   FB_AP_DIR;


/* ----------------------------------------------------- */
/* Error Codes for Read and Write Handler */
/* ----------------------------------------------------- */
#define  E_FB_PARA_CHECK           1
#define  E_FB_PARA_LIMIT           2
#define  E_FB_WRONG_MODE           3
#define  E_FB_WRITE_LOCK           4
#define  E_FB_DATA_NO_WRITE        5
#define  E_FB_BLK_CREATE_FAIL      6
#define  E_FB_BLK_DELETE_FAIL      7
#define  E_FB_ALM_ACK              8
#define  E_FB_EEP_NO_WRITE       128
#define  E_FB_TEMP_ERROR         129
#define  E_FB_DATA_NO_READ       130
#define  E_FB_NO_SUBIDX_ACCESS   131

#define E_FB_EEP_WRITE_ERR         E_FB_EEP_NO_WRITE

#endif /* __FBAP_H__ */

#define TS_TB_DD "Time-stamp: <2013-11-27 12:58:42 GE-300010777>"
/*
#+LINK: SVIFFDocs file:c:/Users/300010777/Desktop/FF Design Documents/
  Web Document:
  http://libraries.ge.com/download?fileid=439958398101&entity_id=24543915101&sid=101 
  Loc. Document: [[SVIFFDocs:FF_Parameter_Mapping_TB_131031.xlsx][FF_Parameter_Mapping_TB.xlsx]]
*/

/*
 * Variable & Record Definitios for transducer block
 */
#include "builtins.h"
#include "svi_tb.h"
#include "svi2_menus.h"
#include "svi_faults.h"
/*-----------------------------------/
** Defines all menus for TB_MENU_LIST
/-----------------------------------*/


/*-----------------------------/
** define the various relations
/-----------------------------*/

/* UNIT pv_unit_relation */
/* { */
/*     PARAM.FINAL_VALUE_RANGE.UNITS_INDEX: */
/*     PARAM.FINAL_VALUE.VALUE, */
/*     PARAM.FINAL_POSITION_VALUE.VALUE */
/* } */

UNIT pressure_unit_relation
{
  PARAM.PRESSURE_RANGE.UNITS_INDEX:
    PARAM.PRESSURE_RANGE.EU_100,
    PARAM.PRESSURE_RANGE.EU_0,
    PARAM.SUPPLY_PRESSURE.VALUE,
    PARAM.ACTUATOR_A_PRESSURE.VALUE,
    PARAM.ACTUATOR_B_PRESSURE.VALUE,
    PARAM.PILOT_PRESSURE.VALUE,
    PARAM.ATMOSPHERIC_PRESSURE.VALUE,
    PARAM.SUPPLY_PRESSURE_HI_ALERT.ALERT_POINT,
    PARAM.SUPPLY_PRESSURE_HI_ALERT.DEADBAND,
    PARAM.SUPPLY_PRESSURE_LOLO_ALERT.ALERT_POINT,
    PARAM.SUPPLY_PRESSURE_LOLO_ALERT.DEADBAND,
    PARAM.SUPPLY_PRESSURE_LO_ALERT.ALERT_POINT,
    PARAM.SUPPLY_PRESSURE_LO_ALERT.DEADBAND,
    PARAM.PRESSURE_EXTREMES.SUPPLY_PRESSURE_MAX,
    PARAM.PRESSURE_EXTREMES.SUPPLY_PRESSURE_MIN,
    PARAM.PRESSURE_EXTREMES.ACTUATOR_A_MAX,
    PARAM.PRESSURE_EXTREMES.ACTUATOR_A_MIN,
    PARAM.PRESSURE_EXTREMES.ACTUATOR_B_MAX,
    PARAM.PRESSURE_EXTREMES.ACTUATOR_B_MIN,
    PARAM.PRESSURE_EXTREMES.PILOT_MAX,
    PARAM.PRESSURE_EXTREMES.PILOT_MIN
}

UNIT working_sp_unit_relation
{
    PARAM.TRAVEL.UNITS_INDEX:
      PARAM.WORKING_SP.VALUE,
      PARAM.WORKING_POS.VALUE
}

UNIT travel_unit_relation
{
    PARAM.TRAVEL.UNITS_INDEX:
        PARAM.TRAVEL_ACCUMULATION_A_ALERT.TRAVEL_ACCUMULATION,
        PARAM.TRAVEL_ACCUMULATION_A_ALERT.ALERT_POINT,
        PARAM.TRAVEL_ACCUMULATION_A_ALERT.DEADBAND_AB,
        PARAM.TRAVEL_ACCUMULATION_B_ALERT.TRAVEL_ACCUMULATION,
        PARAM.TRAVEL_ACCUMULATION_B_ALERT.ALERT_POINT,
        PARAM.TRAVEL_ACCUMULATION_B_ALERT.DEADBAND_AB,
        PARAM.CYCLE_COUNTER_A_ALERT.CYCLE_COUNTER,
        PARAM.CYCLE_COUNTER_A_ALERT.ALERT_POINT,
        PARAM.CYCLE_COUNTER_A_ALERT.DEADBAND_AB,
        PARAM.CYCLE_COUNTER_B_ALERT.CYCLE_COUNTER,
        PARAM.CYCLE_COUNTER_B_ALERT.ALERT_POINT,
        PARAM.CYCLE_COUNTER_B_ALERT.DEADBAND_AB
}

/*---------------------------/
** Variables for common usage
/---------------------------*/


VARIABLE tb_block_err_desc_1
{
    LABEL           LBL_BLOCK_ERR_DESC_1 ;
    HELP            HLP5(HLP_TB_BLOCK_ERR_DESC_1);
    CLASS           CONTAINED & DYNAMIC ;
    TYPE            BIT_ENUMERATED (4)
    {
        TB_BLOCK_ERR_DESC_1_ENUM
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ ;
}

VARIABLE svi2_tb_target_mode
{
    LABEL           [target_mode] ;
    HELP            [target_mode_help] ;
    CLASS           CONTAINED & OPERATE;
    TYPE            BIT_ENUMERATED (1)
    {
        _MODE_AUTOMATIC,
        _MODE_MANUAL,
        _MODE_OUT_OF_SERVICE
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
/*  RESPONSE_CODES  xxx ; */
}

VARIABLE svi2_tb_permitted_mode
{
    LABEL           [permitted_mode] ;
    HELP            [permitted_mode_help] ;
    CLASS           CONTAINED & OPERATE ;
    TYPE            BIT_ENUMERATED (1)
    {
        _PERMITTED_AUTOMATIC,
        _PERMITTED_MANUAL,
        _PERMITTED_OUT_OF_SERVICE
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
/*  RESPONSE_CODES  xxx ; */
}

VARIABLE svi2_tb_normal_mode
{
    LABEL           [normal_mode] ;
    HELP            [normal_mode_help] ;
    CLASS           CONTAINED & OPERATE ;
    TYPE            BIT_ENUMERATED (1)
    {
        _MODE_AUTOMATIC,
        _MODE_MANUAL,
        _MODE_OUT_OF_SERVICE
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
/*  RESPONSE_CODES  xxx ; */
}

_float_contained_r_nd LIKE VARIABLE __float_contained_nd
{
    REDEFINE  HANDLING READ;
}



VARIABLE int_2_element
{
    LABEL           "|en|integer(2) element" ;
    CLASS           CONTAINED;
    TYPE            INTEGER (2) ;
}

VARIABLE int_element
{
    LABEL "|en|int element";
    CLASS CONTAINED;
    TYPE INTEGER;
}

int_2_element_r LIKE VARIABLE int_2_element
{
    REDEFINE  HANDLING READ;
}

METHOD scale_to_percent
{
    LABEL "scaled to percent";
    CLASS INPUT;
    DEFINITION
    {
        long percent;
        get_signed(&percent);
        percent = percent * 100 / 16384;
        put_signed(percent);
    }
}

METHOD scale_to_counts
{
    LABEL "scaled to counts";
    CLASS OUTPUT;
    DEFINITION
    {
        long counts;
        get_signed(&counts);
        counts = counts * 16384 / 100;
        put_signed(counts);
    }
}

int_2_element_scaled LIKE  VARIABLE int_2_element
{

    REDEFINE PRE_WRITE_ACTIONS
    {
        scale_to_counts
    }
    REDEFINE POST_READ_ACTIONS
    {
        scale_to_percent
    }
}

int_2_element_scaled_r LIKE  VARIABLE int_2_element
{
    REDEFINE POST_READ_ACTIONS
    {
        scale_to_percent
    }
    REDEFINE HANDLING READ;
}

VARIABLE int_2_element_d
{
    LABEL           "|en|integer(2) with dynamic" ;
    CLASS           CONTAINED & DYNAMIC;
    TYPE            INTEGER (2) ;
}

VARIABLE uint_element
{
    LABEL           "|en|unsigned integer element" ;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (1) ;
}

uint_element_r LIKE VARIABLE uint_element
{
    REDEFINE  HANDLING READ;
}

VARIABLE uint_element_d
{
    LABEL           "|en|unsigned integer element" ;
    CLASS           CONTAINED & DYNAMIC;
    TYPE            UNSIGNED_INTEGER (1) ;
}

VARIABLE uint_2_element
{
    LABEL           "|en|unsigned integer element" ;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (2) ;
}

uint_2_element_r LIKE VARIABLE uint_2_element
{
    REDEFINE  HANDLING READ;
}

uint_2_element_rd LIKE VARIABLE uint_2_element_d
{
    REDEFINE  HANDLING READ;
}

VARIABLE uint_2_element_d
{
    LABEL           "|en|unsigned integer element" ;
    CLASS           CONTAINED & DYNAMIC;
    TYPE            UNSIGNED_INTEGER (2) ;
}

VARIABLE uint_4_element_r
{
    LABEL           "|en|unsigned integer(4) element" ;
    CLASS           CONTAINED & DYNAMIC ;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE uint_4_element_nd
{
    LABEL           "|en|unsigned integer(4) element" ;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
}

VARIABLE uint_4_element_r_nd
{
    LABEL           "|en|unsigned integer(4) element" ;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

/* RECORD _position_alert */
/* { */
/*     LABEL LBL_POSITION_ALERT; */
/*     HELP  HLP5(HLP_POSITION_ALERT); */
/*     MEMBERS */
/*     { */
/*         POSITION,                           _position; */
/*         ALERT_POINT,                        _alert_point; */
/*         DEADBAND,                           _deadband; */
/*         ALERT,                              _alert; */
/*         HISTORIC_ALERT,                     _historic_alert; */
/*         ENABLE,                             _enable; */
/*     } */
/* } */

VARIABLE _position
{
    LABEL LBL_POSITION;
    HELP  HLP5(HLP_POSITION);
    CLASS CONTAINED & DYNAMIC;
    TYPE FLOAT ;
    CONSTANT_UNIT [unit_code_1342];
    HANDLING READ;
}
VARIABLE _alert_point
{
    LABEL LBL_ALERT_POINT;
    HELP  HLP5(HLP_ALERT_POINT);
    CLASS CONTAINED;
    TYPE FLOAT ;
    CONSTANT_UNIT [unit_code_1342];

}
VARIABLE _deadband
{
    LABEL LBL_DEADBAND;
    HELP  HLP5(HLP_DEADBAND);
    CLASS CONTAINED;
    TYPE FLOAT ;
    CONSTANT_UNIT [unit_code_1342];

}
VARIABLE _alert
{
    LABEL LBL_ALERT;
    HELP  HLP5(HLP_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE  ENUMERATED (1)
    {
        ENUM_ALERT_STATE
    }
    HANDLING READ;
}

VARIABLE _historic_alert_for_near_closed_alert
{
    LABEL LBL_HISTORIC_ALERT;
    HELP  HLP5(HLP_HISTORIC_ALERT_FOR_NEAR_CLOSED_ALERT);
    CLASS CONTAINED;
    TYPE  UNSIGNED_INTEGER (4);
}

VARIABLE _historic_alert
{
    LABEL LBL_HISTORIC_ALERT;
    HELP  HLP5(HLP_HISTORIC_ALERT);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ALERT_STATE
    }
    HANDLING READ;
}
VARIABLE _enable
{
    LABEL LBL_ENABLE;
    HELP  HLP5(HLP_ENABLE);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ENABLE_DISABLE
    }
}

VARIABLE _direction
{
    LABEL "|en|Direction";
    HELP  HLP5(HLP_DIRECTION);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_DIRECTION
    }
}

VARIABLE _function
{
    LABEL "|en|Function";
    HELP  HLP5(HLP_FUNCTION);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_FUNCTION
    }
}

VARIABLE pos_percent
{
    LABEL LBL_POS_5_PERCENT_CLOSED;
    HELP  HLP5(HLP_POS_5_PERCENT_CLOSED);
    CLASS CONTAINED;
    TYPE  FLOAT
    {
        DISPLAY_FORMAT "6.3f";
    }
}

VARIABLE pos_percent_r
{
    LABEL LBL_POS_5_PERCENT_CLOSED;
    HELP  HLP5(HLP_POS_5_PERCENT_CLOSED);
    CLASS CONTAINED;
    TYPE  FLOAT
    {
        DISPLAY_FORMAT "6.3f";
    }
    HANDLING READ;
}

pos_5_percent_closed_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_5_PERCENT_CLOSED;
    REDEFINE HELP  HLP5(HLP_POS_5_PERCENT_CLOSED);
}

pos_10_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_10_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_10_PERCENT);
}
pos_20_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_20_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_20_PERCENT);
}
pos_30_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_30_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_30_PERCENT);
}
pos_40_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_40_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_40_PERCENT);
}
pos_50_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_50_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_50_PERCENT);
}
pos_60_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_60_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_60_PERCENT);
}
pos_70_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_70_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_70_PERCENT);
}
pos_80_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_80_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_80_PERCENT);
}
pos_90_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_90_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_90_PERCENT);
}
pos_95_percent_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_95_PERCENT;
    REDEFINE HELP  HLP5(HLP_POS_95_PERCENT);
}
pos_95_percent_open_r LIKE VARIABLE pos_percent_r
{
    REDEFINE LABEL LBL_POS_95_PERCENT_OPEN;
    REDEFINE HELP  HLP5(HLP_POS_95_PERCENT_OPEN);
}

VARIABLE p
{
    LABEL LBL_P;
    HELP  HLP5(HLP_P);
    CLASS CONTAINED;
    TYPE INTEGER (2)
    {
        MAX_VALUE 5000;
    }
    /*VALIDITY  NA  ;*/
}
VARIABLE i
{
    LABEL LBL_I;
    HELP  HLP5(HLP_I);
    CLASS CONTAINED;
    TYPE INTEGER (2)
    {
        MAX_VALUE 1000;
    }
    /*VALIDITY  NA  ;*/
}
VARIABLE d
{
    LABEL LBL_D;
    HELP  HLP5(HLP_D);
    CLASS CONTAINED;
    TYPE INTEGER (2)
    {
        MAX_VALUE 200;
    }
    /*VALIDITY  NA  ;*/
}
VARIABLE padj
{
    LABEL LBL_PADJ;
    HELP  HLP5(HLP_PADJ);
    CLASS CONTAINED;
    TYPE INTEGER (2)
    {
        MIN_VALUE -3000;
        MAX_VALUE 3000;
    }

    /*VALIDITY  NA  ;*/
}
VARIABLE beta
{
    LABEL LBL_BETA;
    HELP  HLP5(HLP_BETA);
    CLASS CONTAINED;
    TYPE INTEGER (2)
    {
        MIN_VALUE -9;
        MAX_VALUE 9;
    }

    /*VALIDITY  NA  ;*/
}
VARIABLE poscomp
{
    LABEL LBL_POSCOMP;
    HELP  HLP5(HLP_POSCOMP);
    CLASS CONTAINED;
    TYPE INTEGER (2)
    {
        MIN_VALUE 2;
        MAX_VALUE 20;
    }

    /*VALIDITY  NA  ;*/
}
VARIABLE deadzone
{
    LABEL LBL_DEADZONE;
    HELP  HLP5(HLP_DEADZONE);
    CLASS CONTAINED;
    TYPE INTEGER (2)
    {
        MAX_VALUE 821;
    }

    /*VALIDITY  NA  ;*/
}
VARIABLE nonlin
{
    LABEL LBL_NONLIN;
    HELP  HLP5(HLP_NONLIN);
    CLASS CONTAINED;
    TYPE INTEGER (2)
	DATA_LIMITS(0, 20)
    /*VALIDITY  NA  ;*/
}

p_r LIKE VARIABLE p
{
    REDEFINE HANDLING READ;
}
i_r LIKE VARIABLE i
{
    REDEFINE HANDLING READ;
}
d_r LIKE VARIABLE d
{
    REDEFINE HANDLING READ;
}
padj_r LIKE VARIABLE padj
{
    REDEFINE HANDLING READ;
}
beta_r LIKE VARIABLE beta
{
    REDEFINE HANDLING READ;
}
poscomp_r LIKE VARIABLE poscomp
{
    REDEFINE HANDLING READ;
}
deadzone_r LIKE VARIABLE deadzone
{
    REDEFINE HANDLING READ;
}
nonlin_r LIKE VARIABLE nonlin
{
    REDEFINE HANDLING READ;
}

VARIABLE mapped_to_rb
{
    LABEL LBL_MAPPED_TO_RB;
    HELP  HLP5(HLP_MAPPED_TO_RB);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_MAPPED_TO_RB
    }
}

VARIABLE propagate_mode
{
    LABEL LBL_PROPAGATE_MODE;
    HELP  HLP5(HLP_PROPAGATE_MODE);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_PROPAGATE_MODE
    }
}

VARIABLE final_value_f
{
    LABEL           [value_label] ;
    HELP            HLP5(HLP_FINAL_VALUE);
    CLASS           CONTAINED & SERVICE ;
    TYPE            FLOAT;
    HANDLING        READ & WRITE ;
    CONSTANT_UNIT [unit_code_1342];
    /* DATA_LIMITS(-25,125) */
}
float_position_value_r LIKE VARIABLE __float_contained_r
{
    REDEFINE CONSTANT_UNIT [unit_code_1342];
}

VARIABLE _ip_drive_current_deadband
{
    LABEL LBL_DEADBAND;
    HELP  HLP5(HLP_DEADBAND);
    CLASS CONTAINED;
    TYPE  FLOAT
	DATA_LIMITS(0, 20)
    CONSTANT_UNIT [unit_code_1342];

}

VARIABLE _alert_command
{
    LABEL LBL_ALERT_COMMAND;
    HELP  HLP5(HLP_ALERT_COMMAND);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ALERT_LOG_COMMAND
    }
}

VARIABLE _alert_data_r
{
    LABEL LBL_ALERT_DATA;
    HELP  HLP5(HLP_ALERT_DATA);
    CLASS CONTAINED;
    TYPE  UNSIGNED_INTEGER (4) ;
    HANDLING READ;
}

temp_value_with_unit LIKE VARIABLE __float_contained_r
{
    REDEFINE CONSTANT_UNIT [unit_code_1001];

}

VARIABLE discrete_input_value
{
    LABEL LBL_DISCRETE_INPUT;
    HELP  HLP5(HLP_DISCRETE_INPUT);
    CLASS CONTAINED & DYNAMIC;
    TYPE  ENUMERATED (1)
    {
        ENUM_DISCRETE_STATE
    }
    HANDLING READ;
}

VARIABLE aggressiveness
{
    LABEL LBL_AGGRESSIVENESS;
    HELP  HLP5(HLP_AGGRESSIVENESS);
    CLASS CONTAINED;
    TYPE  INTEGER
    {
        MIN_VALUE -9;
        MAX_VALUE 9;
    }
}

tune_flags LIKE VARIABLE uint_element
{
    REDEFINE LABEL LBL_TUNE_FLAGS;
    REDEFINE HELP  HLP5(HLP_TUNE_FLAGS);
}

VARIABLE completion
{
    LABEL LBL_COMPLETION;
    HELP  HLP5(HLP_COMPLETION);
    CLASS CONTAINED & DYNAMIC;
    TYPE ENUMERATED (2)
    {
        ENUM_COMPLETION
    }
}

VARIABLE character_type
{
    LABEL LBL_CHARACTER_TYPE;
    HELP  HLP5(HLP_CHARACTER_TYPE);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_TYPE
    }
    /*VALIDITY  "Man/OOS"    ;*/
}

VARIABLE char_sel_number_of_points_r
{
    LABEL LBL_CHAR_SEL_NUMBER_OF_POINTS;
    HELP  HLP5(HLP_CHAR_SEL_NUMBER_OF_POINTS);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
    /*VALIDITY  "Man/OOS"    ;*/
    HANDLING READ;
}

VARIABLE act_style
{
    LABEL LBL_ACT_STYLE;
    HELP  HLP5(HLP_ACT_STYLE);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ACTUATOR_TYPE
    }
    HANDLING READ;
}

VARIABLE point_closed
{
    LABEL LBL_POINT_CLOSED;
    HELP  HLP5(HLP_POINT_CLOSED);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-49,100)
}

VARIABLE float_nd_with_max_min
{
    LABEL "|en|float";
    HELP  "|en|float value";
    CLASS CONTAINED;
    TYPE  FLOAT
    DATA_LIMITS(0.1, -200)
}


position_hihi_point LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_POSITION_HIHI_POINT;
    REDEFINE HELP  HLP5(HLP_POSITION_HIHI_POINT);
    REDEFINE_DATA_LIMITS(FLOAT, 0.1, 200)
}
position_hihi_deadband LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_POSITION_HIHI_DEADBAND;
    REDEFINE HELP  HLP5(HLP_POSITION_HIHI_DEADBAND);
    REDEFINE_DATA_LIMITS(FLOAT, 0.1, 10)
}

position_hi_point LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_POSITION_HI_POINT;
    REDEFINE HELP  HLP5(HLP_POSITION_HI_POINT);
    REDEFINE_DATA_LIMITS(FLOAT, 0.1, 200)
}
position_hi_deadband LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_POSITION_HI_DEADBAND;
    REDEFINE HELP  HLP5(HLP_POSITION_HI_DEADBAND);
    REDEFINE_DATA_LIMITS(FLOAT, 0.1, 10)
}
position_lo_point LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_POSITION_LO_POINT;
    REDEFINE HELP  HLP5(HLP_POSITION_LO_POINT);
    REDEFINE_DATA_LIMITS(FLOAT, -51, 199)
}
position_lo_deadband LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_POSITION_LO_DEADBAND;
    REDEFINE HELP  HLP5(HLP_POSITION_LO_DEADBAND);
    REDEFINE_DATA_LIMITS(FLOAT, 0.1, 10)
}

position_lolo_point LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_POSITION_LOLO_POINT;
    REDEFINE HELP  HLP5(HLP_POSITION_LOLO_POINT);
    REDEFINE_DATA_LIMITS(FLOAT, -51, 199)
}
position_lolo_deadband LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_POSITION_LOLO_DEADBAND;
    REDEFINE HELP  HLP5(HLP_POSITION_LOLO_DEADBAND);
    REDEFINE_DATA_LIMITS(FLOAT, 0.1, 10)
}

VARIABLE xd_fs_configuration
{
    LABEL LBL_XD_FS_CONFIGURATION;
    HELP  HLP5(HLP_XD_FS_CONFIGURATION);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_XD_FS_CONFIGURATION
    }
}

_eu_100_r LIKE VARIABLE __eu_100
{
    REDEFINE  HANDLING READ;
}
_eu_0_r LIKE VARIABLE __eu_0
{
    REDEFINE  HANDLING READ;
}

VARIABLE _enable_disable
{
    LABEL LBL_ENABLE;
    HELP  HLP5(HLP_ENABLE);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ENABLE_DISABLE
    }
    /* HANDLING READ; */
}

_enable_disable_r LIKE VARIABLE _enable_disable
{
    REDEFINE HANDLING READ;
}

VARIABLE _discrete_value_nd
{
    LABEL LBL_VALUE_D;
    HELP  HLP5(HLP_VALUE_D);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1)
    DATA_LIMITS(0,1)
}

VARIABLE _discrete_state_r
{
    LABEL LBL_VALUE_D;
    HELP HLP5(HLP_VALUE_D);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_DISCRETE_STATE
    }
	HANDLING READ;
}

VARIABLE _discrete_value
{
    LABEL LBL_VALUE_D;
    HELP  HLP5(HLP_VALUE_D);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (1)
    DATA_LIMITS(0,1)
}

_discrete_value_int_nd LIKE VARIABLE _discrete_value_nd
{
    REDEFINE HELP  HLP5(HLP_VALUE_DINT);
    REDEFINE_DATA_LIMITS(UNSIGNED_INTEGER, 0, 100)
}

_discrete_value_int_nd_r LIKE VARIABLE _discrete_value_nd
{
    REDEFINE HELP  HLP5(HLP_VALUE_DINT);
    REDEFINE_DATA_LIMITS(UNSIGNED_INTEGER, 0, 100)
    REDEFINE HANDLING READ;
    
}

/* xd_fstate_opt LIKE VARIABLE __xd_fstate_opt /\* bug 15845 *\/ */
/* { */
/*     REDEFINE HANDLING READ & WRITE; */
/* } */

VARIABLE xd_fstate_opt
{
    LABEL [xd_fstate_opt_label] ;
    HELP [xd_fstate_opt_help] ;
    CLASS CONTAINED & SERVICE ;
    TYPE ENUMERATED (1) 
    {
        { 0, [hold_last_value_label],[hold_last_value_help] },
        { 1, [fail_closed_label], [fail_closed_help] },
        { 2, [fail_open_label], [fail_open_help] },
        { 3, [xd_fstate_val_label], [xd_fstate_val_help] }
    }
    HANDLING READ & WRITE;
}

_float_limited LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS(FLOAT, 0.1, 10000)
}

_deadband_ab_float_nd LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS(FLOAT, 0.1, 10)
}

_deadband_ab_for_cycle_count LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS(FLOAT, 0.05, 10)
}

_alert_point_float_nd LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS(FLOAT, 0.1, 250)
}
_dead_band_float_nd LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS(FLOAT, 0.01, 10)
}

time_since_alert LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS_ONLY_MIN(FLOAT, 0)
}
setpoint_alert_point LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS_ONLY_MIN(FLOAT, 0.5)
}

setpoint_max_time LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS_ONLY_MIN(FLOAT, 0)
}

VARIABLE trvl_accmltn_alert_point
{
    LABEL           "|en|Alert Point" ;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4)
	DATA_LIMITS(0, 0xFFFFFFFF)
}

/*~~~~~~~~~~~~~~~~~~~~~~~~/
*** local var definitions
/~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
*** Device status definitions for current
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

VARIABLE device_status_0_c
{
    LABEL "|en|device status 0(c)";
    HELP "|en|Device status 0";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(0)
    }
}
VARIABLE device_status_1_c
{
    LABEL "|en|device status 1(c)";
    HELP "|en|Device status 1";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(1)
    }
}
VARIABLE device_status_2_c
{
    LABEL "|en|device status 2(c)";
    HELP "|en|Device status 2";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(2)
    }
}
VARIABLE device_status_3_c
{
    LABEL "|en|device status 3(c)";
    HELP "|en|Device status 3";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(3)
    }
}
VARIABLE device_status_4_c
{
    LABEL "|en|device status 4(c)";
    HELP "|en|Device status 4";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(4)
    }
}
VARIABLE device_status_5_c
{
    LABEL "|en|device status 5(c)";
    HELP "|en|Device status 5";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(5)
    }
}
VARIABLE device_status_6_c
{
    LABEL "|en|device status 6(c)";
    HELP "|en|Device status 6";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(6)
    }
}
VARIABLE device_status_7_c
{
    LABEL "|en|device status 7(c)";
    HELP "|en|Device status 7";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(7)
    }
}
VARIABLE device_status_8_c
{
    LABEL "|en|device status 8(c)";
    HELP "|en|Device status 8";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(8)
    }
}

VARIABLE device_status_9_c
{
    LABEL "|en|DEVICE_STATUS_9(C)" ;
    HELP                        "|en|TBD";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS_TBD
    }
}
VARIABLE device_status_10_c
{
    LABEL "|en|DEVICE_STATUS_10(C)" ;
    HELP                        "|en|TBD";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS_TBD
    }
}
VARIABLE device_status_11_c
{
    LABEL "|en|DEVICE_STATUS_11(C)" ;
    HELP                        "|en|TBD";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS_TBD
    }
}

/* Add more variable for travel trend and cycle counter alert*/
VARIABLE _currently_collected
{
    LABEL           "|en|Currently Collected";
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _today
{
    LABEL           LBL_168_TODAY;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _last_day
{
    LABEL           LBL_169_LAST_DAY;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _previous_day
{
    LABEL           LBL_170_PREVIOUS_DAY;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _three_days_ago
{
    LABEL           LBL_171_THREE_DAYS_AGO;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _current_week
{
    LABEL           LBL_172_CURRENT_WEEK;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _last_week
{
    LABEL           LBL_173_LAST_WEEK;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _previous_week
{
    LABEL           LBL_174_PREVIOUS_WEEK;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _three_weeks_ago
{
    LABEL           LBL_175_THREE_WEEKS_AGO;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _current_month
{
    LABEL           LBL_176_CURRENT_MONTH;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _last_month
{
    LABEL           LBL_177_LAST_MONTH;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _previous_month
{
    LABEL           LBL_178_PREVIOUS_MONTH;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _three_months_ago
{
    LABEL           LBL_179_THREE_MONTHS_AGO;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _current_12_months
{
    LABEL           LBL_180_CURRENT_12_MONTHS;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _last_12_months
{
    LABEL           LBL_181_LAST_12_MONTHS;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _previous_12_months
{
    LABEL           LBL_182_PREVIOUS_12_MONTHS;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE _three_years_ago
{
    LABEL           LBL_183_THREE_YEARS_AGO;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
    HANDLING        READ ;
}

VARIABLE    _float_currently_collected
{
    LABEL           "|en|Currently Collected";
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_today
{
    LABEL           LBL_168_TODAY;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_last_day
{
    LABEL           LBL_169_LAST_DAY;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_previous_day
{
    LABEL           LBL_170_PREVIOUS_DAY;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_three_days_ago
{
    LABEL           LBL_171_THREE_DAYS_AGO;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_current_week
{
    LABEL           LBL_172_CURRENT_WEEK;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_last_week
{
    LABEL           LBL_173_LAST_WEEK;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_previous_week
{
    LABEL           LBL_174_PREVIOUS_WEEK;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_three_weeks_ago
{
    LABEL           LBL_175_THREE_WEEKS_AGO;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_current_month
{
    LABEL           LBL_176_CURRENT_MONTH;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_last_month
{
    LABEL           LBL_177_LAST_MONTH;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_previous_month
{
    LABEL           LBL_178_PREVIOUS_MONTH;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_three_months_ago
{
    LABEL           LBL_179_THREE_MONTHS_AGO;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_current_12_months
{
    LABEL           LBL_180_CURRENT_12_MONTHS;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_last_12_months
{
    LABEL           LBL_181_LAST_12_MONTHS;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_previous_12_months
{
    LABEL           LBL_182_PREVIOUS_12_MONTHS;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _float_three_years_ago
{
    LABEL           LBL_183_THREE_YEARS_AGO;
    CLASS           CONTAINED & OPERATE ;
    TYPE            FLOAT ;
    HANDLING        READ ;
}

VARIABLE _travel_accumulation
{
    LABEL           "|en|Travel Accumulation" ;
    CLASS           CONTAINED;
    TYPE            UNSIGNED_INTEGER (4) ;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
*** Device status definitions for history
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

device_status_0_h LIKE VARIABLE device_status_0_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_0(H)";
}
device_status_1_h LIKE VARIABLE device_status_1_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_1(H)";
}
device_status_2_h LIKE VARIABLE device_status_2_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_2(H)";
}
device_status_3_h LIKE VARIABLE device_status_3_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_3(H)";
}
device_status_4_h LIKE VARIABLE device_status_4_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_4(H)";
}
device_status_5_h LIKE VARIABLE device_status_5_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_5(H)";
}
device_status_6_h LIKE VARIABLE device_status_6_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_6(H)";
}
device_status_7_h LIKE VARIABLE device_status_7_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_7(H)";
}
device_status_8_h LIKE VARIABLE device_status_8_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_8(H)";
}
device_status_9_h LIKE VARIABLE device_status_9_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_9(H)";
}
device_status_10_h LIKE VARIABLE device_status_10_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_10(H)";
}
device_status_11_h LIKE VARIABLE device_status_11_c
{
    REDEFINE LABEL "|en|DEVICE_STATUS_11(H)";
}

/*
VARIABLE device_status_0_h
{
    LABEL "|en|device status 0(H)";
    HELP "|en|Device status 0";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(0)
    }
}

VARIABLE device_status_1_h
{
    LABEL "|en|device status 1(H)";
    HELP "|en|Device status 1";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(1)
    }
}

VARIABLE device_status_2_h
{
    LABEL "|en|device status 2(H)";
    HELP "|en|Device status 2";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(2)
    }
}

VARIABLE device_status_3_h
{
    LABEL "|en|device status 3(H)";
    HELP "|en|Device status 3";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(3)
    }
}

VARIABLE device_status_4_h
{
    LABEL "|en|device status 4(H)";
    HELP "|en|Device status 4";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(4)
    }
}

VARIABLE device_status_5_h
{
    LABEL "|en|device status 5(H)";
    HELP "|en|Device status 5";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(5)
    }
}

VARIABLE device_status_6_h
{
    LABEL "|en|device status 6(H)";
    HELP "|en|Device status 6";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(6)
    }
}

VARIABLE device_status_7_h
{
    LABEL "|en|device status 7(H)";
    HELP "|en|Device status 7";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        { FBIT4, "|en|UI_OFF" }
        { FBIT7, "|en|SETPOINT_TIMEOUT" }
    }
}

VARIABLE device_status_8_h
{
    LABEL "|en|device status 8(H)";
    HELP "|en|Device status 8";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS(8)
    }
}

VARIABLE device_status_9_h
{
    LABEL "|en|DEVICE_STATUS_9(H)" ;
    HELP  "|en|TBD";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS_TBD
    }
}
VARIABLE device_status_10_h
{
    LABEL "|en|DEVICE_STATUS_10(H)" ;
    HELP  "|en|TBD";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS_TBD
    }
}
VARIABLE device_status_11_h
{
    LABEL "|en|DEVICE_STATUS_11(H)" ;
    HELP  "|en|TBD";
    CLASS CONTAINED & DYNAMIC;
    HANDLING READ;
    TYPE BIT_ENUMERATED (1)
    {
        FAULTS_ENUMS_TBD
    }
}*/

/*------------------------------------------/
** Related with developing test purpose(TMP)
/------------------------------------------*/

VARIABLE test_input_bit_enum
{
    LABEL       "Test Input Bit Enumeration";
    HELP        _CONDTB_ENUM_HELP;
    CLASS       CONTAINED&DYNAMIC;
    TYPE        BIT_ENUMERATED (4)
    {
        _CONDTB_TEST_INPUT_BIT_ENUM
    }
    HANDLING    READ & WRITE;
}
/*----------------------------/
** Records definition for SVI2
/----------------------------*/

RECORD svi2_tb_mode_blk
{
    LABEL           [mode_blk_label] ;
    HELP            [mode_blk_help] ;
    MEMBERS
    {
        TARGET,       svi2_tb_target_mode ;
        ACTUAL,       __actual_mode ;
        PERMITTED,    svi2_tb_permitted_mode ;
        NORMAL,       svi2_tb_normal_mode ;
    }
}

RECORD final_value
{
    LABEL LBL_045_FINAL_VALUE;
    HELP HLP5(HLP_045_FINAL_VALUE);
    MEMBERS
    {
        STATUS,                                  __status_contained_srv;
        /*VALUE,                                   __float_contained_nd;*/ /*_047_value;*/
        VALUE,                                   final_value_f;
    }
}
RECORD final_value_range
{
    LABEL LBL_048_FINAL_VALUE_RANGE;
    HELP HLP5(HLP_048_FINAL_VALUE_RANGE);
    MEMBERS
    {
        EU_100,                                  _eu_100_r;
        EU_0,                                    _eu_0_r;
        UNITS_INDEX,                             _units_index_percent_r;
        DECIMAL,                                 _decimal_2_r;
    }
}
RECORD position_limits
{
    LABEL LBL_053_POSITION_LIMITS;
    HELP HLP5(HLP_053_POSITION_LIMITS);
    MEMBERS
    {
        LIMITS_PROTECTED,                        _limits_protected;
        ENABLE_HI,                               _enable_hi;
        ENABLE_LO,                               _enable_lo;
        LIMIT_HI,                                _limit_hi;
        LIMIT_LO,                                _limit_lo;
        ENABLE_RATE_HI,                          _enable_rate_hi;
        ENABLE_RATE_LO,                          _enable_rate_lo;
        LIMIT_RATE,                              _limit_rate;
    }
}
RECORD final_value_cutoff_hi
{
    LABEL LBL_062_FINAL_VALUE_CUTOFF_HI;
    HELP HLP5(HLP_062_FINAL_VALUE_CUTOFF_HI);
    MEMBERS
    {
        ENABLE,                                  _enable_disable;
        CUTOFF_POINT_HI,                         cutoff_point_hi;
    }
}
RECORD final_value_cutoff_lo
{
    LABEL LBL_065_FINAL_VALUE_CUTOFF_LO;
    HELP HLP5(HLP_065_FINAL_VALUE_CUTOFF_LO);
    MEMBERS
    {
        ENABLE,                                  _enable_disable;
        CUTOFF_POINT_LO,                         cutoff_point_lo;
    }
}
RECORD final_position_value
{
    LABEL LBL_068_FINAL_POSITION_VALUE;
    HELP HLP5(HLP_068_FINAL_POSITION_VALUE);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   float_position_value_r;
    }
}
RECORD active_control_set
{
    LABEL LBL_072_ACTIVE_CONTROL_SET;
    HELP HLP5(HLP_072_ACTIVE_CONTROL_SET);
    MEMBERS
    {
        SELECTOR,                                _selector_r;
        P,                                       p_r;
        I,                                       i_r;
        D,                                       d_r;
        PADJ,                                    padj_r;
        BETA,                                    beta_r;
        POSCOMP,                                 poscomp_r;
        DEADZONE,                                deadzone_r;
        NONLIN,                                  nonlin_r;
    }
}
RECORD custom_control_set
{
    LABEL LBL_081_CUSTOM_CONTROL_SET;
    HELP HLP5(HLP_081_CUSTOM_CONTROL_SET);
    MEMBERS
    {
        P,                                       p;
        I,                                       i;
        D,                                       d;
        PADJ,                                    padj;
        BETA,                                    beta;
        POSCOMP,                                 poscomp;
        DEADZONE,                                deadzone;
        NONLIN,                                  nonlin;
    }
}
RECORD backup_control_set
{
    LABEL LBL_090_BACKUP_CONTROL_SET;
    HELP HLP5(HLP_090_BACKUP_CONTROL_SET);
    MEMBERS
    {
        SELECTOR,                                _uint_selector_r;
        P,                                       p_r;
        I,                                       i_r;
        D,                                       d_r;
        PADJ,                                    padj_r;
        BETA,                                    beta_r;
        POSCOMP,                                 poscomp_r;
        DEADZONE,                                deadzone_r;
        NONLIN,                                  nonlin_r;
    }
}
RECORD travel_calibration
{
    LABEL LBL_100_TRAVEL_CALIBRATION;
    HELP HLP5(HLP_100_TRAVEL_CALIBRATION);
    MEMBERS
    {
        CAL_LOCATION,                            _cal_location;
        CAL_DATE,                                _cal_date;
        CAL_WHO,                                 _cal_who;
        CAL_STOP_HI_POS,                        _stop_hi_pos;
        CAL_STOP_LO_POS,                        _stop_lo_pos;
        CAL_TYPE,                                _cal_type;
        LAST_RESULT,                             _last_result_r_enum;
    }
}
RECORD travel
{
    LABEL LBL_108_TRAVEL;
    HELP HLP5(HLP_108_TRAVEL);
    MEMBERS
    {
        RANGE,                                   _float_limited;
        UNITS_INDEX,                             _units_index_travel;
    }
}
RECORD working_sp
{
    LABEL LBL_111_WORKING_SP;
    HELP HLP5(HLP_111_WORKING_SP);
    MEMBERS
    {
        STATUS,                                  __status_contained;
        VALUE,                                   __float_contained;
    }
}
RECORD working_pos
{
    LABEL LBL_114_WORKING_POS;
    HELP HLP5(HLP_114_WORKING_POS);
    MEMBERS
    {
        STATUS,                                  __status_contained;
        VALUE,                                   __float_contained;
    }
}
RECORD svi2_deviation_alert
{
    LABEL LBL_117_DEVIATION_ALERT;
    HELP HLP5(HLP_117_DEVIATION_ALERT);
    MEMBERS
    {
        DEVIATION_VALUE_1, /*_svi2*/                    __float_contained_r;
        ALERT_POINT,                             _alert_point_float_nd;
        DEADBAND,                                _dead_band_float_nd;
        DURATION_TIME,                           _time_uint_4;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD position_hihi_alert
{
    LABEL LBL_125_POSITION_HIHI_ALERT;
    HELP HLP5(HLP_125_POSITION_HIHI_ALERT);
    MEMBERS
    {
        POSITION,                                __float_contained_r;
        ALERT_POINT,                             position_hihi_point;
        DEADBAND,                                position_hihi_deadband;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD position_hi_alert
{
    LABEL LBL_132_POSITION_HI_ALERT;
    HELP HLP5(HLP_132_POSITION_HI_ALERT);
    MEMBERS
    {
        POSITION,                                __float_contained_r;
        ALERT_POINT,                             position_hi_point;
        DEADBAND,                                position_hi_deadband;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD position_lo_alert
{
    LABEL LBL_139_POSITION_LO_ALERT;
    HELP HLP5(HLP_139_POSITION_LO_ALERT);
    MEMBERS
    {
        POSITION,                                __float_contained_r;
        ALERT_POINT,                             position_lo_point;
        DEADBAND,                                position_lo_deadband;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD position_lolo_alert
{
    LABEL LBL_146_POSITION_LOLO_ALERT;
    HELP HLP5(HLP_146_POSITION_LOLO_ALERT);
    MEMBERS
    {
        POSITION,                                __float_contained_r;
        ALERT_POINT,                             position_lolo_point;
        DEADBAND,                                position_lolo_deadband;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD travel_accumulation_a_alert
{
    LABEL LBL_153_TRAVEL_ACCUMULATION_A_ALERT;
    HELP HLP5(HLP_153_TRAVEL_ACCUMULATION_A_ALERT);
    MEMBERS
    {
        TRAVEL_ACCUMULATION,                     _travel_accumulation;
        ALERT_POINT,                             trvl_accmltn_alert_point;
        DEADBAND_AB,                             _deadband_ab_float_nd;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD travel_accumulation_b_alert
{
    LABEL LBL_160_TRAVEL_ACCUMULATION_B_ALERT;
    HELP HLP5(HLP_160_TRAVEL_ACCUMULATION_B_ALERT);
    MEMBERS
    {
        TRAVEL_ACCUMULATION,                     _travel_accumulation;
        ALERT_POINT,                             trvl_accmltn_alert_point;
        DEADBAND_AB,                             _deadband_ab_float_nd;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD travel_accumulation_trend
{
    LABEL LBL_167_TRAVEL_ACCUMULATION_TREND;
    HELP HLP5(HLP_167_TRAVEL_ACCUMULATION_TREND);
    MEMBERS
    {
        CURRENTLY_COLLECTED,                     _currently_collected;
        TODAY,                                   _today;
        LAST_DAY,                                _last_day;
        PREVIOUS_DAY,                            _previous_day;
        THREE_DAYS_AGO,                          _three_days_ago;
        CURRENT_WEEK,                            _current_week;
        LAST_WEEK,                               _last_week;
        PREVIOUS_WEEK,                           _previous_week;
        THREE_WEEKS_AGO,                         _three_weeks_ago;
        CURRENT_MONTH,                           _current_month;
        LAST_MONTH,                              _last_month;
        PREVIOUS_MONTH,                          _previous_month;
        THREE_MONTHS_AGO,                        _three_months_ago;
        CURRENT_12_MONTHS,                       _current_12_months;
        LAST_12_MONTHS,                          _last_12_months;
        PREVIOUS_12_MONTHS,                      _previous_12_months;
        THREE_YEARS_AGO,                         _three_years_ago;
        /*TOTAL_SVI2,                              uint_4_element_r_nd;*/ /*_184_total;*/
    }
}
RECORD svi2_cycle_counter_a_alert
{
    LABEL LBL_185_CYCLE_COUNTER_A_ALERT;
    HELP HLP5(HLP_185_CYCLE_COUNTER_A_ALERT);
    MEMBERS
    {
        CYCLE_COUNTER,                           _cycle_counter;
        ALERT_POINT,                             _alert_point_cnt;
        DEADBAND_AB,                             _deadband_ab_for_cycle_count;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD cycle_counter_b_alert
{
    LABEL LBL_192_CYCLE_COUNTER_B_ALERT;
    HELP HLP5(HLP_192_CYCLE_COUNTER_B_ALERT);
    MEMBERS
    {
        CYCLE_COUNTER,                           _cycle_counter;
        ALERT_POINT,                             _alert_point_cnt;
        DEADBAND_AB,                             _deadband_ab_for_cycle_count;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD cycle_counter_trend
{
    LABEL LBL_199_CYCLE_COUNTER_TREND;
    HELP HLP5(HLP_199_CYCLE_COUNTER_TREND);
    MEMBERS
    {
        CURRENTLY_COLLECTED,                     _currently_collected;
        TODAY,                                   _today;
        LAST_DAY,                                _last_day;
        PREVIOUS_DAY,                            _previous_day;
        THREE_DAYS_AGO,                          _three_days_ago;
        CURRENT_WEEK,                            _current_week;
        LAST_WEEK,                               _last_week;
        PREVIOUS_WEEK,                           _previous_week;
        THREE_WEEKS_AGO,                         _three_weeks_ago;
        CURRENT_MONTH,                           _current_month;
        LAST_MONTH,                              _last_month;
        PREVIOUS_MONTH,                          _previous_month;
        THREE_MONTHS_AGO,                        _three_months_ago;
        CURRENT_12_MONTHS,                       _current_12_months;
        LAST_12_MONTHS,                          _last_12_months;
        PREVIOUS_12_MONTHS,                      _previous_12_months;
        THREE_YEARS_AGO,                         _three_years_ago;
    }
}
RECORD position_error_trend
{
    LABEL LBL_217_POSITION_ERROR_TREND;
    HELP HLP5(HLP_217_POSITION_ERROR_TREND);
    MEMBERS
    {
        CURRENTLY_COLLECTED,                     _float_currently_collected;
        TODAY,                                   _float_today;
        LAST_DAY,                                _float_last_day;
        PREVIOUS_DAY,                            _float_previous_day;
        THREE_DAYS_AGO,                          _float_three_days_ago;
        CURRENT_WEEK,                            _float_current_week;
        LAST_WEEK,                               _float_last_week;
        PREVIOUS_WEEK,                           _float_previous_week;
        THREE_WEEKS_AGO,                         _float_three_weeks_ago;
        CURRENT_MONTH,                           _float_current_month;
        LAST_MONTH,                              _float_last_month;
        PREVIOUS_MONTH,                          _float_previous_month;
        THREE_MONTHS_AGO,                        _float_three_months_ago;
        CURRENT_12_MONTHS,                       _float_current_12_months;
        LAST_12_MONTHS,                          _float_last_12_months;
        PREVIOUS_12_MONTHS,                      _float_previous_12_months;
        THREE_YEARS_AGO,                         _float_three_years_ago;
        /*TOTAL are removed in version 26/10*/
    }
}
RECORD position_histogram
{
    LABEL LBL_235_POSITION_HISTOGRAM;
    HELP HLP5(HLP_235_POSITION_HISTOGRAM);
    MEMBERS
    {
        TOTAL_TIME,                              position_history_total_time;
        POS_5_PERCENT_CLOSED,                    pos_5_percent_closed_r;
        POS_10_PERCENT,                          pos_10_percent_r;
        POS_20_PERCENT,                          pos_20_percent_r;
        POS_30_PERCENT,                          pos_30_percent_r;
        POS_40_PERCENT,                          pos_40_percent_r;
        POS_50_PERCENT,                          pos_50_percent_r;
        POS_60_PERCENT,                          pos_60_percent_r;
        POS_70_PERCENT,                          pos_70_percent_r;
        POS_80_PERCENT,                          pos_80_percent_r;
        POS_90_PERCENT,                          pos_90_percent_r;
        POS_95_PERCENT,                          pos_95_percent_r;
        POS_95_PERCENT_OPEN,                     pos_95_percent_open_r;
    }
}
RECORD near_closed_alert
{
    LABEL LBL_249_NEAR_CLOSED_ALERT;
    HELP HLP5(HLP_249_NEAR_CLOSED_ALERT);
    MEMBERS
    {
        POINT_CLOSED,                            point_closed;
        NEAR_CLOSED,                             _near_closed;
        ALERT_POINT,                             _alert_point_time;
        ALERT,                                   _alert;
#ifndef NOUPDATES
        HISTORIC_ALERT,                          _historic_alert;
#else
        HISTORIC_ALERT,                          _historic_alert_for_near_closed_alert; /* see 18272 & mail */
#endif /* NOUPDATES */
        ENABLE,                                  _enable;
    }
}
RECORD position_error_histogram
{
    LABEL LBL_255_POSITION_ERROR_HISTOGRAM;
    HELP HLP5(HLP_255_POSITION_ERROR_HISTOGRAM);
    MEMBERS
    {
        POS_5_PERCENT_CLOSED,                    __float_contained_nd;
        POS_10_PERCENT,                          __float_contained_nd;
        POS_20_PERCENT,                          __float_contained_nd;
        POS_30_PERCENT,                          __float_contained_nd;
        POS_40_PERCENT,                          __float_contained_nd;
        POS_50_PERCENT,                          __float_contained_nd;
        POS_60_PERCENT,                          __float_contained_nd;
        POS_70_PERCENT,                          __float_contained_nd;
        POS_80_PERCENT,                          __float_contained_nd;
        POS_90_PERCENT,                          __float_contained_nd;
        POS_95_PERCENT,                          __float_contained_nd;
        POS_95_PERCENT_OPEN,                     __float_contained_nd;
    }
}
RECORD setpoint_timeout_alert
{
    LABEL LBL_268_SETPOINT_TIMEOUT_ALERT;
    HELP HLP5(HLP_268_SETPOINT_TIMEOUT_ALERT);
    MEMBERS
    {
        TIME_SINCE_UPDATE,                       time_since_alert;
        ALERT_POINT,                             setpoint_alert_point;
        MAX_TIME,                                setpoint_max_time;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD xd_fstate
{
    LABEL LBL_275_XD_FSTATE;
    HELP HLP5(HLP_275_XD_FSTATE);
    MEMBERS
    {
        CONFIGURATION,                           xd_fs_configuration;
        XD_FSTATE_OPT_1, /*_svi2*/                      xd_fstate_opt;
        FSTATE_VALUE_1, /*_svi2*/                       _fstate_value;
        FSTATE_TIME_1, /*_svi2*/                        _fstate_time;
    }
}
RECORD char_selection
{
    LABEL LBL_280_CHAR_SELECTION;
    HELP HLP5(HLP_280_CHAR_SELECTION);
    MEMBERS
    {
        TYPE_1, /*_svi2*/                               character_type;
        NUMBER_OF_POINTS_1, /*_svi2*/                   char_sel_number_of_points_r;
    }
}
RECORD custom_char
{
    LABEL LBL_284_CUSTOM_CHAR;
    HELP HLP5(HLP_284_CUSTOM_CHAR);
    MEMBERS
    {
        ACTION,                                  _action;
        NUMBER_OF_POINTS_1, /*_svi2*/                   _number_of_points_custom_char;
    }
}
RECORD supply_pressure
{
    LABEL LBL_291_SUPPLY_PRESSURE;
    HELP HLP5(HLP_291_SUPPLY_PRESSURE);
    MEMBERS
    {
        STATUS,                                  __status_contained_r; /*see bug:12555*/
        VALUE,                                   __float_contained_r;
    }
}
RECORD pressure_range
{
    LABEL LBL_294_PRESSURE_RANGE;
    HELP HLP5(HLP_294_PRESSURE_RANGE);
    MEMBERS
    {
        EU_100,                                  _eu_100_r;
        EU_0,                                    _eu_0_r;
        UNITS_INDEX,                             _units_index_press;
        DECIMAL,                                 _decimal_3;
    }
}
RECORD supply_pressure_hi_alert
{
    LABEL LBL_299_SUPPLY_PRESSURE_HI_ALERT;
    HELP HLP5(HLP_299_SUPPLY_PRESSURE_HI_ALERT);
    MEMBERS
    {
        PRESSURE,                                __float_contained_r;
        ALERT_POINT,                             _alert_point_press_hi;
        DEADBAND,                                _deadband_press;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD supply_pressure_lo_alert
{
    LABEL LBL_306_SUPPLY_PRESSURE_LO_ALERT;
    HELP HLP5(HLP_306_SUPPLY_PRESSURE_LO_ALERT);
    MEMBERS
    {
        PRESSURE,                                __float_contained_r;
        ALERT_POINT,                             _alert_point_press_lo;
        DEADBAND,                                _deadband_press;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD supply_pressure_lolo_alert
{
    LABEL LBL_313_SUPPLY_PRESSURE_LOLO_ALERT;
    HELP HLP5(HLP_313_SUPPLY_PRESSURE_LOLO_ALERT);
    MEMBERS
    {
        PRESSURE,                                __float_contained_r;
        ALERT_POINT,                             _alert_point_press_lo;
        DEADBAND,                                _deadband_press;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD actuator_a_pressure
{
    LABEL LBL_320_ACTUATOR_A_PRESSURE;
    HELP HLP5(HLP_320_ACTUATOR_A_PRESSURE);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   __float_contained_r;
    }
}
RECORD actuator_b_pressure
{
    LABEL LBL_323_ACTUATOR_B_PRESSURE;
    HELP HLP5(HLP_323_ACTUATOR_B_PRESSURE);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   __float_contained_r;
    }
    EDD(VALIDITY FALSE;)
}
RECORD atmospheric_pressure
{
    LABEL LBL_326_ATMOSPHERIC_PRESSURE;
    HELP HLP5(HLP_326_ATMOSPHERIC_PRESSURE);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   __float_contained_r;
    }
}
RECORD pilot_pressure
{
    LABEL LBL_329_PILOT_PRESSURE;
    HELP HLP5(HLP_329_PILOT_PRESSURE);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   __float_contained_r;
    }
}
/* RECORD pressure_calibration */
/* { */
/*     LABEL LBL_332_PRESSURE_CALIBRATION; */
/*     HELP HLP5(HLP_332_PRESSURE_CALIBRATION); */
/*     MEMBERS */
/*     { */
/*         CAL_LOCATION,                            _cal_location; */
/*         CAL_DATE,                                _cal_date; */
/*         CAL_WHO,                                 _cal_who; */
/*         CAL_TYPE,                                _cal_type_r; */
/*         LAST_RESULT,                             _last_result_r; */
/*     } */
/* } */
RECORD temperature
{
    LABEL LBL_341_TEMPERATURE;
    HELP HLP5(HLP_341_TEMPERATURE);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   temp_value_with_unit;
    }
}
RECORD temperature_hi_alert
{
    LABEL LBL_344_TEMPERATURE_HI_ALERT;
    HELP HLP5(HLP_344_TEMPERATURE_HI_ALERT);
    MEMBERS
    {
        TEMPERATURE_1, /*_svi2*/                        temp_value_with_unit;
        ALERT_POINT,                             _alert_point_temp_hi;
        DEADBAND,                                _deadband_temp;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD temperature_lo_alert
{
    LABEL LBL_351_TEMPERATURE_LO_ALERT;
    HELP HLP5(HLP_351_TEMPERATURE_LO_ALERT);
    MEMBERS
    {
        TEMPERATURE_1, /*_svi2*/                        __float_contained_r;
        ALERT_POINT,                             _alert_point_temp_lo;
        DEADBAND,                                _deadband_temp;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD ip_drive_current
{
    LABEL LBL_358_IP_DRIVE_CURRENT;
    HELP HLP5(HLP_358_IP_DRIVE_CURRENT);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   _value_ip_r;
    }
}
RECORD ip_drive_current_hi_alert
{
    LABEL LBL_361_IP_DRIVE_CURRENT_HI_ALERT;
    HELP HLP5(HLP_361_IP_DRIVE_CURRENT_HI_ALERT);
    MEMBERS
    {
        CURRENT,                                 _current;
        ALERT_POINT,                             _alert_point_ip_current_hi;
        DEADBAND,                                _ip_drive_current_deadband;
        DURATION_TIME,                           _time_float;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD ip_drive_current_lo_alert
{
    LABEL LBL_368_IP_DRIVE_CURRENT_LO_ALERT;
    HELP HLP5(HLP_368_IP_DRIVE_CURRENT_LO_ALERT);
    MEMBERS
    {
        CURRENT,                                 _current;
        ALERT_POINT,                             _alert_point_current_lo;
        DEADBAND,                                _ip_drive_current_deadband;
        DURATION_TIME,                               _time_float;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD autotune
{
    LABEL LBL_376_AUTOTUNE;
    HELP HLP5(HLP_376_AUTOTUNE);
    MEMBERS
    {
        SUPPLY_PRESS,                            __float_contained_nd;
        AGGRESSIVENESS,                          aggressiveness;
        TUNE_FLAGS,                              tune_flags;
        COMPLETION,                              completion;
    }
}
/* RECORD autotune_state */
/* { */
/*     LABEL LBL_380_AUTOTUNE_STATE; */
/*     HELP HLP5(HLP_380_AUTOTUNE_STATE); */
/*     MEMBERS */
/*     { */
/*         START_SVI2,                              _381_start; */
/*         RESULT,                                  _383_result; */
/*     } */
/* } */
/* RECORD travel_calibration */
/* { */
/*     LABEL LBL_384_TRAVEL_CALIBRATION; */
/*     HELP HLP5(HLP_384_TRAVEL_CALIBRATION); */
/*     MEMBERS */
/*     { */
/*         START_SVI2,                              _385_start; */
/*         RESULT,                                  _386_result; */
/*     } */
/* } */
RECORD check_process
{
    LABEL LBL_387_CHECK_PROCESS;
    HELP HLP5(HLP_387_CHECK_PROCESS);
    MEMBERS
    {
        PROCESS_ID,                              _process_id_r;
        PERCENT_COMPLETE,                        _percent_complete_r;
    }
}
RECORD ui_custom_configuration
{
    LABEL LBL_390_UI_CUSTOM_CONFIGURATION;
    HELP HLP5(HLP_390_UI_CUSTOM_CONFIGURATION);
    MEMBERS
    {
        CUSTOM_1_CONFIGURATION,                  custom_1_configuration;
        CUSTOM_2_CONFIGURATION,                  custom_2_configuration;
    }
}
RECORD failed_state
{
    LABEL LBL_393_FAILED_STATE;
    HELP HLP5(HLP_393_FAILED_STATE);
    MEMBERS
    {
        FF,                                      _ff_r;
        APP,                                     _app_r;
        PROPAGATE_MODE,                          propagate_mode;
    }
}
RECORD final_value_d
{
    LABEL LBL_396_FINAL_VALUE_D;
    HELP HLP5(HLP_396_FINAL_VALUE_D);
    MEMBERS
    {
        STATUS,                                  __status_contained;
        VALUE,                                   _discrete_value_nd;
    }
}
RECORD final_position_value_d
{
    LABEL LBL_399_FINAL_POSITION_VALUE_D;
    HELP HLP5(HLP_399_FINAL_POSITION_VALUE_D);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   _discrete_value_int_nd_r;
    }
}
RECORD final_value_dint
{
    LABEL LBL_402_FINAL_VALUE_DINT;
    HELP HLP5(HLP_402_FINAL_VALUE_DINT);
    MEMBERS
    {
        STATUS,                                  __status_contained;
        VALUE,                                   _discrete_value_int_nd;
    }
}
RECORD final_position_value_dint
{
    LABEL LBL_405_FINAL_POSITION_VALUE_DINT;
    HELP HLP5(HLP_405_FINAL_POSITION_VALUE_DINT);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   _discrete_value_int_nd_r;
    }
}
RECORD fsc_set_d 
{
    LABEL LBL_408_FSC_SET_D;
    HELP HLP5(HLP_408_FSC_SET_D);
    MEMBERS
    {
        STATUS,                                  __status_contained;
        VALUE,                                   _discrete_value;
    }
}
RECORD fsc_actual_d
{
    LABEL LBL_411_FSC_ACTUAL_D;
    HELP HLP5(HLP_411_FSC_ACTUAL_D);
    MEMBERS
    {
        STATUS,                                  __status_contained;
        VALUE,                                   _discrete_value;
    }
}
RECORD discrete_output_1_state
{
    LABEL LBL_414_DISCRETE_OUTPUT_1;
    HELP HLP5(HLP_414_DISCRETE_OUTPUT_1);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   _discrete_state_r;
    }
}
RECORD discrete_output_2_state
{
    LABEL LBL_417_DISCRETE_OUTPUT_2;
    HELP HLP5(HLP_417_DISCRETE_OUTPUT_2);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   _discrete_state_r;
    }
}

RECORD discrete_input
{
    LABEL LBL_DISCRETE_INPUT;
    HELP  HLP5(HLP_DISCRETE_INPUT);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   discrete_input_value;
    }
}

RECORD discrete_switch_1_conf
{
    LABEL LBL_426_DISCRETE_SWITCH_1_CONF;
    HELP HLP5(HLP_426_DISCRETE_SWITCH_1_CONF);
    MEMBERS
    {
        DIRECTION,                               _direction;
        FUNCTION,                                _function;
    }
}
RECORD discrete_switch_2_conf
{
    LABEL LBL_429_DISCRETE_SWITCH_2_CONF;
    HELP HLP5(HLP_429_DISCRETE_SWITCH_2_CONF);
    MEMBERS
    {
        DIRECTION,                               _direction;
        FUNCTION,                                _function;
    }
}
RECORD ui_access_control
{
    LABEL LBL_432_UI_ACCESS_CONTROL;
    HELP HLP5(HLP_432_UI_ACCESS_CONTROL);
    MEMBERS
    {
        LOCK_LEVEL,                              _lock_level;
        PASSWORD_ENABLED,                        _password_enabled;
        PASSWORD_1, /*_svi2*/                           _password;
    }
}
RECORD setpoint
{
    LABEL LBL_442_SETPOINT;
    HELP HLP5(HLP_442_SETPOINT);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   _value_open_close_r;
    }
}

/* RECORD manual_setpoint */
/* { */
/*     LABEL LBL_MANUAL_SETPOINT; */
/*     HELP HLP5(HLP_MANUAL_SETPOINT); */
/*     MEMBERS */
/*     { */
/*         STATUS,                                  __status_contained_r; */
/*         VALUE,                                   __float_contained_r; */
/*     } */
/* } */

RECORD actual_position
{
    LABEL LBL_445_ACTUAL_POSITION;
    HELP HLP5(HLP_445_ACTUAL_POSITION);
    MEMBERS
    {
        STATUS,                                  __status_contained_r;
        VALUE,                                   _value_open_close_r;
    }
}

/* RECORD wrong_configuration */
/* { */
/*     LABEL LBL_448_WRONG_CONFIGURATION; */
/*     HELP HLP5(HLP_448_WRONG_CONFIGURATION); */
/*     MEMBERS */
/*     { */
/*         INDEX_SVI2,                              _449_index; */
/*         SUB_INDEX,                               _450_sub_index; */
/*         DESCRIPTION_SVI2,                        _451_description; */
/*     } */
/* } */

RECORD alert_action
{
    LABEL LBL_452_ALERT_ACTION;
    HELP HLP5(HLP_452_ALERT_ACTION);
    MEMBERS
    {
        MAPPED_TO_RB,                            mapped_to_rb;
        DEVIATION_ALERT_ACT,                     _deviation_alert_nd;
        POSITION_HIHI_ALERT_ACT,                 position_hihi_alert_act;
        POSITION_HI_ALERT_ACT,                   position_hi_alert_act;
        POSITION_LO_ALERT_ACT,                   position_lo_alert_act;
        POSITION_LOLO_ALERT_ACT,                 position_lolo_alert_act;
        SET_POINT_TIMEOUT_ALERT_ACT,                 set_point_timeout_alert;
        NEAR_CLOSE_ALERT_ACT,                        near_close_alert;
        TRAVEL_ACCUMULATION_A_ALERT_ACT,         travel_accumulation_a_alert_act;
        TRAVEL_ACCUMULATION_B_ALERT_ACT,         travel_accumulation_b_alert_act;
        CYCLE_COUNTER_A_ALERT_ACT,               cycle_counter_a_alert_act;
        CYCLE_COUNTER_B_ALERT_ACT,               cycle_counter_b_alert_act;
        WORKING_TIME_ALERT_ACT,                  working_time_alert_act;
        SUPPLY_PRESSURE_HI_ALERT_ACT,            supply_pressure_hi_alert_act;
        SUPPLY_PRESSURE_LO_ALERT_ACT,            supply_pressure_lo_alert_act;
        SUPPLY_PRESSURE_LOLO_ALERT_ACT,          supply_pressure_lolo_alert_act;
        TEMPERATURE_HI_ALERT_ACT,                temperature_hi_alert_act;
        TEMPERATURE_LO_ALERT_ACT,                temperature_lo_alert_act;
        IP_DRIVE_CURRENT_ALERT_HI_ACT,           ip_drive_current_alert_hi;
        IP_DRIVE_CURRENT_ALERT_LO_ACT,           ip_drive_current_alert_lo;
        SENSOR_FAILURE_ALERT_ACT,                sensor_failure_alert;
        PROCESSOR_ALERT_ACT,                     processor_alert;
        VALVE_CONTROL_ALERT_ACT,                 valve_control_alert;
        COMMISSIONING_ALERT_ACT,                 commissioning_alert;
        AIR_SUPPLY_ALERT_ACT,                    air_supply_alert;
        SUPPORTING_HARDWARE_ALERT_ACT,           supporting_hardware_alert;
    }
}
RECORD alert_state
{
    LABEL LBL_479_ALERT_STATE;
    HELP HLP5(HLP_479_ALERT_STATE);
    MEMBERS
    {
        DEVIATION_ALERT_STATE,                   _deviation_alert_r;
        POSITION_HIHI_ALERT_STATE,               _position_hihi_alert_r;
        POSITION_HI_ALERT_STATE,                 _position_hi_alert_r;
        POSITION_LO_ALERT_STATE,                 _position_lo_alert_r;
        POSITION_LOLO_ALERT_STATE,               _position_lolo_alert_r;
        SET_POINT_TIMEOUT_ALERT_STATE,                 _set_point_timeout_alert_r;
        NEAR_CLOSE_ALERT_STATE,                        _near_close_alert_r;
        TRAVEL_ACCUMULATION_A_ALERT_STATE,       _travel_accumulation_a_alert_r;
        TRAVEL_ACCUMULATION_B_ALERT_STATE,       _travel_accumulation_b_alert_r;
        CYCLE_COUNTER_A_ALERT_STATE,             _cycle_counter_a_alert_r;
        CYCLE_COUNTER_B_ALERT_STATE,             _cycle_counter_b_alert_r;
        WORKING_TIME_ALERT_STATE,                      _working_time_alert_r;
        SUPPLY_PRESSURE_HI_ALERT_STATE,          _supply_pressure_hi_alert_r;
        SUPPLY_PRESSURE_LO_ALERT_STATE,          _supply_pressure_lo_alert_r;
        SUPPLY_PRESSURE_LOLO_ALERT_STATE,        _supply_pressure_lolo_alert_r;
        TEMPERATURE_HI_ALERT_STATE,              _temperature_hi_alert_r;
        TEMPERATURE_LO_ALERT_STATE,              _temperature_lo_alert_r;
        IP_DRIVE_CURRENT_ALERT_HI_STATE,               _ip_drive_current_alert_hi_r;
        IP_DRIVE_CURRENT_ALERT_LO_STATE,               _ip_drive_current_alert_lo_r;
        SENSOR_FAILURE_ALERT_STATE,                    _sensor_failure_alert_r;
        PROCESSOR_ALERT_STATE,                         _processor_alert_r;
        VALVE_CONTROL_ALERT_STATE,                     _valve_control_alert_r;
        COMMISSIONING_ALERT_STATE,                     _commissioning_alert_r;
        AIR_SUPPLY_ALERT_STATE,                        _air_supply_alert_r;
        SUPPORTING_HARDWARE_ALERT_STATE,               _supporting_hardware_alert_r;
    }
}
RECORD alert_counters
{
    LABEL LBL_505_ALERT_COUNTERS;
    HELP HLP5(HLP_505_ALERT_COUNTERS);
    MEMBERS
    {
        DEVIATION_ALERT_CNT,                     _deviation_alert;
        POSITION_HIHI_ALERT_CNT,                 _position_hihi_alert;
        POSITION_HI_ALERT_CNT,                   _position_hi_alert;
        POSITION_LO_ALERT_CNT,                   _position_lo_alert;
        POSITION_LOLO_ALERT_CNT,                 _position_lolo_alert;
        SET_POINT_TIMEOUT_ALERT_CNT,             _set_point_timeout_alert_2;
        NEAR_CLOSE_ALERT_CNT,                    _near_close_alert;
        TRAVEL_ACCUMULATION_A_ALERT_CNT,         _travel_accumulation_a_alert_2;
        TRAVEL_ACCUMULATION_B_ALERT_CNT,         _travel_accumulation_b_alert_2;
        CYCLE_COUNTER_A_ALERT_CNT,               _cycle_counter_a_alert;
        CYCLE_COUNTER_B_ALERT_CNT,               _cycle_counter_b_alert;
        WORKING_TIME_ALERT_CNT,                  _working_time_alert;
        SUPPLY_PRESSURE_HI_ALERT_CNT,            _supply_pressure_hi_alert_2;
        SUPPLY_PRESSURE_LO_ALERT_CNT,            _supply_pressure_lo_alert_2;
        SUPPLY_PRESSURE_LOLO_ALERT_CNT,          _supply_pressure_lolo_alert_2;
        TEMPERATURE_HI_ALERT_CNT,                _temperature_hi_alert_2;
        TEMPERATURE_LO_ALERT_CNT,                _temperature_lo_alert_2;
        IP_DRIVE_CURRENT_ALERT_HI_CNT,           _ip_drive_current_alert_hi;
        IP_DRIVE_CURRENT_ALERT_LO_CNT,           _ip_drive_current_alert_lo;
        SENSOR_FAILURE_ALERT_CNT,                _sensor_failure_alert_2;
        PROCESSOR_ALERT_CNT,                     _processor_alert_2;
        VALVE_CONTROL_ALERT_CNT,                 _valve_control_alert_2;
        COMMISSIONING_ALERT_CNT,                 _commissioning_alert;
        AIR_SUPPLY_ALERT_CNT,                    _air_supply_alert;
        SUPPORTING_HARDWARE_ALERT_CNT,           _supporting_hardware_alert_2;
    }
}
RECORD alert_log
{
    LABEL LBL_ALERT_LOG;
    HELP HLP5(HLP_ALERT_LOG);
    MEMBERS
    {
        COMMAND,         _alert_command;
        ALERT_TIME,      __time_stamp;
        ALERT_DATA,      _alert_data_r;
    }
}

RECORD working_times
{
    LABEL LBL_531_WORKING_TIMES;
    HELP HLP5(HLP_531_WORKING_TIMES);
    MEMBERS
    {
        SINCE_RESET,                             _since_reset;
        TOTAL_TIME,                              _total_time_r;
    }
}
RECORD working_time_alert
{
    LABEL LBL_535_WORKING_TIME_ALERT;
    HELP HLP5(HLP_535_WORKING_TIME_ALERT);
    MEMBERS
    {
        ALERT_TOTAL_TIME,                        _alert_total_time_r; /*Bug #17523;*/
        ALERT_POINT,                             trvl_accmltn_alert_point;
        ALERT,                                   _alert;
        HISTORIC_ALERT,                          _historic_alert;
        ENABLE,                                  _enable;
    }
}
RECORD actuator_1
{
    LABEL LBL_543_ACTUATOR_1;
    HELP HLP5(HLP_543_ACTUATOR_1);
    MEMBERS
    {
        ACT_MAN_ID_1, /*_svi2*/                         _act_man_id;
        ACT_MODEL_NUM_1, /*_svi2*/                      _act_model_num;
        ACT_SN_1, /*_svi2*/                             _act_sn;
    }
}
RECORD actuator_2
{
    LABEL LBL_548_ACTUATOR_2;
    HELP HLP5(HLP_548_ACTUATOR_2);
    MEMBERS
    {
        ACT_TYPE,                                _act_type;
        ACT_SIZE,                                _act_size;
        ACT_ROTARY_MOMENT_ARM,                   _act_rotary_moment_arm;
        ACT_EFFECTIVE_AREA,                      _act_effective_area;
    }
}
RECORD actuator_3
{
    LABEL LBL_553_ACTUATOR_3;
    HELP HLP5(HLP_553_ACTUATOR_3);
    MEMBERS
    {
        SHUTOFF_DP,                              _shutoff_dp;
        HAND_WHEEL,                              _hand_wheel;
        ACT_STYLE,                               act_style;
        ACT_FAIL_ACTION_1, /*_svi2*/                    _act_fail_action;
        /* SENSOR_INCREASE,                         _558_sensor_increase; */
        RELAY_TYPE,                              _relay_type;
        SUPPLY_PRS_MAX,                          _supply_pres_max;
        PRS_CONTROL_HI,                          _prs_control_hi;
        PRS_CONTROL_LO,                          _prs_control_lo;

    }
}
RECORD actuator_info
{
    LABEL LBL_563_ACTUATOR_INFO;
    HELP HLP5(HLP_563_ACTUATOR_INFO);
    MEMBERS
    {
        DESCRIPTOR,                              _descriptor;
        MESSAGE,                                 _message;
        DATE_INFO,                               _date;
        SPEC_SHEET,                              _spec_sheet;
    }
}
RECORD valve_identification
{
    LABEL LBL_568_VALVE_IDENTIFICATION;
    HELP HLP5(HLP_568_VALVE_IDENTIFICATION);
    MEMBERS
    {
        VALVE_MAN_ID_1, /*_svi2*/                       _valve_man_id;
        VALVE_MODEL_NUM_1, /*_svi2*/                    _valve_model_num;
        VALVE_SN_1, /*_svi2*/                           _valve_sn;
    }
}
RECORD valve_service
{
    LABEL LBL_572_VALVE_SERVICE;
    HELP HLP5(HLP_572_VALVE_SERVICE);
    MEMBERS
    {
        SERVICE_1, /*_svi2*/                            _service;
        PID_NO,                                  _pid_no;
    }
}
RECORD valve_body_1
{
    LABEL LBL_575_VALVE_BODY_1;
    HELP HLP5(HLP_575_VALVE_BODY_1);
    MEMBERS
    {
        VALVE_TYPE_1, /*_svi2*/                         _valve_type;
        BODY_SIZE,                               _body_size;
        PACKING,                                 _packing;
        PLUG_TYPE,                               _plug_type;
        SEAT_RING_TYPE,                          _seat_ring_type;
    }
}
RECORD valve_body_2
{
    LABEL LBL_581_VALVE_BODY_2;
    HELP HLP5(HLP_581_VALVE_BODY_2);
    MEMBERS
    {
        CHARACTERISTIC,                          _characteristic;
        LEAKAGE_CLASS,                           _leakage_class;
    }
}

RECORD valve_body_3
{
    LABEL LBL_581_VALVE_BODY_2;
    HELP HLP5(HLP_581_VALVE_BODY_2);
    MEMBERS
    {
        FLOW_ACTION,                             _flow_action;
        RATED_ADJ_CV,                            _rated_adj_cv;
    }
}
RECORD valve_info
{
    LABEL LBL_587_VALVE_INFO;
    HELP HLP5(HLP_587_VALVE_INFO);
    MEMBERS
    {
        SAME_AS_ACTUATOR,                        _same_as_actuator;
        DESCRIPTOR,                              _descriptor;
        MESSAGE,                                 _message;
        DATE_INFO,                               _date;
        SPEC_SHEET,                              _spec_sheet;
    }
}
RECORD booster
{
    LABEL LBL_593_BOOSTER;
    HELP HLP5(HLP_593_BOOSTER);
    MEMBERS
    {
        MANUFACTURER_INFO,                       _manufacturer;
        MODEL,                                   _model;
        QTY,                                     _qty;
    }
}
RECORD accessory
{
    LABEL LBL_597_ACCESSORY;
    HELP HLP5(HLP_597_ACCESSORY);
    MEMBERS
    {
        SOLENOID,                                _solenoid;
        REMOTE_SENSOR,                           _remote_sensor;
    }
}
RECORD position_extremes
{
    LABEL LBL_599_POSITION_EXTREMES;
    HELP HLP5(HLP_599_POSITION_EXTREMES);
    MEMBERS
    {
        FINAL_VALUE_MAX,                         _final_value_max;
        FINAL_VALUE_MIN,                         _final_value_min;
        FINAL_POS_VALUE_MAX,                     _final_pos_value_max;
        FINAL_POS_VALUE_MIN,                     _final_pos_value_min;
        WORKING_SP_MAX,                          _working_sp_max;
        WORKING_SP_MIN,                          _working_sp_min;
        WORKING_POS_MAX,                         _working_pos_max;
        WORKING_POS_MIN,                         _working_pos_min;
    }
}
RECORD pressure_extremes
{
    LABEL LBL_608_PRESSURE_EXTREMES;
    HELP HLP5(HLP_608_PRESSURE_EXTREMES);
    MEMBERS
    {
        SUPPLY_PRESSURE_MAX,                     _supply_pressure_max;
        SUPPLY_PRESSURE_MIN,                     _supply_pressure_min;
        ACTUATOR_A_MAX,                          _actuator_a_max;
        ACTUATOR_A_MIN,                          _actuator_a_min;
        ACTUATOR_B_MAX,                          _actuator_b_max;
        ACTUATOR_B_MIN,                          _actuator_b_min;
        PILOT_MAX,                               _pilot_max;
        PILOT_MIN,                               _pilot_min;
    }
}
RECORD temperature_extremes
{
    LABEL LBL_617_TEMPERATURE_EXTREMES;
    HELP HLP5(HLP_617_TEMPERATURE_EXTREMES);
    MEMBERS
    {
        TEMPERATURE_MAX,                         _temperature_max;
        TEMPERATURE_MIN,                         _temperature_min;
    }
}
RECORD ip_current_extremes
{
    LABEL LBL_620_IP_CURRENT_EXTREMES;
    HELP HLP5(HLP_620_IP_CURRENT_EXTREMES);
    MEMBERS
    {
        IP_CURRENT_MAX,                          _ip_current_max;
        IP_CURRENT_MIN,                          _ip_current_min;
    }
}

RECORD diagnostic_configuration
{
    LABEL LBL_DIAGNOSTIC_CONFIGURATION;
    HELP HLP5(HLP_DIAGNOSTIC_CONFIGURATION);
    MEMBERS
    {
        START_POSITION,          start_position;
        END_POSITION,            end_position;
        /* NUMBER_OF_POINTS_SVI2,   number_of_points; */
        SET_POINT_RATE,          set_point_rate;
        SAMPLING_TIME,           sampling_time;
        DIRECTION,               diag_direction;
        /* INCREMENT,               increment; */
        OPTION,       diag_option;
        /* DIAG_TIME,               diag_time; */
    }
}

/*-------------------------------------------/
** The variables for SVI2 AP FF in the Record
/-------------------------------------------*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~/
*** variables are refined.
/~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if 0
#include "V_Ds.h"
#endif

VARIABLE  _act_effective_area
{
    LABEL LBL_ACT_EFFECTIVE_AREA;
    HELP  HLP5(HLP_ACT_EFFECTIVE_AREA);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (2);
}
VARIABLE  _act_fail_action
{
    LABEL LBL_ACT_FAIL_ACTION;
    HELP  HLP5(HLP_ACT_FAIL_ACTION);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ACT_FAIL_ACTION
    }
}
VARIABLE  _act_man_id
{
    LABEL LBL_ACT_MAN_ID;
    HELP  HLP5(HLP_ACT_MAN_ID);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _act_model_num
{
    LABEL LBL_ACT_MODEL_NUM;
    HELP  HLP5(HLP_ACT_MODEL_NUM);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _act_rotary_moment_arm
{
    LABEL LBL_ACT_ROTARY_MOMENT_ARM;
    HELP  HLP5(HLP_ACT_ROTARY_MOMENT_ARM);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _act_size
{
    LABEL LBL_ACT_SIZE;
    HELP  HLP5(HLP_ACT_SIZE);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _act_sn
{
    LABEL LBL_ACT_SN;
    HELP  HLP5(HLP_ACT_SN);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _act_type
{
    LABEL LBL_ACT_TYPE;
    HELP  HLP5(HLP_ACT_TYPE);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _action
{
    LABEL LBL_ACTION;
    HELP  HLP5(HLP_ACTION);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_CUSTOM_CHAR_ACTION
    }
}
VARIABLE  _actuator_a_max
{
    LABEL LBL_ACTUATOR_A_MAX;
    HELP  HLP5(HLP_ACTUATOR_A_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(-25,1050)
}
VARIABLE  _actuator_a_min
{
    LABEL LBL_ACTUATOR_A_MIN;
    HELP  HLP5(HLP_ACTUATOR_A_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(-25,1050)
}
VARIABLE  _actuator_b_max
{
    LABEL LBL_ACTUATOR_B_MAX;
    HELP  HLP5(HLP_ACTUATOR_B_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(-25,1050)
}
VARIABLE  _actuator_b_min
{
    LABEL LBL_ACTUATOR_B_MIN;
    HELP  HLP5(HLP_ACTUATOR_B_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(-25,1050)
}
_air_supply_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_AIR_SUPPLY_ALERT;
}
VARIABLE  _air_supply_alert
{
    LABEL LBL_AIR_SUPPLY_ALERT;
    HELP  HLP5(HLP_AIR_SUPPLY_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _alert_point_cnt
{
    LABEL LBL_ALERT_POINT_CNT;
    HELP  HLP5(HLP_ALERT_POINT_CNT);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (4);
}
VARIABLE  _alert_point_current_lo
{
    LABEL LBL_ALERT_POINT_CURRENT_LO;
    HELP  HLP5(HLP_ALERT_POINT_CURRENT_LO);
    CLASS CONTAINED;
    TYPE  FLOAT
	DATA_LIMITS(-20,120)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _alert_point_press_hi
{
    LABEL LBL_ALERT_POINT_PRESS_HI;
    HELP  HLP5(HLP_ALERT_POINT_PRESS_HI);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(0,1034.2)

}
VARIABLE  _alert_point_press_lo
{
    LABEL LBL_ALERT_POINT_PRESS_LO;
    HELP  HLP5(HLP_ALERT_POINT_PRESS_LO);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(0,1034.2)
    /* DATA_LIMITS(0,150) */
}
VARIABLE  _alert_point_temp_hi
{
    LABEL LBL_ALERT_POINT_TEMP_HI;
    HELP  HLP5(HLP_ALERT_POINT_TEMP_HI);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-45,85)
    CONSTANT_UNIT [unit_code_1001];
}
VARIABLE  _alert_point_ip_current_hi
{
    LABEL LBL_ALERT_POINT_TEMP_HI;
    HELP  HLP5(HLP_ALERT_POINT_TEMP_HI);
    CLASS CONTAINED;
    TYPE  FLOAT
	DATA_LIMITS(-20,120)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _alert_point_temp_lo
{
    LABEL LBL_ALERT_POINT_TEMP_LO;
    HELP  HLP5(HLP_ALERT_POINT_TEMP_LO);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-45,85)
    CONSTANT_UNIT [unit_code_1001];
}
VARIABLE  _alert_point_time
{
    LABEL LBL_ALERT_POINT_TIME;
    HELP  HLP5(HLP_ALERT_POINT_TIME);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (4);
    CONSTANT_UNIT [unit_code_1059];
}
VARIABLE  _app_r
{
    LABEL LBL_APP;
    HELP  HLP5(HLP_APP);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
    HANDLING READ; /*spec. V15*/
}
VARIABLE  _body_size
{
    LABEL LBL_BODY_SIZE;
    HELP  HLP5(HLP_BODY_SIZE);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (2);
}
VARIABLE  _cal_date
{
    LABEL LBL_CAL_DATE;
    HELP  HLP5(HLP_CAL_DATE);
    CLASS CONTAINED;
    TYPE DATE_AND_TIME;
}
VARIABLE  _cal_location
{
    LABEL LBL_CAL_LOCATION;
    HELP  HLP5(HLP_CAL_LOCATION);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _cal_type
{
    LABEL LBL_CAL_TYPE;
    HELP  HLP5(HLP_CAL_TYPE);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_CAL_TYPE
    }
}
VARIABLE  _cal_type_r
{
    LABEL LBL_CAL_TYPE;
    HELP  HLP5(HLP_CAL_TYPE);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_CAL_TYPE
    }
    HANDLING READ;
}
VARIABLE  _cal_who
{
    LABEL LBL_CAL_WHO;
    HELP  HLP5(HLP_CAL_WHO);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _characteristic
{
    LABEL LBL_CHARACTERISTIC;
    HELP  HLP5(HLP_CHARACTERISTIC);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
_commissioning_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_COMMISSIONING_ALERT;
}
VARIABLE  _commissioning_alert
{
    LABEL LBL_COMMISSIONING_ALERT;
    HELP  HLP5(HLP_COMMISSIONING_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_working_time_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_WORKING_TIME_ALERT;
}
VARIABLE  _working_time_alert
{
    LABEL LBL_WORKING_TIME_ALERT;
    HELP  HLP5(HLP_WORKING_TIME_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _current
{
    LABEL LBL_CURRENT;
    HELP  HLP5(HLP_CURRENT);
    CLASS CONTAINED & DYNAMIC;
    TYPE  FLOAT ;
    CONSTANT_UNIT [unit_code_1342];
    HANDLING READ;
}
VARIABLE  _cycle_counter
{
    LABEL LBL_CYCLE_COUNTER;
    HELP  HLP5(HLP_CYCLE_COUNTER);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (4);
}
_cycle_counter_a_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_CYCLE_COUNTER_A_ALERT;
}
VARIABLE  _cycle_counter_a_alert
{
    LABEL LBL_CYCLE_COUNTER_A_ALERT;
    HELP  HLP5(HLP_CYCLE_COUNTER_A_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_cycle_counter_b_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_CYCLE_COUNTER_B_ALERT;
}
VARIABLE  _cycle_counter_b_alert
{
    LABEL LBL_CYCLE_COUNTER_B_ALERT;
    HELP  HLP5(HLP_CYCLE_COUNTER_B_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _date
{
    LABEL LBL_DATE;
    HELP  HLP5(HLP_DATE);
    CLASS CONTAINED;
    TYPE VISIBLE (8);
}
VARIABLE  _deadband_press
{
    LABEL LBL_DEADBAND_PRESS;
    HELP  HLP5(HLP_DEADBAND_PRESS);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(0,20)
    /* DATA_LIMITS(0,2) */
}
VARIABLE  _deadband_temp
{
    LABEL LBL_DEADBAND_TEMP;
    HELP  HLP5(HLP_DEADBAND_TEMP);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(0,2)
    CONSTANT_UNIT [unit_code_1001];
}
VARIABLE  _decimal_2_r
{
    LABEL LBL_DECIMAL_2;
    HELP  HLP5(HLP_DECIMAL_2);
    CLASS CONTAINED;
    TYPE INTEGER (1);
    HANDLING READ;
}
VARIABLE  _decimal_3
{
    LABEL LBL_DECIMAL_3;
    HELP  HLP5(HLP_DECIMAL_3);
    CLASS CONTAINED;
    TYPE INTEGER (1);
}
VARIABLE  _descriptor
{
    LABEL LBL_DESCRIPTOR;
    HELP  HLP5(HLP_DESCRIPTOR);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _deviation_alert_nd
{
    LABEL LBL_DEVIATION_ALERT;
    HELP  HLP5(HLP_DEVIATION_ALERT);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_ALERT_ACTION
    }
}
VARIABLE  _deviation_alert_r
{
    LABEL LBL_DEVIATION_ALERT;
    HELP  HLP5(HLP_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE ENUMERATED (1)
    {
        ENUM_ALERT_STATE
    }
    HANDLING READ;
}
VARIABLE  _deviation_alert
{
    LABEL LBL_DEVIATION_ALERT;
    HELP  HLP5(HLP_DEVIATION_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _enable_hi
{
    LABEL LBL_ENABLE_HI;
    HELP  HLP5(HLP_ENABLE_HI);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ENABLE_DISABLE
    }
    /* HANDLING READ; */
}
VARIABLE  _enable_lo
{
    LABEL LBL_ENABLE_LO;
    HELP  HLP5(HLP_ENABLE_LO);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ENABLE_DISABLE
    }
    /* HANDLING READ; */
}
VARIABLE  _enable_rate_hi
{
    LABEL LBL_ENABLE_RATE_HI;
    HELP  HLP5(HLP_ENABLE_RATE_HI);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ENABLE_DISABLE
    }
    /* HANDLING READ; */
}
VARIABLE  _enable_rate_lo
{
    LABEL LBL_ENABLE_RATE_LO;
    HELP  HLP5(HLP_ENABLE_RATE_LO);
    CLASS CONTAINED;
    TYPE  ENUMERATED (1)
    {
        ENUM_ENABLE_DISABLE
    }
    /* HANDLING READ; */
}
VARIABLE  _ff_r
{
    LABEL LBL_FF;
    HELP  HLP5(HLP_FF);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
    HANDLING READ;
}
VARIABLE  _final_pos_value_max
{
    LABEL LBL_FINAL_POS_VALUE_MAX;
    HELP  HLP5(HLP_FINAL_POS_VALUE_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-50,199)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _final_pos_value_min
{
    LABEL LBL_FINAL_POS_VALUE_MIN;
    HELP  HLP5(HLP_FINAL_POS_VALUE_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-50,199)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _final_value_max
{
    LABEL LBL_FINAL_VALUE_MAX;
    HELP  HLP5(HLP_FINAL_VALUE_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-50,199)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _final_value_min
{
    LABEL LBL_FINAL_VALUE_MIN;
    HELP  HLP5(HLP_FINAL_VALUE_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-50,199)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _flow_action
{
    LABEL LBL_FLOW_ACTION;
    HELP  HLP5(HLP_FLOW_ACTION);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _hand_wheel
{
    LABEL LBL_HAND_WHEEL;
    HELP  HLP5(HLP_HAND_WHEEL);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
}
VARIABLE  _ip_current_max
{
    LABEL LBL_IP_CURRENT_MAX;
    HELP  HLP5(HLP_IP_CURRENT_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-25,150)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _ip_current_min
{
    LABEL LBL_IP_CURRENT_MIN;
    HELP  HLP5(HLP_IP_CURRENT_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-25,150)
    CONSTANT_UNIT [unit_code_1342];
}
_ip_drive_current_alert_hi_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_IP_DRIVE_CURRENT_ALERT_HI;
}
VARIABLE  _ip_drive_current_alert_hi
{
    LABEL LBL_IP_DRIVE_CURRENT_ALERT_HI;
    HELP  HLP5(HLP_IP_DRIVE_CURRENT_ALERT_HI);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_ip_drive_current_alert_lo_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_IP_DRIVE_CURRENT_ALERT_LO;
}
VARIABLE  _ip_drive_current_alert_lo
{
    LABEL LBL_IP_DRIVE_CURRENT_ALERT_LO;
    HELP  HLP5(HLP_IP_DRIVE_CURRENT_ALERT_LO);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _last_result_r_enum
{
    LABEL LBL_LAST_RESULT;
    HELP  HLP5(HLP_LAST_RESULT);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_LAST_RESULT
    }
    HANDLING READ;
}
VARIABLE  _last_result_r
{
    LABEL LBL_LAST_RESULT;
    HELP  HLP5(HLP_LAST_RESULT);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
    HANDLING READ;
}
VARIABLE  _leakage_class
{
    LABEL LBL_LEAKAGE_CLASS;
    HELP  HLP5(HLP_LEAKAGE_CLASS);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _limit_hi
{
    LABEL LBL_LIMIT_HI;
    HELP  HLP5(HLP_LIMIT_HI);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(10,110)
    /* HANDLING READ; */
}
VARIABLE  _limit_lo
{
    LABEL LBL_LIMIT_LO;
    HELP  HLP5(HLP_LIMIT_LO);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-10,50)
    /* HANDLING READ; */
}
VARIABLE  _limit_rate
{
    LABEL LBL_LIMIT_RATE;
    HELP  HLP5(HLP_LIMIT_RATE);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(0.49,100.1)
    /* HANDLING READ; */
}
VARIABLE  _limits_protected
{
    LABEL LBL_LIMITS_PROTECTED;
    HELP  HLP5(HLP_LIMITS_PROTECTED);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
}
VARIABLE  _lock_level
{
    LABEL LBL_LOCK_LEVEL;
    HELP  HLP5(HLP_LOCK_LEVEL);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
}
VARIABLE  _manufacturer
{
    LABEL LBL_MANUFACTURER;
    HELP  HLP5(HLP_MANUFACTURER);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _message
{
    LABEL LBL_MESSAGE;
    HELP  HLP5(HLP_MESSAGE);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _model
{
    LABEL LBL_MODEL;
    HELP  HLP5(HLP_MODEL);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
_near_close_alert_r  LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_NEAR_CLOSE_ALERT;
}
VARIABLE  _near_close_alert
{
    LABEL LBL_NEAR_CLOSE_ALERT;
    HELP  HLP5(HLP_NEAR_CLOSE_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _near_closed
{
    LABEL LBL_NEAR_CLOSED;
    HELP  HLP5(HLP_NEAR_CLOSED);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (4);
    CONSTANT_UNIT [unit_code_1059];
}
VARIABLE  _number_of_points_custom_char
{
    LABEL LBL_NUMBER_OF_POINTS_CUSTOM_CHAR;
    HELP  HLP5(HLP_NUMBER_OF_POINTS_CUSTOM_CHAR);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1)
    DATA_LIMITS(0,19)    
}
VARIABLE  _packing
{
    LABEL LBL_PACKING;
    HELP  HLP5(HLP_PACKING);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _password
{
    LABEL LBL_PASSWORD;
    HELP  HLP5(HLP_PASSWORD);
    CLASS CONTAINED;
    TYPE INTEGER (2);
}
VARIABLE  _password_enabled
{
    LABEL LBL_PASSWORD_ENABLED;
    HELP  HLP5(HLP_PASSWORD_ENABLED);
    CLASS CONTAINED;
    /* TYPE  BOOLEAN; */
    TYPE  ENUMERATED (1)
    {
         ENUM_ENABLE_DISABLE
    }
    
}
VARIABLE  _percent_complete_r
{
    LABEL LBL_PERCENT_COMPLETE;
    HELP  HLP5(HLP_PERCENT_COMPLETE);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (4);
    HANDLING READ;
}
VARIABLE  _pid_no
{
    LABEL LBL_PID_NO;
    HELP  HLP5(HLP_PID_NO);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
}
VARIABLE  _pilot_max
{
    LABEL LBL_PILOT_MAX;
    HELP  HLP5(HLP_PILOT_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(-25,1050)
}
VARIABLE  _pilot_min
{
    LABEL LBL_PILOT_MIN;
    HELP  HLP5(HLP_PILOT_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(-25,1050)
}
VARIABLE  _plug_type
{
    LABEL LBL_PLUG_TYPE;
    HELP  HLP5(HLP_PLUG_TYPE);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
_position_hi_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_POSITION_HI_ALERT;
}
VARIABLE  _position_hi_alert
{
    LABEL LBL_POSITION_HI_ALERT;
    HELP  HLP5(HLP_POSITION_HI_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_position_hihi_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_POSITION_HIHI_ALERT;
}
VARIABLE  _position_hihi_alert
{
    LABEL LBL_POSITION_HIHI_ALERT;
    HELP  HLP5(HLP_POSITION_HIHI_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_position_lo_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_POSITION_LO_ALERT;
}
VARIABLE  _position_lo_alert
{
    LABEL LBL_POSITION_LO_ALERT;
    HELP  HLP5(HLP_POSITION_LO_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_position_lolo_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_POSITION_LOLO_ALERT;
}
VARIABLE  _position_lolo_alert
{
    LABEL LBL_POSITION_LOLO_ALERT;
    HELP  HLP5(HLP_POSITION_LOLO_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _process_id_r
{
    LABEL LBL_PROCESS_ID;
    HELP  HLP5(HLP_PROCESS_ID);
    CLASS CONTAINED & DYNAMIC;
    TYPE  ENUMERATED (1)
    {
        ENUM_PROCESS_ID
    }
    HANDLING READ;
}
_processor_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_PROCESSOR_ALERT;
}
VARIABLE  _processor_alert_2
{
    LABEL LBL_PROCESSOR_ALERT;
    HELP  HLP5(HLP_PROCESSOR_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _qty
{
    LABEL LBL_QTY;
    HELP  HLP5(HLP_QTY);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _rated_adj_cv
{
    LABEL LBL_RATED_ADJ_CV;
    HELP  HLP5(HLP_RATED_ADJ_CV);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _relay_type
{
    LABEL LBL_RELAY_TYPE;
    HELP  HLP5(HLP_RELAY_TYPE);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_RELAY_TYPE
    }
}
VARIABLE  _same_as_actuator
{
    LABEL LBL_SAME_AS_ACTUATOR;
    HELP  HLP5(HLP_SAME_AS_ACTUATOR);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
}
VARIABLE  _seat_ring_type
{
    LABEL LBL_SEAT_RING_TYPE;
    HELP  HLP5(HLP_SEAT_RING_TYPE);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _selector_r
{
    LABEL LBL_SELECTOR;
    HELP  HLP5(HLP_SELECTOR);
    CLASS CONTAINED;
    TYPE INTEGER (2);
    HANDLING READ;
}

VARIABLE  _uint_selector_r
{
    LABEL LBL_SELECTOR;
    HELP  HLP5(HLP_SELECTOR);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (2);
    HANDLING READ;
}

_sensor_failure_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_SENSOR_FAILURE_ALERT;
}


VARIABLE  _sensor_failure_alert_2
{
    LABEL LBL_SENSOR_FAILURE_ALERT;
    HELP  HLP5(HLP_SENSOR_FAILURE_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _service
{
    LABEL LBL_SERVICE;
    HELP  HLP5(HLP_SERVICE);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
_set_point_timeout_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_SET_POINT_TIMEOUT_ALERT;
}
VARIABLE  _set_point_timeout_alert_2
{
    LABEL LBL_SET_POINT_TIMEOUT_ALERT;
    HELP  HLP5(HLP_SET_POINT_TIMEOUT_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _shutoff_dp
{
    LABEL LBL_SHUTOFF_DP;
    HELP  HLP5(HLP_SHUTOFF_DP);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (2);
}

VARIABLE  _since_reset
{
    LABEL LBL_SINCE_RESET;
    HELP  HLP5(HLP_SINCE_RESET);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (4);
    CONSTANT_UNIT [unit_code_1059];
}
VARIABLE  _solenoid
{
    LABEL LBL_SOLENOID;
    HELP  HLP5(HLP_SOLENOID);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}

VARIABLE  _remote_sensor
{
    LABEL LBL_REMOTE_SENSOR;
    HELP  HLP5(HLP_REMOTE_SENSOR);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_REMOTE_SENSOR
    }
}

VARIABLE  _spec_sheet
{
    LABEL LBL_SPEC_SHEET;
    HELP  HLP5(HLP_SPEC_SHEET);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _stop_hi_pos
{
    LABEL LBL_STOP_HI_POS;
    HELP  HLP5(HLP_STOP_HI_POS);
    CLASS CONTAINED;
    TYPE INTEGER (4);
    /* HANDLING IF ( PARAM.TRAVEL_CALIBRATION.CAL_TYPE == 43 ) { READ & WRITE;} */
    /*     ELSE { READ; } */
}
VARIABLE  _stop_lo_pos
{
    LABEL LBL_STOP_LO_POS;
    HELP  HLP5(HLP_STOP_LO_POS);
    CLASS CONTAINED;
    TYPE INTEGER (4);
    /* HANDLING IF ( PARAM.TRAVEL_CALIBRATION.CAL_TYPE == 43 ) { READ & WRITE;} */
    /*     ELSE { READ; } */

}
_supply_pressure_hi_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_SUPPLY_PRESSURE_HI_ALERT;
}
VARIABLE  _supply_pressure_hi_alert_2
{
    LABEL LBL_SUPPLY_PRESSURE_HI_ALERT;
    HELP  HLP5(HLP_SUPPLY_PRESSURE_HI_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_supply_pressure_lo_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_SUPPLY_PRESSURE_LO_ALERT;
}
VARIABLE  _supply_pressure_lo_alert_2
{
    LABEL LBL_SUPPLY_PRESSURE_LO_ALERT;
    HELP  HLP5(HLP_SUPPLY_PRESSURE_LO_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_supply_pressure_lolo_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_SUPPLY_PRESSURE_LOLO_ALERT;
}
VARIABLE  _supply_pressure_lolo_alert_2
{
    LABEL LBL_SUPPLY_PRESSURE_LOLO_ALERT;
    HELP  HLP5(HLP_SUPPLY_PRESSURE_LOLO_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _supply_pressure_max
{
    LABEL LBL_SUPPLY_PRESSURE_MAX;
    HELP  HLP5(HLP_SUPPLY_PRESSURE_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(-25,1050)
}
VARIABLE  _supply_pressure_min
{
    LABEL LBL_SUPPLY_PRESSURE_MIN;
    HELP  HLP5(HLP_SUPPLY_PRESSURE_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS_FOR_PRESS_UNITS(-25,1050)
}
_supporting_hardware_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_SUPPORTING_HARDWARE_ALERT;
}
VARIABLE  _supporting_hardware_alert_2
{
    LABEL LBL_SUPPORTING_HARDWARE_ALERT;
    HELP  HLP5(HLP_SUPPORTING_HARDWARE_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_temperature_hi_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_TEMPERATURE_HI_ALERT;
}
VARIABLE  _temperature_hi_alert_2
{
    LABEL LBL_TEMPERATURE_HI_ALERT;
    HELP  HLP5(HLP_TEMPERATURE_HI_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_temperature_lo_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_TEMPERATURE_LO_ALERT;
}
VARIABLE  _temperature_lo_alert_2
{
    LABEL LBL_TEMPERATURE_LO_ALERT;
    HELP  HLP5(HLP_TEMPERATURE_LO_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _temperature_max
{
    LABEL LBL_TEMPERATURE_MAX;
    HELP  HLP5(HLP_TEMPERATURE_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-40,85)
    CONSTANT_UNIT [unit_code_1001];
}
VARIABLE  _temperature_min
{
    LABEL LBL_TEMPERATURE_MIN;
    HELP  HLP5(HLP_TEMPERATURE_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-40,85)
    CONSTANT_UNIT [unit_code_1001];
}
VARIABLE  _time_uint_4
{
    LABEL LBL_TIME;
    HELP  HLP5(HLP_TIME);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (4)
    DATA_LIMITS(0,3600)
    CONSTANT_UNIT [unit_code_1054];
}
VARIABLE  _time_float
{
    LABEL LBL_TIME;
    HELP  HLP5(HLP_TIME);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(1,60)
    CONSTANT_UNIT [unit_code_1054];
}
VARIABLE  _total_time_r
{
    LABEL LBL_TOTAL_TIME;
    HELP  HLP5(HLP_TOTAL_TIME);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (4);
    HANDLING READ;
    CONSTANT_UNIT [unit_code_1059];
}
_alert_total_time_r LIKE VARIABLE _total_time_r
{
    REDEFINE LABEL LBL_ALERT_TOTAL_TIME;
    REDEFINE HELP  HLP5(HLP_ALERT_TOTAL_TIME);
}

_travel_accumulation_a_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_TRAVEL_ACCUMULATION_A_ALERT;
}
VARIABLE  _travel_accumulation_a_alert_2
{
    LABEL LBL_TRAVEL_ACCUMULATION_A_ALERT;
    HELP  HLP5(HLP_TRAVEL_ACCUMULATION_A_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
_travel_accumulation_b_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_TRAVEL_ACCUMULATION_B_ALERT;
}
VARIABLE  _travel_accumulation_b_alert_2
{
    LABEL LBL_TRAVEL_ACCUMULATION_B_ALERT;
    HELP  HLP5(HLP_TRAVEL_ACCUMULATION_B_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _units_index_percent_r
{
    LABEL LBL_UNITS_INDEX_PERCENT;
    HELP  HLP5(HLP_UNITS_INDEX_PERCENT);
    CLASS CONTAINED;
    TYPE ENUMERATED (2)
    {
        ENUM_UNITS_INDEX_1342
    }
    HANDLING READ;
}
VARIABLE  _units_index_press
{
    LABEL LBL_UNITS_INDEX_PRESS;
    HELP  HLP5(HLP_UNITS_INDEX_PRESS);
    CLASS CONTAINED;
    TYPE ENUMERATED (2)
    {
        ENUM_UNITS_INDEX_PRESS
    }
}
VARIABLE  _units_index_travel
{
    LABEL LBL_UNITS_INDEX_TRAVEL;
    HELP  HLP5(HLP_UNITS_INDEX_TRAVEL);
    CLASS CONTAINED;
    TYPE ENUMERATED (2)
    {
        ENUM_UNITS_INDEX_TRAVEL_RANGE
    }
}
VARIABLE  _value_ip_r
{
    LABEL LBL_VALUE_IP;
    HELP  HLP5(HLP_VALUE_IP);
    CLASS CONTAINED & DYNAMIC;
    TYPE FLOAT;
    HANDLING READ;
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _value_open_close_r
{
    LABEL LBL_VALUE_OPEN_CLOSE;
    HELP  HLP5(HLP_VALUE_OPEN_CLOSE);
    CLASS CONTAINED & DYNAMIC;
    TYPE FLOAT
    DATA_LIMITS(-50,199)
    HANDLING        READ ;
}
_valve_control_alert_r LIKE VARIABLE _deviation_alert_r
{
    REDEFINE LABEL LBL_VALVE_CONTROL_ALERT;
}
VARIABLE  _valve_control_alert_2
{
    LABEL LBL_VALVE_CONTROL_ALERT;
    HELP  HLP5(HLP_VALVE_CONTROL_ALERT);
    CLASS CONTAINED & DYNAMIC;
    TYPE UNSIGNED_INTEGER (2)
    DATA_LIMITS(0,0xFFFF)
}
VARIABLE  _valve_man_id
{
    LABEL LBL_VALVE_MAN_ID;
    HELP  HLP5(HLP_VALVE_MAN_ID);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _valve_model_num
{
    LABEL LBL_VALVE_MODEL_NUM;
    HELP  HLP5(HLP_VALVE_MODEL_NUM);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _valve_sn
{
    LABEL LBL_VALVE_SN;
    HELP  HLP5(HLP_VALVE_SN);
    CLASS CONTAINED;
    TYPE VISIBLE (32);
}
VARIABLE  _valve_type
{
    LABEL LBL_VALVE_TYPE;
    HELP  HLP5(HLP_VALVE_TYPE);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_VALVE_TYPE
    }
}
VARIABLE  _working_pos_max
{
    LABEL LBL_WORKING_POS_MAX;
    HELP  HLP5(HLP_WORKING_POS_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-50,199)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _working_pos_min
{
    LABEL LBL_WORKING_POS_MIN;
    HELP  HLP5(HLP_WORKING_POS_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-50,199)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _working_sp_max
{
    LABEL LBL_WORKING_SP_MAX;
    HELP  HLP5(HLP_WORKING_SP_MAX);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-50,199)
    CONSTANT_UNIT [unit_code_1342];
}
VARIABLE  _working_sp_min
{
    LABEL LBL_WORKING_SP_MIN;
    HELP  HLP5(HLP_WORKING_SP_MIN);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(-50,199)
    CONSTANT_UNIT [unit_code_1342];
}


position_hihi_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_455_POSITION_HIHI_ALERT;
}
position_hi_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_456_POSITION_HI_ALERT;
}
position_lo_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_457_POSITION_LO_ALERT;
}
position_lolo_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_458_POSITION_LOLO_ALERT;
}
set_point_timeout_alert LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_459_SET_POINT_TIMEOUT_ALERT;
}
near_close_alert LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_460_NEAR_CLOSE_ALERT;
}
travel_accumulation_a_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_461_TRAVEL_ACCUMULATION_A_ALERT;
}
travel_accumulation_b_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_462_TRAVEL_ACCUMULATION_B_ALERT;
}
cycle_counter_a_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_463_CYCLE_COUNTER_A_ALERT;
}
cycle_counter_b_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_464_CYCLE_COUNTER_B_ALERT;
}
working_time_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_465_WORKING_TIME_ALERT;
}
supply_pressure_hi_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_466_SUPPLY_PRESSURE_HI_ALERT;
}
supply_pressure_lo_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_467_SUPPLY_PRESSURE_LO_ALERT;
}
supply_pressure_lolo_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_468_SUPPLY_PRESSURE_LOLO_ALERT;
}
temperature_hi_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_469_TEMPERATURE_HI_ALERT;
}
temperature_lo_alert_act LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_470_TEMPERATURE_LO_ALERT;
}
ip_drive_current_alert_hi LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_471_IP_DRIVE_CURRENT_ALERT_HI;
}
ip_drive_current_alert_lo LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_472_IP_DRIVE_CURRENT_ALERT_LO;
}
sensor_failure_alert LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_473_SENSOR_FAILURE_ALERT;
    REDEFINE HANDLING READ;
}
processor_alert LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_474_PROCESSOR_ALERT;
    REDEFINE HANDLING READ;
}
valve_control_alert LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_475_VALVE_CONTROL_ALERT;
    REDEFINE HANDLING READ;
}
commissioning_alert LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_476_COMMISSIONING_ALERT;
    REDEFINE HANDLING READ;
}
air_supply_alert LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_477_AIR_SUPPLY_ALERT;
    REDEFINE HANDLING READ;
}
supporting_hardware_alert LIKE VARIABLE _deviation_alert_nd
{
    REDEFINE LABEL LBL_478_SUPPORTING_HARDWARE_ALERT;
    REDEFINE HANDLING READ;
}
cutoff_point_hi LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS_ONLY_MIN(FLOAT, 80)
}

cutoff_point_lo LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE HELP HLP5(HLP_FLOAT_VALUE);
    REDEFINE_DATA_LIMITS_ONLY_MAX(FLOAT, 20)
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
*** variables needed to be refined
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

VARIABLE raw_position
{
    LABEL LBL_RAW_POSITION;
    HELP  HLP5(HLP_RAW_POSITION);
    CLASS CONTAINED & DYNAMIC;
    TYPE  INTEGER (4);
    HANDLING READ;
}

VARIABLE position_transducer_type
{
    LABEL LBL_043_POSITION_TRANSDUCER_TYPE;
    HELP  HLP5(HLP_043_POSITION_TRANSDUCER_TYPE);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (2);
    HANDLING READ;
}
VARIABLE xd_error_pos
{
    LABEL LBL_044_XD_ERROR_POS;
    HELP  HLP5(HLP_044_XD_ERROR_POS);
    CLASS CONTAINED & DYNAMIC;
    TYPE ENUMERATED (1)
    {
        ENUM_XD_ERROR_POS
    }
    HANDLING READ;
}

VARIABLE activate_control_set
{
    LABEL LBL_071_ACTIVATE_CONTROL_SET;
    HELP  HLP5(HLP_071_ACTIVATE_CONTROL_SET);
    CLASS CONTAINED & DYNAMIC;
    TYPE ENUMERATED (1)
    {
        ENUM_ACTIVATE_CONTROL_SET
    }
    /*VALIDITY  Auto    ;*/
}

VARIABLE position_history_total_time
{
    LABEL LBL_236_TOTAL;
    HELP  HLP5(HLP_236_TOTAL);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (4);
    CONSTANT_UNIT [unit_code_1054];
}

ARRAY current_char_points
{
    LABEL LBL_CURRENT_CHAR_POINTS;
    HELP  HLP5(HLP_CURRENT_CHAR_POINTS);
    TYPE int_2_element_r;
    NUMBER_OF_ELEMENTS NBRS_CHAR_ARRAY;
}
ARRAY custom_char_points
{
    LABEL LBL_287_CUSTOM_CHAR_POINTS;
    HELP HLP5(HLP_287_CUSTOM_CHAR_POINTS);
    TYPE int_2_element;
    NUMBER_OF_ELEMENTS NBRS_CHAR_ARRAY;
}
ARRAY advanced
{
    LABEL LBL_ADVANCED;
    HELP HLP5(HLP_ADVANCED);
    TYPE uint_4_element_nd;
    NUMBER_OF_ELEMENTS 10;
}

VARIABLE readback_select
{
    LABEL LBL_288_READBACK_SELECT;
    HELP  HLP5(HLP_288_READBACK_SELECT);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_READBACK_SELECT
    }
}
VARIABLE transducer_type
{
    LABEL LBL_289_TRANSDUCER_TYPE;
    HELP  HLP5(HLP_289_TRANSDUCER_TYPE);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (2);
    HANDLING READ;
}
VARIABLE xd_error_pressure
{
    LABEL LBL_290_XD_ERROR_PRESSURE;
    HELP  HLP5(HLP_290_XD_ERROR_PRESSURE);
    CLASS CONTAINED & DYNAMIC;
    TYPE ENUMERATED (2)
    {
        ENUM_IO_FAILURE
    }
    
    HANDLING READ;
}
VARIABLE set_to_factory_pr_calibration
{
    LABEL LBL_338_SET_TO_FACTORY_PR_CALIBRATION;
    HELP  HLP5(HLP_338_SET_TO_FACTORY_PR_CALIBRATION);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1)
    DATA_LIMITS(1,88)
    /*VALIDITY  Man ;*/
}
VARIABLE temp_transducer_type
{
    LABEL LBL_339_TEMP_TRANSDUCER_TYPE;
    HELP  HLP5(HLP_339_TEMP_TRANSDUCER_TYPE);
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (2);
    HANDLING READ;
}
VARIABLE xd_error_temperature
{
    LABEL LBL_340_XD_ERROR_TEMPERATURE;
    HELP  HLP5(HLP_340_XD_ERROR_TEMPERATURE);
    CLASS CONTAINED & DYNAMIC;
    TYPE  ENUMERATED (2)
    {
        ENUM_IO_FAILURE
    }
    
    HANDLING READ;
}
VARIABLE find_stops
{
    LABEL LBL_375_FIND_STOPS;
    HELP  HLP5(HLP_375_FIND_STOPS);
    CLASS CONTAINED & DYNAMIC;
    TYPE ENUMERATED (1)
    {
        ENUM_FIND_STOPS
    }
    /* HANDLING IF ( PARAM.MODE_BLK.ACTUAL == _MANUAL || PARAM.MODE_BLK.ACTUAL == _LOCKED ) { READ & WRITE;} */
    /*     ELSE { READ; } */
}

VARIABLE offline_diagnostic
{
    LABEL LBL_OFFLINE_DIAGNOSTIC;
    HELP  HLP5(HLP_OFFLINE_DIAGNOSTIC);
    CLASS CONTAINED & DYNAMIC;
    TYPE ENUMERATED (1)
    {
        ENUM_OFFLINE_DIAGNOSTIC
    }
    /*VALIDITY  OOS ;*/
}

start_position LIKE VARIABLE __float_contained_nd
{
    REDEFINE LABEL LBL_START_POSITION;
    REDEFINE HELP  HLP5(HLP_START_POSITION);
}

end_position LIKE VARIABLE __float_contained_nd
{
    REDEFINE LABEL LBL_END_POSITION;
    REDEFINE HELP  HLP5(HLP_END_POSITION);
}

set_point_rate LIKE VARIABLE __float_contained_nd
{
    REDEFINE LABEL LBL_SET_POINT_RATE;
    REDEFINE HELP  HLP5(HLP_SET_POINT_RATE);
    REDEFINE CONSTANT_UNIT [unit_code_1713];
}
sampling_time LIKE VARIABLE uint_2_element
{
    REDEFINE LABEL LBL_SAMPLING_TIME;
    REDEFINE HELP  HLP5(HLP_SAMPLING_TIME);
    REDEFINE CONSTANT_UNIT [unit_code_1054];
}

VARIABLE diag_direction
{
    LABEL "|en|Direction";
    HELP  HLP5(HLP_DIAG_DIRECTION);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_DIAG_DIRECTION
    }
}

VARIABLE diag_option
{
    LABEL "|en|Diagnostic Option";
    HELP  HLP5(HLP_DIAG_OPTION);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_DIAG_OPTION
    }
}

ARRAY diagnostic_data
{
    LABEL LBL_DIAGNOSTIC_DATA;
    HELP HLP5(HLP_DIAGNOSTIC_DATA);
    TYPE uint_2_element_d;
    NUMBER_OF_ELEMENTS 27;
}

VARIABLE custom_1_configuration
{
    LABEL LBL_CUSTOM_1_CONFIGURATION;
    HELP  HLP5(HLP_CUSTOM_1_CONFIGURATION);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_UI_CUSTOM_CONFIGURATION
    }

}
VARIABLE custom_2_configuration
{
    LABEL LBL_CUSTOM_2_CONFIGURATION;
    HELP  HLP5(HLP_CUSTOM_2_CONFIGURATION);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_UI_CUSTOM_CONFIGURATION
    }
}
VARIABLE ui_language
{
    LABEL LBL_436_UI_LANGUAGE;
    HELP  HLP5(HLP_436_UI_LANGUAGE);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_UI_LANGUAGE
    }
}
VARIABLE app_mode
{
    LABEL LBL_437_APP_MODE;
    HELP  HLP5(HLP_437_APP_MODE);
    CLASS CONTAINED & DYNAMIC;
    TYPE ENUMERATED (1)
    {
        ENUM_APP_MODE
    }
}
RECORD complete_status
{
    LABEL LBL_438_COMPLETE_STATUS;
    HELP HLP5(HLP_438_COMPLETE_STATUS);
    MEMBERS
    {
        DEVICE_STATUS;
    }
}
VARIABLE clear_status
{
    LABEL LBL_439_CLEAR_STATUS;
    HELP  HLP5(HLP_439_CLEAR_STATUS);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_CLEAR_STATUS
    }
}
VARIABLE open_stop_adjustment
{
    LABEL LBL_440_OPEN_STOP_ADJUSTMENT;
    HELP  HLP5(HLP_440_OPEN_STOP_ADJUSTMENT);
    CLASS CONTAINED;
    TYPE FLOAT
    DATA_LIMITS(60.00,100.00)
}
VARIABLE setpoint_source
{
    LABEL LBL_441_SETPOINT_SOURCE;
    HELP  HLP5(HLP_441_SETPOINT_SOURCE);
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
    {
        ENUM_SETPOINT_SOURCE
    }
}

_fstate_value LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_FSTATE_VALUE;
    REDEFINE_DATA_LIMITS(FLOAT, -50, 160)
}

VARIABLE _fstate_time
{
    LABEL LBL_FSTATE_TIME;
    HELP  HLP5(HLP_279_FSTATE_TIME);
    CLASS CONTAINED;
    TYPE  FLOAT
	{
		MIN_VALUE 1.0;
		MAX_VALUE 100000.0; /* #15873 */
	}
}

_supply_pres_max LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_560_SUPPLY_PRS_MAX;
    REDEFINE HELP  HLP_560_SUPPLY_PRS_MAX;
	REDEFINE_DATA_LIMITS_FOR_PRESS_UNITS(FLOAT, 0, 241.3)
}
_prs_control_hi LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_561_PRS_CONTROL_HI;
    REDEFINE HELP  HLP_561_PRS_CONTROL_HI;
	REDEFINE_DATA_LIMITS_FOR_PRESS_UNITS(FLOAT, 0, 103.4)
}
_prs_control_lo  LIKE VARIABLE float_nd_with_max_min
{
    REDEFINE LABEL LBL_562_PRS_CONTROL_LO;
    REDEFINE HELP  HLP_562_PRS_CONTROL_LO;
	REDEFINE_DATA_LIMITS_FOR_PRESS_UNITS(FLOAT, 0, 20.6)
}

ARRAY factory_use_1
{
    LABEL LBL_541_FACTORY_USE_1;
    HELP HLP5(HLP_541_FACTORY_USE_1);
    TYPE uint_element;
    NUMBER_OF_ELEMENTS 58;
}
ARRAY factory_use_2
{
    LABEL LBL_542_FACTORY_USE_2;
    HELP HLP5(HLP_542_FACTORY_USE_2);
    TYPE uint_element_r;
    NUMBER_OF_ELEMENTS 58;
}
/*--------------------------------------/
** Defines all method for TB_METHOD_LIST
/--------------------------------------*/
METHOD do_test_wr_limits
{
    LABEL "|en|Test write Limits";
    HELP HLP5("|en|Enable/Disable Position Limits");
    CLASS INPUT;
    DEFINITION
    {
        COMMON_LOCAL_VARS;
        unsigned long old_value, new_value;
        int           ivalue;
        int           sel;
        float         flt_buf;
        long          chk_cnt;

        unsigned long us8_hi;
        unsigned long us8_lo;

        /* check mode, if OOS or MAN? */

        id = ITEM_ID(PARAM.POSITION_LIMITS);
        mb = 0;
        status=read_value(id,mb);

        mb = MEMBER_ID(ENABLE_HI);
        status = put_unsigned_value(id, mb, 1);
        mb = MEMBER_ID(ENABLE_LO);
        status = put_unsigned_value(id, mb, 1);
        status = select_from_menu("|en|Select the way to send data:\n", dummy, dummy, 0, \
                                  "|en|send by record;" \
                                  "send by member\n", &ivalue);
        switch (ivalue){
        case 1:
            mb = 0;
            status = send_value(id, mb);
            break;
        case 2:    
            mb = MEMBER_ID(ENABLE_HI);
            status = send_value(id, mb);
            mb = MEMBER_ID(ENABLE_LO);
            status = send_value(id, mb);
            break;
        }
        mb = 0;
        status = read_value(id, mb);
        METHOD_FINISHED_MACRO
   }
}

METHOD  do_find_stops
{
    LABEL "|en|Auto Find Stops";
    HELP "|en|Run Find Stops";
    CLASS INPUT;
    DEFINITION
    {
        COMMON_LOCAL_VARS;

        int           ivalue;
        int           sel;
        float         flt_buf;
        long          chk_cnt;
        unsigned long func;
        unsigned long ulCal_Type;

        unsigned long trl_ids[7],trl_mbs[7];

        char strPress_Unit[5];
        unsigned long press_unit_index;

        unsigned long act_style;

        float fSupply_press_max, fSupply_press_min;
        float fOpenStopAdj;

        char strLocation[33];
        char strWho[33];
        char strCal_Date[33];

        CHECK_DEVICE_IN_PROCESS;

        add_abort_method(ITEM_ID(cancel_find_stops));

        CHECK_DEVICE_IN_MAN;

        get_acknowledgement(TEXT_STROKE_VALVE, ids, dummy, 0);        


        /* set the default travel parameters */
        /*     ·   Travel = 100 */
        /*     ·   Travel Units = % */
        /*     ·   Open Stop Adjustment = 100 % */


        ids[0] = ITEM_ID(PARAM.TRAVEL);
        mbs[0] = MEMBER_ID(RANGE);

        ids[1] = ids[0];
        mbs[1] = MEMBER_ID(UNITS_INDEX);

        ids[2] = ITEM_ID(PARAM.OPEN_STOP_ADJUSTMENT);
        mbs[2] = 0;

        ids[3] = ITEM_ID(PARAM.FIND_STOPS);
        mbs[3] = 0;

        ids[4] = ITEM_ID(PARAM.ACTUATOR_A_PRESSURE);
        mbs[4] = MEMBER_ID(VALUE);

        ids[5] = ITEM_ID(PARAM.ACTUATOR_B_PRESSURE);
        mbs[5] = MEMBER_ID(VALUE);

        ids[6] = ITEM_ID(PARAM.RAW_POSITION);
        mbs[6] = 0;

        ids[7] = ITEM_ID(PARAM.ACTUAL_POSITION);
        mbs[7] = MEMBER_ID(VALUE);

        ids[8] = ITEM_ID(PARAM.SUPPLY_PRESSURE);
        mbs[8] = MEMBER_ID(VALUE);

        ids[9] = ITEM_ID(PARAM.PRESSURE_EXTREMES);
        mbs[9] = 0;

        ids[10] = ITEM_ID(PARAM.PRESSURE_RANGE);
        mbs[10] = MEMBER_ID(UNITS_INDEX);

        ids[11] = ITEM_ID(PARAM.ACTUATOR_3);
        mbs[11] = MEMBER_ID(ACT_STYLE);

        /* clear status */
        id = ITEM_ID(PARAM.CLEAR_STATUS);
        mb = 0;
        status = put_unsigned_value(id, 0, 1);
        SEND_PARAM(id, 0, "CLEAR_STATUS");

        status = put_float_value(ids[0], mbs[0], 100.0); /* travel.range */
        status = put_unsigned_value(ids[1], mbs[1],1342); /* travel.unit */

        fOpenStopAdj = 100;
        status = put_float_value(ids[2], 0, fOpenStopAdj); /* Open stops adjustment */

        status = get_unsigned_value(ids[10], mbs[10], &press_unit_index);
        GET_ENUM_STRING(ids[10], mbs[10], strPress_Unit, 5);

        status = get_unsigned_value(ids[11], mbs[11], &act_style);
        act_style = 0;

        trl_ids[0]=trl_ids[1]=trl_ids[2]=trl_ids[3]=trl_ids[4]=trl_ids[5]=trl_ids[6]=ITEM_ID(PARAM.TRAVEL_CALIBRATION);
        trl_mbs[0]=MEMBER_ID(CAL_LOCATION);
        trl_mbs[1]=MEMBER_ID(CAL_DATE);
        trl_mbs[2]=MEMBER_ID(CAL_WHO);
        trl_mbs[3]=MEMBER_ID(CAL_STOP_HI_POS);
        trl_mbs[4]=MEMBER_ID(CAL_STOP_LO_POS);
        trl_mbs[5]=MEMBER_ID(CAL_TYPE);
        trl_mbs[6]=MEMBER_ID(LAST_RESULT);

        /* GET_ENUM_STRING(1); */

        status = select_from_menu("|en|\nDefault Travel Setting:\n"           \
                                  "  TRAVEL.RANGE = %[5.2f]{0}\n" \
                                  "  TRAVEL.UNITS_INDEX = %{1}\n" \
                                  "Do you want to change the setting?\n",
                                  ids, mbs, 9, "Yes;No(skip)", &ivalue);

        if (ivalue == 1)
        {
            status = edit_device_value("|en|\n TRAVEL.RANGE(%[5.2f]{0})\n",
                                       ids, mbs, 9, ids[0], mbs[0]);
            status = edit_device_value("|en|\n TRAVEL.UNIT(%{1})\n",
                                       ids, mbs, 9, ids[1], mbs[1]);
        }


        /* The software shall read parameter PRESSURE_EXTREMES and preserve the */
        /* values for SUPPLY_PRESSURE_MAX and SUPPLY_PRESSURE_MIN. */

        /* The software shall set the PRESSURE_EXTREMES.SUPPLY_PRESSURE_MAX to */
        /* -9999.99 and PRESSURE_EXTREMES.SUPPLY_PRESSURE_MIN to 9999.99. This will */
        /* ensure that the values will be updated with the minimum and maximum */
        /* value measured during the procedure. */

        READ_PARAM(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), "PRESSURE_EXTREMES.SUPPLY_PRESSURE_MAX");
        READ_PARAM(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), "PRESSURE_EXTREMES.SUPPLY_PRESSURE_MIN");

        status = get_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), &fSupply_press_max);
        status = get_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), &fSupply_press_min);
        status = put_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), -9999.99);
        status = put_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), 9999.99);

        sel = func = FINDSTOPS_START;
        ulCal_Type = CAL_TYPE_BOTH_ENDPOINTS_AUTOMATIC;

        status = select_from_menu("|en|\nDefault Open Stops Adjustment:\n" \
                                  "OPEN_STOP_ADJUSTMENT = %[5.2f]{2}%%\n\n" \
                                  "Do you want to change the setting?", \
                                  ids, mbs, 9, "Yes;No(skip)", &ivalue);
        if (ivalue == 1)
        {
            status = edit_device_value("|en|\n OPEN_STOP_ADJUSTMENT(%[5.2f]{2})\n",
                                       ids, mbs, 9, ids[2], mbs[2]);
            SEND_PARAM(ids[2], mbs[2], "OPEN_STOP_ADJUSTMENT");
            status = get_float_value(ids[2], mbs[2], &fOpenStopAdj);
        }

        send_all_values();

        status = put_unsigned_value(ids[3], mbs[3], func);

        GET_ENUM_STRING(ids[3], mbs[3], str_buf, 256);

        SEND_PARAM(ids[3], 0, "FIND_STOPS");
        do 
        {
            /* The software shall detect that the procedure is completed, when
             * the value of FIND_STOPS is switched from 11 (Find stops Running)
             * to 0(Find stops Not Running). */
#if 0
            if (SVI2_DBL_ACT == act_style )
            {
                display_dynamics("|en|\nRun %{str_buf}:\n\n"    \
                                 "  FIND STOPS = %{3}\n" \
                                 "  ACTUATOR A PRESSURE = %[7.2f]{4} %{strPress_Unit}\n" \
                                 "  ACTUATOR B PRESSURE = %[7.2f]{5} %{strPress_Unit}\n" \
                                 "  SUPPLY PRESSURE = %[7.2f]{8} %{strPress_Unit}\n"        \
                                 "  RAW POSITION = %[8i]{6}\n"          \
                                 "  ACTUAL POSITION = %[7.2f]{7}\n",    \
                                 ids, mbs, 9);
            }
            else if (SVI2_SNGL_ACT == act_style)
            {
                display_dynamics("|en|\nRun %{str_buf}:\n\n"    \
                                 "  FIND STOPS = %{3}\n" \
                                 "  ACTUATOR PRESSURE = %[7.2f]{4} %{strPress_Unit}\n" \
                                 "  SUPPLY PRESSURE = %[7.2f]{8} %{strPress_Unit}\n"        \
                                 "  RAW POSITION = %[8i]{6}\n"          \
                                 "  ACTUAL POSITION = %[7.2f]{7}\n",    \
                                 ids, mbs, 9);
            }
#endif
            /* delayfor( 3, "|en|Find stops......", dummy, dummy, 0); */
            delayfor(3, "|en|\nRun %{str_buf}:\n\n"       \
                                 "  FIND STOPS = %{3}\n" \
                                 "  ACTUATOR PRESSURE = %[7.2f]{4} %{strPress_Unit}\n" \
                                 "  SUPPLY PRESSURE = %[7.2f]{8} %{strPress_Unit}\n"        \
                                 "  RAW POSITION = %[8i]{6}\n"          \
                                 "  ACTUAL POSITION = %[7.2f]{7}\n",    \
                                 ids, mbs, 9);
            
            READ_PARAM(ids[3], 0, "FIND_STOPS");
            status = get_unsigned_value(ids[3], 0, &us8_buf);
            
        } while(us8_buf == FINDSTOPS_START || us8_buf == FINDSTOPS_RUNNING ) /* Continue */;

        /* report find stops success/faild */
        id = ITEM_ID(PARAM.COMPLETE_STATUS);
        mb = MEMBER_ID(CURRENT_STATUS_1_C);
        READ_PARAM(id, mb, "COMPLETE_STATUS.CURRENT_STATUS_1_C");
        status = get_unsigned_value(id, mb, &us8_buf);
        /* IF_ERROR_ABORT_MACRO_INFO("READ_STATUS:"); */
        if ( 0 != (us8_buf & FBIT1 )) {
            get_acknowledgement("|en|Find stops failed.\n",dummy,dummy,0);
            METHOD_ABORT_MACRO;
        }

        /* if ( 1|| sel == FINDSTOPS_MANUL_LO_HI ) */
        {
            status = put_unsigned_value( trl_ids[5], trl_mbs[5], ulCal_Type);
            SEND_PARAM(trl_ids[5], trl_mbs[5], "TRAVEL_CALIBRATION.CAL_TYPE");

            READ_PARAM(trl_ids[0], trl_mbs[0], "TRAVEL_CALIBRATION.CAL_LOCATION");
            READ_PARAM(trl_ids[1], trl_mbs[1], "TRAVEL_CALIBRATION.CAL_DATE");
            READ_PARAM(trl_ids[2], trl_mbs[2], "TRAVEL_CALIBRATION.CAL_WHO");
            READ_PARAM(trl_ids[3], trl_mbs[3], "TRAVEL_CALIBRATION.CAL_STOP_HI_POS");
            READ_PARAM(trl_ids[4], trl_mbs[4], "TRAVEL_CALIBRATION.CAL_STOP_LO_POS");

            READ_PARAM(trl_ids[5], trl_mbs[4], "TRAVEL_CALIBRATION.CAL_TYPE");
            READ_PARAM(trl_ids[6], trl_mbs[6], "TRAVEL_CALIBRATION.LAST_RESULT");

            maxlen = 33;
            status = get_string_value(trl_ids[0], trl_mbs[0], strLocation, &maxlen);
            maxlen = 33;
            status = get_string_value(trl_ids[2], trl_mbs[2], strWho, &maxlen);
            maxlen = 33;
            status = get_date_value(trl_ids[1], trl_mbs[1], strCal_Date, &maxlen);
                
            /*display the travel calibration restult*/
            display_message("|en|\nTravel Calibration:\n"  \
                            "  CAL_LOCATION:%[s]{strLocation}\n"  \
                            "  CAL_DATe: %{1}\n"  \
                            "  CAL_WHO:%[s]{strWho}\n"  \
                            "  CAL_STOP_HI_POS:%[d]{3}\n"  \
                            "  CAL_STOP_LO_POS:%[d]{4}\n"  \
                            "  CAL_TYPE:%{5}\n"  \
                            "  LAST_RESULT:%{6}\n", trl_ids, trl_mbs, 7);

            status = select_from_menu("\nDo you wish to change the values", dummy, dummy, 0, "Yes;Skip", &ivalue);
            if ( ivalue == 1)
            {
                edit_device_value("|en|Enter the new value for CAL_LOCATION:\n",
                                  trl_ids, trl_mbs, 7, trl_ids[0], trl_mbs[0]);
                edit_device_value("|en|Enter the new value for CAL_DATE:\n",
                                  trl_ids, trl_mbs, 7, trl_ids[1], trl_mbs[1]);
                edit_device_value("|en|Enter the new value for CAL_WHO:\n",
                                  trl_ids, trl_mbs, 7, trl_ids[2], trl_mbs[2]);
                /* send_all_values(); */
            }
            send_all_values();

        }


        /* At the end of Manual Low Stop Limit procedure, the software shall */
        /* recommend to the user to run Manual High Stop Limit procedure. */
        /* At the end of Manual High Stop Limit procedure, the software shall */
        /* recommend to the user to run Manual Low Stop Limit procedure. */


        /*restore the saved supply_pressure_extremes*/
        status = put_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), fSupply_press_max);
        SEND_PARAM(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), "SUPPLY_PRESSURE_EXTREMES");

        status = put_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), fSupply_press_min);
        SEND_PARAM(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), "SUPPLY_PRESSURE_EXTREMES");
        
        status = put_float_value(ITEM_ID(PARAM.OPEN_STOP_ADJUSTMENT), 0, 100.0);
        SEND_PARAM(ITEM_ID(PARAM.OPEN_STOP_ADJUSTMENT), MEMBER_ID(SUPPLY_PRESSURE_MIN), "OPEN_STOP_ADJUSTMENT");

        display_message("|en|Find stops finished.", dummy, dummy, 0);
        display_message("|en|The Auto Stops Adjustment was set to 100%%", dummy, dummy, 0);

        return;
    }
}

METHOD do_autotune
{
    LABEL "|en|Auto tune";
    HELP "|en|Run the auto tune";
    CLASS INPUT;
    DEFINITION
    {
        COMMON_LOCAL_VARS;

        unsigned long  old_value, new_value;
        int            ivalue;
        int            sel;
        float          flt_buf;
        long           chk_cnt;

        CHECK_DEVICE_IN_PROCESS;
        

        /* check mode, if OOS or MAN? */
        id = ITEM_ID(PARAM.MODE_BLK);
        mb = MEMBER_ID(ACTUAL);
        READ_PARAM(id, mb, "MODE_BLK.ACTUAL");
        status = get_unsigned_value(id, mb, &us8_buf);

        if ( us8_buf != _MANUAL ) { /* TB spec. V37 */
            get_acknowledgement("|en|MODE_BLK(Actual) is not in MAN mode.\n"
                                "Please set to the MAN mode.\n",dummy,dummy,0);
            METHOD_ABORT_MACRO;
        }

        /* check for the limits were set and be protected */
        id = ITEM_ID(PARAM.POSITION_LIMITS);
        READ_PARAM(id, MEMBER_ID(LIMITS_PROTECTED),"POSITION_LIMITS.LIMITS_PROTECTED");
        READ_PARAM(id, MEMBER_ID(ENABLE_HI), "POSITION_LIMITS.ENABLE_HI");
        READ_PARAM(id, MEMBER_ID(ENABLE_LO), "POSITION_LIMITS.ENABLE_LO");
        READ_PARAM(id, MEMBER_ID(ENABLE_RATE_HI), "POSITION_LIMITS.ENABLE_RATE_HI");
        READ_PARAM(id, MEMBER_ID(ENABLE_RATE_LO), "POSITION_LIMITS.ENABLE_RATE_LO");


        new_value = 0;

        status = get_unsigned_value(ITEM_ID(PARAM.POSITION_LIMITS), MEMBER_ID(ENABLE_HI), &us8_buf);
        new_value = new_value + us8_buf;

        status = get_unsigned_value(ITEM_ID(PARAM.POSITION_LIMITS), MEMBER_ID(ENABLE_LO), &us8_buf);
        new_value = new_value + us8_buf;

        status = get_unsigned_value(ITEM_ID(PARAM.POSITION_LIMITS), MEMBER_ID(ENABLE_RATE_LO), &us8_buf);
        new_value = new_value + us8_buf;

        status = get_unsigned_value(ITEM_ID(PARAM.POSITION_LIMITS), MEMBER_ID(ENABLE_RATE_HI), &us8_buf);
        new_value = new_value + us8_buf;

        id = ITEM_ID(PARAM.FINAL_VALUE_CUTOFF_LO);
        READ_PARAM(id, MEMBER_ID(ENABLE), "FINAL_VALUE_CUTOFF_LO.ENABLE");
        status = get_unsigned_value(id, MEMBER_ID(ENABLE), &us8_buf);
        new_value = new_value + us8_buf;

        id = ITEM_ID(PARAM.FINAL_VALUE_CUTOFF_HI);
        READ_PARAM(id, MEMBER_ID(ENABLE), "FINAL_VALUE_CUTOFF_HI.ENABLE");
        status = get_unsigned_value(id, MEMBER_ID(ENABLE), &us8_buf);
        new_value = new_value + us8_buf;

        status = get_unsigned_value(ITEM_ID(PARAM.POSITION_LIMITS), MEMBER_ID(LIMITS_PROTECTED), &us8_buf);
        if ( us8_buf != 0 && new_value > 0)
        {

            status = select_from_menu("|en|The device is in protected. \n" \
                                      "disenable the protected mode to continue auto tune?", dummy,dummy,0, \
                                      "Yes;No(Abort)", &ivalue);
            if (ivalue != 1 ) {

                METHOD_ABORT_MACRO;

            }
            else {
                status = put_unsigned_value(ITEM_ID(PARAM.POSITION_LIMITS), MEMBER_ID(LIMITS_PROTECTED), DISABLE);
                SEND_PARAM(ITEM_ID(PARAM.POSITION_LIMITS), MEMBER_ID(LIMITS_PROTECTED), "POSITION_LIMITS.LIMITS_PROTECTED");
            }

        }

        /* check PID slots:
           1. This is decided by jumper in svi ff (posctl_GetCtlSetIndex)
           2. Read soft-settings from activate_control_set by 170,216.
        */
#define CTLPARAMSET_AUTOTUNE 0
        id = ITEM_ID(PARAM.ACTIVE_CONTROL_SET);
        READ_PARAM(id, MEMBER_ID(SELECTOR), "ACTIVE_CONTROL_SET.SELECTOR");
        status = get_unsigned_value(id, MEMBER_ID(SELECTOR), &us8_buf);
        if ( us8_buf != 0 )
        {
            status = select_from_menu("|en|The auto tune operation is required only for " \
                                      "active control set 0.\n"          \
                                      "Do you want to active the control set 0 ? \n", \
                                      dummy,dummy,0, "Yes;No(Abort)", &ivalue);
            if ( ivalue == 2 )
            {
                METHOD_ABORT_MACRO;
            }
            else
            {
                /* switch to custom control set */
                id = ITEM_ID(PARAM.ACTIVATE_CONTROL_SET);
                status = put_unsigned_value(id, 0, CTLPARAMSET_AUTOTUNE);
                SEND_PARAM(id, 0, "ACTIVATE_CONTROL_SET");
            }
        }


        /* Input the auto tune parameters */
        flt_buf = 0;
        status = get_float_value(ITEM_ID(PARAM.AUTOTUNE), MEMBER_ID(SUPPLY_PRESS), &flt_buf);
        if ( 20.0 > flt_buf )
        {
            get_acknowledgement("|en|For successful auto tune process, the supply"
                                " pressure(%[6.3f]{flt_buf}) should be at least 5PSI(34.5kPa) above"
                                " the spring range.\nPlease verify the spring range"
                                " and cancel the procedure to adjust the supply"
                                " pressure if necessary", dummy, dummy, 0);
        }

        status = select_from_menu("|en|Do you want to change auto tune parameters. \n", dummy,dummy,0,\
                                  "Yes;No(Abort)", &ivalue);
        if ( ivalue == 1 )
        {
            EDIT_FLOAT_MACRO_SUB(AUTOTUNE, SUPPLY_PRESS, "Supply Pressure", "psi");

            id=ITEM_ID(PARAM.AUTOTUNE);
            mb=MEMBER_ID(AGGRESSIVENESS);
            status = get_signed_value(id, mb, &ivalue);
            do {
                status = edit_device_value("|en|Agressiveness(%[d]{ivalue})[-9, +9]:", dummy, dummy, 0, id, mb);
                status = get_signed_value(id, mb, &sel);
            } while (sel < -9 || sel > 9);
            SEND_PARAM(id, mb, "AUTOTUNE.AGGRESSIVENESS");
        }
        
        status = select_from_menu("|en|This procedure will take few minutes to conclude. \n" \
                                  "The sequence Number will be listed periodically.      \n" \
                                  "Do you want to proceed?                               \n", \
                                  dummy, dummy, 0, "Yes;No(Abort)", &ivalue);
        if ( ivalue == 2 ) {
            METHOD_ABORT_MACRO;
        }

        /* clear status */
        id = ITEM_ID(PARAM.CLEAR_STATUS);
        status = put_unsigned_value(id, 0, 1);
        SEND_PARAM(id, 0, "CLEAR_STATUS");

        id = ITEM_ID(PARAM.COMPLETE_STATUS);
        mb = MEMBER_ID(CURRENT_STATUS_1_C);
        READ_PARAM(id, mb, "COMPLETE_STATUS.CURRENT_STATUS_1_C");
        status = get_unsigned_value(id, mb, &us8_buf);
        /* IF_ERROR_ABORT_MACRO_INFO("READ_STATUS:"); */

        /* start to Auto Tune */
        id = ITEM_ID(PARAM.AUTOTUNE);
        mb = MEMBER_ID(COMPLETION);

        status = put_unsigned_value(id, mb, AUTOTUNE_START);
        SEND_PARAM(id, mb, "AUTOTUNE.COMPLETION");

        /* read status of running Auto Tune procedure */
        chk_cnt = 0;
        do {

            delayfor( 3, "|en|The auto tune running(%[d]{chk_cnt})......", dummy, dummy, 0);

            /* check the status for find stops in the device status*/
            id = ITEM_ID(PARAM.AUTOTUNE);
            mb = MEMBER_ID(COMPLETION);
            READ_PARAM(id, mb, "AUTOTUNE.COMPLETION");
            status = get_unsigned_value(id, mb, &us8_buf);


            ++chk_cnt;
            ACCELERATE_FOR_DEV(chk_cnt+=10);
            if ( chk_cnt > 120 ) {        /*  5sec * 120 = 10min  */
                chk_cnt = 0;
                status = select_from_menu("|en|Continue?", dummy, dummy, 0, "Yes;No(Abort)", &ivalue);
                if ( ivalue != 1 ) {
                    id = ITEM_ID(PARAM.AUTOTUNE);
                    mb = MEMBER_ID(COMPLETION);
                    status = put_unsigned_value(id, mb, AUTOTUNE_CANCEL);
                    SEND_PARAM(id, mb, "AUTOTUNE.COMPLETION");
                    METHOD_ABORT_MACRO;
                }
            }
        } while ( us8_buf == AUTOTUNE_START || us8_buf == AUTOTUNE_RUNNING );

        delayfor( 5, "|en|The auto tune running(%[d]{chk_cnt})......", dummy, dummy, 0);

        /* report auto tune success/faild */
        id = ITEM_ID(PARAM.COMPLETE_STATUS);
        mb = MEMBER_ID(CURRENT_STATUS_1_C);
        READ_PARAM(id, mb, "COMPLETE_STATUS.CURRENT_STATUS_1_C");
        status = get_unsigned_value(id, mb, &us8_buf);
        /* IF_ERROR_ABORT_MACRO_INFO("READ_STATUS:"); */

        if ( (us8_buf & FBIT2 ) != 0 ) {
            /* display the reason of failed tuning.*/
            id = ITEM_ID(PARAM.AUTOTUNE);
            mb = MEMBER_ID(COMPLETION);
            status = get_unsigned_value(id, mb, &us8_buf);
            if ( us8_buf > 30 ) {
                maxlen = 255;
                status = get_status_string(id, mb, us8_buf, str_buf, maxlen);
                get_acknowledgement("|en|The auto tune failed, due to %{str_buf}.", dummy,dummy,0);
            }
            else
            {
                get_acknowledgement("|en|The auto tune failed.", dummy,dummy,0);
            }
            METHOD_ABORT_MACRO;
        }

        /* read the auto tune parameters */
        display_message("|en|The auto tune finished.", dummy, dummy, 0);
    }
}

METHOD change_app_mode
{
    LABEL "|en|change app mode";
    HELP  "|en|to change the app mode:Setup, Normal or Manual";
    CLASS INPUT;
    DEFINITION
    {
        COMMON_LOCAL_VARS;

            unsigned long  old_value, new_value;
        int            ivalue;
        float          flt_buf;

        maxlen = 256;
        id = ITEM_ID(PARAM.APP_MODE);
        mb = 0;

        READ_PARAM(id, mb, "APP_MODE");
        status = get_unsigned_value(id, mb, &us8_buf);
        status = get_status_string(id, mb, us8_buf, str_buf, maxlen);
        status = select_from_menu("|en| ""APP_MODE" " = %{str_buf}.\n" "Do you want to change the mode" "\n",
                                  dummy, dummy, 0, "Yes;No(Skip)", &ivalue);
        display_message("|en|\n", dummy, dummy, 0);

        if ( ivalue == 1 ) {

            id = ITEM_ID(PARAM.APP_MODE);
            mb = 0;
            status = select_from_menu("|en|Change to:\n", dummy, dummy, 0, "Setup;Normal", &ivalue);
            switch (ivalue){
            case 1:
                status = put_unsigned_value(id, mb, CHANGE_SETUP);
                break;
            case 2:
                status = put_unsigned_value(id, mb, CHANGE_NORMAL);
                break;
            }
            SEND_PARAM(id, mb, "APP_MODE");

            READ_PARAM(id, mb, "APP_MODE");
            status = get_unsigned_value(id, 0, &us8_buf);
            status = get_status_string(id, 0, us8_buf, str_buf, maxlen);
            status = get_acknowledgement("|en| ""APP_MODE" " = %{str_buf}.\n",
                                         dummy, dummy, 0);
        }
    }
}

METHOD  cancel_find_stops
{
    LABEL "|en|cancel findstops";
    HELP HLP5("|en|run in the abort method to cancel findstops process");
    CLASS OUTPUT;
    DEFINITION
    {
        COMMON_LOCAL_VARS;

        id = ITEM_ID(PARAM.FIND_STOPS);
        mb = 0;

        status = get_unsigned_value(id, mb, &us8_buf);
        if ( us8_buf != FINDSTOPS_NOTRUNNING && us8_buf != FINDSTOPS_CANCEL )
        {
            status = put_unsigned_value(id, mb, FINDSTOPS_CANCEL);
            SEND_PARAM(id, mb, "FIND_STOPS:CANCEL");
            display_message("|en|\n The find stops procedure was canceled.", dummy,dummy, 0);
        }

    }
}

METHOD  do_manual_hi_low_stops
{
    LABEL "|en|Manual Stops";
    HELP "|en|Manual High/Low Stop Limits adjustment";
    CLASS OUTPUT;
    DEFINITION
    {
        COMMON_LOCAL_VARS;

        int           ivalue;
        int           sel;
        float         flt_buf;
        long          chk_cnt;

        unsigned long func;
        unsigned long ulCal_Type;

        unsigned long trl_ids[7],trl_mbs[7];

        char strPress_Unit[5];
        unsigned long press_unit_index;

        unsigned long act_style;

        float fSupply_press_max, fSupply_press_min;
        float fOpenStopAdj;

        char strLocation[33];
        char strWho[33];
        char strCal_Date[33];

        CHECK_DEVICE_IN_PROCESS;

        add_abort_method(ITEM_ID(cancel_find_stops));

        CHECK_DEVICE_IN_MAN;


        /* set the default travel parameters */
        /*     ·   Travel = 100 */
        /*     ·   Travel Units = % */
        /*     ·   Open Stop Adjustment = 100 % */


        ids[0] = ITEM_ID(PARAM.TRAVEL);
        mbs[0] = MEMBER_ID(RANGE);

        ids[1] = ids[0];
        mbs[1] = MEMBER_ID(UNITS_INDEX);

        ids[2] = ITEM_ID(PARAM.OPEN_STOP_ADJUSTMENT);
        mbs[2] = 0;


        ids[3] = ITEM_ID(PARAM.FIND_STOPS);
        mbs[3] = 0;

        ids[4] = ITEM_ID(PARAM.ACTUATOR_A_PRESSURE);
        mbs[4] = MEMBER_ID(VALUE);

        ids[5] = ITEM_ID(PARAM.ACTUATOR_B_PRESSURE);
        mbs[5] = MEMBER_ID(VALUE);

        ids[6] = ITEM_ID(PARAM.RAW_POSITION);
        mbs[6] = 0;

        ids[7] = ITEM_ID(PARAM.ACTUAL_POSITION);
        mbs[7] = MEMBER_ID(VALUE);

        ids[8] = ITEM_ID(PARAM.SUPPLY_PRESSURE);
        mbs[8] = MEMBER_ID(VALUE);

        ids[9] = ITEM_ID(PARAM.PRESSURE_EXTREMES);
        mbs[9] = 0;

        ids[10] = ITEM_ID(PARAM.PRESSURE_RANGE);
        mbs[10] = MEMBER_ID(UNITS_INDEX);

        ids[11] = ITEM_ID(PARAM.ACTUATOR_3);
        mbs[11] = MEMBER_ID(ACT_STYLE);

        status = put_float_value(ids[0], mbs[0], 100.0); /* travel.range */
        status = put_unsigned_value(ids[1], mbs[1],1342); /* travel.unit */

        fOpenStopAdj = 100.0;
        status = put_float_value(ids[2], 0, fOpenStopAdj); /* Open stops adjustment */

        status = get_unsigned_value(ids[10], mbs[10], &press_unit_index);
        GET_ENUM_STRING(ids[10], mbs[10], strPress_Unit, 5);

        status = get_unsigned_value(ids[11], mbs[11], &act_style);
        act_style = 0;

        trl_ids[0]=trl_ids[1]=trl_ids[2]=trl_ids[3]=trl_ids[4]=trl_ids[5]=trl_ids[6]=ITEM_ID(PARAM.TRAVEL_CALIBRATION);
        trl_mbs[0]=MEMBER_ID(CAL_LOCATION);
        trl_mbs[1]=MEMBER_ID(CAL_DATE);
        trl_mbs[2]=MEMBER_ID(CAL_WHO);
        trl_mbs[3]=MEMBER_ID(CAL_STOP_HI_POS);
        trl_mbs[4]=MEMBER_ID(CAL_STOP_LO_POS);
        trl_mbs[5]=MEMBER_ID(CAL_TYPE);
        trl_mbs[6]=MEMBER_ID(LAST_RESULT);


        /* GET_ENUM_STRING(1); */

        status = select_from_menu("|en|\nDefault Travel Setting:\n"           \
                                  "  TRAVEL.RANGE = %[5.2f]{0}\n" \
                                  "  TRAVEL.UNITS_INDEX = %{1}\n" \
                                  "Do you want to change the setting?\n",
                                  ids, mbs, 9, "Yes;No(skip)", &ivalue);

        if (ivalue == 1)
        {
            status = edit_device_value("|en|\n TRAVEL.RANGE(%[5.2f]{0})\n",
                                       ids, mbs, 9, ids[0], mbs[0]);
            status = edit_device_value("|en|\n TRAVEL.UNIT(%{1})\n",
                                       ids, mbs, 9, ids[1], mbs[1]);
        }
        READ_PARAM(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), "PRESSURE_EXTREMES.SUPPLY_PRESSURE_MAX");
        READ_PARAM(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), "PRESSURE_EXTREMES.SUPPLY_PRESSURE_MIN");

        status = get_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), &fSupply_press_max);
        status = get_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), &fSupply_press_min);
        status = put_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), -9999.99);
        status = put_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), 9999.99);

        status = select_from_menu("|en|\nPlease select manual stops:\n", \
                                  ids, mbs, 9, \
                                  "Manual LO limit;Manual HI limit;Manual LO/HI Limits;Abort", &ivalue);

        switch (ivalue)
        {
        case 1:
            sel = FINDSTOPS_SETCLOSED;
            func = FINDSTOPS_SETCLOSED;
            ulCal_Type = CAL_TYPE_CLOSED_ENDPOINT_MANUAL;
            break;
        case 2:
            sel = FINDSTOPS_SETOPEN;
            func = FINDSTOPS_SETOPEN;
            ulCal_Type = CAL_TYPE_OPEN_ENDPOINT_MANUAL;
            break;
        case 3:
            sel = FINDSTOPS_MANUL_LO_HI;
            func = FINDSTOPS_SETOPEN;
            ulCal_Type = CAL_TYPE_BOTH_ENDPOINTS_MANUAL;
            break;
        case 4:
            METHOD_ABORT_MACRO;
            break;
        }

        if ( sel == FINDSTOPS_SETCLOSED )
        {    
            status = select_from_menu("|en|\nDefault Open Stops Adjustment:\n\n" \
                                      "  OPEN_STOP_ADJUSTMENT = %[5.2f]{2}%%\n\n", \
                                      ids, mbs, 9, "Continue", &ivalue);
        }
        else
        {
            status = select_from_menu("|en|\nDefault Open Stops Adjustment:\n" \
                                      "OPEN_STOP_ADJUSTMENT = %[5.2f]{2}%%\n\n" \
                                      "Do you want to change the setting?", \
                                      ids, mbs, 9, "Yes;No(skip)", &ivalue);
            if (ivalue == 1)
            {
                status = edit_device_value("|en|\n OPEN_STOP_ADJUSTMENT(%[5.2f]{2})\n",
                                           ids, mbs, 9, ids[2], mbs[2]);
                status = get_float_value(ids[2], mbs[2], &fOpenStopAdj);
            }
        }

        send_all_values();

        status = put_unsigned_value(ids[3], mbs[3], func);

        GET_ENUM_STRING(ids[3], mbs[3], str_buf, 256);
        SEND_PARAM(ids[3], 0, "FIND_STOPS");
        do 
        {
            /* The software shall detect that the procedure is completed, when
             * the value of FIND_STOPS is switched from 11 (Find stops Running)
             * to 0(Find stops Not Running). */
            if (SVI2_DBL_ACT == act_style )
            {
                delayfor(3,  "|en|\nRun %{str_buf}:\n\n" \
                                 "  FIND STOPS = %{3}\n" \
                                 "  ACTUATOR A PRESSURE = %[7.2f]{4} %{strPress_Unit}\n" \
                                 "  ACTUATOR B PRESSURE = %[7.2f]{5} %{strPress_Unit}\n" \
                                 "  SUPPLY PRESSURE = %[7.2f]{8} %{strPress_Unit}\n"        \
                                 "  RAW POSITION = %[8i]{6}\n"          \
                                 "  ACTUAL POSITION = %[7.2f]{7}\n",    \
                                 ids, mbs, 9);
            }
            else if (SVI2_SNGL_ACT == act_style)
            {
                delayfor(3,    "|en|\nRun %{str_buf}:\n\n" \
                                 "  FIND STOPS = %{3}\n" \
                                 "  ACTUATOR PRESSURE = %[7.2f]{4} %{strPress_Unit}\n" \
                                 "  SUPPLY PRESSURE = %[7.2f]{8} %{strPress_Unit}\n"        \
                                 "  RAW POSITION = %[8i]{6}\n"          \
                                 "  ACTUAL POSITION = %[7.2f]{7}\n",    \
                                 ids, mbs, 9);
            }
            

            status = select_from_menu("|en|\nPlease select:\n", 
                                      dummy, dummy, 0, "Accept;Continue;Concel", &ivalue);

            if ( ivalue == 1 ) /* Accept */
            {
                status = put_unsigned_value( ids[3], mbs[3], FINDSTOPS_ACCEPT);
                SEND_PARAM(ids[3], mbs[3], "FINDSTOPS_ACCEPT");

                if ( sel == FINDSTOPS_MANUL_LO_HI && func == FINDSTOPS_SETOPEN )
                {
                    status = put_unsigned_value(ids[3], mbs[3], FINDSTOPS_SETCLOSED);
                    status = get_status_string(ids[3], mbs[3], FINDSTOPS_SETCLOSED, str_buf, maxlen);
                    SEND_PARAM(ids[3], mbs[3], "FINDSTOPS_SETCLOSED");
                    ivalue = 2;
                    func = FINDSTOPS_SETCLOSED;
                }
                
            }
            
            
        } while(ivalue == 2) /* Continue */;
        
        if (ivalue == 3) /* Cancel */
        {
            return;
        }

        if (ivalue == 1)
        {
            status = select_from_menu("|en|\nPlease select:\n", 
                                      dummy, dummy, 0, "Confirm;No(skip)", &ivalue);
            if (ivalue == 1)
            {
                status = put_unsigned_value( ids[3], mbs[3], FINDSTOPS_COMMIT);
                SEND_PARAM(ids[3], mbs[3], "FINDSTOPS:COMMIT");
            }


            /* if ( 1|| sel == FINDSTOPS_MANUL_LO_HI ) */
            {
                status = put_unsigned_value( trl_ids[5], trl_mbs[5], ulCal_Type);
                SEND_PARAM(trl_ids[5], trl_mbs[5], "TRAVEL_CALIBRATION.CAL_TYPE");

                READ_PARAM(trl_ids[0], trl_mbs[0], "TRAVEL_CALIBRATION.CAL_LOCATION");
                READ_PARAM(trl_ids[1], trl_mbs[1], "TRAVEL_CALIBRATION.CAL_DATE");
                READ_PARAM(trl_ids[2], trl_mbs[2], "TRAVEL_CALIBRATION.CAL_WHO");
                READ_PARAM(trl_ids[3], trl_mbs[3], "TRAVEL_CALIBRATION.CAL_STOP_HI_POS");
                READ_PARAM(trl_ids[4], trl_mbs[4], "TRAVEL_CALIBRATION.CAL_STOP_LO_POS");
                READ_PARAM(trl_ids[5], trl_mbs[5], "TRAVEL_CALIBRATION.CAL_TYPE");
                READ_PARAM(trl_ids[6], trl_mbs[6], "TRAVEL_CALIBRATION.LAST_RESULT");

                maxlen = 33;
                status = get_string_value(trl_ids[0], trl_mbs[0], strLocation, &maxlen);
                maxlen = 33;
                status = get_string_value(trl_ids[2], trl_mbs[2], strWho, &maxlen);
                maxlen = 33;
                status = get_date_value(trl_ids[1], trl_mbs[1], strCal_Date, &maxlen);
                
                /*display the travel calibration restult*/
                display_message("|en|\nTravel Calibration:\n"  \
                                "  CAL_LOCATION:%[s]{strLocation}\n"  \
                                "  CAL_DATe: %{1}\n"  \
                                "  CAL_WHO:%[s]{strWho}\n"  \
                                "  CAL_STOP_HI_POS:%[d]{3}\n"  \
                                "  CAL_STOP_LO_POS:%[d]{4}\n"  \
                                "  CAL_TYPE:%{5}\n"  \
                                "  LAST_RESULT:%{6}\n", trl_ids, trl_mbs, 7);

                status = select_from_menu("\nDo you wish to change the values", dummy, dummy, 0, "Yes;Skip", &ivalue);
                if ( ivalue == 1)
                {
                    edit_device_value("|en|Enter the new value for CAL_LOCATION:\n",
                                      trl_ids, trl_mbs, 7, trl_ids[0], trl_mbs[0]);
                    edit_device_value("|en|Enter the new value for CAL_DATE:\n",
                                      trl_ids, trl_mbs, 7, trl_ids[1], trl_mbs[1]);
                    edit_device_value("|en|Enter the new value for CAL_WHO:\n",
                                      trl_ids, trl_mbs, 7, trl_ids[2], trl_mbs[2]);
                    /* send_all_values(); */
                }
                send_all_values();
            }

            /*Check the result of the Auto Stop Limits procedure,by reading the
              COMPLETE_STATUS.FIND_STOPS_FAILD and present the result to the
              user.*/

            READ_PARAM(ids[2], mbs[2], "OPEN_STOP_ADJUSTMENT");

            if ( sel != FINDSTOPS_SETCLOSED)
            {
                status = put_float_value( ids[2], mbs[2], fOpenStopAdj);
                SEND_PARAM(ids[2], mbs[2], "OPEN_STOP_ADJUSTMENT");
            }

            /* At the end of Manual Low Stop Limit procedure, the software shall */
            /* recommend to the user to run Manual High Stop Limit procedure. */
            /* At the end of Manual High Stop Limit procedure, the software shall */
			/* recommend to the user to run Manual Low Stop Limit procedure. */


            /*restore the saved supply_pressure_extremes*/
            status = put_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), fSupply_press_max);
            status = put_float_value(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), fSupply_press_min);
            SEND_PARAM(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MAX), "PRESSURE_EXTREMES.SUPPLY_PRESSURE_MAX");
            SEND_PARAM(ids[9], MEMBER_ID(SUPPLY_PRESSURE_MIN), "PRESSURE_EXTREMES.SUPPLY_PRESSURE_MIN");
        }

        display_message("|en|\nThe Manual Stop Limit finished.", ids, dummy, 0);

        if ( sel == FINDSTOPS_SETCLOSED )
        {
            display_message("|en|\nPlease run the Manual High Stop Limit Adjustment, "
                            "If the procedure was not done.", dummy, dummy, 0);
        }
        if ( sel == FINDSTOPS_SETOPEN )
        {
            display_message("|en|\nPlease run the Manual LOW Stop Limit Adjustment, "
                            "If the procedure was not done.", dummy, dummy, 0);
        }

        return;
    }
}

/*-----------------/
** Revision History
/-----------------*/


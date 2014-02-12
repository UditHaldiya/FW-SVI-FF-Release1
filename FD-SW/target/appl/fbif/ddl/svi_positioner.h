/* ===========================================================================

FILE_NAME          svi_positioner.h



FUNCTIONAL_MODULE_DESCRIPTION

  This file contains definitions for the DD of the SVI Positioner application.

=========================================================================== */

/*------------------------------------------------------------/
** Definitions Strings for LABLE and HELPS for SVI2 AP FF
/------------------------------------------------------------*/

#define   dct_ack_opt_0                                 \
    "|en|Disc Alm Auto Ack"                             \

#define   dct_ack_opt_0_help                            \
    "|en|Discrete alarm auto acknowledge enabled"       \

#define   dct_ack_opt_1                                 \
    "|en|HiHi Alm Auto Ack"                             \

#define   dct_ack_opt_1_help                            \
    "|en|HiHi alarm auto acknowledge enabled"           \

#define   dct_ack_opt_2                                 \
    "|en|Hi Alm Auto Ack"                               \

#define   dct_ack_opt_2_help                            \
    "|en|Hi alarm auto acknowledge enabled"             \

#define   dct_ack_opt_3                                 \
    "|en|LoLo Alm Auto Ack"                             \

#define   dct_ack_opt_3_help                            \
    "|en|LoLo alarm auto acknowledge enabled"           \

#define   dct_ack_opt_4                                 \
    "|en|Lo Alm Auto Ack"                               \

#define   dct_ack_opt_4_help                            \
    "|en|Lo alarm auto acknowledge enabled"             \

#define   dct_ack_opt_5                                 \
    "|en|DevHi Alm Auto Ack"                            \

#define   dct_ack_opt_5_help                            \
    "|en|Deviation high alarm auto acknowledge enabled" \

#define   dct_ack_opt_6                                 \
    "|en|DevLo Alm Auto Ack"                            \

#define   dct_ack_opt_6_help                            \
    "|en|Deviation low alarm auto acknowledge enabled"  \

#define   dct_ack_opt_7                                 \
    "|en|Blk Alm Auto Ack"                              \

#define   dct_ack_opt_7_help                            \
    "|en|Block alarm auto acknowledge enabled"          \

#define   dct_ack_opt_8                                 \
    "|en|Fail Alm Auto Ack"                             \

#define   dct_ack_opt_8_help                            \
    "|en|Fail alarm auto acknowledge enabled"           \

#define   dct_ack_opt_9                                 \
    "|en|Off Spec Alm Auto Ack"                         \

#define   dct_ack_opt_9_help                            \
    "|en|Off spec alarm auto acknowledge enabled"       \

#define   dct_ack_opt_10                                \
    "|en|Maint Alm Auto Ack"                            \

#define   dct_ack_opt_10_help                           \
    "|en|Maintenance alarm auto acknowledge enabled"    \

#define   dct_ack_opt_11                                \
    "|en|Check Alm Auto Ack"                            \

#define   dct_ack_opt_11_help                           \
    "|en|Check alarm auto acknowledge enabled"          \

#define _FF_ACK_OPT_0  { _DISCRETE_ALARM,          dct_ack_opt_0,    dct_ack_opt_0_help }
#define _FF_ACK_OPT_1  { _HIGH_HIGH_ALARM,         dct_ack_opt_1,    dct_ack_opt_1_help }
#define _FF_ACK_OPT_2  { _HIGH_ALARM,              dct_ack_opt_2,    dct_ack_opt_2_help }
#define _FF_ACK_OPT_3  { _LOW_LOW_ALARM,           dct_ack_opt_3,    dct_ack_opt_3_help }
#define _FF_ACK_OPT_4  { _LOW_ALARM,               dct_ack_opt_4,    dct_ack_opt_4_help }
#define _FF_ACK_OPT_5  { _DEVIATION_HIGH_ALARM,    dct_ack_opt_5,    dct_ack_opt_5_help }
#define _FF_ACK_OPT_6  { _DEVIATION_LOW_ALARM,     dct_ack_opt_6,    dct_ack_opt_6_help }
#define _FF_ACK_OPT_7  { _BLOCK_ALARM,             dct_ack_opt_7,    dct_ack_opt_7_help }
#define _FF_ACK_OPT_8  { _FAIL_ALARM,              dct_ack_opt_8,    dct_ack_opt_8_help }
#define _FF_ACK_OPT_9  { _OFF_SPEC_ALARM,          dct_ack_opt_9,    dct_ack_opt_9_help }
#define _FF_ACK_OPT_10 { _MAINTENANCE_ALARM,       dct_ack_opt_10,   dct_ack_opt_10_help }
#define _FF_ACK_OPT_11 { _CHECK_ALARM,             dct_ack_opt_11,   dct_ack_opt_11_help }


#define LBL_CH_AO_NOT_CONFIGURED "|en|Not Configured"                                   \

#define HLP_CH_AO_NOT_CONFIGURED                                                        \
  "|en|Should prevent the Analog Output block to go to Auto Mode"                       \

#define LBL_CH_AO_POSITION "|en|POSITION"                                               \

#define HLP_CH_AO_POSITION                                                              \
  "|en|Should update the FINAL_VALUE parameter in the Transducer block. Readback"       \
  "shall be provided from FINAL_POSITION_VALUE."                                        \
  "\nShould not allow to switch the AO block to auto mode if the"                       \
  "TB.SETPOINT_SOURCE is not equal to 1 (Analog Output block)"                          \
  "\nShould report an error for the setting mismatch."                                  \


#define LBL_CH_DO_NOT_CONFIGURED "|en|Not Configured"                                   \

#define HLP_CH_DO_NOT_CONFIGURED                                                        \
  "|en|Should prevent the Discrete Output block to go to Auto Mode"                     \

#define LBL_CH_DO_OPEN_CLOSE "|en|Open/Close"                                           \

#define HLP_CH_DO_OPEN_CLOSE                                                            \
  "|en|Should update the FINAL_VALUE_D parameter in the Transducer"                     \
  "block. Readback shall be provided from FINAL_POSITION_VALUE_D."                      \
  "\nShould not allow to switch the DO block to auto mode if the"                       \
  "TB.SETPOINT_SOURCE is not equal to 2 (DO block in Open/Close mode)"                  \
  "\nShould report an error for the setting mismatch."                                  \

#define LBL_CH_DO_DISCRETE_POSITION "|en|Discrete Position"                             \

#define HLP_CH_DO_DISCRETE_POSITION                                                     \
  "|en|Should update the FINAL_VALUE_DINT parameter in the Transducer"                  \
  "block. Readback shall be provided from FINAL_POSITION_VALUE_DINT."                   \
  "\nShould not allow to switch the DO block to auto mode if the"                       \
  "TB.SETPOINT_SOURCE is not equal to 3 (Discrete Output block in Discrete"             \
  "Mode)"                                                                               \
  "\nShould report an error for the setting mismatch."                                  \

#define LBL_CH_DO_DISCRETE_SWITCH_1 "|en|Discrete Switch 1"                             \

#define HLP_CH_DO_DISCRETE_SWITCH_1                                                     \
  "|en|The DO.OUT value shall update the TB.DISCRETE_OUTPUT_1 parameter, which"         \
  "controls the DO1 switch controlled by the APP processor."                            \

#define LBL_CH_DO_DISCRETE_SWITCH_2 "|en|Discrete Switch 2"                             \

#define HLP_CH_DO_DISCRETE_SWITCH_2                                                     \
  "|en|The DO.OUT value shall update the TB.DISCRETE_OUTPUT_2 parameter, which"         \
  "controls the DO2 switch controlled by the APP processor."                            \

#define LBL_CH_DO_FAULT_STATE "|en|Fault State"                                         \

#define HLP_CH_DO_FAULT_STATE                                                           \
  "|en|The DO.Out value enables/disables the valve control. Setting the value to"       \
  "TRUE with good state forces the TB to Fault State "                                  \

#define LBL_CH_AI_NOT_CONFIGURED "|en|Not Configured"                                   \

#define HLP_CH_AI_NOT_CONFIGURED                                                        \
  "|en|Should prevent the Analog Input block to go to Auto Mode"                        \

#define LBL_CH_AI_SUPPLY_PRESSURE "|en|Supply Pressure"                                 \

#define HLP_CH_AI_SUPPLY_PRESSURE                                                       \
  "|en|Provides the value of the supply pressure to the FF control"                     \

#define LBL_CH_AI_ACTUATOR_PRESSURE_1 "|en|Actuator Pressure 1"                         \

#define HLP_CH_AI_ACTUATOR_PRESSURE_1                                                   \
  "|en|Provides the value of the pressure in Actuator 1 to the FF control"              \

#define LBL_CH_AI_ACTUATOR_PRESSURE_2 "|en|Actuator Pressure 2"                         \

#define HLP_CH_AI_ACTUATOR_PRESSURE_2                                                   \
  "|en|Provides the value of the pressure in Actuator 2 to the FF control"              \

#define LBL_CH_AI_IP_CURRENT "|en|IP_Current"                                           \

#define HLP_CH_AI_IP_CURRENT                                                            \
  "|en|Provides the value of the IP current to the FF control"                          \

#define LBL_CH_AI_PILOT_PRESSURE "|en|Pilot Pressure"                                   \

#define HLP_CH_AI_PILOT_PRESSURE                                                        \
  "|en|Provides the value of the pilot pressure to the FF control"                      \

#define LBL_CH_AI_TEMPERATURE "|en|Temperature"                                         \

#define HLP_CH_AI_TEMPERATURE                                                           \
  "|en|Provides the board temperature to the FF control"                                \

#define LBL_CH_AI_FINAL_VALUE "|en|Final Value"                                         \

#define HLP_CH_AI_FINAL_VALUE                                                           \
  "|en|Provides back the TB setpoint to the FF control"                                 \

#define LBL_CH_AI_FINAL_POSITION_VALUE "|en|Final Position Value"                       \

#define HLP_CH_AI_FINAL_POSITION_VALUE                                                  \
  "|en|Provides back the TB actual position to the FF control"                          \

#define LBL_CH_AI_WORKING_SP "|en|Working SP"                                           \

#define HLP_CH_AI_WORKING_SP                                                            \
  "|en|Provides back the TB working setpoint (after characterization) to the FF"        \
  "control"                                                                             \

#define LBL_CH_AI_WORKING_POSITION "|en|Working Position"                               \

#define HLP_CH_AI_WORKING_POSITION                                                      \
  "|en|Provides back the TB working position (before de-characterization) to the"       \
  "FF control"                                                                          \

#define LBL_CH_DI_NOT_CONFIGURED "|en|Not Configured"                                   \

#define HLP_CH_DI_NOT_CONFIGURED                                                        \
  "|en|Should prevent the Discrete Output block to go to Auto Mode"                     \

#define LBL_CH_DI_DISCRETE_INPUT "|en|Discrete Input"                                   \

#define HLP_CH_DI_DISCRETE_INPUT                                                        \
  "|en|Provides the state of the discrete input to the FF control"                      \

#define LBL_CH_DI_STATE_OF_SW1 "|en|State of Sw1"                                       \

#define HLP_CH_DI_STATE_OF_SW1                                                          \
  "|en|Provides the state of the discrete Switch 1 from APP processor to the FF"        \
  "control"                                                                             \

#define LBL_CH_DI_STATE_OF_SW2 "|en|State of Sw2"                                       \

#define HLP_CH_DI_STATE_OF_SW2                                                          \
  "|en|Provides the state of the discrete Switch 2 from APP processor to the FF"        \
  "control"                                                                             \

#define LBL_CH_MAI_NOT_CONFIGURED "|en|Not Configured"                                  \

#define HLP_CH_MAI_NOT_CONFIGURED                                                       \
  "|en|Should prevent the MAI block to go to Auto Mode"                                 \

#define LBL_CH_MAI_DEFAULT_CHANNEL "|en|Default MAI Channel number"                     \

#define HLP_CH_MAI_DEFAULT_CHANNEL                                                      \
  "|en|The one and only channel number available for MAI block"                         \

/*------------------------------------------------------------/
** Addtional parameter definitions for PID SVi2 AP FF
/------------------------------------------------------------*/

#define HLP_ALGORITHM                                                                   \
  "|en|Used PID algorithm"                                                              \

#define LBL_ALGORITHM                                                                   \
  "|en|Algorithm"                                                                       \

#define LBL_IDEAL    "|en|Ideal"
#define LBL_SERIAL   "|en|Serial"
#define LBL_PARALLEL "|en|Parallel"
#define LBL_I_PD     "|en|I-PD"
#define LBL_IP_D     "|en|IP-D"

#define ENUM_ALGORITHM              \
  { 0, LBL_IDEAL, LBL_IDEAL},       \
  { 1, LBL_SERIAL, LBL_SERIAL},     \
  { 2, LBL_PARALLEL, LBL_PARALLEL}, \
  { 3, LBL_I_PD, LBL_I_PD},         \
  { 4, LBL_IP_D, LBL_IP_D}          \

#define HLP_TFPD                                                                    \
  "|en|Time constant filtering the Proportional-Derivative component"                                                               \

#define LBL_TFPD                                                                    \
  "|en|Time constant filtering the Proportional-Derivative component"           \

#define HLP_TFPID                                                                   \
  "|en|Time constant of a single exponential filter of all PID components"                                                              \

#define LBL_TFPID                                                                   \
  "|en|Time constant of a single exponential filter of all PID components"           \


/*------------------------------------------------------------/
** Channels Definitions for SVi2 AP FF
/------------------------------------------------------------*/


#define CHANNELS_AO                                                             \
  { 0,  LBL_CH_AO_NOT_CONFIGURED,          HLP_CH_AO_NOT_CONFIGURED},           \
  { 1,  LBL_CH_AO_POSITION,                HLP_CH_AO_POSITION}

#define CHANNELS_DO                                                             \
  { 0,  LBL_CH_DO_NOT_CONFIGURED,          HLP_CH_DO_NOT_CONFIGURED},           \
  { 2,  LBL_CH_DO_OPEN_CLOSE,              HLP_CH_DO_OPEN_CLOSE},               \
  { 3,  LBL_CH_DO_DISCRETE_POSITION,       HLP_CH_DO_DISCRETE_POSITION},        \
  { 4,  LBL_CH_DO_DISCRETE_SWITCH_1,       HLP_CH_DO_DISCRETE_SWITCH_1},        \
  { 5,  LBL_CH_DO_DISCRETE_SWITCH_2,       HLP_CH_DO_DISCRETE_SWITCH_2},        \
  { 6,  LBL_CH_DO_FAULT_STATE,             HLP_CH_DO_FAULT_STATE}

#define CHANNELS_AI                                                             \
  { 0,  LBL_CH_AI_NOT_CONFIGURED,          HLP_CH_AI_NOT_CONFIGURED},           \
  { 7,  LBL_CH_AI_SUPPLY_PRESSURE,         HLP_CH_AI_SUPPLY_PRESSURE},          \
  { 8,  LBL_CH_AI_ACTUATOR_PRESSURE_1,     HLP_CH_AI_ACTUATOR_PRESSURE_1},      \
  { 9,  LBL_CH_AI_ACTUATOR_PRESSURE_2,     HLP_CH_AI_ACTUATOR_PRESSURE_2},      \
  { 10, LBL_CH_AI_IP_CURRENT,              HLP_CH_AI_IP_CURRENT},               \
  { 11, LBL_CH_AI_PILOT_PRESSURE,          HLP_CH_AI_PILOT_PRESSURE},           \
  { 12, LBL_CH_AI_TEMPERATURE,             HLP_CH_AI_TEMPERATURE},              \
  { 13, LBL_CH_AI_FINAL_VALUE,             HLP_CH_AI_FINAL_VALUE},              \
  { 14, LBL_CH_AI_FINAL_POSITION_VALUE,    HLP_CH_AI_FINAL_POSITION_VALUE},     \
  { 15, LBL_CH_AI_WORKING_SP,              HLP_CH_AI_WORKING_SP},               \
  { 16, LBL_CH_AI_WORKING_POSITION,        HLP_CH_AI_WORKING_POSITION}

#define CHANNELS_DI                                                             \
  { 0,  LBL_CH_DI_NOT_CONFIGURED,          HLP_CH_DI_NOT_CONFIGURED},           \
  { 17, LBL_CH_DI_DISCRETE_INPUT,          HLP_CH_DI_DISCRETE_INPUT},           \
  { 18, LBL_CH_DI_STATE_OF_SW1,            HLP_CH_DI_STATE_OF_SW1},             \
  { 19, LBL_CH_DI_STATE_OF_SW2,            HLP_CH_DI_STATE_OF_SW2}

#define CHANNELS_MAI                                                            \
  { 0,  LBL_CH_MAI_NOT_CONFIGURED,         HLP_CH_MAI_NOT_CONFIGURED},          \
  { 20, LBL_CH_MAI_DEFAULT_CHANNEL,        HLP_CH_MAI_DEFAULT_CHANNEL}

/*------------------------------------------------------------/
** Others from Softing
/------------------------------------------------------------*/

/*
**********************************************************************
**********************************************************************
resource block
**********************************************************************
**********************************************************************
*/

#define FEATURES_RES2 { 0x0001,  [feature_unicode],      [feature_unicode_help]                    }, \
                      { 0x0002,  [feature_report],       [feature_report_help]                     }, \
                      { 0x0004,  [feature_faultstate],   [feature_faultstate_help]                 }, \
                      { 0x0008,  [feature_soft_write],   [feature_soft_write_help]                 }, \
                      { 0x0010,  [feature_hard_write],   [feature_hard_write_help]                 }, \
                      { 0x0020,  [feature_out_readback], [feature_out_readback_help]               }, \
                      { 0x0040,  [feature_direct_write], [feature_direct_write_help]               }, \
                      { 0x0080,  [feature_bypass_auto],  [feature_bypass_auto_help]                }, \
                      { 0x0100,  "|en|MVC Reports",      "|en|MVC Report Distribution supported"   }, \
                      { 0x0200,  "|en|MVC Publ/Subscr",  "|en|MVC Publishing/Subscribing supported"}, \
                      { 0x0400,  "|en|Mbit Alarm",       "|en|Multi-bit Alarm supported"           }

#define RESTART_FACTORY_DEFAULT       "|en|Factory Defaults"
#define RESTART_FACTORY_DEFAULT_HELP  "|en|Restart device and reset all configurable device parameters to the values before any configuration was done by the user"

/*
**********************************************************************
**********************************************************************
function blocks
**********************************************************************
**********************************************************************
*/



#define ACK_OPTIONS_AI     { 0x0002,  "|en|HiHi Alm Auto Ack Enabled",  "|en|HiHi alarm automatically acknowledged enabled"  }, \
                           { 0x0004,  "|en|Hi Alm Auto Ack Enabled",    "|en|Hi alarm automatically acknowledged enabled"    }, \
                           { 0x0008,  "|en|LoLo Alm Auto Ack Enabled",  "|en|LoLo alarm automatically acknowledged enabled"  }, \
                           { 0x0010,  "|en|Lo Alm Auto Ack Enabled",    "|en|Lo alarm automatically acknowledged enabled"    }, \
                           { 0x0080,  "|en|Block Alm Auto Ack Enabled", "|en|Block alarm automatically acknowledged enabled" }

#define ACK_OPTIONS_DI     { 0x0001,  "|en|Disc Alm Auto Ack Enabled",     "|en|Discrete alarm automatically acknowledged enabled" }, \
                           { 0x0080,  "|en|Block Alm Auto Ack Enabled",    "|en|Block alarm automatically acknowledged enabled"    }

#define ACK_OPTIONS_PID    { 0x0002,  "|en|HiHi Alm Auto Ack Enabled",     "|en|HiHi alarm automatically acknowledged enabled"         }, \
                           { 0x0004,  "|en|Hi Alm Auto Ack Enabled",       "|en|Hi alarm automatically acknowledged enabled"           }, \
                           { 0x0008,  "|en|LoLo Alm Auto Ack Enabled",     "|en|LoLo alarm automatically acknowledged enabled"         }, \
                           { 0x0010,  "|en|Lo Alm Auto Ack Enabled",       "|en|Lo alarm automatically acknowledged enabled"           }, \
                           { 0x0020,  "|en|DevHi Alm Auto Ack Enabled",    "|en|Deviation Hi alarm automatically acknowledged enabled" }, \
                           { 0x0040,  "|en|DevLo Alm Auto Ack Enabled",    "|en|Deviation Lo alarm automatically acknowledged enabled" }, \
                           { 0x0080,  "|en|Block Alm Auto Ack Enabled",    "|en|Block alarm automatically acknowledged enabled"        }

#define STATUS_OPTIONS_PID { 0x0001, [status_opt_ifs_bad_in],             [status_opt_ifs_bad_in_help]         }, \
                           { 0x0002, [status_opt_ifs_bad_cas_in],         [status_opt_ifs_bad_cas_in_help]     }, \
                           { 0x0004, [status_opt_uncertain_good],         [status_opt_uncertain_good_help]     }, \
                           { 0x0020, [status_opt_man_bad_in],             [status_opt_man_bad_in_help]         }, \
                           { 0x0200, [status_opt_trgt_nxt_bad_casin],     [status_opt_trgt_nxt_bad_casin_help] }, \
                           { 0x0400, "|en|Target to Man if BAD TRK_IN_D", "|en|Target to Man if BAD TRK_IN_D"  }, \
                           { 0x0800, "|en|IFS if BAD TRK_IN_D",           "|en|IFS if BAD TRK_IN_D"            }



#define LBL_ISFB    "|en|IS"
#define HLP_ISFB    "|en| The signal selector block provides selection of" \
                    "up to four inputs and generates an output based on the" \
                    "configured action."

#define LBL_ARFB    "|en|AR"
#define HLP_ARFB    "|en|The Arithmetic Function Block The AR block is" \
                    "intended for use in calculating measurements from" \
                    "combinations of signals from sensors."

#define LBL_MAI "|en|Multiple Analog Input"
      /* standard.dct: "mai Block", NI configurator displays capitals only => "B" */


/*
**********************************************************************
input selector block
**********************************************************************
*/
#define LBL_ISEL_DISABLE_VALUE         "|en|Value"
#define HLP_ISEL_DISABLE_VALUE         [blank]

#define ENUM_ISEL_DISABLE_USE          {0 ,"|en|Use",[blank]}
#define ENUM_ISEL_DISABLE_DISABLE      {1 ,"|en|Disable",[blank]}

#define ENUM_SELECT_TYPE_UNITIALIZED   {0 ,"|en|Uninitalized",[blank]}
#define ENUM_SELECT_TYPE_FIRST_GOOD    {1 ,"|en|First good",[blank]}
#define ENUM_SELECT_TYPE_MINIMUM       {2 ,"|en|Minimum",[blank]}
#define ENUM_SELECT_TYPE_MAXIMUM       {3 ,"|en|Maximum",[blank]}
#define ENUM_SELECT_TYPE_MIDDLE        {4 ,"|en|Middle",[blank]}
#define ENUM_SELECT_TYPE_AVERAGE       {5 ,"|en|Average",[blank]}

#define LBL_OP_SELECT_VALUE            "|en|Value"
#define HLP_OP_SELECT_VALUE            [blank]

#define LBL_SELECTED_VALUE             "|en|Value"
#define HLP_SELECTED_VALUE             [blank]

#define ENUM_OP_SELECT_0               {0 ,"|en|No selection", [blank] }
#define ENUM_OP_SELECT_1               {1 ,"|en|IN_1 selected", [blank] }
#define ENUM_OP_SELECT_2               {2 ,"|en|IN_2 selected", [blank] }
#define ENUM_OP_SELECT_3               {3 ,"|en|IN_3 selected", [blank] }
#define ENUM_OP_SELECT_4               {4 ,"|en|IN_4 selected", [blank] }


/*
**********************************************************************
Output Splitter Function Block
**********************************************************************
*/
#define LBL_OSFB  "|en|OS"
#define HLP_OSFB  "|en|The output splitter block provides the capability to" \
                  " drive two control outputs from a single input. Each output" \
                  " is a linear function of some portion of the input."


#define STATUS_OPTIONS_OS    { 0x0002, [status_opt_ifs_bad_cas_in],         [status_opt_ifs_bad_cas_in_help]     }, \
                             { 0x0200, [status_opt_trgt_nxt_bad_casin],     [status_opt_trgt_nxt_bad_casin_help] }


/*
**********************************************************************
arithmetic block
**********************************************************************
*/
#define ENUM_ARITH_TYPE_UNITIALIZED       {0 ,"|en|Uninitalized",[blank]}
#define ENUM_ARITH_TYPE_FLOW_COMP_LIN     {1 ,"|en|Flow compensation, linear",[blank]}
#define ENUM_ARITH_TYPE_FLOW_COMP_SQR     {2 ,"|en|Flow compensation, square root",[blank]}
#define ENUM_ARITH_TYPE_FLOW_COMP_APPROX  {3 ,"|en|Flow compensation, approximate",[blank]}
#define ENUM_ARITH_TYPE_BTU_FLOW          {4 ,"|en|BTU flow",[blank]}
#define ENUM_ARITH_TYPE_MULT_DIV          {5 ,"|en|Traditional Multiply Divide",[blank]}
#define ENUM_ARITH_TYPE_AVERAGE           {6 ,"|en|Average",[blank]}
#define ENUM_ARITH_TYPE_TRADIT_SUM        {7 ,"|en|Traditional Summer",[blank]}
#define ENUM_ARITH_TYPE_POLYNOM           {8 ,"|en|Fourth order polynomial",[blank]}
#define ENUM_ARITH_TYPE_HTG_COMP          {9 ,"|en|Simple HTG compensated level",[blank]}
#define ENUM_ARITH_TYPE_POLYNOM_2         {10 ,"|en|Fourth order polynomial 2",[blank]}

#define ENUM_AR_INPUT_OPT_IN_USE_UNCERT    { 0x0001 ,"|en|IN Use uncertain",[blank]}
#define ENUM_AR_INPUT_OPT_IN_LO_USE_UNCERT { 0x0002 ,"|en|IN_LO Use uncertain",[blank]}
#define ENUM_AR_INPUT_OPT_IN_1_USE_UNCERT  { 0x0004 ,"|en|IN_1 Use uncertain",[blank]}
#define ENUM_AR_INPUT_OPT_IN_1_USE_BAD     { 0x0008 ,"|en|IN_1 Use bad",[blank]}
#define ENUM_AR_INPUT_OPT_IN_2_USE_UNCERT  { 0x0010 ,"|en|IN_2 Use uncertain",[blank]}
#define ENUM_AR_INPUT_OPT_IN_2_USE_BAD     { 0x0020 ,"|en|IN_2 Use bad",[blank]}
#define ENUM_AR_INPUT_OPT_IN_3_USE_UNCERT  { 0x0040 ,"|en|IN_3 Use uncertain",[blank]}
#define ENUM_AR_INPUT_OPT_IN_3_USE_BAD     { 0x0080 ,"|en|IN_3 Use bad",[blank]}


/*
**********************************************************************
**********************************************************************
transducer blocks
**********************************************************************
**********************************************************************
*/

#define COMM_STATE_ENUMS { 0x00, "|en|No device communication.", [blank] }, \
                         { 0x01, "|en|Device communication OK.", [blank] }



#define HART_COMMAND_ENUMERATION {0, "|en|Ready", "|en|HART Tunnel ready for use"},\
                                 {1, "|en|Execute HART Command", "|en|Execute HART command defined within HART_Tunnel_Data"},\
                                 {2, "|en|Clear HART Tunnel Data", "|en|Set the HART_Tunnel_Data parameter back to initial values"},\
                                 {3, "|en|HART command execution successful", "|en|The HART command was executed successfully"},\
                                 {4, "|en|HART command execution not successful ", "|en|The HART command was executed successfully"}


/*
**********************************************************************
AI_transducer_block
**********************************************************************
*/

#define LBL_POSITIONER_TRANSDUCER_BLOCK   "|en|Positioner Transducer Block"
#define HLP_POSITIONER_TRANSDUCER_BLOCK   [blank]

/*
****** aitb_character ******
*/
#define LBL_PTB_CHARACTER   "|en|ptb_character"
#define HLP_PTB_CHARACTER   [blank]

/*
****** pv_unit ******
*/
#define LBL_PV_UNIT   "|en|PV Unit"
#define HLP_PV_UNIT   "|en|Unit of the primary value as delivered by the HART device"

/*
****** sv_unit ******
*/
#define LBL_SV_UNIT   "|en|SV Unit"
#define HLP_SV_UNIT   "|en|Unit of the secondary value as delivered by the HART device"

/*
****** hart_man_id ******
*/
#define LBL_HART_MAN_ID   "|en|HART Manufacturer ID"
#define HLP_HART_MAN_ID   "|en|Manufacturer-ID of the HART device"

/*
****** hart_dev_type ******
*/
#define LBL_HART_DEV_TYPE   "|en|HART Device Type"
#define HLP_HART_DEV_TYPE   "|en|Device-Type of the HART device"

/*
****** hart_dev_id ******
*/
#define LBL_HART_DEV_ID   "|en|HART Device ID"
#define HLP_HART_DEV_ID   "|en|Device-ID of the HART device"

/*
****** hart_dev_rev ******
*/
#define LBL_HART_DEV_REV   "|en|HART Device Revision"
#define HLP_HART_DEV_REV   "|en|Device-Revision of the HART device"

/*
****** hart_sw_rev ******
*/
#define LBL_HART_SW_REV   "|en|HART SW Revision"
#define HLP_HART_SW_REV   "|en|Software-Revision of the HART device"

/*
****** hart_hw_rev ******
*/
#define LBL_HART_HW_REV   "|en|HART HW Revision"
#define HLP_HART_HW_REV   "|en|Hardware-Revision of the HART device"

/*
****** hart_tag ******
*/
#define LBL_HART_TAG   "|en|HART Tag"
#define HLP_HART_TAG   "|en|Local tag of the HART device as read by CMD#13"

/*
****** hart_descr ******
*/
#define LBL_HART_DESCR   "|en|HART Description"
#define HLP_HART_DESCR   "|en|Local description of the HART device as read by CMD#13"

/*
****** hart_date ******
*/
#define LBL_HART_DATE   "|en|HART Date"
#define HLP_HART_DATE   "|en|Date information of the HART device as read by CMD#13"

/*
****** day ******
*/
#define LBL_DAY   "|en|Day"
#define HLP_DAY   "|en|Date: day of month in HART format"

/*
****** month ******
*/
#define LBL_MONTH   "|en|Month"
#define HLP_MONTH   "|en|Date: month in HART format"

/*
****** year ******
*/
#define LBL_YEAR   "|en|Year"
#define HLP_YEAR   "|en|Date: year in HART format starting at 1900"

/*
****** hart_cmd_major_rev ******
*/
#define LBL_HART_CMD_MAJOR_REV   "|en|HART Command Major Revision"
#define HLP_HART_CMD_MAJOR_REV   "|en|Universal command major revision number implemented by this device"

/*
****** ramp_value ******
*/
#define LBL_RAMP_VALUE   "|en|Ramp Value"
#define HLP_RAMP_VALUE   [blank]

/*
****** ramp_rate ******
*/
#define LBL_RAMP_RATE   "|en|Ramp Rate"
#define HLP_RAMP_RATE   [blank]

/*
****** ramp_min ******
*/
#define LBL_RAMP_MIN   "|en|Ramp Min"
#define HLP_RAMP_MIN   [blank]

/*
****** ramp_max ******
*/
#define LBL_RAMP_MAX   "|en|Ramp Max"
#define HLP_RAMP_MAX   [blank]

/*
****** rect_value ******
*/
#define LBL_RECT_VALUE   "|en|Rect Value"
#define HLP_RECT_VALUE   [blank]

/*
****** rect_period ******
*/
#define LBL_RECT_PERIOD   "|en|Rect Period"
#define HLP_RECT_PERIOD   [blank]

/*
****** rect_min ******
*/
#define LBL_RECT_MIN   "|en|Rect min"
#define HLP_RECT_MIN   [blank]

/*
****** rect_max ******
*/
#define LBL_RECT_MAX   "|en|Rect max"
#define HLP_RECT_MAX   [blank]

/*
****** const_value ******
*/
#define LBL_CONST_VALUE   "|en|Constant Value"
#define HLP_CONST_VALUE   [blank]

/*
****** comm_state ******
*/
#define LBL_COMM_STATE "|en|Communication State"
#define HLP_COMM_STATE [blank]


/*
**********************************************************************
VIEWs
**********************************************************************
*/

#define LBL_VIEW_1_LIST  "|en|View 1"
#define LBL_VIEW_2_LIST  "|en|View 2"
#define LBL_VIEW_3_LIST  "|en|View 3"
#define LBL_VIEW_4_LIST  "|en|View 4"

/*
****** ptb_view_1 ******
*/
#define LBL_PTB_VIEW_1   "|en|ptb_view_1"
#define HLP_PTB_VIEW_1   "|en|"

/*
****** ptb_view_2 ******
*/
#define LBL_PTB_VIEW_2   "|en|ptb_view_2"
#define HLP_PTB_VIEW_2   "|en|"

/*
****** ptb_view_3 ******
*/
#define LBL_PTB_VIEW_3   "|en|ptb_view_3"
#define HLP_PTB_VIEW_3   "|en|"

/*
****** ptb_view_4 ******
*/
#define LBL_PTB_VIEW_4   "|en|ptb_view_4"
#define HLP_PTB_VIEW_4   "|en|"


/*
****** New defines ******
*/


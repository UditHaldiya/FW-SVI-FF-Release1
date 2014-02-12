#ifndef IPCVARIDS_H_
#define IPCVARIDS_H_

//lint ++flb

//---------------------------------------------------
#define TB_SET_TO_FACTORY_CALIBRATION_SIGNATURE     (88u)

//---------------------------------------------------
// Variable IDs

typedef enum IPC_Variable_IDs_t
{
    IPC_VARIABLE_ID_NOTUSED                     = 0u,

    //-------------------------------------------------------------------------
    IPC_RB_BLOCK_TAG,                           // RB Block Tag,
    IPC_RB_BLOCK_ACTUAL_MODE,                   // RB Block Actual Mode
    IPC_RB_BLOCK_ERROR,                         // RB Block Error
    IPC_RB_RESTART,
    IPC_RB_NV_CYCLE_T,
    IPC_RB_BLOCK_ERR_DESC_1,
    IPC_RB_WRONG_CFG,

    IPC_RB_FAULT_STATE,
    IPC_RB_SET_FSTATE,
    IPC_RB_CLR_FSTATE,

    //-------------------------------------------------------------------------
    IPC_TB_BLOCK_TAG,                           // TB Block Tag,
    IPC_TB_BLOCK_ACTUAL_MODE,                   // TB Block Actual Mode -- May not be implemented
    IPC_TB_BLOCK_TARGET_MODE,                   // TB Block Target Mode -- May not be implemented
    IPC_TB_FINAL_POS_VALUE,                     // Index 17
    IPC_TB_FINAL_VALUE,                         // FINAL_VALUE (SP) -- Used via DDE, and may not be implemented
    IPC_APP_MODE,                               // R/W!! Application uC Mode (IN SVI II AP format)
    IPC_TB_BLOCK_ERROR,                         // TB Block Error
    IPC_TB_ST_REV,                              // TB Static Revision

    IPC_TB_FINAL_SP,
    IPC_TB_FINAL_POS,
    IPC_TB_UI_CUST_CONF_0,
    IPC_TB_UI_CUST_CONF_1,

    //-------------------------------------------------------------------------
    IPC_AO_BLOCK_TAG,                           // AO Block Tag
    IPC_AO_BLOCK_ACTUAL_MODE,                   // AO Block Actual Mode
    IPC_AO_BLOCK_ERROR,                         // AO Block Error
    IPC_AO_SP,

    //-------------------------------------------------------------------------
    IPC_AI_OUT,

    //-------------------------------------------------------------------------
    IPC_AI2_OUT,

    //-------------------------------------------------------------------------
    IPC_AI3_OUT,

    //-------------------------------------------------------------------------
    IPC_DO_BLOCK_TAG,                           // DO Block Tag
    IPC_DO_BLOCK_ACTUAL_MODE,                   // DO Block Actual Mode
    IPC_DO_BLOCK_ERROR,                         // DO Block Error

    IPC_DO_SP_D,

    //-------------------------------------------------------------------------
    IPC_DO2_SP_D,

    //-------------------------------------------------------------------------
    IPC_IS_IN1,
    IPC_IS_IN2,
    IPC_IS_IN3,
    IPC_IS_IN4,

    //-------------------------------------------------------------------------
    IPC_PID_BLOCK_TAG,                          // PID Block Tag
    IPC_PID_BLOCK_ACTUAL_MODE,                  // PID Block Actual Mode
    IPC_PID_BLOCK_ERROR,                        // PID Block Error

    IPC_PID_PV,
    IPC_PID_SP,
    IPC_PID_OUT,

    //--------------------------------------------------------------------------
    IPC_PID2_PV,
    IPC_PID2_SP,
    IPC_PID2_OUT,

    //-------------------------------------------------------------------------
    IPC_DEVICE_ID,                              // DEVICE_ID () - 32 char string
    IPC_DEVICE_TAG,                             // Device Tag - 32-char string
    IPC_DEVICE_ADDRESS,                         // u32

    //-------------------------------------------------------------------------
    IPC_XD_ERROR_POSITION,                      // XD_ERROR_POSITION
    IPC_POS_CAL_RESULT,                         // TRAVEL_CALIBRATION.LAST_RESULT
    IPC_CAL_POINT_HI,                           // Index 25.4  IPC_STOP_HI_POS
    IPC_CAL_POINT_LO,                           // Index 25.5  IPC_STOP_LO_POS
    IPC_CAL_TYPE,                               // Index 25.6
    IPC_WORKING_SP,                             // Index 26
    IPC_WORKING_POS,                            // Index 27

    IPC_FINAL_CUTOFF_HI_ENABLE,                 // FINAL_VALUE_CUTOFF_HI.ENABLE
    IPC_FINAL_CUTOFF_HI_POINT,                  // FINAL_VALUE_CUTOFF_HI.POINT
    IPC_FINAL_CUTOFF_LO_ENABLE,                 // FINAL_VALUE_CUTOFF_LO.ENABLE
    IPC_FINAL_CUTOFF_LO_POINT,                  // FINAL_VALUE_CUTOFF_LO.POINT

    //-------------------------------------------------------------------------
    IPC_XD_FSTATE_OPT,                          // Index 43.1
    IPC_FSTATE_VALUE,                           // Index 43.2
    IPC_FSTATE_TIME,                            // Index 43.3

    IPC_CHARACT_NUMPOINTS,                      // Index 44.2

    //-------------------------------------------------------------------------
    IPC_DISCRETE_SWITCH_1,                      // Index 75
    IPC_DISCRETE_SWITCH_2,                      // Index 76

    //-------------------------------------------------------------------------
    IPC_TRVL_ACCUM_A,                           // Index 33.1
    IPC_TRVL_ACCUM_B,                           // Index 34.1
    IPC_TRVL_ACCUM_A_DEADBAND,                  // TRAVEL_ACCUMULATION_A_ALERT.DEADBAND
    IPC_TRVL_ACCUM_B_DEADBAND,                  // TRAVEL_ACCUMULATION_B_ALERT.DEADBAND

    IPC_TRVL_ACCTR_TODAY,                       // Index 35.1
    IPC_TRVL_ACCTR_DAY1,                        // Index 35.2
    IPC_TRVL_ACCTR_DAY2,                        // Index 35.3
    IPC_TRVL_ACCTR_DAY3,
    IPC_TRVL_ACCTR_DAY4,
    IPC_TRVL_ACCTR_DAY5,
    IPC_TRVL_ACCTR_DAY6,
    IPC_TRVL_ACCTR_DAY7,
    IPC_TRVL_ACCTR_CUR15,                       // Index 35.4
    IPC_TRVL_ACCTR_LAST15,                      // Index 35.5
    IPC_TRVL_ACCTR_PREV15,                      // Index 35.6
    IPC_TRVL_ACCTR_CURMONTH,                    // Index 35.7
    IPC_TRVL_ACCTR_MONTH1,                      // Index 35.8
    IPC_TRVL_ACCTR_MONTH2,                      // Index 35.8
    IPC_TRVL_ACCTR_MONTH3,                      // Index 35.8
    IPC_TRVL_ACCTR_MONTH4,                      // Index 35.8
    IPC_TRVL_ACCTR_MONTH5,                      // Index 35.8
    IPC_TRVL_ACCTR_MONTH6,                      // Index 35.8
    IPC_TRVL_ACCTR_MONTH7,                      // Index 35.8
    IPC_TRVL_ACCTR_MONTH8,                      // Index 35.8
    IPC_TRVL_ACCTR_MONTH9,                      // Index 35.8
    IPC_TRVL_ACCTR_MONTH10,                     // Index 35.8
    IPC_TRVL_ACCTR_MONTH11,                     // Index 35.8
    IPC_TRVL_ACCTR_MONTH12,                     // Index 35.8
    IPC_TRVL_ACCTR_CUR12M,                      // Index 35.10
    IPC_TRVL_ACCTR_YEAR1,                       // Index 35.11
    IPC_TRVL_ACCTR_YEAR2,                       // Index 35.11
    IPC_TRVL_ACCTR_YEAR3,                       // Index 35.11
    IPC_TRVL_ACCTR_YEAR4,                       // Index 35.11
    IPC_TRVL_ACCTR_YEAR5,                       // Index 35.11
    IPC_TRVL_ACCTR_TOTAL,                       // Index 35.12

    //--------------------------------------------------------------
    IPC_CYCLE_CTR_A,                            // Index 36.1
    IPC_CYCLE_CTR_B,                            // Index 37.1
    IPC_CYCLE_CTR_A_DEADBAND,                   // CYCLE_COUNTER_A_ALERT.DEADBAND
    IPC_CYCLE_CTR_B_DEADBAND,                   // CYCLE_COUNTER_B_ALERT.DEADBAND

    IPC_CYCLETR_TODAY,                          // CYCLE_COUNTER_TRENDS
    IPC_CYCLETR_DAY1,                           //
    IPC_CYCLETR_DAY2,                           //
    IPC_CYCLETR_DAY3,
    IPC_CYCLETR_DAY4,
    IPC_CYCLETR_DAY5,
    IPC_CYCLETR_DAY6,
    IPC_CYCLETR_DAY7,
    IPC_CYCLETR_CUR15,                          //
    IPC_CYCLETR_LAST15,                         //
    IPC_CYCLETR_PREV15,                         //
    IPC_CYCLETR_CURMONTH,                       //
    IPC_CYCLETR_MONTH1,                         //
    IPC_CYCLETR_MONTH2,                         //
    IPC_CYCLETR_MONTH3,                         //
    IPC_CYCLETR_MONTH4,                         //
    IPC_CYCLETR_MONTH5,                         //
    IPC_CYCLETR_MONTH6,                         //
    IPC_CYCLETR_MONTH7,                         //
    IPC_CYCLETR_MONTH8,                         //
    IPC_CYCLETR_MONTH9,                         //
    IPC_CYCLETR_MONTH10,                        //
    IPC_CYCLETR_MONTH11,                        //
    IPC_CYCLETR_MONTH12,                        //
    IPC_CYCLETR_CUR12M,                         //
    IPC_CYCLETR_YEAR1,                          //
    IPC_CYCLETR_YEAR2,                          //
    IPC_CYCLETR_YEAR3,                          //
    IPC_CYCLETR_YEAR4,                          //
    IPC_CYCLETR_YEAR5,                          //
    IPC_CYCLETR_TOTAL,                          //

    //-------------------------------------------------------------------------
    IPC_XD_ERROR_PRESSURE,                      // XD_ERROR_PRESSURE
    IPC_PRESSURE_SUPPLY,                        // Index 49
    IPC_PRESSURE_ACTUATOR_A,                    // Index 54
    IPC_PRESSURE_ACTUATOR_B,                    // Index 55
    IPC_PRESSURE_ATMOS,                         // Not mentioned in the Table
    IPC_PRESSURE_PILOT,                         // Not mentioned in the Table
    IPC_PRESSURE_UNITS,                         // Index 49.3
    IPC_XD_ERROR_TEMPERATURE,                   // XD_ERROR_TEMPERATURE
    IPC_TEMPERATURE,                            // Index 57
    IPC_IP_CURRENT,                             // Index 60.1

    //-------------------------------------------------------------------------
    IPC_COMPLETE_STATUS_CUR_LO,                 // Index 84
    IPC_COMPLETE_STATUS_CUR_HI,                 // Index 84
    IPC_COMPLETE_STATUS_HIST_LO,                // Index 84
    IPC_COMPLETE_STATUS_HIST_HI,                // Index 84

    IPC_SET_TO_FACTORY_CALIB,                   // Index 56

    //-------------------------------------------------------------------------
    IPC_ACTUATOR_SENSOR_INCREASE,               // ACTUATOR.SENSOR_INCREASE -- Indicates if the Sensor is Clock- or CounterClock- Wise

    //-------------------------------------------------------------------------
    IPC_WORK_TIME_RESET,                        // WORKING_TIME.SINCE_RESET
    IPC_WORK_TIME_POWER,                        // WORKING_TIME.SINCE_POWER_UP
    IPC_WORK_TIME_LIFETIME,                     // WORKING_TIME.LIFETIME

    //-------------------------------------------------------------------------
    IPC_WRONG_CONFIG_PARAM_ID,                  // May not be implemented -- WRONG_CONFIGURATION.PARAMETER_ID
    IPC_WRONG_CONFIG_DESC,                      // May not be implemented -- WRONG_CONFIGURATION.DESCRIPTION - 32char string

    //-------------------------------------------------------------------------
    IPC_PID2_BLOCK_TAG,
    IPC_PID2_BLOCK_ACTUAL_MODE,
    IPC_PID2_BLOCK_ERROR,

    //-------------------------------------------------------------------------
    IPC_DO2_BLOCK_TAG,
    IPC_DO2_BLOCK_ACTUAL_MODE,
    IPC_DO2_BLOCK_ERROR,

    //-------------------------------------------------------------------------
    // SERVO_WARN_COUNT -- Verify - may be expanded

    IPC_COMPLETE_STATUS_CUR_HIHI,               // Index 84
    IPC_COMPLETE_STATUS_HIST_HIHI,              // Index 84

    //-------------------------------------------------------------------------
    // Discrete input

    IPC_DISCRETE_INPUT,                         // DI from APP

    //-------------------------------------------------------------------------
    // Raw position

    IPC_RAW_POSITION_SENSOR,                    // RAW readings from Position Sensor

    IPC_APP_FWVERSION,
    IPC_STOP_HI_POS,
    IPC_STOP_LO_POS,
    IPC_LUI_SIMULATION,

    //-------------------------------------------------------------------------
    // The following ENUMs should remain at the end of the list!
    // New IPC Variable IDs should be added ABOVE.
    IPC_VARID_TEST,
    IPC_VARID_MAX                               // MAX Var ID for ID Validation

} IPC_Variable_IDs_t;

//---------------------------------------------------
#define IPC_BIT_DI_STATE                        (1u)
#define IPC_BIT_WP_STATE                        (2u)

//---------------------------------------------------
// The end
//lint --flb

#endif //IPCVARIDS_H_

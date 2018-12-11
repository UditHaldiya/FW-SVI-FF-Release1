/* ===========================================================================

FILE_NAME          fdiag_FBK2_Start.h


This file contains DD labels and DD help strings for Field Diagnositics parameters

=========================================================================== */
#ifdef FD_USED
#define FD_ELEMENTS(x) x 
#else
#define FD_ELEMENTS(x)  
#endif

#ifdef DD4
#define fd_ver_label                    "|en|Field Diagnostics Revision"
#define fd_ver_help                     "|en|Field Diagnostics Revision"

#define fd_fail_map_label               "|en|Failed Map"
#define fd_fail_map_help                "|en|Failed Map"
#define fd_fail_active_label            "|en|Failed Active"
#define fd_fail_active_help             "|en|Failed Active"
#define fd_fail_mask_label              "|en|Failed Mask"
#define fd_fail_mask_help               "|en|Failed Mask"
#define fd_fail_alm_label               "|en|Failed Alarm"
#define fd_fail_alm_help                "|en|Failed Alarm"
#define fd_fail_pri_label               "|en|Failed Priority"
#define fd_fail_pri_help                "|en|Failed Priority"

#define fd_offspec_map_label            "|en|Off Specification Map"
#define fd_offspec_map_help             "|en|Off Specification Map"
#define fd_offspec_active_label         "|en|Off Specification Active"
#define fd_offspec_active_help          "|en|Off Specification Active"
#define fd_offspec_mask_label           "|en|Off Specification Mask"
#define fd_offspec_mask_help            "|en|Off Specification Mask"
#define fd_offspec_alm_label            "|en|Off Specification Alarm"
#define fd_offspec_alm_help             "|en|Off Specification Alarm"
#define fd_offspec_pri_label            "|en|Off Specification Priority"
#define fd_offspec_pri_help             "|en|Off Specification Priority"

#define fd_maint_map_label              "|en|Maintenance Map"
#define fd_maint_map_help               "|en|Maintenance Map"
#define fd_maint_active_label           "|en|Maintenance Active"
#define fd_maint_active_help            "|en|Maintenance Active"
#define fd_maint_mask_label             "|en|Maintenance Mask"
#define fd_maint_mask_help              "|en|Maintenance Mask"
#define fd_maint_alm_label              "|en|Maintenance Alarm"
#define fd_maint_alm_help               "|en|Maintenance Alarm"
#define fd_maint_pri_label              "|en|Maintenance Priority"
#define fd_maint_pri_help               "|en|Maintenance Priority"

#define fd_check_map_label              "|en|Check Function Map"
#define fd_check_map_help               "|en|Check Function Map"
#define fd_check_active_label           "|en|Check Function Active"
#define fd_check_active_help            "|en|Check Function Active"
#define fd_check_mask_label             "|en|Check Function Mask"
#define fd_check_mask_help              "|en|Check Function Mask"
#define fd_check_alm_label              "|en|Check Function Alarm"
#define fd_check_alm_help               "|en|Check Function Alarm"
#define fd_check_pri_label              "|en|Check Function Priority"
#define fd_check_pri_help               "|en|Check Function Priority"

#define fd_simulate_label               "|en|Field Diagnostics Simulate"
#define fd_simulate_help                "|en|Field Diagnostics Simulate"
#define diagnostic_simulate_value_label "|en|Diagnostic Simulate Value"
#define diagnostic_simulate_value_help  "|en|Diagnostic Simulate Value"
#define diagnostic_value_label          "|en|Diagnostic Value"
#define diagnostic_value_help           "|en|Diagnostic Value"
#define fd_recommended_action_label     "|en|Recommended Action"
#define fd_recommended_action_help      "|en|Recommended Action"

#else  /*DD4*/

#define fd_ver_label                    "|en|Field Diagnostics Revision"
#define fd_ver_help                     "|en|The major version of the Field Diagnostics specification used for the development of this device."

#define fd_fail_map_label               "|en|Failed Map"
#define fd_fail_map_help                "|en|This parameter enables or disables conditions to be detected as active for the 'Failed' category"
#define fd_fail_active_label            "|en|Failed Active"
#define fd_fail_active_help             "|en|This parameter reflects the error conditions that are being detected as active as selected for the 'Failed' category. It is a bit string, so that multiple conditions may be shown."
#define fd_fail_mask_label              "|en|Failed Mask"
#define fd_fail_mask_help               "|en|This parameter is used to suppress any single or multiple conditions that are active, in the 'Failed' category, from being broadcasted through the Alarm parameter."
#define fd_fail_alm_label               "|en|Failed Alarm"
#define fd_fail_alm_help                "|en|This parameter is used to broadcast a change of an unmasked 'Failed' condition."
#define fd_fail_pri_label               "|en|Failed Priority"
#define fd_fail_pri_help                "|en|This parameter allows the user to specify the priority of the 'Failed' Alarm category."

#define fd_offspec_map_label            "|en|Off Specification Map"
#define fd_offspec_map_help             "|en|This parameter enables or disables conditions to be detected as active for the 'Off Specification' category"
#define fd_offspec_active_label         "|en|Off Specification Active"
#define fd_offspec_active_help          "|en|This parameter reflects the error conditions that are being detected as active as selected for the 'Off Specification' category. It is a bit string, so that multiple conditions may be shown."
#define fd_offspec_mask_label           "|en|Off Specification Mask"
#define fd_offspec_mask_help            "|en|This parameter is used to suppress any single or multiple conditions that are active, in the 'Off Specification' category, from being broadcasted through the Alarm parameter."
#define fd_offspec_alm_label            "|en|Off Specification Alarm"
#define fd_offspec_alm_help             "|en|This parameter is used to broadcast a change of an unmasked 'Off Specification' condition."
#define fd_offspec_pri_label            "|en|Off Specification Priority"
#define fd_offspec_pri_help             "|en|This parameter allows the user to specify the priority of the 'Off Specification' Alarm category."

#define fd_maint_map_label              "|en|Maintenance Map"
#define fd_maint_map_help               "|en|This parameter enables or disables conditions to be detected as active for the 'Maintenance' category"
#define fd_maint_active_label           "|en|Maintenance Active"
#define fd_maint_active_help            "|en|This parameter reflects the error conditions that are being detected as active as selected for the 'Maintenance' category. It is a bit string, so that multiple conditions may be shown."
#define fd_maint_mask_label             "|en|Maintenance Mask"
#define fd_maint_mask_help              "|en|This parameter is used to suppress any single or multiple conditions that are active, in the 'Maintenance' category, from being broadcasted through the Alarm parameter."
#define fd_maint_alm_label              "|en|Maintenance Alarm"
#define fd_maint_alm_help               "|en|This parameter is used to broadcast a change of an unmasked 'Maintenance' condition."
#define fd_maint_pri_label              "|en|Maintenance Priority"
#define fd_maint_pri_help               "|en|This parameter allows the user to specify the priority of the 'Maintenance' Alarm category."

#define fd_check_map_label              "|en|Check Function Map"
#define fd_check_map_help               "|en|This parameter enables or disables conditions to be detected as active for the 'Check Function' category"
#define fd_check_active_label           "|en|Check Function Active"
#define fd_check_active_help            "|en|This parameter reflects the error conditions that are being detected as active as selected for the 'Check Function' category. It is a bit string, so that multiple conditions may be shown."
#define fd_check_mask_label             "|en|Check Function Mask"
#define fd_check_mask_help              "|en|This parameter is used to suppress any single or multiple conditions that are active, in the 'Check Function' category, from being broadcasted through the Alarm parameter."
#define fd_check_alm_label              "|en|Check Function Alarm"
#define fd_check_alm_help               "|en|This parameter is used to broadcast a change of an unmasked 'Check Function' condition."
#define fd_check_pri_label              "|en|Check Function Priority"
#define fd_check_pri_help               "|en|This parameter allows the user to specify the priority of the 'Check Function' Alarm category."

#define fd_simulate_label               "|en|Field Diagnostics Simulate"
#define fd_simulate_help                "|en|When simulation is enabled the Field Diagnostics conditions are taken from 'Diagnostic Simulate Value' otherwise the conditions are taken from 'Diagnostic Value'."
#define diagnostic_simulate_value_label "|en|Diagnostic Simulate Value"
#define diagnostic_simulate_value_help  "|en|Allows the conditions to be manually supplied when simuation is enabled. The simulate jumper is required for simulation to be enabled."
#define diagnostic_value_label          "|en|Diagnostic Value"
#define diagnostic_value_help           "|en|Current Field Diagnostic conditions."
#define fd_recommended_action_label     "|en|Recommended Action"
#define fd_recommended_action_help      "|en|This parameter is a device enumerated summarization of the most severe condition or conditions detected. The DD should describe by enumerated action, what should be done to alleviate the condition or conditions."

#endif /*DD4*/


/* -------------------------------------------------------------------- */
/* The following enumerations require a device specific re-definition ! */
/* -------------------------------------------------------------------- */

#define FD_CONDITIONS_ENUM \
    { 0x00000001,  "|en|HART communication lost",                 HLP5("|en|HART communication lost") },\
    { 0x00000002,  "|en|Device Needs Maintenance Now",            HLP5("|en|Device Needs Maintenance Now") },\
    { 0x00000004,  "|en|Lost NV Data",                            HLP5("|en|Lost NV Data") },\
    { 0x00000008,  "|en|Lost Static Data",                        HLP5("|en|Lost Static Data") },\
    { 0x00000010,  "|en|Memory Failure",                          HLP5("|en|Memory Failure") },\
    { 0x00000020,  "|en|Output Failure Detected",                 HLP5("|en|Output Failure Detected") },\
    { 0x00000040,  "|en|Device Needs Maintenance Soon",           HLP5("|en|Device Needs Maintenance Soon") },\
    { 0x00000080,  "|en|Block Configuration Error",               HLP5("|en|Block Configuration Error") },\
    { 0x00000100,  "|en|Other",                                   HLP5("|en|Other") },\
    { 0x00000200,  "|en|Faults reported in Detected_Faults only", HLP5("|en|Faults reported in Detected_Faults only") },\
    { 0x80000000,  "|en|Transducer Block does not work properly", HLP5("|en|Transducer Block does not work properly") }
                                                                  


#define FD_CONDITIONS_DISABLED_ENUM \
    { 0x00000001,  "|en|HART communication lost disabled",                 HLP5("|en|HART communication lost disabled") },\
    { 0x00000002,  "|en|Device Needs Maintenance Now Disabled",            HLP5("|en|Device Needs Maintenance Now disabled") },\
    { 0x00000004,  "|en|Lost NV Data Disabled",                            HLP5("|en|Lost NV Data disabled") },\
    { 0x00000008,  "|en|Lost Static Data Disabled",                        HLP5("|en|Lost Static Data disabled") },\
    { 0x00000010,  "|en|Memory Failure Disabled",                          HLP5("|en|Memory Failure disabled") },\
    { 0x00000020,  "|en|Output Failure Detected Disabled",                 HLP5("|en|Output Failure Detected disabled") },\
    { 0x00000040,  "|en|Device Needs Maintenance Soon Disabled",           HLP5("|en|Device Needs Maintenance Soon disabled") },\
    { 0x00000080,  "|en|Block Configuration Error Disabled",               HLP5("|en|Block Configuration Error disabled") },\
    { 0x00000100,  "|en|Other Disabled",                                   HLP5("|en|Other disabled") },\
    { 0x00000200,  "|en|Faults reported in Detected_Faults only Disabled", HLP5("|en|Faults reported in Detected_Faults only disabled") },\
    { 0x80000000,  "|en|Transducer Block does not work properly Disabled", HLP5("|en|Transducer Block does not work properly disabled ") }
                                                                           

/* Note: the codes 0 and 1 are specified by FF-912, all other codes need a device-specific definitond */

#define FD_RECOMMEN_ACT_ENUM \
    { 0,   "|en|Not Initialized",               HLP5("|en|The Recommended Action parameter is not initialized") },\
    { 1,   "|en|No Action Required",            HLP5("|en|Device works fine no action required") },\
    { 2,   "|en|Remove Simulate Jumper!",       HLP5("|en|Simulate Jumper is installed and must be removed to return to normal operation") },\
    { 3,   "|en|Set Transducer Block into AUTO mode",       HLP5("|en|Transducer Block does not work properly") },\
    { 4,   "|en|Repair communication with connected Slave", HLP5("|en|Repair communication with connected Slave") },\
    { 5,   "|en|Check RESET",                   HLP5("|en|Any reset except caused by RAM checksum error or stack overflow or per trap configuration (ESD and later AP)") },\
    { 6,   "|en|Check LOW_POWER",               HLP5("|en|Input current < 3.2 mA") },\
    { 7,   "|en|Check ACTUATOR",                HLP5("|en|Positioner is unable to position a valve normally") },\
    { 8,   "|en|Check AIR_SUPPLY_LOW",          HLP5("|en|If supply pressure option is configured AND supply pressure < X psi, otherwise, if the I/P pressure is below 0.8 psi") },\
    { 9,   "|en|Check POSITION_ERROR",          HLP5("|en|If T1 error is configured AND the position error outside the error band for T1") },\
    { 10,  "|en|Check I2CBUS",                  HLP5("|en|Low supply pressure alarm limit is configured too low") },\
    { 11,  "|en|Check KEYPAD",                  HLP5("|en|If a non-existent pushbutton appears pressed (can be e.g. with water in there)") },\
    { 12,  "|en|Check MARGINAL_POWER",          HLP5("|en|When raw A/D signal read is below a factory-configurable value but above low power") },\
    { 13,  "|en|Check CALIBRATION_FAILED",      HLP5("|en|If calibration of AO or pressure fails") },\
    { 14,  "|en|Check FIND_STOPS_FAILED",       HLP5("|en|If the find stops process fails for any reason") },\
    { 15,  "|en|Check AUTOTUNE_FAILED",         HLP5("|en|If the self-tune process fails for any reason") },\
    { 16,  "|en|Check STD_DIAGNOSTICS_FAILED",  HLP5("|en|If the std diagnostics process fails for any reason (AP).Not Implemented (ESD)") },\
    { 17,  "|en|Check EXT_DIAGNOSTICS_FAILED",  HLP5("|en|If the any ext diagnostics process fails for any reason (AP). Not Implemented (ESD)") },\
    { 18,  "|en|Check RTOS_SCHEDULING",         HLP5("|en|If any RTOS task overruns itself.(Not in AP 311)") },\
    { 19,  "|en|Check SUPPLY_HIGH",             HLP5("|en|Supply pressure above range") },\
    { 20,  "|en|Check detected_faults bit 15",  HLP5("|en|DETECTED FAULTS BIT 15") },\
    { 21,  "|en|Check BIAS_OUT_OF_RANGE",       HLP5("|en|If BIAS is outside alarm limits [10,000, 30,000]") },\
    { 22,  "|en|Check IP_OUT_OF_RANGE",         HLP5("|en|If temperature compensated I/P current is out of range [-100, 30000] counts for 5 reads in a row (SVI2A) [-.1, 2.1] mA (ESD)") },\
    { 23,  "|en|Check TEMPR_OUT_OF_RANGE",      HLP5("|en|[-40, 85] deg.C") },\
    { 24,  "|en|Check DI_ABNORMAL",             HLP5("|en|DI_ABNORMAL") },\
    { 25,  "|en|Check CONF_1",                  HLP5("|en|Low supply pressure alarm limit is configured too low") },\
    { 26,  "|en|Check LATCH_FSAFE",             HLP5("|en|LATCH_FSAFE") },\
    { 27,  "|en|Check TIGHT_SHUTOFF",           HLP5("|en|TIGHT_SHUTOFF") },\
    { 28,  "|en|Check detected_faults bit 23",  HLP5("|en|DETECTED FAULTS BIT 23") },\
    { 29,  "|en|Check NVM_CHECKSUM0",           HLP5("|en|If an FRAM record and its copy both have CRC errors (as detected by read on initialization,)  OR if the temperature calibration table has not been written in its entirety (detected by CRC of column CRCs) OR bad FRAM signature") },\
    { 30,  "|en|Check RAM_CHECKSUM",            HLP5("|en|A valid hidden record (in RAM) existing upon reset indicating that a RAM data item had a bad checksum, as detected by on-request data test") },\
    { 31,  "|en|Check Check FW_CHECKSUM",       HLP5("|en|Flash CRC test fails. (AP 311 only: The test runs only on (any) reset) The test takes about 4-10 seconds roundtrip") },\
    { 32,  "|en|Check STACK",                   HLP5("|en|A valid hidden record (in RAM) existing upon reset indicating that a stack overflow had occurred, as detected by the task switching code") },\
    { 33,  "|en|Check FACTORYWRITE",            HLP5("|en|Raw write to FRAM") },\
    { 34,  "|en|Check NVM_TEST",                HLP5("|en|If an FRAM record and its copy both have CRC errors (as detected by read by a background test)") },\
    { 35,  "|en|Check detected_faults bit 30",  HLP5("|en|DETECTED FAULTS BIT 30") },\
    { 36,  "|en|Check FACTORYMODE",             HLP5("|en|Enables factory commands") },\
    { 37,  "|en|Check REF_VOLTAGE",             HLP5("|en|If temperature compensated reference voltage is out of range [0, 30000] counts for 5 reads in a row, or if the raw reading is outside the range +-5% of nominal") },\
    { 38,  "|en|Check POSITION_SENSOR",         HLP5("|en|If upon request temperature compensated remote position sensor reading is outside the range [ 2.5, 120.0-2.5] degrees or internal position sensor reading is outside the range [ 70.0, 70.0] Gauss for 5 reads in a row") },\
    { 39,  "|en|Check CURRENT_SENSOR",          HLP5("|en|If, upon request, temperature compensated current sensor reading is outside the range [ 100, 30.0] mA for 5 reads in a row") },\
    { 40,  "|en|Check TEMPERATURE_SENSOR",      HLP5("|en|If, upon request, temperature compensated temperature sensor reading is outside the range [ 60.0, 100.0] C for 5 reads in a row") },\
    { 41,  "|en|Check CURRENT_SENSOR_2ND",      HLP5("|en|If, upon request, temperature compensated 2nd current sensor reading is outside the range [ 0.1, 30.0] mA for 5 reads in a row (SVI2A)") },\
    { 42,  "|en|Check PRESSURE1",               HLP5("|en|If, upon request, temperature compensated pressure sensor 1 reading is outside the range [ 1250, 32000] counts for 5 reads in a row (SVI2AP 311) OR   [-0.5, 155.0] Psi (ESD)") },\
    { 43,  "|en|Check PRESSURE2",               HLP5("|en|If, upon request, temperature compensated pressure sensor 2 reading is outside the range [ 1250, 32000] counts for 5 reads in a row (SVI2AP 311) OR   [-0.5, 155.0] Psi (ESD)") },\
    { 44,  "|en|Check PRESSURE3",               HLP5("|en|If, upon request, temperature compensated pressure sensor 3 (supply) reading is outside the range [ 1250, 32000] counts for 5 reads in a row (SVI2AP 311) OR   [-0.5, 155.0] Psi (ESD)") },\
    { 45,  "|en|Check PRESSURE4",               HLP5("|en|If, upon request, temperature compensated pressure sensor 4 (pilot) reading is outside the range [ 1250, 15000] counts for 5 reads in a row (SVI2AP 311) OR   [-0.5, 155.0] Psi (ESD)") },\
    { 46,  "|en|Check PRESSURE5",               HLP5("|en|If, upon request, temperature compensated pressure sensor 5 (atmospheric) reading is outside the range [ 2500, 2500] counts for 5 reads in a row (SVI2AP 311) OR   [-10.0, 10.0] Psi (ESD)") },\
    { 47,  "|en|Check WATCHDOG_TIMEOUT",        HLP5("|en|Stored upon reset") },\
    { 48,  "|en|Check NVM_WRITE",               HLP5("|en|If write to FRAM fails OR if data repairing in FRAM fails") },\
    { 49,  "|en|Check IRQ_FAULT",               HLP5("|en|A valid hidden record (in RAM) existing upon reset indicating that an illegal interrupt occurred") },\
    { 50,  "|en|Check TMOUT_FLASH_TEST",        HLP5("|en|If a round of flash test is not completed in 2 hrs") },\
    { 51,  "|en|Check SELF_CHECK",              HLP5("|en|A valid hidden record (in RAM) existing upon reset indicating that a fatal event (watchdog, illegal interrupt, stack overflow, data checksum) occurred twice in a N-second period. (AP 311 N=10; ESD N= 20)") },\
    { 52,  "|en|Check SOFTWARE",                HLP5("|en|A catch-all not-otherwise-defined fault. Currently: A valid hidden record (in RAM) existing upon reset indicating that a CPU exception (such as invalid instruction) occurred, OR invalid target mode of the device was found") },\
    { 53,  "|en|Check ESD_TRIP",                HLP5("|en|If I/P feedback is below threshold, position and pressure are deemed stable, actuator1 pressure is below threshold") },\
    { 54,  "|en|Check AI_PV_LOW",               HLP5("|en|PV voltage too low") },\
    { 55,  "|en|Check AI_PV_HIGH",              HLP5("|en|PV voltage too high") },\
    { 56,  "|en|Check IP_SERVO_LOW",            HLP5("|en|IP is below its defined pressure range") },\
    { 57,  "|en|Check IP_SERVO_HIGH",           HLP5("|en|IP is above its defined pressure range") },\
    { 58,  "|en|Check detected_faults bit 53",  HLP5("|en|DETECTED FAULTS BIT 53") },\
    { 59,  "|en|Check DIAGPROC1",               HLP5("|en|DIAGPROC1") },\
    { 60,  "|en|Check RANGE1",                  HLP5("|en|RANGE1") },\
    { 61,  "|en|Check RANGE2",                  HLP5("|en|RANGE2") },\
    { 62,  "|en|Check LINKAGE_DRIFT",           HLP5("|en|The measured position is more than LinkageLimit above/below expected") },\
    { 63,  "|en|Check VALVE_STUCK_CLOSED",      HLP5("|en|Stable & (SP > 99.5%) and (Position < StuckLimit)") },\
    { 64,  "|en|Check VALVE_STUCK_OPENED",      HLP5("|en|stable & (SP < .5%) and (Position > StuckLimit)") },\
    { 65,  "|en|Check UI_OFF",                  HLP5("|en|UI is turned off because it is not responsive at low (main board) temperature, -10 deg.C") },\
    { 66,  "|en|Check RANGE3",                  HLP5("|en|RANGE3") },\
    { 67,  "|en|Check RANGE4",                  HLP5("|en|RANGE4") },\
    { 68,  "|en|Unknown recommended action",    HLP5("|en|Unknown recommended action") }





/* -------------------------------------------------------------------- */
/* Redefines for a Resource Block ALARM_SUM and ACK_OPTION parameter    */
/* -------------------------------------------------------------------- */

#define FD_CURR_SUMM_ENUM \
    { 0x0001, "|en|Discrete Alarm",    "|en|Discrete Alarm active"                               }, \
    { 0x0080, "|en|Block Alarm",    "|en|Block Alarm Active"                               }, \
    { 0x0100, "|en|Fail Alarm",     "|en|Field Diagnostics Failed Alarm Active"            }, \
    { 0x0200, "|en|OffSpec Alarm",  "|en|Field Diagnostics Off Specification Alarm Active" }, \
    { 0x0400, "|en|Maint Alarm",    "|en|Field Diagnostics Maintenance Alarm Active"       }, \
    { 0x0800, "|en|Check Alarm",    "|en|Field Diagnostics Check Function Alarm Active"    }

#define FD_UNACK_SUMM_ENUM \
    { 0x0001, "|en|Discrete Alarm Unack",   "|en|Discrete Alarm not acknowledged by operator"                               }, \
    { 0x0080, "|en|Block Alarm Unack",   "|en|Block Alarm not acknowledged by operator"                               }, \
    { 0x0100, "|en|Fail Alarm Unack",    "|en|Field Diagnostics Failed Alarm not acknowledged by operator"            }, \
    { 0x0200, "|en|OffSpec Alarm Unack", "|en|Field Diagnostics Off Specification Alarm not acknowledged by operator" }, \
    { 0x0400, "|en|Maint Alarm Unack",   "|en|Field Diagnostics Maintenance Alarm not acknowledged by operator"       }, \
    { 0x0800, "|en|Check Alarm Unack",   "|en|Field Diagnostics Check Function Alarm not acknowledged by operator"    }

#define FD_UNREP_SUMM_ENUM \
    { 0x0001, "|en|Discrete Alarm Unrep",   "|en|Write Alarm not reported"                               }, \
    { 0x0080, "|en|Block Alarm Unrep",   "|en|Block Alarm not reported"                               }, \
    { 0x0100, "|en|Fail Alarm Unrep",    "|en|Field Diagnostics Failed Alarm not reported"            }, \
    { 0x0200, "|en|OffSpec Alarm Unrep", "|en|Field Diagnostics Off Specification Alarm not reported" }, \
    { 0x0400, "|en|Maint Alarm Unrep",   "|en|Field Diagnostics Maintenance Alarm not reported"       }, \
    { 0x0800, "|en|Check Alarm Unrep",   "|en|Field Diagnostics Check Function Alarm not reported"    }

#define FD_DISAB_SUMM_ENUM \
    { 0x0001, "|en|Discrete Alarm Disabled",   "|en|Write Alarm disabled"                               }, \
    { 0x0080, "|en|Block Alarm Disabled",   "|en|Block Alarm disabled"                               }, \
    { 0x0100, "|en|Fail Alarm Disabled",    "|en|Field Diagnostics Failed Alarm disabled"            }, \
    { 0x0200, "|en|OffSpec Alarm Disabled", "|en|Field Diagnostics Off Specification Alarm disabled" }, \
    { 0x0400, "|en|Maint Alarm Disabled",   "|en|Field Diagnostics Maintenance Alarm disabled"       }, \
    { 0x0800, "|en|Check Alarm Disabled",   "|en|Field Diagnostics Check Function Alarm disabled"    }

#define ACK_OPTIONS_RES2 \
    { 0x0001, "|en|Write Alm AutoAck Enabled",   "|en|Write Alarm automatic acknowledge enabled"                               }, \
    { 0x0080, "|en|Block Alm AutoAck Enabled",   "|en|Block Alarm automatic acknowledge enabled"                               }, \
    { 0x0100, "|en|Fail Alm AutoAck Enabled",    "|en|Field Diagnostics Failed Alarm automatic acknowledge enabled"            }, \
    { 0x0200, "|en|OffSpec Alm AutoAck Enabled", "|en|Field Diagnostics Off Specification Alarm automatic acknowledge enabled" }, \
    { 0x0400, "|en|Maint Alm AutoAck Enabled",   "|en|Field Diagnostics Maintenance Alarm automatic acknowledge enabled"       }, \
    { 0x0800, "|en|Check Alm AutoAck Enabled",   "|en|Field Diagnostics Check Function Alarm automatic acknowledge enabled"    }

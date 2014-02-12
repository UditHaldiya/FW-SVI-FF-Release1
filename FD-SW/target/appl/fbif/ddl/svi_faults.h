#ifndef SVI_FAULTS_H_2013_04_03_16_06_41_GE_300010777
#define SVI_FAULTS_H_2013_04_03_16_06_41_GE_300010777
#define TS_FAULTS_H "Time-stamp: <2013-10-30 09:45:19 GE-300010777>"
/* Doc: [[file:c:/GE/Firmware-SVI II AP FF/FIRMWARE/FFAPmncb_FAULT_matrix.xls][FFAPmncb_FAULT_matrix.xls CH32690]] */
/*=====================================================================/
* define bits orders ( reversed for svi2 FF)
/=====================================================================*/
#define FBIT0 0x80
#define FBIT1 0x40
#define FBIT2 0x20
#define FBIT3 0x10
#define FBIT4 0x08
#define FBIT5 0x04
#define FBIT6 0x02
#define FBIT7 0x01

/*=====================================================================/
* define faults for groups
/=====================================================================*/
#define FAULT_GROUP_0_0 "|en|LOGONLY"
#define FAULT_GROUP_0_1 "|en|Undefined"
#define FAULT_GROUP_0_2 "|en|ANNUNCIATE"
#define FAULT_GROUP_0_3 "|en|ANNUNCIATE"
#define FAULT_GROUP_0_4 "|en|ANNUNCIATE"
#define FAULT_GROUP_0_5 "|en|FAILSAFE"
#define FAULT_GROUP_0_6 "|en|LOGONLY"
#define FAULT_GROUP_0_7 "|en|Undefined"
#define FAULT_GROUP_1_0 "|en|LOGONLY"
#define FAULT_GROUP_1_1 "|en|LOGONLY"
#define FAULT_GROUP_1_2 "|en|LOGONLY"
#define FAULT_GROUP_1_3 "|en|Undefined"
#define FAULT_GROUP_1_4 "|en|LOGONLY"
#define FAULT_GROUP_1_5 "|en|LOGONLY"
#define FAULT_GROUP_1_6 "|en|Undefined"
#define FAULT_GROUP_1_7 "|en|Undefined"
#define FAULT_GROUP_2_0 "|en|ANNUNCIATE"
#define FAULT_GROUP_2_1 "|en|LOGONLY"
#define FAULT_GROUP_2_2 "|en|LOGONLY"
#define FAULT_GROUP_2_3 "|en|Undefined"
#define FAULT_GROUP_2_4 "|en|Undefined"
#define FAULT_GROUP_2_5 "|en|Undefined"
#define FAULT_GROUP_2_6 "|en|LOGONLY"
#define FAULT_GROUP_2_7 "|en|Undefined"
#define FAULT_GROUP_3_0 "|en|FAILSAFE"
#define FAULT_GROUP_3_1 "|en|ANNUNCIATE"
#define FAULT_GROUP_3_2 "|en|FAILSAFE"
#define FAULT_GROUP_3_3 "|en|ANNUNCIATE"
#define FAULT_GROUP_3_4 "|en|FAILSAFE"
#define FAULT_GROUP_3_5 "|en|ANNUNCIATE"
#define FAULT_GROUP_3_6 "|en|Undefined"
#define FAULT_GROUP_3_7 "|en|LOGONLY"
#define FAULT_GROUP_4_0 "|en|FAILSAFE"
#define FAULT_GROUP_4_1 "|en|FAILSAFE"
#define FAULT_GROUP_4_2 "|en|Undefined"
#define FAULT_GROUP_4_3 "|en|FAILSAFE"
#define FAULT_GROUP_4_4 "|en|LOGONLY"
#define FAULT_GROUP_4_5 "|en|ANNUNCIATE"
#define FAULT_GROUP_4_6 "|en|ANNUNCIATE"
#define FAULT_GROUP_4_7 "|en|ANNUNCIATE"
#define FAULT_GROUP_5_0 "|en|FAILSAFE"
#define FAULT_GROUP_5_1 "|en|ANNUNCIATE"
#define FAULT_GROUP_5_2 "|en|ANNUNCIATE"
#define FAULT_GROUP_5_3 "|en|LOGONLY"
#define FAULT_GROUP_5_4 "|en|LOGONLY"
#define FAULT_GROUP_5_5 "|en|ANNUNCIATE"
#define FAULT_GROUP_5_6 "|en|FAILSAFE"
#define FAULT_GROUP_5_7 "|en|ANNUNCIATE"
#define FAULT_GROUP_6_0 "|en|Undefined"
#define FAULT_GROUP_6_1 "|en|Undefined"
#define FAULT_GROUP_6_2 "|en|Undefined"
#define FAULT_GROUP_6_3 "|en|Undefined"
#define FAULT_GROUP_6_4 "|en|Undefined"
#define FAULT_GROUP_6_5 "|en|Undefined"
#define FAULT_GROUP_6_6 "|en|Undefined"
#define FAULT_GROUP_6_7 "|en|Undefined"
#define FAULT_GROUP_7_0 "|en|Undefined"
#define FAULT_GROUP_7_1 "|en|Undefined"
#define FAULT_GROUP_7_2 "|en|Undefined"
#define FAULT_GROUP_7_3 "|en|Undefined"
#define FAULT_GROUP_7_4 "|en|LOGONLY"
#define FAULT_GROUP_7_5 "|en|Undefined"
#define FAULT_GROUP_7_6 "|en|Undefined"
#define FAULT_GROUP_7_7 "|en|ANNUNCIATE"
#define FAULT_GROUP_8_0 "|en|FAILSAFE"
#define FAULT_GROUP_8_1 "|en|LOGONLY"
#define FAULT_GROUP_8_2 "|en|LOGONLY"
#define FAULT_GROUP_8_3 "|en|LOGONLY"
#define FAULT_GROUP_8_4 "|en|LOGONLY"
#define FAULT_GROUP_8_5 "|en|LOGONLY"
#define FAULT_GROUP_8_6 "|en|LOGONLY"
#define FAULT_GROUP_8_7 "|en|"
/*=====================================================================/
* define each fault bits
/=====================================================================*/
#define FAULT_NAME_0_0 "|en|RESET"
#define FAULT_NAME_0_1 "|en|Undefined"
#define FAULT_NAME_0_2 "|en|ACTUATOR"
#define FAULT_NAME_0_3 "|en|AIR_SUPPLY_LOW"
#define FAULT_NAME_0_4 "|en|POSITION_ERROR"
#define FAULT_NAME_0_5 "|en|I2CBUS"
#define FAULT_NAME_0_6 "|en|KEYPAD"
#define FAULT_NAME_0_7 "|en|Undefined"
#define FAULT_NAME_1_0 "|en|CALIBRATION_FAILED"
#define FAULT_NAME_1_1 "|en|FIND_STOPS_FAILED"
#define FAULT_NAME_1_2 "|en|AUTOTUNE_FAILED"
#define FAULT_NAME_1_3 "|en|Undefined"
#define FAULT_NAME_1_4 "|en|EXT_DIAGNOSTICS_FAILED"
#define FAULT_NAME_1_5 "|en|RTOS_SCHEDULING"
#define FAULT_NAME_1_6 "|en|Undefined"
#define FAULT_NAME_1_7 "|en|Undefined"
#define FAULT_NAME_2_0 "|en|BIAS_OUT_OF_RANGE"
#define FAULT_NAME_2_1 "|en|IP_OUT_OF_RANGE"
#define FAULT_NAME_2_2 "|en|TEMPR_OUT_OF_RANGE"
#define FAULT_NAME_2_3 "|en|Undefined"
#define FAULT_NAME_2_4 "|en|Undefined"
#define FAULT_NAME_2_5 "|en|Undefined"
#define FAULT_NAME_2_6 "|en|POS_CUTOFF_LO"
#define FAULT_NAME_2_7 "|en|Undefined"
#define FAULT_NAME_3_0 "|en|NVM_CHECKSUM0"
#define FAULT_NAME_3_1 "|en|RAM_CHECKSUM"
#define FAULT_NAME_3_2 "|en|FW_CHECKSUM"
#define FAULT_NAME_3_3 "|en|STACK"
#define FAULT_NAME_3_4 "|en|FACTORYWRITE"
#define FAULT_NAME_3_5 "|en|NVM_TEST"
#define FAULT_NAME_3_6 "|en|Undefined"
#define FAULT_NAME_3_7 "|en|FACTORYMODE"
#define FAULT_NAME_4_0 "|en|REF_VOLTAGE"
#define FAULT_NAME_4_1 "|en|POS_SNSR_OUT_OF_RANGE"
#define FAULT_NAME_4_2 "|en|Undefined"
#define FAULT_NAME_4_3 "|en|TEMPERATURE_SENSOR"
#define FAULT_NAME_4_4 "|en|CURRENT_SENSOR_2ND"
#define FAULT_NAME_4_5 "|en|PRESSURE1"
#define FAULT_NAME_4_6 "|en|PRESSURE2"
#define FAULT_NAME_4_7 "|en|PRESSURE3"
#define FAULT_NAME_5_0 "|en|PRESSURE4"
#define FAULT_NAME_5_1 "|en|PRESSURE5"
#define FAULT_NAME_5_2 "|en|WATCHDOG_TIMEOUT"
#define FAULT_NAME_5_3 "|en|NVM_WRITE"
#define FAULT_NAME_5_4 "|en|IRQ_FAULT"
#define FAULT_NAME_5_5 "|en|TMOUT_FLASH_TEST"
#define FAULT_NAME_5_6 "|en|SELF_CHECK"
#define FAULT_NAME_5_7 "|en|SOFTWARE"
#define FAULT_NAME_6_0 "|en|Undefined"
#define FAULT_NAME_6_1 "|en|Undefined"
#define FAULT_NAME_6_2 "|en|Undefined"
#define FAULT_NAME_6_3 "|en|Undefined"
#define FAULT_NAME_6_4 "|en|Undefined"
#define FAULT_NAME_6_5 "|en|Undefined"
#define FAULT_NAME_6_6 "|en|Undefined"
#define FAULT_NAME_6_7 "|en|Undefined"
#define FAULT_NAME_7_0 "|en|Undefined"
#define FAULT_NAME_7_1 "|en|Undefined"
#define FAULT_NAME_7_2 "|en|Undefined"
#define FAULT_NAME_7_3 "|en|Undefined"
#define FAULT_NAME_7_4 "|en|UI_OFF"
#define FAULT_NAME_7_5 "|en|Undefined"
#define FAULT_NAME_7_6 "|en|Undefined"
#define FAULT_NAME_7_7 "|en|SETPOINT_TIMEOUT"
#define FAULT_NAME_8_0 "|en|FSTATE_REQ"
#define FAULT_NAME_8_1 "|en|NEWDIAGSIGN_BASELINE"
#define FAULT_NAME_8_2 "|en|NEWDIAGSIGN_USER"
#define FAULT_NAME_8_3 "|en|IPC_DISCONNECT"
#define FAULT_NAME_8_4 "|en|POS_CUTOFF_HI"
#define FAULT_NAME_8_5 "|en|NEWDIAGSIGN_CURRENT"
#define FAULT_NAME_8_6 "|en|NV_BK_RESTORE"
#define FAULT_NAME_8_7 "|en|"

/*=====================================================================/
* define enumerations for fault bytes.
/=====================================================================*/

#define FAULTS_ENUMS(byte)  \
{ FBIT0, FAULT_NAME_#byte#_0 } \
{ FBIT1, FAULT_NAME_#byte#_1 } \
{ FBIT2, FAULT_NAME_#byte#_2 } \
{ FBIT3, FAULT_NAME_#byte#_3 } \
{ FBIT4, FAULT_NAME_#byte#_4 } \
{ FBIT5, FAULT_NAME_#byte#_5 } \
{ FBIT6, FAULT_NAME_#byte#_6 } \
{ FBIT7, FAULT_NAME_#byte#_7 } \

#define FAULTS_ENUMS_TBD \
{ FBIT0, "|en|TBD." } \
{ FBIT1, "|en|TBD." } \
{ FBIT2, "|en|TBD." } \
{ FBIT3, "|en|TBD." } \
{ FBIT4, "|en|TBD." } \
{ FBIT5, "|en|TBD." } \
{ FBIT6, "|en|TBD." } \
{ FBIT7, "|en|TBD." } \


#endif /* SVI_FAULTS_H_2013_04_03_16_06_41_GE_300010777 */
